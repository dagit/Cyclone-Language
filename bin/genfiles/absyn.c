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
extern unsigned Cyc_Position_segment_join(unsigned,unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 631 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 837
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 854
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 874
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 882
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 884
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 886
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 893
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 895
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 910
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 920
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one (void);
# 926
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 928
void*Cyc_Absyn_starb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 930
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 932
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 934
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 949
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
# 951
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 954
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr);
# 957
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 959
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 967
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 969
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 972
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1032
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);
# 1036
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned);
# 1058
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1062
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1067
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1073
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1100
void*Cyc_Absyn_pointer_expand(void*,int);
# 1102
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1104
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1107
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1115
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);
# 1133
void Cyc_Absyn_do_nested_statement(struct Cyc_Absyn_Exp*,void*,void(*)(void*,struct Cyc_Absyn_Stmt*),int);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);
# 78 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
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
# 35 "absyn.cyc"
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(1;ss1 != 0 && ss2 != 0;(ss1=ss1->tl,ss2=ss2->tl)){
int i=Cyc_strptrcmp((struct _fat_ptr*)ss1->hd,(struct _fat_ptr*)ss2->hd);
if(i != 0)return i;}
# 40
if(ss1 != 0)return 1;
if(ss2 != 0)return -1;
return 0;}
# 44
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 48
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;void*_tmp2;union Cyc_Absyn_Nmspace _tmp1;_tmp1=_tmp0->f1;_tmp2=_tmp0->f2;{union Cyc_Absyn_Nmspace n1=_tmp1;struct _fat_ptr*v1=_tmp2;
struct _tuple0*_tmp3=q2;void*_tmp5;union Cyc_Absyn_Nmspace _tmp4;_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;{union Cyc_Absyn_Nmspace n2=_tmp4;struct _fat_ptr*v2=_tmp5;
int i=Cyc_strptrcmp(v1,v2);
if(i != 0)return i;{
struct _tuple12 _tmp6=({struct _tuple12 _tmpF5;_tmpF5.f1=n1,_tmpF5.f2=n2;_tmpF5;});struct _tuple12 _stmttmp0=_tmp6;struct _tuple12 _tmp7=_stmttmp0;void*_tmp9;void*_tmp8;switch(((_tmp7.f1).Abs_n).tag){case 4: if(((_tmp7.f2).Loc_n).tag == 4)
return 0;else{
# 60
return -1;}case 1: switch(((_tmp7.f2).Loc_n).tag){case 1: _tmp8=((_tmp7.f1).Rel_n).val;_tmp9=((_tmp7.f2).Rel_n).val;{struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 56
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: goto _LL11;default:
# 62
 return -1;}case 2: switch(((_tmp7.f2).Rel_n).tag){case 2: _tmp8=((_tmp7.f1).Abs_n).val;_tmp9=((_tmp7.f2).Abs_n).val;{struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 57
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: goto _LL11;case 1: goto _LL15;default:
# 64
 return -1;}default: switch(((_tmp7.f2).Rel_n).tag){case 3: _tmp8=((_tmp7.f1).C_n).val;_tmp9=((_tmp7.f2).C_n).val;{struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 58
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: _LL11:
# 61
 return 1;case 1: _LL15:
# 63
 return 1;default:
# 65
 return 1;}};}}}}}
# 68
int Cyc_Absyn_hash_qvar(struct _tuple0*q){return Cyc_Hashtable_hash_string(*(*q).f2);}
# 70
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
return i != 0?i: tv1->identity - tv2->identity;}
# 75
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
return C_scope?({union Cyc_Absyn_Nmspace _tmpF6;(_tmpF6.C_n).tag=3U,(_tmpF6.C_n).val=x;_tmpF6;}):({union Cyc_Absyn_Nmspace _tmpF7;(_tmpF7.Abs_n).tag=2U,(_tmpF7.Abs_n).val=x;_tmpF7;});}
# 79
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmpF8;(_tmpF8.Rel_n).tag=1U,(_tmpF8.Rel_n).val=x;_tmpF8;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 82
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpA=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmpA;union Cyc_Absyn_Nmspace _tmpB=_stmttmp1;switch((_tmpB.Loc_n).tag){case 1: if((_tmpB.Rel_n).val == 0)
goto _LL4;else{goto _LL7;}case 2: if((_tmpB.Abs_n).val == 0){_LL4:
 goto _LL6;}else{goto _LL7;}case 4: _LL6:
 return 0;default: _LL7:
 return 1;};}
# 91
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=1,_tmpC->f1=k,_tmpC->f2=0,_tmpC->f3=new_type_counter ++,_tmpC->f4=env;_tmpC;});}
# 95
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Kinds_tmko,tenv);}
# 99
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmpF9;_tmpF9.print_const=0,_tmpF9.q_volatile=0,_tmpF9.q_restrict=0,_tmpF9.real_const=0,_tmpF9.loc=loc;_tmpF9;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmpFA;_tmpFA.print_const=1,_tmpFA.q_volatile=0,_tmpFA.q_restrict=0,_tmpFA.real_const=1,_tmpFA.loc=loc;_tmpFA;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmpFB;_tmpFB.print_const=x.print_const || y.print_const,_tmpFB.q_volatile=
x.q_volatile || y.q_volatile,_tmpFB.q_restrict=
x.q_restrict || y.q_restrict,_tmpFB.real_const=
x.real_const || y.real_const,({
unsigned _tmp119=Cyc_Position_segment_join(x.loc,y.loc);_tmpFB.loc=_tmp119;});_tmpFB;});}
# 108
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 114
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 116
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmpFC;(_tmpFC.UnknownDatatype).tag=1U,(_tmpFC.UnknownDatatype).val=udi;_tmpFC;});}
# 119
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmpFD;(_tmpFD.KnownDatatype).tag=2U,(_tmpFD.KnownDatatype).val=d;_tmpFD;});}
# 122
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmpFE;(_tmpFE.UnknownDatatypefield).tag=1U,(_tmpFE.UnknownDatatypefield).val=s;_tmpFE;});}
# 125
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmpFF;(_tmpFF.KnownDatatypefield).tag=2U,((_tmpFF.KnownDatatypefield).val).f1=dd,((_tmpFF.KnownDatatypefield).val).f2=df;_tmpFF;});}
# 128
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp100;(_tmp100.UnknownAggr).tag=1U,((_tmp100.UnknownAggr).val).f1=ak,((_tmp100.UnknownAggr).val).f2=n,((_tmp100.UnknownAggr).val).f3=tagged;_tmp100;});}
# 131
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp101;(_tmp101.KnownAggr).tag=2U,(_tmp101.KnownAggr).val=ad;_tmp101;});}
# 135
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->tag=0,_tmpD->f1=c,({struct Cyc_List_List*_tmp11A=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array;})(args);_tmpD->f2=_tmp11A;});_tmpD;});}
# 138
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=9,_tmpE->f1=e;_tmpE;});}
# 154 "absyn.cyc"
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
# 180
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 182
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={3};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={4};
static struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct Cyc_Absyn_AccessCon_val={8};
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9};
void*Cyc_Absyn_rgn_handle_type(void*r){return({void*_tmpF[1];_tmpF[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_fat(_tmpF,sizeof(void*),1));});}
void*Cyc_Absyn_tag_type(void*t){return({void*_tmp10[1];_tmp10[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_fat(_tmp10,sizeof(void*),1));});}
void*Cyc_Absyn_access_eff(void*r){return({void*_tmp11[1];_tmp11[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_AccessCon_val,_tag_fat(_tmp11,sizeof(void*),1));});}
void*Cyc_Absyn_regionsof_eff(void*t){return({void*_tmp12[1];_tmp12[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_fat(_tmp12,sizeof(void*),1));});}
void*Cyc_Absyn_thin_bounds_type(void*t){return({void*_tmp13[1];_tmp13[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_fat(_tmp13,sizeof(void*),1));});}
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=0,_tmp14->f1=(void*)& Cyc_Absyn_empty_effect_cval,_tmp14->f2=ts;_tmp14;});}
# 195
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp15=0U;({void*_tmp11B=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=15,_tmp16->f1=n,_tmp16->f2=d;_tmp16;});Cyc_Absyn_app_type(_tmp11B,_tag_fat(_tmp15,sizeof(void*),0));});});}
# 198
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp17=0U;({void*_tmp11C=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=16,_tmp18->f1=fs;_tmp18;});Cyc_Absyn_app_type(_tmp11C,_tag_fat(_tmp17,sizeof(void*),0));});});}
# 201
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp19=0U;({void*_tmp11D=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->tag=17,_tmp1A->f1=s,_tmp1A->f2=k;_tmp1A;});Cyc_Absyn_app_type(_tmp11D,_tag_fat(_tmp19,sizeof(void*),0));});});}
# 204
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=0,({void*_tmp11E=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=18,_tmp1B->f1=di;_tmp1B;});_tmp1C->f1=_tmp11E;}),_tmp1C->f2=args;_tmp1C;});}
# 207
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->tag=0,({void*_tmp11F=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->tag=19,_tmp1D->f1=di;_tmp1D;});_tmp1E->f1=_tmp11F;}),_tmp1E->f2=args;_tmp1E;});}
# 210
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=0,({void*_tmp120=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=20,_tmp1F->f1=ai;_tmp1F;});_tmp20->f1=_tmp120;}),_tmp20->f2=args;_tmp20;});}
# 214
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=2,_tmp21->f1=x;_tmp21;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp22=i;switch((int)_tmp22){case 0:
 return Cyc_Absyn_float_type;case 1:
 return Cyc_Absyn_double_type;case 2:
 return Cyc_Absyn_long_double_type;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp104;_tmp104.tag=0,({struct _fat_ptr _tmp121=({const char*_tmp28="gen_float_type(";_tag_fat(_tmp28,sizeof(char),16U);});_tmp104.f1=_tmp121;});_tmp104;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp25=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp103;_tmp103.tag=12,_tmp103.f1=(int)i;_tmp103;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26=({struct Cyc_Warn_String_Warn_Warg_struct _tmp102;_tmp102.tag=0,({struct _fat_ptr _tmp122=({const char*_tmp27=")";_tag_fat(_tmp27,sizeof(char),2U);});_tmp102.f1=_tmp122;});_tmp102;});void*_tmp23[3];_tmp23[0]=& _tmp24,_tmp23[1]=& _tmp25,_tmp23[2]=& _tmp26;({int(*_tmp123)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp123(_tag_fat(_tmp23,sizeof(void*),3));});});};}
