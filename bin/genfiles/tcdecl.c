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
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 232 "cycboot.h"
extern struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);struct Cyc_Position_Error;
# 43 "position.h"
extern struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned,struct _fat_ptr);
# 49
extern void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 858
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 860
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 128 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 79 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_force_kb(void*);
# 54 "attributes.h"
int Cyc_Atts_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 75
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 38 "tcdecl.h"
void Cyc_Tcdecl_merr(unsigned,struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr ap);
# 61 "tcdecl.h"
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*,int*,struct _fat_ptr*,unsigned,struct _fat_ptr*);char Cyc_Tcdecl_Incompatible[13U]="Incompatible";
# 37 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
# 39
void Cyc_Tcdecl_merr(unsigned loc,struct _fat_ptr*msg1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 43
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _fat_ptr fmt2;
if(Cyc_strlen((struct _fat_ptr)*msg1)== 0U)
fmt2=(struct _fat_ptr)({struct _fat_ptr _tmp1CA=({const char*_tmp0="%s";_tag_fat(_tmp0,sizeof(char),3U);});Cyc_strconcat(_tmp1CA,(struct _fat_ptr)fmt);});else{
# 49
fmt2=(struct _fat_ptr)({struct _fat_ptr _tmp1CB=({const char*_tmp1="%s ";_tag_fat(_tmp1,sizeof(char),4U);});Cyc_strconcat(_tmp1CB,(struct _fat_ptr)fmt);});}{
struct _fat_ptr ap2=({unsigned _tmp4=_get_fat_size(ap,sizeof(void*))+ 1U;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned _tmp161=_get_fat_size(ap,sizeof(void*))+ 1U;unsigned i;for(i=0;i < _tmp161;++ i){
i == 0U?({void*_tmp1CC=(void*)({struct Cyc_String_pa_PrintArg_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=0,_tmp2->f1=(struct _fat_ptr)((struct _fat_ptr)*msg1);_tmp2;});_tmp3[i]=_tmp1CC;}):(_tmp3[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - 1U))));}}0;});_tag_fat(_tmp3,sizeof(void*),_tmp4);});
Cyc_Position_post_error(({unsigned _tmp1CD=loc;Cyc_Position_mk_err(_tmp1CD,(struct _fat_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2));}));}}}
# 57
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 59
struct _fat_ptr s0_str=(int)s0 == 2?({const char*_tmpC="public ";_tag_fat(_tmpC,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s0);
struct _fat_ptr s1_str=(int)s1 == 2?({const char*_tmpB="public ";_tag_fat(_tmpB,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s1);
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0,_tmp165.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp165;});struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0,_tmp164.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp164;});struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0,_tmp163.f1=(struct _fat_ptr)((struct _fat_ptr)s1_str);_tmp163;});struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0,_tmp162.f1=(struct _fat_ptr)((struct _fat_ptr)s0_str);_tmp162;});void*_tmp5[4];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9,_tmp5[3]=& _tmpA;({unsigned _tmp1D0=loc;struct _fat_ptr*_tmp1CF=msg;struct _fat_ptr _tmp1CE=({const char*_tmp6="%s %s is %sbut expected scope is %s";_tag_fat(_tmp6,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp1D0,_tmp1CF,_tmp1CE,_tag_fat(_tmp5,sizeof(void*),4));});});}struct _tuple11{enum Cyc_Absyn_Scope f1;int f2;};struct _tuple12{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 65
struct _tuple11 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg,int externCmerge){
# 67
{struct _tuple12 _tmpD=({struct _tuple12 _tmp168;_tmp168.f1=s0,_tmp168.f2=s1;_tmp168;});struct _tuple12 _stmttmp0=_tmpD;struct _tuple12 _tmpE=_stmttmp0;switch((int)_tmpE.f1){case Cyc_Absyn_ExternC: switch((int)_tmpE.f2){case Cyc_Absyn_ExternC:
# 69
 goto _LL0;case Cyc_Absyn_Public:
# 71
 goto _LL6;case Cyc_Absyn_Extern: _LL6:
 goto _LL8;default: _LLC:
# 75
 goto _LLE;}case Cyc_Absyn_Public: switch((int)_tmpE.f2){case Cyc_Absyn_ExternC: _LL8:
# 73
 goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL16:
# 82
 goto _LL18;default: goto _LL1B;}case Cyc_Absyn_Extern: switch((int)_tmpE.f2){case Cyc_Absyn_ExternC: _LLA:
# 74
 if(externCmerge)goto _LL0;else{goto _LLC;}case Cyc_Absyn_Extern: goto _LLF;default:
# 79
 goto _LL0;}default: switch((int)_tmpE.f2){case Cyc_Absyn_ExternC: _LLE:
# 76
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple11 _tmp166;_tmp166.f1=s1,_tmp166.f2=0;_tmp166;});case Cyc_Absyn_Extern: _LLF:
# 78
 s1=s0;goto _LL0;default: switch((int)_tmpE.f1){case Cyc_Absyn_Static: if(_tmpE.f2 == Cyc_Absyn_Static)
# 81
goto _LL16;else{goto _LL1B;}case Cyc_Absyn_Abstract: if(_tmpE.f2 == Cyc_Absyn_Abstract){_LL18:
# 83
 goto _LL1A;}else{goto _LL1B;}case Cyc_Absyn_Register: if(_tmpE.f2 == Cyc_Absyn_Register){_LL1A:
 goto _LL0;}else{goto _LL1B;}default: _LL1B:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple11 _tmp167;_tmp167.f1=s1,_tmp167.f2=0;_tmp167;});}}}_LL0:;}
