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
 struct Cyc_Core_Opt{void*v;};
# 126 "core.h"
extern int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "/tmp/cyclone/include/list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145
extern void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
extern int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 37 "/tmp/cyclone/include/position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "/tmp/cyclone/src/absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900 "/tmp/cyclone/src/absyn.h"
void*Cyc_Absyn_compress(void*);
# 917
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);struct Cyc_Hashtable_Table;
# 39 "/tmp/cyclone/include/hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*));struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 27 "/tmp/cyclone/src/vcgen.h"
void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);struct Cyc___cycFILE;
# 53 "/tmp/cyclone/include/cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "/tmp/cyclone/src/absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "/tmp/cyclone/src/relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "/tmp/cyclone/src/tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 175
int Cyc_Tcutil_typecmp(void*,void*);
# 260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 25 "/tmp/cyclone/src/attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);struct _tuple11{unsigned f1;int f2;};
# 28 "/tmp/cyclone/src/evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 46 "/tmp/cyclone/src/evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "/tmp/cyclone/include/dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 126 "/tmp/cyclone/include/dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 49 "/tmp/cyclone/include/string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
# 35 "/tmp/cyclone/src/warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct Cyc_Vcgen_Env;struct Cyc_Vcgen_Const_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Vcgen_Var_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;};struct Cyc_Vcgen_Primop_Vcgen_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Vcgen_Cast_Vcgen_Term_struct{int tag;void*f1;void*f2;};
# 127 "/tmp/cyclone/src/vcgen.cyc"
struct _fat_ptr Cyc_Vcgen_term2string(void*t){
void*_tmp0=t;void*_tmp2;void*_tmp1;void*_tmp4;enum Cyc_Absyn_Primop _tmp3;void*_tmp7;void*_tmp6;enum Cyc_Absyn_Primop _tmp5;int _tmp9;struct Cyc_Absyn_Vardecl*_tmp8;int _tmpA;struct Cyc_Absyn_Vardecl*_tmpB;struct Cyc_Absyn_Exp*_tmpC;switch(*((int*)_tmp0)){case 0U: _LL1: _tmpC=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp0)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpC;
return Cyc_Absynpp_exp2string(e);}case 1U: _LL3: _tmpB=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp0)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpB;
return Cyc_Absynpp_qvar2string(vd->name);}case 2U: if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f1 == 0){_LL5: _tmpA=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f2;_LL6: {int i=_tmpA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=1U,_tmp3AB.f1=(unsigned long)i;_tmp3AB;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp428=({const char*_tmpE="_X%d";_tag_fat(_tmpE,sizeof(char),5U);});Cyc_aprintf(_tmp428,_tag_fat(_tmpD,sizeof(void*),1U));});});}}else{_LL7: _tmp8=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f1;_tmp9=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp8;int i=_tmp9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0U,({struct _fat_ptr _tmp429=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name));_tmp3AD.f1=_tmp429;});_tmp3AD;});struct Cyc_Int_pa_PrintArg_struct _tmp13=({struct Cyc_Int_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=1U,_tmp3AC.f1=(unsigned long)i;_tmp3AC;});void*_tmp10[2U];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13;({struct _fat_ptr _tmp42A=({const char*_tmp11="_%s%d";_tag_fat(_tmp11,sizeof(char),6U);});Cyc_aprintf(_tmp42A,_tag_fat(_tmp10,sizeof(void*),2U));});});}}case 3U: if(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_LL9: _tmp5=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f1;_tmp6=(void*)(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->hd;_tmp7=(void*)((((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl)->hd;_LLA: {enum Cyc_Absyn_Primop p=_tmp5;void*t1=_tmp6;void*t2=_tmp7;
# 134
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0U,({struct _fat_ptr _tmp42B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t1));_tmp3B0.f1=_tmp42B;});_tmp3B0;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U,({
struct _fat_ptr _tmp42C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp3AF.f1=_tmp42C;});_tmp3AF;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U,({struct _fat_ptr _tmp42D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t2));_tmp3AE.f1=_tmp42D;});_tmp3AE;});void*_tmp14[3U];_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17,_tmp14[2]=& _tmp18;({struct _fat_ptr _tmp42E=({const char*_tmp15="(%s%s%s)";_tag_fat(_tmp15,sizeof(char),9U);});Cyc_aprintf(_tmp42E,_tag_fat(_tmp14,sizeof(void*),3U));});});}}else{goto _LLB;}}else{_LLB: _tmp3=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f1;_tmp4=(void*)(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp3;void*t=_tmp4;
# 137
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0U,({struct _fat_ptr _tmp42F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp3B2.f1=_tmp42F;});_tmp3B2;});struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0U,({struct _fat_ptr _tmp430=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp3B1.f1=_tmp430;});_tmp3B1;});void*_tmp19[2U];_tmp19[0]=& _tmp1B,_tmp19[1]=& _tmp1C;({struct _fat_ptr _tmp431=({const char*_tmp1A="%s(%s)";_tag_fat(_tmp1A,sizeof(char),7U);});Cyc_aprintf(_tmp431,_tag_fat(_tmp19,sizeof(void*),2U));});});}}}else{_LLF: _LL10:
# 140
({void*_tmp21=0U;({struct _fat_ptr _tmp432=({const char*_tmp22="term2string primop args";_tag_fat(_tmp22,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp432,_tag_fat(_tmp21,sizeof(void*),0U));});});}default: _LLD: _tmp1=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp0)->f2;_LLE: {void*tp=_tmp1;void*tm=_tmp2;
# 139
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0U,({struct _fat_ptr _tmp433=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_tmp3B4.f1=_tmp433;});_tmp3B4;});struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U,({struct _fat_ptr _tmp434=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp3B3.f1=_tmp434;});_tmp3B3;});void*_tmp1D[2U];_tmp1D[0]=& _tmp1F,_tmp1D[1]=& _tmp20;({struct _fat_ptr _tmp435=({const char*_tmp1E="(%s)%s";_tag_fat(_tmp1E,sizeof(char),7U);});Cyc_aprintf(_tmp435,_tag_fat(_tmp1D,sizeof(void*),2U));});});}}_LL0:;}
# 144
static void*Cyc_Vcgen_cnst(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=0U,_tmp23->f1=e;_tmp23;});}
static void*Cyc_Vcgen_zero (void){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp24=Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U);struct Cyc_Absyn_Exp*e=_tmp24;
({void*_tmp436=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp436;});{
void*_tmp25=Cyc_Vcgen_cnst(e);void*r=_tmp25;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 155
static void*Cyc_Vcgen_one (void){
static void*n=0;
if(n == 0){
struct Cyc_Absyn_Exp*_tmp26=Cyc_Absyn_int_exp(Cyc_Absyn_None,1,0U);struct Cyc_Absyn_Exp*e=_tmp26;
({void*_tmp437=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp437;});{
void*_tmp27=Cyc_Vcgen_cnst(e);void*r=_tmp27;
n=r;
return r;}}else{
return(void*)_check_null(n);}}
# 166
static void*Cyc_Vcgen_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=1U,_tmp28->f1=vd;_tmp28;});}
# 168
inline static int Cyc_Vcgen_is_commutative(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp29=p;switch(_tmp29){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Times: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Eq: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Neq: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Bitand: _LL9: _LLA:
 goto _LLC;case Cyc_Absyn_Bitor: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitxor: _LLD: _LLE:
 return 1;default: _LLF: _LL10:
 return 0;}_LL0:;}
# 180
static void*Cyc_Vcgen_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=3U,_tmp2A->f1=p,_tmp2A->f2=ts;_tmp2A;});}
# 183
static void*Cyc_Vcgen_cast(void*tp,void*tm){
return(void*)({struct Cyc_Vcgen_Cast_Vcgen_Term_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=4U,_tmp2B->f1=tp,_tmp2B->f2=tm;_tmp2B;});}
# 186
static void*Cyc_Vcgen_fresh_var (void){
static int counter=0;
return(void*)({struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=2U,_tmp2C->f1=0,_tmp2C->f2=counter ++;_tmp2C;});}struct _tuple12{void*f1;void*f2;};
# 191
static int Cyc_Vcgen_cmp_term(void*t1,void*t2){
struct _tuple12 _tmp2D=({struct _tuple12 _tmp3B5;_tmp3B5.f1=t1,_tmp3B5.f2=t2;_tmp3B5;});struct _tuple12 _stmttmp0=_tmp2D;struct _tuple12 _tmp2E=_stmttmp0;struct Cyc_List_List*_tmp32;enum Cyc_Absyn_Primop _tmp31;struct Cyc_List_List*_tmp30;enum Cyc_Absyn_Primop _tmp2F;void*_tmp36;void*_tmp35;void*_tmp34;void*_tmp33;int _tmp3A;struct Cyc_Absyn_Vardecl*_tmp39;int _tmp38;struct Cyc_Absyn_Vardecl*_tmp37;struct Cyc_Absyn_Vardecl*_tmp3C;struct Cyc_Absyn_Vardecl*_tmp3B;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2E.f1)->tag == 0U){if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2E.f2)->tag == 0U){_LL1: _tmp3D=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2E.f1)->f1;_tmp3E=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2E.f2)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3D;struct Cyc_Absyn_Exp*e2=_tmp3E;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp2E.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2E.f1)->tag == 1U){if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2E.f2)->tag == 1U){_LL7: _tmp3B=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2E.f1)->f1;_tmp3C=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2E.f2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd1=_tmp3B;struct Cyc_Absyn_Vardecl*vd2=_tmp3C;
return(int)vd1 - (int)vd2;}}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp2E.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f1)->tag == 2U){if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f2)->tag == 2U){_LLD: _tmp37=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f1)->f1;_tmp38=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f1)->f2;_tmp39=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f2)->f1;_tmp3A=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f2)->f2;_LLE: {struct Cyc_Absyn_Vardecl*vd1=_tmp37;int i=_tmp38;struct Cyc_Absyn_Vardecl*vd2=_tmp39;int j=_tmp3A;
# 200
if(vd1 == vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{_LLF: _LL10:
 return - 1;}}else{if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp2E.f2)->tag == 2U){_LL11: _LL12:
 return 1;}else{if(((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f1)->tag == 4U){if(((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f2)->tag == 4U){_LL13: _tmp33=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f1)->f1;_tmp34=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f1)->f2;_tmp35=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f2)->f1;_tmp36=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f2)->f2;_LL14: {void*tp1=_tmp33;void*tm1=_tmp34;void*tp2=_tmp35;void*tm2=_tmp36;
# 205
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c != 0)return c;
return Cyc_Vcgen_cmp_term(tm1,tm2);}}else{_LL15: _LL16:
 return - 1;}}else{if(((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp2E.f2)->tag == 4U){_LL17: _LL18:
 return 1;}else{_LL19: _tmp2F=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2E.f1)->f1;_tmp30=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2E.f1)->f2;_tmp31=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2E.f2)->f1;_tmp32=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp2E.f2)->f2;_LL1A: {enum Cyc_Absyn_Primop p1=_tmp2F;struct Cyc_List_List*ts1=_tmp30;enum Cyc_Absyn_Primop p2=_tmp31;struct Cyc_List_List*ts2=_tmp32;
# 211
if((int)p1 != (int)p2)return(int)p1 - (int)p2;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Vcgen_cmp_term,ts1,ts2);}}}}}}}}}_LL0:;}
# 216
static int Cyc_Vcgen_same_term(void*t1,void*t2){return Cyc_Vcgen_cmp_term(t1,t2)== 0;}
# 221
enum Cyc_Vcgen_Primreln{Cyc_Vcgen_Eq =0U,Cyc_Vcgen_Neq =1U,Cyc_Vcgen_SLt =2U,Cyc_Vcgen_SLte =3U,Cyc_Vcgen_ULt =4U,Cyc_Vcgen_ULte =5U};struct Cyc_Vcgen_True_Vcgen_Assn_struct{int tag;};struct Cyc_Vcgen_False_Vcgen_Assn_struct{int tag;};struct Cyc_Vcgen_Prim_Vcgen_Assn_struct{int tag;void*f1;enum Cyc_Vcgen_Primreln f2;void*f3;};struct Cyc_Vcgen_And_Vcgen_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_Vcgen_Or_Vcgen_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_Vcgen_Subst_Vcgen_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_Vcgen_Kill_Vcgen_Assn_struct{int tag;void*f1;};
# 235
static unsigned Cyc_Vcgen_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 237
LOOP:
 if(((int(*)(struct Cyc_List_List*l,void*x))Cyc_List_memq)(*prev,a))return c;
