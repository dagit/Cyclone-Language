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
extern void*Cyc_Core_snd(struct _tuple0*);
# 126
extern int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 280 "core.h"
extern void Cyc_Core_rethrow(void*);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "/tmp/cyclone/include/list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*x);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 37 "/tmp/cyclone/include/position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900 "/tmp/cyclone/src/absyn.h"
void*Cyc_Absyn_compress(void*);
# 932
extern void*Cyc_Absyn_void_type;
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
# 1137
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1139
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 1151
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1153
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1155
int Cyc_Absyn_is_require_union_type(void*);struct Cyc___cycFILE;
# 53 "/tmp/cyclone/include/cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "/tmp/cyclone/src/relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);extern union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 100
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 103
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 108
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 112
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 121
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_RgnOrder_RgnPO;
# 30 "/tmp/cyclone/src/tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 51
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 53
int Cyc_Tcutil_is_bound_one(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 181
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 187
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 196
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 223
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);
# 230
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 240
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 271
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 274
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 25 "/tmp/cyclone/src/attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);struct _tuple14{unsigned f1;int f2;};
# 28 "/tmp/cyclone/src/evexp.h"
extern struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "/tmp/cyclone/include/dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "/tmp/cyclone/include/dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
extern void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
# 39 "/tmp/cyclone/include/hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 41 "/tmp/cyclone/src/cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void);struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "/tmp/cyclone/src/cf_flowinfo.h"
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 153
extern struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void);
# 155
extern int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f);
extern int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f);
# 158
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 161
extern struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
extern void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
extern void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
extern int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
extern void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple16{void*f1;struct Cyc_List_List*f2;};
extern struct _tuple16 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
extern enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 171
extern int Cyc_CfFlowInfo_is_init_pointer(void*r);
extern int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
extern void Cyc_CfFlowInfo_print_absrval(void*rval);
# 181
extern void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "/tmp/cyclone/src/cf_flowinfo.h"
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
extern struct _tuple17 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple17 pr1,struct _tuple17 pr2);
# 32 "/tmp/cyclone/src/new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*);
# 36
extern int Cyc_NewControlFlow_warn_lose_unique;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "/tmp/cyclone/src/tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 112 "/tmp/cyclone/src/tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);
# 29 "/tmp/cyclone/src/warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "/tmp/cyclone/src/absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 52 "/tmp/cyclone/src/new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_fat_size(s1,sizeof(char));
unsigned sz2=_get_fat_size(s2,sizeof(char));
unsigned minsz=sz1 < sz2?sz1: sz2;
# 60
while((unsigned)i < minsz){
char c1=*((const char*)_check_fat_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_fat_subscript(s2,sizeof(char),i));
if((int)c1 == (int)'\000'){
if((int)c2 == (int)'\000')return 0;else{
return - 1;}}else{
if((int)c2 == (int)'\000')return 1;else{
# 68
int diff=(int)c1 - (int)c2;
if(diff != 0)return diff;}}
# 71
++ i;}
# 73
if(sz1 == sz2)return 0;
if(minsz < sz2){
if((int)*((const char*)_check_fat_subscript(s2,sizeof(char),i))== (int)'\000')return 0;else{
return - 1;}}else{
# 78
if((int)*((const char*)_check_fat_subscript(s1,sizeof(char),i))== (int)'\000')return 0;else{
return 1;}}}}
# 84
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple18{void*f1;int f2;};
# 126 "/tmp/cyclone/src/new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple18*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple17 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
# 132
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 136
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp2;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp2=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2: {struct Cyc_NewControlFlow_CFStmtAnnot*x=_tmp2;
return x;}}else{_LL3: _LL4:
({void*_tmp639=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=Cyc_NewControlFlow_CFAnnot,(_tmp3->f1).visited=0;_tmp3;});s->annot=_tmp639;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 143
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp63A=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp63A;});_tmp4;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 150
return*sflow;}
# 153
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp5=f;struct Cyc_List_List*_tmp6;if((_tmp5.BottomFL).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _tmp6=((_tmp5.ReachableFL).val).f2;_LL4: {struct Cyc_List_List*r=_tmp6;
return r;}}_LL0:;}struct _tuple21{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 160
static struct _tuple21 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp7;
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp8;
# 164
({union Cyc_CfFlowInfo_FlowInfo _tmp63B=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*sflow);*sflow=_tmp63B;});
# 170
annot->visited=env->iteration_num;
return({struct _tuple21 _tmp597;_tmp597.f1=annot,_tmp597.f2=sflow;_tmp597;});}
# 180
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 188
({union Cyc_CfFlowInfo_FlowInfo _tmp63C=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp63C;});}struct _tuple22{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned f2;struct Cyc_Dict_Dict f3;};
# 195
static void Cyc_NewControlFlow_check_unique_root(struct _tuple22*ckenv,void*root,void*rval){
# 197
struct _tuple22*_tmp9=ckenv;struct Cyc_Dict_Dict _tmpC;unsigned _tmpB;struct Cyc_NewControlFlow_AnalEnv*_tmpA;_LL1: _tmpA=_tmp9->f1;_tmpB=_tmp9->f2;_tmpC=_tmp9->f3;_LL2: {struct Cyc_NewControlFlow_AnalEnv*env=_tmpA;unsigned loc=_tmpB;struct Cyc_Dict_Dict new_fd=_tmpC;
void*_tmpD=root;struct Cyc_Absyn_Vardecl*_tmpE;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->tag == 0U){_LL4: _tmpE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->f1;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmpE;
# 200
if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(new_fd,root,& rval)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
retry: {void*_tmpF=rval;void*_tmp10;switch(*((int*)_tmpF)){case 8U: _LL9: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF)->f2;_LLA: {void*r=_tmp10;
rval=r;goto retry;}case 7U: _LLB: _LLC:
 goto _LLE;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpF)->f1 == Cyc_CfFlowInfo_NoneIL){_LLD: _LLE:
 goto _LL10;}else{goto _LL11;}case 0U: _LLF: _LL10:
 goto _LL8;default: _LL11: _LL12:
# 209
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp598;_tmp598.tag=0U,({struct _fat_ptr _tmp63D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp598.f1=_tmp63D;});_tmp598;});void*_tmp11[1U];_tmp11[0]=& _tmp13;({unsigned _tmp63F=loc;struct _fat_ptr _tmp63E=({const char*_tmp12="alias-free pointer(s) reachable from %s may become unreachable.";_tag_fat(_tmp12,sizeof(char),64U);});Cyc_Warn_warn(_tmp63F,_tmp63E,_tag_fat(_tmp11,sizeof(void*),1U));});});
goto _LL8;}_LL8:;}}
# 213
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}}
# 221
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp14=Cyc_NewControlFlow_get_stmt_annot(s);struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp14;
union Cyc_CfFlowInfo_FlowInfo*_tmp15=Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp15;
union Cyc_CfFlowInfo_FlowInfo _tmp16=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*sflow);union Cyc_CfFlowInfo_FlowInfo new_flow=_tmp16;
# 227
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple20 _tmp17=({struct _tuple20 _tmp59A;_tmp59A.f1=flow,_tmp59A.f2=new_flow;_tmp59A;});struct _tuple20 _stmttmp1=_tmp17;struct _tuple20 _tmp18=_stmttmp1;struct Cyc_Dict_Dict _tmp1A;struct Cyc_Dict_Dict _tmp19;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_LL1: _tmp19=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;_LL2: {struct Cyc_Dict_Dict fd=_tmp19;struct Cyc_Dict_Dict new_fd=_tmp1A;
# 230
struct _tuple22 _tmp1B=({struct _tuple22 _tmp599;_tmp599.f1=env,_tmp599.f2=s->loc,_tmp599.f3=new_fd;_tmp599;});struct _tuple22 ckenv=_tmp1B;
((void(*)(void(*f)(struct _tuple22*,void*,void*),struct _tuple22*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& ckenv,fd);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 236
if(!Cyc_CfFlowInfo_flow_lessthan_approx(new_flow,*sflow)){
# 243
*sflow=new_flow;
# 247
if(annot->visited == env->iteration_num)
# 249
env->iterate_again=1;}}
# 254
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned loc,int nameit){
# 258
union Cyc_CfFlowInfo_FlowInfo _tmp1C=inflow;struct Cyc_List_List*_tmp1E;struct Cyc_Dict_Dict _tmp1D;if((_tmp1C.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp1D=((_tmp1C.ReachableFL).val).f1;_tmp1E=((_tmp1C.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d=_tmp1D;struct Cyc_List_List*relns=_tmp1E;
# 261
for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp1F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=0U,_tmp22->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp22;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp1F;
void*_tmp20=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);void*rval=_tmp20;
if(nameit)
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=8U,_tmp21->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp21->f2=rval;_tmp21;});
# 268
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,(void*)root,rval);}
# 270
return Cyc_CfFlowInfo_ReachableFL(d,relns);}}_LL0:;}
# 274
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 281
for(0;(unsigned)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp23=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);struct Cyc_Relations_Reln*neg_rln=_tmp23;
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->hd=neg_rln,_tmp24->tl=assume;_tmp24;})))
return 0;}
# 286
return 1;}
# 289
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp25=e->r;void*_stmttmp2=_tmp25;void*_tmp26=_stmttmp2;struct Cyc_Absyn_Exp*_tmp27;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->tag == 14U){_LL1: _tmp27=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp27;
return e1;}}else{_LL3: _LL4:
 return e;}_LL0:;}
# 296
static void Cyc_NewControlFlow_check_union_requires(unsigned loc,void*t,struct _fat_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 298
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_List_List*_tmp29;if((_tmp28.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp29=((_tmp28.ReachableFL).val).f2;_LL4: {struct Cyc_List_List*relns=_tmp29;
# 301
{void*_tmp2A=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp2A;void*_tmp2B=_stmttmp3;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2E;union Cyc_Absyn_AggrInfo _tmp2D;switch(*((int*)_tmp2B)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->tag == 20U){_LL6: _tmp2D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_tmp2E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f2;_LL7: {union Cyc_Absyn_AggrInfo ainfo=_tmp2D;struct Cyc_List_List*ts=_tmp2E;
# 303
struct Cyc_Absyn_Aggrdecl*_tmp2F=Cyc_Absyn_get_known_aggrdecl(ainfo);struct Cyc_Absyn_Aggrdecl*ad=_tmp2F;
struct Cyc_Absyn_Aggrfield*_tmp30=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*field=_tmp30;
struct Cyc_Absyn_Exp*_tmp31=((struct Cyc_Absyn_Aggrfield*)_check_null(field))->requires_clause;struct Cyc_Absyn_Exp*req=_tmp31;
if(req != 0){
struct _RegionHandle _tmp32=_new_region("temp");struct _RegionHandle*temp=& _tmp32;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp33=({struct _RegionHandle*_tmp641=temp;struct Cyc_List_List*_tmp640=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,ts);Cyc_Tcutil_rsubsexp(_tmp641,_tmp640,req);});struct Cyc_Absyn_Exp*ireq=_tmp33;
# 310
if(!({struct Cyc_List_List*_tmp642=relns;Cyc_NewControlFlow_relns_ok(_tmp642,Cyc_Relations_exp2relns(temp,ireq));})){
({struct Cyc_String_pa_PrintArg_struct _tmp36=({struct Cyc_String_pa_PrintArg_struct _tmp59C;_tmp59C.tag=0U,({struct _fat_ptr _tmp643=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(ireq)));_tmp59C.f1=_tmp643;});_tmp59C;});struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0U,_tmp59B.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp59B;});void*_tmp34[2U];_tmp34[0]=& _tmp36,_tmp34[1]=& _tmp37;({unsigned _tmp645=loc;struct _fat_ptr _tmp644=({const char*_tmp35="unable to prove %s, required to access %s";_tag_fat(_tmp35,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp645,_tmp644,_tag_fat(_tmp34,sizeof(void*),2U));});});
({void*_tmp38=0U;({struct Cyc___cycFILE*_tmp647=Cyc_stderr;struct _fat_ptr _tmp646=({const char*_tmp39="  [recorded facts on path: ";_tag_fat(_tmp39,sizeof(char),28U);});Cyc_fprintf(_tmp647,_tmp646,_tag_fat(_tmp38,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,relns);
({void*_tmp3A=0U;({struct Cyc___cycFILE*_tmp649=Cyc_stderr;struct _fat_ptr _tmp648=({const char*_tmp3B="]\n";_tag_fat(_tmp3B,sizeof(char),3U);});Cyc_fprintf(_tmp649,_tmp648,_tag_fat(_tmp3A,sizeof(void*),0U));});});}}
# 308
;_pop_region();}
# 317
goto _LL5;}}else{goto _LLA;}case 7U: _LL8: _tmp2C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {struct Cyc_List_List*fs=_tmp2C;
# 319
struct Cyc_Absyn_Aggrfield*_tmp3C=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp3C;
struct Cyc_Absyn_Exp*_tmp3D=((struct Cyc_Absyn_Aggrfield*)_check_null(field))->requires_clause;struct Cyc_Absyn_Exp*req=_tmp3D;
if(req != 0){
struct _RegionHandle _tmp3E=_new_region("temp");struct _RegionHandle*temp=& _tmp3E;_push_region(temp);
if(!({struct Cyc_List_List*_tmp64A=relns;Cyc_NewControlFlow_relns_ok(_tmp64A,Cyc_Relations_exp2relns(temp,req));})){
({struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp59E;_tmp59E.tag=0U,({struct _fat_ptr _tmp64B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(req)));_tmp59E.f1=_tmp64B;});_tmp59E;});struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp59D;_tmp59D.tag=0U,_tmp59D.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp59D;});void*_tmp3F[2U];_tmp3F[0]=& _tmp41,_tmp3F[1]=& _tmp42;({unsigned _tmp64D=loc;struct _fat_ptr _tmp64C=({const char*_tmp40="unable to prove %s, required to access %s";_tag_fat(_tmp40,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp64D,_tmp64C,_tag_fat(_tmp3F,sizeof(void*),2U));});});
({void*_tmp43=0U;({struct Cyc___cycFILE*_tmp64F=Cyc_stderr;struct _fat_ptr _tmp64E=({const char*_tmp44="  [recorded facts on path: ";_tag_fat(_tmp44,sizeof(char),28U);});Cyc_fprintf(_tmp64F,_tmp64E,_tag_fat(_tmp43,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,relns);
({void*_tmp45=0U;({struct Cyc___cycFILE*_tmp651=Cyc_stderr;struct _fat_ptr _tmp650=({const char*_tmp46="]\n";_tag_fat(_tmp46,sizeof(char),3U);});Cyc_fprintf(_tmp651,_tmp650,_tag_fat(_tmp45,sizeof(void*),0U));});});}
# 323
;_pop_region();}
# 330
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 333
goto _LL0;}}_LL0:;}
# 337
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp47=inflow;struct Cyc_List_List*_tmp49;struct Cyc_Dict_Dict _tmp48;if((_tmp47.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp48=((_tmp47.ReachableFL).val).f1;_tmp49=((_tmp47.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d=_tmp48;struct Cyc_List_List*relns=_tmp49;
# 341
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp4A=0U;({unsigned _tmp653=loc;struct _fat_ptr _tmp652=({const char*_tmp4B="expression may not be fully initialized";_tag_fat(_tmp4B,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp653,_tmp652,_tag_fat(_tmp4A,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp4C=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);struct Cyc_Dict_Dict ans_d=_tmp4C;
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);union Cyc_CfFlowInfo_FlowInfo ans=_tmp4D;
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}}_LL0:;}struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 351
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){
struct _tuple0 _tmp4E=({struct _tuple0 _tmp59F;({void*_tmp654=Cyc_Absyn_compress(t);_tmp59F.f1=_tmp654;}),_tmp59F.f2=r;_tmp59F;});struct _tuple0 _stmttmp4=_tmp4E;struct _tuple0 _tmp4F=_stmttmp4;struct _fat_ptr _tmp52;struct Cyc_List_List*_tmp51;enum Cyc_Absyn_AggrKind _tmp50;struct _fat_ptr _tmp54;struct Cyc_List_List*_tmp53;struct _fat_ptr _tmp57;struct Cyc_List_List*_tmp56;union Cyc_Absyn_AggrInfo _tmp55;struct _fat_ptr _tmp59;struct Cyc_Absyn_Datatypefield*_tmp58;void*_tmp5A;switch(*((int*)_tmp4F.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp4F.f1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 20U: goto _LL13;default: goto _LL13;}case 6U: goto _LL13;case 7U: goto _LL13;case 3U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp5A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL8: {void*r=_tmp5A;
Cyc_NewControlFlow_check_nounique(env,loc,t,r);return;}default: switch(*((int*)_tmp4F.f1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LL9: _tmp58=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLA: {struct Cyc_Absyn_Datatypefield*tuf=_tmp58;struct _fat_ptr ad=_tmp59;
# 358
if(tuf->typs == 0)
return;
_tmp53=tuf->typs;_tmp54=ad;goto _LLC;}}else{goto _LL13;}}else{goto _LL13;}case 20U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLD: _tmp55=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1;_tmp56=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp57=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLE: {union Cyc_Absyn_AggrInfo info=_tmp55;struct Cyc_List_List*targs=_tmp56;struct _fat_ptr d=_tmp57;
# 369
struct Cyc_Absyn_Aggrdecl*_tmp5B=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5B;
if(ad->impl == 0)return;{
struct Cyc_List_List*_tmp5C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp5C;
struct _RegionHandle _tmp5D=_new_region("temp");struct _RegionHandle*temp=& _tmp5D;_push_region(temp);
{struct Cyc_List_List*_tmp5E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,targs);struct Cyc_List_List*inst=_tmp5E;
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd)->type;
if(inst != 0)t=Cyc_Tcutil_rsubstitute(temp,inst,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)d.curr)[i]);}}
# 379
_npop_handler(0U);return;}
# 373
;_pop_region();}}}else{goto _LL13;}default: goto _LL13;}case 6U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLB: _tmp53=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp54=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLC: {struct Cyc_List_List*tqts=_tmp53;struct _fat_ptr ad=_tmp54;
# 362
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
{int i=0;for(0;(unsigned)i < sz;(i ++,tqts=tqts->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp657=env;unsigned _tmp656=loc;void*_tmp655=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp657,_tmp656,_tmp655,*((void**)_check_fat_subscript(ad,sizeof(void*),i)));});}}
# 366
return;}}else{goto _LL13;}case 7U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLF: _tmp50=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp51=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp52=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k=_tmp50;struct Cyc_List_List*fs=_tmp51;struct _fat_ptr d=_tmp52;
# 382
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd)->type,((void**)d.curr)[i]);}}
return;}}else{goto _LL13;}case 3U: _LL11: _LL12:
# 386
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp5F=0U;({unsigned _tmp659=loc;struct _fat_ptr _tmp658=({const char*_tmp60="argument may still contain alias-free pointers";_tag_fat(_tmp60,sizeof(char),47U);});Cyc_Warn_warn(_tmp659,_tmp658,_tag_fat(_tmp5F,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 393
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp61=inflow;struct Cyc_List_List*_tmp63;struct Cyc_Dict_Dict _tmp62;if((_tmp61.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp62=((_tmp61.ReachableFL).val).f1;_tmp63=((_tmp61.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d=_tmp62;struct Cyc_List_List*relns=_tmp63;
# 397
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp64=0U;({struct _fat_ptr _tmp65A=({const char*_tmp65="noliveunique attribute requires alias-free pointer";_tag_fat(_tmp65,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp65A,_tag_fat(_tmp64,sizeof(void*),0U));});});{
void*_tmp66=Cyc_Tcutil_pointer_elt_type(t);void*elt_type=_tmp66;
retry: {void*_tmp67=r;void*_tmp68;struct Cyc_CfFlowInfo_Place*_tmp69;void*_tmp6A;switch(*((int*)_tmp67)){case 8U: _LL6: _tmp6A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp67)->f2;_LL7: {void*r2=_tmp6A;
r=r2;goto retry;}case 4U: _LL8: _tmp69=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp67)->f1;_LL9: {struct Cyc_CfFlowInfo_Place*p=_tmp69;
_tmp68=Cyc_CfFlowInfo_lookup_place(d,p);goto _LLB;}case 5U: _LLA: _tmp68=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp67)->f1;_LLB: {void*r=_tmp68;
Cyc_NewControlFlow_check_nounique(env,loc,elt_type,r);goto _LL5;}default: _LLC: _LLD:
# 405
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(elt_type))
({void*_tmp6B=0U;({unsigned _tmp65C=loc;struct _fat_ptr _tmp65B=({const char*_tmp6C="argument may contain live alias-free pointers";_tag_fat(_tmp6C,sizeof(char),46U);});Cyc_Warn_warn(_tmp65C,_tmp65B,_tag_fat(_tmp6B,sizeof(void*),0U));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 409
struct Cyc_Dict_Dict _tmp6D=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);struct Cyc_Dict_Dict ans_d=_tmp6D;
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp6E=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);union Cyc_CfFlowInfo_FlowInfo ans=_tmp6E;
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}}}_LL0:;}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 419
static struct _tuple24 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 424
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple24 _tmp5A0;_tmp5A0.f1=inflow,_tmp5A0.f2=0;_tmp5A0;});
do{
struct _tuple17 _tmp6F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple17 _stmttmp5=_tmp6F;struct _tuple17 _tmp70=_stmttmp5;void*_tmp72;union Cyc_CfFlowInfo_FlowInfo _tmp71;_LL1: _tmp71=_tmp70.f1;_tmp72=_tmp70.f2;_LL2: {union Cyc_CfFlowInfo_FlowInfo f=_tmp71;void*r=_tmp72;
inflow=f;
rvals=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=r,_tmp73->tl=rvals;_tmp73;});
es=es->tl;
first_is_copy=others_are_copy;}}while(es != 0);
# 435
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple24 _tmp5A1;_tmp5A1.f1=inflow,({struct Cyc_List_List*_tmp65D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp5A1.f2=_tmp65D;});_tmp5A1;});}
# 441
static struct _tuple17 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 443
struct _tuple24 _tmp74=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 443
struct _tuple24 _stmttmp6=_tmp74;struct _tuple24 _tmp75=_stmttmp6;struct Cyc_List_List*_tmp77;union Cyc_CfFlowInfo_FlowInfo _tmp76;_LL1: _tmp76=_tmp75.f1;_tmp77=_tmp75.f2;_LL2: {union Cyc_CfFlowInfo_FlowInfo afterflow=_tmp76;struct Cyc_List_List*rvals=_tmp77;
# 445
{union Cyc_CfFlowInfo_FlowInfo _tmp78=afterflow;struct Cyc_Dict_Dict _tmp79;if((_tmp78.ReachableFL).tag == 2){_LL4: _tmp79=((_tmp78.ReachableFL).val).f1;_LL5: {struct Cyc_Dict_Dict d=_tmp79;
# 447
for(0;rvals != 0;(rvals=rvals->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,(void*)rvals->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp7A=0U;({unsigned _tmp65F=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp65E=({const char*_tmp7B="expression may not be initialized";_tag_fat(_tmp7B,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65F,_tmp65E,_tag_fat(_tmp7A,sizeof(void*),0U));});});}
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 453
return({struct _tuple17 _tmp5A2;_tmp5A2.f1=afterflow,_tmp5A2.f2=(env->fenv)->unknown_all;_tmp5A2;});}}
# 460
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 467
int needs_unconsume=0;
void*_tmp7C=Cyc_CfFlowInfo_lookup_place(new_dict,p);void*old_rval=_tmp7C;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume))
({void*_tmp7D=0U;({struct _fat_ptr _tmp660=({const char*_tmp7E="consume_zero_rval";_tag_fat(_tmp7E,sizeof(char),18U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp660,_tag_fat(_tmp7D,sizeof(void*),0U));});});else{
# 472
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 476
return new_rval;}}}
# 489 "/tmp/cyclone/src/new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 494
union Cyc_CfFlowInfo_FlowInfo _tmp7F=outflow;struct Cyc_List_List*_tmp81;struct Cyc_Dict_Dict _tmp80;if((_tmp7F.BottomFL).tag == 1){_LL1: _LL2:
 return outflow;}else{_LL3: _tmp80=((_tmp7F.ReachableFL).val).f1;_tmp81=((_tmp7F.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d=_tmp80;struct Cyc_List_List*relns=_tmp81;
# 497
union Cyc_CfFlowInfo_AbsLVal _tmp82=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _stmttmp7=_tmp82;union Cyc_CfFlowInfo_AbsLVal _tmp83=_stmttmp7;struct Cyc_CfFlowInfo_Place*_tmp84;if((_tmp83.UnknownL).tag == 2){_LL6: _LL7:
# 501
 return outflow;}else{_LL8: _tmp84=(_tmp83.PlaceL).val;_LL9: {struct Cyc_CfFlowInfo_Place*p=_tmp84;
# 505
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=8U,_tmp85->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp85->f2=nzval;_tmp85;});}
# 509
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmp86=({struct Cyc_Dict_Dict _tmp661=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp661,relns);});union Cyc_CfFlowInfo_FlowInfo outflow=_tmp86;
# 514
return outflow;}}}_LL5:;}}_LL0:;}
# 523
static struct _tuple20 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 525
union Cyc_CfFlowInfo_FlowInfo _tmp87=outflow;struct Cyc_List_List*_tmp89;struct Cyc_Dict_Dict _tmp88;if((_tmp87.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple20 _tmp5A3;_tmp5A3.f1=outflow,_tmp5A3.f2=outflow;_tmp5A3;});}else{_LL3: _tmp88=((_tmp87.ReachableFL).val).f1;_tmp89=((_tmp87.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d=_tmp88;struct Cyc_List_List*relns=_tmp89;
# 528
union Cyc_CfFlowInfo_AbsLVal _tmp8A=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _stmttmp8=_tmp8A;union Cyc_CfFlowInfo_AbsLVal _tmp8B=_stmttmp8;struct Cyc_CfFlowInfo_Place*_tmp8C;if((_tmp8B.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple20 _tmp5A4;_tmp5A4.f1=outflow,_tmp5A4.f2=outflow;_tmp5A4;});}else{_LL8: _tmp8C=(_tmp8B.PlaceL).val;_LL9: {struct Cyc_CfFlowInfo_Place*p=_tmp8C;
# 531
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=8U,_tmp8D->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8D->f2=nzval;_tmp8D;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=8U,_tmp8E->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8E->f2=zval;_tmp8E;});}
# 537
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,zval);
return({struct _tuple20 _tmp5A5;({
union Cyc_CfFlowInfo_FlowInfo _tmp665=({struct Cyc_Dict_Dict _tmp664=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp664,relns);});_tmp5A5.f1=_tmp665;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp663=({struct Cyc_Dict_Dict _tmp662=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,zval);Cyc_CfFlowInfo_ReachableFL(_tmp662,relns);});_tmp5A5.f2=_tmp663;});_tmp5A5;});}}_LL5:;}}_LL0:;}
# 546
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 549
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
# 552
{void*_tmp8F=e->annot;void*_stmttmp9=_tmp8F;void*_tmp90=_stmttmp9;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp92;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp90)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp92=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp90)->f1;_LL2: {struct Cyc_List_List*relns2=_tmp92;
# 554
if(possibly_null && Cyc_Relations_same_relns(relns,relns2))return;
goto _LL0;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp90)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp91=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp90)->f1;_LL4: {struct Cyc_List_List*relns2=_tmp91;
# 557
if(!possibly_null && Cyc_Relations_same_relns(relns,relns2))return;
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 561
if(possibly_null)
({void*_tmp666=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=Cyc_CfFlowInfo_UnknownZ,_tmp93->f1=relns;_tmp93;});e->annot=_tmp666;});else{
# 564
({void*_tmp667=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=Cyc_CfFlowInfo_NotZero,_tmp94->f1=relns;_tmp94;});e->annot=_tmp667;});}}
# 572
static struct _tuple17 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){
# 576
void*_tmp95=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpA=_tmp95;void*_tmp96=_stmttmpA;void*_tmp98;void*_tmp97;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->tag == 3U){_LL1: _tmp97=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->f1).elt_type;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->f1).ptr_atts).bounds;_LL2: {void*elttype=_tmp97;void*bd=_tmp98;
# 578
union Cyc_CfFlowInfo_FlowInfo _tmp99=f;struct Cyc_List_List*_tmp9B;struct Cyc_Dict_Dict _tmp9A;if((_tmp99.BottomFL).tag == 1){_LL6: _LL7:
# 580
 return({struct _tuple17 _tmp5A6;_tmp5A6.f1=f,({void*_tmp668=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp5A6.f2=_tmp668;});_tmp5A6;});}else{_LL8: _tmp9A=((_tmp99.ReachableFL).val).f1;_tmp9B=((_tmp99.ReachableFL).val).f2;_LL9: {struct Cyc_Dict_Dict outdict=_tmp9A;struct Cyc_List_List*relns=_tmp9B;
# 582
struct _tuple16 _tmp9C=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmpB=_tmp9C;struct _tuple16 _tmp9D=_stmttmpB;struct Cyc_List_List*_tmp9F;void*_tmp9E;_LLB: _tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;_LLC: {void*r=_tmp9E;struct Cyc_List_List*names=_tmp9F;
{void*_tmpA0=r;enum Cyc_CfFlowInfo_InitLevel _tmpA1;void*_tmpA2;struct Cyc_CfFlowInfo_Place*_tmpA3;void*_tmpA5;struct Cyc_Absyn_Vardecl*_tmpA4;switch(*((int*)_tmpA0)){case 8U: _LLE: _tmpA4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_tmpA5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f2;_LLF: {struct Cyc_Absyn_Vardecl*n=_tmpA4;void*r2=_tmpA5;
# 585
({void*_tmpA6=0U;({struct _fat_ptr _tmp669=({const char*_tmpA7="named location in anal_derefR";_tag_fat(_tmpA7,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp669,_tag_fat(_tmpA6,sizeof(void*),0U));});});}case 1U: _LL10: _LL11:
# 587
 Cyc_NewControlFlow_update_relns(e,0,relns);
goto _LLD;case 4U: _LL12: _tmpA3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_LL13: {struct Cyc_CfFlowInfo_Place*p=_tmpA3;
# 590
Cyc_NewControlFlow_update_relns(e,0,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _tmp5A7;_tmp5A7.f1=f,({void*_tmp66A=Cyc_CfFlowInfo_lookup_place(outdict,p);_tmp5A7.f2=_tmp66A;});_tmp5A7;});
goto _LLD;}case 5U: _LL14: _tmpA2=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_LL15: {void*r=_tmpA2;
# 596
Cyc_NewControlFlow_update_relns(e,1,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _tmp5A8;_tmp5A8.f1=f,_tmp5A8.f2=r;_tmp5A8;});
goto _LLD;}case 0U: _LL16: _LL17:
# 601
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple17 _tmp5A9;({union Cyc_CfFlowInfo_FlowInfo _tmp66C=Cyc_CfFlowInfo_BottomFL();_tmp5A9.f1=_tmp66C;}),({void*_tmp66B=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp5A9.f2=_tmp66B;});_tmp5A9;});case 2U: _LL18: _tmpA1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_LL19: {enum Cyc_CfFlowInfo_InitLevel il=_tmpA1;
# 605
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
goto _LL1B;}default: _LL1A: _LL1B:
# 608
 Cyc_NewControlFlow_update_relns(e,1,relns);
goto _LLD;}_LLD:;}{
# 611
enum Cyc_CfFlowInfo_InitLevel _tmpA8=Cyc_CfFlowInfo_initlevel(env->fenv,outdict,r);enum Cyc_CfFlowInfo_InitLevel _stmttmpC=_tmpA8;enum Cyc_CfFlowInfo_InitLevel _tmpA9=_stmttmpC;if(_tmpA9 == Cyc_CfFlowInfo_NoneIL){_LL1D: _LL1E: {
# 613
struct _tuple16 _tmpAA=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmpD=_tmpAA;struct _tuple16 _tmpAB=_stmttmpD;void*_tmpAC;_LL22: _tmpAC=_tmpAB.f1;_LL23: {void*r=_tmpAC;
{void*_tmpAD=r;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpAD)->tag == 7U){_LL25: _LL26:
# 616
({void*_tmpAE=0U;({unsigned _tmp66E=e->loc;struct _fat_ptr _tmp66D=({const char*_tmpAF="attempt to dereference a consumed alias-free pointer";_tag_fat(_tmpAF,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp66E,_tmp66D,_tag_fat(_tmpAE,sizeof(void*),0U));});});
goto _LL24;}else{_LL27: _LL28:
# 619
({void*_tmpB0=0U;({unsigned _tmp670=e->loc;struct _fat_ptr _tmp66F=({const char*_tmpB1="dereference of possibly uninitialized pointer";_tag_fat(_tmpB1,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp670,_tmp66F,_tag_fat(_tmpB0,sizeof(void*),0U));});});}_LL24:;}
# 621
goto _LL20;}}}else{_LL1F: _LL20:
# 623
 return({struct _tuple17 _tmp5AA;_tmp5AA.f1=f,({void*_tmp671=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp5AA.f2=_tmp671;});_tmp5AA;});}_LL1C:;}}}}_LL5:;}}else{_LL3: _LL4:
# 626
({void*_tmpB2=0U;({struct _fat_ptr _tmp672=({const char*_tmpB3="right deref of non-pointer-type";_tag_fat(_tmpB3,sizeof(char),32U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp672,_tag_fat(_tmpB2,sizeof(void*),0U));});});}_LL0:;}
# 633
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 635
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 638
{void*_tmpB4=e1->r;void*_stmttmpE=_tmpB4;void*_tmpB5=_stmttmpE;void*_tmpB6;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB5)->tag == 1U){_LL1: _tmpB6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB5)->f1;_LL2: {void*b1=_tmpB6;
# 640
struct Cyc_Absyn_Vardecl*_tmpB7=Cyc_Tcutil_nonesc_vardecl(b1);struct Cyc_Absyn_Vardecl*x=_tmpB7;
if(x != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(x);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);}
# 646
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 650
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);}
# 656
return relns;}}
# 664
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned loc,void*old_rval){
# 669
struct _tuple19 _tmpB8=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple19 _stmttmpF=_tmpB8;struct _tuple19 _tmpB9=_stmttmpF;union Cyc_CfFlowInfo_AbsLVal _tmpBA;_LL1: _tmpBA=_tmpB9.f2;_LL2: {union Cyc_CfFlowInfo_AbsLVal lval=_tmpBA;
{struct _tuple19 _tmpBB=({struct _tuple19 _tmp5AC;_tmp5AC.f1=inflow,_tmp5AC.f2=lval;_tmp5AC;});struct _tuple19 _stmttmp10=_tmpBB;struct _tuple19 _tmpBC=_stmttmp10;struct Cyc_CfFlowInfo_Place*_tmpBF;struct Cyc_List_List*_tmpBE;struct Cyc_Dict_Dict _tmpBD;if(((_tmpBC.f1).ReachableFL).tag == 2){if(((_tmpBC.f2).PlaceL).tag == 1){_LL4: _tmpBD=(((_tmpBC.f1).ReachableFL).val).f1;_tmpBE=(((_tmpBC.f1).ReachableFL).val).f2;_tmpBF=((_tmpBC.f2).PlaceL).val;_LL5: {struct Cyc_Dict_Dict fd=_tmpBD;struct Cyc_List_List*relns=_tmpBE;struct Cyc_CfFlowInfo_Place*p=_tmpBF;
# 672
void*_tmpC0=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);void*new_rval=_tmpC0;
# 674
struct _tuple16 _tmpC1=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _stmttmp11=_tmpC1;struct _tuple16 _tmpC2=_stmttmp11;struct Cyc_List_List*_tmpC4;void*_tmpC3;_LLB: _tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_LLC: {void*old_rval=_tmpC3;struct Cyc_List_List*names=_tmpC4;
for(0;names != 0;names=names->tl){
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=8U,_tmpC5->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmpC5->f2=new_rval;_tmpC5;});}
# 679
fd=Cyc_CfFlowInfo_assign_place(env->fenv,loc,fd,p,new_rval);
inflow=Cyc_CfFlowInfo_ReachableFL(fd,relns);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 685
goto _LL3;}}}else{_LL8: _LL9:
# 688
({struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp5AB;_tmp5AB.tag=0U,({struct _fat_ptr _tmp673=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(exp));_tmp5AB.f1=_tmp673;});_tmp5AB;});void*_tmpC6[1U];_tmpC6[0]=& _tmpC8;({struct _fat_ptr _tmp674=({const char*_tmpC7="noconsume parameter %s must be l-values";_tag_fat(_tmpC7,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp674,_tag_fat(_tmpC6,sizeof(void*),1U));});});
goto _LL3;}}else{_LL6: _LL7:
# 686
 goto _LL3;}_LL3:;}
