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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 123
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 292 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 864 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 895
extern void*Cyc_Absyn_void_type;
# 973
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 985
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
# 1107
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1109
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 1121
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1123
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1125
int Cyc_Absyn_is_require_union_type(void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);extern union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*,union Cyc_Relations_RelnOp*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*,union Cyc_Relations_RelnOp,enum Cyc_Relations_Relation,union Cyc_Relations_RelnOp,struct Cyc_List_List*);
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
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp);
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation);
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
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
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 137
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 173
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 221
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict,int(*)(void*,void*),void*);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict,void*,void**);
# 149
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned,struct _fat_ptr,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
extern union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void);struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 142 "cf_flowinfo.h"
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 157
extern struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void);
# 159
extern int Cyc_CfFlowInfo_get_field_index(void*,struct _fat_ptr*);
extern int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*,struct _fat_ptr*);
# 162
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 165
extern struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int,void*);
extern void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*,int,void*);
extern void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*,void*,struct Cyc_Absyn_Exp*,int,void*,int);
extern int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*,int,void*,int*);
extern void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*,void*);struct _tuple16{void*f1;struct Cyc_List_List*f2;};
extern struct _tuple16 Cyc_CfFlowInfo_unname_rval(void*);
# 172
extern enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 175
extern int Cyc_CfFlowInfo_is_init_pointer(void*);
extern int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 178
extern void Cyc_CfFlowInfo_print_absrval(void*);
# 185
extern void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo);
# 201 "cf_flowinfo.h"
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
extern struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*,unsigned,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*,void*);
# 206
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
extern union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple17{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
extern struct _tuple17 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple17,struct _tuple17);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 112 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 54 "flags.h"
extern int Cyc_Flags_warn_lose_unique;
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 53 "new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_fat_size(s1,sizeof(char));
unsigned sz2=_get_fat_size(s2,sizeof(char));
unsigned minsz=sz1 < sz2?sz1: sz2;
# 61
while((unsigned)i < minsz){
char c1=*((const char*)_check_fat_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_fat_subscript(s2,sizeof(char),i));
if((int)c1 == (int)'\000'){
if((int)c2 == 0)return 0;else{
return - 1;}}else{
if((int)c2 == 0)return 1;else{
# 69
int diff=(int)c1 - (int)c2;
if(diff != 0)return diff;}}
# 72
++ i;}
# 74
if(sz1 == sz2)return 0;
if(minsz < sz2){
if((int)*((const char*)_check_fat_subscript(s2,sizeof(char),i))== (int)'\000')return 0;else{
return - 1;}}else{
# 79
if((int)*((const char*)_check_fat_subscript(s1,sizeof(char),i))== (int)'\000')return 0;else{
return 1;}}}}struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple18{void*f1;int f2;};
# 125 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple18*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int,int,struct Cyc_Absyn_Exp*);
static struct _tuple17 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*,int);struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
# 131
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_List_List*);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;void*_tmp2;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_tmp2=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;{struct Cyc_NewControlFlow_CFStmtAnnot*x=(struct Cyc_NewControlFlow_CFStmtAnnot*)_tmp2;
return x;}}else{
({void*_tmp5AB=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=Cyc_NewControlFlow_CFAnnot,(_tmp3->f1).visited=0;_tmp3;});s->annot=_tmp5AB;});return Cyc_NewControlFlow_get_stmt_annot(s);};}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp5AC=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp5AC;});_tmp4;});
((void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,union Cyc_CfFlowInfo_FlowInfo*))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}
# 152
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp5=f;void*_tmp6;if((_tmp5.BottomFL).tag == 1)
return 0;else{_tmp6=((_tmp5.ReachableFL).val).f2;{struct Cyc_List_List*r=_tmp6;
return r;}};}struct _tuple21{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 159
static struct _tuple21 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp7;
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp8;
# 163
({union Cyc_CfFlowInfo_FlowInfo _tmp5AD=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*sflow);*sflow=_tmp5AD;});
# 169
annot->visited=env->iteration_num;
return({struct _tuple21 _tmp50A;_tmp50A.f1=annot,_tmp50A.f2=sflow;_tmp50A;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp5AE=Cyc_CfFlowInfo_join_tryflow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp5AE;});}struct _tuple22{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned f2;struct Cyc_Dict_Dict f3;};
# 194
static void Cyc_NewControlFlow_check_unique_root(struct _tuple22*ckenv,void*root,void*rval){
# 196
struct _tuple22*_tmp9=ckenv;struct Cyc_Dict_Dict _tmpC;unsigned _tmpB;void*_tmpA;_tmpA=_tmp9->f1;_tmpB=_tmp9->f2;_tmpC=_tmp9->f3;{struct Cyc_NewControlFlow_AnalEnv*env=_tmpA;unsigned loc=_tmpB;struct Cyc_Dict_Dict new_fd=_tmpC;
void*_tmpD=root;void*_tmpE;if(*((int*)_tmpD)== 0){_tmpE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpE;
# 199
if(!((int(*)(struct Cyc_Dict_Dict,void*,void**))Cyc_Dict_lookup_bool)(new_fd,root,& rval)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
retry: {void*_tmpF=rval;void*_tmp10;switch(*((int*)_tmpF)){case 8: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF)->f2;{void*r=_tmp10;
rval=r;goto retry;}case 7:
 goto _LLE;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpF)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE:
 goto _LL10;}else{goto _LL11;}case 0: _LL10:
 goto _LL8;default: _LL11:
# 208
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0,({struct _fat_ptr _tmp5AF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp50B.f1=_tmp5AF;});_tmp50B;});void*_tmp11[1];_tmp11[0]=& _tmp13;({unsigned _tmp5B1=loc;struct _fat_ptr _tmp5B0=({const char*_tmp12="alias-free pointer(s) reachable from %s may become unreachable.";_tag_fat(_tmp12,sizeof(char),64U);});Cyc_Warn_warn(_tmp5B1,_tmp5B0,_tag_fat(_tmp11,sizeof(void*),1));});});
goto _LL8;}_LL8:;}}
# 212
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 220
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp14=Cyc_NewControlFlow_get_stmt_annot(s);struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp14;
union Cyc_CfFlowInfo_FlowInfo*_tmp15=Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp15;
union Cyc_CfFlowInfo_FlowInfo _tmp16=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*sflow);union Cyc_CfFlowInfo_FlowInfo new_flow=_tmp16;
# 226
if(Cyc_Flags_warn_lose_unique){
struct _tuple20 _tmp17=({struct _tuple20 _tmp50D;_tmp50D.f1=flow,_tmp50D.f2=new_flow;_tmp50D;});struct _tuple20 _stmttmp1=_tmp17;struct _tuple20 _tmp18=_stmttmp1;struct Cyc_Dict_Dict _tmp1A;struct Cyc_Dict_Dict _tmp19;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_tmp19=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict fd=_tmp19;struct Cyc_Dict_Dict new_fd=_tmp1A;
# 229
struct _tuple22 _tmp1B=({struct _tuple22 _tmp50C;_tmp50C.f1=env,_tmp50C.f2=s->loc,_tmp50C.f3=new_fd;_tmp50C;});struct _tuple22 ckenv=_tmp1B;
((void(*)(void(*)(struct _tuple22*,void*,void*),struct _tuple22*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& ckenv,fd);
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 234
if(!Cyc_CfFlowInfo_flow_lessthan_approx(new_flow,*sflow)){
# 241
*sflow=new_flow;
# 245
if(annot->visited == env->iteration_num)
# 247
env->iterate_again=1;}}
# 252
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned loc,int nameit){
# 256
union Cyc_CfFlowInfo_FlowInfo _tmp1C=inflow;void*_tmp1E;struct Cyc_Dict_Dict _tmp1D;if((_tmp1C.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_tmp1D=((_tmp1C.ReachableFL).val).f1;_tmp1E=((_tmp1C.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp1D;struct Cyc_List_List*relns=_tmp1E;
# 259
for(1;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp1F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=0,_tmp22->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp22;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp1F;
void*_tmp20=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);void*rval=_tmp20;
if(nameit)
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=8,_tmp21->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp21->f2=rval;_tmp21;});
# 266
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d,(void*)root,rval);}
# 268
return Cyc_CfFlowInfo_ReachableFL(d,relns);}};}
# 272
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 279
for(1;(unsigned)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp23=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);struct Cyc_Relations_Reln*neg_rln=_tmp23;
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->hd=neg_rln,_tmp24->tl=assume;_tmp24;})))
return 0;}
# 284
return 1;}
# 287
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp25=e->r;void*_stmttmp2=_tmp25;void*_tmp26=_stmttmp2;void*_tmp27;if(*((int*)_tmp26)== 14){_tmp27=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->f2;{struct Cyc_Absyn_Exp*e1=_tmp27;
return e1;}}else{
return e;};}
# 294
static void Cyc_NewControlFlow_check_union_requires(unsigned loc,void*t,struct _fat_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 296
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;void*_tmp29;if((_tmp28.BottomFL).tag == 1)
return;else{_tmp29=((_tmp28.ReachableFL).val).f2;{struct Cyc_List_List*relns=_tmp29;
# 299
{void*_tmp2A=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp2A;void*_tmp2B=_stmttmp3;void*_tmp2C;union Cyc_Absyn_AggrInfo _tmp2D;switch(*((int*)_tmp2B)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)== 20){_tmp2D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_tmp2C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f2;{union Cyc_Absyn_AggrInfo ainfo=_tmp2D;struct Cyc_List_List*ts=_tmp2C;
# 301
struct Cyc_Absyn_Aggrdecl*_tmp2E=Cyc_Absyn_get_known_aggrdecl(ainfo);struct Cyc_Absyn_Aggrdecl*ad=_tmp2E;
struct Cyc_Absyn_Aggrfield*_tmp2F=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*field=_tmp2F;
struct Cyc_Absyn_Exp*_tmp30=((struct Cyc_Absyn_Aggrfield*)_check_null(field))->requires_clause;struct Cyc_Absyn_Exp*req=_tmp30;
if(req != 0){
struct _RegionHandle _tmp31=_new_region("temp");struct _RegionHandle*temp=& _tmp31;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp32=({struct _RegionHandle*_tmp5B3=temp;struct Cyc_List_List*_tmp5B2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(temp,temp,ad->tvs,ts);Cyc_Tcutil_rsubsexp(_tmp5B3,_tmp5B2,req);});struct Cyc_Absyn_Exp*ireq=_tmp32;
# 308
if(!({struct Cyc_List_List*_tmp5B4=relns;Cyc_NewControlFlow_relns_ok(_tmp5B4,Cyc_Relations_exp2relns(temp,ireq));})){
({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0,({struct _fat_ptr _tmp5B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(ireq)));_tmp50F.f1=_tmp5B5;});_tmp50F;});struct Cyc_String_pa_PrintArg_struct _tmp36=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0,_tmp50E.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp50E;});void*_tmp33[2];_tmp33[0]=& _tmp35,_tmp33[1]=& _tmp36;({unsigned _tmp5B7=loc;struct _fat_ptr _tmp5B6=({const char*_tmp34="unable to prove %s, required to access %s";_tag_fat(_tmp34,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp5B7,_tmp5B6,_tag_fat(_tmp33,sizeof(void*),2));});});
({void*_tmp37=0U;({struct Cyc___cycFILE*_tmp5B9=Cyc_stderr;struct _fat_ptr _tmp5B8=({const char*_tmp38="  [recorded facts on path: ";_tag_fat(_tmp38,sizeof(char),28U);});Cyc_fprintf(_tmp5B9,_tmp5B8,_tag_fat(_tmp37,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,relns);
({void*_tmp39=0U;({struct Cyc___cycFILE*_tmp5BB=Cyc_stderr;struct _fat_ptr _tmp5BA=({const char*_tmp3A="]\n";_tag_fat(_tmp3A,sizeof(char),3U);});Cyc_fprintf(_tmp5BB,_tmp5BA,_tag_fat(_tmp39,sizeof(void*),0));});});}}
# 306
;_pop_region();}
# 315
goto _LL5;}}else{goto _LLA;}case 7: _tmp2C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B)->f2;{struct Cyc_List_List*fs=_tmp2C;
# 317
struct Cyc_Absyn_Aggrfield*_tmp3B=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp3B;
struct Cyc_Absyn_Exp*_tmp3C=((struct Cyc_Absyn_Aggrfield*)_check_null(field))->requires_clause;struct Cyc_Absyn_Exp*req=_tmp3C;
if(req != 0){
struct _RegionHandle _tmp3D=_new_region("temp");struct _RegionHandle*temp=& _tmp3D;_push_region(temp);
if(!({struct Cyc_List_List*_tmp5BC=relns;Cyc_NewControlFlow_relns_ok(_tmp5BC,Cyc_Relations_exp2relns(temp,req));})){
({struct Cyc_String_pa_PrintArg_struct _tmp40=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0,({struct _fat_ptr _tmp5BD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(req)));_tmp511.f1=_tmp5BD;});_tmp511;});struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0,_tmp510.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp510;});void*_tmp3E[2];_tmp3E[0]=& _tmp40,_tmp3E[1]=& _tmp41;({unsigned _tmp5BF=loc;struct _fat_ptr _tmp5BE=({const char*_tmp3F="unable to prove %s, required to access %s";_tag_fat(_tmp3F,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp5BF,_tmp5BE,_tag_fat(_tmp3E,sizeof(void*),2));});});
({void*_tmp42=0U;({struct Cyc___cycFILE*_tmp5C1=Cyc_stderr;struct _fat_ptr _tmp5C0=({const char*_tmp43="  [recorded facts on path: ";_tag_fat(_tmp43,sizeof(char),28U);});Cyc_fprintf(_tmp5C1,_tmp5C0,_tag_fat(_tmp42,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,relns);
({void*_tmp44=0U;({struct Cyc___cycFILE*_tmp5C3=Cyc_stderr;struct _fat_ptr _tmp5C2=({const char*_tmp45="]\n";_tag_fat(_tmp45,sizeof(char),3U);});Cyc_fprintf(_tmp5C3,_tmp5C2,_tag_fat(_tmp44,sizeof(void*),0));});});}
# 321
;_pop_region();}
# 328
goto _LL5;}default: _LLA:
 goto _LL5;}_LL5:;}
# 331
goto _LL0;}}_LL0:;}
# 335
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp46=inflow;void*_tmp48;struct Cyc_Dict_Dict _tmp47;if((_tmp46.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_tmp47=((_tmp46.ReachableFL).val).f1;_tmp48=((_tmp46.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp47;struct Cyc_List_List*relns=_tmp48;
# 339
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp49=0U;({unsigned _tmp5C5=loc;struct _fat_ptr _tmp5C4=({const char*_tmp4A="expression may not be fully initialized";_tag_fat(_tmp4A,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp5C5,_tmp5C4,_tag_fat(_tmp49,sizeof(void*),0));});});{
struct Cyc_Dict_Dict _tmp4B=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);struct Cyc_Dict_Dict ans_d=_tmp4B;
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);union Cyc_CfFlowInfo_FlowInfo ans=_tmp4C;
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}};}struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 349
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){
struct _tuple0 _tmp4D=({struct _tuple0 _tmp512;({void*_tmp5C6=Cyc_Absyn_compress(t);_tmp512.f1=_tmp5C6;}),_tmp512.f2=r;_tmp512;});struct _tuple0 _stmttmp4=_tmp4D;struct _tuple0 _tmp4E=_stmttmp4;enum Cyc_Absyn_AggrKind _tmp4F;union Cyc_Absyn_AggrInfo _tmp52;struct _fat_ptr _tmp51;void*_tmp50;switch(*((int*)_tmp4E.f2)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f1 == Cyc_CfFlowInfo_NoneIL)
return;else{switch(*((int*)_tmp4E.f1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f1)){case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f1)->f1).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 20: goto _LL13;default: goto _LL13;}case 6: goto _LL13;case 7: goto _LL13;case 3: goto _LL11;default: goto _LL13;}}case 0:
 return;case 7:
 return;case 8: _tmp50=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f2;{void*r=_tmp50;
Cyc_NewControlFlow_check_nounique(env,loc,t,r);return;}default: switch(*((int*)_tmp4E.f1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f1)){case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(*((int*)_tmp4E.f2)== 6){_tmp50=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp51=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp50;struct _fat_ptr ad=_tmp51;
# 356
if(tuf->typs == 0)
return;
_tmp50=tuf->typs;_tmp51=ad;goto _LLC;}}else{goto _LL13;}}else{goto _LL13;}case 20: if(*((int*)_tmp4E.f2)== 6){_tmp52=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f1)->f1;_tmp50=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E.f1)->f2;_tmp51=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f2;{union Cyc_Absyn_AggrInfo info=_tmp52;struct Cyc_List_List*targs=_tmp50;struct _fat_ptr d=_tmp51;
# 367
struct Cyc_Absyn_Aggrdecl*_tmp53=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp53;
if(ad->impl == 0)return;{
struct Cyc_List_List*_tmp54=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp54;
struct _RegionHandle _tmp55=_new_region("temp");struct _RegionHandle*temp=& _tmp55;_push_region(temp);
{struct Cyc_List_List*_tmp56=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(temp,temp,ad->tvs,targs);struct Cyc_List_List*inst=_tmp56;
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd)->type;
if(inst != 0)t=Cyc_Tcutil_rsubstitute(temp,inst,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)d.curr)[i]);}}
# 377
_npop_handler(0);return;}
# 371
;_pop_region();}}}else{goto _LL13;}default: goto _LL13;}case 6: if(*((int*)_tmp4E.f2)== 6){_tmp50=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4E.f1)->f1;_tmp51=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f2;_LLC: {struct Cyc_List_List*tqts=_tmp50;struct _fat_ptr ad=_tmp51;
# 360
unsigned sz=(unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(tqts);
{int i=0;for(0;(unsigned)i < sz;(i ++,tqts=tqts->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp5C9=env;unsigned _tmp5C8=loc;void*_tmp5C7=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp5C9,_tmp5C8,_tmp5C7,*((void**)_check_fat_subscript(ad,sizeof(void*),i)));});}}
# 364
return;}}else{goto _LL13;}case 7: if(*((int*)_tmp4E.f2)== 6){_tmp4F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E.f1)->f1;_tmp50=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E.f1)->f2;_tmp51=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f2;{enum Cyc_Absyn_AggrKind k=_tmp4F;struct Cyc_List_List*fs=_tmp50;struct _fat_ptr d=_tmp51;
# 380
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd)->type,((void**)d.curr)[i]);}}
return;}}else{goto _LL13;}case 3: _LL11:
# 384
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp57=0U;({unsigned _tmp5CB=loc;struct _fat_ptr _tmp5CA=({const char*_tmp58="argument may still contain alias-free pointers";_tag_fat(_tmp58,sizeof(char),47U);});Cyc_Warn_warn(_tmp5CB,_tmp5CA,_tag_fat(_tmp57,sizeof(void*),0));});});
return;default: _LL13:
 return;}};}
