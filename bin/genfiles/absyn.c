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
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
extern int Cyc_List_forall(int(*)(void*),struct Cyc_List_List*);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 34 "position.h"
extern unsigned Cyc_Position_segment_join(unsigned,unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 93 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 307
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 470
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr);
# 473
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 475
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr);
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 641 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 852
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 868
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 875
void*Cyc_Absyn_compress(void*);
# 886
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 889
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 894
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 896
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 898
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 901
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 905
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 907
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 922
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 932
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one (void);
# 938
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 940
void*Cyc_Absyn_starb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 942
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 944
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 946
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 961
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
# 963
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 969
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 971
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1034
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned);
# 1060
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1064
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1069
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1075
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1105
void*Cyc_Absyn_pointer_expand(void*,int);
# 1107
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1109
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1112
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1120
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 59
extern struct Cyc_Core_Opt Cyc_Kinds_tmko;
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Hashtable_Table;
# 82 "hashtable.h"
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 36 "absyn.cyc"
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(1;ss1 != 0 && ss2 != 0;(ss1=ss1->tl,ss2=ss2->tl)){
int i=Cyc_strptrcmp((struct _fat_ptr*)ss1->hd,(struct _fat_ptr*)ss2->hd);
if(i != 0)return i;}
# 41
if(ss1 != 0)return 1;
if(ss2 != 0)return - 1;
return 0;}
# 45
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 49
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;struct _fat_ptr*_tmp2;union Cyc_Absyn_Nmspace _tmp1;_LL1: _tmp1=_tmp0->f1;_tmp2=_tmp0->f2;_LL2: {union Cyc_Absyn_Nmspace n1=_tmp1;struct _fat_ptr*v1=_tmp2;
struct _tuple0*_tmp3=q2;struct _fat_ptr*_tmp5;union Cyc_Absyn_Nmspace _tmp4;_LL4: _tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_LL5: {union Cyc_Absyn_Nmspace n2=_tmp4;struct _fat_ptr*v2=_tmp5;
int i=Cyc_strptrcmp(v1,v2);
if(i != 0)return i;{
struct _tuple12 _tmp6=({struct _tuple12 _tmp12A;_tmp12A.f1=n1,_tmp12A.f2=n2;_tmp12A;});struct _tuple12 _stmttmp0=_tmp6;struct _tuple12 _tmp7=_stmttmp0;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpC;switch(((_tmp7.f1).Abs_n).tag){case 4: if(((_tmp7.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 61
 return - 1;}case 1: switch(((_tmp7.f2).Loc_n).tag){case 1: _LL9: _tmpC=((_tmp7.f1).Rel_n).val;_tmpD=((_tmp7.f2).Rel_n).val;_LLA: {struct Cyc_List_List*x1=_tmpC;struct Cyc_List_List*x2=_tmpD;
# 57
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: goto _LL11;default: _LL13: _LL14:
# 63
 return - 1;}case 2: switch(((_tmp7.f2).Rel_n).tag){case 2: _LLB: _tmpA=((_tmp7.f1).Abs_n).val;_tmpB=((_tmp7.f2).Abs_n).val;_LLC: {struct Cyc_List_List*x1=_tmpA;struct Cyc_List_List*x2=_tmpB;
# 58
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: goto _LL11;case 1: goto _LL15;default: _LL17: _LL18:
# 65
 return - 1;}default: switch(((_tmp7.f2).Rel_n).tag){case 3: _LLD: _tmp8=((_tmp7.f1).C_n).val;_tmp9=((_tmp7.f2).C_n).val;_LLE: {struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 59
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: _LL11: _LL12:
# 62
 return 1;case 1: _LL15: _LL16:
# 64
 return 1;default: _LL19: _LL1A:
# 66
 return 1;}}_LL6:;}}}}}
# 69
int Cyc_Absyn_hash_qvar(struct _tuple0*q){return Cyc_Hashtable_hash_string(*(*q).f2);}
# 71
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
return i != 0?i: tv1->identity - tv2->identity;}
# 76
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
return C_scope?({union Cyc_Absyn_Nmspace _tmp12B;(_tmp12B.C_n).tag=3U,(_tmp12B.C_n).val=x;_tmp12B;}):({union Cyc_Absyn_Nmspace _tmp12C;(_tmp12C.Abs_n).tag=2U,(_tmp12C.Abs_n).val=x;_tmp12C;});}
# 80
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp12D;(_tmp12D.Rel_n).tag=1U,(_tmp12D.Rel_n).val=x;_tmp12D;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 83
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmpE;union Cyc_Absyn_Nmspace _tmpF=_stmttmp1;switch((_tmpF.Loc_n).tag){case 1: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 92
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 96
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Kinds_tmko,tenv);}
# 100
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp12E;_tmp12E.print_const=0,_tmp12E.q_volatile=0,_tmp12E.q_restrict=0,_tmp12E.real_const=0,_tmp12E.loc=loc;_tmp12E;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp12F;_tmp12F.print_const=1,_tmp12F.q_volatile=0,_tmp12F.q_restrict=0,_tmp12F.real_const=1,_tmp12F.loc=loc;_tmp12F;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp130;_tmp130.print_const=x.print_const || y.print_const,_tmp130.q_volatile=
x.q_volatile || y.q_volatile,_tmp130.q_restrict=
x.q_restrict || y.q_restrict,_tmp130.real_const=
x.real_const || y.real_const,({
unsigned _tmp14E=Cyc_Position_segment_join(x.loc,y.loc);_tmp130.loc=_tmp14E;});_tmp130;});}
# 109
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 115
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 117
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp131;(_tmp131.UnknownDatatype).tag=1U,(_tmp131.UnknownDatatype).val=udi;_tmp131;});}
# 120
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp132;(_tmp132.KnownDatatype).tag=2U,(_tmp132.KnownDatatype).val=d;_tmp132;});}
# 123
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp133;(_tmp133.UnknownDatatypefield).tag=1U,(_tmp133.UnknownDatatypefield).val=s;_tmp133;});}
# 126
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp134;(_tmp134.KnownDatatypefield).tag=2U,((_tmp134.KnownDatatypefield).val).f1=dd,((_tmp134.KnownDatatypefield).val).f2=df;_tmp134;});}
# 129
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp135;(_tmp135.UnknownAggr).tag=1U,((_tmp135.UnknownAggr).val).f1=ak,((_tmp135.UnknownAggr).val).f2=n,((_tmp135.UnknownAggr).val).f3=tagged;_tmp135;});}
# 132
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp136;(_tmp136.KnownAggr).tag=2U,(_tmp136.KnownAggr).val=ad;_tmp136;});}
# 136
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=0,_tmp11->f1=c,({struct Cyc_List_List*_tmp14F=((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array)(args);_tmp11->f2=_tmp14F;});_tmp11;});}
# 139
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->tag=9,_tmp12->f1=e;_tmp12;});}
# 155 "absyn.cyc"
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_Absyn_void_type_cval={0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_void_type_tval={0,(void*)& Cyc_Absyn_void_type_cval,0};void*Cyc_Absyn_void_type=(void*)& Cyc_Absyn_void_type_tval;
static struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct Cyc_Absyn_heap_rgn_type_cval={5};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_heap_rgn_type_tval={0,(void*)& Cyc_Absyn_heap_rgn_type_cval,0};void*Cyc_Absyn_heap_rgn_type=(void*)& Cyc_Absyn_heap_rgn_type_tval;
static struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct Cyc_Absyn_unique_rgn_type_cval={6};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_unique_rgn_type_tval={0,(void*)& Cyc_Absyn_unique_rgn_type_cval,0};void*Cyc_Absyn_unique_rgn_type=(void*)& Cyc_Absyn_unique_rgn_type_tval;
static struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct Cyc_Absyn_refcnt_rgn_type_cval={7};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_refcnt_rgn_type_tval={0,(void*)& Cyc_Absyn_refcnt_rgn_type_cval,0};void*Cyc_Absyn_refcnt_rgn_type=(void*)& Cyc_Absyn_refcnt_rgn_type_tval;
static struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct Cyc_Absyn_true_type_cval={11};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_true_type_tval={0,(void*)& Cyc_Absyn_true_type_cval,0};void*Cyc_Absyn_true_type=(void*)& Cyc_Absyn_true_type_tval;
static struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct Cyc_Absyn_false_type_cval={12};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_false_type_tval={0,(void*)& Cyc_Absyn_false_type_cval,0};void*Cyc_Absyn_false_type=(void*)& Cyc_Absyn_false_type_tval;
static struct Cyc_Absyn_FatCon_Absyn_TyCon_struct Cyc_Absyn_fat_bound_type_cval={14};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_fat_bound_type_tval={0,(void*)& Cyc_Absyn_fat_bound_type_cval,0};void*Cyc_Absyn_fat_bound_type=(void*)& Cyc_Absyn_fat_bound_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_schar_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_schar_type_tval={0,(void*)& Cyc_Absyn_schar_type_cval,0};void*Cyc_Absyn_schar_type=(void*)& Cyc_Absyn_schar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uchar_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uchar_type_tval={0,(void*)& Cyc_Absyn_uchar_type_cval,0};void*Cyc_Absyn_uchar_type=(void*)& Cyc_Absyn_uchar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_char_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_char_type_tval={0,(void*)& Cyc_Absyn_char_type_cval,0};void*Cyc_Absyn_char_type=(void*)& Cyc_Absyn_char_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sshort_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sshort_type_tval={0,(void*)& Cyc_Absyn_sshort_type_cval,0};void*Cyc_Absyn_sshort_type=(void*)& Cyc_Absyn_sshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ushort_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ushort_type_tval={0,(void*)& Cyc_Absyn_ushort_type_cval,0};void*Cyc_Absyn_ushort_type=(void*)& Cyc_Absyn_ushort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nshort_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nshort_type_tval={0,(void*)& Cyc_Absyn_nshort_type_cval,0};void*Cyc_Absyn_nshort_type=(void*)& Cyc_Absyn_nshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sint_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sint_type_tval={0,(void*)& Cyc_Absyn_sint_type_cval,0};void*Cyc_Absyn_sint_type=(void*)& Cyc_Absyn_sint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uint_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uint_type_tval={0,(void*)& Cyc_Absyn_uint_type_cval,0};void*Cyc_Absyn_uint_type=(void*)& Cyc_Absyn_uint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nint_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nint_type_tval={0,(void*)& Cyc_Absyn_nint_type_cval,0};void*Cyc_Absyn_nint_type=(void*)& Cyc_Absyn_nint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slong_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slong_type_tval={0,(void*)& Cyc_Absyn_slong_type_cval,0};void*Cyc_Absyn_slong_type=(void*)& Cyc_Absyn_slong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulong_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulong_type_tval={0,(void*)& Cyc_Absyn_ulong_type_cval,0};void*Cyc_Absyn_ulong_type=(void*)& Cyc_Absyn_ulong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlong_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlong_type_tval={0,(void*)& Cyc_Absyn_nlong_type_cval,0};void*Cyc_Absyn_nlong_type=(void*)& Cyc_Absyn_nlong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slonglong_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slonglong_type_tval={0,(void*)& Cyc_Absyn_slonglong_type_cval,0};void*Cyc_Absyn_slonglong_type=(void*)& Cyc_Absyn_slonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulonglong_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulonglong_type_tval={0,(void*)& Cyc_Absyn_ulonglong_type_cval,0};void*Cyc_Absyn_ulonglong_type=(void*)& Cyc_Absyn_ulonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlonglong_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlonglong_type_tval={0,(void*)& Cyc_Absyn_nlonglong_type_cval,0};void*Cyc_Absyn_nlonglong_type=(void*)& Cyc_Absyn_nlonglong_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_float_type_cval={2,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_float_type_tval={0,(void*)& Cyc_Absyn_float_type_cval,0};void*Cyc_Absyn_float_type=(void*)& Cyc_Absyn_float_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_double_type_cval={2,1};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_double_type_tval={0,(void*)& Cyc_Absyn_double_type_cval,0};void*Cyc_Absyn_double_type=(void*)& Cyc_Absyn_double_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_long_double_type_cval={2,2};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_long_double_type_tval={0,(void*)& Cyc_Absyn_long_double_type_cval,0};void*Cyc_Absyn_long_double_type=(void*)& Cyc_Absyn_long_double_type_tval;
# 181
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 183
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={3};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={4};
static struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct Cyc_Absyn_AccessCon_val={8};
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9};
void*Cyc_Absyn_rgn_handle_type(void*r){return({void*_tmp13[1];_tmp13[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_fat(_tmp13,sizeof(void*),1));});}
void*Cyc_Absyn_tag_type(void*t){return({void*_tmp14[1];_tmp14[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_fat(_tmp14,sizeof(void*),1));});}
void*Cyc_Absyn_access_eff(void*r){return({void*_tmp15[1];_tmp15[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_AccessCon_val,_tag_fat(_tmp15,sizeof(void*),1));});}
void*Cyc_Absyn_regionsof_eff(void*t){return({void*_tmp16[1];_tmp16[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_fat(_tmp16,sizeof(void*),1));});}
void*Cyc_Absyn_thin_bounds_type(void*t){return({void*_tmp17[1];_tmp17[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_fat(_tmp17,sizeof(void*),1));});}
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=0,_tmp18->f1=(void*)& Cyc_Absyn_empty_effect_cval,_tmp18->f2=ts;_tmp18;});}
# 196
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp19=0U;({void*_tmp150=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->tag=15,_tmp1A->f1=n,_tmp1A->f2=d;_tmp1A;});Cyc_Absyn_app_type(_tmp150,_tag_fat(_tmp19,sizeof(void*),0));});});}
# 199
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp1B=0U;({void*_tmp151=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=16,_tmp1C->f1=fs;_tmp1C;});Cyc_Absyn_app_type(_tmp151,_tag_fat(_tmp1B,sizeof(void*),0));});});}
# 202
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp1D=0U;({void*_tmp152=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->tag=17,_tmp1E->f1=s,_tmp1E->f2=k;_tmp1E;});Cyc_Absyn_app_type(_tmp152,_tag_fat(_tmp1D,sizeof(void*),0));});});}
# 205
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=0,({void*_tmp153=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=18,_tmp1F->f1=di;_tmp1F;});_tmp20->f1=_tmp153;}),_tmp20->f2=args;_tmp20;});}
# 208
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=0,({void*_tmp154=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=19,_tmp21->f1=di;_tmp21;});_tmp22->f1=_tmp154;}),_tmp22->f2=args;_tmp22;});}
# 211
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=0,({void*_tmp155=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=20,_tmp23->f1=ai;_tmp23;});_tmp24->f1=_tmp155;}),_tmp24->f2=args;_tmp24;});}
# 215
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=2,_tmp25->f1=x;_tmp25;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp26=i;switch(_tmp26){case 0: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28=({struct Cyc_Warn_String_Warn_Warg_struct _tmp139;_tmp139.tag=0,({struct _fat_ptr _tmp156=({const char*_tmp2C="gen_float_type(";_tag_fat(_tmp2C,sizeof(char),16U);});_tmp139.f1=_tmp156;});_tmp139;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp29=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp138;_tmp138.tag=12,_tmp138.f1=(int)i;_tmp138;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp137;_tmp137.tag=0,({struct _fat_ptr _tmp157=({const char*_tmp2B=")";_tag_fat(_tmp2B,sizeof(char),2U);});_tmp137.f1=_tmp157;});_tmp137;});void*_tmp27[3];_tmp27[0]=& _tmp28,_tmp27[1]=& _tmp29,_tmp27[2]=& _tmp2A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp27,sizeof(void*),3));});}_LL0:;}
# 224
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp2D=sn;switch(_tmp2D){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 227
enum Cyc_Absyn_Size_of _tmp2E=sz;switch(_tmp2E){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 236
enum Cyc_Absyn_Size_of _tmp2F=sz;switch(_tmp2F){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 244
 goto _LL8;default: _LL7: _LL8: {
# 246
enum Cyc_Absyn_Size_of _tmp30=sz;switch(_tmp30){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 257
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 260
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 265
void*Cyc_Absyn_bounds_one (void){
static void*bone=0;
if(bone == 0)
bone=Cyc_Absyn_thin_bounds_int(1U);
return(void*)_check_null(bone);}
# 275
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 278
void*Cyc_Absyn_wchar_type (void){
int _tmp31=Sizeof_wchar_t;switch(_tmp31){case 1: _LL1: _LL2:
# 289 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 293
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp32[4U]="exn";
# 298
static struct _fat_ptr Cyc_Absyn_exn_str={_tmp32,_tmp32,_tmp32 + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp39[15U]="Null_Exception";static char _tmp3A[13U]="Array_bounds";static char _tmp3B[16U]="Match_Exception";static char _tmp3C[10U]="Bad_alloc";
# 302
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void){
static struct _fat_ptr builtin_exns[4U]={{_tmp39,_tmp39,_tmp39 + 15U},{_tmp3A,_tmp3A,_tmp3A + 13U},{_tmp3B,_tmp3B,_tmp3B + 16U},{_tmp3C,_tmp3C,_tmp3C + 10U}};
# 305
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_Datatypefield*_tmp15A=({struct Cyc_Absyn_Datatypefield*_tmp35=_cycalloc(sizeof(*_tmp35));({struct _tuple0*_tmp159=({struct _tuple0*_tmp34=_cycalloc(sizeof(*_tmp34));((_tmp34->f1).Abs_n).tag=2U,((_tmp34->f1).Abs_n).val=0,({
struct _fat_ptr*_tmp158=({struct _fat_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));*_tmp33=*((struct _fat_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _fat_ptr),i));_tmp33;});_tmp34->f2=_tmp158;});_tmp34;});
# 309
_tmp35->name=_tmp159;}),_tmp35->typs=0,_tmp35->loc=0U,_tmp35->sc=Cyc_Absyn_Extern;_tmp35;});_tmp36->hd=_tmp15A;}),_tmp36->tl=tufs;_tmp36;});}}
# 313
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->sc=Cyc_Absyn_Extern,_tmp38->name=Cyc_Absyn_exn_name,_tmp38->tvs=0,({struct Cyc_Core_Opt*_tmp15B=({struct Cyc_Core_Opt*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->v=tufs;_tmp37;});_tmp38->fields=_tmp15B;}),_tmp38->is_extensible=1;_tmp38;});}
# 315
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 318
void*Cyc_Absyn_exn_type (void){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp13A;(_tmp13A.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp15D=({struct Cyc_Absyn_Datatypedecl**_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_Absyn_Datatypedecl*_tmp15C=Cyc_Absyn_exn_tud();*_tmp3D=_tmp15C;});_tmp3D;});(_tmp13A.KnownDatatype).val=_tmp15D;});_tmp13A;}),0);
exn_typ=({void*_tmp161=(void*)_check_null(eopt);void*_tmp160=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp15F=Cyc_Absyn_empty_tqual(0U);void*_tmp15E=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp161,_tmp160,_tmp15F,_tmp15E,Cyc_Absyn_false_type);});}
# 325
return(void*)_check_null(exn_typ);}
# 328
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp40=_cycalloc(sizeof(*_tmp40));({union Cyc_Absyn_Nmspace _tmp164=Cyc_Absyn_Abs_n(0,0);_tmp40->f1=_tmp164;}),({struct _fat_ptr*_tmp163=({struct _fat_ptr*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct _fat_ptr _tmp162=({const char*_tmp3E="PrintArg";_tag_fat(_tmp3E,sizeof(char),9U);});*_tmp3F=_tmp162;});_tmp3F;});_tmp40->f2=_tmp163;});_tmp40;});
return(struct _tuple0*)_check_null(q);}
# 334
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp43=_cycalloc(sizeof(*_tmp43));({union Cyc_Absyn_Nmspace _tmp167=Cyc_Absyn_Abs_n(0,0);_tmp43->f1=_tmp167;}),({struct _fat_ptr*_tmp166=({struct _fat_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _fat_ptr _tmp165=({const char*_tmp41="ScanfArg";_tag_fat(_tmp41,sizeof(char),9U);});*_tmp42=_tmp165;});_tmp42;});_tmp43->f2=_tmp166;});_tmp43;});
return(struct _tuple0*)_check_null(q);}
# 346
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp49=_cycalloc(sizeof(*_tmp49));({union Cyc_Absyn_Nmspace _tmp16C=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));({struct _fat_ptr*_tmp16B=({struct _fat_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));({struct _fat_ptr _tmp16A=({const char*_tmp44="Core";_tag_fat(_tmp44,sizeof(char),5U);});*_tmp45=_tmp16A;});_tmp45;});_tmp46->hd=_tmp16B;}),_tmp46->tl=0;_tmp46;}),0);_tmp49->f1=_tmp16C;}),({struct _fat_ptr*_tmp169=({struct _fat_ptr*_tmp48=_cycalloc(sizeof(*_tmp48));({struct _fat_ptr _tmp168=({const char*_tmp47="unique_region";_tag_fat(_tmp47,sizeof(char),14U);});*_tmp48=_tmp168;});_tmp48;});_tmp49->f2=_tmp169;});_tmp49;});
return(struct _tuple0*)_check_null(q);}
# 352
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void){
void*_tmp4A=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);void*t=_tmp4A;
return({struct Cyc_Absyn_Exp*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->topt=t,_tmp4E->loc=0U,_tmp4E->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp171=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=1,({void*_tmp170=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=1,({struct Cyc_Absyn_Vardecl*_tmp16F=({struct Cyc_Absyn_Vardecl*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->sc=Cyc_Absyn_Extern,({
struct _tuple0*_tmp16E=Cyc_Absyn_uniquergn_qvar();_tmp4B->name=_tmp16E;}),_tmp4B->varloc=0U,({
struct Cyc_Absyn_Tqual _tmp16D=Cyc_Absyn_empty_tqual(0U);_tmp4B->tq=_tmp16D;}),_tmp4B->type=t,_tmp4B->initializer=0,_tmp4B->rgn=0,_tmp4B->attributes=0,_tmp4B->escapes=0,_tmp4B->is_proto=0;_tmp4B;});
# 355
_tmp4C->f1=_tmp16F;});_tmp4C;});_tmp4D->f1=_tmp170;});_tmp4D;});_tmp4E->r=_tmp171;});_tmp4E;});}
# 363
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=3,_tmp4F->f1=s;_tmp4F;});}
# 367
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp13B;_tmp13B.elt_type=t,_tmp13B.elt_tq=tq,(_tmp13B.ptr_atts).rgn=r,(_tmp13B.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp13B.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp13B.ptr_atts).zero_term=zt,(_tmp13B.ptr_atts).ptrloc=0,(_tmp13B.ptr_atts).released=rel;_tmp13B;}));}
# 373
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp13C;_tmp13C.elt_type=t,_tmp13C.elt_tq=tq,(_tmp13C.ptr_atts).rgn=r,(_tmp13C.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp13C.ptr_atts).bounds=b,(_tmp13C.ptr_atts).zero_term=zt,(_tmp13C.ptr_atts).ptrloc=0,(_tmp13C.ptr_atts).released=rel;_tmp13C;}));}
# 379
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp13D;_tmp13D.elt_type=t,_tmp13D.elt_tq=tq,(_tmp13D.ptr_atts).rgn=r,(_tmp13D.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp13D.ptr_atts).bounds=b,(_tmp13D.ptr_atts).zero_term=zt,(_tmp13D.ptr_atts).ptrloc=0,(_tmp13D.ptr_atts).released=rel;_tmp13D;}));}
# 385
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm,void*rel){
return({void*_tmp176=t;void*_tmp175=r;struct Cyc_Absyn_Tqual _tmp174=tq;void*_tmp173=Cyc_Absyn_bounds_one();void*_tmp172=zeroterm;Cyc_Absyn_starb_type(_tmp176,_tmp175,_tmp174,_tmp173,_tmp172,rel);});}
# 388
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}
# 391
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm,void*rel){
return({void*_tmp17B=t;void*_tmp17A=r;struct Cyc_Absyn_Tqual _tmp179=tq;void*_tmp178=Cyc_Absyn_bounds_one();void*_tmp177=zeroterm;Cyc_Absyn_atb_type(_tmp17B,_tmp17A,_tmp179,_tmp178,_tmp177,rel);});}
# 394
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp180=Cyc_Absyn_char_type;void*_tmp17F=rgn;struct Cyc_Absyn_Tqual _tmp17E=Cyc_Absyn_empty_tqual(0U);void*_tmp17D=Cyc_Absyn_fat_bound_type;void*_tmp17C=Cyc_Absyn_true_type;Cyc_Absyn_starb_type(_tmp180,_tmp17F,_tmp17E,_tmp17D,_tmp17C,Cyc_Absyn_false_type);});}
# 397
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp185=Cyc_Absyn_char_type;void*_tmp184=rgn;struct Cyc_Absyn_Tqual _tmp183=Cyc_Absyn_const_tqual(0U);void*_tmp182=Cyc_Absyn_fat_bound_type;void*_tmp181=Cyc_Absyn_true_type;Cyc_Absyn_starb_type(_tmp185,_tmp184,_tmp183,_tmp182,_tmp181,Cyc_Absyn_false_type);});}
# 401
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 403
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=4,(_tmp50->f1).elt_type=elt_type,(_tmp50->f1).tq=tq,(_tmp50->f1).num_elts=num_elts,(_tmp50->f1).zero_term=zero_term,(_tmp50->f1).zt_loc=ztloc;_tmp50;});}
# 406
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=11,_tmp51->f1=e;_tmp51;});}
# 411
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 413
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=8,_tmp52->f1=n,_tmp52->f2=args,_tmp52->f3=d,_tmp52->f4=defn;_tmp52;});}
# 419
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=0,({void*_tmp188=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=20,({union Cyc_Absyn_AggrInfo _tmp187=({enum Cyc_Absyn_AggrKind _tmp186=k;Cyc_Absyn_UnknownAggr(_tmp186,({struct _tuple0*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->f1=Cyc_Absyn_rel_ns_null,_tmp53->f2=name;_tmp53;}),0);});_tmp54->f1=_tmp187;});_tmp54;});_tmp55->f1=_tmp188;}),_tmp55->f2=0;_tmp55;});}
# 422
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 425
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 428
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 433
void*Cyc_Absyn_compress(void*t){
void*_tmp56=t;void*_tmp57;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Exp*_tmp59;void**_tmp5A;void**_tmp5B;switch(*((int*)_tmp56)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp56)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp5B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp56)->f2;_LL8: {void**t2opt_ref=_tmp5B;
# 439
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Absyn_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp56)->f4 == 0){_LL3: _LL4:
# 436
 return t;}else{_LL5: _tmp5A=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp56)->f4;_LL6: {void**topt_ref=_tmp5A;
_tmp5B=topt_ref;goto _LL8;}}case 9: _LL9: _tmp59=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp56)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp59;
# 445
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp5C=e->r;void*_stmttmp2=_tmp5C;void*_tmp5D=_stmttmp2;void*_tmp5E;if(*((int*)_tmp5D)== 39){_LL12: _tmp5E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5D)->f1;_LL13: {void*t2=_tmp5E;
return Cyc_Absyn_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11: _LLB: _tmp58=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp56)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp58;
# 451
void*_tmp5F=e->topt;void*t2=_tmp5F;
return t2 != 0?t2: t;}case 10: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp56)->f2 != 0){_LLD: _tmp57=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp56)->f2;_LLE: {void*t=_tmp57;
return Cyc_Absyn_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 459
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp13E;(_tmp13E.Char_c).tag=2U,((_tmp13E.Char_c).val).f1=sn,((_tmp13E.Char_c).val).f2=c;_tmp13E;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp13F;(_tmp13F.Wchar_c).tag=3U,(_tmp13F.Wchar_c).val=s;_tmp13F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp140;(_tmp140.Short_c).tag=4U,((_tmp140.Short_c).val).f1=sn,((_tmp140.Short_c).val).f2=s;_tmp140;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp141;(_tmp141.Int_c).tag=5U,((_tmp141.Int_c).val).f1=sn,((_tmp141.Int_c).val).f2=i;_tmp141;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp142;(_tmp142.LongLong_c).tag=6U,((_tmp142.LongLong_c).val).f1=sn,((_tmp142.LongLong_c).val).f2=l;_tmp142;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp143;(_tmp143.Float_c).tag=7U,((_tmp143.Float_c).val).f1=s,((_tmp143.Float_c).val).f2=i;_tmp143;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp144;(_tmp144.String_c).tag=8U,(_tmp144.String_c).val=s;_tmp144;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp145;(_tmp145.Wstring_c).tag=9U,(_tmp145.Wstring_c).val=s;_tmp145;});}
# 469
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->topt=0,_tmp60->r=r,_tmp60->loc=loc,_tmp60->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp60;});}
# 472
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp189=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=16,_tmp61->f1=rgn_handle,_tmp61->f2=e;_tmp61;});Cyc_Absyn_new_exp(_tmp189,loc);});}
# 475
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp62=_cycalloc(sizeof(*_tmp62));*_tmp62=*e;_tmp62;});}
# 478
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp18A=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=0,_tmp63->f1=c;_tmp63;});Cyc_Absyn_new_exp(_tmp18A,loc);});}
# 481
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 485
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp18B=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp18B,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 493
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 500
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp18C=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp18C,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp18D=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp18D,loc);});}
# 507
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp18E=f(s);Cyc_Absyn_const_exp(_tmp18E,loc);});}
# 510
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 514
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp190=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=1,({void*_tmp18F=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=0,_tmp64->f1=q;_tmp64;});_tmp65->f1=_tmp18F;});_tmp65;});Cyc_Absyn_new_exp(_tmp190,loc);});}
# 517
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp191=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=1,_tmp66->f1=b;_tmp66;});Cyc_Absyn_new_exp(_tmp191,loc);});}
# 521
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 524
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
return({void*_tmp192=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=2,_tmp67->f1=s;_tmp67;});Cyc_Absyn_new_exp(_tmp192,loc);});}
# 527
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp193=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=3,_tmp68->f1=p,_tmp68->f2=es;_tmp68;});Cyc_Absyn_new_exp(_tmp193,loc);});}
# 530
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp195=p;struct Cyc_List_List*_tmp194=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=e,_tmp69->tl=0;_tmp69;});Cyc_Absyn_primop_exp(_tmp195,_tmp194,loc);});}
# 533
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp198=p;struct Cyc_List_List*_tmp197=({struct Cyc_List_List*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->hd=e1,({struct Cyc_List_List*_tmp196=({struct Cyc_List_List*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->hd=e2,_tmp6A->tl=0;_tmp6A;});_tmp6B->tl=_tmp196;});_tmp6B;});Cyc_Absyn_primop_exp(_tmp198,_tmp197,loc);});}
# 536
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp199=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=35,_tmp6C->f1=e1,_tmp6C->f2=e2;_tmp6C;});Cyc_Absyn_new_exp(_tmp199,loc);});}
# 539
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 549
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp19A=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=4,_tmp6D->f1=e1,_tmp6D->f2=popt,_tmp6D->f3=e2;_tmp6D;});Cyc_Absyn_new_exp(_tmp19A,loc);});}
# 552
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 555
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp19B=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=5,_tmp6E->f1=e,_tmp6E->f2=i;_tmp6E;});Cyc_Absyn_new_exp(_tmp19B,loc);});}
# 558
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=6,_tmp6F->f1=e1,_tmp6F->f2=e2,_tmp6F->f3=e3;_tmp6F;});Cyc_Absyn_new_exp(_tmp19C,loc);});}
# 561
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp19D=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=7,_tmp70->f1=e1,_tmp70->f2=e2;_tmp70;});Cyc_Absyn_new_exp(_tmp19D,loc);});}
# 564
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp19E=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=8,_tmp71->f1=e1,_tmp71->f2=e2;_tmp71;});Cyc_Absyn_new_exp(_tmp19E,loc);});}
# 567
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp19F=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=9,_tmp72->f1=e1,_tmp72->f2=e2;_tmp72;});Cyc_Absyn_new_exp(_tmp19F,loc);});}
# 570
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1A0=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=10,_tmp73->f1=e,_tmp73->f2=es,_tmp73->f3=0,_tmp73->f4=0;_tmp73;});Cyc_Absyn_new_exp(_tmp1A0,loc);});}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1A1=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=10,_tmp74->f1=e,_tmp74->f2=es,_tmp74->f3=0,_tmp74->f4=1;_tmp74;});Cyc_Absyn_new_exp(_tmp1A1,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1A2=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=12,_tmp75->f1=e;_tmp75;});Cyc_Absyn_new_exp(_tmp1A2,loc);});}
# 579
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp1A3=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=13,_tmp76->f1=e,_tmp76->f2=ts;_tmp76;});Cyc_Absyn_new_exp(_tmp1A3,loc);});}
# 582
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp1A4=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=14,_tmp77->f1=t,_tmp77->f2=e,_tmp77->f3=user_cast,_tmp77->f4=c;_tmp77;});Cyc_Absyn_new_exp(_tmp1A4,loc);});}
# 585
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1A5=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=11,_tmp78->f1=e,_tmp78->f2=0;_tmp78;});Cyc_Absyn_new_exp(_tmp1A5,loc);});}
# 588
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1A6=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=11,_tmp79->f1=e,_tmp79->f2=1;_tmp79;});Cyc_Absyn_new_exp(_tmp1A6,loc);});}
# 591
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=15,_tmp7A->f1=e;_tmp7A;});Cyc_Absyn_new_exp(_tmp1A7,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=17,_tmp7B->f1=t;_tmp7B;});Cyc_Absyn_new_exp(_tmp1A8,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1A9=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=18,_tmp7C->f1=e;_tmp7C;});Cyc_Absyn_new_exp(_tmp1A9,loc);});}
# 598
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=19,_tmp7D->f1=t,_tmp7D->f2=ofs;_tmp7D;});Cyc_Absyn_new_exp(_tmp1AA,loc);});}
# 601
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1AB=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=20,_tmp7E->f1=e;_tmp7E;});Cyc_Absyn_new_exp(_tmp1AB,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1AC=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=21,_tmp7F->f1=e,_tmp7F->f2=n,_tmp7F->f3=0,_tmp7F->f4=0;_tmp7F;});Cyc_Absyn_new_exp(_tmp1AC,loc);});}
# 605
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1AD=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=22,_tmp80->f1=e,_tmp80->f2=n,_tmp80->f3=0,_tmp80->f4=0;_tmp80;});Cyc_Absyn_new_exp(_tmp1AD,loc);});}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1AE=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=23,_tmp81->f1=e1,_tmp81->f2=e2;_tmp81;});Cyc_Absyn_new_exp(_tmp1AE,loc);});}
# 611
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1AF=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=24,_tmp82->f1=es;_tmp82;});Cyc_Absyn_new_exp(_tmp1AF,loc);});}
# 614
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1B0=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=37,_tmp83->f1=s;_tmp83;});Cyc_Absyn_new_exp(_tmp1B0,loc);});}
# 617
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp1B1=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=39,_tmp84->f1=t;_tmp84;});Cyc_Absyn_new_exp(_tmp1B1,loc);});}
# 621
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 624
return({void*_tmp1B2=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=40,_tmp85->f1=volatile_kw,_tmp85->f2=tmpl,_tmp85->f3=outs,_tmp85->f4=ins,_tmp85->f5=clobs;_tmp85;});Cyc_Absyn_new_exp(_tmp1B2,loc);});}
# 626
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B3=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=41,_tmp86->f1=e;_tmp86;});Cyc_Absyn_new_exp(_tmp1B3,loc);});}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B4=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=42,_tmp87->f1=e;_tmp87;});Cyc_Absyn_new_exp(_tmp1B4,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 633
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp88=0;struct Cyc_List_List*dles=_tmp88;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*_tmp8A));({struct _tuple13*_tmp1B5=({struct _tuple13*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=0,_tmp89->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp89;});_tmp8A->hd=_tmp1B5;}),_tmp8A->tl=dles;_tmp8A;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);
return({void*_tmp1B6=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=26,_tmp8B->f1=dles;_tmp8B;});Cyc_Absyn_new_exp(_tmp1B6,loc);});}
# 640
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 643
return({void*_tmp1B7=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=36,_tmp8C->f1=n,_tmp8C->f2=dles;_tmp8C;});Cyc_Absyn_new_exp(_tmp1B7,loc);});}
# 646
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->r=s,_tmp8D->loc=loc,_tmp8D->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp8D;});}
# 649
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B8=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=1,_tmp8E->f1=e;_tmp8E;});Cyc_Absyn_new_stmt(_tmp1B8,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B9=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=3,_tmp8F->f1=e;_tmp8F;});Cyc_Absyn_new_stmt(_tmp1B9,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1BB=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1BA=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1BB,_tmp1BA,loc);});}}}struct _tuple14{void*f1;void*f2;};
# 662
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple14 _tmp90=({struct _tuple14 _tmp146;_tmp146.f1=s1->r,_tmp146.f2=s2->r;_tmp146;});struct _tuple14 _stmttmp3=_tmp90;struct _tuple14 _tmp91=_stmttmp3;if(*((int*)_tmp91.f1)== 0){_LL1: _LL2:
 return s2;}else{if(*((int*)_tmp91.f2)== 0){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1BC=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=2,_tmp92->f1=s1,_tmp92->f2=s2;_tmp92;});Cyc_Absyn_new_stmt(_tmp1BC,loc);});}}_LL0:;}
