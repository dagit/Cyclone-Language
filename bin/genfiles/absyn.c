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
extern int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
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
# 890
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 896
void*Cyc_Absyn_compress_kb(void*);
# 904
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 907
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 912
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 914
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 916
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 919
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 923
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 925
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 940
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 950
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one (void);
# 956
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 958
void*Cyc_Absyn_starb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 960
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 962
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 964
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 979
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 981
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 987
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 992
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1052
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);
# 1056
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned);
# 1078
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1082
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1087
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1093
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1123
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1125
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1127
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1130
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1138
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1144
int Cyc_Absyn_attribute_cmp(void*,void*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 108 "tcutil.h"
void*Cyc_Tcutil_compress(void*);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 138
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 175
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 47 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 34 "absyn.cyc"
int Cyc_Cyclone_tovc_r=0;
# 36
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 43
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _fat_ptr*)ss1->hd,(struct _fat_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;}}
# 50
if(ss2 != 0)return - 1;
return 0;}
# 53
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple11{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 57
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;struct _fat_ptr*_tmp2;union Cyc_Absyn_Nmspace _tmp1;_LL1: _tmp1=_tmp0->f1;_tmp2=_tmp0->f2;_LL2: {union Cyc_Absyn_Nmspace n1=_tmp1;struct _fat_ptr*v1=_tmp2;
struct _tuple0*_tmp3=q2;struct _fat_ptr*_tmp5;union Cyc_Absyn_Nmspace _tmp4;_LL4: _tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_LL5: {union Cyc_Absyn_Nmspace n2=_tmp4;struct _fat_ptr*v2=_tmp5;
int i=Cyc_strptrcmp(v1,v2);
if(i != 0)return i;{
struct _tuple11 _tmp6=({struct _tuple11 _tmp13D;_tmp13D.f1=n1,_tmp13D.f2=n2;_tmp13D;});struct _tuple11 _stmttmp0=_tmp6;struct _tuple11 _tmp7=_stmttmp0;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpC;switch(((_tmp7.f1).Abs_n).tag){case 4U: if(((_tmp7.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 69
 return - 1;}case 1U: switch(((_tmp7.f2).Loc_n).tag){case 1U: _LL9: _tmpC=((_tmp7.f1).Rel_n).val;_tmpD=((_tmp7.f2).Rel_n).val;_LLA: {struct Cyc_List_List*x1=_tmpC;struct Cyc_List_List*x2=_tmpD;
# 65
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;default: _LL13: _LL14:
# 71
 return - 1;}case 2U: switch(((_tmp7.f2).Rel_n).tag){case 2U: _LLB: _tmpA=((_tmp7.f1).Abs_n).val;_tmpB=((_tmp7.f2).Abs_n).val;_LLC: {struct Cyc_List_List*x1=_tmpA;struct Cyc_List_List*x2=_tmpB;
# 66
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 73
 return - 1;}default: switch(((_tmp7.f2).Rel_n).tag){case 3U: _LLD: _tmp8=((_tmp7.f1).C_n).val;_tmp9=((_tmp7.f2).C_n).val;_LLE: {struct Cyc_List_List*x1=_tmp8;struct Cyc_List_List*x2=_tmp9;
# 67
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4U: _LL11: _LL12:
# 70
 return 1;case 1U: _LL15: _LL16:
# 72
 return 1;default: _LL19: _LL1A:
# 74
 return 1;}}_LL6:;}}}}}
# 78
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 84
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
return C_scope?({union Cyc_Absyn_Nmspace _tmp13E;(_tmp13E.C_n).tag=3U,(_tmp13E.C_n).val=x;_tmp13E;}):({union Cyc_Absyn_Nmspace _tmp13F;(_tmp13F.Abs_n).tag=2U,(_tmp13F.Abs_n).val=x;_tmp13F;});}
# 88
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp140;(_tmp140.Rel_n).tag=1U,(_tmp140.Rel_n).val=x;_tmp140;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 91
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmpE;union Cyc_Absyn_Nmspace _tmpF=_stmttmp1;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 100
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 104
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 108
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp141;_tmp141.print_const=0,_tmp141.q_volatile=0,_tmp141.q_restrict=0,_tmp141.real_const=0,_tmp141.loc=loc;_tmp141;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp142;_tmp142.print_const=1,_tmp142.q_volatile=0,_tmp142.q_restrict=0,_tmp142.real_const=1,_tmp142.loc=loc;_tmp142;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp143;_tmp143.print_const=x.print_const || y.print_const,_tmp143.q_volatile=
x.q_volatile || y.q_volatile,_tmp143.q_restrict=
x.q_restrict || y.q_restrict,_tmp143.real_const=
x.real_const || y.real_const,({
unsigned _tmp15D=Cyc_Position_segment_join(x.loc,y.loc);_tmp143.loc=_tmp15D;});_tmp143;});}
# 117
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 123
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 125
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp144;(_tmp144.UnknownDatatype).tag=1U,(_tmp144.UnknownDatatype).val=udi;_tmp144;});}
# 128
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp145;(_tmp145.KnownDatatype).tag=2U,(_tmp145.KnownDatatype).val=d;_tmp145;});}
# 131
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp146;(_tmp146.UnknownDatatypefield).tag=1U,(_tmp146.UnknownDatatypefield).val=s;_tmp146;});}
# 134
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp147;(_tmp147.KnownDatatypefield).tag=2U,((_tmp147.KnownDatatypefield).val).f1=dd,((_tmp147.KnownDatatypefield).val).f2=df;_tmp147;});}
# 137
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp148;(_tmp148.UnknownAggr).tag=1U,((_tmp148.UnknownAggr).val).f1=ak,((_tmp148.UnknownAggr).val).f2=n,((_tmp148.UnknownAggr).val).f3=tagged;_tmp148;});}
# 140
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp149;(_tmp149.KnownAggr).tag=2U,(_tmp149.KnownAggr).val=ad;_tmp149;});}
# 143
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp12;void**_tmp13;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp13=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8: {void**k2=_tmp13;
# 148
_tmp12=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 147
 return k;}else{_LL9: _tmp12=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA: {void**k2=_tmp12;
# 150
({void*_tmp15E=Cyc_Absyn_compress_kb(*k2);*k2=_tmp15E;});
return*k2;}}}_LL0:;}
# 154
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_stmttmp2=_tmp14;void*_tmp15=_stmttmp2;struct Cyc_Absyn_Kind*_tmp17;struct Cyc_Core_Opt**_tmp16;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp19;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp19=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp19;
return k;}case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp18;
_tmp16=f;_tmp17=& Cyc_Tcutil_bk;goto _LL6;}default: _LL5: _tmp16=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp17=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6: {struct Cyc_Core_Opt**f=_tmp16;struct Cyc_Absyn_Kind*k=_tmp17;
# 159
({struct Cyc_Core_Opt*_tmp160=({struct Cyc_Core_Opt*_tmp1A=_cycalloc(sizeof(*_tmp1A));({void*_tmp15F=Cyc_Tcutil_kind_to_bound(k);_tmp1A->v=_tmp15F;});_tmp1A;});*f=_tmp160;});
return k;}}_LL0:;}
# 165
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp161=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp161;});_tmp1B;});}
# 168
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 184 "absyn.cyc"
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
# 210
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0U,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 212
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
# 225
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp23=0U;({void*_tmp162=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=15U,_tmp24->f1=n,_tmp24->f2=d;_tmp24;});Cyc_Absyn_app_type(_tmp162,_tag_fat(_tmp23,sizeof(void*),0U));});});}
# 228
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp25=0U;({void*_tmp163=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=16U,_tmp26->f1=fs;_tmp26;});Cyc_Absyn_app_type(_tmp163,_tag_fat(_tmp25,sizeof(void*),0U));});});}
# 231
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp27=0U;({void*_tmp164=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=17U,_tmp28->f1=s,_tmp28->f2=k;_tmp28;});Cyc_Absyn_app_type(_tmp164,_tag_fat(_tmp27,sizeof(void*),0U));});});}
# 234
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp165=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=18U,_tmp29->f1=di;_tmp29;});_tmp2A->f1=_tmp165;}),_tmp2A->f2=args;_tmp2A;});}
# 237
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp166=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=di;_tmp2B;});_tmp2C->f1=_tmp166;}),_tmp2C->f2=args;_tmp2C;});}
# 240
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp167=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=20U,_tmp2D->f1=ai;_tmp2D;});_tmp2E->f1=_tmp167;}),_tmp2E->f2=args;_tmp2E;});}
# 244
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,_tmp2F->f1=x;_tmp2F;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp30=i;switch(_tmp30){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Int_pa_PrintArg_struct _tmp33=({struct Cyc_Int_pa_PrintArg_struct _tmp14A;_tmp14A.tag=1U,_tmp14A.f1=(unsigned long)((int)i);_tmp14A;});void*_tmp31[1U];_tmp31[0]=& _tmp33;({struct _fat_ptr _tmp168=({const char*_tmp32="gen_float_type(%d)";_tag_fat(_tmp32,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp168,_tag_fat(_tmp31,sizeof(void*),1U));});});}_LL0:;}
# 253
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp34=sn;switch(_tmp34){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 256
enum Cyc_Absyn_Size_of _tmp35=sz;switch(_tmp35){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 265
enum Cyc_Absyn_Size_of _tmp36=sz;switch(_tmp36){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 273
 goto _LL8;default: _LL7: _LL8: {
# 275
enum Cyc_Absyn_Size_of _tmp37=sz;switch(_tmp37){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 286
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 289
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 294
void*Cyc_Absyn_bounds_one (void){
static void*bone=0;
void*_tmp38=bone;void*b=_tmp38;
if(b == 0){
b=Cyc_Absyn_thin_bounds_int(1U);
bone=b;}
# 301
return b;}
# 307
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 310
void*Cyc_Absyn_wchar_type (void){
int _tmp39=Sizeof_wchar_t;switch(_tmp39){case 1U: _LL1: _LL2:
# 321 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 325
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3A[4U]="exn";
# 330
static struct _fat_ptr Cyc_Absyn_exn_str={_tmp3A,_tmp3A,_tmp3A + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp41[15U]="Null_Exception";static char _tmp42[13U]="Array_bounds";static char _tmp43[16U]="Match_Exception";static char _tmp44[10U]="Bad_alloc";
# 334
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void){
static struct _fat_ptr builtin_exns[4U]={{_tmp41,_tmp41,_tmp41 + 15U},{_tmp42,_tmp42,_tmp42 + 13U},{_tmp43,_tmp43,_tmp43 + 16U},{_tmp44,_tmp44,_tmp44 + 10U}};
# 337
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct Cyc_Absyn_Datatypefield*_tmp16B=({struct Cyc_Absyn_Datatypefield*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct _tuple0*_tmp16A=({struct _tuple0*_tmp3C=_cycalloc(sizeof(*_tmp3C));((_tmp3C->f1).Abs_n).tag=2U,((_tmp3C->f1).Abs_n).val=0,({
struct _fat_ptr*_tmp169=({struct _fat_ptr*_tmp3B=_cycalloc(sizeof(*_tmp3B));*_tmp3B=*((struct _fat_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _fat_ptr),i));_tmp3B;});_tmp3C->f2=_tmp169;});_tmp3C;});
# 341
_tmp3D->name=_tmp16A;}),_tmp3D->typs=0,_tmp3D->loc=0U,_tmp3D->sc=Cyc_Absyn_Extern;_tmp3D;});_tmp3E->hd=_tmp16B;}),_tmp3E->tl=tufs;_tmp3E;});}}
# 345
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->sc=Cyc_Absyn_Extern,_tmp40->name=Cyc_Absyn_exn_name,_tmp40->tvs=0,({struct Cyc_Core_Opt*_tmp16C=({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=tufs;_tmp3F;});_tmp40->fields=_tmp16C;}),_tmp40->is_extensible=1;_tmp40;});}
# 347
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 350
void*Cyc_Absyn_exn_type (void){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp14B;(_tmp14B.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp16E=({struct Cyc_Absyn_Datatypedecl**_tmp45=_cycalloc(sizeof(*_tmp45));({struct Cyc_Absyn_Datatypedecl*_tmp16D=Cyc_Absyn_exn_tud();*_tmp45=_tmp16D;});_tmp45;});(_tmp14B.KnownDatatype).val=_tmp16E;});_tmp14B;}),0);
exn_typ=({void*_tmp171=(void*)_check_null(eopt);void*_tmp170=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp16F=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp171,_tmp170,_tmp16F,Cyc_Absyn_false_type);});}
# 357
return(void*)_check_null(exn_typ);}
# 360
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp48=_cycalloc(sizeof(*_tmp48));({union Cyc_Absyn_Nmspace _tmp174=Cyc_Absyn_Abs_n(0,0);_tmp48->f1=_tmp174;}),({struct _fat_ptr*_tmp173=({struct _fat_ptr*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _fat_ptr _tmp172=({const char*_tmp46="PrintArg";_tag_fat(_tmp46,sizeof(char),9U);});*_tmp47=_tmp172;});_tmp47;});_tmp48->f2=_tmp173;});_tmp48;});
return(struct _tuple0*)_check_null(q);}
# 366
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4B=_cycalloc(sizeof(*_tmp4B));({union Cyc_Absyn_Nmspace _tmp177=Cyc_Absyn_Abs_n(0,0);_tmp4B->f1=_tmp177;}),({struct _fat_ptr*_tmp176=({struct _fat_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct _fat_ptr _tmp175=({const char*_tmp49="ScanfArg";_tag_fat(_tmp49,sizeof(char),9U);});*_tmp4A=_tmp175;});_tmp4A;});_tmp4B->f2=_tmp176;});_tmp4B;});
return(struct _tuple0*)_check_null(q);}
# 378
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp51=_cycalloc(sizeof(*_tmp51));({union Cyc_Absyn_Nmspace _tmp17C=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));({struct _fat_ptr*_tmp17B=({struct _fat_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _fat_ptr _tmp17A=({const char*_tmp4C="Core";_tag_fat(_tmp4C,sizeof(char),5U);});*_tmp4D=_tmp17A;});_tmp4D;});_tmp4E->hd=_tmp17B;}),_tmp4E->tl=0;_tmp4E;}),0);_tmp51->f1=_tmp17C;}),({struct _fat_ptr*_tmp179=({struct _fat_ptr*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _fat_ptr _tmp178=({const char*_tmp4F="unique_region";_tag_fat(_tmp4F,sizeof(char),14U);});*_tmp50=_tmp178;});_tmp50;});_tmp51->f2=_tmp179;});_tmp51;});
return(struct _tuple0*)_check_null(q);}
# 384
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp (void){
void*_tmp52=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);void*t=_tmp52;
# 387
return({struct Cyc_Absyn_Exp*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->topt=t,_tmp56->loc=0U,_tmp56->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp181=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=1U,({void*_tmp180=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp17F=({struct Cyc_Absyn_Vardecl*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->sc=Cyc_Absyn_Extern,({
struct _tuple0*_tmp17E=Cyc_Absyn_uniquergn_qvar();_tmp53->name=_tmp17E;}),_tmp53->varloc=0U,({
struct Cyc_Absyn_Tqual _tmp17D=Cyc_Absyn_empty_tqual(0U);_tmp53->tq=_tmp17D;}),_tmp53->type=t,_tmp53->initializer=0,_tmp53->rgn=0,_tmp53->attributes=0,_tmp53->escapes=0,_tmp53->is_proto=0;_tmp53;});
# 388
_tmp54->f1=_tmp17F;});_tmp54;});_tmp55->f1=_tmp180;});_tmp55;});_tmp56->r=_tmp181;});_tmp56;});}
# 396
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=3U,_tmp57->f1=s;_tmp57;});}
# 400
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp14C;_tmp14C.elt_type=t,_tmp14C.elt_tq=tq,(_tmp14C.ptr_atts).rgn=r,(_tmp14C.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp14C.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp14C.ptr_atts).zero_term=zt,(_tmp14C.ptr_atts).ptrloc=0;_tmp14C;}));}
# 405
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp14D;_tmp14D.elt_type=t,_tmp14D.elt_tq=tq,(_tmp14D.ptr_atts).rgn=r,(_tmp14D.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp14D.ptr_atts).bounds=b,(_tmp14D.ptr_atts).zero_term=zt,(_tmp14D.ptr_atts).ptrloc=0;_tmp14D;}));}
# 410
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp14E;_tmp14E.elt_type=t,_tmp14E.elt_tq=tq,(_tmp14E.ptr_atts).rgn=r,(_tmp14E.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp14E.ptr_atts).bounds=b,(_tmp14E.ptr_atts).zero_term=zt,(_tmp14E.ptr_atts).ptrloc=0;_tmp14E;}));}
# 415
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp185=t;void*_tmp184=r;struct Cyc_Absyn_Tqual _tmp183=tq;void*_tmp182=Cyc_Absyn_bounds_one();Cyc_Absyn_starb_type(_tmp185,_tmp184,_tmp183,_tmp182,zeroterm);});}
# 418
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}
# 421
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp189=t;void*_tmp188=r;struct Cyc_Absyn_Tqual _tmp187=tq;void*_tmp186=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp189,_tmp188,_tmp187,_tmp186,zeroterm);});}
# 424
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp18D=Cyc_Absyn_char_type;void*_tmp18C=rgn;struct Cyc_Absyn_Tqual _tmp18B=Cyc_Absyn_empty_tqual(0U);void*_tmp18A=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp18D,_tmp18C,_tmp18B,_tmp18A,Cyc_Absyn_true_type);});}
# 427
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp191=Cyc_Absyn_char_type;void*_tmp190=rgn;struct Cyc_Absyn_Tqual _tmp18F=Cyc_Absyn_const_tqual(0U);void*_tmp18E=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp191,_tmp190,_tmp18F,_tmp18E,Cyc_Absyn_true_type);});}
# 431
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 433
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=4U,(_tmp58->f1).elt_type=elt_type,(_tmp58->f1).tq=tq,(_tmp58->f1).num_elts=num_elts,(_tmp58->f1).zero_term=zero_term,(_tmp58->f1).zt_loc=ztloc;_tmp58;});}
# 436
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=11U,_tmp59->f1=e;_tmp59;});}
# 441
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 443
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=8U,_tmp5A->f1=n,_tmp5A->f2=args,_tmp5A->f3=d,_tmp5A->f4=defn;_tmp5A;});}
# 449
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=0U,({void*_tmp194=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=20U,({union Cyc_Absyn_AggrInfo _tmp193=({enum Cyc_Absyn_AggrKind _tmp192=k;Cyc_Absyn_UnknownAggr(_tmp192,({struct _tuple0*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->f1=Cyc_Absyn_rel_ns_null,_tmp5B->f2=name;_tmp5B;}),0);});_tmp5C->f1=_tmp193;});_tmp5C;});_tmp5D->f1=_tmp194;}),_tmp5D->f2=0;_tmp5D;});}
# 452
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 455
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 458
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 462
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp14F;(_tmp14F.Char_c).tag=2U,((_tmp14F.Char_c).val).f1=sn,((_tmp14F.Char_c).val).f2=c;_tmp14F;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp150;(_tmp150.Wchar_c).tag=3U,(_tmp150.Wchar_c).val=s;_tmp150;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp151;(_tmp151.Short_c).tag=4U,((_tmp151.Short_c).val).f1=sn,((_tmp151.Short_c).val).f2=s;_tmp151;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp152;(_tmp152.Int_c).tag=5U,((_tmp152.Int_c).val).f1=sn,((_tmp152.Int_c).val).f2=i;_tmp152;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp153;(_tmp153.LongLong_c).tag=6U,((_tmp153.LongLong_c).val).f1=sn,((_tmp153.LongLong_c).val).f2=l;_tmp153;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp154;(_tmp154.Float_c).tag=7U,((_tmp154.Float_c).val).f1=s,((_tmp154.Float_c).val).f2=i;_tmp154;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp155;(_tmp155.String_c).tag=8U,(_tmp155.String_c).val=s;_tmp155;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){return({union Cyc_Absyn_Cnst _tmp156;(_tmp156.Wstring_c).tag=9U,(_tmp156.Wstring_c).val=s;_tmp156;});}
# 472
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->topt=0,_tmp5E->r=r,_tmp5E->loc=loc,_tmp5E->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp5E;});}
# 475
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp195=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=16U,_tmp5F->f1=rgn_handle,_tmp5F->f2=e;_tmp5F;});Cyc_Absyn_new_exp(_tmp195,loc);});}
# 478
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp60=_cycalloc(sizeof(*_tmp60));*_tmp60=*e;_tmp60;});}
# 481
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp196=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=0U,_tmp61->f1=c;_tmp61;});Cyc_Absyn_new_exp(_tmp196,loc);});}
# 484
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 488
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp197=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp197,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 496
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 503
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp198=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp198,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp199=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp199,loc);});}
# 510
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp19A=f(s);Cyc_Absyn_const_exp(_tmp19A,loc);});}
# 513
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 517
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp19C=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=1U,({void*_tmp19B=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=0U,_tmp62->f1=q;_tmp62;});_tmp63->f1=_tmp19B;});_tmp63;});Cyc_Absyn_new_exp(_tmp19C,loc);});}
# 520
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp19D=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=1U,_tmp64->f1=b;_tmp64;});Cyc_Absyn_new_exp(_tmp19D,loc);});}
# 524
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 527
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
return({void*_tmp19E=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=2U,_tmp65->f1=s;_tmp65;});Cyc_Absyn_new_exp(_tmp19E,loc);});}
# 530
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp19F=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=3U,_tmp66->f1=p,_tmp66->f2=es;_tmp66;});Cyc_Absyn_new_exp(_tmp19F,loc);});}
# 533
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1A1=p;struct Cyc_List_List*_tmp1A0=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=e,_tmp67->tl=0;_tmp67;});Cyc_Absyn_primop_exp(_tmp1A1,_tmp1A0,loc);});}
# 536
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1A4=p;struct Cyc_List_List*_tmp1A3=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=e1,({struct Cyc_List_List*_tmp1A2=({struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=e2,_tmp68->tl=0;_tmp68;});_tmp69->tl=_tmp1A2;});_tmp69;});Cyc_Absyn_primop_exp(_tmp1A4,_tmp1A3,loc);});}
# 539
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A5=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=35U,_tmp6A->f1=e1,_tmp6A->f2=e2;_tmp6A;});Cyc_Absyn_new_exp(_tmp1A5,loc);});}
# 542
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 552
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A6=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=4U,_tmp6B->f1=e1,_tmp6B->f2=popt,_tmp6B->f3=e2;_tmp6B;});Cyc_Absyn_new_exp(_tmp1A6,loc);});}
# 555
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 558
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=5U,_tmp6C->f1=e,_tmp6C->f2=i;_tmp6C;});Cyc_Absyn_new_exp(_tmp1A7,loc);});}
# 561
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=6U,_tmp6D->f1=e1,_tmp6D->f2=e2,_tmp6D->f3=e3;_tmp6D;});Cyc_Absyn_new_exp(_tmp1A8,loc);});}
# 564
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1A9=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=7U,_tmp6E->f1=e1,_tmp6E->f2=e2;_tmp6E;});Cyc_Absyn_new_exp(_tmp1A9,loc);});}
# 567
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=8U,_tmp6F->f1=e1,_tmp6F->f2=e2;_tmp6F;});Cyc_Absyn_new_exp(_tmp1AA,loc);});}
# 570
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1AB=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=9U,_tmp70->f1=e1,_tmp70->f2=e2;_tmp70;});Cyc_Absyn_new_exp(_tmp1AB,loc);});}
# 573
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1AC=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=10U,_tmp71->f1=e,_tmp71->f2=es,_tmp71->f3=0,_tmp71->f4=0;_tmp71;});Cyc_Absyn_new_exp(_tmp1AC,loc);});}
# 576
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1AD=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=10U,_tmp72->f1=e,_tmp72->f2=es,_tmp72->f3=0,_tmp72->f4=1;_tmp72;});Cyc_Absyn_new_exp(_tmp1AD,loc);});}
# 579
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1AE=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=12U,_tmp73->f1=e;_tmp73;});Cyc_Absyn_new_exp(_tmp1AE,loc);});}
# 582
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp1AF=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=13U,_tmp74->f1=e,_tmp74->f2=ts;_tmp74;});Cyc_Absyn_new_exp(_tmp1AF,loc);});}
# 585
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp1B0=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=14U,_tmp75->f1=t,_tmp75->f2=e,_tmp75->f3=user_cast,_tmp75->f4=c;_tmp75;});Cyc_Absyn_new_exp(_tmp1B0,loc);});}
# 588
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B1=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=11U,_tmp76->f1=e,_tmp76->f2=0;_tmp76;});Cyc_Absyn_new_exp(_tmp1B1,loc);});}
# 591
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B2=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=11U,_tmp77->f1=e,_tmp77->f2=1;_tmp77;});Cyc_Absyn_new_exp(_tmp1B2,loc);});}
# 594
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B3=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=15U,_tmp78->f1=e;_tmp78;});Cyc_Absyn_new_exp(_tmp1B3,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp1B4=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=17U,_tmp79->f1=t;_tmp79;});Cyc_Absyn_new_exp(_tmp1B4,loc);});}
# 598
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1B5=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=18U,_tmp7A->f1=e;_tmp7A;});Cyc_Absyn_new_exp(_tmp1B5,loc);});}
# 601
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp1B6=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=19U,_tmp7B->f1=t,_tmp7B->f2=ofs;_tmp7B;});Cyc_Absyn_new_exp(_tmp1B6,loc);});}
# 604
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1B7=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=20U,_tmp7C->f1=e;_tmp7C;});Cyc_Absyn_new_exp(_tmp1B7,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1B8=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});Cyc_Absyn_new_exp(_tmp1B8,loc);});}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
return({void*_tmp1B9=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});Cyc_Absyn_new_exp(_tmp1B9,loc);});}
# 611
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1BA=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=23U,_tmp7F->f1=e1,_tmp7F->f2=e2;_tmp7F;});Cyc_Absyn_new_exp(_tmp1BA,loc);});}
# 614
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1BB=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=24U,_tmp80->f1=es;_tmp80;});Cyc_Absyn_new_exp(_tmp1BB,loc);});}
# 617
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1BC=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=37U,_tmp81->f1=s;_tmp81;});Cyc_Absyn_new_exp(_tmp1BC,loc);});}
# 620
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp1BD=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=39U,_tmp82->f1=t;_tmp82;});Cyc_Absyn_new_exp(_tmp1BD,loc);});}
# 624
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 627
return({void*_tmp1BE=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=40U,_tmp83->f1=volatile_kw,_tmp83->f2=tmpl,_tmp83->f3=outs,_tmp83->f4=ins,_tmp83->f5=clobs;_tmp83;});Cyc_Absyn_new_exp(_tmp1BE,loc);});}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1BF=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=41U,_tmp84->f1=e;_tmp84;});Cyc_Absyn_new_exp(_tmp1BF,loc);});}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1C0=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=42U,_tmp85->f1=e;_tmp85;});Cyc_Absyn_new_exp(_tmp1C0,loc);});}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 636
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp86=0;struct Cyc_List_List*dles=_tmp86;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp88=_cycalloc(sizeof(*_tmp88));({struct _tuple12*_tmp1C1=({struct _tuple12*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->f1=0,_tmp87->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp87;});_tmp88->hd=_tmp1C1;}),_tmp88->tl=dles;_tmp88;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return({void*_tmp1C2=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=26U,_tmp89->f1=dles;_tmp89;});Cyc_Absyn_new_exp(_tmp1C2,loc);});}
# 643
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 646
return({void*_tmp1C3=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=36U,_tmp8A->f1=n,_tmp8A->f2=dles;_tmp8A;});Cyc_Absyn_new_exp(_tmp1C3,loc);});}
# 649
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->r=s,_tmp8B->loc=loc,_tmp8B->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp8B;});}
# 652
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1C4=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=1U,_tmp8C->f1=e;_tmp8C;});Cyc_Absyn_new_stmt(_tmp1C4,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1C5=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=3U,_tmp8D->f1=e;_tmp8D;});Cyc_Absyn_new_stmt(_tmp1C5,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1C7=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1C6=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1C7,_tmp1C6,loc);});}}}struct _tuple13{void*f1;void*f2;};
# 665
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple13 _tmp8E=({struct _tuple13 _tmp157;_tmp157.f1=s1->r,_tmp157.f2=s2->r;_tmp157;});struct _tuple13 _stmttmp3=_tmp8E;struct _tuple13 _tmp8F=_stmttmp3;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp8F.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp8F.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1C8=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=2U,_tmp90->f1=s1,_tmp90->f2=s2;_tmp90;});Cyc_Absyn_new_stmt(_tmp1C8,loc);});}}_LL0:;}
# 672
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp1C9=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=4U,_tmp91->f1=e,_tmp91->f2=s1,_tmp91->f3=s2;_tmp91;});Cyc_Absyn_new_stmt(_tmp1C9,loc);});}
# 675
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1CB=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=5U,(_tmp92->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1CA=Cyc_Absyn_skip_stmt(e->loc);(_tmp92->f1).f2=_tmp1CA;}),_tmp92->f2=s;_tmp92;});Cyc_Absyn_new_stmt(_tmp1CB,loc);});}
# 678
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1CE=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=9U,_tmp93->f1=e1,(_tmp93->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1CD=Cyc_Absyn_skip_stmt(e3->loc);(_tmp93->f2).f2=_tmp1CD;}),
(_tmp93->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1CC=Cyc_Absyn_skip_stmt(e3->loc);(_tmp93->f3).f2=_tmp1CC;}),_tmp93->f4=s;_tmp93;});
# 679
Cyc_Absyn_new_stmt(_tmp1CE,loc);});}
# 683
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1D0=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=14U,_tmp94->f1=s,(_tmp94->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1CF=Cyc_Absyn_skip_stmt(e->loc);(_tmp94->f2).f2=_tmp1CF;});_tmp94;});Cyc_Absyn_new_stmt(_tmp1D0,loc);});}
# 686
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1D1=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=10U,_tmp95->f1=e,_tmp95->f2=scs,_tmp95->f3=0;_tmp95;});Cyc_Absyn_new_stmt(_tmp1D1,loc);});}
# 689
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1D2=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=15U,_tmp96->f1=s,_tmp96->f2=scs,_tmp96->f3=0;_tmp96;});Cyc_Absyn_new_stmt(_tmp1D2,loc);});}
# 692
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp1D3=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=11U,_tmp97->f1=el,_tmp97->f2=0;_tmp97;});Cyc_Absyn_new_stmt(_tmp1D3,loc);});}
# 695
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
return({void*_tmp1D4=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=8U,_tmp98->f1=lab;_tmp98;});Cyc_Absyn_new_stmt(_tmp1D4,loc);});}
# 698
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1D5=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=13U,_tmp99->f1=v,_tmp99->f2=s;_tmp99;});Cyc_Absyn_new_stmt(_tmp1D5,loc);});}
# 701
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1D6=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=12U,_tmp9A->f1=d,_tmp9A->f2=s;_tmp9A;});Cyc_Absyn_new_stmt(_tmp1D6,loc);});}
# 704
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1D8=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp1D7=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp9C->f1=_tmp1D7;});_tmp9C;});Cyc_Absyn_new_decl(_tmp1D8,loc);});
return({void*_tmp1D9=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=12U,_tmp9B->f1=d,_tmp9B->f2=s;_tmp9B;});Cyc_Absyn_new_stmt(_tmp1D9,loc);});}
# 708
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp1DA=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1DA,loc);});}
# 712
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->r=p,_tmp9D->topt=0,_tmp9D->loc=s;_tmp9D;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1DB=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=17U,_tmp9E->f1=e;_tmp9E;});Cyc_Absyn_new_pat(_tmp1DB,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 718
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->r=r,_tmp9F->loc=loc;_tmp9F;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1DC=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=2U,_tmpA0->f1=p,_tmpA0->f2=0,_tmpA0->f3=e,_tmpA0->f4=0;_tmpA0;});Cyc_Absyn_new_decl(_tmp1DC,loc);});}
# 722
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp1DD=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=3U,_tmpA1->f1=vds;_tmpA1;});Cyc_Absyn_new_decl(_tmp1DD,loc);});}
# 725
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp1DE=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=4U,_tmpA2->f1=tv,_tmpA2->f2=vd,_tmpA2->f3=open_exp;_tmpA2;});Cyc_Absyn_new_decl(_tmp1DE,loc);});}
# 728
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 730
return({void*_tmp1E1=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=2U,({struct Cyc_Absyn_Pat*_tmp1E0=({void*_tmp1DF=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=2U,_tmpA3->f1=tv,_tmpA3->f2=vd;_tmpA3;});Cyc_Absyn_new_pat(_tmp1DF,loc);});_tmpA4->f1=_tmp1E0;}),_tmpA4->f2=0,_tmpA4->f3=e,_tmpA4->f4=0;_tmpA4;});Cyc_Absyn_new_decl(_tmp1E1,loc);});}
# 733
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->sc=Cyc_Absyn_Public,_tmpA5->name=x,_tmpA5->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1E2=Cyc_Absyn_empty_tqual(0U);_tmpA5->tq=_tmp1E2;}),_tmpA5->type=t,_tmpA5->initializer=init,_tmpA5->rgn=0,_tmpA5->attributes=0,_tmpA5->escapes=0,_tmpA5->is_proto=0;_tmpA5;});}
# 740
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->sc=Cyc_Absyn_Static,_tmpA6->name=x,_tmpA6->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1E3=Cyc_Absyn_empty_tqual(0U);_tmpA6->tq=_tmp1E3;}),_tmpA6->type=t,_tmpA6->initializer=init,_tmpA6->rgn=0,_tmpA6->attributes=0,_tmpA6->escapes=0,_tmpA6->is_proto=0;_tmpA6;});}
# 746
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 750
return({struct Cyc_Absyn_AggrdeclImpl*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->exist_vars=exists,_tmpA7->rgn_po=po,_tmpA7->fields=fs,_tmpA7->tagged=tagged;_tmpA7;});}
# 753
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 756
return({void*_tmp1E5=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp1E4=({struct Cyc_Absyn_Aggrdecl*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->kind=k,_tmpA8->sc=s,_tmpA8->name=n,_tmpA8->tvs=ts,_tmpA8->impl=i,_tmpA8->attributes=atts,_tmpA8->expected_mem_kind=0;_tmpA8;});_tmpA9->f1=_tmp1E4;});_tmpA9;});Cyc_Absyn_new_decl(_tmp1E5,loc);});}
# 761
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 764
return({struct Cyc_Absyn_TypeDecl*_tmpAC=_cycalloc(sizeof(*_tmpAC));({void*_tmp1E7=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp1E6=({struct Cyc_Absyn_Aggrdecl*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->kind=k,_tmpAA->sc=s,_tmpAA->name=n,_tmpAA->tvs=ts,_tmpAA->impl=i,_tmpAA->attributes=atts,_tmpAA->expected_mem_kind=0;_tmpAA;});_tmpAB->f1=_tmp1E6;});_tmpAB;});_tmpAC->r=_tmp1E7;}),_tmpAC->loc=loc;_tmpAC;});}
# 770
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 773
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 775
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 778
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 780
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 783
return({void*_tmp1E9=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp1E8=({struct Cyc_Absyn_Datatypedecl*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->sc=s,_tmpAD->name=n,_tmpAD->tvs=ts,_tmpAD->fields=fs,_tmpAD->is_extensible=is_extensible;_tmpAD;});_tmpAE->f1=_tmp1E8;});_tmpAE;});Cyc_Absyn_new_decl(_tmp1E9,loc);});}
# 786
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 789
return({struct Cyc_Absyn_TypeDecl*_tmpB1=_cycalloc(sizeof(*_tmpB1));({void*_tmp1EB=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp1EA=({struct Cyc_Absyn_Datatypedecl*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->sc=s,_tmpAF->name=n,_tmpAF->tvs=ts,_tmpAF->fields=fs,_tmpAF->is_extensible=is_extensible;_tmpAF;});_tmpB0->f1=_tmp1EA;});_tmpB0;});_tmpB1->r=_tmp1EB;}),_tmpB1->loc=loc;_tmpB1;});}
# 801 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 807
{struct Cyc_List_List*_tmpB2=args;struct Cyc_List_List*args2=_tmpB2;for(0;args2 != 0;args2=args2->tl){
({void*_tmp1EC=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f3,1);(*((struct _tuple8*)args2->hd)).f3=_tmp1EC;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=5U,(_tmpB3->f1).tvars=tvs,(_tmpB3->f1).ret_tqual=ret_tqual,({
# 811
void*_tmp1ED=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpB3->f1).ret_type=_tmp1ED;}),(_tmpB3->f1).effect=eff_type,(_tmpB3->f1).args=args,(_tmpB3->f1).c_varargs=c_varargs,(_tmpB3->f1).cyc_varargs=cyc_varargs,(_tmpB3->f1).rgn_po=rgn_po,(_tmpB3->f1).attributes=atts,(_tmpB3->f1).requires_clause=req,(_tmpB3->f1).requires_relns=0,(_tmpB3->f1).ensures_clause=ens,(_tmpB3->f1).ensures_relns=0,(_tmpB3->f1).return_value=0;_tmpB3;});}
# 826
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpB4=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmpB4;void*_tmpB5=_stmttmp4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB5)->tag == 5U){_LL1: _LL2:
 return({void*_tmp1F0=t;void*_tmp1EF=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=& Cyc_Tcutil_rk;_tmpB6;}),0): Cyc_Absyn_heap_rgn_type;
