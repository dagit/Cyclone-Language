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
# 119 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr);
# 34 "position.h"
unsigned Cyc_Position_segment_join(unsigned,unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 98 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 491
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr);
# 494
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 496
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr);
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 662 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 870
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 891
void*Cyc_Absyn_compress_kb(void*);
# 899
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 902
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 907
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 909
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 911
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 914
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 918
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 920
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 935
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 945
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one();
# 951
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 953
void*Cyc_Absyn_starb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 956
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 959
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 961
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 976
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 978
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1049
struct _tuple0*Cyc_Absyn_uniquergn_qvar();
# 1052
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned);
# 1074
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1078
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1083
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1089
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1119
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1121
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1123
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1126
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1134
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1142
int Cyc_Absyn_attribute_cmp(void*,void*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 107 "tcutil.h"
void*Cyc_Tcutil_compress(void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 166
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 174
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 46 "evexp.h"
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 30 "absyn.cyc"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);
# 37
int Cyc_Cyclone_tovc_r=0;
# 39
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 46
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _fat_ptr*)ss1->hd,(struct _fat_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;}}
# 53
if(ss2 != 0)return - 1;
return 0;}
# 56
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple11{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 60
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;struct _fat_ptr*_tmp2;union Cyc_Absyn_Nmspace _tmp1;_LL1: _tmp1=_tmp0->f1;_tmp2=_tmp0->f2;_LL2: {union Cyc_Absyn_Nmspace n1=_tmp1;struct _fat_ptr*v1=_tmp2;
struct _tuple0*_tmp3=q2;struct _fat_ptr*_tmp5;union Cyc_Absyn_Nmspace _tmp4;_LL4: _tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_LL5: {union Cyc_Absyn_Nmspace n2=_tmp4;struct _fat_ptr*v2=_tmp5;
int i=Cyc_strptrcmp(v1,v2);
if(i != 0)return i;{
struct _tuple11 _tmp6=({struct _tuple11 _tmp181;_tmp181.f1=n1,_tmp181.f2=n2;_tmp181;});struct _tuple11 _stmttmp0=_tmp6;struct _tuple11 _tmp7=_stmttmp0;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpC;switch(((_tmp7.f1).Abs_n).tag){case 4U: if(((_tmp7.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 72
 return - 1;}case 1U: switch(((_tmp7.f2).Loc_n).tag){case 1U: _LL9: _tmpC=((_tmp7.f1).Rel_n).val;_tmpD=((_tmp7.f2).Rel_n).val;_LLA: {struct Cyc_List_List*x1=_tmpC;struct Cyc_List_List*x2=_tmpD;
# 68
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;default: _LL13: _LL14:
# 74
 return - 1;}case 2U: switch(((_tmp7.f2).Rel_n).tag){case 2U: _LLB: _tmpA=((_tmp7.f1).Abs_n).val;_tmpB=((_tmp7.f2).Abs_n).val;_LLC: {struct Cyc_List_List*x1=_tmpA;struct Cyc_List_List*x2=_tmpB;
# 69
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 76
 return - 1;}default: switch(((_tmp7.f2).Rel_n).tag){case 3U: _LLD: _tmp8=((_tmp7.f1).C_n).val;_tmp9=((_tmp7.f2).C_n).val;_LLE: {struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 70
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: _LL11: _LL12:
# 73
 return 1;case 1U: _LL15: _LL16:
# 75
 return 1;default: _LL19: _LL1A:
# 77
 return 1;}}_LL6:;}}}}}
# 81
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 87
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
return C_scope?({union Cyc_Absyn_Nmspace _tmp182;(_tmp182.C_n).tag=3U,(_tmp182.C_n).val=x;_tmp182;}):({union Cyc_Absyn_Nmspace _tmp183;(_tmp183.Abs_n).tag=2U,(_tmp183.Abs_n).val=x;_tmp183;});}
# 91
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp184;(_tmp184.Rel_n).tag=1U,(_tmp184.Rel_n).val=x;_tmp184;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 94
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmpE;union Cyc_Absyn_Nmspace _tmpF=_stmttmp1;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 103
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 107
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 111
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp185;_tmp185.print_const=0,_tmp185.q_volatile=0,_tmp185.q_restrict=0,_tmp185.real_const=0,_tmp185.loc=loc;_tmp185;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp186;_tmp186.print_const=1,_tmp186.q_volatile=0,_tmp186.q_restrict=0,_tmp186.real_const=1,_tmp186.loc=loc;_tmp186;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp187;_tmp187.print_const=x.print_const || y.print_const,_tmp187.q_volatile=
x.q_volatile || y.q_volatile,_tmp187.q_restrict=
x.q_restrict || y.q_restrict,_tmp187.real_const=
x.real_const || y.real_const,({
unsigned _tmp1AE=Cyc_Position_segment_join(x.loc,y.loc);_tmp187.loc=_tmp1AE;});_tmp187;});}
# 120
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 126
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 128
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp188;(_tmp188.UnknownDatatype).tag=1U,(_tmp188.UnknownDatatype).val=udi;_tmp188;});}
# 131
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp189;(_tmp189.KnownDatatype).tag=2U,(_tmp189.KnownDatatype).val=d;_tmp189;});}
# 134
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp18A;(_tmp18A.UnknownDatatypefield).tag=1U,(_tmp18A.UnknownDatatypefield).val=s;_tmp18A;});}
# 137
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp18B;(_tmp18B.KnownDatatypefield).tag=2U,((_tmp18B.KnownDatatypefield).val).f1=dd,((_tmp18B.KnownDatatypefield).val).f2=df;_tmp18B;});}
# 140
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp18C;(_tmp18C.UnknownAggr).tag=1U,((_tmp18C.UnknownAggr).val).f1=ak,((_tmp18C.UnknownAggr).val).f2=n,((_tmp18C.UnknownAggr).val).f3=tagged;_tmp18C;});}
# 143
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp18D;(_tmp18D.KnownAggr).tag=2U,(_tmp18D.KnownAggr).val=ad;_tmp18D;});}
# 146
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp12;void**_tmp13;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp13=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8: {void**k2=_tmp13;
# 151
_tmp12=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 150
 return k;}else{_LL9: _tmp12=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA: {void**k2=_tmp12;
# 153
({void*_tmp1AF=Cyc_Absyn_compress_kb(*k2);*k2=_tmp1AF;});
return*k2;}}}_LL0:;}
# 157
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_stmttmp2=_tmp14;void*_tmp15=_stmttmp2;struct Cyc_Absyn_Kind*_tmp17;struct Cyc_Core_Opt**_tmp16;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp19;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp19=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp19;
return k;}case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp18;
_tmp16=f;_tmp17=& Cyc_Tcutil_bk;goto _LL6;}default: _LL5: _tmp16=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6: {struct Cyc_Core_Opt**f=_tmp16;struct Cyc_Absyn_Kind*k=_tmp17;
# 162
({struct Cyc_Core_Opt*_tmp1B1=({struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));({void*_tmp1B0=Cyc_Tcutil_kind_to_bound(k);_tmp1A->v=_tmp1B0;});_tmp1A;});*f=_tmp1B1;});
return k;}}_LL0:;}
# 168
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp1B2=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp1B2;});_tmp1B;});}
# 171
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 187 "absyn.cyc"
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_Absyn_void_type_cval={0U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_void_type_tval={0U,(void*)& Cyc_Absyn_void_type_cval,0};void*Cyc_Absyn_void_type=(void*)& Cyc_Absyn_void_type_tval;
static struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct Cyc_Absyn_heap_rgn_type_cval={5U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_heap_rgn_type_tval={0U,(void*)& Cyc_Absyn_heap_rgn_type_cval,0};void*Cyc_Absyn_heap_rgn_type=(void*)& Cyc_Absyn_heap_rgn_type_tval;
static struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct Cyc_Absyn_unique_rgn_type_cval={6U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_unique_rgn_type_tval={0U,(void*)& Cyc_Absyn_unique_rgn_type_cval,0};void*Cyc_Absyn_unique_rgn_type=(void*)& Cyc_Absyn_unique_rgn_type_tval;
static struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct Cyc_Absyn_refcnt_rgn_type_cval={7U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_refcnt_rgn_type_tval={0U,(void*)& Cyc_Absyn_refcnt_rgn_type_cval,0};void*Cyc_Absyn_refcnt_rgn_type=(void*)& Cyc_Absyn_refcnt_rgn_type_tval;
static struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct Cyc_Absyn_true_type_cval={11U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_true_type_tval={0U,(void*)& Cyc_Absyn_true_type_cval,0};void*Cyc_Absyn_true_type=(void*)& Cyc_Absyn_true_type_tval;
static struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct Cyc_Absyn_false_type_cval={12U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_false_type_tval={0U,(void*)& Cyc_Absyn_false_type_cval,0};void*Cyc_Absyn_false_type=(void*)& Cyc_Absyn_false_type_tval;
static struct Cyc_Absyn_FatCon_Absyn_TyCon_struct Cyc_Absyn_fat_bound_type_cval={14U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_fat_bound_type_tval={0U,(void*)& Cyc_Absyn_fat_bound_type_cval,0};void*Cyc_Absyn_fat_bound_type=(void*)& Cyc_Absyn_fat_bound_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_schar_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_schar_type_tval={0U,(void*)& Cyc_Absyn_schar_type_cval,0};void*Cyc_Absyn_schar_type=(void*)& Cyc_Absyn_schar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uchar_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uchar_type_tval={0U,(void*)& Cyc_Absyn_uchar_type_cval,0};void*Cyc_Absyn_uchar_type=(void*)& Cyc_Absyn_uchar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_char_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_char_type_tval={0U,(void*)& Cyc_Absyn_char_type_cval,0};void*Cyc_Absyn_char_type=(void*)& Cyc_Absyn_char_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sshort_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sshort_type_tval={0U,(void*)& Cyc_Absyn_sshort_type_cval,0};void*Cyc_Absyn_sshort_type=(void*)& Cyc_Absyn_sshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ushort_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ushort_type_tval={0U,(void*)& Cyc_Absyn_ushort_type_cval,0};void*Cyc_Absyn_ushort_type=(void*)& Cyc_Absyn_ushort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nshort_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nshort_type_tval={0U,(void*)& Cyc_Absyn_nshort_type_cval,0};void*Cyc_Absyn_nshort_type=(void*)& Cyc_Absyn_nshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sint_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sint_type_tval={0U,(void*)& Cyc_Absyn_sint_type_cval,0};void*Cyc_Absyn_sint_type=(void*)& Cyc_Absyn_sint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uint_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uint_type_tval={0U,(void*)& Cyc_Absyn_uint_type_cval,0};void*Cyc_Absyn_uint_type=(void*)& Cyc_Absyn_uint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nint_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nint_type_tval={0U,(void*)& Cyc_Absyn_nint_type_cval,0};void*Cyc_Absyn_nint_type=(void*)& Cyc_Absyn_nint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slong_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slong_type_tval={0U,(void*)& Cyc_Absyn_slong_type_cval,0};void*Cyc_Absyn_slong_type=(void*)& Cyc_Absyn_slong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulong_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulong_type_tval={0U,(void*)& Cyc_Absyn_ulong_type_cval,0};void*Cyc_Absyn_ulong_type=(void*)& Cyc_Absyn_ulong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlong_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlong_type_tval={0U,(void*)& Cyc_Absyn_nlong_type_cval,0};void*Cyc_Absyn_nlong_type=(void*)& Cyc_Absyn_nlong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slonglong_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slonglong_type_tval={0U,(void*)& Cyc_Absyn_slonglong_type_cval,0};void*Cyc_Absyn_slonglong_type=(void*)& Cyc_Absyn_slonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulonglong_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulonglong_type_tval={0U,(void*)& Cyc_Absyn_ulonglong_type_cval,0};void*Cyc_Absyn_ulonglong_type=(void*)& Cyc_Absyn_ulonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlonglong_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlonglong_type_tval={0U,(void*)& Cyc_Absyn_nlonglong_type_cval,0};void*Cyc_Absyn_nlonglong_type=(void*)& Cyc_Absyn_nlonglong_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_float_type_cval={2U,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_float_type_tval={0U,(void*)& Cyc_Absyn_float_type_cval,0};void*Cyc_Absyn_float_type=(void*)& Cyc_Absyn_float_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_double_type_cval={2U,1};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_double_type_tval={0U,(void*)& Cyc_Absyn_double_type_cval,0};void*Cyc_Absyn_double_type=(void*)& Cyc_Absyn_double_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_long_double_type_cval={2U,2};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_long_double_type_tval={0U,(void*)& Cyc_Absyn_long_double_type_cval,0};void*Cyc_Absyn_long_double_type=(void*)& Cyc_Absyn_long_double_type_tval;
# 213
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0U,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 215
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={3U};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={4U};
static struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct Cyc_Absyn_AccessCon_val={8U};
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10U};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13U};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9U};
void*Cyc_Absyn_rgn_handle_type(void*r){return({void*_tmp1D[1U];_tmp1D[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_fat(_tmp1D,sizeof(void*),1U));});}
void*Cyc_Absyn_tag_type(void*t){return({void*_tmp1E[1U];_tmp1E[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_fat(_tmp1E,sizeof(void*),1U));});}
void*Cyc_Absyn_access_eff(void*r){return({void*_tmp1F[1U];_tmp1F[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_AccessCon_val,_tag_fat(_tmp1F,sizeof(void*),1U));});}
void*Cyc_Absyn_regionsof_eff(void*t){return({void*_tmp20[1U];_tmp20[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_fat(_tmp20,sizeof(void*),1U));});}
void*Cyc_Absyn_thin_bounds_type(void*t){return({void*_tmp21[1U];_tmp21[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_fat(_tmp21,sizeof(void*),1U));});}
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=0U,_tmp22->f1=(void*)& Cyc_Absyn_empty_effect_cval,_tmp22->f2=ts;_tmp22;});}
# 228
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp23=0U;({void*_tmp1B3=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=15U,_tmp24->f1=n,_tmp24->f2=d;_tmp24;});Cyc_Absyn_app_type(_tmp1B3,_tag_fat(_tmp23,sizeof(void*),0U));});});}
# 231
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp25=0U;({void*_tmp1B4=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=16U,_tmp26->f1=fs;_tmp26;});Cyc_Absyn_app_type(_tmp1B4,_tag_fat(_tmp25,sizeof(void*),0U));});});}
# 234
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp27=0U;({void*_tmp1B5=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=17U,_tmp28->f1=s,_tmp28->f2=k;_tmp28;});Cyc_Absyn_app_type(_tmp1B5,_tag_fat(_tmp27,sizeof(void*),0U));});});}
# 237
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp1B6=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=18U,_tmp29->f1=di;_tmp29;});_tmp2A->f1=_tmp1B6;}),_tmp2A->f2=args;_tmp2A;});}
# 240
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp1B7=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=di;_tmp2B;});_tmp2C->f1=_tmp1B7;}),_tmp2C->f2=args;_tmp2C;});}
# 243
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp1B8=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=20U,_tmp2D->f1=ai;_tmp2D;});_tmp2E->f1=_tmp1B8;}),_tmp2E->f2=args;_tmp2E;});}
# 247
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,_tmp2F->f1=x;_tmp2F;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp30=i;switch(_tmp30){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Int_pa_PrintArg_struct _tmp33=({struct Cyc_Int_pa_PrintArg_struct _tmp18E;_tmp18E.tag=1U,_tmp18E.f1=(unsigned long)((int)i);_tmp18E;});void*_tmp31[1U];_tmp31[0]=& _tmp33;({struct _fat_ptr _tmp1B9=({const char*_tmp32="gen_float_type(%d)";_tag_fat(_tmp32,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1B9,_tag_fat(_tmp31,sizeof(void*),1U));});});}_LL0:;}
# 256
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp34=sn;switch(_tmp34){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 259
enum Cyc_Absyn_Size_of _tmp35=sz;switch(_tmp35){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 268
enum Cyc_Absyn_Size_of _tmp36=sz;switch(_tmp36){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 276
 goto _LL8;default: _LL7: _LL8: {
# 278
enum Cyc_Absyn_Size_of _tmp37=sz;switch(_tmp37){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 289
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 292
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 297
void*Cyc_Absyn_bounds_one(){
static void*bone=0;
void*_tmp38=bone;void*b=_tmp38;
if(b == 0){
b=Cyc_Absyn_thin_bounds_int(1U);
bone=b;}
# 304
return b;}
# 310
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 313
void*Cyc_Absyn_wchar_type(){
int _tmp39=Sizeof_wchar_t;switch(_tmp39){case 1U: _LL1: _LL2:
# 324 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 328
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3A[4U]="exn";
# 333
static struct _fat_ptr Cyc_Absyn_exn_str={_tmp3A,_tmp3A,_tmp3A + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp41[15U]="Null_Exception";static char _tmp42[13U]="Array_bounds";static char _tmp43[16U]="Match_Exception";static char _tmp44[10U]="Bad_alloc";
# 337
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud(){
static struct _fat_ptr builtin_exns[4U]={{_tmp41,_tmp41,_tmp41 + 15U},{_tmp42,_tmp42,_tmp42 + 13U},{_tmp43,_tmp43,_tmp43 + 16U},{_tmp44,_tmp44,_tmp44 + 10U}};
# 340
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct Cyc_Absyn_Datatypefield*_tmp1BC=({struct Cyc_Absyn_Datatypefield*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct _tuple0*_tmp1BB=({struct _tuple0*_tmp3C=_cycalloc(sizeof(*_tmp3C));((_tmp3C->f1).Abs_n).tag=2U,((_tmp3C->f1).Abs_n).val=0,({
struct _fat_ptr*_tmp1BA=({struct _fat_ptr*_tmp3B=_cycalloc(sizeof(*_tmp3B));*_tmp3B=*((struct _fat_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _fat_ptr),i));_tmp3B;});_tmp3C->f2=_tmp1BA;});_tmp3C;});
# 344
_tmp3D->name=_tmp1BB;}),_tmp3D->typs=0,_tmp3D->loc=0U,_tmp3D->sc=Cyc_Absyn_Extern;_tmp3D;});_tmp3E->hd=_tmp1BC;}),_tmp3E->tl=tufs;_tmp3E;});}}
# 348
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->sc=Cyc_Absyn_Extern,_tmp40->name=Cyc_Absyn_exn_name,_tmp40->tvs=0,({struct Cyc_Core_Opt*_tmp1BD=({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=tufs;_tmp3F;});_tmp40->fields=_tmp1BD;}),_tmp40->is_extensible=1;_tmp40;});}
# 350
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 353
void*Cyc_Absyn_exn_type(){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp18F;(_tmp18F.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp1BF=({struct Cyc_Absyn_Datatypedecl**_tmp45=_cycalloc(sizeof(*_tmp45));({struct Cyc_Absyn_Datatypedecl*_tmp1BE=Cyc_Absyn_exn_tud();*_tmp45=_tmp1BE;});_tmp45;});(_tmp18F.KnownDatatype).val=_tmp1BF;});_tmp18F;}),0);
exn_typ=({void*_tmp1C2=(void*)_check_null(eopt);void*_tmp1C1=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp1C0=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp1C2,_tmp1C1,_tmp1C0,Cyc_Absyn_false_type);});}
# 360
return(void*)_check_null(exn_typ);}
# 363
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar(){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp48=_cycalloc(sizeof(*_tmp48));({union Cyc_Absyn_Nmspace _tmp1C5=Cyc_Absyn_Abs_n(0,0);_tmp48->f1=_tmp1C5;}),({struct _fat_ptr*_tmp1C4=({struct _fat_ptr*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _fat_ptr _tmp1C3=({const char*_tmp46="PrintArg";_tag_fat(_tmp46,sizeof(char),9U);});*_tmp47=_tmp1C3;});_tmp47;});_tmp48->f2=_tmp1C4;});_tmp48;});
return(struct _tuple0*)_check_null(q);}
# 369
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar(){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4B=_cycalloc(sizeof(*_tmp4B));({union Cyc_Absyn_Nmspace _tmp1C8=Cyc_Absyn_Abs_n(0,0);_tmp4B->f1=_tmp1C8;}),({struct _fat_ptr*_tmp1C7=({struct _fat_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct _fat_ptr _tmp1C6=({const char*_tmp49="ScanfArg";_tag_fat(_tmp49,sizeof(char),9U);});*_tmp4A=_tmp1C6;});_tmp4A;});_tmp4B->f2=_tmp1C7;});_tmp4B;});
return(struct _tuple0*)_check_null(q);}
# 381
struct _tuple0*Cyc_Absyn_uniquergn_qvar(){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp51=_cycalloc(sizeof(*_tmp51));({union Cyc_Absyn_Nmspace _tmp1CD=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));({struct _fat_ptr*_tmp1CC=({struct _fat_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _fat_ptr _tmp1CB=({const char*_tmp4C="Core";_tag_fat(_tmp4C,sizeof(char),5U);});*_tmp4D=_tmp1CB;});_tmp4D;});_tmp4E->hd=_tmp1CC;}),_tmp4E->tl=0;_tmp4E;}),0);_tmp51->f1=_tmp1CD;}),({struct _fat_ptr*_tmp1CA=({struct _fat_ptr*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _fat_ptr _tmp1C9=({const char*_tmp4F="unique_region";_tag_fat(_tmp4F,sizeof(char),14U);});*_tmp50=_tmp1C9;});_tmp50;});_tmp51->f2=_tmp1CA;});_tmp51;});
return(struct _tuple0*)_check_null(q);}
# 387
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp(){
static struct Cyc_Absyn_Exp*e=0;
if(e == 0){
void*_tmp52=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);void*t=_tmp52;
e=({struct Cyc_Absyn_Exp*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->topt=t,_tmp56->loc=0U,_tmp56->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp1D2=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=1U,({void*_tmp1D1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp1D0=({struct Cyc_Absyn_Vardecl*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->sc=Cyc_Absyn_Extern,({struct _tuple0*_tmp1CF=Cyc_Absyn_uniquergn_qvar();_tmp53->name=_tmp1CF;}),_tmp53->varloc=0U,({
struct Cyc_Absyn_Tqual _tmp1CE=Cyc_Absyn_empty_tqual(0U);_tmp53->tq=_tmp1CE;}),_tmp53->type=t,_tmp53->initializer=0,_tmp53->rgn=0,_tmp53->attributes=0,_tmp53->escapes=0;_tmp53;});
# 392
_tmp54->f1=_tmp1D0;});_tmp54;});_tmp55->f1=_tmp1D1;});_tmp55;});_tmp56->r=_tmp1D2;});_tmp56;});}
# 396
return(struct Cyc_Absyn_Exp*)_check_null(e);}
# 400
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=3U,_tmp57->f1=s;_tmp57;});}
# 404
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp190;_tmp190.elt_type=t,_tmp190.elt_tq=tq,(_tmp190.ptr_atts).rgn=r,(_tmp190.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp190.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp190.ptr_atts).zero_term=zt,(_tmp190.ptr_atts).ptrloc=0;_tmp190;}));}
# 409
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp191;_tmp191.elt_type=t,_tmp191.elt_tq=tq,(_tmp191.ptr_atts).rgn=r,(_tmp191.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp191.ptr_atts).bounds=b,(_tmp191.ptr_atts).zero_term=zt,(_tmp191.ptr_atts).ptrloc=0;_tmp191;}));}
# 414
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp192;_tmp192.elt_type=t,_tmp192.elt_tq=tq,(_tmp192.ptr_atts).rgn=r,(_tmp192.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp192.ptr_atts).bounds=b,(_tmp192.ptr_atts).zero_term=zt,(_tmp192.ptr_atts).ptrloc=0;_tmp192;}));}
# 419
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp1D6=t;void*_tmp1D5=r;struct Cyc_Absyn_Tqual _tmp1D4=tq;void*_tmp1D3=Cyc_Absyn_bounds_one();Cyc_Absyn_starb_type(_tmp1D6,_tmp1D5,_tmp1D4,_tmp1D3,zeroterm);});}
# 422
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}
# 425
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp1DA=t;void*_tmp1D9=r;struct Cyc_Absyn_Tqual _tmp1D8=tq;void*_tmp1D7=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp1DA,_tmp1D9,_tmp1D8,_tmp1D7,zeroterm);});}
# 428
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp1DE=Cyc_Absyn_char_type;void*_tmp1DD=rgn;struct Cyc_Absyn_Tqual _tmp1DC=Cyc_Absyn_empty_tqual(0U);void*_tmp1DB=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp1DE,_tmp1DD,_tmp1DC,_tmp1DB,Cyc_Absyn_true_type);});}
# 431
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp1E2=Cyc_Absyn_char_type;void*_tmp1E1=rgn;struct Cyc_Absyn_Tqual _tmp1E0=Cyc_Absyn_const_tqual(0U);void*_tmp1DF=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp1E2,_tmp1E1,_tmp1E0,_tmp1DF,Cyc_Absyn_true_type);});}
# 435
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 437
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=4U,(_tmp58->f1).elt_type=elt_type,(_tmp58->f1).tq=tq,(_tmp58->f1).num_elts=num_elts,(_tmp58->f1).zero_term=zero_term,(_tmp58->f1).zt_loc=ztloc;_tmp58;});}
# 440
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=11U,_tmp59->f1=e;_tmp59;});}
# 445
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 447
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=8U,_tmp5A->f1=n,_tmp5A->f2=args,_tmp5A->f3=d,_tmp5A->f4=defn;_tmp5A;});}
# 453
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=0U,({void*_tmp1E5=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=20U,({union Cyc_Absyn_AggrInfo _tmp1E4=({enum Cyc_Absyn_AggrKind _tmp1E3=k;Cyc_Absyn_UnknownAggr(_tmp1E3,({struct _tuple0*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->f1=Cyc_Absyn_rel_ns_null,_tmp5B->f2=name;_tmp5B;}),0);});_tmp5C->f1=_tmp1E4;});_tmp5C;});_tmp5D->f1=_tmp1E5;}),_tmp5D->f2=0;_tmp5D;});}
# 456
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 459
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 462
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 466
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp193;(_tmp193.Char_c).tag=2U,((_tmp193.Char_c).val).f1=sn,((_tmp193.Char_c).val).f2=c;_tmp193;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp194;(_tmp194.Wchar_c).tag=3U,(_tmp194.Wchar_c).val=s;_tmp194;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp195;(_tmp195.Short_c).tag=4U,((_tmp195.Short_c).val).f1=sn,((_tmp195.Short_c).val).f2=s;_tmp195;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp196;(_tmp196.Int_c).tag=5U,((_tmp196.Int_c).val).f1=sn,((_tmp196.Int_c).val).f2=i;_tmp196;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp197;(_tmp197.LongLong_c).tag=6U,((_tmp197.LongLong_c).val).f1=sn,((_tmp197.LongLong_c).val).f2=l;_tmp197;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp198;(_tmp198.Float_c).tag=7U,((_tmp198.Float_c).val).f1=s,((_tmp198.Float_c).val).f2=i;_tmp198;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp199;(_tmp199.String_c).tag=8U,(_tmp199.String_c).val=s;_tmp199;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp19A;(_tmp19A.Wstring_c).tag=9U,(_tmp19A.Wstring_c).val=s;_tmp19A;});}
# 476
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->topt=0,_tmp5E->r=r,_tmp5E->loc=loc,_tmp5E->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp5E;});}
# 479
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1E6=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=16U,_tmp5F->f1=rgn_handle,_tmp5F->f2=e;_tmp5F;});Cyc_Absyn_new_exp(_tmp1E6,loc);});}
# 482
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp60=_cycalloc(sizeof(*_tmp60));*_tmp60=*e;_tmp60;});}
# 485
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp1E7=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=0U,_tmp61->f1=c;_tmp61;});Cyc_Absyn_new_exp(_tmp1E7,loc);});}
# 488
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 492
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp1E8=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp1E8,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 500
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 507
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp1E9=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp1E9,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp1EA=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp1EA,loc);});}
# 514
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp1EB=f(s);Cyc_Absyn_const_exp(_tmp1EB,loc);});}
# 517
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 521
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp1ED=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=1U,({void*_tmp1EC=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=0U,_tmp62->f1=q;_tmp62;});_tmp63->f1=_tmp1EC;});_tmp63;});Cyc_Absyn_new_exp(_tmp1ED,loc);});}
# 524
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp1EE=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=1U,_tmp64->f1=b;_tmp64;});Cyc_Absyn_new_exp(_tmp1EE,loc);});}
# 528
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 531
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
return({void*_tmp1EF=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=2U,_tmp65->f1=s;_tmp65;});Cyc_Absyn_new_exp(_tmp1EF,loc);});}
# 534
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1F0=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=3U,_tmp66->f1=p,_tmp66->f2=es;_tmp66;});Cyc_Absyn_new_exp(_tmp1F0,loc);});}
# 537
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1F2=p;struct Cyc_List_List*_tmp1F1=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=e,_tmp67->tl=0;_tmp67;});Cyc_Absyn_primop_exp(_tmp1F2,_tmp1F1,loc);});}
# 540
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1F5=p;struct Cyc_List_List*_tmp1F4=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=e1,({struct Cyc_List_List*_tmp1F3=({struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=e2,_tmp68->tl=0;_tmp68;});_tmp69->tl=_tmp1F3;});_tmp69;});Cyc_Absyn_primop_exp(_tmp1F5,_tmp1F4,loc);});}
# 543
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1F6=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=35U,_tmp6A->f1=e1,_tmp6A->f2=e2;_tmp6A;});Cyc_Absyn_new_exp(_tmp1F6,loc);});}
# 546
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 556
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1F7=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=4U,_tmp6B->f1=e1,_tmp6B->f2=popt,_tmp6B->f3=e2;_tmp6B;});Cyc_Absyn_new_exp(_tmp1F7,loc);});}
# 559
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 562
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp1F8=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=5U,_tmp6C->f1=e,_tmp6C->f2=i;_tmp6C;});Cyc_Absyn_new_exp(_tmp1F8,loc);});}
# 565
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp1F9=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=6U,_tmp6D->f1=e1,_tmp6D->f2=e2,_tmp6D->f3=e3;_tmp6D;});Cyc_Absyn_new_exp(_tmp1F9,loc);});}
# 568
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1FA=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=7U,_tmp6E->f1=e1,_tmp6E->f2=e2;_tmp6E;});Cyc_Absyn_new_exp(_tmp1FA,loc);});}
# 571
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1FB=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=8U,_tmp6F->f1=e1,_tmp6F->f2=e2;_tmp6F;});Cyc_Absyn_new_exp(_tmp1FB,loc);});}
# 574
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1FC=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=9U,_tmp70->f1=e1,_tmp70->f2=e2;_tmp70;});Cyc_Absyn_new_exp(_tmp1FC,loc);});}
# 577
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1FD=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=10U,_tmp71->f1=e,_tmp71->f2=es,_tmp71->f3=0,_tmp71->f4=0;_tmp71;});Cyc_Absyn_new_exp(_tmp1FD,loc);});}
# 580
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1FE=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=10U,_tmp72->f1=e,_tmp72->f2=es,_tmp72->f3=0,_tmp72->f4=1;_tmp72;});Cyc_Absyn_new_exp(_tmp1FE,loc);});}
# 583
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1FF=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=12U,_tmp73->f1=e;_tmp73;});Cyc_Absyn_new_exp(_tmp1FF,loc);});}
# 586
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp200=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=13U,_tmp74->f1=e,_tmp74->f2=ts;_tmp74;});Cyc_Absyn_new_exp(_tmp200,loc);});}
# 589
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp201=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=14U,_tmp75->f1=t,_tmp75->f2=e,_tmp75->f3=user_cast,_tmp75->f4=c;_tmp75;});Cyc_Absyn_new_exp(_tmp201,loc);});}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp202=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=11U,_tmp76->f1=e,_tmp76->f2=0;_tmp76;});Cyc_Absyn_new_exp(_tmp202,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp203=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=11U,_tmp77->f1=e,_tmp77->f2=1;_tmp77;});Cyc_Absyn_new_exp(_tmp203,loc);});}
# 598
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp204=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=15U,_tmp78->f1=e;_tmp78;});Cyc_Absyn_new_exp(_tmp204,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp205=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=17U,_tmp79->f1=t;_tmp79;});Cyc_Absyn_new_exp(_tmp205,loc);});}
# 602
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp206=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=18U,_tmp7A->f1=e;_tmp7A;});Cyc_Absyn_new_exp(_tmp206,loc);});}
# 605
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp207=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=19U,_tmp7B->f1=t,_tmp7B->f2=ofs;_tmp7B;});Cyc_Absyn_new_exp(_tmp207,loc);});}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp208=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=20U,_tmp7C->f1=e;_tmp7C;});Cyc_Absyn_new_exp(_tmp208,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp209=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});Cyc_Absyn_new_exp(_tmp209,loc);});}
# 612
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp20A=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});Cyc_Absyn_new_exp(_tmp20A,loc);});}
# 615
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp20B=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=23U,_tmp7F->f1=e1,_tmp7F->f2=e2;_tmp7F;});Cyc_Absyn_new_exp(_tmp20B,loc);});}
# 618
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp20C=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=24U,_tmp80->f1=es;_tmp80;});Cyc_Absyn_new_exp(_tmp20C,loc);});}
# 621
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp20D=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=37U,_tmp81->f1=s;_tmp81;});Cyc_Absyn_new_exp(_tmp20D,loc);});}
# 624
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp20E=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=39U,_tmp82->f1=t;_tmp82;});Cyc_Absyn_new_exp(_tmp20E,loc);});}
# 628
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 631
return({void*_tmp20F=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=40U,_tmp83->f1=volatile_kw,_tmp83->f2=tmpl,_tmp83->f3=outs,_tmp83->f4=ins,_tmp83->f5=clobs;_tmp83;});Cyc_Absyn_new_exp(_tmp20F,loc);});}
# 633
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp210=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=41U,_tmp84->f1=e;_tmp84;});Cyc_Absyn_new_exp(_tmp210,loc);});}
# 636
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp211=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=42U,_tmp85->f1=e;_tmp85;});Cyc_Absyn_new_exp(_tmp211,loc);});}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 640
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp86=0;struct Cyc_List_List*dles=_tmp86;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp88=_cycalloc(sizeof(*_tmp88));({struct _tuple12*_tmp212=({struct _tuple12*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->f1=0,_tmp87->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp87;});_tmp88->hd=_tmp212;}),_tmp88->tl=dles;_tmp88;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return({void*_tmp213=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=26U,_tmp89->f1=dles;_tmp89;});Cyc_Absyn_new_exp(_tmp213,loc);});}
# 647
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 650
return({void*_tmp214=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=36U,_tmp8A->f1=n,_tmp8A->f2=dles;_tmp8A;});Cyc_Absyn_new_exp(_tmp214,loc);});}
# 653
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->r=s,_tmp8B->loc=loc,_tmp8B->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp8B;});}
# 656
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp215=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=1U,_tmp8C->f1=e;_tmp8C;});Cyc_Absyn_new_stmt(_tmp215,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp216=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=3U,_tmp8D->f1=e;_tmp8D;});Cyc_Absyn_new_stmt(_tmp216,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp218=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp217=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp218,_tmp217,loc);});}}}struct _tuple13{void*f1;void*f2;};
# 669
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple13 _tmp8E=({struct _tuple13 _tmp19B;_tmp19B.f1=s1->r,_tmp19B.f2=s2->r;_tmp19B;});struct _tuple13 _stmttmp3=_tmp8E;struct _tuple13 _tmp8F=_stmttmp3;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp8F.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp8F.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp219=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=2U,_tmp90->f1=s1,_tmp90->f2=s2;_tmp90;});Cyc_Absyn_new_stmt(_tmp219,loc);});}}_LL0:;}
# 676
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp21A=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=4U,_tmp91->f1=e,_tmp91->f2=s1,_tmp91->f3=s2;_tmp91;});Cyc_Absyn_new_stmt(_tmp21A,loc);});}
# 679
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp21C=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=5U,(_tmp92->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp21B=Cyc_Absyn_skip_stmt(e->loc);(_tmp92->f1).f2=_tmp21B;}),_tmp92->f2=s;_tmp92;});Cyc_Absyn_new_stmt(_tmp21C,loc);});}
# 682
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp21F=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=9U,_tmp93->f1=e1,(_tmp93->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp21E=Cyc_Absyn_skip_stmt(e3->loc);(_tmp93->f2).f2=_tmp21E;}),
(_tmp93->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp21D=Cyc_Absyn_skip_stmt(e3->loc);(_tmp93->f3).f2=_tmp21D;}),_tmp93->f4=s;_tmp93;});
# 683
Cyc_Absyn_new_stmt(_tmp21F,loc);});}
# 687
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp221=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=14U,_tmp94->f1=s,(_tmp94->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp220=Cyc_Absyn_skip_stmt(e->loc);(_tmp94->f2).f2=_tmp220;});_tmp94;});Cyc_Absyn_new_stmt(_tmp221,loc);});}
# 690
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp222=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=10U,_tmp95->f1=e,_tmp95->f2=scs,_tmp95->f3=0;_tmp95;});Cyc_Absyn_new_stmt(_tmp222,loc);});}
# 693
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp223=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=15U,_tmp96->f1=s,_tmp96->f2=scs,_tmp96->f3=0;_tmp96;});Cyc_Absyn_new_stmt(_tmp223,loc);});}
# 696
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp224=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=11U,_tmp97->f1=el,_tmp97->f2=0;_tmp97;});Cyc_Absyn_new_stmt(_tmp224,loc);});}
# 699
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
return({void*_tmp225=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=8U,_tmp98->f1=lab;_tmp98;});Cyc_Absyn_new_stmt(_tmp225,loc);});}
# 702
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp226=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=13U,_tmp99->f1=v,_tmp99->f2=s;_tmp99;});Cyc_Absyn_new_stmt(_tmp226,loc);});}
# 705
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp227=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=12U,_tmp9A->f1=d,_tmp9A->f2=s;_tmp9A;});Cyc_Absyn_new_stmt(_tmp227,loc);});}
# 708
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp229=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp228=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp9C->f1=_tmp228;});_tmp9C;});Cyc_Absyn_new_decl(_tmp229,loc);});
return({void*_tmp22A=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=12U,_tmp9B->f1=d,_tmp9B->f2=s;_tmp9B;});Cyc_Absyn_new_stmt(_tmp22A,loc);});}
# 712
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp22B=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp22B,loc);});}
# 716
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->r=p,_tmp9D->topt=0,_tmp9D->loc=s;_tmp9D;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp22C=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=17U,_tmp9E->f1=e;_tmp9E;});Cyc_Absyn_new_pat(_tmp22C,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 722
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->r=r,_tmp9F->loc=loc;_tmp9F;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp22D=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=2U,_tmpA0->f1=p,_tmpA0->f2=0,_tmpA0->f3=e,_tmpA0->f4=0;_tmpA0;});Cyc_Absyn_new_decl(_tmp22D,loc);});}
# 726
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp22E=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=3U,_tmpA1->f1=vds;_tmpA1;});Cyc_Absyn_new_decl(_tmp22E,loc);});}
# 729
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp22F=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=4U,_tmpA2->f1=tv,_tmpA2->f2=vd,_tmpA2->f3=open_exp;_tmpA2;});Cyc_Absyn_new_decl(_tmp22F,loc);});}
# 732
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 734
return({void*_tmp232=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=2U,({struct Cyc_Absyn_Pat*_tmp231=({void*_tmp230=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=2U,_tmpA3->f1=tv,_tmpA3->f2=vd;_tmpA3;});Cyc_Absyn_new_pat(_tmp230,loc);});_tmpA4->f1=_tmp231;}),_tmpA4->f2=0,_tmpA4->f3=e,_tmpA4->f4=0;_tmpA4;});Cyc_Absyn_new_decl(_tmp232,loc);});}
# 737
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->sc=Cyc_Absyn_Public,_tmpA5->name=x,_tmpA5->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp233=Cyc_Absyn_empty_tqual(0U);_tmpA5->tq=_tmp233;}),_tmpA5->type=t,_tmpA5->initializer=init,_tmpA5->rgn=0,_tmpA5->attributes=0,_tmpA5->escapes=0;_tmpA5;});}
# 743
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->sc=Cyc_Absyn_Static,_tmpA6->name=x,_tmpA6->varloc=0U,({struct Cyc_Absyn_Tqual _tmp234=Cyc_Absyn_empty_tqual(0U);_tmpA6->tq=_tmp234;}),_tmpA6->type=t,_tmpA6->initializer=init,_tmpA6->rgn=0,_tmpA6->attributes=0,_tmpA6->escapes=0;_tmpA6;});}
# 748
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 752
return({struct Cyc_Absyn_AggrdeclImpl*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->exist_vars=exists,_tmpA7->rgn_po=po,_tmpA7->fields=fs,_tmpA7->tagged=tagged;_tmpA7;});}
# 755
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 758
return({void*_tmp236=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp235=({struct Cyc_Absyn_Aggrdecl*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->kind=k,_tmpA8->sc=s,_tmpA8->name=n,_tmpA8->tvs=ts,_tmpA8->impl=i,_tmpA8->attributes=atts,_tmpA8->expected_mem_kind=0;_tmpA8;});_tmpA9->f1=_tmp235;});_tmpA9;});Cyc_Absyn_new_decl(_tmp236,loc);});}
# 763
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 766
return({struct Cyc_Absyn_TypeDecl*_tmpAC=_cycalloc(sizeof(*_tmpAC));({void*_tmp238=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp237=({struct Cyc_Absyn_Aggrdecl*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->kind=k,_tmpAA->sc=s,_tmpAA->name=n,_tmpAA->tvs=ts,_tmpAA->impl=i,_tmpAA->attributes=atts,_tmpAA->expected_mem_kind=0;_tmpAA;});_tmpAB->f1=_tmp237;});_tmpAB;});_tmpAC->r=_tmp238;}),_tmpAC->loc=loc;_tmpAC;});}
# 772
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 775
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 777
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 780
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 782
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 785
return({void*_tmp23A=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp239=({struct Cyc_Absyn_Datatypedecl*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->sc=s,_tmpAD->name=n,_tmpAD->tvs=ts,_tmpAD->fields=fs,_tmpAD->is_extensible=is_extensible;_tmpAD;});_tmpAE->f1=_tmp239;});_tmpAE;});Cyc_Absyn_new_decl(_tmp23A,loc);});}
# 788
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 791
return({struct Cyc_Absyn_TypeDecl*_tmpB1=_cycalloc(sizeof(*_tmpB1));({void*_tmp23C=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp23B=({struct Cyc_Absyn_Datatypedecl*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->sc=s,_tmpAF->name=n,_tmpAF->tvs=ts,_tmpAF->fields=fs,_tmpAF->is_extensible=is_extensible;_tmpAF;});_tmpB0->f1=_tmp23B;});_tmpB0;});_tmpB1->r=_tmp23C;}),_tmpB1->loc=loc;_tmpB1;});}
# 803 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 809
{struct Cyc_List_List*_tmpB2=args;struct Cyc_List_List*args2=_tmpB2;for(0;args2 != 0;args2=args2->tl){
({void*_tmp23D=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f3,1);(*((struct _tuple8*)args2->hd)).f3=_tmp23D;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=5U,(_tmpB3->f1).tvars=tvs,(_tmpB3->f1).ret_tqual=ret_tqual,({
# 813
void*_tmp23E=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpB3->f1).ret_type=_tmp23E;}),(_tmpB3->f1).effect=eff_type,(_tmpB3->f1).args=args,(_tmpB3->f1).c_varargs=c_varargs,(_tmpB3->f1).cyc_varargs=cyc_varargs,(_tmpB3->f1).rgn_po=rgn_po,(_tmpB3->f1).attributes=atts,(_tmpB3->f1).requires_clause=req,(_tmpB3->f1).requires_relns=0,(_tmpB3->f1).ensures_clause=ens,(_tmpB3->f1).ensures_relns=0;_tmpB3;});}
# 827
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpB4=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmpB4;void*_tmpB5=_stmttmp4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB5)->tag == 5U){_LL1: _LL2:
 return({void*_tmp241=t;void*_tmp240=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=& Cyc_Tcutil_rk;_tmpB6;}),0): Cyc_Absyn_heap_rgn_type;
