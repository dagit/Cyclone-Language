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
 struct Cyc_Core_Opt{void*v;};
# 123 "core.h"
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 851 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 864
void*Cyc_Absyn_compress(void*);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 40 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 65
void*Cyc_AssnDef_zero (void);
void*Cyc_AssnDef_one (void);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var(void*);
# 76
void*Cyc_AssnDef_get_term_type(void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 92
int Cyc_AssnDef_assncmp(void*,void*);
# 94
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 108 "assndef.h"
void Cyc_AssnDef_reset_hash_cons_table (void);
# 115
void*Cyc_AssnDef_kill(void*);
# 117
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*,void*,void*);
# 122
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
# 133
int Cyc_AssnDef_simple_prove(void*,void*);
# 135
void*Cyc_AssnDef_kill_mem_term(void*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 71 "flags.h"
extern int Cyc_Flags_allpaths;
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict,void*,void**);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 79
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 114
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Vcgen_Env;
# 108 "vcgen.cyc"
static int Cyc_Vcgen_simple_implies(void**a1,void**a2){
({void*_tmp23B=Cyc_AssnDef_reduce(*a1);*a1=_tmp23B;});
({void*_tmp23C=Cyc_AssnDef_reduce(*a2);*a2=_tmp23C;});
return Cyc_AssnDef_simple_prove(*a1,*a2);}
# 115
static int Cyc_Vcgen_implies(void**a1,void**a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(*a1,*a2);}
# 130
static struct Cyc_Set_Set*Cyc_Vcgen_widen_it(struct Cyc_Set_Set*sopt,void*a){
if(sopt == 0)return 0;{
struct Cyc_Set_Set*_tmp0=sopt;struct Cyc_Set_Set*s=_tmp0;
void*_tmp1=a;enum Cyc_AssnDef_Primreln _tmp4;void*_tmp3;void*_tmp2;switch(*((int*)_tmp1)){case 2: switch((int)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f2){case Cyc_AssnDef_Eq: _tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f1;_tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f3;{void*t1=_tmp2;void*t2=_tmp3;
# 135
s=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;})(s,a);{
# 139
void*_tmp5=Cyc_AssnDef_get_term_type(t1);void*topt1=_tmp5;
void*_tmp6=Cyc_AssnDef_get_term_type(t2);void*topt2=_tmp6;
if(topt1 != 0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2 != 0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*(*_tmp23E)(struct Cyc_Set_Set*,void*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;});struct Cyc_Set_Set*_tmp23D=s;_tmp23E(_tmp23D,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*(*_tmp240)(struct Cyc_Set_Set*,void*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;});struct Cyc_Set_Set*_tmp23F=s;_tmp240(_tmp23F,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*(*_tmp242)(struct Cyc_Set_Set*,void*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;});struct Cyc_Set_Set*_tmp241=s;_tmp242(_tmp241,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*(*_tmp244)(struct Cyc_Set_Set*,void*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;});struct Cyc_Set_Set*_tmp243=s;_tmp244(_tmp243,Cyc_AssnDef_ulte(t2,t1));});}
# 148
return s;}}case Cyc_AssnDef_SLt: _tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f1;_tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f3;{void*t1=_tmp2;void*t2=_tmp3;
# 150
s=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;})(s,a);
s=({struct Cyc_Set_Set*(*_tmp246)(struct Cyc_Set_Set*,void*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;});struct Cyc_Set_Set*_tmp245=s;_tmp246(_tmp245,Cyc_AssnDef_slte(t1,t2));});
return s;}case Cyc_AssnDef_ULt: _tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f1;_tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f3;{void*t1=_tmp2;void*t2=_tmp3;
# 154
s=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;})(s,a);
s=({struct Cyc_Set_Set*(*_tmp248)(struct Cyc_Set_Set*,void*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;});struct Cyc_Set_Set*_tmp247=s;_tmp248(_tmp247,Cyc_AssnDef_ulte(t1,t2));});
return s;}default: _tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f1;_tmp4=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f2;_tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp1)->f3;{void*t1=_tmp2;enum Cyc_AssnDef_Primreln p=_tmp4;void*t2=_tmp3;
return({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,void*))Cyc_Set_insert;})(s,a);}}case 3: _tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp1)->f1;_tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp1)->f2;{void*a1=_tmp2;void*a2=_tmp3;
return({struct Cyc_Set_Set*_tmp249=Cyc_Vcgen_widen_it(s,a1);Cyc_Vcgen_widen_it(_tmp249,a2);});}case 4: _tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp1)->f1;_tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp1)->f2;{void*a1=_tmp2;void*a2=_tmp3;
# 160
struct Cyc_Set_Set*_tmp7=Cyc_Vcgen_widen_it(s,a1);struct Cyc_Set_Set*s1=_tmp7;
struct Cyc_Set_Set*_tmp8=Cyc_Vcgen_widen_it(s,a2);struct Cyc_Set_Set*s2=_tmp8;
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_intersect;})(s1,s2);}case 0:
 return s;case 1:
 return 0;default:
 return({struct Cyc_Set_Set*_tmp24A=s;Cyc_Vcgen_widen_it(_tmp24A,Cyc_AssnDef_reduce(a));});};}}
