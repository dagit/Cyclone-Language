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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 387
extern int Cyc_List_list_prefix(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 662 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 843
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
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
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 207 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 71 "kinds.h"
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 78 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 82
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Doc;
# 50
extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*,int,struct Cyc___cycFILE*);
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*,int);
# 67 "pp.h"
extern struct Cyc_PP_Doc*Cyc_PP_nil_doc (void);
# 69
extern struct Cyc_PP_Doc*Cyc_PP_blank_doc (void);
# 72
extern struct Cyc_PP_Doc*Cyc_PP_line_doc (void);
# 78
extern struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr);
# 80
extern struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*);
# 83
extern struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr,int);
# 91
extern struct Cyc_PP_Doc*Cyc_PP_nest(int,struct Cyc_PP_Doc*);
# 94
extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr);
# 108
extern struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr,struct Cyc_List_List*);
# 112
extern struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*)(void*),struct _fat_ptr,struct Cyc_List_List*);
# 117
extern struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr,struct Cyc_List_List*);
# 120
extern struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*)(void*),struct _fat_ptr,struct Cyc_List_List*);
# 123
extern struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,struct Cyc_List_List*);
# 129
extern struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 52 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 62
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 78
int Cyc_Absynpp_is_anon_aggrtype(void*);
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual,void*);struct _tuple14{int f1;struct Cyc_List_List*f2;};
# 95 "absynpp.h"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*,struct Cyc_List_List*);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 35 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int,struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int,struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int,struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int,enum Cyc_Absyn_Primop,struct Cyc_List_List*);struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual,void*,struct Cyc_Core_Opt*);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,void*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 56
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*);
# 63
static int Cyc_Absynpp_expand_typedefs;
# 67
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 69
struct _fat_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static char _tmp1[14U]="__NoCycPrefix";
# 73
static struct _fat_ptr Cyc_Absynpp_nocyc_str={_tmp1,_tmp1,_tmp1 + 14U};
static struct _fat_ptr*Cyc_Absynpp_nocyc_strptr=& Cyc_Absynpp_nocyc_str;
# 77
static int Cyc_Absynpp_add_cyc_prefix;
# 81
static int Cyc_Absynpp_to_VC;
# 84
static int Cyc_Absynpp_decls_first;
# 88
static int Cyc_Absynpp_rewrite_temp_tvars;
# 91
static int Cyc_Absynpp_print_all_tvars;
# 94
static int Cyc_Absynpp_print_all_kinds;
# 97
static int Cyc_Absynpp_print_all_effects;
# 100
static int Cyc_Absynpp_print_using_stmts;
# 105
static int Cyc_Absynpp_print_externC_stmts;
# 109
static int Cyc_Absynpp_print_full_evars;
# 112
static int Cyc_Absynpp_generate_line_directives;
# 115
static int Cyc_Absynpp_use_curr_namespace;
# 118
static int Cyc_Absynpp_print_zeroterm;
# 121
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 124
int Cyc_Absynpp_print_for_cycdoc=0;
# 145
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 164
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 184
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 204
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 224
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 245
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*(*_tmp503)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp502=Cyc_Absynpp_curr_namespace;_tmp503(_tmp502,({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=v,_tmp2->tl=0;_tmp2;}));});}
# 249
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if((_check_null(*l))->tl == 0)
*l=0;else{
# 253
Cyc_Absynpp_suppr_last(&(_check_null(*l))->tl);}}
# 257
static void Cyc_Absynpp_curr_namespace_drop (void){
({(void(*)(struct Cyc_List_List**))Cyc_Absynpp_suppr_last;})(& Cyc_Absynpp_curr_namespace);}
# 261
struct _fat_ptr Cyc_Absynpp_char_escape(char c){
char _tmp3=c;switch((int)_tmp3){case 7:
 return({const char*_tmp4="\\a";_tag_fat(_tmp4,sizeof(char),3U);});case 8:
 return({const char*_tmp5="\\b";_tag_fat(_tmp5,sizeof(char),3U);});case 12:
 return({const char*_tmp6="\\f";_tag_fat(_tmp6,sizeof(char),3U);});case 10:
 return({const char*_tmp7="\\n";_tag_fat(_tmp7,sizeof(char),3U);});case 13:
 return({const char*_tmp8="\\r";_tag_fat(_tmp8,sizeof(char),3U);});case 9:
 return({const char*_tmp9="\\t";_tag_fat(_tmp9,sizeof(char),3U);});case 11:
 return({const char*_tmpA="\\v";_tag_fat(_tmpA,sizeof(char),3U);});case 92:
 return({const char*_tmpB="\\\\";_tag_fat(_tmpB,sizeof(char),3U);});case 34:
 return({const char*_tmpC="\"";_tag_fat(_tmpC,sizeof(char),2U);});case 39:
 return({const char*_tmpD="\\'";_tag_fat(_tmpD,sizeof(char),3U);});default:
# 274
 if((int)c >= 32 &&(int)c <= 126){
struct _fat_ptr _tmpE=Cyc_Core_new_string(2U);struct _fat_ptr t=_tmpE;
({struct _fat_ptr _tmpF=_fat_ptr_plus(t,sizeof(char),0);char _tmp10=*((char*)_check_fat_subscript(_tmpF,sizeof(char),0U));char _tmp11=c;if(_get_fat_size(_tmpF,sizeof(char))== 1U &&(_tmp10 == 0 && _tmp11 != 0))_throw_arraybounds();*((char*)_tmpF.curr)=_tmp11;});
return(struct _fat_ptr)t;}else{
# 279
struct _fat_ptr _tmp12=Cyc_Core_new_string(5U);struct _fat_ptr t=_tmp12;
int j=0;
({struct _fat_ptr _tmp13=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp14=*((char*)_check_fat_subscript(_tmp13,sizeof(char),0U));char _tmp15='\\';if(_get_fat_size(_tmp13,sizeof(char))== 1U &&(_tmp14 == 0 && _tmp15 != 0))_throw_arraybounds();*((char*)_tmp13.curr)=_tmp15;});
({struct _fat_ptr _tmp16=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp17=*((char*)_check_fat_subscript(_tmp16,sizeof(char),0U));char _tmp18=(char)(48 + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_tmp16,sizeof(char))== 1U &&(_tmp17 == 0 && _tmp18 != 0))_throw_arraybounds();*((char*)_tmp16.curr)=_tmp18;});
({struct _fat_ptr _tmp19=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1A=*((char*)_check_fat_subscript(_tmp19,sizeof(char),0U));char _tmp1B=(char)(48 + ((int)c >> 3 & 7));if(_get_fat_size(_tmp19,sizeof(char))== 1U &&(_tmp1A == 0 && _tmp1B != 0))_throw_arraybounds();*((char*)_tmp19.curr)=_tmp1B;});
({struct _fat_ptr _tmp1C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp1D=*((char*)_check_fat_subscript(_tmp1C,sizeof(char),0U));char _tmp1E=(char)(48 + ((int)c & 7));if(_get_fat_size(_tmp1C,sizeof(char))== 1U &&(_tmp1D == 0 && _tmp1E != 0))_throw_arraybounds();*((char*)_tmp1C.curr)=_tmp1E;});
return(struct _fat_ptr)t;}};}
# 290
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- 1U);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= 32 ||(int)c >= 126)||(int)c == 34)||(int)c == 92)
return 1;}}
# 297
return 0;}
# 300
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 303
int n=(int)(_get_fat_size(s,sizeof(char))- 1U);
# 305
if(n > 0 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),n))== 0)-- n;{
# 307
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1F=((const char*)s.curr)[i];char _stmttmp0=_tmp1F;char _tmp20=_stmttmp0;char _tmp21;switch((int)_tmp20){case 7:
 goto _LL4;case 8: _LL4:
 goto _LL6;case 12: _LL6:
 goto _LL8;case 10: _LL8:
 goto _LLA;case 13: _LLA:
 goto _LLC;case 9: _LLC:
 goto _LLE;case 11: _LLE:
 goto _LL10;case 92: _LL10:
 goto _LL12;case 34: _LL12:
 len +=2;goto _LL0;default: _tmp21=_tmp20;{char c=_tmp21;
# 320
if((int)c >= 32 &&(int)c <= 126)++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 325
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp22=((const char*)s.curr)[i];char _stmttmp1=_tmp22;char _tmp23=_stmttmp1;char _tmp24;switch((int)_tmp23){case 7:
({struct _fat_ptr _tmp25=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp26=*((char*)_check_fat_subscript(_tmp25,sizeof(char),0U));char _tmp27='\\';if(_get_fat_size(_tmp25,sizeof(char))== 1U &&(_tmp26 == 0 && _tmp27 != 0))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({struct _fat_ptr _tmp28=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((char*)_check_fat_subscript(_tmp28,sizeof(char),0U));char _tmp2A='a';if(_get_fat_size(_tmp28,sizeof(char))== 1U &&(_tmp29 == 0 && _tmp2A != 0))_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL15;case 8:
({struct _fat_ptr _tmp2B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2C=*((char*)_check_fat_subscript(_tmp2B,sizeof(char),0U));char _tmp2D='\\';if(_get_fat_size(_tmp2B,sizeof(char))== 1U &&(_tmp2C == 0 && _tmp2D != 0))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({struct _fat_ptr _tmp2E=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((char*)_check_fat_subscript(_tmp2E,sizeof(char),0U));char _tmp30='b';if(_get_fat_size(_tmp2E,sizeof(char))== 1U &&(_tmp2F == 0 && _tmp30 != 0))_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL15;case 12:
({struct _fat_ptr _tmp31=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp32=*((char*)_check_fat_subscript(_tmp31,sizeof(char),0U));char _tmp33='\\';if(_get_fat_size(_tmp31,sizeof(char))== 1U &&(_tmp32 == 0 && _tmp33 != 0))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({struct _fat_ptr _tmp34=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((char*)_check_fat_subscript(_tmp34,sizeof(char),0U));char _tmp36='f';if(_get_fat_size(_tmp34,sizeof(char))== 1U &&(_tmp35 == 0 && _tmp36 != 0))_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL15;case 10:
({struct _fat_ptr _tmp37=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp38=*((char*)_check_fat_subscript(_tmp37,sizeof(char),0U));char _tmp39='\\';if(_get_fat_size(_tmp37,sizeof(char))== 1U &&(_tmp38 == 0 && _tmp39 != 0))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({struct _fat_ptr _tmp3A=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((char*)_check_fat_subscript(_tmp3A,sizeof(char),0U));char _tmp3C='n';if(_get_fat_size(_tmp3A,sizeof(char))== 1U &&(_tmp3B == 0 && _tmp3C != 0))_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL15;case 13:
({struct _fat_ptr _tmp3D=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp3E=*((char*)_check_fat_subscript(_tmp3D,sizeof(char),0U));char _tmp3F='\\';if(_get_fat_size(_tmp3D,sizeof(char))== 1U &&(_tmp3E == 0 && _tmp3F != 0))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({struct _fat_ptr _tmp40=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((char*)_check_fat_subscript(_tmp40,sizeof(char),0U));char _tmp42='r';if(_get_fat_size(_tmp40,sizeof(char))== 1U &&(_tmp41 == 0 && _tmp42 != 0))_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL15;case 9:
({struct _fat_ptr _tmp43=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp44=*((char*)_check_fat_subscript(_tmp43,sizeof(char),0U));char _tmp45='\\';if(_get_fat_size(_tmp43,sizeof(char))== 1U &&(_tmp44 == 0 && _tmp45 != 0))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({struct _fat_ptr _tmp46=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((char*)_check_fat_subscript(_tmp46,sizeof(char),0U));char _tmp48='t';if(_get_fat_size(_tmp46,sizeof(char))== 1U &&(_tmp47 == 0 && _tmp48 != 0))_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL15;case 11:
({struct _fat_ptr _tmp49=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4A=*((char*)_check_fat_subscript(_tmp49,sizeof(char),0U));char _tmp4B='\\';if(_get_fat_size(_tmp49,sizeof(char))== 1U &&(_tmp4A == 0 && _tmp4B != 0))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({struct _fat_ptr _tmp4C=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((char*)_check_fat_subscript(_tmp4C,sizeof(char),0U));char _tmp4E='v';if(_get_fat_size(_tmp4C,sizeof(char))== 1U &&(_tmp4D == 0 && _tmp4E != 0))_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL15;case 92:
({struct _fat_ptr _tmp4F=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp50=*((char*)_check_fat_subscript(_tmp4F,sizeof(char),0U));char _tmp51='\\';if(_get_fat_size(_tmp4F,sizeof(char))== 1U &&(_tmp50 == 0 && _tmp51 != 0))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({struct _fat_ptr _tmp52=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((char*)_check_fat_subscript(_tmp52,sizeof(char),0U));char _tmp54='\\';if(_get_fat_size(_tmp52,sizeof(char))== 1U &&(_tmp53 == 0 && _tmp54 != 0))_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL15;case 34:
({struct _fat_ptr _tmp55=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp56=*((char*)_check_fat_subscript(_tmp55,sizeof(char),0U));char _tmp57='\\';if(_get_fat_size(_tmp55,sizeof(char))== 1U &&(_tmp56 == 0 && _tmp57 != 0))_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});({struct _fat_ptr _tmp58=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp59=*((char*)_check_fat_subscript(_tmp58,sizeof(char),0U));char _tmp5A='"';if(_get_fat_size(_tmp58,sizeof(char))== 1U &&(_tmp59 == 0 && _tmp5A != 0))_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});goto _LL15;default: _tmp24=_tmp23;{char c=_tmp24;
# 339
if((int)c >= 32 &&(int)c <= 126)({struct _fat_ptr _tmp5B=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5C=*((char*)_check_fat_subscript(_tmp5B,sizeof(char),0U));char _tmp5D=c;if(_get_fat_size(_tmp5B,sizeof(char))== 1U &&(_tmp5C == 0 && _tmp5D != 0))_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});else{
# 341
unsigned char uc=(unsigned char)c;
# 344
({struct _fat_ptr _tmp5E=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp5F=*((char*)_check_fat_subscript(_tmp5E,sizeof(char),0U));char _tmp60='\\';if(_get_fat_size(_tmp5E,sizeof(char))== 1U &&(_tmp5F == 0 && _tmp60 != 0))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});
({struct _fat_ptr _tmp61=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp62=*((char*)_check_fat_subscript(_tmp61,sizeof(char),0U));char _tmp63=(char)(48 + ((int)uc >> 6 & 7));if(_get_fat_size(_tmp61,sizeof(char))== 1U &&(_tmp62 == 0 && _tmp63 != 0))_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});
({struct _fat_ptr _tmp64=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp65=*((char*)_check_fat_subscript(_tmp64,sizeof(char),0U));char _tmp66=(char)(48 + ((int)uc >> 3 & 7));if(_get_fat_size(_tmp64,sizeof(char))== 1U &&(_tmp65 == 0 && _tmp66 != 0))_throw_arraybounds();*((char*)_tmp64.curr)=_tmp66;});
({struct _fat_ptr _tmp67=_fat_ptr_plus(t,sizeof(char),j ++);char _tmp68=*((char*)_check_fat_subscript(_tmp67,sizeof(char),0U));char _tmp69=(char)(48 + ((int)uc & 7));if(_get_fat_size(_tmp67,sizeof(char))== 1U &&(_tmp68 == 0 && _tmp69 != 0))_throw_arraybounds();*((char*)_tmp67.curr)=_tmp69;});}
# 349
goto _LL15;}}_LL15:;}}
# 351
return(struct _fat_ptr)t;}}}}static char _tmp6A[9U]="restrict";
# 354
static struct _fat_ptr Cyc_Absynpp_restrict_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[9U]="volatile";
static struct _fat_ptr Cyc_Absynpp_volatile_string={_tmp6B,_tmp6B,_tmp6B + 9U};static char _tmp6C[6U]="const";
static struct _fat_ptr Cyc_Absynpp_const_str={_tmp6C,_tmp6C,_tmp6C + 6U};
static struct _fat_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _fat_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _fat_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 361
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 364
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_restrict_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_volatile_sp,_tmp6E->tl=l;_tmp6E;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=Cyc_Absynpp_const_sp,_tmp6F->tl=l;_tmp6F;});
return({struct _fat_ptr _tmp506=({const char*_tmp70="";_tag_fat(_tmp70,sizeof(char),1U);});struct _fat_ptr _tmp505=({const char*_tmp71=" ";_tag_fat(_tmp71,sizeof(char),2U);});struct _fat_ptr _tmp504=({const char*_tmp72=" ";_tag_fat(_tmp72,sizeof(char),2U);});Cyc_PP_egroup(_tmp506,_tmp505,_tmp504,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_PP_textptr,l));});}
# 370
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){return Cyc_Kinds_kind2string(ka);}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 373
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp73=Cyc_Kinds_compress_kb(c);void*_stmttmp2=_tmp73;void*_tmp74=_stmttmp2;void*_tmp75;switch(*((int*)_tmp74)){case 0: _tmp75=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp74)->f1;{struct Cyc_Absyn_Kind*k=_tmp75;
return Cyc_Absynpp_kind2string(k);}case 1:
# 377
 if(Cyc_PP_tex_output)
return({const char*_tmp76="{?}";_tag_fat(_tmp76,sizeof(char),4U);});else{
return({const char*_tmp77="?";_tag_fat(_tmp77,sizeof(char),2U);});}default: _tmp75=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp74)->f2;{struct Cyc_Absyn_Kind*k=_tmp75;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0,({struct _fat_ptr _tmp507=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_tmp4D6.f1=_tmp507;});_tmp4D6;});void*_tmp78[1];_tmp78[0]=& _tmp7A;({struct _fat_ptr _tmp508=({const char*_tmp79="<=%s";_tag_fat(_tmp79,sizeof(char),5U);});Cyc_aprintf(_tmp508,_tag_fat(_tmp78,sizeof(void*),1));});});}};}
# 384
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp7B=Cyc_Kinds_compress_kb(c);void*_stmttmp3=_tmp7B;void*_tmp7C=_stmttmp3;void*_tmp7D;switch(*((int*)_tmp7C)){case 0: _tmp7D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7C)->f1;{struct Cyc_Absyn_Kind*k=_tmp7D;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1:
# 388
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp7E="{?}";_tag_fat(_tmp7E,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp7F="?";_tag_fat(_tmp7F,sizeof(char),2U);}));}default: _tmp7D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7C)->f2;{struct Cyc_Absyn_Kind*k=_tmp7D;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}};}
# 395
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp50B=({const char*_tmp80="<";_tag_fat(_tmp80,sizeof(char),2U);});struct _fat_ptr _tmp50A=({const char*_tmp81=">";_tag_fat(_tmp81,sizeof(char),2U);});struct _fat_ptr _tmp509=({const char*_tmp82=",";_tag_fat(_tmp82,sizeof(char),2U);});Cyc_PP_egroup(_tmp50B,_tmp50A,_tmp509,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_typ2doc,ts));});}
# 399
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr _tmp83=*tv->name;struct _fat_ptr n=_tmp83;
# 403
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 405
struct _fat_ptr kstring=({const char*_tmp8B="K";_tag_fat(_tmp8B,sizeof(char),2U);});
{void*_tmp84=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp4=_tmp84;void*_tmp85=_stmttmp4;void*_tmp86;switch(*((int*)_tmp85)){case 2: _tmp86=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp85)->f2;{struct Cyc_Absyn_Kind*k=_tmp86;
_tmp86=k;goto _LL4;}case 0: _tmp86=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp85)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_tmp86;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 411
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0,_tmp4D8.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_tmp4D8;});struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0,({struct _fat_ptr _tmp50C=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_tmp4D7.f1=_tmp50C;});_tmp4D7;});void*_tmp87[2];_tmp87[0]=& _tmp89,_tmp87[1]=& _tmp8A;({struct _fat_ptr _tmp50D=({const char*_tmp88="`G%s%s";_tag_fat(_tmp88,sizeof(char),7U);});Cyc_aprintf(_tmp50D,_tag_fat(_tmp87,sizeof(void*),2));});}));}
# 413
return Cyc_PP_text(n);}
# 416
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmp8C=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp5=_tmp8C;void*_tmp8D=_stmttmp5;void*_tmp8E;switch(*((int*)_tmp8D)){case 1:
 goto _LL4;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmp8E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmp8E;
# 421
return({struct Cyc_PP_Doc*_tmp8F[3];({struct Cyc_PP_Doc*_tmp510=Cyc_Absynpp_tvar2doc(tv);_tmp8F[0]=_tmp510;}),({struct Cyc_PP_Doc*_tmp50F=Cyc_PP_text(({const char*_tmp90="::";_tag_fat(_tmp90,sizeof(char),3U);}));_tmp8F[1]=_tmp50F;}),({struct Cyc_PP_Doc*_tmp50E=Cyc_Absynpp_kind2doc(k);_tmp8F[2]=_tmp50E;});Cyc_PP_cat(_tag_fat(_tmp8F,sizeof(struct Cyc_PP_Doc*),3));});}}default: _tmp8E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D)->f2;{struct Cyc_Absyn_Kind*k=_tmp8E;
# 420
_tmp8E=k;goto _LL8;}};}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _tmp513=({const char*_tmp91="<";_tag_fat(_tmp91,sizeof(char),2U);});struct _fat_ptr _tmp512=({const char*_tmp92=">";_tag_fat(_tmp92,sizeof(char),2U);});struct _fat_ptr _tmp511=({const char*_tmp93=",";_tag_fat(_tmp93,sizeof(char),2U);});Cyc_PP_egroup(_tmp513,_tmp512,_tmp511,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_ktvar2doc,tvs));});}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _tmp516=({const char*_tmp94="<";_tag_fat(_tmp94,sizeof(char),2U);});struct _fat_ptr _tmp515=({const char*_tmp95=">";_tag_fat(_tmp95,sizeof(char),2U);});struct _fat_ptr _tmp514=({const char*_tmp96=",";_tag_fat(_tmp96,sizeof(char),2U);});Cyc_PP_egroup(_tmp516,_tmp515,_tmp514,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 435
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 439
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _tmp519=({const char*_tmp97="(";_tag_fat(_tmp97,sizeof(char),2U);});struct _fat_ptr _tmp518=({const char*_tmp98=")";_tag_fat(_tmp98,sizeof(char),2U);});struct _fat_ptr _tmp517=({const char*_tmp99=",";_tag_fat(_tmp99,sizeof(char),2U);});Cyc_PP_group(_tmp519,_tmp518,_tmp517,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_arg2doc,ts));});}
# 443
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmp9A=att;switch(*((int*)_tmp9A)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 return Cyc_PP_nil_doc();default:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));};}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(1;atts != 0;atts=atts->tl){
void*_tmp9B=(void*)atts->hd;void*_stmttmp6=_tmp9B;void*_tmp9C=_stmttmp6;switch(*((int*)_tmp9C)){case 1:
 return Cyc_PP_text(({const char*_tmp9D=" _stdcall ";_tag_fat(_tmp9D,sizeof(char),11U);}));case 2:
 return Cyc_PP_text(({const char*_tmp9E=" _cdecl ";_tag_fat(_tmp9E,sizeof(char),9U);}));case 3:
 return Cyc_PP_text(({const char*_tmp9F=" _fastcall ";_tag_fat(_tmp9F,sizeof(char),12U);}));default:
 goto _LL0;}_LL0:;}
# 460
return Cyc_PP_nil_doc();}
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpA0=(void*)atts2->hd;void*_stmttmp7=_tmpA0;void*_tmpA1=_stmttmp7;switch(*((int*)_tmpA1)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 472
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpA2[3];({struct Cyc_PP_Doc*_tmp51F=Cyc_PP_text(({const char*_tmpA3=" __declspec(";_tag_fat(_tmpA3,sizeof(char),13U);}));_tmpA2[0]=_tmp51F;}),({
struct Cyc_PP_Doc*_tmp51E=({struct _fat_ptr _tmp51D=({const char*_tmpA4="";_tag_fat(_tmpA4,sizeof(char),1U);});struct _fat_ptr _tmp51C=({const char*_tmpA5="";_tag_fat(_tmpA5,sizeof(char),1U);});struct _fat_ptr _tmp51B=({const char*_tmpA6=" ";_tag_fat(_tmpA6,sizeof(char),2U);});Cyc_PP_group(_tmp51D,_tmp51C,_tmp51B,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_noncallatt2doc,atts));});_tmpA2[1]=_tmp51E;}),({
struct Cyc_PP_Doc*_tmp51A=Cyc_PP_text(({const char*_tmpA7=")";_tag_fat(_tmpA7,sizeof(char),2U);}));_tmpA2[2]=_tmp51A;});Cyc_PP_cat(_tag_fat(_tmpA2,sizeof(struct Cyc_PP_Doc*),3));});}
# 479
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
void*_tmpA8=a;int _tmpAB;struct _fat_ptr _tmpA9;void*_tmpAC;int _tmpAA;switch(*((int*)_tmpA8)){case 0: _tmpAA=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpA8)->f1;{int i=_tmpAA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpAF=({struct Cyc_Int_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=1,_tmp4D9.f1=(unsigned long)i;_tmp4D9;});void*_tmpAD[1];_tmpAD[0]=& _tmpAF;({struct _fat_ptr _tmp520=({const char*_tmpAE="regparm(%d)";_tag_fat(_tmpAE,sizeof(char),12U);});Cyc_aprintf(_tmp520,_tag_fat(_tmpAD,sizeof(void*),1));});});}case 1:
 return({const char*_tmpB0="stdcall";_tag_fat(_tmpB0,sizeof(char),8U);});case 2:
 return({const char*_tmpB1="cdecl";_tag_fat(_tmpB1,sizeof(char),6U);});case 3:
 return({const char*_tmpB2="fastcall";_tag_fat(_tmpB2,sizeof(char),9U);});case 4:
 return({const char*_tmpB3="noreturn";_tag_fat(_tmpB3,sizeof(char),9U);});case 5:
 return({const char*_tmpB4="const";_tag_fat(_tmpB4,sizeof(char),6U);});case 6: _tmpAC=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpA8)->f1;{struct Cyc_Absyn_Exp*e=_tmpAC;
# 488
if(e == 0)return({const char*_tmpB5="aligned";_tag_fat(_tmpB5,sizeof(char),8U);});else{
# 490
enum Cyc_Flags_C_Compilers _tmpB6=Cyc_Flags_c_compiler;switch((int)_tmpB6){case Cyc_Flags_Gcc_c:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB9=({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0,({struct _fat_ptr _tmp521=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp4DA.f1=_tmp521;});_tmp4DA;});void*_tmpB7[1];_tmpB7[0]=& _tmpB9;({struct _fat_ptr _tmp522=({const char*_tmpB8="aligned(%s)";_tag_fat(_tmpB8,sizeof(char),12U);});Cyc_aprintf(_tmp522,_tag_fat(_tmpB7,sizeof(void*),1));});});case Cyc_Flags_Vc_c:
 goto _LL41;default: _LL41:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpBC=({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0,({struct _fat_ptr _tmp523=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp4DB.f1=_tmp523;});_tmp4DB;});void*_tmpBA[1];_tmpBA[0]=& _tmpBC;({struct _fat_ptr _tmp524=({const char*_tmpBB="align(%s)";_tag_fat(_tmpBB,sizeof(char),10U);});Cyc_aprintf(_tmp524,_tag_fat(_tmpBA,sizeof(void*),1));});});};}}case 7:
# 495
 return({const char*_tmpBD="packed";_tag_fat(_tmpBD,sizeof(char),7U);});case 8: _tmpA9=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpA8)->f1;{struct _fat_ptr s=_tmpA9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC0=({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0,_tmp4DC.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp4DC;});void*_tmpBE[1];_tmpBE[0]=& _tmpC0;({struct _fat_ptr _tmp525=({const char*_tmpBF="section(\"%s\")";_tag_fat(_tmpBF,sizeof(char),14U);});Cyc_aprintf(_tmp525,_tag_fat(_tmpBE,sizeof(void*),1));});});}case 9:
 return({const char*_tmpC1="nocommon";_tag_fat(_tmpC1,sizeof(char),9U);});case 10:
 return({const char*_tmpC2="shared";_tag_fat(_tmpC2,sizeof(char),7U);});case 11:
 return({const char*_tmpC3="unused";_tag_fat(_tmpC3,sizeof(char),7U);});case 12:
 return({const char*_tmpC4="weak";_tag_fat(_tmpC4,sizeof(char),5U);});case 13:
 return({const char*_tmpC5="dllimport";_tag_fat(_tmpC5,sizeof(char),10U);});case 14:
 return({const char*_tmpC6="dllexport";_tag_fat(_tmpC6,sizeof(char),10U);});case 15:
 return({const char*_tmpC7="no_instrument_function";_tag_fat(_tmpC7,sizeof(char),23U);});case 16:
 return({const char*_tmpC8="constructor";_tag_fat(_tmpC8,sizeof(char),12U);});case 17:
 return({const char*_tmpC9="destructor";_tag_fat(_tmpC9,sizeof(char),11U);});case 18:
 return({const char*_tmpCA="no_check_memory_usage";_tag_fat(_tmpCA,sizeof(char),22U);});case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8)->f1 == Cyc_Absyn_Printf_ft){_tmpAA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8)->f2;_tmpAB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8)->f3;{int n=_tmpAA;int m=_tmpAB;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpCD=({struct Cyc_Int_pa_PrintArg_struct _tmp4DE;_tmp4DE.tag=1,_tmp4DE.f1=(unsigned)n;_tmp4DE;});struct Cyc_Int_pa_PrintArg_struct _tmpCE=({struct Cyc_Int_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=1,_tmp4DD.f1=(unsigned)m;_tmp4DD;});void*_tmpCB[2];_tmpCB[0]=& _tmpCD,_tmpCB[1]=& _tmpCE;({struct _fat_ptr _tmp526=({const char*_tmpCC="format(printf,%u,%u)";_tag_fat(_tmpCC,sizeof(char),21U);});Cyc_aprintf(_tmp526,_tag_fat(_tmpCB,sizeof(void*),2));});});}}else{_tmpAA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8)->f2;_tmpAB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8)->f3;{int n=_tmpAA;int m=_tmpAB;
# 509
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD1=({struct Cyc_Int_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=1,_tmp4E0.f1=(unsigned)n;_tmp4E0;});struct Cyc_Int_pa_PrintArg_struct _tmpD2=({struct Cyc_Int_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=1,_tmp4DF.f1=(unsigned)m;_tmp4DF;});void*_tmpCF[2];_tmpCF[0]=& _tmpD1,_tmpCF[1]=& _tmpD2;({struct _fat_ptr _tmp527=({const char*_tmpD0="format(scanf,%u,%u)";_tag_fat(_tmpD0,sizeof(char),20U);});Cyc_aprintf(_tmp527,_tag_fat(_tmpCF,sizeof(void*),2));});});}}case 20: _tmpAA=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpA8)->f1;{int n=_tmpAA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD5=({struct Cyc_Int_pa_PrintArg_struct _tmp4E1;_tmp4E1.tag=1,_tmp4E1.f1=(unsigned long)n;_tmp4E1;});void*_tmpD3[1];_tmpD3[0]=& _tmpD5;({struct _fat_ptr _tmp528=({const char*_tmpD4="initializes(%d)";_tag_fat(_tmpD4,sizeof(char),16U);});Cyc_aprintf(_tmp528,_tag_fat(_tmpD3,sizeof(void*),1));});});}case 21: _tmpAA=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpA8)->f1;{int n=_tmpAA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD8=({struct Cyc_Int_pa_PrintArg_struct _tmp4E2;_tmp4E2.tag=1,_tmp4E2.f1=(unsigned long)n;_tmp4E2;});void*_tmpD6[1];_tmpD6[0]=& _tmpD8;({struct _fat_ptr _tmp529=({const char*_tmpD7="noliveunique(%d)";_tag_fat(_tmpD7,sizeof(char),17U);});Cyc_aprintf(_tmp529,_tag_fat(_tmpD6,sizeof(void*),1));});});}case 22: _tmpAA=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpA8)->f1;{int n=_tmpAA;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDB=({struct Cyc_Int_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=1,_tmp4E3.f1=(unsigned long)n;_tmp4E3;});void*_tmpD9[1];_tmpD9[0]=& _tmpDB;({struct _fat_ptr _tmp52A=({const char*_tmpDA="consume(%d)";_tag_fat(_tmpDA,sizeof(char),12U);});Cyc_aprintf(_tmp52A,_tag_fat(_tmpD9,sizeof(void*),1));});});}case 23:
 return({const char*_tmpDC="pure";_tag_fat(_tmpDC,sizeof(char),5U);});case 26:
 return({const char*_tmpDD="always_inline";_tag_fat(_tmpDD,sizeof(char),14U);});case 24: _tmpA9=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpA8)->f1;{struct _fat_ptr s=_tmpA9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=0,_tmp4E4.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp4E4;});void*_tmpDE[1];_tmpDE[0]=& _tmpE0;({struct _fat_ptr _tmp52B=({const char*_tmpDF="__mode__(\"%s\")";_tag_fat(_tmpDF,sizeof(char),15U);});Cyc_aprintf(_tmp52B,_tag_fat(_tmpDE,sizeof(void*),1));});});}case 25: _tmpA9=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpA8)->f1;{struct _fat_ptr s=_tmpA9;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE3=({struct Cyc_String_pa_PrintArg_struct _tmp4E5;_tmp4E5.tag=0,_tmp4E5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp4E5;});void*_tmpE1[1];_tmpE1[0]=& _tmpE3;({struct _fat_ptr _tmp52C=({const char*_tmpE2="alias(\"%s\")";_tag_fat(_tmpE2,sizeof(char),12U);});Cyc_aprintf(_tmp52C,_tag_fat(_tmpE1,sizeof(void*),1));});});}default:
 return({const char*_tmpE4="no_throw";_tag_fat(_tmpE4,sizeof(char),9U);});};}