# 828
struct Cyc_Absyn_Tqual _tmp1EE=
# 832
Cyc_Absyn_empty_tqual(0U);
# 828
Cyc_Absyn_at_type(_tmp1F0,_tmp1EF,_tmp1EE,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 833
 return t;}_LL0:;}
# 848 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpB7=e->r;void*_stmttmp5=_tmpB7;void*_tmpB8=_stmttmp5;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Vardecl*_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBD;switch(*((int*)_tmpB8)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB8)->f1)){case 2U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpBD=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB8)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpBD;
_tmpBC=vd;goto _LL6;}case 4U: _LL5: _tmpBC=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB8)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpBC;
# 853
void*_tmpBE=Cyc_Tcutil_compress(vd->type);void*_stmttmp6=_tmpBE;void*_tmpBF=_stmttmp6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBF)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 857
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpBB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB8)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpBB;
return Cyc_Absyn_is_lvalue(e1);}case 13U: _LL11: _tmpBA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB8)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpBA;
return Cyc_Absyn_is_lvalue(e1);}case 12U: _LL13: _tmpB9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB8)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpB9;
return Cyc_Absyn_is_lvalue(e1);}default: _LL15: _LL16:
 return 0;}_LL0:;}
# 868
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*_tmpC0=fields;struct Cyc_List_List*fs=_tmpC0;for(0;fs != 0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 874
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 878
struct _tuple14*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 884
if(ts == 0)
return 0;
return(struct _tuple14*)ts->hd;}
# 889
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
# 912
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
struct _fat_ptr*dname;
for(0;decls != 0;decls=decls->tl){
dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 919
return 0;}
# 922
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
# 941
static int Cyc_Absyn_attribute_case_number(void*att){
void*_tmpC8=att;switch(*((int*)_tmpC8)){case 0U: _LL1: _LL2:
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
# 967
int Cyc_Absyn_attribute_cmp(void*att1,void*att2){
struct _tuple13 _tmpC9=({struct _tuple13 _tmp158;_tmp158.f1=att1,_tmp158.f2=att2;_tmp158;});struct _tuple13 _stmttmp8=_tmpC9;struct _tuple13 _tmpCA=_stmttmp8;int _tmpD0;int _tmpCF;enum Cyc_Absyn_Format_Type _tmpCE;int _tmpCD;int _tmpCC;enum Cyc_Absyn_Format_Type _tmpCB;struct _fat_ptr _tmpD2;struct _fat_ptr _tmpD1;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;int _tmpD6;int _tmpD5;int _tmpD8;int _tmpD7;switch(*((int*)_tmpCA.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpCA.f2)->tag == 0U){_LL1: _tmpD7=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpCA.f1)->f1;_tmpD8=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpCA.f2)->f1;_LL2: {int i1=_tmpD7;int i2=_tmpD8;
_tmpD5=i1;_tmpD6=i2;goto _LL4;}}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpCA.f2)->tag == 20U){_LL3: _tmpD5=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpCA.f1)->f1;_tmpD6=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpCA.f2)->f1;_LL4: {int i1=_tmpD5;int i2=_tmpD6;
# 971
return Cyc_Core_intcmp(i1,i2);}}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpCA.f2)->tag == 6U){_LL5: _tmpD3=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpCA.f1)->f1;_tmpD4=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpCA.f2)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmpD3;struct Cyc_Absyn_Exp*e2=_tmpD4;
# 973
if(e1 == e2)return 0;
if(e1 == 0)return - 1;
if(e2 == 0)return 1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpCA.f2)->tag == 8U){_LL7: _tmpD1=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpCA.f1)->f1;_tmpD2=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpCA.f2)->f1;_LL8: {struct _fat_ptr s1=_tmpD1;struct _fat_ptr s2=_tmpD2;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f2)->tag == 19U){_LL9: _tmpCB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f1)->f1;_tmpCC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f1)->f2;_tmpCD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f1)->f3;_tmpCE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f2)->f1;_tmpCF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f2)->f2;_tmpD0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpCA.f2)->f3;_LLA: {enum Cyc_Absyn_Format_Type ft1=_tmpCB;int i1=_tmpCC;int j1=_tmpCD;enum Cyc_Absyn_Format_Type ft2=_tmpCE;int i2=_tmpCF;int j2=_tmpD0;
# 979
int _tmpD9=Cyc_Core_intcmp((int)((unsigned)ft1),(int)((unsigned)ft2));int ftc=_tmpD9;
if(ftc != 0)return ftc;{
int _tmpDA=Cyc_Core_intcmp(i1,i2);int ic=_tmpDA;
if(ic != 0)return ic;
return Cyc_Core_intcmp(j1,j2);}}}else{goto _LLB;}default: _LLB: _LLC:
# 985
 return({int _tmp1F1=Cyc_Absyn_attribute_case_number(att1);Cyc_Core_intcmp(_tmp1F1,Cyc_Absyn_attribute_case_number(att2));});}_LL0:;}
