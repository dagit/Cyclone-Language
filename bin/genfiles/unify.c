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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];
# 190 "list.h"
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 46 "position.h"
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 172
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 863 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 894
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 923
void*Cyc_Absyn_bounds_one (void);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 131 "relations-ap.h"
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*,struct Cyc_List_List*);
# 100 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 140
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 197
void*Cyc_Tcutil_normalize_effect(void*);
# 246
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 250
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_tycon_cmp(void*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 71
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 54 "attributes.h"
int Cyc_Atts_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 58
int Cyc_Atts_equiv_fn_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*,struct _RegionHandle*,struct Cyc_List_List*,void*);char Cyc_Unify_Unify[6U]="Unify";struct Cyc_Unify_Unify_exn_struct{char*tag;};
# 35 "unify.cyc"
struct Cyc_Unify_Unify_exn_struct Cyc_Unify_Unify_val={Cyc_Unify_Unify};struct _tuple11{void*f1;void*f2;};
# 38
static struct _tuple11 Cyc_Unify_ts_failure={0,0};struct _tuple12{int f1;int f2;};
static struct _tuple12 Cyc_Unify_tqs_const={0,0};
static struct _fat_ptr Cyc_Unify_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 42
static void Cyc_Unify_fail_because(struct _fat_ptr reason){
Cyc_Unify_failure_reason=reason;
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 50
void Cyc_Unify_explain_failure (void){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;
Cyc_fflush(Cyc_stderr);
# 56
if(({struct _fat_ptr _tmpE4=({const char*_tmp0="(qualifiers don't match)";_tag_fat(_tmp0,sizeof(char),25U);});Cyc_strcmp(_tmpE4,(struct _fat_ptr)Cyc_Unify_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmpCA;_tmpCA.tag=0,_tmpCA.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmpCA;});void*_tmp1[1];_tmp1[0]=& _tmp3;({struct Cyc___cycFILE*_tmpE6=Cyc_stderr;struct _fat_ptr _tmpE5=({const char*_tmp2="  %s\n";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_fprintf(_tmpE6,_tmpE5,_tag_fat(_tmp1,sizeof(void*),1));});});
return;}
# 61
if(({struct _fat_ptr _tmpE7=({const char*_tmp4="(function effects do not match)";_tag_fat(_tmp4,sizeof(char),32U);});Cyc_strcmp(_tmpE7,(struct _fat_ptr)Cyc_Unify_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;struct Cyc_Absynpp_Params p=_tmp5;
p.print_all_effects=1;
Cyc_Absynpp_set_params(& p);}{
# 66
struct _tuple11 _tmp6=Cyc_Unify_ts_failure;void*_tmp8;void*_tmp7;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;{void*t1f=_tmp7;void*t2f=_tmp8;
struct _fat_ptr s1=(unsigned)t1f?Cyc_Absynpp_typ2string(t1f):({const char*_tmp23="<?>";_tag_fat(_tmp23,sizeof(char),4U);});
struct _fat_ptr s2=(unsigned)t2f?Cyc_Absynpp_typ2string(t2f):({const char*_tmp22="<?>";_tag_fat(_tmp22,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmpCB;_tmpCB.tag=0,_tmpCB.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmpCB;});void*_tmp9[1];_tmp9[0]=& _tmpB;({struct Cyc___cycFILE*_tmpE9=Cyc_stderr;struct _fat_ptr _tmpE8=({const char*_tmpA="  %s";_tag_fat(_tmpA,sizeof(char),5U);});Cyc_fprintf(_tmpE9,_tmpE8,_tag_fat(_tmp9,sizeof(void*),1));});});
pos +=_get_fat_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpC=0U;({struct Cyc___cycFILE*_tmpEB=Cyc_stderr;struct _fat_ptr _tmpEA=({const char*_tmpD="\n\t";_tag_fat(_tmpD,sizeof(char),3U);});Cyc_fprintf(_tmpEB,_tmpEA,_tag_fat(_tmpC,sizeof(void*),0));});});
pos=8;}else{
# 76
({void*_tmpE=0U;({struct Cyc___cycFILE*_tmpED=Cyc_stderr;struct _fat_ptr _tmpEC=({const char*_tmpF=" ";_tag_fat(_tmpF,sizeof(char),2U);});Cyc_fprintf(_tmpED,_tmpEC,_tag_fat(_tmpE,sizeof(void*),0));});});
++ pos;}
# 79
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmpEF=Cyc_stderr;struct _fat_ptr _tmpEE=({const char*_tmp11="and ";_tag_fat(_tmp11,sizeof(char),5U);});Cyc_fprintf(_tmpEF,_tmpEE,_tag_fat(_tmp10,sizeof(void*),0));});});
pos +=4;
if((unsigned)pos + _get_fat_size(s2,sizeof(char))>= 80U){
({void*_tmp12=0U;({struct Cyc___cycFILE*_tmpF1=Cyc_stderr;struct _fat_ptr _tmpF0=({const char*_tmp13="\n\t";_tag_fat(_tmp13,sizeof(char),3U);});Cyc_fprintf(_tmpF1,_tmpF0,_tag_fat(_tmp12,sizeof(void*),0));});});
pos=8;}
# 85
({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmpCC;_tmpCC.tag=0,_tmpCC.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmpCC;});void*_tmp14[1];_tmp14[0]=& _tmp16;({struct Cyc___cycFILE*_tmpF3=Cyc_stderr;struct _fat_ptr _tmpF2=({const char*_tmp15="%s ";_tag_fat(_tmp15,sizeof(char),4U);});Cyc_fprintf(_tmpF3,_tmpF2,_tag_fat(_tmp14,sizeof(void*),1));});});
pos +=_get_fat_size(s2,sizeof(char))+ 1U;
if(pos + 17 >= 80){
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmpF5=Cyc_stderr;struct _fat_ptr _tmpF4=({const char*_tmp18="\n\t";_tag_fat(_tmp18,sizeof(char),3U);});Cyc_fprintf(_tmpF5,_tmpF4,_tag_fat(_tmp17,sizeof(void*),0));});});
pos=8;}
# 91
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmpF7=Cyc_stderr;struct _fat_ptr _tmpF6=({const char*_tmp1A="are not compatible. ";_tag_fat(_tmp1A,sizeof(char),21U);});Cyc_fprintf(_tmpF7,_tmpF6,_tag_fat(_tmp19,sizeof(void*),0));});});
pos +=17;
if((char*)Cyc_Unify_failure_reason.curr != (char*)(_tag_fat(0,0,0)).curr){
if(({unsigned long _tmpF8=(unsigned long)pos;_tmpF8 + Cyc_strlen((struct _fat_ptr)Cyc_Unify_failure_reason);})>= 80U)
({void*_tmp1B=0U;({struct Cyc___cycFILE*_tmpFA=Cyc_stderr;struct _fat_ptr _tmpF9=({const char*_tmp1C="\n\t";_tag_fat(_tmp1C,sizeof(char),3U);});Cyc_fprintf(_tmpFA,_tmpF9,_tag_fat(_tmp1B,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0,_tmpCD.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmpCD;});void*_tmp1D[1];_tmp1D[0]=& _tmp1F;({struct Cyc___cycFILE*_tmpFC=Cyc_stderr;struct _fat_ptr _tmpFB=({const char*_tmp1E="%s";_tag_fat(_tmp1E,sizeof(char),3U);});Cyc_fprintf(_tmpFC,_tmpFB,_tag_fat(_tmp1D,sizeof(void*),1));});});}
# 98
({void*_tmp20=0U;({struct Cyc___cycFILE*_tmpFE=Cyc_stderr;struct _fat_ptr _tmpFD=({const char*_tmp21="\n";_tag_fat(_tmp21,sizeof(char),2U);});Cyc_fprintf(_tmpFE,_tmpFD,_tag_fat(_tmp20,sizeof(void*),0));});});
Cyc_fflush(Cyc_stderr);}}}
# 104
static int Cyc_Unify_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
# 106
if(r1 == r2)return 1;
return Cyc_Relations_check_logical_implication(r1,r2)&&
 Cyc_Relations_check_logical_implication(r2,r1);}