# 829
struct Cyc_Absyn_Tqual _tmp23F=
# 833
Cyc_Absyn_empty_tqual(0U);
# 829
Cyc_Absyn_at_type(_tmp241,_tmp240,_tmp23F,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 834
 return t;}_LL0:;}
# 849 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpB7=e->r;void*_stmttmp5=_tmpB7;void*_tmpB8=_stmttmp5;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Vardecl*_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBD;switch(*((int*)_tmpB8)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB8)->f1)){case 2U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpBD=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB8)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpBD;
_tmpBC=vd;goto _LL6;}case 4U: _LL5: _tmpBC=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB8)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpBC;
# 854
void*_tmpBE=Cyc_Tcutil_compress(vd->type);void*_stmttmp6=_tmpBE;void*_tmpBF=_stmttmp6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBF)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 858
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpBB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB8)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpBB;
return Cyc_Absyn_is_lvalue(e1);}case 13U: _LL11: _tmpBA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB8)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpBA;
return Cyc_Absyn_is_lvalue(e1);}case 12U: _LL13: _tmpB9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB8)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpB9;
return Cyc_Absyn_is_lvalue(e1);}default: _LL15: _LL16:
 return 0;}_LL0:;}
# 869
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*_tmpC0=fields;struct Cyc_List_List*fs=_tmpC0;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 875
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 879
struct _tuple14*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 885
if(ts == 0)
return 0;
return(struct _tuple14*)ts->hd;}
# 890
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_tmpC1=decl->r;void*_stmttmp7=_tmpC1;void*_tmpC2=_stmttmp7;struct Cyc_Absyn_Fndecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC4;struct Cyc_Absyn_Typedefdecl*_tmpC5;struct Cyc_Absyn_Enumdecl*_tmpC6;struct Cyc_Absyn_Aggrdecl*_tmpC7;switch(*((int*)_tmpC2)){case 5U: _LL1: _tmpC7=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC2)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*x=_tmpC7;
return(*x->name).f2;}case 7U: _LL3: _tmpC6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC2)->f1;_LL4: {struct Cyc_Absyn_Enumdecl*x=_tmpC6;
return(*x->name).f2;}case 8U: _LL5: _tmpC5=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC2)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*x=_tmpC5;
return(*x->name).f2;}case 0U: _LL7: _tmpC4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmpC4;
return(*x->name).f2;}case 1U: _LL9: _tmpC3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC2)->f1;_LLA: {struct Cyc_Absyn_Fndecl*x=_tmpC3;
return(*x->name).f2;}case 13U: _LLB: _LLC:
 goto _LLE;case 14U: _LLD: _LLE:
 goto _LL10;case 15U: _LLF: _LL10:
 goto _LL12;case 16U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14:
 goto _LL16;case 6U: _LL15: _LL16:
 goto _LL18;case 3U: _LL17: _LL18:
 goto _LL1A;case 9U: _LL19: _LL1A:
 goto _LL1C;case 10U: _LL1B: _LL1C:
 goto _LL1E;case 11U: _LL1D: _LL1E:
 goto _LL20;case 12U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return 0;}_LL0:;}