# 171
static void*Cyc_Vcgen_widen(void*a){
struct Cyc_Set_Set*_tmp9=({struct Cyc_Set_Set*_tmp24B=({(struct Cyc_Set_Set*(*)(int(*)(void*,void*)))Cyc_Set_empty;})(Cyc_AssnDef_assncmp);Cyc_Vcgen_widen_it(_tmp24B,a);});struct Cyc_Set_Set*sopt=_tmp9;
if(sopt == 0)return(void*)& Cyc_AssnDef_false_assn;
return({(void*(*)(void*(*)(void*,void*),struct Cyc_Set_Set*,void*))Cyc_Set_fold;})(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);}
# 178
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*,void*);
# 181
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Vardecl*);
# 183
static void*Cyc_Vcgen_kill_var(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
if(vd->escapes)
a=Cyc_AssnDef_kill(a);else{
# 187
void*_tmpA=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmpA;
a=Cyc_AssnDef_subst(vd,v2,a);}
# 190
return a;}
# 194
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_tmpB=tgt;void*_tmpC;if(_tmpB == 0)
# 199
return Cyc_AssnDef_kill(a);else{if(*((int*)_tmpB)== 2){_tmpC=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpB)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpC;
# 205
if(vd->escapes){
a=Cyc_AssnDef_kill(a);
src=src == 0?0: Cyc_AssnDef_kill_mem_term(src);}else{
# 209
void*_tmpD=Cyc_Vcgen_fresh_nonescvar(env,vd);void*v2=_tmpD;
a=Cyc_AssnDef_subst(vd,v2,a);
src=src == 0?0: Cyc_AssnDef_subst_term(src,vd,v2);}
# 214
return({void*_tmp24C=a;Cyc_AssnDef_and(_tmp24C,Cyc_AssnDef_eq(tgt,src));});}}else{
({void*_tmpE=0U;({int(*_tmp24E)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp24D=({const char*_tmpF="vcgen do_assign";_tag_fat(_tmpF,sizeof(char),16U);});_tmp24E(_tmp24D,_tag_fat(_tmpE,sizeof(void*),0));});});}};}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void**try_assn;void**exp_stmt;void*res_term;void**res_assn;int widen_paths;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 257 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 259
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,void*a){
return(env->shared)->widen_paths?Cyc_Vcgen_widen(a): a;}
# 263
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,void**res_assn){
# 265
struct Cyc_Hashtable_Table*_tmp10=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(tables->succ_tables,fd);struct Cyc_Hashtable_Table*succ_table=_tmp10;
struct Cyc_Hashtable_Table*_tmp11=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;}));struct Cyc_Hashtable_Table*assn_table=_tmp11;
struct Cyc_Vcgen_SharedEnv*_tmp12=({struct Cyc_Vcgen_SharedEnv*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->assn_table=assn_table,_tmp15->succ_table=succ_table,_tmp15->try_assn=0,_tmp15->exp_stmt=0,_tmp15->res_term=0,_tmp15->res_assn=res_assn,_tmp15->widen_paths=!Cyc_Flags_allpaths;_tmp15;});struct Cyc_Vcgen_SharedEnv*senv=_tmp12;
# 274
struct Cyc_Dict_Dict _tmp13=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;})(({(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;}));struct Cyc_Dict_Dict d=_tmp13;
return({struct Cyc_Vcgen_Env*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->shared=senv,_tmp14->state_counter=d,_tmp14->assn_info=assn_info;_tmp14;});}
# 278
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_tmp16=_cycalloc(sizeof(*_tmp16));*_tmp16=*env;_tmp16;});}
# 284
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 288
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _tmp24F=({(struct Cyc_Dict_Dict(*)(int(*)(int,struct Cyc_Absyn_Vardecl*,int,int),int,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_union_two_c;})(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_tmp24F;});}
# 296
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int*))Cyc_Dict_lookup_bool;})(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _tmp250=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert;})(env->state_counter,vd,res + 1);env->state_counter=_tmp250;});
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=3,_tmp17->f1=vd,_tmp17->f2=res,_tmp17->f3=vd->type;_tmp17;});}
# 305
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***_tmp18=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);void***popt=_tmp18;
if(popt != 0){
void**p=*popt;
void*_tmp19=Cyc_AssnDef_or(ain,*p);void*new_precondition=_tmp19;
*p=new_precondition;
return new_precondition;}else{
# 313
return ain;}}
# 317
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_AssnDef_false_assn;
void***popt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);
if(popt != 0)
return*popt;else{
# 323
return& false_pointer;}}
# 328
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp1A=*aprevopt;void**aprev=_tmp1A;
# 333
ain=Cyc_Vcgen_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 336
({void(*_tmp253)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**)=({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp252=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp251=s;_tmp253(_tmp252,_tmp251,({void**_tmp1B=_cycalloc(sizeof(*_tmp1B));*_tmp1B=ain;_tmp1B;}));});}
# 338
return ain;}
# 343
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})((env->shared)->assn_table,s);
if(aprevopt != 0){
void**_tmp1C=*aprevopt;void**aprev=_tmp1C;
ain=({struct Cyc_Vcgen_Env*_tmp254=env;Cyc_Vcgen_may_widen(_tmp254,Cyc_AssnDef_or(ain,*aprev));});
*aprev=ain;}else{
# 350
({void(*_tmp257)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**)=({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp256=(env->shared)->assn_table;struct Cyc_Absyn_Stmt*_tmp255=s;_tmp257(_tmp256,_tmp255,({void**_tmp1D=_cycalloc(sizeof(*_tmp1D));*_tmp1D=ain;_tmp1D;}));});}
# 352
return ain;}
# 357
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
if((env->shared)->try_assn == 0)return;
a=({struct Cyc_Vcgen_Env*_tmp258=env;Cyc_Vcgen_may_widen(_tmp258,Cyc_AssnDef_or(*((void**)_check_null((env->shared)->try_assn)),a));});
*((void**)_check_null((env->shared)->try_assn))=a;}
# 364
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a,void*topt){
struct Cyc_Vcgen_SharedEnv*_tmp1E=env->shared;struct Cyc_Vcgen_SharedEnv*shared=_tmp1E;
if(shared->res_assn == 0)return;
if(shared->res_term != 0 || t != 0){
void*_tmp1F=Cyc_AssnDef_fresh_var(topt);void*v=_tmp1F;
shared->res_term=v;
({void*_tmp25C=({
void*_tmp25B=({void*_tmp259=a;Cyc_AssnDef_and(_tmp259,Cyc_AssnDef_eq(v,t));});Cyc_AssnDef_or(_tmp25B,({void*_tmp25A=*((void**)_check_null(shared->res_assn));Cyc_AssnDef_and(_tmp25A,Cyc_AssnDef_eq(v,shared->res_term));}));});
# 370
*((void**)_check_null(shared->res_assn))=_tmp25C;});}else{
# 373
({void*_tmp25D=Cyc_AssnDef_or(a,*((void**)_check_null(shared->res_assn)));*((void**)_check_null(shared->res_assn))=_tmp25D;});}}
# 378
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*_tmp20=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;}));struct Cyc_Hashtable_Table*assn_info=_tmp20;
return({struct Cyc_Hashtable_Table**_tmp21=_cycalloc(sizeof(*_tmp21));*_tmp21=assn_info;_tmp21;});}struct _tuple13{void*f1;void*f2;};
# 383
struct _tuple13 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct _tuple13**_tmp22=({(struct _tuple13**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt;})(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e);struct _tuple13**result=_tmp22;
if(result == 0)
# 388
return({struct _tuple13 _tmp1F0;_tmp1F0.f1=(void*)& Cyc_AssnDef_true_assn,_tmp1F0.f2=(void*)& Cyc_AssnDef_false_assn;_tmp1F0;});else{
# 390
return*(*result);}}
# 394
static void Cyc_Vcgen_insert_assn_info(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct _tuple13*ctxt_assn){
# 402
({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct _tuple13*))Cyc_Hashtable_insert;})(*((struct Cyc_Hashtable_Table**)_check_null(assn_info)),e,ctxt_assn);
return;}
# 406
static struct Cyc_Absyn_Exp*Cyc_Vcgen_shared_zero_exp (void){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return(struct Cyc_Absyn_Exp*)_check_null(ans);}
# 418
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Stmt*,void*);
# 422
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 424
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 426
static struct _tuple13 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 429
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 436
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);struct _tuple14{struct Cyc_List_List*f1;void*f2;};
# 438
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 440
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 442
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Decl*,void*);
# 444
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_tmp23=s->r;void*_stmttmp0=_tmp23;void*_tmp24=_stmttmp0;void*_tmp27;void*_tmp26;void*_tmp25;switch(*((int*)_tmp24)){case 4: _tmp25=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp24)->f1;{struct Cyc_Absyn_Exp*e=_tmp25;
# 447
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0,({struct _fat_ptr _tmp25E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1F1.f1=_tmp25E;});_tmp1F1;});void*_tmp28[1];_tmp28[0]=& _tmp2A;({struct _fat_ptr _tmp25F=({const char*_tmp29="if (%s) ...";_tag_fat(_tmp29,sizeof(char),12U);});Cyc_aprintf(_tmp25F,_tag_fat(_tmp28,sizeof(void*),1));});});}case 5: _tmp25=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp24)->f1).f1;{struct Cyc_Absyn_Exp*e=_tmp25;
# 449
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0,({struct _fat_ptr _tmp260=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1F2.f1=_tmp260;});_tmp1F2;});void*_tmp2B[1];_tmp2B[0]=& _tmp2D;({struct _fat_ptr _tmp261=({const char*_tmp2C="while (%s) ...";_tag_fat(_tmp2C,sizeof(char),15U);});Cyc_aprintf(_tmp261,_tag_fat(_tmp2B,sizeof(void*),1));});});}case 9: _tmp25=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp24)->f1;_tmp26=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp24)->f2).f1;_tmp27=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp24)->f3).f1;{struct Cyc_Absyn_Exp*e1=_tmp25;struct Cyc_Absyn_Exp*e2=_tmp26;struct Cyc_Absyn_Exp*e3=_tmp27;
# 451
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0,({struct _fat_ptr _tmp262=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp1F5.f1=_tmp262;});_tmp1F5;});struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp1F4;_tmp1F4.tag=0,({
struct _fat_ptr _tmp263=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_tmp1F4.f1=_tmp263;});_tmp1F4;});struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0,({struct _fat_ptr _tmp264=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_tmp1F3.f1=_tmp264;});_tmp1F3;});void*_tmp2E[3];_tmp2E[0]=& _tmp30,_tmp2E[1]=& _tmp31,_tmp2E[2]=& _tmp32;({struct _fat_ptr _tmp265=({const char*_tmp2F="for (%s;%s;%s) ...";_tag_fat(_tmp2F,sizeof(char),19U);});Cyc_aprintf(_tmp265,_tag_fat(_tmp2E,sizeof(void*),3));});});}case 10: _tmp25=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp24)->f1;{struct Cyc_Absyn_Exp*e=_tmp25;
# 454
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0,({struct _fat_ptr _tmp266=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1F6.f1=_tmp266;});_tmp1F6;});void*_tmp33[1];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp267=({const char*_tmp34="switch (%s) ...";_tag_fat(_tmp34,sizeof(char),16U);});Cyc_aprintf(_tmp267,_tag_fat(_tmp33,sizeof(void*),1));});});}case 14: _tmp25=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp24)->f2).f1;{struct Cyc_Absyn_Exp*e=_tmp25;
# 456
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0,({struct _fat_ptr _tmp268=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp1F7.f1=_tmp268;});_tmp1F7;});void*_tmp36[1];_tmp36[0]=& _tmp38;({struct _fat_ptr _tmp269=({const char*_tmp37="do ... while (%s)";_tag_fat(_tmp37,sizeof(char),18U);});Cyc_aprintf(_tmp269,_tag_fat(_tmp36,sizeof(void*),1));});});}case 15: _tmp25=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp24)->f1;{struct Cyc_Absyn_Stmt*s=_tmp25;
# 458
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B=({struct Cyc_String_pa_PrintArg_struct _tmp1F8;_tmp1F8.tag=0,({struct _fat_ptr _tmp26A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_tmp1F8.f1=_tmp26A;});_tmp1F8;});void*_tmp39[1];_tmp39[0]=& _tmp3B;({struct _fat_ptr _tmp26B=({const char*_tmp3A="try %s catch ...";_tag_fat(_tmp3A,sizeof(char),17U);});Cyc_aprintf(_tmp26B,_tag_fat(_tmp39,sizeof(void*),1));});});}case 2: _tmp25=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp24)->f1;{struct Cyc_Absyn_Stmt*s=_tmp25;
return Cyc_Vcgen_stmt2shortstring(s);}default:  {
# 461
struct _fat_ptr _tmp3C=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_tmp3C;
unsigned max=_get_fat_size(s,sizeof(char))< 11U?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _tmp40=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_tmp40),1U,_tmp40);});
{int i=0;for(0;(unsigned)i < max - 1U;++ i){
({char _tmp26C=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_tmp26C;});}}
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F=({struct Cyc_String_pa_PrintArg_struct _tmp1F9;_tmp1F9.tag=0,_tmp1F9.f1=(struct _fat_ptr)((struct _fat_ptr)res);_tmp1F9;});void*_tmp3D[1];_tmp3D[0]=& _tmp3F;({struct _fat_ptr _tmp26D=({const char*_tmp3E="%s...";_tag_fat(_tmp3E,sizeof(char),6U);});Cyc_aprintf(_tmp26D,_tag_fat(_tmp3D,sizeof(void*),1));});});}};}
# 521 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 524
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);{
# 536 "vcgen.cyc"
void*_tmp41=s->r;void*_stmttmp1=_tmp41;void*_tmp42=_stmttmp1;void*_tmp48;void*_tmp47;void*_tmp46;void*_tmp45;void*_tmp44;void*_tmp43;switch(*((int*)_tmp42)){case 0:
 return ain;case 1: _tmp43=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;{struct Cyc_Absyn_Exp*e=_tmp43;
# 539
struct _tuple13 _tmp49=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp2=_tmp49;struct _tuple13 _tmp4A=_stmttmp2;void*_tmp4C;void*_tmp4B;_tmp4B=_tmp4A.f1;_tmp4C=_tmp4A.f2;{void*t=_tmp4B;void*aout=_tmp4C;
# 542
if((env->shared)->exp_stmt != 0)
*((void**)_check_null((env->shared)->exp_stmt))=t;
return aout;}}case 2: _tmp43=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp43;struct Cyc_Absyn_Stmt*s2=_tmp44;
# 546
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp42)->f1 == 0){
# 550
Cyc_Vcgen_update_return(env,0,ain,0);
return(void*)& Cyc_AssnDef_false_assn;}else{_tmp43=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;{struct Cyc_Absyn_Exp*e=_tmp43;
# 553
struct _tuple13 _tmp4D=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(e),ain);struct _tuple13 _stmttmp3=_tmp4D;struct _tuple13 _tmp4E=_stmttmp3;void*_tmp50;void*_tmp4F;_tmp4F=_tmp4E.f1;_tmp50=_tmp4E.f2;{void*t=_tmp4F;void*aout=_tmp50;
Cyc_Vcgen_update_return(env,t,aout,e->topt);
return(void*)& Cyc_AssnDef_false_assn;}}}case 4: _tmp43=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;_tmp45=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp42)->f3;{struct Cyc_Absyn_Exp*e1=_tmp43;struct Cyc_Absyn_Stmt*s1=_tmp44;struct Cyc_Absyn_Stmt*s2=_tmp45;
# 557
struct _tuple13 _tmp51=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp4=_tmp51;struct _tuple13 _tmp52=_stmttmp4;void*_tmp54;void*_tmp53;_tmp53=_tmp52.f1;_tmp54=_tmp52.f2;{void*at=_tmp53;void*af=_tmp54;
struct Cyc_Vcgen_Env*_tmp55=Cyc_Vcgen_copy_env(env);struct Cyc_Vcgen_Env*env2=_tmp55;
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return({struct Cyc_Vcgen_Env*_tmp26E=env;Cyc_Vcgen_may_widen(_tmp26E,Cyc_AssnDef_or(at,af));});}}case 5: _tmp43=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp42)->f1).f1;_tmp44=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp42)->f1).f2;_tmp45=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_Absyn_Exp*e=_tmp43;struct Cyc_Absyn_Stmt*dummy=_tmp44;struct Cyc_Absyn_Stmt*s=_tmp45;
# 569
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_tmp56=e->r;void*_stmttmp5=_tmp56;void*_tmp57=_stmttmp5;void*_tmp59;void*_tmp58;if(*((int*)_tmp57)== 7){_tmp58=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp57)->f1;_tmp59=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp57)->f2;{struct Cyc_Absyn_Exp*eleft=_tmp58;struct Cyc_Absyn_Exp*eright=_tmp59;
# 574
{void*_tmp5A=eleft->r;void*_stmttmp6=_tmp5A;void*_tmp5B=_stmttmp6;void*_tmp5C;if(*((int*)_tmp5B)== 42){_tmp5C=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp5B)->f1;{struct Cyc_Absyn_Exp*eassn=_tmp5C;
# 576
struct _tuple13 _tmp5D=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);struct _tuple13 _stmttmp7=_tmp5D;struct _tuple13 _tmp5E=_stmttmp7;void*_tmp60;void*_tmp5F;_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{void*tm=_tmp5F;void*an=_tmp60;
loop_inv=(Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn)).f1;
e=eright;
ain=an;
goto _LL33;}}}else{
# 582
goto _LL33;}_LL33:;}
# 584
goto _LL2E;}}else{
# 586
goto _LL2E;}_LL2E:;}
# 588
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple13 _tmp61=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple13 _stmttmp8=_tmp61;struct _tuple13 _tmp62=_stmttmp8;void*_tmp64;void*_tmp63;_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;{void*at=_tmp63;void*af=_tmp64;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 595
({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0,({
struct _fat_ptr _tmp26F=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_tmp1FB.f1=_tmp26F;});_tmp1FB;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0,({struct _fat_ptr _tmp270=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp1FA.f1=_tmp270;});_tmp1FA;});void*_tmp65[2];_tmp65[0]=& _tmp67,_tmp65[1]=& _tmp68;({unsigned _tmp272=e->loc;struct _fat_ptr _tmp271=({const char*_tmp66="cannot prove loop invariant %s from %s\n";_tag_fat(_tmp66,sizeof(char),40U);});Cyc_Warn_err(_tmp272,_tmp271,_tag_fat(_tmp65,sizeof(void*),2));});});
return(void*)& Cyc_AssnDef_true_assn;}
# 599
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7:
# 601
 goto _LL12;{
# 604
struct Cyc_Absyn_Stmt*_tmp69=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp69;
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6: _LL12: {
# 611
struct Cyc_Absyn_Stmt*_tmp6A=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})((env->shared)->succ_table,s);struct Cyc_Absyn_Stmt*dest=_tmp6A;
if(dest == 0)
Cyc_Vcgen_update_return(env,0,ain,0);else{
# 615
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9: _tmp43=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp42)->f2).f1;_tmp45=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp42)->f2).f2;_tmp46=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp42)->f3).f1;_tmp47=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp42)->f3).f2;_tmp48=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp42)->f4;{struct Cyc_Absyn_Exp*e1=_tmp43;struct Cyc_Absyn_Exp*e2=_tmp44;struct Cyc_Absyn_Stmt*dummy2=_tmp45;struct Cyc_Absyn_Exp*e3=_tmp46;struct Cyc_Absyn_Stmt*dummy3=_tmp47;struct Cyc_Absyn_Stmt*s=_tmp48;
# 618
struct _tuple13 _tmp6B=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp9=_tmp6B;struct _tuple13 _tmp6C=_stmttmp9;void*_tmp6D;_tmp6D=_tmp6C.f2;{void*a=_tmp6D;
ain=a;{
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_tmp6E=e2->r;void*_stmttmpA=_tmp6E;void*_tmp6F=_stmttmpA;void*_tmp71;void*_tmp70;if(*((int*)_tmp6F)== 7){_tmp70=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F)->f1;_tmp71=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F)->f2;{struct Cyc_Absyn_Exp*eleft=_tmp70;struct Cyc_Absyn_Exp*eright=_tmp71;
# 625
{void*_tmp72=eleft->r;void*_stmttmpB=_tmp72;void*_tmp73=_stmttmpB;void*_tmp74;if(*((int*)_tmp73)== 42){_tmp74=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp73)->f1;{struct Cyc_Absyn_Exp*eassn=_tmp74;
# 627
struct _tuple13 _tmp75=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);struct _tuple13 _stmttmpC=_tmp75;struct _tuple13 _tmp76=_stmttmpC;void*_tmp78;void*_tmp77;_tmp77=_tmp76.f1;_tmp78=_tmp76.f2;{void*tm=_tmp77;void*an=_tmp78;
loop_inv=(Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn)).f1;
e2=eright;
ain=an;
goto _LL46;}}}else{
# 633
goto _LL46;}_LL46:;}
# 635
goto _LL41;}}else{
# 637
goto _LL41;}_LL41:;}
# 639
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple13 _tmp79=Cyc_Vcgen_vcgen_test(env,e2,ain);struct _tuple13 _stmttmpD=_tmp79;struct _tuple13 _tmp7A=_stmttmpD;void*_tmp7C;void*_tmp7B;_tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;{void*at=_tmp7B;void*af=_tmp7C;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 645
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain)).f2;
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 649
({struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmp1FD;_tmp1FD.tag=0,({
struct _fat_ptr _tmp273=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_tmp1FD.f1=_tmp273;});_tmp1FD;});struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0,({struct _fat_ptr _tmp274=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp1FC.f1=_tmp274;});_tmp1FC;});void*_tmp7D[2];_tmp7D[0]=& _tmp7F,_tmp7D[1]=& _tmp80;({unsigned _tmp276=e2->loc;struct _fat_ptr _tmp275=({const char*_tmp7E="cannot prove loop invariant %s from %s\n";_tag_fat(_tmp7E,sizeof(char),40U);});Cyc_Warn_err(_tmp276,_tmp275,_tag_fat(_tmp7D,sizeof(void*),2));});});
return(void*)& Cyc_AssnDef_true_assn;}
# 653
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2))&&
 Cyc_Vcgen_implies(& ain,& loop_inv))