# 691
return inflow;}}
# 696
static struct _tuple17 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 703
union Cyc_CfFlowInfo_FlowInfo _tmpC9=outflow;struct Cyc_List_List*_tmpCB;struct Cyc_Dict_Dict _tmpCA;if((_tmpC9.BottomFL).tag == 1){_LL1: _LL2:
# 705
 return({struct _tuple17 _tmp5AD;({union Cyc_CfFlowInfo_FlowInfo _tmp675=Cyc_CfFlowInfo_BottomFL();_tmp5AD.f1=_tmp675;}),_tmp5AD.f2=rval;_tmp5AD;});}else{_LL3: _tmpCA=((_tmpC9.ReachableFL).val).f1;_tmpCB=((_tmpC9.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict indict=_tmpCA;struct Cyc_List_List*relns=_tmpCB;
# 707
union Cyc_CfFlowInfo_AbsLVal _tmpCC=lval;struct Cyc_CfFlowInfo_Place*_tmpCD;if((_tmpCC.PlaceL).tag == 1){_LL6: _tmpCD=(_tmpCC.PlaceL).val;_LL7: {struct Cyc_CfFlowInfo_Place*p=_tmpCD;
# 715
struct Cyc_Dict_Dict _tmpCE=Cyc_CfFlowInfo_assign_place(fenv,loc,indict,p,rval);struct Cyc_Dict_Dict outdict=_tmpCE;
relns=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,relns,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
if(Cyc_NewControlFlow_warn_lose_unique &&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 721
struct _tuple16 _tmpCF=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(indict,p));struct _tuple16 _stmttmp12=_tmpCF;struct _tuple16 _tmpD0=_stmttmp12;void*_tmpD1;_LLB: _tmpD1=_tmpD0.f1;_LLC: {void*rv=_tmpD1;
void*_tmpD2=rv;switch(*((int*)_tmpD2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD2)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 727
({void*_tmpD3=0U;({unsigned _tmp677=lexp->loc;struct _fat_ptr _tmp676=({const char*_tmpD4="assignment may overwrite alias-free pointer(s)";_tag_fat(_tmpD4,sizeof(char),47U);});Cyc_Warn_warn(_tmp677,_tmp676,_tag_fat(_tmpD3,sizeof(void*),0U));});});
goto _LLD;}_LLD:;}}
# 732
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple17 _tmp5AE;_tmp5AE.f1=outflow,_tmp5AE.f2=rval;_tmp5AE;});}}else{_LL8: _LL9:
# 738
 return({struct _tuple17 _tmp5AF;({union Cyc_CfFlowInfo_FlowInfo _tmp678=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp5AF.f1=_tmp678;}),_tmp5AF.f2=rval;_tmp5AF;});}_LL5:;}}_LL0:;}
# 745
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 751
union Cyc_CfFlowInfo_FlowInfo _tmpD5=f;struct Cyc_List_List*_tmpD7;struct Cyc_Dict_Dict _tmpD6;if((_tmpD5.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmpD6=((_tmpD5.ReachableFL).val).f1;_tmpD7=((_tmpD5.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict outdict=_tmpD6;struct Cyc_List_List*relns=_tmpD7;
# 756
outdict=({struct Cyc_CfFlowInfo_FlowEnv*_tmp67D=fenv;unsigned _tmp67C=loc;struct Cyc_Dict_Dict _tmp67B=outdict;struct Cyc_CfFlowInfo_Place*_tmp67A=({struct Cyc_CfFlowInfo_Place*_tmpD9=_cycalloc(sizeof(*_tmpD9));
({void*_tmp679=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=0U,_tmpD8->f1=vd;_tmpD8;});_tmpD9->root=_tmp679;}),_tmpD9->path=0;_tmpD9;});
# 756
Cyc_CfFlowInfo_assign_place(_tmp67D,_tmp67C,_tmp67B,_tmp67A,rval);});
# 759
relns=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,relns,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmpDA=Cyc_CfFlowInfo_ReachableFL(outdict,relns);union Cyc_CfFlowInfo_FlowInfo outflow=_tmpDA;
Cyc_NewControlFlow_update_tryflow(env,outflow);
# 764
return outflow;}}}_LL0:;}struct _tuple25{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 768
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned pat_loc,unsigned exp_loc){
# 775
if(vds == 0)return inflow;{
# 778
struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);struct Cyc_List_List*vs=_tmpDB;
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple25*)x->hd)).f1 == 0)es=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple25*)x->hd)).f2),_tmpDC->tl=es;_tmpDC;});}}
# 784
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,vs,fenv->unknown_all,pat_loc,name_locs);
# 786
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 790
struct _tuple17 _tmpDD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple17 _stmttmp13=_tmpDD;struct _tuple17 _tmpDE=_stmttmp13;void*_tmpE0;union Cyc_CfFlowInfo_FlowInfo _tmpDF;_LL1: _tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;_LL2: {union Cyc_CfFlowInfo_FlowInfo f=_tmpDF;void*r=_tmpE0;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,f,r);}}}{
# 798
struct Cyc_List_List*_tmpE1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*vds=_tmpE1;
for(0;vds != 0;vds=vds->tl){
struct _tuple25*_tmpE2=(struct _tuple25*)vds->hd;struct _tuple25*_stmttmp14=_tmpE2;struct _tuple25*_tmpE3=_stmttmp14;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Vardecl**_tmpE4;_LL4: _tmpE4=_tmpE3->f1;_tmpE5=_tmpE3->f2;_LL5: {struct Cyc_Absyn_Vardecl**vd=_tmpE4;struct Cyc_Absyn_Exp*ve=_tmpE5;
if(vd != 0 && ve != 0){
if(ve->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE8=({struct Cyc_String_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=0U,({
struct _fat_ptr _tmp67E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(ve));_tmp5B0.f1=_tmp67E;});_tmp5B0;});void*_tmpE6[1U];_tmpE6[0]=& _tmpE8;({struct _fat_ptr _tmp67F=({const char*_tmpE7="oops! pattern init expr %s has no type!\n";_tag_fat(_tmpE7,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp67F,_tag_fat(_tmpE6,sizeof(void*),1U));});});{
# 812
struct Cyc_List_List l=({struct Cyc_List_List _tmp5B2;_tmp5B2.hd=ve,_tmp5B2.tl=0;_tmp5B2;});
union Cyc_CfFlowInfo_FlowInfo _tmpE9=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);union Cyc_CfFlowInfo_FlowInfo f=_tmpE9;
struct _tuple19 _tmpEA=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,ve);struct _tuple19 _stmttmp15=_tmpEA;struct _tuple19 _tmpEB=_stmttmp15;union Cyc_CfFlowInfo_AbsLVal _tmpEC;_LL7: _tmpEC=_tmpEB.f2;_LL8: {union Cyc_CfFlowInfo_AbsLVal lval=_tmpEC;
struct _tuple17 _tmpED=Cyc_NewControlFlow_anal_Rexp(env,1,f,ve);struct _tuple17 _stmttmp16=_tmpED;struct _tuple17 _tmpEE=_stmttmp16;void*_tmpF0;union Cyc_CfFlowInfo_FlowInfo _tmpEF;_LLA: _tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;_LLB: {union Cyc_CfFlowInfo_FlowInfo f=_tmpEF;void*rval=_tmpF0;
union Cyc_CfFlowInfo_FlowInfo _tmpF1=f;struct Cyc_List_List*_tmpF3;struct Cyc_Dict_Dict _tmpF2;if((_tmpF1.ReachableFL).tag == 2){_LLD: _tmpF2=((_tmpF1.ReachableFL).val).f1;_tmpF3=((_tmpF1.ReachableFL).val).f2;_LLE: {struct Cyc_Dict_Dict fd=_tmpF2;struct Cyc_List_List*relns=_tmpF3;
# 818
if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmpF4=lval;struct Cyc_CfFlowInfo_Place*_tmpF5;if((_tmpF4.PlaceL).tag == 1){_LL12: _tmpF5=(_tmpF4.PlaceL).val;_LL13: {struct Cyc_CfFlowInfo_Place*p=_tmpF5;
# 821
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=8U,_tmpF6->f1=*vd,_tmpF6->f2=rval;_tmpF6;});{
# 824
void*_tmpF7=Cyc_CfFlowInfo_lookup_place(fd,p);void*new_rval=_tmpF7;
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->tag=8U,_tmpF8->f1=*vd,_tmpF8->f2=new_rval;_tmpF8;});
fd=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);
goto _LL11;}}}else{_LL14: _LL15:
# 831
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(ve->topt))&& !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*vd)->type))
({struct Cyc_String_pa_PrintArg_struct _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmp5B1;_tmp5B1.tag=0U,({
struct _fat_ptr _tmp680=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(ve));_tmp5B1.f1=_tmp680;});_tmp5B1;});void*_tmpF9[1U];_tmpF9[0]=& _tmpFB;({unsigned _tmp682=exp_loc;struct _fat_ptr _tmp681=({const char*_tmpFA="aliased pattern expression not an l-value: %s";_tag_fat(_tmpFA,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp682,_tmp681,_tag_fat(_tmpF9,sizeof(void*),1U));});});}_LL11:;}
# 841
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,f,*vd,ve,rval,pat_loc);
goto _LLC;}}else{_LLF: _LL10:
# 844
 goto _LLC;}_LLC:;}}}}}}
# 849
return inflow;}}}
# 852
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 854
void*_tmpFC=e->r;void*_stmttmp17=_tmpFC;void*_tmpFD=_stmttmp17;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp102;switch(*((int*)_tmpFD)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)){case 4U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: goto _LL11;}case 20U: _LL7: _tmp102=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp102;
_tmp101=e;goto _LLA;}case 21U: _LL9: _tmp101=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp101;
_tmp100=e;goto _LLC;}case 22U: _LLB: _tmp100=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp100;
# 861
return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);}case 23U: _LLD: _tmpFF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmpFF;
# 863
void*_tmp103=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp18=_tmp103;void*_tmp104=_stmttmp18;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp104)->tag == 6U){_LL14: _LL15:
 return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);}else{_LL16: _LL17:
 return 0;}_LL13:;}case 14U: _LLF: _tmpFE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LL10: {struct Cyc_Absyn_Exp*e=_tmpFE;
# 868
if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);else{
return 0;}}default: _LL11: _LL12:
 return 0;}_LL0:;}
# 874
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp105=*rop;union Cyc_Relations_RelnOp _stmttmp19=_tmp105;union Cyc_Relations_RelnOp _tmp106=_stmttmp19;unsigned _tmp107;unsigned _tmp108;switch((_tmp106.RParamNumelts).tag){case 5U: _LL1: _tmp108=(_tmp106.RParam).val;_LL2: {unsigned i=_tmp108;
# 877
struct Cyc_Absyn_Exp*_tmp109=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)i);struct Cyc_Absyn_Exp*e=_tmp109;
return Cyc_Relations_exp2relnop(e,rop);}case 6U: _LL3: _tmp107=(_tmp106.RParamNumelts).val;_LL4: {unsigned i=_tmp107;
# 880
struct Cyc_Absyn_Exp*_tmp10A=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)i);struct Cyc_Absyn_Exp*e=_tmp10A;
return({struct Cyc_Absyn_Exp*_tmp683=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,e,0U);Cyc_Relations_exp2relnop(_tmp683,rop);});}default: _LL5: _LL6:
 return 1;}_LL0:;}
# 886
static struct _fat_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp10B=rop;unsigned _tmp10C;unsigned _tmp10D;switch((_tmp10B.RParamNumelts).tag){case 5U: _LL1: _tmp10D=(_tmp10B.RParam).val;_LL2: {unsigned i=_tmp10D;
# 889
return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)i));}case 6U: _LL3: _tmp10C=(_tmp10B.RParamNumelts).val;_LL4: {unsigned i=_tmp10C;
# 891
return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)i));}default: _LL5: _LL6:
 return Cyc_Relations_rop2string(rop);}_LL0:;}
# 896
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned loc){
# 899
union Cyc_CfFlowInfo_FlowInfo _tmp10E=inflow;struct Cyc_List_List*_tmp110;struct Cyc_Dict_Dict _tmp10F;if((_tmp10E.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp10F=((_tmp10E.ReachableFL).val).f1;_tmp110=((_tmp10E.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict fd=_tmp10F;struct Cyc_List_List*relns=_tmp110;
# 902
for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp111=(struct Cyc_Relations_Reln*)req->hd;struct Cyc_Relations_Reln*reln=_tmp111;
union Cyc_Relations_RelnOp rop1=reln->rop1;
union Cyc_Relations_RelnOp rop2=reln->rop2;
enum Cyc_Relations_Relation _tmp112=Cyc_Relations_flip_relation(reln->relation);enum Cyc_Relations_Relation r=_tmp112;
if((!Cyc_NewControlFlow_subst_param(exps,& rop1)|| !Cyc_NewControlFlow_subst_param(exps,& rop2))||
 Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,r,rop1,relns))){
struct _fat_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _fat_ptr s2=Cyc_Relations_relation2string(reln->relation);
struct _fat_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp115=({struct Cyc_String_pa_PrintArg_struct _tmp5B6;_tmp5B6.tag=0U,_tmp5B6.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp5B6;});struct Cyc_String_pa_PrintArg_struct _tmp116=({struct Cyc_String_pa_PrintArg_struct _tmp5B5;_tmp5B5.tag=0U,_tmp5B5.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp5B5;});struct Cyc_String_pa_PrintArg_struct _tmp117=({struct Cyc_String_pa_PrintArg_struct _tmp5B4;_tmp5B4.tag=0U,_tmp5B4.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp5B4;});struct Cyc_String_pa_PrintArg_struct _tmp118=({struct Cyc_String_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=0U,({
struct _fat_ptr _tmp684=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_relns2string(relns));_tmp5B3.f1=_tmp684;});_tmp5B3;});void*_tmp113[4U];_tmp113[0]=& _tmp115,_tmp113[1]=& _tmp116,_tmp113[2]=& _tmp117,_tmp113[3]=& _tmp118;({unsigned _tmp686=loc;struct _fat_ptr _tmp685=({const char*_tmp114="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_fat(_tmp114,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp686,_tmp685,_tag_fat(_tmp113,sizeof(void*),4U));});});
break;}}
# 917
goto _LL0;}}_LL0:;}struct _tuple26{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple27{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 922
static struct _tuple17 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 926
struct Cyc_CfFlowInfo_FlowEnv*_tmp119=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp119;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 939
{union Cyc_CfFlowInfo_FlowInfo _tmp11A=inflow;struct Cyc_List_List*_tmp11C;struct Cyc_Dict_Dict _tmp11B;if((_tmp11A.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple17 _tmp5B7;({union Cyc_CfFlowInfo_FlowInfo _tmp687=Cyc_CfFlowInfo_BottomFL();_tmp5B7.f1=_tmp687;}),_tmp5B7.f2=fenv->unknown_all;_tmp5B7;});}else{_LL3: _tmp11B=((_tmp11A.ReachableFL).val).f1;_tmp11C=((_tmp11A.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d2=_tmp11B;struct Cyc_List_List*relns2=_tmp11C;
d=d2;relns=relns2;}}_LL0:;}
# 954 "/tmp/cyclone/src/new_control_flow.cyc"
if((copy_ctxt && Cyc_NewControlFlow_is_local_var_rooted_path(e,0))&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 975 "/tmp/cyclone/src/new_control_flow.cyc"
struct _tuple19 _tmp11D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple19 _stmttmp1A=_tmp11D;struct _tuple19 _tmp11E=_stmttmp1A;union Cyc_CfFlowInfo_AbsLVal _tmp120;union Cyc_CfFlowInfo_FlowInfo _tmp11F;_LL6: _tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_LL7: {union Cyc_CfFlowInfo_FlowInfo flval=_tmp11F;union Cyc_CfFlowInfo_AbsLVal lval=_tmp120;
struct _tuple19 _tmp121=({struct _tuple19 _tmp5BB;_tmp5BB.f1=flval,_tmp5BB.f2=lval;_tmp5BB;});struct _tuple19 _stmttmp1B=_tmp121;struct _tuple19 _tmp122=_stmttmp1B;struct Cyc_CfFlowInfo_Place*_tmp125;struct Cyc_List_List*_tmp124;struct Cyc_Dict_Dict _tmp123;if(((_tmp122.f1).ReachableFL).tag == 2){if(((_tmp122.f2).PlaceL).tag == 1){_LL9: _tmp123=(((_tmp122.f1).ReachableFL).val).f1;_tmp124=(((_tmp122.f1).ReachableFL).val).f2;_tmp125=((_tmp122.f2).PlaceL).val;_LLA: {struct Cyc_Dict_Dict fd=_tmp123;struct Cyc_List_List*r=_tmp124;struct Cyc_CfFlowInfo_Place*p=_tmp125;
# 978
void*_tmp126=Cyc_CfFlowInfo_lookup_place(fd,p);void*old_rval=_tmp126;
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume)){
({void*_tmp127=0U;({unsigned _tmp689=e->loc;struct _fat_ptr _tmp688=({const char*_tmp128="expression attempts to copy a consumed alias-free value";_tag_fat(_tmp128,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp689,_tmp688,_tag_fat(_tmp127,sizeof(void*),0U));});});
return({struct _tuple17 _tmp5B8;({union Cyc_CfFlowInfo_FlowInfo _tmp68A=Cyc_CfFlowInfo_BottomFL();_tmp5B8.f1=_tmp68A;}),_tmp5B8.f2=fenv->unknown_all;_tmp5B8;});}else{
# 984
if(needs_unconsume)
# 986
return({struct _tuple17 _tmp5B9;_tmp5B9.f1=flval,({void*_tmp68B=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,old_rval);_tmp5B9.f2=_tmp68B;});_tmp5B9;});else{
# 989
void*_tmp129=Cyc_CfFlowInfo_make_unique_consumed(fenv,(void*)_check_null(e->topt),e,env->iteration_num,old_rval);void*new_rval=_tmp129;
struct Cyc_Dict_Dict _tmp12A=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,new_rval);struct Cyc_Dict_Dict outdict=_tmp12A;
# 1001
return({struct _tuple17 _tmp5BA;({union Cyc_CfFlowInfo_FlowInfo _tmp68C=Cyc_CfFlowInfo_ReachableFL(outdict,r);_tmp5BA.f1=_tmp68C;}),_tmp5BA.f2=old_rval;_tmp5BA;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1005
 goto _LL8;}_LL8:;}}{
# 1008
void*_tmp12B=e->r;void*_stmttmp1C=_tmp12B;void*_tmp12C=_stmttmp1C;struct Cyc_Absyn_Stmt*_tmp12D;int _tmp130;void*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;int _tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Vardecl*_tmp131;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp138;enum Cyc_Absyn_AggrKind _tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct _fat_ptr*_tmp145;struct Cyc_Absyn_Exp*_tmp144;struct _fat_ptr*_tmp147;struct Cyc_Absyn_Exp*_tmp146;struct _fat_ptr*_tmp149;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;int _tmp154;struct Cyc_Absyn_Exp*_tmp153;void**_tmp152;struct Cyc_Absyn_Exp*_tmp151;int _tmp150;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_List_List*_tmp160;enum Cyc_Absyn_Primop _tmp15F;struct Cyc_Absyn_Datatypedecl*_tmp162;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Vardecl*_tmp163;struct Cyc_Absyn_Vardecl*_tmp164;struct Cyc_Absyn_Vardecl*_tmp165;struct _fat_ptr _tmp166;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;switch(*((int*)_tmp12C)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp16B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp16B;
# 1012
struct _tuple17 _tmp16C=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1);struct _tuple17 _stmttmp1D=_tmp16C;struct _tuple17 _tmp16D=_stmttmp1D;void*_tmp16F;union Cyc_CfFlowInfo_FlowInfo _tmp16E;_LL81: _tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;_LL82: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp16E;void*r1=_tmp16F;
struct _tuple17 _tmp170=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _stmttmp1E=_tmp170;struct _tuple17 _tmp171=_stmttmp1E;void*_tmp173;union Cyc_CfFlowInfo_FlowInfo _tmp172;_LL84: _tmp172=_tmp171.f1;_tmp173=_tmp171.f2;_LL85: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp172;void*r2=_tmp173;
return({struct _tuple17 _tmp5BC;_tmp5BC.f1=f2,_tmp5BC.f2=r1;_tmp5BC;});}}}}else{_LL10: _tmp16A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp16A;
# 1018
_tmp169=e1;goto _LL13;}}case 12U: _LL12: _tmp169=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp169;
_tmp168=e1;goto _LL15;}case 41U: _LL14: _tmp168=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL15: {struct Cyc_Absyn_Exp*e1=_tmp168;
_tmp167=e1;goto _LL17;}case 13U: _LL16: _tmp167=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp167;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1);}case 2U: _LL18: _tmp166=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL19: {struct _fat_ptr p=_tmp166;
# 1024
if(!({struct _fat_ptr _tmp68D=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp68D,({const char*_tmp174="print_flow";_tag_fat(_tmp174,sizeof(char),11U);}));})){
struct _fat_ptr _tmp175=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp175;
({struct Cyc_String_pa_PrintArg_struct _tmp178=({struct Cyc_String_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=0U,_tmp5BD.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp5BD;});void*_tmp176[1U];_tmp176[0]=& _tmp178;({struct Cyc___cycFILE*_tmp68F=Cyc_stderr;struct _fat_ptr _tmp68E=({const char*_tmp177="%s: current flow is\n";_tag_fat(_tmp177,sizeof(char),21U);});Cyc_fprintf(_tmp68F,_tmp68E,_tag_fat(_tmp176,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp179=0U;({struct Cyc___cycFILE*_tmp691=Cyc_stderr;struct _fat_ptr _tmp690=({const char*_tmp17A="\n";_tag_fat(_tmp17A,sizeof(char),2U);});Cyc_fprintf(_tmp691,_tmp690,_tag_fat(_tmp179,sizeof(void*),0U));});});}else{
# 1030
if(!({struct _fat_ptr _tmp692=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp692,({const char*_tmp17B="debug_on";_tag_fat(_tmp17B,sizeof(char),9U);}));}));else{
# 1035
if(!({struct _fat_ptr _tmp693=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp693,({const char*_tmp17C="debug_off";_tag_fat(_tmp17C,sizeof(char),10U);}));}));else{
# 1041
({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,_tmp5BE.f1=(struct _fat_ptr)((struct _fat_ptr)p);_tmp5BE;});void*_tmp17D[1U];_tmp17D[0]=& _tmp17F;({unsigned _tmp695=e->loc;struct _fat_ptr _tmp694=({const char*_tmp17E="unknown pragma %s";_tag_fat(_tmp17E,sizeof(char),18U);});Cyc_Warn_warn(_tmp695,_tmp694,_tag_fat(_tmp17D,sizeof(void*),1U));});});}}}
return({struct _tuple17 _tmp5BF;_tmp5BF.f1=inflow,_tmp5BF.f2=fenv->zero;_tmp5BF;});}case 40U: _LL1A: _LL1B:
# 1044
 goto _LL1D;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).Wstring_c).tag){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).Int_c).val).f2 == 0){_LL1E: _LL1F:
 return({struct _tuple17 _tmp5C0;_tmp5C0.f1=inflow,_tmp5C0.f2=fenv->zero;_tmp5C0;});}else{_LL20: _LL21:
 goto _LL23;}case 8U: _LL22: _LL23:
 goto _LL25;case 9U: _LL24: _LL25:
 goto _LL27;default: _LL2A: _LL2B:
# 1053
 goto _LL2D;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)){case 2U: _LL26: _LL27:
# 1050
 return({struct _tuple17 _tmp5C1;_tmp5C1.f1=inflow,_tmp5C1.f2=fenv->notzeroall;_tmp5C1;});case 1U: _LL38: _LL39:
# 1062
 return({struct _tuple17 _tmp5C2;_tmp5C2.f1=inflow,({void*_tmp696=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5C2.f2=_tmp696;});_tmp5C2;});case 4U: _LL3A: _tmp165=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)->f1;_LL3B: {struct Cyc_Absyn_Vardecl*vd=_tmp165;
# 1067
if((int)vd->sc == (int)Cyc_Absyn_Static)
return({struct _tuple17 _tmp5C3;_tmp5C3.f1=inflow,({void*_tmp697=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5C3.f2=_tmp697;});_tmp5C3;});
# 1070
_tmp164=vd;goto _LL3D;}case 3U: _LL3C: _tmp164=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)->f1;_LL3D: {struct Cyc_Absyn_Vardecl*vd=_tmp164;
_tmp163=vd;goto _LL3F;}case 5U: _LL3E: _tmp163=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12C)->f1)->f1;_LL3F: {struct Cyc_Absyn_Vardecl*vd=_tmp163;
# 1074
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5C5;_tmp5C5.tag=0U,_tmp5C5.f1=vd;_tmp5C5;});
return({struct _tuple17 _tmp5C4;_tmp5C4.f1=inflow,({void*_tmp699=({struct Cyc_Dict_Dict _tmp698=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp698,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp180=_cycalloc(sizeof(*_tmp180));*_tmp180=vdroot;_tmp180;}));});_tmp5C4.f2=_tmp699;});_tmp5C4;});}default: _LL78: _LL79:
# 1678
 goto _LL7B;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C)->f1 == 0){_LL28: _LL29:
# 1052
 goto _LL2B;}else{_LL66: _tmp161=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp162=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL67: {struct Cyc_List_List*es=_tmp161;struct Cyc_Absyn_Datatypedecl*tud=_tmp162;
# 1540
_tmp13A=es;goto _LL69;}}case 42U: _LL2C: _LL2D:
# 1054
 goto _LL2F;case 18U: _LL2E: _LL2F:
 goto _LL31;case 17U: _LL30: _LL31:
 goto _LL33;case 19U: _LL32: _LL33:
 goto _LL35;case 33U: _LL34: _LL35:
 goto _LL37;case 32U: _LL36: _LL37:
 return({struct _tuple17 _tmp5C6;_tmp5C6.f1=inflow,_tmp5C6.f2=fenv->unknown_all;_tmp5C6;});case 3U: _LL40: _tmp15F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp160=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL41: {enum Cyc_Absyn_Primop p=_tmp15F;struct Cyc_List_List*es=_tmp160;
# 1081
struct _tuple17 _tmp181=Cyc_NewControlFlow_anal_use_ints(env,inflow,es);struct _tuple17 _stmttmp1F=_tmp181;struct _tuple17 _tmp182=_stmttmp1F;void*_tmp184;union Cyc_CfFlowInfo_FlowInfo _tmp183;_LL87: _tmp183=_tmp182.f1;_tmp184=_tmp182.f2;_LL88: {union Cyc_CfFlowInfo_FlowInfo f=_tmp183;void*r=_tmp184;
return({struct _tuple17 _tmp5C7;_tmp5C7.f1=f,_tmp5C7.f2=r;_tmp5C7;});}}case 5U: _LL42: _tmp15E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL43: {struct Cyc_Absyn_Exp*e1=_tmp15E;
# 1085
struct Cyc_List_List _tmp185=({struct Cyc_List_List _tmp5CB;_tmp5CB.hd=e1,_tmp5CB.tl=0;_tmp5CB;});struct Cyc_List_List arg=_tmp185;
struct _tuple17 _tmp186=Cyc_NewControlFlow_anal_use_ints(env,inflow,& arg);struct _tuple17 _stmttmp20=_tmp186;struct _tuple17 _tmp187=_stmttmp20;union Cyc_CfFlowInfo_FlowInfo _tmp188;_LL8A: _tmp188=_tmp187.f1;_LL8B: {union Cyc_CfFlowInfo_FlowInfo f=_tmp188;
struct _tuple19 _tmp189=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,e1);struct _tuple19 _stmttmp21=_tmp189;struct _tuple19 _tmp18A=_stmttmp21;union Cyc_CfFlowInfo_AbsLVal _tmp18B;_LL8D: _tmp18B=_tmp18A.f2;_LL8E: {union Cyc_CfFlowInfo_AbsLVal lval=_tmp18B;
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(e1->topt));
if(iszt){
# 1092
struct _tuple17 _tmp18C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp22=_tmp18C;struct _tuple17 _tmp18D=_stmttmp22;void*_tmp18F;union Cyc_CfFlowInfo_FlowInfo _tmp18E;_LL90: _tmp18E=_tmp18D.f1;_tmp18F=_tmp18D.f2;_LL91: {union Cyc_CfFlowInfo_FlowInfo g=_tmp18E;void*r=_tmp18F;
Cyc_NewControlFlow_anal_derefR(env,inflow,g,e1,r,0);}}
# 1095
{struct _tuple26 _tmp190=({struct _tuple26 _tmp5C8;_tmp5C8.f1=lval,_tmp5C8.f2=f;_tmp5C8;});struct _tuple26 _stmttmp23=_tmp190;struct _tuple26 _tmp191=_stmttmp23;struct Cyc_List_List*_tmp194;struct Cyc_Dict_Dict _tmp193;struct Cyc_CfFlowInfo_Place*_tmp192;if(((_tmp191.f1).PlaceL).tag == 1){if(((_tmp191.f2).ReachableFL).tag == 2){_LL93: _tmp192=((_tmp191.f1).PlaceL).val;_tmp193=(((_tmp191.f2).ReachableFL).val).f1;_tmp194=(((_tmp191.f2).ReachableFL).val).f2;_LL94: {struct Cyc_CfFlowInfo_Place*p=_tmp192;struct Cyc_Dict_Dict outdict=_tmp193;struct Cyc_List_List*relns=_tmp194;
# 1097
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
f=({struct Cyc_Dict_Dict _tmp69A=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,p,fenv->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp69A,relns);});
# 1102
goto _LL92;}}else{goto _LL95;}}else{_LL95: _LL96:
 goto _LL92;}_LL92:;}
# 1106
if(iszt)
return({struct _tuple17 _tmp5C9;_tmp5C9.f1=f,_tmp5C9.f2=fenv->notzeroall;_tmp5C9;});else{
return({struct _tuple17 _tmp5CA;_tmp5CA.f1=f,_tmp5CA.f2=fenv->unknown_all;_tmp5CA;});}}}}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f2 != 0){_LL44: _tmp15C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp15D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL45: {struct Cyc_Absyn_Exp*l=_tmp15C;struct Cyc_Absyn_Exp*r=_tmp15D;
# 1111
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp195=0U;({unsigned _tmp69C=e->loc;struct _fat_ptr _tmp69B=({const char*_tmp196="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp196,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp69C,_tmp69B,_tag_fat(_tmp195,sizeof(void*),0U));});});
return({struct _tuple17 _tmp5CC;({union Cyc_CfFlowInfo_FlowInfo _tmp69D=Cyc_CfFlowInfo_BottomFL();_tmp5CC.f1=_tmp69D;}),_tmp5CC.f2=fenv->unknown_all;_tmp5CC;});}{
# 1115
struct _tuple17 _tmp197=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,r);struct _tuple17 _stmttmp24=_tmp197;struct _tuple17 _tmp198=_stmttmp24;void*_tmp19A;union Cyc_CfFlowInfo_FlowInfo _tmp199;_LL98: _tmp199=_tmp198.f1;_tmp19A=_tmp198.f2;_LL99: {union Cyc_CfFlowInfo_FlowInfo right_out=_tmp199;void*rval=_tmp19A;
struct _tuple19 _tmp19B=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,l);struct _tuple19 _stmttmp25=_tmp19B;struct _tuple19 _tmp19C=_stmttmp25;union Cyc_CfFlowInfo_AbsLVal _tmp19E;union Cyc_CfFlowInfo_FlowInfo _tmp19D;_LL9B: _tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;_LL9C: {union Cyc_CfFlowInfo_FlowInfo f=_tmp19D;union Cyc_CfFlowInfo_AbsLVal lval=_tmp19E;
{union Cyc_CfFlowInfo_FlowInfo _tmp19F=f;struct Cyc_List_List*_tmp1A1;struct Cyc_Dict_Dict _tmp1A0;if((_tmp19F.ReachableFL).tag == 2){_LL9E: _tmp1A0=((_tmp19F.ReachableFL).val).f1;_tmp1A1=((_tmp19F.ReachableFL).val).f2;_LL9F: {struct Cyc_Dict_Dict outdict=_tmp1A0;struct Cyc_List_List*relns=_tmp1A1;
# 1119
{union Cyc_CfFlowInfo_AbsLVal _tmp1A2=lval;struct Cyc_CfFlowInfo_Place*_tmp1A3;if((_tmp1A2.PlaceL).tag == 1){_LLA3: _tmp1A3=(_tmp1A2.PlaceL).val;_LLA4: {struct Cyc_CfFlowInfo_Place*p=_tmp1A3;
# 1123
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,l);
outdict=Cyc_CfFlowInfo_assign_place(fenv,l->loc,outdict,p,fenv->unknown_all);
# 1126
f=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
# 1130
goto _LLA2;}}else{_LLA5: _LLA6:
# 1133
 goto _LLA2;}_LLA2:;}
# 1135
goto _LL9D;}}else{_LLA0: _LLA1:
 goto _LL9D;}_LL9D:;}
# 1138
return({struct _tuple17 _tmp5CD;_tmp5CD.f1=f,_tmp5CD.f2=fenv->unknown_all;_tmp5CD;});}}}}}else{_LL46: _tmp15A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp15B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL47: {struct Cyc_Absyn_Exp*e1=_tmp15A;struct Cyc_Absyn_Exp*e2=_tmp15B;
# 1141
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp1A4=0U;({unsigned _tmp69F=e->loc;struct _fat_ptr _tmp69E=({const char*_tmp1A5="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp1A5,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp69F,_tmp69E,_tag_fat(_tmp1A4,sizeof(void*),0U));});});
return({struct _tuple17 _tmp5CE;({union Cyc_CfFlowInfo_FlowInfo _tmp6A0=Cyc_CfFlowInfo_BottomFL();_tmp5CE.f1=_tmp6A0;}),_tmp5CE.f2=fenv->unknown_all;_tmp5CE;});}
# 1147
inflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp6A2=env;union Cyc_CfFlowInfo_FlowInfo _tmp6A1=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6A2,_tmp6A1,({struct Cyc_Absyn_Exp*_tmp1A6[2U];_tmp1A6[0]=e1,_tmp1A6[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1A6,sizeof(struct Cyc_Absyn_Exp*),2U));}));});{
# 1149
struct _tuple17 _tmp1A7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2);struct _tuple17 _stmttmp26=_tmp1A7;struct _tuple17 _tmp1A8=_stmttmp26;void*_tmp1AA;union Cyc_CfFlowInfo_FlowInfo _tmp1A9;_LLA8: _tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;_LLA9: {union Cyc_CfFlowInfo_FlowInfo right_out=_tmp1A9;void*rval=_tmp1AA;
struct _tuple19 _tmp1AB=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,e1);struct _tuple19 _stmttmp27=_tmp1AB;struct _tuple19 _tmp1AC=_stmttmp27;union Cyc_CfFlowInfo_AbsLVal _tmp1AE;union Cyc_CfFlowInfo_FlowInfo _tmp1AD;_LLAB: _tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;_LLAC: {union Cyc_CfFlowInfo_FlowInfo outflow=_tmp1AD;union Cyc_CfFlowInfo_AbsLVal lval=_tmp1AE;
struct _tuple17 _tmp1AF=Cyc_NewControlFlow_do_assign(fenv,env,outflow,e1,lval,e2,rval,e->loc);struct _tuple17 _stmttmp28=_tmp1AF;struct _tuple17 _tmp1B0=_stmttmp28;void*_tmp1B2;union Cyc_CfFlowInfo_FlowInfo _tmp1B1;_LLAE: _tmp1B1=_tmp1B0.f1;_tmp1B2=_tmp1B0.f2;_LLAF: {union Cyc_CfFlowInfo_FlowInfo outflow=_tmp1B1;void*rval=_tmp1B2;
# 1155
return({struct _tuple17 _tmp5CF;_tmp5CF.f1=outflow,_tmp5CF.f2=rval;_tmp5CF;});}}}}}}case 9U: _LL48: _tmp158=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp159=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL49: {struct Cyc_Absyn_Exp*e1=_tmp158;struct Cyc_Absyn_Exp*e2=_tmp159;
# 1158
struct _tuple17 _tmp1B3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp29=_tmp1B3;struct _tuple17 _tmp1B4=_stmttmp29;void*_tmp1B6;union Cyc_CfFlowInfo_FlowInfo _tmp1B5;_LLB1: _tmp1B5=_tmp1B4.f1;_tmp1B6=_tmp1B4.f2;_LLB2: {union Cyc_CfFlowInfo_FlowInfo f=_tmp1B5;void*r=_tmp1B6;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f,e2);}}case 11U: _LL4A: _tmp157=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL4B: {struct Cyc_Absyn_Exp*e1=_tmp157;
# 1162
struct _tuple17 _tmp1B7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1);struct _tuple17 _stmttmp2A=_tmp1B7;struct _tuple17 _tmp1B8=_stmttmp2A;void*_tmp1BA;union Cyc_CfFlowInfo_FlowInfo _tmp1B9;_LLB4: _tmp1B9=_tmp1B8.f1;_tmp1BA=_tmp1B8.f2;_LLB5: {union Cyc_CfFlowInfo_FlowInfo f=_tmp1B9;void*r=_tmp1BA;
Cyc_NewControlFlow_use_Rval(env,e1->loc,f,r);
return({struct _tuple17 _tmp5D0;({union Cyc_CfFlowInfo_FlowInfo _tmp6A4=Cyc_CfFlowInfo_BottomFL();_tmp5D0.f1=_tmp6A4;}),({void*_tmp6A3=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5D0.f2=_tmp6A3;});_tmp5D0;});}}case 10U: _LL4C: _tmp155=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp156=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL4D: {struct Cyc_Absyn_Exp*e1=_tmp155;struct Cyc_List_List*es=_tmp156;
# 1167
struct Cyc_List_List*_tmp1BB=es;struct Cyc_List_List*orig_es=_tmp1BB;
struct _tuple17 _tmp1BC=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp2B=_tmp1BC;struct _tuple17 _tmp1BD=_stmttmp2B;void*_tmp1BF;union Cyc_CfFlowInfo_FlowInfo _tmp1BE;_LLB7: _tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;_LLB8: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp1BE;void*r1=_tmp1BF;
# 1170
Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);{
# 1173
struct _tuple24 _tmp1C0=({struct Cyc_NewControlFlow_AnalEnv*_tmp6A6=env;union Cyc_CfFlowInfo_FlowInfo _tmp6A5=f1;Cyc_NewControlFlow_anal_Rexps(_tmp6A6,_tmp6A5,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(es),1,1);});struct _tuple24 _stmttmp2C=_tmp1C0;struct _tuple24 _tmp1C1=_stmttmp2C;struct Cyc_List_List*_tmp1C3;union Cyc_CfFlowInfo_FlowInfo _tmp1C2;_LLBA: _tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;_LLBB: {union Cyc_CfFlowInfo_FlowInfo fst_outflow=_tmp1C2;struct Cyc_List_List*rvals=_tmp1C3;
# 1175
union Cyc_CfFlowInfo_FlowInfo _tmp1C4=Cyc_NewControlFlow_use_Rval(env,e1->loc,fst_outflow,r1);union Cyc_CfFlowInfo_FlowInfo outflow=_tmp1C4;
# 1177
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1C5=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp2D=_tmp1C5;void*_tmp1C6=_stmttmp2D;void*_tmp1C7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->tag == 3U){_LLBD: _tmp1C7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->f1).elt_type;_LLBE: {void*t=_tmp1C7;
# 1184
{void*_tmp1C8=Cyc_Absyn_compress(t);void*_stmttmp2E=_tmp1C8;void*_tmp1C9=_stmttmp2E;struct Cyc_List_List*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CA;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->tag == 5U){_LLC2: _tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).attributes;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).requires_relns;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ensures_relns;_LLC3: {struct Cyc_List_List*atts=_tmp1CA;struct Cyc_List_List*req=_tmp1CB;struct Cyc_List_List*ens=_tmp1CC;
# 1186
requires=req;
ensures=ens;
for(0;atts != 0;atts=atts->tl){
# 1190
void*_tmp1CD=(void*)atts->hd;void*_stmttmp2F=_tmp1CD;void*_tmp1CE=_stmttmp2F;int _tmp1CF;int _tmp1D0;int _tmp1D1;switch(*((int*)_tmp1CE)){case 20U: _LLC7: _tmp1D1=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CE)->f1;_LLC8: {int i=_tmp1D1;
# 1192
init_params=({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->hd=(void*)i,_tmp1D2->tl=init_params;_tmp1D2;});goto _LLC6;}case 21U: _LLC9: _tmp1D0=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CE)->f1;_LLCA: {int i=_tmp1D0;
# 1194
nolive_unique_params=({struct Cyc_List_List*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->hd=(void*)i,_tmp1D3->tl=nolive_unique_params;_tmp1D3;});
consume_params=({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=(void*)i,_tmp1D4->tl=consume_params;_tmp1D4;});
goto _LLC6;}case 22U: _LLCB: _tmp1CF=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp1CE)->f1;_LLCC: {int i=_tmp1CF;
# 1198
consume_params=({struct Cyc_List_List*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->hd=(void*)i,_tmp1D5->tl=consume_params;_tmp1D5;});
goto _LLC6;}default: _LLCD: _LLCE:
 goto _LLC6;}_LLC6:;}