# 913
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
struct _fat_ptr*dname;
for(0;decls != 0;decls=decls->tl){
dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 920
return 0;}
# 923
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={1U};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2U};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3U};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4U};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5U};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7U};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9U};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10U};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11U};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12U};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={13U};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={14U};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={15U};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={16U};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={17U};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={18U};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={23U};
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val={26U};
# 942
struct _fat_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmpC8=a;struct _fat_ptr _tmpC9;struct _fat_ptr _tmpCA;int _tmpCB;int _tmpCC;int _tmpCD;int _tmpCF;int _tmpCE;int _tmpD1;int _tmpD0;struct _fat_ptr _tmpD2;struct Cyc_Absyn_Exp*_tmpD3;int _tmpD4;switch(*((int*)_tmpC8)){case 0U: _LL1: _tmpD4=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL2: {int i=_tmpD4;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD7=({struct Cyc_Int_pa_PrintArg_struct _tmp19C;_tmp19C.tag=1U,_tmp19C.f1=(unsigned long)i;_tmp19C;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({struct _fat_ptr _tmp242=({const char*_tmpD6="regparm(%d)";_tag_fat(_tmpD6,sizeof(char),12U);});Cyc_aprintf(_tmp242,_tag_fat(_tmpD5,sizeof(void*),1U));});});}case 1U: _LL3: _LL4:
 return({const char*_tmpD8="stdcall";_tag_fat(_tmpD8,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpD9="cdecl";_tag_fat(_tmpD9,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpDA="fastcall";_tag_fat(_tmpDA,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpDB="noreturn";_tag_fat(_tmpDB,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpDC="const";_tag_fat(_tmpDC,sizeof(char),6U);});case 6U: _LLD: _tmpD3=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmpD3;
# 951
if(e == 0)return({const char*_tmpDD="aligned";_tag_fat(_tmpDD,sizeof(char),8U);});else{
# 953
enum Cyc_Cyclone_C_Compilers _tmpDE=Cyc_Cyclone_c_compiler;switch(_tmpDE){case Cyc_Cyclone_Gcc_c: _LL3A: _LL3B:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE1=({struct Cyc_String_pa_PrintArg_struct _tmp19D;_tmp19D.tag=0U,({struct _fat_ptr _tmp243=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp19D.f1=_tmp243;});_tmp19D;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _fat_ptr _tmp244=({const char*_tmpE0="aligned(%s)";_tag_fat(_tmpE0,sizeof(char),12U);});Cyc_aprintf(_tmp244,_tag_fat(_tmpDF,sizeof(void*),1U));});});case Cyc_Cyclone_Vc_c: _LL3C: _LL3D:
 goto _LL3F;default: _LL3E: _LL3F:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0U,({struct _fat_ptr _tmp245=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp19E.f1=_tmp245;});_tmp19E;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({struct _fat_ptr _tmp246=({const char*_tmpE3="align(%s)";_tag_fat(_tmpE3,sizeof(char),10U);});Cyc_aprintf(_tmp246,_tag_fat(_tmpE2,sizeof(void*),1U));});});}_LL39:;}}case 7U: _LLF: _LL10:
# 958
 return({const char*_tmpE5="packed";_tag_fat(_tmpE5,sizeof(char),7U);});case 8U: _LL11: _tmpD2=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL12: {struct _fat_ptr s=_tmpD2;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE8=({struct Cyc_String_pa_PrintArg_struct _tmp19F;_tmp19F.tag=0U,_tmp19F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp19F;});void*_tmpE6[1U];_tmpE6[0]=& _tmpE8;({struct _fat_ptr _tmp247=({const char*_tmpE7="section(\"%s\")";_tag_fat(_tmpE7,sizeof(char),14U);});Cyc_aprintf(_tmp247,_tag_fat(_tmpE6,sizeof(void*),1U));});});}case 9U: _LL13: _LL14:
 return({const char*_tmpE9="nocommon";_tag_fat(_tmpE9,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpEA="shared";_tag_fat(_tmpEA,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpEB="unused";_tag_fat(_tmpEB,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpEC="weak";_tag_fat(_tmpEC,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpED="dllimport";_tag_fat(_tmpED,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpEE="dllexport";_tag_fat(_tmpEE,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpEF="no_instrument_function";_tag_fat(_tmpEF,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpF0="constructor";_tag_fat(_tmpF0,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpF1="destructor";_tag_fat(_tmpF1,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpF2="no_check_memory_usage";_tag_fat(_tmpF2,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC8)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpD0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC8)->f2;_tmpD1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC8)->f3;_LL28: {int n=_tmpD0;int m=_tmpD1;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF5=({struct Cyc_Int_pa_PrintArg_struct _tmp1A1;_tmp1A1.tag=1U,_tmp1A1.f1=(unsigned)n;_tmp1A1;});struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp1A0;_tmp1A0.tag=1U,_tmp1A0.f1=(unsigned)m;_tmp1A0;});void*_tmpF3[2U];_tmpF3[0]=& _tmpF5,_tmpF3[1]=& _tmpF6;({struct _fat_ptr _tmp248=({const char*_tmpF4="format(printf,%u,%u)";_tag_fat(_tmpF4,sizeof(char),21U);});Cyc_aprintf(_tmp248,_tag_fat(_tmpF3,sizeof(void*),2U));});});}}else{_LL29: _tmpCE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC8)->f2;_tmpCF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC8)->f3;_LL2A: {int n=_tmpCE;int m=_tmpCF;
# 972
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF9=({struct Cyc_Int_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=1U,_tmp1A3.f1=(unsigned)n;_tmp1A3;});struct Cyc_Int_pa_PrintArg_struct _tmpFA=({struct Cyc_Int_pa_PrintArg_struct _tmp1A2;_tmp1A2.tag=1U,_tmp1A2.f1=(unsigned)m;_tmp1A2;});void*_tmpF7[2U];_tmpF7[0]=& _tmpF9,_tmpF7[1]=& _tmpFA;({struct _fat_ptr _tmp249=({const char*_tmpF8="format(scanf,%u,%u)";_tag_fat(_tmpF8,sizeof(char),20U);});Cyc_aprintf(_tmp249,_tag_fat(_tmpF7,sizeof(void*),2U));});});}}case 20U: _LL2B: _tmpCD=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL2C: {int n=_tmpCD;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFD=({struct Cyc_Int_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=1U,_tmp1A4.f1=(unsigned long)n;_tmp1A4;});void*_tmpFB[1U];_tmpFB[0]=& _tmpFD;({struct _fat_ptr _tmp24A=({const char*_tmpFC="initializes(%d)";_tag_fat(_tmpFC,sizeof(char),16U);});Cyc_aprintf(_tmp24A,_tag_fat(_tmpFB,sizeof(void*),1U));});});}case 21U: _LL2D: _tmpCC=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL2E: {int n=_tmpCC;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp100=({struct Cyc_Int_pa_PrintArg_struct _tmp1A5;_tmp1A5.tag=1U,_tmp1A5.f1=(unsigned long)n;_tmp1A5;});void*_tmpFE[1U];_tmpFE[0]=& _tmp100;({struct _fat_ptr _tmp24B=({const char*_tmpFF="noliveunique(%d)";_tag_fat(_tmpFF,sizeof(char),17U);});Cyc_aprintf(_tmp24B,_tag_fat(_tmpFE,sizeof(void*),1U));});});}case 22U: _LL2F: _tmpCB=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL30: {int n=_tmpCB;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp103=({struct Cyc_Int_pa_PrintArg_struct _tmp1A6;_tmp1A6.tag=1U,_tmp1A6.f1=(unsigned long)n;_tmp1A6;});void*_tmp101[1U];_tmp101[0]=& _tmp103;({struct _fat_ptr _tmp24C=({const char*_tmp102="consume(%d)";_tag_fat(_tmp102,sizeof(char),12U);});Cyc_aprintf(_tmp24C,_tag_fat(_tmp101,sizeof(void*),1U));});});}case 23U: _LL31: _LL32:
 return({const char*_tmp104="pure";_tag_fat(_tmp104,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmp105="always_inline";_tag_fat(_tmp105,sizeof(char),14U);});case 24U: _LL35: _tmpCA=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL36: {struct _fat_ptr s=_tmpCA;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp108=({struct Cyc_String_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=0U,_tmp1A7.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp1A7;});void*_tmp106[1U];_tmp106[0]=& _tmp108;({struct _fat_ptr _tmp24D=({const char*_tmp107="__mode__(\"%s\")";_tag_fat(_tmp107,sizeof(char),15U);});Cyc_aprintf(_tmp24D,_tag_fat(_tmp106,sizeof(void*),1U));});});}default: _LL37: _tmpC9=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpC8)->f1;_LL38: {struct _fat_ptr s=_tmpC9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10B=({struct Cyc_String_pa_PrintArg_struct _tmp1A8;_tmp1A8.tag=0U,_tmp1A8.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp1A8;});void*_tmp109[1U];_tmp109[0]=& _tmp10B;({struct _fat_ptr _tmp24E=({const char*_tmp10A="alias(\"%s\")";_tag_fat(_tmp10A,sizeof(char),12U);});Cyc_aprintf(_tmp24E,_tag_fat(_tmp109,sizeof(void*),1U));});});}}_LL0:;}
