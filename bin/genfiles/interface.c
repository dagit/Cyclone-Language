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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Core_Opt{void*v;};
# 114 "core.h"
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 131 "dict.h"
extern void*Cyc_Dict_fold(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);
# 149
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 852
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 858
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 910
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 1062
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1067
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1121
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty (void);struct _tuple11{struct _fat_ptr f1;struct _fat_ptr f2;};
# 66 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*,struct Cyc_Interface_I*,struct _tuple11*);
# 75
void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 38 "tcdecl.h"
void Cyc_Tcdecl_merr(unsigned,struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr ap);
# 54 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);
# 57
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*);
# 59
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*);
# 61
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*);
# 63
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*);
# 68
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 39 "interface.cyc"
void Cyc_Lex_lex_init (void);struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};struct Cyc_Interface_I{struct Cyc_Interface_Ienv*imports;struct Cyc_Interface_Ienv*exports;struct Cyc_List_List*tds;};
# 70
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv (void){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));({
struct Cyc_Dict_Dict _tmp216=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->aggrdecls=_tmp216;}),({
struct Cyc_Dict_Dict _tmp215=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->datatypedecls=_tmp215;}),({
struct Cyc_Dict_Dict _tmp214=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->enumdecls=_tmp214;}),({
struct Cyc_Dict_Dict _tmp213=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->typedefdecls=_tmp213;}),({
struct Cyc_Dict_Dict _tmp212=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->vardecls=_tmp212;}),({
struct Cyc_Dict_Dict _tmp211=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->xdatatypefielddecls=_tmp211;});_tmp0;});}
# 80
struct Cyc_Interface_I*Cyc_Interface_empty (void){
return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Interface_Ienv*_tmp218=Cyc_Interface_new_ienv();_tmp1->imports=_tmp218;}),({struct Cyc_Interface_Ienv*_tmp217=Cyc_Interface_new_ienv();_tmp1->exports=_tmp217;}),_tmp1->tds=0;_tmp1;});}
# 85
struct Cyc_Interface_I*Cyc_Interface_final (void){
struct Cyc_Interface_I*_tmp2=Cyc_Interface_empty();struct Cyc_Interface_I*i=_tmp2;
# 88
struct Cyc_Absyn_Datatypedecl*_tmp3=({struct Cyc_Absyn_Datatypedecl*_tmp9=_cycalloc(sizeof(*_tmp9));({struct Cyc_Absyn_Datatypedecl _tmp219=*Cyc_Absyn_exn_tud();*_tmp9=_tmp219;});_tmp9;});struct Cyc_Absyn_Datatypedecl*exn_d=_tmp3;
exn_d->sc=Cyc_Absyn_Public;
({struct Cyc_Dict_Dict _tmp21A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert)((i->exports)->datatypedecls,Cyc_Absyn_exn_name,exn_d);(i->exports)->datatypedecls=_tmp21A;});
# 92
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((Cyc_Absyn_exn_tud())->fields))->v;struct Cyc_List_List*tufs=_tmp4;for(0;tufs != 0;tufs=tufs->tl){
struct Cyc_Absyn_Datatypefield*_tmp5=({struct Cyc_Absyn_Datatypefield*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=*((struct Cyc_Absyn_Datatypefield*)tufs->hd);_tmp8;});struct Cyc_Absyn_Datatypefield*exn_f=_tmp5;
exn_f->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp6=({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->base=exn_d,_tmp7->field=exn_f;_tmp7;});struct Cyc_Tcdecl_Xdatatypefielddecl*exn_fd=_tmp6;
({struct Cyc_Dict_Dict _tmp21B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*))Cyc_Dict_insert)((i->exports)->xdatatypefielddecls,((struct Cyc_Absyn_Datatypefield*)tufs->hd)->name,exn_fd);(i->exports)->xdatatypefielddecls=_tmp21B;});}}}
# 117 "interface.cyc"
i->imports=i->exports;
return i;}
# 137 "interface.cyc"
static void Cyc_Interface_err(struct _fat_ptr msg){
({void*_tmpA=0U;({struct _fat_ptr _tmp21C=msg;Cyc_Warn_err(0U,_tmp21C,_tag_fat(_tmpA,sizeof(void*),0));});});}
# 140
static void*Cyc_Interface_invalid_arg(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=Cyc_Core_Invalid_argument,_tmpB->f1=s;_tmpB;}));}
# 143
static void Cyc_Interface_fields_err(struct _fat_ptr sc,struct _fat_ptr t,struct _tuple0*n){
Cyc_Interface_err((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp1E0;_tmp1E0.tag=0,_tmp1E0.f1=(struct _fat_ptr)((struct _fat_ptr)sc);_tmp1E0;});struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp1DF;_tmp1DF.tag=0,_tmp1DF.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1DF;});struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp1DE;_tmp1DE.tag=0,({
struct _fat_ptr _tmp21D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_tmp1DE.f1=_tmp21D;});_tmp1DE;});void*_tmpC[3];_tmpC[0]=& _tmpE,_tmpC[1]=& _tmpF,_tmpC[2]=& _tmp10;({struct _fat_ptr _tmp21E=({const char*_tmpD="fields of %s %s %s have never been defined";_tag_fat(_tmpD,sizeof(char),43U);});Cyc_aprintf(_tmp21E,_tag_fat(_tmpC,sizeof(void*),3));});}));}
# 147
static void Cyc_Interface_body_err(struct _fat_ptr sc,struct _tuple0*n){
Cyc_Interface_err((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp1E2;_tmp1E2.tag=0,_tmp1E2.f1=(struct _fat_ptr)((struct _fat_ptr)sc);_tmp1E2;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0,({
struct _fat_ptr _tmp21F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_tmp1E1.f1=_tmp21F;});_tmp1E1;});void*_tmp11[2];_tmp11[0]=& _tmp13,_tmp11[1]=& _tmp14;({struct _fat_ptr _tmp220=({const char*_tmp12="the body of %s function %s has never been defined";_tag_fat(_tmp12,sizeof(char),50U);});Cyc_aprintf(_tmp220,_tag_fat(_tmp11,sizeof(void*),2));});}));}
# 153
static void Cyc_Interface_static_err(struct _fat_ptr obj1,struct _tuple0*name1,struct _fat_ptr obj2,struct _tuple0*name2){
if(({char*_tmp221=(char*)obj1.curr;_tmp221 != (char*)(_tag_fat(0,0,0)).curr;}))
Cyc_Interface_err((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0,_tmp1E6.f1=(struct _fat_ptr)((struct _fat_ptr)obj1);_tmp1E6;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0,({
struct _fat_ptr _tmp222=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(name1));_tmp1E5.f1=_tmp222;});_tmp1E5;});struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp1E4;_tmp1E4.tag=0,_tmp1E4.f1=(struct _fat_ptr)((struct _fat_ptr)obj2);_tmp1E4;});struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp1E3;_tmp1E3.tag=0,({
struct _fat_ptr _tmp223=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(name2));_tmp1E3.f1=_tmp223;});_tmp1E3;});void*_tmp15[4];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19,_tmp15[3]=& _tmp1A;({struct _fat_ptr _tmp224=({const char*_tmp16="declaration of %s %s relies on static %s %s";_tag_fat(_tmp16,sizeof(char),44U);});Cyc_aprintf(_tmp224,_tag_fat(_tmp15,sizeof(void*),4));});}));}
# 159
static void Cyc_Interface_abstract_err(struct _fat_ptr obj1,struct _tuple0*name1,struct _fat_ptr obj2,struct _tuple0*name2){
if(({char*_tmp225=(char*)obj1.curr;_tmp225 != (char*)(_tag_fat(0,0,0)).curr;}))
Cyc_Interface_err((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0,_tmp1EA.f1=(struct _fat_ptr)((struct _fat_ptr)obj1);_tmp1EA;});struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0,({
struct _fat_ptr _tmp226=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(name1));_tmp1E9.f1=_tmp226;});_tmp1E9;});struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp1E8;_tmp1E8.tag=0,_tmp1E8.f1=(struct _fat_ptr)((struct _fat_ptr)obj2);_tmp1E8;});struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0,({
struct _fat_ptr _tmp227=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(name2));_tmp1E7.f1=_tmp227;});_tmp1E7;});void*_tmp1B[4];_tmp1B[0]=& _tmp1D,_tmp1B[1]=& _tmp1E,_tmp1B[2]=& _tmp1F,_tmp1B[3]=& _tmp20;({struct _fat_ptr _tmp228=({const char*_tmp1C="declaration of %s %s relies on fields of abstract %s %s";_tag_fat(_tmp1C,sizeof(char),56U);});Cyc_aprintf(_tmp228,_tag_fat(_tmp1B,sizeof(void*),4));});}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 175
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen (void){
return({struct Cyc_Interface_Seen*_tmp21=_cycalloc(sizeof(*_tmp21));({struct Cyc_Dict_Dict _tmp22A=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp21->aggrs=_tmp22A;}),({struct Cyc_Dict_Dict _tmp229=((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp21->datatypes=_tmp229;});_tmp21;});}
# 180
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*);
# 182
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _fat_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 184
int _tmp22=1;int res=_tmp22;
for(1;l != 0;l=l->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp22E=ae;struct Cyc_Interface_Seen*_tmp22D=seen;struct _fat_ptr _tmp22C=obj;struct _tuple0*_tmp22B=name;Cyc_Interface_check_public_type(_tmp22E,_tmp22D,_tmp22C,_tmp22B,f(l->hd));}))
res=0;}
return res;}
# 191
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp23;_push_handler(& _tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{int _tmp26=((int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp26;};_pop_handler();}else{void*_tmp24=(void*)Cyc_Core_get_exn_thrown();void*_tmp27=_tmp24;void*_tmp28;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp27)->tag == Cyc_Dict_Absent)
goto _LL0;else{_tmp28=_tmp27;{void*exn=_tmp28;(int)_rethrow(exn);}}_LL0:;}}}{
int _tmp29=1;int res=_tmp29;
({struct Cyc_Dict_Dict _tmp22F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert)(seen->aggrs,d->name,res);seen->aggrs=_tmp22F;});
if(d->impl != 0){
struct Cyc_List_List*_tmp2A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;struct Cyc_List_List*fs=_tmp2A;for(0;fs != 0;fs=fs->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp233=ae;struct Cyc_Interface_Seen*_tmp232=seen;struct _fat_ptr _tmp231=({const char*_tmp2B="type";_tag_fat(_tmp2B,sizeof(char),5U);});struct _tuple0*_tmp230=d->name;Cyc_Interface_check_public_type(_tmp233,_tmp232,_tmp231,_tmp230,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);}))
res=0;}}
# 201
({struct Cyc_Dict_Dict _tmp234=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert)(seen->aggrs,d->name,res);seen->aggrs=_tmp234;});
return res;}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 205
static void*Cyc_Interface_get_type1(struct _tuple12*x){
return(*x).f2;}
# 208
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 212
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){{int _tmp2F=((int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);return _tmp2F;};_pop_handler();}else{void*_tmp2D=(void*)Cyc_Core_get_exn_thrown();void*_tmp30=_tmp2D;void*_tmp31;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp30)->tag == Cyc_Dict_Absent)
goto _LL0;else{_tmp31=_tmp30;{void*exn=_tmp31;(int)_rethrow(exn);}}_LL0:;}}}{
int _tmp32=1;int res=_tmp32;
({struct Cyc_Dict_Dict _tmp235=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert)(seen->datatypes,d->name,res);seen->datatypes=_tmp235;});
if(d->fields != 0){
struct Cyc_List_List*_tmp33=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;struct Cyc_List_List*fs=_tmp33;for(0;fs != 0;fs=fs->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp239=ae;struct Cyc_Interface_Seen*_tmp238=seen;struct _fat_ptr _tmp237=({const char*_tmp34="datatype";_tag_fat(_tmp34,sizeof(char),9U);});struct _tuple0*_tmp236=d->name;((int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list)(_tmp239,_tmp238,_tmp237,_tmp236,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs);}))
# 221
res=0;}}
# 223
({struct Cyc_Dict_Dict _tmp23A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,int))Cyc_Dict_insert)(seen->datatypes,d->name,res);seen->datatypes=_tmp23A;});
return res;}}
# 227
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 231
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return({struct Cyc_Tcenv_Genv*_tmp23E=ae;struct Cyc_Interface_Seen*_tmp23D=seen;struct _fat_ptr _tmp23C=_tag_fat(0,0,0);struct _tuple0*_tmp23B=d->name;Cyc_Interface_check_public_type(_tmp23E,_tmp23D,_tmp23C,_tmp23B,(void*)_check_null(d->defn));});
return 1;}
# 238
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
return({struct Cyc_Tcenv_Genv*_tmp242=ae;struct Cyc_Interface_Seen*_tmp241=seen;struct _fat_ptr _tmp240=({const char*_tmp35="variable";_tag_fat(_tmp35,sizeof(char),9U);});struct _tuple0*_tmp23F=d->name;Cyc_Interface_check_public_type(_tmp242,_tmp241,_tmp240,_tmp23F,d->type);});}
# 242
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _fat_ptr obj,struct _tuple0*name,void*t){
void*_tmp36=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp36;void*_tmp37=_stmttmp0;union Cyc_Absyn_AggrInfo _tmp3B;void*_tmp3A;void*_tmp39;void*_tmp38;switch(*((int*)_tmp37)){case 3: _tmp38=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37)->f1).elt_type;{void*t=_tmp38;
_tmp38=t;goto _LL4;}case 4: _tmp38=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37)->f1).elt_type;_LL4: {void*t=_tmp38;
_tmp38=t;goto _LL6;}case 8: _tmp38=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4;if(_tmp38 != 0){_LL6: {void*t=_tmp38;
# 247
return Cyc_Interface_check_public_type(ae,seen,obj,name,t);}}else{goto _LL13;}case 5: _tmp38=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp37)->f1).ret_type;_tmp39=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp37)->f1).args;_tmp3A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp37)->f1).cyc_varargs;{void*ret=_tmp38;struct Cyc_List_List*args=_tmp39;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp3A;
# 251
int b=((int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,args)&&
 Cyc_Interface_check_public_type(ae,seen,obj,name,ret);
if(cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3C=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp1=_tmp3C;struct Cyc_Absyn_VarargInfo _tmp3D=_stmttmp1;void*_tmp3E;_tmp3E=_tmp3D.type;{void*vt=_tmp3E;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,vt);}}
# 257
return b;}case 6: _tmp38=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp37)->f1;{struct Cyc_List_List*lt=_tmp38;
# 260
return((int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,lt);}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)){case 20: _tmp3B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1;_tmp38=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f2;{union Cyc_Absyn_AggrInfo info=_tmp3B;struct Cyc_List_List*targs=_tmp38;
# 263
struct Cyc_Absyn_Aggrdecl*_tmp3F=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp3F;
if((int)ad->sc == (int)Cyc_Absyn_Static){
({struct _fat_ptr _tmp245=obj;struct _tuple0*_tmp244=name;struct _fat_ptr _tmp243=({const char*_tmp40="type";_tag_fat(_tmp40,sizeof(char),5U);});Cyc_Interface_static_err(_tmp245,_tmp244,_tmp243,ad->name);});
return 0;}
# 268
return((int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(void*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,targs)&&
 Cyc_Interface_check_public_aggrdecl(ae,seen,ad);}case 15: _tmp38=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1;{struct _tuple0*name=_tmp38;
# 272
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp41;_push_handler(& _tmp41);{int _tmp43=0;if(setjmp(_tmp41.handler))_tmp43=1;if(!_tmp43){ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(ae->enumdecls,name);;_pop_handler();}else{void*_tmp42=(void*)Cyc_Core_get_exn_thrown();void*_tmp44=_tmp42;void*_tmp45;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp44)->tag == Cyc_Dict_Absent)
# 275
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp48=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0,({
struct _fat_ptr _tmp246=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(name));_tmp1EB.f1=_tmp246;});_tmp1EB;});void*_tmp46[1];_tmp46[0]=& _tmp48;({struct _fat_ptr _tmp247=({const char*_tmp47="check_public_type (can't find enum %s)";_tag_fat(_tmp47,sizeof(char),39U);});Cyc_aprintf(_tmp247,_tag_fat(_tmp46,sizeof(void*),1));});}));else{_tmp45=_tmp44;{void*exn=_tmp45;(int)_rethrow(exn);}};}}}
# 278
if((int)ed->sc == (int)Cyc_Absyn_Static){
({struct _fat_ptr _tmp24A=obj;struct _tuple0*_tmp249=name;struct _fat_ptr _tmp248=({const char*_tmp49="enum";_tag_fat(_tmp49,sizeof(char),5U);});Cyc_Interface_static_err(_tmp24A,_tmp249,_tmp248,ed->name);});
return 0;}
# 282
return 1;}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatype).tag == 2){_tmp38=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatype).val;_tmp39=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f2;{struct Cyc_Absyn_Datatypedecl*tud0=_tmp38;struct Cyc_List_List*targs=_tmp39;
# 285
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp4A;_push_handler(& _tmp4A);{int _tmp4C=0;if(setjmp(_tmp4A.handler))_tmp4C=1;if(!_tmp4C){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(ae->datatypedecls,tud0->name);;_pop_handler();}else{void*_tmp4B=(void*)Cyc_Core_get_exn_thrown();void*_tmp4D=_tmp4B;void*_tmp4E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4D)->tag == Cyc_Dict_Absent)
# 288
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0,({
struct _fat_ptr _tmp24B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tud0->name));_tmp1EC.f1=_tmp24B;});_tmp1EC;});void*_tmp4F[1];_tmp4F[0]=& _tmp51;({struct _fat_ptr _tmp24C=({const char*_tmp50="check_public_type (can't find datatype %s)";_tag_fat(_tmp50,sizeof(char),43U);});Cyc_aprintf(_tmp24C,_tag_fat(_tmp4F,sizeof(void*),1));});}));else{_tmp4E=_tmp4D;{void*exn=_tmp4E;(int)_rethrow(exn);}};}}}
# 291
if((int)tud->sc == (int)Cyc_Absyn_Static){
({struct _fat_ptr _tmp24F=obj;struct _tuple0*_tmp24E=name;struct _fat_ptr _tmp24D=({const char*_tmp52="datatype";_tag_fat(_tmp52,sizeof(char),9U);});Cyc_Interface_static_err(_tmp24F,_tmp24E,_tmp24D,tud->name);});
return 0;}
# 295
return((int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(void*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,targs)&&
 Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}else{goto _LL13;}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatypefield).tag == 2){_tmp38=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatypefield).val).f1;_tmp39=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatypefield).val).f2;_tmp3A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f2;{struct Cyc_Absyn_Datatypedecl*tud0=_tmp38;struct Cyc_Absyn_Datatypefield*tuf0=_tmp39;struct Cyc_List_List*targs=_tmp3A;
# 299
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp53;_push_handler(& _tmp53);{int _tmp55=0;if(setjmp(_tmp53.handler))_tmp55=1;if(!_tmp55){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(ae->datatypedecls,tud0->name);;_pop_handler();}else{void*_tmp54=(void*)Cyc_Core_get_exn_thrown();void*_tmp56=_tmp54;void*_tmp57;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp56)->tag == Cyc_Dict_Absent)
# 302
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp1ED;_tmp1ED.tag=0,({
struct _fat_ptr _tmp250=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tud0->name));_tmp1ED.f1=_tmp250;});_tmp1ED;});void*_tmp58[1];_tmp58[0]=& _tmp5A;({struct _fat_ptr _tmp251=({const char*_tmp59="check_public_type (can't find datatype %s and search its fields)";_tag_fat(_tmp59,sizeof(char),65U);});Cyc_aprintf(_tmp251,_tag_fat(_tmp58,sizeof(void*),1));});}));else{_tmp57=_tmp56;{void*exn=_tmp57;(int)_rethrow(exn);}};}}}
# 305
if(tud->fields == 0)
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp1EE;_tmp1EE.tag=0,({
struct _fat_ptr _tmp252=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tud->name));_tmp1EE.f1=_tmp252;});_tmp1EE;});void*_tmp5B[1];_tmp5B[0]=& _tmp5D;({struct _fat_ptr _tmp253=({const char*_tmp5C="check_public_type (datatype %s has no fields)";_tag_fat(_tmp5C,sizeof(char),46U);});Cyc_aprintf(_tmp253,_tag_fat(_tmp5B,sizeof(void*),1));});}));{
# 309
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp5E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmp5E;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp((*tuf0->name).f2,(*((struct Cyc_Absyn_Datatypefield*)fs->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 316
if(tuf1 == 0)
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp61=({struct Cyc_String_pa_PrintArg_struct _tmp1EF;_tmp1EF.tag=0,({
struct _fat_ptr _tmp254=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf0->name));_tmp1EF.f1=_tmp254;});_tmp1EF;});void*_tmp5F[1];_tmp5F[0]=& _tmp61;({struct _fat_ptr _tmp255=({const char*_tmp60="check_public_type (can't find datatypefield %s)";_tag_fat(_tmp60,sizeof(char),48U);});Cyc_aprintf(_tmp255,_tag_fat(_tmp5F,sizeof(void*),1));});}));
# 320
if((int)tud->sc == (int)Cyc_Absyn_Static){
({struct _fat_ptr _tmp258=obj;struct _tuple0*_tmp257=name;struct _fat_ptr _tmp256=({const char*_tmp62="datatype";_tag_fat(_tmp62,sizeof(char),9U);});Cyc_Interface_static_err(_tmp258,_tmp257,_tmp256,tud->name);});
return 0;}
# 324
if((int)tud->sc == (int)Cyc_Absyn_Abstract){
({struct _fat_ptr _tmp25B=obj;struct _tuple0*_tmp25A=name;struct _fat_ptr _tmp259=({const char*_tmp63="datatype";_tag_fat(_tmp63,sizeof(char),9U);});Cyc_Interface_abstract_err(_tmp25B,_tmp25A,_tmp259,tud->name);});
return 0;}
# 328
if((int)tuf1->sc == (int)Cyc_Absyn_Static){
({struct _fat_ptr _tmp260=obj;struct _tuple0*_tmp25F=name;struct _fat_ptr _tmp25E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0,({struct _fat_ptr _tmp25C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf1->name));_tmp1F0.f1=_tmp25C;});_tmp1F0;});void*_tmp64[1];_tmp64[0]=& _tmp66;({struct _fat_ptr _tmp25D=({const char*_tmp65="field %s of";_tag_fat(_tmp65,sizeof(char),12U);});Cyc_aprintf(_tmp25D,_tag_fat(_tmp64,sizeof(void*),1));});});Cyc_Interface_static_err(_tmp260,_tmp25F,_tmp25E,tud->name);});
return 0;}
# 332
return((int(*)(struct Cyc_Tcenv_Genv*,struct Cyc_Interface_Seen*,struct _fat_ptr,struct _tuple0*,void*(*)(void*),struct Cyc_List_List*))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,targs)&&
 Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}}else{goto _LL13;}default: goto _LL13;}default: _LL13:
# 335
 return 1;};}struct _tuple13{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Tcenv_Genv*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 341
static void Cyc_Interface_extract_aggrdecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){
# 343
struct _tuple13*_tmp67=env;void*_tmp6C;void*_tmp6B;int _tmp6A;void*_tmp69;void*_tmp68;_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;_tmp6A=_tmp67->f3;_tmp6B=_tmp67->f4;_tmp6C=_tmp67->f5;{struct Cyc_Interface_Ienv*imp=_tmp68;struct Cyc_Interface_Ienv*exp=_tmp69;int check_complete_defs=_tmp6A;struct Cyc_Tcenv_Genv*ae=_tmp6B;struct Cyc_Interface_Seen*seen=_tmp6C;
struct Cyc_Absyn_Aggrdecl*_tmp6D=*dp;struct Cyc_Absyn_Aggrdecl*d=_tmp6D;
enum Cyc_Absyn_Scope _tmp6E=d->sc;enum Cyc_Absyn_Scope _stmttmp2=_tmp6E;enum Cyc_Absyn_Scope _tmp6F=_stmttmp2;switch((int)_tmp6F){case Cyc_Absyn_Static:
# 347
 if(check_complete_defs && d->impl == 0)
({struct _fat_ptr _tmp262=({const char*_tmp70="static";_tag_fat(_tmp70,sizeof(char),7U);});struct _fat_ptr _tmp261=({const char*_tmp71="struct/union";_tag_fat(_tmp71,sizeof(char),13U);});Cyc_Interface_fields_err(_tmp262,_tmp261,d->name);});
goto _LL3;case Cyc_Absyn_Abstract:
# 351
 if(d->impl == 0){
if(check_complete_defs)
({struct _fat_ptr _tmp264=({const char*_tmp72="abstract";_tag_fat(_tmp72,sizeof(char),9U);});struct _fat_ptr _tmp263=({const char*_tmp73="struct/union";_tag_fat(_tmp73,sizeof(char),13U);});Cyc_Interface_fields_err(_tmp264,_tmp263,d->name);});}else{
# 355
d=({struct Cyc_Absyn_Aggrdecl*_tmp74=_cycalloc(sizeof(*_tmp74));*_tmp74=*d;_tmp74;});
d->impl=0;}
# 358
if(Cyc_Interface_check_public_aggrdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp265=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert)(exp->aggrdecls,x,d);exp->aggrdecls=_tmp265;});
goto _LL3;case Cyc_Absyn_Public:
# 362
 if(d->impl == 0){
({struct _fat_ptr _tmp267=({const char*_tmp75="public";_tag_fat(_tmp75,sizeof(char),7U);});struct _fat_ptr _tmp266=({const char*_tmp76="struct/union";_tag_fat(_tmp76,sizeof(char),13U);});Cyc_Interface_fields_err(_tmp267,_tmp266,d->name);});
d=({struct Cyc_Absyn_Aggrdecl*_tmp77=_cycalloc(sizeof(*_tmp77));*_tmp77=*d;_tmp77;});
d->sc=Cyc_Absyn_Abstract;}
# 367
if(Cyc_Interface_check_public_aggrdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp268=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert)(exp->aggrdecls,x,d);exp->aggrdecls=_tmp268;});
goto _LL3;case Cyc_Absyn_ExternC:
 goto _LLD;case Cyc_Absyn_Extern: _LLD:
