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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145 "list.h"
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 880 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 972
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 61 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 63
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
# 70
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var (void);
# 74
int Cyc_AssnDef_cmp_term(void*,void*);
# 79
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 96
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 116 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
# 120
void*Cyc_AssnDef_prim(void*,enum Cyc_AssnDef_Primreln,void*);
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 129
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
void*Cyc_AssnDef_not(void*);
int Cyc_AssnDef_simple_prove(void*,void*);
# 41 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
void*_tmp0=t;void*_tmp2;enum Cyc_Absyn_Primop _tmp3;int _tmp4;void*_tmp1;switch(*((int*)_tmp0)){case 0: _tmp1=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp0)->f1;{struct Cyc_Absyn_Exp*e=_tmp1;
return Cyc_Absynpp_exp2string(e);}case 1: _tmp1=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp1;
return Cyc_Absynpp_qvar2string(vd->name);}case 2: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1 == 0){_tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;{int i=_tmp4;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp7=({struct Cyc_Int_pa_PrintArg_struct _tmpF8;_tmpF8.tag=1,_tmpF8.f1=(unsigned long)i;_tmpF8;});void*_tmp5[1];_tmp5[0]=& _tmp7;({struct _fat_ptr _tmp128=({const char*_tmp6="_X%d";_tag_fat(_tmp6,sizeof(char),5U);});Cyc_aprintf(_tmp128,_tag_fat(_tmp5,sizeof(void*),1));});});}}else{_tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1;_tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp1;int i=_tmp4;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmpFA;_tmpFA.tag=0,({struct _fat_ptr _tmp129=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name));_tmpFA.f1=_tmp129;});_tmpFA;});struct Cyc_Int_pa_PrintArg_struct _tmpB=({struct Cyc_Int_pa_PrintArg_struct _tmpF9;_tmpF9.tag=1,_tmpF9.f1=(unsigned long)i;_tmpF9;});void*_tmp8[2];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB;({struct _fat_ptr _tmp12A=({const char*_tmp9="_%s%d";_tag_fat(_tmp9,sizeof(char),6U);});Cyc_aprintf(_tmp12A,_tag_fat(_tmp8,sizeof(void*),2));});});}}case 3: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp1=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;_tmp2=(void*)((((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp3;void*t1=_tmp1;void*t2=_tmp2;
# 48
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0,({struct _fat_ptr _tmp12B=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpFD.f1=_tmp12B;});_tmpFD;});struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmpFC;_tmpFC.tag=0,({
struct _fat_ptr _tmp12C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmpFC.f1=_tmp12C;});_tmpFC;});struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0,({struct _fat_ptr _tmp12D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpFB.f1=_tmp12D;});_tmpFB;});void*_tmpC[3];_tmpC[0]=& _tmpE,_tmpC[1]=& _tmpF,_tmpC[2]=& _tmp10;({struct _fat_ptr _tmp12E=({const char*_tmpD="(%s%s%s)";_tag_fat(_tmpD,sizeof(char),9U);});Cyc_aprintf(_tmp12E,_tag_fat(_tmpC,sizeof(void*),3));});});}}else{goto _LLB;}}else{_LLB: _tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp1=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;{enum Cyc_Absyn_Primop p=_tmp3;void*t=_tmp1;
# 51
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpFF;_tmpFF.tag=0,({struct _fat_ptr _tmp12F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmpFF.f1=_tmp12F;});_tmpFF;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0,({struct _fat_ptr _tmp130=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmpFE.f1=_tmp130;});_tmpFE;});void*_tmp11[2];_tmp11[0]=& _tmp13,_tmp11[1]=& _tmp14;({struct _fat_ptr _tmp131=({const char*_tmp12="%s(%s)";_tag_fat(_tmp12,sizeof(char),7U);});Cyc_aprintf(_tmp131,_tag_fat(_tmp11,sizeof(void*),2));});});}}}else{
# 54
({void*_tmp19=0U;({struct _fat_ptr _tmp132=({const char*_tmp1A="term2string primop args";_tag_fat(_tmp1A,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp132,_tag_fat(_tmp19,sizeof(void*),0));});});}default: _tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f2;{void*tp=_tmp1;void*tm=_tmp2;
# 53
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0,({struct _fat_ptr _tmp133=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_tmp101.f1=_tmp133;});_tmp101;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp100;_tmp100.tag=0,({struct _fat_ptr _tmp134=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp100.f1=_tmp134;});_tmp100;});void*_tmp15[2];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18;({struct _fat_ptr _tmp135=({const char*_tmp16="(%s)%s";_tag_fat(_tmp16,sizeof(char),7U);});Cyc_aprintf(_tmp135,_tag_fat(_tmp15,sizeof(void*),2));});});}};}
# 58
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0,_tmp1B->f1=e;_tmp1B;});}
void*Cyc_AssnDef_zero (void){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp1C=Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U);struct Cyc_Absyn_Exp*e=_tmp1C;
({void*_tmp136=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp136;});{
void*_tmp1D=Cyc_AssnDef_cnst(e);void*r=_tmp1D;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 69
void*Cyc_AssnDef_one (void){
static void*n=0;
if(n == 0){
struct Cyc_Absyn_Exp*_tmp1E=Cyc_Absyn_int_exp(Cyc_Absyn_None,1,0U);struct Cyc_Absyn_Exp*e=_tmp1E;
({void*_tmp137=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp137;});{
void*_tmp1F=Cyc_AssnDef_cnst(e);void*r=_tmp1F;
n=r;
return r;}}else{
return(void*)_check_null(n);}}
# 80
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=1,_tmp20->f1=vd;_tmp20;});}
# 82
inline static int Cyc_AssnDef_is_commutative(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp21=p;switch((int)_tmp21){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
 goto _LL6;case Cyc_Absyn_Eq: _LL6:
 goto _LL8;case Cyc_Absyn_Neq: _LL8:
 goto _LLA;case Cyc_Absyn_Bitand: _LLA:
 goto _LLC;case Cyc_Absyn_Bitor: _LLC:
 goto _LLE;case Cyc_Absyn_Bitxor: _LLE:
 return 1;default:
 return 0;};}
