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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 232 "cycboot.h"
extern struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);struct Cyc_Position_Error;
# 42 "position.h"
extern struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned,struct _fat_ptr);
# 48
extern void Cyc_Position_post_error(struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 843 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 859
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 128 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 79 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_force_kb(void*);
# 54 "attributes.h"
int Cyc_Atts_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 75
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 38 "tcdecl.h"
void Cyc_Tcdecl_merr(unsigned,struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr ap);
# 61 "tcdecl.h"
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _fat_ptr*);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*,int*,struct _fat_ptr*,unsigned,struct _fat_ptr*);char Cyc_Tcdecl_Incompatible[13U]="Incompatible";
# 37 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
# 39
void Cyc_Tcdecl_merr(unsigned loc,struct _fat_ptr*msg1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 43
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _fat_ptr fmt2;
if(Cyc_strlen(*msg1)== 0U)
fmt2=Cyc_strconcat(({const char*_Tmp0="%s";_tag_fat(_Tmp0,sizeof(char),3U);}),fmt);else{
# 49
fmt2=Cyc_strconcat(({const char*_Tmp0="%s ";_tag_fat(_Tmp0,sizeof(char),4U);}),fmt);}{
struct _fat_ptr ap2=({unsigned _Tmp0=_get_fat_size(ap,sizeof(void*))+ 1U;_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=_get_fat_size(ap,sizeof(void*))+ 1U;unsigned i;for(i=0;i < _Tmp2;++ i){i == 0U?({void*_Tmp3=(void*)({struct Cyc_String_pa_PrintArg_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_String_pa_PrintArg_struct));_Tmp4->tag=0,_Tmp4->f1=*msg1;_Tmp4;});_Tmp1[i]=_Tmp3;}):(_Tmp1[i]=((void**)ap.curr)[(int)(i - 1U)]);}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
# 52
Cyc_Position_post_error(({unsigned _Tmp0=loc;Cyc_Position_mk_err(_Tmp0,Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2));}));}}}
# 57
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 59
struct _fat_ptr s0_str=(int)s0 == 2?({const char*_Tmp0="public ";_tag_fat(_Tmp0,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s0);
struct _fat_ptr s1_str=(int)s1 == 2?({const char*_Tmp0="public ";_tag_fat(_Tmp0,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s1);
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=t;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=v;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=s1_str;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s0_str;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp5="%s %s is %sbut expected scope is %s";_tag_fat(_Tmp5,sizeof(char),36U);}),_tag_fat(_Tmp4,sizeof(void*),4));});}struct _tuple11{enum Cyc_Absyn_Scope f1;int f2;};struct _tuple12{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 65
struct _tuple11 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg,int externCmerge){
# 67
{struct _tuple12 _stmttmp0=({struct _tuple12 _Tmp0;_Tmp0.f1=s0,_Tmp0.f2=s1;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp0;switch((int)_Tmp0.f1){case Cyc_Absyn_ExternC: switch((int)_Tmp0.f2){case Cyc_Absyn_ExternC:
# 69
 goto _LL0;case Cyc_Absyn_Public:
# 71
 goto _LL6;case Cyc_Absyn_Extern: _LL6:
 goto _LL8;default: _LLC:
# 75
 goto _LLE;}case Cyc_Absyn_Public: switch((int)_Tmp0.f2){case Cyc_Absyn_ExternC: _LL8:
# 73
 goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL16:
# 82
 goto _LL18;default: goto _LL1B;}case Cyc_Absyn_Extern: switch((int)_Tmp0.f2){case Cyc_Absyn_ExternC: _LLA:
# 74
 if(externCmerge)goto _LL0;else{goto _LLC;}case Cyc_Absyn_Extern: goto _LLF;default:
# 79
 goto _LL0;}default: switch((int)_Tmp0.f2){case Cyc_Absyn_ExternC: _LLE:
# 76
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple11 _Tmp1;_Tmp1.f1=s1,_Tmp1.f2=0;_Tmp1;});case Cyc_Absyn_Extern: _LLF:
# 78
 s1=s0;goto _LL0;default: switch((int)_Tmp0.f1){case Cyc_Absyn_Static: if(_Tmp0.f2 == Cyc_Absyn_Static)
# 81
goto _LL16;else{goto _LL1B;}case Cyc_Absyn_Abstract: if(_Tmp0.f2 == Cyc_Absyn_Abstract){_LL18:
# 83
 goto _LL1A;}else{goto _LL1B;}case Cyc_Absyn_Register: if(_Tmp0.f2 == Cyc_Absyn_Register){_LL1A:
 goto _LL0;}else{goto _LL1B;}default: _LL1B:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple11 _Tmp1;_Tmp1.f1=s1,_Tmp1.f2=0;_Tmp1;});}}}_LL0:;}