# 87
return({struct _tuple11 _tmp169;_tmp169.f1=s1,_tmp169.f2=1;_tmp169;});}
# 90
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 92
return Cyc_Unify_unify(t0,t1);}
# 96
static unsigned Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_stmttmp1=_tmpF;void*_tmp10=_stmttmp1;int _tmp11;if(*((int*)_tmp10)== 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Int_c).tag == 5){_tmp11=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Int_c).val).f2;{int x=_tmp11;
return(unsigned)x;}}else{goto _LL3;}}else{_LL3:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp1D1=({const char*_tmp12="Tcdecl::get_uint_const_value";_tag_fat(_tmp12,sizeof(char),29U);});_tmp13->f1=_tmp1D1;});_tmp13;}));};}struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 103
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 105
struct Cyc_List_List*inst=0;
for(1;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
inst=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct _tuple13*_tmp1D3=({struct _tuple13*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,({void*_tmp1D2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=2,_tmp14->f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp14;});_tmp15->f2=_tmp1D2;});_tmp15;});_tmp16->hd=_tmp1D3;}),_tmp16->tl=inst;_tmp16;});}
return inst;}
# 111
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 114
if(({int _tmp1D4=((int(*)(struct Cyc_List_List*))Cyc_List_length)(tvs0);_tmp1D4 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(tvs1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0,_tmp16B.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp16B;});struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0,_tmp16A.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp16A;});void*_tmp17[2];_tmp17[0]=& _tmp19,_tmp17[1]=& _tmp1A;({unsigned _tmp1D7=loc;struct _fat_ptr*_tmp1D6=msg;struct _fat_ptr _tmp1D5=({const char*_tmp18="%s %s has a different number of type parameters";_tag_fat(_tmp18,sizeof(char),48U);});Cyc_Tcdecl_merr(_tmp1D7,_tmp1D6,_tmp1D5,_tag_fat(_tmp17,sizeof(void*),2));});});
return 0;}{
# 118
struct Cyc_List_List*_tmp1B=tvs0;struct Cyc_List_List*x0=_tmp1B;
struct Cyc_List_List*_tmp1C=tvs1;struct Cyc_List_List*x1=_tmp1C;
for(1;x0 != 0;(x0=x0->tl,x1=x1->tl)){
Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)x0->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(x1))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp1D=Cyc_Kinds_force_kb(((struct Cyc_Absyn_Tvar*)x0->hd)->kind);struct Cyc_Absyn_Kind*c0=_tmp1D;
struct Cyc_Absyn_Kind*_tmp1E=Cyc_Kinds_force_kb(((struct Cyc_Absyn_Tvar*)x1->hd)->kind);struct Cyc_Absyn_Kind*c1=_tmp1E;
if(c0 == c1)continue;
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0,_tmp170.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp170;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0,_tmp16F.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp16F;});struct Cyc_String_pa_PrintArg_struct _tmp23=({struct Cyc_String_pa_PrintArg_struct _tmp16E;_tmp16E.tag=0,({
struct _fat_ptr _tmp1D8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(c0));_tmp16E.f1=_tmp1D8;});_tmp16E;});struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp16D;_tmp16D.tag=0,_tmp16D.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)x0->hd)->name);_tmp16D;});struct Cyc_String_pa_PrintArg_struct _tmp25=({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0,({struct _fat_ptr _tmp1D9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(c1));_tmp16C.f1=_tmp1D9;});_tmp16C;});void*_tmp1F[5];_tmp1F[0]=& _tmp21,_tmp1F[1]=& _tmp22,_tmp1F[2]=& _tmp23,_tmp1F[3]=& _tmp24,_tmp1F[4]=& _tmp25;({unsigned _tmp1DC=loc;struct _fat_ptr*_tmp1DB=msg;struct _fat_ptr _tmp1DA=({const char*_tmp20="%s %s has a different kind (%s) for type parameter %s (%s)";_tag_fat(_tmp20,sizeof(char),59U);});Cyc_Tcdecl_merr(_tmp1DC,_tmp1DB,_tmp1DA,_tag_fat(_tmp1F,sizeof(void*),5));});});
return 0;}}
# 129
return 1;}}
# 132
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 134
if(Cyc_Atts_same_atts(atts0,atts1))
return 1;
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0,_tmp172.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp172;});struct Cyc_String_pa_PrintArg_struct _tmp29=({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0,_tmp171.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp171;});void*_tmp26[2];_tmp26[0]=& _tmp28,_tmp26[1]=& _tmp29;({unsigned _tmp1DF=loc;struct _fat_ptr*_tmp1DE=msg;struct _fat_ptr _tmp1DD=({const char*_tmp27="%s %s has different attributes";_tag_fat(_tmp27,sizeof(char),31U);});Cyc_Tcdecl_merr(_tmp1DF,_tmp1DE,_tmp1DD,_tag_fat(_tmp26,sizeof(void*),2));});});
return 0;}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple15{void*f1;void*f2;};
# 141
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned loc,struct _fat_ptr*msg){
# 143
struct _fat_ptr _tmp2A=Cyc_Absynpp_qvar2string(d0->name);struct _fat_ptr v=_tmp2A;
int _tmp2B=1;int res=_tmp2B;
# 149
if(!((int)d0->kind == (int)d1->kind))return 0;
# 152
if(!({struct Cyc_List_List*_tmp1E4=d0->tvs;struct Cyc_List_List*_tmp1E3=d1->tvs;struct _fat_ptr _tmp1E2=({const char*_tmp2C="type";_tag_fat(_tmp2C,sizeof(char),5U);});struct _fat_ptr _tmp1E1=v;unsigned _tmp1E0=loc;Cyc_Tcdecl_check_tvs(_tmp1E4,_tmp1E3,_tmp1E2,_tmp1E1,_tmp1E0,msg);}))return 0;{
# 155
struct _tuple11 _tmp2D=({
enum Cyc_Absyn_Scope _tmp1E9=d0->sc;enum Cyc_Absyn_Scope _tmp1E8=d1->sc;struct _fat_ptr _tmp1E7=({const char*_tmp83="type";_tag_fat(_tmp83,sizeof(char),5U);});struct _fat_ptr _tmp1E6=v;unsigned _tmp1E5=loc;Cyc_Tcdecl_merge_scope(_tmp1E9,_tmp1E8,_tmp1E7,_tmp1E6,_tmp1E5,msg,1);});
# 155
struct _tuple11 _stmttmp2=_tmp2D;struct _tuple11 _tmp2E=_stmttmp2;int _tmp30;enum Cyc_Absyn_Scope _tmp2F;_tmp2F=_tmp2E.f1;_tmp30=_tmp2E.f2;{enum Cyc_Absyn_Scope new_scope=_tmp2F;int res_scope=_tmp30;
# 157
if(!res_scope)res=0;
# 160
if(!({struct Cyc_List_List*_tmp1EE=d0->attributes;struct Cyc_List_List*_tmp1ED=d1->attributes;struct _fat_ptr _tmp1EC=({const char*_tmp31="type";_tag_fat(_tmp31,sizeof(char),5U);});struct _fat_ptr _tmp1EB=v;unsigned _tmp1EA=loc;Cyc_Tcdecl_check_atts(_tmp1EE,_tmp1ED,_tmp1EC,_tmp1EB,_tmp1EA,msg);}))res=0;{
# 162
struct Cyc_Absyn_Aggrdecl*d2;
# 164
{struct _tuple14 _tmp32=({struct _tuple14 _tmp18D;_tmp18D.f1=d0->impl,_tmp18D.f2=d1->impl;_tmp18D;});struct _tuple14 _stmttmp3=_tmp32;struct _tuple14 _tmp33=_stmttmp3;int _tmp3B;void*_tmp3A;void*_tmp39;void*_tmp38;int _tmp37;void*_tmp36;void*_tmp35;void*_tmp34;if(_tmp33.f2 == 0){
d2=d0;goto _LL3;}else{if(_tmp33.f1 == 0){
d2=d1;goto _LL3;}else{_tmp34=(_tmp33.f1)->exist_vars;_tmp35=(_tmp33.f1)->rgn_po;_tmp36=(_tmp33.f1)->fields;_tmp37=(_tmp33.f1)->tagged;_tmp38=(_tmp33.f2)->exist_vars;_tmp39=(_tmp33.f2)->rgn_po;_tmp3A=(_tmp33.f2)->fields;_tmp3B=(_tmp33.f2)->tagged;{struct Cyc_List_List*exist_vars0=_tmp34;struct Cyc_List_List*rgn_po0=_tmp35;struct Cyc_List_List*f0s=_tmp36;int tagged0=_tmp37;struct Cyc_List_List*exist_vars1=_tmp38;struct Cyc_List_List*rgn_po1=_tmp39;struct Cyc_List_List*f1s=_tmp3A;int tagged1=_tmp3B;
# 171
if(!({struct Cyc_List_List*_tmp1F3=exist_vars0;struct Cyc_List_List*_tmp1F2=exist_vars1;struct _fat_ptr _tmp1F1=({const char*_tmp3C="type";_tag_fat(_tmp3C,sizeof(char),5U);});struct _fat_ptr _tmp1F0=v;unsigned _tmp1EF=loc;Cyc_Tcdecl_check_tvs(_tmp1F3,_tmp1F2,_tmp1F1,_tmp1F0,_tmp1EF,msg);}))
return 0;{
# 175
struct Cyc_List_List*_tmp3D=({struct Cyc_List_List*_tmp1F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(d0->tvs,exist_vars0);Cyc_Tcdecl_build_tvs_map(_tmp1F4,
((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(d1->tvs,exist_vars1));});
# 175
struct Cyc_List_List*inst=_tmp3D;
# 180
for(1;rgn_po0 != 0 && rgn_po1 != 0;(
rgn_po0=rgn_po0->tl,rgn_po1=rgn_po1->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple15*)rgn_po0->hd)).f1,(*((struct _tuple15*)rgn_po1->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple15*)rgn_po0->hd)).f2,(*((struct _tuple15*)rgn_po1->hd)).f2);}
# 187
for(1;f0s != 0 && f1s != 0;(f0s=f0s->tl,f1s=f1s->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3E=(struct Cyc_Absyn_Aggrfield*)f0s->hd;struct Cyc_Absyn_Aggrfield*_stmttmp4=_tmp3E;struct Cyc_Absyn_Aggrfield*_tmp3F=_stmttmp4;void*_tmp45;void*_tmp44;void*_tmp43;void*_tmp42;struct Cyc_Absyn_Tqual _tmp41;void*_tmp40;_tmp40=_tmp3F->name;_tmp41=_tmp3F->tq;_tmp42=_tmp3F->type;_tmp43=_tmp3F->width;_tmp44=_tmp3F->attributes;_tmp45=_tmp3F->requires_clause;{struct _fat_ptr*fn0=_tmp40;struct Cyc_Absyn_Tqual tq0=_tmp41;void*t0=_tmp42;struct Cyc_Absyn_Exp*width0=_tmp43;struct Cyc_List_List*atts0=_tmp44;struct Cyc_Absyn_Exp*req0=_tmp45;
struct Cyc_Absyn_Aggrfield*_tmp46=(struct Cyc_Absyn_Aggrfield*)f1s->hd;struct Cyc_Absyn_Aggrfield*_stmttmp5=_tmp46;struct Cyc_Absyn_Aggrfield*_tmp47=_stmttmp5;void*_tmp4D;void*_tmp4C;void*_tmp4B;void*_tmp4A;struct Cyc_Absyn_Tqual _tmp49;void*_tmp48;_tmp48=_tmp47->name;_tmp49=_tmp47->tq;_tmp4A=_tmp47->type;_tmp4B=_tmp47->width;_tmp4C=_tmp47->attributes;_tmp4D=_tmp47->requires_clause;{struct _fat_ptr*fn1=_tmp48;struct Cyc_Absyn_Tqual tq1=_tmp49;void*t1=_tmp4A;struct Cyc_Absyn_Exp*width1=_tmp4B;struct Cyc_List_List*atts1=_tmp4C;struct Cyc_Absyn_Exp*req1=_tmp4D;
# 191
if(Cyc_strptrcmp(fn0,fn1)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp176;_tmp176.tag=0,({struct _fat_ptr _tmp1F5=(struct _fat_ptr)({const char*_tmp54="type";_tag_fat(_tmp54,sizeof(char),5U);});_tmp176.f1=_tmp1F5;});_tmp176;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0,_tmp175.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp175;});struct Cyc_String_pa_PrintArg_struct _tmp52=({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0,_tmp174.f1=(struct _fat_ptr)((struct _fat_ptr)*fn0);_tmp174;});struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0,_tmp173.f1=(struct _fat_ptr)((struct _fat_ptr)*fn1);_tmp173;});void*_tmp4E[4];_tmp4E[0]=& _tmp50,_tmp4E[1]=& _tmp51,_tmp4E[2]=& _tmp52,_tmp4E[3]=& _tmp53;({unsigned _tmp1F8=loc;struct _fat_ptr*_tmp1F7=msg;struct _fat_ptr _tmp1F6=({const char*_tmp4F="%s %s : field name mismatch %s != %s";_tag_fat(_tmp4F,sizeof(char),37U);});Cyc_Tcdecl_merr(_tmp1F8,_tmp1F7,_tmp1F6,_tag_fat(_tmp4E,sizeof(void*),4));});});
# 194
return 0;}
# 196
if(!Cyc_Atts_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp57=({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0,({struct _fat_ptr _tmp1F9=(struct _fat_ptr)({const char*_tmp5A="type";_tag_fat(_tmp5A,sizeof(char),5U);});_tmp179.f1=_tmp1F9;});_tmp179;});struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0,_tmp178.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp178;});struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0,_tmp177.f1=(struct _fat_ptr)((struct _fat_ptr)*fn0);_tmp177;});void*_tmp55[3];_tmp55[0]=& _tmp57,_tmp55[1]=& _tmp58,_tmp55[2]=& _tmp59;({unsigned _tmp1FC=loc;struct _fat_ptr*_tmp1FB=msg;struct _fat_ptr _tmp1FA=({const char*_tmp56="%s %s : attribute mismatch on field %s";_tag_fat(_tmp56,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp1FC,_tmp1FB,_tmp1FA,_tag_fat(_tmp55,sizeof(void*),3));});});
res=0;}
# 200
if(!Cyc_Absyn_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp17C;_tmp17C.tag=0,({struct _fat_ptr _tmp1FD=(struct _fat_ptr)({const char*_tmp60="type";_tag_fat(_tmp60,sizeof(char),5U);});_tmp17C.f1=_tmp1FD;});_tmp17C;});struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp17B;_tmp17B.tag=0,_tmp17B.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp17B;});struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0,_tmp17A.f1=(struct _fat_ptr)((struct _fat_ptr)*fn0);_tmp17A;});void*_tmp5B[3];_tmp5B[0]=& _tmp5D,_tmp5B[1]=& _tmp5E,_tmp5B[2]=& _tmp5F;({unsigned _tmp200=loc;struct _fat_ptr*_tmp1FF=msg;struct _fat_ptr _tmp1FE=({const char*_tmp5C="%s %s : qualifier mismatch on field %s";_tag_fat(_tmp5C,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp200,_tmp1FF,_tmp1FE,_tag_fat(_tmp5B,sizeof(void*),3));});});
res=0;}
# 204
if(((width0 != 0 && width1 != 0)&&({
unsigned _tmp201=Cyc_Tcdecl_get_uint_const_value(width0);_tmp201 != 
Cyc_Tcdecl_get_uint_const_value(width1);})||
 width0 == 0 && width1 != 0)||
 width0 != 0 && width1 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp17F;_tmp17F.tag=0,({struct _fat_ptr _tmp202=(struct _fat_ptr)({const char*_tmp66="type";_tag_fat(_tmp66,sizeof(char),5U);});_tmp17F.f1=_tmp202;});_tmp17F;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp17E;_tmp17E.tag=0,_tmp17E.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp17E;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp17D;_tmp17D.tag=0,_tmp17D.f1=(struct _fat_ptr)((struct _fat_ptr)*fn0);_tmp17D;});void*_tmp61[3];_tmp61[0]=& _tmp63,_tmp61[1]=& _tmp64,_tmp61[2]=& _tmp65;({unsigned _tmp205=loc;struct _fat_ptr*_tmp204=msg;struct _fat_ptr _tmp203=({const char*_tmp62="%s %s : bitfield mismatch on field %s";_tag_fat(_tmp62,sizeof(char),38U);});Cyc_Tcdecl_merr(_tmp205,_tmp204,_tmp203,_tag_fat(_tmp61,sizeof(void*),3));});});