# 669
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp1BD=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=4,_tmp93->f1=e,_tmp93->f2=s1,_tmp93->f3=s2;_tmp93;});Cyc_Absyn_new_stmt(_tmp1BD,loc);});}
# 672
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1BF=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=5,(_tmp94->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1BE=Cyc_Absyn_skip_stmt(e->loc);(_tmp94->f1).f2=_tmp1BE;}),_tmp94->f2=s;_tmp94;});Cyc_Absyn_new_stmt(_tmp1BF,loc);});}
# 675
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1C2=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=9,_tmp95->f1=e1,(_tmp95->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1C1=Cyc_Absyn_skip_stmt(e3->loc);(_tmp95->f2).f2=_tmp1C1;}),
(_tmp95->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1C0=Cyc_Absyn_skip_stmt(e3->loc);(_tmp95->f3).f2=_tmp1C0;}),_tmp95->f4=s;_tmp95;});
# 676
Cyc_Absyn_new_stmt(_tmp1C2,loc);});}
# 680
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1C4=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=14,_tmp96->f1=s,(_tmp96->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1C3=Cyc_Absyn_skip_stmt(e->loc);(_tmp96->f2).f2=_tmp1C3;});_tmp96;});Cyc_Absyn_new_stmt(_tmp1C4,loc);});}
# 683
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1C5=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=10,_tmp97->f1=e,_tmp97->f2=scs,_tmp97->f3=0;_tmp97;});Cyc_Absyn_new_stmt(_tmp1C5,loc);});}
# 686
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1C6=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=15,_tmp98->f1=s,_tmp98->f2=scs,_tmp98->f3=0;_tmp98;});Cyc_Absyn_new_stmt(_tmp1C6,loc);});}
# 689
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp1C7=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=11,_tmp99->f1=el,_tmp99->f2=0;_tmp99;});Cyc_Absyn_new_stmt(_tmp1C7,loc);});}
# 692
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
return({void*_tmp1C8=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=8,_tmp9A->f1=lab;_tmp9A;});Cyc_Absyn_new_stmt(_tmp1C8,loc);});}
# 695
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1C9=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=13,_tmp9B->f1=v,_tmp9B->f2=s;_tmp9B;});Cyc_Absyn_new_stmt(_tmp1C9,loc);});}
# 698
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1CA=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=12,_tmp9C->f1=d,_tmp9C->f2=s;_tmp9C;});Cyc_Absyn_new_stmt(_tmp1CA,loc);});}
# 701
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1CC=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=0,({struct Cyc_Absyn_Vardecl*_tmp1CB=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp9E->f1=_tmp1CB;});_tmp9E;});Cyc_Absyn_new_decl(_tmp1CC,loc);});
return({void*_tmp1CD=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=12,_tmp9D->f1=d,_tmp9D->f2=s;_tmp9D;});Cyc_Absyn_new_stmt(_tmp1CD,loc);});}
# 705
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp1CE=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1CE,loc);});}
# 709
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->r=p,_tmp9F->topt=0,_tmp9F->loc=s;_tmp9F;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1CF=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=17,_tmpA0->f1=e;_tmpA0;});Cyc_Absyn_new_pat(_tmp1CF,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9};
# 715
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->r=r,_tmpA1->loc=loc;_tmpA1;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1D0=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=2,_tmpA2->f1=p,_tmpA2->f2=0,_tmpA2->f3=e,_tmpA2->f4=0;_tmpA2;});Cyc_Absyn_new_decl(_tmp1D0,loc);});}
# 719
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp1D1=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=3,_tmpA3->f1=vds;_tmpA3;});Cyc_Absyn_new_decl(_tmp1D1,loc);});}
# 722
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp1D2=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=4,_tmpA4->f1=tv,_tmpA4->f2=vd,_tmpA4->f3=open_exp;_tmpA4;});Cyc_Absyn_new_decl(_tmp1D2,loc);});}
# 725
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 727
return({void*_tmp1D5=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=2,({struct Cyc_Absyn_Pat*_tmp1D4=({void*_tmp1D3=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=2,_tmpA5->f1=tv,_tmpA5->f2=vd;_tmpA5;});Cyc_Absyn_new_pat(_tmp1D3,loc);});_tmpA6->f1=_tmp1D4;}),_tmpA6->f2=0,_tmpA6->f3=e,_tmpA6->f4=0;_tmpA6;});Cyc_Absyn_new_decl(_tmp1D5,loc);});}
# 730
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->sc=Cyc_Absyn_Public,_tmpA7->name=x,_tmpA7->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1D6=Cyc_Absyn_empty_tqual(0U);_tmpA7->tq=_tmp1D6;}),_tmpA7->type=t,_tmpA7->initializer=init,_tmpA7->rgn=0,_tmpA7->attributes=0,_tmpA7->escapes=0,_tmpA7->is_proto=0;_tmpA7;});}
# 737
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->sc=Cyc_Absyn_Static,_tmpA8->name=x,_tmpA8->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1D7=Cyc_Absyn_empty_tqual(0U);_tmpA8->tq=_tmp1D7;}),_tmpA8->type=t,_tmpA8->initializer=init,_tmpA8->rgn=0,_tmpA8->attributes=0,_tmpA8->escapes=0,_tmpA8->is_proto=0;_tmpA8;});}
# 743
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 747
return({struct Cyc_Absyn_AggrdeclImpl*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->exist_vars=exists,_tmpA9->rgn_po=po,_tmpA9->fields=fs,_tmpA9->tagged=tagged;_tmpA9;});}
# 750
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 753
return({void*_tmp1D9=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=5,({struct Cyc_Absyn_Aggrdecl*_tmp1D8=({struct Cyc_Absyn_Aggrdecl*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->kind=k,_tmpAA->sc=s,_tmpAA->name=n,_tmpAA->tvs=ts,_tmpAA->impl=i,_tmpAA->attributes=atts,_tmpAA->expected_mem_kind=0;_tmpAA;});_tmpAB->f1=_tmp1D8;});_tmpAB;});Cyc_Absyn_new_decl(_tmp1D9,loc);});}
# 758
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 761
return({struct Cyc_Absyn_TypeDecl*_tmpAE=_cycalloc(sizeof(*_tmpAE));({void*_tmp1DB=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=0,({struct Cyc_Absyn_Aggrdecl*_tmp1DA=({struct Cyc_Absyn_Aggrdecl*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->kind=k,_tmpAC->sc=s,_tmpAC->name=n,_tmpAC->tvs=ts,_tmpAC->impl=i,_tmpAC->attributes=atts,_tmpAC->expected_mem_kind=0;_tmpAC;});_tmpAD->f1=_tmp1DA;});_tmpAD;});_tmpAE->r=_tmp1DB;}),_tmpAE->loc=loc;_tmpAE;});}
# 767
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 770
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 772
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 775
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 777
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 780
return({void*_tmp1DD=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=6,({struct Cyc_Absyn_Datatypedecl*_tmp1DC=({struct Cyc_Absyn_Datatypedecl*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->sc=s,_tmpAF->name=n,_tmpAF->tvs=ts,_tmpAF->fields=fs,_tmpAF->is_extensible=is_extensible;_tmpAF;});_tmpB0->f1=_tmp1DC;});_tmpB0;});Cyc_Absyn_new_decl(_tmp1DD,loc);});}
# 783
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 786
return({struct Cyc_Absyn_TypeDecl*_tmpB3=_cycalloc(sizeof(*_tmpB3));({void*_tmp1DF=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=2,({struct Cyc_Absyn_Datatypedecl*_tmp1DE=({struct Cyc_Absyn_Datatypedecl*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->sc=s,_tmpB1->name=n,_tmpB1->tvs=ts,_tmpB1->fields=fs,_tmpB1->is_extensible=is_extensible;_tmpB1;});_tmpB2->f1=_tmp1DE;});_tmpB2;});_tmpB3->r=_tmp1DF;}),_tmpB3->loc=loc;_tmpB3;});}
# 798 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 804
{struct Cyc_List_List*_tmpB4=args;struct Cyc_List_List*args2=_tmpB4;for(0;args2 != 0;args2=args2->tl){
({void*_tmp1E0=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f3,1);(*((struct _tuple8*)args2->hd)).f3=_tmp1E0;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=5,(_tmpB5->f1).tvars=tvs,(_tmpB5->f1).ret_tqual=ret_tqual,({
# 808
void*_tmp1E1=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpB5->f1).ret_type=_tmp1E1;}),(_tmpB5->f1).effect=eff_type,(_tmpB5->f1).args=args,(_tmpB5->f1).c_varargs=c_varargs,(_tmpB5->f1).cyc_varargs=cyc_varargs,(_tmpB5->f1).rgn_po=rgn_po,(_tmpB5->f1).attributes=atts,(_tmpB5->f1).requires_clause=req,(_tmpB5->f1).requires_relns=0,(_tmpB5->f1).ensures_clause=ens,(_tmpB5->f1).ensures_relns=0,(_tmpB5->f1).return_value=0;_tmpB5;});}
# 823
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpB6=Cyc_Absyn_compress(t);void*_stmttmp4=_tmpB6;void*_tmpB7=_stmttmp4;if(*((int*)_tmpB7)== 5){_LL1: _LL2:
 return({void*_tmp1E5=t;void*_tmp1E4=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->v=& Cyc_Kinds_rk;_tmpB8;}),0): Cyc_Absyn_heap_rgn_type;
