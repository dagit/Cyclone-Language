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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line);struct Cyc_Hashtable_Table;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 885 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 901
void*Cyc_Absyn_compress(void*);
# 933
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 167
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 178
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t);
# 252
int Cyc_Tcutil_new_tvar_id (void);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 33 "currgn.cyc"
static struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_tvar=0;
static void*Cyc_CurRgn_curr_rgn_typ=0;static char _tmp0[3U]="`C";
# 36
struct _fat_ptr Cyc_CurRgn_curr_rgn_name={_tmp0,_tmp0,_tmp0 + 3U};
# 38
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void){
if(Cyc_CurRgn_curr_tvar == 0){
Cyc_CurRgn_curr_tvar=({struct Cyc_Absyn_Tvar*_tmp2=_cycalloc(sizeof(*_tmp2));({struct _fat_ptr*_tmp107=({struct _fat_ptr*_tmp1=_cycalloc(sizeof(*_tmp1));*_tmp1=Cyc_CurRgn_curr_rgn_name;_tmp1;});_tmp2->name=_tmp107;}),({
int _tmp106=Cyc_Tcutil_new_tvar_id();_tmp2->identity=_tmp106;}),({
void*_tmp105=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp2->kind=_tmp105;});_tmp2;});
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar));}
# 45
return(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);}
# 47
void*Cyc_CurRgn_curr_rgn_type (void){
Cyc_CurRgn_curr_rgn_tvar();
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar));
return(void*)_check_null(Cyc_CurRgn_curr_rgn_typ);}
# 56
static struct _tuple8*Cyc_CurRgn_argtype(struct _tuple8*arg){
struct _tuple8*_tmp3=arg;void*_tmp6;struct Cyc_Absyn_Tqual _tmp5;struct _fat_ptr*_tmp4;_LL1: _tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;_LL2: {struct _fat_ptr*vopt=_tmp4;struct Cyc_Absyn_Tqual tq=_tmp5;void*t=_tmp6;
{void*_tmp7=t;void*_tmp8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL4: _tmp8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->f1).elt_type;_LL5: {void*t1=_tmp8;
# 60
if(!Cyc_Tcutil_is_function_type(t1))return arg;
goto _LL3;}}else{_LL6: _LL7:
 return arg;}_LL3:;}{
# 64
void*_tmp9=Cyc_Tcutil_copy_type(t);void*t2=_tmp9;
void*_tmpA=Cyc_Absyn_compress(t2);void*_stmttmp0=_tmpA;void*_tmpB=_stmttmp0;void*_tmpC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB)->tag == 3U){_LL9: _tmpC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB)->f1).elt_type;_LLA: {void*t1=_tmpC;
# 67
void*_tmpD=Cyc_Absyn_compress(t1);void*_stmttmp1=_tmpD;void*_tmpE=_stmttmp1;struct Cyc_Absyn_Vardecl*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_List_List*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_List_List*_tmp17;struct Cyc_List_List*_tmp16;struct Cyc_Absyn_VarargInfo*_tmp15;int _tmp14;struct Cyc_List_List*_tmp13;void*_tmp12;struct Cyc_Absyn_Tqual _tmp11;void*_tmp10;struct Cyc_List_List**_tmpF;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->tag == 5U){_LLE: _tmpF=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).tvars;_tmp10=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).effect;_tmp11=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).ret_tqual;_tmp12=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).ret_type;_tmp13=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).args;_tmp14=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).c_varargs;_tmp15=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).cyc_varargs;_tmp16=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).rgn_po;_tmp17=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).attributes;_tmp18=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).requires_clause;_tmp19=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).requires_relns;_tmp1A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).ensures_clause;_tmp1B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).ensures_relns;_tmp1C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE)->f1).return_value;_LLF: {struct Cyc_List_List**tvars=_tmpF;void*eff=_tmp10;struct Cyc_Absyn_Tqual res_tq=_tmp11;void*res_typ=_tmp12;struct Cyc_List_List*args_info=_tmp13;int c_vararg=_tmp14;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp15;struct Cyc_List_List*rgn_po=_tmp16;struct Cyc_List_List*atts=_tmp17;struct Cyc_Absyn_Exp*req=_tmp18;struct Cyc_List_List*req_relns=_tmp19;struct Cyc_Absyn_Exp*ens=_tmp1A;struct Cyc_List_List*ens_relns=_tmp1B;struct Cyc_Absyn_Vardecl*ret_var=_tmp1C;
# 73
struct Cyc_List_List*_tmp1D=*tvars;struct Cyc_List_List*tvars2=_tmp1D;
({struct Cyc_List_List*_tmp108=({struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->hd=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmp1E->tl=tvars2;_tmp1E;});*tvars=_tmp108;});
return({struct _tuple8*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->f1=vopt,_tmp1F->f2=tq,_tmp1F->f3=t2;_tmp1F;});}}else{_LL10: _LL11:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp109=({const char*_tmp20="internal compiler error: not a function type";_tag_fat(_tmp20,sizeof(char),45U);});_tmp21->f1=_tmp109;});_tmp21;}));}_LLD:;}}else{_LLB: _LLC:
# 78
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp10A=({const char*_tmp22="internal compiler error: not a pointer type";_tag_fat(_tmp22,sizeof(char),44U);});_tmp23->f1=_tmp10A;});_tmp23;}));}_LL8:;}}}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 83
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn){
void*_tmp24=fntype;struct Cyc_Absyn_Vardecl*_tmp32;struct Cyc_List_List*_tmp31;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_List_List*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;struct Cyc_List_List*_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_Absyn_VarargInfo*_tmp2B;int _tmp2A;struct Cyc_List_List**_tmp29;void*_tmp28;struct Cyc_Absyn_Tqual _tmp27;void*_tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->tag == 5U){_LL1: _tmp25=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).tvars;_tmp26=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).effect;_tmp27=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).ret_tqual;_tmp28=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).ret_type;_tmp29=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).args;_tmp2A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).c_varargs;_tmp2B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).cyc_varargs;_tmp2C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).rgn_po;_tmp2D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).attributes;_tmp2E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).requires_clause;_tmp2F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).requires_relns;_tmp30=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).ensures_clause;_tmp31=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).ensures_relns;_tmp32=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24)->f1).return_value;_LL2: {struct Cyc_List_List*tvars=_tmp25;void*eff=_tmp26;struct Cyc_Absyn_Tqual res_tq=_tmp27;void*res_typ=_tmp28;struct Cyc_List_List**args_info=_tmp29;int c_vararg=_tmp2A;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp2B;struct Cyc_List_List*rgn_po=_tmp2C;struct Cyc_List_List*atts=_tmp2D;struct Cyc_Absyn_Exp*req=_tmp2E;struct Cyc_List_List*req_relns=_tmp2F;struct Cyc_Absyn_Exp*ens=_tmp30;struct Cyc_List_List*ens_relns=_tmp31;struct Cyc_Absyn_Vardecl*ret_var=_tmp32;
# 91
struct Cyc_List_List*_tmp33=*args_info;struct Cyc_List_List*old_args_info=_tmp33;
# 93
({struct Cyc_List_List*_tmp10B=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_CurRgn_argtype,*args_info);*args_info=_tmp10B;});{
# 95
struct Cyc_List_List*_tmp34=({struct Cyc_List_List*_tmp39=_cycalloc(sizeof(*_tmp39));({struct _tuple11*_tmp10C=({struct _tuple11*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmp38->f2=currgn;_tmp38;});_tmp39->hd=_tmp10C;}),_tmp39->tl=0;_tmp39;});struct Cyc_List_List*inst=_tmp34;
void*_tmp35=Cyc_Tcutil_substitute(inst,fntype);void*fntype2=_tmp35;
# 98
{void*_tmp36=fntype2;struct Cyc_List_List**_tmp37;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp36)->tag == 5U){_LL6: _tmp37=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp36)->f1).args;_LL7: {struct Cyc_List_List**args_info2=_tmp37;
# 100
*args_info2=old_args_info;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 104
*args_info=old_args_info;
return fntype2;}}}else{_LL3: _LL4:
 return fntype;}_LL0:;}