# 391
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp59=inflow;void*_tmp5B;struct Cyc_Dict_Dict _tmp5A;if((_tmp59.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_tmp5A=((_tmp59.ReachableFL).val).f1;_tmp5B=((_tmp59.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp5A;struct Cyc_List_List*relns=_tmp5B;
# 395
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp5C=0U;({struct _fat_ptr _tmp5CC=({const char*_tmp5D="noliveunique attribute requires alias-free pointer";_tag_fat(_tmp5D,sizeof(char),51U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5CC,_tag_fat(_tmp5C,sizeof(void*),0));});});{
void*_tmp5E=Cyc_Tcutil_pointer_elt_type(t);void*elt_type=_tmp5E;
retry: {void*_tmp5F=r;void*_tmp60;switch(*((int*)_tmp5F)){case 8: _tmp60=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;{void*r2=_tmp60;
r=r2;goto retry;}case 4: _tmp60=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp60;
_tmp60=Cyc_CfFlowInfo_lookup_place(d,p);goto _LLB;}case 5: _tmp60=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1;_LLB: {void*r=_tmp60;
Cyc_NewControlFlow_check_nounique(env,loc,elt_type,r);goto _LL5;}default:
# 403
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(elt_type))
({void*_tmp61=0U;({unsigned _tmp5CE=loc;struct _fat_ptr _tmp5CD=({const char*_tmp62="argument may contain live alias-free pointers";_tag_fat(_tmp62,sizeof(char),46U);});Cyc_Warn_warn(_tmp5CE,_tmp5CD,_tag_fat(_tmp61,sizeof(void*),0));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 407
struct Cyc_Dict_Dict _tmp63=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);struct Cyc_Dict_Dict ans_d=_tmp63;
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp64=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);union Cyc_CfFlowInfo_FlowInfo ans=_tmp64;
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}}};}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 417
static struct _tuple24 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 423
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple24 _tmp513;_tmp513.f1=inflow,_tmp513.f2=0;_tmp513;});
do{
struct _tuple17 _tmp65=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd,0);struct _tuple17 _stmttmp5=_tmp65;struct _tuple17 _tmp66=_stmttmp5;void*_tmp68;union Cyc_CfFlowInfo_FlowInfo _tmp67;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp67;void*r=_tmp68;
inflow=f;
rvals=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=r,_tmp69->tl=rvals;_tmp69;});
es=es->tl;
first_is_copy=others_are_copy;}}while(es != 0);
# 434
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple24 _tmp514;_tmp514.f1=inflow,({struct Cyc_List_List*_tmp5CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(rvals);_tmp514.f2=_tmp5CF;});_tmp514;});}
# 440
static struct _tuple17 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 442
struct _tuple24 _tmp6A=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 442
struct _tuple24 _stmttmp6=_tmp6A;struct _tuple24 _tmp6B=_stmttmp6;void*_tmp6D;union Cyc_CfFlowInfo_FlowInfo _tmp6C;_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{union Cyc_CfFlowInfo_FlowInfo afterflow=_tmp6C;struct Cyc_List_List*rvals=_tmp6D;
# 444
{union Cyc_CfFlowInfo_FlowInfo _tmp6E=afterflow;struct Cyc_Dict_Dict _tmp6F;if((_tmp6E.ReachableFL).tag == 2){_tmp6F=((_tmp6E.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp6F;
# 446
for(1;rvals != 0;(rvals=rvals->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,(void*)rvals->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp70=0U;({unsigned _tmp5D1=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp5D0=({const char*_tmp71="expression may not be initialized";_tag_fat(_tmp71,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp5D1,_tmp5D0,_tag_fat(_tmp70,sizeof(void*),0));});});}
goto _LL3;}}else{
goto _LL3;}_LL3:;}
# 452
return({struct _tuple17 _tmp515;_tmp515.f1=afterflow,_tmp515.f2=(env->fenv)->unknown_all;_tmp515;});}}
# 459
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 466
int needs_unconsume=0;
void*_tmp72=Cyc_CfFlowInfo_lookup_place(new_dict,p);void*old_rval=_tmp72;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume))
({void*_tmp73=0U;({struct _fat_ptr _tmp5D2=({const char*_tmp74="consume_zero_rval";_tag_fat(_tmp74,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5D2,_tag_fat(_tmp73,sizeof(void*),0));});});else{
# 471
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval,0);else{
# 475
return new_rval;}}}
# 488 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 493
union Cyc_CfFlowInfo_FlowInfo _tmp75=outflow;void*_tmp77;struct Cyc_Dict_Dict _tmp76;if((_tmp75.BottomFL).tag == 1)
return outflow;else{_tmp76=((_tmp75.ReachableFL).val).f1;_tmp77=((_tmp75.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp76;struct Cyc_List_List*relns=_tmp77;
# 496
union Cyc_CfFlowInfo_AbsLVal _tmp78=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _stmttmp7=_tmp78;union Cyc_CfFlowInfo_AbsLVal _tmp79=_stmttmp7;void*_tmp7A;if((_tmp79.UnknownL).tag == 2)
# 500
return outflow;else{_tmp7A=(_tmp79.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp7A;
# 504
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(1;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=8,_tmp7B->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp7B->f2=nzval;_tmp7B;});}
# 508
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmp7C=({struct Cyc_Dict_Dict _tmp5D3=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp5D3,relns);});union Cyc_CfFlowInfo_FlowInfo outflow=_tmp7C;
# 513
return outflow;}}};}};}
# 522
static struct _tuple20 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 524
union Cyc_CfFlowInfo_FlowInfo _tmp7D=outflow;void*_tmp7F;struct Cyc_Dict_Dict _tmp7E;if((_tmp7D.BottomFL).tag == 1)
return({struct _tuple20 _tmp516;_tmp516.f1=outflow,_tmp516.f2=outflow;_tmp516;});else{_tmp7E=((_tmp7D.ReachableFL).val).f1;_tmp7F=((_tmp7D.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp7E;struct Cyc_List_List*relns=_tmp7F;
# 527
union Cyc_CfFlowInfo_AbsLVal _tmp80=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _stmttmp8=_tmp80;union Cyc_CfFlowInfo_AbsLVal _tmp81=_stmttmp8;void*_tmp82;if((_tmp81.UnknownL).tag == 2)
return({struct _tuple20 _tmp517;_tmp517.f1=outflow,_tmp517.f2=outflow;_tmp517;});else{_tmp82=(_tmp81.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp82;
# 530
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(1;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=8,_tmp83->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp83->f2=nzval;_tmp83;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=8,_tmp84->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp84->f2=zval;_tmp84;});}
# 536
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,zval);
return({struct _tuple20 _tmp518;({
union Cyc_CfFlowInfo_FlowInfo _tmp5D7=({struct Cyc_Dict_Dict _tmp5D6=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp5D6,relns);});_tmp518.f1=_tmp5D7;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp5D5=({struct Cyc_Dict_Dict _tmp5D4=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,zval);Cyc_CfFlowInfo_ReachableFL(_tmp5D4,relns);});_tmp518.f2=_tmp5D5;});_tmp518;});}};}};}
# 545
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 548
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
# 551
{void*_tmp85=e->annot;void*_stmttmp9=_tmp85;void*_tmp86=_stmttmp9;void*_tmp87;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp86)->tag == Cyc_CfFlowInfo_UnknownZ){_tmp87=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp86)->f1;{struct Cyc_List_List*relns2=_tmp87;
# 553
if(possibly_null && Cyc_Relations_same_relns(relns,relns2))return;
goto _LL0;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp86)->tag == Cyc_CfFlowInfo_NotZero){_tmp87=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp86)->f1;{struct Cyc_List_List*relns2=_tmp87;
# 556
if(!possibly_null && Cyc_Relations_same_relns(relns,relns2))return;
goto _LL0;}}else{
goto _LL0;}}_LL0:;}
# 560
if(possibly_null)
({void*_tmp5D8=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=Cyc_CfFlowInfo_UnknownZ,_tmp88->f1=relns;_tmp88;});e->annot=_tmp5D8;});else{
# 563
({void*_tmp5D9=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=Cyc_CfFlowInfo_NotZero,_tmp89->f1=relns;_tmp89;});e->annot=_tmp5D9;});}}
# 571
static struct _tuple17 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){
# 575
void*_tmp8A=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpA=_tmp8A;void*_tmp8B=_stmttmpA;void*_tmp8D;void*_tmp8C;if(*((int*)_tmp8B)== 3){_tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmp8D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;{void*elttype=_tmp8C;void*bd=_tmp8D;
# 577
union Cyc_CfFlowInfo_FlowInfo _tmp8E=f;void*_tmp90;struct Cyc_Dict_Dict _tmp8F;if((_tmp8E.BottomFL).tag == 1)
# 579
return({struct _tuple17 _tmp519;_tmp519.f1=f,({void*_tmp5DA=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp519.f2=_tmp5DA;});_tmp519;});else{_tmp8F=((_tmp8E.ReachableFL).val).f1;_tmp90=((_tmp8E.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp8F;struct Cyc_List_List*relns=_tmp90;
# 581
struct _tuple16 _tmp91=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmpB=_tmp91;struct _tuple16 _tmp92=_stmttmpB;void*_tmp94;void*_tmp93;_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;{void*r=_tmp93;struct Cyc_List_List*names=_tmp94;
{void*_tmp95=r;enum Cyc_CfFlowInfo_InitLevel _tmp96;void*_tmp98;void*_tmp97;switch(*((int*)_tmp95)){case 8: _tmp97=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;_tmp98=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp95)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp97;void*r2=_tmp98;
# 584
({void*_tmp99=0U;({struct _fat_ptr _tmp5DB=({const char*_tmp9A="named location in anal_derefR";_tag_fat(_tmp9A,sizeof(char),30U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5DB,_tag_fat(_tmp99,sizeof(void*),0));});});}case 1:
# 586
 Cyc_NewControlFlow_update_relns(e,0,relns);
goto _LLD;case 4: _tmp97=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp97;
# 589
Cyc_NewControlFlow_update_relns(e,0,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _tmp51A;_tmp51A.f1=f,({void*_tmp5DC=Cyc_CfFlowInfo_lookup_place(outdict,p);_tmp51A.f2=_tmp5DC;});_tmp51A;});
goto _LLD;}case 5: _tmp97=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;{void*r=_tmp97;
# 595
Cyc_NewControlFlow_update_relns(e,1,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _tmp51B;_tmp51B.f1=f,_tmp51B.f2=r;_tmp51B;});
goto _LLD;}case 0:
# 600
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple17 _tmp51C;({union Cyc_CfFlowInfo_FlowInfo _tmp5DE=Cyc_CfFlowInfo_BottomFL();_tmp51C.f1=_tmp5DE;}),({void*_tmp5DD=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp51C.f2=_tmp5DD;});_tmp51C;});case 2: _tmp96=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp96;
# 604
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
goto _LL1B;}default: _LL1B:
# 607
 Cyc_NewControlFlow_update_relns(e,1,relns);
goto _LLD;}_LLD:;}{
# 610
enum Cyc_CfFlowInfo_InitLevel _tmp9B=Cyc_CfFlowInfo_initlevel(env->fenv,outdict,r);enum Cyc_CfFlowInfo_InitLevel _stmttmpC=_tmp9B;enum Cyc_CfFlowInfo_InitLevel _tmp9C=_stmttmpC;if(_tmp9C == Cyc_CfFlowInfo_NoneIL){
# 612
struct _tuple16 _tmp9D=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmpD=_tmp9D;struct _tuple16 _tmp9E=_stmttmpD;void*_tmp9F;_tmp9F=_tmp9E.f1;{void*r=_tmp9F;
{void*_tmpA0=r;if(*((int*)_tmpA0)== 7){
# 615
({void*_tmpA1=0U;({unsigned _tmp5E0=e->loc;struct _fat_ptr _tmp5DF=({const char*_tmpA2="attempt to dereference a consumed alias-free pointer";_tag_fat(_tmpA2,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp5E0,_tmp5DF,_tag_fat(_tmpA1,sizeof(void*),0));});});
goto _LL24;}else{
# 618
({void*_tmpA3=0U;({unsigned _tmp5E2=e->loc;struct _fat_ptr _tmp5E1=({const char*_tmpA4="dereference of possibly uninitialized pointer";_tag_fat(_tmpA4,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp5E2,_tmp5E1,_tag_fat(_tmpA3,sizeof(void*),0));});});}_LL24:;}
# 620
goto _LL20;}}else{_LL20:
# 622
 return({struct _tuple17 _tmp51D;_tmp51D.f1=f,({void*_tmp5E3=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp51D.f2=_tmp5E3;});_tmp51D;});};}}}};}}else{
# 625
({void*_tmpA5=0U;({struct _fat_ptr _tmp5E4=({const char*_tmpA6="right deref of non-pointer-type";_tag_fat(_tmpA6,sizeof(char),32U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5E4,_tag_fat(_tmpA5,sizeof(void*),0));});});};}
# 632
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 634
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 637
{void*_tmpA7=e1->r;void*_stmttmpE=_tmpA7;void*_tmpA8=_stmttmpE;void*_tmpA9;if(*((int*)_tmpA8)== 1){_tmpA9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA8)->f1;{void*b1=_tmpA9;
# 639
struct Cyc_Absyn_Vardecl*_tmpAA=Cyc_Tcutil_nonesc_vardecl(b1);struct Cyc_Absyn_Vardecl*x=_tmpAA;
if(x != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(x);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);}
# 645
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 649
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);}
# 655
return relns;}}
# 663
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned loc,void*old_rval){
# 668
struct _tuple19 _tmpAB=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple19 _stmttmpF=_tmpAB;struct _tuple19 _tmpAC=_stmttmpF;union Cyc_CfFlowInfo_AbsLVal _tmpAD;_tmpAD=_tmpAC.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_tmpAD;
{struct _tuple19 _tmpAE=({struct _tuple19 _tmp51F;_tmp51F.f1=inflow,_tmp51F.f2=lval;_tmp51F;});struct _tuple19 _stmttmp10=_tmpAE;struct _tuple19 _tmpAF=_stmttmp10;void*_tmpB2;void*_tmpB1;struct Cyc_Dict_Dict _tmpB0;if(((_tmpAF.f1).ReachableFL).tag == 2){if(((_tmpAF.f2).PlaceL).tag == 1){_tmpB0=(((_tmpAF.f1).ReachableFL).val).f1;_tmpB1=(((_tmpAF.f1).ReachableFL).val).f2;_tmpB2=((_tmpAF.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_tmpB0;struct Cyc_List_List*relns=_tmpB1;struct Cyc_CfFlowInfo_Place*p=_tmpB2;
# 671
void*_tmpB3=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);void*new_rval=_tmpB3;
# 673
struct _tuple16 _tmpB4=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _stmttmp11=_tmpB4;struct _tuple16 _tmpB5=_stmttmp11;void*_tmpB7;void*_tmpB6;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;{void*old_rval=_tmpB6;struct Cyc_List_List*names=_tmpB7;
for(1;names != 0;names=names->tl){
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->tag=8,_tmpB8->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmpB8->f2=new_rval;_tmpB8;});}
# 678
fd=Cyc_CfFlowInfo_assign_place(env->fenv,loc,fd,p,new_rval);
inflow=Cyc_CfFlowInfo_ReachableFL(fd,relns);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 684
goto _LL3;}}}else{
# 687
({struct Cyc_String_pa_PrintArg_struct _tmpBB=({struct Cyc_String_pa_PrintArg_struct _tmp51E;_tmp51E.tag=0,({struct _fat_ptr _tmp5E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(exp));_tmp51E.f1=_tmp5E5;});_tmp51E;});void*_tmpB9[1];_tmpB9[0]=& _tmpBB;({struct _fat_ptr _tmp5E6=({const char*_tmpBA="noconsume parameter %s must be l-values";_tag_fat(_tmpBA,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5E6,_tag_fat(_tmpB9,sizeof(void*),1));});});
goto _LL3;}}else{
# 685
goto _LL3;}_LL3:;}
# 690
return inflow;}}
# 695
static struct _tuple17 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 702
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;void*_tmpBE;struct Cyc_Dict_Dict _tmpBD;if((_tmpBC.BottomFL).tag == 1)
# 704
return({struct _tuple17 _tmp520;({union Cyc_CfFlowInfo_FlowInfo _tmp5E7=Cyc_CfFlowInfo_BottomFL();_tmp520.f1=_tmp5E7;}),_tmp520.f2=rval;_tmp520;});else{_tmpBD=((_tmpBC.ReachableFL).val).f1;_tmpBE=((_tmpBC.ReachableFL).val).f2;{struct Cyc_Dict_Dict indict=_tmpBD;struct Cyc_List_List*relns=_tmpBE;
# 706
union Cyc_CfFlowInfo_AbsLVal _tmpBF=lval;void*_tmpC0;if((_tmpBF.PlaceL).tag == 1){_tmpC0=(_tmpBF.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmpC0;
# 714
struct Cyc_Dict_Dict _tmpC1=Cyc_CfFlowInfo_assign_place(fenv,loc,indict,p,rval);struct Cyc_Dict_Dict outdict=_tmpC1;
relns=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,relns,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
if(Cyc_Flags_warn_lose_unique &&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 720
struct _tuple16 _tmpC2=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(indict,p));struct _tuple16 _stmttmp12=_tmpC2;struct _tuple16 _tmpC3=_stmttmp12;void*_tmpC4;_tmpC4=_tmpC3.f1;{void*rv=_tmpC4;
void*_tmpC5=rv;switch(*((int*)_tmpC5)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC5)->f1 == Cyc_CfFlowInfo_NoneIL)
goto _LL11;else{goto _LL14;}case 7: _LL11:
 goto _LL13;case 0: _LL13:
 goto _LLD;default: _LL14:
# 726
({void*_tmpC6=0U;({unsigned _tmp5E9=lexp->loc;struct _fat_ptr _tmp5E8=({const char*_tmpC7="assignment may overwrite alias-free pointer(s)";_tag_fat(_tmpC7,sizeof(char),47U);});Cyc_Warn_warn(_tmp5E9,_tmp5E8,_tag_fat(_tmpC6,sizeof(void*),0));});});
goto _LLD;}_LLD:;}}
# 731
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple17 _tmp521;_tmp521.f1=outflow,_tmp521.f2=rval;_tmp521;});}}else{
# 737
return({struct _tuple17 _tmp522;({union Cyc_CfFlowInfo_FlowInfo _tmp5EA=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp522.f1=_tmp5EA;}),_tmp522.f2=rval;_tmp522;});};}};}
# 744
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 750
union Cyc_CfFlowInfo_FlowInfo _tmpC8=f;void*_tmpCA;struct Cyc_Dict_Dict _tmpC9;if((_tmpC8.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_tmpC9=((_tmpC8.ReachableFL).val).f1;_tmpCA=((_tmpC8.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmpC9;struct Cyc_List_List*relns=_tmpCA;
# 755
outdict=({struct Cyc_CfFlowInfo_FlowEnv*_tmp5EF=fenv;unsigned _tmp5EE=loc;struct Cyc_Dict_Dict _tmp5ED=outdict;struct Cyc_CfFlowInfo_Place*_tmp5EC=({struct Cyc_CfFlowInfo_Place*_tmpCC=_cycalloc(sizeof(*_tmpCC));
({void*_tmp5EB=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=0,_tmpCB->f1=vd;_tmpCB;});_tmpCC->root=_tmp5EB;}),_tmpCC->path=0;_tmpCC;});
# 755
Cyc_CfFlowInfo_assign_place(_tmp5EF,_tmp5EE,_tmp5ED,_tmp5EC,rval);});
# 758
relns=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,relns,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmpCD=Cyc_CfFlowInfo_ReachableFL(outdict,relns);union Cyc_CfFlowInfo_FlowInfo outflow=_tmpCD;
Cyc_NewControlFlow_update_tryflow(env,outflow);
# 763
return outflow;}}};}struct _tuple25{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 767
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned pat_loc,unsigned exp_loc,int isAliasPat){
# 774
if(vds == 0)return inflow;{
# 777
struct Cyc_List_List*_tmpCE=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)(vds)).f1);struct Cyc_List_List*vs=_tmpCE;
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple25*)x->hd)).f1 == 0)es=({struct Cyc_List_List*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple25*)x->hd)).f2),_tmpCF->tl=es;_tmpCF;});}}
# 783
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,vs,fenv->unknown_all,pat_loc,name_locs);
# 785
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 789
struct _tuple17 _tmpD0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd,0);struct _tuple17 _stmttmp13=_tmpD0;struct _tuple17 _tmpD1=_stmttmp13;void*_tmpD3;union Cyc_CfFlowInfo_FlowInfo _tmpD2;_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmpD2;void*r=_tmpD3;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,f,r);}}}{
# 797
struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(vds);struct Cyc_List_List*vds=_tmpD4;
for(1;vds != 0;vds=vds->tl){
struct _tuple25*_tmpD5=(struct _tuple25*)vds->hd;struct _tuple25*_stmttmp14=_tmpD5;struct _tuple25*_tmpD6=_stmttmp14;void*_tmpD8;void*_tmpD7;_tmpD7=_tmpD6->f1;_tmpD8=_tmpD6->f2;{struct Cyc_Absyn_Vardecl**vd=_tmpD7;struct Cyc_Absyn_Exp*ve=_tmpD8;
if(vd != 0 && ve != 0){
if(ve->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0,({
struct _fat_ptr _tmp5F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(ve));_tmp523.f1=_tmp5F0;});_tmp523;});void*_tmpD9[1];_tmpD9[0]=& _tmpDB;({struct _fat_ptr _tmp5F1=({const char*_tmpDA="oops! pattern init expr %s has no type!\n";_tag_fat(_tmpDA,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5F1,_tag_fat(_tmpD9,sizeof(void*),1));});});{
# 811
struct Cyc_List_List l=({struct Cyc_List_List _tmp525;_tmp525.hd=ve,_tmp525.tl=0;_tmp525;});
union Cyc_CfFlowInfo_FlowInfo _tmpDC=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);union Cyc_CfFlowInfo_FlowInfo f=_tmpDC;
struct _tuple19 _tmpDD=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,ve);struct _tuple19 _stmttmp15=_tmpDD;struct _tuple19 _tmpDE=_stmttmp15;union Cyc_CfFlowInfo_AbsLVal _tmpDF;_tmpDF=_tmpDE.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_tmpDF;
struct _tuple17 _tmpE0=Cyc_NewControlFlow_anal_Rexp(env,1,f,ve,isAliasPat);struct _tuple17 _stmttmp16=_tmpE0;struct _tuple17 _tmpE1=_stmttmp16;void*_tmpE3;union Cyc_CfFlowInfo_FlowInfo _tmpE2;_tmpE2=_tmpE1.f1;_tmpE3=_tmpE1.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmpE2;void*rval=_tmpE3;
union Cyc_CfFlowInfo_FlowInfo _tmpE4=f;void*_tmpE6;struct Cyc_Dict_Dict _tmpE5;if((_tmpE4.ReachableFL).tag == 2){_tmpE5=((_tmpE4.ReachableFL).val).f1;_tmpE6=((_tmpE4.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_tmpE5;struct Cyc_List_List*relns=_tmpE6;
# 817
if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmpE7=lval;void*_tmpE8;if((_tmpE7.PlaceL).tag == 1){_tmpE8=(_tmpE7.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmpE8;
# 820
rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->tag=8,_tmpE9->f1=*vd,_tmpE9->f2=rval;_tmpE9;});{
# 823
void*_tmpEA=Cyc_CfFlowInfo_lookup_place(fd,p);void*new_rval=_tmpEA;
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=8,_tmpEB->f1=*vd,_tmpEB->f2=new_rval;_tmpEB;});
fd=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);
goto _LL11;}}}else{
# 830
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(ve->topt))&& !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*vd)->type))
({struct Cyc_String_pa_PrintArg_struct _tmpEE=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0,({
struct _fat_ptr _tmp5F2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(ve));_tmp524.f1=_tmp5F2;});_tmp524;});void*_tmpEC[1];_tmpEC[0]=& _tmpEE;({unsigned _tmp5F4=exp_loc;struct _fat_ptr _tmp5F3=({const char*_tmpED="aliased pattern expression not an l-value: %s";_tag_fat(_tmpED,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp5F4,_tmp5F3,_tag_fat(_tmpEC,sizeof(void*),1));});});}_LL11:;}
# 840
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,f,*vd,ve,rval,pat_loc);
goto _LLC;}}else{
# 843
goto _LLC;}_LLC:;}}}}}}
# 848
return inflow;}}}
# 851
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 853
void*_tmpEF=e->r;void*_stmttmp17=_tmpEF;void*_tmpF0=_stmttmp17;void*_tmpF1;switch(*((int*)_tmpF0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF0)->f1)){case 4:
 goto _LL4;case 3: _LL4:
 goto _LL6;case 5: _LL6:
 return 1;default: goto _LL11;}case 20: _tmpF1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;{struct Cyc_Absyn_Exp*e=_tmpF1;
_tmpF1=e;goto _LLA;}case 21: _tmpF1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmpF1;
_tmpF1=e;goto _LLC;}case 22: _tmpF1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmpF1;
# 860
return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);}case 23: _tmpF1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;{struct Cyc_Absyn_Exp*e=_tmpF1;
# 862
void*_tmpF2=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp18=_tmpF2;void*_tmpF3=_stmttmp18;if(*((int*)_tmpF3)== 6)
return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);else{
return 0;};}case 14: _tmpF1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;{struct Cyc_Absyn_Exp*e=_tmpF1;
# 867
if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(e,cast_ok);else{
return 0;}}default: _LL11:
 return 0;};}
# 873
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmpF4=*rop;union Cyc_Relations_RelnOp _stmttmp19=_tmpF4;union Cyc_Relations_RelnOp _tmpF5=_stmttmp19;unsigned _tmpF6;switch((_tmpF5.RParamNumelts).tag){case 5: _tmpF6=(_tmpF5.RParam).val;{unsigned i=_tmpF6;
# 876
struct Cyc_Absyn_Exp*_tmpF7=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(exps,(int)i);struct Cyc_Absyn_Exp*e=_tmpF7;
return Cyc_Relations_exp2relnop(e,rop);}case 6: _tmpF6=(_tmpF5.RParamNumelts).val;{unsigned i=_tmpF6;
# 879
struct Cyc_Absyn_Exp*_tmpF8=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(exps,(int)i);struct Cyc_Absyn_Exp*e=_tmpF8;
return({struct Cyc_Absyn_Exp*_tmp5F5=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,e,0U);Cyc_Relations_exp2relnop(_tmp5F5,rop);});}default:
 return 1;};}
# 885
static struct _fat_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmpF9=rop;unsigned _tmpFA;switch((_tmpF9.RParamNumelts).tag){case 5: _tmpFA=(_tmpF9.RParam).val;{unsigned i=_tmpFA;
# 888
return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(exps,(int)i));}case 6: _tmpFA=(_tmpF9.RParamNumelts).val;{unsigned i=_tmpFA;
# 890
return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(exps,(int)i));}default:
 return Cyc_Relations_rop2string(rop);};}
# 895
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned loc){
# 898
union Cyc_CfFlowInfo_FlowInfo _tmpFB=inflow;void*_tmpFD;struct Cyc_Dict_Dict _tmpFC;if((_tmpFB.BottomFL).tag == 1)
return;else{_tmpFC=((_tmpFB.ReachableFL).val).f1;_tmpFD=((_tmpFB.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_tmpFC;struct Cyc_List_List*relns=_tmpFD;
# 901
for(1;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmpFE=(struct Cyc_Relations_Reln*)req->hd;struct Cyc_Relations_Reln*reln=_tmpFE;
union Cyc_Relations_RelnOp rop1=reln->rop1;
union Cyc_Relations_RelnOp rop2=reln->rop2;
enum Cyc_Relations_Relation _tmpFF=Cyc_Relations_flip_relation(reln->relation);enum Cyc_Relations_Relation r=_tmpFF;
if((!Cyc_NewControlFlow_subst_param(exps,& rop1)|| !Cyc_NewControlFlow_subst_param(exps,& rop2))||
 Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,r,rop1,relns))){
struct _fat_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _fat_ptr s2=Cyc_Relations_relation2string(reln->relation);
struct _fat_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0,_tmp529.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp529;});struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0,_tmp528.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp528;});struct Cyc_String_pa_PrintArg_struct _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0,_tmp527.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp527;});struct Cyc_String_pa_PrintArg_struct _tmp105=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0,({
struct _fat_ptr _tmp5F6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_relns2string(relns));_tmp526.f1=_tmp5F6;});_tmp526;});void*_tmp100[4];_tmp100[0]=& _tmp102,_tmp100[1]=& _tmp103,_tmp100[2]=& _tmp104,_tmp100[3]=& _tmp105;({unsigned _tmp5F8=loc;struct _fat_ptr _tmp5F7=({const char*_tmp101="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_fat(_tmp101,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp5F8,_tmp5F7,_tag_fat(_tmp100,sizeof(void*),4));});});
break;}}
# 916
goto _LL0;}}_LL0:;}struct _tuple26{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple27{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 921
static struct _tuple17 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,int local_alias){
# 925
struct Cyc_CfFlowInfo_FlowEnv*_tmp106=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp106;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 938
{union Cyc_CfFlowInfo_FlowInfo _tmp107=inflow;void*_tmp109;struct Cyc_Dict_Dict _tmp108;if((_tmp107.BottomFL).tag == 1)
return({struct _tuple17 _tmp52A;({union Cyc_CfFlowInfo_FlowInfo _tmp5F9=Cyc_CfFlowInfo_BottomFL();_tmp52A.f1=_tmp5F9;}),_tmp52A.f2=fenv->unknown_all;_tmp52A;});else{_tmp108=((_tmp107.ReachableFL).val).f1;_tmp109=((_tmp107.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp108;struct Cyc_List_List*relns2=_tmp109;
d=d2;relns=relns2;}};}
# 953 "new_control_flow.cyc"
if((copy_ctxt && Cyc_NewControlFlow_is_local_var_rooted_path(e,0))&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 974 "new_control_flow.cyc"
struct _tuple19 _tmp10A=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple19 _stmttmp1A=_tmp10A;struct _tuple19 _tmp10B=_stmttmp1A;union Cyc_CfFlowInfo_AbsLVal _tmp10D;union Cyc_CfFlowInfo_FlowInfo _tmp10C;_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;{union Cyc_CfFlowInfo_FlowInfo flval=_tmp10C;union Cyc_CfFlowInfo_AbsLVal lval=_tmp10D;
struct _tuple19 _tmp10E=({struct _tuple19 _tmp52E;_tmp52E.f1=flval,_tmp52E.f2=lval;_tmp52E;});struct _tuple19 _stmttmp1B=_tmp10E;struct _tuple19 _tmp10F=_stmttmp1B;void*_tmp112;void*_tmp111;struct Cyc_Dict_Dict _tmp110;if(((_tmp10F.f1).ReachableFL).tag == 2){if(((_tmp10F.f2).PlaceL).tag == 1){_tmp110=(((_tmp10F.f1).ReachableFL).val).f1;_tmp111=(((_tmp10F.f1).ReachableFL).val).f2;_tmp112=((_tmp10F.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_tmp110;struct Cyc_List_List*r=_tmp111;struct Cyc_CfFlowInfo_Place*p=_tmp112;
# 977
void*_tmp113=Cyc_CfFlowInfo_lookup_place(fd,p);void*old_rval=_tmp113;
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume)){
({void*_tmp114=0U;({unsigned _tmp5FB=e->loc;struct _fat_ptr _tmp5FA=({const char*_tmp115="expression attempts to copy a consumed alias-free value";_tag_fat(_tmp115,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp5FB,_tmp5FA,_tag_fat(_tmp114,sizeof(void*),0));});});
return({struct _tuple17 _tmp52B;({union Cyc_CfFlowInfo_FlowInfo _tmp5FC=Cyc_CfFlowInfo_BottomFL();_tmp52B.f1=_tmp5FC;}),_tmp52B.f2=fenv->unknown_all;_tmp52B;});}else{
# 983
if(needs_unconsume)
# 985
return({struct _tuple17 _tmp52C;_tmp52C.f1=flval,({void*_tmp5FD=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,old_rval);_tmp52C.f2=_tmp5FD;});_tmp52C;});else{
# 988
void*_tmp116=Cyc_CfFlowInfo_make_unique_consumed(fenv,(void*)_check_null(e->topt),e,env->iteration_num,old_rval,local_alias);void*new_rval=_tmp116;
struct Cyc_Dict_Dict _tmp117=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,new_rval);struct Cyc_Dict_Dict outdict=_tmp117;
# 1000
return({struct _tuple17 _tmp52D;({union Cyc_CfFlowInfo_FlowInfo _tmp5FE=Cyc_CfFlowInfo_ReachableFL(outdict,r);_tmp52D.f1=_tmp5FE;}),_tmp52D.f2=old_rval;_tmp52D;});}}}}else{goto _LLB;}}else{_LLB:
# 1004
 goto _LL8;}_LL8:;}}{
# 1007
void*_tmp118=e->r;void*_stmttmp1C=_tmp118;void*_tmp119=_stmttmp1C;enum Cyc_Absyn_AggrKind _tmp11E;int _tmp11F;void*_tmp11D;int _tmp11C;enum Cyc_Absyn_Primop _tmp120;void*_tmp11B;struct _fat_ptr _tmp121;void*_tmp11A;switch(*((int*)_tmp119)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp119)->f4 == Cyc_Absyn_Null_to_NonNull){_tmp11A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1011
struct _tuple17 _tmp122=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);struct _tuple17 _stmttmp1D=_tmp122;struct _tuple17 _tmp123=_stmttmp1D;void*_tmp125;union Cyc_CfFlowInfo_FlowInfo _tmp124;_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp124;void*r1=_tmp125;
struct _tuple17 _tmp126=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _stmttmp1E=_tmp126;struct _tuple17 _tmp127=_stmttmp1E;void*_tmp129;union Cyc_CfFlowInfo_FlowInfo _tmp128;_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp128;void*r2=_tmp129;
return({struct _tuple17 _tmp52F;_tmp52F.f1=f2,_tmp52F.f2=r1;_tmp52F;});}}}}else{_tmp11A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1017
_tmp11A=e1;goto _LL13;}}case 12: _tmp11A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp11A;
_tmp11A=e1;goto _LL15;}case 41: _tmp11A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL15: {struct Cyc_Absyn_Exp*e1=_tmp11A;
_tmp11A=e1;goto _LL17;}case 13: _tmp11A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp11A;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);}case 2: _tmp121=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct _fat_ptr p=_tmp121;
# 1023
if(!({struct _fat_ptr _tmp5FF=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp5FF,({const char*_tmp12A="print_flow";_tag_fat(_tmp12A,sizeof(char),11U);}));})){
struct _fat_ptr _tmp12B=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp12B;
({struct Cyc_String_pa_PrintArg_struct _tmp12E=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0,_tmp530.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp530;});void*_tmp12C[1];_tmp12C[0]=& _tmp12E;({struct Cyc___cycFILE*_tmp601=Cyc_stderr;struct _fat_ptr _tmp600=({const char*_tmp12D="%s: current flow is\n";_tag_fat(_tmp12D,sizeof(char),21U);});Cyc_fprintf(_tmp601,_tmp600,_tag_fat(_tmp12C,sizeof(void*),1));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp12F=0U;({struct Cyc___cycFILE*_tmp603=Cyc_stderr;struct _fat_ptr _tmp602=({const char*_tmp130="\n";_tag_fat(_tmp130,sizeof(char),2U);});Cyc_fprintf(_tmp603,_tmp602,_tag_fat(_tmp12F,sizeof(void*),0));});});}else{
# 1029
if(!({struct _fat_ptr _tmp604=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp604,({const char*_tmp131="debug_on";_tag_fat(_tmp131,sizeof(char),9U);}));}));else{
# 1034
if(!({struct _fat_ptr _tmp605=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp605,({const char*_tmp132="debug_off";_tag_fat(_tmp132,sizeof(char),10U);}));}));}}
# 1042
return({struct _tuple17 _tmp531;_tmp531.f1=inflow,_tmp531.f2=fenv->zero;_tmp531;});}case 40:
# 1044
 goto _LL1D;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp119)->f1).Wstring_c).tag){case 1: _LL1D:
 goto _LL1F;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp119)->f1).Int_c).val).f2 == 0){_LL1F:
 return({struct _tuple17 _tmp532;_tmp532.f1=inflow,_tmp532.f2=fenv->zero;_tmp532;});}else{
goto _LL23;}case 8: _LL23:
 goto _LL25;case 9: _LL25:
 goto _LL27;default: _LL2B:
# 1053
 goto _LL2D;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)){case 2: _LL27:
# 1050
 return({struct _tuple17 _tmp533;_tmp533.f1=inflow,_tmp533.f2=fenv->notzeroall;_tmp533;});case 1:
# 1062
 return({struct _tuple17 _tmp534;_tmp534.f1=inflow,({void*_tmp606=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp534.f2=_tmp606;});_tmp534;});case 4: _tmp11A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp11A;
# 1067
if((int)vd->sc == (int)Cyc_Absyn_Static)
return({struct _tuple17 _tmp535;_tmp535.f1=inflow,({void*_tmp607=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp535.f2=_tmp607;});_tmp535;});
# 1070
_tmp11A=vd;goto _LL3D;}case 3: _tmp11A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL3D: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
_tmp11A=vd;goto _LL3F;}case 5: _tmp11A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL3F: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
# 1074
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp537;_tmp537.tag=0,_tmp537.f1=vd;_tmp537;});
return({struct _tuple17 _tmp536;_tmp536.f1=inflow,({void*_tmp609=({struct Cyc_Dict_Dict _tmp608=d;((void*(*)(struct Cyc_Dict_Dict,int(*)(void*,void*),void*))Cyc_Dict_lookup_other)(_tmp608,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp133=_cycalloc(sizeof(*_tmp133));*_tmp133=vdroot;_tmp133;}));});_tmp536.f2=_tmp609;});_tmp536;});}default:
# 1678
 goto _LL7B;}case 31: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp119)->f1 == 0)
# 1052
goto _LL2B;else{_tmp11A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_List_List*es=_tmp11A;struct Cyc_Absyn_Datatypedecl*tud=_tmp11B;
# 1540
_tmp11A=es;goto _LL69;}}case 42: _LL2D:
# 1054
 goto _LL2F;case 18: _LL2F:
 goto _LL31;case 17: _LL31:
 goto _LL33;case 19: _LL33:
 goto _LL35;case 33: _LL35:
 goto _LL37;case 32: _LL37:
 return({struct _tuple17 _tmp538;_tmp538.f1=inflow,_tmp538.f2=fenv->unknown_all;_tmp538;});case 3: _tmp120=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{enum Cyc_Absyn_Primop p=_tmp120;struct Cyc_List_List*es=_tmp11A;
# 1081
struct _tuple17 _tmp134=Cyc_NewControlFlow_anal_use_ints(env,inflow,es);struct _tuple17 _stmttmp1F=_tmp134;struct _tuple17 _tmp135=_stmttmp1F;void*_tmp137;union Cyc_CfFlowInfo_FlowInfo _tmp136;_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp136;void*r=_tmp137;
return({struct _tuple17 _tmp539;_tmp539.f1=f,_tmp539.f2=r;_tmp539;});}}case 5: _tmp11A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1085
struct Cyc_List_List _tmp138=({struct Cyc_List_List _tmp53D;_tmp53D.hd=e1,_tmp53D.tl=0;_tmp53D;});struct Cyc_List_List arg=_tmp138;
struct _tuple17 _tmp139=Cyc_NewControlFlow_anal_use_ints(env,inflow,& arg);struct _tuple17 _stmttmp20=_tmp139;struct _tuple17 _tmp13A=_stmttmp20;union Cyc_CfFlowInfo_FlowInfo _tmp13B;_tmp13B=_tmp13A.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp13B;
struct _tuple19 _tmp13C=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,e1);struct _tuple19 _stmttmp21=_tmp13C;struct _tuple19 _tmp13D=_stmttmp21;union Cyc_CfFlowInfo_AbsLVal _tmp13E;_tmp13E=_tmp13D.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_tmp13E;
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(e1->topt));
if(iszt){
# 1092
struct _tuple17 _tmp13F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp22=_tmp13F;struct _tuple17 _tmp140=_stmttmp22;void*_tmp142;union Cyc_CfFlowInfo_FlowInfo _tmp141;_tmp141=_tmp140.f1;_tmp142=_tmp140.f2;{union Cyc_CfFlowInfo_FlowInfo g=_tmp141;void*r=_tmp142;
Cyc_NewControlFlow_anal_derefR(env,inflow,g,e1,r,0);}}
# 1095
{struct _tuple26 _tmp143=({struct _tuple26 _tmp53A;_tmp53A.f1=lval,_tmp53A.f2=f;_tmp53A;});struct _tuple26 _stmttmp23=_tmp143;struct _tuple26 _tmp144=_stmttmp23;void*_tmp147;struct Cyc_Dict_Dict _tmp146;void*_tmp145;if(((_tmp144.f1).PlaceL).tag == 1){if(((_tmp144.f2).ReachableFL).tag == 2){_tmp145=((_tmp144.f1).PlaceL).val;_tmp146=(((_tmp144.f2).ReachableFL).val).f1;_tmp147=(((_tmp144.f2).ReachableFL).val).f2;{struct Cyc_CfFlowInfo_Place*p=_tmp145;struct Cyc_Dict_Dict outdict=_tmp146;struct Cyc_List_List*relns=_tmp147;
# 1097
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
f=({struct Cyc_Dict_Dict _tmp60A=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,p,fenv->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp60A,relns);});
# 1102
goto _LL92;}}else{goto _LL95;}}else{_LL95:
 goto _LL92;}_LL92:;}
# 1106
if(iszt)
return({struct _tuple17 _tmp53B;_tmp53B.f1=f,_tmp53B.f2=fenv->notzeroall;_tmp53B;});else{
return({struct _tuple17 _tmp53C;_tmp53C.f1=f,_tmp53C.f2=fenv->unknown_all;_tmp53C;});}}}}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f2 != 0){_tmp11A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*l=_tmp11A;struct Cyc_Absyn_Exp*r=_tmp11B;
# 1111
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp148=0U;({unsigned _tmp60C=e->loc;struct _fat_ptr _tmp60B=({const char*_tmp149="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp149,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp60C,_tmp60B,_tag_fat(_tmp148,sizeof(void*),0));});});
return({struct _tuple17 _tmp53E;({union Cyc_CfFlowInfo_FlowInfo _tmp60D=Cyc_CfFlowInfo_BottomFL();_tmp53E.f1=_tmp60D;}),_tmp53E.f2=fenv->unknown_all;_tmp53E;});}{
# 1115
struct _tuple17 _tmp14A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,r,local_alias);struct _tuple17 _stmttmp24=_tmp14A;struct _tuple17 _tmp14B=_stmttmp24;void*_tmp14D;union Cyc_CfFlowInfo_FlowInfo _tmp14C;_tmp14C=_tmp14B.f1;_tmp14D=_tmp14B.f2;{union Cyc_CfFlowInfo_FlowInfo right_out=_tmp14C;void*rval=_tmp14D;
struct _tuple19 _tmp14E=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,l);struct _tuple19 _stmttmp25=_tmp14E;struct _tuple19 _tmp14F=_stmttmp25;union Cyc_CfFlowInfo_AbsLVal _tmp151;union Cyc_CfFlowInfo_FlowInfo _tmp150;_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp150;union Cyc_CfFlowInfo_AbsLVal lval=_tmp151;
{union Cyc_CfFlowInfo_FlowInfo _tmp152=f;void*_tmp154;struct Cyc_Dict_Dict _tmp153;if((_tmp152.ReachableFL).tag == 2){_tmp153=((_tmp152.ReachableFL).val).f1;_tmp154=((_tmp152.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp153;struct Cyc_List_List*relns=_tmp154;
# 1119
{union Cyc_CfFlowInfo_AbsLVal _tmp155=lval;void*_tmp156;if((_tmp155.PlaceL).tag == 1){_tmp156=(_tmp155.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp156;
# 1123
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,l);
outdict=Cyc_CfFlowInfo_assign_place(fenv,l->loc,outdict,p,fenv->unknown_all);
# 1126
f=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
# 1130
goto _LLA2;}}else{
# 1133
goto _LLA2;}_LLA2:;}
# 1135
goto _LL9D;}}else{
goto _LL9D;}_LL9D:;}
# 1138
return({struct _tuple17 _tmp53F;_tmp53F.f1=f,_tmp53F.f2=fenv->unknown_all;_tmp53F;});}}}}}else{_tmp11A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1141
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp157=0U;({unsigned _tmp60F=e->loc;struct _fat_ptr _tmp60E=({const char*_tmp158="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp158,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp60F,_tmp60E,_tag_fat(_tmp157,sizeof(void*),0));});});
return({struct _tuple17 _tmp540;({union Cyc_CfFlowInfo_FlowInfo _tmp610=Cyc_CfFlowInfo_BottomFL();_tmp540.f1=_tmp610;}),_tmp540.f2=fenv->unknown_all;_tmp540;});}
# 1147
inflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp612=env;union Cyc_CfFlowInfo_FlowInfo _tmp611=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp612,_tmp611,({struct Cyc_Absyn_Exp*_tmp159[2];_tmp159[0]=e1,_tmp159[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp159,sizeof(struct Cyc_Absyn_Exp*),2));}));});{
# 1149
struct _tuple17 _tmp15A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);struct _tuple17 _stmttmp26=_tmp15A;struct _tuple17 _tmp15B=_stmttmp26;void*_tmp15D;union Cyc_CfFlowInfo_FlowInfo _tmp15C;_tmp15C=_tmp15B.f1;_tmp15D=_tmp15B.f2;{union Cyc_CfFlowInfo_FlowInfo right_out=_tmp15C;void*rval=_tmp15D;
struct _tuple19 _tmp15E=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,e1);struct _tuple19 _stmttmp27=_tmp15E;struct _tuple19 _tmp15F=_stmttmp27;union Cyc_CfFlowInfo_AbsLVal _tmp161;union Cyc_CfFlowInfo_FlowInfo _tmp160;_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_tmp160;union Cyc_CfFlowInfo_AbsLVal lval=_tmp161;
struct _tuple17 _tmp162=Cyc_NewControlFlow_do_assign(fenv,env,outflow,e1,lval,e2,rval,e->loc);struct _tuple17 _stmttmp28=_tmp162;struct _tuple17 _tmp163=_stmttmp28;void*_tmp165;union Cyc_CfFlowInfo_FlowInfo _tmp164;_tmp164=_tmp163.f1;_tmp165=_tmp163.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_tmp164;void*rval=_tmp165;
# 1155
return({struct _tuple17 _tmp541;_tmp541.f1=outflow,_tmp541.f2=rval;_tmp541;});}}}}}}case 9: _tmp11A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1158
struct _tuple17 _tmp166=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp29=_tmp166;struct _tuple17 _tmp167=_stmttmp29;void*_tmp169;union Cyc_CfFlowInfo_FlowInfo _tmp168;_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp168;void*r=_tmp169;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f,e2,local_alias);}}case 11: _tmp11A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1162
struct _tuple17 _tmp16A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);struct _tuple17 _stmttmp2A=_tmp16A;struct _tuple17 _tmp16B=_stmttmp2A;void*_tmp16D;union Cyc_CfFlowInfo_FlowInfo _tmp16C;_tmp16C=_tmp16B.f1;_tmp16D=_tmp16B.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp16C;void*r=_tmp16D;
Cyc_NewControlFlow_use_Rval(env,e1->loc,f,r);
return({struct _tuple17 _tmp542;({union Cyc_CfFlowInfo_FlowInfo _tmp614=Cyc_CfFlowInfo_BottomFL();_tmp542.f1=_tmp614;}),({void*_tmp613=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp542.f2=_tmp613;});_tmp542;});}}case 10: _tmp11A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_List_List*es=_tmp11B;
# 1167
struct Cyc_List_List*_tmp16E=es;struct Cyc_List_List*orig_es=_tmp16E;
struct _tuple17 _tmp16F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp2B=_tmp16F;struct _tuple17 _tmp170=_stmttmp2B;void*_tmp172;union Cyc_CfFlowInfo_FlowInfo _tmp171;_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp171;void*r1=_tmp172;
# 1170
Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);{
# 1173
struct _tuple24 _tmp173=({struct Cyc_NewControlFlow_AnalEnv*_tmp616=env;union Cyc_CfFlowInfo_FlowInfo _tmp615=f1;Cyc_NewControlFlow_anal_Rexps(_tmp616,_tmp615,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_copy)(es),1,1);});struct _tuple24 _stmttmp2C=_tmp173;struct _tuple24 _tmp174=_stmttmp2C;void*_tmp176;union Cyc_CfFlowInfo_FlowInfo _tmp175;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{union Cyc_CfFlowInfo_FlowInfo fst_outflow=_tmp175;struct Cyc_List_List*rvals=_tmp176;
# 1175
union Cyc_CfFlowInfo_FlowInfo _tmp177=Cyc_NewControlFlow_use_Rval(env,e1->loc,fst_outflow,r1);union Cyc_CfFlowInfo_FlowInfo outflow=_tmp177;
# 1177
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp178=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp2D=_tmp178;void*_tmp179=_stmttmp2D;void*_tmp17A;if(*((int*)_tmp179)== 3){_tmp17A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).elt_type;{void*t=_tmp17A;
# 1184
{void*_tmp17B=Cyc_Absyn_compress(t);void*_stmttmp2E=_tmp17B;void*_tmp17C=_stmttmp2E;void*_tmp17F;void*_tmp17E;void*_tmp17D;if(*((int*)_tmp17C)== 5){_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17C)->f1).attributes;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17C)->f1).requires_relns;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17C)->f1).ensures_relns;{struct Cyc_List_List*atts=_tmp17D;struct Cyc_List_List*req=_tmp17E;struct Cyc_List_List*ens=_tmp17F;
# 1186
requires=req;
ensures=ens;
for(1;atts != 0;atts=atts->tl){
# 1190
void*_tmp180=(void*)atts->hd;void*_stmttmp2F=_tmp180;void*_tmp181=_stmttmp2F;int _tmp182;switch(*((int*)_tmp181)){case 20: _tmp182=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp181)->f1;{int i=_tmp182;
# 1192
init_params=({struct Cyc_List_List*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->hd=(void*)i,_tmp183->tl=init_params;_tmp183;});goto _LLC6;}case 21: _tmp182=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp181)->f1;{int i=_tmp182;
# 1194
nolive_unique_params=({struct Cyc_List_List*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->hd=(void*)i,_tmp184->tl=nolive_unique_params;_tmp184;});
consume_params=({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->hd=(void*)i,_tmp185->tl=consume_params;_tmp185;});
goto _LLC6;}case 22: _tmp182=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp181)->f1;{int i=_tmp182;
# 1198
consume_params=({struct Cyc_List_List*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->hd=(void*)i,_tmp186->tl=consume_params;_tmp186;});
goto _LLC6;}default:
 goto _LLC6;}_LLC6:;}
