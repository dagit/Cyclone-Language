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
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};
# 229 "dict.h"
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 283
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 878 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 906
extern void*Cyc_Absyn_uint_type;
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 915
extern void*Cyc_Absyn_empty_effect;
# 919
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 40 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
# 43
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 31 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 253
void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*youngest;void*opened_regions;};
# 67 "rgnorder.cyc"
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
struct Cyc_Absyn_Kind*_tmp0=Cyc_Tcutil_type_kind(rgn);struct Cyc_Absyn_Kind*_tmp1=_tmp0;enum Cyc_Absyn_KindQual _tmp16;enum Cyc_Absyn_AliasQual _tmp15;_LL1: _tmp16=_tmp1->kind;_tmp15=_tmp1->aliasqual;_LL2:;
if((int)_tmp16 != (int)3U)
({struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmpB2;_tmpB2.tag=0U,({
struct _fat_ptr _tmpBA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(rgn));_tmpB2.f1=_tmpBA;});_tmpB2;});void*_tmp2[1U];_tmp2[0]=& _tmp4;({struct _fat_ptr _tmpBB=({const char*_tmp3="bad region type |%s| passed to add_outlives_constraint";_tag_fat(_tmp3,sizeof(char),55U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpBB,_tag_fat(_tmp2,sizeof(void*),1U));});});{
void*_tmp5=Cyc_Tcutil_compress(eff);void*_tmp6=_tmp5;void*_tmp14;void*_tmp13;struct Cyc_List_List*_tmp12;switch(*((int*)_tmp6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6)->f1)){case 9U: _LL4: _tmp12=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6)->f2;_LL5:
# 74
 for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
if(!Cyc_RgnOrder_valid_constraint((void*)_tmp12->hd,rgn))return 0;}
return 1;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6)->f2 != 0){_LLA: _tmp13=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6)->f2)->hd;_LLB: {
# 83
struct Cyc_Absyn_Kind*_tmp7=Cyc_Tcutil_type_kind(_tmp13);struct Cyc_Absyn_Kind*_tmp8=_tmp7;enum Cyc_Absyn_KindQual _tmpA;enum Cyc_Absyn_AliasQual _tmp9;_LL11: _tmpA=_tmp8->kind;_tmp9=_tmp8->aliasqual;_LL12:;
return(int)_tmp15 == (int)0U ||(int)_tmp9 == (int)_tmp15;}}else{goto _LLE;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6)->f2 != 0){_LLC: _tmp14=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6)->f2)->hd;_LLD: {
# 88
struct Cyc_Absyn_Kind*_tmpB=Cyc_Tcutil_type_kind(_tmp14);struct Cyc_Absyn_Kind*_tmpC=_tmpB;enum Cyc_Absyn_KindQual _tmpE;enum Cyc_Absyn_AliasQual _tmpD;_LL14: _tmpE=_tmpC->kind;_tmpD=_tmpC->aliasqual;_LL15:;
return(int)_tmp16 == (int)3U &&((int)_tmp15 == (int)0U ||(int)_tmpD == (int)_tmp15);}}else{goto _LLE;}default: goto _LLE;}case 1U: _LL6: _LL7:
# 78
 goto _LL9;case 2U: _LL8: _LL9:
 return 1;default: _LLE: _LLF:
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmpB3;_tmpB3.tag=0U,({
struct _fat_ptr _tmpBC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(eff));_tmpB3.f1=_tmpBC;});_tmpB3;});void*_tmpF[1U];_tmpF[0]=& _tmp11;({struct _fat_ptr _tmpBD=({const char*_tmp10="bad effect |%s| passed to add_outlives_constraint";_tag_fat(_tmp10,sizeof(char),50U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpBD,_tag_fat(_tmpF,sizeof(void*),1U));});});}_LL3:;};}
# 99
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned loc){
# 101
eff=Cyc_Tcutil_normalize_effect(eff);{
struct Cyc_RgnOrder_RgnPO*_tmp17=({struct Cyc_RgnOrder_RgnPO*_tmp26=_cycalloc(sizeof(*_tmp26));*_tmp26=*po;_tmp26;});
# 104
if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){
({void*_tmp18=0U;({unsigned _tmpBF=loc;struct _fat_ptr _tmpBE=({const char*_tmp19="Invalid region ordering constraint; kind mismatch";_tag_fat(_tmp19,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpBF,_tmpBE,_tag_fat(_tmp18,sizeof(void*),0U));});});
return _tmp17;}{
# 109
void*_tmp1A=Cyc_Tcutil_compress(rgn);void*_tmp1B=_tmp1A;struct Cyc_Absyn_Tvar*_tmp25;switch(*((int*)_tmp1B)){case 2U: _LL1: _tmp25=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B)->f1;_LL2: {
# 111
struct Cyc_Dict_Dict _tmp1C=po->d;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(_tmp1C,_tmp25)){
void*_tmp1D=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(_tmp1C,_tmp25);
_tmp1C=({struct Cyc_Dict_Dict _tmpC1=_tmp1C;struct Cyc_Absyn_Tvar*_tmpC0=_tmp25;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmpC1,_tmpC0,Cyc_Absyn_join_eff(({void*_tmp1E[2U];_tmp1E[0]=eff,_tmp1E[1]=_tmp1D;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E,sizeof(void*),2U));})));});}else{
# 116
_tmp1C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp1C,_tmp25,eff);}
_tmp17->d=_tmp1C;
return _tmp17;}case 1U: _LL3: _LL4:
# 120
 Cyc_Unify_unify(rgn,Cyc_Absyn_heap_rgn_type);
