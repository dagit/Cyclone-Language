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
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
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
if((int)c2 == (int)'\000')return 0;else{
return - 1;}}else{
if((int)c2 == (int)'\000')return 1;else{
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
({void*_tmp5AF=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=Cyc_NewControlFlow_CFAnnot,(_tmp3->f1).visited=0;_tmp3;});s->annot=_tmp5AF;});return Cyc_NewControlFlow_get_stmt_annot(s);};}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp5B0=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp5B0;});_tmp4;});
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
({union Cyc_CfFlowInfo_FlowInfo _tmp5B1=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*sflow);*sflow=_tmp5B1;});
# 169
annot->visited=env->iteration_num;
return({struct _tuple21 _tmp50D;_tmp50D.f1=annot,_tmp50D.f2=sflow;_tmp50D;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp5B2=Cyc_CfFlowInfo_join_tryflow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp5B2;});}struct _tuple22{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned f2;struct Cyc_Dict_Dict f3;};
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
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0,({struct _fat_ptr _tmp5B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp50E.f1=_tmp5B3;});_tmp50E;});void*_tmp11[1];_tmp11[0]=& _tmp13;({unsigned _tmp5B5=loc;struct _fat_ptr _tmp5B4=({const char*_tmp12="alias-free pointer(s) reachable from %s may become unreachable.";_tag_fat(_tmp12,sizeof(char),64U);});Cyc_Warn_warn(_tmp5B5,_tmp5B4,_tag_fat(_tmp11,sizeof(void*),1));});});
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
struct _tuple20 _tmp17=({struct _tuple20 _tmp510;_tmp510.f1=flow,_tmp510.f2=new_flow;_tmp510;});struct _tuple20 _stmttmp1=_tmp17;struct _tuple20 _tmp18=_stmttmp1;struct Cyc_Dict_Dict _tmp1A;struct Cyc_Dict_Dict _tmp19;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_tmp19=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict fd=_tmp19;struct Cyc_Dict_Dict new_fd=_tmp1A;
# 229
struct _tuple22 _tmp1B=({struct _tuple22 _tmp50F;_tmp50F.f1=env,_tmp50F.f2=s->loc,_tmp50F.f3=new_fd;_tmp50F;});struct _tuple22 ckenv=_tmp1B;
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
{struct Cyc_Absyn_Exp*_tmp32=({struct _RegionHandle*_tmp5B7=temp;struct Cyc_List_List*_tmp5B6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(temp,temp,ad->tvs,ts);Cyc_Tcutil_rsubsexp(_tmp5B7,_tmp5B6,req);});struct Cyc_Absyn_Exp*ireq=_tmp32;
# 308
if(!({struct Cyc_List_List*_tmp5B8=relns;Cyc_NewControlFlow_relns_ok(_tmp5B8,Cyc_Relations_exp2relns(temp,ireq));})){
({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0,({struct _fat_ptr _tmp5B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(ireq)));_tmp512.f1=_tmp5B9;});_tmp512;});struct Cyc_String_pa_PrintArg_struct _tmp36=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0,_tmp511.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp511;});void*_tmp33[2];_tmp33[0]=& _tmp35,_tmp33[1]=& _tmp36;({unsigned _tmp5BB=loc;struct _fat_ptr _tmp5BA=({const char*_tmp34="unable to prove %s, required to access %s";_tag_fat(_tmp34,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp5BB,_tmp5BA,_tag_fat(_tmp33,sizeof(void*),2));});});
({void*_tmp37=0U;({struct Cyc___cycFILE*_tmp5BD=Cyc_stderr;struct _fat_ptr _tmp5BC=({const char*_tmp38="  [recorded facts on path: ";_tag_fat(_tmp38,sizeof(char),28U);});Cyc_fprintf(_tmp5BD,_tmp5BC,_tag_fat(_tmp37,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,relns);
({void*_tmp39=0U;({struct Cyc___cycFILE*_tmp5BF=Cyc_stderr;struct _fat_ptr _tmp5BE=({const char*_tmp3A="]\n";_tag_fat(_tmp3A,sizeof(char),3U);});Cyc_fprintf(_tmp5BF,_tmp5BE,_tag_fat(_tmp39,sizeof(void*),0));});});}}
# 306
;_pop_region();}
# 315
goto _LL5;}}else{goto _LLA;}case 7: _tmp2C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B)->f2;{struct Cyc_List_List*fs=_tmp2C;
# 317
struct Cyc_Absyn_Aggrfield*_tmp3B=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp3B;
struct Cyc_Absyn_Exp*_tmp3C=((struct Cyc_Absyn_Aggrfield*)_check_null(field))->requires_clause;struct Cyc_Absyn_Exp*req=_tmp3C;
if(req != 0){
struct _RegionHandle _tmp3D=_new_region("temp");struct _RegionHandle*temp=& _tmp3D;_push_region(temp);
if(!({struct Cyc_List_List*_tmp5C0=relns;Cyc_NewControlFlow_relns_ok(_tmp5C0,Cyc_Relations_exp2relns(temp,req));})){
({struct Cyc_String_pa_PrintArg_struct _tmp40=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0,({struct _fat_ptr _tmp5C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(req)));_tmp514.f1=_tmp5C1;});_tmp514;});struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0,_tmp513.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp513;});void*_tmp3E[2];_tmp3E[0]=& _tmp40,_tmp3E[1]=& _tmp41;({unsigned _tmp5C3=loc;struct _fat_ptr _tmp5C2=({const char*_tmp3F="unable to prove %s, required to access %s";_tag_fat(_tmp3F,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp5C3,_tmp5C2,_tag_fat(_tmp3E,sizeof(void*),2));});});
({void*_tmp42=0U;({struct Cyc___cycFILE*_tmp5C5=Cyc_stderr;struct _fat_ptr _tmp5C4=({const char*_tmp43="  [recorded facts on path: ";_tag_fat(_tmp43,sizeof(char),28U);});Cyc_fprintf(_tmp5C5,_tmp5C4,_tag_fat(_tmp42,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,relns);
({void*_tmp44=0U;({struct Cyc___cycFILE*_tmp5C7=Cyc_stderr;struct _fat_ptr _tmp5C6=({const char*_tmp45="]\n";_tag_fat(_tmp45,sizeof(char),3U);});Cyc_fprintf(_tmp5C7,_tmp5C6,_tag_fat(_tmp44,sizeof(void*),0));});});}
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
({void*_tmp49=0U;({unsigned _tmp5C9=loc;struct _fat_ptr _tmp5C8=({const char*_tmp4A="expression may not be fully initialized";_tag_fat(_tmp4A,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp5C9,_tmp5C8,_tag_fat(_tmp49,sizeof(void*),0));});});{
struct Cyc_Dict_Dict _tmp4B=Cyc_CfFlowInfo_escape_deref(env->fenv,d,r);struct Cyc_Dict_Dict ans_d=_tmp4B;
if(d.t == ans_d.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);union Cyc_CfFlowInfo_FlowInfo ans=_tmp4C;
Cyc_NewControlFlow_update_tryflow(env,ans);
return ans;}}}};}struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 349
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,void*r){
struct _tuple0 _tmp4D=({struct _tuple0 _tmp515;({void*_tmp5CA=Cyc_Absyn_compress(t);_tmp515.f1=_tmp5CA;}),_tmp515.f2=r;_tmp515;});struct _tuple0 _stmttmp4=_tmp4D;struct _tuple0 _tmp4E=_stmttmp4;enum Cyc_Absyn_AggrKind _tmp4F;union Cyc_Absyn_AggrInfo _tmp52;struct _fat_ptr _tmp51;void*_tmp50;switch(*((int*)_tmp4E.f2)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f1 == Cyc_CfFlowInfo_NoneIL)
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
({struct Cyc_NewControlFlow_AnalEnv*_tmp5CD=env;unsigned _tmp5CC=loc;void*_tmp5CB=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp5CD,_tmp5CC,_tmp5CB,*((void**)_check_fat_subscript(ad,sizeof(void*),i)));});}}
# 364
return;}}else{goto _LL13;}case 7: if(*((int*)_tmp4E.f2)== 6){_tmp4F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E.f1)->f1;_tmp50=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4E.f1)->f2;_tmp51=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4E.f2)->f2;{enum Cyc_Absyn_AggrKind k=_tmp4F;struct Cyc_List_List*fs=_tmp50;struct _fat_ptr d=_tmp51;
# 380
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));(i ++,fs=fs->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd)->type,((void**)d.curr)[i]);}}
return;}}else{goto _LL13;}case 3: _LL11:
# 384
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp57=0U;({unsigned _tmp5CF=loc;struct _fat_ptr _tmp5CE=({const char*_tmp58="argument may still contain alias-free pointers";_tag_fat(_tmp58,sizeof(char),47U);});Cyc_Warn_warn(_tmp5CF,_tmp5CE,_tag_fat(_tmp57,sizeof(void*),0));});});
return;default: _LL13:
 return;}};}
# 391
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp59=inflow;void*_tmp5B;struct Cyc_Dict_Dict _tmp5A;if((_tmp59.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_tmp5A=((_tmp59.ReachableFL).val).f1;_tmp5B=((_tmp59.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp5A;struct Cyc_List_List*relns=_tmp5B;
# 395
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp5C=0U;({struct _fat_ptr _tmp5D0=({const char*_tmp5D="noliveunique attribute requires alias-free pointer";_tag_fat(_tmp5D,sizeof(char),51U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5D0,_tag_fat(_tmp5C,sizeof(void*),0));});});{
void*_tmp5E=Cyc_Tcutil_pointer_elt_type(t);void*elt_type=_tmp5E;
retry: {void*_tmp5F=r;void*_tmp60;switch(*((int*)_tmp5F)){case 8: _tmp60=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;{void*r2=_tmp60;
r=r2;goto retry;}case 4: _tmp60=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp60;
_tmp60=Cyc_CfFlowInfo_lookup_place(d,p);goto _LLB;}case 5: _tmp60=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1;_LLB: {void*r=_tmp60;
Cyc_NewControlFlow_check_nounique(env,loc,elt_type,r);goto _LL5;}default:
# 403
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(elt_type))
({void*_tmp61=0U;({unsigned _tmp5D2=loc;struct _fat_ptr _tmp5D1=({const char*_tmp62="argument may contain live alias-free pointers";_tag_fat(_tmp62,sizeof(char),46U);});Cyc_Warn_warn(_tmp5D2,_tmp5D1,_tag_fat(_tmp61,sizeof(void*),0));});});
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
return({struct _tuple24 _tmp516;_tmp516.f1=inflow,_tmp516.f2=0;_tmp516;});
do{
struct _tuple17 _tmp65=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd,0);struct _tuple17 _stmttmp5=_tmp65;struct _tuple17 _tmp66=_stmttmp5;void*_tmp68;union Cyc_CfFlowInfo_FlowInfo _tmp67;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp67;void*r=_tmp68;
inflow=f;
rvals=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=r,_tmp69->tl=rvals;_tmp69;});
es=es->tl;
first_is_copy=others_are_copy;}}while(es != 0);
# 434
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple24 _tmp517;_tmp517.f1=inflow,({struct Cyc_List_List*_tmp5D3=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(rvals);_tmp517.f2=_tmp5D3;});_tmp517;});}
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
({void*_tmp70=0U;({unsigned _tmp5D5=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp5D4=({const char*_tmp71="expression may not be initialized";_tag_fat(_tmp71,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp5D5,_tmp5D4,_tag_fat(_tmp70,sizeof(void*),0));});});}
goto _LL3;}}else{
goto _LL3;}_LL3:;}
# 452
return({struct _tuple17 _tmp518;_tmp518.f1=afterflow,_tmp518.f2=(env->fenv)->unknown_all;_tmp518;});}}
# 459
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 466
int needs_unconsume=0;
void*_tmp72=Cyc_CfFlowInfo_lookup_place(new_dict,p);void*old_rval=_tmp72;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume))
({void*_tmp73=0U;({struct _fat_ptr _tmp5D6=({const char*_tmp74="consume_zero_rval";_tag_fat(_tmp74,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5D6,_tag_fat(_tmp73,sizeof(void*),0));});});else{
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
union Cyc_CfFlowInfo_FlowInfo _tmp7C=({struct Cyc_Dict_Dict _tmp5D7=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp5D7,relns);});union Cyc_CfFlowInfo_FlowInfo outflow=_tmp7C;
# 513
return outflow;}}};}};}
# 522
static struct _tuple20 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 524
union Cyc_CfFlowInfo_FlowInfo _tmp7D=outflow;void*_tmp7F;struct Cyc_Dict_Dict _tmp7E;if((_tmp7D.BottomFL).tag == 1)
return({struct _tuple20 _tmp519;_tmp519.f1=outflow,_tmp519.f2=outflow;_tmp519;});else{_tmp7E=((_tmp7D.ReachableFL).val).f1;_tmp7F=((_tmp7D.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp7E;struct Cyc_List_List*relns=_tmp7F;
# 527
union Cyc_CfFlowInfo_AbsLVal _tmp80=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _stmttmp8=_tmp80;union Cyc_CfFlowInfo_AbsLVal _tmp81=_stmttmp8;void*_tmp82;if((_tmp81.UnknownL).tag == 2)
return({struct _tuple20 _tmp51A;_tmp51A.f1=outflow,_tmp51A.f2=outflow;_tmp51A;});else{_tmp82=(_tmp81.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp82;
# 530
void*nzval=(int)il == (int)1U?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(1;names != 0;names=names->tl){
nzval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=8,_tmp83->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp83->f2=nzval;_tmp83;});
zval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=8,_tmp84->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp84->f2=zval;_tmp84;});}
# 536
nzval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,d,p,e,zval);
return({struct _tuple20 _tmp51B;({
union Cyc_CfFlowInfo_FlowInfo _tmp5DB=({struct Cyc_Dict_Dict _tmp5DA=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp5DA,relns);});_tmp51B.f1=_tmp5DB;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp5D9=({struct Cyc_Dict_Dict _tmp5D8=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,d,p,zval);Cyc_CfFlowInfo_ReachableFL(_tmp5D8,relns);});_tmp51B.f2=_tmp5D9;});_tmp51B;});}};}};}
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
({void*_tmp5DC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=Cyc_CfFlowInfo_UnknownZ,_tmp88->f1=relns;_tmp88;});e->annot=_tmp5DC;});else{
# 563
({void*_tmp5DD=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=Cyc_CfFlowInfo_NotZero,_tmp89->f1=relns;_tmp89;});e->annot=_tmp5DD;});}}
# 571
static struct _tuple17 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_Absyn_Exp*index){
# 575
void*_tmp8A=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpA=_tmp8A;void*_tmp8B=_stmttmpA;void*_tmp8D;void*_tmp8C;if(*((int*)_tmp8B)== 3){_tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmp8D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;{void*elttype=_tmp8C;void*bd=_tmp8D;
# 577
union Cyc_CfFlowInfo_FlowInfo _tmp8E=f;void*_tmp90;struct Cyc_Dict_Dict _tmp8F;if((_tmp8E.BottomFL).tag == 1)
# 579
return({struct _tuple17 _tmp51C;_tmp51C.f1=f,({void*_tmp5DE=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp51C.f2=_tmp5DE;});_tmp51C;});else{_tmp8F=((_tmp8E.ReachableFL).val).f1;_tmp90=((_tmp8E.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp8F;struct Cyc_List_List*relns=_tmp90;
# 581
struct _tuple16 _tmp91=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmpB=_tmp91;struct _tuple16 _tmp92=_stmttmpB;void*_tmp94;void*_tmp93;_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;{void*r=_tmp93;struct Cyc_List_List*names=_tmp94;
{void*_tmp95=r;enum Cyc_CfFlowInfo_InitLevel _tmp96;void*_tmp98;void*_tmp97;switch(*((int*)_tmp95)){case 8: _tmp97=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;_tmp98=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp95)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp97;void*r2=_tmp98;
# 584
({void*_tmp99=0U;({struct _fat_ptr _tmp5DF=({const char*_tmp9A="named location in anal_derefR";_tag_fat(_tmp9A,sizeof(char),30U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5DF,_tag_fat(_tmp99,sizeof(void*),0));});});}case 1:
# 586
 Cyc_NewControlFlow_update_relns(e,0,relns);
goto _LLD;case 4: _tmp97=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp97;
# 589
Cyc_NewControlFlow_update_relns(e,0,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _tmp51D;_tmp51D.f1=f,({void*_tmp5E0=Cyc_CfFlowInfo_lookup_place(outdict,p);_tmp51D.f2=_tmp5E0;});_tmp51D;});
goto _LLD;}case 5: _tmp97=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;{void*r=_tmp97;
# 595
Cyc_NewControlFlow_update_relns(e,1,relns);
if(index == 0 && Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple17 _tmp51E;_tmp51E.f1=f,_tmp51E.f2=r;_tmp51E;});
goto _LLD;}case 0:
# 600
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple17 _tmp51F;({union Cyc_CfFlowInfo_FlowInfo _tmp5E2=Cyc_CfFlowInfo_BottomFL();_tmp51F.f1=_tmp5E2;}),({void*_tmp5E1=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp51F.f2=_tmp5E1;});_tmp51F;});case 2: _tmp96=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp95)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp96;
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
({void*_tmpA1=0U;({unsigned _tmp5E4=e->loc;struct _fat_ptr _tmp5E3=({const char*_tmpA2="attempt to dereference a consumed alias-free pointer";_tag_fat(_tmpA2,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp5E4,_tmp5E3,_tag_fat(_tmpA1,sizeof(void*),0));});});
goto _LL24;}else{
# 618
({void*_tmpA3=0U;({unsigned _tmp5E6=e->loc;struct _fat_ptr _tmp5E5=({const char*_tmpA4="dereference of possibly uninitialized pointer";_tag_fat(_tmpA4,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp5E6,_tmp5E5,_tag_fat(_tmpA3,sizeof(void*),0));});});}_LL24:;}
# 620
goto _LL20;}}else{_LL20:
# 622
 return({struct _tuple17 _tmp520;_tmp520.f1=f,({void*_tmp5E7=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,elttype,0,(env->fenv)->unknown_all);_tmp520.f2=_tmp5E7;});_tmp520;});};}}}};}}else{
# 625
({void*_tmpA5=0U;({struct _fat_ptr _tmp5E8=({const char*_tmpA6="right deref of non-pointer-type";_tag_fat(_tmpA6,sizeof(char),32U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5E8,_tag_fat(_tmpA5,sizeof(void*),0));});});};}
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
{struct _tuple19 _tmpAE=({struct _tuple19 _tmp522;_tmp522.f1=inflow,_tmp522.f2=lval;_tmp522;});struct _tuple19 _stmttmp10=_tmpAE;struct _tuple19 _tmpAF=_stmttmp10;void*_tmpB2;void*_tmpB1;struct Cyc_Dict_Dict _tmpB0;if(((_tmpAF.f1).ReachableFL).tag == 2){if(((_tmpAF.f2).PlaceL).tag == 1){_tmpB0=(((_tmpAF.f1).ReachableFL).val).f1;_tmpB1=(((_tmpAF.f1).ReachableFL).val).f2;_tmpB2=((_tmpAF.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_tmpB0;struct Cyc_List_List*relns=_tmpB1;struct Cyc_CfFlowInfo_Place*p=_tmpB2;
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
({struct Cyc_String_pa_PrintArg_struct _tmpBB=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0,({struct _fat_ptr _tmp5E9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(exp));_tmp521.f1=_tmp5E9;});_tmp521;});void*_tmpB9[1];_tmpB9[0]=& _tmpBB;({struct _fat_ptr _tmp5EA=({const char*_tmpBA="noconsume parameter %s must be l-values";_tag_fat(_tmpBA,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5EA,_tag_fat(_tmpB9,sizeof(void*),1));});});
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
return({struct _tuple17 _tmp523;({union Cyc_CfFlowInfo_FlowInfo _tmp5EB=Cyc_CfFlowInfo_BottomFL();_tmp523.f1=_tmp5EB;}),_tmp523.f2=rval;_tmp523;});else{_tmpBD=((_tmpBC.ReachableFL).val).f1;_tmpBE=((_tmpBC.ReachableFL).val).f2;{struct Cyc_Dict_Dict indict=_tmpBD;struct Cyc_List_List*relns=_tmpBE;
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
({void*_tmpC6=0U;({unsigned _tmp5ED=lexp->loc;struct _fat_ptr _tmp5EC=({const char*_tmpC7="assignment may overwrite alias-free pointer(s)";_tag_fat(_tmpC7,sizeof(char),47U);});Cyc_Warn_warn(_tmp5ED,_tmp5EC,_tag_fat(_tmpC6,sizeof(void*),0));});});
goto _LLD;}_LLD:;}}
# 731
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple17 _tmp524;_tmp524.f1=outflow,_tmp524.f2=rval;_tmp524;});}}else{
# 737
return({struct _tuple17 _tmp525;({union Cyc_CfFlowInfo_FlowInfo _tmp5EE=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp525.f1=_tmp5EE;}),_tmp525.f2=rval;_tmp525;});};}};}
# 744
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned loc){
# 750
union Cyc_CfFlowInfo_FlowInfo _tmpC8=f;void*_tmpCA;struct Cyc_Dict_Dict _tmpC9;if((_tmpC8.BottomFL).tag == 1)
return Cyc_CfFlowInfo_BottomFL();else{_tmpC9=((_tmpC8.ReachableFL).val).f1;_tmpCA=((_tmpC8.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmpC9;struct Cyc_List_List*relns=_tmpCA;
# 755
outdict=({struct Cyc_CfFlowInfo_FlowEnv*_tmp5F3=fenv;unsigned _tmp5F2=loc;struct Cyc_Dict_Dict _tmp5F1=outdict;struct Cyc_CfFlowInfo_Place*_tmp5F0=({struct Cyc_CfFlowInfo_Place*_tmpCC=_cycalloc(sizeof(*_tmpCC));
({void*_tmp5EF=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=0,_tmpCB->f1=vd;_tmpCB;});_tmpCC->root=_tmp5EF;}),_tmpCC->path=0;_tmpCC;});
# 755
Cyc_CfFlowInfo_assign_place(_tmp5F3,_tmp5F2,_tmp5F1,_tmp5F0,rval);});
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
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0,({
struct _fat_ptr _tmp5F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(ve));_tmp526.f1=_tmp5F4;});_tmp526;});void*_tmpD9[1];_tmpD9[0]=& _tmpDB;({struct _fat_ptr _tmp5F5=({const char*_tmpDA="oops! pattern init expr %s has no type!\n";_tag_fat(_tmpDA,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp5F5,_tag_fat(_tmpD9,sizeof(void*),1));});});{
# 811
struct Cyc_List_List l=({struct Cyc_List_List _tmp528;_tmp528.hd=ve,_tmp528.tl=0;_tmp528;});
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
({struct Cyc_String_pa_PrintArg_struct _tmpEE=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0,({
struct _fat_ptr _tmp5F6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(ve));_tmp527.f1=_tmp5F6;});_tmp527;});void*_tmpEC[1];_tmpEC[0]=& _tmpEE;({unsigned _tmp5F8=exp_loc;struct _fat_ptr _tmp5F7=({const char*_tmpED="aliased pattern expression not an l-value: %s";_tag_fat(_tmpED,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp5F8,_tmp5F7,_tag_fat(_tmpEC,sizeof(void*),1));});});}_LL11:;}
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
return({struct Cyc_Absyn_Exp*_tmp5F9=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,e,0U);Cyc_Relations_exp2relnop(_tmp5F9,rop);});}default:
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
({struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0,_tmp52C.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp52C;});struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0,_tmp52B.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp52B;});struct Cyc_String_pa_PrintArg_struct _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0,_tmp52A.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp52A;});struct Cyc_String_pa_PrintArg_struct _tmp105=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0,({
struct _fat_ptr _tmp5FA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_relns2string(relns));_tmp529.f1=_tmp5FA;});_tmp529;});void*_tmp100[4];_tmp100[0]=& _tmp102,_tmp100[1]=& _tmp103,_tmp100[2]=& _tmp104,_tmp100[3]=& _tmp105;({unsigned _tmp5FC=loc;struct _fat_ptr _tmp5FB=({const char*_tmp101="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_fat(_tmp101,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp5FC,_tmp5FB,_tag_fat(_tmp100,sizeof(void*),4));});});
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
return({struct _tuple17 _tmp52D;({union Cyc_CfFlowInfo_FlowInfo _tmp5FD=Cyc_CfFlowInfo_BottomFL();_tmp52D.f1=_tmp5FD;}),_tmp52D.f2=fenv->unknown_all;_tmp52D;});else{_tmp108=((_tmp107.ReachableFL).val).f1;_tmp109=((_tmp107.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp108;struct Cyc_List_List*relns2=_tmp109;
d=d2;relns=relns2;}};}
# 953 "new_control_flow.cyc"
if((copy_ctxt && Cyc_NewControlFlow_is_local_var_rooted_path(e,0))&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 974 "new_control_flow.cyc"
struct _tuple19 _tmp10A=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple19 _stmttmp1A=_tmp10A;struct _tuple19 _tmp10B=_stmttmp1A;union Cyc_CfFlowInfo_AbsLVal _tmp10D;union Cyc_CfFlowInfo_FlowInfo _tmp10C;_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;{union Cyc_CfFlowInfo_FlowInfo flval=_tmp10C;union Cyc_CfFlowInfo_AbsLVal lval=_tmp10D;
struct _tuple19 _tmp10E=({struct _tuple19 _tmp531;_tmp531.f1=flval,_tmp531.f2=lval;_tmp531;});struct _tuple19 _stmttmp1B=_tmp10E;struct _tuple19 _tmp10F=_stmttmp1B;void*_tmp112;void*_tmp111;struct Cyc_Dict_Dict _tmp110;if(((_tmp10F.f1).ReachableFL).tag == 2){if(((_tmp10F.f2).PlaceL).tag == 1){_tmp110=(((_tmp10F.f1).ReachableFL).val).f1;_tmp111=(((_tmp10F.f1).ReachableFL).val).f2;_tmp112=((_tmp10F.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_tmp110;struct Cyc_List_List*r=_tmp111;struct Cyc_CfFlowInfo_Place*p=_tmp112;
# 977
void*_tmp113=Cyc_CfFlowInfo_lookup_place(fd,p);void*old_rval=_tmp113;
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,old_rval,& needs_unconsume)){
({void*_tmp114=0U;({unsigned _tmp5FF=e->loc;struct _fat_ptr _tmp5FE=({const char*_tmp115="expression attempts to copy a consumed alias-free value";_tag_fat(_tmp115,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp5FF,_tmp5FE,_tag_fat(_tmp114,sizeof(void*),0));});});
return({struct _tuple17 _tmp52E;({union Cyc_CfFlowInfo_FlowInfo _tmp600=Cyc_CfFlowInfo_BottomFL();_tmp52E.f1=_tmp600;}),_tmp52E.f2=fenv->unknown_all;_tmp52E;});}else{
# 983
if(needs_unconsume)
# 985
return({struct _tuple17 _tmp52F;_tmp52F.f1=flval,({void*_tmp601=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,old_rval);_tmp52F.f2=_tmp601;});_tmp52F;});else{
# 988
void*_tmp116=Cyc_CfFlowInfo_make_unique_consumed(fenv,(void*)_check_null(e->topt),e,env->iteration_num,old_rval,local_alias);void*new_rval=_tmp116;
struct Cyc_Dict_Dict _tmp117=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,new_rval);struct Cyc_Dict_Dict outdict=_tmp117;
# 1000
return({struct _tuple17 _tmp530;({union Cyc_CfFlowInfo_FlowInfo _tmp602=Cyc_CfFlowInfo_ReachableFL(outdict,r);_tmp530.f1=_tmp602;}),_tmp530.f2=old_rval;_tmp530;});}}}}else{goto _LLB;}}else{_LLB:
# 1004
 goto _LL8;}_LL8:;}}{
# 1007
void*_tmp118=e->r;void*_stmttmp1C=_tmp118;void*_tmp119=_stmttmp1C;enum Cyc_Absyn_AggrKind _tmp11E;int _tmp11F;void*_tmp11D;int _tmp11C;enum Cyc_Absyn_Primop _tmp120;void*_tmp11B;struct _fat_ptr _tmp121;void*_tmp11A;switch(*((int*)_tmp119)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp119)->f4 == Cyc_Absyn_Null_to_NonNull){_tmp11A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1011
struct _tuple17 _tmp122=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);struct _tuple17 _stmttmp1D=_tmp122;struct _tuple17 _tmp123=_stmttmp1D;void*_tmp125;union Cyc_CfFlowInfo_FlowInfo _tmp124;_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp124;void*r1=_tmp125;
struct _tuple17 _tmp126=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _stmttmp1E=_tmp126;struct _tuple17 _tmp127=_stmttmp1E;void*_tmp129;union Cyc_CfFlowInfo_FlowInfo _tmp128;_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp128;void*r2=_tmp129;
return({struct _tuple17 _tmp532;_tmp532.f1=f2,_tmp532.f2=r1;_tmp532;});}}}}else{_tmp11A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1017
_tmp11A=e1;goto _LL13;}}case 12: _tmp11A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp11A;
_tmp11A=e1;goto _LL15;}case 41: _tmp11A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL15: {struct Cyc_Absyn_Exp*e1=_tmp11A;
_tmp11A=e1;goto _LL17;}case 13: _tmp11A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp11A;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e1,local_alias);}case 2: _tmp121=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct _fat_ptr p=_tmp121;
# 1023
if(!({struct _fat_ptr _tmp603=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp603,({const char*_tmp12A="print_flow";_tag_fat(_tmp12A,sizeof(char),11U);}));})){
struct _fat_ptr _tmp12B=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp12B;
({struct Cyc_String_pa_PrintArg_struct _tmp12E=({struct Cyc_String_pa_PrintArg_struct _tmp533;_tmp533.tag=0,_tmp533.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp533;});void*_tmp12C[1];_tmp12C[0]=& _tmp12E;({struct Cyc___cycFILE*_tmp605=Cyc_stderr;struct _fat_ptr _tmp604=({const char*_tmp12D="%s: current flow is\n";_tag_fat(_tmp12D,sizeof(char),21U);});Cyc_fprintf(_tmp605,_tmp604,_tag_fat(_tmp12C,sizeof(void*),1));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp12F=0U;({struct Cyc___cycFILE*_tmp607=Cyc_stderr;struct _fat_ptr _tmp606=({const char*_tmp130="\n";_tag_fat(_tmp130,sizeof(char),2U);});Cyc_fprintf(_tmp607,_tmp606,_tag_fat(_tmp12F,sizeof(void*),0));});});}else{
# 1029
if(!({struct _fat_ptr _tmp608=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp608,({const char*_tmp131="debug_on";_tag_fat(_tmp131,sizeof(char),9U);}));}));else{
# 1034
if(!({struct _fat_ptr _tmp609=(struct _fat_ptr)p;Cyc_NewControlFlow_strcmp(_tmp609,({const char*_tmp132="debug_off";_tag_fat(_tmp132,sizeof(char),10U);}));}));else{
# 1040
({struct Cyc_String_pa_PrintArg_struct _tmp135=({struct Cyc_String_pa_PrintArg_struct _tmp534;_tmp534.tag=0,_tmp534.f1=(struct _fat_ptr)((struct _fat_ptr)p);_tmp534;});void*_tmp133[1];_tmp133[0]=& _tmp135;({unsigned _tmp60B=e->loc;struct _fat_ptr _tmp60A=({const char*_tmp134="unknown pragma %s";_tag_fat(_tmp134,sizeof(char),18U);});Cyc_Warn_warn(_tmp60B,_tmp60A,_tag_fat(_tmp133,sizeof(void*),1));});});}}}
return({struct _tuple17 _tmp535;_tmp535.f1=inflow,_tmp535.f2=fenv->zero;_tmp535;});}case 40:
# 1043
 goto _LL1D;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp119)->f1).Wstring_c).tag){case 1: _LL1D:
 goto _LL1F;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp119)->f1).Int_c).val).f2 == 0){_LL1F:
 return({struct _tuple17 _tmp536;_tmp536.f1=inflow,_tmp536.f2=fenv->zero;_tmp536;});}else{
goto _LL23;}case 8: _LL23:
 goto _LL25;case 9: _LL25:
 goto _LL27;default: _LL2B:
# 1052
 goto _LL2D;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)){case 2: _LL27:
# 1049
 return({struct _tuple17 _tmp537;_tmp537.f1=inflow,_tmp537.f2=fenv->notzeroall;_tmp537;});case 1:
# 1061
 return({struct _tuple17 _tmp538;_tmp538.f1=inflow,({void*_tmp60C=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp538.f2=_tmp60C;});_tmp538;});case 4: _tmp11A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp11A;
# 1066
if((int)vd->sc == (int)Cyc_Absyn_Static)
return({struct _tuple17 _tmp539;_tmp539.f1=inflow,({void*_tmp60D=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp539.f2=_tmp60D;});_tmp539;});
# 1069
_tmp11A=vd;goto _LL3D;}case 3: _tmp11A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL3D: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
_tmp11A=vd;goto _LL3F;}case 5: _tmp11A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp119)->f1)->f1;_LL3F: {struct Cyc_Absyn_Vardecl*vd=_tmp11A;
# 1073
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp53B;_tmp53B.tag=0,_tmp53B.f1=vd;_tmp53B;});
return({struct _tuple17 _tmp53A;_tmp53A.f1=inflow,({void*_tmp60F=({struct Cyc_Dict_Dict _tmp60E=d;((void*(*)(struct Cyc_Dict_Dict,int(*)(void*,void*),void*))Cyc_Dict_lookup_other)(_tmp60E,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp136=_cycalloc(sizeof(*_tmp136));*_tmp136=vdroot;_tmp136;}));});_tmp53A.f2=_tmp60F;});_tmp53A;});}default:
# 1677
 goto _LL7B;}case 31: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp119)->f1 == 0)
# 1051
goto _LL2B;else{_tmp11A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_List_List*es=_tmp11A;struct Cyc_Absyn_Datatypedecl*tud=_tmp11B;
# 1539
_tmp11A=es;goto _LL69;}}case 42: _LL2D:
# 1053
 goto _LL2F;case 18: _LL2F:
 goto _LL31;case 17: _LL31:
 goto _LL33;case 19: _LL33:
 goto _LL35;case 33: _LL35:
 goto _LL37;case 32: _LL37:
 return({struct _tuple17 _tmp53C;_tmp53C.f1=inflow,_tmp53C.f2=fenv->unknown_all;_tmp53C;});case 3: _tmp120=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{enum Cyc_Absyn_Primop p=_tmp120;struct Cyc_List_List*es=_tmp11A;
# 1080
struct _tuple17 _tmp137=Cyc_NewControlFlow_anal_use_ints(env,inflow,es);struct _tuple17 _stmttmp1F=_tmp137;struct _tuple17 _tmp138=_stmttmp1F;void*_tmp13A;union Cyc_CfFlowInfo_FlowInfo _tmp139;_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp139;void*r=_tmp13A;
return({struct _tuple17 _tmp53D;_tmp53D.f1=f,_tmp53D.f2=r;_tmp53D;});}}case 5: _tmp11A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1084
struct Cyc_List_List _tmp13B=({struct Cyc_List_List _tmp541;_tmp541.hd=e1,_tmp541.tl=0;_tmp541;});struct Cyc_List_List arg=_tmp13B;
struct _tuple17 _tmp13C=Cyc_NewControlFlow_anal_use_ints(env,inflow,& arg);struct _tuple17 _stmttmp20=_tmp13C;struct _tuple17 _tmp13D=_stmttmp20;union Cyc_CfFlowInfo_FlowInfo _tmp13E;_tmp13E=_tmp13D.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp13E;
struct _tuple19 _tmp13F=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,e1);struct _tuple19 _stmttmp21=_tmp13F;struct _tuple19 _tmp140=_stmttmp21;union Cyc_CfFlowInfo_AbsLVal _tmp141;_tmp141=_tmp140.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_tmp141;
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(e1->topt));
if(iszt){
# 1091
struct _tuple17 _tmp142=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp22=_tmp142;struct _tuple17 _tmp143=_stmttmp22;void*_tmp145;union Cyc_CfFlowInfo_FlowInfo _tmp144;_tmp144=_tmp143.f1;_tmp145=_tmp143.f2;{union Cyc_CfFlowInfo_FlowInfo g=_tmp144;void*r=_tmp145;
Cyc_NewControlFlow_anal_derefR(env,inflow,g,e1,r,0);}}
# 1094
{struct _tuple26 _tmp146=({struct _tuple26 _tmp53E;_tmp53E.f1=lval,_tmp53E.f2=f;_tmp53E;});struct _tuple26 _stmttmp23=_tmp146;struct _tuple26 _tmp147=_stmttmp23;void*_tmp14A;struct Cyc_Dict_Dict _tmp149;void*_tmp148;if(((_tmp147.f1).PlaceL).tag == 1){if(((_tmp147.f2).ReachableFL).tag == 2){_tmp148=((_tmp147.f1).PlaceL).val;_tmp149=(((_tmp147.f2).ReachableFL).val).f1;_tmp14A=(((_tmp147.f2).ReachableFL).val).f2;{struct Cyc_CfFlowInfo_Place*p=_tmp148;struct Cyc_Dict_Dict outdict=_tmp149;struct Cyc_List_List*relns=_tmp14A;
# 1096
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
f=({struct Cyc_Dict_Dict _tmp610=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,p,fenv->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp610,relns);});
# 1101
goto _LL92;}}else{goto _LL95;}}else{_LL95:
 goto _LL92;}_LL92:;}
# 1105
if(iszt)
return({struct _tuple17 _tmp53F;_tmp53F.f1=f,_tmp53F.f2=fenv->notzeroall;_tmp53F;});else{
return({struct _tuple17 _tmp540;_tmp540.f1=f,_tmp540.f2=fenv->unknown_all;_tmp540;});}}}}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f2 != 0){_tmp11A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*l=_tmp11A;struct Cyc_Absyn_Exp*r=_tmp11B;
# 1110
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp14B=0U;({unsigned _tmp612=e->loc;struct _fat_ptr _tmp611=({const char*_tmp14C="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp14C,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp612,_tmp611,_tag_fat(_tmp14B,sizeof(void*),0));});});
return({struct _tuple17 _tmp542;({union Cyc_CfFlowInfo_FlowInfo _tmp613=Cyc_CfFlowInfo_BottomFL();_tmp542.f1=_tmp613;}),_tmp542.f2=fenv->unknown_all;_tmp542;});}{
# 1114
struct _tuple17 _tmp14D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,r,local_alias);struct _tuple17 _stmttmp24=_tmp14D;struct _tuple17 _tmp14E=_stmttmp24;void*_tmp150;union Cyc_CfFlowInfo_FlowInfo _tmp14F;_tmp14F=_tmp14E.f1;_tmp150=_tmp14E.f2;{union Cyc_CfFlowInfo_FlowInfo right_out=_tmp14F;void*rval=_tmp150;
struct _tuple19 _tmp151=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,l);struct _tuple19 _stmttmp25=_tmp151;struct _tuple19 _tmp152=_stmttmp25;union Cyc_CfFlowInfo_AbsLVal _tmp154;union Cyc_CfFlowInfo_FlowInfo _tmp153;_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp153;union Cyc_CfFlowInfo_AbsLVal lval=_tmp154;
{union Cyc_CfFlowInfo_FlowInfo _tmp155=f;void*_tmp157;struct Cyc_Dict_Dict _tmp156;if((_tmp155.ReachableFL).tag == 2){_tmp156=((_tmp155.ReachableFL).val).f1;_tmp157=((_tmp155.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp156;struct Cyc_List_List*relns=_tmp157;
# 1118
{union Cyc_CfFlowInfo_AbsLVal _tmp158=lval;void*_tmp159;if((_tmp158.PlaceL).tag == 1){_tmp159=(_tmp158.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp159;
# 1122
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,l);
outdict=Cyc_CfFlowInfo_assign_place(fenv,l->loc,outdict,p,fenv->unknown_all);
# 1125
f=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
# 1129
goto _LLA2;}}else{
# 1132
goto _LLA2;}_LLA2:;}
# 1134
goto _LL9D;}}else{
goto _LL9D;}_LL9D:;}
# 1137
return({struct _tuple17 _tmp543;_tmp543.f1=f,_tmp543.f2=fenv->unknown_all;_tmp543;});}}}}}else{_tmp11A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1140
if(copy_ctxt && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp15A=0U;({unsigned _tmp615=e->loc;struct _fat_ptr _tmp614=({const char*_tmp15B="cannot track alias-free pointers through multiple assignments";_tag_fat(_tmp15B,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp615,_tmp614,_tag_fat(_tmp15A,sizeof(void*),0));});});
return({struct _tuple17 _tmp544;({union Cyc_CfFlowInfo_FlowInfo _tmp616=Cyc_CfFlowInfo_BottomFL();_tmp544.f1=_tmp616;}),_tmp544.f2=fenv->unknown_all;_tmp544;});}
# 1146
inflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp618=env;union Cyc_CfFlowInfo_FlowInfo _tmp617=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp618,_tmp617,({struct Cyc_Absyn_Exp*_tmp15C[2];_tmp15C[0]=e1,_tmp15C[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp15C,sizeof(struct Cyc_Absyn_Exp*),2));}));});{
# 1148
struct _tuple17 _tmp15D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);struct _tuple17 _stmttmp26=_tmp15D;struct _tuple17 _tmp15E=_stmttmp26;void*_tmp160;union Cyc_CfFlowInfo_FlowInfo _tmp15F;_tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;{union Cyc_CfFlowInfo_FlowInfo right_out=_tmp15F;void*rval=_tmp160;
struct _tuple19 _tmp161=Cyc_NewControlFlow_anal_Lexp(env,right_out,0,0,e1);struct _tuple19 _stmttmp27=_tmp161;struct _tuple19 _tmp162=_stmttmp27;union Cyc_CfFlowInfo_AbsLVal _tmp164;union Cyc_CfFlowInfo_FlowInfo _tmp163;_tmp163=_tmp162.f1;_tmp164=_tmp162.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_tmp163;union Cyc_CfFlowInfo_AbsLVal lval=_tmp164;
struct _tuple17 _tmp165=Cyc_NewControlFlow_do_assign(fenv,env,outflow,e1,lval,e2,rval,e->loc);struct _tuple17 _stmttmp28=_tmp165;struct _tuple17 _tmp166=_stmttmp28;void*_tmp168;union Cyc_CfFlowInfo_FlowInfo _tmp167;_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_tmp167;void*rval=_tmp168;
# 1154
return({struct _tuple17 _tmp545;_tmp545.f1=outflow,_tmp545.f2=rval;_tmp545;});}}}}}}case 9: _tmp11A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1157
struct _tuple17 _tmp169=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp29=_tmp169;struct _tuple17 _tmp16A=_stmttmp29;void*_tmp16C;union Cyc_CfFlowInfo_FlowInfo _tmp16B;_tmp16B=_tmp16A.f1;_tmp16C=_tmp16A.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp16B;void*r=_tmp16C;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f,e2,local_alias);}}case 11: _tmp11A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1161
struct _tuple17 _tmp16D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);struct _tuple17 _stmttmp2A=_tmp16D;struct _tuple17 _tmp16E=_stmttmp2A;void*_tmp170;union Cyc_CfFlowInfo_FlowInfo _tmp16F;_tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp16F;void*r=_tmp170;
Cyc_NewControlFlow_use_Rval(env,e1->loc,f,r);
return({struct _tuple17 _tmp546;({union Cyc_CfFlowInfo_FlowInfo _tmp61A=Cyc_CfFlowInfo_BottomFL();_tmp546.f1=_tmp61A;}),({void*_tmp619=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp546.f2=_tmp619;});_tmp546;});}}case 10: _tmp11A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_List_List*es=_tmp11B;
# 1166
struct Cyc_List_List*_tmp171=es;struct Cyc_List_List*orig_es=_tmp171;
struct _tuple17 _tmp172=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp2B=_tmp172;struct _tuple17 _tmp173=_stmttmp2B;void*_tmp175;union Cyc_CfFlowInfo_FlowInfo _tmp174;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp174;void*r1=_tmp175;
# 1169
Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);{
# 1172
struct _tuple24 _tmp176=({struct Cyc_NewControlFlow_AnalEnv*_tmp61C=env;union Cyc_CfFlowInfo_FlowInfo _tmp61B=f1;Cyc_NewControlFlow_anal_Rexps(_tmp61C,_tmp61B,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_copy)(es),1,1);});struct _tuple24 _stmttmp2C=_tmp176;struct _tuple24 _tmp177=_stmttmp2C;void*_tmp179;union Cyc_CfFlowInfo_FlowInfo _tmp178;_tmp178=_tmp177.f1;_tmp179=_tmp177.f2;{union Cyc_CfFlowInfo_FlowInfo fst_outflow=_tmp178;struct Cyc_List_List*rvals=_tmp179;
# 1174
union Cyc_CfFlowInfo_FlowInfo _tmp17A=Cyc_NewControlFlow_use_Rval(env,e1->loc,fst_outflow,r1);union Cyc_CfFlowInfo_FlowInfo outflow=_tmp17A;
# 1176
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*consume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp17B=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp2D=_tmp17B;void*_tmp17C=_stmttmp2D;void*_tmp17D;if(*((int*)_tmp17C)== 3){_tmp17D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17C)->f1).elt_type;{void*t=_tmp17D;
# 1183
{void*_tmp17E=Cyc_Absyn_compress(t);void*_stmttmp2E=_tmp17E;void*_tmp17F=_stmttmp2E;void*_tmp182;void*_tmp181;void*_tmp180;if(*((int*)_tmp17F)== 5){_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17F)->f1).attributes;_tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17F)->f1).requires_relns;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17F)->f1).ensures_relns;{struct Cyc_List_List*atts=_tmp180;struct Cyc_List_List*req=_tmp181;struct Cyc_List_List*ens=_tmp182;
# 1185
requires=req;
ensures=ens;
for(1;atts != 0;atts=atts->tl){
# 1189
void*_tmp183=(void*)atts->hd;void*_stmttmp2F=_tmp183;void*_tmp184=_stmttmp2F;int _tmp185;switch(*((int*)_tmp184)){case 20: _tmp185=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp184)->f1;{int i=_tmp185;
# 1191
init_params=({struct Cyc_List_List*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->hd=(void*)i,_tmp186->tl=init_params;_tmp186;});goto _LLC6;}case 21: _tmp185=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp184)->f1;{int i=_tmp185;
# 1193
nolive_unique_params=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->hd=(void*)i,_tmp187->tl=nolive_unique_params;_tmp187;});
consume_params=({struct Cyc_List_List*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->hd=(void*)i,_tmp188->tl=consume_params;_tmp188;});
goto _LLC6;}case 22: _tmp185=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp184)->f1;{int i=_tmp185;
# 1197
consume_params=({struct Cyc_List_List*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->hd=(void*)i,_tmp189->tl=consume_params;_tmp189;});
goto _LLC6;}default:
 goto _LLC6;}_LLC6:;}