# 1202
goto _LLC1;}}else{
({void*_tmp187=0U;({struct _fat_ptr _tmp617=({const char*_tmp188="anal_Rexp: bad function type";_tag_fat(_tmp188,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp617,_tag_fat(_tmp187,sizeof(void*),0));});});}_LLC1:;}
# 1205
goto _LLBC;}}else{
({void*_tmp189=0U;({struct _fat_ptr _tmp618=({const char*_tmp18A="anal_Rexp: bad function type";_tag_fat(_tmp18A,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp618,_tag_fat(_tmp189,sizeof(void*),0));});});}_LLBC:;}
# 1208
{int i=1;for(0;rvals != 0;(rvals=rvals->tl,es=((struct Cyc_List_List*)_check_null(es))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp18B=fst_outflow;struct Cyc_Dict_Dict _tmp18C;if((_tmp18B.BottomFL).tag == 1)
goto _LLCF;else{_tmp18C=((_tmp18B.ReachableFL).val).f1;{struct Cyc_Dict_Dict fst_d=_tmp18C;
# 1213
struct _tuple16 _tmp18D=Cyc_CfFlowInfo_unname_rval((void*)rvals->hd);struct _tuple16 _stmttmp30=_tmp18D;struct _tuple16 _tmp18E=_stmttmp30;void*_tmp18F;_tmp18F=_tmp18E.f1;{void*r=_tmp18F;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,fst_d,(void*)rvals->hd)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer((void*)rvals->hd))
({void*_tmp190=0U;({unsigned _tmp61A=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp619=({const char*_tmp191="expression may not be initialized";_tag_fat(_tmp191,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp61A,_tmp619,_tag_fat(_tmp190,sizeof(void*),0));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp192=outflow;void*_tmp194;struct Cyc_Dict_Dict _tmp193;if((_tmp192.BottomFL).tag == 1)
goto _LLD7;else{_tmp193=((_tmp192.ReachableFL).val).f1;_tmp194=((_tmp192.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp193;struct Cyc_List_List*relns=_tmp194;
# 1221
struct Cyc_Dict_Dict _tmp195=Cyc_CfFlowInfo_escape_deref(fenv,d,(void*)rvals->hd);struct Cyc_Dict_Dict ans_d=_tmp195;
{void*_tmp196=(void*)rvals->hd;void*_stmttmp31=_tmp196;void*_tmp197=_stmttmp31;void*_tmp198;switch(*((int*)_tmp197)){case 4: _tmp198=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp197)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp198;
# 1224
{void*_tmp199=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->topt));void*_stmttmp32=_tmp199;void*_tmp19A=_stmttmp32;void*_tmp19B;if(*((int*)_tmp19A)== 3){_tmp19B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A)->f1).elt_type;{void*t=_tmp19B;
# 1226
ans_d=({struct Cyc_CfFlowInfo_FlowEnv*_tmp61E=fenv;unsigned _tmp61D=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct Cyc_Dict_Dict _tmp61C=ans_d;struct Cyc_CfFlowInfo_Place*_tmp61B=p;Cyc_CfFlowInfo_assign_place(_tmp61E,_tmp61D,_tmp61C,_tmp61B,
Cyc_CfFlowInfo_typ_to_absrval(fenv,t,0,fenv->esc_all));});
# 1230
goto _LLE3;}}else{
({void*_tmp19C=0U;({struct _fat_ptr _tmp61F=({const char*_tmp19D="anal_Rexp:bad type for initialized arg";_tag_fat(_tmp19D,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp61F,_tag_fat(_tmp19C,sizeof(void*),0));});});}_LLE3:;}
# 1233
goto _LLDC;}case 5:
# 1235
({void*_tmp19E=0U;({struct _fat_ptr _tmp620=({const char*_tmp19F="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_fat(_tmp19F,sizeof(char),68U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp620,_tag_fat(_tmp19E,sizeof(void*),0));});});default:
# 1237
 goto _LLDC;}_LLDC:;}
# 1239
outflow=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);
goto _LLD7;}}_LLD7:;}
# 1242
goto _LLCF;}}}_LLCF:;}else{
# 1245
if(((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(nolive_unique_params,i))
# 1250
outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp624=env;unsigned _tmp623=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;void*_tmp622=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp621=outflow;Cyc_NewControlFlow_use_nounique_Rval(_tmp624,_tmp623,_tmp622,_tmp621,(void*)rvals->hd);});else{
# 1260
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);
if(!((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(consume_params,i)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),0))
outflow=Cyc_NewControlFlow_restore_noconsume_arg(env,outflow,(struct Cyc_Absyn_Exp*)es->hd,((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)rvals->hd);}}}}
# 1267
Cyc_NewControlFlow_check_fn_requires(env,outflow,orig_es,requires,e->loc);
# 1270
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e1->topt)))
return({struct _tuple17 _tmp543;({union Cyc_CfFlowInfo_FlowInfo _tmp626=Cyc_CfFlowInfo_BottomFL();_tmp543.f1=_tmp626;}),({void*_tmp625=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp543.f2=_tmp625;});_tmp543;});else{
# 1273
return({struct _tuple17 _tmp544;_tmp544.f1=outflow,({void*_tmp627=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp544.f2=_tmp627;});_tmp544;});}}}}}case 34: _tmp11C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).is_calloc;_tmp11A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).rgn;_tmp11B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).elt_type;_tmp11D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).num_elts;_tmp11F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).fat_result;{int iscalloc=_tmp11C;struct Cyc_Absyn_Exp*eopt=_tmp11A;void**topt=_tmp11B;struct Cyc_Absyn_Exp*exp=_tmp11D;int isfat=_tmp11F;
# 1276
void*place_val;
if(isfat)place_val=fenv->notzeroall;else{
if(iscalloc)place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*((void**)_check_null(topt)),0,fenv->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*((void**)_check_null(topt)),0,fenv->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(eopt != 0){
struct _tuple24 _tmp1A0=({struct Cyc_NewControlFlow_AnalEnv*_tmp629=env;union Cyc_CfFlowInfo_FlowInfo _tmp628=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp629,_tmp628,({struct Cyc_Absyn_Exp*_tmp1A4[2];_tmp1A4[0]=eopt,_tmp1A4[1]=exp;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1A4,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple24 _stmttmp33=_tmp1A0;struct _tuple24 _tmp1A1=_stmttmp33;void*_tmp1A3;union Cyc_CfFlowInfo_FlowInfo _tmp1A2;_tmp1A2=_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1A2;struct Cyc_List_List*rvals=_tmp1A3;
outflow=f;}}else{
# 1285
struct _tuple17 _tmp1A5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);struct _tuple17 _stmttmp34=_tmp1A5;struct _tuple17 _tmp1A6=_stmttmp34;union Cyc_CfFlowInfo_FlowInfo _tmp1A7;_tmp1A7=_tmp1A6.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1A7;
outflow=f;}}
# 1290
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple17 _tmp545;_tmp545.f1=outflow,({void*_tmp62A=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->tag=5,_tmp1A8->f1=place_val;_tmp1A8;});_tmp545.f2=_tmp62A;});_tmp545;});else{
# 1294
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->tag=1,_tmp1AE->f1=exp,_tmp1AE->f2=(void*)_check_null(e->topt);_tmp1AE;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->root=root,_tmp1AD->path=0;_tmp1AD;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->tag=4,_tmp1AC->f1=place;_tmp1AC;});
union Cyc_CfFlowInfo_FlowInfo _tmp1A9=outflow;void*_tmp1AB;struct Cyc_Dict_Dict _tmp1AA;if((_tmp1A9.BottomFL).tag == 1)
return({struct _tuple17 _tmp546;_tmp546.f1=outflow,_tmp546.f2=rval;_tmp546;});else{_tmp1AA=((_tmp1A9.ReachableFL).val).f1;_tmp1AB=((_tmp1A9.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp1AA;struct Cyc_List_List*relns=_tmp1AB;
# 1300
return({struct _tuple17 _tmp547;({union Cyc_CfFlowInfo_FlowInfo _tmp62C=({struct Cyc_Dict_Dict _tmp62B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp62B,relns);});_tmp547.f1=_tmp62C;}),_tmp547.f2=rval;_tmp547;});}};}}}case 35: _tmp11A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1305
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1311
struct _tuple24 _tmp1AF=({struct Cyc_NewControlFlow_AnalEnv*_tmp62E=env;union Cyc_CfFlowInfo_FlowInfo _tmp62D=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp62E,_tmp62D,({struct Cyc_Absyn_Exp*_tmp1CA[2];_tmp1CA[0]=e1,_tmp1CA[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CA,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);});struct _tuple24 _stmttmp35=_tmp1AF;struct _tuple24 _tmp1B0=_stmttmp35;void*_tmp1B2;union Cyc_CfFlowInfo_FlowInfo _tmp1B1;_tmp1B1=_tmp1B0.f1;_tmp1B2=_tmp1B0.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1B1;struct Cyc_List_List*rvals=_tmp1B2;
left_rval=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(rvals->tl))->hd;
outflow=f;{
# 1317
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1B3=right_rval;if(*((int*)_tmp1B3)== 0)
goto _LLF6;else{
({void*_tmp1B4=0U;({unsigned _tmp630=e1->loc;struct _fat_ptr _tmp62F=({const char*_tmp1B5="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp1B5,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp630,_tmp62F,_tag_fat(_tmp1B4,sizeof(void*),0));});});}_LLF6:;}else{
# 1328
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1B6=left_rval;if(*((int*)_tmp1B6)== 0)
goto _LLFB;else{
({void*_tmp1B7=0U;({unsigned _tmp632=e2->loc;struct _fat_ptr _tmp631=({const char*_tmp1B8="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp1B8,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp632,_tmp631,_tag_fat(_tmp1B7,sizeof(void*),0));});});}_LLFB:;}}{
# 1336
struct _tuple19 _tmp1B9=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e1);struct _tuple19 _stmttmp36=_tmp1B9;struct _tuple19 _tmp1BA=_stmttmp36;union Cyc_CfFlowInfo_AbsLVal _tmp1BB;_tmp1BB=_tmp1BA.f2;{union Cyc_CfFlowInfo_AbsLVal left_lval=_tmp1BB;
struct _tuple19 _tmp1BC=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e2);struct _tuple19 _stmttmp37=_tmp1BC;struct _tuple19 _tmp1BD=_stmttmp37;union Cyc_CfFlowInfo_AbsLVal _tmp1BE;_tmp1BE=_tmp1BD.f2;{union Cyc_CfFlowInfo_AbsLVal right_lval=_tmp1BE;
{union Cyc_CfFlowInfo_FlowInfo _tmp1BF=outflow;void*_tmp1C1;struct Cyc_Dict_Dict _tmp1C0;if((_tmp1BF.ReachableFL).tag == 2){_tmp1C0=((_tmp1BF.ReachableFL).val).f1;_tmp1C1=((_tmp1BF.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp1C0;struct Cyc_List_List*relns=_tmp1C1;
# 1340
{union Cyc_CfFlowInfo_AbsLVal _tmp1C2=left_lval;void*_tmp1C3;if((_tmp1C2.PlaceL).tag == 1){_tmp1C3=(_tmp1C2.PlaceL).val;{struct Cyc_CfFlowInfo_Place*lp=_tmp1C3;
# 1342
outdict=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,lp,right_rval);
goto _LL10B;}}else{
# 1348
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,right_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp1C4=0U;({unsigned _tmp634=e2->loc;struct _fat_ptr _tmp633=({const char*_tmp1C5="expression may not be fully initialized";_tag_fat(_tmp1C5,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp634,_tmp633,_tag_fat(_tmp1C4,sizeof(void*),0));});});
goto _LL10B;}_LL10B:;}
# 1352
{union Cyc_CfFlowInfo_AbsLVal _tmp1C6=right_lval;void*_tmp1C7;if((_tmp1C6.PlaceL).tag == 1){_tmp1C7=(_tmp1C6.PlaceL).val;{struct Cyc_CfFlowInfo_Place*rp=_tmp1C7;
# 1354
outdict=Cyc_CfFlowInfo_assign_place(fenv,e2->loc,outdict,rp,left_rval);
goto _LL110;}}else{
# 1357
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,left_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp1C8=0U;({unsigned _tmp636=e1->loc;struct _fat_ptr _tmp635=({const char*_tmp1C9="expression may not be fully initialized";_tag_fat(_tmp1C9,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp636,_tmp635,_tag_fat(_tmp1C8,sizeof(void*),0));});});
goto _LL110;}_LL110:;}
# 1362
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e2);
# 1365
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL106;}}else{
goto _LL106;}_LL106:;}
# 1372
return({struct _tuple17 _tmp548;_tmp548.f1=outflow,_tmp548.f2=fenv->unknown_all;_tmp548;});}}}}}}case 16: _tmp11A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*eopt=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1375
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(eopt != 0){
struct _tuple24 _tmp1CB=({struct Cyc_NewControlFlow_AnalEnv*_tmp638=env;union Cyc_CfFlowInfo_FlowInfo _tmp637=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp638,_tmp637,({struct Cyc_Absyn_Exp*_tmp1CF[2];_tmp1CF[0]=eopt,_tmp1CF[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CF,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple24 _stmttmp38=_tmp1CB;struct _tuple24 _tmp1CC=_stmttmp38;void*_tmp1CE;union Cyc_CfFlowInfo_FlowInfo _tmp1CD;_tmp1CD=_tmp1CC.f1;_tmp1CE=_tmp1CC.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1CD;struct Cyc_List_List*rvals=_tmp1CE;
outflow=f;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd;}}else{
# 1382
struct _tuple17 _tmp1D0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);struct _tuple17 _stmttmp39=_tmp1D0;struct _tuple17 _tmp1D1=_stmttmp39;void*_tmp1D3;union Cyc_CfFlowInfo_FlowInfo _tmp1D2;_tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1D2;void*r=_tmp1D3;
outflow=f;
place_val=r;}}
# 1387
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple17 _tmp549;_tmp549.f1=outflow,({void*_tmp639=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->tag=5,_tmp1D4->f1=place_val;_tmp1D4;});_tmp549.f2=_tmp639;});_tmp549;});else{
# 1391
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->tag=1,_tmp1DA->f1=e2,_tmp1DA->f2=(void*)_check_null(e->topt);_tmp1DA;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->root=root,_tmp1D9->path=0;_tmp1D9;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->tag=4,_tmp1D8->f1=place;_tmp1D8;});
union Cyc_CfFlowInfo_FlowInfo _tmp1D5=outflow;void*_tmp1D7;struct Cyc_Dict_Dict _tmp1D6;if((_tmp1D5.BottomFL).tag == 1)
return({struct _tuple17 _tmp54A;_tmp54A.f1=outflow,_tmp54A.f2=rval;_tmp54A;});else{_tmp1D6=((_tmp1D5.ReachableFL).val).f1;_tmp1D7=((_tmp1D5.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp1D6;struct Cyc_List_List*relns=_tmp1D7;
# 1397
return({struct _tuple17 _tmp54B;({union Cyc_CfFlowInfo_FlowInfo _tmp63B=({struct Cyc_Dict_Dict _tmp63A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp63A,relns);});_tmp54B.f1=_tmp63B;}),_tmp54B.f2=rval;_tmp54B;});}};}}case 15: _tmp11A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1402
struct _tuple19 _tmp1DB=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e1);struct _tuple19 _stmttmp3A=_tmp1DB;struct _tuple19 _tmp1DC=_stmttmp3A;union Cyc_CfFlowInfo_AbsLVal _tmp1DE;union Cyc_CfFlowInfo_FlowInfo _tmp1DD;_tmp1DD=_tmp1DC.f1;_tmp1DE=_tmp1DC.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1DD;union Cyc_CfFlowInfo_AbsLVal l=_tmp1DE;
union Cyc_CfFlowInfo_AbsLVal _tmp1DF=l;void*_tmp1E0;if((_tmp1DF.UnknownL).tag == 2)
return({struct _tuple17 _tmp54C;_tmp54C.f1=f,_tmp54C.f2=fenv->notzeroall;_tmp54C;});else{_tmp1E0=(_tmp1DF.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp1E0;
return({struct _tuple17 _tmp54D;_tmp54D.f1=f,({void*_tmp63C=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->tag=4,_tmp1E1->f1=p;_tmp1E1;});_tmp54D.f2=_tmp63C;});_tmp54D;});}};}}case 20: _tmp11A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1409
struct _tuple17 _tmp1E2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp3B=_tmp1E2;struct _tuple17 _tmp1E3=_stmttmp3B;void*_tmp1E5;union Cyc_CfFlowInfo_FlowInfo _tmp1E4;_tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1E4;void*r=_tmp1E5;
return Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,r,0);}}case 21: _tmp11A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*field=_tmp11B;
# 1413
struct _tuple17 _tmp1E6=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp3C=_tmp1E6;struct _tuple17 _tmp1E7=_stmttmp3C;void*_tmp1E9;union Cyc_CfFlowInfo_FlowInfo _tmp1E8;_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1E8;void*r=_tmp1E9;
void*_tmp1EA=(void*)_check_null(e1->topt);void*e1_type=_tmp1EA;
# 1416
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple17 _tmp54E;_tmp54E.f1=f,({void*_tmp63D=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp54E.f2=_tmp63D;});_tmp54E;});
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,field,f);{
struct _tuple16 _tmp1EB=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp3D=_tmp1EB;struct _tuple16 _tmp1EC=_stmttmp3D;void*_tmp1ED;_tmp1ED=_tmp1EC.f1;{void*r=_tmp1ED;
void*_tmp1EE=r;struct _fat_ptr _tmp1F1;int _tmp1F0;int _tmp1EF;if(*((int*)_tmp1EE)== 6){_tmp1EF=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1EE)->f1).is_union;_tmp1F0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1EE)->f1).fieldnum;_tmp1F1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1EE)->f2;{int is_union=_tmp1EF;int fnum=_tmp1F0;struct _fat_ptr rdict=_tmp1F1;
# 1423
int _tmp1F2=Cyc_CfFlowInfo_get_field_index((void*)_check_null(e1->topt),field);int field_no=_tmp1F2;
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _tmp54F;_tmp54F.f1=f,({void*_tmp63E=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),1,fenv->unknown_none);_tmp54F.f2=_tmp63E;});_tmp54F;});
return({struct _tuple17 _tmp550;_tmp550.f1=f,_tmp550.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_tmp550;});}}else{
# 1428
({void*_tmp1F3=0U;({struct Cyc___cycFILE*_tmp640=Cyc_stderr;struct _fat_ptr _tmp63F=({const char*_tmp1F4="the bad rexp is :";_tag_fat(_tmp1F4,sizeof(char),18U);});Cyc_fprintf(_tmp640,_tmp63F,_tag_fat(_tmp1F3,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(r);
({struct Cyc_String_pa_PrintArg_struct _tmp1F7=({struct Cyc_String_pa_PrintArg_struct _tmp551;_tmp551.tag=0,({struct _fat_ptr _tmp641=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp551.f1=_tmp641;});_tmp551;});void*_tmp1F5[1];_tmp1F5[0]=& _tmp1F7;({struct _fat_ptr _tmp642=({const char*_tmp1F6="\nanal_Rexp: AggrMember: %s";_tag_fat(_tmp1F6,sizeof(char),27U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp642,_tag_fat(_tmp1F5,sizeof(void*),1));});});};}}}}case 38: _tmp11A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*field=_tmp11B;
# 1436
struct _tuple17 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp3E=_tmp1F8;struct _tuple17 _tmp1F9=_stmttmp3E;void*_tmp1FB;union Cyc_CfFlowInfo_FlowInfo _tmp1FA;_tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1FA;void*r=_tmp1FB;
# 1438
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(e1->topt)))
return({struct _tuple17 _tmp552;_tmp552.f1=f,_tmp552.f2=fenv->unknown_all;_tmp552;});{
struct _tuple16 _tmp1FC=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp3F=_tmp1FC;struct _tuple16 _tmp1FD=_stmttmp3F;void*_tmp1FE;_tmp1FE=_tmp1FD.f1;{void*r=_tmp1FE;
void*_tmp1FF=r;struct _fat_ptr _tmp202;int _tmp201;int _tmp200;if(*((int*)_tmp1FF)== 6){_tmp200=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FF)->f1).is_union;_tmp201=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FF)->f1).fieldnum;_tmp202=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FF)->f2;{int is_union=_tmp200;int fnum=_tmp201;struct _fat_ptr rdict=_tmp202;
# 1443
int _tmp203=Cyc_CfFlowInfo_get_field_index((void*)_check_null(e1->topt),field);int field_no=_tmp203;
if(is_union && fnum != -1){
if(fnum != field_no)
return({struct _tuple17 _tmp553;_tmp553.f1=f,_tmp553.f2=fenv->zero;_tmp553;});else{
# 1448
return({struct _tuple17 _tmp554;_tmp554.f1=f,_tmp554.f2=fenv->notzeroall;_tmp554;});}}else{
# 1450
return({struct _tuple17 _tmp555;_tmp555.f1=f,_tmp555.f2=fenv->unknown_all;_tmp555;});}}}else{
# 1452
({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp556;_tmp556.tag=0,({struct _fat_ptr _tmp643=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp556.f1=_tmp643;});_tmp556;});void*_tmp204[1];_tmp204[0]=& _tmp206;({struct _fat_ptr _tmp644=({const char*_tmp205="anal_Rexp: TagCheck_e: %s";_tag_fat(_tmp205,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp644,_tag_fat(_tmp204,sizeof(void*),1));});});};}}}}case 22: _tmp11A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*field=_tmp11B;
# 1456
struct _tuple17 _tmp207=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp40=_tmp207;struct _tuple17 _tmp208=_stmttmp40;void*_tmp20A;union Cyc_CfFlowInfo_FlowInfo _tmp209;_tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp209;void*r1=_tmp20A;
# 1459
struct _tuple17 _tmp20B=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _stmttmp41=_tmp20B;struct _tuple17 _tmp20C=_stmttmp41;void*_tmp20E;union Cyc_CfFlowInfo_FlowInfo _tmp20D;_tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp20D;void*r2=_tmp20E;
# 1462
void*_tmp20F=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp42=_tmp20F;void*_tmp210=_stmttmp42;void*_tmp211;if(*((int*)_tmp210)== 3){_tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->f1).elt_type;{void*t2=_tmp211;
# 1464
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t2))
# 1466
return({struct _tuple17 _tmp557;_tmp557.f1=f2,({void*_tmp645=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp557.f2=_tmp645;});_tmp557;});
# 1468
if(Cyc_Absyn_is_require_union_type(t2))
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,field,f1);{
# 1471
struct _tuple16 _tmp212=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _stmttmp43=_tmp212;struct _tuple16 _tmp213=_stmttmp43;void*_tmp214;_tmp214=_tmp213.f1;{void*r2=_tmp214;
void*_tmp215=r2;struct _fat_ptr _tmp218;int _tmp217;int _tmp216;if(*((int*)_tmp215)== 6){_tmp216=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp215)->f1).is_union;_tmp217=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp215)->f1).fieldnum;_tmp218=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp215)->f2;{int is_union=_tmp216;int fnum=_tmp217;struct _fat_ptr rdict=_tmp218;
# 1474
int _tmp219=Cyc_CfFlowInfo_get_field_index(t2,field);int field_no=_tmp219;
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _tmp558;_tmp558.f1=f2,({void*_tmp646=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),1,fenv->unknown_none);_tmp558.f2=_tmp646;});_tmp558;});
return({struct _tuple17 _tmp559;_tmp559.f1=f2,_tmp559.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_tmp559;});}}else{
({void*_tmp21A=0U;({struct _fat_ptr _tmp647=({const char*_tmp21B="anal_Rexp: AggrArrow";_tag_fat(_tmp21B,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp647,_tag_fat(_tmp21A,sizeof(void*),0));});});};}}}}else{
# 1480
({void*_tmp21C=0U;({struct _fat_ptr _tmp648=({const char*_tmp21D="anal_Rexp: AggrArrow ptr";_tag_fat(_tmp21D,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp648,_tag_fat(_tmp21C,sizeof(void*),0));});});};}}}case 6: _tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp119)->f2;_tmp11D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;struct Cyc_Absyn_Exp*e3=_tmp11D;
# 1484
struct _tuple20 _tmp21E=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp44=_tmp21E;struct _tuple20 _tmp21F=_stmttmp44;union Cyc_CfFlowInfo_FlowInfo _tmp221;union Cyc_CfFlowInfo_FlowInfo _tmp220;_tmp220=_tmp21F.f1;_tmp221=_tmp21F.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp220;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp221;
struct _tuple17 _tmp222=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);struct _tuple17 pr2=_tmp222;
struct _tuple17 _tmp223=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e3,local_alias);struct _tuple17 pr3=_tmp223;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}case 7: _tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1490
struct _tuple20 _tmp224=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp45=_tmp224;struct _tuple20 _tmp225=_stmttmp45;union Cyc_CfFlowInfo_FlowInfo _tmp227;union Cyc_CfFlowInfo_FlowInfo _tmp226;_tmp226=_tmp225.f1;_tmp227=_tmp225.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp226;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp227;
struct _tuple17 _tmp228=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);struct _tuple17 _stmttmp46=_tmp228;struct _tuple17 _tmp229=_stmttmp46;void*_tmp22B;union Cyc_CfFlowInfo_FlowInfo _tmp22A;_tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp22A;void*f2r=_tmp22B;
struct _tuple17 _tmp22C=({struct _tuple17 _tmp55B;_tmp55B.f1=f2t,_tmp55B.f2=f2r;_tmp55B;});struct _tuple17 pr2=_tmp22C;
struct _tuple17 _tmp22D=({struct _tuple17 _tmp55A;_tmp55A.f1=f1f,_tmp55A.f2=fenv->zero;_tmp55A;});struct _tuple17 pr3=_tmp22D;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 8: _tmp11A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1497
struct _tuple20 _tmp22E=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp47=_tmp22E;struct _tuple20 _tmp22F=_stmttmp47;union Cyc_CfFlowInfo_FlowInfo _tmp231;union Cyc_CfFlowInfo_FlowInfo _tmp230;_tmp230=_tmp22F.f1;_tmp231=_tmp22F.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp230;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp231;
struct _tuple17 _tmp232=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e2,local_alias);struct _tuple17 _stmttmp48=_tmp232;struct _tuple17 _tmp233=_stmttmp48;void*_tmp235;union Cyc_CfFlowInfo_FlowInfo _tmp234;_tmp234=_tmp233.f1;_tmp235=_tmp233.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp234;void*f2r=_tmp235;
struct _tuple17 _tmp236=({struct _tuple17 _tmp55D;_tmp55D.f1=f2t,_tmp55D.f2=f2r;_tmp55D;});struct _tuple17 pr2=_tmp236;
struct _tuple17 _tmp237=({struct _tuple17 _tmp55C;_tmp55C.f1=f1t,_tmp55C.f2=fenv->notzeroall;_tmp55C;});struct _tuple17 pr3=_tmp237;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 23: _tmp11A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1504
struct _tuple24 _tmp238=({struct Cyc_NewControlFlow_AnalEnv*_tmp64A=env;union Cyc_CfFlowInfo_FlowInfo _tmp649=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp64A,_tmp649,({struct Cyc_Absyn_Exp*_tmp254[2];_tmp254[0]=e1,_tmp254[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp254,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple24 _stmttmp49=_tmp238;struct _tuple24 _tmp239=_stmttmp49;void*_tmp23B;union Cyc_CfFlowInfo_FlowInfo _tmp23A;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp23A;struct Cyc_List_List*rvals=_tmp23B;
# 1508
union Cyc_CfFlowInfo_FlowInfo _tmp23C=f;union Cyc_CfFlowInfo_FlowInfo f2=_tmp23C;
{union Cyc_CfFlowInfo_FlowInfo _tmp23D=f;void*_tmp23F;struct Cyc_Dict_Dict _tmp23E;if((_tmp23D.ReachableFL).tag == 2){_tmp23E=((_tmp23D.ReachableFL).val).f1;_tmp23F=((_tmp23D.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp23E;struct Cyc_List_List*relns=_tmp23F;
# 1513
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp240=0U;({unsigned _tmp64C=e2->loc;struct _fat_ptr _tmp64B=({const char*_tmp241="expression may not be initialized";_tag_fat(_tmp241,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp64C,_tmp64B,_tag_fat(_tmp240,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp242=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);struct Cyc_List_List*new_relns=_tmp242;
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL166;}}}else{
goto _LL166;}_LL166:;}{
# 1521
void*_tmp243=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp4A=_tmp243;void*_tmp244=_stmttmp4A;void*_tmp245;switch(*((int*)_tmp244)){case 6: _tmp245=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp244)->f1;{struct Cyc_List_List*tqts=_tmp245;
# 1523
struct _tuple16 _tmp246=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(rvals))->hd);struct _tuple16 _stmttmp4B=_tmp246;struct _tuple16 _tmp247=_stmttmp4B;void*_tmp248;_tmp248=_tmp247.f1;{void*r=_tmp248;
void*_tmp249=r;struct _fat_ptr _tmp24A;if(*((int*)_tmp249)== 6){_tmp24A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp249)->f2;{struct _fat_ptr rdict=_tmp24A;
# 1526
unsigned i=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;
return({struct _tuple17 _tmp55E;_tmp55E.f1=f2,_tmp55E.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),(int)i));_tmp55E;});}}else{
({void*_tmp24B=0U;({struct _fat_ptr _tmp64D=({const char*_tmp24C="anal_Rexp: Subscript";_tag_fat(_tmp24C,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp64D,_tag_fat(_tmp24B,sizeof(void*),0));});});};}}case 3:  {
# 1531
struct _tuple17 _tmp24D=Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,(void*)((struct Cyc_List_List*)_check_null(rvals))->hd,e2);struct _tuple17 _stmttmp4C=_tmp24D;struct _tuple17 _tmp24E=_stmttmp4C;void*_tmp250;union Cyc_CfFlowInfo_FlowInfo _tmp24F;_tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;{union Cyc_CfFlowInfo_FlowInfo f3=_tmp24F;void*r=_tmp250;
union Cyc_CfFlowInfo_FlowInfo _tmp251=f3;if((_tmp251.BottomFL).tag == 1)
return({struct _tuple17 _tmp55F;_tmp55F.f1=f3,_tmp55F.f2=r;_tmp55F;});else{
return({struct _tuple17 _tmp560;_tmp560.f1=f2,_tmp560.f2=r;_tmp560;});};}}default:
# 1536
({void*_tmp252=0U;({struct _fat_ptr _tmp64E=({const char*_tmp253="anal_Rexp: Subscript -- bad type";_tag_fat(_tmp253,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp64E,_tag_fat(_tmp252,sizeof(void*),0));});});};}}}case 24: _tmp11A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL69: {struct Cyc_List_List*es=_tmp11A;
# 1542
struct _tuple24 _tmp255=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp4D=_tmp255;struct _tuple24 _tmp256=_stmttmp4D;void*_tmp258;union Cyc_CfFlowInfo_FlowInfo _tmp257;_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp257;struct Cyc_List_List*rvals=_tmp258;
struct _fat_ptr aggrdict=({unsigned _tmp25C=(unsigned)
((int(*)(struct Cyc_List_List*))Cyc_List_length)(es);void**_tmp25B=_cycalloc(_check_times(_tmp25C,sizeof(void*)));({{unsigned _tmp562=(unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(es);unsigned i;for(i=0;i < _tmp562;++ i){({
void*_tmp64F=({void*_tmp25A=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;void*temp=_tmp25A;
rvals=rvals->tl;
temp;});
# 1545
_tmp25B[i]=_tmp64F;});}}0;});_tag_fat(_tmp25B,sizeof(void*),_tmp25C);});
# 1549
return({struct _tuple17 _tmp561;_tmp561.f1=f,({void*_tmp650=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->tag=6,(_tmp259->f1).is_union=0,(_tmp259->f1).fieldnum=- 1,_tmp259->f2=aggrdict;_tmp259;});_tmp561.f2=_tmp650;});_tmp561;});}}case 30: _tmp11A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_List_List*des=_tmp11A;
# 1552
struct Cyc_List_List*fs;
{void*_tmp25D=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp4E=_tmp25D;void*_tmp25E=_stmttmp4E;void*_tmp25F;if(*((int*)_tmp25E)== 7){_tmp25F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25E)->f2;{struct Cyc_List_List*f=_tmp25F;
fs=f;goto _LL185;}}else{
({void*_tmp260=0U;({struct _fat_ptr _tmp651=({const char*_tmp261="anal_Rexp:anon struct has bad type";_tag_fat(_tmp261,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp651,_tag_fat(_tmp260,sizeof(void*),0));});});}_LL185:;}
# 1557
_tmp11A=des;_tmp11E=0U;_tmp11B=fs;goto _LL6D;}case 29: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4)->impl != 0){_tmp11A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f3;_tmp11E=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4)->kind;_tmp11B=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4)->impl)->fields;_LL6D: {struct Cyc_List_List*des=_tmp11A;enum Cyc_Absyn_AggrKind kind=_tmp11E;struct Cyc_List_List*fs=_tmp11B;
# 1559
void*exp_type=(void*)_check_null(e->topt);
struct _tuple24 _tmp262=({struct Cyc_NewControlFlow_AnalEnv*_tmp653=env;union Cyc_CfFlowInfo_FlowInfo _tmp652=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp653,_tmp652,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple27*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd,des),1,1);});struct _tuple24 _stmttmp4F=_tmp262;struct _tuple24 _tmp263=_stmttmp4F;void*_tmp265;union Cyc_CfFlowInfo_FlowInfo _tmp264;_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp264;struct Cyc_List_List*rvals=_tmp265;
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)kind == (int)Cyc_Absyn_UnionA,fenv->unknown_all);
# 1564
int field_no=-1;
{int i=0;for(0;rvals != 0;(rvals=rvals->tl,des=des->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple27*)((struct Cyc_List_List*)_check_null(des))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp266=(void*)ds->hd;void*_stmttmp50=_tmp266;void*_tmp267=_stmttmp50;void*_tmp268;if(*((int*)_tmp267)== 0)
({void*_tmp269=0U;({struct _fat_ptr _tmp654=({const char*_tmp26A="anal_Rexp:Aggregate_e";_tag_fat(_tmp26A,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp654,_tag_fat(_tmp269,sizeof(void*),0));});});else{_tmp268=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp267)->f1;{struct _fat_ptr*fld=_tmp268;
# 1571
field_no=Cyc_CfFlowInfo_get_field_index_fs(fs,fld);
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),field_no))=(void*)rvals->hd;
# 1574
if((int)kind == (int)1U){
struct Cyc_Absyn_Exp*_tmp26B=(*((struct _tuple27*)des->hd)).f2;struct Cyc_Absyn_Exp*e=_tmp26B;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,(void*)rvals->hd);
# 1578
Cyc_NewControlFlow_check_union_requires(e->loc,exp_type,fld,f);}}};}}}{
# 1581
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26C=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->tag=6,(_tmp26D->f1).is_union=(int)kind == (int)Cyc_Absyn_UnionA,(_tmp26D->f1).fieldnum=field_no,_tmp26D->f2=aggrdict;_tmp26D;});struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*res=_tmp26C;
return({struct _tuple17 _tmp563;_tmp563.f1=f,_tmp563.f2=(void*)res;_tmp563;});}}}}else{goto _LL6E;}}else{_LL6E:
# 1585
({struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0,({struct _fat_ptr _tmp655=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp564.f1=_tmp655;});_tmp564;});void*_tmp26E[1];_tmp26E[0]=& _tmp270;({struct _fat_ptr _tmp656=({const char*_tmp26F="anal_Rexp:missing aggrdeclimpl in %s";_tag_fat(_tmp26F,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp656,_tag_fat(_tmp26E,sizeof(void*),1));});});}case 26: _tmp11A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_List_List*dles=_tmp11A;
# 1587
struct Cyc_List_List*_tmp271=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple27*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd,dles);struct Cyc_List_List*es=_tmp271;
struct _tuple24 _tmp272=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp51=_tmp272;struct _tuple24 _tmp273=_stmttmp51;void*_tmp275;union Cyc_CfFlowInfo_FlowInfo _tmp274;_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_tmp274;struct Cyc_List_List*rvals=_tmp275;
for(1;rvals != 0;(rvals=rvals->tl,es=es->tl)){
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);}
# 1592
return({struct _tuple17 _tmp565;_tmp565.f1=outflow,({void*_tmp657=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->notzeroall);_tmp565.f2=_tmp657;});_tmp565;});}}case 27: _tmp11A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f2;_tmp11D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f3;_tmp11C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp11A;struct Cyc_Absyn_Exp*e1=_tmp11B;struct Cyc_Absyn_Exp*e2=_tmp11D;int zt=_tmp11C;
# 1596
struct _tuple17 _tmp276=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);struct _tuple17 _stmttmp52=_tmp276;struct _tuple17 _tmp277=_stmttmp52;void*_tmp279;union Cyc_CfFlowInfo_FlowInfo _tmp278;_tmp278=_tmp277.f1;_tmp279=_tmp277.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp278;void*r1=_tmp279;
union Cyc_CfFlowInfo_FlowInfo _tmp27A=f1;void*_tmp27C;struct Cyc_Dict_Dict _tmp27B;if((_tmp27A.BottomFL).tag == 1)
return({struct _tuple17 _tmp566;_tmp566.f1=f1,_tmp566.f2=fenv->unknown_all;_tmp566;});else{_tmp27B=((_tmp27A.ReachableFL).val).f1;_tmp27C=((_tmp27A.ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp27B;struct Cyc_List_List*relns=_tmp27C;
# 1600
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d1,r1)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp27D=0U;({unsigned _tmp659=e1->loc;struct _fat_ptr _tmp658=({const char*_tmp27E="expression may not be initialized";_tag_fat(_tmp27E,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp659,_tmp658,_tag_fat(_tmp27D,sizeof(void*),0));});});{
# 1604
struct Cyc_List_List*new_relns=relns;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(vd);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& n2))
new_relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,relns);
if(relns != new_relns)
f1=Cyc_CfFlowInfo_ReachableFL(d1,new_relns);{
# 1613
void*_tmp27F=r1;switch(*((int*)_tmp27F)){case 0:
 return({struct _tuple17 _tmp567;_tmp567.f1=f1,_tmp567.f2=fenv->unknown_all;_tmp567;});case 1:
 goto _LL1A3;case 4: _LL1A3: {
# 1617
struct Cyc_List_List _tmp280=({struct Cyc_List_List _tmp56A;_tmp56A.hd=vd,_tmp56A.tl=0;_tmp56A;});struct Cyc_List_List l=_tmp280;
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
# 1620
struct _tuple17 _tmp281=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);struct _tuple17 _stmttmp53=_tmp281;struct _tuple17 _tmp282=_stmttmp53;void*_tmp284;union Cyc_CfFlowInfo_FlowInfo _tmp283;_tmp283=_tmp282.f1;_tmp284=_tmp282.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp283;void*r2=_tmp284;
{union Cyc_CfFlowInfo_FlowInfo _tmp285=f2;struct Cyc_Dict_Dict _tmp286;if((_tmp285.BottomFL).tag == 1)
return({struct _tuple17 _tmp568;_tmp568.f1=f2,_tmp568.f2=fenv->unknown_all;_tmp568;});else{_tmp286=((_tmp285.ReachableFL).val).f1;{struct Cyc_Dict_Dict d2=_tmp286;
# 1624
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp287=0U;({unsigned _tmp65B=e1->loc;struct _fat_ptr _tmp65A=({const char*_tmp288="expression may not be initialized";_tag_fat(_tmp288,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65B,_tmp65A,_tag_fat(_tmp287,sizeof(void*),0));});});
return({struct _tuple17 _tmp569;({union Cyc_CfFlowInfo_FlowInfo _tmp65C=Cyc_CfFlowInfo_BottomFL();_tmp569.f1=_tmp65C;}),_tmp569.f2=fenv->unknown_all;_tmp569;});}}};}
# 1629
f1=f2;
goto _LL1A5;}}}default: _LL1A5:
# 1632
 while(1){
struct Cyc_List_List _tmp289=({struct Cyc_List_List _tmp56C;_tmp56C.hd=vd,_tmp56C.tl=0;_tmp56C;});struct Cyc_List_List l=_tmp289;
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
struct _tuple17 _tmp28A=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);struct _tuple17 _stmttmp54=_tmp28A;struct _tuple17 _tmp28B=_stmttmp54;void*_tmp28D;union Cyc_CfFlowInfo_FlowInfo _tmp28C;_tmp28C=_tmp28B.f1;_tmp28D=_tmp28B.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp28C;void*r2=_tmp28D;
{union Cyc_CfFlowInfo_FlowInfo _tmp28E=f2;struct Cyc_Dict_Dict _tmp28F;if((_tmp28E.BottomFL).tag == 1)
goto _LL1B1;else{_tmp28F=((_tmp28E.ReachableFL).val).f1;{struct Cyc_Dict_Dict d2=_tmp28F;
# 1639
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp290=0U;({unsigned _tmp65E=e1->loc;struct _fat_ptr _tmp65D=({const char*_tmp291="expression may not be initialized";_tag_fat(_tmp291,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65E,_tmp65D,_tag_fat(_tmp290,sizeof(void*),0));});});
return({struct _tuple17 _tmp56B;({union Cyc_CfFlowInfo_FlowInfo _tmp65F=Cyc_CfFlowInfo_BottomFL();_tmp56B.f1=_tmp65F;}),_tmp56B.f2=fenv->unknown_all;_tmp56B;});}}}_LL1B1:;}{
# 1644
union Cyc_CfFlowInfo_FlowInfo _tmp292=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);union Cyc_CfFlowInfo_FlowInfo newflow=_tmp292;
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,f1))
break;
f1=newflow;}}}}
# 1649
return({struct _tuple17 _tmp56D;_tmp56D.f1=f1,_tmp56D.f2=fenv->notzeroall;_tmp56D;});};}}}};}}case 28: _tmp11A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp119)->f2;_tmp11C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*exp=_tmp11A;void*typ=_tmp11B;int iszeroterm=_tmp11C;
# 1655
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->tag=1,_tmp29B->f1=exp,_tmp29B->f2=(void*)_check_null(e->topt);_tmp29B;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->root=root,_tmp29A->path=0;_tmp29A;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->tag=4,_tmp299->f1=place;_tmp299;});
void*place_val;
# 1663
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,typ,0,fenv->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple17 _tmp293=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);struct _tuple17 _stmttmp55=_tmp293;struct _tuple17 _tmp294=_stmttmp55;union Cyc_CfFlowInfo_FlowInfo _tmp295;_tmp295=_tmp294.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp295;
outflow=f;{
union Cyc_CfFlowInfo_FlowInfo _tmp296=outflow;void*_tmp298;struct Cyc_Dict_Dict _tmp297;if((_tmp296.BottomFL).tag == 1)
return({struct _tuple17 _tmp56E;_tmp56E.f1=outflow,_tmp56E.f2=rval;_tmp56E;});else{_tmp297=((_tmp296.ReachableFL).val).f1;_tmp298=((_tmp296.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp297;struct Cyc_List_List*relns=_tmp298;
# 1670
return({struct _tuple17 _tmp56F;({union Cyc_CfFlowInfo_FlowInfo _tmp661=({struct Cyc_Dict_Dict _tmp660=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp660,relns);});_tmp56F.f1=_tmp661;}),_tmp56F.f2=rval;_tmp56F;});}};}}}}case 37: _tmp11A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Stmt*s=_tmp11A;
# 1674
struct _tuple18 _tmp29C=({struct _tuple18 _tmp571;_tmp571.f1=(env->fenv)->unknown_all,_tmp571.f2=copy_ctxt;_tmp571;});struct _tuple18 rval_opt=_tmp29C;
union Cyc_CfFlowInfo_FlowInfo _tmp29D=Cyc_NewControlFlow_anal_stmt(env,inflow,s,& rval_opt);union Cyc_CfFlowInfo_FlowInfo f=_tmp29D;
return({struct _tuple17 _tmp570;_tmp570.f1=f,_tmp570.f2=rval_opt.f1;_tmp570;});}case 36: _LL7B:
# 1679
 goto _LL7D;case 25: _LL7D:
 goto _LL7F;default: _LL7F:
# 1682
({void*_tmp29E=0U;({struct _fat_ptr _tmp662=({const char*_tmp29F="anal_Rexp, unexpected exp form";_tag_fat(_tmp29F,sizeof(char),31U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp662,_tag_fat(_tmp29E,sizeof(void*),0));});});};}}
# 1694 "new_control_flow.cyc"
static struct _tuple19 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1703
struct Cyc_CfFlowInfo_FlowEnv*_tmp2A0=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp2A0;
void*_tmp2A1=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp56=_tmp2A1;void*_tmp2A2=_stmttmp56;void*_tmp2A3;if(*((int*)_tmp2A2)== 3){_tmp2A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A2)->f1).ptr_atts).bounds;{void*bd=_tmp2A3;
# 1706
union Cyc_CfFlowInfo_FlowInfo _tmp2A4=f;void*_tmp2A6;struct Cyc_Dict_Dict _tmp2A5;if((_tmp2A4.BottomFL).tag == 1)
return({struct _tuple19 _tmp572;_tmp572.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp663=Cyc_CfFlowInfo_UnknownL();_tmp572.f2=_tmp663;});_tmp572;});else{_tmp2A5=((_tmp2A4.ReachableFL).val).f1;_tmp2A6=((_tmp2A4.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp2A5;struct Cyc_List_List*relns=_tmp2A6;
# 1710
struct _tuple16 _tmp2A7=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp57=_tmp2A7;struct _tuple16 _tmp2A8=_stmttmp57;void*_tmp2AA;void*_tmp2A9;_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;{void*r=_tmp2A9;struct Cyc_List_List*names=_tmp2AA;
retry: {void*_tmp2AB=r;enum Cyc_CfFlowInfo_InitLevel _tmp2AD;void*_tmp2AE;void*_tmp2AC;switch(*((int*)_tmp2AB)){case 8:
# 1713
({void*_tmp2AF=0U;({struct _fat_ptr _tmp664=({const char*_tmp2B0="named location in anal_derefL";_tag_fat(_tmp2B0,sizeof(char),30U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp664,_tag_fat(_tmp2AF,sizeof(void*),0));});});case 1:
# 1715
({void*_tmp665=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=Cyc_CfFlowInfo_NotZero,_tmp2B1->f1=relns;_tmp2B1;});e->annot=_tmp665;});goto _LLD;case 5: _tmp2AC=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->f1;{void*rv=_tmp2AC;
# 1718
if(expanded_place_possibly_null)
# 1721
({void*_tmp666=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2B2->f1=relns;_tmp2B2;});e->annot=_tmp666;});else{
# 1724
void*_tmp2B3=e->annot;void*_stmttmp58=_tmp2B3;void*_tmp2B4=_stmttmp58;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_CfFlowInfo_UnknownZ){
# 1728
({void*_tmp667=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2B5->f1=relns;_tmp2B5;});e->annot=_tmp667;});
goto _LL1E;}else{
# 1731
({void*_tmp668=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->tag=Cyc_CfFlowInfo_NotZero,_tmp2B6->f1=relns;_tmp2B6;});e->annot=_tmp668;});
goto _LL1E;}_LL1E:;}
# 1736
if(Cyc_Tcutil_is_bound_one(bd))return({struct _tuple19 _tmp573;_tmp573.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp669=Cyc_CfFlowInfo_UnknownL();_tmp573.f2=_tmp669;});_tmp573;});
goto _LLD;}case 4: _tmp2AC=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->f1)->root;_tmp2AE=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->f1)->path;{void*root=_tmp2AC;struct Cyc_List_List*path2=_tmp2AE;
# 1740
({void*_tmp66A=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->tag=Cyc_CfFlowInfo_NotZero,_tmp2B7->f1=relns;_tmp2B7;});e->annot=_tmp66A;});
if(Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple19 _tmp574;_tmp574.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp66C=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->root=root,({struct Cyc_List_List*_tmp66B=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(path2,path);_tmp2B8->path=_tmp66B;});_tmp2B8;}));_tmp574.f2=_tmp66C;});_tmp574;});
goto _LLD;}case 0:
# 1745
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple19 _tmp575;({union Cyc_CfFlowInfo_FlowInfo _tmp66E=Cyc_CfFlowInfo_BottomFL();_tmp575.f1=_tmp66E;}),({union Cyc_CfFlowInfo_AbsLVal _tmp66D=Cyc_CfFlowInfo_UnknownL();_tmp575.f2=_tmp66D;});_tmp575;});case 2: _tmp2AD=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp2AD;
# 1750
if(Cyc_Tcutil_is_bound_one(bd))
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
({void*_tmp66F=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2B9->f1=relns;_tmp2B9;});e->annot=_tmp66F;});
goto _LLD;}case 7: _tmp2AC=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2AB)->f3;{void*r2=_tmp2AC;
# 1755
if(passthrough_consumes){
r=r2;goto retry;}
# 1758
goto _LL1D;}default: _LL1D:
# 1760
({void*_tmp670=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2BA->f1=relns;_tmp2BA;});e->annot=_tmp670;});
goto _LLD;}_LLD:;}
# 1763
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,r)== (int)Cyc_CfFlowInfo_NoneIL){
struct _tuple16 _tmp2BB=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp59=_tmp2BB;struct _tuple16 _tmp2BC=_stmttmp59;void*_tmp2BD;_tmp2BD=_tmp2BC.f1;{void*r=_tmp2BD;
void*_tmp2BE=r;if(*((int*)_tmp2BE)== 7){
# 1767
({void*_tmp2BF=0U;({unsigned _tmp672=e->loc;struct _fat_ptr _tmp671=({const char*_tmp2C0="attempt to dereference an alias-free that has already been copied";_tag_fat(_tmp2C0,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp672,_tmp671,_tag_fat(_tmp2BF,sizeof(void*),0));});});
goto _LL26;}else{
# 1770
({void*_tmp2C1=0U;({unsigned _tmp674=e->loc;struct _fat_ptr _tmp673=({const char*_tmp2C2="dereference of possibly uninitialized pointer";_tag_fat(_tmp2C2,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp674,_tmp673,_tag_fat(_tmp2C1,sizeof(void*),0));});});
goto _LL26;}_LL26:;}}
# 1774
return({struct _tuple19 _tmp576;_tmp576.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp675=Cyc_CfFlowInfo_UnknownL();_tmp576.f2=_tmp675;});_tmp576;});}}};}}else{
# 1776
({void*_tmp2C3=0U;({struct _fat_ptr _tmp676=({const char*_tmp2C4="left deref of non-pointer-type";_tag_fat(_tmp2C4,sizeof(char),31U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp676,_tag_fat(_tmp2C3,sizeof(void*),0));});});};}
# 1782
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp2C5=p;void*_tmp2C7;void*_tmp2C6;_tmp2C6=_tmp2C5->root;_tmp2C7=_tmp2C5->path;{void*proot=_tmp2C6;struct Cyc_List_List*ppath=_tmp2C7;
path=({struct Cyc_List_List*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({void*_tmp677=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->tag=1;_tmp2C8;});_tmp2C9->hd=_tmp677;}),_tmp2C9->tl=path;_tmp2C9;});
return({struct Cyc_CfFlowInfo_Place*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->root=proot,({struct Cyc_List_List*_tmp678=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ppath,path);_tmp2CA->path=_tmp678;});_tmp2CA;});}}
# 1792
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1796
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp2CB=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp2CB;
{union Cyc_CfFlowInfo_FlowInfo _tmp2CC=inflow;void*_tmp2CE;struct Cyc_Dict_Dict _tmp2CD;if((_tmp2CC.BottomFL).tag == 1)
return({struct _tuple19 _tmp577;({union Cyc_CfFlowInfo_FlowInfo _tmp67A=Cyc_CfFlowInfo_BottomFL();_tmp577.f1=_tmp67A;}),({union Cyc_CfFlowInfo_AbsLVal _tmp679=Cyc_CfFlowInfo_UnknownL();_tmp577.f2=_tmp679;});_tmp577;});else{_tmp2CD=((_tmp2CC.ReachableFL).val).f1;_tmp2CE=((_tmp2CC.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp2CD;struct Cyc_List_List*relns=_tmp2CE;
# 1801
d=d2;}};}{
# 1805
void*_tmp2CF=e->r;void*_stmttmp5A=_tmp2CF;void*_tmp2D0=_stmttmp5A;void*_tmp2D2;void*_tmp2D1;switch(*((int*)_tmp2D0)){case 14: _tmp2D1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D0)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D1;
_tmp2D1=e1;goto _LL9;}case 12: _tmp2D1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp2D1;
_tmp2D1=e1;goto _LLB;}case 13: _tmp2D1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_LLB: {struct Cyc_Absyn_Exp*e1=_tmp2D1;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e1,path);}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1)){case 4: _tmp2D1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp2D1;
# 1811
if((int)vd->sc == (int)Cyc_Absyn_Static)
return({struct _tuple19 _tmp578;_tmp578.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp67B=Cyc_CfFlowInfo_UnknownL();_tmp578.f2=_tmp67B;});_tmp578;});
_tmp2D1=vd;goto _LLF;}case 3: _tmp2D1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1)->f1;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp2D1;
_tmp2D1=vd;goto _LL11;}case 5: _tmp2D1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1)->f1;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp2D1;
# 1816
return({struct _tuple19 _tmp579;_tmp579.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp67D=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));({void*_tmp67C=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->tag=0,_tmp2D3->f1=vd;_tmp2D3;});_tmp2D4->root=_tmp67C;}),_tmp2D4->path=path;_tmp2D4;}));_tmp579.f2=_tmp67D;});_tmp579;});}case 1: _tmp2D1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp2D1;
# 1818
return({struct _tuple19 _tmp57A;_tmp57A.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp67E=Cyc_CfFlowInfo_UnknownL();_tmp57A.f2=_tmp67E;});_tmp57A;});}default: goto _LL1C;}case 22: _tmp2D1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_tmp2D2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D0)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D1;struct _fat_ptr*f=_tmp2D2;
# 1821
{void*_tmp2D5=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp5B=_tmp2D5;void*_tmp2D6=_stmttmp5B;void*_tmp2D7;if(*((int*)_tmp2D6)== 3){_tmp2D7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D6)->f1).elt_type;{void*t2=_tmp2D7;
# 1823
if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(t2)){
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,f,inflow);
path=({struct Cyc_List_List*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));({void*_tmp680=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->tag=0,({int _tmp67F=Cyc_CfFlowInfo_get_field_index(t2,f);_tmp2D8->f1=_tmp67F;});_tmp2D8;});_tmp2D9->hd=_tmp680;}),_tmp2D9->tl=path;_tmp2D9;});}
# 1827
goto _LL1E;}}else{
({void*_tmp2DA=0U;({struct _fat_ptr _tmp681=({const char*_tmp2DB="anal_Lexp: AggrArrow ptr";_tag_fat(_tmp2DB,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp681,_tag_fat(_tmp2DA,sizeof(void*),0));});});}_LL1E:;}
# 1830
_tmp2D1=e1;goto _LL17;}case 20: _tmp2D1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp2D1;
# 1834
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)){
# 1836
struct _tuple19 _tmp2DC=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,e1);struct _tuple19 _stmttmp5C=_tmp2DC;struct _tuple19 _tmp2DD=_stmttmp5C;union Cyc_CfFlowInfo_AbsLVal _tmp2DF;union Cyc_CfFlowInfo_FlowInfo _tmp2DE;_tmp2DE=_tmp2DD.f1;_tmp2DF=_tmp2DD.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2DE;union Cyc_CfFlowInfo_AbsLVal lval=_tmp2DF;
struct _tuple19 _tmp2E0=({struct _tuple19 _tmp57E;_tmp57E.f1=f,_tmp57E.f2=lval;_tmp57E;});struct _tuple19 _stmttmp5D=_tmp2E0;struct _tuple19 _tmp2E1=_stmttmp5D;void*_tmp2E4;void*_tmp2E3;struct Cyc_Dict_Dict _tmp2E2;if(((_tmp2E1.f1).ReachableFL).tag == 2){if(((_tmp2E1.f2).PlaceL).tag == 1){_tmp2E2=(((_tmp2E1.f1).ReachableFL).val).f1;_tmp2E3=(((_tmp2E1.f1).ReachableFL).val).f2;_tmp2E4=((_tmp2E1.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_tmp2E2;struct Cyc_List_List*r=_tmp2E3;struct Cyc_CfFlowInfo_Place*p=_tmp2E4;
# 1839
void*_tmp2E5=Cyc_CfFlowInfo_lookup_place(fd,p);void*old_rval=_tmp2E5;
struct _tuple16 _tmp2E6=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _stmttmp5E=_tmp2E6;struct _tuple16 _tmp2E7=_stmttmp5E;void*_tmp2E9;void*_tmp2E8;_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;{void*rval=_tmp2E8;struct Cyc_List_List*names=_tmp2E9;
if(expand_unique){
int _tmp2EA=1;int possibly_null=_tmp2EA;
void*_tmp2EB=rval;void*_tmp2ED;void*_tmp2EC;switch(*((int*)_tmp2EB)){case 8: _tmp2EC=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2EB)->f1;_tmp2ED=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2EB)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp2EC;void*r=_tmp2ED;
# 1845
({void*_tmp2EE=0U;({struct _fat_ptr _tmp682=({const char*_tmp2EF="bad named location in anal_Lexp:deref";_tag_fat(_tmp2EF,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp682,_tag_fat(_tmp2EE,sizeof(void*),0));});});}case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2EB)->f3)){case 4:
 goto _LL34;case 5:
# 1849
 goto _LL38;default: goto _LL3B;}case 4: _LL34:
# 1848
 return Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);case 5: _LL38: {
# 1853
struct _tuple19 _tmp2F0=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _stmttmp5F=_tmp2F0;struct _tuple19 _tmp2F1=_stmttmp5F;union Cyc_CfFlowInfo_FlowInfo _tmp2F2;_tmp2F2=_tmp2F1.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2F2;
return({struct _tuple19 _tmp57B;_tmp57B.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp683=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp57B.f2=_tmp683;});_tmp57B;});}}case 1:
# 1856
 possibly_null=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1863
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(fenv,fd,rval);
void*leaf=(int)il == (int)1U?fenv->unknown_all: fenv->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->tag=5,({void*_tmp686=({struct Cyc_CfFlowInfo_FlowEnv*_tmp685=fenv;void*_tmp684=Cyc_Tcutil_pointer_elt_type((void*)_check_null(e1->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp685,_tmp684,0,leaf);});_tmp2F8->f1=_tmp686;});_tmp2F8;});
for(1;names != 0;names=names->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->tag=8,_tmp2F3->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp2F3->f2=res;_tmp2F3;});}
fd=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp2F4=Cyc_CfFlowInfo_ReachableFL(fd,r);union Cyc_CfFlowInfo_FlowInfo outflow=_tmp2F4;
# 1876
struct _tuple19 _tmp2F5=Cyc_NewControlFlow_anal_derefL(env,outflow,e1,outflow,res,passthrough_consumes,possibly_null,path);struct _tuple19 _stmttmp60=_tmp2F5;struct _tuple19 _tmp2F6=_stmttmp60;union Cyc_CfFlowInfo_FlowInfo _tmp2F7;_tmp2F7=_tmp2F6.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2F7;
# 1878
return({struct _tuple19 _tmp57C;_tmp57C.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp687=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp57C.f2=_tmp687;});_tmp57C;});}}}};}else{
# 1882
void*_tmp2F9=rval;switch(*((int*)_tmp2F9)){case 7: if(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2F9)->f3)== 5)
goto _LL47;else{goto _LL48;}case 5: _LL47: {
# 1887
struct _tuple19 _tmp2FA=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _stmttmp61=_tmp2FA;struct _tuple19 _tmp2FB=_stmttmp61;union Cyc_CfFlowInfo_FlowInfo _tmp2FC;_tmp2FC=_tmp2FB.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2FC;
return({struct _tuple19 _tmp57D;_tmp57D.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp688=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp57D.f2=_tmp688;});_tmp57D;});}}default: _LL48:
# 1891
 goto _LL43;}_LL43:;}
# 1894
goto _LL26;}}}else{goto _LL29;}}else{_LL29:
 goto _LL26;}_LL26:;}}{
# 1899
struct _tuple17 _tmp2FD=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);struct _tuple17 _stmttmp62=_tmp2FD;struct _tuple17 _tmp2FE=_stmttmp62;void*_tmp300;union Cyc_CfFlowInfo_FlowInfo _tmp2FF;_tmp2FF=_tmp2FE.f1;_tmp300=_tmp2FE.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2FF;void*r=_tmp300;
# 1901
return Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,r,passthrough_consumes,0,path);}}}case 23: _tmp2D1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_tmp2D2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D0)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D1;struct Cyc_Absyn_Exp*e2=_tmp2D2;
# 1904
void*_tmp301=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp63=_tmp301;void*_tmp302=_stmttmp63;switch(*((int*)_tmp302)){case 6:  {
# 1906
unsigned _tmp303=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;unsigned fld=_tmp303;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp68E=env;union Cyc_CfFlowInfo_FlowInfo _tmp68D=inflow;int _tmp68C=expand_unique;int _tmp68B=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp68A=e1;Cyc_NewControlFlow_anal_Lexp_rec(_tmp68E,_tmp68D,_tmp68C,_tmp68B,_tmp68A,({struct Cyc_List_List*_tmp305=_cycalloc(sizeof(*_tmp305));
({void*_tmp689=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->tag=0,_tmp304->f1=(int)fld;_tmp304;});_tmp305->hd=_tmp689;}),_tmp305->tl=path;_tmp305;}));});}case 3:  {
# 1910
struct _tuple24 _tmp306=({struct Cyc_NewControlFlow_AnalEnv*_tmp690=env;union Cyc_CfFlowInfo_FlowInfo _tmp68F=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp690,_tmp68F,({struct Cyc_Absyn_Exp*_tmp316[2];_tmp316[0]=e1,_tmp316[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp316,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple24 _stmttmp64=_tmp306;struct _tuple24 _tmp307=_stmttmp64;void*_tmp309;union Cyc_CfFlowInfo_FlowInfo _tmp308;_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp308;struct Cyc_List_List*rvals=_tmp309;
union Cyc_CfFlowInfo_FlowInfo _tmp30A=f;union Cyc_CfFlowInfo_FlowInfo f2=_tmp30A;
{union Cyc_CfFlowInfo_FlowInfo _tmp30B=f;void*_tmp30D;struct Cyc_Dict_Dict _tmp30C;if((_tmp30B.ReachableFL).tag == 2){_tmp30C=((_tmp30B.ReachableFL).val).f1;_tmp30D=((_tmp30B.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp30C;struct Cyc_List_List*relns=_tmp30D;
# 1914
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp30E=0U;({unsigned _tmp692=e2->loc;struct _fat_ptr _tmp691=({const char*_tmp30F="expression may not be initialized";_tag_fat(_tmp30F,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp692,_tmp691,_tag_fat(_tmp30E,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp310=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);struct Cyc_List_List*new_relns=_tmp310;
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL5A;}}}else{
goto _LL5A;}_LL5A:;}{
# 1923
struct _tuple19 _tmp311=Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,(void*)((struct Cyc_List_List*)_check_null(rvals))->hd,passthrough_consumes,0,path);struct _tuple19 _stmttmp65=_tmp311;struct _tuple19 _tmp312=_stmttmp65;union Cyc_CfFlowInfo_AbsLVal _tmp314;union Cyc_CfFlowInfo_FlowInfo _tmp313;_tmp313=_tmp312.f1;_tmp314=_tmp312.f2;{union Cyc_CfFlowInfo_FlowInfo f3=_tmp313;union Cyc_CfFlowInfo_AbsLVal r=_tmp314;
union Cyc_CfFlowInfo_FlowInfo _tmp315=f3;if((_tmp315.BottomFL).tag == 1)
return({struct _tuple19 _tmp57F;_tmp57F.f1=f3,_tmp57F.f2=r;_tmp57F;});else{
return({struct _tuple19 _tmp580;_tmp580.f1=f2,_tmp580.f2=r;_tmp580;});};}}}}default:
# 1928
({void*_tmp317=0U;({struct _fat_ptr _tmp693=({const char*_tmp318="anal_Lexp: Subscript -- bad type";_tag_fat(_tmp318,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp693,_tag_fat(_tmp317,sizeof(void*),0));});});};}case 21: _tmp2D1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_tmp2D2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D0)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D1;struct _fat_ptr*fld=_tmp2D2;
# 1932
void*_tmp319=(void*)_check_null(e1->topt);void*e1_type=_tmp319;
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,fld,inflow);
# 1936
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple19 _tmp581;_tmp581.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp694=Cyc_CfFlowInfo_UnknownL();_tmp581.f2=_tmp694;});_tmp581;});
# 1939
return({struct Cyc_NewControlFlow_AnalEnv*_tmp69B=env;union Cyc_CfFlowInfo_FlowInfo _tmp69A=inflow;int _tmp699=expand_unique;int _tmp698=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp697=e1;Cyc_NewControlFlow_anal_Lexp_rec(_tmp69B,_tmp69A,_tmp699,_tmp698,_tmp697,({struct Cyc_List_List*_tmp31B=_cycalloc(sizeof(*_tmp31B));
({void*_tmp696=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A->tag=0,({int _tmp695=Cyc_CfFlowInfo_get_field_index(e1_type,fld);_tmp31A->f1=_tmp695;});_tmp31A;});_tmp31B->hd=_tmp696;}),_tmp31B->tl=path;_tmp31B;}));});}default: _LL1C:
# 1943
 return({struct _tuple19 _tmp582;({union Cyc_CfFlowInfo_FlowInfo _tmp69D=Cyc_CfFlowInfo_BottomFL();_tmp582.f1=_tmp69D;}),({union Cyc_CfFlowInfo_AbsLVal _tmp69C=Cyc_CfFlowInfo_UnknownL();_tmp582.f2=_tmp69C;});_tmp582;});};}}