# 111
int Cyc_Unify_unify_kindbound(void*kb1,void*kb2){
struct _tuple11 _tmp24=({struct _tuple11 _tmpCE;({void*_tmp100=Cyc_Kinds_compress_kb(kb1);_tmpCE.f1=_tmp100;}),({void*_tmpFF=Cyc_Kinds_compress_kb(kb2);_tmpCE.f2=_tmpFF;});_tmpCE;});struct _tuple11 _stmttmp0=_tmp24;struct _tuple11 _tmp25=_stmttmp0;void*_tmp29;void*_tmp28;void*_tmp27;void*_tmp26;switch(*((int*)_tmp25.f1)){case 0: switch(*((int*)_tmp25.f2)){case 0: _tmp26=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp27=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;{struct Cyc_Absyn_Kind*k1=_tmp26;struct Cyc_Absyn_Kind*k2=_tmp27;
return k1 == k2;}case 2: _tmp26=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp27=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_tmp28=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f2;_LL8: {struct Cyc_Absyn_Kind*k1=_tmp26;struct Cyc_Core_Opt**x=_tmp27;struct Cyc_Absyn_Kind*k2=_tmp28;
# 122
if(!Cyc_Kinds_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_tmp101=({struct Cyc_Core_Opt*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->v=kb1;_tmp2B;});*x=_tmp101;});
return 1;}default: goto _LLB;}case 2: switch(*((int*)_tmp25.f2)){case 0:
# 114
 return Cyc_Unify_unify_kindbound(kb2,kb1);case 2: _tmp26=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp27=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f2;_tmp28=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_tmp29=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f2;{struct Cyc_Core_Opt**y=_tmp26;struct Cyc_Absyn_Kind*k1=_tmp27;struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_tmp28;struct Cyc_Absyn_Kind*k2=_tmp29;
# 116
if(Cyc_Kinds_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp102=({struct Cyc_Core_Opt*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->v=kb2;_tmp2A;});*y=_tmp102;});
return 1;}
# 120
_tmp26=k1;_tmp27=x;_tmp28=k2;goto _LL8;}default: _LLB: _tmp26=_tmp25.f1;_tmp27=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LLC: {void*y=_tmp26;struct Cyc_Core_Opt**x=_tmp27;
# 128
({struct Cyc_Core_Opt*_tmp103=({struct Cyc_Core_Opt*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->v=y;_tmp2C;});*x=_tmp103;});
return 1;}}default: _tmp26=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp27=_tmp25.f2;{struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_tmp26;void*y=_tmp27;
# 126
_tmp26=y;_tmp27=x;goto _LLC;}};}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 135
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Absyn_compress(t);{
void*_tmp2D=t;void*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;struct Cyc_Absyn_Tqual _tmp30;struct Cyc_Absyn_PtrInfo _tmp35;void*_tmp2F;void*_tmp2E;switch(*((int*)_tmp2D)){case 2: _tmp2E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp2E;
# 139
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,env,tv))
Cyc_Unify_fail_because(({const char*_tmp36="(type variable would escape scope)";_tag_fat(_tmp36,sizeof(char),35U);}));
goto _LL0;}case 1: _tmp2E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D)->f2;_tmp2F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D)->f4;{void*rg=_tmp2E;struct Cyc_Core_Opt**sopt=_tmp2F;
# 143
if(t == evar)
Cyc_Unify_fail_because(({const char*_tmp37="(occurs check)";_tag_fat(_tmp37,sizeof(char),15U);}));
if(rg != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCF;_tmpCF.tag=0,({struct _fat_ptr _tmp104=({const char*_tmp3A="occurs check: constrained Evar in compressed type";_tag_fat(_tmp3A,sizeof(char),50U);});_tmpCF.f1=_tmp104;});_tmpCF;});void*_tmp38[1];_tmp38[0]=& _tmp39;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp38,sizeof(void*),1));});{
# 148
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)(_check_null(*sopt))->v;for(0;s != 0 && !problem;s=s->tl){
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
problem=1;}}
# 153
if(problem){
struct Cyc_List_List*_tmp3B=0;struct Cyc_List_List*result=_tmp3B;
{struct Cyc_List_List*s=(struct Cyc_List_List*)(_check_null(*sopt))->v;for(0;s != 0;s=s->tl){
if(({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
result=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp3C->tl=result;_tmp3C;});}}
({struct Cyc_Core_Opt*_tmp105=({struct Cyc_Core_Opt*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->v=result;_tmp3D;});*sopt=_tmp105;});}
# 160
goto _LL0;}}case 3: _tmp35=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1;{struct Cyc_Absyn_PtrInfo pinfo=_tmp35;
# 162
Cyc_Unify_occurs(evar,r,env,pinfo.elt_type);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).rgn);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).nullable);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).bounds);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).zero_term);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).autoreleased);
goto _LL0;}case 4: _tmp2E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp2F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;{void*t2=_tmp2E;void*zt=_tmp2F;
# 171
Cyc_Unify_occurs(evar,r,env,t2);
Cyc_Unify_occurs(evar,r,env,zt);
goto _LL0;}case 5: _tmp2E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).tvars;_tmp2F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).effect;_tmp30=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).ret_tqual;_tmp31=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).ret_type;_tmp32=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).args;_tmp33=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).cyc_varargs;_tmp34=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D)->f1).rgn_po;{struct Cyc_List_List*tvs=_tmp2E;void*eff=_tmp2F;struct Cyc_Absyn_Tqual rt_tq=_tmp30;void*rt=_tmp31;struct Cyc_List_List*args=_tmp32;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp33;struct Cyc_List_List*rgn_po=_tmp34;
# 176
env=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rappend;})(r,tvs,env);
if(eff != 0)
Cyc_Unify_occurs(evar,r,env,eff);
Cyc_Unify_occurs(evar,r,env,rt);
for(1;args != 0;args=args->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple8*)args->hd)).f3);}
if(cyc_varargs != 0)
Cyc_Unify_occurs(evar,r,env,cyc_varargs->type);
for(1;rgn_po != 0;rgn_po=rgn_po->tl){
struct _tuple11*_tmp3E=(struct _tuple11*)rgn_po->hd;struct _tuple11*_stmttmp1=_tmp3E;struct _tuple11*_tmp3F=_stmttmp1;void*_tmp41;void*_tmp40;_tmp40=_tmp3F->f1;_tmp41=_tmp3F->f2;{void*r1=_tmp40;void*r2=_tmp41;
Cyc_Unify_occurs(evar,r,env,r1);
Cyc_Unify_occurs(evar,r,env,r2);}}
# 189
goto _LL0;}case 6: _tmp2E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D)->f1;{struct Cyc_List_List*args=_tmp2E;
# 191
for(1;args != 0;args=args->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple13*)args->hd)).f2);}
goto _LL0;}case 7: _tmp2E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D)->f2;{struct Cyc_List_List*fs=_tmp2E;
# 196
for(1;fs != 0;fs=fs->tl){
Cyc_Unify_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);}
goto _LL0;}case 8: _tmp2E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D)->f2;{struct Cyc_List_List*ts=_tmp2E;
_tmp2E=ts;goto _LL12;}case 0: _tmp2E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D)->f2;_LL12: {struct Cyc_List_List*ts=_tmp2E;
# 201
for(1;ts != 0;ts=ts->tl){
Cyc_Unify_occurs(evar,r,env,(void*)ts->hd);}
goto _LL0;}default:
# 206
 goto _LL0;}_LL0:;}}