# 87
return({struct _tuple11 _Tmp0;_Tmp0.f1=s1,_Tmp0.f2=1;_Tmp0;});}
# 90
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 92
return Cyc_Unify_unify(t0,t1);}
# 96
static unsigned Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_stmttmp1=e->r;void*_Tmp0=_stmttmp1;int _Tmp1;if(*((int*)_Tmp0)== 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).tag == 5){_Tmp1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).val).f2;{int x=_Tmp1;
return(unsigned)x;}}else{goto _LL3;}}else{_LL3:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp2->tag=Cyc_Core_Invalid_argument,_Tmp2->f1=({const char*_Tmp3="Tcdecl::get_uint_const_value";_tag_fat(_Tmp3,sizeof(char),29U);});_Tmp2;}));};}struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 103
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 105
struct Cyc_List_List*inst=0;
for(1;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
inst=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple13*_Tmp1=({struct _tuple13*_Tmp2=_cycalloc(sizeof(struct _tuple13));_Tmp2->f1=(struct Cyc_Absyn_Tvar*)(_check_null(tvs1))->hd,({void*_Tmp3=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp4->tag=2,_Tmp4->f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=inst;_Tmp0;});}
return inst;}
# 111
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 114
if(({int _Tmp0=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tvs0);_Tmp0 != ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tvs1);})){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=t;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=v;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp3="%s %s has a different number of type parameters";_tag_fat(_Tmp3,sizeof(char),48U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
return 0;}{
# 118
struct Cyc_List_List*x0=tvs0;
struct Cyc_List_List*x1=tvs1;
for(1;x0 != 0;(x0=x0->tl,x1=x1->tl)){
Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)x0->hd)->kind,((struct Cyc_Absyn_Tvar*)(_check_null(x1))->hd)->kind);{
struct Cyc_Absyn_Kind*c0=Cyc_Kinds_force_kb(((struct Cyc_Absyn_Tvar*)x0->hd)->kind);
struct Cyc_Absyn_Kind*c1=Cyc_Kinds_force_kb(((struct Cyc_Absyn_Tvar*)x1->hd)->kind);
if(c0 == c1)continue;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=t;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=v;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=Cyc_Absynpp_kind2string(c0);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*((struct Cyc_Absyn_Tvar*)x0->hd)->name;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_kind2string(c1);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp6="%s %s has a different kind (%s) for type parameter %s (%s)";_tag_fat(_Tmp6,sizeof(char),59U);}),_tag_fat(_Tmp5,sizeof(void*),5));});
return 0;}}
# 129
return 1;}}
# 132
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 134
if(Cyc_Atts_same_atts(atts0,atts1))
return 1;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=t;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=v;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp3="%s %s has different attributes";_tag_fat(_Tmp3,sizeof(char),31U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
return 0;}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple15{void*f1;void*f2;};
# 141
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned loc,struct _fat_ptr*msg){
# 143
struct _fat_ptr v=Cyc_Absynpp_qvar2string(d0->name);
int res=1;
# 149
if(!((int)d0->kind == (int)d1->kind))return 0;
# 152
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_Tmp0="type";_tag_fat(_Tmp0,sizeof(char),5U);}),v,loc,msg))return 0;{
# 155
struct _tuple11 _stmttmp2=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,({const char*_Tmp0="type";_tag_fat(_Tmp0,sizeof(char),5U);}),v,loc,msg,1);
# 155
struct _tuple11 _Tmp0=_stmttmp2;int _Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Scope new_scope=_Tmp2;int res_scope=_Tmp1;
# 157
if(!res_scope)res=0;
# 160
if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,({const char*_Tmp3="type";_tag_fat(_Tmp3,sizeof(char),5U);}),v,loc,msg))res=0;{
# 162
struct Cyc_Absyn_Aggrdecl*d2;
# 164
{struct _tuple14 _stmttmp3=({struct _tuple14 _Tmp3;_Tmp3.f1=d0->impl,_Tmp3.f2=d1->impl;_Tmp3;});struct _tuple14 _Tmp3=_stmttmp3;int _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;int _Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;if(_Tmp3.f2 == 0){
d2=d0;goto _LL3;}else{if(_Tmp3.f1 == 0){
d2=d1;goto _LL3;}else{_TmpB=(_Tmp3.f1)->exist_vars;_TmpA=(_Tmp3.f1)->rgn_po;_Tmp9=(_Tmp3.f1)->fields;_Tmp8=(_Tmp3.f1)->tagged;_Tmp7=(_Tmp3.f2)->exist_vars;_Tmp6=(_Tmp3.f2)->rgn_po;_Tmp5=(_Tmp3.f2)->fields;_Tmp4=(_Tmp3.f2)->tagged;{struct Cyc_List_List*exist_vars0=_TmpB;struct Cyc_List_List*rgn_po0=_TmpA;struct Cyc_List_List*f0s=_Tmp9;int tagged0=_Tmp8;struct Cyc_List_List*exist_vars1=_Tmp7;struct Cyc_List_List*rgn_po1=_Tmp6;struct Cyc_List_List*f1s=_Tmp5;int tagged1=_Tmp4;
# 171
if(!Cyc_Tcdecl_check_tvs(exist_vars0,exist_vars1,({const char*_TmpC="type";_tag_fat(_TmpC,sizeof(char),5U);}),v,loc,msg))
return 0;{
# 175
struct Cyc_List_List*inst=({struct Cyc_List_List*_TmpC=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(d0->tvs,exist_vars0);Cyc_Tcdecl_build_tvs_map(_TmpC,
({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(d1->tvs,exist_vars1));});
# 180
for(1;rgn_po0 != 0 && rgn_po1 != 0;(
rgn_po0=rgn_po0->tl,rgn_po1=rgn_po1->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple15*)rgn_po0->hd)).f1,(*((struct _tuple15*)rgn_po1->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple15*)rgn_po0->hd)).f2,(*((struct _tuple15*)rgn_po1->hd)).f2);}
# 187
for(1;f0s != 0 && f1s != 0;(f0s=f0s->tl,f1s=f1s->tl)){
struct Cyc_Absyn_Aggrfield*_stmttmp4=(struct Cyc_Absyn_Aggrfield*)f0s->hd;struct Cyc_Absyn_Aggrfield*_TmpC=_stmttmp4;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpC->name;_Tmp11=_TmpC->tq;_Tmp10=_TmpC->type;_TmpF=_TmpC->width;_TmpE=_TmpC->attributes;_TmpD=_TmpC->requires_clause;{struct _fat_ptr*fn0=_Tmp12;struct Cyc_Absyn_Tqual tq0=_Tmp11;void*t0=_Tmp10;struct Cyc_Absyn_Exp*width0=_TmpF;struct Cyc_List_List*atts0=_TmpE;struct Cyc_Absyn_Exp*req0=_TmpD;
struct Cyc_Absyn_Aggrfield*_stmttmp5=(struct Cyc_Absyn_Aggrfield*)f1s->hd;struct Cyc_Absyn_Aggrfield*_Tmp13=_stmttmp5;void*_Tmp14;void*_Tmp15;void*_Tmp16;void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;void*_Tmp19;_Tmp19=_Tmp13->name;_Tmp18=_Tmp13->tq;_Tmp17=_Tmp13->type;_Tmp16=_Tmp13->width;_Tmp15=_Tmp13->attributes;_Tmp14=_Tmp13->requires_clause;{struct _fat_ptr*fn1=_Tmp19;struct Cyc_Absyn_Tqual tq1=_Tmp18;void*t1=_Tmp17;struct Cyc_Absyn_Exp*width1=_Tmp16;struct Cyc_List_List*atts1=_Tmp15;struct Cyc_Absyn_Exp*req1=_Tmp14;
# 191
if(Cyc_strptrcmp(fn0,fn1)!= 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=({const char*_Tmp1C="type";_tag_fat(_Tmp1C,sizeof(char),5U);});_Tmp1B;});struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=v;_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=*fn0;_Tmp1D;});struct Cyc_String_pa_PrintArg_struct _Tmp1D=({struct Cyc_String_pa_PrintArg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=*fn1;_Tmp1E;});void*_Tmp1E[4];_Tmp1E[0]=& _Tmp1A,_Tmp1E[1]=& _Tmp1B,_Tmp1E[2]=& _Tmp1C,_Tmp1E[3]=& _Tmp1D;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1F="%s %s : field name mismatch %s != %s";_tag_fat(_Tmp1F,sizeof(char),37U);}),_tag_fat(_Tmp1E,sizeof(void*),4));});
# 194
return 0;}
# 196
if(!Cyc_Atts_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=({const char*_Tmp1C="type";_tag_fat(_Tmp1C,sizeof(char),5U);});_Tmp1B;});struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=v;_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=*fn0;_Tmp1D;});void*_Tmp1D[3];_Tmp1D[0]=& _Tmp1A,_Tmp1D[1]=& _Tmp1B,_Tmp1D[2]=& _Tmp1C;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1E="%s %s : attribute mismatch on field %s";_tag_fat(_Tmp1E,sizeof(char),39U);}),_tag_fat(_Tmp1D,sizeof(void*),3));});
res=0;}
# 200
if(!Cyc_Absyn_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=({const char*_Tmp1C="type";_tag_fat(_Tmp1C,sizeof(char),5U);});_Tmp1B;});struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=v;_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=*fn0;_Tmp1D;});void*_Tmp1D[3];_Tmp1D[0]=& _Tmp1A,_Tmp1D[1]=& _Tmp1B,_Tmp1D[2]=& _Tmp1C;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1E="%s %s : qualifier mismatch on field %s";_tag_fat(_Tmp1E,sizeof(char),39U);}),_tag_fat(_Tmp1D,sizeof(void*),3));});
res=0;}
# 204
if(((width0 != 0 && width1 != 0)&&({
unsigned _Tmp1A=Cyc_Tcdecl_get_uint_const_value(width0);_Tmp1A != 
Cyc_Tcdecl_get_uint_const_value(width1);})||
 width0 == 0 && width1 != 0)||
 width0 != 0 && width1 == 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=({const char*_Tmp1C="type";_tag_fat(_Tmp1C,sizeof(char),5U);});_Tmp1B;});struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=v;_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=*fn0;_Tmp1D;});void*_Tmp1D[3];_Tmp1D[0]=& _Tmp1A,_Tmp1D[1]=& _Tmp1B,_Tmp1D[2]=& _Tmp1C;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1E="%s %s : bitfield mismatch on field %s";_tag_fat(_Tmp1E,sizeof(char),38U);}),_tag_fat(_Tmp1D,sizeof(void*),3));});