return Cyc_AssnDef_and(loop_inv,af);}}}}}}case 14: _tmp43=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp42)->f2).f1;_tmp45=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp42)->f2).f2;{struct Cyc_Absyn_Stmt*s=_tmp43;struct Cyc_Absyn_Exp*e=_tmp44;struct Cyc_Absyn_Stmt*dummy=_tmp45;
# 660
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_tmp81=e->r;void*_stmttmpE=_tmp81;void*_tmp82=_stmttmpE;void*_tmp84;void*_tmp83;if(*((int*)_tmp82)== 7){_tmp83=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp84=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp82)->f2;{struct Cyc_Absyn_Exp*eleft=_tmp83;struct Cyc_Absyn_Exp*eright=_tmp84;
# 665
{void*_tmp85=eleft->r;void*_stmttmpF=_tmp85;void*_tmp86=_stmttmpF;void*_tmp87;if(*((int*)_tmp86)== 42){_tmp87=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp86)->f1;{struct Cyc_Absyn_Exp*eassn=_tmp87;
# 667
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple13 _tmp88=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);struct _tuple13 _stmttmp10=_tmp88;struct _tuple13 _tmp89=_stmttmp10;void*_tmp8B;void*_tmp8A;_tmp8A=_tmp89.f1;_tmp8B=_tmp89.f2;{void*tm=_tmp8A;void*an=_tmp8B;
loop_inv=(Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn)).f1;
e=eright;
ain=an;
goto _LL56;}}}}else{
# 674
goto _LL56;}_LL56:;}
# 676
goto _LL51;}}else{
# 678
goto _LL51;}_LL51:;}
# 680
while(1){
ain=Cyc_AssnDef_and(ain,loop_inv);
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 685
({struct Cyc_String_pa_PrintArg_struct _tmp8E=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0,({
struct _fat_ptr _tmp277=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_tmp1FF.f1=_tmp277;});_tmp1FF;});struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0,({struct _fat_ptr _tmp278=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_tmp1FE.f1=_tmp278;});_tmp1FE;});void*_tmp8C[2];_tmp8C[0]=& _tmp8E,_tmp8C[1]=& _tmp8F;({unsigned _tmp27A=e->loc;struct _fat_ptr _tmp279=({const char*_tmp8D="cannot prove loop invariant %s from %s\n";_tag_fat(_tmp8D,sizeof(char),40U);});Cyc_Warn_err(_tmp27A,_tmp279,_tag_fat(_tmp8C,sizeof(void*),2));});});
return(void*)& Cyc_AssnDef_true_assn;}{
# 689
struct _tuple13 _tmp90=Cyc_Vcgen_vcgen_test(env,e,ain);struct _tuple13 _stmttmp11=_tmp90;struct _tuple13 _tmp91=_stmttmp11;void*_tmp93;void*_tmp92;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{void*at=_tmp92;void*af=_tmp93;
ain=({struct Cyc_Vcgen_Env*_tmp27C=env;Cyc_Vcgen_may_widen(_tmp27C,({void*_tmp27B=*Cyc_Vcgen_get_assn(env,dummy);Cyc_AssnDef_or(_tmp27B,ain);}));});
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10: _tmp43=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_Absyn_Exp*e=_tmp43;struct Cyc_List_List*scs=_tmp44;
# 700
struct _tuple13 _tmp94=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp12=_tmp94;struct _tuple13 _tmp95=_stmttmp12;void*_tmp97;void*_tmp96;_tmp96=_tmp95.f1;_tmp97=_tmp95.f2;{void*t=_tmp96;void*a=_tmp97;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp42)->f2 == 0)
({void*_tmp98=0U;({int(*_tmp27E)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp27D=({const char*_tmp99="vcgen fallthru";_tag_fat(_tmp99,sizeof(char),15U);});_tmp27E(_tmp27D,_tag_fat(_tmp98,sizeof(void*),0));});});else{_tmp43=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_List_List*es=_tmp43;struct Cyc_Absyn_Switch_clause*dest=_tmp44;
# 705
struct _tuple14 _tmp9A=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp13=_tmp9A;struct _tuple14 _tmp9B=_stmttmp13;void*_tmp9D;void*_tmp9C;_tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;{struct Cyc_List_List*ts=_tmp9C;void*a=_tmp9D;
if(ts != 0){
# 708
struct Cyc_List_List*_tmp9E=({struct Cyc_List_List*(*_tmp27F)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls;});_tmp27F((({(struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split;})((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(dest->pat_vars))->v)).f1);});struct Cyc_List_List*x=_tmp9E;
# 710
for(1;x != 0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*_tmp9F=(struct Cyc_Absyn_Vardecl*)x->hd;struct Cyc_Absyn_Vardecl*vd=_tmp9F;
if(vd != 0)
a=({struct Cyc_Vcgen_Env*_tmp282=env;void*_tmp281=a;void*_tmp280=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=2,_tmpA0->f1=vd;_tmpA0;});Cyc_Vcgen_do_assign(_tmp282,_tmp281,_tmp280,(void*)((struct Cyc_List_List*)_check_null(ts))->hd);});}}
# 716
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12: _tmp43=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_Absyn_Decl*d=_tmp43;struct Cyc_Absyn_Stmt*s1=_tmp44;
# 719
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13: _tmp43=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp43;
# 727
ain=(void*)& Cyc_AssnDef_true_assn;
s=s1;
goto LOOP;}case 8:
 return(void*)& Cyc_AssnDef_false_assn;default: _tmp43=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp42)->f1;_tmp44=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp42)->f2;{struct Cyc_Absyn_Stmt*s=_tmp43;struct Cyc_List_List*scs=_tmp44;
# 733
void**_tmpA1=(env->shared)->try_assn;void**old_try_assn=_tmpA1;
# 735
({void**_tmp283=({void**_tmpA2=_cycalloc(sizeof(*_tmpA2));*_tmpA2=(void*)& Cyc_AssnDef_false_assn;_tmpA2;});(env->shared)->try_assn=_tmp283;});
# 738
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 740
void*_tmpA3=*((void**)_check_null((env->shared)->try_assn));void*new_try_assn=_tmpA3;
(env->shared)->try_assn=old_try_assn;
# 744
return({void*_tmp284=ain;Cyc_AssnDef_or(_tmp284,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}};}}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 748
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 752
void*res=(void*)& Cyc_AssnDef_false_assn;
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmpA4=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmpA4;
void*_tmpA5=ain;void*a=_tmpA5;
{struct Cyc_List_List*_tmpA6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sc->pat_vars))->v;struct Cyc_List_List*pvs=_tmpA6;for(0;pvs != 0;pvs=pvs->tl){
struct _tuple15*_tmpA7=(struct _tuple15*)pvs->hd;struct _tuple15*_stmttmp14=_tmpA7;struct _tuple15*_tmpA8=_stmttmp14;void*_tmpAA;void*_tmpA9;_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmpA9;struct Cyc_Absyn_Exp*eopt=_tmpAA;
if(vdopt != 0)
# 763
a=Cyc_Vcgen_kill_var(env,*vdopt,a);}}}
# 767
if(sc->where_clause != 0)
a=(Cyc_Vcgen_vcgen_test(env,(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause),a)).f1;
res=({void*_tmp285=res;Cyc_AssnDef_or(_tmp285,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 771
return res;}
# 774
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 777
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpAB=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2,_tmpB0->f1=vd;_tmpB0;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmpAB;
a=Cyc_Vcgen_kill_var(env,vd,a);
if(vd->initializer != 0){
struct _tuple13 _tmpAC=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer),a);struct _tuple13 _stmttmp15=_tmpAC;struct _tuple13 _tmpAD=_stmttmp15;void*_tmpAF;void*_tmpAE;_tmpAE=_tmpAD.f1;_tmpAF=_tmpAD.f2;{void*t=_tmpAE;void*a1=_tmpAF;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 783
return a;}
# 786
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_tmpB1=d->r;void*_stmttmp16=_tmpB1;void*_tmpB2=_stmttmp16;void*_tmpB5;void*_tmpB4;void*_tmpB3;switch(*((int*)_tmpB2)){case 0: _tmpB3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB2)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpB3;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2: _tmpB3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB2)->f1;_tmpB4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB2)->f2;_tmpB5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB2)->f3;{struct Cyc_Absyn_Pat*p=_tmpB3;struct Cyc_Core_Opt*vds=_tmpB4;struct Cyc_Absyn_Exp*e=_tmpB5;
# 790
struct _tuple13 _tmpB6=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple13 _stmttmp17=_tmpB6;struct _tuple13 _tmpB7=_stmttmp17;void*_tmpB8;_tmpB8=_tmpB7.f2;{void*a=_tmpB8;
{struct Cyc_List_List*_tmpB9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(vds))->v;struct Cyc_List_List*vds=_tmpB9;for(0;vds != 0;vds=vds->tl){
struct _tuple15 _tmpBA=*((struct _tuple15*)vds->hd);struct _tuple15 _stmttmp18=_tmpBA;struct _tuple15 _tmpBB=_stmttmp18;void*_tmpBD;void*_tmpBC;_tmpBC=_tmpBB.f1;_tmpBD=_tmpBB.f2;{struct Cyc_Absyn_Vardecl**vdopt=_tmpBC;struct Cyc_Absyn_Exp*eopt=_tmpBD;
if(vdopt != 0){
if(eopt == 0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 796
struct _tuple13 _tmpBE=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple13 _stmttmp19=_tmpBE;struct _tuple13 _tmpBF=_stmttmp19;void*_tmpC1;void*_tmpC0;_tmpC0=_tmpBF.f1;_tmpC1=_tmpBF.f2;{void*t=_tmpC0;void*a1=_tmpC1;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpC2=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=2,_tmpC3->f1=*vdopt;_tmpC3;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv=_tmpC2;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 802
return a;}}case 3: _tmpB3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpB2)->f1;{struct Cyc_List_List*vds=_tmpB3;
# 804
for(1;vds != 0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4: _tmpB3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB2)->f2;_tmpB4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB2)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpB3;struct Cyc_Absyn_Exp*eopt=_tmpB4;
# 808
if(eopt != 0){
struct _tuple13 _tmpC4=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);struct _tuple13 _stmttmp1A=_tmpC4;struct _tuple13 _tmpC5=_stmttmp1A;void*_tmpC7;void*_tmpC6;_tmpC6=_tmpC5.f1;_tmpC7=_tmpC5.f2;{void*t=_tmpC6;void*a1=_tmpC7;
a=a1;}}
# 813
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default:
 return a;};}
# 821
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpC8=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*t1=_tmpC8;
void*_tmpC9=Cyc_Absyn_compress((void*)_check_null(e2->topt));void*t2=_tmpC9;
struct _tuple13 _tmpCA=({struct _tuple13 _tmp200;_tmp200.f1=t1,_tmp200.f2=t2;_tmp200;});struct _tuple13 _stmttmp1B=_tmpCA;struct _tuple13 _tmpCB=_stmttmp1B;if(*((int*)_tmpCB.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f1)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f1)->f1)->f1 == Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_tmpCB.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmpCB.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_tmpCB.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCB.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_tmpCB.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmpCB.f1)== 3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_tmpCB.f1)== 3){if(*((int*)_tmpCB.f2)== 3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 837
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmpCC=e->r;void*_stmttmp1C=_tmpCC;void*_tmpCD=_stmttmp1C;enum Cyc_Absyn_Primop _tmpCE;void*_tmpD1;void*_tmpD0;void*_tmpCF;switch(*((int*)_tmpCD)){case 6: _tmpCF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_tmpD0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCD)->f2;_tmpD1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCD)->f3;{struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;struct Cyc_Absyn_Exp*e3=_tmpD1;
# 840
struct _tuple13 _tmpD2=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp1D=_tmpD2;struct _tuple13 _tmpD3=_stmttmp1D;void*_tmpD5;void*_tmpD4;_tmpD4=_tmpD3.f1;_tmpD5=_tmpD3.f2;{void*a1t=_tmpD4;void*a1f=_tmpD5;
struct _tuple13 _tmpD6=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple13 _stmttmp1E=_tmpD6;struct _tuple13 _tmpD7=_stmttmp1E;void*_tmpD9;void*_tmpD8;_tmpD8=_tmpD7.f1;_tmpD9=_tmpD7.f2;{void*a2t=_tmpD8;void*a2f=_tmpD9;
struct _tuple13 _tmpDA=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct _tuple13 _stmttmp1F=_tmpDA;struct _tuple13 _tmpDB=_stmttmp1F;void*_tmpDD;void*_tmpDC;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;{void*a3t=_tmpDC;void*a3f=_tmpDD;
return({struct _tuple13 _tmp201;({void*_tmp287=Cyc_AssnDef_or(a2t,a3t);_tmp201.f1=_tmp287;}),({void*_tmp286=Cyc_AssnDef_or(a2f,a3f);_tmp201.f2=_tmp286;});_tmp201;});}}}}case 7: _tmpCF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_tmpD0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCD)->f2;{struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;
# 845
struct _tuple13 _tmpDE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp20=_tmpDE;struct _tuple13 _tmpDF=_stmttmp20;void*_tmpE1;void*_tmpE0;_tmpE0=_tmpDF.f1;_tmpE1=_tmpDF.f2;{void*a1t=_tmpE0;void*a1f=_tmpE1;
struct _tuple13 _tmpE2=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct _tuple13 _stmttmp21=_tmpE2;struct _tuple13 _tmpE3=_stmttmp21;void*_tmpE5;void*_tmpE4;_tmpE4=_tmpE3.f1;_tmpE5=_tmpE3.f2;{void*a2t=_tmpE4;void*a2f=_tmpE5;
return({struct _tuple13 _tmp202;_tmp202.f1=a2t,({void*_tmp288=Cyc_AssnDef_or(a1f,a2f);_tmp202.f2=_tmp288;});_tmp202;});}}}case 8: _tmpCF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_tmpD0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCD)->f2;{struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;
# 849
struct _tuple13 _tmpE6=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp22=_tmpE6;struct _tuple13 _tmpE7=_stmttmp22;void*_tmpE9;void*_tmpE8;_tmpE8=_tmpE7.f1;_tmpE9=_tmpE7.f2;{void*a1t=_tmpE8;void*a1f=_tmpE9;
struct _tuple13 _tmpEA=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct _tuple13 _stmttmp23=_tmpEA;struct _tuple13 _tmpEB=_stmttmp23;void*_tmpED;void*_tmpEC;_tmpEC=_tmpEB.f1;_tmpED=_tmpEB.f2;{void*a2t=_tmpEC;void*a2f=_tmpED;
return({struct _tuple13 _tmp203;({void*_tmp289=Cyc_AssnDef_or(a1t,a2t);_tmp203.f1=_tmp289;}),_tmp203.f2=a2f;_tmp203;});}}}case 9: _tmpCF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_tmpD0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCD)->f2;{struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;
# 853
struct _tuple13 _tmpEE=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp24=_tmpEE;struct _tuple13 _tmpEF=_stmttmp24;void*_tmpF0;_tmpF0=_tmpEF.f2;{void*ain=_tmpF0;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2 != 0){_tmpCF=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2)->hd;{struct Cyc_Absyn_Exp*e1=_tmpCF;
# 856
struct _tuple13 _tmpF1=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp25=_tmpF1;struct _tuple13 _tmpF2=_stmttmp25;void*_tmpF4;void*_tmpF3;_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;{void*at=_tmpF3;void*af=_tmpF4;
return({struct _tuple13 _tmp204;_tmp204.f1=af,_tmp204.f2=at;_tmp204;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2)->tl)->tl == 0){_tmpCE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_tmpCF=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2)->hd;_tmpD0=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCD)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmpCE;struct Cyc_Absyn_Exp*e1=_tmpCF;struct Cyc_Absyn_Exp*e2=_tmpD0;
# 859
struct _tuple13 _tmpF5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp26=_tmpF5;struct _tuple13 _tmpF6=_stmttmp26;void*_tmpF8;void*_tmpF7;_tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;{void*t1=_tmpF7;void*a1=_tmpF8;
struct _tuple13 _tmpF9=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp27=_tmpF9;struct _tuple13 _tmpFA=_stmttmp27;void*_tmpFC;void*_tmpFB;_tmpFB=_tmpFA.f1;_tmpFC=_tmpFA.f2;{void*t2=_tmpFB;void*a2=_tmpFC;
void*_tmpFD=a2;void*at=_tmpFD;
void*_tmpFE=a2;void*af=_tmpFE;
{enum Cyc_Absyn_Primop _tmpFF=p;switch((int)_tmpFF){case Cyc_Absyn_Eq:
# 868
 at=({void*_tmp28A=at;Cyc_AssnDef_and(_tmp28A,Cyc_AssnDef_eq(t1,t2));});
af=({void*_tmp28B=at;Cyc_AssnDef_and(_tmp28B,Cyc_AssnDef_neq(t1,t2));});
goto _LL30;case Cyc_Absyn_Neq:
# 872
 at=({void*_tmp28C=af;Cyc_AssnDef_and(_tmp28C,Cyc_AssnDef_neq(t1,t2));});
af=({void*_tmp28D=af;Cyc_AssnDef_and(_tmp28D,Cyc_AssnDef_eq(t1,t2));});
goto _LL30;case Cyc_Absyn_Lt:
# 876
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp28E=at;Cyc_AssnDef_and(_tmp28E,Cyc_AssnDef_ult(t1,t2));});af=({void*_tmp28F=af;Cyc_AssnDef_and(_tmp28F,Cyc_AssnDef_ulte(t2,t1));});}else{
# 879
at=({void*_tmp290=at;Cyc_AssnDef_and(_tmp290,Cyc_AssnDef_slt(t1,t2));});af=({void*_tmp291=af;Cyc_AssnDef_and(_tmp291,Cyc_AssnDef_slte(t2,t1));});}
# 881
goto _LL30;case Cyc_Absyn_Lte:
# 883
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp292=at;Cyc_AssnDef_and(_tmp292,Cyc_AssnDef_ulte(t1,t2));});af=({void*_tmp293=af;Cyc_AssnDef_and(_tmp293,Cyc_AssnDef_ult(t2,t1));});}else{
# 886
at=({void*_tmp294=at;Cyc_AssnDef_and(_tmp294,Cyc_AssnDef_slte(t1,t2));});af=({void*_tmp295=af;Cyc_AssnDef_and(_tmp295,Cyc_AssnDef_slt(t2,t1));});}
# 888
goto _LL30;case Cyc_Absyn_Gt:
# 890
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp296=at;Cyc_AssnDef_and(_tmp296,Cyc_AssnDef_ult(t2,t1));});af=({void*_tmp297=af;Cyc_AssnDef_and(_tmp297,Cyc_AssnDef_ulte(t1,t2));});}else{
# 893
at=({void*_tmp298=at;Cyc_AssnDef_and(_tmp298,Cyc_AssnDef_slt(t2,t1));});af=({void*_tmp299=af;Cyc_AssnDef_and(_tmp299,Cyc_AssnDef_slte(t1,t2));});}
# 895
goto _LL30;case Cyc_Absyn_Gte:
# 897
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_tmp29A=at;Cyc_AssnDef_and(_tmp29A,Cyc_AssnDef_ulte(t2,t1));});af=({void*_tmp29B=af;Cyc_AssnDef_and(_tmp29B,Cyc_AssnDef_ult(t1,t2));});}else{
# 900
at=({void*_tmp29C=at;Cyc_AssnDef_and(_tmp29C,Cyc_AssnDef_slte(t2,t1));});af=({void*_tmp29D=af;Cyc_AssnDef_and(_tmp29D,Cyc_AssnDef_slt(t1,t2));});}
# 902
goto _LL30;default:
 goto _LL30;}_LL30:;}