# 825
struct Cyc_Absyn_Tqual _tmp1E3=
# 829
Cyc_Absyn_empty_tqual(0U);
# 825
void*_tmp1E2=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp1E5,_tmp1E4,_tmp1E3,_tmp1E2,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 830
 return t;}_LL0:;}
# 845 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpB9=e->r;void*_stmttmp5=_tmpB9;void*_tmpBA=_stmttmp5;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;struct Cyc_Absyn_Vardecl*_tmpBF;switch(*((int*)_tmpBA)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBA)->f1)){case 2: _LL1: _LL2:
 return 0;case 1: _LL3: _tmpBF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBA)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpBF;
_tmpBE=vd;goto _LL6;}case 4: _LL5: _tmpBE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBA)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpBE;
# 850
void*_tmpC0=Cyc_Absyn_compress(vd->type);void*_stmttmp6=_tmpC0;void*_tmpC1=_stmttmp6;if(*((int*)_tmpC1)== 4){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 854
 goto _LLA;}case 22: _LL9: _LLA:
 goto _LLC;case 20: _LLB: _LLC:
 goto _LLE;case 23: _LLD: _LLE:
 return 1;case 21: _LLF: _tmpBD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpBD;
return Cyc_Absyn_is_lvalue(e1);}case 13: _LL11: _tmpBC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpBC;
return Cyc_Absyn_is_lvalue(e1);}case 12: _LL13: _tmpBB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBA)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpBB;
return Cyc_Absyn_is_lvalue(e1);}default: _LL15: _LL16:
 return 0;}_LL0:;}
