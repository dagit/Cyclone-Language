#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
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
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void *);
#endif
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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
extern int Cyc_fputc(int,struct Cyc___cycFILE*);
# 278 "cycboot.h"
extern int Cyc_file_string_write(struct Cyc___cycFILE*,struct _fat_ptr,int,int);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 504
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 669 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 864
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 870
void*Cyc_Absyn_compress(void*);
# 874
int Cyc_Absyn_type2bool(int,void*);
# 883
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1125
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1133
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
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;struct _tuple15{struct _fat_ptr f1;unsigned f2;};
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
static int Cyc_Absyndump_to_VC;static char _TmpG0[14U]="__NoCycPrefix";
# 53
static struct _fat_ptr Cyc_Absyndump_nocyc_str={_TmpG0,_TmpG0,_TmpG0 + 14U};
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
# 96
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
static char Cyc_Absyndump_prev_char='x';
# 99
static int Cyc_Absyndump_need_space_before (void){
char _Tmp0=Cyc_Absyndump_prev_char;switch((int)_Tmp0){case 123:
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
unsigned sz=Cyc_strlen(s);
# 126
if(Cyc_Absyndump_need_space_before())
Cyc_fputc(32,*Cyc_Absyndump_dump_file);
if(sz >= 1U){
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),(int)(sz - 1U)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 134
static void Cyc_Absyndump_dump_nospace(struct _fat_ptr s){
int sz=(int)Cyc_strlen(s);
# 137
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),sz - 1));}}
# 142
static void Cyc_Absyndump_dump_char(int c){
# 144
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _TmpG1[1U]="";
# 148
static void Cyc_Absyndump_dumploc(unsigned loc){
static struct _fat_ptr last_file={_TmpG1,_TmpG1,_TmpG1 + 1U};
static unsigned last_line=0U;
if(loc == 0U)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 154
struct _tuple15 _stmttmp0=Cyc_Lex_xlate_pos(loc);struct _tuple15 _Tmp0=_stmttmp0;unsigned _Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct _fat_ptr f=_Tmp2;unsigned d=_Tmp1;
if((char*)f.curr == (char*)(_tag_fat(0,0,0)).curr ||(char*)f.curr == (char*)last_file.curr && d == last_line)return;
if((char*)f.curr == (char*)last_file.curr && d == last_line + 1U)
Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_Tmp3="\n";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));else{
if((char*)f.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)d);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_Tmp5="\n# %d\n";_tag_fat(_Tmp5,sizeof(char),7U);}),_tag_fat(_Tmp4,sizeof(void*),1));});else{
# 161
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)d);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=f;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(*Cyc_Absyndump_dump_file,({const char*_Tmp6="\n# %d %s\n";_tag_fat(_Tmp6,sizeof(char),10U);}),_tag_fat(_Tmp5,sizeof(void*),2));});}}
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
for(1;(_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(1;(_check_null(l))->tl != 0;l=l->tl){
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
struct Cyc_List_List*nsl=0;
struct _fat_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _stmttmp1=(*v).f1;union Cyc_Absyn_Nmspace _Tmp0=_stmttmp1;void*_Tmp1;switch((_Tmp0.C_n).tag){case 4:
 goto _LL0;case 1: _Tmp1=(_Tmp0.Rel_n).val;{struct Cyc_List_List*x=_Tmp1;
nsl=x;goto _LL0;}case 3: _Tmp1=(_Tmp0.C_n).val;{struct Cyc_List_List*x=_Tmp1;
goto _LL0;}default: _Tmp1=(_Tmp0.Abs_n).val;{struct Cyc_List_List*x=_Tmp1;
# 226
if(Cyc_Absyndump_qvar_to_Cids || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
nsl=x;
goto _LL0;}}_LL0:;}
# 231
if(Cyc_Absyndump_need_space_before())
Cyc_Absyndump_dump_char(32);{
struct _fat_ptr sep=Cyc_Absyndump_qvar_to_Cids?({const char*_Tmp0="_";_tag_fat(_Tmp0,sizeof(char),2U);}):({const char*_Tmp0="::";_tag_fat(_Tmp0,sizeof(char),3U);});
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
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_Tmp0="restrict";_tag_fat(_Tmp0,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_Tmp0="volatile";_tag_fat(_Tmp0,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_Tmp0="const";_tag_fat(_Tmp0,sizeof(char),6U);}));}
# 252
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _Tmp0=sc;switch((int)_Tmp0){case Cyc_Absyn_Static:
 Cyc_Absyndump_dump(({const char*_Tmp1="static";_tag_fat(_Tmp1,sizeof(char),7U);}));return;case Cyc_Absyn_Public:
 return;case Cyc_Absyn_Extern:
 Cyc_Absyndump_dump(({const char*_Tmp1="extern";_tag_fat(_Tmp1,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC:
 Cyc_Absyndump_dump(({const char*_Tmp1="extern \"C\"";_tag_fat(_Tmp1,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract:
 Cyc_Absyndump_dump(({const char*_Tmp1="abstract";_tag_fat(_Tmp1,sizeof(char),9U);}));return;case Cyc_Absyn_Register:
 Cyc_Absyndump_dump(({const char*_Tmp1="register";_tag_fat(_Tmp1,sizeof(char),9U);}));return;default:
 return;};}
# 263
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _Tmp0=k;if(_Tmp0 == Cyc_Absyn_StructA){
Cyc_Absyndump_dump(({const char*_Tmp1="struct";_tag_fat(_Tmp1,sizeof(char),7U);}));return;}else{
Cyc_Absyndump_dump(({const char*_Tmp1="union";_tag_fat(_Tmp1,sizeof(char),6U);}));return;};}
# 270
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
Cyc_Absyndump_dump(Cyc_Absynpp_kind2string(ka));}
# 274
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,ts,({const char*_Tmp0="<";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=">";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 277
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
if((int)*((const char*)_check_fat_subscript(n,sizeof(char),0))== 35){
Cyc_Absyndump_dump(({const char*_Tmp0="`G";_tag_fat(_Tmp0,sizeof(char),3U);}));
{void*_stmttmp2=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp0=_stmttmp2;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
Cyc_Absyndump_dumpkind(k);goto _LL0;}default:
 Cyc_Absyndump_dump_nospace(({const char*_Tmp2="K";_tag_fat(_Tmp2,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 286
Cyc_Absyndump_dump_nospace(_fat_ptr_plus(n,sizeof(char),1));}else{
# 288
Cyc_Absyndump_dump(n);}}
# 290
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_stmttmp3=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp0=_stmttmp3;void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
Cyc_Absyndump_dump(({const char*_Tmp2="::";_tag_fat(_Tmp2,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(k);goto _LL0;}}}_LL0:;}}
# 299
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;})(Cyc_Absyndump_dumptvar,tvs,({const char*_Tmp0="<";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=">";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 302
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;})(Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_Tmp0="<";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=">";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 305
static void Cyc_Absyndump_dumparg(struct _tuple16*pr){
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int))Cyc_Absyndump_dumptqtd;})((*pr).f1,(*pr).f2,({(void(*)(int))Cyc_Absyndump_ignore;}),0);}
# 308
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({(void(*)(void(*)(struct _tuple16*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumparg,ts,({const char*_Tmp0="(";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=")";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 312
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(1;atts != 0;atts=atts->tl){
void*_stmttmp4=(void*)atts->hd;void*_Tmp0=_stmttmp4;switch(*((int*)_Tmp0)){case 1:
 Cyc_Absyndump_dump(({const char*_Tmp1="_stdcall";_tag_fat(_Tmp1,sizeof(char),9U);}));return;case 2:
 Cyc_Absyndump_dump(({const char*_Tmp1="_cdecl";_tag_fat(_Tmp1,sizeof(char),7U);}));return;case 3:
 Cyc_Absyndump_dump(({const char*_Tmp1="_fastcall";_tag_fat(_Tmp1,sizeof(char),10U);}));return;default:
 goto _LL0;}_LL0:;}}
# 322
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 324
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_stmttmp5=(void*)(_check_null(atts))->hd;void*_Tmp0=_stmttmp5;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 332
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_Tmp0="__declspec(";_tag_fat(_Tmp0,sizeof(char),12U);}));
for(1;atts != 0;atts=atts->tl){
void*_stmttmp6=(void*)atts->hd;void*_Tmp0=_stmttmp6;switch(*((int*)_Tmp0)){case 1:
 goto _LLD;case 2: _LLD:
 goto _LLF;case 3: _LLF:
 goto _LL9;default:
 Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 342
Cyc_Absyndump_dump_char(41);}
# 345
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Flags_C_Compilers _Tmp0=Cyc_Flags_c_compiler;if(_Tmp0 == Cyc_Flags_Gcc_c){
# 349
Cyc_Absyndump_dump(({const char*_Tmp1=" __attribute__((";_tag_fat(_Tmp1,sizeof(char),17U);}));
for(1;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_Tmp1=",";_tag_fat(_Tmp1,sizeof(char),2U);}));}
# 354
Cyc_Absyndump_dump(({const char*_Tmp1=")) ";_tag_fat(_Tmp1,sizeof(char),4U);}));
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
{void*_stmttmp7=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp7;void*_Tmp1;if(*((int*)_Tmp0)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2 != 0){_Tmp1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd;{void*r=_Tmp1;
rgions=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=r,_Tmp2->tl=rgions;_Tmp2;});goto _LL0;}}else{goto _LL5;}case 9: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp1;
# 372
for(1;ts != 0;ts=ts->tl){
struct _tuple17 _stmttmp8=Cyc_Absyndump_effects_split((void*)ts->hd);struct _tuple17 _Tmp2=_stmttmp8;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*rs=_Tmp4;struct Cyc_List_List*es=_Tmp3;
rgions=Cyc_List_imp_append(rs,rgions);
effects=Cyc_List_imp_append(es,effects);}}
# 377
goto _LL0;}default: goto _LL5;}else{_LL5:
 effects=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=t,_Tmp2->tl=effects;_Tmp2;});goto _LL0;}_LL0:;}
# 380
return({struct _tuple17 _Tmp0;_Tmp0.f1=rgions,_Tmp0.f2=effects;_Tmp0;});}
# 383
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple17 _stmttmp9=Cyc_Absyndump_effects_split(t);struct _tuple17 _Tmp0=_stmttmp9;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*rgions=_Tmp2;struct Cyc_List_List*effects=_Tmp1;
rgions=Cyc_List_imp_rev(rgions);
effects=Cyc_List_imp_rev(effects);
if(effects != 0){
Cyc_Absyndump_group(Cyc_Absyndump_dumpntyp,effects,({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({const char*_Tmp3="+";_tag_fat(_Tmp3,sizeof(char),2U);}));
if(rgions != 0)Cyc_Absyndump_dump_char(43);}
# 391
if(rgions != 0 || effects == 0)
Cyc_Absyndump_group(Cyc_Absyndump_dumprgn,rgions,({const char*_Tmp3="{";_tag_fat(_Tmp3,sizeof(char),2U);}),({const char*_Tmp3="}";_tag_fat(_Tmp3,sizeof(char),2U);}),({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);}));}}
# 396
static void Cyc_Absyndump_dumpntyp(void*t){
void*_Tmp0=t;enum Cyc_Absyn_AggrKind _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 4:
# 399
 goto _LL4;case 5: _LL4:
 goto _LL6;case 3: _LL6:
 return;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 0){
Cyc_Absyndump_dump(({const char*_Tmp5="void";_tag_fat(_Tmp5,sizeof(char),5U);}));return;}else{_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 419
void*_Tmp5=c;struct _fat_ptr _Tmp6;void*_Tmp7;union Cyc_Absyn_AggrInfo _Tmp8;int _Tmp9;union Cyc_Absyn_DatatypeFieldInfo _TmpA;union Cyc_Absyn_DatatypeInfo _TmpB;switch(*((int*)_Tmp5)){case 18: _TmpB=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_Tmp5)->f1;{union Cyc_Absyn_DatatypeInfo tu_info=_TmpB;
# 421
{union Cyc_Absyn_DatatypeInfo _TmpC=tu_info;int _TmpD;void*_TmpE;if((_TmpC.UnknownDatatype).tag == 1){_TmpE=((_TmpC.UnknownDatatype).val).name;_TmpD=((_TmpC.UnknownDatatype).val).is_extensible;{struct _tuple0*n=_TmpE;int is_x=_TmpD;
_TmpE=n;_TmpD=is_x;goto _LL6C;}}else{_TmpE=(*(_TmpC.KnownDatatype).val)->name;_TmpD=(*(_TmpC.KnownDatatype).val)->is_extensible;_LL6C: {struct _tuple0*n=_TmpE;int is_x=_TmpD;
# 424
if(is_x)Cyc_Absyndump_dump(({const char*_TmpF="@extensible";_tag_fat(_TmpF,sizeof(char),12U);}));
Cyc_Absyndump_dump(({const char*_TmpF="datatype";_tag_fat(_TmpF,sizeof(char),9U);}));Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
goto _LL68;}}_LL68:;}
# 428
return;}case 19: _TmpA=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_Tmp5)->f1;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_TmpA;
# 430
{union Cyc_Absyn_DatatypeFieldInfo _TmpC=tuf_info;int _TmpD;void*_TmpE;void*_TmpF;if((_TmpC.UnknownDatatypefield).tag == 1){_TmpF=((_TmpC.UnknownDatatypefield).val).datatype_name;_TmpE=((_TmpC.UnknownDatatypefield).val).field_name;_TmpD=((_TmpC.UnknownDatatypefield).val).is_extensible;{struct _tuple0*tname=_TmpF;struct _tuple0*fname=_TmpE;int is_x=_TmpD;
# 432
_TmpF=tname;_TmpD=is_x;_TmpE=fname;goto _LL71;}}else{_TmpF=(((_TmpC.KnownDatatypefield).val).f1)->name;_TmpD=(((_TmpC.KnownDatatypefield).val).f1)->is_extensible;_TmpE=(((_TmpC.KnownDatatypefield).val).f2)->name;_LL71: {struct _tuple0*tname=_TmpF;int is_x=_TmpD;struct _tuple0*fname=_TmpE;
# 435
if(is_x)Cyc_Absyndump_dump(({const char*_Tmp10="@extensible ";_tag_fat(_Tmp10,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_Tmp10="datatype ";_tag_fat(_Tmp10,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(tname);Cyc_Absyndump_dump(({const char*_Tmp10=".";_tag_fat(_Tmp10,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(fname);
Cyc_Absyndump_dumptps(ts);
goto _LL6D;}}_LL6D:;}
# 440
return;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp5)->f1){case Cyc_Absyn_None: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp5)->f2){case Cyc_Absyn_Int_sz:
 goto _LL29;case Cyc_Absyn_Long_sz:
# 443
 goto _LL2D;case Cyc_Absyn_Char_sz:
# 445
 Cyc_Absyndump_dump(({const char*_TmpC="char";_tag_fat(_TmpC,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz:
# 448
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL41:
# 454
 goto _LL43;default: goto _LL66;}case Cyc_Absyn_Signed: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp5)->f2){case Cyc_Absyn_Int_sz: _LL29:
# 442
 Cyc_Absyndump_dump(({const char*_TmpC="int";_tag_fat(_TmpC,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL2D:
# 444
 Cyc_Absyndump_dump(({const char*_TmpC="long";_tag_fat(_TmpC,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz:
# 446
 Cyc_Absyndump_dump(({const char*_TmpC="signed char";_tag_fat(_TmpC,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL37:
# 449
 Cyc_Absyndump_dump(({const char*_TmpC="short";_tag_fat(_TmpC,sizeof(char),6U);}));return;case Cyc_Absyn_LongLong_sz: _LL43: {
# 456
enum Cyc_Flags_C_Compilers _TmpC=Cyc_Flags_c_compiler;if(_TmpC == Cyc_Flags_Gcc_c){
Cyc_Absyndump_dump(({const char*_TmpD="long long";_tag_fat(_TmpD,sizeof(char),10U);}));return;}else{
Cyc_Absyndump_dump(({const char*_TmpD="__int64";_tag_fat(_TmpD,sizeof(char),8U);}));return;};}default: goto _LL66;}case Cyc_Absyn_Unsigned: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp5)->f2){case Cyc_Absyn_Char_sz:
# 447
 Cyc_Absyndump_dump(({const char*_TmpC="unsigned char";_tag_fat(_TmpC,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz:
# 450
 Cyc_Absyndump_dump(({const char*_TmpC="unsigned short";_tag_fat(_TmpC,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz:
 Cyc_Absyndump_dump(({const char*_TmpC="unsigned";_tag_fat(_TmpC,sizeof(char),9U);}));return;case Cyc_Absyn_Long_sz:
 Cyc_Absyndump_dump(({const char*_TmpC="unsigned long";_tag_fat(_TmpC,sizeof(char),14U);}));return;case Cyc_Absyn_LongLong_sz:
 Cyc_Absyndump_dump(({const char*_TmpC="unsigned";_tag_fat(_TmpC,sizeof(char),9U);}));goto _LL41;default: goto _LL66;}default: goto _LL66;}case 2: _Tmp9=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp5)->f1;{int i=_Tmp9;
# 461
int _TmpC=i;switch((int)_TmpC){case 0:
 Cyc_Absyndump_dump(({const char*_TmpD="float";_tag_fat(_TmpD,sizeof(char),6U);}));return;case 1:
 Cyc_Absyndump_dump(({const char*_TmpD="double";_tag_fat(_TmpD,sizeof(char),7U);}));return;default:
 Cyc_Absyndump_dump(({const char*_TmpD="long double";_tag_fat(_TmpD,sizeof(char),12U);}));return;};}case 20: _Tmp8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_Tmp5)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp8;
# 467
struct _tuple11 _stmttmpA=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _TmpC=_stmttmpA;void*_TmpD;enum Cyc_Absyn_AggrKind _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{enum Cyc_Absyn_AggrKind k=_TmpE;struct _tuple0*n=_TmpD;
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
return;}}case 15: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_Tmp5)->f1;{struct _tuple0*n=_Tmp7;
Cyc_Absyndump_dump(({const char*_TmpC="enum";_tag_fat(_TmpC,sizeof(char),5U);}));Cyc_Absyndump_dumpqvar(n);return;}case 16: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_Tmp5)->f1;{struct Cyc_List_List*fs=_Tmp7;
Cyc_Absyndump_dump(({const char*_TmpC="enum{";_tag_fat(_TmpC,sizeof(char),6U);}));Cyc_Absyndump_dumpenumfields(fs);Cyc_Absyndump_dump(({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}));return;}case 17: _Tmp6=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_Tmp5)->f1;{struct _fat_ptr t=_Tmp6;
Cyc_Absyndump_dump(t);return;}case 3:
 Cyc_Absyndump_dump(({const char*_TmpC="region_t<";_tag_fat(_TmpC,sizeof(char),10U);}));Cyc_Absyndump_dumprgn((void*)(_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_TmpC=">";_tag_fat(_TmpC,sizeof(char),2U);}));return;case 4:
 Cyc_Absyndump_dump(({const char*_TmpC="tag_t<";_tag_fat(_TmpC,sizeof(char),7U);}));Cyc_Absyndump_dumptyp((void*)(_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_TmpC=">";_tag_fat(_TmpC,sizeof(char),2U);}));return;case 6:
 Cyc_Absyndump_dump(({const char*_TmpC="`U";_tag_fat(_TmpC,sizeof(char),3U);}));return;case 7:
 Cyc_Absyndump_dump(({const char*_TmpC="`RC";_tag_fat(_TmpC,sizeof(char),4U);}));return;case 5:
 Cyc_Absyndump_dump(({const char*_TmpC="`H";_tag_fat(_TmpC,sizeof(char),3U);}));return;case 8:
 Cyc_Absyndump_dump(({const char*_TmpC="{";_tag_fat(_TmpC,sizeof(char),2U);}));Cyc_Absyndump_dumptyp((void*)(_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}));return;case 10:
 Cyc_Absyndump_dump(({const char*_TmpC="regions(";_tag_fat(_TmpC,sizeof(char),9U);}));Cyc_Absyndump_dumptyp((void*)(_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));return;case 9:
 Cyc_Absyndump_group(Cyc_Absyndump_dumptyp,ts,({const char*_TmpC="";_tag_fat(_TmpC,sizeof(char),1U);}),({const char*_TmpC="";_tag_fat(_TmpC,sizeof(char),1U);}),({const char*_TmpC="+";_tag_fat(_TmpC,sizeof(char),2U);}));return;case 11:
 Cyc_Absyndump_dump(({const char*_TmpC="@true";_tag_fat(_TmpC,sizeof(char),6U);}));return;case 12:
 Cyc_Absyndump_dump(({const char*_TmpC="@false";_tag_fat(_TmpC,sizeof(char),7U);}));return;case 13:
 Cyc_Absyndump_dump(({const char*_TmpC="@thin";_tag_fat(_TmpC,sizeof(char),6U);}));return;case 14:
 Cyc_Absyndump_dump(({const char*_TmpC="@fat";_tag_fat(_TmpC,sizeof(char),5U);}));return;default: _LL66:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="bad type constructor";_tag_fat(_TmpE,sizeof(char),21U);});_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpD,sizeof(void*),1));});};}}case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 403
Cyc_Absyndump_dumptvar(tv);return;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2 == 0){_Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_Core_Opt*k=_Tmp4;int i=_Tmp2;
# 405
Cyc_Absyndump_dump(({const char*_Tmp5="`E";_tag_fat(_Tmp5,sizeof(char),3U);}));
if(k == 0)Cyc_Absyndump_dump(({const char*_Tmp5="K";_tag_fat(_Tmp5,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)k->v);}
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="%d";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(_Tmp6,sizeof(void*),1));}));return;}}else{_Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_Core_Opt*k=_Tmp4;void*t=_Tmp3;int i=_Tmp2;
Cyc_Absyndump_dumpntyp(_check_null(t));return;}}case 6: _Tmp4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*ts=_Tmp4;
Cyc_Absyndump_dump_char(36);Cyc_Absyndump_dumpargs(ts);return;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp4;
# 411
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpaggrfields(fs);Cyc_Absyndump_dump_char(125);return;}case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)){case 0: _Tmp4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_Tmp4;
Cyc_Absyndump_dump_aggrdecl(d);return;}case 1: _Tmp4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_Tmp4;
Cyc_Absyndump_dump_enumdecl(d);return;}default: _Tmp4=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_Tmp4;
Cyc_Absyndump_dump_datatypedecl(d);return;}}case 8: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;{struct _tuple0*n=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
Cyc_Absyndump_dumpqvar(n),Cyc_Absyndump_dumptps(ts);return;}case 9: _Tmp4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dump(({const char*_Tmp5="valueof_t(";_tag_fat(_Tmp5,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_Tmp5=")";_tag_fat(_Tmp5,sizeof(char),2U);}));return;}default: _Tmp4=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dump(({const char*_Tmp5="typeof(";_tag_fat(_Tmp5,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_Tmp5=")";_tag_fat(_Tmp5,sizeof(char),2U);}));return;}};}
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
({(void(*)(void(*)(struct _tuple18*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;})(Cyc_Absyndump_dump_rgncmp,rgn_po,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 502
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 506
if((((args == 0 && !c_varargs)&& cyc_varargs == 0)&& effopt == 0)&& rgn_po == 0)
# 508
Cyc_Absyndump_dump(({const char*_Tmp0="(void)";_tag_fat(_Tmp0,sizeof(char),7U);}));else{
# 511
({(void(*)(void(*)(struct _tuple8*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpfunarg,args,({const char*_Tmp0="(";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));
if(c_varargs || cyc_varargs != 0)
Cyc_Absyndump_dump(({const char*_Tmp0=",...";_tag_fat(_Tmp0,sizeof(char),5U);}));
if(cyc_varargs != 0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_Tmp0=" inject ";_tag_fat(_Tmp0,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(({struct _tuple8*_Tmp0=_cycalloc(sizeof(struct _tuple8));_Tmp0->f1=cyc_varargs->name,_Tmp0->f2=cyc_varargs->tq,_Tmp0->f3=cyc_varargs->type;_Tmp0;}));}
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
Cyc_Absyndump_dump(({const char*_Tmp0=" @requires(";_tag_fat(_Tmp0,sizeof(char),12U);}));Cyc_Absyndump_dumpexp(req);Cyc_Absyndump_dump_char(41);}
# 529
if(ens != 0){
Cyc_Absyndump_dump(({const char*_Tmp0=" @ensures(";_tag_fat(_Tmp0,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(ens);Cyc_Absyndump_dump_char(41);}}
# 534
static void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int))Cyc_Absyndump_dumptqtd;})(_Tmp0,t,({(void(*)(int))Cyc_Absyndump_ignore;}),0);});}
# 538
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_Tmp0=d;void*_Tmp1;if(*((int*)_Tmp0)== 0){_Tmp1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
Cyc_Absyndump_dump(({const char*_Tmp2=".[";_tag_fat(_Tmp2,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(93);goto _LL0;}}else{_Tmp1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp0)->f1;{struct _fat_ptr*v=_Tmp1;
Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*v);goto _LL0;}}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 544
static void Cyc_Absyndump_dumpde(struct _tuple19*de){
Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f1,({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0="=";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="=";_tag_fat(_Tmp0,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp((*de).f2);}
# 549
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_Tmp0=f;unsigned _Tmp1;void*_Tmp2;if(*((int*)_Tmp0)== 0){_Tmp2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp0)->f1;{struct _fat_ptr*n=_Tmp2;
Cyc_Absyndump_dump_nospace(*n);return;}}else{_Tmp1=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_Tmp0)->f1;{unsigned n=_Tmp1;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)n);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(({const char*_Tmp5="%d";_tag_fat(_Tmp5,sizeof(char),3U);}),_tag_fat(_Tmp4,sizeof(void*),1));}));return;}};}struct _tuple20{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 556
static void Cyc_Absyndump_dump_asm_iolist(struct Cyc_List_List*iolist){
while((unsigned)iolist){
struct _tuple20*_stmttmpB=(struct _tuple20*)iolist->hd;struct _tuple20*_Tmp0=_stmttmpB;void*_Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct _fat_ptr constr=_Tmp2;struct Cyc_Absyn_Exp*e1=_Tmp1;
Cyc_Absyndump_dump_char(34);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(constr));Cyc_Absyndump_dump_char(34);
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);
iolist=iolist->tl;
if((unsigned)iolist)
Cyc_Absyndump_dump_char(44);}}}
# 567
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({(void(*)(void(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group_c;})(Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 571
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_Tmp0="(";_tag_fat(_Tmp0,sizeof(char),2U);}));
{void*_stmttmpC=e->r;void*_Tmp0=_stmttmpC;enum Cyc_Absyn_MallocKind _Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_Primop _Tmp4;void*_Tmp5;long long _Tmp6;int _Tmp7;short _Tmp8;struct _fat_ptr _Tmp9;char _TmpA;enum Cyc_Absyn_Sign _TmpB;switch(*((int*)_Tmp0)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).String_c).tag){case 2: _TmpB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Char_c).val).f1;_TmpA=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Char_c).val).f2;{enum Cyc_Absyn_Sign sg=_TmpB;char ch=_TmpA;
# 577
Cyc_Absyndump_dump_char(39);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_char(39);
goto _LL0;}case 3: _Tmp9=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Wchar_c).val;{struct _fat_ptr s=_Tmp9;
Cyc_Absyndump_dump(({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=s;_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;Cyc_aprintf(({const char*_TmpE="L'%s'";_tag_fat(_TmpE,sizeof(char),6U);}),_tag_fat(_TmpD,sizeof(void*),1));}));goto _LL0;}case 4: _TmpB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Short_c).val).f1;_Tmp8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Short_c).val).f2;{enum Cyc_Absyn_Sign sg=_TmpB;short s=_Tmp8;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)((int)s);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;Cyc_aprintf(({const char*_TmpE="%d";_tag_fat(_TmpE,sizeof(char),3U);}),_tag_fat(_TmpD,sizeof(void*),1));}));goto _LL0;}case 5: switch((int)(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).val).f1){case Cyc_Absyn_None: _Tmp7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).val).f2;{int i=_Tmp7;
_Tmp7=i;goto _LLA;}case Cyc_Absyn_Signed: _Tmp7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).val).f2;_LLA: {int i=_Tmp7;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)i;_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;Cyc_aprintf(({const char*_TmpE="%d";_tag_fat(_TmpE,sizeof(char),3U);}),_tag_fat(_TmpD,sizeof(void*),1));}));goto _LL0;}default: _Tmp7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Int_c).val).f2;{int i=_Tmp7;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned)i;_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;Cyc_aprintf(({const char*_TmpE="%uU";_tag_fat(_TmpE,sizeof(char),4U);}),_tag_fat(_TmpD,sizeof(void*),1));}));goto _LL0;}}case 6: _TmpB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).LongLong_c).val).f1;_Tmp6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).LongLong_c).val).f2;{enum Cyc_Absyn_Sign sg=_TmpB;long long i=_Tmp6;
# 586
Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)i));goto _LL0;}case 7: _Tmp9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Float_c).val).f1;{struct _fat_ptr x=_Tmp9;
Cyc_Absyndump_dump(x);goto _LL0;}case 1:
 Cyc_Absyndump_dump(({const char*_TmpC="NULL";_tag_fat(_TmpC,sizeof(char),5U);}));goto _LL0;case 8: _Tmp9=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).String_c).val;{struct _fat_ptr s=_Tmp9;
# 590
Cyc_Absyndump_dump_char(34);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(s));Cyc_Absyndump_dump_char(34);
goto _LL0;}default: _Tmp9=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).Wstring_c).val;{struct _fat_ptr s=_Tmp9;
# 593
Cyc_Absyndump_dump(({const char*_TmpC="L\"";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char(34);goto _LL0;}}case 1: _Tmp5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp5;
# 595
Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _Tmp9=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_Tmp9;
# 598
Cyc_Absyndump_dump(({const char*_TmpC="__cyclone_pragma__(";_tag_fat(_TmpC,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(p);Cyc_Absyndump_dump_char(41);goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp4;struct Cyc_List_List*es=_Tmp5;
# 601
struct _fat_ptr pstr=Cyc_Absynpp_prim2str(p);
{int _stmttmpD=Cyc_List_length(es);int _TmpC=_stmttmpD;switch((int)_TmpC){case 1:
# 604
 if((int)p == (int)18U){
Cyc_Absyndump_dump(({const char*_TmpD="numelts(";_tag_fat(_TmpD,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)(_check_null(es))->hd);
Cyc_Absyndump_dump(({const char*_TmpD=")";_tag_fat(_TmpD,sizeof(char),2U);}));}else{
# 609
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)(_check_null(es))->hd);}
# 612
goto _LL71;case 2:
# 614
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)(_check_null(es))->hd);
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dump_char(32);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)(_check_null(es->tl))->hd);
goto _LL71;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpD="Absyndump -- Bad number of arguments to primop";_tag_fat(_TmpD,sizeof(char),47U);}),_tag_fat(0U,sizeof(void*),0));}_LL71:;}
# 621
goto _LL0;}case 4: _Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Core_Opt*popt=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 624
Cyc_Absyndump_dumpexp_prec(myprec,e1);
if(popt != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)popt->v));
Cyc_Absyndump_dump_nospace(({const char*_TmpC="=";_tag_fat(_TmpC,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,e2);
goto _LL0;}case 5: switch((int)((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2){case Cyc_Absyn_PreInc: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp5;
# 631
Cyc_Absyndump_dump(({const char*_TmpC="++";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PreDec: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp5;
Cyc_Absyndump_dump(({const char*_TmpC="--";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PostInc: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp5;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_TmpC="++";_tag_fat(_TmpC,sizeof(char),3U);}));goto _LL0;}default: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp5;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_TmpC="--";_tag_fat(_TmpC,sizeof(char),3U);}));goto _LL0;}}case 6: _Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 637
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_char(63);Cyc_Absyndump_dumpexp_prec(0,e2);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp_prec(myprec,e3);
goto _LL0;}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 644
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_TmpC="&&";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 648
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_TmpC="||";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 654
Cyc_Absyndump_dumploc(e->loc);
# 657
Cyc_Absyndump_dumpexp_prec(myprec - 1,e1);Cyc_Absyndump_dump_char(44);Cyc_Absyndump_dumpexp_prec(myprec - 1,e2);
goto _LL0;}case 10: _Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_List_List*es=_Tmp3;
# 661
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_nospace(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));{
# 666
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc == 0U))&& !(e1->loc == 0U);
Cyc_Absyndump_dumpexps_prec(20,es);
Cyc_Absyndump_dump_nospace(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;}}case 11: _Tmp5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 675
Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_TmpC="throw";_tag_fat(_TmpC,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 12: _Tmp5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 677
_Tmp5=e1;goto _LL36;}case 13: _Tmp5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp5;
Cyc_Absyndump_dumpexp_prec(inprec,e1);goto _LL0;}case 14: _Tmp5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 681
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dumpexp_prec(myprec,e1);
goto _LL0;}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 684
Cyc_Absyndump_dump_char(38);Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
Cyc_Absyndump_dump_char(42);Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 16: _Tmp5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*ropt=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 688
Cyc_Absyndump_dump(({const char*_TmpC="new";_tag_fat(_TmpC,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 17: _Tmp5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp5;
Cyc_Absyndump_dump(({const char*_TmpC="sizeof(";_tag_fat(_TmpC,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);goto _LL0;}case 18: _Tmp5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
Cyc_Absyndump_dump(({const char*_TmpC="sizeof(";_tag_fat(_TmpC,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 39: _Tmp5=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp5;
Cyc_Absyndump_dump(({const char*_TmpC="valueof(";_tag_fat(_TmpC,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);goto _LL0;}case 41: _Tmp5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 694
Cyc_Absyndump_dump(({const char*_TmpC="__extension__(";_tag_fat(_TmpC,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 42: _Tmp5=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 696
Cyc_Absyndump_dump(({const char*_TmpC="@assert(";_tag_fat(_TmpC,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 40: _Tmp7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_Tmp2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f5;{int vol=_Tmp7;struct _fat_ptr t=_Tmp9;struct Cyc_List_List*o=_Tmp5;struct Cyc_List_List*i=_Tmp3;struct Cyc_List_List*cl=_Tmp2;
# 699
Cyc_Absyndump_dump(({const char*_TmpC="__asm__";_tag_fat(_TmpC,sizeof(char),8U);}));
if(vol)Cyc_Absyndump_dump(({const char*_TmpC=" volatile ";_tag_fat(_TmpC,sizeof(char),11U);}));
Cyc_Absyndump_dump_nospace(({const char*_TmpC="(\"";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(t));Cyc_Absyndump_dump_char(34);
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
Cyc_Absyndump_dump_nospace(({const char*_TmpC=" :";_tag_fat(_TmpC,sizeof(char),3U);}));}}
while((unsigned)cl){
Cyc_Absyndump_dump_char(34);
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(*((struct _fat_ptr*)cl->hd)));
Cyc_Absyndump_dump_char(34);
cl=cl->tl;
if((unsigned)cl)
Cyc_Absyndump_dump_char(44);}}
# 725
Cyc_Absyndump_dump_char(41);
goto _LL0;}case 38: _Tmp5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp5;struct _fat_ptr*f=_Tmp3;
# 729
Cyc_Absyndump_dump(({const char*_TmpC="tagcheck(";_tag_fat(_TmpC,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*f);
Cyc_Absyndump_dump_char(41);
goto _LL0;}case 19: _Tmp5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp5;struct Cyc_List_List*l=_Tmp3;
# 734
Cyc_Absyndump_dump(({const char*_TmpC="offsetof(";_tag_fat(_TmpC,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_group(Cyc_Absyndump_dumpoffset_field,l,({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=".";_tag_fat(_TmpC,sizeof(char),2U);}));goto _LL0;}case 21: _Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*n=_Tmp3;
# 737
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*n=_Tmp3;
# 739
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_TmpC="->";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 23: _Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 742
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char(91);Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char(93);goto _LL0;}case 24: _Tmp5=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp5;
# 744
Cyc_Absyndump_dump(({const char*_TmpC="$(";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_char(41);goto _LL0;}case 25: _Tmp5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple8*vat=_Tmp5;struct Cyc_List_List*des=_Tmp3;
# 747
Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dumptyp((*vat).f3);
Cyc_Absyndump_dump_char(41);
({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpde,des,({const char*_TmpC="{";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));
goto _LL0;}case 26: _Tmp5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp5;
# 753
({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpde,des,({const char*_TmpC="{";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));goto _LL0;}case 27: _Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 756
Cyc_Absyndump_dump(({const char*_TmpC="{for";_tag_fat(_TmpC,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*vd->name).f2);Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumpexp(e1);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 28: _Tmp5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp5;void*t=_Tmp3;
# 761
Cyc_Absyndump_dump(({const char*_TmpC="{for x ";_tag_fat(_TmpC,sizeof(char),8U);}));Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumpexp(e);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 29: _Tmp5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct _tuple0*n=_Tmp5;struct Cyc_List_List*ts=_Tmp3;struct Cyc_List_List*des=_Tmp2;
# 766
Cyc_Absyndump_dumpqvar(n);
Cyc_Absyndump_dump_char(123);
if(ts != 0)
Cyc_Absyndump_dumptps(ts);
({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpde,des,({const char*_TmpC="";_tag_fat(_TmpC,sizeof(char),1U);}),({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));
goto _LL0;}case 30: _Tmp5=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*des=_Tmp5;
# 773
({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpde,des,({const char*_TmpC="{";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));goto _LL0;}case 32: _Tmp5=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp5;
_Tmp5=ef;goto _LL66;}case 33: _Tmp5=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL66: {struct Cyc_Absyn_Enumfield*ef=_Tmp5;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 31: _Tmp5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp5;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;
# 778
Cyc_Absyndump_dumpqvar(tuf->name);
if(es != 0)({(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpexp,es,({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));
goto _LL0;}case 34: _Tmp1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).mknd;_Tmp5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).elt_type;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;_Tmp7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp1;struct Cyc_Absyn_Exp*ropt=_Tmp5;void**topt=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;int inline_call=_Tmp7;
# 783
Cyc_Absyndump_dumploc(e->loc);
if((int)mknd == 1){
if(ropt != 0){
Cyc_Absyndump_dump(({const char*_TmpC="rcalloc(";_tag_fat(_TmpC,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));}else{
# 788
Cyc_Absyndump_dump(({const char*_TmpC="calloc(";_tag_fat(_TmpC,sizeof(char),8U);}));}
# 790
Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*_check_null(topt),0U));Cyc_Absyndump_dump(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));}else{
# 793
if(ropt != 0){
Cyc_Absyndump_dump((int)mknd == 2?({const char*_TmpC="rvmalloc(";_tag_fat(_TmpC,sizeof(char),10U);}):(inline_call?({const char*_TmpC="rmalloc_inline(";_tag_fat(_TmpC,sizeof(char),16U);}):({const char*_TmpC="rmalloc(";_tag_fat(_TmpC,sizeof(char),9U);})));
Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));}else{
# 797
Cyc_Absyndump_dump(({const char*_TmpC="malloc(";_tag_fat(_TmpC,sizeof(char),8U);}));}
# 800
if(topt != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_TmpC,e,0U);}));else{
# 803
Cyc_Absyndump_dumpexp(e);}
Cyc_Absyndump_dump(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));}
# 806
goto _LL0;}case 35: _Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 809
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_TmpC=":=:";_tag_fat(_TmpC,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 36: _Tmp5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*n=_Tmp5;struct Cyc_List_List*des=_Tmp3;
# 812
({(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpde,des,({const char*_TmpC="{";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC="}";_tag_fat(_TmpC,sizeof(char),2U);}),({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));goto _LL0;}default: _Tmp5=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp5;
# 815
Cyc_Absyndump_dump_nospace(({const char*_TmpC="({";_tag_fat(_TmpC,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s,1,0);Cyc_Absyndump_dump_nospace(({const char*_TmpC="})";_tag_fat(_TmpC,sizeof(char),3U);}));goto _LL0;}}_LL0:;}
# 817
if(inprec >= myprec)
Cyc_Absyndump_dump_char(41);}
# 821
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 825
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs,struct Cyc_List_List*varsinblock){
# 827
for(1;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*c=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_Tmp0="default:";_tag_fat(_Tmp0,sizeof(char),9U);}));else{
# 832
Cyc_Absyndump_dump(({const char*_Tmp0="case";_tag_fat(_Tmp0,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(c->pattern);
if(c->where_clause != 0){
Cyc_Absyndump_dump(({const char*_Tmp0="&&";_tag_fat(_Tmp0,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp(_check_null(c->where_clause));}
# 838
Cyc_Absyndump_dump_nospace(({const char*_Tmp0=":";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 840
if(Cyc_Absynpp_is_declaration(c->body)){
Cyc_Absyndump_dump(({const char*_Tmp0=" {";_tag_fat(_Tmp0,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(c->body,0,0),Cyc_Absyndump_dump_char(125);}else{
# 843
Cyc_Absyndump_dumpstmt(c->body,0,varsinblock);}}}
# 847
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt,struct Cyc_List_List*varsinblock){
Cyc_Absyndump_dumploc(s->loc);{
void*_stmttmpE=s->r;void*_Tmp0=_stmttmpE;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dumpexp_prec(-100,e);Cyc_Absyndump_dump_semi();goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 856
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);}else{
# 859
Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}
if(Cyc_Absynpp_is_declaration(s2)){
if(expstmt)Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpstmt(s2,expstmt,0);Cyc_Absyndump_dump_char(125);
if(expstmt){Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dump_semi();}}else{
# 865
Cyc_Absyndump_dumpstmt(s2,expstmt,varsinblock);}
goto _LL0;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1 == 0){
Cyc_Absyndump_dump(({const char*_Tmp5="return;";_tag_fat(_Tmp5,sizeof(char),8U);}));goto _LL0;}else{_Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dump(({const char*_Tmp5="return";_tag_fat(_Tmp5,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(_check_null(e));Cyc_Absyndump_dump_semi();goto _LL0;}}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 870
Cyc_Absyndump_dump(({const char*_Tmp5="if(";_tag_fat(_Tmp5,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(e);
{void*_stmttmpF=s1->r;void*_Tmp5=_stmttmpF;switch(*((int*)_Tmp5)){case 2:
 goto _LL29;case 12: _LL29:
 goto _LL2B;case 4: _LL2B:
 goto _LL2D;case 13: _LL2D:
# 876
 Cyc_Absyndump_dump_nospace(({const char*_Tmp6="){";_tag_fat(_Tmp6,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);goto _LL25;default:
 Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}_LL25:;}
# 879
{void*_stmttmp10=s2->r;void*_Tmp5=_stmttmp10;if(*((int*)_Tmp5)== 0)
goto _LL30;else{
Cyc_Absyndump_dump(({const char*_Tmp6="else{";_tag_fat(_Tmp6,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(s2,0,0);Cyc_Absyndump_dump_char(125);goto _LL30;}_LL30:;}
# 883
goto _LL0;}case 5: _Tmp4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1).f1;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 886
Cyc_Absyndump_dump(({const char*_Tmp5="while(";_tag_fat(_Tmp5,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_Tmp5="){";_tag_fat(_Tmp5,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;_Tmp2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3).f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 890
Cyc_Absyndump_dump(({const char*_Tmp5="for(";_tag_fat(_Tmp5,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e3);
Cyc_Absyndump_dump_nospace(({const char*_Tmp5="){";_tag_fat(_Tmp5,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 894
Cyc_Absyndump_dump(({const char*_Tmp5="do{";_tag_fat(_Tmp5,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,0);
Cyc_Absyndump_dump_nospace(({const char*_Tmp5="}while(";_tag_fat(_Tmp5,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_Tmp5=");";_tag_fat(_Tmp5,sizeof(char),3U);}));
goto _LL0;}case 6:
# 898
 Cyc_Absyndump_dump(({const char*_Tmp5="break;";_tag_fat(_Tmp5,sizeof(char),7U);}));goto _LL0;case 7:
 Cyc_Absyndump_dump(({const char*_Tmp5="continue;";_tag_fat(_Tmp5,sizeof(char),10U);}));goto _LL0;case 8: _Tmp4=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct _fat_ptr*x=_Tmp4;
Cyc_Absyndump_dump(({const char*_Tmp5="goto";_tag_fat(_Tmp5,sizeof(char),5U);}));Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_semi();goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 903
Cyc_Absyndump_dump(({const char*_Tmp5="switch(";_tag_fat(_Tmp5,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_Tmp5="){";_tag_fat(_Tmp5,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);
Cyc_Absyndump_dump_char(125);
goto _LL0;}case 15: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 908
Cyc_Absyndump_dump(({const char*_Tmp5="try";_tag_fat(_Tmp5,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,varsinblock);
Cyc_Absyndump_dump(({const char*_Tmp5="catch{";_tag_fat(_Tmp5,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 914
struct _tuple13 _stmttmp11=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple13 _Tmp5=_stmttmp11;void*_Tmp6;int _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{int newblock=_Tmp7;struct Cyc_List_List*newvarsinblock=_Tmp6;
if(newblock){
if(expstmt)Cyc_Absyndump_dump(({const char*_Tmp8="({";_tag_fat(_Tmp8,sizeof(char),3U);}));else{Cyc_Absyndump_dump(({const char*_Tmp8="{";_tag_fat(_Tmp8,sizeof(char),2U);}));}
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_Tmp8="});";_tag_fat(_Tmp8,sizeof(char),4U);}));else{Cyc_Absyndump_dump(({const char*_Tmp8="}";_tag_fat(_Tmp8,sizeof(char),2U);}));}}else{
# 921
Cyc_Absyndump_dumpdecl(d);Cyc_Absyndump_dumpstmt(s1,expstmt,newvarsinblock);}
# 923
goto _LL0;}}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*x=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 929
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_str(x);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_Tmp5=": ({";_tag_fat(_Tmp5,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_Tmp5=": {";_tag_fat(_Tmp5,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_Tmp5="});";_tag_fat(_Tmp5,sizeof(char),4U);}));else{Cyc_Absyndump_dump_char(125);}}else{
# 935
Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpstmt(s1,expstmt,varsinblock);}
# 937
goto _LL0;}default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1 == 0){
# 939
Cyc_Absyndump_dump(({const char*_Tmp5="fallthru;";_tag_fat(_Tmp5,sizeof(char),10U);}));goto _LL0;}else{_Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 941
Cyc_Absyndump_dump(({const char*_Tmp5="fallthru(";_tag_fat(_Tmp5,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_nospace(({const char*_Tmp5=");";_tag_fat(_Tmp5,sizeof(char),3U);}));goto _LL0;}}}_LL0:;}}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 945
static void Cyc_Absyndump_dumpdp(struct _tuple21*dp){
Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0="=";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0="=";_tag_fat(_Tmp0,sizeof(char),2U);}));
Cyc_Absyndump_dumppat((*dp).f2);}
# 949
static struct _fat_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_Tmp0="...)";_tag_fat(_Tmp0,sizeof(char),5U);}):({const char*_Tmp0=")";_tag_fat(_Tmp0,sizeof(char),2U);});}
# 951
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_stmttmp12=p->r;void*_Tmp0=_stmttmp12;union Cyc_Absyn_AggrInfo _Tmp1;void*_Tmp2;char _Tmp3;void*_Tmp4;struct _fat_ptr _Tmp5;int _Tmp6;switch(*((int*)_Tmp0)){case 0:
 Cyc_Absyndump_dump_char(95);goto _LL0;case 9:
 Cyc_Absyndump_dump(({const char*_Tmp7="NULL";_tag_fat(_Tmp7,sizeof(char),5U);}));goto _LL0;case 10: switch((int)((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1){case Cyc_Absyn_None: _Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{int i=_Tmp6;
_Tmp6=i;goto _LL8;}case Cyc_Absyn_Signed: _Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL8: {int i=_Tmp6;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="%d";_tag_fat(_Tmp9,sizeof(char),3U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));goto _LL0;}default: _Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{int i=_Tmp6;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)i;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="%u";_tag_fat(_Tmp9,sizeof(char),3U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));goto _LL0;}}case 12: _Tmp5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr x=_Tmp5;
Cyc_Absyndump_dump(x);goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp4;
_Tmp4=ef;goto _LL10;}case 14: _Tmp4=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Enumfield*ef=_Tmp4;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 11: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char ch=_Tmp3;
# 962
Cyc_Absyndump_dump(({const char*_Tmp7="'";_tag_fat(_Tmp7,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_nospace(({const char*_Tmp7="'";_tag_fat(_Tmp7,sizeof(char),2U);}));goto _LL0;}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)== 0){_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
Cyc_Absyndump_dump(({const char*_Tmp7="*";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp4=vd;goto _LL16;}}else{_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 965
Cyc_Absyndump_dump(({const char*_Tmp7="*";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp4=vd;_Tmp2=p2;goto _LL1A;}}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)== 0){_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 964
Cyc_Absyndump_dumpqvar(vd->name);goto _LL0;}}else{_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 966
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump(({const char*_Tmp7=" as ";_tag_fat(_Tmp7,sizeof(char),5U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}}case 2: _Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 968
Cyc_Absyndump_dump(({const char*_Tmp7="alias";_tag_fat(_Tmp7,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_Tmp7="<";_tag_fat(_Tmp7,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_Tmp7=">";_tag_fat(_Tmp7,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(vd,p->loc,0);
goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp4;int dots=_Tmp6;
({struct Cyc_List_List*_Tmp7=ts;struct _fat_ptr _Tmp8=({const char*_Tmp9="$(";_tag_fat(_Tmp9,sizeof(char),3U);});struct _fat_ptr _Tmp9=Cyc_Absyndump_pat_term(dots);({(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumppat,_Tmp7,_Tmp8,_Tmp9,({const char*_TmpA=",";_tag_fat(_TmpA,sizeof(char),2U);}));});goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp4;
Cyc_Absyndump_dump(({const char*_Tmp7="&";_tag_fat(_Tmp7,sizeof(char),2U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 974
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump_char(62);goto _LL0;}case 15: _Tmp4=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*q=_Tmp4;
Cyc_Absyndump_dumpqvar(q);goto _LL0;}case 16: _Tmp4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct _tuple0*q=_Tmp4;struct Cyc_List_List*ps=_Tmp2;int dots=_Tmp6;
# 977
Cyc_Absyndump_dumpqvar(q);({struct Cyc_List_List*_Tmp7=ps;struct _fat_ptr _Tmp8=({const char*_Tmp9="(";_tag_fat(_Tmp9,sizeof(char),2U);});struct _fat_ptr _Tmp9=Cyc_Absyndump_pat_term(dots);({(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumppat,_Tmp7,_Tmp8,_Tmp9,({const char*_TmpA=",";_tag_fat(_TmpA,sizeof(char),2U);}));});goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1 != 0){_Tmp1=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{union Cyc_Absyn_AggrInfo info=_Tmp1;struct Cyc_List_List*exists=_Tmp4;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp6;
# 979
Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(info)).f2);
_Tmp4=exists;_Tmp2=dps;_Tmp6=dots;goto _LL2A;}}else{_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_LL2A: {struct Cyc_List_List*exists=_Tmp4;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp6;
# 982
Cyc_Absyndump_dump_char(123);
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;})(Cyc_Absyndump_dumptvar,exists,({const char*_Tmp7="<";_tag_fat(_Tmp7,sizeof(char),2U);}),({const char*_Tmp7=">";_tag_fat(_Tmp7,sizeof(char),2U);}),({const char*_Tmp7=",";_tag_fat(_Tmp7,sizeof(char),2U);}));
({struct Cyc_List_List*_Tmp7=dps;struct _fat_ptr _Tmp8=({const char*_Tmp9="";_tag_fat(_Tmp9,sizeof(char),1U);});struct _fat_ptr _Tmp9=Cyc_Absyndump_pat_term(dots);({(void(*)(void(*)(struct _tuple21*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpdp,_Tmp7,_Tmp8,_Tmp9,({const char*_TmpA=",";_tag_fat(_TmpA,sizeof(char),2U);}));});
goto _LL0;}}case 8: _Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;struct Cyc_List_List*ps=_Tmp2;int dots=_Tmp6;
# 987
Cyc_Absyndump_dumpqvar(tuf->name);
if(ps != 0)({struct Cyc_List_List*_Tmp7=ps;struct _fat_ptr _Tmp8=({const char*_Tmp9="(";_tag_fat(_Tmp9,sizeof(char),2U);});struct _fat_ptr _Tmp9=Cyc_Absyndump_pat_term(dots);({(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumppat,_Tmp7,_Tmp8,_Tmp9,({const char*_TmpA=",";_tag_fat(_TmpA,sizeof(char),2U);}));});
goto _LL0;}default: _Tmp4=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dumpexp(e);goto _LL0;}}_LL0:;}
# 994
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 999
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({(void(*)(void(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;})(Cyc_Absyndump_dumpdatatypefield,fields,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 1002
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_Tmp0="=";_tag_fat(_Tmp0,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(_check_null(ef->tag));}}
# 1008
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({(void(*)(void(*)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;})(Cyc_Absyndump_dumpenumfield,fields,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));}
# 1012
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(1;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp13=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_Tmp0=_stmttmp13;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;_Tmp6=_Tmp0->name;_Tmp5=_Tmp0->tq;_Tmp4=_Tmp0->type;_Tmp3=_Tmp0->width;_Tmp2=_Tmp0->attributes;_Tmp1=_Tmp0->requires_clause;{struct _fat_ptr*name=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*type=_Tmp4;struct Cyc_Absyn_Exp*width=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*req=_Tmp1;
{enum Cyc_Flags_C_Compilers _Tmp7=Cyc_Flags_c_compiler;if(_Tmp7 == Cyc_Flags_Gcc_c){
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;})(tq,type,Cyc_Absyndump_dump_str,name);Cyc_Absyndump_dumpatts(atts);goto _LL3;}else{
Cyc_Absyndump_dumpatts(atts);({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;})(tq,type,Cyc_Absyndump_dump_str,name);goto _LL3;}_LL3:;}
# 1019
if((unsigned)req){
Cyc_Absyndump_dump(({const char*_Tmp7="@requires ";_tag_fat(_Tmp7,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(req);}
# 1022
if(width != 0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp(width);}
# 1025
Cyc_Absyndump_dump_semi();}}}
# 1029
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1034
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple22{void*f1;struct _tuple0*f2;};
# 1038
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple22*pr){
{void*_stmttmp14=(*pr).f1;void*_Tmp0=_stmttmp14;switch(*((int*)_Tmp0)){case 11:
 goto _LL0;case 1:
 Cyc_Absyndump_dump(({const char*_Tmp1="_stdcall";_tag_fat(_Tmp1,sizeof(char),9U);}));goto _LL0;case 2:
 Cyc_Absyndump_dump(({const char*_Tmp1="_cdecl";_tag_fat(_Tmp1,sizeof(char),7U);}));goto _LL0;case 3:
 Cyc_Absyndump_dump(({const char*_Tmp1="_fastcall";_tag_fat(_Tmp1,sizeof(char),10U);}));goto _LL0;default:
 goto _LL0;}_LL0:;}
# 1046
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1048
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1052
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1056
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1058
void*_Tmp0=t;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;if(_Tmp1 != 0){void*def=_Tmp1;
return Cyc_Absyndump_is_char_ptr(def);}else{goto _LL5;}case 3: _Tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;{void*elt_typ=_Tmp1;
# 1061
while(1){
void*_Tmp2=elt_typ;void*_Tmp3;switch(*((int*)_Tmp2)){case 1: _Tmp3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f2;if(_Tmp3 != 0){void*t=_Tmp3;
elt_typ=t;goto _LL7;}else{goto _LLE;}case 8: _Tmp3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f4;if(_Tmp3 != 0){void*t=_Tmp3;
elt_typ=t;goto _LL7;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f2 == Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;}_LL7:;}}default: _LL5:
# 1068
 return 0;};}
# 1072
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
struct Cyc_Absyn_Vardecl*_Tmp0=vd;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;unsigned _Tmp6;void*_Tmp7;enum Cyc_Absyn_Scope _Tmp8;_Tmp8=_Tmp0->sc;_Tmp7=_Tmp0->name;_Tmp6=_Tmp0->varloc;_Tmp5=_Tmp0->tq;_Tmp4=_Tmp0->type;_Tmp3=_Tmp0->initializer;_Tmp2=_Tmp0->attributes;_Tmp1=_Tmp0->is_proto;{enum Cyc_Absyn_Scope sc=_Tmp8;struct _tuple0*name=_Tmp7;unsigned varloc=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*type=_Tmp4;struct Cyc_Absyn_Exp*initializer=_Tmp3;struct Cyc_List_List*atts=_Tmp2;int is_proto=_Tmp1;
{enum Cyc_Flags_C_Compilers _Tmp9=Cyc_Flags_c_compiler;if(_Tmp9 == Cyc_Flags_Gcc_c){
# 1077
if((int)sc == 3 && Cyc_Absyndump_qvar_to_Cids){
void*_stmttmp15=Cyc_Absyn_compress(type);void*_TmpA=_stmttmp15;if(*((int*)_TmpA)== 5){
# 1080
if(!is_proto)
Cyc_Absyndump_dumpscope(sc);
goto _LL8;}else{
Cyc_Absyndump_dumpscope(sc);}_LL8:;}else{
# 1086
Cyc_Absyndump_dumpscope(sc);}
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd;})(tq,type,Cyc_Absyndump_dumpqvar,name);
Cyc_Absyndump_dumpatts(atts);
goto _LL3;}else{
# 1091
Cyc_Absyndump_dumpatts(atts);
Cyc_Absyndump_dumpscope(sc);{
struct _RegionHandle _TmpA=_new_region("temp");struct _RegionHandle*temp=& _TmpA;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(type);
struct _tuple12 _stmttmp16=Cyc_Absynpp_to_tms(temp,tq,type);struct _tuple12 _TmpB=_stmttmp16;void*_TmpC;void*_TmpD;struct Cyc_Absyn_Tqual _TmpE;_TmpE=_TmpB.f1;_TmpD=_TmpB.f2;_TmpC=_TmpB.f3;{struct Cyc_Absyn_Tqual tq=_TmpE;void*t=_TmpD;struct Cyc_List_List*tms=_TmpC;
# 1097
void*call_conv=(void*)& Cyc_Atts_Unused_att_val;
{struct Cyc_List_List*tms2=tms;for(0;tms2 != 0;tms2=tms2->tl){
void*_stmttmp17=(void*)tms2->hd;void*_TmpF=_stmttmp17;void*_Tmp10;if(*((int*)_TmpF)== 5){_Tmp10=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_TmpF)->f2;{struct Cyc_List_List*atts=_Tmp10;
# 1101
for(1;atts != 0;atts=atts->tl){
void*_stmttmp18=(void*)atts->hd;void*_Tmp11=_stmttmp18;switch(*((int*)_Tmp11)){case 1:
 call_conv=(void*)& Cyc_Atts_Stdcall_att_val;goto _LL15;case 2:
 call_conv=(void*)& Cyc_Atts_Cdecl_att_val;goto _LL15;case 3:
 call_conv=(void*)& Cyc_Atts_Fastcall_att_val;goto _LL15;default:
 goto _LL15;}_LL15:;}
# 1108
goto _LL10;}}else{
goto _LL10;}_LL10:;}}
# 1111
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);{
struct _tuple22 pr=({struct _tuple22 _TmpF;_TmpF.f1=call_conv,_TmpF.f2=name;_TmpF;});
({int _TmpF=is_cp;({(void(*)(int,struct Cyc_List_List*,void(*)(struct _tuple22*),struct _tuple22*))Cyc_Absyndump_dumptms;})(_TmpF,Cyc_List_imp_rev(tms),Cyc_Absyndump_dump_callconv_qvar,& pr);});
_npop_handler(0);goto _LL3;}}}
# 1094
;_pop_region();}}_LL3:;}
# 1118
if(initializer != 0){
Cyc_Absyndump_dump_char(61);Cyc_Absyndump_dumpexp(initializer);}
# 1121
if(do_semi)Cyc_Absyndump_dump_semi();}}
# 1124
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0 &&(_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_Tmp0="@tagged ";_tag_fat(_Tmp0,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char(123);
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;})(Cyc_Absyndump_dumpkindedtvar,(_check_null(ad->impl))->exist_vars,({const char*_Tmp0="<";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=">";_tag_fat(_Tmp0,sizeof(char),2U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}));
if((_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dump_rgnpo((_check_null(ad->impl))->rgn_po);}
# 1135
Cyc_Absyndump_dumpaggrfields((_check_null(ad->impl))->fields);
Cyc_Absyndump_dump_char(125);
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1140
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_Tmp0=ed;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Scope _Tmp3;_Tmp3=_Tmp0->sc;_Tmp2=_Tmp0->name;_Tmp1=_Tmp0->fields;{enum Cyc_Absyn_Scope sc=_Tmp3;struct _tuple0*nm=_Tmp2;struct Cyc_Core_Opt*fields=_Tmp1;
Cyc_Absyndump_dumpscope(sc);Cyc_Absyndump_dump(({const char*_Tmp4="enum ";_tag_fat(_Tmp4,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(nm);
if(fields != 0){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char(125);}}}
# 1147
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_Tmp0=dd;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Scope _Tmp5;_Tmp5=_Tmp0->sc;_Tmp4=_Tmp0->name;_Tmp3=_Tmp0->tvs;_Tmp2=_Tmp0->fields;_Tmp1=_Tmp0->is_extensible;{enum Cyc_Absyn_Scope sc=_Tmp5;struct _tuple0*name=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_Core_Opt*fields=_Tmp2;int is_x=_Tmp1;
Cyc_Absyndump_dumpscope(sc);
if(is_x)
Cyc_Absyndump_dump(({const char*_Tmp6="@extensible ";_tag_fat(_Tmp6,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_Tmp6="datatype ";_tag_fat(_Tmp6,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(name);Cyc_Absyndump_dumptvars(tvs);
if(fields != 0){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char(125);}}}struct _tuple23{unsigned f1;struct _tuple0*f2;int f3;};
# 1158
static void Cyc_Absyndump_dumpexport(struct _tuple23*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1161
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char(123);
for(1;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char(125);}
# 1168
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_stmttmp19=d->r;void*_Tmp0=_stmttmp19;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1172
{enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;if(_Tmp5 == Cyc_Flags_Vc_c){
Cyc_Absyndump_dumpatts((fd->i).attributes);goto _LL23;}else{
goto _LL23;}_LL23:;}
# 1176
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;if(_Tmp5 == Cyc_Flags_Vc_c){
Cyc_Absyndump_dump(({const char*_Tmp6="__inline";_tag_fat(_Tmp6,sizeof(char),9U);}));goto _LL28;}else{
Cyc_Absyndump_dump(({const char*_Tmp6="inline";_tag_fat(_Tmp6,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1181
Cyc_Absyndump_dumpscope(fd->sc);{
# 1183
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=5,_Tmp5->f1=type_info;_Tmp5;});
if(fd->cached_type != 0){
void*_stmttmp1A=Cyc_Absyn_compress(_check_null(fd->cached_type));void*_Tmp5=_stmttmp1A;struct Cyc_Absyn_FnInfo _Tmp6;if(*((int*)_Tmp5)== 5){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp6;
# 1189
({struct Cyc_List_List*_Tmp7=Cyc_List_append((fd->i).attributes,i.attributes);(fd->i).attributes=_Tmp7;});goto _LL2D;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp7="function has non-function type";_tag_fat(_Tmp7,sizeof(char),31U);}),_tag_fat(0U,sizeof(void*),0));}_LL2D:;}
# 1193
{enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;if(_Tmp5 == Cyc_Flags_Gcc_c){
({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=t;({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;})(_Tmp6,_Tmp7,Cyc_Absyndump_dump_atts_qvar,fd);});goto _LL32;}else{
({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=t;({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;})(_Tmp6,_Tmp7,Cyc_Absyndump_dump_callconv_fdqvar,fd);});goto _LL32;}_LL32:;}
# 1197
Cyc_Absyndump_dump_char(123);
Cyc_Absyndump_dumpstmt(fd->body,0,0);
Cyc_Absyndump_dump_char(125);
goto _LL0;}}}case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
Cyc_Absyndump_dump_aggrdecl(ad);Cyc_Absyndump_dump_semi();goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump_semi();goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump_semi();goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp4;
({(void(*)(void(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dumpid,vds,({const char*_Tmp5=" let ";_tag_fat(_Tmp5,sizeof(char),6U);}),({const char*_Tmp5=";";_tag_fat(_Tmp5,sizeof(char),2U);}),({const char*_Tmp5=",";_tag_fat(_Tmp5,sizeof(char),2U);}));goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1207
Cyc_Absyndump_dump(({const char*_Tmp5="let";_tag_fat(_Tmp5,sizeof(char),4U);}));Cyc_Absyndump_dumppat(p);Cyc_Absyndump_dump_char(61);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp2;
# 1209
Cyc_Absyndump_dump(({const char*_Tmp5="region";_tag_fat(_Tmp5,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_Tmp5="<";_tag_fat(_Tmp5,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_Tmp5="> ";_tag_fat(_Tmp5,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(vd->name);
if((unsigned)open_exp_opt){
Cyc_Absyndump_dump(({const char*_Tmp5=" = open(";_tag_fat(_Tmp5,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(open_exp_opt);Cyc_Absyndump_dump(({const char*_Tmp5=")";_tag_fat(_Tmp5,sizeof(char),2U);}));}
# 1213
Cyc_Absyndump_dump_semi();
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 1220
Cyc_Absyndump_dump(({const char*_Tmp5="typedef";_tag_fat(_Tmp5,sizeof(char),8U);}));{
void*t=td->defn == 0?Cyc_Absyn_new_evar(td->kind,0): _check_null(td->defn);
({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd;})(td->tq,t,Cyc_Absyndump_dumptypedefname,td);
Cyc_Absyndump_dumpatts(td->atts);
Cyc_Absyndump_dump_semi();
# 1226
goto _LL0;}}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1228
Cyc_Absyndump_dump(({const char*_Tmp5="namespace";_tag_fat(_Tmp5,sizeof(char),10U);}));Cyc_Absyndump_dump_str(v);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple0*q=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1230
Cyc_Absyndump_dump(({const char*_Tmp5="using";_tag_fat(_Tmp5,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(q);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*tdl=_Tmp4;
# 1232
Cyc_Absyndump_dump(({const char*_Tmp5="extern \"C\" ";_tag_fat(_Tmp5,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*tdl=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple10*wc=_Tmp1;
# 1234
Cyc_Absyndump_dump(({const char*_Tmp5="extern \"C include\" ";_tag_fat(_Tmp5,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(tdl);
if(ovrs != 0){
Cyc_Absyndump_dump(({const char*_Tmp5=" cyclone_override ";_tag_fat(_Tmp5,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(ovrs);}
# 1239
if((unsigned)wc){
Cyc_Absyndump_dump(({const char*_Tmp5=" export { * }\n";_tag_fat(_Tmp5,sizeof(char),15U);}));{
struct _tuple10*_Tmp5=wc;void*_Tmp6;_Tmp6=_Tmp5->f2;{struct Cyc_List_List*hides=_Tmp6;
if((unsigned)hides)
({(void(*)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;})(Cyc_Absyndump_dumpqvar,hides,({const char*_Tmp7=" cyclone_hide {";_tag_fat(_Tmp7,sizeof(char),16U);}),({const char*_Tmp7="}";_tag_fat(_Tmp7,sizeof(char),2U);}),({const char*_Tmp7=",";_tag_fat(_Tmp7,sizeof(char),2U);}));}}}else{
# 1247
({(void(*)(void(*)(struct _tuple23*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;})(Cyc_Absyndump_dumpexport,exs,({const char*_Tmp5=" export {";_tag_fat(_Tmp5,sizeof(char),10U);}),({const char*_Tmp5="}";_tag_fat(_Tmp5,sizeof(char),2U);}),({const char*_Tmp5=",";_tag_fat(_Tmp5,sizeof(char),2U);}));}
# 1249
goto _LL0;}case 13:
 Cyc_Absyndump_dump(({const char*_Tmp5=" __cyclone_port_on__; ";_tag_fat(_Tmp5,sizeof(char),23U);}));goto _LL0;case 14:
 Cyc_Absyndump_dump(({const char*_Tmp5=" __cyclone_port_off__; ";_tag_fat(_Tmp5,sizeof(char),24U);}));goto _LL0;case 15:
 Cyc_Absyndump_dump(({const char*_Tmp5=" __tempest_on__; ";_tag_fat(_Tmp5,sizeof(char),18U);}));goto _LL0;default:
 Cyc_Absyndump_dump(({const char*_Tmp5=" __tempest_off__; ";_tag_fat(_Tmp5,sizeof(char),19U);}));goto _LL0;}_LL0:;}}
# 1257
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple14 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1U || !pr.f2){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(125);}}
# 1264
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1266
if(tms == 0){
f(a);
return;}{
# 1270
void*_stmttmp1B=(void*)tms->hd;void*_Tmp0=_stmttmp1B;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;if(*((int*)_Tmp0)== 2){_Tmp6=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).rgn;_Tmp5=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).nullable;_Tmp4=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).bounds;_Tmp3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).zero_term;_Tmp2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).autoreleased;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*rgn=_Tmp6;void*nullable=_Tmp5;void*bd=_Tmp4;void*zt=_Tmp3;void*rel=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 1274
{void*_stmttmp1C=Cyc_Absyn_compress(bd);void*_Tmp7=_stmttmp1C;void*_Tmp8;if(*((int*)_Tmp7)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f1)){case 14:
 Cyc_Absyndump_dump_char(63);goto _LL5;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f2)->hd)== 9){_Tmp8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f2)->hd)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1277
Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));Cyc_Absyndump_dump_upperbound(e);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA:
# 1279
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));}_LL5:;}
# 1281
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_Tmp7="@zeroterm";_tag_fat(_Tmp7,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_Tmp7="@nozeroterm";_tag_fat(_Tmp7,sizeof(char),12U);}));
if(!Cyc_Absyndump_qvar_to_Cids && Cyc_Absyn_type2bool(0,rel))Cyc_Absyndump_dump(({const char*_Tmp7="@autoreleased";_tag_fat(_Tmp7,sizeof(char),14U);}));
{void*_stmttmp1D=Cyc_Absyn_compress(rgn);void*_Tmp7=_stmttmp1D;void*_Tmp8;switch(*((int*)_Tmp7)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f1)){case 5:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_Tmp9="`H";_tag_fat(_Tmp9,sizeof(char),3U);}));goto _LLC;case 6:
 Cyc_Absyndump_dump(({const char*_Tmp9="`U";_tag_fat(_Tmp9,sizeof(char),3U);}));goto _LLC;case 7:
 Cyc_Absyndump_dump(({const char*_Tmp9="`RC";_tag_fat(_Tmp9,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2: _Tmp8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp8;
Cyc_Absyndump_dumptvar(tv);goto _LLC;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp7)->f2 == 0){
Cyc_Absyndump_dumpntyp(Cyc_Absyn_compress(rgn));goto _LLC;}else{goto _LL19;}case 8:
 Cyc_Absyndump_dump(({const char*_Tmp9="@region(";_tag_fat(_Tmp9,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(rgn);Cyc_Absyndump_dump(({const char*_Tmp9=")";_tag_fat(_Tmp9,sizeof(char),2U);}));goto _LLC;default: _LL19:
 Cyc_Warn_impos(({const char*_Tmp9="dumptms: bad rgn type in Pointer_mod";_tag_fat(_Tmp9,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));}_LLC:;}
# 1293
Cyc_Absyndump_dumptq(tq2);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}}else{
# 1298
int next_is_pointer=0;
if(tms->tl != 0){
void*_stmttmp1E=(void*)(_check_null(tms->tl))->hd;void*_Tmp7=_stmttmp1E;if(*((int*)_Tmp7)== 2){
next_is_pointer=1;goto _LL1B;}else{
goto _LL1B;}_LL1B:;}
# 1304
if(next_is_pointer)
Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char(41);
{void*_stmttmp1F=(void*)tms->hd;void*_Tmp7=_stmttmp1F;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;int _TmpC;void*_TmpD;void*_TmpE;switch(*((int*)_Tmp7)){case 0: _TmpE=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1;{void*zeroterm=_TmpE;
# 1311
Cyc_Absyndump_dump(({const char*_TmpF="[]";_tag_fat(_TmpF,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_TmpF="@zeroterm";_tag_fat(_TmpF,sizeof(char),10U);}));
goto _LL20;}case 1: _TmpE=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1;_TmpD=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*e=_TmpE;void*zeroterm=_TmpD;
# 1315
Cyc_Absyndump_dump_char(91);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(93);
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_TmpF="@zeroterm";_tag_fat(_TmpF,sizeof(char),10U);}));
goto _LL20;}case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)== 1){_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f1;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f2;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f3;_TmpB=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f4;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f5;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f6;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f7;{struct Cyc_List_List*args=_TmpE;int c_varargs=_TmpC;struct Cyc_Absyn_VarargInfo*cyc_varargs=_TmpD;void*effopt=_TmpB;struct Cyc_List_List*rgn_po=_TmpA;struct Cyc_Absyn_Exp*req=_Tmp9;struct Cyc_Absyn_Exp*ens=_Tmp8;
# 1319
Cyc_Absyndump_dumpfunargs(args,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);goto _LL20;}}else{_TmpE=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1)->f1;{struct Cyc_List_List*sl=_TmpE;
# 1321
({(void(*)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;})(Cyc_Absyndump_dump_str,sl,({const char*_TmpF="(";_tag_fat(_TmpF,sizeof(char),2U);}),({const char*_TmpF=")";_tag_fat(_TmpF,sizeof(char),2U);}),({const char*_TmpF=",";_tag_fat(_TmpF,sizeof(char),2U);}));goto _LL20;}}case 4: _TmpE=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp7)->f1;_TmpC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp7)->f3;{struct Cyc_List_List*ts=_TmpE;int print_kinds=_TmpC;
# 1323
if(print_kinds)Cyc_Absyndump_dumpkindedtvars(ts);else{Cyc_Absyndump_dumptvars(ts);}goto _LL20;}case 5: _TmpE=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp7)->f2;{struct Cyc_List_List*atts=_TmpE;
Cyc_Absyndump_dumpatts(atts);goto _LL20;}default:
 Cyc_Warn_impos(({const char*_TmpF="dumptms";_tag_fat(_TmpF,sizeof(char),8U);}),_tag_fat(0U,sizeof(void*),0));}_LL20:;}
# 1327
return;};}}
# 1331
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int cp=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct _tuple12 _stmttmp20=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple12 _Tmp1=_stmttmp20;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;struct Cyc_List_List*tms=_Tmp2;
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);
({int _Tmp5=cp;struct Cyc_List_List*_Tmp6=Cyc_List_imp_rev(tms);void(*_Tmp7)(void*)=f;Cyc_Absyndump_dumptms(_Tmp5,_Tmp6,_Tmp7,a);});}}
# 1334
;_pop_region();}
# 1340
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1342
*Cyc_Absyndump_dump_file=f;
for(1;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_fprintf(f,({const char*_Tmp0="\n";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 1348
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_stmttmp21=d->r;void*_Tmp0=_stmttmp21;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 1351
if((int)vd->sc == 0)return;{
struct Cyc_Absyn_Exp*init=vd->initializer;
vd->initializer=0;
if((int)vd->sc == 2)
Cyc_Absyndump_dump(({const char*_Tmp5="extern ";_tag_fat(_Tmp5,sizeof(char),8U);}));else{
if(!vd->is_proto){
void*_stmttmp22=Cyc_Absyn_compress(vd->type);void*_Tmp5=_stmttmp22;if(*((int*)_Tmp5)== 5){
Cyc_Absyndump_dump(({const char*_Tmp6="extern ";_tag_fat(_Tmp6,sizeof(char),8U);}));goto _LL17;}else{
goto _LL17;}_LL17:;}}
# 1361
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);
Cyc_Absyndump_dump(({const char*_Tmp5="\n";_tag_fat(_Tmp5,sizeof(char),2U);}));
vd->initializer=init;
goto _LL0;}}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1366
if((int)fd->sc == 0)return;
Cyc_Absyndump_dumpscope(fd->sc);{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=5,_Tmp5->f1=type_info;_Tmp5;});
({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);void*_Tmp6=t;({(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;})(_Tmp5,_Tmp6,Cyc_Absyndump_dump_atts_qvar,fd);});
Cyc_Absyndump_dump(({const char*_Tmp5=";\n";_tag_fat(_Tmp5,sizeof(char),3U);}));
goto _LL0;}}}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 1375
if((int)ad->sc == 0)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if((int)ad->sc == 1)ad->impl=0;
Cyc_Absyndump_dump_aggrdecl(ad);
ad->impl=impl;
Cyc_Absyndump_dump(({const char*_Tmp5=";\n";_tag_fat(_Tmp5,sizeof(char),3U);}));
goto _LL0;}}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump(({const char*_Tmp5=";\n";_tag_fat(_Tmp5,sizeof(char),3U);}));goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump(({const char*_Tmp5=";\n";_tag_fat(_Tmp5,sizeof(char),3U);}));goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 1385
if(td->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_Tmp5="\n";_tag_fat(_Tmp5,sizeof(char),2U);}));}
# 1389
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*ds=_Tmp3;
# 1391
Cyc_Absyndump_dump(({const char*_Tmp5="namespace ";_tag_fat(_Tmp5,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(v);
Cyc_Absyndump_dump(({const char*_Tmp5="{\n";_tag_fat(_Tmp5,sizeof(char),3U);}));
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_Tmp5="}\n";_tag_fat(_Tmp5,sizeof(char),3U);}));
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds=_Tmp4;
# 1399
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp4;
# 1403
Cyc_Absyndump_dump(({const char*_Tmp5="extern \"C\" {";_tag_fat(_Tmp5,sizeof(char),13U);}));
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_Tmp5="}\n";_tag_fat(_Tmp5,sizeof(char),3U);}));
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*ds=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple10*wc=_Tmp1;
# 1424 "absyndump.cyc"
goto _LL0;}default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="bad top-level declaration";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 1429
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(1;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
