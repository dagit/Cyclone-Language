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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
extern int Cyc_fputc(int,struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 278 "cycboot.h"
extern int Cyc_file_string_write(struct Cyc___cycFILE*,struct _fat_ptr,int,int);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 662 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 863
void*Cyc_Absyn_compress(void*);
# 867
int Cyc_Absyn_type2bool(int,void*);
# 876
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1118
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1126
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 62
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 80
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual,void*);struct _tuple13{int f1;struct Cyc_List_List*f2;};
# 95 "absynpp.h"
struct _tuple13 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*,struct Cyc_List_List*);
# 25 "attributes.h"
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Atts_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Atts_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Atts_Fastcall_att_val;
# 33
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Atts_Unused_att_val;struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 78 "kinds.h"
void*Cyc_Kinds_compress_kb(void*);
# 78 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 82
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct _tuple15{struct _fat_ptr f1;unsigned f2;};
# 28 "absyndump.cyc"
extern struct _tuple15 Cyc_Lex_xlate_pos(unsigned);
# 37
static int Cyc_Absyndump_expand_typedefs;
# 41
static int Cyc_Absyndump_qvar_to_Cids;
# 44
static int Cyc_Absyndump_add_cyc_prefix;
# 47
static int Cyc_Absyndump_generate_line_directives;
# 51
static int Cyc_Absyndump_to_VC;static char _tmp0[14U]="__NoCycPrefix";
# 53
static struct _fat_ptr Cyc_Absyndump_nocyc_str={_tmp0,_tmp0,_tmp0 + 14U};
static struct _fat_ptr*Cyc_Absyndump_nocyc_strptr=& Cyc_Absyndump_nocyc_str;
# 56
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 71 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 74
static void Cyc_Absyndump_dumptyp(void*);
static void Cyc_Absyndump_dumpntyp(void*);
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int,struct Cyc_List_List*);
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned,int);
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
static void Cyc_Absyndump_dumptms(int,struct Cyc_List_List*,void(*)(void*),void*);
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*)(void*),void*);
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*);
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*);
# 93
static void Cyc_Absyndump_dumploc(unsigned);
# 96
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
static char Cyc_Absyndump_prev_char='x';
# 99
static int Cyc_Absyndump_need_space_before (void){
char _tmp1=Cyc_Absyndump_prev_char;switch((int)_tmp1){case 123:
 goto _LL4;case 125: _LL4:
 goto _LL6;case 40: _LL6:
 goto _LL8;case 41: _LL8:
 goto _LLA;case 91: _LLA:
 goto _LLC;case 93: _LLC:
 goto _LLE;case 59: _LLE:
 goto _LL10;case 44: _LL10:
 goto _LL12;case 61: _LL12:
 goto _LL14;case 63: _LL14:
 goto _LL16;case 33: _LL16:
 goto _LL18;case 32: _LL18:
 goto _LL1A;case 10: _LL1A:
 goto _LL1C;case 42: _LL1C:
 return 0;default:
 return 1;};}
# 119
static void Cyc_Absyndump_dump(struct _fat_ptr s){
unsigned sz=Cyc_strlen((struct _fat_ptr)s);
# 126
if(Cyc_Absyndump_need_space_before())
Cyc_fputc(32,*Cyc_Absyndump_dump_file);
if(sz >= 1U){
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),(int)(sz - 1U)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 134
static void Cyc_Absyndump_dump_nospace(struct _fat_ptr s){
int sz=(int)Cyc_strlen((struct _fat_ptr)s);
# 137
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),sz - 1));}}
# 142
static void Cyc_Absyndump_dump_char(int c){
# 144
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpF[1U]="";
# 148
static void Cyc_Absyndump_dumploc(unsigned loc){
static struct _fat_ptr last_file={_tmpF,_tmpF,_tmpF + 1U};
static unsigned last_line=0U;
if(loc == 0U)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 154
struct _tuple15 _tmp2=Cyc_Lex_xlate_pos(loc);struct _tuple15 _stmttmp0=_tmp2;struct _tuple15 _tmp3=_stmttmp0;unsigned _tmp5;struct _fat_ptr _tmp4;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{struct _fat_ptr f=_tmp4;unsigned d=_tmp5;
if((char*)f.curr == (char*)(_tag_fat(0,0,0)).curr ||(char*)f.curr == (char*)last_file.curr && d == last_line)return;
if((char*)f.curr == (char*)last_file.curr && d == last_line + 1U)
({void*_tmp6=0U;({struct Cyc___cycFILE*_tmp244=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp243=({const char*_tmp7="\n";_tag_fat(_tmp7,sizeof(char),2U);});Cyc_fprintf(_tmp244,_tmp243,_tag_fat(_tmp6,sizeof(void*),0));});});else{
if((char*)f.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp235;_tmp235.tag=1,_tmp235.f1=(unsigned long)((int)d);_tmp235;});void*_tmp8[1];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp246=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp245=({const char*_tmp9="\n# %d\n";_tag_fat(_tmp9,sizeof(char),7U);});Cyc_fprintf(_tmp246,_tmp245,_tag_fat(_tmp8,sizeof(void*),1));});});else{
# 161
({struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp237;_tmp237.tag=1,_tmp237.f1=(unsigned long)((int)d);_tmp237;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp236;_tmp236.tag=0,_tmp236.f1=(struct _fat_ptr)((struct _fat_ptr)f);_tmp236;});void*_tmpB[2];_tmpB[0]=& _tmpD,_tmpB[1]=& _tmpE;({struct Cyc___cycFILE*_tmp248=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp247=({const char*_tmpC="\n# %d %s\n";_tag_fat(_tmpC,sizeof(char),10U);});Cyc_fprintf(_tmp248,_tmp247,_tag_fat(_tmpB,sizeof(void*),2));});});}}
# 163
last_file=f;
last_line=d;}}}
# 169
static void Cyc_Absyndump_dump_str(struct _fat_ptr*s){
Cyc_Absyndump_dump(*s);}
# 172
static void Cyc_Absyndump_dump_str_nospace(struct _fat_ptr*s){
Cyc_Absyndump_dump_nospace(*s);}
# 176
static void Cyc_Absyndump_dump_semi (void){
Cyc_Absyndump_dump_char(59);}
# 180
static void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(1;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(1;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
static void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
static void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
static void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 212
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 216
static void Cyc_Absyndump_ignore(void*x){;}
# 218
static void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmp10=0;struct Cyc_List_List*nsl=_tmp10;
struct _fat_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp11=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmp11;union Cyc_Absyn_Nmspace _tmp12=_stmttmp1;void*_tmp13;switch((_tmp12.C_n).tag){case 4:
 goto _LL0;case 1: _tmp13=(_tmp12.Rel_n).val;{struct Cyc_List_List*x=_tmp13;
nsl=x;goto _LL0;}case 3: _tmp13=(_tmp12.C_n).val;{struct Cyc_List_List*x=_tmp13;
goto _LL0;}default: _tmp13=(_tmp12.Abs_n).val;{struct Cyc_List_List*x=_tmp13;
# 226
if(Cyc_Absyndump_qvar_to_Cids || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
nsl=x;
goto _LL0;}}_LL0:;}
# 231
if(Cyc_Absyndump_need_space_before())
Cyc_Absyndump_dump_char(32);{
struct _fat_ptr sep=Cyc_Absyndump_qvar_to_Cids?({const char*_tmp14="_";_tag_fat(_tmp14,sizeof(char),2U);}):({const char*_tmp15="::";_tag_fat(_tmp15,sizeof(char),3U);});
if((unsigned)nsl && !Cyc_strptrcmp((struct _fat_ptr*)nsl->hd,Cyc_Absyndump_nocyc_strptr)){
nsl=nsl->tl;
prefix=0;}
# 238
if(prefix != 0){
Cyc_Absyndump_dump_nospace(*(*prefix));Cyc_Absyndump_dump_nospace(sep);}
# 241
if(nsl != 0){
({(void(*)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;})(Cyc_Absyndump_dump_str_nospace,nsl,sep);Cyc_Absyndump_dump_nospace(sep);}
# 244
Cyc_Absyndump_dump_nospace(*(*v).f2);}}
# 247
static void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp16="restrict";_tag_fat(_tmp16,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp17="volatile";_tag_fat(_tmp17,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp18="const";_tag_fat(_tmp18,sizeof(char),6U);}));}
# 252
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp19=sc;switch((int)_tmp19){case Cyc_Absyn_Static:
 Cyc_Absyndump_dump(({const char*_tmp1A="static";_tag_fat(_tmp1A,sizeof(char),7U);}));return;case Cyc_Absyn_Public:
 return;case Cyc_Absyn_Extern:
 Cyc_Absyndump_dump(({const char*_tmp1B="extern";_tag_fat(_tmp1B,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC:
 Cyc_Absyndump_dump(({const char*_tmp1C="extern \"C\"";_tag_fat(_tmp1C,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract:
 Cyc_Absyndump_dump(({const char*_tmp1D="abstract";_tag_fat(_tmp1D,sizeof(char),9U);}));return;case Cyc_Absyn_Register:
 Cyc_Absyndump_dump(({const char*_tmp1E="register";_tag_fat(_tmp1E,sizeof(char),9U);}));return;default:
 return;};}
# 263
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp1F=k;if(_tmp1F == Cyc_Absyn_StructA){
Cyc_Absyndump_dump(({const char*_tmp20="struct";_tag_fat(_tmp20,sizeof(char),7U);}));return;}else{
Cyc_Absyndump_dump(({const char*_tmp21="union";_tag_fat(_tmp21,sizeof(char),6U);}));return;};}
# 270
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
Cyc_Absyndump_dump(Cyc_Absynpp_kind2string(ka));}
# 274
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({void(*_tmp24C)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp24B=ts;struct _fat_ptr _tmp24A=({const char*_tmp22="<";_tag_fat(_tmp22,sizeof(char),2U);});struct _fat_ptr _tmp249=({const char*_tmp23=">";_tag_fat(_tmp23,sizeof(char),2U);});_tmp24C(Cyc_Absyndump_dumptyp,_tmp24B,_tmp24A,_tmp249,({const char*_tmp24=",";_tag_fat(_tmp24,sizeof(char),2U);}));});}
# 277
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
if((int)*((const char*)_check_fat_subscript(n,sizeof(char),0))== 35){
Cyc_Absyndump_dump(({const char*_tmp25="`G";_tag_fat(_tmp25,sizeof(char),3U);}));
{void*_tmp26=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp2=_tmp26;void*_tmp27=_stmttmp2;void*_tmp28;switch(*((int*)_tmp27)){case 0: _tmp28=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp27)->f1;{struct Cyc_Absyn_Kind*k=_tmp28;
_tmp28=k;goto _LL4;}case 2: _tmp28=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp27)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp28;
Cyc_Absyndump_dumpkind(k);goto _LL0;}default:
 Cyc_Absyndump_dump_nospace(({const char*_tmp29="K";_tag_fat(_tmp29,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 286
Cyc_Absyndump_dump_nospace(_fat_ptr_plus(n,sizeof(char),1));}else{
# 288
Cyc_Absyndump_dump(n);}}
# 290
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp2A=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp3=_tmp2A;void*_tmp2B=_stmttmp3;void*_tmp2C;switch(*((int*)_tmp2B)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2B)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2B)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp2C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2B)->f1;{struct Cyc_Absyn_Kind*k=_tmp2C;
Cyc_Absyndump_dump(({const char*_tmp2D="::";_tag_fat(_tmp2D,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(k);goto _LL0;}}}_LL0:;}}
# 299
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({void(*_tmp250)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp24F=tvs;struct _fat_ptr _tmp24E=({const char*_tmp2E="<";_tag_fat(_tmp2E,sizeof(char),2U);});struct _fat_ptr _tmp24D=({const char*_tmp2F=">";_tag_fat(_tmp2F,sizeof(char),2U);});_tmp250(Cyc_Absyndump_dumptvar,_tmp24F,_tmp24E,_tmp24D,({const char*_tmp30=",";_tag_fat(_tmp30,sizeof(char),2U);}));});}
# 302
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({void(*_tmp254)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp253=tvs;struct _fat_ptr _tmp252=({const char*_tmp31="<";_tag_fat(_tmp31,sizeof(char),2U);});struct _fat_ptr _tmp251=({const char*_tmp32=">";_tag_fat(_tmp32,sizeof(char),2U);});_tmp254(Cyc_Absyndump_dumpkindedtvar,_tmp253,_tmp252,_tmp251,({const char*_tmp33=",";_tag_fat(_tmp33,sizeof(char),2U);}));});}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 305
static void Cyc_Absyndump_dumparg(struct _tuple16*pr){
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int))Cyc_Absyndump_dumptqtd;})((*pr).f1,(*pr).f2,({(void(*)(int))Cyc_Absyndump_ignore;}),0);}
# 308
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({void(*_tmp258)(void(*)(struct _tuple16*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple16*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp257=ts;struct _fat_ptr _tmp256=({const char*_tmp34="(";_tag_fat(_tmp34,sizeof(char),2U);});struct _fat_ptr _tmp255=({const char*_tmp35=")";_tag_fat(_tmp35,sizeof(char),2U);});_tmp258(Cyc_Absyndump_dumparg,_tmp257,_tmp256,_tmp255,({const char*_tmp36=",";_tag_fat(_tmp36,sizeof(char),2U);}));});}
# 312
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(1;atts != 0;atts=atts->tl){
void*_tmp37=(void*)atts->hd;void*_stmttmp4=_tmp37;void*_tmp38=_stmttmp4;switch(*((int*)_tmp38)){case 1:
 Cyc_Absyndump_dump(({const char*_tmp39="_stdcall";_tag_fat(_tmp39,sizeof(char),9U);}));return;case 2:
 Cyc_Absyndump_dump(({const char*_tmp3A="_cdecl";_tag_fat(_tmp3A,sizeof(char),7U);}));return;case 3:
 Cyc_Absyndump_dump(({const char*_tmp3B="_fastcall";_tag_fat(_tmp3B,sizeof(char),10U);}));return;default:
 goto _LL0;}_LL0:;}}
