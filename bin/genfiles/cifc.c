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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 349
extern struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*,void*);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 857 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 894
extern void*Cyc_Absyn_void_type;
# 935
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1061
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1106
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);
# 1114
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 34 "cifc.h"
void Cyc_Cifc_merge_sys_user_decl(unsigned,struct Cyc_Tcenv_Tenv*,int,struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*);
# 30 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 82 "attributes.h"
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 41 "cifc.cyc"
static int Cyc_Cifc_glob_tvar_id=0;
int Cyc_Cifc_opt_inst_tvar=0;
# 44
void Cyc_Cifc_set_inst_tvar (void){
Cyc_Cifc_opt_inst_tvar=1;}
# 48
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){
struct _fat_ptr _tmp0=({void*_tmp8=0U;({struct _fat_ptr _tmp17E=({const char*_tmp9="(";_tag_fat(_tmp9,sizeof(char),2U);});Cyc_aprintf(_tmp17E,_tag_fat(_tmp8,sizeof(void*),0));});});struct _fat_ptr ret=_tmp0;
while((unsigned)l){
ret=({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0,_tmp153.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp153;});struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0,({struct _fat_ptr _tmp17F=(struct _fat_ptr)((struct _fat_ptr)((struct _fat_ptr(*)(void*))_check_null(tostr))(l->hd));_tmp152.f1=_tmp17F;});_tmp152;});void*_tmp1[2];_tmp1[0]=& _tmp3,_tmp1[1]=& _tmp4;({struct _fat_ptr _tmp180=({const char*_tmp2="%s %s,";_tag_fat(_tmp2,sizeof(char),7U);});Cyc_aprintf(_tmp180,_tag_fat(_tmp1,sizeof(void*),2));});});
l=l->tl;}
# 54
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0,_tmp154.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_tmp154;});void*_tmp5[1];_tmp5[0]=& _tmp7;({struct _fat_ptr _tmp181=({const char*_tmp6="%s)";_tag_fat(_tmp6,sizeof(char),4U);});Cyc_aprintf(_tmp181,_tag_fat(_tmp5,sizeof(void*),1));});});}
# 59
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){
# 61
if(is_buildlib){
struct _fat_ptr preamble=warn?({const char*_tmpF="Warning: user-defined";_tag_fat(_tmpF,sizeof(char),22U);}):({const char*_tmp10="User-defined";_tag_fat(_tmp10,sizeof(char),13U);});
({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0,_tmp157.f1=(struct _fat_ptr)((struct _fat_ptr)preamble);_tmp157;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0,({
# 65
struct _fat_ptr _tmp182=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_tmp156.f1=_tmp182;});_tmp156;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0,_tmp155.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp155;});void*_tmpA[3];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD,_tmpA[2]=& _tmpE;({struct Cyc___cycFILE*_tmp184=Cyc_stderr;struct _fat_ptr _tmp183=({const char*_tmpB="%s type for %s incompatible with system version %s\n";_tag_fat(_tmpB,sizeof(char),52U);});Cyc_fprintf(_tmp184,_tmp183,_tag_fat(_tmpA,sizeof(void*),3));});});}else{
# 67
if(warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B;_tmp15B.tag=0,({struct _fat_ptr _tmp185=({const char*_tmp17="User-defined type for ";_tag_fat(_tmp17,sizeof(char),23U);});_tmp15B.f1=_tmp185;});_tmp15B;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp13=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp15A;_tmp15A.tag=1,_tmp15A.f1=n;_tmp15A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp159;_tmp159.tag=0,({struct _fat_ptr _tmp186=({const char*_tmp16=" incompatible with system version ";_tag_fat(_tmp16,sizeof(char),35U);});_tmp159.f1=_tmp186;});_tmp159;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp158;_tmp158.tag=0,_tmp158.f1=s;_tmp158;});void*_tmp11[4];_tmp11[0]=& _tmp12,_tmp11[1]=& _tmp13,_tmp11[2]=& _tmp14,_tmp11[3]=& _tmp15;Cyc_Warn_warn2(loc,_tag_fat(_tmp11,sizeof(void*),4));});else{
# 70
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15F;_tmp15F.tag=0,({struct _fat_ptr _tmp187=({const char*_tmp1E="User-defined type for ";_tag_fat(_tmp1E,sizeof(char),23U);});_tmp15F.f1=_tmp187;});_tmp15F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp15E;_tmp15E.tag=1,_tmp15E.f1=n;_tmp15E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D;_tmp15D.tag=0,({struct _fat_ptr _tmp188=({const char*_tmp1D=" incompatible with system version ";_tag_fat(_tmp1D,sizeof(char),35U);});_tmp15D.f1=_tmp188;});_tmp15D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15C;_tmp15C.tag=0,_tmp15C.f1=s;_tmp15C;});void*_tmp18[4];_tmp18[0]=& _tmp19,_tmp18[1]=& _tmp1A,_tmp18[2]=& _tmp1B,_tmp18[3]=& _tmp1C;Cyc_Warn_err2(loc,_tag_fat(_tmp18,sizeof(void*),4));});}}}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 75
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){
# 77
void*_tmp1F=ctyp;void*_tmp21;enum Cyc_Absyn_AggrKind _tmp22;void*_tmp20;switch(*((int*)_tmp1F)){case 8:
# 80
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
{void*_tmp23=ctyp;void*_tmp24;if(*((int*)_tmp23)== 8){_tmp24=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23)->f4;{void*to=_tmp24;
# 84
return(void*)_check_null(to);}}else{
# 86
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26=({struct Cyc_Warn_String_Warn_Warg_struct _tmp160;_tmp160.tag=0,({struct _fat_ptr _tmp189=({const char*_tmp27="Impos";_tag_fat(_tmp27,sizeof(char),6U);});_tmp160.f1=_tmp189;});_tmp160;});void*_tmp25[1];_tmp25[0]=& _tmp26;Cyc_Warn_err2(loc,_tag_fat(_tmp25,sizeof(void*),1));});
goto _LL19;}_LL19:;}
# 89
return ctyp;case 5: _tmp20=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_FnInfo*finfo=_tmp20;
# 91
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 93
({void*_tmp18C=({unsigned _tmp18B=loc;struct Cyc_Tcenv_Tenv*_tmp18A=te;Cyc_Cifc_expand_c_type(_tmp18B,_tmp18A,Cyc_Toc_typ_to_c(finfo->ret_type));});finfo->ret_type=_tmp18C;});{
struct Cyc_List_List*_tmp28=finfo->args;struct Cyc_List_List*args=_tmp28;
while((unsigned)args){
struct _tuple8*_tmp29=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp0=_tmp29;struct _tuple8*_tmp2A=_stmttmp0;void*_tmp2B;_tmp2B=(void**)& _tmp2A->f3;{void**argType=(void**)_tmp2B;
({void*_tmp18F=({unsigned _tmp18E=loc;struct Cyc_Tcenv_Tenv*_tmp18D=te;Cyc_Cifc_expand_c_type(_tmp18E,_tmp18D,Cyc_Toc_typ_to_c(*argType));});*argType=_tmp18F;});
args=args->tl;}}
# 101
finfo->tvars=0;
finfo->effect=0;
return ctyp;}}case 3: _tmp20=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_PtrInfo*pinfo=_tmp20;
# 107
({void*_tmp192=({unsigned _tmp191=loc;struct Cyc_Tcenv_Tenv*_tmp190=te;Cyc_Cifc_expand_c_type(_tmp191,_tmp190,Cyc_Toc_typ_to_c(pinfo->elt_type));});pinfo->elt_type=_tmp192;});
return ctyp;}case 4: _tmp20=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_ArrayInfo*ainfo=_tmp20;
# 110
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 112
({void*_tmp195=({unsigned _tmp194=loc;struct Cyc_Tcenv_Tenv*_tmp193=te;Cyc_Cifc_expand_c_type(_tmp194,_tmp193,Cyc_Toc_typ_to_c(ainfo->elt_type));});ainfo->elt_type=_tmp195;});
return ctyp;}case 6: _tmp20=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_List_List*tlst=_tmp20;
# 115
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 117
while((unsigned)tlst){
struct _tuple11*_tmp2C=(struct _tuple11*)tlst->hd;struct _tuple11*_stmttmp1=_tmp2C;struct _tuple11*_tmp2D=_stmttmp1;void*_tmp2F;struct Cyc_Absyn_Tqual _tmp2E;_tmp2E=_tmp2D->f1;_tmp2F=(void**)& _tmp2D->f2;{struct Cyc_Absyn_Tqual tq=_tmp2E;void**elt_type=(void**)_tmp2F;
({void*_tmp198=({unsigned _tmp197=loc;struct Cyc_Tcenv_Tenv*_tmp196=te;Cyc_Cifc_expand_c_type(_tmp197,_tmp196,Cyc_Toc_typ_to_c(*elt_type));});*elt_type=_tmp198;});
tlst=tlst->tl;}}
# 123
return ctyp;}case 7: _tmp22=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f1;_tmp20=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F)->f2;{enum Cyc_Absyn_AggrKind knd=_tmp22;struct Cyc_List_List*flst=_tmp20;
# 126
return ctyp;}case 10: _tmp20=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f1;_tmp21=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1F)->f2;{struct Cyc_Absyn_TypeDecl*td=_tmp20;void**tptr=_tmp21;
# 129
return ctyp;}case 9: _tmp20=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_Exp*e=_tmp20;
# 132
return ctyp;}case 11: _tmp20=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1F)->f1;{struct Cyc_Absyn_Exp*e=_tmp20;
# 135
return ctyp;}case 0:
# 138
 return ctyp;case 1:
# 141
 return ctyp;default:
# 144
 return ctyp;};}
# 148
static int Cyc_Cifc_is_boxed_kind(void*t){
# 150
void*_tmp30=t;void*_tmp32;void*_tmp31;switch(*((int*)_tmp30)){case 0: _tmp31=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1;_tmp32=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f2;{void*tc=_tmp31;struct Cyc_List_List*ts=_tmp32;
# 152
{void*_tmp33=tc;enum Cyc_Absyn_Size_of _tmp34;switch(*((int*)_tmp33)){case 1: _tmp34=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp33)->f2;{enum Cyc_Absyn_Size_of sz=_tmp34;
# 154
return(int)sz == 2 ||(int)sz == 3;}case 3:
 goto _LLF;case 4: _LLF:
# 157
 return 1;default:
# 159
 return 0;};}
# 161
goto _LL0;}case 3: _tmp31=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30)->f1;{struct Cyc_Absyn_PtrInfo*pi=_tmp31;
# 163
{void*_tmp35=(pi->ptr_atts).bounds;void*_stmttmp2=_tmp35;void*_tmp36=_stmttmp2;void*_tmp37;if(*((int*)_tmp36)== 0){_tmp37=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36)->f1;{void*tc=_tmp37;
# 165
void*_tmp38=tc;if(*((int*)_tmp38)== 14)
# 167
return 0;else{
# 169
return 1;};}}else{
# 172
({void*_tmp39=0U;({int(*_tmp19A)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp199=({const char*_tmp3A="ptrbound_t must have an AppType";_tag_fat(_tmp3A,sizeof(char),32U);});_tmp19A(_tmp199,_tag_fat(_tmp39,sizeof(void*),0));});});};}
# 174
goto _LL0;}case 9: _tmp31=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp30)->f1;{struct Cyc_Absyn_Exp*e=_tmp31;
# 176
return 1;}default:
# 178
 return 0;}_LL0:;}
