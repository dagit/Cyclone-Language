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
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 328
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);
# 37 "position.h"
struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 904 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 59
void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*));struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 27 "vcgen.h"
void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 72
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 59
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 107
void*Cyc_Tcutil_compress(void*);
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 193
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Vcgen_Env;struct Cyc_Vcgen_Const_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Vcgen_Var_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;};struct Cyc_Vcgen_Primop_Vcgen_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};
# 117 "vcgen.cyc"
struct _fat_ptr Cyc_Vcgen_term2string(void*t){
void*_tmp0=t;void*_tmp2;enum Cyc_Absyn_Primop _tmp1;void*_tmp5;void*_tmp4;enum Cyc_Absyn_Primop _tmp3;int _tmp7;struct Cyc_Absyn_Vardecl*_tmp6;int _tmp8;struct Cyc_Absyn_Vardecl*_tmp9;struct Cyc_Absyn_Exp*_tmpA;switch(*((int*)_tmp0)){case 0U: _LL1: _tmpA=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp0)->f1;_LL2: {struct Cyc_Absyn_Exp*c=_tmpA;
return Cyc_Absynpp_exp2string(c);}case 1U: _LL3: _tmp9=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp0)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp9;
return Cyc_Absynpp_qvar2string(vd->name);}case 2U: if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f1 == 0){_LL5: _tmp8=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f2;_LL6: {int i=_tmp8;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=1U,_tmp2B0.f1=(unsigned long)i;_tmp2B0;});void*_tmpB[1U];_tmpB[0]=& _tmpD;({struct _fat_ptr _tmp2F4=({const char*_tmpC="_X%d";_tag_fat(_tmpC,sizeof(char),5U);});Cyc_aprintf(_tmp2F4,_tag_fat(_tmpB,sizeof(void*),1U));});});}}else{_LL7: _tmp6=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f1;_tmp7=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp0)->f2;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp6;int i=_tmp7;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp2B2;_tmp2B2.tag=0U,({struct _fat_ptr _tmp2F5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name));_tmp2B2.f1=_tmp2F5;});_tmp2B2;});struct Cyc_Int_pa_PrintArg_struct _tmp11=({struct Cyc_Int_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=1U,_tmp2B1.f1=(unsigned long)i;_tmp2B1;});void*_tmpE[2U];_tmpE[0]=& _tmp10,_tmpE[1]=& _tmp11;({struct _fat_ptr _tmp2F6=({const char*_tmpF="_%s%d";_tag_fat(_tmpF,sizeof(char),6U);});Cyc_aprintf(_tmp2F6,_tag_fat(_tmpE,sizeof(void*),2U));});});}}default: if(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_LL9: _tmp3=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f1;_tmp4=(void*)(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->hd;_tmp5=(void*)((((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->tl)->hd;_LLA: {enum Cyc_Absyn_Primop p=_tmp3;void*t1=_tmp4;void*t2=_tmp5;
# 124
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0U,({struct _fat_ptr _tmp2F7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t1));_tmp2B5.f1=_tmp2F7;});_tmp2B5;});struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp2B4;_tmp2B4.tag=0U,({
struct _fat_ptr _tmp2F8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp2B4.f1=_tmp2F8;});_tmp2B4;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0U,({struct _fat_ptr _tmp2F9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t2));_tmp2B3.f1=_tmp2F9;});_tmp2B3;});void*_tmp12[3U];_tmp12[0]=& _tmp14,_tmp12[1]=& _tmp15,_tmp12[2]=& _tmp16;({struct _fat_ptr _tmp2FA=({const char*_tmp13="(%s%s%s)";_tag_fat(_tmp13,sizeof(char),9U);});Cyc_aprintf(_tmp2FA,_tag_fat(_tmp12,sizeof(void*),3U));});});}}else{goto _LLB;}}else{_LLB: _tmp1=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f1;_tmp2=(void*)(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp0)->f2)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp1;void*t=_tmp2;
# 127
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp2B7;_tmp2B7.tag=0U,({struct _fat_ptr _tmp2FB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp2B7.f1=_tmp2FB;});_tmp2B7;});struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0U,({struct _fat_ptr _tmp2FC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t));_tmp2B6.f1=_tmp2FC;});_tmp2B6;});void*_tmp17[2U];_tmp17[0]=& _tmp19,_tmp17[1]=& _tmp1A;({struct _fat_ptr _tmp2FD=({const char*_tmp18="%s(%s)";_tag_fat(_tmp18,sizeof(char),7U);});Cyc_aprintf(_tmp2FD,_tag_fat(_tmp17,sizeof(void*),2U));});});}}}else{_LLD: _LLE:
({void*_tmp1B=0U;({struct _fat_ptr _tmp2FE=({const char*_tmp1C="term2string primop args";_tag_fat(_tmp1C,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2FE,_tag_fat(_tmp1B,sizeof(void*),0U));});});}}_LL0:;}
# 132
void*Cyc_Vcgen_cnst(struct Cyc_Absyn_Exp*c){return(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->tag=0U,_tmp1D->f1=c;_tmp1D;});}
void*Cyc_Vcgen_zero(){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp1E=Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U);struct Cyc_Absyn_Exp*e=_tmp1E;
({void*_tmp2FF=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp2FF;});{
void*_tmp1F=Cyc_Vcgen_cnst(e);void*r=_tmp1F;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 143
void*Cyc_Vcgen_one(){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp20=Cyc_Absyn_int_exp(Cyc_Absyn_None,1,0U);struct Cyc_Absyn_Exp*e=_tmp20;
({void*_tmp300=Cyc_Absyn_int_type(Cyc_Absyn_None,Cyc_Absyn_Int_sz);e->topt=_tmp300;});{
void*_tmp21=Cyc_Vcgen_cnst(e);void*r=_tmp21;
z=r;
return r;}}else{
return(void*)_check_null(z);}}
# 154
void*Cyc_Vcgen_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=1U,_tmp22->f1=vd;_tmp22;});}
void*Cyc_Vcgen_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=3U,_tmp23->f1=p,_tmp23->f2=ts;_tmp23;});}
# 158
void*Cyc_Vcgen_fresh_var(){
static int counter=0;
return(void*)({struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=2U,_tmp24->f1=0,_tmp24->f2=counter ++;_tmp24;});}struct _tuple11{void*f1;void*f2;};
# 163
int Cyc_Vcgen_same_term(void*t1,void*t2){
struct _tuple11 _tmp25=({struct _tuple11 _tmp2B8;_tmp2B8.f1=t1,_tmp2B8.f2=t2;_tmp2B8;});struct _tuple11 _stmttmp0=_tmp25;struct _tuple11 _tmp26=_stmttmp0;struct Cyc_List_List*_tmp2A;enum Cyc_Absyn_Primop _tmp29;struct Cyc_List_List*_tmp28;enum Cyc_Absyn_Primop _tmp27;int _tmp2E;struct Cyc_Absyn_Vardecl*_tmp2D;int _tmp2C;struct Cyc_Absyn_Vardecl*_tmp2B;struct Cyc_Absyn_Vardecl*_tmp30;struct Cyc_Absyn_Vardecl*_tmp2F;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Exp*_tmp31;switch(*((int*)_tmp26.f1)){case 0U: if(((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp26.f2)->tag == 0U){_LL1: _tmp31=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp26.f1)->f1;_tmp32=((struct Cyc_Vcgen_Const_Vcgen_Term_struct*)_tmp26.f2)->f1;_LL2: {struct Cyc_Absyn_Exp*c1=_tmp31;struct Cyc_Absyn_Exp*c2=_tmp32;
return Cyc_Evexp_same_const_exp(c1,c2);}}else{goto _LL9;}case 1U: if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp26.f2)->tag == 1U){_LL3: _tmp2F=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp26.f1)->f1;_tmp30=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmp26.f2)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd1=_tmp2F;struct Cyc_Absyn_Vardecl*vd2=_tmp30;
return vd1 == vd2;}}else{goto _LL9;}case 2U: if(((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp26.f2)->tag == 2U){_LL5: _tmp2B=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp26.f1)->f1;_tmp2C=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp26.f1)->f2;_tmp2D=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp26.f2)->f1;_tmp2E=((struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*)_tmp26.f2)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd1=_tmp2B;int i=_tmp2C;struct Cyc_Absyn_Vardecl*vd2=_tmp2D;int j=_tmp2E;
return vd1 == vd2 && i == j;}}else{goto _LL9;}default: if(((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp26.f2)->tag == 3U){_LL7: _tmp27=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp26.f1)->f1;_tmp28=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp26.f1)->f2;_tmp29=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp26.f2)->f1;_tmp2A=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmp26.f2)->f2;_LL8: {enum Cyc_Absyn_Primop p1=_tmp27;struct Cyc_List_List*ts1=_tmp28;enum Cyc_Absyn_Primop p2=_tmp29;struct Cyc_List_List*ts2=_tmp2A;
# 169
if((int)p1 != (int)p2)return 0;
for(0;ts1 != 0 && ts2 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Vcgen_same_term((void*)ts1->hd,(void*)ts2->hd))return 0;}
# 173
if(ts2 != 0 || ts1 != 0)return 0;
return 1;}}else{_LL9: _LLA:
 return 0;}}_LL0:;}