goto _LL6;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B)->f1)){case 5U: _LL5: _LL6:
# 123
({void*_tmpC2=Cyc_Absyn_join_eff(({void*_tmp1F[2U];_tmp1F[0]=eff,_tmp1F[1]=po->these_outlive_heap;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1F,sizeof(void*),2U));}));_tmp17->these_outlive_heap=_tmpC2;});
return _tmp17;case 6U: _LL7: _LL8:
# 126
({void*_tmpC3=Cyc_Absyn_join_eff(({void*_tmp20[2U];_tmp20[0]=eff,_tmp20[1]=po->these_outlive_unique;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp20,sizeof(void*),2U));}));_tmp17->these_outlive_unique=_tmpC3;});
return _tmp17;case 7U: _LL9: _LLA:
# 129
({void*_tmp21=0U;({struct _fat_ptr _tmpC4=({const char*_tmp22="RgnOrder::add_outlives_constraint can't outlive RC for now";_tag_fat(_tmp22,sizeof(char),59U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpC4,_tag_fat(_tmp21,sizeof(void*),0U));});});default: goto _LLB;}default: _LLB: _LLC:
({void*_tmp23=0U;({struct _fat_ptr _tmpC5=({const char*_tmp24="RgnOrder::add_outlives_constraint passed a bad region";_tag_fat(_tmp24,sizeof(char),54U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpC5,_tag_fat(_tmp23,sizeof(void*),0U));});});}_LL0:;};};}
# 134
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened){
# 136
if(!opened &&((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,rgn))
({void*_tmp27=0U;({struct _fat_ptr _tmpC6=({const char*_tmp28="RgnOrder::add_youngest: repeated region";_tag_fat(_tmp28,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpC6,_tag_fat(_tmp27,sizeof(void*),0U));});});{
struct Cyc_RgnOrder_RgnPO*_tmp29=({struct Cyc_RgnOrder_RgnPO*_tmp2D=_cycalloc(sizeof(*_tmp2D));*_tmp2D=*po;_tmp2D;});
if(opened){
({struct Cyc_Dict_Dict _tmpC7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(po->d,rgn,Cyc_Absyn_empty_effect);_tmp29->d=_tmpC7;});
({void*_tmpC9=Cyc_Absyn_join_eff(({void*_tmp2A[2U];({void*_tmpC8=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(rgn));_tmp2A[0]=_tmpC8;}),_tmp2A[1]=_tmp29->opened_regions;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2A,sizeof(void*),2U));}));_tmp29->opened_regions=_tmpC9;});}else{
# 143
void*_tmp2B=Cyc_Absyn_join_eff(({void*_tmp2C[2U];({void*_tmpCA=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(po->youngest));_tmp2C[0]=_tmpCA;}),_tmp2C[1]=_tmp29->opened_regions;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2C,sizeof(void*),2U));}));
# 145
({struct Cyc_Dict_Dict _tmpCB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(po->d,rgn,_tmp2B);_tmp29->d=_tmpCB;});
_tmp29->youngest=rgn;}
# 148
return _tmp29;};}
# 150
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn){
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,rgn))
({void*_tmp2E=0U;({struct _fat_ptr _tmpCC=({const char*_tmp2F="RgnOrder::add_unordered: repeated region";_tag_fat(_tmp2F,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpCC,_tag_fat(_tmp2E,sizeof(void*),0U));});});{
struct Cyc_RgnOrder_RgnPO*_tmp30=({struct Cyc_RgnOrder_RgnPO*_tmp31=_cycalloc(sizeof(*_tmp31));*_tmp31=*po;_tmp31;});
({struct Cyc_Dict_Dict _tmpCD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp30->d,rgn,Cyc_Absyn_empty_effect);_tmp30->d=_tmpCD;});
return _tmp30;};}
# 158
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned loc){
# 161
struct Cyc_Dict_Dict _tmp32=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_empty)(Cyc_Absyn_tvar_cmp);
{struct Cyc_List_List*_tmp33=tvs;for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
if((int)(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp33->hd,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_RgnKind)
# 166
_tmp32=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp32,(struct Cyc_Absyn_Tvar*)_tmp33->hd,Cyc_Absyn_empty_effect);}}
# 169
if(!({void*_tmpCE=effect;Cyc_RgnOrder_valid_constraint(_tmpCE,Cyc_Absyn_var_type(fst_rgn));})){
# 171
({void*_tmp34=0U;({unsigned _tmpD0=loc;struct _fat_ptr _tmpCF=({const char*_tmp35="Invalid region ordering constraint; kind mismatch";_tag_fat(_tmp35,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpD0,_tmpCF,_tag_fat(_tmp34,sizeof(void*),0U));});});
return({struct Cyc_RgnOrder_RgnPO*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->d=_tmp32,_tmp36->these_outlive_heap=Cyc_Absyn_empty_effect,_tmp36->these_outlive_unique=Cyc_Absyn_empty_effect,_tmp36->youngest=fst_rgn,_tmp36->opened_regions=Cyc_Absyn_empty_effect;_tmp36;});}
# 177
_tmp32=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,void*v))Cyc_Dict_insert)(_tmp32,fst_rgn,effect);{
# 179
struct Cyc_RgnOrder_RgnPO*_tmp37=({struct Cyc_RgnOrder_RgnPO*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->d=_tmp32,_tmp38->these_outlive_heap=Cyc_Absyn_empty_effect,_tmp38->these_outlive_unique=Cyc_Absyn_empty_effect,_tmp38->youngest=fst_rgn,_tmp38->opened_regions=Cyc_Absyn_empty_effect;_tmp38;});
for(0;po != 0;po=po->tl){
_tmp37=Cyc_RgnOrder_add_outlives_constraint(_tmp37,(*((struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);}
# 183
return _tmp37;};}
# 208 "rgnorder.cyc"
static int Cyc_RgnOrder_contains_rgnseff(struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){
void*_tmp39=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(eff));void*_tmp3A=_tmp39;struct Cyc_List_List*_tmp3F;void*_tmp3E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A)->f1)){case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A)->f2 != 0){_LL1: _tmp3E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A)->f2)->hd;_LL2: {
# 211
void*_tmp3B=Cyc_Tcutil_compress(_tmp3E);void*_tmp3C=_tmp3B;struct Cyc_Absyn_Tvar*_tmp3D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C)->tag == 2U){_LL8: _tmp3D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C)->f1;_LL9:
 return Cyc_Absyn_tvar_cmp(_tmp3D,rgns_of_var)== 0;}else{_LLA: _LLB:
 return 0;}_LL7:;}}else{goto _LL5;}case 9U: _LL3: _tmp3F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A)->f2;_LL4:
# 216
 for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){
if(Cyc_RgnOrder_contains_rgnseff(rgns_of_var,(void*)_tmp3F->hd))
return 1;}
return 0;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;struct Cyc_List_List*todo;};
# 230
static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*env,void*eff){
void*_tmp40=Cyc_Tcutil_compress(eff);void*_tmp41=_tmp40;struct Cyc_List_List*_tmp46;struct Cyc_Absyn_Tvar*_tmp45;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd)->tag == 2U){_LL1: _tmp45=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd)->f1;_LL2:
# 233
{struct Cyc_List_List*_tmp42=env->seen;for(0;_tmp42 != 0;_tmp42=_tmp42->tl){
if(Cyc_Absyn_tvar_cmp(_tmp45,(struct Cyc_Absyn_Tvar*)_tmp42->hd)== 0)
return;}}
({struct Cyc_List_List*_tmpD1=({struct Cyc_List_List*_tmp43=_region_malloc(env->r,sizeof(*_tmp43));_tmp43->hd=_tmp45,_tmp43->tl=env->seen;_tmp43;});env->seen=_tmpD1;});
({struct Cyc_List_List*_tmpD2=({struct Cyc_List_List*_tmp44=_region_malloc(env->r,sizeof(*_tmp44));_tmp44->hd=_tmp45,_tmp44->tl=env->todo;_tmp44;});env->todo=_tmpD2;});
return;}else{goto _LL5;}}else{goto _LL5;}case 9U: _LL3: _tmp46=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2;_LL4:
# 240
 for(0;_tmp46 != 0;_tmp46=_tmp46->tl){
Cyc_RgnOrder_add_to_search(env,(void*)_tmp46->hd);}
return;default: goto _LL5;}else{_LL5: _LL6:
 return;}_LL0:;}
