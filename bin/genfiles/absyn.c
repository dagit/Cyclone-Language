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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr);
# 496
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 498
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr);
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 664 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 875
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 891
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 897
void*Cyc_Absyn_compress_kb(void*);
# 901
void*Cyc_Absyn_compress(void*);
# 912
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 915
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 920
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 922
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 924
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 927
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 931
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 933
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 948
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 958
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one (void);
# 964
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 966
void*Cyc_Absyn_starb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 968
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 970
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 972
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 987
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 989
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 992
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1060
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);
# 1064
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned);
# 1086
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1095
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1131
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1133
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1135
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1138
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1146
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
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _fat_ptr*)ss1->hd,(struct _fat_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;}}
# 43
if(ss2 != 0)return - 1;
return 0;}
# 46
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 50
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;struct _fat_ptr*_tmp2;union Cyc_Absyn_Nmspace _tmp1;_LL1: _tmp1=_tmp0->f1;_tmp2=_tmp0->f2;_LL2: {union Cyc_Absyn_Nmspace n1=_tmp1;struct _fat_ptr*v1=_tmp2;
struct _tuple0*_tmp3=q2;struct _fat_ptr*_tmp5;union Cyc_Absyn_Nmspace _tmp4;_LL4: _tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_LL5: {union Cyc_Absyn_Nmspace n2=_tmp4;struct _fat_ptr*v2=_tmp5;
int i=Cyc_strptrcmp(v1,v2);
if(i != 0)return i;{
struct _tuple12 _tmp6=({struct _tuple12 _tmp136;_tmp136.f1=n1,_tmp136.f2=n2;_tmp136;});struct _tuple12 _stmttmp0=_tmp6;struct _tuple12 _tmp7=_stmttmp0;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpC;switch(((_tmp7.f1).Abs_n).tag){case 4U: if(((_tmp7.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 62
 return - 1;}case 1U: switch(((_tmp7.f2).Loc_n).tag){case 1U: _LL9: _tmpC=((_tmp7.f1).Rel_n).val;_tmpD=((_tmp7.f2).Rel_n).val;_LLA: {struct Cyc_List_List*x1=_tmpC;struct Cyc_List_List*x2=_tmpD;
# 58
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;default: _LL13: _LL14:
# 64
 return - 1;}case 2U: switch(((_tmp7.f2).Rel_n).tag){case 2U: _LLB: _tmpA=((_tmp7.f1).Abs_n).val;_tmpB=((_tmp7.f2).Abs_n).val;_LLC: {struct Cyc_List_List*x1=_tmpA;struct Cyc_List_List*x2=_tmpB;
# 59
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 66
 return - 1;}default: switch(((_tmp7.f2).Rel_n).tag){case 3U: _LLD: _tmp8=((_tmp7.f1).C_n).val;_tmp9=((_tmp7.f2).C_n).val;_LLE: {struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 60
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: _LL11: _LL12:
# 63
 return 1;case 1U: _LL15: _LL16:
# 65
 return 1;default: _LL19: _LL1A:
# 67
 return 1;}}_LL6:;}}}}}
# 70
int Cyc_Absyn_hash_qvar(struct _tuple0*q){return Cyc_Hashtable_hash_string(*(*q).f2);}
# 72
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 78
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
return C_scope?({union Cyc_Absyn_Nmspace _tmp137;(_tmp137.C_n).tag=3U,(_tmp137.C_n).val=x;_tmp137;}):({union Cyc_Absyn_Nmspace _tmp138;(_tmp138.Abs_n).tag=2U,(_tmp138.Abs_n).val=x;_tmp138;});}
# 82
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp139;(_tmp139.Rel_n).tag=1U,(_tmp139.Rel_n).val=x;_tmp139;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 85
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmpE;union Cyc_Absyn_Nmspace _tmpF=_stmttmp1;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 94
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 98
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 102
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp13A;_tmp13A.print_const=0,_tmp13A.q_volatile=0,_tmp13A.q_restrict=0,_tmp13A.real_const=0,_tmp13A.loc=loc;_tmp13A;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp13B;_tmp13B.print_const=1,_tmp13B.q_volatile=0,_tmp13B.q_restrict=0,_tmp13B.real_const=1,_tmp13B.loc=loc;_tmp13B;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp13C;_tmp13C.print_const=x.print_const || y.print_const,_tmp13C.q_volatile=
x.q_volatile || y.q_volatile,_tmp13C.q_restrict=
x.q_restrict || y.q_restrict,_tmp13C.real_const=
x.real_const || y.real_const,({
unsigned _tmp15A=Cyc_Position_segment_join(x.loc,y.loc);_tmp13C.loc=_tmp15A;});_tmp13C;});}
# 111
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 117
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 119
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp13D;(_tmp13D.UnknownDatatype).tag=1U,(_tmp13D.UnknownDatatype).val=udi;_tmp13D;});}
# 122
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp13E;(_tmp13E.KnownDatatype).tag=2U,(_tmp13E.KnownDatatype).val=d;_tmp13E;});}
# 125
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp13F;(_tmp13F.UnknownDatatypefield).tag=1U,(_tmp13F.UnknownDatatypefield).val=s;_tmp13F;});}
# 128
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp140;(_tmp140.KnownDatatypefield).tag=2U,((_tmp140.KnownDatatypefield).val).f1=dd,((_tmp140.KnownDatatypefield).val).f2=df;_tmp140;});}
# 131
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp141;(_tmp141.UnknownAggr).tag=1U,((_tmp141.UnknownAggr).val).f1=ak,((_tmp141.UnknownAggr).val).f2=n,((_tmp141.UnknownAggr).val).f3=tagged;_tmp141;});}
# 134
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp142;(_tmp142.KnownAggr).tag=2U,(_tmp142.KnownAggr).val=ad;_tmp142;});}
# 137
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp12;void**_tmp13;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp13=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8: {void**k2=_tmp13;
# 142
_tmp12=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 141
 return k;}else{_LL9: _tmp12=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA: {void**k2=_tmp12;
# 144
({void*_tmp15B=Cyc_Absyn_compress_kb(*k2);*k2=_tmp15B;});
return*k2;}}}_LL0:;}
# 148
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_stmttmp2=_tmp14;void*_tmp15=_stmttmp2;struct Cyc_Absyn_Kind*_tmp17;struct Cyc_Core_Opt**_tmp16;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp19;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp19=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp19;
return k;}case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp18;
_tmp16=f;_tmp17=& Cyc_Tcutil_bk;goto _LL6;}default: _LL5: _tmp16=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6: {struct Cyc_Core_Opt**f=_tmp16;struct Cyc_Absyn_Kind*k=_tmp17;
# 153
({struct Cyc_Core_Opt*_tmp15D=({struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));({void*_tmp15C=Cyc_Tcutil_kind_to_bound(k);_tmp1A->v=_tmp15C;});_tmp1A;});*f=_tmp15D;});
return k;}}_LL0:;}
# 159
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp15E=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp15E;});_tmp1B;});}
# 162
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 178 "absyn.cyc"
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
# 204
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0U,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 206
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
# 219
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp23=0U;({void*_tmp15F=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=15U,_tmp24->f1=n,_tmp24->f2=d;_tmp24;});Cyc_Absyn_app_type(_tmp15F,_tag_fat(_tmp23,sizeof(void*),0U));});});}
# 222
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp25=0U;({void*_tmp160=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=16U,_tmp26->f1=fs;_tmp26;});Cyc_Absyn_app_type(_tmp160,_tag_fat(_tmp25,sizeof(void*),0U));});});}
# 225
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp27=0U;({void*_tmp161=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=17U,_tmp28->f1=s,_tmp28->f2=k;_tmp28;});Cyc_Absyn_app_type(_tmp161,_tag_fat(_tmp27,sizeof(void*),0U));});});}
# 228
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp162=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=18U,_tmp29->f1=di;_tmp29;});_tmp2A->f1=_tmp162;}),_tmp2A->f2=args;_tmp2A;});}
# 231
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp163=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=di;_tmp2B;});_tmp2C->f1=_tmp163;}),_tmp2C->f2=args;_tmp2C;});}
# 234
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp164=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=20U,_tmp2D->f1=ai;_tmp2D;});_tmp2E->f1=_tmp164;}),_tmp2E->f2=args;_tmp2E;});}
# 238
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,_tmp2F->f1=x;_tmp2F;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp30=i;switch(_tmp30){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp32=({struct Cyc_Warn_String_Warn_Warg_struct _tmp145;_tmp145.tag=0U,({struct _fat_ptr _tmp165=({const char*_tmp36="gen_float_type(";_tag_fat(_tmp36,sizeof(char),16U);});_tmp145.f1=_tmp165;});_tmp145;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp33=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp144;_tmp144.tag=12U,_tmp144.f1=(int)i;_tmp144;});struct Cyc_Warn_String_Warn_Warg_struct _tmp34=({struct Cyc_Warn_String_Warn_Warg_struct _tmp143;_tmp143.tag=0U,({struct _fat_ptr _tmp166=({const char*_tmp35=")";_tag_fat(_tmp35,sizeof(char),2U);});_tmp143.f1=_tmp166;});_tmp143;});void*_tmp31[3U];_tmp31[0]=& _tmp32,_tmp31[1]=& _tmp33,_tmp31[2]=& _tmp34;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp31,sizeof(void*),3U));});}_LL0:;}
# 247
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp37=sn;switch(_tmp37){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 250
enum Cyc_Absyn_Size_of _tmp38=sz;switch(_tmp38){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 259
enum Cyc_Absyn_Size_of _tmp39=sz;switch(_tmp39){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 267
 goto _LL8;default: _LL7: _LL8: {
# 269
enum Cyc_Absyn_Size_of _tmp3A=sz;switch(_tmp3A){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 280
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 283
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 288
void*Cyc_Absyn_bounds_one (void){
static void*bone=0;
void*_tmp3B=bone;void*b=_tmp3B;
if(b == 0){
b=Cyc_Absyn_thin_bounds_int(1U);
bone=b;}
# 295
return b;}
# 301
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 304
void*Cyc_Absyn_wchar_type (void){
int _tmp3C=Sizeof_wchar_t;switch(_tmp3C){case 1U: _LL1: _LL2:
# 315 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 319
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3D[4U]="exn";
# 324
static struct _fat_ptr Cyc_Absyn_exn_str={_tmp3D,_tmp3D,_tmp3D + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp44[15U]="Null_Exception";static char _tmp45[13U]="Array_bounds";static char _tmp46[16U]="Match_Exception";static char _tmp47[10U]="Bad_alloc";
# 328
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void){
static struct _fat_ptr builtin_exns[4U]={{_tmp44,_tmp44,_tmp44 + 15U},{_tmp45,_tmp45,_tmp45 + 13U},{_tmp46,_tmp46,_tmp46 + 16U},{_tmp47,_tmp47,_tmp47 + 10U}};
# 331
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp41=_cycalloc(sizeof(*_tmp41));({struct Cyc_Absyn_Datatypefield*_tmp169=({struct Cyc_Absyn_Datatypefield*_tmp40=_cycalloc(sizeof(*_tmp40));({struct _tuple0*_tmp168=({struct _tuple0*_tmp3F=_cycalloc(sizeof(*_tmp3F));((_tmp3F->f1).Abs_n).tag=2U,((_tmp3F->f1).Abs_n).val=0,({
struct _fat_ptr*_tmp167=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=*((struct _fat_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _fat_ptr),i));_tmp3E;});_tmp3F->f2=_tmp167;});_tmp3F;});
# 335
_tmp40->name=_tmp168;}),_tmp40->typs=0,_tmp40->loc=0U,_tmp40->sc=Cyc_Absyn_Extern;_tmp40;});_tmp41->hd=_tmp169;}),_tmp41->tl=tufs;_tmp41;});}}
# 339
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->sc=Cyc_Absyn_Extern,_tmp43->name=Cyc_Absyn_exn_name,_tmp43->tvs=0,({struct Cyc_Core_Opt*_tmp16A=({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=tufs;_tmp42;});_tmp43->fields=_tmp16A;}),_tmp43->is_extensible=1;_tmp43;});}
# 341
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 344
void*Cyc_Absyn_exn_type (void){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp146;(_tmp146.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp16C=({struct Cyc_Absyn_Datatypedecl**_tmp48=_cycalloc(sizeof(*_tmp48));({struct Cyc_Absyn_Datatypedecl*_tmp16B=Cyc_Absyn_exn_tud();*_tmp48=_tmp16B;});_tmp48;});(_tmp146.KnownDatatype).val=_tmp16C;});_tmp146;}),0);
exn_typ=({void*_tmp16F=(void*)_check_null(eopt);void*_tmp16E=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp16D=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp16F,_tmp16E,_tmp16D,Cyc_Absyn_false_type);});}
# 351
return(void*)_check_null(exn_typ);}
# 354
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4B=_cycalloc(sizeof(*_tmp4B));({union Cyc_Absyn_Nmspace _tmp172=Cyc_Absyn_Abs_n(0,0);_tmp4B->f1=_tmp172;}),({struct _fat_ptr*_tmp171=({struct _fat_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct _fat_ptr _tmp170=({const char*_tmp49="PrintArg";_tag_fat(_tmp49,sizeof(char),9U);});*_tmp4A=_tmp170;});_tmp4A;});_tmp4B->f2=_tmp171;});_tmp4B;});
return(struct _tuple0*)_check_null(q);}
# 360
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4E=_cycalloc(sizeof(*_tmp4E));({union Cyc_Absyn_Nmspace _tmp175=Cyc_Absyn_Abs_n(0,0);_tmp4E->f1=_tmp175;}),({struct _fat_ptr*_tmp174=({struct _fat_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _fat_ptr _tmp173=({const char*_tmp4C="ScanfArg";_tag_fat(_tmp4C,sizeof(char),9U);});*_tmp4D=_tmp173;});_tmp4D;});_tmp4E->f2=_tmp174;});_tmp4E;});
return(struct _tuple0*)_check_null(q);}
# 372
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp54=_cycalloc(sizeof(*_tmp54));({union Cyc_Absyn_Nmspace _tmp17A=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({struct _fat_ptr*_tmp179=({struct _fat_ptr*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _fat_ptr _tmp178=({const char*_tmp4F="Core";_tag_fat(_tmp4F,sizeof(char),5U);});*_tmp50=_tmp178;});_tmp50;});_tmp51->hd=_tmp179;}),_tmp51->tl=0;_tmp51;}),0);_tmp54->f1=_tmp17A;}),({struct _fat_ptr*_tmp177=({struct _fat_ptr*_tmp53=_cycalloc(sizeof(*_tmp53));({struct _fat_ptr _tmp176=({const char*_tmp52="unique_region";_tag_fat(_tmp52,sizeof(char),14U);});*_tmp53=_tmp176;});_tmp53;});_tmp54->f2=_tmp177;});_tmp54;});
return(struct _tuple0*)_check_null(q);}
# 378
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void){
void*_tmp55=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);void*t=_tmp55;
# 381
return({struct Cyc_Absyn_Exp*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->topt=t,_tmp59->loc=0U,_tmp59->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp17F=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=1U,({void*_tmp17E=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp17D=({struct Cyc_Absyn_Vardecl*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->sc=Cyc_Absyn_Extern,({
struct _tuple0*_tmp17C=Cyc_Absyn_uniquergn_qvar();_tmp56->name=_tmp17C;}),_tmp56->varloc=0U,({
struct Cyc_Absyn_Tqual _tmp17B=Cyc_Absyn_empty_tqual(0U);_tmp56->tq=_tmp17B;}),_tmp56->type=t,_tmp56->initializer=0,_tmp56->rgn=0,_tmp56->attributes=0,_tmp56->escapes=0,_tmp56->is_proto=0;_tmp56;});
# 382
_tmp57->f1=_tmp17D;});_tmp57;});_tmp58->f1=_tmp17E;});_tmp58;});_tmp59->r=_tmp17F;});_tmp59;});}
# 390
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=3U,_tmp5A->f1=s;_tmp5A;});}
# 394
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp147;_tmp147.elt_type=t,_tmp147.elt_tq=tq,(_tmp147.ptr_atts).rgn=r,(_tmp147.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp147.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp147.ptr_atts).zero_term=zt,(_tmp147.ptr_atts).ptrloc=0;_tmp147;}));}
# 399
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp148;_tmp148.elt_type=t,_tmp148.elt_tq=tq,(_tmp148.ptr_atts).rgn=r,(_tmp148.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp148.ptr_atts).bounds=b,(_tmp148.ptr_atts).zero_term=zt,(_tmp148.ptr_atts).ptrloc=0;_tmp148;}));}
# 404
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp149;_tmp149.elt_type=t,_tmp149.elt_tq=tq,(_tmp149.ptr_atts).rgn=r,(_tmp149.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp149.ptr_atts).bounds=b,(_tmp149.ptr_atts).zero_term=zt,(_tmp149.ptr_atts).ptrloc=0;_tmp149;}));}
# 409
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp183=t;void*_tmp182=r;struct Cyc_Absyn_Tqual _tmp181=tq;void*_tmp180=Cyc_Absyn_bounds_one();Cyc_Absyn_starb_type(_tmp183,_tmp182,_tmp181,_tmp180,zeroterm);});}
# 412
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}
# 415
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp187=t;void*_tmp186=r;struct Cyc_Absyn_Tqual _tmp185=tq;void*_tmp184=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp187,_tmp186,_tmp185,_tmp184,zeroterm);});}
# 418
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp18B=Cyc_Absyn_char_type;void*_tmp18A=rgn;struct Cyc_Absyn_Tqual _tmp189=Cyc_Absyn_empty_tqual(0U);void*_tmp188=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp18B,_tmp18A,_tmp189,_tmp188,Cyc_Absyn_true_type);});}
# 421
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp18F=Cyc_Absyn_char_type;void*_tmp18E=rgn;struct Cyc_Absyn_Tqual _tmp18D=Cyc_Absyn_const_tqual(0U);void*_tmp18C=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp18F,_tmp18E,_tmp18D,_tmp18C,Cyc_Absyn_true_type);});}
# 425
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 427
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=4U,(_tmp5B->f1).elt_type=elt_type,(_tmp5B->f1).tq=tq,(_tmp5B->f1).num_elts=num_elts,(_tmp5B->f1).zero_term=zero_term,(_tmp5B->f1).zt_loc=ztloc;_tmp5B;});}
# 430
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=11U,_tmp5C->f1=e;_tmp5C;});}
# 435
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 437
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=8U,_tmp5D->f1=n,_tmp5D->f2=args,_tmp5D->f3=d,_tmp5D->f4=defn;_tmp5D;});}
# 443
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=0U,({void*_tmp192=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=20U,({union Cyc_Absyn_AggrInfo _tmp191=({enum Cyc_Absyn_AggrKind _tmp190=k;Cyc_Absyn_UnknownAggr(_tmp190,({struct _tuple0*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->f1=Cyc_Absyn_rel_ns_null,_tmp5E->f2=name;_tmp5E;}),0);});_tmp5F->f1=_tmp191;});_tmp5F;});_tmp60->f1=_tmp192;}),_tmp60->f2=0;_tmp60;});}
# 446
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 449
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 452
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 457
void*Cyc_Absyn_compress(void*t){
void*_tmp61=t;void*_tmp62;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*_tmp64;void**_tmp65;void**_tmp66;switch(*((int*)_tmp61)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp66=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f2;_LL8: {void**t2opt_ref=_tmp66;
# 463
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Absyn_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp61)->f4 == 0){_LL3: _LL4:
# 460
 return t;}else{_LL5: _tmp65=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp61)->f4;_LL6: {void**topt_ref=_tmp65;
_tmp66=topt_ref;goto _LL8;}}case 9U: _LL9: _tmp64=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp61)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp64;
# 469
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp67=e->r;void*_stmttmp3=_tmp67;void*_tmp68=_stmttmp3;void*_tmp69;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp68)->tag == 39U){_LL12: _tmp69=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp68)->f1;_LL13: {void*t2=_tmp69;
return Cyc_Absyn_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11U: _LLB: _tmp63=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp61)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp63;
# 475
void*_tmp6A=e->topt;void*t2=_tmp6A;
return t2 != 0?t2: t;}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp61)->f2 != 0){_LLD: _tmp62=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp61)->f2;_LLE: {void*t=_tmp62;
return Cyc_Absyn_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 483
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp14A;(_tmp14A.Char_c).tag=2U,((_tmp14A.Char_c).val).f1=sn,((_tmp14A.Char_c).val).f2=c;_tmp14A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp14B;(_tmp14B.Wchar_c).tag=3U,(_tmp14B.Wchar_c).val=s;_tmp14B;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp14C;(_tmp14C.Short_c).tag=4U,((_tmp14C.Short_c).val).f1=sn,((_tmp14C.Short_c).val).f2=s;_tmp14C;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp14D;(_tmp14D.Int_c).tag=5U,((_tmp14D.Int_c).val).f1=sn,((_tmp14D.Int_c).val).f2=i;_tmp14D;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp14E;(_tmp14E.LongLong_c).tag=6U,((_tmp14E.LongLong_c).val).f1=sn,((_tmp14E.LongLong_c).val).f2=l;_tmp14E;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp14F;(_tmp14F.Float_c).tag=7U,((_tmp14F.Float_c).val).f1=s,((_tmp14F.Float_c).val).f2=i;_tmp14F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp150;(_tmp150.String_c).tag=8U,(_tmp150.String_c).val=s;_tmp150;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp151;(_tmp151.Wstring_c).tag=9U,(_tmp151.Wstring_c).val=s;_tmp151;});}
# 493
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->topt=0,_tmp6B->r=r,_tmp6B->loc=loc,_tmp6B->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp6B;});}
# 496
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp193=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=16U,_tmp6C->f1=rgn_handle,_tmp6C->f2=e;_tmp6C;});Cyc_Absyn_new_exp(_tmp193,loc);});}
# 499
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp6D=_cycalloc(sizeof(*_tmp6D));*_tmp6D=*e;_tmp6D;});}
# 502
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp194=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=0U,_tmp6E->f1=c;_tmp6E;});Cyc_Absyn_new_exp(_tmp194,loc);});}
# 505
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 509
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp195=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp195,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 517
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 524
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp196=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp196,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp197=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp197,loc);});}
# 531
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp198=f(s);Cyc_Absyn_const_exp(_tmp198,loc);});}
# 534
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 538
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp19A=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,({void*_tmp199=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=0U,_tmp6F->f1=q;_tmp6F;});_tmp70->f1=_tmp199;});_tmp70;});Cyc_Absyn_new_exp(_tmp19A,loc);});}
# 541
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp19B=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=1U,_tmp71->f1=b;_tmp71;});Cyc_Absyn_new_exp(_tmp19B,loc);});}
# 545
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 548
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=2U,_tmp72->f1=s;_tmp72;});Cyc_Absyn_new_exp(_tmp19C,loc);});}
# 551
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp19D=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=3U,_tmp73->f1=p,_tmp73->f2=es;_tmp73;});Cyc_Absyn_new_exp(_tmp19D,loc);});}
# 554
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp19F=p;struct Cyc_List_List*_tmp19E=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->hd=e,_tmp74->tl=0;_tmp74;});Cyc_Absyn_primop_exp(_tmp19F,_tmp19E,loc);});}
# 557
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1A2=p;struct Cyc_List_List*_tmp1A1=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->hd=e1,({struct Cyc_List_List*_tmp1A0=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->hd=e2,_tmp75->tl=0;_tmp75;});_tmp76->tl=_tmp1A0;});_tmp76;});Cyc_Absyn_primop_exp(_tmp1A2,_tmp1A1,loc);});}
# 560
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A3=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=35U,_tmp77->f1=e1,_tmp77->f2=e2;_tmp77;});Cyc_Absyn_new_exp(_tmp1A3,loc);});}
# 563
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A4=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=4U,_tmp78->f1=e1,_tmp78->f2=popt,_tmp78->f3=e2;_tmp78;});Cyc_Absyn_new_exp(_tmp1A4,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 579
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp1A5=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=5U,_tmp79->f1=e,_tmp79->f2=i;_tmp79;});Cyc_Absyn_new_exp(_tmp1A5,loc);});}
# 582
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp1A6=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=6U,_tmp7A->f1=e1,_tmp7A->f2=e2,_tmp7A->f3=e3;_tmp7A;});Cyc_Absyn_new_exp(_tmp1A6,loc);});}
# 585
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A7=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=7U,_tmp7B->f1=e1,_tmp7B->f2=e2;_tmp7B;});Cyc_Absyn_new_exp(_tmp1A7,loc);});}
# 588
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=8U,_tmp7C->f1=e1,_tmp7C->f2=e2;_tmp7C;});Cyc_Absyn_new_exp(_tmp1A8,loc);});}
# 591
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A9=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=9U,_tmp7D->f1=e1,_tmp7D->f2=e2;_tmp7D;});Cyc_Absyn_new_exp(_tmp1A9,loc);});}
# 594
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=10U,_tmp7E->f1=e,_tmp7E->f2=es,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});Cyc_Absyn_new_exp(_tmp1AA,loc);});}
# 597
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1AB=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=10U,_tmp7F->f1=e,_tmp7F->f2=es,_tmp7F->f3=0,_tmp7F->f4=1;_tmp7F;});Cyc_Absyn_new_exp(_tmp1AB,loc);});}
# 600
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1AC=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=12U,_tmp80->f1=e;_tmp80;});Cyc_Absyn_new_exp(_tmp1AC,loc);});}
# 603
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp1AD=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=13U,_tmp81->f1=e,_tmp81->f2=ts;_tmp81;});Cyc_Absyn_new_exp(_tmp1AD,loc);});}
# 606
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp1AE=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=14U,_tmp82->f1=t,_tmp82->f2=e,_tmp82->f3=user_cast,_tmp82->f4=c;_tmp82;});Cyc_Absyn_new_exp(_tmp1AE,loc);});}
# 609
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1AF=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=11U,_tmp83->f1=e,_tmp83->f2=0;_tmp83;});Cyc_Absyn_new_exp(_tmp1AF,loc);});}
# 612
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B0=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=11U,_tmp84->f1=e,_tmp84->f2=1;_tmp84;});Cyc_Absyn_new_exp(_tmp1B0,loc);});}
# 615
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B1=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=15U,_tmp85->f1=e;_tmp85;});Cyc_Absyn_new_exp(_tmp1B1,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp1B2=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=17U,_tmp86->f1=t;_tmp86;});Cyc_Absyn_new_exp(_tmp1B2,loc);});}
# 619
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B3=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=18U,_tmp87->f1=e;_tmp87;});Cyc_Absyn_new_exp(_tmp1B3,loc);});}
# 622
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp1B4=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=19U,_tmp88->f1=t,_tmp88->f2=ofs;_tmp88;});Cyc_Absyn_new_exp(_tmp1B4,loc);});}
# 625
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B5=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=20U,_tmp89->f1=e;_tmp89;});Cyc_Absyn_new_exp(_tmp1B5,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1B6=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=21U,_tmp8A->f1=e,_tmp8A->f2=n,_tmp8A->f3=0,_tmp8A->f4=0;_tmp8A;});Cyc_Absyn_new_exp(_tmp1B6,loc);});}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1B7=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=22U,_tmp8B->f1=e,_tmp8B->f2=n,_tmp8B->f3=0,_tmp8B->f4=0;_tmp8B;});Cyc_Absyn_new_exp(_tmp1B7,loc);});}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1B8=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=23U,_tmp8C->f1=e1,_tmp8C->f2=e2;_tmp8C;});Cyc_Absyn_new_exp(_tmp1B8,loc);});}
# 635
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1B9=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=24U,_tmp8D->f1=es;_tmp8D;});Cyc_Absyn_new_exp(_tmp1B9,loc);});}
# 638
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1BA=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=37U,_tmp8E->f1=s;_tmp8E;});Cyc_Absyn_new_exp(_tmp1BA,loc);});}
# 641
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp1BB=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=39U,_tmp8F->f1=t;_tmp8F;});Cyc_Absyn_new_exp(_tmp1BB,loc);});}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 648
return({void*_tmp1BC=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=40U,_tmp90->f1=volatile_kw,_tmp90->f2=tmpl,_tmp90->f3=outs,_tmp90->f4=ins,_tmp90->f5=clobs;_tmp90;});Cyc_Absyn_new_exp(_tmp1BC,loc);});}
# 650
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1BD=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=41U,_tmp91->f1=e;_tmp91;});Cyc_Absyn_new_exp(_tmp1BD,loc);});}
# 653
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1BE=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=42U,_tmp92->f1=e;_tmp92;});Cyc_Absyn_new_exp(_tmp1BE,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 657
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp93=0;struct Cyc_List_List*dles=_tmp93;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));({struct _tuple13*_tmp1BF=({struct _tuple13*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=0,_tmp94->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp94;});_tmp95->hd=_tmp1BF;}),_tmp95->tl=dles;_tmp95;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return({void*_tmp1C0=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=26U,_tmp96->f1=dles;_tmp96;});Cyc_Absyn_new_exp(_tmp1C0,loc);});}
# 664
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 667
return({void*_tmp1C1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=36U,_tmp97->f1=n,_tmp97->f2=dles;_tmp97;});Cyc_Absyn_new_exp(_tmp1C1,loc);});}
# 670
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->r=s,_tmp98->loc=loc,_tmp98->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp98;});}
# 673
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1C2=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=1U,_tmp99->f1=e;_tmp99;});Cyc_Absyn_new_stmt(_tmp1C2,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1C3=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=3U,_tmp9A->f1=e;_tmp9A;});Cyc_Absyn_new_stmt(_tmp1C3,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1C5=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1C4=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1C5,_tmp1C4,loc);});}}}struct _tuple14{void*f1;void*f2;};
# 686
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple14 _tmp9B=({struct _tuple14 _tmp152;_tmp152.f1=s1->r,_tmp152.f2=s2->r;_tmp152;});struct _tuple14 _stmttmp4=_tmp9B;struct _tuple14 _tmp9C=_stmttmp4;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9C.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9C.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1C6=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=2U,_tmp9D->f1=s1,_tmp9D->f2=s2;_tmp9D;});Cyc_Absyn_new_stmt(_tmp1C6,loc);});}}_LL0:;}
# 693
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp1C7=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=4U,_tmp9E->f1=e,_tmp9E->f2=s1,_tmp9E->f3=s2;_tmp9E;});Cyc_Absyn_new_stmt(_tmp1C7,loc);});}
# 696
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1C9=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=5U,(_tmp9F->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1C8=Cyc_Absyn_skip_stmt(e->loc);(_tmp9F->f1).f2=_tmp1C8;}),_tmp9F->f2=s;_tmp9F;});Cyc_Absyn_new_stmt(_tmp1C9,loc);});}
# 699
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1CC=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=9U,_tmpA0->f1=e1,(_tmpA0->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1CB=Cyc_Absyn_skip_stmt(e3->loc);(_tmpA0->f2).f2=_tmp1CB;}),
(_tmpA0->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1CA=Cyc_Absyn_skip_stmt(e3->loc);(_tmpA0->f3).f2=_tmp1CA;}),_tmpA0->f4=s;_tmpA0;});
# 700
Cyc_Absyn_new_stmt(_tmp1CC,loc);});}
# 704
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1CE=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=14U,_tmpA1->f1=s,(_tmpA1->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1CD=Cyc_Absyn_skip_stmt(e->loc);(_tmpA1->f2).f2=_tmp1CD;});_tmpA1;});Cyc_Absyn_new_stmt(_tmp1CE,loc);});}
# 707
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1CF=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=10U,_tmpA2->f1=e,_tmpA2->f2=scs,_tmpA2->f3=0;_tmpA2;});Cyc_Absyn_new_stmt(_tmp1CF,loc);});}
# 710
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1D0=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=15U,_tmpA3->f1=s,_tmpA3->f2=scs,_tmpA3->f3=0;_tmpA3;});Cyc_Absyn_new_stmt(_tmp1D0,loc);});}
# 713
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp1D1=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=11U,_tmpA4->f1=el,_tmpA4->f2=0;_tmpA4;});Cyc_Absyn_new_stmt(_tmp1D1,loc);});}
# 716
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
return({void*_tmp1D2=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=8U,_tmpA5->f1=lab;_tmpA5;});Cyc_Absyn_new_stmt(_tmp1D2,loc);});}
# 719
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1D3=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=13U,_tmpA6->f1=v,_tmpA6->f2=s;_tmpA6;});Cyc_Absyn_new_stmt(_tmp1D3,loc);});}
# 722
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1D4=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=12U,_tmpA7->f1=d,_tmpA7->f2=s;_tmpA7;});Cyc_Absyn_new_stmt(_tmp1D4,loc);});}
# 725
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1D6=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp1D5=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmpA9->f1=_tmp1D5;});_tmpA9;});Cyc_Absyn_new_decl(_tmp1D6,loc);});
return({void*_tmp1D7=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=12U,_tmpA8->f1=d,_tmpA8->f2=s;_tmpA8;});Cyc_Absyn_new_stmt(_tmp1D7,loc);});}
# 729
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp1D8=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1D8,loc);});}
# 733
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->r=p,_tmpAA->topt=0,_tmpAA->loc=s;_tmpAA;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1D9=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=17U,_tmpAB->f1=e;_tmpAB;});Cyc_Absyn_new_pat(_tmp1D9,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 739
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->r=r,_tmpAC->loc=loc;_tmpAC;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1DA=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=2U,_tmpAD->f1=p,_tmpAD->f2=0,_tmpAD->f3=e,_tmpAD->f4=0;_tmpAD;});Cyc_Absyn_new_decl(_tmp1DA,loc);});}
# 743
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp1DB=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=3U,_tmpAE->f1=vds;_tmpAE;});Cyc_Absyn_new_decl(_tmp1DB,loc);});}
# 746
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp1DC=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=4U,_tmpAF->f1=tv,_tmpAF->f2=vd,_tmpAF->f3=open_exp;_tmpAF;});Cyc_Absyn_new_decl(_tmp1DC,loc);});}
# 749
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 751
return({void*_tmp1DF=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=2U,({struct Cyc_Absyn_Pat*_tmp1DE=({void*_tmp1DD=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,_tmpB0->f1=tv,_tmpB0->f2=vd;_tmpB0;});Cyc_Absyn_new_pat(_tmp1DD,loc);});_tmpB1->f1=_tmp1DE;}),_tmpB1->f2=0,_tmpB1->f3=e,_tmpB1->f4=0;_tmpB1;});Cyc_Absyn_new_decl(_tmp1DF,loc);});}
# 754
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->sc=Cyc_Absyn_Public,_tmpB2->name=x,_tmpB2->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1E0=Cyc_Absyn_empty_tqual(0U);_tmpB2->tq=_tmp1E0;}),_tmpB2->type=t,_tmpB2->initializer=init,_tmpB2->rgn=0,_tmpB2->attributes=0,_tmpB2->escapes=0,_tmpB2->is_proto=0;_tmpB2;});}
# 761
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->sc=Cyc_Absyn_Static,_tmpB3->name=x,_tmpB3->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1E1=Cyc_Absyn_empty_tqual(0U);_tmpB3->tq=_tmp1E1;}),_tmpB3->type=t,_tmpB3->initializer=init,_tmpB3->rgn=0,_tmpB3->attributes=0,_tmpB3->escapes=0,_tmpB3->is_proto=0;_tmpB3;});}
# 767
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 771
return({struct Cyc_Absyn_AggrdeclImpl*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->exist_vars=exists,_tmpB4->rgn_po=po,_tmpB4->fields=fs,_tmpB4->tagged=tagged;_tmpB4;});}
# 774
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 777
return({void*_tmp1E3=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp1E2=({struct Cyc_Absyn_Aggrdecl*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->kind=k,_tmpB5->sc=s,_tmpB5->name=n,_tmpB5->tvs=ts,_tmpB5->impl=i,_tmpB5->attributes=atts,_tmpB5->expected_mem_kind=0;_tmpB5;});_tmpB6->f1=_tmp1E2;});_tmpB6;});Cyc_Absyn_new_decl(_tmp1E3,loc);});}
# 782
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 785
return({struct Cyc_Absyn_TypeDecl*_tmpB9=_cycalloc(sizeof(*_tmpB9));({void*_tmp1E5=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp1E4=({struct Cyc_Absyn_Aggrdecl*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->kind=k,_tmpB7->sc=s,_tmpB7->name=n,_tmpB7->tvs=ts,_tmpB7->impl=i,_tmpB7->attributes=atts,_tmpB7->expected_mem_kind=0;_tmpB7;});_tmpB8->f1=_tmp1E4;});_tmpB8;});_tmpB9->r=_tmp1E5;}),_tmpB9->loc=loc;_tmpB9;});}
# 791
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 794
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 796
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 799
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 801
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 804
return({void*_tmp1E7=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp1E6=({struct Cyc_Absyn_Datatypedecl*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->sc=s,_tmpBA->name=n,_tmpBA->tvs=ts,_tmpBA->fields=fs,_tmpBA->is_extensible=is_extensible;_tmpBA;});_tmpBB->f1=_tmp1E6;});_tmpBB;});Cyc_Absyn_new_decl(_tmp1E7,loc);});}
# 807
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 810
return({struct Cyc_Absyn_TypeDecl*_tmpBE=_cycalloc(sizeof(*_tmpBE));({void*_tmp1E9=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp1E8=({struct Cyc_Absyn_Datatypedecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->sc=s,_tmpBC->name=n,_tmpBC->tvs=ts,_tmpBC->fields=fs,_tmpBC->is_extensible=is_extensible;_tmpBC;});_tmpBD->f1=_tmp1E8;});_tmpBD;});_tmpBE->r=_tmp1E9;}),_tmpBE->loc=loc;_tmpBE;});}
# 822 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 828
{struct Cyc_List_List*_tmpBF=args;struct Cyc_List_List*args2=_tmpBF;for(0;args2 != 0;args2=args2->tl){
({void*_tmp1EA=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f3,1);(*((struct _tuple8*)args2->hd)).f3=_tmp1EA;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=5U,(_tmpC0->f1).tvars=tvs,(_tmpC0->f1).ret_tqual=ret_tqual,({
# 832
void*_tmp1EB=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpC0->f1).ret_type=_tmp1EB;}),(_tmpC0->f1).effect=eff_type,(_tmpC0->f1).args=args,(_tmpC0->f1).c_varargs=c_varargs,(_tmpC0->f1).cyc_varargs=cyc_varargs,(_tmpC0->f1).rgn_po=rgn_po,(_tmpC0->f1).attributes=atts,(_tmpC0->f1).requires_clause=req,(_tmpC0->f1).requires_relns=0,(_tmpC0->f1).ensures_clause=ens,(_tmpC0->f1).ensures_relns=0,(_tmpC0->f1).return_value=0;_tmpC0;});}
# 847
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpC1=Cyc_Absyn_compress(t);void*_stmttmp5=_tmpC1;void*_tmpC2=_stmttmp5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC2)->tag == 5U){_LL1: _LL2:
 return({void*_tmp1EE=t;void*_tmp1ED=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->v=& Cyc_Tcutil_rk;_tmpC3;}),0): Cyc_Absyn_heap_rgn_type;