res=0;}
# 212
if(req0 != 0 && req1 != 0){
void*subst_w1=({struct Cyc_List_List*_Tmp1A=inst;Cyc_Tcutil_substitute(_Tmp1A,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp1B=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp1B->tag=9,_Tmp1B->f1=req1;_Tmp1B;}));});
if(!({void*_Tmp1A=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp1B=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp1B->tag=9,_Tmp1B->f1=req0;_Tmp1B;});Cyc_Tcdecl_check_type(_Tmp1A,subst_w1);})){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=v;_Tmp1B;});struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,({
struct _fat_ptr _Tmp1D=Cyc_Absynpp_exp2string(req0);_Tmp1C.f1=_Tmp1D;});_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,({struct _fat_ptr _Tmp1E=Cyc_Absynpp_exp2string(req1);_Tmp1D.f1=_Tmp1E;});_Tmp1D;});void*_Tmp1D[3];_Tmp1D[0]=& _Tmp1A,_Tmp1D[1]=& _Tmp1B,_Tmp1D[2]=& _Tmp1C;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1E="type %s : mismatch on @requires clauses: %s != %s";_tag_fat(_Tmp1E,sizeof(char),50U);}),_tag_fat(_Tmp1D,sizeof(void*),3));});
res=0;}}else{
# 219
if(req0 == 0 && req1 != 0 ||
 req0 != 0 && req1 == 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=v;_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1C="type %s : mismatch on @requires clauses";_tag_fat(_Tmp1C,sizeof(char),40U);}),_tag_fat(_Tmp1B,sizeof(void*),1));});