({struct Cyc_List_List*_tmp438=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->hd=a,_tmp3F->tl=*prev;_tmp3F;});*prev=_tmp438;});{
void*_tmp40=a;void*_tmp41;void*_tmp44;void*_tmp43;struct Cyc_Absyn_Vardecl*_tmp42;void*_tmp46;void*_tmp45;void*_tmp48;void*_tmp47;switch(*((int*)_tmp40)){case 0U: _LL1: _LL2:
 return c;case 1U: _LL3: _LL4:
 return c;case 2U: _LL5: _LL6:
 return(unsigned)1 + c;case 3U: _LL7: _tmp47=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp40)->f1;_tmp48=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp40)->f2;_LL8: {void*a1=_tmp47;void*a2=_tmp48;
_tmp45=a1;_tmp46=a2;goto _LLA;}case 4U: _LL9: _tmp45=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp40)->f1;_tmp46=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp40)->f2;_LLA: {void*a1=_tmp45;void*a2=_tmp46;
# 246
c=Cyc_Vcgen_assn_size_loop(a2,(unsigned)1 + c,prev);
a=a1;
goto LOOP;}case 5U: _LLB: _tmp42=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp40)->f1;_tmp43=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp40)->f2;_tmp44=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp40)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp42;void*t=_tmp43;void*a1=_tmp44;
# 250
++ c;
a=a1;
goto LOOP;}default: _LLD: _tmp41=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp40)->f1;_LLE: {void*a1=_tmp41;
# 254
++ c;
a=a1;
goto LOOP;}}_LL0:;}}
# 261
static unsigned Cyc_Vcgen_max_assn_size=0U;
# 264
static unsigned Cyc_Vcgen_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp49=Cyc_Vcgen_assn_size_loop(a,0U,& prev);unsigned c=_tmp49;
if(c > Cyc_Vcgen_max_assn_size)
Cyc_Vcgen_max_assn_size=c;
# 270
return c;}
# 275
static struct _fat_ptr Cyc_Vcgen_a2string(void*a,int prec,int*size){
void*_tmp4A=a;void*_tmp4B;void*_tmp4E;void*_tmp4D;struct Cyc_Absyn_Vardecl*_tmp4C;void*_tmp50;void*_tmp4F;void*_tmp52;void*_tmp51;void*_tmp55;enum Cyc_Vcgen_Primreln _tmp54;void*_tmp53;switch(*((int*)_tmp4A)){case 0U: _LL1: _LL2:
 return({const char*_tmp56="true";_tag_fat(_tmp56,sizeof(char),5U);});case 1U: _LL3: _LL4:
 return({const char*_tmp57="false";_tag_fat(_tmp57,sizeof(char),6U);});case 2U: _LL5: _tmp53=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp4A)->f1;_tmp54=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp4A)->f2;_tmp55=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp4A)->f3;_LL6: {void*t1=_tmp53;enum Cyc_Vcgen_Primreln p=_tmp54;void*t2=_tmp55;
# 280
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_Vcgen_Primreln _tmp58=p;switch(_tmp58){case Cyc_Vcgen_Eq: _LL10: _LL11:
 ps=({const char*_tmp59="==";_tag_fat(_tmp59,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_Neq: _LL12: _LL13:
 ps=({const char*_tmp5A="!=";_tag_fat(_tmp5A,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_SLt: _LL14: _LL15:
 ps=({const char*_tmp5B="S<";_tag_fat(_tmp5B,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_SLte: _LL16: _LL17:
 ps=({const char*_tmp5C="S<=";_tag_fat(_tmp5C,sizeof(char),4U);});goto _LLF;case Cyc_Vcgen_ULt: _LL18: _LL19:
 ps=({const char*_tmp5D="U<";_tag_fat(_tmp5D,sizeof(char),3U);});goto _LLF;case Cyc_Vcgen_ULte: _LL1A: _LL1B:
 ps=({const char*_tmp5E="U<=";_tag_fat(_tmp5E,sizeof(char),4U);});goto _LLF;default: _LL1C: _LL1D:
({void*_tmp5F=0U;({struct _fat_ptr _tmp439=({const char*_tmp60="assn2string primop";_tag_fat(_tmp60,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp439,_tag_fat(_tmp5F,sizeof(void*),0U));});});}_LLF:;}
# 291
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0U,({struct _fat_ptr _tmp43A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t1));_tmp3B8.f1=_tmp43A;});_tmp3B8;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0U,_tmp3B7.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp3B7;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0U,({struct _fat_ptr _tmp43B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t2));_tmp3B6.f1=_tmp43B;});_tmp3B6;});void*_tmp61[3U];_tmp61[0]=& _tmp63,_tmp61[1]=& _tmp64,_tmp61[2]=& _tmp65;({struct _fat_ptr _tmp43C=({const char*_tmp62="%s%s%s";_tag_fat(_tmp62,sizeof(char),7U);});Cyc_aprintf(_tmp43C,_tag_fat(_tmp61,sizeof(void*),3U));});});}}case 4U: _LL7: _tmp51=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp4A)->f1;_tmp52=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp4A)->f2;_LL8: {void*a1=_tmp51;void*a2=_tmp52;
# 293
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0U,({struct _fat_ptr _tmp43D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,5,size));_tmp3BA.f1=_tmp43D;});_tmp3BA;});struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0U,({struct _fat_ptr _tmp43E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,5,size));_tmp3B9.f1=_tmp43E;});_tmp3B9;});void*_tmp66[2U];_tmp66[0]=& _tmp68,_tmp66[1]=& _tmp69;({struct _fat_ptr _tmp43F=({const char*_tmp67="%s || %s";_tag_fat(_tmp67,sizeof(char),9U);});Cyc_aprintf(_tmp43F,_tag_fat(_tmp66,sizeof(void*),2U));});});else{
# 297
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0U,({struct _fat_ptr _tmp440=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,5,size));_tmp3BC.f1=_tmp440;});_tmp3BC;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0U,({struct _fat_ptr _tmp441=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,5,size));_tmp3BB.f1=_tmp441;});_tmp3BB;});void*_tmp6A[2U];_tmp6A[0]=& _tmp6C,_tmp6A[1]=& _tmp6D;({struct _fat_ptr _tmp442=({const char*_tmp6B="(%s || %s)";_tag_fat(_tmp6B,sizeof(char),11U);});Cyc_aprintf(_tmp442,_tag_fat(_tmp6A,sizeof(void*),2U));});});}}case 3U: _LL9: _tmp4F=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp4A)->f1;_tmp50=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp4A)->f2;_LLA: {void*a1=_tmp4F;void*a2=_tmp50;
# 299
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0U,({struct _fat_ptr _tmp443=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,10,size));_tmp3BE.f1=_tmp443;});_tmp3BE;});struct Cyc_String_pa_PrintArg_struct _tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0U,({struct _fat_ptr _tmp444=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,10,size));_tmp3BD.f1=_tmp444;});_tmp3BD;});void*_tmp6E[2U];_tmp6E[0]=& _tmp70,_tmp6E[1]=& _tmp71;({struct _fat_ptr _tmp445=({const char*_tmp6F="%s && %s";_tag_fat(_tmp6F,sizeof(char),9U);});Cyc_aprintf(_tmp445,_tag_fat(_tmp6E,sizeof(void*),2U));});});}case 5U: _LLB: _tmp4C=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp4A)->f1;_tmp4D=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp4A)->f2;_tmp4E=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp4A)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;void*t=_tmp4D;void*a=_tmp4E;
# 302
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0U,({struct _fat_ptr _tmp446=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp3C1.f1=_tmp446;});_tmp3C1;});struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0U,({
struct _fat_ptr _tmp447=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp3C0.f1=_tmp447;});_tmp3C0;});struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0U,({struct _fat_ptr _tmp448=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a,0,size));_tmp3BF.f1=_tmp448;});_tmp3BF;});void*_tmp72[3U];_tmp72[0]=& _tmp74,_tmp72[1]=& _tmp75,_tmp72[2]=& _tmp76;({struct _fat_ptr _tmp449=({const char*_tmp73="Subst[%s:=%s](%s)";_tag_fat(_tmp73,sizeof(char),18U);});Cyc_aprintf(_tmp449,_tag_fat(_tmp72,sizeof(void*),3U));});});}default: _LLD: _tmp4B=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp4A)->f1;_LLE: {void*a=_tmp4B;
# 306
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0U,({struct _fat_ptr _tmp44A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a,0,size));_tmp3C2.f1=_tmp44A;});_tmp3C2;});void*_tmp77[1U];_tmp77[0]=& _tmp79;({struct _fat_ptr _tmp44B=({const char*_tmp78="Kill(%s)";_tag_fat(_tmp78,sizeof(char),9U);});Cyc_aprintf(_tmp44B,_tag_fat(_tmp77,sizeof(void*),1U));});});}}_LL0:;}
# 311
static int Cyc_Vcgen_tree_size=0;
# 314
struct _fat_ptr Cyc_Vcgen_assn2string(void*a){
Cyc_Vcgen_tree_size=0;
return Cyc_Vcgen_a2string(a,0,& Cyc_Vcgen_tree_size);}
# 320
static struct Cyc_Vcgen_True_Vcgen_Assn_struct Cyc_Vcgen_true_assn={0U};
static struct Cyc_Vcgen_False_Vcgen_Assn_struct Cyc_Vcgen_false_assn={1U};
# 331 "/tmp/cyclone/src/vcgen.cyc"
static int Cyc_Vcgen_assnhash(void*a){
void*_tmp7A=a;void*_tmp7B;void*_tmp7E;void*_tmp7D;struct Cyc_Absyn_Vardecl*_tmp7C;void*_tmp80;void*_tmp7F;void*_tmp82;void*_tmp81;void*_tmp85;enum Cyc_Vcgen_Primreln _tmp84;void*_tmp83;switch(*((int*)_tmp7A)){case 1U: _LL1: _LL2:
 return 0;case 0U: _LL3: _LL4:
 return 1;case 2U: _LL5: _tmp83=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp7A)->f1;_tmp84=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp7A)->f2;_tmp85=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp7A)->f3;_LL6: {void*t1=_tmp83;enum Cyc_Vcgen_Primreln p=_tmp84;void*t2=_tmp85;
# 336
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3U: _LL7: _tmp81=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp7A)->f1;_tmp82=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp7A)->f2;_LL8: {void*a1=_tmp81;void*a2=_tmp82;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4U: _LL9: _tmp7F=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp7A)->f1;_tmp80=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp7A)->f2;_LLA: {void*a1=_tmp7F;void*a2=_tmp80;
return(int)((unsigned)a1 ^ (unsigned)a2 | (unsigned)1);}case 5U: _LLB: _tmp7C=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp7A)->f1;_tmp7D=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp7A)->f2;_tmp7E=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp7A)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp7C;void*t=_tmp7D;void*a=_tmp7E;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | (unsigned)3);}default: _LLD: _tmp7B=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp7A)->f1;_LLE: {void*a=_tmp7B;
# 341
return(int)((unsigned)a + (unsigned)37);}}_LL0:;}
# 347
static int Cyc_Vcgen_assncmp(void*a1,void*a2){
LOOP:
 if(a1 == a2)return 0;{
struct _tuple12 _tmp86=({struct _tuple12 _tmp3C3;_tmp3C3.f1=a1,_tmp3C3.f2=a2;_tmp3C3;});struct _tuple12 _stmttmp1=_tmp86;struct _tuple12 _tmp87=_stmttmp1;void*_tmp8D;void*_tmp8C;struct Cyc_Absyn_Vardecl*_tmp8B;void*_tmp8A;void*_tmp89;struct Cyc_Absyn_Vardecl*_tmp88;void*_tmp8F;void*_tmp8E;void*_tmp95;enum Cyc_Vcgen_Primreln _tmp94;void*_tmp93;void*_tmp92;enum Cyc_Vcgen_Primreln _tmp91;void*_tmp90;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;void*_tmp9D;void*_tmp9C;void*_tmp9B;void*_tmp9A;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp87.f1)->tag == 0U){if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp87.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp87.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp87.f1)->tag == 1U){if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp87.f2)->tag == 1U){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp87.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{switch(*((int*)_tmp87.f1)){case 4U: if(((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp87.f2)->tag == 4U){_LLD: _tmp9A=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp87.f1)->f1;_tmp9B=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp87.f1)->f2;_tmp9C=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp87.f2)->f1;_tmp9D=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp87.f2)->f2;_LLE: {void*a11=_tmp9A;void*a12=_tmp9B;void*a21=_tmp9C;void*a22=_tmp9D;
_tmp96=a11;_tmp97=a12;_tmp98=a21;_tmp99=a22;goto _LL10;}}else{_LL11: _LL12:
# 364
 return - 1;}case 3U: switch(*((int*)_tmp87.f2)){case 3U: _LLF: _tmp96=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp87.f1)->f1;_tmp97=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp87.f1)->f2;_tmp98=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp87.f2)->f1;_tmp99=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp87.f2)->f2;_LL10: {void*a11=_tmp96;void*a12=_tmp97;void*a21=_tmp98;void*a22=_tmp99;
# 359
int c=Cyc_Vcgen_assncmp(a11,a21);
if(c != 0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4U: goto _LL13;default: _LL15: _LL16:
# 366
 return - 1;}default: switch(*((int*)_tmp87.f2)){case 4U: _LL13: _LL14:
# 365
 return 1;case 3U: _LL17: _LL18:
# 367
 return 1;default: if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f1)->tag == 2U){if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f2)->tag == 2U){_LL19: _tmp90=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f1)->f1;_tmp91=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f1)->f2;_tmp92=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f1)->f3;_tmp93=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f2)->f1;_tmp94=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f2)->f2;_tmp95=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f2)->f3;_LL1A: {void*t11=_tmp90;enum Cyc_Vcgen_Primreln p1=_tmp91;void*t12=_tmp92;void*t21=_tmp93;enum Cyc_Vcgen_Primreln p2=_tmp94;void*t22=_tmp95;
# 369
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_Vcgen_cmp_term(t11,t21);
if(c != 0)return c;
return Cyc_Vcgen_cmp_term(t12,t22);}}else{_LL1B: _LL1C:
 return - 1;}}else{if(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp87.f2)->tag == 2U){_LL1D: _LL1E:
 return 1;}else{if(((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp87.f1)->tag == 6U){if(((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp87.f2)->tag == 6U){_LL1F: _tmp8E=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp87.f1)->f1;_tmp8F=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp87.f2)->f1;_LL20: {void*a1=_tmp8E;void*a2=_tmp8F;
return Cyc_Vcgen_assncmp(a1,a2);}}else{_LL21: _LL22:
 return - 1;}}else{if(((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmp87.f2)->tag == 6U){_LL23: _LL24:
 return 1;}else{_LL25: _tmp88=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp87.f1)->f1;_tmp89=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp87.f1)->f2;_tmp8A=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp87.f1)->f3;_tmp8B=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp87.f2)->f1;_tmp8C=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp87.f2)->f2;_tmp8D=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmp87.f2)->f3;_LL26: {struct Cyc_Absyn_Vardecl*vd1=_tmp88;void*t1=_tmp89;void*a11=_tmp8A;struct Cyc_Absyn_Vardecl*vd2=_tmp8B;void*t2=_tmp8C;void*a22=_tmp8D;
# 380
int c=(int)vd1 - (int)vd2;
if(c != 0)return c;
c=Cyc_Vcgen_cmp_term(t1,t2);
if(c != 0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}}_LL0:;}}
# 393
static struct Cyc_Hashtable_Table*Cyc_Vcgen_hash_cons_table=0;struct _tuple13{void*f1;int f2;};
# 398
static int Cyc_Vcgen_is_reduced(void*a){
if(Cyc_Vcgen_hash_cons_table == 0)return 0;{
struct _tuple13**_tmp9E=((struct _tuple13**(*)(struct Cyc_Hashtable_Table*t,void*key))Cyc_Hashtable_lookup_opt)((struct Cyc_Hashtable_Table*)_check_null(Cyc_Vcgen_hash_cons_table),a);struct _tuple13**resopt=_tmp9E;
if(resopt == 0)return 0;{
struct _tuple13*_tmp9F=*resopt;struct _tuple13*_stmttmp2=_tmp9F;struct _tuple13*_tmpA0=_stmttmp2;int _tmpA1;_LL1: _tmpA1=_tmpA0->f2;_LL2: {int b=_tmpA1;
return b;}}}}
# 409
static struct _tuple13 Cyc_Vcgen_copy_assn(void*a){
void*_tmpA2=a;void*_tmpA3;void*_tmpA6;void*_tmpA5;struct Cyc_Absyn_Vardecl*_tmpA4;void*_tmpA9;enum Cyc_Vcgen_Primreln _tmpA8;void*_tmpA7;void*_tmpAB;void*_tmpAA;void*_tmpAD;void*_tmpAC;switch(*((int*)_tmpA2)){case 0U: _LL1: _LL2:
 return({struct _tuple13 _tmp3C4;_tmp3C4.f1=(void*)& Cyc_Vcgen_true_assn,_tmp3C4.f2=1;_tmp3C4;});case 1U: _LL3: _LL4:
 return({struct _tuple13 _tmp3C5;_tmp3C5.f1=(void*)& Cyc_Vcgen_false_assn,_tmp3C5.f2=0;_tmp3C5;});case 3U: _LL5: _tmpAC=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpA2)->f1;_tmpAD=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpA2)->f2;_LL6: {void*a1=_tmpAC;void*a2=_tmpAD;
# 414
return({struct _tuple13 _tmp3C6;({void*_tmp44D=(void*)({struct Cyc_Vcgen_And_Vcgen_Assn_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=3U,_tmpAE->f1=a1,_tmpAE->f2=a2;_tmpAE;});_tmp3C6.f1=_tmp44D;}),({int _tmp44C=Cyc_Vcgen_is_reduced(a1)&& Cyc_Vcgen_is_reduced(a2);_tmp3C6.f2=_tmp44C;});_tmp3C6;});}case 4U: _LL7: _tmpAA=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpA2)->f1;_tmpAB=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpA2)->f2;_LL8: {void*a1=_tmpAA;void*a2=_tmpAB;
# 416
return({struct _tuple13 _tmp3C7;({void*_tmp44F=(void*)({struct Cyc_Vcgen_Or_Vcgen_Assn_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=4U,_tmpAF->f1=a1,_tmpAF->f2=a2;_tmpAF;});_tmp3C7.f1=_tmp44F;}),({int _tmp44E=Cyc_Vcgen_is_reduced(a1)&& Cyc_Vcgen_is_reduced(a2);_tmp3C7.f2=_tmp44E;});_tmp3C7;});}case 2U: _LL9: _tmpA7=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA2)->f1;_tmpA8=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA2)->f2;_tmpA9=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA2)->f3;_LLA: {void*t1=_tmpA7;enum Cyc_Vcgen_Primreln p=_tmpA8;void*t2=_tmpA9;
return({struct _tuple13 _tmp3C8;({void*_tmp450=(void*)({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,_tmpB0->f1=t1,_tmpB0->f2=p,_tmpB0->f3=t2;_tmpB0;});_tmp3C8.f1=_tmp450;}),_tmp3C8.f2=1;_tmp3C8;});}case 5U: _LLB: _tmpA4=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpA2)->f1;_tmpA5=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpA2)->f2;_tmpA6=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpA2)->f3;_LLC: {struct Cyc_Absyn_Vardecl*vd1=_tmpA4;void*t1=_tmpA5;void*a1=_tmpA6;
return({struct _tuple13 _tmp3C9;({void*_tmp451=(void*)({struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=5U,_tmpB1->f1=vd1,_tmpB1->f2=t1,_tmpB1->f3=a1;_tmpB1;});_tmp3C9.f1=_tmp451;}),_tmp3C9.f2=0;_tmp3C9;});}default: _LLD: _tmpA3=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmpA2)->f1;_LLE: {void*a=_tmpA3;
return({struct _tuple13 _tmp3CA;({void*_tmp452=(void*)({struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=6U,_tmpB2->f1=a;_tmpB2;});_tmp3CA.f1=_tmp452;}),_tmp3CA.f2=0;_tmp3CA;});}}_LL0:;}
# 427
static void*Cyc_Vcgen_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_Vcgen_hash_cons_table == 0){
h=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(123,Cyc_Vcgen_assncmp,Cyc_Vcgen_assnhash);
Cyc_Vcgen_hash_cons_table=h;
# 434
({struct Cyc_Hashtable_Table*_tmp453=h;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple13*val))Cyc_Hashtable_insert)(_tmp453,(void*)& Cyc_Vcgen_true_assn,({struct _tuple13*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->f1=(void*)& Cyc_Vcgen_true_assn,_tmpB3->f2=1;_tmpB3;}));});
({struct Cyc_Hashtable_Table*_tmp454=h;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple13*val))Cyc_Hashtable_insert)(_tmp454,(void*)& Cyc_Vcgen_false_assn,({struct _tuple13*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=(void*)& Cyc_Vcgen_false_assn,_tmpB4->f2=1;_tmpB4;}));});}else{
# 437
h=(struct Cyc_Hashtable_Table*)_check_null(Cyc_Vcgen_hash_cons_table);}{
struct _tuple13**_tmpB5=((struct _tuple13**(*)(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_lookup_other_opt)(h,a,Cyc_Vcgen_assncmp,Cyc_Vcgen_assnhash);struct _tuple13**resopt=_tmpB5;
if(resopt == 0){
struct _tuple13 _tmpB6=Cyc_Vcgen_copy_assn(a);struct _tuple13 _stmttmp3=_tmpB6;struct _tuple13 _tmpB7=_stmttmp3;int _tmpB9;void*_tmpB8;_LL1: _tmpB8=_tmpB7.f1;_tmpB9=_tmpB7.f2;_LL2: {void*res=_tmpB8;int reduced=_tmpB9;
({struct Cyc_Hashtable_Table*_tmp456=h;void*_tmp455=res;((void(*)(struct Cyc_Hashtable_Table*t,void*key,struct _tuple13*val))Cyc_Hashtable_insert)(_tmp456,_tmp455,({struct _tuple13*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=res,_tmpBA->f2=reduced;_tmpBA;}));});
return res;}}else{
# 444
struct _tuple13*_tmpBB=*resopt;struct _tuple13*_stmttmp4=_tmpBB;struct _tuple13*_tmpBC=_stmttmp4;void*_tmpBD;_LL4: _tmpBD=_tmpBC->f1;_LL5: {void*res=_tmpBD;
return res;}}}}
# 450
static void*Cyc_Vcgen_kill(void*a){
void*_tmpBE=a;switch(*((int*)_tmpBE)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 6U: _LL5: _LL6:
 return a;default: _LL7: _LL8: {
# 456
struct Cyc_Vcgen_Kill_Vcgen_Assn_struct _tmpBF=({struct Cyc_Vcgen_Kill_Vcgen_Assn_struct _tmp3CB;_tmp3CB.tag=6U,_tmp3CB.f1=a;_tmp3CB;});struct Cyc_Vcgen_Kill_Vcgen_Assn_struct p=_tmpBF;
return Cyc_Vcgen_hash_cons((void*)& p);}}_LL0:;}
# 462
static void*Cyc_Vcgen_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple12 _tmpC0=({struct _tuple12 _tmp3CD;_tmp3CD.f1=a1,_tmp3CD.f2=a2;_tmp3CD;});struct _tuple12 _stmttmp5=_tmpC0;struct _tuple12 _tmpC1=_stmttmp5;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC1.f1)->tag == 0U){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC1.f2)->tag == 0U){_LL3: _LL4:
 return a1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC1.f1)->tag == 1U){_LL5: _LL6:
 return a1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC1.f2)->tag == 1U){_LL7: _LL8:
 return a2;}else{_LL9: _LLA:
# 473
 if((int)a1 > (int)a2)
({void*_tmpC2=a1;void*_tmpC3=a2;a1=_tmpC3;a2=_tmpC2;});{
struct Cyc_Vcgen_And_Vcgen_Assn_struct _tmpC4=({struct Cyc_Vcgen_And_Vcgen_Assn_struct _tmp3CC;_tmp3CC.tag=3U,_tmp3CC.f1=a1,_tmp3CC.f2=a2;_tmp3CC;});struct Cyc_Vcgen_And_Vcgen_Assn_struct a=_tmpC4;
return Cyc_Vcgen_hash_cons((void*)& a);}}}}}_LL0:;}}
# 481
static void*Cyc_Vcgen_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple12 _tmpC5=({struct _tuple12 _tmp3CF;_tmp3CF.f1=a1,_tmp3CF.f2=a2;_tmp3CF;});struct _tuple12 _stmttmp6=_tmpC5;struct _tuple12 _tmpC6=_stmttmp6;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC6.f1)->tag == 0U){_LL1: _LL2:
 return a1;}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmpC6.f2)->tag == 0U){_LL3: _LL4:
 return a2;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC6.f1)->tag == 1U){_LL5: _LL6:
 return a2;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmpC6.f2)->tag == 1U){_LL7: _LL8:
 return a1;}else{_LL9: _LLA:
# 490
 if((int)a1 > (int)a2)
({void*_tmpC7=a1;void*_tmpC8=a2;a1=_tmpC8;a2=_tmpC7;});{
struct Cyc_Vcgen_Or_Vcgen_Assn_struct _tmpC9=({struct Cyc_Vcgen_Or_Vcgen_Assn_struct _tmp3CE;_tmp3CE.tag=4U,_tmp3CE.f1=a1,_tmp3CE.f2=a2;_tmp3CE;});struct Cyc_Vcgen_Or_Vcgen_Assn_struct a=_tmpC9;
return Cyc_Vcgen_hash_cons((void*)& a);}}}}}_LL0:;}}
# 498
static void*Cyc_Vcgen_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_tmpCA=a;switch(*((int*)_tmpCA)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;default: _LL5: _LL6: {
# 503
struct Cyc_Vcgen_Subst_Vcgen_Assn_struct _tmpCB=({struct Cyc_Vcgen_Subst_Vcgen_Assn_struct _tmp3D0;_tmp3D0.tag=5U,_tmp3D0.f1=x,_tmp3D0.f2=newx,_tmp3D0.f3=a;_tmp3D0;});struct Cyc_Vcgen_Subst_Vcgen_Assn_struct p=_tmpCB;
return Cyc_Vcgen_hash_cons((void*)& p);}}_LL0:;}
# 509
static void*Cyc_Vcgen_prim(void*t1,enum Cyc_Vcgen_Primreln p,void*t2){
struct Cyc_Vcgen_Prim_Vcgen_Assn_struct _tmpCC=({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct _tmp3D1;_tmp3D1.tag=2U,_tmp3D1.f1=t1,_tmp3D1.f2=p,_tmp3D1.f3=t2;_tmp3D1;});struct Cyc_Vcgen_Prim_Vcgen_Assn_struct ptr=_tmpCC;
return Cyc_Vcgen_hash_cons((void*)& ptr);}
# 519
static void*Cyc_Vcgen_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Eq,t2);}
static void*Cyc_Vcgen_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Neq,t2);}
static void*Cyc_Vcgen_slt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_SLt,t2);}
static void*Cyc_Vcgen_slte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_SLte,t2);}
static void*Cyc_Vcgen_ult(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_ULt,t2);}
static void*Cyc_Vcgen_ulte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_ULte,t2);}
# 531
static void*Cyc_Vcgen_reduce(void*a);
# 536
static void*Cyc_Vcgen_not(void*a){
void*_tmpCD=a;void*_tmpD0;enum Cyc_Vcgen_Primreln _tmpCF;void*_tmpCE;void*_tmpD2;void*_tmpD1;void*_tmpD4;void*_tmpD3;switch(*((int*)_tmpCD)){case 0U: _LL1: _LL2:
 return(void*)& Cyc_Vcgen_false_assn;case 1U: _LL3: _LL4:
 return(void*)& Cyc_Vcgen_true_assn;case 3U: _LL5: _tmpD3=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpCD)->f1;_tmpD4=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpCD)->f2;_LL6: {void*a1=_tmpD3;void*a2=_tmpD4;
return({void*_tmp457=Cyc_Vcgen_not(a1);Cyc_Vcgen_or(_tmp457,Cyc_Vcgen_not(a2));});}case 4U: _LL7: _tmpD1=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpCD)->f1;_tmpD2=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpCD)->f2;_LL8: {void*a1=_tmpD1;void*a2=_tmpD2;
return({void*_tmp458=Cyc_Vcgen_not(a1);Cyc_Vcgen_and(_tmp458,Cyc_Vcgen_not(a2));});}case 2U: _LL9: _tmpCE=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpCD)->f1;_tmpCF=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpCD)->f2;_tmpD0=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpCD)->f3;_LLA: {void*t1=_tmpCE;enum Cyc_Vcgen_Primreln p=_tmpCF;void*t2=_tmpD0;
# 543
enum Cyc_Vcgen_Primreln _tmpD5=p;switch(_tmpD5){case Cyc_Vcgen_Eq: _LL10: _LL11:
 return Cyc_Vcgen_neq(t1,t2);case Cyc_Vcgen_Neq: _LL12: _LL13:
 return Cyc_Vcgen_eq(t1,t2);case Cyc_Vcgen_SLt: _LL14: _LL15:
 return Cyc_Vcgen_slte(t2,t1);case Cyc_Vcgen_SLte: _LL16: _LL17:
 return Cyc_Vcgen_slt(t2,t1);case Cyc_Vcgen_ULt: _LL18: _LL19:
 return Cyc_Vcgen_ulte(t2,t1);case Cyc_Vcgen_ULte: _LL1A: _LL1B:
 return Cyc_Vcgen_ult(t2,t1);default: _LL1C: _LL1D:
({void*_tmpD6=0U;({struct _fat_ptr _tmp459=({const char*_tmpD7="Vcgen::invert: non-primop";_tag_fat(_tmpD7,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp459,_tag_fat(_tmpD6,sizeof(void*),0U));});});}_LLF:;}case 5U: _LLB: _LLC:
# 552
 return Cyc_Vcgen_not(Cyc_Vcgen_reduce(a));default: _LLD: _LLE:
 return Cyc_Vcgen_not(Cyc_Vcgen_reduce(a));}_LL0:;}struct _tuple14{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 575 "/tmp/cyclone/src/vcgen.cyc"
static void*Cyc_Vcgen_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 578
void*_tmpD8=t;struct Cyc_Absyn_Vardecl*_tmpD9;struct Cyc_List_List*_tmpDB;enum Cyc_Absyn_Primop _tmpDA;void*_tmpDD;void*_tmpDC;switch(*((int*)_tmpD8)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return t;case 4U: _LL5: _tmpDC=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmpD8)->f1;_tmpDD=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmpD8)->f2;_LL6: {void*tp=_tmpDC;void*tm=_tmpDD;
# 582
void*_tmpDE=Cyc_Vcgen_reduce_term(tm,non_esc_subst,esc_subst);void*tm2=_tmpDE;
if(tm != tm2)return Cyc_Vcgen_cast(tp,tm2);else{
return t;}}case 3U: _LL7: _tmpDA=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpD8)->f1;_tmpDB=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpD8)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmpDA;struct Cyc_List_List*ts=_tmpDB;
# 586
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpDF=ts;struct Cyc_List_List*ptr=_tmpDF;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpE0=(void*)ptr->hd;void*old_term=_tmpE0;
void*_tmpE1=Cyc_Vcgen_reduce_term(old_term,non_esc_subst,esc_subst);void*new_term=_tmpE1;
res=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=new_term,_tmpE2->tl=res;_tmpE2;});
if(new_term != old_term)change=1;}}
# 594
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp45A=p;Cyc_Vcgen_primop(_tmp45A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res));});}default: _LL9: _tmpD9=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmpD8)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpD9;
# 597
if(vd->escapes && esc_subst != 0){
# 600
struct Cyc_List_List*_tmpE3=*esc_subst;struct Cyc_List_List*s=_tmpE3;
for(0;s != 0;s=s->tl){
struct _tuple14 _tmpE4=*((struct _tuple14*)s->hd);struct _tuple14 _stmttmp7=_tmpE4;struct _tuple14 _tmpE5=_stmttmp7;void*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpE6;_LLC: _tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;_LLD: {struct Cyc_Absyn_Vardecl*vd2=_tmpE6;void*t2=_tmpE7;
if(vd2 == vd)return t;}}{
# 607
void*_tmpE8=Cyc_Vcgen_fresh_var();void*t=_tmpE8;
({struct Cyc_List_List*_tmp45C=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct _tuple14*_tmp45B=({struct _tuple14*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->f1=vd,_tmpE9->f2=t;_tmpE9;});_tmpEA->hd=_tmp45B;}),_tmpEA->tl=*esc_subst;_tmpEA;});*esc_subst=_tmp45C;});
return Cyc_Vcgen_fresh_var();}}else{
if(!vd->escapes)
# 612
for(0;non_esc_subst != 0;non_esc_subst=non_esc_subst->tl){
struct _tuple14 _tmpEB=*((struct _tuple14*)non_esc_subst->hd);struct _tuple14 _stmttmp8=_tmpEB;struct _tuple14 _tmpEC=_stmttmp8;void*_tmpEE;struct Cyc_Absyn_Vardecl*_tmpED;_LLF: _tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_LL10: {struct Cyc_Absyn_Vardecl*vd2=_tmpED;void*t2=_tmpEE;
if(vd == vd2)return t2;}}}
# 617
return t;}}_LL0:;}
# 621
static void*Cyc_Vcgen_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 624
LOOP: {
void*_tmpEF=a;void*_tmpF2;void*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpF0;void*_tmpF3;void*_tmpF5;void*_tmpF4;void*_tmpF7;void*_tmpF6;void*_tmpFA;enum Cyc_Vcgen_Primreln _tmpF9;void*_tmpF8;switch(*((int*)_tmpEF)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;case 2U: _LL5: _tmpF8=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpEF)->f1;_tmpF9=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpEF)->f2;_tmpFA=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpEF)->f3;_LL6: {void*t1=_tmpF8;enum Cyc_Vcgen_Primreln p=_tmpF9;void*t2=_tmpFA;
# 629
void*_tmpFB=Cyc_Vcgen_reduce_term(t1,non_esc_subst,esc_subst);void*newt1=_tmpFB;
void*_tmpFC=Cyc_Vcgen_reduce_term(t2,non_esc_subst,esc_subst);void*newt2=_tmpFC;
if(t1 == newt1 && t2 == newt2)return a;
return Cyc_Vcgen_prim(newt1,p,newt2);}case 3U: _LL7: _tmpF6=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpEF)->f1;_tmpF7=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpEF)->f2;_LL8: {void*a1=_tmpF6;void*a2=_tmpF7;
# 634
void*_tmpFD=Cyc_Vcgen_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpFD;
void*_tmpFE=Cyc_Vcgen_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpFE;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_Vcgen_and(newa1,newa2);}case 4U: _LL9: _tmpF4=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpEF)->f1;_tmpF5=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpEF)->f2;_LLA: {void*a1=_tmpF4;void*a2=_tmpF5;
# 639
void*_tmpFF=Cyc_Vcgen_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpFF;
void*_tmp100=Cyc_Vcgen_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmp100;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_Vcgen_or(newa1,newa2);}case 6U: _LLB: _tmpF3=(void*)((struct Cyc_Vcgen_Kill_Vcgen_Assn_struct*)_tmpEF)->f1;_LLC: {void*a=_tmpF3;
# 646
struct Cyc_List_List*new_esc_subst=0;
return Cyc_Vcgen_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _LLD: _tmpF0=((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpEF)->f1;_tmpF1=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpEF)->f2;_tmpF2=(void*)((struct Cyc_Vcgen_Subst_Vcgen_Assn_struct*)_tmpEF)->f3;_LLE: {struct Cyc_Absyn_Vardecl*vd=_tmpF0;void*t=_tmpF1;void*a1=_tmpF2;
# 650
t=Cyc_Vcgen_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_tmp102=_cycalloc(sizeof(*_tmp102));({struct _tuple14*_tmp45D=({struct _tuple14*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->f1=vd,_tmp101->f2=t;_tmp101;});_tmp102->hd=_tmp45D;}),_tmp102->tl=non_esc_subst;_tmp102;});
a=a1;
goto LOOP;}}_LL0:;}}
# 658
static void*Cyc_Vcgen_reduce(void*a){
# 660
if(Cyc_Vcgen_is_reduced(a))return a;
return Cyc_Vcgen_reduce_assn(a,0,0);}
# 665
static void*Cyc_Vcgen_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmp103=t;void*_tmp105;void*_tmp104;struct Cyc_List_List*_tmp107;enum Cyc_Absyn_Primop _tmp106;struct Cyc_Absyn_Vardecl*_tmp108;switch(*((int*)_tmp103)){case 1U: _LL1: _tmp108=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp103)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp108;
# 668
if(vd == x)return newx;
return t;}case 3U: _LL3: _tmp106=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp103)->f1;_tmp107=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp103)->f2;_LL4: {enum Cyc_Absyn_Primop p=_tmp106;struct Cyc_List_List*ts=_tmp107;
# 671
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmp109=ts;struct Cyc_List_List*ptr=_tmp109;for(0;ptr != 0;ptr=ptr->tl){
void*_tmp10A=(void*)ptr->hd;void*old_term=_tmp10A;
void*_tmp10B=Cyc_Vcgen_subst_term(old_term,x,newx);void*new_term=_tmp10B;
res=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=new_term,_tmp10C->tl=res;_tmp10C;});
if(new_term != old_term)change=1;}}
# 679
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp45E=p;Cyc_Vcgen_primop(_tmp45E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res));});}case 4U: _LL5: _tmp104=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp103)->f1;_tmp105=(void*)((struct Cyc_Vcgen_Cast_Vcgen_Term_struct*)_tmp103)->f2;_LL6: {void*tp=_tmp104;void*tm=_tmp105;
# 682
void*_tmp10D=Cyc_Vcgen_subst_term(tm,x,newx);void*tm2=_tmp10D;
if(tm != tm2)return Cyc_Vcgen_cast(tp,tm2);else{
return t;}}case 0U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 return t;}_LL0:;}
# 691
static void*Cyc_Vcgen_kill_mem_term(void*t){
struct Cyc_List_List*_tmp10E=0;struct Cyc_List_List*esc_subst=_tmp10E;
return Cyc_Vcgen_reduce_term(t,0,& esc_subst);}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 706 "/tmp/cyclone/src/vcgen.cyc"
static void Cyc_Vcgen_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 710
while(1){
struct _tuple15 _tmp10F=({struct _tuple15 _tmp3D2;_tmp3D2.f1=a1s,_tmp3D2.f2=a2s;_tmp3D2;});struct _tuple15 _stmttmp9=_tmp10F;struct _tuple15 _tmp110=_stmttmp9;if(_tmp110.f1 == 0){if(_tmp110.f2 == 0){_LL1: _LL2:
 return;}else{_LL5: _LL6:
# 714
({struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(a2s,*res2);*res2=_tmp45F;});return;}}else{if(_tmp110.f2 == 0){_LL3: _LL4:
# 713
({struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(a1s,*res1);*res1=_tmp460;});return;}else{_LL7: _LL8: {
# 716
int c=({void*_tmp461=(void*)((struct Cyc_List_List*)_check_null(a1s))->hd;Cyc_Vcgen_assncmp(_tmp461,(void*)((struct Cyc_List_List*)_check_null(a2s))->hd);});
if(c == 0){
({struct Cyc_List_List*_tmp462=({struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->hd=(void*)a1s->hd,_tmp111->tl=*common;_tmp111;});*common=_tmp462;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_tmp463=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->hd=(void*)a1s->hd,_tmp112->tl=*res1;_tmp112;});*res1=_tmp463;});
a1s=a1s->tl;}else{
# 725
({struct Cyc_List_List*_tmp464=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=(void*)a2s->hd,_tmp113->tl=*res2;_tmp113;});*res2=_tmp464;});
a2s=a2s->tl;}}}}}_LL0:;}}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 732
static struct _tuple16 Cyc_Vcgen_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 734
struct Cyc_List_List*_tmp114=0;struct Cyc_List_List*common=_tmp114;
struct Cyc_List_List*_tmp115=0;struct Cyc_List_List*res1=_tmp115;
struct Cyc_List_List*_tmp116=0;struct Cyc_List_List*res2=_tmp116;
Cyc_Vcgen_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple16 _tmp3D3;({struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(common);_tmp3D3.f1=_tmp467;}),({struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res1);_tmp3D3.f2=_tmp466;}),({struct Cyc_List_List*_tmp465=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res2);_tmp3D3.f3=_tmp465;});_tmp3D3;});}struct _tuple17{struct Cyc_List_List*f1;void*f2;};
# 743
static struct _tuple17 Cyc_Vcgen_factor(void*a){
void*_tmp117=a;void*_tmp119;void*_tmp118;void*_tmp11B;void*_tmp11A;switch(*((int*)_tmp117)){case 0U: _LL1: _LL2:
 return({struct _tuple17 _tmp3D4;_tmp3D4.f1=0,_tmp3D4.f2=(void*)& Cyc_Vcgen_true_assn;_tmp3D4;});case 1U: _LL3: _LL4:
 return({struct _tuple17 _tmp3D5;_tmp3D5.f1=0,_tmp3D5.f2=(void*)& Cyc_Vcgen_false_assn;_tmp3D5;});case 2U: _LL5: _LL6:
 return({struct _tuple17 _tmp3D6;({struct Cyc_List_List*_tmp468=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->hd=a,_tmp11C->tl=0;_tmp11C;});_tmp3D6.f1=_tmp468;}),_tmp3D6.f2=(void*)& Cyc_Vcgen_true_assn;_tmp3D6;});case 3U: _LL7: _tmp11A=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp117)->f1;_tmp11B=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp117)->f2;_LL8: {void*a1=_tmp11A;void*a2=_tmp11B;
# 749
struct _tuple17 _tmp11D=Cyc_Vcgen_factor(a1);struct _tuple17 _stmttmpA=_tmp11D;struct _tuple17 _tmp11E=_stmttmpA;void*_tmp120;struct Cyc_List_List*_tmp11F;_LLE: _tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_LLF: {struct Cyc_List_List*c1s=_tmp11F;void*a1=_tmp120;
struct _tuple17 _tmp121=Cyc_Vcgen_factor(a2);struct _tuple17 _stmttmpB=_tmp121;struct _tuple17 _tmp122=_stmttmpB;void*_tmp124;struct Cyc_List_List*_tmp123;_LL11: _tmp123=_tmp122.f1;_tmp124=_tmp122.f2;_LL12: {struct Cyc_List_List*c2s=_tmp123;void*a2=_tmp124;
return({struct _tuple17 _tmp3D7;({struct Cyc_List_List*_tmp46A=((struct Cyc_List_List*(*)(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_merge)(Cyc_Vcgen_assncmp,c1s,c2s);_tmp3D7.f1=_tmp46A;}),({void*_tmp469=Cyc_Vcgen_and(a1,a2);_tmp3D7.f2=_tmp469;});_tmp3D7;});}}}case 4U: _LL9: _tmp118=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp117)->f1;_tmp119=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp117)->f2;_LLA: {void*a1=_tmp118;void*a2=_tmp119;
# 753
struct _tuple17 _tmp125=Cyc_Vcgen_factor(a1);struct _tuple17 _stmttmpC=_tmp125;struct _tuple17 _tmp126=_stmttmpC;void*_tmp128;struct Cyc_List_List*_tmp127;_LL14: _tmp127=_tmp126.f1;_tmp128=_tmp126.f2;_LL15: {struct Cyc_List_List*c1s=_tmp127;void*a1=_tmp128;
struct _tuple17 _tmp129=Cyc_Vcgen_factor(a2);struct _tuple17 _stmttmpD=_tmp129;struct _tuple17 _tmp12A=_stmttmpD;void*_tmp12C;struct Cyc_List_List*_tmp12B;_LL17: _tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;_LL18: {struct Cyc_List_List*c2s=_tmp12B;void*a2=_tmp12C;
struct _tuple16 _tmp12D=Cyc_Vcgen_intersect_assns(c1s,c2s);struct _tuple16 _stmttmpE=_tmp12D;struct _tuple16 _tmp12E=_stmttmpE;struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp12F;_LL1A: _tmp12F=_tmp12E.f1;_tmp130=_tmp12E.f2;_tmp131=_tmp12E.f3;_LL1B: {struct Cyc_List_List*cs=_tmp12F;struct Cyc_List_List*c1s=_tmp130;struct Cyc_List_List*c2s=_tmp131;
return({struct _tuple17 _tmp3D8;_tmp3D8.f1=cs,({void*_tmp46C=({void*_tmp46B=((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Vcgen_and,a1,c1s);Cyc_Vcgen_or(_tmp46B,((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Vcgen_and,a2,c2s));});_tmp3D8.f2=_tmp46C;});_tmp3D8;});}}}}default: _LLB: _LLC:
 return({struct _tuple17 _tmp3D9;_tmp3D9.f1=0,_tmp3D9.f2=a;_tmp3D9;});}_LL0:;}
