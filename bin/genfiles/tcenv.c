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

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
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

static inline unsigned int _check_times(unsigned x, unsigned y) {
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 172
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 843 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 863
void*Cyc_Absyn_compress(void*);
# 888
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 890
extern void*Cyc_Absyn_empty_effect;
# 892
extern void*Cyc_Absyn_false_type;
# 894
extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 909
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 937
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*,struct Cyc_List_List*,void*,struct Cyc_Absyn_Tvar*,unsigned);
# 40
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*,void*,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*,int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*);
# 44
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*,void*,void*);
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*,void*);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,int);
# 100 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 140
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 144
int Cyc_Tcutil_region_in_effect(int,void*,void*);
# 159
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 204
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 75
struct _tuple11 Cyc_Kinds_swap_kind(void*,void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 173 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 67 "tcenv.h"
void*Cyc_Tcenv_env_err(struct _fat_ptr);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 51 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 53
void*Cyc_Tcenv_env_err(struct _fat_ptr msg){
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct _tuple12*ctrl_env;void*capability;void*curr_rgn;void*curr_lifo_rgn;struct Cyc_Tcenv_FenvFlags flags;};
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void){
# 117
struct Cyc_Tcenv_Genv*_tmp0=({struct Cyc_Tcenv_Genv*_tmp3=_cycalloc(sizeof(*_tmp3));({struct Cyc_Dict_Dict _tmp10F=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmp3->aggrdecls=_tmp10F;}),({
struct Cyc_Dict_Dict _tmp10E=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmp3->datatypedecls=_tmp10E;}),({
struct Cyc_Dict_Dict _tmp10D=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmp3->enumdecls=_tmp10D;}),({
struct Cyc_Dict_Dict _tmp10C=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmp3->typedefs=_tmp10C;}),({
struct Cyc_Dict_Dict _tmp10B=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmp3->ordinaries=_tmp10B;});_tmp3;});
# 117
struct Cyc_Tcenv_Genv*ae=_tmp0;
# 122
({struct Cyc_Dict_Dict _tmp114=({struct Cyc_Dict_Dict(*_tmp113)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp112=ae->datatypedecls;struct _tuple0*_tmp111=Cyc_Absyn_exn_name;_tmp113(_tmp112,_tmp111,({struct Cyc_Absyn_Datatypedecl**_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Datatypedecl*_tmp110=Cyc_Absyn_exn_tud();*_tmp1=_tmp110;});_tmp1;}));});ae->datatypedecls=_tmp114;});
return({struct Cyc_Tcenv_Tenv*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->ns=0,_tmp2->ae=ae,_tmp2->le=0,_tmp2->allow_valueof=0,_tmp2->in_extern_c_include=0,_tmp2->in_tempest=0,_tmp2->tempest_generalize=0,_tmp2->in_extern_c_inc_repeat=0;_tmp2;});}struct _tuple13{void*f1;int f2;};
# 125
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q,int is_use){
struct _tuple13*_tmp4=({(struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((te->ae)->ordinaries,q);struct _tuple13*ans=_tmp4;
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 131
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({(struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((te->ae)->aggrdecls,q);}
# 134
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((te->ae)->datatypedecls,q);}
# 137
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmp5=_region_malloc(r,sizeof(*_tmp5));({struct Cyc_Absyn_Datatypedecl**_tmp115=({(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((te->ae)->datatypedecls,q);*_tmp5=_tmp115;});_tmp5;});}
# 140
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((te->ae)->enumdecls,q);}
# 143
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({(struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((te->ae)->typedefs,q);}
# 149
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp6=te->le;struct Cyc_Tcenv_Fenv*le=_tmp6;
if(le == 0)({(int(*)(struct _fat_ptr))Cyc_Tcenv_env_err;})(err_msg);
return le;}
# 154
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)({int(*_tmp116)(struct _fat_ptr)=({(int(*)(struct _fat_ptr))Cyc_Tcenv_env_err;});_tmp116(({const char*_tmp7="put_fenv";_tag_fat(_tmp7,sizeof(char),9U);}));});{
struct Cyc_Tcenv_Tenv*_tmp8=({struct Cyc_Tcenv_Tenv*_tmp9=_cycalloc(sizeof(*_tmp9));*_tmp9=*te;_tmp9;});struct Cyc_Tcenv_Tenv*ans=_tmp8;
ans->le=fe;
return ans;}}
# 160
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpA=({struct Cyc_Tcenv_Tenv*_tmpB=_cycalloc(sizeof(*_tmpB));*_tmpB=*te;_tmpB;});struct Cyc_Tcenv_Tenv*ans=_tmpA;
ans->le=0;
return ans;}
# 166
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
return(({struct Cyc_Tcenv_Tenv*_tmp117=te;Cyc_Tcenv_get_fenv(_tmp117,({const char*_tmpC="return_typ";_tag_fat(_tmpC,sizeof(char),11U);}));})->shared)->return_typ;}
# 170
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD=te->le;struct Cyc_Tcenv_Fenv*le=_tmpD;
if(te->le == 0)return 0;
return(_check_null(le))->type_vars;}
# 175
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpE=_cycalloc(sizeof(*_tmpE));({struct Cyc_List_List*_tmp118=Cyc_Tcenv_lookup_type_vars(te);_tmpE->v=_tmp118;});_tmpE;});}
# 179
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv*_tmpF=({struct Cyc_Tcenv_Fenv*_tmp12=_cycalloc(sizeof(*_tmp12));({struct Cyc_Tcenv_Fenv _tmp11A=*({struct Cyc_Tcenv_Tenv*_tmp119=te;Cyc_Tcenv_get_fenv(_tmp119,({const char*_tmp11="add_type_vars";_tag_fat(_tmp11,sizeof(char),14U);}));});*_tmp12=_tmp11A;});_tmp12;});struct Cyc_Tcenv_Fenv*fe=_tmpF;
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp10=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(tvs,fe->type_vars);struct Cyc_List_List*new_tvs=_tmp10;
Cyc_Tcutil_check_unique_tvars(loc,new_tvs);
fe->type_vars=new_tvs;
return Cyc_Tcenv_put_fenv(te,fe);}}
# 188
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp13=te->le;struct Cyc_Tcenv_Fenv*le=_tmp13;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp14=({struct Cyc_Tcenv_Fenv*_tmp15=_cycalloc(sizeof(*_tmp15));*_tmp15=*le;_tmp15;});struct Cyc_Tcenv_Fenv*ans=_tmp14;
(ans->flags).in_new=status;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 195
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16=te->le;struct Cyc_Tcenv_Fenv*le=_tmp16;
if(le == 0)return 0U;
return(le->flags).in_new;}
# 200
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17=te->le;struct Cyc_Tcenv_Fenv*le=_tmp17;
if(le == 0)return 0;
return(le->flags).abstract_ok;}
# 205
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18=te->le;struct Cyc_Tcenv_Fenv*le=_tmp18;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp19=({struct Cyc_Tcenv_Fenv*_tmp1A=_cycalloc(sizeof(*_tmp1A));*_tmp1A=*le;_tmp1A;});struct Cyc_Tcenv_Fenv*ans=_tmp19;
(ans->flags).abstract_ok=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 212
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1B=te->le;struct Cyc_Tcenv_Fenv*le=_tmp1B;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1C=({struct Cyc_Tcenv_Fenv*_tmp1D=_cycalloc(sizeof(*_tmp1D));*_tmp1D=*le;_tmp1D;});struct Cyc_Tcenv_Fenv*ans=_tmp1C;
(ans->flags).abstract_ok=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 219
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1E=te->le;struct Cyc_Tcenv_Fenv*le=_tmp1E;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1F=({struct Cyc_Tcenv_Fenv*_tmp20=_cycalloc(sizeof(*_tmp20));*_tmp20=*le;_tmp20;});struct Cyc_Tcenv_Fenv*ans=_tmp1F;
(ans->flags).in_notreadctxt=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 226
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp21=te->le;struct Cyc_Tcenv_Fenv*le=_tmp21;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp22=({struct Cyc_Tcenv_Fenv*_tmp23=_cycalloc(sizeof(*_tmp23));*_tmp23=*le;_tmp23;});struct Cyc_Tcenv_Fenv*ans=_tmp22;
(ans->flags).in_notreadctxt=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 233
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp24=te->le;struct Cyc_Tcenv_Fenv*le=_tmp24;
if(le == 0)return 0;
return(le->flags).in_notreadctxt;}
# 238
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25=te->le;struct Cyc_Tcenv_Fenv*le=_tmp25;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp26=({struct Cyc_Tcenv_Fenv*_tmp27=_cycalloc(sizeof(*_tmp27));*_tmp27=*le;_tmp27;});struct Cyc_Tcenv_Fenv*ans=_tmp26;
(ans->flags).in_lhs=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 245
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp28=te->le;struct Cyc_Tcenv_Fenv*le=_tmp28;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp29=({struct Cyc_Tcenv_Fenv*_tmp2A=_cycalloc(sizeof(*_tmp2A));*_tmp2A=*le;_tmp2A;});struct Cyc_Tcenv_Fenv*ans=_tmp29;
(ans->flags).in_lhs=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 252
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B=te->le;struct Cyc_Tcenv_Fenv*le=_tmp2B;
if(le == 0)return 0;
return(le->flags).in_lhs;}
# 257
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C=te->le;struct Cyc_Tcenv_Fenv*le=_tmp2C;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp2D=({struct Cyc_Tcenv_Fenv*_tmp2E=_cycalloc(sizeof(*_tmp2E));*_tmp2E=*le;_tmp2E;});struct Cyc_Tcenv_Fenv*ans=_tmp2D;
(ans->flags).in_stmt_exp=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 264
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2F=te->le;struct Cyc_Tcenv_Fenv*le=_tmp2F;
if(le == 0)return 0;
return(le->flags).in_stmt_exp;}
# 271
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 273
const struct _tuple12*_tmp30=({struct Cyc_Tcenv_Tenv*_tmp11B=te;Cyc_Tcenv_get_fenv(_tmp11B,({const char*_tmp32="process_fallthru";_tag_fat(_tmp32,sizeof(char),17U);}));})->ctrl_env;const struct _tuple12*ans=_tmp30;
if(ans != (const struct _tuple12*)0)
({struct Cyc_Absyn_Switch_clause**_tmp11C=({struct Cyc_Absyn_Switch_clause**_tmp31=_cycalloc(sizeof(*_tmp31));*_tmp31=(*ans).f1;_tmp31;});*clauseopt=_tmp11C;});
return ans;}
# 279
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 281
struct Cyc_List_List*ft_typ=0;
for(1;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,_tmp33->tl=ft_typ;_tmp33;});}{
struct _tuple12*_tmp34=({struct _tuple12*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->f1=clause,_tmp38->f2=new_tvs,({struct Cyc_List_List*_tmp11D=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(ft_typ);_tmp38->f3=_tmp11D;});_tmp38;});struct _tuple12*new_ctrl_env=_tmp34;
struct Cyc_Tcenv_Fenv*_tmp35=({struct Cyc_Tcenv_Fenv*_tmp37=_cycalloc(sizeof(*_tmp37));({struct Cyc_Tcenv_Fenv _tmp11F=*({struct Cyc_Tcenv_Tenv*_tmp11E=te;Cyc_Tcenv_get_fenv(_tmp11E,({const char*_tmp36="set_fallthru";_tag_fat(_tmp36,sizeof(char),13U);}));});*_tmp37=_tmp11F;});_tmp37;});struct Cyc_Tcenv_Fenv*new_fe=_tmp35;
new_fe->ctrl_env=(const struct _tuple12*)new_ctrl_env;
return Cyc_Tcenv_put_fenv(te,new_fe);}}
# 290
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Fenv*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct Cyc_Tcenv_Fenv _tmp121=*({struct Cyc_Tcenv_Tenv*_tmp120=te;Cyc_Tcenv_get_fenv(_tmp120,({const char*_tmp39="clear_fallthru";_tag_fat(_tmp39,sizeof(char),15U);}));});*_tmp3A=_tmp121;});_tmp3A;});
fe->ctrl_env=0;
return Cyc_Tcenv_put_fenv(te,fe);}
# 296
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3B=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=*_check_null(te->le);_tmp3E;});struct Cyc_Tcenv_Fenv*fe=_tmp3B;
struct Cyc_Tcenv_Tenv*_tmp3C=({struct Cyc_Tcenv_Tenv*_tmp3D=_cycalloc(sizeof(*_tmp3D));*_tmp3D=*te;_tmp3D;});struct Cyc_Tcenv_Tenv*ans=_tmp3C;
ans->allow_valueof=1;
return ans;}
# 302
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3F=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp42=_cycalloc(sizeof(*_tmp42));*_tmp42=*_check_null(te->le);_tmp42;});struct Cyc_Tcenv_Fenv*fe=_tmp3F;
struct Cyc_Tcenv_Tenv*_tmp40=({struct Cyc_Tcenv_Tenv*_tmp41=_cycalloc(sizeof(*_tmp41));*_tmp41=*te;_tmp41;});struct Cyc_Tcenv_Tenv*ans=_tmp40;
ans->in_extern_c_include=1;
return ans;}
# 308
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp43=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=*_check_null(te->le);_tmp46;});struct Cyc_Tcenv_Fenv*fe=_tmp43;
struct Cyc_Tcenv_Tenv*_tmp44=({struct Cyc_Tcenv_Tenv*_tmp45=_cycalloc(sizeof(*_tmp45));*_tmp45=*te;_tmp45;});struct Cyc_Tcenv_Tenv*ans=_tmp44;
ans->in_tempest=1;
return ans;}
# 314
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp47=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=*_check_null(te->le);_tmp4A;});struct Cyc_Tcenv_Fenv*fe=_tmp47;
struct Cyc_Tcenv_Tenv*_tmp48=({struct Cyc_Tcenv_Tenv*_tmp49=_cycalloc(sizeof(*_tmp49));*_tmp49=*te;_tmp49;});struct Cyc_Tcenv_Tenv*ans=_tmp48;
ans->in_tempest=0;
return ans;}
# 320
int Cyc_Tcenv_in_tempest(struct Cyc_Tcenv_Tenv*te){
return te->in_tempest;}
# 323
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened,int lifo){
struct Cyc_Tcenv_Fenv*_tmp4B=({struct Cyc_Tcenv_Fenv*_tmp53=_cycalloc(sizeof(*_tmp53));({struct Cyc_Tcenv_Fenv _tmp123=*({struct Cyc_Tcenv_Tenv*_tmp122=te;Cyc_Tcenv_get_fenv(_tmp122,({const char*_tmp52="add_region";_tag_fat(_tmp52,sizeof(char),11U);}));});*_tmp53=_tmp123;});_tmp53;});struct Cyc_Tcenv_Fenv*ans=_tmp4B;
struct Cyc_RgnOrder_RgnPO*_tmp4C=ans->region_order;struct Cyc_RgnOrder_RgnPO*region_order=_tmp4C;
{void*_tmp4D=Cyc_Absyn_compress(rgn);void*_stmttmp0=_tmp4D;void*_tmp4E=_stmttmp0;void*_tmp4F;if(*((int*)_tmp4E)== 2){_tmp4F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->f1;{struct Cyc_Absyn_Tvar*x=_tmp4F;
region_order=Cyc_RgnOrder_add_youngest(region_order,x,opened);goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 330
ans->region_order=region_order;
({void*_tmp126=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({void*_tmp125=Cyc_Absyn_access_eff(rgn);_tmp51->hd=_tmp125;}),({
struct Cyc_List_List*_tmp124=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=ans->capability,_tmp50->tl=0;_tmp50;});_tmp51->tl=_tmp124;});_tmp51;}));
# 331
ans->capability=_tmp126;});
# 334
if(lifo)ans->curr_lifo_rgn=rgn;
return Cyc_Tcenv_put_fenv(te,ans);}
# 338
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
struct Cyc_Tcenv_Fenv*_tmp54=({struct Cyc_Tcenv_Fenv*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Tcenv_Fenv _tmp128=*({struct Cyc_Tcenv_Tenv*_tmp127=te;Cyc_Tcenv_get_fenv(_tmp127,({const char*_tmp5A="new_named_block";_tag_fat(_tmp5A,sizeof(char),16U);}));});*_tmp5B=_tmp128;});_tmp5B;});struct Cyc_Tcenv_Fenv*ans=_tmp54;
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp55=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=2,_tmp59->f1=block_rgn;_tmp59;});struct Cyc_Absyn_VarType_Absyn_Type_struct*block_typ=_tmp55;
({struct Cyc_List_List*_tmp129=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=block_rgn,_tmp56->tl=ans->type_vars;_tmp56;});ans->type_vars=_tmp129;});
Cyc_Tcutil_check_unique_tvars(loc,ans->type_vars);
({struct Cyc_RgnOrder_RgnPO*_tmp12A=Cyc_RgnOrder_add_youngest(ans->region_order,block_rgn,0);ans->region_order=_tmp12A;});
({void*_tmp12D=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({void*_tmp12C=Cyc_Absyn_access_eff((void*)block_typ);_tmp58->hd=_tmp12C;}),({
struct Cyc_List_List*_tmp12B=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=ans->capability,_tmp57->tl=0;_tmp57;});_tmp58->tl=_tmp12B;});_tmp58;}));
# 344
ans->capability=_tmp12D;});
# 346
ans->curr_rgn=(void*)block_typ;
return Cyc_Tcenv_put_fenv(te,ans);}
# 350
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Kinds_rk};
# 352
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(loc,te,t);}struct _tuple14{void*f1;void*f2;};
# 358
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned loc){
# 360
struct Cyc_Tcenv_Fenv*_tmp5C=({struct Cyc_Tcenv_Fenv*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Tcenv_Fenv _tmp12F=*({struct Cyc_Tcenv_Tenv*_tmp12E=te;Cyc_Tcenv_get_fenv(_tmp12E,({const char*_tmp5D="new_outlives_constraints";_tag_fat(_tmp5D,sizeof(char),25U);}));});*_tmp5E=_tmp12F;});_tmp5E;});struct Cyc_Tcenv_Fenv*ans=_tmp5C;
for(1;cs != 0;cs=cs->tl){
({struct Cyc_RgnOrder_RgnPO*_tmp130=
Cyc_RgnOrder_add_outlives_constraint(ans->region_order,(*((struct _tuple14*)cs->hd)).f1,(*((struct _tuple14*)cs->hd)).f2,loc);
# 362
ans->region_order=_tmp130;});}
# 364
return Cyc_Tcenv_put_fenv(te,ans);}
# 367
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned loc){
# 371
r1=Cyc_Absyn_compress(r1);
r2=Cyc_Absyn_compress(r2);{
struct Cyc_Absyn_Kind*_tmp5F=Cyc_Tcutil_type_kind(r1);struct Cyc_Absyn_Kind*r1k=_tmp5F;
struct Cyc_Absyn_Kind*_tmp60=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*r2k=_tmp60;
# 377
int r1_le_r2=Cyc_Kinds_kind_leq(r1k,r2k);
int r2_le_r1=Cyc_Kinds_kind_leq(r2k,r1k);
if(!r1_le_r2 && !r2_le_r1){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmpFE;_tmpFE.tag=0,({struct _fat_ptr _tmp131=({const char*_tmp6F="Cannot compare region handles for ";_tag_fat(_tmp6F,sizeof(char),35U);});_tmpFE.f1=_tmp131;});_tmpFE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFD;_tmpFD.tag=2,_tmpFD.f1=(void*)r1;_tmpFD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmpFC;_tmpFC.tag=0,({struct _fat_ptr _tmp132=({const char*_tmp6E=" and ";_tag_fat(_tmp6E,sizeof(char),6U);});_tmpFC.f1=_tmp132;});_tmpFC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFB;_tmpFB.tag=2,_tmpFB.f1=(void*)r2;_tmpFB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp66=({struct Cyc_Warn_String_Warn_Warg_struct _tmpFA;_tmpFA.tag=0,({
struct _fat_ptr _tmp133=({const char*_tmp6D="\n  kinds ";_tag_fat(_tmp6D,sizeof(char),10U);});_tmpFA.f1=_tmp133;});_tmpFA;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp67=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmpF9;_tmpF9.tag=9,_tmpF9.f1=r1k;_tmpF9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp68=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF8;_tmpF8.tag=0,({struct _fat_ptr _tmp134=({const char*_tmp6C=" and ";_tag_fat(_tmp6C,sizeof(char),6U);});_tmpF8.f1=_tmp134;});_tmpF8;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp69=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmpF7;_tmpF7.tag=9,_tmpF7.f1=r2k;_tmpF7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF6;_tmpF6.tag=0,({struct _fat_ptr _tmp135=({const char*_tmp6B=" are not compatible";_tag_fat(_tmp6B,sizeof(char),20U);});_tmpF6.f1=_tmp135;});_tmpF6;});void*_tmp61[9];_tmp61[0]=& _tmp62,_tmp61[1]=& _tmp63,_tmp61[2]=& _tmp64,_tmp61[3]=& _tmp65,_tmp61[4]=& _tmp66,_tmp61[5]=& _tmp67,_tmp61[6]=& _tmp68,_tmp61[7]=& _tmp69,_tmp61[8]=& _tmp6A;Cyc_Warn_err2(loc,_tag_fat(_tmp61,sizeof(void*),9));});
return Cyc_Tcenv_new_outlives_constraints(te,0,loc);}else{
# 384
if(r1_le_r2 && !r2_le_r1)
({struct _tuple11*_tmp138=({struct _tuple11*_tmp70=_cycalloc(sizeof(*_tmp70));({struct _tuple11 _tmp137=({void*_tmp136=r2;Cyc_Kinds_swap_kind(_tmp136,Cyc_Kinds_kind_to_bound(r1k));});*_tmp70=_tmp137;});_tmp70;});*oldtv=_tmp138;});else{
if(!r1_le_r2 && r2_le_r1)
({struct _tuple11*_tmp13B=({struct _tuple11*_tmp71=_cycalloc(sizeof(*_tmp71));({struct _tuple11 _tmp13A=({void*_tmp139=r1;Cyc_Kinds_swap_kind(_tmp139,Cyc_Kinds_kind_to_bound(r2k));});*_tmp71=_tmp13A;});_tmp71;});*oldtv=_tmp13B;});}}{
# 390
struct Cyc_List_List*_tmp72=0;struct Cyc_List_List*bds=_tmp72;
if((r1 != Cyc_Absyn_heap_rgn_type && r1 != Cyc_Absyn_unique_rgn_type)&& r1 != Cyc_Absyn_refcnt_rgn_type)
bds=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));({struct _tuple14*_tmp13D=({struct _tuple14*_tmp73=_cycalloc(sizeof(*_tmp73));({void*_tmp13C=Cyc_Absyn_access_eff(r1);_tmp73->f1=_tmp13C;}),_tmp73->f2=r2;_tmp73;});_tmp74->hd=_tmp13D;}),_tmp74->tl=bds;_tmp74;});
if((r2 != Cyc_Absyn_heap_rgn_type && r2 != Cyc_Absyn_unique_rgn_type)&& r2 != Cyc_Absyn_refcnt_rgn_type)
bds=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));({struct _tuple14*_tmp13F=({struct _tuple14*_tmp75=_cycalloc(sizeof(*_tmp75));({void*_tmp13E=Cyc_Absyn_access_eff(r2);_tmp75->f1=_tmp13E;}),_tmp75->f2=r1;_tmp75;});_tmp76->hd=_tmp13F;}),_tmp76->tl=bds;_tmp76;});
return Cyc_Tcenv_new_outlives_constraints(te,bds,loc);}}}
# 398
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp77=te->le;struct Cyc_Tcenv_Fenv*le=_tmp77;
if(le == 0)return Cyc_Absyn_heap_rgn_type;
return le->curr_rgn;}
# 403
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp78=te->le;struct Cyc_Tcenv_Fenv*le=_tmp78;
if(le == 0)return Cyc_Absyn_heap_rgn_type;
return le->curr_lifo_rgn;}
# 408
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp79=te->le;struct Cyc_Tcenv_Fenv*le=_tmp79;
if(le == 0)return 0;
return le->region_order;}
# 417
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Tenv*_tmp140=te;Cyc_Tcenv_get_fenv(_tmp140,({const char*_tmp7E="check_rgn_accessible";_tag_fat(_tmp7E,sizeof(char),21U);}));});
if(Cyc_Tcutil_region_in_effect(0,rgn,fe->capability)||
 Cyc_Tcutil_region_in_effect(1,rgn,fe->capability))