res=0;}}{
# 225
void*subst_t1=Cyc_Tcutil_substitute(inst,t1);
if(!Cyc_Tcdecl_check_type(t0,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp1A=({struct Cyc_String_pa_PrintArg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=v;_Tmp1B;});struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=*fn0;_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,({
struct _fat_ptr _Tmp1E=Cyc_Absynpp_typ2string(t0);_Tmp1D.f1=_Tmp1E;});_Tmp1D;});struct Cyc_String_pa_PrintArg_struct _Tmp1D=({struct Cyc_String_pa_PrintArg_struct _Tmp1E;_Tmp1E.tag=0,({struct _fat_ptr _Tmp1F=Cyc_Absynpp_typ2string(subst_t1);_Tmp1E.f1=_Tmp1F;});_Tmp1E;});void*_Tmp1E[4];_Tmp1E[0]=& _Tmp1A,_Tmp1E[1]=& _Tmp1B,_Tmp1E[2]=& _Tmp1C,_Tmp1E[3]=& _Tmp1D;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp1F="type %s : type mismatch on field %s: %s != %s";_tag_fat(_Tmp1F,sizeof(char),46U);}),_tag_fat(_Tmp1E,sizeof(void*),4));});
Cyc_Unify_explain_failure();
res=0;}}}}}
# 234
if(f0s != 0){
({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=v;_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=*((struct Cyc_Absyn_Aggrfield*)f0s->hd)->name;_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_Tcdecl_merr(loc,msg,({const char*_TmpF="type %s is missing field %s";_tag_fat(_TmpF,sizeof(char),28U);}),_tag_fat(_TmpE,sizeof(void*),2));});
res=0;}
# 238
if(f1s != 0){
({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=v;_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=*((struct Cyc_Absyn_Aggrfield*)f1s->hd)->name;_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_Tcdecl_merr(loc,msg,({const char*_TmpF="type %s has extra field %s";_tag_fat(_TmpF,sizeof(char),27U);}),_tag_fat(_TmpE,sizeof(void*),2));});
res=0;}
# 243
if(tagged0 != tagged1){
({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=v;_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;Cyc_Tcdecl_merr(loc,msg,({const char*_TmpE="%s : previous declaration disagrees with @tagged qualifier";_tag_fat(_TmpE,sizeof(char),59U);}),_tag_fat(_TmpD,sizeof(void*),1));});
res=0;}
# 248
d2=d0;
goto _LL3;}}}}_LL3:;}
# 252
if(!res)return 0;
if((int)new_scope == (int)d2->sc)return d2;
d2=({struct Cyc_Absyn_Aggrdecl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));*_Tmp3=*d2;_Tmp3;});
d2->sc=new_scope;
return d2;}}}}
# 259
inline static struct _fat_ptr Cyc_Tcdecl_is_x2string(int is_x){
return is_x?({const char*_Tmp0="@extensible datatype";_tag_fat(_Tmp0,sizeof(char),21U);}):({const char*_Tmp0="datatype";_tag_fat(_Tmp0,sizeof(char),9U);});}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 265
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _fat_ptr t,struct _fat_ptr v,struct _fat_ptr*msg){
# 269
unsigned loc=f1->loc;
# 272
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=t;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=v;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=*(*f1->name).f2;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*(*f0->name).f2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp5="%s %s: field name mismatch %s != %s";_tag_fat(_Tmp5,sizeof(char),36U);}),_tag_fat(_Tmp4,sizeof(void*),4));});
# 275
return 0;}{
# 278
struct _fat_ptr name=*(*f0->name).f2;
# 281
struct _tuple11 _stmttmp6=({
enum Cyc_Absyn_Scope _Tmp0=f0->sc;enum Cyc_Absyn_Scope _Tmp1=f1->sc;struct _fat_ptr _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="in %s %s, field";_tag_fat(_Tmp6,sizeof(char),16U);}),_tag_fat(_Tmp5,sizeof(void*),2));});struct _fat_ptr _Tmp3=name;unsigned _Tmp4=loc;Cyc_Tcdecl_merge_scope(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,msg,0);});
# 281
struct _tuple11 _Tmp0=_stmttmp6;int _Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Scope new_scope=_Tmp2;int res=_Tmp1;
# 285
struct Cyc_List_List*typs0=f0->typs;
struct Cyc_List_List*typs1=f1->typs;
if(({int _Tmp3=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(typs0);_Tmp3 != ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(typs1);})){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=name;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp7="%s %s, field %s: parameter number mismatch";_tag_fat(_Tmp7,sizeof(char),43U);}),_tag_fat(_Tmp6,sizeof(void*),3));});
res=0;}
# 292
for(1;typs0 != 0 && typs1 != 0;(typs0=typs0->tl,typs1=typs1->tl)){
# 294
if(!Cyc_Absyn_equal_tqual((*((struct _tuple16*)typs0->hd)).f1,(*((struct _tuple16*)typs1->hd)).f1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=name;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp7="%s %s, field %s: parameter qualifier";_tag_fat(_Tmp7,sizeof(char),37U);}),_tag_fat(_Tmp6,sizeof(void*),3));});
res=0;}{
# 298
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple16*)typs1->hd)).f2);
# 300
if(!Cyc_Tcdecl_check_type((*((struct _tuple16*)typs0->hd)).f2,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=name;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_Absynpp_typ2string((*((struct _tuple16*)typs0->hd)).f2);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_typ2string(subst_t1);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[5];_Tmp8[0]=& _Tmp3,_Tmp8[1]=& _Tmp4,_Tmp8[2]=& _Tmp5,_Tmp8[3]=& _Tmp6,_Tmp8[4]=& _Tmp7;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp9="%s %s, field %s: parameter type mismatch %s != %s";_tag_fat(_Tmp9,sizeof(char),50U);}),_tag_fat(_Tmp8,sizeof(void*),5));});
Cyc_Unify_explain_failure();
res=0;}}}
# 308
if(!res)return 0;
if((int)f0->sc == (int)new_scope)return f0;{
struct Cyc_Absyn_Datatypefield*f2=({struct Cyc_Absyn_Datatypefield*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));*_Tmp3=*f0;_Tmp3;});
f2->sc=new_scope;
return f2;}}}}
# 315
static struct _tuple16*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple16*x){
# 318
struct _tuple16*_Tmp0=x;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_Absyn_Tqual tq=_Tmp2;void*t=_Tmp1;
return({struct _tuple16*_Tmp3=_cycalloc(sizeof(struct _tuple16));_Tmp3->f1=tq,({void*_Tmp4=Cyc_Tcutil_substitute(inst,t);_Tmp3->f2=_Tmp4;});_Tmp3;});}}
# 324
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 326
struct Cyc_Absyn_Datatypefield*f0=({struct Cyc_Absyn_Datatypefield*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));*_Tmp0=*f1;_Tmp0;});
({struct Cyc_List_List*_Tmp0=({(struct Cyc_List_List*(*)(struct _tuple16*(*)(struct Cyc_List_List*,struct _tuple16*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);f0->typs=_Tmp0;});
# 329
return f0;}
# 336
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 345
struct Cyc_List_List**f2sp=({struct Cyc_List_List**_Tmp0=_cycalloc(sizeof(struct Cyc_List_List*));*_Tmp0=0;_Tmp0;});
struct Cyc_List_List**next=f2sp;
int cmp=-1;
# 349
for(1;f0s != 0 && f1s != 0;f1s=f1s->tl){
while(f0s != 0 &&(cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*node=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,_Tmp0->tl=0;_Tmp0;});
*next=node;
next=& node->tl;
f0s=f0s->tl;}
# 356
if(f0s == 0 || cmp > 0){
# 358
*incl=0;{
struct Cyc_List_List*node=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp1=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
*next=node;
next=& node->tl;}}else{
# 364
struct Cyc_Absyn_Datatypefield*f2=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(f2 != 0){
if(f2 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*node=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=f2,_Tmp0->tl=0;_Tmp0;});
*next=node;
next=& node->tl;}}else{
# 371
*res=0;}
# 373
f0s=f0s->tl;}}
# 378
if(f1s != 0){
*incl=0;
*next=f1s;}else{
# 382
*next=f0s;}
# 384
return*f2sp;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 387
static struct _tuple17 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0)return({struct _tuple17 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=0;_Tmp0;});
if(f->tl == 0)return({struct _tuple17 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=0;_Tmp0;});{
struct _tuple17 _stmttmp7=Cyc_Tcdecl_split((_check_null(f->tl))->tl);struct _tuple17 _Tmp0=_stmttmp7;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*f1=_Tmp2;struct Cyc_List_List*f2=_Tmp1;
return({struct _tuple17 _Tmp3;({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=f->hd,_Tmp5->tl=f1;_Tmp5;});_Tmp3.f1=_Tmp4;}),({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=(_check_null(f->tl))->hd,_Tmp5->tl=f2;_Tmp5;});_Tmp3.f2=_Tmp4;});_Tmp3;});}}}
# 396
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _fat_ptr*v,unsigned loc,struct _fat_ptr*msg){
# 399
struct _tuple17 _stmttmp8=({(struct _tuple17(*)(struct Cyc_List_List*))Cyc_Tcdecl_split;})(f);struct _tuple17 _Tmp0=_stmttmp8;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*f1=_Tmp2;struct Cyc_List_List*f2=_Tmp1;
if(f1 != 0 && f1->tl != 0)
f1=Cyc_Tcdecl_sort_xdatatype_fields(f1,res,v,loc,msg);
if(f2 != 0 && f2->tl != 0)
f2=Cyc_Tcdecl_sort_xdatatype_fields(f2,res,v,loc,msg);
return({struct Cyc_List_List*_Tmp3=f1;struct Cyc_List_List*_Tmp4=f2;int*_Tmp5=res;int*_Tmp6=({int*_Tmp7=_cycalloc_atomic(sizeof(int));*_Tmp7=1;_Tmp7;});struct _fat_ptr _Tmp7=({const char*_Tmp8="@extensible datatype";_tag_fat(_Tmp8,sizeof(char),21U);});struct _fat_ptr _Tmp8=*v;unsigned _Tmp9=loc;Cyc_Tcdecl_merge_xdatatype_fields(_Tmp3,_Tmp4,0,0,0,_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,msg);});}}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 408
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned loc,struct _fat_ptr*msg){
# 410
struct _fat_ptr v=Cyc_Absynpp_qvar2string(d0->name);
struct _fat_ptr t=({const char*_Tmp0="datatype";_tag_fat(_Tmp0,sizeof(char),9U);});
int res=1;
# 417
if(d0->is_extensible != d1->is_extensible){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=v;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_Tcdecl_is_x2string(d0->is_extensible);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Tcdecl_is_x2string(d1->is_extensible);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp4="expected %s to be a %s instead of a %s";_tag_fat(_Tmp4,sizeof(char),39U);}),_tag_fat(_Tmp3,sizeof(void*),3));});
res=0;}else{
# 422
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 425
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,v,loc,msg))return 0;{
# 428
struct _tuple11 _stmttmp9=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,v,loc,msg,0);struct _tuple11 _Tmp0=_stmttmp9;int _Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Scope new_scope=_Tmp2;int res_scope=_Tmp1;
if(!res_scope)res=0;{
# 431
struct Cyc_Absyn_Datatypedecl*d2;
# 434
{struct _tuple18 _stmttmpA=({struct _tuple18 _Tmp3;_Tmp3.f1=d0->fields,_Tmp3.f2=d1->fields;_Tmp3;});struct _tuple18 _Tmp3=_stmttmpA;void*_Tmp4;void*_Tmp5;if(_Tmp3.f2 == 0){
d2=d0;goto _LL3;}else{if(_Tmp3.f1 == 0){
d2=d1;goto _LL3;}else{_Tmp5=(struct Cyc_List_List*)(_Tmp3.f1)->v;_Tmp4=(struct Cyc_List_List*)(_Tmp3.f2)->v;{struct Cyc_List_List*f0s=_Tmp5;struct Cyc_List_List*f1s=_Tmp4;
# 448 "tcdecl.cyc"
struct Cyc_List_List*inst=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 451
if(d0->is_extensible){
# 455
int incl=1;
struct Cyc_List_List*f2s=Cyc_Tcdecl_merge_xdatatype_fields(f0s,f1s,inst,d0->tvs,d1->tvs,& res,& incl,t,v,loc,msg);
# 459
if(incl)
d2=d0;else{
# 462
d2=({struct Cyc_Absyn_Datatypedecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp6=*d0;_Tmp6;});
d2->sc=new_scope;
({struct Cyc_Core_Opt*_Tmp6=({struct Cyc_Core_Opt*_Tmp7=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp7->v=f2s;_Tmp7;});d2->fields=_Tmp6;});}}else{
# 468
for(1;f0s != 0 && f1s != 0;(f0s=f0s->tl,f1s=f1s->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);}
if(f0s != 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=t;_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=v;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=*(*((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name).f2;_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_Tcdecl_merr(loc,msg,({const char*_TmpA="%s %s is missing field %s";_tag_fat(_TmpA,sizeof(char),26U);}),_tag_fat(_Tmp9,sizeof(void*),3));});
res=0;}
# 474
if(f1s != 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=t;_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=v;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=*(*((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name).f2;_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_Tcdecl_merr(loc,msg,({const char*_TmpA="%s %s has extra field %s";_tag_fat(_TmpA,sizeof(char),25U);}),_tag_fat(_Tmp9,sizeof(void*),3));});
res=0;}
# 478
d2=d0;}
# 480
goto _LL3;}}}_LL3:;}
# 483
if(!res)return 0;
if((int)new_scope == (int)d2->sc)return d2;
d2=({struct Cyc_Absyn_Datatypedecl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));*_Tmp3=*d2;_Tmp3;});
d2->sc=new_scope;
return d2;}}}}
# 490
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned loc,struct _fat_ptr*msg){
# 492
struct _fat_ptr v=Cyc_Absynpp_qvar2string(d0->name);
int res=1;
# 496
struct _tuple11 _stmttmpB=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,({const char*_Tmp0="enum";_tag_fat(_Tmp0,sizeof(char),5U);}),v,loc,msg,1);struct _tuple11 _Tmp0=_stmttmpB;int _Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Scope new_scope=_Tmp2;int res_scope=_Tmp1;
if(!res_scope)res=0;{
# 499
struct Cyc_Absyn_Enumdecl*d2;
# 502
{struct _tuple18 _stmttmpC=({struct _tuple18 _Tmp3;_Tmp3.f1=d0->fields,_Tmp3.f2=d1->fields;_Tmp3;});struct _tuple18 _Tmp3=_stmttmpC;void*_Tmp4;void*_Tmp5;if(_Tmp3.f2 == 0){
d2=d0;goto _LL3;}else{if(_Tmp3.f1 == 0){
d2=d1;goto _LL3;}else{_Tmp5=(struct Cyc_List_List*)(_Tmp3.f1)->v;_Tmp4=(struct Cyc_List_List*)(_Tmp3.f2)->v;{struct Cyc_List_List*f0s=_Tmp5;struct Cyc_List_List*f1s=_Tmp4;
# 507
for(1;f0s != 0 && f1s != 0;(f0s=f0s->tl,f1s=f1s->tl)){
struct Cyc_Absyn_Enumfield*_stmttmpD=(struct Cyc_Absyn_Enumfield*)f0s->hd;struct Cyc_Absyn_Enumfield*_Tmp6=_stmttmpD;unsigned _Tmp7;void*_Tmp8;void*_Tmp9;_Tmp9=(_Tmp6->name)->f2;_Tmp8=_Tmp6->tag;_Tmp7=_Tmp6->loc;{struct _fat_ptr*fname0=_Tmp9;struct Cyc_Absyn_Exp*tag0=_Tmp8;unsigned loc0=_Tmp7;
struct Cyc_Absyn_Enumfield*_stmttmpE=(struct Cyc_Absyn_Enumfield*)f1s->hd;struct Cyc_Absyn_Enumfield*_TmpA=_stmttmpE;unsigned _TmpB;void*_TmpC;void*_TmpD;_TmpD=(_TmpA->name)->f2;_TmpC=_TmpA->tag;_TmpB=_TmpA->loc;{struct _fat_ptr*fname1=_TmpD;struct Cyc_Absyn_Exp*tag1=_TmpC;unsigned loc1=_TmpB;
# 511
if(Cyc_strptrcmp(fname0,fname1)!= 0){
({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=v;_TmpF;});struct Cyc_String_pa_PrintArg_struct _TmpF=({struct Cyc_String_pa_PrintArg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=*fname0;_Tmp10;});struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=*fname1;_Tmp11;});void*_Tmp11[3];_Tmp11[0]=& _TmpE,_Tmp11[1]=& _TmpF,_Tmp11[2]=& _Tmp10;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp12="enum %s: field name mismatch %s != %s";_tag_fat(_Tmp12,sizeof(char),38U);}),_tag_fat(_Tmp11,sizeof(void*),3));});
res=0;}
# 515
if(({unsigned _TmpE=Cyc_Tcdecl_get_uint_const_value(_check_null(tag0));_TmpE != Cyc_Tcdecl_get_uint_const_value(_check_null(tag1));})){
({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=v;_TmpF;});struct Cyc_String_pa_PrintArg_struct _TmpF=({struct Cyc_String_pa_PrintArg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=*fname1;_Tmp10;});void*_Tmp10[2];_Tmp10[0]=& _TmpE,_Tmp10[1]=& _TmpF;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp11="enum %s, field %s, value mismatch";_tag_fat(_Tmp11,sizeof(char),34U);}),_tag_fat(_Tmp10,sizeof(void*),2));});
res=0;}}}}
# 520
if(f0s != 0 || f1s != 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=v;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp8="enum %s declarations: number of fields differs";_tag_fat(_Tmp8,sizeof(char),47U);}),_tag_fat(_Tmp7,sizeof(void*),1));});
res=0;}
# 524
d2=d0;
goto _LL3;}}}_LL3:;}
# 528
if(!res)return 0;
if((int)d2->sc == (int)new_scope)return d2;
d2=({struct Cyc_Absyn_Enumdecl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));*_Tmp3=*d2;_Tmp3;});
d2->sc=new_scope;
return d2;}}}
# 535
static struct _tuple11 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){
# 539
int res=1;
# 542
struct _tuple11 _stmttmpF=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);struct _tuple11 _Tmp0=_stmttmpF;int _Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Scope new_scope=_Tmp2;int res_scope=_Tmp1;
if(!res_scope)res=0;
# 546
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _fat_ptr s1=Cyc_Absynpp_typ2string(t0);
struct _fat_ptr s2=Cyc_Absynpp_typ2string(t1);
if(({unsigned long _Tmp3=({unsigned long _Tmp4=({unsigned long _Tmp5=({unsigned long _Tmp6=Cyc_strlen(s1);_Tmp6 + Cyc_strlen(s2);});_Tmp5 + Cyc_strlen(t);});_Tmp4 + Cyc_strlen(v);});_Tmp3 + 
Cyc_strlen(({const char*_Tmp4="  was declared  instead of ";_tag_fat(_Tmp4,sizeof(char),28U);}));})> 70U)
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s2;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s1;_Tmp7;});void*_Tmp7[4];_Tmp7[0]=& _Tmp3,_Tmp7[1]=& _Tmp4,_Tmp7[2]=& _Tmp5,_Tmp7[3]=& _Tmp6;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp8="%s %s was declared \n\t%s\n instead of \n\t%s";_tag_fat(_Tmp8,sizeof(char),41U);}),_tag_fat(_Tmp7,sizeof(void*),4));});else{
# 554
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s2;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s1;_Tmp7;});void*_Tmp7[4];_Tmp7[0]=& _Tmp3,_Tmp7[1]=& _Tmp4,_Tmp7[2]=& _Tmp5,_Tmp7[3]=& _Tmp6;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp8="%s %s was declared %s instead of %s";_tag_fat(_Tmp8,sizeof(char),36U);}),_tag_fat(_Tmp7,sizeof(void*),4));});}
# 556
Cyc_Unify_explain_failure();
res=0;}
# 561
if(!Cyc_Absyn_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp6="%s %s has different type qualifiers";_tag_fat(_Tmp6,sizeof(char),36U);}),_tag_fat(_Tmp5,sizeof(void*),2));});
res=0;}
# 567
if(!Cyc_Atts_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=t;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=v;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp6="%s %s has different attributes";_tag_fat(_Tmp6,sizeof(char),31U);}),_tag_fat(_Tmp5,sizeof(void*),2));});
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\tprevious attributes: ";_tag_fat(_Tmp3,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));
for(1;atts0 != 0;atts0=atts0->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_attribute2string((void*)atts0->hd);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%s ";_tag_fat(_Tmp5,sizeof(char),4U);}),_tag_fat(_Tmp4,sizeof(void*),1));});}
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n\tcurrent attributes: ";_tag_fat(_Tmp3,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));
for(1;atts1 != 0;atts1=atts1->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_attribute2string((void*)atts1->hd);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%s ";_tag_fat(_Tmp5,sizeof(char),4U);}),_tag_fat(_Tmp4,sizeof(void*),1));});}
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
res=0;}
# 579
return({struct _tuple11 _Tmp3;_Tmp3.f1=new_scope,_Tmp3.f2=res;_Tmp3;});}}
# 582
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned loc,struct _fat_ptr*msg){
# 584
struct _fat_ptr v=Cyc_Absynpp_qvar2string(d0->name);
# 587
struct _tuple11 _stmttmp10=
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,({const char*_Tmp0="variable";_tag_fat(_Tmp0,sizeof(char),9U);}),v,loc,msg);
# 587
struct _tuple11 _Tmp0=_stmttmp10;int _Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Scope new_scope=_Tmp2;int res=_Tmp1;
# 592
if(!res)return 0;
if((int)d0->sc == (int)new_scope)return d0;{
struct Cyc_Absyn_Vardecl*d2=({struct Cyc_Absyn_Vardecl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));*_Tmp3=*d0;_Tmp3;});
d2->sc=new_scope;
return d2;}}}
# 599
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned loc,struct _fat_ptr*msg){
# 601
struct _fat_ptr v=Cyc_Absynpp_qvar2string(d0->name);
# 604
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_Tmp0="typedef";_tag_fat(_Tmp0,sizeof(char),8U);}),v,loc,msg))return 0;{
# 607
struct Cyc_List_List*inst=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 610
if(d0->defn != 0 && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(inst,_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type(_check_null(d0->defn),subst_defn1)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=v;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_Absynpp_typ2string(subst_defn1);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_typ2string(_check_null(d0->defn));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp4="typedef %s does not refer to the same type: %s != %s";_tag_fat(_Tmp4,sizeof(char),53U);}),_tag_fat(_Tmp3,sizeof(void*),3));});
return 0;}}
# 618
return d0;}}
# 621
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned loc,struct _fat_ptr*msg){
# 623
struct _tuple15 _stmttmp11=({struct _tuple15 _Tmp0;_Tmp0.f1=b0,_Tmp0.f2=b1;_Tmp0;});struct _tuple15 _Tmp0=_stmttmp11;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0.f1)){case 1: switch(*((int*)_Tmp0.f2)){case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Vardecl*d0=_Tmp2;struct Cyc_Absyn_Vardecl*d1=_Tmp1;
# 625
struct Cyc_Absyn_Vardecl*d2=Cyc_Tcdecl_merge_vardecl(d0,d1,loc,msg);
if(d2 == 0)return 0;
if(d2 == d0)return b0;
if(d2 == d1)return b1;
return(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_Tmp3->tag=1,_Tmp3->f1=d2;_Tmp3;});}case 2: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Vardecl*d0=_Tmp2;struct Cyc_Absyn_Fndecl*d1=_Tmp1;
# 631
struct _tuple11 _stmttmp12=({enum Cyc_Absyn_Scope _Tmp3=d0->sc;void*_Tmp4=d0->type;struct Cyc_Absyn_Tqual _Tmp5=d0->tq;struct Cyc_List_List*_Tmp6=d0->attributes;enum Cyc_Absyn_Scope _Tmp7=d1->sc;void*_Tmp8=
_check_null(d1->cached_type);
# 631
struct Cyc_Absyn_Tqual _Tmp9=
Cyc_Absyn_empty_tqual(0U);
# 631
struct Cyc_List_List*_TmpA=(d1->i).attributes;struct _fat_ptr _TmpB=({const char*_TmpC="function";_tag_fat(_TmpC,sizeof(char),9U);});struct _fat_ptr _TmpC=
# 634
Cyc_Absynpp_qvar2string(d0->name);
# 631
unsigned _TmpD=loc;Cyc_Tcdecl_check_var_or_fn_decl(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,_TmpB,_TmpC,_TmpD,msg);});struct _tuple11 _Tmp3=_stmttmp12;int _Tmp4;_Tmp4=_Tmp3.f2;{int res=_Tmp4;
# 635
if(!res)return 0;
d0->is_proto=1;
return b1;}}default: goto _LL9;}case 2: switch(*((int*)_Tmp0.f2)){case 2: _Tmp2=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Fndecl*d1=_Tmp2;
# 639
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(d1->name);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Tcdecl_merr(loc,msg,({const char*_Tmp5="redefinition of function %s";_tag_fat(_Tmp5,sizeof(char),28U);}),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0;}case 1: _Tmp2=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Fndecl*d0=_Tmp2;struct Cyc_Absyn_Vardecl*d1=_Tmp1;
# 642
struct _tuple11 _stmttmp13=({enum Cyc_Absyn_Scope _Tmp3=d0->sc;void*_Tmp4=_check_null(d0->cached_type);struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);struct Cyc_List_List*_Tmp6=(d0->i).attributes;enum Cyc_Absyn_Scope _Tmp7=d1->sc;void*_Tmp8=d1->type;struct Cyc_Absyn_Tqual _Tmp9=d1->tq;struct Cyc_List_List*_TmpA=d1->attributes;struct _fat_ptr _TmpB=({const char*_TmpC="variable";_tag_fat(_TmpC,sizeof(char),9U);});struct _fat_ptr _TmpC=
# 645
Cyc_Absynpp_qvar2string(d0->name);
# 642
unsigned _TmpD=loc;Cyc_Tcdecl_check_var_or_fn_decl(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,_TmpB,_TmpC,_TmpD,msg);});struct _tuple11 _Tmp3=_stmttmp13;int _Tmp4;_Tmp4=_Tmp3.f2;{int res=_Tmp4;
# 646
if(!res)return 0;
d1->is_proto=1;
return b0;}}default: goto _LL9;}default: _LL9:
 return 0;};}