# 180
int Cyc_Vcgen_termcmp(void*t1,void*t2){
if((unsigned)Cyc_Vcgen_same_term)return 0;
return(int)t1 - (int)t2;}
# 188
enum Cyc_Vcgen_Primreln{Cyc_Vcgen_Eq =0U,Cyc_Vcgen_Neq =1U,Cyc_Vcgen_Lt =2U,Cyc_Vcgen_Lte =3U};struct Cyc_Vcgen_True_Vcgen_Assn_struct{int tag;};struct Cyc_Vcgen_False_Vcgen_Assn_struct{int tag;};struct Cyc_Vcgen_Prim_Vcgen_Assn_struct{int tag;void*f1;enum Cyc_Vcgen_Primreln f2;void*f3;};struct Cyc_Vcgen_And_Vcgen_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_Vcgen_Or_Vcgen_Assn_struct{int tag;void*f1;void*f2;};
# 198
unsigned Cyc_Vcgen_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
if(((int(*)(struct Cyc_List_List*l,void*x))Cyc_List_memq)(*prev,a))return c;
({struct Cyc_List_List*_tmp301=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=a,_tmp33->tl=*prev;_tmp33;});*prev=_tmp301;});{
void*_tmp34=a;void*_tmp36;void*_tmp35;void*_tmp38;void*_tmp37;switch(*((int*)_tmp34)){case 0U: _LL1: _LL2:
 return c;case 1U: _LL3: _LL4:
 return c;case 2U: _LL5: _LL6:
 return(unsigned)1 + c;case 3U: _LL7: _tmp37=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp34)->f1;_tmp38=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp34)->f2;_LL8: {void*a1=_tmp37;void*a2=_tmp38;
_tmp35=a1;_tmp36=a2;goto _LLA;}default: _LL9: _tmp35=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp34)->f1;_tmp36=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp34)->f2;_LLA: {void*a1=_tmp35;void*a2=_tmp36;
# 207
return({void*_tmp303=a1;unsigned _tmp302=Cyc_Vcgen_assn_size_loop(a2,(unsigned)1 + c,prev);Cyc_Vcgen_assn_size_loop(_tmp303,_tmp302,prev);});}}_LL0:;}}
# 211
static unsigned Cyc_Vcgen_max_assn_size=0U;
# 213
unsigned Cyc_Vcgen_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp39=Cyc_Vcgen_assn_size_loop(a,0U,& prev);unsigned c=_tmp39;
if(c > Cyc_Vcgen_max_assn_size){
Cyc_Vcgen_max_assn_size=c;
({struct Cyc_Int_pa_PrintArg_struct _tmp3C=({struct Cyc_Int_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=1U,_tmp2B9.f1=(unsigned long)((int)Cyc_Vcgen_max_assn_size);_tmp2B9;});void*_tmp3A[1U];_tmp3A[0]=& _tmp3C;({struct Cyc___cycFILE*_tmp305=Cyc_stderr;struct _fat_ptr _tmp304=({const char*_tmp3B="Max Assn Size = %d\n";_tag_fat(_tmp3B,sizeof(char),20U);});Cyc_fprintf(_tmp305,_tmp304,_tag_fat(_tmp3A,sizeof(void*),1U));});});}
# 220
return c;}
# 223
struct _fat_ptr Cyc_Vcgen_a2string(void*a,int prec,int*size){
void*_tmp3D=a;void*_tmp3F;void*_tmp3E;void*_tmp41;void*_tmp40;void*_tmp44;enum Cyc_Vcgen_Primreln _tmp43;void*_tmp42;switch(*((int*)_tmp3D)){case 0U: _LL1: _LL2:
 return({const char*_tmp45="true";_tag_fat(_tmp45,sizeof(char),5U);});case 1U: _LL3: _LL4:
 return({const char*_tmp46="false";_tag_fat(_tmp46,sizeof(char),6U);});case 2U: _LL5: _tmp42=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp3D)->f1;_tmp43=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp3D)->f2;_tmp44=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp3D)->f3;_LL6: {void*t1=_tmp42;enum Cyc_Vcgen_Primreln p=_tmp43;void*t2=_tmp44;
# 228
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_Vcgen_Primreln _tmp47=p;switch(_tmp47){case Cyc_Vcgen_Eq: _LLC: _LLD:
 ps=({const char*_tmp48="==";_tag_fat(_tmp48,sizeof(char),3U);});goto _LLB;case Cyc_Vcgen_Neq: _LLE: _LLF:
 ps=({const char*_tmp49="!=";_tag_fat(_tmp49,sizeof(char),3U);});goto _LLB;case Cyc_Vcgen_Lt: _LL10: _LL11:
 ps=({const char*_tmp4A="<";_tag_fat(_tmp4A,sizeof(char),2U);});goto _LLB;case Cyc_Vcgen_Lte: _LL12: _LL13:
 ps=({const char*_tmp4B="<=";_tag_fat(_tmp4B,sizeof(char),3U);});goto _LLB;default: _LL14: _LL15:
({void*_tmp4C=0U;({struct _fat_ptr _tmp306=({const char*_tmp4D="assn2string primop";_tag_fat(_tmp4D,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp306,_tag_fat(_tmp4C,sizeof(void*),0U));});});}_LLB:;}
# 237
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0U,({struct _fat_ptr _tmp307=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t1));_tmp2BC.f1=_tmp307;});_tmp2BC;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0U,_tmp2BB.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp2BB;});struct Cyc_String_pa_PrintArg_struct _tmp52=({struct Cyc_String_pa_PrintArg_struct _tmp2BA;_tmp2BA.tag=0U,({struct _fat_ptr _tmp308=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_term2string(t2));_tmp2BA.f1=_tmp308;});_tmp2BA;});void*_tmp4E[3U];_tmp4E[0]=& _tmp50,_tmp4E[1]=& _tmp51,_tmp4E[2]=& _tmp52;({struct _fat_ptr _tmp309=({const char*_tmp4F="%s%s%s";_tag_fat(_tmp4F,sizeof(char),7U);});Cyc_aprintf(_tmp309,_tag_fat(_tmp4E,sizeof(void*),3U));});});}}case 4U: _LL7: _tmp40=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp3D)->f1;_tmp41=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp3D)->f2;_LL8: {void*a1=_tmp40;void*a2=_tmp41;
# 239
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp2BE;_tmp2BE.tag=0U,({struct _fat_ptr _tmp30A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,5,size));_tmp2BE.f1=_tmp30A;});_tmp2BE;});struct Cyc_String_pa_PrintArg_struct _tmp56=({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0U,({struct _fat_ptr _tmp30B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,5,size));_tmp2BD.f1=_tmp30B;});_tmp2BD;});void*_tmp53[2U];_tmp53[0]=& _tmp55,_tmp53[1]=& _tmp56;({struct _fat_ptr _tmp30C=({const char*_tmp54="%s || %s";_tag_fat(_tmp54,sizeof(char),9U);});Cyc_aprintf(_tmp30C,_tag_fat(_tmp53,sizeof(void*),2U));});});else{
# 243
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp2C0;_tmp2C0.tag=0U,({struct _fat_ptr _tmp30D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,5,size));_tmp2C0.f1=_tmp30D;});_tmp2C0;});struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0U,({struct _fat_ptr _tmp30E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,5,size));_tmp2BF.f1=_tmp30E;});_tmp2BF;});void*_tmp57[2U];_tmp57[0]=& _tmp59,_tmp57[1]=& _tmp5A;({struct _fat_ptr _tmp30F=({const char*_tmp58="(%s || %s)";_tag_fat(_tmp58,sizeof(char),11U);});Cyc_aprintf(_tmp30F,_tag_fat(_tmp57,sizeof(void*),2U));});});}}default: _LL9: _tmp3E=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp3D)->f1;_tmp3F=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp3D)->f2;_LLA: {void*a1=_tmp3E;void*a2=_tmp3F;
# 245
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0U,({struct _fat_ptr _tmp310=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a1,10,size));_tmp2C2.f1=_tmp310;});_tmp2C2;});struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp311=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_a2string(a2,10,size));_tmp2C1.f1=_tmp311;});_tmp2C1;});void*_tmp5B[2U];_tmp5B[0]=& _tmp5D,_tmp5B[1]=& _tmp5E;({struct _fat_ptr _tmp312=({const char*_tmp5C="%s && %s";_tag_fat(_tmp5C,sizeof(char),9U);});Cyc_aprintf(_tmp312,_tag_fat(_tmp5B,sizeof(void*),2U));});});}}_LL0:;}
# 250
static int Cyc_Vcgen_tree_size=0;
# 252
struct _fat_ptr Cyc_Vcgen_assn2string(void*a){
Cyc_Vcgen_tree_size=0;
return Cyc_Vcgen_a2string(a,0,& Cyc_Vcgen_tree_size);}
# 257
struct Cyc_Vcgen_True_Vcgen_Assn_struct Cyc_Vcgen_true_assn={0U};
struct Cyc_Vcgen_False_Vcgen_Assn_struct Cyc_Vcgen_false_assn={1U};
# 269 "vcgen.cyc"
int Cyc_Vcgen_assnhash(void*a){
void*_tmp5F=a;void*_tmp61;void*_tmp60;void*_tmp63;void*_tmp62;void*_tmp66;enum Cyc_Vcgen_Primreln _tmp65;void*_tmp64;switch(*((int*)_tmp5F)){case 1U: _LL1: _LL2:
 return 0;case 0U: _LL3: _LL4:
 return 1;case 2U: _LL5: _tmp64=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp5F)->f1;_tmp65=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp5F)->f2;_tmp66=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp5F)->f3;_LL6: {void*t1=_tmp64;enum Cyc_Vcgen_Primreln p=_tmp65;void*t2=_tmp66;
# 274
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3U: _LL7: _tmp62=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp5F)->f2;_LL8: {void*a1=_tmp62;void*a2=_tmp63;
return(int)((unsigned)a1 ^ (unsigned)a2);}default: _LL9: _tmp60=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp5F)->f1;_tmp61=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp5F)->f2;_LLA: {void*a1=_tmp60;void*a2=_tmp61;
return(int)((unsigned)a1 ^ (unsigned)a2 | (unsigned)1);}}_LL0:;}
# 280
int Cyc_Vcgen_assncmp(void*a1,void*a2){
if(a1 == a2)return 0;{
struct _tuple11 _tmp67=({struct _tuple11 _tmp2C3;_tmp2C3.f1=a1,_tmp2C3.f2=a2;_tmp2C3;});struct _tuple11 _stmttmp1=_tmp67;struct _tuple11 _tmp68=_stmttmp1;void*_tmp6E;enum Cyc_Vcgen_Primreln _tmp6D;void*_tmp6C;void*_tmp6B;enum Cyc_Vcgen_Primreln _tmp6A;void*_tmp69;void*_tmp72;void*_tmp71;void*_tmp70;void*_tmp6F;void*_tmp76;void*_tmp75;void*_tmp74;void*_tmp73;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp68.f1)->tag == 0U){if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp68.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp68.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp68.f1)->tag == 1U){if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp68.f2)->tag == 1U){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp68.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{switch(*((int*)_tmp68.f1)){case 4U: if(((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp68.f2)->tag == 4U){_LLD: _tmp73=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp68.f1)->f1;_tmp74=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp68.f1)->f2;_tmp75=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp68.f2)->f1;_tmp76=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp68.f2)->f2;_LLE: {void*a11=_tmp73;void*a12=_tmp74;void*a21=_tmp75;void*a22=_tmp76;
_tmp6F=a11;_tmp70=a12;_tmp71=a21;_tmp72=a22;goto _LL10;}}else{_LL11: _LL12:
# 294
 return - 1;}case 3U: switch(*((int*)_tmp68.f2)){case 3U: _LLF: _tmp6F=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp68.f1)->f1;_tmp70=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp68.f1)->f2;_tmp71=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp68.f2)->f1;_tmp72=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp68.f2)->f2;_LL10: {void*a11=_tmp6F;void*a12=_tmp70;void*a21=_tmp71;void*a22=_tmp72;
# 291
int c=Cyc_Vcgen_assncmp(a11,a21);
if(c != 0)return c;
return Cyc_Vcgen_assncmp(a12,a22);}case 4U: goto _LL13;default: _LL15: _LL16:
# 296
 return - 1;}default: switch(*((int*)_tmp68.f2)){case 4U: _LL13: _LL14:
# 295
 return 1;case 3U: _LL17: _LL18:
# 297
 return 1;default: _LL19: _tmp69=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp68.f1)->f1;_tmp6A=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp68.f1)->f2;_tmp6B=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp68.f1)->f3;_tmp6C=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp68.f2)->f1;_tmp6D=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp68.f2)->f2;_tmp6E=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp68.f2)->f3;_LL1A: {void*t11=_tmp69;enum Cyc_Vcgen_Primreln p1=_tmp6A;void*t12=_tmp6B;void*t21=_tmp6C;enum Cyc_Vcgen_Primreln p2=_tmp6D;void*t22=_tmp6E;
# 299
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_Vcgen_termcmp(t11,t21);
if(c != 0)return c;
return Cyc_Vcgen_termcmp(t12,t22);}}}}}}}_LL0:;}}
# 307
static struct Cyc_Hashtable_Table*Cyc_Vcgen_hash_cons_table=0;
# 309
void*Cyc_Vcgen_copy_assn(void*a){
void*_tmp77=a;void*_tmp7A;enum Cyc_Vcgen_Primreln _tmp79;void*_tmp78;void*_tmp7C;void*_tmp7B;void*_tmp7E;void*_tmp7D;switch(*((int*)_tmp77)){case 0U: _LL1: _LL2:
 return(void*)& Cyc_Vcgen_true_assn;case 1U: _LL3: _LL4:
 return(void*)& Cyc_Vcgen_false_assn;case 3U: _LL5: _tmp7D=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp77)->f1;_tmp7E=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp77)->f2;_LL6: {void*a1=_tmp7D;void*a2=_tmp7E;
return(void*)({struct Cyc_Vcgen_And_Vcgen_Assn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=3U,_tmp7F->f1=a1,_tmp7F->f2=a2;_tmp7F;});}case 4U: _LL7: _tmp7B=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp77)->f1;_tmp7C=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp77)->f2;_LL8: {void*a1=_tmp7B;void*a2=_tmp7C;
return(void*)({struct Cyc_Vcgen_Or_Vcgen_Assn_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=4U,_tmp80->f1=a1,_tmp80->f2=a2;_tmp80;});}default: _LL9: _tmp78=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp77)->f1;_tmp79=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp77)->f2;_tmp7A=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp77)->f3;_LLA: {void*t1=_tmp78;enum Cyc_Vcgen_Primreln p=_tmp79;void*t2=_tmp7A;
return(void*)({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=2U,_tmp81->f1=t1,_tmp81->f2=p,_tmp81->f3=t2;_tmp81;});}}_LL0:;}
# 319
void*Cyc_Vcgen_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_Vcgen_hash_cons_table == 0){
h=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(123,Cyc_Vcgen_assncmp,Cyc_Vcgen_assnhash);
Cyc_Vcgen_hash_cons_table=h;}else{
# 325
h=(struct Cyc_Hashtable_Table*)_check_null(Cyc_Vcgen_hash_cons_table);}{
void**_tmp82=((void**(*)(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_lookup_other_opt)(h,a,Cyc_Vcgen_assncmp,Cyc_Vcgen_assnhash);void**resopt=_tmp82;
if(resopt == 0){
void*res=Cyc_Vcgen_copy_assn(a);
((void(*)(struct Cyc_Hashtable_Table*t,void*key,void*val))Cyc_Hashtable_insert)(h,res,res);
return res;}else{
return*resopt;}}}
# 337
void*Cyc_Vcgen_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp83=({struct _tuple11 _tmp2C5;_tmp2C5.f1=a1,_tmp2C5.f2=a2;_tmp2C5;});struct _tuple11 _stmttmp2=_tmp83;struct _tuple11 _tmp84=_stmttmp2;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp84.f1)->tag == 0U){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp84.f2)->tag == 0U){_LL3: _LL4:
 return a1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp84.f1)->tag == 1U){_LL5: _LL6:
 return a1;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp84.f2)->tag == 1U){_LL7: _LL8:
 return a2;}else{_LL9: _LLA:
# 345
 if((int)a1 > (int)a2)
({void*_tmp85=a1;void*_tmp86=a2;a1=_tmp86;a2=_tmp85;});{
struct Cyc_Vcgen_And_Vcgen_Assn_struct _tmp87=({struct Cyc_Vcgen_And_Vcgen_Assn_struct _tmp2C4;_tmp2C4.tag=3U,_tmp2C4.f1=a1,_tmp2C4.f2=a2;_tmp2C4;});struct Cyc_Vcgen_And_Vcgen_Assn_struct a=_tmp87;
return Cyc_Vcgen_hash_cons((void*)& a);}}}}}_LL0:;}}
# 352
void*Cyc_Vcgen_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp88=({struct _tuple11 _tmp2C7;_tmp2C7.f1=a1,_tmp2C7.f2=a2;_tmp2C7;});struct _tuple11 _stmttmp3=_tmp88;struct _tuple11 _tmp89=_stmttmp3;if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp89.f1)->tag == 0U){_LL1: _LL2:
 return a1;}else{if(((struct Cyc_Vcgen_True_Vcgen_Assn_struct*)_tmp89.f2)->tag == 0U){_LL3: _LL4:
 return a2;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp89.f1)->tag == 1U){_LL5: _LL6:
 return a2;}else{if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp89.f2)->tag == 1U){_LL7: _LL8:
 return a1;}else{_LL9: _LLA:
# 360
 if((int)a1 > (int)a2)
({void*_tmp8A=a1;void*_tmp8B=a2;a1=_tmp8B;a2=_tmp8A;});{
struct Cyc_Vcgen_Or_Vcgen_Assn_struct _tmp8C=({struct Cyc_Vcgen_Or_Vcgen_Assn_struct _tmp2C6;_tmp2C6.tag=4U,_tmp2C6.f1=a1,_tmp2C6.f2=a2;_tmp2C6;});struct Cyc_Vcgen_Or_Vcgen_Assn_struct a=_tmp8C;
return Cyc_Vcgen_hash_cons((void*)& a);}}}}}_LL0:;}}
# 367
void*Cyc_Vcgen_prim(void*t1,enum Cyc_Vcgen_Primreln p,void*t2){
struct Cyc_Vcgen_Prim_Vcgen_Assn_struct _tmp8D=({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct _tmp2C8;_tmp2C8.tag=2U,_tmp2C8.f1=t1,_tmp2C8.f2=p,_tmp2C8.f3=t2;_tmp2C8;});struct Cyc_Vcgen_Prim_Vcgen_Assn_struct ptr=_tmp8D;
return Cyc_Vcgen_hash_cons((void*)& ptr);}
# 376
void*Cyc_Vcgen_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Eq,t2);}
void*Cyc_Vcgen_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Neq,t2);}
void*Cyc_Vcgen_lt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Lt,t2);}
void*Cyc_Vcgen_lte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_Vcgen_true_assn;return Cyc_Vcgen_prim(t1,Cyc_Vcgen_Lte,t2);}
# 383
void*Cyc_Vcgen_not(void*a){
void*_tmp8E=a;void*_tmp90;void*_tmp8F;void*_tmp92;void*_tmp91;void*_tmp95;enum Cyc_Vcgen_Primreln _tmp94;void*_tmp93;switch(*((int*)_tmp8E)){case 0U: _LL1: _LL2:
 return(void*)& Cyc_Vcgen_false_assn;case 1U: _LL3: _LL4:
 return(void*)& Cyc_Vcgen_true_assn;case 2U: _LL5: _tmp93=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp8E)->f1;_tmp94=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp8E)->f2;_tmp95=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp8E)->f3;_LL6: {void*t1=_tmp93;enum Cyc_Vcgen_Primreln pr=_tmp94;void*t2=_tmp95;
# 388
enum Cyc_Vcgen_Primreln _tmp96=pr;switch(_tmp96){case Cyc_Vcgen_Eq: _LLC: _LLD:
 return Cyc_Vcgen_neq(t1,t2);case Cyc_Vcgen_Neq: _LLE: _LLF:
 return Cyc_Vcgen_eq(t1,t2);case Cyc_Vcgen_Lt: _LL10: _LL11:
 return Cyc_Vcgen_lte(t2,t1);case Cyc_Vcgen_Lte: _LL12: _LL13:
 return Cyc_Vcgen_lt(t2,t1);default: _LL14: _LL15:
({void*_tmp97=0U;({struct _fat_ptr _tmp313=({const char*_tmp98="vcgen not";_tag_fat(_tmp98,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp313,_tag_fat(_tmp97,sizeof(void*),0U));});});}_LLB:;}case 3U: _LL7: _tmp91=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp8E)->f1;_tmp92=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp8E)->f2;_LL8: {void*a1=_tmp91;void*a2=_tmp92;
# 395
return({void*_tmp314=Cyc_Vcgen_not(a1);Cyc_Vcgen_or(_tmp314,Cyc_Vcgen_not(a2));});}default: _LL9: _tmp8F=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp8E)->f1;_tmp90=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp8E)->f2;_LLA: {void*a1=_tmp8F;void*a2=_tmp90;
return({void*_tmp315=Cyc_Vcgen_not(a1);Cyc_Vcgen_and(_tmp315,Cyc_Vcgen_not(a2));});}}_LL0:;}struct _tuple12{enum Cyc_Vcgen_Primreln f1;enum Cyc_Vcgen_Primreln f2;};
# 404
int Cyc_Vcgen_find_ctxt(void*ctxt,void*t1,enum Cyc_Vcgen_Primreln p1,void*u1){
void*_tmp99=ctxt;void*_tmp9C;enum Cyc_Vcgen_Primreln _tmp9B;void*_tmp9A;void*_tmp9E;void*_tmp9D;void*_tmpA0;void*_tmp9F;switch(*((int*)_tmp99)){case 1U: _LL1: _LL2:
 return 1;case 0U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmp9F=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp99)->f1;_tmpA0=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmp99)->f2;_LL6: {void*a1=_tmp9F;void*a2=_tmpA0;
return Cyc_Vcgen_find_ctxt(a1,t1,p1,u1)|| Cyc_Vcgen_find_ctxt(a2,t1,p1,u1);}case 4U: _LL7: _tmp9D=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp99)->f1;_tmp9E=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmp99)->f2;_LL8: {void*a1=_tmp9D;void*a2=_tmp9E;
return Cyc_Vcgen_find_ctxt(a1,t1,p1,u1)&& Cyc_Vcgen_find_ctxt(a2,t1,p1,u1);}default: _LL9: _tmp9A=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp99)->f1;_tmp9B=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp99)->f2;_tmp9C=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmp99)->f3;_LLA: {void*t2=_tmp9A;enum Cyc_Vcgen_Primreln p2=_tmp9B;void*u2=_tmp9C;
# 411
struct _tuple12 _tmpA1=({struct _tuple12 _tmp2C9;_tmp2C9.f1=p1,_tmp2C9.f2=p2;_tmp2C9;});struct _tuple12 _stmttmp4=_tmpA1;struct _tuple12 _tmpA2=_stmttmp4;switch(_tmpA2.f1){case Cyc_Vcgen_Eq: switch(_tmpA2.f2){case Cyc_Vcgen_Eq: _LLC: _LLD:
 goto _LLF;case Cyc_Vcgen_Lte: _LLE: _LLF:
# 414
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2)||
 Cyc_Vcgen_same_term(t1,u2)&& Cyc_Vcgen_same_term(u1,t2);default: goto _LL1A;}case Cyc_Vcgen_Lt: switch(_tmpA2.f2){case Cyc_Vcgen_Neq: _LL10: _LL11:
# 417
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2)||
 Cyc_Vcgen_same_term(t1,u2)&& Cyc_Vcgen_same_term(u1,t2);case Cyc_Vcgen_Lt: _LL12: _LL13:
 goto _LL15;case Cyc_Vcgen_Lte: _LL14: _LL15:
 goto _LL17;default: goto _LL1A;}case Cyc_Vcgen_Lte: if(_tmpA2.f2 == Cyc_Vcgen_Lte){_LL16: _LL17:
 goto _LL19;}else{goto _LL1A;}case Cyc_Vcgen_Neq: if(_tmpA2.f2 == Cyc_Vcgen_Neq){_LL18: _LL19:
# 423
 return Cyc_Vcgen_same_term(t1,t2)&& Cyc_Vcgen_same_term(u1,u2);}else{goto _LL1A;}default: _LL1A: _LL1B:
 return 0;}_LLB:;}}_LL0:;}