# 95
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_AssnDef_Primop_AssnDef_Term_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=3,_tmp22->f1=p,_tmp22->f2=ts;_tmp22;});}
# 98
void*Cyc_AssnDef_cast(void*tp,void*tm){
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=4,_tmp23->f1=tp,_tmp23->f2=tm;_tmp23;});}
# 101
void*Cyc_AssnDef_fresh_var (void){
static int counter=0;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=2,_tmp24->f1=0,_tmp24->f2=counter ++;_tmp24;});}struct _tuple11{void*f1;void*f2;};
# 106
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple11 _tmp25=({struct _tuple11 _tmp102;_tmp102.f1=t1,_tmp102.f2=t2;_tmp102;});struct _tuple11 _stmttmp0=_tmp25;struct _tuple11 _tmp26=_stmttmp0;enum Cyc_Absyn_Primop _tmp29;enum Cyc_Absyn_Primop _tmp27;void*_tmp2C;void*_tmp2B;int _tmp2E;int _tmp2D;void*_tmp2A;void*_tmp28;if(*((int*)_tmp26.f1)== 0){if(*((int*)_tmp26.f2)== 0){_tmp28=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp26.f1)->f1;_tmp2A=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp26.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp28;struct Cyc_Absyn_Exp*e2=_tmp2A;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{
return - 1;}}else{if(*((int*)_tmp26.f2)== 0)
return 1;else{if(*((int*)_tmp26.f1)== 1){if(*((int*)_tmp26.f2)== 1){_tmp28=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp26.f1)->f1;_tmp2A=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp26.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_tmp28;struct Cyc_Absyn_Vardecl*vd2=_tmp2A;
return(int)vd1 - (int)vd2;}}else{
return - 1;}}else{if(*((int*)_tmp26.f2)== 1)
return 1;else{if(*((int*)_tmp26.f1)== 2){if(*((int*)_tmp26.f2)== 2){_tmp28=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp26.f1)->f1;_tmp2D=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp26.f1)->f2;_tmp2A=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp26.f2)->f1;_tmp2E=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp26.f2)->f2;{struct Cyc_Absyn_Vardecl*vd1=_tmp28;int i=_tmp2D;struct Cyc_Absyn_Vardecl*vd2=_tmp2A;int j=_tmp2E;
# 115
if(vd1 == vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return - 1;}}else{if(*((int*)_tmp26.f2)== 2)
return 1;else{if(*((int*)_tmp26.f1)== 4){if(*((int*)_tmp26.f2)== 4){_tmp28=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp26.f1)->f1;_tmp2A=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp26.f1)->f2;_tmp2B=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp26.f2)->f1;_tmp2C=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp26.f2)->f2;{void*tp1=_tmp28;void*tm1=_tmp2A;void*tp2=_tmp2B;void*tm2=_tmp2C;
# 120
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return - 1;}}else{if(*((int*)_tmp26.f2)== 4)
return 1;else{_tmp27=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp26.f1)->f1;_tmp28=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp26.f1)->f2;_tmp29=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp26.f2)->f1;_tmp2A=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp26.f2)->f2;{enum Cyc_Absyn_Primop p1=_tmp27;struct Cyc_List_List*ts1=_tmp28;enum Cyc_Absyn_Primop p2=_tmp29;struct Cyc_List_List*ts2=_tmp2A;
# 126
if((int)p1 != (int)p2)return(int)p1 - (int)p2;
return((int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_AssnDef_cmp_term,ts1,ts2);}}}}}}}}};}
# 131
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)== 0;}
# 150
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 152
LOOP:
 if(((int(*)(struct Cyc_List_List*,void*))Cyc_List_memq)(*prev,a))return c;