# 1947
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1951
struct _tuple19 _tmp31C=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple19 _stmttmp66=_tmp31C;struct _tuple19 _tmp31D=_stmttmp66;union Cyc_CfFlowInfo_AbsLVal _tmp31F;union Cyc_CfFlowInfo_FlowInfo _tmp31E;_tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp31E;union Cyc_CfFlowInfo_AbsLVal r=_tmp31F;
return({struct _tuple19 _tmp583;_tmp583.f1=f,_tmp583.f2=r;_tmp583;});}}
# 1958
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1961
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 1965
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
# 1968
struct _tuple19 _tmp320=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple19 _stmttmp67=_tmp320;struct _tuple19 _tmp321=_stmttmp67;union Cyc_CfFlowInfo_FlowInfo _tmp322;_tmp322=_tmp321.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp322;
inflow=f;}}}}
# 1973
return inflow;}struct _tuple28{enum Cyc_Absyn_Primop f1;void*f2;void*f3;};
# 1979
static struct _tuple20 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1981
struct Cyc_CfFlowInfo_FlowEnv*_tmp323=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp323;
# 1984
struct _tuple17 _tmp324=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,0);struct _tuple17 _stmttmp68=_tmp324;struct _tuple17 _tmp325=_stmttmp68;void*_tmp327;union Cyc_CfFlowInfo_FlowInfo _tmp326;_tmp326=_tmp325.f1;_tmp327=_tmp325.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp326;void*r1=_tmp327;
struct _tuple17 _tmp328=Cyc_NewControlFlow_anal_Rexp(env,0,f1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd,0);struct _tuple17 _stmttmp69=_tmp328;struct _tuple17 _tmp329=_stmttmp69;void*_tmp32B;union Cyc_CfFlowInfo_FlowInfo _tmp32A;_tmp32A=_tmp329.f1;_tmp32B=_tmp329.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp32A;void*r2=_tmp32B;
union Cyc_CfFlowInfo_FlowInfo _tmp32C=f2;union Cyc_CfFlowInfo_FlowInfo f=_tmp32C;
# 1989
union Cyc_CfFlowInfo_FlowInfo _tmp32D=f;void*_tmp32F;struct Cyc_Dict_Dict _tmp32E;if((_tmp32D.BottomFL).tag == 1)
return({struct _tuple20 _tmp584;_tmp584.f1=f,_tmp584.f2=f;_tmp584;});else{_tmp32E=((_tmp32D.ReachableFL).val).f1;_tmp32F=((_tmp32D.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp32E;struct Cyc_List_List*relns=_tmp32F;
# 1992
struct Cyc_Absyn_Exp*_tmp330=(struct Cyc_Absyn_Exp*)es->hd;struct Cyc_Absyn_Exp*e1=_tmp330;
struct Cyc_Absyn_Exp*_tmp331=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp331;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r1)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp332=0U;({unsigned _tmp69F=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _fat_ptr _tmp69E=({const char*_tmp333="expression may not be initialized";_tag_fat(_tmp333,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp69F,_tmp69E,_tag_fat(_tmp332,sizeof(void*),0));});});
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r2)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp334=0U;({unsigned _tmp6A1=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _fat_ptr _tmp6A0=({const char*_tmp335="expression may not be initialized";_tag_fat(_tmp335,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6A1,_tmp6A0,_tag_fat(_tmp334,sizeof(void*),0));});});{
# 1999
union Cyc_CfFlowInfo_FlowInfo _tmp336=f;union Cyc_CfFlowInfo_FlowInfo ft=_tmp336;
union Cyc_CfFlowInfo_FlowInfo _tmp337=f;union Cyc_CfFlowInfo_FlowInfo ff=_tmp337;
# 2004
if((int)p == 5 ||(int)p == 6){
struct _tuple16 _tmp338=Cyc_CfFlowInfo_unname_rval(r1);struct _tuple16 _stmttmp6A=_tmp338;struct _tuple16 _tmp339=_stmttmp6A;void*_tmp33B;void*_tmp33A;_tmp33A=_tmp339.f1;_tmp33B=_tmp339.f2;{void*r1=_tmp33A;struct Cyc_List_List*r1n=_tmp33B;
struct _tuple16 _tmp33C=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _stmttmp6B=_tmp33C;struct _tuple16 _tmp33D=_stmttmp6B;void*_tmp33F;void*_tmp33E;_tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;{void*r2=_tmp33E;struct Cyc_List_List*r2n=_tmp33F;
struct _tuple0 _tmp340=({struct _tuple0 _tmp585;_tmp585.f1=r1,_tmp585.f2=r2;_tmp585;});struct _tuple0 _stmttmp6C=_tmp340;struct _tuple0 _tmp341=_stmttmp6C;enum Cyc_CfFlowInfo_InitLevel _tmp342;switch(*((int*)_tmp341.f1)){case 2: if(*((int*)_tmp341.f2)== 0){_tmp342=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp341.f1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp342;
# 2009
struct _tuple20 _tmp343=Cyc_NewControlFlow_splitzero(env,inflow,f,e1,il,r1n);struct _tuple20 _stmttmp6D=_tmp343;struct _tuple20 _tmp344=_stmttmp6D;union Cyc_CfFlowInfo_FlowInfo _tmp346;union Cyc_CfFlowInfo_FlowInfo _tmp345;_tmp345=_tmp344.f1;_tmp346=_tmp344.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp345;union Cyc_CfFlowInfo_FlowInfo f2=_tmp346;
{enum Cyc_Absyn_Primop _tmp347=p;switch((int)_tmp347){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL25;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL25;default:
({void*_tmp348=0U;({struct _fat_ptr _tmp6A2=({const char*_tmp349="anal_test, zero-split";_tag_fat(_tmp349,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6A2,_tag_fat(_tmp348,sizeof(void*),0));});});}_LL25:;}
# 2015
goto _LL11;}}}else{goto _LL20;}case 0: switch(*((int*)_tmp341.f2)){case 2: _tmp342=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp341.f2)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp342;
# 2017
struct _tuple20 _tmp34A=Cyc_NewControlFlow_splitzero(env,f2,f,e2,il,r2n);struct _tuple20 _stmttmp6E=_tmp34A;struct _tuple20 _tmp34B=_stmttmp6E;union Cyc_CfFlowInfo_FlowInfo _tmp34D;union Cyc_CfFlowInfo_FlowInfo _tmp34C;_tmp34C=_tmp34B.f1;_tmp34D=_tmp34B.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp34C;union Cyc_CfFlowInfo_FlowInfo f2=_tmp34D;
{enum Cyc_Absyn_Primop _tmp34E=p;switch((int)_tmp34E){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL2F;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL2F;default:
({void*_tmp34F=0U;({struct _fat_ptr _tmp6A3=({const char*_tmp350="anal_test, zero-split";_tag_fat(_tmp350,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6A3,_tag_fat(_tmp34F,sizeof(void*),0));});});}_LL2F:;}
# 2023
goto _LL11;}}case 0:
# 2025
 if((int)p == 5)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;case 1:
 goto _LL1B;case 4: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1: if(*((int*)_tmp341.f2)== 0){_LL1D:
 goto _LL1F;}else{goto _LL20;}case 4: if(*((int*)_tmp341.f2)== 0){_LL1F:
# 2032
 if((int)p == 6)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;}else{goto _LL20;}default: _LL20:
 goto _LL11;}_LL11:;}}}
# 2044
{struct _tuple28 _tmp351=({struct _tuple28 _tmp587;_tmp587.f1=p,({
void*_tmp6A5=Cyc_Absyn_compress((void*)_check_null(e1->topt));_tmp587.f2=_tmp6A5;}),({
void*_tmp6A4=Cyc_Absyn_compress((void*)_check_null(e2->topt));_tmp587.f3=_tmp6A4;});_tmp587;});
# 2044
struct _tuple28 _stmttmp6F=_tmp351;struct _tuple28 _tmp352=_stmttmp6F;switch((int)_tmp352.f1){case Cyc_Absyn_Eq:
# 2047
 goto _LL3A;case Cyc_Absyn_Neq: _LL3A:
 goto _LL3C;default: if(*((int*)_tmp352.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3C:
 goto _LL3E;}else{if(*((int*)_tmp352.f3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{goto _LL43;}case 4: goto _LL41;default: goto _LL43;}else{goto _LL43;}}}else{if(*((int*)_tmp352.f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f2)->f1)== 4)goto _LL3F;else{goto _LL43;}}}else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f2)->f1)== 4)goto _LL3F;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)== 4)goto _LL41;else{goto _LL43;}}}}else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f2)->f1)== 4){_LL3F: _LL40:
# 2051
 goto _LL42;}else{goto _LL43;}}}}else{if(*((int*)_tmp352.f3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352.f3)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3D: _LL3E:
# 2050
 goto _LL40;}else{goto _LL43;}case 4: _LL41: _LL42:
# 2052
 goto _LL36;default: goto _LL43;}else{_LL43:
 return({struct _tuple20 _tmp586;_tmp586.f1=ft,_tmp586.f2=ff;_tmp586;});}}}_LL36:;}
# 2057
{enum Cyc_Absyn_Primop _tmp353=p;switch((int)_tmp353){case Cyc_Absyn_Eq:
 goto _LL49;case Cyc_Absyn_Neq: _LL49: goto _LL4B;case Cyc_Absyn_Gt: _LL4B: goto _LL4D;case Cyc_Absyn_Gte: _LL4D: goto _LL4F;case Cyc_Absyn_Lt: _LL4F: goto _LL51;case Cyc_Absyn_Lte: _LL51: goto _LL45;default:
 return({struct _tuple20 _tmp588;_tmp588.f1=ft,_tmp588.f2=ff;_tmp588;});}_LL45:;}{
# 2062
struct _RegionHandle*_tmp354=Cyc_Core_heap_region;struct _RegionHandle*r=_tmp354;
struct _tuple13 _tmp355=Cyc_Relations_primop2relation(e1,p,e2);struct _tuple13 _stmttmp70=_tmp355;struct _tuple13 _tmp356=_stmttmp70;void*_tmp359;enum Cyc_Relations_Relation _tmp358;void*_tmp357;_tmp357=_tmp356.f1;_tmp358=_tmp356.f2;_tmp359=_tmp356.f3;{struct Cyc_Absyn_Exp*e1=_tmp357;enum Cyc_Relations_Relation relation=_tmp358;struct Cyc_Absyn_Exp*e2=_tmp359;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2067
if(Cyc_Relations_exp2relnop(e1,& n1)&& Cyc_Relations_exp2relnop(e2,& n2)){
# 2069
struct Cyc_List_List*_tmp35A=Cyc_Relations_add_relation(r,n1,relation,n2,relns);struct Cyc_List_List*relns_true=_tmp35A;
# 2073
struct Cyc_List_List*_tmp35B=({struct _RegionHandle*_tmp6A9=r;union Cyc_Relations_RelnOp _tmp6A8=n2;enum Cyc_Relations_Relation _tmp6A7=Cyc_Relations_flip_relation(relation);union Cyc_Relations_RelnOp _tmp6A6=n1;Cyc_Relations_add_relation(_tmp6A9,_tmp6A8,_tmp6A7,_tmp6A6,relns);});struct Cyc_List_List*relns_false=_tmp35B;
struct _tuple20 _tmp35C=({struct _tuple20 _tmp58D;_tmp58D.f1=ft,_tmp58D.f2=ff;_tmp58D;});struct _tuple20 _stmttmp71=_tmp35C;struct _tuple20 _tmp35D=_stmttmp71;struct Cyc_Dict_Dict _tmp35F;struct Cyc_Dict_Dict _tmp35E;if(((_tmp35D.f1).ReachableFL).tag == 2){if(((_tmp35D.f2).ReachableFL).tag == 2){_tmp35E=(((_tmp35D.f1).ReachableFL).val).f1;_tmp35F=(((_tmp35D.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict dt=_tmp35E;struct Cyc_Dict_Dict df=_tmp35F;
# 2076
return({struct _tuple20 _tmp589;({union Cyc_CfFlowInfo_FlowInfo _tmp6AB=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_tmp589.f1=_tmp6AB;}),({union Cyc_CfFlowInfo_FlowInfo _tmp6AA=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_tmp589.f2=_tmp6AA;});_tmp589;});}}else{_tmp35E=(((_tmp35D.f1).ReachableFL).val).f1;{struct Cyc_Dict_Dict dt=_tmp35E;
# 2080
return({struct _tuple20 _tmp58A;({union Cyc_CfFlowInfo_FlowInfo _tmp6AC=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_tmp58A.f1=_tmp6AC;}),_tmp58A.f2=ff;_tmp58A;});}}}else{if(((_tmp35D.f2).ReachableFL).tag == 2){_tmp35E=(((_tmp35D.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict df=_tmp35E;
# 2078
return({struct _tuple20 _tmp58B;_tmp58B.f1=ft,({union Cyc_CfFlowInfo_FlowInfo _tmp6AD=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_tmp58B.f2=_tmp6AD;});_tmp58B;});}}else{
# 2082
return({struct _tuple20 _tmp58C;_tmp58C.f1=ft,_tmp58C.f2=ff;_tmp58C;});}};}else{
# 2085
return({struct _tuple20 _tmp58E;_tmp58E.f1=ft,_tmp58E.f2=ff;_tmp58E;});}}}}}};}}}
# 2091
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp360=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp360;
void*_tmp361=e->r;void*_stmttmp72=_tmp361;void*_tmp362=_stmttmp72;enum Cyc_Absyn_Primop _tmp363;void*_tmp366;void*_tmp365;void*_tmp364;switch(*((int*)_tmp362)){case 6: _tmp364=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp362)->f1;_tmp365=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp362)->f2;_tmp366=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp362)->f3;{struct Cyc_Absyn_Exp*e1=_tmp364;struct Cyc_Absyn_Exp*e2=_tmp365;struct Cyc_Absyn_Exp*e3=_tmp366;
# 2095
struct _tuple20 _tmp367=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp73=_tmp367;struct _tuple20 _tmp368=_stmttmp73;union Cyc_CfFlowInfo_FlowInfo _tmp36A;union Cyc_CfFlowInfo_FlowInfo _tmp369;_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp369;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp36A;
struct _tuple20 _tmp36B=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _stmttmp74=_tmp36B;struct _tuple20 _tmp36C=_stmttmp74;union Cyc_CfFlowInfo_FlowInfo _tmp36E;union Cyc_CfFlowInfo_FlowInfo _tmp36D;_tmp36D=_tmp36C.f1;_tmp36E=_tmp36C.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp36D;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp36E;
struct _tuple20 _tmp36F=Cyc_NewControlFlow_anal_test(env,f1f,e3);struct _tuple20 _stmttmp75=_tmp36F;struct _tuple20 _tmp370=_stmttmp75;union Cyc_CfFlowInfo_FlowInfo _tmp372;union Cyc_CfFlowInfo_FlowInfo _tmp371;_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;{union Cyc_CfFlowInfo_FlowInfo f3t=_tmp371;union Cyc_CfFlowInfo_FlowInfo f3f=_tmp372;
return({struct _tuple20 _tmp58F;({union Cyc_CfFlowInfo_FlowInfo _tmp6AF=Cyc_CfFlowInfo_join_flow(fenv,f2t,f3t);_tmp58F.f1=_tmp6AF;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp6AE=Cyc_CfFlowInfo_join_flow(fenv,f2f,f3f);_tmp58F.f2=_tmp6AE;});_tmp58F;});}}}}case 7: _tmp364=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp362)->f1;_tmp365=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp362)->f2;{struct Cyc_Absyn_Exp*e1=_tmp364;struct Cyc_Absyn_Exp*e2=_tmp365;
# 2101
struct _tuple20 _tmp373=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp76=_tmp373;struct _tuple20 _tmp374=_stmttmp76;union Cyc_CfFlowInfo_FlowInfo _tmp376;union Cyc_CfFlowInfo_FlowInfo _tmp375;_tmp375=_tmp374.f1;_tmp376=_tmp374.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp375;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp376;
struct _tuple20 _tmp377=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _stmttmp77=_tmp377;struct _tuple20 _tmp378=_stmttmp77;union Cyc_CfFlowInfo_FlowInfo _tmp37A;union Cyc_CfFlowInfo_FlowInfo _tmp379;_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp379;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp37A;
return({struct _tuple20 _tmp590;_tmp590.f1=f2t,({union Cyc_CfFlowInfo_FlowInfo _tmp6B0=Cyc_CfFlowInfo_join_flow(fenv,f1f,f2f);_tmp590.f2=_tmp6B0;});_tmp590;});}}}case 8: _tmp364=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp362)->f1;_tmp365=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp362)->f2;{struct Cyc_Absyn_Exp*e1=_tmp364;struct Cyc_Absyn_Exp*e2=_tmp365;
# 2105
struct _tuple20 _tmp37B=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp78=_tmp37B;struct _tuple20 _tmp37C=_stmttmp78;union Cyc_CfFlowInfo_FlowInfo _tmp37E;union Cyc_CfFlowInfo_FlowInfo _tmp37D;_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp37D;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp37E;
struct _tuple20 _tmp37F=Cyc_NewControlFlow_anal_test(env,f1f,e2);struct _tuple20 _stmttmp79=_tmp37F;struct _tuple20 _tmp380=_stmttmp79;union Cyc_CfFlowInfo_FlowInfo _tmp382;union Cyc_CfFlowInfo_FlowInfo _tmp381;_tmp381=_tmp380.f1;_tmp382=_tmp380.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp381;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp382;
return({struct _tuple20 _tmp591;({union Cyc_CfFlowInfo_FlowInfo _tmp6B1=Cyc_CfFlowInfo_join_flow(fenv,f1t,f2t);_tmp591.f1=_tmp6B1;}),_tmp591.f2=f2f;_tmp591;});}}}case 9: _tmp364=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp362)->f1;_tmp365=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp362)->f2;{struct Cyc_Absyn_Exp*e1=_tmp364;struct Cyc_Absyn_Exp*e2=_tmp365;
# 2109
struct _tuple17 _tmp383=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);struct _tuple17 _stmttmp7A=_tmp383;struct _tuple17 _tmp384=_stmttmp7A;void*_tmp386;union Cyc_CfFlowInfo_FlowInfo _tmp385;_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp385;void*r=_tmp386;
return Cyc_NewControlFlow_anal_test(env,f,e2);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp362)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp362)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp362)->f2)->tl == 0){_tmp364=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp362)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_tmp364;
# 2112
struct _tuple20 _tmp387=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp7B=_tmp387;struct _tuple20 _tmp388=_stmttmp7B;union Cyc_CfFlowInfo_FlowInfo _tmp38A;union Cyc_CfFlowInfo_FlowInfo _tmp389;_tmp389=_tmp388.f1;_tmp38A=_tmp388.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp389;union Cyc_CfFlowInfo_FlowInfo f2=_tmp38A;
return({struct _tuple20 _tmp592;_tmp592.f1=f2,_tmp592.f2=f1;_tmp592;});}}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp363=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp362)->f1;_tmp364=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp362)->f2;{enum Cyc_Absyn_Primop p=_tmp363;struct Cyc_List_List*es=_tmp364;
# 2115
return Cyc_NewControlFlow_anal_primop_test(env,inflow,p,es);}}default:  {
# 2119
struct _tuple17 _tmp38B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0);struct _tuple17 _stmttmp7C=_tmp38B;struct _tuple17 _tmp38C=_stmttmp7C;void*_tmp38E;union Cyc_CfFlowInfo_FlowInfo _tmp38D;_tmp38D=_tmp38C.f1;_tmp38E=_tmp38C.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp38D;void*r=_tmp38E;
union Cyc_CfFlowInfo_FlowInfo _tmp38F=f;struct Cyc_Dict_Dict _tmp390;if((_tmp38F.BottomFL).tag == 1)
return({struct _tuple20 _tmp593;_tmp593.f1=f,_tmp593.f2=f;_tmp593;});else{_tmp390=((_tmp38F.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp390;
# 2123
struct _tuple16 _tmp391=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp7D=_tmp391;struct _tuple16 _tmp392=_stmttmp7D;void*_tmp394;void*_tmp393;_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;{void*r=_tmp393;struct Cyc_List_List*names=_tmp394;
void*_tmp395=r;enum Cyc_CfFlowInfo_InitLevel _tmp396;void*_tmp398;void*_tmp397;switch(*((int*)_tmp395)){case 8: _tmp397=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp395)->f1;_tmp398=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp395)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp397;void*r2=_tmp398;
# 2126
({void*_tmp399=0U;({struct _fat_ptr _tmp6B2=({const char*_tmp39A="anal_test: bad namedlocation";_tag_fat(_tmp39A,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6B2,_tag_fat(_tmp399,sizeof(void*),0));});});}case 0:
 return({struct _tuple20 _tmp594;({union Cyc_CfFlowInfo_FlowInfo _tmp6B3=Cyc_CfFlowInfo_BottomFL();_tmp594.f1=_tmp6B3;}),_tmp594.f2=f;_tmp594;});case 1:
 goto _LL3D;case 5: _LL3D:
 goto _LL3F;case 4: _LL3F:
 return({struct _tuple20 _tmp595;_tmp595.f1=f,({union Cyc_CfFlowInfo_FlowInfo _tmp6B4=Cyc_CfFlowInfo_BottomFL();_tmp595.f2=_tmp6B4;});_tmp595;});case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp395)->f1 == Cyc_CfFlowInfo_NoneIL)
goto _LL43;else{_tmp396=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp395)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp396;
# 2136
return Cyc_NewControlFlow_splitzero(env,inflow,f,e,il,names);}}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp395)->f1 == Cyc_CfFlowInfo_NoneIL){_LL43:
# 2132
 goto _LL45;}else{
# 2137
return({struct _tuple20 _tmp596;_tmp596.f1=f,_tmp596.f2=f;_tmp596;});}case 7: _LL45:
# 2134
({void*_tmp39B=0U;({unsigned _tmp6B6=e->loc;struct _fat_ptr _tmp6B5=({const char*_tmp39C="expression may not be initialized";_tag_fat(_tmp39C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6B6,_tmp6B5,_tag_fat(_tmp39B,sizeof(void*),0));});});
return({struct _tuple20 _tmp597;({union Cyc_CfFlowInfo_FlowInfo _tmp6B8=Cyc_CfFlowInfo_BottomFL();_tmp597.f1=_tmp6B8;}),({union Cyc_CfFlowInfo_FlowInfo _tmp6B7=Cyc_CfFlowInfo_BottomFL();_tmp597.f2=_tmp6B7;});_tmp597;});default:
# 2138
({void*_tmp39D=0U;({struct _fat_ptr _tmp6B9=({const char*_tmp39E="anal_test";_tag_fat(_tmp39E,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6B9,_tag_fat(_tmp39D,sizeof(void*),0));});});};}}};}}};}struct _tuple29{unsigned f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2144
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple29*ckenv,void*root,void*rval){
# 2146
struct _tuple29*_tmp39F=ckenv;struct Cyc_Dict_Dict _tmp3A2;void*_tmp3A1;unsigned _tmp3A0;_tmp3A0=_tmp39F->f1;_tmp3A1=_tmp39F->f2;_tmp3A2=_tmp39F->f3;{unsigned loc=_tmp3A0;struct Cyc_NewControlFlow_AnalEnv*env=_tmp3A1;struct Cyc_Dict_Dict fd=_tmp3A2;
void*_tmp3A3=root;void*_tmp3A4;if(*((int*)_tmp3A3)== 0){_tmp3A4=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3A3)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp3A4;
# 2149
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
struct _tuple16 _tmp3A5=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple16 _stmttmp7E=_tmp3A5;struct _tuple16 _tmp3A6=_stmttmp7E;void*_tmp3A7;_tmp3A7=_tmp3A6.f1;{void*rval=_tmp3A7;
void*_tmp3A8=rval;switch(*((int*)_tmp3A8)){case 7:
 goto _LLF;case 0: _LLF:
 goto _LL11;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3A8)->f1 == Cyc_CfFlowInfo_NoneIL){_LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12:
# 2156
({struct Cyc_String_pa_PrintArg_struct _tmp3AB=({struct Cyc_String_pa_PrintArg_struct _tmp598;_tmp598.tag=0,({struct _fat_ptr _tmp6BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp598.f1=_tmp6BA;});_tmp598;});void*_tmp3A9[1];_tmp3A9[0]=& _tmp3AB;({unsigned _tmp6BC=loc;struct _fat_ptr _tmp6BB=({const char*_tmp3AA="unique pointers reachable from %s may become unreachable";_tag_fat(_tmp3AA,sizeof(char),57U);});Cyc_Warn_warn(_tmp6BC,_tmp6BB,_tag_fat(_tmp3A9,sizeof(void*),1));});});
goto _LLB;}_LLB:;}}
# 2160
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 2165
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp3AC=flow;struct Cyc_Dict_Dict _tmp3AD;if((_tmp3AC.BottomFL).tag == 1)
return;else{_tmp3AD=((_tmp3AC.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp3AD;
# 2169
{struct Cyc_List_List*_tmp3AE=env->param_roots;struct Cyc_List_List*inits=_tmp3AE;for(0;inits != 0;inits=inits->tl){
if((int)({struct Cyc_CfFlowInfo_FlowEnv*_tmp6BE=env->fenv;struct Cyc_Dict_Dict _tmp6BD=d;Cyc_CfFlowInfo_initlevel(_tmp6BE,_tmp6BD,Cyc_CfFlowInfo_lookup_place(d,(struct Cyc_CfFlowInfo_Place*)inits->hd));})!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp3AF=0U;({unsigned _tmp6C0=loc;struct _fat_ptr _tmp6BF=({const char*_tmp3B0="function may not initialize all the parameters with attribute 'initializes'";_tag_fat(_tmp3B0,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp6C0,_tmp6BF,_tag_fat(_tmp3AF,sizeof(void*),0));});});}}
# 2173
if(Cyc_Flags_warn_lose_unique){
struct _tuple29 _tmp3B1=({struct _tuple29 _tmp599;_tmp599.f1=loc,_tmp599.f2=env,_tmp599.f3=d;_tmp599;});struct _tuple29 check_env=_tmp3B1;
((void(*)(void(*)(struct _tuple29*,void*,void*),struct _tuple29*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& check_env,d);}
# 2177
return;}};}
# 2186
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2188
struct Cyc_List_List*_tmp3B2=0;struct Cyc_List_List*roots=_tmp3B2;
struct Cyc_List_List*_tmp3B3=0;struct Cyc_List_List*es=_tmp3B3;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp3B4=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp7F=_tmp3B4;struct _tuple25*_tmp3B5=_stmttmp7F;void*_tmp3B7;void*_tmp3B6;_tmp3B6=_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;{struct Cyc_Absyn_Vardecl**vopt=_tmp3B6;struct Cyc_Absyn_Exp*eopt=_tmp3B7;
if((vopt != 0 && eopt != 0)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(eopt->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3B8=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=0,_tmp3BD->f1=*vopt;_tmp3BD;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp3B8;
struct Cyc_CfFlowInfo_Place*_tmp3B9=({struct Cyc_CfFlowInfo_Place*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->root=(void*)root,_tmp3BC->path=0;_tmp3BC;});struct Cyc_CfFlowInfo_Place*rp=_tmp3B9;
roots=({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->hd=rp,_tmp3BA->tl=roots;_tmp3BA;});
es=({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->hd=eopt,_tmp3BB->tl=es;_tmp3BB;});}}}}
# 2200
return({struct _tuple1 _tmp59A;_tmp59A.f1=roots,_tmp59A.f2=es;_tmp59A;});}struct _tuple30{int f1;void*f2;};
# 2206
static struct _tuple30 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2213
union Cyc_CfFlowInfo_FlowInfo _tmp3BE=flow;struct Cyc_Dict_Dict _tmp3BF;if((_tmp3BE.BottomFL).tag == 1)
({void*_tmp3C0=0U;({struct _fat_ptr _tmp6C1=({const char*_tmp3C1="noconsume_place_ok: flow became Bottom!";_tag_fat(_tmp3C1,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6C1,_tag_fat(_tmp3C0,sizeof(void*),0));});});else{_tmp3BF=((_tmp3BE.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp3BF;
# 2221
struct Cyc_Absyn_Exp*_tmp3C2=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*bogus_exp=_tmp3C2;
int _tmp3C3=0;int bogus_bool=_tmp3C3;
int _tmp3C4=1;int bogus_int=_tmp3C4;
void*_tmp3C5=Cyc_CfFlowInfo_lookup_place(d,place);void*curr_rval=_tmp3C5;
void*_tmp3C6=curr_rval;void*rval=_tmp3C6;
# 2233
int varok=0;
{void*_tmp3C7=curr_rval;void*_tmp3C9;void*_tmp3C8;if(*((int*)_tmp3C7)== 8){_tmp3C8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3C7)->f1;_tmp3C9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3C7)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp3C8;void*r=_tmp3C9;
# 2236
if(vd == n){
rval=r;
# 2239
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2241
if(Cyc_CfFlowInfo_is_unique_consumed(bogus_exp,bogus_int,rval,& bogus_bool)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp3CC=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0,({
# 2245
struct _fat_ptr _tmp6C2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp59B.f1=_tmp6C2;});_tmp59B;});void*_tmp3CA[1];_tmp3CA[0]=& _tmp3CC;({unsigned _tmp6C4=loc;struct _fat_ptr _tmp6C3=({const char*_tmp3CB="function consumes parameter %s which is does not have the 'consume' attribute";_tag_fat(_tmp3CB,sizeof(char),78U);});Cyc_CfFlowInfo_aerr(_tmp6C4,_tmp6C3,_tag_fat(_tmp3CA,sizeof(void*),1));});});}else{
# 2248
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,rval)!= (int)Cyc_CfFlowInfo_AllIL && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp59C;_tmp59C.tag=0,({
# 2251
struct _fat_ptr _tmp6C5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp59C.f1=_tmp6C5;});_tmp59C;});void*_tmp3CD[1];_tmp3CD[0]=& _tmp3CF;({unsigned _tmp6C7=loc;struct _fat_ptr _tmp6C6=({const char*_tmp3CE="function consumes value pointed to by parameter %s, which does not have the 'consume' attribute";_tag_fat(_tmp3CE,sizeof(char),96U);});Cyc_CfFlowInfo_aerr(_tmp6C7,_tmp6C6,_tag_fat(_tmp3CD,sizeof(void*),1));});});else{
# 2253
varok=1;}}}else{
# 2256
varok=1;}}else{
# 2259
goto _LL9;}
goto _LL5;}}else{_LL9:
# 2263
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp3D2=({struct Cyc_String_pa_PrintArg_struct _tmp59D;_tmp59D.tag=0,({
# 2268
struct _fat_ptr _tmp6C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp59D.f1=_tmp6C8;});_tmp59D;});void*_tmp3D0[1];_tmp3D0[0]=& _tmp3D2;({unsigned _tmp6CA=loc;struct _fat_ptr _tmp6C9=({const char*_tmp3D1="function parameter %s without 'consume' attribute no longer set to its original value";_tag_fat(_tmp3D1,sizeof(char),86U);});Cyc_CfFlowInfo_aerr(_tmp6CA,_tmp6C9,_tag_fat(_tmp3D0,sizeof(void*),1));});});}
goto _LL5;}_LL5:;}
# 2275
return({struct _tuple30 _tmp59E;_tmp59E.f1=varok,_tmp59E.f2=rval;_tmp59E;});}};}
# 2281
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp3D3=p;void*_tmp3D5;void*_tmp3D4;_tmp3D4=_tmp3D3->root;_tmp3D5=_tmp3D3->path;{void*root=_tmp3D4;struct Cyc_List_List*fs=_tmp3D5;
if(fs != 0)
({void*_tmp3D6=0U;({struct _fat_ptr _tmp6CB=({const char*_tmp3D7="unconsume_params: param to unconsume is non-variable";_tag_fat(_tmp3D7,sizeof(char),53U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6CB,_tag_fat(_tmp3D6,sizeof(void*),0));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp3D8=root;void*_tmp3D9;if(*((int*)_tmp3D8)== 0){_tmp3D9=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3D8)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp3D9;
return vd;}}else{
({void*_tmp3DA=0U;({struct _fat_ptr _tmp6CC=({const char*_tmp3DB="unconsume_params: root is not a varroot";_tag_fat(_tmp3DB,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6CC,_tag_fat(_tmp3DA,sizeof(void*),0));});});};}}}
# 2300 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2308
{union Cyc_CfFlowInfo_FlowInfo _tmp3DC=flow;struct Cyc_Dict_Dict _tmp3DD;if((_tmp3DC.BottomFL).tag == 1)
return flow;else{_tmp3DD=((_tmp3DC.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp3DD;
# 2315
struct _tuple19 _tmp3DE=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple19 _stmttmp80=_tmp3DE;struct _tuple19 _tmp3DF=_stmttmp80;union Cyc_CfFlowInfo_AbsLVal _tmp3E1;union Cyc_CfFlowInfo_FlowInfo _tmp3E0;_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp3E0;union Cyc_CfFlowInfo_AbsLVal lval=_tmp3E1;
# 2318
{union Cyc_CfFlowInfo_AbsLVal _tmp3E2=lval;void*_tmp3E3;if((_tmp3E2.PlaceL).tag == 1){_tmp3E3=(_tmp3E2.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp3E3;
# 2322
void*_tmp3E4=Cyc_CfFlowInfo_lookup_place(d,p);void*old_rval=_tmp3E4;
{void*_tmp3E5=old_rval;void*_tmp3E7;void*_tmp3E6;if(*((int*)_tmp3E5)== 8){_tmp3E6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E5)->f1;_tmp3E7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E5)->f2;{struct Cyc_Absyn_Vardecl*old_vd=_tmp3E6;void*r=_tmp3E7;
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
struct _tuple17 _tmp3E8=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp6CE=env->fenv;struct _tuple17 _tmp6CD=({struct _tuple17 _tmp59F;_tmp59F.f1=f,_tmp59F.f2=old_rval;_tmp59F;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp6CE,_tmp6CD,({struct _tuple17 _tmp5A0;_tmp5A0.f1=f,_tmp5A0.f2=ropt;_tmp5A0;}));});
# 2345
struct _tuple17 _stmttmp81=_tmp3E8;struct _tuple17 _tmp3E9=_stmttmp81;void*_tmp3EB;union Cyc_CfFlowInfo_FlowInfo _tmp3EA;_tmp3EA=_tmp3E9.f1;_tmp3EB=_tmp3E9.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp3EA;void*new_rval2=_tmp3EB;
# 2349
f=f2;new_rval=new_rval2;}}else{
# 2354
new_rval=old_rval;}}else{
# 2357
new_rval=r;}
# 2359
{union Cyc_CfFlowInfo_FlowInfo _tmp3EC=f;void*_tmp3EE;struct Cyc_Dict_Dict _tmp3ED;if((_tmp3EC.ReachableFL).tag == 2){_tmp3ED=((_tmp3EC.ReachableFL).val).f1;_tmp3EE=((_tmp3EC.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp3ED;struct Cyc_List_List*relns=_tmp3EE;
# 2361
flow=({struct Cyc_Dict_Dict _tmp6CF=Cyc_CfFlowInfo_assign_place(env->fenv,loc,d2,p,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp6CF,relns);});
# 2365
goto _LL15;}}else{
# 2367
({void*_tmp3EF=0U;({struct _fat_ptr _tmp6D0=({const char*_tmp3F0="unconsume_params: joined flow became bot!";_tag_fat(_tmp3F0,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D0,_tag_fat(_tmp3EF,sizeof(void*),0));});});}_LL15:;}
# 2369
goto _LLD;}else{
# 2371
goto _LL11;}
goto _LLD;}}else{_LL11:
# 2378
 if(ropt != 0 && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp3F3=({struct Cyc_String_pa_PrintArg_struct _tmp5A1;_tmp5A1.tag=0,({
struct _fat_ptr _tmp6D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp5A1.f1=_tmp6D1;});_tmp5A1;});void*_tmp3F1[1];_tmp3F1[0]=& _tmp3F3;({unsigned _tmp6D3=loc;struct _fat_ptr _tmp6D2=({const char*_tmp3F2="aliased expression %s was overwritten";_tag_fat(_tmp3F2,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp6D3,_tmp6D2,_tag_fat(_tmp3F1,sizeof(void*),1));});});
# 2395 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2397
goto _LL8;}}else{
# 2403
goto _LL8;}_LL8:;}
# 2405
goto _LL0;}}}_LL0:;}
# 2407
return flow;}
# 2420 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2426
{union Cyc_CfFlowInfo_FlowInfo _tmp3F4=flow;if((_tmp3F4.BottomFL).tag == 1)
return flow;else{
goto _LL0;}_LL0:;}{
# 2430
int _tmp3F5=unconsume_exps != 0;int do_unconsume=_tmp3F5;
struct Cyc_List_List*_tmp3F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(consumed_vals);struct Cyc_List_List*consumed_vals=_tmp3F6;
struct Cyc_List_List*_tmp3F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(unconsume_exps);struct Cyc_List_List*unconsume_exps=_tmp3F7;
{struct Cyc_List_List*_tmp3F8=consumed_vals;struct Cyc_List_List*params=_tmp3F8;for(0;params != 0;(
params=params->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2439
struct Cyc_Absyn_Vardecl*_tmp3F9=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)params->hd);struct Cyc_Absyn_Vardecl*vd=_tmp3F9;
struct _tuple30 _tmp3FA=
is_region_open?({struct _tuple30 _tmp5A2;_tmp5A2.f1=1,_tmp5A2.f2=0;_tmp5A2;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)params->hd,do_unconsume,vd,flow,loc);
# 2440
struct _tuple30 _stmttmp82=_tmp3FA;struct _tuple30 _tmp3FB=_stmttmp82;void*_tmp3FD;int _tmp3FC;_tmp3FC=_tmp3FB.f1;_tmp3FD=_tmp3FB.f2;{int varok=_tmp3FC;void*rval=_tmp3FD;
# 2446
if(do_unconsume)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,vd,varok,rval,flow,loc);}}}
# 2449
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;}}struct _tuple31{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2453
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){
# 2455
struct Cyc_CfFlowInfo_FlowEnv*_tmp3FE=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp3FE;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp3FF=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmp83=_tmp3FF;struct Cyc_Absyn_Switch_clause*_tmp400=_stmttmp83;unsigned _tmp404;void*_tmp403;void*_tmp402;void*_tmp401;_tmp401=_tmp400->pat_vars;_tmp402=_tmp400->where_clause;_tmp403=_tmp400->body;_tmp404=_tmp400->loc;{struct Cyc_Core_Opt*vds_opt=_tmp401;struct Cyc_Absyn_Exp*where_opt=_tmp402;struct Cyc_Absyn_Stmt*body=_tmp403;unsigned loc=_tmp404;
struct _tuple1 _tmp405=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds_opt))->v);struct _tuple1 _stmttmp84=_tmp405;struct _tuple1 _tmp406=_stmttmp84;void*_tmp408;void*_tmp407;_tmp407=_tmp406.f1;_tmp408=_tmp406.f2;{struct Cyc_List_List*roots=_tmp407;struct Cyc_List_List*es=_tmp408;
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)vds_opt->v,roots != 0,loc,exp_loc,0);
# 2463
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp409=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp409;
# 2466
if(Cyc_Tcpat_has_vars(vds_opt))
({struct Cyc_List_List*_tmp6D5=({struct Cyc_List_List*_tmp40B=_cycalloc(sizeof(*_tmp40B));
({struct _tuple31*_tmp6D4=({struct _tuple31*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->f1=0,_tmp40A->f2=body,_tmp40A->f3=roots,_tmp40A->f4=es;_tmp40A;});_tmp40B->hd=_tmp6D4;}),_tmp40B->tl=old_unique_pat_vars;_tmp40B;});
# 2467
env->unique_pat_vars=_tmp6D5;});
# 2470
if(where_opt != 0){
struct Cyc_Absyn_Exp*wexp=where_opt;
struct _tuple20 _tmp40C=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple20 _stmttmp85=_tmp40C;struct _tuple20 _tmp40D=_stmttmp85;union Cyc_CfFlowInfo_FlowInfo _tmp40F;union Cyc_CfFlowInfo_FlowInfo _tmp40E;_tmp40E=_tmp40D.f1;_tmp40F=_tmp40D.f2;{union Cyc_CfFlowInfo_FlowInfo ft=_tmp40E;union Cyc_CfFlowInfo_FlowInfo ff=_tmp40F;
inflow=ff;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,ft,body,0);}}else{
# 2476
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,body,0);}
# 2478
env->unique_pat_vars=old_unique_pat_vars;{
union Cyc_CfFlowInfo_FlowInfo _tmp410=clause_outflow;if((_tmp410.BottomFL).tag == 1)
goto _LL9;else{
# 2483
clause_outflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,clause_outflow,loc);
# 2485
if(scs->tl == 0)
return clause_outflow;else{
# 2490
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp411=0U;({unsigned _tmp6D7=body->loc;struct _fat_ptr _tmp6D6=({const char*_tmp412="switch clause may implicitly fallthru";_tag_fat(_tmp412,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp6D7,_tmp6D6,_tag_fat(_tmp411,sizeof(void*),0));});});else{
# 2493
({void*_tmp413=0U;({unsigned _tmp6D9=body->loc;struct _fat_ptr _tmp6D8=({const char*_tmp414="switch clause may implicitly fallthru";_tag_fat(_tmp414,sizeof(char),38U);});Cyc_Warn_warn(_tmp6D9,_tmp6D8,_tag_fat(_tmp413,sizeof(void*),0));});});}
# 2495
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2497
goto _LL9;}_LL9:;}}}}
# 2500
return Cyc_CfFlowInfo_BottomFL();}struct _tuple32{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned f3;};
# 2505
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple32*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2507
struct _tuple32*_tmp415=vdenv;unsigned _tmp418;struct Cyc_Dict_Dict _tmp417;void*_tmp416;_tmp416=_tmp415->f1;_tmp417=_tmp415->f2;_tmp418=_tmp415->f3;{struct Cyc_NewControlFlow_AnalEnv*env=_tmp416;struct Cyc_Dict_Dict fd=_tmp417;unsigned loc=_tmp418;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2510
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5A4;_tmp5A4.tag=0,_tmp5A4.f1=vd;_tmp5A4;});
struct _tuple16 _tmp419=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp6DA=fd;((void*(*)(struct Cyc_Dict_Dict,int(*)(void*,void*),void*))Cyc_Dict_lookup_other)(_tmp6DA,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp420=_cycalloc(sizeof(*_tmp420));*_tmp420=vdroot;_tmp420;}));}));struct _tuple16 _stmttmp86=_tmp419;struct _tuple16 _tmp41A=_stmttmp86;void*_tmp41B;_tmp41B=_tmp41A.f1;{void*rval=_tmp41B;
void*_tmp41C=rval;switch(*((int*)_tmp41C)){case 7:
 goto _LLA;case 0: _LLA:
 goto _LLC;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp41C)->f1 == Cyc_CfFlowInfo_NoneIL){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
# 2517
({struct Cyc_String_pa_PrintArg_struct _tmp41F=({struct Cyc_String_pa_PrintArg_struct _tmp5A3;_tmp5A3.tag=0,({struct _fat_ptr _tmp6DB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp5A3.f1=_tmp6DB;});_tmp5A3;});void*_tmp41D[1];_tmp41D[0]=& _tmp41F;({unsigned _tmp6DD=loc;struct _fat_ptr _tmp6DC=({const char*_tmp41E="unique pointers may still exist after variable %s goes out of scope";_tag_fat(_tmp41E,sizeof(char),68U);});Cyc_Warn_warn(_tmp6DD,_tmp6DC,_tag_fat(_tmp41D,sizeof(void*),1));});});
# 2519
goto _LL6;}_LL6:;}}}}
# 2524
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp421=inflow;struct Cyc_Dict_Dict _tmp422;if((_tmp421.ReachableFL).tag == 2){_tmp422=((_tmp421.ReachableFL).val).f1;{struct Cyc_Dict_Dict fd=_tmp422;
# 2527
struct _tuple32 _tmp423=({struct _tuple32 _tmp5A5;_tmp5A5.f1=env,_tmp5A5.f2=fd,_tmp5A5.f3=decl->loc;_tmp5A5;});struct _tuple32 vdenv=_tmp423;
struct Cyc_CfFlowInfo_FlowEnv*_tmp424=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp424;
{void*_tmp425=decl->r;void*_stmttmp87=_tmp425;void*_tmp426=_stmttmp87;void*_tmp427;switch(*((int*)_tmp426)){case 0: _tmp427=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp426)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp427;
# 2531
Cyc_NewControlFlow_check_dropped_unique_vd(& vdenv,vd);
goto _LL5;}case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp426)->f2 != 0){_tmp427=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp426)->f2)->v;{struct Cyc_List_List*vds=_tmp427;
# 2534
struct _tuple1 _tmp428=((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)(vds);struct _tuple1 _stmttmp88=_tmp428;struct _tuple1 _tmp429=_stmttmp88;void*_tmp42A;_tmp42A=_tmp429.f1;{struct Cyc_List_List*vs=_tmp42A;
struct Cyc_List_List*_tmp42B=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)(vs);{struct Cyc_List_List*vs=_tmp42B;
_tmp427=vs;goto _LLB;}}}}else{goto _LLC;}case 3: _tmp427=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp426)->f1;_LLB: {struct Cyc_List_List*vds=_tmp427;
# 2538
((void(*)(void(*)(struct _tuple32*,struct Cyc_Absyn_Vardecl*),struct _tuple32*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& vdenv,vds);
goto _LL5;}default: _LLC:
 goto _LL5;}_LL5:;}