# 761
static void*Cyc_Vcgen_factor_assn(void*a){
# 763
struct _tuple17 _tmp132=Cyc_Vcgen_factor(a);struct _tuple17 _stmttmpF=_tmp132;struct _tuple17 _tmp133=_stmttmpF;void*_tmp135;struct Cyc_List_List*_tmp134;_LL1: _tmp134=_tmp133.f1;_tmp135=_tmp133.f2;_LL2: {struct Cyc_List_List*cs=_tmp134;void*a=_tmp135;
a=((void*(*)(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Vcgen_and,a,cs);
# 768
return a;}}struct _tuple18{enum Cyc_Vcgen_Primreln f1;enum Cyc_Vcgen_Primreln f2;};
# 775
static int Cyc_Vcgen_find_ctxt(void*ctxt,void*t1,enum Cyc_Vcgen_Primreln p1,void*u1){
LOOP: {
void*_tmp136=ctxt;void*_tmp139;enum Cyc_Vcgen_Primreln _tmp138;void*_tmp137;void*_tmp13B;void*_tmp13A;void*_tmp13D;void*_tmp13C;switch(*((int*)_tmp136)){case 1U: _LL1: _LL2:
 return 1;case 0U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp13C=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp136)->f1;_tmp13D=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp136)->f2;_LL6: {void*a1=_tmp13C;void*a2=_tmp13D;
# 781
if(Cyc_Vcgen_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4U: _LL7: _tmp13A=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp136)->f1;_tmp13B=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp136)->f2;_LL8: {void*a1=_tmp13A;void*a2=_tmp13B;
# 785
if(!Cyc_Vcgen_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2U: _LL9: _tmp137=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp136)->f1;_tmp138=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp136)->f2;_tmp139=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp136)->f3;_LLA: {void*t2=_tmp137;enum Cyc_Vcgen_Primreln p2=_tmp138;void*u2=_tmp139;
# 789
struct _tuple18 _tmp13E=({struct _tuple18 _tmp3DA;_tmp3DA.f1=p2,_tmp3DA.f2=p1;_tmp3DA;});struct _tuple18 _stmttmp10=_tmp13E;struct _tuple18 _tmp13F=_stmttmp10;switch(_tmp13F.f1){case Cyc_Vcgen_Neq: if(_tmp13F.f2 == Cyc_Vcgen_Neq){_LLE: _LLF:
 goto _LL11;}else{goto _LL26;}case Cyc_Vcgen_Eq: switch(_tmp13F.f2){case Cyc_Vcgen_Eq: _LL10: _LL11:
 goto _LL13;case Cyc_Vcgen_SLte: _LL12: _LL13:
 goto _LL15;case Cyc_Vcgen_ULte: _LL14: _LL15:
# 794
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2)||
 Cyc_Vcgen_same_term(t1,u2)&& Cyc_Vcgen_same_term(u1,t2);default: goto _LL26;}case Cyc_Vcgen_SLt: switch(_tmp13F.f2){case Cyc_Vcgen_Neq: _LL16: _LL17:
 goto _LL19;case Cyc_Vcgen_SLt: _LL1A: _LL1B:
# 800
 goto _LL1D;case Cyc_Vcgen_SLte: _LL1E: _LL1F:
# 802
 goto _LL21;default: goto _LL26;}case Cyc_Vcgen_ULt: switch(_tmp13F.f2){case Cyc_Vcgen_Neq: _LL18: _LL19:
# 798
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2)||
 Cyc_Vcgen_same_term(t1,u2)&& Cyc_Vcgen_same_term(u1,t2);case Cyc_Vcgen_ULt: _LL1C: _LL1D:
# 801
 goto _LL1F;case Cyc_Vcgen_ULte: _LL20: _LL21:
# 803
 goto _LL23;default: goto _LL26;}case Cyc_Vcgen_SLte: if(_tmp13F.f2 == Cyc_Vcgen_SLte){_LL22: _LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_Vcgen_ULte: if(_tmp13F.f2 == Cyc_Vcgen_ULte){_LL24: _LL25:
# 806
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2);}else{goto _LL26;}default: _LL26: _LL27:
 return 0;}_LLD:;}default: _LLB: _LLC:
# 810
 ctxt=Cyc_Vcgen_reduce(ctxt);
goto LOOP;}_LL0:;}}
# 817
static int Cyc_Vcgen_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt == a)return 1;{
void*_tmp140=a;void*_tmp143;enum Cyc_Vcgen_Primreln _tmp142;void*_tmp141;void*_tmp145;void*_tmp144;void*_tmp147;void*_tmp146;switch(*((int*)_tmp140)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp146=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp140)->f1;_tmp147=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp140)->f2;_LL6: {void*a1=_tmp146;void*a2=_tmp147;
# 824
if(!Cyc_Vcgen_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4U: _LL7: _tmp144=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp140)->f1;_tmp145=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp140)->f2;_LL8: {void*a1=_tmp144;void*a2=_tmp145;
# 828
if(Cyc_Vcgen_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2U: _LL9: _tmp141=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp140)->f1;_tmp142=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp140)->f2;_tmp143=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp140)->f3;_LLA: {void*t1=_tmp141;enum Cyc_Vcgen_Primreln p=_tmp142;void*t2=_tmp143;
return Cyc_Vcgen_find_ctxt(ctxt,t1,p,t2);}default: _LLB: _LLC:
# 833
 a=Cyc_Vcgen_reduce(a);
