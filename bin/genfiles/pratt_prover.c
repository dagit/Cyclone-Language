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
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);struct Cyc_Hashtable_Table;struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 64
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
void*Cyc_AssnDef_zero (void);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
# 75
int Cyc_AssnDef_cmp_term(void*,void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 117 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
# 121
void*Cyc_AssnDef_prim(void*,enum Cyc_AssnDef_Primreln,void*);
# 124
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
# 132
void*Cyc_AssnDef_not(void*);
# 28 "pratt_prover.cyc"
enum Cyc_PrattProver_DistanceInfo{Cyc_PrattProver_Unsigned_valid =1,Cyc_PrattProver_Signed_valid =2,Cyc_PrattProver_Both_valid =3};struct Cyc_PrattProver_Dist{int length;void*prim;};struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;void*target;enum Cyc_PrattProver_DistanceInfo dinfo;struct Cyc_PrattProver_Dist unsigned_dist;struct Cyc_PrattProver_Dist signed_dist;};struct Cyc_PrattProver_Row{struct Cyc_PrattProver_Row*next;void*source;struct Cyc_PrattProver_Distance*distance;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Row*rows;};
# 66 "pratt_prover.cyc"
static void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*gs){
({void*_tmp0=0U;({struct Cyc___cycFILE*_tmp114=Cyc_stderr;struct _fat_ptr _tmp113=({const char*_tmp1="Graphs:\n";_tag_fat(_tmp1,sizeof(char),9U);});Cyc_fprintf(_tmp114,_tmp113,_tag_fat(_tmp0,sizeof(void*),0));});});
for(1;gs != 0;gs=gs->next){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp116=Cyc_stderr;struct _fat_ptr _tmp115=({const char*_tmp3="\t{";_tag_fat(_tmp3,sizeof(char),3U);});Cyc_fprintf(_tmp116,_tmp115,_tag_fat(_tmp2,sizeof(void*),0));});});{
struct Cyc_PrattProver_Row*_tmp4=gs->rows;struct Cyc_PrattProver_Row*ns=_tmp4;for(0;ns != 0;ns=ns->next){
void*_tmp5=ns->source;void*t1=_tmp5;
{struct Cyc_PrattProver_Distance*_tmp6=ns->distance;struct Cyc_PrattProver_Distance*ds=_tmp6;for(0;ds != 0;ds=ds->next){
void*_tmp7=ds->target;void*t2=_tmp7;
if((int)ds->dinfo == 3)
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0,({struct _fat_ptr _tmp117=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpEF.f1=_tmp117;});_tmpEF;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0,({
struct _fat_ptr _tmp118=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpEE.f1=_tmp118;});_tmpEE;});struct Cyc_Int_pa_PrintArg_struct _tmpC=({struct Cyc_Int_pa_PrintArg_struct _tmpED;_tmpED.tag=1,_tmpED.f1=(unsigned long)(ds->unsigned_dist).length;_tmpED;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0,({struct _fat_ptr _tmp119=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpEC.f1=_tmp119;});_tmpEC;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmpEB;_tmpEB.tag=0,({
struct _fat_ptr _tmp11A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpEB.f1=_tmp11A;});_tmpEB;});struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmpEA;_tmpEA.tag=1,_tmpEA.f1=(unsigned long)(ds->signed_dist).length;_tmpEA;});void*_tmp8[6];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB,_tmp8[2]=& _tmpC,_tmp8[3]=& _tmpD,_tmp8[4]=& _tmpE,_tmp8[5]=& _tmpF;({struct Cyc___cycFILE*_tmp11C=Cyc_stderr;struct _fat_ptr _tmp11B=({const char*_tmp9="%s - %s U<= %d, %s - %s S<= %d";_tag_fat(_tmp9,sizeof(char),31U);});Cyc_fprintf(_tmp11C,_tmp11B,_tag_fat(_tmp8,sizeof(void*),6));});});else{
if((int)ds->dinfo == 2)
({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0,({struct _fat_ptr _tmp11D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpF2.f1=_tmp11D;});_tmpF2;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0,({
struct _fat_ptr _tmp11E=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpF1.f1=_tmp11E;});_tmpF1;});struct Cyc_Int_pa_PrintArg_struct _tmp14=({struct Cyc_Int_pa_PrintArg_struct _tmpF0;_tmpF0.tag=1,_tmpF0.f1=(unsigned long)(ds->signed_dist).length;_tmpF0;});void*_tmp10[3];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13,_tmp10[2]=& _tmp14;({struct Cyc___cycFILE*_tmp120=Cyc_stderr;struct _fat_ptr _tmp11F=({const char*_tmp11="%s - %s S<= %d";_tag_fat(_tmp11,sizeof(char),15U);});Cyc_fprintf(_tmp120,_tmp11F,_tag_fat(_tmp10,sizeof(void*),3));});});else{
# 82
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0,({struct _fat_ptr _tmp121=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpF5.f1=_tmp121;});_tmpF5;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0,({
struct _fat_ptr _tmp122=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpF4.f1=_tmp122;});_tmpF4;});struct Cyc_Int_pa_PrintArg_struct _tmp19=({struct Cyc_Int_pa_PrintArg_struct _tmpF3;_tmpF3.tag=1,_tmpF3.f1=(unsigned long)(ds->unsigned_dist).length;_tmpF3;});void*_tmp15[3];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19;({struct Cyc___cycFILE*_tmp124=Cyc_stderr;struct _fat_ptr _tmp123=({const char*_tmp16="%s - %s U<= %d";_tag_fat(_tmp16,sizeof(char),15U);});Cyc_fprintf(_tmp124,_tmp123,_tag_fat(_tmp15,sizeof(void*),3));});});}}
# 85
if(ds->next != 0)({void*_tmp1A=0U;({struct Cyc___cycFILE*_tmp126=Cyc_stderr;struct _fat_ptr _tmp125=({const char*_tmp1B=",";_tag_fat(_tmp1B,sizeof(char),2U);});Cyc_fprintf(_tmp126,_tmp125,_tag_fat(_tmp1A,sizeof(void*),0));});});}}
# 87
({void*_tmp1C=0U;({struct Cyc___cycFILE*_tmp128=Cyc_stderr;struct _fat_ptr _tmp127=({const char*_tmp1D="}\n";_tag_fat(_tmp1D,sizeof(char),3U);});Cyc_fprintf(_tmp128,_tmp127,_tag_fat(_tmp1C,sizeof(void*),0));});});}}}}
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
struct Cyc_PrattProver_Distance*_tmp129=Cyc_PrattProver_copy_distance(ns->distance);_tmp21->distance=_tmp129;});_tmp21;});}
# 122
return res;}
# 126
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graph(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Graph*res=0;
for(1;g != 0;g=g->next){
res=({struct Cyc_PrattProver_Graph*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->next=res,({struct Cyc_PrattProver_Row*_tmp12A=Cyc_PrattProver_copy_rows(g->rows);_tmp22->rows=_tmp12A;});_tmp22;});}
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
void*_tmp24=t;void*_tmp25;switch(*((int*)_tmp24)){case 0:
 return Cyc_Absyn_int_type(2U,2U);case 1: _tmp25=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp24)->f1;{struct Cyc_Absyn_Exp*e=_tmp25;
return e->topt;}case 2: _tmp25=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp24)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp25;
return vd->type;}case 3: _tmp25=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp24)->f3;{void*topt=_tmp25;
return topt;}case 4:
# 155
 return 0;default: _tmp25=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp24)->f1;{void*type=_tmp25;
return type;}};}
# 160
static void Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*,void*);
# 162
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
Cyc_PrattProver_add_node(g,t2);
# 165
Cyc_PrattProver_add_constraint(g,t1,5U,t2);
Cyc_PrattProver_add_constraint(g,t1,3U,t2);
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}
# 174
static void Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*n){
void*_tmp26=Cyc_PrattProver_get_term_type(n);void*topt=_tmp26;
if(topt != 0){
void*t=topt;
struct Cyc_Absyn_Exp*_tmp27=Cyc_Tcutil_get_type_bound(t);struct Cyc_Absyn_Exp*eopt=_tmp27;
if(eopt != 0){
void*t1=Cyc_AssnDef_primop(18U,({void*_tmp2F[1];_tmp2F[0]=n;({struct Cyc_List_List*(*_tmp12B)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp12B(_tag_fat(_tmp2F,sizeof(void*),1));});}));
# 182
struct Cyc_Absyn_Exp*e=eopt;
loop: {
void*_tmp28=e->r;void*_stmttmp0=_tmp28;void*_tmp29=_stmttmp0;void*_tmp2B;void*_tmp2A;if(*((int*)_tmp29)== 14){_tmp2A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp29)->f1;_tmp2B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp29)->f2;{void*tp2=_tmp2A;struct Cyc_Absyn_Exp*e2=_tmp2B;
# 186
{void*_tmp2C=Cyc_Absyn_compress(tp2);void*_stmttmp1=_tmp2C;void*_tmp2D=_stmttmp1;enum Cyc_Absyn_Size_of _tmp2E;if(*((int*)_tmp2D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D)->f1)== 1){_tmp2E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D)->f1)->f2;{enum Cyc_Absyn_Size_of s=_tmp2E;
# 188
if((int)s == 2 ||(int)s == 3){
e=e2;goto loop;}
# 191
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 194
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 197
void*t2=Cyc_AssnDef_cnst(e);
Cyc_PrattProver_add_eq(g,t1,t2);}}{
# 200
void*_tmp30=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp30;void*_tmp31=_stmttmp2;void*_tmp32;if(*((int*)_tmp31)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2 != 0){_tmp32=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2)->hd;{void*v=_tmp32;
# 202
{void*_tmp33=Cyc_Absyn_compress(v);void*_stmttmp3=_tmp33;void*_tmp34=_stmttmp3;void*_tmp35;if(*((int*)_tmp34)== 9){_tmp35=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp34)->f1;{struct Cyc_Absyn_Exp*e=_tmp35;
({struct Cyc_PrattProver_Graph*_tmp12D=g;void*_tmp12C=n;Cyc_PrattProver_add_eq(_tmp12D,_tmp12C,Cyc_AssnDef_cnst(e));});goto _LLF;}}else{_tmp35=_tmp34;{void*v2=_tmp35;
({struct Cyc_PrattProver_Graph*_tmp12F=g;void*_tmp12E=n;Cyc_PrattProver_add_eq(_tmp12F,_tmp12E,Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U)));});goto _LLF;}}_LLF:;}
# 206
goto _LLA;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}
# 213
static void Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*g,void*n){
{struct Cyc_PrattProver_Row*_tmp36=g->rows;struct Cyc_PrattProver_Row*ns=_tmp36;for(0;ns != 0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(ns->source,n)== 0)return;}}
# 217
({struct Cyc_PrattProver_Row*_tmp130=({struct Cyc_PrattProver_Row*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->next=g->rows,_tmp37->source=n,_tmp37->distance=0;_tmp37;});g->rows=_tmp130;});{
# 219
void*_tmp38=n;void*_tmp39;if(*((int*)_tmp38)== 4){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp38)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp38)->f2 != 0){_tmp39=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp38)->f2)->hd;{void*x=_tmp39;
# 221
({struct Cyc_PrattProver_Graph*_tmp132=g;void*_tmp131=n;Cyc_PrattProver_add_edge(_tmp132,0,_tmp131,Cyc_AssnDef_zero(),2147483646,0);});
({struct Cyc_PrattProver_Graph*_tmp134=g;void*_tmp133=n;Cyc_PrattProver_add_edge(_tmp134,1,_tmp133,Cyc_AssnDef_zero(),2147483646,0);});
# 224
Cyc_PrattProver_add_type_info(g,x);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 227
 Cyc_PrattProver_add_type_info(g,n);
goto _LL0;}_LL0:;}}
# 232
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row(struct Cyc_PrattProver_Graph*g,void*i){
{struct Cyc_PrattProver_Row*_tmp3A=g->rows;struct Cyc_PrattProver_Row*ns=_tmp3A;for(0;ns != 0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(i,ns->source)== 0)return ns;}}
return 0;}
# 238
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_lookup_col(struct Cyc_PrattProver_Row*n,void*j){
{struct Cyc_PrattProver_Distance*_tmp3B=((struct Cyc_PrattProver_Row*)_check_null(n))->distance;struct Cyc_PrattProver_Distance*ds=_tmp3B;for(0;ds != 0;ds=ds->next){
if(Cyc_AssnDef_cmp_term(j,ds->target)== 0)return ds;}}
return 0;}
# 246
static struct Cyc_PrattProver_Dist*Cyc_PrattProver_lookup_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j){
static struct Cyc_PrattProver_Dist zero={0,0};
if(i == 0)return 0;
if(Cyc_AssnDef_cmp_term(i->source,j)== 0)
return& zero;{
struct Cyc_PrattProver_Distance*_tmp3C=Cyc_PrattProver_lookup_col(i,j);struct Cyc_PrattProver_Distance*ds=_tmp3C;
if(ds != 0){
if(is_signed &&(int)ds->dinfo & 2)
return& ds->signed_dist;else{
if(!is_signed &&(int)ds->dinfo & 1)
return& ds->unsigned_dist;else{
# 258
return 0;}}}else{
# 260
return 0;}}}
# 268
static void Cyc_PrattProver_insert_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j,int dist,void*origin){
if(Cyc_AssnDef_cmp_term(((struct Cyc_PrattProver_Row*)_check_null(i))->source,j)== 0)return;{
struct Cyc_PrattProver_Distance*_tmp3D=Cyc_PrattProver_lookup_col(i,j);struct Cyc_PrattProver_Distance*ds=_tmp3D;
if(ds != 0){
if(is_signed){
ds->dinfo |=2;
(ds->signed_dist).length=dist;
(ds->signed_dist).prim=origin;}else{
# 277
ds->dinfo |=1;
(ds->unsigned_dist).length=dist;
(ds->unsigned_dist).prim=origin;}
# 281
return;}{
# 283
struct Cyc_PrattProver_Dist d=({struct Cyc_PrattProver_Dist _tmpF6;_tmpF6.length=dist,_tmpF6.prim=origin;_tmpF6;});
({struct Cyc_PrattProver_Distance*_tmp135=({struct Cyc_PrattProver_Distance*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->next=i->distance,_tmp3E->target=j,is_signed?_tmp3E->dinfo=2:(_tmp3E->dinfo=1),_tmp3E->unsigned_dist=d,_tmp3E->signed_dist=d;_tmp3E;});i->distance=_tmp135;});}}}struct _tuple12{int f1;int f2;};
# 291
enum Cyc_PrattProver_Operator{Cyc_PrattProver_PLUS =0U,Cyc_PrattProver_MINUS =1U};
# 301
static struct _tuple12 Cyc_PrattProver_is_signed_overflow(int c1,enum Cyc_PrattProver_Operator op,int c2){
enum Cyc_PrattProver_Operator _tmp3F=op;switch((int)_tmp3F){case Cyc_PrattProver_PLUS:  {
# 304
int _tmp40=c1 + c2;int sum=_tmp40;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 307
if(sum > 0)return({struct _tuple12 _tmpF7;_tmpF7.f1=1,_tmpF7.f2=-2147483648;_tmpF7;});else{
return({struct _tuple12 _tmpF8;_tmpF8.f1=1,_tmpF8.f2=2147483647;_tmpF8;});}}else{
return({struct _tuple12 _tmpF9;_tmpF9.f1=0,_tmpF9.f2=sum;_tmpF9;});}}case Cyc_PrattProver_MINUS:  {
# 311
int _tmp41=c1 - c2;int dif=_tmp41;
if(((c1 ^ c2)& (c1 ^ dif))>> 31){
# 314
if(dif > 0)return({struct _tuple12 _tmpFA;_tmpFA.f1=1,_tmpFA.f2=-2147483648;_tmpFA;});else{
return({struct _tuple12 _tmpFB;_tmpFB.f1=1,_tmpFB.f2=2147483647;_tmpFB;});}}else{
return({struct _tuple12 _tmpFC;_tmpFC.f1=0,_tmpFC.f2=dif;_tmpFC;});}}default:
# 318
({void*_tmp42=0U;({int(*_tmp137)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp136=({const char*_tmp43="Pratt_Prover: bad operator in overflow check";_tag_fat(_tmp43,sizeof(char),45U);});_tmp137(_tmp136,_tag_fat(_tmp42,sizeof(void*),0));});});};}
# 326
static struct _tuple12 Cyc_PrattProver_is_unsigned_overflow(unsigned c1,enum Cyc_PrattProver_Operator op,unsigned c2){
# 328
enum Cyc_PrattProver_Operator _tmp44=op;if(_tmp44 == Cyc_PrattProver_MINUS){
# 330
int dif=(int)(c1 - c2);
if((int)(((c1 ^ c2)& ~(c1 ^ (unsigned)dif))>> 31U)){
# 333
if(dif > 0)return({struct _tuple12 _tmpFD;_tmpFD.f1=1,_tmpFD.f2=-2147483648;_tmpFD;});else{
return({struct _tuple12 _tmpFE;_tmpFE.f1=1,_tmpFE.f2=2147483647;_tmpFE;});}}else{
return({struct _tuple12 _tmpFF;_tmpFF.f1=0,_tmpFF.f2=dif;_tmpFF;});}}else{
# 337
({void*_tmp45=0U;({int(*_tmp139)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp138=({const char*_tmp46="Pratt_Prover: bad operator in overflow check";_tag_fat(_tmp46,sizeof(char),45U);});_tmp139(_tmp138,_tag_fat(_tmp45,sizeof(void*),0));});});};}
# 349 "pratt_prover.cyc"
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*g,int is_signed,void*i,void*j,int dist,void*origin){
Cyc_PrattProver_add_node(g,i);
Cyc_PrattProver_add_node(g,j);{
struct Cyc_PrattProver_Dist*ij_dist=({struct Cyc_PrattProver_Row*_tmp13B=Cyc_PrattProver_lookup_row(g,i);int _tmp13A=is_signed;Cyc_PrattProver_lookup_distance(_tmp13B,_tmp13A,j);});
# 354
if(ij_dist != 0 && ij_dist->length < dist)
return 1;{
struct Cyc_PrattProver_Dist*ji_dist=({struct Cyc_PrattProver_Row*_tmp13D=Cyc_PrattProver_lookup_row(g,j);int _tmp13C=is_signed;Cyc_PrattProver_lookup_distance(_tmp13D,_tmp13C,i);});
# 358
if(ji_dist != 0){
struct _tuple12 _tmp47=Cyc_PrattProver_is_signed_overflow(ji_dist->length,0U,dist);struct _tuple12 _stmttmp4=_tmp47;struct _tuple12 _tmp48=_stmttmp4;int _tmp4A;int _tmp49;_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;{int overflow=_tmp49;int sum=_tmp4A;
if(sum < 0)
# 362
return 0;else{
if(overflow && sum > 0)
# 367
return 1;}}}{
# 373
struct Cyc_PrattProver_Row*_tmp4B=Cyc_PrattProver_lookup_row(g,j);struct Cyc_PrattProver_Row*jrow=_tmp4B;
{struct Cyc_PrattProver_Row*_tmp4C=g->rows;struct Cyc_PrattProver_Row*ks=_tmp4C;for(0;ks != 0;ks=ks->next){
void*_tmp4D=ks->source;void*k=_tmp4D;
struct Cyc_PrattProver_Dist*_tmp4E=Cyc_PrattProver_lookup_distance(ks,is_signed,i);struct Cyc_PrattProver_Dist*ki_dist=_tmp4E;
if(ki_dist == 0)continue;{
struct Cyc_PrattProver_Row*_tmp4F=g->rows;struct Cyc_PrattProver_Row*ls=_tmp4F;for(0;ls != 0;ls=ls->next){
void*_tmp50=ls->source;void*l=_tmp50;
struct Cyc_PrattProver_Dist*_tmp51=Cyc_PrattProver_lookup_distance(ks,is_signed,l);struct Cyc_PrattProver_Dist*kl_dist=_tmp51;
# 382
if(kl_dist != 0 && kl_dist->length == -2147483648)continue;{
# 384
struct Cyc_PrattProver_Dist*_tmp52=Cyc_PrattProver_lookup_distance(jrow,is_signed,l);struct Cyc_PrattProver_Dist*jl_dist=_tmp52;
if(jl_dist == 0)continue;{
# 388
int final;
int sum;
struct _tuple12 _tmp53=Cyc_PrattProver_is_signed_overflow(ki_dist->length,0U,dist);struct _tuple12 _stmttmp5=_tmp53;struct _tuple12 _tmp54=_stmttmp5;int _tmp56;int _tmp55;_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;{int of1=_tmp55;int sum1=_tmp56;
if(!of1){
# 393
struct _tuple12 _tmp57=Cyc_PrattProver_is_signed_overflow(sum1,0U,jl_dist->length);struct _tuple12 _stmttmp6=_tmp57;struct _tuple12 _tmp58=_stmttmp6;int _tmp5A;int _tmp59;_tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;{int of2=_tmp59;int sum2=_tmp5A;
final=of2;
sum=sum2;}}else{
# 399
struct _tuple12 _tmp5B=Cyc_PrattProver_is_signed_overflow(ki_dist->length,0U,jl_dist->length);struct _tuple12 _stmttmp7=_tmp5B;struct _tuple12 _tmp5C=_stmttmp7;int _tmp5E;int _tmp5D;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{int of1=_tmp5D;int sum1=_tmp5E;
if(!of1){
struct _tuple12 _tmp5F=Cyc_PrattProver_is_signed_overflow(sum1,0U,dist);struct _tuple12 _stmttmp8=_tmp5F;struct _tuple12 _tmp60=_stmttmp8;int _tmp62;int _tmp61;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;{int of2=_tmp61;int sum2=_tmp62;
final=of2;
sum=sum;}}else{
# 407
final=1;
if(dist < 0)
# 410
sum=-2147483648;else{
# 413
sum=2147483647;}}}}
# 417
if(!final || sum < 0){
# 420
if(kl_dist == 0 || kl_dist->length > sum)
Cyc_PrattProver_insert_distance(ks,is_signed,l,sum,0);}}}}}}}}
# 425
return 1;}}}}
# 428
static struct _tuple11 Cyc_PrattProver_eval_term(void*t){
void*_tmp63=t;void*_tmp64;unsigned _tmp65;switch(*((int*)_tmp63)){case 0: _tmp65=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp63)->f1;{unsigned i=_tmp65;
return({struct _tuple11 _tmp100;_tmp100.f1=i,_tmp100.f2=1;_tmp100;});}case 1: _tmp64=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp63)->f1;{struct Cyc_Absyn_Exp*e=_tmp64;
return Cyc_Evexp_eval_const_uint_exp(e);}default:
 return({struct _tuple11 _tmp101;_tmp101.f1=0U,_tmp101.f2=0;_tmp101;});};}struct _tuple13{void*f1;int f2;};