# 905
return({struct _tuple13 _tmp205;_tmp205.f1=at,_tmp205.f2=af;_tmp205;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: {
# 907
struct _tuple13 _tmp100=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp28=_tmp100;struct _tuple13 _tmp101=_stmttmp28;void*_tmp103;void*_tmp102;_tmp102=_tmp101.f1;_tmp103=_tmp101.f2;{void*t=_tmp102;void*aout=_tmp103;
return({struct _tuple13 _tmp206;({void*_tmp2A3=({void*_tmp2A2=aout;Cyc_AssnDef_and(_tmp2A2,({void*_tmp2A1=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_tmp2A1,t);}));});_tmp206.f1=_tmp2A3;}),({void*_tmp2A0=({void*_tmp29F=aout;Cyc_AssnDef_and(_tmp29F,({void*_tmp29E=t;Cyc_AssnDef_eq(_tmp29E,Cyc_AssnDef_zero());}));});_tmp206.f2=_tmp2A0;});_tmp206;});}}};}
# 912
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(1;es != 0;es=es->tl){
a=(Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a)).f2;}
return a;}struct _tuple16{struct Cyc_List_List**f1;void*f2;};
# 918
static struct _tuple16 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(1;es != 0;es=es->tl){
struct _tuple13 _tmp104=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);struct _tuple13 _stmttmp29=_tmp104;struct _tuple13 _tmp105=_stmttmp29;void*_tmp107;void*_tmp106;_tmp106=_tmp105.f1;_tmp107=_tmp105.f2;{void*t=_tmp106;void*aout=_tmp107;
a=aout;
if(valid && t != 0)
res=({struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->hd=t,_tmp108->tl=res;_tmp108;});else{
# 927
valid=0;}}}
# 930
return({struct _tuple16 _tmp207;valid?({struct Cyc_List_List**_tmp2A5=({struct Cyc_List_List**_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_List_List*_tmp2A4=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res);*_tmp109=_tmp2A4;});_tmp109;});_tmp207.f1=_tmp2A5;}):(_tmp207.f1=0),_tmp207.f2=a;_tmp207;});}
# 933
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple13 _tmp10A=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp2A=_tmp10A;struct _tuple13 _tmp10B=_stmttmp2A;void*_tmp10D;void*_tmp10C;_tmp10C=_tmp10B.f1;_tmp10D=_tmp10B.f2;{void*topt=_tmp10C;void*a=_tmp10D;
return({struct _tuple13 _tmp208;topt == 0?({void*_tmp2A6=Cyc_AssnDef_fresh_var(e->topt);_tmp208.f1=_tmp2A6;}):(_tmp208.f1=topt),_tmp208.f2=a;_tmp208;});}}
# 938
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(1;es != 0;es=es->tl){
struct _tuple13 _tmp10E=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);struct _tuple13 _stmttmp2B=_tmp10E;struct _tuple13 _tmp10F=_stmttmp2B;void*_tmp111;void*_tmp110;_tmp110=_tmp10F.f1;_tmp111=_tmp10F.f2;{void*t=_tmp110;void*a=_tmp111;
ain=a;
res=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->hd=t,_tmp112->tl=res;_tmp112;});}}
# 945
return({struct _tuple14 _tmp209;({struct Cyc_List_List*_tmp2A7=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res);_tmp209.f1=_tmp2A7;}),_tmp209.f2=ain;_tmp209;});}
# 948
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_tmp113=b;void*_tmp114;switch(*((int*)_tmp113)){case 1: _tmp114=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp113)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp114;
return vd;}case 2: _tmp114=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp113)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp114;
return fd->fn_vardecl;}case 0:
({void*_tmp115=0U;({int(*_tmp2A9)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp2A8=({const char*_tmp116="vcgen bind2vardecl";_tag_fat(_tmp116,sizeof(char),19U);});_tmp2A9(_tmp2A8,_tag_fat(_tmp115,sizeof(void*),0));});});case 3: _tmp114=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp113)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp114;
return vd;}case 4: _tmp114=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp113)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp114;
return vd;}default: _tmp114=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp113)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp114;
return vd;}};}
# 959
static void*Cyc_Vcgen_varopt(void*b){
struct Cyc_Absyn_Vardecl*_tmp117=Cyc_Vcgen_bind2vardecl(b);struct Cyc_Absyn_Vardecl*vdopt=_tmp117;
if(vdopt == 0)return 0;
return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=2,_tmp118->f1=vdopt;_tmp118;});}
# 965
static struct _tuple13 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_tmp119=e->r;void*_stmttmp2C=_tmp119;void*_tmp11A=_stmttmp2C;void*_tmp11B;if(*((int*)_tmp11A)== 1){_tmp11B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11A)->f1;{void*b=_tmp11B;
return({struct _tuple13 _tmp20A;({void*_tmp2AA=Cyc_Vcgen_varopt(b);_tmp20A.f1=_tmp2AA;}),_tmp20A.f2=ain;_tmp20A;});}}else{
return({struct _tuple13 _tmp20B;_tmp20B.f1=0,({void*_tmp2AB=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp20B.f2=_tmp2AB;});_tmp20B;});};}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Vardecl*f4;};
# 974
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple17 _tmp20C;_tmp20C.f1=fi->args,_tmp20C.f2=fi->requires_clause,_tmp20C.f3=fi->ensures_clause,_tmp20C.f4=fi->return_value;_tmp20C;});}
# 978
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt != 0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_tmp11C=t;void*_tmp11D;if(*((int*)_tmp11C)== 5){_tmp11D=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11C)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_tmp11D;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}
# 986
return({struct _tuple17 _tmp20D;_tmp20D.f1=0,_tmp20D.f2=0,_tmp20D.f3=0,_tmp20D.f4=0;_tmp20D;});}
# 989
static void Cyc_Vcgen_check_requires(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*args,void*a,struct Cyc_Absyn_Exp*reqopt){
# 992
if(reqopt != 0){
struct Cyc_Absyn_Exp*req=reqopt;
# 996
for(1;args != 0 && inputs != 0;(args=args->tl,inputs=inputs->tl)){
struct _tuple9*_tmp11E=(struct _tuple9*)args->hd;struct _tuple9*_stmttmp2D=_tmp11E;struct _tuple9*_tmp11F=_stmttmp2D;void*_tmp122;struct Cyc_Absyn_Tqual _tmp121;void*_tmp120;_tmp120=_tmp11F->f1;_tmp121=_tmp11F->f2;_tmp122=_tmp11F->f3;{struct _fat_ptr*vopt=_tmp120;struct Cyc_Absyn_Tqual tq=_tmp121;void*t=_tmp122;
if(vopt != 0){
# 1000
struct _tuple1*qv=({struct _tuple1*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->f1=Cyc_Absyn_Loc_n,_tmp126->f2=vopt;_tmp126;});
struct Cyc_Absyn_Vardecl*_tmp123=({struct Cyc_Absyn_Vardecl*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->sc=2U,_tmp125->name=qv,_tmp125->varloc=0U,_tmp125->tq=tq,_tmp125->type=t,_tmp125->initializer=0,_tmp125->rgn=0,_tmp125->attributes=0,_tmp125->escapes=0,_tmp125->is_proto=0;_tmp125;});struct Cyc_Absyn_Vardecl*vd=_tmp123;
# 1006
a=({void*_tmp2AD=a;Cyc_AssnDef_and(_tmp2AD,({void*_tmp2AC=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=2,_tmp124->f1=vd;_tmp124;});Cyc_AssnDef_eq(_tmp2AC,(void*)inputs->hd);}));});}}}{
# 1010
struct _tuple13 _tmp127=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp2E=_tmp127;struct _tuple13 _tmp128=_stmttmp2E;void*_tmp129;_tmp129=_tmp128.f1;{void*at=_tmp129;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp12C=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0,({
struct _fat_ptr _tmp2AE=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp20F.f1=_tmp2AE;});_tmp20F;});struct Cyc_String_pa_PrintArg_struct _tmp12D=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0,({struct _fat_ptr _tmp2AF=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a)));_tmp20E.f1=_tmp2AF;});_tmp20E;});void*_tmp12A[2];_tmp12A[0]=& _tmp12C,_tmp12A[1]=& _tmp12D;({unsigned _tmp2B1=req->loc;struct _fat_ptr _tmp2B0=({const char*_tmp12B="cannot prove @requires %s from %s";_tag_fat(_tmp12B,sizeof(char),34U);});Cyc_Warn_err(_tmp2B1,_tmp2B0,_tag_fat(_tmp12A,sizeof(void*),2));});});}}}}
# 1017
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}
# 1021
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){
void*t=e->topt;
if(t == 0)return 0;{
void*_tmp12E=t;enum Cyc_Absyn_Size_of _tmp12F;if(_tmp12E != 0){if(*((int*)_tmp12E)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->f1 == Cyc_Absyn_Unsigned){_tmp12F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->f2;{enum Cyc_Absyn_Size_of sz=_tmp12F;
# 1027
if((int)sz == 3 ||(int)sz == 2)
return 1;else{
return 0;}}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1034
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e)){
# 1037
{void*_tmp130=e->r;void*_stmttmp2F=_tmp130;void*_tmp131=_stmttmp2F;struct _fat_ptr _tmp132;if(*((int*)_tmp131)== 2){_tmp132=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp131)->f1;{struct _fat_ptr s=_tmp132;
# 1039
if(({struct _fat_ptr _tmp2B2=(struct _fat_ptr)s;Cyc_strcmp(_tmp2B2,({const char*_tmp133="print_assn";_tag_fat(_tmp133,sizeof(char),11U);}));})== 0){
struct _fat_ptr _tmp134=Cyc_Position_string_of_segment(e->loc);struct _fat_ptr seg_str=_tmp134;
({struct Cyc_String_pa_PrintArg_struct _tmp137=({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0,_tmp211.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_tmp211;});struct Cyc_String_pa_PrintArg_struct _tmp138=({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0,({struct _fat_ptr _tmp2B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp210.f1=_tmp2B3;});_tmp210;});void*_tmp135[2];_tmp135[0]=& _tmp137,_tmp135[1]=& _tmp138;({struct Cyc___cycFILE*_tmp2B5=Cyc_stderr;struct _fat_ptr _tmp2B4=({const char*_tmp136="%s: current assn is %s\n";_tag_fat(_tmp136,sizeof(char),24U);});Cyc_fprintf(_tmp2B5,_tmp2B4,_tag_fat(_tmp135,sizeof(void*),2));});});}
# 1043
if(({struct _fat_ptr _tmp2B6=(struct _fat_ptr)s;Cyc_strcmp(_tmp2B6,({const char*_tmp139="all_paths";_tag_fat(_tmp139,sizeof(char),10U);}));})== 0)
(env->shared)->widen_paths=0;
# 1046
if(({struct _fat_ptr _tmp2B7=(struct _fat_ptr)s;Cyc_strcmp(_tmp2B7,({const char*_tmp13A="one_path";_tag_fat(_tmp13A,sizeof(char),9U);}));})== 0)
(env->shared)->widen_paths=1;
# 1049
return({struct _tuple13 _tmp212;({void*_tmp2B8=Cyc_AssnDef_zero();_tmp212.f1=_tmp2B8;}),_tmp212.f2=ain;_tmp212;});}}else{
goto _LL0;}_LL0:;}{
# 1052
struct _tuple12 _tmp13B=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp30=_tmp13B;struct _tuple12 _tmp13C=_stmttmp30;int _tmp13E;unsigned _tmp13D;_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;{unsigned cn=_tmp13D;int known=_tmp13E;
if(known)return({struct _tuple13 _tmp213;({void*_tmp2B9=(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->tag=0,_tmp13F->f1=cn;_tmp13F;});_tmp213.f1=_tmp2B9;}),_tmp213.f2=ain;_tmp213;});}}}{
# 1055
void*_tmp140=e->r;void*_stmttmp31=_tmp140;void*_tmp141=_stmttmp31;struct _fat_ptr _tmp143;struct Cyc_Absyn_MallocInfo _tmp145;enum Cyc_Absyn_Coercion _tmp147;enum Cyc_Absyn_Incrementor _tmp148;void*_tmp146;void*_tmp144;enum Cyc_Absyn_Primop _tmp149;void*_tmp142;union Cyc_Absyn_Cnst _tmp14A;switch(*((int*)_tmp141)){case 0: _tmp14A=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{union Cyc_Absyn_Cnst c=_tmp14A;
# 1058
return({struct _tuple13 _tmp214;({void*_tmp2BA=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->tag=1,_tmp14B->f1=e;_tmp14B;});_tmp214.f1=_tmp2BA;}),_tmp214.f2=ain;_tmp214;});}case 1: _tmp142=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{void*b=_tmp142;
return({struct _tuple13 _tmp215;({void*_tmp2BB=Cyc_Vcgen_varopt(b);_tmp215.f1=_tmp2BB;}),_tmp215.f2=ain;_tmp215;});}case 3: _tmp149=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp142=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{enum Cyc_Absyn_Primop p=_tmp149;struct Cyc_List_List*es=_tmp142;
# 1061
struct _tuple14 _tmp14C=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp32=_tmp14C;struct _tuple14 _tmp14D=_stmttmp32;void*_tmp14F;void*_tmp14E;_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;{struct Cyc_List_List*ts=_tmp14E;void*aout=_tmp14F;
void*t=0;
if(ts != 0)t=Cyc_AssnDef_primop(p,ts);
# 1066
if((int)p == 4 && Cyc_Vcgen_is_unsigned_int_exp(e)){
# 1068
struct Cyc_List_List*_tmp150=ts;void*_tmp152;void*_tmp151;if(_tmp150 != 0){if(((struct Cyc_List_List*)_tmp150)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp150)->tl)->tl == 0){_tmp151=(void*)_tmp150->hd;_tmp152=(void*)(_tmp150->tl)->hd;{void*t1=_tmp151;void*t2=_tmp152;
# 1070
aout=({void*_tmp2BC=aout;Cyc_AssnDef_and(_tmp2BC,Cyc_AssnDef_ult(t,t2));});
goto _LL62;}}else{goto _LL65;}}else{goto _LL65;}}else{_LL65:
 goto _LL62;}_LL62:;}