# 372
 if(Cyc_Interface_check_public_aggrdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp269=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert)(imp->aggrdecls,x,d);imp->aggrdecls=_tmp269;});
goto _LL3;case Cyc_Absyn_Register:
 goto _LL11;default: _LL11:
# 377
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)(({const char*_tmp78="add_aggrdecl";_tag_fat(_tmp78,sizeof(char),13U);}));
goto _LL3;}_LL3:;}}
# 381
static void Cyc_Interface_extract_enumdecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){
# 383
struct _tuple13*_tmp79=env;void*_tmp7E;void*_tmp7D;int _tmp7C;void*_tmp7B;void*_tmp7A;_tmp7A=_tmp79->f1;_tmp7B=_tmp79->f2;_tmp7C=_tmp79->f3;_tmp7D=_tmp79->f4;_tmp7E=_tmp79->f5;{struct Cyc_Interface_Ienv*imp=_tmp7A;struct Cyc_Interface_Ienv*exp=_tmp7B;int check_complete_defs=_tmp7C;struct Cyc_Tcenv_Genv*ae=_tmp7D;struct Cyc_Interface_Seen*seen=_tmp7E;
struct Cyc_Absyn_Enumdecl*_tmp7F=*dp;struct Cyc_Absyn_Enumdecl*d=_tmp7F;
enum Cyc_Absyn_Scope _tmp80=d->sc;enum Cyc_Absyn_Scope _stmttmp3=_tmp80;enum Cyc_Absyn_Scope _tmp81=_stmttmp3;switch((int)_tmp81){case Cyc_Absyn_Static:
# 387
 if(check_complete_defs && d->fields == 0)
({struct _fat_ptr _tmp26B=({const char*_tmp82="static";_tag_fat(_tmp82,sizeof(char),7U);});struct _fat_ptr _tmp26A=({const char*_tmp83="enum";_tag_fat(_tmp83,sizeof(char),5U);});Cyc_Interface_fields_err(_tmp26B,_tmp26A,d->name);});
goto _LL3;case Cyc_Absyn_Abstract:
# 391
 if(d->fields == 0){
if(check_complete_defs)
({struct _fat_ptr _tmp26D=({const char*_tmp84="abstract";_tag_fat(_tmp84,sizeof(char),9U);});struct _fat_ptr _tmp26C=({const char*_tmp85="enum";_tag_fat(_tmp85,sizeof(char),5U);});Cyc_Interface_fields_err(_tmp26D,_tmp26C,d->name);});}else{
# 395
d=({struct Cyc_Absyn_Enumdecl*_tmp86=_cycalloc(sizeof(*_tmp86));*_tmp86=*d;_tmp86;});
d->fields=0;}
# 398
if(Cyc_Interface_check_public_enumdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp26E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert)(exp->enumdecls,x,d);exp->enumdecls=_tmp26E;});
goto _LL3;case Cyc_Absyn_Public:
# 402
 if(d->fields == 0){
({struct _fat_ptr _tmp270=({const char*_tmp87="public";_tag_fat(_tmp87,sizeof(char),7U);});struct _fat_ptr _tmp26F=({const char*_tmp88="enum";_tag_fat(_tmp88,sizeof(char),5U);});Cyc_Interface_fields_err(_tmp270,_tmp26F,d->name);});
d=({struct Cyc_Absyn_Enumdecl*_tmp89=_cycalloc(sizeof(*_tmp89));*_tmp89=*d;_tmp89;});
d->sc=Cyc_Absyn_Abstract;}
# 407
if(Cyc_Interface_check_public_enumdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp271=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert)(exp->enumdecls,x,d);exp->enumdecls=_tmp271;});
goto _LL3;case Cyc_Absyn_ExternC:
 goto _LLD;case Cyc_Absyn_Extern: _LLD:
# 412
 if(Cyc_Interface_check_public_enumdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp272=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert)(imp->enumdecls,x,d);imp->enumdecls=_tmp272;});
goto _LL3;case Cyc_Absyn_Register:
 goto _LL11;default: _LL11:((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)(({const char*_tmp8A="add_enumdecl";_tag_fat(_tmp8A,sizeof(char),13U);}));}_LL3:;}}
# 419
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 421
struct Cyc_Interface_Ienv*env;
{enum Cyc_Absyn_Scope _tmp8B=f->sc;enum Cyc_Absyn_Scope _stmttmp4=_tmp8B;enum Cyc_Absyn_Scope _tmp8C=_stmttmp4;switch((int)_tmp8C){case Cyc_Absyn_Static:
 return;case Cyc_Absyn_Extern:
 env=i->imports;goto _LL0;case Cyc_Absyn_Public:
 env=i->exports;goto _LL0;default:
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)(({const char*_tmp8D="add_xdatatypefielddecl";_tag_fat(_tmp8D,sizeof(char),23U);}));}_LL0:;}
# 429
({struct Cyc_Dict_Dict _tmp275=({
struct Cyc_Dict_Dict _tmp274=env->xdatatypefielddecls;struct _tuple0*_tmp273=f->name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*))Cyc_Dict_insert)(_tmp274,_tmp273,({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8E=_cycalloc(sizeof(*_tmp8E));
_tmp8E->base=d,_tmp8E->field=f;_tmp8E;}));});
# 429
env->xdatatypefielddecls=_tmp275;});}
# 434
static void Cyc_Interface_extract_datatypedecl(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){
# 436
struct _tuple13*_tmp8F=env;void*_tmp95;void*_tmp94;void*_tmp93;int _tmp92;void*_tmp91;void*_tmp90;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;_tmp92=_tmp8F->f3;_tmp93=_tmp8F->f4;_tmp94=_tmp8F->f5;_tmp95=_tmp8F->f6;{struct Cyc_Interface_Ienv*imp=_tmp90;struct Cyc_Interface_Ienv*exp=_tmp91;int check_complete_defs=_tmp92;struct Cyc_Tcenv_Genv*ae=_tmp93;struct Cyc_Interface_Seen*seen=_tmp94;struct Cyc_Interface_I*i=_tmp95;
struct Cyc_Absyn_Datatypedecl*_tmp96=*dp;struct Cyc_Absyn_Datatypedecl*d=_tmp96;
# 439
enum Cyc_Absyn_Scope _tmp97=d->sc;enum Cyc_Absyn_Scope _stmttmp5=_tmp97;enum Cyc_Absyn_Scope _tmp98=_stmttmp5;switch((int)_tmp98){case Cyc_Absyn_Static:
# 441
 if((!d->is_extensible && check_complete_defs)&& d->fields == 0)
({struct _fat_ptr _tmp277=({const char*_tmp99="static";_tag_fat(_tmp99,sizeof(char),7U);});struct _fat_ptr _tmp276=({const char*_tmp9A="datatype";_tag_fat(_tmp9A,sizeof(char),9U);});Cyc_Interface_fields_err(_tmp277,_tmp276,d->name);});
goto _LL3;case Cyc_Absyn_Abstract:
# 445
 if(d->fields == 0){
if(!d->is_extensible && check_complete_defs)
({struct _fat_ptr _tmp279=({const char*_tmp9B="abstract";_tag_fat(_tmp9B,sizeof(char),9U);});struct _fat_ptr _tmp278=({const char*_tmp9C="datatype";_tag_fat(_tmp9C,sizeof(char),9U);});Cyc_Interface_fields_err(_tmp279,_tmp278,d->name);});}else{
# 449
d=({struct Cyc_Absyn_Datatypedecl*_tmp9D=_cycalloc(sizeof(*_tmp9D));*_tmp9D=*d;_tmp9D;});
d->fields=0;}
# 452
if(Cyc_Interface_check_public_datatypedecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp27A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert)(exp->datatypedecls,x,d);exp->datatypedecls=_tmp27A;});
goto _LL3;case Cyc_Absyn_Public:
# 456
 d=({struct Cyc_Absyn_Datatypedecl*_tmp9E=_cycalloc(sizeof(*_tmp9E));*_tmp9E=*d;_tmp9E;});
if(!d->is_extensible && d->fields == 0){
({struct _fat_ptr _tmp27C=({const char*_tmp9F="public";_tag_fat(_tmp9F,sizeof(char),7U);});struct _fat_ptr _tmp27B=({const char*_tmpA0="datatype";_tag_fat(_tmpA0,sizeof(char),9U);});Cyc_Interface_fields_err(_tmp27C,_tmp27B,d->name);});
d->sc=Cyc_Absyn_Abstract;}
# 461
if(Cyc_Interface_check_public_datatypedecl(ae,seen,d)){
if(d->is_extensible && d->fields != 0){
struct Cyc_List_List*_tmpA1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;struct Cyc_List_List*fields=_tmpA1;
d->fields=0;{
struct Cyc_List_List*_tmpA2=fields;struct Cyc_List_List*f=_tmpA2;for(0;f != 0;f=f->tl){
Cyc_Interface_extract_xdatatypefielddecl(i,d,(struct Cyc_Absyn_Datatypefield*)f->hd);}}}
# 468
({struct Cyc_Dict_Dict _tmp27D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert)(exp->datatypedecls,x,d);exp->datatypedecls=_tmp27D;});}
# 470
goto _LL3;case Cyc_Absyn_ExternC:
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)(({const char*_tmpA3="extract_datatypedecl";_tag_fat(_tmpA3,sizeof(char),21U);}));case Cyc_Absyn_Extern:
# 473
 if(Cyc_Interface_check_public_datatypedecl(ae,seen,d)){
if(d->is_extensible && d->fields != 0){
d=({struct Cyc_Absyn_Datatypedecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));*_tmpA4=*d;_tmpA4;});{
struct Cyc_List_List*_tmpA5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;struct Cyc_List_List*fields=_tmpA5;
d->fields=0;
{struct Cyc_List_List*_tmpA6=fields;struct Cyc_List_List*f=_tmpA6;for(0;f != 0;f=f->tl){
Cyc_Interface_extract_xdatatypefielddecl(i,d,(struct Cyc_Absyn_Datatypefield*)f->hd);}}
({struct Cyc_Dict_Dict _tmp27E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl*))Cyc_Dict_insert)(imp->datatypedecls,x,d);imp->datatypedecls=_tmp27E;});}}}
# 482
goto _LL3;case Cyc_Absyn_Register:
 goto _LL11;default: _LL11:
# 485
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)(({const char*_tmpA7="add_datatypedecl";_tag_fat(_tmpA7,sizeof(char),17U);}));
goto _LL3;}_LL3:;}}
# 490
static void Cyc_Interface_extract_typedef(struct _tuple13*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){
# 492
struct _tuple13*_tmpA8=env;void*_tmpAB;void*_tmpAA;void*_tmpA9;_tmpA9=_tmpA8->f2;_tmpAA=_tmpA8->f4;_tmpAB=_tmpA8->f5;{struct Cyc_Interface_Ienv*exp=_tmpA9;struct Cyc_Tcenv_Genv*ae=_tmpAA;struct Cyc_Interface_Seen*seen=_tmpAB;
if(Cyc_Interface_check_public_typedefdecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp27F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert)(exp->typedefdecls,x,d);exp->typedefdecls=_tmp27F;});}}struct _tuple14{void*f1;int f2;};
# 497
static void Cyc_Interface_extract_ordinarie(struct _tuple13*env,struct _tuple0*x,struct _tuple14*v){
# 499
struct _tuple13*_tmpAC=env;void*_tmpB1;void*_tmpB0;int _tmpAF;void*_tmpAE;void*_tmpAD;_tmpAD=_tmpAC->f1;_tmpAE=_tmpAC->f2;_tmpAF=_tmpAC->f3;_tmpB0=_tmpAC->f4;_tmpB1=_tmpAC->f5;{struct Cyc_Interface_Ienv*imp=_tmpAD;struct Cyc_Interface_Ienv*exp=_tmpAE;int check_complete_defs=_tmpAF;struct Cyc_Tcenv_Genv*ae=_tmpB0;struct Cyc_Interface_Seen*seen=_tmpB1;
# 501
void*_tmpB2=(*v).f1;void*_stmttmp6=_tmpB2;void*_tmpB3=_stmttmp6;void*_tmpB4;switch(*((int*)_tmpB3)){case 2: _tmpB4=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpB3)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpB4;
# 503
struct Cyc_Absyn_Vardecl*_tmpB5=({struct Cyc_Absyn_Vardecl*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->sc=fd->sc,_tmpB6->name=fd->name,_tmpB6->varloc=0U,({
# 507
struct Cyc_Absyn_Tqual _tmp280=Cyc_Absyn_empty_tqual(0U);_tmpB6->tq=_tmp280;}),_tmpB6->type=(void*)_check_null(fd->cached_type),_tmpB6->initializer=0,_tmpB6->rgn=0,_tmpB6->attributes=0,_tmpB6->escapes=0,_tmpB6->is_proto=fd->body != 0;_tmpB6;});
# 503
struct Cyc_Absyn_Vardecl*vd=_tmpB5;
# 515
check_complete_defs=0;
_tmpB4=vd;goto _LL7;}case 1: _tmpB4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB3)->f1;_LL7: {struct Cyc_Absyn_Vardecl*d=_tmpB4;
# 518
if(d->initializer != 0){
d=({struct Cyc_Absyn_Vardecl*_tmpB7=_cycalloc(sizeof(*_tmpB7));*_tmpB7=*d;_tmpB7;});
d->initializer=0;}
# 522
{enum Cyc_Absyn_Scope _tmpB8=d->sc;enum Cyc_Absyn_Scope _stmttmp7=_tmpB8;enum Cyc_Absyn_Scope _tmpB9=_stmttmp7;switch((int)_tmpB9){case Cyc_Absyn_Static:
# 524
 if(check_complete_defs && Cyc_Tcutil_is_function_type(d->type))
({struct _fat_ptr _tmp281=({const char*_tmpBA="static";_tag_fat(_tmpBA,sizeof(char),7U);});Cyc_Interface_body_err(_tmp281,d->name);});
goto _LLA;case Cyc_Absyn_Register:
 goto _LL10;case Cyc_Absyn_Abstract: _LL10:
((int(*)(struct _fat_ptr))Cyc_Interface_invalid_arg)(({const char*_tmpBB="extract_ordinarie";_tag_fat(_tmpBB,sizeof(char),18U);}));case Cyc_Absyn_Public:
# 530
 if(check_complete_defs && Cyc_Tcutil_is_function_type(d->type))
({struct _fat_ptr _tmp282=({const char*_tmpBC="public";_tag_fat(_tmpBC,sizeof(char),7U);});Cyc_Interface_body_err(_tmp282,d->name);});
if(Cyc_Interface_check_public_vardecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp283=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*))Cyc_Dict_insert)(exp->vardecls,x,d);exp->vardecls=_tmp283;});
goto _LLA;case Cyc_Absyn_ExternC:
 goto _LL16;case Cyc_Absyn_Extern: _LL16:
 goto _LL18;default: _LL18:
# 538
 if(Cyc_Interface_check_public_vardecl(ae,seen,d))
({struct Cyc_Dict_Dict _tmp284=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Vardecl*))Cyc_Dict_insert)(imp->vardecls,x,d);imp->vardecls=_tmp284;});
goto _LLA;}_LLA:;}
# 542
goto _LL3;}default:
 goto _LL3;}_LL3:;}}struct _tuple15{void*f1;void*f2;};
# 547
static struct Cyc_List_List*Cyc_Interface_remove_decl_from_list(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*d){
if(l == 0)return 0;
{struct _tuple15 _tmpBD=({struct _tuple15 _tmp1F1;_tmp1F1.f1=d->r,_tmp1F1.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmp1F1;});struct _tuple15 _stmttmp8=_tmpBD;struct _tuple15 _tmpBE=_stmttmp8;void*_tmpC0;void*_tmpBF;switch(*((int*)_tmpBE.f1)){case 8: if(*((int*)_tmpBE.f2)== 8){_tmpBF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpBE.f1)->f1;_tmpC0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpBE.f2)->f1;{struct Cyc_Absyn_Typedefdecl*a1=_tmpBF;struct Cyc_Absyn_Typedefdecl*a2=_tmpC0;
# 551
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!= 0)goto _LL0;
return Cyc_Interface_remove_decl_from_list(l->tl,d);}}else{goto _LL7;}case 7: if(*((int*)_tmpBE.f2)== 7){_tmpBF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpBE.f1)->f1;_tmpC0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpBE.f2)->f1;{struct Cyc_Absyn_Enumdecl*a1=_tmpBF;struct Cyc_Absyn_Enumdecl*a2=_tmpC0;
# 554
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!= 0)goto _LL0;
if((int)a1->sc == (int)Cyc_Absyn_Extern)a1->sc=a2->sc;
return Cyc_Interface_remove_decl_from_list(l->tl,d);
goto _LL0;}}else{goto _LL7;}case 0: if(*((int*)_tmpBE.f2)== 0){_tmpBF=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBE.f1)->f1;_tmpC0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBE.f2)->f1;{struct Cyc_Absyn_Vardecl**vd1=(struct Cyc_Absyn_Vardecl**)_tmpBF;struct Cyc_Absyn_Vardecl*vd2=_tmpC0;
# 559
if(Cyc_Absyn_qvar_cmp((*vd1)->name,vd2->name)!= 0)goto _LL0;
if((int)(*vd1)->sc == (int)Cyc_Absyn_Extern)({struct Cyc_Absyn_Vardecl*_tmp285=({struct Cyc_Absyn_Vardecl*_tmpC1=_cycalloc(sizeof(*_tmpC1));*_tmpC1=*vd2;_tmpC1;});*vd1=_tmp285;});
return Cyc_Interface_remove_decl_from_list(l->tl,d);}}else{goto _LL7;}default: _LL7:
# 563
 goto _LL0;}_LL0:;}
# 565
return({struct Cyc_List_List*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->hd=(struct Cyc_Absyn_Decl*)l->hd,({struct Cyc_List_List*_tmp286=Cyc_Interface_remove_decl_from_list(l->tl,d);_tmpC2->tl=_tmp286;});_tmpC2;});}
# 568
static struct Cyc_List_List*Cyc_Interface_uniqify_decl_list(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
if(accum == 0)return({struct Cyc_List_List*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->hd=d,_tmpC3->tl=0;_tmpC3;});
{struct Cyc_List_List*l=accum;for(0;l != 0;l=l->tl){
struct _tuple15 _tmpC4=({struct _tuple15 _tmp1F2;_tmp1F2.f1=d->r,_tmp1F2.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmp1F2;});struct _tuple15 _stmttmp9=_tmpC4;struct _tuple15 _tmpC5=_stmttmp9;void*_tmpC9;void*_tmpC8;void*_tmpC7;void*_tmpC6;switch(*((int*)_tmpC5.f1)){case 0: if(*((int*)_tmpC5.f2)== 0){_tmpC6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC5.f1)->f1;_tmpC7=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC5.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_tmpC6;struct Cyc_Absyn_Vardecl**vd2=_tmpC7;
# 573
if(Cyc_Absyn_qvar_cmp(vd1->name,(*vd2)->name)!= 0)goto _LL0;
if((int)(*vd2)->sc == (int)Cyc_Absyn_Extern)({struct Cyc_Absyn_Vardecl*_tmp287=({struct Cyc_Absyn_Vardecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));*_tmpCA=*vd1;_tmpCA;});*vd2=_tmp287;});
return accum;}}else{goto _LL9;}case 8: if(*((int*)_tmpC5.f2)== 8){_tmpC6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC5.f1)->f1;_tmpC7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC5.f2)->f1;{struct Cyc_Absyn_Typedefdecl*a1=_tmpC6;struct Cyc_Absyn_Typedefdecl*a2=_tmpC7;
# 577
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!= 0)goto _LL0;
return accum;}}else{goto _LL9;}case 7: if(*((int*)_tmpC5.f2)== 7){_tmpC6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC5.f1)->f1;_tmpC7=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC5.f2)->f1;{struct Cyc_Absyn_Enumdecl*a1=_tmpC6;struct Cyc_Absyn_Enumdecl*a2=_tmpC7;
# 580
if(Cyc_Absyn_qvar_cmp(a1->name,a2->name)!= 0)goto _LL0;
return accum;}}else{goto _LL9;}case 9: if(*((int*)_tmpC5.f2)== 9){_tmpC6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC5.f1)->f1;_tmpC7=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC5.f1)->f2;_tmpC8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC5.f2)->f1;_tmpC9=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC5.f2)->f2;{struct _fat_ptr*a1=_tmpC6;struct Cyc_List_List**b1=(struct Cyc_List_List**)_tmpC7;struct _fat_ptr*a2=_tmpC8;struct Cyc_List_List*b2=_tmpC9;
# 583
if(Cyc_strptrcmp(a1,a2)!= 0)goto _LL0;
{struct Cyc_List_List*_tmpCB=b2;struct Cyc_List_List*dl=_tmpCB;for(0;dl != 0;dl=dl->tl){
({struct Cyc_List_List*_tmp288=Cyc_Interface_remove_decl_from_list(*b1,(struct Cyc_Absyn_Decl*)dl->hd);*b1=_tmp288;});}}
goto _LL0;}}else{goto _LL9;}default: _LL9:
 goto _LL0;}_LL0:;}}
# 589
return({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=d,_tmpCC->tl=accum;_tmpCC;});}
# 592
static struct Cyc_List_List*Cyc_Interface_filterstatics(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
{void*_tmpCD=d->r;void*_stmttmpA=_tmpCD;void*_tmpCE=_stmttmpA;void*_tmpD0;void*_tmpCF;switch(*((int*)_tmpCE)){case 0: _tmpCF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpCF;
# 595
if((int)vd->sc == (int)Cyc_Absyn_ExternC)return accum;
if((int)vd->sc == (int)Cyc_Absyn_Static)return accum;{
struct Cyc_Absyn_Vardecl*_tmpD1=({struct Cyc_Absyn_Vardecl*_tmpD4=_cycalloc(sizeof(*_tmpD4));*_tmpD4=*vd;_tmpD4;});struct Cyc_Absyn_Vardecl*nvd=_tmpD1;
nvd->initializer=0;
if(Cyc_Tcutil_is_function_type(nvd->type)&&(int)nvd->sc != (int)Cyc_Absyn_Extern)
nvd->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));({struct Cyc_Absyn_Decl*_tmp289=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=0,_tmpD2->f1=nvd;_tmpD2;}),0U);_tmpD3->hd=_tmp289;}),_tmpD3->tl=accum;_tmpD3;});}}case 1: _tmpCF=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpCF;
# 603
if((int)fd->sc == (int)Cyc_Absyn_Static)return accum;
if((int)fd->sc == (int)Cyc_Absyn_ExternC)return accum;{
struct Cyc_Absyn_Vardecl*_tmpD5=Cyc_Absyn_new_vardecl(0U,fd->name,(void*)_check_null(fd->cached_type),0);struct Cyc_Absyn_Vardecl*vd=_tmpD5;
vd->sc=fd->sc;
return({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct Cyc_Absyn_Decl*_tmp28A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=0,_tmpD6->f1=vd;_tmpD6;}),0U);_tmpD7->hd=_tmp28A;}),_tmpD7->tl=accum;_tmpD7;});}}case 5: _tmpCF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;{struct Cyc_Absyn_Aggrdecl*a=_tmpCF;
# 610
if((int)a->sc == (int)Cyc_Absyn_ExternC)return accum;
goto _LL0;}case 6: _tmpCF=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;{struct Cyc_Absyn_Datatypedecl*a=_tmpCF;
# 614
if((int)a->sc == (int)Cyc_Absyn_ExternC)return accum;
goto _LL0;}case 7: _tmpCF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;{struct Cyc_Absyn_Enumdecl*a=_tmpCF;
# 617
if((int)a->sc == (int)Cyc_Absyn_Static)return accum;
if((int)a->sc == (int)Cyc_Absyn_ExternC)return accum;
goto _LL0;}case 8: _tmpCF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;{struct Cyc_Absyn_Typedefdecl*a=_tmpCF;
# 621
goto _LL0;}case 9: _tmpCF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;_tmpD0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpCE)->f2;{struct _fat_ptr*a=_tmpCF;struct Cyc_List_List*b=_tmpD0;
# 623
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,b));struct Cyc_List_List*l=_tmpD8;
return({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Decl*_tmp28B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=9,_tmpD9->f1=a,_tmpD9->f2=l;_tmpD9;}),0U);_tmpDA->hd=_tmp28B;}),_tmpDA->tl=accum;_tmpDA;});}case 10: _tmpCF=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpCE)->f2;{struct Cyc_List_List*b=_tmpCF;
# 626
return({struct Cyc_List_List*_tmp28C=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,b);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp28C,accum);});}case 2:
 goto _LL14;case 3: _LL14:
 goto _LL16;case 4: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 return accum;}_LL0:;}