# 1201
goto _LLC1;}}else{
({void*_tmp18A=0U;({struct _fat_ptr _tmp61D=({const char*_tmp18B="anal_Rexp: bad function type";_tag_fat(_tmp18B,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp61D,_tag_fat(_tmp18A,sizeof(void*),0));});});}_LLC1:;}
# 1204
goto _LLBC;}}else{
({void*_tmp18C=0U;({struct _fat_ptr _tmp61E=({const char*_tmp18D="anal_Rexp: bad function type";_tag_fat(_tmp18D,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp61E,_tag_fat(_tmp18C,sizeof(void*),0));});});}_LLBC:;}
# 1207
{int i=1;for(0;rvals != 0;(rvals=rvals->tl,es=((struct Cyc_List_List*)_check_null(es))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp18E=fst_outflow;struct Cyc_Dict_Dict _tmp18F;if((_tmp18E.BottomFL).tag == 1)
goto _LLCF;else{_tmp18F=((_tmp18E.ReachableFL).val).f1;{struct Cyc_Dict_Dict fst_d=_tmp18F;
# 1212
struct _tuple16 _tmp190=Cyc_CfFlowInfo_unname_rval((void*)rvals->hd);struct _tuple16 _stmttmp30=_tmp190;struct _tuple16 _tmp191=_stmttmp30;void*_tmp192;_tmp192=_tmp191.f1;{void*r=_tmp192;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,fst_d,(void*)rvals->hd)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer((void*)rvals->hd))
({void*_tmp193=0U;({unsigned _tmp620=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp61F=({const char*_tmp194="expression may not be initialized";_tag_fat(_tmp194,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp620,_tmp61F,_tag_fat(_tmp193,sizeof(void*),0));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp195=outflow;void*_tmp197;struct Cyc_Dict_Dict _tmp196;if((_tmp195.BottomFL).tag == 1)
goto _LLD7;else{_tmp196=((_tmp195.ReachableFL).val).f1;_tmp197=((_tmp195.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp196;struct Cyc_List_List*relns=_tmp197;
# 1220
struct Cyc_Dict_Dict _tmp198=Cyc_CfFlowInfo_escape_deref(fenv,d,(void*)rvals->hd);struct Cyc_Dict_Dict ans_d=_tmp198;
{void*_tmp199=(void*)rvals->hd;void*_stmttmp31=_tmp199;void*_tmp19A=_stmttmp31;void*_tmp19B;switch(*((int*)_tmp19A)){case 4: _tmp19B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp19A)->f1;{struct Cyc_CfFlowInfo_Place*p=_tmp19B;
# 1223
{void*_tmp19C=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->topt));void*_stmttmp32=_tmp19C;void*_tmp19D=_stmttmp32;void*_tmp19E;if(*((int*)_tmp19D)== 3){_tmp19E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19D)->f1).elt_type;{void*t=_tmp19E;
# 1225
ans_d=({struct Cyc_CfFlowInfo_FlowEnv*_tmp624=fenv;unsigned _tmp623=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct Cyc_Dict_Dict _tmp622=ans_d;struct Cyc_CfFlowInfo_Place*_tmp621=p;Cyc_CfFlowInfo_assign_place(_tmp624,_tmp623,_tmp622,_tmp621,
Cyc_CfFlowInfo_typ_to_absrval(fenv,t,0,fenv->esc_all));});
# 1229
goto _LLE3;}}else{
({void*_tmp19F=0U;({struct _fat_ptr _tmp625=({const char*_tmp1A0="anal_Rexp:bad type for initialized arg";_tag_fat(_tmp1A0,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp625,_tag_fat(_tmp19F,sizeof(void*),0));});});}_LLE3:;}
# 1232
goto _LLDC;}case 5:
# 1234
({void*_tmp1A1=0U;({struct _fat_ptr _tmp626=({const char*_tmp1A2="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_fat(_tmp1A2,sizeof(char),68U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp626,_tag_fat(_tmp1A1,sizeof(void*),0));});});default:
# 1236
 goto _LLDC;}_LLDC:;}
# 1238
outflow=Cyc_CfFlowInfo_ReachableFL(ans_d,relns);
goto _LLD7;}}_LLD7:;}
# 1241
goto _LLCF;}}}_LLCF:;}else{
# 1244
if(((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(nolive_unique_params,i))
# 1249
outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp62A=env;unsigned _tmp629=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;void*_tmp628=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp627=outflow;Cyc_NewControlFlow_use_nounique_Rval(_tmp62A,_tmp629,_tmp628,_tmp627,(void*)rvals->hd);});else{
# 1259
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);
if(!((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(consume_params,i)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),0))
outflow=Cyc_NewControlFlow_restore_noconsume_arg(env,outflow,(struct Cyc_Absyn_Exp*)es->hd,((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)rvals->hd);}}}}
# 1266
Cyc_NewControlFlow_check_fn_requires(env,outflow,orig_es,requires,e->loc);
# 1269
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e1->topt)))
return({struct _tuple17 _tmp547;({union Cyc_CfFlowInfo_FlowInfo _tmp62C=Cyc_CfFlowInfo_BottomFL();_tmp547.f1=_tmp62C;}),({void*_tmp62B=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp547.f2=_tmp62B;});_tmp547;});else{
# 1272
return({struct _tuple17 _tmp548;_tmp548.f1=outflow,({void*_tmp62D=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp548.f2=_tmp62D;});_tmp548;});}}}}}case 34: _tmp11C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).is_calloc;_tmp11A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).rgn;_tmp11B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).elt_type;_tmp11D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).num_elts;_tmp11F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp119)->f1).fat_result;{int iscalloc=_tmp11C;struct Cyc_Absyn_Exp*eopt=_tmp11A;void**topt=_tmp11B;struct Cyc_Absyn_Exp*exp=_tmp11D;int isfat=_tmp11F;
# 1275
void*place_val;
if(isfat)place_val=fenv->notzeroall;else{
if(iscalloc)place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*((void**)_check_null(topt)),0,fenv->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,*((void**)_check_null(topt)),0,fenv->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(eopt != 0){
struct _tuple24 _tmp1A3=({struct Cyc_NewControlFlow_AnalEnv*_tmp62F=env;union Cyc_CfFlowInfo_FlowInfo _tmp62E=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp62F,_tmp62E,({struct Cyc_Absyn_Exp*_tmp1A7[2];_tmp1A7[0]=eopt,_tmp1A7[1]=exp;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1A7,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple24 _stmttmp33=_tmp1A3;struct _tuple24 _tmp1A4=_stmttmp33;void*_tmp1A6;union Cyc_CfFlowInfo_FlowInfo _tmp1A5;_tmp1A5=_tmp1A4.f1;_tmp1A6=_tmp1A4.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1A5;struct Cyc_List_List*rvals=_tmp1A6;
outflow=f;}}else{
# 1284
struct _tuple17 _tmp1A8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);struct _tuple17 _stmttmp34=_tmp1A8;struct _tuple17 _tmp1A9=_stmttmp34;union Cyc_CfFlowInfo_FlowInfo _tmp1AA;_tmp1AA=_tmp1A9.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1AA;
outflow=f;}}
# 1289
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple17 _tmp549;_tmp549.f1=outflow,({void*_tmp630=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->tag=5,_tmp1AB->f1=place_val;_tmp1AB;});_tmp549.f2=_tmp630;});_tmp549;});else{
# 1293
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->tag=1,_tmp1B1->f1=exp,_tmp1B1->f2=(void*)_check_null(e->topt);_tmp1B1;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->root=root,_tmp1B0->path=0;_tmp1B0;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->tag=4,_tmp1AF->f1=place;_tmp1AF;});
union Cyc_CfFlowInfo_FlowInfo _tmp1AC=outflow;void*_tmp1AE;struct Cyc_Dict_Dict _tmp1AD;if((_tmp1AC.BottomFL).tag == 1)
return({struct _tuple17 _tmp54A;_tmp54A.f1=outflow,_tmp54A.f2=rval;_tmp54A;});else{_tmp1AD=((_tmp1AC.ReachableFL).val).f1;_tmp1AE=((_tmp1AC.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp1AD;struct Cyc_List_List*relns=_tmp1AE;
# 1299
return({struct _tuple17 _tmp54B;({union Cyc_CfFlowInfo_FlowInfo _tmp632=({struct Cyc_Dict_Dict _tmp631=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp631,relns);});_tmp54B.f1=_tmp632;}),_tmp54B.f2=rval;_tmp54B;});}};}}}case 35: _tmp11A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1304
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1310
struct _tuple24 _tmp1B2=({struct Cyc_NewControlFlow_AnalEnv*_tmp634=env;union Cyc_CfFlowInfo_FlowInfo _tmp633=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp634,_tmp633,({struct Cyc_Absyn_Exp*_tmp1CD[2];_tmp1CD[0]=e1,_tmp1CD[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CD,sizeof(struct Cyc_Absyn_Exp*),2));}),0,0);});struct _tuple24 _stmttmp35=_tmp1B2;struct _tuple24 _tmp1B3=_stmttmp35;void*_tmp1B5;union Cyc_CfFlowInfo_FlowInfo _tmp1B4;_tmp1B4=_tmp1B3.f1;_tmp1B5=_tmp1B3.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1B4;struct Cyc_List_List*rvals=_tmp1B5;
left_rval=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(rvals->tl))->hd;
outflow=f;{
# 1316
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1B6=right_rval;if(*((int*)_tmp1B6)== 0)
goto _LLF6;else{
({void*_tmp1B7=0U;({unsigned _tmp636=e1->loc;struct _fat_ptr _tmp635=({const char*_tmp1B8="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp1B8,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp636,_tmp635,_tag_fat(_tmp1B7,sizeof(void*),0));});});}_LLF6:;}else{
# 1327
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1B9=left_rval;if(*((int*)_tmp1B9)== 0)
goto _LLFB;else{
({void*_tmp1BA=0U;({unsigned _tmp638=e2->loc;struct _fat_ptr _tmp637=({const char*_tmp1BB="cannot swap value into zeroterm array not known to be 0";_tag_fat(_tmp1BB,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp638,_tmp637,_tag_fat(_tmp1BA,sizeof(void*),0));});});}_LLFB:;}}{
# 1335
struct _tuple19 _tmp1BC=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e1);struct _tuple19 _stmttmp36=_tmp1BC;struct _tuple19 _tmp1BD=_stmttmp36;union Cyc_CfFlowInfo_AbsLVal _tmp1BE;_tmp1BE=_tmp1BD.f2;{union Cyc_CfFlowInfo_AbsLVal left_lval=_tmp1BE;
struct _tuple19 _tmp1BF=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,e2);struct _tuple19 _stmttmp37=_tmp1BF;struct _tuple19 _tmp1C0=_stmttmp37;union Cyc_CfFlowInfo_AbsLVal _tmp1C1;_tmp1C1=_tmp1C0.f2;{union Cyc_CfFlowInfo_AbsLVal right_lval=_tmp1C1;
{union Cyc_CfFlowInfo_FlowInfo _tmp1C2=outflow;void*_tmp1C4;struct Cyc_Dict_Dict _tmp1C3;if((_tmp1C2.ReachableFL).tag == 2){_tmp1C3=((_tmp1C2.ReachableFL).val).f1;_tmp1C4=((_tmp1C2.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp1C3;struct Cyc_List_List*relns=_tmp1C4;
# 1339
{union Cyc_CfFlowInfo_AbsLVal _tmp1C5=left_lval;void*_tmp1C6;if((_tmp1C5.PlaceL).tag == 1){_tmp1C6=(_tmp1C5.PlaceL).val;{struct Cyc_CfFlowInfo_Place*lp=_tmp1C6;
# 1341
outdict=Cyc_CfFlowInfo_assign_place(fenv,e1->loc,outdict,lp,right_rval);
goto _LL10B;}}else{
# 1347
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,right_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp1C7=0U;({unsigned _tmp63A=e2->loc;struct _fat_ptr _tmp639=({const char*_tmp1C8="expression may not be fully initialized";_tag_fat(_tmp1C8,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp63A,_tmp639,_tag_fat(_tmp1C7,sizeof(void*),0));});});
goto _LL10B;}_LL10B:;}
# 1351
{union Cyc_CfFlowInfo_AbsLVal _tmp1C9=right_lval;void*_tmp1CA;if((_tmp1C9.PlaceL).tag == 1){_tmp1CA=(_tmp1C9.PlaceL).val;{struct Cyc_CfFlowInfo_Place*rp=_tmp1CA;
# 1353
outdict=Cyc_CfFlowInfo_assign_place(fenv,e2->loc,outdict,rp,left_rval);
goto _LL110;}}else{
# 1356
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,left_rval)!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp1CB=0U;({unsigned _tmp63C=e1->loc;struct _fat_ptr _tmp63B=({const char*_tmp1CC="expression may not be fully initialized";_tag_fat(_tmp1CC,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp63C,_tmp63B,_tag_fat(_tmp1CB,sizeof(void*),0));});});
goto _LL110;}_LL110:;}
# 1361
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e1);
relns=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,relns,e2);
# 1364
outflow=Cyc_CfFlowInfo_ReachableFL(outdict,relns);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL106;}}else{
goto _LL106;}_LL106:;}
# 1371
return({struct _tuple17 _tmp54C;_tmp54C.f1=outflow,_tmp54C.f2=fenv->unknown_all;_tmp54C;});}}}}}}case 16: _tmp11A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*eopt=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1374
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(eopt != 0){
struct _tuple24 _tmp1CE=({struct Cyc_NewControlFlow_AnalEnv*_tmp63E=env;union Cyc_CfFlowInfo_FlowInfo _tmp63D=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp63E,_tmp63D,({struct Cyc_Absyn_Exp*_tmp1D2[2];_tmp1D2[0]=eopt,_tmp1D2[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D2,sizeof(struct Cyc_Absyn_Exp*),2));}),1,1);});struct _tuple24 _stmttmp38=_tmp1CE;struct _tuple24 _tmp1CF=_stmttmp38;void*_tmp1D1;union Cyc_CfFlowInfo_FlowInfo _tmp1D0;_tmp1D0=_tmp1CF.f1;_tmp1D1=_tmp1CF.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1D0;struct Cyc_List_List*rvals=_tmp1D1;
outflow=f;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd;}}else{
# 1381
struct _tuple17 _tmp1D3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e2,local_alias);struct _tuple17 _stmttmp39=_tmp1D3;struct _tuple17 _tmp1D4=_stmttmp39;void*_tmp1D6;union Cyc_CfFlowInfo_FlowInfo _tmp1D5;_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1D5;void*r=_tmp1D6;
outflow=f;
place_val=r;}}
# 1386
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple17 _tmp54D;_tmp54D.f1=outflow,({void*_tmp63F=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->tag=5,_tmp1D7->f1=place_val;_tmp1D7;});_tmp54D.f2=_tmp63F;});_tmp54D;});else{
# 1390
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->tag=1,_tmp1DD->f1=e2,_tmp1DD->f2=(void*)_check_null(e->topt);_tmp1DD;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->root=root,_tmp1DC->path=0;_tmp1DC;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->tag=4,_tmp1DB->f1=place;_tmp1DB;});
union Cyc_CfFlowInfo_FlowInfo _tmp1D8=outflow;void*_tmp1DA;struct Cyc_Dict_Dict _tmp1D9;if((_tmp1D8.BottomFL).tag == 1)
return({struct _tuple17 _tmp54E;_tmp54E.f1=outflow,_tmp54E.f2=rval;_tmp54E;});else{_tmp1D9=((_tmp1D8.ReachableFL).val).f1;_tmp1DA=((_tmp1D8.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp1D9;struct Cyc_List_List*relns=_tmp1DA;
# 1396
return({struct _tuple17 _tmp54F;({union Cyc_CfFlowInfo_FlowInfo _tmp641=({struct Cyc_Dict_Dict _tmp640=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp640,relns);});_tmp54F.f1=_tmp641;}),_tmp54F.f2=rval;_tmp54F;});}};}}case 15: _tmp11A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1401
struct _tuple19 _tmp1DE=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e1);struct _tuple19 _stmttmp3A=_tmp1DE;struct _tuple19 _tmp1DF=_stmttmp3A;union Cyc_CfFlowInfo_AbsLVal _tmp1E1;union Cyc_CfFlowInfo_FlowInfo _tmp1E0;_tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1E0;union Cyc_CfFlowInfo_AbsLVal l=_tmp1E1;
union Cyc_CfFlowInfo_AbsLVal _tmp1E2=l;void*_tmp1E3;if((_tmp1E2.UnknownL).tag == 2)
return({struct _tuple17 _tmp550;_tmp550.f1=f,_tmp550.f2=fenv->notzeroall;_tmp550;});else{_tmp1E3=(_tmp1E2.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp1E3;
return({struct _tuple17 _tmp551;_tmp551.f1=f,({void*_tmp642=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->tag=4,_tmp1E4->f1=p;_tmp1E4;});_tmp551.f2=_tmp642;});_tmp551;});}};}}case 20: _tmp11A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Exp*e1=_tmp11A;
# 1408
struct _tuple17 _tmp1E5=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp3B=_tmp1E5;struct _tuple17 _tmp1E6=_stmttmp3B;void*_tmp1E8;union Cyc_CfFlowInfo_FlowInfo _tmp1E7;_tmp1E7=_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1E7;void*r=_tmp1E8;
return Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,r,0);}}case 21: _tmp11A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*field=_tmp11B;
# 1412
struct _tuple17 _tmp1E9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp3C=_tmp1E9;struct _tuple17 _tmp1EA=_stmttmp3C;void*_tmp1EC;union Cyc_CfFlowInfo_FlowInfo _tmp1EB;_tmp1EB=_tmp1EA.f1;_tmp1EC=_tmp1EA.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1EB;void*r=_tmp1EC;
void*_tmp1ED=(void*)_check_null(e1->topt);void*e1_type=_tmp1ED;
# 1415
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple17 _tmp552;_tmp552.f1=f,({void*_tmp643=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp552.f2=_tmp643;});_tmp552;});
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,field,f);{
struct _tuple16 _tmp1EE=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp3D=_tmp1EE;struct _tuple16 _tmp1EF=_stmttmp3D;void*_tmp1F0;_tmp1F0=_tmp1EF.f1;{void*r=_tmp1F0;
void*_tmp1F1=r;struct _fat_ptr _tmp1F4;int _tmp1F3;int _tmp1F2;if(*((int*)_tmp1F1)== 6){_tmp1F2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F1)->f1).is_union;_tmp1F3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F1)->f1).fieldnum;_tmp1F4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F1)->f2;{int is_union=_tmp1F2;int fnum=_tmp1F3;struct _fat_ptr rdict=_tmp1F4;
# 1422
int _tmp1F5=Cyc_CfFlowInfo_get_field_index((void*)_check_null(e1->topt),field);int field_no=_tmp1F5;
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _tmp553;_tmp553.f1=f,({void*_tmp644=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),1,fenv->unknown_none);_tmp553.f2=_tmp644;});_tmp553;});
return({struct _tuple17 _tmp554;_tmp554.f1=f,_tmp554.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_tmp554;});}}else{
# 1427
({void*_tmp1F6=0U;({struct Cyc___cycFILE*_tmp646=Cyc_stderr;struct _fat_ptr _tmp645=({const char*_tmp1F7="the bad rexp is :";_tag_fat(_tmp1F7,sizeof(char),18U);});Cyc_fprintf(_tmp646,_tmp645,_tag_fat(_tmp1F6,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(r);
({struct Cyc_String_pa_PrintArg_struct _tmp1FA=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0,({struct _fat_ptr _tmp647=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp555.f1=_tmp647;});_tmp555;});void*_tmp1F8[1];_tmp1F8[0]=& _tmp1FA;({struct _fat_ptr _tmp648=({const char*_tmp1F9="\nanal_Rexp: AggrMember: %s";_tag_fat(_tmp1F9,sizeof(char),27U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp648,_tag_fat(_tmp1F8,sizeof(void*),1));});});};}}}}case 38: _tmp11A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*field=_tmp11B;
# 1435
struct _tuple17 _tmp1FB=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp3E=_tmp1FB;struct _tuple17 _tmp1FC=_stmttmp3E;void*_tmp1FE;union Cyc_CfFlowInfo_FlowInfo _tmp1FD;_tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp1FD;void*r=_tmp1FE;
# 1437
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(e1->topt)))
return({struct _tuple17 _tmp556;_tmp556.f1=f,_tmp556.f2=fenv->unknown_all;_tmp556;});{
struct _tuple16 _tmp1FF=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp3F=_tmp1FF;struct _tuple16 _tmp200=_stmttmp3F;void*_tmp201;_tmp201=_tmp200.f1;{void*r=_tmp201;
void*_tmp202=r;struct _fat_ptr _tmp205;int _tmp204;int _tmp203;if(*((int*)_tmp202)== 6){_tmp203=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp202)->f1).is_union;_tmp204=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp202)->f1).fieldnum;_tmp205=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp202)->f2;{int is_union=_tmp203;int fnum=_tmp204;struct _fat_ptr rdict=_tmp205;
# 1442
int _tmp206=Cyc_CfFlowInfo_get_field_index((void*)_check_null(e1->topt),field);int field_no=_tmp206;
if(is_union && fnum != -1){
if(fnum != field_no)
return({struct _tuple17 _tmp557;_tmp557.f1=f,_tmp557.f2=fenv->zero;_tmp557;});else{
# 1447
return({struct _tuple17 _tmp558;_tmp558.f1=f,_tmp558.f2=fenv->notzeroall;_tmp558;});}}else{
# 1449
return({struct _tuple17 _tmp559;_tmp559.f1=f,_tmp559.f2=fenv->unknown_all;_tmp559;});}}}else{
# 1451
({struct Cyc_String_pa_PrintArg_struct _tmp209=({struct Cyc_String_pa_PrintArg_struct _tmp55A;_tmp55A.tag=0,({struct _fat_ptr _tmp649=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp55A.f1=_tmp649;});_tmp55A;});void*_tmp207[1];_tmp207[0]=& _tmp209;({struct _fat_ptr _tmp64A=({const char*_tmp208="anal_Rexp: TagCheck_e: %s";_tag_fat(_tmp208,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp64A,_tag_fat(_tmp207,sizeof(void*),1));});});};}}}}case 22: _tmp11A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct _fat_ptr*field=_tmp11B;
# 1455
struct _tuple17 _tmp20A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,local_alias);struct _tuple17 _stmttmp40=_tmp20A;struct _tuple17 _tmp20B=_stmttmp40;void*_tmp20D;union Cyc_CfFlowInfo_FlowInfo _tmp20C;_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp20C;void*r1=_tmp20D;
# 1458
struct _tuple17 _tmp20E=Cyc_NewControlFlow_anal_derefR(env,inflow,f1,e1,r1,0);struct _tuple17 _stmttmp41=_tmp20E;struct _tuple17 _tmp20F=_stmttmp41;void*_tmp211;union Cyc_CfFlowInfo_FlowInfo _tmp210;_tmp210=_tmp20F.f1;_tmp211=_tmp20F.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp210;void*r2=_tmp211;
# 1461
void*_tmp212=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp42=_tmp212;void*_tmp213=_stmttmp42;void*_tmp214;if(*((int*)_tmp213)== 3){_tmp214=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213)->f1).elt_type;{void*t2=_tmp214;
# 1463
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t2))
# 1465
return({struct _tuple17 _tmp55B;_tmp55B.f1=f2,({void*_tmp64B=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->unknown_all);_tmp55B.f2=_tmp64B;});_tmp55B;});
# 1467
if(Cyc_Absyn_is_require_union_type(t2))
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,field,f1);{
# 1470
struct _tuple16 _tmp215=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _stmttmp43=_tmp215;struct _tuple16 _tmp216=_stmttmp43;void*_tmp217;_tmp217=_tmp216.f1;{void*r2=_tmp217;
void*_tmp218=r2;struct _fat_ptr _tmp21B;int _tmp21A;int _tmp219;if(*((int*)_tmp218)== 6){_tmp219=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp218)->f1).is_union;_tmp21A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp218)->f1).fieldnum;_tmp21B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp218)->f2;{int is_union=_tmp219;int fnum=_tmp21A;struct _fat_ptr rdict=_tmp21B;
# 1473
int _tmp21C=Cyc_CfFlowInfo_get_field_index(t2,field);int field_no=_tmp21C;
if((is_union && fnum != -1)&& fnum != field_no)
return({struct _tuple17 _tmp55C;_tmp55C.f1=f2,({void*_tmp64C=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),1,fenv->unknown_none);_tmp55C.f2=_tmp64C;});_tmp55C;});
return({struct _tuple17 _tmp55D;_tmp55D.f1=f2,_tmp55D.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),field_no));_tmp55D;});}}else{
({void*_tmp21D=0U;({struct _fat_ptr _tmp64D=({const char*_tmp21E="anal_Rexp: AggrArrow";_tag_fat(_tmp21E,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp64D,_tag_fat(_tmp21D,sizeof(void*),0));});});};}}}}else{
# 1479
({void*_tmp21F=0U;({struct _fat_ptr _tmp64E=({const char*_tmp220="anal_Rexp: AggrArrow ptr";_tag_fat(_tmp220,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp64E,_tag_fat(_tmp21F,sizeof(void*),0));});});};}}}case 6: _tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp119)->f2;_tmp11D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;struct Cyc_Absyn_Exp*e3=_tmp11D;
# 1483
struct _tuple20 _tmp221=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp44=_tmp221;struct _tuple20 _tmp222=_stmttmp44;union Cyc_CfFlowInfo_FlowInfo _tmp224;union Cyc_CfFlowInfo_FlowInfo _tmp223;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp223;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp224;
struct _tuple17 _tmp225=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);struct _tuple17 pr2=_tmp225;
struct _tuple17 _tmp226=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e3,local_alias);struct _tuple17 pr3=_tmp226;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}case 7: _tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1489
struct _tuple20 _tmp227=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp45=_tmp227;struct _tuple20 _tmp228=_stmttmp45;union Cyc_CfFlowInfo_FlowInfo _tmp22A;union Cyc_CfFlowInfo_FlowInfo _tmp229;_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp229;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp22A;
struct _tuple17 _tmp22B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1t,e2,local_alias);struct _tuple17 _stmttmp46=_tmp22B;struct _tuple17 _tmp22C=_stmttmp46;void*_tmp22E;union Cyc_CfFlowInfo_FlowInfo _tmp22D;_tmp22D=_tmp22C.f1;_tmp22E=_tmp22C.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp22D;void*f2r=_tmp22E;
struct _tuple17 _tmp22F=({struct _tuple17 _tmp55F;_tmp55F.f1=f2t,_tmp55F.f2=f2r;_tmp55F;});struct _tuple17 pr2=_tmp22F;
struct _tuple17 _tmp230=({struct _tuple17 _tmp55E;_tmp55E.f1=f1f,_tmp55E.f2=fenv->zero;_tmp55E;});struct _tuple17 pr3=_tmp230;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 8: _tmp11A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1496
struct _tuple20 _tmp231=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp47=_tmp231;struct _tuple20 _tmp232=_stmttmp47;union Cyc_CfFlowInfo_FlowInfo _tmp234;union Cyc_CfFlowInfo_FlowInfo _tmp233;_tmp233=_tmp232.f1;_tmp234=_tmp232.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp233;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp234;
struct _tuple17 _tmp235=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,f1f,e2,local_alias);struct _tuple17 _stmttmp48=_tmp235;struct _tuple17 _tmp236=_stmttmp48;void*_tmp238;union Cyc_CfFlowInfo_FlowInfo _tmp237;_tmp237=_tmp236.f1;_tmp238=_tmp236.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp237;void*f2r=_tmp238;
struct _tuple17 _tmp239=({struct _tuple17 _tmp561;_tmp561.f1=f2t,_tmp561.f2=f2r;_tmp561;});struct _tuple17 pr2=_tmp239;
struct _tuple17 _tmp23A=({struct _tuple17 _tmp560;_tmp560.f1=f1t,_tmp560.f2=fenv->notzeroall;_tmp560;});struct _tuple17 pr3=_tmp23A;
return Cyc_CfFlowInfo_join_flow_and_rval(fenv,pr2,pr3);}}}case 23: _tmp11A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 1503
struct _tuple24 _tmp23B=({struct Cyc_NewControlFlow_AnalEnv*_tmp650=env;union Cyc_CfFlowInfo_FlowInfo _tmp64F=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp650,_tmp64F,({struct Cyc_Absyn_Exp*_tmp257[2];_tmp257[0]=e1,_tmp257[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp257,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple24 _stmttmp49=_tmp23B;struct _tuple24 _tmp23C=_stmttmp49;void*_tmp23E;union Cyc_CfFlowInfo_FlowInfo _tmp23D;_tmp23D=_tmp23C.f1;_tmp23E=_tmp23C.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp23D;struct Cyc_List_List*rvals=_tmp23E;
# 1507
union Cyc_CfFlowInfo_FlowInfo _tmp23F=f;union Cyc_CfFlowInfo_FlowInfo f2=_tmp23F;
{union Cyc_CfFlowInfo_FlowInfo _tmp240=f;void*_tmp242;struct Cyc_Dict_Dict _tmp241;if((_tmp240.ReachableFL).tag == 2){_tmp241=((_tmp240.ReachableFL).val).f1;_tmp242=((_tmp240.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp241;struct Cyc_List_List*relns=_tmp242;
# 1512
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp243=0U;({unsigned _tmp652=e2->loc;struct _fat_ptr _tmp651=({const char*_tmp244="expression may not be initialized";_tag_fat(_tmp244,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp652,_tmp651,_tag_fat(_tmp243,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp245=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);struct Cyc_List_List*new_relns=_tmp245;
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL166;}}}else{
goto _LL166;}_LL166:;}{
# 1520
void*_tmp246=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp4A=_tmp246;void*_tmp247=_stmttmp4A;void*_tmp248;switch(*((int*)_tmp247)){case 6: _tmp248=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp247)->f1;{struct Cyc_List_List*tqts=_tmp248;
# 1522
struct _tuple16 _tmp249=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(rvals))->hd);struct _tuple16 _stmttmp4B=_tmp249;struct _tuple16 _tmp24A=_stmttmp4B;void*_tmp24B;_tmp24B=_tmp24A.f1;{void*r=_tmp24B;
void*_tmp24C=r;struct _fat_ptr _tmp24D;if(*((int*)_tmp24C)== 6){_tmp24D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24C)->f2;{struct _fat_ptr rdict=_tmp24D;
# 1525
unsigned i=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;
return({struct _tuple17 _tmp562;_tmp562.f1=f2,_tmp562.f2=*((void**)_check_fat_subscript(rdict,sizeof(void*),(int)i));_tmp562;});}}else{
({void*_tmp24E=0U;({struct _fat_ptr _tmp653=({const char*_tmp24F="anal_Rexp: Subscript";_tag_fat(_tmp24F,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp653,_tag_fat(_tmp24E,sizeof(void*),0));});});};}}case 3:  {
# 1530
struct _tuple17 _tmp250=Cyc_NewControlFlow_anal_derefR(env,inflow,f,e1,(void*)((struct Cyc_List_List*)_check_null(rvals))->hd,e2);struct _tuple17 _stmttmp4C=_tmp250;struct _tuple17 _tmp251=_stmttmp4C;void*_tmp253;union Cyc_CfFlowInfo_FlowInfo _tmp252;_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;{union Cyc_CfFlowInfo_FlowInfo f3=_tmp252;void*r=_tmp253;
union Cyc_CfFlowInfo_FlowInfo _tmp254=f3;if((_tmp254.BottomFL).tag == 1)
return({struct _tuple17 _tmp563;_tmp563.f1=f3,_tmp563.f2=r;_tmp563;});else{
return({struct _tuple17 _tmp564;_tmp564.f1=f2,_tmp564.f2=r;_tmp564;});};}}default:
# 1535
({void*_tmp255=0U;({struct _fat_ptr _tmp654=({const char*_tmp256="anal_Rexp: Subscript -- bad type";_tag_fat(_tmp256,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp654,_tag_fat(_tmp255,sizeof(void*),0));});});};}}}case 24: _tmp11A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_LL69: {struct Cyc_List_List*es=_tmp11A;
# 1541
struct _tuple24 _tmp258=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp4D=_tmp258;struct _tuple24 _tmp259=_stmttmp4D;void*_tmp25B;union Cyc_CfFlowInfo_FlowInfo _tmp25A;_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp25A;struct Cyc_List_List*rvals=_tmp25B;
struct _fat_ptr aggrdict=({unsigned _tmp25F=(unsigned)
((int(*)(struct Cyc_List_List*))Cyc_List_length)(es);void**_tmp25E=_cycalloc(_check_times(_tmp25F,sizeof(void*)));({{unsigned _tmp566=(unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(es);unsigned i;for(i=0;i < _tmp566;++ i){({
void*_tmp655=({void*_tmp25D=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;void*temp=_tmp25D;
rvals=rvals->tl;
temp;});
# 1544
_tmp25E[i]=_tmp655;});}}0;});_tag_fat(_tmp25E,sizeof(void*),_tmp25F);});
# 1548
return({struct _tuple17 _tmp565;_tmp565.f1=f,({void*_tmp656=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C->tag=6,(_tmp25C->f1).is_union=0,(_tmp25C->f1).fieldnum=- 1,_tmp25C->f2=aggrdict;_tmp25C;});_tmp565.f2=_tmp656;});_tmp565;});}}case 30: _tmp11A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp119)->f2;{struct Cyc_List_List*des=_tmp11A;
# 1551
struct Cyc_List_List*fs;
{void*_tmp260=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp4E=_tmp260;void*_tmp261=_stmttmp4E;void*_tmp262;if(*((int*)_tmp261)== 7){_tmp262=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp261)->f2;{struct Cyc_List_List*f=_tmp262;
fs=f;goto _LL185;}}else{
({void*_tmp263=0U;({struct _fat_ptr _tmp657=({const char*_tmp264="anal_Rexp:anon struct has bad type";_tag_fat(_tmp264,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp657,_tag_fat(_tmp263,sizeof(void*),0));});});}_LL185:;}
# 1556
_tmp11A=des;_tmp11E=0U;_tmp11B=fs;goto _LL6D;}case 29: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4)->impl != 0){_tmp11A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f3;_tmp11E=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4)->kind;_tmp11B=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp119)->f4)->impl)->fields;_LL6D: {struct Cyc_List_List*des=_tmp11A;enum Cyc_Absyn_AggrKind kind=_tmp11E;struct Cyc_List_List*fs=_tmp11B;
# 1558
void*exp_type=(void*)_check_null(e->topt);
struct _tuple24 _tmp265=({struct Cyc_NewControlFlow_AnalEnv*_tmp659=env;union Cyc_CfFlowInfo_FlowInfo _tmp658=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp659,_tmp658,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple27*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd,des),1,1);});struct _tuple24 _stmttmp4F=_tmp265;struct _tuple24 _tmp266=_stmttmp4F;void*_tmp268;union Cyc_CfFlowInfo_FlowInfo _tmp267;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp267;struct Cyc_List_List*rvals=_tmp268;
struct _fat_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)kind == (int)Cyc_Absyn_UnionA,fenv->unknown_all);
# 1563
int field_no=-1;
{int i=0;for(0;rvals != 0;(rvals=rvals->tl,des=des->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple27*)((struct Cyc_List_List*)_check_null(des))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp269=(void*)ds->hd;void*_stmttmp50=_tmp269;void*_tmp26A=_stmttmp50;void*_tmp26B;if(*((int*)_tmp26A)== 0)
({void*_tmp26C=0U;({struct _fat_ptr _tmp65A=({const char*_tmp26D="anal_Rexp:Aggregate_e";_tag_fat(_tmp26D,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp65A,_tag_fat(_tmp26C,sizeof(void*),0));});});else{_tmp26B=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp26A)->f1;{struct _fat_ptr*fld=_tmp26B;
# 1570
field_no=Cyc_CfFlowInfo_get_field_index_fs(fs,fld);
*((void**)_check_fat_subscript(aggrdict,sizeof(void*),field_no))=(void*)rvals->hd;
# 1573
if((int)kind == (int)1U){
struct Cyc_Absyn_Exp*_tmp26E=(*((struct _tuple27*)des->hd)).f2;struct Cyc_Absyn_Exp*e=_tmp26E;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,(void*)rvals->hd);
# 1577
Cyc_NewControlFlow_check_union_requires(e->loc,exp_type,fld,f);}}};}}}{
# 1580
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26F=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->tag=6,(_tmp270->f1).is_union=(int)kind == (int)Cyc_Absyn_UnionA,(_tmp270->f1).fieldnum=field_no,_tmp270->f2=aggrdict;_tmp270;});struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*res=_tmp26F;
return({struct _tuple17 _tmp567;_tmp567.f1=f,_tmp567.f2=(void*)res;_tmp567;});}}}}else{goto _LL6E;}}else{_LL6E:
# 1584
({struct Cyc_String_pa_PrintArg_struct _tmp273=({struct Cyc_String_pa_PrintArg_struct _tmp568;_tmp568.tag=0,({struct _fat_ptr _tmp65B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp568.f1=_tmp65B;});_tmp568;});void*_tmp271[1];_tmp271[0]=& _tmp273;({struct _fat_ptr _tmp65C=({const char*_tmp272="anal_Rexp:missing aggrdeclimpl in %s";_tag_fat(_tmp272,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp65C,_tag_fat(_tmp271,sizeof(void*),1));});});}case 26: _tmp11A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_List_List*dles=_tmp11A;
# 1586
struct Cyc_List_List*_tmp274=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple27*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple27*))Cyc_Core_snd,dles);struct Cyc_List_List*es=_tmp274;
struct _tuple24 _tmp275=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp51=_tmp275;struct _tuple24 _tmp276=_stmttmp51;void*_tmp278;union Cyc_CfFlowInfo_FlowInfo _tmp277;_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=_tmp277;struct Cyc_List_List*rvals=_tmp278;
for(1;rvals != 0;(rvals=rvals->tl,es=es->tl)){
outflow=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,outflow,(void*)rvals->hd);}
# 1591
return({struct _tuple17 _tmp569;_tmp569.f1=outflow,({void*_tmp65D=Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)_check_null(e->topt),0,fenv->notzeroall);_tmp569.f2=_tmp65D;});_tmp569;});}}case 27: _tmp11A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f2;_tmp11D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f3;_tmp11C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp119)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp11A;struct Cyc_Absyn_Exp*e1=_tmp11B;struct Cyc_Absyn_Exp*e2=_tmp11D;int zt=_tmp11C;
# 1595
struct _tuple17 _tmp279=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e1,local_alias);struct _tuple17 _stmttmp52=_tmp279;struct _tuple17 _tmp27A=_stmttmp52;void*_tmp27C;union Cyc_CfFlowInfo_FlowInfo _tmp27B;_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp27B;void*r1=_tmp27C;
union Cyc_CfFlowInfo_FlowInfo _tmp27D=f1;void*_tmp27F;struct Cyc_Dict_Dict _tmp27E;if((_tmp27D.BottomFL).tag == 1)
return({struct _tuple17 _tmp56A;_tmp56A.f1=f1,_tmp56A.f2=fenv->unknown_all;_tmp56A;});else{_tmp27E=((_tmp27D.ReachableFL).val).f1;_tmp27F=((_tmp27D.ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_tmp27E;struct Cyc_List_List*relns=_tmp27F;
# 1599
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d1,r1)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp280=0U;({unsigned _tmp65F=e1->loc;struct _fat_ptr _tmp65E=({const char*_tmp281="expression may not be initialized";_tag_fat(_tmp281,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65F,_tmp65E,_tag_fat(_tmp280,sizeof(void*),0));});});{
# 1603
struct Cyc_List_List*new_relns=relns;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(vd);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& n2))
new_relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,relns);
if(relns != new_relns)
f1=Cyc_CfFlowInfo_ReachableFL(d1,new_relns);{
# 1612
void*_tmp282=r1;switch(*((int*)_tmp282)){case 0:
 return({struct _tuple17 _tmp56B;_tmp56B.f1=f1,_tmp56B.f2=fenv->unknown_all;_tmp56B;});case 1:
 goto _LL1A3;case 4: _LL1A3: {
# 1616
struct Cyc_List_List _tmp283=({struct Cyc_List_List _tmp56E;_tmp56E.hd=vd,_tmp56E.tl=0;_tmp56E;});struct Cyc_List_List l=_tmp283;
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
# 1619
struct _tuple17 _tmp284=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);struct _tuple17 _stmttmp53=_tmp284;struct _tuple17 _tmp285=_stmttmp53;void*_tmp287;union Cyc_CfFlowInfo_FlowInfo _tmp286;_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp286;void*r2=_tmp287;
{union Cyc_CfFlowInfo_FlowInfo _tmp288=f2;struct Cyc_Dict_Dict _tmp289;if((_tmp288.BottomFL).tag == 1)
return({struct _tuple17 _tmp56C;_tmp56C.f1=f2,_tmp56C.f2=fenv->unknown_all;_tmp56C;});else{_tmp289=((_tmp288.ReachableFL).val).f1;{struct Cyc_Dict_Dict d2=_tmp289;
# 1623
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp28A=0U;({unsigned _tmp661=e1->loc;struct _fat_ptr _tmp660=({const char*_tmp28B="expression may not be initialized";_tag_fat(_tmp28B,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp661,_tmp660,_tag_fat(_tmp28A,sizeof(void*),0));});});
return({struct _tuple17 _tmp56D;({union Cyc_CfFlowInfo_FlowInfo _tmp662=Cyc_CfFlowInfo_BottomFL();_tmp56D.f1=_tmp662;}),_tmp56D.f2=fenv->unknown_all;_tmp56D;});}}};}
# 1628
f1=f2;
goto _LL1A5;}}}default: _LL1A5:
# 1631
 while(1){
struct Cyc_List_List _tmp28C=({struct Cyc_List_List _tmp570;_tmp570.hd=vd,_tmp570.tl=0;_tmp570;});struct Cyc_List_List l=_tmp28C;
f1=Cyc_NewControlFlow_add_vars(fenv,f1,& l,fenv->unknown_all,e->loc,0);{
struct _tuple17 _tmp28D=Cyc_NewControlFlow_anal_Rexp(env,1,f1,e2,local_alias);struct _tuple17 _stmttmp54=_tmp28D;struct _tuple17 _tmp28E=_stmttmp54;void*_tmp290;union Cyc_CfFlowInfo_FlowInfo _tmp28F;_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp28F;void*r2=_tmp290;
{union Cyc_CfFlowInfo_FlowInfo _tmp291=f2;struct Cyc_Dict_Dict _tmp292;if((_tmp291.BottomFL).tag == 1)
goto _LL1B1;else{_tmp292=((_tmp291.ReachableFL).val).f1;{struct Cyc_Dict_Dict d2=_tmp292;
# 1638
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,r2)!= (int)Cyc_CfFlowInfo_AllIL){
({void*_tmp293=0U;({unsigned _tmp664=e1->loc;struct _fat_ptr _tmp663=({const char*_tmp294="expression may not be initialized";_tag_fat(_tmp294,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp664,_tmp663,_tag_fat(_tmp293,sizeof(void*),0));});});
return({struct _tuple17 _tmp56F;({union Cyc_CfFlowInfo_FlowInfo _tmp665=Cyc_CfFlowInfo_BottomFL();_tmp56F.f1=_tmp665;}),_tmp56F.f2=fenv->unknown_all;_tmp56F;});}}}_LL1B1:;}{
# 1643
union Cyc_CfFlowInfo_FlowInfo _tmp295=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);union Cyc_CfFlowInfo_FlowInfo newflow=_tmp295;
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,f1))
break;
f1=newflow;}}}}
# 1648
return({struct _tuple17 _tmp571;_tmp571.f1=f1,_tmp571.f2=fenv->notzeroall;_tmp571;});};}}}};}}case 28: _tmp11A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp119)->f1;_tmp11B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp119)->f2;_tmp11C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp119)->f3;{struct Cyc_Absyn_Exp*exp=_tmp11A;void*typ=_tmp11B;int iszeroterm=_tmp11C;
# 1654
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E->tag=1,_tmp29E->f1=exp,_tmp29E->f2=(void*)_check_null(e->topt);_tmp29E;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->root=root,_tmp29D->path=0;_tmp29D;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->tag=4,_tmp29C->f1=place;_tmp29C;});
void*place_val;
# 1662
place_val=Cyc_CfFlowInfo_typ_to_absrval(fenv,typ,0,fenv->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple17 _tmp296=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,exp,local_alias);struct _tuple17 _stmttmp55=_tmp296;struct _tuple17 _tmp297=_stmttmp55;union Cyc_CfFlowInfo_FlowInfo _tmp298;_tmp298=_tmp297.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp298;
outflow=f;{
union Cyc_CfFlowInfo_FlowInfo _tmp299=outflow;void*_tmp29B;struct Cyc_Dict_Dict _tmp29A;if((_tmp299.BottomFL).tag == 1)
return({struct _tuple17 _tmp572;_tmp572.f1=outflow,_tmp572.f2=rval;_tmp572;});else{_tmp29A=((_tmp299.ReachableFL).val).f1;_tmp29B=((_tmp299.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp29A;struct Cyc_List_List*relns=_tmp29B;
# 1669
return({struct _tuple17 _tmp573;({union Cyc_CfFlowInfo_FlowInfo _tmp667=({struct Cyc_Dict_Dict _tmp666=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d2,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp666,relns);});_tmp573.f1=_tmp667;}),_tmp573.f2=rval;_tmp573;});}};}}}}case 37: _tmp11A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp119)->f1;{struct Cyc_Absyn_Stmt*s=_tmp11A;
# 1673
struct _tuple18 _tmp29F=({struct _tuple18 _tmp575;_tmp575.f1=(env->fenv)->unknown_all,_tmp575.f2=copy_ctxt;_tmp575;});struct _tuple18 rval_opt=_tmp29F;
union Cyc_CfFlowInfo_FlowInfo _tmp2A0=Cyc_NewControlFlow_anal_stmt(env,inflow,s,& rval_opt);union Cyc_CfFlowInfo_FlowInfo f=_tmp2A0;
return({struct _tuple17 _tmp574;_tmp574.f1=f,_tmp574.f2=rval_opt.f1;_tmp574;});}case 36: _LL7B:
# 1678
 goto _LL7D;case 25: _LL7D:
 goto _LL7F;default: _LL7F:
# 1681
({void*_tmp2A1=0U;({struct _fat_ptr _tmp668=({const char*_tmp2A2="anal_Rexp, unexpected exp form";_tag_fat(_tmp2A2,sizeof(char),31U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp668,_tag_fat(_tmp2A1,sizeof(void*),0));});});};}}
# 1693 "new_control_flow.cyc"
static struct _tuple19 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1702
struct Cyc_CfFlowInfo_FlowEnv*_tmp2A3=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp2A3;
void*_tmp2A4=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp56=_tmp2A4;void*_tmp2A5=_stmttmp56;void*_tmp2A6;if(*((int*)_tmp2A5)== 3){_tmp2A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A5)->f1).ptr_atts).bounds;{void*bd=_tmp2A6;
# 1705
union Cyc_CfFlowInfo_FlowInfo _tmp2A7=f;void*_tmp2A9;struct Cyc_Dict_Dict _tmp2A8;if((_tmp2A7.BottomFL).tag == 1)
return({struct _tuple19 _tmp576;_tmp576.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp669=Cyc_CfFlowInfo_UnknownL();_tmp576.f2=_tmp669;});_tmp576;});else{_tmp2A8=((_tmp2A7.ReachableFL).val).f1;_tmp2A9=((_tmp2A7.ReachableFL).val).f2;{struct Cyc_Dict_Dict outdict=_tmp2A8;struct Cyc_List_List*relns=_tmp2A9;
# 1709
struct _tuple16 _tmp2AA=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp57=_tmp2AA;struct _tuple16 _tmp2AB=_stmttmp57;void*_tmp2AD;void*_tmp2AC;_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;{void*r=_tmp2AC;struct Cyc_List_List*names=_tmp2AD;
retry: {void*_tmp2AE=r;enum Cyc_CfFlowInfo_InitLevel _tmp2B0;void*_tmp2B1;void*_tmp2AF;switch(*((int*)_tmp2AE)){case 8:
# 1712
({void*_tmp2B2=0U;({struct _fat_ptr _tmp66A=({const char*_tmp2B3="named location in anal_derefL";_tag_fat(_tmp2B3,sizeof(char),30U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp66A,_tag_fat(_tmp2B2,sizeof(void*),0));});});case 1:
# 1714
({void*_tmp66B=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->tag=Cyc_CfFlowInfo_NotZero,_tmp2B4->f1=relns;_tmp2B4;});e->annot=_tmp66B;});goto _LLD;case 5: _tmp2AF=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2AE)->f1;{void*rv=_tmp2AF;
# 1717
if(expanded_place_possibly_null)
# 1720
({void*_tmp66C=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2B5->f1=relns;_tmp2B5;});e->annot=_tmp66C;});else{
# 1723
void*_tmp2B6=e->annot;void*_stmttmp58=_tmp2B6;void*_tmp2B7=_stmttmp58;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B7)->tag == Cyc_CfFlowInfo_UnknownZ){
# 1727
({void*_tmp66D=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2B8->f1=relns;_tmp2B8;});e->annot=_tmp66D;});
goto _LL1E;}else{
# 1730
({void*_tmp66E=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->tag=Cyc_CfFlowInfo_NotZero,_tmp2B9->f1=relns;_tmp2B9;});e->annot=_tmp66E;});
goto _LL1E;}_LL1E:;}
# 1735
if(Cyc_Tcutil_is_bound_one(bd))return({struct _tuple19 _tmp577;_tmp577.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp66F=Cyc_CfFlowInfo_UnknownL();_tmp577.f2=_tmp66F;});_tmp577;});
goto _LLD;}case 4: _tmp2AF=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AE)->f1)->root;_tmp2B1=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2AE)->f1)->path;{void*root=_tmp2AF;struct Cyc_List_List*path2=_tmp2B1;
# 1739
({void*_tmp670=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->tag=Cyc_CfFlowInfo_NotZero,_tmp2BA->f1=relns;_tmp2BA;});e->annot=_tmp670;});
if(Cyc_Tcutil_is_bound_one(bd))
return({struct _tuple19 _tmp578;_tmp578.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp672=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->root=root,({struct Cyc_List_List*_tmp671=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(path2,path);_tmp2BB->path=_tmp671;});_tmp2BB;}));_tmp578.f2=_tmp672;});_tmp578;});
goto _LLD;}case 0:
# 1744
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple19 _tmp579;({union Cyc_CfFlowInfo_FlowInfo _tmp674=Cyc_CfFlowInfo_BottomFL();_tmp579.f1=_tmp674;}),({union Cyc_CfFlowInfo_AbsLVal _tmp673=Cyc_CfFlowInfo_UnknownL();_tmp579.f2=_tmp673;});_tmp579;});case 2: _tmp2B0=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2AE)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp2B0;
# 1749
if(Cyc_Tcutil_is_bound_one(bd))
f=Cyc_NewControlFlow_notzero(env,inflow,f,e,il,names);
({void*_tmp675=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2BC->f1=relns;_tmp2BC;});e->annot=_tmp675;});
goto _LLD;}case 7: _tmp2AF=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2AE)->f3;{void*r2=_tmp2AF;
# 1754
if(passthrough_consumes){
r=r2;goto retry;}
# 1757
goto _LL1D;}default: _LL1D:
# 1759
({void*_tmp676=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2BD->f1=relns;_tmp2BD;});e->annot=_tmp676;});
goto _LLD;}_LLD:;}
# 1762
if((int)Cyc_CfFlowInfo_initlevel(fenv,outdict,r)== (int)Cyc_CfFlowInfo_NoneIL){
struct _tuple16 _tmp2BE=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp59=_tmp2BE;struct _tuple16 _tmp2BF=_stmttmp59;void*_tmp2C0;_tmp2C0=_tmp2BF.f1;{void*r=_tmp2C0;
void*_tmp2C1=r;if(*((int*)_tmp2C1)== 7){
# 1766
({void*_tmp2C2=0U;({unsigned _tmp678=e->loc;struct _fat_ptr _tmp677=({const char*_tmp2C3="attempt to dereference an alias-free that has already been copied";_tag_fat(_tmp2C3,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp678,_tmp677,_tag_fat(_tmp2C2,sizeof(void*),0));});});
goto _LL26;}else{
# 1769
({void*_tmp2C4=0U;({unsigned _tmp67A=e->loc;struct _fat_ptr _tmp679=({const char*_tmp2C5="dereference of possibly uninitialized pointer";_tag_fat(_tmp2C5,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp67A,_tmp679,_tag_fat(_tmp2C4,sizeof(void*),0));});});
goto _LL26;}_LL26:;}}
# 1773
return({struct _tuple19 _tmp57A;_tmp57A.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp67B=Cyc_CfFlowInfo_UnknownL();_tmp57A.f2=_tmp67B;});_tmp57A;});}}};}}else{
# 1775
({void*_tmp2C6=0U;({struct _fat_ptr _tmp67C=({const char*_tmp2C7="left deref of non-pointer-type";_tag_fat(_tmp2C7,sizeof(char),31U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp67C,_tag_fat(_tmp2C6,sizeof(void*),0));});});};}
# 1781
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp2C8=p;void*_tmp2CA;void*_tmp2C9;_tmp2C9=_tmp2C8->root;_tmp2CA=_tmp2C8->path;{void*proot=_tmp2C9;struct Cyc_List_List*ppath=_tmp2CA;
path=({struct Cyc_List_List*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));({void*_tmp67D=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->tag=1;_tmp2CB;});_tmp2CC->hd=_tmp67D;}),_tmp2CC->tl=path;_tmp2CC;});
return({struct Cyc_CfFlowInfo_Place*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->root=proot,({struct Cyc_List_List*_tmp67E=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ppath,path);_tmp2CD->path=_tmp67E;});_tmp2CD;});}}
# 1791
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1795
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp2CE=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp2CE;
{union Cyc_CfFlowInfo_FlowInfo _tmp2CF=inflow;void*_tmp2D1;struct Cyc_Dict_Dict _tmp2D0;if((_tmp2CF.BottomFL).tag == 1)
return({struct _tuple19 _tmp57B;({union Cyc_CfFlowInfo_FlowInfo _tmp680=Cyc_CfFlowInfo_BottomFL();_tmp57B.f1=_tmp680;}),({union Cyc_CfFlowInfo_AbsLVal _tmp67F=Cyc_CfFlowInfo_UnknownL();_tmp57B.f2=_tmp67F;});_tmp57B;});else{_tmp2D0=((_tmp2CF.ReachableFL).val).f1;_tmp2D1=((_tmp2CF.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp2D0;struct Cyc_List_List*relns=_tmp2D1;
# 1800
d=d2;}};}{
# 1804
void*_tmp2D2=e->r;void*_stmttmp5A=_tmp2D2;void*_tmp2D3=_stmttmp5A;void*_tmp2D5;void*_tmp2D4;switch(*((int*)_tmp2D3)){case 14: _tmp2D4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D3)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D4;
_tmp2D4=e1;goto _LL9;}case 12: _tmp2D4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp2D4;
_tmp2D4=e1;goto _LLB;}case 13: _tmp2D4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1;_LLB: {struct Cyc_Absyn_Exp*e1=_tmp2D4;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e1,path);}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1)){case 4: _tmp2D4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp2D4;
# 1810
if((int)vd->sc == (int)Cyc_Absyn_Static)
return({struct _tuple19 _tmp57C;_tmp57C.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp681=Cyc_CfFlowInfo_UnknownL();_tmp57C.f2=_tmp681;});_tmp57C;});
_tmp2D4=vd;goto _LLF;}case 3: _tmp2D4=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1)->f1;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp2D4;
_tmp2D4=vd;goto _LL11;}case 5: _tmp2D4=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1)->f1;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp2D4;
# 1815
return({struct _tuple19 _tmp57D;_tmp57D.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp683=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));({void*_tmp682=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->tag=0,_tmp2D6->f1=vd;_tmp2D6;});_tmp2D7->root=_tmp682;}),_tmp2D7->path=path;_tmp2D7;}));_tmp57D.f2=_tmp683;});_tmp57D;});}case 1: _tmp2D4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp2D4;
# 1817
return({struct _tuple19 _tmp57E;_tmp57E.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp684=Cyc_CfFlowInfo_UnknownL();_tmp57E.f2=_tmp684;});_tmp57E;});}default: goto _LL1C;}case 22: _tmp2D4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1;_tmp2D5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D3)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D4;struct _fat_ptr*f=_tmp2D5;
# 1820
{void*_tmp2D8=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp5B=_tmp2D8;void*_tmp2D9=_stmttmp5B;void*_tmp2DA;if(*((int*)_tmp2D9)== 3){_tmp2DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9)->f1).elt_type;{void*t2=_tmp2DA;
# 1822
if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(t2)){
Cyc_NewControlFlow_check_union_requires(e1->loc,t2,f,inflow);
path=({struct Cyc_List_List*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));({void*_tmp686=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB->tag=0,({int _tmp685=Cyc_CfFlowInfo_get_field_index(t2,f);_tmp2DB->f1=_tmp685;});_tmp2DB;});_tmp2DC->hd=_tmp686;}),_tmp2DC->tl=path;_tmp2DC;});}
# 1826
goto _LL1E;}}else{
({void*_tmp2DD=0U;({struct _fat_ptr _tmp687=({const char*_tmp2DE="anal_Lexp: AggrArrow ptr";_tag_fat(_tmp2DE,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp687,_tag_fat(_tmp2DD,sizeof(void*),0));});});}_LL1E:;}
# 1829
_tmp2D4=e1;goto _LL17;}case 20: _tmp2D4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp2D4;
# 1833
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)){
# 1835
struct _tuple19 _tmp2DF=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,e1);struct _tuple19 _stmttmp5C=_tmp2DF;struct _tuple19 _tmp2E0=_stmttmp5C;union Cyc_CfFlowInfo_AbsLVal _tmp2E2;union Cyc_CfFlowInfo_FlowInfo _tmp2E1;_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2E1;union Cyc_CfFlowInfo_AbsLVal lval=_tmp2E2;
struct _tuple19 _tmp2E3=({struct _tuple19 _tmp582;_tmp582.f1=f,_tmp582.f2=lval;_tmp582;});struct _tuple19 _stmttmp5D=_tmp2E3;struct _tuple19 _tmp2E4=_stmttmp5D;void*_tmp2E7;void*_tmp2E6;struct Cyc_Dict_Dict _tmp2E5;if(((_tmp2E4.f1).ReachableFL).tag == 2){if(((_tmp2E4.f2).PlaceL).tag == 1){_tmp2E5=(((_tmp2E4.f1).ReachableFL).val).f1;_tmp2E6=(((_tmp2E4.f1).ReachableFL).val).f2;_tmp2E7=((_tmp2E4.f2).PlaceL).val;{struct Cyc_Dict_Dict fd=_tmp2E5;struct Cyc_List_List*r=_tmp2E6;struct Cyc_CfFlowInfo_Place*p=_tmp2E7;
# 1838
void*_tmp2E8=Cyc_CfFlowInfo_lookup_place(fd,p);void*old_rval=_tmp2E8;
struct _tuple16 _tmp2E9=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple16 _stmttmp5E=_tmp2E9;struct _tuple16 _tmp2EA=_stmttmp5E;void*_tmp2EC;void*_tmp2EB;_tmp2EB=_tmp2EA.f1;_tmp2EC=_tmp2EA.f2;{void*rval=_tmp2EB;struct Cyc_List_List*names=_tmp2EC;
if(expand_unique){
int _tmp2ED=1;int possibly_null=_tmp2ED;
void*_tmp2EE=rval;void*_tmp2F0;void*_tmp2EF;switch(*((int*)_tmp2EE)){case 8: _tmp2EF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2EE)->f1;_tmp2F0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2EE)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp2EF;void*r=_tmp2F0;
# 1844
({void*_tmp2F1=0U;({struct _fat_ptr _tmp688=({const char*_tmp2F2="bad named location in anal_Lexp:deref";_tag_fat(_tmp2F2,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp688,_tag_fat(_tmp2F1,sizeof(void*),0));});});}case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2EE)->f3)){case 4:
 goto _LL34;case 5:
# 1848
 goto _LL38;default: goto _LL3B;}case 4: _LL34:
# 1847
 return Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);case 5: _LL38: {
# 1852
struct _tuple19 _tmp2F3=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _stmttmp5F=_tmp2F3;struct _tuple19 _tmp2F4=_stmttmp5F;union Cyc_CfFlowInfo_FlowInfo _tmp2F5;_tmp2F5=_tmp2F4.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2F5;
return({struct _tuple19 _tmp57F;_tmp57F.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp689=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp57F.f2=_tmp689;});_tmp57F;});}}case 1:
# 1855
 possibly_null=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1862
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(fenv,fd,rval);
void*leaf=(int)il == (int)1U?fenv->unknown_all: fenv->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->tag=5,({void*_tmp68C=({struct Cyc_CfFlowInfo_FlowEnv*_tmp68B=fenv;void*_tmp68A=Cyc_Tcutil_pointer_elt_type((void*)_check_null(e1->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp68B,_tmp68A,0,leaf);});_tmp2FB->f1=_tmp68C;});_tmp2FB;});
for(1;names != 0;names=names->tl){
res=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->tag=8,_tmp2F6->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp2F6->f2=res;_tmp2F6;});}
fd=Cyc_CfFlowInfo_assign_place(fenv,e->loc,fd,p,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp2F7=Cyc_CfFlowInfo_ReachableFL(fd,r);union Cyc_CfFlowInfo_FlowInfo outflow=_tmp2F7;
# 1875
struct _tuple19 _tmp2F8=Cyc_NewControlFlow_anal_derefL(env,outflow,e1,outflow,res,passthrough_consumes,possibly_null,path);struct _tuple19 _stmttmp60=_tmp2F8;struct _tuple19 _tmp2F9=_stmttmp60;union Cyc_CfFlowInfo_FlowInfo _tmp2FA;_tmp2FA=_tmp2F9.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2FA;
# 1877
return({struct _tuple19 _tmp580;_tmp580.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp68D=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp580.f2=_tmp68D;});_tmp580;});}}}};}else{
# 1881
void*_tmp2FC=rval;switch(*((int*)_tmp2FC)){case 7: if(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2FC)->f3)== 5)
goto _LL47;else{goto _LL48;}case 5: _LL47: {
# 1886
struct _tuple19 _tmp2FD=Cyc_NewControlFlow_anal_derefL(env,f,e1,f,old_rval,passthrough_consumes,0,path);struct _tuple19 _stmttmp61=_tmp2FD;struct _tuple19 _tmp2FE=_stmttmp61;union Cyc_CfFlowInfo_FlowInfo _tmp2FF;_tmp2FF=_tmp2FE.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp2FF;
return({struct _tuple19 _tmp581;_tmp581.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp68E=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(p,path));_tmp581.f2=_tmp68E;});_tmp581;});}}default: _LL48:
# 1890
 goto _LL43;}_LL43:;}