# 436
static struct _tuple13 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,int is_signed,void*t){
int c=0;
unsigned cu=0U;
void*res=t;
enum Cyc_Absyn_Primop p;
{void*_tmp66=t;void*_tmp67;if(*((int*)_tmp66)== 4)switch((int)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp66)->f1){case Cyc_Absyn_Plus: _tmp67=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp66)->f2;{struct Cyc_List_List*ts=_tmp67;
# 443
p=0U;
if(ts == 0)
({void*_tmp68=0U;({int(*_tmp13F)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp13E=({const char*_tmp69="no argument for primop";_tag_fat(_tmp69,sizeof(char),23U);});_tmp13F(_tmp13E,_tag_fat(_tmp68,sizeof(void*),0));});});else{
if(ts->tl == 0)
# 448
return({struct _tuple13 _tmp102;_tmp102.f1=(void*)ts->hd,_tmp102.f2=0;_tmp102;});else{
if(((struct Cyc_List_List*)_check_null(ts->tl))->tl == 0){
struct _tuple11 _tmp6A=Cyc_PrattProver_eval_term((void*)ts->hd);struct _tuple11 _stmttmp9=_tmp6A;struct _tuple11 _tmp6B=_stmttmp9;int _tmp6D;unsigned _tmp6C;_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{unsigned c1=_tmp6C;int okay1=_tmp6D;
struct _tuple11 _tmp6E=Cyc_PrattProver_eval_term((void*)((struct Cyc_List_List*)_check_null(ts->tl))->hd);struct _tuple11 _stmttmpA=_tmp6E;struct _tuple11 _tmp6F=_stmttmpA;int _tmp71;unsigned _tmp70;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{unsigned c2=_tmp70;int okay2=_tmp71;
if(okay1){
res=(void*)((struct Cyc_List_List*)_check_null(ts->tl))->hd;
c=(int)c1;
cu=c1;}else{
if(okay2){
res=(void*)ts->hd;
c=(int)c2;
cu=c2;}else{
return({struct _tuple13 _tmp103;_tmp103.f1=t,_tmp103.f2=0;_tmp103;});}}}}}else{
# 463
return({struct _tuple13 _tmp104;_tmp104.f1=t,_tmp104.f2=0;_tmp104;});}}}
goto _LL0;}case Cyc_Absyn_Minus: _tmp67=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp66)->f2;{struct Cyc_List_List*ts=_tmp67;
# 466
p=2U;
if(ts == 0)
({void*_tmp72=0U;({int(*_tmp141)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp140=({const char*_tmp73="no argument for primop";_tag_fat(_tmp73,sizeof(char),23U);});_tmp141(_tmp140,_tag_fat(_tmp72,sizeof(void*),0));});});else{
if(ts->tl == 0)
# 471
return({struct _tuple13 _tmp105;_tmp105.f1=t,_tmp105.f2=0;_tmp105;});else{
if(((struct Cyc_List_List*)_check_null(ts->tl))->tl == 0){
struct _tuple11 _tmp74=Cyc_PrattProver_eval_term((void*)((struct Cyc_List_List*)_check_null(ts->tl))->hd);struct _tuple11 _stmttmpB=_tmp74;struct _tuple11 _tmp75=_stmttmpB;int _tmp77;unsigned _tmp76;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;{unsigned c2=_tmp76;int okay2=_tmp77;
if(okay2){
res=(void*)ts->hd;
c=-(int)c2;
cu=c2;}else{
# 479
return({struct _tuple13 _tmp106;_tmp106.f1=t,_tmp106.f2=0;_tmp106;});}}}else{
return({struct _tuple13 _tmp107;_tmp107.f1=t,_tmp107.f2=0;_tmp107;});}}}
goto _LL0;}default: goto _LL5;}else{_LL5:
# 483
 return({struct _tuple13 _tmp108;_tmp108.f1=t,_tmp108.f2=0;_tmp108;});}_LL0:;}