# 2542
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 2545
return;}
# 2551
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2555
int num_pop=((int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple31*_tmp42C=(struct _tuple31*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple31*_stmttmp89=_tmp42C;struct _tuple31*_tmp42D=_stmttmp89;void*_tmp431;void*_tmp430;void*_tmp42F;int _tmp42E;_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;_tmp430=_tmp42D->f3;_tmp431=_tmp42D->f4;{int is_open=_tmp42E;struct Cyc_Absyn_Stmt*pat_stmt=_tmp42F;struct Cyc_List_List*roots=_tmp430;struct Cyc_List_List*es=_tmp431;
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,is_open,inflow,dest->loc);}}}
# 2560
return inflow;}
# 2566
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple18*rval_opt){
# 2568
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple21 _tmp432=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple21 _stmttmp8A=_tmp432;struct _tuple21 _tmp433=_stmttmp8A;void*_tmp435;void*_tmp434;_tmp434=_tmp433.f1;_tmp435=_tmp433.f2;{struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp434;union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp435;
inflow=*sflow;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp436=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp436;
# 2575
void*_tmp437=s->r;void*_stmttmp8B=_tmp437;void*_tmp438=_stmttmp8B;unsigned _tmp43B;void*_tmp43F;void*_tmp43E;void*_tmp43D;void*_tmp43C;void*_tmp43A;void*_tmp439;switch(*((int*)_tmp438)){case 0:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp438)->f1 == 0){
# 2579
if(env->noreturn)
({void*_tmp440=0U;({unsigned _tmp6DF=s->loc;struct _fat_ptr _tmp6DE=({const char*_tmp441="`noreturn' function might return";_tag_fat(_tmp441,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp6DF,_tmp6DE,_tag_fat(_tmp440,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_tmp439=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;{struct Cyc_Absyn_Exp*e=_tmp439;
# 2585
if(env->noreturn)
({void*_tmp442=0U;({unsigned _tmp6E1=s->loc;struct _fat_ptr _tmp6E0=({const char*_tmp443="`noreturn' function might return";_tag_fat(_tmp443,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp6E1,_tmp6E0,_tag_fat(_tmp442,sizeof(void*),0));});});{
struct _tuple17 _tmp444=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(e),0);struct _tuple17 _stmttmp8C=_tmp444;struct _tuple17 _tmp445=_stmttmp8C;void*_tmp447;union Cyc_CfFlowInfo_FlowInfo _tmp446;_tmp446=_tmp445.f1;_tmp447=_tmp445.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp446;void*r=_tmp447;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,r);
Cyc_NewControlFlow_check_init_params(s->loc,env,f);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,f,s->loc);
return Cyc_CfFlowInfo_BottomFL();}}}}case 1: _tmp439=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;{struct Cyc_Absyn_Exp*e=_tmp439;
# 2594
struct _tuple18*_tmp448=rval_opt;int _tmp44A;void*_tmp449;if(_tmp448 != 0){_tmp449=(void**)& _tmp448->f1;_tmp44A=_tmp448->f2;{void**rval=(void**)_tmp449;int copy_ctxt=_tmp44A;
# 2596
struct _tuple17 _tmp44B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e,0);struct _tuple17 _stmttmp8D=_tmp44B;struct _tuple17 _tmp44C=_stmttmp8D;void*_tmp44E;union Cyc_CfFlowInfo_FlowInfo _tmp44D;_tmp44D=_tmp44C.f1;_tmp44E=_tmp44C.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp44D;void*r=_tmp44E;
*rval=r;
return f;}}}else{
# 2600
return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0)).f1;};}case 2: _tmp439=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp439;struct Cyc_Absyn_Stmt*s2=_tmp43A;
# 2604
return({struct Cyc_NewControlFlow_AnalEnv*_tmp6E4=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E3=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);struct Cyc_Absyn_Stmt*_tmp6E2=s2;Cyc_NewControlFlow_anal_stmt(_tmp6E4,_tmp6E3,_tmp6E2,rval_opt);});}case 4: _tmp439=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;_tmp43C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp438)->f3;{struct Cyc_Absyn_Exp*e=_tmp439;struct Cyc_Absyn_Stmt*s1=_tmp43A;struct Cyc_Absyn_Stmt*s2=_tmp43C;
# 2607
struct _tuple20 _tmp44F=Cyc_NewControlFlow_anal_test(env,inflow,e);struct _tuple20 _stmttmp8E=_tmp44F;struct _tuple20 _tmp450=_stmttmp8E;union Cyc_CfFlowInfo_FlowInfo _tmp452;union Cyc_CfFlowInfo_FlowInfo _tmp451;_tmp451=_tmp450.f1;_tmp452=_tmp450.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp451;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp452;
# 2614
union Cyc_CfFlowInfo_FlowInfo _tmp453=Cyc_NewControlFlow_anal_stmt(env,f1f,s2,0);union Cyc_CfFlowInfo_FlowInfo ff=_tmp453;
union Cyc_CfFlowInfo_FlowInfo _tmp454=Cyc_NewControlFlow_anal_stmt(env,f1t,s1,0);union Cyc_CfFlowInfo_FlowInfo ft=_tmp454;
return Cyc_CfFlowInfo_join_flow(fenv,ft,ff);}}case 5: _tmp439=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp438)->f1).f1;_tmp43A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp438)->f1).f2;_tmp43C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;{struct Cyc_Absyn_Exp*e=_tmp439;struct Cyc_Absyn_Stmt*cont=_tmp43A;struct Cyc_Absyn_Stmt*body=_tmp43C;
# 2622
struct _tuple21 _tmp455=Cyc_NewControlFlow_pre_stmt_check(env,inflow,cont);struct _tuple21 _stmttmp8F=_tmp455;struct _tuple21 _tmp456=_stmttmp8F;void*_tmp457;_tmp457=_tmp456.f2;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_tmp457;
union Cyc_CfFlowInfo_FlowInfo _tmp458=*eflow_ptr;union Cyc_CfFlowInfo_FlowInfo e_inflow=_tmp458;
struct _tuple20 _tmp459=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _stmttmp90=_tmp459;struct _tuple20 _tmp45A=_stmttmp90;union Cyc_CfFlowInfo_FlowInfo _tmp45C;union Cyc_CfFlowInfo_FlowInfo _tmp45B;_tmp45B=_tmp45A.f1;_tmp45C=_tmp45A.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp45B;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp45C;
union Cyc_CfFlowInfo_FlowInfo _tmp45D=Cyc_NewControlFlow_anal_stmt(env,f1t,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp45D;
Cyc_NewControlFlow_update_flow(env,cont,body_outflow);
return f1f;}}}case 14: _tmp439=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp438)->f2).f1;_tmp43C=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp438)->f2).f2;{struct Cyc_Absyn_Stmt*body=_tmp439;struct Cyc_Absyn_Exp*e=_tmp43A;struct Cyc_Absyn_Stmt*cont=_tmp43C;
# 2632
union Cyc_CfFlowInfo_FlowInfo _tmp45E=Cyc_NewControlFlow_anal_stmt(env,inflow,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp45E;
struct _tuple21 _tmp45F=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _stmttmp91=_tmp45F;struct _tuple21 _tmp460=_stmttmp91;void*_tmp461;_tmp461=_tmp460.f2;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_tmp461;
union Cyc_CfFlowInfo_FlowInfo _tmp462=*eflow_ptr;union Cyc_CfFlowInfo_FlowInfo e_inflow=_tmp462;
struct _tuple20 _tmp463=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _stmttmp92=_tmp463;struct _tuple20 _tmp464=_stmttmp92;union Cyc_CfFlowInfo_FlowInfo _tmp466;union Cyc_CfFlowInfo_FlowInfo _tmp465;_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp465;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp466;
Cyc_NewControlFlow_update_flow(env,body,f1t);
return f1f;}}}case 9: _tmp439=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp438)->f2).f1;_tmp43C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp438)->f2).f2;_tmp43D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp438)->f3).f1;_tmp43E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp438)->f3).f2;_tmp43F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp438)->f4;{struct Cyc_Absyn_Exp*e1=_tmp439;struct Cyc_Absyn_Exp*e2=_tmp43A;struct Cyc_Absyn_Stmt*guard=_tmp43C;struct Cyc_Absyn_Exp*e3=_tmp43D;struct Cyc_Absyn_Stmt*cont=_tmp43E;struct Cyc_Absyn_Stmt*body=_tmp43F;
# 2641
union Cyc_CfFlowInfo_FlowInfo _tmp467=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0)).f1;union Cyc_CfFlowInfo_FlowInfo e1_outflow=_tmp467;
struct _tuple21 _tmp468=Cyc_NewControlFlow_pre_stmt_check(env,e1_outflow,guard);struct _tuple21 _stmttmp93=_tmp468;struct _tuple21 _tmp469=_stmttmp93;void*_tmp46A;_tmp46A=_tmp469.f2;{union Cyc_CfFlowInfo_FlowInfo*e2flow_ptr=_tmp46A;
union Cyc_CfFlowInfo_FlowInfo _tmp46B=*e2flow_ptr;union Cyc_CfFlowInfo_FlowInfo e2_inflow=_tmp46B;
struct _tuple20 _tmp46C=Cyc_NewControlFlow_anal_test(env,e2_inflow,e2);struct _tuple20 _stmttmp94=_tmp46C;struct _tuple20 _tmp46D=_stmttmp94;union Cyc_CfFlowInfo_FlowInfo _tmp46F;union Cyc_CfFlowInfo_FlowInfo _tmp46E;_tmp46E=_tmp46D.f1;_tmp46F=_tmp46D.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp46E;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp46F;
union Cyc_CfFlowInfo_FlowInfo _tmp470=Cyc_NewControlFlow_anal_stmt(env,f2t,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp470;
struct _tuple21 _tmp471=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _stmttmp95=_tmp471;struct _tuple21 _tmp472=_stmttmp95;void*_tmp473;_tmp473=_tmp472.f2;{union Cyc_CfFlowInfo_FlowInfo*e3flow_ptr=_tmp473;
union Cyc_CfFlowInfo_FlowInfo _tmp474=*e3flow_ptr;union Cyc_CfFlowInfo_FlowInfo e3_inflow=_tmp474;
union Cyc_CfFlowInfo_FlowInfo _tmp475=(Cyc_NewControlFlow_anal_Rexp(env,0,e3_inflow,e3,0)).f1;union Cyc_CfFlowInfo_FlowInfo e3_outflow=_tmp475;
Cyc_NewControlFlow_update_flow(env,guard,e3_outflow);
return f2f;}}}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp438)->f2 != 0){_tmp439=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;{struct Cyc_List_List*es=_tmp439;struct Cyc_Absyn_Switch_clause*destclause=_tmp43A;
# 2653
struct _tuple24 _tmp476=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp96=_tmp476;struct _tuple24 _tmp477=_stmttmp96;void*_tmp479;union Cyc_CfFlowInfo_FlowInfo _tmp478;_tmp478=_tmp477.f1;_tmp479=_tmp477.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp478;struct Cyc_List_List*rvals=_tmp479;
# 2655
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,destclause->body);{
# 2657
struct Cyc_List_List*_tmp47A=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(destclause->pat_vars))->v)).f1);struct Cyc_List_List*vds=_tmp47A;
f=Cyc_NewControlFlow_add_vars(fenv,f,vds,fenv->unknown_all,s->loc,0);
# 2660
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(destclause->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp47B=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp97=_tmp47B;struct _tuple25*_tmp47C=_stmttmp97;void*_tmp47E;void*_tmp47D;_tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;{struct Cyc_Absyn_Vardecl**vd=_tmp47D;struct Cyc_Absyn_Exp*ve=_tmp47E;
if(vd != 0){
f=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6EA=fenv;struct Cyc_NewControlFlow_AnalEnv*_tmp6E9=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E8=f;struct Cyc_Absyn_Vardecl*_tmp6E7=*vd;struct Cyc_Absyn_Exp*_tmp6E6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;void*_tmp6E5=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp6EA,_tmp6E9,_tmp6E8,_tmp6E7,_tmp6E6,_tmp6E5,s->loc);});
rvals=rvals->tl;
es=es->tl;}}}}
# 2668
Cyc_NewControlFlow_update_flow(env,destclause->body,f);
return Cyc_CfFlowInfo_BottomFL();}}}}else{
# 2839
({void*_tmp4B1=0U;({struct _fat_ptr _tmp6EB=({const char*_tmp4B2="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_fat(_tmp4B2,sizeof(char),54U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6EB,_tag_fat(_tmp4B1,sizeof(void*),0));});});}case 6:
# 2674
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp47F=inflow;if((_tmp47F.ReachableFL).tag == 2){
# 2677
if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type)||
 Cyc_Tcutil_is_any_int_type(env->return_type))
({void*_tmp480=0U;({unsigned _tmp6ED=s->loc;struct _fat_ptr _tmp6EC=({const char*_tmp481="break may cause function not to return a value";_tag_fat(_tmp481,sizeof(char),47U);});Cyc_Warn_warn(_tmp6ED,_tmp6EC,_tag_fat(_tmp480,sizeof(void*),0));});});else{
# 2682
({void*_tmp482=0U;({unsigned _tmp6EF=s->loc;struct _fat_ptr _tmp6EE=({const char*_tmp483="break may cause function not to return a value";_tag_fat(_tmp483,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp6EF,_tmp6EE,_tag_fat(_tmp482,sizeof(void*),0));});});}}
# 2684
goto _LL55;}else{
goto _LL55;}_LL55:;}
# 2687
if(env->noreturn)
({void*_tmp484=0U;({unsigned _tmp6F1=s->loc;struct _fat_ptr _tmp6F0=({const char*_tmp485="`noreturn' function might return";_tag_fat(_tmp485,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp6F1,_tmp6F0,_tag_fat(_tmp484,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2693
goto _LL1B;case 7: _LL1B:
 goto _LL1D;case 8: _LL1D: {
# 2697
struct Cyc_Absyn_Stmt*_tmp486=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(env->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp486;
if(dest == 0)
({void*_tmp487=0U;({unsigned _tmp6F3=s->loc;struct _fat_ptr _tmp6F2=({const char*_tmp488="jump has no target (should be impossible)";_tag_fat(_tmp488,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp6F3,_tmp6F2,_tag_fat(_tmp487,sizeof(void*),0));});});
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(dest));
# 2702
Cyc_NewControlFlow_update_flow(env,dest,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _tmp439=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;_tmp43C=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp438)->f3;{struct Cyc_Absyn_Exp*e=_tmp439;struct Cyc_List_List*scs=_tmp43A;void*dec_tree=_tmp43C;
# 2708
return Cyc_NewControlFlow_anal_scs(env,inflow,scs,e->loc);}case 15: _tmp439=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;_tmp43C=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp438)->f3;{struct Cyc_Absyn_Stmt*s1=_tmp439;struct Cyc_List_List*scs=_tmp43A;void*dec_tree=_tmp43C;
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
# 2726
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,scs,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp489=scs_outflow;if((_tmp489.BottomFL).tag == 1)
goto _LL5A;else{
({void*_tmp48A=0U;({unsigned _tmp6F5=s->loc;struct _fat_ptr _tmp6F4=({const char*_tmp48B="last catch clause may implicitly fallthru";_tag_fat(_tmp48B,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp6F5,_tmp6F4,_tag_fat(_tmp48A,sizeof(void*),0));});});}_LL5A:;}
# 2731
outflow=s1_outflow;
# 2733
return outflow;}}}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)->f2 != 0){_tmp439=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)->f1;_tmp43A=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)->f2)->v;_tmp43C=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)->f3;_tmp43B=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->loc;_tmp43D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;{struct Cyc_Absyn_Pat*letpat=_tmp439;struct Cyc_List_List*vds=_tmp43A;struct Cyc_Absyn_Exp*e=_tmp43C;unsigned loc=_tmp43B;struct Cyc_Absyn_Stmt*s1=_tmp43D;
# 2738
({void*_tmp48C=0U;({struct Cyc___cycFILE*_tmp6F7=Cyc_stderr;struct _fat_ptr _tmp6F6=({const char*_tmp48D="";_tag_fat(_tmp48D,sizeof(char),1U);});Cyc_fprintf(_tmp6F7,_tmp6F6,_tag_fat(_tmp48C,sizeof(void*),0));});});{
# 2744
int isAliasPat=0;
{void*_tmp48E=letpat->r;void*_stmttmp98=_tmp48E;void*_tmp48F=_stmttmp98;if(*((int*)_tmp48F)== 2){
isAliasPat=1;goto _LL5F;}else{
goto _LL5F;}_LL5F:;}{
# 2749
struct _tuple1 _tmp490=Cyc_NewControlFlow_get_unconsume_pat_vars(vds);struct _tuple1 _stmttmp99=_tmp490;struct _tuple1 _tmp491=_stmttmp99;void*_tmp493;void*_tmp492;_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;{struct Cyc_List_List*roots=_tmp492;struct Cyc_List_List*es=_tmp493;
inflow=Cyc_NewControlFlow_initialize_pat_vars(fenv,env,inflow,vds,roots != 0,loc,e->loc,isAliasPat);{
struct Cyc_List_List*_tmp494=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp494;
# 2753
({struct Cyc_List_List*_tmp6F9=({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));
({struct _tuple31*_tmp6F8=({struct _tuple31*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->f1=0,_tmp495->f2=s,_tmp495->f3=roots,_tmp495->f4=es;_tmp495;});_tmp496->hd=_tmp6F8;}),_tmp496->tl=old_unique_pat_vars;_tmp496;});
# 2753
env->unique_pat_vars=_tmp6F9;});
# 2758
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2762
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,inflow,loc);
# 2766
return inflow;}}}}}}else{goto _LL26;}case 4: _tmp439=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)->f2;_tmp43A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->r)->f3;_tmp43B=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1)->loc;_tmp43C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;if((struct Cyc_Absyn_Exp*)_tmp43A != 0){struct Cyc_Absyn_Vardecl*vd=_tmp439;struct Cyc_Absyn_Exp*open_exp_opt=_tmp43A;unsigned loc=_tmp43B;struct Cyc_Absyn_Stmt*s1=_tmp43C;
# 2778
struct Cyc_List_List l=({struct Cyc_List_List _tmp5A7;_tmp5A7.hd=open_exp_opt,_tmp5A7.tl=0;_tmp5A7;});
union Cyc_CfFlowInfo_FlowInfo _tmp497=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);union Cyc_CfFlowInfo_FlowInfo f=_tmp497;
struct _tuple19 _tmp498=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,open_exp_opt);struct _tuple19 _stmttmp9A=_tmp498;struct _tuple19 _tmp499=_stmttmp9A;union Cyc_CfFlowInfo_AbsLVal _tmp49A;_tmp49A=_tmp499.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_tmp49A;
struct _tuple17 _tmp49B=Cyc_NewControlFlow_anal_Rexp(env,1,f,open_exp_opt,0);struct _tuple17 _stmttmp9B=_tmp49B;struct _tuple17 _tmp49C=_stmttmp9B;union Cyc_CfFlowInfo_FlowInfo _tmp49D;_tmp49D=_tmp49C.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp49D;
struct Cyc_List_List*_tmp49E=0;struct Cyc_List_List*roots=_tmp49E;
struct Cyc_List_List*_tmp49F=0;struct Cyc_List_List*es=_tmp49F;
{union Cyc_CfFlowInfo_FlowInfo _tmp4A0=f;void*_tmp4A2;struct Cyc_Dict_Dict _tmp4A1;if((_tmp4A0.ReachableFL).tag == 2){_tmp4A1=((_tmp4A0.ReachableFL).val).f1;_tmp4A2=((_tmp4A0.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_tmp4A1;struct Cyc_List_List*relns=_tmp4A2;
# 2786
{union Cyc_CfFlowInfo_AbsLVal _tmp4A3=lval;void*_tmp4A4;if((_tmp4A3.PlaceL).tag == 1){_tmp4A4=(_tmp4A3.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp4A4;
# 2790
void*_tmp4A5=Cyc_CfFlowInfo_lookup_place(fd,p);void*new_rval=_tmp4A5;
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->tag=8,_tmp4A6->f1=vd,_tmp4A6->f2=new_rval;_tmp4A6;});
fd=Cyc_CfFlowInfo_assign_place(fenv,open_exp_opt->loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4A7=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->tag=0,_tmp4AC->f1=vd;_tmp4AC;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp4A7;
struct Cyc_CfFlowInfo_Place*_tmp4A8=({struct Cyc_CfFlowInfo_Place*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->root=(void*)root,_tmp4AB->path=0;_tmp4AB;});struct Cyc_CfFlowInfo_Place*rp=_tmp4A8;
roots=({struct Cyc_List_List*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->hd=rp,_tmp4A9->tl=roots;_tmp4A9;});
es=({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->hd=open_exp_opt,_tmp4AA->tl=es;_tmp4AA;});
goto _LL72;}}}else{
# 2805
goto _LL72;}_LL72:;}
# 2807
goto _LL6D;}}else{
# 2809
goto _LL6D;}_LL6D:;}{
# 2812
struct Cyc_List_List _tmp4AD=({struct Cyc_List_List _tmp5A6;_tmp5A6.hd=vd,_tmp5A6.tl=0;_tmp5A6;});struct Cyc_List_List vds=_tmp4AD;
f=Cyc_NewControlFlow_add_vars(fenv,f,& vds,fenv->unknown_all,loc,0);{
# 2816
struct Cyc_List_List*_tmp4AE=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp4AE;
({struct Cyc_List_List*_tmp6FB=({struct Cyc_List_List*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));({struct _tuple31*_tmp6FA=({struct _tuple31*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->f1=1,_tmp4AF->f2=s,_tmp4AF->f3=roots,_tmp4AF->f4=es;_tmp4AF;});_tmp4B0->hd=_tmp6FA;}),_tmp4B0->tl=old_unique_pat_vars;_tmp4B0;});env->unique_pat_vars=_tmp6FB;});
# 2821
f=Cyc_NewControlFlow_anal_stmt(env,f,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2825
f=Cyc_NewControlFlow_unconsume_params(env,roots,es,1,f,loc);
# 2829
return f;}}}}}else{goto _LL26;}default: _LL26: _tmp439=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f1;_tmp43A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;{struct Cyc_Absyn_Decl*d=_tmp439;struct Cyc_Absyn_Stmt*s=_tmp43A;
# 2832
outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp6FE=env;union Cyc_CfFlowInfo_FlowInfo _tmp6FD=Cyc_NewControlFlow_anal_decl(env,inflow,d);struct Cyc_Absyn_Stmt*_tmp6FC=s;Cyc_NewControlFlow_anal_stmt(_tmp6FE,_tmp6FD,_tmp6FC,rval_opt);});
if(Cyc_Flags_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,d);
return outflow;}}default: _tmp439=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp438)->f2;{struct Cyc_Absyn_Stmt*s=_tmp439;
# 2837
return Cyc_NewControlFlow_anal_stmt(env,inflow,s,rval_opt);}};}}}
# 2843
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Fndecl*);
# 2847
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp4B3=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp4B3;
void*_tmp4B4=decl->r;void*_stmttmp9C=_tmp4B4;void*_tmp4B5=_stmttmp9C;void*_tmp4B8;void*_tmp4B7;void*_tmp4B6;switch(*((int*)_tmp4B5)){case 0: _tmp4B6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4B5)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp4B6;
# 2855
if((int)vd->sc == (int)Cyc_Absyn_Static)
return inflow;{
# 2860
struct Cyc_List_List _tmp4B9=({struct Cyc_List_List _tmp5A8;_tmp5A8.hd=vd,_tmp5A8.tl=0;_tmp5A8;});struct Cyc_List_List vds=_tmp4B9;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp4BA=vd->initializer;struct Cyc_Absyn_Exp*e=_tmp4BA;
if(e == 0)
return inflow;{
struct _tuple17 _tmp4BB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);struct _tuple17 _stmttmp9D=_tmp4BB;struct _tuple17 _tmp4BC=_stmttmp9D;void*_tmp4BE;union Cyc_CfFlowInfo_FlowInfo _tmp4BD;_tmp4BD=_tmp4BC.f1;_tmp4BE=_tmp4BC.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp4BD;void*r=_tmp4BE;
return Cyc_NewControlFlow_do_initialize_var(fenv,env,f,vd,e,r,decl->loc);}}}}}case 3: _tmp4B6=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4B5)->f1;{struct Cyc_List_List*vds=_tmp4B6;
# 2869
return Cyc_NewControlFlow_add_vars(fenv,inflow,vds,fenv->unknown_none,decl->loc,0);}case 1: _tmp4B6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4B5)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp4B6;
# 2872
Cyc_NewControlFlow_check_nested_fun(env->all_tables,fenv,inflow,fd);{
struct Cyc_Absyn_Vardecl*_tmp4BF=(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);struct Cyc_Absyn_Vardecl*vd=_tmp4BF;
# 2877
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp702=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp701=inflow;struct Cyc_List_List*_tmp700=({struct Cyc_List_List*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->hd=vd,_tmp4C0->tl=0;_tmp4C0;});void*_tmp6FF=fenv->unknown_all;Cyc_NewControlFlow_add_vars(_tmp702,_tmp701,_tmp700,_tmp6FF,decl->loc,0);});}}case 4: _tmp4B6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4B5)->f1;_tmp4B7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4B5)->f2;_tmp4B8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4B5)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp4B6;struct Cyc_Absyn_Vardecl*vd=_tmp4B7;struct Cyc_Absyn_Exp*open_exp_opt=_tmp4B8;
# 2880
if(open_exp_opt != 0)
({void*_tmp4C1=0U;({struct _fat_ptr _tmp703=({const char*_tmp4C2="found open expression in declaration!";_tag_fat(_tmp4C2,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp703,_tag_fat(_tmp4C1,sizeof(void*),0));});});{
struct Cyc_List_List _tmp4C3=({struct Cyc_List_List _tmp5A9;_tmp5A9.hd=vd,_tmp5A9.tl=0;_tmp5A9;});struct Cyc_List_List vds=_tmp4C3;
return Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_all,decl->loc,0);}}default:
# 2886
({void*_tmp4C4=0U;({struct _fat_ptr _tmp704=({const char*_tmp4C5="anal_decl: unexpected decl variant";_tag_fat(_tmp4C5,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp704,_tag_fat(_tmp4C4,sizeof(void*),0));});});};}
# 2894
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp4C6;_push_handler(& _tmp4C6);{int _tmp4C8=0;if(setjmp(_tmp4C6.handler))_tmp4C8=1;if(!_tmp4C8){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp707=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp706=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp705=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp707,_tmp706,_tmp705,fd);});}
# 2896
;_pop_handler();}else{void*_tmp4C7=(void*)Cyc_Core_get_exn_thrown();void*_tmp4C9=_tmp4C7;void*_tmp4CA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4C9)->tag == Cyc_Dict_Absent){
# 2902
if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->tag=Cyc_Dict_Absent;_tmp4CB;}));}}else{_tmp4CA=_tmp4C9;{void*exn=_tmp4CA;(int)_rethrow(exn);}}_LL0:;}}}
# 2908
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2913
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp4CC=r;unsigned _tmp4CD;if((_tmp4CC.RParam).tag == 5){_tmp4CD=(_tmp4CC.RParam).val;{unsigned i=_tmp4CD;
# 2916
struct Cyc_Absyn_Vardecl*_tmp4CE=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(vds,(int)i);struct Cyc_Absyn_Vardecl*vd=_tmp4CE;
if(!vd->escapes)
return Cyc_Relations_RVar(vd);
return r;}}else{
return r;};}
# 2927
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){
# 2929
struct _RegionHandle _tmp4CF=_new_region("r");struct _RegionHandle*r=& _tmp4CF;_push_region(r);
{int _tmp4D0=((int(*)(struct Cyc_List_List*))Cyc_List_length)(param_vardecls);int len=_tmp4D0;
struct _fat_ptr _tmp4D1=({unsigned _tmp4DD=(unsigned)len;int*_tmp4DC=({struct _RegionHandle*_tmp708=r;_region_malloc(_tmp708,_check_times(_tmp4DD,sizeof(int)));});({{unsigned _tmp5AA=(unsigned)len;unsigned i;for(i=0;i < _tmp5AA;++ i){_tmp4DC[i]=0;}}0;});_tag_fat(_tmp4DC,sizeof(int),_tmp4DD);});struct _fat_ptr cons=_tmp4D1;
for(1;atts != 0;atts=atts->tl){
void*_tmp4D2=(void*)atts->hd;void*_stmttmp9E=_tmp4D2;void*_tmp4D3=_stmttmp9E;int _tmp4D4;switch(*((int*)_tmp4D3)){case 22: _tmp4D4=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp4D3)->f1;{int i=_tmp4D4;
_tmp4D4=i;goto _LL4;}case 21: _tmp4D4=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp4D3)->f1;_LL4: {int i=_tmp4D4;
*((int*)_check_fat_subscript(cons,sizeof(int),i - 1))=1;goto _LL0;}default:
 goto _LL0;}_LL0:;}{
# 2939
struct Cyc_List_List*noconsume_roots=0;
{int i=0;for(0;param_vardecls != 0;(param_vardecls=param_vardecls->tl,i ++)){
struct Cyc_Absyn_Vardecl*_tmp4D5=(struct Cyc_Absyn_Vardecl*)param_vardecls->hd;struct Cyc_Absyn_Vardecl*vd=_tmp4D5;
if(Cyc_Tcutil_is_noalias_pointer(vd->type,0)&& !(*((int*)_check_fat_subscript(cons,sizeof(int),i)))){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D6=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->tag=0,_tmp4DA->f1=vd;_tmp4DA;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp4D6;
struct Cyc_CfFlowInfo_Place*_tmp4D7=({struct Cyc_CfFlowInfo_Place*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->root=(void*)root,_tmp4D9->path=0;_tmp4D9;});struct Cyc_CfFlowInfo_Place*rp=_tmp4D7;
noconsume_roots=({struct Cyc_List_List*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->hd=rp,_tmp4D8->tl=noconsume_roots;_tmp4D8;});}}}{
# 2948
struct Cyc_List_List*_tmp4DB=noconsume_roots;_npop_handler(0);return _tmp4DB;}}}
# 2930
;_pop_region();}
# 2951
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2955
unsigned _tmp4DE=(fd->body)->loc;unsigned loc=_tmp4DE;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,loc,1);{
# 2960
struct Cyc_List_List*param_roots=0;
struct _tuple15 _tmp4DF=({union Cyc_CfFlowInfo_FlowInfo _tmp506=inflow;if((_tmp506.ReachableFL).tag != 2)_throw_match();(_tmp506.ReachableFL).val;});struct _tuple15 _stmttmp9F=_tmp4DF;struct _tuple15 _tmp4E0=_stmttmp9F;void*_tmp4E2;struct Cyc_Dict_Dict _tmp4E1;_tmp4E1=_tmp4E0.f1;_tmp4E2=_tmp4E0.f2;{struct Cyc_Dict_Dict d=_tmp4E1;struct Cyc_List_List*relns=_tmp4E2;
# 2964
struct Cyc_List_List*_tmp4E3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vardecls=_tmp4E3;
{struct Cyc_List_List*_tmp4E4=(fd->i).requires_relns;struct Cyc_List_List*reqs=_tmp4E4;for(0;reqs != 0;reqs=reqs->tl){
struct Cyc_Relations_Reln*_tmp4E5=(struct Cyc_Relations_Reln*)reqs->hd;struct Cyc_Relations_Reln*req=_tmp4E5;
relns=({struct _RegionHandle*_tmp70C=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp70B=Cyc_NewControlFlow_translate_rop(vardecls,req->rop1);enum Cyc_Relations_Relation _tmp70A=req->relation;union Cyc_Relations_RelnOp _tmp709=
Cyc_NewControlFlow_translate_rop(vardecls,req->rop2);
# 2967
Cyc_Relations_add_relation(_tmp70C,_tmp70B,_tmp70A,_tmp709,relns);});}}{
# 2973
struct Cyc_List_List*atts;
{void*_tmp4E6=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmpA0=_tmp4E6;void*_tmp4E7=_stmttmpA0;void*_tmp4E8;if(*((int*)_tmp4E7)== 5){_tmp4E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E7)->f1).attributes;{struct Cyc_List_List*as=_tmp4E8;
atts=as;goto _LL3;}}else{
({void*_tmp4E9=0U;({struct _fat_ptr _tmp70D=({const char*_tmp4EA="check_fun: non-function type cached with fndecl_t";_tag_fat(_tmp4EA,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp70D,_tag_fat(_tmp4E9,sizeof(void*),0));});});}_LL3:;}{
# 2978
struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,atts);
# 2981
for(1;atts != 0;atts=atts->tl){
void*_tmp4EB=(void*)atts->hd;void*_stmttmpA1=_tmp4EB;void*_tmp4EC=_stmttmpA1;int _tmp4ED;switch(*((int*)_tmp4EC)){case 21: _tmp4ED=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp4EC)->f1;{int i=_tmp4ED;
# 2984
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp4EE=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,i - 1);struct Cyc_Absyn_Vardecl*vd=_tmp4EE;
void*t=Cyc_Absyn_compress(vd->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp4EF=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp710=fenv;void*_tmp70F=elttype;struct Cyc_Absyn_Exp*_tmp70E=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp710,_tmp70F,_tmp70E,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all),0);});
# 2988
void*rval=_tmp4EF;
# 2992
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4F0=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->tag=2,_tmp4F5->f1=i,_tmp4F5->f2=t;_tmp4F5;});struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r=_tmp4F0;
struct Cyc_CfFlowInfo_Place*_tmp4F1=({struct Cyc_CfFlowInfo_Place*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->root=(void*)r,_tmp4F4->path=0;_tmp4F4;});struct Cyc_CfFlowInfo_Place*rp=_tmp4F1;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d,(void*)r,rval);
d=({struct Cyc_Dict_Dict _tmp712=d;void*_tmp711=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->tag=0,_tmp4F2->f1=vd;_tmp4F2;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp712,_tmp711,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->tag=4,_tmp4F3->f1=rp;_tmp4F3;}));});
goto _LL8;}case 20: _tmp4ED=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp4EC)->f1;{int i=_tmp4ED;
# 2998
struct Cyc_Absyn_Vardecl*_tmp4F6=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,i - 1);struct Cyc_Absyn_Vardecl*vd=_tmp4F6;
void*elttype=Cyc_Tcutil_pointer_elt_type(vd->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4F7=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->tag=2,_tmp4FD->f1=i,_tmp4FD->f2=elttype;_tmp4FD;});struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r=_tmp4F7;
struct Cyc_CfFlowInfo_Place*_tmp4F8=({struct Cyc_CfFlowInfo_Place*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->root=(void*)r,_tmp4FC->path=0;_tmp4FC;});struct Cyc_CfFlowInfo_Place*rp=_tmp4F8;
d=({struct Cyc_Dict_Dict _tmp714=d;void*_tmp713=(void*)r;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp714,_tmp713,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});
d=({struct Cyc_Dict_Dict _tmp716=d;void*_tmp715=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->tag=0,_tmp4F9->f1=vd;_tmp4F9;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp716,_tmp715,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->tag=4,_tmp4FA->f1=rp;_tmp4FA;}));});
param_roots=({struct Cyc_List_List*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->hd=rp,_tmp4FB->tl=param_roots;_tmp4FB;});
goto _LL8;}default:
 goto _LL8;}_LL8:;}