# 111
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))!= 0;}
# 116
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))== 0;}struct Cyc_CurRgn_rgnsubenv{struct Cyc_List_List*inst;int in_externC;int toplevel;};
# 126
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e);
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e);
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds);
# 131
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud);
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad);
# 135
static void Cyc_CurRgn_check_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
if(Cyc_Absyn_tvar_cmp(tv,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp103;_tmp103.tag=0U,({struct _fat_ptr _tmp10D=({const char*_tmp3D="Cannot abstract special type variable ";_tag_fat(_tmp3D,sizeof(char),39U);});_tmp103.f1=_tmp10D;});_tmp103;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp3C=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp102;_tmp102.tag=7U,_tmp102.f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);_tmp102;});void*_tmp3A[2U];_tmp3A[0]=& _tmp3B,_tmp3A[1]=& _tmp3C;({unsigned _tmp10E=loc;Cyc_Warn_err2(_tmp10E,_tag_fat(_tmp3A,sizeof(void*),2U));});});}
# 139
static void Cyc_CurRgn_check_tvars(unsigned loc,struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}}
# 144
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 147
static void Cyc_CurRgn_rgnsubtype(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,void*type){
void*_tmp3E=Cyc_Absyn_compress(type);void*_stmttmp2=_tmp3E;void*_tmp3F=_stmttmp2;void**_tmp41;struct Cyc_Absyn_TypeDecl*_tmp40;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_List_List*_tmp45;void*_tmp44;void*_tmp46;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp48;void*_tmp4C;void*_tmp4B;void*_tmp4A;void*_tmp49;void*_tmp4F;struct Cyc_Absyn_Exp*_tmp4E;void*_tmp4D;struct Cyc_List_List*_tmp50;struct Cyc_Absyn_Tvar*_tmp51;switch(*((int*)_tmp3F)){case 2U: _LL1: _tmp51=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp51;
# 150
if(env.toplevel)Cyc_CurRgn_check_tvar(loc,v);return;}case 8U: _LL3: _tmp50=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3F)->f2;_LL4: {struct Cyc_List_List*ts=_tmp50;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 4U: _LL5: _tmp4D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).num_elts;_tmp4F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_LL6: {void*t1=_tmp4D;struct Cyc_Absyn_Exp*e=_tmp4E;void*zt=_tmp4F;
# 153
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubexpopt(env,e);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 3U: _LL7: _tmp49=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp4A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F)->f1).ptr_atts).rgn;_tmp4B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F)->f1).ptr_atts).bounds;_tmp4C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp49;void*r=_tmp4A;void*b=_tmp4B;void*zt=_tmp4C;
# 158
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubtype(loc,env,r);
Cyc_CurRgn_rgnsubtype(loc,env,b);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 5U: _LL9: _LLA:
# 164
 return;case 6U: _LLB: _tmp48=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp48;