# 1893
goto _LL26;}}}else{goto _LL29;}}else{_LL29:
 goto _LL26;}_LL26:;}}{
# 1898
struct _tuple17 _tmp300=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);struct _tuple17 _stmttmp62=_tmp300;struct _tuple17 _tmp301=_stmttmp62;void*_tmp303;union Cyc_CfFlowInfo_FlowInfo _tmp302;_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp302;void*r=_tmp303;
# 1900
return Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,r,passthrough_consumes,0,path);}}}case 23: _tmp2D4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1;_tmp2D5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D3)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D4;struct Cyc_Absyn_Exp*e2=_tmp2D5;
# 1903
void*_tmp304=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp63=_tmp304;void*_tmp305=_stmttmp63;switch(*((int*)_tmp305)){case 6:  {
# 1905
unsigned _tmp306=(Cyc_Evexp_eval_const_uint_exp(e2)).f1;unsigned fld=_tmp306;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp694=env;union Cyc_CfFlowInfo_FlowInfo _tmp693=inflow;int _tmp692=expand_unique;int _tmp691=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp690=e1;Cyc_NewControlFlow_anal_Lexp_rec(_tmp694,_tmp693,_tmp692,_tmp691,_tmp690,({struct Cyc_List_List*_tmp308=_cycalloc(sizeof(*_tmp308));
({void*_tmp68F=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307->tag=0,_tmp307->f1=(int)fld;_tmp307;});_tmp308->hd=_tmp68F;}),_tmp308->tl=path;_tmp308;}));});}case 3:  {
# 1909
struct _tuple24 _tmp309=({struct Cyc_NewControlFlow_AnalEnv*_tmp696=env;union Cyc_CfFlowInfo_FlowInfo _tmp695=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp696,_tmp695,({struct Cyc_Absyn_Exp*_tmp319[2];_tmp319[0]=e1,_tmp319[1]=e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp319,sizeof(struct Cyc_Absyn_Exp*),2));}),0,1);});struct _tuple24 _stmttmp64=_tmp309;struct _tuple24 _tmp30A=_stmttmp64;void*_tmp30C;union Cyc_CfFlowInfo_FlowInfo _tmp30B;_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp30B;struct Cyc_List_List*rvals=_tmp30C;
union Cyc_CfFlowInfo_FlowInfo _tmp30D=f;union Cyc_CfFlowInfo_FlowInfo f2=_tmp30D;
{union Cyc_CfFlowInfo_FlowInfo _tmp30E=f;void*_tmp310;struct Cyc_Dict_Dict _tmp30F;if((_tmp30E.ReachableFL).tag == 2){_tmp30F=((_tmp30E.ReachableFL).val).f1;_tmp310=((_tmp30E.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp30F;struct Cyc_List_List*relns=_tmp310;
# 1913
if((int)Cyc_CfFlowInfo_initlevel(fenv,d2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(rvals))->tl))->hd)== (int)Cyc_CfFlowInfo_NoneIL)
({void*_tmp311=0U;({unsigned _tmp698=e2->loc;struct _fat_ptr _tmp697=({const char*_tmp312="expression may not be initialized";_tag_fat(_tmp312,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp698,_tmp697,_tag_fat(_tmp311,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp313=Cyc_NewControlFlow_add_subscript_reln(relns,e1,e2);struct Cyc_List_List*new_relns=_tmp313;
if(relns != new_relns)
f2=Cyc_CfFlowInfo_ReachableFL(d2,new_relns);
goto _LL5A;}}}else{
goto _LL5A;}_LL5A:;}{
# 1922
struct _tuple19 _tmp314=Cyc_NewControlFlow_anal_derefL(env,inflow,e1,f,(void*)((struct Cyc_List_List*)_check_null(rvals))->hd,passthrough_consumes,0,path);struct _tuple19 _stmttmp65=_tmp314;struct _tuple19 _tmp315=_stmttmp65;union Cyc_CfFlowInfo_AbsLVal _tmp317;union Cyc_CfFlowInfo_FlowInfo _tmp316;_tmp316=_tmp315.f1;_tmp317=_tmp315.f2;{union Cyc_CfFlowInfo_FlowInfo f3=_tmp316;union Cyc_CfFlowInfo_AbsLVal r=_tmp317;
union Cyc_CfFlowInfo_FlowInfo _tmp318=f3;if((_tmp318.BottomFL).tag == 1)
return({struct _tuple19 _tmp583;_tmp583.f1=f3,_tmp583.f2=r;_tmp583;});else{
return({struct _tuple19 _tmp584;_tmp584.f1=f2,_tmp584.f2=r;_tmp584;});};}}}}default:
# 1927
({void*_tmp31A=0U;({struct _fat_ptr _tmp699=({const char*_tmp31B="anal_Lexp: Subscript -- bad type";_tag_fat(_tmp31B,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp699,_tag_fat(_tmp31A,sizeof(void*),0));});});};}case 21: _tmp2D4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D3)->f1;_tmp2D5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D3)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2D4;struct _fat_ptr*fld=_tmp2D5;
# 1931
void*_tmp31C=(void*)_check_null(e1->topt);void*e1_type=_tmp31C;
if(Cyc_Absyn_is_require_union_type(e1_type))
Cyc_NewControlFlow_check_union_requires(e1->loc,e1_type,fld,inflow);
# 1935
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(e1_type))
return({struct _tuple19 _tmp585;_tmp585.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp69A=Cyc_CfFlowInfo_UnknownL();_tmp585.f2=_tmp69A;});_tmp585;});
# 1938
return({struct Cyc_NewControlFlow_AnalEnv*_tmp6A1=env;union Cyc_CfFlowInfo_FlowInfo _tmp6A0=inflow;int _tmp69F=expand_unique;int _tmp69E=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp69D=e1;Cyc_NewControlFlow_anal_Lexp_rec(_tmp6A1,_tmp6A0,_tmp69F,_tmp69E,_tmp69D,({struct Cyc_List_List*_tmp31E=_cycalloc(sizeof(*_tmp31E));
({void*_tmp69C=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->tag=0,({int _tmp69B=Cyc_CfFlowInfo_get_field_index(e1_type,fld);_tmp31D->f1=_tmp69B;});_tmp31D;});_tmp31E->hd=_tmp69C;}),_tmp31E->tl=path;_tmp31E;}));});}default: _LL1C:
# 1942
 return({struct _tuple19 _tmp586;({union Cyc_CfFlowInfo_FlowInfo _tmp6A3=Cyc_CfFlowInfo_BottomFL();_tmp586.f1=_tmp6A3;}),({union Cyc_CfFlowInfo_AbsLVal _tmp6A2=Cyc_CfFlowInfo_UnknownL();_tmp586.f2=_tmp6A2;});_tmp586;});};}}
# 1946
static struct _tuple19 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1950
struct _tuple19 _tmp31F=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple19 _stmttmp66=_tmp31F;struct _tuple19 _tmp320=_stmttmp66;union Cyc_CfFlowInfo_AbsLVal _tmp322;union Cyc_CfFlowInfo_FlowInfo _tmp321;_tmp321=_tmp320.f1;_tmp322=_tmp320.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp321;union Cyc_CfFlowInfo_AbsLVal r=_tmp322;
return({struct _tuple19 _tmp587;_tmp587.f1=f,_tmp587.f2=r;_tmp587;});}}
# 1957
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1960
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 1964
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1)&&
 Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
# 1967
struct _tuple19 _tmp323=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple19 _stmttmp67=_tmp323;struct _tuple19 _tmp324=_stmttmp67;union Cyc_CfFlowInfo_FlowInfo _tmp325;_tmp325=_tmp324.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp325;
inflow=f;}}}}
# 1972
return inflow;}struct _tuple28{enum Cyc_Absyn_Primop f1;void*f2;void*f3;};
# 1978
static struct _tuple20 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1980
struct Cyc_CfFlowInfo_FlowEnv*_tmp326=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp326;
# 1983
struct _tuple17 _tmp327=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,0);struct _tuple17 _stmttmp68=_tmp327;struct _tuple17 _tmp328=_stmttmp68;void*_tmp32A;union Cyc_CfFlowInfo_FlowInfo _tmp329;_tmp329=_tmp328.f1;_tmp32A=_tmp328.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp329;void*r1=_tmp32A;
struct _tuple17 _tmp32B=Cyc_NewControlFlow_anal_Rexp(env,0,f1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd,0);struct _tuple17 _stmttmp69=_tmp32B;struct _tuple17 _tmp32C=_stmttmp69;void*_tmp32E;union Cyc_CfFlowInfo_FlowInfo _tmp32D;_tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp32D;void*r2=_tmp32E;
union Cyc_CfFlowInfo_FlowInfo _tmp32F=f2;union Cyc_CfFlowInfo_FlowInfo f=_tmp32F;
# 1988
union Cyc_CfFlowInfo_FlowInfo _tmp330=f;void*_tmp332;struct Cyc_Dict_Dict _tmp331;if((_tmp330.BottomFL).tag == 1)
return({struct _tuple20 _tmp588;_tmp588.f1=f,_tmp588.f2=f;_tmp588;});else{_tmp331=((_tmp330.ReachableFL).val).f1;_tmp332=((_tmp330.ReachableFL).val).f2;{struct Cyc_Dict_Dict d=_tmp331;struct Cyc_List_List*relns=_tmp332;
# 1991
struct Cyc_Absyn_Exp*_tmp333=(struct Cyc_Absyn_Exp*)es->hd;struct Cyc_Absyn_Exp*e1=_tmp333;
struct Cyc_Absyn_Exp*_tmp334=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp334;
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r1)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp335=0U;({unsigned _tmp6A5=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _fat_ptr _tmp6A4=({const char*_tmp336="expression may not be initialized";_tag_fat(_tmp336,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6A5,_tmp6A4,_tag_fat(_tmp335,sizeof(void*),0));});});
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,r2)== (int)Cyc_CfFlowInfo_NoneIL && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp337=0U;({unsigned _tmp6A7=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _fat_ptr _tmp6A6=({const char*_tmp338="expression may not be initialized";_tag_fat(_tmp338,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6A7,_tmp6A6,_tag_fat(_tmp337,sizeof(void*),0));});});{
# 1998
union Cyc_CfFlowInfo_FlowInfo _tmp339=f;union Cyc_CfFlowInfo_FlowInfo ft=_tmp339;
union Cyc_CfFlowInfo_FlowInfo _tmp33A=f;union Cyc_CfFlowInfo_FlowInfo ff=_tmp33A;
# 2003
if((int)p == (int)5U ||(int)p == (int)6U){
struct _tuple16 _tmp33B=Cyc_CfFlowInfo_unname_rval(r1);struct _tuple16 _stmttmp6A=_tmp33B;struct _tuple16 _tmp33C=_stmttmp6A;void*_tmp33E;void*_tmp33D;_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;{void*r1=_tmp33D;struct Cyc_List_List*r1n=_tmp33E;
struct _tuple16 _tmp33F=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple16 _stmttmp6B=_tmp33F;struct _tuple16 _tmp340=_stmttmp6B;void*_tmp342;void*_tmp341;_tmp341=_tmp340.f1;_tmp342=_tmp340.f2;{void*r2=_tmp341;struct Cyc_List_List*r2n=_tmp342;
struct _tuple0 _tmp343=({struct _tuple0 _tmp589;_tmp589.f1=r1,_tmp589.f2=r2;_tmp589;});struct _tuple0 _stmttmp6C=_tmp343;struct _tuple0 _tmp344=_stmttmp6C;enum Cyc_CfFlowInfo_InitLevel _tmp345;switch(*((int*)_tmp344.f1)){case 2: if(*((int*)_tmp344.f2)== 0){_tmp345=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp344.f1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp345;
# 2008
struct _tuple20 _tmp346=Cyc_NewControlFlow_splitzero(env,inflow,f,e1,il,r1n);struct _tuple20 _stmttmp6D=_tmp346;struct _tuple20 _tmp347=_stmttmp6D;union Cyc_CfFlowInfo_FlowInfo _tmp349;union Cyc_CfFlowInfo_FlowInfo _tmp348;_tmp348=_tmp347.f1;_tmp349=_tmp347.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp348;union Cyc_CfFlowInfo_FlowInfo f2=_tmp349;
{enum Cyc_Absyn_Primop _tmp34A=p;switch((int)_tmp34A){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL25;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL25;default:
({void*_tmp34B=0U;({struct _fat_ptr _tmp6A8=({const char*_tmp34C="anal_test, zero-split";_tag_fat(_tmp34C,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6A8,_tag_fat(_tmp34B,sizeof(void*),0));});});}_LL25:;}
# 2014
goto _LL11;}}}else{goto _LL20;}case 0: switch(*((int*)_tmp344.f2)){case 2: _tmp345=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp344.f2)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp345;
# 2016
struct _tuple20 _tmp34D=Cyc_NewControlFlow_splitzero(env,f2,f,e2,il,r2n);struct _tuple20 _stmttmp6E=_tmp34D;struct _tuple20 _tmp34E=_stmttmp6E;union Cyc_CfFlowInfo_FlowInfo _tmp350;union Cyc_CfFlowInfo_FlowInfo _tmp34F;_tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp34F;union Cyc_CfFlowInfo_FlowInfo f2=_tmp350;
{enum Cyc_Absyn_Primop _tmp351=p;switch((int)_tmp351){case Cyc_Absyn_Eq:
 ft=f2;ff=f1;goto _LL2F;case Cyc_Absyn_Neq:
 ft=f1;ff=f2;goto _LL2F;default:
({void*_tmp352=0U;({struct _fat_ptr _tmp6A9=({const char*_tmp353="anal_test, zero-split";_tag_fat(_tmp353,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6A9,_tag_fat(_tmp352,sizeof(void*),0));});});}_LL2F:;}
# 2022
goto _LL11;}}case 0:
# 2024
 if((int)p == (int)5U)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;case 1:
 goto _LL1B;case 4: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1: if(*((int*)_tmp344.f2)== 0){_LL1D:
 goto _LL1F;}else{goto _LL20;}case 4: if(*((int*)_tmp344.f2)== 0){_LL1F:
# 2031
 if((int)p == (int)6U)ff=Cyc_CfFlowInfo_BottomFL();else{
ft=Cyc_CfFlowInfo_BottomFL();}
goto _LL11;}else{goto _LL20;}default: _LL20:
 goto _LL11;}_LL11:;}}}
# 2043
{struct _tuple28 _tmp354=({struct _tuple28 _tmp58B;_tmp58B.f1=p,({
void*_tmp6AB=Cyc_Absyn_compress((void*)_check_null(e1->topt));_tmp58B.f2=_tmp6AB;}),({
void*_tmp6AA=Cyc_Absyn_compress((void*)_check_null(e2->topt));_tmp58B.f3=_tmp6AA;});_tmp58B;});
# 2043
struct _tuple28 _stmttmp6F=_tmp354;struct _tuple28 _tmp355=_stmttmp6F;switch((int)_tmp355.f1){case Cyc_Absyn_Eq:
# 2046
 goto _LL3A;case Cyc_Absyn_Neq: _LL3A:
 goto _LL3C;default: if(*((int*)_tmp355.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3C:
 goto _LL3E;}else{if(*((int*)_tmp355.f3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{goto _LL43;}case 4: goto _LL41;default: goto _LL43;}else{goto _LL43;}}}else{if(*((int*)_tmp355.f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3D;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f2)->f1)== 4)goto _LL3F;else{goto _LL43;}}}else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f2)->f1)== 4)goto _LL3F;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)== 4)goto _LL41;else{goto _LL43;}}}}else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f2)->f1)== 4){_LL3F: _LL40:
# 2050
 goto _LL42;}else{goto _LL43;}}}}else{if(*((int*)_tmp355.f3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp355.f3)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3D: _LL3E:
# 2049
 goto _LL40;}else{goto _LL43;}case 4: _LL41: _LL42:
# 2051
 goto _LL36;default: goto _LL43;}else{_LL43:
 return({struct _tuple20 _tmp58A;_tmp58A.f1=ft,_tmp58A.f2=ff;_tmp58A;});}}}_LL36:;}