# 1202
goto _LLC1;}}else{_LLC4: _LLC5:
({void*_tmp1D6=0U;({struct _fat_ptr _tmp6A7=({const char*_tmp1D7="anal_Rexp: bad function type";_tag_fat(_tmp1D7,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6A7,_tag_fat(_tmp1D6,sizeof(void*),0U));});});}_LLC1:;}
# 1205
goto _LLBC;}}else{_LLBF: _LLC0:
({void*_tmp1D8=0U;({struct _fat_ptr _tmp6A8=({const char*_tmp1D9="anal_Rexp: bad function type";_tag_fat(_tmp1D9,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6A8,_tag_fat(_tmp1D8,sizeof(void*),0U));});});}_LLBC:;}
# 1208
{int i=1;for(0;rvals != 0;(rvals=rvals->tl,es=((struct Cyc_List_List*)_check_null(es))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1DA=fst_outflow;struct Cyc_Dict_Dict _tmp1DB;if((_tmp1DA.BottomFL).tag == 1){_LLD0: _LLD1:
 goto _LLCF;}else{_LLD2: _tmp1DB=((_tmp1DA.ReachableFL).val).f1;_LLD3: {struct Cyc_Dict_Dict fst_d=_tmp1DB;
# 1213
struct _tuple16 _tmp1DC=Cyc_CfFlowInfo_unname_rval((void*)rvals->hd);struct _tuple16 _stmttmp30=_tmp1DC;struct _tuple16 _tmp1DD=_stmttmp30;void*_tmp1DE;_LLD5: _tmp1DE=_tmp1DD.f1;_LLD6: {void*r=_tmp1DE;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,fst_d,(void*)rvals->hd)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer((void*)rvals->hd))
({void*_tmp1DF=0U;({unsigned _tmp6AA=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp6A9=({const char*_tmp1E0="expression may not be initialized";_tag_fat(_tmp1E0,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6AA,_tmp6A9,_tag_fat(_tmp1DF,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1E1=outflow;struct Cyc_List_List*_tmp1E3;struct Cyc_Dict_Dict _tmp1E2;if((_tmp1E1.BottomFL).tag == 1){_LLD8: _LLD9:
 goto _LLD7;}else{_LLDA: _tmp1E2=((_tmp1E1.ReachableFL).val).f1;_tmp1E3=((_tmp1E1.ReachableFL).val).f2;_LLDB: {struct Cyc_Dict_Dict d=_tmp1E2;struct Cyc_List_List*relns=_tmp1E3;
# 1221
struct Cyc_Dict_Dict _tmp1E4=Cyc_CfFlowInfo_escape_deref(fenv,d,(void*)rvals->hd);struct Cyc_Dict_Dict ans_d=_tmp1E4;
{void*_tmp1E5=(void*)rvals->hd;void*_stmttmp31=_tmp1E5;void*_tmp1E6=_stmttmp31;struct Cyc_CfFlowInfo_Place*_tmp1E7;switch(*((int*)_tmp1E6)){case 4U: _LLDD: _tmp1E7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E6)->f1;_LLDE: {struct Cyc_CfFlowInfo_Place*p=_tmp1E7;
# 1224
{void*_tmp1E8=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->topt));void*_stmttmp32=_tmp1E8;void*_tmp1E9=_stmttmp32;void*_tmp1EA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E9)->tag == 3U){_LLE4: _tmp1EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E9)->f1).elt_type;_LLE5: {void*t=_tmp1EA;
# 1226
ans_d=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6AE=fenv;unsigned _tmp6AD=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct Cyc_Dict_Dict _tmp6AC=ans_d;struct Cyc_CfFlowInfo_Place*_tmp6AB=p;Cyc_CfFlowInfo_assign_place(_tmp6AE,_tmp6AD,_tmp6AC,_tmp6AB,
Cyc_CfFlowInfo_typ_to_absrval(fenv,t,0,fenv->esc_all));});
# 1230
goto _LLE3;}}else{_LLE6: _LLE7:
({void*_tmp1EB=0U;({struct _fat_ptr _tmp6AF=({const char*_tmp1EC="anal_Rexp:bad type for initialized arg";_tag_fat(_tmp1EC,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6AF,_tag_fat(_tmp1EB,sizeof(void*),0U));});});}_LLE3:;}
# 1233
goto _LLDC;}case 5U: _LLDF: _LLE0:
# 1235
({void*_tmp1ED=0U;({struct _fat_ptr _tmp6B0=({const char*_tmp1EE="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_fat(_tmp1EE,sizeof(char),68U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6B0,_tag_fat(_tmp1ED,sizeof(void*),0U));});});default: _LLE1: _LLE2:
# 1237
 goto _LLDC;}_LLDC:;}
# 1239
outflow=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);
goto _LLD7;}}_LLD7:;}
# 1242
goto _LLCF;}}}_LLCF:;}else{
# 1245
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1250
outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp6B4=env;unsigned _tmp6B3=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;void*_tmp6B2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6B1=outflow;Cyc_NewControlFlow_use_nounique_Rval(_tmp6B4,_tmp6B3,_tmp6B2,_tmp6B1,(void*)rvals->hd);});else{
# 1260
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);
if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(consume_params,i)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),0))
outflow=Cyc_NewControlFlow_restore_noconsume_arg(env,outflow,(struct Cyc_Absyn_Exp*)es->hd,((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)rvals->hd);}}}}
# 1267
Cyc_NewControlFlow_check_fn_requires(env,outflow,orig_es,requires,e->loc);
# 1270
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e1->topt)))
return({struct _tuple17 _tmp5D1;({union Cyc_CfFlowInfo_FlowInfo _tmp6B6=Cyc_CfFlowInfo_BottomFL();_tmp5D1.f1=_tmp6B6;}),({void*_tmp6B5=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5D1.f2=_tmp6B5;});_tmp5D1;});else{
# 1273
return({struct _tuple17 _tmp5D2;_tmp5D2.f1=outflow,({void*_tmp6B7=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5D2.f2=_tmp6B7;});_tmp5D2;});}}}}}case 34U: _LL4E: _tmp150=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).is_calloc;_tmp151=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).rgn;_tmp152=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).elt_type;_tmp153=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).num_elts;_tmp154=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12C)->f1).fat_result;_LL4F: {int iscalloc=_tmp150;struct Cyc_Absyn_Exp*eopt=_tmp151;void**topt=_tmp152;struct Cyc_Absyn_Exp*exp=_tmp153;int isfat=_tmp154;
# 1276
void*place_val;
if(isfat)place_val=fenv->notzeroall;else{
if(iscalloc)place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*((void**)_check_null(topt)),0,fenv->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*((void**)_check_null(topt)),0,fenv->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(eopt != 0){
struct _tuple24 _tmp1EF=({struct Cyc_NewControlFlow_AnalEnv*_tmp6B9=env;union Cyc_CfFlowInfo_FlowInfo _tmp6B8=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6B9,_tmp6B8,({struct Cyc_Absyn_Exp*_tmp1F3[2U];_tmp1F3[0]=eopt,_tmp1F3[1]=exp;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1F3,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple24 _stmttmp33=_tmp1EF;struct _tuple24 _tmp1F0=_stmttmp33;struct Cyc_List_List*_tmp1F2;union Cyc_CfFlowInfo_FlowInfo _tmp1F1;_LLE9: _tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;_LLEA: {union Cyc_CfFlowInfo_FlowInfo f=_tmp1F1;struct Cyc_List_List*rvals=_tmp1F2;
outflow=f;}}else{
# 1285
struct _tuple17 _tmp1F4=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp);struct _tuple17 _stmttmp34=_tmp1F4;struct _tuple17 _tmp1F5=_stmttmp34;union Cyc_CfFlowInfo_FlowInfo _tmp1F6;_LLEC: _tmp1F6=_tmp1F5.f1;_LLED: {union Cyc_CfFlowInfo_FlowInfo f=_tmp1F6;
outflow=f;}}
# 1290
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple17 _tmp5D3;_tmp5D3.f1=outflow,({void*_tmp6BA=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->tag=5U,_tmp1F7->f1=place_val;_tmp1F7;});_tmp5D3.f2=_tmp6BA;});_tmp5D3;});else{
# 1294
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->tag=1U,_tmp1FD->f1=exp,_tmp1FD->f2=(void*)_check_null(e->topt);_tmp1FD;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->root=root,_tmp1FC->path=0;_tmp1FC;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->tag=4U,_tmp1FB->f1=place;_tmp1FB;});
union Cyc_CfFlowInfo_FlowInfo _tmp1F8=outflow;struct Cyc_List_List*_tmp1FA;struct Cyc_Dict_Dict _tmp1F9;if((_tmp1F8.BottomFL).tag == 1){_LLEF: _LLF0:
 return({struct _tuple17 _tmp5D4;_tmp5D4.f1=outflow,_tmp5D4.f2=rval;_tmp5D4;});}else{_LLF1: _tmp1F9=((_tmp1F8.ReachableFL).val).f1;_tmp1FA=((_tmp1F8.ReachableFL).val).f2;_LLF2: {struct Cyc_Dict_Dict d2=_tmp1F9;struct Cyc_List_List*relns=_tmp1FA;
# 1300
return({struct _tuple17 _tmp5D5;({union Cyc_CfFlowInfo_FlowInfo _tmp6BC=({struct Cyc_Dict_Dict _tmp6BB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6BB,relns);});_tmp5D5.f1=_tmp6BC;}),_tmp5D5.f2=rval;_tmp5D5;});}}_LLEE:;}}}case 35U: _LL50: _tmp14E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp14F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL51: {struct Cyc_Absyn_Exp*e1=_tmp14E;struct Cyc_Absyn_Exp*e2=_tmp14F;
# 1305
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1311
struct _tuple24 _tmp1FE=({struct Cyc_NewControlFlow_AnalEnv*_tmp6BE=env;union Cyc_CfFlowInfo_FlowInfo _tmp6BD=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6BE,_tmp6BD,({struct Cyc_Absyn_Exp*_tmp219[2U];_tmp219[0]=e1,_tmp219[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp219,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple24 _stmttmp35=_tmp1FE;struct _tuple24 _tmp1FF=_stmttmp35;struct Cyc_List_List*_tmp201;union Cyc_CfFlowInfo_FlowInfo _tmp200;_LLF4: _tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;_LLF5: {union Cyc_CfFlowInfo_FlowInfo f=_tmp200;struct Cyc_List_List*rvals=_tmp201;
left_rval=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(rvals->tl))->hd;
outflow=f;{
# 1317
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp202=right_rval;if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp202)->tag == 0U){_LLF7: _LLF8:
 goto _LLF6;}else{_LLF9: _LLFA:
({void*_tmp203=0U;({unsigned _tmp6C0=e1->loc;struct _fat_ptr _tmp6BF=({const char*_tmp204="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp204,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6C0,_tmp6BF,_tag_fat(_tmp203,sizeof(void*),0U));});});}_LLF6:;}else{
# 1328
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp205=left_rval;if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp205)->tag == 0U){_LLFC: _LLFD:
 goto _LLFB;}else{_LLFE: _LLFF:
({void*_tmp206=0U;({unsigned _tmp6C2=e2->loc;struct _fat_ptr _tmp6C1=({const char*_tmp207="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp207,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6C2,_tmp6C1,_tag_fat(_tmp206,sizeof(void*),0U));});});}_LLFB:;}}{
# 1336
struct _tuple19 _tmp208=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e1);struct _tuple19 _stmttmp36=_tmp208;struct _tuple19 _tmp209=_stmttmp36;union Cyc_CfFlowInfo_AbsLVal _tmp20A;_LL101: _tmp20A=_tmp209.f2;_LL102: {union Cyc_CfFlowInfo_AbsLVal left_lval=_tmp20A;
struct _tuple19 _tmp20B=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e2);struct _tuple19 _stmttmp37=_tmp20B;struct _tuple19 _tmp20C=_stmttmp37;union Cyc_CfFlowInfo_AbsLVal _tmp20D;_LL104: _tmp20D=_tmp20C.f2;_LL105: {union Cyc_CfFlowInfo_AbsLVal right_lval=_tmp20D;
{union Cyc_CfFlowInfo_FlowInfo _tmp20E=outflow;struct Cyc_List_List*_tmp210;struct Cyc_Dict_Dict _tmp20F;if((_tmp20E.ReachableFL).tag == 2){_LL107: _tmp20F=((_tmp20E.ReachableFL).val).f1;_tmp210=((_tmp20E.ReachableFL).val).f2;_LL108: {struct Cyc_Dict_Dict outdict=_tmp20F;struct Cyc_List_List*relns=_tmp210;
# 1340
{union Cyc_CfFlowInfo_AbsLVal _tmp211=left_lval;struct Cyc_CfFlowInfo_Place*_tmp212;if((_tmp211.PlaceL).tag == 1){_LL10C: _tmp212=(_tmp211.PlaceL).val;_LL10D: {struct Cyc_CfFlowInfo_Place*lp=_tmp212;
# 1342
outdict=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,lp,right_rval);
goto _LL10B;}}else{_LL10E: _LL10F:
# 1348
 if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,right_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp213=0U;({unsigned _tmp6C4=e2->loc;struct _fat_ptr _tmp6C3=({const char*_tmp214="expression may not be fully initialized";_tag_fat(_tmp214,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6C4,_tmp6C3,_tag_fat(_tmp213,sizeof(void*),0U));});});
goto _LL10B;}_LL10B:;}
# 1352
{union Cyc_CfFlowInfo_AbsLVal _tmp215=right_lval;struct Cyc_CfFlowInfo_Place*_tmp216;if((_tmp215.PlaceL).tag == 1){_LL111: _tmp216=(_tmp215.PlaceL).val;_LL112: {struct Cyc_CfFlowInfo_Place*rp=_tmp216;
# 1354
outdict=Cyc_CfFlowInfo_assign_place(fenv,e2->loc,outdict,rp,left_rval);
goto _LL110;}}else{_LL113: _LL114:
# 1357
 if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,left_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp217=0U;({unsigned _tmp6C6=e1->loc;struct _fat_ptr _tmp6C5=({const char*_tmp218="expression may not be fully initialized";_tag_fat(_tmp218,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6C6,_tmp6C5,_tag_fat(_tmp217,sizeof(void*),0U));});});
goto _LL110;}_LL110:;}
# 1362
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e2);
# 1365
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL106;}}else{_LL109: _LL10A:
 goto _LL106;}_LL106:;}
# 1372
return({struct _tuple17 _tmp5D6;_tmp5D6.f1=outflow,_tmp5D6.f2=fenv->unknown_all;_tmp5D6;});}}}}}}case 16U: _LL52: _tmp14C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp14D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL53: {struct Cyc_Absyn_Exp*eopt=_tmp14C;struct Cyc_Absyn_Exp*e2=_tmp14D;
# 1375
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(eopt != 0){
struct _tuple24 _tmp21A=({struct Cyc_NewControlFlow_AnalEnv*_tmp6C8=env;union Cyc_CfFlowInfo_FlowInfo _tmp6C7=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6C8,_tmp6C7,({struct Cyc_Absyn_Exp*_tmp21E[2U];_tmp21E[0]=eopt,_tmp21E[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple24 _stmttmp38=_tmp21A;struct _tuple24 _tmp21B=_stmttmp38;struct Cyc_List_List*_tmp21D;union Cyc_CfFlowInfo_FlowInfo _tmp21C;_LL116: _tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;_LL117: {union Cyc_CfFlowInfo_FlowInfo f=_tmp21C;struct Cyc_List_List*rvals=_tmp21D;
outflow=f;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd;}}else{
# 1382
struct _tuple17 _tmp21F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2);struct _tuple17 _stmttmp39=_tmp21F;struct _tuple17 _tmp220=_stmttmp39;void*_tmp222;union Cyc_CfFlowInfo_FlowInfo _tmp221;_LL119: _tmp221=_tmp220.f1;_tmp222=_tmp220.f2;_LL11A: {union Cyc_CfFlowInfo_FlowInfo f=_tmp221;void*r=_tmp222;
outflow=f;
place_val=r;}}
# 1387
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple17 _tmp5D7;_tmp5D7.f1=outflow,({void*_tmp6C9=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->tag=5U,_tmp223->f1=place_val;_tmp223;});_tmp5D7.f2=_tmp6C9;});_tmp5D7;});else{
# 1391
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->tag=1U,_tmp229->f1=e2,_tmp229->f2=(void*)_check_null(e->topt);_tmp229;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228->root=root,_tmp228->path=0;_tmp228;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->tag=4U,_tmp227->f1=place;_tmp227;});
union Cyc_CfFlowInfo_FlowInfo _tmp224=outflow;struct Cyc_List_List*_tmp226;struct Cyc_Dict_Dict _tmp225;if((_tmp224.BottomFL).tag == 1){_LL11C: _LL11D:
 return({struct _tuple17 _tmp5D8;_tmp5D8.f1=outflow,_tmp5D8.f2=rval;_tmp5D8;});}else{_LL11E: _tmp225=((_tmp224.ReachableFL).val).f1;_tmp226=((_tmp224.ReachableFL).val).f2;_LL11F: {struct Cyc_Dict_Dict d2=_tmp225;struct Cyc_List_List*relns=_tmp226;
# 1397
return({struct _tuple17 _tmp5D9;({union Cyc_CfFlowInfo_FlowInfo _tmp6CB=({struct Cyc_Dict_Dict _tmp6CA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6CA,relns);});_tmp5D9.f1=_tmp6CB;}),_tmp5D9.f2=rval;_tmp5D9;});}}_LL11B:;}}case 15U: _LL54: _tmp14B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp14B;
# 1402
struct _tuple19 _tmp22A=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e1);struct _tuple19 _stmttmp3A=_tmp22A;struct _tuple19 _tmp22B=_stmttmp3A;union Cyc_CfFlowInfo_AbsLVal _tmp22D;union Cyc_CfFlowInfo_FlowInfo _tmp22C;_LL121: _tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;_LL122: {union Cyc_CfFlowInfo_FlowInfo f=_tmp22C;union Cyc_CfFlowInfo_AbsLVal l=_tmp22D;
union Cyc_CfFlowInfo_AbsLVal _tmp22E=l;struct Cyc_CfFlowInfo_Place*_tmp22F;if((_tmp22E.UnknownL).tag == 2){_LL124: _LL125:
 return({struct _tuple17 _tmp5DA;_tmp5DA.f1=f,_tmp5DA.f2=fenv->notzeroall;_tmp5DA;});}else{_LL126: _tmp22F=(_tmp22E.PlaceL).val;_LL127: {struct Cyc_CfFlowInfo_Place*p=_tmp22F;
return({struct _tuple17 _tmp5DB;_tmp5DB.f1=f,({void*_tmp6CC=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->tag=4U,_tmp230->f1=p;_tmp230;});_tmp5DB.f2=_tmp6CC;});_tmp5DB;});}}_LL123:;}}case 20U: _LL56: _tmp14A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL57: {struct Cyc_Absyn_Exp*e1=_tmp14A;
# 1409
struct _tuple17 _tmp231=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp3B=_tmp231;struct _tuple17 _tmp232=_stmttmp3B;void*_tmp234;union Cyc_CfFlowInfo_FlowInfo _tmp233;_LL129: _tmp233=_tmp232.f1;_tmp234=_tmp232.f2;_LL12A: {union Cyc_CfFlowInfo_FlowInfo f=_tmp233;void*r=_tmp234;
return Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,r,0);}}case 21U: _LL58: _tmp148=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp149=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL59: {struct Cyc_Absyn_Exp*e1=_tmp148;struct _fat_ptr*field=_tmp149;
# 1413
struct _tuple17 _tmp235=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp3C=_tmp235;struct _tuple17 _tmp236=_stmttmp3C;void*_tmp238;union Cyc_CfFlowInfo_FlowInfo _tmp237;_LL12C: _tmp237=_tmp236.f1;_tmp238=_tmp236.f2;_LL12D: {union Cyc_CfFlowInfo_FlowInfo f=_tmp237;void*r=_tmp238;
void*_tmp239=(void*)_check_null(e1->topt);void*e1_type=_tmp239;
# 1416
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple17 _tmp5DC;_tmp5DC.f1=f,({void*_tmp6CD=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5DC.f2=_tmp6CD;});_tmp5DC;});
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,field,f);{
struct _tuple16 _tmp23A=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp3D=_tmp23A;struct _tuple16 _tmp23B=_stmttmp3D;void*_tmp23C;_LL12F: _tmp23C=_tmp23B.f1;_LL130: {void*r=_tmp23C;
void*_tmp23D=r;struct _fat_ptr _tmp240;int _tmp23F;int _tmp23E;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23D)->tag == 6U){_LL132: _tmp23E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23D)->f1).is_union;_tmp23F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23D)->f1).fieldnum;_tmp240=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp23D)->f2;_LL133: {int is_union=_tmp23E;int fnum=_tmp23F;struct _fat_ptr rdict=_tmp240;
# 1423
int _tmp241=Cyc_CfFlowInfo_get_field_index((void*)_check_null(e1->topt),field);int field_no=_tmp241;
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _tmp5DD;_tmp5DD.f1=f,({void*_tmp6CE=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),1,fenv->unknown_none);_tmp5DD.f2=_tmp6CE;});_tmp5DD;});
return({struct _tuple17 _tmp5DE;_tmp5DE.f1=f,_tmp5DE.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_tmp5DE;});}}else{_LL134: _LL135:
# 1428
({void*_tmp242=0U;({struct Cyc___cycFILE*_tmp6D0=Cyc_stderr;struct _fat_ptr _tmp6CF=({const char*_tmp243="the bad rexp is :";_tag_fat(_tmp243,sizeof(char),18U);});Cyc_fprintf(_tmp6D0,_tmp6CF,_tag_fat(_tmp242,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(r);
({struct Cyc_String_pa_PrintArg_struct _tmp246=({struct Cyc_String_pa_PrintArg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp6D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5DF.f1=_tmp6D1;});_tmp5DF;});void*_tmp244[1U];_tmp244[0]=& _tmp246;({struct _fat_ptr _tmp6D2=({const char*_tmp245="\nanal_Rexp: AggrMember: %s";_tag_fat(_tmp245,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D2,_tag_fat(_tmp244,sizeof(void*),1U));});});}_LL131:;}}}}case 38U: _LL5A: _tmp146=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp147=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL5B: {struct Cyc_Absyn_Exp*e1=_tmp146;struct _fat_ptr*field=_tmp147;
# 1436
struct _tuple17 _tmp247=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp3E=_tmp247;struct _tuple17 _tmp248=_stmttmp3E;void*_tmp24A;union Cyc_CfFlowInfo_FlowInfo _tmp249;_LL137: _tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;_LL138: {union Cyc_CfFlowInfo_FlowInfo f=_tmp249;void*r=_tmp24A;
# 1438
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(e1->topt)))
return({struct _tuple17 _tmp5E0;_tmp5E0.f1=f,_tmp5E0.f2=fenv->unknown_all;_tmp5E0;});{
struct _tuple16 _tmp24B=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp3F=_tmp24B;struct _tuple16 _tmp24C=_stmttmp3F;void*_tmp24D;_LL13A: _tmp24D=_tmp24C.f1;_LL13B: {void*r=_tmp24D;
void*_tmp24E=r;struct _fat_ptr _tmp251;int _tmp250;int _tmp24F;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24E)->tag == 6U){_LL13D: _tmp24F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24E)->f1).is_union;_tmp250=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24E)->f1).fieldnum;_tmp251=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24E)->f2;_LL13E: {int is_union=_tmp24F;int fnum=_tmp250;struct _fat_ptr rdict=_tmp251;
# 1443
int _tmp252=Cyc_CfFlowInfo_get_field_index((void*)_check_null(e1->topt),field);int field_no=_tmp252;
if(is_union && fnum != -1){
if(fnum != field_no)
return({struct _tuple17 _tmp5E1;_tmp5E1.f1=f,_tmp5E1.f2=fenv->zero;_tmp5E1;});else{
# 1448
return({struct _tuple17 _tmp5E2;_tmp5E2.f1=f,_tmp5E2.f2=fenv->notzeroall;_tmp5E2;});}}else{
# 1450
return({struct _tuple17 _tmp5E3;_tmp5E3.f1=f,_tmp5E3.f2=fenv->unknown_all;_tmp5E3;});}}}else{_LL13F: _LL140:
# 1452
({struct Cyc_String_pa_PrintArg_struct _tmp255=({struct Cyc_String_pa_PrintArg_struct _tmp5E4;_tmp5E4.tag=0U,({struct _fat_ptr _tmp6D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5E4.f1=_tmp6D3;});_tmp5E4;});void*_tmp253[1U];_tmp253[0]=& _tmp255;({struct _fat_ptr _tmp6D4=({const char*_tmp254="anal_Rexp: TagCheck_e: %s";_tag_fat(_tmp254,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D4,_tag_fat(_tmp253,sizeof(void*),1U));});});}_LL13C:;}}}}case 22U: _LL5C: _tmp144=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp145=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL5D: {struct Cyc_Absyn_Exp*e1=_tmp144;struct _fat_ptr*field=_tmp145;
# 1456
struct _tuple17 _tmp256=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp40=_tmp256;struct _tuple17 _tmp257=_stmttmp40;void*_tmp259;union Cyc_CfFlowInfo_FlowInfo _tmp258;_LL142: _tmp258=_tmp257.f1;_tmp259=_tmp257.f2;_LL143: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp258;void*r1=_tmp259;
# 1459
struct _tuple17 _tmp25A=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _stmttmp41=_tmp25A;struct _tuple17 _tmp25B=_stmttmp41;void*_tmp25D;union Cyc_CfFlowInfo_FlowInfo _tmp25C;_LL145: _tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;_LL146: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp25C;void*r2=_tmp25D;
# 1462
void*_tmp25E=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp42=_tmp25E;void*_tmp25F=_stmttmp42;void*_tmp260;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25F)->tag == 3U){_LL148: _tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25F)->f1).elt_type;_LL149: {void*t2=_tmp260;
# 1464
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t2))
# 1466
return({struct _tuple17 _tmp5E5;_tmp5E5.f1=f2,({void*_tmp6D5=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp5E5.f2=_tmp6D5;});_tmp5E5;});
# 1468
if(Cyc_Absyn_is_require_union_type(t2))
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,field,f1);{
# 1471
struct _tuple16 _tmp261=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _stmttmp43=_tmp261;struct _tuple16 _tmp262=_stmttmp43;void*_tmp263;_LL14D: _tmp263=_tmp262.f1;_LL14E: {void*r2=_tmp263;
void*_tmp264=r2;struct _fat_ptr _tmp267;int _tmp266;int _tmp265;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp264)->tag == 6U){_LL150: _tmp265=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp264)->f1).is_union;_tmp266=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp264)->f1).fieldnum;_tmp267=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp264)->f2;_LL151: {int is_union=_tmp265;int fnum=_tmp266;struct _fat_ptr rdict=_tmp267;
# 1474
int _tmp268=Cyc_CfFlowInfo_get_field_index(t2,field);int field_no=_tmp268;
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _tmp5E6;_tmp5E6.f1=f2,({void*_tmp6D6=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),1,fenv->unknown_none);_tmp5E6.f2=_tmp6D6;});_tmp5E6;});
return({struct _tuple17 _tmp5E7;_tmp5E7.f1=f2,_tmp5E7.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_tmp5E7;});}}else{_LL152: _LL153:
({void*_tmp269=0U;({struct _fat_ptr _tmp6D7=({const char*_tmp26A="anal_Rexp: AggrArrow";_tag_fat(_tmp26A,sizeof(char),21U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D7,_tag_fat(_tmp269,sizeof(void*),0U));});});}_LL14F:;}}}}else{_LL14A: _LL14B:
# 1480
({void*_tmp26B=0U;({struct _fat_ptr _tmp6D8=({const char*_tmp26C="anal_Rexp: AggrArrow ptr";_tag_fat(_tmp26C,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D8,_tag_fat(_tmp26B,sizeof(void*),0U));});});}_LL147:;}}}case 6U: _LL5E: _tmp141=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp142=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_tmp143=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL5F: {struct Cyc_Absyn_Exp*e1=_tmp141;struct Cyc_Absyn_Exp*e2=_tmp142;struct Cyc_Absyn_Exp*e3=_tmp143;
# 1484
struct _tuple20 _tmp26D=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp44=_tmp26D;struct _tuple20 _tmp26E=_stmttmp44;union Cyc_CfFlowInfo_FlowInfo _tmp270;union Cyc_CfFlowInfo_FlowInfo _tmp26F;_LL155: _tmp26F=_tmp26E.f1;_tmp270=_tmp26E.f2;_LL156: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp26F;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp270;
struct _tuple17 _tmp271=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2);struct _tuple17 pr2=_tmp271;
struct _tuple17 _tmp272=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e3);struct _tuple17 pr3=_tmp272;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}case 7U: _LL60: _tmp13F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp140=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL61: {struct Cyc_Absyn_Exp*e1=_tmp13F;struct Cyc_Absyn_Exp*e2=_tmp140;
# 1490
struct _tuple20 _tmp273=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp45=_tmp273;struct _tuple20 _tmp274=_stmttmp45;union Cyc_CfFlowInfo_FlowInfo _tmp276;union Cyc_CfFlowInfo_FlowInfo _tmp275;_LL158: _tmp275=_tmp274.f1;_tmp276=_tmp274.f2;_LL159: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp275;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp276;
struct _tuple17 _tmp277=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2);struct _tuple17 _stmttmp46=_tmp277;struct _tuple17 _tmp278=_stmttmp46;void*_tmp27A;union Cyc_CfFlowInfo_FlowInfo _tmp279;_LL15B: _tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;_LL15C: {union Cyc_CfFlowInfo_FlowInfo f2t=_tmp279;void*f2r=_tmp27A;
struct _tuple17 _tmp27B=({struct _tuple17 _tmp5E9;_tmp5E9.f1=f2t,_tmp5E9.f2=f2r;_tmp5E9;});struct _tuple17 pr2=_tmp27B;
struct _tuple17 _tmp27C=({struct _tuple17 _tmp5E8;_tmp5E8.f1=f1f,_tmp5E8.f2=fenv->zero;_tmp5E8;});struct _tuple17 pr3=_tmp27C;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 8U: _LL62: _tmp13D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp13E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL63: {struct Cyc_Absyn_Exp*e1=_tmp13D;struct Cyc_Absyn_Exp*e2=_tmp13E;
# 1497
struct _tuple20 _tmp27D=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp47=_tmp27D;struct _tuple20 _tmp27E=_stmttmp47;union Cyc_CfFlowInfo_FlowInfo _tmp280;union Cyc_CfFlowInfo_FlowInfo _tmp27F;_LL15E: _tmp27F=_tmp27E.f1;_tmp280=_tmp27E.f2;_LL15F: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp27F;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp280;
struct _tuple17 _tmp281=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e2);struct _tuple17 _stmttmp48=_tmp281;struct _tuple17 _tmp282=_stmttmp48;void*_tmp284;union Cyc_CfFlowInfo_FlowInfo _tmp283;_LL161: _tmp283=_tmp282.f1;_tmp284=_tmp282.f2;_LL162: {union Cyc_CfFlowInfo_FlowInfo f2t=_tmp283;void*f2r=_tmp284;
struct _tuple17 _tmp285=({struct _tuple17 _tmp5EB;_tmp5EB.f1=f2t,_tmp5EB.f2=f2r;_tmp5EB;});struct _tuple17 pr2=_tmp285;
struct _tuple17 _tmp286=({struct _tuple17 _tmp5EA;_tmp5EA.f1=f1t,_tmp5EA.f2=fenv->notzeroall;_tmp5EA;});struct _tuple17 pr3=_tmp286;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 23U: _LL64: _tmp13B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp13C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL65: {struct Cyc_Absyn_Exp*e1=_tmp13B;struct Cyc_Absyn_Exp*e2=_tmp13C;
# 1504
struct _tuple24 _tmp287=({struct Cyc_NewControlFlow_AnalEnv*_tmp6DA=env;union Cyc_CfFlowInfo_FlowInfo _tmp6D9=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6DA,_tmp6D9,({struct Cyc_Absyn_Exp*_tmp2A3[2U];_tmp2A3[0]=e1,_tmp2A3[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2A3,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple24 _stmttmp49=_tmp287;struct _tuple24 _tmp288=_stmttmp49;struct Cyc_List_List*_tmp28A;union Cyc_CfFlowInfo_FlowInfo _tmp289;_LL164: _tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;_LL165: {union Cyc_CfFlowInfo_FlowInfo f=_tmp289;struct Cyc_List_List*rvals=_tmp28A;
# 1508
union Cyc_CfFlowInfo_FlowInfo _tmp28B=f;union Cyc_CfFlowInfo_FlowInfo f2=_tmp28B;
{union Cyc_CfFlowInfo_FlowInfo _tmp28C=f;struct Cyc_List_List*_tmp28E;struct Cyc_Dict_Dict _tmp28D;if((_tmp28C.ReachableFL).tag == 2){_LL167: _tmp28D=((_tmp28C.ReachableFL).val).f1;_tmp28E=((_tmp28C.ReachableFL).val).f2;_LL168: {struct Cyc_Dict_Dict d2=_tmp28D;struct Cyc_List_List*relns=_tmp28E;
# 1513
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp28F=0U;({unsigned _tmp6DC=e2->loc;struct _fat_ptr _tmp6DB=({const char*_tmp290="expression may not be initialized";_tag_fat(_tmp290,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6DC,_tmp6DB,_tag_fat(_tmp28F,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp291=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);struct Cyc_List_List*new_relns=_tmp291;
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL166;}}}else{_LL169: _LL16A:
 goto _LL166;}_LL166:;}{
# 1521
void*_tmp292=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp4A=_tmp292;void*_tmp293=_stmttmp4A;struct Cyc_List_List*_tmp294;switch(*((int*)_tmp293)){case 6U: _LL16C: _tmp294=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp293)->f1;_LL16D: {struct Cyc_List_List*tqts=_tmp294;
# 1523
struct _tuple16 _tmp295=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(rvals))->hd);struct _tuple16 _stmttmp4B=_tmp295;struct _tuple16 _tmp296=_stmttmp4B;void*_tmp297;_LL173: _tmp297=_tmp296.f1;_LL174: {void*r=_tmp297;
void*_tmp298=r;struct _fat_ptr _tmp299;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp298)->tag == 6U){_LL176: _tmp299=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp298)->f2;_LL177: {struct _fat_ptr rdict=_tmp299;
# 1526
unsigned i=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;
return({struct _tuple17 _tmp5EC;_tmp5EC.f1=f2,_tmp5EC.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),(int)i));_tmp5EC;});}}else{_LL178: _LL179:
({void*_tmp29A=0U;({struct _fat_ptr _tmp6DD=({const char*_tmp29B="anal_Rexp: Subscript";_tag_fat(_tmp29B,sizeof(char),21U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6DD,_tag_fat(_tmp29A,sizeof(void*),0U));});});}_LL175:;}}case 3U: _LL16E: _LL16F: {
# 1531
struct _tuple17 _tmp29C=Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,(void*)((struct Cyc_List_List*)_check_null(rvals))->hd,e2);struct _tuple17 _stmttmp4C=_tmp29C;struct _tuple17 _tmp29D=_stmttmp4C;void*_tmp29F;union Cyc_CfFlowInfo_FlowInfo _tmp29E;_LL17B: _tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;_LL17C: {union Cyc_CfFlowInfo_FlowInfo f3=_tmp29E;void*r=_tmp29F;
union Cyc_CfFlowInfo_FlowInfo _tmp2A0=f3;if((_tmp2A0.BottomFL).tag == 1){_LL17E: _LL17F:
 return({struct _tuple17 _tmp5ED;_tmp5ED.f1=f3,_tmp5ED.f2=r;_tmp5ED;});}else{_LL180: _LL181:
 return({struct _tuple17 _tmp5EE;_tmp5EE.f1=f2,_tmp5EE.f2=r;_tmp5EE;});}_LL17D:;}}default: _LL170: _LL171:
# 1536
({void*_tmp2A1=0U;({struct _fat_ptr _tmp6DE=({const char*_tmp2A2="anal_Rexp: Subscript -- bad type";_tag_fat(_tmp2A2,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6DE,_tag_fat(_tmp2A1,sizeof(void*),0U));});});}_LL16B:;}}}case 24U: _LL68: _tmp13A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL69: {struct Cyc_List_List*es=_tmp13A;
# 1542
struct _tuple24 _tmp2A4=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp4D=_tmp2A4;struct _tuple24 _tmp2A5=_stmttmp4D;struct Cyc_List_List*_tmp2A7;union Cyc_CfFlowInfo_FlowInfo _tmp2A6;_LL183: _tmp2A6=_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;_LL184: {union Cyc_CfFlowInfo_FlowInfo f=_tmp2A6;struct Cyc_List_List*rvals=_tmp2A7;
struct _fat_ptr aggrdict=({unsigned _tmp2AB=(unsigned)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);void**_tmp2AA=_cycalloc(_check_times(_tmp2AB,sizeof(void*)));({{unsigned _tmp5F0=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);unsigned i;for(i=0;i < _tmp5F0;++ i){({
void*_tmp6DF=({void*_tmp2A9=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;void*temp=_tmp2A9;
rvals=rvals->tl;
temp;});
# 1545
_tmp2AA[i]=_tmp6DF;});}}0;});_tag_fat(_tmp2AA,sizeof(void*),_tmp2AB);});
# 1549
return({struct _tuple17 _tmp5EF;_tmp5EF.f1=f,({void*_tmp6E0=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->tag=6U,(_tmp2A8->f1).is_union=0,(_tmp2A8->f1).fieldnum=- 1,_tmp2A8->f2=aggrdict;_tmp2A8;});_tmp5EF.f2=_tmp6E0;});_tmp5EF;});}}case 30U: _LL6A: _tmp139=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_LL6B: {struct Cyc_List_List*des=_tmp139;
# 1552
struct Cyc_List_List*fs;
{void*_tmp2AC=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp4E=_tmp2AC;void*_tmp2AD=_stmttmp4E;struct Cyc_List_List*_tmp2AE;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2AD)->tag == 7U){_LL186: _tmp2AE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2AD)->f2;_LL187: {struct Cyc_List_List*f=_tmp2AE;
fs=f;goto _LL185;}}else{_LL188: _LL189:
({void*_tmp2AF=0U;({struct _fat_ptr _tmp6E1=({const char*_tmp2B0="anal_Rexp:anon struct has bad type";_tag_fat(_tmp2B0,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6E1,_tag_fat(_tmp2AF,sizeof(void*),0U));});});}_LL185:;}
# 1557
_tmp136=des;_tmp137=0U;_tmp138=fs;goto _LL6D;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4)->impl != 0){_LL6C: _tmp136=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_tmp137=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4)->kind;_tmp138=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12C)->f4)->impl)->fields;_LL6D: {struct Cyc_List_List*des=_tmp136;enum Cyc_Absyn_AggrKind kind=_tmp137;struct Cyc_List_List*fs=_tmp138;
# 1559
void*exp_type=(void*)_check_null(e->topt);
struct _tuple24 _tmp2B1=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E3=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E2=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E3,_tmp6E2,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple27*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd,des),1,1);});struct _tuple24 _stmttmp4F=_tmp2B1;struct _tuple24 _tmp2B2=_stmttmp4F;struct Cyc_List_List*_tmp2B4;union Cyc_CfFlowInfo_FlowInfo _tmp2B3;_LL18B: _tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_LL18C: {union Cyc_CfFlowInfo_FlowInfo f=_tmp2B3;struct Cyc_List_List*rvals=_tmp2B4;
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)kind == (int)Cyc_Absyn_UnionA,fenv->unknown_all);
# 1564
int field_no=-1;
{int i=0;for(0;rvals != 0;(rvals=rvals->tl,des=des->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple27*)((struct Cyc_List_List*)_check_null(des))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2B5=(void*)ds->hd;void*_stmttmp50=_tmp2B5;void*_tmp2B6=_stmttmp50;struct _fat_ptr*_tmp2B7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2B6)->tag == 0U){_LL18E: _LL18F:
({void*_tmp2B8=0U;({struct _fat_ptr _tmp6E4=({const char*_tmp2B9="anal_Rexp:Aggregate_e";_tag_fat(_tmp2B9,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6E4,_tag_fat(_tmp2B8,sizeof(void*),0U));});});}else{_LL190: _tmp2B7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2B6)->f1;_LL191: {struct _fat_ptr*fld=_tmp2B7;
# 1571
field_no=Cyc_CfFlowInfo_get_field_index_fs(fs,fld);
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),field_no))=(void*)rvals->hd;
# 1574
if((int)kind == (int)1U){
struct Cyc_Absyn_Exp*_tmp2BA=(*((struct _tuple27*)des->hd)).f2;struct Cyc_Absyn_Exp*e=_tmp2BA;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,(void*)rvals->hd);
# 1578
Cyc_NewControlFlow_check_union_requires(e->loc,exp_type,fld,f);}}}_LL18D:;}}}{
# 1581
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2BB=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->tag=6U,(_tmp2BC->f1).is_union=(int)kind == (int)Cyc_Absyn_UnionA,(_tmp2BC->f1).fieldnum=field_no,_tmp2BC->f2=aggrdict;_tmp2BC;});struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*res=_tmp2BB;
return({struct _tuple17 _tmp5F1;_tmp5F1.f1=f,_tmp5F1.f2=(void*)res;_tmp5F1;});}}}}else{goto _LL6E;}}else{_LL6E: _LL6F:
# 1585
({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp6E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp5F2.f1=_tmp6E5;});_tmp5F2;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct _fat_ptr _tmp6E6=({const char*_tmp2BE="anal_Rexp:missing aggrdeclimpl in %s";_tag_fat(_tmp2BE,sizeof(char),37U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6E6,_tag_fat(_tmp2BD,sizeof(void*),1U));});});}case 26U: _LL70: _tmp135=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL71: {struct Cyc_List_List*dles=_tmp135;
# 1587
struct Cyc_List_List*_tmp2C0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple27*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd,dles);struct Cyc_List_List*es=_tmp2C0;
struct _tuple24 _tmp2C1=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp51=_tmp2C1;struct _tuple24 _tmp2C2=_stmttmp51;struct Cyc_List_List*_tmp2C4;union Cyc_CfFlowInfo_FlowInfo _tmp2C3;_LL193: _tmp2C3=_tmp2C2.f1;_tmp2C4=_tmp2C2.f2;_LL194: {union Cyc_CfFlowInfo_FlowInfo outflow=_tmp2C3;struct Cyc_List_List*rvals=_tmp2C4;
for(0;rvals != 0;(rvals=rvals->tl,es=es->tl)){
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);}
# 1592
return({struct _tuple17 _tmp5F3;_tmp5F3.f1=outflow,({void*_tmp6E7=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->notzeroall);_tmp5F3.f2=_tmp6E7;});_tmp5F3;});}}case 27U: _LL72: _tmp131=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp132=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_tmp133=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_tmp134=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12C)->f4;_LL73: {struct Cyc_Absyn_Vardecl*vd=_tmp131;struct Cyc_Absyn_Exp*e1=_tmp132;struct Cyc_Absyn_Exp*e2=_tmp133;int zt=_tmp134;
# 1596
struct _tuple17 _tmp2C5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1);struct _tuple17 _stmttmp52=_tmp2C5;struct _tuple17 _tmp2C6=_stmttmp52;void*_tmp2C8;union Cyc_CfFlowInfo_FlowInfo _tmp2C7;_LL196: _tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;_LL197: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp2C7;void*r1=_tmp2C8;
union Cyc_CfFlowInfo_FlowInfo _tmp2C9=f1;struct Cyc_List_List*_tmp2CB;struct Cyc_Dict_Dict _tmp2CA;if((_tmp2C9.BottomFL).tag == 1){_LL199: _LL19A:
 return({struct _tuple17 _tmp5F4;_tmp5F4.f1=f1,_tmp5F4.f2=fenv->unknown_all;_tmp5F4;});}else{_LL19B: _tmp2CA=((_tmp2C9.ReachableFL).val).f1;_tmp2CB=((_tmp2C9.ReachableFL).val).f2;_LL19C: {struct Cyc_Dict_Dict d1=_tmp2CA;struct Cyc_List_List*relns=_tmp2CB;
# 1600
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d1,r1)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp2CC=0U;({unsigned _tmp6E9=e1->loc;struct _fat_ptr _tmp6E8=({const char*_tmp2CD="expression may not be initialized";_tag_fat(_tmp2CD,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6E9,_tmp6E8,_tag_fat(_tmp2CC,sizeof(void*),0U));});});{
# 1604
struct Cyc_List_List*new_relns=relns;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(vd);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& n2))
new_relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,relns);
if(relns != new_relns)
f1=Cyc_CfFlowInfo_ReachableFL(d1,new_relns);{
# 1613
void*_tmp2CE=r1;switch(*((int*)_tmp2CE)){case 0U: _LL19E: _LL19F:
 return({struct _tuple17 _tmp5F5;_tmp5F5.f1=f1,_tmp5F5.f2=fenv->unknown_all;_tmp5F5;});case 1U: _LL1A0: _LL1A1:
 goto _LL1A3;case 4U: _LL1A2: _LL1A3: {
# 1617
struct Cyc_List_List _tmp2CF=({struct Cyc_List_List _tmp5F8;_tmp5F8.hd=vd,_tmp5F8.tl=0;_tmp5F8;});struct Cyc_List_List l=_tmp2CF;
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
# 1620
struct _tuple17 _tmp2D0=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2);struct _tuple17 _stmttmp53=_tmp2D0;struct _tuple17 _tmp2D1=_stmttmp53;void*_tmp2D3;union Cyc_CfFlowInfo_FlowInfo _tmp2D2;_LL1A7: _tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;_LL1A8: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp2D2;void*r2=_tmp2D3;
{union Cyc_CfFlowInfo_FlowInfo _tmp2D4=f2;struct Cyc_Dict_Dict _tmp2D5;if((_tmp2D4.BottomFL).tag == 1){_LL1AA: _LL1AB:
 return({struct _tuple17 _tmp5F6;_tmp5F6.f1=f2,_tmp5F6.f2=fenv->unknown_all;_tmp5F6;});}else{_LL1AC: _tmp2D5=((_tmp2D4.ReachableFL).val).f1;_LL1AD: {struct Cyc_Dict_Dict d2=_tmp2D5;
# 1624
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp2D6=0U;({unsigned _tmp6EB=e1->loc;struct _fat_ptr _tmp6EA=({const char*_tmp2D7="expression may not be initialized";_tag_fat(_tmp2D7,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6EB,_tmp6EA,_tag_fat(_tmp2D6,sizeof(void*),0U));});});
return({struct _tuple17 _tmp5F7;({union Cyc_CfFlowInfo_FlowInfo _tmp6EC=Cyc_CfFlowInfo_BottomFL();_tmp5F7.f1=_tmp6EC;}),_tmp5F7.f2=fenv->unknown_all;_tmp5F7;});}}}_LL1A9:;}
# 1629
f1=f2;
goto _LL1A5;}}}default: _LL1A4: _LL1A5:
# 1632
 while(1){
struct Cyc_List_List _tmp2D8=({struct Cyc_List_List _tmp5FA;_tmp5FA.hd=vd,_tmp5FA.tl=0;_tmp5FA;});struct Cyc_List_List l=_tmp2D8;
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
struct _tuple17 _tmp2D9=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2);struct _tuple17 _stmttmp54=_tmp2D9;struct _tuple17 _tmp2DA=_stmttmp54;void*_tmp2DC;union Cyc_CfFlowInfo_FlowInfo _tmp2DB;_LL1AF: _tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;_LL1B0: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp2DB;void*r2=_tmp2DC;
{union Cyc_CfFlowInfo_FlowInfo _tmp2DD=f2;struct Cyc_Dict_Dict _tmp2DE;if((_tmp2DD.BottomFL).tag == 1){_LL1B2: _LL1B3:
 goto _LL1B1;}else{_LL1B4: _tmp2DE=((_tmp2DD.ReachableFL).val).f1;_LL1B5: {struct Cyc_Dict_Dict d2=_tmp2DE;
# 1639
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp2DF=0U;({unsigned _tmp6EE=e1->loc;struct _fat_ptr _tmp6ED=({const char*_tmp2E0="expression may not be initialized";_tag_fat(_tmp2E0,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6EE,_tmp6ED,_tag_fat(_tmp2DF,sizeof(void*),0U));});});
return({struct _tuple17 _tmp5F9;({union Cyc_CfFlowInfo_FlowInfo _tmp6EF=Cyc_CfFlowInfo_BottomFL();_tmp5F9.f1=_tmp6EF;}),_tmp5F9.f2=fenv->unknown_all;_tmp5F9;});}}}_LL1B1:;}{
# 1644
union Cyc_CfFlowInfo_FlowInfo _tmp2E1=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);union Cyc_CfFlowInfo_FlowInfo newflow=_tmp2E1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,f1))
break;
f1=newflow;}}}}
# 1649
return({struct _tuple17 _tmp5FB;_tmp5FB.f1=f1,_tmp5FB.f2=fenv->notzeroall;_tmp5FB;});}_LL19D:;}}}}_LL198:;}}case 28U: _LL74: _tmp12E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_tmp12F=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C)->f2;_tmp130=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp12C)->f3;_LL75: {struct Cyc_Absyn_Exp*exp=_tmp12E;void*typ=_tmp12F;int iszeroterm=_tmp130;
# 1655
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->tag=1U,_tmp2EA->f1=exp,_tmp2EA->f2=(void*)_check_null(e->topt);_tmp2EA;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->root=root,_tmp2E9->path=0;_tmp2E9;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8->tag=4U,_tmp2E8->f1=place;_tmp2E8;});
void*place_val;
# 1663
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,typ,0,fenv->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple17 _tmp2E2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp);struct _tuple17 _stmttmp55=_tmp2E2;struct _tuple17 _tmp2E3=_stmttmp55;union Cyc_CfFlowInfo_FlowInfo _tmp2E4;_LL1B7: _tmp2E4=_tmp2E3.f1;_LL1B8: {union Cyc_CfFlowInfo_FlowInfo f=_tmp2E4;
outflow=f;{
union Cyc_CfFlowInfo_FlowInfo _tmp2E5=outflow;struct Cyc_List_List*_tmp2E7;struct Cyc_Dict_Dict _tmp2E6;if((_tmp2E5.BottomFL).tag == 1){_LL1BA: _LL1BB:
 return({struct _tuple17 _tmp5FC;_tmp5FC.f1=outflow,_tmp5FC.f2=rval;_tmp5FC;});}else{_LL1BC: _tmp2E6=((_tmp2E5.ReachableFL).val).f1;_tmp2E7=((_tmp2E5.ReachableFL).val).f2;_LL1BD: {struct Cyc_Dict_Dict d2=_tmp2E6;struct Cyc_List_List*relns=_tmp2E7;
# 1670
return({struct _tuple17 _tmp5FD;({union Cyc_CfFlowInfo_FlowInfo _tmp6F1=({struct Cyc_Dict_Dict _tmp6F0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6F0,relns);});_tmp5FD.f1=_tmp6F1;}),_tmp5FD.f2=rval;_tmp5FD;});}}_LL1B9:;}}}}case 37U: _LL76: _tmp12D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12C)->f1;_LL77: {struct Cyc_Absyn_Stmt*s=_tmp12D;
# 1674
struct _tuple18 _tmp2EB=({struct _tuple18 _tmp5FF;_tmp5FF.f1=(env->fenv)->unknown_all,_tmp5FF.f2=copy_ctxt;_tmp5FF;});struct _tuple18 rval_opt=_tmp2EB;
union Cyc_CfFlowInfo_FlowInfo _tmp2EC=Cyc_NewControlFlow_anal_stmt(env,inflow,s,& rval_opt);union Cyc_CfFlowInfo_FlowInfo f=_tmp2EC;
return({struct _tuple17 _tmp5FE;_tmp5FE.f1=f,_tmp5FE.f2=rval_opt.f1;_tmp5FE;});}case 36U: _LL7A: _LL7B:
# 1679
 goto _LL7D;case 25U: _LL7C: _LL7D:
 goto _LL7F;default: _LL7E: _LL7F:
# 1682
({void*_tmp2ED=0U;({struct _fat_ptr _tmp6F2=({const char*_tmp2EE="anal_Rexp, unexpected exp form";_tag_fat(_tmp2EE,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6F2,_tag_fat(_tmp2ED,sizeof(void*),0U));});});}_LLD:;}}
# 1694 "/tmp/cyclone/src/new_control_flow.cyc"
static struct _tuple19 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1703
struct Cyc_CfFlowInfo_FlowEnv*_tmp2EF=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp2EF;
void*_tmp2F0=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp56=_tmp2F0;void*_tmp2F1=_stmttmp56;void*_tmp2F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->tag == 3U){_LL1: _tmp2F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).bounds;_LL2: {void*bd=_tmp2F2;
# 1706
union Cyc_CfFlowInfo_FlowInfo _tmp2F3=f;struct Cyc_List_List*_tmp2F5;struct Cyc_Dict_Dict _tmp2F4;if((_tmp2F3.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple19 _tmp600;_tmp600.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp6F3=Cyc_CfFlowInfo_UnknownL();_tmp600.f2=_tmp6F3;});_tmp600;});}else{_LL8: _tmp2F4=((_tmp2F3.ReachableFL).val).f1;_tmp2F5=((_tmp2F3.ReachableFL).val).f2;_LL9: {struct Cyc_Dict_Dict outdict=_tmp2F4;struct Cyc_List_List*relns=_tmp2F5;
# 1710
struct _tuple16 _tmp2F6=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp57=_tmp2F6;struct _tuple16 _tmp2F7=_stmttmp57;struct Cyc_List_List*_tmp2F9;void*_tmp2F8;_LLB: _tmp2F8=_tmp2F7.f1;_tmp2F9=_tmp2F7.f2;_LLC: {void*r=_tmp2F8;struct Cyc_List_List*names=_tmp2F9;
retry: {void*_tmp2FA=r;void*_tmp2FB;enum Cyc_CfFlowInfo_InitLevel _tmp2FC;struct Cyc_List_List*_tmp2FE;void*_tmp2FD;void*_tmp2FF;switch(*((int*)_tmp2FA)){case 8U: _LLE: _LLF:
# 1713
({void*_tmp300=0U;({struct _fat_ptr _tmp6F4=({const char*_tmp301="named location in anal_derefL";_tag_fat(_tmp301,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp6F4,_tag_fat(_tmp300,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1715
({void*_tmp6F5=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302->tag=Cyc_CfFlowInfo_NotZero,_tmp302->f1=relns;_tmp302;});e->annot=_tmp6F5;});goto _LLD;case 5U: _LL12: _tmp2FF=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2FA)->f1;_LL13: {void*rv=_tmp2FF;
# 1718
if(expanded_place_possibly_null)
# 1721
({void*_tmp6F6=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->tag=Cyc_CfFlowInfo_UnknownZ,_tmp303->f1=relns;_tmp303;});e->annot=_tmp6F6;});else{
# 1724
void*_tmp304=e->annot;void*_stmttmp58=_tmp304;void*_tmp305=_stmttmp58;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp305)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1728
({void*_tmp6F7=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->tag=Cyc_CfFlowInfo_UnknownZ,_tmp306->f1=relns;_tmp306;});e->annot=_tmp6F7;});
goto _LL1E;}else{_LL21: _LL22:
# 1731
({void*_tmp6F8=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307->tag=Cyc_CfFlowInfo_NotZero,_tmp307->f1=relns;_tmp307;});e->annot=_tmp6F8;});
goto _LL1E;}_LL1E:;}
# 1736
if(Cyc_Tcutil_is_bound_one(bd))return({struct _tuple19 _tmp601;_tmp601.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp6F9=Cyc_CfFlowInfo_UnknownL();_tmp601.f2=_tmp6F9;});_tmp601;});
goto _LLD;}case 4U: _LL14: _tmp2FD=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2FA)->f1)->root;_tmp2FE=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2FA)->f1)->path;_LL15: {void*root=_tmp2FD;struct Cyc_List_List*path2=_tmp2FE;
# 1740
({void*_tmp6FA=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->tag=Cyc_CfFlowInfo_NotZero,_tmp308->f1=relns;_tmp308;});e->annot=_tmp6FA;});
if(Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple19 _tmp602;_tmp602.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp6FC=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->root=root,({struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(path2,path);_tmp309->path=_tmp6FB;});_tmp309;}));_tmp602.f2=_tmp6FC;});_tmp602;});
goto _LLD;}case 0U: _LL16: _LL17:
# 1745
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple19 _tmp603;({union Cyc_CfFlowInfo_FlowInfo _tmp6FE=Cyc_CfFlowInfo_BottomFL();_tmp603.f1=_tmp6FE;}),({union Cyc_CfFlowInfo_AbsLVal _tmp6FD=Cyc_CfFlowInfo_UnknownL();_tmp603.f2=_tmp6FD;});_tmp603;});case 2U: _LL18: _tmp2FC=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2FA)->f1;_LL19: {enum Cyc_CfFlowInfo_InitLevel il=_tmp2FC;
# 1750
if(Cyc_Tcutil_is_bound_one(bd))
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
({void*_tmp6FF=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->tag=Cyc_CfFlowInfo_UnknownZ,_tmp30A->f1=relns;_tmp30A;});e->annot=_tmp6FF;});
goto _LLD;}case 7U: _LL1A: _tmp2FB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2FA)->f3;_LL1B: {void*r2=_tmp2FB;
# 1755
if(passthrough_consumes){
r=r2;goto retry;}
# 1758
goto _LL1D;}default: _LL1C: _LL1D:
# 1760
({void*_tmp700=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->tag=Cyc_CfFlowInfo_UnknownZ,_tmp30B->f1=relns;_tmp30B;});e->annot=_tmp700;});
goto _LLD;}_LLD:;}
# 1763
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,r)== (int)Cyc_CfFlowInfo_NoneIL){
struct _tuple16 _tmp30C=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp59=_tmp30C;struct _tuple16 _tmp30D=_stmttmp59;void*_tmp30E;_LL24: _tmp30E=_tmp30D.f1;_LL25: {void*r=_tmp30E;
void*_tmp30F=r;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp30F)->tag == 7U){_LL27: _LL28:
# 1767
({void*_tmp310=0U;({unsigned _tmp702=e->loc;struct _fat_ptr _tmp701=({const char*_tmp311="attempt to dereference an alias-free that has already been copied";_tag_fat(_tmp311,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp702,_tmp701,_tag_fat(_tmp310,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1770
({void*_tmp312=0U;({unsigned _tmp704=e->loc;struct _fat_ptr _tmp703=({const char*_tmp313="dereference of possibly uninitialized pointer";_tag_fat(_tmp313,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp704,_tmp703,_tag_fat(_tmp312,sizeof(void*),0U));});});
goto _LL26;}_LL26:;}}
# 1774
return({struct _tuple19 _tmp604;_tmp604.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp705=Cyc_CfFlowInfo_UnknownL();_tmp604.f2=_tmp705;});_tmp604;});}}}_LL5:;}}else{_LL3: _LL4:
# 1776
({void*_tmp314=0U;({struct _fat_ptr _tmp706=({const char*_tmp315="left deref of non-pointer-type";_tag_fat(_tmp315,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp706,_tag_fat(_tmp314,sizeof(void*),0U));});});}_LL0:;}
# 1782
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp316=p;struct Cyc_List_List*_tmp318;void*_tmp317;_LL1: _tmp317=_tmp316->root;_tmp318=_tmp316->path;_LL2: {void*proot=_tmp317;struct Cyc_List_List*ppath=_tmp318;
path=({struct Cyc_List_List*_tmp31A=_cycalloc(sizeof(*_tmp31A));({void*_tmp707=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->tag=1U;_tmp319;});_tmp31A->hd=_tmp707;}),_tmp31A->tl=path;_tmp31A;});
return({struct Cyc_CfFlowInfo_Place*_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B->root=proot,({struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ppath,path);_tmp31B->path=_tmp708;});_tmp31B;});}}
# 1792
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1796
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp31C=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp31C;
{union Cyc_CfFlowInfo_FlowInfo _tmp31D=inflow;struct Cyc_List_List*_tmp31F;struct Cyc_Dict_Dict _tmp31E;if((_tmp31D.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple19 _tmp605;({union Cyc_CfFlowInfo_FlowInfo _tmp70A=Cyc_CfFlowInfo_BottomFL();_tmp605.f1=_tmp70A;}),({union Cyc_CfFlowInfo_AbsLVal _tmp709=Cyc_CfFlowInfo_UnknownL();_tmp605.f2=_tmp709;});_tmp605;});}else{_LL3: _tmp31E=((_tmp31D.ReachableFL).val).f1;_tmp31F=((_tmp31D.ReachableFL).val).f2;_LL4: {struct Cyc_Dict_Dict d2=_tmp31E;struct Cyc_List_List*relns=_tmp31F;
# 1801
d=d2;}}_LL0:;}{
# 1805
void*_tmp320=e->r;void*_stmttmp5A=_tmp320;void*_tmp321=_stmttmp5A;struct _fat_ptr*_tmp323;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*_tmp326;struct _fat_ptr*_tmp328;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Vardecl*_tmp329;struct Cyc_Absyn_Vardecl*_tmp32A;struct Cyc_Absyn_Vardecl*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;switch(*((int*)_tmp321)){case 14U: _LL6: _tmp32F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp32F;
_tmp32E=e1;goto _LL9;}case 12U: _LL8: _tmp32E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp32E;
_tmp32D=e1;goto _LLB;}case 13U: _LLA: _tmp32D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LLB: {struct Cyc_Absyn_Exp*e1=_tmp32D;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e1,path);}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)){case 4U: _LLC: _tmp32C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LLD: {struct Cyc_Absyn_Vardecl*vd=_tmp32C;
# 1811
if((int)vd->sc == (int)Cyc_Absyn_Static)
return({struct _tuple19 _tmp606;_tmp606.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp70B=Cyc_CfFlowInfo_UnknownL();_tmp606.f2=_tmp70B;});_tmp606;});
_tmp32B=vd;goto _LLF;}case 3U: _LLE: _tmp32B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp32B;
_tmp32A=vd;goto _LL11;}case 5U: _LL10: _tmp32A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp32A;
# 1816
return({struct _tuple19 _tmp607;_tmp607.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp70D=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp331=_cycalloc(sizeof(*_tmp331));({void*_tmp70C=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->tag=0U,_tmp330->f1=vd;_tmp330;});_tmp331->root=_tmp70C;}),_tmp331->path=path;_tmp331;}));_tmp607.f2=_tmp70D;});_tmp607;});}case 1U: _LL12: _tmp329=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp321)->f1)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp329;
# 1818
return({struct _tuple19 _tmp608;_tmp608.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp70E=Cyc_CfFlowInfo_UnknownL();_tmp608.f2=_tmp70E;});_tmp608;});}default: goto _LL1C;}case 22U: _LL14: _tmp327=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp328=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL15: {struct Cyc_Absyn_Exp*e1=_tmp327;struct _fat_ptr*f=_tmp328;
# 1821
{void*_tmp332=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp5B=_tmp332;void*_tmp333=_stmttmp5B;void*_tmp334;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp333)->tag == 3U){_LL1F: _tmp334=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp333)->f1).elt_type;_LL20: {void*t2=_tmp334;
# 1823
if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(t2)){
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,f,inflow);
path=({struct Cyc_List_List*_tmp336=_cycalloc(sizeof(*_tmp336));({void*_tmp710=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->tag=0U,({int _tmp70F=Cyc_CfFlowInfo_get_field_index(t2,f);_tmp335->f1=_tmp70F;});_tmp335;});_tmp336->hd=_tmp710;}),_tmp336->tl=path;_tmp336;});}
# 1827
goto _LL1E;}}else{_LL21: _LL22:
({void*_tmp337=0U;({struct _fat_ptr _tmp711=({const char*_tmp338="anal_Lexp: AggrArrow ptr";_tag_fat(_tmp338,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp711,_tag_fat(_tmp337,sizeof(void*),0U));});});}_LL1E:;}
# 1830
_tmp326=e1;goto _LL17;}case 20U: _LL16: _tmp326=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp326;
# 1834
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)){
# 1836
struct _tuple19 _tmp339=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,e1);struct _tuple19 _stmttmp5C=_tmp339;struct _tuple19 _tmp33A=_stmttmp5C;union Cyc_CfFlowInfo_AbsLVal _tmp33C;union Cyc_CfFlowInfo_FlowInfo _tmp33B;_LL24: _tmp33B=_tmp33A.f1;_tmp33C=_tmp33A.f2;_LL25: {union Cyc_CfFlowInfo_FlowInfo f=_tmp33B;union Cyc_CfFlowInfo_AbsLVal lval=_tmp33C;
struct _tuple19 _tmp33D=({struct _tuple19 _tmp60C;_tmp60C.f1=f,_tmp60C.f2=lval;_tmp60C;});struct _tuple19 _stmttmp5D=_tmp33D;struct _tuple19 _tmp33E=_stmttmp5D;struct Cyc_CfFlowInfo_Place*_tmp341;struct Cyc_List_List*_tmp340;struct Cyc_Dict_Dict _tmp33F;if(((_tmp33E.f1).ReachableFL).tag == 2){if(((_tmp33E.f2).PlaceL).tag == 1){_LL27: _tmp33F=(((_tmp33E.f1).ReachableFL).val).f1;_tmp340=(((_tmp33E.f1).ReachableFL).val).f2;_tmp341=((_tmp33E.f2).PlaceL).val;_LL28: {struct Cyc_Dict_Dict fd=_tmp33F;struct Cyc_List_List*r=_tmp340;struct Cyc_CfFlowInfo_Place*p=_tmp341;
# 1839
void*_tmp342=Cyc_CfFlowInfo_lookup_place(fd,p);void*old_rval=_tmp342;
struct _tuple16 _tmp343=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _stmttmp5E=_tmp343;struct _tuple16 _tmp344=_stmttmp5E;struct Cyc_List_List*_tmp346;void*_tmp345;_LL2C: _tmp345=_tmp344.f1;_tmp346=_tmp344.f2;_LL2D: {void*rval=_tmp345;struct Cyc_List_List*names=_tmp346;
if(expand_unique){
int _tmp347=1;int possibly_null=_tmp347;
void*_tmp348=rval;void*_tmp34A;struct Cyc_Absyn_Vardecl*_tmp349;switch(*((int*)_tmp348)){case 8U: _LL2F: _tmp349=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp348)->f1;_tmp34A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp348)->f2;_LL30: {struct Cyc_Absyn_Vardecl*n=_tmp349;void*r=_tmp34A;
# 1845
({void*_tmp34B=0U;({struct _fat_ptr _tmp712=({const char*_tmp34C="bad named location in anal_Lexp:deref";_tag_fat(_tmp34C,sizeof(char),38U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp712,_tag_fat(_tmp34B,sizeof(void*),0U));});});}case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp348)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1849
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1848
 return Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1853
struct _tuple19 _tmp34D=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _stmttmp5F=_tmp34D;struct _tuple19 _tmp34E=_stmttmp5F;union Cyc_CfFlowInfo_FlowInfo _tmp34F;_LL3E: _tmp34F=_tmp34E.f1;_LL3F: {union Cyc_CfFlowInfo_FlowInfo f=_tmp34F;
return({struct _tuple19 _tmp609;_tmp609.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp713=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp609.f2=_tmp713;});_tmp609;});}}case 1U: _LL39: _LL3A:
# 1856
 possibly_null=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1863
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(fenv,fd,rval);
void*leaf=(int)il == (int)1U?fenv->unknown_all: fenv->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->tag=5U,({void*_tmp716=({struct Cyc_CfFlowInfo_FlowEnv*_tmp715=fenv;void*_tmp714=Cyc_Tcutil_pointer_elt_type((void*)_check_null(e1->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp715,_tmp714,0,leaf);});_tmp355->f1=_tmp716;});_tmp355;});
for(0;names != 0;names=names->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->tag=8U,_tmp350->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp350->f2=res;_tmp350;});}
fd=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp351=Cyc_CfFlowInfo_ReachableFL(fd,r);union Cyc_CfFlowInfo_FlowInfo outflow=_tmp351;
# 1876
struct _tuple19 _tmp352=Cyc_NewControlFlow_anal_derefL(env,outflow,e1,outflow,res,passthrough_consumes,possibly_null,path);struct _tuple19 _stmttmp60=_tmp352;struct _tuple19 _tmp353=_stmttmp60;union Cyc_CfFlowInfo_FlowInfo _tmp354;_LL41: _tmp354=_tmp353.f1;_LL42: {union Cyc_CfFlowInfo_FlowInfo f=_tmp354;
# 1878
return({struct _tuple19 _tmp60A;_tmp60A.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp717=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp60A.f2=_tmp717;});_tmp60A;});}}}}_LL2E:;}else{
# 1882
void*_tmp356=rval;switch(*((int*)_tmp356)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp356)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1887
struct _tuple19 _tmp357=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _stmttmp61=_tmp357;struct _tuple19 _tmp358=_stmttmp61;union Cyc_CfFlowInfo_FlowInfo _tmp359;_LL4B: _tmp359=_tmp358.f1;_LL4C: {union Cyc_CfFlowInfo_FlowInfo f=_tmp359;
return({struct _tuple19 _tmp60B;_tmp60B.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp718=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp60B.f2=_tmp718;});_tmp60B;});}}default: _LL48: _LL49:
# 1891
 goto _LL43;}_LL43:;}