# 183
static int Cyc_Cifc_check_fntype_kinds(unsigned,struct Cyc_Tcenv_Tenv*,void*,void*);
# 189
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){
struct Cyc_Absyn_Tqual tq;
# 192
void*_tmp3B=Cyc_Tcutil_copy_type(ctyp);void*ctyp_cp=_tmp3B;
void*_tmp3C=Cyc_Tcutil_copy_type(cyctyp);void*utyp_cp=_tmp3C;
# 195
void*_tmp3D=Cyc_Toc_typ_to_c(ctyp_cp);void*c_typ=_tmp3D;
void*_tmp3E=Cyc_Toc_typ_to_c(utyp_cp);void*u_typ=_tmp3E;
# 200
if(!Cyc_Unify_unify(c_typ,u_typ)){
c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);
if(!Cyc_Unify_unify(c_typ,u_typ)){
# 206
if(Cyc_Tcutil_is_function_type(c_typ))
return Cyc_Cifc_check_fntype_kinds(loc,te,c_typ,u_typ);
return Cyc_Cifc_is_boxed_kind(c_typ)&& Cyc_Cifc_is_boxed_kind(u_typ);}}
# 211
return 1;}struct _tuple12{void*f1;void*f2;};
# 223 "cifc.cyc"
static int Cyc_Cifc_check_fntype_kinds(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*c_typ,void*u_typ){
struct _tuple12 _tmp3F=({struct _tuple12 _tmp161;_tmp161.f1=c_typ,_tmp161.f2=u_typ;_tmp161;});struct _tuple12 _stmttmp3=_tmp3F;struct _tuple12 _tmp40=_stmttmp3;struct Cyc_Absyn_FnInfo _tmp42;struct Cyc_Absyn_FnInfo _tmp41;if(*((int*)_tmp40.f1)== 5){if(*((int*)_tmp40.f2)== 5){_tmp41=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40.f1)->f1;_tmp42=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40.f2)->f1;{struct Cyc_Absyn_FnInfo cfi=_tmp41;struct Cyc_Absyn_FnInfo ufi=_tmp42;
# 226
int typesOk=Cyc_Cifc_c_types_ok(loc,te,cfi.ret_type,ufi.ret_type);
if(typesOk){
struct Cyc_List_List*_tmp43=cfi.args;struct Cyc_List_List*ca=_tmp43;
struct Cyc_List_List*_tmp44=ufi.args;struct Cyc_List_List*ua=_tmp44;
while((unsigned)ca && typesOk){
if(!((unsigned)ua))
return 0;
typesOk=Cyc_Cifc_c_types_ok(loc,te,(*((struct _tuple8*)ca->hd)).f3,(*((struct _tuple8*)ua->hd)).f3);
# 235
ca=ca->tl;
ua=ua->tl;}}
# 239
return typesOk;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
# 242
 return 0;}_LL0:;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 249
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 251
struct _tuple12 _tmp45=({struct _tuple12 _tmp172;_tmp172.f1=c_decl->r,_tmp172.f2=user_decl->r;_tmp172;});struct _tuple12 _stmttmp4=_tmp45;struct _tuple12 _tmp46=_stmttmp4;void*_tmp48;void*_tmp47;switch(*((int*)_tmp46.f1)){case 0: if(*((int*)_tmp46.f2)== 0){_tmp47=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp46.f1)->f1;_tmp48=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp46.f2)->f1;{struct Cyc_Absyn_Vardecl*cd=_tmp47;struct Cyc_Absyn_Vardecl*ud=_tmp48;
# 253
if(!Cyc_Cifc_c_types_ok(loc,te,cd->type,ud->type)){
({unsigned _tmp1A0=loc;int _tmp19F=is_buildlib;struct _tuple0*_tmp19E=cd->name;Cyc_Cifc_fail_merge(0,_tmp1A0,_tmp19F,_tmp19E,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4B=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0,({struct _fat_ptr _tmp19B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp163.f1=_tmp19B;});_tmp163;});struct Cyc_String_pa_PrintArg_struct _tmp4C=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0,({struct _fat_ptr _tmp19C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cd->type));_tmp162.f1=_tmp19C;});_tmp162;});void*_tmp49[2];_tmp49[0]=& _tmp4B,_tmp49[1]=& _tmp4C;({struct _fat_ptr _tmp19D=({const char*_tmp4A=": type %s != %s";_tag_fat(_tmp4A,sizeof(char),16U);});Cyc_aprintf(_tmp19D,_tag_fat(_tmp49,sizeof(void*),2));});}));});if(!0)return;}else{
# 261
if(ud->attributes != 0)
({struct Cyc_List_List*_tmp1A1=Cyc_Atts_merge_attributes(cd->attributes,ud->attributes);cd->attributes=_tmp1A1;});
cd->type=ud->type;
cd->tq=ud->tq;}
# 266
goto _LL0;}}else{goto _LLB;}case 1: if(*((int*)_tmp46.f2)== 0){_tmp47=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp46.f1)->f1;_tmp48=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp46.f2)->f1;{struct Cyc_Absyn_Fndecl*cd=_tmp47;struct Cyc_Absyn_Vardecl*ud=_tmp48;
# 269
if(!Cyc_Tcutil_is_function_type(ud->type)){
({unsigned _tmp1A4=loc;int _tmp1A3=is_buildlib;struct _tuple0*_tmp1A2=ud->name;Cyc_Cifc_fail_merge(0,_tmp1A4,_tmp1A3,_tmp1A2,({const char*_tmp4D=": type must be a function type to match decl\n";_tag_fat(_tmp4D,sizeof(char),46U);}));});if(!0)return;}{
# 272
void*cdtype;
if(cd->cached_type != 0)
cdtype=(void*)_check_null(cd->cached_type);else{
# 276
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=5,_tmp4E->f1=cd->i;_tmp4E;});}
if(!Cyc_Cifc_c_types_ok(loc,te,cdtype,ud->type)){
({unsigned _tmp1AA=loc;int _tmp1A9=is_buildlib;struct _tuple0*_tmp1A8=ud->name;Cyc_Cifc_fail_merge(0,_tmp1AA,_tmp1A9,_tmp1A8,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0,({struct _fat_ptr _tmp1A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cdtype));_tmp165.f1=_tmp1A5;});_tmp165;});struct Cyc_String_pa_PrintArg_struct _tmp52=({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0,({struct _fat_ptr _tmp1A6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_tmp164.f1=_tmp1A6;});_tmp164;});void*_tmp4F[2];_tmp4F[0]=& _tmp51,_tmp4F[1]=& _tmp52;({struct _fat_ptr _tmp1A7=({const char*_tmp50=": type %s != %s";_tag_fat(_tmp50,sizeof(char),16U);});Cyc_aprintf(_tmp1A7,_tag_fat(_tmp4F,sizeof(void*),2));});}));});if(!0)return;}else{
# 285
void*_tmp53=ud->type;void*_stmttmp5=_tmp53;void*_tmp54=_stmttmp5;struct Cyc_Absyn_FnInfo _tmp55;if(*((int*)_tmp54)== 5){_tmp55=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp54)->f1;{struct Cyc_Absyn_FnInfo fi=_tmp55;
# 287
struct Cyc_List_List*_tmp56=fi.tvars;struct Cyc_List_List*old_tvars=_tmp56;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ud->type);
if((cd->i).attributes != 0)
({struct Cyc_List_List*_tmp1AB=Cyc_Atts_merge_attributes(fi.attributes,(cd->i).attributes);fi.attributes=_tmp1AB;});
cd->i=fi;
goto _LLD;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58=({struct Cyc_Warn_String_Warn_Warg_struct _tmp166;_tmp166.tag=0,({struct _fat_ptr _tmp1AC=({const char*_tmp59="oops!\n";_tag_fat(_tmp59,sizeof(char),7U);});_tmp166.f1=_tmp1AC;});_tmp166;});void*_tmp57[1];_tmp57[0]=& _tmp58;Cyc_Warn_err2(0U,_tag_fat(_tmp57,sizeof(void*),1));});}_LLD:;}
# 296
goto _LL0;}}}else{goto _LLB;}case 5: if(*((int*)_tmp46.f2)== 5){_tmp47=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp46.f1)->f1;_tmp48=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp46.f2)->f1;{struct Cyc_Absyn_Aggrdecl*cd=_tmp47;struct Cyc_Absyn_Aggrdecl*ud=_tmp48;
# 304
if((int)ud->sc != (int)cd->sc){
({unsigned _tmp1AF=loc;int _tmp1AE=is_buildlib;struct _tuple0*_tmp1AD=cd->name;Cyc_Cifc_fail_merge(1,_tmp1AF,_tmp1AE,_tmp1AD,({const char*_tmp5A=": scopes don't match (ignoring)";_tag_fat(_tmp5A,sizeof(char),32U);}));});if(!1)return;}
# 307
if(ud->impl == 0){
({unsigned _tmp1B2=loc;int _tmp1B1=is_buildlib;struct _tuple0*_tmp1B0=cd->name;Cyc_Cifc_fail_merge(0,_tmp1B2,_tmp1B1,_tmp1B0,({const char*_tmp5B=": no user definition";_tag_fat(_tmp5B,sizeof(char),21U);}));});if(!0)return;}
if(cd->impl == 0){
({unsigned _tmp1B5=loc;int _tmp1B4=is_buildlib;struct _tuple0*_tmp1B3=cd->name;Cyc_Cifc_fail_merge(1,_tmp1B5,_tmp1B4,_tmp1B3,({const char*_tmp5C=": no definition for system version";_tag_fat(_tmp5C,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 314
{struct _tuple13 _tmp5D=({struct _tuple13 _tmp16C;_tmp16C.f1=cd->impl,_tmp16C.f2=ud->impl;_tmp16C;});struct _tuple13 _stmttmp6=_tmp5D;struct _tuple13 _tmp5E=_stmttmp6;int _tmp65;void*_tmp64;void*_tmp63;void*_tmp62;void*_tmp61;void*_tmp60;void*_tmp5F;if(_tmp5E.f1 != 0){if(_tmp5E.f2 != 0){_tmp5F=(struct Cyc_List_List**)&(_tmp5E.f1)->exist_vars;_tmp60=(struct Cyc_List_List**)&(_tmp5E.f1)->rgn_po;_tmp61=(_tmp5E.f1)->fields;_tmp62=(_tmp5E.f2)->exist_vars;_tmp63=(_tmp5E.f2)->rgn_po;_tmp64=(_tmp5E.f2)->fields;_tmp65=(_tmp5E.f2)->tagged;{struct Cyc_List_List**tvarsC=(struct Cyc_List_List**)_tmp5F;struct Cyc_List_List**rgnpoC=(struct Cyc_List_List**)_tmp60;struct Cyc_List_List*cfields=_tmp61;struct Cyc_List_List*tvars=_tmp62;struct Cyc_List_List*rgnpo=_tmp63;struct Cyc_List_List*ufields=_tmp64;int tagged=_tmp65;
# 317
if(tagged){
({unsigned _tmp1B8=loc;int _tmp1B7=is_buildlib;struct _tuple0*_tmp1B6=cd->name;Cyc_Cifc_fail_merge(0,_tmp1B8,_tmp1B7,_tmp1B6,({const char*_tmp66=": user @tagged annotation not allowed (ignoring)";_tag_fat(_tmp66,sizeof(char),49U);}));});if(!0)return;}{
# 320
struct Cyc_List_List*_tmp67=cfields;struct Cyc_List_List*x=_tmp67;
# 322
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp68=(struct Cyc_Absyn_Aggrfield*)x->hd;struct Cyc_Absyn_Aggrfield*cfield=_tmp68;
struct Cyc_Absyn_Aggrfield*_tmp69=Cyc_Absyn_lookup_field(ufields,cfield->name);struct Cyc_Absyn_Aggrfield*ufield=_tmp69;
# 326
if(ufield != 0){
# 329
if(!Cyc_Cifc_c_types_ok(loc,te,cfield->type,ufield->type)){
({unsigned _tmp1BE=loc;int _tmp1BD=is_buildlib;struct _tuple0*_tmp1BC=cd->name;Cyc_Cifc_fail_merge(0,_tmp1BE,_tmp1BD,_tmp1BC,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp169;_tmp169.tag=0,({struct _fat_ptr _tmp1B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp169.f1=_tmp1B9;});_tmp169;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp168;_tmp168.tag=0,_tmp168.f1=(struct _fat_ptr)((struct _fat_ptr)*cfield->name);_tmp168;});struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp167;_tmp167.tag=0,({struct _fat_ptr _tmp1BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cfield->type));_tmp167.f1=_tmp1BA;});_tmp167;});void*_tmp6A[3];_tmp6A[0]=& _tmp6C,_tmp6A[1]=& _tmp6D,_tmp6A[2]=& _tmp6E;({struct _fat_ptr _tmp1BB=({const char*_tmp6B=": type %s of user definition of field %s != %s";_tag_fat(_tmp6B,sizeof(char),47U);});Cyc_aprintf(_tmp1BB,_tag_fat(_tmp6A,sizeof(void*),3));});}));});if(!0)return;}else{
# 338
if(ufield->width != 0){
({unsigned _tmp1C3=loc;int _tmp1C2=is_buildlib;struct _tuple0*_tmp1C1=cd->name;Cyc_Cifc_fail_merge(1,_tmp1C3,_tmp1C2,_tmp1C1,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0,({struct _fat_ptr _tmp1BF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_tmp16A.f1=_tmp1BF;});_tmp16A;});void*_tmp6F[1];_tmp6F[0]=& _tmp71;({struct _fat_ptr _tmp1C0=({const char*_tmp70=": ignoring width of user definition of field %s";_tag_fat(_tmp70,sizeof(char),48U);});Cyc_aprintf(_tmp1C0,_tag_fat(_tmp6F,sizeof(void*),1));});}));});if(!1)return;}
# 343
if(ufield->attributes != 0)
({struct Cyc_List_List*_tmp1C4=Cyc_Atts_merge_attributes(cfield->attributes,ufield->attributes);cfield->attributes=_tmp1C4;});
# 347
cfield->type=ufield->type;
cfield->tq=ufield->tq;
cfield->requires_clause=ufield->requires_clause;}}
# 352
x=x->tl;}
# 356
if(ud->tvs != 0)cd->tvs=ud->tvs;
if((unsigned)tvars)*tvarsC=tvars;
if((unsigned)rgnpo)*rgnpoC=rgnpo;
# 361
x=ufields;{
int missing_fields=0;
while(x != 0){
struct Cyc_Absyn_Aggrfield*_tmp72=Cyc_Absyn_lookup_field(cfields,((struct Cyc_Absyn_Aggrfield*)x->hd)->name);struct Cyc_Absyn_Aggrfield*cfield=_tmp72;
if(cfield == 0){
missing_fields=1;
({unsigned _tmp1C8=loc;int _tmp1C7=is_buildlib;struct _tuple0*_tmp1C6=cd->name;Cyc_Cifc_fail_merge(1,_tmp1C8,_tmp1C7,_tmp1C6,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0,_tmp16B.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)x->hd)->name);_tmp16B;});void*_tmp73[1];_tmp73[0]=& _tmp75;({struct _fat_ptr _tmp1C5=({const char*_tmp74=": no definition of field %s in system version";_tag_fat(_tmp74,sizeof(char),46U);});Cyc_aprintf(_tmp1C5,_tag_fat(_tmp73,sizeof(void*),1));});}));});if(!1)return;}
# 371
x=x->tl;}
# 373
goto _LL12;}}}}else{goto _LL15;}}else{_LL15:
# 375
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp1C9=({const char*_tmp76="Internal Error: NULL cases not possible";_tag_fat(_tmp76,sizeof(char),40U);});_tmp77->f1=_tmp1C9;});_tmp77;}));}_LL12:;}
# 377
goto _LL0;}}else{goto _LLB;}case 7: if(*((int*)_tmp46.f2)== 7){_tmp47=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp46.f1)->f1;_tmp48=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp46.f2)->f1;{struct Cyc_Absyn_Enumdecl*cd=_tmp47;struct Cyc_Absyn_Enumdecl*ud=_tmp48;
# 380
({unsigned _tmp1CC=loc;int _tmp1CB=is_buildlib;struct _tuple0*_tmp1CA=cd->name;Cyc_Cifc_fail_merge(0,_tmp1CC,_tmp1CB,_tmp1CA,({const char*_tmp78=": enum merging not currently supported";_tag_fat(_tmp78,sizeof(char),39U);}));});if(!0)return;}}else{goto _LLB;}case 8: if(*((int*)_tmp46.f2)== 8){_tmp47=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp46.f1)->f1;_tmp48=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp46.f2)->f1;{struct Cyc_Absyn_Typedefdecl*cd=_tmp47;struct Cyc_Absyn_Typedefdecl*ud=_tmp48;
# 383
if(ud->defn == 0){
({unsigned _tmp1CF=loc;int _tmp1CE=is_buildlib;struct _tuple0*_tmp1CD=cd->name;Cyc_Cifc_fail_merge(0,_tmp1CF,_tmp1CE,_tmp1CD,({const char*_tmp79=": no user definition";_tag_fat(_tmp79,sizeof(char),21U);}));});if(!0)return;}
if(cd->defn == 0){
({unsigned _tmp1D2=loc;int _tmp1D1=is_buildlib;struct _tuple0*_tmp1D0=cd->name;Cyc_Cifc_fail_merge(1,_tmp1D2,_tmp1D1,_tmp1D0,({const char*_tmp7A=": no definition for system version";_tag_fat(_tmp7A,sizeof(char),35U);}));});if(!1)return;
c_decl->r=user_decl->r;
return;}
# 391
if(!({unsigned _tmp1D5=loc;struct Cyc_Tcenv_Tenv*_tmp1D4=te;void*_tmp1D3=(void*)_check_null(cd->defn);Cyc_Cifc_c_types_ok(_tmp1D5,_tmp1D4,_tmp1D3,(void*)_check_null(ud->defn));})){
({unsigned _tmp1DB=loc;int _tmp1DA=is_buildlib;struct _tuple0*_tmp1D9=cd->name;Cyc_Cifc_fail_merge(0,_tmp1DB,_tmp1DA,_tmp1D9,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7D=({struct Cyc_String_pa_PrintArg_struct _tmp16E;_tmp16E.tag=0,({struct _fat_ptr _tmp1D6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(ud->defn)));_tmp16E.f1=_tmp1D6;});_tmp16E;});struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp16D;_tmp16D.tag=0,({struct _fat_ptr _tmp1D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(cd->defn)));_tmp16D.f1=_tmp1D7;});_tmp16D;});void*_tmp7B[2];_tmp7B[0]=& _tmp7D,_tmp7B[1]=& _tmp7E;({struct _fat_ptr _tmp1D8=({const char*_tmp7C=": type definition %s of user definition != %s";_tag_fat(_tmp7C,sizeof(char),46U);});Cyc_aprintf(_tmp1D8,_tag_fat(_tmp7B,sizeof(void*),2));});}));});if(!0)return;}else{
# 398
cd->tvs=ud->tvs;
cd->defn=ud->defn;
cd->tq=ud->tq;
if(ud->atts != 0)
({struct Cyc_List_List*_tmp1DC=Cyc_Atts_merge_attributes(cd->atts,ud->atts);cd->atts=_tmp1DC;});}
# 405
goto _LL0;}}else{goto _LLB;}default: _LLB:
# 408
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _tmp81=({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0,({
struct _fat_ptr _tmp1DD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp82[1];_tmp82[0]=user_decl;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp82,sizeof(struct Cyc_Absyn_Decl*),1));})));_tmp16F.f1=_tmp1DD;});_tmp16F;});void*_tmp7F[1];_tmp7F[0]=& _tmp81;({struct Cyc___cycFILE*_tmp1DF=Cyc_stderr;struct _fat_ptr _tmp1DE=({const char*_tmp80="Error in .cys file: bad (or unsupported) user-defined type %s\n";_tag_fat(_tmp80,sizeof(char),63U);});Cyc_fprintf(_tmp1DF,_tmp1DE,_tag_fat(_tmp7F,sizeof(void*),1));});});else{
# 412
({struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp171;_tmp171.tag=0,({struct _fat_ptr _tmp1E0=({const char*_tmp87="bad (or unsupported) user-defined type %s";_tag_fat(_tmp87,sizeof(char),42U);});_tmp171.f1=_tmp1E0;});_tmp171;});struct Cyc_Warn_String_Warn_Warg_struct _tmp85=({struct Cyc_Warn_String_Warn_Warg_struct _tmp170;_tmp170.tag=0,({
struct _fat_ptr _tmp1E1=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp86[1];_tmp86[0]=user_decl;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp86,sizeof(struct Cyc_Absyn_Decl*),1));}));_tmp170.f1=_tmp1E1;});_tmp170;});void*_tmp83[2];_tmp83[0]=& _tmp84,_tmp83[1]=& _tmp85;Cyc_Warn_err2(loc,_tag_fat(_tmp83,sizeof(void*),2));});}
return;}_LL0:;}
# 418
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){
{void*_tmp88=ud->r;void*_stmttmp7=_tmp88;void*_tmp89=_stmttmp7;void*_tmp8A;if(*((int*)_tmp89)== 5){_tmp8A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp89)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp8A;
# 421
return ad->tvs != 0;}}else{
# 423
goto _LL0;}_LL0:;}
# 425
return 0;}
# 428
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){
void*_tmp8B=ud->r;void*_stmttmp8=_tmp8B;void*_tmp8C=_stmttmp8;void*_tmp8D;if(*((int*)_tmp8C)== 5){_tmp8D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8C)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp8D;
# 431
struct Cyc_Absyn_Aggrdecl*_tmp8E=({struct Cyc_Absyn_Aggrdecl*_tmp91=_cycalloc(sizeof(*_tmp91));*_tmp91=*ad;_tmp91;});struct Cyc_Absyn_Aggrdecl*absad=_tmp8E;
absad->impl=0;{
struct Cyc_Absyn_Decl*_tmp8F=({void*_tmp1E2=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=5,_tmp90->f1=absad;_tmp90;});Cyc_Absyn_new_decl(_tmp1E2,ud->loc);});struct Cyc_Absyn_Decl*nd=_tmp8F;
return nd;}}}else{
# 436
({void*_tmp92=0U;({int(*_tmp1E4)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp1E3=({const char*_tmp93="Only aggrdecls";_tag_fat(_tmp93,sizeof(char),15U);});_tmp1E4(_tmp1E3,_tag_fat(_tmp92,sizeof(void*),0));});});};}
# 441
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){
void*_tmp94=kb1;void*_tmp96;void*_tmp95;switch(*((int*)_tmp94)){case 0: _tmp95=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;{struct Cyc_Absyn_Kind*k1=_tmp95;
# 444
void*_tmp97=kb2;void*_tmp98;if(*((int*)_tmp97)== 0){_tmp98=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97)->f1;{struct Cyc_Absyn_Kind*k2=_tmp98;
# 446
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{
# 448
return 0;};}case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94)->f1 != 0){_tmp95=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94)->f1)->v;{void*kbb1=_tmp95;
# 451
void*_tmp99=kb2;void*_tmp9A;if(*((int*)_tmp99)== 1){if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99)->f1 != 0){_tmp9A=(void*)(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99)->f1)->v;{void*kbb2=_tmp9A;
# 453
return Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL13;}}else{_LL13:
# 455
 return 1;};}}else{
# 458
return 1;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f1 != 0){_tmp95=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f1)->v;_tmp96=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;{void*kbb1=_tmp95;struct Cyc_Absyn_Kind*k1=_tmp96;
# 460
void*_tmp9B=kb2;void*_tmp9D;void*_tmp9C;if(*((int*)_tmp9B)== 2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->f1 != 0){_tmp9C=(void*)(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->f1)->v;_tmp9D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9B)->f2;{void*kbb2=_tmp9C;struct Cyc_Absyn_Kind*k2=_tmp9D;
# 462
return((int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual)&& Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL18;}}else{_LL18:
# 464
 return 0;};}}else{_tmp95=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;{struct Cyc_Absyn_Kind*k1=_tmp95;
# 467
void*_tmp9E=kb2;void*_tmp9F;if(*((int*)_tmp9E)== 2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f1 == 0){_tmp9F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;{struct Cyc_Absyn_Kind*k2=_tmp9F;
# 469
return(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}}else{goto _LL1D;}}else{_LL1D:
# 471
 return 0;};}}};}