goto LOOP;}_LL0:;}}
# 859 "/tmp/cyclone/src/vcgen.cyc"
enum Cyc_Vcgen_DistanceInfo{Cyc_Vcgen_Unsigned_valid =1,Cyc_Vcgen_Signed_valid =2,Cyc_Vcgen_Both_valid =3};struct Cyc_Vcgen_Distance{struct Cyc_Vcgen_Distance*next;void*t1;void*t2;enum Cyc_Vcgen_DistanceInfo dinfo;int unsigned_dist;int signed_dist;};struct Cyc_Vcgen_Graph{struct Cyc_Vcgen_Graph*next;struct Cyc_List_List*nodes;struct Cyc_Vcgen_Distance*distance;};
# 881
static void Cyc_Vcgen_print_graphs(struct Cyc_Vcgen_Graph*gs){
({void*_tmp148=0U;({struct Cyc___cycFILE*_tmp46E=Cyc_stderr;struct _fat_ptr _tmp46D=({const char*_tmp149="Graphs:\n";_tag_fat(_tmp149,sizeof(char),9U);});Cyc_fprintf(_tmp46E,_tmp46D,_tag_fat(_tmp148,sizeof(void*),0U));});});
for(0;gs != 0;gs=gs->next){
({void*_tmp14A=0U;({struct Cyc___cycFILE*_tmp470=Cyc_stderr;struct _fat_ptr _tmp46F=({const char*_tmp14B="\t{";_tag_fat(_tmp14B,sizeof(char),3U);});Cyc_fprintf(_tmp470,_tmp46F,_tag_fat(_tmp14A,sizeof(void*),0U));});});
{struct Cyc_Vcgen_Distance*_tmp14C=gs->distance;struct Cyc_Vcgen_Distance*ds=_tmp14C;for(0;ds != 0;ds=ds->next){
if((int)ds->dinfo == (int)Cyc_Vcgen_Both_valid)
({struct Cyc_String_pa_PrintArg_struct _tmp14F=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0U,({struct _fat_ptr _tmp471=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t1));_tmp3E0.f1=_tmp471;});_tmp3E0;});struct Cyc_String_pa_PrintArg_struct _tmp150=({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0U,({
struct _fat_ptr _tmp472=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t2));_tmp3DF.f1=_tmp472;});_tmp3DF;});struct Cyc_Int_pa_PrintArg_struct _tmp151=({struct Cyc_Int_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=1U,_tmp3DE.f1=(unsigned long)ds->unsigned_dist;_tmp3DE;});struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=0U,({struct _fat_ptr _tmp473=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t1));_tmp3DD.f1=_tmp473;});_tmp3DD;});struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0U,({
struct _fat_ptr _tmp474=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t2));_tmp3DC.f1=_tmp474;});_tmp3DC;});struct Cyc_Int_pa_PrintArg_struct _tmp154=({struct Cyc_Int_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=1U,_tmp3DB.f1=(unsigned long)ds->signed_dist;_tmp3DB;});void*_tmp14D[6U];_tmp14D[0]=& _tmp14F,_tmp14D[1]=& _tmp150,_tmp14D[2]=& _tmp151,_tmp14D[3]=& _tmp152,_tmp14D[4]=& _tmp153,_tmp14D[5]=& _tmp154;({struct Cyc___cycFILE*_tmp476=Cyc_stderr;struct _fat_ptr _tmp475=({const char*_tmp14E="%s - %s U<= %d, %s - %s S<= %d";_tag_fat(_tmp14E,sizeof(char),31U);});Cyc_fprintf(_tmp476,_tmp475,_tag_fat(_tmp14D,sizeof(void*),6U));});});else{
if((int)ds->dinfo == (int)Cyc_Vcgen_Signed_valid)
({struct Cyc_String_pa_PrintArg_struct _tmp157=({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0U,({struct _fat_ptr _tmp477=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t1));_tmp3E3.f1=_tmp477;});_tmp3E3;});struct Cyc_String_pa_PrintArg_struct _tmp158=({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0U,({
struct _fat_ptr _tmp478=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t2));_tmp3E2.f1=_tmp478;});_tmp3E2;});struct Cyc_Int_pa_PrintArg_struct _tmp159=({struct Cyc_Int_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=1U,_tmp3E1.f1=(unsigned long)ds->signed_dist;_tmp3E1;});void*_tmp155[3U];_tmp155[0]=& _tmp157,_tmp155[1]=& _tmp158,_tmp155[2]=& _tmp159;({struct Cyc___cycFILE*_tmp47A=Cyc_stderr;struct _fat_ptr _tmp479=({const char*_tmp156="%s - %s S<= %d";_tag_fat(_tmp156,sizeof(char),15U);});Cyc_fprintf(_tmp47A,_tmp479,_tag_fat(_tmp155,sizeof(void*),3U));});});else{
# 894
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0U,({struct _fat_ptr _tmp47B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t1));_tmp3E6.f1=_tmp47B;});_tmp3E6;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0U,({
struct _fat_ptr _tmp47C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(ds->t2));_tmp3E5.f1=_tmp47C;});_tmp3E5;});struct Cyc_Int_pa_PrintArg_struct _tmp15E=({struct Cyc_Int_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=1U,_tmp3E4.f1=(unsigned long)ds->unsigned_dist;_tmp3E4;});void*_tmp15A[3U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D,_tmp15A[2]=& _tmp15E;({struct Cyc___cycFILE*_tmp47E=Cyc_stderr;struct _fat_ptr _tmp47D=({const char*_tmp15B="%s - %s U<= %d";_tag_fat(_tmp15B,sizeof(char),15U);});Cyc_fprintf(_tmp47E,_tmp47D,_tag_fat(_tmp15A,sizeof(void*),3U));});});}}
# 897
if(ds->next != 0)({void*_tmp15F=0U;({struct Cyc___cycFILE*_tmp480=Cyc_stderr;struct _fat_ptr _tmp47F=({const char*_tmp160=",";_tag_fat(_tmp160,sizeof(char),2U);});Cyc_fprintf(_tmp480,_tmp47F,_tag_fat(_tmp15F,sizeof(void*),0U));});});}}
# 899
({void*_tmp161=0U;({struct Cyc___cycFILE*_tmp482=Cyc_stderr;struct _fat_ptr _tmp481=({const char*_tmp162="}\n";_tag_fat(_tmp162,sizeof(char),3U);});Cyc_fprintf(_tmp482,_tmp481,_tag_fat(_tmp161,sizeof(void*),0U));});});}}
# 904
static unsigned Cyc_Vcgen_num_graphs(struct Cyc_Vcgen_Graph*gs){
unsigned n=0U;
for(0;gs != 0;gs=gs->next){
++ n;}
return n;}
# 913
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_true_graph (void){return({struct Cyc_Vcgen_Graph*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->next=0,_tmp163->nodes=0,_tmp163->distance=0;_tmp163;});}
# 916
static struct Cyc_Vcgen_Distance*Cyc_Vcgen_copy_distance(struct Cyc_Vcgen_Distance*ds){
struct Cyc_Vcgen_Distance*res=0;
for(0;ds != 0;ds=ds->next){
struct Cyc_Vcgen_Distance*_tmp164=({struct Cyc_Vcgen_Distance*_tmp165=_cycalloc(sizeof(*_tmp165));*_tmp165=*ds;_tmp165;});struct Cyc_Vcgen_Distance*newds=_tmp164;
newds->next=res;
res=newds;}
# 923
return res;}
# 927
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_copy_graph(struct Cyc_Vcgen_Graph*g){
struct Cyc_Vcgen_Graph*res=0;
for(0;g != 0;g=g->next){
res=({struct Cyc_Vcgen_Graph*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->next=res,_tmp166->nodes=g->nodes,({struct Cyc_Vcgen_Distance*_tmp483=Cyc_Vcgen_copy_distance(g->distance);_tmp166->distance=_tmp483;});_tmp166;});}
# 932
return res;}
# 937
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_graph_append(struct Cyc_Vcgen_Graph*g1,struct Cyc_Vcgen_Graph*g2){
if(g1 == 0)return g2;
if(g2 == 0)return g1;{
struct Cyc_Vcgen_Graph*p=g1;
{struct Cyc_Vcgen_Graph*_tmp167=p->next;struct Cyc_Vcgen_Graph*x=_tmp167;for(0;x != 0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}
# 947
static void Cyc_Vcgen_add_node(struct Cyc_Vcgen_Graph*g,void*n){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Vcgen_cmp_term,g->nodes,n))
({struct Cyc_List_List*_tmp484=({struct Cyc_List_List*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->hd=n,_tmp168->tl=g->nodes;_tmp168;});g->nodes=_tmp484;});}
# 954
static int*Cyc_Vcgen_lookup_distance(struct Cyc_Vcgen_Graph*g,int is_signed,void*i,void*j){
static int zero=0;
if(Cyc_Vcgen_cmp_term(i,j)== 0)return& zero;
{struct Cyc_Vcgen_Distance*_tmp169=g->distance;struct Cyc_Vcgen_Distance*ds=_tmp169;for(0;ds != 0;ds=ds->next){
if(Cyc_Vcgen_cmp_term(i,ds->t1)== 0 && Cyc_Vcgen_cmp_term(j,ds->t2)== 0){
if(is_signed &&(int)ds->dinfo & (int)Cyc_Vcgen_Signed_valid)
return& ds->signed_dist;else{
if(!is_signed &&(int)ds->dinfo & (int)Cyc_Vcgen_Unsigned_valid)
return& ds->unsigned_dist;else{
# 964
return 0;}}}}}
# 967
return 0;}
# 974
static void Cyc_Vcgen_insert_distance(struct Cyc_Vcgen_Graph*g,int is_signed,void*i,void*j,int dist){
if(Cyc_Vcgen_cmp_term(i,j)== 0)return;
{struct Cyc_Vcgen_Distance*_tmp16A=g->distance;struct Cyc_Vcgen_Distance*ds=_tmp16A;for(0;ds != 0;ds=ds->next){
if(Cyc_Vcgen_cmp_term(i,ds->t1)== 0 && Cyc_Vcgen_cmp_term(j,ds->t2)== 0){
if(is_signed){
ds->dinfo |=(int)Cyc_Vcgen_Signed_valid;
ds->signed_dist=dist;}else{
# 982
ds->dinfo |=(int)Cyc_Vcgen_Unsigned_valid;
ds->unsigned_dist=dist;}
# 985
return;}}}
# 988
({struct Cyc_Vcgen_Distance*_tmp485=({struct Cyc_Vcgen_Distance*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->next=g->distance,_tmp16B->t1=i,_tmp16B->t2=j,is_signed?_tmp16B->dinfo=Cyc_Vcgen_Signed_valid:(_tmp16B->dinfo=Cyc_Vcgen_Unsigned_valid),_tmp16B->unsigned_dist=dist,_tmp16B->signed_dist=dist;_tmp16B;});g->distance=_tmp485;});}
# 995
static int Cyc_Vcgen_add_edge(struct Cyc_Vcgen_Graph*g,int is_signed,void*i,void*j,int dist){
Cyc_Vcgen_add_node(g,i);
Cyc_Vcgen_add_node(g,j);{
int*ij_dist=Cyc_Vcgen_lookup_distance(g,is_signed,i,j);
# 1000
if(ij_dist != 0 &&*ij_dist < dist)
return 1;{
int*ji_dist=Cyc_Vcgen_lookup_distance(g,is_signed,j,i);
# 1005
if(ji_dist != 0 &&*ji_dist + dist < 0)
return 0;
# 1008
{struct Cyc_List_List*_tmp16C=g->nodes;struct Cyc_List_List*ks=_tmp16C;for(0;ks != 0;ks=ks->tl){
void*_tmp16D=(void*)ks->hd;void*k=_tmp16D;
int*_tmp16E=Cyc_Vcgen_lookup_distance(g,is_signed,k,i);int*ki_dist=_tmp16E;
if(ki_dist == 0)continue;{
struct Cyc_List_List*_tmp16F=g->nodes;struct Cyc_List_List*ls=_tmp16F;for(0;ls != 0;ls=ls->tl){
void*_tmp170=(void*)ls->hd;void*l=_tmp170;
int*_tmp171=Cyc_Vcgen_lookup_distance(g,is_signed,j,l);int*jl_dist=_tmp171;
if(jl_dist != 0){
int*_tmp172=Cyc_Vcgen_lookup_distance(g,is_signed,k,l);int*kl_dist=_tmp172;
# 1018
int _tmp173=(*ki_dist + dist)+ *jl_dist;int new_dist=_tmp173;
if(kl_dist == 0)
Cyc_Vcgen_insert_distance(g,is_signed,k,l,new_dist);else{
if(*kl_dist > new_dist)
*kl_dist=new_dist;}}}}}}
# 1027
return 1;}}}
# 1030
static struct _tuple11 Cyc_Vcgen_eval_term(void*t){
void*_tmp174=t;struct Cyc_Absyn_Exp*_tmp175;if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp174)->tag == 0U){_LL1: _tmp175=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp174)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp175;
return Cyc_Evexp_eval_const_uint_exp(e);}}else{_LL3: _LL4:
 return({struct _tuple11 _tmp3E7;_tmp3E7.f1=0U,_tmp3E7.f2=0;_tmp3E7;});}_LL0:;}