# 223
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp29=sn;switch((int)_tmp29){case Cyc_Absyn_Signed:  {
# 226
enum Cyc_Absyn_Size_of _tmp2A=sz;switch((int)_tmp2A){case Cyc_Absyn_Char_sz:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz:
 goto _LL15;default: _LL15:
 return Cyc_Absyn_slonglong_type;};}case Cyc_Absyn_Unsigned:  {
# 235
enum Cyc_Absyn_Size_of _tmp2B=sz;switch((int)_tmp2B){case Cyc_Absyn_Char_sz:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz:
 goto _LL22;default: _LL22:
 return Cyc_Absyn_ulonglong_type;};}case Cyc_Absyn_None:
# 243
 goto _LL8;default: _LL8: {
# 245
enum Cyc_Absyn_Size_of _tmp2C=sz;switch((int)_tmp2C){case Cyc_Absyn_Char_sz:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz:
 goto _LL2F;default: _LL2F:
 return Cyc_Absyn_nlonglong_type;};}};}
# 256
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 259
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 264
void*Cyc_Absyn_bounds_one (void){
static void*bone=0;
if(bone == 0)
bone=Cyc_Absyn_thin_bounds_int(1U);
return(void*)_check_null(bone);}
# 274
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 277
void*Cyc_Absyn_wchar_type (void){
int _tmp2D=Sizeof_wchar_t;switch((int)_tmp2D){case 1:
# 288 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default:
# 292
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;};}static char _tmp2E[4U]="exn";
# 297
static struct _fat_ptr Cyc_Absyn_exn_str={_tmp2E,_tmp2E,_tmp2E + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp35[15U]="Null_Exception";static char _tmp36[13U]="Array_bounds";static char _tmp37[16U]="Match_Exception";static char _tmp38[10U]="Bad_alloc";
# 301
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void){
static struct _fat_ptr builtin_exns[4U]={{_tmp35,_tmp35,_tmp35 + 15U},{_tmp36,_tmp36,_tmp36 + 13U},{_tmp37,_tmp37,_tmp37 + 16U},{_tmp38,_tmp38,_tmp38 + 10U}};
# 304
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp32=_cycalloc(sizeof(*_tmp32));({struct Cyc_Absyn_Datatypefield*_tmp126=({struct Cyc_Absyn_Datatypefield*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _tuple0*_tmp125=({struct _tuple0*_tmp30=_cycalloc(sizeof(*_tmp30));((_tmp30->f1).Abs_n).tag=2U,((_tmp30->f1).Abs_n).val=0,({
struct _fat_ptr*_tmp124=({struct _fat_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));*_tmp2F=builtin_exns[i];_tmp2F;});_tmp30->f2=_tmp124;});_tmp30;});
# 308
_tmp31->name=_tmp125;}),_tmp31->typs=0,_tmp31->loc=0U,_tmp31->sc=3U;_tmp31;});_tmp32->hd=_tmp126;}),_tmp32->tl=tufs;_tmp32;});}}
# 312
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->sc=3U,_tmp34->name=Cyc_Absyn_exn_name,_tmp34->tvs=0,({struct Cyc_Core_Opt*_tmp127=({struct Cyc_Core_Opt*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->v=tufs;_tmp33;});_tmp34->fields=_tmp127;}),_tmp34->is_extensible=1;_tmp34;});}
# 314
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 317
void*Cyc_Absyn_exn_type (void){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp105;(_tmp105.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp129=({struct Cyc_Absyn_Datatypedecl**_tmp39=_cycalloc(sizeof(*_tmp39));({struct Cyc_Absyn_Datatypedecl*_tmp128=Cyc_Absyn_exn_tud();*_tmp39=_tmp128;});_tmp39;});(_tmp105.KnownDatatype).val=_tmp129;});_tmp105;}),0);
exn_typ=({void*_tmp12D=(void*)_check_null(eopt);void*_tmp12C=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp12B=Cyc_Absyn_empty_tqual(0U);void*_tmp12A=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp12D,_tmp12C,_tmp12B,_tmp12A,Cyc_Absyn_false_type);});}
# 324
return(void*)_check_null(exn_typ);}
# 327
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp3C=_cycalloc(sizeof(*_tmp3C));({union Cyc_Absyn_Nmspace _tmp130=Cyc_Absyn_Abs_n(0,0);_tmp3C->f1=_tmp130;}),({struct _fat_ptr*_tmp12F=({struct _fat_ptr*_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct _fat_ptr _tmp12E=({const char*_tmp3A="PrintArg";_tag_fat(_tmp3A,sizeof(char),9U);});*_tmp3B=_tmp12E;});_tmp3B;});_tmp3C->f2=_tmp12F;});_tmp3C;});
return(struct _tuple0*)_check_null(q);}
# 333
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp3F=_cycalloc(sizeof(*_tmp3F));({union Cyc_Absyn_Nmspace _tmp133=Cyc_Absyn_Abs_n(0,0);_tmp3F->f1=_tmp133;}),({struct _fat_ptr*_tmp132=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp131=({const char*_tmp3D="ScanfArg";_tag_fat(_tmp3D,sizeof(char),9U);});*_tmp3E=_tmp131;});_tmp3E;});_tmp3F->f2=_tmp132;});_tmp3F;});
return(struct _tuple0*)_check_null(q);}
# 345
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp45=_cycalloc(sizeof(*_tmp45));({union Cyc_Absyn_Nmspace _tmp138=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _fat_ptr*_tmp137=({struct _fat_ptr*_tmp41=_cycalloc(sizeof(*_tmp41));({struct _fat_ptr _tmp136=({const char*_tmp40="Core";_tag_fat(_tmp40,sizeof(char),5U);});*_tmp41=_tmp136;});_tmp41;});_tmp42->hd=_tmp137;}),_tmp42->tl=0;_tmp42;}),0);_tmp45->f1=_tmp138;}),({struct _fat_ptr*_tmp135=({struct _fat_ptr*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _fat_ptr _tmp134=({const char*_tmp43="unique_region";_tag_fat(_tmp43,sizeof(char),14U);});*_tmp44=_tmp134;});_tmp44;});_tmp45->f2=_tmp135;});_tmp45;});
return(struct _tuple0*)_check_null(q);}
# 351
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void){
void*_tmp46=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);void*t=_tmp46;
struct Cyc_Absyn_Vardecl*_tmp47=({struct _tuple0*_tmp139=Cyc_Absyn_uniquergn_qvar();Cyc_Absyn_new_vardecl(0U,_tmp139,t,0);});struct Cyc_Absyn_Vardecl*vd=_tmp47;
vd->sc=3U;
return({struct Cyc_Absyn_Exp*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->topt=t,_tmp4A->loc=0U,_tmp4A->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp13B=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=1,({void*_tmp13A=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=1,_tmp48->f1=vd;_tmp48;});_tmp49->f1=_tmp13A;});_tmp49;});_tmp4A->r=_tmp13B;});_tmp4A;});}
# 360
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=3,_tmp4B->f1=s;_tmp4B;});}
# 364
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp106;_tmp106.elt_type=t,_tmp106.elt_tq=tq,(_tmp106.ptr_atts).rgn=r,(_tmp106.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp106.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp106.ptr_atts).zero_term=zt,(_tmp106.ptr_atts).ptrloc=0,(_tmp106.ptr_atts).released=rel;_tmp106;}));}
# 370
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp107;_tmp107.elt_type=t,_tmp107.elt_tq=tq,(_tmp107.ptr_atts).rgn=r,(_tmp107.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp107.ptr_atts).bounds=b,(_tmp107.ptr_atts).zero_term=zt,(_tmp107.ptr_atts).ptrloc=0,(_tmp107.ptr_atts).released=rel;_tmp107;}));}
# 376
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp108;_tmp108.elt_type=t,_tmp108.elt_tq=tq,(_tmp108.ptr_atts).rgn=r,(_tmp108.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp108.ptr_atts).bounds=b,(_tmp108.ptr_atts).zero_term=zt,(_tmp108.ptr_atts).ptrloc=0,(_tmp108.ptr_atts).released=rel;_tmp108;}));}
# 382
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm,void*rel){
return({void*_tmp140=t;void*_tmp13F=r;struct Cyc_Absyn_Tqual _tmp13E=tq;void*_tmp13D=Cyc_Absyn_bounds_one();void*_tmp13C=zeroterm;Cyc_Absyn_starb_type(_tmp140,_tmp13F,_tmp13E,_tmp13D,_tmp13C,rel);});}
# 385
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}
# 388
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm,void*rel){
return({void*_tmp145=t;void*_tmp144=r;struct Cyc_Absyn_Tqual _tmp143=tq;void*_tmp142=Cyc_Absyn_bounds_one();void*_tmp141=zeroterm;Cyc_Absyn_atb_type(_tmp145,_tmp144,_tmp143,_tmp142,_tmp141,rel);});}
# 391
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp14A=Cyc_Absyn_char_type;void*_tmp149=rgn;struct Cyc_Absyn_Tqual _tmp148=Cyc_Absyn_empty_tqual(0U);void*_tmp147=Cyc_Absyn_fat_bound_type;void*_tmp146=Cyc_Absyn_true_type;Cyc_Absyn_starb_type(_tmp14A,_tmp149,_tmp148,_tmp147,_tmp146,Cyc_Absyn_false_type);});}
# 394
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp14F=Cyc_Absyn_char_type;void*_tmp14E=rgn;struct Cyc_Absyn_Tqual _tmp14D=Cyc_Absyn_const_tqual(0U);void*_tmp14C=Cyc_Absyn_fat_bound_type;void*_tmp14B=Cyc_Absyn_true_type;Cyc_Absyn_starb_type(_tmp14F,_tmp14E,_tmp14D,_tmp14C,_tmp14B,Cyc_Absyn_false_type);});}
# 398
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 400
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=4,(_tmp4C->f1).elt_type=elt_type,(_tmp4C->f1).tq=tq,(_tmp4C->f1).num_elts=num_elts,(_tmp4C->f1).zero_term=zero_term,(_tmp4C->f1).zt_loc=ztloc;_tmp4C;});}
# 403
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=11,_tmp4D->f1=e;_tmp4D;});}
# 408
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 410
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=8,_tmp4E->f1=n,_tmp4E->f2=args,_tmp4E->f3=d,_tmp4E->f4=defn;_tmp4E;});}
# 416
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=0,({void*_tmp152=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=20,({union Cyc_Absyn_AggrInfo _tmp151=({enum Cyc_Absyn_AggrKind _tmp150=k;Cyc_Absyn_UnknownAggr(_tmp150,({struct _tuple0*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=Cyc_Absyn_rel_ns_null,_tmp4F->f2=name;_tmp4F;}),0);});_tmp50->f1=_tmp151;});_tmp50;});_tmp51->f1=_tmp152;}),_tmp51->f2=0;_tmp51;});}
# 419
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(0U,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(1U,name);}
# 422
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(0U,name,0),0);}
# 425
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(1U,name,0),0);}
# 430
void*Cyc_Absyn_compress(void*t){
void*_tmp52=t;void*_tmp53;switch(*((int*)_tmp52)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52)->f2 == 0)
goto _LL4;else{_tmp53=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52)->f2;_LL8: {void**t2opt_ref=_tmp53;
# 436
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Absyn_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f4 == 0){_LL4:
# 433
 return t;}else{_tmp53=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f4;{void**topt_ref=_tmp53;
_tmp53=topt_ref;goto _LL8;}}case 9: _tmp53=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52)->f1;{struct Cyc_Absyn_Exp*e=_tmp53;
# 442
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp54=e->r;void*_stmttmp2=_tmp54;void*_tmp55=_stmttmp2;void*_tmp56;if(*((int*)_tmp55)== 39){_tmp56=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp55)->f1;{void*t2=_tmp56;
return Cyc_Absyn_compress(t2);}}else{
return t;};}}case 11: _tmp53=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp52)->f1;{struct Cyc_Absyn_Exp*e=_tmp53;
# 448
void*_tmp57=e->topt;void*t2=_tmp57;
return t2 != 0?t2: t;}case 10: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f2 != 0){_tmp53=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f2;{void*t=_tmp53;
return Cyc_Absyn_compress(t);}}else{goto _LLF;}default: _LLF:
 return t;};}