# 1075
return({struct _tuple13 _tmp216;_tmp216.f1=t,_tmp216.f2=aout;_tmp216;});}}case 4: _tmp142=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_tmp146=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp141)->f3;{struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Core_Opt*popt=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1077
struct _tuple13 _tmp153=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple13 _stmttmp33=_tmp153;struct _tuple13 _tmp154=_stmttmp33;void*_tmp156;void*_tmp155;_tmp155=_tmp154.f1;_tmp156=_tmp154.f2;{void*lt=_tmp155;void*a1=_tmp156;
struct _tuple13 _tmp157=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp34=_tmp157;struct _tuple13 _tmp158=_stmttmp34;void*_tmp15A;void*_tmp159;_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;{void*t2=_tmp159;void*a2=_tmp15A;
if(popt != 0){
void*_tmp15B=Cyc_Vcgen_deref_lterm(env,lt,a2);void*t1=_tmp15B;
if(t1 == 0 || t2 == 0)
t2=0;else{
# 1084
t2=({enum Cyc_Absyn_Primop _tmp2BE=(enum Cyc_Absyn_Primop)popt->v;Cyc_AssnDef_primop(_tmp2BE,({void*_tmp15C[2];_tmp15C[0]=t1,_tmp15C[1]=t2;({struct Cyc_List_List*(*_tmp2BD)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2BD(_tag_fat(_tmp15C,sizeof(void*),2));});}));});}}{
# 1086
void*_tmp15D=Cyc_Vcgen_do_assign(env,a2,lt,t2);void*a=_tmp15D;
return({struct _tuple13 _tmp217;_tmp217.f1=lt,_tmp217.f2=a;_tmp217;});}}}}case 5: _tmp142=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp148=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*e=_tmp142;enum Cyc_Absyn_Incrementor i=_tmp148;
# 1089
struct _tuple13 _tmp15E=Cyc_Vcgen_vcgen_lexp(env,e,ain);struct _tuple13 _stmttmp35=_tmp15E;struct _tuple13 _tmp15F=_stmttmp35;void*_tmp161;void*_tmp160;_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;{void*lt=_tmp160;void*a1=_tmp161;
void*_tmp162=Cyc_Vcgen_deref_lterm(env,lt,a1);void*t1=_tmp162;
void*res=0;
void*assn=0;
{enum Cyc_Absyn_Incrementor _tmp163=i;switch((int)_tmp163){case Cyc_Absyn_PreInc:
# 1095
 if(t1 != 0)assn=Cyc_AssnDef_primop(0U,({void*_tmp164[2];_tmp164[0]=t1,({void*_tmp2BF=Cyc_AssnDef_one();_tmp164[1]=_tmp2BF;});({struct Cyc_List_List*(*_tmp2C0)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2C0(_tag_fat(_tmp164,sizeof(void*),2));});}));
res=assn;
goto _LL70;case Cyc_Absyn_PostInc:
# 1099
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(0U,({void*_tmp165[2];_tmp165[0]=t1,({void*_tmp2C1=Cyc_AssnDef_one();_tmp165[1]=_tmp2C1;});({struct Cyc_List_List*(*_tmp2C2)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2C2(_tag_fat(_tmp165,sizeof(void*),2));});}));
goto _LL70;case Cyc_Absyn_PreDec:
# 1103
 if(t1 != 0)assn=Cyc_AssnDef_primop(2U,({void*_tmp166[2];_tmp166[0]=t1,({void*_tmp2C3=Cyc_AssnDef_one();_tmp166[1]=_tmp2C3;});({struct Cyc_List_List*(*_tmp2C4)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2C4(_tag_fat(_tmp166,sizeof(void*),2));});}));
res=assn;
goto _LL70;case Cyc_Absyn_PostDec:
# 1107
 res=t1;
if(t1 != 0)assn=Cyc_AssnDef_primop(2U,({void*_tmp167[2];_tmp167[0]=t1,({void*_tmp2C5=Cyc_AssnDef_one();_tmp167[1]=_tmp2C5;});({struct Cyc_List_List*(*_tmp2C6)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2C6(_tag_fat(_tmp167,sizeof(void*),2));});}));
goto _LL70;default:
({void*_tmp168=0U;({int(*_tmp2C8)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp2C7=({const char*_tmp169="vcgen vcgen_rexp increment_e";_tag_fat(_tmp169,sizeof(char),29U);});_tmp2C8(_tmp2C7,_tag_fat(_tmp168,sizeof(void*),0));});});}_LL70:;}{
# 1112
void*_tmp16A=Cyc_Vcgen_do_assign(env,a1,lt,assn);void*a=_tmp16A;
return({struct _tuple13 _tmp218;_tmp218.f1=res,_tmp218.f2=a;_tmp218;});}}}case 6: _tmp142=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_tmp146=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp141)->f3;{struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Absyn_Exp*e2=_tmp144;struct Cyc_Absyn_Exp*e3=_tmp146;
# 1115
struct _tuple13 _tmp16B=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp36=_tmp16B;struct _tuple13 _tmp16C=_stmttmp36;void*_tmp16E;void*_tmp16D;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{void*at=_tmp16D;void*af=_tmp16E;
struct _tuple13 _tmp16F=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp37=_tmp16F;struct _tuple13 _tmp170=_stmttmp37;void*_tmp172;void*_tmp171;_tmp171=_tmp170.f1;_tmp172=_tmp170.f2;{void*t1=_tmp171;void*at=_tmp172;
struct _tuple13 _tmp173=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);struct _tuple13 _stmttmp38=_tmp173;struct _tuple13 _tmp174=_stmttmp38;void*_tmp176;void*_tmp175;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{void*t2=_tmp175;void*af=_tmp176;
if(t1 != 0 && t2 != 0){
void*_tmp177=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp177;
at=({void*_tmp2C9=at;Cyc_AssnDef_and(_tmp2C9,Cyc_AssnDef_eq(v,t1));});
af=({void*_tmp2CA=af;Cyc_AssnDef_and(_tmp2CA,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple13 _tmp219;_tmp219.f1=v,({void*_tmp2CC=({struct Cyc_Vcgen_Env*_tmp2CB=env;Cyc_Vcgen_may_widen(_tmp2CB,Cyc_AssnDef_or(at,af));});_tmp219.f2=_tmp2CC;});_tmp219;});}else{
# 1124
return({struct _tuple13 _tmp21A;_tmp21A.f1=0,({void*_tmp2CE=({struct Cyc_Vcgen_Env*_tmp2CD=env;Cyc_Vcgen_may_widen(_tmp2CD,Cyc_AssnDef_or(at,af));});_tmp21A.f2=_tmp2CE;});_tmp21A;});}}}}}case 7: _tmp142=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Absyn_Exp*e2=_tmp144;
# 1126
struct _tuple13 _tmp178=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp39=_tmp178;struct _tuple13 _tmp179=_stmttmp39;void*_tmp17B;void*_tmp17A;_tmp17A=_tmp179.f1;_tmp17B=_tmp179.f2;{void*at=_tmp17A;void*af=_tmp17B;
struct _tuple13 _tmp17C=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp3A=_tmp17C;struct _tuple13 _tmp17D=_stmttmp3A;void*_tmp17F;void*_tmp17E;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;{void*t=_tmp17E;void*at=_tmp17F;
void*_tmp180=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp180;
at=({void*_tmp2CF=at;Cyc_AssnDef_and(_tmp2CF,Cyc_AssnDef_eq(v,t));});
af=({void*_tmp2D1=af;Cyc_AssnDef_and(_tmp2D1,({void*_tmp2D0=v;Cyc_AssnDef_eq(_tmp2D0,Cyc_AssnDef_zero());}));});
return({struct _tuple13 _tmp21B;_tmp21B.f1=v,({void*_tmp2D3=({struct Cyc_Vcgen_Env*_tmp2D2=env;Cyc_Vcgen_may_widen(_tmp2D2,Cyc_AssnDef_or(at,af));});_tmp21B.f2=_tmp2D3;});_tmp21B;});}}}case 8: _tmp142=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Absyn_Exp*e2=_tmp144;
# 1133
struct _tuple13 _tmp181=Cyc_Vcgen_vcgen_test(env,e1,ain);struct _tuple13 _stmttmp3B=_tmp181;struct _tuple13 _tmp182=_stmttmp3B;void*_tmp184;void*_tmp183;_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;{void*at=_tmp183;void*af=_tmp184;
struct _tuple13 _tmp185=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);struct _tuple13 _stmttmp3C=_tmp185;struct _tuple13 _tmp186=_stmttmp3C;void*_tmp188;void*_tmp187;_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;{void*t=_tmp187;void*af=_tmp188;
void*_tmp189=Cyc_AssnDef_fresh_var(e->topt);void*v=_tmp189;
at=({void*_tmp2D5=at;Cyc_AssnDef_and(_tmp2D5,({void*_tmp2D4=v;Cyc_AssnDef_eq(_tmp2D4,Cyc_AssnDef_one());}));});
af=({void*_tmp2D6=af;Cyc_AssnDef_and(_tmp2D6,Cyc_AssnDef_eq(v,t));});
return({struct _tuple13 _tmp21C;_tmp21C.f1=v,({void*_tmp2D8=({struct Cyc_Vcgen_Env*_tmp2D7=env;Cyc_Vcgen_may_widen(_tmp2D7,Cyc_AssnDef_or(at,af));});_tmp21C.f2=_tmp2D8;});_tmp21C;});}}}case 9: _tmp142=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Absyn_Exp*e2=_tmp144;
# 1140
struct _tuple13 _tmp18A=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp3D=_tmp18A;struct _tuple13 _tmp18B=_stmttmp3D;void*_tmp18C;_tmp18C=_tmp18B.f2;{void*a=_tmp18C;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10: _tmp142=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*e=_tmp142;struct Cyc_List_List*es=_tmp144;
# 1143
struct _tuple14 _tmp18D=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);struct _tuple14 _stmttmp3E=_tmp18D;struct _tuple14 _tmp18E=_stmttmp3E;void*_tmp190;void*_tmp18F;_tmp18F=_tmp18E.f1;_tmp190=_tmp18E.f2;{struct Cyc_List_List*ts=_tmp18F;void*a=_tmp190;
struct _tuple13 _tmp191=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);struct _tuple13 _stmttmp3F=_tmp191;struct _tuple13 _tmp192=_stmttmp3F;void*_tmp193;_tmp193=_tmp192.f2;{void*a2=_tmp193;
# 1146
struct _tuple17 _tmp194=Cyc_Vcgen_get_requires_and_ensures(e->topt);struct _tuple17 _stmttmp40=_tmp194;struct _tuple17 _tmp195=_stmttmp40;void*_tmp199;void*_tmp198;void*_tmp197;void*_tmp196;_tmp196=_tmp195.f1;_tmp197=_tmp195.f2;_tmp198=_tmp195.f3;_tmp199=_tmp195.f4;{struct Cyc_List_List*args=_tmp196;struct Cyc_Absyn_Exp*reqopt=_tmp197;struct Cyc_Absyn_Exp*ensopt=_tmp198;struct Cyc_Absyn_Vardecl*ret_value=_tmp199;
Cyc_Vcgen_check_requires(env,ts,args,a2,reqopt);
# 1150
a2=Cyc_AssnDef_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 1153
if(Cyc_Atts_is_noreturn_fn_type((void*)_check_null(e->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt == 0)
return({struct _tuple13 _tmp21D;_tmp21D.f1=0,_tmp21D.f2=a2;_tmp21D;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 1158
struct _tuple13 _tmp19A=Cyc_Vcgen_vcgen_test(env,ensures,a2);struct _tuple13 _stmttmp41=_tmp19A;struct _tuple13 _tmp19B=_stmttmp41;void*_tmp19C;_tmp19C=_tmp19B.f1;{void*at=_tmp19C;
if(ret_value != 0){
void*_tmp19D=Cyc_AssnDef_fresh_var(ret_value->type);void*v=_tmp19D;
# 1162
at=Cyc_AssnDef_subst(ret_value,v,at);
return({struct _tuple13 _tmp21E;_tmp21E.f1=v,_tmp21E.f2=at;_tmp21E;});}
# 1165
return({struct _tuple13 _tmp21F;_tmp21F.f1=0,_tmp21F.f2=at;_tmp21F;});}}}}}}case 11: _tmp142=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
# 1167
struct _tuple13 _tmp19E=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);struct _tuple13 _stmttmp42=_tmp19E;struct _tuple13 _tmp19F=_stmttmp42;void*_tmp1A0;_tmp1A0=_tmp19F.f2;{void*a=_tmp1A0;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple13 _tmp220;_tmp220.f1=0,_tmp220.f2=(void*)& Cyc_AssnDef_false_assn;_tmp220;});}}case 12: _tmp142=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13: _tmp142=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14: _tmp142=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_tmp147=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp141)->f4;{void*tp=_tmp142;struct Cyc_Absyn_Exp*e1=_tmp144;enum Cyc_Absyn_Coercion c=_tmp147;
# 1175
struct _tuple13 _tmp1A1=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);struct _tuple13 _stmttmp43=_tmp1A1;struct _tuple13 _tmp1A2=_stmttmp43;void*_tmp1A4;void*_tmp1A3;_tmp1A3=_tmp1A2.f1;_tmp1A4=_tmp1A2.f2;{void*topt=_tmp1A3;void*a=_tmp1A4;
# 1183
{struct _tuple13 _tmp1A5=({struct _tuple13 _tmp224;({void*_tmp2DA=Cyc_Absyn_compress(tp);_tmp224.f1=_tmp2DA;}),({void*_tmp2D9=Cyc_Absyn_compress((void*)_check_null(e->topt));_tmp224.f2=_tmp2D9;});_tmp224;});struct _tuple13 _stmttmp44=_tmp1A5;struct _tuple13 _tmp1A6=_stmttmp44;enum Cyc_Absyn_Size_of _tmp1AA;enum Cyc_Absyn_Sign _tmp1A9;enum Cyc_Absyn_Size_of _tmp1A8;enum Cyc_Absyn_Sign _tmp1A7;if(*((int*)_tmp1A6.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f1)->f1)== 1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f1)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_tmp1A6.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f2)->f1)){case 4:
# 1187
 return({struct _tuple13 _tmp221;_tmp221.f1=topt,_tmp221.f2=a;_tmp221;});case 1: goto _LLAA;default: goto _LLAC;}else{goto _LLAC;}case Cyc_Absyn_Long_sz: if(*((int*)_tmp1A6.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f2)->f1)){case 4:
# 1189
 return({struct _tuple13 _tmp222;_tmp222.f1=topt,_tmp222.f2=a;_tmp222;});case 1: goto _LLAA;default: goto _LLAC;}else{goto _LLAC;}default: if(*((int*)_tmp1A6.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f2)->f1)== 1){_LLAA: _tmp1A7=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f1)->f1)->f1;_tmp1A8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f1)->f1)->f2;_tmp1A9=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f2)->f1)->f1;_tmp1AA=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6.f2)->f1)->f2;{enum Cyc_Absyn_Sign u1=_tmp1A7;enum Cyc_Absyn_Size_of s1=_tmp1A8;enum Cyc_Absyn_Sign u2=_tmp1A9;enum Cyc_Absyn_Size_of s2=_tmp1AA;
# 1195
s1=(int)s1 == 3?2U: s1;
s2=(int)s2 == 3?2U: s2;
if((int)s1 == (int)s2)return({struct _tuple13 _tmp223;_tmp223.f1=topt,_tmp223.f2=a;_tmp223;});
goto _LLA5;}}else{goto _LLAC;}}else{goto _LLAC;}}else{goto _LLAC;}}else{_LLAC:
 goto _LLA5;}_LLA5:;}
