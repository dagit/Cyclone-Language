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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);struct _tuple0{void*f1;void*f2;};
# 229 "dict.h"
extern struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 283
extern struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 847 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 864
void*Cyc_Absyn_compress(void*);
# 882
extern void*Cyc_Absyn_uint_type;
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 891
extern void*Cyc_Absyn_empty_effect;
# 895
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 40 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*,void*,unsigned);
# 43
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*,void*);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*,void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 100 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 197
void*Cyc_Tcutil_normalize_effect(void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*youngest;void*opened_regions;};
# 67 "rgnorder.cyc"
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
struct Cyc_Absyn_Kind*_tmp0=Cyc_Tcutil_type_kind(rgn);struct Cyc_Absyn_Kind*_stmttmp0=_tmp0;struct Cyc_Absyn_Kind*_tmp1=_stmttmp0;enum Cyc_Absyn_AliasQual _tmp3;enum Cyc_Absyn_KindQual _tmp2;_tmp2=_tmp1->kind;_tmp3=_tmp1->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp2;enum Cyc_Absyn_AliasQual a=_tmp3;
if((int)k != 3)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB3;_tmpB3.tag=0,({struct _fat_ptr _tmpBF=({const char*_tmp9="bad region type |";_tag_fat(_tmp9,sizeof(char),18U);});_tmpB3.f1=_tmpBF;});_tmpB3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB2;_tmpB2.tag=2,_tmpB2.f1=(void*)rgn;_tmpB2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB1;_tmpB1.tag=0,({struct _fat_ptr _tmpC0=({const char*_tmp8="| passed to add_outlives_constraint";_tag_fat(_tmp8,sizeof(char),36U);});_tmpB1.f1=_tmpC0;});_tmpB1;});void*_tmp4[3];_tmp4[0]=& _tmp5,_tmp4[1]=& _tmp6,_tmp4[2]=& _tmp7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4,sizeof(void*),3));});{
void*_tmpA=Cyc_Absyn_compress(eff);void*_stmttmp1=_tmpA;void*_tmpB=_stmttmp1;void*_tmpC;switch(*((int*)_tmpB)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)){case 9: _tmpC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f2;{struct Cyc_List_List*es=_tmpC;
# 73
for(1;es != 0;es=es->tl){
if(!Cyc_RgnOrder_valid_constraint((void*)es->hd,rgn))return 0;}
return 1;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f2 != 0){_tmpC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f2)->hd;{void*t=_tmpC;
# 82
struct Cyc_Absyn_Kind*_tmpD=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp2=_tmpD;struct Cyc_Absyn_Kind*_tmpE=_stmttmp2;enum Cyc_Absyn_AliasQual _tmp10;enum Cyc_Absyn_KindQual _tmpF;_tmpF=_tmpE->kind;_tmp10=_tmpE->aliasqual;{enum Cyc_Absyn_KindQual tk=_tmpF;enum Cyc_Absyn_AliasQual ta=_tmp10;
return(int)a == 0 ||(int)ta == (int)a;}}}else{goto _LLE;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f2 != 0){_tmpC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f2)->hd;{void*r=_tmpC;
# 85
struct Cyc_Absyn_Kind*_tmp11=Cyc_Tcutil_type_kind(r);struct Cyc_Absyn_Kind*_stmttmp3=_tmp11;struct Cyc_Absyn_Kind*_tmp12=_stmttmp3;enum Cyc_Absyn_AliasQual _tmp14;enum Cyc_Absyn_KindQual _tmp13;_tmp13=_tmp12->kind;_tmp14=_tmp12->aliasqual;{enum Cyc_Absyn_KindQual rk=_tmp13;enum Cyc_Absyn_AliasQual ra=_tmp14;
return(int)k == 3 &&((int)a == 0 ||(int)ra == (int)a);}}}else{goto _LLE;}default: goto _LLE;}case 1:
# 77
 goto _LL9;case 2: _LL9:
 return 1;default: _LLE:
