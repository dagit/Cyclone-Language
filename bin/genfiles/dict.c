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
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 252 "cycboot.h"
extern int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
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
return({struct Cyc_Dict_Dict _tmpD2;_tmpD2.rel=comp,_tmpD2.r=r,_tmpD2.t=0;_tmpD2;});}
# 43
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 46
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){
# 48
return({struct Cyc_Dict_Dict _tmpD3;_tmpD3.rel=comp,_tmpD3.r=r,({const struct Cyc_Dict_T*_tmpED=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->color=Cyc_Dict_B,_tmp0->left=0,_tmp0->right=0,(_tmp0->key_val).f1=key,(_tmp0->key_val).f2=data;_tmp0;});_tmpD3.t=_tmpED;});_tmpD3;});}
# 50
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}
# 54
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
const struct Cyc_Dict_T*t2=t;
return({struct Cyc_Dict_Dict _tmpD4;_tmpD4.rel=d.rel,_tmpD4.r=r,_tmpD4.t=t2;_tmpD4;});}
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
struct _tuple1 _tmpF=quad;struct _tuple0 _tmp13;const struct Cyc_Dict_T*_tmp12;const struct Cyc_Dict_T*_tmp11;enum Cyc_Dict_Color _tmp10;struct _tuple0 _tmp1A;struct _tuple0 _tmp19;struct _tuple0 _tmp18;const struct Cyc_Dict_T*_tmp17;const struct Cyc_Dict_T*_tmp16;const struct Cyc_Dict_T*_tmp15;const struct Cyc_Dict_T*_tmp14;struct _tuple0 _tmp21;struct _tuple0 _tmp20;const struct Cyc_Dict_T*_tmp1F;struct _tuple0 _tmp1E;const struct Cyc_Dict_T*_tmp1D;const struct Cyc_Dict_T*_tmp1C;const struct Cyc_Dict_T*_tmp1B;struct _tuple0 _tmp28;const struct Cyc_Dict_T*_tmp27;struct _tuple0 _tmp26;struct _tuple0 _tmp25;const struct Cyc_Dict_T*_tmp24;const struct Cyc_Dict_T*_tmp23;const struct Cyc_Dict_T*_tmp22;struct _tuple0 _tmp2F;const struct Cyc_Dict_T*_tmp2E;struct _tuple0 _tmp2D;const struct Cyc_Dict_T*_tmp2C;struct _tuple0 _tmp2B;const struct Cyc_Dict_T*_tmp2A;const struct Cyc_Dict_T*_tmp29;if(_tmpF.f1 == Cyc_Dict_B){if(_tmpF.f2 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f2)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f2)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->left)->color == Cyc_Dict_R){_LL1: _tmp29=((_tmpF.f2)->left)->left;_tmp2A=((_tmpF.f2)->left)->right;_tmp2B=((_tmpF.f2)->left)->key_val;_tmp2C=(_tmpF.f2)->right;_tmp2D=(_tmpF.f2)->key_val;_tmp2E=_tmpF.f3;_tmp2F=_tmpF.f4;_LL2: {const struct Cyc_Dict_T*a=_tmp29;const struct Cyc_Dict_T*b=_tmp2A;struct _tuple0 x=_tmp2B;const struct Cyc_Dict_T*c=_tmp2C;struct _tuple0 y=_tmp2D;const struct Cyc_Dict_T*d=_tmp2E;struct _tuple0 z=_tmp2F;
# 131
return({struct Cyc_Dict_T*_tmp32=_region_malloc(r,sizeof(*_tmp32));_tmp32->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpEF=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp30=_region_malloc(r,sizeof(*_tmp30));_tmp30->color=Cyc_Dict_B,_tmp30->left=a,_tmp30->right=b,_tmp30->key_val=x;_tmp30;});_tmp32->left=_tmpEF;}),({const struct Cyc_Dict_T*_tmpEE=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp31=_region_malloc(r,sizeof(*_tmp31));_tmp31->color=Cyc_Dict_B,_tmp31->left=c,_tmp31->right=d,_tmp31->key_val=z;_tmp31;});_tmp32->right=_tmpEE;}),_tmp32->key_val=y;_tmp32;});}}else{if(((const struct Cyc_Dict_T*)_tmpF.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->right)->color == Cyc_Dict_R)goto _LL3;else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->right)->color == Cyc_Dict_R){_LL3: _tmp22=(_tmpF.f2)->left;_tmp23=((_tmpF.f2)->right)->left;_tmp24=((_tmpF.f2)->right)->right;_tmp25=((_tmpF.f2)->right)->key_val;_tmp26=(_tmpF.f2)->key_val;_tmp27=_tmpF.f3;_tmp28=_tmpF.f4;_LL4: {const struct Cyc_Dict_T*a=_tmp22;const struct Cyc_Dict_T*b=_tmp23;const struct Cyc_Dict_T*c=_tmp24;struct _tuple0 y=_tmp25;struct _tuple0 x=_tmp26;const struct Cyc_Dict_T*d=_tmp27;struct _tuple0 z=_tmp28;
# 133
return({struct Cyc_Dict_T*_tmp35=_region_malloc(r,sizeof(*_tmp35));_tmp35->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF1=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp33=_region_malloc(r,sizeof(*_tmp33));_tmp33->color=Cyc_Dict_B,_tmp33->left=a,_tmp33->right=b,_tmp33->key_val=x;_tmp33;});_tmp35->left=_tmpF1;}),({const struct Cyc_Dict_T*_tmpF0=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp34=_region_malloc(r,sizeof(*_tmp34));_tmp34->color=Cyc_Dict_B,_tmp34->left=c,_tmp34->right=d,_tmp34->key_val=z;_tmp34;});_tmp35->right=_tmpF0;}),_tmp35->key_val=y;_tmp35;});}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R){_LL5: _tmp1B=_tmpF.f2;_tmp1C=((_tmpF.f3)->left)->left;_tmp1D=((_tmpF.f3)->left)->right;_tmp1E=((_tmpF.f3)->left)->key_val;_tmp1F=(_tmpF.f3)->right;_tmp20=(_tmpF.f3)->key_val;_tmp21=_tmpF.f4;_LL6: {const struct Cyc_Dict_T*a=_tmp1B;const struct Cyc_Dict_T*b=_tmp1C;const struct Cyc_Dict_T*c=_tmp1D;struct _tuple0 y=_tmp1E;const struct Cyc_Dict_T*d=_tmp1F;struct _tuple0 z=_tmp20;struct _tuple0 x=_tmp21;
# 135
return({struct Cyc_Dict_T*_tmp38=_region_malloc(r,sizeof(*_tmp38));_tmp38->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF3=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp36=_region_malloc(r,sizeof(*_tmp36));_tmp36->color=Cyc_Dict_B,_tmp36->left=a,_tmp36->right=b,_tmp36->key_val=x;_tmp36;});_tmp38->left=_tmpF3;}),({const struct Cyc_Dict_T*_tmpF2=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp37=_region_malloc(r,sizeof(*_tmp37));_tmp37->color=Cyc_Dict_B,_tmp37->left=c,_tmp37->right=d,_tmp37->key_val=z;_tmp37;});_tmp38->right=_tmpF2;}),_tmp38->key_val=y;_tmp38;});}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R){_LL7: _tmp14=_tmpF.f2;_tmp15=(_tmpF.f3)->left;_tmp16=((_tmpF.f3)->right)->left;_tmp17=((_tmpF.f3)->right)->right;_tmp18=((_tmpF.f3)->right)->key_val;_tmp19=(_tmpF.f3)->key_val;_tmp1A=_tmpF.f4;_LL8: {const struct Cyc_Dict_T*a=_tmp14;const struct Cyc_Dict_T*b=_tmp15;const struct Cyc_Dict_T*c=_tmp16;const struct Cyc_Dict_T*d=_tmp17;struct _tuple0 z=_tmp18;struct _tuple0 y=_tmp19;struct _tuple0 x=_tmp1A;
# 137
return({struct Cyc_Dict_T*_tmp3B=_region_malloc(r,sizeof(*_tmp3B));_tmp3B->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF5=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp39=_region_malloc(r,sizeof(*_tmp39));_tmp39->color=Cyc_Dict_B,_tmp39->left=a,_tmp39->right=b,_tmp39->key_val=x;_tmp39;});_tmp3B->left=_tmpF5;}),({const struct Cyc_Dict_T*_tmpF4=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp3A=_region_malloc(r,sizeof(*_tmp3A));_tmp3A->color=Cyc_Dict_B,_tmp3A->left=c,_tmp3A->right=d,_tmp3A->key_val=z;_tmp3A;});_tmp3B->right=_tmpF4;}),_tmp3B->key_val=y;_tmp3B;});}}else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{_LL9: _tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;_tmp13=_tmpF.f4;_LLA: {enum Cyc_Dict_Color a=_tmp10;const struct Cyc_Dict_T*b=_tmp11;const struct Cyc_Dict_T*c=_tmp12;struct _tuple0 d=_tmp13;
# 139
return({struct Cyc_Dict_T*_tmp3C=_region_malloc(r,sizeof(*_tmp3C));_tmp3C->color=a,_tmp3C->left=b,_tmp3C->right=c,_tmp3C->key_val=d;_tmp3C;});}}_LL0:;}
# 143
static struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 145
const struct Cyc_Dict_T*_tmp3D=t;struct _tuple0 _tmp41;const struct Cyc_Dict_T*_tmp40;const struct Cyc_Dict_T*_tmp3F;enum Cyc_Dict_Color _tmp3E;if(_tmp3D == 0){_LL1: _LL2:
 return({struct Cyc_Dict_T*_tmp42=_region_malloc(r,sizeof(*_tmp42));_tmp42->color=Cyc_Dict_R,_tmp42->left=0,_tmp42->right=0,_tmp42->key_val=key_val;_tmp42;});}else{_LL3: _tmp3E=_tmp3D->color;_tmp3F=_tmp3D->left;_tmp40=_tmp3D->right;_tmp41=_tmp3D->key_val;_LL4: {enum Cyc_Dict_Color color=_tmp3E;const struct Cyc_Dict_T*a=_tmp3F;const struct Cyc_Dict_T*b=_tmp40;struct _tuple0 y=_tmp41;
# 148
int _tmp43=rel(key_val.f1,y.f1);int i=_tmp43;
if(i < 0)return({struct _RegionHandle*_tmpF7=r;Cyc_Dict_balance(_tmpF7,({struct _tuple1 _tmpD5;_tmpD5.f1=color,({const struct Cyc_Dict_T*_tmpF6=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,a);_tmpD5.f2=_tmpF6;}),_tmpD5.f3=b,_tmpD5.f4=y;_tmpD5;}));});
if(i > 0)return({struct _RegionHandle*_tmpF9=r;Cyc_Dict_balance(_tmpF9,({struct _tuple1 _tmpD6;_tmpD6.f1=color,_tmpD6.f2=a,({const struct Cyc_Dict_T*_tmpF8=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,b);_tmpD6.f3=_tmpF8;}),_tmpD6.f4=y;_tmpD6;}));});
return({struct Cyc_Dict_T*_tmp44=_region_malloc(r,sizeof(*_tmp44));_tmp44->color=color,_tmp44->left=a,_tmp44->right=b,_tmp44->key_val=key_val;_tmp44;});}}_LL0:;}
# 155
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
struct Cyc_Dict_T*_tmp45=({struct _RegionHandle*_tmpFC=d.r;int(*_tmpFB)(void*,void*)=d.rel;struct _tuple0 _tmpFA=({struct _tuple0 _tmpD8;_tmpD8.f1=key,_tmpD8.f2=data;_tmpD8;});Cyc_Dict_ins(_tmpFC,_tmpFB,_tmpFA,d.t);});struct Cyc_Dict_T*ans=_tmp45;
((struct Cyc_Dict_T*)_check_null(ans))->color=Cyc_Dict_B;
return({struct Cyc_Dict_Dict _tmpD7;_tmpD7.rel=d.rel,_tmpD7.r=d.r,_tmpD7.t=(const struct Cyc_Dict_T*)ans;_tmpD7;});}
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
const struct Cyc_Dict_T*_tmp46=t;void*_tmp4A;void*_tmp49;const struct Cyc_Dict_T*_tmp48;const struct Cyc_Dict_T*_tmp47;_LL1: _tmp47=_tmp46->left;_tmp48=_tmp46->right;_tmp49=(_tmp46->key_val).f1;_tmp4A=(_tmp46->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp47;const struct Cyc_Dict_T*right=_tmp48;void*key=_tmp49;void*val=_tmp4A;
if(left != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,left,accum);
accum=f(key,val,accum);
if(right != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,right,accum);
return accum;}}
# 183
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp4B=d.t;const struct Cyc_Dict_T*t=_tmp4B;
if(t == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree(f,t,accum);}
# 190
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){
# 192
const struct Cyc_Dict_T*_tmp4C=t;void*_tmp50;void*_tmp4F;const struct Cyc_Dict_T*_tmp4E;const struct Cyc_Dict_T*_tmp4D;_LL1: _tmp4D=_tmp4C->left;_tmp4E=_tmp4C->right;_tmp4F=(_tmp4C->key_val).f1;_tmp50=(_tmp4C->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp4D;const struct Cyc_Dict_T*right=_tmp4E;void*key=_tmp4F;void*val=_tmp50;
if(left != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,left,accum);
accum=f(env,key,val,accum);
if(right != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,right,accum);
return accum;}}
# 201
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp51=d.t;const struct Cyc_Dict_T*t=_tmp51;
if(t == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree_c(f,env,t,accum);}
# 208
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp52=t;void*_tmp56;void*_tmp55;const struct Cyc_Dict_T*_tmp54;const struct Cyc_Dict_T*_tmp53;_LL1: _tmp53=_tmp52->left;_tmp54=_tmp52->right;_tmp55=(_tmp52->key_val).f1;_tmp56=(_tmp52->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp53;const struct Cyc_Dict_T*right=_tmp54;void*key=_tmp55;void*val=_tmp56;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,left);
f(key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,right);}}
# 215
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp57=d.t;const struct Cyc_Dict_T*t=_tmp57;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree(f,t);}
# 221
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp58=t;void*_tmp5C;void*_tmp5B;const struct Cyc_Dict_T*_tmp5A;const struct Cyc_Dict_T*_tmp59;_LL1: _tmp59=_tmp58->left;_tmp5A=_tmp58->right;_tmp5B=(_tmp58->key_val).f1;_tmp5C=(_tmp58->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp59;const struct Cyc_Dict_T*right=_tmp5A;void*key=_tmp5B;void*val=_tmp5C;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,left);
f(env,key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,right);}}
# 228
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp5D=d.t;const struct Cyc_Dict_T*t=_tmp5D;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree_c(f,env,t);}
# 234
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp5E=t;void*_tmp62;void*_tmp61;const struct Cyc_Dict_T*_tmp60;const struct Cyc_Dict_T*_tmp5F;_LL1: _tmp5F=_tmp5E->left;_tmp60=_tmp5E->right;_tmp61=(_tmp5E->key_val).f1;_tmp62=(_tmp5E->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp5F;const struct Cyc_Dict_T*right=_tmp60;void*key=_tmp61;void*val=_tmp62;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,left);
f(key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,right);}}
# 241
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp63=d.t;const struct Cyc_Dict_T*t=_tmp63;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree(f,t);}
# 247
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp64=t;void*_tmp68;void*_tmp67;const struct Cyc_Dict_T*_tmp66;const struct Cyc_Dict_T*_tmp65;_LL1: _tmp65=_tmp64->left;_tmp66=_tmp64->right;_tmp67=(_tmp64->key_val).f1;_tmp68=(_tmp64->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp65;const struct Cyc_Dict_T*right=_tmp66;void*key=_tmp67;void*val=_tmp68;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,left);
f(env,key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,right);}}
# 254
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp69=d.t;const struct Cyc_Dict_T*t=_tmp69;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree_c(f,env,t);}
# 260
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
*cnt=*cnt + 1;}
# 263
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
int num=0;
((void(*)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Dict_count_elem,& num,d);
return num;}struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};
# 269
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 271
struct _tuple2*_tmp6A=env;struct Cyc_Dict_Dict _tmp6C;void(*_tmp6B)(void*,void*);_LL1: _tmp6B=_tmp6A->f1;_tmp6C=_tmp6A->f2;_LL2: {void(*f)(void*,void*)=_tmp6B;struct Cyc_Dict_Dict d2=_tmp6C;
({void(*_tmpFE)(void*,void*)=f;void*_tmpFD=b1;_tmpFE(_tmpFD,Cyc_Dict_lookup(d2,a));});}}
# 275
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 278
struct _tuple2 _tmp6D=({struct _tuple2 _tmpD9;_tmpD9.f1=f,_tmpD9.f2=d2;_tmpD9;});struct _tuple2 env=_tmp6D;
((void(*)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,& env,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 282
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 284
struct _tuple3*_tmp6E=env;void*_tmp71;struct Cyc_Dict_Dict _tmp70;void(*_tmp6F)(void*,void*,void*);_LL1: _tmp6F=_tmp6E->f1;_tmp70=_tmp6E->f2;_tmp71=_tmp6E->f3;_LL2: {void(*f)(void*,void*,void*)=_tmp6F;struct Cyc_Dict_Dict d2=_tmp70;void*inner_env=_tmp71;
({void(*_tmp101)(void*,void*,void*)=f;void*_tmp100=inner_env;void*_tmpFF=b1;_tmp101(_tmp100,_tmpFF,Cyc_Dict_lookup(d2,a));});}}
# 288
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 291
struct _tuple3 _tmp72=({struct _tuple3 _tmpDA;_tmpDA.f1=f,_tmpDA.f2=d2,_tmpDA.f3=inner_env;_tmpDA;});struct _tuple3 env=_tmp72;
((void(*)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,& env,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 295
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 299
struct _tuple4*_tmp73=env;void*_tmp76;struct Cyc_Dict_Dict _tmp75;void*(*_tmp74)(void*,void*,void*,void*,void*);_LL1: _tmp74=_tmp73->f1;_tmp75=_tmp73->f2;_tmp76=_tmp73->f3;_LL2: {void*(*f)(void*,void*,void*,void*,void*)=_tmp74;struct Cyc_Dict_Dict d2=_tmp75;void*inner_env=_tmp76;
return({void*(*_tmp106)(void*,void*,void*,void*,void*)=f;void*_tmp105=inner_env;void*_tmp104=a;void*_tmp103=b1;void*_tmp102=Cyc_Dict_lookup(d2,a);_tmp106(_tmp105,_tmp104,_tmp103,_tmp102,accum);});}}
# 303
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 307
struct _tuple4 _tmp77=({struct _tuple4 _tmpDB;_tmpDB.f1=f,_tmpDB.f2=d2,_tmpDB.f3=inner_env;_tmpDB;});struct _tuple4 env=_tmp77;
return((void*(*)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,& env,d1,accum);}
# 311
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){
if(t == (const struct Cyc_Dict_T*)0)return 0;else{
# 314
struct Cyc_Dict_T _tmp78=*t;struct Cyc_Dict_T _stmttmp0=_tmp78;struct Cyc_Dict_T _tmp79=_stmttmp0;struct _tuple0 _tmp7D;const struct Cyc_Dict_T*_tmp7C;const struct Cyc_Dict_T*_tmp7B;enum Cyc_Dict_Color _tmp7A;_LL1: _tmp7A=_tmp79.color;_tmp7B=_tmp79.left;_tmp7C=_tmp79.right;_tmp7D=_tmp79.key_val;_LL2: {enum Cyc_Dict_Color c=_tmp7A;const struct Cyc_Dict_T*left=_tmp7B;const struct Cyc_Dict_T*right=_tmp7C;struct _tuple0 pr=_tmp7D;
const struct Cyc_Dict_T*_tmp7E=Cyc_Dict_copy_tree(r2,left);const struct Cyc_Dict_T*new_left=_tmp7E;
const struct Cyc_Dict_T*_tmp7F=Cyc_Dict_copy_tree(r2,right);const struct Cyc_Dict_T*new_right=_tmp7F;
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp80=_region_malloc(r2,sizeof(*_tmp80));_tmp80->color=c,_tmp80->left=new_left,_tmp80->right=new_right,_tmp80->key_val=pr;_tmp80;});}}}
# 321
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
return({struct Cyc_Dict_Dict _tmpDC;_tmpDC.rel=d.rel,_tmpDC.r=r2,({const struct Cyc_Dict_T*_tmp107=Cyc_Dict_copy_tree(r2,d.t);_tmpDC.t=_tmp107;});_tmpDC;});}
# 325
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 329
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){
# 331
const struct Cyc_Dict_T*_tmp81=t;void*_tmp86;void*_tmp85;const struct Cyc_Dict_T*_tmp84;const struct Cyc_Dict_T*_tmp83;enum Cyc_Dict_Color _tmp82;_LL1: _tmp82=_tmp81->color;_tmp83=_tmp81->left;_tmp84=_tmp81->right;_tmp85=(_tmp81->key_val).f1;_tmp86=(_tmp81->key_val).f2;_LL2: {enum Cyc_Dict_Color c=_tmp82;const struct Cyc_Dict_T*left=_tmp83;const struct Cyc_Dict_T*right=_tmp84;void*key=_tmp85;void*val=_tmp86;
const struct Cyc_Dict_T*_tmp87=left == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,left);const struct Cyc_Dict_T*new_left=_tmp87;
void*_tmp88=f(val);void*new_val=_tmp88;
const struct Cyc_Dict_T*_tmp89=right == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,right);const struct Cyc_Dict_T*new_right=_tmp89;
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp8A=_region_malloc(r,sizeof(*_tmp8A));_tmp8A->color=c,_tmp8A->left=new_left,_tmp8A->right=new_right,(_tmp8A->key_val).f1=key,(_tmp8A->key_val).f2=new_val;_tmp8A;});}}
# 338
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp8B=d.t;const struct Cyc_Dict_T*t=_tmp8B;
if(t == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpDD;_tmpDD.rel=d.rel,_tmpDD.r=r,_tmpDD.t=0;_tmpDD;});
return({struct Cyc_Dict_Dict _tmpDE;_tmpDE.rel=d.rel,_tmpDE.r=r,({const struct Cyc_Dict_T*_tmp108=Cyc_Dict_map_tree(r,f,t);_tmpDE.t=_tmp108;});_tmpDE;});}
# 345
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 349
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){
# 351
const struct Cyc_Dict_T*_tmp8C=t;void*_tmp91;void*_tmp90;const struct Cyc_Dict_T*_tmp8F;const struct Cyc_Dict_T*_tmp8E;enum Cyc_Dict_Color _tmp8D;_LL1: _tmp8D=_tmp8C->color;_tmp8E=_tmp8C->left;_tmp8F=_tmp8C->right;_tmp90=(_tmp8C->key_val).f1;_tmp91=(_tmp8C->key_val).f2;_LL2: {enum Cyc_Dict_Color c=_tmp8D;const struct Cyc_Dict_T*left=_tmp8E;const struct Cyc_Dict_T*right=_tmp8F;void*key=_tmp90;void*val=_tmp91;
const struct Cyc_Dict_T*_tmp92=
left == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,left);
# 352
const struct Cyc_Dict_T*new_left=_tmp92;
# 354
void*_tmp93=f(env,val);void*new_val=_tmp93;
const struct Cyc_Dict_T*_tmp94=
right == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,right);
# 355
const struct Cyc_Dict_T*new_right=_tmp94;
# 357
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp95=_region_malloc(r,sizeof(*_tmp95));_tmp95->color=c,_tmp95->left=new_left,_tmp95->right=new_right,(_tmp95->key_val).f1=key,(_tmp95->key_val).f2=new_val;_tmp95;});}}
# 360
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
# 362
const struct Cyc_Dict_T*_tmp96=d.t;const struct Cyc_Dict_T*t=_tmp96;
if(t == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpDF;_tmpDF.rel=d.rel,_tmpDF.r=r,_tmpDF.t=0;_tmpDF;});
return({struct Cyc_Dict_Dict _tmpE0;_tmpE0.rel=d.rel,_tmpE0.r=r,({const struct Cyc_Dict_T*_tmp109=Cyc_Dict_map_tree_c(r,f,env,t);_tmpE0.t=_tmp109;});_tmpE0;});}
# 368
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 372
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
if(d.t == (const struct Cyc_Dict_T*)0)
(int)_throw((void*)& Cyc_Dict_Absent_val);
return({struct _tuple0*_tmp97=_region_malloc(r,sizeof(*_tmp97));_tmp97->f1=((d.t)->key_val).f1,_tmp97->f2=((d.t)->key_val).f2;_tmp97;});}
# 378
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp98=t;void*_tmp9C;void*_tmp9B;const struct Cyc_Dict_T*_tmp9A;const struct Cyc_Dict_T*_tmp99;_LL1: _tmp99=_tmp98->left;_tmp9A=_tmp98->right;_tmp9B=(_tmp98->key_val).f1;_tmp9C=(_tmp98->key_val).f2;_LL2: {const struct Cyc_Dict_T*left=_tmp99;const struct Cyc_Dict_T*right=_tmp9A;void*key=_tmp9B;void*val=_tmp9C;
return
((left == (const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,left))&&
 f(env,key,val))&&(
right == (const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,right));}}
# 386
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp9D=d.t;const struct Cyc_Dict_T*t=_tmp9D;
if(t == (const struct Cyc_Dict_T*)0)
return 1;
return Cyc_Dict_forall_tree_c(f,env,t);}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};
# 393
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 395
struct _tuple5*_tmp9E=env;struct Cyc_Dict_Dict _tmpA0;int(*_tmp9F)(void*,void*,void*);_LL1: _tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;_LL2: {int(*f)(void*,void*,void*)=_tmp9F;struct Cyc_Dict_Dict d2=_tmpA0;
if(Cyc_Dict_member(d2,a))
return({int(*_tmp10C)(void*,void*,void*)=f;void*_tmp10B=a;void*_tmp10A=b;_tmp10C(_tmp10B,_tmp10A,Cyc_Dict_lookup(d2,a));});
return 1;}}
# 402
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
struct _tuple5 _tmpA1=({struct _tuple5 _tmpE1;_tmpE1.f1=f,_tmpE1.f2=d2;_tmpE1;});struct _tuple5 env=_tmpA1;
return((int(*)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& env,d1);}struct _tuple6{void*(*f1)(void*,void*,void*,void*);void*f2;};
# 408
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){
# 412
if(Cyc_Dict_member(*d1,a)){
# 414
void*_tmpA2=Cyc_Dict_lookup(*d1,a);void*old_val=_tmpA2;
void*_tmpA3=(((*env).f1))((*env).f2,a,old_val,b);void*new_val=_tmpA3;
if(new_val != old_val)
({struct Cyc_Dict_Dict _tmp10D=Cyc_Dict_insert(*d1,a,new_val);*d1=_tmp10D;});
return d1;}
# 420
({struct Cyc_Dict_Dict _tmp10E=Cyc_Dict_insert(*d1,a,b);*d1=_tmp10E;});
return d1;}
# 424
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 427
if((int)d1.t == (int)d2.t)return d1;
if(d1.t == (const struct Cyc_Dict_T*)0)return d2;
if(d2.t == (const struct Cyc_Dict_T*)0)return d1;{
struct _tuple6 _tmpA4=({struct _tuple6 _tmpE2;_tmpE2.f1=f,_tmpE2.f2=env;_tmpE2;});struct _tuple6 fenvpair=_tmpA4;
((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c)(Cyc_Dict_union_f,& fenvpair,d2,& d1);
return d1;}}
# 435
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 438
const struct Cyc_Dict_T*_tmpA5=d2.t;const struct Cyc_Dict_T*t2=_tmpA5;
if(t2 == (const struct Cyc_Dict_T*)0)return d2;
if((int)d1.t == (int)t2)return d2;{
const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _tmpA6=_new_region("temp");struct _RegionHandle*temp=& _tmpA6;_push_region(temp);{
# 444
struct _fat_ptr queue=_tag_fat(({unsigned _tmpB0=16U;const struct Cyc_Dict_T**_tmpAF=({struct _RegionHandle*_tmp10F=temp;_region_malloc(_tmp10F,_check_times(_tmpB0,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _tmpE5=16U;unsigned i;for(i=0;i < _tmpE5;++ i){_tmpAF[i]=t2;}}0;});_tmpAF;}),sizeof(const struct Cyc_Dict_T*),16U);
int ind=0;
while(ind != -1){
const struct Cyc_Dict_T*_tmpA7=*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),ind --));const struct Cyc_Dict_T*_stmttmp1=_tmpA7;const struct Cyc_Dict_T*_tmpA8=_stmttmp1;void*_tmpAC;void*_tmpAB;const struct Cyc_Dict_T*_tmpAA;const struct Cyc_Dict_T*_tmpA9;_LL1: _tmpA9=_tmpA8->left;_tmpAA=_tmpA8->right;_tmpAB=(_tmpA8->key_val).f1;_tmpAC=(_tmpA8->key_val).f2;_LL2: {const struct Cyc_Dict_T*l=_tmpA9;const struct Cyc_Dict_T*r=_tmpAA;void*k=_tmpAB;void*v=_tmpAC;
if((unsigned)(ind + 2)>= _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*)))
queue=({unsigned _tmpAE=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* (unsigned)2;const struct Cyc_Dict_T**_tmpAD=({struct _RegionHandle*_tmp110=temp;_region_malloc(_tmp110,_check_times(_tmpAE,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _tmpE3=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* (unsigned)2;unsigned i;for(i=0;i < _tmpE3;++ i){
i < _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))?_tmpAD[i]=((const struct Cyc_Dict_T**)queue.curr)[(int)i]:(_tmpAD[i]=t2);}}0;});_tag_fat(_tmpAD,sizeof(const struct Cyc_Dict_T*),_tmpAE);});
if(l != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=l;
if(r != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=r;
if(Cyc_Dict_member(d1,k))
ans_tree=(const struct Cyc_Dict_T*)({struct _RegionHandle*_tmp118=d2.r;int(*_tmp117)(void*,void*)=d2.rel;struct _tuple0 _tmp116=({struct _tuple0 _tmpE4;_tmpE4.f1=k,({void*_tmp115=({void*(*_tmp114)(void*,void*,void*,void*)=f;void*_tmp113=env;void*_tmp112=k;void*_tmp111=Cyc_Dict_lookup(d1,k);_tmp114(_tmp113,_tmp112,_tmp111,v);});_tmpE4.f2=_tmp115;});_tmpE4;});Cyc_Dict_ins(_tmp118,_tmp117,_tmp116,ans_tree);});}}}{
# 457
struct Cyc_Dict_Dict _tmpB1=({struct Cyc_Dict_Dict _tmpE6;_tmpE6.rel=d2.rel,_tmpE6.r=d2.r,_tmpE6.t=ans_tree;_tmpE6;});_npop_handler(0);return _tmpB1;}
# 442
;_pop_region();}}
# 460
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){
return f(a,b1,b2);}
# 464
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 467
return((struct Cyc_Dict_Dict(*)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}
# 470
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
# 472
return({struct Cyc_List_List*_tmpB3=_region_malloc(r,sizeof(*_tmpB3));({struct _tuple0*_tmp119=({struct _tuple0*_tmpB2=_region_malloc(r,sizeof(*_tmpB2));_tmpB2->f1=k,_tmpB2->f2=v;_tmpB2;});_tmpB3->hd=_tmp119;}),_tmpB3->tl=accum;_tmpB3;});}
# 475
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold_c)(Cyc_Dict_to_list_f,r,d,0);}
# 478
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,void*);struct _RegionHandle*f2;};
# 482
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 484
struct _tuple7*_tmpB4=env;struct _RegionHandle*_tmpB6;int(*_tmpB5)(void*,void*);_LL1: _tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;_LL2: {int(*f)(void*,void*)=_tmpB5;struct _RegionHandle*r=_tmpB6;
if(f(x,y))
({struct Cyc_Dict_Dict _tmp11A=Cyc_Dict_insert(*acc,x,y);*acc=_tmp11A;});
# 488
return acc;}}
# 491
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){
struct _tuple7 _tmpB7=({struct _tuple7 _tmpE7;_tmpE7.f1=f,_tmpE7.f2=r2;_tmpE7;});struct _tuple7 env=_tmpB7;
struct Cyc_Dict_Dict _tmpB8=Cyc_Dict_rempty(r2,d.rel);struct Cyc_Dict_Dict res=_tmpB8;
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& env,d,& res);}
# 502
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,void*,void*);void*f2;struct _RegionHandle*f3;};
# 507
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 510
struct _tuple8*_tmpB9=env;struct _RegionHandle*_tmpBC;void*_tmpBB;int(*_tmpBA)(void*,void*,void*);_LL1: _tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;_tmpBC=_tmpB9->f3;_LL2: {int(*f)(void*,void*,void*)=_tmpBA;void*f_env=_tmpBB;struct _RegionHandle*r2=_tmpBC;
if(f(f_env,x,y))
({struct Cyc_Dict_Dict _tmp11B=Cyc_Dict_insert(*acc,x,y);*acc=_tmp11B;});
return acc;}}
# 516
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 518
struct _tuple8 _tmpBD=({struct _tuple8 _tmpE8;_tmpE8.f1=f,_tmpE8.f2=f_env,_tmpE8.f3=r2;_tmpE8;});struct _tuple8 env=_tmpBD;
struct Cyc_Dict_Dict _tmpBE=Cyc_Dict_rempty(r2,d.rel);struct Cyc_Dict_Dict res=_tmpBE;
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& env,d,& res);}
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
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,& d2,d1);}
# 536
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f1)(void*,void*);void*f2;};
# 540
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
struct _tuple9*_tmpBF=env;void*_tmpC1;int(*_tmpC0)(void*,void*);_LL1: _tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_LL2: {int(*rel)(void*,void*)=_tmpC0;void*x0=_tmpC1;
return rel(x0,x)!= 0;}}
# 545
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
struct _tuple9 _tmpC2=({struct _tuple9 _tmpE9;_tmpE9.f1=d.rel,_tmpE9.f2=x;_tmpE9;});struct _tuple9 env=_tmpC2;
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_delete_f,& env,d);}}
# 551
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return d;{
struct _tuple9 _tmpC3=({struct _tuple9 _tmpEA;_tmpEA.f1=d.rel,_tmpEA.f2=x;_tmpEA;});struct _tuple9 env=_tmpC3;
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c)(d.r,Cyc_Dict_delete_f,& env,d);}}
# 557
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct _fat_ptr f1;int f2;};
# 561
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 564
struct _tuple10*_tmpC4=stk;int*_tmpC6;struct _fat_ptr _tmpC5;_LL1: _tmpC5=_tmpC4->f1;_tmpC6=(int*)& _tmpC4->f2;_LL2: {struct _fat_ptr stack=_tmpC5;int*indp=_tmpC6;
int _tmpC7=*indp;int ind=_tmpC7;
if(ind == -1)
return 0;{
const struct Cyc_Dict_T*_tmpC8=*((const struct Cyc_Dict_T**)_check_fat_subscript(stack,sizeof(const struct Cyc_Dict_T*),ind));const struct Cyc_Dict_T*t=_tmpC8;
*dest=((const struct Cyc_Dict_T*)_check_null(t))->key_val;
-- ind;
if((unsigned)t->left)
*((const struct Cyc_Dict_T**)_check_fat_subscript(stack,sizeof(const struct Cyc_Dict_T*),++ ind))=t->left;
if((unsigned)t->right)
*((const struct Cyc_Dict_T**)_check_fat_subscript(stack,sizeof(const struct Cyc_Dict_T*),++ ind))=t->right;
*indp=ind;
return 1;}}}
# 579
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){
# 584
int half_max_size=1;
const struct Cyc_Dict_T*_tmpC9=d.t;const struct Cyc_Dict_T*t=_tmpC9;
while(t != (const struct Cyc_Dict_T*)0){
t=t->left;
++ half_max_size;}
# 590
t=d.t;{
struct _fat_ptr _tmpCA=({unsigned _tmpCD=(unsigned)(2 * half_max_size);const struct Cyc_Dict_T**_tmpCC=({struct _RegionHandle*_tmp11C=rgn;_region_malloc(_tmp11C,_check_times(_tmpCD,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _tmpEC=(unsigned)(2 * half_max_size);unsigned i;for(i=0;i < _tmpEC;++ i){_tmpCC[i]=t;}}0;});_tag_fat(_tmpCC,sizeof(const struct Cyc_Dict_T*),_tmpCD);});struct _fat_ptr stack=_tmpCA;
return({struct Cyc_Iter_Iter _tmpEB;({struct _tuple10*_tmp11D=({struct _tuple10*_tmpCB=_region_malloc(rgn,sizeof(*_tmpCB));_tmpCB->f1=stack,(unsigned)t?_tmpCB->f2=0:(_tmpCB->f2=- 1);_tmpCB;});_tmpEB.env=_tmp11D;}),_tmpEB.next=Cyc_Dict_iter_f;_tmpEB;});}}
# 600
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 607
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=((int(*)(struct Cyc_List_List*))Cyc_List_length)(dict_list);
# 611
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp11E=({const char*_tmpCE="Dict::marshal: Write failure";_tag_fat(_tmpCE,sizeof(char),29U);});_tmpCF->f1=_tmp11E;});_tmpCF;}));
# 613
while(dict_list != 0){
env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key)(env,fp,(struct _tuple0*)dict_list->hd);
env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val)(env,fp,(struct _tuple0*)dict_list->hd);
dict_list=dict_list->tl;}
# 618
return env;}
# 621
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 628
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
int len=Cyc_getw(fp);
if(len == -1)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp11F=({const char*_tmpD0="Dict::unmarshal: list length is -1";_tag_fat(_tmpD0,sizeof(char),35U);});_tmpD1->f1=_tmp11F;});_tmpD1;}));
{int i=0;for(0;i < len;++ i){
void*key=read_key(env,fp);
void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}}
# 637
return dict;}