# 1201
if((int)c == 1){
if(topt == 0)return({struct _tuple13 _tmp225;_tmp225.f1=0,_tmp225.f2=a;_tmp225;});{
void*t=topt;
return({struct _tuple13 _tmp226;({void*_tmp2DB=Cyc_AssnDef_cast(tp,t);_tmp226.f1=_tmp2DB;}),_tmp226.f2=a;_tmp226;});}}else{
# 1207
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _tmp227;topt == 0?_tmp227.f1=0:({void*_tmp2DC=Cyc_AssnDef_cast(tp,topt);_tmp227.f1=_tmp2DC;}),_tmp227.f2=a;_tmp227;});}}}case 16: _tmp142=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*eopt=_tmp142;struct Cyc_Absyn_Exp*e=_tmp144;
# 1211
if(eopt != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain)).f2;
return({struct _tuple13 _tmp228;_tmp228.f1=0,({void*_tmp2DD=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp228.f2=_tmp2DD;});_tmp228;});}case 32:
# 1216
 goto _LL2A;case 33: _LL2A:
 goto _LL2C;case 17: _LL2C:
 goto _LL2E;case 18: _LL2E:
 goto _LL30;case 39: _LL30:
 goto _LL32;case 19: _LL32:
 return({struct _tuple13 _tmp229;({void*_tmp2DE=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->tag=1,_tmp1AB->f1=e;_tmp1AB;});_tmp229.f1=_tmp2DE;}),_tmp229.f2=ain;_tmp229;});case 21: _tmp142=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