# 88
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB6;_tmpB6.tag=0,({struct _fat_ptr _tmpC1=({const char*_tmp1A="bad effect |";_tag_fat(_tmp1A,sizeof(char),13U);});_tmpB6.f1=_tmpC1;});_tmpB6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB5;_tmpB5.tag=2,_tmpB5.f1=(void*)eff;_tmpB5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB4;_tmpB4.tag=0,({struct _fat_ptr _tmpC2=({const char*_tmp19="| passed to add_outlives_constraint";_tag_fat(_tmp19,sizeof(char),36U);});_tmpB4.f1=_tmpC2;});_tmpB4;});void*_tmp15[3];_tmp15[0]=& _tmp16,_tmp15[1]=& _tmp17,_tmp15[2]=& _tmp18;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp15,sizeof(void*),3));});};}}}
# 95
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned loc){
# 97
eff=Cyc_Tcutil_normalize_effect(eff);{
struct Cyc_RgnOrder_RgnPO*_tmp1B=({struct Cyc_RgnOrder_RgnPO*_tmp2B=_cycalloc(sizeof(*_tmp2B));*_tmp2B=*po;_tmp2B;});struct Cyc_RgnOrder_RgnPO*ans=_tmp1B;
# 100
if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB7;_tmpB7.tag=0,({struct _fat_ptr _tmpC3=({const char*_tmp1E="Invalid region ordering constraint; kind mismatch";_tag_fat(_tmp1E,sizeof(char),50U);});_tmpB7.f1=_tmpC3;});_tmpB7;});void*_tmp1C[1];_tmp1C[0]=& _tmp1D;({unsigned _tmpC4=loc;Cyc_Warn_err2(_tmpC4,_tag_fat(_tmp1C,sizeof(void*),1));});});
return ans;}{
# 105
void*_tmp1F=Cyc_Absyn_compress(rgn);void*_stmttmp4=_tmp1F;void*_tmp20=_stmttmp4;void*_tmp21;switch(*((int*)_tmp20)){case 2: _tmp21=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp20)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp21;
# 107
struct Cyc_Dict_Dict _tmp22=po->d;struct Cyc_Dict_Dict d=_tmp22;
void*_tmp23=
((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member)(d,tv)?Cyc_Absyn_join_eff(({void*_tmp24[2];_tmp24[0]=eff,({void*_tmpC5=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_lookup)(d,tv);_tmp24[1]=_tmpC5;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp24,sizeof(void*),2));})): eff;
# 108
void*new_eff=_tmp23;
# 110
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert)(d,tv,new_eff);
ans->d=d;
return ans;}case 1:
# 114
 Cyc_Unify_unify(rgn,Cyc_Absyn_heap_rgn_type);