# 210
static void Cyc_Unify_unify_it(void*,void*);
# 213
int Cyc_Unify_unify(void*t1,void*t2){
struct _handler_cons _tmp42;_push_handler(& _tmp42);{int _tmp44=0;if(setjmp(_tmp42.handler))_tmp44=1;if(!_tmp44){
Cyc_Unify_unify_it(t1,t2);{
int _tmp45=1;_npop_handler(0);return _tmp45;}
# 215
;_pop_handler();}else{void*_tmp43=(void*)Cyc_Core_get_exn_thrown();void*_tmp46=_tmp43;void*_tmp47;if(((struct Cyc_Unify_Unify_exn_struct*)_tmp46)->tag == Cyc_Unify_Unify)
# 223
return 0;else{_tmp47=_tmp46;{void*exn=_tmp47;(int)_rethrow(exn);}};}}}
# 228
static void Cyc_Unify_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(1;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Unify_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 236
static void Cyc_Unify_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const && !tq1.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD0;_tmpD0.tag=0,({struct _fat_ptr _tmp106=({const char*_tmp4A="tq1 real_const not set.";_tag_fat(_tmp4A,sizeof(char),24U);});_tmpD0.f1=_tmp106;});_tmpD0;});void*_tmp48[1];_tmp48[0]=& _tmp49;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp48,sizeof(void*),1));});
if(tq2.print_const && !tq2.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD1;_tmpD1.tag=0,({struct _fat_ptr _tmp107=({const char*_tmp4D="tq2 real_const not set.";_tag_fat(_tmp4D,sizeof(char),24U);});_tmpD1.f1=_tmp107;});_tmpD1;});void*_tmp4B[1];_tmp4B[0]=& _tmp4C;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp4B,sizeof(void*),1));});
# 242
if((tq1.real_const != tq2.real_const || tq1.q_volatile != tq2.q_volatile)|| tq1.q_restrict != tq2.q_restrict){
# 245
Cyc_Unify_ts_failure=({struct _tuple11 _tmpD2;_tmpD2.f1=t1,_tmpD2.f2=t2;_tmpD2;});
Cyc_Unify_tqs_const=({struct _tuple12 _tmpD3;_tmpD3.f1=tq1.real_const,_tmpD3.f2=tq2.real_const;_tmpD3;});
Cyc_Unify_failure_reason=({const char*_tmp4E="(qualifiers don't match)";_tag_fat(_tmp4E,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 251
Cyc_Unify_tqs_const=({struct _tuple12 _tmpD4;_tmpD4.f1=0,_tmpD4.f2=0;_tmpD4;});}
# 265 "unify.cyc"
static int Cyc_Unify_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2)&& Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2)&& Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 275
static int Cyc_Unify_unify_const_exp_opt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
if(e1 == 0 && e2 == 0)
return 1;
if(e1 == 0 || e2 == 0)
return 0;
return Cyc_Evexp_same_uint_const_exp(e1,e2);}struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple15{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 284
static void Cyc_Unify_unify_it(void*t1,void*t2){
Cyc_Unify_ts_failure=({struct _tuple11 _tmpD5;_tmpD5.f1=t1,_tmpD5.f2=t2;_tmpD5;});
Cyc_Unify_failure_reason=_tag_fat(0,0,0);
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return;{
struct _tuple11 _tmp4F=({struct _tuple11 _tmpE3;_tmpE3.f1=t2,_tmpE3.f2=t1;_tmpE3;});struct _tuple11 _stmttmp2=_tmp4F;struct _tuple11 _tmp50=_stmttmp2;enum Cyc_Absyn_AggrKind _tmp56;enum Cyc_Absyn_AggrKind _tmp55;void*_tmp6E;void*_tmp6D;void*_tmp6C;void*_tmp6B;void*_tmp6A;void*_tmp69;void*_tmp68;void*_tmp67;int _tmp66;void*_tmp65;void*_tmp64;void*_tmp62;void*_tmp61;int _tmp58;void*_tmp60;void*_tmp5F;void*_tmp5E;void*_tmp5D;void*_tmp5C;struct Cyc_Absyn_Tqual _tmp63;void*_tmp5B;void*_tmp5A;struct Cyc_Absyn_Tqual _tmp57;void*_tmp59;void*_tmp54;void*_tmp53;void*_tmp52;void*_tmp51;if(*((int*)_tmp50.f1)== 1){if(*((int*)_tmp50.f2)== 1){_tmp51=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f1)->f1;_tmp52=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f1)->f2;_tmp53=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f1)->f4;_tmp54=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f2)->f1;_tmp59=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f2)->f4;{struct Cyc_Core_Opt**kind1opt=_tmp51;void**t2r=_tmp52;struct Cyc_Core_Opt**s1opt=_tmp53;struct Cyc_Core_Opt**kind2opt=_tmp54;struct Cyc_Core_Opt**s2opt=_tmp59;
# 292
if(({struct Cyc_Absyn_Kind*_tmp108=(struct Cyc_Absyn_Kind*)(_check_null(*kind1opt))->v;Cyc_Kinds_kind_leq(_tmp108,(struct Cyc_Absyn_Kind*)(_check_null(*kind2opt))->v);}))
*kind2opt=*kind1opt;else{
if(({struct Cyc_Absyn_Kind*_tmp109=(struct Cyc_Absyn_Kind*)(_check_null(*kind2opt))->v;Cyc_Kinds_kind_leq(_tmp109,(struct Cyc_Absyn_Kind*)(_check_null(*kind1opt))->v);}))
*kind1opt=*kind2opt;else{
# 297
Cyc_Unify_fail_because(({const char*_tmp6F="(kinds are incompatible)";_tag_fat(_tmp6F,sizeof(char),25U);}));}}
*t2r=t1;{
# 300
struct Cyc_List_List*_tmp70=(struct Cyc_List_List*)(_check_null(*s1opt))->v;struct Cyc_List_List*s1=_tmp70;
struct Cyc_List_List*_tmp71=(struct Cyc_List_List*)(_check_null(*s2opt))->v;struct Cyc_List_List*s2=_tmp71;
if(s1 == s2)return;
for(1;s1 != 0;s1=s1->tl){
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,s2,(struct Cyc_Absyn_Tvar*)s1->hd))
break;}
if(s1 == 0){
*s2opt=*s1opt;
return;}
# 310
s1=(struct Cyc_List_List*)(_check_null(*s1opt))->v;
for(1;s2 != 0;s2=s2->tl){
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,s1,(struct Cyc_Absyn_Tvar*)s2->hd))
break;}
if(s2 == 0){
*s1opt=*s2opt;
return;}
# 318
s2=(struct Cyc_List_List*)(_check_null(*s2opt))->v;{
struct Cyc_List_List*_tmp72=0;struct Cyc_List_List*ans=_tmp72;
for(1;s2 != 0;s2=s2->tl){
if(({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,s1,(struct Cyc_Absyn_Tvar*)s2->hd))
ans=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=(struct Cyc_Absyn_Tvar*)s2->hd,_tmp73->tl=ans;_tmp73;});}
({struct Cyc_Core_Opt*_tmp10B=({struct Cyc_Core_Opt*_tmp10A=({struct Cyc_Core_Opt*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->v=ans;_tmp74;});*s2opt=_tmp10A;});*s1opt=_tmp10B;});
return;}}}}else{
# 354
Cyc_Unify_unify_it(t2,t1);return;}}else{if(*((int*)_tmp50.f2)== 1){_tmp51=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f2)->f1;_tmp52=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f2)->f2;_tmp53=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp50.f2)->f4;{struct Cyc_Core_Opt*kind1=_tmp51;void**ref1_ref=_tmp52;struct Cyc_Core_Opt*s1opt=_tmp53;
# 329
Cyc_Unify_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)(_check_null(s1opt))->v,t2);{
struct Cyc_Absyn_Kind*_tmp75=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*kind2=_tmp75;
# 333
if(Cyc_Kinds_kind_leq(kind2,(struct Cyc_Absyn_Kind*)(_check_null(kind1))->v)){
*ref1_ref=t2;
return;}{
# 340
void*_tmp76=t2;struct Cyc_Absyn_PtrInfo _tmp77;if(*((int*)_tmp76)== 3){_tmp77=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76)->f1;if((int)((struct Cyc_Absyn_Kind*)kind1->v)->kind == 2){struct Cyc_Absyn_PtrInfo pinfo=_tmp77;
# 342
void*_tmp78=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);void*c=_tmp78;
void*_tmp79=c;if(*((int*)_tmp79)== 1){
# 345
({void*_tmp10C=c;Cyc_Unify_unify(_tmp10C,Cyc_Absyn_bounds_one());});
*ref1_ref=t2;
return;}else{
Cyc_Unify_fail_because(({const char*_tmp7A="(kinds are incompatible)";_tag_fat(_tmp7A,sizeof(char),25U);}));};}else{goto _LL2A;}}else{_LL2A:
# 350
 Cyc_Unify_fail_because(({const char*_tmp7B="(kinds are incompatible)";_tag_fat(_tmp7B,sizeof(char),25U);}));};}}}}else{if(*((int*)_tmp50.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f1)->f1)== 9)