# 247
static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){
# 249
struct Cyc_RgnOrder_OutlivesEnv _tmp47=({struct Cyc_RgnOrder_OutlivesEnv _tmpB4;_tmpB4.r=listrgn,_tmpB4.seen=0,_tmpB4.todo=0;_tmpB4;});
void*r=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Kind*_tmp48=Cyc_Tcutil_type_kind(r);struct Cyc_Absyn_Kind*_tmp49=_tmp48;enum Cyc_Absyn_KindQual _tmp52;enum Cyc_Absyn_AliasQual _tmp51;_LL1: _tmp52=_tmp49->kind;_tmp51=_tmp49->aliasqual;_LL2:;
if((int)_tmp52 == (int)3U){
enum Cyc_Absyn_AliasQual _tmp4A=_tmp51;switch(_tmp4A){case Cyc_Absyn_Aliasable: _LL4: _LL5:
# 256
 Cyc_RgnOrder_add_to_search(& _tmp47,po->these_outlive_unique);
Cyc_RgnOrder_add_to_search(& _tmp47,po->these_outlive_heap);
goto _LL3;case Cyc_Absyn_Unique: _LL6: _LL7:
# 260
 Cyc_RgnOrder_add_to_search(& _tmp47,po->these_outlive_unique);goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
 goto _LLB;default: _LLA: _LLB:
# 265
 goto _LL3;}_LL3:;}else{
# 269
({void*_tmp4B=0U;({struct _fat_ptr _tmpD3=({const char*_tmp4C="RgnOrder: rgn not of correct kind";_tag_fat(_tmp4C,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD3,_tag_fat(_tmp4B,sizeof(void*),0U));});});}{
# 271
void*_tmp4D=r;struct Cyc_Absyn_Tvar*_tmp50;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4D)->tag == 2U){_LLD: _tmp50=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4D)->f1;_LLE:
# 273
({struct Cyc_List_List*_tmpD4=({struct Cyc_List_List*_tmp4E=_region_malloc(listrgn,sizeof(*_tmp4E));_tmp4E->hd=_tmp50,_tmp4E->tl=_tmp47.seen;_tmp4E;});_tmp47.seen=_tmpD4;});
({struct Cyc_List_List*_tmpD5=({struct Cyc_List_List*_tmp4F=_region_malloc(listrgn,sizeof(*_tmp4F));_tmp4F->hd=_tmp50,_tmp4F->tl=_tmp47.todo;_tmp4F;});_tmp47.todo=_tmpD5;});
goto _LL10;}else{_LLF: _LL10:
 return _tmp47;}_LLC:;};}
