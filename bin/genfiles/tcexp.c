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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 111 "core.h"
extern void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
extern struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
extern void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 305
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 495
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 890 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 907
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 909
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 912
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 914
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 916
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned i);
# 919
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 923
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 925
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 942
void*Cyc_Absyn_exn_type (void);
# 950
extern void*Cyc_Absyn_fat_bound_type;
# 952
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 954
void*Cyc_Absyn_bounds_one (void);
# 956
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 960
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 962
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 964
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 968
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 976
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 980
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void);
# 1058
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1123
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1125
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1127
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1130
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1132
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "warn.h"
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 31 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 53
int Cyc_Tcutil_is_bound_one(void*);
# 55
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 65
void*Cyc_Tcutil_pointer_elt_type(void*);
# 67
void*Cyc_Tcutil_pointer_region(void*);
# 70
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 73
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 78
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 80
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 85
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 88
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 95
void*Cyc_Tcutil_copy_type(void*);
# 98
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 101
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 106
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 114
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*,void*,struct Cyc_List_List*);
# 118
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 120
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 125
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 128
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 146
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 156
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 164
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 180
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 186
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 189
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 203
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 208
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 219
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 221
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 223
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 234
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 237
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 242
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 247
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple15{int f1;void*f2;};
# 251
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 274
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 285
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 296
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 299
int Cyc_Tcutil_zeroable_type(void*);
# 303
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 306
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 308
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);struct _tuple16{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 54
extern int Cyc_Evexp_okay_szofarg(void*t);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 76 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 93
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 120
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 124
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 143
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 145
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*eff);
# 150
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*po);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
extern struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 33
extern struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr msg_part);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);
# 44 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 46
Cyc_Warn_verr2(loc,ap);
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
return*topt;}
# 51
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 56
void Cyc_Tcexp_check_consume(unsigned loc,void*t,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
# 61
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp517;_tmp517.tag=0U,({struct _fat_ptr _tmp63F=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp517.f1=_tmp63F;});_tmp517;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp640=loc;Cyc_Warn_err2(_tmp640,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 65
static struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->v=tenv_tvs;_tmp9;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
return({struct Cyc_Absyn_PtrInfo _tmp518;_tmp518.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp641=Cyc_Absyn_empty_tqual(0U);_tmp518.elt_tq=_tmp641;}),(_tmp518.ptr_atts).rgn=rt,(_tmp518.ptr_atts).nullable=nbl,(_tmp518.ptr_atts).bounds=b,(_tmp518.ptr_atts).zero_term=zt,(_tmp518.ptr_atts).ptrloc=0;_tmp518;});}
# 75
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 79
if(topt == 0){
# 81
({void*_tmp642=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp642;});
return;}{
# 84
void*t=*topt;
void*_tmpC=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmpC;void*_tmpD=_stmttmp0;struct Cyc_Absyn_Tqual _tmpF;void*_tmpE;union Cyc_Absyn_AggrInfo _tmp10;switch(*((int*)_tmpD)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 20U){_LL1: _tmp10=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp10;
# 87
{union Cyc_Absyn_AggrInfo _tmp11=info;struct Cyc_Absyn_Aggrdecl*_tmp12;if((_tmp11.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp519;_tmp519.tag=0U,({struct _fat_ptr _tmp643=({const char*_tmp15="struct type improperly set";_tag_fat(_tmp15,sizeof(char),27U);});_tmp519.f1=_tmp643;});_tmp519;});void*_tmp13[1U];_tmp13[0]=& _tmp14;({unsigned _tmp644=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp644,_tag_fat(_tmp13,sizeof(void*),1U));});});}else{_LLC: _tmp12=*(_tmp11.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp12;
({void*_tmp645=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=29U,_tmp16->f1=ad->name,_tmp16->f2=0,_tmp16->f3=des,_tmp16->f4=ad;_tmp16;});e->r=_tmp645;});}}_LL9:;}
# 91
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).elt_type;_tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).tq;_LL4: {void*at=_tmpE;struct Cyc_Absyn_Tqual aq=_tmpF;
({void*_tmp646=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=26U,_tmp17->f1=des;_tmp17;});e->r=_tmp646;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp647=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=30U,_tmp18->f1=t,_tmp18->f2=des;_tmp18;});e->r=_tmp647;});goto _LL0;default: _LL7: _LL8:
({void*_tmp648=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=26U,_tmp19->f1=des;_tmp19;});e->r=_tmp648;});goto _LL0;}_LL0:;}}
# 101
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 105
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1A=e->r;void*_stmttmp1=_tmp1A;void*_tmp1B=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->f2 == 0){_LL1: _LL2:
# 108
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51A;_tmp51A.tag=0U,({struct _fat_ptr _tmp649=({const char*_tmp1E="assignment in test";_tag_fat(_tmp1E,sizeof(char),19U);});_tmp51A.f1=_tmp649;});_tmp51A;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1D;({unsigned _tmp64A=e->loc;Cyc_Warn_warn2(_tmp64A,_tag_fat(_tmp1C,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 116
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51F;_tmp51F.tag=0U,({struct _fat_ptr _tmp64B=({const char*_tmp27="test of ";_tag_fat(_tmp27,sizeof(char),9U);});_tmp51F.f1=_tmp64B;});_tmp51F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51E;_tmp51E.tag=0U,_tmp51E.f1=msg_part;_tmp51E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51D;_tmp51D.tag=0U,({struct _fat_ptr _tmp64C=({const char*_tmp26=" has type ";_tag_fat(_tmp26,sizeof(char),11U);});_tmp51D.f1=_tmp64C;});_tmp51D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp23=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp51C;_tmp51C.tag=3U,_tmp51C.f1=(void*)e->topt;_tmp51C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51B;_tmp51B.tag=0U,({
struct _fat_ptr _tmp64D=({const char*_tmp25=" instead of integral or pointer type";_tag_fat(_tmp25,sizeof(char),37U);});_tmp51B.f1=_tmp64D;});_tmp51B;});void*_tmp1F[5U];_tmp1F[0]=& _tmp20,_tmp1F[1]=& _tmp21,_tmp1F[2]=& _tmp22,_tmp1F[3]=& _tmp23,_tmp1F[4]=& _tmp24;({unsigned _tmp64E=e->loc;Cyc_Warn_err2(_tmp64E,_tag_fat(_tmp1F,sizeof(void*),5U));});});}
# 138 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 143
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp28;union Cyc_Absyn_Cnst _tmp29=_stmttmp2;struct _fat_ptr _tmp2A;struct _fat_ptr _tmp2B;int _tmp2D;enum Cyc_Absyn_Sign _tmp2C;int _tmp2E;switch((_tmp29.String_c).tag){case 2U: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 return Cyc_Absyn_uchar_type;default: _LL5: _LL6:
 return Cyc_Absyn_char_type;}case 3U: _LL7: _LL8:
 return Cyc_Absyn_wchar_type();case 4U: if(((_tmp29.Short_c).val).f1 == Cyc_Absyn_Unsigned){_LL9: _LLA:
 return Cyc_Absyn_ushort_type;}else{_LLB: _LLC:
 return Cyc_Absyn_sshort_type;}case 6U: if(((_tmp29.LongLong_c).val).f1 == Cyc_Absyn_Unsigned){_LLD: _LLE:
 return Cyc_Absyn_ulonglong_type;}else{_LLF: _LL10:
 return Cyc_Absyn_slonglong_type;}case 7U: _LL11: _tmp2E=((_tmp29.Float_c).val).f2;_LL12: {int i=_tmp2E;
# 158
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp2F=Cyc_Tcutil_compress(*topt);void*_stmttmp3=_tmp2F;void*_tmp30=_stmttmp3;int _tmp31;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->tag == 2U){_LL1C: _tmp31=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->f1;_LL1D: {int i=_tmp31;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E: _LL1F:
 return Cyc_Absyn_gen_float_type((unsigned)i);}_LL1B:;}}case 5U: _LL13: _tmp2C=((_tmp29.Int_c).val).f1;_tmp2D=((_tmp29.Int_c).val).f2;_LL14: {enum Cyc_Absyn_Sign csn=_tmp2C;int i=_tmp2D;
# 168
if(topt == 0)
return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_stmttmp4=_tmp32;void*_tmp33=_stmttmp4;void*_tmp34;enum Cyc_Absyn_Sign _tmp35;enum Cyc_Absyn_Sign _tmp36;enum Cyc_Absyn_Sign _tmp37;enum Cyc_Absyn_Sign _tmp38;switch(*((int*)_tmp33)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp38=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp38;
# 172
({union Cyc_Absyn_Cnst _tmp64F=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp64F;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp37=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp37;
# 175
({union Cyc_Absyn_Cnst _tmp650=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp650;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp36=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp36;
_tmp35=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp35=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp35;
# 181
({union Cyc_Absyn_Cnst _tmp651=Cyc_Absyn_Int_c(sn,i);*c=_tmp651;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2 != 0){_LL2B: _tmp34=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2)->hd;_LL2C: {void*t1=_tmp34;
# 190
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=9U,({struct Cyc_Absyn_Exp*_tmp652=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp39->f1=_tmp652;});_tmp39;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 185
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp653=({union Cyc_Absyn_Cnst _tmp520;(_tmp520.Null_c).tag=1U,(_tmp520.Null_c).val=0;_tmp520;});*c=_tmp653;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 192
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 195
 if(topt != 0){
void*_tmp3A=Cyc_Tcutil_compress(*topt);void*_stmttmp5=_tmp3A;void*_tmp3B=_stmttmp5;void*_tmp3C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->tag == 3U){_LL30: _tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp3C;
# 198
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp522;_tmp522.tag=0U,({struct _fat_ptr _tmp654=({const char*_tmp40="Used NULL when expecting a value of type ";_tag_fat(_tmp40,sizeof(char),42U);});_tmp522.f1=_tmp654;});_tmp522;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp521;_tmp521.tag=2U,_tmp521.f1=(void*)*topt;_tmp521;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3E,_tmp3D[1]=& _tmp3F;({unsigned _tmp655=e->loc;Cyc_Warn_err2(_tmp655,_tag_fat(_tmp3D,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 203
struct Cyc_Absyn_PtrInfo _tmp41=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp41;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8U: _LL17: _tmp2B=(_tmp29.String_c).val;_LL18: {struct _fat_ptr s=_tmp2B;
# 208
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp2A=s;goto _LL1A;}default: _LL19: _tmp2A=(_tmp29.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp2A;
# 211
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 215
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp656=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp656,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 220
void*_tmp42=({void*_tmp65A=string_elt_typ;void*_tmp659=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp658=Cyc_Absyn_const_tqual(0U);void*_tmp657=
Cyc_Absyn_thin_bounds_exp(elen);
# 220
Cyc_Absyn_atb_type(_tmp65A,_tmp659,_tmp658,_tmp657,Cyc_Absyn_true_type);});void*t=_tmp42;
# 222
if(topt == 0)
return t;{
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_stmttmp6=_tmp43;void*_tmp44=_stmttmp6;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 4U: _LL35: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp45;
# 228
return({void*_tmp65D=string_elt_typ;struct Cyc_Absyn_Tqual _tmp65C=tq;struct Cyc_Absyn_Exp*_tmp65B=elen;Cyc_Absyn_array_type(_tmp65D,_tmp65C,_tmp65B,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 233
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp660=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp65F=loc;void*_tmp65E=t;Cyc_Tcutil_silent_castable(_tmp660,_tmp65F,_tmp65E,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 238
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 245
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp46=*((void**)_check_null(b));void*_stmttmp7=_tmp46;void*_tmp47=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp48;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4A;struct Cyc_Absyn_Fndecl*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;struct _tuple1*_tmp4D;switch(*((int*)_tmp47)){case 0U: _LL1: _tmp4D=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp47)->f1;_LL2: {struct _tuple1*q=_tmp4D;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp523;_tmp523.tag=0U,({struct _fat_ptr _tmp661=({const char*_tmp50="unresolved binding in tcVar";_tag_fat(_tmp50,sizeof(char),28U);});_tmp523.f1=_tmp661;});_tmp523;});void*_tmp4E[1U];_tmp4E[0]=& _tmp4F;({unsigned _tmp662=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp662,_tag_fat(_tmp4E,sizeof(void*),1U));});});}case 1U: _LL3: _tmp4C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp47)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;
# 251
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp4B=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp47)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp4B;
# 258
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp4A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp47)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp4A;
_tmp49=vd;goto _LLA;}case 4U: _LL9: _tmp49=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp47)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp49;
_tmp48=vd;goto _LLC;}default: _LLB: _tmp48=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp47)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp48;
# 264
if(te->allow_valueof){
void*_tmp51=Cyc_Tcutil_compress(vd->type);void*_stmttmp8=_tmp51;void*_tmp52=_stmttmp8;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2)->hd;_LLF: {void*i=_tmp53;
({void*_tmp663=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=39U,_tmp54->f1=i;_tmp54;});e->r=_tmp663;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 269
return vd->type;}}_LL0:;}
# 273
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 280
void*_tmp55=fmt->r;void*_stmttmp9=_tmp55;void*_tmp56=_stmttmp9;struct _fat_ptr _tmp57;struct _fat_ptr _tmp58;switch(*((int*)_tmp56)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).tag == 8){_LL1: _tmp58=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp58;
_tmp57=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp57=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp57;
# 283
struct Cyc_List_List*_tmp59=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);struct Cyc_List_List*desc_types=_tmp59;
if(opt_args == 0)
return;{
struct Cyc_List_List*_tmp5A=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5A;
# 288
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp665=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp664=e;Cyc_Tcutil_coerce_arg(_tmp665,_tmp664,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp527;_tmp527.tag=0U,({struct _fat_ptr _tmp666=({const char*_tmp61="descriptor has type ";_tag_fat(_tmp61,sizeof(char),21U);});_tmp527.f1=_tmp666;});_tmp527;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp526;_tmp526.tag=2U,_tmp526.f1=(void*)t;_tmp526;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp525;_tmp525.tag=0U,({
struct _fat_ptr _tmp667=({const char*_tmp60=" but argument has type ";_tag_fat(_tmp60,sizeof(char),24U);});_tmp525.f1=_tmp667;});_tmp525;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp524;_tmp524.tag=3U,_tmp524.f1=(void*)e->topt;_tmp524;});void*_tmp5B[4U];_tmp5B[0]=& _tmp5C,_tmp5B[1]=& _tmp5D,_tmp5B[2]=& _tmp5E,_tmp5B[3]=& _tmp5F;({unsigned _tmp668=e->loc;Cyc_Tcexp_err_and_explain(_tmp668,_tag_fat(_tmp5B,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp669=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)arg_cnt,_tmp62->tl=*alias_arg_exps;_tmp62;});*alias_arg_exps=_tmp669;});
Cyc_Tcexp_check_consume(e->loc,t,e);}
# 302
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp528;_tmp528.tag=0U,({struct _fat_ptr _tmp66A=({const char*_tmp65="too few arguments";_tag_fat(_tmp65,sizeof(char),18U);});_tmp528.f1=_tmp66A;});_tmp528;});void*_tmp63[1U];_tmp63[0]=& _tmp64;({unsigned _tmp66B=fmt->loc;Cyc_Warn_err2(_tmp66B,_tag_fat(_tmp63,sizeof(void*),1U));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp529;_tmp529.tag=0U,({struct _fat_ptr _tmp66C=({const char*_tmp68="too many arguments";_tag_fat(_tmp68,sizeof(char),19U);});_tmp529.f1=_tmp66C;});_tmp529;});void*_tmp66[1U];_tmp66[0]=& _tmp67;({unsigned _tmp66D=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp66D,_tag_fat(_tmp66,sizeof(void*),1U));});});
# 307
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 310
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 314
 if(opt_args == 0)