# 431
int Cyc_Vcgen_simple_prove(void*ctxt,void*a){
if(ctxt == a)return 1;{
void*_tmpA3=a;void*_tmpA6;enum Cyc_Vcgen_Primreln _tmpA5;void*_tmpA4;void*_tmpA8;void*_tmpA7;void*_tmpAA;void*_tmpA9;switch(*((int*)_tmpA3)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 return 0;case 3U: _LL5: _tmpA9=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpA3)->f1;_tmpAA=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpA3)->f2;_LL6: {void*a1=_tmpA9;void*a2=_tmpAA;
return Cyc_Vcgen_simple_prove(ctxt,a1)&& Cyc_Vcgen_simple_prove(ctxt,a2);}case 4U: _LL7: _tmpA7=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpA3)->f1;_tmpA8=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpA3)->f2;_LL8: {void*a1=_tmpA7;void*a2=_tmpA8;
return Cyc_Vcgen_simple_prove(ctxt,a1)|| Cyc_Vcgen_simple_prove(ctxt,a2);}default: _LL9: _tmpA4=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA3)->f1;_tmpA5=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA3)->f2;_tmpA6=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpA3)->f3;_LLA: {void*t1=_tmpA4;enum Cyc_Vcgen_Primreln p=_tmpA5;void*t2=_tmpA6;
return Cyc_Vcgen_find_ctxt(ctxt,t1,p,t2);}}_LL0:;}}
# 442
int Cyc_Vcgen_constraint_prove(void*ctxt,void*a){
# 444
return 0;}
# 449
int Cyc_Vcgen_implies(void*a1,void*a2){
return Cyc_Vcgen_simple_prove(a1,a2)|| Cyc_Vcgen_constraint_prove(a1,a2);}
# 458
void*Cyc_Vcgen_weaken(void*ctxt,void*a){
if(Cyc_Vcgen_implies(ctxt,a))return a;{
void*_tmpAB=a;void*_tmpAD;void*_tmpAC;void*_tmpAF;void*_tmpAE;switch(*((int*)_tmpAB)){case 3U: _LL1: _tmpAE=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpAB)->f1;_tmpAF=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpAB)->f2;_LL2: {void*a1=_tmpAE;void*a2=_tmpAF;
# 462
void*_tmpB0=Cyc_Vcgen_weaken(ctxt,a1);void*b1=_tmpB0;
void*_tmpB1=Cyc_Vcgen_weaken(ctxt,a2);void*b2=_tmpB1;
if(Cyc_Vcgen_implies(b1,b2))return b1;
if(Cyc_Vcgen_implies(b2,b1))return b2;
return Cyc_Vcgen_and(b1,b2);}case 4U: _LL3: _tmpAC=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpAB)->f1;_tmpAD=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpAB)->f2;_LL4: {void*a1=_tmpAC;void*a2=_tmpAD;
# 468
void*_tmpB2=Cyc_Vcgen_weaken(a2,a1);void*b1=_tmpB2;
void*_tmpB3=Cyc_Vcgen_weaken(a1,a2);void*b2=_tmpB3;
void*_tmpB4=({void*_tmp316=ctxt;Cyc_Vcgen_weaken(_tmp316,Cyc_Vcgen_and(b1,b2));});void*c=_tmpB4;
return c;}case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 return(void*)& Cyc_Vcgen_true_assn;}_LL0:;}}
# 484
void*Cyc_Vcgen_widen_assn(void*a1,void*a2){
return Cyc_Vcgen_weaken(a1,a2);}
# 491
void*Cyc_Vcgen_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmpB5=t;struct Cyc_List_List*_tmpB7;enum Cyc_Absyn_Primop _tmpB6;struct Cyc_Absyn_Vardecl*_tmpB8;switch(*((int*)_tmpB5)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmpB8=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmpB5)->f1;if(_tmpB8 == x){_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpB8;
return newx;}}else{_LL5: _LL6:
 return t;}case 2U: _LL7: _LL8:
 return t;default: _LL9: _tmpB6=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpB5)->f1;_tmpB7=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpB5)->f2;_LLA: {enum Cyc_Absyn_Primop p=_tmpB6;struct Cyc_List_List*ts=_tmpB7;
# 498
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpB9=ts;struct Cyc_List_List*ptr=_tmpB9;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpBA=(void*)ptr->hd;void*old_term=_tmpBA;
void*_tmpBB=Cyc_Vcgen_subst_term(old_term,x,newx);void*new_term=_tmpBB;
res=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=new_term,_tmpBC->tl=res;_tmpBC;});
if(new_term != old_term)change=1;}}
# 506
if(!change)return t;
return(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=3U,_tmpBD->f1=p,({struct Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);_tmpBD->f2=_tmp317;});_tmpBD;});}}_LL0:;}
# 512
void*Cyc_Vcgen_subst_assn(void*a,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmpBE=a;void*_tmpC0;void*_tmpBF;void*_tmpC2;void*_tmpC1;void*_tmpC5;enum Cyc_Vcgen_Primreln _tmpC4;void*_tmpC3;switch(*((int*)_tmpBE)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;case 2U: _LL5: _tmpC3=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpBE)->f1;_tmpC4=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpBE)->f2;_tmpC5=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpBE)->f3;_LL6: {void*t1=_tmpC3;enum Cyc_Vcgen_Primreln p=_tmpC4;void*t2=_tmpC5;
# 517
void*_tmpC6=Cyc_Vcgen_subst_term(t1,x,newx);void*newt1=_tmpC6;
void*_tmpC7=Cyc_Vcgen_subst_term(t2,x,newx);void*newt2=_tmpC7;
if(newt1 == t1 && newt2 == t2)return a;
return Cyc_Vcgen_hash_cons((void*)({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=2U,_tmpC8->f1=newt1,_tmpC8->f2=p,_tmpC8->f3=newt2;_tmpC8;}));}case 3U: _LL7: _tmpC1=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpBE)->f1;_tmpC2=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpBE)->f2;_LL8: {void*a1=_tmpC1;void*a2=_tmpC2;
# 522
void*_tmpC9=Cyc_Vcgen_subst_assn(a1,x,newx);void*newa1=_tmpC9;
void*_tmpCA=Cyc_Vcgen_subst_assn(a2,x,newx);void*newa2=_tmpCA;
if(newa1 == a1 && newa2 == a2)return a;
return Cyc_Vcgen_and(newa1,newa2);}default: _LL9: _tmpBF=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpBE)->f1;_tmpC0=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpBE)->f2;_LLA: {void*a1=_tmpBF;void*a2=_tmpC0;
# 527
void*_tmpCB=Cyc_Vcgen_subst_assn(a1,x,newx);void*newa1=_tmpCB;
void*_tmpCC=Cyc_Vcgen_subst_assn(a2,x,newx);void*newa2=_tmpCC;
if(newa1 == a1 && newa2 == a2)return a;
return Cyc_Vcgen_or(newa1,newa2);}}_LL0:;}struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 536
void*Cyc_Vcgen_term_kill_vars(void*t,struct Cyc_List_List**newvds){
void*_tmpCD=t;struct Cyc_List_List*_tmpCF;enum Cyc_Absyn_Primop _tmpCE;struct Cyc_Absyn_Vardecl*_tmpD0;switch(*((int*)_tmpCD)){case 1U: _LL1: _tmpD0=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmpCD)->f1;if(_tmpD0->escapes){_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmpD0;
# 539
struct _handler_cons _tmpD1;_push_handler(& _tmpD1);{int _tmpD3=0;if(setjmp(_tmpD1.handler))_tmpD3=1;if(!_tmpD3){
{void*_tmpD4=((void*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(*newvds,vd);_npop_handler(0U);return _tmpD4;};_pop_handler();}else{void*_tmpD2=(void*)Cyc_Core_get_exn_thrown();void*_tmpD5=_tmpD2;void*_tmpD6;if(((struct Cyc_Core_Not_found_exn_struct*)_tmpD5)->tag == Cyc_Core_Not_found){_LL8: _LL9: {
# 543
void*_tmpD7=Cyc_Vcgen_fresh_var();void*newv=_tmpD7;
({struct Cyc_List_List*_tmp319=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct _tuple13*_tmp318=({struct _tuple13*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->f1=vd,_tmpD8->f2=newv;_tmpD8;});_tmpD9->hd=_tmp318;}),_tmpD9->tl=*newvds;_tmpD9;});*newvds=_tmp319;});
return newv;}}else{_LLA: _tmpD6=_tmpD5;_LLB: {void*exn=_tmpD6;(int)_rethrow(exn);}}_LL7:;}}}}else{goto _LL5;}case 3U: _LL3: _tmpCE=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpCD)->f1;_tmpCF=((struct Cyc_Vcgen_Primop_Vcgen_Term_struct*)_tmpCD)->f2;_LL4: {enum Cyc_Absyn_Primop p=_tmpCE;struct Cyc_List_List*ts=_tmpCF;
# 548
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpDA=ts;struct Cyc_List_List*ptr=_tmpDA;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpDB=(void*)ptr->hd;void*old_term=_tmpDB;
void*_tmpDC=Cyc_Vcgen_term_kill_vars(old_term,newvds);void*new_term=_tmpDC;
res=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=new_term,_tmpDD->tl=res;_tmpDD;});
if(new_term != old_term)change=1;}}
# 556
if(!change)return t;
return(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=3U,_tmpDE->f1=p,({struct Cyc_List_List*_tmp31A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);_tmpDE->f2=_tmp31A;});_tmpDE;});}default: _LL5: _LL6:
 return t;}_LL0:;}