return;
if(({struct Cyc_RgnOrder_RgnPO*_tmp142=fe->region_order;void*_tmp141=Cyc_Absyn_access_eff(rgn);Cyc_RgnOrder_eff_outlives_eff(_tmp142,_tmp141,fe->capability);}))
return;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp100;_tmp100.tag=0,({struct _fat_ptr _tmp143=({const char*_tmp7D="Expression accesses unavailable region ";_tag_fat(_tmp7D,sizeof(char),40U);});_tmp100.f1=_tmp143;});_tmp100;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFF;_tmpFF.tag=2,_tmpFF.f1=(void*)rgn;_tmpFF;});void*_tmp7A[2];_tmp7A[0]=& _tmp7B,_tmp7A[1]=& _tmp7C;Cyc_Warn_err2(loc,_tag_fat(_tmp7A,sizeof(void*),2));});}
# 430
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
return({struct Cyc_RgnOrder_RgnPO*_tmp145=Cyc_Tcenv_curr_rgnpo(te);void*_tmp144=rt_a;Cyc_RgnOrder_rgn_outlives_rgn(_tmp145,_tmp144,rt_b);});}struct _tuple15{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned f4;};
# 436
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp7F=({
struct Cyc_Tcenv_Tenv*_tmp146=te;Cyc_Tcenv_get_fenv(_tmp146,({const char*_tmp86="check_effect_accessible";_tag_fat(_tmp86,sizeof(char),24U);}));});
# 437
struct Cyc_Tcenv_Fenv*_stmttmp1=_tmp7F;struct Cyc_Tcenv_Fenv*_tmp80=_stmttmp1;void*_tmp83;void*_tmp82;void*_tmp81;_tmp81=_tmp80->capability;_tmp82=_tmp80->region_order;_tmp83=_tmp80->shared;{void*capability=_tmp81;struct Cyc_RgnOrder_RgnPO*ro=_tmp82;struct Cyc_Tcenv_SharedFenv*shared=_tmp83;
# 439
if(Cyc_Tcutil_subset_effect(0,eff,capability))
return;
if(Cyc_RgnOrder_eff_outlives_eff(ro,eff,capability))
return;
({struct Cyc_List_List*_tmp148=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));
({struct _tuple15*_tmp147=({struct _tuple15*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=capability,_tmp84->f2=eff,_tmp84->f3=ro,_tmp84->f4=loc;_tmp84;});_tmp85->hd=_tmp147;}),_tmp85->tl=shared->delayed_effect_checks;_tmp85;});
# 443
shared->delayed_effect_checks=_tmp148;});}}
# 446
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp87=(({
struct Cyc_Tcenv_Tenv*_tmp149=te;Cyc_Tcenv_get_fenv(_tmp149,({const char*_tmp95="check_delayed_constraints";_tag_fat(_tmp95,sizeof(char),26U);}));})->shared)->delayed_effect_checks;
# 447
struct Cyc_List_List*checks=_tmp87;
# 449
for(1;checks != 0;checks=checks->tl){
struct _tuple15*_tmp88=(struct _tuple15*)checks->hd;struct _tuple15*_stmttmp2=_tmp88;struct _tuple15*_tmp89=_stmttmp2;unsigned _tmp8D;void*_tmp8C;void*_tmp8B;void*_tmp8A;_tmp8A=_tmp89->f1;_tmp8B=_tmp89->f2;_tmp8C=_tmp89->f3;_tmp8D=_tmp89->f4;{void*capability=_tmp8A;void*eff=_tmp8B;struct Cyc_RgnOrder_RgnPO*rgn_order=_tmp8C;unsigned loc=_tmp8D;
if(Cyc_Tcutil_subset_effect(1,eff,capability))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(rgn_order,eff,capability))
continue;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp104;_tmp104.tag=0,({struct _fat_ptr _tmp14A=({const char*_tmp94="Capability \n";_tag_fat(_tmp94,sizeof(char),13U);});_tmp104.f1=_tmp14A;});_tmp104;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp90=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp103;_tmp103.tag=2,_tmp103.f1=(void*)capability;_tmp103;});struct Cyc_Warn_String_Warn_Warg_struct _tmp91=({struct Cyc_Warn_String_Warn_Warg_struct _tmp102;_tmp102.tag=0,({
struct _fat_ptr _tmp14B=({const char*_tmp93="\ndoes not cover function's effect\n";_tag_fat(_tmp93,sizeof(char),35U);});_tmp102.f1=_tmp14B;});_tmp102;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp92=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp101;_tmp101.tag=2,_tmp101.f1=(void*)eff;_tmp101;});void*_tmp8E[4];_tmp8E[0]=& _tmp8F,_tmp8E[1]=& _tmp90,_tmp8E[2]=& _tmp91,_tmp8E[3]=& _tmp92;Cyc_Warn_err2(loc,_tag_fat(_tmp8E,sizeof(void*),4));});}}}struct _tuple16{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned f3;};
# 464
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*po){
# 466
struct Cyc_Tcenv_Fenv*_tmp96=te->le;struct Cyc_Tcenv_Fenv*le=_tmp96;
if(le == 0){
# 469
for(1;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple14*)po->hd)).f1,Cyc_Absyn_empty_effect)|| !({
void*_tmp14C=Cyc_Absyn_access_eff((*((struct _tuple14*)po->hd)).f2);Cyc_Tcutil_subset_effect(1,_tmp14C,Cyc_Absyn_empty_effect);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp105;_tmp105.tag=0,({struct _fat_ptr _tmp14D=({const char*_tmp99="the required region ordering is not satisfied here";_tag_fat(_tmp99,sizeof(char),51U);});_tmp105.f1=_tmp14D;});_tmp105;});void*_tmp97[1];_tmp97[0]=& _tmp98;Cyc_Warn_err2(loc,_tag_fat(_tmp97,sizeof(void*),1));});}
return;}{
# 475
struct Cyc_Tcenv_Fenv*_tmp9A=le;struct Cyc_Tcenv_Fenv*_stmttmp3=_tmp9A;struct Cyc_Tcenv_Fenv*_tmp9B=_stmttmp3;void*_tmp9D;void*_tmp9C;_tmp9C=_tmp9B->region_order;_tmp9D=_tmp9B->shared;{struct Cyc_RgnOrder_RgnPO*ro=_tmp9C;struct Cyc_Tcenv_SharedFenv*shared=_tmp9D;
if(!Cyc_RgnOrder_satisfies_constraints(ro,po,Cyc_Absyn_heap_rgn_type,0))
({struct Cyc_List_List*_tmp14F=({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));
({struct _tuple16*_tmp14E=({struct _tuple16*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=ro,_tmp9E->f2=po,_tmp9E->f3=loc;_tmp9E;});_tmp9F->hd=_tmp14E;}),_tmp9F->tl=shared->delayed_constraint_checks;_tmp9F;});
# 477
shared->delayed_constraint_checks=_tmp14F;});}}}
# 481
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmpA0=(({
struct Cyc_Tcenv_Tenv*_tmp150=te;Cyc_Tcenv_get_fenv(_tmp150,({const char*_tmpA9="check_delayed_constraints";_tag_fat(_tmpA9,sizeof(char),26U);}));})->shared)->delayed_constraint_checks;
# 482
struct Cyc_List_List*checks=_tmpA0;
# 484
for(1;checks != 0;checks=checks->tl){
struct _tuple16*_tmpA1=(struct _tuple16*)checks->hd;struct _tuple16*_stmttmp4=_tmpA1;struct _tuple16*_tmpA2=_stmttmp4;unsigned _tmpA5;void*_tmpA4;void*_tmpA3;_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;_tmpA5=_tmpA2->f3;{struct Cyc_RgnOrder_RgnPO*rgn_order=_tmpA3;struct Cyc_List_List*po=_tmpA4;unsigned loc=_tmpA5;
if(!Cyc_RgnOrder_satisfies_constraints(rgn_order,po,Cyc_Absyn_heap_rgn_type,1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp106;_tmp106.tag=0,({struct _fat_ptr _tmp151=({const char*_tmpA8="the required region ordering is not satisfied here";_tag_fat(_tmpA8,sizeof(char),51U);});_tmp106.f1=_tmp151;});_tmp106;});void*_tmpA6[1];_tmpA6[0]=& _tmpA7;Cyc_Warn_err2(loc,_tag_fat(_tmpA6,sizeof(void*),1));});}}}
# 491
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){
return({struct Cyc_Tcenv_SharedFenv*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->return_typ=ret,_tmpAA->delayed_effect_checks=0,_tmpAA->delayed_constraint_checks=0;_tmpAA;});}struct _tuple17{struct Cyc_List_List*f1;void*f2;void*f3;struct Cyc_List_List*f4;};
# 497
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpC8=_cycalloc(sizeof(*_tmpC8));
({struct _fat_ptr*_tmp155=({struct _fat_ptr*_tmpC7=_cycalloc(sizeof(*_tmpC7));({struct _fat_ptr _tmp154=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0,_tmp108.f1=(struct _fat_ptr)((struct _fat_ptr)*(*fd->name).f2);_tmp108;});void*_tmpC4[1];_tmpC4[0]=& _tmpC6;({struct _fat_ptr _tmp153=({const char*_tmpC5="`%s";_tag_fat(_tmpC5,sizeof(char),4U);});Cyc_aprintf(_tmp153,_tag_fat(_tmpC4,sizeof(void*),1));});});*_tmpC7=_tmp154;});_tmpC7;});_tmpC8->name=_tmp155;}),({
int _tmp152=Cyc_Tcutil_new_tvar_id();_tmpC8->identity=_tmp152;}),_tmpC8->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpC8;});
# 502
struct _tuple17 _tmpAB=({struct _tuple17 _tmp107;({struct Cyc_List_List*_tmp15A=({struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*_tmpC1));({struct Cyc_Absyn_Tvar*_tmp159=Cyc_CurRgn_curr_rgn_tvar();_tmpC1->hd=_tmp159;}),_tmpC1->tl=(fd->i).tvars;_tmpC1;});_tmp107.f1=_tmp15A;}),({
void*_tmp158=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpC3=_cycalloc(sizeof(*_tmpC3));({void*_tmp157=Cyc_Absyn_access_eff(Cyc_CurRgn_curr_rgn_type());_tmpC3->hd=_tmp157;}),({
struct Cyc_List_List*_tmp156=({struct Cyc_List_List*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->hd=_check_null((fd->i).effect),_tmpC2->tl=0;_tmpC2;});_tmpC3->tl=_tmp156;});_tmpC3;}));
# 503
_tmp107.f2=_tmp158;}),_tmp107.f3=(fd->i).ret_type,_tmp107.f4=(fd->i).rgn_po;_tmp107;});
# 502
struct _tuple17 _stmttmp5=_tmpAB;struct _tuple17 _tmpAC=_stmttmp5;void*_tmpB0;void*_tmpAF;void*_tmpAE;void*_tmpAD;_tmpAD=_tmpAC.f1;_tmpAE=_tmpAC.f2;_tmpAF=_tmpAC.f3;_tmpB0=_tmpAC.f4;{struct Cyc_List_List*tvs1=_tmpAD;void*cap=_tmpAE;void*ret_t=_tmpAF;struct Cyc_List_List*f_rgn_po=_tmpB0;
# 506
struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->hd=rgn0,_tmpC0->tl=tvs1;_tmpC0;});struct Cyc_List_List*tvs=_tmpB1;
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
struct Cyc_RgnOrder_RgnPO*_tmpB2=Cyc_RgnOrder_initial_fn_po(tvs1,f_rgn_po,cap,rgn0,loc);struct Cyc_RgnOrder_RgnPO*rgn_po=_tmpB2;
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=2,_tmpBF->f1=rgn0;_tmpBF;});
{struct Cyc_List_List*_tmpB3=(struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpB3;for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpB4=*_check_null((fd->i).cyc_varargs);struct Cyc_Absyn_VarargInfo _stmttmp6=_tmpB4;struct Cyc_Absyn_VarargInfo _tmpB5=_stmttmp6;int _tmpB9;void*_tmpB8;struct Cyc_Absyn_Tqual _tmpB7;void*_tmpB6;_tmpB6=_tmpB5.name;_tmpB7=_tmpB5.tq;_tmpB8=_tmpB5.type;_tmpB9=_tmpB5.inject;{struct _fat_ptr*n=_tmpB6;struct Cyc_Absyn_Tqual tq=_tmpB7;void*t=_tmpB8;int i=_tmpB9;
# 515
struct Cyc_List_List*_tmpBA=(struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpBA;for(0;vds != 0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*_tmpBB=(struct Cyc_Absyn_Vardecl*)vds->hd;struct Cyc_Absyn_Vardecl*vd=_tmpBB;
if(Cyc_strptrcmp((*vd->name).f2,_check_null(n))== 0){
({void*_tmp15B=Cyc_Absyn_fatptr_type(t,param_rgn,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);vd->type=_tmp15B;});
break;}}}}
# 523
return({struct Cyc_Tcenv_Fenv*_tmpBE=_cycalloc(sizeof(*_tmpBE));
({struct Cyc_Tcenv_SharedFenv*_tmp160=Cyc_Tcenv_new_shared_fenv(ret_t);_tmpBE->shared=_tmp160;}),_tmpBE->type_vars=tvs,_tmpBE->region_order=rgn_po,_tmpBE->ctrl_env=0,({
# 528
void*_tmp15F=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*_tmpBD));({void*_tmp15E=Cyc_Absyn_access_eff(param_rgn);_tmpBD->hd=_tmp15E;}),({
struct Cyc_List_List*_tmp15D=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=cap,_tmpBC->tl=0;_tmpBC;});_tmpBD->tl=_tmp15D;});_tmpBD;}));
# 528
_tmpBE->capability=_tmp15F;}),_tmpBE->curr_rgn=param_rgn,({
# 531
void*_tmp15C=Cyc_CurRgn_curr_rgn_type();_tmpBE->curr_lifo_rgn=_tmp15C;}),
(_tmpBE->flags).in_new=0U,(_tmpBE->flags).in_notreadctxt=0,(_tmpBE->flags).in_lhs=0,(_tmpBE->flags).abstract_ok=0,(_tmpBE->flags).in_stmt_exp=0;_tmpBE;});}}}
# 536
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmpC9=old_fenv;void*_tmpCC;void*_tmpCB;void*_tmpCA;_tmpCA=_tmpC9->region_order;_tmpCB=_tmpC9->type_vars;_tmpCC=_tmpC9->shared;{struct Cyc_RgnOrder_RgnPO*rgn_po=_tmpCA;struct Cyc_List_List*type_vars=_tmpCB;struct Cyc_Tcenv_SharedFenv*shared=_tmpCC;
# 539
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpE9=_cycalloc(sizeof(*_tmpE9));
({struct _fat_ptr*_tmp164=({struct _fat_ptr*_tmpE8=_cycalloc(sizeof(*_tmpE8));({struct _fat_ptr _tmp163=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE7=({struct Cyc_String_pa_PrintArg_struct _tmp10A;_tmp10A.tag=0,_tmp10A.f1=(struct _fat_ptr)((struct _fat_ptr)*(*fd->name).f2);_tmp10A;});void*_tmpE5[1];_tmpE5[0]=& _tmpE7;({struct _fat_ptr _tmp162=({const char*_tmpE6="`%s";_tag_fat(_tmpE6,sizeof(char),4U);});Cyc_aprintf(_tmp162,_tag_fat(_tmpE5,sizeof(void*),1));});});*_tmpE8=_tmp163;});_tmpE8;});_tmpE9->name=_tmp164;}),({
int _tmp161=Cyc_Tcutil_new_tvar_id();_tmpE9->identity=_tmp161;}),_tmpE9->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpE9;});
{struct Cyc_List_List*_tmpCD=(fd->i).tvars;struct Cyc_List_List*tvars=_tmpCD;for(0;tvars != 0;tvars=tvars->tl){
struct Cyc_Absyn_Kind*_tmpCE=Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp7=_tmpCE;struct Cyc_Absyn_Kind*_tmpCF=_stmttmp7;enum Cyc_Absyn_AliasQual _tmpD1;enum Cyc_Absyn_KindQual _tmpD0;_tmpD0=_tmpCF->kind;_tmpD1=_tmpCF->aliasqual;{enum Cyc_Absyn_KindQual k=_tmpD0;enum Cyc_Absyn_AliasQual a=_tmpD1;
if((int)k == 3){
if((int)a == 0)
rgn_po=Cyc_RgnOrder_add_unordered(rgn_po,(struct Cyc_Absyn_Tvar*)tvars->hd);else{
# 548
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp109;_tmp109.tag=0,({struct _fat_ptr _tmp165=({const char*_tmpD4="non-intuitionistic tvar in nested_fenv";_tag_fat(_tmpD4,sizeof(char),39U);});_tmp109.f1=_tmp165;});_tmp109;});void*_tmpD2[1];_tmpD2[0]=& _tmpD3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmpD2,sizeof(void*),1));});}}}}}
# 550
rgn_po=Cyc_RgnOrder_add_youngest(rgn_po,rgn0,0);
{struct Cyc_List_List*_tmpD5=(fd->i).rgn_po;struct Cyc_List_List*po2=_tmpD5;for(0;po2 != 0;po2=po2->tl){
rgn_po=Cyc_RgnOrder_add_outlives_constraint(rgn_po,(*((struct _tuple14*)po2->hd)).f1,(*((struct _tuple14*)po2->hd)).f2,loc);}}{
struct Cyc_List_List*_tmpD6=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->hd=rgn0,({struct Cyc_List_List*_tmp166=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((fd->i).tvars,type_vars);_tmpE4->tl=_tmp166;});_tmpE4;});struct Cyc_List_List*tvs=_tmpD6;
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->tag=2,_tmpE3->f1=rgn0;_tmpE3;});
{struct Cyc_List_List*_tmpD7=(struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpD7;for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpD8=*_check_null((fd->i).cyc_varargs);struct Cyc_Absyn_VarargInfo _stmttmp8=_tmpD8;struct Cyc_Absyn_VarargInfo _tmpD9=_stmttmp8;int _tmpDD;void*_tmpDC;struct Cyc_Absyn_Tqual _tmpDB;void*_tmpDA;_tmpDA=_tmpD9.name;_tmpDB=_tmpD9.tq;_tmpDC=_tmpD9.type;_tmpDD=_tmpD9.inject;{struct _fat_ptr*n=_tmpDA;struct Cyc_Absyn_Tqual tq=_tmpDB;void*t=_tmpDC;int i=_tmpDD;
# 561
struct Cyc_List_List*_tmpDE=(struct Cyc_List_List*)(_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpDE;for(0;vds != 0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*_tmpDF=(struct Cyc_Absyn_Vardecl*)vds->hd;struct Cyc_Absyn_Vardecl*vd=_tmpDF;
if(Cyc_strptrcmp((*vd->name).f2,_check_null(n))== 0){
({void*_tmp167=Cyc_Absyn_fatptr_type(t,param_rgn,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);vd->type=_tmp167;});
break;}}}}
# 569
return({struct Cyc_Tcenv_Fenv*_tmpE2=_cycalloc(sizeof(*_tmpE2));
({struct Cyc_Tcenv_SharedFenv*_tmp16C=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpE2->shared=_tmp16C;}),_tmpE2->type_vars=tvs,_tmpE2->region_order=rgn_po,_tmpE2->ctrl_env=0,({
# 574
void*_tmp16B=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));({void*_tmp16A=Cyc_Absyn_access_eff(param_rgn);_tmpE1->hd=_tmp16A;}),({
struct Cyc_List_List*_tmp169=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=_check_null((fd->i).effect),_tmpE0->tl=0;_tmpE0;});_tmpE1->tl=_tmp169;});_tmpE1;}));
# 574
_tmpE2->capability=_tmp16B;}),_tmpE2->curr_rgn=param_rgn,({
# 577
void*_tmp168=Cyc_CurRgn_curr_rgn_type();_tmpE2->curr_lifo_rgn=_tmp168;}),
(_tmpE2->flags).in_new=0U,(_tmpE2->flags).in_notreadctxt=0,(_tmpE2->flags).in_lhs=0,(_tmpE2->flags).abstract_ok=0,(_tmpE2->flags).in_stmt_exp=0;_tmpE2;});}}}}
# 582
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){
# 584
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct _fat_ptr*_tmp16F=({struct _fat_ptr*_tmpF1=_cycalloc(sizeof(*_tmpF1));({struct _fat_ptr _tmp16E=({const char*_tmpF0="bogus";_tag_fat(_tmpF0,sizeof(char),6U);});*_tmpF1=_tmp16E;});_tmpF1;});_tmpF2->name=_tmp16F;}),({int _tmp16D=Cyc_Tcutil_new_tvar_id();_tmpF2->identity=_tmp16D;}),_tmpF2->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpF2;});
struct Cyc_List_List*_tmpEA=({struct Cyc_List_List*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->hd=rgn0,_tmpEF->tl=0;_tmpEF;});struct Cyc_List_List*tvs=_tmpEA;
struct Cyc_RgnOrder_RgnPO*_tmpEB=({void*_tmp170=Cyc_Absyn_join_eff(0);Cyc_RgnOrder_initial_fn_po(0,0,_tmp170,rgn0,0U);});struct Cyc_RgnOrder_RgnPO*rgn_po=_tmpEB;
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=2,_tmpEE->f1=rgn0;_tmpEE;});
return({struct Cyc_Tcenv_Fenv*_tmpED=_cycalloc(sizeof(*_tmpED));
({struct Cyc_Tcenv_SharedFenv*_tmp174=Cyc_Tcenv_new_shared_fenv(ret_type);_tmpED->shared=_tmp174;}),_tmpED->type_vars=tvs,_tmpED->region_order=rgn_po,_tmpED->ctrl_env=0,({
# 593
void*_tmp173=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpEC=_cycalloc(sizeof(*_tmpEC));({void*_tmp172=Cyc_Absyn_access_eff(param_rgn);_tmpEC->hd=_tmp172;}),_tmpEC->tl=0;_tmpEC;}));_tmpED->capability=_tmp173;}),_tmpED->curr_rgn=param_rgn,({
# 595
void*_tmp171=Cyc_CurRgn_curr_rgn_type();_tmpED->curr_lifo_rgn=_tmp171;}),
(_tmpED->flags).in_new=0U,(_tmpED->flags).in_notreadctxt=0,(_tmpED->flags).in_lhs=0,(_tmpED->flags).abstract_ok=0,(_tmpED->flags).in_stmt_exp=0;_tmpED;});}
# 600
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpF3=({struct Cyc_Tcenv_Tenv*_tmpF5=_cycalloc(sizeof(*_tmpF5));*_tmpF5=*te;_tmpF5;});struct Cyc_Tcenv_Tenv*ans=_tmpF3;
({struct Cyc_Tcenv_Genv*_tmp175=({struct Cyc_Tcenv_Genv*_tmpF4=_cycalloc(sizeof(*_tmpF4));*_tmpF4=*te->ae;_tmpF4;});ans->ae=_tmp175;});
({struct Cyc_Dict_Dict _tmp176=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy;})((te->ae)->aggrdecls);(ans->ae)->aggrdecls=_tmp176;});
({struct Cyc_Dict_Dict _tmp177=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy;})((te->ae)->enumdecls);(ans->ae)->enumdecls=_tmp177;});
({struct Cyc_Dict_Dict _tmp178=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy;})((te->ae)->datatypedecls);(ans->ae)->datatypedecls=_tmp178;});
({struct Cyc_Dict_Dict _tmp179=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy;})((te->ae)->typedefs);(ans->ae)->typedefs=_tmp179;});
({struct Cyc_Dict_Dict _tmp17A=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy;})((te->ae)->ordinaries);(ans->ae)->ordinaries=_tmp17A;});
ans->le=0;
return ans;}