# 1224
_tmp142=e;goto _LL36;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _tmp22A;_tmp22A.f1=0,_tmp22A.f2=ain;_tmp22A;});}case 38: _tmp142=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp142;
# 1229
_tmp142=e;goto _LL38;}case 15: _tmp142=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp142;
# 1231
return({struct _tuple13 _tmp22B;_tmp22B.f1=0,({void*_tmp2DF=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp22B.f2=_tmp2DF;});_tmp22B;});}case 20: _tmp142=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
# 1233
_tmp142=e;goto _LL3C;}case 22: _tmp142=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp142;
_tmp142=e;_tmp144=Cyc_Vcgen_shared_zero_exp();goto _LL3E;}case 23: _tmp142=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Absyn_Exp*e2=_tmp144;
# 1236
struct _tuple13 _tmp1AC=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple13 _stmttmp45=_tmp1AC;struct _tuple13 _tmp1AD=_stmttmp45;void*_tmp1AF;void*_tmp1AE;_tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;{void*t1=_tmp1AE;void*a1=_tmp1AF;
struct _tuple13 _tmp1B0=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);struct _tuple13 _stmttmp46=_tmp1B0;struct _tuple13 _tmp1B1=_stmttmp46;void*_tmp1B3;void*_tmp1B2;_tmp1B2=_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{void*t2=_tmp1B2;void*a2=_tmp1B3;
{void*_tmp1B4=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp47=_tmp1B4;void*_tmp1B5=_stmttmp47;if(*((int*)_tmp1B5)== 6)
goto _LLB4;else{
# 1243
Cyc_Vcgen_update_try_assn(env,a2);{
void*term_numelts;void*term_typebound;
# 1247
if(t2 == 0)goto _LLB4;
term_numelts=Cyc_AssnDef_primop(18U,({void*_tmp1B6[1];_tmp1B6[0]=t1;({struct Cyc_List_List*(*_tmp2E0)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2E0(_tag_fat(_tmp1B6,sizeof(void*),1));});}));{
# 1259 "vcgen.cyc"
void*_tmp1B7=Cyc_AssnDef_ult(t2,term_numelts);void*inbound_assn=_tmp1B7;
({struct Cyc_Hashtable_Table**_tmp2E2=env->assn_info;struct Cyc_Absyn_Exp*_tmp2E1=e1;Cyc_Vcgen_insert_assn_info(_tmp2E2,_tmp2E1,({struct _tuple13*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->f1=a2,_tmp1B8->f2=inbound_assn;_tmp1B8;}));});
a2=Cyc_AssnDef_and(a2,inbound_assn);}}}_LLB4:;}
# 1263
return({struct _tuple13 _tmp22C;_tmp22C.f1=0,_tmp22C.f2=a2;_tmp22C;});}}}case 31: _tmp142=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_List_List*es=_tmp142;
# 1265
_tmp142=es;goto _LL42;}case 24: _tmp142=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL42: {struct Cyc_List_List*es=_tmp142;
return({struct _tuple13 _tmp22D;_tmp22D.f1=0,({void*_tmp2E3=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_tmp22D.f2=_tmp2E3;});_tmp22D;});}case 25: _tmp142=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_List_List*dles=_tmp142;
# 1268
_tmp142=dles;goto _LL46;}case 26: _tmp142=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_LL46: {struct Cyc_List_List*dles=_tmp142;
_tmp142=dles;goto _LL48;}case 29: _tmp142=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp141)->f3;_LL48: {struct Cyc_List_List*dles=_tmp142;
_tmp142=dles;goto _LL4A;}case 36: _tmp142=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_LL4A: {struct Cyc_List_List*dles=_tmp142;
_tmp142=dles;goto _LL4C;}case 30: _tmp142=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp142;
# 1273
for(1;dles != 0;dles=dles->tl){
struct _tuple18*_tmp1B9=(struct _tuple18*)dles->hd;struct _tuple18*_stmttmp48=_tmp1B9;struct _tuple18*_tmp1BA=_stmttmp48;void*_tmp1BB;_tmp1BB=_tmp1BA->f2;{struct Cyc_Absyn_Exp*e=_tmp1BB;
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;}}
# 1277
return({struct _tuple13 _tmp22E;_tmp22E.f1=0,_tmp22E.f2=ain;_tmp22E;});}case 27: _tmp142=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp141)->f2;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp141)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp142;struct Cyc_Absyn_Exp*e1=_tmp144;struct Cyc_Absyn_Exp*e2=_tmp146;
# 1281
struct _tuple13 _tmp1BC=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);struct _tuple13 _stmttmp49=_tmp1BC;struct _tuple13 _tmp1BD=_stmttmp49;void*_tmp1BF;void*_tmp1BE;_tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;{void*t1=_tmp1BE;void*a=_tmp1BF;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1C0=({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->tag=2,_tmp1C7->f1=vd;_tmp1C7;});struct Cyc_AssnDef_Var_AssnDef_Term_struct*v=_tmp1C0;
# 1284
a=({struct Cyc_Vcgen_Env*_tmp2E6=env;void*_tmp2E5=a;void*_tmp2E4=(void*)v;Cyc_Vcgen_do_assign(_tmp2E6,_tmp2E5,_tmp2E4,Cyc_AssnDef_zero());});
while(1){
void*_tmp1C1=({void*_tmp2E7=a;Cyc_AssnDef_and(_tmp2E7,Cyc_AssnDef_ult((void*)v,t1));});void*at=_tmp1C1;
void*_tmp1C2=({void*_tmp2E8=a;Cyc_AssnDef_and(_tmp2E8,Cyc_AssnDef_ulte(t1,(void*)v));});void*af=_tmp1C2;
struct _tuple13 _tmp1C3=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);struct _tuple13 _stmttmp4A=_tmp1C3;struct _tuple13 _tmp1C4=_stmttmp4A;void*_tmp1C5;_tmp1C5=_tmp1C4.f2;{void*abody=_tmp1C5;
abody=({struct Cyc_Vcgen_Env*_tmp2ED=env;void*_tmp2EC=abody;void*_tmp2EB=(void*)v;Cyc_Vcgen_do_assign(_tmp2ED,_tmp2EC,_tmp2EB,Cyc_AssnDef_primop(0U,({void*_tmp1C6[2];_tmp1C6[0]=(void*)v,({void*_tmp2E9=Cyc_AssnDef_one();_tmp1C6[1]=_tmp2E9;});({struct Cyc_List_List*(*_tmp2EA)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp2EA(_tag_fat(_tmp1C6,sizeof(void*),2));});})));});
if(Cyc_Vcgen_simple_implies(& abody,& a)){a=af;break;}
# 1292
a=Cyc_Vcgen_widen(Cyc_AssnDef_or(abody,a));}}
# 1294
return({struct _tuple13 _tmp22F;_tmp22F.f1=0,_tmp22F.f2=a;_tmp22F;});}}case 28: _tmp142=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
# 1296
return({struct _tuple13 _tmp230;_tmp230.f1=0,({void*_tmp2EE=(Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain)).f2;_tmp230.f2=_tmp2EE;});_tmp230;});}case 34: _tmp145=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_MallocInfo mi=_tmp145;
# 1298
if(mi.rgn != 0)
ain=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain)).f2;
return({struct _tuple13 _tmp231;_tmp231.f1=0,({void*_tmp2EF=(Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain)).f2;_tmp231.f2=_tmp2EF;});_tmp231;});}case 35: _tmp142=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp141)->f1;_tmp144=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp141)->f2;{struct Cyc_Absyn_Exp*e1=_tmp142;struct Cyc_Absyn_Exp*e2=_tmp144;
# 1302
struct _tuple13 _tmp1C8=Cyc_Vcgen_vcgen_lexp(env,e1,ain);struct _tuple13 _stmttmp4B=_tmp1C8;struct _tuple13 _tmp1C9=_stmttmp4B;void*_tmp1CB;void*_tmp1CA;_tmp1CA=_tmp1C9.f1;_tmp1CB=_tmp1C9.f2;{void*lt1=_tmp1CA;void*a1=_tmp1CB;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple13 _tmp1CC=Cyc_Vcgen_vcgen_lexp(env,e2,a1);struct _tuple13 _stmttmp4C=_tmp1CC;struct _tuple13 _tmp1CD=_stmttmp4C;void*_tmp1CF;void*_tmp1CE;_tmp1CE=_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;{void*lt2=_tmp1CE;void*a2=_tmp1CF;
Cyc_Vcgen_update_try_assn(env,a2);{
void*_tmp1D0=Cyc_Vcgen_deref_lterm(env,lt1,a2);void*t1=_tmp1D0;
void*_tmp1D1=Cyc_Vcgen_deref_lterm(env,lt2,a2);void*t2=_tmp1D1;
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple13 _tmp232;_tmp232.f1=0,_tmp232.f2=a2;_tmp232;});}}}}}case 37: _tmp142=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Stmt*s=_tmp142;
# 1312
void**_tmp1D2=(env->shared)->exp_stmt;void**old_exp_stmt=_tmp1D2;
({void**_tmp2F0=({void**_tmp1D3=_cycalloc(sizeof(*_tmp1D3));*_tmp1D3=0;_tmp1D3;});(env->shared)->exp_stmt=_tmp2F0;});{
void*_tmp1D4=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);void*aout=_tmp1D4;
void*_tmp1D5=*((void**)_check_null((env->shared)->exp_stmt));void*t=_tmp1D5;
(env->shared)->exp_stmt=old_exp_stmt;
return({struct _tuple13 _tmp233;_tmp233.f1=t,_tmp233.f2=aout;_tmp233;});}}case 40:
 return({struct _tuple13 _tmp234;_tmp234.f1=0,_tmp234.f2=ain;_tmp234;});case 41: _tmp142=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e=_tmp142;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2: _tmp143=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct _fat_ptr s=_tmp143;