# 456
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp109;(_tmp109.Char_c).tag=2U,((_tmp109.Char_c).val).f1=sn,((_tmp109.Char_c).val).f2=c;_tmp109;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp10A;(_tmp10A.Wchar_c).tag=3U,(_tmp10A.Wchar_c).val=s;_tmp10A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp10B;(_tmp10B.Short_c).tag=4U,((_tmp10B.Short_c).val).f1=sn,((_tmp10B.Short_c).val).f2=s;_tmp10B;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp10C;(_tmp10C.Int_c).tag=5U,((_tmp10C.Int_c).val).f1=sn,((_tmp10C.Int_c).val).f2=i;_tmp10C;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp10D;(_tmp10D.LongLong_c).tag=6U,((_tmp10D.LongLong_c).val).f1=sn,((_tmp10D.LongLong_c).val).f2=l;_tmp10D;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp10E;(_tmp10E.Float_c).tag=7U,((_tmp10E.Float_c).val).f1=s,((_tmp10E.Float_c).val).f2=i;_tmp10E;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp10F;(_tmp10F.String_c).tag=8U,(_tmp10F.String_c).val=s;_tmp10F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp110;(_tmp110.Wstring_c).tag=9U,(_tmp110.Wstring_c).val=s;_tmp110;});}
# 466
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->topt=0,_tmp58->r=r,_tmp58->loc=loc,_tmp58->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp58;});}
# 469
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp153=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=16,_tmp59->f1=rgn_handle,_tmp59->f2=e;_tmp59;});Cyc_Absyn_new_exp(_tmp153,loc);});}
# 472
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp5A=_cycalloc(sizeof(*_tmp5A));*_tmp5A=*e;_tmp5A;});}
# 475
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp154=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=0,_tmp5B->f1=c;_tmp5B;});Cyc_Absyn_new_exp(_tmp154,loc);});}
# 478
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 482
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp155=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp155,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{0U,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{0U,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);
return Cyc_Absyn_int_exp(0U,i,loc);}
# 490
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{1U,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{1U,1}}}};
if(i == 0U)return Cyc_Absyn_new_exp((void*)& uzero,loc);
if(i == 1U)return Cyc_Absyn_new_exp((void*)& uone,loc);
return Cyc_Absyn_int_exp(1U,(int)i,loc);}
# 497
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp156=Cyc_Absyn_Char_c(2U,c);Cyc_Absyn_const_exp(_tmp156,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp157=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp157,loc);});}
# 504
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp158=f(s);Cyc_Absyn_const_exp(_tmp158,loc);});}
# 507
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 511
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp15A=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=1,({void*_tmp159=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=0,_tmp5C->f1=q;_tmp5C;});_tmp5D->f1=_tmp159;});_tmp5D;});Cyc_Absyn_new_exp(_tmp15A,loc);});}
# 514
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp15B=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=1,_tmp5E->f1=b;_tmp5E;});Cyc_Absyn_new_exp(_tmp15B,loc);});}
# 518
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 521
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
return({void*_tmp15C=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=2,_tmp5F->f1=s;_tmp5F;});Cyc_Absyn_new_exp(_tmp15C,loc);});}
# 524
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp15D=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=3,_tmp60->f1=p,_tmp60->f2=es;_tmp60;});Cyc_Absyn_new_exp(_tmp15D,loc);});}
# 527
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp15F=p;struct Cyc_List_List*_tmp15E=({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=e,_tmp61->tl=0;_tmp61;});Cyc_Absyn_primop_exp(_tmp15F,_tmp15E,loc);});}
# 530
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp162=p;struct Cyc_List_List*_tmp161=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->hd=e1,({struct Cyc_List_List*_tmp160=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=e2,_tmp62->tl=0;_tmp62;});_tmp63->tl=_tmp160;});_tmp63;});Cyc_Absyn_primop_exp(_tmp162,_tmp161,loc);});}
# 533
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp163=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=35,_tmp64->f1=e1,_tmp64->f2=e2;_tmp64;});Cyc_Absyn_new_exp(_tmp163,loc);});}
# 536
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(0U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(1U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(3U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(5U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(6U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(7U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(8U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(9U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(10U,e1,e2,loc);}
# 546
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp164=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=4,_tmp65->f1=e1,_tmp65->f2=popt,_tmp65->f3=e2;_tmp65;});Cyc_Absyn_new_exp(_tmp164,loc);});}
# 549
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 552
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp165=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=5,_tmp66->f1=e,_tmp66->f2=i;_tmp66;});Cyc_Absyn_new_exp(_tmp165,loc);});}
# 555
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp166=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=6,_tmp67->f1=e1,_tmp67->f2=e2,_tmp67->f3=e3;_tmp67;});Cyc_Absyn_new_exp(_tmp166,loc);});}
# 558
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp167=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=7,_tmp68->f1=e1,_tmp68->f2=e2;_tmp68;});Cyc_Absyn_new_exp(_tmp167,loc);});}
# 561
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp168=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=8,_tmp69->f1=e1,_tmp69->f2=e2;_tmp69;});Cyc_Absyn_new_exp(_tmp168,loc);});}
# 564
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp169=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=9,_tmp6A->f1=e1,_tmp6A->f2=e2;_tmp6A;});Cyc_Absyn_new_exp(_tmp169,loc);});}
# 567
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp16A=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=10,_tmp6B->f1=e,_tmp6B->f2=es,_tmp6B->f3=0,_tmp6B->f4=0;_tmp6B;});Cyc_Absyn_new_exp(_tmp16A,loc);});}
# 570
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp16B=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=10,_tmp6C->f1=e,_tmp6C->f2=es,_tmp6C->f3=0,_tmp6C->f4=1;_tmp6C;});Cyc_Absyn_new_exp(_tmp16B,loc);});}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp16C=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=12,_tmp6D->f1=e;_tmp6D;});Cyc_Absyn_new_exp(_tmp16C,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp16D=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=13,_tmp6E->f1=e,_tmp6E->f2=ts;_tmp6E;});Cyc_Absyn_new_exp(_tmp16D,loc);});}
# 579
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp16E=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=14,_tmp6F->f1=t,_tmp6F->f2=e,_tmp6F->f3=user_cast,_tmp6F->f4=c;_tmp6F;});Cyc_Absyn_new_exp(_tmp16E,loc);});}
# 582
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp16F=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=11,_tmp70->f1=e,_tmp70->f2=0;_tmp70;});Cyc_Absyn_new_exp(_tmp16F,loc);});}
# 585
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp170=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=11,_tmp71->f1=e,_tmp71->f2=1;_tmp71;});Cyc_Absyn_new_exp(_tmp170,loc);});}
# 588
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp171=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=15,_tmp72->f1=e;_tmp72;});Cyc_Absyn_new_exp(_tmp171,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp172=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=17,_tmp73->f1=t;_tmp73;});Cyc_Absyn_new_exp(_tmp172,loc);});}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp173=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=18,_tmp74->f1=e;_tmp74;});Cyc_Absyn_new_exp(_tmp173,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp174=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=19,_tmp75->f1=t,_tmp75->f2=ofs;_tmp75;});Cyc_Absyn_new_exp(_tmp174,loc);});}
# 598
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp175=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=20,_tmp76->f1=e;_tmp76;});Cyc_Absyn_new_exp(_tmp175,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp176=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=21,_tmp77->f1=e,_tmp77->f2=n,_tmp77->f3=0,_tmp77->f4=0;_tmp77;});Cyc_Absyn_new_exp(_tmp176,loc);});}
# 602
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp177=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=22,_tmp78->f1=e,_tmp78->f2=n,_tmp78->f3=0,_tmp78->f4=0;_tmp78;});Cyc_Absyn_new_exp(_tmp177,loc);});}
# 605
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp178=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=23,_tmp79->f1=e1,_tmp79->f2=e2;_tmp79;});Cyc_Absyn_new_exp(_tmp178,loc);});}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp179=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=24,_tmp7A->f1=es;_tmp7A;});Cyc_Absyn_new_exp(_tmp179,loc);});}
# 611
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp17A=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=37,_tmp7B->f1=s;_tmp7B;});Cyc_Absyn_new_exp(_tmp17A,loc);});}
# 614
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp17B=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=39,_tmp7C->f1=t;_tmp7C;});Cyc_Absyn_new_exp(_tmp17B,loc);});}
# 618
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 621
return({void*_tmp17C=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=40,_tmp7D->f1=volatile_kw,_tmp7D->f2=tmpl,_tmp7D->f3=outs,_tmp7D->f4=ins,_tmp7D->f5=clobs;_tmp7D;});Cyc_Absyn_new_exp(_tmp17C,loc);});}
# 623
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp17D=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=41,_tmp7E->f1=e;_tmp7E;});Cyc_Absyn_new_exp(_tmp17D,loc);});}
# 626
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp17E=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=42,_tmp7F->f1=e;_tmp7F;});Cyc_Absyn_new_exp(_tmp17E,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 630
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp80=0;struct Cyc_List_List*dles=_tmp80;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));({struct _tuple13*_tmp17F=({struct _tuple13*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=0,_tmp81->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp81;});_tmp82->hd=_tmp17F;}),_tmp82->tl=dles;_tmp82;});}
return({void*_tmp181=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=26,({struct Cyc_List_List*_tmp180=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(dles);_tmp83->f1=_tmp180;});_tmp83;});Cyc_Absyn_new_exp(_tmp181,loc);});}
# 636
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 639
return({void*_tmp182=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=36,_tmp84->f1=n,_tmp84->f2=dles;_tmp84;});Cyc_Absyn_new_exp(_tmp182,loc);});}
# 642
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->r=s,_tmp85->loc=loc,_tmp85->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp85;});}
# 645
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp183=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=1,_tmp86->f1=e;_tmp86;});Cyc_Absyn_new_stmt(_tmp183,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp184=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=3,_tmp87->f1=e;_tmp87;});Cyc_Absyn_new_stmt(_tmp184,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;
return({struct Cyc_Absyn_Stmt*_tmp186=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp185=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp186,_tmp185,loc);});}struct _tuple14{void*f1;void*f2;};
# 658
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple14 _tmp88=({struct _tuple14 _tmp111;_tmp111.f1=s1->r,_tmp111.f2=s2->r;_tmp111;});struct _tuple14 _stmttmp3=_tmp88;struct _tuple14 _tmp89=_stmttmp3;if(*((int*)_tmp89.f1)== 0)
return s2;else{if(*((int*)_tmp89.f2)== 0)
return s1;else{
return({void*_tmp187=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=2,_tmp8A->f1=s1,_tmp8A->f2=s2;_tmp8A;});Cyc_Absyn_new_stmt(_tmp187,loc);});}};}
# 665
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp188=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=4,_tmp8B->f1=e,_tmp8B->f2=s1,_tmp8B->f3=s2;_tmp8B;});Cyc_Absyn_new_stmt(_tmp188,loc);});}
# 668
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp18A=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=5,(_tmp8C->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp189=Cyc_Absyn_skip_stmt(e->loc);(_tmp8C->f1).f2=_tmp189;}),_tmp8C->f2=s;_tmp8C;});Cyc_Absyn_new_stmt(_tmp18A,loc);});}
# 671
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp18D=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=9,_tmp8D->f1=e1,(_tmp8D->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp18C=Cyc_Absyn_skip_stmt(e3->loc);(_tmp8D->f2).f2=_tmp18C;}),
(_tmp8D->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp18B=Cyc_Absyn_skip_stmt(e3->loc);(_tmp8D->f3).f2=_tmp18B;}),_tmp8D->f4=s;_tmp8D;});
# 672
Cyc_Absyn_new_stmt(_tmp18D,loc);});}
# 676
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp18F=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=14,_tmp8E->f1=s,(_tmp8E->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp18E=Cyc_Absyn_skip_stmt(e->loc);(_tmp8E->f2).f2=_tmp18E;});_tmp8E;});Cyc_Absyn_new_stmt(_tmp18F,loc);});}
# 679
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp190=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=10,_tmp8F->f1=e,_tmp8F->f2=scs,_tmp8F->f3=0;_tmp8F;});Cyc_Absyn_new_stmt(_tmp190,loc);});}
# 682
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp191=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=15,_tmp90->f1=s,_tmp90->f2=scs,_tmp90->f3=0;_tmp90;});Cyc_Absyn_new_stmt(_tmp191,loc);});}
# 685
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp192=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=11,_tmp91->f1=el,_tmp91->f2=0;_tmp91;});Cyc_Absyn_new_stmt(_tmp192,loc);});}
# 688
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
return({void*_tmp193=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=8,_tmp92->f1=lab;_tmp92;});Cyc_Absyn_new_stmt(_tmp193,loc);});}
# 691
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp194=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=13,_tmp93->f1=v,_tmp93->f2=s;_tmp93;});Cyc_Absyn_new_stmt(_tmp194,loc);});}
# 694
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp195=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=12,_tmp94->f1=d,_tmp94->f2=s;_tmp94;});Cyc_Absyn_new_stmt(_tmp195,loc);});}
# 697
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp197=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=0,({struct Cyc_Absyn_Vardecl*_tmp196=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp96->f1=_tmp196;});_tmp96;});Cyc_Absyn_new_decl(_tmp197,loc);});
return({void*_tmp198=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=12,_tmp95->f1=d,_tmp95->f2=s;_tmp95;});Cyc_Absyn_new_stmt(_tmp198,loc);});}
# 701
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp199=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp199,loc);});}
# 705
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->r=p,_tmp97->topt=0,_tmp97->loc=s;_tmp97;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp19A=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=17,_tmp98->f1=e;_tmp98;});Cyc_Absyn_new_pat(_tmp19A,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9};
# 711
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->r=r,_tmp99->loc=loc;_tmp99;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp19B=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=2,_tmp9A->f1=p,_tmp9A->f2=0,_tmp9A->f3=e,_tmp9A->f4=0;_tmp9A;});Cyc_Absyn_new_decl(_tmp19B,loc);});}
# 715
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=3,_tmp9B->f1=vds;_tmp9B;});Cyc_Absyn_new_decl(_tmp19C,loc);});}
# 718
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp19D=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=4,_tmp9C->f1=tv,_tmp9C->f2=vd,_tmp9C->f3=open_exp;_tmp9C;});Cyc_Absyn_new_decl(_tmp19D,loc);});}
# 721
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 723
return({void*_tmp1A0=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=2,({struct Cyc_Absyn_Pat*_tmp19F=({void*_tmp19E=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=2,_tmp9D->f1=tv,_tmp9D->f2=vd;_tmp9D;});Cyc_Absyn_new_pat(_tmp19E,loc);});_tmp9E->f1=_tmp19F;}),_tmp9E->f2=0,_tmp9E->f3=e,_tmp9E->f4=0;_tmp9E;});Cyc_Absyn_new_decl(_tmp1A0,loc);});}
# 726
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->sc=2U,_tmp9F->name=x,_tmp9F->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1A1=Cyc_Absyn_empty_tqual(0U);_tmp9F->tq=_tmp1A1;}),_tmp9F->type=t,_tmp9F->initializer=init,_tmp9F->rgn=0,_tmp9F->attributes=0,_tmp9F->escapes=0,_tmp9F->is_proto=0;_tmp9F;});}
# 731
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
struct Cyc_Absyn_Vardecl*_tmpA0=Cyc_Absyn_new_vardecl(0U,x,t,init);struct Cyc_Absyn_Vardecl*ans=_tmpA0;
ans->sc=0U;
return ans;}
# 736
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 740
return({struct Cyc_Absyn_AggrdeclImpl*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->exist_vars=exists,_tmpA1->rgn_po=po,_tmpA1->fields=fs,_tmpA1->tagged=tagged;_tmpA1;});}
# 743
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 746
return({void*_tmp1A3=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=5,({struct Cyc_Absyn_Aggrdecl*_tmp1A2=({struct Cyc_Absyn_Aggrdecl*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->kind=k,_tmpA2->sc=s,_tmpA2->name=n,_tmpA2->tvs=ts,_tmpA2->impl=i,_tmpA2->attributes=atts,_tmpA2->expected_mem_kind=0;_tmpA2;});_tmpA3->f1=_tmp1A2;});_tmpA3;});Cyc_Absyn_new_decl(_tmp1A3,loc);});}
# 751
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 754
return({struct Cyc_Absyn_TypeDecl*_tmpA6=_cycalloc(sizeof(*_tmpA6));({void*_tmp1A5=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=0,({struct Cyc_Absyn_Aggrdecl*_tmp1A4=({struct Cyc_Absyn_Aggrdecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->kind=k,_tmpA4->sc=s,_tmpA4->name=n,_tmpA4->tvs=ts,_tmpA4->impl=i,_tmpA4->attributes=atts,_tmpA4->expected_mem_kind=0;_tmpA4;});_tmpA5->f1=_tmp1A4;});_tmpA5;});_tmpA6->r=_tmp1A5;}),_tmpA6->loc=loc;_tmpA6;});}
# 760
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 763
return Cyc_Absyn_aggr_decl(0U,s,n,ts,i,atts,loc);}
# 765
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 768
return Cyc_Absyn_aggr_decl(1U,s,n,ts,i,atts,loc);}
# 770
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 773
return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=6,({struct Cyc_Absyn_Datatypedecl*_tmp1A6=({struct Cyc_Absyn_Datatypedecl*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->sc=s,_tmpA7->name=n,_tmpA7->tvs=ts,_tmpA7->fields=fs,_tmpA7->is_extensible=is_extensible;_tmpA7;});_tmpA8->f1=_tmp1A6;});_tmpA8;});Cyc_Absyn_new_decl(_tmp1A7,loc);});}
# 776
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 779
return({struct Cyc_Absyn_TypeDecl*_tmpAB=_cycalloc(sizeof(*_tmpAB));({void*_tmp1A9=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=2,({struct Cyc_Absyn_Datatypedecl*_tmp1A8=({struct Cyc_Absyn_Datatypedecl*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->sc=s,_tmpA9->name=n,_tmpA9->tvs=ts,_tmpA9->fields=fs,_tmpA9->is_extensible=is_extensible;_tmpA9;});_tmpAA->f1=_tmp1A8;});_tmpAA;});_tmpAB->r=_tmp1A9;}),_tmpAB->loc=loc;_tmpAB;});}
# 791 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 797
{struct Cyc_List_List*_tmpAC=args;struct Cyc_List_List*args2=_tmpAC;for(0;args2 != 0;args2=args2->tl){
({void*_tmp1AA=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f3,1);(*((struct _tuple8*)args2->hd)).f3=_tmp1AA;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=5,(_tmpAD->f1).tvars=tvs,(_tmpAD->f1).ret_tqual=ret_tqual,({
# 801
void*_tmp1AB=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpAD->f1).ret_type=_tmp1AB;}),(_tmpAD->f1).effect=eff_type,(_tmpAD->f1).args=args,(_tmpAD->f1).c_varargs=c_varargs,(_tmpAD->f1).cyc_varargs=cyc_varargs,(_tmpAD->f1).rgn_po=rgn_po,(_tmpAD->f1).attributes=atts,(_tmpAD->f1).requires_clause=req,(_tmpAD->f1).requires_relns=0,(_tmpAD->f1).ensures_clause=ens,(_tmpAD->f1).ensures_relns=0,(_tmpAD->f1).return_value=0;_tmpAD;});}
# 816
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpAE=Cyc_Absyn_compress(t);void*_stmttmp4=_tmpAE;void*_tmpAF=_stmttmp4;if(*((int*)_tmpAF)== 5){
# 819
void*_tmpB0=fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=& Cyc_Kinds_rk;_tmpB1;}),0): Cyc_Absyn_heap_rgn_type;void*rtyp=_tmpB0;
return({void*_tmp1AF=t;void*_tmp1AE=rtyp;struct Cyc_Absyn_Tqual _tmp1AD=Cyc_Absyn_empty_tqual(0U);void*_tmp1AC=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp1AF,_tmp1AE,_tmp1AD,_tmp1AC,Cyc_Absyn_false_type);});}else{
return t;};}
# 835 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpB2=e->r;void*_stmttmp5=_tmpB2;void*_tmpB3=_stmttmp5;void*_tmpB4;switch(*((int*)_tmpB3)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)){case 2:
 return 0;case 1: _tmpB4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpB4;