# 653
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned loc,struct _fat_ptr*msg){
# 656
struct Cyc_Tcdecl_Xdatatypefielddecl*_Tmp0=d0;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->base;_Tmp1=_Tmp0->field;{struct Cyc_Absyn_Datatypedecl*b0=_Tmp2;struct Cyc_Absyn_Datatypefield*f0=_Tmp1;
struct Cyc_Tcdecl_Xdatatypefielddecl*_Tmp3=d1;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3->base;_Tmp4=_Tmp3->field;{struct Cyc_Absyn_Datatypedecl*b1=_Tmp5;struct Cyc_Absyn_Datatypefield*f1=_Tmp4;
struct _fat_ptr v=Cyc_Absynpp_qvar2string(f0->name);
# 661
if(Cyc_Absyn_qvar_cmp(b0->name,b1->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_Absynpp_qvar2string(b0->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_qvar2string(b1->name);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=v;_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_Tcdecl_merr(loc,msg,({const char*_TmpA="@extensible datatype %s and %s both have a field named %s";_tag_fat(_TmpA,sizeof(char),58U);}),_tag_fat(_Tmp9,sizeof(void*),3));});
return 0;}
# 668
if(!({struct Cyc_List_List*_Tmp6=b0->tvs;struct Cyc_List_List*_Tmp7=b1->tvs;struct _fat_ptr _Tmp8=({const char*_Tmp9="@extensible datatype";_tag_fat(_Tmp9,sizeof(char),21U);});struct _fat_ptr _Tmp9=Cyc_Absynpp_qvar2string(b0->name);unsigned _TmpA=loc;Cyc_Tcdecl_check_tvs(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,msg);}))
return 0;{
# 671
struct Cyc_List_List*inst1=Cyc_Tcdecl_build_tvs_map(b0->tvs,b1->tvs);
# 674
struct Cyc_Absyn_Datatypefield*f2=Cyc_Tcdecl_merge_datatypefield(f0,f1,inst1,({const char*_Tmp6="@extensible datatype field";_tag_fat(_Tmp6,sizeof(char),27U);}),v,msg);
if(f2 == 0)return 0;
if(f2 == f0)return d0;
return({struct Cyc_Tcdecl_Xdatatypefielddecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Tcdecl_Xdatatypefielddecl));_Tmp6->base=b0,_Tmp6->field=f2;_Tmp6;});}}}}