return;
{struct Cyc_List_List*_tmp69=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp69;for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)args->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt),(struct Cyc_Absyn_Exp*)args->hd);}}
# 320
return;}_LL0:;}
# 324
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 326
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6A=p;switch(_tmp6A){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 330
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52B;_tmp52B.tag=0U,({struct _fat_ptr _tmp66E=({const char*_tmp6E="expecting numeric type but found ";_tag_fat(_tmp6E,sizeof(char),34U);});_tmp52B.f1=_tmp66E;});_tmp52B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52A;_tmp52A.tag=2U,_tmp52A.f1=(void*)t;_tmp52A;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp66F=loc;Cyc_Warn_err2(_tmp66F,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 334
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D;_tmp52D.tag=0U,({struct _fat_ptr _tmp670=({const char*_tmp72="expecting integral or * type but found ";_tag_fat(_tmp72,sizeof(char),40U);});_tmp52D.f1=_tmp670;});_tmp52D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52C;_tmp52C.tag=2U,_tmp52C.f1=(void*)t;_tmp52C;});void*_tmp6F[2U];_tmp6F[0]=& _tmp70,_tmp6F[1]=& _tmp71;({unsigned _tmp671=loc;Cyc_Warn_err2(_tmp671,_tag_fat(_tmp6F,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 339
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52F;_tmp52F.tag=0U,({struct _fat_ptr _tmp672=({const char*_tmp76="expecting integral type but found ";_tag_fat(_tmp76,sizeof(char),35U);});_tmp52F.f1=_tmp672;});_tmp52F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52E;_tmp52E.tag=2U,_tmp52E.f1=(void*)t;_tmp52E;});void*_tmp73[2U];_tmp73[0]=& _tmp74,_tmp73[1]=& _tmp75;({unsigned _tmp673=loc;Cyc_Warn_err2(_tmp673,_tag_fat(_tmp73,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 343
{void*_tmp77=t;void*_tmp78;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp78;
# 345
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp79;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp530;_tmp530.tag=0U,({struct _fat_ptr _tmp674=({const char*_tmp7C="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp7C,sizeof(char),53U);});_tmp530.f1=_tmp674;});_tmp530;});void*_tmp7A[1U];_tmp7A[0]=& _tmp7B;({unsigned _tmp675=loc;Cyc_Warn_err2(_tmp675,_tag_fat(_tmp7A,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp532;_tmp532.tag=0U,({struct _fat_ptr _tmp676=({const char*_tmp80="numelts requires pointer type, not ";_tag_fat(_tmp80,sizeof(char),36U);});_tmp532.f1=_tmp676;});_tmp532;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp531;_tmp531.tag=2U,_tmp531.f1=(void*)t;_tmp531;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F;({unsigned _tmp677=loc;Cyc_Warn_err2(_tmp677,_tag_fat(_tmp7D,sizeof(void*),2U));});});}_LLD:;}
# 351
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp533;_tmp533.tag=0U,({struct _fat_ptr _tmp678=({const char*_tmp83="Non-unary primop";_tag_fat(_tmp83,sizeof(char),17U);});_tmp533.f1=_tmp678;});_tmp533;});void*_tmp81[1U];_tmp81[0]=& _tmp82;({unsigned _tmp679=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp679,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 358
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp84=(void*)_check_null(e1->topt);void*t1=_tmp84;
void*_tmp85=(void*)_check_null(e2->topt);void*t2=_tmp85;
void*_tmp86=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp86;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,Cyc_Absyn_No_coercion);
return new_typ;}
# 368
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 371
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp536;_tmp536.tag=0U,({struct _fat_ptr _tmp67A=({const char*_tmp8C="type ";_tag_fat(_tmp8C,sizeof(char),6U);});_tmp536.f1=_tmp67A;});_tmp536;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp89=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp535;_tmp535.tag=3U,_tmp535.f1=(void*)e1->topt;_tmp535;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp534;_tmp534.tag=0U,({struct _fat_ptr _tmp67B=({const char*_tmp8B=" cannot be used here";_tag_fat(_tmp8B,sizeof(char),21U);});_tmp534.f1=_tmp67B;});_tmp534;});void*_tmp87[3U];_tmp87[0]=& _tmp88,_tmp87[1]=& _tmp89,_tmp87[2]=& _tmp8A;({struct Cyc_Tcenv_Tenv*_tmp67D=te;unsigned _tmp67C=e1->loc;Cyc_Tcexp_expr_err(_tmp67D,_tmp67C,0,_tag_fat(_tmp87,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp539;_tmp539.tag=0U,({struct _fat_ptr _tmp67E=({const char*_tmp92="type ";_tag_fat(_tmp92,sizeof(char),6U);});_tmp539.f1=_tmp67E;});_tmp539;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp538;_tmp538.tag=3U,_tmp538.f1=(void*)e2->topt;_tmp538;});struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp537;_tmp537.tag=0U,({struct _fat_ptr _tmp67F=({const char*_tmp91=" cannot be used here";_tag_fat(_tmp91,sizeof(char),21U);});_tmp537.f1=_tmp67F;});_tmp537;});void*_tmp8D[3U];_tmp8D[0]=& _tmp8E,_tmp8D[1]=& _tmp8F,_tmp8D[2]=& _tmp90;({struct Cyc_Tcenv_Tenv*_tmp681=te;unsigned _tmp680=e2->loc;Cyc_Tcexp_expr_err(_tmp681,_tmp680,0,_tag_fat(_tmp8D,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 378
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp93=t1;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;struct Cyc_Absyn_Tqual _tmp95;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_type;_tmp95=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_tq;_tmp96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).rgn;_tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).nullable;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp94;struct Cyc_Absyn_Tqual tq=_tmp95;void*r=_tmp96;void*n=_tmp97;void*b=_tmp98;void*zt=_tmp99;
# 383
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A;_tmp53A.tag=0U,({struct _fat_ptr _tmp682=({const char*_tmp9C="can't do arithmetic on abstract pointer type";_tag_fat(_tmp9C,sizeof(char),45U);});_tmp53A.f1=_tmp682;});_tmp53A;});void*_tmp9A[1U];_tmp9A[0]=& _tmp9B;({unsigned _tmp683=e1->loc;Cyc_Warn_err2(_tmp683,_tag_fat(_tmp9A,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0U,({struct _fat_ptr _tmp684=({const char*_tmp9F="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp9F,sizeof(char),49U);});_tmp53B.f1=_tmp684;});_tmp53B;});void*_tmp9D[1U];_tmp9D[0]=& _tmp9E;({unsigned _tmp685=e1->loc;Cyc_Warn_err2(_tmp685,_tag_fat(_tmp9D,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D;_tmp53D.tag=0U,({struct _fat_ptr _tmp686=({const char*_tmpA3="expecting int but found ";_tag_fat(_tmpA3,sizeof(char),25U);});_tmp53D.f1=_tmp686;});_tmp53D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53C;_tmp53C.tag=2U,_tmp53C.f1=(void*)t2;_tmp53C;});void*_tmpA0[2U];_tmpA0[0]=& _tmpA1,_tmpA0[1]=& _tmpA2;({unsigned _tmp687=e2->loc;Cyc_Warn_err2(_tmp687,_tag_fat(_tmpA0,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmpA4=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmpA4;
if(eopt == 0)
return t1;
# 394
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmpA5=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmpA5;struct _tuple16 _tmpA6=_stmttmpA;int _tmpA8;unsigned _tmpA7;_LL6: _tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL7: {unsigned i=_tmpA7;int known=_tmpA8;
if(known && i == (unsigned)1)
({void*_tmpA9=0U;({unsigned _tmp689=e1->loc;struct _fat_ptr _tmp688=({const char*_tmpAA="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpAA,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp689,_tmp688,_tag_fat(_tmpA9,sizeof(void*),0U));});});}}{
# 405
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAB=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=3U,(_tmpAC->f1).elt_type=et,(_tmpAC->f1).elt_tq=tq,((_tmpAC->f1).ptr_atts).rgn=r,((_tmpAC->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpAC->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpAC->f1).ptr_atts).zero_term=zt,((_tmpAC->f1).ptr_atts).ptrloc=0;_tmpAC;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpAB;
# 408
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 415
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0U,({struct _fat_ptr _tmp68A=({const char*_tmpB7="pointer arithmetic on values of different ";_tag_fat(_tmpB7,sizeof(char),43U);});_tmp543.f1=_tmp68A;});_tmp543;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0U,({
struct _fat_ptr _tmp68B=({const char*_tmpB6="types (";_tag_fat(_tmpB6,sizeof(char),8U);});_tmp542.f1=_tmp68B;});_tmp542;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp541;_tmp541.tag=2U,_tmp541.f1=(void*)t1;_tmp541;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0U,({struct _fat_ptr _tmp68C=({const char*_tmpB5=" != ";_tag_fat(_tmpB5,sizeof(char),5U);});_tmp540.f1=_tmp68C;});_tmp540;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53F;_tmp53F.tag=2U,_tmp53F.f1=(void*)t2;_tmp53F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E;_tmp53E.tag=0U,({struct _fat_ptr _tmp68D=({const char*_tmpB4=")";_tag_fat(_tmpB4,sizeof(char),2U);});_tmp53E.f1=_tmp68D;});_tmp53E;});void*_tmpAD[6U];_tmpAD[0]=& _tmpAE,_tmpAD[1]=& _tmpAF,_tmpAD[2]=& _tmpB0,_tmpAD[3]=& _tmpB1,_tmpAD[4]=& _tmpB2,_tmpAD[5]=& _tmpB3;({unsigned _tmp68E=e1->loc;Cyc_Tcexp_err_and_explain(_tmp68E,_tag_fat(_tmpAD,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 427
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp68F=t1_elt;Cyc_Unify_unify(_tmp68F,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp549;_tmp549.tag=0U,({struct _fat_ptr _tmp690=({const char*_tmpC2="pointer arithmetic on values of different ";_tag_fat(_tmpC2,sizeof(char),43U);});_tmp549.f1=_tmp690;});_tmp549;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp548;_tmp548.tag=0U,({
struct _fat_ptr _tmp691=({const char*_tmpC1="types(";_tag_fat(_tmpC1,sizeof(char),7U);});_tmp548.f1=_tmp691;});_tmp548;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp547;_tmp547.tag=2U,_tmp547.f1=(void*)t1;_tmp547;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp546;_tmp546.tag=0U,({struct _fat_ptr _tmp692=({const char*_tmpC0=" != ";_tag_fat(_tmpC0,sizeof(char),5U);});_tmp546.f1=_tmp692;});_tmp546;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp545;_tmp545.tag=2U,_tmp545.f1=(void*)t2;_tmp545;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp544;_tmp544.tag=0U,({struct _fat_ptr _tmp693=({const char*_tmpBF=")";_tag_fat(_tmpBF,sizeof(char),2U);});_tmp544.f1=_tmp693;});_tmp544;});void*_tmpB8[6U];_tmpB8[0]=& _tmpB9,_tmpB8[1]=& _tmpBA,_tmpB8[2]=& _tmpBB,_tmpB8[3]=& _tmpBC,_tmpB8[4]=& _tmpBD,_tmpB8[5]=& _tmpBE;({unsigned _tmp694=e1->loc;Cyc_Tcexp_err_and_explain(_tmp694,_tag_fat(_tmpB8,sizeof(void*),6U));});});
# 432
({void*_tmpC3=0U;({unsigned _tmp696=e1->loc;struct _fat_ptr _tmp695=({const char*_tmpC4="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpC4,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp696,_tmp695,_tag_fat(_tmpC3,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp69A=e1;Cyc_Tcutil_unchecked_cast(_tmp69A,({void*_tmp699=t1_elt;void*_tmp698=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp697=
Cyc_Absyn_empty_tqual(0U);
# 433
Cyc_Absyn_star_type(_tmp699,_tmp698,_tmp697,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 436
return Cyc_Absyn_sint_type;}
# 438
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A;_tmp54A.tag=0U,({struct _fat_ptr _tmp69B=({const char*_tmpC7="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpC7,sizeof(char),50U);});_tmp54A.f1=_tmp69B;});_tmp54A;});void*_tmpC5[1U];_tmpC5[0]=& _tmpC6;({unsigned _tmp69C=e1->loc;Cyc_Warn_err2(_tmp69C,_tag_fat(_tmpC5,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({struct _fat_ptr _tmp69D=({const char*_tmpCA="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpCA,sizeof(char),54U);});_tmp54B.f1=_tmp69D;});_tmp54B;});void*_tmpC8[1U];_tmpC8[0]=& _tmpC9;({unsigned _tmp69E=e1->loc;Cyc_Warn_err2(_tmp69E,_tag_fat(_tmpC8,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp69F=({const char*_tmpD1="expecting ";_tag_fat(_tmpD1,sizeof(char),11U);});_tmp54F.f1=_tmp69F;});_tmp54F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54E;_tmp54E.tag=2U,_tmp54E.f1=(void*)t1;_tmp54E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D;_tmp54D.tag=0U,({struct _fat_ptr _tmp6A0=({const char*_tmpD0=" or int but found ";_tag_fat(_tmpD0,sizeof(char),19U);});_tmp54D.f1=_tmp6A0;});_tmp54D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54C;_tmp54C.tag=2U,_tmp54C.f1=(void*)t2;_tmp54C;});void*_tmpCB[4U];_tmpCB[0]=& _tmpCC,_tmpCB[1]=& _tmpCD,_tmpCB[2]=& _tmpCE,_tmpCB[3]=& _tmpCF;({unsigned _tmp6A1=e2->loc;Cyc_Tcexp_err_and_explain(_tmp6A1,_tag_fat(_tmpCB,sizeof(void*),4U));});});
return t1;}
# 447
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp6A2=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp6A2,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({void*_tmpD2=0U;({unsigned _tmp6A4=e1->loc;struct _fat_ptr _tmp6A3=({const char*_tmpD3="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpD3,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6A4,_tmp6A3,_tag_fat(_tmpD2,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp6A8=e2;Cyc_Tcutil_unchecked_cast(_tmp6A8,({void*_tmp6A7=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp6A6=
Cyc_Tcutil_pointer_region(t2);
# 452
struct Cyc_Absyn_Tqual _tmp6A5=
# 454
Cyc_Absyn_empty_tqual(0U);
# 452
Cyc_Absyn_star_type(_tmp6A7,_tmp6A6,_tmp6A5,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 457
({void*_tmpD4=0U;({unsigned _tmp6AA=e1->loc;struct _fat_ptr _tmp6A9=({const char*_tmpD5="thin pointer subtraction!";_tag_fat(_tmpD5,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp6AA,_tmp6A9,_tag_fat(_tmpD4,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 460
({void*_tmpD6=0U;({unsigned _tmp6AC=e1->loc;struct _fat_ptr _tmp6AB=({const char*_tmpD7="coercing thin pointer to integer to support subtraction";_tag_fat(_tmpD7,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp6AC,_tmp6AB,_tag_fat(_tmpD6,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 463
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD8=0U;({unsigned _tmp6AE=e1->loc;struct _fat_ptr _tmp6AD=({const char*_tmpD9="coercing pointer to integer to support subtraction";_tag_fat(_tmpD9,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp6AE,_tmp6AD,_tag_fat(_tmpD8,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 468
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 471
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 479
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp6AF=t1;Cyc_Unify_unify(_tmp6AF,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 484
if(({struct Cyc_RgnOrder_RgnPO*_tmp6B2=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6B1=loc;void*_tmp6B0=t2;Cyc_Tcutil_silent_castable(_tmp6B2,_tmp6B1,_tmp6B0,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 488
if(({struct Cyc_RgnOrder_RgnPO*_tmp6B5=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6B4=loc;void*_tmp6B3=t1;Cyc_Tcutil_silent_castable(_tmp6B5,_tmp6B4,_tmp6B3,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 492
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 497
{struct _tuple0 _tmpDA=({struct _tuple0 _tmp550;({void*_tmp6B7=Cyc_Tcutil_compress(t1);_tmp550.f1=_tmp6B7;}),({void*_tmp6B6=Cyc_Tcutil_compress(t2);_tmp550.f2=_tmp6B6;});_tmp550;});struct _tuple0 _stmttmpB=_tmpDA;struct _tuple0 _tmpDB=_stmttmpB;void*_tmpDD;void*_tmpDC;switch(*((int*)_tmpDB.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB.f2)->tag == 3U){_LL1: _tmpDC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB.f1)->f1).elt_type;_tmpDD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB.f2)->f1).elt_type;_LL2: {void*t1a=_tmpDC;void*t2a=_tmpDD;
# 499
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDB.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDB.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDB.f2)->f1)->tag == 3U){_LL3: _LL4:
# 503
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 507
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp554;_tmp554.tag=0U,({struct _fat_ptr _tmp6B8=({const char*_tmpE4="comparison not allowed between ";_tag_fat(_tmpE4,sizeof(char),32U);});_tmp554.f1=_tmp6B8;});_tmp554;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp553;_tmp553.tag=2U,_tmp553.f1=(void*)t1;_tmp553;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp6B9=({const char*_tmpE3=" and ";_tag_fat(_tmpE3,sizeof(char),6U);});_tmp552.f1=_tmp6B9;});_tmp552;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp551;_tmp551.tag=2U,_tmp551.f1=(void*)t2;_tmp551;});void*_tmpDE[4U];_tmpDE[0]=& _tmpDF,_tmpDE[1]=& _tmpE0,_tmpDE[2]=& _tmpE1,_tmpDE[3]=& _tmpE2;({unsigned _tmp6BA=loc;Cyc_Tcexp_err_and_explain(_tmp6BA,_tag_fat(_tmpDE,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 513
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 515
enum Cyc_Absyn_Primop _tmpE5=p;switch(_tmpE5){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 519
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 522
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 531
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A:
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 538
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp6BB=({const char*_tmpE8="bad binary primop";_tag_fat(_tmpE8,sizeof(char),18U);});_tmp555.f1=_tmp6BB;});_tmp555;});void*_tmpE6[1U];_tmpE6[0]=& _tmpE7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE6,sizeof(void*),1U));});}_LL0:;}
# 542
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 550
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpE9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpE9;
void*_tmpEA=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpEA;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp557;_tmp557.tag=0U,({struct _fat_ptr _tmp6BC=({const char*_tmpEE="expecting numeric type but found ";_tag_fat(_tmpEE,sizeof(char),34U);});_tmp557.f1=_tmp6BC;});_tmp557;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpED=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp556;_tmp556.tag=2U,_tmp556.f1=(void*)t;_tmp556;});void*_tmpEB[2U];_tmpEB[0]=& _tmpEC,_tmpEB[1]=& _tmpED;({unsigned _tmp6BD=e->loc;Cyc_Warn_err2(_tmp6BD,_tag_fat(_tmpEB,sizeof(void*),2U));});});
return t;}
# 557
{struct Cyc_List_List*_tmpEF=es;struct Cyc_List_List*es2=_tmpEF;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpF0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpF0;int _tmpF1=_stmttmpC;switch(_tmpF1){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0U,({struct _fat_ptr _tmp6BE=({const char*_tmpF4="primitive operator has 0 arguments";_tag_fat(_tmpF4,sizeof(char),35U);});_tmp558.f1=_tmp6BE;});_tmp558;});void*_tmpF2[1U];_tmpF2[0]=& _tmpF3;({struct Cyc_Tcenv_Tenv*_tmp6C1=te;unsigned _tmp6C0=loc;void**_tmp6BF=topt;Cyc_Tcexp_expr_err(_tmp6C1,_tmp6C0,_tmp6BF,_tag_fat(_tmpF2,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2U: _LL5: _LL6:
 return({struct Cyc_Tcenv_Tenv*_tmp6C6=te;unsigned _tmp6C5=loc;void**_tmp6C4=topt;enum Cyc_Absyn_Primop _tmp6C3=p;struct Cyc_Absyn_Exp*_tmp6C2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp6C6,_tmp6C5,_tmp6C4,_tmp6C3,_tmp6C2,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp6C7=({const char*_tmpF7="primitive operator has > 2 arguments";_tag_fat(_tmpF7,sizeof(char),37U);});_tmp559.f1=_tmp6C7;});_tmp559;});void*_tmpF5[1U];_tmpF5[0]=& _tmpF6;({struct Cyc_Tcenv_Tenv*_tmp6CA=te;unsigned _tmp6C9=loc;void**_tmp6C8=topt;Cyc_Tcexp_expr_err(_tmp6CA,_tmp6C9,_tmp6C8,_tag_fat(_tmpF5,sizeof(void*),1U));});});}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 567
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpF8=Cyc_Tcutil_compress(t);void*t=_tmpF8;
void*_tmpF9=t;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Tqual _tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Datatypefield*_tmpFE;struct Cyc_Absyn_Aggrdecl*_tmpFF;switch(*((int*)_tmpF9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpFF=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpFF;
# 571
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp101=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0U,({struct _fat_ptr _tmp6CB=({const char*_tmp103="attempt to write abstract ";_tag_fat(_tmp103,sizeof(char),27U);});_tmp55B.f1=_tmp6CB;});_tmp55B;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp102=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp55A;_tmp55A.tag=6U,_tmp55A.f1=ad;_tmp55A;});void*_tmp100[2U];_tmp100[0]=& _tmp101,_tmp100[1]=& _tmp102;({unsigned _tmp6CC=loc;Cyc_Warn_err2(_tmp6CC,_tag_fat(_tmp100,sizeof(void*),2U));});});
return 0;}
# 575
_tmpFD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpFE=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmpFE;
# 587
{struct Cyc_List_List*_tmp109=df->typs;struct Cyc_List_List*fs=_tmp109;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp10A=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp10A;struct _tuple17*_tmp10B=_stmttmpD;void*_tmp10D;struct Cyc_Absyn_Tqual _tmp10C;_LLE: _tmp10C=_tmp10B->f1;_tmp10D=_tmp10B->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp10C;void*t=_tmp10D;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55E;_tmp55E.tag=0U,({struct _fat_ptr _tmp6CD=({const char*_tmp113="attempt to overwrite a datatype field (";_tag_fat(_tmp113,sizeof(char),40U);});_tmp55E.f1=_tmp6CD;});_tmp55E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp110=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp55D;_tmp55D.tag=1U,_tmp55D.f1=df->name;_tmp55D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp111=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({
struct _fat_ptr _tmp6CE=({const char*_tmp112=") with a const member";_tag_fat(_tmp112,sizeof(char),22U);});_tmp55C.f1=_tmp6CE;});_tmp55C;});void*_tmp10E[3U];_tmp10E[0]=& _tmp10F,_tmp10E[1]=& _tmp110,_tmp10E[2]=& _tmp111;({unsigned _tmp6CF=loc;Cyc_Warn_err2(_tmp6CF,_tag_fat(_tmp10E,sizeof(void*),3U));});});
return 0;}
# 594
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 596
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpFD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF9)->f2;_LL4: {struct Cyc_List_List*fs=_tmpFD;
# 577
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp104=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp104;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp106=({struct Cyc_Warn_String_Warn_Warg_struct _tmp560;_tmp560.tag=0U,({struct _fat_ptr _tmp6D0=({const char*_tmp108="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp108,sizeof(char),53U);});_tmp560.f1=_tmp6D0;});_tmp560;});struct Cyc_Warn_String_Warn_Warg_struct _tmp107=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55F;_tmp55F.tag=0U,_tmp55F.f1=*f->name;_tmp55F;});void*_tmp105[2U];_tmp105[0]=& _tmp106,_tmp105[1]=& _tmp107;({unsigned _tmp6D1=loc;Cyc_Warn_err2(_tmp6D1,_tag_fat(_tmp105,sizeof(void*),2U));});});
return 0;}
# 583
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 585
return 1;}case 4U: _LL7: _tmpFB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF9)->f1).elt_type;_tmpFC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF9)->f1).tq;_LL8: {void*elt_type=_tmpFB;struct Cyc_Absyn_Tqual tq=_tmpFC;
# 598
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp115=({struct Cyc_Warn_String_Warn_Warg_struct _tmp561;_tmp561.tag=0U,({struct _fat_ptr _tmp6D2=({const char*_tmp116="attempt to overwrite a const array";_tag_fat(_tmp116,sizeof(char),35U);});_tmp561.f1=_tmp6D2;});_tmp561;});void*_tmp114[1U];_tmp114[0]=& _tmp115;({unsigned _tmp6D3=loc;Cyc_Warn_err2(_tmp6D3,_tag_fat(_tmp114,sizeof(void*),1U));});});
return 0;}
# 602
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpFA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF9)->f1;_LLA: {struct Cyc_List_List*fs=_tmpFA;
# 604
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp117=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp117;struct _tuple17*_tmp118=_stmttmpE;void*_tmp11A;struct Cyc_Absyn_Tqual _tmp119;_LL11: _tmp119=_tmp118->f1;_tmp11A=_tmp118->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp119;void*t=_tmp11A;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp562;_tmp562.tag=0U,({struct _fat_ptr _tmp6D4=({const char*_tmp11D="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp11D,sizeof(char),55U);});_tmp562.f1=_tmp6D4;});_tmp562;});void*_tmp11B[1U];_tmp11B[0]=& _tmp11C;({unsigned _tmp6D5=loc;Cyc_Warn_err2(_tmp6D5,_tag_fat(_tmp11B,sizeof(void*),1U));});});
return 0;}
# 610
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 612
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 620
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 623
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp11E=e->r;void*_stmttmpF=_tmp11E;void*_tmp11F=_stmttmpF;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct _fat_ptr*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct _fat_ptr*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Vardecl*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12A;struct Cyc_Absyn_Vardecl*_tmp12B;struct Cyc_Absyn_Vardecl*_tmp12C;switch(*((int*)_tmp11F)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11F)->f1)){case 3U: _LL1: _tmp12C=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11F)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp12C;
_tmp12B=vd;goto _LL4;}case 4U: _LL3: _tmp12B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11F)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp12B;
_tmp12A=vd;goto _LL6;}case 5U: _LL5: _tmp12A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11F)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp12A;
_tmp129=vd;goto _LL8;}case 1U: _LL7: _tmp129=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11F)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp129;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp127=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp11F)->f1;_tmp128=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp11F)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp127;struct Cyc_Absyn_Exp*e2=_tmp128;
# 630
{void*_tmp12D=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp12D;void*_tmp12E=_stmttmp10;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_Tqual _tmp130;struct Cyc_Absyn_Tqual _tmp131;switch(*((int*)_tmp12E)){case 3U: _LL18: _tmp131=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp131;
_tmp130=tq;goto _LL1B;}case 4U: _LL1A: _tmp130=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12E)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp130;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp12F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12E)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp12F;
# 634
struct _tuple16 _tmp132=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp132;struct _tuple16 _tmp133=_stmttmp11;int _tmp135;unsigned _tmp134;_LL21: _tmp134=_tmp133.f1;_tmp135=_tmp133.f2;_LL22: {unsigned i=_tmp134;int known=_tmp135;
if(!known){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp137=({struct Cyc_Warn_String_Warn_Warg_struct _tmp563;_tmp563.tag=0U,({struct _fat_ptr _tmp6D6=({const char*_tmp138="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp138,sizeof(char),47U);});_tmp563.f1=_tmp6D6;});_tmp563;});void*_tmp136[1U];_tmp136[0]=& _tmp137;({unsigned _tmp6D7=e->loc;Cyc_Warn_err2(_tmp6D7,_tag_fat(_tmp136,sizeof(void*),1U));});});
return;}
# 639
{struct _handler_cons _tmp139;_push_handler(& _tmp139);{int _tmp13B=0;if(setjmp(_tmp139.handler))_tmp13B=1;if(!_tmp13B){
{struct _tuple17*_tmp13C=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp13C;struct _tuple17*_tmp13D=_stmttmp12;struct Cyc_Absyn_Tqual _tmp13E;_LL24: _tmp13E=_tmp13D->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp13E;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 640
;_pop_handler();}else{void*_tmp13A=(void*)Cyc_Core_get_exn_thrown();void*_tmp13F=_tmp13A;void*_tmp140;if(((struct Cyc_List_Nth_exn_struct*)_tmp13F)->tag == Cyc_List_Nth){_LL27: _LL28:
# 642
 return;}else{_LL29: _tmp140=_tmp13F;_LL2A: {void*exn=_tmp140;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 646
goto _LL0;}case 21U: _LLB: _tmp125=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp11F)->f1;_tmp126=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp11F)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp125;struct _fat_ptr*f=_tmp126;
# 648
{void*_tmp141=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp141;void*_tmp142=_stmttmp13;struct Cyc_List_List*_tmp143;struct Cyc_Absyn_Aggrdecl**_tmp144;switch(*((int*)_tmp142)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp144=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp144;
# 650
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp143=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp142)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp143;
# 655
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 660
goto _LL0;}case 22U: _LLD: _tmp123=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp11F)->f1;_tmp124=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp11F)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp123;struct _fat_ptr*f=_tmp124;
# 662
{void*_tmp145=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp145;void*_tmp146=_stmttmp14;struct Cyc_Absyn_Tqual _tmp148;void*_tmp147;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->tag == 3U){_LL33: _tmp147=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp148=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_tq;_LL34: {void*elt_typ=_tmp147;struct Cyc_Absyn_Tqual tq=_tmp148;
# 664
if(!tq.real_const){
void*_tmp149=Cyc_Tcutil_compress(elt_typ);void*_stmttmp15=_tmp149;void*_tmp14A=_stmttmp15;struct Cyc_List_List*_tmp14B;struct Cyc_Absyn_Aggrdecl**_tmp14C;switch(*((int*)_tmp14A)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14A)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp14C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14A)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp14C;
# 667
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp14B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14A)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp14B;
# 672
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 678
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 681
goto _LL0;}case 20U: _LLF: _tmp122=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp11F)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp122;
# 683
{void*_tmp14D=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp14D;void*_tmp14E=_stmttmp16;struct Cyc_Absyn_Tqual _tmp14F;struct Cyc_Absyn_Tqual _tmp150;switch(*((int*)_tmp14E)){case 3U: _LL3F: _tmp150=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14E)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp150;
_tmp14F=tq;goto _LL42;}case 4U: _LL41: _tmp14F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14E)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp14F;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 688
goto _LL0;}case 12U: _LL11: _tmp121=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp11F)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp121;
_tmp120=e1;goto _LL14;}case 13U: _LL13: _tmp120=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp11F)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp120;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 693
({struct Cyc_Warn_String_Warn_Warg_struct _tmp152=({struct Cyc_Warn_String_Warn_Warg_struct _tmp565;_tmp565.tag=0U,({struct _fat_ptr _tmp6D8=({const char*_tmp154="attempt to write a const location: ";_tag_fat(_tmp154,sizeof(char),36U);});_tmp565.f1=_tmp6D8;});_tmp565;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp153=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp564;_tmp564.tag=4U,_tmp564.f1=e;_tmp564;});void*_tmp151[2U];_tmp151[0]=& _tmp152,_tmp151[1]=& _tmp153;({unsigned _tmp6D9=e->loc;Cyc_Warn_err2(_tmp6D9,_tag_fat(_tmp151,sizeof(void*),2U));});});}
# 696
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 699
({struct Cyc_Tcenv_Tenv*_tmp6DA=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp6DA,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp156=({struct Cyc_Warn_String_Warn_Warg_struct _tmp566;_tmp566.tag=0U,({struct _fat_ptr _tmp6DB=({const char*_tmp157="increment/decrement of non-lvalue";_tag_fat(_tmp157,sizeof(char),34U);});_tmp566.f1=_tmp6DB;});_tmp566;});void*_tmp155[1U];_tmp155[0]=& _tmp156;({struct Cyc_Tcenv_Tenv*_tmp6DE=te;unsigned _tmp6DD=loc;void**_tmp6DC=topt;Cyc_Tcexp_expr_err(_tmp6DE,_tmp6DD,_tmp6DC,_tag_fat(_tmp155,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 705
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp158=0U;({unsigned _tmp6E0=e->loc;struct _fat_ptr _tmp6DF=({const char*_tmp159="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp159,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6E0,_tmp6DF,_tag_fat(_tmp158,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp15A=0U;({unsigned _tmp6E2=e->loc;struct _fat_ptr _tmp6E1=({const char*_tmp15B="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp15B,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp6E2,_tmp6E1,_tag_fat(_tmp15A,sizeof(void*),0U));});});}else{
# 714
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp568;_tmp568.tag=0U,({struct _fat_ptr _tmp6E3=({const char*_tmp15F="expecting arithmetic or ? type but found ";_tag_fat(_tmp15F,sizeof(char),42U);});_tmp568.f1=_tmp6E3;});_tmp568;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp567;_tmp567.tag=2U,_tmp567.f1=(void*)t;_tmp567;});void*_tmp15C[2U];_tmp15C[0]=& _tmp15D,_tmp15C[1]=& _tmp15E;({unsigned _tmp6E4=e->loc;Cyc_Warn_err2(_tmp6E4,_tag_fat(_tmp15C,sizeof(void*),2U));});});}}
# 716
return t;}}
# 720
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 722
({struct Cyc_Tcenv_Tenv*_tmp6E6=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp6E5=e1;Cyc_Tcexp_tcTest(_tmp6E6,_tmp6E5,({const char*_tmp160="conditional expression";_tag_fat(_tmp160,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp161=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tako:& Cyc_Tcutil_tmko;struct Cyc_Core_Opt*ko=_tmp161;
void*_tmp162=({struct Cyc_Core_Opt*_tmp6E7=ko;Cyc_Absyn_new_evar(_tmp6E7,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp162;
struct Cyc_List_List _tmp163=({struct Cyc_List_List _tmp56E;_tmp56E.hd=e3,_tmp56E.tl=0;_tmp56E;});struct Cyc_List_List l1=_tmp163;
struct Cyc_List_List _tmp164=({struct Cyc_List_List _tmp56D;_tmp56D.hd=e2,_tmp56D.tl=& l1;_tmp56D;});struct Cyc_List_List l2=_tmp164;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6E8=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp6E8,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp166=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56C;_tmp56C.tag=0U,({struct _fat_ptr _tmp6E9=({const char*_tmp16B="conditional clause types do not match: ";_tag_fat(_tmp16B,sizeof(char),40U);});_tmp56C.f1=_tmp6E9;});_tmp56C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp167=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp56B;_tmp56B.tag=3U,_tmp56B.f1=(void*)e2->topt;_tmp56B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56A;_tmp56A.tag=0U,({
struct _fat_ptr _tmp6EA=({const char*_tmp16A=" != ";_tag_fat(_tmp16A,sizeof(char),5U);});_tmp56A.f1=_tmp6EA;});_tmp56A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp169=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp569;_tmp569.tag=3U,_tmp569.f1=(void*)e3->topt;_tmp569;});void*_tmp165[4U];_tmp165[0]=& _tmp166,_tmp165[1]=& _tmp167,_tmp165[2]=& _tmp168,_tmp165[3]=& _tmp169;({unsigned _tmp6EB=loc;Cyc_Tcexp_err_and_explain(_tmp6EB,_tag_fat(_tmp165,sizeof(void*),4U));});});
return t;}}
# 736
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 738
({struct Cyc_Tcenv_Tenv*_tmp6ED=te;struct Cyc_Absyn_Exp*_tmp6EC=e1;Cyc_Tcexp_tcTest(_tmp6ED,_tmp6EC,({const char*_tmp16C="logical-and expression";_tag_fat(_tmp16C,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6EF=te;struct Cyc_Absyn_Exp*_tmp6EE=e2;Cyc_Tcexp_tcTest(_tmp6EF,_tmp6EE,({const char*_tmp16D="logical-and expression";_tag_fat(_tmp16D,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 742
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 744
({struct Cyc_Tcenv_Tenv*_tmp6F1=te;struct Cyc_Absyn_Exp*_tmp6F0=e1;Cyc_Tcexp_tcTest(_tmp6F1,_tmp6F0,({const char*_tmp16E="logical-or expression";_tag_fat(_tmp16E,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6F3=te;struct Cyc_Absyn_Exp*_tmp6F2=e2;Cyc_Tcexp_tcTest(_tmp6F3,_tmp6F2,({const char*_tmp16F="logical-or expression";_tag_fat(_tmp16F,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 750
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 757
({struct Cyc_Tcenv_Tenv*_tmp6F4=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp6F4,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 762
{void*_tmp170=Cyc_Tcutil_compress(t1);void*_stmttmp17=_tmp170;void*_tmp171=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp171)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp173=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F;_tmp56F.tag=0U,({struct _fat_ptr _tmp6F5=({const char*_tmp174="cannot assign to an array";_tag_fat(_tmp174,sizeof(char),26U);});_tmp56F.f1=_tmp6F5;});_tmp56F;});void*_tmp172[1U];_tmp172[0]=& _tmp173;({unsigned _tmp6F6=loc;Cyc_Warn_err2(_tmp6F6,_tag_fat(_tmp172,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 767
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp176=({struct Cyc_Warn_String_Warn_Warg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp6F7=({const char*_tmp177="type is abstract (can't determine size)";_tag_fat(_tmp177,sizeof(char),40U);});_tmp570.f1=_tmp6F7;});_tmp570;});void*_tmp175[1U];_tmp175[0]=& _tmp176;({unsigned _tmp6F8=loc;Cyc_Warn_err2(_tmp6F8,_tag_fat(_tmp175,sizeof(void*),1U));});});
# 771
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp179=({struct Cyc_Warn_String_Warn_Warg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp6F9=({const char*_tmp17A="assignment to non-lvalue";_tag_fat(_tmp17A,sizeof(char),25U);});_tmp571.f1=_tmp6F9;});_tmp571;});void*_tmp178[1U];_tmp178[0]=& _tmp179;({struct Cyc_Tcenv_Tenv*_tmp6FC=te;unsigned _tmp6FB=loc;void**_tmp6FA=topt;Cyc_Tcexp_expr_err(_tmp6FC,_tmp6FB,_tmp6FA,_tag_fat(_tmp178,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
Cyc_Tcexp_check_consume(e2->loc,t2,e2);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6FE=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6FD=e2;Cyc_Tcutil_coerce_assign(_tmp6FE,_tmp6FD,t1);})){
void*_tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp575;_tmp575.tag=0U,({struct _fat_ptr _tmp6FF=({const char*_tmp182="type mismatch: ";_tag_fat(_tmp182,sizeof(char),16U);});_tmp575.f1=_tmp6FF;});_tmp575;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp574;_tmp574.tag=2U,_tmp574.f1=(void*)t1;_tmp574;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp700=({const char*_tmp181=" != ";_tag_fat(_tmp181,sizeof(char),5U);});_tmp573.f1=_tmp700;});_tmp573;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp180=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp572;_tmp572.tag=2U,_tmp572.f1=(void*)t2;_tmp572;});void*_tmp17C[4U];_tmp17C[0]=& _tmp17D,_tmp17C[1]=& _tmp17E,_tmp17C[2]=& _tmp17F,_tmp17C[3]=& _tmp180;({struct Cyc_Tcenv_Tenv*_tmp703=te;unsigned _tmp702=loc;void**_tmp701=topt;Cyc_Tcexp_expr_err(_tmp703,_tmp702,_tmp701,_tag_fat(_tmp17C,sizeof(void*),4U));});});void*result=_tmp17B;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 782
return t1;}{
# 784
struct Cyc_Absyn_Exp*_tmp183=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp183;
void*_tmp184=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);void*t_result=_tmp184;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp185=({struct Cyc_Warn_String_Warn_Warg_struct _tmp187=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57A;_tmp57A.tag=0U,({
struct _fat_ptr _tmp704=({const char*_tmp18E="Cannot use this operator in an assignment when ";_tag_fat(_tmp18E,sizeof(char),48U);});_tmp57A.f1=_tmp704;});_tmp57A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp188=({struct Cyc_Warn_String_Warn_Warg_struct _tmp579;_tmp579.tag=0U,({
struct _fat_ptr _tmp705=({const char*_tmp18D="the arguments have types ";_tag_fat(_tmp18D,sizeof(char),26U);});_tmp579.f1=_tmp705;});_tmp579;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp189=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp578;_tmp578.tag=2U,_tmp578.f1=(void*)t1;_tmp578;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0U,({struct _fat_ptr _tmp706=({const char*_tmp18C=" and ";_tag_fat(_tmp18C,sizeof(char),6U);});_tmp577.f1=_tmp706;});_tmp577;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp576;_tmp576.tag=2U,_tmp576.f1=(void*)t2;_tmp576;});void*_tmp186[5U];_tmp186[0]=& _tmp187,_tmp186[1]=& _tmp188,_tmp186[2]=& _tmp189,_tmp186[3]=& _tmp18A,_tmp186[4]=& _tmp18B;({struct Cyc_Tcenv_Tenv*_tmp709=te;unsigned _tmp708=loc;void**_tmp707=topt;Cyc_Tcexp_expr_err(_tmp709,_tmp708,_tmp707,_tag_fat(_tmp186,sizeof(void*),5U));});});
# 789
void*result=_tmp185;
# 792
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 796
return t_result;}}}}
# 800
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp70A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp70A,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp70C=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp70B=topt;Cyc_Tcexp_tcExp(_tmp70C,_tmp70B,e2);});
return(void*)_check_null(e2->topt);}
# 807
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 810
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 813
{void*_tmp18F=Cyc_Tcutil_compress(t1);void*_stmttmp18=_tmp18F;void*_tmp190=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)->f1 == 0){_LL1: _LL2:
# 815
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 818
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 822
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp191=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp191;struct Cyc_Absyn_Datatypefield _tmp192=_stmttmp19;enum Cyc_Absyn_Scope _tmp196;unsigned _tmp195;struct Cyc_List_List*_tmp194;struct _tuple1*_tmp193;_LLA: _tmp193=_tmp192.name;_tmp194=_tmp192.typs;_tmp195=_tmp192.loc;_tmp196=_tmp192.sc;_LLB: {struct _tuple1*n=_tmp193;struct Cyc_List_List*typs=_tmp194;unsigned loc=_tmp195;enum Cyc_Absyn_Scope sc=_tmp196;
# 825
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 828
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 832
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp197=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp197;struct Cyc_Absyn_Datatypefield _tmp198=_stmttmp1A;enum Cyc_Absyn_Scope _tmp19C;unsigned _tmp19B;struct Cyc_List_List*_tmp19A;struct _tuple1*_tmp199;_LLD: _tmp199=_tmp198.name;_tmp19A=_tmp198.typs;_tmp19B=_tmp198.loc;_tmp19C=_tmp198.sc;_LLE: {struct _tuple1*n=_tmp199;struct Cyc_List_List*typs=_tmp19A;unsigned loc=_tmp19B;enum Cyc_Absyn_Scope sc=_tmp19C;
# 835
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 839
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp70E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp70D=e;Cyc_Tcutil_coerce_arg(_tmp70E,_tmp70D,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 843
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp70F=({const char*_tmp1A3="can't find a field in ";_tag_fat(_tmp1A3,sizeof(char),23U);});_tmp57E.f1=_tmp70F;});_tmp57E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57D;_tmp57D.tag=2U,_tmp57D.f1=(void*)tu;_tmp57D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({struct _fat_ptr _tmp710=({const char*_tmp1A2=" to inject value of type ";_tag_fat(_tmp1A2,sizeof(char),26U);});_tmp57C.f1=_tmp710;});_tmp57C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57B;_tmp57B.tag=2U,_tmp57B.f1=(void*)t1;_tmp57B;});void*_tmp19D[4U];_tmp19D[0]=& _tmp19E,_tmp19D[1]=& _tmp19F,_tmp19D[2]=& _tmp1A0,_tmp19D[3]=& _tmp1A1;({unsigned _tmp711=e->loc;Cyc_Warn_err2(_tmp711,_tag_fat(_tmp19D,sizeof(void*),4U));});});
return 0;}
# 848
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 854
struct Cyc_List_List*_tmp1A4=args;struct Cyc_List_List*es=_tmp1A4;
int _tmp1A5=0;int arg_cnt=_tmp1A5;
struct Cyc_Tcenv_Tenv*_tmp1A6=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp1A6;
struct Cyc_Tcenv_Tenv*_tmp1A7=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp1A7;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 863
void*_tmp1A8=t;void*_tmp1AE;void*_tmp1AD;void*_tmp1AC;void*_tmp1AB;struct Cyc_Absyn_Tqual _tmp1AA;void*_tmp1A9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->tag == 3U){_LL1: _tmp1A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).elt_type;_tmp1AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).elt_tq;_tmp1AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).ptr_atts).rgn;_tmp1AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).ptr_atts).nullable;_tmp1AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).ptr_atts).bounds;_tmp1AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A8)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp1A9;struct Cyc_Absyn_Tqual tq=_tmp1AA;void*rgn=_tmp1AB;void*x=_tmp1AC;void*b=_tmp1AD;void*zt=_tmp1AE;
# 868
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 870
Cyc_Tcutil_check_nonzero_bound(loc,b);{
void*_tmp1AF=Cyc_Tcutil_compress(t1);void*_stmttmp1B=_tmp1AF;void*_tmp1B0=_stmttmp1B;struct Cyc_Absyn_Vardecl*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_List_List*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_VarargInfo*_tmp1B7;int _tmp1B6;struct Cyc_List_List*_tmp1B5;void*_tmp1B4;struct Cyc_Absyn_Tqual _tmp1B3;void*_tmp1B2;struct Cyc_List_List*_tmp1B1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->tag == 5U){_LL6: _tmp1B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).tvars;_tmp1B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).effect;_tmp1B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).ret_tqual;_tmp1B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).ret_type;_tmp1B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).args;_tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).c_varargs;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).cyc_varargs;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).rgn_po;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).attributes;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).requires_clause;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).requires_relns;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).ensures_clause;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).ensures_relns;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B0)->f1).return_value;_LL7: {struct Cyc_List_List*tvars=_tmp1B1;void*eff=_tmp1B2;struct Cyc_Absyn_Tqual res_tq=_tmp1B3;void*res_typ=_tmp1B4;struct Cyc_List_List*args_info=_tmp1B5;int c_vararg=_tmp1B6;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1B7;struct Cyc_List_List*rgn_po=_tmp1B8;struct Cyc_List_List*atts=_tmp1B9;struct Cyc_Absyn_Exp*req=_tmp1BA;struct Cyc_List_List*req_relns=_tmp1BB;struct Cyc_Absyn_Exp*ens=_tmp1BC;struct Cyc_List_List*ens_relns=_tmp1BD;struct Cyc_Absyn_Vardecl*ret_var=_tmp1BE;
# 875
if(tvars != 0 || rgn_po != 0)
({void*_tmp1BF=0U;({unsigned _tmp713=e->loc;struct _fat_ptr _tmp712=({const char*_tmp1C0="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1C0,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp713,_tmp712,_tag_fat(_tmp1BF,sizeof(void*),0U));});});
# 879
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 881
while(es != 0 && args_info != 0){
# 883
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp715=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp714=e1;Cyc_Tcutil_coerce_arg(_tmp715,_tmp714,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1D2="actual argument has type ";_tag_fat(_tmp1D2,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1D1=" but formal has type ";_tag_fat(_tmp1D1,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp718=({unsigned long _tmp717=({unsigned long _tmp716=Cyc_strlen((struct _fat_ptr)s0);_tmp716 + Cyc_strlen((struct _fat_ptr)s1);});_tmp717 + Cyc_strlen((struct _fat_ptr)s2);});_tmp718 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp585;_tmp585.tag=0U,_tmp585.f1=s0;_tmp585;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0U,({struct _fat_ptr _tmp719=({const char*_tmp1CB="\n\t";_tag_fat(_tmp1CB,sizeof(char),3U);});_tmp584.f1=_tmp719;});_tmp584;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0U,_tmp583.f1=s1;_tmp583;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp582;_tmp582.tag=0U,({struct _fat_ptr _tmp71A=({const char*_tmp1CA="\n";_tag_fat(_tmp1CA,sizeof(char),2U);});_tmp582.f1=_tmp71A;});_tmp582;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,_tmp581.f1=s2;_tmp581;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp71B=({const char*_tmp1C9="\n\t";_tag_fat(_tmp1C9,sizeof(char),3U);});_tmp580.f1=_tmp71B;});_tmp580;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57F;_tmp57F.tag=0U,_tmp57F.f1=s3;_tmp57F;});void*_tmp1C1[7U];_tmp1C1[0]=& _tmp1C2,_tmp1C1[1]=& _tmp1C3,_tmp1C1[2]=& _tmp1C4,_tmp1C1[3]=& _tmp1C5,_tmp1C1[4]=& _tmp1C6,_tmp1C1[5]=& _tmp1C7,_tmp1C1[6]=& _tmp1C8;({unsigned _tmp71C=e1->loc;Cyc_Warn_err2(_tmp71C,_tag_fat(_tmp1C1,sizeof(void*),7U));});});else{
# 895
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp589;_tmp589.tag=0U,_tmp589.f1=s0;_tmp589;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp588;_tmp588.tag=0U,_tmp588.f1=s1;_tmp588;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp587;_tmp587.tag=0U,_tmp587.f1=s2;_tmp587;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0U,_tmp586.f1=s3;_tmp586;});void*_tmp1CC[4U];_tmp1CC[0]=& _tmp1CD,_tmp1CC[1]=& _tmp1CE,_tmp1CC[2]=& _tmp1CF,_tmp1CC[3]=& _tmp1D0;({unsigned _tmp71D=e1->loc;Cyc_Warn_err2(_tmp71D,_tag_fat(_tmp1CC,sizeof(void*),4U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 900
if(alias_coercion)
({struct Cyc_List_List*_tmp71E=({struct Cyc_List_List*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->hd=(void*)arg_cnt,_tmp1D3->tl=*alias_arg_exps;_tmp1D3;});*alias_arg_exps=_tmp71E;});
Cyc_Tcexp_check_consume(e1->loc,t2,e1);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 910
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1D4=(void*)a->hd;void*_stmttmp1C=_tmp1D4;void*_tmp1D5=_stmttmp1C;int _tmp1D8;int _tmp1D7;enum Cyc_Absyn_Format_Type _tmp1D6;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D5)->tag == 19U){_LLB: _tmp1D6=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D5)->f1;_tmp1D7=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D5)->f2;_tmp1D8=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D5)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1D6;int fmt_arg_pos=_tmp1D7;int arg_start_pos=_tmp1D8;
# 914
{struct _handler_cons _tmp1D9;_push_handler(& _tmp1D9);{int _tmp1DB=0;if(setjmp(_tmp1D9.handler))_tmp1DB=1;if(!_tmp1DB){
# 916
{struct Cyc_Absyn_Exp*_tmp1DC=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1DC;
# 918
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 922
fmt_args=({struct Cyc_Core_Opt*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));({struct Cyc_List_List*_tmp71F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1DD->v=_tmp71F;});_tmp1DD;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1DE=ft;switch(_tmp1DE){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 926
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 929
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 932
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 935
goto _LLF;}_LLF:;}}
# 916
;_pop_handler();}else{void*_tmp1DA=(void*)Cyc_Core_get_exn_thrown();void*_tmp1DF=_tmp1DA;void*_tmp1E0;if(((struct Cyc_List_Nth_exn_struct*)_tmp1DF)->tag == Cyc_List_Nth){_LL17: _LL18:
# 937
({void*_tmp1E1=0U;({unsigned _tmp721=loc;struct _fat_ptr _tmp720=({const char*_tmp1E2="bad format arguments";_tag_fat(_tmp1E2,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp721,_tmp720,_tag_fat(_tmp1E1,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp1E0=_tmp1DF;_LL1A: {void*exn=_tmp1E0;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 942
if(args_info != 0)({void*_tmp1E3=0U;({unsigned _tmp723=loc;struct _fat_ptr _tmp722=({const char*_tmp1E4="too few arguments for function";_tag_fat(_tmp1E4,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp723,_tmp722,_tag_fat(_tmp1E3,sizeof(void*),0U));});});else{
# 944
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({void*_tmp1E5=0U;({unsigned _tmp725=loc;struct _fat_ptr _tmp724=({const char*_tmp1E6="too many arguments for function";_tag_fat(_tmp1E6,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp725,_tmp724,_tag_fat(_tmp1E5,sizeof(void*),0U));});});else{
# 951
struct Cyc_Absyn_VarargInfo _tmp1E7=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1E7;struct Cyc_Absyn_VarargInfo _tmp1E8=_stmttmp1D;int _tmp1EA;void*_tmp1E9;_LL1C: _tmp1E9=_tmp1E8.type;_tmp1EA=_tmp1E8.inject;_LL1D: {void*vt=_tmp1E9;int inject=_tmp1EA;
struct Cyc_Absyn_VarargCallInfo*_tmp1EB=({struct Cyc_Absyn_VarargCallInfo*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->num_varargs=0,_tmp203->injectors=0,_tmp203->vai=cyc_vararg;_tmp203;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1EB;
# 955
*vararg_call_info=vci;
# 957
if(!inject)
# 959
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp727=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp726=e1;Cyc_Tcutil_coerce_arg(_tmp727,_tmp726,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58D;_tmp58D.tag=0U,({struct _fat_ptr _tmp728=({const char*_tmp1F2="vararg requires type ";_tag_fat(_tmp1F2,sizeof(char),22U);});_tmp58D.f1=_tmp728;});_tmp58D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1EE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58C;_tmp58C.tag=2U,_tmp58C.f1=(void*)vt;_tmp58C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58B;_tmp58B.tag=0U,({
struct _fat_ptr _tmp729=({const char*_tmp1F1=" but argument has type ";_tag_fat(_tmp1F1,sizeof(char),24U);});_tmp58B.f1=_tmp729;});_tmp58B;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp58A;_tmp58A.tag=3U,_tmp58A.f1=(void*)e1->topt;_tmp58A;});void*_tmp1EC[4U];_tmp1EC[0]=& _tmp1ED,_tmp1EC[1]=& _tmp1EE,_tmp1EC[2]=& _tmp1EF,_tmp1EC[3]=& _tmp1F0;({unsigned _tmp72A=loc;Cyc_Tcexp_err_and_explain(_tmp72A,_tag_fat(_tmp1EC,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp72B=({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->hd=(void*)arg_cnt,_tmp1F3->tl=*alias_arg_exps;_tmp1F3;});*alias_arg_exps=_tmp72B;});
Cyc_Tcexp_check_consume(e1->loc,vt,e1);}else{
# 974
void*_tmp1F4=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1F4;void*_tmp1F5=_stmttmp1E;struct Cyc_List_List*_tmp1F7;struct Cyc_Absyn_Datatypedecl*_tmp1F6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1F6=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f1)->f1).KnownDatatype).val;_tmp1F7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1F6;struct Cyc_List_List*targs=_tmp1F7;
# 978
struct Cyc_Absyn_Datatypedecl*_tmp1F8=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1F8;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F;_tmp58F.tag=0U,({struct _fat_ptr _tmp72C=({const char*_tmp1FC="can't inject into abstract ";_tag_fat(_tmp1FC,sizeof(char),28U);});_tmp58F.f1=_tmp72C;});_tmp58F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1FB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58E;_tmp58E.tag=2U,_tmp58E.f1=(void*)vt;_tmp58E;});void*_tmp1F9[2U];_tmp1F9[0]=& _tmp1FA,_tmp1F9[1]=& _tmp1FB;({unsigned _tmp72D=loc;Cyc_Warn_err2(_tmp72D,_tag_fat(_tmp1F9,sizeof(void*),2U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 988
({void*_tmp72E=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp72E,Cyc_Tcenv_curr_rgn(te));});{
# 990
struct Cyc_List_List*_tmp1FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp1FD;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 995
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_check_consume(e1->loc,(void*)_check_null(e1->topt),e1);}{
# 999
struct Cyc_Absyn_Datatypefield*_tmp1FE=({struct Cyc_Tcenv_Tenv*_tmp732=te;struct Cyc_Absyn_Exp*_tmp731=e1;void*_tmp730=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp72F=inst;Cyc_Tcexp_tcInjection(_tmp732,_tmp731,_tmp730,_tmp72F,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1FE;
if(f != 0)
({struct Cyc_List_List*_tmp734=({
struct Cyc_List_List*_tmp733=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp733,({struct Cyc_List_List*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->hd=f,_tmp1FF->tl=0;_tmp1FF;}));});
# 1001
vci->injectors=_tmp734;});}}}
# 1004
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp201=({struct Cyc_Warn_String_Warn_Warg_struct _tmp590;_tmp590.tag=0U,({struct _fat_ptr _tmp735=({const char*_tmp202="bad inject vararg type";_tag_fat(_tmp202,sizeof(char),23U);});_tmp590.f1=_tmp735;});_tmp590;});void*_tmp200[1U];_tmp200[0]=& _tmp201;({unsigned _tmp736=loc;Cyc_Warn_err2(_tmp736,_tag_fat(_tmp200,sizeof(void*),1U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1010
if(*alias_arg_exps == 0)
# 1019 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1021
return res_typ;}}}else{_LL8: _LL9:
# 1023
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp205=({struct Cyc_Warn_String_Warn_Warg_struct _tmp592;_tmp592.tag=0U,({struct _fat_ptr _tmp737=({const char*_tmp207="expected pointer to function but found ";_tag_fat(_tmp207,sizeof(char),40U);});_tmp592.f1=_tmp737;});_tmp592;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp206=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp591;_tmp591.tag=2U,_tmp591.f1=(void*)t;_tmp591;});void*_tmp204[2U];_tmp204[0]=& _tmp205,_tmp204[1]=& _tmp206;({struct Cyc_Tcenv_Tenv*_tmp73A=te;unsigned _tmp739=loc;void**_tmp738=topt;Cyc_Tcexp_expr_err(_tmp73A,_tmp739,_tmp738,_tag_fat(_tmp204,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1026
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp209=({struct Cyc_Warn_String_Warn_Warg_struct _tmp594;_tmp594.tag=0U,({struct _fat_ptr _tmp73B=({const char*_tmp20B="expected pointer to function but found ";_tag_fat(_tmp20B,sizeof(char),40U);});_tmp594.f1=_tmp73B;});_tmp594;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp593;_tmp593.tag=2U,_tmp593.f1=(void*)t;_tmp593;});void*_tmp208[2U];_tmp208[0]=& _tmp209,_tmp208[1]=& _tmp20A;({struct Cyc_Tcenv_Tenv*_tmp73E=te;unsigned _tmp73D=loc;void**_tmp73C=topt;Cyc_Tcexp_expr_err(_tmp73E,_tmp73D,_tmp73C,_tag_fat(_tmp208,sizeof(void*),2U));});});}_LL0:;}}}
# 1030
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp20C=Cyc_Absyn_exn_type();void*exception_type=_tmp20C;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp740=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp73F=e;Cyc_Tcutil_coerce_arg(_tmp740,_tmp73F,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp598;_tmp598.tag=0U,({struct _fat_ptr _tmp741=({const char*_tmp213="expected ";_tag_fat(_tmp213,sizeof(char),10U);});_tmp598.f1=_tmp741;});_tmp598;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp597;_tmp597.tag=2U,_tmp597.f1=(void*)exception_type;_tmp597;});struct Cyc_Warn_String_Warn_Warg_struct _tmp210=({struct Cyc_Warn_String_Warn_Warg_struct _tmp596;_tmp596.tag=0U,({struct _fat_ptr _tmp742=({const char*_tmp212=" but found ";_tag_fat(_tmp212,sizeof(char),12U);});_tmp596.f1=_tmp742;});_tmp596;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp211=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp595;_tmp595.tag=3U,_tmp595.f1=(void*)e->topt;_tmp595;});void*_tmp20D[4U];_tmp20D[0]=& _tmp20E,_tmp20D[1]=& _tmp20F,_tmp20D[2]=& _tmp210,_tmp20D[3]=& _tmp211;({unsigned _tmp743=loc;Cyc_Warn_err2(_tmp743,_tag_fat(_tmp20D,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1041
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1043
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp214=t1;struct Cyc_Absyn_PtrAtts _tmp217;struct Cyc_Absyn_Tqual _tmp216;void*_tmp215;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->tag == 3U){_LL1: _tmp215=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).elt_type;_tmp216=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).elt_tq;_tmp217=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).ptr_atts;_LL2: {void*t=_tmp215;struct Cyc_Absyn_Tqual tq=_tmp216;struct Cyc_Absyn_PtrAtts atts=_tmp217;
# 1046
{void*_tmp218=Cyc_Tcutil_compress(t);void*_stmttmp1F=_tmp218;void*_tmp219=_stmttmp1F;struct Cyc_Absyn_FnInfo _tmp21A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp219)->tag == 5U){_LL6: _tmp21A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp219)->f1;_LL7: {struct Cyc_Absyn_FnInfo info=_tmp21A;
# 1048
struct Cyc_List_List*_tmp21B=info.tvars;struct Cyc_List_List*tvars=_tmp21B;
struct Cyc_List_List*instantiation=0;
# 1051
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp747=loc;struct Cyc_Tcenv_Tenv*_tmp746=te;struct Cyc_List_List*_tmp745=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp744=k;Cyc_Tctyp_check_type(_tmp747,_tmp746,_tmp745,_tmp744,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp21D=_cycalloc(sizeof(*_tmp21D));({struct _tuple14*_tmp748=({struct _tuple14*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp21C->f2=(void*)ts->hd;_tmp21C;});_tmp21D->hd=_tmp748;}),_tmp21D->tl=instantiation;_tmp21D;});}
# 1057
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp21F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp599;_tmp599.tag=0U,({struct _fat_ptr _tmp749=({const char*_tmp220="too many type variables in instantiation";_tag_fat(_tmp220,sizeof(char),41U);});_tmp599.f1=_tmp749;});_tmp599;});void*_tmp21E[1U];_tmp21E[0]=& _tmp21F;({struct Cyc_Tcenv_Tenv*_tmp74C=te;unsigned _tmp74B=loc;void**_tmp74A=topt;Cyc_Tcexp_expr_err(_tmp74C,_tmp74B,_tmp74A,_tag_fat(_tmp21E,sizeof(void*),1U));});});
# 1063
if(tvars == 0){
({struct Cyc_List_List*_tmp74D=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp74D;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1068
void*new_fn_typ=({struct Cyc_List_List*_tmp74E=instantiation;Cyc_Tcutil_substitute(_tmp74E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->tag=5U,_tmp221->f1=info;_tmp221;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp59A;_tmp59A.elt_type=new_fn_typ,_tmp59A.elt_tq=tq,_tmp59A.ptr_atts=atts;_tmp59A;}));}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1072
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1075
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp223=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59C;_tmp59C.tag=0U,({struct _fat_ptr _tmp74F=({const char*_tmp225="expecting polymorphic type but found ";_tag_fat(_tmp225,sizeof(char),38U);});_tmp59C.f1=_tmp74F;});_tmp59C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp224=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59B;_tmp59B.tag=2U,_tmp59B.f1=(void*)t1;_tmp59B;});void*_tmp222[2U];_tmp222[0]=& _tmp223,_tmp222[1]=& _tmp224;({struct Cyc_Tcenv_Tenv*_tmp752=te;unsigned _tmp751=loc;void**_tmp750=topt;Cyc_Tcexp_expr_err(_tmp752,_tmp751,_tmp750,_tag_fat(_tmp222,sizeof(void*),2U));});});}
# 1079
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1081
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1083
({void*_tmp753=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp753;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1088
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1090
({unsigned _tmp757=loc;struct Cyc_Tcenv_Tenv*_tmp756=te;struct Cyc_List_List*_tmp755=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp754=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1090
Cyc_Tctyp_check_type(_tmp757,_tmp756,_tmp755,_tmp754,1,0,t);});
# 1092
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp75A=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp759=loc;void*_tmp758=t2;Cyc_Tcutil_silent_castable(_tmp75A,_tmp759,_tmp758,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1098
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp75D=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp75C=loc;void*_tmp75B=t2;Cyc_Tcutil_castable(_tmp75D,_tmp75C,_tmp75B,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1105
Cyc_Unify_unify(t2,t);{
void*_tmp226=({struct Cyc_Warn_String_Warn_Warg_struct _tmp228=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0U,({struct _fat_ptr _tmp75E=({const char*_tmp22D="cannot cast ";_tag_fat(_tmp22D,sizeof(char),13U);});_tmp5A0.f1=_tmp75E;});_tmp5A0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp229=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59F;_tmp59F.tag=2U,_tmp59F.f1=(void*)t2;_tmp59F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59E;_tmp59E.tag=0U,({struct _fat_ptr _tmp75F=({const char*_tmp22C=" to ";_tag_fat(_tmp22C,sizeof(char),5U);});_tmp59E.f1=_tmp75F;});_tmp59E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp22B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59D;_tmp59D.tag=2U,_tmp59D.f1=(void*)t;_tmp59D;});void*_tmp227[4U];_tmp227[0]=& _tmp228,_tmp227[1]=& _tmp229,_tmp227[2]=& _tmp22A,_tmp227[3]=& _tmp22B;({struct Cyc_Tcenv_Tenv*_tmp761=te;unsigned _tmp760=loc;Cyc_Tcexp_expr_err(_tmp761,_tmp760,& t,_tag_fat(_tmp227,sizeof(void*),4U));});});void*result=_tmp226;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1114
struct _tuple0 _tmp22E=({struct _tuple0 _tmp5A1;_tmp5A1.f1=e->r,({void*_tmp762=Cyc_Tcutil_compress(t);_tmp5A1.f2=_tmp762;});_tmp5A1;});struct _tuple0 _stmttmp20=_tmp22E;struct _tuple0 _tmp22F=_stmttmp20;void*_tmp233;void*_tmp232;void*_tmp231;int _tmp230;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp22F.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F.f2)->tag == 3U){_LL1: _tmp230=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp22F.f1)->f1).fat_result;_tmp231=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F.f2)->f1).ptr_atts).nullable;_tmp232=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F.f2)->f1).ptr_atts).bounds;_tmp233=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22F.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp230;void*nbl=_tmp231;void*bds=_tmp232;void*zt=_tmp233;
# 1118
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp234=({void*_tmp763=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp763,bds);});struct Cyc_Absyn_Exp*eopt=_tmp234;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == (unsigned)1)
({void*_tmp235=0U;({unsigned _tmp765=loc;struct _fat_ptr _tmp764=({const char*_tmp236="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp236,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp765,_tmp764,_tag_fat(_tmp235,sizeof(void*),0U));});});}}
# 1124
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;}_LL0:;}}}
# 1130
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp237=0;void**topt2=_tmp237;
struct Cyc_Absyn_Tqual _tmp238=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp238;
int _tmp239=0;int nullable=_tmp239;
if(topt != 0){
void*_tmp23A=Cyc_Tcutil_compress(*topt);void*_stmttmp21=_tmp23A;void*_tmp23B=_stmttmp21;void*_tmp23E;struct Cyc_Absyn_Tqual _tmp23D;void**_tmp23C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B)->tag == 3U){_LL1: _tmp23C=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B)->f1).elt_type;_tmp23D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B)->f1).elt_tq;_tmp23E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp23C;struct Cyc_Absyn_Tqual tq=_tmp23D;void*n=_tmp23E;
# 1137
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1151
({struct Cyc_Tcenv_Tenv*_tmp767=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp766=topt2;Cyc_Tcexp_tcExpNoInst(_tmp767,_tmp766,e);});
# 1153
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp240=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=0U,({struct _fat_ptr _tmp768=({const char*_tmp241="cannot take the address of an alias-free path";_tag_fat(_tmp241,sizeof(char),46U);});_tmp5A2.f1=_tmp768;});_tmp5A2;});void*_tmp23F[1U];_tmp23F[0]=& _tmp240;({unsigned _tmp769=e->loc;Cyc_Warn_err2(_tmp769,_tag_fat(_tmp23F,sizeof(void*),1U));});});
# 1158
{void*_tmp242=e->r;void*_stmttmp22=_tmp242;void*_tmp243=_stmttmp22;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp244;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp243)->tag == 23U){_LL6: _tmp244=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_tmp245=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp243)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp244;struct Cyc_Absyn_Exp*e2=_tmp245;
# 1160
{void*_tmp246=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp246;void*_tmp247=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp247)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1165
({void*_tmp76A=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp76A;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1168
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1173
{void*_tmp248=e->r;void*_stmttmp24=_tmp248;void*_tmp249=_stmttmp24;switch(*((int*)_tmp249)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp249)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp249)->f3 == 1){_LL12: _LL13:
# 1176
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=0U,({struct _fat_ptr _tmp76B=({const char*_tmp24C="cannot take the address of a @tagged union member";_tag_fat(_tmp24C,sizeof(char),50U);});_tmp5A3.f1=_tmp76B;});_tmp5A3;});void*_tmp24A[1U];_tmp24A[0]=& _tmp24B;({unsigned _tmp76C=e->loc;Cyc_Warn_err2(_tmp76C,_tag_fat(_tmp24A,sizeof(void*),1U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1182
struct _tuple15 _tmp24D=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp25=_tmp24D;struct _tuple15 _tmp24E=_stmttmp25;void*_tmp250;int _tmp24F;_LL17: _tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;_LL18: {int is_const=_tmp24F;void*rgn=_tmp250;
# 1184
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp252=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,({struct _fat_ptr _tmp76D=({const char*_tmp253="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp253,sizeof(char),60U);});_tmp5A4.f1=_tmp76D;});_tmp5A4;});void*_tmp251[1U];_tmp251[0]=& _tmp252;({unsigned _tmp76E=e->loc;Cyc_Warn_err2(_tmp76E,_tag_fat(_tmp251,sizeof(void*),1U));});});{
# 1187
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp76F=tq.real_const=1;tq.print_const=_tmp76F;});
# 1191
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);}}}}
# 1195
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1198
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp772=loc;struct Cyc_Tcenv_Tenv*_tmp771=te;struct Cyc_List_List*_tmp770=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp772,_tmp771,_tmp770,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp255=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,({struct _fat_ptr _tmp773=({const char*_tmp259="sizeof applied to type ";_tag_fat(_tmp259,sizeof(char),24U);});_tmp5A7.f1=_tmp773;});_tmp5A7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp256=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=2U,_tmp5A6.f1=(void*)t;_tmp5A6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp257=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=0U,({struct _fat_ptr _tmp774=({const char*_tmp258=" which has unknown size here";_tag_fat(_tmp258,sizeof(char),29U);});_tmp5A5.f1=_tmp774;});_tmp5A5;});void*_tmp254[3U];_tmp254[0]=& _tmp255,_tmp254[1]=& _tmp256,_tmp254[2]=& _tmp257;({unsigned _tmp775=loc;Cyc_Warn_err2(_tmp775,_tag_fat(_tmp254,sizeof(void*),3U));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp25A=Cyc_Tcutil_compress(*topt);void*_stmttmp26=_tmp25A;void*_tmp25B=_stmttmp26;void*_tmp25C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f2 != 0){_LL1: _tmp25C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B)->f2)->hd;_LL2: {void*tt=_tmp25C;
# 1208
if(({void*_tmp777=tt;Cyc_Unify_unify(_tmp777,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->tag=9U,({struct Cyc_Absyn_Exp*_tmp776=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp25D->f1=_tmp776;});_tmp25D;}));}))
return Cyc_Tcutil_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_uint_type;}_LL0:;}}
# 1215
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1223
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1225
({unsigned _tmp77A=loc;struct Cyc_Tcenv_Tenv*_tmp779=te;struct Cyc_List_List*_tmp778=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp77A,_tmp779,_tmp778,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp25E=fs;struct Cyc_List_List*l=_tmp25E;
void*_tmp25F=t_orig;void*t=_tmp25F;
for(0;l != 0;l=l->tl){
void*_tmp260=(void*)l->hd;void*n=_tmp260;
void*_tmp261=n;unsigned _tmp262;struct _fat_ptr*_tmp263;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp261)->tag == 0U){_LL1: _tmp263=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp261)->f1;_LL2: {struct _fat_ptr*n=_tmp263;
# 1233
int bad_type=1;
{void*_tmp264=Cyc_Tcutil_compress(t);void*_stmttmp27=_tmp264;void*_tmp265=_stmttmp27;struct Cyc_List_List*_tmp266;struct Cyc_Absyn_Aggrdecl**_tmp267;switch(*((int*)_tmp265)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp265)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp265)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp267=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp265)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp267;
# 1236
if((*adp)->impl == 0)
goto _LL5;
_tmp266=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp266=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp265)->f2;_LL9: {struct Cyc_List_List*fields=_tmp266;
# 1240
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp269=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,({struct _fat_ptr _tmp77B=({const char*_tmp26B="no field of struct/union has name %s";_tag_fat(_tmp26B,sizeof(char),37U);});_tmp5A9.f1=_tmp77B;});_tmp5A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0U,_tmp5A8.f1=*n;_tmp5A8;});void*_tmp268[2U];_tmp268[0]=& _tmp269,_tmp268[1]=& _tmp26A;({unsigned _tmp77C=loc;Cyc_Warn_err2(_tmp77C,_tag_fat(_tmp268,sizeof(void*),2U));});});else{
# 1244
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1249
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp26D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=2U,_tmp5AB.f1=(void*)t;_tmp5AB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=0U,({struct _fat_ptr _tmp77D=({const char*_tmp26F=" is not a known struct/union type";_tag_fat(_tmp26F,sizeof(char),34U);});_tmp5AA.f1=_tmp77D;});_tmp5AA;});void*_tmp26C[2U];_tmp26C[0]=& _tmp26D,_tmp26C[1]=& _tmp26E;({unsigned _tmp77E=loc;Cyc_Warn_err2(_tmp77E,_tag_fat(_tmp26C,sizeof(void*),2U));});});else{
# 1253
struct _fat_ptr _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp5B4;_tmp5B4.tag=0U,({struct _fat_ptr _tmp77F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp5B4.f1=_tmp77F;});_tmp5B4;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({struct _fat_ptr _tmp780=({const char*_tmp285="(%s)";_tag_fat(_tmp285,sizeof(char),5U);});Cyc_aprintf(_tmp780,_tag_fat(_tmp284,sizeof(void*),1U));});});struct _fat_ptr s=_tmp270;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp271=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp271;void*_tmp272=_stmttmp28;unsigned _tmp273;struct _fat_ptr*_tmp274;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp272)->tag == 0U){_LLD: _tmp274=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp272)->f1;_LLE: {struct _fat_ptr*n=_tmp274;
s=({struct Cyc_String_pa_PrintArg_struct _tmp277=({struct Cyc_String_pa_PrintArg_struct _tmp5AD;_tmp5AD.tag=0U,_tmp5AD.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5AD;});struct Cyc_String_pa_PrintArg_struct _tmp278=({struct Cyc_String_pa_PrintArg_struct _tmp5AC;_tmp5AC.tag=0U,_tmp5AC.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5AC;});void*_tmp275[2U];_tmp275[0]=& _tmp277,_tmp275[1]=& _tmp278;({struct _fat_ptr _tmp781=({const char*_tmp276="%s.%s";_tag_fat(_tmp276,sizeof(char),6U);});Cyc_aprintf(_tmp781,_tag_fat(_tmp275,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp273=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp272)->f1;_LL10: {unsigned n=_tmp273;
s=({struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp5AF;_tmp5AF.tag=0U,_tmp5AF.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5AF;});struct Cyc_Int_pa_PrintArg_struct _tmp27C=({struct Cyc_Int_pa_PrintArg_struct _tmp5AE;_tmp5AE.tag=1U,_tmp5AE.f1=(unsigned long)((int)n);_tmp5AE;});void*_tmp279[2U];_tmp279[0]=& _tmp27B,_tmp279[1]=& _tmp27C;({struct _fat_ptr _tmp782=({const char*_tmp27A="%s.%d";_tag_fat(_tmp27A,sizeof(char),6U);});Cyc_aprintf(_tmp782,_tag_fat(_tmp279,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}}
# 1259
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=0U,_tmp5B3.f1=(struct _fat_ptr)s;_tmp5B3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=0U,({struct _fat_ptr _tmp783=({const char*_tmp283=" == ";_tag_fat(_tmp283,sizeof(char),5U);});_tmp5B2.f1=_tmp783;});_tmp5B2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp280=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=2U,_tmp5B1.f1=(void*)t;_tmp5B1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp281=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,({struct _fat_ptr _tmp784=({const char*_tmp282=" is not a struct/union type";_tag_fat(_tmp282,sizeof(char),28U);});_tmp5B0.f1=_tmp784;});_tmp5B0;});void*_tmp27D[4U];_tmp27D[0]=& _tmp27E,_tmp27D[1]=& _tmp27F,_tmp27D[2]=& _tmp280,_tmp27D[3]=& _tmp281;({unsigned _tmp785=loc;Cyc_Warn_err2(_tmp785,_tag_fat(_tmp27D,sizeof(void*),4U));});});}}
# 1262
goto _LL0;}}else{_LL3: _tmp262=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp261)->f1;_LL4: {unsigned n=_tmp262;
# 1264
int bad_type=1;
{void*_tmp287=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp287;void*_tmp288=_stmttmp29;struct Cyc_List_List*_tmp289;struct Cyc_List_List*_tmp28A;struct Cyc_Absyn_Datatypefield*_tmp28B;struct Cyc_Absyn_Aggrdecl**_tmp28C;switch(*((int*)_tmp288)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp288)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp288)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp28C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp288)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp28C;
# 1267
if((*adp)->impl == 0)
goto _LL11;
_tmp28A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp288)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp28B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp288)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp28B;
# 1287
if(({unsigned _tmp786=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp786 < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp29A=({struct Cyc_Int_pa_PrintArg_struct _tmp5B6;_tmp5B6.tag=1U,({
unsigned long _tmp787=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp5B6.f1=_tmp787;});_tmp5B6;});struct Cyc_Int_pa_PrintArg_struct _tmp29B=({struct Cyc_Int_pa_PrintArg_struct _tmp5B5;_tmp5B5.tag=1U,_tmp5B5.f1=(unsigned long)((int)n);_tmp5B5;});void*_tmp298[2U];_tmp298[0]=& _tmp29A,_tmp298[1]=& _tmp29B;({unsigned _tmp789=loc;struct _fat_ptr _tmp788=({const char*_tmp299="datatype field has too few components: %d < %d";_tag_fat(_tmp299,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp789,_tmp788,_tag_fat(_tmp298,sizeof(void*),2U));});});else{
# 1291
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp29C=0U;({unsigned _tmp78B=loc;struct _fat_ptr _tmp78A=({const char*_tmp29D="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp29D,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp78B,_tmp78A,_tag_fat(_tmp29C,sizeof(void*),0U));});});}}
# 1296
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp28A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp288)->f2;_LL15: {struct Cyc_List_List*fields=_tmp28A;
# 1271
if(({unsigned _tmp78C=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp78C <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=0U,({struct _fat_ptr _tmp78D=({const char*_tmp293="struct/union has too few components: ";_tag_fat(_tmp293,sizeof(char),38U);});_tmp5BA.f1=_tmp78D;});_tmp5BA;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp28F=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=12U,({
int _tmp78E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp5B9.f1=_tmp78E;});_tmp5B9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp290=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=0U,({struct _fat_ptr _tmp78F=({const char*_tmp292=" <= ";_tag_fat(_tmp292,sizeof(char),5U);});_tmp5B8.f1=_tmp78F;});_tmp5B8;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp291=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=12U,_tmp5B7.f1=(int)n;_tmp5B7;});void*_tmp28D[4U];_tmp28D[0]=& _tmp28E,_tmp28D[1]=& _tmp28F,_tmp28D[2]=& _tmp290,_tmp28D[3]=& _tmp291;({unsigned _tmp790=loc;Cyc_Warn_err2(_tmp790,_tag_fat(_tmp28D,sizeof(void*),4U));});});else{
# 1275
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp289=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp288)->f1;_LL17: {struct Cyc_List_List*l=_tmp289;
# 1279
if(({unsigned _tmp791=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp791 <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp296=({struct Cyc_Int_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=1U,({
unsigned long _tmp792=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp5BC.f1=_tmp792;});_tmp5BC;});struct Cyc_Int_pa_PrintArg_struct _tmp297=({struct Cyc_Int_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=1U,_tmp5BB.f1=(unsigned long)((int)n);_tmp5BB;});void*_tmp294[2U];_tmp294[0]=& _tmp296,_tmp294[1]=& _tmp297;({unsigned _tmp794=loc;struct _fat_ptr _tmp793=({const char*_tmp295="tuple has too few components: %d <= %d";_tag_fat(_tmp295,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp794,_tmp793,_tag_fat(_tmp294,sizeof(void*),2U));});});else{
# 1283
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1298
 goto _LL11;}_LL11:;}
# 1300
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp29F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BE;_tmp5BE.tag=2U,_tmp5BE.f1=(void*)t;_tmp5BE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BD;_tmp5BD.tag=0U,({struct _fat_ptr _tmp795=({const char*_tmp2A1=" is not a known type";_tag_fat(_tmp2A1,sizeof(char),21U);});_tmp5BD.f1=_tmp795;});_tmp5BD;});void*_tmp29E[2U];_tmp29E[0]=& _tmp29F,_tmp29E[1]=& _tmp2A0;({unsigned _tmp796=loc;Cyc_Warn_err2(_tmp796,_tag_fat(_tmp29E,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 1305
return Cyc_Absyn_uint_type;}}
# 1309
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp2A2=t;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A2)->tag == 1U){_LL1: _LL2: {
# 1315
struct Cyc_Absyn_PtrInfo _tmp2A3=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp2A3;
({void*_tmp798=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));({struct Cyc_List_List*_tmp797=Cyc_Tcenv_lookup_type_vars(te);_tmp2A4->v=_tmp797;});_tmp2A4;}));pi.elt_type=_tmp798;});{
void*_tmp2A5=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp2A5;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Tcutil_compress(t);
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1323
void*_tmp2A6=t;void*_tmp2AA;void*_tmp2A9;void*_tmp2A8;void*_tmp2A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A6)->tag == 3U){_LL6: _tmp2A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A6)->f1).elt_type;_tmp2A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A6)->f1).ptr_atts).rgn;_tmp2A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A6)->f1).ptr_atts).bounds;_tmp2AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A6)->f1).ptr_atts).zero_term;_LL7: {void*t2=_tmp2A7;void*rt=_tmp2A8;void*b=_tmp2A9;void*zt=_tmp2AA;
# 1325
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2AB=Cyc_Tcutil_compress(t2);void*_stmttmp2A=_tmp2AB;void*_tmp2AC=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AC)->tag == 5U){_LLB: _LLC:
# 1330
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BF;_tmp5BF.tag=0U,({struct _fat_ptr _tmp799=({const char*_tmp2AF="unnecessary dereference for function type";_tag_fat(_tmp2AF,sizeof(char),42U);});_tmp5BF.f1=_tmp799;});_tmp5BF;});void*_tmp2AD[1U];_tmp2AD[0]=& _tmp2AE;({unsigned _tmp79A=loc;Cyc_Warn_warn2(_tmp79A,_tag_fat(_tmp2AD,sizeof(void*),1U));});});
return t;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C0;_tmp5C0.tag=0U,({struct _fat_ptr _tmp79B=({const char*_tmp2B2="cannot dereference abstract pointer type";_tag_fat(_tmp2B2,sizeof(char),41U);});_tmp5C0.f1=_tmp79B;});_tmp5C0;});void*_tmp2B0[1U];_tmp2B0[0]=& _tmp2B1;({unsigned _tmp79C=loc;Cyc_Warn_err2(_tmp79C,_tag_fat(_tmp2B0,sizeof(void*),1U));});});}_LLA:;}
# 1335
return t2;}}else{_LL8: _LL9:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=0U,({struct _fat_ptr _tmp79D=({const char*_tmp2B6="expecting pointer type but found ";_tag_fat(_tmp2B6,sizeof(char),34U);});_tmp5C2.f1=_tmp79D;});_tmp5C2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2B5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=2U,_tmp5C1.f1=(void*)t;_tmp5C1;});void*_tmp2B3[2U];_tmp2B3[0]=& _tmp2B4,_tmp2B3[1]=& _tmp2B5;({struct Cyc_Tcenv_Tenv*_tmp7A0=te;unsigned _tmp79F=loc;void**_tmp79E=topt;Cyc_Tcexp_expr_err(_tmp7A0,_tmp79F,_tmp79E,_tag_fat(_tmp2B3,sizeof(void*),2U));});});}_LL5:;}}}
# 1340
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1344
({int _tmp7A1=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7A1;});{
void*_tmp2B7=Cyc_Tcutil_compress(aggr_type);void*_stmttmp2B=_tmp2B7;void*_tmp2B8=_stmttmp2B;struct Cyc_List_List*_tmp2BA;enum Cyc_Absyn_AggrKind _tmp2B9;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Aggrdecl*_tmp2BB;switch(*((int*)_tmp2B8)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2BB=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f1)->f1).KnownAggr).val;_tmp2BC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2BB;struct Cyc_List_List*ts=_tmp2BC;
# 1347
struct Cyc_Absyn_Aggrfield*_tmp2BD=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2BD;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2BF=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5C6;_tmp5C6.tag=6U,_tmp5C6.f1=ad;_tmp5C6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C5;_tmp5C5.tag=0U,({struct _fat_ptr _tmp7A2=({const char*_tmp2C4=" has no ";_tag_fat(_tmp2C4,sizeof(char),9U);});_tmp5C5.f1=_tmp7A2;});_tmp5C5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=0U,_tmp5C4.f1=*f;_tmp5C4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=0U,({struct _fat_ptr _tmp7A3=({const char*_tmp2C3=" member";_tag_fat(_tmp2C3,sizeof(char),8U);});_tmp5C3.f1=_tmp7A3;});_tmp5C3;});void*_tmp2BE[4U];_tmp2BE[0]=& _tmp2BF,_tmp2BE[1]=& _tmp2C0,_tmp2BE[2]=& _tmp2C1,_tmp2BE[3]=& _tmp2C2;({struct Cyc_Tcenv_Tenv*_tmp7A6=te;unsigned _tmp7A5=loc;void**_tmp7A4=topt;Cyc_Tcexp_expr_err(_tmp7A6,_tmp7A5,_tmp7A4,_tag_fat(_tmp2BE,sizeof(void*),4U));});});
# 1351
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp7A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp7A7,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2C5=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2C5;
# 1359
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2C6=Cyc_Tcutil_compress(t2);void*_stmttmp2C=_tmp2C6;void*_tmp2C7=_stmttmp2C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7)->tag == 4U){_LL8: _LL9:
 goto _LL7;}else{_LLA: _LLB:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=0U,({struct _fat_ptr _tmp7A8=({const char*_tmp2CD="cannot get member ";_tag_fat(_tmp2CD,sizeof(char),19U);});_tmp5C9.f1=_tmp7A8;});_tmp5C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=0U,_tmp5C8.f1=*f;_tmp5C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=0U,({
struct _fat_ptr _tmp7A9=({const char*_tmp2CC=" since its type is abstract";_tag_fat(_tmp2CC,sizeof(char),28U);});_tmp5C7.f1=_tmp7A9;});_tmp5C7;});void*_tmp2C8[3U];_tmp2C8[0]=& _tmp2C9,_tmp2C8[1]=& _tmp2CA,_tmp2C8[2]=& _tmp2CB;({struct Cyc_Tcenv_Tenv*_tmp7AC=te;unsigned _tmp7AB=loc;void**_tmp7AA=topt;Cyc_Tcexp_expr_err(_tmp7AC,_tmp7AB,_tmp7AA,_tag_fat(_tmp2C8,sizeof(void*),3U));});});}_LL7:;}
# 1367
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=0U,({struct _fat_ptr _tmp7AD=({const char*_tmp2D3="cannot read union member ";_tag_fat(_tmp2D3,sizeof(char),26U);});_tmp5CC.f1=_tmp7AD;});_tmp5CC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=0U,_tmp5CB.f1=*f;_tmp5CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0U,({
struct _fat_ptr _tmp7AE=({const char*_tmp2D2=" since it is not `bits-only'";_tag_fat(_tmp2D2,sizeof(char),29U);});_tmp5CA.f1=_tmp7AE;});_tmp5CA;});void*_tmp2CE[3U];_tmp2CE[0]=& _tmp2CF,_tmp2CE[1]=& _tmp2D0,_tmp2CE[2]=& _tmp2D1;({struct Cyc_Tcenv_Tenv*_tmp7B1=te;unsigned _tmp7B0=loc;void**_tmp7AF=topt;Cyc_Tcexp_expr_err(_tmp7B1,_tmp7B0,_tmp7AF,_tag_fat(_tmp2CE,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1373
if(!({void*_tmp7B2=t2;Cyc_Unify_unify(_tmp7B2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CF;_tmp5CF.tag=0U,({struct _fat_ptr _tmp7B3=({const char*_tmp2D9="must use pattern-matching to access field ";_tag_fat(_tmp2D9,sizeof(char),43U);});_tmp5CF.f1=_tmp7B3;});_tmp5CF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0U,_tmp5CE.f1=*f;_tmp5CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,({struct _fat_ptr _tmp7B4=({const char*_tmp2D8="\n\tdue to existential type variables.";_tag_fat(_tmp2D8,sizeof(char),37U);});_tmp5CD.f1=_tmp7B4;});_tmp5CD;});void*_tmp2D4[3U];_tmp2D4[0]=& _tmp2D5,_tmp2D4[1]=& _tmp2D6,_tmp2D4[2]=& _tmp2D7;({struct Cyc_Tcenv_Tenv*_tmp7B7=te;unsigned _tmp7B6=loc;void**_tmp7B5=topt;Cyc_Tcexp_expr_err(_tmp7B7,_tmp7B6,_tmp7B5,_tag_fat(_tmp2D4,sizeof(void*),3U));});});}
# 1376
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2B9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B8)->f1;_tmp2BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B8)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2B9;struct Cyc_List_List*fs=_tmp2BA;
# 1378
struct Cyc_Absyn_Aggrfield*_tmp2DA=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2DA;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=0U,({struct _fat_ptr _tmp7B8=({const char*_tmp2E0="type has no ";_tag_fat(_tmp2E0,sizeof(char),13U);});_tmp5D2.f1=_tmp7B8;});_tmp5D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=0U,_tmp5D1.f1=*f;_tmp5D1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D0;_tmp5D0.tag=0U,({struct _fat_ptr _tmp7B9=({const char*_tmp2DF=" member";_tag_fat(_tmp2DF,sizeof(char),8U);});_tmp5D0.f1=_tmp7B9;});_tmp5D0;});void*_tmp2DB[3U];_tmp2DB[0]=& _tmp2DC,_tmp2DB[1]=& _tmp2DD,_tmp2DB[2]=& _tmp2DE;({struct Cyc_Tcenv_Tenv*_tmp7BC=te;unsigned _tmp7BB=loc;void**_tmp7BA=topt;Cyc_Tcexp_expr_err(_tmp7BC,_tmp7BB,_tmp7BA,_tag_fat(_tmp2DB,sizeof(void*),3U));});});
# 1383
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1385
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp7BD=({const char*_tmp2E6="cannot read union member ";_tag_fat(_tmp2E6,sizeof(char),26U);});_tmp5D5.f1=_tmp7BD;});_tmp5D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,_tmp5D4.f1=*f;_tmp5D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,({struct _fat_ptr _tmp7BE=({const char*_tmp2E5=" since it is not `bits-only'";_tag_fat(_tmp2E5,sizeof(char),29U);});_tmp5D3.f1=_tmp7BE;});_tmp5D3;});void*_tmp2E1[3U];_tmp2E1[0]=& _tmp2E2,_tmp2E1[1]=& _tmp2E3,_tmp2E1[2]=& _tmp2E4;({struct Cyc_Tcenv_Tenv*_tmp7C1=te;unsigned _tmp7C0=loc;void**_tmp7BF=topt;Cyc_Tcexp_expr_err(_tmp7C1,_tmp7C0,_tmp7BF,_tag_fat(_tmp2E1,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
# 1388
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D7;_tmp5D7.tag=0U,({struct _fat_ptr _tmp7C2=({const char*_tmp2EA="expecting struct or union, found ";_tag_fat(_tmp2EA,sizeof(char),34U);});_tmp5D7.f1=_tmp7C2;});_tmp5D7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2E9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=2U,_tmp5D6.f1=(void*)aggr_type;_tmp5D6;});void*_tmp2E7[2U];_tmp2E7[0]=& _tmp2E8,_tmp2E7[1]=& _tmp2E9;({struct Cyc_Tcenv_Tenv*_tmp7C5=te;unsigned _tmp7C4=loc;void**_tmp7C3=topt;Cyc_Tcexp_expr_err(_tmp7C5,_tmp7C4,_tmp7C3,_tag_fat(_tmp2E7,sizeof(void*),2U));});});}_LL0:;}}
# 1393
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1397
({struct Cyc_Tcenv_Tenv*_tmp7C6=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp7C6,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1402
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1406
void*t2=({struct Cyc_Tcenv_Tenv*_tmp7C8=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp7C7=loc;Cyc_Tcexp_tcDeref(_tmp7C8,_tmp7C7,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1411
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1413
struct Cyc_Tcenv_Tenv*_tmp2EB=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2EB;
({struct Cyc_Tcenv_Tenv*_tmp7C9=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7C9,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp7CA=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7CA,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D9;_tmp5D9.tag=0U,({struct _fat_ptr _tmp7CB=({const char*_tmp2EF="expecting int subscript, found ";_tag_fat(_tmp2EF,sizeof(char),32U);});_tmp5D9.f1=_tmp7CB;});_tmp5D9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2EE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D8;_tmp5D8.tag=2U,_tmp5D8.f1=(void*)t2;_tmp5D8;});void*_tmp2EC[2U];_tmp2EC[0]=& _tmp2ED,_tmp2EC[1]=& _tmp2EE;({struct Cyc_Tcenv_Tenv*_tmp7CE=te;unsigned _tmp7CD=e2->loc;void**_tmp7CC=topt;Cyc_Tcexp_expr_err(_tmp7CE,_tmp7CD,_tmp7CC,_tag_fat(_tmp2EC,sizeof(void*),2U));});});{
# 1422
void*_tmp2F0=t1;struct Cyc_List_List*_tmp2F1;void*_tmp2F6;void*_tmp2F5;void*_tmp2F4;struct Cyc_Absyn_Tqual _tmp2F3;void*_tmp2F2;switch(*((int*)_tmp2F0)){case 3U: _LL1: _tmp2F2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).elt_type;_tmp2F3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).elt_tq;_tmp2F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).rgn;_tmp2F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).bounds;_tmp2F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp2F2;struct Cyc_Absyn_Tqual tq=_tmp2F3;void*rt=_tmp2F4;void*b=_tmp2F5;void*zt=_tmp2F6;
# 1426
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp2F7=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp2F7;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp2F8=eopt;struct Cyc_Absyn_Exp*e3=_tmp2F8;
struct _tuple16 _tmp2F9=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2D=_tmp2F9;struct _tuple16 _tmp2FA=_stmttmp2D;int _tmp2FC;unsigned _tmp2FB;_LL8: _tmp2FB=_tmp2FA.f1;_tmp2FC=_tmp2FA.f2;_LL9: {unsigned i=_tmp2FB;int known_i=_tmp2FC;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp2FD=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2E=_tmp2FD;struct _tuple16 _tmp2FE=_stmttmp2E;int _tmp300;unsigned _tmp2FF;_LLB: _tmp2FF=_tmp2FE.f1;_tmp300=_tmp2FE.f2;_LLC: {unsigned j=_tmp2FF;int known_j=_tmp300;
if(known_j){
struct _tuple16 _tmp301=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2F=_tmp301;struct _tuple16 _tmp302=_stmttmp2F;int _tmp304;unsigned _tmp303;_LLE: _tmp303=_tmp302.f1;_tmp304=_tmp302.f2;_LLF: {unsigned j=_tmp303;int knownj=_tmp304;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp305=0U;({unsigned _tmp7D0=loc;struct _fat_ptr _tmp7CF=({const char*_tmp306="subscript will fail at run-time";_tag_fat(_tmp306,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7D0,_tmp7CF,_tag_fat(_tmp305,sizeof(void*),0U));});});}}}}}}
# 1442
if(emit_warning)
({void*_tmp307=0U;({unsigned _tmp7D2=e2->loc;struct _fat_ptr _tmp7D1=({const char*_tmp308="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp308,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp7D2,_tmp7D1,_tag_fat(_tmp307,sizeof(void*),0U));});});}else{
# 1446
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp309=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp30=_tmp309;struct _tuple16 _tmp30A=_stmttmp30;int _tmp30C;unsigned _tmp30B;_LL11: _tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;_LL12: {unsigned i=_tmp30B;int known=_tmp30C;
if(known)
# 1451
({unsigned _tmp7D5=loc;unsigned _tmp7D4=i;void*_tmp7D3=b;Cyc_Tcutil_check_bound(_tmp7D5,_tmp7D4,_tmp7D3,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1454
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp30D=0U;({unsigned _tmp7D7=e1->loc;struct _fat_ptr _tmp7D6=({const char*_tmp30E="subscript applied to pointer to one object";_tag_fat(_tmp30E,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp7D7,_tmp7D6,_tag_fat(_tmp30D,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1459
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp30F=0U;({unsigned _tmp7D9=e1->loc;struct _fat_ptr _tmp7D8=({const char*_tmp310="can't subscript an abstract pointer";_tag_fat(_tmp310,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp7D9,_tmp7D8,_tag_fat(_tmp30F,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp2F1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F0)->f1;_LL4: {struct Cyc_List_List*ts=_tmp2F1;
# 1464
struct _tuple16 _tmp311=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp31=_tmp311;struct _tuple16 _tmp312=_stmttmp31;int _tmp314;unsigned _tmp313;_LL14: _tmp313=_tmp312.f1;_tmp314=_tmp312.f2;_LL15: {unsigned i=_tmp313;int known=_tmp314;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp316=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=0U,({
struct _fat_ptr _tmp7DA=({const char*_tmp317="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp317,sizeof(char),47U);});_tmp5DA.f1=_tmp7DA;});_tmp5DA;});void*_tmp315[1U];_tmp315[0]=& _tmp316;({struct Cyc_Tcenv_Tenv*_tmp7DC=te;unsigned _tmp7DB=loc;Cyc_Tcexp_expr_err(_tmp7DC,_tmp7DB,0,_tag_fat(_tmp315,sizeof(void*),1U));});});{
struct _handler_cons _tmp318;_push_handler(& _tmp318);{int _tmp31A=0;if(setjmp(_tmp318.handler))_tmp31A=1;if(!_tmp31A){{void*_tmp31B=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp31B;};_pop_handler();}else{void*_tmp319=(void*)Cyc_Core_get_exn_thrown();void*_tmp31C=_tmp319;void*_tmp31D;if(((struct Cyc_List_Nth_exn_struct*)_tmp31C)->tag == Cyc_List_Nth){_LL17: _LL18:
# 1470
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp31F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp7DD=({const char*_tmp326="index is ";_tag_fat(_tmp326,sizeof(char),10U);});_tmp5DF.f1=_tmp7DD;});_tmp5DF;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp320=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=12U,_tmp5DE.f1=(int)i;_tmp5DE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp321=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _fat_ptr _tmp7DE=({const char*_tmp325=" but tuple has only ";_tag_fat(_tmp325,sizeof(char),21U);});_tmp5DD.f1=_tmp7DE;});_tmp5DD;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp322=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=12U,({
int _tmp7DF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp5DC.f1=_tmp7DF;});_tmp5DC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp323=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,({struct _fat_ptr _tmp7E0=({const char*_tmp324=" fields";_tag_fat(_tmp324,sizeof(char),8U);});_tmp5DB.f1=_tmp7E0;});_tmp5DB;});void*_tmp31E[5U];_tmp31E[0]=& _tmp31F,_tmp31E[1]=& _tmp320,_tmp31E[2]=& _tmp321,_tmp31E[3]=& _tmp322,_tmp31E[4]=& _tmp323;({struct Cyc_Tcenv_Tenv*_tmp7E2=te;unsigned _tmp7E1=loc;Cyc_Tcexp_expr_err(_tmp7E2,_tmp7E1,0,_tag_fat(_tmp31E,sizeof(void*),5U));});});}else{_LL19: _tmp31D=_tmp31C;_LL1A: {void*exn=_tmp31D;(int)_rethrow(exn);}}_LL16:;}}}}}default: _LL5: _LL6:
# 1473
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp328=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E1;_tmp5E1.tag=0U,({struct _fat_ptr _tmp7E3=({const char*_tmp32A="subscript applied to ";_tag_fat(_tmp32A,sizeof(char),22U);});_tmp5E1.f1=_tmp7E3;});_tmp5E1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp329=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E0;_tmp5E0.tag=2U,_tmp5E0.f1=(void*)t1;_tmp5E0;});void*_tmp327[2U];_tmp327[0]=& _tmp328,_tmp327[1]=& _tmp329;({struct Cyc_Tcenv_Tenv*_tmp7E6=te;unsigned _tmp7E5=loc;void**_tmp7E4=topt;Cyc_Tcexp_expr_err(_tmp7E6,_tmp7E5,_tmp7E4,_tag_fat(_tmp327,sizeof(void*),2U));});});}_LL0:;}}}
# 1478
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp32B=Cyc_Tcutil_compress(*topt);void*_stmttmp32=_tmp32B;void*_tmp32C=_stmttmp32;struct Cyc_List_List*_tmp32D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp32C)->tag == 6U){_LL1: _tmp32D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp32C)->f1;_LL2: {struct Cyc_List_List*ts=_tmp32D;
# 1485
if(({int _tmp7E7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp7E7 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
goto _LL0;
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp32E=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp32E;
({struct Cyc_Tcenv_Tenv*_tmp7E8=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7E8,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1492
({struct Cyc_RgnOrder_RgnPO*_tmp7EA=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7E9=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp7EA,_tmp7E9,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp330=_cycalloc(sizeof(*_tmp330));({struct _tuple17*_tmp7EB=({struct _tuple17*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp32F->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp32F;});_tmp330->hd=_tmp7EB;}),_tmp330->tl=fields;_tmp330;});}
# 1495
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1499
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp7EC=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7EC,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp332=_cycalloc(sizeof(*_tmp332));({struct _tuple17*_tmp7EE=({struct _tuple17*_tmp331=_cycalloc(sizeof(*_tmp331));({struct Cyc_Absyn_Tqual _tmp7ED=Cyc_Absyn_empty_tqual(0U);_tmp331->f1=_tmp7ED;}),_tmp331->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp331;});_tmp332->hd=_tmp7EE;}),_tmp332->tl=fields;_tmp332;});}
# 1504
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->tag=6U,({struct Cyc_List_List*_tmp7EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp333->f1=_tmp7EF;});_tmp333;});}
# 1508
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1513
struct _tuple9*_tmp334=targ;void*_tmp335;_LL1: _tmp335=_tmp334->f3;_LL2: {void*t=_tmp335;
({unsigned _tmp7F3=loc;struct Cyc_Tcenv_Tenv*_tmp7F2=te;struct Cyc_List_List*_tmp7F1=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp7F0=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1514
Cyc_Tctyp_check_type(_tmp7F3,_tmp7F2,_tmp7F1,_tmp7F0,1,0,t);});
# 1516
({void*_tmp7F4=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->tag=36U,_tmp336->f1=0,_tmp336->f2=des;_tmp336;});orig_exp->r=_tmp7F4;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1529 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1532
void*res_t2;
int _tmp337=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp337;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp338=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->tag=0U,({union Cyc_Absyn_Cnst _tmp7F5=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp356->f1=_tmp7F5;});_tmp356;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp338;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1540
if(zero_term){
struct Cyc_Absyn_Exp*_tmp339=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp339;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp33A=0U;({unsigned _tmp7F7=e->loc;struct _fat_ptr _tmp7F6=({const char*_tmp33B="zero-terminated array doesn't end with zero.";_tag_fat(_tmp33B,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp7F7,_tmp7F6,_tag_fat(_tmp33A,sizeof(void*),0U));});});}
# 1545
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp7FA=res;struct Cyc_Absyn_Tqual _tmp7F9=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1546
struct Cyc_Absyn_Exp*_tmp7F8=sz_exp;Cyc_Absyn_array_type(_tmp7FA,_tmp7F9,_tmp7F8,
# 1548
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1550
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1553
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7FC=Cyc_Tcenv_curr_rgnpo(te);void*_tmp7FB=res;Cyc_Tcutil_coerce_list(_tmp7FC,_tmp7FB,es);}))
# 1555
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0U,({struct _fat_ptr _tmp7FD=({const char*_tmp341="elements of array do not all have the same type (";_tag_fat(_tmp341,sizeof(char),50U);});_tmp5E4.f1=_tmp7FD;});_tmp5E4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp33E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=2U,_tmp5E3.f1=(void*)res;_tmp5E3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp33F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=0U,({
struct _fat_ptr _tmp7FE=({const char*_tmp340=")";_tag_fat(_tmp340,sizeof(char),2U);});_tmp5E2.f1=_tmp7FE;});_tmp5E2;});void*_tmp33C[3U];_tmp33C[0]=& _tmp33D,_tmp33C[1]=& _tmp33E,_tmp33C[2]=& _tmp33F;({unsigned _tmp7FF=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;Cyc_Warn_err2(_tmp7FF,_tag_fat(_tmp33C,sizeof(void*),3U));});});
# 1558
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
void*_tmp342=(void*)ds->hd;void*_stmttmp33=_tmp342;void*_tmp343=_stmttmp33;struct Cyc_Absyn_Exp*_tmp344;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp343)->tag == 1U){_LL1: _LL2:
# 1563
({struct Cyc_Warn_String_Warn_Warg_struct _tmp346=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,({struct _fat_ptr _tmp800=({const char*_tmp347="only array index designators are supported";_tag_fat(_tmp347,sizeof(char),43U);});_tmp5E5.f1=_tmp800;});_tmp5E5;});void*_tmp345[1U];_tmp345[0]=& _tmp346;({unsigned _tmp801=loc;Cyc_Warn_err2(_tmp801,_tag_fat(_tmp345,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp344=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp343)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp344;
# 1565
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp348=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp34=_tmp348;struct _tuple16 _tmp349=_stmttmp34;int _tmp34B;unsigned _tmp34A;_LL6: _tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;_LL7: {unsigned i=_tmp34A;int known=_tmp34B;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=0U,({struct _fat_ptr _tmp802=({const char*_tmp34E="index designator cannot use sizeof or offsetof";_tag_fat(_tmp34E,sizeof(char),47U);});_tmp5E6.f1=_tmp802;});_tmp5E6;});void*_tmp34C[1U];_tmp34C[0]=& _tmp34D;({unsigned _tmp803=e->loc;Cyc_Warn_err2(_tmp803,_tag_fat(_tmp34C,sizeof(void*),1U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp350=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({struct _fat_ptr _tmp804=({const char*_tmp355="expecting index designator ";_tag_fat(_tmp355,sizeof(char),28U);});_tmp5EA.f1=_tmp804;});_tmp5EA;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp351=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=12U,_tmp5E9.f1=offset;_tmp5E9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp352=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0U,({struct _fat_ptr _tmp805=({const char*_tmp354=" but found ";_tag_fat(_tmp354,sizeof(char),12U);});_tmp5E8.f1=_tmp805;});_tmp5E8;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp353=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=12U,_tmp5E7.f1=(int)i;_tmp5E7;});void*_tmp34F[4U];_tmp34F[0]=& _tmp350,_tmp34F[1]=& _tmp351,_tmp34F[2]=& _tmp352,_tmp34F[3]=& _tmp353;({unsigned _tmp806=e->loc;Cyc_Warn_err2(_tmp806,_tag_fat(_tmp34F,sizeof(void*),4U));});});}
goto _LL0;}}}}_LL0:;}}}
# 1575
return res_t2;}struct _tuple19{void**f1;struct Cyc_Absyn_Tqual*f2;void**f3;};
# 1580
static struct _tuple19 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt))
return({struct _tuple19 _tmp5EB;_tmp5EB.f1=0,_tmp5EB.f2=0,_tmp5EB.f3=0;_tmp5EB;});{
void*_tmp357=Cyc_Tcutil_compress(*topt);void*_stmttmp35=_tmp357;void*_tmp358=_stmttmp35;struct Cyc_Absyn_ArrayInfo _tmp359;struct Cyc_Absyn_PtrInfo _tmp35A;switch(*((int*)_tmp358)){case 3U: _LL1: _tmp35A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1;_LL2: {struct Cyc_Absyn_PtrInfo x=_tmp35A;
# 1585
return({struct _tuple19 _tmp5EC;({void**_tmp809=({void**_tmp35B=_cycalloc(sizeof(*_tmp35B));*_tmp35B=x.elt_type;_tmp35B;});_tmp5EC.f1=_tmp809;}),({struct Cyc_Absyn_Tqual*_tmp808=({struct Cyc_Absyn_Tqual*_tmp35C=_cycalloc(sizeof(*_tmp35C));*_tmp35C=x.elt_tq;_tmp35C;});_tmp5EC.f2=_tmp808;}),({void**_tmp807=({void**_tmp35D=_cycalloc(sizeof(*_tmp35D));*_tmp35D=(x.ptr_atts).zero_term;_tmp35D;});_tmp5EC.f3=_tmp807;});_tmp5EC;});}case 4U: _LL3: _tmp359=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1;_LL4: {struct Cyc_Absyn_ArrayInfo x=_tmp359;
return({struct _tuple19 _tmp5ED;({void**_tmp80C=({void**_tmp35E=_cycalloc(sizeof(*_tmp35E));*_tmp35E=x.elt_type;_tmp35E;});_tmp5ED.f1=_tmp80C;}),({struct Cyc_Absyn_Tqual*_tmp80B=({struct Cyc_Absyn_Tqual*_tmp35F=_cycalloc(sizeof(*_tmp35F));*_tmp35F=x.tq;_tmp35F;});_tmp5ED.f2=_tmp80B;}),({void**_tmp80A=({void**_tmp360=_cycalloc(sizeof(*_tmp360));*_tmp360=x.zero_term;_tmp360;});_tmp5ED.f3=_tmp80A;});_tmp5ED;});}default: _LL5: _LL6:
 return({struct _tuple19 _tmp5EE;_tmp5EE.f1=0,_tmp5EE.f2=0,_tmp5EE.f3=0;_tmp5EE;});}_LL0:;}}