# 983
static int Cyc_Absyn_attribute_case_number(void*att){
void*_tmp10C=att;switch(*((int*)_tmp10C)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 9U: _LL13: _LL14:
 return 9;case 10U: _LL15: _LL16:
 return 10;case 11U: _LL17: _LL18:
 return 11;case 12U: _LL19: _LL1A:
 return 12;case 13U: _LL1B: _LL1C:
 return 13;case 14U: _LL1D: _LL1E:
 return 14;case 15U: _LL1F: _LL20:
 return 15;case 16U: _LL21: _LL22:
 return 16;case 17U: _LL23: _LL24:
 return 17;case 18U: _LL25: _LL26:
 return 18;case 19U: _LL27: _LL28:
 return 19;case 20U: _LL29: _LL2A:
 return 20;default: _LL2B: _LL2C:
 return 21;}_LL0:;}
# 1009
int Cyc_Absyn_attribute_cmp(void*att1,void*att2){
struct _tuple13 _tmp10D=({struct _tuple13 _tmp1A9;_tmp1A9.f1=att1,_tmp1A9.f2=att2;_tmp1A9;});struct _tuple13 _stmttmp8=_tmp10D;struct _tuple13 _tmp10E=_stmttmp8;int _tmp114;int _tmp113;enum Cyc_Absyn_Format_Type _tmp112;int _tmp111;int _tmp110;enum Cyc_Absyn_Format_Type _tmp10F;struct _fat_ptr _tmp116;struct _fat_ptr _tmp115;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;int _tmp11A;int _tmp119;int _tmp11C;int _tmp11B;switch(*((int*)_tmp10E.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10E.f2)->tag == 0U){_LL1: _tmp11B=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10E.f1)->f1;_tmp11C=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10E.f2)->f1;_LL2: {int i1=_tmp11B;int i2=_tmp11C;
_tmp119=i1;_tmp11A=i2;goto _LL4;}}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10E.f2)->tag == 20U){_LL3: _tmp119=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10E.f1)->f1;_tmp11A=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10E.f2)->f1;_LL4: {int i1=_tmp119;int i2=_tmp11A;
# 1013
return Cyc_Core_intcmp(i1,i2);}}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10E.f2)->tag == 6U){_LL5: _tmp117=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10E.f1)->f1;_tmp118=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10E.f2)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp117;struct Cyc_Absyn_Exp*e2=_tmp118;
# 1015
if(e1 == e2)return 0;
if(e1 == 0)return - 1;
if(e2 == 0)return 1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10E.f2)->tag == 8U){_LL7: _tmp115=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10E.f1)->f1;_tmp116=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10E.f2)->f1;_LL8: {struct _fat_ptr s1=_tmp115;struct _fat_ptr s2=_tmp116;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f2)->tag == 19U){_LL9: _tmp10F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f1)->f1;_tmp110=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f1)->f2;_tmp111=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f1)->f3;_tmp112=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f2)->f1;_tmp113=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f2)->f2;_tmp114=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10E.f2)->f3;_LLA: {enum Cyc_Absyn_Format_Type ft1=_tmp10F;int i1=_tmp110;int j1=_tmp111;enum Cyc_Absyn_Format_Type ft2=_tmp112;int i2=_tmp113;int j2=_tmp114;
# 1021
int _tmp11D=Cyc_Core_intcmp((int)((unsigned)ft1),(int)((unsigned)ft2));int ftc=_tmp11D;
if(ftc != 0)return ftc;{
int _tmp11E=Cyc_Core_intcmp(i1,i2);int ic=_tmp11E;
if(ic != 0)return ic;
return Cyc_Core_intcmp(j1,j2);}}}else{goto _LLB;}default: _LLB: _LLC:
# 1027
 return({int _tmp24F=Cyc_Absyn_attribute_case_number(att1);Cyc_Core_intcmp(_tmp24F,Cyc_Absyn_attribute_case_number(att2));});}_LL0:;}