_tmpB4=vd;goto _LL6;}case 4: _tmpB4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpB4;
# 840
void*_tmpB5=Cyc_Absyn_compress(vd->type);void*_stmttmp6=_tmpB5;void*_tmpB6=_stmttmp6;if(*((int*)_tmpB6)== 4)
return 0;else{
return 1;};}default:
# 844
 goto _LLA;}case 22: _LLA:
 goto _LLC;case 20: _LLC:
 goto _LLE;case 23: _LLE:
 return 1;case 21: _tmpB4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;{struct Cyc_Absyn_Exp*e1=_tmpB4;
return Cyc_Absyn_is_lvalue(e1);}case 13: _tmpB4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;{struct Cyc_Absyn_Exp*e1=_tmpB4;
return Cyc_Absyn_is_lvalue(e1);}case 12: _tmpB4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;{struct Cyc_Absyn_Exp*e1=_tmpB4;
return Cyc_Absyn_is_lvalue(e1);}default:
 return 0;};}
# 855
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*_tmpB7=fields;struct Cyc_List_List*fs=_tmpB7;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 861
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 865
struct _tuple15*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(1;i != 0 && ts != 0;(-- i,ts=ts->tl)){
;}
return ts == 0?0:(struct _tuple15*)ts->hd;}
# 871
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_tmpB8=decl->r;void*_stmttmp7=_tmpB8;void*_tmpB9=_stmttmp7;void*_tmpBA;switch(*((int*)_tmpB9)){case 5: _tmpBA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpB9)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmpBA;
return(*x->name).f2;}case 7: _tmpBA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpB9)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmpBA;
return(*x->name).f2;}case 8: _tmpBA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpB9)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmpBA;
return(*x->name).f2;}case 0: _tmpBA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB9)->f1;{struct Cyc_Absyn_Vardecl*x=_tmpBA;
return(*x->name).f2;}case 1: _tmpBA=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpB9)->f1;{struct Cyc_Absyn_Fndecl*x=_tmpBA;
return(*x->name).f2;}case 13:
 goto _LLE;case 14: _LLE:
 goto _LL10;case 15: _LL10:
 goto _LL12;case 16: _LL12:
 goto _LL14;case 2: _LL14:
 goto _LL16;case 6: _LL16:
 goto _LL18;case 3: _LL18:
 goto _LL1A;case 9: _LL1A:
 goto _LL1C;case 10: _LL1C:
 goto _LL1E;case 11: _LL1E:
 goto _LL20;case 12: _LL20:
 goto _LL22;default: _LL22:
 return 0;};}