return({struct _tuple13 _tmp235;({void*_tmp2F1=Cyc_AssnDef_zero();_tmp235.f1=_tmp2F1;}),_tmp235.f2=ain;_tmp235;});}default: _tmp142=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp141)->f1;{struct Cyc_Absyn_Exp*e1=_tmp142;
# 1322
struct _tuple13 _tmp1D6=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp4D=_tmp1D6;struct _tuple13 _tmp1D7=_stmttmp4D;void*_tmp1D8;_tmp1D8=_tmp1D7.f1;{void*at=_tmp1D8;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1DB=({struct Cyc_String_pa_PrintArg_struct _tmp237;_tmp237.tag=0,({
struct _fat_ptr _tmp2F2=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(at));_tmp237.f1=_tmp2F2;});_tmp237;});struct Cyc_String_pa_PrintArg_struct _tmp1DC=({struct Cyc_String_pa_PrintArg_struct _tmp236;_tmp236.tag=0,({struct _fat_ptr _tmp2F3=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_tmp236.f1=_tmp2F3;});_tmp236;});void*_tmp1D9[2];_tmp1D9[0]=& _tmp1DB,_tmp1D9[1]=& _tmp1DC;({unsigned _tmp2F5=e->loc;struct _fat_ptr _tmp2F4=({const char*_tmp1DA="cannot prove @assert(%s) \n\nfrom %s";_tag_fat(_tmp1DA,sizeof(char),35U);});Cyc_Warn_err(_tmp2F5,_tmp2F4,_tag_fat(_tmp1D9,sizeof(void*),2));});});
return({struct _tuple13 _tmp238;({void*_tmp2F7=Cyc_AssnDef_one();_tmp238.f1=_tmp2F7;}),({void*_tmp2F6=Cyc_AssnDef_and(ain,at);_tmp238.f2=_tmp2F6;});_tmp238;});}}};}}
# 1332
void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1335
struct _tuple17 _tmp1DD=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);struct _tuple17 _stmttmp4E=_tmp1DD;struct _tuple17 _tmp1DE=_stmttmp4E;void*_tmp1E2;void*_tmp1E1;void*_tmp1E0;void*_tmp1DF;_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;_tmp1E1=_tmp1DE.f3;_tmp1E2=_tmp1DE.f4;{struct Cyc_List_List*args=_tmp1DF;struct Cyc_Absyn_Exp*reqopt=_tmp1E0;struct Cyc_Absyn_Exp*ensopt=_tmp1E1;struct Cyc_Absyn_Vardecl*ret_value=_tmp1E2;
void**result_assn=ensopt == 0?0:({void**_tmp1EF=_cycalloc(sizeof(*_tmp1EF));*_tmp1EF=(void*)& Cyc_AssnDef_false_assn;_tmp1EF;});
struct Cyc_Vcgen_Env*_tmp1E3=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn);struct Cyc_Vcgen_Env*env=_tmp1E3;
if(reqopt != 0)
# 1340
a=(Cyc_Vcgen_vcgen_test(env,reqopt,a)).f1;
# 1342
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1344
if(ensopt != 0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple13 _tmp1E4=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_AssnDef_true_assn);struct _tuple13 _stmttmp4F=_tmp1E4;struct _tuple13 _tmp1E5=_stmttmp4F;void*_tmp1E6;_tmp1E6=_tmp1E5.f1;{void*at=_tmp1E6;
struct Cyc_Vcgen_SharedEnv*_tmp1E7=env->shared;struct Cyc_Vcgen_SharedEnv*senv=_tmp1E7;
void*_tmp1E8=*((void**)_check_null(senv->res_assn));void*res_assn_opt=_tmp1E8;
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt != 0)res_assn=res_assn_opt;{
struct _fat_ptr str=({const char*_tmp1EE="NULL";_tag_fat(_tmp1EE,sizeof(char),5U);});
if(senv->res_term != 0)str=Cyc_AssnDef_term2string((void*)_check_null(senv->res_term));
if(ret_value != 0 &&(unsigned)senv->res_term)
res_assn=({void*_tmp2F9=res_assn;Cyc_AssnDef_and(_tmp2F9,({void*_tmp2F8=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->tag=2,_tmp1E9->f1=ret_value;_tmp1E9;});Cyc_AssnDef_eq(_tmp2F8,senv->res_term);}));});
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _tmp1EC=({struct Cyc_String_pa_PrintArg_struct _tmp23A;_tmp23A.tag=0,({
struct _fat_ptr _tmp2FA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_tmp23A.f1=_tmp2FA;});_tmp23A;});struct Cyc_String_pa_PrintArg_struct _tmp1ED=({struct Cyc_String_pa_PrintArg_struct _tmp239;_tmp239.tag=0,({struct _fat_ptr _tmp2FB=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn)));_tmp239.f1=_tmp2FB;});_tmp239;});void*_tmp1EA[2];_tmp1EA[0]=& _tmp1EC,_tmp1EA[1]=& _tmp1ED;({unsigned _tmp2FD=ens->loc;struct _fat_ptr _tmp2FC=({const char*_tmp1EB="cannot prove the @ensures clause %s from %s";_tag_fat(_tmp1EB,sizeof(char),44U);});Cyc_Warn_err(_tmp2FD,_tmp2FC,_tag_fat(_tmp1EA,sizeof(void*),2));});});}}}
# 1359
Cyc_AssnDef_reset_hash_cons_table();}}