# 563
void*Cyc_Vcgen_assn_kill_vars(void*a,struct Cyc_List_List**accum){
void*_tmpDF=a;void*_tmpE1;void*_tmpE0;void*_tmpE3;void*_tmpE2;void*_tmpE6;enum Cyc_Vcgen_Primreln _tmpE5;void*_tmpE4;switch(*((int*)_tmpDF)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return a;case 2U: _LL5: _tmpE4=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpDF)->f1;_tmpE5=((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpDF)->f2;_tmpE6=(void*)((struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*)_tmpDF)->f3;_LL6: {void*t1=_tmpE4;enum Cyc_Vcgen_Primreln p=_tmpE5;void*t2=_tmpE6;
# 568
void*_tmpE7=Cyc_Vcgen_term_kill_vars(t1,accum);void*newt1=_tmpE7;
void*_tmpE8=Cyc_Vcgen_term_kill_vars(t2,accum);void*newt2=_tmpE8;
if(newt1 == t1 && newt2 == t2)return a;
return Cyc_Vcgen_hash_cons((void*)({struct Cyc_Vcgen_Prim_Vcgen_Assn_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->tag=2U,_tmpE9->f1=newt1,_tmpE9->f2=p,_tmpE9->f3=newt2;_tmpE9;}));}case 3U: _LL7: _tmpE2=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpDF)->f1;_tmpE3=(void*)((struct Cyc_Vcgen_And_Vcgen_Assn_struct*)_tmpDF)->f2;_LL8: {void*a1=_tmpE2;void*a2=_tmpE3;
# 573
void*_tmpEA=Cyc_Vcgen_assn_kill_vars(a1,accum);void*newa1=_tmpEA;
void*_tmpEB=Cyc_Vcgen_assn_kill_vars(a2,accum);void*newa2=_tmpEB;
if(newa1 == a1 && newa2 == a2)return a;
return Cyc_Vcgen_and(newa1,newa2);}default: _LL9: _tmpE0=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpDF)->f1;_tmpE1=(void*)((struct Cyc_Vcgen_Or_Vcgen_Assn_struct*)_tmpDF)->f2;_LLA: {void*a1=_tmpE0;void*a2=_tmpE1;
# 578
void*_tmpEC=Cyc_Vcgen_assn_kill_vars(a1,accum);void*newa1=_tmpEC;
void*_tmpED=Cyc_Vcgen_assn_kill_vars(a2,accum);void*newa2=_tmpED;
if(newa1 == a1 && newa2 == a2)return a;
return Cyc_Vcgen_or(newa1,newa2);}}_LL0:;}
# 587
void*Cyc_Vcgen_kill_mem(void*a){
struct Cyc_List_List**accum=({struct Cyc_List_List**_tmpEE=_cycalloc(sizeof(*_tmpEE));*_tmpEE=0;_tmpEE;});
return Cyc_Vcgen_assn_kill_vars(a,accum);}
# 592
void*Cyc_Vcgen_kill_mem_term(void*t){
struct Cyc_List_List*accum=0;
return Cyc_Vcgen_term_kill_vars(t,& accum);}
# 597
void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd);
# 600
void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmpEF=tgt;struct Cyc_Absyn_Vardecl*_tmpF0;if(_tmpEF == 0){_LL1: _LL2:
# 605
 return Cyc_Vcgen_kill_mem(a);}else{if(((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmpEF)->tag == 1U){_LL3: _tmpF0=((struct Cyc_Vcgen_Var_Vcgen_Term_struct*)_tmpEF)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpF0;
# 610
if(vd->escapes){
a=Cyc_Vcgen_kill_mem(a);
src=src == 0?0: Cyc_Vcgen_kill_mem_term(src);}else{
# 614
void*_tmpF1=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmpF1;
a=Cyc_Vcgen_subst_assn(a,vd,v2);
src=src == 0?0: Cyc_Vcgen_subst_term(src,vd,v2);}
# 619
return({void*_tmp31B=a;Cyc_Vcgen_and(_tmp31B,Cyc_Vcgen_eq(tgt,src));});}}else{_LL5: _LL6:
({void*_tmpF2=0U;({struct _fat_ptr _tmp31C=({const char*_tmpF3="vcgen do_assign";_tag_fat(_tmpF3,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp31C,_tag_fat(_tmpF2,sizeof(void*),0U));});});}}_LL0:;}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void*try_assn;void**exp_stmt;void*res_term;void*res_assn;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;};
# 659 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 661
struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Hashtable_Table*_tmpF4=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmpF4;
struct Cyc_Hashtable_Table*_tmpF5=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(57,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_Vcgen_hash_ptr);struct Cyc_Hashtable_Table*assn_table=_tmpF5;
struct Cyc_Vcgen_SharedEnv*_tmpF6=({struct Cyc_Vcgen_SharedEnv*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->assn_table=assn_table,_tmpF9->succ_table=succ_table,_tmpF9->try_assn=(void*)& Cyc_Vcgen_false_assn,_tmpF9->exp_stmt=0,_tmpF9->res_term=0,_tmpF9->res_assn=(void*)& Cyc_Vcgen_false_assn;_tmpF9;});struct Cyc_Vcgen_SharedEnv*senv=_tmpF6;
# 670
struct Cyc_Dict_Dict _tmpF7=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty)((int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp);struct Cyc_Dict_Dict d=_tmpF7;
return({struct Cyc_Vcgen_Env*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->shared=senv,_tmpF8->state_counter=d;_tmpF8;});}
# 674
struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmpFA=_cycalloc(sizeof(*_tmpFA));*_tmpFA=*env;_tmpFA;});}
# 678
int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 682
void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp31D=((struct Cyc_Dict_Dict(*)(int(*f)(int,struct Cyc_Absyn_Vardecl*,int,int),int env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp31D;});}
# 688
void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int*ans))Cyc_Dict_lookup_bool)(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp31E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Vardecl*k,int v))Cyc_Dict_insert)(env->state_counter,vd,res + 1);env->state_counter=_tmp31E;});
return(void*)({struct Cyc_Vcgen_LogicVar_Vcgen_Term_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=2U,_tmpFB->f1=vd,_tmpFB->f2=res;_tmpFB;});}
# 697
void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
struct _handler_cons _tmpFC;_push_handler(& _tmpFC);{int _tmpFE=0;if(setjmp(_tmpFC.handler))_tmpFE=1;if(!_tmpFE){
{void**_tmpFF=((void**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->assn_table,s);void**p=_tmpFF;
void*_tmp100=Cyc_Vcgen_or(ain,*p);void*new_precondition=_tmp100;
*p=new_precondition;{
void*_tmp101=new_precondition;_npop_handler(0U);return _tmp101;}}
# 699
;_pop_handler();}else{void*_tmpFD=(void*)Cyc_Core_get_exn_thrown();void*_tmp102=_tmpFD;void*_tmp103;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp102)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 704
 return ain;}else{_LL3: _tmp103=_tmp102;_LL4: {void*exn=_tmp103;(int)_rethrow(exn);}}_LL0:;}}}
# 708
void*Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
struct _handler_cons _tmp104;_push_handler(& _tmp104);{int _tmp106=0;if(setjmp(_tmp104.handler))_tmp106=1;if(!_tmp106){
{void*_tmp107=*((void**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->assn_table,s);_npop_handler(0U);return _tmp107;};_pop_handler();}else{void*_tmp105=(void*)Cyc_Core_get_exn_thrown();void*_tmp108=_tmp105;void*_tmp109;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp108)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 712
 return(void*)& Cyc_Vcgen_false_assn;}else{_LL3: _tmp109=_tmp108;_LL4: {void*exn=_tmp109;(int)_rethrow(exn);}}_LL0:;}}}