({struct Cyc_List_List*_tmp138=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->hd=a,_tmp2F->tl=*prev;_tmp2F;});*prev=_tmp138;});{
void*_tmp30=a;void*_tmp33;void*_tmp32;void*_tmp31;switch(*((int*)_tmp30)){case 0:
 return c;case 1:
 return c;case 2:
 return(unsigned)1 + c;case 3: _tmp31=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp30)->f1;_tmp32=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp30)->f2;{void*a1=_tmp31;void*a2=_tmp32;
_tmp31=a1;_tmp32=a2;goto _LLA;}case 4: _tmp31=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp30)->f1;_tmp32=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp30)->f2;_LLA: {void*a1=_tmp31;void*a2=_tmp32;
# 161
c=Cyc_AssnDef_assn_size_loop(a2,(unsigned)1 + c,prev);
a=a1;
goto LOOP;}case 5: _tmp31=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp30)->f1;_tmp32=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp30)->f2;_tmp33=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp30)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp31;void*t=_tmp32;void*a1=_tmp33;
# 165
++ c;
a=a1;
goto LOOP;}default: _tmp31=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp30)->f1;{void*a1=_tmp31;
# 169
++ c;
a=a1;
goto LOOP;}};}}
# 178
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 181
static unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp34=Cyc_AssnDef_assn_size_loop(a,0U,& prev);unsigned c=_tmp34;
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 187
return c;}
# 192
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_tmp35=a;void*_tmp38;void*_tmp37;enum Cyc_AssnDef_Primreln _tmp39;void*_tmp36;switch(*((int*)_tmp35)){case 0:
 return({const char*_tmp3A="true";_tag_fat(_tmp3A,sizeof(char),5U);});case 1:
 return({const char*_tmp3B="false";_tag_fat(_tmp3B,sizeof(char),6U);});case 2: _tmp36=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp35)->f1;_tmp39=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp35)->f2;_tmp37=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp35)->f3;{void*t1=_tmp36;enum Cyc_AssnDef_Primreln p=_tmp39;void*t2=_tmp37;
# 197
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_AssnDef_Primreln _tmp3C=p;switch((int)_tmp3C){case Cyc_AssnDef_Eq:
 ps=({const char*_tmp3D="==";_tag_fat(_tmp3D,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_Neq:
 ps=({const char*_tmp3E="!=";_tag_fat(_tmp3E,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLt:
 ps=({const char*_tmp3F="S<";_tag_fat(_tmp3F,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLte:
 ps=({const char*_tmp40="S<=";_tag_fat(_tmp40,sizeof(char),4U);});goto _LLF;case Cyc_AssnDef_ULt:
 ps=({const char*_tmp41="U<";_tag_fat(_tmp41,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_ULte:
 ps=({const char*_tmp42="U<=";_tag_fat(_tmp42,sizeof(char),4U);});goto _LLF;default:
({void*_tmp43=0U;({struct _fat_ptr _tmp139=({const char*_tmp44="assn2string primop";_tag_fat(_tmp44,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp139,_tag_fat(_tmp43,sizeof(void*),0));});});}_LLF:;}
# 208
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp47=({struct Cyc_String_pa_PrintArg_struct _tmp105;_tmp105.tag=0,({struct _fat_ptr _tmp13A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp105.f1=_tmp13A;});_tmp105;});struct Cyc_String_pa_PrintArg_struct _tmp48=({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0,_tmp104.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp104;});struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0,({struct _fat_ptr _tmp13B=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp103.f1=_tmp13B;});_tmp103;});void*_tmp45[3];_tmp45[0]=& _tmp47,_tmp45[1]=& _tmp48,_tmp45[2]=& _tmp49;({struct _fat_ptr _tmp13C=({const char*_tmp46="%s%s%s";_tag_fat(_tmp46,sizeof(char),7U);});Cyc_aprintf(_tmp13C,_tag_fat(_tmp45,sizeof(void*),3));});});}}case 4: _tmp36=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp35)->f1;_tmp37=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp35)->f2;{void*a1=_tmp36;void*a2=_tmp37;
# 210
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4C=({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0,({struct _fat_ptr _tmp13D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp107.f1=_tmp13D;});_tmp107;});struct Cyc_String_pa_PrintArg_struct _tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmp106;_tmp106.tag=0,({struct _fat_ptr _tmp13E=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp106.f1=_tmp13E;});_tmp106;});void*_tmp4A[2];_tmp4A[0]=& _tmp4C,_tmp4A[1]=& _tmp4D;({struct _fat_ptr _tmp13F=({const char*_tmp4B="%s || %s";_tag_fat(_tmp4B,sizeof(char),9U);});Cyc_aprintf(_tmp13F,_tag_fat(_tmp4A,sizeof(void*),2));});});else{
# 214
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp109;_tmp109.tag=0,({struct _fat_ptr _tmp140=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp109.f1=_tmp140;});_tmp109;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0,({struct _fat_ptr _tmp141=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp108.f1=_tmp141;});_tmp108;});void*_tmp4E[2];_tmp4E[0]=& _tmp50,_tmp4E[1]=& _tmp51;({struct _fat_ptr _tmp142=({const char*_tmp4F="(%s || %s)";_tag_fat(_tmp4F,sizeof(char),11U);});Cyc_aprintf(_tmp142,_tag_fat(_tmp4E,sizeof(void*),2));});});}}case 3: _tmp36=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp35)->f1;_tmp37=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp35)->f2;{void*a1=_tmp36;void*a2=_tmp37;
# 216
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp10B;_tmp10B.tag=0,({struct _fat_ptr _tmp143=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_tmp10B.f1=_tmp143;});_tmp10B;});struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp10A;_tmp10A.tag=0,({struct _fat_ptr _tmp144=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_tmp10A.f1=_tmp144;});_tmp10A;});void*_tmp52[2];_tmp52[0]=& _tmp54,_tmp52[1]=& _tmp55;({struct _fat_ptr _tmp145=({const char*_tmp53="%s && %s";_tag_fat(_tmp53,sizeof(char),9U);});Cyc_aprintf(_tmp145,_tag_fat(_tmp52,sizeof(void*),2));});});}case 5: _tmp36=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp35)->f1;_tmp37=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp35)->f2;_tmp38=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp35)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp36;void*t=_tmp37;void*a=_tmp38;
# 219
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0,({struct _fat_ptr _tmp146=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp10E.f1=_tmp146;});_tmp10E;});struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0,({
struct _fat_ptr _tmp147=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp10D.f1=_tmp147;});_tmp10D;});struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0,({struct _fat_ptr _tmp148=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp10C.f1=_tmp148;});_tmp10C;});void*_tmp56[3];_tmp56[0]=& _tmp58,_tmp56[1]=& _tmp59,_tmp56[2]=& _tmp5A;({struct _fat_ptr _tmp149=({const char*_tmp57="Subst[%s:=%s](%s)";_tag_fat(_tmp57,sizeof(char),18U);});Cyc_aprintf(_tmp149,_tag_fat(_tmp56,sizeof(void*),3));});});}default: _tmp36=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp35)->f1;{void*a=_tmp36;
# 223
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0,({struct _fat_ptr _tmp14A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp10F.f1=_tmp14A;});_tmp10F;});void*_tmp5B[1];_tmp5B[0]=& _tmp5D;({struct _fat_ptr _tmp14B=({const char*_tmp5C="Kill(%s)";_tag_fat(_tmp5C,sizeof(char),9U);});Cyc_aprintf(_tmp14B,_tag_fat(_tmp5B,sizeof(void*),1));});});}};}
# 231
int Cyc_AssnDef_tree_size=0;
# 234
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 240
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 251 "assndef.cyc"
static int Cyc_AssnDef_assnhash(void*a){
void*_tmp5E=a;void*_tmp61;void*_tmp60;enum Cyc_AssnDef_Primreln _tmp62;void*_tmp5F;switch(*((int*)_tmp5E)){case 1:
 return 0;case 0:
 return 1;case 2: _tmp5F=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp5E)->f1;_tmp62=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp5E)->f2;_tmp60=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp5E)->f3;{void*t1=_tmp5F;enum Cyc_AssnDef_Primreln p=_tmp62;void*t2=_tmp60;
# 256
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _tmp5F=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp5E)->f1;_tmp60=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp5E)->f2;{void*a1=_tmp5F;void*a2=_tmp60;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _tmp5F=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp5E)->f1;_tmp60=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp5E)->f2;{void*a1=_tmp5F;void*a2=_tmp60;
return(int)((unsigned)a1 ^ (unsigned)a2 | (unsigned)1);}case 5: _tmp5F=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp5E)->f1;_tmp60=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp5E)->f2;_tmp61=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp5E)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp5F;void*t=_tmp60;void*a=_tmp61;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | (unsigned)3);}default: _tmp5F=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp5E)->f1;{void*a=_tmp5F;
# 261
return(int)((unsigned)a + (unsigned)37);}};}
# 268
static int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1 == a2)return 0;{
struct _tuple11 _tmp63=({struct _tuple11 _tmp110;_tmp110.f1=a1,_tmp110.f2=a2;_tmp110;});struct _tuple11 _stmttmp1=_tmp63;struct _tuple11 _tmp64=_stmttmp1;void*_tmp6A;void*_tmp69;enum Cyc_AssnDef_Primreln _tmp6C;enum Cyc_AssnDef_Primreln _tmp6B;void*_tmp68;void*_tmp67;void*_tmp66;void*_tmp65;if(*((int*)_tmp64.f1)== 0){if(*((int*)_tmp64.f2)== 0)
return 0;else{
return - 1;}}else{if(*((int*)_tmp64.f2)== 0)
return 1;else{if(*((int*)_tmp64.f1)== 1){if(*((int*)_tmp64.f2)== 1)
return 0;else{
return - 1;}}else{if(*((int*)_tmp64.f2)== 1)
return 1;else{switch(*((int*)_tmp64.f1)){case 4: if(*((int*)_tmp64.f2)== 4){_tmp65=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp64.f1)->f1;_tmp66=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp64.f1)->f2;_tmp67=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp64.f2)->f1;_tmp68=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp64.f2)->f2;{void*a11=_tmp65;void*a12=_tmp66;void*a21=_tmp67;void*a22=_tmp68;
_tmp65=a11;_tmp66=a12;_tmp67=a21;_tmp68=a22;goto _LL10;}}else{
# 285
return - 1;}case 3: switch(*((int*)_tmp64.f2)){case 3: _tmp65=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp64.f1)->f1;_tmp66=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp64.f1)->f2;_tmp67=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp64.f2)->f1;_tmp68=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp64.f2)->f2;_LL10: {void*a11=_tmp65;void*a12=_tmp66;void*a21=_tmp67;void*a22=_tmp68;
# 280
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c != 0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 287
 return - 1;}default: switch(*((int*)_tmp64.f2)){case 4: _LL13:
# 286
 return 1;case 3:
# 288
 return 1;default: if(*((int*)_tmp64.f1)== 2){if(*((int*)_tmp64.f2)== 2){_tmp65=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64.f1)->f1;_tmp6B=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64.f1)->f2;_tmp66=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64.f1)->f3;_tmp67=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64.f2)->f1;_tmp6C=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64.f2)->f2;_tmp68=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp64.f2)->f3;{void*t11=_tmp65;enum Cyc_AssnDef_Primreln p1=_tmp6B;void*t12=_tmp66;void*t21=_tmp67;enum Cyc_AssnDef_Primreln p2=_tmp6C;void*t22=_tmp68;
# 290
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return - 1;}}else{if(*((int*)_tmp64.f2)== 2)
return 1;else{if(*((int*)_tmp64.f1)== 6){if(*((int*)_tmp64.f2)== 6){_tmp65=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp64.f1)->f1;_tmp66=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp64.f2)->f1;{void*a1=_tmp65;void*a2=_tmp66;
return Cyc_AssnDef_assncmp(a1,a2);}}else{
return - 1;}}else{if(*((int*)_tmp64.f2)== 6)
return 1;else{_tmp65=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64.f1)->f1;_tmp66=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64.f1)->f2;_tmp67=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64.f1)->f3;_tmp68=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64.f2)->f1;_tmp69=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64.f2)->f2;_tmp6A=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp64.f2)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp65;void*t1=_tmp66;void*a11=_tmp67;struct Cyc_Absyn_Vardecl*vd2=_tmp68;void*t2=_tmp69;void*a22=_tmp6A;
# 301
int c=(int)vd1 - (int)vd2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c != 0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}};}}
# 317
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 320
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_hash_cons_table=0;}struct _tuple12{void*f1;int f2;};
# 325
int Cyc_AssnDef_is_reduced(void*a){
if(Cyc_AssnDef_hash_cons_table == 0)return 0;{
struct _tuple12**_tmp6D=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt)((struct Cyc_Hashtable_Table*)_check_null(Cyc_AssnDef_hash_cons_table),a);struct _tuple12**resopt=_tmp6D;
if(resopt == 0)return 0;{
struct _tuple12*_tmp6E=*resopt;struct _tuple12*_stmttmp2=_tmp6E;struct _tuple12*_tmp6F=_stmttmp2;int _tmp70;_tmp70=_tmp6F->f2;{int b=_tmp70;
return b;}}}}
# 336
static struct _tuple12 Cyc_AssnDef_copy_assn(void*a){
void*_tmp71=a;void*_tmp74;enum Cyc_AssnDef_Primreln _tmp75;void*_tmp73;void*_tmp72;switch(*((int*)_tmp71)){case 0:
 return({struct _tuple12 _tmp111;_tmp111.f1=(void*)& Cyc_AssnDef_true_assn,_tmp111.f2=1;_tmp111;});case 1:
 return({struct _tuple12 _tmp112;_tmp112.f1=(void*)& Cyc_AssnDef_false_assn,_tmp112.f2=0;_tmp112;});case 3: _tmp72=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp71)->f1;_tmp73=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp71)->f2;{void*a1=_tmp72;void*a2=_tmp73;
# 341
return({struct _tuple12 _tmp113;({void*_tmp14D=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=3,_tmp76->f1=a1,_tmp76->f2=a2;_tmp76;});_tmp113.f1=_tmp14D;}),({int _tmp14C=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp113.f2=_tmp14C;});_tmp113;});}case 4: _tmp72=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp71)->f1;_tmp73=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp71)->f2;{void*a1=_tmp72;void*a2=_tmp73;
# 343
return({struct _tuple12 _tmp114;({void*_tmp14F=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=4,_tmp77->f1=a1,_tmp77->f2=a2;_tmp77;});_tmp114.f1=_tmp14F;}),({int _tmp14E=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp114.f2=_tmp14E;});_tmp114;});}case 2: _tmp72=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp71)->f1;_tmp75=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp71)->f2;_tmp73=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp71)->f3;{void*t1=_tmp72;enum Cyc_AssnDef_Primreln p=_tmp75;void*t2=_tmp73;
return({struct _tuple12 _tmp115;({void*_tmp150=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=2,_tmp78->f1=t1,_tmp78->f2=p,_tmp78->f3=t2;_tmp78;});_tmp115.f1=_tmp150;}),_tmp115.f2=1;_tmp115;});}case 5: _tmp72=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp71)->f1;_tmp73=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp71)->f2;_tmp74=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp71)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp72;void*t1=_tmp73;void*a1=_tmp74;
return({struct _tuple12 _tmp116;({void*_tmp151=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=5,_tmp79->f1=vd1,_tmp79->f2=t1,_tmp79->f3=a1;_tmp79;});_tmp116.f1=_tmp151;}),_tmp116.f2=0;_tmp116;});}default: _tmp72=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp71)->f1;{void*a=_tmp72;
return({struct _tuple12 _tmp117;({void*_tmp152=(void*)({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=6,_tmp7A->f1=a;_tmp7A;});_tmp117.f1=_tmp152;}),_tmp117.f2=0;_tmp117;});}};}
# 354
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table == 0){
h=((struct Cyc_Hashtable_Table*(*)(int,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_create)(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;
# 361
({struct Cyc_Hashtable_Table*_tmp153=h;((void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert)(_tmp153,(void*)& Cyc_AssnDef_true_assn,({struct _tuple12*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->f1=(void*)& Cyc_AssnDef_true_assn,_tmp7B->f2=1;_tmp7B;}));});
({struct Cyc_Hashtable_Table*_tmp154=h;((void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert)(_tmp154,(void*)& Cyc_AssnDef_false_assn,({struct _tuple12*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=(void*)& Cyc_AssnDef_false_assn,_tmp7C->f2=1;_tmp7C;}));});}else{
# 364
h=(struct Cyc_Hashtable_Table*)_check_null(Cyc_AssnDef_hash_cons_table);}{
struct _tuple12**_tmp7D=((struct _tuple12**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt)(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);struct _tuple12**resopt=_tmp7D;
if(resopt == 0){
struct _tuple12 _tmp7E=Cyc_AssnDef_copy_assn(a);struct _tuple12 _stmttmp3=_tmp7E;struct _tuple12 _tmp7F=_stmttmp3;int _tmp81;void*_tmp80;_tmp80=_tmp7F.f1;_tmp81=_tmp7F.f2;{void*res=_tmp80;int reduced=_tmp81;
({struct Cyc_Hashtable_Table*_tmp156=h;void*_tmp155=res;((void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert)(_tmp156,_tmp155,({struct _tuple12*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=res,_tmp82->f2=reduced;_tmp82;}));});
return res;}}else{
# 371
struct _tuple12*_tmp83=*resopt;struct _tuple12*_stmttmp4=_tmp83;struct _tuple12*_tmp84=_stmttmp4;void*_tmp85;_tmp85=_tmp84->f1;{void*res=_tmp85;
return res;}}}}
# 377
void*Cyc_AssnDef_kill(void*a){
void*_tmp86=a;switch(*((int*)_tmp86)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 6: _LL6:
 return a;default:  {
# 383
struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp87=({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp118;_tmp118.tag=6,_tmp118.f1=a;_tmp118;});struct Cyc_AssnDef_Kill_AssnDef_Assn_struct p=_tmp87;
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 389
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp88=({struct _tuple11 _tmp11A;_tmp11A.f1=a1,_tmp11A.f2=a2;_tmp11A;});struct _tuple11 _stmttmp5=_tmp88;struct _tuple11 _tmp89=_stmttmp5;if(*((int*)_tmp89.f1)== 0)
return a2;else{if(*((int*)_tmp89.f2)== 0)
return a1;else{if(*((int*)_tmp89.f1)== 1)
return a1;else{if(*((int*)_tmp89.f2)== 1)
return a2;else{
# 400
if((int)a1 > (int)a2)
({void*_tmp8A=a1;void*_tmp8B=a2;a1=_tmp8B;a2=_tmp8A;});{
struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp8C=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp119;_tmp119.tag=3,_tmp119.f1=a1,_tmp119.f2=a2;_tmp119;});struct Cyc_AssnDef_And_AssnDef_Assn_struct a=_tmp8C;
return Cyc_AssnDef_hash_cons((void*)& a);}}}}};}}
# 408
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp8D=({struct _tuple11 _tmp11C;_tmp11C.f1=a1,_tmp11C.f2=a2;_tmp11C;});struct _tuple11 _stmttmp6=_tmp8D;struct _tuple11 _tmp8E=_stmttmp6;if(*((int*)_tmp8E.f1)== 0)
return a1;else{if(*((int*)_tmp8E.f2)== 0)
return a2;else{if(*((int*)_tmp8E.f1)== 1)
return a2;else{if(*((int*)_tmp8E.f2)== 1)
return a1;else{
# 417
if((int)a1 > (int)a2)
({void*_tmp8F=a1;void*_tmp90=a2;a1=_tmp90;a2=_tmp8F;});{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp91=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp11B;_tmp11B.tag=4,_tmp11B.f1=a1,_tmp11B.f2=a2;_tmp11B;});struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=_tmp91;
void*_tmp92=Cyc_AssnDef_hash_cons((void*)& a);void*res=_tmp92;
return res;}}}}};}}
# 426
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_tmp93=a;switch(*((int*)_tmp93)){case 0:
 goto _LL4;case 1: _LL4:
 return a;default:  {
# 431
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp94=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp11D;_tmp11D.tag=5,_tmp11D.f1=x,_tmp11D.f2=newx,_tmp11D.f3=a;_tmp11D;});struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=_tmp94;
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 437
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp95=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp11E;_tmp11E.tag=2,_tmp11E.f1=t1,_tmp11E.f2=p,_tmp11E.f3=t2;_tmp11E;});struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=_tmp95;
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 447
void*Cyc_AssnDef_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_Eq,t2);}
void*Cyc_AssnDef_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_Neq,t2);}
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_SLt,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_SLte,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_ULt,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,Cyc_AssnDef_ULte,t2);}
# 464 "assndef.cyc"
void*Cyc_AssnDef_not(void*a){
void*_tmp96=a;enum Cyc_AssnDef_Primreln _tmp98;void*_tmp99;void*_tmp97;switch(*((int*)_tmp96)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _tmp97=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp96)->f1;_tmp99=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp96)->f2;{void*a1=_tmp97;void*a2=_tmp99;
return({void*_tmp157=Cyc_AssnDef_not(a1);Cyc_AssnDef_or(_tmp157,Cyc_AssnDef_not(a2));});}case 4: _tmp97=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp96)->f1;_tmp99=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp96)->f2;{void*a1=_tmp97;void*a2=_tmp99;
return({void*_tmp158=Cyc_AssnDef_not(a1);Cyc_AssnDef_and(_tmp158,Cyc_AssnDef_not(a2));});}case 2: _tmp97=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp96)->f1;_tmp98=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp96)->f2;_tmp99=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp96)->f3;{void*t1=_tmp97;enum Cyc_AssnDef_Primreln p=_tmp98;void*t2=_tmp99;
# 471
enum Cyc_AssnDef_Primreln _tmp9A=p;switch((int)_tmp9A){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({void*_tmp9B=0U;({struct _fat_ptr _tmp159=({const char*_tmp9C="Vcgen::invert: non-primop";_tag_fat(_tmp9C,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp159,_tag_fat(_tmp9B,sizeof(void*),0));});});};}case 5:
# 480
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));default:
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));};}struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 503 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 506
void*_tmp9D=t;enum Cyc_Absyn_Primop _tmp9F;void*_tmpA0;void*_tmp9E;switch(*((int*)_tmp9D)){case 0:
 goto _LL4;case 2: _LL4:
 return t;case 4: _tmp9E=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp9D)->f1;_tmpA0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp9D)->f2;{void*tp=_tmp9E;void*tm=_tmpA0;
# 510
void*_tmpA1=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);void*tm2=_tmpA1;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 3: _tmp9F=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp9D)->f1;_tmp9E=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp9D)->f2;{enum Cyc_Absyn_Primop p=_tmp9F;struct Cyc_List_List*ts=_tmp9E;
# 514
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpA2=ts;struct Cyc_List_List*ptr=_tmpA2;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpA3=(void*)ptr->hd;void*old_term=_tmpA3;
void*_tmpA4=Cyc_AssnDef_reduce_term(old_term,non_esc_subst,esc_subst);void*new_term=_tmpA4;
res=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=new_term,_tmpA5->tl=res;_tmpA5;});
if(new_term != old_term)change=1;}}
# 522
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp15A=p;Cyc_AssnDef_primop(_tmp15A,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res));});}default: _tmp9E=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp9D)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp9E;
# 525
if(vd->escapes && esc_subst != 0){
# 528
struct Cyc_List_List*_tmpA6=*esc_subst;struct Cyc_List_List*s=_tmpA6;
for(1;s != 0;s=s->tl){
struct _tuple13 _tmpA7=*((struct _tuple13*)s->hd);struct _tuple13 _stmttmp7=_tmpA7;struct _tuple13 _tmpA8=_stmttmp7;void*_tmpAA;void*_tmpA9;_tmpA9=_tmpA8.f1;_tmpAA=_tmpA8.f2;{struct Cyc_Absyn_Vardecl*vd2=_tmpA9;void*t2=_tmpAA;
if(vd2 == vd)return t;}}{
# 535
void*_tmpAB=Cyc_AssnDef_fresh_var();void*t=_tmpAB;
({struct Cyc_List_List*_tmp15C=({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _tuple13*_tmp15B=({struct _tuple13*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->f1=vd,_tmpAC->f2=t;_tmpAC;});_tmpAD->hd=_tmp15B;}),_tmpAD->tl=*esc_subst;_tmpAD;});*esc_subst=_tmp15C;});
return Cyc_AssnDef_fresh_var();}}else{
if(!vd->escapes)
# 540
for(1;non_esc_subst != 0;non_esc_subst=non_esc_subst->tl){
struct _tuple13 _tmpAE=*((struct _tuple13*)non_esc_subst->hd);struct _tuple13 _stmttmp8=_tmpAE;struct _tuple13 _tmpAF=_stmttmp8;void*_tmpB1;void*_tmpB0;_tmpB0=_tmpAF.f1;_tmpB1=_tmpAF.f2;{struct Cyc_Absyn_Vardecl*vd2=_tmpB0;void*t2=_tmpB1;
if(vd == vd2)return t2;}}}
# 545
return t;}};}
# 549
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 552
LOOP: {
void*_tmpB2=a;void*_tmpB5;void*_tmpB4;enum Cyc_AssnDef_Primreln _tmpB6;void*_tmpB3;switch(*((int*)_tmpB2)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 2: _tmpB3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpB2)->f1;_tmpB6=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpB2)->f2;_tmpB4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpB2)->f3;{void*t1=_tmpB3;enum Cyc_AssnDef_Primreln p=_tmpB6;void*t2=_tmpB4;
# 557
void*_tmpB7=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);void*newt1=_tmpB7;
void*_tmpB8=Cyc_AssnDef_reduce_term(t2,non_esc_subst,esc_subst);void*newt2=_tmpB8;
if(t1 == newt1 && t2 == newt2)return a;
return Cyc_AssnDef_prim(newt1,p,newt2);}case 3: _tmpB3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpB2)->f1;_tmpB4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpB2)->f2;{void*a1=_tmpB3;void*a2=_tmpB4;
# 562
void*_tmpB9=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpB9;
void*_tmpBA=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpBA;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_and(newa1,newa2);}case 4: _tmpB3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpB2)->f1;_tmpB4=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpB2)->f2;{void*a1=_tmpB3;void*a2=_tmpB4;
# 567
void*_tmpBB=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpBB;
void*_tmpBC=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpBC;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_or(newa1,newa2);}case 6: _tmpB3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmpB2)->f1;{void*a=_tmpB3;
# 574
struct Cyc_List_List*new_esc_subst=0;
return Cyc_AssnDef_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _tmpB3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpB2)->f1;_tmpB4=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpB2)->f2;_tmpB5=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpB2)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpB3;void*t=_tmpB4;void*a1=_tmpB5;
# 578
t=Cyc_AssnDef_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));({struct _tuple13*_tmp15D=({struct _tuple13*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->f1=vd,_tmpBD->f2=t;_tmpBD;});_tmpBE->hd=_tmp15D;}),_tmpBE->tl=non_esc_subst;_tmpBE;});
a=a1;
goto LOOP;}};}}
# 586
void*Cyc_AssnDef_reduce(void*a){
# 588
if(Cyc_AssnDef_is_reduced(a))return a;
return Cyc_AssnDef_reduce_assn(a,0,0);}
# 593
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmpBF=t;void*_tmpC1;enum Cyc_Absyn_Primop _tmpC2;void*_tmpC0;switch(*((int*)_tmpBF)){case 1: _tmpC0=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpBF)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpC0;
# 596
if(vd == x)return newx;
return t;}case 3: _tmpC2=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpBF)->f1;_tmpC0=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpBF)->f2;{enum Cyc_Absyn_Primop p=_tmpC2;struct Cyc_List_List*ts=_tmpC0;
# 599
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpC3=ts;struct Cyc_List_List*ptr=_tmpC3;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpC4=(void*)ptr->hd;void*old_term=_tmpC4;
void*_tmpC5=Cyc_AssnDef_subst_term(old_term,x,newx);void*new_term=_tmpC5;
res=({struct Cyc_List_List*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->hd=new_term,_tmpC6->tl=res;_tmpC6;});
if(new_term != old_term)change=1;}}
# 607
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp15E=p;Cyc_AssnDef_primop(_tmp15E,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res));});}case 4: _tmpC0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpBF)->f1;_tmpC1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpBF)->f2;{void*tp=_tmpC0;void*tm=_tmpC1;
# 610
void*_tmpC7=Cyc_AssnDef_subst_term(tm,x,newx);void*tm2=_tmpC7;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 0:
 goto _LLA;default: _LLA:
 return t;};}