# 166
{struct Cyc_List_List*_tmp52=tqts;struct Cyc_List_List*ts1=_tmp52;for(0;ts1 != 0;ts1=ts1->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple12*)ts1->hd)).f2);}}
# 169
return;}case 7U: _LLD: _tmp47=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F)->f2;_LLE: {struct Cyc_List_List*fs=_tmp47;
# 171
for(0;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}
# 175
return;}case 1U: _LLF: _tmp46=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F)->f2;_LL10: {void*r=_tmp46;
# 177
if(r != 0)Cyc_CurRgn_rgnsubtype(loc,env,r);
return;}case 0U: _LL11: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f2;_LL12: {void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 9U: _LL13: _tmp43=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp43;
Cyc_CurRgn_rgnsubexp(env,e);return;}case 11U: _LL15: _tmp42=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp42;
Cyc_CurRgn_rgnsubexp(env,e);return;}default: _LL17: _tmp40=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3F)->f1;_tmp41=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3F)->f2;_LL18: {struct Cyc_Absyn_TypeDecl*td=_tmp40;void**topt=_tmp41;
# 183
topt == 0?0:({struct _fat_ptr _tmp10F=({const char*_tmp53="topt == NULL";_tag_fat(_tmp53,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp10F,({const char*_tmp54="currgn.cyc";_tag_fat(_tmp54,sizeof(char),11U);}),183U);});
{void*_tmp55=td->r;void*_stmttmp3=_tmp55;void*_tmp56=_stmttmp3;struct Cyc_Absyn_Datatypedecl*_tmp57;struct Cyc_Absyn_Aggrdecl*_tmp58;switch(*((int*)_tmp56)){case 0U: _LL1A: _tmp58=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp56)->f1;_LL1B: {struct Cyc_Absyn_Aggrdecl*ad=_tmp58;
Cyc_CurRgn_rgnsubaggrdecl(loc,env,ad);return;}case 2U: _LL1C: _tmp57=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp56)->f1;_LL1D: {struct Cyc_Absyn_Datatypedecl*dtd=_tmp57;
Cyc_CurRgn_rgnsubdtdecl(loc,env,dtd);return;}default: _LL1E: _LL1F:
 return;}_LL19:;}
# 189
return;}}_LL0:;}
# 193
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts){
# 195
for(0;ts != 0;ts=ts->tl){Cyc_CurRgn_rgnsubtype(loc,env,(void*)ts->hd);}}
# 200
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
{void*_tmp59=e->r;void*_stmttmp4=_tmp59;void*_tmp5A=_stmttmp4;struct Cyc_Absyn_Stmt*_tmp5B;struct Cyc_Absyn_Exp*_tmp5E;void**_tmp5D;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_List_List*_tmp60;void**_tmp5F;struct Cyc_List_List*_tmp62;struct Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*_tmp66;struct _tuple8*_tmp65;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_List_List*_tmp6A;struct Cyc_Absyn_Exp*_tmp69;int _tmp6E;struct Cyc_Absyn_VarargCallInfo*_tmp6D;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_Exp*_tmp6B;void**_tmp6F;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_Absyn_Exp*_tmp79;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_Absyn_Exp*_tmp7C;void**_tmp7B;void**_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_List_List*_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_Datatypefield*_tmp83;struct Cyc_Absyn_Datatypedecl*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;int _tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Vardecl*_tmp90;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp94;void**_tmp97;struct Cyc_List_List*_tmp99;void**_tmp98;switch(*((int*)_tmp5A)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 1U: _LLB: _LLC:
 goto _LL0;case 19U: _LLD: _tmp98=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp99=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LLE: {void**t=_tmp98;struct Cyc_List_List*f=_tmp99;
# 209
_tmp97=t;goto _LL10;}case 17U: _LLF: _tmp97=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL10: {void**t=_tmp97;
# 211
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmp110=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp110;});
goto _LL0;}case 6U: _LL11: _tmp94=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp95=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp96=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp94;struct Cyc_Absyn_Exp*e2=_tmp95;struct Cyc_Absyn_Exp*e3=_tmp96;
# 217
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);Cyc_CurRgn_rgnsubexp(env,e3);goto _LL0;}case 27U: _LL13: _tmp90=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp91=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp92=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_tmp93=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A)->f4;_LL14: {struct Cyc_Absyn_Vardecl*vd=_tmp90;struct Cyc_Absyn_Exp*e1=_tmp91;struct Cyc_Absyn_Exp*e2=_tmp92;int res=_tmp93;
# 219
_tmp8E=e1;_tmp8F=e2;goto _LL16;}case 4U: _LL15: _tmp8E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp8F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp8E;struct Cyc_Absyn_Exp*e2=_tmp8F;
_tmp8C=e1;_tmp8D=e2;goto _LL18;}case 35U: _LL17: _tmp8C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp8D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp8C;struct Cyc_Absyn_Exp*e2=_tmp8D;
_tmp8A=e1;_tmp8B=e2;goto _LL1A;}case 7U: _LL19: _tmp8A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp8B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp8A;struct Cyc_Absyn_Exp*e2=_tmp8B;
_tmp88=e1;_tmp89=e2;goto _LL1C;}case 8U: _LL1B: _tmp88=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp89=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp88;struct Cyc_Absyn_Exp*e2=_tmp89;
_tmp86=e1;_tmp87=e2;goto _LL1E;}case 23U: _LL1D: _tmp86=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp87=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp86;struct Cyc_Absyn_Exp*e2=_tmp87;
_tmp84=e1;_tmp85=e2;goto _LL20;}case 9U: _LL1F: _tmp84=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp85=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp84;struct Cyc_Absyn_Exp*e2=_tmp85;
# 226
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);goto _LL0;}case 31U: _LL21: _tmp81=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp82=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp83=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL22: {struct Cyc_List_List*es=_tmp81;struct Cyc_Absyn_Datatypedecl*dtd=_tmp82;struct Cyc_Absyn_Datatypefield*dtfd=_tmp83;
# 228
_tmp80=es;goto _LL24;}case 3U: _LL23: _tmp80=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL24: {struct Cyc_List_List*es=_tmp80;
# 230
_tmp7F=es;goto _LL26;}case 24U: _LL25: _tmp7F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL26: {struct Cyc_List_List*es=_tmp7F;
# 232
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 236
goto _LL0;}case 28U: _LL27: _tmp7D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp7E=(void**)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp7D;void**t=_tmp7E;
# 238
_tmp7B=t;_tmp7C=e1;goto _LL2A;}case 14U: _LL29: _tmp7B=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp7C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL2A: {void**t=_tmp7B;struct Cyc_Absyn_Exp*e1=_tmp7C;
# 240
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmp111=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp111;});
_tmp7A=e1;goto _LL2C;}case 21U: _LL2B: _tmp7A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp7A;
# 245
_tmp79=e1;goto _LL2E;}case 22U: _LL2D: _tmp79=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp79;
_tmp78=e1;goto _LL30;}case 38U: _LL2F: _tmp78=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp78;
_tmp77=e1;goto _LL32;}case 5U: _LL31: _tmp77=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp77;
_tmp76=e1;goto _LL34;}case 42U: _LL33: _tmp76=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp76;
_tmp75=e1;goto _LL36;}case 20U: _LL35: _tmp75=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp75;
_tmp74=e1;goto _LL38;}case 15U: _LL37: _tmp74=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp74;
_tmp73=e1;goto _LL3A;}case 18U: _LL39: _tmp73=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp73;
_tmp72=e1;goto _LL3C;}case 41U: _LL3B: _tmp72=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp72;
_tmp71=e1;goto _LL3E;}case 11U: _LL3D: _tmp71=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp71;
_tmp70=e1;goto _LL40;}case 12U: _LL3F: _tmp70=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp70;
Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 39U: _LL41: _tmp6F=(void**)&((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL42: {void**t=_tmp6F;
# 257
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel){
void*_tmp9A=Cyc_Tcutil_substitute_nofun(env.inst,*t);void*new_typ=_tmp9A;
if(new_typ == *t)goto _LL0;{
# 262
void*_tmp9B=Cyc_Absyn_compress(new_typ);void*_stmttmp5=_tmp9B;void*_tmp9C=_stmttmp5;struct Cyc_Absyn_Exp*_tmp9D;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9C)->tag == 9U){_LL58: _tmp9D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9C)->f1;_LL59: {struct Cyc_Absyn_Exp*e1=_tmp9D;
# 264
e->r=e1->r;
e->topt=e1->topt;
e->annot=e1->annot;
goto _LL57;}}else{_LL5A: _LL5B:
*t=new_typ;goto _LL57;}_LL57:;}}
# 271
goto _LL0;}case 10U: _LL43: _tmp6B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp6C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp6D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_tmp6E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5A)->f4;_LL44: {struct Cyc_Absyn_Exp*e1=_tmp6B;struct Cyc_List_List*es=_tmp6C;struct Cyc_Absyn_VarargCallInfo*vararg=_tmp6D;int resv=_tmp6E;
# 273
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 277
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 13U: _LL45: _tmp69=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp6A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL46: {struct Cyc_Absyn_Exp*e1=_tmp69;struct Cyc_List_List*ts=_tmp6A;
# 280
while(ts != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_tmp112=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_tmp112;});
ts=ts->tl;}
# 286
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 16U: _LL47: _tmp67=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp68=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL48: {struct Cyc_Absyn_Exp*eopt=_tmp67;struct Cyc_Absyn_Exp*e1=_tmp68;
# 290
Cyc_CurRgn_rgnsubexpopt(env,eopt);Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 25U: _LL49: _tmp65=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp66=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL4A: {struct _tuple8*i=_tmp65;struct Cyc_List_List*ds=_tmp66;
# 293
Cyc_CurRgn_rgnsubtype(e->loc,env,(*i).f3);
if(!env.toplevel)
({void*_tmp113=Cyc_Tcutil_substitute_nofun(env.inst,(*i).f3);(*i).f3=_tmp113;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 36U: _LL4B: _tmp64=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL4C: {struct Cyc_List_List*ds=_tmp64;
# 299
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 26U: _LL4D: _tmp63=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL4E: {struct Cyc_List_List*ds=_tmp63;
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 29U: _LL4F: _tmp61=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_tmp62=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5A)->f3;_LL50: {struct Cyc_List_List*ts=_tmp61;struct Cyc_List_List*ds=_tmp62;
# 303
for(0;ts != 0;ts=ts->tl){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_tmp114=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_tmp114;});}
# 308
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 30U: _LL51: _tmp5F=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_tmp60=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5A)->f2;_LL52: {void**t=_tmp5F;struct Cyc_List_List*ds=_tmp60;
# 312
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmp115=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp115;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 34U: _LL53: _tmp5C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5A)->f1).rgn;_tmp5D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5A)->f1).elt_type;_tmp5E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5A)->f1).num_elts;_LL54: {struct Cyc_Absyn_Exp*rgnopt=_tmp5C;void**elt_t=_tmp5D;struct Cyc_Absyn_Exp*e1=_tmp5E;
# 319
!env.toplevel?0:({struct _fat_ptr _tmp116=({const char*_tmp9E="!env.toplevel";_tag_fat(_tmp9E,sizeof(char),14U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp116,({const char*_tmp9F="currgn.cyc";_tag_fat(_tmp9F,sizeof(char),11U);}),319U);});
Cyc_CurRgn_rgnsubexpopt(env,rgnopt);
if(elt_t != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,*elt_t);
({void*_tmp117=Cyc_Tcutil_substitute_nofun(env.inst,*elt_t);*elt_t=_tmp117;});}
# 325
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}default: _LL55: _tmp5B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL56: {struct Cyc_Absyn_Stmt*s=_tmp5B;
# 328
Cyc_CurRgn_rgnsubstmt(env,s);goto _LL0;}}_LL0:;}
# 330
if(e->topt != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)_check_null(e->topt));
if(!env.toplevel)
({void*_tmp118=Cyc_Tcutil_substitute_nofun(env.inst,(void*)_check_null(e->topt));e->topt=_tmp118;});}}
# 347 "currgn.cyc"
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
if(e == 0)return;else{
Cyc_CurRgn_rgnsubexp(env,e);}}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 352
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
# 354
for(0;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubexp(env,(*((struct _tuple13*)ds->hd)).f2);}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 358
static void Cyc_CurRgn_rgnsubpat(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Pat*p){
!env.toplevel?0:({struct _fat_ptr _tmp119=({const char*_tmpA0="!env.toplevel";_tag_fat(_tmpA0,sizeof(char),14U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp119,({const char*_tmpA1="currgn.cyc";_tag_fat(_tmpA1,sizeof(char),11U);}),359U);});{
void*_tmpA2=p->r;void*_stmttmp6=_tmpA2;void*_tmpA3=_stmttmp6;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA6;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_Pat*_tmpAA;struct Cyc_Absyn_Vardecl*_tmpAC;struct Cyc_Absyn_Tvar*_tmpAB;struct Cyc_Absyn_Vardecl*_tmpAE;struct Cyc_Absyn_Tvar*_tmpAD;struct Cyc_Absyn_Pat*_tmpB0;struct Cyc_Absyn_Vardecl*_tmpAF;struct Cyc_Absyn_Pat*_tmpB2;struct Cyc_Absyn_Vardecl*_tmpB1;switch(*((int*)_tmpA3)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmpB1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_tmpB2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmpA3)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpB1;struct Cyc_Absyn_Pat*p1=_tmpB2;
# 363
_tmpAF=vd;_tmpB0=p1;goto _LL6;}case 3U: _LL5: _tmpAF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_tmpB0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmpA3)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmpAF;struct Cyc_Absyn_Pat*p1=_tmpB0;
# 366
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmp11A=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmp11A;});}
# 370
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 2U: _LL7: _tmpAD=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_tmpAE=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmpA3)->f2;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmpAD;struct Cyc_Absyn_Vardecl*vd=_tmpAE;
# 374
Cyc_CurRgn_check_tvar(p->loc,tv);
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmp11B=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmp11B;});}
# 379
return;}case 4U: _LL9: _tmpAB=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_tmpAC=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmpA3)->f2;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmpAB;struct Cyc_Absyn_Vardecl*vd=_tmpAC;
# 383
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmp11C=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmp11C;});}
# 387
Cyc_CurRgn_check_tvar(p->loc,tv);
return;}case 6U: _LLB: _tmpAA=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_LLC: {struct Cyc_Absyn_Pat*p1=_tmpAA;
# 391
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 7U: _LLD: _tmpA8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpA3)->f2;_tmpA9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpA3)->f3;_LLE: {struct Cyc_List_List*tvs=_tmpA8;struct Cyc_List_List*ds=_tmpA9;
# 395
for(0;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(p->loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}
for(0;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubpat(env,(*((struct _tuple14*)ds->hd)).f2);}
# 400
return;}case 8U: _LLF: _tmpA7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmpA3)->f3;_LL10: {struct Cyc_List_List*ps=_tmpA7;
# 403
_tmpA6=ps;goto _LL12;}case 5U: _LL11: _tmpA6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_LL12: {struct Cyc_List_List*ps=_tmpA6;
_tmpA5=ps;goto _LL14;}case 16U: _LL13: _tmpA5=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmpA3)->f2;_LL14: {struct Cyc_List_List*ps=_tmpA5;
# 406
for(0;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubpat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 9U: _LL15: _LL16:
# 410
 goto _LL18;case 10U: _LL17: _LL18:
 goto _LL1A;case 11U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 goto _LL1E;case 13U: _LL1D: _LL1E:
 goto _LL20;case 14U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return;default: _LL23: _tmpA4=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmpA3)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmpA4;
# 419
Cyc_CurRgn_rgnsubexp(env,e);
return;}}_LL0:;}}struct _tuple15{void*f1;void*f2;};
# 424
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad){
Cyc_CurRgn_check_tvars(loc,ad->tvs);
if(ad->impl != 0){
Cyc_CurRgn_check_tvars(loc,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);{
struct Cyc_List_List*_tmpB3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*ps=_tmpB3;
for(0;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f1);
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f2);}{
# 433
struct Cyc_List_List*_tmpB4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmpB4;
for(0;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}}}}}
# 441
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud){
Cyc_CurRgn_check_tvars(loc,tud->tvs);
if(tud->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_List_List*_tmpB5=((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs;struct Cyc_List_List*ts=_tmpB5;
while(ts != 0){
struct _tuple12*_tmpB6=(struct _tuple12*)ts->hd;struct _tuple12*_stmttmp7=_tmpB6;struct _tuple12*_tmpB7=_stmttmp7;void**_tmpB8;_LL1: _tmpB8=(void**)& _tmpB7->f2;_LL2: {void**typ=_tmpB8;
Cyc_CurRgn_rgnsubtype(loc,env,*typ);
ts=ts->tl;}}}}}
# 455
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds);
static void Cyc_CurRgn_rgnsubdecl(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Decl*d){
void*_tmpB9=d->r;void*_stmttmp8=_tmpB9;void*_tmpBA=_stmttmp8;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Pat*_tmpBC;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpC0;struct _fat_ptr*_tmpBF;struct Cyc_Absyn_Datatypedecl*_tmpC1;struct Cyc_Absyn_Aggrdecl*_tmpC2;struct Cyc_Absyn_Typedefdecl*_tmpC3;struct Cyc_Absyn_Fndecl*_tmpC4;struct Cyc_Absyn_Vardecl*_tmpC5;switch(*((int*)_tmpBA)){case 0U: _LL1: _tmpC5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmpC5;
# 459
struct Cyc_Absyn_Vardecl*_tmpC6=vd;struct Cyc_Absyn_Exp*_tmpC8;void**_tmpC7;_LL24: _tmpC7=(void**)& _tmpC6->type;_tmpC8=_tmpC6->initializer;_LL25: {void**t=_tmpC7;struct Cyc_Absyn_Exp*initializer=_tmpC8;
Cyc_CurRgn_rgnsubexpopt(env,initializer);
Cyc_CurRgn_rgnsubtype(d->loc,env,*t);
if(!env.toplevel)
({void*_tmp11D=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmp11D;});
return;}}case 1U: _LL3: _tmpC4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmpC4;
# 472
env.toplevel=0;
({struct Cyc_List_List*_tmp11F=({struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct _tuple11*_tmp11E=({struct _tuple11*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmpC9->f2=(void*)_check_null(Cyc_CurRgn_curr_rgn_typ);_tmpC9;});_tmpCA->hd=_tmp11E;}),_tmpCA->tl=0;_tmpCA;});env.inst=_tmp11F;});
Cyc_CurRgn_rgnsubstmt(env,fd->body);
# 477
return;}case 8U: _LL5: _tmpC3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmpC3;
# 480
env.toplevel?0:({struct _fat_ptr _tmp120=({const char*_tmpCB="env.toplevel";_tag_fat(_tmpCB,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp120,({const char*_tmpCC="currgn.cyc";_tag_fat(_tmpCC,sizeof(char),11U);}),480U);});
Cyc_CurRgn_check_tvars(d->loc,td->tvs);
if(td->defn != 0)Cyc_CurRgn_rgnsubtype(d->loc,env,(void*)_check_null(td->defn));
return;}case 5U: _LL7: _tmpC2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LL8: {struct Cyc_Absyn_Aggrdecl*ad=_tmpC2;
# 486
env.toplevel?0:({struct _fat_ptr _tmp121=({const char*_tmpCD="env.toplevel";_tag_fat(_tmpCD,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp121,({const char*_tmpCE="currgn.cyc";_tag_fat(_tmpCE,sizeof(char),11U);}),486U);});
Cyc_CurRgn_rgnsubaggrdecl(d->loc,env,ad);
return;}case 6U: _LL9: _tmpC1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmpC1;
# 491
env.toplevel?0:({struct _fat_ptr _tmp122=({const char*_tmpCF="env.toplevel";_tag_fat(_tmpCF,sizeof(char),13U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp122,({const char*_tmpD0="currgn.cyc";_tag_fat(_tmpD0,sizeof(char),11U);}),491U);});
Cyc_CurRgn_rgnsubdtdecl(d->loc,env,tud);
return;}case 11U: _LLB: _LLC:
# 497
 goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 15U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 return;case 9U: _LL19: _tmpBF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_tmpC0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpBA)->f2;_LL1A: {struct _fat_ptr*v=_tmpBF;struct Cyc_List_List*ds2=_tmpC0;
# 505
_tmpBE=ds2;goto _LL1C;}case 10U: _LL1B: _tmpBE=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpBA)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmpBE;
Cyc_CurRgn_rgnsubdecls(env,ds2);return;}case 2U: _LL1D: _tmpBC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_tmpBD=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBA)->f3;_LL1E: {struct Cyc_Absyn_Pat*p=_tmpBC;struct Cyc_Absyn_Exp*e=_tmpBD;
# 509
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexp(env,e);
return;}case 3U: _LL1F: _tmpBB=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LL20: {struct Cyc_List_List*vds=_tmpBB;
# 514
!env.toplevel?0:({struct _fat_ptr _tmp123=({const char*_tmpD1="!env.toplevel";_tag_fat(_tmpD1,sizeof(char),14U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp123,({const char*_tmpD2="currgn.cyc";_tag_fat(_tmpD2,sizeof(char),11U);}),514U);});
for(0;vds != 0;vds=vds->tl){
Cyc_CurRgn_rgnsubtype(d->loc,env,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);
({void*_tmp124=Cyc_Tcutil_substitute_nofun(env.inst,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);((struct Cyc_Absyn_Vardecl*)vds->hd)->type=_tmp124;});}
# 519
return;}default: _LL21: _LL22:
# 521
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp125=({const char*_tmpD3="internal compiler error: region at toplevel";_tag_fat(_tmpD3,sizeof(char),44U);});_tmpD4->f1=_tmp125;});_tmpD4;}));}_LL0:;}
# 535 "currgn.cyc"
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s){
void*_tmpD5=s->r;void*_stmttmp9=_tmpD5;void*_tmpD6=_stmttmp9;struct Cyc_Absyn_Stmt*_tmpD8;struct Cyc_Absyn_Decl*_tmpD7;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_Stmt*_tmpD9;struct Cyc_List_List*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Stmt*_tmpDF;struct _fat_ptr*_tmpDE;struct Cyc_Absyn_Stmt*_tmpE5;struct Cyc_Absyn_Stmt*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Stmt*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Stmt*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Stmt*_tmpE8;struct Cyc_Absyn_Stmt*_tmpEC;struct Cyc_Absyn_Stmt*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Stmt*_tmpEF;struct Cyc_Absyn_Stmt*_tmpEE;struct Cyc_Absyn_Exp*_tmpF0;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 1U: _LL9: _tmpF0=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmpF0;
# 542
Cyc_CurRgn_rgnsubexp(env,e);return;}case 2U: _LLB: _tmpEE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpEF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_LLC: {struct Cyc_Absyn_Stmt*s1=_tmpEE;struct Cyc_Absyn_Stmt*s2=_tmpEF;
Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 3U: _LLD: _tmpED=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_LLE: {struct Cyc_Absyn_Exp*eopt=_tmpED;
Cyc_CurRgn_rgnsubexpopt(env,eopt);return;}case 4U: _LLF: _tmpEA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpEB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_tmpEC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpD6)->f3;_LL10: {struct Cyc_Absyn_Exp*e=_tmpEA;struct Cyc_Absyn_Stmt*s1=_tmpEB;struct Cyc_Absyn_Stmt*s2=_tmpEC;
# 547
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 14U: _LL11: _tmpE8=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpE9=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmpE8;struct Cyc_Absyn_Exp*e=_tmpE9;
# 549
_tmpE6=e;_tmpE7=s;goto _LL14;}case 5U: _LL13: _tmpE6=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1).f1;_tmpE7=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmpE6;struct Cyc_Absyn_Stmt*s1=_tmpE7;
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 9U: _LL15: _tmpE0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpE1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2).f1;_tmpE2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2).f2;_tmpE3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpD6)->f3).f1;_tmpE4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpD6)->f3).f2;_tmpE5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpD6)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpE0;struct Cyc_Absyn_Exp*e2=_tmpE1;struct Cyc_Absyn_Stmt*guard_s=_tmpE2;struct Cyc_Absyn_Exp*e3=_tmpE3;struct Cyc_Absyn_Stmt*cont_s=_tmpE4;struct Cyc_Absyn_Stmt*s1=_tmpE5;
# 553
Cyc_CurRgn_rgnsubexp(env,e1);
Cyc_CurRgn_rgnsubexp(env,e2);
Cyc_CurRgn_rgnsubexp(env,e3);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}case 13U: _LL17: _tmpDE=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpDF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_LL18: {struct _fat_ptr*l=_tmpDE;struct Cyc_Absyn_Stmt*s1=_tmpDF;
# 559
Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 11U: _LL19: _tmpDD=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_LL1A: {struct Cyc_List_List*es=_tmpDD;
# 562
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 566
return;}case 10U: _LL1B: _tmpDB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpDC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmpDB;struct Cyc_List_List*scs=_tmpDC;
# 569
Cyc_CurRgn_rgnsubexp(env,e);
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_tmpF1=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpA=_tmpF1;struct Cyc_Absyn_Switch_clause*_tmpF2=_stmttmpA;struct Cyc_Absyn_Stmt*_tmpF5;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Pat*_tmpF3;_LL22: _tmpF3=_tmpF2->pattern;_tmpF4=_tmpF2->where_clause;_tmpF5=_tmpF2->body;_LL23: {struct Cyc_Absyn_Pat*p=_tmpF3;struct Cyc_Absyn_Exp*eopt=_tmpF4;struct Cyc_Absyn_Stmt*s1=_tmpF5;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 577
return;}case 15U: _LL1D: _tmpD9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpDA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_LL1E: {struct Cyc_Absyn_Stmt*s1=_tmpD9;struct Cyc_List_List*scs=_tmpDA;
# 580
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_tmpF6=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpB=_tmpF6;struct Cyc_Absyn_Switch_clause*_tmpF7=_stmttmpB;struct Cyc_Absyn_Stmt*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Pat*_tmpF8;_LL25: _tmpF8=_tmpF7->pattern;_tmpF9=_tmpF7->where_clause;_tmpFA=_tmpF7->body;_LL26: {struct Cyc_Absyn_Pat*p=_tmpF8;struct Cyc_Absyn_Exp*eopt=_tmpF9;struct Cyc_Absyn_Stmt*s1=_tmpFA;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 587
return;}default: _LL1F: _tmpD7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD6)->f1;_tmpD8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD6)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmpD7;struct Cyc_Absyn_Stmt*s1=_tmpD8;
# 590
void*_tmpFB=d->r;void*_stmttmpC=_tmpFB;void*_tmpFC=_stmttmpC;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Vardecl*_tmpFE;struct Cyc_Absyn_Tvar*_tmpFD;if(((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpFC)->tag == 4U){_LL28: _tmpFD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpFC)->f1;_tmpFE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpFC)->f2;_tmpFF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpFC)->f3;_LL29: {struct Cyc_Absyn_Tvar*tv=_tmpFD;struct Cyc_Absyn_Vardecl*vd=_tmpFE;struct Cyc_Absyn_Exp*open_exp_opt=_tmpFF;
# 592
Cyc_CurRgn_check_tvar(d->loc,tv);
Cyc_CurRgn_rgnsubexpopt(env,open_exp_opt);
# 597
if(open_exp_opt == 0){
({struct Cyc_List_List*_tmp128=({struct Cyc_List_List*_tmp101=_cycalloc(sizeof(*_tmp101));({struct _tuple11*_tmp127=({struct _tuple11*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),({void*_tmp126=Cyc_Absyn_var_type(tv);_tmp100->f2=_tmp126;});_tmp100;});_tmp101->hd=_tmp127;}),_tmp101->tl=0;_tmp101;});env.inst=_tmp128;});
Cyc_CurRgn_rgnsubstmt(env,s1);}else{
# 602
Cyc_CurRgn_rgnsubstmt(env,s1);}
return;}}else{_LL2A: _LL2B:
# 605
 Cyc_CurRgn_rgnsubdecl(env,d);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}_LL27:;}}_LL0:;}
# 622 "currgn.cyc"
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}}
# 628
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*tds){
Cyc_CurRgn_curr_rgn_type();{
struct Cyc_CurRgn_rgnsubenv env=({struct Cyc_CurRgn_rgnsubenv _tmp104;_tmp104.inst=0,_tmp104.in_externC=0,_tmp104.toplevel=1;_tmp104;});
Cyc_CurRgn_rgnsubdecls(env,tds);}}