# 718
void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 720
{struct _handler_cons _tmp10A;_push_handler(& _tmp10A);{int _tmp10C=0;if(setjmp(_tmp10A.handler))_tmp10C=1;if(!_tmp10C){
{void**_tmp10D=((void**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->assn_table,s);void**aprev=_tmp10D;
ain=Cyc_Vcgen_widen_assn(ain,*aprev);
*aprev=ain;}
# 721
;_pop_handler();}else{void*_tmp10B=(void*)Cyc_Core_get_exn_thrown();void*_tmp10E=_tmp10B;void*_tmp10F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp10E)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 726
({struct Cyc_Hashtable_Table*_tmp320=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp31F=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp320,_tmp31F,({void**_tmp110=_cycalloc(sizeof(*_tmp110));*_tmp110=ain;_tmp110;}));});
goto _LL0;}else{_LL3: _tmp10F=_tmp10E;_LL4: {void*exn=_tmp10F;(int)_rethrow(exn);}}_LL0:;}}}
# 729
return ain;}
# 734
void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
{struct _handler_cons _tmp111;_push_handler(& _tmp111);{int _tmp113=0;if(setjmp(_tmp111.handler))_tmp113=1;if(!_tmp113){
{void**_tmp114=((void**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->assn_table,s);void**aprev=_tmp114;
ain=Cyc_Vcgen_or(ain,*aprev);
*aprev=ain;}
# 736
;_pop_handler();}else{void*_tmp112=(void*)Cyc_Core_get_exn_thrown();void*_tmp115=_tmp112;void*_tmp116;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp115)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 741
({struct Cyc_Hashtable_Table*_tmp322=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp321=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,void**val))Cyc_Hashtable_insert)(_tmp322,_tmp321,({void**_tmp117=_cycalloc(sizeof(*_tmp117));*_tmp117=ain;_tmp117;}));});
goto _LL0;}else{_LL3: _tmp116=_tmp115;_LL4: {void*exn=_tmp116;(int)_rethrow(exn);}}_LL0:;}}}
# 744
return ain;}
# 749
void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
({void*_tmp323=Cyc_Vcgen_or((env->shared)->try_assn,a);(env->shared)->try_assn=_tmp323;});}
# 754
void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a){
struct Cyc_Vcgen_SharedEnv*_tmp118=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp118;
if(shared->res_term != 0 || t != 0){
void*_tmp119=Cyc_Vcgen_fresh_var();void*v=_tmp119;
({void*_tmp327=({
void*_tmp326=({void*_tmp324=a;Cyc_Vcgen_and(_tmp324,Cyc_Vcgen_eq(v,t));});Cyc_Vcgen_or(_tmp326,({void*_tmp325=shared->res_assn;Cyc_Vcgen_and(_tmp325,Cyc_Vcgen_eq(v,shared->res_term));}));});
# 758
shared->res_assn=_tmp327;});}else{
# 761
({void*_tmp328=Cyc_Vcgen_or(a,shared->res_assn);shared->res_assn=_tmp328;});}}
# 770
void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain);
# 774
struct _tuple11 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 776
struct _tuple11 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 778
struct _tuple11 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);
# 781
struct _tuple11 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain);struct _tuple14{struct Cyc_List_List**f1;void*f2;};
# 785
struct _tuple14 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);
# 788
void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a);struct _tuple15{struct Cyc_List_List*f1;void*f2;};
# 790
struct _tuple15 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain);
# 792
void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain);
# 794
void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a);struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 865 "vcgen.cyc"
void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 868
ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);
Cyc_Vcgen_assn_size(ain);
# 872
{void*_tmp11A=ain;if(((struct Cyc_Vcgen_False_Vcgen_Assn_struct*)_tmp11A)->tag == 1U){_LL1: _LL2:
 return ain;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 876
void*_tmp11B=s->r;void*_stmttmp5=_tmp11B;void*_tmp11C=_stmttmp5;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Stmt*_tmp11D;struct Cyc_Absyn_Stmt*_tmp11F;struct Cyc_Absyn_Stmt*_tmp121;struct Cyc_Absyn_Decl*_tmp120;struct Cyc_Absyn_Switch_clause*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Stmt*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Stmt*_tmp126;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Absyn_Stmt*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Stmt*_tmp131;struct Cyc_Absyn_Stmt*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Absyn_Stmt*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Stmt*_tmp137;struct Cyc_Absyn_Stmt*_tmp136;struct Cyc_Absyn_Exp*_tmp138;switch(*((int*)_tmp11C)){case 0U: _LL6: _LL7:
 return ain;case 1U: _LL8: _tmp138=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp138;
# 879
struct _tuple11 _tmp139=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp6=_tmp139;struct _tuple11 _tmp13A=_stmttmp6;void*_tmp13C;void*_tmp13B;_LL2B: _tmp13B=_tmp13A.f1;_tmp13C=_tmp13A.f2;_LL2C: {void*t=_tmp13B;void*aout=_tmp13C;
# 882
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2U: _LLA: _tmp136=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp137=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LLB: {struct Cyc_Absyn_Stmt*s1=_tmp136;struct Cyc_Absyn_Stmt*s2=_tmp137;
return({struct Cyc_Vcgen_Env*_tmp32A=env;struct Cyc_Absyn_Stmt*_tmp329=s2;Cyc_Vcgen_vcgen_stmt_nodebug(_tmp32A,_tmp329,Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain));});}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1 == 0){_LLC: _LLD:
# 887
 Cyc_Vcgen_update_return(env,0,ain);
return(void*)& Cyc_Vcgen_false_assn;}else{_LLE: _tmp135=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp135;
# 890
struct _tuple11 _tmp13D=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple11 _stmttmp7=_tmp13D;struct _tuple11 _tmp13E=_stmttmp7;void*_tmp140;void*_tmp13F;_LL2E: _tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;_LL2F: {void*t=_tmp13F;void*aout=_tmp140;
Cyc_Vcgen_update_return(env,t,aout);
return(void*)& Cyc_Vcgen_false_assn;}}}case 4U: _LL10: _tmp132=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp133=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_tmp134=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp11C)->f3;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp132;struct Cyc_Absyn_Stmt*s1=_tmp133;struct Cyc_Absyn_Stmt*s2=_tmp134;
# 894
struct _tuple11 _tmp141=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp8=_tmp141;struct _tuple11 _tmp142=_stmttmp8;void*_tmp144;void*_tmp143;_LL31: _tmp143=_tmp142.f1;_tmp144=_tmp142.f2;_LL32: {void*at=_tmp143;void*af=_tmp144;
struct Cyc_Vcgen_Env*_tmp145=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp145;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return Cyc_Vcgen_or(at,af);}}case 5U: _LL12: _tmp12F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1).f1;_tmp130=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1).f2;_tmp131=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LL13: {struct Cyc_Absyn_Exp*e=_tmp12F;struct Cyc_Absyn_Stmt*dummy=_tmp130;struct Cyc_Absyn_Stmt*s=_tmp131;
# 906
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);{
struct _tuple11 _tmp146=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple11 _stmttmp9=_tmp146;struct _tuple11 _tmp147=_stmttmp9;void*_tmp149;void*_tmp148;_LL34: _tmp148=_tmp147.f1;_tmp149=_tmp147.f2;_LL35: {void*at=_tmp148;void*af=_tmp149;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(({void*_tmp32B=ain;Cyc_Vcgen_implies(_tmp32B,Cyc_Vcgen_get_assn(env,dummy));}))return af;}}}}case 7U: _LL14: _LL15:
# 912
 goto _LL17;{
# 915
struct Cyc_Absyn_Stmt*_tmp14A=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp14A;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_Vcgen_false_assn;}case 6U: _LL16: _LL17: {
# 922
struct Cyc_Absyn_Stmt*_tmp14B=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp14B;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain);else{
# 926
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_Vcgen_false_assn;}case 9U: _LL18: _tmp129=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp12A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2).f1;_tmp12B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2).f2;_tmp12C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11C)->f3).f1;_tmp12D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11C)->f3).f2;_tmp12E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11C)->f4;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp129;struct Cyc_Absyn_Exp*e2=_tmp12A;struct Cyc_Absyn_Stmt*dummy2=_tmp12B;struct Cyc_Absyn_Exp*e3=_tmp12C;struct Cyc_Absyn_Stmt*dummy3=_tmp12D;struct Cyc_Absyn_Stmt*s=_tmp12E;
# 929
struct _tuple11 _tmp14C=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,ain);struct _tuple11 _stmttmpA=_tmp14C;struct _tuple11 _tmp14D=_stmttmpA;void*_tmp14E;_LL37: _tmp14E=_tmp14D.f2;_LL38: {void*a=_tmp14E;
ain=a;
# 932
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);{
struct _tuple11 _tmp14F=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple11 _stmttmpB=_tmp14F;struct _tuple11 _tmp150=_stmttmpB;void*_tmp152;void*_tmp151;_LL3A: _tmp151=_tmp150.f1;_tmp152=_tmp150.f2;_LL3B: {void*at=_tmp151;void*af=_tmp152;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 937
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(({void*_tmp32C=ain;Cyc_Vcgen_implies(_tmp32C,Cyc_Vcgen_get_assn(env,dummy2));}))return af;}}}}}case 14U: _LL1A: _tmp126=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp127=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2).f1;_tmp128=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2).f2;_LL1B: {struct Cyc_Absyn_Stmt*s=_tmp126;struct Cyc_Absyn_Exp*e=_tmp127;struct Cyc_Absyn_Stmt*dummy=_tmp128;
# 943
while(1){
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple11 _tmp153=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple11 _stmttmpC=_tmp153;struct _tuple11 _tmp154=_stmttmpC;void*_tmp156;void*_tmp155;_LL3D: _tmp155=_tmp154.f1;_tmp156=_tmp154.f2;_LL3E: {void*at=_tmp155;void*af=_tmp156;
ain=({void*_tmp32D=Cyc_Vcgen_get_assn(env,dummy);Cyc_Vcgen_widen_assn(_tmp32D,ain);});
if(({void*_tmp32E=ain;Cyc_Vcgen_implies(_tmp32E,Cyc_Vcgen_get_assn(env,dummy));}))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10U: _LL1C: _tmp124=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp125=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LL1D: {struct Cyc_Absyn_Exp*e=_tmp124;struct Cyc_List_List*scs=_tmp125;
# 956
struct _tuple11 _tmp157=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmpD=_tmp157;struct _tuple11 _tmp158=_stmttmpD;void*_tmp15A;void*_tmp159;_LL40: _tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;_LL41: {void*t=_tmp159;void*a=_tmp15A;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2 == 0){_LL1E: _LL1F:
({void*_tmp15B=0U;({struct _fat_ptr _tmp32F=({const char*_tmp15C="vcgen fallthru";_tag_fat(_tmp15C,sizeof(char),15U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp32F,_tag_fat(_tmp15B,sizeof(void*),0U));});});}else{_LL20: _tmp122=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp123=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LL21: {struct Cyc_List_List*es=_tmp122;struct Cyc_Absyn_Switch_clause*dest=_tmp123;
# 961
struct _tuple15 _tmp15D=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple15 _stmttmpE=_tmp15D;struct _tuple15 _tmp15E=_stmttmpE;void*_tmp160;struct Cyc_List_List*_tmp15F;_LL43: _tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;_LL44: {struct Cyc_List_List*ts=_tmp15F;void*a=_tmp160;
# 963
{struct Cyc_List_List*_tmp161=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v;struct Cyc_List_List*x=_tmp161;for(0;x != 0;(x=x->tl,ts=((struct Cyc_List_List*)_check_null(ts))->tl)){
struct _tuple16*_tmp162=(struct _tuple16*)x->hd;struct _tuple16*_stmttmpF=_tmp162;struct _tuple16*_tmp163=_stmttmpF;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Vardecl**_tmp164;_LL46: _tmp164=_tmp163->f1;_tmp165=_tmp163->f2;_LL47: {struct Cyc_Absyn_Vardecl**vd=_tmp164;struct Cyc_Absyn_Exp*ve=_tmp165;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp332=env;void*_tmp331=a;void*_tmp330=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->tag=1U,_tmp166->f1=*vd;_tmp166;});Cyc_Vcgen_do_assign(_tmp332,_tmp331,_tmp330,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}}
# 968
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_Vcgen_false_assn;}}}case 12U: _LL22: _tmp120=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp121=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LL23: {struct Cyc_Absyn_Decl*d=_tmp120;struct Cyc_Absyn_Stmt*s=_tmp121;
# 971
return({struct Cyc_Vcgen_Env*_tmp334=env;struct Cyc_Absyn_Stmt*_tmp333=s;Cyc_Vcgen_vcgen_stmt_nodebug(_tmp334,_tmp333,Cyc_Vcgen_vcgen_local_decl(env,d,ain));});}case 13U: _LL24: _tmp11F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LL25: {struct Cyc_Absyn_Stmt*s=_tmp11F;
# 977
return Cyc_Vcgen_vcgen_stmt_nodebug(env,s,(void*)& Cyc_Vcgen_true_assn);}case 8U: _LL26: _LL27:
 return(void*)& Cyc_Vcgen_false_assn;default: _LL28: _tmp11D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp11C)->f2;_LL29: {struct Cyc_Absyn_Stmt*s=_tmp11D;struct Cyc_List_List*scs=_tmp11E;
# 981
void*_tmp167=(env->shared)->try_assn;void*old_try_assn=_tmp167;
# 983
(env->shared)->try_assn=(void*)& Cyc_Vcgen_false_assn;
# 986
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 988
void*_tmp168=(env->shared)->try_assn;void*new_try_assn=_tmp168;
(env->shared)->try_assn=old_try_assn;
# 992
return({void*_tmp335=ain;Cyc_Vcgen_or(_tmp335,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}}_LL5:;}}
# 996
void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 1000
void*res=(void*)& Cyc_Vcgen_false_assn;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp169=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp169;
void*_tmp16A=ain;void*a=_tmp16A;
{struct Cyc_List_List*_tmp16B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmp16B;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple16*_tmp16C=(struct _tuple16*)pvs->hd;struct _tuple16*_stmttmp10=_tmp16C;struct _tuple16*_tmp16D=_stmttmp10;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Vardecl**_tmp16E;_LL1: _tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_LL2: {struct Cyc_Absyn_Vardecl**vdopt=_tmp16E;struct Cyc_Absyn_Exp*eopt=_tmp16F;
if(vdopt != 0){
if(eopt != 0)
a=({struct Cyc_Vcgen_Env*_tmp338=env;void*_tmp337=a;void*_tmp336=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=1U,_tmp170->f1=*vdopt;_tmp170;});Cyc_Vcgen_do_assign(_tmp338,_tmp337,_tmp336,(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a)).f1);});else{
# 1010
a=({struct Cyc_Vcgen_Env*_tmp33B=env;void*_tmp33A=a;void*_tmp339=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->tag=1U,_tmp171->f1=*vdopt;_tmp171;});Cyc_Vcgen_do_assign(_tmp33B,_tmp33A,_tmp339,Cyc_Vcgen_fresh_var());});}}}}}
# 1013
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp33C=res;Cyc_Vcgen_or(_tmp33C,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 1017
return res;}
# 1020
void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 1023
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp172=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->tag=1U,_tmp177->f1=vd;_tmp177;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*tv=_tmp172;
a=({struct Cyc_Vcgen_Env*_tmp33F=env;void*_tmp33E=a;void*_tmp33D=(void*)tv;Cyc_Vcgen_do_assign(_tmp33F,_tmp33E,_tmp33D,Cyc_Vcgen_fresh_var());});
if(vd->initializer != 0){
struct _tuple11 _tmp173=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple11 _stmttmp11=_tmp173;struct _tuple11 _tmp174=_stmttmp11;void*_tmp176;void*_tmp175;_LL1: _tmp175=_tmp174.f1;_tmp176=_tmp174.f2;_LL2: {void*t=_tmp175;void*a1=_tmp176;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 1029
return a;}
# 1032
void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmp178=d->r;void*_stmttmp12=_tmp178;void*_tmp179=_stmttmp12;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17A;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Absyn_Pat*_tmp17D;struct Cyc_Absyn_Vardecl*_tmp180;switch(*((int*)_tmp179)){case 0U: _LL1: _tmp180=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp179)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp180;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2U: _LL3: _tmp17D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp179)->f1;_tmp17E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp179)->f2;_tmp17F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp179)->f3;_LL4: {struct Cyc_Absyn_Pat*p=_tmp17D;struct Cyc_Core_Opt*vds=_tmp17E;struct Cyc_Absyn_Exp*e=_tmp17F;
# 1036
struct _tuple11 _tmp181=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple11 _stmttmp13=_tmp181;struct _tuple11 _tmp182=_stmttmp13;void*_tmp183;_LLC: _tmp183=_tmp182.f2;_LLD: {void*a=_tmp183;
{struct Cyc_List_List*_tmp184=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmp184;for(0;vds != 0;vds=vds->tl){
struct _tuple16 _tmp185=*((struct _tuple16*)vds->hd);struct _tuple16 _stmttmp14=_tmp185;struct _tuple16 _tmp186=_stmttmp14;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Vardecl**_tmp187;_LLF: _tmp187=_tmp186.f1;_tmp188=_tmp186.f2;_LL10: {struct Cyc_Absyn_Vardecl**vdopt=_tmp187;struct Cyc_Absyn_Exp*eopt=_tmp188;
if(vdopt != 0){
if(eopt == 0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 1042
struct _tuple11 _tmp189=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple11 _stmttmp15=_tmp189;struct _tuple11 _tmp18A=_stmttmp15;void*_tmp18C;void*_tmp18B;_LL12: _tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;_LL13: {void*t=_tmp18B;void*a1=_tmp18C;
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp18D=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->tag=1U,_tmp18E->f1=*vdopt;_tmp18E;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*tv=_tmp18D;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 1048
return a;}}case 3U: _LL5: _tmp17C=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp179)->f1;_LL6: {struct Cyc_List_List*vds=_tmp17C;
# 1050
for(0;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4U: _LL7: _tmp17A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp179)->f2;_tmp17B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp179)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp17A;struct Cyc_Absyn_Exp*eopt=_tmp17B;
# 1054
if(eopt != 0){
struct _tuple11 _tmp18F=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple11 _stmttmp16=_tmp18F;struct _tuple11 _tmp190=_stmttmp16;void*_tmp192;void*_tmp191;_LL15: _tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_LL16: {void*t=_tmp191;void*a1=_tmp192;
a=a1;}}
# 1059
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default: _LL9: _LLA:
 return a;}_LL0:;}
# 1065
struct _tuple11 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp193=e->r;void*_stmttmp17=_tmp193;void*_tmp194=_stmttmp17;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp196;enum Cyc_Absyn_Primop _tmp195;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp19F;switch(*((int*)_tmp194)){case 6U: _LL1: _tmp19F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp194)->f1;_tmp1A0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp194)->f2;_tmp1A1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp194)->f3;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp19F;struct Cyc_Absyn_Exp*e2=_tmp1A0;struct Cyc_Absyn_Exp*e3=_tmp1A1;
# 1068
struct _tuple11 _tmp1A2=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp18=_tmp1A2;struct _tuple11 _tmp1A3=_stmttmp18;void*_tmp1A5;void*_tmp1A4;_LL10: _tmp1A4=_tmp1A3.f1;_tmp1A5=_tmp1A3.f2;_LL11: {void*a1t=_tmp1A4;void*a1f=_tmp1A5;
struct _tuple11 _tmp1A6=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple11 _stmttmp19=_tmp1A6;struct _tuple11 _tmp1A7=_stmttmp19;void*_tmp1A9;void*_tmp1A8;_LL13: _tmp1A8=_tmp1A7.f1;_tmp1A9=_tmp1A7.f2;_LL14: {void*a2t=_tmp1A8;void*a2f=_tmp1A9;
struct _tuple11 _tmp1AA=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple11 _stmttmp1A=_tmp1AA;struct _tuple11 _tmp1AB=_stmttmp1A;void*_tmp1AD;void*_tmp1AC;_LL16: _tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f2;_LL17: {void*a3t=_tmp1AC;void*a3f=_tmp1AD;
return({struct _tuple11 _tmp2CA;({void*_tmp341=Cyc_Vcgen_or(a2t,a3t);_tmp2CA.f1=_tmp341;}),({void*_tmp340=Cyc_Vcgen_or(a2f,a3f);_tmp2CA.f2=_tmp340;});_tmp2CA;});}}}}case 7U: _LL3: _tmp19D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp194)->f1;_tmp19E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp194)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp19D;struct Cyc_Absyn_Exp*e2=_tmp19E;
# 1073
struct _tuple11 _tmp1AE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp1B=_tmp1AE;struct _tuple11 _tmp1AF=_stmttmp1B;void*_tmp1B1;void*_tmp1B0;_LL19: _tmp1B0=_tmp1AF.f1;_tmp1B1=_tmp1AF.f2;_LL1A: {void*a1t=_tmp1B0;void*a1f=_tmp1B1;
struct _tuple11 _tmp1B2=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple11 _stmttmp1C=_tmp1B2;struct _tuple11 _tmp1B3=_stmttmp1C;void*_tmp1B5;void*_tmp1B4;_LL1C: _tmp1B4=_tmp1B3.f1;_tmp1B5=_tmp1B3.f2;_LL1D: {void*a2t=_tmp1B4;void*a2f=_tmp1B5;
return({struct _tuple11 _tmp2CB;_tmp2CB.f1=a2t,({void*_tmp342=Cyc_Vcgen_or(a1f,a2f);_tmp2CB.f2=_tmp342;});_tmp2CB;});}}}case 8U: _LL5: _tmp19B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp194)->f1;_tmp19C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp194)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp19B;struct Cyc_Absyn_Exp*e2=_tmp19C;
# 1077
struct _tuple11 _tmp1B6=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp1D=_tmp1B6;struct _tuple11 _tmp1B7=_stmttmp1D;void*_tmp1B9;void*_tmp1B8;_LL1F: _tmp1B8=_tmp1B7.f1;_tmp1B9=_tmp1B7.f2;_LL20: {void*a1t=_tmp1B8;void*a1f=_tmp1B9;
struct _tuple11 _tmp1BA=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple11 _stmttmp1E=_tmp1BA;struct _tuple11 _tmp1BB=_stmttmp1E;void*_tmp1BD;void*_tmp1BC;_LL22: _tmp1BC=_tmp1BB.f1;_tmp1BD=_tmp1BB.f2;_LL23: {void*a2t=_tmp1BC;void*a2f=_tmp1BD;
return({struct _tuple11 _tmp2CC;({void*_tmp343=Cyc_Vcgen_or(a1t,a2t);_tmp2CC.f1=_tmp343;}),_tmp2CC.f2=a2f;_tmp2CC;});}}}case 9U: _LL7: _tmp199=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp194)->f1;_tmp19A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp194)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp199;struct Cyc_Absyn_Exp*e2=_tmp19A;
# 1081
struct _tuple11 _tmp1BE=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp1F=_tmp1BE;struct _tuple11 _tmp1BF=_stmttmp1F;void*_tmp1C0;_LL25: _tmp1C0=_tmp1BF.f2;_LL26: {void*ain=_tmp1C0;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2 != 0){_LL9: _tmp198=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2)->hd;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp198;
# 1084
struct _tuple11 _tmp1C1=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp20=_tmp1C1;struct _tuple11 _tmp1C2=_stmttmp20;void*_tmp1C4;void*_tmp1C3;_LL28: _tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;_LL29: {void*at=_tmp1C3;void*af=_tmp1C4;
return({struct _tuple11 _tmp2CD;_tmp2CD.f1=af,_tmp2CD.f2=at;_tmp2CD;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2)->tl)->tl == 0){_LLB: _tmp195=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f1;_tmp196=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2)->hd;_tmp197=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp194)->f2)->tl)->hd;_LLC: {enum Cyc_Absyn_Primop p=_tmp195;struct Cyc_Absyn_Exp*e1=_tmp196;struct Cyc_Absyn_Exp*e2=_tmp197;
# 1087
struct _tuple11 _tmp1C5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp21=_tmp1C5;struct _tuple11 _tmp1C6=_stmttmp21;void*_tmp1C8;void*_tmp1C7;_LL2B: _tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;_LL2C: {void*t1=_tmp1C7;void*a1=_tmp1C8;
struct _tuple11 _tmp1C9=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple11 _stmttmp22=_tmp1C9;struct _tuple11 _tmp1CA=_stmttmp22;void*_tmp1CC;void*_tmp1CB;_LL2E: _tmp1CB=_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;_LL2F: {void*t2=_tmp1CB;void*a2=_tmp1CC;
void*_tmp1CD=a2;void*at=_tmp1CD;
void*_tmp1CE=a2;void*af=_tmp1CE;
{enum Cyc_Absyn_Primop _tmp1CF=p;switch(_tmp1CF){case Cyc_Absyn_Eq: _LL31: _LL32:
 at=({void*_tmp344=at;Cyc_Vcgen_and(_tmp344,Cyc_Vcgen_eq(t1,t2));});af=({void*_tmp345=af;Cyc_Vcgen_and(_tmp345,Cyc_Vcgen_neq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq: _LL33: _LL34:
 at=({void*_tmp346=at;Cyc_Vcgen_and(_tmp346,Cyc_Vcgen_neq(t1,t2));});af=({void*_tmp347=af;Cyc_Vcgen_and(_tmp347,Cyc_Vcgen_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt: _LL35: _LL36:
 at=({void*_tmp348=at;Cyc_Vcgen_and(_tmp348,Cyc_Vcgen_lt(t1,t2));});af=({void*_tmp349=af;Cyc_Vcgen_and(_tmp349,Cyc_Vcgen_lte(t2,t1));});goto _LL30;case Cyc_Absyn_Lte: _LL37: _LL38:
 at=({void*_tmp34A=at;Cyc_Vcgen_and(_tmp34A,Cyc_Vcgen_lte(t1,t2));});af=({void*_tmp34B=af;Cyc_Vcgen_and(_tmp34B,Cyc_Vcgen_lt(t2,t1));});goto _LL30;case Cyc_Absyn_Gt: _LL39: _LL3A:
 at=({void*_tmp34C=at;Cyc_Vcgen_and(_tmp34C,Cyc_Vcgen_lt(t2,t1));});af=({void*_tmp34D=at;Cyc_Vcgen_and(_tmp34D,Cyc_Vcgen_lte(t1,t2));});goto _LL30;case Cyc_Absyn_Gte: _LL3B: _LL3C:
 at=({void*_tmp34E=at;Cyc_Vcgen_and(_tmp34E,Cyc_Vcgen_lte(t2,t1));});af=({void*_tmp34F=at;Cyc_Vcgen_and(_tmp34F,Cyc_Vcgen_lt(t1,t2));});goto _LL30;default: _LL3D: _LL3E:
 goto _LL30;}_LL30:;}
# 1100
return({struct _tuple11 _tmp2CE;_tmp2CE.f1=at,_tmp2CE.f2=af;_tmp2CE;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: _LLE: {
# 1102
struct _tuple11 _tmp1D0=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp23=_tmp1D0;struct _tuple11 _tmp1D1=_stmttmp23;void*_tmp1D3;void*_tmp1D2;_LL40: _tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;_LL41: {void*t=_tmp1D2;void*aout=_tmp1D3;
return({struct _tuple11 _tmp2CF;({void*_tmp355=({void*_tmp354=aout;Cyc_Vcgen_and(_tmp354,({void*_tmp353=t;Cyc_Vcgen_neq(_tmp353,Cyc_Vcgen_zero());}));});_tmp2CF.f1=_tmp355;}),({void*_tmp352=({void*_tmp351=aout;Cyc_Vcgen_and(_tmp351,({void*_tmp350=t;Cyc_Vcgen_eq(_tmp350,Cyc_Vcgen_zero());}));});_tmp2CF.f2=_tmp352;});_tmp2CF;});}}}_LL0:;}
# 1107
void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(0;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}
# 1113
struct _tuple14 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(0;es != 0;es=es->tl){
struct _tuple11 _tmp1D4=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple11 _stmttmp24=_tmp1D4;struct _tuple11 _tmp1D5=_stmttmp24;void*_tmp1D7;void*_tmp1D6;_LL1: _tmp1D6=_tmp1D5.f1;_tmp1D7=_tmp1D5.f2;_LL2: {void*t=_tmp1D6;void*aout=_tmp1D7;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->hd=t,_tmp1D8->tl=res;_tmp1D8;});else{
# 1122
valid=0;}}}
# 1125
return({struct _tuple14 _tmp2D0;valid?({struct Cyc_List_List**_tmp357=({struct Cyc_List_List**_tmp1D9=_cycalloc(sizeof(*_tmp1D9));({struct Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);*_tmp1D9=_tmp356;});_tmp1D9;});_tmp2D0.f1=_tmp357;}):(_tmp2D0.f1=0),_tmp2D0.f2=a;_tmp2D0;});}
# 1128
struct _tuple11 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple11 _tmp1DA=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp25=_tmp1DA;struct _tuple11 _tmp1DB=_stmttmp25;void*_tmp1DD;void*_tmp1DC;_LL1: _tmp1DC=_tmp1DB.f1;_tmp1DD=_tmp1DB.f2;_LL2: {void*topt=_tmp1DC;void*a=_tmp1DD;
return({struct _tuple11 _tmp2D1;topt == 0?({void*_tmp358=Cyc_Vcgen_fresh_var();_tmp2D1.f1=_tmp358;}):(_tmp2D1.f1=topt),_tmp2D1.f2=a;_tmp2D1;});}}
# 1133
struct _tuple15 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(0;es != 0;es=es->tl){
struct _tuple11 _tmp1DE=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple11 _stmttmp26=_tmp1DE;struct _tuple11 _tmp1DF=_stmttmp26;void*_tmp1E1;void*_tmp1E0;_LL1: _tmp1E0=_tmp1DF.f1;_tmp1E1=_tmp1DF.f2;_LL2: {void*t=_tmp1E0;void*a=_tmp1E1;
ain=a;
res=({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2->hd=t,_tmp1E2->tl=res;_tmp1E2;});}}
# 1140
return({struct _tuple15 _tmp2D2;_tmp2D2.f1=res,_tmp2D2.f2=ain;_tmp2D2;});}
# 1143
struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_tmp1E3=b;struct Cyc_Absyn_Vardecl*_tmp1E4;struct Cyc_Absyn_Vardecl*_tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1E6;struct Cyc_Absyn_Fndecl*_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E8;switch(*((int*)_tmp1E3)){case 1U: _LL1: _tmp1E8=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1E3)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1E8;
return vd;}case 2U: _LL3: _tmp1E7=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1E3)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp1E7;
return(struct Cyc_Absyn_Vardecl*)_check_null(fd->fn_vardecl);}case 0U: _LL5: _LL6:
({void*_tmp1E9=0U;({struct _fat_ptr _tmp359=({const char*_tmp1EA="vcgen bind2vardecl";_tag_fat(_tmp1EA,sizeof(char),19U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp359,_tag_fat(_tmp1E9,sizeof(void*),0U));});});case 3U: _LL7: _tmp1E6=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp1E3)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp1E6;
return vd;}case 4U: _LL9: _tmp1E5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1E3)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp1E5;
return vd;}default: _LLB: _tmp1E4=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1E3)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp1E4;
return vd;}}_LL0:;}
# 1154
struct _tuple11 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp1EB=e->r;void*_stmttmp27=_tmp1EB;void*_tmp1EC=_stmttmp27;void*_tmp1ED;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1EC)->tag == 1U){_LL1: _tmp1ED=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_LL2: {void*b=_tmp1ED;
# 1157
return({struct _tuple11 _tmp2D3;({void*_tmp35B=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp35A=Cyc_Vcgen_bind2vardecl(b);_tmp1EE->f1=_tmp35A;});_tmp1EE;});_tmp2D3.f1=_tmp35B;}),_tmp2D3.f2=ain;_tmp2D3;});}}else{_LL3: _LL4:
 return({struct _tuple11 _tmp2D4;_tmp2D4.f1=0,({void*_tmp35C=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp2D4.f2=_tmp35C;});_tmp2D4;});}_LL0:;}