# 1042
static int Cyc_Vcgen_add_constraint(struct Cyc_Vcgen_Graph*g,void*t1,enum Cyc_Vcgen_Primreln p,void*t2){
# 1045
struct _tuple11 _tmp176=Cyc_Vcgen_eval_term(t1);struct _tuple11 _stmttmp11=_tmp176;struct _tuple11 _tmp177=_stmttmp11;int _tmp179;unsigned _tmp178;_LL1: _tmp178=_tmp177.f1;_tmp179=_tmp177.f2;_LL2: {unsigned c1=_tmp178;int okay1=_tmp179;
struct _tuple11 _tmp17A=Cyc_Vcgen_eval_term(t2);struct _tuple11 _stmttmp12=_tmp17A;struct _tuple11 _tmp17B=_stmttmp12;int _tmp17D;unsigned _tmp17C;_LL4: _tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;_LL5: {unsigned c2=_tmp17C;int okay2=_tmp17D;
# 1050
if(okay1 && okay2){
# 1052
enum Cyc_Vcgen_Primreln _tmp17E=p;switch(_tmp17E){case Cyc_Vcgen_ULt: _LL7: _LL8:
 return c1 < c2;case Cyc_Vcgen_SLt: _LL9: _LLA:
 return(int)c1 < (int)c2;case Cyc_Vcgen_ULte: _LLB: _LLC:
 return c1 <= c2;case Cyc_Vcgen_SLte: _LLD: _LLE:
 return(int)c1 <= (int)c2;default: _LLF: _LL10:
({void*_tmp17F=0U;({struct _fat_ptr _tmp486=({const char*_tmp180="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp180,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp486,_tag_fat(_tmp17F,sizeof(void*),0U));});});}_LL6:;}else{
# 1059
if(okay2){
# 1061
enum Cyc_Vcgen_Primreln _tmp181=p;switch(_tmp181){case Cyc_Vcgen_ULt: _LL12: _LL13:
# 1063
 c2=c2 - (unsigned)1;
goto _LL15;case Cyc_Vcgen_ULte: _LL14: _LL15:
# 1067
 if(c2 <= 2147483647U){
if(!({struct Cyc_Vcgen_Graph*_tmp489=g;void*_tmp488=t1;void*_tmp487=Cyc_Vcgen_zero();Cyc_Vcgen_add_edge(_tmp489,0,_tmp488,_tmp487,(int)c2);}))return 0;
# 1070
return({struct Cyc_Vcgen_Graph*_tmp48C=g;void*_tmp48B=t1;void*_tmp48A=Cyc_Vcgen_zero();Cyc_Vcgen_add_edge(_tmp48C,1,_tmp48B,_tmp48A,(int)c2);});}
# 1072
goto _LL11;case Cyc_Vcgen_SLt: _LL16: _LL17:
# 1074
 c2=c2 - (unsigned)1;
goto _LL19;case Cyc_Vcgen_SLte: _LL18: _LL19:
# 1077
 if(!({struct Cyc_Vcgen_Graph*_tmp48F=g;void*_tmp48E=t1;void*_tmp48D=Cyc_Vcgen_zero();Cyc_Vcgen_add_edge(_tmp48F,1,_tmp48E,_tmp48D,(int)c2);}))return 0;
# 1079
if(c2 <= 2147483647U)
return({struct Cyc_Vcgen_Graph*_tmp492=g;void*_tmp491=t1;void*_tmp490=Cyc_Vcgen_zero();Cyc_Vcgen_add_edge(_tmp492,0,_tmp491,_tmp490,(int)c2);});else{
return 1;}default: _LL1A: _LL1B:
({void*_tmp182=0U;({struct _fat_ptr _tmp493=({const char*_tmp183="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp183,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp493,_tag_fat(_tmp182,sizeof(void*),0U));});});}_LL11:;}else{
# 1084
if(okay1){
# 1087
enum Cyc_Vcgen_Primreln _tmp184=p;switch(_tmp184){case Cyc_Vcgen_ULt: _LL1D: _LL1E:
# 1089
 c1=c1 + (unsigned)1;
goto _LL20;case Cyc_Vcgen_ULte: _LL1F: _LL20: {
# 1092
int k=-(int)c1;
return({struct Cyc_Vcgen_Graph*_tmp496=g;void*_tmp495=Cyc_Vcgen_zero();void*_tmp494=t2;Cyc_Vcgen_add_edge(_tmp496,0,_tmp495,_tmp494,k);});}case Cyc_Vcgen_SLt: _LL21: _LL22:
# 1095
 c1=c1 + (unsigned)1;
goto _LL24;case Cyc_Vcgen_SLte: _LL23: _LL24: {
# 1098
int k=-(int)c1;
return({struct Cyc_Vcgen_Graph*_tmp499=g;void*_tmp498=Cyc_Vcgen_zero();void*_tmp497=t2;Cyc_Vcgen_add_edge(_tmp499,1,_tmp498,_tmp497,k);});}default: _LL25: _LL26:
({void*_tmp185=0U;({struct _fat_ptr _tmp49A=({const char*_tmp186="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp186,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp49A,_tag_fat(_tmp185,sizeof(void*),0U));});});}_LL1C:;}}}{
# 1105
enum Cyc_Vcgen_Primreln _tmp187=p;switch(_tmp187){case Cyc_Vcgen_ULt: _LL28: _LL29:
# 1108
 return Cyc_Vcgen_add_edge(g,0,t1,t2,- 1);case Cyc_Vcgen_ULte: _LL2A: _LL2B:
# 1111
 return Cyc_Vcgen_add_edge(g,0,t1,t2,0);case Cyc_Vcgen_SLt: _LL2C: _LL2D:
# 1114
 return Cyc_Vcgen_add_edge(g,1,t1,t2,- 1);case Cyc_Vcgen_SLte: _LL2E: _LL2F:
# 1117
 return Cyc_Vcgen_add_edge(g,1,t1,t2,0);default: _LL30: _LL31:
({void*_tmp188=0U;({struct _fat_ptr _tmp49B=({const char*_tmp189="Vcgen:found bad primop in add_constraint";_tag_fat(_tmp189,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp49B,_tag_fat(_tmp188,sizeof(void*),0U));});});}_LL27:;}}}}char Cyc_Vcgen_TooLarge[9U]="TooLarge";struct Cyc_Vcgen_TooLarge_exn_struct{char*tag;};
# 1123
struct Cyc_Vcgen_TooLarge_exn_struct Cyc_Vcgen_too_large={Cyc_Vcgen_TooLarge};
# 1126
unsigned Cyc_Vcgen_max_paths=33U;
unsigned Cyc_Vcgen_max_paths_seen=0U;
# 1135
static struct Cyc_Vcgen_Graph*Cyc_Vcgen_cgraph(struct Cyc_Vcgen_Graph*gs,void*a){
LOOP:
# 1139
 if(gs == 0)return gs;
{void*_tmp18A=a;void*_tmp18D;enum Cyc_Vcgen_Primreln _tmp18C;void*_tmp18B;void*_tmp18F;void*_tmp18E;void*_tmp191;void*_tmp190;void*_tmp193;void*_tmp192;void*_tmp195;void*_tmp194;switch(*((int*)_tmp18A)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 gs=0;goto _LL0;case 3U: _LL5: _tmp194=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp18A)->f1;_tmp195=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp18A)->f2;_LL6: {void*a1=_tmp194;void*a2=_tmp195;
# 1144
gs=Cyc_Vcgen_cgraph(gs,a2);
a=a1;
goto LOOP;}case 4U: _LL7: _tmp192=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp18A)->f1;_tmp193=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp18A)->f2;_LL8: {void*a1=_tmp192;void*a2=_tmp193;
# 1148
unsigned n=Cyc_Vcgen_num_graphs(gs);
if(Cyc_Vcgen_max_paths_seen < n)Cyc_Vcgen_max_paths_seen=n;
# 1151
if(n >= Cyc_Vcgen_max_paths)(int)_throw((void*)& Cyc_Vcgen_too_large);{
# 1153
struct Cyc_Vcgen_Graph*gs1=gs;
struct Cyc_Vcgen_Graph*gs2=Cyc_Vcgen_copy_graph(gs);
# 1156
gs1=Cyc_Vcgen_cgraph(gs1,a1);
# 1158
gs2=Cyc_Vcgen_cgraph(gs2,a2);
# 1160
gs=Cyc_Vcgen_graph_append(gs1,gs2);
goto _LL0;}}case 2U: switch(((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f2){case Cyc_Vcgen_Neq: _LL9: _tmp190=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f1;_tmp191=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f3;_LLA: {void*t1=_tmp190;void*t2=_tmp191;
# 1165
a=({void*_tmp49E=({void*_tmp49C=Cyc_Vcgen_slt(t1,t2);Cyc_Vcgen_or(_tmp49C,Cyc_Vcgen_slt(t2,t1));});Cyc_Vcgen_and(_tmp49E,({void*_tmp49D=Cyc_Vcgen_ult(t1,t2);Cyc_Vcgen_or(_tmp49D,Cyc_Vcgen_ult(t2,t1));}));});
goto LOOP;}case Cyc_Vcgen_Eq: _LLB: _tmp18E=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f1;_tmp18F=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f3;_LLC: {void*t1=_tmp18E;void*t2=_tmp18F;
# 1169
a=({void*_tmp4A1=({void*_tmp4A0=({void*_tmp49F=Cyc_Vcgen_slte(t1,t2);Cyc_Vcgen_and(_tmp49F,Cyc_Vcgen_slte(t2,t1));});Cyc_Vcgen_and(_tmp4A0,Cyc_Vcgen_ulte(t1,t2));});Cyc_Vcgen_and(_tmp4A1,Cyc_Vcgen_ulte(t2,t1));});
goto LOOP;}default: _LLD: _tmp18B=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f1;_tmp18C=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f2;_tmp18D=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp18A)->f3;_LLE: {void*t1=_tmp18B;enum Cyc_Vcgen_Primreln p=_tmp18C;void*t2=_tmp18D;
# 1174
struct Cyc_Vcgen_Graph*prev=0;
{struct Cyc_Vcgen_Graph*_tmp196=gs;struct Cyc_Vcgen_Graph*g=_tmp196;for(0;g != 0;g=g->next){
if(!Cyc_Vcgen_add_constraint(g,t1,p,t2)){
if(prev == 0)
gs=g->next;else{
# 1180
prev->next=g->next;}}else{
# 1183
prev=g;}}}
# 1186
goto _LL0;}}default: _LLF: _LL10:
# 1189
 a=Cyc_Vcgen_reduce(a);
goto LOOP;}_LL0:;}
# 1193
return gs;}
# 1199
static int Cyc_Vcgen_consistent(void*a){
struct _handler_cons _tmp197;_push_handler(& _tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){
# 1202
{struct _fat_ptr gs=_tag_fat(({struct Cyc_Vcgen_Graph*_tmp4A2=Cyc_Vcgen_true_graph();Cyc_Vcgen_cgraph(_tmp4A2,a);}),sizeof(struct Cyc_Vcgen_Graph),1U);
# 1205
int _tmp19A=({struct Cyc_Vcgen_Graph*_tmp4A3=(struct Cyc_Vcgen_Graph*)gs.curr;_tmp4A3 != (struct Cyc_Vcgen_Graph*)(_tag_fat(0,0,0)).curr;});_npop_handler(0U);return _tmp19A;}
# 1202
;_pop_handler();}else{void*_tmp198=(void*)Cyc_Core_get_exn_thrown();void*_tmp19B=_tmp198;void*_tmp19C;if(((struct Cyc_Vcgen_TooLarge_exn_struct*)_tmp19B)->tag == Cyc_Vcgen_TooLarge){_LL1: _LL2:
# 1209
 return 1;}else{_LL3: _tmp19C=_tmp19B;_LL4: {void*exn=_tmp19C;(int)_rethrow(exn);}}_LL0:;}}}
# 1214
static int Cyc_Vcgen_constraint_prove(void*ctxt,void*a){
void*b=({void*_tmp4A4=ctxt;Cyc_Vcgen_and(_tmp4A4,Cyc_Vcgen_not(a));});
return !Cyc_Vcgen_consistent(b);}
# 1221
static int Cyc_Vcgen_implies_noreduce(void*a1,void*a2){
return Cyc_Vcgen_simple_prove(a1,a2)|| Cyc_Vcgen_constraint_prove(a1,a2);}
# 1227
static int Cyc_Vcgen_implies(void**a1,void**a2){
({void*_tmp4A5=Cyc_Vcgen_reduce(*a1);*a1=_tmp4A5;});
({void*_tmp4A6=Cyc_Vcgen_reduce(*a2);*a2=_tmp4A6;});
return Cyc_Vcgen_implies_noreduce(*a1,*a2);}
# 1238
static void*Cyc_Vcgen_weaken_it(void*ctxt,void*a){
if(Cyc_Vcgen_simple_prove(ctxt,a))return a;{
void*res;
{void*_tmp19D=a;void*_tmp19F;void*_tmp19E;void*_tmp1A1;void*_tmp1A0;switch(*((int*)_tmp19D)){case 3U: _LL1: _tmp1A0=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp19D)->f1;_tmp1A1=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp19D)->f2;_LL2: {void*a1=_tmp1A0;void*a2=_tmp1A1;
# 1243
void*_tmp1A2=Cyc_Vcgen_weaken_it(ctxt,a1);void*b1=_tmp1A2;
void*_tmp1A3=Cyc_Vcgen_weaken_it(ctxt,a2);void*b2=_tmp1A3;
# 1247
res=Cyc_Vcgen_and(b1,b2);
goto _LL0;}case 4U: _LL3: _tmp19E=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp19D)->f1;_tmp19F=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp19D)->f2;_LL4: {void*a1=_tmp19E;void*a2=_tmp19F;
# 1250
void*_tmp1A4=Cyc_Vcgen_weaken_it(a2,a1);void*b1=_tmp1A4;
void*_tmp1A5=Cyc_Vcgen_weaken_it(a1,a2);void*b2=_tmp1A5;
res=({void*_tmp4A7=ctxt;Cyc_Vcgen_weaken_it(_tmp4A7,Cyc_Vcgen_and(b1,b2));});
goto _LL0;}case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA:
# 1257
 res=(void*)& Cyc_Vcgen_true_assn;
goto _LL0;default: _LLB: _LLC:
 return({void*_tmp4A8=ctxt;Cyc_Vcgen_weaken_it(_tmp4A8,Cyc_Vcgen_reduce(a));});}_LL0:;}
# 1261
return res;}}
# 1270
static void*Cyc_Vcgen_weaken_assn(void*ctxt,void*a){
if(Cyc_Vcgen_implies(& ctxt,& a))return a;
return Cyc_Vcgen_weaken_it(ctxt,a);}
# 1276
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd);
# 1279
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmp1A6=tgt;struct Cyc_Absyn_Vardecl*_tmp1A7;if(_tmp1A6 == 0){_LL1: _LL2:
# 1284
 return Cyc_Vcgen_kill(a);}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp1A6)->tag == 1U){_LL3: _tmp1A7=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp1A6)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp1A7;
# 1290
if(vd->escapes){
a=Cyc_Vcgen_kill(a);
src=src == 0?0: Cyc_Vcgen_kill_mem_term(src);}else{
# 1294
void*_tmp1A8=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmp1A8;
a=Cyc_Vcgen_subst(vd,v2,a);
src=src == 0?0: Cyc_Vcgen_subst_term(src,vd,v2);}
# 1299
return({void*_tmp4A9=a;Cyc_Vcgen_and(_tmp4A9,Cyc_Vcgen_eq(tgt,src));});}}else{_LL5: _LL6:
({void*_tmp1A9=0U;({struct _fat_ptr _tmp4AA=({const char*_tmp1AA="vcgen do_assign";_tag_fat(_tmp1AA,sizeof(char),16U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp4AA,_tag_fat(_tmp1A9,sizeof(void*),0U));});});}}_LL0:;}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void*try_assn;void**exp_stmt;void*res_term;void*res_assn;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;};
# 1338 "/tmp/cyclone/src/vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 1340
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
# 1342
struct Cyc_Hashtable_Table*_tmp1AB=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmp1AB;
struct Cyc_Hashtable_Table*_tmp1AC=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_table=_tmp1AC;
struct Cyc_Vcgen_SharedEnv*_tmp1AD=({struct Cyc_Vcgen_SharedEnv*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->assn_table=assn_table,_tmp1B0->succ_table=succ_table,_tmp1B0->try_assn=(void*)& Cyc_Vcgen_false_assn,_tmp1B0->exp_stmt=0,_tmp1B0->res_term=0,_tmp1B0->res_assn=(void*)& Cyc_Vcgen_false_assn;_tmp1B0;});struct Cyc_Vcgen_SharedEnv*senv=_tmp1AD;
# 1350
struct Cyc_Dict_Dict _tmp1AE=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty)((int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp);struct Cyc_Dict_Dict d=_tmp1AE;
return({struct Cyc_Vcgen_Env*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->shared=senv,_tmp1AF->state_counter=d;_tmp1AF;});}
# 1354
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));*_tmp1B1=*env;_tmp1B1;});}
# 1360
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 1364
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp4AB=((struct Cyc_Dict_Dict(*)(int(*f)(int,struct Cyc_Absyn_Vardecl*,int,int),int env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp4AB;});}
# 1372
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int*ans))Cyc_Dict_lookup_bool)(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp4AC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int v))Cyc_Dict_insert)(env->state_counter,vd,res + 1);env->state_counter=_tmp4AC;});
return(void*)({struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->tag=2U,_tmp1B2->f1=vd,_tmp1B2->f2=res;_tmp1B2;});}
# 1381
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp1B3=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);void***popt=_tmp1B3;
if(popt != 0){
void**p=*popt;
void*_tmp1B4=Cyc_Vcgen_or(ain,*p);void*new_precondition=_tmp1B4;
*p=new_precondition;
return new_precondition;}else{
# 1389
return ain;}}
# 1393
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_Vcgen_false_assn;
void***popt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 1399
return& false_pointer;}}
# 1404
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp1B5=*aprevopt;void**aprev=_tmp1B5;
ain=Cyc_Vcgen_weaken_assn(ain,*aprev);
*aprev=ain;}else{
# 1411
({struct Cyc_Hashtable_Table*_tmp4AE=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp4AD=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp4AE,_tmp4AD,({void**_tmp1B6=_cycalloc(sizeof(*_tmp1B6));*_tmp1B6=ain;_tmp1B6;}));});}
# 1413
return ain;}
# 1418
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=((void***(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp1B7=*aprevopt;void**aprev=_tmp1B7;
ain=Cyc_Vcgen_or(ain,*aprev);
*aprev=ain;}else{
# 1425
({struct Cyc_Hashtable_Table*_tmp4B0=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp4AF=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp4B0,_tmp4AF,({void**_tmp1B8=_cycalloc(sizeof(*_tmp1B8));*_tmp1B8=ain;_tmp1B8;}));});}
# 1427
return ain;}
# 1432
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
({void*_tmp4B1=Cyc_Vcgen_or((env->shared)->try_assn,a);(env->shared)->try_assn=_tmp4B1;});}
# 1437
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a){
struct Cyc_Vcgen_SharedEnv*_tmp1B9=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp1B9;
if(shared->res_term != 0 || t != 0){
void*_tmp1BA=Cyc_Vcgen_fresh_var();void*v=_tmp1BA;
shared->res_term=v;
({void*_tmp4B5=({
void*_tmp4B4=({void*_tmp4B2=a;Cyc_Vcgen_and(_tmp4B2,Cyc_Vcgen_eq(v,t));});Cyc_Vcgen_or(_tmp4B4,({void*_tmp4B3=shared->res_assn;Cyc_Vcgen_and(_tmp4B3,Cyc_Vcgen_eq(v,shared->res_term));}));});
# 1442
shared->res_assn=_tmp4B5;});}else{
# 1445
({void*_tmp4B6=Cyc_Vcgen_or(a,shared->res_assn);shared->res_assn=_tmp4B6;});}}
# 1454
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain);
# 1458
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 1460
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 1462
static struct _tuple12 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 1465
static struct _tuple12 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);struct _tuple19{struct Cyc_List_List**f1;void*f2;};
# 1469
static struct _tuple19 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);
# 1472
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);
# 1474
static struct _tuple17 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain);
# 1476
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain);
# 1478
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a);
# 1480
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_tmp1BB=s->r;void*_stmttmp13=_tmp1BB;void*_tmp1BC=_stmttmp13;struct Cyc_Absyn_Stmt*_tmp1BD;struct Cyc_Absyn_Stmt*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C5;switch(*((int*)_tmp1BC)){case 4U: _LL1: _tmp1C5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp1C5;
# 1483
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C8=({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0U,({struct _fat_ptr _tmp4B7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3E8.f1=_tmp4B7;});_tmp3E8;});void*_tmp1C6[1U];_tmp1C6[0]=& _tmp1C8;({struct _fat_ptr _tmp4B8=({const char*_tmp1C7="if (%s) ...";_tag_fat(_tmp1C7,sizeof(char),12U);});Cyc_aprintf(_tmp4B8,_tag_fat(_tmp1C6,sizeof(void*),1U));});});}case 5U: _LL3: _tmp1C4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1).f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp1C4;
# 1485
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1CB=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0U,({struct _fat_ptr _tmp4B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3E9.f1=_tmp4B9;});_tmp3E9;});void*_tmp1C9[1U];_tmp1C9[0]=& _tmp1CB;({struct _fat_ptr _tmp4BA=({const char*_tmp1CA="while (%s) ...";_tag_fat(_tmp1CA,sizeof(char),15U);});Cyc_aprintf(_tmp4BA,_tag_fat(_tmp1C9,sizeof(void*),1U));});});}case 9U: _LL5: _tmp1C1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_tmp1C2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2).f1;_tmp1C3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f3).f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp1C1;struct Cyc_Absyn_Exp*e2=_tmp1C2;struct Cyc_Absyn_Exp*e3=_tmp1C3;
# 1487
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1CE=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0U,({struct _fat_ptr _tmp4BB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp3EC.f1=_tmp4BB;});_tmp3EC;});struct Cyc_String_pa_PrintArg_struct _tmp1CF=({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0U,({
struct _fat_ptr _tmp4BC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp3EB.f1=_tmp4BC;});_tmp3EB;});struct Cyc_String_pa_PrintArg_struct _tmp1D0=({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0U,({struct _fat_ptr _tmp4BD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp3EA.f1=_tmp4BD;});_tmp3EA;});void*_tmp1CC[3U];_tmp1CC[0]=& _tmp1CE,_tmp1CC[1]=& _tmp1CF,_tmp1CC[2]=& _tmp1D0;({struct _fat_ptr _tmp4BE=({const char*_tmp1CD="for (%s;%s;%s) ...";_tag_fat(_tmp1CD,sizeof(char),19U);});Cyc_aprintf(_tmp4BE,_tag_fat(_tmp1CC,sizeof(void*),3U));});});}case 10U: _LL7: _tmp1C0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp1C0;
# 1490
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1D3=({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0U,({struct _fat_ptr _tmp4BF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3ED.f1=_tmp4BF;});_tmp3ED;});void*_tmp1D1[1U];_tmp1D1[0]=& _tmp1D3;({struct _fat_ptr _tmp4C0=({const char*_tmp1D2="switch (%s) ...";_tag_fat(_tmp1D2,sizeof(char),16U);});Cyc_aprintf(_tmp4C0,_tag_fat(_tmp1D1,sizeof(void*),1U));});});}case 14U: _LL9: _tmp1BF=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f2).f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp1BF;
# 1492
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1D6=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0U,({struct _fat_ptr _tmp4C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3EE.f1=_tmp4C1;});_tmp3EE;});void*_tmp1D4[1U];_tmp1D4[0]=& _tmp1D6;({struct _fat_ptr _tmp4C2=({const char*_tmp1D5="do ... while (%s)";_tag_fat(_tmp1D5,sizeof(char),18U);});Cyc_aprintf(_tmp4C2,_tag_fat(_tmp1D4,sizeof(void*),1U));});});}case 15U: _LLB: _tmp1BE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp1BE;
# 1494
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1D9=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0U,({struct _fat_ptr _tmp4C3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp3EF.f1=_tmp4C3;});_tmp3EF;});void*_tmp1D7[1U];_tmp1D7[0]=& _tmp1D9;({struct _fat_ptr _tmp4C4=({const char*_tmp1D8="try %s catch ...";_tag_fat(_tmp1D8,sizeof(char),17U);});Cyc_aprintf(_tmp4C4,_tag_fat(_tmp1D7,sizeof(void*),1U));});});}case 2U: _LLD: _tmp1BD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1BC)->f1;_LLE: {struct Cyc_Absyn_Stmt*s=_tmp1BD;
return Cyc_Vcgen_stmt2shortstring(s);}default: _LLF: _LL10: {
# 1497
struct _fat_ptr _tmp1DA=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp1DA;
unsigned max=_get_fat_size(s,sizeof(char))< (unsigned)11?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp1DE=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp1DE),1U,_tmp1DE);});
{int i=0;for(0;(unsigned)i < max - (unsigned)1;++ i){
({char _tmp4C5=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp4C5;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1DD=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0U,_tmp3F0.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp3F0;});void*_tmp1DB[1U];_tmp1DB[0]=& _tmp1DD;({struct _fat_ptr _tmp4C6=({const char*_tmp1DC="%s...";_tag_fat(_tmp1DC,sizeof(char),6U);});Cyc_aprintf(_tmp4C6,_tag_fat(_tmp1DB,sizeof(void*),1U));});});}}_LL0:;}struct _tuple20{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 1556 "/tmp/cyclone/src/vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 1559
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);
# 1564
{void*_tmp1DF=ain;if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp1DF)->tag == 1U){_LL1: _LL2:
 return ain;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1568
void*_tmp1E0=s->r;void*_stmttmp14=_tmp1E0;void*_tmp1E1=_stmttmp14;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Stmt*_tmp1E2;struct Cyc_Absyn_Stmt*_tmp1E4;struct Cyc_Absyn_Stmt*_tmp1E6;struct Cyc_Absyn_Decl*_tmp1E5;struct Cyc_Absyn_Switch_clause*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Stmt*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Stmt*_tmp1EB;struct Cyc_Absyn_Stmt*_tmp1F3;struct Cyc_Absyn_Stmt*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Stmt*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Stmt*_tmp1F6;struct Cyc_Absyn_Stmt*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Stmt*_tmp1F9;struct Cyc_Absyn_Stmt*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Stmt*_tmp1FC;struct Cyc_Absyn_Stmt*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;switch(*((int*)_tmp1E1)){case 0U: _LL6: _LL7:
 return ain;case 1U: _LL8: _tmp1FD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp1FD;
# 1571
struct _tuple12 _tmp1FE=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp15=_tmp1FE;struct _tuple12 _tmp1FF=_stmttmp15;void*_tmp201;void*_tmp200;_LL2B: _tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;_LL2C: {void*t=_tmp200;void*aout=_tmp201;
# 1574
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2U: _LLA: _tmp1FB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1FC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LLB: {struct Cyc_Absyn_Stmt*s1=_tmp1FB;struct Cyc_Absyn_Stmt*s2=_tmp1FC;
# 1578
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1 == 0){_LLC: _LLD:
# 1582
 Cyc_Vcgen_update_return(env,0,ain);
return(void*)& Cyc_Vcgen_false_assn;}else{_LLE: _tmp1FA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp1FA;
# 1585
struct _tuple12 _tmp202=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple12 _stmttmp16=_tmp202;struct _tuple12 _tmp203=_stmttmp16;void*_tmp205;void*_tmp204;_LL2E: _tmp204=_tmp203.f1;_tmp205=_tmp203.f2;_LL2F: {void*t=_tmp204;void*aout=_tmp205;
Cyc_Vcgen_update_return(env,t,aout);
return(void*)& Cyc_Vcgen_false_assn;}}}case 4U: _LL10: _tmp1F7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1F8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_tmp1F9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f3;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp1F7;struct Cyc_Absyn_Stmt*s1=_tmp1F8;struct Cyc_Absyn_Stmt*s2=_tmp1F9;
# 1589
struct _tuple12 _tmp206=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp17=_tmp206;struct _tuple12 _tmp207=_stmttmp17;void*_tmp209;void*_tmp208;_LL31: _tmp208=_tmp207.f1;_tmp209=_tmp207.f2;_LL32: {void*at=_tmp208;void*af=_tmp209;
struct Cyc_Vcgen_Env*_tmp20A=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp20A;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return Cyc_Vcgen_or(at,af);}}case 5U: _LL12: _tmp1F4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1).f1;_tmp1F5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1).f2;_tmp1F6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LL13: {struct Cyc_Absyn_Exp*e=_tmp1F4;struct Cyc_Absyn_Stmt*dummy=_tmp1F5;struct Cyc_Absyn_Stmt*s=_tmp1F6;
# 1601
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);{
struct _tuple12 _tmp20B=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple12 _stmttmp18=_tmp20B;struct _tuple12 _tmp20C=_stmttmp18;void*_tmp20E;void*_tmp20D;_LL34: _tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;_LL35: {void*at=_tmp20D;void*af=_tmp20E;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7U: _LL14: _LL15:
# 1607
 goto _LL17;{
# 1610
struct Cyc_Absyn_Stmt*_tmp20F=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp20F;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_Vcgen_false_assn;}case 6U: _LL16: _LL17: {
# 1617
struct Cyc_Absyn_Stmt*_tmp210=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp210;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain);else{
# 1621
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_Vcgen_false_assn;}case 9U: _LL18: _tmp1EE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1EF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2).f1;_tmp1F0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2).f2;_tmp1F1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f3).f1;_tmp1F2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f3).f2;_tmp1F3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f4;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp1EE;struct Cyc_Absyn_Exp*e2=_tmp1EF;struct Cyc_Absyn_Stmt*dummy2=_tmp1F0;struct Cyc_Absyn_Exp*e3=_tmp1F1;struct Cyc_Absyn_Stmt*dummy3=_tmp1F2;struct Cyc_Absyn_Stmt*s=_tmp1F3;
# 1624
struct _tuple12 _tmp211=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,ain);struct _tuple12 _stmttmp19=_tmp211;struct _tuple12 _tmp212=_stmttmp19;void*_tmp213;_LL37: _tmp213=_tmp212.f2;_LL38: {void*a=_tmp213;
ain=a;
# 1627
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);{
struct _tuple12 _tmp214=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple12 _stmttmp1A=_tmp214;struct _tuple12 _tmp215=_stmttmp1A;void*_tmp217;void*_tmp216;_LL3A: _tmp216=_tmp215.f1;_tmp217=_tmp215.f2;_LL3B: {void*at=_tmp216;void*af=_tmp217;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 1632
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2)))return af;}}}}}case 14U: _LL1A: _tmp1EB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1EC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2).f1;_tmp1ED=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2).f2;_LL1B: {struct Cyc_Absyn_Stmt*s=_tmp1EB;struct Cyc_Absyn_Exp*e=_tmp1EC;struct Cyc_Absyn_Stmt*dummy=_tmp1ED;
# 1638
while(1){
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple12 _tmp218=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple12 _stmttmp1B=_tmp218;struct _tuple12 _tmp219=_stmttmp1B;void*_tmp21B;void*_tmp21A;_LL3D: _tmp21A=_tmp219.f1;_tmp21B=_tmp219.f2;_LL3E: {void*at=_tmp21A;void*af=_tmp21B;
ain=({void*_tmp4C7=*Cyc_Vcgen_get_assn(env,dummy);Cyc_Vcgen_weaken_assn(_tmp4C7,ain);});
if(Cyc_Vcgen_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10U: _LL1C: _tmp1E9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1EA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LL1D: {struct Cyc_Absyn_Exp*e=_tmp1E9;struct Cyc_List_List*scs=_tmp1EA;
# 1651
struct _tuple12 _tmp21C=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp1C=_tmp21C;struct _tuple12 _tmp21D=_stmttmp1C;void*_tmp21F;void*_tmp21E;_LL40: _tmp21E=_tmp21D.f1;_tmp21F=_tmp21D.f2;_LL41: {void*t=_tmp21E;void*a=_tmp21F;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2 == 0){_LL1E: _LL1F:
({void*_tmp220=0U;({struct _fat_ptr _tmp4C8=({const char*_tmp221="vcgen fallthru";_tag_fat(_tmp221,sizeof(char),15U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp4C8,_tag_fat(_tmp220,sizeof(void*),0U));});});}else{_LL20: _tmp1E7=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1E8=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LL21: {struct Cyc_List_List*es=_tmp1E7;struct Cyc_Absyn_Switch_clause*dest=_tmp1E8;
# 1656
struct _tuple17 _tmp222=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple17 _stmttmp1D=_tmp222;struct _tuple17 _tmp223=_stmttmp1D;void*_tmp225;struct Cyc_List_List*_tmp224;_LL43: _tmp224=_tmp223.f1;_tmp225=_tmp223.f2;_LL44: {struct Cyc_List_List*ts=_tmp224;void*a=_tmp225;
# 1658
{struct Cyc_List_List*_tmp226=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v;struct Cyc_List_List*x=_tmp226;for(0;x != 0;(x=x->tl,ts=((struct Cyc_List_List*)_check_null(ts))->tl)){
struct _tuple20*_tmp227=(struct _tuple20*)x->hd;struct _tuple20*_stmttmp1E=_tmp227;struct _tuple20*_tmp228=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp22A;struct Cyc_Absyn_Vardecl**_tmp229;_LL46: _tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;_LL47: {struct Cyc_Absyn_Vardecl**vd=_tmp229;struct Cyc_Absyn_Exp*ve=_tmp22A;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp4CB=env;void*_tmp4CA=a;void*_tmp4C9=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B->tag=1U,_tmp22B->f1=*vd;_tmp22B;});Cyc_Vcgen_do_assign(_tmp4CB,_tmp4CA,_tmp4C9,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}}
# 1663
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_Vcgen_false_assn;}}}case 12U: _LL22: _tmp1E5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1E6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LL23: {struct Cyc_Absyn_Decl*d=_tmp1E5;struct Cyc_Absyn_Stmt*s1=_tmp1E6;
# 1666
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13U: _LL24: _tmp1E4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LL25: {struct Cyc_Absyn_Stmt*s=_tmp1E4;
# 1674
return Cyc_Vcgen_vcgen_stmt_nodebug(env,s,(void*)& Cyc_Vcgen_true_assn);}case 8U: _LL26: _LL27:
 return(void*)& Cyc_Vcgen_false_assn;default: _LL28: _tmp1E2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f1;_tmp1E3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1E1)->f2;_LL29: {struct Cyc_Absyn_Stmt*s=_tmp1E2;struct Cyc_List_List*scs=_tmp1E3;
# 1678
void*_tmp22C=(env->shared)->try_assn;void*old_try_assn=_tmp22C;
# 1680
(env->shared)->try_assn=(void*)& Cyc_Vcgen_false_assn;
# 1683
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 1685
void*_tmp22D=(env->shared)->try_assn;void*new_try_assn=_tmp22D;
(env->shared)->try_assn=old_try_assn;
# 1689
return({void*_tmp4CC=ain;Cyc_Vcgen_or(_tmp4CC,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}}_LL5:;}}
# 1693
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 1697
void*res=(void*)& Cyc_Vcgen_false_assn;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp22E=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp22E;
void*_tmp22F=ain;void*a=_tmp22F;
{struct Cyc_List_List*_tmp230=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmp230;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple20*_tmp231=(struct _tuple20*)pvs->hd;struct _tuple20*_stmttmp1F=_tmp231;struct _tuple20*_tmp232=_stmttmp1F;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Vardecl**_tmp233;_LL1: _tmp233=_tmp232->f1;_tmp234=_tmp232->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp233;struct Cyc_Absyn_Exp*eopt=_tmp234;
if(vdopt != 0){
if(eopt != 0)
a=({struct Cyc_Vcgen_Env*_tmp4CF=env;void*_tmp4CE=a;void*_tmp4CD=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp235=_cycalloc(sizeof(*_tmp235));_tmp235->tag=1U,_tmp235->f1=*vdopt;_tmp235;});Cyc_Vcgen_do_assign(_tmp4CF,_tmp4CE,_tmp4CD,(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a)).f1);});else{
# 1707
a=({struct Cyc_Vcgen_Env*_tmp4D2=env;void*_tmp4D1=a;void*_tmp4D0=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->tag=1U,_tmp236->f1=*vdopt;_tmp236;});Cyc_Vcgen_do_assign(_tmp4D2,_tmp4D1,_tmp4D0,Cyc_Vcgen_fresh_var());});}}}}}
# 1710
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp4D3=res;Cyc_Vcgen_or(_tmp4D3,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 1714
return res;}
# 1717
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 1720
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp237=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->tag=1U,_tmp23C->f1=vd;_tmp23C;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*tv=_tmp237;
a=({struct Cyc_Vcgen_Env*_tmp4D6=env;void*_tmp4D5=a;void*_tmp4D4=(void*)tv;Cyc_Vcgen_do_assign(_tmp4D6,_tmp4D5,_tmp4D4,Cyc_Vcgen_fresh_var());});
if(vd->initializer != 0){
struct _tuple12 _tmp238=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple12 _stmttmp20=_tmp238;struct _tuple12 _tmp239=_stmttmp20;void*_tmp23B;void*_tmp23A;_LL1: _tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;_LL2: {void*t=_tmp23A;void*a1=_tmp23B;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 1726
return a;}
# 1729
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmp23D=d->r;void*_stmttmp21=_tmp23D;void*_tmp23E=_stmttmp21;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Vardecl*_tmp23F;struct Cyc_List_List*_tmp241;struct Cyc_Absyn_Exp*_tmp244;struct Cyc_Core_Opt*_tmp243;struct Cyc_Absyn_Pat*_tmp242;struct Cyc_Absyn_Vardecl*_tmp245;switch(*((int*)_tmp23E)){case 0U: _LL1: _tmp245=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp23E)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp245;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2U: _LL3: _tmp242=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp23E)->f1;_tmp243=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp23E)->f2;_tmp244=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp23E)->f3;_LL4: {struct Cyc_Absyn_Pat*p=_tmp242;struct Cyc_Core_Opt*vds=_tmp243;struct Cyc_Absyn_Exp*e=_tmp244;
# 1733
struct _tuple12 _tmp246=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmp22=_tmp246;struct _tuple12 _tmp247=_stmttmp22;void*_tmp248;_LLC: _tmp248=_tmp247.f2;_LLD: {void*a=_tmp248;
{struct Cyc_List_List*_tmp249=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmp249;for(0;vds != 0;vds=vds->tl){
struct _tuple20 _tmp24A=*((struct _tuple20*)vds->hd);struct _tuple20 _stmttmp23=_tmp24A;struct _tuple20 _tmp24B=_stmttmp23;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Vardecl**_tmp24C;_LLF: _tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;_LL10: {struct Cyc_Absyn_Vardecl**vdopt=_tmp24C;struct Cyc_Absyn_Exp*eopt=_tmp24D;
if(vdopt != 0){
if(eopt == 0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 1739
struct _tuple12 _tmp24E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmp24=_tmp24E;struct _tuple12 _tmp24F=_stmttmp24;void*_tmp251;void*_tmp250;_LL12: _tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;_LL13: {void*t=_tmp250;void*a1=_tmp251;
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp252=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->tag=1U,_tmp253->f1=*vdopt;_tmp253;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*tv=_tmp252;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 1745
return a;}}case 3U: _LL5: _tmp241=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp23E)->f1;_LL6: {struct Cyc_List_List*vds=_tmp241;
# 1747
for(0;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4U: _LL7: _tmp23F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp23E)->f2;_tmp240=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp23E)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp23F;struct Cyc_Absyn_Exp*eopt=_tmp240;
# 1751
if(eopt != 0){
struct _tuple12 _tmp254=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple12 _stmttmp25=_tmp254;struct _tuple12 _tmp255=_stmttmp25;void*_tmp257;void*_tmp256;_LL15: _tmp256=_tmp255.f1;_tmp257=_tmp255.f2;_LL16: {void*t=_tmp256;void*a1=_tmp257;
a=a1;}}
# 1756
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default: _LL9: _LLA:
 return a;}_LL0:;}
# 1764
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp258=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*t1=_tmp258;
void*_tmp259=Cyc_Absyn_compress((void*)_check_null(e2->topt));void*t2=_tmp259;
struct _tuple12 _tmp25A=({struct _tuple12 _tmp3F1;_tmp3F1.f1=t1,_tmp3F1.f2=t2;_tmp3F1;});struct _tuple12 _stmttmp26=_tmp25A;struct _tuple12 _tmp25B=_stmttmp26;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f1)->f1)->f1 == Cyc_Absyn_Unsigned){_LL1: _LL2:
 return 1;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25B.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3: _LL4:
 return 1;}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25B.f1)->tag == 3U)goto _LL7;else{goto _LL7;}}}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25B.f1)->tag == 3U)goto _LL7;else{goto _LL7;}}}else{if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25B.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25B.f2)->tag == 3U){_LL5: _LL6:
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}}}_LL0:;}
# 1775
static struct _tuple12 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp25C=e->r;void*_stmttmp27=_tmp25C;void*_tmp25D=_stmttmp27;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp25F;enum Cyc_Absyn_Primop _tmp25E;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp268;switch(*((int*)_tmp25D)){case 6U: _LL1: _tmp268=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp269=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_tmp26A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp25D)->f3;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp268;struct Cyc_Absyn_Exp*e2=_tmp269;struct Cyc_Absyn_Exp*e3=_tmp26A;
# 1778
struct _tuple12 _tmp26B=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp28=_tmp26B;struct _tuple12 _tmp26C=_stmttmp28;void*_tmp26E;void*_tmp26D;_LL10: _tmp26D=_tmp26C.f1;_tmp26E=_tmp26C.f2;_LL11: {void*a1t=_tmp26D;void*a1f=_tmp26E;
struct _tuple12 _tmp26F=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple12 _stmttmp29=_tmp26F;struct _tuple12 _tmp270=_stmttmp29;void*_tmp272;void*_tmp271;_LL13: _tmp271=_tmp270.f1;_tmp272=_tmp270.f2;_LL14: {void*a2t=_tmp271;void*a2f=_tmp272;
struct _tuple12 _tmp273=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple12 _stmttmp2A=_tmp273;struct _tuple12 _tmp274=_stmttmp2A;void*_tmp276;void*_tmp275;_LL16: _tmp275=_tmp274.f1;_tmp276=_tmp274.f2;_LL17: {void*a3t=_tmp275;void*a3f=_tmp276;
return({struct _tuple12 _tmp3F2;({void*_tmp4D8=Cyc_Vcgen_or(a2t,a3t);_tmp3F2.f1=_tmp4D8;}),({void*_tmp4D7=Cyc_Vcgen_or(a2f,a3f);_tmp3F2.f2=_tmp4D7;});_tmp3F2;});}}}}case 7U: _LL3: _tmp266=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp267=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp266;struct Cyc_Absyn_Exp*e2=_tmp267;
# 1783
struct _tuple12 _tmp277=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2B=_tmp277;struct _tuple12 _tmp278=_stmttmp2B;void*_tmp27A;void*_tmp279;_LL19: _tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;_LL1A: {void*a1t=_tmp279;void*a1f=_tmp27A;
struct _tuple12 _tmp27B=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple12 _stmttmp2C=_tmp27B;struct _tuple12 _tmp27C=_stmttmp2C;void*_tmp27E;void*_tmp27D;_LL1C: _tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;_LL1D: {void*a2t=_tmp27D;void*a2f=_tmp27E;
return({struct _tuple12 _tmp3F3;_tmp3F3.f1=a2t,({void*_tmp4D9=Cyc_Vcgen_or(a1f,a2f);_tmp3F3.f2=_tmp4D9;});_tmp3F3;});}}}case 8U: _LL5: _tmp264=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp265=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp264;struct Cyc_Absyn_Exp*e2=_tmp265;
# 1787
struct _tuple12 _tmp27F=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp2D=_tmp27F;struct _tuple12 _tmp280=_stmttmp2D;void*_tmp282;void*_tmp281;_LL1F: _tmp281=_tmp280.f1;_tmp282=_tmp280.f2;_LL20: {void*a1t=_tmp281;void*a1f=_tmp282;
struct _tuple12 _tmp283=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple12 _stmttmp2E=_tmp283;struct _tuple12 _tmp284=_stmttmp2E;void*_tmp286;void*_tmp285;_LL22: _tmp285=_tmp284.f1;_tmp286=_tmp284.f2;_LL23: {void*a2t=_tmp285;void*a2f=_tmp286;
return({struct _tuple12 _tmp3F4;({void*_tmp4DA=Cyc_Vcgen_or(a1t,a2t);_tmp3F4.f1=_tmp4DA;}),_tmp3F4.f2=a2f;_tmp3F4;});}}}case 9U: _LL7: _tmp262=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp263=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp262;struct Cyc_Absyn_Exp*e2=_tmp263;
# 1791
struct _tuple12 _tmp287=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp2F=_tmp287;struct _tuple12 _tmp288=_stmttmp2F;void*_tmp289;_LL25: _tmp289=_tmp288.f2;_LL26: {void*ain=_tmp289;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2 != 0){_LL9: _tmp261=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2)->hd;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp261;
# 1794
struct _tuple12 _tmp28A=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp30=_tmp28A;struct _tuple12 _tmp28B=_stmttmp30;void*_tmp28D;void*_tmp28C;_LL28: _tmp28C=_tmp28B.f1;_tmp28D=_tmp28B.f2;_LL29: {void*at=_tmp28C;void*af=_tmp28D;
return({struct _tuple12 _tmp3F5;_tmp3F5.f1=af,_tmp3F5.f2=at;_tmp3F5;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2)->tl)->tl == 0){_LLB: _tmp25E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp25F=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2)->hd;_tmp260=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25D)->f2)->tl)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp25E;struct Cyc_Absyn_Exp*e1=_tmp25F;struct Cyc_Absyn_Exp*e2=_tmp260;
# 1797
struct _tuple12 _tmp28E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp31=_tmp28E;struct _tuple12 _tmp28F=_stmttmp31;void*_tmp291;void*_tmp290;_LL2B: _tmp290=_tmp28F.f1;_tmp291=_tmp28F.f2;_LL2C: {void*t1=_tmp290;void*a1=_tmp291;
struct _tuple12 _tmp292=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp32=_tmp292;struct _tuple12 _tmp293=_stmttmp32;void*_tmp295;void*_tmp294;_LL2E: _tmp294=_tmp293.f1;_tmp295=_tmp293.f2;_LL2F: {void*t2=_tmp294;void*a2=_tmp295;
void*_tmp296=a2;void*at=_tmp296;
void*_tmp297=a2;void*af=_tmp297;
{enum Cyc_Absyn_Primop _tmp298=p;switch(_tmp298){case Cyc_Absyn_Eq: _LL31: _LL32:
 at=({void*_tmp4DB=at;Cyc_Vcgen_and(_tmp4DB,Cyc_Vcgen_eq(t1,t2));});af=({void*_tmp4DC=af;Cyc_Vcgen_and(_tmp4DC,Cyc_Vcgen_neq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq: _LL33: _LL34:
 at=({void*_tmp4DD=at;Cyc_Vcgen_and(_tmp4DD,Cyc_Vcgen_neq(t1,t2));});af=({void*_tmp4DE=af;Cyc_Vcgen_and(_tmp4DE,Cyc_Vcgen_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt: _LL35: _LL36:
# 1805
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp4DF=at;Cyc_Vcgen_and(_tmp4DF,Cyc_Vcgen_ult(t1,t2));});af=({void*_tmp4E0=af;Cyc_Vcgen_and(_tmp4E0,Cyc_Vcgen_ulte(t2,t1));});}else{
# 1808
at=({void*_tmp4E1=at;Cyc_Vcgen_and(_tmp4E1,Cyc_Vcgen_slt(t1,t2));});af=({void*_tmp4E2=af;Cyc_Vcgen_and(_tmp4E2,Cyc_Vcgen_slte(t2,t1));});}
# 1810
goto _LL30;case Cyc_Absyn_Lte: _LL37: _LL38:
# 1812
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp4E3=at;Cyc_Vcgen_and(_tmp4E3,Cyc_Vcgen_ulte(t1,t2));});af=({void*_tmp4E4=af;Cyc_Vcgen_and(_tmp4E4,Cyc_Vcgen_ult(t2,t1));});}else{
# 1815
at=({void*_tmp4E5=at;Cyc_Vcgen_and(_tmp4E5,Cyc_Vcgen_slte(t1,t2));});af=({void*_tmp4E6=af;Cyc_Vcgen_and(_tmp4E6,Cyc_Vcgen_slt(t2,t1));});}
# 1817
goto _LL30;case Cyc_Absyn_Gt: _LL39: _LL3A:
# 1819
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp4E7=at;Cyc_Vcgen_and(_tmp4E7,Cyc_Vcgen_ult(t2,t1));});af=({void*_tmp4E8=af;Cyc_Vcgen_and(_tmp4E8,Cyc_Vcgen_ulte(t1,t2));});}else{
# 1822
at=({void*_tmp4E9=at;Cyc_Vcgen_and(_tmp4E9,Cyc_Vcgen_slt(t2,t1));});af=({void*_tmp4EA=af;Cyc_Vcgen_and(_tmp4EA,Cyc_Vcgen_slte(t1,t2));});}
# 1824
goto _LL30;case Cyc_Absyn_Gte: _LL3B: _LL3C:
# 1826
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp4EB=at;Cyc_Vcgen_and(_tmp4EB,Cyc_Vcgen_ulte(t2,t1));});af=({void*_tmp4EC=af;Cyc_Vcgen_and(_tmp4EC,Cyc_Vcgen_ult(t1,t2));});}else{
# 1829
at=({void*_tmp4ED=at;Cyc_Vcgen_and(_tmp4ED,Cyc_Vcgen_slte(t2,t1));});af=({void*_tmp4EE=af;Cyc_Vcgen_and(_tmp4EE,Cyc_Vcgen_slt(t1,t2));});}
# 1831
goto _LL30;default: _LL3D: _LL3E:
 goto _LL30;}_LL30:;}