# 849
struct Cyc_Absyn_Tqual _tmp1EC=
# 853
Cyc_Absyn_empty_tqual(0U);
# 849
Cyc_Absyn_at_type(_tmp1EE,_tmp1ED,_tmp1EC,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 854
 return t;}_LL0:;}
# 869 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpC4=e->r;void*_stmttmp6=_tmpC4;void*_tmpC5=_stmttmp6;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;struct Cyc_Absyn_Vardecl*_tmpCA;switch(*((int*)_tmpC5)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC5)->f1)){case 2U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpCA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC5)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpCA;
_tmpC9=vd;goto _LL6;}case 4U: _LL5: _tmpC9=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC5)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpC9;
# 874
void*_tmpCB=Cyc_Absyn_compress(vd->type);void*_stmttmp7=_tmpCB;void*_tmpCC=_stmttmp7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 878
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpC8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC5)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpC8;
return Cyc_Absyn_is_lvalue(e1);}case 13U: _LL11: _tmpC7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC5)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpC7;
return Cyc_Absyn_is_lvalue(e1);}case 12U: _LL13: _tmpC6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC5)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpC6;
return Cyc_Absyn_is_lvalue(e1);}default: _LL15: _LL16:
 return 0;}_LL0:;}
# 889
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*_tmpCD=fields;struct Cyc_List_List*fs=_tmpCD;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 895
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 899
struct _tuple15*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;(-- i,ts=ts->tl)){
if(ts == 0)
return 0;}
if(ts == 0)
return 0;
return(struct _tuple15*)ts->hd;}
# 908
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_tmpCE=decl->r;void*_stmttmp8=_tmpCE;void*_tmpCF=_stmttmp8;struct Cyc_Absyn_Fndecl*_tmpD0;struct Cyc_Absyn_Vardecl*_tmpD1;struct Cyc_Absyn_Typedefdecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD3;struct Cyc_Absyn_Aggrdecl*_tmpD4;switch(*((int*)_tmpCF)){case 5U: _LL1: _tmpD4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpCF)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*x=_tmpD4;
return(*x->name).f2;}case 7U: _LL3: _tmpD3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCF)->f1;_LL4: {struct Cyc_Absyn_Enumdecl*x=_tmpD3;
return(*x->name).f2;}case 8U: _LL5: _tmpD2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCF)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*x=_tmpD2;
return(*x->name).f2;}case 0U: _LL7: _tmpD1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCF)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmpD1;
return(*x->name).f2;}case 1U: _LL9: _tmpD0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpCF)->f1;_LLA: {struct Cyc_Absyn_Fndecl*x=_tmpD0;
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
# 931
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
struct _fat_ptr*dname;
for(0;decls != 0;decls=decls->tl){
dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 938
return 0;}
# 941
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
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val={26U};static char _tmpDB[3U]="f0";
# 960
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 962
static struct _fat_ptr f0={_tmpDB,_tmpDB,_tmpDB + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)field_names_v,(void*)field_names_v,(void*)(field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmpDA=(unsigned)(i + 1);struct _fat_ptr**_tmpD9=_cycalloc(_check_times(_tmpDA,sizeof(struct _fat_ptr*)));({{unsigned _tmp153=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp153;++ j){
# 969
j < fsz?_tmpD9[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_tmp1F1=({struct _fat_ptr*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct _fat_ptr _tmp1F0=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD7=({struct Cyc_Int_pa_PrintArg_struct _tmp154;_tmp154.tag=1U,_tmp154.f1=(unsigned long)((int)j);_tmp154;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({struct _fat_ptr _tmp1EF=({const char*_tmpD6="f%d";_tag_fat(_tmpD6,sizeof(char),4U);});Cyc_aprintf(_tmp1EF,_tag_fat(_tmpD5,sizeof(void*),1U));});});*_tmpD8=_tmp1F0;});_tmpD8;});_tmpD9[j]=_tmp1F1;});}}0;});_tag_fat(_tmpD9,sizeof(struct _fat_ptr*),_tmpDA);});
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple16{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 973
struct _tuple16 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpDC=info;struct _tuple0*_tmpDE;enum Cyc_Absyn_AggrKind _tmpDD;struct _tuple0*_tmpE0;enum Cyc_Absyn_AggrKind _tmpDF;if((_tmpDC.UnknownAggr).tag == 1){_LL1: _tmpDF=((_tmpDC.UnknownAggr).val).f1;_tmpE0=((_tmpDC.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind ak=_tmpDF;struct _tuple0*n=_tmpE0;
return({struct _tuple16 _tmp155;_tmp155.f1=ak,_tmp155.f2=n;_tmp155;});}}else{_LL3: _tmpDD=(*(_tmpDC.KnownAggr).val)->kind;_tmpDE=(*(_tmpDC.KnownAggr).val)->name;_LL4: {enum Cyc_Absyn_AggrKind k=_tmpDD;struct _tuple0*n=_tmpDE;
return({struct _tuple16 _tmp156;_tmp156.f1=k,_tmp156.f2=n;_tmp156;});}}_LL0:;}
# 979
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpE1=info;struct Cyc_Absyn_Aggrdecl*_tmpE2;if((_tmpE1.UnknownAggr).tag == 1){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp157;_tmp157.tag=0U,({struct _fat_ptr _tmp1F2=({const char*_tmpE5="unchecked aggrdecl";_tag_fat(_tmpE5,sizeof(char),19U);});_tmp157.f1=_tmp1F2;});_tmp157;});void*_tmpE3[1U];_tmpE3[0]=& _tmpE4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE3,sizeof(void*),1U));});}else{_LL3: _tmpE2=*(_tmpE1.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmpE2;
return ad;}}_LL0:;}
# 985
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmpE6=Cyc_Absyn_compress(t);void*_stmttmp9=_tmpE6;void*_tmpE7=_stmttmp9;union Cyc_Absyn_AggrInfo _tmpE8;struct Cyc_List_List*_tmpE9;switch(*((int*)_tmpE7)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE7)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpE9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE7)->f2;_LL2: {struct Cyc_List_List*fs=_tmpE9;
# 988
if(fs == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause == 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE7)->f1)->tag == 20U){_LL3: _tmpE8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpE7)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpE8;
# 991
union Cyc_Absyn_AggrInfo _tmpEA=info;int _tmpEC;enum Cyc_Absyn_AggrKind _tmpEB;enum Cyc_Absyn_AggrKind _tmpED;struct Cyc_Absyn_Aggrdecl*_tmpEE;if((_tmpEA.KnownAggr).tag == 2){_LL8: _tmpEE=*(_tmpEA.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpEE;
# 993
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpEF=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpEF;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpF0=impl->fields;struct Cyc_List_List*fields=_tmpF0;
if(fields == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause == 0;}}}}else{if(((_tmpEA.UnknownAggr).val).f3 == 0){_LLA: _tmpED=((_tmpEA.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpED;
return(int)k == (int)Cyc_Absyn_UnionA;}}else{_LLC: _tmpEB=((_tmpEA.UnknownAggr).val).f1;_tmpEC=(int)(((_tmpEA.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpEB;int b=_tmpEC;
return(int)k == (int)1U && !b;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1002
 return 0;}_LL0:;}
# 1005
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmpF1=Cyc_Absyn_compress(t);void*_stmttmpA=_tmpF1;void*_tmpF2=_stmttmpA;union Cyc_Absyn_AggrInfo _tmpF3;struct Cyc_List_List*_tmpF4;switch(*((int*)_tmpF2)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF2)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpF4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF2)->f2;_LL2: {struct Cyc_List_List*fs=_tmpF4;
# 1008
if(fs == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause != 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF2)->f1)->tag == 20U){_LL3: _tmpF3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF2)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpF3;
# 1011
union Cyc_Absyn_AggrInfo _tmpF5=info;int _tmpF7;enum Cyc_Absyn_AggrKind _tmpF6;enum Cyc_Absyn_AggrKind _tmpF8;struct Cyc_Absyn_Aggrdecl*_tmpF9;if((_tmpF5.KnownAggr).tag == 2){_LL8: _tmpF9=*(_tmpF5.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF9;
# 1013
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpFA=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpFA;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpFB=impl->fields;struct Cyc_List_List*fields=_tmpFB;
if(fields == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause != 0;}}}}else{if(((_tmpF5.UnknownAggr).val).f3 == 0){_LLA: _tmpF8=((_tmpF5.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpF8;
return 0;}}else{_LLC: _tmpF6=((_tmpF5.UnknownAggr).val).f1;_tmpF7=(int)(((_tmpF5.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpF6;int b=_tmpF7;
return 0;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1022
 return 0;}_LL0:;}
# 1026
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmpFC=b;struct Cyc_Absyn_Fndecl*_tmpFD;struct Cyc_Absyn_Vardecl*_tmpFE;struct Cyc_Absyn_Vardecl*_tmpFF;struct Cyc_Absyn_Vardecl*_tmp100;struct Cyc_Absyn_Vardecl*_tmp101;struct _tuple0*_tmp102;switch(*((int*)_tmpFC)){case 0U: _LL1: _tmp102=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpFC)->f1;_LL2: {struct _tuple0*qv=_tmp102;
return qv;}case 1U: _LL3: _tmp101=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpFC)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp101;
_tmp100=vd;goto _LL6;}case 3U: _LL5: _tmp100=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpFC)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp100;
_tmpFF=vd;goto _LL8;}case 4U: _LL7: _tmpFF=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpFC)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpFF;
_tmpFE=vd;goto _LLA;}case 5U: _LL9: _tmpFE=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpFC)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpFE;
return vd->name;}default: _LLB: _tmpFD=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpFC)->f1;_LLC: {struct Cyc_Absyn_Fndecl*fd=_tmpFD;
return fd->name;}}_LL0:;}
# 1037
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp158;_tmp158.tag=0U,({struct _fat_ptr _tmp1F3=({const char*_tmp105="designator list not of length 1";_tag_fat(_tmp105,sizeof(char),32U);});_tmp158.f1=_tmp1F3;});_tmp158;});void*_tmp103[1U];_tmp103[0]=& _tmp104;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp103,sizeof(void*),1U));});{
void*_tmp106=(void*)ds->hd;void*_stmttmpB=_tmp106;void*_tmp107=_stmttmpB;struct _fat_ptr*_tmp108;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp107)->tag == 1U){_LL1: _tmp108=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp107)->f1;_LL2: {struct _fat_ptr*f=_tmp108;
return f;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp159;_tmp159.tag=0U,({struct _fat_ptr _tmp1F4=({const char*_tmp10B="array designator in struct";_tag_fat(_tmp10B,sizeof(char),27U);});_tmp159.f1=_tmp1F4;});_tmp159;});void*_tmp109[1U];_tmp109[0]=& _tmp10A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp109,sizeof(void*),1U));});}_LL0:;}}
# 1046
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp10C=Cyc_Absyn_compress(t);void*_stmttmpC=_tmp10C;void*_tmp10D=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1057
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
void*_tmp10E=e->r;void*_stmttmpD=_tmp10E;void*_tmp10F=_stmttmpD;struct Cyc_List_List*_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;switch(*((int*)_tmp10F)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 1066
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp133=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp134=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_tmp135=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10F)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp133;struct Cyc_Absyn_Exp*e2=_tmp134;struct Cyc_Absyn_Exp*e3=_tmp135;
# 1076
if(!Cyc_Absyn_no_side_effects_exp(e1))return 0;
_tmp131=e2;_tmp132=e3;goto _LL20;}case 27U: _LL1F: _tmp131=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_tmp132=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10F)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp131;struct Cyc_Absyn_Exp*e2=_tmp132;
_tmp12F=e1;_tmp130=e2;goto _LL22;}case 7U: _LL21: _tmp12F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp130=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp12F;struct Cyc_Absyn_Exp*e2=_tmp130;
_tmp12D=e1;_tmp12E=e2;goto _LL24;}case 8U: _LL23: _tmp12D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp12E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp12D;struct Cyc_Absyn_Exp*e2=_tmp12E;
_tmp12B=e1;_tmp12C=e2;goto _LL26;}case 23U: _LL25: _tmp12B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp12C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp12B;struct Cyc_Absyn_Exp*e2=_tmp12C;
_tmp129=e1;_tmp12A=e2;goto _LL28;}case 9U: _LL27: _tmp129=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp12A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp129;struct Cyc_Absyn_Exp*e2=_tmp12A;
# 1083
return Cyc_Absyn_no_side_effects_exp(e1)&& Cyc_Absyn_no_side_effects_exp(e2);}case 41U: _LL29: _tmp128=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp128;
# 1085
_tmp127=e;goto _LL2C;}case 38U: _LL2B: _tmp127=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp127;
_tmp126=e;goto _LL2E;}case 11U: _LL2D: _tmp126=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp126;
_tmp125=e;goto _LL30;}case 12U: _LL2F: _tmp125=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp125;
_tmp124=e;goto _LL32;}case 13U: _LL31: _tmp124=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp124;
_tmp123=e;goto _LL34;}case 14U: _LL33: _tmp123=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp123;
_tmp122=e;goto _LL36;}case 18U: _LL35: _tmp122=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp122;
_tmp121=e;goto _LL38;}case 20U: _LL37: _tmp121=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp121;
_tmp120=e;goto _LL3A;}case 21U: _LL39: _tmp120=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp120;
_tmp11F=e;goto _LL3C;}case 22U: _LL3B: _tmp11F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp11F;
_tmp11E=e;goto _LL3E;}case 28U: _LL3D: _tmp11E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp11E;
_tmp11D=e;goto _LL40;}case 42U: _LL3F: _tmp11D=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp11D;
_tmp11C=e;goto _LL42;}case 15U: _LL41: _tmp11C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL42: {struct Cyc_Absyn_Exp*e=_tmp11C;
return Cyc_Absyn_no_side_effects_exp(e);}case 34U: _LL43: _tmp11A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10F)->f1).rgn;_tmp11B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10F)->f1).num_elts;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp11A;struct Cyc_Absyn_Exp*e=_tmp11B;
# 1099
_tmp118=eopt;_tmp119=e;goto _LL46;}case 16U: _LL45: _tmp118=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp119=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL46: {struct Cyc_Absyn_Exp*eopt=_tmp118;struct Cyc_Absyn_Exp*e=_tmp119;
# 1101
if(!Cyc_Absyn_no_side_effects_exp(e))return 0;
if(eopt != 0)return Cyc_Absyn_no_side_effects_exp(eopt);
return 1;}case 3U: _LL47: _tmp117=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL48: {struct Cyc_List_List*es=_tmp117;
# 1105
_tmp116=es;goto _LL4A;}case 31U: _LL49: _tmp116=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL4A: {struct Cyc_List_List*es=_tmp116;
_tmp115=es;goto _LL4C;}case 24U: _LL4B: _tmp115=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL4C: {struct Cyc_List_List*es=_tmp115;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Absyn_no_side_effects_exp,es);}case 36U: _LL4D: _tmp114=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL4E: {struct Cyc_List_List*dles=_tmp114;
# 1109
_tmp113=dles;goto _LL50;}case 29U: _LL4F: _tmp113=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10F)->f3;_LL50: {struct Cyc_List_List*dles=_tmp113;
_tmp112=dles;goto _LL52;}case 30U: _LL51: _tmp112=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL52: {struct Cyc_List_List*dles=_tmp112;
_tmp111=dles;goto _LL54;}case 26U: _LL53: _tmp111=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LL54: {struct Cyc_List_List*dles=_tmp111;
_tmp110=dles;goto _LL56;}default: _LL55: _tmp110=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LL56: {struct Cyc_List_List*dles=_tmp110;
# 1114
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Absyn_no_side_effects_exp((*((struct _tuple13*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}
# 1121
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