res=0;}
# 212
if(req0 != 0 && req1 != 0){
void*subst_w1=({struct Cyc_List_List*_tmp206=inst;Cyc_Tcutil_substitute(_tmp206,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=9,_tmp6D->f1=req1;_tmp6D;}));});
if(!({void*_tmp207=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=9,_tmp67->f1=req0;_tmp67;});Cyc_Tcdecl_check_type(_tmp207,subst_w1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmp182;_tmp182.tag=0,_tmp182.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp182;});struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp181;_tmp181.tag=0,({
struct _fat_ptr _tmp208=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(req0));_tmp181.f1=_tmp208;});_tmp181;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp180;_tmp180.tag=0,({struct _fat_ptr _tmp209=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(req1));_tmp180.f1=_tmp209;});_tmp180;});void*_tmp68[3];_tmp68[0]=& _tmp6A,_tmp68[1]=& _tmp6B,_tmp68[2]=& _tmp6C;({unsigned _tmp20C=loc;struct _fat_ptr*_tmp20B=msg;struct _fat_ptr _tmp20A=({const char*_tmp69="type %s : mismatch on @requires clauses: %s != %s";_tag_fat(_tmp69,sizeof(char),50U);});Cyc_Tcdecl_merr(_tmp20C,_tmp20B,_tmp20A,_tag_fat(_tmp68,sizeof(void*),3));});});
res=0;}}else{
# 219
if(req0 == 0 && req1 != 0 ||
 req0 != 0 && req1 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp183;_tmp183.tag=0,_tmp183.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp183;});void*_tmp6E[1];_tmp6E[0]=& _tmp70;({unsigned _tmp20F=loc;struct _fat_ptr*_tmp20E=msg;struct _fat_ptr _tmp20D=({const char*_tmp6F="type %s : mismatch on @requires clauses";_tag_fat(_tmp6F,sizeof(char),40U);});Cyc_Tcdecl_merr(_tmp20F,_tmp20E,_tmp20D,_tag_fat(_tmp6E,sizeof(void*),1));});});