# 1834
return({struct _tuple12 _tmp3F6;_tmp3F6.f1=at,_tmp3F6.f2=af;_tmp3F6;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: _LLE: {
# 1836
struct _tuple12 _tmp299=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp33=_tmp299;struct _tuple12 _tmp29A=_stmttmp33;void*_tmp29C;void*_tmp29B;_LL40: _tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;_LL41: {void*t=_tmp29B;void*aout=_tmp29C;
return({struct _tuple12 _tmp3F7;({void*_tmp4F4=({void*_tmp4F3=aout;Cyc_Vcgen_and(_tmp4F3,({void*_tmp4F2=t;Cyc_Vcgen_neq(_tmp4F2,Cyc_Vcgen_zero());}));});_tmp3F7.f1=_tmp4F4;}),({void*_tmp4F1=({void*_tmp4F0=aout;Cyc_Vcgen_and(_tmp4F0,({void*_tmp4EF=t;Cyc_Vcgen_eq(_tmp4EF,Cyc_Vcgen_zero());}));});_tmp3F7.f2=_tmp4F1;});_tmp3F7;});}}}_LL0:;}
# 1841
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(0;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}
# 1847
static struct _tuple19 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(0;es != 0;es=es->tl){
struct _tuple12 _tmp29D=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple12 _stmttmp34=_tmp29D;struct _tuple12 _tmp29E=_stmttmp34;void*_tmp2A0;void*_tmp29F;_LL1: _tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;_LL2: {void*t=_tmp29F;void*aout=_tmp2A0;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->hd=t,_tmp2A1->tl=res;_tmp2A1;});else{
# 1856
valid=0;}}}
# 1859
return({struct _tuple19 _tmp3F8;valid?({struct Cyc_List_List**_tmp4F6=({struct Cyc_List_List**_tmp2A2=_cycalloc(sizeof(*_tmp2A2));({struct Cyc_List_List*_tmp4F5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);*_tmp2A2=_tmp4F5;});_tmp2A2;});_tmp3F8.f1=_tmp4F6;}):(_tmp3F8.f1=0),_tmp3F8.f2=a;_tmp3F8;});}
# 1862
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple12 _tmp2A3=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp35=_tmp2A3;struct _tuple12 _tmp2A4=_stmttmp35;void*_tmp2A6;void*_tmp2A5;_LL1: _tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;_LL2: {void*topt=_tmp2A5;void*a=_tmp2A6;
return({struct _tuple12 _tmp3F9;topt == 0?({void*_tmp4F7=Cyc_Vcgen_fresh_var();_tmp3F9.f1=_tmp4F7;}):(_tmp3F9.f1=topt),_tmp3F9.f2=a;_tmp3F9;});}}
# 1867
static struct _tuple17 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(0;es != 0;es=es->tl){
struct _tuple12 _tmp2A7=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple12 _stmttmp36=_tmp2A7;struct _tuple12 _tmp2A8=_stmttmp36;void*_tmp2AA;void*_tmp2A9;_LL1: _tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_LL2: {void*t=_tmp2A9;void*a=_tmp2AA;
ain=a;
res=({struct Cyc_List_List*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->hd=t,_tmp2AB->tl=res;_tmp2AB;});}}
# 1874
return({struct _tuple17 _tmp3FA;_tmp3FA.f1=res,_tmp3FA.f2=ain;_tmp3FA;});}
# 1877
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_tmp2AC=b;struct Cyc_Absyn_Vardecl*_tmp2AD;struct Cyc_Absyn_Vardecl*_tmp2AE;struct Cyc_Absyn_Vardecl*_tmp2AF;struct Cyc_Absyn_Fndecl*_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2B1;switch(*((int*)_tmp2AC)){case 1U: _LL1: _tmp2B1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp2AC)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp2B1;
return vd;}case 2U: _LL3: _tmp2B0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp2AC)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp2B0;
return(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);}case 0U: _LL5: _LL6:
({void*_tmp2B2=0U;({struct _fat_ptr _tmp4F8=({const char*_tmp2B3="vcgen bind2vardecl";_tag_fat(_tmp2B3,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp4F8,_tag_fat(_tmp2B2,sizeof(void*),0U));});});case 3U: _LL7: _tmp2AF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp2AC)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp2AF;
return vd;}case 4U: _LL9: _tmp2AE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp2AC)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp2AE;
return vd;}default: _LLB: _tmp2AD=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp2AC)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp2AD;
return vd;}}_LL0:;}
# 1888
static struct _tuple12 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp2B4=e->r;void*_stmttmp37=_tmp2B4;void*_tmp2B5=_stmttmp37;void*_tmp2B6;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2B5)->tag == 1U){_LL1: _tmp2B6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2B5)->f1;_LL2: {void*b=_tmp2B6;
# 1891
return({struct _tuple12 _tmp3FB;({void*_tmp4FA=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp4F9=Cyc_Vcgen_bind2vardecl(b);_tmp2B7->f1=_tmp4F9;});_tmp2B7;});_tmp3FB.f1=_tmp4FA;}),_tmp3FB.f2=ain;_tmp3FB;});}}else{_LL3: _LL4:
 return({struct _tuple12 _tmp3FC;_tmp3FC.f1=0,({void*_tmp4FB=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp3FC.f2=_tmp4FB;});_tmp3FC;});}_LL0:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 1898