# 894
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
for(1;decls != 0;decls=decls->tl){
struct _fat_ptr*_tmpBB=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);struct _fat_ptr*dname=_tmpBB;
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 900
return 0;}static char _tmpC2[3U]="f0";
# 903
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 905
static struct _fat_ptr f0={_tmpC2,_tmpC2,_tmpC2 + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)field_names_v,(void*)field_names_v,(void*)(field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmpC1=(unsigned)(i + 1);struct _fat_ptr**_tmpC0=_cycalloc(_check_times(_tmpC1,sizeof(struct _fat_ptr*)));({{unsigned _tmp112=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp112;++ j){
# 912
j < fsz?_tmpC0[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_tmp1B2=({struct _fat_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct _fat_ptr _tmp1B1=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpBE=({struct Cyc_Int_pa_PrintArg_struct _tmp113;_tmp113.tag=1,_tmp113.f1=(unsigned long)((int)j);_tmp113;});void*_tmpBC[1];_tmpBC[0]=& _tmpBE;({struct _fat_ptr _tmp1B0=({const char*_tmpBD="f%d";_tag_fat(_tmpBD,sizeof(char),4U);});Cyc_aprintf(_tmp1B0,_tag_fat(_tmpBC,sizeof(void*),1));});});*_tmpBF=_tmp1B1;});_tmpBF;});_tmpC0[j]=_tmp1B2;});}}0;});_tag_fat(_tmpC0,sizeof(struct _fat_ptr*),_tmpC1);});
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple16{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 916
struct _tuple16 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpC3=info;void*_tmpC5;enum Cyc_Absyn_AggrKind _tmpC4;if((_tmpC3.UnknownAggr).tag == 1){_tmpC4=((_tmpC3.UnknownAggr).val).f1;_tmpC5=((_tmpC3.UnknownAggr).val).f2;{enum Cyc_Absyn_AggrKind ak=_tmpC4;struct _tuple0*n=_tmpC5;
return({struct _tuple16 _tmp114;_tmp114.f1=ak,_tmp114.f2=n;_tmp114;});}}else{_tmpC4=(*(_tmpC3.KnownAggr).val)->kind;_tmpC5=(*(_tmpC3.KnownAggr).val)->name;{enum Cyc_Absyn_AggrKind k=_tmpC4;struct _tuple0*n=_tmpC5;
return({struct _tuple16 _tmp115;_tmp115.f1=k,_tmp115.f2=n;_tmp115;});}};}
# 922
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpC6=info;void*_tmpC7;if((_tmpC6.UnknownAggr).tag == 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp116;_tmp116.tag=0,({struct _fat_ptr _tmp1B3=({const char*_tmpCA="unchecked aggrdecl";_tag_fat(_tmpCA,sizeof(char),19U);});_tmp116.f1=_tmp1B3;});_tmp116;});void*_tmpC8[1];_tmpC8[0]=& _tmpC9;({int(*_tmp1B4)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp1B4(_tag_fat(_tmpC8,sizeof(void*),1));});});else{_tmpC7=*(_tmpC6.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpC7;
return ad;}};}
# 928
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmpCB=Cyc_Absyn_compress(t);void*_stmttmp8=_tmpCB;void*_tmpCC=_stmttmp8;union Cyc_Absyn_AggrInfo _tmpCD;void*_tmpCE;switch(*((int*)_tmpCC)){case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCC)->f1 == Cyc_Absyn_UnionA){_tmpCE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCC)->f2;{struct Cyc_List_List*fs=_tmpCE;
# 931
return fs == 0 ||((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause == 0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCC)->f1)== 20){_tmpCD=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCC)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmpCD;
# 933
union Cyc_Absyn_AggrInfo _tmpCF=info;int _tmpD1;enum Cyc_Absyn_AggrKind _tmpD0;void*_tmpD2;if((_tmpCF.KnownAggr).tag == 2){_tmpD2=*(_tmpCF.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpD2;
# 935
if((int)ad->kind != 1)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpD3=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpD3;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpD4=impl->fields;struct Cyc_List_List*fields=_tmpD4;
return fields == 0 ||((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause == 0;}}}}else{if(((_tmpCF.UnknownAggr).val).f3 == 0){_tmpD0=((_tmpCF.UnknownAggr).val).f1;{enum Cyc_Absyn_AggrKind k=_tmpD0;
return(int)k == 1;}}else{_tmpD0=((_tmpCF.UnknownAggr).val).f1;_tmpD1=(int)(((_tmpCF.UnknownAggr).val).f3)->v;{enum Cyc_Absyn_AggrKind k=_tmpD0;int b=_tmpD1;
return(int)k == 1 && !b;}}};}}else{goto _LL5;}default: _LL5:
# 943
 return 0;};}