# 988
int Cyc_Absyn_equal_att(void*a1,void*a2){
return Cyc_Absyn_attribute_cmp(a1,a2)== 0;}
# 993
int Cyc_Absyn_fntype_att(void*a){
void*_tmpDB=a;int _tmpDC;switch(*((int*)_tmpDB)){case 0U: _LL1: _tmpDC=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpDB)->f1;_LL2: {int i=_tmpDC;
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
 return 0;}_LL0:;}static char _tmpE3[3U]="f0";
# 1010
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 1012
static struct _fat_ptr f0={_tmpE3,_tmpE3,_tmpE3 + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)field_names_v,(void*)field_names_v,(void*)(field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmpE2=(unsigned)(i + 1);struct _fat_ptr**_tmpE1=_cycalloc(_check_times(_tmpE2,sizeof(struct _fat_ptr*)));({{unsigned _tmp159=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp159;++ j){
# 1019
j < fsz?_tmpE1[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_tmp1F4=({struct _fat_ptr*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct _fat_ptr _tmp1F3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDF=({struct Cyc_Int_pa_PrintArg_struct _tmp15A;_tmp15A.tag=1U,_tmp15A.f1=(unsigned long)((int)j);_tmp15A;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _fat_ptr _tmp1F2=({const char*_tmpDE="f%d";_tag_fat(_tmpDE,sizeof(char),4U);});Cyc_aprintf(_tmp1F2,_tag_fat(_tmpDD,sizeof(void*),1U));});});*_tmpE0=_tmp1F3;});_tmpE0;});_tmpE1[j]=_tmp1F4;});}}0;});_tag_fat(_tmpE1,sizeof(struct _fat_ptr*),_tmpE2);});
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple15{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1023
struct _tuple15 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpE4=info;struct _tuple0*_tmpE6;enum Cyc_Absyn_AggrKind _tmpE5;struct _tuple0*_tmpE8;enum Cyc_Absyn_AggrKind _tmpE7;if((_tmpE4.UnknownAggr).tag == 1){_LL1: _tmpE7=((_tmpE4.UnknownAggr).val).f1;_tmpE8=((_tmpE4.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind ak=_tmpE7;struct _tuple0*n=_tmpE8;
return({struct _tuple15 _tmp15B;_tmp15B.f1=ak,_tmp15B.f2=n;_tmp15B;});}}else{_LL3: _tmpE5=(*(_tmpE4.KnownAggr).val)->kind;_tmpE6=(*(_tmpE4.KnownAggr).val)->name;_LL4: {enum Cyc_Absyn_AggrKind k=_tmpE5;struct _tuple0*n=_tmpE6;
return({struct _tuple15 _tmp15C;_tmp15C.f1=k,_tmp15C.f2=n;_tmp15C;});}}_LL0:;}
# 1029
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmpE9=info;struct Cyc_Absyn_Aggrdecl*_tmpEA;if((_tmpE9.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmpEB=0U;({struct _fat_ptr _tmp1F5=({const char*_tmpEC="unchecked aggrdecl";_tag_fat(_tmpEC,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1F5,_tag_fat(_tmpEB,sizeof(void*),0U));});});}else{_LL3: _tmpEA=*(_tmpE9.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmpEA;
return ad;}}_LL0:;}
# 1035
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmpED=Cyc_Tcutil_compress(t);void*_stmttmp9=_tmpED;void*_tmpEE=_stmttmp9;union Cyc_Absyn_AggrInfo _tmpEF;struct Cyc_List_List*_tmpF0;switch(*((int*)_tmpEE)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEE)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpF0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEE)->f2;_LL2: {struct Cyc_List_List*fs=_tmpF0;
# 1038
if(fs == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause == 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->tag == 20U){_LL3: _tmpEF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpEF;
# 1041
union Cyc_Absyn_AggrInfo _tmpF1=info;int _tmpF3;enum Cyc_Absyn_AggrKind _tmpF2;enum Cyc_Absyn_AggrKind _tmpF4;struct Cyc_Absyn_Aggrdecl*_tmpF5;if((_tmpF1.KnownAggr).tag == 2){_LL8: _tmpF5=*(_tmpF1.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF5;
# 1043
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmpF6=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmpF6;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmpF7=impl->fields;struct Cyc_List_List*fields=_tmpF7;
if(fields == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause == 0;}}}}else{if(((_tmpF1.UnknownAggr).val).f3 == 0){_LLA: _tmpF4=((_tmpF1.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpF4;
return(int)k == (int)Cyc_Absyn_UnionA;}}else{_LLC: _tmpF2=((_tmpF1.UnknownAggr).val).f1;_tmpF3=(int)(((_tmpF1.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpF2;int b=_tmpF3;
return(int)k == (int)1U && !b;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1052
 return 0;}_LL0:;}
# 1055
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmpF8=Cyc_Tcutil_compress(t);void*_stmttmpA=_tmpF8;void*_tmpF9=_stmttmpA;union Cyc_Absyn_AggrInfo _tmpFA;struct Cyc_List_List*_tmpFB;switch(*((int*)_tmpF9)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF9)->f1 == Cyc_Absyn_UnionA){_LL1: _tmpFB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF9)->f2;_LL2: {struct Cyc_List_List*fs=_tmpFB;
# 1058
if(fs == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause != 0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)->tag == 20U){_LL3: _tmpFA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF9)->f1)->f1;_LL4: {union Cyc_Absyn_AggrInfo info=_tmpFA;
# 1061
union Cyc_Absyn_AggrInfo _tmpFC=info;int _tmpFE;enum Cyc_Absyn_AggrKind _tmpFD;enum Cyc_Absyn_AggrKind _tmpFF;struct Cyc_Absyn_Aggrdecl*_tmp100;if((_tmpFC.KnownAggr).tag == 2){_LL8: _tmp100=*(_tmpFC.KnownAggr).val;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmp100;
# 1063
if((int)ad->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp101=ad->impl;struct Cyc_Absyn_AggrdeclImpl*impl=_tmp101;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(impl))->tagged)return 0;{
struct Cyc_List_List*_tmp102=impl->fields;struct Cyc_List_List*fields=_tmp102;
if(fields == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause != 0;}}}}else{if(((_tmpFC.UnknownAggr).val).f3 == 0){_LLA: _tmpFF=((_tmpFC.UnknownAggr).val).f1;_LLB: {enum Cyc_Absyn_AggrKind k=_tmpFF;
return 0;}}else{_LLC: _tmpFD=((_tmpFC.UnknownAggr).val).f1;_tmpFE=(int)(((_tmpFC.UnknownAggr).val).f3)->v;_LLD: {enum Cyc_Absyn_AggrKind k=_tmpFD;int b=_tmpFE;
return 0;}}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1072
 return 0;}_LL0:;}
# 1076
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp103=b;struct Cyc_Absyn_Fndecl*_tmp104;struct Cyc_Absyn_Vardecl*_tmp105;struct Cyc_Absyn_Vardecl*_tmp106;struct Cyc_Absyn_Vardecl*_tmp107;struct Cyc_Absyn_Vardecl*_tmp108;struct _tuple0*_tmp109;switch(*((int*)_tmp103)){case 0U: _LL1: _tmp109=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp103)->f1;_LL2: {struct _tuple0*qv=_tmp109;
return qv;}case 1U: _LL3: _tmp108=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp103)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp108;
_tmp107=vd;goto _LL6;}case 3U: _LL5: _tmp107=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp103)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp107;
_tmp106=vd;goto _LL8;}case 4U: _LL7: _tmp106=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp103)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp106;
_tmp105=vd;goto _LLA;}case 5U: _LL9: _tmp105=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp103)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp105;
return vd->name;}default: _LLB: _tmp104=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp103)->f1;_LLC: {struct Cyc_Absyn_Fndecl*fd=_tmp104;
return fd->name;}}_LL0:;}
# 1087
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({void*_tmp10A=0U;({struct _fat_ptr _tmp1F6=({const char*_tmp10B="designator list not of length 1";_tag_fat(_tmp10B,sizeof(char),32U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1F6,_tag_fat(_tmp10A,sizeof(void*),0U));});});{
void*_tmp10C=(void*)ds->hd;void*_stmttmpB=_tmp10C;void*_tmp10D=_stmttmpB;struct _fat_ptr*_tmp10E;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp10D)->tag == 1U){_LL1: _tmp10E=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp10D)->f1;_LL2: {struct _fat_ptr*f=_tmp10E;
return f;}}else{_LL3: _LL4:
({void*_tmp10F=0U;({struct _fat_ptr _tmp1F7=({const char*_tmp110="array designator in struct";_tag_fat(_tmp110,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp1F7,_tag_fat(_tmp10F,sizeof(void*),0U));});});}_LL0:;}}
# 1096
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp111=Cyc_Tcutil_compress(t);void*_stmttmpC=_tmp111;void*_tmp112=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp112)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp112)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1106
unsigned Cyc_Absyn_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmp113=e->r;void*_stmttmpD=_tmp113;void*_tmp114=_stmttmpD;switch(*((int*)_tmp114)){case 0U: _LL1: _LL2:
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
# 1157
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
void*_tmp115=e->r;void*_stmttmpE=_tmp115;void*_tmp116=_stmttmpE;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13A;switch(*((int*)_tmp116)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 1166
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp13A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_tmp13B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_tmp13C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp116)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp13A;struct Cyc_Absyn_Exp*e2=_tmp13B;struct Cyc_Absyn_Exp*e3=_tmp13C;
# 1176
if(!Cyc_Absyn_no_side_effects_exp(e1))return 0;
_tmp138=e2;_tmp139=e3;goto _LL20;}case 27U: _LL1F: _tmp138=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_tmp139=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp116)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp138;struct Cyc_Absyn_Exp*e2=_tmp139;
_tmp136=e1;_tmp137=e2;goto _LL22;}case 7U: _LL21: _tmp136=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_tmp137=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp136;struct Cyc_Absyn_Exp*e2=_tmp137;
_tmp134=e1;_tmp135=e2;goto _LL24;}case 8U: _LL23: _tmp134=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_tmp135=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp134;struct Cyc_Absyn_Exp*e2=_tmp135;
_tmp132=e1;_tmp133=e2;goto _LL26;}case 23U: _LL25: _tmp132=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_tmp133=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp132;struct Cyc_Absyn_Exp*e2=_tmp133;
_tmp130=e1;_tmp131=e2;goto _LL28;}case 9U: _LL27: _tmp130=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_tmp131=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp130;struct Cyc_Absyn_Exp*e2=_tmp131;
# 1183
return Cyc_Absyn_no_side_effects_exp(e1)&& Cyc_Absyn_no_side_effects_exp(e2);}case 41U: _LL29: _tmp12F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp12F;
# 1185
_tmp12E=e;goto _LL2C;}case 38U: _LL2B: _tmp12E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp12E;
_tmp12D=e;goto _LL2E;}case 11U: _LL2D: _tmp12D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp12D;
_tmp12C=e;goto _LL30;}case 12U: _LL2F: _tmp12C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp12C;
_tmp12B=e;goto _LL32;}case 13U: _LL31: _tmp12B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp12B;
_tmp12A=e;goto _LL34;}case 14U: _LL33: _tmp12A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp12A;
_tmp129=e;goto _LL36;}case 18U: _LL35: _tmp129=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp129;
_tmp128=e;goto _LL38;}case 20U: _LL37: _tmp128=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp128;
_tmp127=e;goto _LL3A;}case 21U: _LL39: _tmp127=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp127;
_tmp126=e;goto _LL3C;}case 22U: _LL3B: _tmp126=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp126;
_tmp125=e;goto _LL3E;}case 28U: _LL3D: _tmp125=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp125;
_tmp124=e;goto _LL40;}case 42U: _LL3F: _tmp124=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp124;
_tmp123=e;goto _LL42;}case 15U: _LL41: _tmp123=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL42: {struct Cyc_Absyn_Exp*e=_tmp123;
return Cyc_Absyn_no_side_effects_exp(e);}case 34U: _LL43: _tmp121=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp116)->f1).rgn;_tmp122=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp116)->f1).num_elts;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp121;struct Cyc_Absyn_Exp*e=_tmp122;
# 1199
_tmp11F=eopt;_tmp120=e;goto _LL46;}case 16U: _LL45: _tmp11F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_tmp120=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL46: {struct Cyc_Absyn_Exp*eopt=_tmp11F;struct Cyc_Absyn_Exp*e=_tmp120;
# 1201
if(!Cyc_Absyn_no_side_effects_exp(e))return 0;
if(eopt != 0)return Cyc_Absyn_no_side_effects_exp(eopt);
return 1;}case 3U: _LL47: _tmp11E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL48: {struct Cyc_List_List*es=_tmp11E;
# 1205
_tmp11D=es;goto _LL4A;}case 31U: _LL49: _tmp11D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL4A: {struct Cyc_List_List*es=_tmp11D;
_tmp11C=es;goto _LL4C;}case 24U: _LL4B: _tmp11C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL4C: {struct Cyc_List_List*es=_tmp11C;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Absyn_no_side_effects_exp,es);}case 36U: _LL4D: _tmp11B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL4E: {struct Cyc_List_List*dles=_tmp11B;
# 1209
_tmp11A=dles;goto _LL50;}case 29U: _LL4F: _tmp11A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp116)->f3;_LL50: {struct Cyc_List_List*dles=_tmp11A;
_tmp119=dles;goto _LL52;}case 30U: _LL51: _tmp119=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL52: {struct Cyc_List_List*dles=_tmp119;
_tmp118=dles;goto _LL54;}case 26U: _LL53: _tmp118=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp116)->f1;_LL54: {struct Cyc_List_List*dles=_tmp118;
_tmp117=dles;goto _LL56;}default: _LL55: _tmp117=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp116)->f2;_LL56: {struct Cyc_List_List*dles=_tmp117;
# 1214
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Absyn_no_side_effects_exp((*((struct _tuple12*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}
# 1221
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
# 1226
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;