static struct _tuple21 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple21 _tmp3FD;_tmp3FD.f1=fi->args,_tmp3FD.f2=fi->requires_clause,_tmp3FD.f3=fi->ensures_clause,_tmp3FD.f4=fi->return_value;_tmp3FD;});}
# 1902
static struct _tuple21 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmp2B8=t;struct Cyc_Absyn_FnInfo*_tmp2B9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B8)->tag == 5U){_LL1: _tmp2B9=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B8)->f1;_LL2: {struct Cyc_Absyn_FnInfo*fi=_tmp2B9;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1910
return({struct _tuple21 _tmp3FE;_tmp3FE.f1=0,_tmp3FE.f2=0,_tmp3FE.f3=0,_tmp3FE.f4=0;_tmp3FE;});}
# 1913
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 1916
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 1920
for(0;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple8*_tmp2BA=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp38=_tmp2BA;struct _tuple8*_tmp2BB=_stmttmp38;void*_tmp2BE;struct Cyc_Absyn_Tqual _tmp2BD;struct _fat_ptr*_tmp2BC;_LL1: _tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;_tmp2BE=_tmp2BB->f3;_LL2: {struct _fat_ptr*vopt=_tmp2BC;struct Cyc_Absyn_Tqual tq=_tmp2BD;void*t=_tmp2BE;
if(vopt != 0){
# 1924
struct _tuple0*qv=({struct _tuple0*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2->f1=Cyc_Absyn_Loc_n,_tmp2C2->f2=vopt;_tmp2C2;});
struct Cyc_Absyn_Vardecl*_tmp2BF=({struct Cyc_Absyn_Vardecl*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->sc=Cyc_Absyn_Public,_tmp2C1->name=qv,_tmp2C1->varloc=0U,_tmp2C1->tq=tq,_tmp2C1->type=t,_tmp2C1->initializer=0,_tmp2C1->rgn=0,_tmp2C1->attributes=0,_tmp2C1->escapes=0,_tmp2C1->is_proto=0;_tmp2C1;});struct Cyc_Absyn_Vardecl*vd=_tmp2BF;
# 1930
a=({void*_tmp4FD=a;Cyc_Vcgen_and(_tmp4FD,({void*_tmp4FC=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->tag=1U,_tmp2C0->f1=vd;_tmp2C0;});Cyc_Vcgen_eq(_tmp4FC,(void*)inputs->hd);}));});}}}{
# 1934
struct _tuple12 _tmp2C3=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_Vcgen_true_assn);struct _tuple12 _stmttmp39=_tmp2C3;struct _tuple12 _tmp2C4=_stmttmp39;void*_tmp2C5;_LL4: _tmp2C5=_tmp2C4.f1;_LL5: {void*at=_tmp2C5;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp2C8=({struct Cyc_String_pa_PrintArg_struct _tmp400;_tmp400.tag=0U,({
struct _fat_ptr _tmp4FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(at)));_tmp400.f1=_tmp4FE;});_tmp400;});struct Cyc_String_pa_PrintArg_struct _tmp2C9=({struct Cyc_String_pa_PrintArg_struct _tmp3FF;_tmp3FF.tag=0U,({struct _fat_ptr _tmp4FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(a)));_tmp3FF.f1=_tmp4FF;});_tmp3FF;});void*_tmp2C6[2U];_tmp2C6[0]=& _tmp2C8,_tmp2C6[1]=& _tmp2C9;({unsigned _tmp501=req->loc;struct _fat_ptr _tmp500=({const char*_tmp2C7="cannot prove @requires %s from %s";_tag_fat(_tmp2C7,sizeof(char),34U);});Cyc_Warn_err(_tmp501,_tmp500,_tag_fat(_tmp2C6,sizeof(void*),2U));});});}}}}
# 1941
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1945
static struct _tuple12 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e))return({struct _tuple12 _tmp401;({void*_tmp502=Cyc_Vcgen_cnst(e);_tmp401.f1=_tmp502;}),_tmp401.f2=ain;_tmp401;});{
void*_tmp2CA=e->r;void*_stmttmp3A=_tmp2CA;void*_tmp2CB=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp2CC;struct _fat_ptr _tmp2CD;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Stmt*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_MallocInfo _tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Vardecl*_tmp2D4;struct Cyc_List_List*_tmp2D7;struct Cyc_List_List*_tmp2D8;struct Cyc_List_List*_tmp2D9;struct Cyc_List_List*_tmp2DA;struct Cyc_List_List*_tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_List_List*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E5;enum Cyc_Absyn_Coercion _tmp2E9;struct Cyc_Absyn_Exp*_tmp2E8;void*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2ED;struct Cyc_Absyn_Exp*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2EF;struct Cyc_Absyn_Exp*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F3;struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F5;enum Cyc_Absyn_Incrementor _tmp2F9;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Core_Opt*_tmp2FB;struct Cyc_Absyn_Exp*_tmp2FA;struct Cyc_List_List*_tmp2FE;enum Cyc_Absyn_Primop _tmp2FD;void*_tmp2FF;union Cyc_Absyn_Cnst _tmp300;switch(*((int*)_tmp2CB)){case 0U: _LL1: _tmp300=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp300;
# 1950
return({struct _tuple12 _tmp402;({void*_tmp503=(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->tag=0U,_tmp301->f1=e;_tmp301;});_tmp402.f1=_tmp503;}),_tmp402.f2=ain;_tmp402;});}case 1U: _LL3: _tmp2FF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL4: {void*b=_tmp2FF;
return({struct _tuple12 _tmp403;({void*_tmp505=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp504=Cyc_Vcgen_bind2vardecl(b);_tmp302->f1=_tmp504;});_tmp302;});_tmp403.f1=_tmp505;}),_tmp403.f2=ain;_tmp403;});}case 3U: _LL5: _tmp2FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2FE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL6: {enum Cyc_Absyn_Primop p=_tmp2FD;struct Cyc_List_List*es=_tmp2FE;
# 1953
struct _tuple19 _tmp303=Cyc_Vcgen_vcgen_rexps(env,es,ain);struct _tuple19 _stmttmp3B=_tmp303;struct _tuple19 _tmp304=_stmttmp3B;void*_tmp306;struct Cyc_List_List**_tmp305;_LL58: _tmp305=_tmp304.f1;_tmp306=_tmp304.f2;_LL59: {struct Cyc_List_List**ts=_tmp305;void*aout=_tmp306;
void*t=0;
if(ts != 0)t=Cyc_Vcgen_primop(p,*ts);
return({struct _tuple12 _tmp404;_tmp404.f1=t,_tmp404.f2=aout;_tmp404;});}}case 4U: _LL7: _tmp2FA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2FB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_tmp2FC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2CB)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp2FA;struct Cyc_Core_Opt*popt=_tmp2FB;struct Cyc_Absyn_Exp*e2=_tmp2FC;
# 1958
struct _tuple12 _tmp307=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple12 _stmttmp3C=_tmp307;struct _tuple12 _tmp308=_stmttmp3C;void*_tmp30A;void*_tmp309;_LL5B: _tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;_LL5C: {void*lt=_tmp309;void*a1=_tmp30A;
struct _tuple12 _tmp30B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp3D=_tmp30B;struct _tuple12 _tmp30C=_stmttmp3D;void*_tmp30E;void*_tmp30D;_LL5E: _tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;_LL5F: {void*t2=_tmp30D;void*a2=_tmp30E;
if(popt != 0){
void*_tmp30F=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp30F;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 1965
t2=({enum Cyc_Absyn_Primop _tmp506=(enum Cyc_Absyn_Primop)popt->v;Cyc_Vcgen_primop(_tmp506,({void*_tmp310[2U];_tmp310[0]=t1,_tmp310[1]=t2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp310,sizeof(void*),2U));}));});}}{
# 1967
void*_tmp311=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp311;
return({struct _tuple12 _tmp405;_tmp405.f1=lt,_tmp405.f2=a;_tmp405;});}}}}case 5U: _LL9: _tmp2F8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2F9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp2F8;enum Cyc_Absyn_Incrementor i=_tmp2F9;
# 1970
struct _tuple12 _tmp312=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple12 _stmttmp3E=_tmp312;struct _tuple12 _tmp313=_stmttmp3E;void*_tmp315;void*_tmp314;_LL61: _tmp314=_tmp313.f1;_tmp315=_tmp313.f2;_LL62: {void*lt=_tmp314;void*a1=_tmp315;
void*_tmp316=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp316;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp317=i;switch(_tmp317){case Cyc_Absyn_PreInc: _LL64: _LL65:
# 1976
 if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Plus,({void*_tmp318[2U];_tmp318[0]=t1,({void*_tmp507=Cyc_Vcgen_one();_tmp318[1]=_tmp507;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp318,sizeof(void*),2U));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostInc: _LL66: _LL67:
# 1980
 res=t1;
if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Plus,({void*_tmp319[2U];_tmp319[0]=t1,({void*_tmp508=Cyc_Vcgen_one();_tmp319[1]=_tmp508;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp319,sizeof(void*),2U));}));
goto _LL63;case Cyc_Absyn_PreDec: _LL68: _LL69:
# 1984
 if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Minus,({void*_tmp31A[2U];_tmp31A[0]=t1,({void*_tmp509=Cyc_Vcgen_one();_tmp31A[1]=_tmp509;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp31A,sizeof(void*),2U));}));
res=assn;
goto _LL63;case Cyc_Absyn_PostDec: _LL6A: _LL6B:
# 1988
 res=t1;
if(t1 != 0)assn=Cyc_Vcgen_primop(Cyc_Absyn_Minus,({void*_tmp31B[2U];_tmp31B[0]=t1,({void*_tmp50A=Cyc_Vcgen_one();_tmp31B[1]=_tmp50A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp31B,sizeof(void*),2U));}));
goto _LL63;default: _LL6C: _LL6D:
({void*_tmp31C=0U;({struct _fat_ptr _tmp50B=({const char*_tmp31D="vcgen vcgen_rexp increment_e";_tag_fat(_tmp31D,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp50B,_tag_fat(_tmp31C,sizeof(void*),0U));});});}_LL63:;}{
# 1993
void*_tmp31E=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp31E;
return({struct _tuple12 _tmp406;_tmp406.f1=res,_tmp406.f2=a;_tmp406;});}}}case 6U: _LLB: _tmp2F5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2F6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_tmp2F7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2CB)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp2F5;struct Cyc_Absyn_Exp*e2=_tmp2F6;struct Cyc_Absyn_Exp*e3=_tmp2F7;
# 1996
struct _tuple12 _tmp31F=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp3F=_tmp31F;struct _tuple12 _tmp320=_stmttmp3F;void*_tmp322;void*_tmp321;_LL6F: _tmp321=_tmp320.f1;_tmp322=_tmp320.f2;_LL70: {void*at=_tmp321;void*af=_tmp322;
struct _tuple12 _tmp323=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp40=_tmp323;struct _tuple12 _tmp324=_stmttmp40;void*_tmp326;void*_tmp325;_LL72: _tmp325=_tmp324.f1;_tmp326=_tmp324.f2;_LL73: {void*t1=_tmp325;void*at=_tmp326;
struct _tuple12 _tmp327=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple12 _stmttmp41=_tmp327;struct _tuple12 _tmp328=_stmttmp41;void*_tmp32A;void*_tmp329;_LL75: _tmp329=_tmp328.f1;_tmp32A=_tmp328.f2;_LL76: {void*t2=_tmp329;void*af=_tmp32A;
if(t1 != 0 && t2 != 0){
void*_tmp32B=Cyc_Vcgen_fresh_var();void*v=_tmp32B;
at=({void*_tmp50C=at;Cyc_Vcgen_and(_tmp50C,Cyc_Vcgen_eq(v,t1));});
af=({void*_tmp50D=af;Cyc_Vcgen_and(_tmp50D,Cyc_Vcgen_eq(v,t2));});
return({struct _tuple12 _tmp407;_tmp407.f1=v,({void*_tmp50E=Cyc_Vcgen_or(at,af);_tmp407.f2=_tmp50E;});_tmp407;});}else{
# 2005
return({struct _tuple12 _tmp408;_tmp408.f1=0,({void*_tmp50F=Cyc_Vcgen_or(at,af);_tmp408.f2=_tmp50F;});_tmp408;});}}}}}case 7U: _LLD: _tmp2F3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2F4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp2F3;struct Cyc_Absyn_Exp*e2=_tmp2F4;
# 2007
struct _tuple12 _tmp32C=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp42=_tmp32C;struct _tuple12 _tmp32D=_stmttmp42;void*_tmp32F;void*_tmp32E;_LL78: _tmp32E=_tmp32D.f1;_tmp32F=_tmp32D.f2;_LL79: {void*at=_tmp32E;void*af=_tmp32F;
struct _tuple12 _tmp330=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp43=_tmp330;struct _tuple12 _tmp331=_stmttmp43;void*_tmp333;void*_tmp332;_LL7B: _tmp332=_tmp331.f1;_tmp333=_tmp331.f2;_LL7C: {void*t=_tmp332;void*at=_tmp333;
void*_tmp334=Cyc_Vcgen_fresh_var();void*v=_tmp334;
at=({void*_tmp510=at;Cyc_Vcgen_and(_tmp510,Cyc_Vcgen_eq(v,t));});
af=({void*_tmp512=af;Cyc_Vcgen_and(_tmp512,({void*_tmp511=v;Cyc_Vcgen_eq(_tmp511,Cyc_Vcgen_zero());}));});
return({struct _tuple12 _tmp409;_tmp409.f1=v,({void*_tmp513=Cyc_Vcgen_or(at,af);_tmp409.f2=_tmp513;});_tmp409;});}}}case 8U: _LLF: _tmp2F1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2F2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp2F1;struct Cyc_Absyn_Exp*e2=_tmp2F2;
# 2014
struct _tuple12 _tmp335=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple12 _stmttmp44=_tmp335;struct _tuple12 _tmp336=_stmttmp44;void*_tmp338;void*_tmp337;_LL7E: _tmp337=_tmp336.f1;_tmp338=_tmp336.f2;_LL7F: {void*at=_tmp337;void*af=_tmp338;
struct _tuple12 _tmp339=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple12 _stmttmp45=_tmp339;struct _tuple12 _tmp33A=_stmttmp45;void*_tmp33C;void*_tmp33B;_LL81: _tmp33B=_tmp33A.f1;_tmp33C=_tmp33A.f2;_LL82: {void*t=_tmp33B;void*af=_tmp33C;
void*_tmp33D=Cyc_Vcgen_fresh_var();void*v=_tmp33D;
at=({void*_tmp515=at;Cyc_Vcgen_and(_tmp515,({void*_tmp514=v;Cyc_Vcgen_eq(_tmp514,Cyc_Vcgen_one());}));});
af=({void*_tmp516=af;Cyc_Vcgen_and(_tmp516,Cyc_Vcgen_eq(v,t));});
return({struct _tuple12 _tmp40A;_tmp40A.f1=v,({void*_tmp517=Cyc_Vcgen_or(at,af);_tmp40A.f2=_tmp517;});_tmp40A;});}}}case 9U: _LL11: _tmp2EF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2F0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp2EF;struct Cyc_Absyn_Exp*e2=_tmp2F0;
# 2021
struct _tuple12 _tmp33E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp46=_tmp33E;struct _tuple12 _tmp33F=_stmttmp46;void*_tmp340;_LL84: _tmp340=_tmp33F.f2;_LL85: {void*a=_tmp340;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10U: _LL13: _tmp2ED=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2EE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmp2ED;struct Cyc_List_List*es=_tmp2EE;
# 2024
struct _tuple17 _tmp341=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple17 _stmttmp47=_tmp341;struct _tuple17 _tmp342=_stmttmp47;void*_tmp344;struct Cyc_List_List*_tmp343;_LL87: _tmp343=_tmp342.f1;_tmp344=_tmp342.f2;_LL88: {struct Cyc_List_List*ts=_tmp343;void*a=_tmp344;
struct _tuple12 _tmp345=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple12 _stmttmp48=_tmp345;struct _tuple12 _tmp346=_stmttmp48;void*_tmp347;_LL8A: _tmp347=_tmp346.f2;_LL8B: {void*a2=_tmp347;
# 2027
struct _tuple21 _tmp348=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple21 _stmttmp49=_tmp348;struct _tuple21 _tmp349=_stmttmp49;struct Cyc_Absyn_Vardecl*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_List_List*_tmp34A;_LL8D: _tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;_tmp34C=_tmp349.f3;_tmp34D=_tmp349.f4;_LL8E: {struct Cyc_List_List*args=_tmp34A;struct Cyc_Absyn_Exp*reqopt=_tmp34B;struct Cyc_Absyn_Exp*ensopt=_tmp34C;struct Cyc_Absyn_Vardecl*ret_value=_tmp34D;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 2031
a2=Cyc_Vcgen_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 2034
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_Vcgen_false_assn;
if(ensopt == 0)
return({struct _tuple12 _tmp40B;_tmp40B.f1=0,_tmp40B.f2=a2;_tmp40B;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 2039
struct _tuple12 _tmp34E=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple12 _stmttmp4A=_tmp34E;struct _tuple12 _tmp34F=_stmttmp4A;void*_tmp350;_LL90: _tmp350=_tmp34F.f1;_LL91: {void*at=_tmp350;
if(ret_value != 0){
void*_tmp351=Cyc_Vcgen_fresh_var();void*v=_tmp351;
# 2043
at=Cyc_Vcgen_subst(ret_value,v,at);
return({struct _tuple12 _tmp40C;_tmp40C.f1=v,_tmp40C.f2=at;_tmp40C;});}
# 2046
return({struct _tuple12 _tmp40D;_tmp40D.f1=0,_tmp40D.f2=at;_tmp40D;});}}}}}}case 11U: _LL15: _tmp2EC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp2EC;
# 2048
struct _tuple12 _tmp352=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp4B=_tmp352;struct _tuple12 _tmp353=_stmttmp4B;void*_tmp354;_LL93: _tmp354=_tmp353.f2;_LL94: {void*a=_tmp354;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple12 _tmp40E;_tmp40E.f1=0,_tmp40E.f2=(void*)& Cyc_Vcgen_false_assn;_tmp40E;});}}case 12U: _LL17: _tmp2EB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp2EB;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13U: _LL19: _tmp2EA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp2EA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14U: _LL1B: _tmp2E7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_tmp2E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2CB)->f4;_LL1C: {void*tp=_tmp2E7;struct Cyc_Absyn_Exp*e=_tmp2E8;enum Cyc_Absyn_Coercion c=_tmp2E9;
# 2056
struct _tuple12 _tmp355=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple12 _stmttmp4C=_tmp355;struct _tuple12 _tmp356=_stmttmp4C;void*_tmp358;void*_tmp357;_LL96: _tmp357=_tmp356.f1;_tmp358=_tmp356.f2;_LL97: {void*topt=_tmp357;void*a=_tmp358;
if((int)c == (int)1U){
if(topt == 0)return({struct _tuple12 _tmp40F;_tmp40F.f1=0,_tmp40F.f2=a;_tmp40F;});{
void*t=topt;
# 2066
{struct _tuple12 _tmp359=({struct _tuple12 _tmp411;({void*_tmp519=Cyc_Absyn_compress(tp);_tmp411.f1=_tmp519;}),({void*_tmp518=Cyc_Absyn_compress((void*)_check_null(e->topt));_tmp411.f2=_tmp518;});_tmp411;});struct _tuple12 _stmttmp4D=_tmp359;struct _tuple12 _tmp35A=_stmttmp4D;enum Cyc_Absyn_Size_of _tmp35E;enum Cyc_Absyn_Sign _tmp35D;enum Cyc_Absyn_Size_of _tmp35C;enum Cyc_Absyn_Sign _tmp35B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->tag == 1U){_LL99: _tmp35B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->f1;_tmp35C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->f2;_tmp35D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->f1;_tmp35E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->f2;_LL9A: {enum Cyc_Absyn_Sign u1=_tmp35B;enum Cyc_Absyn_Size_of s1=_tmp35C;enum Cyc_Absyn_Sign u2=_tmp35D;enum Cyc_Absyn_Size_of s2=_tmp35E;
# 2068
if((int)s1 == (int)s2)return({struct _tuple12 _tmp410;_tmp410.f1=t,_tmp410.f2=a;_tmp410;});
goto _LL98;}}else{goto _LL9B;}}else{goto _LL9B;}}else{goto _LL9B;}}else{_LL9B: _LL9C:
 goto _LL98;}_LL98:;}