# 865
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*_tmpC2=fields;struct Cyc_List_List*fs=_tmpC2;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 871
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 875
struct _tuple15*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(1;i != 0 && ts != 0;(-- i,ts=ts->tl)){
;}
return ts == 0?0:(struct _tuple15*)ts->hd;}
# 881
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_tmpC3=decl->r;void*_stmttmp7=_tmpC3;void*_tmpC4=_stmttmp7;struct Cyc_Absyn_Fndecl*_tmpC5;struct Cyc_Absyn_Vardecl*_tmpC6;struct Cyc_Absyn_Typedefdecl*_tmpC7;struct Cyc_Absyn_Enumdecl*_tmpC8;struct Cyc_Absyn_Aggrdecl*_tmpC9;switch(*((int*)_tmpC4)){case 5: _LL1: _tmpC9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*x=_tmpC9;
return(*x->name).f2;}case 7: _LL3: _tmpC8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL4: {struct Cyc_Absyn_Enumdecl*x=_tmpC8;
return(*x->name).f2;}case 8: _LL5: _tmpC7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*x=_tmpC7;
return(*x->name).f2;}case 0: _LL7: _tmpC6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmpC6;
return(*x->name).f2;}case 1: _LL9: _tmpC5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC4)->f1;_LLA: {struct Cyc_Absyn_Fndecl*x=_tmpC5;
return(*x->name).f2;}case 13: _LLB: _LLC:
 goto _LLE;case 14: _LLD: _LLE:
 goto _LL10;case 15: _LLF: _LL10:
 goto _LL12;case 16: _LL11: _LL12:
 goto _LL14;case 2: _LL13: _LL14:
 goto _LL16;case 6: _LL15: _LL16:
 goto _LL18;case 3: _LL17: _LL18:
 goto _LL1A;case 9: _LL19: _LL1A:
 goto _LL1C;case 10: _LL1B: _LL1C:
 goto _LL1E;case 11: _LL1D: _LL1E:
 goto _LL20;case 12: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return 0;}_LL0:;}