# 637
return({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Absyn_Decl*_tmp28D=({struct Cyc_Absyn_Decl*_tmpDB=_cycalloc(sizeof(*_tmpDB));*_tmpDB=*d;_tmpDB;});_tmpDC->hd=_tmp28D;}),_tmpDC->tl=accum;_tmpDC;});}struct _tuple16{struct Cyc_Interface_I*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;};
# 640
static void Cyc_Interface_extract_f(struct _tuple16*env_f){
struct _tuple16*_tmpDD=env_f;void*_tmpE1;void*_tmpE0;int _tmpDF;void*_tmpDE;_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;_tmpE0=_tmpDD->f3;_tmpE1=_tmpDD->f4;{struct Cyc_Interface_I*i=_tmpDE;int check_complete_defs=_tmpDF;struct Cyc_Tcenv_Genv*ae=_tmpE0;struct Cyc_Interface_Seen*seen=_tmpE1;
struct _tuple13 _tmpE2=({struct _tuple13 _tmp1F3;_tmp1F3.f1=i->imports,_tmp1F3.f2=i->exports,_tmp1F3.f3=check_complete_defs,_tmp1F3.f4=ae,_tmp1F3.f5=seen,_tmp1F3.f6=i;_tmp1F3;});struct _tuple13 env=_tmpE2;
((void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& env,ae->aggrdecls);
((void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& env,ae->datatypedecls);
((void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& env,ae->enumdecls);
((void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& env,ae->typedefs);
((void(*)(void(*)(struct _tuple13*,struct _tuple0*,struct _tuple14*),struct _tuple13*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& env,ae->ordinaries);}}
# 650
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs,struct Cyc_List_List*tds){
struct _tuple16 env=({struct _tuple16 _tmp1F4;({
struct Cyc_Interface_I*_tmp28F=Cyc_Interface_empty();_tmp1F4.f1=_tmp28F;}),_tmp1F4.f2=check_complete_defs,_tmp1F4.f3=ae,({struct Cyc_Interface_Seen*_tmp28E=Cyc_Interface_new_seen();_tmp1F4.f4=_tmp28E;});_tmp1F4;});
Cyc_Interface_extract_f(& env);{
struct Cyc_Interface_I*_tmpE3=env.f1;struct Cyc_Interface_I*i=_tmpE3;
({struct Cyc_List_List*_tmp290=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,tds));i->tds=_tmp290;});
({struct Cyc_List_List*_tmp291=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,0,i->tds));i->tds=_tmp291;});
return i;}}
# 660
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae,struct Cyc_List_List*tds){
return Cyc_Interface_gen_extract(ae,1,tds);}
# 666
static void Cyc_Interface_check_err(struct _fat_ptr*msg1,struct _fat_ptr msg2){
({struct Cyc_String_pa_PrintArg_struct _tmpE6=({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0,_tmp1F5.f1=(struct _fat_ptr)((struct _fat_ptr)msg2);_tmp1F5;});void*_tmpE4[1];_tmpE4[0]=& _tmpE6;({struct _fat_ptr*_tmp293=msg1;struct _fat_ptr _tmp292=({const char*_tmpE5="%s";_tag_fat(_tmpE5,sizeof(char),3U);});Cyc_Tcdecl_merr(0U,_tmp293,_tmp292,_tag_fat(_tmpE4,sizeof(void*),1));});});}struct _tuple17{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _fat_ptr*);struct _fat_ptr f4;struct _fat_ptr*f5;};
# 670
static void Cyc_Interface_incl_dict_f(struct _tuple17*env,struct _tuple0*x,void*y1){
# 677
struct _tuple17*_tmpE7=env;void*_tmpEC;struct _fat_ptr _tmpEB;void*_tmpEA;struct Cyc_Dict_Dict _tmpE9;void*_tmpE8;_tmpE8=(int*)& _tmpE7->f1;_tmpE9=_tmpE7->f2;_tmpEA=_tmpE7->f3;_tmpEB=_tmpE7->f4;_tmpEC=_tmpE7->f5;{int*res=(int*)_tmpE8;struct Cyc_Dict_Dict dic2=_tmpE9;int(*incl_f)(void*,void*,struct _fat_ptr*)=_tmpEA;struct _fat_ptr t=_tmpEB;struct _fat_ptr*msg=_tmpEC;
struct _handler_cons _tmpED;_push_handler(& _tmpED);{int _tmpEF=0;if(setjmp(_tmpED.handler))_tmpEF=1;if(!_tmpEF){
{void*_tmpF0=((void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(dic2,x);void*y2=_tmpF0;
if(!incl_f(y1,y2,msg))*res=0;}
# 679
;_pop_handler();}else{void*_tmpEE=(void*)Cyc_Core_get_exn_thrown();void*_tmpF1=_tmpEE;void*_tmpF2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpF1)->tag == Cyc_Dict_Absent){
# 682
({struct _fat_ptr*_tmp296=msg;Cyc_Interface_check_err(_tmp296,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0,_tmp1F7.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1F7;});struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0,({struct _fat_ptr _tmp294=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(x));_tmp1F6.f1=_tmp294;});_tmp1F6;});void*_tmpF3[2];_tmpF3[0]=& _tmpF5,_tmpF3[1]=& _tmpF6;({struct _fat_ptr _tmp295=({const char*_tmpF4="%s %s is missing";_tag_fat(_tmpF4,sizeof(char),17U);});Cyc_aprintf(_tmp295,_tag_fat(_tmpF3,sizeof(void*),2));});}));});
*res=0;
goto _LL3;}else{_tmpF2=_tmpF1;{void*exn=_tmpF2;(int)_rethrow(exn);}}_LL3:;}}}}
# 688
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _fat_ptr*),struct _fat_ptr t,struct _fat_ptr*msg){
# 693
struct _tuple17 _tmpF7=({struct _tuple17 _tmp1F8;_tmp1F8.f1=1,_tmp1F8.f2=dic2,_tmp1F8.f3=incl_f,_tmp1F8.f4=t,_tmp1F8.f5=msg;_tmp1F8;});struct _tuple17 env=_tmpF7;
((void(*)(void(*)(struct _tuple17*,struct _tuple0*,void*),struct _tuple17*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& env,dic1);
return env.f1;}
# 710 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmpF8=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0U,msg);struct Cyc_Absyn_Aggrdecl*d=_tmpF8;if(d == 0)return 0;if(d0 != d){({struct _fat_ptr*_tmp299=msg;Cyc_Interface_check_err(_tmp299,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmp1F9;_tmp1F9.tag=0,({struct _fat_ptr _tmp297=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp1F9.f1=_tmp297;});_tmp1F9;});void*_tmpF9[1];_tmpF9[0]=& _tmpFB;({struct _fat_ptr _tmp298=({const char*_tmpFA="declaration of type %s discloses too much information";_tag_fat(_tmpFA,sizeof(char),54U);});Cyc_aprintf(_tmp298,_tag_fat(_tmpF9,sizeof(void*),1));});}));});return 0;}return 1;}
# 712
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmpFC=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0U,msg);struct Cyc_Absyn_Datatypedecl*d=_tmpFC;if(d == 0)return 0;if(d0 != d){({struct _fat_ptr*_tmp29C=msg;Cyc_Interface_check_err(_tmp29C,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFF=({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0,({struct _fat_ptr _tmp29A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp1FA.f1=_tmp29A;});_tmp1FA;});void*_tmpFD[1];_tmpFD[0]=& _tmpFF;({struct _fat_ptr _tmp29B=({const char*_tmpFE="declaration of datatype %s discloses too much information";_tag_fat(_tmpFE,sizeof(char),58U);});Cyc_aprintf(_tmp29B,_tag_fat(_tmpFD,sizeof(void*),1));});}));});return 0;}return 1;}
# 714
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmp100=Cyc_Tcdecl_merge_enumdecl(d0,d1,0U,msg);struct Cyc_Absyn_Enumdecl*d=_tmp100;if(d == 0)return 0;if(d0 != d){({struct _fat_ptr*_tmp29F=msg;Cyc_Interface_check_err(_tmp29F,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0,({struct _fat_ptr _tmp29D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp1FB.f1=_tmp29D;});_tmp1FB;});void*_tmp101[1];_tmp101[0]=& _tmp103;({struct _fat_ptr _tmp29E=({const char*_tmp102="declaration of enum %s discloses too much information";_tag_fat(_tmp102,sizeof(char),54U);});Cyc_aprintf(_tmp29E,_tag_fat(_tmp101,sizeof(void*),1));});}));});return 0;}return 1;}
# 716
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Vardecl*_tmp104=Cyc_Tcdecl_merge_vardecl(d0,d1,0U,msg);struct Cyc_Absyn_Vardecl*d=_tmp104;if(d == 0)return 0;if(d0 != d){({struct _fat_ptr*_tmp2A2=msg;Cyc_Interface_check_err(_tmp2A2,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0,({struct _fat_ptr _tmp2A0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp1FC.f1=_tmp2A0;});_tmp1FC;});void*_tmp105[1];_tmp105[0]=& _tmp107;({struct _fat_ptr _tmp2A1=({const char*_tmp106="declaration of variable %s discloses too much information";_tag_fat(_tmp106,sizeof(char),58U);});Cyc_aprintf(_tmp2A1,_tag_fat(_tmp105,sizeof(void*),1));});}));});return 0;}return 1;}
# 718
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _fat_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp108=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0U,msg);struct Cyc_Absyn_Typedefdecl*d=_tmp108;if(d == 0)return 0;if(d0 != d){({struct _fat_ptr*_tmp2A5=msg;Cyc_Interface_check_err(_tmp2A5,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10B=({struct Cyc_String_pa_PrintArg_struct _tmp1FD;_tmp1FD.tag=0,({struct _fat_ptr _tmp2A3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp1FD.f1=_tmp2A3;});_tmp1FD;});void*_tmp109[1];_tmp109[0]=& _tmp10B;({struct _fat_ptr _tmp2A4=({const char*_tmp10A="declaration of typedef %s discloses too much information";_tag_fat(_tmp10A,sizeof(char),57U);});Cyc_aprintf(_tmp2A4,_tag_fat(_tmp109,sizeof(void*),1));});}));});return 0;}return 1;}
# 720
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _fat_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp10C=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0U,msg);struct Cyc_Tcdecl_Xdatatypefielddecl*d=_tmp10C;if(d == 0)return 0;if(d0 != d){({struct _fat_ptr*_tmp2A8=msg;Cyc_Interface_check_err(_tmp2A8,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0,({struct _fat_ptr _tmp2A6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((d1->field)->name));_tmp1FE.f1=_tmp2A6;});_tmp1FE;});void*_tmp10D[1];_tmp10D[0]=& _tmp10F;({struct _fat_ptr _tmp2A7=({const char*_tmp10E="declaration of xdatatypefield %s discloses too much information";_tag_fat(_tmp10E,sizeof(char),64U);});Cyc_aprintf(_tmp2A7,_tag_fat(_tmp10D,sizeof(void*),1));});}));});return 0;}return 1;}
# 725
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,void*(*)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*);
# 734
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _fat_ptr*msg){
int _tmp110=({struct Cyc_Dict_Dict _tmp2AB=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2AA=ie2->aggrdecls;struct _fat_ptr _tmp2A9=({const char*_tmp11B="type";_tag_fat(_tmp11B,sizeof(char),5U);});((int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict)(_tmp2AB,_tmp2AA,Cyc_Interface_incl_aggrdecl,_tmp2A9,msg);});int r1=_tmp110;
int _tmp111=({struct Cyc_Dict_Dict _tmp2AE=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2AD=ie2->datatypedecls;struct _fat_ptr _tmp2AC=({const char*_tmp11A="datatype";_tag_fat(_tmp11A,sizeof(char),9U);});((int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict)(_tmp2AE,_tmp2AD,Cyc_Interface_incl_datatypedecl,_tmp2AC,msg);});int r2=_tmp111;
int _tmp112=({struct Cyc_Dict_Dict _tmp2B1=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2B0=ie2->enumdecls;struct _fat_ptr _tmp2AF=({const char*_tmp119="enum";_tag_fat(_tmp119,sizeof(char),5U);});((int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict)(_tmp2B1,_tmp2B0,Cyc_Interface_incl_enumdecl,_tmp2AF,msg);});int r3=_tmp112;
# 739
int _tmp113=({struct Cyc_Dict_Dict _tmp2B5=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2B4=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2B3=
((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 739
struct _fat_ptr _tmp2B2=({const char*_tmp118="typedef";_tag_fat(_tmp118,sizeof(char),8U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2B5,_tmp2B4,_tmp2B3,Cyc_Tcdecl_merge_typedefdecl,_tmp2B2,msg);})!= 0;int r4=_tmp113;
# 742
int _tmp114=({struct Cyc_Dict_Dict _tmp2B8=ie1->vardecls;struct Cyc_Dict_Dict _tmp2B7=ie2->vardecls;struct _fat_ptr _tmp2B6=({const char*_tmp117="variable";_tag_fat(_tmp117,sizeof(char),9U);});((int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict)(_tmp2B8,_tmp2B7,Cyc_Interface_incl_vardecl,_tmp2B6,msg);});int r5=_tmp114;
int _tmp115=({struct Cyc_Dict_Dict _tmp2BB=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2BA=ie2->xdatatypefielddecls;struct _fat_ptr _tmp2B9=({const char*_tmp116="xdatatypefield";_tag_fat(_tmp116,sizeof(char),15U);});((int(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,int(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_incl_dict)(_tmp2BB,_tmp2BA,Cyc_Interface_incl_xdatatypefielddecl,_tmp2B9,msg);});int r6=_tmp115;
return((((r1 && r2)&& r3)&& r4)&& r5)&& r6;}
# 747
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info){
struct _handler_cons _tmp11C;_push_handler(& _tmp11C);{int _tmp11E=0;if(setjmp(_tmp11C.handler))_tmp11E=1;if(!_tmp11E){
{int _tmp11F=1;int res=_tmp11F;
struct _fat_ptr*msg=0;
# 752
if(info != 0)
msg=({struct _fat_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _fat_ptr _tmp2BD=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0,_tmp200.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f1);_tmp200;});struct Cyc_String_pa_PrintArg_struct _tmp123=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0,_tmp1FF.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f2);_tmp1FF;});void*_tmp120[2];_tmp120[0]=& _tmp122,_tmp120[1]=& _tmp123;({struct _fat_ptr _tmp2BC=({const char*_tmp121="checking inclusion of %s exports into %s exports,";_tag_fat(_tmp121,sizeof(char),50U);});Cyc_aprintf(_tmp2BC,_tag_fat(_tmp120,sizeof(void*),2));});});*_tmp124=_tmp2BD;});_tmp124;});
if(!Cyc_Interface_incl_ienv(i1->exports,i2->exports,msg))res=0;
# 756
if(info != 0)
msg=({struct _fat_ptr*_tmp129=_cycalloc(sizeof(*_tmp129));({struct _fat_ptr _tmp2BF=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp127=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0,_tmp202.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f2);_tmp202;});struct Cyc_String_pa_PrintArg_struct _tmp128=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0,_tmp201.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f1);_tmp201;});void*_tmp125[2];_tmp125[0]=& _tmp127,_tmp125[1]=& _tmp128;({struct _fat_ptr _tmp2BE=({const char*_tmp126="checking inclusion of %s imports into %s imports,";_tag_fat(_tmp126,sizeof(char),50U);});Cyc_aprintf(_tmp2BE,_tag_fat(_tmp125,sizeof(void*),2));});});*_tmp129=_tmp2BF;});_tmp129;});
if(!Cyc_Interface_incl_ienv(i2->imports,i1->imports,msg))res=0;{
# 760
int _tmp12A=res;_npop_handler(0);return _tmp12A;}}
# 749
;_pop_handler();}else{void*_tmp11D=(void*)Cyc_Core_get_exn_thrown();void*_tmp12B=_tmp11D;void*_tmp12C;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp12B)->tag == Cyc_Tcdecl_Incompatible)
# 761
return 0;else{_tmp12C=_tmp12B;{void*exn=_tmp12C;(int)_rethrow(exn);}};}}}struct _tuple18{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned,struct _fat_ptr*);struct _fat_ptr f6;struct _fat_ptr*f7;};
# 768
void Cyc_Interface_compat_merge_dict_f(struct _tuple18*env,struct _tuple0*x,void*y2){
# 775
struct _tuple18*_tmp12D=env;void*_tmp134;struct _fat_ptr _tmp133;void*_tmp132;struct Cyc_Dict_Dict _tmp131;struct Cyc_Dict_Dict _tmp130;void*_tmp12F;void*_tmp12E;_tmp12E=(int*)& _tmp12D->f1;_tmp12F=(struct Cyc_Dict_Dict*)& _tmp12D->f2;_tmp130=_tmp12D->f3;_tmp131=_tmp12D->f4;_tmp132=_tmp12D->f5;_tmp133=_tmp12D->f6;_tmp134=_tmp12D->f7;{int*res=(int*)_tmp12E;struct Cyc_Dict_Dict*res_dic=(struct Cyc_Dict_Dict*)_tmp12F;struct Cyc_Dict_Dict dic1=_tmp130;struct Cyc_Dict_Dict excl=_tmp131;void*(*merge_f)(void*,void*,unsigned,struct _fat_ptr*)=_tmp132;struct _fat_ptr t=_tmp133;struct _fat_ptr*msg=_tmp134;
void*y;
{struct _handler_cons _tmp135;_push_handler(& _tmp135);{int _tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){
{void*_tmp138=((void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(dic1,x);void*y1=_tmp138;
# 781
void*_tmp139=merge_f(y1,y2,0U,msg);void*yt=_tmp139;
if(!((unsigned)yt)){
*res=0;
_npop_handler(0);return;}
# 786
y=yt;}
# 778
;_pop_handler();}else{void*_tmp136=(void*)Cyc_Core_get_exn_thrown();void*_tmp13A=_tmp136;void*_tmp13B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp13A)->tag == Cyc_Dict_Absent){
# 788
y=y2;
goto _LL3;}else{_tmp13B=_tmp13A;{void*exn=_tmp13B;(int)_rethrow(exn);}}_LL3:;}}}{
# 792
struct _handler_cons _tmp13C;_push_handler(& _tmp13C);{int _tmp13E=0;if(setjmp(_tmp13C.handler))_tmp13E=1;if(!_tmp13E){
{void*_tmp13F=((void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(excl,x);void*ye=_tmp13F;
# 797
void*_tmp140=merge_f(ye,y,0U,msg);void*yt=_tmp140;
if(yt != ye){
if((unsigned)yt)
({struct _fat_ptr*_tmp2C2=msg;Cyc_Interface_check_err(_tmp2C2,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp143=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0,_tmp204.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp204;});struct Cyc_String_pa_PrintArg_struct _tmp144=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0,({
struct _fat_ptr _tmp2C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(x));_tmp203.f1=_tmp2C0;});_tmp203;});void*_tmp141[2];_tmp141[0]=& _tmp143,_tmp141[1]=& _tmp144;({struct _fat_ptr _tmp2C1=({const char*_tmp142="abstract %s %s is being imported as non-abstract";_tag_fat(_tmp142,sizeof(char),49U);});Cyc_aprintf(_tmp2C1,_tag_fat(_tmp141,sizeof(void*),2));});}));});
*res=0;}}
# 793
;_pop_handler();}else{void*_tmp13D=(void*)Cyc_Core_get_exn_thrown();void*_tmp145=_tmp13D;void*_tmp146;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp145)->tag == Cyc_Dict_Absent){
# 805
if(*res)
({struct Cyc_Dict_Dict _tmp2C3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))Cyc_Dict_insert)(*res_dic,x,y);*res_dic=_tmp2C3;});
goto _LL8;}else{_tmp146=_tmp145;{void*exn=_tmp146;(int)_rethrow(exn);}}_LL8:;}}}}}
# 811
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned,struct _fat_ptr*),struct _fat_ptr t,struct _fat_ptr*msg){
# 819
struct _tuple18 _tmp147=({struct _tuple18 _tmp205;_tmp205.f1=1,_tmp205.f2=dic1,_tmp205.f3=dic1,_tmp205.f4=excl,_tmp205.f5=merge_f,_tmp205.f6=t,_tmp205.f7=msg;_tmp205;});struct _tuple18 env=_tmp147;
((void(*)(void(*)(struct _tuple18*,struct _tuple0*,void*),struct _tuple18*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& env,dic2);
return env.f1?({struct Cyc_Dict_Dict*_tmp148=_cycalloc(sizeof(*_tmp148));*_tmp148=env.f2;_tmp148;}): 0;}
# 829
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _fat_ptr*msg){
struct Cyc_Dict_Dict*_tmp149=({struct Cyc_Dict_Dict _tmp2C7=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2C6=ie2->aggrdecls;struct Cyc_Dict_Dict _tmp2C5=iexcl->aggrdecls;struct _fat_ptr _tmp2C4=({const char*_tmp155="type";_tag_fat(_tmp155,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2C7,_tmp2C6,_tmp2C5,Cyc_Tcdecl_merge_aggrdecl,_tmp2C4,msg);});struct Cyc_Dict_Dict*r1=_tmp149;
struct Cyc_Dict_Dict*_tmp14A=({struct Cyc_Dict_Dict _tmp2CB=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2CA=ie2->datatypedecls;struct Cyc_Dict_Dict _tmp2C9=iexcl->datatypedecls;struct _fat_ptr _tmp2C8=({const char*_tmp154="datatype";_tag_fat(_tmp154,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Datatypedecl*(*)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2CB,_tmp2CA,_tmp2C9,Cyc_Tcdecl_merge_datatypedecl,_tmp2C8,msg);});struct Cyc_Dict_Dict*r2=_tmp14A;
struct Cyc_Dict_Dict*_tmp14B=({struct Cyc_Dict_Dict _tmp2CF=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2CE=ie2->enumdecls;struct Cyc_Dict_Dict _tmp2CD=iexcl->enumdecls;struct _fat_ptr _tmp2CC=({const char*_tmp153="enum";_tag_fat(_tmp153,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2CF,_tmp2CE,_tmp2CD,Cyc_Tcdecl_merge_enumdecl,_tmp2CC,msg);});struct Cyc_Dict_Dict*r3=_tmp14B;
struct Cyc_Dict_Dict*_tmp14C=({struct Cyc_Dict_Dict _tmp2D3=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2D2=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2D1=iexcl->typedefdecls;struct _fat_ptr _tmp2D0=({const char*_tmp152="typedef";_tag_fat(_tmp152,sizeof(char),8U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2D3,_tmp2D2,_tmp2D1,Cyc_Tcdecl_merge_typedefdecl,_tmp2D0,msg);});struct Cyc_Dict_Dict*r4=_tmp14C;
struct Cyc_Dict_Dict*_tmp14D=({struct Cyc_Dict_Dict _tmp2D7=ie1->vardecls;struct Cyc_Dict_Dict _tmp2D6=ie2->vardecls;struct Cyc_Dict_Dict _tmp2D5=iexcl->vardecls;struct _fat_ptr _tmp2D4=({const char*_tmp151="variable";_tag_fat(_tmp151,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2D7,_tmp2D6,_tmp2D5,Cyc_Tcdecl_merge_vardecl,_tmp2D4,msg);});struct Cyc_Dict_Dict*r5=_tmp14D;
struct Cyc_Dict_Dict*_tmp14E=({struct Cyc_Dict_Dict _tmp2DB=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2DA=ie2->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2D9=iexcl->xdatatypefielddecls;struct _fat_ptr _tmp2D8=({const char*_tmp150="xdatatypefield";_tag_fat(_tmp150,sizeof(char),15U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Tcdecl_Xdatatypefielddecl*(*)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2DB,_tmp2DA,_tmp2D9,Cyc_Tcdecl_merge_xdatatypefielddecl,_tmp2D8,msg);});struct Cyc_Dict_Dict*r6=_tmp14E;
if(((((!((unsigned)r1)|| !((unsigned)r2))|| !((unsigned)r3))|| !((unsigned)r4))|| !((unsigned)r5))|| !((unsigned)r6))
return 0;
return({struct Cyc_Interface_Ienv*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->aggrdecls=*r1,_tmp14F->datatypedecls=*r2,_tmp14F->enumdecls=*r3,_tmp14F->typedefdecls=*r4,_tmp14F->vardecls=*r5,_tmp14F->xdatatypefielddecls=*r6;_tmp14F;});}struct _tuple19{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _fat_ptr f4;struct _fat_ptr*f5;};
# 842
void Cyc_Interface_disj_merge_dict_f(struct _tuple19*env,struct _tuple0*x,void*y){
# 845
struct _tuple19*_tmp156=env;void*_tmp15B;struct _fat_ptr _tmp15A;struct Cyc_Dict_Dict _tmp159;void*_tmp158;void*_tmp157;_tmp157=(int*)& _tmp156->f1;_tmp158=(struct Cyc_Dict_Dict*)& _tmp156->f2;_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;_tmp15B=_tmp156->f5;{int*res=(int*)_tmp157;struct Cyc_Dict_Dict*res_dic=(struct Cyc_Dict_Dict*)_tmp158;struct Cyc_Dict_Dict dic1=_tmp159;struct _fat_ptr t=_tmp15A;struct _fat_ptr*msg=_tmp15B;
if(((int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_member)(dic1,x)){
({struct _fat_ptr*_tmp2DE=msg;Cyc_Interface_check_err(_tmp2DE,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15E=({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0,_tmp207.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp207;});struct Cyc_String_pa_PrintArg_struct _tmp15F=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0,({
struct _fat_ptr _tmp2DC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(x));_tmp206.f1=_tmp2DC;});_tmp206;});void*_tmp15C[2];_tmp15C[0]=& _tmp15E,_tmp15C[1]=& _tmp15F;({struct _fat_ptr _tmp2DD=({const char*_tmp15D="%s %s is exported more than once";_tag_fat(_tmp15D,sizeof(char),33U);});Cyc_aprintf(_tmp2DD,_tag_fat(_tmp15C,sizeof(void*),2));});}));});
*res=0;}else{
if(*res)
({struct Cyc_Dict_Dict _tmp2DF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))Cyc_Dict_insert)(*res_dic,x,y);*res_dic=_tmp2DF;});}}}
# 854
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _fat_ptr t,struct _fat_ptr*msg){
# 858
struct _tuple19 _tmp160=({struct _tuple19 _tmp208;_tmp208.f1=1,_tmp208.f2=dic1,_tmp208.f3=dic1,_tmp208.f4=t,_tmp208.f5=msg;_tmp208;});struct _tuple19 env=_tmp160;
((void(*)(void(*)(struct _tuple19*,struct _tuple0*,void*),struct _tuple19*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& env,dic2);
return env.f1?({struct Cyc_Dict_Dict*_tmp161=_cycalloc(sizeof(*_tmp161));*_tmp161=env.f2;_tmp161;}): 0;}
# 863
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _fat_ptr*msg){
struct Cyc_Dict_Dict*_tmp162=({struct Cyc_Dict_Dict _tmp2E3=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2E2=ie2->aggrdecls;struct Cyc_Dict_Dict _tmp2E1=
((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 864
struct _fat_ptr _tmp2E0=({const char*_tmp16E="type";_tag_fat(_tmp16E,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2E3,_tmp2E2,_tmp2E1,Cyc_Tcdecl_merge_aggrdecl,_tmp2E0,msg);});struct Cyc_Dict_Dict*r1=_tmp162;
# 867
struct Cyc_Dict_Dict*_tmp163=({struct Cyc_Dict_Dict _tmp2E6=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2E5=ie2->datatypedecls;struct _fat_ptr _tmp2E4=({const char*_tmp16D="datatype";_tag_fat(_tmp16D,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_disj_merge_dict)(_tmp2E6,_tmp2E5,_tmp2E4,msg);});struct Cyc_Dict_Dict*r2=_tmp163;
struct Cyc_Dict_Dict*_tmp164=({struct Cyc_Dict_Dict _tmp2E9=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2E8=ie2->enumdecls;struct _fat_ptr _tmp2E7=({const char*_tmp16C="enum";_tag_fat(_tmp16C,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_disj_merge_dict)(_tmp2E9,_tmp2E8,_tmp2E7,msg);});struct Cyc_Dict_Dict*r3=_tmp164;
# 870
struct Cyc_Dict_Dict*_tmp165=({struct Cyc_Dict_Dict _tmp2ED=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2EC=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2EB=
((struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 870
struct _fat_ptr _tmp2EA=({const char*_tmp16B="typedef";_tag_fat(_tmp16B,sizeof(char),8U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*),struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_compat_merge_dict)(_tmp2ED,_tmp2EC,_tmp2EB,Cyc_Tcdecl_merge_typedefdecl,_tmp2EA,msg);});struct Cyc_Dict_Dict*r4=_tmp165;
# 873
struct Cyc_Dict_Dict*_tmp166=({struct Cyc_Dict_Dict _tmp2F0=ie1->vardecls;struct Cyc_Dict_Dict _tmp2EF=ie2->vardecls;struct _fat_ptr _tmp2EE=({const char*_tmp16A="variable";_tag_fat(_tmp16A,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_disj_merge_dict)(_tmp2F0,_tmp2EF,_tmp2EE,msg);});struct Cyc_Dict_Dict*r5=_tmp166;
struct Cyc_Dict_Dict*_tmp167=({struct Cyc_Dict_Dict _tmp2F3=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2F2=ie2->xdatatypefielddecls;struct _fat_ptr _tmp2F1=({const char*_tmp169="xdatatypefield";_tag_fat(_tmp169,sizeof(char),15U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict,struct _fat_ptr,struct _fat_ptr*))Cyc_Interface_disj_merge_dict)(_tmp2F3,_tmp2F2,_tmp2F1,msg);});struct Cyc_Dict_Dict*r6=_tmp167;
# 877
if(((((!((unsigned)r1)|| !((unsigned)r2))|| !((unsigned)r3))|| !((unsigned)r4))|| !((unsigned)r5))|| !((unsigned)r6))
return 0;
return({struct Cyc_Interface_Ienv*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->aggrdecls=*r1,_tmp168->datatypedecls=*r2,_tmp168->enumdecls=*r3,_tmp168->typedefdecls=*r4,_tmp168->vardecls=*r5,_tmp168->xdatatypefielddecls=*r6;_tmp168;});}
# 883
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info){
struct _handler_cons _tmp16F;_push_handler(& _tmp16F);{int _tmp171=0;if(setjmp(_tmp16F.handler))_tmp171=1;if(!_tmp171){
{struct _fat_ptr*msg=0;
# 887
if(info != 0)
msg=({struct _fat_ptr*_tmp176=_cycalloc(sizeof(*_tmp176));({struct _fat_ptr _tmp2F5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp174=({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0,_tmp20A.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f1);_tmp20A;});struct Cyc_String_pa_PrintArg_struct _tmp175=({struct Cyc_String_pa_PrintArg_struct _tmp209;_tmp209.tag=0,_tmp209.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f2);_tmp209;});void*_tmp172[2];_tmp172[0]=& _tmp174,_tmp172[1]=& _tmp175;({struct _fat_ptr _tmp2F4=({const char*_tmp173="merging exports of %s and %s,";_tag_fat(_tmp173,sizeof(char),30U);});Cyc_aprintf(_tmp2F4,_tag_fat(_tmp172,sizeof(void*),2));});});*_tmp176=_tmp2F5;});_tmp176;});{
# 891
struct Cyc_Interface_Ienv*_tmp177=Cyc_Interface_disj_merge_ienv(i1->exports,i2->exports,msg);struct Cyc_Interface_Ienv*exp=_tmp177;
if(exp == 0){struct Cyc_Interface_I*_tmp178=0;_npop_handler(0);return _tmp178;}
# 894
if(info != 0)
msg=({struct _fat_ptr*_tmp17D=_cycalloc(sizeof(*_tmp17D));({struct _fat_ptr _tmp2F7=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp20C;_tmp20C.tag=0,_tmp20C.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f1);_tmp20C;});struct Cyc_String_pa_PrintArg_struct _tmp17C=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0,_tmp20B.f1=(struct _fat_ptr)((struct _fat_ptr)(*info).f2);_tmp20B;});void*_tmp179[2];_tmp179[0]=& _tmp17B,_tmp179[1]=& _tmp17C;({struct _fat_ptr _tmp2F6=({const char*_tmp17A="merging imports of %s and %s,";_tag_fat(_tmp17A,sizeof(char),30U);});Cyc_aprintf(_tmp2F6,_tag_fat(_tmp179,sizeof(void*),2));});});*_tmp17D=_tmp2F7;});_tmp17D;});{
# 898
struct Cyc_Interface_Ienv*_tmp17E=Cyc_Interface_compat_merge_ienv(i1->imports,i2->imports,exp,msg);struct Cyc_Interface_Ienv*imp=_tmp17E;
if(imp == 0){struct Cyc_Interface_I*_tmp17F=0;_npop_handler(0);return _tmp17F;}{
# 901
struct Cyc_List_List*newtds=0;struct Cyc_List_List*l=i2->tds;
for(1;l != 0;l=l->tl){
newtds=({struct Cyc_List_List*_tmp181=_cycalloc(sizeof(*_tmp181));({struct Cyc_Absyn_Decl*_tmp2F8=({struct Cyc_Absyn_Decl*_tmp180=_cycalloc(sizeof(*_tmp180));*_tmp180=*((struct Cyc_Absyn_Decl*)l->hd);_tmp180;});_tmp181->hd=_tmp2F8;}),_tmp181->tl=newtds;_tmp181;});}
newtds=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,newtds,i1->tds));{
struct Cyc_Interface_I*_tmp183=({struct Cyc_Interface_I*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->imports=imp,_tmp182->exports=exp,_tmp182->tds=newtds;_tmp182;});_npop_handler(0);return _tmp183;}}}}}
# 885
;_pop_handler();}else{void*_tmp170=(void*)Cyc_Core_get_exn_thrown();void*_tmp184=_tmp170;void*_tmp185;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp184)->tag == Cyc_Tcdecl_Incompatible)
# 906
return 0;else{_tmp185=_tmp184;{void*exn=_tmp185;(int)_rethrow(exn);}};}}}
# 909
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 912
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _fat_ptr*curr_info=linfo != 0?(struct _fat_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 917
for(1;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp186=({struct Cyc_Interface_I*_tmp2FA=curr_i;struct Cyc_Interface_I*_tmp2F9=(struct Cyc_Interface_I*)li->hd;Cyc_Interface_merge(_tmp2FA,_tmp2F9,
curr_info != 0 && linfo != 0?({struct _tuple11*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->f1=*curr_info,_tmp18C->f2=*((struct _fat_ptr*)linfo->hd);_tmp18C;}): 0);});
# 918
struct Cyc_Interface_I*i=_tmp186;
# 920
if(i == 0)return 0;
curr_i=i;
if(curr_info != 0)
curr_info=linfo != 0?({struct _fat_ptr*_tmp18B=_cycalloc(sizeof(*_tmp18B));({struct _fat_ptr _tmp2FC=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0,_tmp20E.f1=(struct _fat_ptr)((struct _fat_ptr)*curr_info);_tmp20E;});struct Cyc_String_pa_PrintArg_struct _tmp18A=({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0,_tmp20D.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)linfo->hd));_tmp20D;});void*_tmp187[2];_tmp187[0]=& _tmp189,_tmp187[1]=& _tmp18A;({struct _fat_ptr _tmp2FB=({const char*_tmp188="%s+%s";_tag_fat(_tmp188,sizeof(char),6U);});Cyc_aprintf(_tmp2FB,_tag_fat(_tmp187,sizeof(void*),2));});});*_tmp18B=_tmp2FC;});_tmp18B;}): 0;
if(linfo != 0)linfo=linfo->tl;}
# 926
return curr_i;}}
# 929
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 932
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _fat_ptr*curr_info=linfo != 0?(struct _fat_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 937
for(1;la != 0;la=la->tl){
struct Cyc_Interface_I*_tmp18D=({struct Cyc_Interface_I*_tmp2FE=curr_i;struct Cyc_Interface_I*_tmp2FD=get(la->hd);Cyc_Interface_merge(_tmp2FE,_tmp2FD,
curr_info != 0 && linfo != 0?({struct _tuple11*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->f1=*curr_info,_tmp193->f2=*((struct _fat_ptr*)linfo->hd);_tmp193;}): 0);});
# 938
struct Cyc_Interface_I*i=_tmp18D;
# 940
if(i == 0)return 0;
curr_i=i;
if(curr_info != 0)
curr_info=linfo != 0?({struct _fat_ptr*_tmp192=_cycalloc(sizeof(*_tmp192));({struct _fat_ptr _tmp300=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp190=({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0,_tmp210.f1=(struct _fat_ptr)((struct _fat_ptr)*curr_info);_tmp210;});struct Cyc_String_pa_PrintArg_struct _tmp191=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0,_tmp20F.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)linfo->hd));_tmp20F;});void*_tmp18E[2];_tmp18E[0]=& _tmp190,_tmp18E[1]=& _tmp191;({struct _fat_ptr _tmp2FF=({const char*_tmp18F="%s+%s";_tag_fat(_tmp18F,sizeof(char),6U);});Cyc_aprintf(_tmp2FF,_tag_fat(_tmp18E,sizeof(void*),2));});});*_tmp192=_tmp300;});_tmp192;}): 0;
# 945
if(linfo != 0)linfo=linfo->tl;}
# 947
return curr_i;}}
# 953
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){
struct Cyc_List_List*_tmp194=0;struct Cyc_List_List*ans=_tmp194;
for(1;tds != 0;tds=tds->tl){
struct _tuple0*qv;
struct Cyc_Absyn_Decl*_tmp195=(struct Cyc_Absyn_Decl*)tds->hd;struct Cyc_Absyn_Decl*d=_tmp195;
{void*_tmp196=d->r;void*_stmttmpB=_tmp196;void*_tmp197=_stmttmpB;void*_tmp198;switch(*((int*)_tmp197)){case 0: _tmp198=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp197)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp198;
qv=vd->name;goto _LL0;}case 1: _tmp198=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp197)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp198;
qv=fd->name;goto _LL0;}case 5: _tmp198=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp197)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp198;
qv=ad->name;goto _LL0;}case 6: _tmp198=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp197)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp198;
qv=dd->name;goto _LL0;}case 7: _tmp198=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp197)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp198;
qv=ed->name;goto _LL0;}case 8: _tmp198=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp197)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp198;
qv=td->name;goto _LL0;}default:
 Cyc_Interface_err(({const char*_tmp199="bad decl form in Interface::add_namespace";_tag_fat(_tmp199,sizeof(char),42U);}));return 0;}_LL0:;}{
# 967
struct Cyc_List_List*vs;
{union Cyc_Absyn_Nmspace _tmp19A=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmpC=_tmp19A;union Cyc_Absyn_Nmspace _tmp19B=_stmttmpC;void*_tmp19C;switch((_tmp19B.C_n).tag){case 2: _tmp19C=(_tmp19B.Abs_n).val;{struct Cyc_List_List*x=_tmp19C;
vs=x;goto _LLF;}case 3: _tmp19C=(_tmp19B.C_n).val;{struct Cyc_List_List*x=_tmp19C;
vs=x;goto _LLF;}default:
 Cyc_Interface_err(({const char*_tmp19D="bad namespace in Interface::add_namespace";_tag_fat(_tmp19D,sizeof(char),42U);}));return 0;}_LLF:;}
# 973
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(vs);
({union Cyc_Absyn_Nmspace _tmp301=Cyc_Absyn_Rel_n(0);(*qv).f1=_tmp301;});
for(1;vs != 0;vs=vs->tl){
d=({void*_tmp303=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->tag=9,_tmp19F->f1=(struct _fat_ptr*)vs->hd,({struct Cyc_List_List*_tmp302=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->hd=d,_tmp19E->tl=0;_tmp19E;});_tmp19F->f2=_tmp302;});_tmp19F;});Cyc_Absyn_new_decl(_tmp303,d->loc);});}
ans=({struct Cyc_List_List*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=d,_tmp1A0->tl=ans;_tmp1A0;});}}
# 979
return((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ans);}
# 982
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));({struct Cyc_Absyn_Decl*_tmp304=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->tag=5,_tmp1A1->f1=d;_tmp1A1;}),0U);_tmp1A2->hd=_tmp304;}),_tmp1A2->tl=tds;_tmp1A2;});}
# 986
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 988
d=({struct Cyc_Absyn_Aggrdecl*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));*_tmp1A3=*d;_tmp1A3;});
d->impl=0;
if((int)d->sc != (int)Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({struct Cyc_Absyn_Decl*_tmp305=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->tag=5,_tmp1A4->f1=d;_tmp1A4;}),0U);_tmp1A5->hd=_tmp305;}),_tmp1A5->tl=tds;_tmp1A5;});}
# 994
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));({struct Cyc_Absyn_Decl*_tmp306=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->tag=6,_tmp1A6->f1=d;_tmp1A6;}),0U);_tmp1A7->hd=_tmp306;}),_tmp1A7->tl=tds;_tmp1A7;});}static char _tmp1A8[2U]="_";
# 998
static struct _fat_ptr Cyc_Interface_us={_tmp1A8,_tmp1A8,_tmp1A8 + 2U};
static struct _fat_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1001
static struct _tuple12*Cyc_Interface_rewrite_datatypefield_type(struct _tuple12*x){
return({struct _tuple12*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));({struct Cyc_Absyn_Tqual _tmp30A=Cyc_Absyn_empty_tqual(0U);_tmp1AB->f1=_tmp30A;}),({
void*_tmp309=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->tag=2,({struct Cyc_Absyn_Tvar*_tmp308=({struct Cyc_Absyn_Tvar*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->name=Cyc_Interface_us_p,_tmp1A9->identity=- 1,({void*_tmp307=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_tmp1A9->kind=_tmp307;});_tmp1A9;});_tmp1AA->f1=_tmp308;});_tmp1AA;});_tmp1AB->f2=_tmp309;});_tmp1AB;});}
# 1006
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
f=({struct Cyc_Absyn_Datatypefield*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));*_tmp1AC=*f;_tmp1AC;});
({struct Cyc_List_List*_tmp30B=((struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);f->typs=_tmp30B;});
return f;}
# 1012
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1014
d=({struct Cyc_Absyn_Datatypedecl*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));*_tmp1AD=*d;_tmp1AD;});
# 1016
if(d->fields != 0)({struct Cyc_Core_Opt*_tmp30D=({struct Cyc_Core_Opt*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));({struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v);_tmp1AE->v=_tmp30C;});_tmp1AE;});d->fields=_tmp30D;});
if((int)d->sc != (int)Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({struct Cyc_Absyn_Decl*_tmp30E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->tag=6,_tmp1AF->f1=d;_tmp1AF;}),0U);_tmp1B0->hd=_tmp30E;}),_tmp1B0->tl=tds;_tmp1B0;});}
# 1021
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));({struct Cyc_Absyn_Decl*_tmp30F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->tag=7,_tmp1B1->f1=d;_tmp1B1;}),0U);_tmp1B2->hd=_tmp30F;}),_tmp1B2->tl=tds;_tmp1B2;});}
# 1025
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1027
d=({struct Cyc_Absyn_Enumdecl*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));*_tmp1B3=*d;_tmp1B3;});
d->fields=0;
if((int)d->sc != (int)Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));({struct Cyc_Absyn_Decl*_tmp310=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->tag=7,_tmp1B4->f1=d;_tmp1B4;}),0U);_tmp1B5->hd=_tmp310;}),_tmp1B5->tl=tds;_tmp1B5;});}
# 1033
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1035
return({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({struct Cyc_Absyn_Decl*_tmp311=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->tag=8,_tmp1B6->f1=d;_tmp1B6;}),0U);_tmp1B7->hd=_tmp311;}),_tmp1B7->tl=tds;_tmp1B7;});}
# 1038
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({struct Cyc_Absyn_Decl*_tmp312=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->tag=0,_tmp1B8->f1=d;_tmp1B8;}),0U);_tmp1B9->hd=_tmp312;}),_tmp1B9->tl=tds;_tmp1B9;});}
# 1042
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1044
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1BA=d;void*_tmp1BC;void*_tmp1BB;_tmp1BB=_tmp1BA->base;_tmp1BC=_tmp1BA->field;{struct Cyc_Absyn_Datatypedecl*b=_tmp1BB;struct Cyc_Absyn_Datatypefield*f=_tmp1BC;
b=({struct Cyc_Absyn_Datatypedecl*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));*_tmp1BD=*b;_tmp1BD;});
({struct Cyc_Core_Opt*_tmp314=({struct Cyc_Core_Opt*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));({struct Cyc_List_List*_tmp313=({struct Cyc_List_List*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=f,_tmp1BE->tl=0;_tmp1BE;});_tmp1BF->v=_tmp313;});_tmp1BF;});b->fields=_tmp314;});
b->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));({struct Cyc_Absyn_Decl*_tmp315=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->tag=6,_tmp1C0->f1=b;_tmp1C0;}),0U);_tmp1C1->hd=_tmp315;}),_tmp1C1->tl=tds;_tmp1C1;});}}
# 1051
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1053
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1C2=d;void*_tmp1C4;void*_tmp1C3;_tmp1C3=_tmp1C2->base;_tmp1C4=_tmp1C2->field;{struct Cyc_Absyn_Datatypedecl*b=_tmp1C3;struct Cyc_Absyn_Datatypefield*f=_tmp1C4;
b=({struct Cyc_Absyn_Datatypedecl*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));*_tmp1C5=*b;_tmp1C5;});
f=Cyc_Interface_rewrite_datatypefield(f);
f->sc=Cyc_Absyn_Extern;
({struct Cyc_Core_Opt*_tmp317=({struct Cyc_Core_Opt*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({struct Cyc_List_List*_tmp316=({struct Cyc_List_List*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->hd=f,_tmp1C6->tl=0;_tmp1C6;});_tmp1C7->v=_tmp316;});_tmp1C7;});b->fields=_tmp317;});
b->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));({struct Cyc_Absyn_Decl*_tmp318=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->tag=6,_tmp1C8->f1=b;_tmp1C8;}),0U);_tmp1C9->hd=_tmp318;}),_tmp1C9->tl=tds;_tmp1C9;});}}
# 1062
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,0);struct Cyc_List_List*tds=_tmp1CA;
tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,tds);
tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,tds);
if(tds != 0)
({struct Cyc_List_List*_tmp319=Cyc_Interface_add_namespace(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(tds));Cyc_Absynpp_decllist2file(_tmp319,f);});}
# 1070
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1CB=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);struct Cyc_List_List*tds=_tmp1CB;
if(tds != 0)
({struct Cyc_List_List*_tmp31A=Cyc_Interface_add_namespace(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(tds));Cyc_Absynpp_decllist2file(_tmp31A,f);});}
# 1076
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);struct Cyc_List_List*tds=_tmp1CC;
if(tds != 0)
({struct Cyc_List_List*_tmp31B=Cyc_Interface_add_namespace(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(tds));Cyc_Absynpp_decllist2file(_tmp31B,f);});}
# 1082
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,0);struct Cyc_List_List*tds=_tmp1CD;
tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,tds);
tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,tds);
tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,tds);
tds=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,tds);
if(tds != 0)
({struct Cyc_List_List*_tmp31C=Cyc_Interface_add_namespace(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(tds));Cyc_Absynpp_decllist2file(_tmp31C,f);});}
# 1095
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
({void*_tmp1CE=0U;({struct Cyc___cycFILE*_tmp31E=f;struct _fat_ptr _tmp31D=({const char*_tmp1CF="/****** needed (headers) ******/\n";_tag_fat(_tmp1CF,sizeof(char),34U);});Cyc_fprintf(_tmp31E,_tmp31D,_tag_fat(_tmp1CE,sizeof(void*),0));});});
Cyc_Interface_print_ns_headers(f,i->imports);
# 1100
({void*_tmp1D0=0U;({struct Cyc___cycFILE*_tmp320=f;struct _fat_ptr _tmp31F=({const char*_tmp1D1="\n/****** provided (headers) ******/\n";_tag_fat(_tmp1D1,sizeof(char),37U);});Cyc_fprintf(_tmp320,_tmp31F,_tag_fat(_tmp1D0,sizeof(void*),0));});});
Cyc_Interface_print_ns_headers(f,i->exports);
# 1105
({void*_tmp1D2=0U;({struct Cyc___cycFILE*_tmp322=f;struct _fat_ptr _tmp321=({const char*_tmp1D3="\n/****** needed (headers of xdatatypefielddecls) ******/\n";_tag_fat(_tmp1D3,sizeof(char),58U);});Cyc_fprintf(_tmp322,_tmp321,_tag_fat(_tmp1D2,sizeof(void*),0));});});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->imports);
# 1108
({void*_tmp1D4=0U;({struct Cyc___cycFILE*_tmp324=f;struct _fat_ptr _tmp323=({const char*_tmp1D5="\n/****** provided (headers of xdatatypefielddecls) ******/\n";_tag_fat(_tmp1D5,sizeof(char),60U);});Cyc_fprintf(_tmp324,_tmp323,_tag_fat(_tmp1D4,sizeof(void*),0));});});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->exports);
# 1112
({void*_tmp1D6=0U;({struct Cyc___cycFILE*_tmp326=f;struct _fat_ptr _tmp325=({const char*_tmp1D7="\n/****** provided (typedefs) ******/\n";_tag_fat(_tmp1D7,sizeof(char),38U);});Cyc_fprintf(_tmp326,_tmp325,_tag_fat(_tmp1D6,sizeof(void*),0));});});
Cyc_Interface_print_ns_typedefs(f,i->exports);
# 1115
({void*_tmp1D8=0U;({struct Cyc___cycFILE*_tmp328=f;struct _fat_ptr _tmp327=({const char*_tmp1D9="\n/****** needed (declarations) ******/\n";_tag_fat(_tmp1D9,sizeof(char),40U);});Cyc_fprintf(_tmp328,_tmp327,_tag_fat(_tmp1D8,sizeof(void*),0));});});
Cyc_Interface_print_ns_decls(f,i->imports);
# 1118
({void*_tmp1DA=0U;({struct Cyc___cycFILE*_tmp32A=f;struct _fat_ptr _tmp329=({const char*_tmp1DB="\n/****** provided (declarations) ******/\n";_tag_fat(_tmp1DB,sizeof(char),42U);});Cyc_fprintf(_tmp32A,_tmp329,_tag_fat(_tmp1DA,sizeof(void*),0));});});
Cyc_Interface_print_ns_decls(f,i->exports);}
# 1125
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp1DC=Cyc_Parse_parse_file(f);struct Cyc_List_List*tds=_tmp1DC;
Cyc_Binding_resolve_all(tds);{
struct Cyc_Tcenv_Tenv*_tmp1DD=Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Tenv*te=_tmp1DD;
Cyc_Tc_tc(te,0,tds);
return Cyc_Interface_gen_extract(te->ae,0,tds);}}}
# 1138
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1144
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