# 476
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){
struct Cyc_List_List*_tmpA0=*lst;struct Cyc_List_List*cur=_tmpA0;
struct Cyc_List_List*prev=0;
while((unsigned)cur){
void*_tmpA1=(void*)cur->hd;void*t=_tmpA1;
{void*_tmpA2=t;void*_tmpA3;if(*((int*)_tmpA2)== 2){_tmpA3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2)->f1;{struct Cyc_Absyn_Tvar*tv=_tmpA3;
# 483
if(Cyc_Cifc_kindbound_subsumed(tv->kind,kind)){
if((unsigned)prev)
prev->tl=cur->tl;else{
# 487
*lst=cur->tl;}
cur->tl=0;
return 1;}
# 491
goto _LL0;}}else{
# 493
({struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0,({struct _fat_ptr _tmp1E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp174.f1=_tmp1E5;});_tmp174;});struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0,({struct _fat_ptr _tmp1E6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_get_type_kind(t));_tmp173.f1=_tmp1E6;});_tmp173;});void*_tmpA4[2];_tmpA4[0]=& _tmpA6,_tmpA4[1]=& _tmpA7;({int(*_tmp1E8)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp1E7=({const char*_tmpA5="expects a VarType list only -- got %s(%s)";_tag_fat(_tmpA5,sizeof(char),42U);});_tmp1E8(_tmp1E7,_tag_fat(_tmpA4,sizeof(void*),2));});});}_LL0:;}
# 495
prev=cur;
cur=cur->tl;}
# 498
return 0;}
# 501
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){
struct Cyc_List_List*ret=0;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpA8=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*can=_tmpA8;
if(!Cyc_Cifc_find_and_remove(& remove,can->kind))
ret=({struct Cyc_List_List*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->hd=can,_tmpA9->tl=ret;_tmpA9;});
tvs=tvs->tl;}
# 509
return ret;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 514
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 516
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){
struct Cyc_List_List*res=0;
while((unsigned)ts){
void*_tmpAA=(void*)ts->hd;void*t=_tmpAA;
{void*_tmpAB=t;void*_tmpAC;if(*((int*)_tmpAB)== 2){_tmpAC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB)->f1;{struct Cyc_Absyn_Tvar*tv=_tmpAC;
# 522
res=({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->hd=tv,_tmpAD->tl=res;_tmpAD;});
goto _LL0;}}else{
# 525
(int)_throw((void*)& Cyc_Cifc_Contains_nontvar_val);}_LL0:;}
# 527
ts=ts->tl;}
# 529
return res;}
# 532
static void*Cyc_Cifc_instantiate_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
{void*_tmpAE=tv->kind;void*_stmttmp9=_tmpAE;void*_tmpAF=_stmttmp9;void*_tmpB0;switch(*((int*)_tmpAF)){case 0: _tmpB0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAF)->f1;{struct Cyc_Absyn_Kind*k=_tmpB0;
_tmpB0=k;goto _LL4;}case 2: _tmpB0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAF)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmpB0;
# 536
if((int)k->kind == 2 ||(int)k->kind == 0)
return({void*_tmp1E9=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_tmp1E9,Cyc_Absyn_empty_tqual(loc));});else{
if((int)k->kind == 3)
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=0,({void*_tmp1EA=(void*)({struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=5;_tmpB1;});_tmpB2->f1=_tmp1EA;}),_tmpB2->f2=0;_tmpB2;});}
goto _LL0;}default:
# 542
 return({void*_tmp1EB=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_tmp1EB,Cyc_Absyn_empty_tqual(loc));});}_LL0:;}