# 1030
int Cyc_Absyn_equal_att(void*a1,void*a2){
return Cyc_Absyn_attribute_cmp(a1,a2)== 0;}
# 1035
int Cyc_Absyn_fntype_att(void*a){
void*_tmp11F=a;int _tmp120;switch(*((int*)_tmp11F)){case 0U: _LL1: _tmp120=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp11F)->f1;_LL2: {int i=_tmp120;
goto _LL4;}case 3U: _LL3: _LL4:
 goto _LL6;case 1U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 23U: _LLB: _LLC:
 goto _LLE;case 19U: _LLD: _LLE:
 goto _LL10;case 5U: _LLF: _LL10:
 goto _LL12;case 21U: _LL11: _LL12:
 goto _LL14;case 20U: _LL13: _LL14:
 goto _LL16;case 22U: _LL15: _LL16:
 return 1;default: _LL17: _LL18:
 return 0;}_LL0:;}static char _tmp127[3U]="f0";
# 1052
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 1054
static struct _fat_ptr f0={_tmp127,_tmp127,_tmp127 + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)((struct _fat_ptr**)field_names_v),(void*)((struct _fat_ptr**)field_names_v),(void*)((struct _fat_ptr**)field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmp126=(unsigned)(i + 1);struct _fat_ptr**_tmp125=_cycalloc(_check_times(_tmp126,sizeof(struct _fat_ptr*)));({{unsigned _tmp1AA=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp1AA;++ j){
# 1061
j < fsz?_tmp125[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_tmp252=({struct _fat_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _fat_ptr _tmp251=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp1AB;_tmp1AB.tag=1U,_tmp1AB.f1=(unsigned long)((int)j);_tmp1AB;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _fat_ptr _tmp250=({const char*_tmp122="f%d";_tag_fat(_tmp122,sizeof(char),4U);});Cyc_aprintf(_tmp250,_tag_fat(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp251;});_tmp124;});_tmp125[j]=_tmp252;});}}0;});_tag_fat(_tmp125,sizeof(struct _fat_ptr*),_tmp126);});
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple15{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1065
struct _tuple15 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmp128=info;struct _tuple0*_tmp12A;enum Cyc_Absyn_AggrKind _tmp129;struct _tuple0*_tmp12C;enum Cyc_Absyn_AggrKind _tmp12B;if((_tmp128.UnknownAggr).tag == 1){_LL1: _tmp12B=((_tmp128.UnknownAggr).val).f1;_tmp12C=((_tmp128.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind ak=_tmp12B;struct _tuple0*n=_tmp12C;
return({struct _tuple15 _tmp1AC;_tmp1AC.f1=ak,_tmp1AC.f2=n;_tmp1AC;});}}else{_LL3: _tmp129=(*(_tmp128.KnownAggr).val)->kind;_tmp12A=(*(_tmp128.KnownAggr).val)->name;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp129;struct _tuple0*n=_tmp12A;
return({struct _tuple15 _tmp1AD;_tmp1AD.f1=k,_tmp1AD.f2=n;_tmp1AD;});}}_LL0:;}
# 1071
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmp12D=info;struct Cyc_Absyn_Aggrdecl*_tmp12E;if((_tmp12D.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmp12F=0U;({struct _fat_ptr _tmp253=({const char*_tmp130="unchecked aggrdecl";_tag_fat(_tmp130,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp253,_tag_fat(_tmp12F,sizeof(void*),0U));});});}else{_LL3: _tmp12E=*(_tmp12D.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp12E;
return ad;}}_LL0:;}
# 1077
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp131=Cyc_Tcutil_compress(t);void*_stmttmp9=_tmp131;void*_tmp132=_stmttmp9;union Cyc_Absyn_AggrInfo _tmp133;struct Cyc_List_List*_tmp134;switch(*((int*)_tmp132)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp132)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp134=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp132)->f2;_LL2: {struct Cyc_List_List*fs=_tmp134;
# 1080
if(fs == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause == 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp132)->f1)->tag == 20U){_LL3: _tmp133=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp132)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmp133;
# 1083
union Cyc_Absyn_AggrInfo _tmp135=info;int _tmp137;enum Cyc_Absyn_AggrKind _tmp136;enum Cyc_Absyn_AggrKind _tmp138;struct Cyc_Absyn_Aggrdecl*_tmp139;if((_tmp135.KnownAggr).tag == 2){_LL8: _tmp139=*(_tmp135.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmp139;
# 1085
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp13A=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmp13A;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmp13B=impl->fields;struct Cyc_List_List*fields=_tmp13B;
if(fields == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause == 0;}}}}else{if(((_tmp135.UnknownAggr).val).f3 == 0){_LLA: _tmp138=((_tmp135.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmp138;
return(int)k == (int)Cyc_Absyn_UnionA;}}else{_LLC: _tmp136=((_tmp135.UnknownAggr).val).f1;_tmp137=(int)(((_tmp135.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmp136;int b=_tmp137;
return(int)k == (int)1U && !b;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1094
 return 0;}_LL0:;}
# 1097
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp13C=Cyc_Tcutil_compress(t);void*_stmttmpA=_tmp13C;void*_tmp13D=_stmttmpA;union Cyc_Absyn_AggrInfo _tmp13E;struct Cyc_List_List*_tmp13F;switch(*((int*)_tmp13D)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13D)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp13F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13D)->f2;_LL2: {struct Cyc_List_List*fs=_tmp13F;
# 1100
if(fs == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause != 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->tag == 20U){_LL3: _tmp13E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmp13E;
# 1103
union Cyc_Absyn_AggrInfo _tmp140=info;int _tmp142;enum Cyc_Absyn_AggrKind _tmp141;enum Cyc_Absyn_AggrKind _tmp143;struct Cyc_Absyn_Aggrdecl*_tmp144;if((_tmp140.KnownAggr).tag == 2){_LL8: _tmp144=*(_tmp140.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmp144;
# 1105
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp145=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmp145;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmp146=impl->fields;struct Cyc_List_List*fields=_tmp146;
if(fields == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause != 0;}}}}else{if(((_tmp140.UnknownAggr).val).f3 == 0){_LLA: _tmp143=((_tmp140.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmp143;
return 0;}}else{_LLC: _tmp141=((_tmp140.UnknownAggr).val).f1;_tmp142=(int)(((_tmp140.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmp141;int b=_tmp142;
return 0;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1114
 return 0;}_LL0:;}
# 1118
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp147=b;struct Cyc_Absyn_Fndecl*_tmp148;struct Cyc_Absyn_Vardecl*_tmp149;struct Cyc_Absyn_Vardecl*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp14B;struct Cyc_Absyn_Vardecl*_tmp14C;struct _tuple0*_tmp14D;switch(*((int*)_tmp147)){case 0U: _LL1: _tmp14D=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp147)->f1;_LL2: {struct _tuple0*qv=_tmp14D;
return qv;}case 1U: _LL3: _tmp14C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp147)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp14C;
_tmp14B=vd;goto _LL6;}case 3U: _LL5: _tmp14B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp147)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp14B;
_tmp14A=vd;goto _LL8;}case 4U: _LL7: _tmp14A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp147)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp14A;
_tmp149=vd;goto _LLA;}case 5U: _LL9: _tmp149=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp147)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp149;
return vd->name;}default: _LLB: _tmp148=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp147)->f1;_LLC: {struct Cyc_Absyn_Fndecl*fd=_tmp148;
return fd->name;}}_LL0:;}
# 1129
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({void*_tmp14E=0U;({struct _fat_ptr _tmp254=({const char*_tmp14F="designator list not of length 1";_tag_fat(_tmp14F,sizeof(char),32U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp254,_tag_fat(_tmp14E,sizeof(void*),0U));});});{
void*_tmp150=(void*)ds->hd;void*_stmttmpB=_tmp150;void*_tmp151=_stmttmpB;struct _fat_ptr*_tmp152;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp151)->tag == 1U){_LL1: _tmp152=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp151)->f1;_LL2: {struct _fat_ptr*f=_tmp152;
return f;}}else{_LL3: _LL4:
({void*_tmp153=0U;({struct _fat_ptr _tmp255=({const char*_tmp154="array designator in struct";_tag_fat(_tmp154,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp255,_tag_fat(_tmp153,sizeof(void*),0U));});});}_LL0:;}}
# 1138
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp155=Cyc_Tcutil_compress(t);void*_stmttmpC=_tmp155;void*_tmp156=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1148
unsigned Cyc_Absyn_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmp157=e->r;void*_stmttmpD=_tmp157;void*_tmp158=_stmttmpD;switch(*((int*)_tmp158)){case 0U: _LL1: _LL2:
 return 0U;case 1U: _LL3: _LL4:
 return 1U;case 2U: _LL5: _LL6:
 return 2U;case 3U: _LL7: _LL8:
 return 3U;case 4U: _LL9: _LLA:
 return 4U;case 5U: _LLB: _LLC:
 return 5U;case 6U: _LLD: _LLE:
 return 6U;case 7U: _LLF: _LL10:
 return 7U;case 8U: _LL11: _LL12:
 return 8U;case 9U: _LL13: _LL14:
 return 9U;case 10U: _LL15: _LL16:
 return 10U;case 11U: _LL17: _LL18:
 return 11U;case 12U: _LL19: _LL1A:
 return 12U;case 13U: _LL1B: _LL1C:
 return 13U;case 14U: _LL1D: _LL1E:
 return 14U;case 15U: _LL1F: _LL20:
 return 15U;case 16U: _LL21: _LL22:
 return 16U;case 17U: _LL23: _LL24:
 return 17U;case 18U: _LL25: _LL26:
 return 18U;case 19U: _LL27: _LL28:
 return 19U;case 20U: _LL29: _LL2A:
 return 20U;case 21U: _LL2B: _LL2C:
 return 21U;case 22U: _LL2D: _LL2E:
 return 22U;case 23U: _LL2F: _LL30:
 return 23U;case 24U: _LL31: _LL32:
 return 24U;case 25U: _LL33: _LL34:
 return 25U;case 26U: _LL35: _LL36:
 return 26U;case 27U: _LL37: _LL38:
 return 27U;case 28U: _LL39: _LL3A:
 return 28U;case 29U: _LL3B: _LL3C:
 return 29U;case 30U: _LL3D: _LL3E:
 return 30U;case 31U: _LL3F: _LL40:
 return 31U;case 32U: _LL41: _LL42:
 return 32U;case 33U: _LL43: _LL44:
 return 33U;case 34U: _LL45: _LL46:
 return 34U;case 35U: _LL47: _LL48:
 return 35U;case 36U: _LL49: _LL4A:
 return 36U;case 37U: _LL4B: _LL4C:
 return 37U;case 38U: _LL4D: _LL4E:
 return 38U;case 39U: _LL4F: _LL50:
 return 39U;case 40U: _LL51: _LL52:
 return 40U;case 41U: _LL53: _LL54:
 return 41U;default: _LL55: _LL56:
 return 42U;}_LL0:;}