goto _LL6;case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20)->f1)){case 5: _LL6:
# 117
({void*_tmpC6=Cyc_Absyn_join_eff(({void*_tmp25[2];_tmp25[0]=eff,_tmp25[1]=po->these_outlive_heap;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp25,sizeof(void*),2));}));ans->these_outlive_heap=_tmpC6;});
return ans;case 6:
# 120
({void*_tmpC7=Cyc_Absyn_join_eff(({void*_tmp26[2];_tmp26[0]=eff,_tmp26[1]=po->these_outlive_unique;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp26,sizeof(void*),2));}));ans->these_outlive_unique=_tmpC7;});
return ans;case 7:
# 123
({void*_tmp27=0U;({struct _fat_ptr _tmpC8=({const char*_tmp28="RgnOrder::add_outlives_constraint can't outlive RC for now";_tag_fat(_tmp28,sizeof(char),59U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpC8,_tag_fat(_tmp27,sizeof(void*),0));});});default: goto _LLB;}default: _LLB:
({void*_tmp29=0U;({struct _fat_ptr _tmpC9=({const char*_tmp2A="RgnOrder::add_outlives_constraint passed a bad region";_tag_fat(_tmp2A,sizeof(char),54U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpC9,_tag_fat(_tmp29,sizeof(void*),0));});});};}}}
# 128
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened){
# 130
if(!opened &&((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member)(po->d,rgn))
({void*_tmp2C=0U;({struct _fat_ptr _tmpCA=({const char*_tmp2D="RgnOrder::add_youngest: repeated region";_tag_fat(_tmp2D,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpCA,_tag_fat(_tmp2C,sizeof(void*),0));});});{
struct Cyc_RgnOrder_RgnPO*_tmp2E=({struct Cyc_RgnOrder_RgnPO*_tmp32=_cycalloc(sizeof(*_tmp32));*_tmp32=*po;_tmp32;});struct Cyc_RgnOrder_RgnPO*ans=_tmp2E;
if(opened){
({struct Cyc_Dict_Dict _tmpCB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert)(po->d,rgn,Cyc_Absyn_empty_effect);ans->d=_tmpCB;});
({void*_tmpCD=Cyc_Absyn_join_eff(({void*_tmp2F[2];({void*_tmpCC=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(rgn));_tmp2F[0]=_tmpCC;}),_tmp2F[1]=ans->opened_regions;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2F,sizeof(void*),2));}));ans->opened_regions=_tmpCD;});}else{
# 137
void*_tmp30=Cyc_Absyn_join_eff(({void*_tmp31[2];({void*_tmpCE=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(po->youngest));_tmp31[0]=_tmpCE;}),_tmp31[1]=ans->opened_regions;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp31,sizeof(void*),2));}));void*eff=_tmp30;
# 139
({struct Cyc_Dict_Dict _tmpCF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert)(po->d,rgn,eff);ans->d=_tmpCF;});
ans->youngest=rgn;}
# 142
return ans;}}
# 144
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn){
if(((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member)(po->d,rgn))
({void*_tmp33=0U;({struct _fat_ptr _tmpD0=({const char*_tmp34="RgnOrder::add_unordered: repeated region";_tag_fat(_tmp34,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpD0,_tag_fat(_tmp33,sizeof(void*),0));});});{
struct Cyc_RgnOrder_RgnPO*_tmp35=({struct Cyc_RgnOrder_RgnPO*_tmp36=_cycalloc(sizeof(*_tmp36));*_tmp36=*po;_tmp36;});struct Cyc_RgnOrder_RgnPO*ans=_tmp35;
({struct Cyc_Dict_Dict _tmpD1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert)(ans->d,rgn,Cyc_Absyn_empty_effect);ans->d=_tmpD1;});
return ans;}}
# 152
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned loc){
# 155
struct Cyc_Dict_Dict _tmp37=((struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_empty)(Cyc_Absyn_tvar_cmp);struct Cyc_Dict_Dict d=_tmp37;
{struct Cyc_List_List*_tmp38=tvs;struct Cyc_List_List*tvs2=_tmp38;for(0;tvs2 != 0;tvs2=tvs2->tl){
if((int)(Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs2->hd,& Cyc_Kinds_bk))->kind == 3)
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert)(d,(struct Cyc_Absyn_Tvar*)tvs2->hd,Cyc_Absyn_empty_effect);}}
# 160
if(!({void*_tmpD2=effect;Cyc_RgnOrder_valid_constraint(_tmpD2,Cyc_Absyn_var_type(fst_rgn));})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB8;_tmpB8.tag=0,({struct _fat_ptr _tmpD3=({const char*_tmp3B="Invalid region ordering constraint; kind mismatch";_tag_fat(_tmp3B,sizeof(char),50U);});_tmpB8.f1=_tmpD3;});_tmpB8;});void*_tmp39[1];_tmp39[0]=& _tmp3A;({unsigned _tmpD4=loc;Cyc_Warn_err2(_tmpD4,_tag_fat(_tmp39,sizeof(void*),1));});});
return({struct Cyc_RgnOrder_RgnPO*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->d=d,_tmp3C->these_outlive_heap=Cyc_Absyn_empty_effect,_tmp3C->these_outlive_unique=Cyc_Absyn_empty_effect,_tmp3C->youngest=fst_rgn,_tmp3C->opened_regions=Cyc_Absyn_empty_effect;_tmp3C;});}
# 164
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert)(d,fst_rgn,effect);{
# 166
struct Cyc_RgnOrder_RgnPO*_tmp3D=({struct Cyc_RgnOrder_RgnPO*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->d=d,_tmp3E->these_outlive_heap=Cyc_Absyn_empty_effect,_tmp3E->these_outlive_unique=Cyc_Absyn_empty_effect,_tmp3E->youngest=fst_rgn,_tmp3E->opened_regions=Cyc_Absyn_empty_effect;_tmp3E;});struct Cyc_RgnOrder_RgnPO*ans=_tmp3D;
for(1;po != 0;po=po->tl){
ans=Cyc_RgnOrder_add_outlives_constraint(ans,(*((struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);}
return ans;}}
# 194 "rgnorder.cyc"
static int Cyc_RgnOrder_contains_rgnseff(struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){
void*_tmp3F=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(eff));void*_stmttmp5=_tmp3F;void*_tmp40=_stmttmp5;void*_tmp41;if(*((int*)_tmp40)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f1)){case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f2 != 0){_tmp41=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f2)->hd;{void*t=_tmp41;
# 197
void*_tmp42=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp42;void*_tmp43=_stmttmp6;void*_tmp44;if(*((int*)_tmp43)== 2){_tmp44=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp43)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp44;
return Cyc_Absyn_tvar_cmp(tv,rgns_of_var)== 0;}}else{
return 0;};}}else{goto _LL5;}case 9: _tmp41=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f2;{struct Cyc_List_List*effs=_tmp41;
# 202
for(1;effs != 0;effs=effs->tl){
if(Cyc_RgnOrder_contains_rgnseff(rgns_of_var,(void*)effs->hd))
return 1;}
return 0;}default: goto _LL5;}else{_LL5:
 return 0;};}struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;struct Cyc_List_List*todo;};