res=0;}}{
# 225
void*subst_t1=Cyc_Tcutil_substitute(inst,t1);
if(!Cyc_Tcdecl_check_type(t0,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp187;_tmp187.tag=0,_tmp187.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp187;});struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp186;_tmp186.tag=0,_tmp186.f1=(struct _fat_ptr)((struct _fat_ptr)*fn0);_tmp186;});struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0,({
struct _fat_ptr _tmp210=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t0));_tmp185.f1=_tmp210;});_tmp185;});struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0,({struct _fat_ptr _tmp211=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(subst_t1));_tmp184.f1=_tmp211;});_tmp184;});void*_tmp71[4];_tmp71[0]=& _tmp73,_tmp71[1]=& _tmp74,_tmp71[2]=& _tmp75,_tmp71[3]=& _tmp76;({unsigned _tmp214=loc;struct _fat_ptr*_tmp213=msg;struct _fat_ptr _tmp212=({const char*_tmp72="type %s : type mismatch on field %s: %s != %s";_tag_fat(_tmp72,sizeof(char),46U);});Cyc_Tcdecl_merr(_tmp214,_tmp213,_tmp212,_tag_fat(_tmp71,sizeof(void*),4));});});
Cyc_Unify_explain_failure();
res=0;}}}}}
# 234
if(f0s != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp189;_tmp189.tag=0,_tmp189.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp189;});struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp188;_tmp188.tag=0,_tmp188.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)f0s->hd)->name);_tmp188;});void*_tmp77[2];_tmp77[0]=& _tmp79,_tmp77[1]=& _tmp7A;({unsigned _tmp217=loc;struct _fat_ptr*_tmp216=msg;struct _fat_ptr _tmp215=({const char*_tmp78="type %s is missing field %s";_tag_fat(_tmp78,sizeof(char),28U);});Cyc_Tcdecl_merr(_tmp217,_tmp216,_tmp215,_tag_fat(_tmp77,sizeof(void*),2));});});
res=0;}
# 238
if(f1s != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7D=({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0,_tmp18B.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp18B;});struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp18A;_tmp18A.tag=0,_tmp18A.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)f1s->hd)->name);_tmp18A;});void*_tmp7B[2];_tmp7B[0]=& _tmp7D,_tmp7B[1]=& _tmp7E;({unsigned _tmp21A=loc;struct _fat_ptr*_tmp219=msg;struct _fat_ptr _tmp218=({const char*_tmp7C="type %s has extra field %s";_tag_fat(_tmp7C,sizeof(char),27U);});Cyc_Tcdecl_merr(_tmp21A,_tmp219,_tmp218,_tag_fat(_tmp7B,sizeof(void*),2));});});
res=0;}
# 243
if(tagged0 != tagged1){
({struct Cyc_String_pa_PrintArg_struct _tmp81=({struct Cyc_String_pa_PrintArg_struct _tmp18C;_tmp18C.tag=0,_tmp18C.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp18C;});void*_tmp7F[1];_tmp7F[0]=& _tmp81;({unsigned _tmp21D=loc;struct _fat_ptr*_tmp21C=msg;struct _fat_ptr _tmp21B=({const char*_tmp80="%s : previous declaration disagrees with @tagged qualifier";_tag_fat(_tmp80,sizeof(char),59U);});Cyc_Tcdecl_merr(_tmp21D,_tmp21C,_tmp21B,_tag_fat(_tmp7F,sizeof(void*),1));});});
res=0;}
# 248
d2=d0;
goto _LL3;}}}}_LL3:;}
# 252
if(!res)return 0;
if((int)new_scope == (int)d2->sc)return d2;
d2=({struct Cyc_Absyn_Aggrdecl*_tmp82=_cycalloc(sizeof(*_tmp82));*_tmp82=*d2;_tmp82;});
d2->sc=new_scope;
return d2;}}}}
# 259
inline static struct _fat_ptr Cyc_Tcdecl_is_x2string(int is_x){
return is_x?({const char*_tmp84="@extensible datatype";_tag_fat(_tmp84,sizeof(char),21U);}):({const char*_tmp85="datatype";_tag_fat(_tmp85,sizeof(char),9U);});}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 265
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _fat_ptr t,struct _fat_ptr v,struct _fat_ptr*msg){
# 269
unsigned loc=f1->loc;
# 272
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp88=({struct Cyc_String_pa_PrintArg_struct _tmp191;_tmp191.tag=0,_tmp191.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp191;});struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp190;_tmp190.tag=0,_tmp190.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp190;});struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp18F;_tmp18F.tag=0,_tmp18F.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f1->name).f2);_tmp18F;});struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp18E;_tmp18E.tag=0,_tmp18E.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f0->name).f2);_tmp18E;});void*_tmp86[4];_tmp86[0]=& _tmp88,_tmp86[1]=& _tmp89,_tmp86[2]=& _tmp8A,_tmp86[3]=& _tmp8B;({unsigned _tmp220=loc;struct _fat_ptr*_tmp21F=msg;struct _fat_ptr _tmp21E=({const char*_tmp87="%s %s: field name mismatch %s != %s";_tag_fat(_tmp87,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp220,_tmp21F,_tmp21E,_tag_fat(_tmp86,sizeof(void*),4));});});
# 275
return 0;}{
# 278
struct _fat_ptr _tmp8C=*(*f0->name).f2;struct _fat_ptr name=_tmp8C;
# 281
struct _tuple11 _tmp8D=({
enum Cyc_Absyn_Scope _tmp226=f0->sc;enum Cyc_Absyn_Scope _tmp225=f1->sc;struct _fat_ptr _tmp224=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA8=({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0,_tmp19E.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp19E;});struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp19D;_tmp19D.tag=0,_tmp19D.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp19D;});void*_tmpA6[2];_tmpA6[0]=& _tmpA8,_tmpA6[1]=& _tmpA9;({struct _fat_ptr _tmp221=({const char*_tmpA7="in %s %s, field";_tag_fat(_tmpA7,sizeof(char),16U);});Cyc_aprintf(_tmp221,_tag_fat(_tmpA6,sizeof(void*),2));});});struct _fat_ptr _tmp223=name;unsigned _tmp222=loc;Cyc_Tcdecl_merge_scope(_tmp226,_tmp225,_tmp224,_tmp223,_tmp222,msg,0);});
# 281
struct _tuple11 _stmttmp6=_tmp8D;struct _tuple11 _tmp8E=_stmttmp6;int _tmp90;enum Cyc_Absyn_Scope _tmp8F;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;{enum Cyc_Absyn_Scope new_scope=_tmp8F;int res=_tmp90;
# 285
struct Cyc_List_List*_tmp91=f0->typs;struct Cyc_List_List*typs0=_tmp91;
struct Cyc_List_List*_tmp92=f1->typs;struct Cyc_List_List*typs1=_tmp92;
if(({int _tmp227=((int(*)(struct Cyc_List_List*))Cyc_List_length)(typs0);_tmp227 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(typs1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp95=({struct Cyc_String_pa_PrintArg_struct _tmp194;_tmp194.tag=0,_tmp194.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp194;});struct Cyc_String_pa_PrintArg_struct _tmp96=({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0,_tmp193.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp193;});struct Cyc_String_pa_PrintArg_struct _tmp97=({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0,_tmp192.f1=(struct _fat_ptr)((struct _fat_ptr)name);_tmp192;});void*_tmp93[3];_tmp93[0]=& _tmp95,_tmp93[1]=& _tmp96,_tmp93[2]=& _tmp97;({unsigned _tmp22A=loc;struct _fat_ptr*_tmp229=msg;struct _fat_ptr _tmp228=({const char*_tmp94="%s %s, field %s: parameter number mismatch";_tag_fat(_tmp94,sizeof(char),43U);});Cyc_Tcdecl_merr(_tmp22A,_tmp229,_tmp228,_tag_fat(_tmp93,sizeof(void*),3));});});
res=0;}
# 292
for(1;typs0 != 0 && typs1 != 0;(typs0=typs0->tl,typs1=typs1->tl)){
# 294
if(!Cyc_Absyn_equal_tqual((*((struct _tuple16*)typs0->hd)).f1,(*((struct _tuple16*)typs1->hd)).f1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9A=({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0,_tmp197.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp197;});struct Cyc_String_pa_PrintArg_struct _tmp9B=({struct Cyc_String_pa_PrintArg_struct _tmp196;_tmp196.tag=0,_tmp196.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp196;});struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0,_tmp195.f1=(struct _fat_ptr)((struct _fat_ptr)name);_tmp195;});void*_tmp98[3];_tmp98[0]=& _tmp9A,_tmp98[1]=& _tmp9B,_tmp98[2]=& _tmp9C;({unsigned _tmp22D=loc;struct _fat_ptr*_tmp22C=msg;struct _fat_ptr _tmp22B=({const char*_tmp99="%s %s, field %s: parameter qualifier";_tag_fat(_tmp99,sizeof(char),37U);});Cyc_Tcdecl_merr(_tmp22D,_tmp22C,_tmp22B,_tag_fat(_tmp98,sizeof(void*),3));});});
res=0;}{
# 298
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple16*)typs1->hd)).f2);
# 300
if(!Cyc_Tcdecl_check_type((*((struct _tuple16*)typs0->hd)).f2,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9F=({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0,_tmp19C.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp19C;});struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp19B;_tmp19B.tag=0,_tmp19B.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp19B;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0,_tmp19A.f1=(struct _fat_ptr)((struct _fat_ptr)name);_tmp19A;});struct Cyc_String_pa_PrintArg_struct _tmpA2=({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0,({
struct _fat_ptr _tmp22E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((*((struct _tuple16*)typs0->hd)).f2));_tmp199.f1=_tmp22E;});_tmp199;});struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0,({struct _fat_ptr _tmp22F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(subst_t1));_tmp198.f1=_tmp22F;});_tmp198;});void*_tmp9D[5];_tmp9D[0]=& _tmp9F,_tmp9D[1]=& _tmpA0,_tmp9D[2]=& _tmpA1,_tmp9D[3]=& _tmpA2,_tmp9D[4]=& _tmpA3;({unsigned _tmp232=loc;struct _fat_ptr*_tmp231=msg;struct _fat_ptr _tmp230=({const char*_tmp9E="%s %s, field %s: parameter type mismatch %s != %s";_tag_fat(_tmp9E,sizeof(char),50U);});Cyc_Tcdecl_merr(_tmp232,_tmp231,_tmp230,_tag_fat(_tmp9D,sizeof(void*),5));});});
Cyc_Unify_explain_failure();
res=0;}}}
# 308
if(!res)return 0;
if((int)f0->sc == (int)new_scope)return f0;{
struct Cyc_Absyn_Datatypefield*_tmpA4=({struct Cyc_Absyn_Datatypefield*_tmpA5=_cycalloc(sizeof(*_tmpA5));*_tmpA5=*f0;_tmpA5;});struct Cyc_Absyn_Datatypefield*f2=_tmpA4;
f2->sc=new_scope;
return f2;}}}}
# 315
static struct _tuple16*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple16*x){
# 318
struct _tuple16*_tmpAA=x;void*_tmpAC;struct Cyc_Absyn_Tqual _tmpAB;_tmpAB=_tmpAA->f1;_tmpAC=_tmpAA->f2;{struct Cyc_Absyn_Tqual tq=_tmpAB;void*t=_tmpAC;
return({struct _tuple16*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->f1=tq,({void*_tmp233=Cyc_Tcutil_substitute(inst,t);_tmpAD->f2=_tmp233;});_tmpAD;});}}
# 324
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 326
struct Cyc_Absyn_Datatypefield*_tmpAE=({struct Cyc_Absyn_Datatypefield*_tmpAF=_cycalloc(sizeof(*_tmpAF));*_tmpAF=*f1;_tmpAF;});struct Cyc_Absyn_Datatypefield*f0=_tmpAE;
({struct Cyc_List_List*_tmp234=((struct Cyc_List_List*(*)(struct _tuple16*(*)(struct Cyc_List_List*,struct _tuple16*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);f0->typs=_tmp234;});
# 329
return f0;}
# 336
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 345
struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpB8=_cycalloc(sizeof(*_tmpB8));*_tmpB8=0;_tmpB8;});
struct Cyc_List_List**_tmpB0=f2sp;struct Cyc_List_List**next=_tmpB0;
int cmp=-1;
# 349
for(1;f0s != 0 && f1s != 0;f1s=f1s->tl){
while(f0s != 0 &&(cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,_tmpB2->tl=0;_tmpB2;});struct Cyc_List_List*node=_tmpB1;
*next=node;
next=& node->tl;
f0s=f0s->tl;}
# 356
if(f0s == 0 || cmp > 0){
# 358
*incl=0;{
struct Cyc_List_List*_tmpB3=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct Cyc_Absyn_Datatypefield*_tmp235=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpB4->hd=_tmp235;}),_tmpB4->tl=0;_tmpB4;});struct Cyc_List_List*node=_tmpB3;
*next=node;
next=& node->tl;}}else{
# 364
struct Cyc_Absyn_Datatypefield*_tmpB5=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);struct Cyc_Absyn_Datatypefield*f2=_tmpB5;
if(f2 != 0){
if(f2 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmpB6=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=f2,_tmpB7->tl=0;_tmpB7;});struct Cyc_List_List*node=_tmpB6;
*next=node;
next=& node->tl;}}else{
# 371
*res=0;}
# 373
f0s=f0s->tl;}}
# 378
if(f1s != 0){
*incl=0;
*next=f1s;}else{
# 382
*next=f0s;}
# 384
return*f2sp;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 387
static struct _tuple17 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0)return({struct _tuple17 _tmp19F;_tmp19F.f1=0,_tmp19F.f2=0;_tmp19F;});
if(f->tl == 0)return({struct _tuple17 _tmp1A0;_tmp1A0.f1=f,_tmp1A0.f2=0;_tmp1A0;});{
struct _tuple17 _tmpB9=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct _tuple17 _stmttmp7=_tmpB9;struct _tuple17 _tmpBA=_stmttmp7;void*_tmpBC;void*_tmpBB;_tmpBB=_tmpBA.f1;_tmpBC=_tmpBA.f2;{struct Cyc_List_List*f1=_tmpBB;struct Cyc_List_List*f2=_tmpBC;
return({struct _tuple17 _tmp1A1;({struct Cyc_List_List*_tmp237=({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->hd=f->hd,_tmpBD->tl=f1;_tmpBD;});_tmp1A1.f1=_tmp237;}),({struct Cyc_List_List*_tmp236=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,_tmpBE->tl=f2;_tmpBE;});_tmp1A1.f2=_tmp236;});_tmp1A1;});}}}
# 396
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _fat_ptr*v,unsigned loc,struct _fat_ptr*msg){
# 399
struct _tuple17 _tmpBF=((struct _tuple17(*)(struct Cyc_List_List*))Cyc_Tcdecl_split)(f);struct _tuple17 _stmttmp8=_tmpBF;struct _tuple17 _tmpC0=_stmttmp8;void*_tmpC2;void*_tmpC1;_tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;{struct Cyc_List_List*f1=_tmpC1;struct Cyc_List_List*f2=_tmpC2;
if(f1 != 0 && f1->tl != 0)
f1=Cyc_Tcdecl_sort_xdatatype_fields(f1,res,v,loc,msg);
if(f2 != 0 && f2->tl != 0)
f2=Cyc_Tcdecl_sort_xdatatype_fields(f2,res,v,loc,msg);
return({struct Cyc_List_List*_tmp23E=f1;struct Cyc_List_List*_tmp23D=f2;int*_tmp23C=res;int*_tmp23B=({int*_tmpC3=_cycalloc_atomic(sizeof(*_tmpC3));*_tmpC3=1;_tmpC3;});struct _fat_ptr _tmp23A=({const char*_tmpC4="@extensible datatype";_tag_fat(_tmpC4,sizeof(char),21U);});struct _fat_ptr _tmp239=*v;unsigned _tmp238=loc;Cyc_Tcdecl_merge_xdatatype_fields(_tmp23E,_tmp23D,0,0,0,_tmp23C,_tmp23B,_tmp23A,_tmp239,_tmp238,msg);});}}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 408
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned loc,struct _fat_ptr*msg){
# 410
struct _fat_ptr _tmpC5=Cyc_Absynpp_qvar2string(d0->name);struct _fat_ptr v=_tmpC5;
struct _fat_ptr t=({const char*_tmpE4="datatype";_tag_fat(_tmpE4,sizeof(char),9U);});
int _tmpC6=1;int res=_tmpC6;
# 417
if(d0->is_extensible != d1->is_extensible){
({struct Cyc_String_pa_PrintArg_struct _tmpC9=({struct Cyc_String_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=0,_tmp1A4.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1A4;});struct Cyc_String_pa_PrintArg_struct _tmpCA=({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0,({
struct _fat_ptr _tmp23F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible));_tmp1A3.f1=_tmp23F;});_tmp1A3;});struct Cyc_String_pa_PrintArg_struct _tmpCB=({struct Cyc_String_pa_PrintArg_struct _tmp1A2;_tmp1A2.tag=0,({struct _fat_ptr _tmp240=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible));_tmp1A2.f1=_tmp240;});_tmp1A2;});void*_tmpC7[3];_tmpC7[0]=& _tmpC9,_tmpC7[1]=& _tmpCA,_tmpC7[2]=& _tmpCB;({unsigned _tmp243=loc;struct _fat_ptr*_tmp242=msg;struct _fat_ptr _tmp241=({const char*_tmpC8="expected %s to be a %s instead of a %s";_tag_fat(_tmpC8,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp243,_tmp242,_tmp241,_tag_fat(_tmpC7,sizeof(void*),3));});});
res=0;}else{
# 422
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 425
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,v,loc,msg))return 0;{
# 428
struct _tuple11 _tmpCC=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,v,loc,msg,0);struct _tuple11 _stmttmp9=_tmpCC;struct _tuple11 _tmpCD=_stmttmp9;int _tmpCF;enum Cyc_Absyn_Scope _tmpCE;_tmpCE=_tmpCD.f1;_tmpCF=_tmpCD.f2;{enum Cyc_Absyn_Scope new_scope=_tmpCE;int res_scope=_tmpCF;
if(!res_scope)res=0;{
# 431
struct Cyc_Absyn_Datatypedecl*d2;
# 434
{struct _tuple18 _tmpD0=({struct _tuple18 _tmp1AB;_tmp1AB.f1=d0->fields,_tmp1AB.f2=d1->fields;_tmp1AB;});struct _tuple18 _stmttmpA=_tmpD0;struct _tuple18 _tmpD1=_stmttmpA;void*_tmpD3;void*_tmpD2;if(_tmpD1.f2 == 0){
d2=d0;goto _LL3;}else{if(_tmpD1.f1 == 0){
d2=d1;goto _LL3;}else{_tmpD2=(struct Cyc_List_List*)(_tmpD1.f1)->v;_tmpD3=(struct Cyc_List_List*)(_tmpD1.f2)->v;{struct Cyc_List_List*f0s=_tmpD2;struct Cyc_List_List*f1s=_tmpD3;
# 448 "tcdecl.cyc"
struct Cyc_List_List*_tmpD4=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);struct Cyc_List_List*inst=_tmpD4;
# 451
if(d0->is_extensible){
# 455
int _tmpD5=1;int incl=_tmpD5;
struct Cyc_List_List*_tmpD6=Cyc_Tcdecl_merge_xdatatype_fields(f0s,f1s,inst,d0->tvs,d1->tvs,& res,& incl,t,v,loc,msg);struct Cyc_List_List*f2s=_tmpD6;
# 459
if(incl)
d2=d0;else{
# 462
d2=({struct Cyc_Absyn_Datatypedecl*_tmpD7=_cycalloc(sizeof(*_tmpD7));*_tmpD7=*d0;_tmpD7;});
d2->sc=new_scope;
({struct Cyc_Core_Opt*_tmp244=({struct Cyc_Core_Opt*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->v=f2s;_tmpD8;});d2->fields=_tmp244;});}}else{
# 468
for(1;f0s != 0 && f1s != 0;(f0s=f0s->tl,f1s=f1s->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);}
if(f0s != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=0,_tmp1A7.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1A7;});struct Cyc_String_pa_PrintArg_struct _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmp1A6;_tmp1A6.tag=0,_tmp1A6.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1A6;});struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp1A5;_tmp1A5.tag=0,_tmp1A5.f1=(struct _fat_ptr)((struct _fat_ptr)*(*((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name).f2);_tmp1A5;});void*_tmpD9[3];_tmpD9[0]=& _tmpDB,_tmpD9[1]=& _tmpDC,_tmpD9[2]=& _tmpDD;({unsigned _tmp247=loc;struct _fat_ptr*_tmp246=msg;struct _fat_ptr _tmp245=({const char*_tmpDA="%s %s is missing field %s";_tag_fat(_tmpDA,sizeof(char),26U);});Cyc_Tcdecl_merr(_tmp247,_tmp246,_tmp245,_tag_fat(_tmpD9,sizeof(void*),3));});});
res=0;}
# 474
if(f1s != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp1AA;_tmp1AA.tag=0,_tmp1AA.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1AA;});struct Cyc_String_pa_PrintArg_struct _tmpE1=({struct Cyc_String_pa_PrintArg_struct _tmp1A9;_tmp1A9.tag=0,_tmp1A9.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1A9;});struct Cyc_String_pa_PrintArg_struct _tmpE2=({struct Cyc_String_pa_PrintArg_struct _tmp1A8;_tmp1A8.tag=0,_tmp1A8.f1=(struct _fat_ptr)((struct _fat_ptr)*(*((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name).f2);_tmp1A8;});void*_tmpDE[3];_tmpDE[0]=& _tmpE0,_tmpDE[1]=& _tmpE1,_tmpDE[2]=& _tmpE2;({unsigned _tmp24A=loc;struct _fat_ptr*_tmp249=msg;struct _fat_ptr _tmp248=({const char*_tmpDF="%s %s has extra field %s";_tag_fat(_tmpDF,sizeof(char),25U);});Cyc_Tcdecl_merr(_tmp24A,_tmp249,_tmp248,_tag_fat(_tmpDE,sizeof(void*),3));});});
res=0;}
# 478
d2=d0;}
# 480
goto _LL3;}}}_LL3:;}
# 483
if(!res)return 0;
if((int)new_scope == (int)d2->sc)return d2;
d2=({struct Cyc_Absyn_Datatypedecl*_tmpE3=_cycalloc(sizeof(*_tmpE3));*_tmpE3=*d2;_tmpE3;});
d2->sc=new_scope;
return d2;}}}}
# 490
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned loc,struct _fat_ptr*msg){
# 492
struct _fat_ptr _tmpE5=Cyc_Absynpp_qvar2string(d0->name);struct _fat_ptr v=_tmpE5;
int _tmpE6=1;int res=_tmpE6;
# 496
struct _tuple11 _tmpE7=({enum Cyc_Absyn_Scope _tmp24F=d0->sc;enum Cyc_Absyn_Scope _tmp24E=d1->sc;struct _fat_ptr _tmp24D=({const char*_tmp106="enum";_tag_fat(_tmp106,sizeof(char),5U);});struct _fat_ptr _tmp24C=v;unsigned _tmp24B=loc;Cyc_Tcdecl_merge_scope(_tmp24F,_tmp24E,_tmp24D,_tmp24C,_tmp24B,msg,1);});struct _tuple11 _stmttmpB=_tmpE7;struct _tuple11 _tmpE8=_stmttmpB;int _tmpEA;enum Cyc_Absyn_Scope _tmpE9;_tmpE9=_tmpE8.f1;_tmpEA=_tmpE8.f2;{enum Cyc_Absyn_Scope new_scope=_tmpE9;int res_scope=_tmpEA;
if(!res_scope)res=0;{
# 499
struct Cyc_Absyn_Enumdecl*d2;
# 502
{struct _tuple18 _tmpEB=({struct _tuple18 _tmp1B2;_tmp1B2.f1=d0->fields,_tmp1B2.f2=d1->fields;_tmp1B2;});struct _tuple18 _stmttmpC=_tmpEB;struct _tuple18 _tmpEC=_stmttmpC;void*_tmpEE;void*_tmpED;if(_tmpEC.f2 == 0){
d2=d0;goto _LL3;}else{if(_tmpEC.f1 == 0){
d2=d1;goto _LL3;}else{_tmpED=(struct Cyc_List_List*)(_tmpEC.f1)->v;_tmpEE=(struct Cyc_List_List*)(_tmpEC.f2)->v;{struct Cyc_List_List*f0s=_tmpED;struct Cyc_List_List*f1s=_tmpEE;
# 507
for(1;f0s != 0 && f1s != 0;(f0s=f0s->tl,f1s=f1s->tl)){
struct Cyc_Absyn_Enumfield*_tmpEF=(struct Cyc_Absyn_Enumfield*)f0s->hd;struct Cyc_Absyn_Enumfield*_stmttmpD=_tmpEF;struct Cyc_Absyn_Enumfield*_tmpF0=_stmttmpD;unsigned _tmpF3;void*_tmpF2;void*_tmpF1;_tmpF1=(_tmpF0->name)->f2;_tmpF2=_tmpF0->tag;_tmpF3=_tmpF0->loc;{struct _fat_ptr*fname0=_tmpF1;struct Cyc_Absyn_Exp*tag0=_tmpF2;unsigned loc0=_tmpF3;
struct Cyc_Absyn_Enumfield*_tmpF4=(struct Cyc_Absyn_Enumfield*)f1s->hd;struct Cyc_Absyn_Enumfield*_stmttmpE=_tmpF4;struct Cyc_Absyn_Enumfield*_tmpF5=_stmttmpE;unsigned _tmpF8;void*_tmpF7;void*_tmpF6;_tmpF6=(_tmpF5->name)->f2;_tmpF7=_tmpF5->tag;_tmpF8=_tmpF5->loc;{struct _fat_ptr*fname1=_tmpF6;struct Cyc_Absyn_Exp*tag1=_tmpF7;unsigned loc1=_tmpF8;
# 511
if(Cyc_strptrcmp(fname0,fname1)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmp1AE;_tmp1AE.tag=0,_tmp1AE.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1AE;});struct Cyc_String_pa_PrintArg_struct _tmpFC=({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0,_tmp1AD.f1=(struct _fat_ptr)((struct _fat_ptr)*fname0);_tmp1AD;});struct Cyc_String_pa_PrintArg_struct _tmpFD=({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0,_tmp1AC.f1=(struct _fat_ptr)((struct _fat_ptr)*fname1);_tmp1AC;});void*_tmpF9[3];_tmpF9[0]=& _tmpFB,_tmpF9[1]=& _tmpFC,_tmpF9[2]=& _tmpFD;({unsigned _tmp252=loc;struct _fat_ptr*_tmp251=msg;struct _fat_ptr _tmp250=({const char*_tmpFA="enum %s: field name mismatch %s != %s";_tag_fat(_tmpFA,sizeof(char),38U);});Cyc_Tcdecl_merr(_tmp252,_tmp251,_tmp250,_tag_fat(_tmpF9,sizeof(void*),3));});});
res=0;}
# 515
if(({unsigned _tmp253=Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(tag0));_tmp253 != Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(tag1));})){
({struct Cyc_String_pa_PrintArg_struct _tmp100=({struct Cyc_String_pa_PrintArg_struct _tmp1B0;_tmp1B0.tag=0,_tmp1B0.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1B0;});struct Cyc_String_pa_PrintArg_struct _tmp101=({struct Cyc_String_pa_PrintArg_struct _tmp1AF;_tmp1AF.tag=0,_tmp1AF.f1=(struct _fat_ptr)((struct _fat_ptr)*fname1);_tmp1AF;});void*_tmpFE[2];_tmpFE[0]=& _tmp100,_tmpFE[1]=& _tmp101;({unsigned _tmp256=loc;struct _fat_ptr*_tmp255=msg;struct _fat_ptr _tmp254=({const char*_tmpFF="enum %s, field %s, value mismatch";_tag_fat(_tmpFF,sizeof(char),34U);});Cyc_Tcdecl_merr(_tmp256,_tmp255,_tmp254,_tag_fat(_tmpFE,sizeof(void*),2));});});
res=0;}}}}
# 520
if(f0s != 0 || f1s != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp1B1;_tmp1B1.tag=0,_tmp1B1.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1B1;});void*_tmp102[1];_tmp102[0]=& _tmp104;({unsigned _tmp259=loc;struct _fat_ptr*_tmp258=msg;struct _fat_ptr _tmp257=({const char*_tmp103="enum %s declarations: number of fields differs";_tag_fat(_tmp103,sizeof(char),47U);});Cyc_Tcdecl_merr(_tmp259,_tmp258,_tmp257,_tag_fat(_tmp102,sizeof(void*),1));});});
res=0;}
# 524
d2=d0;
goto _LL3;}}}_LL3:;}
# 528
if(!res)return 0;
if((int)d2->sc == (int)new_scope)return d2;
d2=({struct Cyc_Absyn_Enumdecl*_tmp105=_cycalloc(sizeof(*_tmp105));*_tmp105=*d2;_tmp105;});
d2->sc=new_scope;
return d2;}}}
# 535
static struct _tuple11 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 539
int _tmp107=1;int res=_tmp107;
# 542
struct _tuple11 _tmp108=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);struct _tuple11 _stmttmpF=_tmp108;struct _tuple11 _tmp109=_stmttmpF;int _tmp10B;enum Cyc_Absyn_Scope _tmp10A;_tmp10A=_tmp109.f1;_tmp10B=_tmp109.f2;{enum Cyc_Absyn_Scope new_scope=_tmp10A;int res_scope=_tmp10B;
if(!res_scope)res=0;
# 546
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _fat_ptr _tmp10C=Cyc_Absynpp_typ2string(t0);struct _fat_ptr s1=_tmp10C;
struct _fat_ptr _tmp10D=Cyc_Absynpp_typ2string(t1);struct _fat_ptr s2=_tmp10D;
if(({unsigned long _tmp25D=({unsigned long _tmp25C=({unsigned long _tmp25B=({unsigned long _tmp25A=Cyc_strlen((struct _fat_ptr)s1);_tmp25A + Cyc_strlen((struct _fat_ptr)s2);});_tmp25B + Cyc_strlen((struct _fat_ptr)t);});_tmp25C + Cyc_strlen((struct _fat_ptr)v);});_tmp25D + 
Cyc_strlen(({const char*_tmp10E="  was declared  instead of ";_tag_fat(_tmp10E,sizeof(char),28U);}));})> 70U)
({struct Cyc_String_pa_PrintArg_struct _tmp111=({struct Cyc_String_pa_PrintArg_struct _tmp1B6;_tmp1B6.tag=0,_tmp1B6.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1B6;});struct Cyc_String_pa_PrintArg_struct _tmp112=({struct Cyc_String_pa_PrintArg_struct _tmp1B5;_tmp1B5.tag=0,_tmp1B5.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1B5;});struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0,_tmp1B4.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1B4;});struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0,_tmp1B3.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp1B3;});void*_tmp10F[4];_tmp10F[0]=& _tmp111,_tmp10F[1]=& _tmp112,_tmp10F[2]=& _tmp113,_tmp10F[3]=& _tmp114;({unsigned _tmp260=loc;struct _fat_ptr*_tmp25F=msg;struct _fat_ptr _tmp25E=({const char*_tmp110="%s %s was declared \n\t%s\n instead of \n\t%s";_tag_fat(_tmp110,sizeof(char),41U);});Cyc_Tcdecl_merr(_tmp260,_tmp25F,_tmp25E,_tag_fat(_tmp10F,sizeof(void*),4));});});else{
# 554
({struct Cyc_String_pa_PrintArg_struct _tmp117=({struct Cyc_String_pa_PrintArg_struct _tmp1BA;_tmp1BA.tag=0,_tmp1BA.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1BA;});struct Cyc_String_pa_PrintArg_struct _tmp118=({struct Cyc_String_pa_PrintArg_struct _tmp1B9;_tmp1B9.tag=0,_tmp1B9.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1B9;});struct Cyc_String_pa_PrintArg_struct _tmp119=({struct Cyc_String_pa_PrintArg_struct _tmp1B8;_tmp1B8.tag=0,_tmp1B8.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1B8;});struct Cyc_String_pa_PrintArg_struct _tmp11A=({struct Cyc_String_pa_PrintArg_struct _tmp1B7;_tmp1B7.tag=0,_tmp1B7.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp1B7;});void*_tmp115[4];_tmp115[0]=& _tmp117,_tmp115[1]=& _tmp118,_tmp115[2]=& _tmp119,_tmp115[3]=& _tmp11A;({unsigned _tmp263=loc;struct _fat_ptr*_tmp262=msg;struct _fat_ptr _tmp261=({const char*_tmp116="%s %s was declared %s instead of %s";_tag_fat(_tmp116,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp263,_tmp262,_tmp261,_tag_fat(_tmp115,sizeof(void*),4));});});}
# 556
Cyc_Unify_explain_failure();
res=0;}
# 561
if(!Cyc_Absyn_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _tmp11D=({struct Cyc_String_pa_PrintArg_struct _tmp1BC;_tmp1BC.tag=0,_tmp1BC.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1BC;});struct Cyc_String_pa_PrintArg_struct _tmp11E=({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0,_tmp1BB.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1BB;});void*_tmp11B[2];_tmp11B[0]=& _tmp11D,_tmp11B[1]=& _tmp11E;({unsigned _tmp266=loc;struct _fat_ptr*_tmp265=msg;struct _fat_ptr _tmp264=({const char*_tmp11C="%s %s has different type qualifiers";_tag_fat(_tmp11C,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp266,_tmp265,_tmp264,_tag_fat(_tmp11B,sizeof(void*),2));});});
res=0;}
# 567
if(!Cyc_Atts_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp1BE;_tmp1BE.tag=0,_tmp1BE.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp1BE;});struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp1BD;_tmp1BD.tag=0,_tmp1BD.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1BD;});void*_tmp11F[2];_tmp11F[0]=& _tmp121,_tmp11F[1]=& _tmp122;({unsigned _tmp269=loc;struct _fat_ptr*_tmp268=msg;struct _fat_ptr _tmp267=({const char*_tmp120="%s %s has different attributes";_tag_fat(_tmp120,sizeof(char),31U);});Cyc_Tcdecl_merr(_tmp269,_tmp268,_tmp267,_tag_fat(_tmp11F,sizeof(void*),2));});});
({void*_tmp123=0U;({struct Cyc___cycFILE*_tmp26B=Cyc_stderr;struct _fat_ptr _tmp26A=({const char*_tmp124="\tprevious attributes: ";_tag_fat(_tmp124,sizeof(char),23U);});Cyc_fprintf(_tmp26B,_tmp26A,_tag_fat(_tmp123,sizeof(void*),0));});});
for(1;atts0 != 0;atts0=atts0->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp127=({struct Cyc_String_pa_PrintArg_struct _tmp1BF;_tmp1BF.tag=0,({struct _fat_ptr _tmp26C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_attribute2string((void*)atts0->hd));_tmp1BF.f1=_tmp26C;});_tmp1BF;});void*_tmp125[1];_tmp125[0]=& _tmp127;({struct Cyc___cycFILE*_tmp26E=Cyc_stderr;struct _fat_ptr _tmp26D=({const char*_tmp126="%s ";_tag_fat(_tmp126,sizeof(char),4U);});Cyc_fprintf(_tmp26E,_tmp26D,_tag_fat(_tmp125,sizeof(void*),1));});});}
({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp270=Cyc_stderr;struct _fat_ptr _tmp26F=({const char*_tmp129="\n\tcurrent attributes: ";_tag_fat(_tmp129,sizeof(char),23U);});Cyc_fprintf(_tmp270,_tmp26F,_tag_fat(_tmp128,sizeof(void*),0));});});
for(1;atts1 != 0;atts1=atts1->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp12C=({struct Cyc_String_pa_PrintArg_struct _tmp1C0;_tmp1C0.tag=0,({struct _fat_ptr _tmp271=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_attribute2string((void*)atts1->hd));_tmp1C0.f1=_tmp271;});_tmp1C0;});void*_tmp12A[1];_tmp12A[0]=& _tmp12C;({struct Cyc___cycFILE*_tmp273=Cyc_stderr;struct _fat_ptr _tmp272=({const char*_tmp12B="%s ";_tag_fat(_tmp12B,sizeof(char),4U);});Cyc_fprintf(_tmp273,_tmp272,_tag_fat(_tmp12A,sizeof(void*),1));});});}
({void*_tmp12D=0U;({struct Cyc___cycFILE*_tmp275=Cyc_stderr;struct _fat_ptr _tmp274=({const char*_tmp12E="\n";_tag_fat(_tmp12E,sizeof(char),2U);});Cyc_fprintf(_tmp275,_tmp274,_tag_fat(_tmp12D,sizeof(void*),0));});});
res=0;}
# 579
return({struct _tuple11 _tmp1C1;_tmp1C1.f1=new_scope,_tmp1C1.f2=res;_tmp1C1;});}}
# 582
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned loc,struct _fat_ptr*msg){
# 584
struct _fat_ptr _tmp12F=Cyc_Absynpp_qvar2string(d0->name);struct _fat_ptr v=_tmp12F;
# 587
struct _tuple11 _tmp130=({
enum Cyc_Absyn_Scope _tmp280=d0->sc;void*_tmp27F=d0->type;struct Cyc_Absyn_Tqual _tmp27E=d0->tq;struct Cyc_List_List*_tmp27D=d0->attributes;enum Cyc_Absyn_Scope _tmp27C=d1->sc;void*_tmp27B=d1->type;struct Cyc_Absyn_Tqual _tmp27A=d1->tq;struct Cyc_List_List*_tmp279=d1->attributes;struct _fat_ptr _tmp278=({const char*_tmp136="variable";_tag_fat(_tmp136,sizeof(char),9U);});struct _fat_ptr _tmp277=v;unsigned _tmp276=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp280,_tmp27F,_tmp27E,_tmp27D,_tmp27C,_tmp27B,_tmp27A,_tmp279,_tmp278,_tmp277,_tmp276,msg);});
# 587
struct _tuple11 _stmttmp10=_tmp130;struct _tuple11 _tmp131=_stmttmp10;int _tmp133;enum Cyc_Absyn_Scope _tmp132;_tmp132=_tmp131.f1;_tmp133=_tmp131.f2;{enum Cyc_Absyn_Scope new_scope=_tmp132;int res=_tmp133;
# 592
if(!res)return 0;
if((int)d0->sc == (int)new_scope)return d0;{
struct Cyc_Absyn_Vardecl*_tmp134=({struct Cyc_Absyn_Vardecl*_tmp135=_cycalloc(sizeof(*_tmp135));*_tmp135=*d0;_tmp135;});struct Cyc_Absyn_Vardecl*d2=_tmp134;
d2->sc=new_scope;
return d2;}}}
# 599
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned loc,struct _fat_ptr*msg){
# 601
struct _fat_ptr _tmp137=Cyc_Absynpp_qvar2string(d0->name);struct _fat_ptr v=_tmp137;
# 604
if(!({struct Cyc_List_List*_tmp285=d0->tvs;struct Cyc_List_List*_tmp284=d1->tvs;struct _fat_ptr _tmp283=({const char*_tmp138="typedef";_tag_fat(_tmp138,sizeof(char),8U);});struct _fat_ptr _tmp282=v;unsigned _tmp281=loc;Cyc_Tcdecl_check_tvs(_tmp285,_tmp284,_tmp283,_tmp282,_tmp281,msg);}))return 0;{
# 607
struct Cyc_List_List*_tmp139=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);struct Cyc_List_List*inst=_tmp139;
# 610
if(d0->defn != 0 && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(inst,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
({struct Cyc_String_pa_PrintArg_struct _tmp13C=({struct Cyc_String_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=0,_tmp1C4.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1C4;});struct Cyc_String_pa_PrintArg_struct _tmp13D=({struct Cyc_String_pa_PrintArg_struct _tmp1C3;_tmp1C3.tag=0,({
struct _fat_ptr _tmp286=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(subst_defn1));_tmp1C3.f1=_tmp286;});_tmp1C3;});struct Cyc_String_pa_PrintArg_struct _tmp13E=({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0,({struct _fat_ptr _tmp287=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(d0->defn)));_tmp1C2.f1=_tmp287;});_tmp1C2;});void*_tmp13A[3];_tmp13A[0]=& _tmp13C,_tmp13A[1]=& _tmp13D,_tmp13A[2]=& _tmp13E;({unsigned _tmp28A=loc;struct _fat_ptr*_tmp289=msg;struct _fat_ptr _tmp288=({const char*_tmp13B="typedef %s does not refer to the same type: %s != %s";_tag_fat(_tmp13B,sizeof(char),53U);});Cyc_Tcdecl_merr(_tmp28A,_tmp289,_tmp288,_tag_fat(_tmp13A,sizeof(void*),3));});});
return 0;}}
# 618
return d0;}}
# 621
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned loc,struct _fat_ptr*msg){
# 623
struct _tuple15 _tmp13F=({struct _tuple15 _tmp1C6;_tmp1C6.f1=b0,_tmp1C6.f2=b1;_tmp1C6;});struct _tuple15 _stmttmp11=_tmp13F;struct _tuple15 _tmp140=_stmttmp11;void*_tmp142;void*_tmp141;switch(*((int*)_tmp140.f1)){case 1: switch(*((int*)_tmp140.f2)){case 1: _tmp141=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp140.f1)->f1;_tmp142=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp140.f2)->f1;{struct Cyc_Absyn_Vardecl*d0=_tmp141;struct Cyc_Absyn_Vardecl*d1=_tmp142;
# 625
struct Cyc_Absyn_Vardecl*_tmp143=Cyc_Tcdecl_merge_vardecl(d0,d1,loc,msg);struct Cyc_Absyn_Vardecl*d2=_tmp143;
if(d2 == 0)return 0;
if(d2 == d0)return b0;
if(d2 == d1)return b1;
return(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->tag=1,_tmp144->f1=d2;_tmp144;});}case 2: _tmp141=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp140.f1)->f1;_tmp142=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp140.f2)->f1;{struct Cyc_Absyn_Vardecl*d0=_tmp141;struct Cyc_Absyn_Fndecl*d1=_tmp142;
# 631
struct _tuple11 _tmp145=({enum Cyc_Absyn_Scope _tmp295=d0->sc;void*_tmp294=d0->type;struct Cyc_Absyn_Tqual _tmp293=d0->tq;struct Cyc_List_List*_tmp292=d0->attributes;enum Cyc_Absyn_Scope _tmp291=d1->sc;void*_tmp290=(void*)_check_null(d1->cached_type);struct Cyc_Absyn_Tqual _tmp28F=
Cyc_Absyn_empty_tqual(0U);
# 631
struct Cyc_List_List*_tmp28E=(d1->i).attributes;struct _fat_ptr _tmp28D=({const char*_tmp148="function";_tag_fat(_tmp148,sizeof(char),9U);});struct _fat_ptr _tmp28C=
# 634
Cyc_Absynpp_qvar2string(d0->name);
# 631
unsigned _tmp28B=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp295,_tmp294,_tmp293,_tmp292,_tmp291,_tmp290,_tmp28F,_tmp28E,_tmp28D,_tmp28C,_tmp28B,msg);});struct _tuple11 _stmttmp12=_tmp145;struct _tuple11 _tmp146=_stmttmp12;int _tmp147;_tmp147=_tmp146.f2;{int res=_tmp147;
# 635
if(!res)return 0;
d0->is_proto=1;
return b1;}}default: goto _LL9;}case 2: switch(*((int*)_tmp140.f2)){case 2: _tmp141=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp140.f2)->f1;{struct Cyc_Absyn_Fndecl*d1=_tmp141;
# 639
({struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp1C5;_tmp1C5.tag=0,({struct _fat_ptr _tmp296=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp1C5.f1=_tmp296;});_tmp1C5;});void*_tmp149[1];_tmp149[0]=& _tmp14B;({unsigned _tmp299=loc;struct _fat_ptr*_tmp298=msg;struct _fat_ptr _tmp297=({const char*_tmp14A="redefinition of function %s";_tag_fat(_tmp14A,sizeof(char),28U);});Cyc_Tcdecl_merr(_tmp299,_tmp298,_tmp297,_tag_fat(_tmp149,sizeof(void*),1));});});
return 0;}case 1: _tmp141=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp140.f1)->f1;_tmp142=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp140.f2)->f1;{struct Cyc_Absyn_Fndecl*d0=_tmp141;struct Cyc_Absyn_Vardecl*d1=_tmp142;
# 642
struct _tuple11 _tmp14C=({enum Cyc_Absyn_Scope _tmp2A4=d0->sc;void*_tmp2A3=(void*)_check_null(d0->cached_type);struct Cyc_Absyn_Tqual _tmp2A2=Cyc_Absyn_empty_tqual(0U);struct Cyc_List_List*_tmp2A1=(d0->i).attributes;enum Cyc_Absyn_Scope _tmp2A0=d1->sc;void*_tmp29F=d1->type;struct Cyc_Absyn_Tqual _tmp29E=d1->tq;struct Cyc_List_List*_tmp29D=d1->attributes;struct _fat_ptr _tmp29C=({const char*_tmp14F="variable";_tag_fat(_tmp14F,sizeof(char),9U);});struct _fat_ptr _tmp29B=
# 645
Cyc_Absynpp_qvar2string(d0->name);
# 642
unsigned _tmp29A=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp2A4,_tmp2A3,_tmp2A2,_tmp2A1,_tmp2A0,_tmp29F,_tmp29E,_tmp29D,_tmp29C,_tmp29B,_tmp29A,msg);});struct _tuple11 _stmttmp13=_tmp14C;struct _tuple11 _tmp14D=_stmttmp13;int _tmp14E;_tmp14E=_tmp14D.f2;{int res=_tmp14E;
# 646
if(!res)return 0;
d1->is_proto=1;
return b0;}}default: goto _LL9;}default: _LL9:
 return 0;};}