# 1162
void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1166
struct _tuple11 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e))return({struct _tuple11 _tmp2D5;({void*_tmp35D=(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->tag=0U,_tmp1EF->f1=e;_tmp1EF;});_tmp2D5.f1=_tmp35D;}),_tmp2D5.f2=ain;_tmp2D5;});{
void*_tmp1F0=e->r;void*_stmttmp28=_tmp1F0;void*_tmp1F1=_stmttmp28;struct _fat_ptr _tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Stmt*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_MallocInfo _tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Vardecl*_tmp1F9;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp201;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Exp*_tmp204;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;enum Cyc_Absyn_Coercion _tmp20E;struct Cyc_Absyn_Exp*_tmp20D;void*_tmp20C;struct Cyc_Absyn_Exp*_tmp20F;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_Absyn_Exp*_tmp211;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_Exp*_tmp212;struct Cyc_Absyn_Exp*_tmp215;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*_tmp217;struct Cyc_Absyn_Exp*_tmp216;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21A;enum Cyc_Absyn_Incrementor _tmp21E;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Core_Opt*_tmp220;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_List_List*_tmp223;enum Cyc_Absyn_Primop _tmp222;void*_tmp224;union Cyc_Absyn_Cnst _tmp225;switch(*((int*)_tmp1F1)){case 0U: _LL1: _tmp225=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp225;
# 1171
return({struct _tuple11 _tmp2D6;({void*_tmp35E=(void*)({struct Cyc_Vcgen_Const_Vcgen_Term_struct*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226->tag=0U,_tmp226->f1=e;_tmp226;});_tmp2D6.f1=_tmp35E;}),_tmp2D6.f2=ain;_tmp2D6;});}case 1U: _LL3: _tmp224=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL4: {void*b=_tmp224;
return({struct _tuple11 _tmp2D7;({void*_tmp360=(void*)({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp35F=Cyc_Vcgen_bind2vardecl(b);_tmp227->f1=_tmp35F;});_tmp227;});_tmp2D7.f1=_tmp360;}),_tmp2D7.f2=ain;_tmp2D7;});}case 3U: _LL5: _tmp222=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp223=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL6: {enum Cyc_Absyn_Primop p=_tmp222;struct Cyc_List_List*es=_tmp223;
# 1174
struct _tuple14 _tmp228=Cyc_Vcgen_vcgen_rexps(env,es,ain);struct _tuple14 _stmttmp29=_tmp228;struct _tuple14 _tmp229=_stmttmp29;void*_tmp22B;struct Cyc_List_List**_tmp22A;_LL56: _tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_LL57: {struct Cyc_List_List**ts=_tmp22A;void*aout=_tmp22B;
void*t=0;
if(ts != 0)t=Cyc_Vcgen_primop(p,*ts);
return({struct _tuple11 _tmp2D8;_tmp2D8.f1=t,_tmp2D8.f2=aout;_tmp2D8;});}}case 4U: _LL7: _tmp21F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp220=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_tmp221=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1F1)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp21F;struct Cyc_Core_Opt*popt=_tmp220;struct Cyc_Absyn_Exp*e2=_tmp221;
# 1179
struct _tuple11 _tmp22C=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple11 _stmttmp2A=_tmp22C;struct _tuple11 _tmp22D=_stmttmp2A;void*_tmp22F;void*_tmp22E;_LL59: _tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;_LL5A: {void*lt=_tmp22E;void*a1=_tmp22F;
struct _tuple11 _tmp230=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple11 _stmttmp2B=_tmp230;struct _tuple11 _tmp231=_stmttmp2B;void*_tmp233;void*_tmp232;_LL5C: _tmp232=_tmp231.f1;_tmp233=_tmp231.f2;_LL5D: {void*t2=_tmp232;void*a2=_tmp233;
if(popt != 0){
void*_tmp234=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp234;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 1186
t2=({enum Cyc_Absyn_Primop _tmp361=(enum Cyc_Absyn_Primop)popt->v;Cyc_Vcgen_primop(_tmp361,({void*_tmp235[2U];_tmp235[0]=t1,_tmp235[1]=t2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp235,sizeof(void*),2U));}));});}}{
# 1188
void*_tmp236=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp236;
return({struct _tuple11 _tmp2D9;_tmp2D9.f1=lt,_tmp2D9.f2=a;_tmp2D9;});}}}}case 5U: _LL9: _tmp21D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp21E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp21D;enum Cyc_Absyn_Incrementor i=_tmp21E;
# 1191
struct _tuple11 _tmp237=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple11 _stmttmp2C=_tmp237;struct _tuple11 _tmp238=_stmttmp2C;void*_tmp23A;void*_tmp239;_LL5F: _tmp239=_tmp238.f1;_tmp23A=_tmp238.f2;_LL60: {void*lt=_tmp239;void*a1=_tmp23A;
void*_tmp23B=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp23B;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp23C=i;switch(_tmp23C){case Cyc_Absyn_PreInc: _LL62: _LL63:
# 1197
 if(t1 != 0)assn=(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E->tag=3U,_tmp23E->f1=Cyc_Absyn_Plus,({struct Cyc_List_List*_tmp363=({void*_tmp23D[2U];_tmp23D[0]=t1,({void*_tmp362=Cyc_Vcgen_one();_tmp23D[1]=_tmp362;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp23D,sizeof(void*),2U));});_tmp23E->f2=_tmp363;});_tmp23E;});
res=assn;
goto _LL61;case Cyc_Absyn_PostInc: _LL64: _LL65:
# 1201
 res=t1;
if(t1 != 0)assn=(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240->tag=3U,_tmp240->f1=Cyc_Absyn_Plus,({struct Cyc_List_List*_tmp365=({void*_tmp23F[2U];_tmp23F[0]=t1,({void*_tmp364=Cyc_Vcgen_one();_tmp23F[1]=_tmp364;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp23F,sizeof(void*),2U));});_tmp240->f2=_tmp365;});_tmp240;});
goto _LL61;case Cyc_Absyn_PreDec: _LL66: _LL67:
# 1205
 if(t1 != 0)assn=(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->tag=3U,_tmp242->f1=Cyc_Absyn_Minus,({struct Cyc_List_List*_tmp367=({void*_tmp241[2U];_tmp241[0]=t1,({void*_tmp366=Cyc_Vcgen_one();_tmp241[1]=_tmp366;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp241,sizeof(void*),2U));});_tmp242->f2=_tmp367;});_tmp242;});