# 1199
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
void*_tmp159=e->r;void*_stmttmpE=_tmp159;void*_tmp15A=_stmttmpE;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp179;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp17E;switch(*((int*)_tmp15A)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 1208
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp17E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp17F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_tmp180=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15A)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp17E;struct Cyc_Absyn_Exp*e2=_tmp17F;struct Cyc_Absyn_Exp*e3=_tmp180;
# 1218
if(!Cyc_Absyn_no_side_effects_exp(e1))return 0;
_tmp17C=e2;_tmp17D=e3;goto _LL20;}case 27U: _LL1F: _tmp17C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_tmp17D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15A)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp17C;struct Cyc_Absyn_Exp*e2=_tmp17D;
_tmp17A=e1;_tmp17B=e2;goto _LL22;}case 7U: _LL21: _tmp17A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp17B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp17A;struct Cyc_Absyn_Exp*e2=_tmp17B;
_tmp178=e1;_tmp179=e2;goto _LL24;}case 8U: _LL23: _tmp178=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp179=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp178;struct Cyc_Absyn_Exp*e2=_tmp179;
_tmp176=e1;_tmp177=e2;goto _LL26;}case 23U: _LL25: _tmp176=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp177=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp176;struct Cyc_Absyn_Exp*e2=_tmp177;
_tmp174=e1;_tmp175=e2;goto _LL28;}case 9U: _LL27: _tmp174=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp175=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp174;struct Cyc_Absyn_Exp*e2=_tmp175;
# 1225
return Cyc_Absyn_no_side_effects_exp(e1)&& Cyc_Absyn_no_side_effects_exp(e2);}case 41U: _LL29: _tmp173=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp173;
# 1227
_tmp172=e;goto _LL2C;}case 38U: _LL2B: _tmp172=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp172;
_tmp171=e;goto _LL2E;}case 11U: _LL2D: _tmp171=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp171;
_tmp170=e;goto _LL30;}case 12U: _LL2F: _tmp170=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp170;
_tmp16F=e;goto _LL32;}case 13U: _LL31: _tmp16F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp16F;
_tmp16E=e;goto _LL34;}case 14U: _LL33: _tmp16E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp16E;
_tmp16D=e;goto _LL36;}case 18U: _LL35: _tmp16D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp16D;
_tmp16C=e;goto _LL38;}case 20U: _LL37: _tmp16C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp16C;
_tmp16B=e;goto _LL3A;}case 21U: _LL39: _tmp16B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp16B;
_tmp16A=e;goto _LL3C;}case 22U: _LL3B: _tmp16A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp16A;
_tmp169=e;goto _LL3E;}case 28U: _LL3D: _tmp169=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp169;
_tmp168=e;goto _LL40;}case 42U: _LL3F: _tmp168=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp168;
_tmp167=e;goto _LL42;}case 15U: _LL41: _tmp167=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL42: {struct Cyc_Absyn_Exp*e=_tmp167;
return Cyc_Absyn_no_side_effects_exp(e);}case 34U: _LL43: _tmp165=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15A)->f1).rgn;_tmp166=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15A)->f1).num_elts;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp165;struct Cyc_Absyn_Exp*e=_tmp166;
# 1241
_tmp163=eopt;_tmp164=e;goto _LL46;}case 16U: _LL45: _tmp163=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_tmp164=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL46: {struct Cyc_Absyn_Exp*eopt=_tmp163;struct Cyc_Absyn_Exp*e=_tmp164;
# 1243
if(!Cyc_Absyn_no_side_effects_exp(e))return 0;
if(eopt != 0)return Cyc_Absyn_no_side_effects_exp(eopt);
return 1;}case 3U: _LL47: _tmp162=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL48: {struct Cyc_List_List*es=_tmp162;
# 1247
_tmp161=es;goto _LL4A;}case 31U: _LL49: _tmp161=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL4A: {struct Cyc_List_List*es=_tmp161;
_tmp160=es;goto _LL4C;}case 24U: _LL4B: _tmp160=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL4C: {struct Cyc_List_List*es=_tmp160;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Absyn_no_side_effects_exp,es);}case 36U: _LL4D: _tmp15F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL4E: {struct Cyc_List_List*dles=_tmp15F;
# 1251
_tmp15E=dles;goto _LL50;}case 29U: _LL4F: _tmp15E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15A)->f3;_LL50: {struct Cyc_List_List*dles=_tmp15E;
_tmp15D=dles;goto _LL52;}case 30U: _LL51: _tmp15D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL52: {struct Cyc_List_List*dles=_tmp15D;
_tmp15C=dles;goto _LL54;}case 26U: _LL53: _tmp15C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp15A)->f1;_LL54: {struct Cyc_List_List*dles=_tmp15C;
_tmp15B=dles;goto _LL56;}default: _LL55: _tmp15B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp15A)->f2;_LL56: {struct Cyc_List_List*dles=_tmp15B;
# 1256
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Absyn_no_side_effects_exp((*((struct _tuple12*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}
# 1264
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
# 1269
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;