# 544
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp176;_tmp176.tag=0,({struct _fat_ptr _tmp1EC=({const char*_tmpB6="Unable to instantiate tvar ";_tag_fat(_tmpB6,sizeof(char),28U);});_tmp176.f1=_tmp1EC;});_tmp176;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp175;_tmp175.tag=0,({struct _fat_ptr _tmp1ED=Cyc_Absynpp_tvar2string(tv);_tmp175.f1=_tmp1ED;});_tmp175;});void*_tmpB3[2];_tmpB3[0]=& _tmpB4,_tmpB3[1]=& _tmpB5;Cyc_Warn_err2(loc,_tag_fat(_tmpB3,sizeof(void*),2));});
return Cyc_Absyn_void_type;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 554
static struct _tuple14*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t,struct Cyc_Absyn_Decl*enclosing_decl,int instantiate){
# 558
{void*_tmpB7=t;struct Cyc_Absyn_FnInfo _tmpB9;void*_tmpBA;void*_tmpB8;switch(*((int*)_tmpB7)){case 0: _tmpB8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB7)->f1;_tmpBA=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB7)->f2;{void*tc=_tmpB8;struct Cyc_List_List**ts=(struct Cyc_List_List**)_tmpBA;
# 560
{void*_tmpBB=tc;union Cyc_Absyn_AggrInfo _tmpBC;if(*((int*)_tmpBB)== 20){_tmpBC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpBB)->f1;{union Cyc_Absyn_AggrInfo ai=_tmpBC;
# 562
union Cyc_Absyn_AggrInfo _tmpBD=ai;struct _tuple2 _tmpBE;void*_tmpBF;if((_tmpBD.KnownAggr).tag == 2){_tmpBF=*(_tmpBD.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpBF;
# 564
struct _tuple0*_tmpC0=ad->name;struct _tuple0*_stmttmpA=_tmpC0;struct _tuple0*_tmpC1=_stmttmpA;void*_tmpC3;union Cyc_Absyn_Nmspace _tmpC2;_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;{union Cyc_Absyn_Nmspace ns=_tmpC2;struct _fat_ptr*name=_tmpC3;
# 567
struct Cyc_Absyn_Decl*_tmpC4=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmpC4;
if((unsigned)ovd){
# 571
void*_tmpC5=ovd->r;void*_stmttmpB=_tmpC5;void*_tmpC6=_stmttmpB;void*_tmpC7;if(*((int*)_tmpC6)== 5){_tmpC7=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpC7;
# 573
struct Cyc_List_List*removed_tvars=0;
{struct _handler_cons _tmpC8;_push_handler(& _tmpC8);{int _tmpCA=0;if(setjmp(_tmpC8.handler))_tmpCA=1;if(!_tmpCA){
removed_tvars=Cyc_Cifc_extract_tvars(*ts);;_pop_handler();}else{void*_tmpC9=(void*)Cyc_Core_get_exn_thrown();void*_tmpCB=_tmpC9;void*_tmpCC;if(((struct Cyc_Cifc_Contains_nontvar_exn_struct*)_tmpCB)->tag == Cyc_Cifc_Contains_nontvar)
# 580
return 0;else{_tmpCC=_tmpCB;{void*exn=_tmpCC;(int)_rethrow(exn);}};}}}
# 582
*ts=0;{
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmpCD=ad->tvs;struct Cyc_List_List*tvs=_tmpCD;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmpCE=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpCE;
if(enclosing_decl == ovd)
({struct Cyc_List_List*_tmp1EF=({struct Cyc_List_List*_tmpD0=_cycalloc(sizeof(*_tmpD0));({void*_tmp1EE=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=2,_tmpCF->f1=tv;_tmpCF;});_tmpD0->hd=_tmp1EE;}),_tmpD0->tl=*ts;_tmpD0;});*ts=_tmp1EF;});else{
# 591
if(instantiate || Cyc_Cifc_opt_inst_tvar)
# 593
({struct Cyc_List_List*_tmp1F1=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));({void*_tmp1F0=Cyc_Cifc_instantiate_tvar(enclosing_decl->loc,tv);_tmpD1->hd=_tmp1F0;}),_tmpD1->tl=*ts;_tmpD1;});*ts=_tmp1F1;});else{
# 596
struct Cyc_Absyn_Tvar*_tmpD2=({struct Cyc_Absyn_Tvar*_tmpDB=_cycalloc(sizeof(*_tmpDB));*_tmpDB=*tv;_tmpDB;});struct Cyc_Absyn_Tvar*tvcpy=_tmpD2;
struct _fat_ptr*_tmpD3=({struct _fat_ptr*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct _fat_ptr _tmp1F3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD9=({struct Cyc_Int_pa_PrintArg_struct _tmp177;_tmp177.tag=1,_tmp177.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp177;});void*_tmpD7[1];_tmpD7[0]=& _tmpD9;({struct _fat_ptr _tmp1F2=({const char*_tmpD8="`ovr_%d";_tag_fat(_tmpD8,sizeof(char),8U);});Cyc_aprintf(_tmp1F2,_tag_fat(_tmpD7,sizeof(void*),1));});});*_tmpDA=_tmp1F3;});_tmpDA;});struct _fat_ptr*tvn=_tmpD3;
tvcpy->name=tvn;
added_tvars=({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=tvcpy,_tmpD4->tl=added_tvars;_tmpD4;});
({struct Cyc_List_List*_tmp1F5=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({void*_tmp1F4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->tag=2,_tmpD5->f1=tvcpy;_tmpD5;});_tmpD6->hd=_tmp1F4;}),_tmpD6->tl=*ts;_tmpD6;});*ts=_tmp1F5;});}}
# 603
tvs=tvs->tl;}
# 605
({struct Cyc_List_List*_tmp1F6=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(*ts);*ts=_tmp1F6;});
return((enclosing_decl == ovd || Cyc_Cifc_opt_inst_tvar)|| instantiate)?0:({struct _tuple14*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=added_tvars,_tmpDC->f2=removed_tvars;_tmpDC;});}}}else{
# 608
({void*_tmpDD=0U;({int(*_tmp1F8)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp1F7=({const char*_tmpDE="ovd must be an aggr type";_tag_fat(_tmpDE,sizeof(char),25U);});_tmp1F8(_tmp1F7,_tag_fat(_tmpDD,sizeof(void*),0));});});};}else{
# 612
return 0;}}}}else{_tmpBE=(_tmpBD.UnknownAggr).val;{struct _tuple2 ua=_tmpBE;
# 614
return 0;}};}}else{
# 617
goto _LLB;}_LLB:;}
# 619
return 0;}case 3: _tmpB8=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB7)->f1;{struct Cyc_Absyn_PtrInfo*pi=_tmpB8;
# 621
return Cyc_Cifc_update_tvars(te,tv_ovrs,pi->elt_type,enclosing_decl,instantiate);}case 5: _tmpB9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB7)->f1;{struct Cyc_Absyn_FnInfo fi=_tmpB9;
# 623
struct Cyc_List_List*_tmpDF=0;struct Cyc_List_List*added_tvars=_tmpDF;
struct Cyc_List_List*_tmpE0=0;struct Cyc_List_List*removed_tvars=_tmpE0;
if(Cyc_Cifc_opt_inst_tvar)
Cyc_Cifc_update_tvars(te,tv_ovrs,fi.ret_type,enclosing_decl,instantiate);{
# 628
struct Cyc_List_List*_tmpE1=fi.args;struct Cyc_List_List*argit=_tmpE1;
while((unsigned)argit){
struct _tuple8*_tmpE2=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpC=_tmpE2;struct _tuple8*_tmpE3=_stmttmpC;void*_tmpE4;_tmpE4=_tmpE3->f3;{void*at=_tmpE4;
struct _tuple14*_tmpE5=Cyc_Cifc_update_tvars(te,tv_ovrs,at,enclosing_decl,instantiate);struct _tuple14*ar_tup=_tmpE5;
if((unsigned)ar_tup){
if((unsigned)(*ar_tup).f1)
added_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ar_tup).f1,added_tvars);
if((unsigned)(*ar_tup).f2)
removed_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ar_tup).f2,removed_tvars);}
# 638
argit=argit->tl;}}
# 640
if((unsigned)added_tvars ||(unsigned)removed_tvars)
return({struct _tuple14*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->f1=added_tvars,_tmpE6->f2=removed_tvars;_tmpE6;});
return 0;}}case 4: _tmpB8=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB7)->f1;{struct Cyc_Absyn_ArrayInfo*ai=(struct Cyc_Absyn_ArrayInfo*)_tmpB8;
# 644
return Cyc_Cifc_update_tvars(te,tv_ovrs,ai->elt_type,enclosing_decl,instantiate);}default:
# 646
 return 0;};}