# 322
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 324
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp3C=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_stmttmp5=_tmp3C;void*_tmp3D=_stmttmp5;switch(*((int*)_tmp3D)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 332
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp3E="__declspec(";_tag_fat(_tmp3E,sizeof(char),12U);}));
for(1;atts != 0;atts=atts->tl){
void*_tmp3F=(void*)atts->hd;void*_stmttmp6=_tmp3F;void*_tmp40=_stmttmp6;switch(*((int*)_tmp40)){case 1:
 goto _LLD;case 2: _LLD:
 goto _LLF;case 3: _LLF:
 goto _LL9;default:
 Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 342
Cyc_Absyndump_dump_char(41);}
# 345
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Flags_C_Compilers _tmp41=Cyc_Flags_c_compiler;if(_tmp41 == Cyc_Flags_Gcc_c){
# 349
Cyc_Absyndump_dump(({const char*_tmp42=" __attribute__((";_tag_fat(_tmp42,sizeof(char),17U);}));
for(1;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp43=",";_tag_fat(_tmp43,sizeof(char),2U);}));}
# 354
Cyc_Absyndump_dump(({const char*_tmp44=")) ";_tag_fat(_tmp44,sizeof(char),4U);}));
return;}else{
# 357
Cyc_Absyndump_dump_noncallconv(atts);
return;};}}
# 362
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 366
static struct _tuple17 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_tmp45=Cyc_Absyn_compress(t);void*_stmttmp7=_tmp45;void*_tmp46=_stmttmp7;void*_tmp47;if(*((int*)_tmp46)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2 != 0){_tmp47=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2)->hd;{void*r=_tmp47;
rgions=({struct Cyc_List_List*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->hd=r,_tmp48->tl=rgions;_tmp48;});goto _LL0;}}else{goto _LL5;}case 9: _tmp47=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46)->f2;{struct Cyc_List_List*ts=_tmp47;
# 372
for(1;ts != 0;ts=ts->tl){
struct _tuple17 _tmp49=Cyc_Absyndump_effects_split((void*)ts->hd);struct _tuple17 _stmttmp8=_tmp49;struct _tuple17 _tmp4A=_stmttmp8;void*_tmp4C;void*_tmp4B;_tmp4B=_tmp4A.f1;_tmp4C=_tmp4A.f2;{struct Cyc_List_List*rs=_tmp4B;struct Cyc_List_List*es=_tmp4C;
rgions=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;})(rs,rgions);
effects=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;})(es,effects);}}
# 377
goto _LL0;}default: goto _LL5;}else{_LL5:
 effects=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=t,_tmp4D->tl=effects;_tmp4D;});goto _LL0;}_LL0:;}