# 619
void*Cyc_AssnDef_kill_mem_term(void*t){
struct Cyc_List_List*_tmpC8=0;struct Cyc_List_List*esc_subst=_tmpC8;
return Cyc_AssnDef_reduce_term(t,0,& esc_subst);}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 634 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 638
while(1){
struct _tuple14 _tmpC9=({struct _tuple14 _tmp11F;_tmp11F.f1=a1s,_tmp11F.f2=a2s;_tmp11F;});struct _tuple14 _stmttmp9=_tmpC9;struct _tuple14 _tmpCA=_stmttmp9;if(_tmpCA.f1 == 0){if(_tmpCA.f2 == 0)
return;else{
# 642
({struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend)(a2s,*res2);*res2=_tmp15F;});return;}}else{if(_tmpCA.f2 == 0){
# 641
({struct Cyc_List_List*_tmp160=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend)(a1s,*res1);*res1=_tmp160;});return;}else{
# 644
int c=({void*_tmp161=(void*)((struct Cyc_List_List*)_check_null(a1s))->hd;Cyc_AssnDef_assncmp(_tmp161,(void*)((struct Cyc_List_List*)_check_null(a2s))->hd);});
if(c == 0){
({struct Cyc_List_List*_tmp162=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=(void*)a1s->hd,_tmpCB->tl=*common;_tmpCB;});*common=_tmp162;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_tmp163=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=(void*)a1s->hd,_tmpCC->tl=*res1;_tmpCC;});*res1=_tmp163;});
a1s=a1s->tl;}else{
# 653
({struct Cyc_List_List*_tmp164=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)a2s->hd,_tmpCD->tl=*res2;_tmpCD;});*res2=_tmp164;});
a2s=a2s->tl;}}}};}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 660
struct _tuple15 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 662
struct Cyc_List_List*_tmpCE=0;struct Cyc_List_List*common=_tmpCE;
struct Cyc_List_List*_tmpCF=0;struct Cyc_List_List*res1=_tmpCF;
struct Cyc_List_List*_tmpD0=0;struct Cyc_List_List*res2=_tmpD0;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple15 _tmp120;({struct Cyc_List_List*_tmp167=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(common);_tmp120.f1=_tmp167;}),({struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res1);_tmp120.f2=_tmp166;}),({struct Cyc_List_List*_tmp165=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res2);_tmp120.f3=_tmp165;});_tmp120;});}struct _tuple16{struct Cyc_List_List*f1;void*f2;};
# 671
struct _tuple16 Cyc_AssnDef_factor(void*a){
void*_tmpD1=a;void*_tmpD3;void*_tmpD2;switch(*((int*)_tmpD1)){case 0:
 return({struct _tuple16 _tmp121;_tmp121.f1=0,_tmp121.f2=(void*)& Cyc_AssnDef_true_assn;_tmp121;});case 1:
 return({struct _tuple16 _tmp122;_tmp122.f1=0,_tmp122.f2=(void*)& Cyc_AssnDef_false_assn;_tmp122;});case 2:
 return({struct _tuple16 _tmp123;({struct Cyc_List_List*_tmp168=({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=a,_tmpD4->tl=0;_tmpD4;});_tmp123.f1=_tmp168;}),_tmp123.f2=(void*)& Cyc_AssnDef_true_assn;_tmp123;});case 3: _tmpD2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpD1)->f1;_tmpD3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpD1)->f2;{void*a1=_tmpD2;void*a2=_tmpD3;
# 677
struct _tuple16 _tmpD5=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpA=_tmpD5;struct _tuple16 _tmpD6=_stmttmpA;void*_tmpD8;void*_tmpD7;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{struct Cyc_List_List*c1s=_tmpD7;void*a1=_tmpD8;
struct _tuple16 _tmpD9=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpB=_tmpD9;struct _tuple16 _tmpDA=_stmttmpB;void*_tmpDC;void*_tmpDB;_tmpDB=_tmpDA.f1;_tmpDC=_tmpDA.f2;{struct Cyc_List_List*c2s=_tmpDB;void*a2=_tmpDC;
return({struct _tuple16 _tmp124;({struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_merge)(Cyc_AssnDef_assncmp,c1s,c2s);_tmp124.f1=_tmp16A;}),({void*_tmp169=Cyc_AssnDef_and(a1,a2);_tmp124.f2=_tmp169;});_tmp124;});}}}case 4: _tmpD2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpD1)->f1;_tmpD3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpD1)->f2;{void*a1=_tmpD2;void*a2=_tmpD3;
# 681
struct _tuple16 _tmpDD=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpC=_tmpDD;struct _tuple16 _tmpDE=_stmttmpC;void*_tmpE0;void*_tmpDF;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;{struct Cyc_List_List*c1s=_tmpDF;void*a1=_tmpE0;
struct _tuple16 _tmpE1=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpD=_tmpE1;struct _tuple16 _tmpE2=_stmttmpD;void*_tmpE4;void*_tmpE3;_tmpE3=_tmpE2.f1;_tmpE4=_tmpE2.f2;{struct Cyc_List_List*c2s=_tmpE3;void*a2=_tmpE4;
struct _tuple15 _tmpE5=Cyc_AssnDef_intersect_assns(c1s,c2s);struct _tuple15 _stmttmpE=_tmpE5;struct _tuple15 _tmpE6=_stmttmpE;void*_tmpE9;void*_tmpE8;void*_tmpE7;_tmpE7=_tmpE6.f1;_tmpE8=_tmpE6.f2;_tmpE9=_tmpE6.f3;{struct Cyc_List_List*cs=_tmpE7;struct Cyc_List_List*c1s=_tmpE8;struct Cyc_List_List*c2s=_tmpE9;
return({struct _tuple16 _tmp125;_tmp125.f1=cs,({void*_tmp16C=({void*_tmp16B=((void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_tmp16B,((void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_AssnDef_and,a2,c2s));});_tmp125.f2=_tmp16C;});_tmp125;});}}}}default:
 return({struct _tuple16 _tmp126;_tmp126.f1=0,_tmp126.f2=a;_tmp126;});};}