# 1592
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1595
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp361=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp36=_tmp361;void*_tmp362=_stmttmp36;void*_tmp363;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362)->f2 != 0){_LL1: _tmp363=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362)->f2)->hd;_LL2: {void*t=_tmp363;
# 1600
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp364=({void*_tmp80D=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp80D,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp364;
b->topt=bound->topt;
bound=b;}
# 1605
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1607
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp366=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _fat_ptr _tmp80E=({const char*_tmp368="expecting unsigned int, found ";_tag_fat(_tmp368,sizeof(char),31U);});_tmp5F0.f1=_tmp80E;});_tmp5F0;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp367=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=3U,_tmp5EF.f1=(void*)bound->topt;_tmp5EF;});void*_tmp365[2U];_tmp365[0]=& _tmp366,_tmp365[1]=& _tmp367;({unsigned _tmp80F=bound->loc;Cyc_Warn_err2(_tmp80F,_tag_fat(_tmp365,sizeof(void*),2U));});});}_LL0:;}
# 1611
if(!(vd->tq).real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,({struct _fat_ptr _tmp810=({const char*_tmp36B="comprehension index variable is not declared const!";_tag_fat(_tmp36B,sizeof(char),52U);});_tmp5F1.f1=_tmp810;});_tmp5F1;});void*_tmp369[1U];_tmp369[0]=& _tmp36A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp369,sizeof(void*),1U));});
# 1614
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1617
struct _tuple19 _tmp36C=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp37=_tmp36C;struct _tuple19 _tmp36D=_stmttmp37;void**_tmp370;struct Cyc_Absyn_Tqual*_tmp36F;void**_tmp36E;_LL6: _tmp36E=_tmp36D.f1;_tmp36F=_tmp36D.f2;_tmp370=_tmp36D.f3;_LL7: {void**topt2=_tmp36E;struct Cyc_Absyn_Tqual*tqopt=_tmp36F;void**ztopt=_tmp370;
# 1620
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1622
Cyc_Tcexp_check_consume(body->loc,t,body);
if(te->le == 0){
# 1625
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp372=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp811=({const char*_tmp373="bound is not constant";_tag_fat(_tmp373,sizeof(char),22U);});_tmp5F2.f1=_tmp811;});_tmp5F2;});void*_tmp371[1U];_tmp371[0]=& _tmp372;({unsigned _tmp812=bound->loc;Cyc_Warn_err2(_tmp812,_tag_fat(_tmp371,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=0U,({struct _fat_ptr _tmp813=({const char*_tmp376="body is not constant";_tag_fat(_tmp376,sizeof(char),21U);});_tmp5F3.f1=_tmp813;});_tmp5F3;});void*_tmp374[1U];_tmp374[0]=& _tmp375;({unsigned _tmp814=bound->loc;Cyc_Warn_err2(_tmp814,_tag_fat(_tmp374,sizeof(void*),1U));});});}
# 1630
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1632
struct Cyc_Absyn_Exp*_tmp377=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp377;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1636
Cyc_Tcexp_check_consume(body->loc,(void*)_check_null(body->topt),body);
# 1638
return({void*_tmp817=t;struct Cyc_Absyn_Tqual _tmp816=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp815=bound;Cyc_Absyn_array_type(_tmp817,_tmp816,_tmp815,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1643
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1646
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp378=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp38=_tmp378;void*_tmp379=_stmttmp38;void*_tmp37A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp379)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp379)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp379)->f2 != 0){_LL1: _tmp37A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp379)->f2)->hd;_LL2: {void*t=_tmp37A;
# 1651
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp37B=({void*_tmp818=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp818,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp37B;
b->topt=bound->topt;
bound=b;}
# 1656
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1658
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0U,({struct _fat_ptr _tmp819=({const char*_tmp37F="expecting unsigned int, found ";_tag_fat(_tmp37F,sizeof(char),31U);});_tmp5F5.f1=_tmp819;});_tmp5F5;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp37E=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=3U,_tmp5F4.f1=(void*)bound->topt;_tmp5F4;});void*_tmp37C[2U];_tmp37C[0]=& _tmp37D,_tmp37C[1]=& _tmp37E;({unsigned _tmp81A=bound->loc;Cyc_Warn_err2(_tmp81A,_tag_fat(_tmp37C,sizeof(void*),2U));});});}_LL0:;}{
# 1661
struct _tuple19 _tmp380=Cyc_Tcexp_ptrEltAtts(topt);struct _tuple19 _stmttmp39=_tmp380;struct _tuple19 _tmp381=_stmttmp39;void**_tmp384;struct Cyc_Absyn_Tqual*_tmp383;void**_tmp382;_LL6: _tmp382=_tmp381.f1;_tmp383=_tmp381.f2;_tmp384=_tmp381.f3;_LL7: {void**topt2=_tmp382;struct Cyc_Absyn_Tqual*tqopt=_tmp383;void**ztopt=_tmp384;
# 1664
({unsigned _tmp81D=loc;struct Cyc_Tcenv_Tenv*_tmp81C=te;struct Cyc_List_List*_tmp81B=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp81D,_tmp81C,_tmp81B,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)
Cyc_Unify_unify(*topt2,t);
# 1668
if(te->le == 0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp386=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0U,({struct _fat_ptr _tmp81E=({const char*_tmp387="bound is not constant";_tag_fat(_tmp387,sizeof(char),22U);});_tmp5F6.f1=_tmp81E;});_tmp5F6;});void*_tmp385[1U];_tmp385[0]=& _tmp386;({unsigned _tmp81F=bound->loc;Cyc_Warn_err2(_tmp81F,_tag_fat(_tmp385,sizeof(void*),1U));});});
# 1672
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp389=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=0U,({struct _fat_ptr _tmp820=({const char*_tmp38A="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp38A,sizeof(char),74U);});_tmp5F7.f1=_tmp820;});_tmp5F7;});void*_tmp388[1U];_tmp388[0]=& _tmp389;({unsigned _tmp821=loc;Cyc_Warn_err2(_tmp821,_tag_fat(_tmp388,sizeof(void*),1U));});});
# 1676
return({void*_tmp824=t;struct Cyc_Absyn_Tqual _tmp823=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp822=bound;Cyc_Absyn_array_type(_tmp824,_tmp823,_tmp822,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});}}}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1690 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1694
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp38B=_cycalloc(sizeof(*_tmp38B));*_tmp38B=ad;_tmp38B;});}else{
# 1700
{struct _handler_cons _tmp38C;_push_handler(& _tmp38C);{int _tmp38E=0;if(setjmp(_tmp38C.handler))_tmp38E=1;if(!_tmp38E){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1700
;_pop_handler();}else{void*_tmp38D=(void*)Cyc_Core_get_exn_thrown();void*_tmp38F=_tmp38D;void*_tmp390;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp38F)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1703
({struct Cyc_Warn_String_Warn_Warg_struct _tmp392=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=0U,({struct _fat_ptr _tmp825=({const char*_tmp394="unbound struct/union name ";_tag_fat(_tmp394,sizeof(char),27U);});_tmp5F9.f1=_tmp825;});_tmp5F9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp393=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=1U,_tmp5F8.f1=*tn;_tmp5F8;});void*_tmp391[2U];_tmp391[0]=& _tmp392,_tmp391[1]=& _tmp393;({unsigned _tmp826=loc;Cyc_Warn_err2(_tmp826,_tag_fat(_tmp391,sizeof(void*),2U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp390=_tmp38F;_LL4: {void*exn=_tmp390;(int)_rethrow(exn);}}_LL0:;}}}
# 1706
*ad_opt=ad;
*tn=ad->name;}
# 1709
if(ad->impl == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp396=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=0U,({struct _fat_ptr _tmp827=({const char*_tmp398="can't construct abstract ";_tag_fat(_tmp398,sizeof(char),26U);});_tmp5FB.f1=_tmp827;});_tmp5FB;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp397=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=6U,_tmp5FA.f1=ad;_tmp5FA;});void*_tmp395[2U];_tmp395[0]=& _tmp396,_tmp395[1]=& _tmp397;({struct Cyc_Tcenv_Tenv*_tmp829=te;unsigned _tmp828=loc;Cyc_Tcexp_expr_err(_tmp829,_tmp828,0,_tag_fat(_tmp395,sizeof(void*),2U));});});{
# 1716
struct Cyc_Tcenv_Tenv*_tmp399=(int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNew?Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te): te;struct Cyc_Tcenv_Tenv*te2=_tmp399;
# 1722
struct _tuple13 _tmp39A=({struct _tuple13 _tmp604;({struct Cyc_List_List*_tmp82A=Cyc_Tcenv_lookup_type_vars(te2);_tmp604.f1=_tmp82A;}),_tmp604.f2=Cyc_Core_heap_region;_tmp604;});struct _tuple13 env=_tmp39A;
struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp39B;
struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp39C;
struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp39D;
struct Cyc_List_List*_tmp39E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp39E;
struct Cyc_List_List*_tmp39F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp39F;
void*res_typ;
# 1733
if(topt != 0){
void*_tmp3A0=Cyc_Tcutil_compress(*topt);void*_stmttmp3A=_tmp3A0;void*_tmp3A1=_stmttmp3A;struct Cyc_List_List*_tmp3A3;struct Cyc_Absyn_Aggrdecl**_tmp3A2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A1)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A1)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A1)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp3A2=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A1)->f1)->f1).KnownAggr).val;_tmp3A3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A1)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp3A2;struct Cyc_List_List*all_typs2=_tmp3A3;
# 1736
if(*adptr2 != *adptr)
goto _LL9;
{struct Cyc_List_List*_tmp3A4=all_typs;struct Cyc_List_List*ats=_tmp3A4;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1742
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _tmp82B=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp82B,all_typs);});}_LL5:;}else{
# 1747
res_typ=({union Cyc_Absyn_AggrInfo _tmp82C=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp82C,all_typs);});}{
# 1750
struct Cyc_List_List*_tmp3A5=*ts;struct Cyc_List_List*user_ex_ts=_tmp3A5;
struct Cyc_List_List*_tmp3A6=exist_typs;struct Cyc_List_List*ex_ts=_tmp3A6;
while(user_ex_ts != 0 && ex_ts != 0){
# 1754
({unsigned _tmp82F=loc;struct Cyc_Tcenv_Tenv*_tmp82E=te2;struct Cyc_List_List*_tmp82D=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp82F,_tmp82E,_tmp82D,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1760
if(user_ex_ts != 0)
({void*_tmp3A7=0U;({unsigned _tmp831=loc;struct _fat_ptr _tmp830=({const char*_tmp3A8="too many explicit witness types";_tag_fat(_tmp3A8,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp831,_tmp830,_tag_fat(_tmp3A7,sizeof(void*),0U));});});
# 1763
*ts=exist_typs;{
# 1766
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3A9=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3B=_tmp3A9;struct _tuple20*_tmp3AA=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Aggrfield*_tmp3AB;_LLB: _tmp3AB=_tmp3AA->f1;_tmp3AC=_tmp3AA->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3AB;struct Cyc_Absyn_Exp*field_exp=_tmp3AC;
void*_tmp3AD=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3AD;
({struct Cyc_Tcenv_Tenv*_tmp832=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp832,& inst_fieldtyp,field_exp);});
# 1774
if(!({struct Cyc_RgnOrder_RgnPO*_tmp834=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp833=field_exp;Cyc_Tcutil_coerce_arg(_tmp834,_tmp833,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp603;_tmp603.tag=0U,({struct _fat_ptr _tmp835=({const char*_tmp3BA="field ";_tag_fat(_tmp3BA,sizeof(char),7U);});_tmp603.f1=_tmp835;});_tmp603;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp602;_tmp602.tag=0U,_tmp602.f1=*field->name;_tmp602;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp601;_tmp601.tag=0U,({struct _fat_ptr _tmp836=({const char*_tmp3B9=" of ";_tag_fat(_tmp3B9,sizeof(char),5U);});_tmp601.f1=_tmp836;});_tmp601;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3B2=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp600;_tmp600.tag=6U,_tmp600.f1=ad;_tmp600;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FF;_tmp5FF.tag=0U,({
struct _fat_ptr _tmp837=({const char*_tmp3B8=" expects type ";_tag_fat(_tmp3B8,sizeof(char),15U);});_tmp5FF.f1=_tmp837;});_tmp5FF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3B4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FE;_tmp5FE.tag=2U,_tmp5FE.f1=(void*)inst_fieldtyp;_tmp5FE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=0U,({struct _fat_ptr _tmp838=({const char*_tmp3B7=" != ";_tag_fat(_tmp3B7,sizeof(char),5U);});_tmp5FD.f1=_tmp838;});_tmp5FD;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3B6=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=3U,_tmp5FC.f1=(void*)field_exp->topt;_tmp5FC;});void*_tmp3AE[8U];_tmp3AE[0]=& _tmp3AF,_tmp3AE[1]=& _tmp3B0,_tmp3AE[2]=& _tmp3B1,_tmp3AE[3]=& _tmp3B2,_tmp3AE[4]=& _tmp3B3,_tmp3AE[5]=& _tmp3B4,_tmp3AE[6]=& _tmp3B5,_tmp3AE[7]=& _tmp3B6;({unsigned _tmp839=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp839,_tag_fat(_tmp3AE,sizeof(void*),8U));});});}}{
# 1779
struct Cyc_List_List*_tmp3BB=0;struct Cyc_List_List*rpo_inst=_tmp3BB;
{struct Cyc_List_List*_tmp3BC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3BC;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({struct _tuple0*_tmp83C=({struct _tuple0*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({void*_tmp83B=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3BD->f1=_tmp83B;}),({
void*_tmp83A=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3BD->f2=_tmp83A;});_tmp3BD;});
# 1781
_tmp3BE->hd=_tmp83C;}),_tmp3BE->tl=rpo_inst;_tmp3BE;});}}
# 1784
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}
# 1790
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1792
{void*_tmp3BF=Cyc_Tcutil_compress(ts);void*_stmttmp3C=_tmp3BF;void*_tmp3C0=_stmttmp3C;struct Cyc_List_List*_tmp3C2;enum Cyc_Absyn_AggrKind _tmp3C1;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C0)->tag == 7U){_LL1: _tmp3C1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C0)->f1;_tmp3C2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C0)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3C1;struct Cyc_List_List*fs=_tmp3C2;
# 1794
if((int)k == (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp605;_tmp605.tag=0U,({struct _fat_ptr _tmp83D=({const char*_tmp3C5="expecting struct but found union";_tag_fat(_tmp3C5,sizeof(char),33U);});_tmp605.f1=_tmp83D;});_tmp605;});void*_tmp3C3[1U];_tmp3C3[0]=& _tmp3C4;({unsigned _tmp83E=loc;Cyc_Warn_err2(_tmp83E,_tag_fat(_tmp3C3,sizeof(void*),1U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple20*_tmp3C6=(struct _tuple20*)fields->hd;struct _tuple20*_stmttmp3D=_tmp3C6;struct _tuple20*_tmp3C7=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Aggrfield*_tmp3C8;_LL6: _tmp3C8=_tmp3C7->f1;_tmp3C9=_tmp3C7->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3C8;struct Cyc_Absyn_Exp*field_exp=_tmp3C9;
({struct Cyc_Tcenv_Tenv*_tmp840=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp83F=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp840,_tmp83F,field_exp);});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp842=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp841=field_exp;Cyc_Tcutil_coerce_arg(_tmp842,_tmp841,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0U,({struct _fat_ptr _tmp843=({const char*_tmp3D3="field ";_tag_fat(_tmp3D3,sizeof(char),7U);});_tmp60B.f1=_tmp843;});_tmp60B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60A;_tmp60A.tag=0U,_tmp60A.f1=*field->name;_tmp60A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp609;_tmp609.tag=0U,({
struct _fat_ptr _tmp844=({const char*_tmp3D2=" of struct expects type ";_tag_fat(_tmp3D2,sizeof(char),25U);});_tmp609.f1=_tmp844;});_tmp609;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3CE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp608;_tmp608.tag=2U,_tmp608.f1=(void*)field->type;_tmp608;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp845=({const char*_tmp3D1=" != ";_tag_fat(_tmp3D1,sizeof(char),5U);});_tmp607.f1=_tmp845;});_tmp607;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3D0=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp606;_tmp606.tag=3U,_tmp606.f1=(void*)field_exp->topt;_tmp606;});void*_tmp3CA[6U];_tmp3CA[0]=& _tmp3CB,_tmp3CA[1]=& _tmp3CC,_tmp3CA[2]=& _tmp3CD,_tmp3CA[3]=& _tmp3CE,_tmp3CA[4]=& _tmp3CF,_tmp3CA[5]=& _tmp3D0;({unsigned _tmp846=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp846,_tag_fat(_tmp3CA,sizeof(void*),6U));});});}}
# 1807
goto _LL0;}}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60C;_tmp60C.tag=0U,({struct _fat_ptr _tmp847=({const char*_tmp3D6="tcAnonStruct: wrong type";_tag_fat(_tmp3D6,sizeof(char),25U);});_tmp60C.f1=_tmp847;});_tmp60C;});void*_tmp3D4[1U];_tmp3D4[0]=& _tmp3D5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3D4,sizeof(void*),1U));});}_LL0:;}
# 1810
return ts;}
# 1814
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1817
struct _tuple13 _tmp3D7=({struct _tuple13 _tmp617;({struct Cyc_List_List*_tmp848=Cyc_Tcenv_lookup_type_vars(te);_tmp617.f1=_tmp848;}),_tmp617.f2=Cyc_Core_heap_region;_tmp617;});struct _tuple13 env=_tmp3D7;
struct Cyc_List_List*_tmp3D8=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3D8;
struct Cyc_List_List*_tmp3D9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3D9;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp849=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp849,all_typs);});
# 1822
if(topt != 0){
void*_tmp3DA=Cyc_Tcutil_compress(*topt);void*_stmttmp3E=_tmp3DA;void*_tmp3DB=_stmttmp3E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1828
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp84B=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp84A=e;Cyc_Tcutil_coerce_arg(_tmp84B,_tmp84A,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp612;_tmp612.tag=0U,({struct _fat_ptr _tmp84C=({const char*_tmp3E6="datatype constructor ";_tag_fat(_tmp3E6,sizeof(char),22U);});_tmp612.f1=_tmp84C;});_tmp612;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp611;_tmp611.tag=1U,_tmp611.f1=tuf->name;_tmp611;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp610;_tmp610.tag=0U,({
struct _fat_ptr _tmp84D=({const char*_tmp3E5=" expects argument of type ";_tag_fat(_tmp3E5,sizeof(char),27U);});_tmp610.f1=_tmp84D;});_tmp610;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3E0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60F;_tmp60F.tag=2U,_tmp60F.f1=(void*)t;_tmp60F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60E;_tmp60E.tag=0U,({struct _fat_ptr _tmp84E=({const char*_tmp3E4=" but this argument has type ";_tag_fat(_tmp3E4,sizeof(char),29U);});_tmp60E.f1=_tmp84E;});_tmp60E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0U,
e->topt == 0?({struct _fat_ptr _tmp850=(struct _fat_ptr)({const char*_tmp3E3="?";_tag_fat(_tmp3E3,sizeof(char),2U);});_tmp60D.f1=_tmp850;}):({struct _fat_ptr _tmp84F=Cyc_Absynpp_typ2string((void*)_check_null(e->topt));_tmp60D.f1=_tmp84F;});_tmp60D;});void*_tmp3DC[6U];_tmp3DC[0]=& _tmp3DD,_tmp3DC[1]=& _tmp3DE,_tmp3DC[2]=& _tmp3DF,_tmp3DC[3]=& _tmp3E0,_tmp3DC[4]=& _tmp3E1,_tmp3DC[5]=& _tmp3E2;({unsigned _tmp851=e->loc;Cyc_Warn_err2(_tmp851,_tag_fat(_tmp3DC,sizeof(void*),6U));});});
Cyc_Unify_explain_failure();}}
# 1841
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp614;_tmp614.tag=0U,({
struct _fat_ptr _tmp852=({const char*_tmp3EA="too many arguments for datatype constructor ";_tag_fat(_tmp3EA,sizeof(char),45U);});_tmp614.f1=_tmp852;});_tmp614;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3E9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp613;_tmp613.tag=1U,_tmp613.f1=tuf->name;_tmp613;});void*_tmp3E7[2U];_tmp3E7[0]=& _tmp3E8,_tmp3E7[1]=& _tmp3E9;({struct Cyc_Tcenv_Tenv*_tmp855=te;unsigned _tmp854=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp853=topt;Cyc_Tcexp_expr_err(_tmp855,_tmp854,_tmp853,_tag_fat(_tmp3E7,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({
struct _fat_ptr _tmp856=({const char*_tmp3EE="too few arguments for datatype constructor ";_tag_fat(_tmp3EE,sizeof(char),44U);});_tmp616.f1=_tmp856;});_tmp616;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3ED=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp615;_tmp615.tag=1U,_tmp615.f1=tuf->name;_tmp615;});void*_tmp3EB[2U];_tmp3EB[0]=& _tmp3EC,_tmp3EB[1]=& _tmp3ED;({struct Cyc_Tcenv_Tenv*_tmp859=te;unsigned _tmp858=loc;void**_tmp857=topt;Cyc_Tcexp_expr_err(_tmp859,_tmp858,_tmp857,_tag_fat(_tmp3EB,sizeof(void*),2U));});});
return res;}}
# 1851
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1853
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1856
if(topt == 0)
return 0;{
void*_tmp3EF=Cyc_Tcutil_compress(*topt);void*_stmttmp3F=_tmp3EF;void*_tmp3F0=_stmttmp3F;void*_tmp3F1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0)->tag == 3U){_LL1: _tmp3F1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0)->f1).elt_type;_LL2: {void*elt_typ=_tmp3F1;
# 1860
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1866
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1868
enum Cyc_Absyn_AliasQual _tmp3F2=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp40=_tmp3F2;enum Cyc_Absyn_AliasQual _tmp3F3=_stmttmp40;if(_tmp3F3 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 1874
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1878
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1882
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3F4=Cyc_Tcutil_compress(handle_type);void*_stmttmp41=_tmp3F4;void*_tmp3F5=_stmttmp41;void*_tmp3F6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F5)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F5)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F5)->f2 != 0){_LL1: _tmp3F6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F5)->f2)->hd;_LL2: {void*r=_tmp3F6;
# 1887
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1891
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp85A=({const char*_tmp3FA="expecting region_t type but found ";_tag_fat(_tmp3FA,sizeof(char),35U);});_tmp619.f1=_tmp85A;});_tmp619;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp618;_tmp618.tag=2U,_tmp618.f1=(void*)handle_type;_tmp618;});void*_tmp3F7[2U];_tmp3F7[0]=& _tmp3F8,_tmp3F7[1]=& _tmp3F9;({unsigned _tmp85B=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;Cyc_Warn_err2(_tmp85B,_tag_fat(_tmp3F7,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}else{
# 1897
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp85C=Cyc_Absyn_uniquergn_exp();*ropt=_tmp85C;});}}}
# 1904
({struct Cyc_Tcenv_Tenv*_tmp85D=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp85D,& Cyc_Absyn_uint_type,*e);});{
# 1913 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp85E=({const char*_tmp3FD="calloc with empty type";_tag_fat(_tmp3FD,sizeof(char),23U);});_tmp61A.f1=_tmp85E;});_tmp61A;});void*_tmp3FB[1U];_tmp3FB[0]=& _tmp3FC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3FB,sizeof(void*),1U));});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp861=loc;struct Cyc_Tcenv_Tenv*_tmp860=te;struct Cyc_List_List*_tmp85F=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp861,_tmp860,_tmp85F,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp862=({const char*_tmp403="calloc cannot be used with type ";_tag_fat(_tmp403,sizeof(char),33U);});_tmp61D.f1=_tmp862;});_tmp61D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp400=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61C;_tmp61C.tag=2U,_tmp61C.f1=(void*)elt_type;_tmp61C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp401=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0U,({
struct _fat_ptr _tmp863=({const char*_tmp402="\n\t(type needs initialization)";_tag_fat(_tmp402,sizeof(char),30U);});_tmp61B.f1=_tmp863;});_tmp61B;});void*_tmp3FE[3U];_tmp3FE[0]=& _tmp3FF,_tmp3FE[1]=& _tmp400,_tmp3FE[2]=& _tmp401;({unsigned _tmp864=loc;Cyc_Warn_err2(_tmp864,_tag_fat(_tmp3FE,sizeof(void*),3U));});});
num_elts=*e;
one_elt=0;}else{
# 1928
void*er=(*e)->r;
retry_sizeof: {
void*_tmp404=er;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;void*_tmp407;switch(*((int*)_tmp404)){case 17U: _LL6: _tmp407=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp404)->f1;_LL7: {void*t2=_tmp407;
# 1932
elt_type=t2;
({void**_tmp865=({void**_tmp408=_cycalloc(sizeof(*_tmp408));*_tmp408=elt_type;_tmp408;});*t=_tmp865;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp404)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp404)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp404)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp404)->f2)->tl)->tl == 0){_LL8: _tmp405=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp404)->f2)->hd;_tmp406=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp404)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp405;struct Cyc_Absyn_Exp*e2=_tmp406;
# 1939
{struct _tuple0 _tmp409=({struct _tuple0 _tmp621;_tmp621.f1=e1->r,_tmp621.f2=e2->r;_tmp621;});struct _tuple0 _stmttmp42=_tmp409;struct _tuple0 _tmp40A=_stmttmp42;void*_tmp40B;void*_tmp40C;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp40A.f1)->tag == 17U){_LLD: _tmp40C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp40A.f1)->f1;_LLE: {void*t1=_tmp40C;
# 1941
e1=e2;
_tmp40B=t1;goto _LL10;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp40A.f2)->tag == 17U){_LLF: _tmp40B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp40A.f2)->f1;_LL10: {void*t2=_tmp40B;
# 1945
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 1948
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp866=({const char*_tmp412="malloc cannot be used with type ";_tag_fat(_tmp412,sizeof(char),33U);});_tmp620.f1=_tmp866;});_tmp620;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp40F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61F;_tmp61F.tag=2U,_tmp61F.f1=(void*)t2;_tmp61F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp410=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0U,({
struct _fat_ptr _tmp867=({const char*_tmp411="\n\t(type needs initialization)";_tag_fat(_tmp411,sizeof(char),30U);});_tmp61E.f1=_tmp867;});_tmp61E;});void*_tmp40D[3U];_tmp40D[0]=& _tmp40E,_tmp40D[1]=& _tmp40F,_tmp40D[2]=& _tmp410;({unsigned _tmp868=loc;Cyc_Warn_err2(_tmp868,_tag_fat(_tmp40D,sizeof(void*),3U));});});else{
# 1952
*is_calloc=1;}}
# 1954
elt_type=t2;
({void**_tmp869=({void**_tmp413=_cycalloc(sizeof(*_tmp413));*_tmp413=elt_type;_tmp413;});*t=_tmp869;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 1961
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 1963
 No_sizeof: {
# 1966
struct Cyc_Absyn_Exp*_tmp414=*e;struct Cyc_Absyn_Exp*real_e=_tmp414;
{void*_tmp415=real_e->r;void*_stmttmp43=_tmp415;void*_tmp416=_stmttmp43;struct Cyc_Absyn_Exp*_tmp417;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp416)->tag == 14U){_LL14: _tmp417=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp416)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp417;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 1971
{void*_tmp418=Cyc_Tcutil_compress((void*)_check_null(real_e->topt));void*_stmttmp44=_tmp418;void*_tmp419=_stmttmp44;void*_tmp41A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp419)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp419)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp419)->f2 != 0){_LL19: _tmp41A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp419)->f2)->hd;_LL1A: {void*tagt=_tmp41A;
# 1973
{void*_tmp41B=Cyc_Tcutil_compress(tagt);void*_stmttmp45=_tmp41B;void*_tmp41C=_stmttmp45;struct Cyc_Absyn_Exp*_tmp41D;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp41C)->tag == 9U){_LL1E: _tmp41D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp41C)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp41D;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 1977
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 1980
elt_type=Cyc_Absyn_char_type;
({void**_tmp86A=({void**_tmp41E=_cycalloc(sizeof(*_tmp41E));*_tmp41E=elt_type;_tmp41E;});*t=_tmp86A;});
num_elts=*e;
one_elt=0;}
# 1985
goto _LL5;}_LL5:;}}
# 1989
*is_fat=!one_elt;
# 1992
{void*_tmp41F=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp420;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp420=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp420;
# 1994
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp422=({struct Cyc_Warn_String_Warn_Warg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp86B=({const char*_tmp423="malloc with existential types not yet implemented";_tag_fat(_tmp423,sizeof(char),50U);});_tmp622.f1=_tmp86B;});_tmp622;});void*_tmp421[1U];_tmp421[0]=& _tmp422;({unsigned _tmp86C=loc;Cyc_Warn_err2(_tmp86C,_tag_fat(_tmp421,sizeof(void*),1U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2001
void*(*_tmp424)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp424;
void*_tmp425=Cyc_Absyn_false_type;void*zero_term=_tmp425;
if(topt != 0){
void*_tmp426=Cyc_Tcutil_compress(*topt);void*_stmttmp46=_tmp426;void*_tmp427=_stmttmp46;void*_tmp42A;void*_tmp429;void*_tmp428;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp427)->tag == 3U){_LL28: _tmp428=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp427)->f1).ptr_atts).nullable;_tmp429=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp427)->f1).ptr_atts).bounds;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp427)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp428;void*b=_tmp429;void*zt=_tmp42A;
# 2006
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2011
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp42C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp86D=({const char*_tmp42D="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp42D,sizeof(char),55U);});_tmp623.f1=_tmp86D;});_tmp623;});void*_tmp42B[1U];_tmp42B[0]=& _tmp42C;({unsigned _tmp86E=loc;Cyc_Warn_warn2(_tmp86E,_tag_fat(_tmp42B,sizeof(void*),1U));});});
*is_calloc=1;}{
# 2017
struct Cyc_Absyn_Exp*_tmp42E=({void*_tmp86F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp86F,b);});struct Cyc_Absyn_Exp*eopt=_tmp42E;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp42F=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp42F;
int _tmp430=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp430;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp873=elt_type;void*_tmp872=rgn;struct Cyc_Absyn_Tqual _tmp871=Cyc_Absyn_empty_tqual(0U);void*_tmp870=b;Cyc_Absyn_atb_type(_tmp873,_tmp872,_tmp871,_tmp870,zero_term);});}{
# 2025
void*_tmp431=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_stmttmp47=_tmp431;void*_tmp432=_stmttmp47;void*_tmp433;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->f2 != 0){_LL2D: _tmp433=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp432)->f2)->hd;_LL2E: {void*tagtyp=_tmp433;
# 2027
struct Cyc_Absyn_Exp*_tmp434=({void*_tmp874=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp874,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp434;
# 2029
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp878=elt_type;void*_tmp877=rgn;struct Cyc_Absyn_Tqual _tmp876=Cyc_Absyn_empty_tqual(0U);void*_tmp875=b;Cyc_Absyn_atb_type(_tmp878,_tmp877,_tmp876,_tmp875,zero_term);});}
# 2033
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2037
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2040
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp87C)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp87B=elt_type;void*_tmp87A=rgn;struct Cyc_Absyn_Tqual _tmp879=Cyc_Absyn_empty_tqual(0U);_tmp87C(_tmp87B,_tmp87A,_tmp879,zero_term);});}}}
# 2045
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2047
struct Cyc_Tcenv_Tenv*_tmp435=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp435;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp436=(void*)_check_null(e1->topt);void*t1=_tmp436;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2054
{void*_tmp437=Cyc_Tcutil_compress(t1);void*_stmttmp48=_tmp437;void*_tmp438=_stmttmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp438)->tag == 4U){_LL1: _LL2:
({void*_tmp439=0U;({unsigned _tmp87E=loc;struct _fat_ptr _tmp87D=({const char*_tmp43A="cannot assign to an array";_tag_fat(_tmp43A,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp87E,_tmp87D,_tag_fat(_tmp439,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2059
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp87F=({const char*_tmp43D="swap not allowed for non-pointer or non-word-sized types";_tag_fat(_tmp43D,sizeof(char),57U);});_tmp624.f1=_tmp87F;});_tmp624;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43C;({unsigned _tmp880=loc;Cyc_Warn_err2(_tmp880,_tag_fat(_tmp43B,sizeof(void*),1U));});});
# 2063
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp43F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp881=({const char*_tmp440="swap non-lvalue";_tag_fat(_tmp440,sizeof(char),16U);});_tmp625.f1=_tmp881;});_tmp625;});void*_tmp43E[1U];_tmp43E[0]=& _tmp43F;({struct Cyc_Tcenv_Tenv*_tmp884=te;unsigned _tmp883=e1->loc;void**_tmp882=topt;Cyc_Tcexp_expr_err(_tmp884,_tmp883,_tmp882,_tag_fat(_tmp43E,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp442=({struct Cyc_Warn_String_Warn_Warg_struct _tmp626;_tmp626.tag=0U,({struct _fat_ptr _tmp885=({const char*_tmp443="swap non-lvalue";_tag_fat(_tmp443,sizeof(char),16U);});_tmp626.f1=_tmp885;});_tmp626;});void*_tmp441[1U];_tmp441[0]=& _tmp442;({struct Cyc_Tcenv_Tenv*_tmp888=te;unsigned _tmp887=e2->loc;void**_tmp886=topt;Cyc_Tcexp_expr_err(_tmp888,_tmp887,_tmp886,_tag_fat(_tmp441,sizeof(void*),1U));});});
# 2068
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp445=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp889=({const char*_tmp44A="type mismatch: ";_tag_fat(_tmp44A,sizeof(char),16U);});_tmp62A.f1=_tmp889;});_tmp62A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp446=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp629;_tmp629.tag=2U,_tmp629.f1=(void*)t1;_tmp629;});struct Cyc_Warn_String_Warn_Warg_struct _tmp447=({struct Cyc_Warn_String_Warn_Warg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp88A=({const char*_tmp449=" != ";_tag_fat(_tmp449,sizeof(char),5U);});_tmp628.f1=_tmp88A;});_tmp628;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp448=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp627;_tmp627.tag=2U,_tmp627.f1=(void*)t2;_tmp627;});void*_tmp444[4U];_tmp444[0]=& _tmp445,_tmp444[1]=& _tmp446,_tmp444[2]=& _tmp447,_tmp444[3]=& _tmp448;({struct Cyc_Tcenv_Tenv*_tmp88D=te;unsigned _tmp88C=loc;void**_tmp88B=topt;Cyc_Tcexp_expr_err(_tmp88D,_tmp88C,_tmp88B,_tag_fat(_tmp444,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2076
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp88E=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp88E,s,1);});
# 2079
while(1){
void*_tmp44B=s->r;void*_stmttmp49=_tmp44B;void*_tmp44C=_stmttmp49;struct Cyc_Absyn_Stmt*_tmp44D;struct Cyc_Absyn_Stmt*_tmp44E;struct Cyc_Absyn_Exp*_tmp44F;switch(*((int*)_tmp44C)){case 1U: _LL1: _tmp44F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp44C)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp44F;
# 2083
void*_tmp450=(void*)_check_null(e->topt);void*t=_tmp450;
if(!({void*_tmp88F=t;Cyc_Unify_unify(_tmp88F,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp452=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp890=({const char*_tmp454="statement expression returns type ";_tag_fat(_tmp454,sizeof(char),35U);});_tmp62C.f1=_tmp890;});_tmp62C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp453=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62B;_tmp62B.tag=2U,_tmp62B.f1=(void*)t;_tmp62B;});void*_tmp451[2U];_tmp451[0]=& _tmp452,_tmp451[1]=& _tmp453;({unsigned _tmp891=loc;Cyc_Tcexp_err_and_explain(_tmp891,_tag_fat(_tmp451,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp44E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44C)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp44E;
s=s2;continue;}case 12U: _LL5: _tmp44D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44C)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp44D;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp456=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62D;_tmp62D.tag=0U,({
struct _fat_ptr _tmp892=({const char*_tmp457="statement expression must end with expression";_tag_fat(_tmp457,sizeof(char),46U);});_tmp62D.f1=_tmp892;});_tmp62D;});void*_tmp455[1U];_tmp455[0]=& _tmp456;({struct Cyc_Tcenv_Tenv*_tmp895=te;unsigned _tmp894=loc;void**_tmp893=topt;Cyc_Tcexp_expr_err(_tmp895,_tmp894,_tmp893,_tag_fat(_tmp455,sizeof(void*),1U));});});}_LL0:;}}
# 2094
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp896=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp896,0,e);}));
{void*_tmp458=t;struct Cyc_Absyn_Aggrdecl*_tmp459;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp458)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp458)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp458)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp459=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp458)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp459;
# 2098
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2102
({struct Cyc_Warn_String_Warn_Warg_struct _tmp45B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62F;_tmp62F.tag=0U,({struct _fat_ptr _tmp897=({const char*_tmp45D="expecting @tagged union but found ";_tag_fat(_tmp45D,sizeof(char),35U);});_tmp62F.f1=_tmp897;});_tmp62F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62E;_tmp62E.tag=2U,_tmp62E.f1=(void*)t;_tmp62E;});void*_tmp45A[2U];_tmp45A[0]=& _tmp45B,_tmp45A[1]=& _tmp45C;({unsigned _tmp898=loc;Cyc_Warn_err2(_tmp898,_tag_fat(_tmp45A,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}
# 2105
return Cyc_Absyn_uint_type;}
# 2108
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp89A=te;struct Cyc_Absyn_Exp*_tmp899=e;Cyc_Tcexp_tcTest(_tmp89A,_tmp899,({const char*_tmp45E="@assert";_tag_fat(_tmp45E,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp460=({struct Cyc_Warn_String_Warn_Warg_struct _tmp630;_tmp630.tag=0U,({struct _fat_ptr _tmp89B=({const char*_tmp461="@assert expression may have side effects";_tag_fat(_tmp461,sizeof(char),41U);});_tmp630.f1=_tmp89B;});_tmp630;});void*_tmp45F[1U];_tmp45F[0]=& _tmp460;({unsigned _tmp89C=loc;Cyc_Warn_err2(_tmp89C,_tag_fat(_tmp45F,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;}
# 2116
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2120
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp462=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));struct Cyc_Tcenv_Tenv*te=_tmp462;
if(*rgn_handle != 0){
# 2125
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp463=Cyc_Tcutil_compress(handle_type);void*_stmttmp4A=_tmp463;void*_tmp464=_stmttmp4A;void*_tmp465;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f2 != 0){_LL1: _tmp465=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp464)->f2)->hd;_LL2: {void*r=_tmp465;
# 2130
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2134
({struct Cyc_Warn_String_Warn_Warg_struct _tmp467=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp89D=({const char*_tmp469="expecting region_t type but found ";_tag_fat(_tmp469,sizeof(char),35U);});_tmp632.f1=_tmp89D;});_tmp632;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp468=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp631;_tmp631.tag=2U,_tmp631.f1=(void*)handle_type;_tmp631;});void*_tmp466[2U];_tmp466[0]=& _tmp467,_tmp466[1]=& _tmp468;({unsigned _tmp89E=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp89E,_tag_fat(_tmp466,sizeof(void*),2U));});});
# 2136
goto _LL0;}_LL0:;}else{
# 2138
if(topt != 0){
# 2141
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)
({struct Cyc_Absyn_Exp*_tmp89F=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp89F;});}}}{
# 2149
void*_tmp46A=e1->r;void*_stmttmp4B=_tmp46A;void*_tmp46B=_stmttmp4B;struct Cyc_List_List*_tmp46C;struct Cyc_List_List*_tmp46E;struct Cyc_Core_Opt*_tmp46D;switch(*((int*)_tmp46B)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2154
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp470=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp8A0=({const char*_tmp471="tcNew: comprehension returned non-array type";_tag_fat(_tmp471,sizeof(char),45U);});_tmp633.f1=_tmp8A0;});_tmp633;});void*_tmp46F[1U];_tmp46F[0]=& _tmp470;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp46F,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8A3=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8A2=loc;void*_tmp8A1=res_typ;Cyc_Tcutil_silent_castable(_tmp8A3,_tmp8A2,_tmp8A1,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2166
return res_typ;}case 36U: _LLA: _tmp46D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp46E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp46D;struct Cyc_List_List*des=_tmp46E;
# 2168
({void*_tmp8A4=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->tag=26U,_tmp472->f1=des;_tmp472;});e1->r=_tmp8A4;});
_tmp46C=des;goto _LLD;}case 26U: _LLC: _tmp46C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LLD: {struct Cyc_List_List*des=_tmp46C;
# 2171
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp473=Cyc_Tcutil_compress(*topt);void*_stmttmp4C=_tmp473;void*_tmp474=_stmttmp4C;void*_tmp476;void**_tmp475;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->tag == 3U){_LL15: _tmp475=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).elt_type;_tmp476=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp475;void*zt=_tmp476;
# 2177
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2183
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp478=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,({struct _fat_ptr _tmp8A5=({const char*_tmp479="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp479,sizeof(char),50U);});_tmp634.f1=_tmp8A5;});_tmp634;});void*_tmp477[1U];_tmp477[0]=& _tmp478;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp477,sizeof(void*),1U));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2192
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8A8=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8A7=loc;void*_tmp8A6=res_typ;Cyc_Tcutil_silent_castable(_tmp8A8,_tmp8A7,_tmp8A6,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2199
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2204
void*_tmp47A=({void*_tmp8AC=Cyc_Absyn_char_type;void*_tmp8AB=rgn;struct Cyc_Absyn_Tqual _tmp8AA=Cyc_Absyn_const_tqual(0U);void*_tmp8A9=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8AC,_tmp8AB,_tmp8AA,_tmp8A9,Cyc_Absyn_true_type);});void*topt2=_tmp47A;
# 2206
void*_tmp47B=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp47B;
return({void*_tmp8B0=t;void*_tmp8AF=rgn;struct Cyc_Absyn_Tqual _tmp8AE=Cyc_Absyn_empty_tqual(0U);void*_tmp8AD=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2207
Cyc_Absyn_atb_type(_tmp8B0,_tmp8AF,_tmp8AE,_tmp8AD,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2211
void*_tmp47C=({void*_tmp8B4=Cyc_Absyn_wchar_type();void*_tmp8B3=rgn;struct Cyc_Absyn_Tqual _tmp8B2=Cyc_Absyn_const_tqual(0U);void*_tmp8B1=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8B4,_tmp8B3,_tmp8B2,_tmp8B1,Cyc_Absyn_true_type);});void*topt2=_tmp47C;
# 2213
void*_tmp47D=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp47D;
return({void*_tmp8B8=t;void*_tmp8B7=rgn;struct Cyc_Absyn_Tqual _tmp8B6=Cyc_Absyn_empty_tqual(0U);void*_tmp8B5=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2214
Cyc_Absyn_atb_type(_tmp8B8,_tmp8B7,_tmp8B6,_tmp8B5,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2220
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp47E=Cyc_Tcutil_compress(*topt);void*_stmttmp4D=_tmp47E;void*_tmp47F=_stmttmp4D;struct Cyc_Absyn_Tqual _tmp481;void**_tmp480;switch(*((int*)_tmp47F)){case 3U: _LL1A: _tmp480=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47F)->f1).elt_type;_tmp481=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47F)->f1).elt_tq;_LL1B: {void**elttype=_tmp480;struct Cyc_Absyn_Tqual tq=_tmp481;
# 2226
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47F)->f1)->tag == 18U){_LL1C: _LL1D:
# 2230
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2236
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2238
Cyc_Tcexp_check_consume(e1->loc,telt,e1);{
struct Cyc_Absyn_PtrInfo _tmp482=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp482;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp8B9=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp8B9;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp8BC=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8BB=loc;void*_tmp8BA=res_typ;Cyc_Tcutil_silent_castable(_tmp8BC,_tmp8BB,_tmp8BA,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2251
return res_typ;}}}}}_LL5:;}}
# 2257
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8BE=t=({void*_tmp8BD=t;Cyc_Tcutil_promote_array(_tmp8BD,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8BE;});
return t;}
# 2267
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2271
Cyc_Tcexp_check_consume(e->loc,t,e);{
void*_tmp483=e->r;void*_stmttmp4E=_tmp483;void*_tmp484=_stmttmp4E;switch(*((int*)_tmp484)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp484)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2279
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8C0=t=({void*_tmp8BF=t;Cyc_Tcutil_promote_array(_tmp8BF,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8C0;});
return t;}_LL0:;}}
# 2292 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp8C1=Cyc_Tcutil_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp8C1;});
# 2296
{void*_tmp485=e->r;void*_stmttmp4F=_tmp485;void*_tmp486=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp487;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp486)->tag == 12U){_LL1: _tmp487=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp486)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp487;
return t;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2301
void*_tmp488=t;void*_tmp489;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp488)->tag == 3U){_LL6: _tmp489=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp488)->f1).elt_type;_LL7: {void*t2=_tmp489;
# 2303
void*_tmp48A=Cyc_Tcutil_compress(t2);void*_stmttmp50=_tmp48A;void*_tmp48B=_stmttmp50;struct Cyc_Absyn_FnInfo _tmp48C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48B)->tag == 5U){_LLB: _tmp48C=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48B)->f1;_LLC: {struct Cyc_Absyn_FnInfo info=_tmp48C;
# 2305
struct _tuple13 _tmp48D=({struct _tuple13 _tmp635;({struct Cyc_List_List*_tmp8C2=Cyc_Tcenv_lookup_type_vars(te);_tmp635.f1=_tmp8C2;}),_tmp635.f2=Cyc_Core_heap_region;_tmp635;});struct _tuple13 env=_tmp48D;
struct Cyc_List_List*_tmp48E=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp48E;
struct Cyc_List_List*_tmp48F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*ts=_tmp48F;
struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp490;
({void*_tmp8C3=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->tag=13U,_tmp491->f1=inner,_tmp491->f2=ts;_tmp491;});e->r=_tmp8C3;});
({void*_tmp8C4=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp8C4;});
return(void*)_check_null(e->topt);}}else{_LLD: _LLE:
 return t;}_LLA:;}}else{_LL8: _LL9:
# 2314
 return t;}_LL5:;}}}