# 216
static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*env,void*eff){
void*_tmp45=Cyc_Absyn_compress(eff);void*_stmttmp7=_tmp45;void*_tmp46=_stmttmp7;void*_tmp47;if(*((int*)_tmp46)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2)->hd)== 2){_tmp47=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2)->hd)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp47;
# 219
{struct Cyc_List_List*_tmp48=env->seen;struct Cyc_List_List*seen=_tmp48;for(0;seen != 0;seen=seen->tl){
if(Cyc_Absyn_tvar_cmp(tv,(struct Cyc_Absyn_Tvar*)seen->hd)== 0)
return;}}
({struct Cyc_List_List*_tmpD5=({struct Cyc_List_List*_tmp49=_region_malloc(env->r,sizeof(*_tmp49));_tmp49->hd=tv,_tmp49->tl=env->seen;_tmp49;});env->seen=_tmpD5;});
({struct Cyc_List_List*_tmpD6=({struct Cyc_List_List*_tmp4A=_region_malloc(env->r,sizeof(*_tmp4A));_tmp4A->hd=tv,_tmp4A->tl=env->todo;_tmp4A;});env->todo=_tmpD6;});
return;}}else{goto _LL5;}}else{goto _LL5;}case 9: _tmp47=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2;{struct Cyc_List_List*effs=_tmp47;
# 226
for(1;effs != 0;effs=effs->tl){
Cyc_RgnOrder_add_to_search(env,(void*)effs->hd);}
return;}default: goto _LL5;}else{_LL5:
 return;};}
# 233
static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){
# 235
struct Cyc_RgnOrder_OutlivesEnv _tmp4B=({struct Cyc_RgnOrder_OutlivesEnv _tmpB9;_tmpB9.r=listrgn,_tmpB9.seen=0,_tmpB9.todo=0;_tmpB9;});struct Cyc_RgnOrder_OutlivesEnv ans=_tmp4B;
void*r=Cyc_Absyn_compress(rgn);
struct Cyc_Absyn_Kind*_tmp4C=Cyc_Tcutil_type_kind(r);struct Cyc_Absyn_Kind*_stmttmp8=_tmp4C;struct Cyc_Absyn_Kind*_tmp4D=_stmttmp8;enum Cyc_Absyn_AliasQual _tmp4F;enum Cyc_Absyn_KindQual _tmp4E;_tmp4E=_tmp4D->kind;_tmp4F=_tmp4D->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp4E;enum Cyc_Absyn_AliasQual a=_tmp4F;
if((int)k != 3)
({void*_tmp50=0U;({struct _fat_ptr _tmpD7=({const char*_tmp51="RgnOrder: rgn not of correct kind";_tag_fat(_tmp51,sizeof(char),34U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpD7,_tag_fat(_tmp50,sizeof(void*),0));});});
{enum Cyc_Absyn_AliasQual _tmp52=a;switch((int)_tmp52){case Cyc_Absyn_Aliasable:
# 243
 Cyc_RgnOrder_add_to_search(& ans,po->these_outlive_unique);
Cyc_RgnOrder_add_to_search(& ans,po->these_outlive_heap);
goto _LL3;case Cyc_Absyn_Unique:
 Cyc_RgnOrder_add_to_search(& ans,po->these_outlive_unique);goto _LL3;case Cyc_Absyn_Top:
# 250
 goto _LLB;default: _LLB: goto _LL3;}_LL3:;}{
# 252
void*_tmp53=r;void*_tmp54;if(*((int*)_tmp53)== 2){_tmp54=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp54;
# 254
({struct Cyc_List_List*_tmpD8=({struct Cyc_List_List*_tmp55=_region_malloc(listrgn,sizeof(*_tmp55));_tmp55->hd=tv,_tmp55->tl=ans.seen;_tmp55;});ans.seen=_tmpD8;});
({struct Cyc_List_List*_tmpD9=({struct Cyc_List_List*_tmp56=_region_malloc(listrgn,sizeof(*_tmp56));_tmp56->hd=tv,_tmp56->tl=ans.todo;_tmp56;});ans.todo=_tmpD9;});
goto _LL10;}}else{_LL10:
 return ans;};}}}
