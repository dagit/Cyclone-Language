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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_Hashtable_Table;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 847 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 864
void*Cyc_Absyn_compress(void*);
# 895
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*,void*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
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
Cyc_CurRgn_curr_tvar=({struct Cyc_Absyn_Tvar*_tmp2=_cycalloc(sizeof(*_tmp2));({struct _fat_ptr*_tmp96=({struct _fat_ptr*_tmp1=_cycalloc(sizeof(*_tmp1));*_tmp1=Cyc_CurRgn_curr_rgn_name;_tmp1;});_tmp2->name=_tmp96;}),({
int _tmp95=Cyc_Tcutil_new_tvar_id();_tmp2->identity=_tmp95;}),({
void*_tmp94=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp2->kind=_tmp94;});_tmp2;});
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar));}
# 45
return(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);}
# 48
void*Cyc_CurRgn_curr_rgn_type (void){
Cyc_CurRgn_curr_rgn_tvar();
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar));
return(void*)_check_null(Cyc_CurRgn_curr_rgn_typ);}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 54
void Cyc_CurRgn_subst_inst(struct Cyc_List_List*inst,struct Cyc_Absyn_Tvar*tv,void*t){
while(inst != 0){
struct _tuple11*_tmp3=(struct _tuple11*)inst->hd;struct _tuple11*_stmttmp0=_tmp3;struct _tuple11*_tmp4=_stmttmp0;void*_tmp6;void*_tmp5;_tmp5=(struct Cyc_Absyn_Tvar**)& _tmp4->f1;_tmp6=(void**)& _tmp4->f2;{struct Cyc_Absyn_Tvar**tvi=(struct Cyc_Absyn_Tvar**)_tmp5;void**ti=(void**)_tmp6;
if(Cyc_Absyn_tvar_cmp(*tvi,tv)== 0){
*ti=t;
return;}
# 61
inst=inst->tl;}}}
# 68
static struct _tuple8*Cyc_CurRgn_argtype(struct _tuple8*arg){
struct _tuple8*_tmp7=arg;void*_tmpA;struct Cyc_Absyn_Tqual _tmp9;void*_tmp8;_tmp8=_tmp7->f1;_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;{struct _fat_ptr*vopt=_tmp8;struct Cyc_Absyn_Tqual tq=_tmp9;void*t=_tmpA;
{void*_tmpB=t;void*_tmpC;if(*((int*)_tmpB)== 3){_tmpC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB)->f1).elt_type;{void*t1=_tmpC;
# 72
if(!Cyc_Tcutil_is_function_type(t1))return arg;
goto _LL3;}}else{
return arg;}_LL3:;}{
# 76
void*_tmpD=Cyc_Tcutil_copy_type(t);void*t2=_tmpD;
void*_tmpE=Cyc_Absyn_compress(t2);void*_stmttmp1=_tmpE;void*_tmpF=_stmttmp1;void*_tmp10;if(*((int*)_tmpF)== 3){_tmp10=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF)->f1).elt_type;{void*t1=_tmp10;
# 79
void*_tmp11=Cyc_Absyn_compress(t1);void*_stmttmp2=_tmp11;void*_tmp12=_stmttmp2;void*_tmp20;void*_tmp1F;void*_tmp1E;void*_tmp1D;void*_tmp1C;void*_tmp1B;void*_tmp1A;void*_tmp19;int _tmp18;void*_tmp17;void*_tmp16;struct Cyc_Absyn_Tqual _tmp15;void*_tmp14;void*_tmp13;if(*((int*)_tmp12)== 5){_tmp13=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).tvars;_tmp14=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).effect;_tmp15=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).ret_tqual;_tmp16=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).ret_type;_tmp17=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).args;_tmp18=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).c_varargs;_tmp19=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).cyc_varargs;_tmp1A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).rgn_po;_tmp1B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).attributes;_tmp1C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).requires_clause;_tmp1D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).requires_relns;_tmp1E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).ensures_clause;_tmp1F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).ensures_relns;_tmp20=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12)->f1).return_value;{struct Cyc_List_List**tvars=(struct Cyc_List_List**)_tmp13;void*eff=_tmp14;struct Cyc_Absyn_Tqual res_tq=_tmp15;void*res_typ=_tmp16;struct Cyc_List_List*args_info=_tmp17;int c_vararg=_tmp18;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp19;struct Cyc_List_List*rgn_po=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*req=_tmp1C;struct Cyc_List_List*req_relns=_tmp1D;struct Cyc_Absyn_Exp*ens=_tmp1E;struct Cyc_List_List*ens_relns=_tmp1F;struct Cyc_Absyn_Vardecl*ret_var=_tmp20;
# 85
struct Cyc_List_List*_tmp21=*tvars;struct Cyc_List_List*tvars2=_tmp21;
({struct Cyc_List_List*_tmp97=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmp22->tl=tvars2;_tmp22;});*tvars=_tmp97;});
return({struct _tuple8*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->f1=vopt,_tmp23->f2=tq,_tmp23->f3=t2;_tmp23;});}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp98=({const char*_tmp24="internal compiler error: not a function type";_tag_fat(_tmp24,sizeof(char),45U);});_tmp25->f1=_tmp98;});_tmp25;}));};}}else{
# 90
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp99=({const char*_tmp26="internal compiler error: not a pointer type";_tag_fat(_tmp26,sizeof(char),44U);});_tmp27->f1=_tmp99;});_tmp27;}));};}}}
# 95
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn){
void*_tmp28=fntype;void*_tmp36;void*_tmp35;void*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;void*_tmp2F;int _tmp2E;void*_tmp2D;void*_tmp2C;struct Cyc_Absyn_Tqual _tmp2B;void*_tmp2A;void*_tmp29;if(*((int*)_tmp28)== 5){_tmp29=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).tvars;_tmp2A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).effect;_tmp2B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).ret_tqual;_tmp2C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).ret_type;_tmp2D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).args;_tmp2E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).c_varargs;_tmp2F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).cyc_varargs;_tmp30=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).rgn_po;_tmp31=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).attributes;_tmp32=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).requires_clause;_tmp33=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).requires_relns;_tmp34=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).ensures_clause;_tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).ensures_relns;_tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp28)->f1).return_value;{struct Cyc_List_List*tvars=_tmp29;void*eff=_tmp2A;struct Cyc_Absyn_Tqual res_tq=_tmp2B;void*res_typ=_tmp2C;struct Cyc_List_List**args_info=(struct Cyc_List_List**)_tmp2D;int c_vararg=_tmp2E;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp2F;struct Cyc_List_List*rgn_po=_tmp30;struct Cyc_List_List*atts=_tmp31;struct Cyc_Absyn_Exp*req=_tmp32;struct Cyc_List_List*req_relns=_tmp33;struct Cyc_Absyn_Exp*ens=_tmp34;struct Cyc_List_List*ens_relns=_tmp35;struct Cyc_Absyn_Vardecl*ret_var=_tmp36;
# 103
struct Cyc_List_List*_tmp37=*args_info;struct Cyc_List_List*old_args_info=_tmp37;
# 105
({struct Cyc_List_List*_tmp9A=({(struct Cyc_List_List*(*)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_CurRgn_argtype,*args_info);*args_info=_tmp9A;});{
# 107
struct Cyc_List_List*_tmp38=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct _tuple11*_tmp9B=({struct _tuple11*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmp3C->f2=currgn;_tmp3C;});_tmp3D->hd=_tmp9B;}),_tmp3D->tl=0;_tmp3D;});struct Cyc_List_List*inst=_tmp38;
void*_tmp39=Cyc_Tcutil_substitute(inst,fntype);void*fntype2=_tmp39;
# 110
{void*_tmp3A=fntype2;void*_tmp3B;if(*((int*)_tmp3A)== 5){_tmp3B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3A)->f1).args;{struct Cyc_List_List**args_info2=(struct Cyc_List_List**)_tmp3B;
# 112
*args_info2=old_args_info;
goto _LL5;}}else{
goto _LL5;}_LL5:;}
# 116
*args_info=old_args_info;
return fntype2;}}}else{
return fntype;};}
# 123
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))!= 0;}
# 128
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))== 0;}struct Cyc_CurRgn_rgnsubenv{struct Cyc_List_List*inst;int in_externC;int toplevel;};
# 138
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Stmt*);
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Exp*);
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Exp*);
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
# 143
static void Cyc_CurRgn_rgnsubdtdecl(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Datatypedecl*);
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Aggrdecl*);
# 147
static void Cyc_CurRgn_check_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
if(Cyc_Absyn_tvar_cmp(tv,(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar))== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp92;_tmp92.tag=0,({struct _fat_ptr _tmp9C=({const char*_tmp41="Cannot abstract special type variable ";_tag_fat(_tmp41,sizeof(char),39U);});_tmp92.f1=_tmp9C;});_tmp92;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp40=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp91;_tmp91.tag=7,_tmp91.f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);_tmp91;});void*_tmp3E[2];_tmp3E[0]=& _tmp3F,_tmp3E[1]=& _tmp40;({unsigned _tmp9D=loc;Cyc_Warn_err2(_tmp9D,_tag_fat(_tmp3E,sizeof(void*),2));});});}
# 151
static void Cyc_CurRgn_check_tvars(unsigned loc,struct Cyc_List_List*tvs){
for(1;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}}
# 156
static void Cyc_CurRgn_rgnsubtypes(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 159
static void Cyc_CurRgn_rgnsubtype(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,void*type){
void*_tmp42=Cyc_Absyn_compress(type);void*_stmttmp3=_tmp42;void*_tmp43=_stmttmp3;void*_tmp48;void*_tmp47;void*_tmp46;void*_tmp45;void*_tmp44;switch(*((int*)_tmp43)){case 2: _tmp44=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp43)->f1;{struct Cyc_Absyn_Tvar*v=_tmp44;
# 162
if(env.toplevel)Cyc_CurRgn_check_tvar(loc,v);return;}case 8: _tmp44=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43)->f2;{struct Cyc_List_List*ts=_tmp44;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 4: _tmp44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43)->f1).elt_type;_tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43)->f1).num_elts;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43)->f1).zero_term;{void*t1=_tmp44;struct Cyc_Absyn_Exp*e=_tmp45;void*zt=_tmp46;
# 165
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubexpopt(env,e);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 3: _tmp44=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43)->f1).elt_type;_tmp45=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43)->f1).ptr_atts).rgn;_tmp46=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43)->f1).ptr_atts).bounds;_tmp47=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43)->f1).ptr_atts).zero_term;_tmp48=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43)->f1).ptr_atts).autoreleased;{void*t1=_tmp44;void*r=_tmp45;void*b=_tmp46;void*zt=_tmp47;void*rel=_tmp48;
# 170
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubtype(loc,env,r);
Cyc_CurRgn_rgnsubtype(loc,env,b);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
Cyc_CurRgn_rgnsubtype(loc,env,rel);
return;}case 5:
# 177
 return;case 6: _tmp44=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp43)->f1;{struct Cyc_List_List*tqts=_tmp44;
# 179
{struct Cyc_List_List*_tmp49=tqts;struct Cyc_List_List*ts1=_tmp49;for(0;ts1 != 0;ts1=ts1->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple12*)ts1->hd)).f2);}}
# 182
return;}case 7: _tmp44=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43)->f2;{struct Cyc_List_List*fs=_tmp44;
# 184
for(1;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}
# 188
return;}case 1: _tmp44=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp43)->f2;{void*r=_tmp44;
# 190
if(r != 0)Cyc_CurRgn_rgnsubtype(loc,env,r);
return;}case 0: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43)->f2;{void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 9: _tmp44=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43)->f1;{struct Cyc_Absyn_Exp*e=_tmp44;
Cyc_CurRgn_rgnsubexp(env,e);return;}case 11: _tmp44=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp43)->f1;{struct Cyc_Absyn_Exp*e=_tmp44;
Cyc_CurRgn_rgnsubexp(env,e);return;}default: _tmp44=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp43)->f1;_tmp45=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp43)->f2;{struct Cyc_Absyn_TypeDecl*td=_tmp44;void**topt=_tmp45;
# 196
topt == 0?0:({int(*_tmp9F)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp9E=({const char*_tmp4A="topt == NULL";_tag_fat(_tmp4A,sizeof(char),13U);});_tmp9F(_tmp9E,({const char*_tmp4B="currgn.cyc";_tag_fat(_tmp4B,sizeof(char),11U);}),196U);});
{void*_tmp4C=td->r;void*_stmttmp4=_tmp4C;void*_tmp4D=_stmttmp4;void*_tmp4E;switch(*((int*)_tmp4D)){case 0: _tmp4E=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp4D)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp4E;
Cyc_CurRgn_rgnsubaggrdecl(loc,env,ad);return;}case 2: _tmp4E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp4D)->f1;{struct Cyc_Absyn_Datatypedecl*dtd=_tmp4E;
Cyc_CurRgn_rgnsubdtdecl(loc,env,dtd);return;}default:
 return;};}