# 1894
goto _LL26;}}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}}{
# 1899
struct _tuple17 _tmp35A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp62=_tmp35A;struct _tuple17 _tmp35B=_stmttmp62;void*_tmp35D;union Cyc_CfFlowInfo_FlowInfo _tmp35C;_LL4E: _tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;_LL4F: {union Cyc_CfFlowInfo_FlowInfo f=_tmp35C;void*r=_tmp35D;
# 1901
return Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,r,passthrough_consumes,0,path);}}}case 23U: _LL18: _tmp324=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp325=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp324;struct Cyc_Absyn_Exp*e2=_tmp325;
# 1904
void*_tmp35E=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp63=_tmp35E;void*_tmp35F=_stmttmp63;switch(*((int*)_tmp35F)){case 6U: _LL51: _LL52: {
# 1906
unsigned _tmp360=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;unsigned fld=_tmp360;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp71E=env;union Cyc_CfFlowInfo_FlowInfo _tmp71D=inflow;int _tmp71C=expand_unique;int _tmp71B=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp71A=e1;Cyc_NewControlFlow_anal_Lexp_rec(_tmp71E,_tmp71D,_tmp71C,_tmp71B,_tmp71A,({struct Cyc_List_List*_tmp362=_cycalloc(sizeof(*_tmp362));
({void*_tmp719=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->tag=0U,_tmp361->f1=(int)fld;_tmp361;});_tmp362->hd=_tmp719;}),_tmp362->tl=path;_tmp362;}));});}case 3U: _LL53: _LL54: {
# 1910
struct _tuple24 _tmp363=({struct Cyc_NewControlFlow_AnalEnv*_tmp720=env;union Cyc_CfFlowInfo_FlowInfo _tmp71F=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp720,_tmp71F,({struct Cyc_Absyn_Exp*_tmp373[2U];_tmp373[0]=e1,_tmp373[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp373,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple24 _stmttmp64=_tmp363;struct _tuple24 _tmp364=_stmttmp64;struct Cyc_List_List*_tmp366;union Cyc_CfFlowInfo_FlowInfo _tmp365;_LL58: _tmp365=_tmp364.f1;_tmp366=_tmp364.f2;_LL59: {union Cyc_CfFlowInfo_FlowInfo f=_tmp365;struct Cyc_List_List*rvals=_tmp366;
union Cyc_CfFlowInfo_FlowInfo _tmp367=f;union Cyc_CfFlowInfo_FlowInfo f2=_tmp367;
{union Cyc_CfFlowInfo_FlowInfo _tmp368=f;struct Cyc_List_List*_tmp36A;struct Cyc_Dict_Dict _tmp369;if((_tmp368.ReachableFL).tag == 2){_LL5B: _tmp369=((_tmp368.ReachableFL).val).f1;_tmp36A=((_tmp368.ReachableFL).val).f2;_LL5C: {struct Cyc_Dict_Dict d2=_tmp369;struct Cyc_List_List*relns=_tmp36A;
# 1914
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp36B=0U;({unsigned _tmp722=e2->loc;struct _fat_ptr _tmp721=({const char*_tmp36C="expression may not be initialized";_tag_fat(_tmp36C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp722,_tmp721,_tag_fat(_tmp36B,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp36D=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);struct Cyc_List_List*new_relns=_tmp36D;
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL5A;}}}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1923
struct _tuple19 _tmp36E=Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,(void*)((struct Cyc_List_List*)_check_null(rvals))->hd,passthrough_consumes,0,path);struct _tuple19 _stmttmp65=_tmp36E;struct _tuple19 _tmp36F=_stmttmp65;union Cyc_CfFlowInfo_AbsLVal _tmp371;union Cyc_CfFlowInfo_FlowInfo _tmp370;_LL60: _tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;_LL61: {union Cyc_CfFlowInfo_FlowInfo f3=_tmp370;union Cyc_CfFlowInfo_AbsLVal r=_tmp371;
union Cyc_CfFlowInfo_FlowInfo _tmp372=f3;if((_tmp372.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple19 _tmp60D;_tmp60D.f1=f3,_tmp60D.f2=r;_tmp60D;});}else{_LL65: _LL66:
 return({struct _tuple19 _tmp60E;_tmp60E.f1=f2,_tmp60E.f2=r;_tmp60E;});}_LL62:;}}}}default: _LL55: _LL56:
# 1928
({void*_tmp374=0U;({struct _fat_ptr _tmp723=({const char*_tmp375="anal_Lexp: Subscript -- bad type";_tag_fat(_tmp375,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp723,_tag_fat(_tmp374,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp322=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp321)->f1;_tmp323=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp321)->f2;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp322;struct _fat_ptr*fld=_tmp323;
# 1932
void*_tmp376=(void*)_check_null(e1->topt);void*e1_type=_tmp376;
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,fld,inflow);
# 1936
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple19 _tmp60F;_tmp60F.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp724=Cyc_CfFlowInfo_UnknownL();_tmp60F.f2=_tmp724;});_tmp60F;});
# 1939
return({struct Cyc_NewControlFlow_AnalEnv*_tmp72B=env;union Cyc_CfFlowInfo_FlowInfo _tmp72A=inflow;int _tmp729=expand_unique;int _tmp728=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp727=e1;Cyc_NewControlFlow_anal_Lexp_rec(_tmp72B,_tmp72A,_tmp729,_tmp728,_tmp727,({struct Cyc_List_List*_tmp378=_cycalloc(sizeof(*_tmp378));
({void*_tmp726=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->tag=0U,({int _tmp725=Cyc_CfFlowInfo_get_field_index(e1_type,fld);_tmp377->f1=_tmp725;});_tmp377;});_tmp378->hd=_tmp726;}),_tmp378->tl=path;_tmp378;}));});}default: _LL1C: _LL1D:
# 1943
 return({struct _tuple19 _tmp610;({union Cyc_CfFlowInfo_FlowInfo _tmp72D=Cyc_CfFlowInfo_BottomFL();_tmp610.f1=_tmp72D;}),({union Cyc_CfFlowInfo_AbsLVal _tmp72C=Cyc_CfFlowInfo_UnknownL();_tmp610.f2=_tmp72C;});_tmp610;});}_LL5:;}}
# 1947
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1951
struct _tuple19 _tmp379=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple19 _stmttmp66=_tmp379;struct _tuple19 _tmp37A=_stmttmp66;union Cyc_CfFlowInfo_AbsLVal _tmp37C;union Cyc_CfFlowInfo_FlowInfo _tmp37B;_LL1: _tmp37B=_tmp37A.f1;_tmp37C=_tmp37A.f2;_LL2: {union Cyc_CfFlowInfo_FlowInfo f=_tmp37B;union Cyc_CfFlowInfo_AbsLVal r=_tmp37C;
return({struct _tuple19 _tmp611;_tmp611.f1=f,_tmp611.f2=r;_tmp611;});}}
# 1958
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1961
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 1965
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
# 1968
struct _tuple19 _tmp37D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple19 _stmttmp67=_tmp37D;struct _tuple19 _tmp37E=_stmttmp67;union Cyc_CfFlowInfo_FlowInfo _tmp37F;_LL1: _tmp37F=_tmp37E.f1;_LL2: {union Cyc_CfFlowInfo_FlowInfo f=_tmp37F;
inflow=f;}}}}
# 1973
return inflow;}struct _tuple28{enum Cyc_Absyn_Primop f1;void*f2;void*f3;};
# 1979
static struct _tuple20 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1981
struct Cyc_CfFlowInfo_FlowEnv*_tmp380=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp380;
# 1984
struct _tuple17 _tmp381=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);struct _tuple17 _stmttmp68=_tmp381;struct _tuple17 _tmp382=_stmttmp68;void*_tmp384;union Cyc_CfFlowInfo_FlowInfo _tmp383;_LL1: _tmp383=_tmp382.f1;_tmp384=_tmp382.f2;_LL2: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp383;void*r1=_tmp384;
struct _tuple17 _tmp385=Cyc_NewControlFlow_anal_Rexp(env,0,f1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);struct _tuple17 _stmttmp69=_tmp385;struct _tuple17 _tmp386=_stmttmp69;void*_tmp388;union Cyc_CfFlowInfo_FlowInfo _tmp387;_LL4: _tmp387=_tmp386.f1;_tmp388=_tmp386.f2;_LL5: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp387;void*r2=_tmp388;
union Cyc_CfFlowInfo_FlowInfo _tmp389=f2;union Cyc_CfFlowInfo_FlowInfo f=_tmp389;
# 1989
union Cyc_CfFlowInfo_FlowInfo _tmp38A=f;struct Cyc_List_List*_tmp38C;struct Cyc_Dict_Dict _tmp38B;if((_tmp38A.BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple20 _tmp612;_tmp612.f1=f,_tmp612.f2=f;_tmp612;});}else{_LL9: _tmp38B=((_tmp38A.ReachableFL).val).f1;_tmp38C=((_tmp38A.ReachableFL).val).f2;_LLA: {struct Cyc_Dict_Dict d=_tmp38B;struct Cyc_List_List*relns=_tmp38C;
# 1992
struct Cyc_Absyn_Exp*_tmp38D=(struct Cyc_Absyn_Exp*)es->hd;struct Cyc_Absyn_Exp*e1=_tmp38D;
struct Cyc_Absyn_Exp*_tmp38E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp38E;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r1)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp38F=0U;({unsigned _tmp72F=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _fat_ptr _tmp72E=({const char*_tmp390="expression may not be initialized";_tag_fat(_tmp390,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp72F,_tmp72E,_tag_fat(_tmp38F,sizeof(void*),0U));});});
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r2)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp391=0U;({unsigned _tmp731=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _fat_ptr _tmp730=({const char*_tmp392="expression may not be initialized";_tag_fat(_tmp392,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp731,_tmp730,_tag_fat(_tmp391,sizeof(void*),0U));});});{
# 1999
union Cyc_CfFlowInfo_FlowInfo _tmp393=f;union Cyc_CfFlowInfo_FlowInfo ft=_tmp393;
union Cyc_CfFlowInfo_FlowInfo _tmp394=f;union Cyc_CfFlowInfo_FlowInfo ff=_tmp394;
# 2004
if((int)p == (int)5U ||(int)p == (int)6U){
struct _tuple16 _tmp395=Cyc_CfFlowInfo_unname_rval(r1);struct _tuple16 _stmttmp6A=_tmp395;struct _tuple16 _tmp396=_stmttmp6A;struct Cyc_List_List*_tmp398;void*_tmp397;_LLC: _tmp397=_tmp396.f1;_tmp398=_tmp396.f2;_LLD: {void*r1=_tmp397;struct Cyc_List_List*r1n=_tmp398;
struct _tuple16 _tmp399=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _stmttmp6B=_tmp399;struct _tuple16 _tmp39A=_stmttmp6B;struct Cyc_List_List*_tmp39C;void*_tmp39B;_LLF: _tmp39B=_tmp39A.f1;_tmp39C=_tmp39A.f2;_LL10: {void*r2=_tmp39B;struct Cyc_List_List*r2n=_tmp39C;
struct _tuple0 _tmp39D=({struct _tuple0 _tmp613;_tmp613.f1=r1,_tmp613.f2=r2;_tmp613;});struct _tuple0 _stmttmp6C=_tmp39D;struct _tuple0 _tmp39E=_stmttmp6C;enum Cyc_CfFlowInfo_InitLevel _tmp39F;enum Cyc_CfFlowInfo_InitLevel _tmp3A0;switch(*((int*)_tmp39E.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39E.f2)->tag == 0U){_LL12: _tmp3A0=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp39E.f1)->f1;_LL13: {enum Cyc_CfFlowInfo_InitLevel il=_tmp3A0;
# 2009
struct _tuple20 _tmp3A1=Cyc_NewControlFlow_splitzero(env,inflow,f,e1,il,r1n);struct _tuple20 _stmttmp6D=_tmp3A1;struct _tuple20 _tmp3A2=_stmttmp6D;union Cyc_CfFlowInfo_FlowInfo _tmp3A4;union Cyc_CfFlowInfo_FlowInfo _tmp3A3;_LL23: _tmp3A3=_tmp3A2.f1;_tmp3A4=_tmp3A2.f2;_LL24: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp3A3;union Cyc_CfFlowInfo_FlowInfo f2=_tmp3A4;
{enum Cyc_Absyn_Primop _tmp3A5=p;switch(_tmp3A5){case Cyc_Absyn_Eq: _LL26: _LL27:
 ft=f2;ff=f1;goto _LL25;case Cyc_Absyn_Neq: _LL28: _LL29:
 ft=f1;ff=f2;goto _LL25;default: _LL2A: _LL2B:
({void*_tmp3A6=0U;({struct _fat_ptr _tmp732=({const char*_tmp3A7="anal_test, zero-split";_tag_fat(_tmp3A7,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp732,_tag_fat(_tmp3A6,sizeof(void*),0U));});});}_LL25:;}
# 2015
goto _LL11;}}}else{goto _LL20;}case 0U: switch(*((int*)_tmp39E.f2)){case 2U: _LL14: _tmp39F=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp39E.f2)->f1;_LL15: {enum Cyc_CfFlowInfo_InitLevel il=_tmp39F;
# 2017
struct _tuple20 _tmp3A8=Cyc_NewControlFlow_splitzero(env,f2,f,e2,il,r2n);struct _tuple20 _stmttmp6E=_tmp3A8;struct _tuple20 _tmp3A9=_stmttmp6E;union Cyc_CfFlowInfo_FlowInfo _tmp3AB;union Cyc_CfFlowInfo_FlowInfo _tmp3AA;_LL2D: _tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A9.f2;_LL2E: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp3AA;union Cyc_CfFlowInfo_FlowInfo f2=_tmp3AB;
{enum Cyc_Absyn_Primop _tmp3AC=p;switch(_tmp3AC){case Cyc_Absyn_Eq: _LL30: _LL31:
 ft=f2;ff=f1;goto _LL2F;case Cyc_Absyn_Neq: _LL32: _LL33:
 ft=f1;ff=f2;goto _LL2F;default: _LL34: _LL35:
({void*_tmp3AD=0U;({struct _fat_ptr _tmp733=({const char*_tmp3AE="anal_test, zero-split";_tag_fat(_tmp3AE,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp733,_tag_fat(_tmp3AD,sizeof(void*),0U));});});}_LL2F:;}
# 2023
goto _LL11;}}case 0U: _LL16: _LL17:
# 2025
 if((int)p == (int)5U)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;case 1U: _LL18: _LL19:
 goto _LL1B;case 4U: _LL1A: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39E.f2)->tag == 0U){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL20;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39E.f2)->tag == 0U){_LL1E: _LL1F:
# 2032
 if((int)p == (int)6U)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;}else{goto _LL20;}default: _LL20: _LL21:
 goto _LL11;}_LL11:;}}}
# 2044
{struct _tuple28 _tmp3AF=({struct _tuple28 _tmp615;_tmp615.f1=p,({
void*_tmp735=Cyc_Absyn_compress((void*)_check_null(e1->topt));_tmp615.f2=_tmp735;}),({
void*_tmp734=Cyc_Absyn_compress((void*)_check_null(e2->topt));_tmp615.f3=_tmp734;});_tmp615;});
# 2044
struct _tuple28 _stmttmp6F=_tmp3AF;struct _tuple28 _tmp3B0=_stmttmp6F;switch(_tmp3B0.f1){case Cyc_Absyn_Eq: _LL37: _LL38:
# 2047
 goto _LL3A;case Cyc_Absyn_Neq: _LL39: _LL3A:
 goto _LL3C;default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3B: _LL3C:
 goto _LL3E;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{goto _LL43;}case 4U: goto _LL41;default: goto _LL43;}else{goto _LL43;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f2)->f1)->tag == 4U)goto _LL3F;else{goto _LL43;}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f2)->f1)->tag == 4U)goto _LL3F;else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)->tag == 4U)goto _LL41;else{goto _LL43;}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f2)->f1)->tag == 4U){_LL3F: _LL40:
# 2051
 goto _LL42;}else{goto _LL43;}}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0.f3)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3D: _LL3E:
# 2050
 goto _LL40;}else{goto _LL43;}case 4U: _LL41: _LL42:
# 2052
 goto _LL36;default: goto _LL43;}else{_LL43: _LL44:
 return({struct _tuple20 _tmp614;_tmp614.f1=ft,_tmp614.f2=ff;_tmp614;});}}}_LL36:;}