# 263
static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
struct _RegionHandle _tmp57=_new_region("listrgn");struct _RegionHandle*listrgn=& _tmp57;_push_region(listrgn);
{void*_tmp58=Cyc_Absyn_compress(eff);void*_stmttmp9=_tmp58;void*_tmp59=_stmttmp9;void*_tmp5A;if(*((int*)_tmp59)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2 != 0){_tmp5A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2)->hd;{void*vt=_tmp5A;
# 268
void*_tmp5B=Cyc_Absyn_compress(vt);void*_stmttmpA=_tmp5B;void*_tmp5C=_stmttmpA;void*_tmp5D;switch(*((int*)_tmp5C)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C)->f1)){case 6:  {
# 271
struct Cyc_Absyn_Kind*_tmp5E=Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn));struct Cyc_Absyn_Kind*_stmttmpB=_tmp5E;struct Cyc_Absyn_Kind*_tmp5F=_stmttmpB;enum Cyc_Absyn_AliasQual _tmp61;enum Cyc_Absyn_KindQual _tmp60;_tmp60=_tmp5F->kind;_tmp61=_tmp5F->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp60;enum Cyc_Absyn_AliasQual a=_tmp61;
int _tmp62=(int)k == 3 &&(int)a != 2;_npop_handler(0);return _tmp62;}}case 7:
# 276
 if(({void*_tmpDA=Cyc_Absyn_compress(rgn);_tmpDA == Cyc_Absyn_refcnt_rgn_type;})){int _tmp63=1;_npop_handler(0);return _tmp63;}