# 2323
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2325
struct Cyc_List_List*_tmp492=0;struct Cyc_List_List*decls=_tmp492;
# 2327
{void*_tmp493=fn_exp->r;void*_stmttmp51=_tmp493;void*_tmp494=_stmttmp51;struct Cyc_List_List*_tmp495;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp494)->tag == 10U){_LL1: _tmp495=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp494)->f2;_LL2: {struct Cyc_List_List*es=_tmp495;
# 2329
{void*_tmp496=e->r;void*_stmttmp52=_tmp496;void*_tmp497=_stmttmp52;struct Cyc_List_List*_tmp498;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->tag == 10U){_LL6: _tmp498=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497)->f2;_LL7: {struct Cyc_List_List*es2=_tmp498;
# 2331
struct Cyc_List_List*_tmp499=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp499;
int _tmp49A=0;int arg_cnt=_tmp49A;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp8C5=({const char*_tmp4A3="bad count ";_tag_fat(_tmp4A3,sizeof(char),11U);});_tmp63A.f1=_tmp8C5;});_tmp63A;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp49D=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp639;_tmp639.tag=12U,_tmp639.f1=arg_cnt;_tmp639;});struct Cyc_Warn_String_Warn_Warg_struct _tmp49E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp638;_tmp638.tag=0U,({struct _fat_ptr _tmp8C6=({const char*_tmp4A2="/";_tag_fat(_tmp4A2,sizeof(char),2U);});_tmp638.f1=_tmp8C6;});_tmp638;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp49F=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp637;_tmp637.tag=12U,_tmp637.f1=(int)arg_exps->hd;_tmp637;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp636;_tmp636.tag=0U,({
struct _fat_ptr _tmp8C7=({const char*_tmp4A1=" for alias coercion!";_tag_fat(_tmp4A1,sizeof(char),21U);});_tmp636.f1=_tmp8C7;});_tmp636;});void*_tmp49B[5U];_tmp49B[0]=& _tmp49C,_tmp49B[1]=& _tmp49D,_tmp49B[2]=& _tmp49E,_tmp49B[3]=& _tmp49F,_tmp49B[4]=& _tmp4A0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp49B,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2343
struct _tuple12 _tmp4A4=({struct Cyc_Absyn_Exp*_tmp8C8=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp8C8,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp53=_tmp4A4;struct _tuple12 _tmp4A5=_stmttmp53;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Decl*_tmp4A6;_LLB: _tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp4A6;struct Cyc_Absyn_Exp*ve=_tmp4A7;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8->hd=d,_tmp4A8->tl=decls;_tmp4A8;});
arg_exps=arg_exps->tl;}}}
# 2348
goto _LL5;}}else{_LL8: _LL9:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0U,({struct _fat_ptr _tmp8C9=({const char*_tmp4AB="not a function call!";_tag_fat(_tmp4AB,sizeof(char),21U);});_tmp63B.f1=_tmp8C9;});_tmp63B;});void*_tmp4A9[1U];_tmp4A9[0]=& _tmp4AA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4A9,sizeof(void*),1U));});}_LL5:;}
# 2351
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63C;_tmp63C.tag=0U,({struct _fat_ptr _tmp8CA=({const char*_tmp4AE="not a function call!";_tag_fat(_tmp4AE,sizeof(char),21U);});_tmp63C.f1=_tmp8CA;});_tmp63C;});void*_tmp4AC[1U];_tmp4AC[0]=& _tmp4AD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4AC,sizeof(void*),1U));});}_LL0:;}
# 2355
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4AF=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4AF;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp8CD=({struct Cyc_Absyn_Decl*_tmp8CC=d;struct Cyc_Absyn_Stmt*_tmp8CB=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp8CC,_tmp8CB,e->loc);});Cyc_Absyn_stmt_exp(_tmp8CD,e->loc);});
decls=decls->tl;}
# 2361
e->topt=0;
e->r=fn_exp->r;}
# 2366
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2370
{void*_tmp4B0=e->r;void*_stmttmp54=_tmp4B0;void*_tmp4B1=_stmttmp54;void*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct _fat_ptr*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;int*_tmp4BD;struct Cyc_Absyn_Exp**_tmp4BC;void***_tmp4BB;struct Cyc_Absyn_Exp**_tmp4BA;int*_tmp4B9;struct Cyc_Absyn_Enumfield*_tmp4BF;void*_tmp4BE;struct Cyc_Absyn_Enumfield*_tmp4C1;struct Cyc_Absyn_Enumdecl*_tmp4C0;struct Cyc_Absyn_Datatypefield*_tmp4C4;struct Cyc_Absyn_Datatypedecl*_tmp4C3;struct Cyc_List_List*_tmp4C2;struct Cyc_List_List*_tmp4C6;void*_tmp4C5;struct Cyc_Absyn_Aggrdecl**_tmp4CA;struct Cyc_List_List*_tmp4C9;struct Cyc_List_List**_tmp4C8;struct _tuple1**_tmp4C7;int*_tmp4CD;void*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CB;int*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Stmt*_tmp4D2;struct Cyc_List_List*_tmp4D3;struct Cyc_List_List*_tmp4D5;struct _tuple9*_tmp4D4;struct Cyc_List_List*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;int*_tmp4DC;int*_tmp4DB;struct _fat_ptr*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;int*_tmp4E0;int*_tmp4DF;struct _fat_ptr*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_List_List*_tmp4E3;void*_tmp4E2;void*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp**_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;enum Cyc_Absyn_Coercion*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;void*_tmp4E9;struct Cyc_List_List*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Core_Opt*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4F8;enum Cyc_Absyn_Incrementor _tmp4FC;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_List_List*_tmp4FE;enum Cyc_Absyn_Primop _tmp4FD;void**_tmp4FF;union Cyc_Absyn_Cnst*_tmp500;struct Cyc_List_List*_tmp502;struct Cyc_Core_Opt*_tmp501;struct Cyc_Absyn_VarargCallInfo**_tmp505;struct Cyc_List_List*_tmp504;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp506;switch(*((int*)_tmp4B1)){case 12U: _LL1: _tmp506=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp506;
# 2375
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B1)->f4 == 0){_LL3: _LL4:
# 2379
({struct Cyc_Warn_String_Warn_Warg_struct _tmp508=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({struct _fat_ptr _tmp8CE=({const char*_tmp509="unresolved function: tcExpNoInst";_tag_fat(_tmp509,sizeof(char),33U);});_tmp63D.f1=_tmp8CE;});_tmp63D;});void*_tmp507[1U];_tmp507[0]=& _tmp508;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp507,sizeof(void*),1U));});}else{_LL1B: _tmp503=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp504=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp505=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp503;struct Cyc_List_List*es=_tmp504;struct Cyc_Absyn_VarargCallInfo**vci=_tmp505;
# 2402
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp50A;_push_handler(& _tmp50A);{int _tmp50C=0;if(setjmp(_tmp50A.handler))_tmp50C=1;if(!_tmp50C){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp50B=(void*)Cyc_Core_get_exn_thrown();void*_tmp50D=_tmp50B;void*_tmp50E;if(((struct Cyc_Core_Failure_exn_struct*)_tmp50D)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2407
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp50E=_tmp50D;_LL5D: {void*exn=_tmp50E;(int)_rethrow(exn);}}_LL59:;}}}
# 2411
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2418
goto _LL0;}}case 36U: _LL5: _tmp501=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp502=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp501;struct Cyc_List_List*des=_tmp502;
# 2383
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp500=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp500;
# 2387
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp4FF=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LLA: {void**b=_tmp4FF;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp4FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4FE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp4FD;struct Cyc_List_List*es=_tmp4FE;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp4FB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4FC=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp4FB;enum Cyc_Absyn_Incrementor i=_tmp4FC;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp4F8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4F9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4FA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp4F8;struct Cyc_Core_Opt*popt=_tmp4F9;struct Cyc_Absyn_Exp*e2=_tmp4FA;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp4F5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4F6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4F7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp4F5;struct Cyc_Absyn_Exp*e2=_tmp4F6;struct Cyc_Absyn_Exp*e3=_tmp4F7;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp4F3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4F4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp4F3;struct Cyc_Absyn_Exp*e2=_tmp4F4;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp4F1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4F2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp4F1;struct Cyc_Absyn_Exp*e2=_tmp4F2;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp4EF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4F0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp4EF;struct Cyc_Absyn_Exp*e2=_tmp4F0;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp4EE=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp4EE;
# 2419
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp4EC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4ED=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp4EC;struct Cyc_List_List*ts=_tmp4ED;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp4E9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4EA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4EB=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B1)->f4;_LL22: {void*t1=_tmp4E9;struct Cyc_Absyn_Exp*e1=_tmp4EA;enum Cyc_Absyn_Coercion*c=_tmp4EB;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp4E8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp4E8;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp4E6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4E7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp4E6;struct Cyc_Absyn_Exp*e1=_tmp4E7;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp4E5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp4E5;
_tmp4E4=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17U: _LL29: _tmp4E4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL2A: {void*t1=_tmp4E4;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp4E2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4E3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL2C: {void*t1=_tmp4E2;struct Cyc_List_List*l=_tmp4E3;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp4E1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp4E1;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4DD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4DE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4DF=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_tmp4E0=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B1)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4DD;struct _fat_ptr*f=_tmp4DE;int*is_tagged=_tmp4DF;int*is_read=_tmp4E0;
# 2429
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4D9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4DA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4DB=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_tmp4DC=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B1)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4D9;struct _fat_ptr*f=_tmp4DA;int*is_tagged=_tmp4DB;int*is_read=_tmp4DC;
# 2431
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4D7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4D8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4D7;struct Cyc_Absyn_Exp*e2=_tmp4D8;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4D6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL36: {struct Cyc_List_List*es=_tmp4D6;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4D4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4D5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL38: {struct _tuple9*t1=_tmp4D4;struct Cyc_List_List*des=_tmp4D5;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4D3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4D3;
# 2438
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp50F=Cyc_Tcutil_compress(*topt);void*_stmttmp55=_tmp50F;void*_tmp510=_stmttmp55;void*_tmp513;struct Cyc_Absyn_Tqual*_tmp512;void**_tmp511;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->tag == 4U){_LL5F: _tmp511=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->f1).elt_type;_tmp512=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->f1).tq;_tmp513=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->f1).zero_term;_LL60: {void**et=_tmp511;struct Cyc_Absyn_Tqual*etq=_tmp512;void*zt=_tmp513;
# 2444
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2450
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4D2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4D2;
# 2452
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4CE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4CF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4D0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_tmp4D1=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B1)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4CE;struct Cyc_Absyn_Exp*e1=_tmp4CF;struct Cyc_Absyn_Exp*e2=_tmp4D0;int*iszeroterm=_tmp4D1;
# 2454
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4CB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4CC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4CD=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4CB;void*t1=_tmp4CC;int*iszeroterm=_tmp4CD;
# 2456
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4C7=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4C8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4C9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_tmp4CA=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B1)->f4;_LL42: {struct _tuple1**tn=_tmp4C7;struct Cyc_List_List**ts=_tmp4C8;struct Cyc_List_List*args=_tmp4C9;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4CA;
# 2458
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4C5=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4C6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL44: {void*ts=_tmp4C5;struct Cyc_List_List*args=_tmp4C6;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4C2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4C3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_tmp4C4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B1)->f3;_LL46: {struct Cyc_List_List*es=_tmp4C2;struct Cyc_Absyn_Datatypedecl*tud=_tmp4C3;struct Cyc_Absyn_Datatypefield*tuf=_tmp4C4;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4C0=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4C1=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4C0;struct Cyc_Absyn_Enumfield*ef=_tmp4C1;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4BE=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4BF=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL4A: {void*t2=_tmp4BE;struct Cyc_Absyn_Enumfield*ef=_tmp4BF;
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4B9=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1).is_calloc;_tmp4BA=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1).rgn;_tmp4BB=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1).elt_type;_tmp4BC=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1).num_elts;_tmp4BD=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4B9;struct Cyc_Absyn_Exp**ropt=_tmp4BA;void***t2=_tmp4BB;struct Cyc_Absyn_Exp**e2=_tmp4BC;int*isfat=_tmp4BD;
# 2464
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp4B7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4B8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp4B7;struct Cyc_Absyn_Exp*e2=_tmp4B8;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp4B5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_tmp4B6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B1)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp4B5;struct _fat_ptr*f=_tmp4B6;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp4B4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp4B4;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp4B3=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp4B3;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp4B2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4B1)->f1;_LL56: {void*t2=_tmp4B2;
# 2470
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp515=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0U,({struct _fat_ptr _tmp8CF=({const char*_tmp516="valueof(-) can only occur within types";_tag_fat(_tmp516,sizeof(char),39U);});_tmp63E.f1=_tmp8CF;});_tmp63E;});void*_tmp514[1U];_tmp514[0]=& _tmp515;({unsigned _tmp8D0=e->loc;Cyc_Warn_err2(_tmp8D0,_tag_fat(_tmp514,sizeof(void*),1U));});});
# 2478
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2482
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2484
e->topt=t;}