# 648
return 0;}
# 651
static int Cyc_Cifc_update_fninfo_usage(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_Absyn_FnInfo*fi,struct Cyc_Absyn_Decl*enclosing_decl){
struct _tuple14*_tmpE7=Cyc_Cifc_update_tvars(te,tv_ovrs,((struct Cyc_Absyn_FnInfo*)_check_null(fi))->ret_type,enclosing_decl,0);struct _tuple14*ad=_tmpE7;
# 654
struct Cyc_List_List*_tmpE8=fi->args;struct Cyc_List_List*argit=_tmpE8;
struct Cyc_List_List*added_tvars=(unsigned)ad?(*ad).f1: 0;
struct Cyc_List_List*removed_tvars=(unsigned)ad?(*ad).f2: 0;
int change=0;
while((unsigned)argit){
struct _tuple8*_tmpE9=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmpD=_tmpE9;struct _tuple8*_tmpEA=_stmttmpD;void*_tmpEB;_tmpEB=(void**)& _tmpEA->f3;{void**at=(void**)_tmpEB;
struct _tuple14*_tmpEC=Cyc_Cifc_update_tvars(te,tv_ovrs,*at,enclosing_decl,0);struct _tuple14*ad=_tmpEC;
if((unsigned)ad){
added_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ad).f1,added_tvars);
removed_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((*ad).f2,removed_tvars);}
# 665
argit=argit->tl;}}
# 667
while((unsigned)removed_tvars){
change=1;{
struct Cyc_Absyn_Tvar*_tmpED=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmpED;
{struct _handler_cons _tmpEE;_push_handler(& _tmpEE);{int _tmpF0=0;if(setjmp(_tmpEE.handler))_tmpF0=1;if(!_tmpF0){
({struct Cyc_List_List*_tmp1F9=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete;})(fi->tvars,rtv);fi->tvars=_tmp1F9;});;_pop_handler();}else{void*_tmpEF=(void*)Cyc_Core_get_exn_thrown();void*_tmpF1=_tmpEF;void*_tmpF2;if(((struct Cyc_Core_Not_found_exn_struct*)_tmpF1)->tag == Cyc_Core_Not_found){
# 675
({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0,({struct _fat_ptr _tmp1FA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp178.f1=_tmp1FA;});_tmp178;});void*_tmpF3[1];_tmpF3[0]=& _tmpF5;({unsigned _tmp1FC=loc;struct _fat_ptr _tmp1FB=({const char*_tmpF4="Removed tvar %s not found";_tag_fat(_tmpF4,sizeof(char),26U);});Cyc_Warn_warn(_tmp1FC,_tmp1FB,_tag_fat(_tmpF3,sizeof(void*),1));});});
goto _LL3;}else{_tmpF2=_tmpF1;{void*exn=_tmpF2;(int)_rethrow(exn);}}_LL3:;}}}
# 678
removed_tvars=removed_tvars->tl;}}
# 680
if((unsigned)added_tvars){
change=1;
({struct Cyc_List_List*_tmp1FD=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(added_tvars,fi->tvars);fi->tvars=_tmp1FD;});}
# 685
fi->effect=0;
return change;}
# 689
static struct Cyc_List_List*Cyc_Cifc_remove_cur(struct Cyc_List_List*head,struct Cyc_List_List*cur,struct Cyc_List_List*prev){
if((unsigned)prev)
prev->tl=((struct Cyc_List_List*)_check_null(cur))->tl;else{
# 693
head=((struct Cyc_List_List*)_check_null(cur))->tl;}
# 695
return head;}
# 698
static int Cyc_Cifc_decl_name_cmp(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b){
struct _fat_ptr*_tmpF6=Cyc_Absyn_decl_name(a);struct _fat_ptr*na=_tmpF6;
struct _fat_ptr*_tmpF7=Cyc_Absyn_decl_name(b);struct _fat_ptr*nb=_tmpF7;
if(na == 0 ^ nb == 0)
return 1;
if((unsigned)na)
return Cyc_strcmp((struct _fat_ptr)*na,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(nb)));
return 1;}
# 712
static struct Cyc_List_List*Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*in_tv_ovrs,struct Cyc_List_List*dsmm){
unsigned niter=0U;
struct Cyc_List_List*abs_decls=0;
int some_change=0;
struct Cyc_List_List*tv_ovrs=in_tv_ovrs;
struct Cyc_List_List*_tmpF8=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_copy;})(dsmm);struct Cyc_List_List*dsm=_tmpF8;
struct Cyc_List_List*changed_decls=0;
do{
changed_decls=0;{
struct Cyc_List_List*_tmpF9=dsm;struct Cyc_List_List*ds=_tmpF9;
struct Cyc_List_List*_tmpFA=0;struct Cyc_List_List*prev=_tmpFA;
some_change=0;
while((unsigned)ds){
int changed=0;
struct Cyc_Absyn_Decl*_tmpFB=(struct Cyc_Absyn_Decl*)ds->hd;struct Cyc_Absyn_Decl*d=_tmpFB;
struct _fat_ptr*_tmpFC=Cyc_Absyn_decl_name(d);struct _fat_ptr*dname=_tmpFC;
# 730
{void*_tmpFD=d->r;void*_stmttmpE=_tmpFD;void*_tmpFE=_stmttmpE;void*_tmpFF;switch(*((int*)_tmpFE)){case 0: _tmpFF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpFE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpFF;
# 732
{void*_tmp100=vd->type;void*_stmttmpF=_tmp100;void*_tmp101=_stmttmpF;void*_tmp103;void*_tmp102;switch(*((int*)_tmp101)){case 5: _tmp102=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp101)->f1;{struct Cyc_Absyn_FnInfo*fi=_tmp102;
# 734
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,fi,d);
# 740
goto _LLB;}case 0: _tmp102=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp101)->f1;_tmp103=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp101)->f2;{void*tc=_tmp102;struct Cyc_List_List*ts=_tmp103;
# 742
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,vd->type,d,1)!= 0;
goto _LLB;}case 3: _tmp102=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101)->f1).elt_type;{void*et=_tmp102;
_tmp102=et;goto _LL13;}case 4: _tmp102=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp101)->f1).elt_type;_LL13: {void*et=_tmp102;
# 746
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,et,d,1)!= 0;
goto _LLB;}default:
# 749
 goto _LLB;}_LLB:;}