goto _LLD;case 5: _LLD: {
# 280
struct Cyc_Absyn_Kind*_tmp64=Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn));struct Cyc_Absyn_Kind*_stmttmpC=_tmp64;struct Cyc_Absyn_Kind*_tmp65=_stmttmpC;enum Cyc_Absyn_AliasQual _tmp67;enum Cyc_Absyn_KindQual _tmp66;_tmp66=_tmp65->kind;_tmp67=_tmp65->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp66;enum Cyc_Absyn_AliasQual a=_tmp67;
int _tmp68=(int)k == 3 &&(int)a == 0;_npop_handler(0);return _tmp68;}}default: goto _LL10;}case 2: _tmp5D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5C)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp5D;
# 283
if(po == 0)
({void*_tmp69=0U;({struct _fat_ptr _tmpDB=({const char*_tmp6A="RgnOrder: tvar without a partial-order";_tag_fat(_tmp6A,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpDB,_tag_fat(_tmp69,sizeof(void*),0));});});{
# 295 "rgnorder.cyc"
struct Cyc_RgnOrder_OutlivesEnv _tmp6B=Cyc_RgnOrder_initial_env(listrgn,po,rgn);struct Cyc_RgnOrder_OutlivesEnv env=_tmp6B;
while(env.todo != 0){
struct Cyc_Absyn_Tvar*_tmp6C=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(env.todo))->hd;struct Cyc_Absyn_Tvar*next=_tmp6C;
env.todo=((struct Cyc_List_List*)_check_null(env.todo))->tl;
if(Cyc_Absyn_tvar_cmp(next,tv)== 0){
int _tmp6D=1;_npop_handler(0);return _tmp6D;}
# 305
if(((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member)(po->d,next))
Cyc_RgnOrder_add_to_search(& env,((void*(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_lookup)(po->d,next));else{
# 308
int _tmp6E=0;_npop_handler(0);return _tmp6E;}}{
# 311
int _tmp6F=0;_npop_handler(0);return _tmp6F;}}}default: _LL10: {
int _tmp70=0;_npop_handler(0);return _tmp70;}};}}else{goto _LL5;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2 != 0){_tmp5A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2)->hd;{void*vt=_tmp5A;
# 315
void*_tmp71=Cyc_Absyn_compress(vt);void*_stmttmpD=_tmp71;void*_tmp72=_stmttmpD;void*_tmp73;if(*((int*)_tmp72)== 2){_tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp73;
# 318
if(po == 0)
({void*_tmp74=0U;({struct _fat_ptr _tmpDC=({const char*_tmp75="RgnOrder: tvar without a partial-order";_tag_fat(_tmp75,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpDC,_tag_fat(_tmp74,sizeof(void*),0));});});{
# 323
struct Cyc_RgnOrder_OutlivesEnv _tmp76=Cyc_RgnOrder_initial_env(listrgn,po,rgn);struct Cyc_RgnOrder_OutlivesEnv env=_tmp76;
struct Cyc_Absyn_Kind*_tmp77=Cyc_Tcutil_type_kind(rgn);struct Cyc_Absyn_Kind*_stmttmpE=_tmp77;struct Cyc_Absyn_Kind*_tmp78=_stmttmpE;enum Cyc_Absyn_AliasQual _tmp7A;enum Cyc_Absyn_KindQual _tmp79;_tmp79=_tmp78->kind;_tmp7A=_tmp78->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp79;enum Cyc_Absyn_AliasQual a=_tmp7A;
# 326
if((int)k == 3){
if((int)a == 0){
if(Cyc_RgnOrder_contains_rgnseff(tv,po->these_outlive_heap)||
 Cyc_RgnOrder_contains_rgnseff(tv,po->these_outlive_unique)){
int _tmp7B=1;_npop_handler(0);return _tmp7B;}}else{
if((int)a == 1){
if(Cyc_RgnOrder_contains_rgnseff(tv,po->these_outlive_unique)){
int _tmp7C=1;_npop_handler(0);return _tmp7C;}}}}
# 336
while(env.todo != 0){
struct Cyc_Absyn_Tvar*_tmp7D=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(env.todo))->hd;struct Cyc_Absyn_Tvar*next=_tmp7D;
env.todo=((struct Cyc_List_List*)_check_null(env.todo))->tl;
if(!((int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member)(po->d,next))
({void*_tmp7E=0U;({struct _fat_ptr _tmpDD=({const char*_tmp7F="RgnOrder: type variable not in dict";_tag_fat(_tmp7F,sizeof(char),36U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpDD,_tag_fat(_tmp7E,sizeof(void*),0));});});{
void*_tmp80=((void*(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_lookup)(po->d,next);void*next_eff=_tmp80;
if(Cyc_RgnOrder_contains_rgnseff(tv,next_eff)){
int _tmp81=1;_npop_handler(0);return _tmp81;}
Cyc_RgnOrder_add_to_search(& env,next_eff);}}{
# 346
int _tmp82=0;_npop_handler(0);return _tmp82;}}}}}else{
int _tmp83=0;_npop_handler(0);return _tmp83;};}}else{goto _LL5;}default: goto _LL5;}else{_LL5: {
# 349
int _tmp84=0;_npop_handler(0);return _tmp84;}};}
# 265 "rgnorder.cyc"
;_pop_region();}
# 355 "rgnorder.cyc"
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_tmp85=Cyc_Absyn_compress(eff);void*_stmttmpF=_tmp85;void*_tmp86=_stmttmpF;void*_tmp87;if(*((int*)_tmp86)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f1)== 9){_tmp87=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f2;{struct Cyc_List_List*effs=_tmp87;
# 359
for(1;effs != 0;effs=effs->tl){
if(!Cyc_RgnOrder_effect_outlives(po,(void*)effs->hd,rgn))
return 0;}
return 1;}}else{goto _LL3;}}else{_LL3:
 return Cyc_RgnOrder_atomic_effect_outlives(po,eff,rgn);};}}