# 2072
return({struct _tuple12 _tmp412;({void*_tmp51A=Cyc_Vcgen_cast(tp,t);_tmp412.f1=_tmp51A;}),_tmp412.f2=a;_tmp412;});}}else{
# 2075
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple12 _tmp413;topt == 0?_tmp413.f1=0:({void*_tmp51B=Cyc_Vcgen_cast(tp,topt);_tmp413.f1=_tmp51B;}),_tmp413.f2=a;_tmp413;});}}}case 16U: _LL1D: _tmp2E5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2E6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL1E: {struct Cyc_Absyn_Exp*eopt=_tmp2E5;struct Cyc_Absyn_Exp*e=_tmp2E6;
# 2079
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple12 _tmp414;_tmp414.f1=0,({void*_tmp51C=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp414.f2=_tmp51C;});_tmp414;});}case 32U: _LL1F: _LL20:
# 2084
 goto _LL22;case 33U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 19U: _LL29: _LL2A:
 return({struct _tuple12 _tmp415;_tmp415.f1=0,_tmp415.f2=ain;_tmp415;});case 20U: _LL2B: _tmp2E4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp2E4;
# 2092
_tmp2E3=e;goto _LL2E;}case 21U: _LL2D: _tmp2E3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp2E3;
_tmp2E2=e;goto _LL30;}case 22U: _LL2F: _tmp2E2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp2E2;
# 2095
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple12 _tmp416;_tmp416.f1=0,_tmp416.f2=ain;_tmp416;});}case 38U: _LL31: _tmp2E1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp2E1;
# 2099
_tmp2E0=e;goto _LL34;}case 15U: _LL33: _tmp2E0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL34: {struct Cyc_Absyn_Exp*e=_tmp2E0;
# 2101
return({struct _tuple12 _tmp417;_tmp417.f1=0,({void*_tmp51D=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp417.f2=_tmp51D;});_tmp417;});}case 23U: _LL35: _tmp2DE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2DF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp2DE;struct Cyc_Absyn_Exp*e2=_tmp2DF;
# 2104
struct _tuple12 _tmp35F=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp4E=_tmp35F;struct _tuple12 _tmp360=_stmttmp4E;void*_tmp362;void*_tmp361;_LL9E: _tmp361=_tmp360.f1;_tmp362=_tmp360.f2;_LL9F: {void*t1=_tmp361;void*a1=_tmp362;
struct _tuple12 _tmp363=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple12 _stmttmp4F=_tmp363;struct _tuple12 _tmp364=_stmttmp4F;void*_tmp366;void*_tmp365;_LLA1: _tmp365=_tmp364.f1;_tmp366=_tmp364.f2;_LLA2: {void*t2=_tmp365;void*a2=_tmp366;
{void*_tmp367=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp50=_tmp367;void*_tmp368=_stmttmp50;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp368)->tag == 6U){_LLA4: _LLA5:
 goto _LLA3;}else{_LLA6: _LLA7:
# 2109
 if(t1 != 0){
Cyc_Vcgen_update_try_assn(env,ain);
a2=({void*_tmp51F=a2;Cyc_Vcgen_and(_tmp51F,({void*_tmp51E=t2;Cyc_Vcgen_ult(_tmp51E,Cyc_Vcgen_primop(Cyc_Absyn_Numelts,({void*_tmp369[1U];_tmp369[0]=t1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp369,sizeof(void*),1U));})));}));});}}_LLA3:;}
# 2114
return({struct _tuple12 _tmp418;_tmp418.f1=0,_tmp418.f2=a2;_tmp418;});}}}case 31U: _LL37: _tmp2DD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL38: {struct Cyc_List_List*es=_tmp2DD;
# 2116
_tmp2DC=es;goto _LL3A;}case 24U: _LL39: _tmp2DC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL3A: {struct Cyc_List_List*es=_tmp2DC;
return({struct _tuple12 _tmp419;_tmp419.f1=0,({void*_tmp520=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp419.f2=_tmp520;});_tmp419;});}case 25U: _LL3B: _tmp2DB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp2DB;
# 2119
_tmp2DA=dles;goto _LL3E;}case 26U: _LL3D: _tmp2DA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL3E: {struct Cyc_List_List*dles=_tmp2DA;
_tmp2D9=dles;goto _LL40;}case 29U: _LL3F: _tmp2D9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2CB)->f3;_LL40: {struct Cyc_List_List*dles=_tmp2D9;
_tmp2D8=dles;goto _LL42;}case 36U: _LL41: _tmp2D8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL42: {struct Cyc_List_List*dles=_tmp2D8;
_tmp2D7=dles;goto _LL44;}case 30U: _LL43: _tmp2D7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL44: {struct Cyc_List_List*dles=_tmp2D7;
# 2124
for(0;dles != 0;dles=dles->tl){
struct _tuple22*_tmp36A=(struct _tuple22*)dles->hd;struct _tuple22*_stmttmp51=_tmp36A;struct _tuple22*_tmp36B=_stmttmp51;struct Cyc_Absyn_Exp*_tmp36C;_LLA9: _tmp36C=_tmp36B->f2;_LLAA: {struct Cyc_Absyn_Exp*e=_tmp36C;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 2128
return({struct _tuple12 _tmp41A;_tmp41A.f1=0,_tmp41A.f2=ain;_tmp41A;});}case 27U: _LL45: _tmp2D4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2D5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_tmp2D6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2CB)->f3;_LL46: {struct Cyc_Absyn_Vardecl*vd=_tmp2D4;struct Cyc_Absyn_Exp*e1=_tmp2D5;struct Cyc_Absyn_Exp*e2=_tmp2D6;
# 2132
struct _tuple12 _tmp36D=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple12 _stmttmp52=_tmp36D;struct _tuple12 _tmp36E=_stmttmp52;void*_tmp370;void*_tmp36F;_LLAC: _tmp36F=_tmp36E.f1;_tmp370=_tmp36E.f2;_LLAD: {void*t1=_tmp36F;void*a=_tmp370;
void*_tmp371=t1 == 0?Cyc_Vcgen_fresh_var(): t1;{void*t1=_tmp371;
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp372=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->tag=1U,_tmp379->f1=vd;_tmp379;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*v=_tmp372;
# 2136
a=({struct Cyc_Vcgen_Env*_tmp523=env;void*_tmp522=a;void*_tmp521=(void*)v;Cyc_Vcgen_do_assign(_tmp523,_tmp522,_tmp521,Cyc_Vcgen_zero());});
while(1){
void*_tmp373=({void*_tmp524=a;Cyc_Vcgen_and(_tmp524,Cyc_Vcgen_ult((void*)v,t1));});void*at=_tmp373;
void*_tmp374=({void*_tmp525=a;Cyc_Vcgen_and(_tmp525,Cyc_Vcgen_ulte(t1,(void*)v));});void*af=_tmp374;
struct _tuple12 _tmp375=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple12 _stmttmp53=_tmp375;struct _tuple12 _tmp376=_stmttmp53;void*_tmp377;_LLAF: _tmp377=_tmp376.f2;_LLB0: {void*abody=_tmp377;
abody=({struct Cyc_Vcgen_Env*_tmp529=env;void*_tmp528=abody;void*_tmp527=(void*)v;Cyc_Vcgen_do_assign(_tmp529,_tmp528,_tmp527,Cyc_Vcgen_primop(Cyc_Absyn_Plus,({void*_tmp378[2U];_tmp378[0]=(void*)v,({void*_tmp526=Cyc_Vcgen_one();_tmp378[1]=_tmp526;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp378,sizeof(void*),2U));})));});
if(Cyc_Vcgen_implies(& abody,& a)){a=af;break;}
a=Cyc_Vcgen_weaken_assn(abody,a);}}
# 2145
return({struct _tuple12 _tmp41B;_tmp41B.f1=0,_tmp41B.f2=a;_tmp41B;});}}}case 28U: _LL47: _tmp2D3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL48: {struct Cyc_Absyn_Exp*e=_tmp2D3;
# 2147
return({struct _tuple12 _tmp41C;_tmp41C.f1=0,({void*_tmp52A=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp41C.f2=_tmp52A;});_tmp41C;});}case 34U: _LL49: _tmp2D2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL4A: {struct Cyc_Absyn_MallocInfo mi=_tmp2D2;
# 2149
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple12 _tmp41D;_tmp41D.f1=0,({void*_tmp52B=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp41D.f2=_tmp52B;});_tmp41D;});}case 35U: _LL4B: _tmp2D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_tmp2D1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmp2D0;struct Cyc_Absyn_Exp*e2=_tmp2D1;
# 2153
struct _tuple12 _tmp37A=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple12 _stmttmp54=_tmp37A;struct _tuple12 _tmp37B=_stmttmp54;void*_tmp37D;void*_tmp37C;_LLB2: _tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;_LLB3: {void*lt1=_tmp37C;void*a1=_tmp37D;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple12 _tmp37E=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple12 _stmttmp55=_tmp37E;struct _tuple12 _tmp37F=_stmttmp55;void*_tmp381;void*_tmp380;_LLB5: _tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;_LLB6: {void*lt2=_tmp380;void*a2=_tmp381;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp382=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp382;
void*_tmp383=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp383;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple12 _tmp41E;_tmp41E.f1=0,_tmp41E.f2=a2;_tmp41E;});}}}}}case 37U: _LL4D: _tmp2CF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL4E: {struct Cyc_Absyn_Stmt*s=_tmp2CF;
# 2163
void**_tmp384=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp384;
({void**_tmp52C=({void**_tmp385=_cycalloc(sizeof(*_tmp385));*_tmp385=0;_tmp385;});(env->shared)->exp_stmt=_tmp52C;});{
void*_tmp386=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp386;
void*_tmp387=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp387;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple12 _tmp41F;_tmp41F.f1=t,_tmp41F.f2=aout;_tmp41F;});}}case 40U: _LL4F: _LL50:
 return({struct _tuple12 _tmp420;_tmp420.f1=0,_tmp420.f2=ain;_tmp420;});case 41U: _LL51: _tmp2CE=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL52: {struct Cyc_Absyn_Exp*e=_tmp2CE;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2U: _LL53: _tmp2CD=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL54: {struct _fat_ptr s=_tmp2CD;
# 2172
if(({struct _fat_ptr _tmp52D=(struct _fat_ptr)s;Cyc_strcmp(_tmp52D,({const char*_tmp388="print_assn";_tag_fat(_tmp388,sizeof(char),11U);}));})){
struct _fat_ptr _tmp389=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp389;
({struct Cyc_String_pa_PrintArg_struct _tmp38C=({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0U,_tmp422.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp422;});struct Cyc_String_pa_PrintArg_struct _tmp38D=({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0U,({struct _fat_ptr _tmp52E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(ain));_tmp421.f1=_tmp52E;});_tmp421;});void*_tmp38A[2U];_tmp38A[0]=& _tmp38C,_tmp38A[1]=& _tmp38D;({struct Cyc___cycFILE*_tmp530=Cyc_stderr;struct _fat_ptr _tmp52F=({const char*_tmp38B="%s: current assn is %s\n";_tag_fat(_tmp38B,sizeof(char),24U);});Cyc_fprintf(_tmp530,_tmp52F,_tag_fat(_tmp38A,sizeof(void*),2U));});});}
# 2176
return({struct _tuple12 _tmp423;({void*_tmp531=Cyc_Vcgen_zero();_tmp423.f1=_tmp531;}),_tmp423.f2=ain;_tmp423;});}default: _LL55: _tmp2CC=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL56: {struct Cyc_Absyn_Exp*e1=_tmp2CC;
# 2178
struct _tuple12 _tmp38E=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_Vcgen_true_assn);struct _tuple12 _stmttmp56=_tmp38E;struct _tuple12 _tmp38F=_stmttmp56;void*_tmp390;_LLB8: _tmp390=_tmp38F.f1;_LLB9: {void*at=_tmp390;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0U,({
struct _fat_ptr _tmp532=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(ain)));_tmp424.f1=_tmp532;});_tmp424;});void*_tmp391[1U];_tmp391[0]=& _tmp393;({unsigned _tmp534=e->loc;struct _fat_ptr _tmp533=({const char*_tmp392="cannot prove @assert from %s";_tag_fat(_tmp392,sizeof(char),29U);});Cyc_Warn_err(_tmp534,_tmp533,_tag_fat(_tmp391,sizeof(void*),1U));});});
return({struct _tuple12 _tmp425;({void*_tmp536=Cyc_Vcgen_one();_tmp425.f1=_tmp536;}),({void*_tmp535=Cyc_Vcgen_and(ain,at);_tmp425.f2=_tmp535;});_tmp425;});}}}_LL0:;}}
# 2187
void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp394=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_stmttmp57=_tmp394;void*_tmp395=_stmttmp57;struct Cyc_List_List*_tmp396;struct Cyc_List_List*_tmp397;struct Cyc_Absyn_Fndecl*_tmp398;switch(*((int*)_tmp395)){case 1U: _LL1: _tmp398=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp395)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp398;
# 2193
struct Cyc_Vcgen_Env*_tmp399=Cyc_Vcgen_initial_env(tables,fd);struct Cyc_Vcgen_Env*env=_tmp399;
void*a=(void*)& Cyc_Vcgen_true_assn;
# 2196
struct _tuple21 _tmp39A=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple21 _stmttmp58=_tmp39A;struct _tuple21 _tmp39B=_stmttmp58;struct Cyc_Absyn_Vardecl*_tmp39F;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_List_List*_tmp39C;_LLA: _tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;_tmp39E=_tmp39B.f3;_tmp39F=_tmp39B.f4;_LLB: {struct Cyc_List_List*args=_tmp39C;struct Cyc_Absyn_Exp*reqopt=_tmp39D;struct Cyc_Absyn_Exp*ensopt=_tmp39E;struct Cyc_Absyn_Vardecl*ret_value=_tmp39F;
if(reqopt != 0)
# 2199
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 2201
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 2203
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple12 _tmp3A0=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_Vcgen_true_assn);struct _tuple12 _stmttmp59=_tmp3A0;struct _tuple12 _tmp3A1=_stmttmp59;void*_tmp3A2;_LLD: _tmp3A2=_tmp3A1.f1;_LLE: {void*at=_tmp3A2;
struct Cyc_Vcgen_SharedEnv*_tmp3A3=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp3A3;
void*_tmp3A4=senv->res_assn;void*res_assn_opt=_tmp3A4;
void*res_assn=(void*)& Cyc_Vcgen_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp3AA="NULL";_tag_fat(_tmp3AA,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_Vcgen_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp538=res_assn;Cyc_Vcgen_and(_tmp538,({void*_tmp537=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=1U,_tmp3A5->f1=ret_value;_tmp3A5;});Cyc_Vcgen_eq(_tmp537,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp3A8=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0U,({
struct _fat_ptr _tmp539=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(at)));_tmp427.f1=_tmp539;});_tmp427;});struct Cyc_String_pa_PrintArg_struct _tmp3A9=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U,({struct _fat_ptr _tmp53A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(Cyc_Vcgen_reduce(res_assn)));_tmp426.f1=_tmp53A;});_tmp426;});void*_tmp3A6[2U];_tmp3A6[0]=& _tmp3A8,_tmp3A6[1]=& _tmp3A9;({unsigned _tmp53C=ens->loc;struct _fat_ptr _tmp53B=({const char*_tmp3A7="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp3A7,sizeof(char),44U);});Cyc_Warn_err(_tmp53C,_tmp53B,_tag_fat(_tmp3A6,sizeof(void*),2U));});});}}}
# 2218
Cyc_Vcgen_hash_cons_table=0;
goto _LL0;}}case 9U: _LL3: _tmp397=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp395)->f2;_LL4: {struct Cyc_List_List*ds=_tmp397;
_tmp396=ds;goto _LL6;}case 10U: _LL5: _tmp396=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp395)->f2;_LL6: {struct Cyc_List_List*ds=_tmp396;
Cyc_Vcgen_vcgen_topdecls(tables,ds);goto _LL0;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