# 946
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmpD5=Cyc_Absyn_compress(t);void*_stmttmp9=_tmpD5;void*_tmpD6=_stmttmp9;union Cyc_Absyn_AggrInfo _tmpD7;void*_tmpD8;switch(*((int*)_tmpD6)){case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD6)->f1 == Cyc_Absyn_UnionA){_tmpD8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD6)->f2;{struct Cyc_List_List*fs=_tmpD8;
# 949
return fs != 0 &&((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause != 0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD6)->f1)== 20){_tmpD7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD6)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmpD7;
# 951
union Cyc_Absyn_AggrInfo _tmpD9=info;enum Cyc_Absyn_AggrKind _tmpDA;void*_tmpDB;if((_tmpD9.KnownAggr).tag == 2){_tmpDB=*(_tmpD9.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmpDB;
# 953
if((int)ad->kind != 1)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpDC=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpDC;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpDD=impl->fields;struct Cyc_List_List*fields=_tmpDD;
return fields != 0 &&((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause != 0;}}}}else{_tmpDA=((_tmpD9.UnknownAggr).val).f1;{enum Cyc_Absyn_AggrKind k=_tmpDA;
return 0;}};}}else{goto _LL5;}default: _LL5:
# 960
 return 0;};}
# 964
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmpDE=b;void*_tmpDF;switch(*((int*)_tmpDE)){case 0: _tmpDF=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct _tuple0*qv=_tmpDF;
return qv;}case 1: _tmpDF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpDF;
_tmpDF=vd;goto _LL6;}case 3: _tmpDF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpDE)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpDF;
_tmpDF=vd;goto _LL8;}case 4: _tmpDF=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpDE)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpDF;
_tmpDF=vd;goto _LLA;}case 5: _tmpDF=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpDE)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpDF;
return vd->name;}default: _tmpDF=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpDE)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmpDF;
return fd->name;}};}
# 975
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp117;_tmp117.tag=0,({struct _fat_ptr _tmp1B5=({const char*_tmpE2="designator list not of length 1";_tag_fat(_tmpE2,sizeof(char),32U);});_tmp117.f1=_tmp1B5;});_tmp117;});void*_tmpE0[1];_tmpE0[0]=& _tmpE1;({int(*_tmp1B6)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp1B6(_tag_fat(_tmpE0,sizeof(void*),1));});});{
void*_tmpE3=(void*)ds->hd;void*_stmttmpA=_tmpE3;void*_tmpE4=_stmttmpA;void*_tmpE5;if(*((int*)_tmpE4)== 1){_tmpE5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE4)->f1;{struct _fat_ptr*f=_tmpE5;
return f;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp118;_tmp118.tag=0,({struct _fat_ptr _tmp1B7=({const char*_tmpE8="array designator in struct";_tag_fat(_tmpE8,sizeof(char),27U);});_tmp118.f1=_tmp1B7;});_tmp118;});void*_tmpE6[1];_tmpE6[0]=& _tmpE7;({int(*_tmp1B8)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;});_tmp1B8(_tag_fat(_tmpE6,sizeof(void*),1));});});};}}
# 984
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmpE9=Cyc_Absyn_compress(t);void*_stmttmpB=_tmpE9;void*_tmpEA=_stmttmpB;if(*((int*)_tmpEA)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEA)->f1)){case 11:
 return 1;case 12:
 return 0;default: goto _LL5;}else{_LL5:
 return def;};}