# 2056
{enum Cyc_Absyn_Primop _tmp356=p;switch((int)_tmp356){case Cyc_Absyn_Eq:
 goto _LL49;case Cyc_Absyn_Neq: _LL49: goto _LL4B;case Cyc_Absyn_Gt: _LL4B: goto _LL4D;case Cyc_Absyn_Gte: _LL4D: goto _LL4F;case Cyc_Absyn_Lt: _LL4F: goto _LL51;case Cyc_Absyn_Lte: _LL51: goto _LL45;default:
 return({struct _tuple20 _tmp58C;_tmp58C.f1=ft,_tmp58C.f2=ff;_tmp58C;});}_LL45:;}{
# 2061
struct _RegionHandle*_tmp357=Cyc_Core_heap_region;struct _RegionHandle*r=_tmp357;
struct _tuple13 _tmp358=Cyc_Relations_primop2relation(e1,p,e2);struct _tuple13 _stmttmp70=_tmp358;struct _tuple13 _tmp359=_stmttmp70;void*_tmp35C;enum Cyc_Relations_Relation _tmp35B;void*_tmp35A;_tmp35A=_tmp359.f1;_tmp35B=_tmp359.f2;_tmp35C=_tmp359.f3;{struct Cyc_Absyn_Exp*e1=_tmp35A;enum Cyc_Relations_Relation relation=_tmp35B;struct Cyc_Absyn_Exp*e2=_tmp35C;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2066
if(Cyc_Relations_exp2relnop(e1,& n1)&& Cyc_Relations_exp2relnop(e2,& n2)){
# 2068
struct Cyc_List_List*_tmp35D=Cyc_Relations_add_relation(r,n1,relation,n2,relns);struct Cyc_List_List*relns_true=_tmp35D;
# 2072
struct Cyc_List_List*_tmp35E=({struct _RegionHandle*_tmp6AF=r;union Cyc_Relations_RelnOp _tmp6AE=n2;enum Cyc_Relations_Relation _tmp6AD=Cyc_Relations_flip_relation(relation);union Cyc_Relations_RelnOp _tmp6AC=n1;Cyc_Relations_add_relation(_tmp6AF,_tmp6AE,_tmp6AD,_tmp6AC,relns);});struct Cyc_List_List*relns_false=_tmp35E;
struct _tuple20 _tmp35F=({struct _tuple20 _tmp591;_tmp591.f1=ft,_tmp591.f2=ff;_tmp591;});struct _tuple20 _stmttmp71=_tmp35F;struct _tuple20 _tmp360=_stmttmp71;struct Cyc_Dict_Dict _tmp362;struct Cyc_Dict_Dict _tmp361;if(((_tmp360.f1).ReachableFL).tag == 2){if(((_tmp360.f2).ReachableFL).tag == 2){_tmp361=(((_tmp360.f1).ReachableFL).val).f1;_tmp362=(((_tmp360.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict dt=_tmp361;struct Cyc_Dict_Dict df=_tmp362;
# 2075
return({struct _tuple20 _tmp58D;({union Cyc_CfFlowInfo_FlowInfo _tmp6B1=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_tmp58D.f1=_tmp6B1;}),({union Cyc_CfFlowInfo_FlowInfo _tmp6B0=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_tmp58D.f2=_tmp6B0;});_tmp58D;});}}else{_tmp361=(((_tmp360.f1).ReachableFL).val).f1;{struct Cyc_Dict_Dict dt=_tmp361;
# 2079
return({struct _tuple20 _tmp58E;({union Cyc_CfFlowInfo_FlowInfo _tmp6B2=Cyc_CfFlowInfo_ReachableFL(dt,relns_true);_tmp58E.f1=_tmp6B2;}),_tmp58E.f2=ff;_tmp58E;});}}}else{if(((_tmp360.f2).ReachableFL).tag == 2){_tmp361=(((_tmp360.f2).ReachableFL).val).f1;{struct Cyc_Dict_Dict df=_tmp361;
# 2077
return({struct _tuple20 _tmp58F;_tmp58F.f1=ft,({union Cyc_CfFlowInfo_FlowInfo _tmp6B3=Cyc_CfFlowInfo_ReachableFL(df,relns_false);_tmp58F.f2=_tmp6B3;});_tmp58F;});}}else{
# 2081
return({struct _tuple20 _tmp590;_tmp590.f1=ft,_tmp590.f2=ff;_tmp590;});}};}else{
# 2084
return({struct _tuple20 _tmp592;_tmp592.f1=ft,_tmp592.f2=ff;_tmp592;});}}}}}};}}}
# 2090
static struct _tuple20 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp363=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp363;
void*_tmp364=e->r;void*_stmttmp72=_tmp364;void*_tmp365=_stmttmp72;enum Cyc_Absyn_Primop _tmp366;void*_tmp369;void*_tmp368;void*_tmp367;switch(*((int*)_tmp365)){case 6: _tmp367=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp365)->f1;_tmp368=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp365)->f2;_tmp369=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp365)->f3;{struct Cyc_Absyn_Exp*e1=_tmp367;struct Cyc_Absyn_Exp*e2=_tmp368;struct Cyc_Absyn_Exp*e3=_tmp369;
# 2094
struct _tuple20 _tmp36A=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp73=_tmp36A;struct _tuple20 _tmp36B=_stmttmp73;union Cyc_CfFlowInfo_FlowInfo _tmp36D;union Cyc_CfFlowInfo_FlowInfo _tmp36C;_tmp36C=_tmp36B.f1;_tmp36D=_tmp36B.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp36C;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp36D;
struct _tuple20 _tmp36E=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _stmttmp74=_tmp36E;struct _tuple20 _tmp36F=_stmttmp74;union Cyc_CfFlowInfo_FlowInfo _tmp371;union Cyc_CfFlowInfo_FlowInfo _tmp370;_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp370;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp371;
struct _tuple20 _tmp372=Cyc_NewControlFlow_anal_test(env,f1f,e3);struct _tuple20 _stmttmp75=_tmp372;struct _tuple20 _tmp373=_stmttmp75;union Cyc_CfFlowInfo_FlowInfo _tmp375;union Cyc_CfFlowInfo_FlowInfo _tmp374;_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;{union Cyc_CfFlowInfo_FlowInfo f3t=_tmp374;union Cyc_CfFlowInfo_FlowInfo f3f=_tmp375;
return({struct _tuple20 _tmp593;({union Cyc_CfFlowInfo_FlowInfo _tmp6B5=Cyc_CfFlowInfo_join_flow(fenv,f2t,f3t);_tmp593.f1=_tmp6B5;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp6B4=Cyc_CfFlowInfo_join_flow(fenv,f2f,f3f);_tmp593.f2=_tmp6B4;});_tmp593;});}}}}case 7: _tmp367=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp365)->f1;_tmp368=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp365)->f2;{struct Cyc_Absyn_Exp*e1=_tmp367;struct Cyc_Absyn_Exp*e2=_tmp368;
# 2100
struct _tuple20 _tmp376=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp76=_tmp376;struct _tuple20 _tmp377=_stmttmp76;union Cyc_CfFlowInfo_FlowInfo _tmp379;union Cyc_CfFlowInfo_FlowInfo _tmp378;_tmp378=_tmp377.f1;_tmp379=_tmp377.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp378;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp379;
struct _tuple20 _tmp37A=Cyc_NewControlFlow_anal_test(env,f1t,e2);struct _tuple20 _stmttmp77=_tmp37A;struct _tuple20 _tmp37B=_stmttmp77;union Cyc_CfFlowInfo_FlowInfo _tmp37D;union Cyc_CfFlowInfo_FlowInfo _tmp37C;_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp37C;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp37D;
return({struct _tuple20 _tmp594;_tmp594.f1=f2t,({union Cyc_CfFlowInfo_FlowInfo _tmp6B6=Cyc_CfFlowInfo_join_flow(fenv,f1f,f2f);_tmp594.f2=_tmp6B6;});_tmp594;});}}}case 8: _tmp367=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp365)->f1;_tmp368=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp365)->f2;{struct Cyc_Absyn_Exp*e1=_tmp367;struct Cyc_Absyn_Exp*e2=_tmp368;
# 2104
struct _tuple20 _tmp37E=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp78=_tmp37E;struct _tuple20 _tmp37F=_stmttmp78;union Cyc_CfFlowInfo_FlowInfo _tmp381;union Cyc_CfFlowInfo_FlowInfo _tmp380;_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp380;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp381;
struct _tuple20 _tmp382=Cyc_NewControlFlow_anal_test(env,f1f,e2);struct _tuple20 _stmttmp79=_tmp382;struct _tuple20 _tmp383=_stmttmp79;union Cyc_CfFlowInfo_FlowInfo _tmp385;union Cyc_CfFlowInfo_FlowInfo _tmp384;_tmp384=_tmp383.f1;_tmp385=_tmp383.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp384;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp385;
return({struct _tuple20 _tmp595;({union Cyc_CfFlowInfo_FlowInfo _tmp6B7=Cyc_CfFlowInfo_join_flow(fenv,f1t,f2t);_tmp595.f1=_tmp6B7;}),_tmp595.f2=f2f;_tmp595;});}}}case 9: _tmp367=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp365)->f1;_tmp368=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp365)->f2;{struct Cyc_Absyn_Exp*e1=_tmp367;struct Cyc_Absyn_Exp*e2=_tmp368;
# 2108
struct _tuple17 _tmp386=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0);struct _tuple17 _stmttmp7A=_tmp386;struct _tuple17 _tmp387=_stmttmp7A;void*_tmp389;union Cyc_CfFlowInfo_FlowInfo _tmp388;_tmp388=_tmp387.f1;_tmp389=_tmp387.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp388;void*r=_tmp389;
return Cyc_NewControlFlow_anal_test(env,f,e2);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365)->f2)->tl == 0){_tmp367=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_tmp367;
# 2111
struct _tuple20 _tmp38A=Cyc_NewControlFlow_anal_test(env,inflow,e1);struct _tuple20 _stmttmp7B=_tmp38A;struct _tuple20 _tmp38B=_stmttmp7B;union Cyc_CfFlowInfo_FlowInfo _tmp38D;union Cyc_CfFlowInfo_FlowInfo _tmp38C;_tmp38C=_tmp38B.f1;_tmp38D=_tmp38B.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_tmp38C;union Cyc_CfFlowInfo_FlowInfo f2=_tmp38D;
return({struct _tuple20 _tmp596;_tmp596.f1=f2,_tmp596.f2=f1;_tmp596;});}}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp366=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365)->f1;_tmp367=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp365)->f2;{enum Cyc_Absyn_Primop p=_tmp366;struct Cyc_List_List*es=_tmp367;
# 2114
return Cyc_NewControlFlow_anal_primop_test(env,inflow,p,es);}}default:  {
# 2118
struct _tuple17 _tmp38E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0);struct _tuple17 _stmttmp7C=_tmp38E;struct _tuple17 _tmp38F=_stmttmp7C;void*_tmp391;union Cyc_CfFlowInfo_FlowInfo _tmp390;_tmp390=_tmp38F.f1;_tmp391=_tmp38F.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp390;void*r=_tmp391;
union Cyc_CfFlowInfo_FlowInfo _tmp392=f;struct Cyc_Dict_Dict _tmp393;if((_tmp392.BottomFL).tag == 1)
return({struct _tuple20 _tmp597;_tmp597.f1=f,_tmp597.f2=f;_tmp597;});else{_tmp393=((_tmp392.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp393;
# 2122
struct _tuple16 _tmp394=Cyc_CfFlowInfo_unname_rval(r);struct _tuple16 _stmttmp7D=_tmp394;struct _tuple16 _tmp395=_stmttmp7D;void*_tmp397;void*_tmp396;_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;{void*r=_tmp396;struct Cyc_List_List*names=_tmp397;
void*_tmp398=r;enum Cyc_CfFlowInfo_InitLevel _tmp399;void*_tmp39B;void*_tmp39A;switch(*((int*)_tmp398)){case 8: _tmp39A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp398)->f1;_tmp39B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp398)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp39A;void*r2=_tmp39B;
# 2125
({void*_tmp39C=0U;({struct _fat_ptr _tmp6B8=({const char*_tmp39D="anal_test: bad namedlocation";_tag_fat(_tmp39D,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6B8,_tag_fat(_tmp39C,sizeof(void*),0));});});}case 0:
 return({struct _tuple20 _tmp598;({union Cyc_CfFlowInfo_FlowInfo _tmp6B9=Cyc_CfFlowInfo_BottomFL();_tmp598.f1=_tmp6B9;}),_tmp598.f2=f;_tmp598;});case 1:
 goto _LL3D;case 5: _LL3D:
 goto _LL3F;case 4: _LL3F:
 return({struct _tuple20 _tmp599;_tmp599.f1=f,({union Cyc_CfFlowInfo_FlowInfo _tmp6BA=Cyc_CfFlowInfo_BottomFL();_tmp599.f2=_tmp6BA;});_tmp599;});case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp398)->f1 == Cyc_CfFlowInfo_NoneIL)
goto _LL43;else{_tmp399=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp398)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_tmp399;
# 2135
return Cyc_NewControlFlow_splitzero(env,inflow,f,e,il,names);}}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp398)->f1 == Cyc_CfFlowInfo_NoneIL){_LL43:
# 2131
 goto _LL45;}else{
# 2136
return({struct _tuple20 _tmp59A;_tmp59A.f1=f,_tmp59A.f2=f;_tmp59A;});}case 7: _LL45:
# 2133
({void*_tmp39E=0U;({unsigned _tmp6BC=e->loc;struct _fat_ptr _tmp6BB=({const char*_tmp39F="expression may not be initialized";_tag_fat(_tmp39F,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6BC,_tmp6BB,_tag_fat(_tmp39E,sizeof(void*),0));});});
return({struct _tuple20 _tmp59B;({union Cyc_CfFlowInfo_FlowInfo _tmp6BE=Cyc_CfFlowInfo_BottomFL();_tmp59B.f1=_tmp6BE;}),({union Cyc_CfFlowInfo_FlowInfo _tmp6BD=Cyc_CfFlowInfo_BottomFL();_tmp59B.f2=_tmp6BD;});_tmp59B;});default:
# 2137
({void*_tmp3A0=0U;({struct _fat_ptr _tmp6BF=({const char*_tmp3A1="anal_test";_tag_fat(_tmp3A1,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6BF,_tag_fat(_tmp3A0,sizeof(void*),0));});});};}}};}}};}struct _tuple29{unsigned f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2143
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple29*ckenv,void*root,void*rval){
# 2145
struct _tuple29*_tmp3A2=ckenv;struct Cyc_Dict_Dict _tmp3A5;void*_tmp3A4;unsigned _tmp3A3;_tmp3A3=_tmp3A2->f1;_tmp3A4=_tmp3A2->f2;_tmp3A5=_tmp3A2->f3;{unsigned loc=_tmp3A3;struct Cyc_NewControlFlow_AnalEnv*env=_tmp3A4;struct Cyc_Dict_Dict fd=_tmp3A5;
void*_tmp3A6=root;void*_tmp3A7;if(*((int*)_tmp3A6)== 0){_tmp3A7=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3A6)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp3A7;
# 2148
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
struct _tuple16 _tmp3A8=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple16 _stmttmp7E=_tmp3A8;struct _tuple16 _tmp3A9=_stmttmp7E;void*_tmp3AA;_tmp3AA=_tmp3A9.f1;{void*rval=_tmp3AA;
void*_tmp3AB=rval;switch(*((int*)_tmp3AB)){case 7:
 goto _LLF;case 0: _LLF:
 goto _LL11;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3AB)->f1 == Cyc_CfFlowInfo_NoneIL){_LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12:
# 2155
({struct Cyc_String_pa_PrintArg_struct _tmp3AE=({struct Cyc_String_pa_PrintArg_struct _tmp59C;_tmp59C.tag=0,({struct _fat_ptr _tmp6C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp59C.f1=_tmp6C0;});_tmp59C;});void*_tmp3AC[1];_tmp3AC[0]=& _tmp3AE;({unsigned _tmp6C2=loc;struct _fat_ptr _tmp6C1=({const char*_tmp3AD="unique pointers reachable from %s may become unreachable";_tag_fat(_tmp3AD,sizeof(char),57U);});Cyc_Warn_warn(_tmp6C2,_tmp6C1,_tag_fat(_tmp3AC,sizeof(void*),1));});});
goto _LLB;}_LLB:;}}
# 2159
goto _LL3;}}else{
goto _LL3;}_LL3:;}}
# 2164
static void Cyc_NewControlFlow_check_init_params(unsigned loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp3AF=flow;struct Cyc_Dict_Dict _tmp3B0;if((_tmp3AF.BottomFL).tag == 1)
return;else{_tmp3B0=((_tmp3AF.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp3B0;
# 2168
{struct Cyc_List_List*_tmp3B1=env->param_roots;struct Cyc_List_List*inits=_tmp3B1;for(0;inits != 0;inits=inits->tl){
if((int)({struct Cyc_CfFlowInfo_FlowEnv*_tmp6C4=env->fenv;struct Cyc_Dict_Dict _tmp6C3=d;Cyc_CfFlowInfo_initlevel(_tmp6C4,_tmp6C3,Cyc_CfFlowInfo_lookup_place(d,(struct Cyc_CfFlowInfo_Place*)inits->hd));})!= (int)Cyc_CfFlowInfo_AllIL)
({void*_tmp3B2=0U;({unsigned _tmp6C6=loc;struct _fat_ptr _tmp6C5=({const char*_tmp3B3="function may not initialize all the parameters with attribute 'initializes'";_tag_fat(_tmp3B3,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp6C6,_tmp6C5,_tag_fat(_tmp3B2,sizeof(void*),0));});});}}
# 2172
if(Cyc_Flags_warn_lose_unique){
struct _tuple29 _tmp3B4=({struct _tuple29 _tmp59D;_tmp59D.f1=loc,_tmp59D.f2=env,_tmp59D.f3=d;_tmp59D;});struct _tuple29 check_env=_tmp3B4;
((void(*)(void(*)(struct _tuple29*,void*,void*),struct _tuple29*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& check_env,d);}
# 2176
return;}};}
# 2185
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2187
struct Cyc_List_List*_tmp3B5=0;struct Cyc_List_List*roots=_tmp3B5;
struct Cyc_List_List*_tmp3B6=0;struct Cyc_List_List*es=_tmp3B6;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp3B7=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp7F=_tmp3B7;struct _tuple25*_tmp3B8=_stmttmp7F;void*_tmp3BA;void*_tmp3B9;_tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B8->f2;{struct Cyc_Absyn_Vardecl**vopt=_tmp3B9;struct Cyc_Absyn_Exp*eopt=_tmp3BA;
if((vopt != 0 && eopt != 0)&&
 Cyc_Tcutil_is_noalias_pointer((void*)_check_null(eopt->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3BB=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->tag=0,_tmp3C0->f1=*vopt;_tmp3C0;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp3BB;
struct Cyc_CfFlowInfo_Place*_tmp3BC=({struct Cyc_CfFlowInfo_Place*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->root=(void*)root,_tmp3BF->path=0;_tmp3BF;});struct Cyc_CfFlowInfo_Place*rp=_tmp3BC;
roots=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->hd=rp,_tmp3BD->tl=roots;_tmp3BD;});
es=({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->hd=eopt,_tmp3BE->tl=es;_tmp3BE;});}}}}
# 2199
return({struct _tuple1 _tmp59E;_tmp59E.f1=roots,_tmp59E.f2=es;_tmp59E;});}struct _tuple30{int f1;void*f2;};
# 2205
static struct _tuple30 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2212
union Cyc_CfFlowInfo_FlowInfo _tmp3C1=flow;struct Cyc_Dict_Dict _tmp3C2;if((_tmp3C1.BottomFL).tag == 1)
({void*_tmp3C3=0U;({struct _fat_ptr _tmp6C7=({const char*_tmp3C4="noconsume_place_ok: flow became Bottom!";_tag_fat(_tmp3C4,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6C7,_tag_fat(_tmp3C3,sizeof(void*),0));});});else{_tmp3C2=((_tmp3C1.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp3C2;
# 2220
struct Cyc_Absyn_Exp*_tmp3C5=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*bogus_exp=_tmp3C5;
int _tmp3C6=0;int bogus_bool=_tmp3C6;
int _tmp3C7=1;int bogus_int=_tmp3C7;
void*_tmp3C8=Cyc_CfFlowInfo_lookup_place(d,place);void*curr_rval=_tmp3C8;
void*_tmp3C9=curr_rval;void*rval=_tmp3C9;
# 2232
int varok=0;
{void*_tmp3CA=curr_rval;void*_tmp3CC;void*_tmp3CB;if(*((int*)_tmp3CA)== 8){_tmp3CB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3CA)->f1;_tmp3CC=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3CA)->f2;{struct Cyc_Absyn_Vardecl*n=_tmp3CB;void*r=_tmp3CC;
# 2235
if(vd == n){
rval=r;
# 2238
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2240
if(Cyc_CfFlowInfo_is_unique_consumed(bogus_exp,bogus_int,rval,& bogus_bool)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp59F;_tmp59F.tag=0,({
# 2244
struct _fat_ptr _tmp6C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp59F.f1=_tmp6C8;});_tmp59F;});void*_tmp3CD[1];_tmp3CD[0]=& _tmp3CF;({unsigned _tmp6CA=loc;struct _fat_ptr _tmp6C9=({const char*_tmp3CE="function consumes parameter %s which is does not have the 'consume' attribute";_tag_fat(_tmp3CE,sizeof(char),78U);});Cyc_CfFlowInfo_aerr(_tmp6CA,_tmp6C9,_tag_fat(_tmp3CD,sizeof(void*),1));});});}else{
# 2247
if((int)Cyc_CfFlowInfo_initlevel(env->fenv,d,rval)!= (int)Cyc_CfFlowInfo_AllIL && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp3D2=({struct Cyc_String_pa_PrintArg_struct _tmp5A0;_tmp5A0.tag=0,({
# 2250
struct _fat_ptr _tmp6CB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp5A0.f1=_tmp6CB;});_tmp5A0;});void*_tmp3D0[1];_tmp3D0[0]=& _tmp3D2;({unsigned _tmp6CD=loc;struct _fat_ptr _tmp6CC=({const char*_tmp3D1="function consumes value pointed to by parameter %s, which does not have the 'consume' attribute";_tag_fat(_tmp3D1,sizeof(char),96U);});Cyc_CfFlowInfo_aerr(_tmp6CD,_tmp6CC,_tag_fat(_tmp3D0,sizeof(void*),1));});});else{
# 2252
varok=1;}}}else{
# 2255
varok=1;}}else{
# 2258
goto _LL9;}
goto _LL5;}}else{_LL9:
# 2262
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp3D5=({struct Cyc_String_pa_PrintArg_struct _tmp5A1;_tmp5A1.tag=0,({
# 2267
struct _fat_ptr _tmp6CE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp5A1.f1=_tmp6CE;});_tmp5A1;});void*_tmp3D3[1];_tmp3D3[0]=& _tmp3D5;({unsigned _tmp6D0=loc;struct _fat_ptr _tmp6CF=({const char*_tmp3D4="function parameter %s without 'consume' attribute no longer set to its original value";_tag_fat(_tmp3D4,sizeof(char),86U);});Cyc_CfFlowInfo_aerr(_tmp6D0,_tmp6CF,_tag_fat(_tmp3D3,sizeof(void*),1));});});}
goto _LL5;}_LL5:;}
# 2274
return({struct _tuple30 _tmp5A2;_tmp5A2.f1=varok,_tmp5A2.f2=rval;_tmp5A2;});}};}
# 2280
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp3D6=p;void*_tmp3D8;void*_tmp3D7;_tmp3D7=_tmp3D6->root;_tmp3D8=_tmp3D6->path;{void*root=_tmp3D7;struct Cyc_List_List*fs=_tmp3D8;
if(fs != 0)
({void*_tmp3D9=0U;({struct _fat_ptr _tmp6D1=({const char*_tmp3DA="unconsume_params: param to unconsume is non-variable";_tag_fat(_tmp3DA,sizeof(char),53U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D1,_tag_fat(_tmp3D9,sizeof(void*),0));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp3DB=root;void*_tmp3DC;if(*((int*)_tmp3DB)== 0){_tmp3DC=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp3DB)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp3DC;
return vd;}}else{
({void*_tmp3DD=0U;({struct _fat_ptr _tmp6D2=({const char*_tmp3DE="unconsume_params: root is not a varroot";_tag_fat(_tmp3DE,sizeof(char),40U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D2,_tag_fat(_tmp3DD,sizeof(void*),0));});});};}}}
# 2299 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2307
{union Cyc_CfFlowInfo_FlowInfo _tmp3DF=flow;struct Cyc_Dict_Dict _tmp3E0;if((_tmp3DF.BottomFL).tag == 1)
return flow;else{_tmp3E0=((_tmp3DF.ReachableFL).val).f1;{struct Cyc_Dict_Dict d=_tmp3E0;
# 2314
struct _tuple19 _tmp3E1=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple19 _stmttmp80=_tmp3E1;struct _tuple19 _tmp3E2=_stmttmp80;union Cyc_CfFlowInfo_AbsLVal _tmp3E4;union Cyc_CfFlowInfo_FlowInfo _tmp3E3;_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp3E3;union Cyc_CfFlowInfo_AbsLVal lval=_tmp3E4;
# 2317
{union Cyc_CfFlowInfo_AbsLVal _tmp3E5=lval;void*_tmp3E6;if((_tmp3E5.PlaceL).tag == 1){_tmp3E6=(_tmp3E5.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp3E6;
# 2321
void*_tmp3E7=Cyc_CfFlowInfo_lookup_place(d,p);void*old_rval=_tmp3E7;
{void*_tmp3E8=old_rval;void*_tmp3EA;void*_tmp3E9;if(*((int*)_tmp3E8)== 8){_tmp3E9=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E8)->f1;_tmp3EA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E8)->f2;{struct Cyc_Absyn_Vardecl*old_vd=_tmp3E9;void*r=_tmp3EA;
# 2327
void*new_rval;
if(old_vd == vd){
# 2331
if(varok){
# 2333
old_rval=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,r);
# 2338
if(ropt != 0){
# 2344
struct _tuple17 _tmp3EB=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp6D4=env->fenv;struct _tuple17 _tmp6D3=({struct _tuple17 _tmp5A3;_tmp5A3.f1=f,_tmp5A3.f2=old_rval;_tmp5A3;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp6D4,_tmp6D3,({struct _tuple17 _tmp5A4;_tmp5A4.f1=f,_tmp5A4.f2=ropt;_tmp5A4;}));});
# 2344
struct _tuple17 _stmttmp81=_tmp3EB;struct _tuple17 _tmp3EC=_stmttmp81;void*_tmp3EE;union Cyc_CfFlowInfo_FlowInfo _tmp3ED;_tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_tmp3ED;void*new_rval2=_tmp3EE;
# 2348
f=f2;new_rval=new_rval2;}}else{
# 2353
new_rval=old_rval;}}else{
# 2356
new_rval=r;}
# 2358
{union Cyc_CfFlowInfo_FlowInfo _tmp3EF=f;void*_tmp3F1;struct Cyc_Dict_Dict _tmp3F0;if((_tmp3EF.ReachableFL).tag == 2){_tmp3F0=((_tmp3EF.ReachableFL).val).f1;_tmp3F1=((_tmp3EF.ReachableFL).val).f2;{struct Cyc_Dict_Dict d2=_tmp3F0;struct Cyc_List_List*relns=_tmp3F1;
# 2360
flow=({struct Cyc_Dict_Dict _tmp6D5=Cyc_CfFlowInfo_assign_place(env->fenv,loc,d2,p,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp6D5,relns);});
# 2364
goto _LL15;}}else{
# 2366
({void*_tmp3F2=0U;({struct _fat_ptr _tmp6D6=({const char*_tmp3F3="unconsume_params: joined flow became bot!";_tag_fat(_tmp3F3,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6D6,_tag_fat(_tmp3F2,sizeof(void*),0));});});}_LL15:;}
# 2368
goto _LLD;}else{
# 2370
goto _LL11;}
goto _LLD;}}else{_LL11:
# 2377
 if(ropt != 0 && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp3F6=({struct Cyc_String_pa_PrintArg_struct _tmp5A5;_tmp5A5.tag=0,({
struct _fat_ptr _tmp6D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp5A5.f1=_tmp6D7;});_tmp5A5;});void*_tmp3F4[1];_tmp3F4[0]=& _tmp3F6;({unsigned _tmp6D9=loc;struct _fat_ptr _tmp6D8=({const char*_tmp3F5="aliased expression %s was overwritten";_tag_fat(_tmp3F5,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp6D9,_tmp6D8,_tag_fat(_tmp3F4,sizeof(void*),1));});});
# 2394 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2396
goto _LL8;}}else{
# 2402
goto _LL8;}_LL8:;}
# 2404
goto _LL0;}}}_LL0:;}
# 2406
return flow;}
# 2419 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned loc){
# 2425
{union Cyc_CfFlowInfo_FlowInfo _tmp3F7=flow;if((_tmp3F7.BottomFL).tag == 1)
return flow;else{
goto _LL0;}_LL0:;}{
# 2429
int _tmp3F8=unconsume_exps != 0;int do_unconsume=_tmp3F8;
struct Cyc_List_List*_tmp3F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(consumed_vals);struct Cyc_List_List*consumed_vals=_tmp3F9;
struct Cyc_List_List*_tmp3FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(unconsume_exps);struct Cyc_List_List*unconsume_exps=_tmp3FA;
{struct Cyc_List_List*_tmp3FB=consumed_vals;struct Cyc_List_List*params=_tmp3FB;for(0;params != 0;(
params=params->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2438
struct Cyc_Absyn_Vardecl*_tmp3FC=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)params->hd);struct Cyc_Absyn_Vardecl*vd=_tmp3FC;
struct _tuple30 _tmp3FD=
is_region_open?({struct _tuple30 _tmp5A6;_tmp5A6.f1=1,_tmp5A6.f2=0;_tmp5A6;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)params->hd,do_unconsume,vd,flow,loc);
# 2439
struct _tuple30 _stmttmp82=_tmp3FD;struct _tuple30 _tmp3FE=_stmttmp82;void*_tmp400;int _tmp3FF;_tmp3FF=_tmp3FE.f1;_tmp400=_tmp3FE.f2;{int varok=_tmp3FF;void*rval=_tmp400;
# 2445
if(do_unconsume)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,vd,varok,rval,flow,loc);}}}
# 2448
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;}}struct _tuple31{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2452
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned exp_loc){
# 2454
struct Cyc_CfFlowInfo_FlowEnv*_tmp401=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp401;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp402=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmp83=_tmp402;struct Cyc_Absyn_Switch_clause*_tmp403=_stmttmp83;unsigned _tmp407;void*_tmp406;void*_tmp405;void*_tmp404;_tmp404=_tmp403->pat_vars;_tmp405=_tmp403->where_clause;_tmp406=_tmp403->body;_tmp407=_tmp403->loc;{struct Cyc_Core_Opt*vds_opt=_tmp404;struct Cyc_Absyn_Exp*where_opt=_tmp405;struct Cyc_Absyn_Stmt*body=_tmp406;unsigned loc=_tmp407;
struct _tuple1 _tmp408=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds_opt))->v);struct _tuple1 _stmttmp84=_tmp408;struct _tuple1 _tmp409=_stmttmp84;void*_tmp40B;void*_tmp40A;_tmp40A=_tmp409.f1;_tmp40B=_tmp409.f2;{struct Cyc_List_List*roots=_tmp40A;struct Cyc_List_List*es=_tmp40B;
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)vds_opt->v,roots != 0,loc,exp_loc,0);
# 2462
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp40C=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp40C;
# 2465
if(Cyc_Tcpat_has_vars(vds_opt))
({struct Cyc_List_List*_tmp6DB=({struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(*_tmp40E));
({struct _tuple31*_tmp6DA=({struct _tuple31*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->f1=0,_tmp40D->f2=body,_tmp40D->f3=roots,_tmp40D->f4=es;_tmp40D;});_tmp40E->hd=_tmp6DA;}),_tmp40E->tl=old_unique_pat_vars;_tmp40E;});
# 2466
env->unique_pat_vars=_tmp6DB;});
# 2469
if(where_opt != 0){
struct Cyc_Absyn_Exp*wexp=where_opt;
struct _tuple20 _tmp40F=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple20 _stmttmp85=_tmp40F;struct _tuple20 _tmp410=_stmttmp85;union Cyc_CfFlowInfo_FlowInfo _tmp412;union Cyc_CfFlowInfo_FlowInfo _tmp411;_tmp411=_tmp410.f1;_tmp412=_tmp410.f2;{union Cyc_CfFlowInfo_FlowInfo ft=_tmp411;union Cyc_CfFlowInfo_FlowInfo ff=_tmp412;
inflow=ff;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,ft,body,0);}}else{
# 2475
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,body,0);}
# 2477
env->unique_pat_vars=old_unique_pat_vars;{
union Cyc_CfFlowInfo_FlowInfo _tmp413=clause_outflow;if((_tmp413.BottomFL).tag == 1)
goto _LL9;else{
# 2482
clause_outflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,clause_outflow,loc);
# 2484
if(scs->tl == 0)
return clause_outflow;else{
# 2489
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp414=0U;({unsigned _tmp6DD=body->loc;struct _fat_ptr _tmp6DC=({const char*_tmp415="switch clause may implicitly fallthru";_tag_fat(_tmp415,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp6DD,_tmp6DC,_tag_fat(_tmp414,sizeof(void*),0));});});else{
# 2492
({void*_tmp416=0U;({unsigned _tmp6DF=body->loc;struct _fat_ptr _tmp6DE=({const char*_tmp417="switch clause may implicitly fallthru";_tag_fat(_tmp417,sizeof(char),38U);});Cyc_Warn_warn(_tmp6DF,_tmp6DE,_tag_fat(_tmp416,sizeof(void*),0));});});}
# 2494
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2496
goto _LL9;}_LL9:;}}}}
# 2499
return Cyc_CfFlowInfo_BottomFL();}struct _tuple32{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned f3;};
# 2504
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple32*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2506
struct _tuple32*_tmp418=vdenv;unsigned _tmp41B;struct Cyc_Dict_Dict _tmp41A;void*_tmp419;_tmp419=_tmp418->f1;_tmp41A=_tmp418->f2;_tmp41B=_tmp418->f3;{struct Cyc_NewControlFlow_AnalEnv*env=_tmp419;struct Cyc_Dict_Dict fd=_tmp41A;unsigned loc=_tmp41B;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2509
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5A8;_tmp5A8.tag=0,_tmp5A8.f1=vd;_tmp5A8;});
struct _tuple16 _tmp41C=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp6E0=fd;((void*(*)(struct Cyc_Dict_Dict,int(*)(void*,void*),void*))Cyc_Dict_lookup_other)(_tmp6E0,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp423=_cycalloc(sizeof(*_tmp423));*_tmp423=vdroot;_tmp423;}));}));struct _tuple16 _stmttmp86=_tmp41C;struct _tuple16 _tmp41D=_stmttmp86;void*_tmp41E;_tmp41E=_tmp41D.f1;{void*rval=_tmp41E;
void*_tmp41F=rval;switch(*((int*)_tmp41F)){case 7:
 goto _LLA;case 0: _LLA:
 goto _LLC;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp41F)->f1 == Cyc_CfFlowInfo_NoneIL){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
# 2516
({struct Cyc_String_pa_PrintArg_struct _tmp422=({struct Cyc_String_pa_PrintArg_struct _tmp5A7;_tmp5A7.tag=0,({struct _fat_ptr _tmp6E1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp5A7.f1=_tmp6E1;});_tmp5A7;});void*_tmp420[1];_tmp420[0]=& _tmp422;({unsigned _tmp6E3=loc;struct _fat_ptr _tmp6E2=({const char*_tmp421="unique pointers may still exist after variable %s goes out of scope";_tag_fat(_tmp421,sizeof(char),68U);});Cyc_Warn_warn(_tmp6E3,_tmp6E2,_tag_fat(_tmp420,sizeof(void*),1));});});
# 2518
goto _LL6;}_LL6:;}}}}
# 2523
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp424=inflow;struct Cyc_Dict_Dict _tmp425;if((_tmp424.ReachableFL).tag == 2){_tmp425=((_tmp424.ReachableFL).val).f1;{struct Cyc_Dict_Dict fd=_tmp425;
# 2526
struct _tuple32 _tmp426=({struct _tuple32 _tmp5A9;_tmp5A9.f1=env,_tmp5A9.f2=fd,_tmp5A9.f3=decl->loc;_tmp5A9;});struct _tuple32 vdenv=_tmp426;
struct Cyc_CfFlowInfo_FlowEnv*_tmp427=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp427;
{void*_tmp428=decl->r;void*_stmttmp87=_tmp428;void*_tmp429=_stmttmp87;void*_tmp42A;switch(*((int*)_tmp429)){case 0: _tmp42A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp429)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp42A;
# 2530
Cyc_NewControlFlow_check_dropped_unique_vd(& vdenv,vd);
goto _LL5;}case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp429)->f2 != 0){_tmp42A=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp429)->f2)->v;{struct Cyc_List_List*vds=_tmp42A;
# 2533
struct _tuple1 _tmp42B=((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)(vds);struct _tuple1 _stmttmp88=_tmp42B;struct _tuple1 _tmp42C=_stmttmp88;void*_tmp42D;_tmp42D=_tmp42C.f1;{struct Cyc_List_List*vs=_tmp42D;
struct Cyc_List_List*_tmp42E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)(vs);{struct Cyc_List_List*vs=_tmp42E;
_tmp42A=vs;goto _LLB;}}}}else{goto _LLC;}case 3: _tmp42A=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp429)->f1;_LLB: {struct Cyc_List_List*vds=_tmp42A;
# 2537
((void(*)(void(*)(struct _tuple32*,struct Cyc_Absyn_Vardecl*),struct _tuple32*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& vdenv,vds);
goto _LL5;}default: _LLC:
 goto _LL5;}_LL5:;}