# 653
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned loc,struct _fat_ptr*msg){
# 656
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp150=d0;void*_tmp152;void*_tmp151;_tmp151=_tmp150->base;_tmp152=_tmp150->field;{struct Cyc_Absyn_Datatypedecl*b0=_tmp151;struct Cyc_Absyn_Datatypefield*f0=_tmp152;
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp153=d1;void*_tmp155;void*_tmp154;_tmp154=_tmp153->base;_tmp155=_tmp153->field;{struct Cyc_Absyn_Datatypedecl*b1=_tmp154;struct Cyc_Absyn_Datatypefield*f1=_tmp155;
struct _fat_ptr _tmp156=Cyc_Absynpp_qvar2string(f0->name);struct _fat_ptr v=_tmp156;
# 661
if(Cyc_Absyn_qvar_cmp(b0->name,b1->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp1C9;_tmp1C9.tag=0,({
struct _fat_ptr _tmp2A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(b0->name));_tmp1C9.f1=_tmp2A5;});_tmp1C9;});struct Cyc_String_pa_PrintArg_struct _tmp15A=({struct Cyc_String_pa_PrintArg_struct _tmp1C8;_tmp1C8.tag=0,({struct _fat_ptr _tmp2A6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(b1->name));_tmp1C8.f1=_tmp2A6;});_tmp1C8;});struct Cyc_String_pa_PrintArg_struct _tmp15B=({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0,_tmp1C7.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp1C7;});void*_tmp157[3];_tmp157[0]=& _tmp159,_tmp157[1]=& _tmp15A,_tmp157[2]=& _tmp15B;({unsigned _tmp2A9=loc;struct _fat_ptr*_tmp2A8=msg;struct _fat_ptr _tmp2A7=({const char*_tmp158="@extensible datatype %s and %s both have a field named %s";_tag_fat(_tmp158,sizeof(char),58U);});Cyc_Tcdecl_merr(_tmp2A9,_tmp2A8,_tmp2A7,_tag_fat(_tmp157,sizeof(void*),3));});});
return 0;}
# 668
if(!({struct Cyc_List_List*_tmp2AE=b0->tvs;struct Cyc_List_List*_tmp2AD=b1->tvs;struct _fat_ptr _tmp2AC=({const char*_tmp15C="@extensible datatype";_tag_fat(_tmp15C,sizeof(char),21U);});struct _fat_ptr _tmp2AB=Cyc_Absynpp_qvar2string(b0->name);unsigned _tmp2AA=loc;Cyc_Tcdecl_check_tvs(_tmp2AE,_tmp2AD,_tmp2AC,_tmp2AB,_tmp2AA,msg);}))
return 0;{
# 671
struct Cyc_List_List*_tmp15D=Cyc_Tcdecl_build_tvs_map(b0->tvs,b1->tvs);struct Cyc_List_List*inst1=_tmp15D;
# 674
struct Cyc_Absyn_Datatypefield*_tmp15E=({struct Cyc_Absyn_Datatypefield*_tmp2B3=f0;struct Cyc_Absyn_Datatypefield*_tmp2B2=f1;struct Cyc_List_List*_tmp2B1=inst1;struct _fat_ptr _tmp2B0=({const char*_tmp160="@extensible datatype field";_tag_fat(_tmp160,sizeof(char),27U);});struct _fat_ptr _tmp2AF=v;Cyc_Tcdecl_merge_datatypefield(_tmp2B3,_tmp2B2,_tmp2B1,_tmp2B0,_tmp2AF,msg);});struct Cyc_Absyn_Datatypefield*f2=_tmp15E;
if(f2 == 0)return 0;
if(f2 == f0)return d0;
return({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->base=b0,_tmp15F->field=f2;_tmp15F;});}}}}