# 751
goto _LL0;}case 1: _tmpFF=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpFE)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpFF;
# 753
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,& fd->i,d);
# 757
goto _LL0;}case 5: _tmpFF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpFE)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpFF;
# 759
if((unsigned)ad->impl){
struct Cyc_List_List*_tmp104=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fit=_tmp104;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
while((unsigned)fit){
struct Cyc_Absyn_Aggrfield*_tmp105=(struct Cyc_Absyn_Aggrfield*)fit->hd;struct Cyc_Absyn_Aggrfield*fld=_tmp105;
struct _tuple14*_tmp106=Cyc_Cifc_update_tvars(te,tv_ovrs,fld->type,d,0);struct _tuple14*tvtup=_tmp106;
if((unsigned)tvtup){
changed=1;{
struct Cyc_List_List*_tmp107=(*tvtup).f1;struct Cyc_List_List*ad=_tmp107;
if((unsigned)ad)
added_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(ad,added_tvars);{
struct Cyc_List_List*_tmp108=(*tvtup).f2;struct Cyc_List_List*rm=_tmp108;
if((unsigned)rm)
removed_tvars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(rm,removed_tvars);}}}
# 775
fit=fit->tl;}
# 777
while((unsigned)removed_tvars){
struct Cyc_Absyn_Tvar*_tmp109=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;struct Cyc_Absyn_Tvar*rtv=_tmp109;
{struct _handler_cons _tmp10A;_push_handler(& _tmp10A);{int _tmp10C=0;if(setjmp(_tmp10A.handler))_tmp10C=1;if(!_tmp10C){
({struct Cyc_List_List*_tmp1FE=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete;})(ad->tvs,rtv);ad->tvs=_tmp1FE;});;_pop_handler();}else{void*_tmp10B=(void*)Cyc_Core_get_exn_thrown();void*_tmp10D=_tmp10B;void*_tmp10E;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp10D)->tag == Cyc_Core_Not_found){
# 784
({struct Cyc_String_pa_PrintArg_struct _tmp111=({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0,({struct _fat_ptr _tmp1FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_tmp179.f1=_tmp1FF;});_tmp179;});void*_tmp10F[1];_tmp10F[0]=& _tmp111;({unsigned _tmp201=loc;struct _fat_ptr _tmp200=({const char*_tmp110="Removed tvar %s not found";_tag_fat(_tmp110,sizeof(char),26U);});Cyc_Warn_warn(_tmp201,_tmp200,_tag_fat(_tmp10F,sizeof(void*),1));});});
goto _LL16;}else{_tmp10E=_tmp10D;{void*exn=_tmp10E;(int)_rethrow(exn);}}_LL16:;}}}
# 787
removed_tvars=removed_tvars->tl;}
# 789
if((unsigned)added_tvars){
({struct Cyc_List_List*_tmp202=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(added_tvars,ad->tvs);ad->tvs=_tmp202;});{
struct Cyc_Absyn_Decl*_tmp112=Cyc_Cifc_make_abstract_decl(d);struct Cyc_Absyn_Decl*absdecl=_tmp112;
{struct _handler_cons _tmp113;_push_handler(& _tmp113);{int _tmp115=0;if(setjmp(_tmp113.handler))_tmp115=1;if(!_tmp115){
abs_decls=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*))Cyc_List_delete_cmp;})(Cyc_Cifc_decl_name_cmp,abs_decls,absdecl);;_pop_handler();}else{void*_tmp114=(void*)Cyc_Core_get_exn_thrown();void*_tmp116=_tmp114;void*_tmp117;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp116)->tag == Cyc_Core_Not_found)
# 796
goto _LL1B;else{_tmp117=_tmp116;{void*exn=_tmp117;(int)_rethrow(exn);}}_LL1B:;}}}
# 798
abs_decls=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->hd=absdecl,_tmp118->tl=abs_decls;_tmp118;});}}
# 800
if(changed)
changed_decls=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=d,_tmp119->tl=changed_decls;_tmp119;});}else{
# 804
struct _tuple0*_tmp11A=ad->name;struct _tuple0*_stmttmp10=_tmp11A;struct _tuple0*_tmp11B=_stmttmp10;void*_tmp11C;_tmp11C=_tmp11B->f2;{struct _fat_ptr*name=_tmp11C;
struct Cyc_Absyn_Decl*_tmp11D=Cyc_Absyn_lookup_decl(tv_ovrs,name);struct Cyc_Absyn_Decl*ovd=_tmp11D;
if((unsigned)ovd){
changed=1;{
void*_tmp11E=ovd->r;void*_stmttmp11=_tmp11E;void*_tmp11F=_stmttmp11;void*_tmp120;if(*((int*)_tmp11F)== 5){_tmp120=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp11F)->f1;{struct Cyc_Absyn_Aggrdecl*ovd_ad=_tmp120;
# 810
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*_tmp121=ovd_ad->tvs;struct Cyc_List_List*tvs=_tmp121;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*_tmp122=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp122;
struct Cyc_Absyn_Tvar*_tmp123=({struct Cyc_Absyn_Tvar*_tmp12A=_cycalloc(sizeof(*_tmp12A));*_tmp12A=*tv;_tmp12A;});struct Cyc_Absyn_Tvar*tvcpy=_tmp123;
struct _fat_ptr*_tmp124=({struct _fat_ptr*_tmp129=_cycalloc(sizeof(*_tmp129));({struct _fat_ptr _tmp204=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp128=({struct Cyc_Int_pa_PrintArg_struct _tmp17A;_tmp17A.tag=1,_tmp17A.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_tmp17A;});void*_tmp126[1];_tmp126[0]=& _tmp128;({struct _fat_ptr _tmp203=({const char*_tmp127="`ovr_%d";_tag_fat(_tmp127,sizeof(char),8U);});Cyc_aprintf(_tmp203,_tag_fat(_tmp126,sizeof(void*),1));});});*_tmp129=_tmp204;});_tmp129;});struct _fat_ptr*tvn=_tmp124;
tvcpy->name=tvn;
({struct Cyc_List_List*_tmp205=({struct Cyc_List_List*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->hd=tvcpy,_tmp125->tl=ad->tvs;_tmp125;});ad->tvs=_tmp205;});
tvs=tvs->tl;}
# 820
({struct Cyc_List_List*_tmp206=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ad->tvs);ad->tvs=_tmp206;});
goto _LL23;}}else{
# 823
({void*_tmp12B=0U;({int(*_tmp208)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp207=({const char*_tmp12C="ovd must be an aggr type";_tag_fat(_tmp12C,sizeof(char),25U);});_tmp208(_tmp207,_tag_fat(_tmp12B,sizeof(void*),0));});});}_LL23:;}}}}
# 829
goto _LL0;}case 8: _tmpFF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpFE)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmpFF;
# 831
goto _LL0;}default:
# 833
 goto _LL0;}_LL0:;}