# 2541
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 2544
return;}
# 2550
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2554
int num_pop=((int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple31*_tmp42F=(struct _tuple31*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple31*_stmttmp89=_tmp42F;struct _tuple31*_tmp430=_stmttmp89;void*_tmp434;void*_tmp433;void*_tmp432;int _tmp431;_tmp431=_tmp430->f1;_tmp432=_tmp430->f2;_tmp433=_tmp430->f3;_tmp434=_tmp430->f4;{int is_open=_tmp431;struct Cyc_Absyn_Stmt*pat_stmt=_tmp432;struct Cyc_List_List*roots=_tmp433;struct Cyc_List_List*es=_tmp434;
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,is_open,inflow,dest->loc);}}}
# 2559
return inflow;}
# 2565
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple18*rval_opt){
# 2567
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple21 _tmp435=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple21 _stmttmp8A=_tmp435;struct _tuple21 _tmp436=_stmttmp8A;void*_tmp438;void*_tmp437;_tmp437=_tmp436.f1;_tmp438=_tmp436.f2;{struct Cyc_NewControlFlow_CFStmtAnnot*annot=_tmp437;union Cyc_CfFlowInfo_FlowInfo*sflow=_tmp438;
inflow=*sflow;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp439=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp439;
# 2574
void*_tmp43A=s->r;void*_stmttmp8B=_tmp43A;void*_tmp43B=_stmttmp8B;unsigned _tmp43E;void*_tmp442;void*_tmp441;void*_tmp440;void*_tmp43F;void*_tmp43D;void*_tmp43C;switch(*((int*)_tmp43B)){case 0:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1 == 0){
# 2578
if(env->noreturn)
({void*_tmp443=0U;({unsigned _tmp6E5=s->loc;struct _fat_ptr _tmp6E4=({const char*_tmp444="`noreturn' function might return";_tag_fat(_tmp444,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp6E5,_tmp6E4,_tag_fat(_tmp443,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_tmp43C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;{struct Cyc_Absyn_Exp*e=_tmp43C;
# 2584
if(env->noreturn)
({void*_tmp445=0U;({unsigned _tmp6E7=s->loc;struct _fat_ptr _tmp6E6=({const char*_tmp446="`noreturn' function might return";_tag_fat(_tmp446,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp6E7,_tmp6E6,_tag_fat(_tmp445,sizeof(void*),0));});});{
struct _tuple17 _tmp447=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(e),0);struct _tuple17 _stmttmp8C=_tmp447;struct _tuple17 _tmp448=_stmttmp8C;void*_tmp44A;union Cyc_CfFlowInfo_FlowInfo _tmp449;_tmp449=_tmp448.f1;_tmp44A=_tmp448.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp449;void*r=_tmp44A;
f=Cyc_NewControlFlow_use_Rval(env,e->loc,f,r);
Cyc_NewControlFlow_check_init_params(s->loc,env,f);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,f,s->loc);
return Cyc_CfFlowInfo_BottomFL();}}}}case 1: _tmp43C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;{struct Cyc_Absyn_Exp*e=_tmp43C;
# 2593
struct _tuple18*_tmp44B=rval_opt;int _tmp44D;void*_tmp44C;if(_tmp44B != 0){_tmp44C=(void**)& _tmp44B->f1;_tmp44D=_tmp44B->f2;{void**rval=(void**)_tmp44C;int copy_ctxt=_tmp44D;
# 2595
struct _tuple17 _tmp44E=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,e,0);struct _tuple17 _stmttmp8D=_tmp44E;struct _tuple17 _tmp44F=_stmttmp8D;void*_tmp451;union Cyc_CfFlowInfo_FlowInfo _tmp450;_tmp450=_tmp44F.f1;_tmp451=_tmp44F.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp450;void*r=_tmp451;
*rval=r;
return f;}}}else{
# 2599
return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e,0)).f1;};}case 2: _tmp43C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp43C;struct Cyc_Absyn_Stmt*s2=_tmp43D;
# 2603
return({struct Cyc_NewControlFlow_AnalEnv*_tmp6EA=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E9=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);struct Cyc_Absyn_Stmt*_tmp6E8=s2;Cyc_NewControlFlow_anal_stmt(_tmp6EA,_tmp6E9,_tmp6E8,rval_opt);});}case 4: _tmp43C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;_tmp43F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp43B)->f3;{struct Cyc_Absyn_Exp*e=_tmp43C;struct Cyc_Absyn_Stmt*s1=_tmp43D;struct Cyc_Absyn_Stmt*s2=_tmp43F;
# 2606
struct _tuple20 _tmp452=Cyc_NewControlFlow_anal_test(env,inflow,e);struct _tuple20 _stmttmp8E=_tmp452;struct _tuple20 _tmp453=_stmttmp8E;union Cyc_CfFlowInfo_FlowInfo _tmp455;union Cyc_CfFlowInfo_FlowInfo _tmp454;_tmp454=_tmp453.f1;_tmp455=_tmp453.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp454;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp455;
# 2613
union Cyc_CfFlowInfo_FlowInfo _tmp456=Cyc_NewControlFlow_anal_stmt(env,f1f,s2,0);union Cyc_CfFlowInfo_FlowInfo ff=_tmp456;
union Cyc_CfFlowInfo_FlowInfo _tmp457=Cyc_NewControlFlow_anal_stmt(env,f1t,s1,0);union Cyc_CfFlowInfo_FlowInfo ft=_tmp457;
return Cyc_CfFlowInfo_join_flow(fenv,ft,ff);}}case 5: _tmp43C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1).f1;_tmp43D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1).f2;_tmp43F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;{struct Cyc_Absyn_Exp*e=_tmp43C;struct Cyc_Absyn_Stmt*cont=_tmp43D;struct Cyc_Absyn_Stmt*body=_tmp43F;
# 2621
struct _tuple21 _tmp458=Cyc_NewControlFlow_pre_stmt_check(env,inflow,cont);struct _tuple21 _stmttmp8F=_tmp458;struct _tuple21 _tmp459=_stmttmp8F;void*_tmp45A;_tmp45A=_tmp459.f2;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_tmp45A;
union Cyc_CfFlowInfo_FlowInfo _tmp45B=*eflow_ptr;union Cyc_CfFlowInfo_FlowInfo e_inflow=_tmp45B;
struct _tuple20 _tmp45C=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _stmttmp90=_tmp45C;struct _tuple20 _tmp45D=_stmttmp90;union Cyc_CfFlowInfo_FlowInfo _tmp45F;union Cyc_CfFlowInfo_FlowInfo _tmp45E;_tmp45E=_tmp45D.f1;_tmp45F=_tmp45D.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp45E;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp45F;
union Cyc_CfFlowInfo_FlowInfo _tmp460=Cyc_NewControlFlow_anal_stmt(env,f1t,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp460;
Cyc_NewControlFlow_update_flow(env,cont,body_outflow);
return f1f;}}}case 14: _tmp43C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2).f1;_tmp43F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2).f2;{struct Cyc_Absyn_Stmt*body=_tmp43C;struct Cyc_Absyn_Exp*e=_tmp43D;struct Cyc_Absyn_Stmt*cont=_tmp43F;
# 2631
union Cyc_CfFlowInfo_FlowInfo _tmp461=Cyc_NewControlFlow_anal_stmt(env,inflow,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp461;
struct _tuple21 _tmp462=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _stmttmp91=_tmp462;struct _tuple21 _tmp463=_stmttmp91;void*_tmp464;_tmp464=_tmp463.f2;{union Cyc_CfFlowInfo_FlowInfo*eflow_ptr=_tmp464;
union Cyc_CfFlowInfo_FlowInfo _tmp465=*eflow_ptr;union Cyc_CfFlowInfo_FlowInfo e_inflow=_tmp465;
struct _tuple20 _tmp466=Cyc_NewControlFlow_anal_test(env,e_inflow,e);struct _tuple20 _stmttmp92=_tmp466;struct _tuple20 _tmp467=_stmttmp92;union Cyc_CfFlowInfo_FlowInfo _tmp469;union Cyc_CfFlowInfo_FlowInfo _tmp468;_tmp468=_tmp467.f1;_tmp469=_tmp467.f2;{union Cyc_CfFlowInfo_FlowInfo f1t=_tmp468;union Cyc_CfFlowInfo_FlowInfo f1f=_tmp469;
Cyc_NewControlFlow_update_flow(env,body,f1t);
return f1f;}}}case 9: _tmp43C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2).f1;_tmp43F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2).f2;_tmp440=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43B)->f3).f1;_tmp441=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43B)->f3).f2;_tmp442=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43B)->f4;{struct Cyc_Absyn_Exp*e1=_tmp43C;struct Cyc_Absyn_Exp*e2=_tmp43D;struct Cyc_Absyn_Stmt*guard=_tmp43F;struct Cyc_Absyn_Exp*e3=_tmp440;struct Cyc_Absyn_Stmt*cont=_tmp441;struct Cyc_Absyn_Stmt*body=_tmp442;
# 2640
union Cyc_CfFlowInfo_FlowInfo _tmp46A=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e1,0)).f1;union Cyc_CfFlowInfo_FlowInfo e1_outflow=_tmp46A;
struct _tuple21 _tmp46B=Cyc_NewControlFlow_pre_stmt_check(env,e1_outflow,guard);struct _tuple21 _stmttmp93=_tmp46B;struct _tuple21 _tmp46C=_stmttmp93;void*_tmp46D;_tmp46D=_tmp46C.f2;{union Cyc_CfFlowInfo_FlowInfo*e2flow_ptr=_tmp46D;
union Cyc_CfFlowInfo_FlowInfo _tmp46E=*e2flow_ptr;union Cyc_CfFlowInfo_FlowInfo e2_inflow=_tmp46E;
struct _tuple20 _tmp46F=Cyc_NewControlFlow_anal_test(env,e2_inflow,e2);struct _tuple20 _stmttmp94=_tmp46F;struct _tuple20 _tmp470=_stmttmp94;union Cyc_CfFlowInfo_FlowInfo _tmp472;union Cyc_CfFlowInfo_FlowInfo _tmp471;_tmp471=_tmp470.f1;_tmp472=_tmp470.f2;{union Cyc_CfFlowInfo_FlowInfo f2t=_tmp471;union Cyc_CfFlowInfo_FlowInfo f2f=_tmp472;
union Cyc_CfFlowInfo_FlowInfo _tmp473=Cyc_NewControlFlow_anal_stmt(env,f2t,body,0);union Cyc_CfFlowInfo_FlowInfo body_outflow=_tmp473;
struct _tuple21 _tmp474=Cyc_NewControlFlow_pre_stmt_check(env,body_outflow,cont);struct _tuple21 _stmttmp95=_tmp474;struct _tuple21 _tmp475=_stmttmp95;void*_tmp476;_tmp476=_tmp475.f2;{union Cyc_CfFlowInfo_FlowInfo*e3flow_ptr=_tmp476;
union Cyc_CfFlowInfo_FlowInfo _tmp477=*e3flow_ptr;union Cyc_CfFlowInfo_FlowInfo e3_inflow=_tmp477;
union Cyc_CfFlowInfo_FlowInfo _tmp478=(Cyc_NewControlFlow_anal_Rexp(env,0,e3_inflow,e3,0)).f1;union Cyc_CfFlowInfo_FlowInfo e3_outflow=_tmp478;
Cyc_NewControlFlow_update_flow(env,guard,e3_outflow);
return f2f;}}}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2 != 0){_tmp43C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;{struct Cyc_List_List*es=_tmp43C;struct Cyc_Absyn_Switch_clause*destclause=_tmp43D;
# 2652
struct _tuple24 _tmp479=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,1,1);struct _tuple24 _stmttmp96=_tmp479;struct _tuple24 _tmp47A=_stmttmp96;void*_tmp47C;union Cyc_CfFlowInfo_FlowInfo _tmp47B;_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp47B;struct Cyc_List_List*rvals=_tmp47C;
# 2654
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,destclause->body);{
# 2656
struct Cyc_List_List*_tmp47D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(destclause->pat_vars))->v)).f1);struct Cyc_List_List*vds=_tmp47D;
f=Cyc_NewControlFlow_add_vars(fenv,f,vds,fenv->unknown_all,s->loc,0);
# 2659
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(destclause->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp47E=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp97=_tmp47E;struct _tuple25*_tmp47F=_stmttmp97;void*_tmp481;void*_tmp480;_tmp480=_tmp47F->f1;_tmp481=_tmp47F->f2;{struct Cyc_Absyn_Vardecl**vd=_tmp480;struct Cyc_Absyn_Exp*ve=_tmp481;
if(vd != 0){
f=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6F0=fenv;struct Cyc_NewControlFlow_AnalEnv*_tmp6EF=env;union Cyc_CfFlowInfo_FlowInfo _tmp6EE=f;struct Cyc_Absyn_Vardecl*_tmp6ED=*vd;struct Cyc_Absyn_Exp*_tmp6EC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;void*_tmp6EB=(void*)((struct Cyc_List_List*)_check_null(rvals))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp6F0,_tmp6EF,_tmp6EE,_tmp6ED,_tmp6EC,_tmp6EB,s->loc);});
rvals=rvals->tl;
es=es->tl;}}}}
# 2667
Cyc_NewControlFlow_update_flow(env,destclause->body,f);
return Cyc_CfFlowInfo_BottomFL();}}}}else{
# 2838
({void*_tmp4B4=0U;({struct _fat_ptr _tmp6F1=({const char*_tmp4B5="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_fat(_tmp4B5,sizeof(char),54U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6F1,_tag_fat(_tmp4B4,sizeof(void*),0));});});}case 6:
# 2673
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp482=inflow;if((_tmp482.ReachableFL).tag == 2){
# 2676
if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type)||
 Cyc_Tcutil_is_any_int_type(env->return_type))
({void*_tmp483=0U;({unsigned _tmp6F3=s->loc;struct _fat_ptr _tmp6F2=({const char*_tmp484="break may cause function not to return a value";_tag_fat(_tmp484,sizeof(char),47U);});Cyc_Warn_warn(_tmp6F3,_tmp6F2,_tag_fat(_tmp483,sizeof(void*),0));});});else{
# 2681
({void*_tmp485=0U;({unsigned _tmp6F5=s->loc;struct _fat_ptr _tmp6F4=({const char*_tmp486="break may cause function not to return a value";_tag_fat(_tmp486,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp6F5,_tmp6F4,_tag_fat(_tmp485,sizeof(void*),0));});});}}
# 2683
goto _LL55;}else{
goto _LL55;}_LL55:;}
# 2686
if(env->noreturn)
({void*_tmp487=0U;({unsigned _tmp6F7=s->loc;struct _fat_ptr _tmp6F6=({const char*_tmp488="`noreturn' function might return";_tag_fat(_tmp488,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp6F7,_tmp6F6,_tag_fat(_tmp487,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2692
goto _LL1B;case 7: _LL1B:
 goto _LL1D;case 8: _LL1D: {
# 2696
struct Cyc_Absyn_Stmt*_tmp489=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(env->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp489;
if(dest == 0)
({void*_tmp48A=0U;({unsigned _tmp6F9=s->loc;struct _fat_ptr _tmp6F8=({const char*_tmp48B="jump has no target (should be impossible)";_tag_fat(_tmp48B,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp6F9,_tmp6F8,_tag_fat(_tmp48A,sizeof(void*),0));});});
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(dest));
# 2701
Cyc_NewControlFlow_update_flow(env,dest,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _tmp43C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;_tmp43F=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp43B)->f3;{struct Cyc_Absyn_Exp*e=_tmp43C;struct Cyc_List_List*scs=_tmp43D;void*dec_tree=_tmp43F;
# 2707
return Cyc_NewControlFlow_anal_scs(env,inflow,scs,e->loc);}case 15: _tmp43C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;_tmp43F=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp43B)->f3;{struct Cyc_Absyn_Stmt*s1=_tmp43C;struct Cyc_List_List*scs=_tmp43D;void*dec_tree=_tmp43F;
# 2712
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2720
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2723
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
# 2725
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,scs,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp48C=scs_outflow;if((_tmp48C.BottomFL).tag == 1)
goto _LL5A;else{
({void*_tmp48D=0U;({unsigned _tmp6FB=s->loc;struct _fat_ptr _tmp6FA=({const char*_tmp48E="last catch clause may implicitly fallthru";_tag_fat(_tmp48E,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp6FB,_tmp6FA,_tag_fat(_tmp48D,sizeof(void*),0));});});}_LL5A:;}
# 2730
outflow=s1_outflow;
# 2732
return outflow;}}}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)->f2 != 0){_tmp43C=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)->f1;_tmp43D=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)->f2)->v;_tmp43F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)->f3;_tmp43E=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->loc;_tmp440=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;{struct Cyc_Absyn_Pat*letpat=_tmp43C;struct Cyc_List_List*vds=_tmp43D;struct Cyc_Absyn_Exp*e=_tmp43F;unsigned loc=_tmp43E;struct Cyc_Absyn_Stmt*s1=_tmp440;
# 2737
({void*_tmp48F=0U;({struct Cyc___cycFILE*_tmp6FD=Cyc_stderr;struct _fat_ptr _tmp6FC=({const char*_tmp490="";_tag_fat(_tmp490,sizeof(char),1U);});Cyc_fprintf(_tmp6FD,_tmp6FC,_tag_fat(_tmp48F,sizeof(void*),0));});});{
# 2743
int isAliasPat=0;
{void*_tmp491=letpat->r;void*_stmttmp98=_tmp491;void*_tmp492=_stmttmp98;if(*((int*)_tmp492)== 2){
isAliasPat=1;goto _LL5F;}else{
goto _LL5F;}_LL5F:;}{
# 2748
struct _tuple1 _tmp493=Cyc_NewControlFlow_get_unconsume_pat_vars(vds);struct _tuple1 _stmttmp99=_tmp493;struct _tuple1 _tmp494=_stmttmp99;void*_tmp496;void*_tmp495;_tmp495=_tmp494.f1;_tmp496=_tmp494.f2;{struct Cyc_List_List*roots=_tmp495;struct Cyc_List_List*es=_tmp496;
inflow=Cyc_NewControlFlow_initialize_pat_vars(fenv,env,inflow,vds,roots != 0,loc,e->loc,isAliasPat);{
struct Cyc_List_List*_tmp497=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp497;
# 2752
({struct Cyc_List_List*_tmp6FF=({struct Cyc_List_List*_tmp499=_cycalloc(sizeof(*_tmp499));
({struct _tuple31*_tmp6FE=({struct _tuple31*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->f1=0,_tmp498->f2=s,_tmp498->f3=roots,_tmp498->f4=es;_tmp498;});_tmp499->hd=_tmp6FE;}),_tmp499->tl=old_unique_pat_vars;_tmp499;});
# 2752
env->unique_pat_vars=_tmp6FF;});
# 2757
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2761
inflow=Cyc_NewControlFlow_unconsume_params(env,roots,es,0,inflow,loc);
# 2765
return inflow;}}}}}}else{goto _LL26;}case 4: _tmp43C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)->f2;_tmp43D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->r)->f3;_tmp43E=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1)->loc;_tmp43F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;if((struct Cyc_Absyn_Exp*)_tmp43D != 0){struct Cyc_Absyn_Vardecl*vd=_tmp43C;struct Cyc_Absyn_Exp*open_exp_opt=_tmp43D;unsigned loc=_tmp43E;struct Cyc_Absyn_Stmt*s1=_tmp43F;
# 2777
struct Cyc_List_List l=({struct Cyc_List_List _tmp5AB;_tmp5AB.hd=open_exp_opt,_tmp5AB.tl=0;_tmp5AB;});
union Cyc_CfFlowInfo_FlowInfo _tmp49A=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);union Cyc_CfFlowInfo_FlowInfo f=_tmp49A;
struct _tuple19 _tmp49B=Cyc_NewControlFlow_anal_Lexp(env,f,0,0,open_exp_opt);struct _tuple19 _stmttmp9A=_tmp49B;struct _tuple19 _tmp49C=_stmttmp9A;union Cyc_CfFlowInfo_AbsLVal _tmp49D;_tmp49D=_tmp49C.f2;{union Cyc_CfFlowInfo_AbsLVal lval=_tmp49D;
struct _tuple17 _tmp49E=Cyc_NewControlFlow_anal_Rexp(env,1,f,open_exp_opt,0);struct _tuple17 _stmttmp9B=_tmp49E;struct _tuple17 _tmp49F=_stmttmp9B;union Cyc_CfFlowInfo_FlowInfo _tmp4A0;_tmp4A0=_tmp49F.f1;{union Cyc_CfFlowInfo_FlowInfo f=_tmp4A0;
struct Cyc_List_List*_tmp4A1=0;struct Cyc_List_List*roots=_tmp4A1;
struct Cyc_List_List*_tmp4A2=0;struct Cyc_List_List*es=_tmp4A2;
{union Cyc_CfFlowInfo_FlowInfo _tmp4A3=f;void*_tmp4A5;struct Cyc_Dict_Dict _tmp4A4;if((_tmp4A3.ReachableFL).tag == 2){_tmp4A4=((_tmp4A3.ReachableFL).val).f1;_tmp4A5=((_tmp4A3.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_tmp4A4;struct Cyc_List_List*relns=_tmp4A5;
# 2785
{union Cyc_CfFlowInfo_AbsLVal _tmp4A6=lval;void*_tmp4A7;if((_tmp4A6.PlaceL).tag == 1){_tmp4A7=(_tmp4A6.PlaceL).val;{struct Cyc_CfFlowInfo_Place*p=_tmp4A7;
# 2789
void*_tmp4A8=Cyc_CfFlowInfo_lookup_place(fd,p);void*new_rval=_tmp4A8;
new_rval=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->tag=8,_tmp4A9->f1=vd,_tmp4A9->f2=new_rval;_tmp4A9;});
fd=Cyc_CfFlowInfo_assign_place(fenv,open_exp_opt->loc,fd,p,new_rval);
f=Cyc_CfFlowInfo_ReachableFL(fd,relns);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4AA=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->tag=0,_tmp4AF->f1=vd;_tmp4AF;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp4AA;
struct Cyc_CfFlowInfo_Place*_tmp4AB=({struct Cyc_CfFlowInfo_Place*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->root=(void*)root,_tmp4AE->path=0;_tmp4AE;});struct Cyc_CfFlowInfo_Place*rp=_tmp4AB;
roots=({struct Cyc_List_List*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->hd=rp,_tmp4AC->tl=roots;_tmp4AC;});
es=({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->hd=open_exp_opt,_tmp4AD->tl=es;_tmp4AD;});
goto _LL72;}}}else{
# 2804
goto _LL72;}_LL72:;}
# 2806
goto _LL6D;}}else{
# 2808
goto _LL6D;}_LL6D:;}{
# 2811
struct Cyc_List_List _tmp4B0=({struct Cyc_List_List _tmp5AA;_tmp5AA.hd=vd,_tmp5AA.tl=0;_tmp5AA;});struct Cyc_List_List vds=_tmp4B0;
f=Cyc_NewControlFlow_add_vars(fenv,f,& vds,fenv->unknown_all,loc,0);{
# 2815
struct Cyc_List_List*_tmp4B1=env->unique_pat_vars;struct Cyc_List_List*old_unique_pat_vars=_tmp4B1;
({struct Cyc_List_List*_tmp701=({struct Cyc_List_List*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));({struct _tuple31*_tmp700=({struct _tuple31*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->f1=1,_tmp4B2->f2=s,_tmp4B2->f3=roots,_tmp4B2->f4=es;_tmp4B2;});_tmp4B3->hd=_tmp700;}),_tmp4B3->tl=old_unique_pat_vars;_tmp4B3;});env->unique_pat_vars=_tmp701;});
# 2820
f=Cyc_NewControlFlow_anal_stmt(env,f,s1,rval_opt);
env->unique_pat_vars=old_unique_pat_vars;
# 2824
f=Cyc_NewControlFlow_unconsume_params(env,roots,es,1,f,loc);
# 2828
return f;}}}}}else{goto _LL26;}default: _LL26: _tmp43C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f1;_tmp43D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;{struct Cyc_Absyn_Decl*d=_tmp43C;struct Cyc_Absyn_Stmt*s=_tmp43D;
# 2831
outflow=({struct Cyc_NewControlFlow_AnalEnv*_tmp704=env;union Cyc_CfFlowInfo_FlowInfo _tmp703=Cyc_NewControlFlow_anal_decl(env,inflow,d);struct Cyc_Absyn_Stmt*_tmp702=s;Cyc_NewControlFlow_anal_stmt(_tmp704,_tmp703,_tmp702,rval_opt);});
if(Cyc_Flags_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,d);
return outflow;}}default: _tmp43C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp43B)->f2;{struct Cyc_Absyn_Stmt*s=_tmp43C;
# 2836
return Cyc_NewControlFlow_anal_stmt(env,inflow,s,rval_opt);}};}}}
# 2842
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Fndecl*);
# 2846
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp4B6=env->fenv;struct Cyc_CfFlowInfo_FlowEnv*fenv=_tmp4B6;
void*_tmp4B7=decl->r;void*_stmttmp9C=_tmp4B7;void*_tmp4B8=_stmttmp9C;void*_tmp4BB;void*_tmp4BA;void*_tmp4B9;switch(*((int*)_tmp4B8)){case 0: _tmp4B9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4B8)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp4B9;
# 2854
if((int)vd->sc == (int)Cyc_Absyn_Static)
return inflow;{
# 2859
struct Cyc_List_List _tmp4BC=({struct Cyc_List_List _tmp5AC;_tmp5AC.hd=vd,_tmp5AC.tl=0;_tmp5AC;});struct Cyc_List_List vds=_tmp4BC;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp4BD=vd->initializer;struct Cyc_Absyn_Exp*e=_tmp4BD;
if(e == 0)
return inflow;{
struct _tuple17 _tmp4BE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,e,0);struct _tuple17 _stmttmp9D=_tmp4BE;struct _tuple17 _tmp4BF=_stmttmp9D;void*_tmp4C1;union Cyc_CfFlowInfo_FlowInfo _tmp4C0;_tmp4C0=_tmp4BF.f1;_tmp4C1=_tmp4BF.f2;{union Cyc_CfFlowInfo_FlowInfo f=_tmp4C0;void*r=_tmp4C1;
return Cyc_NewControlFlow_do_initialize_var(fenv,env,f,vd,e,r,decl->loc);}}}}}case 3: _tmp4B9=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4B8)->f1;{struct Cyc_List_List*vds=_tmp4B9;
# 2868
return Cyc_NewControlFlow_add_vars(fenv,inflow,vds,fenv->unknown_none,decl->loc,0);}case 1: _tmp4B9=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4B8)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp4B9;
# 2871
Cyc_NewControlFlow_check_nested_fun(env->all_tables,fenv,inflow,fd);{
struct Cyc_Absyn_Vardecl*_tmp4C2=(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);struct Cyc_Absyn_Vardecl*vd=_tmp4C2;
# 2876
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp708=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp707=inflow;struct Cyc_List_List*_tmp706=({struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->hd=vd,_tmp4C3->tl=0;_tmp4C3;});void*_tmp705=fenv->unknown_all;Cyc_NewControlFlow_add_vars(_tmp708,_tmp707,_tmp706,_tmp705,decl->loc,0);});}}case 4: _tmp4B9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4B8)->f1;_tmp4BA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4B8)->f2;_tmp4BB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4B8)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp4B9;struct Cyc_Absyn_Vardecl*vd=_tmp4BA;struct Cyc_Absyn_Exp*open_exp_opt=_tmp4BB;
# 2879
if(open_exp_opt != 0)
({void*_tmp4C4=0U;({struct _fat_ptr _tmp709=({const char*_tmp4C5="found open expression in declaration!";_tag_fat(_tmp4C5,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp709,_tag_fat(_tmp4C4,sizeof(void*),0));});});{
struct Cyc_List_List _tmp4C6=({struct Cyc_List_List _tmp5AD;_tmp5AD.hd=vd,_tmp5AD.tl=0;_tmp5AD;});struct Cyc_List_List vds=_tmp4C6;
return Cyc_NewControlFlow_add_vars(fenv,inflow,& vds,fenv->unknown_all,decl->loc,0);}}default:
# 2885
({void*_tmp4C7=0U;({struct _fat_ptr _tmp70A=({const char*_tmp4C8="anal_decl: unexpected decl variant";_tag_fat(_tmp4C8,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp70A,_tag_fat(_tmp4C7,sizeof(void*),0));});});};}
# 2893
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp4C9;_push_handler(& _tmp4C9);{int _tmp4CB=0;if(setjmp(_tmp4C9.handler))_tmp4CB=1;if(!_tmp4CB){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp70D=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp70C=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp70B=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp70D,_tmp70C,_tmp70B,fd);});}
# 2895
;_pop_handler();}else{void*_tmp4CA=(void*)Cyc_Core_get_exn_thrown();void*_tmp4CC=_tmp4CA;void*_tmp4CD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4CC)->tag == Cyc_Dict_Absent){
# 2901
if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE->tag=Cyc_Dict_Absent;_tmp4CE;}));}}else{_tmp4CD=_tmp4CC;{void*exn=_tmp4CD;(int)_rethrow(exn);}}_LL0:;}}}
# 2907
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2912
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp4CF=r;unsigned _tmp4D0;if((_tmp4CF.RParam).tag == 5){_tmp4D0=(_tmp4CF.RParam).val;{unsigned i=_tmp4D0;
# 2915
struct Cyc_Absyn_Vardecl*_tmp4D1=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(vds,(int)i);struct Cyc_Absyn_Vardecl*vd=_tmp4D1;
if(!vd->escapes)
return Cyc_Relations_RVar(vd);
return r;}}else{
return r;};}
# 2926
static struct Cyc_List_List*Cyc_NewControlFlow_get_noconsume_params(struct Cyc_List_List*param_vardecls,struct Cyc_List_List*atts){
# 2928
struct _RegionHandle _tmp4D2=_new_region("r");struct _RegionHandle*r=& _tmp4D2;_push_region(r);
{int _tmp4D3=((int(*)(struct Cyc_List_List*))Cyc_List_length)(param_vardecls);int len=_tmp4D3;
struct _fat_ptr _tmp4D4=({unsigned _tmp4E0=(unsigned)len;int*_tmp4DF=({struct _RegionHandle*_tmp70E=r;_region_malloc(_tmp70E,_check_times(_tmp4E0,sizeof(int)));});({{unsigned _tmp5AE=(unsigned)len;unsigned i;for(i=0;i < _tmp5AE;++ i){_tmp4DF[i]=0;}}0;});_tag_fat(_tmp4DF,sizeof(int),_tmp4E0);});struct _fat_ptr cons=_tmp4D4;
for(1;atts != 0;atts=atts->tl){
void*_tmp4D5=(void*)atts->hd;void*_stmttmp9E=_tmp4D5;void*_tmp4D6=_stmttmp9E;int _tmp4D7;switch(*((int*)_tmp4D6)){case 22: _tmp4D7=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp4D6)->f1;{int i=_tmp4D7;
_tmp4D7=i;goto _LL4;}case 21: _tmp4D7=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp4D6)->f1;_LL4: {int i=_tmp4D7;
*((int*)_check_fat_subscript(cons,sizeof(int),i - 1))=1;goto _LL0;}default:
 goto _LL0;}_LL0:;}{
# 2938
struct Cyc_List_List*noconsume_roots=0;
{int i=0;for(0;param_vardecls != 0;(param_vardecls=param_vardecls->tl,i ++)){
struct Cyc_Absyn_Vardecl*_tmp4D8=(struct Cyc_Absyn_Vardecl*)param_vardecls->hd;struct Cyc_Absyn_Vardecl*vd=_tmp4D8;
if(Cyc_Tcutil_is_noalias_pointer(vd->type,0)&& !(*((int*)_check_fat_subscript(cons,sizeof(int),i)))){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D9=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->tag=0,_tmp4DD->f1=vd;_tmp4DD;});struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*root=_tmp4D9;
struct Cyc_CfFlowInfo_Place*_tmp4DA=({struct Cyc_CfFlowInfo_Place*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->root=(void*)root,_tmp4DC->path=0;_tmp4DC;});struct Cyc_CfFlowInfo_Place*rp=_tmp4DA;
noconsume_roots=({struct Cyc_List_List*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->hd=rp,_tmp4DB->tl=noconsume_roots;_tmp4DB;});}}}{
# 2947
struct Cyc_List_List*_tmp4DE=noconsume_roots;_npop_handler(0);return _tmp4DE;}}}
# 2929
;_pop_region();}
# 2950
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2954
unsigned _tmp4E1=(fd->body)->loc;unsigned loc=_tmp4E1;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,loc,1);{
# 2959
struct Cyc_List_List*param_roots=0;
struct _tuple15 _tmp4E2=({union Cyc_CfFlowInfo_FlowInfo _tmp509=inflow;if((_tmp509.ReachableFL).tag != 2)_throw_match();(_tmp509.ReachableFL).val;});struct _tuple15 _stmttmp9F=_tmp4E2;struct _tuple15 _tmp4E3=_stmttmp9F;void*_tmp4E5;struct Cyc_Dict_Dict _tmp4E4;_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;{struct Cyc_Dict_Dict d=_tmp4E4;struct Cyc_List_List*relns=_tmp4E5;
# 2963
struct Cyc_List_List*_tmp4E6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vardecls=_tmp4E6;
{struct Cyc_List_List*_tmp4E7=(fd->i).requires_relns;struct Cyc_List_List*reqs=_tmp4E7;for(0;reqs != 0;reqs=reqs->tl){
struct Cyc_Relations_Reln*_tmp4E8=(struct Cyc_Relations_Reln*)reqs->hd;struct Cyc_Relations_Reln*req=_tmp4E8;
relns=({struct _RegionHandle*_tmp712=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp711=Cyc_NewControlFlow_translate_rop(vardecls,req->rop1);enum Cyc_Relations_Relation _tmp710=req->relation;union Cyc_Relations_RelnOp _tmp70F=
Cyc_NewControlFlow_translate_rop(vardecls,req->rop2);
# 2966
Cyc_Relations_add_relation(_tmp712,_tmp711,_tmp710,_tmp70F,relns);});}}{
# 2972
struct Cyc_List_List*atts;
{void*_tmp4E9=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmpA0=_tmp4E9;void*_tmp4EA=_stmttmpA0;void*_tmp4EB;if(*((int*)_tmp4EA)== 5){_tmp4EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4EA)->f1).attributes;{struct Cyc_List_List*as=_tmp4EB;
atts=as;goto _LL3;}}else{
({void*_tmp4EC=0U;({struct _fat_ptr _tmp713=({const char*_tmp4ED="check_fun: non-function type cached with fndecl_t";_tag_fat(_tmp4ED,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp713,_tag_fat(_tmp4EC,sizeof(void*),0));});});}_LL3:;}{
# 2977
struct Cyc_List_List*noconsume_roots=Cyc_NewControlFlow_get_noconsume_params((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,atts);
# 2980
for(1;atts != 0;atts=atts->tl){
void*_tmp4EE=(void*)atts->hd;void*_stmttmpA1=_tmp4EE;void*_tmp4EF=_stmttmpA1;int _tmp4F0;switch(*((int*)_tmp4EF)){case 21: _tmp4F0=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp4EF)->f1;{int i=_tmp4F0;
# 2983
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp4F1=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,i - 1);struct Cyc_Absyn_Vardecl*vd=_tmp4F1;
void*t=Cyc_Absyn_compress(vd->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp4F2=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp716=fenv;void*_tmp715=elttype;struct Cyc_Absyn_Exp*_tmp714=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp716,_tmp715,_tmp714,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all),0);});
# 2987
void*rval=_tmp4F2;
# 2991
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4F3=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->tag=2,_tmp4F8->f1=i,_tmp4F8->f2=t;_tmp4F8;});struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r=_tmp4F3;
struct Cyc_CfFlowInfo_Place*_tmp4F4=({struct Cyc_CfFlowInfo_Place*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->root=(void*)r,_tmp4F7->path=0;_tmp4F7;});struct Cyc_CfFlowInfo_Place*rp=_tmp4F4;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(d,(void*)r,rval);
d=({struct Cyc_Dict_Dict _tmp718=d;void*_tmp717=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->tag=0,_tmp4F5->f1=vd;_tmp4F5;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp718,_tmp717,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->tag=4,_tmp4F6->f1=rp;_tmp4F6;}));});
goto _LL8;}case 20: _tmp4F0=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp4EF)->f1;{int i=_tmp4F0;
# 2997
struct Cyc_Absyn_Vardecl*_tmp4F9=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,int))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,i - 1);struct Cyc_Absyn_Vardecl*vd=_tmp4F9;
void*elttype=Cyc_Tcutil_pointer_elt_type(vd->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp4FA=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->tag=2,_tmp500->f1=i,_tmp500->f2=elttype;_tmp500;});struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*r=_tmp4FA;
struct Cyc_CfFlowInfo_Place*_tmp4FB=({struct Cyc_CfFlowInfo_Place*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->root=(void*)r,_tmp4FF->path=0;_tmp4FF;});struct Cyc_CfFlowInfo_Place*rp=_tmp4FB;
d=({struct Cyc_Dict_Dict _tmp71A=d;void*_tmp719=(void*)r;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp71A,_tmp719,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});
d=({struct Cyc_Dict_Dict _tmp71C=d;void*_tmp71B=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->tag=0,_tmp4FC->f1=vd;_tmp4FC;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert)(_tmp71C,_tmp71B,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->tag=4,_tmp4FD->f1=rp;_tmp4FD;}));});
param_roots=({struct Cyc_List_List*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->hd=rp,_tmp4FE->tl=param_roots;_tmp4FE;});
goto _LL8;}default:
 goto _LL8;}_LL8:;}