# 282
static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
struct _RegionHandle _tmp53=_new_region("listrgn");struct _RegionHandle*listrgn=& _tmp53;_push_region(listrgn);
{void*_tmp54=Cyc_Tcutil_compress(eff);void*_tmp55=_tmp54;void*_tmp81;void*_tmp80;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp55)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp55)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp55)->f2 != 0){_LL1: _tmp80=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp55)->f2)->hd;_LL2: {
# 287
void*_tmp56=Cyc_Tcutil_compress(_tmp80);void*_tmp57=_tmp56;struct Cyc_Absyn_Tvar*_tmp6B;switch(*((int*)_tmp57)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57)->f1)){case 6U: _LL8: _LL9: {
# 290
struct Cyc_Absyn_Kind*_tmp58=Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn));struct Cyc_Absyn_Kind*_tmp59=_tmp58;enum Cyc_Absyn_KindQual _tmp5C;enum Cyc_Absyn_AliasQual _tmp5B;_LL13: _tmp5C=_tmp59->kind;_tmp5B=_tmp59->aliasqual;_LL14:;{
int _tmp5A=(int)_tmp5C == (int)3U &&(int)_tmp5B != (int)Cyc_Absyn_Top;_npop_handler(0U);return _tmp5A;};}case 7U: _LLA: _LLB:
# 295
 if(({void*_tmpD6=Cyc_Tcutil_compress(rgn);_tmpD6 == Cyc_Absyn_refcnt_rgn_type;})){int _tmp5D=1;_npop_handler(0U);return _tmp5D;}
goto _LLD;case 5U: _LLC: _LLD: {
# 299
struct Cyc_Absyn_Kind*_tmp5E=Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn));struct Cyc_Absyn_Kind*_tmp5F=_tmp5E;enum Cyc_Absyn_KindQual _tmp62;enum Cyc_Absyn_AliasQual _tmp61;_LL16: _tmp62=_tmp5F->kind;_tmp61=_tmp5F->aliasqual;_LL17:;{
int _tmp60=(int)_tmp62 == (int)3U &&(int)_tmp61 == (int)Cyc_Absyn_Aliasable;_npop_handler(0U);return _tmp60;};}default: goto _LL10;}case 2U: _LLE: _tmp6B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57)->f1;_LLF:
# 302
 if(po == 0)