# 3009
inflow=Cyc_CfFlowInfo_ReachableFL(d,relns);{
# 3011
int noreturn=Cyc_Atts_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp505=_cycalloc(sizeof(*_tmp505));
_tmp505->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp718=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp505->succ_table=_tmp718;}),({
struct Cyc_Hashtable_Table*_tmp717=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp505->pat_pop_table=_tmp717;}),_tmp505->fenv=fenv,_tmp505->iterate_again=1,_tmp505->iteration_num=0,_tmp505->in_try=0,_tmp505->tryflow=inflow,_tmp505->noreturn=noreturn,_tmp505->return_type=(fd->i).ret_type,_tmp505->unique_pat_vars=0,_tmp505->param_roots=param_roots,_tmp505->noconsume_params=noconsume_roots,_tmp505->flow_table=flow_table,_tmp505->return_relns=(fd->i).ensures_relns;_tmp505;});
# 3021
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3027
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3030
union Cyc_CfFlowInfo_FlowInfo _tmp4FE=outflow;if((_tmp4FE.BottomFL).tag == 1)
goto _LLF;else{
# 3033
Cyc_NewControlFlow_check_init_params(loc,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,loc);
# 3037
if(noreturn)
({void*_tmp4FF=0U;({unsigned _tmp71A=loc;struct _fat_ptr _tmp719=({const char*_tmp500="`noreturn' function might (implicitly) return";_tag_fat(_tmp500,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp71A,_tmp719,_tag_fat(_tmp4FF,sizeof(void*),0));});});else{
if(!Cyc_Tcutil_is_void_type((fd->i).ret_type)){
if(Cyc_Tcutil_is_any_float_type((fd->i).ret_type)||
 Cyc_Tcutil_is_any_int_type((fd->i).ret_type))
({void*_tmp501=0U;({unsigned _tmp71C=loc;struct _fat_ptr _tmp71B=({const char*_tmp502="function may not return a value";_tag_fat(_tmp502,sizeof(char),32U);});Cyc_Warn_warn(_tmp71C,_tmp71B,_tag_fat(_tmp501,sizeof(void*),0));});});else{
# 3044
({void*_tmp503=0U;({unsigned _tmp71E=loc;struct _fat_ptr _tmp71D=({const char*_tmp504="function may not return a value";_tag_fat(_tmp504,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp71E,_tmp71D,_tag_fat(_tmp503,sizeof(void*),0));});});}goto _LLF;}}
# 3046
goto _LLF;}_LLF:;}}}}}}}
# 3050
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(1;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp507=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_stmttmpA2=_tmp507;void*_tmp508=_stmttmpA2;void*_tmp509;switch(*((int*)_tmp508)){case 1: _tmp509=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp508)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp509;
Cyc_NewControlFlow_check_fun(tables,fd);goto _LL0;}case 11: _tmp509=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp508)->f1;{struct Cyc_List_List*ds2=_tmp509;
# 3056
_tmp509=ds2;goto _LL6;}case 10: _tmp509=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp508)->f2;_LL6: {struct Cyc_List_List*ds2=_tmp509;
_tmp509=ds2;goto _LL8;}case 9: _tmp509=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp508)->f2;_LL8: {struct Cyc_List_List*ds2=_tmp509;
Cyc_NewControlFlow_cf_check(tables,ds2);goto _LL0;}case 12:
 goto _LL0;default:
 goto _LL0;}_LL0:;}}}