# 521
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 525
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Flags_C_Compilers _tmpE5=Cyc_Flags_c_compiler;if(_tmpE5 == Cyc_Flags_Vc_c)
return Cyc_Absynpp_noncallconv2doc(atts);else{
# 530
return({struct Cyc_PP_Doc*_tmpE6[2];({struct Cyc_PP_Doc*_tmp531=Cyc_PP_text(({const char*_tmpE7=" __attribute__";_tag_fat(_tmpE7,sizeof(char),15U);}));_tmpE6[0]=_tmp531;}),({
struct Cyc_PP_Doc*_tmp530=({struct _fat_ptr _tmp52F=({const char*_tmpE8="((";_tag_fat(_tmpE8,sizeof(char),3U);});struct _fat_ptr _tmp52E=({const char*_tmpE9="))";_tag_fat(_tmpE9,sizeof(char),3U);});struct _fat_ptr _tmp52D=({const char*_tmpEA=",";_tag_fat(_tmpEA,sizeof(char),2U);});Cyc_PP_group(_tmp52F,_tmp52E,_tmp52D,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_att2doc,atts));});_tmpE6[1]=_tmp530;});Cyc_PP_cat(_tag_fat(_tmpE6,sizeof(struct Cyc_PP_Doc*),2));});};}}
# 535
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpEB=(void*)tms->hd;void*_stmttmp8=_tmpEB;void*_tmpEC=_stmttmp8;switch(*((int*)_tmpEC)){case 2:
 return 1;case 5:  {
# 540
enum Cyc_Flags_C_Compilers _tmpED=Cyc_Flags_c_compiler;if(_tmpED == Cyc_Flags_Gcc_c)
return 0;else{
return Cyc_Absynpp_next_is_pointer(tms->tl);};}default:
# 544
 return 0;};}}
# 548
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*);
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question (void){
if(!((unsigned)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpEE="{?}";_tag_fat(_tmpEE,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpEF="?";_tag_fat(_tmpEF,sizeof(char),2U);}));}}
# 558
return _check_null(Cyc_Absynpp_cache_question);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpF0="{\\lb}";_tag_fat(_tmpF0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpF1="{";_tag_fat(_tmpF1,sizeof(char),2U);}));}}
# 567
return _check_null(Cyc_Absynpp_cache_lb);}
# 569
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpF2="{\\rb}";_tag_fat(_tmpF2,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpF3="}";_tag_fat(_tmpF3,sizeof(char),2U);}));}}
# 576
return _check_null(Cyc_Absynpp_cache_rb);}
# 578
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpF4="\\$";_tag_fat(_tmpF4,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpF5="$";_tag_fat(_tmpF5,sizeof(char),2U);}));}}
# 585
return _check_null(Cyc_Absynpp_cache_dollar);}
# 587
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpF6[3];({struct Cyc_PP_Doc*_tmp534=Cyc_Absynpp_lb();_tmpF6[0]=_tmp534;}),({struct Cyc_PP_Doc*_tmp533=Cyc_PP_seq(sep,ss);_tmpF6[1]=_tmp533;}),({struct Cyc_PP_Doc*_tmp532=Cyc_Absynpp_rb();_tmpF6[2]=_tmp532;});Cyc_PP_cat(_tag_fat(_tmpF6,sizeof(struct Cyc_PP_Doc*),3));});}
# 592
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(1;tms != 0;tms=tms->tl){
void*_tmpF7=(void*)tms->hd;void*_stmttmp9=_tmpF7;void*_tmpF8=_stmttmp9;void*_tmpF9;switch(*((int*)_tmpF8)){case 0:
({void*_tmpFA=0U;({struct Cyc___cycFILE*_tmp536=Cyc_stderr;struct _fat_ptr _tmp535=({const char*_tmpFB="Carray_mod ";_tag_fat(_tmpFB,sizeof(char),12U);});Cyc_fprintf(_tmp536,_tmp535,_tag_fat(_tmpFA,sizeof(void*),0));});});goto _LL0;case 1:
({void*_tmpFC=0U;({struct Cyc___cycFILE*_tmp538=Cyc_stderr;struct _fat_ptr _tmp537=({const char*_tmpFD="ConstArray_mod ";_tag_fat(_tmpFD,sizeof(char),16U);});Cyc_fprintf(_tmp538,_tmp537,_tag_fat(_tmpFC,sizeof(void*),0));});});goto _LL0;case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF8)->f1)== 1){_tmpF9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF8)->f1)->f1;{struct Cyc_List_List*ys=_tmpF9;
# 598
({void*_tmpFE=0U;({struct Cyc___cycFILE*_tmp53A=Cyc_stderr;struct _fat_ptr _tmp539=({const char*_tmpFF="Function_mod(";_tag_fat(_tmpFF,sizeof(char),14U);});Cyc_fprintf(_tmp53A,_tmp539,_tag_fat(_tmpFE,sizeof(void*),0));});});
for(1;ys != 0;ys=ys->tl){
struct _fat_ptr*_tmp100=(*((struct _tuple8*)ys->hd)).f1;struct _fat_ptr*v=_tmp100;
if(v == 0)({void*_tmp101=0U;({struct Cyc___cycFILE*_tmp53C=Cyc_stderr;struct _fat_ptr _tmp53B=({const char*_tmp102="?";_tag_fat(_tmp102,sizeof(char),2U);});Cyc_fprintf(_tmp53C,_tmp53B,_tag_fat(_tmp101,sizeof(void*),0));});});else{
({void*_tmp103=0U;Cyc_fprintf(Cyc_stderr,*v,_tag_fat(_tmp103,sizeof(void*),0));});}
if(ys->tl != 0)({void*_tmp104=0U;({struct Cyc___cycFILE*_tmp53E=Cyc_stderr;struct _fat_ptr _tmp53D=({const char*_tmp105=",";_tag_fat(_tmp105,sizeof(char),2U);});Cyc_fprintf(_tmp53E,_tmp53D,_tag_fat(_tmp104,sizeof(void*),0));});});}
# 605
({void*_tmp106=0U;({struct Cyc___cycFILE*_tmp540=Cyc_stderr;struct _fat_ptr _tmp53F=({const char*_tmp107=") ";_tag_fat(_tmp107,sizeof(char),3U);});Cyc_fprintf(_tmp540,_tmp53F,_tag_fat(_tmp106,sizeof(void*),0));});});
goto _LL0;}}else{
# 608
({void*_tmp108=0U;({struct Cyc___cycFILE*_tmp542=Cyc_stderr;struct _fat_ptr _tmp541=({const char*_tmp109="Function_mod()";_tag_fat(_tmp109,sizeof(char),15U);});Cyc_fprintf(_tmp542,_tmp541,_tag_fat(_tmp108,sizeof(void*),0));});});goto _LL0;}case 5:
({void*_tmp10A=0U;({struct Cyc___cycFILE*_tmp544=Cyc_stderr;struct _fat_ptr _tmp543=({const char*_tmp10B="Attributes_mod ";_tag_fat(_tmp10B,sizeof(char),16U);});Cyc_fprintf(_tmp544,_tmp543,_tag_fat(_tmp10A,sizeof(void*),0));});});goto _LL0;case 4:
({void*_tmp10C=0U;({struct Cyc___cycFILE*_tmp546=Cyc_stderr;struct _fat_ptr _tmp545=({const char*_tmp10D="TypeParams_mod ";_tag_fat(_tmp10D,sizeof(char),16U);});Cyc_fprintf(_tmp546,_tmp545,_tag_fat(_tmp10C,sizeof(void*),0));});});goto _LL0;default:
({void*_tmp10E=0U;({struct Cyc___cycFILE*_tmp548=Cyc_stderr;struct _fat_ptr _tmp547=({const char*_tmp10F="Pointer_mod ";_tag_fat(_tmp10F,sizeof(char),13U);});Cyc_fprintf(_tmp548,_tmp547,_tag_fat(_tmp10E,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 613
({void*_tmp110=0U;({struct Cyc___cycFILE*_tmp54A=Cyc_stderr;struct _fat_ptr _tmp549=({const char*_tmp111="\n";_tag_fat(_tmp111,sizeof(char),2U);});Cyc_fprintf(_tmp54A,_tmp549,_tag_fat(_tmp110,sizeof(void*),0));});});}
# 616
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*);
# 618
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp152[3];({struct Cyc_PP_Doc*_tmp54C=Cyc_PP_text(({const char*_tmp153="(";_tag_fat(_tmp153,sizeof(char),2U);}));_tmp152[0]=_tmp54C;}),_tmp152[1]=rest,({struct Cyc_PP_Doc*_tmp54B=Cyc_PP_text(({const char*_tmp154=")";_tag_fat(_tmp154,sizeof(char),2U);}));_tmp152[2]=_tmp54B;});Cyc_PP_cat(_tag_fat(_tmp152,sizeof(struct Cyc_PP_Doc*),3));});
void*_tmp112=(void*)tms->hd;void*_stmttmpA=_tmp112;void*_tmp113=_stmttmpA;struct Cyc_Absyn_Tqual _tmp119;void*_tmp118;void*_tmp117;void*_tmp116;int _tmp11B;unsigned _tmp11A;void*_tmp115;void*_tmp114;switch(*((int*)_tmp113)){case 0: _tmp114=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp113)->f1;{void*zeroterm=_tmp114;
# 624
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp11C[2];_tmp11C[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_tmp54E=Cyc_PP_text(({const char*_tmp11D="[]@zeroterm";_tag_fat(_tmp11D,sizeof(char),12U);}));_tmp11C[1]=_tmp54E;}):({struct Cyc_PP_Doc*_tmp54D=Cyc_PP_text(({const char*_tmp11E="[]";_tag_fat(_tmp11E,sizeof(char),3U);}));_tmp11C[1]=_tmp54D;});Cyc_PP_cat(_tag_fat(_tmp11C,sizeof(struct Cyc_PP_Doc*),2));});}case 1: _tmp114=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp113)->f1;_tmp115=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp113)->f2;{struct Cyc_Absyn_Exp*e=_tmp114;void*zeroterm=_tmp115;
# 628
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp11F[4];_tmp11F[0]=rest,({struct Cyc_PP_Doc*_tmp552=Cyc_PP_text(({const char*_tmp120="[";_tag_fat(_tmp120,sizeof(char),2U);}));_tmp11F[1]=_tmp552;}),({struct Cyc_PP_Doc*_tmp551=Cyc_Absynpp_exp2doc(e);_tmp11F[2]=_tmp551;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_tmp550=Cyc_PP_text(({const char*_tmp121="]@zeroterm";_tag_fat(_tmp121,sizeof(char),11U);}));_tmp11F[3]=_tmp550;}):({struct Cyc_PP_Doc*_tmp54F=Cyc_PP_text(({const char*_tmp122="]";_tag_fat(_tmp122,sizeof(char),2U);}));_tmp11F[3]=_tmp54F;});Cyc_PP_cat(_tag_fat(_tmp11F,sizeof(struct Cyc_PP_Doc*),4));});}case 3: _tmp114=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp113)->f1;{void*args=_tmp114;
# 632
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp123=args;unsigned _tmp125;void*_tmp12B;void*_tmp12A;void*_tmp129;void*_tmp128;void*_tmp127;int _tmp126;void*_tmp124;if(*((int*)_tmp123)== 1){_tmp124=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f1;_tmp126=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f2;_tmp127=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f3;_tmp128=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f4;_tmp129=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f5;_tmp12A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f6;_tmp12B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp123)->f7;{struct Cyc_List_List*args2=_tmp124;int c_varargs=_tmp126;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp127;void*effopt=_tmp128;struct Cyc_List_List*rgn_po=_tmp129;struct Cyc_Absyn_Exp*req=_tmp12A;struct Cyc_Absyn_Exp*ens=_tmp12B;
# 635
return({struct Cyc_PP_Doc*_tmp12C[2];_tmp12C[0]=rest,({struct Cyc_PP_Doc*_tmp553=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_tmp12C[1]=_tmp553;});Cyc_PP_cat(_tag_fat(_tmp12C,sizeof(struct Cyc_PP_Doc*),2));});}}else{_tmp124=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp123)->f1;_tmp125=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp123)->f2;{struct Cyc_List_List*sl=_tmp124;unsigned loc=_tmp125;
# 637
return({struct Cyc_PP_Doc*_tmp12D[2];_tmp12D[0]=rest,({struct Cyc_PP_Doc*_tmp557=({struct _fat_ptr _tmp556=({const char*_tmp12E="(";_tag_fat(_tmp12E,sizeof(char),2U);});struct _fat_ptr _tmp555=({const char*_tmp12F=")";_tag_fat(_tmp12F,sizeof(char),2U);});struct _fat_ptr _tmp554=({const char*_tmp130=",";_tag_fat(_tmp130,sizeof(char),2U);});Cyc_PP_group(_tmp556,_tmp555,_tmp554,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_PP_textptr,sl));});_tmp12D[1]=_tmp557;});Cyc_PP_cat(_tag_fat(_tmp12D,sizeof(struct Cyc_PP_Doc*),2));});}};}}case 5: _tmp114=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp113)->f2;{struct Cyc_List_List*atts=_tmp114;
# 640
enum Cyc_Flags_C_Compilers _tmp131=Cyc_Flags_c_compiler;if(_tmp131 == Cyc_Flags_Gcc_c){
# 642
if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp132[2];_tmp132[0]=rest,({struct Cyc_PP_Doc*_tmp558=Cyc_Absynpp_atts2doc(atts);_tmp132[1]=_tmp558;});Cyc_PP_cat(_tag_fat(_tmp132,sizeof(struct Cyc_PP_Doc*),2));});}else{
# 646
if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp133[2];({struct Cyc_PP_Doc*_tmp559=Cyc_Absynpp_callconv2doc(atts);_tmp133[0]=_tmp559;}),_tmp133[1]=rest;Cyc_PP_cat(_tag_fat(_tmp133,sizeof(struct Cyc_PP_Doc*),2));});
return rest;};}case 4: _tmp114=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp113)->f1;_tmp11A=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp113)->f2;_tmp11B=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp113)->f3;{struct Cyc_List_List*ts=_tmp114;unsigned loc=_tmp11A;int print_kinds=_tmp11B;
# 651
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_tmp134[2];_tmp134[0]=rest,({struct Cyc_PP_Doc*_tmp55A=Cyc_Absynpp_ktvars2doc(ts);_tmp134[1]=_tmp55A;});Cyc_PP_cat(_tag_fat(_tmp134,sizeof(struct Cyc_PP_Doc*),2));});else{
# 655
return({struct Cyc_PP_Doc*_tmp135[2];_tmp135[0]=rest,({struct Cyc_PP_Doc*_tmp55B=Cyc_Absynpp_tvars2doc(ts);_tmp135[1]=_tmp55B;});Cyc_PP_cat(_tag_fat(_tmp135,sizeof(struct Cyc_PP_Doc*),2));});}}default: _tmp114=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp113)->f1).rgn;_tmp115=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp113)->f1).nullable;_tmp116=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp113)->f1).bounds;_tmp117=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp113)->f1).zero_term;_tmp118=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp113)->f1).autoreleased;_tmp119=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp113)->f2;{void*rgn=_tmp114;void*nullable=_tmp115;void*bd=_tmp116;void*zt=_tmp117;void*rel=_tmp118;struct Cyc_Absyn_Tqual tq2=_tmp119;
# 659
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*reld=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_tmp136=Cyc_Absyn_compress(bd);void*_stmttmpB=_tmp136;void*_tmp137=_stmttmpB;void*_tmp138;if(*((int*)_tmp137)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)){case 14:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2 != 0){_tmp138=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2)->hd;{void*targ=_tmp138;
# 668
{void*_tmp139=Cyc_Absyn_compress(targ);void*_stmttmpC=_tmp139;void*_tmp13A=_stmttmpC;void*_tmp13B;if(*((int*)_tmp13A)== 9){_tmp13B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp13A)->f1;{struct Cyc_Absyn_Exp*e=_tmp13B;
# 670
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp13C="*";_tag_fat(_tmp13C,sizeof(char),2U);}):({const char*_tmp13D="@";_tag_fat(_tmp13D,sizeof(char),2U);}));{
struct _tuple12 _tmp13E=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmpD=_tmp13E;struct _tuple12 _tmp13F=_stmttmpD;int _tmp141;unsigned _tmp140;_tmp140=_tmp13F.f1;_tmp141=_tmp13F.f2;{unsigned val=_tmp140;int known=_tmp141;
if(!known || val != 1U)
ptr=({struct Cyc_PP_Doc*_tmp142[4];_tmp142[0]=ptr,({struct Cyc_PP_Doc*_tmp55E=Cyc_Absynpp_lb();_tmp142[1]=_tmp55E;}),({struct Cyc_PP_Doc*_tmp55D=Cyc_Absynpp_exp2doc(e);_tmp142[2]=_tmp55D;}),({struct Cyc_PP_Doc*_tmp55C=Cyc_Absynpp_rb();_tmp142[3]=_tmp55C;});Cyc_PP_cat(_tag_fat(_tmp142,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}}}}else{
# 676
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp143="*";_tag_fat(_tmp143,sizeof(char),2U);}):({const char*_tmp144="@";_tag_fat(_tmp144,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp145[4];_tmp145[0]=ptr,({struct Cyc_PP_Doc*_tmp561=Cyc_Absynpp_lb();_tmp145[1]=_tmp561;}),({struct Cyc_PP_Doc*_tmp560=Cyc_Absynpp_typ2doc(targ);_tmp145[2]=_tmp560;}),({struct Cyc_PP_Doc*_tmp55F=Cyc_Absynpp_rb();_tmp145[3]=_tmp55F;});Cyc_PP_cat(_tag_fat(_tmp145,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}_LL1E:;}
# 680
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C:
# 682
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_tmp146="*";_tag_fat(_tmp146,sizeof(char),2U);}):({const char*_tmp147="@";_tag_fat(_tmp147,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp148[4];_tmp148[0]=ptr,({struct Cyc_PP_Doc*_tmp564=Cyc_Absynpp_lb();_tmp148[1]=_tmp564;}),({struct Cyc_PP_Doc*_tmp563=Cyc_Absynpp_typ2doc(bd);_tmp148[2]=_tmp563;}),({struct Cyc_PP_Doc*_tmp562=Cyc_Absynpp_rb();_tmp148[3]=_tmp562;});Cyc_PP_cat(_tag_fat(_tmp148,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL17;}_LL17:;}
# 687
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp149="@zeroterm";_tag_fat(_tmp149,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_tmp14A="@nozeroterm";_tag_fat(_tmp14A,sizeof(char),12U);}));}
if(Cyc_Absyn_type2bool(0,rel))
reld=Cyc_PP_text(({const char*_tmp14B="@autoreleased";_tag_fat(_tmp14B,sizeof(char),14U);}));}
# 695
{void*_tmp14C=Cyc_Absyn_compress(rgn);void*_stmttmpE=_tmp14C;void*_tmp14D=_stmttmpE;switch(*((int*)_tmp14D)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14D)->f1)== 5)
goto _LL26;else{goto _LL2B;}case 1: if(Cyc_Absynpp_print_for_cycdoc)
goto _LL26;else{goto _LL2B;}default: _LL2B:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 700
struct Cyc_PP_Doc*spacer0=reld != mt && ztd != mt?Cyc_PP_text(({const char*_tmp151=" ";_tag_fat(_tmp151,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer1=tqd != mt &&((ztd != mt || rgd != mt)|| reld != mt)?Cyc_PP_text(({const char*_tmp150=" ";_tag_fat(_tmp150,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp14F=" ";_tag_fat(_tmp14F,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp14E[9];_tmp14E[0]=ptr,_tmp14E[1]=ztd,_tmp14E[2]=spacer0,_tmp14E[3]=reld,_tmp14E[4]=rgd,_tmp14E[5]=spacer1,_tmp14E[6]=tqd,_tmp14E[7]=spacer2,_tmp14E[8]=rest;Cyc_PP_cat(_tag_fat(_tmp14E,sizeof(struct Cyc_PP_Doc*),9));});}}};}}
# 707
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp155=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp155;void*_tmp156=_stmttmpF;if(*((int*)_tmp156)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)){case 5:
 return Cyc_PP_text(({const char*_tmp157="`H";_tag_fat(_tmp157,sizeof(char),3U);}));case 6:
 return Cyc_PP_text(({const char*_tmp158="`U";_tag_fat(_tmp158,sizeof(char),3U);}));case 7:
 return Cyc_PP_text(({const char*_tmp159="`RC";_tag_fat(_tmp159,sizeof(char),4U);}));default: goto _LL7;}else{_LL7:
 return Cyc_Absynpp_ntyp2doc(t);};}
