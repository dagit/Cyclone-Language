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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
extern int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr);
# 34 "position.h"
extern unsigned Cyc_Position_segment_join(unsigned,unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 311
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 496
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr);
# 499
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 501
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr);
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 667 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 878
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 894
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 900
void*Cyc_Absyn_compress_kb(void*);
# 904
void*Cyc_Absyn_compress(void*);
# 915
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 918
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 923
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 925
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 927
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 930
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 934
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 936
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 961
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one (void);
# 967
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 969
void*Cyc_Absyn_starb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 971
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 973
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 975
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 990
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 992
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1063
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);
# 1067
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned);
# 1089
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1093
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1098
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1134
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1136
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1138
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1141
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1149
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_EvalOrder =11U,Cyc_Flags_CCompiler =12U,Cyc_Flags_AllPasses =13U};
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Hashtable_Table;
# 82 "hashtable.h"
extern int Cyc_Hashtable_hash_string(struct _fat_ptr s);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 128 "tcutil.h"
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 130
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 159
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 167
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 36 "absyn.cyc"
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0 && ss2 != 0;(ss1=ss1->tl,ss2=ss2->tl)){
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
struct _tuple12 _tmp6=({struct _tuple12 _tmp135;_tmp135.f1=n1,_tmp135.f2=n2;_tmp135;});struct _tuple12 _stmttmp0=_tmp6;struct _tuple12 _tmp7=_stmttmp0;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpC;switch(((_tmp7.f1).Abs_n).tag){case 4U: if(((_tmp7.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 61
 return - 1;}case 1U: switch(((_tmp7.f2).Loc_n).tag){case 1U: _LL9: _tmpC=((_tmp7.f1).Rel_n).val;_tmpD=((_tmp7.f2).Rel_n).val;_LLA: {struct Cyc_List_List*x1=_tmpC;struct Cyc_List_List*x2=_tmpD;
# 57
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;default: _LL13: _LL14:
# 63
 return - 1;}case 2U: switch(((_tmp7.f2).Rel_n).tag){case 2U: _LLB: _tmpA=((_tmp7.f1).Abs_n).val;_tmpB=((_tmp7.f2).Abs_n).val;_LLC: {struct Cyc_List_List*x1=_tmpA;struct Cyc_List_List*x2=_tmpB;
# 58
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 65
 return - 1;}default: switch(((_tmp7.f2).Rel_n).tag){case 3U: _LLD: _tmp8=((_tmp7.f1).C_n).val;_tmp9=((_tmp7.f2).C_n).val;_LLE: {struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 59
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: _LL11: _LL12:
# 62
 return 1;case 1U: _LL15: _LL16:
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
return C_scope?({union Cyc_Absyn_Nmspace _tmp136;(_tmp136.C_n).tag=3U,(_tmp136.C_n).val=x;_tmp136;}):({union Cyc_Absyn_Nmspace _tmp137;(_tmp137.Abs_n).tag=2U,(_tmp137.Abs_n).val=x;_tmp137;});}
# 80
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp138;(_tmp138.Rel_n).tag=1U,(_tmp138.Rel_n).val=x;_tmp138;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 83
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmpE;union Cyc_Absyn_Nmspace _tmpF=_stmttmp1;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 92
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 96
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 100
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp139;_tmp139.print_const=0,_tmp139.q_volatile=0,_tmp139.q_restrict=0,_tmp139.real_const=0,_tmp139.loc=loc;_tmp139;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp13A;_tmp13A.print_const=1,_tmp13A.q_volatile=0,_tmp13A.q_restrict=0,_tmp13A.real_const=1,_tmp13A.loc=loc;_tmp13A;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp13B;_tmp13B.print_const=x.print_const || y.print_const,_tmp13B.q_volatile=
x.q_volatile || y.q_volatile,_tmp13B.q_restrict=
x.q_restrict || y.q_restrict,_tmp13B.real_const=
x.real_const || y.real_const,({
unsigned _tmp159=Cyc_Position_segment_join(x.loc,y.loc);_tmp13B.loc=_tmp159;});_tmp13B;});}
# 109
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 115
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 117
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp13C;(_tmp13C.UnknownDatatype).tag=1U,(_tmp13C.UnknownDatatype).val=udi;_tmp13C;});}
# 120
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp13D;(_tmp13D.KnownDatatype).tag=2U,(_tmp13D.KnownDatatype).val=d;_tmp13D;});}
# 123
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp13E;(_tmp13E.UnknownDatatypefield).tag=1U,(_tmp13E.UnknownDatatypefield).val=s;_tmp13E;});}
# 126
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp13F;(_tmp13F.KnownDatatypefield).tag=2U,((_tmp13F.KnownDatatypefield).val).f1=dd,((_tmp13F.KnownDatatypefield).val).f2=df;_tmp13F;});}
# 129
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp140;(_tmp140.UnknownAggr).tag=1U,((_tmp140.UnknownAggr).val).f1=ak,((_tmp140.UnknownAggr).val).f2=n,((_tmp140.UnknownAggr).val).f3=tagged;_tmp140;});}
# 132
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp141;(_tmp141.KnownAggr).tag=2U,(_tmp141.KnownAggr).val=ad;_tmp141;});}
# 135
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp12;void**_tmp13;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp13=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8: {void**k2=_tmp13;
# 140
_tmp12=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 139
 return k;}else{_LL9: _tmp12=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA: {void**k2=_tmp12;
# 142
({void*_tmp15A=Cyc_Absyn_compress_kb(*k2);*k2=_tmp15A;});
return*k2;}}}_LL0:;}
# 146
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_stmttmp2=_tmp14;void*_tmp15=_stmttmp2;struct Cyc_Absyn_Kind*_tmp17;struct Cyc_Core_Opt**_tmp16;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp19;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp19=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp19;
return k;}case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp18;
_tmp16=f;_tmp17=& Cyc_Tcutil_bk;goto _LL6;}default: _LL5: _tmp16=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6: {struct Cyc_Core_Opt**f=_tmp16;struct Cyc_Absyn_Kind*k=_tmp17;
# 151
({struct Cyc_Core_Opt*_tmp15C=({struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));({void*_tmp15B=Cyc_Tcutil_kind_to_bound(k);_tmp1A->v=_tmp15B;});_tmp1A;});*f=_tmp15C;});
return k;}}_LL0:;}
# 157
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp15D=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp15D;});_tmp1B;});}
# 160
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 176 "absyn.cyc"
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
# 202
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0U,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 204
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
# 217
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp23=0U;({void*_tmp15E=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=15U,_tmp24->f1=n,_tmp24->f2=d;_tmp24;});Cyc_Absyn_app_type(_tmp15E,_tag_fat(_tmp23,sizeof(void*),0U));});});}
# 220
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp25=0U;({void*_tmp15F=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=16U,_tmp26->f1=fs;_tmp26;});Cyc_Absyn_app_type(_tmp15F,_tag_fat(_tmp25,sizeof(void*),0U));});});}
# 223
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp27=0U;({void*_tmp160=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=17U,_tmp28->f1=s,_tmp28->f2=k;_tmp28;});Cyc_Absyn_app_type(_tmp160,_tag_fat(_tmp27,sizeof(void*),0U));});});}
# 226
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp161=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=18U,_tmp29->f1=di;_tmp29;});_tmp2A->f1=_tmp161;}),_tmp2A->f2=args;_tmp2A;});}
# 229
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp162=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=di;_tmp2B;});_tmp2C->f1=_tmp162;}),_tmp2C->f2=args;_tmp2C;});}
# 232
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp163=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=20U,_tmp2D->f1=ai;_tmp2D;});_tmp2E->f1=_tmp163;}),_tmp2E->f2=args;_tmp2E;});}
# 236
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,_tmp2F->f1=x;_tmp2F;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp30=i;switch(_tmp30){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp32=({struct Cyc_Warn_String_Warn_Warg_struct _tmp144;_tmp144.tag=0U,({struct _fat_ptr _tmp164=({const char*_tmp36="gen_float_type(";_tag_fat(_tmp36,sizeof(char),16U);});_tmp144.f1=_tmp164;});_tmp144;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp33=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp143;_tmp143.tag=12U,_tmp143.f1=(int)i;_tmp143;});struct Cyc_Warn_String_Warn_Warg_struct _tmp34=({struct Cyc_Warn_String_Warn_Warg_struct _tmp142;_tmp142.tag=0U,({struct _fat_ptr _tmp165=({const char*_tmp35=")";_tag_fat(_tmp35,sizeof(char),2U);});_tmp142.f1=_tmp165;});_tmp142;});void*_tmp31[3U];_tmp31[0]=& _tmp32,_tmp31[1]=& _tmp33,_tmp31[2]=& _tmp34;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp31,sizeof(void*),3U));});}_LL0:;}
# 245
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp37=sn;switch(_tmp37){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 248
enum Cyc_Absyn_Size_of _tmp38=sz;switch(_tmp38){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 257
enum Cyc_Absyn_Size_of _tmp39=sz;switch(_tmp39){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 265
 goto _LL8;default: _LL7: _LL8: {
# 267
enum Cyc_Absyn_Size_of _tmp3A=sz;switch(_tmp3A){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 278
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 281
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 286
void*Cyc_Absyn_bounds_one (void){
static void*bone=0;
if(bone == 0)
bone=Cyc_Absyn_thin_bounds_int(1U);
return(void*)_check_null(bone);}
# 296
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 299
void*Cyc_Absyn_wchar_type (void){
int _tmp3B=Sizeof_wchar_t;switch(_tmp3B){case 1U: _LL1: _LL2:
# 310 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 314
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3C[4U]="exn";
# 319
static struct _fat_ptr Cyc_Absyn_exn_str={_tmp3C,_tmp3C,_tmp3C + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp43[15U]="Null_Exception";static char _tmp44[13U]="Array_bounds";static char _tmp45[16U]="Match_Exception";static char _tmp46[10U]="Bad_alloc";
# 323
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void){
static struct _fat_ptr builtin_exns[4U]={{_tmp43,_tmp43,_tmp43 + 15U},{_tmp44,_tmp44,_tmp44 + 13U},{_tmp45,_tmp45,_tmp45 + 16U},{_tmp46,_tmp46,_tmp46 + 10U}};
# 326
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));({struct Cyc_Absyn_Datatypefield*_tmp168=({struct Cyc_Absyn_Datatypefield*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct _tuple0*_tmp167=({struct _tuple0*_tmp3E=_cycalloc(sizeof(*_tmp3E));((_tmp3E->f1).Abs_n).tag=2U,((_tmp3E->f1).Abs_n).val=0,({
struct _fat_ptr*_tmp166=({struct _fat_ptr*_tmp3D=_cycalloc(sizeof(*_tmp3D));*_tmp3D=*((struct _fat_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _fat_ptr),i));_tmp3D;});_tmp3E->f2=_tmp166;});_tmp3E;});
# 330
_tmp3F->name=_tmp167;}),_tmp3F->typs=0,_tmp3F->loc=0U,_tmp3F->sc=Cyc_Absyn_Extern;_tmp3F;});_tmp40->hd=_tmp168;}),_tmp40->tl=tufs;_tmp40;});}}
# 334
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->sc=Cyc_Absyn_Extern,_tmp42->name=Cyc_Absyn_exn_name,_tmp42->tvs=0,({struct Cyc_Core_Opt*_tmp169=({struct Cyc_Core_Opt*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->v=tufs;_tmp41;});_tmp42->fields=_tmp169;}),_tmp42->is_extensible=1;_tmp42;});}
# 336
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 339
void*Cyc_Absyn_exn_type (void){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp145;(_tmp145.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp16B=({struct Cyc_Absyn_Datatypedecl**_tmp47=_cycalloc(sizeof(*_tmp47));({struct Cyc_Absyn_Datatypedecl*_tmp16A=Cyc_Absyn_exn_tud();*_tmp47=_tmp16A;});_tmp47;});(_tmp145.KnownDatatype).val=_tmp16B;});_tmp145;}),0);
exn_typ=({void*_tmp16E=(void*)_check_null(eopt);void*_tmp16D=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp16C=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp16E,_tmp16D,_tmp16C,Cyc_Absyn_false_type);});}
# 346
return(void*)_check_null(exn_typ);}
# 349
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4A=_cycalloc(sizeof(*_tmp4A));({union Cyc_Absyn_Nmspace _tmp171=Cyc_Absyn_Abs_n(0,0);_tmp4A->f1=_tmp171;}),({struct _fat_ptr*_tmp170=({struct _fat_ptr*_tmp49=_cycalloc(sizeof(*_tmp49));({struct _fat_ptr _tmp16F=({const char*_tmp48="PrintArg";_tag_fat(_tmp48,sizeof(char),9U);});*_tmp49=_tmp16F;});_tmp49;});_tmp4A->f2=_tmp170;});_tmp4A;});
return(struct _tuple0*)_check_null(q);}
# 355
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4D=_cycalloc(sizeof(*_tmp4D));({union Cyc_Absyn_Nmspace _tmp174=Cyc_Absyn_Abs_n(0,0);_tmp4D->f1=_tmp174;}),({struct _fat_ptr*_tmp173=({struct _fat_ptr*_tmp4C=_cycalloc(sizeof(*_tmp4C));({struct _fat_ptr _tmp172=({const char*_tmp4B="ScanfArg";_tag_fat(_tmp4B,sizeof(char),9U);});*_tmp4C=_tmp172;});_tmp4C;});_tmp4D->f2=_tmp173;});_tmp4D;});
return(struct _tuple0*)_check_null(q);}
# 367
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp53=_cycalloc(sizeof(*_tmp53));({union Cyc_Absyn_Nmspace _tmp179=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _fat_ptr*_tmp178=({struct _fat_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct _fat_ptr _tmp177=({const char*_tmp4E="Core";_tag_fat(_tmp4E,sizeof(char),5U);});*_tmp4F=_tmp177;});_tmp4F;});_tmp50->hd=_tmp178;}),_tmp50->tl=0;_tmp50;}),0);_tmp53->f1=_tmp179;}),({struct _fat_ptr*_tmp176=({struct _fat_ptr*_tmp52=_cycalloc(sizeof(*_tmp52));({struct _fat_ptr _tmp175=({const char*_tmp51="unique_region";_tag_fat(_tmp51,sizeof(char),14U);});*_tmp52=_tmp175;});_tmp52;});_tmp53->f2=_tmp176;});_tmp53;});
return(struct _tuple0*)_check_null(q);}
# 373
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void){
void*_tmp54=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);void*t=_tmp54;
return({struct Cyc_Absyn_Exp*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->topt=t,_tmp58->loc=0U,_tmp58->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp17E=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=1U,({void*_tmp17D=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp17C=({struct Cyc_Absyn_Vardecl*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->sc=Cyc_Absyn_Extern,({
struct _tuple0*_tmp17B=Cyc_Absyn_uniquergn_qvar();_tmp55->name=_tmp17B;}),_tmp55->varloc=0U,({
struct Cyc_Absyn_Tqual _tmp17A=Cyc_Absyn_empty_tqual(0U);_tmp55->tq=_tmp17A;}),_tmp55->type=t,_tmp55->initializer=0,_tmp55->rgn=0,_tmp55->attributes=0,_tmp55->escapes=0,_tmp55->is_proto=0;_tmp55;});
# 376
_tmp56->f1=_tmp17C;});_tmp56;});_tmp57->f1=_tmp17D;});_tmp57;});_tmp58->r=_tmp17E;});_tmp58;});}
# 384
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=3U,_tmp59->f1=s;_tmp59;});}
# 388
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp146;_tmp146.elt_type=t,_tmp146.elt_tq=tq,(_tmp146.ptr_atts).rgn=r,(_tmp146.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp146.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp146.ptr_atts).zero_term=zt,(_tmp146.ptr_atts).ptrloc=0;_tmp146;}));}
# 393
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp147;_tmp147.elt_type=t,_tmp147.elt_tq=tq,(_tmp147.ptr_atts).rgn=r,(_tmp147.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp147.ptr_atts).bounds=b,(_tmp147.ptr_atts).zero_term=zt,(_tmp147.ptr_atts).ptrloc=0;_tmp147;}));}
# 398
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp148;_tmp148.elt_type=t,_tmp148.elt_tq=tq,(_tmp148.ptr_atts).rgn=r,(_tmp148.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp148.ptr_atts).bounds=b,(_tmp148.ptr_atts).zero_term=zt,(_tmp148.ptr_atts).ptrloc=0;_tmp148;}));}
# 403
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp182=t;void*_tmp181=r;struct Cyc_Absyn_Tqual _tmp180=tq;void*_tmp17F=Cyc_Absyn_bounds_one();Cyc_Absyn_starb_type(_tmp182,_tmp181,_tmp180,_tmp17F,zeroterm);});}
# 406
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}
# 409
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp186=t;void*_tmp185=r;struct Cyc_Absyn_Tqual _tmp184=tq;void*_tmp183=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp186,_tmp185,_tmp184,_tmp183,zeroterm);});}
# 412
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp18A=Cyc_Absyn_char_type;void*_tmp189=rgn;struct Cyc_Absyn_Tqual _tmp188=Cyc_Absyn_empty_tqual(0U);void*_tmp187=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp18A,_tmp189,_tmp188,_tmp187,Cyc_Absyn_true_type);});}
# 415
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp18E=Cyc_Absyn_char_type;void*_tmp18D=rgn;struct Cyc_Absyn_Tqual _tmp18C=Cyc_Absyn_const_tqual(0U);void*_tmp18B=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp18E,_tmp18D,_tmp18C,_tmp18B,Cyc_Absyn_true_type);});}
# 419
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 421
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=4U,(_tmp5A->f1).elt_type=elt_type,(_tmp5A->f1).tq=tq,(_tmp5A->f1).num_elts=num_elts,(_tmp5A->f1).zero_term=zero_term,(_tmp5A->f1).zt_loc=ztloc;_tmp5A;});}
# 424
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=11U,_tmp5B->f1=e;_tmp5B;});}
# 429
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 431
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=8U,_tmp5C->f1=n,_tmp5C->f2=args,_tmp5C->f3=d,_tmp5C->f4=defn;_tmp5C;});}
# 437
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=0U,({void*_tmp191=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=20U,({union Cyc_Absyn_AggrInfo _tmp190=({enum Cyc_Absyn_AggrKind _tmp18F=k;Cyc_Absyn_UnknownAggr(_tmp18F,({struct _tuple0*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->f1=Cyc_Absyn_rel_ns_null,_tmp5D->f2=name;_tmp5D;}),0);});_tmp5E->f1=_tmp190;});_tmp5E;});_tmp5F->f1=_tmp191;}),_tmp5F->f2=0;_tmp5F;});}
# 440
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 443
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 446
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 451
void*Cyc_Absyn_compress(void*t){
void*_tmp60=t;void*_tmp61;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Exp*_tmp63;void**_tmp64;void**_tmp65;switch(*((int*)_tmp60)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp60)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp65=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp60)->f2;_LL8: {void**t2opt_ref=_tmp65;
# 457
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Absyn_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp60)->f4 == 0){_LL3: _LL4:
# 454
 return t;}else{_LL5: _tmp64=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp60)->f4;_LL6: {void**topt_ref=_tmp64;
_tmp65=topt_ref;goto _LL8;}}case 9U: _LL9: _tmp63=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp60)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp63;
# 463
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp66=e->r;void*_stmttmp3=_tmp66;void*_tmp67=_stmttmp3;void*_tmp68;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp67)->tag == 39U){_LL12: _tmp68=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp67)->f1;_LL13: {void*t2=_tmp68;
return Cyc_Absyn_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11U: _LLB: _tmp62=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp60)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp62;
# 469
void*_tmp69=e->topt;void*t2=_tmp69;
return t2 != 0?t2: t;}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60)->f2 != 0){_LLD: _tmp61=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60)->f2;_LLE: {void*t=_tmp61;
return Cyc_Absyn_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 477
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp149;(_tmp149.Char_c).tag=2U,((_tmp149.Char_c).val).f1=sn,((_tmp149.Char_c).val).f2=c;_tmp149;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp14A;(_tmp14A.Wchar_c).tag=3U,(_tmp14A.Wchar_c).val=s;_tmp14A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp14B;(_tmp14B.Short_c).tag=4U,((_tmp14B.Short_c).val).f1=sn,((_tmp14B.Short_c).val).f2=s;_tmp14B;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp14C;(_tmp14C.Int_c).tag=5U,((_tmp14C.Int_c).val).f1=sn,((_tmp14C.Int_c).val).f2=i;_tmp14C;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp14D;(_tmp14D.LongLong_c).tag=6U,((_tmp14D.LongLong_c).val).f1=sn,((_tmp14D.LongLong_c).val).f2=l;_tmp14D;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp14E;(_tmp14E.Float_c).tag=7U,((_tmp14E.Float_c).val).f1=s,((_tmp14E.Float_c).val).f2=i;_tmp14E;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp14F;(_tmp14F.String_c).tag=8U,(_tmp14F.String_c).val=s;_tmp14F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp150;(_tmp150.Wstring_c).tag=9U,(_tmp150.Wstring_c).val=s;_tmp150;});}
# 487
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->topt=0,_tmp6A->r=r,_tmp6A->loc=loc,_tmp6A->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp6A;});}
# 490
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp192=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=16U,_tmp6B->f1=rgn_handle,_tmp6B->f2=e;_tmp6B;});Cyc_Absyn_new_exp(_tmp192,loc);});}
# 493
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp6C=_cycalloc(sizeof(*_tmp6C));*_tmp6C=*e;_tmp6C;});}
# 496
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp193=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=0U,_tmp6D->f1=c;_tmp6D;});Cyc_Absyn_new_exp(_tmp193,loc);});}
# 499
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 503
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp194=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp194,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 511
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 518
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp195=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp195,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp196=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp196,loc);});}
# 525
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp197=f(s);Cyc_Absyn_const_exp(_tmp197,loc);});}
# 528
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 532
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp199=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=1U,({void*_tmp198=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=0U,_tmp6E->f1=q;_tmp6E;});_tmp6F->f1=_tmp198;});_tmp6F;});Cyc_Absyn_new_exp(_tmp199,loc);});}
# 535
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp19A=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,_tmp70->f1=b;_tmp70;});Cyc_Absyn_new_exp(_tmp19A,loc);});}
# 539
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 542
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
return({void*_tmp19B=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2U,_tmp71->f1=s;_tmp71;});Cyc_Absyn_new_exp(_tmp19B,loc);});}
# 545
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=3U,_tmp72->f1=p,_tmp72->f2=es;_tmp72;});Cyc_Absyn_new_exp(_tmp19C,loc);});}
# 548
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp19E=p;struct Cyc_List_List*_tmp19D=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=e,_tmp73->tl=0;_tmp73;});Cyc_Absyn_primop_exp(_tmp19E,_tmp19D,loc);});}
# 551
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1A1=p;struct Cyc_List_List*_tmp1A0=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=e1,({struct Cyc_List_List*_tmp19F=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->hd=e2,_tmp74->tl=0;_tmp74;});_tmp75->tl=_tmp19F;});_tmp75;});Cyc_Absyn_primop_exp(_tmp1A1,_tmp1A0,loc);});}
# 554
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A2=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=35U,_tmp76->f1=e1,_tmp76->f2=e2;_tmp76;});Cyc_Absyn_new_exp(_tmp1A2,loc);});}
# 557
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 567
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A3=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=4U,_tmp77->f1=e1,_tmp77->f2=popt,_tmp77->f3=e2;_tmp77;});Cyc_Absyn_new_exp(_tmp1A3,loc);});}
# 570
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp1A4=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=5U,_tmp78->f1=e,_tmp78->f2=i;_tmp78;});Cyc_Absyn_new_exp(_tmp1A4,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp1A5=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=6U,_tmp79->f1=e1,_tmp79->f2=e2,_tmp79->f3=e3;_tmp79;});Cyc_Absyn_new_exp(_tmp1A5,loc);});}
# 579
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A6=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=7U,_tmp7A->f1=e1,_tmp7A->f2=e2;_tmp7A;});Cyc_Absyn_new_exp(_tmp1A6,loc);});}
# 582
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=8U,_tmp7B->f1=e1,_tmp7B->f2=e2;_tmp7B;});Cyc_Absyn_new_exp(_tmp1A7,loc);});}
# 585
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=9U,_tmp7C->f1=e1,_tmp7C->f2=e2;_tmp7C;});Cyc_Absyn_new_exp(_tmp1A8,loc);});}
# 588
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1A9=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=10U,_tmp7D->f1=e,_tmp7D->f2=es,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});Cyc_Absyn_new_exp(_tmp1A9,loc);});}
# 591
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=10U,_tmp7E->f1=e,_tmp7E->f2=es,_tmp7E->f3=0,_tmp7E->f4=1;_tmp7E;});Cyc_Absyn_new_exp(_tmp1AA,loc);});}
# 594
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1AB=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=12U,_tmp7F->f1=e;_tmp7F;});Cyc_Absyn_new_exp(_tmp1AB,loc);});}
# 597
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp1AC=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=13U,_tmp80->f1=e,_tmp80->f2=ts;_tmp80;});Cyc_Absyn_new_exp(_tmp1AC,loc);});}
# 600
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp1AD=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=14U,_tmp81->f1=t,_tmp81->f2=e,_tmp81->f3=user_cast,_tmp81->f4=c;_tmp81;});Cyc_Absyn_new_exp(_tmp1AD,loc);});}
# 603
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1AE=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=11U,_tmp82->f1=e,_tmp82->f2=0;_tmp82;});Cyc_Absyn_new_exp(_tmp1AE,loc);});}
# 606
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1AF=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=11U,_tmp83->f1=e,_tmp83->f2=1;_tmp83;});Cyc_Absyn_new_exp(_tmp1AF,loc);});}
# 609
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B0=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=15U,_tmp84->f1=e;_tmp84;});Cyc_Absyn_new_exp(_tmp1B0,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp1B1=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=17U,_tmp85->f1=t;_tmp85;});Cyc_Absyn_new_exp(_tmp1B1,loc);});}
# 613
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B2=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=18U,_tmp86->f1=e;_tmp86;});Cyc_Absyn_new_exp(_tmp1B2,loc);});}
# 616
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp1B3=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=19U,_tmp87->f1=t,_tmp87->f2=ofs;_tmp87;});Cyc_Absyn_new_exp(_tmp1B3,loc);});}
# 619
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B4=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=20U,_tmp88->f1=e;_tmp88;});Cyc_Absyn_new_exp(_tmp1B4,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1B5=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=21U,_tmp89->f1=e,_tmp89->f2=n,_tmp89->f3=0,_tmp89->f4=0;_tmp89;});Cyc_Absyn_new_exp(_tmp1B5,loc);});}
# 623
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1B6=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=22U,_tmp8A->f1=e,_tmp8A->f2=n,_tmp8A->f3=0,_tmp8A->f4=0;_tmp8A;});Cyc_Absyn_new_exp(_tmp1B6,loc);});}
# 626
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1B7=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=23U,_tmp8B->f1=e1,_tmp8B->f2=e2;_tmp8B;});Cyc_Absyn_new_exp(_tmp1B7,loc);});}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1B8=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=24U,_tmp8C->f1=es;_tmp8C;});Cyc_Absyn_new_exp(_tmp1B8,loc);});}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1B9=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=37U,_tmp8D->f1=s;_tmp8D;});Cyc_Absyn_new_exp(_tmp1B9,loc);});}
# 635
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp1BA=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=39U,_tmp8E->f1=t;_tmp8E;});Cyc_Absyn_new_exp(_tmp1BA,loc);});}
# 639
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 642
return({void*_tmp1BB=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=40U,_tmp8F->f1=volatile_kw,_tmp8F->f2=tmpl,_tmp8F->f3=outs,_tmp8F->f4=ins,_tmp8F->f5=clobs;_tmp8F;});Cyc_Absyn_new_exp(_tmp1BB,loc);});}
# 644
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1BC=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=41U,_tmp90->f1=e;_tmp90;});Cyc_Absyn_new_exp(_tmp1BC,loc);});}
# 647
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1BD=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=42U,_tmp91->f1=e;_tmp91;});Cyc_Absyn_new_exp(_tmp1BD,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp92=0;struct Cyc_List_List*dles=_tmp92;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));({struct _tuple13*_tmp1BE=({struct _tuple13*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->f1=0,_tmp93->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp93;});_tmp94->hd=_tmp1BE;}),_tmp94->tl=dles;_tmp94;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return({void*_tmp1BF=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=26U,_tmp95->f1=dles;_tmp95;});Cyc_Absyn_new_exp(_tmp1BF,loc);});}
# 658
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 661
return({void*_tmp1C0=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=36U,_tmp96->f1=n,_tmp96->f2=dles;_tmp96;});Cyc_Absyn_new_exp(_tmp1C0,loc);});}
# 664
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->r=s,_tmp97->loc=loc,_tmp97->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp97;});}
# 667
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1C1=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=1U,_tmp98->f1=e;_tmp98;});Cyc_Absyn_new_stmt(_tmp1C1,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1C2=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=3U,_tmp99->f1=e;_tmp99;});Cyc_Absyn_new_stmt(_tmp1C2,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1C4=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1C3=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1C4,_tmp1C3,loc);});}}}struct _tuple14{void*f1;void*f2;};
# 680
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple14 _tmp9A=({struct _tuple14 _tmp151;_tmp151.f1=s1->r,_tmp151.f2=s2->r;_tmp151;});struct _tuple14 _stmttmp4=_tmp9A;struct _tuple14 _tmp9B=_stmttmp4;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9B.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9B.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1C5=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=2U,_tmp9C->f1=s1,_tmp9C->f2=s2;_tmp9C;});Cyc_Absyn_new_stmt(_tmp1C5,loc);});}}_LL0:;}
# 687
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp1C6=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=4U,_tmp9D->f1=e,_tmp9D->f2=s1,_tmp9D->f3=s2;_tmp9D;});Cyc_Absyn_new_stmt(_tmp1C6,loc);});}
# 690
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1C8=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=5U,(_tmp9E->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1C7=Cyc_Absyn_skip_stmt(e->loc);(_tmp9E->f1).f2=_tmp1C7;}),_tmp9E->f2=s;_tmp9E;});Cyc_Absyn_new_stmt(_tmp1C8,loc);});}
# 693
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1CB=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=9U,_tmp9F->f1=e1,(_tmp9F->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1CA=Cyc_Absyn_skip_stmt(e3->loc);(_tmp9F->f2).f2=_tmp1CA;}),
(_tmp9F->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1C9=Cyc_Absyn_skip_stmt(e3->loc);(_tmp9F->f3).f2=_tmp1C9;}),_tmp9F->f4=s;_tmp9F;});
# 694
Cyc_Absyn_new_stmt(_tmp1CB,loc);});}
# 698
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1CD=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=14U,_tmpA0->f1=s,(_tmpA0->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1CC=Cyc_Absyn_skip_stmt(e->loc);(_tmpA0->f2).f2=_tmp1CC;});_tmpA0;});Cyc_Absyn_new_stmt(_tmp1CD,loc);});}
# 701
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1CE=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=10U,_tmpA1->f1=e,_tmpA1->f2=scs,_tmpA1->f3=0;_tmpA1;});Cyc_Absyn_new_stmt(_tmp1CE,loc);});}
# 704
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1CF=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=15U,_tmpA2->f1=s,_tmpA2->f2=scs,_tmpA2->f3=0;_tmpA2;});Cyc_Absyn_new_stmt(_tmp1CF,loc);});}
# 707
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp1D0=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=11U,_tmpA3->f1=el,_tmpA3->f2=0;_tmpA3;});Cyc_Absyn_new_stmt(_tmp1D0,loc);});}
# 710
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
return({void*_tmp1D1=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=8U,_tmpA4->f1=lab;_tmpA4;});Cyc_Absyn_new_stmt(_tmp1D1,loc);});}
# 713
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1D2=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=13U,_tmpA5->f1=v,_tmpA5->f2=s;_tmpA5;});Cyc_Absyn_new_stmt(_tmp1D2,loc);});}
# 716
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1D3=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=12U,_tmpA6->f1=d,_tmpA6->f2=s;_tmpA6;});Cyc_Absyn_new_stmt(_tmp1D3,loc);});}
# 719
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1D5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp1D4=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmpA8->f1=_tmp1D4;});_tmpA8;});Cyc_Absyn_new_decl(_tmp1D5,loc);});
return({void*_tmp1D6=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=12U,_tmpA7->f1=d,_tmpA7->f2=s;_tmpA7;});Cyc_Absyn_new_stmt(_tmp1D6,loc);});}
# 723
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp1D7=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1D7,loc);});}
# 727
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->r=p,_tmpA9->topt=0,_tmpA9->loc=s;_tmpA9;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1D8=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=17U,_tmpAA->f1=e;_tmpAA;});Cyc_Absyn_new_pat(_tmp1D8,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 733
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->r=r,_tmpAB->loc=loc;_tmpAB;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1D9=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=2U,_tmpAC->f1=p,_tmpAC->f2=0,_tmpAC->f3=e,_tmpAC->f4=0;_tmpAC;});Cyc_Absyn_new_decl(_tmp1D9,loc);});}
# 737
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp1DA=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=3U,_tmpAD->f1=vds;_tmpAD;});Cyc_Absyn_new_decl(_tmp1DA,loc);});}
# 740
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp1DB=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=4U,_tmpAE->f1=tv,_tmpAE->f2=vd,_tmpAE->f3=open_exp;_tmpAE;});Cyc_Absyn_new_decl(_tmp1DB,loc);});}
# 743
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 745
return({void*_tmp1DE=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,({struct Cyc_Absyn_Pat*_tmp1DD=({void*_tmp1DC=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=2U,_tmpAF->f1=tv,_tmpAF->f2=vd;_tmpAF;});Cyc_Absyn_new_pat(_tmp1DC,loc);});_tmpB0->f1=_tmp1DD;}),_tmpB0->f2=0,_tmpB0->f3=e,_tmpB0->f4=0;_tmpB0;});Cyc_Absyn_new_decl(_tmp1DE,loc);});}
# 748
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->sc=Cyc_Absyn_Public,_tmpB1->name=x,_tmpB1->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1DF=Cyc_Absyn_empty_tqual(0U);_tmpB1->tq=_tmp1DF;}),_tmpB1->type=t,_tmpB1->initializer=init,_tmpB1->rgn=0,_tmpB1->attributes=0,_tmpB1->escapes=0,_tmpB1->is_proto=0;_tmpB1;});}
# 755
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->sc=Cyc_Absyn_Static,_tmpB2->name=x,_tmpB2->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1E0=Cyc_Absyn_empty_tqual(0U);_tmpB2->tq=_tmp1E0;}),_tmpB2->type=t,_tmpB2->initializer=init,_tmpB2->rgn=0,_tmpB2->attributes=0,_tmpB2->escapes=0,_tmpB2->is_proto=0;_tmpB2;});}
# 761
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 765
return({struct Cyc_Absyn_AggrdeclImpl*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->exist_vars=exists,_tmpB3->rgn_po=po,_tmpB3->fields=fs,_tmpB3->tagged=tagged;_tmpB3;});}
# 768
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 771
return({void*_tmp1E2=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp1E1=({struct Cyc_Absyn_Aggrdecl*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->kind=k,_tmpB4->sc=s,_tmpB4->name=n,_tmpB4->tvs=ts,_tmpB4->impl=i,_tmpB4->attributes=atts,_tmpB4->expected_mem_kind=0;_tmpB4;});_tmpB5->f1=_tmp1E1;});_tmpB5;});Cyc_Absyn_new_decl(_tmp1E2,loc);});}
# 776
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 779
return({struct Cyc_Absyn_TypeDecl*_tmpB8=_cycalloc(sizeof(*_tmpB8));({void*_tmp1E4=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp1E3=({struct Cyc_Absyn_Aggrdecl*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->kind=k,_tmpB6->sc=s,_tmpB6->name=n,_tmpB6->tvs=ts,_tmpB6->impl=i,_tmpB6->attributes=atts,_tmpB6->expected_mem_kind=0;_tmpB6;});_tmpB7->f1=_tmp1E3;});_tmpB7;});_tmpB8->r=_tmp1E4;}),_tmpB8->loc=loc;_tmpB8;});}
# 785
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 788
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 790
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 793
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 795
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 798
return({void*_tmp1E6=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp1E5=({struct Cyc_Absyn_Datatypedecl*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->sc=s,_tmpB9->name=n,_tmpB9->tvs=ts,_tmpB9->fields=fs,_tmpB9->is_extensible=is_extensible;_tmpB9;});_tmpBA->f1=_tmp1E5;});_tmpBA;});Cyc_Absyn_new_decl(_tmp1E6,loc);});}
# 801
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 804
return({struct Cyc_Absyn_TypeDecl*_tmpBD=_cycalloc(sizeof(*_tmpBD));({void*_tmp1E8=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp1E7=({struct Cyc_Absyn_Datatypedecl*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->sc=s,_tmpBB->name=n,_tmpBB->tvs=ts,_tmpBB->fields=fs,_tmpBB->is_extensible=is_extensible;_tmpBB;});_tmpBC->f1=_tmp1E7;});_tmpBC;});_tmpBD->r=_tmp1E8;}),_tmpBD->loc=loc;_tmpBD;});}
# 816 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 822
{struct Cyc_List_List*_tmpBE=args;struct Cyc_List_List*args2=_tmpBE;for(0;args2 != 0;args2=args2->tl){
({void*_tmp1E9=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f3,1);(*((struct _tuple8*)args2->hd)).f3=_tmp1E9;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=5U,(_tmpBF->f1).tvars=tvs,(_tmpBF->f1).ret_tqual=ret_tqual,({
# 826
void*_tmp1EA=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpBF->f1).ret_type=_tmp1EA;}),(_tmpBF->f1).effect=eff_type,(_tmpBF->f1).args=args,(_tmpBF->f1).c_varargs=c_varargs,(_tmpBF->f1).cyc_varargs=cyc_varargs,(_tmpBF->f1).rgn_po=rgn_po,(_tmpBF->f1).attributes=atts,(_tmpBF->f1).requires_clause=req,(_tmpBF->f1).requires_relns=0,(_tmpBF->f1).ensures_clause=ens,(_tmpBF->f1).ensures_relns=0,(_tmpBF->f1).return_value=0;_tmpBF;});}
# 841
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpC0=Cyc_Absyn_compress(t);void*_stmttmp5=_tmpC0;void*_tmpC1=_stmttmp5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC1)->tag == 5U){_LL1: _LL2:
 return({void*_tmp1ED=t;void*_tmp1EC=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->v=& Cyc_Tcutil_rk;_tmpC2;}),0): Cyc_Absyn_heap_rgn_type;