# 995
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
void*_tmpEB=e->r;void*_stmttmpC=_tmpEB;void*_tmpEC=_stmttmpC;void*_tmpEF;void*_tmpEE;void*_tmpED;switch(*((int*)_tmpEC)){case 10:
 goto _LL4;case 4: _LL4:
 goto _LL6;case 35: _LL6:
 goto _LL8;case 40: _LL8:
 goto _LLA;case 37: _LLA:
 goto _LLC;case 5: _LLC:
 return 0;case 39:
# 1004
 goto _LL10;case 0: _LL10:
 goto _LL12;case 1: _LL12:
 goto _LL14;case 17: _LL14:
 goto _LL16;case 19: _LL16:
 goto _LL18;case 32: _LL18:
 goto _LL1A;case 33: _LL1A:
 goto _LL1C;case 2: _LL1C:
 return 1;case 6: _tmpED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_tmpEF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;{struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;struct Cyc_Absyn_Exp*e3=_tmpEF;
# 1014
if(!Cyc_Absyn_no_side_effects_exp(e1))return 0;
_tmpED=e2;_tmpEE=e3;goto _LL20;}case 27: _tmpED=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_tmpEE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
_tmpED=e1;_tmpEE=e2;goto _LL22;}case 7: _tmpED=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
_tmpED=e1;_tmpEE=e2;goto _LL24;}case 8: _tmpED=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
_tmpED=e1;_tmpEE=e2;goto _LL26;}case 23: _tmpED=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
_tmpED=e1;_tmpEE=e2;goto _LL28;}case 9: _tmpED=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
# 1021
return Cyc_Absyn_no_side_effects_exp(e1)&& Cyc_Absyn_no_side_effects_exp(e2);}case 41: _tmpED=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;{struct Cyc_Absyn_Exp*e=_tmpED;
# 1023
_tmpED=e;goto _LL2C;}case 38: _tmpED=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL2E;}case 11: _tmpED=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL30;}case 12: _tmpED=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL32;}case 13: _tmpED=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL34;}case 14: _tmpED=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL36;}case 18: _tmpED=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL38;}case 20: _tmpED=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL3A;}case 21: _tmpED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL3C;}case 22: _tmpED=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL3E;}case 28: _tmpED=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL40;}case 42: _tmpED=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmpED;
_tmpED=e;goto _LL42;}case 15: _tmpED=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL42: {struct Cyc_Absyn_Exp*e=_tmpED;
return Cyc_Absyn_no_side_effects_exp(e);}case 34: _tmpED=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).rgn;_tmpEE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEC)->f1).num_elts;{struct Cyc_Absyn_Exp*eopt=_tmpED;struct Cyc_Absyn_Exp*e=_tmpEE;
# 1037
_tmpED=eopt;_tmpEE=e;goto _LL46;}case 16: _tmpED=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL46: {struct Cyc_Absyn_Exp*eopt=_tmpED;struct Cyc_Absyn_Exp*e=_tmpEE;
# 1039
if(!Cyc_Absyn_no_side_effects_exp(e))return 0;
if(eopt != 0)return Cyc_Absyn_no_side_effects_exp(eopt);
return 1;}case 3: _tmpED=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;{struct Cyc_List_List*es=_tmpED;
# 1043
_tmpED=es;goto _LL4A;}case 31: _tmpED=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL4A: {struct Cyc_List_List*es=_tmpED;
_tmpED=es;goto _LL4C;}case 24: _tmpED=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL4C: {struct Cyc_List_List*es=_tmpED;
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;})(Cyc_Absyn_no_side_effects_exp,es);}case 36: _tmpED=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;{struct Cyc_List_List*dles=_tmpED;
# 1047
_tmpED=dles;goto _LL50;}case 29: _tmpED=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEC)->f3;_LL50: {struct Cyc_List_List*dles=_tmpED;
_tmpED=dles;goto _LL52;}case 30: _tmpED=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL52: {struct Cyc_List_List*dles=_tmpED;
_tmpED=dles;goto _LL54;}case 26: _tmpED=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpEC)->f1;_LL54: {struct Cyc_List_List*dles=_tmpED;
_tmpED=dles;goto _LL56;}default: _tmpED=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEC)->f2;_LL56: {struct Cyc_List_List*dles=_tmpED;
# 1052
for(1;dles != 0;dles=dles->tl){
if(!Cyc_Absyn_no_side_effects_exp((*((struct _tuple13*)dles->hd)).f2))
return 0;}
return 1;}};}
# 1059
void Cyc_Absyn_do_nested_statement(struct Cyc_Absyn_Exp*e,void*env,void(*f)(void*,struct Cyc_Absyn_Stmt*),int szeof){
void*_tmpF0=e->r;void*_stmttmpD=_tmpF0;void*_tmpF1=_stmttmpD;void*_tmpF4;void*_tmpF3;void*_tmpF2;switch(*((int*)_tmpF1)){case 37: _tmpF2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;{struct Cyc_Absyn_Stmt*s=_tmpF2;
return f(env,s);}case 0:
# 1063
 goto _LL6;case 1: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 32: _LLA:
 goto _LLC;case 33: _LLC:
 goto _LLE;case 39: _LLE:
 goto _LL10;case 40: _LL10:
 goto _LL12;case 19: _LL12:
 goto _LL14;case 17: _LL14:
 goto _LL0;case 41: _tmpF2=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;{struct Cyc_Absyn_Exp*e1=_tmpF2;
# 1073
_tmpF2=e1;goto _LL18;}case 42: _tmpF2=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL1A;}case 5: _tmpF2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL1C;}case 11: _tmpF2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL1E;}case 12: _tmpF2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL20;}case 13: _tmpF2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL22;}case 14: _tmpF2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL24;}case 15: _tmpF2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL26;}case 20: _tmpF2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL28;}case 21: _tmpF2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL2A;}case 28: _tmpF2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL2C;}case 38: _tmpF2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF2;
_tmpF2=e1;goto _LL2E;}case 22: _tmpF2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF2;
Cyc_Absyn_do_nested_statement(e1,env,f,szeof);goto _LL0;}case 18: _tmpF2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;{struct Cyc_Absyn_Exp*e=_tmpF2;
# 1088
if(szeof)Cyc_Absyn_do_nested_statement(e,env,f,szeof);
goto _LL0;}case 4: _tmpF2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;{struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
# 1091
_tmpF2=e1;_tmpF3=e2;goto _LL34;}case 7: _tmpF2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
_tmpF2=e1;_tmpF3=e2;goto _LL36;}case 8: _tmpF2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
_tmpF2=e1;_tmpF3=e2;goto _LL38;}case 9: _tmpF2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
_tmpF2=e1;_tmpF3=e2;goto _LL3A;}case 23: _tmpF2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
_tmpF2=e1;_tmpF3=e2;goto _LL3C;}case 35: _tmpF2=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
_tmpF2=e1;_tmpF3=e2;goto _LL3E;}case 27: _tmpF2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmpF3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
# 1098
Cyc_Absyn_do_nested_statement(e1,env,f,szeof);
Cyc_Absyn_do_nested_statement(e2,env,f,szeof);
goto _LL0;}case 6: _tmpF2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_tmpF4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;{struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;struct Cyc_Absyn_Exp*e3=_tmpF4;
# 1103
Cyc_Absyn_do_nested_statement(e1,env,f,szeof);
Cyc_Absyn_do_nested_statement(e2,env,f,szeof);
Cyc_Absyn_do_nested_statement(e3,env,f,szeof);
goto _LL0;}case 10: _tmpF2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;{struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_List_List*lexp=_tmpF3;
# 1109
for(1;lexp != 0;lexp=lexp->tl){
Cyc_Absyn_do_nested_statement((struct Cyc_Absyn_Exp*)lexp->hd,env,f,szeof);}
Cyc_Absyn_do_nested_statement(e1,env,f,szeof);
goto _LL0;}case 26: _tmpF2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;{struct Cyc_List_List*ldt=_tmpF2;
# 1114
_tmpF2=ldt;goto _LL46;}case 25: _tmpF2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL46: {struct Cyc_List_List*ldt=_tmpF2;
_tmpF2=ldt;goto _LL48;}case 36: _tmpF2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL48: {struct Cyc_List_List*ldt=_tmpF2;
_tmpF2=ldt;goto _LL4A;}case 29: _tmpF2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF1)->f3;_LL4A: {struct Cyc_List_List*ldt=_tmpF2;
_tmpF2=ldt;goto _LL4C;}case 30: _tmpF2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL4C: {struct Cyc_List_List*ldt=_tmpF2;
# 1119
for(1;ldt != 0;ldt=ldt->tl){
Cyc_Absyn_do_nested_statement((*((struct _tuple13*)ldt->hd)).f2,env,f,szeof);}
goto _LL0;}case 24: _tmpF2=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;{struct Cyc_List_List*lexp=_tmpF2;
# 1123
_tmpF2=lexp;goto _LL50;}case 3: _tmpF2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL50: {struct Cyc_List_List*lexp=_tmpF2;
_tmpF2=lexp;goto _LL52;}case 31: _tmpF2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_LL52: {struct Cyc_List_List*lexp=_tmpF2;
# 1126
for(1;lexp != 0;lexp=lexp->tl){
Cyc_Absyn_do_nested_statement((struct Cyc_Absyn_Exp*)lexp->hd,env,f,szeof);}
goto _LL0;}case 34: _tmpF2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).rgn;_tmpF3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF1)->f1).num_elts;{struct Cyc_Absyn_Exp*e1o=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
# 1130
_tmpF2=e1o;_tmpF3=e2;goto _LL56;}default: _tmpF2=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f1;_tmpF3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF1)->f2;_LL56: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
# 1132
if(e1 != 0)Cyc_Absyn_do_nested_statement(e1,env,f,szeof);
Cyc_Absyn_do_nested_statement(e2,env,f,szeof);
goto _LL0;}}_LL0:;}
# 1138
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16};