# 717
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 721
void*_tmp15A=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp15A;void*_tmp15B=_stmttmp10;void*_tmp15C;if(*((int*)_tmp15B)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2 != 0){_tmp15C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2)->hd;{void*r=_tmp15C;
({struct Cyc_List_List*_tmp566=({struct Cyc_List_List*_tmp15D=_cycalloc(sizeof(*_tmp15D));({struct Cyc_PP_Doc*_tmp565=Cyc_Absynpp_rgn2doc(r);_tmp15D->hd=_tmp565;}),_tmp15D->tl=*rgions;_tmp15D;});*rgions=_tmp566;});goto _LL0;}}else{goto _LL5;}case 9: _tmp15C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2;{struct Cyc_List_List*ts=_tmp15C;
# 724
for(1;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 727
goto _LL0;}default: goto _LL5;}else{_LL5:
({struct Cyc_List_List*_tmp568=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));({struct Cyc_PP_Doc*_tmp567=Cyc_Absynpp_typ2doc(t);_tmp15E->hd=_tmp567;}),_tmp15E->tl=*effects;_tmp15E;});*effects=_tmp568;});goto _LL0;}_LL0:;}
# 732
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(rgions);
effects=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(effects);
if(rgions == 0 && effects != 0)
return({struct _fat_ptr _tmp56B=({const char*_tmp15F="";_tag_fat(_tmp15F,sizeof(char),1U);});struct _fat_ptr _tmp56A=({const char*_tmp160="";_tag_fat(_tmp160,sizeof(char),1U);});struct _fat_ptr _tmp569=({const char*_tmp161="+";_tag_fat(_tmp161,sizeof(char),2U);});Cyc_PP_group(_tmp56B,_tmp56A,_tmp569,effects);});else{
# 740
struct Cyc_PP_Doc*_tmp162=({struct _fat_ptr _tmp56C=({const char*_tmp167=",";_tag_fat(_tmp167,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp56C,rgions);});struct Cyc_PP_Doc*doc1=_tmp162;
return({struct _fat_ptr _tmp571=({const char*_tmp163="";_tag_fat(_tmp163,sizeof(char),1U);});struct _fat_ptr _tmp570=({const char*_tmp164="";_tag_fat(_tmp164,sizeof(char),1U);});struct _fat_ptr _tmp56F=({const char*_tmp165="+";_tag_fat(_tmp165,sizeof(char),2U);});Cyc_PP_group(_tmp571,_tmp570,_tmp56F,({struct Cyc_List_List*(*_tmp56E)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp56D=effects;_tmp56E(_tmp56D,({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->hd=doc1,_tmp166->tl=0;_tmp166;}));}));});}}
# 745
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp168=k;if(_tmp168 == Cyc_Absyn_StructA)
return Cyc_PP_text(({const char*_tmp169="struct ";_tag_fat(_tmp169,sizeof(char),8U);}));else{
return Cyc_PP_text(({const char*_tmp16A="union ";_tag_fat(_tmp16A,sizeof(char),7U);}));};}
# 753
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp16B=t;enum Cyc_Absyn_AggrKind _tmp16F;void*_tmp16E;void*_tmp16D;struct _fat_ptr _tmp171;union Cyc_Absyn_AggrInfo _tmp172;int _tmp170;enum Cyc_Absyn_Size_of _tmp174;enum Cyc_Absyn_Sign _tmp173;union Cyc_Absyn_DatatypeFieldInfo _tmp175;void*_tmp16C;union Cyc_Absyn_DatatypeInfo _tmp176;switch(*((int*)_tmp16B)){case 4:
# 757
 return Cyc_PP_text(({const char*_tmp177="[[[array]]]";_tag_fat(_tmp177,sizeof(char),12U);}));case 5:
 return Cyc_PP_nil_doc();case 3:
 return Cyc_PP_nil_doc();case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)){case 0:
# 761
 s=Cyc_PP_text(({const char*_tmp178="void";_tag_fat(_tmp178,sizeof(char),5U);}));goto _LL0;case 18: _tmp176=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;_tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;{union Cyc_Absyn_DatatypeInfo tu_info=_tmp176;struct Cyc_List_List*ts=_tmp16C;
# 777
{union Cyc_Absyn_DatatypeInfo _tmp180=tu_info;int _tmp182;void*_tmp181;if((_tmp180.UnknownDatatype).tag == 1){_tmp181=((_tmp180.UnknownDatatype).val).name;_tmp182=((_tmp180.UnknownDatatype).val).is_extensible;{struct _tuple0*n=_tmp181;int is_x=_tmp182;
_tmp181=n;_tmp182=is_x;goto _LL4B;}}else{_tmp181=(*(_tmp180.KnownDatatype).val)->name;_tmp182=(*(_tmp180.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_tmp181;int is_x=_tmp182;
# 780
struct Cyc_PP_Doc*_tmp183=Cyc_PP_text(({const char*_tmp187="datatype ";_tag_fat(_tmp187,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp183;
struct Cyc_PP_Doc*_tmp184=is_x?Cyc_PP_text(({const char*_tmp186="@extensible ";_tag_fat(_tmp186,sizeof(char),13U);})): Cyc_PP_nil_doc();struct Cyc_PP_Doc*ext=_tmp184;
s=({struct Cyc_PP_Doc*_tmp185[4];_tmp185[0]=ext,_tmp185[1]=kw,({struct Cyc_PP_Doc*_tmp573=Cyc_Absynpp_qvar2doc(n);_tmp185[2]=_tmp573;}),({struct Cyc_PP_Doc*_tmp572=Cyc_Absynpp_tps2doc(ts);_tmp185[3]=_tmp572;});Cyc_PP_cat(_tag_fat(_tmp185,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL47;}}_LL47:;}
# 785
goto _LL0;}case 19: _tmp175=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;_tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp175;struct Cyc_List_List*ts=_tmp16C;
# 787
{union Cyc_Absyn_DatatypeFieldInfo _tmp188=tuf_info;int _tmp18A;void*_tmp18B;void*_tmp189;if((_tmp188.UnknownDatatypefield).tag == 1){_tmp189=((_tmp188.UnknownDatatypefield).val).datatype_name;_tmp18B=((_tmp188.UnknownDatatypefield).val).field_name;_tmp18A=((_tmp188.UnknownDatatypefield).val).is_extensible;{struct _tuple0*tname=_tmp189;struct _tuple0*fname=_tmp18B;int is_x=_tmp18A;
# 789
_tmp189=tname;_tmp18A=is_x;_tmp18B=fname;goto _LL50;}}else{_tmp189=(((_tmp188.KnownDatatypefield).val).f1)->name;_tmp18A=(((_tmp188.KnownDatatypefield).val).f1)->is_extensible;_tmp18B=(((_tmp188.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_tmp189;int is_x=_tmp18A;struct _tuple0*fname=_tmp18B;
# 792
struct Cyc_PP_Doc*_tmp18C=Cyc_PP_text(is_x?({const char*_tmp18F="@extensible datatype ";_tag_fat(_tmp18F,sizeof(char),22U);}):({const char*_tmp190="datatype ";_tag_fat(_tmp190,sizeof(char),10U);}));struct Cyc_PP_Doc*kw=_tmp18C;
s=({struct Cyc_PP_Doc*_tmp18D[4];_tmp18D[0]=kw,({struct Cyc_PP_Doc*_tmp576=Cyc_Absynpp_qvar2doc(tname);_tmp18D[1]=_tmp576;}),({struct Cyc_PP_Doc*_tmp575=Cyc_PP_text(({const char*_tmp18E=".";_tag_fat(_tmp18E,sizeof(char),2U);}));_tmp18D[2]=_tmp575;}),({struct Cyc_PP_Doc*_tmp574=Cyc_Absynpp_qvar2doc(fname);_tmp18D[3]=_tmp574;});Cyc_PP_cat(_tag_fat(_tmp18D,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL4C;}}_LL4C:;}
# 796
goto _LL0;}case 1: _tmp173=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;_tmp174=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f2;{enum Cyc_Absyn_Sign sn=_tmp173;enum Cyc_Absyn_Size_of sz=_tmp174;
# 798
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _tmp191=sn;switch((int)_tmp191){case Cyc_Absyn_None:
 goto _LL55;case Cyc_Absyn_Signed: _LL55:
 sns=({const char*_tmp192="";_tag_fat(_tmp192,sizeof(char),1U);});goto _LL51;default:
 sns=({const char*_tmp193="unsigned ";_tag_fat(_tmp193,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 805
{enum Cyc_Absyn_Size_of _tmp194=sz;switch((int)_tmp194){case Cyc_Absyn_Char_sz:
# 807
{enum Cyc_Absyn_Sign _tmp195=sn;switch((int)_tmp195){case Cyc_Absyn_None:
 sns=({const char*_tmp196="";_tag_fat(_tmp196,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed:
 sns=({const char*_tmp197="signed ";_tag_fat(_tmp197,sizeof(char),8U);});goto _LL65;default:
 sns=({const char*_tmp198="unsigned ";_tag_fat(_tmp198,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 812
ts=({const char*_tmp199="char";_tag_fat(_tmp199,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz:
 ts=({const char*_tmp19A="short";_tag_fat(_tmp19A,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz:
 ts=({const char*_tmp19B="int";_tag_fat(_tmp19B,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz:
 ts=({const char*_tmp19C="long";_tag_fat(_tmp19C,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz:
 goto _LL64;default: _LL64:
# 819
{enum Cyc_Flags_C_Compilers _tmp19D=Cyc_Flags_c_compiler;if(_tmp19D == Cyc_Flags_Gcc_c){
ts=({const char*_tmp19E="long long";_tag_fat(_tmp19E,sizeof(char),10U);});goto _LL6C;}else{
ts=({const char*_tmp19F="__int64";_tag_fat(_tmp19F,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 823
goto _LL58;}_LL58:;}
# 825
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A2=({struct Cyc_String_pa_PrintArg_struct _tmp4E7;_tmp4E7.tag=0,_tmp4E7.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_tmp4E7;});struct Cyc_String_pa_PrintArg_struct _tmp1A3=({struct Cyc_String_pa_PrintArg_struct _tmp4E6;_tmp4E6.tag=0,_tmp4E6.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_tmp4E6;});void*_tmp1A0[2];_tmp1A0[0]=& _tmp1A2,_tmp1A0[1]=& _tmp1A3;({struct _fat_ptr _tmp577=({const char*_tmp1A1="%s%s";_tag_fat(_tmp1A1,sizeof(char),5U);});Cyc_aprintf(_tmp577,_tag_fat(_tmp1A0,sizeof(void*),2));});}));
goto _LL0;}case 2: _tmp170=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;{int i=_tmp170;
# 828
{int _tmp1A4=i;switch((int)_tmp1A4){case 0:
 s=Cyc_PP_text(({const char*_tmp1A5="float";_tag_fat(_tmp1A5,sizeof(char),6U);}));goto _LL71;case 1:
 s=Cyc_PP_text(({const char*_tmp1A6="double";_tag_fat(_tmp1A6,sizeof(char),7U);}));goto _LL71;default:
 s=Cyc_PP_text(({const char*_tmp1A7="long double";_tag_fat(_tmp1A7,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 833
goto _LL0;}case 20: _tmp172=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;_tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;{union Cyc_Absyn_AggrInfo info=_tmp172;struct Cyc_List_List*ts=_tmp16C;
# 836
struct _tuple11 _tmp1A9=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp11=_tmp1A9;struct _tuple11 _tmp1AA=_stmttmp11;void*_tmp1AC;enum Cyc_Absyn_AggrKind _tmp1AB;_tmp1AB=_tmp1AA.f1;_tmp1AC=_tmp1AA.f2;{enum Cyc_Absyn_AggrKind k=_tmp1AB;struct _tuple0*n=_tmp1AC;
s=({struct Cyc_PP_Doc*_tmp1AD[3];({struct Cyc_PP_Doc*_tmp57A=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AD[0]=_tmp57A;}),({struct Cyc_PP_Doc*_tmp579=Cyc_Absynpp_qvar2doc(n);_tmp1AD[1]=_tmp579;}),({struct Cyc_PP_Doc*_tmp578=Cyc_Absynpp_tps2doc(ts);_tmp1AD[2]=_tmp578;});Cyc_PP_cat(_tag_fat(_tmp1AD,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}}case 16: _tmp16C=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;{struct Cyc_List_List*fs=_tmp16C;
# 845
s=({struct Cyc_PP_Doc*_tmp1AF[4];({struct Cyc_PP_Doc*_tmp57E=Cyc_PP_text(({const char*_tmp1B0="enum ";_tag_fat(_tmp1B0,sizeof(char),6U);}));_tmp1AF[0]=_tmp57E;}),({struct Cyc_PP_Doc*_tmp57D=Cyc_Absynpp_lb();_tmp1AF[1]=_tmp57D;}),({struct Cyc_PP_Doc*_tmp57C=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_tmp1AF[2]=_tmp57C;}),({struct Cyc_PP_Doc*_tmp57B=Cyc_Absynpp_rb();_tmp1AF[3]=_tmp57B;});Cyc_PP_cat(_tag_fat(_tmp1AF,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 15: _tmp16C=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;{struct _tuple0*n=_tmp16C;
s=({struct Cyc_PP_Doc*_tmp1B1[2];({struct Cyc_PP_Doc*_tmp580=Cyc_PP_text(({const char*_tmp1B2="enum ";_tag_fat(_tmp1B2,sizeof(char),6U);}));_tmp1B1[0]=_tmp580;}),({struct Cyc_PP_Doc*_tmp57F=Cyc_Absynpp_qvar2doc(n);_tmp1B1[1]=_tmp57F;});Cyc_PP_cat(_tag_fat(_tmp1B1,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 17: _tmp171=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f1)->f1;{struct _fat_ptr t=_tmp171;
# 849
s=Cyc_PP_text(t);goto _LL0;}case 3: _tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;{struct Cyc_List_List*ts=_tmp16C;
# 859
s=({struct Cyc_PP_Doc*_tmp1BA[3];({struct Cyc_PP_Doc*_tmp583=Cyc_PP_text(({const char*_tmp1BB="region_t<";_tag_fat(_tmp1BB,sizeof(char),10U);}));_tmp1BA[0]=_tmp583;}),({struct Cyc_PP_Doc*_tmp582=Cyc_Absynpp_rgn2doc((void*)(_check_null(ts))->hd);_tmp1BA[1]=_tmp582;}),({struct Cyc_PP_Doc*_tmp581=Cyc_PP_text(({const char*_tmp1BC=">";_tag_fat(_tmp1BC,sizeof(char),2U);}));_tmp1BA[2]=_tmp581;});Cyc_PP_cat(_tag_fat(_tmp1BA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 4: _tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;{struct Cyc_List_List*ts=_tmp16C;
# 861
s=({struct Cyc_PP_Doc*_tmp1BD[3];({struct Cyc_PP_Doc*_tmp586=Cyc_PP_text(({const char*_tmp1BE="tag_t<";_tag_fat(_tmp1BE,sizeof(char),7U);}));_tmp1BD[0]=_tmp586;}),({struct Cyc_PP_Doc*_tmp585=Cyc_Absynpp_typ2doc((void*)(_check_null(ts))->hd);_tmp1BD[1]=_tmp585;}),({struct Cyc_PP_Doc*_tmp584=Cyc_PP_text(({const char*_tmp1BF=">";_tag_fat(_tmp1BF,sizeof(char),2U);}));_tmp1BD[2]=_tmp584;});Cyc_PP_cat(_tag_fat(_tmp1BD,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 6:
 goto _LL2E;case 5: _LL2E:
 goto _LL30;case 7: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10: _tmp16C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2;{struct Cyc_List_List*ts=_tmp16C;
# 866
s=({struct Cyc_PP_Doc*_tmp1C0[3];({struct Cyc_PP_Doc*_tmp589=Cyc_PP_text(({const char*_tmp1C1="regions(";_tag_fat(_tmp1C1,sizeof(char),9U);}));_tmp1C0[0]=_tmp589;}),({struct Cyc_PP_Doc*_tmp588=Cyc_Absynpp_typ2doc((void*)(_check_null(ts))->hd);_tmp1C0[1]=_tmp588;}),({struct Cyc_PP_Doc*_tmp587=Cyc_PP_text(({const char*_tmp1C2=")";_tag_fat(_tmp1C2,sizeof(char),2U);}));_tmp1C0[2]=_tmp587;});Cyc_PP_cat(_tag_fat(_tmp1C0,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 8:
 goto _LL36;case 9: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11:
# 872
 s=Cyc_PP_text(({const char*_tmp1C3="@true";_tag_fat(_tmp1C3,sizeof(char),6U);}));goto _LL0;case 12:
 s=Cyc_PP_text(({const char*_tmp1C4="@false";_tag_fat(_tmp1C4,sizeof(char),7U);}));goto _LL0;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2 != 0){_tmp16C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16B)->f2)->hd;{void*t=_tmp16C;
# 875
s=({struct Cyc_PP_Doc*_tmp1C5[4];({struct Cyc_PP_Doc*_tmp58D=Cyc_PP_text(({const char*_tmp1C6="@thin @numelts";_tag_fat(_tmp1C6,sizeof(char),15U);}));_tmp1C5[0]=_tmp58D;}),({struct Cyc_PP_Doc*_tmp58C=Cyc_Absynpp_lb();_tmp1C5[1]=_tmp58C;}),({struct Cyc_PP_Doc*_tmp58B=Cyc_Absynpp_typ2doc(t);_tmp1C5[2]=_tmp58B;}),({struct Cyc_PP_Doc*_tmp58A=Cyc_Absynpp_rb();_tmp1C5[3]=_tmp58A;});Cyc_PP_cat(_tag_fat(_tmp1C5,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}}else{
# 877
s=Cyc_PP_text(({const char*_tmp1C7="@thin";_tag_fat(_tmp1C7,sizeof(char),6U);}));goto _LL0;}default:
 s=Cyc_PP_text(({const char*_tmp1C8="@fat";_tag_fat(_tmp1C8,sizeof(char),5U);}));goto _LL0;}case 1: _tmp16C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16B)->f1;_tmp16D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16B)->f2;_tmp170=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16B)->f3;_tmp16E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16B)->f4;{struct Cyc_Core_Opt*k=_tmp16C;void*topt=_tmp16D;int i=_tmp170;struct Cyc_Core_Opt*tvs=_tmp16E;
# 763
if(topt != 0)
# 765
return Cyc_Absynpp_ntyp2doc(topt);else{
# 767
struct _fat_ptr kindstring=k == 0?({const char*_tmp17D="K";_tag_fat(_tmp17D,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp4E9;_tmp4E9.tag=0,_tmp4E9.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_tmp4E9;});struct Cyc_Int_pa_PrintArg_struct _tmp17C=({struct Cyc_Int_pa_PrintArg_struct _tmp4E8;_tmp4E8.tag=1,_tmp4E8.f1=(unsigned long)i;_tmp4E8;});void*_tmp179[2];_tmp179[0]=& _tmp17B,_tmp179[1]=& _tmp17C;({struct _fat_ptr _tmp58E=({const char*_tmp17A="`E%s%d";_tag_fat(_tmp17A,sizeof(char),7U);});Cyc_aprintf(_tmp58E,_tag_fat(_tmp179,sizeof(void*),2));});}));}
# 770
goto _LL0;}case 2: _tmp16C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp16B)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp16C;
# 772
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp17E[3];_tmp17E[0]=s,({struct Cyc_PP_Doc*_tmp590=Cyc_PP_text(({const char*_tmp17F="::";_tag_fat(_tmp17F,sizeof(char),3U);}));_tmp17E[1]=_tmp590;}),({struct Cyc_PP_Doc*_tmp58F=Cyc_Absynpp_kindbound2doc(tv->kind);_tmp17E[2]=_tmp58F;});Cyc_PP_cat(_tag_fat(_tmp17E,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 6: _tmp16C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16B)->f1;{struct Cyc_List_List*ts=_tmp16C;
# 834
s=({struct Cyc_PP_Doc*_tmp1A8[2];({struct Cyc_PP_Doc*_tmp592=Cyc_Absynpp_dollar();_tmp1A8[0]=_tmp592;}),({struct Cyc_PP_Doc*_tmp591=Cyc_Absynpp_args2doc(ts);_tmp1A8[1]=_tmp591;});Cyc_PP_cat(_tag_fat(_tmp1A8,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 7: _tmp16F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16B)->f1;_tmp16C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16B)->f2;{enum Cyc_Absyn_AggrKind k=_tmp16F;struct Cyc_List_List*fs=_tmp16C;
# 840
s=({struct Cyc_PP_Doc*_tmp1AE[4];({struct Cyc_PP_Doc*_tmp596=Cyc_Absynpp_aggr_kind2doc(k);_tmp1AE[0]=_tmp596;}),({struct Cyc_PP_Doc*_tmp595=Cyc_Absynpp_lb();_tmp1AE[1]=_tmp595;}),({
struct Cyc_PP_Doc*_tmp594=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_tmp1AE[2]=_tmp594;}),({
struct Cyc_PP_Doc*_tmp593=Cyc_Absynpp_rb();_tmp1AE[3]=_tmp593;});Cyc_PP_cat(_tag_fat(_tmp1AE,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 9: _tmp16C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp16B)->f1;{struct Cyc_Absyn_Exp*e=_tmp16C;
# 847
s=({struct Cyc_PP_Doc*_tmp1B3[3];({struct Cyc_PP_Doc*_tmp599=Cyc_PP_text(({const char*_tmp1B4="valueof_t(";_tag_fat(_tmp1B4,sizeof(char),11U);}));_tmp1B3[0]=_tmp599;}),({struct Cyc_PP_Doc*_tmp598=Cyc_Absynpp_exp2doc(e);_tmp1B3[1]=_tmp598;}),({struct Cyc_PP_Doc*_tmp597=Cyc_PP_text(({const char*_tmp1B5=")";_tag_fat(_tmp1B5,sizeof(char),2U);}));_tmp1B3[2]=_tmp597;});Cyc_PP_cat(_tag_fat(_tmp1B3,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 11: _tmp16C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp16B)->f1;{struct Cyc_Absyn_Exp*e=_tmp16C;
s=({struct Cyc_PP_Doc*_tmp1B6[3];({struct Cyc_PP_Doc*_tmp59C=Cyc_PP_text(({const char*_tmp1B7="typeof(";_tag_fat(_tmp1B7,sizeof(char),8U);}));_tmp1B6[0]=_tmp59C;}),({struct Cyc_PP_Doc*_tmp59B=Cyc_Absynpp_exp2doc(e);_tmp1B6[1]=_tmp59B;}),({struct Cyc_PP_Doc*_tmp59A=Cyc_PP_text(({const char*_tmp1B8=")";_tag_fat(_tmp1B8,sizeof(char),2U);}));_tmp1B6[2]=_tmp59A;});Cyc_PP_cat(_tag_fat(_tmp1B6,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 8: _tmp16C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16B)->f1;_tmp16D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16B)->f2;_tmp16E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16B)->f3;{struct _tuple0*n=_tmp16C;struct Cyc_List_List*ts=_tmp16D;struct Cyc_Absyn_Typedefdecl*kopt=_tmp16E;
# 856
s=({struct Cyc_PP_Doc*_tmp1B9[2];({struct Cyc_PP_Doc*_tmp59E=Cyc_Absynpp_qvar2doc(n);_tmp1B9[0]=_tmp59E;}),({struct Cyc_PP_Doc*_tmp59D=Cyc_Absynpp_tps2doc(ts);_tmp1B9[1]=_tmp59D;});Cyc_PP_cat(_tag_fat(_tmp1B9,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16B)->f1)->r)){case 0: _tmp16C=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16B)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_tmp16C;
# 869
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1: _tmp16C=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16B)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_tmp16C;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _tmp16C=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16B)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_tmp16C;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 880
return s;}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 887
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_tmp1C9=cmp;void*_tmp1CB;void*_tmp1CA;_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;{void*r1=_tmp1CA;void*r2=_tmp1CB;
return({struct Cyc_PP_Doc*_tmp1CC[3];({struct Cyc_PP_Doc*_tmp5A1=Cyc_Absynpp_rgn2doc(r1);_tmp1CC[0]=_tmp5A1;}),({struct Cyc_PP_Doc*_tmp5A0=Cyc_PP_text(({const char*_tmp1CD=" > ";_tag_fat(_tmp1CD,sizeof(char),4U);}));_tmp1CC[1]=_tmp5A0;}),({struct Cyc_PP_Doc*_tmp59F=Cyc_Absynpp_rgn2doc(r2);_tmp1CC[2]=_tmp59F;});Cyc_PP_cat(_tag_fat(_tmp1CC,sizeof(struct Cyc_PP_Doc*),3));});}}
# 892
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _tmp5A4=({const char*_tmp1CE="";_tag_fat(_tmp1CE,sizeof(char),1U);});struct _fat_ptr _tmp5A3=({const char*_tmp1CF="";_tag_fat(_tmp1CF,sizeof(char),1U);});struct _fat_ptr _tmp5A2=({const char*_tmp1D0=",";_tag_fat(_tmp1D0,sizeof(char),2U);});Cyc_PP_group(_tmp5A4,_tmp5A3,_tmp5A2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple18*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 896
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*_tmp1D1=(*t).f1;struct _fat_ptr*vo=_tmp1D1;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_PP_Doc*_tmp5A5=Cyc_PP_text(*vo);_tmp1D2->v=_tmp5A5;});_tmp1D2;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 902
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 906
struct Cyc_List_List*_tmp1D3=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_funarg2doc,args);struct Cyc_List_List*arg_docs=_tmp1D3;
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*(*_tmp5A8)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp5A7=arg_docs;_tmp5A8(_tmp5A7,({struct Cyc_List_List*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));({struct Cyc_PP_Doc*_tmp5A6=Cyc_PP_text(({const char*_tmp1D4="...";_tag_fat(_tmp1D4,sizeof(char),4U);}));_tmp1D5->hd=_tmp5A6;}),_tmp1D5->tl=0;_tmp1D5;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1D6=({struct Cyc_PP_Doc*_tmp1D8[3];({struct Cyc_PP_Doc*_tmp5AC=Cyc_PP_text(({const char*_tmp1D9="...";_tag_fat(_tmp1D9,sizeof(char),4U);}));_tmp1D8[0]=_tmp5AC;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp5AB=Cyc_PP_text(({const char*_tmp1DA=" inject ";_tag_fat(_tmp1DA,sizeof(char),9U);}));_tmp1D8[1]=_tmp5AB;}):({struct Cyc_PP_Doc*_tmp5AA=Cyc_PP_text(({const char*_tmp1DB=" ";_tag_fat(_tmp1DB,sizeof(char),2U);}));_tmp1D8[1]=_tmp5AA;}),({
struct Cyc_PP_Doc*_tmp5A9=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->f1=cyc_varargs->name,_tmp1DC->f2=cyc_varargs->tq,_tmp1DC->f3=cyc_varargs->type;_tmp1DC;}));_tmp1D8[2]=_tmp5A9;});Cyc_PP_cat(_tag_fat(_tmp1D8,sizeof(struct Cyc_PP_Doc*),3));});
# 911
struct Cyc_PP_Doc*varargs_doc=_tmp1D6;
# 915
arg_docs=({struct Cyc_List_List*(*_tmp5AE)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp5AD=arg_docs;_tmp5AE(_tmp5AD,({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=varargs_doc,_tmp1D7->tl=0;_tmp1D7;}));});}}{
# 917
struct Cyc_PP_Doc*_tmp1DD=({struct _fat_ptr _tmp5B1=({const char*_tmp1EC="";_tag_fat(_tmp1EC,sizeof(char),1U);});struct _fat_ptr _tmp5B0=({const char*_tmp1ED="";_tag_fat(_tmp1ED,sizeof(char),1U);});struct _fat_ptr _tmp5AF=({const char*_tmp1EE=",";_tag_fat(_tmp1EE,sizeof(char),2U);});Cyc_PP_group(_tmp5B1,_tmp5B0,_tmp5AF,arg_docs);});struct Cyc_PP_Doc*arg_doc=_tmp1DD;
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_tmp1DE[3];_tmp1DE[0]=arg_doc,({struct Cyc_PP_Doc*_tmp5B3=Cyc_PP_text(({const char*_tmp1DF=";";_tag_fat(_tmp1DF,sizeof(char),2U);}));_tmp1DE[1]=_tmp5B3;}),({struct Cyc_PP_Doc*_tmp5B2=Cyc_Absynpp_eff2doc(effopt);_tmp1DE[2]=_tmp5B2;});Cyc_PP_cat(_tag_fat(_tmp1DE,sizeof(struct Cyc_PP_Doc*),3));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_tmp1E0[3];_tmp1E0[0]=arg_doc,({struct Cyc_PP_Doc*_tmp5B5=Cyc_PP_text(({const char*_tmp1E1=":";_tag_fat(_tmp1E1,sizeof(char),2U);}));_tmp1E0[1]=_tmp5B5;}),({struct Cyc_PP_Doc*_tmp5B4=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1E0[2]=_tmp5B4;});Cyc_PP_cat(_tag_fat(_tmp1E0,sizeof(struct Cyc_PP_Doc*),3));});{
struct Cyc_PP_Doc*_tmp1E2=({struct Cyc_PP_Doc*_tmp1E9[3];({struct Cyc_PP_Doc*_tmp5B7=Cyc_PP_text(({const char*_tmp1EA="(";_tag_fat(_tmp1EA,sizeof(char),2U);}));_tmp1E9[0]=_tmp5B7;}),_tmp1E9[1]=arg_doc,({struct Cyc_PP_Doc*_tmp5B6=Cyc_PP_text(({const char*_tmp1EB=")";_tag_fat(_tmp1EB,sizeof(char),2U);}));_tmp1E9[2]=_tmp5B6;});Cyc_PP_cat(_tag_fat(_tmp1E9,sizeof(struct Cyc_PP_Doc*),3));});struct Cyc_PP_Doc*res=_tmp1E2;
if(req != 0)
res=({struct Cyc_PP_Doc*_tmp1E3[4];_tmp1E3[0]=res,({struct Cyc_PP_Doc*_tmp5BA=Cyc_PP_text(({const char*_tmp1E4=" @requires(";_tag_fat(_tmp1E4,sizeof(char),12U);}));_tmp1E3[1]=_tmp5BA;}),({struct Cyc_PP_Doc*_tmp5B9=Cyc_Absynpp_exp2doc(req);_tmp1E3[2]=_tmp5B9;}),({struct Cyc_PP_Doc*_tmp5B8=Cyc_PP_text(({const char*_tmp1E5=")";_tag_fat(_tmp1E5,sizeof(char),2U);}));_tmp1E3[3]=_tmp5B8;});Cyc_PP_cat(_tag_fat(_tmp1E3,sizeof(struct Cyc_PP_Doc*),4));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_tmp1E6[4];_tmp1E6[0]=res,({struct Cyc_PP_Doc*_tmp5BD=Cyc_PP_text(({const char*_tmp1E7=" @ensures(";_tag_fat(_tmp1E7,sizeof(char),11U);}));_tmp1E6[1]=_tmp5BD;}),({struct Cyc_PP_Doc*_tmp5BC=Cyc_Absynpp_exp2doc(ens);_tmp1E6[2]=_tmp5BC;}),({struct Cyc_PP_Doc*_tmp5BB=Cyc_PP_text(({const char*_tmp1E8=")";_tag_fat(_tmp1E8,sizeof(char),2U);}));_tmp1E6[3]=_tmp5BB;});Cyc_PP_cat(_tag_fat(_tmp1E6,sizeof(struct Cyc_PP_Doc*),4));});
return res;}}}
# 930
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 932
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1EF=0;struct Cyc_List_List*prefix=_tmp1EF;
int match;
{union Cyc_Absyn_Nmspace _tmp1F0=(*q).f1;union Cyc_Absyn_Nmspace _stmttmp12=_tmp1F0;union Cyc_Absyn_Nmspace _tmp1F1=_stmttmp12;void*_tmp1F2;switch((_tmp1F1.C_n).tag){case 4:
 _tmp1F2=0;goto _LL4;case 1: _tmp1F2=(_tmp1F1.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp1F2;
# 938
match=0;
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 942
prefix=x;}
goto _LL0;}case 3: _tmp1F2=(_tmp1F1.C_n).val;{struct Cyc_List_List*x=_tmp1F2;
# 945
match=Cyc_Absynpp_use_curr_namespace &&({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 947
goto _LL0;}default: _tmp1F2=(_tmp1F1.Abs_n).val;{struct Cyc_List_List*x=_tmp1F2;
# 949
match=Cyc_Absynpp_use_curr_namespace &&({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
if(Cyc_Absynpp_qvar_to_Cids){
if((unsigned)x && !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 954
prefix=Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->hd=Cyc_Absynpp_cyc_stringptr,_tmp1F3->tl=x;_tmp1F3;}): x;}}
# 956
goto _LL0;}}_LL0:;}
# 958
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _fat_ptr)({struct Cyc_List_List*_tmp5C0=({struct Cyc_List_List*(*_tmp5BF)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp5BE=prefix;_tmp5BF(_tmp5BE,({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));
_tmp1F4->hd=(*q).f2,_tmp1F4->tl=0;_tmp1F4;}));});
# 959
Cyc_str_sepstr(_tmp5C0,({const char*_tmp1F5="_";_tag_fat(_tmp1F5,sizeof(char),2U);}));});else{
# 963
if(match)
return*(*q).f2;else{
# 966
return(struct _fat_ptr)({struct Cyc_List_List*_tmp5C3=({struct Cyc_List_List*(*_tmp5C2)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp5C1=prefix;_tmp5C2(_tmp5C1,({struct Cyc_List_List*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->hd=(*q).f2,_tmp1F6->tl=0;_tmp1F6;}));});Cyc_str_sepstr(_tmp5C3,({const char*_tmp1F7="::";_tag_fat(_tmp1F7,sizeof(char),3U);}));});}}}
# 970
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 974
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr _tmp1F8=Cyc_Absynpp_qvar2string(q);struct _fat_ptr qs=_tmp1F8;
if(Cyc_PP_tex_output)
# 978
return({struct _fat_ptr _tmp5C6=(struct _fat_ptr)({struct _fat_ptr _tmp5C5=(struct _fat_ptr)({struct _fat_ptr _tmp5C4=({const char*_tmp1F9="\\textbf{";_tag_fat(_tmp1F9,sizeof(char),9U);});Cyc_strconcat(_tmp5C4,(struct _fat_ptr)qs);});Cyc_strconcat(_tmp5C5,({const char*_tmp1FA="}";_tag_fat(_tmp1FA,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp5C6,(int)
Cyc_strlen((struct _fat_ptr)qs));});else{
return Cyc_PP_text(qs);}}
# 983
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 985
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 988
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1FB=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp13=_tmp1FB;union Cyc_Absyn_Nmspace _tmp1FC=_stmttmp13;void*_tmp1FD;switch((_tmp1FC.C_n).tag){case 4:
 goto _LL4;case 1: if((_tmp1FC.Rel_n).val == 0){_LL4:
 return*(*v).f2;}else{_LLA:
# 999
 return(struct _fat_ptr)({struct _fat_ptr _tmp5C7=({const char*_tmp1FE="/* bad namespace : */ ";_tag_fat(_tmp1FE,sizeof(char),23U);});Cyc_strconcat(_tmp5C7,(struct _fat_ptr)Cyc_Absynpp_qvar2string(v));});}case 3: _tmp1FD=(_tmp1FC.C_n).val;{struct Cyc_List_List*l=_tmp1FD;
# 992
_tmp1FD=l;goto _LL8;}default: _tmp1FD=(_tmp1FC.Abs_n).val;_LL8: {struct Cyc_List_List*l=_tmp1FD;
# 994
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 997
goto _LLA;}}};}else{
# 1002
return*(*v).f2;}}
# 1005
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1008
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _fat_ptr _tmp1FF=Cyc_Absynpp_typedef_name2string(v);struct _fat_ptr vs=_tmp1FF;
if(Cyc_PP_tex_output)
# 1012
return({struct _fat_ptr _tmp5CA=(struct _fat_ptr)({struct _fat_ptr _tmp5C9=(struct _fat_ptr)({struct _fat_ptr _tmp5C8=({const char*_tmp200="\\textbf{";_tag_fat(_tmp200,sizeof(char),9U);});Cyc_strconcat(_tmp5C8,(struct _fat_ptr)vs);});Cyc_strconcat(_tmp5C9,({const char*_tmp201="}";_tag_fat(_tmp201,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp5CA,(int)
Cyc_strlen((struct _fat_ptr)vs));});else{
return Cyc_PP_text(vs);}}
# 1017
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp5CB=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp5CB,t,0);});}
# 1021
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_tmp202=f;unsigned _tmp203;void*_tmp204;if(*((int*)_tmp202)== 0){_tmp204=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp202)->f1;{struct _fat_ptr*n=_tmp204;
return Cyc_PP_textptr(n);}}else{_tmp203=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp202)->f1;{unsigned n=_tmp203;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp207=({struct Cyc_Int_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=1,_tmp4EA.f1=(unsigned long)((int)n);_tmp4EA;});void*_tmp205[1];_tmp205[0]=& _tmp207;({struct _fat_ptr _tmp5CC=({const char*_tmp206="%d";_tag_fat(_tmp206,sizeof(char),3U);});Cyc_aprintf(_tmp5CC,_tag_fat(_tmp205,sizeof(void*),1));});}));}};}
# 1035 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp208=e->r;void*_stmttmp14=_tmp208;void*_tmp209=_stmttmp14;void*_tmp20A;enum Cyc_Absyn_Primop _tmp20B;switch(*((int*)_tmp209)){case 0:
 goto _LL4;case 1: _LL4:
 return 10000;case 3: _tmp20B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f1;{enum Cyc_Absyn_Primop p=_tmp20B;
# 1040
enum Cyc_Absyn_Primop _tmp20C=p;switch((int)_tmp20C){case Cyc_Absyn_Plus:
 return 100;case Cyc_Absyn_Times:
 return 110;case Cyc_Absyn_Minus:
 return 100;case Cyc_Absyn_Div:
 goto _LL61;case Cyc_Absyn_Mod: _LL61:
 return 110;case Cyc_Absyn_Eq:
 goto _LL65;case Cyc_Absyn_Neq: _LL65:
 return 70;case Cyc_Absyn_Gt:
 goto _LL69;case Cyc_Absyn_Lt: _LL69:
 goto _LL6B;case Cyc_Absyn_Gte: _LL6B:
 goto _LL6D;case Cyc_Absyn_Lte: _LL6D:
 return 80;case Cyc_Absyn_Not:
 goto _LL71;case Cyc_Absyn_Bitnot: _LL71:
 return 130;case Cyc_Absyn_Bitand:
 return 60;case Cyc_Absyn_Bitor:
 return 40;case Cyc_Absyn_Bitxor:
 return 50;case Cyc_Absyn_Bitlshift:
 return 90;case Cyc_Absyn_Bitlrshift:
 return 80;case Cyc_Absyn_Numelts:
 return 140;default:
 return 140;};}case 4:
# 1062
 return 20;case 5:
 return 130;case 6:
 return 30;case 7:
 return 35;case 8:
 return 30;case 9:
 return -10;case 10:
 return 140;case 2:
 return 140;case 11:
 return 130;case 12: _tmp20A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;{struct Cyc_Absyn_Exp*e2=_tmp20A;
return Cyc_Absynpp_exp_prec(e2);}case 13: _tmp20A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;{struct Cyc_Absyn_Exp*e2=_tmp20A;
return Cyc_Absynpp_exp_prec(e2);}case 14:
 return 120;case 16:
 return 15;case 15:
 goto _LL24;case 17: _LL24:
 goto _LL26;case 18: _LL26:
 goto _LL28;case 39: _LL28:
 goto _LL2A;case 40: _LL2A:
 goto _LL2C;case 38: _LL2C:
 goto _LL2E;case 19: _LL2E:
 goto _LL30;case 20: _LL30:
 goto _LL32;case 42: _LL32:
 goto _LL34;case 41: _LL34:
 return 130;case 21:
 goto _LL38;case 22: _LL38:
 goto _LL3A;case 23: _LL3A:
 return 140;case 24:
 return 150;case 25:
 goto _LL40;case 26: _LL40:
 goto _LL42;case 27: _LL42:
 goto _LL44;case 28: _LL44:
 goto _LL46;case 29: _LL46:
 goto _LL48;case 30: _LL48:
 goto _LL4A;case 31: _LL4A:
 goto _LL4C;case 32: _LL4C:
 goto _LL4E;case 33: _LL4E:
 goto _LL50;case 34: _LL50:
 goto _LL52;case 35: _LL52:
 goto _LL54;case 36: _LL54:
 return 140;default:
 return 10000;};}
# 1105
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}struct _tuple19{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1109
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_tmp20D=(struct _tuple19*)o->hd;struct _tuple19*_stmttmp15=_tmp20D;struct _tuple19*_tmp20E=_stmttmp15;void*_tmp210;struct _fat_ptr _tmp20F;_tmp20F=_tmp20E->f1;_tmp210=_tmp20E->f2;{struct _fat_ptr c=_tmp20F;struct Cyc_Absyn_Exp*e=_tmp210;
s=({struct Cyc_PP_Doc*_tmp211[6];_tmp211[0]=s,({struct Cyc_PP_Doc*_tmp5D1=Cyc_PP_text(({const char*_tmp212="\"";_tag_fat(_tmp212,sizeof(char),2U);}));_tmp211[1]=_tmp5D1;}),({struct Cyc_PP_Doc*_tmp5D0=Cyc_PP_text(c);_tmp211[2]=_tmp5D0;}),({struct Cyc_PP_Doc*_tmp5CF=Cyc_PP_text(({const char*_tmp213="\" (";_tag_fat(_tmp213,sizeof(char),4U);}));_tmp211[3]=_tmp5CF;}),({struct Cyc_PP_Doc*_tmp5CE=Cyc_Absynpp_exp2doc(e);_tmp211[4]=_tmp5CE;}),({struct Cyc_PP_Doc*_tmp5CD=Cyc_PP_text(({const char*_tmp214=")";_tag_fat(_tmp214,sizeof(char),2U);}));_tmp211[5]=_tmp5CD;});Cyc_PP_cat(_tag_fat(_tmp211,sizeof(struct Cyc_PP_Doc*),6));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp215[2];_tmp215[0]=s,({struct Cyc_PP_Doc*_tmp5D2=Cyc_PP_text(({const char*_tmp216=",";_tag_fat(_tmp216,sizeof(char),2U);}));_tmp215[1]=_tmp5D2;});Cyc_PP_cat(_tag_fat(_tmp215,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1119
return s;}
# 1122
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_tmp217[2];({struct Cyc_PP_Doc*_tmp5D4=Cyc_PP_text(({const char*_tmp218=": ";_tag_fat(_tmp218,sizeof(char),3U);}));_tmp217[0]=_tmp5D4;}),({struct Cyc_PP_Doc*_tmp5D3=Cyc_Absynpp_asm_iolist_doc_in(o);_tmp217[1]=_tmp5D3;});Cyc_PP_cat(_tag_fat(_tmp217,sizeof(struct Cyc_PP_Doc*),2));});
# 1127
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_tmp219[3];_tmp219[0]=s,({struct Cyc_PP_Doc*_tmp5D6=Cyc_PP_text(({const char*_tmp21A=": : ";_tag_fat(_tmp21A,sizeof(char),5U);}));_tmp219[1]=_tmp5D6;}),({struct Cyc_PP_Doc*_tmp5D5=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp219[2]=_tmp5D5;});Cyc_PP_cat(_tag_fat(_tmp219,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1131
s=({struct Cyc_PP_Doc*_tmp21B[3];_tmp21B[0]=s,({struct Cyc_PP_Doc*_tmp5D8=Cyc_PP_text(({const char*_tmp21C=" : ";_tag_fat(_tmp21C,sizeof(char),4U);}));_tmp21B[1]=_tmp5D8;}),({struct Cyc_PP_Doc*_tmp5D7=Cyc_Absynpp_asm_iolist_doc_in(i);_tmp21B[2]=_tmp5D7;});Cyc_PP_cat(_tag_fat(_tmp21B,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1133
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_tmp21D[2];_tmp21D[0]=s,ncol == 0?({struct Cyc_PP_Doc*_tmp5DB=Cyc_PP_text(({const char*_tmp21E=": : :";_tag_fat(_tmp21E,sizeof(char),6U);}));_tmp21D[1]=_tmp5DB;}):(ncol == 1?({struct Cyc_PP_Doc*_tmp5DA=Cyc_PP_text(({const char*_tmp21F=" : : ";_tag_fat(_tmp21F,sizeof(char),6U);}));_tmp21D[1]=_tmp5DA;}):({struct Cyc_PP_Doc*_tmp5D9=Cyc_PP_text(({const char*_tmp220=" : ";_tag_fat(_tmp220,sizeof(char),4U);}));_tmp21D[1]=_tmp5D9;}));Cyc_PP_cat(_tag_fat(_tmp21D,sizeof(struct Cyc_PP_Doc*),2));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_tmp221[4];_tmp221[0]=s,({struct Cyc_PP_Doc*_tmp5DE=Cyc_PP_text(({const char*_tmp222="\"";_tag_fat(_tmp222,sizeof(char),2U);}));_tmp221[1]=_tmp5DE;}),({struct Cyc_PP_Doc*_tmp5DD=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_tmp221[2]=_tmp5DD;}),({struct Cyc_PP_Doc*_tmp5DC=Cyc_PP_text(({const char*_tmp223="\"";_tag_fat(_tmp223,sizeof(char),2U);}));_tmp221[3]=_tmp5DC;});Cyc_PP_cat(_tag_fat(_tmp221,sizeof(struct Cyc_PP_Doc*),4));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_tmp224[2];_tmp224[0]=s,({struct Cyc_PP_Doc*_tmp5DF=Cyc_PP_text(({const char*_tmp225=", ";_tag_fat(_tmp225,sizeof(char),3U);}));_tmp224[1]=_tmp5DF;});Cyc_PP_cat(_tag_fat(_tmp224,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1143
return s;}
# 1146
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp226=e->r;void*_stmttmp16=_tmp226;void*_tmp227=_stmttmp16;int _tmp22C;int _tmp22A;enum Cyc_Absyn_Incrementor _tmp22E;void*_tmp22B;void*_tmp229;enum Cyc_Absyn_Primop _tmp22F;struct _fat_ptr _tmp22D;void*_tmp228;union Cyc_Absyn_Cnst _tmp230;switch(*((int*)_tmp227)){case 0: _tmp230=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{union Cyc_Absyn_Cnst c=_tmp230;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1: _tmp228=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{void*b=_tmp228;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _tmp22D=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct _fat_ptr p=_tmp22D;
# 1153
s=({struct Cyc_PP_Doc*_tmp231[4];({struct Cyc_PP_Doc*_tmp5E3=Cyc_PP_text(({const char*_tmp232="__cyclone_pragma__";_tag_fat(_tmp232,sizeof(char),19U);}));_tmp231[0]=_tmp5E3;}),({struct Cyc_PP_Doc*_tmp5E2=Cyc_PP_text(({const char*_tmp233="(";_tag_fat(_tmp233,sizeof(char),2U);}));_tmp231[1]=_tmp5E2;}),({struct Cyc_PP_Doc*_tmp5E1=Cyc_PP_text(p);_tmp231[2]=_tmp5E1;}),({struct Cyc_PP_Doc*_tmp5E0=Cyc_PP_text(({const char*_tmp234=")";_tag_fat(_tmp234,sizeof(char),2U);}));_tmp231[3]=_tmp5E0;});Cyc_PP_cat(_tag_fat(_tmp231,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 3: _tmp22F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp228=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{enum Cyc_Absyn_Primop p=_tmp22F;struct Cyc_List_List*es=_tmp228;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4: _tmp228=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_tmp22B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp227)->f3;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Core_Opt*popt=_tmp229;struct Cyc_Absyn_Exp*e2=_tmp22B;
# 1156
s=({struct Cyc_PP_Doc*_tmp235[5];({struct Cyc_PP_Doc*_tmp5E9=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp235[0]=_tmp5E9;}),({
struct Cyc_PP_Doc*_tmp5E8=Cyc_PP_text(({const char*_tmp236=" ";_tag_fat(_tmp236,sizeof(char),2U);}));_tmp235[1]=_tmp5E8;}),
popt == 0?({struct Cyc_PP_Doc*_tmp5E7=Cyc_PP_nil_doc();_tmp235[2]=_tmp5E7;}):({struct Cyc_PP_Doc*_tmp5E6=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_tmp235[2]=_tmp5E6;}),({
struct Cyc_PP_Doc*_tmp5E5=Cyc_PP_text(({const char*_tmp237="= ";_tag_fat(_tmp237,sizeof(char),3U);}));_tmp235[3]=_tmp5E5;}),({
struct Cyc_PP_Doc*_tmp5E4=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp235[4]=_tmp5E4;});Cyc_PP_cat(_tag_fat(_tmp235,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 5: _tmp228=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp22E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e2=_tmp228;enum Cyc_Absyn_Incrementor i=_tmp22E;
# 1163
struct Cyc_PP_Doc*_tmp238=Cyc_Absynpp_exp2doc_prec(myprec,e2);struct Cyc_PP_Doc*es=_tmp238;
{enum Cyc_Absyn_Incrementor _tmp239=i;switch((int)_tmp239){case Cyc_Absyn_PreInc:
 s=({struct Cyc_PP_Doc*_tmp23A[2];({struct Cyc_PP_Doc*_tmp5EA=Cyc_PP_text(({const char*_tmp23B="++";_tag_fat(_tmp23B,sizeof(char),3U);}));_tmp23A[0]=_tmp5EA;}),_tmp23A[1]=es;Cyc_PP_cat(_tag_fat(_tmp23A,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PreDec:
 s=({struct Cyc_PP_Doc*_tmp23C[2];({struct Cyc_PP_Doc*_tmp5EB=Cyc_PP_text(({const char*_tmp23D="--";_tag_fat(_tmp23D,sizeof(char),3U);}));_tmp23C[0]=_tmp5EB;}),_tmp23C[1]=es;Cyc_PP_cat(_tag_fat(_tmp23C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PostInc:
 s=({struct Cyc_PP_Doc*_tmp23E[2];_tmp23E[0]=es,({struct Cyc_PP_Doc*_tmp5EC=Cyc_PP_text(({const char*_tmp23F="++";_tag_fat(_tmp23F,sizeof(char),3U);}));_tmp23E[1]=_tmp5EC;});Cyc_PP_cat(_tag_fat(_tmp23E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PostDec:
 goto _LL61;default: _LL61:
 s=({struct Cyc_PP_Doc*_tmp240[2];_tmp240[0]=es,({struct Cyc_PP_Doc*_tmp5ED=Cyc_PP_text(({const char*_tmp241="--";_tag_fat(_tmp241,sizeof(char),3U);}));_tmp240[1]=_tmp5ED;});Cyc_PP_cat(_tag_fat(_tmp240,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;}_LL57:;}
# 1171
goto _LL0;}case 6: _tmp228=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_tmp22B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp227)->f3;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;struct Cyc_Absyn_Exp*e3=_tmp22B;
# 1173
s=({struct Cyc_PP_Doc*_tmp242[5];({struct Cyc_PP_Doc*_tmp5F2=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp242[0]=_tmp5F2;}),({struct Cyc_PP_Doc*_tmp5F1=Cyc_PP_text(({const char*_tmp243=" ? ";_tag_fat(_tmp243,sizeof(char),4U);}));_tmp242[1]=_tmp5F1;}),({struct Cyc_PP_Doc*_tmp5F0=Cyc_Absynpp_exp2doc_prec(0,e2);_tmp242[2]=_tmp5F0;}),({
struct Cyc_PP_Doc*_tmp5EF=Cyc_PP_text(({const char*_tmp244=" : ";_tag_fat(_tmp244,sizeof(char),4U);}));_tmp242[3]=_tmp5EF;}),({struct Cyc_PP_Doc*_tmp5EE=Cyc_Absynpp_exp2doc_prec(myprec,e3);_tmp242[4]=_tmp5EE;});Cyc_PP_cat(_tag_fat(_tmp242,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 7: _tmp228=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;
# 1177
s=({struct Cyc_PP_Doc*_tmp245[3];({struct Cyc_PP_Doc*_tmp5F5=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp245[0]=_tmp5F5;}),({struct Cyc_PP_Doc*_tmp5F4=Cyc_PP_text(({const char*_tmp246=" && ";_tag_fat(_tmp246,sizeof(char),5U);}));_tmp245[1]=_tmp5F4;}),({struct Cyc_PP_Doc*_tmp5F3=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp245[2]=_tmp5F3;});Cyc_PP_cat(_tag_fat(_tmp245,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 8: _tmp228=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;
# 1180
s=({struct Cyc_PP_Doc*_tmp247[3];({struct Cyc_PP_Doc*_tmp5F8=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp247[0]=_tmp5F8;}),({struct Cyc_PP_Doc*_tmp5F7=Cyc_PP_text(({const char*_tmp248=" || ";_tag_fat(_tmp248,sizeof(char),5U);}));_tmp247[1]=_tmp5F7;}),({struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp247[2]=_tmp5F6;});Cyc_PP_cat(_tag_fat(_tmp247,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 9: _tmp228=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;
# 1185
s=({struct Cyc_PP_Doc*_tmp249[3];({struct Cyc_PP_Doc*_tmp5FB=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_tmp249[0]=_tmp5FB;}),({struct Cyc_PP_Doc*_tmp5FA=Cyc_PP_text(({const char*_tmp24A=", ";_tag_fat(_tmp24A,sizeof(char),3U);}));_tmp249[1]=_tmp5FA;}),({struct Cyc_PP_Doc*_tmp5F9=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_tmp249[2]=_tmp5F9;});Cyc_PP_cat(_tag_fat(_tmp249,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 10: _tmp228=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_List_List*es=_tmp229;
# 1188
s=({struct Cyc_PP_Doc*_tmp24B[4];({struct Cyc_PP_Doc*_tmp5FF=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp24B[0]=_tmp5FF;}),({
struct Cyc_PP_Doc*_tmp5FE=Cyc_PP_text(({const char*_tmp24C="(";_tag_fat(_tmp24C,sizeof(char),2U);}));_tmp24B[1]=_tmp5FE;}),({
struct Cyc_PP_Doc*_tmp5FD=Cyc_Absynpp_exps2doc_prec(20,es);_tmp24B[2]=_tmp5FD;}),({
struct Cyc_PP_Doc*_tmp5FC=Cyc_PP_text(({const char*_tmp24D=")";_tag_fat(_tmp24D,sizeof(char),2U);}));_tmp24B[3]=_tmp5FC;});Cyc_PP_cat(_tag_fat(_tmp24B,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 11: _tmp228=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e1=_tmp228;
s=({struct Cyc_PP_Doc*_tmp24E[2];({struct Cyc_PP_Doc*_tmp601=Cyc_PP_text(({const char*_tmp24F="throw ";_tag_fat(_tmp24F,sizeof(char),7U);}));_tmp24E[0]=_tmp601;}),({struct Cyc_PP_Doc*_tmp600=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp24E[1]=_tmp600;});Cyc_PP_cat(_tag_fat(_tmp24E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 12: _tmp228=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e1=_tmp228;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13: _tmp228=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e1=_tmp228;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14: _tmp228=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{void*t=_tmp228;struct Cyc_Absyn_Exp*e1=_tmp229;
# 1197
s=({struct Cyc_PP_Doc*_tmp250[4];({struct Cyc_PP_Doc*_tmp605=Cyc_PP_text(({const char*_tmp251="(";_tag_fat(_tmp251,sizeof(char),2U);}));_tmp250[0]=_tmp605;}),({struct Cyc_PP_Doc*_tmp604=Cyc_Absynpp_typ2doc(t);_tmp250[1]=_tmp604;}),({struct Cyc_PP_Doc*_tmp603=Cyc_PP_text(({const char*_tmp252=")";_tag_fat(_tmp252,sizeof(char),2U);}));_tmp250[2]=_tmp603;}),({struct Cyc_PP_Doc*_tmp602=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp250[3]=_tmp602;});Cyc_PP_cat(_tag_fat(_tmp250,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 15: _tmp228=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e1=_tmp228;
s=({struct Cyc_PP_Doc*_tmp253[2];({struct Cyc_PP_Doc*_tmp607=Cyc_PP_text(({const char*_tmp254="&";_tag_fat(_tmp254,sizeof(char),2U);}));_tmp253[0]=_tmp607;}),({struct Cyc_PP_Doc*_tmp606=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp253[1]=_tmp606;});Cyc_PP_cat(_tag_fat(_tmp253,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 16: _tmp228=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*ropt=_tmp228;struct Cyc_Absyn_Exp*e1=_tmp229;
# 1200
if(ropt == 0)
s=({struct Cyc_PP_Doc*_tmp255[2];({struct Cyc_PP_Doc*_tmp609=Cyc_PP_text(({const char*_tmp256="new ";_tag_fat(_tmp256,sizeof(char),5U);}));_tmp255[0]=_tmp609;}),({struct Cyc_PP_Doc*_tmp608=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp255[1]=_tmp608;});Cyc_PP_cat(_tag_fat(_tmp255,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1203
s=({struct Cyc_PP_Doc*_tmp257[4];({struct Cyc_PP_Doc*_tmp60D=Cyc_PP_text(({const char*_tmp258="rnew(";_tag_fat(_tmp258,sizeof(char),6U);}));_tmp257[0]=_tmp60D;}),({struct Cyc_PP_Doc*_tmp60C=Cyc_Absynpp_exp2doc(ropt);_tmp257[1]=_tmp60C;}),({struct Cyc_PP_Doc*_tmp60B=Cyc_PP_text(({const char*_tmp259=") ";_tag_fat(_tmp259,sizeof(char),3U);}));_tmp257[2]=_tmp60B;}),({struct Cyc_PP_Doc*_tmp60A=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp257[3]=_tmp60A;});Cyc_PP_cat(_tag_fat(_tmp257,sizeof(struct Cyc_PP_Doc*),4));});}
goto _LL0;}case 17: _tmp228=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{void*t=_tmp228;
s=({struct Cyc_PP_Doc*_tmp25A[3];({struct Cyc_PP_Doc*_tmp610=Cyc_PP_text(({const char*_tmp25B="sizeof(";_tag_fat(_tmp25B,sizeof(char),8U);}));_tmp25A[0]=_tmp610;}),({struct Cyc_PP_Doc*_tmp60F=Cyc_Absynpp_typ2doc(t);_tmp25A[1]=_tmp60F;}),({struct Cyc_PP_Doc*_tmp60E=Cyc_PP_text(({const char*_tmp25C=")";_tag_fat(_tmp25C,sizeof(char),2U);}));_tmp25A[2]=_tmp60E;});Cyc_PP_cat(_tag_fat(_tmp25A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 18: _tmp228=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e1=_tmp228;
s=({struct Cyc_PP_Doc*_tmp25D[3];({struct Cyc_PP_Doc*_tmp613=Cyc_PP_text(({const char*_tmp25E="sizeof(";_tag_fat(_tmp25E,sizeof(char),8U);}));_tmp25D[0]=_tmp613;}),({struct Cyc_PP_Doc*_tmp612=Cyc_Absynpp_exp2doc(e1);_tmp25D[1]=_tmp612;}),({struct Cyc_PP_Doc*_tmp611=Cyc_PP_text(({const char*_tmp25F=")";_tag_fat(_tmp25F,sizeof(char),2U);}));_tmp25D[2]=_tmp611;});Cyc_PP_cat(_tag_fat(_tmp25D,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 42: _tmp228=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e=_tmp228;
# 1208
s=({struct Cyc_PP_Doc*_tmp260[3];({struct Cyc_PP_Doc*_tmp616=Cyc_PP_text(({const char*_tmp261="@assert(";_tag_fat(_tmp261,sizeof(char),9U);}));_tmp260[0]=_tmp616;}),({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_exp2doc(e);_tmp260[1]=_tmp615;}),({struct Cyc_PP_Doc*_tmp614=Cyc_PP_text(({const char*_tmp262=")";_tag_fat(_tmp262,sizeof(char),2U);}));_tmp260[2]=_tmp614;});Cyc_PP_cat(_tag_fat(_tmp260,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 41: _tmp228=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e=_tmp228;
# 1210
s=({struct Cyc_PP_Doc*_tmp263[3];({struct Cyc_PP_Doc*_tmp619=Cyc_PP_text(({const char*_tmp264="__extension__(";_tag_fat(_tmp264,sizeof(char),15U);}));_tmp263[0]=_tmp619;}),({struct Cyc_PP_Doc*_tmp618=Cyc_Absynpp_exp2doc(e);_tmp263[1]=_tmp618;}),({struct Cyc_PP_Doc*_tmp617=Cyc_PP_text(({const char*_tmp265=")";_tag_fat(_tmp265,sizeof(char),2U);}));_tmp263[2]=_tmp617;});Cyc_PP_cat(_tag_fat(_tmp263,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 39: _tmp228=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{void*t=_tmp228;
s=({struct Cyc_PP_Doc*_tmp266[3];({struct Cyc_PP_Doc*_tmp61C=Cyc_PP_text(({const char*_tmp267="valueof(";_tag_fat(_tmp267,sizeof(char),9U);}));_tmp266[0]=_tmp61C;}),({struct Cyc_PP_Doc*_tmp61B=Cyc_Absynpp_typ2doc(t);_tmp266[1]=_tmp61B;}),({struct Cyc_PP_Doc*_tmp61A=Cyc_PP_text(({const char*_tmp268=")";_tag_fat(_tmp268,sizeof(char),2U);}));_tmp266[2]=_tmp61A;});Cyc_PP_cat(_tag_fat(_tmp266,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 40: _tmp22A=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp22D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_tmp228=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp227)->f3;_tmp229=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp227)->f4;_tmp22B=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp227)->f5;{int vol=_tmp22A;struct _fat_ptr t=_tmp22D;struct Cyc_List_List*o=_tmp228;struct Cyc_List_List*i=_tmp229;struct Cyc_List_List*cl=_tmp22B;
# 1213
if(vol)
s=({struct Cyc_PP_Doc*_tmp269[7];({struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp26A="__asm__";_tag_fat(_tmp26A,sizeof(char),8U);}));_tmp269[0]=_tmp623;}),({struct Cyc_PP_Doc*_tmp622=Cyc_PP_text(({const char*_tmp26B=" volatile (";_tag_fat(_tmp26B,sizeof(char),12U);}));_tmp269[1]=_tmp622;}),({struct Cyc_PP_Doc*_tmp621=Cyc_PP_text(({const char*_tmp26C="\"";_tag_fat(_tmp26C,sizeof(char),2U);}));_tmp269[2]=_tmp621;}),({struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp269[3]=_tmp620;}),({struct Cyc_PP_Doc*_tmp61F=Cyc_PP_text(({const char*_tmp26D="\"";_tag_fat(_tmp26D,sizeof(char),2U);}));_tmp269[4]=_tmp61F;}),({struct Cyc_PP_Doc*_tmp61E=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp269[5]=_tmp61E;}),({struct Cyc_PP_Doc*_tmp61D=Cyc_PP_text(({const char*_tmp26E=")";_tag_fat(_tmp26E,sizeof(char),2U);}));_tmp269[6]=_tmp61D;});Cyc_PP_cat(_tag_fat(_tmp269,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1216
s=({struct Cyc_PP_Doc*_tmp26F[6];({struct Cyc_PP_Doc*_tmp629=Cyc_PP_text(({const char*_tmp270="__asm__(";_tag_fat(_tmp270,sizeof(char),9U);}));_tmp26F[0]=_tmp629;}),({struct Cyc_PP_Doc*_tmp628=Cyc_PP_text(({const char*_tmp271="\"";_tag_fat(_tmp271,sizeof(char),2U);}));_tmp26F[1]=_tmp628;}),({struct Cyc_PP_Doc*_tmp627=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_tmp26F[2]=_tmp627;}),({struct Cyc_PP_Doc*_tmp626=Cyc_PP_text(({const char*_tmp272="\"";_tag_fat(_tmp272,sizeof(char),2U);}));_tmp26F[3]=_tmp626;}),({struct Cyc_PP_Doc*_tmp625=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_tmp26F[4]=_tmp625;}),({struct Cyc_PP_Doc*_tmp624=Cyc_PP_text(({const char*_tmp273=")";_tag_fat(_tmp273,sizeof(char),2U);}));_tmp26F[5]=_tmp624;});Cyc_PP_cat(_tag_fat(_tmp26F,sizeof(struct Cyc_PP_Doc*),6));});}
goto _LL0;}case 38: _tmp228=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e=_tmp228;struct _fat_ptr*f=_tmp229;
# 1219
s=({struct Cyc_PP_Doc*_tmp274[5];({struct Cyc_PP_Doc*_tmp62E=Cyc_PP_text(({const char*_tmp275="tagcheck(";_tag_fat(_tmp275,sizeof(char),10U);}));_tmp274[0]=_tmp62E;}),({struct Cyc_PP_Doc*_tmp62D=Cyc_Absynpp_exp2doc(e);_tmp274[1]=_tmp62D;}),({struct Cyc_PP_Doc*_tmp62C=Cyc_PP_text(({const char*_tmp276=".";_tag_fat(_tmp276,sizeof(char),2U);}));_tmp274[2]=_tmp62C;}),({struct Cyc_PP_Doc*_tmp62B=Cyc_PP_textptr(f);_tmp274[3]=_tmp62B;}),({struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp277=")";_tag_fat(_tmp277,sizeof(char),2U);}));_tmp274[4]=_tmp62A;});Cyc_PP_cat(_tag_fat(_tmp274,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 19: _tmp228=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{void*t=_tmp228;struct Cyc_List_List*l=_tmp229;
# 1222
s=({struct Cyc_PP_Doc*_tmp278[5];({struct Cyc_PP_Doc*_tmp634=Cyc_PP_text(({const char*_tmp279="offsetof(";_tag_fat(_tmp279,sizeof(char),10U);}));_tmp278[0]=_tmp634;}),({struct Cyc_PP_Doc*_tmp633=Cyc_Absynpp_typ2doc(t);_tmp278[1]=_tmp633;}),({struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp27A=",";_tag_fat(_tmp27A,sizeof(char),2U);}));_tmp278[2]=_tmp632;}),({
struct Cyc_PP_Doc*_tmp631=({struct _fat_ptr _tmp630=({const char*_tmp27B=".";_tag_fat(_tmp27B,sizeof(char),2U);});Cyc_PP_seq(_tmp630,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_offsetof_field_to_doc,l));});_tmp278[3]=_tmp631;}),({struct Cyc_PP_Doc*_tmp62F=Cyc_PP_text(({const char*_tmp27C=")";_tag_fat(_tmp27C,sizeof(char),2U);}));_tmp278[4]=_tmp62F;});Cyc_PP_cat(_tag_fat(_tmp278,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 20: _tmp228=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Exp*e1=_tmp228;
s=({struct Cyc_PP_Doc*_tmp27D[2];({struct Cyc_PP_Doc*_tmp636=Cyc_PP_text(({const char*_tmp27E="*";_tag_fat(_tmp27E,sizeof(char),2U);}));_tmp27D[0]=_tmp636;}),({struct Cyc_PP_Doc*_tmp635=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp27D[1]=_tmp635;});Cyc_PP_cat(_tag_fat(_tmp27D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 21: _tmp228=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct _fat_ptr*n=_tmp229;
# 1227
s=({struct Cyc_PP_Doc*_tmp27F[3];({struct Cyc_PP_Doc*_tmp639=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp27F[0]=_tmp639;}),({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp280=".";_tag_fat(_tmp280,sizeof(char),2U);}));_tmp27F[1]=_tmp638;}),({struct Cyc_PP_Doc*_tmp637=Cyc_PP_textptr(n);_tmp27F[2]=_tmp637;});Cyc_PP_cat(_tag_fat(_tmp27F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 22: _tmp228=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct _fat_ptr*n=_tmp229;
# 1229
s=({struct Cyc_PP_Doc*_tmp281[3];({struct Cyc_PP_Doc*_tmp63C=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp281[0]=_tmp63C;}),({struct Cyc_PP_Doc*_tmp63B=Cyc_PP_text(({const char*_tmp282="->";_tag_fat(_tmp282,sizeof(char),3U);}));_tmp281[1]=_tmp63B;}),({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_textptr(n);_tmp281[2]=_tmp63A;});Cyc_PP_cat(_tag_fat(_tmp281,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 23: _tmp228=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;
# 1231
s=({struct Cyc_PP_Doc*_tmp283[4];({struct Cyc_PP_Doc*_tmp640=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp283[0]=_tmp640;}),({struct Cyc_PP_Doc*_tmp63F=Cyc_PP_text(({const char*_tmp284="[";_tag_fat(_tmp284,sizeof(char),2U);}));_tmp283[1]=_tmp63F;}),({struct Cyc_PP_Doc*_tmp63E=Cyc_Absynpp_exp2doc(e2);_tmp283[2]=_tmp63E;}),({struct Cyc_PP_Doc*_tmp63D=Cyc_PP_text(({const char*_tmp285="]";_tag_fat(_tmp285,sizeof(char),2U);}));_tmp283[3]=_tmp63D;});Cyc_PP_cat(_tag_fat(_tmp283,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 24: _tmp228=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_List_List*es=_tmp228;
# 1233
s=({struct Cyc_PP_Doc*_tmp286[4];({struct Cyc_PP_Doc*_tmp644=Cyc_Absynpp_dollar();_tmp286[0]=_tmp644;}),({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp287="(";_tag_fat(_tmp287,sizeof(char),2U);}));_tmp286[1]=_tmp643;}),({struct Cyc_PP_Doc*_tmp642=Cyc_Absynpp_exps2doc_prec(20,es);_tmp286[2]=_tmp642;}),({struct Cyc_PP_Doc*_tmp641=Cyc_PP_text(({const char*_tmp288=")";_tag_fat(_tmp288,sizeof(char),2U);}));_tmp286[3]=_tmp641;});Cyc_PP_cat(_tag_fat(_tmp286,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 25: _tmp228=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct _tuple8*vat=_tmp228;struct Cyc_List_List*des=_tmp229;
# 1235
s=({struct Cyc_PP_Doc*_tmp289[4];({struct Cyc_PP_Doc*_tmp649=Cyc_PP_text(({const char*_tmp28A="(";_tag_fat(_tmp28A,sizeof(char),2U);}));_tmp289[0]=_tmp649;}),({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_typ2doc((*vat).f3);_tmp289[1]=_tmp648;}),({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp28B=")";_tag_fat(_tmp28B,sizeof(char),2U);}));_tmp289[2]=_tmp647;}),({
struct Cyc_PP_Doc*_tmp646=({struct _fat_ptr _tmp645=({const char*_tmp28C=",";_tag_fat(_tmp28C,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp645,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});_tmp289[3]=_tmp646;});Cyc_PP_cat(_tag_fat(_tmp289,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 26: _tmp228=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_List_List*des=_tmp228;
s=({struct _fat_ptr _tmp64A=({const char*_tmp28D=",";_tag_fat(_tmp28D,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp64A,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27: _tmp228=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_tmp22B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp227)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp228;struct Cyc_Absyn_Exp*e1=_tmp229;struct Cyc_Absyn_Exp*e2=_tmp22B;
# 1240
s=({struct Cyc_PP_Doc*_tmp28E[8];({struct Cyc_PP_Doc*_tmp652=Cyc_Absynpp_lb();_tmp28E[0]=_tmp652;}),({struct Cyc_PP_Doc*_tmp651=Cyc_PP_text(({const char*_tmp28F="for ";_tag_fat(_tmp28F,sizeof(char),5U);}));_tmp28E[1]=_tmp651;}),({
struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(*(*vd->name).f2);_tmp28E[2]=_tmp650;}),({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp290=" < ";_tag_fat(_tmp290,sizeof(char),4U);}));_tmp28E[3]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_Absynpp_exp2doc(e1);_tmp28E[4]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp291=" : ";_tag_fat(_tmp291,sizeof(char),4U);}));_tmp28E[5]=_tmp64D;}),({
struct Cyc_PP_Doc*_tmp64C=Cyc_Absynpp_exp2doc(e2);_tmp28E[6]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_Absynpp_rb();_tmp28E[7]=_tmp64B;});Cyc_PP_cat(_tag_fat(_tmp28E,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 28: _tmp228=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e=_tmp228;void*t=_tmp229;
# 1245
s=({struct Cyc_PP_Doc*_tmp292[7];({struct Cyc_PP_Doc*_tmp659=Cyc_Absynpp_lb();_tmp292[0]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_PP_text(({const char*_tmp293="for x ";_tag_fat(_tmp293,sizeof(char),7U);}));_tmp292[1]=_tmp658;}),({
struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp294=" < ";_tag_fat(_tmp294,sizeof(char),4U);}));_tmp292[2]=_tmp657;}),({
struct Cyc_PP_Doc*_tmp656=Cyc_Absynpp_exp2doc(e);_tmp292[3]=_tmp656;}),({
struct Cyc_PP_Doc*_tmp655=Cyc_PP_text(({const char*_tmp295=" : ";_tag_fat(_tmp295,sizeof(char),4U);}));_tmp292[4]=_tmp655;}),({
struct Cyc_PP_Doc*_tmp654=Cyc_Absynpp_typ2doc(t);_tmp292[5]=_tmp654;}),({
struct Cyc_PP_Doc*_tmp653=Cyc_Absynpp_rb();_tmp292[6]=_tmp653;});Cyc_PP_cat(_tag_fat(_tmp292,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 29: _tmp228=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_tmp22B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp227)->f3;{struct _tuple0*n=_tmp228;struct Cyc_List_List*ts=_tmp229;struct Cyc_List_List*des=_tmp22B;
# 1253
struct Cyc_List_List*_tmp296=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des);struct Cyc_List_List*des_doc=_tmp296;
s=({struct Cyc_PP_Doc*_tmp297[2];({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_qvar2doc(n);_tmp297[0]=_tmp65D;}),({
struct Cyc_PP_Doc*_tmp65C=({struct _fat_ptr _tmp65B=({const char*_tmp298=",";_tag_fat(_tmp298,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp65B,
ts != 0?({struct Cyc_List_List*_tmp299=_cycalloc(sizeof(*_tmp299));({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_tps2doc(ts);_tmp299->hd=_tmp65A;}),_tmp299->tl=des_doc;_tmp299;}): des_doc);});
# 1255
_tmp297[1]=_tmp65C;});Cyc_PP_cat(_tag_fat(_tmp297,sizeof(struct Cyc_PP_Doc*),2));});
# 1257
goto _LL0;}case 30: _tmp228=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_List_List*des=_tmp228;
s=({struct _fat_ptr _tmp65E=({const char*_tmp29A=",";_tag_fat(_tmp29A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp65E,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31: _tmp228=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp227)->f3;{struct Cyc_List_List*es=_tmp228;struct Cyc_Absyn_Datatypefield*tuf=_tmp229;
# 1260
if(es == 0)
# 1262
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1264
s=({struct Cyc_PP_Doc*_tmp29B[2];({struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_qvar2doc(tuf->name);_tmp29B[0]=_tmp663;}),({
struct Cyc_PP_Doc*_tmp662=({struct _fat_ptr _tmp661=({const char*_tmp29C="(";_tag_fat(_tmp29C,sizeof(char),2U);});struct _fat_ptr _tmp660=({const char*_tmp29D=")";_tag_fat(_tmp29D,sizeof(char),2U);});struct _fat_ptr _tmp65F=({const char*_tmp29E=",";_tag_fat(_tmp29E,sizeof(char),2U);});Cyc_PP_egroup(_tmp661,_tmp660,_tmp65F,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_exp2doc,es));});_tmp29B[1]=_tmp662;});Cyc_PP_cat(_tag_fat(_tmp29B,sizeof(struct Cyc_PP_Doc*),2));});}
goto _LL0;}case 32: _tmp228=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp228;
_tmp228=ef;goto _LL4E;}case 33: _tmp228=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp227)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_tmp228;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34: _tmp22A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp227)->f1).is_calloc;_tmp228=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp227)->f1).rgn;_tmp229=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp227)->f1).elt_type;_tmp22B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp227)->f1).num_elts;_tmp22C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp227)->f1).inline_call;{int is_calloc=_tmp22A;struct Cyc_Absyn_Exp*rgnopt=_tmp228;void**topt=_tmp229;struct Cyc_Absyn_Exp*e=_tmp22B;int inline_call=_tmp22C;
# 1272
if(is_calloc){
# 1274
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*_check_null(topt),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp29F[5];({struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp2A0="calloc(";_tag_fat(_tmp2A0,sizeof(char),8U);}));_tmp29F[0]=_tmp668;}),({struct Cyc_PP_Doc*_tmp667=Cyc_Absynpp_exp2doc(e);_tmp29F[1]=_tmp667;}),({struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp2A1=",";_tag_fat(_tmp2A1,sizeof(char),2U);}));_tmp29F[2]=_tmp666;}),({struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_exp2doc(st);_tmp29F[3]=_tmp665;}),({struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp2A2=")";_tag_fat(_tmp2A2,sizeof(char),2U);}));_tmp29F[4]=_tmp664;});Cyc_PP_cat(_tag_fat(_tmp29F,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1278
s=({struct Cyc_PP_Doc*_tmp2A3[7];({struct Cyc_PP_Doc*_tmp66F=Cyc_PP_text(({const char*_tmp2A4="rcalloc(";_tag_fat(_tmp2A4,sizeof(char),9U);}));_tmp2A3[0]=_tmp66F;}),({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_exp2doc(rgnopt);_tmp2A3[1]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp2A5=",";_tag_fat(_tmp2A5,sizeof(char),2U);}));_tmp2A3[2]=_tmp66D;}),({
struct Cyc_PP_Doc*_tmp66C=Cyc_Absynpp_exp2doc(e);_tmp2A3[3]=_tmp66C;}),({struct Cyc_PP_Doc*_tmp66B=Cyc_PP_text(({const char*_tmp2A6=",";_tag_fat(_tmp2A6,sizeof(char),2U);}));_tmp2A3[4]=_tmp66B;}),({struct Cyc_PP_Doc*_tmp66A=Cyc_Absynpp_exp2doc(st);_tmp2A3[5]=_tmp66A;}),({struct Cyc_PP_Doc*_tmp669=Cyc_PP_text(({const char*_tmp2A7=")";_tag_fat(_tmp2A7,sizeof(char),2U);}));_tmp2A3[6]=_tmp669;});Cyc_PP_cat(_tag_fat(_tmp2A3,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1281
struct Cyc_Absyn_Exp*new_e;
# 1283
if(topt == 0)
new_e=e;else{
# 1286
new_e=({struct Cyc_Absyn_Exp*_tmp670=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp670,e,0U);});}
# 1288
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_tmp2A8[3];({struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(({const char*_tmp2A9="malloc(";_tag_fat(_tmp2A9,sizeof(char),8U);}));_tmp2A8[0]=_tmp673;}),({struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_exp2doc(new_e);_tmp2A8[1]=_tmp672;}),({struct Cyc_PP_Doc*_tmp671=Cyc_PP_text(({const char*_tmp2AA=")";_tag_fat(_tmp2AA,sizeof(char),2U);}));_tmp2A8[2]=_tmp671;});Cyc_PP_cat(_tag_fat(_tmp2A8,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1291
if(inline_call)
s=({struct Cyc_PP_Doc*_tmp2AB[5];({struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp2AC="rmalloc_inline(";_tag_fat(_tmp2AC,sizeof(char),16U);}));_tmp2AB[0]=_tmp678;}),({struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_exp2doc(rgnopt);_tmp2AB[1]=_tmp677;}),({struct Cyc_PP_Doc*_tmp676=Cyc_PP_text(({const char*_tmp2AD=",";_tag_fat(_tmp2AD,sizeof(char),2U);}));_tmp2AB[2]=_tmp676;}),({
struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_exp2doc(new_e);_tmp2AB[3]=_tmp675;}),({struct Cyc_PP_Doc*_tmp674=Cyc_PP_text(({const char*_tmp2AE=")";_tag_fat(_tmp2AE,sizeof(char),2U);}));_tmp2AB[4]=_tmp674;});Cyc_PP_cat(_tag_fat(_tmp2AB,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1295
s=({struct Cyc_PP_Doc*_tmp2AF[5];({struct Cyc_PP_Doc*_tmp67D=Cyc_PP_text(({const char*_tmp2B0="rmalloc(";_tag_fat(_tmp2B0,sizeof(char),9U);}));_tmp2AF[0]=_tmp67D;}),({struct Cyc_PP_Doc*_tmp67C=Cyc_Absynpp_exp2doc(rgnopt);_tmp2AF[1]=_tmp67C;}),({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp2B1=",";_tag_fat(_tmp2B1,sizeof(char),2U);}));_tmp2AF[2]=_tmp67B;}),({
struct Cyc_PP_Doc*_tmp67A=Cyc_Absynpp_exp2doc(new_e);_tmp2AF[3]=_tmp67A;}),({struct Cyc_PP_Doc*_tmp679=Cyc_PP_text(({const char*_tmp2B2=")";_tag_fat(_tmp2B2,sizeof(char),2U);}));_tmp2AF[4]=_tmp679;});Cyc_PP_cat(_tag_fat(_tmp2AF,sizeof(struct Cyc_PP_Doc*),5));});}}}
# 1299
goto _LL0;}case 35: _tmp228=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;
# 1301
s=({struct Cyc_PP_Doc*_tmp2B3[3];({struct Cyc_PP_Doc*_tmp680=Cyc_Absynpp_exp2doc_prec(myprec,e1);_tmp2B3[0]=_tmp680;}),({struct Cyc_PP_Doc*_tmp67F=Cyc_PP_text(({const char*_tmp2B4=" :=: ";_tag_fat(_tmp2B4,sizeof(char),6U);}));_tmp2B3[1]=_tmp67F;}),({struct Cyc_PP_Doc*_tmp67E=Cyc_Absynpp_exp2doc_prec(myprec,e2);_tmp2B3[2]=_tmp67E;});Cyc_PP_cat(_tag_fat(_tmp2B3,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 36: _tmp228=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp227)->f1;_tmp229=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp227)->f2;{struct Cyc_Core_Opt*n=_tmp228;struct Cyc_List_List*des=_tmp229;
# 1304
s=({struct _fat_ptr _tmp681=({const char*_tmp2B5=",";_tag_fat(_tmp2B5,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp681,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _tmp228=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp227)->f1;{struct Cyc_Absyn_Stmt*s2=_tmp228;
# 1307
s=({struct Cyc_PP_Doc*_tmp2B6[7];({struct Cyc_PP_Doc*_tmp688=Cyc_PP_text(({const char*_tmp2B7="(";_tag_fat(_tmp2B7,sizeof(char),2U);}));_tmp2B6[0]=_tmp688;}),({struct Cyc_PP_Doc*_tmp687=Cyc_Absynpp_lb();_tmp2B6[1]=_tmp687;}),({struct Cyc_PP_Doc*_tmp686=Cyc_PP_blank_doc();_tmp2B6[2]=_tmp686;}),({
struct Cyc_PP_Doc*_tmp685=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_tmp2B6[3]=_tmp685;}),({
struct Cyc_PP_Doc*_tmp684=Cyc_PP_blank_doc();_tmp2B6[4]=_tmp684;}),({struct Cyc_PP_Doc*_tmp683=Cyc_Absynpp_rb();_tmp2B6[5]=_tmp683;}),({struct Cyc_PP_Doc*_tmp682=Cyc_PP_text(({const char*_tmp2B8=")";_tag_fat(_tmp2B8,sizeof(char),2U);}));_tmp2B6[6]=_tmp682;});Cyc_PP_cat(_tag_fat(_tmp2B6,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}}_LL0:;}
# 1312
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2B9[3];({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(({const char*_tmp2BA="(";_tag_fat(_tmp2BA,sizeof(char),2U);}));_tmp2B9[0]=_tmp68A;}),_tmp2B9[1]=s,({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp2BB=")";_tag_fat(_tmp2BB,sizeof(char),2U);}));_tmp2B9[2]=_tmp689;});Cyc_PP_cat(_tag_fat(_tmp2B9,sizeof(struct Cyc_PP_Doc*),3));});
return s;}
# 1317
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2BC=d;void*_tmp2BD;if(*((int*)_tmp2BC)== 0){_tmp2BD=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2BC)->f1;{struct Cyc_Absyn_Exp*e=_tmp2BD;
return({struct Cyc_PP_Doc*_tmp2BE[3];({struct Cyc_PP_Doc*_tmp68D=Cyc_PP_text(({const char*_tmp2BF=".[";_tag_fat(_tmp2BF,sizeof(char),3U);}));_tmp2BE[0]=_tmp68D;}),({struct Cyc_PP_Doc*_tmp68C=Cyc_Absynpp_exp2doc(e);_tmp2BE[1]=_tmp68C;}),({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp2C0="]";_tag_fat(_tmp2C0,sizeof(char),2U);}));_tmp2BE[2]=_tmp68B;});Cyc_PP_cat(_tag_fat(_tmp2BE,sizeof(struct Cyc_PP_Doc*),3));});}}else{_tmp2BD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2BC)->f1;{struct _fat_ptr*v=_tmp2BD;
return({struct Cyc_PP_Doc*_tmp2C1[2];({struct Cyc_PP_Doc*_tmp68F=Cyc_PP_text(({const char*_tmp2C2=".";_tag_fat(_tmp2C2,sizeof(char),2U);}));_tmp2C1[0]=_tmp68F;}),({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_textptr(v);_tmp2C1[1]=_tmp68E;});Cyc_PP_cat(_tag_fat(_tmp2C1,sizeof(struct Cyc_PP_Doc*),2));});}};}
# 1324
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2C3[2];({struct Cyc_PP_Doc*_tmp694=({struct _fat_ptr _tmp693=({const char*_tmp2C4="";_tag_fat(_tmp2C4,sizeof(char),1U);});struct _fat_ptr _tmp692=({const char*_tmp2C5="=";_tag_fat(_tmp2C5,sizeof(char),2U);});struct _fat_ptr _tmp691=({const char*_tmp2C6="=";_tag_fat(_tmp2C6,sizeof(char),2U);});Cyc_PP_egroup(_tmp693,_tmp692,_tmp691,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2C3[0]=_tmp694;}),({
struct Cyc_PP_Doc*_tmp690=Cyc_Absynpp_exp2doc((*de).f2);_tmp2C3[1]=_tmp690;});Cyc_PP_cat(_tag_fat(_tmp2C3,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1330
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _tmp697=({const char*_tmp2C7="";_tag_fat(_tmp2C7,sizeof(char),1U);});struct _fat_ptr _tmp696=({const char*_tmp2C8="";_tag_fat(_tmp2C8,sizeof(char),1U);});struct _fat_ptr _tmp695=({const char*_tmp2C9=",";_tag_fat(_tmp2C9,sizeof(char),2U);});Cyc_PP_group(_tmp697,_tmp696,_tmp695,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1334
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_tmp2DE=({unsigned _tmp2DD=28U + 1U;char*_tmp2DC=_cycalloc_atomic(_check_times(_tmp2DD,sizeof(char)));({{unsigned _tmp4EB=28U;unsigned i;for(i=0;i < _tmp4EB;++ i){_tmp2DC[i]='z';}_tmp2DC[_tmp4EB]=0;}0;});_tmp2DC;});_tag_fat(_tmp2DE,sizeof(char),29U);});
({struct _fat_ptr _tmp2CA=_fat_ptr_plus(x,sizeof(char),27);char _tmp2CB=*((char*)_check_fat_subscript(_tmp2CA,sizeof(char),0U));char _tmp2CC='\000';if(_get_fat_size(_tmp2CA,sizeof(char))== 1U &&(_tmp2CB == 0 && _tmp2CC != 0))_throw_arraybounds();*((char*)_tmp2CA.curr)=_tmp2CC;});
({struct _fat_ptr _tmp2CD=_fat_ptr_plus(x,sizeof(char),26);char _tmp2CE=*((char*)_check_fat_subscript(_tmp2CD,sizeof(char),0U));char _tmp2CF='L';if(_get_fat_size(_tmp2CD,sizeof(char))== 1U &&(_tmp2CE == 0 && _tmp2CF != 0))_throw_arraybounds();*((char*)_tmp2CD.curr)=_tmp2CF;});
({struct _fat_ptr _tmp2D0=_fat_ptr_plus(x,sizeof(char),25);char _tmp2D1=*((char*)_check_fat_subscript(_tmp2D0,sizeof(char),0U));char _tmp2D2='L';if(_get_fat_size(_tmp2D0,sizeof(char))== 1U &&(_tmp2D1 == 0 && _tmp2D2 != 0))_throw_arraybounds();*((char*)_tmp2D0.curr)=_tmp2D2;});
({struct _fat_ptr _tmp2D3=_fat_ptr_plus(x,sizeof(char),24);char _tmp2D4=*((char*)_check_fat_subscript(_tmp2D3,sizeof(char),0U));char _tmp2D5='U';if(_get_fat_size(_tmp2D3,sizeof(char))== 1U &&(_tmp2D4 == 0 && _tmp2D5 != 0))_throw_arraybounds();*((char*)_tmp2D3.curr)=_tmp2D5;});
({struct _fat_ptr _tmp2D6=_fat_ptr_plus(x,sizeof(char),23);char _tmp2D7=*((char*)_check_fat_subscript(_tmp2D6,sizeof(char),0U));char _tmp2D8='0';if(_get_fat_size(_tmp2D6,sizeof(char))== 1U &&(_tmp2D7 == 0 && _tmp2D8 != 0))_throw_arraybounds();*((char*)_tmp2D6.curr)=_tmp2D8;});{
int index=23;
while(i != 0U){
char c=(char)(48U + i % 10U);
({struct _fat_ptr _tmp2D9=_fat_ptr_plus(x,sizeof(char),index);char _tmp2DA=*((char*)_check_fat_subscript(_tmp2D9,sizeof(char),0U));char _tmp2DB=c;if(_get_fat_size(_tmp2D9,sizeof(char))== 1U &&(_tmp2DA == 0 && _tmp2DB != 0))_throw_arraybounds();*((char*)_tmp2D9.curr)=_tmp2DB;});
i=i / 10U;
-- index;}
# 1348
return(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1352
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2DF=c;long long _tmp2E2;int _tmp2E3;short _tmp2E4;struct _fat_ptr _tmp2E0;char _tmp2E5;enum Cyc_Absyn_Sign _tmp2E1;switch((_tmp2DF.String_c).tag){case 2: _tmp2E1=((_tmp2DF.Char_c).val).f1;_tmp2E5=((_tmp2DF.Char_c).val).f2;{enum Cyc_Absyn_Sign sg=_tmp2E1;char ch=_tmp2E5;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E8=({struct Cyc_String_pa_PrintArg_struct _tmp4EC;_tmp4EC.tag=0,({struct _fat_ptr _tmp698=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp4EC.f1=_tmp698;});_tmp4EC;});void*_tmp2E6[1];_tmp2E6[0]=& _tmp2E8;({struct _fat_ptr _tmp699=({const char*_tmp2E7="'%s'";_tag_fat(_tmp2E7,sizeof(char),5U);});Cyc_aprintf(_tmp699,_tag_fat(_tmp2E6,sizeof(void*),1));});}));}case 3: _tmp2E0=(_tmp2DF.Wchar_c).val;{struct _fat_ptr s=_tmp2E0;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EB=({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0,_tmp4ED.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp4ED;});void*_tmp2E9[1];_tmp2E9[0]=& _tmp2EB;({struct _fat_ptr _tmp69A=({const char*_tmp2EA="L'%s'";_tag_fat(_tmp2EA,sizeof(char),6U);});Cyc_aprintf(_tmp69A,_tag_fat(_tmp2E9,sizeof(void*),1));});}));}case 4: _tmp2E1=((_tmp2DF.Short_c).val).f1;_tmp2E4=((_tmp2DF.Short_c).val).f2;{enum Cyc_Absyn_Sign sg=_tmp2E1;short s=_tmp2E4;
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2EE=({struct Cyc_Int_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=1,_tmp4EE.f1=(unsigned long)((int)s);_tmp4EE;});void*_tmp2EC[1];_tmp2EC[0]=& _tmp2EE;({struct _fat_ptr _tmp69B=({const char*_tmp2ED="%d";_tag_fat(_tmp2ED,sizeof(char),3U);});Cyc_aprintf(_tmp69B,_tag_fat(_tmp2EC,sizeof(void*),1));});}));}case 5: _tmp2E1=((_tmp2DF.Int_c).val).f1;_tmp2E3=((_tmp2DF.Int_c).val).f2;{enum Cyc_Absyn_Sign sn=_tmp2E1;int i=_tmp2E3;
# 1358
if((int)sn == 1)return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2F1=({struct Cyc_Int_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=1,_tmp4EF.f1=(unsigned)i;_tmp4EF;});void*_tmp2EF[1];_tmp2EF[0]=& _tmp2F1;({struct _fat_ptr _tmp69C=({const char*_tmp2F0="%uU";_tag_fat(_tmp2F0,sizeof(char),4U);});Cyc_aprintf(_tmp69C,_tag_fat(_tmp2EF,sizeof(void*),1));});}));else{
return Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2F4=({struct Cyc_Int_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=1,_tmp4F0.f1=(unsigned long)i;_tmp4F0;});void*_tmp2F2[1];_tmp2F2[0]=& _tmp2F4;({struct _fat_ptr _tmp69D=({const char*_tmp2F3="%d";_tag_fat(_tmp2F3,sizeof(char),3U);});Cyc_aprintf(_tmp69D,_tag_fat(_tmp2F2,sizeof(void*),1));});}));}}case 6: _tmp2E1=((_tmp2DF.LongLong_c).val).f1;_tmp2E2=((_tmp2DF.LongLong_c).val).f2;{enum Cyc_Absyn_Sign sg=_tmp2E1;long long i=_tmp2E2;
# 1362
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7: _tmp2E0=((_tmp2DF.Float_c).val).f1;{struct _fat_ptr x=_tmp2E0;
return Cyc_PP_text(x);}case 1:
 return Cyc_PP_text(({const char*_tmp2F5="NULL";_tag_fat(_tmp2F5,sizeof(char),5U);}));case 8: _tmp2E0=(_tmp2DF.String_c).val;{struct _fat_ptr s=_tmp2E0;
return({struct Cyc_PP_Doc*_tmp2F6[3];({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp2F7="\"";_tag_fat(_tmp2F7,sizeof(char),2U);}));_tmp2F6[0]=_tmp6A0;}),({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_tmp2F6[1]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_PP_text(({const char*_tmp2F8="\"";_tag_fat(_tmp2F8,sizeof(char),2U);}));_tmp2F6[2]=_tmp69E;});Cyc_PP_cat(_tag_fat(_tmp2F6,sizeof(struct Cyc_PP_Doc*),3));});}default: _tmp2E0=(_tmp2DF.Wstring_c).val;{struct _fat_ptr s=_tmp2E0;
return({struct Cyc_PP_Doc*_tmp2F9[3];({struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp2FA="L\"";_tag_fat(_tmp2FA,sizeof(char),3U);}));_tmp2F9[0]=_tmp6A3;}),({struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(s);_tmp2F9[1]=_tmp6A2;}),({struct Cyc_PP_Doc*_tmp6A1=Cyc_PP_text(({const char*_tmp2FB="\"";_tag_fat(_tmp2FB,sizeof(char),2U);}));_tmp2F9[2]=_tmp6A1;});Cyc_PP_cat(_tag_fat(_tmp2F9,sizeof(struct Cyc_PP_Doc*),3));});}};}
# 1370
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6A6=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0,({struct _fat_ptr _tmp6A4=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp4F1.f1=_tmp6A4;});_tmp4F1;});void*_tmp2FC[1];_tmp2FC[0]=& _tmp2FE;({struct _fat_ptr _tmp6A5=({const char*_tmp2FD="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_tmp2FD,sizeof(char),47U);});Cyc_aprintf(_tmp6A5,_tag_fat(_tmp2FC,sizeof(void*),1));});});_tmp2FF->f1=_tmp6A6;});_tmp2FF;}));
# 1376
return({struct Cyc_PP_Doc*_tmp300[3];({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_text(({const char*_tmp301="numelts(";_tag_fat(_tmp301,sizeof(char),9U);}));_tmp300[0]=_tmp6A9;}),({struct Cyc_PP_Doc*_tmp6A8=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp300[1]=_tmp6A8;}),({struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_text(({const char*_tmp302=")";_tag_fat(_tmp302,sizeof(char),2U);}));_tmp300[2]=_tmp6A7;});Cyc_PP_cat(_tag_fat(_tmp300,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1378
struct Cyc_List_List*ds=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6AC=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp305=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0,({struct _fat_ptr _tmp6AA=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp4F2.f1=_tmp6AA;});_tmp4F2;});void*_tmp303[1];_tmp303[0]=& _tmp305;({struct _fat_ptr _tmp6AB=({const char*_tmp304="Absynpp::primapp2doc: %s with no args";_tag_fat(_tmp304,sizeof(char),38U);});Cyc_aprintf(_tmp6AB,_tag_fat(_tmp303,sizeof(void*),1));});});_tmp306->f1=_tmp6AC;});_tmp306;}));else{
# 1382
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp307[3];_tmp307[0]=ps,({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp308=" ";_tag_fat(_tmp308,sizeof(char),2U);}));_tmp307[1]=_tmp6AD;}),_tmp307[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_tmp307,sizeof(struct Cyc_PP_Doc*),3));});else{
if((_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6B0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30B=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0,({struct _fat_ptr _tmp6AE=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_tmp4F3.f1=_tmp6AE;});_tmp4F3;});void*_tmp309[1];_tmp309[0]=& _tmp30B;({struct _fat_ptr _tmp6AF=({const char*_tmp30A="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_tmp30A,sizeof(char),47U);});Cyc_aprintf(_tmp6AF,_tag_fat(_tmp309,sizeof(void*),1));});});_tmp30C->f1=_tmp6B0;});_tmp30C;}));else{
# 1388
return({struct Cyc_PP_Doc*_tmp30D[5];_tmp30D[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp6B2=Cyc_PP_text(({const char*_tmp30E=" ";_tag_fat(_tmp30E,sizeof(char),2U);}));_tmp30D[1]=_tmp6B2;}),_tmp30D[2]=ps,({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp30F=" ";_tag_fat(_tmp30F,sizeof(char),2U);}));_tmp30D[3]=_tmp6B1;}),_tmp30D[4]=(struct Cyc_PP_Doc*)(_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_tmp30D,sizeof(struct Cyc_PP_Doc*),5));});}}}}}
# 1392
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp310=p;switch((int)_tmp310){case Cyc_Absyn_Plus:
 return({const char*_tmp311="+";_tag_fat(_tmp311,sizeof(char),2U);});case Cyc_Absyn_Times:
 return({const char*_tmp312="*";_tag_fat(_tmp312,sizeof(char),2U);});case Cyc_Absyn_Minus:
 return({const char*_tmp313="-";_tag_fat(_tmp313,sizeof(char),2U);});case Cyc_Absyn_Div:
 return({const char*_tmp314="/";_tag_fat(_tmp314,sizeof(char),2U);});case Cyc_Absyn_Mod:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp315="\\%";_tag_fat(_tmp315,sizeof(char),3U);}):({const char*_tmp316="%";_tag_fat(_tmp316,sizeof(char),2U);});case Cyc_Absyn_Eq:
 return({const char*_tmp317="==";_tag_fat(_tmp317,sizeof(char),3U);});case Cyc_Absyn_Neq:
 return({const char*_tmp318="!=";_tag_fat(_tmp318,sizeof(char),3U);});case Cyc_Absyn_Gt:
 return({const char*_tmp319=">";_tag_fat(_tmp319,sizeof(char),2U);});case Cyc_Absyn_Lt:
 return({const char*_tmp31A="<";_tag_fat(_tmp31A,sizeof(char),2U);});case Cyc_Absyn_Gte:
 return({const char*_tmp31B=">=";_tag_fat(_tmp31B,sizeof(char),3U);});case Cyc_Absyn_Lte:
 return({const char*_tmp31C="<=";_tag_fat(_tmp31C,sizeof(char),3U);});case Cyc_Absyn_Not:
 return({const char*_tmp31D="!";_tag_fat(_tmp31D,sizeof(char),2U);});case Cyc_Absyn_Bitnot:
 return({const char*_tmp31E="~";_tag_fat(_tmp31E,sizeof(char),2U);});case Cyc_Absyn_Bitand:
 return({const char*_tmp31F="&";_tag_fat(_tmp31F,sizeof(char),2U);});case Cyc_Absyn_Bitor:
 return({const char*_tmp320="|";_tag_fat(_tmp320,sizeof(char),2U);});case Cyc_Absyn_Bitxor:
 return({const char*_tmp321="^";_tag_fat(_tmp321,sizeof(char),2U);});case Cyc_Absyn_Bitlshift:
 return({const char*_tmp322="<<";_tag_fat(_tmp322,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift:
 return({const char*_tmp323=">>";_tag_fat(_tmp323,sizeof(char),3U);});case Cyc_Absyn_Numelts:
 return({const char*_tmp324="numelts";_tag_fat(_tmp324,sizeof(char),8U);});default:
 return({const char*_tmp325="?";_tag_fat(_tmp325,sizeof(char),2U);});};}
# 1417
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1421
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp326=s->r;void*_stmttmp17=_tmp326;void*_tmp327=_stmttmp17;if(*((int*)_tmp327)== 12)
return 1;else{
return 0;};}
# 1436 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_tmp328=d->r;void*_stmttmp18=_tmp328;void*_tmp329=_stmttmp18;void*_tmp32A;if(*((int*)_tmp329)== 0){_tmp32A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp329)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp32A;
# 1439
if(({(int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))Cyc_List_mem;})(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _tmp4F4;_tmp4F4.f1=1,({struct Cyc_List_List*_tmp6B3=({struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->hd=vd->name,_tmp32B->tl=0;_tmp32B;});_tmp4F4.f2=_tmp6B3;});_tmp4F4;});
return({struct _tuple14 _tmp4F5;_tmp4F5.f1=0,({struct Cyc_List_List*_tmp6B4=({struct Cyc_List_List*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->hd=vd->name,_tmp32C->tl=varsinblock;_tmp32C;});_tmp4F5.f2=_tmp6B4;});_tmp4F5;});}}else{
return({struct _tuple14 _tmp4F6;_tmp4F6.f1=0,_tmp4F6.f2=varsinblock;_tmp4F6;});};}
# 1446
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_tmp32D[8];({struct Cyc_PP_Doc*_tmp6BC=Cyc_PP_text(({const char*_tmp32E="(";_tag_fat(_tmp32E,sizeof(char),2U);}));_tmp32D[0]=_tmp6BC;}),({struct Cyc_PP_Doc*_tmp6BB=Cyc_Absynpp_lb();_tmp32D[1]=_tmp6BB;}),({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_blank_doc();_tmp32D[2]=_tmp6BA;}),({struct Cyc_PP_Doc*_tmp6B9=Cyc_PP_nest(2,d);_tmp32D[3]=_tmp6B9;}),({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_line_doc();_tmp32D[4]=_tmp6B8;}),({struct Cyc_PP_Doc*_tmp6B7=Cyc_Absynpp_rb();_tmp32D[5]=_tmp6B7;}),({
struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_text(({const char*_tmp32F=");";_tag_fat(_tmp32F,sizeof(char),3U);}));_tmp32D[6]=_tmp6B6;}),({struct Cyc_PP_Doc*_tmp6B5=Cyc_PP_line_doc();_tmp32D[7]=_tmp6B5;});Cyc_PP_cat(_tag_fat(_tmp32D,sizeof(struct Cyc_PP_Doc*),8));});
return({struct Cyc_PP_Doc*_tmp330[6];({struct Cyc_PP_Doc*_tmp6C2=Cyc_Absynpp_lb();_tmp330[0]=_tmp6C2;}),({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_blank_doc();_tmp330[1]=_tmp6C1;}),({struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_nest(2,d);_tmp330[2]=_tmp6C0;}),({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_line_doc();_tmp330[3]=_tmp6BF;}),({struct Cyc_PP_Doc*_tmp6BE=Cyc_Absynpp_rb();_tmp330[4]=_tmp6BE;}),({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_line_doc();_tmp330[5]=_tmp6BD;});Cyc_PP_cat(_tag_fat(_tmp330,sizeof(struct Cyc_PP_Doc*),6));});}
# 1453
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_tmp331=st->r;void*_stmttmp19=_tmp331;void*_tmp332=_stmttmp19;void*_tmp336;void*_tmp335;void*_tmp334;void*_tmp333;switch(*((int*)_tmp332)){case 0:
 s=Cyc_PP_text(({const char*_tmp337=";";_tag_fat(_tmp337,sizeof(char),2U);}));goto _LL0;case 1: _tmp333=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;{struct Cyc_Absyn_Exp*e=_tmp333;
s=({struct Cyc_PP_Doc*_tmp338[2];({struct Cyc_PP_Doc*_tmp6C4=Cyc_Absynpp_exp2doc(e);_tmp338[0]=_tmp6C4;}),({struct Cyc_PP_Doc*_tmp6C3=Cyc_PP_text(({const char*_tmp339=";";_tag_fat(_tmp339,sizeof(char),2U);}));_tmp338[1]=_tmp6C3;});Cyc_PP_cat(_tag_fat(_tmp338,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 2: _tmp333=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp333;struct Cyc_Absyn_Stmt*s2=_tmp334;
# 1459
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp33A[2];({struct Cyc_PP_Doc*_tmp6C8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp33A[0]=_tmp6C8;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_tmp6C7=({int _tmp6C6=stmtexp;Cyc_Absynpp_block(_tmp6C6,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp33A[1]=_tmp6C7;}):({
struct Cyc_PP_Doc*_tmp6C5=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp33A[1]=_tmp6C5;});Cyc_PP_cat(_tag_fat(_tmp33A,sizeof(struct Cyc_PP_Doc*),2));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_tmp33B[3];({struct Cyc_PP_Doc*_tmp6CC=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp33B[0]=_tmp6CC;}),({
struct Cyc_PP_Doc*_tmp6CB=Cyc_PP_line_doc();_tmp33B[1]=_tmp6CB;}),({
struct Cyc_PP_Doc*_tmp6CA=({int _tmp6C9=stmtexp;Cyc_Absynpp_block(_tmp6C9,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_tmp33B[2]=_tmp6CA;});Cyc_PP_cat(_tag_fat(_tmp33B,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1470
s=({struct Cyc_PP_Doc*_tmp33C[3];({struct Cyc_PP_Doc*_tmp6CF=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_tmp33C[0]=_tmp6CF;}),({struct Cyc_PP_Doc*_tmp6CE=Cyc_PP_line_doc();_tmp33C[1]=_tmp6CE;}),({
struct Cyc_PP_Doc*_tmp6CD=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_tmp33C[2]=_tmp6CD;});Cyc_PP_cat(_tag_fat(_tmp33C,sizeof(struct Cyc_PP_Doc*),3));});}}}else{
# 1474
s=({struct Cyc_PP_Doc*_tmp33D[3];({struct Cyc_PP_Doc*_tmp6D2=Cyc_Absynpp_stmt2doc(s1,0,0);_tmp33D[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_line_doc();_tmp33D[1]=_tmp6D1;}),({struct Cyc_PP_Doc*_tmp6D0=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_tmp33D[2]=_tmp6D0;});Cyc_PP_cat(_tag_fat(_tmp33D,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 3: _tmp333=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp333;
# 1477
if(eopt == 0)
s=Cyc_PP_text(({const char*_tmp33E="return;";_tag_fat(_tmp33E,sizeof(char),8U);}));else{
# 1480
s=({struct Cyc_PP_Doc*_tmp33F[3];({struct Cyc_PP_Doc*_tmp6D6=Cyc_PP_text(({const char*_tmp340="return ";_tag_fat(_tmp340,sizeof(char),8U);}));_tmp33F[0]=_tmp6D6;}),
eopt == 0?({struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_nil_doc();_tmp33F[1]=_tmp6D5;}):({struct Cyc_PP_Doc*_tmp6D4=Cyc_Absynpp_exp2doc(eopt);_tmp33F[1]=_tmp6D4;}),({
struct Cyc_PP_Doc*_tmp6D3=Cyc_PP_text(({const char*_tmp341=";";_tag_fat(_tmp341,sizeof(char),2U);}));_tmp33F[2]=_tmp6D3;});Cyc_PP_cat(_tag_fat(_tmp33F,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 4: _tmp333=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;_tmp335=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp332)->f3;{struct Cyc_Absyn_Exp*e=_tmp333;struct Cyc_Absyn_Stmt*s1=_tmp334;struct Cyc_Absyn_Stmt*s2=_tmp335;
# 1485
int print_else;
{void*_tmp342=s2->r;void*_stmttmp1A=_tmp342;void*_tmp343=_stmttmp1A;if(*((int*)_tmp343)== 0){
print_else=0;goto _LL23;}else{
print_else=1;goto _LL23;}_LL23:;}
# 1490
s=({struct Cyc_PP_Doc*_tmp344[5];({struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_text(({const char*_tmp345="if (";_tag_fat(_tmp345,sizeof(char),5U);}));_tmp344[0]=_tmp6DF;}),({
struct Cyc_PP_Doc*_tmp6DE=Cyc_Absynpp_exp2doc(e);_tmp344[1]=_tmp6DE;}),({
struct Cyc_PP_Doc*_tmp6DD=Cyc_PP_text(({const char*_tmp346=") ";_tag_fat(_tmp346,sizeof(char),3U);}));_tmp344[2]=_tmp6DD;}),({
struct Cyc_PP_Doc*_tmp6DC=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp344[3]=_tmp6DC;}),
print_else?({
struct Cyc_PP_Doc*_tmp6DB=({struct Cyc_PP_Doc*_tmp347[3];({struct Cyc_PP_Doc*_tmp6DA=Cyc_PP_line_doc();_tmp347[0]=_tmp6DA;}),({
struct Cyc_PP_Doc*_tmp6D9=Cyc_PP_text(({const char*_tmp348="else ";_tag_fat(_tmp348,sizeof(char),6U);}));_tmp347[1]=_tmp6D9;}),({
struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_tmp347[2]=_tmp6D8;});Cyc_PP_cat(_tag_fat(_tmp347,sizeof(struct Cyc_PP_Doc*),3));});
# 1495
_tmp344[4]=_tmp6DB;}):({
# 1498
struct Cyc_PP_Doc*_tmp6D7=Cyc_PP_nil_doc();_tmp344[4]=_tmp6D7;});Cyc_PP_cat(_tag_fat(_tmp344,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 5: _tmp333=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp332)->f1).f1;_tmp334=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;{struct Cyc_Absyn_Exp*e=_tmp333;struct Cyc_Absyn_Stmt*s1=_tmp334;
# 1501
s=({struct Cyc_PP_Doc*_tmp349[4];({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_text(({const char*_tmp34A="while (";_tag_fat(_tmp34A,sizeof(char),8U);}));_tmp349[0]=_tmp6E3;}),({
struct Cyc_PP_Doc*_tmp6E2=Cyc_Absynpp_exp2doc(e);_tmp349[1]=_tmp6E2;}),({
struct Cyc_PP_Doc*_tmp6E1=Cyc_PP_text(({const char*_tmp34B=") ";_tag_fat(_tmp34B,sizeof(char),3U);}));_tmp349[2]=_tmp6E1;}),({
struct Cyc_PP_Doc*_tmp6E0=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp349[3]=_tmp6E0;});Cyc_PP_cat(_tag_fat(_tmp349,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 6:
 s=Cyc_PP_text(({const char*_tmp34C="break;";_tag_fat(_tmp34C,sizeof(char),7U);}));goto _LL0;case 7:
 s=Cyc_PP_text(({const char*_tmp34D="continue;";_tag_fat(_tmp34D,sizeof(char),10U);}));goto _LL0;case 8: _tmp333=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;{struct _fat_ptr*x=_tmp333;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp350=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0,_tmp4F7.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp4F7;});void*_tmp34E[1];_tmp34E[0]=& _tmp350;({struct _fat_ptr _tmp6E4=({const char*_tmp34F="goto %s;";_tag_fat(_tmp34F,sizeof(char),9U);});Cyc_aprintf(_tmp6E4,_tag_fat(_tmp34E,sizeof(void*),1));});}));goto _LL0;}case 9: _tmp333=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp332)->f2).f1;_tmp335=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp332)->f3).f1;_tmp336=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp332)->f4;{struct Cyc_Absyn_Exp*e1=_tmp333;struct Cyc_Absyn_Exp*e2=_tmp334;struct Cyc_Absyn_Exp*e3=_tmp335;struct Cyc_Absyn_Stmt*s1=_tmp336;
# 1510
s=({struct Cyc_PP_Doc*_tmp351[8];({struct Cyc_PP_Doc*_tmp6EC=Cyc_PP_text(({const char*_tmp352="for(";_tag_fat(_tmp352,sizeof(char),5U);}));_tmp351[0]=_tmp6EC;}),({
struct Cyc_PP_Doc*_tmp6EB=Cyc_Absynpp_exp2doc(e1);_tmp351[1]=_tmp6EB;}),({
struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_text(({const char*_tmp353="; ";_tag_fat(_tmp353,sizeof(char),3U);}));_tmp351[2]=_tmp6EA;}),({
struct Cyc_PP_Doc*_tmp6E9=Cyc_Absynpp_exp2doc(e2);_tmp351[3]=_tmp6E9;}),({
struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_text(({const char*_tmp354="; ";_tag_fat(_tmp354,sizeof(char),3U);}));_tmp351[4]=_tmp6E8;}),({
struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_exp2doc(e3);_tmp351[5]=_tmp6E7;}),({
struct Cyc_PP_Doc*_tmp6E6=Cyc_PP_text(({const char*_tmp355=") ";_tag_fat(_tmp355,sizeof(char),3U);}));_tmp351[6]=_tmp6E6;}),({
struct Cyc_PP_Doc*_tmp6E5=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp351[7]=_tmp6E5;});Cyc_PP_cat(_tag_fat(_tmp351,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 10: _tmp333=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;{struct Cyc_Absyn_Exp*e=_tmp333;struct Cyc_List_List*ss=_tmp334;
# 1520
s=({struct Cyc_PP_Doc*_tmp356[8];({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp357="switch (";_tag_fat(_tmp357,sizeof(char),9U);}));_tmp356[0]=_tmp6F4;}),({
struct Cyc_PP_Doc*_tmp6F3=Cyc_Absynpp_exp2doc(e);_tmp356[1]=_tmp6F3;}),({
struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_text(({const char*_tmp358=") ";_tag_fat(_tmp358,sizeof(char),3U);}));_tmp356[2]=_tmp6F2;}),({
struct Cyc_PP_Doc*_tmp6F1=Cyc_Absynpp_lb();_tmp356[3]=_tmp6F1;}),({
struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_line_doc();_tmp356[4]=_tmp6F0;}),({
struct Cyc_PP_Doc*_tmp6EF=Cyc_Absynpp_switchclauses2doc(ss);_tmp356[5]=_tmp6EF;}),({
struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_line_doc();_tmp356[6]=_tmp6EE;}),({
struct Cyc_PP_Doc*_tmp6ED=Cyc_Absynpp_rb();_tmp356[7]=_tmp6ED;});Cyc_PP_cat(_tag_fat(_tmp356,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp332)->f1 == 0){
s=Cyc_PP_text(({const char*_tmp359="fallthru;";_tag_fat(_tmp359,sizeof(char),10U);}));goto _LL0;}else{_tmp333=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;{struct Cyc_List_List*es=_tmp333;
# 1531
s=({struct Cyc_PP_Doc*_tmp35A[3];({struct Cyc_PP_Doc*_tmp6F7=Cyc_PP_text(({const char*_tmp35B="fallthru(";_tag_fat(_tmp35B,sizeof(char),10U);}));_tmp35A[0]=_tmp6F7;}),({struct Cyc_PP_Doc*_tmp6F6=Cyc_Absynpp_exps2doc_prec(20,es);_tmp35A[1]=_tmp6F6;}),({struct Cyc_PP_Doc*_tmp6F5=Cyc_PP_text(({const char*_tmp35C=");";_tag_fat(_tmp35C,sizeof(char),3U);}));_tmp35A[2]=_tmp6F5;});Cyc_PP_cat(_tag_fat(_tmp35A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}}case 12: _tmp333=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;{struct Cyc_Absyn_Decl*d=_tmp333;struct Cyc_Absyn_Stmt*s1=_tmp334;
# 1533
struct _tuple14 _tmp35D=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _stmttmp1B=_tmp35D;struct _tuple14 _tmp35E=_stmttmp1B;void*_tmp360;int _tmp35F;_tmp35F=_tmp35E.f1;_tmp360=_tmp35E.f2;{int newblock=_tmp35F;struct Cyc_List_List*newvarsinblock=_tmp360;
s=({struct Cyc_PP_Doc*_tmp361[3];({struct Cyc_PP_Doc*_tmp6FA=Cyc_Absynpp_decl2doc(d);_tmp361[0]=_tmp6FA;}),({struct Cyc_PP_Doc*_tmp6F9=Cyc_PP_line_doc();_tmp361[1]=_tmp6F9;}),({struct Cyc_PP_Doc*_tmp6F8=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_tmp361[2]=_tmp6F8;});Cyc_PP_cat(_tag_fat(_tmp361,sizeof(struct Cyc_PP_Doc*),3));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13: _tmp333=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;{struct _fat_ptr*x=_tmp333;struct Cyc_Absyn_Stmt*s1=_tmp334;
# 1538
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_tmp362[3];({struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_textptr(x);_tmp362[0]=_tmp6FE;}),({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_text(({const char*_tmp363=": ";_tag_fat(_tmp363,sizeof(char),3U);}));_tmp362[1]=_tmp6FD;}),({struct Cyc_PP_Doc*_tmp6FC=({int _tmp6FB=stmtexp;Cyc_Absynpp_block(_tmp6FB,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_tmp362[2]=_tmp6FC;});Cyc_PP_cat(_tag_fat(_tmp362,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1541
s=({struct Cyc_PP_Doc*_tmp364[3];({struct Cyc_PP_Doc*_tmp701=Cyc_PP_textptr(x);_tmp364[0]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_PP_text(({const char*_tmp365=": ";_tag_fat(_tmp365,sizeof(char),3U);}));_tmp364[1]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_tmp364[2]=_tmp6FF;});Cyc_PP_cat(_tag_fat(_tmp364,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 14: _tmp333=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp332)->f2).f1;{struct Cyc_Absyn_Stmt*s1=_tmp333;struct Cyc_Absyn_Exp*e=_tmp334;
# 1544
s=({struct Cyc_PP_Doc*_tmp366[5];({struct Cyc_PP_Doc*_tmp706=Cyc_PP_text(({const char*_tmp367="do ";_tag_fat(_tmp367,sizeof(char),4U);}));_tmp366[0]=_tmp706;}),({
struct Cyc_PP_Doc*_tmp705=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp366[1]=_tmp705;}),({
struct Cyc_PP_Doc*_tmp704=Cyc_PP_text(({const char*_tmp368=" while (";_tag_fat(_tmp368,sizeof(char),9U);}));_tmp366[2]=_tmp704;}),({
struct Cyc_PP_Doc*_tmp703=Cyc_Absynpp_exp2doc(e);_tmp366[3]=_tmp703;}),({
struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp369=");";_tag_fat(_tmp369,sizeof(char),3U);}));_tmp366[4]=_tmp702;});Cyc_PP_cat(_tag_fat(_tmp366,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}default: _tmp333=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp332)->f1;_tmp334=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp332)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp333;struct Cyc_List_List*ss=_tmp334;
# 1551
s=({struct Cyc_PP_Doc*_tmp36A[4];({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_text(({const char*_tmp36B="try ";_tag_fat(_tmp36B,sizeof(char),5U);}));_tmp36A[0]=_tmp70A;}),({
struct Cyc_PP_Doc*_tmp709=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_tmp36A[1]=_tmp709;}),({
struct Cyc_PP_Doc*_tmp708=Cyc_PP_text(({const char*_tmp36C=" catch ";_tag_fat(_tmp36C,sizeof(char),8U);}));_tmp36A[2]=_tmp708;}),({
struct Cyc_PP_Doc*_tmp707=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_tmp36A[3]=_tmp707;});Cyc_PP_cat(_tag_fat(_tmp36A,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}}_LL0:;}
# 1557
return s;}
# 1560
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp36D=p->r;void*_stmttmp1C=_tmp36D;void*_tmp36E=_stmttmp1C;union Cyc_Absyn_AggrInfo _tmp372;void*_tmp370;void*_tmp36F;struct _fat_ptr _tmp373;char _tmp374;int _tmp371;enum Cyc_Absyn_Sign _tmp375;switch(*((int*)_tmp36E)){case 0:
 s=Cyc_PP_text(({const char*_tmp376="_";_tag_fat(_tmp376,sizeof(char),2U);}));goto _LL0;case 9:
 s=Cyc_PP_text(({const char*_tmp377="NULL";_tag_fat(_tmp377,sizeof(char),5U);}));goto _LL0;case 10: _tmp375=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp371=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{enum Cyc_Absyn_Sign sg=_tmp375;int i=_tmp371;
# 1566
if((int)sg != (int)1U)
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp37A=({struct Cyc_Int_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=1,_tmp4F8.f1=(unsigned long)i;_tmp4F8;});void*_tmp378[1];_tmp378[0]=& _tmp37A;({struct _fat_ptr _tmp70B=({const char*_tmp379="%d";_tag_fat(_tmp379,sizeof(char),3U);});Cyc_aprintf(_tmp70B,_tag_fat(_tmp378,sizeof(void*),1));});}));else{
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp37D=({struct Cyc_Int_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=1,_tmp4F9.f1=(unsigned)i;_tmp4F9;});void*_tmp37B[1];_tmp37B[0]=& _tmp37D;({struct _fat_ptr _tmp70C=({const char*_tmp37C="%u";_tag_fat(_tmp37C,sizeof(char),3U);});Cyc_aprintf(_tmp70C,_tag_fat(_tmp37B,sizeof(void*),1));});}));}
goto _LL0;}case 11: _tmp374=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{char ch=_tmp374;
s=Cyc_PP_text((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp380=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0,({struct _fat_ptr _tmp70D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_tmp4FA.f1=_tmp70D;});_tmp4FA;});void*_tmp37E[1];_tmp37E[0]=& _tmp380;({struct _fat_ptr _tmp70E=({const char*_tmp37F="'%s'";_tag_fat(_tmp37F,sizeof(char),5U);});Cyc_aprintf(_tmp70E,_tag_fat(_tmp37E,sizeof(void*),1));});}));goto _LL0;}case 12: _tmp373=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{struct _fat_ptr x=_tmp373;
s=Cyc_PP_text(x);goto _LL0;}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp36E)->f2)->r)== 0){_tmp36F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp36F;
# 1573
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_tmp36F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp370=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp36F;struct Cyc_Absyn_Pat*p2=_tmp370;
# 1575
s=({struct Cyc_PP_Doc*_tmp381[3];({struct Cyc_PP_Doc*_tmp711=Cyc_Absynpp_qvar2doc(vd->name);_tmp381[0]=_tmp711;}),({struct Cyc_PP_Doc*_tmp710=Cyc_PP_text(({const char*_tmp382=" as ";_tag_fat(_tmp382,sizeof(char),5U);}));_tmp381[1]=_tmp710;}),({struct Cyc_PP_Doc*_tmp70F=Cyc_Absynpp_pat2doc(p2);_tmp381[2]=_tmp70F;});Cyc_PP_cat(_tag_fat(_tmp381,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}}case 2: _tmp36F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp370=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp36F;struct Cyc_Absyn_Vardecl*vd=_tmp370;
# 1577
s=({struct Cyc_PP_Doc*_tmp383[5];({struct Cyc_PP_Doc*_tmp716=Cyc_PP_text(({const char*_tmp384="alias";_tag_fat(_tmp384,sizeof(char),6U);}));_tmp383[0]=_tmp716;}),({struct Cyc_PP_Doc*_tmp715=Cyc_PP_text(({const char*_tmp385=" <";_tag_fat(_tmp385,sizeof(char),3U);}));_tmp383[1]=_tmp715;}),({struct Cyc_PP_Doc*_tmp714=Cyc_Absynpp_tvar2doc(tv);_tmp383[2]=_tmp714;}),({struct Cyc_PP_Doc*_tmp713=Cyc_PP_text(({const char*_tmp386="> ";_tag_fat(_tmp386,sizeof(char),3U);}));_tmp383[3]=_tmp713;}),({
struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_vardecl2doc(vd);_tmp383[4]=_tmp712;});Cyc_PP_cat(_tag_fat(_tmp383,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 4: _tmp36F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp370=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_Absyn_Tvar*tv=_tmp36F;struct Cyc_Absyn_Vardecl*vd=_tmp370;
# 1581
s=({struct Cyc_PP_Doc*_tmp387[4];({struct Cyc_PP_Doc*_tmp71A=Cyc_Absynpp_qvar2doc(vd->name);_tmp387[0]=_tmp71A;}),({struct Cyc_PP_Doc*_tmp719=Cyc_PP_text(({const char*_tmp388="<";_tag_fat(_tmp388,sizeof(char),2U);}));_tmp387[1]=_tmp719;}),({struct Cyc_PP_Doc*_tmp718=Cyc_Absynpp_tvar2doc(tv);_tmp387[2]=_tmp718;}),({struct Cyc_PP_Doc*_tmp717=Cyc_PP_text(({const char*_tmp389=">";_tag_fat(_tmp389,sizeof(char),2U);}));_tmp387[3]=_tmp717;});Cyc_PP_cat(_tag_fat(_tmp387,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 5: _tmp36F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp371=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_List_List*ts=_tmp36F;int dots=_tmp371;
# 1584
s=({struct Cyc_PP_Doc*_tmp38A[4];({struct Cyc_PP_Doc*_tmp721=Cyc_Absynpp_dollar();_tmp38A[0]=_tmp721;}),({struct Cyc_PP_Doc*_tmp720=Cyc_PP_text(({const char*_tmp38B="(";_tag_fat(_tmp38B,sizeof(char),2U);}));_tmp38A[1]=_tmp720;}),({struct Cyc_PP_Doc*_tmp71F=({struct Cyc_PP_Doc*(*_tmp71E)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;});struct _fat_ptr _tmp71D=({const char*_tmp38C=",";_tag_fat(_tmp38C,sizeof(char),2U);});_tmp71E(Cyc_Absynpp_pat2doc,_tmp71D,ts);});_tmp38A[2]=_tmp71F;}),
dots?({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp38D=", ...)";_tag_fat(_tmp38D,sizeof(char),7U);}));_tmp38A[3]=_tmp71C;}):({struct Cyc_PP_Doc*_tmp71B=Cyc_PP_text(({const char*_tmp38E=")";_tag_fat(_tmp38E,sizeof(char),2U);}));_tmp38A[3]=_tmp71B;});Cyc_PP_cat(_tag_fat(_tmp38A,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 6: _tmp36F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{struct Cyc_Absyn_Pat*p2=_tmp36F;
# 1588
s=({struct Cyc_PP_Doc*_tmp38F[2];({struct Cyc_PP_Doc*_tmp723=Cyc_PP_text(({const char*_tmp390="&";_tag_fat(_tmp390,sizeof(char),2U);}));_tmp38F[0]=_tmp723;}),({struct Cyc_PP_Doc*_tmp722=Cyc_Absynpp_pat2doc(p2);_tmp38F[1]=_tmp722;});Cyc_PP_cat(_tag_fat(_tmp38F,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp36E)->f2)->r)== 0){_tmp36F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp36F;
# 1591
s=({struct Cyc_PP_Doc*_tmp391[2];({struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp392="*";_tag_fat(_tmp392,sizeof(char),2U);}));_tmp391[0]=_tmp725;}),({struct Cyc_PP_Doc*_tmp724=Cyc_Absynpp_qvar2doc(vd->name);_tmp391[1]=_tmp724;});Cyc_PP_cat(_tag_fat(_tmp391,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}}else{_tmp36F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp370=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp36F;struct Cyc_Absyn_Pat*p2=_tmp370;
# 1594
s=({struct Cyc_PP_Doc*_tmp393[4];({struct Cyc_PP_Doc*_tmp729=Cyc_PP_text(({const char*_tmp394="*";_tag_fat(_tmp394,sizeof(char),2U);}));_tmp393[0]=_tmp729;}),({struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_qvar2doc(vd->name);_tmp393[1]=_tmp728;}),({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp395=" as ";_tag_fat(_tmp395,sizeof(char),5U);}));_tmp393[2]=_tmp727;}),({struct Cyc_PP_Doc*_tmp726=Cyc_Absynpp_pat2doc(p2);_tmp393[3]=_tmp726;});Cyc_PP_cat(_tag_fat(_tmp393,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}}case 15: _tmp36F=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{struct _tuple0*q=_tmp36F;
# 1597
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16: _tmp36F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp370=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;_tmp371=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp36E)->f3;{struct _tuple0*q=_tmp36F;struct Cyc_List_List*ps=_tmp370;int dots=_tmp371;
# 1600
struct _fat_ptr term=dots?({const char*_tmp399=", ...)";_tag_fat(_tmp399,sizeof(char),7U);}):({const char*_tmp39A=")";_tag_fat(_tmp39A,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp396[2];({struct Cyc_PP_Doc*_tmp72E=Cyc_Absynpp_qvar2doc(q);_tmp396[0]=_tmp72E;}),({struct Cyc_PP_Doc*_tmp72D=({struct _fat_ptr _tmp72C=({const char*_tmp397="(";_tag_fat(_tmp397,sizeof(char),2U);});struct _fat_ptr _tmp72B=term;struct _fat_ptr _tmp72A=({const char*_tmp398=",";_tag_fat(_tmp398,sizeof(char),2U);});Cyc_PP_group(_tmp72C,_tmp72B,_tmp72A,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_pat2doc,ps));});_tmp396[1]=_tmp72D;});Cyc_PP_cat(_tag_fat(_tmp396,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f1 != 0){_tmp372=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;_tmp36F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;_tmp370=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f3;_tmp371=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f4;{union Cyc_Absyn_AggrInfo info=_tmp372;struct Cyc_List_List*exists=_tmp36F;struct Cyc_List_List*dps=_tmp370;int dots=_tmp371;
# 1604
struct _fat_ptr term=dots?({const char*_tmp3A4=", ...}";_tag_fat(_tmp3A4,sizeof(char),7U);}):({const char*_tmp3A5="}";_tag_fat(_tmp3A5,sizeof(char),2U);});
struct _tuple11 _tmp39B=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmp1D=_tmp39B;struct _tuple11 _tmp39C=_stmttmp1D;void*_tmp39D;_tmp39D=_tmp39C.f2;{struct _tuple0*n=_tmp39D;
s=({struct Cyc_PP_Doc*_tmp39E[4];({struct Cyc_PP_Doc*_tmp738=Cyc_Absynpp_qvar2doc(n);_tmp39E[0]=_tmp738;}),({struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_lb();_tmp39E[1]=_tmp737;}),({
struct Cyc_PP_Doc*_tmp736=({struct _fat_ptr _tmp735=({const char*_tmp39F="[";_tag_fat(_tmp39F,sizeof(char),2U);});struct _fat_ptr _tmp734=({const char*_tmp3A0="]";_tag_fat(_tmp3A0,sizeof(char),2U);});struct _fat_ptr _tmp733=({const char*_tmp3A1=",";_tag_fat(_tmp3A1,sizeof(char),2U);});Cyc_PP_egroup(_tmp735,_tmp734,_tmp733,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,exists));});_tmp39E[2]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp732=({struct _fat_ptr _tmp731=({const char*_tmp3A2="";_tag_fat(_tmp3A2,sizeof(char),1U);});struct _fat_ptr _tmp730=term;struct _fat_ptr _tmp72F=({const char*_tmp3A3=",";_tag_fat(_tmp3A3,sizeof(char),2U);});Cyc_PP_group(_tmp731,_tmp730,_tmp72F,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_dp2doc,dps));});_tmp39E[3]=_tmp732;});Cyc_PP_cat(_tag_fat(_tmp39E,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}}}else{_tmp36F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;_tmp370=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f3;_tmp371=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp36E)->f4;{struct Cyc_List_List*exists=_tmp36F;struct Cyc_List_List*dps=_tmp370;int dots=_tmp371;
# 1611
struct _fat_ptr term=dots?({const char*_tmp3AC=", ...}";_tag_fat(_tmp3AC,sizeof(char),7U);}):({const char*_tmp3AD="}";_tag_fat(_tmp3AD,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3A6[3];({struct Cyc_PP_Doc*_tmp741=Cyc_Absynpp_lb();_tmp3A6[0]=_tmp741;}),({
struct Cyc_PP_Doc*_tmp740=({struct _fat_ptr _tmp73F=({const char*_tmp3A7="[";_tag_fat(_tmp3A7,sizeof(char),2U);});struct _fat_ptr _tmp73E=({const char*_tmp3A8="]";_tag_fat(_tmp3A8,sizeof(char),2U);});struct _fat_ptr _tmp73D=({const char*_tmp3A9=",";_tag_fat(_tmp3A9,sizeof(char),2U);});Cyc_PP_egroup(_tmp73F,_tmp73E,_tmp73D,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,exists));});_tmp3A6[1]=_tmp740;}),({
struct Cyc_PP_Doc*_tmp73C=({struct _fat_ptr _tmp73B=({const char*_tmp3AA="";_tag_fat(_tmp3AA,sizeof(char),1U);});struct _fat_ptr _tmp73A=term;struct _fat_ptr _tmp739=({const char*_tmp3AB=",";_tag_fat(_tmp3AB,sizeof(char),2U);});Cyc_PP_group(_tmp73B,_tmp73A,_tmp739,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_dp2doc,dps));});_tmp3A6[2]=_tmp73C;});Cyc_PP_cat(_tag_fat(_tmp3A6,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}}case 13: _tmp36F=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_Absyn_Enumfield*ef=_tmp36F;
_tmp36F=ef;goto _LL26;}case 14: _tmp36F=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_tmp36F;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp36E)->f3 == 0){_tmp36F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp36F;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_tmp36F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp36E)->f2;_tmp370=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp36E)->f3;_tmp371=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp36E)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_tmp36F;struct Cyc_List_List*ps=_tmp370;int dots=_tmp371;
# 1620
struct _fat_ptr term=dots?({const char*_tmp3B1=", ...)";_tag_fat(_tmp3B1,sizeof(char),7U);}):({const char*_tmp3B2=")";_tag_fat(_tmp3B2,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3AE[2];({struct Cyc_PP_Doc*_tmp746=Cyc_Absynpp_qvar2doc(tuf->name);_tmp3AE[0]=_tmp746;}),({struct Cyc_PP_Doc*_tmp745=({struct _fat_ptr _tmp744=({const char*_tmp3AF="(";_tag_fat(_tmp3AF,sizeof(char),2U);});struct _fat_ptr _tmp743=term;struct _fat_ptr _tmp742=({const char*_tmp3B0=",";_tag_fat(_tmp3B0,sizeof(char),2U);});Cyc_PP_egroup(_tmp744,_tmp743,_tmp742,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_pat2doc,ps));});_tmp3AE[1]=_tmp745;});Cyc_PP_cat(_tag_fat(_tmp3AE,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}}default: _tmp36F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp36E)->f1;{struct Cyc_Absyn_Exp*e=_tmp36F;
# 1624
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1626
return s;}
# 1629
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_tmp3B3[2];({struct Cyc_PP_Doc*_tmp74B=({struct _fat_ptr _tmp74A=({const char*_tmp3B4="";_tag_fat(_tmp3B4,sizeof(char),1U);});struct _fat_ptr _tmp749=({const char*_tmp3B5="=";_tag_fat(_tmp3B5,sizeof(char),2U);});struct _fat_ptr _tmp748=({const char*_tmp3B6="=";_tag_fat(_tmp3B6,sizeof(char),2U);});Cyc_PP_egroup(_tmp74A,_tmp749,_tmp748,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp3B3[0]=_tmp74B;}),({
struct Cyc_PP_Doc*_tmp747=Cyc_Absynpp_pat2doc((*dp).f2);_tmp3B3[1]=_tmp747;});Cyc_PP_cat(_tag_fat(_tmp3B3,sizeof(struct Cyc_PP_Doc*),2));});}
# 1634
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1636
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp3B7[2];({struct Cyc_PP_Doc*_tmp74E=Cyc_PP_text(({const char*_tmp3B8="default: ";_tag_fat(_tmp3B8,sizeof(char),10U);}));_tmp3B7[0]=_tmp74E;}),({
struct Cyc_PP_Doc*_tmp74D=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3B9[2];({struct Cyc_PP_Doc*_tmp74C=Cyc_PP_line_doc();_tmp3B9[0]=_tmp74C;}),_tmp3B9[1]=body;Cyc_PP_cat(_tag_fat(_tmp3B9,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3B7[1]=_tmp74D;});Cyc_PP_cat(_tag_fat(_tmp3B7,sizeof(struct Cyc_PP_Doc*),2));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp3BA[4];({struct Cyc_PP_Doc*_tmp753=Cyc_PP_text(({const char*_tmp3BB="case ";_tag_fat(_tmp3BB,sizeof(char),6U);}));_tmp3BA[0]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_Absynpp_pat2doc(c->pattern);_tmp3BA[1]=_tmp752;}),({
struct Cyc_PP_Doc*_tmp751=Cyc_PP_text(({const char*_tmp3BC=": ";_tag_fat(_tmp3BC,sizeof(char),3U);}));_tmp3BA[2]=_tmp751;}),({
struct Cyc_PP_Doc*_tmp750=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3BD[2];({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_line_doc();_tmp3BD[0]=_tmp74F;}),_tmp3BD[1]=body;Cyc_PP_cat(_tag_fat(_tmp3BD,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3BA[3]=_tmp750;});Cyc_PP_cat(_tag_fat(_tmp3BA,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1648
return({struct Cyc_PP_Doc*_tmp3BE[6];({struct Cyc_PP_Doc*_tmp75A=Cyc_PP_text(({const char*_tmp3BF="case ";_tag_fat(_tmp3BF,sizeof(char),6U);}));_tmp3BE[0]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_pat2doc(c->pattern);_tmp3BE[1]=_tmp759;}),({
struct Cyc_PP_Doc*_tmp758=Cyc_PP_text(({const char*_tmp3C0=" && ";_tag_fat(_tmp3C0,sizeof(char),5U);}));_tmp3BE[2]=_tmp758;}),({
struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_exp2doc(_check_null(c->where_clause));_tmp3BE[3]=_tmp757;}),({
struct Cyc_PP_Doc*_tmp756=Cyc_PP_text(({const char*_tmp3C1=": ";_tag_fat(_tmp3C1,sizeof(char),3U);}));_tmp3BE[4]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3C2[2];({struct Cyc_PP_Doc*_tmp754=Cyc_PP_line_doc();_tmp3C2[0]=_tmp754;}),_tmp3C2[1]=body;Cyc_PP_cat(_tag_fat(_tmp3C2,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3BE[5]=_tmp755;});Cyc_PP_cat(_tag_fat(_tmp3BE,sizeof(struct Cyc_PP_Doc*),6));});}}}
# 1656
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct Cyc_PP_Doc*(*_tmp75C)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp75B=({const char*_tmp3C3="";_tag_fat(_tmp3C3,sizeof(char),1U);});_tmp75C(Cyc_Absynpp_switchclause2doc,_tmp75B,cs);});}
# 1660
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1664
return({struct Cyc_PP_Doc*_tmp3C4[3];({struct Cyc_PP_Doc*_tmp75F=Cyc_Absynpp_qvar2doc(f->name);_tmp3C4[0]=_tmp75F;}),({struct Cyc_PP_Doc*_tmp75E=Cyc_PP_text(({const char*_tmp3C5=" = ";_tag_fat(_tmp3C5,sizeof(char),4U);}));_tmp3C4[1]=_tmp75E;}),({struct Cyc_PP_Doc*_tmp75D=Cyc_Absynpp_exp2doc(_check_null(f->tag));_tmp3C4[2]=_tmp75D;});Cyc_PP_cat(_tag_fat(_tmp3C4,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1667
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct Cyc_PP_Doc*(*_tmp761)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp760=({const char*_tmp3C6=",";_tag_fat(_tmp3C6,sizeof(char),2U);});_tmp761(Cyc_Absynpp_enumfield2doc,_tmp760,fs);});}
# 1671
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1675
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct Cyc_PP_Doc*(*_tmp763)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;});struct _fat_ptr _tmp762=({const char*_tmp3C7=",";_tag_fat(_tmp3C7,sizeof(char),2U);});_tmp763(Cyc_Absynpp_id2doc,_tmp762,vds);});}
# 1679
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp3C8=vd;void*_tmp3CF;void*_tmp3CE;void*_tmp3CD;struct Cyc_Absyn_Tqual _tmp3CC;unsigned _tmp3CB;void*_tmp3CA;enum Cyc_Absyn_Scope _tmp3C9;_tmp3C9=_tmp3C8->sc;_tmp3CA=_tmp3C8->name;_tmp3CB=_tmp3C8->varloc;_tmp3CC=_tmp3C8->tq;_tmp3CD=_tmp3C8->type;_tmp3CE=_tmp3C8->initializer;_tmp3CF=_tmp3C8->attributes;{enum Cyc_Absyn_Scope sc=_tmp3C9;struct _tuple0*name=_tmp3CA;unsigned varloc=_tmp3CB;struct Cyc_Absyn_Tqual tq=_tmp3CC;void*type=_tmp3CD;struct Cyc_Absyn_Exp*initializer=_tmp3CE;struct Cyc_List_List*atts=_tmp3CF;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp3D0=Cyc_Flags_c_compiler;if(_tmp3D0 == Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL3;}else{
# 1688
{void*_tmp3D1=Cyc_Absyn_compress(type);void*_stmttmp1E=_tmp3D1;void*_tmp3D2=_stmttmp1E;void*_tmp3D3;if(*((int*)_tmp3D2)== 5){_tmp3D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3D2)->f1).attributes;{struct Cyc_List_List*atts2=_tmp3D3;
# 1690
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{
beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1694
goto _LL3;}_LL3:;}{
# 1697
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Flags_C_Compilers _tmp3D4=Cyc_Flags_c_compiler;if(_tmp3D4 == Cyc_Flags_Gcc_c){
tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{
tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1702
s=({struct Cyc_PP_Doc*_tmp3D5[5];_tmp3D5[0]=tmp_doc,({
# 1704
struct Cyc_PP_Doc*_tmp76D=Cyc_Absynpp_scope2doc(sc);_tmp3D5[1]=_tmp76D;}),({
struct Cyc_PP_Doc*_tmp76C=({struct Cyc_Absyn_Tqual _tmp76B=tq;void*_tmp76A=type;Cyc_Absynpp_tqtd2doc(_tmp76B,_tmp76A,({struct Cyc_Core_Opt*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct Cyc_PP_Doc*_tmp769=({struct Cyc_PP_Doc*_tmp3D6[2];_tmp3D6[0]=beforenamedoc,_tmp3D6[1]=sn;Cyc_PP_cat(_tag_fat(_tmp3D6,sizeof(struct Cyc_PP_Doc*),2));});_tmp3D7->v=_tmp769;});_tmp3D7;}));});_tmp3D5[2]=_tmp76C;}),
initializer == 0?({
struct Cyc_PP_Doc*_tmp768=Cyc_PP_nil_doc();_tmp3D5[3]=_tmp768;}):({
struct Cyc_PP_Doc*_tmp767=({struct Cyc_PP_Doc*_tmp3D8[2];({struct Cyc_PP_Doc*_tmp766=Cyc_PP_text(({const char*_tmp3D9=" = ";_tag_fat(_tmp3D9,sizeof(char),4U);}));_tmp3D8[0]=_tmp766;}),({struct Cyc_PP_Doc*_tmp765=Cyc_Absynpp_exp2doc(initializer);_tmp3D8[1]=_tmp765;});Cyc_PP_cat(_tag_fat(_tmp3D8,sizeof(struct Cyc_PP_Doc*),2));});_tmp3D5[3]=_tmp767;}),({
struct Cyc_PP_Doc*_tmp764=Cyc_PP_text(({const char*_tmp3DA=";";_tag_fat(_tmp3DA,sizeof(char),2U);}));_tmp3D5[4]=_tmp764;});Cyc_PP_cat(_tag_fat(_tmp3D5,sizeof(struct Cyc_PP_Doc*),5));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1713
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _tmp3DB=*x;struct _tuple20 _stmttmp1F=_tmp3DB;struct _tuple20 _tmp3DC=_stmttmp1F;void*_tmp3DD;_tmp3DD=_tmp3DC.f2;{struct _tuple0*v=_tmp3DD;
return Cyc_Absynpp_qvar2doc(v);}}
# 1718
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp3DE[4];({struct Cyc_PP_Doc*_tmp771=Cyc_Absynpp_scope2doc(ad->sc);_tmp3DE[0]=_tmp771;}),({
struct Cyc_PP_Doc*_tmp770=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp3DE[1]=_tmp770;}),({
struct Cyc_PP_Doc*_tmp76F=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp3DE[2]=_tmp76F;}),({
struct Cyc_PP_Doc*_tmp76E=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp3DE[3]=_tmp76E;});Cyc_PP_cat(_tag_fat(_tmp3DE,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1725
return({struct Cyc_PP_Doc*_tmp3DF[12];({struct Cyc_PP_Doc*_tmp782=Cyc_Absynpp_scope2doc(ad->sc);_tmp3DF[0]=_tmp782;}),({
struct Cyc_PP_Doc*_tmp781=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp3DF[1]=_tmp781;}),({
struct Cyc_PP_Doc*_tmp780=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp3DF[2]=_tmp780;}),({
struct Cyc_PP_Doc*_tmp77F=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp3DF[3]=_tmp77F;}),({
struct Cyc_PP_Doc*_tmp77E=Cyc_PP_blank_doc();_tmp3DF[4]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_Absynpp_lb();_tmp3DF[5]=_tmp77D;}),({
struct Cyc_PP_Doc*_tmp77C=Cyc_Absynpp_ktvars2doc((_check_null(ad->impl))->exist_vars);_tmp3DF[6]=_tmp77C;}),
(_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp77B=Cyc_PP_nil_doc();_tmp3DF[7]=_tmp77B;}):({
struct Cyc_PP_Doc*_tmp77A=({struct Cyc_PP_Doc*_tmp3E0[2];({struct Cyc_PP_Doc*_tmp779=Cyc_PP_text(({const char*_tmp3E1=":";_tag_fat(_tmp3E1,sizeof(char),2U);}));_tmp3E0[0]=_tmp779;}),({struct Cyc_PP_Doc*_tmp778=Cyc_Absynpp_rgnpo2doc((_check_null(ad->impl))->rgn_po);_tmp3E0[1]=_tmp778;});Cyc_PP_cat(_tag_fat(_tmp3E0,sizeof(struct Cyc_PP_Doc*),2));});_tmp3DF[7]=_tmp77A;}),({
struct Cyc_PP_Doc*_tmp777=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3E2[2];({struct Cyc_PP_Doc*_tmp776=Cyc_PP_line_doc();_tmp3E2[0]=_tmp776;}),({struct Cyc_PP_Doc*_tmp775=Cyc_Absynpp_aggrfields2doc((_check_null(ad->impl))->fields);_tmp3E2[1]=_tmp775;});Cyc_PP_cat(_tag_fat(_tmp3E2,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3DF[8]=_tmp777;}),({
struct Cyc_PP_Doc*_tmp774=Cyc_PP_line_doc();_tmp3DF[9]=_tmp774;}),({
struct Cyc_PP_Doc*_tmp773=Cyc_Absynpp_rb();_tmp3DF[10]=_tmp773;}),({
struct Cyc_PP_Doc*_tmp772=Cyc_Absynpp_atts2doc(ad->attributes);_tmp3DF[11]=_tmp772;});Cyc_PP_cat(_tag_fat(_tmp3DF,sizeof(struct Cyc_PP_Doc*),12));});}}
# 1739
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp3E3=dd;int _tmp3E8;void*_tmp3E7;void*_tmp3E6;void*_tmp3E5;enum Cyc_Absyn_Scope _tmp3E4;_tmp3E4=_tmp3E3->sc;_tmp3E5=_tmp3E3->name;_tmp3E6=_tmp3E3->tvs;_tmp3E7=_tmp3E3->fields;_tmp3E8=_tmp3E3->is_extensible;{enum Cyc_Absyn_Scope sc=_tmp3E4;struct _tuple0*name=_tmp3E5;struct Cyc_List_List*tvs=_tmp3E6;struct Cyc_Core_Opt*fields=_tmp3E7;int is_x=_tmp3E8;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp3E9[5];({struct Cyc_PP_Doc*_tmp789=Cyc_Absynpp_scope2doc(sc);_tmp3E9[0]=_tmp789;}),
is_x?({struct Cyc_PP_Doc*_tmp788=Cyc_PP_text(({const char*_tmp3EA="@extensible ";_tag_fat(_tmp3EA,sizeof(char),13U);}));_tmp3E9[1]=_tmp788;}):({struct Cyc_PP_Doc*_tmp787=Cyc_PP_blank_doc();_tmp3E9[1]=_tmp787;}),({
struct Cyc_PP_Doc*_tmp786=Cyc_PP_text(({const char*_tmp3EB="datatype ";_tag_fat(_tmp3EB,sizeof(char),10U);}));_tmp3E9[2]=_tmp786;}),
is_x?({struct Cyc_PP_Doc*_tmp785=Cyc_Absynpp_qvar2bolddoc(name);_tmp3E9[3]=_tmp785;}):({struct Cyc_PP_Doc*_tmp784=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp3E9[3]=_tmp784;}),({
struct Cyc_PP_Doc*_tmp783=Cyc_Absynpp_ktvars2doc(tvs);_tmp3E9[4]=_tmp783;});Cyc_PP_cat(_tag_fat(_tmp3E9,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1748
return({struct Cyc_PP_Doc*_tmp3EC[10];({struct Cyc_PP_Doc*_tmp797=Cyc_Absynpp_scope2doc(sc);_tmp3EC[0]=_tmp797;}),
is_x?({struct Cyc_PP_Doc*_tmp796=Cyc_PP_text(({const char*_tmp3ED="@extensible ";_tag_fat(_tmp3ED,sizeof(char),13U);}));_tmp3EC[1]=_tmp796;}):({struct Cyc_PP_Doc*_tmp795=Cyc_PP_blank_doc();_tmp3EC[1]=_tmp795;}),({
struct Cyc_PP_Doc*_tmp794=Cyc_PP_text(({const char*_tmp3EE="datatype ";_tag_fat(_tmp3EE,sizeof(char),10U);}));_tmp3EC[2]=_tmp794;}),
is_x?({struct Cyc_PP_Doc*_tmp793=Cyc_Absynpp_qvar2bolddoc(name);_tmp3EC[3]=_tmp793;}):({struct Cyc_PP_Doc*_tmp792=Cyc_Absynpp_typedef_name2bolddoc(name);_tmp3EC[3]=_tmp792;}),({
struct Cyc_PP_Doc*_tmp791=Cyc_Absynpp_ktvars2doc(tvs);_tmp3EC[4]=_tmp791;}),({
struct Cyc_PP_Doc*_tmp790=Cyc_PP_blank_doc();_tmp3EC[5]=_tmp790;}),({struct Cyc_PP_Doc*_tmp78F=Cyc_Absynpp_lb();_tmp3EC[6]=_tmp78F;}),({
struct Cyc_PP_Doc*_tmp78E=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3EF[2];({struct Cyc_PP_Doc*_tmp78D=Cyc_PP_line_doc();_tmp3EF[0]=_tmp78D;}),({struct Cyc_PP_Doc*_tmp78C=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_tmp3EF[1]=_tmp78C;});Cyc_PP_cat(_tag_fat(_tmp3EF,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3EC[7]=_tmp78E;}),({
struct Cyc_PP_Doc*_tmp78B=Cyc_PP_line_doc();_tmp3EC[8]=_tmp78B;}),({
struct Cyc_PP_Doc*_tmp78A=Cyc_Absynpp_rb();_tmp3EC[9]=_tmp78A;});Cyc_PP_cat(_tag_fat(_tmp3EC,sizeof(struct Cyc_PP_Doc*),10));});}}}
# 1759
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp3F0=ed;void*_tmp3F3;void*_tmp3F2;enum Cyc_Absyn_Scope _tmp3F1;_tmp3F1=_tmp3F0->sc;_tmp3F2=_tmp3F0->name;_tmp3F3=_tmp3F0->fields;{enum Cyc_Absyn_Scope sc=_tmp3F1;struct _tuple0*n=_tmp3F2;struct Cyc_Core_Opt*fields=_tmp3F3;
if(fields == 0)
return({struct Cyc_PP_Doc*_tmp3F4[3];({struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_scope2doc(sc);_tmp3F4[0]=_tmp79A;}),({
struct Cyc_PP_Doc*_tmp799=Cyc_PP_text(({const char*_tmp3F5="enum ";_tag_fat(_tmp3F5,sizeof(char),6U);}));_tmp3F4[1]=_tmp799;}),({
struct Cyc_PP_Doc*_tmp798=Cyc_Absynpp_typedef_name2bolddoc(n);_tmp3F4[2]=_tmp798;});Cyc_PP_cat(_tag_fat(_tmp3F4,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1767
return({struct Cyc_PP_Doc*_tmp3F6[8];({struct Cyc_PP_Doc*_tmp7A4=Cyc_Absynpp_scope2doc(sc);_tmp3F6[0]=_tmp7A4;}),({
struct Cyc_PP_Doc*_tmp7A3=Cyc_PP_text(({const char*_tmp3F7="enum ";_tag_fat(_tmp3F7,sizeof(char),6U);}));_tmp3F6[1]=_tmp7A3;}),({
struct Cyc_PP_Doc*_tmp7A2=Cyc_Absynpp_qvar2bolddoc(n);_tmp3F6[2]=_tmp7A2;}),({
struct Cyc_PP_Doc*_tmp7A1=Cyc_PP_blank_doc();_tmp3F6[3]=_tmp7A1;}),({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_lb();_tmp3F6[4]=_tmp7A0;}),({
struct Cyc_PP_Doc*_tmp79F=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3F8[2];({struct Cyc_PP_Doc*_tmp79E=Cyc_PP_line_doc();_tmp3F8[0]=_tmp79E;}),({struct Cyc_PP_Doc*_tmp79D=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_tmp3F8[1]=_tmp79D;});Cyc_PP_cat(_tag_fat(_tmp3F8,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3F6[5]=_tmp79F;}),({
struct Cyc_PP_Doc*_tmp79C=Cyc_PP_line_doc();_tmp3F6[6]=_tmp79C;}),({
struct Cyc_PP_Doc*_tmp79B=Cyc_Absynpp_rb();_tmp3F6[7]=_tmp79B;});Cyc_PP_cat(_tag_fat(_tmp3F6,sizeof(struct Cyc_PP_Doc*),8));});}}}
# 1776
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp3F9=d->r;void*_stmttmp20=_tmp3F9;void*_tmp3FA=_stmttmp20;void*_tmp3FE;void*_tmp3FD;void*_tmp3FC;void*_tmp3FB;switch(*((int*)_tmp3FA)){case 1: _tmp3FB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp3FB;
# 1780
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->tag=5,_tmp40F->f1=type_info;_tmp40F;});
if(fd->cached_type != 0){
void*_tmp3FF=Cyc_Absyn_compress(_check_null(fd->cached_type));void*_stmttmp21=_tmp3FF;void*_tmp400=_stmttmp21;struct Cyc_Absyn_FnInfo _tmp401;if(*((int*)_tmp400)== 5){_tmp401=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp400)->f1;{struct Cyc_Absyn_FnInfo i=_tmp401;
# 1786
({struct Cyc_List_List*_tmp7A5=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp7A5;});goto _LL23;}}else{
({void*_tmp402=0U;({int(*_tmp7A7)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp7A6=({const char*_tmp403="function has non-function type";_tag_fat(_tmp403,sizeof(char),31U);});_tmp7A7(_tmp7A6,_tag_fat(_tmp402,sizeof(void*),0));});});}_LL23:;}{
# 1789
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _tmp404=Cyc_Flags_c_compiler;if(_tmp404 == Cyc_Flags_Gcc_c){
inlinedoc=Cyc_PP_text(({const char*_tmp405="inline ";_tag_fat(_tmp405,sizeof(char),8U);}));goto _LL28;}else{
inlinedoc=Cyc_PP_text(({const char*_tmp406="__inline ";_tag_fat(_tmp406,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1797
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _tmp407=Cyc_Flags_c_compiler;if(_tmp407 == Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL2D;}else{
beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1804
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp7AA=Cyc_Absyn_empty_tqual(0U);void*_tmp7A9=t;Cyc_Absynpp_tqtd2doc(_tmp7AA,_tmp7A9,({struct Cyc_Core_Opt*_tmp40E=_cycalloc(sizeof(*_tmp40E));({
struct Cyc_PP_Doc*_tmp7A8=({struct Cyc_PP_Doc*_tmp40D[2];_tmp40D[0]=beforenamedoc,_tmp40D[1]=namedoc;Cyc_PP_cat(_tag_fat(_tmp40D,sizeof(struct Cyc_PP_Doc*),2));});_tmp40E->v=_tmp7A8;});_tmp40E;}));});
# 1812
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp40B[5];({struct Cyc_PP_Doc*_tmp7B1=Cyc_PP_blank_doc();_tmp40B[0]=_tmp7B1;}),({struct Cyc_PP_Doc*_tmp7B0=Cyc_Absynpp_lb();_tmp40B[1]=_tmp7B0;}),({
struct Cyc_PP_Doc*_tmp7AF=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp40C[2];({struct Cyc_PP_Doc*_tmp7AE=Cyc_PP_line_doc();_tmp40C[0]=_tmp7AE;}),({struct Cyc_PP_Doc*_tmp7AD=Cyc_Absynpp_stmt2doc(fd->body,0,0);_tmp40C[1]=_tmp7AD;});Cyc_PP_cat(_tag_fat(_tmp40C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp40B[2]=_tmp7AF;}),({
struct Cyc_PP_Doc*_tmp7AC=Cyc_PP_line_doc();_tmp40B[3]=_tmp7AC;}),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_Absynpp_rb();_tmp40B[4]=_tmp7AB;});Cyc_PP_cat(_tag_fat(_tmp40B,sizeof(struct Cyc_PP_Doc*),5));});
s=({struct Cyc_PP_Doc*_tmp408[4];_tmp408[0]=inlinedoc,_tmp408[1]=scopedoc,_tmp408[2]=tqtddoc,_tmp408[3]=bodydoc;Cyc_PP_cat(_tag_fat(_tmp408,sizeof(struct Cyc_PP_Doc*),4));});
# 1818
{enum Cyc_Flags_C_Compilers _tmp409=Cyc_Flags_c_compiler;if(_tmp409 == Cyc_Flags_Vc_c){
s=({struct Cyc_PP_Doc*_tmp40A[2];_tmp40A[0]=attsdoc,_tmp40A[1]=s;Cyc_PP_cat(_tag_fat(_tmp40A,sizeof(struct Cyc_PP_Doc*),2));});goto _LL32;}else{
goto _LL32;}_LL32:;}
# 1823
goto _LL0;}}}}}case 5: _tmp3FB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp3FB;
# 1826
s=({struct Cyc_PP_Doc*_tmp410[2];({struct Cyc_PP_Doc*_tmp7B3=Cyc_Absynpp_aggrdecl2doc(ad);_tmp410[0]=_tmp7B3;}),({struct Cyc_PP_Doc*_tmp7B2=Cyc_PP_text(({const char*_tmp411=";";_tag_fat(_tmp411,sizeof(char),2U);}));_tmp410[1]=_tmp7B2;});Cyc_PP_cat(_tag_fat(_tmp410,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 0: _tmp3FB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp3FB;
# 1829
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4: _tmp3FB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;_tmp3FC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp3FA)->f2;_tmp3FD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp3FA)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp3FB;struct Cyc_Absyn_Vardecl*vd=_tmp3FC;struct Cyc_Absyn_Exp*open_exp_opt=_tmp3FD;
# 1832
s=({struct Cyc_PP_Doc*_tmp412[7];({struct Cyc_PP_Doc*_tmp7BE=Cyc_PP_text(({const char*_tmp413="region";_tag_fat(_tmp413,sizeof(char),7U);}));_tmp412[0]=_tmp7BE;}),({
struct Cyc_PP_Doc*_tmp7BD=Cyc_PP_text(({const char*_tmp414="<";_tag_fat(_tmp414,sizeof(char),2U);}));_tmp412[1]=_tmp7BD;}),({
struct Cyc_PP_Doc*_tmp7BC=Cyc_Absynpp_tvar2doc(tv);_tmp412[2]=_tmp7BC;}),({
struct Cyc_PP_Doc*_tmp7BB=Cyc_PP_text(({const char*_tmp415=">";_tag_fat(_tmp415,sizeof(char),2U);}));_tmp412[3]=_tmp7BB;}),({
struct Cyc_PP_Doc*_tmp7BA=Cyc_Absynpp_qvar2doc(vd->name);_tmp412[4]=_tmp7BA;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_tmp7B9=({struct Cyc_PP_Doc*_tmp416[3];({struct Cyc_PP_Doc*_tmp7B8=Cyc_PP_text(({const char*_tmp417=" = open(";_tag_fat(_tmp417,sizeof(char),9U);}));_tmp416[0]=_tmp7B8;}),({struct Cyc_PP_Doc*_tmp7B7=Cyc_Absynpp_exp2doc(open_exp_opt);_tmp416[1]=_tmp7B7;}),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_PP_text(({const char*_tmp418=")";_tag_fat(_tmp418,sizeof(char),2U);}));_tmp416[2]=_tmp7B6;});Cyc_PP_cat(_tag_fat(_tmp416,sizeof(struct Cyc_PP_Doc*),3));});
# 1837
_tmp412[5]=_tmp7B9;}):({
struct Cyc_PP_Doc*_tmp7B5=Cyc_PP_nil_doc();_tmp412[5]=_tmp7B5;}),({
struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_text(({const char*_tmp419=";";_tag_fat(_tmp419,sizeof(char),2U);}));_tmp412[6]=_tmp7B4;});Cyc_PP_cat(_tag_fat(_tmp412,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 6: _tmp3FB=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp3FB;
# 1842
s=({struct Cyc_PP_Doc*_tmp41A[2];({struct Cyc_PP_Doc*_tmp7C0=Cyc_Absynpp_datatypedecl2doc(dd);_tmp41A[0]=_tmp7C0;}),({struct Cyc_PP_Doc*_tmp7BF=Cyc_PP_text(({const char*_tmp41B=";";_tag_fat(_tmp41B,sizeof(char),2U);}));_tmp41A[1]=_tmp7BF;});Cyc_PP_cat(_tag_fat(_tmp41A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 2: _tmp3FB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;_tmp3FC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp3FA)->f3;{struct Cyc_Absyn_Pat*p=_tmp3FB;struct Cyc_Absyn_Exp*e=_tmp3FC;
# 1845
s=({struct Cyc_PP_Doc*_tmp41C[5];({struct Cyc_PP_Doc*_tmp7C5=Cyc_PP_text(({const char*_tmp41D="let ";_tag_fat(_tmp41D,sizeof(char),5U);}));_tmp41C[0]=_tmp7C5;}),({
struct Cyc_PP_Doc*_tmp7C4=Cyc_Absynpp_pat2doc(p);_tmp41C[1]=_tmp7C4;}),({
struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_text(({const char*_tmp41E=" = ";_tag_fat(_tmp41E,sizeof(char),4U);}));_tmp41C[2]=_tmp7C3;}),({
struct Cyc_PP_Doc*_tmp7C2=Cyc_Absynpp_exp2doc(e);_tmp41C[3]=_tmp7C2;}),({
struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_text(({const char*_tmp41F=";";_tag_fat(_tmp41F,sizeof(char),2U);}));_tmp41C[4]=_tmp7C1;});Cyc_PP_cat(_tag_fat(_tmp41C,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 3: _tmp3FB=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_List_List*vds=_tmp3FB;
# 1852
s=({struct Cyc_PP_Doc*_tmp420[3];({struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_text(({const char*_tmp421="let ";_tag_fat(_tmp421,sizeof(char),5U);}));_tmp420[0]=_tmp7C8;}),({struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_ids2doc(vds);_tmp420[1]=_tmp7C7;}),({struct Cyc_PP_Doc*_tmp7C6=Cyc_PP_text(({const char*_tmp422=";";_tag_fat(_tmp422,sizeof(char),2U);}));_tmp420[2]=_tmp7C6;});Cyc_PP_cat(_tag_fat(_tmp420,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 7: _tmp3FB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp3FB;
# 1855
s=({struct Cyc_PP_Doc*_tmp423[2];({struct Cyc_PP_Doc*_tmp7CA=Cyc_Absynpp_enumdecl2doc(ed);_tmp423[0]=_tmp7CA;}),({struct Cyc_PP_Doc*_tmp7C9=Cyc_PP_text(({const char*_tmp424=";";_tag_fat(_tmp424,sizeof(char),2U);}));_tmp423[1]=_tmp7C9;});Cyc_PP_cat(_tag_fat(_tmp423,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 8: _tmp3FB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp3FB;
# 1858
void*t;
if(td->defn != 0)
t=_check_null(td->defn);else{
# 1862
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_tmp425[4];({struct Cyc_PP_Doc*_tmp7D3=Cyc_PP_text(({const char*_tmp426="typedef ";_tag_fat(_tmp426,sizeof(char),9U);}));_tmp425[0]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D2=({struct Cyc_Absyn_Tqual _tmp7D1=td->tq;void*_tmp7D0=t;Cyc_Absynpp_tqtd2doc(_tmp7D1,_tmp7D0,({struct Cyc_Core_Opt*_tmp428=_cycalloc(sizeof(*_tmp428));({
# 1866
struct Cyc_PP_Doc*_tmp7CF=({struct Cyc_PP_Doc*_tmp427[2];({struct Cyc_PP_Doc*_tmp7CE=Cyc_Absynpp_typedef_name2bolddoc(td->name);_tmp427[0]=_tmp7CE;}),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_Absynpp_tvars2doc(td->tvs);_tmp427[1]=_tmp7CD;});Cyc_PP_cat(_tag_fat(_tmp427,sizeof(struct Cyc_PP_Doc*),2));});
# 1866
_tmp428->v=_tmp7CF;});_tmp428;}));});
# 1864
_tmp425[1]=_tmp7D2;}),({
# 1869
struct Cyc_PP_Doc*_tmp7CC=Cyc_Absynpp_atts2doc(td->atts);_tmp425[2]=_tmp7CC;}),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_PP_text(({const char*_tmp429=";";_tag_fat(_tmp429,sizeof(char),2U);}));_tmp425[3]=_tmp7CB;});Cyc_PP_cat(_tag_fat(_tmp425,sizeof(struct Cyc_PP_Doc*),4));});
# 1872
goto _LL0;}case 9: _tmp3FB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;_tmp3FC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp3FA)->f2;{struct _fat_ptr*v=_tmp3FB;struct Cyc_List_List*tdl=_tmp3FC;
# 1874
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_tmp42A[8];({struct Cyc_PP_Doc*_tmp7DD=Cyc_PP_text(({const char*_tmp42B="namespace ";_tag_fat(_tmp42B,sizeof(char),11U);}));_tmp42A[0]=_tmp7DD;}),({
struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_textptr(v);_tmp42A[1]=_tmp7DC;}),({
struct Cyc_PP_Doc*_tmp7DB=Cyc_PP_blank_doc();_tmp42A[2]=_tmp7DB;}),({struct Cyc_PP_Doc*_tmp7DA=Cyc_Absynpp_lb();_tmp42A[3]=_tmp7DA;}),({
struct Cyc_PP_Doc*_tmp7D9=Cyc_PP_line_doc();_tmp42A[4]=_tmp7D9;}),({
struct Cyc_PP_Doc*_tmp7D8=({struct Cyc_PP_Doc*(*_tmp7D7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp7D6=({const char*_tmp42C="";_tag_fat(_tmp42C,sizeof(char),1U);});_tmp7D7(Cyc_Absynpp_decl2doc,_tmp7D6,tdl);});_tmp42A[5]=_tmp7D8;}),({
struct Cyc_PP_Doc*_tmp7D5=Cyc_PP_line_doc();_tmp42A[6]=_tmp7D5;}),({
struct Cyc_PP_Doc*_tmp7D4=Cyc_Absynpp_rb();_tmp42A[7]=_tmp7D4;});Cyc_PP_cat(_tag_fat(_tmp42A,sizeof(struct Cyc_PP_Doc*),8));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10: _tmp3FB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;_tmp3FC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp3FA)->f2;{struct _tuple0*q=_tmp3FB;struct Cyc_List_List*tdl=_tmp3FC;
# 1885
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp42D[8];({struct Cyc_PP_Doc*_tmp7E7=Cyc_PP_text(({const char*_tmp42E="using ";_tag_fat(_tmp42E,sizeof(char),7U);}));_tmp42D[0]=_tmp7E7;}),({
struct Cyc_PP_Doc*_tmp7E6=Cyc_Absynpp_qvar2doc(q);_tmp42D[1]=_tmp7E6;}),({
struct Cyc_PP_Doc*_tmp7E5=Cyc_PP_blank_doc();_tmp42D[2]=_tmp7E5;}),({struct Cyc_PP_Doc*_tmp7E4=Cyc_Absynpp_lb();_tmp42D[3]=_tmp7E4;}),({
struct Cyc_PP_Doc*_tmp7E3=Cyc_PP_line_doc();_tmp42D[4]=_tmp7E3;}),({
struct Cyc_PP_Doc*_tmp7E2=({struct Cyc_PP_Doc*(*_tmp7E1)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp7E0=({const char*_tmp42F="";_tag_fat(_tmp42F,sizeof(char),1U);});_tmp7E1(Cyc_Absynpp_decl2doc,_tmp7E0,tdl);});_tmp42D[5]=_tmp7E2;}),({
struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_line_doc();_tmp42D[6]=_tmp7DF;}),({
struct Cyc_PP_Doc*_tmp7DE=Cyc_Absynpp_rb();_tmp42D[7]=_tmp7DE;});Cyc_PP_cat(_tag_fat(_tmp42D,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1894
s=({struct Cyc_PP_Doc*_tmp430[11];({struct Cyc_PP_Doc*_tmp7F4=Cyc_PP_text(({const char*_tmp431="/* using ";_tag_fat(_tmp431,sizeof(char),10U);}));_tmp430[0]=_tmp7F4;}),({
struct Cyc_PP_Doc*_tmp7F3=Cyc_Absynpp_qvar2doc(q);_tmp430[1]=_tmp7F3;}),({
struct Cyc_PP_Doc*_tmp7F2=Cyc_PP_blank_doc();_tmp430[2]=_tmp7F2;}),({
struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_lb();_tmp430[3]=_tmp7F1;}),({
struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp432=" */";_tag_fat(_tmp432,sizeof(char),4U);}));_tmp430[4]=_tmp7F0;}),({
struct Cyc_PP_Doc*_tmp7EF=Cyc_PP_line_doc();_tmp430[5]=_tmp7EF;}),({
struct Cyc_PP_Doc*_tmp7EE=({struct Cyc_PP_Doc*(*_tmp7ED)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp7EC=({const char*_tmp433="";_tag_fat(_tmp433,sizeof(char),1U);});_tmp7ED(Cyc_Absynpp_decl2doc,_tmp7EC,tdl);});_tmp430[6]=_tmp7EE;}),({
struct Cyc_PP_Doc*_tmp7EB=Cyc_PP_line_doc();_tmp430[7]=_tmp7EB;}),({
struct Cyc_PP_Doc*_tmp7EA=Cyc_PP_text(({const char*_tmp434="/* ";_tag_fat(_tmp434,sizeof(char),4U);}));_tmp430[8]=_tmp7EA;}),({
struct Cyc_PP_Doc*_tmp7E9=Cyc_Absynpp_rb();_tmp430[9]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=Cyc_PP_text(({const char*_tmp435=" */";_tag_fat(_tmp435,sizeof(char),4U);}));_tmp430[10]=_tmp7E8;});Cyc_PP_cat(_tag_fat(_tmp430,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL0;}case 11: _tmp3FB=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;{struct Cyc_List_List*tdl=_tmp3FB;
# 1907
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp436[6];({struct Cyc_PP_Doc*_tmp7FC=Cyc_PP_text(({const char*_tmp437="extern \"C\" ";_tag_fat(_tmp437,sizeof(char),12U);}));_tmp436[0]=_tmp7FC;}),({
struct Cyc_PP_Doc*_tmp7FB=Cyc_Absynpp_lb();_tmp436[1]=_tmp7FB;}),({
struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_line_doc();_tmp436[2]=_tmp7FA;}),({
struct Cyc_PP_Doc*_tmp7F9=({struct Cyc_PP_Doc*(*_tmp7F8)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp7F7=({const char*_tmp438="";_tag_fat(_tmp438,sizeof(char),1U);});_tmp7F8(Cyc_Absynpp_decl2doc,_tmp7F7,tdl);});_tmp436[3]=_tmp7F9;}),({
struct Cyc_PP_Doc*_tmp7F6=Cyc_PP_line_doc();_tmp436[4]=_tmp7F6;}),({
struct Cyc_PP_Doc*_tmp7F5=Cyc_Absynpp_rb();_tmp436[5]=_tmp7F5;});Cyc_PP_cat(_tag_fat(_tmp436,sizeof(struct Cyc_PP_Doc*),6));});else{
# 1915
s=({struct Cyc_PP_Doc*_tmp439[9];({struct Cyc_PP_Doc*_tmp807=Cyc_PP_text(({const char*_tmp43A="/* extern \"C\" ";_tag_fat(_tmp43A,sizeof(char),15U);}));_tmp439[0]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp806=Cyc_Absynpp_lb();_tmp439[1]=_tmp806;}),({
struct Cyc_PP_Doc*_tmp805=Cyc_PP_text(({const char*_tmp43B=" */";_tag_fat(_tmp43B,sizeof(char),4U);}));_tmp439[2]=_tmp805;}),({
struct Cyc_PP_Doc*_tmp804=Cyc_PP_line_doc();_tmp439[3]=_tmp804;}),({
struct Cyc_PP_Doc*_tmp803=({struct Cyc_PP_Doc*(*_tmp802)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp801=({const char*_tmp43C="";_tag_fat(_tmp43C,sizeof(char),1U);});_tmp802(Cyc_Absynpp_decl2doc,_tmp801,tdl);});_tmp439[4]=_tmp803;}),({
struct Cyc_PP_Doc*_tmp800=Cyc_PP_line_doc();_tmp439[5]=_tmp800;}),({
struct Cyc_PP_Doc*_tmp7FF=Cyc_PP_text(({const char*_tmp43D="/* ";_tag_fat(_tmp43D,sizeof(char),4U);}));_tmp439[6]=_tmp7FF;}),({
struct Cyc_PP_Doc*_tmp7FE=Cyc_Absynpp_rb();_tmp439[7]=_tmp7FE;}),({
struct Cyc_PP_Doc*_tmp7FD=Cyc_PP_text(({const char*_tmp43E=" */";_tag_fat(_tmp43E,sizeof(char),4U);}));_tmp439[8]=_tmp7FD;});Cyc_PP_cat(_tag_fat(_tmp439,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 12: _tmp3FB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp3FA)->f1;_tmp3FC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp3FA)->f2;_tmp3FD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp3FA)->f3;_tmp3FE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp3FA)->f4;{struct Cyc_List_List*tdl=_tmp3FB;struct Cyc_List_List*ovrs=_tmp3FC;struct Cyc_List_List*exs=_tmp3FD;struct _tuple10*wc=_tmp3FE;
# 1926
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp43F[7];({struct Cyc_PP_Doc*_tmp810=Cyc_Absynpp_rb();_tmp43F[0]=_tmp810;}),({struct Cyc_PP_Doc*_tmp80F=Cyc_PP_text(({const char*_tmp440=" export ";_tag_fat(_tmp440,sizeof(char),9U);}));_tmp43F[1]=_tmp80F;}),({struct Cyc_PP_Doc*_tmp80E=Cyc_Absynpp_lb();_tmp43F[2]=_tmp80E;}),({
struct Cyc_PP_Doc*_tmp80D=Cyc_PP_line_doc();_tmp43F[3]=_tmp80D;}),({struct Cyc_PP_Doc*_tmp80C=({struct Cyc_PP_Doc*(*_tmp80B)(struct Cyc_PP_Doc*(*)(struct _tuple20*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct _tuple20*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp80A=({const char*_tmp441=",";_tag_fat(_tmp441,sizeof(char),2U);});_tmp80B(Cyc_Absynpp_export2doc,_tmp80A,exs);});_tmp43F[4]=_tmp80C;}),({
struct Cyc_PP_Doc*_tmp809=Cyc_PP_line_doc();_tmp43F[5]=_tmp809;}),({struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_rb();_tmp43F[6]=_tmp808;});Cyc_PP_cat(_tag_fat(_tmp43F,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1934
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp442[7];({struct Cyc_PP_Doc*_tmp819=Cyc_Absynpp_rb();_tmp442[0]=_tmp819;}),({struct Cyc_PP_Doc*_tmp818=Cyc_PP_text(({const char*_tmp443=" cycdef ";_tag_fat(_tmp443,sizeof(char),9U);}));_tmp442[1]=_tmp818;}),({struct Cyc_PP_Doc*_tmp817=Cyc_Absynpp_lb();_tmp442[2]=_tmp817;}),({
struct Cyc_PP_Doc*_tmp816=Cyc_PP_line_doc();_tmp442[3]=_tmp816;}),({struct Cyc_PP_Doc*_tmp815=({struct Cyc_PP_Doc*(*_tmp814)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp813=({const char*_tmp444="";_tag_fat(_tmp444,sizeof(char),1U);});_tmp814(Cyc_Absynpp_decl2doc,_tmp813,ovrs);});_tmp442[4]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp812=Cyc_PP_line_doc();_tmp442[5]=_tmp812;}),({struct Cyc_PP_Doc*_tmp811=Cyc_Absynpp_rb();_tmp442[6]=_tmp811;});Cyc_PP_cat(_tag_fat(_tmp442,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1940
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp445[6];({struct Cyc_PP_Doc*_tmp820=Cyc_PP_text(({const char*_tmp446="extern \"C include\" ";_tag_fat(_tmp446,sizeof(char),20U);}));_tmp445[0]=_tmp820;}),({
struct Cyc_PP_Doc*_tmp81F=Cyc_Absynpp_lb();_tmp445[1]=_tmp81F;}),({
struct Cyc_PP_Doc*_tmp81E=Cyc_PP_line_doc();_tmp445[2]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=({struct Cyc_PP_Doc*(*_tmp81C)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp81B=({const char*_tmp447="";_tag_fat(_tmp447,sizeof(char),1U);});_tmp81C(Cyc_Absynpp_decl2doc,_tmp81B,tdl);});_tmp445[3]=_tmp81D;}),({
struct Cyc_PP_Doc*_tmp81A=Cyc_PP_line_doc();_tmp445[4]=_tmp81A;}),_tmp445[5]=exs_doc;Cyc_PP_cat(_tag_fat(_tmp445,sizeof(struct Cyc_PP_Doc*),6));});}else{
# 1948
s=({struct Cyc_PP_Doc*_tmp448[9];({struct Cyc_PP_Doc*_tmp82B=Cyc_PP_text(({const char*_tmp449="/* extern \"C include\" ";_tag_fat(_tmp449,sizeof(char),23U);}));_tmp448[0]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_Absynpp_lb();_tmp448[1]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp829=Cyc_PP_text(({const char*_tmp44A=" */";_tag_fat(_tmp44A,sizeof(char),4U);}));_tmp448[2]=_tmp829;}),({
struct Cyc_PP_Doc*_tmp828=Cyc_PP_line_doc();_tmp448[3]=_tmp828;}),({
struct Cyc_PP_Doc*_tmp827=({struct Cyc_PP_Doc*(*_tmp826)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp825=({const char*_tmp44B="";_tag_fat(_tmp44B,sizeof(char),1U);});_tmp826(Cyc_Absynpp_decl2doc,_tmp825,tdl);});_tmp448[4]=_tmp827;}),({
struct Cyc_PP_Doc*_tmp824=Cyc_PP_line_doc();_tmp448[5]=_tmp824;}),({
struct Cyc_PP_Doc*_tmp823=Cyc_PP_text(({const char*_tmp44C="/* ";_tag_fat(_tmp44C,sizeof(char),4U);}));_tmp448[6]=_tmp823;}),({
struct Cyc_PP_Doc*_tmp822=Cyc_Absynpp_rb();_tmp448[7]=_tmp822;}),({
struct Cyc_PP_Doc*_tmp821=Cyc_PP_text(({const char*_tmp44D=" */";_tag_fat(_tmp44D,sizeof(char),4U);}));_tmp448[8]=_tmp821;});Cyc_PP_cat(_tag_fat(_tmp448,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 13:
# 1959
 s=({struct Cyc_PP_Doc*_tmp44E[2];({struct Cyc_PP_Doc*_tmp82D=Cyc_PP_text(({const char*_tmp44F="__cyclone_port_on__;";_tag_fat(_tmp44F,sizeof(char),21U);}));_tmp44E[0]=_tmp82D;}),({struct Cyc_PP_Doc*_tmp82C=Cyc_Absynpp_lb();_tmp44E[1]=_tmp82C;});Cyc_PP_cat(_tag_fat(_tmp44E,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;case 14:
# 1962
 s=({struct Cyc_PP_Doc*_tmp450[2];({struct Cyc_PP_Doc*_tmp82F=Cyc_PP_text(({const char*_tmp451="__cyclone_port_off__;";_tag_fat(_tmp451,sizeof(char),22U);}));_tmp450[0]=_tmp82F;}),({struct Cyc_PP_Doc*_tmp82E=Cyc_Absynpp_lb();_tmp450[1]=_tmp82E;});Cyc_PP_cat(_tag_fat(_tmp450,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;case 15:
# 1965
 s=({struct Cyc_PP_Doc*_tmp452[2];({struct Cyc_PP_Doc*_tmp831=Cyc_PP_text(({const char*_tmp453="__tempest_on__;";_tag_fat(_tmp453,sizeof(char),16U);}));_tmp452[0]=_tmp831;}),({struct Cyc_PP_Doc*_tmp830=Cyc_Absynpp_lb();_tmp452[1]=_tmp830;});Cyc_PP_cat(_tag_fat(_tmp452,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;default:
# 1968
 s=({struct Cyc_PP_Doc*_tmp454[2];({struct Cyc_PP_Doc*_tmp833=Cyc_PP_text(({const char*_tmp455="__tempest_off__;";_tag_fat(_tmp455,sizeof(char),17U);}));_tmp454[0]=_tmp833;}),({struct Cyc_PP_Doc*_tmp832=Cyc_Absynpp_lb();_tmp454[1]=_tmp832;});Cyc_PP_cat(_tag_fat(_tmp454,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}_LL0:;}
# 1972
return s;}
# 1975
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp456=sc;switch((int)_tmp456){case Cyc_Absyn_Static:
 return Cyc_PP_text(({const char*_tmp457="static ";_tag_fat(_tmp457,sizeof(char),8U);}));case Cyc_Absyn_Public:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
 return Cyc_PP_text(({const char*_tmp458="extern ";_tag_fat(_tmp458,sizeof(char),8U);}));case Cyc_Absyn_ExternC:
 return Cyc_PP_text(({const char*_tmp459="extern \"C\" ";_tag_fat(_tmp459,sizeof(char),12U);}));case Cyc_Absyn_Abstract:
 return Cyc_PP_text(({const char*_tmp45A="abstract ";_tag_fat(_tmp45A,sizeof(char),10U);}));case Cyc_Absyn_Register:
 return Cyc_PP_text(({const char*_tmp45B="register ";_tag_fat(_tmp45B,sizeof(char),10U);}));default:
 return Cyc_PP_nil_doc();};}}
# 1989
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp45C=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp45C;void*_tmp45D=_stmttmp22;void*_tmp45E;switch(*((int*)_tmp45D)){case 2: _tmp45E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp45D)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp45E;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45D)->f1)== 9){_tmp45E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45D)->f2;{struct Cyc_List_List*l=_tmp45E;
return({(int(*)(int(*)(void*),struct Cyc_List_List*))Cyc_List_exists;})(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5:
 return 0;};}
# 2001
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp45F=t;void*_tmp461;void*_tmp460;switch(*((int*)_tmp45F)){case 7:
 return 1;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45F)->f1)== 16)
return 1;else{goto _LL7;}case 8: _tmp460=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp45F)->f3;_tmp461=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp45F)->f4;if(_tmp461 != 0){struct Cyc_Absyn_Typedefdecl*td=_tmp460;void*x=_tmp461;
# 2008
return Cyc_Absynpp_is_anon_aggrtype(x);}else{goto _LL7;}default: _LL7:
 return 0;};}
# 2015
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2018
if(tms != 0 && tms->tl != 0){
struct _tuple18 _tmp462=({struct _tuple18 _tmp4FB;_tmp4FB.f1=(void*)tms->hd,_tmp4FB.f2=(void*)(_check_null(tms->tl))->hd;_tmp4FB;});struct _tuple18 _stmttmp23=_tmp462;struct _tuple18 _tmp463=_stmttmp23;if(*((int*)_tmp463.f1)== 2){if(*((int*)_tmp463.f2)== 3)
# 2021
return({struct Cyc_List_List*_tmp465=_region_malloc(r,sizeof(*_tmp465));_tmp465->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp835=({struct Cyc_List_List*_tmp464=_region_malloc(r,sizeof(*_tmp464));_tmp464->hd=(void*)(_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp834=Cyc_Absynpp_bubble_attributes(r,atts,(_check_null(tms->tl))->tl);_tmp464->tl=_tmp834;});_tmp464;});_tmp465->tl=_tmp835;});_tmp465;});else{goto _LL3;}}else{_LL3:
 return({struct Cyc_List_List*_tmp466=_region_malloc(r,sizeof(*_tmp466));_tmp466->hd=atts,_tmp466->tl=tms;_tmp466;});};}else{
# 2024
return({struct Cyc_List_List*_tmp467=_region_malloc(r,sizeof(*_tmp467));_tmp467->hd=atts,_tmp467->tl=tms;_tmp467;});}}
# 2027
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp468=({struct _fat_ptr _tmp836=({const char*_tmp46E="`";_tag_fat(_tmp46E,sizeof(char),2U);});Cyc_strconcat(_tmp836,(struct _fat_ptr)*t->name);});struct _fat_ptr s=_tmp468;
({struct _fat_ptr _tmp469=_fat_ptr_plus(s,sizeof(char),1);char _tmp46A=*((char*)_check_fat_subscript(_tmp469,sizeof(char),0U));char _tmp46B='t';if(_get_fat_size(_tmp469,sizeof(char))== 1U &&(_tmp46A == 0 && _tmp46B != 0))_throw_arraybounds();*((char*)_tmp469.curr)=_tmp46B;});
({struct _fat_ptr*_tmp837=({unsigned _tmp46D=1;struct _fat_ptr*_tmp46C=_cycalloc(_check_times(_tmp46D,sizeof(struct _fat_ptr)));_tmp46C[0]=(struct _fat_ptr)s;_tmp46C;});t->name=_tmp837;});}}
# 2036
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2038
void*_tmp46F=t;void*_tmp47A;void*_tmp479;void*_tmp478;void*_tmp477;void*_tmp476;int _tmp474;void*_tmp473;struct Cyc_Absyn_PtrAtts _tmp47B;unsigned _tmp47C;void*_tmp472;void*_tmp471;struct Cyc_Absyn_Tqual _tmp475;void*_tmp470;switch(*((int*)_tmp46F)){case 4: _tmp470=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46F)->f1).elt_type;_tmp475=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46F)->f1).tq;_tmp471=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46F)->f1).num_elts;_tmp472=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46F)->f1).zero_term;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46F)->f1).zt_loc;{void*t2=_tmp470;struct Cyc_Absyn_Tqual tq2=_tmp475;struct Cyc_Absyn_Exp*e=_tmp471;void*zeroterm=_tmp472;unsigned ztl=_tmp47C;
# 2041
struct _tuple13 _tmp47D=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp24=_tmp47D;struct _tuple13 _tmp47E=_stmttmp24;void*_tmp481;void*_tmp480;struct Cyc_Absyn_Tqual _tmp47F;_tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;_tmp481=_tmp47E.f3;{struct Cyc_Absyn_Tqual tq3=_tmp47F;void*t3=_tmp480;struct Cyc_List_List*tml3=_tmp481;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp482=_region_malloc(r,sizeof(*_tmp482));_tmp482->tag=0,_tmp482->f1=zeroterm,_tmp482->f2=ztl;_tmp482;});else{
# 2046
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp483=_region_malloc(r,sizeof(*_tmp483));_tmp483->tag=1,_tmp483->f1=e,_tmp483->f2=zeroterm,_tmp483->f3=ztl;_tmp483;});}
return({struct _tuple13 _tmp4FC;_tmp4FC.f1=tq3,_tmp4FC.f2=t3,({struct Cyc_List_List*_tmp838=({struct Cyc_List_List*_tmp484=_region_malloc(r,sizeof(*_tmp484));_tmp484->hd=tm,_tmp484->tl=tml3;_tmp484;});_tmp4FC.f3=_tmp838;});_tmp4FC;});}}case 3: _tmp470=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).elt_type;_tmp475=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).elt_tq;_tmp47B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).ptr_atts;{void*t2=_tmp470;struct Cyc_Absyn_Tqual tq2=_tmp475;struct Cyc_Absyn_PtrAtts ptratts=_tmp47B;
# 2050
struct _tuple13 _tmp485=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _stmttmp25=_tmp485;struct _tuple13 _tmp486=_stmttmp25;void*_tmp489;void*_tmp488;struct Cyc_Absyn_Tqual _tmp487;_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;_tmp489=_tmp486.f3;{struct Cyc_Absyn_Tqual tq3=_tmp487;void*t3=_tmp488;struct Cyc_List_List*tml3=_tmp489;
tml3=({struct Cyc_List_List*_tmp48B=_region_malloc(r,sizeof(*_tmp48B));({void*_tmp839=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp48A=_region_malloc(r,sizeof(*_tmp48A));_tmp48A->tag=2,_tmp48A->f1=ptratts,_tmp48A->f2=tq;_tmp48A;});_tmp48B->hd=_tmp839;}),_tmp48B->tl=tml3;_tmp48B;});
return({struct _tuple13 _tmp4FD;_tmp4FD.f1=tq3,_tmp4FD.f2=t3,_tmp4FD.f3=tml3;_tmp4FD;});}}case 5: _tmp470=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).tvars;_tmp471=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).effect;_tmp475=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).ret_tqual;_tmp472=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).ret_type;_tmp473=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).args;_tmp474=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).c_varargs;_tmp476=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).cyc_varargs;_tmp477=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).rgn_po;_tmp478=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).attributes;_tmp479=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).requires_clause;_tmp47A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp46F)->f1).ensures_clause;{struct Cyc_List_List*typvars=_tmp470;void*effopt=_tmp471;struct Cyc_Absyn_Tqual t2qual=_tmp475;void*t2=_tmp472;struct Cyc_List_List*args=_tmp473;int c_varargs=_tmp474;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp476;struct Cyc_List_List*rgn_po=_tmp477;struct Cyc_List_List*fn_atts=_tmp478;struct Cyc_Absyn_Exp*req=_tmp479;struct Cyc_Absyn_Exp*ens=_tmp47A;
# 2056
if(!Cyc_Absynpp_print_all_tvars){
# 2060
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2065
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2068
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2073
struct _tuple13 _tmp48C=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _stmttmp26=_tmp48C;struct _tuple13 _tmp48D=_stmttmp26;void*_tmp490;void*_tmp48F;struct Cyc_Absyn_Tqual _tmp48E;_tmp48E=_tmp48D.f1;_tmp48F=_tmp48D.f2;_tmp490=_tmp48D.f3;{struct Cyc_Absyn_Tqual tq3=_tmp48E;void*t3=_tmp48F;struct Cyc_List_List*tml3=_tmp490;
struct Cyc_List_List*tms=tml3;
# 2084 "absynpp.cyc"
{enum Cyc_Flags_C_Compilers _tmp491=Cyc_Flags_c_compiler;if(_tmp491 == Cyc_Flags_Gcc_c){
# 2086
if(fn_atts != 0)
# 2089
tms=({struct _RegionHandle*_tmp83B=r;void*_tmp83A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp492=_region_malloc(r,sizeof(*_tmp492));_tmp492->tag=5,_tmp492->f1=0U,_tmp492->f2=fn_atts;_tmp492;});Cyc_Absynpp_bubble_attributes(_tmp83B,_tmp83A,tms);});
# 2091
tms=({struct Cyc_List_List*_tmp495=_region_malloc(r,sizeof(*_tmp495));({void*_tmp83D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp494=_region_malloc(r,sizeof(*_tmp494));
_tmp494->tag=3,({void*_tmp83C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp493=_region_malloc(r,sizeof(*_tmp493));_tmp493->tag=1,_tmp493->f1=args,_tmp493->f2=c_varargs,_tmp493->f3=cyc_varargs,_tmp493->f4=effopt,_tmp493->f5=rgn_po,_tmp493->f6=req,_tmp493->f7=ens;_tmp493;});_tmp494->f1=_tmp83C;});_tmp494;});
# 2091
_tmp495->hd=_tmp83D;}),_tmp495->tl=tms;_tmp495;});
# 2095
goto _LL16;}else{
# 2097
tms=({struct Cyc_List_List*_tmp498=_region_malloc(r,sizeof(*_tmp498));({void*_tmp83F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp497=_region_malloc(r,sizeof(*_tmp497));
_tmp497->tag=3,({void*_tmp83E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp496=_region_malloc(r,sizeof(*_tmp496));_tmp496->tag=1,_tmp496->f1=args,_tmp496->f2=c_varargs,_tmp496->f3=cyc_varargs,_tmp496->f4=effopt,_tmp496->f5=rgn_po,_tmp496->f6=req,_tmp496->f7=ens;_tmp496;});_tmp497->f1=_tmp83E;});_tmp497;});
# 2097
_tmp498->hd=_tmp83F;}),_tmp498->tl=tms;_tmp498;});
# 2101
for(1;fn_atts != 0;fn_atts=fn_atts->tl){
void*_tmp499=(void*)fn_atts->hd;void*_stmttmp27=_tmp499;void*_tmp49A=_stmttmp27;switch(*((int*)_tmp49A)){case 1:
 goto _LL1F;case 2: _LL1F:
 goto _LL21;case 3: _LL21:
# 2106
 tms=({struct Cyc_List_List*_tmp49D=_region_malloc(r,sizeof(*_tmp49D));({void*_tmp841=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp49C=_region_malloc(r,sizeof(*_tmp49C));_tmp49C->tag=5,_tmp49C->f1=0U,({struct Cyc_List_List*_tmp840=({struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->hd=(void*)fn_atts->hd,_tmp49B->tl=0;_tmp49B;});_tmp49C->f2=_tmp840;});_tmp49C;});_tmp49D->hd=_tmp841;}),_tmp49D->tl=tms;_tmp49D;});
goto AfterAtts;default:
 goto _LL1B;}_LL1B:;}
# 2110
goto _LL16;}_LL16:;}
# 2113
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_tmp49F=_region_malloc(r,sizeof(*_tmp49F));({void*_tmp842=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp49E=_region_malloc(r,sizeof(*_tmp49E));_tmp49E->tag=4,_tmp49E->f1=typvars,_tmp49E->f2=0U,_tmp49E->f3=1;_tmp49E;});_tmp49F->hd=_tmp842;}),_tmp49F->tl=tms;_tmp49F;});
return({struct _tuple13 _tmp4FE;_tmp4FE.f1=tq3,_tmp4FE.f2=t3,_tmp4FE.f3=tms;_tmp4FE;});}}}case 1: _tmp470=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46F)->f1;_tmp471=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46F)->f2;_tmp474=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46F)->f3;{struct Cyc_Core_Opt*k=_tmp470;void*topt=_tmp471;int i=_tmp474;
# 2119
if(topt == 0)
return({struct _tuple13 _tmp4FF;_tmp4FF.f1=tq,_tmp4FF.f2=t,_tmp4FF.f3=0;_tmp4FF;});else{
# 2122
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8: _tmp470=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp46F)->f1;_tmp471=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp46F)->f2;_tmp472=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp46F)->f3;_tmp473=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp46F)->f4;{struct _tuple0*n=_tmp470;struct Cyc_List_List*ts=_tmp471;struct Cyc_Absyn_Typedefdecl*td=_tmp472;void*topt=_tmp473;
# 2128
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _tmp500;_tmp500.f1=tq,_tmp500.f2=t,_tmp500.f3=0;_tmp500;});else{
# 2131
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default:
# 2136
 return({struct _tuple13 _tmp501;_tmp501.f1=tq,_tmp501.f2=t,_tmp501.f3=0;_tmp501;});};}
# 2140
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2142
void*_tmp4A0=t;void*_tmp4A1;switch(*((int*)_tmp4A0)){case 1: _tmp4A1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4A0)->f2;if(_tmp4A1 != 0){void*def=_tmp4A1;
return Cyc_Absynpp_is_char_ptr(def);}else{goto _LL5;}case 3: _tmp4A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A0)->f1).elt_type;{void*elt_typ=_tmp4A1;
# 2145
L: {
void*_tmp4A2=elt_typ;void*_tmp4A3;switch(*((int*)_tmp4A2)){case 1: _tmp4A3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4A2)->f2;if(_tmp4A3 != 0){void*t=_tmp4A3;
elt_typ=t;goto L;}else{goto _LLE;}case 8: _tmp4A3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4A2)->f4;if(_tmp4A3 != 0){void*t=_tmp4A3;
elt_typ=t;goto L;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A2)->f1)->f2 == Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;};}}default: _LL5:
# 2152
 return 0;};}
# 2156
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp4A4=_new_region("temp");struct _RegionHandle*temp=& _tmp4A4;_push_region(temp);
{struct _tuple13 _tmp4A5=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _stmttmp28=_tmp4A5;struct _tuple13 _tmp4A6=_stmttmp28;void*_tmp4A9;void*_tmp4A8;struct Cyc_Absyn_Tqual _tmp4A7;_tmp4A7=_tmp4A6.f1;_tmp4A8=_tmp4A6.f2;_tmp4A9=_tmp4A6.f3;{struct Cyc_Absyn_Tqual tq=_tmp4A7;void*t=_tmp4A8;struct Cyc_List_List*tms=_tmp4A9;
tms=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_tmp4AB=({struct Cyc_PP_Doc*_tmp4AA[2];({struct Cyc_PP_Doc*_tmp844=Cyc_Absynpp_tqual2doc(tq);_tmp4AA[0]=_tmp844;}),({struct Cyc_PP_Doc*_tmp843=Cyc_Absynpp_ntyp2doc(t);_tmp4AA[1]=_tmp843;});Cyc_PP_cat(_tag_fat(_tmp4AA,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _tmp4AB;}else{
# 2163
struct Cyc_PP_Doc*_tmp4AE=({struct Cyc_PP_Doc*_tmp4AC[4];({
struct Cyc_PP_Doc*_tmp84A=Cyc_Absynpp_tqual2doc(tq);_tmp4AC[0]=_tmp84A;}),({
struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_ntyp2doc(t);_tmp4AC[1]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_PP_text(({const char*_tmp4AD=" ";_tag_fat(_tmp4AD,sizeof(char),2U);}));_tmp4AC[2]=_tmp848;}),({
struct Cyc_PP_Doc*_tmp847=({int _tmp846=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp845=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp846,_tmp845,tms);});_tmp4AC[3]=_tmp847;});Cyc_PP_cat(_tag_fat(_tmp4AC,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _tmp4AE;}}}
# 2158
;_pop_region();}
# 2171
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp4AF=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp4AF;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_tmp4B0[2];({struct Cyc_PP_Doc*_tmp84C=Cyc_PP_text(({const char*_tmp4B1="@requires ";_tag_fat(_tmp4B1,sizeof(char),11U);}));_tmp4B0[0]=_tmp84C;}),({struct Cyc_PP_Doc*_tmp84B=Cyc_Absynpp_exp2doc(req);_tmp4B0[1]=_tmp84B;});Cyc_PP_cat(_tag_fat(_tmp4B0,sizeof(struct Cyc_PP_Doc*),2));});else{
# 2177
requires_doc=Cyc_PP_nil_doc();}{
# 2179
enum Cyc_Flags_C_Compilers _tmp4B2=Cyc_Flags_c_compiler;if(_tmp4B2 == Cyc_Flags_Gcc_c){
# 2182
if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp4B3[5];({struct Cyc_PP_Doc*_tmp854=({struct Cyc_Absyn_Tqual _tmp853=f->tq;void*_tmp852=f->type;Cyc_Absynpp_tqtd2doc(_tmp853,_tmp852,({struct Cyc_Core_Opt*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));({struct Cyc_PP_Doc*_tmp851=Cyc_PP_textptr(f->name);_tmp4B4->v=_tmp851;});_tmp4B4;}));});_tmp4B3[0]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp850=Cyc_PP_text(({const char*_tmp4B5=":";_tag_fat(_tmp4B5,sizeof(char),2U);}));_tmp4B3[1]=_tmp850;}),({struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_exp2doc(_check_null(f->width));_tmp4B3[2]=_tmp84F;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_Absynpp_atts2doc(f->attributes);_tmp4B3[3]=_tmp84E;}),({struct Cyc_PP_Doc*_tmp84D=Cyc_PP_text(({const char*_tmp4B6=";";_tag_fat(_tmp4B6,sizeof(char),2U);}));_tmp4B3[4]=_tmp84D;});Cyc_PP_cat(_tag_fat(_tmp4B3,sizeof(struct Cyc_PP_Doc*),5));});
return({struct Cyc_PP_Doc*_tmp4B7[4];({struct Cyc_PP_Doc*_tmp85A=({struct Cyc_Absyn_Tqual _tmp859=f->tq;void*_tmp858=f->type;Cyc_Absynpp_tqtd2doc(_tmp859,_tmp858,({struct Cyc_Core_Opt*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));({struct Cyc_PP_Doc*_tmp857=Cyc_PP_textptr(f->name);_tmp4B8->v=_tmp857;});_tmp4B8;}));});_tmp4B7[0]=_tmp85A;}),({
struct Cyc_PP_Doc*_tmp856=Cyc_Absynpp_atts2doc(f->attributes);_tmp4B7[1]=_tmp856;}),_tmp4B7[2]=requires_doc,({struct Cyc_PP_Doc*_tmp855=Cyc_PP_text(({const char*_tmp4B9=";";_tag_fat(_tmp4B9,sizeof(char),2U);}));_tmp4B7[3]=_tmp855;});Cyc_PP_cat(_tag_fat(_tmp4B7,sizeof(struct Cyc_PP_Doc*),4));});}else{
# 2189
if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp4BA[5];({struct Cyc_PP_Doc*_tmp862=Cyc_Absynpp_atts2doc(f->attributes);_tmp4BA[0]=_tmp862;}),({
struct Cyc_PP_Doc*_tmp861=({struct Cyc_Absyn_Tqual _tmp860=f->tq;void*_tmp85F=f->type;Cyc_Absynpp_tqtd2doc(_tmp860,_tmp85F,({struct Cyc_Core_Opt*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));({struct Cyc_PP_Doc*_tmp85E=Cyc_PP_textptr(f->name);_tmp4BB->v=_tmp85E;});_tmp4BB;}));});_tmp4BA[1]=_tmp861;}),({
struct Cyc_PP_Doc*_tmp85D=Cyc_PP_text(({const char*_tmp4BC=":";_tag_fat(_tmp4BC,sizeof(char),2U);}));_tmp4BA[2]=_tmp85D;}),({struct Cyc_PP_Doc*_tmp85C=Cyc_Absynpp_exp2doc(_check_null(f->width));_tmp4BA[3]=_tmp85C;}),({struct Cyc_PP_Doc*_tmp85B=Cyc_PP_text(({const char*_tmp4BD=";";_tag_fat(_tmp4BD,sizeof(char),2U);}));_tmp4BA[4]=_tmp85B;});Cyc_PP_cat(_tag_fat(_tmp4BA,sizeof(struct Cyc_PP_Doc*),5));});
return({struct Cyc_PP_Doc*_tmp4BE[4];({struct Cyc_PP_Doc*_tmp868=Cyc_Absynpp_atts2doc(f->attributes);_tmp4BE[0]=_tmp868;}),({
struct Cyc_PP_Doc*_tmp867=({struct Cyc_Absyn_Tqual _tmp866=f->tq;void*_tmp865=f->type;Cyc_Absynpp_tqtd2doc(_tmp866,_tmp865,({struct Cyc_Core_Opt*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct Cyc_PP_Doc*_tmp864=Cyc_PP_textptr(f->name);_tmp4BF->v=_tmp864;});_tmp4BF;}));});_tmp4BE[1]=_tmp867;}),_tmp4BE[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp863=Cyc_PP_text(({const char*_tmp4C0=";";_tag_fat(_tmp4C0,sizeof(char),2U);}));_tmp4BE[3]=_tmp863;});Cyc_PP_cat(_tag_fat(_tmp4BE,sizeof(struct Cyc_PP_Doc*),4));});};}}
# 2200
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct Cyc_PP_Doc*(*_tmp86A)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp869=({const char*_tmp4C1="";_tag_fat(_tmp4C1,sizeof(char),1U);});_tmp86A(Cyc_Absynpp_aggrfield2doc,_tmp869,fields);});}
# 2204
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp4C2[3];({struct Cyc_PP_Doc*_tmp86E=Cyc_Absynpp_scope2doc(f->sc);_tmp4C2[0]=_tmp86E;}),({struct Cyc_PP_Doc*_tmp86D=Cyc_Absynpp_typedef_name2doc(f->name);_tmp4C2[1]=_tmp86D;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp86C=Cyc_PP_nil_doc();_tmp4C2[2]=_tmp86C;}):({struct Cyc_PP_Doc*_tmp86B=Cyc_Absynpp_args2doc(f->typs);_tmp4C2[2]=_tmp86B;});Cyc_PP_cat(_tag_fat(_tmp4C2,sizeof(struct Cyc_PP_Doc*),3));});}
# 2209
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct Cyc_PP_Doc*(*_tmp870)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr,struct Cyc_List_List*)=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;});struct _fat_ptr _tmp86F=({const char*_tmp4C3=",";_tag_fat(_tmp4C3,sizeof(char),2U);});_tmp870(Cyc_Absynpp_datatypefield2doc,_tmp86F,fields);});}
# 2218
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(1;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp871=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp871,72,f);});
({void*_tmp4C4=0U;({struct Cyc___cycFILE*_tmp873=f;struct _fat_ptr _tmp872=({const char*_tmp4C5="\n";_tag_fat(_tmp4C5,sizeof(char),2U);});Cyc_fprintf(_tmp873,_tmp872,_tag_fat(_tmp4C4,sizeof(void*),0));});});}}
# 2225
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _tmp874=({const char*_tmp4C6="";_tag_fat(_tmp4C6,sizeof(char),1U);});Cyc_PP_seql(_tmp874,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2228
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0,0),72);}
struct _fat_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _fat_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;}}
# 2242
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2247
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return(struct _fat_ptr)({const char*_tmp4C7="NULL";_tag_fat(_tmp4C7,sizeof(char),5U);});{
void*_tmp4C8=t;void*_tmp4C9;switch(*((int*)_tmp4C8)){case 0:
 return(struct _fat_ptr)({const char*_tmp4CA="AppType";_tag_fat(_tmp4CA,sizeof(char),8U);});case 1:
 return(struct _fat_ptr)({const char*_tmp4CB="Evar";_tag_fat(_tmp4CB,sizeof(char),5U);});case 2:
 return(struct _fat_ptr)({const char*_tmp4CC="Vartype";_tag_fat(_tmp4CC,sizeof(char),8U);});case 3:
 return(struct _fat_ptr)({const char*_tmp4CD="Pointertype";_tag_fat(_tmp4CD,sizeof(char),12U);});case 4:
 return(struct _fat_ptr)({const char*_tmp4CE="ArrayType";_tag_fat(_tmp4CE,sizeof(char),10U);});case 5:
 return(struct _fat_ptr)({const char*_tmp4CF="FnType";_tag_fat(_tmp4CF,sizeof(char),7U);});case 6:
 return(struct _fat_ptr)({const char*_tmp4D0="TupleType";_tag_fat(_tmp4D0,sizeof(char),10U);});case 7:
 return(struct _fat_ptr)({const char*_tmp4D1="AnonAggrType";_tag_fat(_tmp4D1,sizeof(char),13U);});case 8: _tmp4C9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C8)->f3;{struct Cyc_Absyn_Typedefdecl*td=_tmp4C9;
# 2260
return(struct _fat_ptr)({const char*_tmp4D2="Typedeftype";_tag_fat(_tmp4D2,sizeof(char),12U);});}case 9:
 return(struct _fat_ptr)({const char*_tmp4D3="Valueoftype";_tag_fat(_tmp4D3,sizeof(char),12U);});case 10:
 return(struct _fat_ptr)({const char*_tmp4D4="Typedecltype";_tag_fat(_tmp4D4,sizeof(char),13U);});default:
 return(struct _fat_ptr)({const char*_tmp4D5="Typeoftype";_tag_fat(_tmp4D5,sizeof(char),11U);});};}}