# 843
struct Cyc_Absyn_Tqual _tmp1EB=
# 847
Cyc_Absyn_empty_tqual(0U);
# 843
Cyc_Absyn_at_type(_tmp1ED,_tmp1EC,_tmp1EB,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 848
 return t;}_LL0:;}
# 863 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpC3=e->r;void*_stmttmp6=_tmpC3;void*_tmpC4=_stmttmp6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;switch(*((int*)_tmpC4)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC4)->f1)){case 2U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpC9=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC4)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpC9;
_tmpC8=vd;goto _LL6;}case 4U: _LL5: _tmpC8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC4)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpC8;
# 868
void*_tmpCA=Cyc_Absyn_compress(vd->type);void*_stmttmp7=_tmpCA;void*_tmpCB=_stmttmp7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCB)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 872
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpC7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC4)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpC7;
return Cyc_Absyn_is_lvalue(e1);}case 13U: _LL11: _tmpC6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC4)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpC6;
return Cyc_Absyn_is_lvalue(e1);}case 12U: _LL13: _tmpC5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC4)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpC5;
return Cyc_Absyn_is_lvalue(e1);}default: _LL15: _LL16:
 return 0;}_LL0:;}
# 883
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*_tmpCC=fields;struct Cyc_List_List*fs=_tmpCC;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 889
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 893
struct _tuple15*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;(-- i,ts=ts->tl)){
if(ts == 0)
return 0;}
if(ts == 0)
return 0;
return(struct _tuple15*)ts->hd;}
# 902
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_tmpCD=decl->r;void*_stmttmp8=_tmpCD;void*_tmpCE=_stmttmp8;struct Cyc_Absyn_Fndecl*_tmpCF;struct Cyc_Absyn_Vardecl*_tmpD0;struct Cyc_Absyn_Typedefdecl*_tmpD1;struct Cyc_Absyn_Enumdecl*_tmpD2;struct Cyc_Absyn_Aggrdecl*_tmpD3;switch(*((int*)_tmpCE)){case 5U: _LL1: _tmpD3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*x=_tmpD3;
return(*x->name).f2;}case 7U: _LL3: _tmpD2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;_LL4: {struct Cyc_Absyn_Enumdecl*x=_tmpD2;
return(*x->name).f2;}case 8U: _LL5: _tmpD1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*x=_tmpD1;
return(*x->name).f2;}case 0U: _LL7: _tmpD0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmpD0;
return(*x->name).f2;}case 1U: _LL9: _tmpCF=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpCE)->f1;_LLA: {struct Cyc_Absyn_Fndecl*x=_tmpCF;
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
# 925
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
struct _fat_ptr*dname;
for(0;decls != 0;decls=decls->tl){
dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 932
return 0;}
# 935
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
struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Absyn_No_throw_att_val={27U};static char _tmpDA[3U]="f0";
# 955
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 957
static struct _fat_ptr f0={_tmpDA,_tmpDA,_tmpDA + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)field_names_v,(void*)field_names_v,(void*)(field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmpD9=(unsigned)(i + 1);struct _fat_ptr**_tmpD8=_cycalloc(_check_times(_tmpD9,sizeof(struct _fat_ptr*)));({{unsigned _tmp152=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp152;++ j){
# 964
j < fsz?_tmpD8[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_tmp1F0=({struct _fat_ptr*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _fat_ptr _tmp1EF=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD6=({struct Cyc_Int_pa_PrintArg_struct _tmp153;_tmp153.tag=1U,_tmp153.f1=(unsigned long)((int)j);_tmp153;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({struct _fat_ptr _tmp1EE=({const char*_tmpD5="f%d";_tag_fat(_tmpD5,sizeof(char),4U);});Cyc_aprintf(_tmp1EE,_tag_fat(_tmpD4,sizeof(void*),1U));});});*_tmpD7=_tmp1EF;});_tmpD7;});_tmpD8[j]=_tmp1F0;});}}0;});_tag_fat(_tmpD8,sizeof(struct _fat_ptr*),_tmpD9);});
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple16{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 968
struct _tuple16 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpDB=info;struct _tuple0*_tmpDD;enum Cyc_Absyn_AggrKind _tmpDC;struct _tuple0*_tmpDF;enum Cyc_Absyn_AggrKind _tmpDE;if((_tmpDB.UnknownAggr).tag == 1){_LL1: _tmpDE=((_tmpDB.UnknownAggr).val).f1;_tmpDF=((_tmpDB.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind ak=_tmpDE;struct _tuple0*n=_tmpDF;
return({struct _tuple16 _tmp154;_tmp154.f1=ak,_tmp154.f2=n;_tmp154;});}}else{_LL3: _tmpDC=(*(_tmpDB.KnownAggr).val)->kind;_tmpDD=(*(_tmpDB.KnownAggr).val)->name;_LL4: {enum Cyc_Absyn_AggrKind k=_tmpDC;struct _tuple0*n=_tmpDD;
return({struct _tuple16 _tmp155;_tmp155.f1=k,_tmp155.f2=n;_tmp155;});}}_LL0:;}
# 974
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpE0=info;struct Cyc_Absyn_Aggrdecl*_tmpE1;if((_tmpE0.UnknownAggr).tag == 1){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp156;_tmp156.tag=0U,({struct _fat_ptr _tmp1F1=({const char*_tmpE4="unchecked aggrdecl";_tag_fat(_tmpE4,sizeof(char),19U);});_tmp156.f1=_tmp1F1;});_tmp156;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE2,sizeof(void*),1U));});}else{_LL3: _tmpE1=*(_tmpE0.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmpE1;
return ad;}}_LL0:;}
# 980
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmpE5=Cyc_Absyn_compress(t);void*_stmttmp9=_tmpE5;void*_tmpE6=_stmttmp9;union Cyc_Absyn_AggrInfo _tmpE7;struct Cyc_List_List*_tmpE8;switch(*((int*)_tmpE6)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE6)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpE8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE6)->f2;_LL2: {struct Cyc_List_List*fs=_tmpE8;
# 983
if(fs == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause == 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE6)->f1)->tag == 20U){_LL3: _tmpE7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE6)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpE7;
# 986
union Cyc_Absyn_AggrInfo _tmpE9=info;int _tmpEB;enum Cyc_Absyn_AggrKind _tmpEA;enum Cyc_Absyn_AggrKind _tmpEC;struct Cyc_Absyn_Aggrdecl*_tmpED;if((_tmpE9.KnownAggr).tag == 2){_LL8: _tmpED=*(_tmpE9.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpED;
# 988
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpEE=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpEE;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpEF=impl->fields;struct Cyc_List_List*fields=_tmpEF;
if(fields == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause == 0;}}}}else{if(((_tmpE9.UnknownAggr).val).f3 == 0){_LLA: _tmpEC=((_tmpE9.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpEC;
return(int)k == (int)Cyc_Absyn_UnionA;}}else{_LLC: _tmpEA=((_tmpE9.UnknownAggr).val).f1;_tmpEB=(int)(((_tmpE9.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpEA;int b=_tmpEB;
return(int)k == (int)1U && !b;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 997
 return 0;}_LL0:;}
# 1000
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmpF0=Cyc_Absyn_compress(t);void*_stmttmpA=_tmpF0;void*_tmpF1=_stmttmpA;union Cyc_Absyn_AggrInfo _tmpF2;struct Cyc_List_List*_tmpF3;switch(*((int*)_tmpF1)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF1)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpF3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF1)->f2;_LL2: {struct Cyc_List_List*fs=_tmpF3;
# 1003
if(fs == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause != 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF1)->f1)->tag == 20U){_LL3: _tmpF2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF1)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpF2;
# 1006
union Cyc_Absyn_AggrInfo _tmpF4=info;int _tmpF6;enum Cyc_Absyn_AggrKind _tmpF5;enum Cyc_Absyn_AggrKind _tmpF7;struct Cyc_Absyn_Aggrdecl*_tmpF8;if((_tmpF4.KnownAggr).tag == 2){_LL8: _tmpF8=*(_tmpF4.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF8;
# 1008
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpF9=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpF9;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpFA=impl->fields;struct Cyc_List_List*fields=_tmpFA;
if(fields == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause != 0;}}}}else{if(((_tmpF4.UnknownAggr).val).f3 == 0){_LLA: _tmpF7=((_tmpF4.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpF7;
return 0;}}else{_LLC: _tmpF5=((_tmpF4.UnknownAggr).val).f1;_tmpF6=(int)(((_tmpF4.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpF5;int b=_tmpF6;
return 0;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1017
 return 0;}_LL0:;}
# 1021
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmpFB=b;struct Cyc_Absyn_Fndecl*_tmpFC;struct Cyc_Absyn_Vardecl*_tmpFD;struct Cyc_Absyn_Vardecl*_tmpFE;struct Cyc_Absyn_Vardecl*_tmpFF;struct Cyc_Absyn_Vardecl*_tmp100;struct _tuple0*_tmp101;switch(*((int*)_tmpFB)){case 0U: _LL1: _tmp101=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpFB)->f1;_LL2: {struct _tuple0*qv=_tmp101;
return qv;}case 1U: _LL3: _tmp100=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpFB)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp100;
_tmpFF=vd;goto _LL6;}case 3U: _LL5: _tmpFF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpFB)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpFF;
_tmpFE=vd;goto _LL8;}case 4U: _LL7: _tmpFE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpFB)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpFE;
_tmpFD=vd;goto _LLA;}case 5U: _LL9: _tmpFD=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpFB)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpFD;
return vd->name;}default: _LLB: _tmpFC=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpFB)->f1;_LLC: {struct Cyc_Absyn_Fndecl*fd=_tmpFC;
return fd->name;}}_LL0:;}
# 1032
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp103=({struct Cyc_Warn_String_Warn_Warg_struct _tmp157;_tmp157.tag=0U,({struct _fat_ptr _tmp1F2=({const char*_tmp104="designator list not of length 1";_tag_fat(_tmp104,sizeof(char),32U);});_tmp157.f1=_tmp1F2;});_tmp157;});void*_tmp102[1U];_tmp102[0]=& _tmp103;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp102,sizeof(void*),1U));});{
void*_tmp105=(void*)ds->hd;void*_stmttmpB=_tmp105;void*_tmp106=_stmttmpB;struct _fat_ptr*_tmp107;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp106)->tag == 1U){_LL1: _tmp107=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp106)->f1;_LL2: {struct _fat_ptr*f=_tmp107;
return f;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp109=({struct Cyc_Warn_String_Warn_Warg_struct _tmp158;_tmp158.tag=0U,({struct _fat_ptr _tmp1F3=({const char*_tmp10A="array designator in struct";_tag_fat(_tmp10A,sizeof(char),27U);});_tmp158.f1=_tmp1F3;});_tmp158;});void*_tmp108[1U];_tmp108[0]=& _tmp109;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp108,sizeof(void*),1U));});}_LL0:;}}
# 1041
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp10B=Cyc_Absyn_compress(t);void*_stmttmpC=_tmp10B;void*_tmp10C=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10C)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1052
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10D=e->r;void*_stmttmpD=_tmp10D;void*_tmp10E=_stmttmpD;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;switch(*((int*)_tmp10E)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 1061
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp132=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_tmp133=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_tmp134=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10E)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp132;struct Cyc_Absyn_Exp*e2=_tmp133;struct Cyc_Absyn_Exp*e3=_tmp134;
# 1071
if(!Cyc_Absyn_no_side_effects_exp(e1))return 0;
_tmp130=e2;_tmp131=e3;goto _LL20;}case 27U: _LL1F: _tmp130=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_tmp131=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10E)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp130;struct Cyc_Absyn_Exp*e2=_tmp131;
_tmp12E=e1;_tmp12F=e2;goto _LL22;}case 7U: _LL21: _tmp12E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_tmp12F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp12E;struct Cyc_Absyn_Exp*e2=_tmp12F;
_tmp12C=e1;_tmp12D=e2;goto _LL24;}case 8U: _LL23: _tmp12C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_tmp12D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp12C;struct Cyc_Absyn_Exp*e2=_tmp12D;
_tmp12A=e1;_tmp12B=e2;goto _LL26;}case 23U: _LL25: _tmp12A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_tmp12B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp12A;struct Cyc_Absyn_Exp*e2=_tmp12B;
_tmp128=e1;_tmp129=e2;goto _LL28;}case 9U: _LL27: _tmp128=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_tmp129=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp128;struct Cyc_Absyn_Exp*e2=_tmp129;
# 1078
return Cyc_Absyn_no_side_effects_exp(e1)&& Cyc_Absyn_no_side_effects_exp(e2);}case 41U: _LL29: _tmp127=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp127;
# 1080
_tmp126=e;goto _LL2C;}case 38U: _LL2B: _tmp126=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp126;
_tmp125=e;goto _LL2E;}case 11U: _LL2D: _tmp125=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp125;
_tmp124=e;goto _LL30;}case 12U: _LL2F: _tmp124=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp124;
_tmp123=e;goto _LL32;}case 13U: _LL31: _tmp123=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp123;
_tmp122=e;goto _LL34;}case 14U: _LL33: _tmp122=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp122;
_tmp121=e;goto _LL36;}case 18U: _LL35: _tmp121=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp121;
_tmp120=e;goto _LL38;}case 20U: _LL37: _tmp120=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp120;
_tmp11F=e;goto _LL3A;}case 21U: _LL39: _tmp11F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp11F;
_tmp11E=e;goto _LL3C;}case 22U: _LL3B: _tmp11E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp11E;
_tmp11D=e;goto _LL3E;}case 28U: _LL3D: _tmp11D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp11D;
_tmp11C=e;goto _LL40;}case 42U: _LL3F: _tmp11C=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp11C;
_tmp11B=e;goto _LL42;}case 15U: _LL41: _tmp11B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL42: {struct Cyc_Absyn_Exp*e=_tmp11B;
return Cyc_Absyn_no_side_effects_exp(e);}case 34U: _LL43: _tmp119=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10E)->f1).rgn;_tmp11A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10E)->f1).num_elts;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp119;struct Cyc_Absyn_Exp*e=_tmp11A;
# 1094
_tmp117=eopt;_tmp118=e;goto _LL46;}case 16U: _LL45: _tmp117=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_tmp118=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL46: {struct Cyc_Absyn_Exp*eopt=_tmp117;struct Cyc_Absyn_Exp*e=_tmp118;
# 1096
if(!Cyc_Absyn_no_side_effects_exp(e))return 0;
if(eopt != 0)return Cyc_Absyn_no_side_effects_exp(eopt);
return 1;}case 3U: _LL47: _tmp116=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL48: {struct Cyc_List_List*es=_tmp116;
# 1100
_tmp115=es;goto _LL4A;}case 31U: _LL49: _tmp115=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL4A: {struct Cyc_List_List*es=_tmp115;
_tmp114=es;goto _LL4C;}case 24U: _LL4B: _tmp114=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL4C: {struct Cyc_List_List*es=_tmp114;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Absyn_no_side_effects_exp,es);}case 36U: _LL4D: _tmp113=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL4E: {struct Cyc_List_List*dles=_tmp113;
# 1104
_tmp112=dles;goto _LL50;}case 29U: _LL4F: _tmp112=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10E)->f3;_LL50: {struct Cyc_List_List*dles=_tmp112;
_tmp111=dles;goto _LL52;}case 30U: _LL51: _tmp111=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL52: {struct Cyc_List_List*dles=_tmp111;
_tmp110=dles;goto _LL54;}case 26U: _LL53: _tmp110=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10E)->f1;_LL54: {struct Cyc_List_List*dles=_tmp110;
_tmp10F=dles;goto _LL56;}default: _LL55: _tmp10F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10E)->f2;_LL56: {struct Cyc_List_List*dles=_tmp10F;
# 1109
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Absyn_no_side_effects_exp((*((struct _tuple13*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}
# 1116
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