({void*_tmp63=0U;({struct _fat_ptr _tmpD7=({const char*_tmp64="RgnOrder: tvar without a partial-order";_tag_fat(_tmp64,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD7,_tag_fat(_tmp63,sizeof(void*),0U));});});{
# 314 "rgnorder.cyc"
struct Cyc_RgnOrder_OutlivesEnv _tmp65=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
while(_tmp65.todo != 0){
struct Cyc_Absyn_Tvar*_tmp66=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp65.todo))->hd;
_tmp65.todo=((struct Cyc_List_List*)_check_null(_tmp65.todo))->tl;
if(Cyc_Absyn_tvar_cmp(_tmp66,_tmp6B)== 0){
int _tmp67=1;_npop_handler(0U);return _tmp67;}
# 324
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp66))
Cyc_RgnOrder_add_to_search(& _tmp65,((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp66));else{
# 327
int _tmp68=0;_npop_handler(0U);return _tmp68;}}{
# 330
int _tmp69=0;_npop_handler(0U);return _tmp69;};};default: _LL10: _LL11: {
int _tmp6A=0;_npop_handler(0U);return _tmp6A;}}_LL7:;}}else{goto _LL5;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp55)->f2 != 0){_LL3: _tmp81=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp55)->f2)->hd;_LL4: {
# 334
void*_tmp6C=Cyc_Tcutil_compress(_tmp81);void*_tmp6D=_tmp6C;struct Cyc_Absyn_Tvar*_tmp7E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->tag == 2U){_LL19: _tmp7E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1;_LL1A:
# 337
 if(po == 0)
({void*_tmp6E=0U;({struct _fat_ptr _tmpD8=({const char*_tmp6F="RgnOrder: tvar without a partial-order";_tag_fat(_tmp6F,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD8,_tag_fat(_tmp6E,sizeof(void*),0U));});});{
# 342
struct Cyc_RgnOrder_OutlivesEnv _tmp70=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
struct Cyc_Absyn_Kind*_tmp71=Cyc_Tcutil_type_kind(rgn);struct Cyc_Absyn_Kind*_tmp72=_tmp71;enum Cyc_Absyn_KindQual _tmp7C;enum Cyc_Absyn_AliasQual _tmp7B;_LL1E: _tmp7C=_tmp72->kind;_tmp7B=_tmp72->aliasqual;_LL1F:;
# 345
if((int)_tmp7C == (int)3U){
if((int)_tmp7B == (int)0U){
if(Cyc_RgnOrder_contains_rgnseff(_tmp7E,po->these_outlive_heap)||
 Cyc_RgnOrder_contains_rgnseff(_tmp7E,po->these_outlive_unique)){
int _tmp73=1;_npop_handler(0U);return _tmp73;}}else{
# 351
if((int)_tmp7B == (int)1U){
if(Cyc_RgnOrder_contains_rgnseff(_tmp7E,po->these_outlive_unique)){
int _tmp74=1;_npop_handler(0U);return _tmp74;}}}}
# 356
while(_tmp70.todo != 0){
struct Cyc_Absyn_Tvar*_tmp75=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp70.todo))->hd;
_tmp70.todo=((struct Cyc_List_List*)_check_null(_tmp70.todo))->tl;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp75)){
void*_tmp76=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp75);
if(Cyc_RgnOrder_contains_rgnseff(_tmp7E,_tmp76)){
int _tmp77=1;_npop_handler(0U);return _tmp77;}
Cyc_RgnOrder_add_to_search(& _tmp70,_tmp76);}else{
({void*_tmp78=0U;({struct _fat_ptr _tmpD9=({const char*_tmp79="RgnOrder: type variable not in dict";_tag_fat(_tmp79,sizeof(char),36U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD9,_tag_fat(_tmp78,sizeof(void*),0U));});});}}{
# 366
int _tmp7A=0;_npop_handler(0U);return _tmp7A;};};}else{_LL1B: _LL1C: {
int _tmp7D=0;_npop_handler(0U);return _tmp7D;}}_LL18:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6: {
# 369
int _tmp7F=0;_npop_handler(0U);return _tmp7F;}}_LL0:;}
# 284 "rgnorder.cyc"
;_pop_region(listrgn);}
# 376 "rgnorder.cyc"
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_tmp82=Cyc_Tcutil_compress(eff);void*_tmp83=_tmp82;struct Cyc_List_List*_tmp84;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp83)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp83)->f1)->tag == 9U){_LL1: _tmp84=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp83)->f2;_LL2:
# 380
 for(0;_tmp84 != 0;_tmp84=_tmp84->tl){
if(!Cyc_RgnOrder_effect_outlives(po,(void*)_tmp84->hd,rgn))
return 0;}
return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_RgnOrder_atomic_effect_outlives(po,eff,rgn);}_LL0:;};}
# 388
static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_tmp85=Cyc_Tcutil_compress(eff);void*_tmp86=_tmp85;void*_tmp8D;void*_tmp8C;struct Cyc_List_List*_tmp8B;switch(*((int*)_tmp86)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f1)){case 9U: _LL1: _tmp8B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f2;_LL2:
# 392
 for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
Cyc_RgnOrder_pin_effect((void*)_tmp8B->hd,bd);}
return;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f2 != 0){_LL3: _tmp8C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f2)->hd;_LL4: {
# 396
void*_tmp87=Cyc_Tcutil_compress(_tmp8C);void*_tmp88=_tmp87;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp88)->tag == 1U){_LLC: _LLD:
 Cyc_Unify_unify(_tmp8C,Cyc_Absyn_uint_type);return;}else{_LLE: _LLF:
 return;}_LLB:;}}else{goto _LL9;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f2 != 0){_LL5: _tmp8D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp86)->f2)->hd;_LL6: {
# 401
void*_tmp89=Cyc_Tcutil_compress(_tmp8D);void*_tmp8A=_tmp89;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8A)->tag == 1U){_LL11: _LL12:
 Cyc_Unify_unify(_tmp8D,bd);return;}else{_LL13: _LL14:
 return;}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _LL8:
# 405
 Cyc_Unify_unify(eff,Cyc_Absyn_empty_effect);return;default: _LL9: _LLA:
 return;}_LL0:;};}
# 412
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*po,void*rgn1,void*rgn2){
return({struct Cyc_RgnOrder_RgnPO*_tmpDB=po;void*_tmpDA=Cyc_Absyn_access_eff(rgn1);Cyc_RgnOrder_effect_outlives(_tmpDB,_tmpDA,rgn2);});}
# 416
static int Cyc_RgnOrder_eff_outlives_atomic_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
# 418
eff2=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff2));{
void*_tmp8E=eff2;void*_tmp97;void*_tmp96;struct Cyc_List_List*_tmp95;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f1)){case 9U: _LL1: _tmp95=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f2;_LL2:
# 421
 for(0;_tmp95 != 0;_tmp95=_tmp95->tl){
if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)_tmp95->hd))
return 1;}
return 0;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f2 != 0){_LL3: _tmp96=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f2)->hd;_LL4:
 return Cyc_RgnOrder_effect_outlives(po,eff1,_tmp96);}else{goto _LL7;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f2 != 0){_LL5: _tmp97=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f2)->hd;_LL6: {
# 427
void*_tmp8F=eff1;void*_tmp94;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2 != 0){_LLA: _tmp94=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F)->f2)->hd;_LLB:
# 429
{struct _tuple0 _tmp90=({struct _tuple0 _tmpB5;({void*_tmpDD=Cyc_Tcutil_compress(_tmp97);_tmpB5.f1=_tmpDD;}),({void*_tmpDC=Cyc_Tcutil_compress(_tmp94);_tmpB5.f2=_tmpDC;});_tmpB5;});struct _tuple0 _tmp91=_tmp90;struct Cyc_Absyn_Tvar*_tmp93;struct Cyc_Absyn_Tvar*_tmp92;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91.f2)->tag == 2U){_LLF: _tmp93=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91.f1)->f1;_tmp92=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91.f2)->f1;_LL10:
 return Cyc_Absyn_tvar_cmp(_tmp93,_tmp92)== 0;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 433
goto _LLD;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _LLD:
 return eff1 == Cyc_Absyn_heap_rgn_type;}_LL9:;}}else{goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
# 436
 return eff1 == Cyc_Absyn_heap_rgn_type;}_LL0:;};}