res=assn;
goto _LL61;case Cyc_Absyn_PostDec: _LL68: _LL69:
# 1209
 res=t1;
if(t1 != 0)assn=(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244->tag=3U,_tmp244->f1=Cyc_Absyn_Minus,({struct Cyc_List_List*_tmp369=({void*_tmp243[2U];_tmp243[0]=t1,({void*_tmp368=Cyc_Vcgen_one();_tmp243[1]=_tmp368;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp243,sizeof(void*),2U));});_tmp244->f2=_tmp369;});_tmp244;});
goto _LL61;default: _LL6A: _LL6B:
({void*_tmp245=0U;({struct _fat_ptr _tmp36A=({const char*_tmp246="vcgen vcgen_rexp increment_e";_tag_fat(_tmp246,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp36A,_tag_fat(_tmp245,sizeof(void*),0U));});});}_LL61:;}{
# 1214
void*_tmp247=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp247;
return({struct _tuple11 _tmp2DA;_tmp2DA.f1=res,_tmp2DA.f2=a;_tmp2DA;});}}}case 6U: _LLB: _tmp21A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp21B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_tmp21C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1F1)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp21A;struct Cyc_Absyn_Exp*e2=_tmp21B;struct Cyc_Absyn_Exp*e3=_tmp21C;
# 1217
struct _tuple11 _tmp248=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp2D=_tmp248;struct _tuple11 _tmp249=_stmttmp2D;void*_tmp24B;void*_tmp24A;_LL6D: _tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;_LL6E: {void*at=_tmp24A;void*af=_tmp24B;
struct _tuple11 _tmp24C=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple11 _stmttmp2E=_tmp24C;struct _tuple11 _tmp24D=_stmttmp2E;void*_tmp24F;void*_tmp24E;_LL70: _tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;_LL71: {void*t1=_tmp24E;void*at=_tmp24F;
struct _tuple11 _tmp250=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple11 _stmttmp2F=_tmp250;struct _tuple11 _tmp251=_stmttmp2F;void*_tmp253;void*_tmp252;_LL73: _tmp252=_tmp251.f1;_tmp253=_tmp251.f2;_LL74: {void*t2=_tmp252;void*af=_tmp253;
if(t1 != 0 && t2 != 0){
void*_tmp254=Cyc_Vcgen_fresh_var();void*v=_tmp254;
at=({void*_tmp36B=at;Cyc_Vcgen_and(_tmp36B,Cyc_Vcgen_eq(v,t1));});
af=({void*_tmp36C=af;Cyc_Vcgen_and(_tmp36C,Cyc_Vcgen_eq(v,t2));});
return({struct _tuple11 _tmp2DB;_tmp2DB.f1=v,({void*_tmp36D=Cyc_Vcgen_or(at,af);_tmp2DB.f2=_tmp36D;});_tmp2DB;});}else{
# 1226
return({struct _tuple11 _tmp2DC;_tmp2DC.f1=0,({void*_tmp36E=Cyc_Vcgen_or(at,af);_tmp2DC.f2=_tmp36E;});_tmp2DC;});}}}}}case 7U: _LLD: _tmp218=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp219=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp218;struct Cyc_Absyn_Exp*e2=_tmp219;
# 1228
struct _tuple11 _tmp255=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp30=_tmp255;struct _tuple11 _tmp256=_stmttmp30;void*_tmp258;void*_tmp257;_LL76: _tmp257=_tmp256.f1;_tmp258=_tmp256.f2;_LL77: {void*at=_tmp257;void*af=_tmp258;
struct _tuple11 _tmp259=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple11 _stmttmp31=_tmp259;struct _tuple11 _tmp25A=_stmttmp31;void*_tmp25C;void*_tmp25B;_LL79: _tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;_LL7A: {void*t=_tmp25B;void*at=_tmp25C;
void*_tmp25D=Cyc_Vcgen_fresh_var();void*v=_tmp25D;
at=({void*_tmp36F=at;Cyc_Vcgen_and(_tmp36F,Cyc_Vcgen_eq(v,t));});
af=({void*_tmp371=af;Cyc_Vcgen_and(_tmp371,({void*_tmp370=v;Cyc_Vcgen_eq(_tmp370,Cyc_Vcgen_zero());}));});
return({struct _tuple11 _tmp2DD;_tmp2DD.f1=v,({void*_tmp372=Cyc_Vcgen_or(at,af);_tmp2DD.f2=_tmp372;});_tmp2DD;});}}}case 8U: _LLF: _tmp216=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp217=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp216;struct Cyc_Absyn_Exp*e2=_tmp217;
# 1235
struct _tuple11 _tmp25E=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple11 _stmttmp32=_tmp25E;struct _tuple11 _tmp25F=_stmttmp32;void*_tmp261;void*_tmp260;_LL7C: _tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_LL7D: {void*at=_tmp260;void*af=_tmp261;
struct _tuple11 _tmp262=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple11 _stmttmp33=_tmp262;struct _tuple11 _tmp263=_stmttmp33;void*_tmp265;void*_tmp264;_LL7F: _tmp264=_tmp263.f1;_tmp265=_tmp263.f2;_LL80: {void*t=_tmp264;void*af=_tmp265;
void*_tmp266=Cyc_Vcgen_fresh_var();void*v=_tmp266;
at=({void*_tmp374=at;Cyc_Vcgen_and(_tmp374,({void*_tmp373=v;Cyc_Vcgen_eq(_tmp373,Cyc_Vcgen_one());}));});
af=({void*_tmp375=af;Cyc_Vcgen_and(_tmp375,Cyc_Vcgen_eq(v,t));});
return({struct _tuple11 _tmp2DE;_tmp2DE.f1=v,({void*_tmp376=Cyc_Vcgen_or(at,af);_tmp2DE.f2=_tmp376;});_tmp2DE;});}}}case 9U: _LL11: _tmp214=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp215=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp214;struct Cyc_Absyn_Exp*e2=_tmp215;
# 1242
struct _tuple11 _tmp267=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp34=_tmp267;struct _tuple11 _tmp268=_stmttmp34;void*_tmp269;_LL82: _tmp269=_tmp268.f2;_LL83: {void*a=_tmp269;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10U: _LL13: _tmp212=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp213=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmp212;struct Cyc_List_List*es=_tmp213;
# 1245
ain=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);{
struct _tuple11 _tmp26A=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp35=_tmp26A;struct _tuple11 _tmp26B=_stmttmp35;void*_tmp26C;_LL85: _tmp26C=_tmp26B.f2;_LL86: {void*a2=_tmp26C;
# 1249
a2=Cyc_Vcgen_kill_mem(a2);
# 1252
Cyc_Vcgen_update_try_assn(env,a2);
# 1254
if(Cyc_Tcutil_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_Vcgen_false_assn;
return({struct _tuple11 _tmp2DF;_tmp2DF.f1=0,_tmp2DF.f2=a2;_tmp2DF;});}}}case 11U: _LL15: _tmp211=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp211;
# 1257
struct _tuple11 _tmp26D=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp36=_tmp26D;struct _tuple11 _tmp26E=_stmttmp36;void*_tmp26F;_LL88: _tmp26F=_tmp26E.f2;_LL89: {void*a=_tmp26F;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple11 _tmp2E0;_tmp2E0.f1=0,_tmp2E0.f2=(void*)& Cyc_Vcgen_false_assn;_tmp2E0;});}}case 12U: _LL17: _tmp210=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp210;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13U: _LL19: _tmp20F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp20F;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14U: _LL1B: _tmp20C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp20D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_tmp20E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1F1)->f4;_LL1C: {void*t=_tmp20C;struct Cyc_Absyn_Exp*e=_tmp20D;enum Cyc_Absyn_Coercion c=_tmp20E;
# 1265
struct _tuple11 _tmp270=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple11 _stmttmp37=_tmp270;struct _tuple11 _tmp271=_stmttmp37;void*_tmp273;void*_tmp272;_LL8B: _tmp272=_tmp271.f1;_tmp273=_tmp271.f2;_LL8C: {void*t=_tmp272;void*a=_tmp273;
if((int)c == (int)1U)
return({struct _tuple11 _tmp2E1;_tmp2E1.f1=t,_tmp2E1.f2=a;_tmp2E1;});else{
# 1269
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple11 _tmp2E2;_tmp2E2.f1=0,_tmp2E2.f2=a;_tmp2E2;});}}}case 16U: _LL1D: _tmp20A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp20B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL1E: {struct Cyc_Absyn_Exp*eopt=_tmp20A;struct Cyc_Absyn_Exp*e=_tmp20B;
# 1273
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple11 _tmp2E3;_tmp2E3.f1=0,({void*_tmp377=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp2E3.f2=_tmp377;});_tmp2E3;});}case 32U: _LL1F: _LL20:
# 1278
 goto _LL22;case 33U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 19U: _LL29: _LL2A:
 return({struct _tuple11 _tmp2E4;_tmp2E4.f1=0,_tmp2E4.f2=ain;_tmp2E4;});case 20U: _LL2B: _tmp209=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp209;