# 689
void*Cyc_AssnDef_factor_assn(void*a){
# 691
struct _tuple16 _tmpEA=Cyc_AssnDef_factor(a);struct _tuple16 _stmttmpF=_tmpEA;struct _tuple16 _tmpEB=_stmttmpF;void*_tmpED;void*_tmpEC;_tmpEC=_tmpEB.f1;_tmpED=_tmpEB.f2;{struct Cyc_List_List*cs=_tmpEC;void*a=_tmpED;
a=((void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left)(Cyc_AssnDef_and,a,cs);
# 696
return a;}}struct _tuple17{enum Cyc_AssnDef_Primreln f1;enum Cyc_AssnDef_Primreln f2;};
# 703
int Cyc_AssnDef_find_ctxt(void*ctxt,void*t1,enum Cyc_AssnDef_Primreln p1,void*u1){
LOOP: {
void*_tmpEE=ctxt;enum Cyc_AssnDef_Primreln _tmpF0;void*_tmpF1;void*_tmpEF;switch(*((int*)_tmpEE)){case 1:
 return 1;case 0:
 return 0;case 3: _tmpEF=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpEE)->f1;_tmpF1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpEE)->f2;{void*a1=_tmpEF;void*a2=_tmpF1;
# 709
if(Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4: _tmpEF=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpEE)->f1;_tmpF1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpEE)->f2;{void*a1=_tmpEF;void*a2=_tmpF1;
# 713
if(!Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2: _tmpEF=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpEE)->f1;_tmpF0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpEE)->f2;_tmpF1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpEE)->f3;{void*t2=_tmpEF;enum Cyc_AssnDef_Primreln p2=_tmpF0;void*u2=_tmpF1;
# 717
struct _tuple17 _tmpF2=({struct _tuple17 _tmp127;_tmp127.f1=p2,_tmp127.f2=p1;_tmp127;});struct _tuple17 _stmttmp10=_tmpF2;struct _tuple17 _tmpF3=_stmttmp10;switch((int)_tmpF3.f1){case Cyc_AssnDef_Neq: if(_tmpF3.f2 == Cyc_AssnDef_Neq)
goto _LL11;else{goto _LL26;}case Cyc_AssnDef_Eq: switch((int)_tmpF3.f2){case Cyc_AssnDef_Eq: _LL11:
 goto _LL13;case Cyc_AssnDef_SLte: _LL13:
 goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 722
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);default: goto _LL26;}case Cyc_AssnDef_SLt: switch((int)_tmpF3.f2){case Cyc_AssnDef_Neq:
 goto _LL19;case Cyc_AssnDef_SLt:
# 728
 goto _LL1D;case Cyc_AssnDef_SLte: _LL1F:
# 730
 goto _LL21;default: goto _LL26;}case Cyc_AssnDef_ULt: switch((int)_tmpF3.f2){case Cyc_AssnDef_Neq: _LL19:
# 726
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);case Cyc_AssnDef_ULt: _LL1D:
# 729
 goto _LL1F;case Cyc_AssnDef_ULte: _LL21:
# 731
 goto _LL23;default: goto _LL26;}case Cyc_AssnDef_SLte: if(_tmpF3.f2 == Cyc_AssnDef_SLte){_LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_AssnDef_ULte: if(_tmpF3.f2 == Cyc_AssnDef_ULte){_LL25:
# 734
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2);}else{goto _LL26;}default: _LL26:
 return 0;};}default:
# 738
 ctxt=Cyc_AssnDef_reduce(ctxt);
goto LOOP;};}}
# 745
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt == a)return 1;{
void*_tmpF4=a;enum Cyc_AssnDef_Primreln _tmpF6;void*_tmpF7;void*_tmpF5;switch(*((int*)_tmpF4)){case 0:
 return 1;case 1:
 return 0;case 3: _tmpF5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpF4)->f1;_tmpF7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpF4)->f2;{void*a1=_tmpF5;void*a2=_tmpF7;
# 752
if(!Cyc_AssnDef_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4: _tmpF5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpF4)->f1;_tmpF7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpF4)->f2;{void*a1=_tmpF5;void*a2=_tmpF7;
# 756
if(Cyc_AssnDef_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2: _tmpF5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF4)->f1;_tmpF6=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF4)->f2;_tmpF7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF4)->f3;{void*t1=_tmpF5;enum Cyc_AssnDef_Primreln p=_tmpF6;void*t2=_tmpF7;
return Cyc_AssnDef_find_ctxt(ctxt,t1,p,t2);}default:
# 761
 a=Cyc_AssnDef_reduce(a);
goto LOOP;};}}