# 440
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
eff1=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff1));{
void*_tmp98=eff1;struct Cyc_List_List*_tmp99;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98)->f1)->tag == 9U){_LL1: _tmp99=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98)->f2;_LL2:
# 444
 for(0;_tmp99 != 0;_tmp99=_tmp99->tl){
if(!Cyc_RgnOrder_eff_outlives_eff(po,(void*)_tmp99->hd,eff2))
return 0;}
return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,eff2);}_LL0:;};}
# 462 "rgnorder.cyc"
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin){
# 466
{struct Cyc_List_List*_tmp9A=constraints;for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
struct _tuple0*_tmp9B=(struct _tuple0*)_tmp9A->hd;struct _tuple0*_tmp9C=_tmp9B;void*_tmp9F;_LL1: _tmp9F=_tmp9C->f2;_LL2:;{
void*_tmp9D=Cyc_Tcutil_compress(_tmp9F);void*_tmp9E=_tmp9D;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9E)->tag == 1U){_LL4: _LL5:
# 470
 if(do_pin)
Cyc_Unify_unify(_tmp9F,default_bound);
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}}
# 476
{struct Cyc_List_List*_tmpA0=constraints;for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
struct _tuple0*_tmpA1=(struct _tuple0*)_tmpA0->hd;struct _tuple0*_tmpA2=_tmpA1;void*_tmpA4;void*_tmpA3;_LL9: _tmpA4=_tmpA2->f1;_tmpA3=_tmpA2->f2;_LLA:;
if(do_pin)
Cyc_RgnOrder_pin_effect(_tmpA4,_tmpA3);
if(!Cyc_RgnOrder_effect_outlives(po,_tmpA4,_tmpA3))
return 0;}}
# 483
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 487
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,po->d);
struct _tuple12 _tmpA5=*((struct _tuple12*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(Cyc_Core_heap_region,po->d);
({void*_tmpA6=0U;({struct Cyc___cycFILE*_tmpDF=Cyc_stderr;struct _fat_ptr _tmpDE=({const char*_tmpA7="region po:\n";_tag_fat(_tmpA7,sizeof(char),12U);});Cyc_fprintf(_tmpDF,_tmpDE,_tag_fat(_tmpA6,sizeof(void*),0U));});});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next)(iter,& _tmpA5)){
({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmpB7;_tmpB7.tag=0U,_tmpB7.f1=(struct _fat_ptr)((struct _fat_ptr)*(_tmpA5.f1)->name);_tmpB7;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0U,({
struct _fat_ptr _tmpE0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpA5.f2));_tmpB6.f1=_tmpE0;});_tmpB6;});void*_tmpA8[2U];_tmpA8[0]=& _tmpAA,_tmpA8[1]=& _tmpAB;({struct Cyc___cycFILE*_tmpE2=Cyc_stderr;struct _fat_ptr _tmpE1=({const char*_tmpA9="  %s outlived by %s\n";_tag_fat(_tmpA9,sizeof(char),21U);});Cyc_fprintf(_tmpE2,_tmpE1,_tag_fat(_tmpA8,sizeof(void*),2U));});});}
({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmpB8;_tmpB8.tag=0U,({
struct _fat_ptr _tmpE3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(po->these_outlive_heap));_tmpB8.f1=_tmpE3;});_tmpB8;});void*_tmpAC[1U];_tmpAC[0]=& _tmpAE;({struct Cyc___cycFILE*_tmpE5=Cyc_stderr;struct _fat_ptr _tmpE4=({const char*_tmpAD="  these outlive heap: %s\n";_tag_fat(_tmpAD,sizeof(char),26U);});Cyc_fprintf(_tmpE5,_tmpE4,_tag_fat(_tmpAC,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmpB1=({struct Cyc_String_pa_PrintArg_struct _tmpB9;_tmpB9.tag=0U,({
struct _fat_ptr _tmpE6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(po->these_outlive_unique));_tmpB9.f1=_tmpE6;});_tmpB9;});void*_tmpAF[1U];_tmpAF[0]=& _tmpB1;({struct Cyc___cycFILE*_tmpE8=Cyc_stderr;struct _fat_ptr _tmpE7=({const char*_tmpB0="  these outlive unique: %s\n";_tag_fat(_tmpB0,sizeof(char),28U);});Cyc_fprintf(_tmpE8,_tmpE7,_tag_fat(_tmpAF,sizeof(void*),1U));});});}