# 380
return({struct _tuple17 _tmp238;_tmp238.f1=rgions,_tmp238.f2=effects;_tmp238;});}
# 383
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple17 _tmp4E=Cyc_Absyndump_effects_split(t);struct _tuple17 _stmttmp9=_tmp4E;struct _tuple17 _tmp4F=_stmttmp9;void*_tmp51;void*_tmp50;_tmp50=_tmp4F.f1;_tmp51=_tmp4F.f2;{struct Cyc_List_List*rgions=_tmp50;struct Cyc_List_List*effects=_tmp51;
rgions=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(rgions);
effects=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(effects);
if(effects != 0){
({void(*_tmp25C)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp25B=effects;struct _fat_ptr _tmp25A=({const char*_tmp52="";_tag_fat(_tmp52,sizeof(char),1U);});struct _fat_ptr _tmp259=({const char*_tmp53="";_tag_fat(_tmp53,sizeof(char),1U);});_tmp25C(Cyc_Absyndump_dumpntyp,_tmp25B,_tmp25A,_tmp259,({const char*_tmp54="+";_tag_fat(_tmp54,sizeof(char),2U);}));});
if(rgions != 0)Cyc_Absyndump_dump_char(43);}
# 391
if(rgions != 0 || effects == 0)
({void(*_tmp260)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp25F=rgions;struct _fat_ptr _tmp25E=({const char*_tmp55="{";_tag_fat(_tmp55,sizeof(char),2U);});struct _fat_ptr _tmp25D=({const char*_tmp56="}";_tag_fat(_tmp56,sizeof(char),2U);});_tmp260(Cyc_Absyndump_dumprgn,_tmp25F,_tmp25E,_tmp25D,({const char*_tmp57=",";_tag_fat(_tmp57,sizeof(char),2U);}));});}}
# 396
static void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp58=t;enum Cyc_Absyn_AggrKind _tmp5B;int _tmp5C;void*_tmp5A;void*_tmp59;switch(*((int*)_tmp58)){case 4:
# 399
 goto _LL4;case 5: _LL4:
 goto _LL6;case 3: _LL6:
 return;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58)->f1)== 0){
Cyc_Absyndump_dump(({const char*_tmp5D="void";_tag_fat(_tmp5D,sizeof(char),5U);}));return;}else{_tmp59=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58)->f1;_tmp5A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58)->f2;{void*c=_tmp59;struct Cyc_List_List*ts=_tmp5A;
# 419
void*_tmp67=c;struct _fat_ptr _tmp68;void*_tmp69;union Cyc_Absyn_AggrInfo _tmp6A;int _tmp6B;union Cyc_Absyn_DatatypeFieldInfo _tmp6C;union Cyc_Absyn_DatatypeInfo _tmp6D;switch(*((int*)_tmp67)){case 18: _tmp6D=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp67)->f1;{union Cyc_Absyn_DatatypeInfo tu_info=_tmp6D;
# 421
{union Cyc_Absyn_DatatypeInfo _tmp6E=tu_info;int _tmp70;void*_tmp6F;if((_tmp6E.UnknownDatatype).tag == 1){_tmp6F=((_tmp6E.UnknownDatatype).val).name;_tmp70=((_tmp6E.UnknownDatatype).val).is_extensible;{struct _tuple0*n=_tmp6F;int is_x=_tmp70;
_tmp6F=n;_tmp70=is_x;goto _LL6C;}}else{_tmp6F=(*(_tmp6E.KnownDatatype).val)->name;_tmp70=(*(_tmp6E.KnownDatatype).val)->is_extensible;_LL6C: {struct _tuple0*n=_tmp6F;int is_x=_tmp70;
# 424
if(is_x)Cyc_Absyndump_dump(({const char*_tmp71="@extensible";_tag_fat(_tmp71,sizeof(char),12U);}));
Cyc_Absyndump_dump(({const char*_tmp72="datatype";_tag_fat(_tmp72,sizeof(char),9U);}));Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
goto _LL68;}}_LL68:;}
# 428
return;}case 19: _tmp6C=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp67)->f1;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp6C;
# 430
{union Cyc_Absyn_DatatypeFieldInfo _tmp73=tuf_info;int _tmp75;void*_tmp76;void*_tmp74;if((_tmp73.UnknownDatatypefield).tag == 1){_tmp74=((_tmp73.UnknownDatatypefield).val).datatype_name;_tmp76=((_tmp73.UnknownDatatypefield).val).field_name;_tmp75=((_tmp73.UnknownDatatypefield).val).is_extensible;{struct _tuple0*tname=_tmp74;struct _tuple0*fname=_tmp76;int is_x=_tmp75;
# 432
_tmp74=tname;_tmp75=is_x;_tmp76=fname;goto _LL71;}}else{_tmp74=(((_tmp73.KnownDatatypefield).val).f1)->name;_tmp75=(((_tmp73.KnownDatatypefield).val).f1)->is_extensible;_tmp76=(((_tmp73.KnownDatatypefield).val).f2)->name;_LL71: {struct _tuple0*tname=_tmp74;int is_x=_tmp75;struct _tuple0*fname=_tmp76;
# 435
if(is_x)Cyc_Absyndump_dump(({const char*_tmp77="@extensible ";_tag_fat(_tmp77,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp78="datatype ";_tag_fat(_tmp78,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(tname);Cyc_Absyndump_dump(({const char*_tmp79=".";_tag_fat(_tmp79,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(fname);
Cyc_Absyndump_dumptps(ts);
goto _LL6D;}}_LL6D:;}
# 440
return;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp67)->f1){case Cyc_Absyn_None: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp67)->f2){case Cyc_Absyn_Int_sz:
 goto _LL29;case Cyc_Absyn_Long_sz:
# 443
 goto _LL2D;case Cyc_Absyn_Char_sz:
# 445
 Cyc_Absyndump_dump(({const char*_tmp7C="char";_tag_fat(_tmp7C,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz:
# 448
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL41:
# 454
 goto _LL43;default: goto _LL66;}case Cyc_Absyn_Signed: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp67)->f2){case Cyc_Absyn_Int_sz: _LL29:
# 442
 Cyc_Absyndump_dump(({const char*_tmp7A="int";_tag_fat(_tmp7A,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL2D:
# 444
 Cyc_Absyndump_dump(({const char*_tmp7B="long";_tag_fat(_tmp7B,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz:
# 446
 Cyc_Absyndump_dump(({const char*_tmp7D="signed char";_tag_fat(_tmp7D,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL37:
# 449
 Cyc_Absyndump_dump(({const char*_tmp7F="short";_tag_fat(_tmp7F,sizeof(char),6U);}));return;case Cyc_Absyn_LongLong_sz: _LL43: {
# 456
enum Cyc_Flags_C_Compilers _tmp84=Cyc_Flags_c_compiler;if(_tmp84 == Cyc_Flags_Gcc_c){
Cyc_Absyndump_dump(({const char*_tmp85="long long";_tag_fat(_tmp85,sizeof(char),10U);}));return;}else{
Cyc_Absyndump_dump(({const char*_tmp86="__int64";_tag_fat(_tmp86,sizeof(char),8U);}));return;};}default: goto _LL66;}case Cyc_Absyn_Unsigned: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp67)->f2){case Cyc_Absyn_Char_sz:
# 447
 Cyc_Absyndump_dump(({const char*_tmp7E="unsigned char";_tag_fat(_tmp7E,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz:
# 450
 Cyc_Absyndump_dump(({const char*_tmp80="unsigned short";_tag_fat(_tmp80,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz:
 Cyc_Absyndump_dump(({const char*_tmp81="unsigned";_tag_fat(_tmp81,sizeof(char),9U);}));return;case Cyc_Absyn_Long_sz:
 Cyc_Absyndump_dump(({const char*_tmp82="unsigned long";_tag_fat(_tmp82,sizeof(char),14U);}));return;case Cyc_Absyn_LongLong_sz:
 Cyc_Absyndump_dump(({const char*_tmp83="unsigned";_tag_fat(_tmp83,sizeof(char),9U);}));goto _LL41;default: goto _LL66;}default: goto _LL66;}case 2: _tmp6B=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp67)->f1;{int i=_tmp6B;
# 461
int _tmp87=i;switch((int)_tmp87){case 0:
 Cyc_Absyndump_dump(({const char*_tmp88="float";_tag_fat(_tmp88,sizeof(char),6U);}));return;case 1:
 Cyc_Absyndump_dump(({const char*_tmp89="double";_tag_fat(_tmp89,sizeof(char),7U);}));return;default:
 Cyc_Absyndump_dump(({const char*_tmp8A="long double";_tag_fat(_tmp8A,sizeof(char),12U);}));return;};}case 20: _tmp6A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp67)->f1;{union Cyc_Absyn_AggrInfo info=_tmp6A;
# 467
struct _tuple11 _tmp8B=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmpA=_tmp8B;struct _tuple11 _tmp8C=_stmttmpA;void*_tmp8E;enum Cyc_Absyn_AggrKind _tmp8D;_tmp8D=_tmp8C.f1;_tmp8E=_tmp8C.f2;{enum Cyc_Absyn_AggrKind k=_tmp8D;struct _tuple0*n=_tmp8E;
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
return;}}case 15: _tmp69=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp67)->f1;{struct _tuple0*n=_tmp69;
Cyc_Absyndump_dump(({const char*_tmp8F="enum";_tag_fat(_tmp8F,sizeof(char),5U);}));Cyc_Absyndump_dumpqvar(n);return;}case 16: _tmp69=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp67)->f1;{struct Cyc_List_List*fs=_tmp69;
Cyc_Absyndump_dump(({const char*_tmp90="enum{";_tag_fat(_tmp90,sizeof(char),6U);}));Cyc_Absyndump_dumpenumfields(fs);Cyc_Absyndump_dump(({const char*_tmp91="}";_tag_fat(_tmp91,sizeof(char),2U);}));return;}case 17: _tmp68=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp67)->f1;{struct _fat_ptr t=_tmp68;
Cyc_Absyndump_dump(t);return;}case 3:
 Cyc_Absyndump_dump(({const char*_tmp92="region_t<";_tag_fat(_tmp92,sizeof(char),10U);}));Cyc_Absyndump_dumprgn((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmp93=">";_tag_fat(_tmp93,sizeof(char),2U);}));return;case 4:
 Cyc_Absyndump_dump(({const char*_tmp94="tag_t<";_tag_fat(_tmp94,sizeof(char),7U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmp95=">";_tag_fat(_tmp95,sizeof(char),2U);}));return;case 6:
 Cyc_Absyndump_dump(({const char*_tmp96="`U";_tag_fat(_tmp96,sizeof(char),3U);}));return;case 7:
 Cyc_Absyndump_dump(({const char*_tmp97="`RC";_tag_fat(_tmp97,sizeof(char),4U);}));return;case 5:
 Cyc_Absyndump_dump(({const char*_tmp98="`H";_tag_fat(_tmp98,sizeof(char),3U);}));return;case 8:
 Cyc_Absyndump_dump(({const char*_tmp99="{";_tag_fat(_tmp99,sizeof(char),2U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmp9A="}";_tag_fat(_tmp9A,sizeof(char),2U);}));return;case 10:
 Cyc_Absyndump_dump(({const char*_tmp9B="regions(";_tag_fat(_tmp9B,sizeof(char),9U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmp9C=")";_tag_fat(_tmp9C,sizeof(char),2U);}));return;case 9:
({void(*_tmp264)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp263=ts;struct _fat_ptr _tmp262=({const char*_tmp9D="";_tag_fat(_tmp9D,sizeof(char),1U);});struct _fat_ptr _tmp261=({const char*_tmp9E="";_tag_fat(_tmp9E,sizeof(char),1U);});_tmp264(Cyc_Absyndump_dumptyp,_tmp263,_tmp262,_tmp261,({const char*_tmp9F="+";_tag_fat(_tmp9F,sizeof(char),2U);}));});return;case 11:
 Cyc_Absyndump_dump(({const char*_tmpA0="@true";_tag_fat(_tmpA0,sizeof(char),6U);}));return;case 12:
 Cyc_Absyndump_dump(({const char*_tmpA1="@false";_tag_fat(_tmpA1,sizeof(char),7U);}));return;case 13:
 Cyc_Absyndump_dump(({const char*_tmpA2="@thin";_tag_fat(_tmpA2,sizeof(char),6U);}));return;case 14:
 Cyc_Absyndump_dump(({const char*_tmpA3="@fat";_tag_fat(_tmpA3,sizeof(char),5U);}));return;default: _LL66:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp239;_tmp239.tag=0,({struct _fat_ptr _tmp265=({const char*_tmpA6="bad type constructor";_tag_fat(_tmpA6,sizeof(char),21U);});_tmp239.f1=_tmp265;});_tmp239;});void*_tmpA4[1];_tmpA4[0]=& _tmpA5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmpA4,sizeof(void*),1));});};}}case 2: _tmp59=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp58)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp59;
# 403
Cyc_Absyndump_dumptvar(tv);return;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58)->f2 == 0){_tmp59=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58)->f1;_tmp5C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58)->f3;{struct Cyc_Core_Opt*k=_tmp59;int i=_tmp5C;
# 405
Cyc_Absyndump_dump(({const char*_tmp5E="`E";_tag_fat(_tmp5E,sizeof(char),3U);}));
if(k == 0)Cyc_Absyndump_dump(({const char*_tmp5F="K";_tag_fat(_tmp5F,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)k->v);}
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp62=({struct Cyc_Int_pa_PrintArg_struct _tmp23A;_tmp23A.tag=1,_tmp23A.f1=(unsigned long)i;_tmp23A;});void*_tmp60[1];_tmp60[0]=& _tmp62;({struct _fat_ptr _tmp266=({const char*_tmp61="%d";_tag_fat(_tmp61,sizeof(char),3U);});Cyc_aprintf(_tmp266,_tag_fat(_tmp60,sizeof(void*),1));});}));return;}}else{_tmp59=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58)->f1;_tmp5A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58)->f2;_tmp5C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp58)->f3;{struct Cyc_Core_Opt*k=_tmp59;void*t=_tmp5A;int i=_tmp5C;
Cyc_Absyndump_dumpntyp((void*)_check_null(t));return;}}case 6: _tmp59=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp58)->f1;{struct Cyc_List_List*ts=_tmp59;
Cyc_Absyndump_dump_char(36);Cyc_Absyndump_dumpargs(ts);return;}case 7: _tmp5B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp58)->f1;_tmp59=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp58)->f2;{enum Cyc_Absyn_AggrKind k=_tmp5B;struct Cyc_List_List*fs=_tmp59;
# 411
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpaggrfields(fs);Cyc_Absyndump_dump_char(125);return;}case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp58)->f1)->r)){case 0: _tmp59=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp58)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_tmp59;
Cyc_Absyndump_dump_aggrdecl(d);return;}case 1: _tmp59=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp58)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_tmp59;
Cyc_Absyndump_dump_enumdecl(d);return;}default: _tmp59=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp58)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_tmp59;
Cyc_Absyndump_dump_datatypedecl(d);return;}}case 8: _tmp59=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp58)->f1;_tmp5A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp58)->f2;{struct _tuple0*n=_tmp59;struct Cyc_List_List*ts=_tmp5A;
Cyc_Absyndump_dumpqvar(n),Cyc_Absyndump_dumptps(ts);return;}case 9: _tmp59=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp58)->f1;{struct Cyc_Absyn_Exp*e=_tmp59;
Cyc_Absyndump_dump(({const char*_tmp63="valueof_t(";_tag_fat(_tmp63,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp64=")";_tag_fat(_tmp64,sizeof(char),2U);}));return;}default: _tmp59=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp58)->f1;{struct Cyc_Absyn_Exp*e=_tmp59;
Cyc_Absyndump_dump(({const char*_tmp65="typeof(";_tag_fat(_tmp65,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp66=")";_tag_fat(_tmp66,sizeof(char),2U);}));return;}};}
# 490
static void Cyc_Absyndump_dumpvaropt(struct _fat_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 493
static void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;})((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple18{void*f1;void*f2;};
# 496
static void Cyc_Absyndump_dump_rgncmp(struct _tuple18*cmp){
Cyc_Absyndump_dumpeff((*cmp).f1);Cyc_Absyndump_dump_char(62);Cyc_Absyndump_dumprgn((*cmp).f2);}
# 499
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({void(*_tmp268)(void(*)(struct _tuple18*),struct Cyc_List_List*,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple18*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;});struct Cyc_List_List*_tmp267=rgn_po;_tmp268(Cyc_Absyndump_dump_rgncmp,_tmp267,({const char*_tmpA7=",";_tag_fat(_tmpA7,sizeof(char),2U);}));});}
# 502
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 506
if((((args == 0 && !c_varargs)&& cyc_varargs == 0)&& effopt == 0)&& rgn_po == 0)
# 508
Cyc_Absyndump_dump(({const char*_tmpA8="(void)";_tag_fat(_tmpA8,sizeof(char),7U);}));else{
# 511
({void(*_tmp26C)(void(*)(struct _tuple8*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple8*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp26B=args;struct _fat_ptr _tmp26A=({const char*_tmpA9="(";_tag_fat(_tmpA9,sizeof(char),2U);});struct _fat_ptr _tmp269=({const char*_tmpAA="";_tag_fat(_tmpAA,sizeof(char),1U);});_tmp26C(Cyc_Absyndump_dumpfunarg,_tmp26B,_tmp26A,_tmp269,({const char*_tmpAB=",";_tag_fat(_tmpAB,sizeof(char),2U);}));});
if(c_varargs || cyc_varargs != 0)
Cyc_Absyndump_dump(({const char*_tmpAC=",...";_tag_fat(_tmpAC,sizeof(char),5U);}));
if(cyc_varargs != 0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpAD=" inject ";_tag_fat(_tmpAD,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(({struct _tuple8*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->f1=cyc_varargs->name,_tmpAE->f2=cyc_varargs->tq,_tmpAE->f3=cyc_varargs->type;_tmpAE;}));}
# 518
if(effopt != 0){
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff(effopt);}
# 521
if(rgn_po != 0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 524
Cyc_Absyndump_dump_char(41);}
# 526
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpAF=" @requires(";_tag_fat(_tmpAF,sizeof(char),12U);}));Cyc_Absyndump_dumpexp(req);Cyc_Absyndump_dump_char(41);}
# 529
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpB0=" @ensures(";_tag_fat(_tmpB0,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(ens);Cyc_Absyndump_dump_char(41);}}
# 534
static void Cyc_Absyndump_dumptyp(void*t){
({void(*_tmp26F)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int)=({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int))Cyc_Absyndump_dumptqtd;});struct Cyc_Absyn_Tqual _tmp26E=Cyc_Absyn_empty_tqual(0U);void*_tmp26D=t;_tmp26F(_tmp26E,_tmp26D,({(void(*)(int))Cyc_Absyndump_ignore;}),0);});}
# 538
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpB1=d;void*_tmpB2;if(*((int*)_tmpB1)== 0){_tmpB2=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB1)->f1;{struct Cyc_Absyn_Exp*e=_tmpB2;
Cyc_Absyndump_dump(({const char*_tmpB3=".[";_tag_fat(_tmpB3,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(93);goto _LL0;}}else{_tmpB2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB1)->f1;{struct _fat_ptr*v=_tmpB2;
Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*v);goto _LL0;}}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 544
static void Cyc_Absyndump_dumpde(struct _tuple19*de){
({void(*_tmp273)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp272=(*de).f1;struct _fat_ptr _tmp271=({const char*_tmpB4="";_tag_fat(_tmpB4,sizeof(char),1U);});struct _fat_ptr _tmp270=({const char*_tmpB5="=";_tag_fat(_tmpB5,sizeof(char),2U);});_tmp273(Cyc_Absyndump_dumpdesignator,_tmp272,_tmp271,_tmp270,({const char*_tmpB6="=";_tag_fat(_tmpB6,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 549
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_tmpB7=f;unsigned _tmpB8;void*_tmpB9;if(*((int*)_tmpB7)== 0){_tmpB9=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpB7)->f1;{struct _fat_ptr*n=_tmpB9;
Cyc_Absyndump_dump_nospace(*n);return;}}else{_tmpB8=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpB7)->f1;{unsigned n=_tmpB8;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpBC=({struct Cyc_Int_pa_PrintArg_struct _tmp23B;_tmp23B.tag=1,_tmp23B.f1=(unsigned long)((int)n);_tmp23B;});void*_tmpBA[1];_tmpBA[0]=& _tmpBC;({struct _fat_ptr _tmp274=({const char*_tmpBB="%d";_tag_fat(_tmpBB,sizeof(char),3U);});Cyc_aprintf(_tmp274,_tag_fat(_tmpBA,sizeof(void*),1));});}));return;}};}struct _tuple20{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 556
static void Cyc_Absyndump_dump_asm_iolist(struct Cyc_List_List*iolist){
while((unsigned)iolist){
struct _tuple20*_tmpBD=(struct _tuple20*)iolist->hd;struct _tuple20*_stmttmpB=_tmpBD;struct _tuple20*_tmpBE=_stmttmpB;void*_tmpC0;struct _fat_ptr _tmpBF;_tmpBF=_tmpBE->f1;_tmpC0=_tmpBE->f2;{struct _fat_ptr constr=_tmpBF;struct Cyc_Absyn_Exp*e1=_tmpC0;
Cyc_Absyndump_dump_char(34);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(constr));Cyc_Absyndump_dump_char(34);
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);
iolist=iolist->tl;
if((unsigned)iolist)
Cyc_Absyndump_dump_char(44);}}}
# 567
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({void(*_tmp279)(void(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group_c;});int _tmp278=inprec;struct Cyc_List_List*_tmp277=es;struct _fat_ptr _tmp276=({const char*_tmpC1="";_tag_fat(_tmpC1,sizeof(char),1U);});struct _fat_ptr _tmp275=({const char*_tmpC2="";_tag_fat(_tmpC2,sizeof(char),1U);});_tmp279(Cyc_Absyndump_dumpexp_prec,_tmp278,_tmp277,_tmp276,_tmp275,({const char*_tmpC3=",";_tag_fat(_tmpC3,sizeof(char),2U);}));});}
# 571
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpC4="(";_tag_fat(_tmpC4,sizeof(char),2U);}));
{void*_tmpC5=e->r;void*_stmttmpC=_tmpC5;void*_tmpC6=_stmttmpC;int _tmpCB;void*_tmpCA;void*_tmpC8;enum Cyc_Absyn_Primop _tmpCD;void*_tmpC7;long long _tmpCF;int _tmpC9;short _tmpD0;struct _fat_ptr _tmpCC;char _tmpD1;enum Cyc_Absyn_Sign _tmpCE;switch(*((int*)_tmpC6)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).String_c).tag){case 2: _tmpCE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Char_c).val).f1;_tmpD1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Char_c).val).f2;{enum Cyc_Absyn_Sign sg=_tmpCE;char ch=_tmpD1;
# 577
Cyc_Absyndump_dump_char(39);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_char(39);
goto _LL0;}case 3: _tmpCC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Wchar_c).val;{struct _fat_ptr s=_tmpCC;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD4=({struct Cyc_String_pa_PrintArg_struct _tmp23C;_tmp23C.tag=0,_tmp23C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp23C;});void*_tmpD2[1];_tmpD2[0]=& _tmpD4;({struct _fat_ptr _tmp27A=({const char*_tmpD3="L'%s'";_tag_fat(_tmpD3,sizeof(char),6U);});Cyc_aprintf(_tmp27A,_tag_fat(_tmpD2,sizeof(void*),1));});}));goto _LL0;}case 4: _tmpCE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Short_c).val).f1;_tmpD0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Short_c).val).f2;{enum Cyc_Absyn_Sign sg=_tmpCE;short s=_tmpD0;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD7=({struct Cyc_Int_pa_PrintArg_struct _tmp23D;_tmp23D.tag=1,_tmp23D.f1=(unsigned long)((int)s);_tmp23D;});void*_tmpD5[1];_tmpD5[0]=& _tmpD7;({struct _fat_ptr _tmp27B=({const char*_tmpD6="%d";_tag_fat(_tmpD6,sizeof(char),3U);});Cyc_aprintf(_tmp27B,_tag_fat(_tmpD5,sizeof(void*),1));});}));goto _LL0;}case 5: switch((int)(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Int_c).val).f1){case Cyc_Absyn_None: _tmpC9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Int_c).val).f2;{int i=_tmpC9;
_tmpC9=i;goto _LLA;}case Cyc_Absyn_Signed: _tmpC9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Int_c).val).f2;_LLA: {int i=_tmpC9;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDA=({struct Cyc_Int_pa_PrintArg_struct _tmp23E;_tmp23E.tag=1,_tmp23E.f1=(unsigned long)i;_tmp23E;});void*_tmpD8[1];_tmpD8[0]=& _tmpDA;({struct _fat_ptr _tmp27C=({const char*_tmpD9="%d";_tag_fat(_tmpD9,sizeof(char),3U);});Cyc_aprintf(_tmp27C,_tag_fat(_tmpD8,sizeof(void*),1));});}));goto _LL0;}default: _tmpC9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Int_c).val).f2;{int i=_tmpC9;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDD=({struct Cyc_Int_pa_PrintArg_struct _tmp23F;_tmp23F.tag=1,_tmp23F.f1=(unsigned)i;_tmp23F;});void*_tmpDB[1];_tmpDB[0]=& _tmpDD;({struct _fat_ptr _tmp27D=({const char*_tmpDC="%uU";_tag_fat(_tmpDC,sizeof(char),4U);});Cyc_aprintf(_tmp27D,_tag_fat(_tmpDB,sizeof(void*),1));});}));goto _LL0;}}case 6: _tmpCE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).LongLong_c).val).f1;_tmpCF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).LongLong_c).val).f2;{enum Cyc_Absyn_Sign sg=_tmpCE;long long i=_tmpCF;
# 586
Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)i));goto _LL0;}case 7: _tmpCC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Float_c).val).f1;{struct _fat_ptr x=_tmpCC;
Cyc_Absyndump_dump(x);goto _LL0;}case 1:
 Cyc_Absyndump_dump(({const char*_tmpDE="NULL";_tag_fat(_tmpDE,sizeof(char),5U);}));goto _LL0;case 8: _tmpCC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).String_c).val;{struct _fat_ptr s=_tmpCC;
# 590
Cyc_Absyndump_dump_char(34);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(s));Cyc_Absyndump_dump_char(34);
goto _LL0;}default: _tmpCC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).Wstring_c).val;{struct _fat_ptr s=_tmpCC;
# 593
Cyc_Absyndump_dump(({const char*_tmpDF="L\"";_tag_fat(_tmpDF,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char(34);goto _LL0;}}case 1: _tmpC7=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{void*b=_tmpC7;
# 595
Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _tmpCC=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct _fat_ptr p=_tmpCC;
# 598
Cyc_Absyndump_dump(({const char*_tmpE0="__cyclone_pragma__(";_tag_fat(_tmpE0,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(p);Cyc_Absyndump_dump_char(41);goto _LL0;}case 3: _tmpCD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{enum Cyc_Absyn_Primop p=_tmpCD;struct Cyc_List_List*es=_tmpC7;
# 601
struct _fat_ptr _tmpE1=Cyc_Absynpp_prim2str(p);struct _fat_ptr pstr=_tmpE1;
{int _tmpE2=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(es);int _stmttmpD=_tmpE2;int _tmpE3=_stmttmpD;switch((int)_tmpE3){case 1:
# 604
 if((int)p == (int)18U){
Cyc_Absyndump_dump(({const char*_tmpE4="numelts(";_tag_fat(_tmpE4,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(({const char*_tmpE5=")";_tag_fat(_tmpE5,sizeof(char),2U);}));}else{
# 609
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);}
# 612
goto _LL71;case 2:
# 614
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dump_char(32);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);
goto _LL71;default:
({void*_tmpE6=0U;({int(*_tmp27F)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp27E=({const char*_tmpE7="Absyndump -- Bad number of arguments to primop";_tag_fat(_tmpE7,sizeof(char),47U);});_tmp27F(_tmp27E,_tag_fat(_tmpE6,sizeof(void*),0));});});}_LL71:;}
# 621
goto _LL0;}case 4: _tmpC7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;_tmpCA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC6)->f3;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Core_Opt*popt=_tmpC8;struct Cyc_Absyn_Exp*e2=_tmpCA;
# 624
Cyc_Absyndump_dumpexp_prec(myprec,e1);
if(popt != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)popt->v));
Cyc_Absyndump_dump_nospace(({const char*_tmpE8="=";_tag_fat(_tmpE8,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,e2);
goto _LL0;}case 5: switch((int)((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC6)->f2){case Cyc_Absyn_PreInc: _tmpC7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e2=_tmpC7;
# 631
Cyc_Absyndump_dump(({const char*_tmpE9="++";_tag_fat(_tmpE9,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PreDec: _tmpC7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e2=_tmpC7;
Cyc_Absyndump_dump(({const char*_tmpEA="--";_tag_fat(_tmpEA,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PostInc: _tmpC7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e2=_tmpC7;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmpEB="++";_tag_fat(_tmpEB,sizeof(char),3U);}));goto _LL0;}default: _tmpC7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e2=_tmpC7;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmpEC="--";_tag_fat(_tmpEC,sizeof(char),3U);}));goto _LL0;}}case 6: _tmpC7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;_tmpCA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC6)->f3;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;struct Cyc_Absyn_Exp*e3=_tmpCA;
# 637
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_char(63);Cyc_Absyndump_dumpexp_prec(0,e2);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp_prec(myprec,e3);
goto _LL0;}case 7: _tmpC7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
# 644
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmpED="&&";_tag_fat(_tmpED,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 8: _tmpC7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
# 648
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmpEE="||";_tag_fat(_tmpEE,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 9: _tmpC7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
# 654
Cyc_Absyndump_dumploc(e->loc);
# 657
Cyc_Absyndump_dumpexp_prec(myprec - 1,e1);Cyc_Absyndump_dump_char(44);Cyc_Absyndump_dumpexp_prec(myprec - 1,e2);
goto _LL0;}case 10: _tmpC7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_List_List*es=_tmpC8;
# 661
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_nospace(({const char*_tmpEF="(";_tag_fat(_tmpEF,sizeof(char),2U);}));{
# 666
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc == 0U))&& !(e1->loc == 0U);
Cyc_Absyndump_dumpexps_prec(20,es);
Cyc_Absyndump_dump_nospace(({const char*_tmpF0=")";_tag_fat(_tmpF0,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;}}case 11: _tmpC7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
# 675
Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_tmpF1="throw";_tag_fat(_tmpF1,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 12: _tmpC7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
# 677
_tmpC7=e1;goto _LL36;}case 13: _tmpC7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpC7;
Cyc_Absyndump_dumpexp_prec(inprec,e1);goto _LL0;}case 14: _tmpC7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{void*t=_tmpC7;struct Cyc_Absyn_Exp*e1=_tmpC8;
# 681
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dumpexp_prec(myprec,e1);
goto _LL0;}case 15: _tmpC7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
# 684
Cyc_Absyndump_dump_char(38);Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 20: _tmpC7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
Cyc_Absyndump_dump_char(42);Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 16: _tmpC7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*ropt=_tmpC7;struct Cyc_Absyn_Exp*e1=_tmpC8;
# 688
Cyc_Absyndump_dump(({const char*_tmpF2="new";_tag_fat(_tmpF2,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 17: _tmpC7=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{void*t=_tmpC7;
Cyc_Absyndump_dump(({const char*_tmpF3="sizeof(";_tag_fat(_tmpF3,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);goto _LL0;}case 18: _tmpC7=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
Cyc_Absyndump_dump(({const char*_tmpF4="sizeof(";_tag_fat(_tmpF4,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 39: _tmpC7=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{void*t=_tmpC7;
Cyc_Absyndump_dump(({const char*_tmpF5="valueof(";_tag_fat(_tmpF5,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);goto _LL0;}case 41: _tmpC7=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
# 694
Cyc_Absyndump_dump(({const char*_tmpF6="__extension__(";_tag_fat(_tmpF6,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 42: _tmpC7=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC7;
# 696
Cyc_Absyndump_dump(({const char*_tmpF7="@assert(";_tag_fat(_tmpF7,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 40: _tmpC9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpCC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;_tmpC7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC6)->f3;_tmpC8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC6)->f4;_tmpCA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpC6)->f5;{int vol=_tmpC9;struct _fat_ptr t=_tmpCC;struct Cyc_List_List*o=_tmpC7;struct Cyc_List_List*i=_tmpC8;struct Cyc_List_List*cl=_tmpCA;
# 699
Cyc_Absyndump_dump(({const char*_tmpF8="__asm__";_tag_fat(_tmpF8,sizeof(char),8U);}));
if(vol)Cyc_Absyndump_dump(({const char*_tmpF9=" volatile ";_tag_fat(_tmpF9,sizeof(char),11U);}));
Cyc_Absyndump_dump_nospace(({const char*_tmpFA="(\"";_tag_fat(_tmpFA,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(t));Cyc_Absyndump_dump_char(34);
if((unsigned)o){
Cyc_Absyndump_dump_char(58);
Cyc_Absyndump_dump_asm_iolist(o);}
# 706
if((unsigned)i){
if(!((unsigned)o))
Cyc_Absyndump_dump_char(58);
Cyc_Absyndump_dump_char(58);
Cyc_Absyndump_dump_asm_iolist(i);}
# 712
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
{int cols=0;for(0;cols < 3 - ncol;++ cols){
Cyc_Absyndump_dump_nospace(({const char*_tmpFB=" :";_tag_fat(_tmpFB,sizeof(char),3U);}));}}
while((unsigned)cl){
Cyc_Absyndump_dump_char(34);
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(*((struct _fat_ptr*)cl->hd)));
Cyc_Absyndump_dump_char(34);
cl=cl->tl;
if((unsigned)cl)
Cyc_Absyndump_dump_char(44);}}
# 725
Cyc_Absyndump_dump_char(41);
goto _LL0;}case 38: _tmpC7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e=_tmpC7;struct _fat_ptr*f=_tmpC8;
# 729
Cyc_Absyndump_dump(({const char*_tmpFC="tagcheck(";_tag_fat(_tmpFC,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*f);
Cyc_Absyndump_dump_char(41);
goto _LL0;}case 19: _tmpC7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{void*t=_tmpC7;struct Cyc_List_List*l=_tmpC8;
# 734
Cyc_Absyndump_dump(({const char*_tmpFD="offsetof(";_tag_fat(_tmpFD,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(t);({void(*_tmp283)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp282=l;struct _fat_ptr _tmp281=({const char*_tmpFE=",";_tag_fat(_tmpFE,sizeof(char),2U);});struct _fat_ptr _tmp280=({const char*_tmpFF=")";_tag_fat(_tmpFF,sizeof(char),2U);});_tmp283(Cyc_Absyndump_dumpoffset_field,_tmp282,_tmp281,_tmp280,({const char*_tmp100=".";_tag_fat(_tmp100,sizeof(char),2U);}));});goto _LL0;}case 21: _tmpC7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct _fat_ptr*n=_tmpC8;
# 737
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 22: _tmpC7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct _fat_ptr*n=_tmpC8;
# 739
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp101="->";_tag_fat(_tmp101,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 23: _tmpC7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
# 742
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char(91);Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char(93);goto _LL0;}case 24: _tmpC7=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_List_List*es=_tmpC7;
# 744
Cyc_Absyndump_dump(({const char*_tmp102="$(";_tag_fat(_tmp102,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_char(41);goto _LL0;}case 25: _tmpC7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct _tuple8*vat=_tmpC7;struct Cyc_List_List*des=_tmpC8;
# 747
Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dumptyp((*vat).f3);
Cyc_Absyndump_dump_char(41);
({void(*_tmp287)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp286=des;struct _fat_ptr _tmp285=({const char*_tmp103="{";_tag_fat(_tmp103,sizeof(char),2U);});struct _fat_ptr _tmp284=({const char*_tmp104="}";_tag_fat(_tmp104,sizeof(char),2U);});_tmp287(Cyc_Absyndump_dumpde,_tmp286,_tmp285,_tmp284,({const char*_tmp105=",";_tag_fat(_tmp105,sizeof(char),2U);}));});
goto _LL0;}case 26: _tmpC7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_List_List*des=_tmpC7;
# 753
({void(*_tmp28B)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp28A=des;struct _fat_ptr _tmp289=({const char*_tmp106="{";_tag_fat(_tmp106,sizeof(char),2U);});struct _fat_ptr _tmp288=({const char*_tmp107="}";_tag_fat(_tmp107,sizeof(char),2U);});_tmp28B(Cyc_Absyndump_dumpde,_tmp28A,_tmp289,_tmp288,({const char*_tmp108=",";_tag_fat(_tmp108,sizeof(char),2U);}));});goto _LL0;}case 27: _tmpC7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;_tmpCA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpC6)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpC7;struct Cyc_Absyn_Exp*e1=_tmpC8;struct Cyc_Absyn_Exp*e2=_tmpCA;
# 756
Cyc_Absyndump_dump(({const char*_tmp109="{for";_tag_fat(_tmp109,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*vd->name).f2);Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumpexp(e1);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 28: _tmpC7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e=_tmpC7;void*t=_tmpC8;
# 761
Cyc_Absyndump_dump(({const char*_tmp10A="{for x ";_tag_fat(_tmp10A,sizeof(char),8U);}));Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumpexp(e);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 29: _tmpC7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;_tmpCA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpC6)->f3;{struct _tuple0*n=_tmpC7;struct Cyc_List_List*ts=_tmpC8;struct Cyc_List_List*des=_tmpCA;
# 766
Cyc_Absyndump_dumpqvar(n);
Cyc_Absyndump_dump_char(123);
if(ts != 0)
Cyc_Absyndump_dumptps(ts);
({void(*_tmp28F)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp28E=des;struct _fat_ptr _tmp28D=({const char*_tmp10B="";_tag_fat(_tmp10B,sizeof(char),1U);});struct _fat_ptr _tmp28C=({const char*_tmp10C="}";_tag_fat(_tmp10C,sizeof(char),2U);});_tmp28F(Cyc_Absyndump_dumpde,_tmp28E,_tmp28D,_tmp28C,({const char*_tmp10D=",";_tag_fat(_tmp10D,sizeof(char),2U);}));});
goto _LL0;}case 30: _tmpC7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_List_List*des=_tmpC7;
# 773
({void(*_tmp293)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp292=des;struct _fat_ptr _tmp291=({const char*_tmp10E="{";_tag_fat(_tmp10E,sizeof(char),2U);});struct _fat_ptr _tmp290=({const char*_tmp10F="}";_tag_fat(_tmp10F,sizeof(char),2U);});_tmp293(Cyc_Absyndump_dumpde,_tmp292,_tmp291,_tmp290,({const char*_tmp110=",";_tag_fat(_tmp110,sizeof(char),2U);}));});goto _LL0;}case 32: _tmpC7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmpC7;
_tmpC7=ef;goto _LL66;}case 33: _tmpC7=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;_LL66: {struct Cyc_Absyn_Enumfield*ef=_tmpC7;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 31: _tmpC7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpC6)->f3;{struct Cyc_List_List*es=_tmpC7;struct Cyc_Absyn_Datatypefield*tuf=_tmpC8;
# 778
Cyc_Absyndump_dumpqvar(tuf->name);
if(es != 0)({void(*_tmp297)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp296=es;struct _fat_ptr _tmp295=({const char*_tmp111="(";_tag_fat(_tmp111,sizeof(char),2U);});struct _fat_ptr _tmp294=({const char*_tmp112=")";_tag_fat(_tmp112,sizeof(char),2U);});_tmp297(Cyc_Absyndump_dumpexp,_tmp296,_tmp295,_tmp294,({const char*_tmp113=",";_tag_fat(_tmp113,sizeof(char),2U);}));});
goto _LL0;}case 34: _tmpC9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).is_calloc;_tmpC7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).rgn;_tmpC8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).elt_type;_tmpCA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).num_elts;_tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpC6)->f1).inline_call;{int is_calloc=_tmpC9;struct Cyc_Absyn_Exp*ropt=_tmpC7;void**topt=_tmpC8;struct Cyc_Absyn_Exp*e=_tmpCA;int inline_call=_tmpCB;
# 783
Cyc_Absyndump_dumploc(e->loc);
if(is_calloc){
if(ropt != 0){
Cyc_Absyndump_dump(({const char*_tmp114="rcalloc(";_tag_fat(_tmp114,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp115=",";_tag_fat(_tmp115,sizeof(char),2U);}));}else{
# 788
Cyc_Absyndump_dump(({const char*_tmp116="calloc(";_tag_fat(_tmp116,sizeof(char),8U);}));}
# 790
Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp117=",";_tag_fat(_tmp117,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U));Cyc_Absyndump_dump(({const char*_tmp118=")";_tag_fat(_tmp118,sizeof(char),2U);}));}else{
# 792
if(ropt != 0){
Cyc_Absyndump_dump(inline_call?({const char*_tmp119="rmalloc_inline(";_tag_fat(_tmp119,sizeof(char),16U);}):({const char*_tmp11A="rmalloc(";_tag_fat(_tmp11A,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp11B=",";_tag_fat(_tmp11B,sizeof(char),2U);}));}else{
# 796
Cyc_Absyndump_dump(({const char*_tmp11C="malloc(";_tag_fat(_tmp11C,sizeof(char),8U);}));}
# 799
if(topt != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp298=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp298,e,0U);}));else{
# 802
Cyc_Absyndump_dumpexp(e);}
Cyc_Absyndump_dump(({const char*_tmp11D=")";_tag_fat(_tmp11D,sizeof(char),2U);}));}
# 805
goto _LL0;}case 35: _tmpC7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
# 808
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp11E=":=:";_tag_fat(_tmp11E,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 36: _tmpC7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC6)->f2;{struct Cyc_Core_Opt*n=_tmpC7;struct Cyc_List_List*des=_tmpC8;
# 811
({void(*_tmp29C)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp29B=des;struct _fat_ptr _tmp29A=({const char*_tmp11F="{";_tag_fat(_tmp11F,sizeof(char),2U);});struct _fat_ptr _tmp299=({const char*_tmp120="}";_tag_fat(_tmp120,sizeof(char),2U);});_tmp29C(Cyc_Absyndump_dumpde,_tmp29B,_tmp29A,_tmp299,({const char*_tmp121=",";_tag_fat(_tmp121,sizeof(char),2U);}));});goto _LL0;}default: _tmpC7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC6)->f1;{struct Cyc_Absyn_Stmt*s=_tmpC7;
# 814
Cyc_Absyndump_dump_nospace(({const char*_tmp122="({";_tag_fat(_tmp122,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s,1,0);Cyc_Absyndump_dump_nospace(({const char*_tmp123="})";_tag_fat(_tmp123,sizeof(char),3U);}));goto _LL0;}}_LL0:;}
# 816
if(inprec >= myprec)
Cyc_Absyndump_dump_char(41);}
# 820
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 824
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs,struct Cyc_List_List*varsinblock){
# 826
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp124=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*c=_tmp124;
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp125="default:";_tag_fat(_tmp125,sizeof(char),9U);}));else{
# 831
Cyc_Absyndump_dump(({const char*_tmp126="case";_tag_fat(_tmp126,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(c->pattern);
if(c->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp127="&&";_tag_fat(_tmp127,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));}
# 837
Cyc_Absyndump_dump_nospace(({const char*_tmp128=":";_tag_fat(_tmp128,sizeof(char),2U);}));}
# 839
if(Cyc_Absynpp_is_declaration(c->body)){
Cyc_Absyndump_dump(({const char*_tmp129=" {";_tag_fat(_tmp129,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(c->body,0,0),Cyc_Absyndump_dump_char(125);}else{
# 842
Cyc_Absyndump_dumpstmt(c->body,0,varsinblock);}}}
# 846
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt,struct Cyc_List_List*varsinblock){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp12A=s->r;void*_stmttmpE=_tmp12A;void*_tmp12B=_stmttmpE;void*_tmp12F;void*_tmp12E;void*_tmp12D;void*_tmp12C;switch(*((int*)_tmp12B)){case 0:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1: _tmp12C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;{struct Cyc_Absyn_Exp*e=_tmp12C;
Cyc_Absyndump_dumpexp_prec(-100,e);Cyc_Absyndump_dump_semi();goto _LL0;}case 2: _tmp12C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp12C;struct Cyc_Absyn_Stmt*s2=_tmp12D;
# 855
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);}else{
# 858
Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}
if(Cyc_Absynpp_is_declaration(s2)){
if(expstmt)Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpstmt(s2,expstmt,0);Cyc_Absyndump_dump_char(125);
if(expstmt){Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dump_semi();}}else{
# 864
Cyc_Absyndump_dumpstmt(s2,expstmt,varsinblock);}
goto _LL0;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1 == 0){
Cyc_Absyndump_dump(({const char*_tmp130="return;";_tag_fat(_tmp130,sizeof(char),8U);}));goto _LL0;}else{_tmp12C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;{struct Cyc_Absyn_Exp*e=_tmp12C;
Cyc_Absyndump_dump(({const char*_tmp131="return";_tag_fat(_tmp131,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(e));Cyc_Absyndump_dump_semi();goto _LL0;}}case 4: _tmp12C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;_tmp12E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp12B)->f3;{struct Cyc_Absyn_Exp*e=_tmp12C;struct Cyc_Absyn_Stmt*s1=_tmp12D;struct Cyc_Absyn_Stmt*s2=_tmp12E;
# 869
Cyc_Absyndump_dump(({const char*_tmp132="if(";_tag_fat(_tmp132,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(e);
{void*_tmp133=s1->r;void*_stmttmpF=_tmp133;void*_tmp134=_stmttmpF;switch(*((int*)_tmp134)){case 2:
 goto _LL29;case 12: _LL29:
 goto _LL2B;case 4: _LL2B:
 goto _LL2D;case 13: _LL2D:
# 875
 Cyc_Absyndump_dump_nospace(({const char*_tmp135="){";_tag_fat(_tmp135,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);goto _LL25;default:
 Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}_LL25:;}
# 878
{void*_tmp136=s2->r;void*_stmttmp10=_tmp136;void*_tmp137=_stmttmp10;if(*((int*)_tmp137)== 0)
goto _LL30;else{
Cyc_Absyndump_dump(({const char*_tmp138="else{";_tag_fat(_tmp138,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(s2,0,0);Cyc_Absyndump_dump_char(125);goto _LL30;}_LL30:;}
# 882
goto _LL0;}case 5: _tmp12C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1).f1;_tmp12D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;{struct Cyc_Absyn_Exp*e=_tmp12C;struct Cyc_Absyn_Stmt*s1=_tmp12D;
# 885
Cyc_Absyndump_dump(({const char*_tmp139="while(";_tag_fat(_tmp139,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp13A="){";_tag_fat(_tmp13A,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 9: _tmp12C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2).f1;_tmp12E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp12B)->f3).f1;_tmp12F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp12B)->f4;{struct Cyc_Absyn_Exp*e1=_tmp12C;struct Cyc_Absyn_Exp*e2=_tmp12D;struct Cyc_Absyn_Exp*e3=_tmp12E;struct Cyc_Absyn_Stmt*s1=_tmp12F;
# 889
Cyc_Absyndump_dump(({const char*_tmp13B="for(";_tag_fat(_tmp13B,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e3);
Cyc_Absyndump_dump_nospace(({const char*_tmp13C="){";_tag_fat(_tmp13C,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 14: _tmp12C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2).f1;{struct Cyc_Absyn_Stmt*s1=_tmp12C;struct Cyc_Absyn_Exp*e=_tmp12D;
# 893
Cyc_Absyndump_dump(({const char*_tmp13D="do{";_tag_fat(_tmp13D,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp13E="}while(";_tag_fat(_tmp13E,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp13F=");";_tag_fat(_tmp13F,sizeof(char),3U);}));
goto _LL0;}case 6:
# 897
 Cyc_Absyndump_dump(({const char*_tmp140="break;";_tag_fat(_tmp140,sizeof(char),7U);}));goto _LL0;case 7:
 Cyc_Absyndump_dump(({const char*_tmp141="continue;";_tag_fat(_tmp141,sizeof(char),10U);}));goto _LL0;case 8: _tmp12C=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;{struct _fat_ptr*x=_tmp12C;
Cyc_Absyndump_dump(({const char*_tmp142="goto";_tag_fat(_tmp142,sizeof(char),5U);}));Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_semi();goto _LL0;}case 10: _tmp12C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;{struct Cyc_Absyn_Exp*e=_tmp12C;struct Cyc_List_List*ss=_tmp12D;
# 902
Cyc_Absyndump_dump(({const char*_tmp143="switch(";_tag_fat(_tmp143,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp144="){";_tag_fat(_tmp144,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);
Cyc_Absyndump_dump_char(125);
goto _LL0;}case 15: _tmp12C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp12C;struct Cyc_List_List*ss=_tmp12D;
# 907
Cyc_Absyndump_dump(({const char*_tmp145="try";_tag_fat(_tmp145,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,varsinblock);
Cyc_Absyndump_dump(({const char*_tmp146="catch{";_tag_fat(_tmp146,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 12: _tmp12C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;{struct Cyc_Absyn_Decl*d=_tmp12C;struct Cyc_Absyn_Stmt*s1=_tmp12D;
# 913
struct _tuple13 _tmp147=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple13 _stmttmp11=_tmp147;struct _tuple13 _tmp148=_stmttmp11;void*_tmp14A;int _tmp149;_tmp149=_tmp148.f1;_tmp14A=_tmp148.f2;{int newblock=_tmp149;struct Cyc_List_List*newvarsinblock=_tmp14A;
if(newblock){
if(expstmt)Cyc_Absyndump_dump(({const char*_tmp14B="({";_tag_fat(_tmp14B,sizeof(char),3U);}));else{Cyc_Absyndump_dump(({const char*_tmp14C="{";_tag_fat(_tmp14C,sizeof(char),2U);}));}
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp14D="});";_tag_fat(_tmp14D,sizeof(char),4U);}));else{Cyc_Absyndump_dump(({const char*_tmp14E="}";_tag_fat(_tmp14E,sizeof(char),2U);}));}}else{
# 920
Cyc_Absyndump_dumpdecl(d);Cyc_Absyndump_dumpstmt(s1,expstmt,newvarsinblock);}
# 922
goto _LL0;}}case 13: _tmp12C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;_tmp12D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp12B)->f2;{struct _fat_ptr*x=_tmp12C;struct Cyc_Absyn_Stmt*s1=_tmp12D;
# 928
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_str(x);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp14F=": ({";_tag_fat(_tmp14F,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp150=": {";_tag_fat(_tmp150,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp151="});";_tag_fat(_tmp151,sizeof(char),4U);}));else{Cyc_Absyndump_dump_char(125);}}else{
# 934
Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpstmt(s1,expstmt,varsinblock);}
# 936
goto _LL0;}default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1 == 0){
# 938
Cyc_Absyndump_dump(({const char*_tmp152="fallthru;";_tag_fat(_tmp152,sizeof(char),10U);}));goto _LL0;}else{_tmp12C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp12B)->f1;{struct Cyc_List_List*es=_tmp12C;
# 940
Cyc_Absyndump_dump(({const char*_tmp153="fallthru(";_tag_fat(_tmp153,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_nospace(({const char*_tmp154=");";_tag_fat(_tmp154,sizeof(char),3U);}));goto _LL0;}}}_LL0:;}}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 944
static void Cyc_Absyndump_dumpdp(struct _tuple21*dp){
({void(*_tmp2A0)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(void*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp29F=(*dp).f1;struct _fat_ptr _tmp29E=({const char*_tmp155="";_tag_fat(_tmp155,sizeof(char),1U);});struct _fat_ptr _tmp29D=({const char*_tmp156="=";_tag_fat(_tmp156,sizeof(char),2U);});_tmp2A0(Cyc_Absyndump_dumpdesignator,_tmp29F,_tmp29E,_tmp29D,({const char*_tmp157="=";_tag_fat(_tmp157,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 948
static struct _fat_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_tmp158="...)";_tag_fat(_tmp158,sizeof(char),5U);}):({const char*_tmp159=")";_tag_fat(_tmp159,sizeof(char),2U);});}
# 950
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp15A=p->r;void*_stmttmp12=_tmp15A;void*_tmp15B=_stmttmp12;union Cyc_Absyn_AggrInfo _tmp15F;void*_tmp15D;char _tmp160;void*_tmp15C;struct _fat_ptr _tmp161;int _tmp15E;switch(*((int*)_tmp15B)){case 0:
 Cyc_Absyndump_dump_char(95);goto _LL0;case 9:
 Cyc_Absyndump_dump(({const char*_tmp162="NULL";_tag_fat(_tmp162,sizeof(char),5U);}));goto _LL0;case 10: switch((int)((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15B)->f1){case Cyc_Absyn_None: _tmp15E=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{int i=_tmp15E;
_tmp15E=i;goto _LL8;}case Cyc_Absyn_Signed: _tmp15E=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_LL8: {int i=_tmp15E;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp165=({struct Cyc_Int_pa_PrintArg_struct _tmp240;_tmp240.tag=1,_tmp240.f1=(unsigned long)i;_tmp240;});void*_tmp163[1];_tmp163[0]=& _tmp165;({struct _fat_ptr _tmp2A1=({const char*_tmp164="%d";_tag_fat(_tmp164,sizeof(char),3U);});Cyc_aprintf(_tmp2A1,_tag_fat(_tmp163,sizeof(void*),1));});}));goto _LL0;}default: _tmp15E=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{int i=_tmp15E;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp168=({struct Cyc_Int_pa_PrintArg_struct _tmp241;_tmp241.tag=1,_tmp241.f1=(unsigned)i;_tmp241;});void*_tmp166[1];_tmp166[0]=& _tmp168;({struct _fat_ptr _tmp2A2=({const char*_tmp167="%u";_tag_fat(_tmp167,sizeof(char),3U);});Cyc_aprintf(_tmp2A2,_tag_fat(_tmp166,sizeof(void*),1));});}));goto _LL0;}}case 12: _tmp161=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;{struct _fat_ptr x=_tmp161;
Cyc_Absyndump_dump(x);goto _LL0;}case 13: _tmp15C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp15C;
_tmp15C=ef;goto _LL10;}case 14: _tmp15C=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_LL10: {struct Cyc_Absyn_Enumfield*ef=_tmp15C;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 11: _tmp160=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;{char ch=_tmp160;
# 961
Cyc_Absyndump_dump(({const char*_tmp169="'";_tag_fat(_tmp169,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_nospace(({const char*_tmp16A="'";_tag_fat(_tmp16A,sizeof(char),2U);}));goto _LL0;}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15B)->f2)->r)== 0){_tmp15C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp15C;
Cyc_Absyndump_dump(({const char*_tmp16B="*";_tag_fat(_tmp16B,sizeof(char),2U);}));_tmp15C=vd;goto _LL16;}}else{_tmp15C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp15C;struct Cyc_Absyn_Pat*p2=_tmp15D;
# 964
Cyc_Absyndump_dump(({const char*_tmp16C="*";_tag_fat(_tmp16C,sizeof(char),2U);}));_tmp15C=vd;_tmp15D=p2;goto _LL1A;}}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp15B)->f2)->r)== 0){_tmp15C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_LL16: {struct Cyc_Absyn_Vardecl*vd=_tmp15C;
# 963
Cyc_Absyndump_dumpqvar(vd->name);goto _LL0;}}else{_tmp15C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp15C;struct Cyc_Absyn_Pat*p2=_tmp15D;
# 965
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump(({const char*_tmp16D=" as ";_tag_fat(_tmp16D,sizeof(char),5U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}}case 2: _tmp15C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15D=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp15C;struct Cyc_Absyn_Vardecl*vd=_tmp15D;
# 967
Cyc_Absyndump_dump(({const char*_tmp16E="alias";_tag_fat(_tmp16E,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_tmp16F="<";_tag_fat(_tmp16F,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp170=">";_tag_fat(_tmp170,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(vd,p->loc,0);
goto _LL0;}case 5: _tmp15C=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{struct Cyc_List_List*ts=_tmp15C;int dots=_tmp15E;
({void(*_tmp2A6)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp2A5=ts;struct _fat_ptr _tmp2A4=({const char*_tmp171="$(";_tag_fat(_tmp171,sizeof(char),3U);});struct _fat_ptr _tmp2A3=Cyc_Absyndump_pat_term(dots);_tmp2A6(Cyc_Absyndump_dumppat,_tmp2A5,_tmp2A4,_tmp2A3,({const char*_tmp172=",";_tag_fat(_tmp172,sizeof(char),2U);}));});goto _LL0;}case 6: _tmp15C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;{struct Cyc_Absyn_Pat*p2=_tmp15C;
Cyc_Absyndump_dump(({const char*_tmp173="&";_tag_fat(_tmp173,sizeof(char),2U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}case 4: _tmp15C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp15C;struct Cyc_Absyn_Vardecl*vd=_tmp15D;
# 973
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump_char(62);goto _LL0;}case 15: _tmp15C=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;{struct _tuple0*q=_tmp15C;
Cyc_Absyndump_dumpqvar(q);goto _LL0;}case 16: _tmp15C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_tmp15E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp15B)->f3;{struct _tuple0*q=_tmp15C;struct Cyc_List_List*ps=_tmp15D;int dots=_tmp15E;
# 976
Cyc_Absyndump_dumpqvar(q);({void(*_tmp2AA)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp2A9=ps;struct _fat_ptr _tmp2A8=({const char*_tmp174="(";_tag_fat(_tmp174,sizeof(char),2U);});struct _fat_ptr _tmp2A7=Cyc_Absyndump_pat_term(dots);_tmp2AA(Cyc_Absyndump_dumppat,_tmp2A9,_tmp2A8,_tmp2A7,({const char*_tmp175=",";_tag_fat(_tmp175,sizeof(char),2U);}));});goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f1 != 0){_tmp15F=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;_tmp15C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_tmp15D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f3;_tmp15E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f4;{union Cyc_Absyn_AggrInfo info=_tmp15F;struct Cyc_List_List*exists=_tmp15C;struct Cyc_List_List*dps=_tmp15D;int dots=_tmp15E;
# 978
Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(info)).f2);
_tmp15C=exists;_tmp15D=dps;_tmp15E=dots;goto _LL2A;}}else{_tmp15C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_tmp15D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f3;_tmp15E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15B)->f4;_LL2A: {struct Cyc_List_List*exists=_tmp15C;struct Cyc_List_List*dps=_tmp15D;int dots=_tmp15E;
# 981
Cyc_Absyndump_dump_char(123);
({void(*_tmp2AE)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp2AD=exists;struct _fat_ptr _tmp2AC=({const char*_tmp176="<";_tag_fat(_tmp176,sizeof(char),2U);});struct _fat_ptr _tmp2AB=({const char*_tmp177=">";_tag_fat(_tmp177,sizeof(char),2U);});_tmp2AE(Cyc_Absyndump_dumptvar,_tmp2AD,_tmp2AC,_tmp2AB,({const char*_tmp178=",";_tag_fat(_tmp178,sizeof(char),2U);}));});
({void(*_tmp2B2)(void(*)(struct _tuple21*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple21*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp2B1=dps;struct _fat_ptr _tmp2B0=({const char*_tmp179="";_tag_fat(_tmp179,sizeof(char),1U);});struct _fat_ptr _tmp2AF=Cyc_Absyndump_pat_term(dots);_tmp2B2(Cyc_Absyndump_dumpdp,_tmp2B1,_tmp2B0,_tmp2AF,({const char*_tmp17A=",";_tag_fat(_tmp17A,sizeof(char),2U);}));});
goto _LL0;}}case 8: _tmp15C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15B)->f2;_tmp15D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15B)->f3;_tmp15E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15B)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_tmp15C;struct Cyc_List_List*ps=_tmp15D;int dots=_tmp15E;
# 986
Cyc_Absyndump_dumpqvar(tuf->name);
if(ps != 0)({void(*_tmp2B6)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp2B5=ps;struct _fat_ptr _tmp2B4=({const char*_tmp17B="(";_tag_fat(_tmp17B,sizeof(char),2U);});struct _fat_ptr _tmp2B3=Cyc_Absyndump_pat_term(dots);_tmp2B6(Cyc_Absyndump_dumppat,_tmp2B5,_tmp2B4,_tmp2B3,({const char*_tmp17C=",";_tag_fat(_tmp17C,sizeof(char),2U);}));});
goto _LL0;}default: _tmp15C=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp15B)->f1;{struct Cyc_Absyn_Exp*e=_tmp15C;
Cyc_Absyndump_dumpexp(e);goto _LL0;}}_LL0:;}
# 993
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 998
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({void(*_tmp2B8)(void(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;});struct Cyc_List_List*_tmp2B7=fields;_tmp2B8(Cyc_Absyndump_dumpdatatypefield,_tmp2B7,({const char*_tmp17D=",";_tag_fat(_tmp17D,sizeof(char),2U);}));});}
# 1001
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp17E="=";_tag_fat(_tmp17E,sizeof(char),2U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1007
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({void(*_tmp2BA)(void(*)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;});struct Cyc_List_List*_tmp2B9=fields;_tmp2BA(Cyc_Absyndump_dumpenumfield,_tmp2B9,({const char*_tmp17F=",";_tag_fat(_tmp17F,sizeof(char),2U);}));});}
# 1011
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(1;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp180=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_stmttmp13=_tmp180;struct Cyc_Absyn_Aggrfield*_tmp181=_stmttmp13;void*_tmp187;void*_tmp186;void*_tmp185;void*_tmp184;struct Cyc_Absyn_Tqual _tmp183;void*_tmp182;_tmp182=_tmp181->name;_tmp183=_tmp181->tq;_tmp184=_tmp181->type;_tmp185=_tmp181->width;_tmp186=_tmp181->attributes;_tmp187=_tmp181->requires_clause;{struct _fat_ptr*name=_tmp182;struct Cyc_Absyn_Tqual tq=_tmp183;void*type=_tmp184;struct Cyc_Absyn_Exp*width=_tmp185;struct Cyc_List_List*atts=_tmp186;struct Cyc_Absyn_Exp*req=_tmp187;
{enum Cyc_Flags_C_Compilers _tmp188=Cyc_Flags_c_compiler;if(_tmp188 == Cyc_Flags_Gcc_c){
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;})(tq,type,Cyc_Absyndump_dump_str,name);Cyc_Absyndump_dumpatts(atts);goto _LL3;}else{
Cyc_Absyndump_dumpatts(atts);({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;})(tq,type,Cyc_Absyndump_dump_str,name);goto _LL3;}_LL3:;}
# 1018
if((unsigned)req){
Cyc_Absyndump_dump(({const char*_tmp189="@requires ";_tag_fat(_tmp189,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(req);}
# 1021
if(width != 0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp(width);}
# 1024
Cyc_Absyndump_dump_semi();}}}
# 1028
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1033
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple22{void*f1;struct _tuple0*f2;};
# 1037
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple22*pr){
{void*_tmp18A=(*pr).f1;void*_stmttmp14=_tmp18A;void*_tmp18B=_stmttmp14;switch(*((int*)_tmp18B)){case 11:
 goto _LL0;case 1:
 Cyc_Absyndump_dump(({const char*_tmp18C="_stdcall";_tag_fat(_tmp18C,sizeof(char),9U);}));goto _LL0;case 2:
 Cyc_Absyndump_dump(({const char*_tmp18D="_cdecl";_tag_fat(_tmp18D,sizeof(char),7U);}));goto _LL0;case 3:
 Cyc_Absyndump_dump(({const char*_tmp18E="_fastcall";_tag_fat(_tmp18E,sizeof(char),10U);}));goto _LL0;default:
 goto _LL0;}_LL0:;}
# 1045
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1047
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1051
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1055
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1057
void*_tmp18F=t;void*_tmp190;switch(*((int*)_tmp18F)){case 1: _tmp190=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18F)->f2;if(_tmp190 != 0){void*def=_tmp190;
return Cyc_Absyndump_is_char_ptr(def);}else{goto _LL5;}case 3: _tmp190=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_type;{void*elt_typ=_tmp190;
# 1060
while(1){
void*_tmp191=elt_typ;void*_tmp192;switch(*((int*)_tmp191)){case 1: _tmp192=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp191)->f2;if(_tmp192 != 0){void*t=_tmp192;
elt_typ=t;goto _LL7;}else{goto _LLE;}case 8: _tmp192=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp191)->f4;if(_tmp192 != 0){void*t=_tmp192;
elt_typ=t;goto _LL7;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f1)->f2 == Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;}_LL7:;}}default: _LL5:
# 1067
 return 0;};}
# 1071
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp193=vd;int _tmp19B;void*_tmp19A;void*_tmp199;void*_tmp198;struct Cyc_Absyn_Tqual _tmp197;unsigned _tmp196;void*_tmp195;enum Cyc_Absyn_Scope _tmp194;_tmp194=_tmp193->sc;_tmp195=_tmp193->name;_tmp196=_tmp193->varloc;_tmp197=_tmp193->tq;_tmp198=_tmp193->type;_tmp199=_tmp193->initializer;_tmp19A=_tmp193->attributes;_tmp19B=_tmp193->is_proto;{enum Cyc_Absyn_Scope sc=_tmp194;struct _tuple0*name=_tmp195;unsigned varloc=_tmp196;struct Cyc_Absyn_Tqual tq=_tmp197;void*type=_tmp198;struct Cyc_Absyn_Exp*initializer=_tmp199;struct Cyc_List_List*atts=_tmp19A;int is_proto=_tmp19B;
{enum Cyc_Flags_C_Compilers _tmp19C=Cyc_Flags_c_compiler;if(_tmp19C == Cyc_Flags_Gcc_c){
# 1076
if((int)sc == 3 && Cyc_Absyndump_qvar_to_Cids){
void*_tmp19D=Cyc_Absyn_compress(type);void*_stmttmp15=_tmp19D;void*_tmp19E=_stmttmp15;if(*((int*)_tmp19E)== 5){
# 1079
if(!is_proto)
Cyc_Absyndump_dumpscope(sc);
goto _LL8;}else{
Cyc_Absyndump_dumpscope(sc);}_LL8:;}else{
# 1085
Cyc_Absyndump_dumpscope(sc);}
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd;})(tq,type,Cyc_Absyndump_dumpqvar,name);
Cyc_Absyndump_dumpatts(atts);
goto _LL3;}else{
# 1090
Cyc_Absyndump_dumpatts(atts);
Cyc_Absyndump_dumpscope(sc);{
struct _RegionHandle _tmp19F=_new_region("temp");struct _RegionHandle*temp=& _tmp19F;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(type);
struct _tuple12 _tmp1A0=Cyc_Absynpp_to_tms(temp,tq,type);struct _tuple12 _stmttmp16=_tmp1A0;struct _tuple12 _tmp1A1=_stmttmp16;void*_tmp1A4;void*_tmp1A3;struct Cyc_Absyn_Tqual _tmp1A2;_tmp1A2=_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;_tmp1A4=_tmp1A1.f3;{struct Cyc_Absyn_Tqual tq=_tmp1A2;void*t=_tmp1A3;struct Cyc_List_List*tms=_tmp1A4;
# 1096
void*call_conv=(void*)& Cyc_Atts_Unused_att_val;
{struct Cyc_List_List*tms2=tms;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp1A5=(void*)tms2->hd;void*_stmttmp17=_tmp1A5;void*_tmp1A6=_stmttmp17;void*_tmp1A7;if(*((int*)_tmp1A6)== 5){_tmp1A7=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp1A6)->f2;{struct Cyc_List_List*atts=_tmp1A7;
# 1100
for(1;atts != 0;atts=atts->tl){
void*_tmp1A8=(void*)atts->hd;void*_stmttmp18=_tmp1A8;void*_tmp1A9=_stmttmp18;switch(*((int*)_tmp1A9)){case 1:
 call_conv=(void*)& Cyc_Atts_Stdcall_att_val;goto _LL15;case 2:
 call_conv=(void*)& Cyc_Atts_Cdecl_att_val;goto _LL15;case 3:
 call_conv=(void*)& Cyc_Atts_Fastcall_att_val;goto _LL15;default:
 goto _LL15;}_LL15:;}
# 1107
goto _LL10;}}else{
goto _LL10;}_LL10:;}}
# 1110
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);{
struct _tuple22 _tmp1AA=({struct _tuple22 _tmp242;_tmp242.f1=call_conv,_tmp242.f2=name;_tmp242;});struct _tuple22 pr=_tmp1AA;
({void(*_tmp2BC)(int,struct Cyc_List_List*,void(*)(struct _tuple22*),struct _tuple22*)=({(void(*)(int,struct Cyc_List_List*,void(*)(struct _tuple22*),struct _tuple22*))Cyc_Absyndump_dumptms;});int _tmp2BB=is_cp;_tmp2BC(_tmp2BB,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(tms),Cyc_Absyndump_dump_callconv_qvar,& pr);});
_npop_handler(0);goto _LL3;}}}
# 1093
;_pop_region();}}_LL3:;}
# 1117
if(initializer != 0){
Cyc_Absyndump_dump_char(61);Cyc_Absyndump_dumpexp(initializer);}
# 1120
if(do_semi)Cyc_Absyndump_dump_semi();}}
# 1123
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp1AB="@tagged ";_tag_fat(_tmp1AB,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char(123);
({void(*_tmp2C0)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp2BF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _fat_ptr _tmp2BE=({const char*_tmp1AC="<";_tag_fat(_tmp1AC,sizeof(char),2U);});struct _fat_ptr _tmp2BD=({const char*_tmp1AD=">";_tag_fat(_tmp1AD,sizeof(char),2U);});_tmp2C0(Cyc_Absyndump_dumpkindedtvar,_tmp2BF,_tmp2BE,_tmp2BD,({const char*_tmp1AE=",";_tag_fat(_tmp1AE,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1134
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump_char(125);
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1139
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp1AF=ed;void*_tmp1B2;void*_tmp1B1;enum Cyc_Absyn_Scope _tmp1B0;_tmp1B0=_tmp1AF->sc;_tmp1B1=_tmp1AF->name;_tmp1B2=_tmp1AF->fields;{enum Cyc_Absyn_Scope sc=_tmp1B0;struct _tuple0*nm=_tmp1B1;struct Cyc_Core_Opt*fields=_tmp1B2;
Cyc_Absyndump_dumpscope(sc);Cyc_Absyndump_dump(({const char*_tmp1B3="enum ";_tag_fat(_tmp1B3,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(nm);
if(fields != 0){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char(125);}}}
# 1146
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp1B4=dd;int _tmp1B9;void*_tmp1B8;void*_tmp1B7;void*_tmp1B6;enum Cyc_Absyn_Scope _tmp1B5;_tmp1B5=_tmp1B4->sc;_tmp1B6=_tmp1B4->name;_tmp1B7=_tmp1B4->tvs;_tmp1B8=_tmp1B4->fields;_tmp1B9=_tmp1B4->is_extensible;{enum Cyc_Absyn_Scope sc=_tmp1B5;struct _tuple0*name=_tmp1B6;struct Cyc_List_List*tvs=_tmp1B7;struct Cyc_Core_Opt*fields=_tmp1B8;int is_x=_tmp1B9;
Cyc_Absyndump_dumpscope(sc);
if(is_x)
Cyc_Absyndump_dump(({const char*_tmp1BA="@extensible ";_tag_fat(_tmp1BA,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp1BB="datatype ";_tag_fat(_tmp1BB,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(name);Cyc_Absyndump_dumptvars(tvs);
if(fields != 0){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char(125);}}}struct _tuple23{unsigned f1;struct _tuple0*f2;int f3;};
# 1157
static void Cyc_Absyndump_dumpexport(struct _tuple23*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1160
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char(123);
for(1;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char(125);}
# 1167
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp1BC=d->r;void*_stmttmp19=_tmp1BC;void*_tmp1BD=_stmttmp19;void*_tmp1C1;void*_tmp1C0;void*_tmp1BF;void*_tmp1BE;switch(*((int*)_tmp1BD)){case 1: _tmp1BE=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp1BE;
# 1171
{enum Cyc_Flags_C_Compilers _tmp1C2=Cyc_Flags_c_compiler;if(_tmp1C2 == Cyc_Flags_Vc_c){
Cyc_Absyndump_dumpatts((fd->i).attributes);goto _LL23;}else{
goto _LL23;}_LL23:;}
# 1175
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _tmp1C3=Cyc_Flags_c_compiler;if(_tmp1C3 == Cyc_Flags_Vc_c){
Cyc_Absyndump_dump(({const char*_tmp1C4="__inline";_tag_fat(_tmp1C4,sizeof(char),9U);}));goto _LL28;}else{
Cyc_Absyndump_dump(({const char*_tmp1C5="inline";_tag_fat(_tmp1C5,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1180
Cyc_Absyndump_dumpscope(fd->sc);{
# 1182
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->tag=5,_tmp1CC->f1=type_info;_tmp1CC;});
if(fd->cached_type != 0){
void*_tmp1C6=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmp1A=_tmp1C6;void*_tmp1C7=_stmttmp1A;struct Cyc_Absyn_FnInfo _tmp1C8;if(*((int*)_tmp1C7)== 5){_tmp1C8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C7)->f1;{struct Cyc_Absyn_FnInfo i=_tmp1C8;
# 1188
({struct Cyc_List_List*_tmp2C1=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp2C1;});goto _LL2D;}}else{
({void*_tmp1C9=0U;({int(*_tmp2C3)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp2C2=({const char*_tmp1CA="function has non-function type";_tag_fat(_tmp1CA,sizeof(char),31U);});_tmp2C3(_tmp2C2,_tag_fat(_tmp1C9,sizeof(void*),0));});});}_LL2D:;}
# 1192
{enum Cyc_Flags_C_Compilers _tmp1CB=Cyc_Flags_c_compiler;if(_tmp1CB == Cyc_Flags_Gcc_c){
({void(*_tmp2C6)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;});struct Cyc_Absyn_Tqual _tmp2C5=Cyc_Absyn_empty_tqual(0U);void*_tmp2C4=t;_tmp2C6(_tmp2C5,_tmp2C4,Cyc_Absyndump_dump_atts_qvar,fd);});goto _LL32;}else{
({void(*_tmp2C9)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;});struct Cyc_Absyn_Tqual _tmp2C8=Cyc_Absyn_empty_tqual(0U);void*_tmp2C7=t;_tmp2C9(_tmp2C8,_tmp2C7,Cyc_Absyndump_dump_callconv_fdqvar,fd);});goto _LL32;}_LL32:;}
# 1196
Cyc_Absyndump_dump_char(123);
Cyc_Absyndump_dumpstmt(fd->body,0,0);
Cyc_Absyndump_dump_char(125);
goto _LL0;}}}case 0: _tmp1BE=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp1BE;
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);goto _LL0;}case 5: _tmp1BE=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp1BE;
Cyc_Absyndump_dump_aggrdecl(ad);Cyc_Absyndump_dump_semi();goto _LL0;}case 6: _tmp1BE=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp1BE;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump_semi();goto _LL0;}case 7: _tmp1BE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp1BE;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump_semi();goto _LL0;}case 3: _tmp1BE=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_List_List*vds=_tmp1BE;
({void(*_tmp2CD)(void(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp2CC=vds;struct _fat_ptr _tmp2CB=({const char*_tmp1CD=" let ";_tag_fat(_tmp1CD,sizeof(char),6U);});struct _fat_ptr _tmp2CA=({const char*_tmp1CE=";";_tag_fat(_tmp1CE,sizeof(char),2U);});_tmp2CD(Cyc_Absyndump_dumpid,_tmp2CC,_tmp2CB,_tmp2CA,({const char*_tmp1CF=",";_tag_fat(_tmp1CF,sizeof(char),2U);}));});goto _LL0;}case 2: _tmp1BE=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp1BD)->f3;{struct Cyc_Absyn_Pat*p=_tmp1BE;struct Cyc_Absyn_Exp*e=_tmp1BF;
# 1206
Cyc_Absyndump_dump(({const char*_tmp1D0="let";_tag_fat(_tmp1D0,sizeof(char),4U);}));Cyc_Absyndump_dumppat(p);Cyc_Absyndump_dump_char(61);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}case 4: _tmp1BE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;_tmp1C0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1BD)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp1BE;struct Cyc_Absyn_Vardecl*vd=_tmp1BF;struct Cyc_Absyn_Exp*open_exp_opt=_tmp1C0;
# 1208
Cyc_Absyndump_dump(({const char*_tmp1D1="region";_tag_fat(_tmp1D1,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_tmp1D2="<";_tag_fat(_tmp1D2,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp1D3="> ";_tag_fat(_tmp1D3,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(vd->name);
if((unsigned)open_exp_opt){
Cyc_Absyndump_dump(({const char*_tmp1D4=" = open(";_tag_fat(_tmp1D4,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(open_exp_opt);Cyc_Absyndump_dump(({const char*_tmp1D5=")";_tag_fat(_tmp1D5,sizeof(char),2U);}));}
# 1212
Cyc_Absyndump_dump_semi();
goto _LL0;}case 8: _tmp1BE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp1BE;
# 1219
Cyc_Absyndump_dump(({const char*_tmp1D6="typedef";_tag_fat(_tmp1D6,sizeof(char),8U);}));{
void*t=td->defn == 0?Cyc_Absyn_new_evar(td->kind,0):(void*)_check_null(td->defn);
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd;})(td->tq,t,Cyc_Absyndump_dumptypedefname,td);
Cyc_Absyndump_dumpatts(td->atts);
Cyc_Absyndump_dump_semi();
# 1225
goto _LL0;}}case 9: _tmp1BE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;{struct _fat_ptr*v=_tmp1BE;struct Cyc_List_List*tdl=_tmp1BF;
# 1227
Cyc_Absyndump_dump(({const char*_tmp1D7="namespace";_tag_fat(_tmp1D7,sizeof(char),10U);}));Cyc_Absyndump_dump_str(v);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 10: _tmp1BE=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;{struct _tuple0*q=_tmp1BE;struct Cyc_List_List*tdl=_tmp1BF;
# 1229
Cyc_Absyndump_dump(({const char*_tmp1D8="using";_tag_fat(_tmp1D8,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(q);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 11: _tmp1BE=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;{struct Cyc_List_List*tdl=_tmp1BE;
# 1231
Cyc_Absyndump_dump(({const char*_tmp1D9="extern \"C\" ";_tag_fat(_tmp1D9,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 12: _tmp1BE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1BF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;_tmp1C0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BD)->f3;_tmp1C1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BD)->f4;{struct Cyc_List_List*tdl=_tmp1BE;struct Cyc_List_List*ovrs=_tmp1BF;struct Cyc_List_List*exs=_tmp1C0;struct _tuple10*wc=_tmp1C1;
# 1233
Cyc_Absyndump_dump(({const char*_tmp1DA="extern \"C include\" ";_tag_fat(_tmp1DA,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(tdl);
if(ovrs != 0){
Cyc_Absyndump_dump(({const char*_tmp1DB=" cyclone_override ";_tag_fat(_tmp1DB,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(ovrs);}
# 1238
if((unsigned)wc){
Cyc_Absyndump_dump(({const char*_tmp1DC=" export { * }\n";_tag_fat(_tmp1DC,sizeof(char),15U);}));{
struct _tuple10*_tmp1DD=wc;void*_tmp1DE;_tmp1DE=_tmp1DD->f2;{struct Cyc_List_List*hides=_tmp1DE;
if((unsigned)hides)
({void(*_tmp2D1)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp2D0=hides;struct _fat_ptr _tmp2CF=({const char*_tmp1DF=" cyclone_hide {";_tag_fat(_tmp1DF,sizeof(char),16U);});struct _fat_ptr _tmp2CE=({const char*_tmp1E0="}";_tag_fat(_tmp1E0,sizeof(char),2U);});_tmp2D1(Cyc_Absyndump_dumpqvar,_tmp2D0,_tmp2CF,_tmp2CE,({const char*_tmp1E1=",";_tag_fat(_tmp1E1,sizeof(char),2U);}));});}}}else{
# 1246
({void(*_tmp2D5)(void(*)(struct _tuple23*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _tuple23*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;});struct Cyc_List_List*_tmp2D4=exs;struct _fat_ptr _tmp2D3=({const char*_tmp1E2=" export {";_tag_fat(_tmp1E2,sizeof(char),10U);});struct _fat_ptr _tmp2D2=({const char*_tmp1E3="}";_tag_fat(_tmp1E3,sizeof(char),2U);});_tmp2D5(Cyc_Absyndump_dumpexport,_tmp2D4,_tmp2D3,_tmp2D2,({const char*_tmp1E4=",";_tag_fat(_tmp1E4,sizeof(char),2U);}));});}
# 1248
goto _LL0;}case 13:
 Cyc_Absyndump_dump(({const char*_tmp1E5=" __cyclone_port_on__; ";_tag_fat(_tmp1E5,sizeof(char),23U);}));goto _LL0;case 14:
 Cyc_Absyndump_dump(({const char*_tmp1E6=" __cyclone_port_off__; ";_tag_fat(_tmp1E6,sizeof(char),24U);}));goto _LL0;case 15:
 Cyc_Absyndump_dump(({const char*_tmp1E7=" __tempest_on__; ";_tag_fat(_tmp1E7,sizeof(char),18U);}));goto _LL0;default:
 Cyc_Absyndump_dump(({const char*_tmp1E8=" __tempest_off__; ";_tag_fat(_tmp1E8,sizeof(char),19U);}));goto _LL0;}_LL0:;}}
# 1256
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple14 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1U || !pr.f2){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(125);}}
# 1263
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1265
if(tms == 0){
f(a);
return;}{
# 1269
void*_tmp1E9=(void*)tms->hd;void*_stmttmp1B=_tmp1E9;void*_tmp1EA=_stmttmp1B;struct Cyc_Absyn_Tqual _tmp1F0;void*_tmp1EF;void*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;if(*((int*)_tmp1EA)== 2){_tmp1EB=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp1EA)->f1).rgn;_tmp1EC=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp1EA)->f1).nullable;_tmp1ED=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp1EA)->f1).bounds;_tmp1EE=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp1EA)->f1).zero_term;_tmp1EF=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp1EA)->f1).autoreleased;_tmp1F0=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp1EA)->f2;{void*rgn=_tmp1EB;void*nullable=_tmp1EC;void*bd=_tmp1ED;void*zt=_tmp1EE;void*rel=_tmp1EF;struct Cyc_Absyn_Tqual tq2=_tmp1F0;
# 1273
{void*_tmp1F1=Cyc_Absyn_compress(bd);void*_stmttmp1C=_tmp1F1;void*_tmp1F2=_stmttmp1C;void*_tmp1F3;if(*((int*)_tmp1F2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f1)){case 14:
 Cyc_Absyndump_dump_char(63);goto _LL5;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd)== 9){_tmp1F3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd)->f1;{struct Cyc_Absyn_Exp*e=_tmp1F3;
# 1276
Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));Cyc_Absyndump_dump_upperbound(e);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA:
# 1278
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));}_LL5:;}
# 1280
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp1F4="@zeroterm";_tag_fat(_tmp1F4,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp1F5="@nozeroterm";_tag_fat(_tmp1F5,sizeof(char),12U);}));
if(!Cyc_Absyndump_qvar_to_Cids && Cyc_Absyn_type2bool(0,rel))Cyc_Absyndump_dump(({const char*_tmp1F6="@autoreleased";_tag_fat(_tmp1F6,sizeof(char),14U);}));
{void*_tmp1F7=Cyc_Absyn_compress(rgn);void*_stmttmp1D=_tmp1F7;void*_tmp1F8=_stmttmp1D;void*_tmp1F9;switch(*((int*)_tmp1F8)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F8)->f1)){case 5:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp1FA="`H";_tag_fat(_tmp1FA,sizeof(char),3U);}));goto _LLC;case 6:
 Cyc_Absyndump_dump(({const char*_tmp1FB="`U";_tag_fat(_tmp1FB,sizeof(char),3U);}));goto _LLC;case 7:
 Cyc_Absyndump_dump(({const char*_tmp1FC="`RC";_tag_fat(_tmp1FC,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2: _tmp1F9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F8)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp1F9;
Cyc_Absyndump_dumptvar(tv);goto _LLC;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F8)->f2 == 0){
Cyc_Absyndump_dumpntyp(Cyc_Absyn_compress(rgn));goto _LLC;}else{goto _LL19;}case 8:
 Cyc_Absyndump_dump(({const char*_tmp1FD="@region(";_tag_fat(_tmp1FD,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(rgn);Cyc_Absyndump_dump(({const char*_tmp1FE=")";_tag_fat(_tmp1FE,sizeof(char),2U);}));goto _LLC;default: _LL19:
({void*_tmp1FF=0U;({struct _fat_ptr _tmp2D6=({const char*_tmp200="dumptms: bad rgn type in Pointer_mod";_tag_fat(_tmp200,sizeof(char),37U);});Cyc_Warn_impos(_tmp2D6,_tag_fat(_tmp1FF,sizeof(void*),0));});});}_LLC:;}
# 1292
Cyc_Absyndump_dumptq(tq2);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}}else{
# 1297
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp201=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmp1E=_tmp201;void*_tmp202=_stmttmp1E;if(*((int*)_tmp202)== 2){
next_is_pointer=1;goto _LL1B;}else{
goto _LL1B;}_LL1B:;}
# 1303
if(next_is_pointer)
Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char(41);
{void*_tmp203=(void*)tms->hd;void*_stmttmp1F=_tmp203;void*_tmp204=_stmttmp1F;void*_tmp20B;void*_tmp20A;void*_tmp209;void*_tmp208;int _tmp206;void*_tmp207;void*_tmp205;switch(*((int*)_tmp204)){case 0: _tmp205=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp204)->f1;{void*zeroterm=_tmp205;
# 1310
Cyc_Absyndump_dump(({const char*_tmp20C="[]";_tag_fat(_tmp20C,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp20D="@zeroterm";_tag_fat(_tmp20D,sizeof(char),10U);}));
goto _LL20;}case 1: _tmp205=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp204)->f1;_tmp207=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp204)->f2;{struct Cyc_Absyn_Exp*e=_tmp205;void*zeroterm=_tmp207;
# 1314
Cyc_Absyndump_dump_char(91);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(93);
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp20E="@zeroterm";_tag_fat(_tmp20E,sizeof(char),10U);}));
goto _LL20;}case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)== 1){_tmp205=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f1;_tmp206=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f2;_tmp207=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f3;_tmp208=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f4;_tmp209=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f5;_tmp20A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f6;_tmp20B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f7;{struct Cyc_List_List*args=_tmp205;int c_varargs=_tmp206;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp207;void*effopt=_tmp208;struct Cyc_List_List*rgn_po=_tmp209;struct Cyc_Absyn_Exp*req=_tmp20A;struct Cyc_Absyn_Exp*ens=_tmp20B;
# 1318
Cyc_Absyndump_dumpfunargs(args,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);goto _LL20;}}else{_tmp205=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp204)->f1)->f1;{struct Cyc_List_List*sl=_tmp205;
# 1320
({void(*_tmp2DA)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({(void(*)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;});struct Cyc_List_List*_tmp2D9=sl;struct _fat_ptr _tmp2D8=({const char*_tmp20F="(";_tag_fat(_tmp20F,sizeof(char),2U);});struct _fat_ptr _tmp2D7=({const char*_tmp210=")";_tag_fat(_tmp210,sizeof(char),2U);});_tmp2DA(Cyc_Absyndump_dump_str,_tmp2D9,_tmp2D8,_tmp2D7,({const char*_tmp211=",";_tag_fat(_tmp211,sizeof(char),2U);}));});goto _LL20;}}case 4: _tmp205=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp204)->f1;_tmp206=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp204)->f3;{struct Cyc_List_List*ts=_tmp205;int print_kinds=_tmp206;
# 1322
if(print_kinds)Cyc_Absyndump_dumpkindedtvars(ts);else{Cyc_Absyndump_dumptvars(ts);}goto _LL20;}case 5: _tmp205=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp204)->f2;{struct Cyc_List_List*atts=_tmp205;
Cyc_Absyndump_dumpatts(atts);goto _LL20;}default:
({void*_tmp212=0U;({struct _fat_ptr _tmp2DB=({const char*_tmp213="dumptms";_tag_fat(_tmp213,sizeof(char),8U);});Cyc_Warn_impos(_tmp2DB,_tag_fat(_tmp212,sizeof(void*),0));});});}_LL20:;}
# 1326
return;};}}
# 1330
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp214=Cyc_Absyndump_is_char_ptr(t);int cp=_tmp214;
struct _RegionHandle _tmp215=_new_region("temp");struct _RegionHandle*temp=& _tmp215;_push_region(temp);
{struct _tuple12 _tmp216=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple12 _stmttmp20=_tmp216;struct _tuple12 _tmp217=_stmttmp20;void*_tmp21A;void*_tmp219;struct Cyc_Absyn_Tqual _tmp218;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;_tmp21A=_tmp217.f3;{struct Cyc_Absyn_Tqual tq=_tmp218;void*t=_tmp219;struct Cyc_List_List*tms=_tmp21A;
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);
({int _tmp2DE=cp;struct Cyc_List_List*_tmp2DD=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(tms);void(*_tmp2DC)(void*)=f;Cyc_Absyndump_dumptms(_tmp2DE,_tmp2DD,_tmp2DC,a);});}}
# 1333
;_pop_region();}
# 1339
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1341
*Cyc_Absyndump_dump_file=f;
for(1;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp21B=0U;({struct Cyc___cycFILE*_tmp2E0=f;struct _fat_ptr _tmp2DF=({const char*_tmp21C="\n";_tag_fat(_tmp21C,sizeof(char),2U);});Cyc_fprintf(_tmp2E0,_tmp2DF,_tag_fat(_tmp21B,sizeof(void*),0));});});}
# 1347
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp21D=d->r;void*_stmttmp21=_tmp21D;void*_tmp21E=_stmttmp21;void*_tmp222;void*_tmp221;void*_tmp220;void*_tmp21F;switch(*((int*)_tmp21E)){case 0: _tmp21F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp21F;
# 1350
if((int)vd->sc == 0)return;{
struct Cyc_Absyn_Exp*init=vd->initializer;
vd->initializer=0;
if((int)vd->sc == 2)
Cyc_Absyndump_dump(({const char*_tmp223="extern ";_tag_fat(_tmp223,sizeof(char),8U);}));else{
if(!vd->is_proto){
void*_tmp224=Cyc_Absyn_compress(vd->type);void*_stmttmp22=_tmp224;void*_tmp225=_stmttmp22;if(*((int*)_tmp225)== 5){
Cyc_Absyndump_dump(({const char*_tmp226="extern ";_tag_fat(_tmp226,sizeof(char),8U);}));goto _LL17;}else{
goto _LL17;}_LL17:;}}
# 1360
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp227="\n";_tag_fat(_tmp227,sizeof(char),2U);}));
vd->initializer=init;
goto _LL0;}}case 1: _tmp21F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp21F;
# 1365
if((int)fd->sc == 0)return;
Cyc_Absyndump_dumpscope(fd->sc);{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->tag=5,_tmp229->f1=type_info;_tmp229;});
({void(*_tmp2E3)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;});struct Cyc_Absyn_Tqual _tmp2E2=Cyc_Absyn_empty_tqual(0U);void*_tmp2E1=t;_tmp2E3(_tmp2E2,_tmp2E1,Cyc_Absyndump_dump_atts_qvar,fd);});
Cyc_Absyndump_dump(({const char*_tmp228=";\n";_tag_fat(_tmp228,sizeof(char),3U);}));
goto _LL0;}}}case 5: _tmp21F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp21F;
# 1374
if((int)ad->sc == 0)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if((int)ad->sc == 1)ad->impl=0;
Cyc_Absyndump_dump_aggrdecl(ad);
ad->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp22A=";\n";_tag_fat(_tmp22A,sizeof(char),3U);}));
goto _LL0;}}case 6: _tmp21F=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp21F;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump(({const char*_tmp22B=";\n";_tag_fat(_tmp22B,sizeof(char),3U);}));goto _LL0;}case 7: _tmp21F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp21F;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump(({const char*_tmp22C=";\n";_tag_fat(_tmp22C,sizeof(char),3U);}));goto _LL0;}case 8: _tmp21F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp21F;
# 1384
if(td->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp22D="\n";_tag_fat(_tmp22D,sizeof(char),2U);}));}
# 1388
goto _LL0;}case 9: _tmp21F=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;_tmp220=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp21E)->f2;{struct _fat_ptr*v=_tmp21F;struct Cyc_List_List*ds=_tmp220;
# 1390
Cyc_Absyndump_dump(({const char*_tmp22E="namespace ";_tag_fat(_tmp22E,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(v);
Cyc_Absyndump_dump(({const char*_tmp22F="{\n";_tag_fat(_tmp22F,sizeof(char),3U);}));
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp230="}\n";_tag_fat(_tmp230,sizeof(char),3U);}));
goto _LL0;}case 10: _tmp21F=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp21E)->f2;{struct Cyc_List_List*ds=_tmp21F;
# 1398
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
goto _LL0;}case 11: _tmp21F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;{struct Cyc_List_List*ds=_tmp21F;
# 1402
Cyc_Absyndump_dump(({const char*_tmp231="extern \"C\" {";_tag_fat(_tmp231,sizeof(char),13U);}));
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp232="}\n";_tag_fat(_tmp232,sizeof(char),3U);}));
goto _LL0;}case 12: _tmp21F=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21E)->f1;_tmp220=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21E)->f2;_tmp221=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21E)->f3;_tmp222=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21E)->f4;{struct Cyc_List_List*ds=_tmp21F;struct Cyc_List_List*ovrs=_tmp220;struct Cyc_List_List*exs=_tmp221;struct _tuple10*wc=_tmp222;
# 1423 "absyndump.cyc"
goto _LL0;}default:
({void*_tmp233=0U;({int(*_tmp2E5)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp2E4=({const char*_tmp234="bad top-level declaration";_tag_fat(_tmp234,sizeof(char),26U);});_tmp2E5(_tmp2E4,_tag_fat(_tmp233,sizeof(void*),0));});});}_LL0:;}
# 1428
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