# 356
goto _LLA;else{if(*((int*)_tmp50.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f2)->f1)== 9)goto _LL9;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f1)->f1)== 8)goto _LLB;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f2)->f1)== 8)goto _LLD;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f1)->f1)== 10)goto _LLF;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f2)->f1)== 10)goto _LL11;else{_tmp51=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f1)->f1;_tmp52=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f1)->f2;_tmp53=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f2)->f1;_tmp54=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f2)->f2;{void*c1=_tmp51;struct Cyc_List_List*ts1=_tmp52;void*c2=_tmp53;struct Cyc_List_List*ts2=_tmp54;
# 367
if(Cyc_Tcutil_tycon_cmp(c1,c2)!= 0)
Cyc_Unify_fail_because(({const char*_tmp7D="(different type constructors)";_tag_fat(_tmp7D,sizeof(char),30U);}));
Cyc_Unify_unify_list(ts1,ts2);
return;}}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f1)->f1)){case 8: _LLB: _LLC:
# 358
 goto _LLE;case 10: _LLF: _LL10:
# 360
 goto _LL12;default: goto _LL25;}}}}else{if(*((int*)_tmp50.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50.f2)->f1)){case 9: _LL9: _LLA:
# 357
 goto _LLC;case 8: _LLD: _LLE:
# 359
 goto _LL10;case 10: _LL11: _LL12:
# 362
 if(!Cyc_Unify_unify_effect(t1,t2))
Cyc_Unify_fail_because(({const char*_tmp7C="(effects don't unify)";_tag_fat(_tmp7C,sizeof(char),22U);}));
return;default: switch(*((int*)_tmp50.f1)){case 2: goto _LL25;case 3: goto _LL25;case 9: goto _LL25;case 4: goto _LL25;case 5: goto _LL25;case 6: goto _LL25;case 7: goto _LL25;case 8: goto _LL25;default: goto _LL25;}}else{switch(*((int*)_tmp50.f1)){case 2: if(*((int*)_tmp50.f2)== 2){_tmp51=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp50.f1)->f1;_tmp52=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp50.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_tmp51;struct Cyc_Absyn_Tvar*tv1=_tmp52;
# 374
if(tv2->identity != tv1->identity)
Cyc_Unify_fail_because(({const char*_tmp7E="(variable types are not the same)";_tag_fat(_tmp7E,sizeof(char),34U);}));
return;}}else{goto _LL25;}case 3: if(*((int*)_tmp50.f2)== 3){_tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).elt_tq;_tmp52=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).ptr_atts).rgn;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).ptr_atts).nullable;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).ptr_atts).bounds;_tmp59=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).ptr_atts).zero_term;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f1)->f1).ptr_atts).autoreleased;_tmp5B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).elt_type;_tmp63=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).elt_tq;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).ptr_atts).rgn;_tmp5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).ptr_atts).nullable;_tmp5E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).ptr_atts).bounds;_tmp5F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).ptr_atts).zero_term;_tmp60=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50.f2)->f1).ptr_atts).autoreleased;{void*t2a=_tmp51;struct Cyc_Absyn_Tqual tqual2a=_tmp57;void*rgn2=_tmp52;void*null2a=_tmp53;void*b2=_tmp54;void*zt2=_tmp59;void*rel2=_tmp5A;void*t1a=_tmp5B;struct Cyc_Absyn_Tqual tqual1a=_tmp63;void*rgn1=_tmp5C;void*null1a=_tmp5D;void*b1=_tmp5E;void*zt1=_tmp5F;void*rel1=_tmp60;
# 380
Cyc_Unify_unify_it(t1a,t2a);
Cyc_Unify_unify_it(rgn2,rgn1);{
struct _fat_ptr _tmp7F=Cyc_Unify_failure_reason;struct _fat_ptr orig_failure=_tmp7F;
if(!Cyc_Unify_unify(zt1,zt2)){
Cyc_Unify_ts_failure=({struct _tuple11 _tmpD6;_tmpD6.f1=t1,_tmpD6.f2=t2;_tmpD6;});
Cyc_Unify_fail_because(({const char*_tmp80="(not both zero terminated)";_tag_fat(_tmp80,sizeof(char),27U);}));}
# 387
if(!Cyc_Unify_unify(rel1,rel2)){
Cyc_Unify_ts_failure=({struct _tuple11 _tmpD7;_tmpD7.f1=t1,_tmpD7.f2=t2;_tmpD7;});
Cyc_Unify_fail_because(({const char*_tmp81="(not both autoreleased)";_tag_fat(_tmp81,sizeof(char),24U);}));}
# 391
Cyc_Unify_unify_tqual(tqual1a,t1a,tqual2a,t2a);
if(!Cyc_Unify_unify(b1,b2)){
Cyc_Unify_ts_failure=({struct _tuple11 _tmpD8;_tmpD8.f1=t1,_tmpD8.f2=t2;_tmpD8;});
Cyc_Unify_fail_because(({const char*_tmp82="(different pointer bounds)";_tag_fat(_tmp82,sizeof(char),27U);}));}{
# 397
void*_tmp83=Cyc_Absyn_compress(b1);void*_stmttmp3=_tmp83;void*_tmp84=_stmttmp3;if(*((int*)_tmp84)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp84)->f1)== 14){
# 399
Cyc_Unify_failure_reason=orig_failure;
return;}else{goto _LL34;}}else{_LL34:
# 402
 Cyc_Unify_failure_reason=({const char*_tmp85="(incompatible pointer types)";_tag_fat(_tmp85,sizeof(char),29U);});
Cyc_Unify_unify_it(null1a,null2a);
return;};}}}}else{goto _LL25;}case 9: if(*((int*)_tmp50.f2)== 9){_tmp51=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp50.f1)->f1;_tmp52=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp50.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
# 408
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))
Cyc_Unify_fail_because(({const char*_tmp86="(cannot prove expressions are the same)";_tag_fat(_tmp86,sizeof(char),40U);}));
return;}}else{goto _LL25;}case 4: if(*((int*)_tmp50.f2)== 4){_tmp51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f1)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f1)->f1).tq;_tmp52=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f1)->f1).num_elts;_tmp53=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f1)->f1).zero_term;_tmp54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f2)->f1).elt_type;_tmp63=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f2)->f1).tq;_tmp59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f2)->f1).num_elts;_tmp5A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50.f2)->f1).zero_term;{void*t2a=_tmp51;struct Cyc_Absyn_Tqual tq2a=_tmp57;struct Cyc_Absyn_Exp*e1=_tmp52;void*zt1=_tmp53;void*t1a=_tmp54;struct Cyc_Absyn_Tqual tq1a=_tmp63;struct Cyc_Absyn_Exp*e2=_tmp59;void*zt2=_tmp5A;
# 414
Cyc_Unify_unify_it(t1a,t2a);
Cyc_Unify_unify_tqual(tq1a,t1a,tq2a,t2a);
Cyc_Unify_failure_reason=({const char*_tmp87="(not both zero terminated)";_tag_fat(_tmp87,sizeof(char),27U);});
Cyc_Unify_unify_it(zt1,zt2);
if(!Cyc_Unify_unify_const_exp_opt(e1,e2))
Cyc_Unify_fail_because(({const char*_tmp88="(different array sizes)";_tag_fat(_tmp88,sizeof(char),24U);}));
return;}}else{goto _LL25;}case 5: if(*((int*)_tmp50.f2)== 5){_tmp51=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).tvars;_tmp52=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).effect;_tmp57=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).ret_tqual;_tmp53=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).ret_type;_tmp54=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).args;_tmp58=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).c_varargs;_tmp59=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).cyc_varargs;_tmp5A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).rgn_po;_tmp5B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).attributes;_tmp5C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).requires_clause;_tmp5D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).requires_relns;_tmp5E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).ensures_clause;_tmp5F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).ensures_relns;_tmp60=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f1)->f1).return_value;_tmp61=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).tvars;_tmp62=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).effect;_tmp63=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).ret_tqual;_tmp64=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).ret_type;_tmp65=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).args;_tmp66=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).c_varargs;_tmp67=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).cyc_varargs;_tmp68=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).rgn_po;_tmp69=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).attributes;_tmp6A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).requires_clause;_tmp6B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).requires_relns;_tmp6C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).ensures_clause;_tmp6D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).ensures_relns;_tmp6E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp50.f2)->f1).return_value;{struct Cyc_List_List*tvs2=_tmp51;void*eff2=_tmp52;struct Cyc_Absyn_Tqual rt_tq2=_tmp57;void*rt2=_tmp53;struct Cyc_List_List*args2=_tmp54;int c_vararg2=_tmp58;struct Cyc_Absyn_VarargInfo*cyc_vararg2=_tmp59;struct Cyc_List_List*rpo2=_tmp5A;struct Cyc_List_List*atts2=_tmp5B;struct Cyc_Absyn_Exp*req2=_tmp5C;struct Cyc_List_List*req_relns2=_tmp5D;struct Cyc_Absyn_Exp*ens2=_tmp5E;struct Cyc_List_List*ens_relns2=_tmp5F;struct Cyc_Absyn_Vardecl*return_value2=_tmp60;struct Cyc_List_List*tvs1=_tmp61;void*eff1=_tmp62;struct Cyc_Absyn_Tqual rt_tq1=_tmp63;void*rt1=_tmp64;struct Cyc_List_List*args1=_tmp65;int c_vararg1=_tmp66;struct Cyc_Absyn_VarargInfo*cyc_vararg1=_tmp67;struct Cyc_List_List*rpo1=_tmp68;struct Cyc_List_List*atts1=_tmp69;struct Cyc_Absyn_Exp*req1=_tmp6A;struct Cyc_List_List*req_relns1=_tmp6B;struct Cyc_Absyn_Exp*ens1=_tmp6C;struct Cyc_List_List*ens_relns1=_tmp6D;struct Cyc_Absyn_Vardecl*return_value1=_tmp6E;
# 424
{struct _RegionHandle _tmp89=_new_region("rgn");struct _RegionHandle*rgn=& _tmp89;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(tvs2 == 0)
Cyc_Unify_fail_because(({const char*_tmp8A="(second function type has too few type variables)";_tag_fat(_tmp8A,sizeof(char),50U);}));{
void*_tmp8B=((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind;void*kb1=_tmp8B;
void*_tmp8C=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*kb2=_tmp8C;
if(!Cyc_Unify_unify_kindbound(kb1,kb2))
Cyc_Unify_fail_because((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmpDB;_tmpDB.tag=0,({
struct _fat_ptr _tmp10D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)tvs1->hd));_tmpDB.f1=_tmp10D;});_tmpDB;});struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0,({
struct _fat_ptr _tmp10E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Kinds_kind2string(Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs1->hd,& Cyc_Kinds_bk)));_tmpDA.f1=_tmp10E;});_tmpDA;});struct Cyc_String_pa_PrintArg_struct _tmp91=({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0,({
# 436
struct _fat_ptr _tmp10F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Kinds_kind2string(Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs2->hd,& Cyc_Kinds_bk)));_tmpD9.f1=_tmp10F;});_tmpD9;});void*_tmp8D[3];_tmp8D[0]=& _tmp8F,_tmp8D[1]=& _tmp90,_tmp8D[2]=& _tmp91;({struct _fat_ptr _tmp110=({const char*_tmp8E="(type var %s has different kinds %s and %s)";_tag_fat(_tmp8E,sizeof(char),44U);});Cyc_aprintf(_tmp110,_tag_fat(_tmp8D,sizeof(void*),3));});}));
# 438
inst=({struct Cyc_List_List*_tmp93=_region_malloc(rgn,sizeof(*_tmp93));({struct _tuple14*_tmp112=({struct _tuple14*_tmp92=_region_malloc(rgn,sizeof(*_tmp92));_tmp92->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp111=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp92->f2=_tmp111;});_tmp92;});_tmp93->hd=_tmp112;}),_tmp93->tl=inst;_tmp93;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}}
# 442
if(tvs2 != 0)
Cyc_Unify_fail_because(({const char*_tmp94="(second function type has too many type variables)";_tag_fat(_tmp94,sizeof(char),51U);}));
if(inst != 0){
({void*_tmp115=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=5,(_tmp95->f1).tvars=0,(_tmp95->f1).effect=eff1,(_tmp95->f1).ret_tqual=rt_tq1,(_tmp95->f1).ret_type=rt1,(_tmp95->f1).args=args1,(_tmp95->f1).c_varargs=c_vararg1,(_tmp95->f1).cyc_varargs=cyc_vararg1,(_tmp95->f1).rgn_po=rpo1,(_tmp95->f1).attributes=atts1,(_tmp95->f1).requires_clause=req1,(_tmp95->f1).requires_relns=req_relns1,(_tmp95->f1).ensures_clause=ens1,(_tmp95->f1).ensures_relns=ens_relns1,(_tmp95->f1).return_value=return_value1;_tmp95;});Cyc_Unify_unify_it(_tmp115,({
# 448
struct _RegionHandle*_tmp114=rgn;struct Cyc_List_List*_tmp113=inst;Cyc_Tcutil_rsubstitute(_tmp114,_tmp113,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));
_tmp96->tag=5,(_tmp96->f1).tvars=0,(_tmp96->f1).effect=eff2,(_tmp96->f1).ret_tqual=rt_tq2,(_tmp96->f1).ret_type=rt2,(_tmp96->f1).args=args2,(_tmp96->f1).c_varargs=c_vararg2,(_tmp96->f1).cyc_varargs=cyc_vararg2,(_tmp96->f1).rgn_po=rpo2,(_tmp96->f1).attributes=atts2,(_tmp96->f1).requires_clause=req2,(_tmp96->f1).requires_relns=req_relns2,(_tmp96->f1).ensures_clause=ens2,(_tmp96->f1).ensures_relns=ens_relns2,(_tmp96->f1).return_value=return_value2;_tmp96;}));}));});
# 453
_npop_handler(0);return;}}
# 425
;_pop_region();}
# 456
Cyc_Unify_unify_it(rt1,rt2);
Cyc_Unify_unify_tqual(rt_tq1,rt1,rt_tq2,rt2);
for(1;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple8 _tmp97=*((struct _tuple8*)args1->hd);struct _tuple8 _stmttmp4=_tmp97;struct _tuple8 _tmp98=_stmttmp4;void*_tmp9A;struct Cyc_Absyn_Tqual _tmp99;_tmp99=_tmp98.f2;_tmp9A=_tmp98.f3;{struct Cyc_Absyn_Tqual tqa=_tmp99;void*ta=_tmp9A;
struct _tuple8 _tmp9B=*((struct _tuple8*)args2->hd);struct _tuple8 _stmttmp5=_tmp9B;struct _tuple8 _tmp9C=_stmttmp5;void*_tmp9E;struct Cyc_Absyn_Tqual _tmp9D;_tmp9D=_tmp9C.f2;_tmp9E=_tmp9C.f3;{struct Cyc_Absyn_Tqual tqb=_tmp9D;void*tb=_tmp9E;
Cyc_Unify_unify_it(ta,tb);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}
# 464
Cyc_Unify_ts_failure=({struct _tuple11 _tmpDC;_tmpDC.f1=t1,_tmpDC.f2=t2;_tmpDC;});
if(args1 != 0 || args2 != 0)
Cyc_Unify_fail_because(({const char*_tmp9F="(function types have different number of arguments)";_tag_fat(_tmp9F,sizeof(char),52U);}));
if(c_vararg1 != c_vararg2)
Cyc_Unify_fail_because(({const char*_tmpA0="(only one function type takes C varargs)";_tag_fat(_tmpA0,sizeof(char),41U);}));
# 470
{struct _tuple15 _tmpA1=({struct _tuple15 _tmpDD;_tmpDD.f1=cyc_vararg1,_tmpDD.f2=cyc_vararg2;_tmpDD;});struct _tuple15 _stmttmp6=_tmpA1;struct _tuple15 _tmpA2=_stmttmp6;int _tmpAA;void*_tmpA9;struct Cyc_Absyn_Tqual _tmpA8;void*_tmpA7;int _tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;if(_tmpA2.f1 == 0){if(_tmpA2.f2 == 0)
goto _LL3C;else{
goto _LL42;}}else{if(_tmpA2.f2 == 0){_LL42:
 Cyc_Unify_fail_because(({const char*_tmpAB="(only one function type takes varargs)";_tag_fat(_tmpAB,sizeof(char),39U);}));}else{_tmpA3=(_tmpA2.f1)->name;_tmpA4=(_tmpA2.f1)->tq;_tmpA5=(_tmpA2.f1)->type;_tmpA6=(_tmpA2.f1)->inject;_tmpA7=(_tmpA2.f2)->name;_tmpA8=(_tmpA2.f2)->tq;_tmpA9=(_tmpA2.f2)->type;_tmpAA=(_tmpA2.f2)->inject;{struct _fat_ptr*n1=_tmpA3;struct Cyc_Absyn_Tqual tq1=_tmpA4;void*tp1=_tmpA5;int i1=_tmpA6;struct _fat_ptr*n2=_tmpA7;struct Cyc_Absyn_Tqual tq2=_tmpA8;void*tp2=_tmpA9;int i2=_tmpAA;
# 475
Cyc_Unify_unify_it(tp1,tp2);
Cyc_Unify_unify_tqual(tq1,tp1,tq2,tp2);
if(i1 != i2)
Cyc_Unify_fail_because(({const char*_tmpAC="(only one function type injects varargs)";_tag_fat(_tmpAC,sizeof(char),41U);}));}}}_LL3C:;}{
# 482
int bad_effect;
if(eff1 == 0 && eff2 == 0)
bad_effect=0;else{
if(eff1 == 0 || eff2 == 0)
bad_effect=1;else{
# 488
bad_effect=!Cyc_Unify_unify_effect(eff1,eff2);}}
Cyc_Unify_ts_failure=({struct _tuple11 _tmpDE;_tmpDE.f1=t1,_tmpDE.f2=t2;_tmpDE;});
if(bad_effect)
Cyc_Unify_fail_because((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0,({struct _fat_ptr _tmp116=(struct _fat_ptr)((struct _fat_ptr)((unsigned)eff1?Cyc_Absynpp_typ2string(eff1):(struct _fat_ptr)({const char*_tmpB2="-";_tag_fat(_tmpB2,sizeof(char),2U);})));_tmpE0.f1=_tmp116;});_tmpE0;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmpDF;_tmpDF.tag=0,({
struct _fat_ptr _tmp117=(struct _fat_ptr)((struct _fat_ptr)((unsigned)eff2?Cyc_Absynpp_typ2string(eff2):(struct _fat_ptr)({const char*_tmpB1="-";_tag_fat(_tmpB1,sizeof(char),2U);})));_tmpDF.f1=_tmp117;});_tmpDF;});void*_tmpAD[2];_tmpAD[0]=& _tmpAF,_tmpAD[1]=& _tmpB0;({struct _fat_ptr _tmp118=({const char*_tmpAE="(function effects (%s,%s) do not match)";_tag_fat(_tmpAE,sizeof(char),40U);});Cyc_aprintf(_tmp118,_tag_fat(_tmpAD,sizeof(void*),2));});}));
if(!Cyc_Atts_equiv_fn_atts(atts2,atts1))
Cyc_Unify_fail_because(({const char*_tmpB3="(function types have different attributes)";_tag_fat(_tmpB3,sizeof(char),43U);}));
if(!Cyc_Tcutil_same_rgn_po(rpo2,rpo1))
Cyc_Unify_fail_because(({const char*_tmpB4="(function types have different region lifetime orderings)";_tag_fat(_tmpB4,sizeof(char),58U);}));
if(!Cyc_Unify_check_logical_equivalence(req_relns1,req_relns2))
Cyc_Unify_fail_because(({const char*_tmpB5="(@requires clauses not equivalent)";_tag_fat(_tmpB5,sizeof(char),35U);}));
if(!Cyc_Unify_check_logical_equivalence(ens_relns1,ens_relns2))
Cyc_Unify_fail_because(({const char*_tmpB6="(@ensures clauses not equivalent)";_tag_fat(_tmpB6,sizeof(char),34U);}));
return;}}}else{goto _LL25;}case 6: if(*((int*)_tmp50.f2)== 6){_tmp51=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp50.f1)->f1;_tmp52=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp50.f2)->f1;{struct Cyc_List_List*ts2=_tmp51;struct Cyc_List_List*ts1=_tmp52;
# 504
for(1;ts1 != 0 && ts2 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
struct _tuple13 _tmpB7=*((struct _tuple13*)ts1->hd);struct _tuple13 _stmttmp7=_tmpB7;struct _tuple13 _tmpB8=_stmttmp7;void*_tmpBA;struct Cyc_Absyn_Tqual _tmpB9;_tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;{struct Cyc_Absyn_Tqual tqa=_tmpB9;void*ta=_tmpBA;
struct _tuple13 _tmpBB=*((struct _tuple13*)ts2->hd);struct _tuple13 _stmttmp8=_tmpBB;struct _tuple13 _tmpBC=_stmttmp8;void*_tmpBE;struct Cyc_Absyn_Tqual _tmpBD;_tmpBD=_tmpBC.f1;_tmpBE=_tmpBC.f2;{struct Cyc_Absyn_Tqual tqb=_tmpBD;void*tb=_tmpBE;
Cyc_Unify_unify_it(ta,tb);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}
# 510
if(ts1 == 0 && ts2 == 0)
return;
Cyc_Unify_ts_failure=({struct _tuple11 _tmpE1;_tmpE1.f1=t1,_tmpE1.f2=t2;_tmpE1;});
Cyc_Unify_fail_because(({const char*_tmpBF="(tuple types have different numbers of components)";_tag_fat(_tmpBF,sizeof(char),51U);}));}}else{goto _LL25;}case 7: if(*((int*)_tmp50.f2)== 7){_tmp55=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp50.f1)->f1;_tmp51=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp50.f1)->f2;_tmp56=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp50.f2)->f1;_tmp52=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp50.f2)->f2;{enum Cyc_Absyn_AggrKind k2=_tmp55;struct Cyc_List_List*fs2=_tmp51;enum Cyc_Absyn_AggrKind k1=_tmp56;struct Cyc_List_List*fs1=_tmp52;
# 516
if((int)k1 != (int)k2)
Cyc_Unify_fail_because(({const char*_tmpC0="(struct and union type)";_tag_fat(_tmpC0,sizeof(char),24U);}));
for(1;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
struct Cyc_Absyn_Aggrfield*_tmpC1=(struct Cyc_Absyn_Aggrfield*)fs1->hd;struct Cyc_Absyn_Aggrfield*f1=_tmpC1;
struct Cyc_Absyn_Aggrfield*_tmpC2=(struct Cyc_Absyn_Aggrfield*)fs2->hd;struct Cyc_Absyn_Aggrfield*f2=_tmpC2;
if(Cyc_strptrcmp(f1->name,f2->name)!= 0)
Cyc_Unify_fail_because(({const char*_tmpC3="(different member names)";_tag_fat(_tmpC3,sizeof(char),25U);}));
Cyc_Unify_unify_it(f1->type,f2->type);
Cyc_Unify_unify_tqual(f1->tq,f1->type,f2->tq,f2->type);
Cyc_Unify_ts_failure=({struct _tuple11 _tmpE2;_tmpE2.f1=t1,_tmpE2.f2=t2;_tmpE2;});
if(!Cyc_Atts_same_atts(f1->attributes,f2->attributes))
Cyc_Unify_fail_because(({const char*_tmpC4="(different attributes on member)";_tag_fat(_tmpC4,sizeof(char),33U);}));
if(!Cyc_Unify_unify_const_exp_opt(f1->width,f2->width))
Cyc_Unify_fail_because(({const char*_tmpC5="(different bitfield widths on member)";_tag_fat(_tmpC5,sizeof(char),38U);}));
if(!Cyc_Unify_unify_const_exp_opt(f1->requires_clause,f2->requires_clause))
Cyc_Unify_fail_because(({const char*_tmpC6="(different @requires clauses on member)";_tag_fat(_tmpC6,sizeof(char),40U);}));}
# 533
if(fs1 != 0 || fs2 != 0)
Cyc_Unify_fail_because(({const char*_tmpC7="(different number of members)";_tag_fat(_tmpC7,sizeof(char),30U);}));
return;}}else{goto _LL25;}case 8: if(*((int*)_tmp50.f2)== 8){_tmp51=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50.f1)->f2;_tmp52=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50.f1)->f3;_tmp53=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50.f2)->f2;_tmp54=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50.f2)->f3;{struct Cyc_List_List*ts1=_tmp51;struct Cyc_Absyn_Typedefdecl*td1=_tmp52;struct Cyc_List_List*ts2=_tmp53;struct Cyc_Absyn_Typedefdecl*td2=_tmp54;
# 538
if(td1 != td2)
Cyc_Unify_fail_because(({const char*_tmpC8="(different abstract typedefs)";_tag_fat(_tmpC8,sizeof(char),30U);}));
Cyc_Unify_failure_reason=({const char*_tmpC9="(type parameters to typedef differ)";_tag_fat(_tmpC9,sizeof(char),36U);});
Cyc_Unify_unify_list(ts1,ts2);
return;}}else{goto _LL25;}default: _LL25:
(int)_throw((void*)& Cyc_Unify_Unify_val);}}}}};}}