# 904
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
for(1;decls != 0;decls=decls->tl){
struct _fat_ptr*_tmpCA=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);struct _fat_ptr*dname=_tmpCA;
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 910
return 0;}static char _tmpD1[3U]="f0";
# 913
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 915
static struct _fat_ptr f0={_tmpD1,_tmpD1,_tmpD1 + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)field_names_v,(void*)field_names_v,(void*)(field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmpD0=(unsigned)(i + 1);struct _fat_ptr**_tmpCF=_cycalloc(_check_times(_tmpD0,sizeof(struct _fat_ptr*)));({{unsigned _tmp147=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp147;++ j){
# 922
j < fsz?_tmpCF[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_tmp1E8=({struct _fat_ptr*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct _fat_ptr _tmp1E7=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpCD=({struct Cyc_Int_pa_PrintArg_struct _tmp148;_tmp148.tag=1,_tmp148.f1=(unsigned long)((int)j);_tmp148;});void*_tmpCB[1];_tmpCB[0]=& _tmpCD;({struct _fat_ptr _tmp1E6=({const char*_tmpCC="f%d";_tag_fat(_tmpCC,sizeof(char),4U);});Cyc_aprintf(_tmp1E6,_tag_fat(_tmpCB,sizeof(void*),1));});});*_tmpCE=_tmp1E7;});_tmpCE;});_tmpCF[j]=_tmp1E8;});}}0;});_tag_fat(_tmpCF,sizeof(struct _fat_ptr*),_tmpD0);});
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple16{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 926
struct _tuple16 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpD2=info;struct _tuple0*_tmpD4;enum Cyc_Absyn_AggrKind _tmpD3;struct _tuple0*_tmpD6;enum Cyc_Absyn_AggrKind _tmpD5;if((_tmpD2.UnknownAggr).tag == 1){_LL1: _tmpD5=((_tmpD2.UnknownAggr).val).f1;_tmpD6=((_tmpD2.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind ak=_tmpD5;struct _tuple0*n=_tmpD6;
return({struct _tuple16 _tmp149;_tmp149.f1=ak,_tmp149.f2=n;_tmp149;});}}else{_LL3: _tmpD3=(*(_tmpD2.KnownAggr).val)->kind;_tmpD4=(*(_tmpD2.KnownAggr).val)->name;_LL4: {enum Cyc_Absyn_AggrKind k=_tmpD3;struct _tuple0*n=_tmpD4;
return({struct _tuple16 _tmp14A;_tmp14A.f1=k,_tmp14A.f2=n;_tmp14A;});}}_LL0:;}
# 932
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpD7=info;struct Cyc_Absyn_Aggrdecl*_tmpD8;if((_tmpD7.UnknownAggr).tag == 1){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14B;_tmp14B.tag=0,({struct _fat_ptr _tmp1E9=({const char*_tmpDB="unchecked aggrdecl";_tag_fat(_tmpDB,sizeof(char),19U);});_tmp14B.f1=_tmp1E9;});_tmp14B;});void*_tmpD9[1];_tmpD9[0]=& _tmpDA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD9,sizeof(void*),1));});}else{_LL3: _tmpD8=*(_tmpD7.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmpD8;
return ad;}}_LL0:;}
# 938
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmpDC=Cyc_Absyn_compress(t);void*_stmttmp8=_tmpDC;void*_tmpDD=_stmttmp8;union Cyc_Absyn_AggrInfo _tmpDE;struct Cyc_List_List*_tmpDF;switch(*((int*)_tmpDD)){case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDD)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpDF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDD)->f2;_LL2: {struct Cyc_List_List*fs=_tmpDF;
# 941
return fs == 0 ||((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause == 0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDD)->f1)== 20){_LL3: _tmpDE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpDD)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpDE;
# 943
union Cyc_Absyn_AggrInfo _tmpE0=info;int _tmpE2;enum Cyc_Absyn_AggrKind _tmpE1;enum Cyc_Absyn_AggrKind _tmpE3;struct Cyc_Absyn_Aggrdecl*_tmpE4;if((_tmpE0.KnownAggr).tag == 2){_LL8: _tmpE4=*(_tmpE0.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpE4;
# 945
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpE5=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpE5;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpE6=impl->fields;struct Cyc_List_List*fields=_tmpE6;
return fields == 0 ||((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause == 0;}}}}else{if(((_tmpE0.UnknownAggr).val).f3 == 0){_LLA: _tmpE3=((_tmpE0.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpE3;
return(int)k == (int)Cyc_Absyn_UnionA;}}else{_LLC: _tmpE1=((_tmpE0.UnknownAggr).val).f1;_tmpE2=(int)(((_tmpE0.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpE1;int b=_tmpE2;
return(int)k == (int)1U && !b;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 953
 return 0;}_LL0:;}
# 956
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmpE7=Cyc_Absyn_compress(t);void*_stmttmp9=_tmpE7;void*_tmpE8=_stmttmp9;union Cyc_Absyn_AggrInfo _tmpE9;struct Cyc_List_List*_tmpEA;switch(*((int*)_tmpE8)){case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE8)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpEA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE8)->f2;_LL2: {struct Cyc_List_List*fs=_tmpEA;
# 959
return fs != 0 &&((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause != 0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE8)->f1)== 20){_LL3: _tmpE9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE8)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpE9;
# 961
union Cyc_Absyn_AggrInfo _tmpEB=info;enum Cyc_Absyn_AggrKind _tmpEC;struct Cyc_Absyn_Aggrdecl*_tmpED;if((_tmpEB.KnownAggr).tag == 2){_LL8: _tmpED=*(_tmpEB.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpED;
# 963
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpEE=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpEE;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpEF=impl->fields;struct Cyc_List_List*fields=_tmpEF;
return fields != 0 &&((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause != 0;}}}}else{_LLA: _tmpEC=((_tmpEB.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpEC;
return 0;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 970
 return 0;}_LL0:;}
# 974
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmpF0=b;struct Cyc_Absyn_Fndecl*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpF2;struct Cyc_Absyn_Vardecl*_tmpF3;struct Cyc_Absyn_Vardecl*_tmpF4;struct Cyc_Absyn_Vardecl*_tmpF5;struct _tuple0*_tmpF6;switch(*((int*)_tmpF0)){case 0: _LL1: _tmpF6=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF0)->f1;_LL2: {struct _tuple0*qv=_tmpF6;
return qv;}case 1: _LL3: _tmpF5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF0)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpF5;
_tmpF4=vd;goto _LL6;}case 3: _LL5: _tmpF4=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpF0)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpF4;
_tmpF3=vd;goto _LL8;}case 4: _LL7: _tmpF3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpF0)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpF3;
_tmpF2=vd;goto _LLA;}case 5: _LL9: _tmpF2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpF0)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpF2;
return vd->name;}default: _LLB: _tmpF1=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpF0)->f1;_LLC: {struct Cyc_Absyn_Fndecl*fd=_tmpF1;
return fd->name;}}_LL0:;}
# 985
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14C;_tmp14C.tag=0,({struct _fat_ptr _tmp1EA=({const char*_tmpF9="designator list not of length 1";_tag_fat(_tmpF9,sizeof(char),32U);});_tmp14C.f1=_tmp1EA;});_tmp14C;});void*_tmpF7[1];_tmpF7[0]=& _tmpF8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF7,sizeof(void*),1));});{
void*_tmpFA=(void*)ds->hd;void*_stmttmpA=_tmpFA;void*_tmpFB=_stmttmpA;struct _fat_ptr*_tmpFC;if(*((int*)_tmpFB)== 1){_LL1: _tmpFC=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpFB)->f1;_LL2: {struct _fat_ptr*f=_tmpFC;
return f;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpFE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14D;_tmp14D.tag=0,({struct _fat_ptr _tmp1EB=({const char*_tmpFF="array designator in struct";_tag_fat(_tmpFF,sizeof(char),27U);});_tmp14D.f1=_tmp1EB;});_tmp14D;});void*_tmpFD[1];_tmpFD[0]=& _tmpFE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpFD,sizeof(void*),1));});}_LL0:;}}
# 994
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp100=Cyc_Absyn_compress(t);void*_stmttmpB=_tmp100;void*_tmp101=_stmttmpB;if(*((int*)_tmp101)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp101)->f1)){case 11: _LL1: _LL2:
 return 1;case 12: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1005
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
void*_tmp102=e->r;void*_stmttmpC=_tmp102;void*_tmp103=_stmttmpC;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;switch(*((int*)_tmp103)){case 10: _LL1: _LL2:
 goto _LL4;case 4: _LL3: _LL4:
 goto _LL6;case 35: _LL5: _LL6:
 goto _LL8;case 40: _LL7: _LL8:
 goto _LLA;case 37: _LL9: _LLA:
 goto _LLC;case 5: _LLB: _LLC:
 return 0;case 39: _LLD: _LLE:
# 1014
 goto _LL10;case 0: _LLF: _LL10:
 goto _LL12;case 1: _LL11: _LL12:
 goto _LL14;case 17: _LL13: _LL14:
 goto _LL16;case 19: _LL15: _LL16:
 goto _LL18;case 32: _LL17: _LL18:
 goto _LL1A;case 33: _LL19: _LL1A:
 goto _LL1C;case 2: _LL1B: _LL1C:
 return 1;case 6: _LL1D: _tmp127=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_tmp128=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_tmp129=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp103)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp127;struct Cyc_Absyn_Exp*e2=_tmp128;struct Cyc_Absyn_Exp*e3=_tmp129;
# 1024
if(!Cyc_Absyn_no_side_effects_exp(e1))return 0;
_tmp125=e2;_tmp126=e3;goto _LL20;}case 27: _LL1F: _tmp125=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_tmp126=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp103)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp125;struct Cyc_Absyn_Exp*e2=_tmp126;
_tmp123=e1;_tmp124=e2;goto _LL22;}case 7: _LL21: _tmp123=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_tmp124=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp123;struct Cyc_Absyn_Exp*e2=_tmp124;
_tmp121=e1;_tmp122=e2;goto _LL24;}case 8: _LL23: _tmp121=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_tmp122=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp121;struct Cyc_Absyn_Exp*e2=_tmp122;
_tmp11F=e1;_tmp120=e2;goto _LL26;}case 23: _LL25: _tmp11F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_tmp120=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp11F;struct Cyc_Absyn_Exp*e2=_tmp120;
_tmp11D=e1;_tmp11E=e2;goto _LL28;}case 9: _LL27: _tmp11D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_tmp11E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp11D;struct Cyc_Absyn_Exp*e2=_tmp11E;
# 1031
return Cyc_Absyn_no_side_effects_exp(e1)&& Cyc_Absyn_no_side_effects_exp(e2);}case 41: _LL29: _tmp11C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp11C;
# 1033
_tmp11B=e;goto _LL2C;}case 38: _LL2B: _tmp11B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp11B;
_tmp11A=e;goto _LL2E;}case 11: _LL2D: _tmp11A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp11A;
_tmp119=e;goto _LL30;}case 12: _LL2F: _tmp119=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp119;
_tmp118=e;goto _LL32;}case 13: _LL31: _tmp118=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp118;
_tmp117=e;goto _LL34;}case 14: _LL33: _tmp117=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp117;
_tmp116=e;goto _LL36;}case 18: _LL35: _tmp116=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp116;
_tmp115=e;goto _LL38;}case 20: _LL37: _tmp115=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp115;
_tmp114=e;goto _LL3A;}case 21: _LL39: _tmp114=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp114;
_tmp113=e;goto _LL3C;}case 22: _LL3B: _tmp113=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp113;
_tmp112=e;goto _LL3E;}case 28: _LL3D: _tmp112=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp112;
_tmp111=e;goto _LL40;}case 42: _LL3F: _tmp111=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp111;
_tmp110=e;goto _LL42;}case 15: _LL41: _tmp110=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL42: {struct Cyc_Absyn_Exp*e=_tmp110;
return Cyc_Absyn_no_side_effects_exp(e);}case 34: _LL43: _tmp10E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp103)->f1).rgn;_tmp10F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp103)->f1).num_elts;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp10E;struct Cyc_Absyn_Exp*e=_tmp10F;
# 1047
_tmp10C=eopt;_tmp10D=e;goto _LL46;}case 16: _LL45: _tmp10C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_tmp10D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL46: {struct Cyc_Absyn_Exp*eopt=_tmp10C;struct Cyc_Absyn_Exp*e=_tmp10D;
# 1049
if(!Cyc_Absyn_no_side_effects_exp(e))return 0;
if(eopt != 0)return Cyc_Absyn_no_side_effects_exp(eopt);
return 1;}case 3: _LL47: _tmp10B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL48: {struct Cyc_List_List*es=_tmp10B;
# 1053
_tmp10A=es;goto _LL4A;}case 31: _LL49: _tmp10A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL4A: {struct Cyc_List_List*es=_tmp10A;
_tmp109=es;goto _LL4C;}case 24: _LL4B: _tmp109=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL4C: {struct Cyc_List_List*es=_tmp109;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall)(Cyc_Absyn_no_side_effects_exp,es);}case 36: _LL4D: _tmp108=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL4E: {struct Cyc_List_List*dles=_tmp108;
# 1057
_tmp107=dles;goto _LL50;}case 29: _LL4F: _tmp107=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp103)->f3;_LL50: {struct Cyc_List_List*dles=_tmp107;
_tmp106=dles;goto _LL52;}case 30: _LL51: _tmp106=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL52: {struct Cyc_List_List*dles=_tmp106;
_tmp105=dles;goto _LL54;}case 26: _LL53: _tmp105=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp103)->f1;_LL54: {struct Cyc_List_List*dles=_tmp105;
_tmp104=dles;goto _LL56;}default: _LL55: _tmp104=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp103)->f2;_LL56: {struct Cyc_List_List*dles=_tmp104;
# 1062
for(1;dles != 0;dles=dles->tl){
if(!Cyc_Absyn_no_side_effects_exp((*((struct _tuple13*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}
# 1069
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16};