# 2057
{enum Cyc_Absyn_Primop _tmp3B1=p;switch(_tmp3B1){case Cyc_Absyn_Eq: _LL46: _LL47:
 goto _LL49;case Cyc_Absyn_Neq: _LL48: _LL49: goto _LL4B;case Cyc_Absyn_Gt: _LL4A: _LL4B: goto _LL4D;case Cyc_Absyn_Gte: _LL4C: _LL4D: goto _LL4F;case Cyc_Absyn_Lt: _LL4E: _LL4F: goto _LL51;case Cyc_Absyn_Lte: _LL50: _LL51: goto _LL45;default: _LL52: _LL53:
 return({struct _tuple20 _tmp616;_tmp616.f1=ft,_tmp616.f2=ff;_tmp616;});}_LL45:;}{
# 2062
struct _RegionHandle*_tmp3B2=Cyc_Core_heap_region;struct _RegionHandle*r=_tmp3B2;
struct _tuple13 _tmp3B3=Cyc_Relations_primop2relation(e1,p,e2);struct _tuple13 _stmttmp70=_tmp3B3;struct _tuple13 _tmp3B4=_stmttmp70;struct Cyc_Absyn_Exp*_tmp3B7;enum Cyc_Relations_Relation _tmp3B6;struct Cyc_Absyn_Exp*_tmp3B5;_LL55: _tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;_tmp3B7=_tmp3B4.f3;_LL56: {struct Cyc_Absyn_Exp*e1=_tmp3B5;enum Cyc_Relations_Relation relation=_tmp3B6;struct Cyc_Absyn_Exp*e2=_tmp3B7;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2067
if(Cyc_Relations_exp2relnop(e1,& n1)&& Cyc_Relations_exp2relnop(e2,& n2)){
# 2069
struct Cyc_List_List*_tmp3B8=Cyc_Relations_add_relation(r,n1,relation,n2,relns);struct Cyc_List_List*relns_true=_tmp3B8;
# 2073
struct Cyc_List_List*_tmp3B9=({struct _RegionHandle*_tmp739=r;union Cyc_Relations_RelnOp _tmp738=n2;enum Cyc_Relations_Relation _tmp737=Cyc_Relations_flip_relation(relation);union Cyc_Relations_RelnOp _tmp736=n1;Cyc_Relations_add_relation(_tmp739,_tmp738,_tmp737,_tmp736,relns);});struct Cyc_List_List*relns_false=_tmp3B9;
struct _tuple20 _tmp3BA=({struct _tuple20 _tmp61B;_tmp61B.f1=ft,_tmp61B.f2=ff;_tmp61B;});struct _tuple20 _stmttmp71=_tmp3BA;struct _tuple20 _tmp3BB=_stmttmp71;struct Cyc_Dict_Dict _tmp3BC;struct Cyc_Dict_Dict _tmp3BD;struct Cyc_Dict_Dict _tmp3BF;struct Cyc_Dict_Dict _tmp3BE;if(((_tmp3BB.f1).ReachableFL).tag == 2){if(((_tmp3BB.f2).ReachableFL).tag == 2){_LL58: _tmp3BE=(((_tmp3BB.f1).ReachableFL).val).f1;_tmp3BF=(((_tmp3BB.f2).ReachableFL).val).f1;_LL59: {struct Cyc_Dict_Dict dt=_tmp3BE;struct Cyc_Dict_Dict df=_tmp3BF;
# 2076
return({struct _tuple20 _tmp617;({union Cyc_CfFlowInfo_FlowInfo _tmp73B=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_tmp617.f1=_tmp73B;}),({union Cyc_CfFlowInfo_FlowInfo _tmp73A=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_tmp617.f2=_tmp73A;});_tmp617;});}}else{_LL5C: _tmp3BD=(((_tmp3BB.f1).ReachableFL).val).f1;_LL5D: {struct Cyc_Dict_Dict dt=_tmp3BD;
# 2080
return({struct _tuple20 _tmp618;({union Cyc_CfFlowInfo_FlowInfo _tmp73C=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_tmp618.f1=_tmp73C;}),_tmp618.f2=ff;_tmp618;});}}}else{if(((_tmp3BB.f2).ReachableFL).tag == 2){_LL5A: _tmp3BC=(((_tmp3BB.f2).ReachableFL).val).f1;_LL5B: {struct Cyc_Dict_Dict df=_tmp3BC;
# 2078
return({struct _tuple20 _tmp619;_tmp619.f1=ft,({union Cyc_CfFlowInfo_FlowInfo _tmp73D=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_tmp619.f2=_tmp73D;});_tmp619;});}}else{_LL5E: _LL5F:
# 2082
 return({struct _tuple20 _tmp61A;_tmp61A.f1=ft,_tmp61A.f2=ff;_tmp61A;});}}_LL57:;}else{
# 2085
return({struct _tuple20 _tmp61C;_tmp61C.f1=ft,_tmp61C.f2=ff;_tmp61C;});}}}}}}_LL6:;}}}
# 2091
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C0=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp3C0;
void*_tmp3C1=e->r;void*_stmttmp72=_tmp3C1;void*_tmp3C2=_stmttmp72;struct Cyc_List_List*_tmp3C4;enum Cyc_Absyn_Primop _tmp3C3;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CC;switch(*((int*)_tmp3C2)){case 6U: _LL1: _tmp3CC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3CD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_tmp3CE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C2)->f3;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3CC;struct Cyc_Absyn_Exp*e2=_tmp3CD;struct Cyc_Absyn_Exp*e3=_tmp3CE;
# 2095
struct _tuple20 _tmp3CF=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp73=_tmp3CF;struct _tuple20 _tmp3D0=_stmttmp73;union Cyc_CfFlowInfo_FlowInfo _tmp3D2;union Cyc_CfFlowInfo_FlowInfo _tmp3D1;_LL10: _tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;_LL11: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp3D1;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp3D2;
struct _tuple20 _tmp3D3=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _stmttmp74=_tmp3D3;struct _tuple20 _tmp3D4=_stmttmp74;union Cyc_CfFlowInfo_FlowInfo _tmp3D6;union Cyc_CfFlowInfo_FlowInfo _tmp3D5;_LL13: _tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;_LL14: {union Cyc_CfFlowInfo_FlowInfo f2t=_tmp3D5;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp3D6;
struct _tuple20 _tmp3D7=Cyc_NewControlFlow_anal_test(env,f1f,e3);struct _tuple20 _stmttmp75=_tmp3D7;struct _tuple20 _tmp3D8=_stmttmp75;union Cyc_CfFlowInfo_FlowInfo _tmp3DA;union Cyc_CfFlowInfo_FlowInfo _tmp3D9;_LL16: _tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;_LL17: {union Cyc_CfFlowInfo_FlowInfo f3t=_tmp3D9;union Cyc_CfFlowInfo_FlowInfo f3f=_tmp3DA;
return({struct _tuple20 _tmp61D;({union Cyc_CfFlowInfo_FlowInfo _tmp73F=Cyc_CfFlowInfo_join_flow(fenv,f2t,f3t);_tmp61D.f1=_tmp73F;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp73E=Cyc_CfFlowInfo_join_flow(fenv,f2f,f3f);_tmp61D.f2=_tmp73E;});_tmp61D;});}}}}case 7U: _LL3: _tmp3CA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3CB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp3CA;struct Cyc_Absyn_Exp*e2=_tmp3CB;
# 2101
struct _tuple20 _tmp3DB=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp76=_tmp3DB;struct _tuple20 _tmp3DC=_stmttmp76;union Cyc_CfFlowInfo_FlowInfo _tmp3DE;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;_LL19: _tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_LL1A: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp3DD;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp3DE;
struct _tuple20 _tmp3DF=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _stmttmp77=_tmp3DF;struct _tuple20 _tmp3E0=_stmttmp77;union Cyc_CfFlowInfo_FlowInfo _tmp3E2;union Cyc_CfFlowInfo_FlowInfo _tmp3E1;_LL1C: _tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3E0.f2;_LL1D: {union Cyc_CfFlowInfo_FlowInfo f2t=_tmp3E1;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp3E2;
return({struct _tuple20 _tmp61E;_tmp61E.f1=f2t,({union Cyc_CfFlowInfo_FlowInfo _tmp740=Cyc_CfFlowInfo_join_flow(fenv,f1f,f2f);_tmp61E.f2=_tmp740;});_tmp61E;});}}}case 8U: _LL5: _tmp3C8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3C9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp3C8;struct Cyc_Absyn_Exp*e2=_tmp3C9;
# 2105
struct _tuple20 _tmp3E3=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp78=_tmp3E3;struct _tuple20 _tmp3E4=_stmttmp78;union Cyc_CfFlowInfo_FlowInfo _tmp3E6;union Cyc_CfFlowInfo_FlowInfo _tmp3E5;_LL1F: _tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;_LL20: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp3E5;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp3E6;
struct _tuple20 _tmp3E7=Cyc_NewControlFlow_anal_test(env,f1f,e2);struct _tuple20 _stmttmp79=_tmp3E7;struct _tuple20 _tmp3E8=_stmttmp79;union Cyc_CfFlowInfo_FlowInfo _tmp3EA;union Cyc_CfFlowInfo_FlowInfo _tmp3E9;_LL22: _tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;_LL23: {union Cyc_CfFlowInfo_FlowInfo f2t=_tmp3E9;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp3EA;
return({struct _tuple20 _tmp61F;({union Cyc_CfFlowInfo_FlowInfo _tmp741=Cyc_CfFlowInfo_join_flow(fenv,f1t,f2t);_tmp61F.f1=_tmp741;}),_tmp61F.f2=f2f;_tmp61F;});}}}case 9U: _LL7: _tmp3C6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3C7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp3C6;struct Cyc_Absyn_Exp*e2=_tmp3C7;
# 2109
struct _tuple17 _tmp3EB=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1);struct _tuple17 _stmttmp7A=_tmp3EB;struct _tuple17 _tmp3EC=_stmttmp7A;void*_tmp3EE;union Cyc_CfFlowInfo_FlowInfo _tmp3ED;_LL25: _tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;_LL26: {union Cyc_CfFlowInfo_FlowInfo f=_tmp3ED;void*r=_tmp3EE;
return Cyc_NewControlFlow_anal_test(env,f,e2);}}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->tl == 0){_LL9: _tmp3C5=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->hd;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp3C5;
# 2112
struct _tuple20 _tmp3EF=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp7B=_tmp3EF;struct _tuple20 _tmp3F0=_stmttmp7B;union Cyc_CfFlowInfo_FlowInfo _tmp3F2;union Cyc_CfFlowInfo_FlowInfo _tmp3F1;_LL28: _tmp3F1=_tmp3F0.f1;_tmp3F2=_tmp3F0.f2;_LL29: {union Cyc_CfFlowInfo_FlowInfo f1=_tmp3F1;union Cyc_CfFlowInfo_FlowInfo f2=_tmp3F2;
return({struct _tuple20 _tmp620;_tmp620.f1=f2,_tmp620.f2=f1;_tmp620;});}}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp3C3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_tmp3C4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp3C3;struct Cyc_List_List*es=_tmp3C4;
# 2115
return Cyc_NewControlFlow_anal_primop_test(env,inflow,p,es);}}default: _LLD: _LLE: {
# 2119
struct _tuple17 _tmp3F3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple17 _stmttmp7C=_tmp3F3;struct _tuple17 _tmp3F4=_stmttmp7C;void*_tmp3F6;union Cyc_CfFlowInfo_FlowInfo _tmp3F5;_LL2B: _tmp3F5=_tmp3F4.f1;_tmp3F6=_tmp3F4.f2;_LL2C: {union Cyc_CfFlowInfo_FlowInfo f=_tmp3F5;void*r=_tmp3F6;
union Cyc_CfFlowInfo_FlowInfo _tmp3F7=f;struct Cyc_Dict_Dict _tmp3F8;if((_tmp3F7.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple20 _tmp621;_tmp621.f1=f,_tmp621.f2=f;_tmp621;});}else{_LL30: _tmp3F8=((_tmp3F7.ReachableFL).val).f1;_LL31: {struct Cyc_Dict_Dict d=_tmp3F8;
# 2123
struct _tuple16 _tmp3F9=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp7D=_tmp3F9;struct _tuple16 _tmp3FA=_stmttmp7D;struct Cyc_List_List*_tmp3FC;void*_tmp3FB;_LL33: _tmp3FB=_tmp3FA.f1;_tmp3FC=_tmp3FA.f2;_LL34: {void*r=_tmp3FB;struct Cyc_List_List*names=_tmp3FC;
void*_tmp3FD=r;enum Cyc_CfFlowInfo_InitLevel _tmp3FE;void*_tmp400;struct Cyc_Absyn_Vardecl*_tmp3FF;switch(*((int*)_tmp3FD)){case 8U: _LL36: _tmp3FF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3FD)->f1;_tmp400=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3FD)->f2;_LL37: {struct Cyc_Absyn_Vardecl*n=_tmp3FF;void*r2=_tmp400;
# 2126
({void*_tmp401=0U;({struct _fat_ptr _tmp742=({const char*_tmp402="anal_test: bad namedlocation";_tag_fat(_tmp402,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp742,_tag_fat(_tmp401,sizeof(void*),0U));});});}case 0U: _LL38: _LL39:
 return({struct _tuple20 _tmp622;({union Cyc_CfFlowInfo_FlowInfo _tmp743=Cyc_CfFlowInfo_BottomFL();_tmp622.f1=_tmp743;}),_tmp622.f2=f;_tmp622;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple20 _tmp623;_tmp623.f1=f,({union Cyc_CfFlowInfo_FlowInfo _tmp744=Cyc_CfFlowInfo_BottomFL();_tmp623.f2=_tmp744;});_tmp623;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3FD)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp3FE=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3FD)->f1;_LL47: {enum Cyc_CfFlowInfo_InitLevel il=_tmp3FE;
# 2136
return Cyc_NewControlFlow_splitzero(env,inflow,f,e,il,names);}}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp3FD)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2132
 goto _LL45;}else{_LL48: _LL49:
# 2137
 return({struct _tuple20 _tmp624;_tmp624.f1=f,_tmp624.f2=f;_tmp624;});}case 7U: _LL44: _LL45:
# 2134
({void*_tmp403=0U;({unsigned _tmp746=e->loc;struct _fat_ptr _tmp745=({const char*_tmp404="expression may not be initialized";_tag_fat(_tmp404,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp746,_tmp745,_tag_fat(_tmp403,sizeof(void*),0U));});});
return({struct _tuple20 _tmp625;({union Cyc_CfFlowInfo_FlowInfo _tmp748=Cyc_CfFlowInfo_BottomFL();_tmp625.f1=_tmp748;}),({union Cyc_CfFlowInfo_FlowInfo _tmp747=Cyc_CfFlowInfo_BottomFL();_tmp625.f2=_tmp747;});_tmp625;});default: _LL4A: _LL4B:
# 2138
({void*_tmp405=0U;({struct _fat_ptr _tmp749=({const char*_tmp406="anal_test";_tag_fat(_tmp406,sizeof(char),10U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp749,_tag_fat(_tmp405,sizeof(void*),0U));});});}_LL35:;}}}_LL2D:;}}}_LL0:;}struct _tuple29{unsigned f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2144
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple29*ckenv,void*root,void*rval){
# 2146
struct _tuple29*_tmp407=ckenv;struct Cyc_Dict_Dict _tmp40A;struct Cyc_NewControlFlow_AnalEnv*_tmp409;unsigned _tmp408;_LL1: _tmp408=_tmp407->f1;_tmp409=_tmp407->f2;_tmp40A=_tmp407->f3;_LL2: {unsigned loc=_tmp408;struct Cyc_NewControlFlow_AnalEnv*env=_tmp409;struct Cyc_Dict_Dict fd=_tmp40A;
void*_tmp40B=root;struct Cyc_Absyn_Vardecl*_tmp40C;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp40B)->tag == 0U){_LL4: _tmp40C=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp40B)->f1;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp40C;
# 2149
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
struct _tuple16 _tmp40D=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple16 _stmttmp7E=_tmp40D;struct _tuple16 _tmp40E=_stmttmp7E;void*_tmp40F;_LL9: _tmp40F=_tmp40E.f1;_LLA: {void*rval=_tmp40F;
void*_tmp410=rval;switch(*((int*)_tmp410)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp410)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2156
({struct Cyc_String_pa_PrintArg_struct _tmp413=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({struct _fat_ptr _tmp74A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp626.f1=_tmp74A;});_tmp626;});void*_tmp411[1U];_tmp411[0]=& _tmp413;({unsigned _tmp74C=loc;struct _fat_ptr _tmp74B=({const char*_tmp412="unique pointers reachable from %s may become unreachable";_tag_fat(_tmp412,sizeof(char),57U);});Cyc_Warn_warn(_tmp74C,_tmp74B,_tag_fat(_tmp411,sizeof(void*),1U));});});
goto _LLB;}_LLB:;}}
# 2160
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}}
# 2165
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp414=flow;struct Cyc_Dict_Dict _tmp415;if((_tmp414.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp415=((_tmp414.ReachableFL).val).f1;_LL4: {struct Cyc_Dict_Dict d=_tmp415;
# 2169
{struct Cyc_List_List*_tmp416=env->param_roots;struct Cyc_List_List*inits=_tmp416;for(0;inits != 0;inits=inits->tl){
if((int)({struct Cyc_CfFlowInfo_FlowEnv*_tmp74E=env->fenv;struct Cyc_Dict_Dict _tmp74D=d;Cyc_CfFlowInfo_initlevel(_tmp74E,_tmp74D,Cyc_CfFlowInfo_lookup_place(d,(struct Cyc_CfFlowInfo_Place*)inits->hd));})!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp417=0U;({unsigned _tmp750=loc;struct _fat_ptr _tmp74F=({const char*_tmp418="function may not initialize all the parameters with attribute 'initializes'";_tag_fat(_tmp418,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp750,_tmp74F,_tag_fat(_tmp417,sizeof(void*),0U));});});}}
# 2173
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple29 _tmp419=({struct _tuple29 _tmp627;_tmp627.f1=loc,_tmp627.f2=env,_tmp627.f3=d;_tmp627;});struct _tuple29 check_env=_tmp419;
((void(*)(void(*f)(struct _tuple29*,void*,void*),struct _tuple29*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& check_env,d);}
# 2177
return;}}_LL0:;}
# 2186
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2188
struct Cyc_List_List*_tmp41A=0;struct Cyc_List_List*roots=_tmp41A;
struct Cyc_List_List*_tmp41B=0;struct Cyc_List_List*es=_tmp41B;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp41C=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp7F=_tmp41C;struct _tuple25*_tmp41D=_stmttmp7F;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Vardecl**_tmp41E;_LL1: _tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;_LL2: {struct Cyc_Absyn_Vardecl**vopt=_tmp41E;struct Cyc_Absyn_Exp*eopt=_tmp41F;
if((vopt != 0 && eopt != 0)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(eopt->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp420=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=0U,_tmp425->f1=*vopt;_tmp425;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp420;
struct Cyc_CfFlowInfo_Place*_tmp421=({struct Cyc_CfFlowInfo_Place*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->root=(void*)root,_tmp424->path=0;_tmp424;});struct Cyc_CfFlowInfo_Place*rp=_tmp421;
roots=({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->hd=rp,_tmp422->tl=roots;_tmp422;});
es=({struct Cyc_List_List*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->hd=eopt,_tmp423->tl=es;_tmp423;});}}}}
# 2200
return({struct _tuple1 _tmp628;_tmp628.f1=roots,_tmp628.f2=es;_tmp628;});}struct _tuple30{int f1;void*f2;};
# 2206
static struct _tuple30 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2213
union Cyc_CfFlowInfo_FlowInfo _tmp426=flow;struct Cyc_Dict_Dict _tmp427;if((_tmp426.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp428=0U;({struct _fat_ptr _tmp751=({const char*_tmp429="noconsume_place_ok: flow became Bottom!";_tag_fat(_tmp429,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp751,_tag_fat(_tmp428,sizeof(void*),0U));});});}else{_LL3: _tmp427=((_tmp426.ReachableFL).val).f1;_LL4: {struct Cyc_Dict_Dict d=_tmp427;
# 2221
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*bogus_exp=_tmp42A;
int _tmp42B=0;int bogus_bool=_tmp42B;
int _tmp42C=1;int bogus_int=_tmp42C;
void*_tmp42D=Cyc_CfFlowInfo_lookup_place(d,place);void*curr_rval=_tmp42D;
void*_tmp42E=curr_rval;void*rval=_tmp42E;
# 2233
int varok=0;
{void*_tmp42F=curr_rval;void*_tmp431;struct Cyc_Absyn_Vardecl*_tmp430;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42F)->tag == 8U){_LL6: _tmp430=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42F)->f1;_tmp431=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp42F)->f2;_LL7: {struct Cyc_Absyn_Vardecl*n=_tmp430;void*r=_tmp431;
# 2236
if(vd == n){
rval=r;
# 2239
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2241
if(Cyc_CfFlowInfo_is_unique_consumed(bogus_exp,bogus_int,rval,& bogus_bool)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp434=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,({
# 2245
struct _fat_ptr _tmp752=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp629.f1=_tmp752;});_tmp629;});void*_tmp432[1U];_tmp432[0]=& _tmp434;({unsigned _tmp754=loc;struct _fat_ptr _tmp753=({const char*_tmp433="function consumes parameter %s which is does not have the 'consume' attribute";_tag_fat(_tmp433,sizeof(char),78U);});Cyc_CfFlowInfo_aerr(_tmp754,_tmp753,_tag_fat(_tmp432,sizeof(void*),1U));});});}else{
# 2248
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,rval)!= (int)Cyc_CfFlowInfo_AllIL && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp437=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,({
# 2251
struct _fat_ptr _tmp755=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp62A.f1=_tmp755;});_tmp62A;});void*_tmp435[1U];_tmp435[0]=& _tmp437;({unsigned _tmp757=loc;struct _fat_ptr _tmp756=({const char*_tmp436="function consumes value pointed to by parameter %s, which does not have the 'consume' attribute";_tag_fat(_tmp436,sizeof(char),96U);});Cyc_CfFlowInfo_aerr(_tmp757,_tmp756,_tag_fat(_tmp435,sizeof(void*),1U));});});else{
# 2253
varok=1;}}}else{
# 2256
varok=1;}}else{
# 2259
goto _LL9;}
goto _LL5;}}else{_LL8: _LL9:
# 2263
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp43A=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,({
# 2268
struct _fat_ptr _tmp758=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp62B.f1=_tmp758;});_tmp62B;});void*_tmp438[1U];_tmp438[0]=& _tmp43A;({unsigned _tmp75A=loc;struct _fat_ptr _tmp759=({const char*_tmp439="function parameter %s without 'consume' attribute no longer set to its original value";_tag_fat(_tmp439,sizeof(char),86U);});Cyc_CfFlowInfo_aerr(_tmp75A,_tmp759,_tag_fat(_tmp438,sizeof(void*),1U));});});}
goto _LL5;}_LL5:;}
# 2275
return({struct _tuple30 _tmp62C;_tmp62C.f1=varok,_tmp62C.f2=rval;_tmp62C;});}}_LL0:;}
# 2281
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp43B=p;struct Cyc_List_List*_tmp43D;void*_tmp43C;_LL1: _tmp43C=_tmp43B->root;_tmp43D=_tmp43B->path;_LL2: {void*root=_tmp43C;struct Cyc_List_List*fs=_tmp43D;
if(fs != 0)
({void*_tmp43E=0U;({struct _fat_ptr _tmp75B=({const char*_tmp43F="unconsume_params: param to unconsume is non-variable";_tag_fat(_tmp43F,sizeof(char),53U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp75B,_tag_fat(_tmp43E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp440=root;struct Cyc_Absyn_Vardecl*_tmp441;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp440)->tag == 0U){_LL4: _tmp441=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp440)->f1;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp441;
return vd;}}else{_LL6: _LL7:
({void*_tmp442=0U;({struct _fat_ptr _tmp75C=({const char*_tmp443="unconsume_params: root is not a varroot";_tag_fat(_tmp443,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp75C,_tag_fat(_tmp442,sizeof(void*),0U));});});}_LL3:;}}}
# 2300 "/tmp/cyclone/src/new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2308
{union Cyc_CfFlowInfo_FlowInfo _tmp444=flow;struct Cyc_Dict_Dict _tmp445;if((_tmp444.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp445=((_tmp444.ReachableFL).val).f1;_LL4: {struct Cyc_Dict_Dict d=_tmp445;
# 2315
struct _tuple19 _tmp446=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple19 _stmttmp80=_tmp446;struct _tuple19 _tmp447=_stmttmp80;union Cyc_CfFlowInfo_AbsLVal _tmp449;union Cyc_CfFlowInfo_FlowInfo _tmp448;_LL6: _tmp448=_tmp447.f1;_tmp449=_tmp447.f2;_LL7: {union Cyc_CfFlowInfo_FlowInfo f=_tmp448;union Cyc_CfFlowInfo_AbsLVal lval=_tmp449;
# 2318
{union Cyc_CfFlowInfo_AbsLVal _tmp44A=lval;struct Cyc_CfFlowInfo_Place*_tmp44B;if((_tmp44A.PlaceL).tag == 1){_LL9: _tmp44B=(_tmp44A.PlaceL).val;_LLA: {struct Cyc_CfFlowInfo_Place*p=_tmp44B;
# 2322
void*_tmp44C=Cyc_CfFlowInfo_lookup_place(d,p);void*old_rval=_tmp44C;
{void*_tmp44D=old_rval;void*_tmp44F;struct Cyc_Absyn_Vardecl*_tmp44E;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp44D)->tag == 8U){_LLE: _tmp44E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp44D)->f1;_tmp44F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp44D)->f2;_LLF: {struct Cyc_Absyn_Vardecl*old_vd=_tmp44E;void*r=_tmp44F;
# 2328
void*new_rval;
if(old_vd == vd){
# 2332
if(varok){
# 2334
old_rval=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,r);
# 2339
if(ropt != 0){
# 2345
struct _tuple17 _tmp450=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp75E=env->fenv;struct _tuple17 _tmp75D=({struct _tuple17 _tmp62D;_tmp62D.f1=f,_tmp62D.f2=old_rval;_tmp62D;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp75E,_tmp75D,({struct _tuple17 _tmp62E;_tmp62E.f1=f,_tmp62E.f2=ropt;_tmp62E;}));});
# 2345
struct _tuple17 _stmttmp81=_tmp450;struct _tuple17 _tmp451=_stmttmp81;void*_tmp453;union Cyc_CfFlowInfo_FlowInfo _tmp452;_LL13: _tmp452=_tmp451.f1;_tmp453=_tmp451.f2;_LL14: {union Cyc_CfFlowInfo_FlowInfo f2=_tmp452;void*new_rval2=_tmp453;
# 2349
f=f2;new_rval=new_rval2;}}else{
# 2354
new_rval=old_rval;}}else{
# 2357
new_rval=r;}
# 2359
{union Cyc_CfFlowInfo_FlowInfo _tmp454=f;struct Cyc_List_List*_tmp456;struct Cyc_Dict_Dict _tmp455;if((_tmp454.ReachableFL).tag == 2){_LL16: _tmp455=((_tmp454.ReachableFL).val).f1;_tmp456=((_tmp454.ReachableFL).val).f2;_LL17: {struct Cyc_Dict_Dict d2=_tmp455;struct Cyc_List_List*relns=_tmp456;
# 2361
flow=({struct Cyc_Dict_Dict _tmp75F=Cyc_CfFlowInfo_assign_place(env->fenv,loc,d2,p,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp75F,relns);});
# 2365
goto _LL15;}}else{_LL18: _LL19:
# 2367
({void*_tmp457=0U;({struct _fat_ptr _tmp760=({const char*_tmp458="unconsume_params: joined flow became bot!";_tag_fat(_tmp458,sizeof(char),42U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp760,_tag_fat(_tmp457,sizeof(void*),0U));});});}_LL15:;}
# 2369
goto _LLD;}else{
# 2371
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2378
 if(ropt != 0 && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp45B=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp761=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp62F.f1=_tmp761;});_tmp62F;});void*_tmp459[1U];_tmp459[0]=& _tmp45B;({unsigned _tmp763=loc;struct _fat_ptr _tmp762=({const char*_tmp45A="aliased expression %s was overwritten";_tag_fat(_tmp45A,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp763,_tmp762,_tag_fat(_tmp459,sizeof(void*),1U));});});
# 2395 "/tmp/cyclone/src/new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2397
goto _LL8;}}else{_LLB: _LLC:
# 2403
 goto _LL8;}_LL8:;}
# 2405
goto _LL0;}}}_LL0:;}
# 2407
return flow;}
# 2420 "/tmp/cyclone/src/new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2426
{union Cyc_CfFlowInfo_FlowInfo _tmp45C=flow;if((_tmp45C.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2430
int _tmp45D=unconsume_exps != 0;int do_unconsume=_tmp45D;
struct Cyc_List_List*_tmp45E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);struct Cyc_List_List*consumed_vals=_tmp45E;
struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);struct Cyc_List_List*unconsume_exps=_tmp45F;
{struct Cyc_List_List*_tmp460=consumed_vals;struct Cyc_List_List*params=_tmp460;for(0;params != 0;(
params=params->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2439
struct Cyc_Absyn_Vardecl*_tmp461=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)params->hd);struct Cyc_Absyn_Vardecl*vd=_tmp461;
struct _tuple30 _tmp462=
is_region_open?({struct _tuple30 _tmp630;_tmp630.f1=1,_tmp630.f2=0;_tmp630;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)params->hd,do_unconsume,vd,flow,loc);
# 2440
struct _tuple30 _stmttmp82=_tmp462;struct _tuple30 _tmp463=_stmttmp82;void*_tmp465;int _tmp464;_LL6: _tmp464=_tmp463.f1;_tmp465=_tmp463.f2;_LL7: {int varok=_tmp464;void*rval=_tmp465;
# 2446
if(do_unconsume)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,vd,varok,rval,flow,loc);}}}
# 2449
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;}}struct _tuple31{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2453
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){
# 2455
struct Cyc_CfFlowInfo_FlowEnv*_tmp466=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp466;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp467=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmp83=_tmp467;struct Cyc_Absyn_Switch_clause*_tmp468=_stmttmp83;unsigned _tmp46C;struct Cyc_Absyn_Stmt*_tmp46B;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_Core_Opt*_tmp469;_LL1: _tmp469=_tmp468->pat_vars;_tmp46A=_tmp468->where_clause;_tmp46B=_tmp468->body;_tmp46C=_tmp468->loc;_LL2: {struct Cyc_Core_Opt*vds_opt=_tmp469;struct Cyc_Absyn_Exp*where_opt=_tmp46A;struct Cyc_Absyn_Stmt*body=_tmp46B;unsigned loc=_tmp46C;
struct _tuple1 _tmp46D=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds_opt))->v);struct _tuple1 _stmttmp84=_tmp46D;struct _tuple1 _tmp46E=_stmttmp84;struct Cyc_List_List*_tmp470;struct Cyc_List_List*_tmp46F;_LL4: _tmp46F=_tmp46E.f1;_tmp470=_tmp46E.f2;_LL5: {struct Cyc_List_List*roots=_tmp46F;struct Cyc_List_List*es=_tmp470;
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)vds_opt->v,roots != 0,loc,exp_loc);
# 2463
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp471=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp471;
# 2466
if(Cyc_Tcpat_has_vars(vds_opt))
({struct Cyc_List_List*_tmp765=({struct Cyc_List_List*_tmp473=_cycalloc(sizeof(*_tmp473));
({struct _tuple31*_tmp764=({struct _tuple31*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->f1=0,_tmp472->f2=body,_tmp472->f3=roots,_tmp472->f4=es;_tmp472;});_tmp473->hd=_tmp764;}),_tmp473->tl=old_unique_pat_vars;_tmp473;});
# 2467
env->unique_pat_vars=_tmp765;});
# 2470
if(where_opt != 0){
struct Cyc_Absyn_Exp*wexp=where_opt;
struct _tuple20 _tmp474=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple20 _stmttmp85=_tmp474;struct _tuple20 _tmp475=_stmttmp85;union Cyc_CfFlowInfo_FlowInfo _tmp477;union Cyc_CfFlowInfo_FlowInfo _tmp476;_LL7: _tmp476=_tmp475.f1;_tmp477=_tmp475.f2;_LL8: {union Cyc_CfFlowInfo_FlowInfo ft=_tmp476;union Cyc_CfFlowInfo_FlowInfo ff=_tmp477;
inflow=ff;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,ft,body,0);}}else{
# 2476
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,body,0);}
# 2478
env->unique_pat_vars=old_unique_pat_vars;{
union Cyc_CfFlowInfo_FlowInfo _tmp478=clause_outflow;if((_tmp478.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2483
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,clause_outflow,loc);
# 2485
if(scs->tl == 0)
return clause_outflow;else{
# 2490
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp479=0U;({unsigned _tmp767=body->loc;struct _fat_ptr _tmp766=({const char*_tmp47A="switch clause may implicitly fallthru";_tag_fat(_tmp47A,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp767,_tmp766,_tag_fat(_tmp479,sizeof(void*),0U));});});else{
# 2493
({void*_tmp47B=0U;({unsigned _tmp769=body->loc;struct _fat_ptr _tmp768=({const char*_tmp47C="switch clause may implicitly fallthru";_tag_fat(_tmp47C,sizeof(char),38U);});Cyc_Warn_warn(_tmp769,_tmp768,_tag_fat(_tmp47B,sizeof(void*),0U));});});}
# 2495
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2497
goto _LL9;}_LL9:;}}}}
# 2500
return Cyc_CfFlowInfo_BottomFL();}struct _tuple32{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned f3;};
# 2505
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple32*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2507
struct _tuple32*_tmp47D=vdenv;unsigned _tmp480;struct Cyc_Dict_Dict _tmp47F;struct Cyc_NewControlFlow_AnalEnv*_tmp47E;_LL1: _tmp47E=_tmp47D->f1;_tmp47F=_tmp47D->f2;_tmp480=_tmp47D->f3;_LL2: {struct Cyc_NewControlFlow_AnalEnv*env=_tmp47E;struct Cyc_Dict_Dict fd=_tmp47F;unsigned loc=_tmp480;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2510
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp632;_tmp632.tag=0U,_tmp632.f1=vd;_tmp632;});
struct _tuple16 _tmp481=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp76A=fd;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp76A,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp488=_cycalloc(sizeof(*_tmp488));*_tmp488=vdroot;_tmp488;}));}));struct _tuple16 _stmttmp86=_tmp481;struct _tuple16 _tmp482=_stmttmp86;void*_tmp483;_LL4: _tmp483=_tmp482.f1;_LL5: {void*rval=_tmp483;
void*_tmp484=rval;switch(*((int*)_tmp484)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp484)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2517
({struct Cyc_String_pa_PrintArg_struct _tmp487=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,({struct _fat_ptr _tmp76B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp631.f1=_tmp76B;});_tmp631;});void*_tmp485[1U];_tmp485[0]=& _tmp487;({unsigned _tmp76D=loc;struct _fat_ptr _tmp76C=({const char*_tmp486="unique pointers may still exist after variable %s goes out of scope";_tag_fat(_tmp486,sizeof(char),68U);});Cyc_Warn_warn(_tmp76D,_tmp76C,_tag_fat(_tmp485,sizeof(void*),1U));});});
# 2519
goto _LL6;}_LL6:;}}}}
# 2524
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp489=inflow;struct Cyc_Dict_Dict _tmp48A;if((_tmp489.ReachableFL).tag == 2){_LL1: _tmp48A=((_tmp489.ReachableFL).val).f1;_LL2: {struct Cyc_Dict_Dict fd=_tmp48A;
# 2527
struct _tuple32 _tmp48B=({struct _tuple32 _tmp633;_tmp633.f1=env,_tmp633.f2=fd,_tmp633.f3=decl->loc;_tmp633;});struct _tuple32 vdenv=_tmp48B;
struct Cyc_CfFlowInfo_FlowEnv*_tmp48C=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp48C;
{void*_tmp48D=decl->r;void*_stmttmp87=_tmp48D;void*_tmp48E=_stmttmp87;struct Cyc_List_List*_tmp48F;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Vardecl*_tmp491;switch(*((int*)_tmp48E)){case 0U: _LL6: _tmp491=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp48E)->f1;_LL7: {struct Cyc_Absyn_Vardecl*vd=_tmp491;
# 2531
Cyc_NewControlFlow_check_dropped_unique_vd(& vdenv,vd);
goto _LL5;}case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48E)->f2 != 0){_LL8: _tmp490=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48E)->f2)->v;_LL9: {struct Cyc_List_List*vds=_tmp490;
# 2534
struct _tuple1 _tmp492=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds);struct _tuple1 _stmttmp88=_tmp492;struct _tuple1 _tmp493=_stmttmp88;struct Cyc_List_List*_tmp494;_LLF: _tmp494=_tmp493.f1;_LL10: {struct Cyc_List_List*vs=_tmp494;
struct Cyc_List_List*_tmp495=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)(vs);{struct Cyc_List_List*vs=_tmp495;
_tmp48F=vs;goto _LLB;}}}}else{goto _LLC;}case 3U: _LLA: _tmp48F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp48E)->f1;_LLB: {struct Cyc_List_List*vds=_tmp48F;
# 2538
((void(*)(void(*f)(struct _tuple32*,struct Cyc_Absyn_Vardecl*),struct _tuple32*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& vdenv,vds);
goto _LL5;}default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 2542
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2545
return;}
# 2551
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2555
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple31*_tmp496=(struct _tuple31*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple31*_stmttmp89=_tmp496;struct _tuple31*_tmp497=_stmttmp89;struct Cyc_List_List*_tmp49B;struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Stmt*_tmp499;int _tmp498;_LL1: _tmp498=_tmp497->f1;_tmp499=_tmp497->f2;_tmp49A=_tmp497->f3;_tmp49B=_tmp497->f4;_LL2: {int is_open=_tmp498;struct Cyc_Absyn_Stmt*pat_stmt=_tmp499;struct Cyc_List_List*roots=_tmp49A;struct Cyc_List_List*es=_tmp49B;
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,is_open,inflow,dest->loc);}}}
# 2560
return inflow;}
# 2566
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple18*rval_opt){
# 2568
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple21 _tmp49C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple21 _stmttmp8A=_tmp49C;struct _tuple21 _tmp49D=_stmttmp8A;union Cyc_CfFlowInfo_FlowInfo*_tmp49F;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp49E;_LL1: _tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;_LL2: {struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp49E;union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp49F;
inflow=*sflow;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp4A0=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp4A0;
# 2575
void*_tmp4A1=s->r;void*_stmttmp8B=_tmp4A1;void*_tmp4A2=_stmttmp8B;struct Cyc_Absyn_Stmt*_tmp4A3;struct Cyc_Absyn_Stmt*_tmp4A5;struct Cyc_Absyn_Decl*_tmp4A4;struct Cyc_Absyn_Stmt*_tmp4A9;unsigned _tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Vardecl*_tmp4A6;struct Cyc_Absyn_Stmt*_tmp4AD;unsigned _tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_List_List*_tmp4AA;void*_tmp4B0;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Stmt*_tmp4AE;void*_tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Switch_clause*_tmp4B5;struct Cyc_List_List*_tmp4B4;struct Cyc_Absyn_Stmt*_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Stmt*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Stmt*_tmp4BC;struct Cyc_Absyn_Stmt*_tmp4C1;struct Cyc_Absyn_Stmt*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Stmt*_tmp4C4;struct Cyc_Absyn_Stmt*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Stmt*_tmp4C6;struct Cyc_Absyn_Stmt*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;switch(*((int*)_tmp4A2)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1 == 0){_LL6: _LL7:
# 2579
 if(env->noreturn)
({void*_tmp4C9=0U;({unsigned _tmp76F=s->loc;struct _fat_ptr _tmp76E=({const char*_tmp4CA="`noreturn' function might return";_tag_fat(_tmp4CA,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp76F,_tmp76E,_tag_fat(_tmp4C9,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp4C8=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp4C8;
# 2585
if(env->noreturn)
({void*_tmp4CB=0U;({unsigned _tmp771=s->loc;struct _fat_ptr _tmp770=({const char*_tmp4CC="`noreturn' function might return";_tag_fat(_tmp4CC,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp771,_tmp770,_tag_fat(_tmp4CB,sizeof(void*),0U));});});{
struct _tuple17 _tmp4CD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(e));struct _tuple17 _stmttmp8C=_tmp4CD;struct _tuple17 _tmp4CE=_stmttmp8C;void*_tmp4D0;union Cyc_CfFlowInfo_FlowInfo _tmp4CF;_LL2D: _tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;_LL2E: {union Cyc_CfFlowInfo_FlowInfo f=_tmp4CF;void*r=_tmp4D0;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,r);
Cyc_NewControlFlow_check_init_params(s->loc,env,f);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,f,s->loc);
return Cyc_CfFlowInfo_BottomFL();}}}}case 1U: _LLA: _tmp4C7=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_LLB: {struct Cyc_Absyn_Exp*e=_tmp4C7;
# 2594
struct _tuple18*_tmp4D1=rval_opt;int _tmp4D3;void**_tmp4D2;if(_tmp4D1 != 0){_LL30: _tmp4D2=(void**)& _tmp4D1->f1;_tmp4D3=_tmp4D1->f2;_LL31: {void**rval=_tmp4D2;int copy_ctxt=_tmp4D3;
# 2596
struct _tuple17 _tmp4D4=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e);struct _tuple17 _stmttmp8D=_tmp4D4;struct _tuple17 _tmp4D5=_stmttmp8D;void*_tmp4D7;union Cyc_CfFlowInfo_FlowInfo _tmp4D6;_LL35: _tmp4D6=_tmp4D5.f1;_tmp4D7=_tmp4D5.f2;_LL36: {union Cyc_CfFlowInfo_FlowInfo f=_tmp4D6;void*r=_tmp4D7;
*rval=r;
return f;}}}else{_LL32: _LL33:
# 2600
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e)).f1;}_LL2F:;}case 2U: _LLC: _tmp4C5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4C6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LLD: {struct Cyc_Absyn_Stmt*s1=_tmp4C5;struct Cyc_Absyn_Stmt*s2=_tmp4C6;
# 2604
return({struct Cyc_NewControlFlow_AnalEnv*_tmp774=env;union Cyc_CfFlowInfo_FlowInfo _tmp773=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);struct Cyc_Absyn_Stmt*_tmp772=s2;Cyc_NewControlFlow_anal_stmt(_tmp774,_tmp773,_tmp772,rval_opt);});}case 4U: _LLE: _tmp4C2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4C3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_tmp4C4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3;_LLF: {struct Cyc_Absyn_Exp*e=_tmp4C2;struct Cyc_Absyn_Stmt*s1=_tmp4C3;struct Cyc_Absyn_Stmt*s2=_tmp4C4;
# 2607
struct _tuple20 _tmp4D8=Cyc_NewControlFlow_anal_test(env,inflow,e);struct _tuple20 _stmttmp8E=_tmp4D8;struct _tuple20 _tmp4D9=_stmttmp8E;union Cyc_CfFlowInfo_FlowInfo _tmp4DB;union Cyc_CfFlowInfo_FlowInfo _tmp4DA;_LL38: _tmp4DA=_tmp4D9.f1;_tmp4DB=_tmp4D9.f2;_LL39: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp4DA;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp4DB;
# 2614
union Cyc_CfFlowInfo_FlowInfo _tmp4DC=Cyc_NewControlFlow_anal_stmt(env,f1f,s2,0);union Cyc_CfFlowInfo_FlowInfo ff=_tmp4DC;
union Cyc_CfFlowInfo_FlowInfo _tmp4DD=Cyc_NewControlFlow_anal_stmt(env,f1t,s1,0);union Cyc_CfFlowInfo_FlowInfo ft=_tmp4DD;
return Cyc_CfFlowInfo_join_flow(fenv,ft,ff);}}case 5U: _LL10: _tmp4BF=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1).f1;_tmp4C0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1).f2;_tmp4C1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL11: {struct Cyc_Absyn_Exp*e=_tmp4BF;struct Cyc_Absyn_Stmt*cont=_tmp4C0;struct Cyc_Absyn_Stmt*body=_tmp4C1;
# 2622
struct _tuple21 _tmp4DE=Cyc_NewControlFlow_pre_stmt_check(env,inflow,cont);struct _tuple21 _stmttmp8F=_tmp4DE;struct _tuple21 _tmp4DF=_stmttmp8F;union Cyc_CfFlowInfo_FlowInfo*_tmp4E0;_LL3B: _tmp4E0=_tmp4DF.f2;_LL3C: {union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_tmp4E0;
union Cyc_CfFlowInfo_FlowInfo _tmp4E1=*eflow_ptr;union Cyc_CfFlowInfo_FlowInfo e_inflow=_tmp4E1;
struct _tuple20 _tmp4E2=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _stmttmp90=_tmp4E2;struct _tuple20 _tmp4E3=_stmttmp90;union Cyc_CfFlowInfo_FlowInfo _tmp4E5;union Cyc_CfFlowInfo_FlowInfo _tmp4E4;_LL3E: _tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;_LL3F: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp4E4;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp4E5;
union Cyc_CfFlowInfo_FlowInfo _tmp4E6=Cyc_NewControlFlow_anal_stmt(env,f1t,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp4E6;
Cyc_NewControlFlow_update_flow(env,cont,body_outflow);
return f1f;}}}case 14U: _LL12: _tmp4BC=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4BD=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f1;_tmp4BE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f2;_LL13: {struct Cyc_Absyn_Stmt*body=_tmp4BC;struct Cyc_Absyn_Exp*e=_tmp4BD;struct Cyc_Absyn_Stmt*cont=_tmp4BE;
# 2632
union Cyc_CfFlowInfo_FlowInfo _tmp4E7=Cyc_NewControlFlow_anal_stmt(env,inflow,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp4E7;
struct _tuple21 _tmp4E8=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _stmttmp91=_tmp4E8;struct _tuple21 _tmp4E9=_stmttmp91;union Cyc_CfFlowInfo_FlowInfo*_tmp4EA;_LL41: _tmp4EA=_tmp4E9.f2;_LL42: {union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_tmp4EA;
union Cyc_CfFlowInfo_FlowInfo _tmp4EB=*eflow_ptr;union Cyc_CfFlowInfo_FlowInfo e_inflow=_tmp4EB;
struct _tuple20 _tmp4EC=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _stmttmp92=_tmp4EC;struct _tuple20 _tmp4ED=_stmttmp92;union Cyc_CfFlowInfo_FlowInfo _tmp4EF;union Cyc_CfFlowInfo_FlowInfo _tmp4EE;_LL44: _tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;_LL45: {union Cyc_CfFlowInfo_FlowInfo f1t=_tmp4EE;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp4EF;
Cyc_NewControlFlow_update_flow(env,body,f1t);
return f1f;}}}case 9U: _LL14: _tmp4B6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4B7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f1;_tmp4B8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f2;_tmp4B9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3).f1;_tmp4BA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3).f2;_tmp4BB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f4;_LL15: {struct Cyc_Absyn_Exp*e1=_tmp4B6;struct Cyc_Absyn_Exp*e2=_tmp4B7;struct Cyc_Absyn_Stmt*guard=_tmp4B8;struct Cyc_Absyn_Exp*e3=_tmp4B9;struct Cyc_Absyn_Stmt*cont=_tmp4BA;struct Cyc_Absyn_Stmt*body=_tmp4BB;
# 2641
union Cyc_CfFlowInfo_FlowInfo _tmp4F0=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1)).f1;union Cyc_CfFlowInfo_FlowInfo e1_outflow=_tmp4F0;
struct _tuple21 _tmp4F1=Cyc_NewControlFlow_pre_stmt_check(env,e1_outflow,guard);struct _tuple21 _stmttmp93=_tmp4F1;struct _tuple21 _tmp4F2=_stmttmp93;union Cyc_CfFlowInfo_FlowInfo*_tmp4F3;_LL47: _tmp4F3=_tmp4F2.f2;_LL48: {union Cyc_CfFlowInfo_FlowInfo*e2flow_ptr=_tmp4F3;
union Cyc_CfFlowInfo_FlowInfo _tmp4F4=*e2flow_ptr;union Cyc_CfFlowInfo_FlowInfo e2_inflow=_tmp4F4;
struct _tuple20 _tmp4F5=Cyc_NewControlFlow_anal_test(env,e2_inflow,e2);struct _tuple20 _stmttmp94=_tmp4F5;struct _tuple20 _tmp4F6=_stmttmp94;union Cyc_CfFlowInfo_FlowInfo _tmp4F8;union Cyc_CfFlowInfo_FlowInfo _tmp4F7;_LL4A: _tmp4F7=_tmp4F6.f1;_tmp4F8=_tmp4F6.f2;_LL4B: {union Cyc_CfFlowInfo_FlowInfo f2t=_tmp4F7;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp4F8;
union Cyc_CfFlowInfo_FlowInfo _tmp4F9=Cyc_NewControlFlow_anal_stmt(env,f2t,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp4F9;
struct _tuple21 _tmp4FA=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _stmttmp95=_tmp4FA;struct _tuple21 _tmp4FB=_stmttmp95;union Cyc_CfFlowInfo_FlowInfo*_tmp4FC;_LL4D: _tmp4FC=_tmp4FB.f2;_LL4E: {union Cyc_CfFlowInfo_FlowInfo*e3flow_ptr=_tmp4FC;
union Cyc_CfFlowInfo_FlowInfo _tmp4FD=*e3flow_ptr;union Cyc_CfFlowInfo_FlowInfo e3_inflow=_tmp4FD;
union Cyc_CfFlowInfo_FlowInfo _tmp4FE=(Cyc_NewControlFlow_anal_Rexp(env,0,e3_inflow,e3)).f1;union Cyc_CfFlowInfo_FlowInfo e3_outflow=_tmp4FE;
Cyc_NewControlFlow_update_flow(env,guard,e3_outflow);
return f2f;}}}}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2 != 0){_LL16: _tmp4B4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4B5=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL17: {struct Cyc_List_List*es=_tmp4B4;struct Cyc_Absyn_Switch_clause*destclause=_tmp4B5;
# 2653
struct _tuple24 _tmp4FF=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp96=_tmp4FF;struct _tuple24 _tmp500=_stmttmp96;struct Cyc_List_List*_tmp502;union Cyc_CfFlowInfo_FlowInfo _tmp501;_LL50: _tmp501=_tmp500.f1;_tmp502=_tmp500.f2;_LL51: {union Cyc_CfFlowInfo_FlowInfo f=_tmp501;struct Cyc_List_List*rvals=_tmp502;
# 2655
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,destclause->body);{
# 2657
struct Cyc_List_List*_tmp503=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(destclause->pat_vars))->v)).f1);struct Cyc_List_List*vds=_tmp503;
f=Cyc_NewControlFlow_add_vars(fenv,f,vds,fenv->unknown_all,s->loc,0);
# 2660
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(destclause->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp504=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp97=_tmp504;struct _tuple25*_tmp505=_stmttmp97;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Vardecl**_tmp506;_LL53: _tmp506=_tmp505->f1;_tmp507=_tmp505->f2;_LL54: {struct Cyc_Absyn_Vardecl**vd=_tmp506;struct Cyc_Absyn_Exp*ve=_tmp507;
if(vd != 0){
f=({struct Cyc_CfFlowInfo_FlowEnv*_tmp77A=fenv;struct Cyc_NewControlFlow_AnalEnv*_tmp779=env;union Cyc_CfFlowInfo_FlowInfo _tmp778=f;struct Cyc_Absyn_Vardecl*_tmp777=*vd;struct Cyc_Absyn_Exp*_tmp776=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;void*_tmp775=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp77A,_tmp779,_tmp778,_tmp777,_tmp776,_tmp775,s->loc);});
rvals=rvals->tl;
es=es->tl;}}}}
# 2668
Cyc_NewControlFlow_update_flow(env,destclause->body,f);
return Cyc_CfFlowInfo_BottomFL();}}}}else{_LL2A: _LL2B:
# 2832
({void*_tmp536=0U;({struct _fat_ptr _tmp77B=({const char*_tmp537="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_fat(_tmp537,sizeof(char),54U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp77B,_tag_fat(_tmp536,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2674
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp508=inflow;if((_tmp508.ReachableFL).tag == 2){_LL56: _LL57:
# 2677
 if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type)||
 Cyc_Tcutil_is_any_int_type(env->return_type))
({void*_tmp509=0U;({unsigned _tmp77D=s->loc;struct _fat_ptr _tmp77C=({const char*_tmp50A="break may cause function not to return a value";_tag_fat(_tmp50A,sizeof(char),47U);});Cyc_Warn_warn(_tmp77D,_tmp77C,_tag_fat(_tmp509,sizeof(void*),0U));});});else{
# 2682
({void*_tmp50B=0U;({unsigned _tmp77F=s->loc;struct _fat_ptr _tmp77E=({const char*_tmp50C="break may cause function not to return a value";_tag_fat(_tmp50C,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp77F,_tmp77E,_tag_fat(_tmp50B,sizeof(void*),0U));});});}}
# 2684
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2687
if(env->noreturn)
({void*_tmp50D=0U;({unsigned _tmp781=s->loc;struct _fat_ptr _tmp780=({const char*_tmp50E="`noreturn' function might return";_tag_fat(_tmp50E,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp781,_tmp780,_tag_fat(_tmp50D,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2693
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2697
struct Cyc_Absyn_Stmt*_tmp50F=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp50F;
if(dest == 0)
({void*_tmp510=0U;({unsigned _tmp783=s->loc;struct _fat_ptr _tmp782=({const char*_tmp511="jump has no target (should be impossible)";_tag_fat(_tmp511,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp783,_tmp782,_tag_fat(_tmp510,sizeof(void*),0U));});});
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(dest));
# 2702
Cyc_NewControlFlow_update_flow(env,dest,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp4B1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4B2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_tmp4B3=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3;_LL1F: {struct Cyc_Absyn_Exp*e=_tmp4B1;struct Cyc_List_List*scs=_tmp4B2;void*dec_tree=_tmp4B3;
# 2708
return Cyc_NewControlFlow_anal_scs(env,inflow,scs,e->loc);}case 15U: _LL20: _tmp4AE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4AF=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_tmp4B0=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3;_LL21: {struct Cyc_Absyn_Stmt*s1=_tmp4AE;struct Cyc_List_List*scs=_tmp4AF;void*dec_tree=_tmp4B0;
# 2713
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2721
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2724
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,scs,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp512=scs_outflow;if((_tmp512.BottomFL).tag == 1){_LL5B: _LL5C:
 goto _LL5A;}else{_LL5D: _LL5E:
({void*_tmp513=0U;({unsigned _tmp785=s->loc;struct _fat_ptr _tmp784=({const char*_tmp514="last catch clause may implicitly fallthru";_tag_fat(_tmp514,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp785,_tmp784,_tag_fat(_tmp513,sizeof(void*),0U));});});}_LL5A:;}
# 2730
outflow=s1_outflow;
# 2732
return outflow;}}}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f2 != 0){_LL22: _tmp4AA=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f2)->v;_tmp4AB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f3;_tmp4AC=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->loc;_tmp4AD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL23: {struct Cyc_List_List*vds=_tmp4AA;struct Cyc_Absyn_Exp*e=_tmp4AB;unsigned loc=_tmp4AC;struct Cyc_Absyn_Stmt*s1=_tmp4AD;
# 2742
struct _tuple1 _tmp515=Cyc_NewControlFlow_get_unconsume_pat_vars(vds);struct _tuple1 _stmttmp98=_tmp515;struct _tuple1 _tmp516=_stmttmp98;struct Cyc_List_List*_tmp518;struct Cyc_List_List*_tmp517;_LL60: _tmp517=_tmp516.f1;_tmp518=_tmp516.f2;_LL61: {struct Cyc_List_List*roots=_tmp517;struct Cyc_List_List*es=_tmp518;
inflow=Cyc_NewControlFlow_initialize_pat_vars(fenv,env,inflow,vds,roots != 0,loc,e->loc);{
struct Cyc_List_List*_tmp519=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp519;
# 2746
({struct Cyc_List_List*_tmp787=({struct Cyc_List_List*_tmp51B=_cycalloc(sizeof(*_tmp51B));
({struct _tuple31*_tmp786=({struct _tuple31*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->f1=0,_tmp51A->f2=s,_tmp51A->f3=roots,_tmp51A->f4=es;_tmp51A;});_tmp51B->hd=_tmp786;}),_tmp51B->tl=old_unique_pat_vars;_tmp51B;});
# 2746
env->unique_pat_vars=_tmp787;});
# 2751
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2755
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,inflow,loc);
# 2759
return inflow;}}}}else{goto _LL26;}case 4U: _LL24: _tmp4A6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f2;_tmp4A7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f3;_tmp4A8=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->loc;_tmp4A9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;if(_tmp4A7 != 0){_LL25: {struct Cyc_Absyn_Vardecl*vd=_tmp4A6;struct Cyc_Absyn_Exp*open_exp_opt=_tmp4A7;unsigned loc=_tmp4A8;struct Cyc_Absyn_Stmt*s1=_tmp4A9;
# 2771
struct Cyc_List_List l=({struct Cyc_List_List _tmp635;_tmp635.hd=open_exp_opt,_tmp635.tl=0;_tmp635;});
union Cyc_CfFlowInfo_FlowInfo _tmp51C=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);union Cyc_CfFlowInfo_FlowInfo f=_tmp51C;
struct _tuple19 _tmp51D=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,open_exp_opt);struct _tuple19 _stmttmp99=_tmp51D;struct _tuple19 _tmp51E=_stmttmp99;union Cyc_CfFlowInfo_AbsLVal _tmp51F;_LL63: _tmp51F=_tmp51E.f2;_LL64: {union Cyc_CfFlowInfo_AbsLVal lval=_tmp51F;
struct _tuple17 _tmp520=Cyc_NewControlFlow_anal_Rexp(env,1,f,open_exp_opt);struct _tuple17 _stmttmp9A=_tmp520;struct _tuple17 _tmp521=_stmttmp9A;union Cyc_CfFlowInfo_FlowInfo _tmp522;_LL66: _tmp522=_tmp521.f1;_LL67: {union Cyc_CfFlowInfo_FlowInfo f=_tmp522;
struct Cyc_List_List*_tmp523=0;struct Cyc_List_List*roots=_tmp523;
struct Cyc_List_List*_tmp524=0;struct Cyc_List_List*es=_tmp524;
{union Cyc_CfFlowInfo_FlowInfo _tmp525=f;struct Cyc_List_List*_tmp527;struct Cyc_Dict_Dict _tmp526;if((_tmp525.ReachableFL).tag == 2){_LL69: _tmp526=((_tmp525.ReachableFL).val).f1;_tmp527=((_tmp525.ReachableFL).val).f2;_LL6A: {struct Cyc_Dict_Dict fd=_tmp526;struct Cyc_List_List*relns=_tmp527;
# 2779
{union Cyc_CfFlowInfo_AbsLVal _tmp528=lval;struct Cyc_CfFlowInfo_Place*_tmp529;if((_tmp528.PlaceL).tag == 1){_LL6E: _tmp529=(_tmp528.PlaceL).val;_LL6F: {struct Cyc_CfFlowInfo_Place*p=_tmp529;
# 2783
void*_tmp52A=Cyc_CfFlowInfo_lookup_place(fd,p);void*new_rval=_tmp52A;
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->tag=8U,_tmp52B->f1=vd,_tmp52B->f2=new_rval;_tmp52B;});
fd=Cyc_CfFlowInfo_assign_place(fenv,open_exp_opt->loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp52C=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->tag=0U,_tmp531->f1=vd;_tmp531;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp52C;
struct Cyc_CfFlowInfo_Place*_tmp52D=({struct Cyc_CfFlowInfo_Place*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->root=(void*)root,_tmp530->path=0;_tmp530;});struct Cyc_CfFlowInfo_Place*rp=_tmp52D;
roots=({struct Cyc_List_List*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->hd=rp,_tmp52E->tl=roots;_tmp52E;});
es=({struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->hd=open_exp_opt,_tmp52F->tl=es;_tmp52F;});
goto _LL6D;}}}else{_LL70: _LL71:
# 2798
 goto _LL6D;}_LL6D:;}
# 2800
goto _LL68;}}else{_LL6B: _LL6C:
# 2802
 goto _LL68;}_LL68:;}{
# 2805
struct Cyc_List_List _tmp532=({struct Cyc_List_List _tmp634;_tmp634.hd=vd,_tmp634.tl=0;_tmp634;});struct Cyc_List_List vds=_tmp532;
f=Cyc_NewControlFlow_add_vars(fenv,f,& vds,fenv->unknown_all,loc,0);{
# 2809
struct Cyc_List_List*_tmp533=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp533;
({struct Cyc_List_List*_tmp789=({struct Cyc_List_List*_tmp535=_cycalloc(sizeof(*_tmp535));({struct _tuple31*_tmp788=({struct _tuple31*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->f1=1,_tmp534->f2=s,_tmp534->f3=roots,_tmp534->f4=es;_tmp534;});_tmp535->hd=_tmp788;}),_tmp535->tl=old_unique_pat_vars;_tmp535;});env->unique_pat_vars=_tmp789;});
# 2814
f=Cyc_NewControlFlow_anal_stmt(env,f,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2818
f=Cyc_NewControlFlow_unconsume_params(env,roots,es,1,f,loc);
# 2822
return f;}}}}}}else{goto _LL26;}default: _LL26: _tmp4A4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp4A5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL27: {struct Cyc_Absyn_Decl*d=_tmp4A4;struct Cyc_Absyn_Stmt*s=_tmp4A5;
# 2825
outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp78C=env;union Cyc_CfFlowInfo_FlowInfo _tmp78B=Cyc_NewControlFlow_anal_decl(env,inflow,d);struct Cyc_Absyn_Stmt*_tmp78A=s;Cyc_NewControlFlow_anal_stmt(_tmp78C,_tmp78B,_tmp78A,rval_opt);});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,d);
return outflow;}}default: _LL28: _tmp4A3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL29: {struct Cyc_Absyn_Stmt*s=_tmp4A3;
# 2830
return Cyc_NewControlFlow_anal_stmt(env,inflow,s,rval_opt);}}_LL3:;}}}
# 2836
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2840
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp538=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp538;
void*_tmp539=decl->r;void*_stmttmp9B=_tmp539;void*_tmp53A=_stmttmp9B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp53C;struct Cyc_Absyn_Tvar*_tmp53B;struct Cyc_Absyn_Fndecl*_tmp53E;struct Cyc_List_List*_tmp53F;struct Cyc_Absyn_Vardecl*_tmp540;switch(*((int*)_tmp53A)){case 0U: _LL1: _tmp540=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp540;
# 2848
if((int)vd->sc == (int)Cyc_Absyn_Static)
return inflow;{
# 2853
struct Cyc_List_List _tmp541=({struct Cyc_List_List _tmp636;_tmp636.hd=vd,_tmp636.tl=0;_tmp636;});struct Cyc_List_List vds=_tmp541;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp542=vd->initializer;struct Cyc_Absyn_Exp*e=_tmp542;
if(e == 0)
return inflow;{
struct _tuple17 _tmp543=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e);struct _tuple17 _stmttmp9C=_tmp543;struct _tuple17 _tmp544=_stmttmp9C;void*_tmp546;union Cyc_CfFlowInfo_FlowInfo _tmp545;_LLC: _tmp545=_tmp544.f1;_tmp546=_tmp544.f2;_LLD: {union Cyc_CfFlowInfo_FlowInfo f=_tmp545;void*r=_tmp546;
return Cyc_NewControlFlow_do_initialize_var(fenv,env,f,vd,e,r,decl->loc);}}}}}case 3U: _LL3: _tmp53F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_LL4: {struct Cyc_List_List*vds=_tmp53F;
# 2862
return Cyc_NewControlFlow_add_vars(fenv,inflow,vds,fenv->unknown_none,decl->loc,0);}case 1U: _LL5: _tmp53E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp53E;
# 2865
Cyc_NewControlFlow_check_nested_fun(env->all_tables,fenv,inflow,fd);{
struct Cyc_Absyn_Vardecl*_tmp547=(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);struct Cyc_Absyn_Vardecl*vd=_tmp547;
# 2870
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp790=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp78F=inflow;struct Cyc_List_List*_tmp78E=({struct Cyc_List_List*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548->hd=vd,_tmp548->tl=0;_tmp548;});void*_tmp78D=fenv->unknown_all;Cyc_NewControlFlow_add_vars(_tmp790,_tmp78F,_tmp78E,_tmp78D,decl->loc,0);});}}case 4U: _LL7: _tmp53B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53A)->f1;_tmp53C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53A)->f2;_tmp53D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53A)->f3;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp53B;struct Cyc_Absyn_Vardecl*vd=_tmp53C;struct Cyc_Absyn_Exp*open_exp_opt=_tmp53D;
# 2873
if(open_exp_opt != 0)
({void*_tmp549=0U;({struct _fat_ptr _tmp791=({const char*_tmp54A="found open expression in declaration!";_tag_fat(_tmp54A,sizeof(char),38U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp791,_tag_fat(_tmp549,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp54B=({struct Cyc_List_List _tmp637;_tmp637.hd=vd,_tmp637.tl=0;_tmp637;});struct Cyc_List_List vds=_tmp54B;
return Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_all,decl->loc,0);}}default: _LL9: _LLA:
# 2879
({void*_tmp54C=0U;({struct _fat_ptr _tmp792=({const char*_tmp54D="anal_decl: unexpected decl variant";_tag_fat(_tmp54D,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp792,_tag_fat(_tmp54C,sizeof(void*),0U));});});}_LL0:;}
# 2887
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp54E;_push_handler(& _tmp54E);{int _tmp550=0;if(setjmp(_tmp54E.handler))_tmp550=1;if(!_tmp550){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp795=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp794=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp793=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp795,_tmp794,_tmp793,fd);});}
# 2889
;_pop_handler();}else{void*_tmp54F=(void*)Cyc_Core_get_exn_thrown();void*_tmp551=_tmp54F;void*_tmp552;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp551)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2895
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=Cyc_Dict_Absent;_tmp553;}));}}else{_LL3: _tmp552=_tmp551;_LL4: {void*exn=_tmp552;(int)_rethrow(exn);}}_LL0:;}}}
# 2901
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2906
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp554=r;unsigned _tmp555;if((_tmp554.RParam).tag == 5){_LL1: _tmp555=(_tmp554.RParam).val;_LL2: {unsigned i=_tmp555;
# 2909
struct Cyc_Absyn_Vardecl*_tmp556=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)i);struct Cyc_Absyn_Vardecl*vd=_tmp556;
if(!vd->escapes)
return Cyc_Relations_RVar(vd);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2920
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){
# 2922
struct _RegionHandle _tmp557=_new_region("r");struct _RegionHandle*r=& _tmp557;_push_region(r);
{int _tmp558=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(param_vardecls);int len=_tmp558;
struct _fat_ptr _tmp559=({unsigned _tmp566=(unsigned)len;int*_tmp565=({struct _RegionHandle*_tmp796=r;_region_malloc(_tmp796,_check_times(_tmp566,sizeof(int)));});({{unsigned _tmp638=(unsigned)len;unsigned i;for(i=0;i < _tmp638;++ i){_tmp565[i]=0;}}0;});_tag_fat(_tmp565,sizeof(int),_tmp566);});struct _fat_ptr cons=_tmp559;
for(0;atts != 0;atts=atts->tl){
void*_tmp55A=(void*)atts->hd;void*_stmttmp9D=_tmp55A;void*_tmp55B=_stmttmp9D;int _tmp55C;int _tmp55D;switch(*((int*)_tmp55B)){case 22U: _LL1: _tmp55D=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp55B)->f1;_LL2: {int i=_tmp55D;
_tmp55C=i;goto _LL4;}case 21U: _LL3: _tmp55C=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp55B)->f1;_LL4: {int i=_tmp55C;
*((int*)_check_fat_subscript(cons,sizeof(int),i - 1))=1;goto _LL0;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2932
struct Cyc_List_List*noconsume_roots=0;
{int i=0;for(0;param_vardecls != 0;(param_vardecls=param_vardecls->tl,i ++)){
struct Cyc_Absyn_Vardecl*_tmp55E=(struct Cyc_Absyn_Vardecl*)param_vardecls->hd;struct Cyc_Absyn_Vardecl*vd=_tmp55E;
if(Cyc_Tcutil_is_noalias_pointer(vd->type,0)&& !(*((int*)_check_fat_subscript(cons,sizeof(int),i)))){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp55F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=0U,_tmp563->f1=vd;_tmp563;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp55F;
struct Cyc_CfFlowInfo_Place*_tmp560=({struct Cyc_CfFlowInfo_Place*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->root=(void*)root,_tmp562->path=0;_tmp562;});struct Cyc_CfFlowInfo_Place*rp=_tmp560;
noconsume_roots=({struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->hd=rp,_tmp561->tl=noconsume_roots;_tmp561;});}}}{
# 2941
struct Cyc_List_List*_tmp564=noconsume_roots;_npop_handler(0U);return _tmp564;}}}
# 2923
;_pop_region();}
# 2944
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2948
unsigned _tmp567=(fd->body)->loc;unsigned loc=_tmp567;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,loc,1);{
# 2953
struct Cyc_List_List*param_roots=0;
struct _tuple15 _tmp568=({union Cyc_CfFlowInfo_FlowInfo _tmp590=inflow;if((_tmp590.ReachableFL).tag != 2)_throw_match();(_tmp590.ReachableFL).val;});struct _tuple15 _stmttmp9E=_tmp568;struct _tuple15 _tmp569=_stmttmp9E;struct Cyc_List_List*_tmp56B;struct Cyc_Dict_Dict _tmp56A;_LL1: _tmp56A=_tmp569.f1;_tmp56B=_tmp569.f2;_LL2: {struct Cyc_Dict_Dict d=_tmp56A;struct Cyc_List_List*relns=_tmp56B;
# 2957
struct Cyc_List_List*_tmp56C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vardecls=_tmp56C;
{struct Cyc_List_List*_tmp56D=(fd->i).requires_relns;struct Cyc_List_List*reqs=_tmp56D;for(0;reqs != 0;reqs=reqs->tl){
struct Cyc_Relations_Reln*_tmp56E=(struct Cyc_Relations_Reln*)reqs->hd;struct Cyc_Relations_Reln*req=_tmp56E;
relns=({struct _RegionHandle*_tmp79A=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp799=Cyc_NewControlFlow_translate_rop(vardecls,req->rop1);enum Cyc_Relations_Relation _tmp798=req->relation;union Cyc_Relations_RelnOp _tmp797=
Cyc_NewControlFlow_translate_rop(vardecls,req->rop2);
# 2960
Cyc_Relations_add_relation(_tmp79A,_tmp799,_tmp798,_tmp797,relns);});}}{
# 2966
struct Cyc_List_List*atts;
{void*_tmp56F=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmp9F=_tmp56F;void*_tmp570=_stmttmp9F;struct Cyc_List_List*_tmp571;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp570)->tag == 5U){_LL4: _tmp571=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp570)->f1).attributes;_LL5: {struct Cyc_List_List*as=_tmp571;
atts=as;goto _LL3;}}else{_LL6: _LL7:
({void*_tmp572=0U;({struct _fat_ptr _tmp79B=({const char*_tmp573="check_fun: non-function type cached with fndecl_t";_tag_fat(_tmp573,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp79B,_tag_fat(_tmp572,sizeof(void*),0U));});});}_LL3:;}{
# 2971
struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,atts);
# 2974
for(0;atts != 0;atts=atts->tl){
void*_tmp574=(void*)atts->hd;void*_stmttmpA0=_tmp574;void*_tmp575=_stmttmpA0;int _tmp576;int _tmp577;switch(*((int*)_tmp575)){case 21U: _LL9: _tmp577=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp575)->f1;_LLA: {int i=_tmp577;
# 2977
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp578=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,i - 1);struct Cyc_Absyn_Vardecl*vd=_tmp578;
void*t=Cyc_Absyn_compress(vd->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp579=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp79E=fenv;void*_tmp79D=elttype;struct Cyc_Absyn_Exp*_tmp79C=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp79E,_tmp79D,_tmp79C,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
# 2981
void*rval=_tmp579;
# 2984
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp57A=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->tag=2U,_tmp57F->f1=i,_tmp57F->f2=t;_tmp57F;});struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r=_tmp57A;
struct Cyc_CfFlowInfo_Place*_tmp57B=({struct Cyc_CfFlowInfo_Place*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->root=(void*)r,_tmp57E->path=0;_tmp57E;});struct Cyc_CfFlowInfo_Place*rp=_tmp57B;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,(void*)r,rval);
d=({struct Cyc_Dict_Dict _tmp7A0=d;void*_tmp79F=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->tag=0U,_tmp57C->f1=vd;_tmp57C;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7A0,_tmp79F,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=4U,_tmp57D->f1=rp;_tmp57D;}));});
goto _LL8;}case 20U: _LLB: _tmp576=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp575)->f1;_LLC: {int i=_tmp576;
# 2990
struct Cyc_Absyn_Vardecl*_tmp580=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,i - 1);struct Cyc_Absyn_Vardecl*vd=_tmp580;
void*elttype=Cyc_Tcutil_pointer_elt_type(vd->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp581=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->tag=2U,_tmp587->f1=i,_tmp587->f2=elttype;_tmp587;});struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r=_tmp581;
struct Cyc_CfFlowInfo_Place*_tmp582=({struct Cyc_CfFlowInfo_Place*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->root=(void*)r,_tmp586->path=0;_tmp586;});struct Cyc_CfFlowInfo_Place*rp=_tmp582;
d=({struct Cyc_Dict_Dict _tmp7A2=d;void*_tmp7A1=(void*)r;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7A2,_tmp7A1,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});
d=({struct Cyc_Dict_Dict _tmp7A4=d;void*_tmp7A3=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->tag=0U,_tmp583->f1=vd;_tmp583;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7A4,_tmp7A3,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->tag=4U,_tmp584->f1=rp;_tmp584;}));});
param_roots=({struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->hd=rp,_tmp585->tl=param_roots;_tmp585;});
goto _LL8;}default: _LLD: _LLE:
 goto _LL8;}_LL8:;}