# 487
if(is_signed){
if(c == 0)return({struct _tuple13 _tmp109;_tmp109.f1=res,_tmp109.f2=0;_tmp109;});else{
if(c > 0){
# 491
struct Cyc_PrattProver_Dist*_tmp78=({struct Cyc_PrattProver_Row*_tmp142=Cyc_PrattProver_lookup_row(g,res);Cyc_PrattProver_lookup_distance(_tmp142,1,Cyc_AssnDef_zero());});struct Cyc_PrattProver_Dist*dist=_tmp78;
if(dist == 0 || dist->length > 2147483647 - c)
return({struct _tuple13 _tmp10A;_tmp10A.f1=t,_tmp10A.f2=0;_tmp10A;});else{
# 495
return({struct _tuple13 _tmp10B;_tmp10B.f1=res,_tmp10B.f2=c;_tmp10B;});}}else{
# 498
struct Cyc_PrattProver_Dist*_tmp79=({struct Cyc_PrattProver_Row*_tmp144=({struct Cyc_PrattProver_Graph*_tmp143=g;Cyc_PrattProver_lookup_row(_tmp143,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_tmp144,1,res);});struct Cyc_PrattProver_Dist*dist=_tmp79;
if(dist == 0 || dist->length > c - -2147483648)
return({struct _tuple13 _tmp10C;_tmp10C.f1=t,_tmp10C.f2=0;_tmp10C;});else{
# 505
return({struct _tuple13 _tmp10D;_tmp10D.f1=res,_tmp10D.f2=c;_tmp10D;});}}}}else{
# 509
if((int)p == 0){
struct Cyc_PrattProver_Dist*_tmp7A=({struct Cyc_PrattProver_Row*_tmp145=Cyc_PrattProver_lookup_row(g,res);Cyc_PrattProver_lookup_distance(_tmp145,0,Cyc_AssnDef_zero());});struct Cyc_PrattProver_Dist*dist=_tmp7A;
if(dist == 0)return({struct _tuple13 _tmp10E;_tmp10E.f1=t,_tmp10E.f2=0;_tmp10E;});
if(dist->length <= 0)return({struct _tuple13 _tmp10F;_tmp10F.f1=res,_tmp10F.f2=(int)cu;_tmp10F;});
if(dist->length > 0 &&(unsigned)dist->length <= 4294967295U - cu)
return({struct _tuple13 _tmp110;_tmp110.f1=res,_tmp110.f2=(int)cu;_tmp110;});}else{
# 517
return({struct _tuple13 _tmp111;_tmp111.f1=t,_tmp111.f2=0;_tmp111;});}}
# 520
return({struct _tuple13 _tmp112;_tmp112.f1=t,_tmp112.f2=0;_tmp112;});}
# 525
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
# 528
struct _tuple11 _tmp7B=Cyc_PrattProver_eval_term(t1);struct _tuple11 _stmttmpC=_tmp7B;struct _tuple11 _tmp7C=_stmttmpC;int _tmp7E;unsigned _tmp7D;_tmp7D=_tmp7C.f1;_tmp7E=_tmp7C.f2;{unsigned c1=_tmp7D;int okay1=_tmp7E;
struct _tuple11 _tmp7F=Cyc_PrattProver_eval_term(t2);struct _tuple11 _stmttmpD=_tmp7F;struct _tuple11 _tmp80=_stmttmpD;int _tmp82;unsigned _tmp81;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{unsigned c2=_tmp81;int okay2=_tmp82;
void*_tmp83=Cyc_AssnDef_prim(t1,p,t2);void*origin=_tmp83;
# 533
if(okay1 && okay2){
# 535
enum Cyc_AssnDef_Primreln _tmp84=p;switch((int)_tmp84){case Cyc_AssnDef_ULt:
 return c1 < c2;case Cyc_AssnDef_SLt:
 return(int)c1 < (int)c2;case Cyc_AssnDef_ULte:
 return c1 <= c2;case Cyc_AssnDef_SLte:
 return(int)c1 <= (int)c2;default:
({void*_tmp85=0U;({int(*_tmp147)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp146=({const char*_tmp86="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp86,sizeof(char),42U);});_tmp147(_tmp146,_tag_fat(_tmp85,sizeof(void*),0));});});};}else{
# 542
if(okay2){
# 544
enum Cyc_AssnDef_Primreln _tmp87=p;switch((int)_tmp87){case Cyc_AssnDef_ULt:
# 547
 if(c2 == 0U)return 0;
c2=c2 - 1U;
goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 552
 if(c2 <= 2147483647U){
if(!({struct Cyc_PrattProver_Graph*_tmp14B=g;void*_tmp14A=t1;void*_tmp149=Cyc_AssnDef_zero();int _tmp148=(int)c2;Cyc_PrattProver_add_edge(_tmp14B,0,_tmp14A,_tmp149,_tmp148,origin);}))return 0;
# 555
return({struct Cyc_PrattProver_Graph*_tmp14E=g;void*_tmp14D=t1;void*_tmp14C=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmp14E,1,_tmp14D,_tmp14C,(int)c2,0);});}else{
# 557
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_tmp14F=Cyc_PrattProver_lookup_row(g,t1);Cyc_PrattProver_lookup_distance(_tmp14F,1,Cyc_AssnDef_zero());});
if(c1 != 0 && c1->length <= -1)
return({struct Cyc_PrattProver_Graph*_tmp152=g;void*_tmp151=t1;void*_tmp150=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmp152,1,_tmp151,_tmp150,(int)c2,0);});}
# 562
return 1;case Cyc_AssnDef_SLt:
# 565
 if(c2 == 2147483648U)return 0;
c2=c2 - 1U;
goto _LL19;case Cyc_AssnDef_SLte: _LL19:
# 570
 if(!({struct Cyc_PrattProver_Graph*_tmp156=g;void*_tmp155=t1;void*_tmp154=Cyc_AssnDef_zero();int _tmp153=(int)c2;Cyc_PrattProver_add_edge(_tmp156,1,_tmp155,_tmp154,_tmp153,origin);}))return 0;
if((int)c2 > 0){
# 573
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_tmp158=({struct Cyc_PrattProver_Graph*_tmp157=g;Cyc_PrattProver_lookup_row(_tmp157,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_tmp158,1,t1);});
if(c1 != 0 && c1->length <= 0)
# 576
return({struct Cyc_PrattProver_Graph*_tmp15B=g;void*_tmp15A=t1;void*_tmp159=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmp15B,0,_tmp15A,_tmp159,(int)c2,0);});}
# 578
return 1;default:
({void*_tmp88=0U;({int(*_tmp15D)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp15C=({const char*_tmp89="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp89,sizeof(char),42U);});_tmp15D(_tmp15C,_tag_fat(_tmp88,sizeof(void*),0));});});};}else{
# 581
if(okay1){
# 583
enum Cyc_AssnDef_Primreln _tmp8A=p;switch((int)_tmp8A){case Cyc_AssnDef_ULt:
# 586
 if(c1 == 4294967295U)return 0;
c1=c1 + 1U;
goto _LL20;case Cyc_AssnDef_ULte: _LL20:
# 591
 if(c1 > 2147483647U){
# 594
if(!({struct Cyc_PrattProver_Graph*_tmp15F=g;void*_tmp15E=t2;Cyc_PrattProver_add_edge(_tmp15F,1,_tmp15E,Cyc_AssnDef_zero(),-1,0);}))return 0;{
int k=-(int)c1;
if(k > 0){
if(!({struct Cyc_PrattProver_Graph*_tmp162=g;void*_tmp161=Cyc_AssnDef_zero();void*_tmp160=t2;Cyc_PrattProver_add_edge(_tmp162,1,_tmp161,_tmp160,k,0);}))return 0;}}}
# 600
if(c1 <= 2147483648U){
# 602
int k=-(int)c1;
return({struct Cyc_PrattProver_Graph*_tmp166=g;void*_tmp165=Cyc_AssnDef_zero();void*_tmp164=t2;int _tmp163=k;Cyc_PrattProver_add_edge(_tmp166,0,_tmp165,_tmp164,_tmp163,origin);});}
# 605
return 1;case Cyc_AssnDef_SLt:
# 608
 if(c1 == 2147483647U)return 0;
c1=c1 + 1U;
goto _LL24;case Cyc_AssnDef_SLte: _LL24:
# 613
 if(c1 == 2147483648U)return 1;else{
# 615
int k=-(int)c1;
if((int)c1 >= 0){
# 618
if(!({struct Cyc_PrattProver_Graph*_tmp169=g;void*_tmp168=Cyc_AssnDef_zero();void*_tmp167=t2;Cyc_PrattProver_add_edge(_tmp169,0,_tmp168,_tmp167,k,0);}))return 0;}
# 620
return({struct Cyc_PrattProver_Graph*_tmp16D=g;void*_tmp16C=Cyc_AssnDef_zero();void*_tmp16B=t2;int _tmp16A=k;Cyc_PrattProver_add_edge(_tmp16D,1,_tmp16C,_tmp16B,_tmp16A,origin);});}default:
# 622
({void*_tmp8B=0U;({int(*_tmp16F)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp16E=({const char*_tmp8C="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp8C,sizeof(char),42U);});_tmp16F(_tmp16E,_tag_fat(_tmp8B,sizeof(void*),0));});});};}}}{
# 629
enum Cyc_AssnDef_Primreln _tmp8D=p;switch((int)_tmp8D){case Cyc_AssnDef_ULt:  {
# 632
struct _tuple13 _tmp8E=Cyc_PrattProver_break_term(g,0,t1);struct _tuple13 _stmttmpE=_tmp8E;struct _tuple13 _tmp8F=_stmttmpE;int _tmp91;void*_tmp90;_tmp90=_tmp8F.f1;_tmp91=_tmp8F.f2;{void*tt1=_tmp90;int c1=_tmp91;
struct _tuple13 _tmp92=Cyc_PrattProver_break_term(g,0,t2);struct _tuple13 _stmttmpF=_tmp92;struct _tuple13 _tmp93=_stmttmpF;int _tmp95;void*_tmp94;_tmp94=_tmp93.f1;_tmp95=_tmp93.f2;{void*tt2=_tmp94;int c2=_tmp95;
struct _tuple12 _tmp96=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);struct _tuple12 _stmttmp10=_tmp96;struct _tuple12 _tmp97=_stmttmp10;int _tmp99;int _tmp98;_tmp98=_tmp97.f1;_tmp99=_tmp97.f2;{int of=_tmp98;int dist=_tmp99;
if(!of){
struct _tuple12 _tmp9A=Cyc_PrattProver_is_unsigned_overflow((unsigned)dist,1U,1U);struct _tuple12 _stmttmp11=_tmp9A;struct _tuple12 _tmp9B=_stmttmp11;int _tmp9D;int _tmp9C;_tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;{int of=_tmp9C;int dist=_tmp9D;
if(!of){
if(!Cyc_PrattProver_add_edge(g,0,tt1,tt2,dist,origin))
return 0;}}}
# 642
return Cyc_PrattProver_add_edge(g,0,t1,t2,-1,origin);}}}}case Cyc_AssnDef_ULte:  {
# 645
struct _tuple13 _tmp9E=Cyc_PrattProver_break_term(g,0,t1);struct _tuple13 _stmttmp12=_tmp9E;struct _tuple13 _tmp9F=_stmttmp12;int _tmpA1;void*_tmpA0;_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;{void*tt1=_tmpA0;int c1=_tmpA1;
struct _tuple13 _tmpA2=Cyc_PrattProver_break_term(g,0,t2);struct _tuple13 _stmttmp13=_tmpA2;struct _tuple13 _tmpA3=_stmttmp13;int _tmpA5;void*_tmpA4;_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;{void*tt2=_tmpA4;int c2=_tmpA5;
struct _tuple12 _tmpA6=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);struct _tuple12 _stmttmp14=_tmpA6;struct _tuple12 _tmpA7=_stmttmp14;int _tmpA9;int _tmpA8;_tmpA8=_tmpA7.f1;_tmpA9=_tmpA7.f2;{int of=_tmpA8;int dist=_tmpA9;
if(!of){
# 651
if(!Cyc_PrattProver_add_edge(g,0,tt1,tt2,dist,origin))
return 0;}
# 654
return Cyc_PrattProver_add_edge(g,0,t1,t2,0,origin);}}}}case Cyc_AssnDef_SLt:  {
# 656
struct _tuple13 _tmpAA=Cyc_PrattProver_break_term(g,1,t1);struct _tuple13 _stmttmp15=_tmpAA;struct _tuple13 _tmpAB=_stmttmp15;int _tmpAD;void*_tmpAC;_tmpAC=_tmpAB.f1;_tmpAD=_tmpAB.f2;{void*tt1=_tmpAC;int c1=_tmpAD;
struct _tuple13 _tmpAE=Cyc_PrattProver_break_term(g,1,t2);struct _tuple13 _stmttmp16=_tmpAE;struct _tuple13 _tmpAF=_stmttmp16;int _tmpB1;void*_tmpB0;_tmpB0=_tmpAF.f1;_tmpB1=_tmpAF.f2;{void*tt2=_tmpB0;int c2=_tmpB1;
struct _tuple12 _tmpB2=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);struct _tuple12 _stmttmp17=_tmpB2;struct _tuple12 _tmpB3=_stmttmp17;int _tmpB5;int _tmpB4;_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;{int of=_tmpB4;int dist=_tmpB5;
if(!of){
struct _tuple12 _tmpB6=Cyc_PrattProver_is_signed_overflow(dist,1U,1);struct _tuple12 _stmttmp18=_tmpB6;struct _tuple12 _tmpB7=_stmttmp18;int _tmpB9;int _tmpB8;_tmpB8=_tmpB7.f1;_tmpB9=_tmpB7.f2;{int of=_tmpB8;int dist=_tmpB9;
if(!of){
if(!Cyc_PrattProver_add_edge(g,1,tt1,tt2,dist,origin))
return 0;}}}
# 666
return Cyc_PrattProver_add_edge(g,1,t1,t2,-1,origin);}}}}case Cyc_AssnDef_SLte:  {
# 669
struct _tuple13 _tmpBA=Cyc_PrattProver_break_term(g,1,t1);struct _tuple13 _stmttmp19=_tmpBA;struct _tuple13 _tmpBB=_stmttmp19;int _tmpBD;void*_tmpBC;_tmpBC=_tmpBB.f1;_tmpBD=_tmpBB.f2;{void*tt1=_tmpBC;int c1=_tmpBD;
struct _tuple13 _tmpBE=Cyc_PrattProver_break_term(g,1,t2);struct _tuple13 _stmttmp1A=_tmpBE;struct _tuple13 _tmpBF=_stmttmp1A;int _tmpC1;void*_tmpC0;_tmpC0=_tmpBF.f1;_tmpC1=_tmpBF.f2;{void*tt2=_tmpC0;int c2=_tmpC1;
struct _tuple12 _tmpC2=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);struct _tuple12 _stmttmp1B=_tmpC2;struct _tuple12 _tmpC3=_stmttmp1B;int _tmpC5;int _tmpC4;_tmpC4=_tmpC3.f1;_tmpC5=_tmpC3.f2;{int of=_tmpC4;int dist=_tmpC5;
if(!of){
if(!Cyc_PrattProver_add_edge(g,1,tt1,tt2,dist,origin))
return 0;}
# 676
return Cyc_PrattProver_add_edge(g,1,t1,t2,0,origin);}}}}default:
({void*_tmpC6=0U;({int(*_tmp171)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp170=({const char*_tmpC7="Vcgen:found bad primop in add_constraint";_tag_fat(_tmpC7,sizeof(char),41U);});_tmp171(_tmp170,_tag_fat(_tmpC6,sizeof(void*),0));});});};}}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 682
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 685
unsigned Cyc_PrattProver_max_paths=33U;
unsigned Cyc_PrattProver_max_paths_seen=0U;
# 694
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,void*a){
LOOP:
# 698
 if(gs == 0)return gs;
{void*_tmpC8=a;enum Cyc_AssnDef_Primreln _tmpCA;void*_tmpCB;void*_tmpC9;switch(*((int*)_tmpC8)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _tmpC9=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpC8)->f1;_tmpCB=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpC8)->f2;{void*a1=_tmpC9;void*a2=_tmpCB;
# 703
gs=Cyc_PrattProver_cgraph(gs,a1);
a=a2;
goto LOOP;}case 4: _tmpC9=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpC8)->f1;_tmpCB=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpC8)->f2;{void*a1=_tmpC9;void*a2=_tmpCB;
# 707
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(Cyc_PrattProver_max_paths_seen < n)Cyc_PrattProver_max_paths_seen=n;
# 710
if(n >= Cyc_PrattProver_max_paths)(int)_throw((void*)& Cyc_PrattProver_too_large);{
# 712
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graph(gs);
# 715
gs1=Cyc_PrattProver_cgraph(gs1,a1);
# 717
gs2=Cyc_PrattProver_cgraph(gs2,a2);
# 719
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}case 2: switch((int)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f2){case Cyc_AssnDef_Neq: _tmpC9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f1;_tmpCB=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f3;{void*t1=_tmpC9;void*t2=_tmpCB;
# 724
a=({void*_tmp174=({void*_tmp172=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_tmp172,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_tmp174,({void*_tmp173=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_tmp173,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}case Cyc_AssnDef_Eq: _tmpC9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f1;_tmpCB=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f3;{void*t1=_tmpC9;void*t2=_tmpCB;
# 728
a=({void*_tmp177=({void*_tmp176=({void*_tmp175=Cyc_AssnDef_slte(t1,t2);Cyc_AssnDef_and(_tmp175,Cyc_AssnDef_slte(t2,t1));});Cyc_AssnDef_and(_tmp176,Cyc_AssnDef_ulte(t1,t2));});Cyc_AssnDef_and(_tmp177,Cyc_AssnDef_ulte(t2,t1));});
goto LOOP;}default: _tmpC9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f1;_tmpCA=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f2;_tmpCB=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC8)->f3;{void*t1=_tmpC9;enum Cyc_AssnDef_Primreln p=_tmpCA;void*t2=_tmpCB;
# 733
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*_tmpCC=gs;struct Cyc_PrattProver_Graph*g=_tmpCC;for(0;g != 0;g=g->next){
if(!Cyc_PrattProver_add_constraint(g,t1,p,t2)){
if(prev == 0)
gs=g->next;else{
# 739
prev->next=g->next;}}else{
# 742
prev=g;}}}
# 745
goto _LL0;}}default:
# 748
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}
# 752
return gs;}
# 758
static int Cyc_PrattProver_consistent(void*a){
struct _handler_cons _tmpCD;_push_handler(& _tmpCD);{int _tmpCF=0;if(setjmp(_tmpCD.handler))_tmpCF=1;if(!_tmpCF){
# 761
{struct _fat_ptr gs=_tag_fat(({struct Cyc_PrattProver_Graph*_tmp178=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_tmp178,a);}),sizeof(struct Cyc_PrattProver_Graph),1U);
# 764
int _tmpD0=({struct Cyc_PrattProver_Graph*_tmp179=(struct Cyc_PrattProver_Graph*)gs.curr;_tmp179 != (struct Cyc_PrattProver_Graph*)(_tag_fat(0,0,0)).curr;});_npop_handler(0);return _tmpD0;}
# 761
;_pop_handler();}else{void*_tmpCE=(void*)Cyc_Core_get_exn_thrown();void*_tmpD1=_tmpCE;void*_tmpD2;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_tmpD1)->tag == Cyc_PrattProver_TooLarge)
# 768
return 1;else{_tmpD2=_tmpD1;{void*exn=_tmpD2;(int)_rethrow(exn);}};}}}
# 773
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_tmp17A=ctxt;Cyc_AssnDef_and(_tmp17A,Cyc_AssnDef_not(a));});
# 778
return !Cyc_PrattProver_consistent(b);}
# 781
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row_in_rows(struct Cyc_PrattProver_Row*r,void*i){
for(1;r != 0;r=r->next){
if(Cyc_AssnDef_cmp_term(i,r->source)== 0)return r;}
# 785
return 0;}
# 788
static struct Cyc_PrattProver_Row*Cyc_PrattProver_or2rows(struct Cyc_PrattProver_Row*g1,struct Cyc_PrattProver_Row*g2){
struct Cyc_PrattProver_Row*res=0;
for(1;g1 != 0;g1=g1->next){
struct Cyc_PrattProver_Row*_tmpD3=Cyc_PrattProver_lookup_row_in_rows(g2,g1->source);struct Cyc_PrattProver_Row*r=_tmpD3;
if(r != 0){
# 794
struct Cyc_PrattProver_Row*_tmpD4=({struct Cyc_PrattProver_Row*_tmpE0=_cycalloc(sizeof(*_tmpE0));*_tmpE0=*g1;_tmpE0;});struct Cyc_PrattProver_Row*newrow=_tmpD4;
newrow->next=res;
res=newrow;
res->distance=0;{
# 799
struct Cyc_PrattProver_Distance*_tmpD5=g1->distance;struct Cyc_PrattProver_Distance*d=_tmpD5;for(0;d != 0;d=d->next){
struct Cyc_PrattProver_Distance*dist_res=0;
if((int)d->dinfo & 2){
int _tmpD6=(d->signed_dist).length;int d1=_tmpD6;
struct Cyc_PrattProver_Dist*_tmpD7=Cyc_PrattProver_lookup_distance(r,1,d->target);struct Cyc_PrattProver_Dist*d2_ptr=_tmpD7;
if(d2_ptr != 0){
int _tmpD8=d2_ptr->length;int d2=_tmpD8;
struct Cyc_PrattProver_Dist _tmpD9=d1 > d2?d->signed_dist:*d2_ptr;struct Cyc_PrattProver_Dist newd=_tmpD9;
dist_res=({struct Cyc_PrattProver_Distance*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->target=d->target,_tmpDA->next=res->distance,_tmpDA->dinfo=2,
# 810
(_tmpDA->unsigned_dist).length=0,(_tmpDA->unsigned_dist).prim=0,_tmpDA->signed_dist=newd;_tmpDA;});}}
# 814
if((int)d->dinfo & 1){
int _tmpDB=(d->unsigned_dist).length;int d1=_tmpDB;
struct Cyc_PrattProver_Dist*_tmpDC=Cyc_PrattProver_lookup_distance(r,0,d->target);struct Cyc_PrattProver_Dist*d2_ptr=_tmpDC;
if(d2_ptr != 0){
int _tmpDD=d2_ptr->length;int d2=_tmpDD;
struct Cyc_PrattProver_Dist _tmpDE=d1 > d2?d->unsigned_dist:*d2_ptr;struct Cyc_PrattProver_Dist newd=_tmpDE;
if(dist_res != 0){
dist_res->unsigned_dist=newd;
dist_res->dinfo=3;}else{
# 824
dist_res=({struct Cyc_PrattProver_Distance*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->target=d->target,_tmpDF->next=res->distance,_tmpDF->dinfo=1,_tmpDF->unsigned_dist=newd,
# 828
(_tmpDF->signed_dist).length=0,(_tmpDF->signed_dist).prim=0;_tmpDF;});}}}
# 832
if(dist_res != 0)
res->distance=dist_res;}}}}
# 838
return res;}
# 841
void*Cyc_PrattProver_int2term(int is_signed,int c){
enum Cyc_Absyn_Sign _tmpE1=is_signed?0U: 1U;enum Cyc_Absyn_Sign s=_tmpE1;
struct Cyc_Absyn_Exp*_tmpE2=Cyc_Absyn_int_exp(s,c,0U);struct Cyc_Absyn_Exp*e=_tmpE2;
({void*_tmp17B=Cyc_Absyn_int_type(s,2U);e->topt=_tmp17B;});
return Cyc_AssnDef_cnst(e);}
# 848
void*Cyc_PrattProver_edge2assn(void*source,void*target,int weight,int is_signed){
enum Cyc_AssnDef_Primreln _tmpE3=is_signed?3U: 5U;enum Cyc_AssnDef_Primreln s=_tmpE3;
enum Cyc_AssnDef_Primreln _tmpE4=is_signed?2U: 4U;enum Cyc_AssnDef_Primreln s1=_tmpE4;
void*result=(void*)& Cyc_AssnDef_true_assn;
if(({void*_tmp17C=source;_tmp17C == Cyc_AssnDef_zero();}))
result=({void*_tmp17E=Cyc_PrattProver_int2term(is_signed,- weight);enum Cyc_AssnDef_Primreln _tmp17D=s;Cyc_AssnDef_prim(_tmp17E,_tmp17D,target);});else{
if(({void*_tmp17F=target;_tmp17F == Cyc_AssnDef_zero();}))
result=({void*_tmp181=source;enum Cyc_AssnDef_Primreln _tmp180=s;Cyc_AssnDef_prim(_tmp181,_tmp180,Cyc_PrattProver_int2term(is_signed,weight));});else{
# 857
int _tmpE5=weight;switch((int)_tmpE5){case 0:
 result=Cyc_AssnDef_prim(source,s,target);goto _LL0;case -1:
 result=Cyc_AssnDef_prim(source,s1,target);goto _LL0;default:
# 865
 goto _LL0;}_LL0:;}}
# 868
return result;}
# 872
void*Cyc_PrattProver_row2assn(struct Cyc_PrattProver_Row*r){
void*a=(void*)& Cyc_AssnDef_true_assn;
for(1;r != 0;r=r->next){
struct Cyc_PrattProver_Distance*_tmpE6=r->distance;struct Cyc_PrattProver_Distance*d=_tmpE6;for(0;d != 0;d=d->next){
if((int)d->dinfo & 2){
if((d->signed_dist).prim != 0)
a=Cyc_AssnDef_and(a,(void*)_check_null((d->signed_dist).prim));else{
# 880
a=({void*_tmp182=a;Cyc_AssnDef_and(_tmp182,Cyc_PrattProver_edge2assn(r->source,d->target,(d->signed_dist).length,1));});}}
# 883
if((int)d->dinfo & 1){
if((d->unsigned_dist).prim != 0)
a=Cyc_AssnDef_and(a,(void*)_check_null((d->unsigned_dist).prim));else{
# 887
a=({void*_tmp183=a;Cyc_AssnDef_and(_tmp183,Cyc_PrattProver_edge2assn(r->source,d->target,(d->unsigned_dist).length,0));});}}}}
# 892
return a;}
# 895
void*Cyc_PrattProver_merge_assn(void*a1,void*a2){
# 897
struct Cyc_PrattProver_Graph*_tmpE7=({struct Cyc_PrattProver_Graph*_tmp184=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_tmp184,Cyc_AssnDef_or(a1,a2));});struct Cyc_PrattProver_Graph*g=_tmpE7;
if(g == 0)return(void*)& Cyc_AssnDef_false_assn;{
struct Cyc_PrattProver_Row*_tmpE8=g->rows;struct Cyc_PrattProver_Row*row=_tmpE8;
{struct Cyc_PrattProver_Graph*_tmpE9=g->next;struct Cyc_PrattProver_Graph*row1=_tmpE9;for(0;row1 != 0;row1=row1->next){
row=Cyc_PrattProver_or2rows(row,row1->rows);}}
# 903
return Cyc_PrattProver_row2assn(row);}}