# 835
prev=ds;
ds=ds->tl;
if(changed)
some_change=1;}
# 840
tv_ovrs=changed_decls;}}while(
some_change && ++ niter < 10U);
if(niter >= 10U){
({struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp17B;_tmp17B.tag=0,({
struct _fat_ptr _tmp209=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(changed_decls));_tmp17B.f1=_tmp209;});_tmp17B;});void*_tmp12D[1];_tmp12D[0]=& _tmp12F;({struct Cyc___cycFILE*_tmp20B=Cyc_stderr;struct _fat_ptr _tmp20A=({const char*_tmp12E="Suspected mutually recursive structs involving the following decls \n %s";_tag_fat(_tmp12E,sizeof(char),72U);});Cyc_fprintf(_tmp20B,_tmp20A,_tag_fat(_tmp12D,sizeof(void*),1));});});
({struct Cyc_Warn_String_Warn_Warg_struct _tmp131=({struct Cyc_Warn_String_Warn_Warg_struct _tmp17C;_tmp17C.tag=0,({struct _fat_ptr _tmp20C=({const char*_tmp132="Suspected mutually recursive structs -- abandoning cifc";_tag_fat(_tmp132,sizeof(char),56U);});_tmp17C.f1=_tmp20C;});_tmp17C;});void*_tmp130[1];_tmp130[0]=& _tmp131;Cyc_Warn_err2(loc,_tag_fat(_tmp130,sizeof(void*),1));});}
# 847
return abs_decls;}
# 850
static void Cyc_Cifc_i_clear_vartype_ids(void*t){
void*_tmp133=t;void*_tmp135;struct Cyc_Absyn_FnInfo _tmp136;struct Cyc_Absyn_ArrayInfo _tmp137;struct Cyc_Absyn_PtrInfo _tmp138;void*_tmp134;switch(*((int*)_tmp133)){case 2: _tmp134=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp134;
# 853
tv->identity=-1;
goto _LL0;}case 0: _tmp134=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f2;{struct Cyc_List_List*ts=_tmp134;
# 856
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 860
goto _LL0;}case 3: _tmp138=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Absyn_PtrInfo pi=_tmp138;
# 862
Cyc_Cifc_i_clear_vartype_ids(pi.elt_type);
goto _LL0;}case 4: _tmp137=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp137;
# 865
Cyc_Cifc_i_clear_vartype_ids(ai.elt_type);
goto _LL0;}case 5: _tmp136=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_Absyn_FnInfo fi=_tmp136;
# 868
Cyc_Cifc_i_clear_vartype_ids(fi.ret_type);{
struct Cyc_List_List*_tmp139=fi.args;struct Cyc_List_List*argit=_tmp139;
while((unsigned)argit){
struct _tuple8*_tmp13A=(struct _tuple8*)argit->hd;struct _tuple8*_stmttmp12=_tmp13A;struct _tuple8*_tmp13B=_stmttmp12;void*_tmp13C;_tmp13C=_tmp13B->f3;{void*at=_tmp13C;
Cyc_Cifc_i_clear_vartype_ids(at);
argit=argit->tl;}}
# 875
goto _LL0;}}case 6: _tmp134=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp133)->f1;{struct Cyc_List_List*ts=_tmp134;
# 877
while((unsigned)ts){
struct _tuple11*_tmp13D=(struct _tuple11*)ts->hd;struct _tuple11*_stmttmp13=_tmp13D;struct _tuple11*_tmp13E=_stmttmp13;void*_tmp13F;_tmp13F=_tmp13E->f2;{void*et=_tmp13F;
Cyc_Cifc_i_clear_vartype_ids(et);
ts=ts->tl;}}
# 882
goto _LL0;}case 8: _tmp134=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f2;_tmp135=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f4;{struct Cyc_List_List*ts=_tmp134;void*to=_tmp135;
# 887
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 891
goto _LL0;}default:
# 896
 goto _LL0;}_LL0:;}
# 900
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){
void*_tmp140=d->r;void*_stmttmp14=_tmp140;void*_tmp141=_stmttmp14;void*_tmp142;switch(*((int*)_tmp141)){case 0: _tmp142=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp141)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp142;
# 903
Cyc_Cifc_i_clear_vartype_ids(vd->type);
goto _LL0;}case 1: _tmp142=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp141)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp142;
# 906
Cyc_Cifc_i_clear_vartype_ids((fd->i).ret_type);{
struct Cyc_List_List*_tmp143=(fd->i).args;struct Cyc_List_List*ai=_tmp143;
while((unsigned)ai){
Cyc_Cifc_i_clear_vartype_ids((*((struct _tuple8*)ai->hd)).f3);}
# 911
(fd->i).tvars=0;
goto _LL0;}}case 5: _tmp142=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp141)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp142;
# 914
goto _LL0;}case 8: _tmp142=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp141)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp142;
# 916
goto _LL0;}default:
# 919
 goto _LL0;}_LL0:;}
# 923
static int Cyc_Cifc_eq(void*a,void*b){
return a == b;}
# 927
static int Cyc_Cifc_decl_not_present(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*a){
return !({(int(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*))Cyc_List_exists_c;})(({(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*))Cyc_Cifc_eq;}),a,l);}
# 932
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*type_overrides=0;
struct Cyc_List_List*overriden_decls=0;
struct Cyc_List_List*abs_decls=0;
{struct Cyc_List_List*_tmp144=ovrs;struct Cyc_List_List*x=_tmp144;for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Decl*_tmp145=(struct Cyc_Absyn_Decl*)x->hd;struct Cyc_Absyn_Decl*ud=_tmp145;
struct _fat_ptr*_tmp146=Cyc_Absyn_decl_name(ud);struct _fat_ptr*un=_tmp146;
if(!((unsigned)un))({void*_tmp147=0U;({unsigned _tmp20E=ud->loc;struct _fat_ptr _tmp20D=({const char*_tmp148="Overriding decl without a name\n";_tag_fat(_tmp148,sizeof(char),32U);});Cyc_Warn_warn(_tmp20E,_tmp20D,_tag_fat(_tmp147,sizeof(void*),0));});});else{
# 941
struct Cyc_Absyn_Decl*_tmp149=Cyc_Absyn_lookup_decl(*((struct Cyc_List_List**)_check_null(ds)),un);struct Cyc_Absyn_Decl*d=_tmp149;
if(!((unsigned)d))({struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp17D;_tmp17D.tag=0,_tmp17D.f1=(struct _fat_ptr)((struct _fat_ptr)*un);_tmp17D;});void*_tmp14A[1];_tmp14A[0]=& _tmp14C;({unsigned _tmp210=ud->loc;struct _fat_ptr _tmp20F=({const char*_tmp14B="%s is overridden but not defined";_tag_fat(_tmp14B,sizeof(char),33U);});Cyc_Warn_warn(_tmp210,_tmp20F,_tag_fat(_tmp14A,sizeof(void*),1));});});else{
# 944
overriden_decls=({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=d,_tmp14D->tl=overriden_decls;_tmp14D;});{
int _tmp14E=Cyc_Cifc_contains_type_vars(d);int pre_tvars_d=_tmp14E;
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);
if(Cyc_Cifc_contains_type_vars(ud)&& !pre_tvars_d){
abs_decls=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));({struct Cyc_Absyn_Decl*_tmp211=Cyc_Cifc_make_abstract_decl(ud);_tmp14F->hd=_tmp211;}),_tmp14F->tl=abs_decls;_tmp14F;});
type_overrides=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=d,_tmp150->tl=type_overrides;_tmp150;});}}}}}}{
# 956
struct Cyc_List_List*_tmp151=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_filter_c;})(Cyc_Cifc_decl_not_present,overriden_decls,*((struct Cyc_List_List**)_check_null(ds)));struct Cyc_List_List*unoverriden_decls=_tmp151;
abs_decls=({struct Cyc_List_List*(*_tmp213)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp212=abs_decls;_tmp213(_tmp212,
Cyc_Cifc_update_usages(loc,te,type_overrides,unoverriden_decls));});
if((unsigned)abs_decls)
({struct Cyc_List_List*_tmp214=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(abs_decls,*ds);*ds=_tmp214;});}}