# 367
static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_tmp88=Cyc_Absyn_compress(eff);void*_stmttmp10=_tmp88;void*_tmp89=_stmttmp10;void*_tmp8A;switch(*((int*)_tmp89)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f1)){case 9: _tmp8A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f2;{struct Cyc_List_List*effs=_tmp8A;
# 371
for(1;effs != 0;effs=effs->tl){
Cyc_RgnOrder_pin_effect((void*)effs->hd,bd);}
return;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f2 != 0){_tmp8A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f2)->hd;{void*t=_tmp8A;
# 375
void*_tmp8B=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp8B;void*_tmp8C=_stmttmp11;if(*((int*)_tmp8C)== 1){
Cyc_Unify_unify(t,Cyc_Absyn_uint_type);return;}else{
return;};}}else{goto _LL9;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f2 != 0){_tmp8A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f2)->hd;{void*r=_tmp8A;
# 380
void*_tmp8D=Cyc_Absyn_compress(r);void*_stmttmp12=_tmp8D;void*_tmp8E=_stmttmp12;if(*((int*)_tmp8E)== 1){
Cyc_Unify_unify(r,bd);return;}else{
return;};}}else{goto _LL9;}default: goto _LL9;}case 1:
# 384
 Cyc_Unify_unify(eff,Cyc_Absyn_empty_effect);return;default: _LL9:
 return;};}}
# 391
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*po,void*rgn1,void*rgn2){
return({struct Cyc_RgnOrder_RgnPO*_tmpDF=po;void*_tmpDE=Cyc_Absyn_access_eff(rgn1);Cyc_RgnOrder_effect_outlives(_tmpDF,_tmpDE,rgn2);});}
# 395
static int Cyc_RgnOrder_eff_outlives_atomic_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
# 397
eff2=Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff2));{
void*_tmp8F=eff2;void*_tmp90;if(*((int*)_tmp8F)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f1)){case 9: _tmp90=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2;{struct Cyc_List_List*effs=_tmp90;
# 400
for(1;effs != 0;effs=effs->tl){
if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)effs->hd))
return 1;}
return 0;}case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2 != 0){_tmp90=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2)->hd;{void*vt=_tmp90;
return Cyc_RgnOrder_effect_outlives(po,eff1,vt);}}else{goto _LL7;}case 10: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2 != 0){_tmp90=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2)->hd;{void*vt1=_tmp90;
# 406
void*_tmp91=eff1;void*_tmp92;if(*((int*)_tmp91)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp91)->f1)== 10){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp91)->f2 != 0){_tmp92=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp91)->f2)->hd;{void*vt2=_tmp92;
# 408
{struct _tuple0 _tmp93=({struct _tuple0 _tmpBA;({void*_tmpE1=Cyc_Absyn_compress(vt1);_tmpBA.f1=_tmpE1;}),({void*_tmpE0=Cyc_Absyn_compress(vt2);_tmpBA.f2=_tmpE0;});_tmpBA;});struct _tuple0 _stmttmp13=_tmp93;struct _tuple0 _tmp94=_stmttmp13;void*_tmp96;void*_tmp95;if(*((int*)_tmp94.f1)== 2){if(*((int*)_tmp94.f2)== 2){_tmp95=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94.f1)->f1;_tmp96=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_tmp95;struct Cyc_Absyn_Tvar*tv2=_tmp96;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LL11;}}else{_LL11:
 goto _LLE;}_LLE:;}
# 412
goto _LLD;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _LLD:
 return eff1 == Cyc_Absyn_heap_rgn_type;};}}else{goto _LL7;}default: goto _LL7;}else{_LL7:
# 415
 return eff1 == Cyc_Absyn_heap_rgn_type;};}}
# 419
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
eff1=Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff1));{
void*_tmp97=eff1;void*_tmp98;if(*((int*)_tmp97)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp97)->f1)== 9){_tmp98=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp97)->f2;{struct Cyc_List_List*effs=_tmp98;
# 423
for(1;effs != 0;effs=effs->tl){
if(!Cyc_RgnOrder_eff_outlives_eff(po,(void*)effs->hd,eff2))
return 0;}
return 1;}}else{goto _LL3;}}else{_LL3:
 return Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,eff2);};}}