# 3001
inflow=Cyc_CfFlowInfo_ReachableFL(d,relns);{
# 3003
int noreturn=Cyc_Atts_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp7A6=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp58F->succ_table=_tmp7A6;}),({
struct Cyc_Hashtable_Table*_tmp7A5=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp58F->pat_pop_table=_tmp7A5;}),_tmp58F->fenv=fenv,_tmp58F->iterate_again=1,_tmp58F->iteration_num=0,_tmp58F->in_try=0,_tmp58F->tryflow=inflow,_tmp58F->noreturn=noreturn,_tmp58F->return_type=(fd->i).ret_type,_tmp58F->unique_pat_vars=0,_tmp58F->param_roots=param_roots,_tmp58F->noconsume_params=noconsume_roots,_tmp58F->flow_table=flow_table,_tmp58F->return_relns=(fd->i).ensures_relns;_tmp58F;});
# 3013
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3019
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3022
union Cyc_CfFlowInfo_FlowInfo _tmp588=outflow;if((_tmp588.BottomFL).tag == 1){_LL10: _LL11:
 goto _LLF;}else{_LL12: _LL13:
# 3025
 Cyc_NewControlFlow_check_init_params(loc,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,loc);
# 3029
if(noreturn)
({void*_tmp589=0U;({unsigned _tmp7A8=loc;struct _fat_ptr _tmp7A7=({const char*_tmp58A="`noreturn' function might (implicitly) return";_tag_fat(_tmp58A,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp7A8,_tmp7A7,_tag_fat(_tmp589,sizeof(void*),0U));});});else{
if(!Cyc_Tcutil_is_void_type((fd->i).ret_type)){
if(Cyc_Tcutil_is_any_float_type((fd->i).ret_type)||
 Cyc_Tcutil_is_any_int_type((fd->i).ret_type))
({void*_tmp58B=0U;({unsigned _tmp7AA=loc;struct _fat_ptr _tmp7A9=({const char*_tmp58C="function may not return a value";_tag_fat(_tmp58C,sizeof(char),32U);});Cyc_Warn_warn(_tmp7AA,_tmp7A9,_tag_fat(_tmp58B,sizeof(void*),0U));});});else{
# 3036
({void*_tmp58D=0U;({unsigned _tmp7AC=loc;struct _fat_ptr _tmp7AB=({const char*_tmp58E="function may not return a value";_tag_fat(_tmp58E,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp7AC,_tmp7AB,_tag_fat(_tmp58D,sizeof(void*),0U));});});}goto _LLF;}}
# 3038
goto _LLF;}_LLF:;}}}}}}}
# 3042
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp591=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_stmttmpA1=_tmp591;void*_tmp592=_stmttmpA1;struct Cyc_List_List*_tmp593;struct Cyc_List_List*_tmp594;struct Cyc_List_List*_tmp595;struct Cyc_Absyn_Fndecl*_tmp596;switch(*((int*)_tmp592)){case 1U: _LL1: _tmp596=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp592)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp596;
Cyc_NewControlFlow_check_fun(tables,fd);goto _LL0;}case 11U: _LL3: _tmp595=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp592)->f1;_LL4: {struct Cyc_List_List*ds2=_tmp595;
# 3048
_tmp594=ds2;goto _LL6;}case 10U: _LL5: _tmp594=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp592)->f2;_LL6: {struct Cyc_List_List*ds2=_tmp594;
_tmp593=ds2;goto _LL8;}case 9U: _LL7: _tmp593=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp592)->f2;_LL8: {struct Cyc_List_List*ds2=_tmp593;
Cyc_NewControlFlow_cf_check(tables,ds2);goto _LL0;}case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;}}}