# 3008
inflow=Cyc_CfFlowInfo_ReachableFL(d,relns);{
# 3010
int noreturn=Cyc_Atts_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp508=_cycalloc(sizeof(*_tmp508));
_tmp508->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp71E=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp508->succ_table=_tmp71E;}),({
struct Cyc_Hashtable_Table*_tmp71D=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp508->pat_pop_table=_tmp71D;}),_tmp508->fenv=fenv,_tmp508->iterate_again=1,_tmp508->iteration_num=0,_tmp508->in_try=0,_tmp508->tryflow=inflow,_tmp508->noreturn=noreturn,_tmp508->return_type=(fd->i).ret_type,_tmp508->unique_pat_vars=0,_tmp508->param_roots=param_roots,_tmp508->noconsume_params=noconsume_roots,_tmp508->flow_table=flow_table,_tmp508->return_relns=(fd->i).ensures_relns;_tmp508;});
# 3020
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3026
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 3029
union Cyc_CfFlowInfo_FlowInfo _tmp501=outflow;if((_tmp501.BottomFL).tag == 1)
goto _LLF;else{
# 3032
Cyc_NewControlFlow_check_init_params(loc,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,loc);
# 3036
if(noreturn)
({void*_tmp502=0U;({unsigned _tmp720=loc;struct _fat_ptr _tmp71F=({const char*_tmp503="`noreturn' function might (implicitly) return";_tag_fat(_tmp503,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp720,_tmp71F,_tag_fat(_tmp502,sizeof(void*),0));});});else{
if(!Cyc_Tcutil_is_void_type((fd->i).ret_type)){
if(Cyc_Tcutil_is_any_float_type((fd->i).ret_type)||
 Cyc_Tcutil_is_any_int_type((fd->i).ret_type))
({void*_tmp504=0U;({unsigned _tmp722=loc;struct _fat_ptr _tmp721=({const char*_tmp505="function may not return a value";_tag_fat(_tmp505,sizeof(char),32U);});Cyc_Warn_warn(_tmp722,_tmp721,_tag_fat(_tmp504,sizeof(void*),0));});});else{
# 3043
({void*_tmp506=0U;({unsigned _tmp724=loc;struct _fat_ptr _tmp723=({const char*_tmp507="function may not return a value";_tag_fat(_tmp507,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp724,_tmp723,_tag_fat(_tmp506,sizeof(void*),0));});});}goto _LLF;}}
# 3045
goto _LLF;}_LLF:;}}}}}}}
# 3049
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(1;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp50A=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_stmttmpA2=_tmp50A;void*_tmp50B=_stmttmpA2;void*_tmp50C;switch(*((int*)_tmp50B)){case 1: _tmp50C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp50B)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp50C;
Cyc_NewControlFlow_check_fun(tables,fd);goto _LL0;}case 11: _tmp50C=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp50B)->f1;{struct Cyc_List_List*ds2=_tmp50C;
# 3055
_tmp50C=ds2;goto _LL6;}case 10: _tmp50C=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp50B)->f2;_LL6: {struct Cyc_List_List*ds2=_tmp50C;
_tmp50C=ds2;goto _LL8;}case 9: _tmp50C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp50B)->f2;_LL8: {struct Cyc_List_List*ds2=_tmp50C;
Cyc_NewControlFlow_cf_check(tables,ds2);goto _LL0;}case 12:
 goto _LL0;default:
 goto _LL0;}_LL0:;}}}