# 441 "rgnorder.cyc"
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin){
# 445
{struct Cyc_List_List*_tmp99=constraints;struct Cyc_List_List*cs=_tmp99;for(0;cs != 0;cs=cs->tl){
struct _tuple0*_tmp9A=(struct _tuple0*)cs->hd;struct _tuple0*_stmttmp14=_tmp9A;struct _tuple0*_tmp9B=_stmttmp14;void*_tmp9C;_tmp9C=_tmp9B->f2;{void*bd=_tmp9C;
void*_tmp9D=Cyc_Absyn_compress(bd);void*_stmttmp15=_tmp9D;void*_tmp9E=_stmttmp15;if(*((int*)_tmp9E)== 1){
# 449
if(do_pin)
Cyc_Unify_unify(bd,default_bound);
goto _LL3;}else{
goto _LL3;}_LL3:;}}}
# 455
{struct Cyc_List_List*_tmp9F=constraints;struct Cyc_List_List*cs=_tmp9F;for(0;cs != 0;cs=cs->tl){
struct _tuple0*_tmpA0=(struct _tuple0*)cs->hd;struct _tuple0*_stmttmp16=_tmpA0;struct _tuple0*_tmpA1=_stmttmp16;void*_tmpA3;void*_tmpA2;_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f2;{void*eff=_tmpA2;void*bd=_tmpA3;
if(do_pin)
Cyc_RgnOrder_pin_effect(eff,bd);
if(!Cyc_RgnOrder_effect_outlives(po,eff,bd))
return 0;}}}
# 462
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 466
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_make_iter)(Cyc_Core_heap_region,po->d);
struct _tuple12 _tmpA4=*((struct _tuple12*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rchoose)(Cyc_Core_heap_region,po->d);struct _tuple12 elem=_tmpA4;
({void*_tmpA5=0U;({struct Cyc___cycFILE*_tmpE3=Cyc_stderr;struct _fat_ptr _tmpE2=({const char*_tmpA6="region po:\n";_tag_fat(_tmpA6,sizeof(char),12U);});Cyc_fprintf(_tmpE3,_tmpE2,_tag_fat(_tmpA5,sizeof(void*),0));});});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next)(iter,& elem)){
({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmpBC;_tmpBC.tag=0,_tmpBC.f1=(struct _fat_ptr)((struct _fat_ptr)*(elem.f1)->name);_tmpBC;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmpBB;_tmpBB.tag=0,({
struct _fat_ptr _tmpE4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elem.f2));_tmpBB.f1=_tmpE4;});_tmpBB;});void*_tmpA7[2];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({struct Cyc___cycFILE*_tmpE6=Cyc_stderr;struct _fat_ptr _tmpE5=({const char*_tmpA8="  %s outlived by %s\n";_tag_fat(_tmpA8,sizeof(char),21U);});Cyc_fprintf(_tmpE6,_tmpE5,_tag_fat(_tmpA7,sizeof(void*),2));});});}
({struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmpBD;_tmpBD.tag=0,({
struct _fat_ptr _tmpE7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(po->these_outlive_heap));_tmpBD.f1=_tmpE7;});_tmpBD;});void*_tmpAB[1];_tmpAB[0]=& _tmpAD;({struct Cyc___cycFILE*_tmpE9=Cyc_stderr;struct _fat_ptr _tmpE8=({const char*_tmpAC="  these outlive heap: %s\n";_tag_fat(_tmpAC,sizeof(char),26U);});Cyc_fprintf(_tmpE9,_tmpE8,_tag_fat(_tmpAB,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0,({
struct _fat_ptr _tmpEA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(po->these_outlive_unique));_tmpBE.f1=_tmpEA;});_tmpBE;});void*_tmpAE[1];_tmpAE[0]=& _tmpB0;({struct Cyc___cycFILE*_tmpEC=Cyc_stderr;struct _fat_ptr _tmpEB=({const char*_tmpAF="  these outlive unique: %s\n";_tag_fat(_tmpAF,sizeof(char),28U);});Cyc_fprintf(_tmpEC,_tmpEB,_tag_fat(_tmpAE,sizeof(void*),1));});});}