# 202
return;}};}
# 206
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts){
# 208
for(1;ts != 0;ts=ts->tl){Cyc_CurRgn_rgnsubtype(loc,env,(void*)ts->hd);}}
# 213
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
{void*_tmp4F=e->r;void*_stmttmp5=_tmp4F;void*_tmp50=_stmttmp5;int _tmp54;void*_tmp53;void*_tmp52;void*_tmp51;switch(*((int*)_tmp50)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 40: _LLA:
 goto _LLC;case 1: _LLC:
 goto _LL0;case 19: _tmp51=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{void**t=_tmp51;struct Cyc_List_List*f=_tmp52;
# 222
_tmp51=t;goto _LL10;}case 17: _tmp51=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL10: {void**t=_tmp51;
# 224
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmpA0=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmpA0;});
goto _LL0;}case 6: _tmp51=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_tmp53=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50)->f3;{struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;struct Cyc_Absyn_Exp*e3=_tmp53;
# 230
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);Cyc_CurRgn_rgnsubexp(env,e3);goto _LL0;}case 27: _tmp51=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_tmp53=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp50)->f3;_tmp54=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp50)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp51;struct Cyc_Absyn_Exp*e1=_tmp52;struct Cyc_Absyn_Exp*e2=_tmp53;int res=_tmp54;
# 232
_tmp51=e1;_tmp52=e2;goto _LL16;}case 4: _tmp51=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp50)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL18;}case 35: _tmp51=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL1A;}case 7: _tmp51=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL1C;}case 8: _tmp51=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL1E;}case 23: _tmp51=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL20;}case 9: _tmp51=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
# 239
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);goto _LL0;}case 31: _tmp51=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_tmp53=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp50)->f3;{struct Cyc_List_List*es=_tmp51;struct Cyc_Absyn_Datatypedecl*dtd=_tmp52;struct Cyc_Absyn_Datatypefield*dtfd=_tmp53;
# 241
_tmp51=es;goto _LL24;}case 3: _tmp51=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL24: {struct Cyc_List_List*es=_tmp51;
# 243
_tmp51=es;goto _LL26;}case 24: _tmp51=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL26: {struct Cyc_List_List*es=_tmp51;
# 245
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 249
goto _LL0;}case 28: _tmp51=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=(void**)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{struct Cyc_Absyn_Exp*e1=_tmp51;void**t=_tmp52;
# 251
_tmp51=t;_tmp52=e1;goto _LL2A;}case 14: _tmp51=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL2A: {void**t=_tmp51;struct Cyc_Absyn_Exp*e1=_tmp52;
# 253
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmpA1=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmpA1;});
_tmp51=e1;goto _LL2C;}case 21: _tmp51=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp51;
# 258
_tmp51=e1;goto _LL2E;}case 22: _tmp51=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL30;}case 38: _tmp51=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL32;}case 5: _tmp51=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL34;}case 42: _tmp51=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL36;}case 20: _tmp51=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL38;}case 15: _tmp51=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL3A;}case 18: _tmp51=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL3C;}case 41: _tmp51=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL3E;}case 11: _tmp51=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp51;
_tmp51=e1;goto _LL40;}case 12: _tmp51=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp51;
Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 39: _tmp51=(void**)&((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp50)->f1;{void**t=_tmp51;
# 270
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel){
void*_tmp55=Cyc_Tcutil_substitute_nofun(env.inst,*t);void*new_typ=_tmp55;
if(new_typ == *t)goto _LL0;{
# 275
void*_tmp56=Cyc_Absyn_compress(new_typ);void*_stmttmp6=_tmp56;void*_tmp57=_stmttmp6;void*_tmp58;if(*((int*)_tmp57)== 9){_tmp58=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp57)->f1;{struct Cyc_Absyn_Exp*e1=_tmp58;
# 277
e->r=e1->r;
e->topt=e1->topt;
e->annot=e1->annot;
goto _LL57;}}else{
*t=new_typ;goto _LL57;}_LL57:;}}
# 284
goto _LL0;}case 10: _tmp51=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_tmp53=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp50)->f3;_tmp54=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp50)->f4;{struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_List_List*es=_tmp52;struct Cyc_Absyn_VarargCallInfo*vararg=_tmp53;int resv=_tmp54;
# 286
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 290
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 13: _tmp51=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_List_List*ts=_tmp52;
# 293
while(ts != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_tmpA2=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_tmpA2;});
ts=ts->tl;}
# 299
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 16: _tmp51=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{struct Cyc_Absyn_Exp*eopt=_tmp51;struct Cyc_Absyn_Exp*e1=_tmp52;
# 303
Cyc_CurRgn_rgnsubexpopt(env,eopt);Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 25: _tmp51=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{struct _tuple8*i=_tmp51;struct Cyc_List_List*ds=_tmp52;
# 306
Cyc_CurRgn_rgnsubtype(e->loc,env,(*i).f3);
if(!env.toplevel)
({void*_tmpA3=Cyc_Tcutil_substitute_nofun(env.inst,(*i).f3);(*i).f3=_tmpA3;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 36: _tmp51=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{struct Cyc_List_List*ds=_tmp51;
# 312
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 26: _tmp51=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp50)->f1;{struct Cyc_List_List*ds=_tmp51;
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 29: _tmp51=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_tmp52=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp50)->f3;{struct Cyc_List_List*ts=_tmp51;struct Cyc_List_List*ds=_tmp52;
# 316
for(1;ts != 0;ts=ts->tl){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_tmpA4=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_tmpA4;});}
# 321
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 30: _tmp51=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{void**t=_tmp51;struct Cyc_List_List*ds=_tmp52;
# 325
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_tmpA5=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmpA5;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 34: _tmp51=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp50)->f1).rgn;_tmp52=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp50)->f1).elt_type;_tmp53=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp50)->f1).num_elts;{struct Cyc_Absyn_Exp*rgnopt=_tmp51;void**elt_t=_tmp52;struct Cyc_Absyn_Exp*e1=_tmp53;
# 332
!env.toplevel?0:({int(*_tmpA7)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpA6=({const char*_tmp59="!env.toplevel";_tag_fat(_tmp59,sizeof(char),14U);});_tmpA7(_tmpA6,({const char*_tmp5A="currgn.cyc";_tag_fat(_tmp5A,sizeof(char),11U);}),332U);});
Cyc_CurRgn_rgnsubexpopt(env,rgnopt);
if(elt_t != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,*elt_t);
({void*_tmpA8=Cyc_Tcutil_substitute_nofun(env.inst,*elt_t);*elt_t=_tmpA8;});}
# 338
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}default: _tmp51=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp50)->f1;{struct Cyc_Absyn_Stmt*s=_tmp51;
# 341
Cyc_CurRgn_rgnsubstmt(env,s);goto _LL0;}}_LL0:;}
# 343
if(e->topt != 0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)_check_null(e->topt));
if(!env.toplevel)
({void*_tmpA9=Cyc_Tcutil_substitute_nofun(env.inst,(void*)_check_null(e->topt));e->topt=_tmpA9;});}}
# 360 "currgn.cyc"
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
if(e == 0)return;else{
Cyc_CurRgn_rgnsubexp(env,e);}}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 365
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
# 367
for(1;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubexp(env,(*((struct _tuple13*)ds->hd)).f2);}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 371
static void Cyc_CurRgn_rgnsubpat(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Pat*p){
!env.toplevel?0:({int(*_tmpAB)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpAA=({const char*_tmp5B="!env.toplevel";_tag_fat(_tmp5B,sizeof(char),14U);});_tmpAB(_tmpAA,({const char*_tmp5C="currgn.cyc";_tag_fat(_tmp5C,sizeof(char),11U);}),372U);});{
void*_tmp5D=p->r;void*_stmttmp7=_tmp5D;void*_tmp5E=_stmttmp7;void*_tmp60;void*_tmp5F;switch(*((int*)_tmp5E)){case 0:
 return;case 1: _tmp5F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;_tmp60=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp5F;struct Cyc_Absyn_Pat*p1=_tmp60;
# 376
_tmp5F=vd;_tmp60=p1;goto _LL6;}case 3: _tmp5F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;_tmp60=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp5E)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp5F;struct Cyc_Absyn_Pat*p1=_tmp60;
# 379
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmpAC=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmpAC;});}
# 383
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 2: _tmp5F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;_tmp60=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp5F;struct Cyc_Absyn_Vardecl*vd=_tmp60;
# 387
Cyc_CurRgn_check_tvar(p->loc,tv);
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmpAD=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmpAD;});}
# 392
return;}case 4: _tmp5F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;_tmp60=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp5F;struct Cyc_Absyn_Vardecl*vd=_tmp60;
# 396
if(vd->type != 0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_tmpAE=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_tmpAE;});}
# 400
Cyc_CurRgn_check_tvar(p->loc,tv);
return;}case 6: _tmp5F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;{struct Cyc_Absyn_Pat*p1=_tmp5F;
# 404
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 7: _tmp5F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp5E)->f2;_tmp60=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp5E)->f3;{struct Cyc_List_List*tvs=_tmp5F;struct Cyc_List_List*ds=_tmp60;
# 408
for(1;tvs != 0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(p->loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}
for(1;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubpat(env,(*((struct _tuple14*)ds->hd)).f2);}
# 413
return;}case 8: _tmp5F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp5E)->f3;{struct Cyc_List_List*ps=_tmp5F;
# 416
_tmp5F=ps;goto _LL12;}case 5: _tmp5F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;_LL12: {struct Cyc_List_List*ps=_tmp5F;
_tmp5F=ps;goto _LL14;}case 16: _tmp5F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp5E)->f2;_LL14: {struct Cyc_List_List*ps=_tmp5F;
# 419
for(1;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubpat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 9:
# 423
 goto _LL18;case 10: _LL18:
 goto _LL1A;case 11: _LL1A:
 goto _LL1C;case 12: _LL1C:
 goto _LL1E;case 13: _LL1E:
 goto _LL20;case 14: _LL20:
 goto _LL22;case 15: _LL22:
 return;default: _tmp5F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp5E)->f1;{struct Cyc_Absyn_Exp*e=_tmp5F;
# 432
Cyc_CurRgn_rgnsubexp(env,e);
return;}};}}struct _tuple15{void*f1;void*f2;};
# 437
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad){
Cyc_CurRgn_check_tvars(loc,ad->tvs);
if(ad->impl != 0){
Cyc_CurRgn_check_tvars(loc,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);{
struct Cyc_List_List*_tmp61=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*ps=_tmp61;
for(1;ps != 0;ps=ps->tl){
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f1);
Cyc_CurRgn_rgnsubtype(loc,env,(*((struct _tuple15*)ps->hd)).f2);}{
# 446
struct Cyc_List_List*_tmp62=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp62;
for(1;fs != 0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}}}}}
# 454
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud){
Cyc_CurRgn_check_tvars(loc,tud->tvs);
if(tud->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_List_List*_tmp63=((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs;struct Cyc_List_List*ts=_tmp63;
while(ts != 0){
struct _tuple12*_tmp64=(struct _tuple12*)ts->hd;struct _tuple12*_stmttmp8=_tmp64;struct _tuple12*_tmp65=_stmttmp8;void*_tmp66;_tmp66=(void**)& _tmp65->f2;{void**typ=(void**)_tmp66;
Cyc_CurRgn_rgnsubtype(loc,env,*typ);
ts=ts->tl;}}}}}
# 468
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
static void Cyc_CurRgn_rgnsubdecl(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Decl*d){
void*_tmp67=d->r;void*_stmttmp9=_tmp67;void*_tmp68=_stmttmp9;void*_tmp6A;void*_tmp69;switch(*((int*)_tmp68)){case 0: _tmp69=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp68)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp69;
# 472
struct Cyc_Absyn_Vardecl*_tmp6B=vd;void*_tmp6D;void*_tmp6C;_tmp6C=(void**)& _tmp6B->type;_tmp6D=_tmp6B->initializer;{void**t=(void**)_tmp6C;struct Cyc_Absyn_Exp*initializer=_tmp6D;
Cyc_CurRgn_rgnsubexpopt(env,initializer);
Cyc_CurRgn_rgnsubtype(d->loc,env,*t);
if(!env.toplevel)
({void*_tmpAF=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_tmpAF;});
return;}}case 1: _tmp69=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp68)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp69;
# 485
env.toplevel=0;
({struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct _tuple11*_tmpB0=({struct _tuple11*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar),_tmp6E->f2=(void*)_check_null(Cyc_CurRgn_curr_rgn_typ);_tmp6E;});_tmp6F->hd=_tmpB0;}),_tmp6F->tl=0;_tmp6F;});env.inst=_tmpB1;});
# 488
Cyc_CurRgn_rgnsubstmt(env,fd->body);
# 491
return;}case 8: _tmp69=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp68)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp69;
# 494
env.toplevel?0:({int(*_tmpB3)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB2=({const char*_tmp70="env.toplevel";_tag_fat(_tmp70,sizeof(char),13U);});_tmpB3(_tmpB2,({const char*_tmp71="currgn.cyc";_tag_fat(_tmp71,sizeof(char),11U);}),494U);});
Cyc_CurRgn_check_tvars(d->loc,td->tvs);
if(td->defn != 0)Cyc_CurRgn_rgnsubtype(d->loc,env,(void*)_check_null(td->defn));
return;}case 5: _tmp69=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp68)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp69;
# 500
env.toplevel?0:({int(*_tmpB5)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB4=({const char*_tmp72="env.toplevel";_tag_fat(_tmp72,sizeof(char),13U);});_tmpB5(_tmpB4,({const char*_tmp73="currgn.cyc";_tag_fat(_tmp73,sizeof(char),11U);}),500U);});
Cyc_CurRgn_rgnsubaggrdecl(d->loc,env,ad);
return;}case 6: _tmp69=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp68)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_tmp69;
# 505
env.toplevel?0:({int(*_tmpB7)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB6=({const char*_tmp74="env.toplevel";_tag_fat(_tmp74,sizeof(char),13U);});_tmpB7(_tmpB6,({const char*_tmp75="currgn.cyc";_tag_fat(_tmp75,sizeof(char),11U);}),505U);});
Cyc_CurRgn_rgnsubdtdecl(d->loc,env,tud);
return;}case 11:
# 511
 goto _LLE;case 12: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 13: _LL12:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 15: _LL16:
 goto _LL18;case 16: _LL18:
 return;case 9: _tmp69=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp68)->f1;_tmp6A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp68)->f2;{struct _fat_ptr*v=_tmp69;struct Cyc_List_List*ds2=_tmp6A;
# 519
_tmp69=ds2;goto _LL1C;}case 10: _tmp69=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp68)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp69;
Cyc_CurRgn_rgnsubdecls(env,ds2);return;}case 2: _tmp69=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp68)->f1;_tmp6A=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp68)->f3;{struct Cyc_Absyn_Pat*p=_tmp69;struct Cyc_Absyn_Exp*e=_tmp6A;
# 523
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexp(env,e);
return;}case 3: _tmp69=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp68)->f1;{struct Cyc_List_List*vds=_tmp69;
# 528
!env.toplevel?0:({int(*_tmpB9)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB8=({const char*_tmp76="!env.toplevel";_tag_fat(_tmp76,sizeof(char),14U);});_tmpB9(_tmpB8,({const char*_tmp77="currgn.cyc";_tag_fat(_tmp77,sizeof(char),11U);}),528U);});
for(1;vds != 0;vds=vds->tl){
Cyc_CurRgn_rgnsubtype(d->loc,env,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);
({void*_tmpBA=Cyc_Tcutil_substitute_nofun(env.inst,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);((struct Cyc_Absyn_Vardecl*)vds->hd)->type=_tmpBA;});}
# 533
return;}default:
# 535
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=Cyc_Core_Failure,({struct _fat_ptr _tmpBB=({const char*_tmp78="internal compiler error: region or pool at toplevel";_tag_fat(_tmp78,sizeof(char),52U);});_tmp79->f1=_tmpBB;});_tmp79;}));};}
# 549 "currgn.cyc"
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s){
void*_tmp7A=s->r;void*_stmttmpA=_tmp7A;void*_tmp7B=_stmttmpA;void*_tmp81;void*_tmp80;void*_tmp7F;void*_tmp7E;void*_tmp7D;void*_tmp7C;switch(*((int*)_tmp7B)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 1: _tmp7C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;{struct Cyc_Absyn_Exp*e=_tmp7C;
# 556
Cyc_CurRgn_rgnsubexp(env,e);return;}case 2: _tmp7C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp7C;struct Cyc_Absyn_Stmt*s2=_tmp7D;
Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 3: _tmp7C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp7C;
Cyc_CurRgn_rgnsubexpopt(env,eopt);return;}case 4: _tmp7C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;_tmp7E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7B)->f3;{struct Cyc_Absyn_Exp*e=_tmp7C;struct Cyc_Absyn_Stmt*s1=_tmp7D;struct Cyc_Absyn_Stmt*s2=_tmp7E;
# 561
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 14: _tmp7C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2).f1;{struct Cyc_Absyn_Stmt*s=_tmp7C;struct Cyc_Absyn_Exp*e=_tmp7D;
# 563
_tmp7C=e;_tmp7D=s;goto _LL14;}case 5: _tmp7C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1).f1;_tmp7D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_tmp7C;struct Cyc_Absyn_Stmt*s1=_tmp7D;
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 9: _tmp7C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2).f1;_tmp7E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2).f2;_tmp7F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B)->f3).f1;_tmp80=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B)->f3).f2;_tmp81=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B)->f4;{struct Cyc_Absyn_Exp*e1=_tmp7C;struct Cyc_Absyn_Exp*e2=_tmp7D;struct Cyc_Absyn_Stmt*guard_s=_tmp7E;struct Cyc_Absyn_Exp*e3=_tmp7F;struct Cyc_Absyn_Stmt*cont_s=_tmp80;struct Cyc_Absyn_Stmt*s1=_tmp81;
# 567
Cyc_CurRgn_rgnsubexp(env,e1);
Cyc_CurRgn_rgnsubexp(env,e2);
Cyc_CurRgn_rgnsubexp(env,e3);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}case 13: _tmp7C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;{struct _fat_ptr*l=_tmp7C;struct Cyc_Absyn_Stmt*s1=_tmp7D;
# 573
Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 11: _tmp7C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;{struct Cyc_List_List*es=_tmp7C;
# 576
while(es != 0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;}
# 580
return;}case 10: _tmp7C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;{struct Cyc_Absyn_Exp*e=_tmp7C;struct Cyc_List_List*scs=_tmp7D;
# 583
Cyc_CurRgn_rgnsubexp(env,e);
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_tmp82=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpB=_tmp82;struct Cyc_Absyn_Switch_clause*_tmp83=_stmttmpB;void*_tmp86;void*_tmp85;void*_tmp84;_tmp84=_tmp83->pattern;_tmp85=_tmp83->where_clause;_tmp86=_tmp83->body;{struct Cyc_Absyn_Pat*p=_tmp84;struct Cyc_Absyn_Exp*eopt=_tmp85;struct Cyc_Absyn_Stmt*s1=_tmp86;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 591
return;}case 15: _tmp7C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp7C;struct Cyc_List_List*scs=_tmp7D;
# 594
while(scs != 0){
struct Cyc_Absyn_Switch_clause*_tmp87=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpC=_tmp87;struct Cyc_Absyn_Switch_clause*_tmp88=_stmttmpC;void*_tmp8B;void*_tmp8A;void*_tmp89;_tmp89=_tmp88->pattern;_tmp8A=_tmp88->where_clause;_tmp8B=_tmp88->body;{struct Cyc_Absyn_Pat*p=_tmp89;struct Cyc_Absyn_Exp*eopt=_tmp8A;struct Cyc_Absyn_Stmt*s1=_tmp8B;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 601
return;}default: _tmp7C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7B)->f1;_tmp7D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7B)->f2;{struct Cyc_Absyn_Decl*d=_tmp7C;struct Cyc_Absyn_Stmt*s1=_tmp7D;
# 604
void*_tmp8C=d->r;void*_stmttmpD=_tmp8C;void*_tmp8D=_stmttmpD;void*_tmp90;void*_tmp8F;void*_tmp8E;if(*((int*)_tmp8D)== 4){_tmp8E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp8D)->f1;_tmp8F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp8D)->f2;_tmp90=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp8D)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp8E;struct Cyc_Absyn_Vardecl*vd=_tmp8F;struct Cyc_Absyn_Exp*open_exp_opt=_tmp90;
# 606
Cyc_CurRgn_check_tvar(d->loc,tv);
Cyc_CurRgn_rgnsubexpopt(env,open_exp_opt);
# 611
if(open_exp_opt == 0){
({struct Cyc_List_List*_tmpBD=env.inst;struct Cyc_Absyn_Tvar*_tmpBC=(struct Cyc_Absyn_Tvar*)_check_null(Cyc_CurRgn_curr_tvar);Cyc_CurRgn_subst_inst(_tmpBD,_tmpBC,Cyc_Absyn_var_type(tv));});
Cyc_CurRgn_rgnsubstmt(env,s1);}else{
# 616
Cyc_CurRgn_rgnsubstmt(env,s1);}
return;}}else{
# 620
Cyc_CurRgn_rgnsubdecl(env,d);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;};}};}
# 637 "currgn.cyc"
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
for(1;ds != 0;ds=ds->tl){
Cyc_CurRgn_rgnsubdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}}
# 643
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*tds){
Cyc_CurRgn_curr_rgn_type();{
struct Cyc_CurRgn_rgnsubenv env=({struct Cyc_CurRgn_rgnsubenv _tmp93;_tmp93.inst=0,_tmp93.in_externC=0,_tmp93.toplevel=1;_tmp93;});
Cyc_CurRgn_rgnsubdecls(env,tds);}}
