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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 864 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 880
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 972
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);struct Cyc_Hashtable_Table;struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 61 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 63
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
void*Cyc_AssnDef_zero (void);
# 70
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
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
# 123
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 129
void*Cyc_AssnDef_reduce(void*);
# 131
void*Cyc_AssnDef_not(void*);
# 28 "pratt_prover.cyc"
enum Cyc_PrattProver_DistanceInfo{Cyc_PrattProver_Unsigned_valid =1,Cyc_PrattProver_Signed_valid =2,Cyc_PrattProver_Both_valid =3};struct Cyc_PrattProver_Dist{int length;void*prim;};struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;void*target;enum Cyc_PrattProver_DistanceInfo dinfo;struct Cyc_PrattProver_Dist unsigned_dist;struct Cyc_PrattProver_Dist signed_dist;};struct Cyc_PrattProver_Row{struct Cyc_PrattProver_Row*next;void*source;struct Cyc_PrattProver_Distance*distance;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Row*rows;};
# 66 "pratt_prover.cyc"
static void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*gs){
({void*_tmp0=0U;({struct Cyc___cycFILE*_tmpAE=Cyc_stderr;struct _fat_ptr _tmpAD=({const char*_tmp1="Graphs:\n";_tag_fat(_tmp1,sizeof(char),9U);});Cyc_fprintf(_tmpAE,_tmpAD,_tag_fat(_tmp0,sizeof(void*),0));});});
for(1;gs != 0;gs=gs->next){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmpB0=Cyc_stderr;struct _fat_ptr _tmpAF=({const char*_tmp3="\t{";_tag_fat(_tmp3,sizeof(char),3U);});Cyc_fprintf(_tmpB0,_tmpAF,_tag_fat(_tmp2,sizeof(void*),0));});});{
struct Cyc_PrattProver_Row*_tmp4=gs->rows;struct Cyc_PrattProver_Row*ns=_tmp4;for(0;ns != 0;ns=ns->next){
void*_tmp5=ns->source;void*t1=_tmp5;
{struct Cyc_PrattProver_Distance*_tmp6=ns->distance;struct Cyc_PrattProver_Distance*ds=_tmp6;for(0;ds != 0;ds=ds->next){
void*_tmp7=ds->target;void*t2=_tmp7;
if((int)ds->dinfo == (int)Cyc_PrattProver_Both_valid)
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp9E;_tmp9E.tag=0,({struct _fat_ptr _tmpB1=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp9E.f1=_tmpB1;});_tmp9E;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmp9D;_tmp9D.tag=0,({
struct _fat_ptr _tmpB2=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp9D.f1=_tmpB2;});_tmp9D;});struct Cyc_Int_pa_PrintArg_struct _tmpC=({struct Cyc_Int_pa_PrintArg_struct _tmp9C;_tmp9C.tag=1,_tmp9C.f1=(unsigned long)(ds->unsigned_dist).length;_tmp9C;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp9B;_tmp9B.tag=0,({struct _fat_ptr _tmpB3=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp9B.f1=_tmpB3;});_tmp9B;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp9A;_tmp9A.tag=0,({
struct _fat_ptr _tmpB4=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp9A.f1=_tmpB4;});_tmp9A;});struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp99;_tmp99.tag=1,_tmp99.f1=(unsigned long)(ds->signed_dist).length;_tmp99;});void*_tmp8[6];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB,_tmp8[2]=& _tmpC,_tmp8[3]=& _tmpD,_tmp8[4]=& _tmpE,_tmp8[5]=& _tmpF;({struct Cyc___cycFILE*_tmpB6=Cyc_stderr;struct _fat_ptr _tmpB5=({const char*_tmp9="%s - %s U<= %d, %s - %s S<= %d";_tag_fat(_tmp9,sizeof(char),31U);});Cyc_fprintf(_tmpB6,_tmpB5,_tag_fat(_tmp8,sizeof(void*),6));});});else{
if((int)ds->dinfo == (int)Cyc_PrattProver_Signed_valid)
({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0,({struct _fat_ptr _tmpB7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpA1.f1=_tmpB7;});_tmpA1;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0,({
struct _fat_ptr _tmpB8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpA0.f1=_tmpB8;});_tmpA0;});struct Cyc_Int_pa_PrintArg_struct _tmp14=({struct Cyc_Int_pa_PrintArg_struct _tmp9F;_tmp9F.tag=1,_tmp9F.f1=(unsigned long)(ds->signed_dist).length;_tmp9F;});void*_tmp10[3];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13,_tmp10[2]=& _tmp14;({struct Cyc___cycFILE*_tmpBA=Cyc_stderr;struct _fat_ptr _tmpB9=({const char*_tmp11="%s - %s S<= %d";_tag_fat(_tmp11,sizeof(char),15U);});Cyc_fprintf(_tmpBA,_tmpB9,_tag_fat(_tmp10,sizeof(void*),3));});});else{
# 82
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmpA4;_tmpA4.tag=0,({struct _fat_ptr _tmpBB=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpA4.f1=_tmpBB;});_tmpA4;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmpA3;_tmpA3.tag=0,({
struct _fat_ptr _tmpBC=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpA3.f1=_tmpBC;});_tmpA3;});struct Cyc_Int_pa_PrintArg_struct _tmp19=({struct Cyc_Int_pa_PrintArg_struct _tmpA2;_tmpA2.tag=1,_tmpA2.f1=(unsigned long)(ds->unsigned_dist).length;_tmpA2;});void*_tmp15[3];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19;({struct Cyc___cycFILE*_tmpBE=Cyc_stderr;struct _fat_ptr _tmpBD=({const char*_tmp16="%s - %s U<= %d";_tag_fat(_tmp16,sizeof(char),15U);});Cyc_fprintf(_tmpBE,_tmpBD,_tag_fat(_tmp15,sizeof(void*),3));});});}}
# 85
if(ds->next != 0)({void*_tmp1A=0U;({struct Cyc___cycFILE*_tmpC0=Cyc_stderr;struct _fat_ptr _tmpBF=({const char*_tmp1B=",";_tag_fat(_tmp1B,sizeof(char),2U);});Cyc_fprintf(_tmpC0,_tmpBF,_tag_fat(_tmp1A,sizeof(void*),0));});});}}
# 87
({void*_tmp1C=0U;({struct Cyc___cycFILE*_tmpC2=Cyc_stderr;struct _fat_ptr _tmpC1=({const char*_tmp1D="}\n";_tag_fat(_tmp1D,sizeof(char),3U);});Cyc_fprintf(_tmpC2,_tmpC1,_tag_fat(_tmp1C,sizeof(void*),0));});});}}}}
# 93
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned n=0U;
for(1;gs != 0;gs=gs->next){
++ n;}
return n;}
# 102
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){return({struct Cyc_PrattProver_Graph*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->next=0,_tmp1E->rows=0;_tmp1E;});}
# 105
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distance(struct Cyc_PrattProver_Distance*ds){
struct Cyc_PrattProver_Distance*res=0;
for(1;ds != 0;ds=ds->next){
struct Cyc_PrattProver_Distance*_tmp1F=({struct Cyc_PrattProver_Distance*_tmp20=_cycalloc(sizeof(*_tmp20));*_tmp20=*ds;_tmp20;});struct Cyc_PrattProver_Distance*newds=_tmp1F;
newds->next=res;
res=newds;}
# 112
return res;}
# 115
static struct Cyc_PrattProver_Row*Cyc_PrattProver_copy_rows(struct Cyc_PrattProver_Row*ns){
struct Cyc_PrattProver_Row*res=0;
for(1;ns != 0;ns=ns->next){
res=({struct Cyc_PrattProver_Row*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->next=res,_tmp21->source=ns->source,({
# 120
struct Cyc_PrattProver_Distance*_tmpC3=Cyc_PrattProver_copy_distance(ns->distance);_tmp21->distance=_tmpC3;});_tmp21;});}
# 122
return res;}
# 126
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graph(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Graph*res=0;
for(1;g != 0;g=g->next){
res=({struct Cyc_PrattProver_Graph*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->next=res,({struct Cyc_PrattProver_Row*_tmpC4=Cyc_PrattProver_copy_rows(g->rows);_tmp22->rows=_tmpC4;});_tmp22;});}
# 131
return res;}
# 136
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1 == 0)return g2;
if(g2 == 0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*_tmp23=p->next;struct Cyc_PrattProver_Graph*x=_tmp23;for(0;x != 0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}
# 145
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*,int,void*,void*,int,void*);
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
# 148
void*Cyc_PrattProver_get_term_type(void*t){
void*_tmp24=t;void*_tmp25;switch(*((int*)_tmp24)){case 0: _tmp25=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp24)->f1;{struct Cyc_Absyn_Exp*e=_tmp25;
return e->topt;}case 1: _tmp25=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp24)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp25;
return vd->type;}case 2: _tmp25=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp24)->f3;{void*topt=_tmp25;
return topt;}case 3:
# 154
 return 0;default: _tmp25=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp24)->f1;{void*type=_tmp25;
return type;}};}
# 159
static void Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*,void*);
# 161
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
Cyc_PrattProver_add_node(g,t2);
# 164
Cyc_PrattProver_add_constraint(g,t1,Cyc_AssnDef_ULte,t2);
Cyc_PrattProver_add_constraint(g,t1,Cyc_AssnDef_SLte,t2);
Cyc_PrattProver_add_constraint(g,t2,Cyc_AssnDef_ULte,t1);
Cyc_PrattProver_add_constraint(g,t2,Cyc_AssnDef_SLte,t1);}
# 173
static void Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*n){
void*_tmp26=Cyc_PrattProver_get_term_type(n);void*topt=_tmp26;
if(topt != 0){
void*t=topt;
struct Cyc_Absyn_Exp*_tmp27=Cyc_Tcutil_get_type_bound(t);struct Cyc_Absyn_Exp*eopt=_tmp27;
if(eopt != 0){
void*t1=Cyc_AssnDef_primop(Cyc_Absyn_Numelts,({void*_tmp2F[1];_tmp2F[0]=n;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2F,sizeof(void*),1));}));
# 181
struct Cyc_Absyn_Exp*e=eopt;
loop: {
void*_tmp28=e->r;void*_stmttmp0=_tmp28;void*_tmp29=_stmttmp0;void*_tmp2B;void*_tmp2A;if(*((int*)_tmp29)== 14){_tmp2A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp29)->f1;_tmp2B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp29)->f2;{void*tp2=_tmp2A;struct Cyc_Absyn_Exp*e2=_tmp2B;
# 185
{void*_tmp2C=Cyc_Absyn_compress(tp2);void*_stmttmp1=_tmp2C;void*_tmp2D=_stmttmp1;enum Cyc_Absyn_Size_of _tmp2E;if(*((int*)_tmp2D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D)->f1)== 1){_tmp2E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D)->f1)->f2;{enum Cyc_Absyn_Size_of s=_tmp2E;
# 187
if((int)s == (int)2U ||(int)s == (int)3U){
e=e2;goto loop;}
# 190
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 193
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 196
void*t2=Cyc_AssnDef_cnst(e);
Cyc_PrattProver_add_eq(g,t1,t2);}}{
# 199
void*_tmp30=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp30;void*_tmp31=_stmttmp2;void*_tmp32;if(*((int*)_tmp31)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2 != 0){_tmp32=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2)->hd;{void*v=_tmp32;
# 201
{void*_tmp33=Cyc_Absyn_compress(v);void*_stmttmp3=_tmp33;void*_tmp34=_stmttmp3;void*_tmp35;if(*((int*)_tmp34)== 9){_tmp35=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp34)->f1;{struct Cyc_Absyn_Exp*e=_tmp35;
({struct Cyc_PrattProver_Graph*_tmpC6=g;void*_tmpC5=n;Cyc_PrattProver_add_eq(_tmpC6,_tmpC5,Cyc_AssnDef_cnst(e));});goto _LLF;}}else{_tmp35=_tmp34;{void*v2=_tmp35;
({struct Cyc_PrattProver_Graph*_tmpC8=g;void*_tmpC7=n;Cyc_PrattProver_add_eq(_tmpC8,_tmpC7,Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U)));});goto _LLF;}}_LLF:;}
# 205
goto _LLA;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}
# 212
static void Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*g,void*n){
{struct Cyc_PrattProver_Row*_tmp36=g->rows;struct Cyc_PrattProver_Row*ns=_tmp36;for(0;ns != 0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(ns->source,n)== 0)return;}}
# 216
({struct Cyc_PrattProver_Row*_tmpC9=({struct Cyc_PrattProver_Row*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->next=g->rows,_tmp37->source=n,_tmp37->distance=0;_tmp37;});g->rows=_tmpC9;});{
# 218
void*_tmp38=n;void*_tmp39;if(*((int*)_tmp38)== 3){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp38)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp38)->f2 != 0){_tmp39=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp38)->f2)->hd;{void*x=_tmp39;
# 220
({struct Cyc_PrattProver_Graph*_tmpCB=g;void*_tmpCA=n;Cyc_PrattProver_add_edge(_tmpCB,0,_tmpCA,Cyc_AssnDef_zero(),2147483647 - 1,0);});
({struct Cyc_PrattProver_Graph*_tmpCD=g;void*_tmpCC=n;Cyc_PrattProver_add_edge(_tmpCD,1,_tmpCC,Cyc_AssnDef_zero(),2147483647 - 1,0);});
# 223
Cyc_PrattProver_add_type_info(g,x);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 226
 Cyc_PrattProver_add_type_info(g,n);
goto _LL0;}_LL0:;}}
# 231
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row(struct Cyc_PrattProver_Graph*g,void*i){
{struct Cyc_PrattProver_Row*_tmp3A=g->rows;struct Cyc_PrattProver_Row*ns=_tmp3A;for(0;ns != 0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(i,ns->source)== 0)return ns;}}
return 0;}
# 237
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_lookup_col(struct Cyc_PrattProver_Row*n,void*j){
{struct Cyc_PrattProver_Distance*_tmp3B=((struct Cyc_PrattProver_Row*)_check_null(n))->distance;struct Cyc_PrattProver_Distance*ds=_tmp3B;for(0;ds != 0;ds=ds->next){
if(Cyc_AssnDef_cmp_term(j,ds->target)== 0)return ds;}}
return 0;}
# 245
static struct Cyc_PrattProver_Dist*Cyc_PrattProver_lookup_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j){
static struct Cyc_PrattProver_Dist zero={0,0};
if(i == 0)return 0;
if(Cyc_AssnDef_cmp_term(i->source,j)== 0)
return& zero;{
struct Cyc_PrattProver_Distance*_tmp3C=Cyc_PrattProver_lookup_col(i,j);struct Cyc_PrattProver_Distance*ds=_tmp3C;
if(ds != 0){
if(is_signed &&(int)ds->dinfo & (int)Cyc_PrattProver_Signed_valid)
return& ds->signed_dist;else{
if(!is_signed &&(int)ds->dinfo & (int)Cyc_PrattProver_Unsigned_valid)
return& ds->unsigned_dist;else{
# 257
return 0;}}}else{
# 259
return 0;}}}
# 267
static void Cyc_PrattProver_insert_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j,int dist,void*origin){
if(Cyc_AssnDef_cmp_term(((struct Cyc_PrattProver_Row*)_check_null(i))->source,j)== 0)return;{
struct Cyc_PrattProver_Distance*_tmp3D=Cyc_PrattProver_lookup_col(i,j);struct Cyc_PrattProver_Distance*ds=_tmp3D;
if(ds != 0){
if(is_signed){
ds->dinfo |=(int)Cyc_PrattProver_Signed_valid;
(ds->signed_dist).length=dist;
(ds->signed_dist).prim=origin;}else{
# 276
ds->dinfo |=(int)Cyc_PrattProver_Unsigned_valid;
(ds->unsigned_dist).length=dist;
(ds->unsigned_dist).prim=origin;}
# 280
return;}{
# 282
struct Cyc_PrattProver_Dist d=({struct Cyc_PrattProver_Dist _tmpA5;_tmpA5.length=dist,_tmpA5.prim=origin;_tmpA5;});
({struct Cyc_PrattProver_Distance*_tmpCE=({struct Cyc_PrattProver_Distance*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->next=i->distance,_tmp3E->target=j,is_signed?_tmp3E->dinfo=2:(_tmp3E->dinfo=1),_tmp3E->unsigned_dist=d,_tmp3E->signed_dist=d;_tmp3E;});i->distance=_tmpCE;});}}}struct _tuple12{int f1;int f2;};
# 290
enum Cyc_PrattProver_Operator{Cyc_PrattProver_PLUS =0U,Cyc_PrattProver_MINUS =1U};
# 300
static struct _tuple12 Cyc_PrattProver_is_overflow(int c1,enum Cyc_PrattProver_Operator op,int c2){
enum Cyc_PrattProver_Operator _tmp3F=op;switch((int)_tmp3F){case Cyc_PrattProver_PLUS:  {
# 303
int _tmp40=c1 + c2;int sum=_tmp40;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 306
if(sum > 0)return({struct _tuple12 _tmpA6;_tmpA6.f1=1,_tmpA6.f2=(- 2147483647)- 1;_tmpA6;});else{
return({struct _tuple12 _tmpA7;_tmpA7.f1=1,_tmpA7.f2=2147483647;_tmpA7;});}}else{
return({struct _tuple12 _tmpA8;_tmpA8.f1=0,_tmpA8.f2=sum;_tmpA8;});}}case Cyc_PrattProver_MINUS:  {
# 310
int _tmp41=c1 - c2;int dif=_tmp41;
if(((c1 ^ c2)& (c1 ^ dif))>> 31){
# 313
if(dif > 0)return({struct _tuple12 _tmpA9;_tmpA9.f1=1,_tmpA9.f2=(- 2147483647)- 1;_tmpA9;});else{
return({struct _tuple12 _tmpAA;_tmpAA.f1=1,_tmpAA.f2=2147483647;_tmpAA;});}}else{
return({struct _tuple12 _tmpAB;_tmpAB.f1=0,_tmpAB.f2=dif;_tmpAB;});}}default:
# 317
({void*_tmp42=0U;({struct _fat_ptr _tmpCF=({const char*_tmp43="Pratt_Prover: bad operator in overflow check";_tag_fat(_tmp43,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpCF,_tag_fat(_tmp42,sizeof(void*),0));});});};}
# 329 "pratt_prover.cyc"
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*g,int is_signed,void*i,void*j,int dist,void*origin){
Cyc_PrattProver_add_node(g,i);
Cyc_PrattProver_add_node(g,j);{
struct Cyc_PrattProver_Dist*ij_dist=({struct Cyc_PrattProver_Row*_tmpD1=Cyc_PrattProver_lookup_row(g,i);int _tmpD0=is_signed;Cyc_PrattProver_lookup_distance(_tmpD1,_tmpD0,j);});
# 334
if(ij_dist != 0 && ij_dist->length < dist)
return 1;{
struct Cyc_PrattProver_Dist*ji_dist=({struct Cyc_PrattProver_Row*_tmpD3=Cyc_PrattProver_lookup_row(g,j);int _tmpD2=is_signed;Cyc_PrattProver_lookup_distance(_tmpD3,_tmpD2,i);});
# 338
if(ji_dist != 0){
struct _tuple12 _tmp44=Cyc_PrattProver_is_overflow(ji_dist->length,Cyc_PrattProver_PLUS,dist);struct _tuple12 _stmttmp4=_tmp44;struct _tuple12 _tmp45=_stmttmp4;int _tmp47;int _tmp46;_tmp46=_tmp45.f1;_tmp47=_tmp45.f2;{int overflow=_tmp46;int sum=_tmp47;
if(sum < 0)
# 342
return 0;else{
if(overflow && sum > 0)
# 347
return 1;}}}{
# 353
struct Cyc_PrattProver_Row*_tmp48=Cyc_PrattProver_lookup_row(g,j);struct Cyc_PrattProver_Row*jrow=_tmp48;
{struct Cyc_PrattProver_Row*_tmp49=g->rows;struct Cyc_PrattProver_Row*ks=_tmp49;for(0;ks != 0;ks=ks->next){
void*_tmp4A=ks->source;void*k=_tmp4A;
struct Cyc_PrattProver_Dist*_tmp4B=Cyc_PrattProver_lookup_distance(ks,is_signed,i);struct Cyc_PrattProver_Dist*ki_dist=_tmp4B;
if(ki_dist == 0)continue;{
struct Cyc_PrattProver_Row*_tmp4C=g->rows;struct Cyc_PrattProver_Row*ls=_tmp4C;for(0;ls != 0;ls=ls->next){
void*_tmp4D=ls->source;void*l=_tmp4D;
struct Cyc_PrattProver_Dist*_tmp4E=Cyc_PrattProver_lookup_distance(ks,is_signed,l);struct Cyc_PrattProver_Dist*kl_dist=_tmp4E;
# 362
if(kl_dist != 0 && kl_dist->length == (- 2147483647)- 1)continue;{
# 364
struct Cyc_PrattProver_Dist*_tmp4F=Cyc_PrattProver_lookup_distance(jrow,is_signed,l);struct Cyc_PrattProver_Dist*jl_dist=_tmp4F;
if(jl_dist == 0)continue;{
# 368
int final;
int sum;
struct _tuple12 _tmp50=Cyc_PrattProver_is_overflow(ki_dist->length,Cyc_PrattProver_PLUS,dist);struct _tuple12 _stmttmp5=_tmp50;struct _tuple12 _tmp51=_stmttmp5;int _tmp53;int _tmp52;_tmp52=_tmp51.f1;_tmp53=_tmp51.f2;{int of1=_tmp52;int sum1=_tmp53;
if(!of1){
# 373
struct _tuple12 _tmp54=Cyc_PrattProver_is_overflow(sum1,Cyc_PrattProver_PLUS,jl_dist->length);struct _tuple12 _stmttmp6=_tmp54;struct _tuple12 _tmp55=_stmttmp6;int _tmp57;int _tmp56;_tmp56=_tmp55.f1;_tmp57=_tmp55.f2;{int of2=_tmp56;int sum2=_tmp57;
final=of2;
sum=sum2;}}else{
# 379
struct _tuple12 _tmp58=Cyc_PrattProver_is_overflow(ki_dist->length,Cyc_PrattProver_PLUS,jl_dist->length);struct _tuple12 _stmttmp7=_tmp58;struct _tuple12 _tmp59=_stmttmp7;int _tmp5B;int _tmp5A;_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;{int of1=_tmp5A;int sum1=_tmp5B;
if(!of1){
struct _tuple12 _tmp5C=Cyc_PrattProver_is_overflow(sum1,Cyc_PrattProver_PLUS,dist);struct _tuple12 _stmttmp8=_tmp5C;struct _tuple12 _tmp5D=_stmttmp8;int _tmp5F;int _tmp5E;_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{int of2=_tmp5E;int sum2=_tmp5F;
final=of2;
sum=sum;}}else{
# 387
final=1;
if(dist < 0)
# 390
sum=-2147483648;else{
# 393
sum=2147483647;}}}}
# 397
if(!final || sum < 0){
# 400
if(kl_dist == 0 || kl_dist->length > sum)
Cyc_PrattProver_insert_distance(ks,is_signed,l,sum,0);}}}}}}}}
# 405
return 1;}}}}
# 408
static struct _tuple11 Cyc_PrattProver_eval_term(void*t){
void*_tmp60=t;void*_tmp61;if(*((int*)_tmp60)== 0){_tmp61=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp60)->f1;{struct Cyc_Absyn_Exp*e=_tmp61;
return Cyc_Evexp_eval_const_uint_exp(e);}}else{
return({struct _tuple11 _tmpAC;_tmpAC.f1=0U,_tmpAC.f2=0;_tmpAC;});};}
# 418
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
# 421
struct _tuple11 _tmp62=Cyc_PrattProver_eval_term(t1);struct _tuple11 _stmttmp9=_tmp62;struct _tuple11 _tmp63=_stmttmp9;int _tmp65;unsigned _tmp64;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;{unsigned c1=_tmp64;int okay1=_tmp65;
struct _tuple11 _tmp66=Cyc_PrattProver_eval_term(t2);struct _tuple11 _stmttmpA=_tmp66;struct _tuple11 _tmp67=_stmttmpA;int _tmp69;unsigned _tmp68;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{unsigned c2=_tmp68;int okay2=_tmp69;
void*_tmp6A=Cyc_AssnDef_prim(t1,p,t2);void*origin=_tmp6A;
# 426
if(okay1 && okay2){
# 428
enum Cyc_AssnDef_Primreln _tmp6B=p;switch((int)_tmp6B){case Cyc_AssnDef_ULt:
 return c1 < c2;case Cyc_AssnDef_SLt:
 return(int)c1 < (int)c2;case Cyc_AssnDef_ULte:
 return c1 <= c2;case Cyc_AssnDef_SLte:
 return(int)c1 <= (int)c2;default:
({void*_tmp6C=0U;({struct _fat_ptr _tmpD4=({const char*_tmp6D="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp6D,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpD4,_tag_fat(_tmp6C,sizeof(void*),0));});});};}else{
# 435
if(okay2){
# 437
enum Cyc_AssnDef_Primreln _tmp6E=p;switch((int)_tmp6E){case Cyc_AssnDef_ULt:
# 440
 if(c2 == 0U)return 0;
c2=c2 - (unsigned)1;
goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 445
 if(c2 <= 2147483647U){
if(!({struct Cyc_PrattProver_Graph*_tmpD8=g;void*_tmpD7=t1;void*_tmpD6=Cyc_AssnDef_zero();int _tmpD5=(int)c2;Cyc_PrattProver_add_edge(_tmpD8,0,_tmpD7,_tmpD6,_tmpD5,origin);}))return 0;
# 448
return({struct Cyc_PrattProver_Graph*_tmpDB=g;void*_tmpDA=t1;void*_tmpD9=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmpDB,1,_tmpDA,_tmpD9,(int)c2,0);});}else{
# 450
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_tmpDC=Cyc_PrattProver_lookup_row(g,t1);Cyc_PrattProver_lookup_distance(_tmpDC,1,Cyc_AssnDef_zero());});
if(c1 != 0 && c1->length <= - 1)
return({struct Cyc_PrattProver_Graph*_tmpDF=g;void*_tmpDE=t1;void*_tmpDD=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmpDF,1,_tmpDE,_tmpDD,(int)c2,0);});}
# 455
return 1;case Cyc_AssnDef_SLt:
# 458
 if(c2 == 2147483648U)return 0;
c2=c2 - (unsigned)1;
goto _LL19;case Cyc_AssnDef_SLte: _LL19:
# 463
 if(!({struct Cyc_PrattProver_Graph*_tmpE3=g;void*_tmpE2=t1;void*_tmpE1=Cyc_AssnDef_zero();int _tmpE0=(int)c2;Cyc_PrattProver_add_edge(_tmpE3,1,_tmpE2,_tmpE1,_tmpE0,origin);}))return 0;
if((int)c2 > 0){
# 466
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_tmpE5=({struct Cyc_PrattProver_Graph*_tmpE4=g;Cyc_PrattProver_lookup_row(_tmpE4,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_tmpE5,1,t1);});
if(c1 != 0 && c1->length <= 0)
# 469
return({struct Cyc_PrattProver_Graph*_tmpE8=g;void*_tmpE7=t1;void*_tmpE6=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmpE8,0,_tmpE7,_tmpE6,(int)c2,0);});}
# 471
return 1;default:
({void*_tmp6F=0U;({struct _fat_ptr _tmpE9=({const char*_tmp70="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp70,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpE9,_tag_fat(_tmp6F,sizeof(void*),0));});});};}else{
# 474
if(okay1){
# 476
enum Cyc_AssnDef_Primreln _tmp71=p;switch((int)_tmp71){case Cyc_AssnDef_ULt:
# 479
 if(c1 == (unsigned)-1)return 0;
c1=c1 + (unsigned)1;
goto _LL20;case Cyc_AssnDef_ULte: _LL20:
# 484
 if(c1 > 2147483647U){
# 487
if(!({struct Cyc_PrattProver_Graph*_tmpEB=g;void*_tmpEA=t2;Cyc_PrattProver_add_edge(_tmpEB,1,_tmpEA,Cyc_AssnDef_zero(),- 1,0);}))return 0;{
int k=-(int)c1;
if(k > 0){
if(!({struct Cyc_PrattProver_Graph*_tmpEE=g;void*_tmpED=Cyc_AssnDef_zero();void*_tmpEC=t2;Cyc_PrattProver_add_edge(_tmpEE,1,_tmpED,_tmpEC,k,0);}))return 0;}}}
# 493
if(c1 <= 2147483648U){
# 495
int k=-(int)c1;
return({struct Cyc_PrattProver_Graph*_tmpF2=g;void*_tmpF1=Cyc_AssnDef_zero();void*_tmpF0=t2;int _tmpEF=k;Cyc_PrattProver_add_edge(_tmpF2,0,_tmpF1,_tmpF0,_tmpEF,origin);});}
# 498
return 1;case Cyc_AssnDef_SLt:
# 501
 if(c1 == (unsigned)2147483647)return 0;
c1=c1 + (unsigned)1;
goto _LL24;case Cyc_AssnDef_SLte: _LL24:
# 506
 if(c1 == (unsigned)-2147483648)return 1;else{
# 508
int k=-(int)c1;
if((int)c1 >= 0){
# 511
if(!({struct Cyc_PrattProver_Graph*_tmpF5=g;void*_tmpF4=Cyc_AssnDef_zero();void*_tmpF3=t2;Cyc_PrattProver_add_edge(_tmpF5,0,_tmpF4,_tmpF3,k,0);}))return 0;}
# 513
return({struct Cyc_PrattProver_Graph*_tmpF9=g;void*_tmpF8=Cyc_AssnDef_zero();void*_tmpF7=t2;int _tmpF6=k;Cyc_PrattProver_add_edge(_tmpF9,1,_tmpF8,_tmpF7,_tmpF6,origin);});}default:
# 515
({void*_tmp72=0U;({struct _fat_ptr _tmpFA=({const char*_tmp73="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp73,sizeof(char),42U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpFA,_tag_fat(_tmp72,sizeof(void*),0));});});};}}}{
# 520
enum Cyc_AssnDef_Primreln _tmp74=p;switch((int)_tmp74){case Cyc_AssnDef_ULt:
# 523
 return Cyc_PrattProver_add_edge(g,0,t1,t2,- 1,origin);case Cyc_AssnDef_ULte:
# 526
 return Cyc_PrattProver_add_edge(g,0,t1,t2,0,origin);case Cyc_AssnDef_SLt:
# 529
 return Cyc_PrattProver_add_edge(g,1,t1,t2,- 1,origin);case Cyc_AssnDef_SLte:
# 532
 return Cyc_PrattProver_add_edge(g,1,t1,t2,0,origin);default:
({void*_tmp75=0U;({struct _fat_ptr _tmpFB=({const char*_tmp76="Vcgen:found bad primop in add_constraint";_tag_fat(_tmp76,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpFB,_tag_fat(_tmp75,sizeof(void*),0));});});};}}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 538
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 541
unsigned Cyc_PrattProver_max_paths=17U;
unsigned Cyc_PrattProver_max_paths_seen=0U;
unsigned Cyc_PrattProver_max_parallel_assn=4U;
# 551
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,void*a){
LOOP:
# 555
 if(gs == 0)return gs;
{void*_tmp77=a;enum Cyc_AssnDef_Primreln _tmp79;void*_tmp7A;void*_tmp78;switch(*((int*)_tmp77)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _tmp78=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp77)->f1;_tmp7A=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp77)->f2;{void*a1=_tmp78;void*a2=_tmp7A;
# 560
gs=Cyc_PrattProver_cgraph(gs,a1);
a=a2;
goto LOOP;}case 4: _tmp78=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp77)->f1;_tmp7A=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp77)->f2;{void*a1=_tmp78;void*a2=_tmp7A;
# 564
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(Cyc_PrattProver_max_paths_seen < n)Cyc_PrattProver_max_paths_seen=n;
# 567
if(n >= Cyc_PrattProver_max_paths)(int)_throw((void*)& Cyc_PrattProver_too_large);{
# 569
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graph(gs);
# 572
gs1=Cyc_PrattProver_cgraph(gs1,a1);
# 574
gs2=Cyc_PrattProver_cgraph(gs2,a2);
# 576
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}case 2: switch((int)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f2){case Cyc_AssnDef_Neq: _tmp78=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f1;_tmp7A=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f3;{void*t1=_tmp78;void*t2=_tmp7A;
# 581
a=({void*_tmpFE=({void*_tmpFC=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_tmpFC,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_tmpFE,({void*_tmpFD=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_tmpFD,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}case Cyc_AssnDef_Eq: _tmp78=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f1;_tmp7A=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f3;{void*t1=_tmp78;void*t2=_tmp7A;
# 585
a=({void*_tmp101=({void*_tmp100=({void*_tmpFF=Cyc_AssnDef_slte(t1,t2);Cyc_AssnDef_and(_tmpFF,Cyc_AssnDef_slte(t2,t1));});Cyc_AssnDef_and(_tmp100,Cyc_AssnDef_ulte(t1,t2));});Cyc_AssnDef_and(_tmp101,Cyc_AssnDef_ulte(t2,t1));});
goto LOOP;}default: _tmp78=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f1;_tmp79=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f2;_tmp7A=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp77)->f3;{void*t1=_tmp78;enum Cyc_AssnDef_Primreln p=_tmp79;void*t2=_tmp7A;
# 590
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*_tmp7B=gs;struct Cyc_PrattProver_Graph*g=_tmp7B;for(0;g != 0;g=g->next){
if(!Cyc_PrattProver_add_constraint(g,t1,p,t2)){
if(prev == 0)
gs=g->next;else{
# 596
prev->next=g->next;}}else{
# 599
prev=g;}}}
# 602
goto _LL0;}}default:
# 605
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}
# 609
return gs;}
# 615
static int Cyc_PrattProver_consistent(void*a){
struct _handler_cons _tmp7C;_push_handler(& _tmp7C);{int _tmp7E=0;if(setjmp(_tmp7C.handler))_tmp7E=1;if(!_tmp7E){
# 618
{struct _fat_ptr gs=_tag_fat(({struct Cyc_PrattProver_Graph*_tmp102=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_tmp102,a);}),sizeof(struct Cyc_PrattProver_Graph),1U);
# 621
int _tmp7F=({struct Cyc_PrattProver_Graph*_tmp103=(struct Cyc_PrattProver_Graph*)gs.curr;_tmp103 != (struct Cyc_PrattProver_Graph*)(_tag_fat(0,0,0)).curr;});_npop_handler(0);return _tmp7F;}
# 618
;_pop_handler();}else{void*_tmp7D=(void*)Cyc_Core_get_exn_thrown();void*_tmp80=_tmp7D;void*_tmp81;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_tmp80)->tag == Cyc_PrattProver_TooLarge)
# 625
return 1;else{_tmp81=_tmp80;{void*exn=_tmp81;(int)_rethrow(exn);}};}}}
# 630
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_tmp104=ctxt;Cyc_AssnDef_and(_tmp104,Cyc_AssnDef_not(a));});
# 635
return !Cyc_PrattProver_consistent(b);}
# 638
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row_in_rows(struct Cyc_PrattProver_Row*r,void*i){
for(1;r != 0;r=r->next){
if(Cyc_AssnDef_cmp_term(i,r->source)== 0)return r;}
# 642
return 0;}
# 645
static struct Cyc_PrattProver_Row*Cyc_PrattProver_or2rows(struct Cyc_PrattProver_Row*g1,struct Cyc_PrattProver_Row*g2){
struct Cyc_PrattProver_Row*res=0;
for(1;g1 != 0;g1=g1->next){
struct Cyc_PrattProver_Row*_tmp82=Cyc_PrattProver_lookup_row_in_rows(g2,g1->source);struct Cyc_PrattProver_Row*r=_tmp82;
if(r != 0){
# 651
struct Cyc_PrattProver_Row*_tmp83=({struct Cyc_PrattProver_Row*_tmp8F=_cycalloc(sizeof(*_tmp8F));*_tmp8F=*g1;_tmp8F;});struct Cyc_PrattProver_Row*newrow=_tmp83;
newrow->next=res;
res=newrow;
res->distance=0;{
# 656
struct Cyc_PrattProver_Distance*_tmp84=g1->distance;struct Cyc_PrattProver_Distance*d=_tmp84;for(0;d != 0;d=d->next){
struct Cyc_PrattProver_Distance*dist_res=0;
if((int)d->dinfo & (int)Cyc_PrattProver_Signed_valid){
int _tmp85=(d->signed_dist).length;int d1=_tmp85;
struct Cyc_PrattProver_Dist*_tmp86=Cyc_PrattProver_lookup_distance(r,1,d->target);struct Cyc_PrattProver_Dist*d2_ptr=_tmp86;
if(d2_ptr != 0){
int _tmp87=d2_ptr->length;int d2=_tmp87;
struct Cyc_PrattProver_Dist _tmp88=d1 > d2?d->signed_dist:*d2_ptr;struct Cyc_PrattProver_Dist newd=_tmp88;
dist_res=({struct Cyc_PrattProver_Distance*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->target=d->target,_tmp89->next=res->distance,_tmp89->dinfo=Cyc_PrattProver_Signed_valid,
# 667
(_tmp89->unsigned_dist).length=0,(_tmp89->unsigned_dist).prim=0,_tmp89->signed_dist=newd;_tmp89;});}}
# 671
if((int)d->dinfo & (int)Cyc_PrattProver_Unsigned_valid){
int _tmp8A=(d->unsigned_dist).length;int d1=_tmp8A;
struct Cyc_PrattProver_Dist*_tmp8B=Cyc_PrattProver_lookup_distance(r,0,d->target);struct Cyc_PrattProver_Dist*d2_ptr=_tmp8B;
if(d2_ptr != 0){
int _tmp8C=d2_ptr->length;int d2=_tmp8C;
struct Cyc_PrattProver_Dist _tmp8D=d1 > d2?d->unsigned_dist:*d2_ptr;struct Cyc_PrattProver_Dist newd=_tmp8D;
if(dist_res != 0){
dist_res->unsigned_dist=newd;
dist_res->dinfo=Cyc_PrattProver_Both_valid;}else{
# 681
dist_res=({struct Cyc_PrattProver_Distance*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->target=d->target,_tmp8E->next=res->distance,_tmp8E->dinfo=Cyc_PrattProver_Unsigned_valid,_tmp8E->unsigned_dist=newd,
# 685
(_tmp8E->signed_dist).length=0,(_tmp8E->signed_dist).prim=0;_tmp8E;});}}}
# 689
if(dist_res != 0)
res->distance=dist_res;}}}}
# 695
return res;}
# 698
void*Cyc_PrattProver_int2term(int is_signed,int c){
enum Cyc_Absyn_Sign _tmp90=is_signed?0U: 1U;enum Cyc_Absyn_Sign s=_tmp90;
struct Cyc_Absyn_Exp*_tmp91=Cyc_Absyn_int_exp(s,c,0U);struct Cyc_Absyn_Exp*e=_tmp91;
({void*_tmp105=Cyc_Absyn_int_type(s,Cyc_Absyn_Int_sz);e->topt=_tmp105;});
return Cyc_AssnDef_cnst(e);}
# 705
void*Cyc_PrattProver_edge2assn(void*source,void*target,int weight,int is_signed){
enum Cyc_AssnDef_Primreln _tmp92=is_signed?3U: 5U;enum Cyc_AssnDef_Primreln s=_tmp92;
enum Cyc_AssnDef_Primreln _tmp93=is_signed?2U: 4U;enum Cyc_AssnDef_Primreln s1=_tmp93;
void*result=(void*)& Cyc_AssnDef_true_assn;
if(({void*_tmp106=source;_tmp106 == Cyc_AssnDef_zero();}))
result=({void*_tmp108=Cyc_PrattProver_int2term(is_signed,- weight);enum Cyc_AssnDef_Primreln _tmp107=s;Cyc_AssnDef_prim(_tmp108,_tmp107,target);});else{
if(({void*_tmp109=target;_tmp109 == Cyc_AssnDef_zero();}))
result=({void*_tmp10B=source;enum Cyc_AssnDef_Primreln _tmp10A=s;Cyc_AssnDef_prim(_tmp10B,_tmp10A,Cyc_PrattProver_int2term(is_signed,weight));});else{
# 714
int _tmp94=weight;switch((int)_tmp94){case 0:
 result=Cyc_AssnDef_prim(source,s,target);goto _LL0;case -1:
 result=Cyc_AssnDef_prim(source,s1,target);goto _LL0;default:
# 722
 goto _LL0;}_LL0:;}}
# 725
return result;}
# 729
void*Cyc_PrattProver_row2assn(struct Cyc_PrattProver_Row*r){
void*a=(void*)& Cyc_AssnDef_true_assn;
for(1;r != 0;r=r->next){
struct Cyc_PrattProver_Distance*_tmp95=r->distance;struct Cyc_PrattProver_Distance*d=_tmp95;for(0;d != 0;d=d->next){
if((int)d->dinfo & (int)Cyc_PrattProver_Signed_valid){
if((d->signed_dist).prim != 0)
a=Cyc_AssnDef_and(a,(void*)_check_null((d->signed_dist).prim));else{
# 737
a=({void*_tmp10C=a;Cyc_AssnDef_and(_tmp10C,Cyc_PrattProver_edge2assn(r->source,d->target,(d->signed_dist).length,1));});}}
# 740
if((int)d->dinfo & (int)Cyc_PrattProver_Unsigned_valid){
if((d->unsigned_dist).prim != 0)
a=Cyc_AssnDef_and(a,(void*)_check_null((d->unsigned_dist).prim));else{
# 744
a=({void*_tmp10D=a;Cyc_AssnDef_and(_tmp10D,Cyc_PrattProver_edge2assn(r->source,d->target,(d->unsigned_dist).length,0));});}}}}
# 749
return a;}
# 752
void*Cyc_PrattProver_merge_assn(void*a1,void*a2){
# 754
struct Cyc_PrattProver_Graph*_tmp96=({struct Cyc_PrattProver_Graph*_tmp10E=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_tmp10E,Cyc_AssnDef_or(a1,a2));});struct Cyc_PrattProver_Graph*g=_tmp96;
if(g == 0)return(void*)& Cyc_AssnDef_false_assn;
if(({unsigned _tmp10F=Cyc_PrattProver_num_graphs(g);_tmp10F > Cyc_PrattProver_max_parallel_assn;})){
struct Cyc_PrattProver_Row*_tmp97=g->rows;struct Cyc_PrattProver_Row*row=_tmp97;
{struct Cyc_PrattProver_Graph*_tmp98=g->next;struct Cyc_PrattProver_Graph*row1=_tmp98;for(0;row1 != 0;row1=row1->next){
row=Cyc_PrattProver_or2rows(row,row1->rows);}}
# 761
return Cyc_PrattProver_row2assn(row);}else{
# 763
return Cyc_AssnDef_or(a1,a2);}}