# 1286
_tmp208=e;goto _LL2E;}case 21U: _LL2D: _tmp208=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp208;
_tmp207=e;goto _LL30;}case 22U: _LL2F: _tmp207=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp207;
# 1289
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple11 _tmp2E5;_tmp2E5.f1=0,_tmp2E5.f2=ain;_tmp2E5;});}case 38U: _LL31: _tmp206=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp206;
# 1293
_tmp205=e;goto _LL34;}case 15U: _LL33: _tmp205=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL34: {struct Cyc_Absyn_Exp*e=_tmp205;
# 1295
return({struct _tuple11 _tmp2E6;_tmp2E6.f1=0,({void*_tmp378=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp2E6.f2=_tmp378;});_tmp2E6;});}case 23U: _LL35: _tmp203=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp204=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp203;struct Cyc_Absyn_Exp*e2=_tmp204;
# 1298
struct _tuple11 _tmp274=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp38=_tmp274;struct _tuple11 _tmp275=_stmttmp38;void*_tmp277;void*_tmp276;_LL8E: _tmp276=_tmp275.f1;_tmp277=_tmp275.f2;_LL8F: {void*t1=_tmp276;void*a1=_tmp277;
struct _tuple11 _tmp278=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple11 _stmttmp39=_tmp278;struct _tuple11 _tmp279=_stmttmp39;void*_tmp27B;void*_tmp27A;_LL91: _tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_LL92: {void*t2=_tmp27A;void*a2=_tmp27B;
{void*_tmp27C=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp3A=_tmp27C;void*_tmp27D=_stmttmp3A;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp27D)->tag == 6U){_LL94: _LL95:
 goto _LL93;}else{_LL96: _LL97:
# 1303
 if(t1 != 0){
Cyc_Vcgen_update_try_assn(env,ain);
a2=({void*_tmp37B=a2;Cyc_Vcgen_and(_tmp37B,({void*_tmp37A=t2;Cyc_Vcgen_lt(_tmp37A,(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->tag=3U,_tmp27F->f1=Cyc_Absyn_Numelts,({struct Cyc_List_List*_tmp379=({void*_tmp27E[1U];_tmp27E[0]=t1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp27E,sizeof(void*),1U));});_tmp27F->f2=_tmp379;});_tmp27F;}));}));});}}_LL93:;}
# 1308
return({struct _tuple11 _tmp2E7;_tmp2E7.f1=0,_tmp2E7.f2=a2;_tmp2E7;});}}}case 31U: _LL37: _tmp202=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL38: {struct Cyc_List_List*es=_tmp202;
# 1310
_tmp201=es;goto _LL3A;}case 24U: _LL39: _tmp201=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL3A: {struct Cyc_List_List*es=_tmp201;
return({struct _tuple11 _tmp2E8;_tmp2E8.f1=0,({void*_tmp37C=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp2E8.f2=_tmp37C;});_tmp2E8;});}case 25U: _LL3B: _tmp200=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp200;
# 1313
_tmp1FF=dles;goto _LL3E;}case 26U: _LL3D: _tmp1FF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL3E: {struct Cyc_List_List*dles=_tmp1FF;
_tmp1FE=dles;goto _LL40;}case 29U: _LL3F: _tmp1FE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp1F1)->f3;_LL40: {struct Cyc_List_List*dles=_tmp1FE;
_tmp1FD=dles;goto _LL42;}case 36U: _LL41: _tmp1FD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL42: {struct Cyc_List_List*dles=_tmp1FD;
_tmp1FC=dles;goto _LL44;}case 30U: _LL43: _tmp1FC=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL44: {struct Cyc_List_List*dles=_tmp1FC;
# 1318
for(0;dles != 0;dles=dles->tl){
struct _tuple17*_tmp280=(struct _tuple17*)dles->hd;struct _tuple17*_stmttmp3B=_tmp280;struct _tuple17*_tmp281=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp282;_LL99: _tmp282=_tmp281->f2;_LL9A: {struct Cyc_Absyn_Exp*e=_tmp282;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1322
return({struct _tuple11 _tmp2E9;_tmp2E9.f1=0,_tmp2E9.f2=ain;_tmp2E9;});}case 27U: _LL45: _tmp1F9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp1FA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_tmp1FB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1F1)->f3;_LL46: {struct Cyc_Absyn_Vardecl*vd=_tmp1F9;struct Cyc_Absyn_Exp*e1=_tmp1FA;struct Cyc_Absyn_Exp*e2=_tmp1FB;
# 1326
struct _tuple11 _tmp283=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple11 _stmttmp3C=_tmp283;struct _tuple11 _tmp284=_stmttmp3C;void*_tmp286;void*_tmp285;_LL9C: _tmp285=_tmp284.f1;_tmp286=_tmp284.f2;_LL9D: {void*t1=_tmp285;void*a=_tmp286;
void*_tmp287=t1 == 0?Cyc_Vcgen_fresh_var(): t1;{void*t1=_tmp287;
struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp288=({struct Cyc_Vcgen_Var_Vcgen_Term_struct*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->tag=1U,_tmp290->f1=vd;_tmp290;});struct Cyc_Vcgen_Var_Vcgen_Term_struct*v=_tmp288;
# 1330
a=({struct Cyc_Vcgen_Env*_tmp37F=env;void*_tmp37E=a;void*_tmp37D=(void*)v;Cyc_Vcgen_do_assign(_tmp37F,_tmp37E,_tmp37D,Cyc_Vcgen_zero());});
while(1){
void*_tmp289=({void*_tmp380=a;Cyc_Vcgen_and(_tmp380,Cyc_Vcgen_lt((void*)v,t1));});void*at=_tmp289;
void*_tmp28A=({void*_tmp381=a;Cyc_Vcgen_and(_tmp381,Cyc_Vcgen_lte(t1,(void*)v));});void*af=_tmp28A;
struct _tuple11 _tmp28B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple11 _stmttmp3D=_tmp28B;struct _tuple11 _tmp28C=_stmttmp3D;void*_tmp28D;_LL9F: _tmp28D=_tmp28C.f2;_LLA0: {void*abody=_tmp28D;
abody=({struct Cyc_Vcgen_Env*_tmp386=env;void*_tmp385=abody;void*_tmp384=(void*)v;Cyc_Vcgen_do_assign(_tmp386,_tmp385,_tmp384,(void*)({struct Cyc_Vcgen_Primop_Vcgen_Term_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->tag=3U,_tmp28F->f1=Cyc_Absyn_Plus,({struct Cyc_List_List*_tmp383=({void*_tmp28E[2U];_tmp28E[0]=(void*)v,({void*_tmp382=Cyc_Vcgen_one();_tmp28E[1]=_tmp382;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp28E,sizeof(void*),2U));});_tmp28F->f2=_tmp383;});_tmp28F;}));});
if(Cyc_Vcgen_implies(abody,a)){a=af;break;}
a=Cyc_Vcgen_widen_assn(abody,a);}}
# 1339
return({struct _tuple11 _tmp2EA;_tmp2EA.f1=0,_tmp2EA.f2=a;_tmp2EA;});}}}case 28U: _LL47: _tmp1F8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL48: {struct Cyc_Absyn_Exp*e=_tmp1F8;
# 1341
return({struct _tuple11 _tmp2EB;_tmp2EB.f1=0,({void*_tmp387=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp2EB.f2=_tmp387;});_tmp2EB;});}case 34U: _LL49: _tmp1F7=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL4A: {struct Cyc_Absyn_MallocInfo mi=_tmp1F7;
# 1343
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple11 _tmp2EC;_tmp2EC.f1=0,({void*_tmp388=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp2EC.f2=_tmp388;});_tmp2EC;});}case 35U: _LL4B: _tmp1F5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_tmp1F6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1F1)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmp1F5;struct Cyc_Absyn_Exp*e2=_tmp1F6;
# 1347
struct _tuple11 _tmp291=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple11 _stmttmp3E=_tmp291;struct _tuple11 _tmp292=_stmttmp3E;void*_tmp294;void*_tmp293;_LLA2: _tmp293=_tmp292.f1;_tmp294=_tmp292.f2;_LLA3: {void*lt1=_tmp293;void*a1=_tmp294;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple11 _tmp295=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple11 _stmttmp3F=_tmp295;struct _tuple11 _tmp296=_stmttmp3F;void*_tmp298;void*_tmp297;_LLA5: _tmp297=_tmp296.f1;_tmp298=_tmp296.f2;_LLA6: {void*lt2=_tmp297;void*a2=_tmp298;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp299=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp299;
void*_tmp29A=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp29A;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple11 _tmp2ED;_tmp2ED.f1=0,_tmp2ED.f2=a2;_tmp2ED;});}}}}}case 37U: _LL4D: _tmp1F4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL4E: {struct Cyc_Absyn_Stmt*s=_tmp1F4;
# 1357
void**_tmp29B=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp29B;
({void**_tmp389=({void**_tmp29C=_cycalloc(sizeof(*_tmp29C));*_tmp29C=0;_tmp29C;});(env->shared)->exp_stmt=_tmp389;});{
void*_tmp29D=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp29D;
void*_tmp29E=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp29E;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple11 _tmp2EE;_tmp2EE.f1=t,_tmp2EE.f2=aout;_tmp2EE;});}}case 40U: _LL4F: _LL50:
 return({struct _tuple11 _tmp2EF;_tmp2EF.f1=0,_tmp2EF.f2=ain;_tmp2EF;});case 41U: _LL51: _tmp1F3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL52: {struct Cyc_Absyn_Exp*e=_tmp1F3;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}default: _LL53: _tmp1F2=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp1F1)->f1;_LL54: {struct _fat_ptr s=_tmp1F2;
# 1366
if(({struct _fat_ptr _tmp38A=(struct _fat_ptr)s;Cyc_strcmp(_tmp38A,({const char*_tmp29F="print_assn";_tag_fat(_tmp29F,sizeof(char),11U);}));})){
struct _fat_ptr _tmp2A0=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp2A0;
({struct Cyc_String_pa_PrintArg_struct _tmp2A3=({struct Cyc_String_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=0U,_tmp2F1.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp2F1;});struct Cyc_String_pa_PrintArg_struct _tmp2A4=({struct Cyc_String_pa_PrintArg_struct _tmp2F0;_tmp2F0.tag=0U,({struct _fat_ptr _tmp38B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_assn2string(ain));_tmp2F0.f1=_tmp38B;});_tmp2F0;});void*_tmp2A1[2U];_tmp2A1[0]=& _tmp2A3,_tmp2A1[1]=& _tmp2A4;({struct Cyc___cycFILE*_tmp38D=Cyc_stderr;struct _fat_ptr _tmp38C=({const char*_tmp2A2="%s: current assn is %s\n";_tag_fat(_tmp2A2,sizeof(char),24U);});Cyc_fprintf(_tmp38D,_tmp38C,_tag_fat(_tmp2A1,sizeof(void*),2U));});});}
# 1370
return({struct _tuple11 _tmp2F2;({void*_tmp38E=Cyc_Vcgen_zero();_tmp2F2.f1=_tmp38E;}),_tmp2F2.f2=ain;_tmp2F2;});}}_LL0:;}}
# 1375
void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
void*_tmp2A5=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_stmttmp40=_tmp2A5;void*_tmp2A6=_stmttmp40;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A8;struct Cyc_Absyn_Fndecl*_tmp2A9;switch(*((int*)_tmp2A6)){case 1U: _LL1: _tmp2A9=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2A6)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp2A9;
# 1379
({struct Cyc_String_pa_PrintArg_struct _tmp2AC=({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0U,({struct _fat_ptr _tmp38F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(fd->name));_tmp2F3.f1=_tmp38F;});_tmp2F3;});void*_tmp2AA[1U];_tmp2AA[0]=& _tmp2AC;({struct Cyc___cycFILE*_tmp391=Cyc_stderr;struct _fat_ptr _tmp390=({const char*_tmp2AB="Vcgen function %s\n";_tag_fat(_tmp2AB,sizeof(char),19U);});Cyc_fprintf(_tmp391,_tmp390,_tag_fat(_tmp2AA,sizeof(void*),1U));});});
({void*_tmp2AD=0U;({struct Cyc___cycFILE*_tmp393=Cyc_stderr;struct _fat_ptr _tmp392=({const char*_tmp2AE="---------------------------------------\n";_tag_fat(_tmp2AE,sizeof(char),41U);});Cyc_fprintf(_tmp393,_tmp392,_tag_fat(_tmp2AD,sizeof(void*),0U));});});{
struct Cyc_Vcgen_Env*_tmp2AF=Cyc_Vcgen_initial_env(tables,fd);struct Cyc_Vcgen_Env*env=_tmp2AF;
# 1383
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,(void*)& Cyc_Vcgen_true_assn);
Cyc_Vcgen_hash_cons_table=0;
goto _LL0;}}case 9U: _LL3: _tmp2A8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2A6)->f2;_LL4: {struct Cyc_List_List*ds=_tmp2A8;
_tmp2A7=ds;goto _LL6;}case 10U: _LL5: _tmp2A7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2A6)->f2;_LL6: {struct Cyc_List_List*ds=_tmp2A7;
Cyc_Vcgen_vcgen_topdecls(tables,ds);goto _LL0;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
