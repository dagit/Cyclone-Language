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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
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
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 504
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 669 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 850
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
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
# 66
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 79
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple14{int f1;struct Cyc_List_List*f2;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
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
# 46
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
static int Cyc_Absynpp_qvar_to_Cids;static char _TmpG0[4U]="Cyc";
# 69
struct _fat_ptr Cyc_Absynpp_cyc_string={_TmpG0,_TmpG0,_TmpG0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static char _TmpG1[14U]="__NoCycPrefix";
# 73
static struct _fat_ptr Cyc_Absynpp_nocyc_str={_TmpG1,_TmpG1,_TmpG1 + 14U};
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
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_Tmp0=Cyc_Absynpp_curr_namespace;Cyc_List_imp_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=v,_Tmp1->tl=0;_Tmp1;}));});}
# 249
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if((_check_null(*l))->tl == 0)
*l=0;else{
# 253
Cyc_Absynpp_suppr_last(&(_check_null(*l))->tl);}}
# 257
static void Cyc_Absynpp_curr_namespace_drop (void){
Cyc_Absynpp_suppr_last(& Cyc_Absynpp_curr_namespace);}
# 261
struct _fat_ptr Cyc_Absynpp_char_escape(char c){
char _Tmp0=c;switch((int)_Tmp0){case 7:
 return({const char*_Tmp1="\\a";_tag_fat(_Tmp1,sizeof(char),3U);});case 8:
 return({const char*_Tmp1="\\b";_tag_fat(_Tmp1,sizeof(char),3U);});case 12:
 return({const char*_Tmp1="\\f";_tag_fat(_Tmp1,sizeof(char),3U);});case 10:
 return({const char*_Tmp1="\\n";_tag_fat(_Tmp1,sizeof(char),3U);});case 13:
 return({const char*_Tmp1="\\r";_tag_fat(_Tmp1,sizeof(char),3U);});case 9:
 return({const char*_Tmp1="\\t";_tag_fat(_Tmp1,sizeof(char),3U);});case 11:
 return({const char*_Tmp1="\\v";_tag_fat(_Tmp1,sizeof(char),3U);});case 92:
 return({const char*_Tmp1="\\\\";_tag_fat(_Tmp1,sizeof(char),3U);});case 34:
 return({const char*_Tmp1="\"";_tag_fat(_Tmp1,sizeof(char),2U);});case 39:
 return({const char*_Tmp1="\\'";_tag_fat(_Tmp1,sizeof(char),3U);});default:
# 274
 if((int)c >= 32 &&(int)c <= 126){
struct _fat_ptr t=Cyc_Core_new_string(2U);
({struct _fat_ptr _Tmp1=_fat_ptr_plus(t,sizeof(char),0);char _Tmp2=*((char*)_check_fat_subscript(_Tmp1,sizeof(char),0U));char _Tmp3=c;if(_get_fat_size(_Tmp1,sizeof(char))== 1U &&(_Tmp2 == 0 && _Tmp3 != 0))_throw_arraybounds();*((char*)_Tmp1.curr)=_Tmp3;});
return t;}else{
# 279
struct _fat_ptr t=Cyc_Core_new_string(5U);
int j=0;
({struct _fat_ptr _Tmp1=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp2=*((char*)_check_fat_subscript(_Tmp1,sizeof(char),0U));char _Tmp3='\\';if(_get_fat_size(_Tmp1,sizeof(char))== 1U &&(_Tmp2 == 0 && _Tmp3 != 0))_throw_arraybounds();*((char*)_Tmp1.curr)=_Tmp3;});
({struct _fat_ptr _Tmp1=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp2=*((char*)_check_fat_subscript(_Tmp1,sizeof(char),0U));char _Tmp3=(char)(48 + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_Tmp1,sizeof(char))== 1U &&(_Tmp2 == 0 && _Tmp3 != 0))_throw_arraybounds();*((char*)_Tmp1.curr)=_Tmp3;});
({struct _fat_ptr _Tmp1=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp2=*((char*)_check_fat_subscript(_Tmp1,sizeof(char),0U));char _Tmp3=(char)(48 + ((int)c >> 3 & 7));if(_get_fat_size(_Tmp1,sizeof(char))== 1U &&(_Tmp2 == 0 && _Tmp3 != 0))_throw_arraybounds();*((char*)_Tmp1.curr)=_Tmp3;});
({struct _fat_ptr _Tmp1=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp2=*((char*)_check_fat_subscript(_Tmp1,sizeof(char),0U));char _Tmp3=(char)(48 + ((int)c & 7));if(_get_fat_size(_Tmp1,sizeof(char))== 1U &&(_Tmp2 == 0 && _Tmp3 != 0))_throw_arraybounds();*((char*)_Tmp1.curr)=_Tmp3;});
return t;}};}
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
char _stmttmp0=((const char*)s.curr)[i];char _Tmp0=_stmttmp0;char _Tmp1;switch((int)_Tmp0){case 7:
 goto _LL4;case 8: _LL4:
 goto _LL6;case 12: _LL6:
 goto _LL8;case 10: _LL8:
 goto _LLA;case 13: _LLA:
 goto _LLC;case 9: _LLC:
 goto _LLE;case 11: _LLE:
 goto _LL10;case 92: _LL10:
 goto _LL12;case 34: _LL12:
 len +=2;goto _LL0;default: _Tmp1=_Tmp0;{char c=_Tmp1;
# 320
if((int)c >= 32 &&(int)c <= 126)++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 325
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _stmttmp1=((const char*)s.curr)[i];char _Tmp0=_stmttmp1;char _Tmp1;switch((int)_Tmp0){case 7:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='a';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 8:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='b';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 12:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='f';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 10:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='n';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 13:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='r';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 9:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='t';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 11:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='v';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 92:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 34:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='"';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;default: _Tmp1=_Tmp0;{char c=_Tmp1;
# 339
if((int)c >= 32 &&(int)c <= 126)({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=c;if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});else{
# 341
unsigned char uc=(unsigned char)c;
# 344
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc >> 6 & 7));if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc >> 3 & 7));if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc & 7));if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});}
# 349
goto _LL15;}}_LL15:;}}
# 351
return t;}}}}static char _TmpG2[9U]="restrict";
# 354
static struct _fat_ptr Cyc_Absynpp_restrict_string={_TmpG2,_TmpG2,_TmpG2 + 9U};static char _TmpG3[9U]="volatile";
static struct _fat_ptr Cyc_Absynpp_volatile_string={_TmpG3,_TmpG3,_TmpG3 + 9U};static char _TmpG4[6U]="const";
static struct _fat_ptr Cyc_Absynpp_const_str={_TmpG4,_TmpG4,_TmpG4 + 6U};
static struct _fat_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _fat_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _fat_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 361
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 364
if(tq.q_restrict)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=Cyc_Absynpp_restrict_sp,_Tmp0->tl=l;_Tmp0;});
if(tq.q_volatile)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=Cyc_Absynpp_volatile_sp,_Tmp0->tl=l;_Tmp0;});
if(tq.print_const)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=Cyc_Absynpp_const_sp,_Tmp0->tl=l;_Tmp0;});
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="";_tag_fat(_Tmp1,sizeof(char),1U);});struct _fat_ptr _Tmp1=({const char*_Tmp2=" ";_tag_fat(_Tmp2,sizeof(char),2U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=" ";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_PP_textptr,l));});}
# 370
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){return Cyc_Kinds_kind2string(ka);}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 373
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_stmttmp2=Cyc_Kinds_compress_kb(c);void*_Tmp0=_stmttmp2;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_Absynpp_kind2string(k);}case 1:
# 377
 if(Cyc_PP_tex_output)
return({const char*_Tmp2="{?}";_tag_fat(_Tmp2,sizeof(char),4U);});else{
return({const char*_Tmp2="?";_tag_fat(_Tmp2,sizeof(char),2U);});}default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_kind2string(k);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="<=%s";_tag_fat(_Tmp4,sizeof(char),5U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}};}
# 384
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_stmttmp3=Cyc_Kinds_compress_kb(c);void*_Tmp0=_stmttmp3;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1:
# 388
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_Tmp2="{?}";_tag_fat(_Tmp2,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_Tmp2="?";_tag_fat(_Tmp2,sizeof(char),2U);}));}default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}};}
# 395
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="<";_tag_fat(_Tmp1,sizeof(char),2U);});struct _fat_ptr _Tmp1=({const char*_Tmp2=">";_tag_fat(_Tmp2,sizeof(char),2U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_typ2doc,ts));});}
# 399
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
# 403
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
# 405
struct _fat_ptr kstring=({const char*_Tmp0="K";_tag_fat(_Tmp0,sizeof(char),2U);});
{void*_stmttmp4=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp0=_stmttmp4;void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 411
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=kstring;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=_fat_ptr_plus(n,sizeof(char),1);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="`G%s%s";_tag_fat(_Tmp3,sizeof(char),7U);}),_tag_fat(_Tmp2,sizeof(void*),2));}));}
# 413
return Cyc_PP_text(n);}
# 416
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_stmttmp5=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp0=_stmttmp5;void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_Tmp1;
# 421
return({struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_tvar2doc(tv);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(({const char*_Tmp4="::";_tag_fat(_Tmp4,sizeof(char),3U);}));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_kind2doc(k);_Tmp2[2]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));});}}default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
# 420
_Tmp1=k;goto _LL8;}};}
# 425
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="<";_tag_fat(_Tmp1,sizeof(char),2U);});struct _fat_ptr _Tmp1=({const char*_Tmp2=">";_tag_fat(_Tmp2,sizeof(char),2U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_ktvar2doc,tvs));});}
# 429
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="<";_tag_fat(_Tmp1,sizeof(char),2U);});struct _fat_ptr _Tmp1=({const char*_Tmp2=">";_tag_fat(_Tmp2,sizeof(char),2U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 435
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple17*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 439
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="(";_tag_fat(_Tmp1,sizeof(char),2U);});struct _fat_ptr _Tmp1=({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_arg2doc,ts));});}
# 443
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_Tmp0=att;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 return Cyc_PP_nil_doc();default:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));};}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(1;atts != 0;atts=atts->tl){
void*_stmttmp6=(void*)atts->hd;void*_Tmp0=_stmttmp6;switch(*((int*)_Tmp0)){case 1:
 return Cyc_PP_text(({const char*_Tmp1=" _stdcall ";_tag_fat(_Tmp1,sizeof(char),11U);}));case 2:
 return Cyc_PP_text(({const char*_Tmp1=" _cdecl ";_tag_fat(_Tmp1,sizeof(char),9U);}));case 3:
 return Cyc_PP_text(({const char*_Tmp1=" _fastcall ";_tag_fat(_Tmp1,sizeof(char),12U);}));default:
 goto _LL0;}_LL0:;}
# 460
return Cyc_PP_nil_doc();}
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_stmttmp7=(void*)atts2->hd;void*_Tmp0=_stmttmp7;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 472
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" __declspec(";_tag_fat(_Tmp2,sizeof(char),13U);}));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);});struct _fat_ptr _Tmp3=({const char*_Tmp4="";_tag_fat(_Tmp4,sizeof(char),1U);});struct _fat_ptr _Tmp4=({const char*_Tmp5=" ";_tag_fat(_Tmp5,sizeof(char),2U);});Cyc_PP_group(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_noncallatt2doc,atts));});_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 479
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
void*_Tmp0=a;int _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;int _Tmp4;switch(*((int*)_Tmp0)){case 0: _Tmp4=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_Tmp0)->f1;{int i=_Tmp4;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="regparm(%d)";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}case 1:
 return({const char*_Tmp5="stdcall";_tag_fat(_Tmp5,sizeof(char),8U);});case 2:
 return({const char*_Tmp5="cdecl";_tag_fat(_Tmp5,sizeof(char),6U);});case 3:
 return({const char*_Tmp5="fastcall";_tag_fat(_Tmp5,sizeof(char),9U);});case 4:
 return({const char*_Tmp5="noreturn";_tag_fat(_Tmp5,sizeof(char),9U);});case 5:
 return({const char*_Tmp5="const";_tag_fat(_Tmp5,sizeof(char),6U);});case 6: _Tmp3=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 488
if(e == 0)return({const char*_Tmp5="aligned";_tag_fat(_Tmp5,sizeof(char),8U);});else{
# 490
enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;switch((int)_Tmp5){case Cyc_Flags_Gcc_c:
 return({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(e);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="aligned(%s)";_tag_fat(_Tmp8,sizeof(char),12U);}),_tag_fat(_Tmp7,sizeof(void*),1));});case Cyc_Flags_Vc_c:
 goto _LL41;default: _LL41:
 return({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(e);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="align(%s)";_tag_fat(_Tmp8,sizeof(char),10U);}),_tag_fat(_Tmp7,sizeof(void*),1));});};}}case 7:
# 495
 return({const char*_Tmp5="packed";_tag_fat(_Tmp5,sizeof(char),7U);});case 8: _Tmp2=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp2;
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="section(\"%s\")";_tag_fat(_Tmp7,sizeof(char),14U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}case 9:
 return({const char*_Tmp5="nocommon";_tag_fat(_Tmp5,sizeof(char),9U);});case 10:
 return({const char*_Tmp5="shared";_tag_fat(_Tmp5,sizeof(char),7U);});case 11:
 return({const char*_Tmp5="unused";_tag_fat(_Tmp5,sizeof(char),7U);});case 12:
 return({const char*_Tmp5="weak";_tag_fat(_Tmp5,sizeof(char),5U);});case 13:
 return({const char*_Tmp5="dllimport";_tag_fat(_Tmp5,sizeof(char),10U);});case 14:
 return({const char*_Tmp5="dllexport";_tag_fat(_Tmp5,sizeof(char),10U);});case 15:
 return({const char*_Tmp5="no_instrument_function";_tag_fat(_Tmp5,sizeof(char),23U);});case 16:
 return({const char*_Tmp5="constructor";_tag_fat(_Tmp5,sizeof(char),12U);});case 17:
 return({const char*_Tmp5="destructor";_tag_fat(_Tmp5,sizeof(char),11U);});case 18:
 return({const char*_Tmp5="no_check_memory_usage";_tag_fat(_Tmp5,sizeof(char),22U);});case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0)->f1 == Cyc_Absyn_Printf_ft){_Tmp4=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0)->f3;{int n=_Tmp4;int m=_Tmp1;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned)n;_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned)m;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="format(printf,%u,%u)";_tag_fat(_Tmp8,sizeof(char),21U);}),_tag_fat(_Tmp7,sizeof(void*),2));});}}else{_Tmp4=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0)->f3;{int n=_Tmp4;int m=_Tmp1;
# 509
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned)n;_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned)m;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="format(scanf,%u,%u)";_tag_fat(_Tmp8,sizeof(char),20U);}),_tag_fat(_Tmp7,sizeof(void*),2));});}}case 20: _Tmp4=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp0)->f1;{int n=_Tmp4;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)n;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="initializes(%d)";_tag_fat(_Tmp7,sizeof(char),16U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}case 21: _Tmp4=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp0)->f1;{int n=_Tmp4;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)n;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="noliveunique(%d)";_tag_fat(_Tmp7,sizeof(char),17U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}case 22: _Tmp4=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_Tmp0)->f1;{int n=_Tmp4;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)n;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="consume(%d)";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}case 23:
 return({const char*_Tmp5="pure";_tag_fat(_Tmp5,sizeof(char),5U);});case 26:
 return({const char*_Tmp5="always_inline";_tag_fat(_Tmp5,sizeof(char),14U);});case 24: _Tmp2=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp2;
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="__mode__(\"%s\")";_tag_fat(_Tmp7,sizeof(char),15U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}case 25: _Tmp2=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp2;
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="alias(\"%s\")";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}default:
 return({const char*_Tmp5="no_throw";_tag_fat(_Tmp5,sizeof(char),9U);});};}
# 521
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 525
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Flags_C_Compilers _Tmp0=Cyc_Flags_c_compiler;if(_Tmp0 == Cyc_Flags_Vc_c)
return Cyc_Absynpp_noncallconv2doc(atts);else{
# 530
return({struct Cyc_PP_Doc*_Tmp1[2];({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=" __attribute__";_tag_fat(_Tmp3,sizeof(char),15U);}));_Tmp1[0]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=({struct _fat_ptr _Tmp3=({const char*_Tmp4="((";_tag_fat(_Tmp4,sizeof(char),3U);});struct _fat_ptr _Tmp4=({const char*_Tmp5="))";_tag_fat(_Tmp5,sizeof(char),3U);});struct _fat_ptr _Tmp5=({const char*_Tmp6=",";_tag_fat(_Tmp6,sizeof(char),2U);});Cyc_PP_group(_Tmp3,_Tmp4,_Tmp5,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_att2doc,atts));});_Tmp1[1]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),2));});};}}
# 535
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_stmttmp8=(void*)tms->hd;void*_Tmp0=_stmttmp8;switch(*((int*)_Tmp0)){case 2:
 return 1;case 5:  {
# 540
enum Cyc_Flags_C_Compilers _Tmp1=Cyc_Flags_c_compiler;if(_Tmp1 == Cyc_Flags_Gcc_c)
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
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_Tmp0="{?}";_tag_fat(_Tmp0,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_Tmp0="?";_tag_fat(_Tmp0,sizeof(char),2U);}));}}
# 558
return _check_null(Cyc_Absynpp_cache_question);}
# 560
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
if(!((unsigned)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_Tmp0="{\\lb}";_tag_fat(_Tmp0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_Tmp0="{";_tag_fat(_Tmp0,sizeof(char),2U);}));}}
# 567
return _check_null(Cyc_Absynpp_cache_lb);}
# 569
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
if(!((unsigned)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_Tmp0="{\\rb}";_tag_fat(_Tmp0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_Tmp0="}";_tag_fat(_Tmp0,sizeof(char),2U);}));}}
# 576
return _check_null(Cyc_Absynpp_cache_rb);}
# 578
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
if(!((unsigned)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_Tmp0="\\$";_tag_fat(_Tmp0,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_Tmp0="$";_tag_fat(_Tmp0,sizeof(char),2U);}));}}
# 585
return _check_null(Cyc_Absynpp_cache_dollar);}
# 587
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seq(sep,ss);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 592
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(1;tms != 0;tms=tms->tl){
void*_stmttmp9=(void*)tms->hd;void*_Tmp0=_stmttmp9;void*_Tmp1;switch(*((int*)_Tmp0)){case 0:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Carray_mod ";_tag_fat(_Tmp2,sizeof(char),12U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="ConstArray_mod ";_tag_fat(_Tmp2,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1)== 1){_Tmp1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1)->f1;{struct Cyc_List_List*ys=_Tmp1;
# 598
Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Function_mod(";_tag_fat(_Tmp2,sizeof(char),14U);}),_tag_fat(0U,sizeof(void*),0));
for(1;ys != 0;ys=ys->tl){
struct _fat_ptr*v=(*((struct _tuple8*)ys->hd)).f1;
if(v == 0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="?";_tag_fat(_Tmp2,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));else{
Cyc_fprintf(Cyc_stderr,*v,_tag_fat(0U,sizeof(void*),0));}
if(ys->tl != 0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp2=",";_tag_fat(_Tmp2,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 605
Cyc_fprintf(Cyc_stderr,({const char*_Tmp2=") ";_tag_fat(_Tmp2,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}else{
# 608
Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Function_mod()";_tag_fat(_Tmp2,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 5:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Attributes_mod ";_tag_fat(_Tmp2,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 4:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="TypeParams_mod ";_tag_fat(_Tmp2,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;default:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Pointer_mod ";_tag_fat(_Tmp2,sizeof(char),13U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 613
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="\n";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 616
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*);
# 618
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="(";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=rest,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
void*_stmttmpA=(void*)tms->hd;void*_Tmp0=_stmttmpA;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;unsigned _Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp0)){case 0: _Tmp8=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*zeroterm=_Tmp8;
# 624
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_Tmp9[2];_Tmp9[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(({const char*_TmpB="[]@zeroterm";_tag_fat(_TmpB,sizeof(char),12U);}));_Tmp9[1]=_TmpA;}):({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(({const char*_TmpB="[]";_tag_fat(_TmpB,sizeof(char),3U);}));_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});}case 1: _Tmp8=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp7=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;void*zeroterm=_Tmp7;
# 628
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_Tmp9[4];_Tmp9[0]=rest,({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(({const char*_TmpB="[";_tag_fat(_TmpB,sizeof(char),2U);}));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_exp2doc(e);_Tmp9[2]=_TmpA;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(({const char*_TmpB="]@zeroterm";_tag_fat(_TmpB,sizeof(char),11U);}));_Tmp9[3]=_TmpA;}):({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(({const char*_TmpB="]";_tag_fat(_TmpB,sizeof(char),2U);}));_Tmp9[3]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));});}case 3: _Tmp8=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp8;
# 632
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_Tmp9=args;unsigned _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;int _Tmp10;void*_Tmp11;if(*((int*)_Tmp9)== 1){_Tmp11=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f1;_Tmp10=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f2;_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f3;_TmpE=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f4;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f5;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f6;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp9)->f7;{struct Cyc_List_List*args2=_Tmp11;int c_varargs=_Tmp10;struct Cyc_Absyn_VarargInfo*cyc_varargs=_TmpF;void*effopt=_TmpE;struct Cyc_List_List*rgn_po=_TmpD;struct Cyc_Absyn_Exp*req=_TmpC;struct Cyc_Absyn_Exp*ens=_TmpB;
# 635
return({struct Cyc_PP_Doc*_Tmp12[2];_Tmp12[0]=rest,({struct Cyc_PP_Doc*_Tmp13=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);_Tmp12[1]=_Tmp13;});Cyc_PP_cat(_tag_fat(_Tmp12,sizeof(struct Cyc_PP_Doc*),2));});}}else{_Tmp11=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_Tmp9)->f2;{struct Cyc_List_List*sl=_Tmp11;unsigned loc=_TmpA;
# 637
return({struct Cyc_PP_Doc*_Tmp12[2];_Tmp12[0]=rest,({struct Cyc_PP_Doc*_Tmp13=({struct _fat_ptr _Tmp14=({const char*_Tmp15="(";_tag_fat(_Tmp15,sizeof(char),2U);});struct _fat_ptr _Tmp15=({const char*_Tmp16=")";_tag_fat(_Tmp16,sizeof(char),2U);});struct _fat_ptr _Tmp16=({const char*_Tmp17=",";_tag_fat(_Tmp17,sizeof(char),2U);});Cyc_PP_group(_Tmp14,_Tmp15,_Tmp16,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_PP_textptr,sl));});_Tmp12[1]=_Tmp13;});Cyc_PP_cat(_tag_fat(_Tmp12,sizeof(struct Cyc_PP_Doc*),2));});}};}}case 5: _Tmp8=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*atts=_Tmp8;
# 640
enum Cyc_Flags_C_Compilers _Tmp9=Cyc_Flags_c_compiler;if(_Tmp9 == Cyc_Flags_Gcc_c){
# 642
if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_atts2doc(atts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});}else{
# 646
if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_callconv2doc(atts);_TmpA[0]=_TmpB;}),_TmpA[1]=rest;Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
return rest;};}case 4: _Tmp8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_List_List*ts=_Tmp8;unsigned loc=_Tmp6;int print_kinds=_Tmp5;
# 651
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_Tmp9[2];_Tmp9[0]=rest,({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_ktvars2doc(ts);_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});else{
# 655
return({struct Cyc_PP_Doc*_Tmp9[2];_Tmp9[0]=rest,({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_tvars2doc(ts);_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});}}default: _Tmp8=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).rgn;_Tmp7=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).nullable;_Tmp4=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).bounds;_Tmp3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).zero_term;_Tmp2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1).autoreleased;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*rgn=_Tmp8;void*nullable=_Tmp7;void*bd=_Tmp4;void*zt=_Tmp3;void*rel=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 659
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*reld=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_stmttmpB=Cyc_Absyn_compress(bd);void*_Tmp9=_stmttmpB;void*_TmpA;if(*((int*)_Tmp9)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)){case 14:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f2 != 0){_TmpA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f2)->hd;{void*targ=_TmpA;
# 668
{void*_stmttmpC=Cyc_Absyn_compress(targ);void*_TmpB=_stmttmpC;void*_TmpC;if(*((int*)_TmpB)== 9){_TmpC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TmpB)->f1;{struct Cyc_Absyn_Exp*e=_TmpC;
# 670
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_TmpD="*";_tag_fat(_TmpD,sizeof(char),2U);}):({const char*_TmpD="@";_tag_fat(_TmpD,sizeof(char),2U);}));{
struct _tuple12 _stmttmpD=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _TmpD=_stmttmpD;int _TmpE;unsigned _TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{unsigned val=_TmpF;int known=_TmpE;
if(!known || val != 1U)
ptr=({struct Cyc_PP_Doc*_Tmp10[4];_Tmp10[0]=ptr,({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_lb();_Tmp10[1]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_exp2doc(e);_Tmp10[2]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_rb();_Tmp10[3]=_Tmp11;});Cyc_PP_cat(_tag_fat(_Tmp10,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}}}}else{
# 676
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_TmpD="*";_tag_fat(_TmpD,sizeof(char),2U);}):({const char*_TmpD="@";_tag_fat(_TmpD,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_TmpD[4];_TmpD[0]=ptr,({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc(targ);_TmpD[2]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_rb();_TmpD[3]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}_LL1E:;}
# 680
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C:
# 682
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?({const char*_TmpB="*";_tag_fat(_TmpB,sizeof(char),2U);}):({const char*_TmpB="@";_tag_fat(_TmpB,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_TmpB[4];_TmpB[0]=ptr,({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_lb();_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc(bd);_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_rb();_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL17;}_LL17:;}
# 687
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_Tmp9="@zeroterm";_tag_fat(_Tmp9,sizeof(char),10U);}));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(({const char*_Tmp9="@nozeroterm";_tag_fat(_Tmp9,sizeof(char),12U);}));}
if(Cyc_Absyn_type2bool(0,rel))
reld=Cyc_PP_text(({const char*_Tmp9="@autoreleased";_tag_fat(_Tmp9,sizeof(char),14U);}));}
# 695
{void*_stmttmpE=Cyc_Absyn_compress(rgn);void*_Tmp9=_stmttmpE;switch(*((int*)_Tmp9)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)== 5)
goto _LL26;else{goto _LL2B;}case 1: if(Cyc_Absynpp_print_for_cycdoc)
goto _LL26;else{goto _LL2B;}default: _LL2B:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL26:;}{
# 700
struct Cyc_PP_Doc*spacer0=reld != mt && ztd != mt?Cyc_PP_text(({const char*_Tmp9=" ";_tag_fat(_Tmp9,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer1=tqd != mt &&((ztd != mt || rgd != mt)|| reld != mt)?Cyc_PP_text(({const char*_Tmp9=" ";_tag_fat(_Tmp9,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_Tmp9=" ";_tag_fat(_Tmp9,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_Tmp9[9];_Tmp9[0]=ptr,_Tmp9[1]=ztd,_Tmp9[2]=spacer0,_Tmp9[3]=reld,_Tmp9[4]=rgd,_Tmp9[5]=spacer1,_Tmp9[6]=tqd,_Tmp9[7]=spacer2,_Tmp9[8]=rest;Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),9));});}}};}}
# 707
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_stmttmpF=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmpF;if(*((int*)_Tmp0)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 5:
 return Cyc_PP_text(({const char*_Tmp1="`H";_tag_fat(_Tmp1,sizeof(char),3U);}));case 6:
 return Cyc_PP_text(({const char*_Tmp1="`U";_tag_fat(_Tmp1,sizeof(char),3U);}));case 7:
 return Cyc_PP_text(({const char*_Tmp1="`RC";_tag_fat(_Tmp1,sizeof(char),4U);}));default: goto _LL7;}else{_LL7:
 return Cyc_Absynpp_ntyp2doc(t);};}
# 717
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 721
void*_stmttmp10=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp10;void*_Tmp1;if(*((int*)_Tmp0)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 8: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2 != 0){_Tmp1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd;{void*r=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_rgn2doc(r);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=*rgions;_Tmp3;});*rgions=_Tmp2;});goto _LL0;}}else{goto _LL5;}case 9: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp1;
# 724
for(1;ts != 0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
# 727
goto _LL0;}default: goto _LL5;}else{_LL5:
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_typ2doc(t);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=*effects;_Tmp3;});*effects=_Tmp2;});goto _LL0;}_LL0:;}
# 732
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=Cyc_List_imp_rev(rgions);
effects=Cyc_List_imp_rev(effects);
if(rgions == 0 && effects != 0)
return Cyc_PP_group(({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0="+";_tag_fat(_Tmp0,sizeof(char),2U);}),effects);else{
# 740
struct Cyc_PP_Doc*doc1=Cyc_Absynpp_group_braces(({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),rgions);
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="";_tag_fat(_Tmp1,sizeof(char),1U);});struct _fat_ptr _Tmp1=({const char*_Tmp2="";_tag_fat(_Tmp2,sizeof(char),1U);});struct _fat_ptr _Tmp2=({const char*_Tmp3="+";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*_Tmp3=effects;Cyc_List_imp_append(_Tmp3,({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=doc1,_Tmp4->tl=0;_Tmp4;}));}));});}}
# 745
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _Tmp0=k;if(_Tmp0 == Cyc_Absyn_StructA)
return Cyc_PP_text(({const char*_Tmp1="struct ";_tag_fat(_Tmp1,sizeof(char),8U);}));else{
return Cyc_PP_text(({const char*_Tmp1="union ";_tag_fat(_Tmp1,sizeof(char),7U);}));};}
# 753
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_Tmp0=t;enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;struct _fat_ptr _Tmp4;union Cyc_Absyn_AggrInfo _Tmp5;int _Tmp6;enum Cyc_Absyn_Size_of _Tmp7;enum Cyc_Absyn_Sign _Tmp8;union Cyc_Absyn_DatatypeFieldInfo _Tmp9;void*_TmpA;union Cyc_Absyn_DatatypeInfo _TmpB;switch(*((int*)_Tmp0)){case 4:
# 757
 return Cyc_PP_text(({const char*_TmpC="[[[array]]]";_tag_fat(_TmpC,sizeof(char),12U);}));case 5:
 return Cyc_PP_nil_doc();case 3:
 return Cyc_PP_nil_doc();case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 0:
# 761
 s=Cyc_PP_text(({const char*_TmpC="void";_tag_fat(_TmpC,sizeof(char),5U);}));goto _LL0;case 18: _TmpB=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_DatatypeInfo tu_info=_TmpB;struct Cyc_List_List*ts=_TmpA;
# 777
{union Cyc_Absyn_DatatypeInfo _TmpC=tu_info;int _TmpD;void*_TmpE;if((_TmpC.UnknownDatatype).tag == 1){_TmpE=((_TmpC.UnknownDatatype).val).name;_TmpD=((_TmpC.UnknownDatatype).val).is_extensible;{struct _tuple0*n=_TmpE;int is_x=_TmpD;
_TmpE=n;_TmpD=is_x;goto _LL4B;}}else{_TmpE=(*(_TmpC.KnownDatatype).val)->name;_TmpD=(*(_TmpC.KnownDatatype).val)->is_extensible;_LL4B: {struct _tuple0*n=_TmpE;int is_x=_TmpD;
# 780
struct Cyc_PP_Doc*kw=Cyc_PP_text(({const char*_TmpF="datatype ";_tag_fat(_TmpF,sizeof(char),10U);}));
struct Cyc_PP_Doc*ext=is_x?Cyc_PP_text(({const char*_TmpF="@extensible ";_tag_fat(_TmpF,sizeof(char),13U);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_TmpF[4];_TmpF[0]=ext,_TmpF[1]=kw,({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_qvar2doc(n);_TmpF[2]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_tps2doc(ts);_TmpF[3]=_Tmp10;});Cyc_PP_cat(_tag_fat(_TmpF,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL47;}}_LL47:;}
# 785
goto _LL0;}case 19: _Tmp9=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_Tmp9;struct Cyc_List_List*ts=_TmpA;
# 787
{union Cyc_Absyn_DatatypeFieldInfo _TmpC=tuf_info;int _TmpD;void*_TmpE;void*_TmpF;if((_TmpC.UnknownDatatypefield).tag == 1){_TmpF=((_TmpC.UnknownDatatypefield).val).datatype_name;_TmpE=((_TmpC.UnknownDatatypefield).val).field_name;_TmpD=((_TmpC.UnknownDatatypefield).val).is_extensible;{struct _tuple0*tname=_TmpF;struct _tuple0*fname=_TmpE;int is_x=_TmpD;
# 789
_TmpF=tname;_TmpD=is_x;_TmpE=fname;goto _LL50;}}else{_TmpF=(((_TmpC.KnownDatatypefield).val).f1)->name;_TmpD=(((_TmpC.KnownDatatypefield).val).f1)->is_extensible;_TmpE=(((_TmpC.KnownDatatypefield).val).f2)->name;_LL50: {struct _tuple0*tname=_TmpF;int is_x=_TmpD;struct _tuple0*fname=_TmpE;
# 792
struct Cyc_PP_Doc*kw=Cyc_PP_text(is_x?({const char*_Tmp10="@extensible datatype ";_tag_fat(_Tmp10,sizeof(char),22U);}):({const char*_Tmp10="datatype ";_tag_fat(_Tmp10,sizeof(char),10U);}));
s=({struct Cyc_PP_Doc*_Tmp10[4];_Tmp10[0]=kw,({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_qvar2doc(tname);_Tmp10[1]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_PP_text(({const char*_Tmp12=".";_tag_fat(_Tmp12,sizeof(char),2U);}));_Tmp10[2]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_qvar2doc(fname);_Tmp10[3]=_Tmp11;});Cyc_PP_cat(_tag_fat(_Tmp10,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL4C;}}_LL4C:;}
# 796
goto _LL0;}case 1: _Tmp8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp7=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2;{enum Cyc_Absyn_Sign sn=_Tmp8;enum Cyc_Absyn_Size_of sz=_Tmp7;
# 798
struct _fat_ptr sns;
struct _fat_ptr ts;
{enum Cyc_Absyn_Sign _TmpC=sn;switch((int)_TmpC){case Cyc_Absyn_None:
 goto _LL55;case Cyc_Absyn_Signed: _LL55:
 sns=({const char*_TmpD="";_tag_fat(_TmpD,sizeof(char),1U);});goto _LL51;default:
 sns=({const char*_TmpD="unsigned ";_tag_fat(_TmpD,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 805
{enum Cyc_Absyn_Size_of _TmpC=sz;switch((int)_TmpC){case Cyc_Absyn_Char_sz:
# 807
{enum Cyc_Absyn_Sign _TmpD=sn;switch((int)_TmpD){case Cyc_Absyn_None:
 sns=({const char*_TmpE="";_tag_fat(_TmpE,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed:
 sns=({const char*_TmpE="signed ";_tag_fat(_TmpE,sizeof(char),8U);});goto _LL65;default:
 sns=({const char*_TmpE="unsigned ";_tag_fat(_TmpE,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 812
ts=({const char*_TmpD="char";_tag_fat(_TmpD,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz:
 ts=({const char*_TmpD="short";_tag_fat(_TmpD,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz:
 ts=({const char*_TmpD="int";_tag_fat(_TmpD,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz:
 ts=({const char*_TmpD="long";_tag_fat(_TmpD,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz:
 goto _LL64;default: _LL64:
# 819
{enum Cyc_Flags_C_Compilers _TmpD=Cyc_Flags_c_compiler;if(_TmpD == Cyc_Flags_Gcc_c){
ts=({const char*_TmpE="long long";_tag_fat(_TmpE,sizeof(char),10U);});goto _LL6C;}else{
ts=({const char*_TmpE="__int64";_tag_fat(_TmpE,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 823
goto _LL58;}_LL58:;}
# 825
s=Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=sns;_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=ts;_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_aprintf(({const char*_TmpF="%s%s";_tag_fat(_TmpF,sizeof(char),5U);}),_tag_fat(_TmpE,sizeof(void*),2));}));
goto _LL0;}case 2: _Tmp6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{int i=_Tmp6;
# 828
{int _TmpC=i;switch((int)_TmpC){case 0:
 s=Cyc_PP_text(({const char*_TmpD="float";_tag_fat(_TmpD,sizeof(char),6U);}));goto _LL71;case 1:
 s=Cyc_PP_text(({const char*_TmpD="double";_tag_fat(_TmpD,sizeof(char),7U);}));goto _LL71;default:
 s=Cyc_PP_text(({const char*_TmpD="long double";_tag_fat(_TmpD,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 833
goto _LL0;}case 20: _Tmp5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp5;struct Cyc_List_List*ts=_TmpA;
# 836
struct _tuple11 _stmttmp11=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _TmpC=_stmttmp11;void*_TmpD;enum Cyc_Absyn_AggrKind _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{enum Cyc_Absyn_AggrKind k=_TmpE;struct _tuple0*n=_TmpD;
s=({struct Cyc_PP_Doc*_TmpF[3];({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_aggr_kind2doc(k);_TmpF[0]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_qvar2doc(n);_TmpF[1]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_tps2doc(ts);_TmpF[2]=_Tmp10;});Cyc_PP_cat(_tag_fat(_TmpF,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}}case 16: _TmpA=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{struct Cyc_List_List*fs=_TmpA;
# 845
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="enum ";_tag_fat(_TmpE,sizeof(char),6U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 15: _TmpA=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{struct _tuple0*n=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="enum ";_tag_fat(_TmpE,sizeof(char),6U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_qvar2doc(n);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 17: _Tmp4=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{struct _fat_ptr t=_Tmp4;
# 849
s=Cyc_PP_text(t);goto _LL0;}case 3: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_TmpA;
# 859
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="region_t<";_tag_fat(_TmpE,sizeof(char),10U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rgn2doc((void*)(_check_null(ts))->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE=">";_tag_fat(_TmpE,sizeof(char),2U);}));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 4: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_TmpA;
# 861
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="tag_t<";_tag_fat(_TmpE,sizeof(char),7U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((void*)(_check_null(ts))->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE=">";_tag_fat(_TmpE,sizeof(char),2U);}));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 6:
 goto _LL2E;case 5: _LL2E:
 goto _LL30;case 7: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_TmpA;
# 866
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="regions(";_tag_fat(_TmpE,sizeof(char),9U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((void*)(_check_null(ts))->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE=")";_tag_fat(_TmpE,sizeof(char),2U);}));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 8:
 goto _LL36;case 9: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11:
# 872
 s=Cyc_PP_text(({const char*_TmpC="@true";_tag_fat(_TmpC,sizeof(char),6U);}));goto _LL0;case 12:
 s=Cyc_PP_text(({const char*_TmpC="@false";_tag_fat(_TmpC,sizeof(char),7U);}));goto _LL0;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2 != 0){_TmpA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd;{void*t=_TmpA;
# 875
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="@thin @numelts";_tag_fat(_TmpE,sizeof(char),15U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}}else{
# 877
s=Cyc_PP_text(({const char*_TmpC="@thin";_tag_fat(_TmpC,sizeof(char),6U);}));goto _LL0;}default:
 s=Cyc_PP_text(({const char*_TmpC="@fat";_tag_fat(_TmpC,sizeof(char),5U);}));goto _LL0;}case 1: _TmpA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f4;{struct Cyc_Core_Opt*k=_TmpA;void*topt=_Tmp3;int i=_Tmp6;struct Cyc_Core_Opt*tvs=_Tmp2;
# 763
if(topt != 0)
# 765
return Cyc_Absynpp_ntyp2doc(topt);else{
# 767
struct _fat_ptr kindstring=k == 0?({const char*_TmpC="K";_tag_fat(_TmpC,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
s=Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=kindstring;_TmpD;});struct Cyc_Int_pa_PrintArg_struct _TmpD=({struct Cyc_Int_pa_PrintArg_struct _TmpE;_TmpE.tag=1,_TmpE.f1=(unsigned long)i;_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_aprintf(({const char*_TmpF="`E%s%d";_tag_fat(_TmpF,sizeof(char),7U);}),_tag_fat(_TmpE,sizeof(void*),2));}));}
# 770
goto _LL0;}case 2: _TmpA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_TmpA;
# 772
s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_TmpC[3];_TmpC[0]=s,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="::";_tag_fat(_TmpE,sizeof(char),3U);}));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_kindbound2doc(tv->kind);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 6: _TmpA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*ts=_TmpA;
# 834
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_dollar();_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_args2doc(ts);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_TmpA;
# 840
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_aggr_kind2doc(k);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 9: _TmpA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 847
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="valueof_t(";_tag_fat(_TmpE,sizeof(char),11U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE=")";_tag_fat(_TmpE,sizeof(char),2U);}));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 11: _TmpA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE="typeof(";_tag_fat(_TmpE,sizeof(char),8U);}));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(({const char*_TmpE=")";_tag_fat(_TmpE,sizeof(char),2U);}));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 8: _TmpA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;{struct _tuple0*n=_TmpA;struct Cyc_List_List*ts=_Tmp3;struct Cyc_Absyn_Typedefdecl*kopt=_Tmp2;
# 856
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_qvar2doc(n);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_tps2doc(ts);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)){case 0: _TmpA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_TmpA;
# 869
s=Cyc_Absynpp_aggrdecl2doc(d);goto _LL0;}case 1: _TmpA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_TmpA;
s=Cyc_Absynpp_enumdecl2doc(d);goto _LL0;}default: _TmpA=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_TmpA;
s=Cyc_Absynpp_datatypedecl2doc(d);goto _LL0;}}}_LL0:;}
# 880
return s;}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _fat_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple18{void*f1;void*f2;};
# 887
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple18*cmp){
struct _tuple18*_Tmp0=cmp;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{void*r1=_Tmp2;void*r2=_Tmp1;
return({struct Cyc_PP_Doc*_Tmp3[3];({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_rgn2doc(r1);_Tmp3[0]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(({const char*_Tmp5=" > ";_tag_fat(_Tmp5,sizeof(char),4U);}));_Tmp3[1]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_rgn2doc(r2);_Tmp3[2]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),3));});}}
# 892
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="";_tag_fat(_Tmp1,sizeof(char),1U);});struct _fat_ptr _Tmp1=({const char*_Tmp2="";_tag_fat(_Tmp2,sizeof(char),1U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple18*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 896
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _fat_ptr*vo=(*t).f1;
struct Cyc_Core_Opt*dopt=vo == 0?0:({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(*vo);_Tmp0->v=_Tmp1;});_Tmp0;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 902
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 906
struct Cyc_List_List*arg_docs=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_Tmp0=arg_docs;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3="...";_tag_fat(_Tmp3,sizeof(char),4U);}));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*varargs_doc=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="...";_tag_fat(_Tmp2,sizeof(char),4U);}));_Tmp0[0]=_Tmp1;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" inject ";_tag_fat(_Tmp2,sizeof(char),9U);}));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" ";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_funarg2doc(({struct _tuple8*_Tmp2=_cycalloc(sizeof(struct _tuple8));_Tmp2->f1=cyc_varargs->name,_Tmp2->f2=cyc_varargs->tq,_Tmp2->f3=cyc_varargs->type;_Tmp2;}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
# 915
arg_docs=({struct Cyc_List_List*_Tmp0=arg_docs;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=varargs_doc,_Tmp1->tl=0;_Tmp1;}));});}}{
# 917
struct Cyc_PP_Doc*arg_doc=Cyc_PP_group(({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),arg_docs);
if(effopt != 0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=";";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_eff2doc(effopt);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(rgn_po != 0)
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=":";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rgnpo2doc(rgn_po);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});{
struct Cyc_PP_Doc*res=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="(";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(req != 0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" @requires(";_tag_fat(_Tmp2,sizeof(char),12U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(req);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
if(ens != 0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" @ensures(";_tag_fat(_Tmp2,sizeof(char),11U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(ens);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
return res;}}}
# 930
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _fat_ptr*v){return Cyc_PP_text(*v);}
# 932
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*prefix=0;
int match;
{union Cyc_Absyn_Nmspace _stmttmp12=(*q).f1;union Cyc_Absyn_Nmspace _Tmp0=_stmttmp12;void*_Tmp1;switch((_Tmp0.C_n).tag){case 4:
 _Tmp1=0;goto _LL4;case 1: _Tmp1=(_Tmp0.Rel_n).val;_LL4: {struct Cyc_List_List*x=_Tmp1;
# 938
match=0;
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 942
prefix=x;}
goto _LL0;}case 3: _Tmp1=(_Tmp0.C_n).val;{struct Cyc_List_List*x=_Tmp1;
# 945
match=Cyc_Absynpp_use_curr_namespace &&({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 947
goto _LL0;}default: _Tmp1=(_Tmp0.Abs_n).val;{struct Cyc_List_List*x=_Tmp1;
# 949
match=Cyc_Absynpp_use_curr_namespace &&({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
if(Cyc_Absynpp_qvar_to_Cids){
if((unsigned)x && !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 954
prefix=Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=Cyc_Absynpp_cyc_stringptr,_Tmp2->tl=x;_Tmp2;}): x;}}
# 956
goto _LL0;}}_LL0:;}
# 958
if(Cyc_Absynpp_qvar_to_Cids)
return({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));
_Tmp2->hd=(*q).f2,_Tmp2->tl=0;_Tmp2;}));});
# 959
Cyc_str_sepstr(_Tmp0,({const char*_Tmp1="_";_tag_fat(_Tmp1,sizeof(char),2U);}));});else{
# 963
if(match)
return*(*q).f2;else{
# 966
return({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(*q).f2,_Tmp2->tl=0;_Tmp2;}));});Cyc_str_sepstr(_Tmp0,({const char*_Tmp1="::";_tag_fat(_Tmp1,sizeof(char),3U);}));});}}}
# 970
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 974
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _fat_ptr qs=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 978
return({struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1=Cyc_strconcat(({const char*_Tmp2="\\textbf{";_tag_fat(_Tmp2,sizeof(char),9U);}),qs);Cyc_strconcat(_Tmp1,({const char*_Tmp2="}";_tag_fat(_Tmp2,sizeof(char),2U);}));});Cyc_PP_text_width(_Tmp0,(int)
Cyc_strlen(qs));});else{
return Cyc_PP_text(qs);}}
# 983
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 985
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 988
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _stmttmp13=(*v).f1;union Cyc_Absyn_Nmspace _Tmp0=_stmttmp13;void*_Tmp1;switch((_Tmp0.C_n).tag){case 4:
 goto _LL4;case 1: if((_Tmp0.Rel_n).val == 0){_LL4:
 return*(*v).f2;}else{_LLA:
# 999
 return({struct _fat_ptr _Tmp2=({const char*_Tmp3="/* bad namespace : */ ";_tag_fat(_Tmp3,sizeof(char),23U);});Cyc_strconcat(_Tmp2,Cyc_Absynpp_qvar2string(v));});}case 3: _Tmp1=(_Tmp0.C_n).val;{struct Cyc_List_List*l=_Tmp1;
# 992
_Tmp1=l;goto _LL8;}default: _Tmp1=(_Tmp0.Abs_n).val;_LL8: {struct Cyc_List_List*l=_Tmp1;
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
struct _fat_ptr vs=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 1012
return({struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1=Cyc_strconcat(({const char*_Tmp2="\\textbf{";_tag_fat(_Tmp2,sizeof(char),9U);}),vs);Cyc_strconcat(_Tmp1,({const char*_Tmp2="}";_tag_fat(_Tmp2,sizeof(char),2U);}));});Cyc_PP_text_width(_Tmp0,(int)
Cyc_strlen(vs));});else{
return Cyc_PP_text(vs);}}
# 1017
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_Tmp0,t,0);});}
# 1021
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_Tmp0=f;unsigned _Tmp1;void*_Tmp2;if(*((int*)_Tmp0)== 0){_Tmp2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp0)->f1;{struct _fat_ptr*n=_Tmp2;
return Cyc_PP_textptr(n);}}else{_Tmp1=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_Tmp0)->f1;{unsigned n=_Tmp1;
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)n);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(({const char*_Tmp5="%d";_tag_fat(_Tmp5,sizeof(char),3U);}),_tag_fat(_Tmp4,sizeof(void*),1));}));}};}
# 1035 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_stmttmp14=e->r;void*_Tmp0=_stmttmp14;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 1: _LL4:
 return 10000;case 3: _Tmp2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{enum Cyc_Absyn_Primop p=_Tmp2;
# 1040
enum Cyc_Absyn_Primop _Tmp3=p;switch((int)_Tmp3){case Cyc_Absyn_Plus:
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
 return 130;case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return Cyc_Absynpp_exp_prec(e2);}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
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
struct _tuple19*_stmttmp15=(struct _tuple19*)o->hd;struct _tuple19*_Tmp0=_stmttmp15;void*_Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct _fat_ptr c=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
s=({struct Cyc_PP_Doc*_Tmp3[6];_Tmp3[0]=s,({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(({const char*_Tmp5="\"";_tag_fat(_Tmp5,sizeof(char),2U);}));_Tmp3[1]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(c);_Tmp3[2]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(({const char*_Tmp5="\" (";_tag_fat(_Tmp5,sizeof(char),4U);}));_Tmp3[3]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_exp2doc(e);_Tmp3[4]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(({const char*_Tmp5=")";_tag_fat(_Tmp5,sizeof(char),2U);}));_Tmp3[5]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),6));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_Tmp3[2];_Tmp3[0]=s,({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(({const char*_Tmp5=",";_tag_fat(_Tmp5,sizeof(char),2U);}));_Tmp3[1]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1119
return s;}
# 1122
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=": ";_tag_fat(_Tmp2,sizeof(char),3U);}));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(o);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
# 1127
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=": : ";_tag_fat(_Tmp2,sizeof(char),5U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(i);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1131
s=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" : ";_tag_fat(_Tmp2,sizeof(char),4U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(i);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1133
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_Tmp0[2];_Tmp0[0]=s,ncol == 0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=": : :";_tag_fat(_Tmp2,sizeof(char),6U);}));_Tmp0[1]=_Tmp1;}):(ncol == 1?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" : : ";_tag_fat(_Tmp2,sizeof(char),6U);}));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" : ";_tag_fat(_Tmp2,sizeof(char),4U);}));_Tmp0[1]=_Tmp1;}));Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
while(cl != 0){
s=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="\"";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="\"";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_Tmp0[2];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=", ";_tag_fat(_Tmp2,sizeof(char),3U);}));_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1143
return s;}
# 1146
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_stmttmp16=e->r;void*_Tmp0=_stmttmp16;enum Cyc_Absyn_MallocKind _Tmp1;int _Tmp2;enum Cyc_Absyn_Incrementor _Tmp3;void*_Tmp4;void*_Tmp5;enum Cyc_Absyn_Primop _Tmp6;struct _fat_ptr _Tmp7;void*_Tmp8;union Cyc_Absyn_Cnst _Tmp9;switch(*((int*)_Tmp0)){case 0: _Tmp9=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_Tmp9;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1: _Tmp8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp8;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _Tmp7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_Tmp7;
# 1153
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="__cyclone_pragma__";_tag_fat(_TmpC,sizeof(char),19U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(p);_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 3: _Tmp6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp6;struct Cyc_List_List*es=_Tmp8;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4: _Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Core_Opt*popt=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 1156
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" ";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),
popt == 0?({struct Cyc_PP_Doc*_TmpB=Cyc_PP_nil_doc();_TmpA[2]=_TmpB;}):({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="= ";_tag_fat(_TmpC,sizeof(char),3U);}));_TmpA[3]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 5: _Tmp8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp8;enum Cyc_Absyn_Incrementor i=_Tmp3;
# 1163
struct Cyc_PP_Doc*es=Cyc_Absynpp_exp2doc_prec(myprec,e2);
{enum Cyc_Absyn_Incrementor _TmpA=i;switch((int)_TmpA){case Cyc_Absyn_PreInc:
 s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(({const char*_TmpD="++";_tag_fat(_TmpD,sizeof(char),3U);}));_TmpB[0]=_TmpC;}),_TmpB[1]=es;Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PreDec:
 s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(({const char*_TmpD="--";_tag_fat(_TmpD,sizeof(char),3U);}));_TmpB[0]=_TmpC;}),_TmpB[1]=es;Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PostInc:
 s=({struct Cyc_PP_Doc*_TmpB[2];_TmpB[0]=es,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(({const char*_TmpD="++";_tag_fat(_TmpD,sizeof(char),3U);}));_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PostDec:
 goto _LL61;default: _LL61:
 s=({struct Cyc_PP_Doc*_TmpB[2];_TmpB[0]=es,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(({const char*_TmpD="--";_tag_fat(_TmpD,sizeof(char),3U);}));_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;}_LL57:;}
# 1171
goto _LL0;}case 6: _Tmp8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Exp*e3=_Tmp4;
# 1173
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" ? ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(0,e2);_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" : ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e3);_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 7: _Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1177
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" && ";_tag_fat(_TmpC,sizeof(char),5U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 8: _Tmp8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1180
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" || ";_tag_fat(_TmpC,sizeof(char),5U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 9: _Tmp8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1185
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=", ";_tag_fat(_TmpC,sizeof(char),3U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 10: _Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_List_List*es=_Tmp5;
# 1188
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exps2doc_prec(20,es);_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 11: _Tmp8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="throw ";_tag_fat(_TmpC,sizeof(char),7U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 12: _Tmp8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13: _Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14: _Tmp8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp5;
# 1197
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_typ2doc(t);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 15: _Tmp8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="&";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 16: _Tmp8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*ropt=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp5;
# 1200
if(ropt == 0)
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="new ";_tag_fat(_TmpC,sizeof(char),5U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});else{
# 1203
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="rnew(";_tag_fat(_TmpC,sizeof(char),6U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(ropt);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=") ";_tag_fat(_TmpC,sizeof(char),3U);}));_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});}
goto _LL0;}case 17: _Tmp8=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp8;
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="sizeof(";_tag_fat(_TmpC,sizeof(char),8U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_typ2doc(t);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 18: _Tmp8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="sizeof(";_tag_fat(_TmpC,sizeof(char),8U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e1);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 42: _Tmp8=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1208
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="@assert(";_tag_fat(_TmpC,sizeof(char),9U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 41: _Tmp8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1210
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="__extension__(";_tag_fat(_TmpC,sizeof(char),15U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 39: _Tmp8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp8;
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="valueof(";_tag_fat(_TmpC,sizeof(char),9U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_typ2doc(t);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 40: _Tmp2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp5=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_Tmp4=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f5;{int vol=_Tmp2;struct _fat_ptr t=_Tmp7;struct Cyc_List_List*o=_Tmp8;struct Cyc_List_List*i=_Tmp5;struct Cyc_List_List*cl=_Tmp4;
# 1213
if(vol)
s=({struct Cyc_PP_Doc*_TmpA[7];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="__asm__";_tag_fat(_TmpC,sizeof(char),8U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" volatile (";_tag_fat(_TmpC,sizeof(char),12U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="\"";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="\"";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_TmpA[5]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[6]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1216
s=({struct Cyc_PP_Doc*_TmpA[6];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="__asm__(";_tag_fat(_TmpC,sizeof(char),9U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="\"";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="\"";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_TmpA[4]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[5]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),6));});}
goto _LL0;}case 38: _Tmp8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;struct _fat_ptr*f=_Tmp5;
# 1219
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="tagcheck(";_tag_fat(_TmpC,sizeof(char),10U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=".";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_textptr(f);_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 19: _Tmp8=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp8;struct Cyc_List_List*l=_Tmp5;
# 1222
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="offsetof(";_tag_fat(_TmpC,sizeof(char),10U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_typ2doc(t);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=({struct _fat_ptr _TmpC=({const char*_TmpD=".";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_PP_seq(_TmpC,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_offsetof_field_to_doc,l));});_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 20: _Tmp8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="*";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 21: _Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*n=_Tmp5;
# 1227
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=".";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_textptr(n);_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 22: _Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*n=_Tmp5;
# 1229
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="->";_tag_fat(_TmpC,sizeof(char),3U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_textptr(n);_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 23: _Tmp8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1231
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="[";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e2);_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="]";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 24: _Tmp8=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp8;
# 1233
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_dollar();_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exps2doc_prec(20,es);_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 25: _Tmp8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple8*vat=_Tmp8;struct Cyc_List_List*des=_Tmp5;
# 1235
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_typ2doc((*vat).f3);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=({struct _fat_ptr _TmpC=({const char*_TmpD=",";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_Absynpp_group_braces(_TmpC,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 26: _Tmp8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp8;
s=({struct _fat_ptr _TmpA=({const char*_TmpB=",";_tag_fat(_TmpB,sizeof(char),2U);});Cyc_Absynpp_group_braces(_TmpA,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 27: _Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 1240
s=({struct Cyc_PP_Doc*_TmpA[8];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_lb();_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="for ";_tag_fat(_TmpC,sizeof(char),5U);}));_TmpA[1]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(*(*vd->name).f2);_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" < ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e1);_TmpA[4]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" : ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[5]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e2);_TmpA[6]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_rb();_TmpA[7]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 28: _Tmp8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;void*t=_Tmp5;
# 1245
s=({struct Cyc_PP_Doc*_TmpA[7];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_lb();_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="for x ";_tag_fat(_TmpC,sizeof(char),7U);}));_TmpA[1]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" < ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[3]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" : ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[4]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_typ2doc(t);_TmpA[5]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_rb();_TmpA[6]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 29: _Tmp8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct _tuple0*n=_Tmp8;struct Cyc_List_List*ts=_Tmp5;struct Cyc_List_List*des=_Tmp4;
# 1253
struct Cyc_List_List*des_doc=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des);
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_qvar2doc(n);_TmpA[0]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=({struct _fat_ptr _TmpC=({const char*_TmpD=",";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_Absynpp_group_braces(_TmpC,
ts != 0?({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_tps2doc(ts);_TmpD->hd=_TmpE;}),_TmpD->tl=des_doc;_TmpD;}): des_doc);});
# 1255
_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
# 1257
goto _LL0;}case 30: _Tmp8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*des=_Tmp8;
s=({struct _fat_ptr _TmpA=({const char*_TmpB=",";_tag_fat(_TmpB,sizeof(char),2U);});Cyc_Absynpp_group_braces(_TmpA,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 31: _Tmp8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp8;struct Cyc_Absyn_Datatypefield*tuf=_Tmp5;
# 1260
if(es == 0)
# 1262
s=Cyc_Absynpp_qvar2doc(tuf->name);else{
# 1264
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_qvar2doc(tuf->name);_TmpA[0]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=({struct _fat_ptr _TmpC=({const char*_TmpD="(";_tag_fat(_TmpD,sizeof(char),2U);});struct _fat_ptr _TmpD=({const char*_TmpE=")";_tag_fat(_TmpE,sizeof(char),2U);});struct _fat_ptr _TmpE=({const char*_TmpF=",";_tag_fat(_TmpF,sizeof(char),2U);});Cyc_PP_egroup(_TmpC,_TmpD,_TmpE,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_exp2doc,es));});_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});}
goto _LL0;}case 32: _Tmp8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp8;
_Tmp8=ef;goto _LL4E;}case 33: _Tmp8=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_Tmp8;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 34: _Tmp1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).mknd;_Tmp8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).elt_type;_Tmp4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp1;struct Cyc_Absyn_Exp*rgnopt=_Tmp8;void**topt=_Tmp5;struct Cyc_Absyn_Exp*e=_Tmp4;int inline_call=_Tmp2;
# 1272
if((int)mknd == 1){
# 1274
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*_check_null(topt),0U);
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="calloc(";_tag_fat(_TmpC,sizeof(char),8U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(st);_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1278
s=({struct Cyc_PP_Doc*_TmpA[7];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="rcalloc(";_tag_fat(_TmpC,sizeof(char),9U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(rgnopt);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(st);_TmpA[5]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[6]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1281
struct Cyc_Absyn_Exp*new_e;
# 1283
if(topt == 0)
new_e=e;else{
# 1286
new_e=({struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_TmpA,e,0U);});}
# 1288
if(rgnopt == 0)
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="malloc(";_tag_fat(_TmpC,sizeof(char),8U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(new_e);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1291
if(inline_call)
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="rmalloc_inline(";_tag_fat(_TmpC,sizeof(char),16U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(rgnopt);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(new_e);_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});else{
if((int)mknd == 2)
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="rvmalloc(";_tag_fat(_TmpC,sizeof(char),10U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(rgnopt);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(new_e);_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1298
s=({struct Cyc_PP_Doc*_TmpA[5];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="rmalloc(";_tag_fat(_TmpC,sizeof(char),9U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(rgnopt);_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=",";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(new_e);_TmpA[3]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[4]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),5));});}}}}
# 1302
goto _LL0;}case 35: _Tmp8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1304
s=({struct Cyc_PP_Doc*_TmpA[3];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" :=: ";_tag_fat(_TmpC,sizeof(char),6U);}));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpA[2]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 36: _Tmp8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*n=_Tmp8;struct Cyc_List_List*des=_Tmp5;
# 1307
s=({struct _fat_ptr _TmpA=({const char*_TmpB=",";_tag_fat(_TmpB,sizeof(char),2U);});Cyc_Absynpp_group_braces(_TmpA,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _Tmp8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s2=_Tmp8;
# 1310
s=({struct Cyc_PP_Doc*_TmpA[7];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC="(";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_lb();_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_blank_doc();_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_TmpA[3]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_blank_doc();_TmpA[4]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_rb();_TmpA[5]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=")";_tag_fat(_TmpC,sizeof(char),2U);}));_TmpA[6]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}}_LL0:;}
# 1315
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="(";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
return s;}
# 1320
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_Tmp0=d;void*_Tmp1;if(*((int*)_Tmp0)== 0){_Tmp1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
return({struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(({const char*_Tmp4=".[";_tag_fat(_Tmp4,sizeof(char),3U);}));_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_exp2doc(e);_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(({const char*_Tmp4="]";_tag_fat(_Tmp4,sizeof(char),2U);}));_Tmp2[2]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));});}}else{_Tmp1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp0)->f1;{struct _fat_ptr*v=_Tmp1;
return({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(({const char*_Tmp4=".";_tag_fat(_Tmp4,sizeof(char),2U);}));_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_textptr(v);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));});}};}
# 1327
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple16*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);});struct _fat_ptr _Tmp3=({const char*_Tmp4="=";_tag_fat(_Tmp4,sizeof(char),2U);});struct _fat_ptr _Tmp4=({const char*_Tmp5="=";_tag_fat(_Tmp5,sizeof(char),2U);});Cyc_PP_egroup(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_designator2doc,(*de).f1));});_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((*de).f2);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1333
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _Tmp0=({const char*_Tmp1="";_tag_fat(_Tmp1,sizeof(char),1U);});struct _fat_ptr _Tmp1=({const char*_Tmp2="";_tag_fat(_Tmp2,sizeof(char),1U);});struct _fat_ptr _Tmp2=({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);});Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1337
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=({char*_Tmp0=({unsigned _Tmp1=28U + 1U;({char*_Tmp2=_cycalloc_atomic(_check_times(_Tmp1,sizeof(char)));({{unsigned _Tmp3=28U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='z';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;});});_tag_fat(_Tmp0,sizeof(char),29U);});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),27);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),26);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='L';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),25);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='L';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),24);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='U';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),23);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});{
int index=23;
while(i != 0U){
char c=(char)(48U + i % 10U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),index);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
i=i / 10U;
-- index;}
# 1351
return _fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1355
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _Tmp0=c;long long _Tmp1;int _Tmp2;short _Tmp3;struct _fat_ptr _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;switch((_Tmp0.String_c).tag){case 2: _Tmp6=((_Tmp0.Char_c).val).f1;_Tmp5=((_Tmp0.Char_c).val).f2;{enum Cyc_Absyn_Sign sg=_Tmp6;char ch=_Tmp5;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_char_escape(ch);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="'%s'";_tag_fat(_Tmp9,sizeof(char),5U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));}case 3: _Tmp4=(_Tmp0.Wchar_c).val;{struct _fat_ptr s=_Tmp4;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=s;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="L'%s'";_tag_fat(_Tmp9,sizeof(char),6U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));}case 4: _Tmp6=((_Tmp0.Short_c).val).f1;_Tmp3=((_Tmp0.Short_c).val).f2;{enum Cyc_Absyn_Sign sg=_Tmp6;short s=_Tmp3;
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)s);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="%d";_tag_fat(_Tmp9,sizeof(char),3U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));}case 5: _Tmp6=((_Tmp0.Int_c).val).f1;_Tmp2=((_Tmp0.Int_c).val).f2;{enum Cyc_Absyn_Sign sn=_Tmp6;int i=_Tmp2;
# 1361
if((int)sn == 1)return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)i;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="%uU";_tag_fat(_Tmp9,sizeof(char),4U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));else{
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="%d";_tag_fat(_Tmp9,sizeof(char),3U);}),_tag_fat(_Tmp8,sizeof(void*),1));}));}}case 6: _Tmp6=((_Tmp0.LongLong_c).val).f1;_Tmp1=((_Tmp0.LongLong_c).val).f2;{enum Cyc_Absyn_Sign sg=_Tmp6;long long i=_Tmp1;
# 1365
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7: _Tmp4=((_Tmp0.Float_c).val).f1;{struct _fat_ptr x=_Tmp4;
return Cyc_PP_text(x);}case 1:
 return Cyc_PP_text(({const char*_Tmp7="NULL";_tag_fat(_Tmp7,sizeof(char),5U);}));case 8: _Tmp4=(_Tmp0.String_c).val;{struct _fat_ptr s=_Tmp4;
return({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9="\"";_tag_fat(_Tmp9,sizeof(char),2U);}));_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_Tmp7[1]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9="\"";_tag_fat(_Tmp9,sizeof(char),2U);}));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});}default: _Tmp4=(_Tmp0.Wstring_c).val;{struct _fat_ptr s=_Tmp4;
return({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9="L\"";_tag_fat(_Tmp9,sizeof(char),3U);}));_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(s);_Tmp7[1]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9="\"";_tag_fat(_Tmp9,sizeof(char),2U);}));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});}};}
# 1373
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)18U){
if(es == 0 || es->tl != 0)
(void*)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,({struct _fat_ptr _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_PP_string_of_doc(ps,72);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="Absynpp::primapp2doc Numelts: %s with bad args";_tag_fat(_Tmp4,sizeof(char),47U);}),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0->f1=_Tmp1;});_Tmp0;}));
# 1379
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="numelts(";_tag_fat(_Tmp2,sizeof(char),9U);}));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=")";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1381
struct Cyc_List_List*ds=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(void*)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,({struct _fat_ptr _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_PP_string_of_doc(ps,72);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="Absynpp::primapp2doc: %s with no args";_tag_fat(_Tmp4,sizeof(char),38U);}),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0->f1=_Tmp1;});_Tmp0;}));else{
# 1385
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=ps,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" ";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[1]=_Tmp1;}),_Tmp0[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});else{
if((_check_null(ds->tl))->tl != 0)
(void*)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,({struct _fat_ptr _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_PP_string_of_doc(ps,72);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="Absynpp::primapp2doc: %s with more than 2 args";_tag_fat(_Tmp4,sizeof(char),47U);}),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0->f1=_Tmp1;});_Tmp0;}));else{
# 1391
return({struct Cyc_PP_Doc*_Tmp0[5];_Tmp0[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" ";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[1]=_Tmp1;}),_Tmp0[2]=ps,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" ";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[3]=_Tmp1;}),_Tmp0[4]=(struct Cyc_PP_Doc*)(_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));});}}}}}
# 1395
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _Tmp0=p;switch((int)_Tmp0){case Cyc_Absyn_Plus:
 return({const char*_Tmp1="+";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Times:
 return({const char*_Tmp1="*";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Minus:
 return({const char*_Tmp1="-";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Div:
 return({const char*_Tmp1="/";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Mod:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_Tmp1="\\%";_tag_fat(_Tmp1,sizeof(char),3U);}):({const char*_Tmp1="%";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Eq:
 return({const char*_Tmp1="==";_tag_fat(_Tmp1,sizeof(char),3U);});case Cyc_Absyn_Neq:
 return({const char*_Tmp1="!=";_tag_fat(_Tmp1,sizeof(char),3U);});case Cyc_Absyn_Gt:
 return({const char*_Tmp1=">";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Lt:
 return({const char*_Tmp1="<";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Gte:
 return({const char*_Tmp1=">=";_tag_fat(_Tmp1,sizeof(char),3U);});case Cyc_Absyn_Lte:
 return({const char*_Tmp1="<=";_tag_fat(_Tmp1,sizeof(char),3U);});case Cyc_Absyn_Not:
 return({const char*_Tmp1="!";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Bitnot:
 return({const char*_Tmp1="~";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Bitand:
 return({const char*_Tmp1="&";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Bitor:
 return({const char*_Tmp1="|";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Bitxor:
 return({const char*_Tmp1="^";_tag_fat(_Tmp1,sizeof(char),2U);});case Cyc_Absyn_Bitlshift:
 return({const char*_Tmp1="<<";_tag_fat(_Tmp1,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift:
 return({const char*_Tmp1=">>";_tag_fat(_Tmp1,sizeof(char),3U);});case Cyc_Absyn_Numelts:
 return({const char*_Tmp1="numelts";_tag_fat(_Tmp1,sizeof(char),8U);});default:
 return({const char*_Tmp1="?";_tag_fat(_Tmp1,sizeof(char),2U);});};}
# 1420
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1424
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_stmttmp17=s->r;void*_Tmp0=_stmttmp17;if(*((int*)_Tmp0)== 12)
return 1;else{
return 0;};}
# 1439 "absynpp.cyc"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_stmttmp18=d->r;void*_Tmp0=_stmttmp18;void*_Tmp1;if(*((int*)_Tmp0)== 0){_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1442
if(({(int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))Cyc_List_mem;})(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple14 _Tmp2;_Tmp2.f1=1,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd->name,_Tmp4->tl=0;_Tmp4;});_Tmp2.f2=_Tmp3;});_Tmp2;});
return({struct _tuple14 _Tmp2;_Tmp2.f1=0,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd->name,_Tmp4->tl=varsinblock;_Tmp4;});_Tmp2.f2=_Tmp3;});_Tmp2;});}}else{
return({struct _tuple14 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=varsinblock;_Tmp2;});};}
# 1449
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_Tmp0[8];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="(";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,d);_Tmp0[3]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[5]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=");";_tag_fat(_Tmp2,sizeof(char),3U);}));_Tmp0[6]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[7]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),8));});
return({struct Cyc_PP_Doc*_Tmp0[6];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,d);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[3]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[5]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),6));});}
# 1456
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
struct Cyc_PP_Doc*s;
{void*_stmttmp19=st->r;void*_Tmp0=_stmttmp19;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 s=Cyc_PP_text(({const char*_Tmp5=";";_tag_fat(_Tmp5,sizeof(char),2U);}));goto _LL0;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 1462
if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[0]=_Tmp6;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_Tmp5[1]=_Tmp6;}):({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});else{
if(Cyc_Absynpp_is_declaration(s2))
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1473
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}}}else{
# 1477
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,0);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
# 1480
if(eopt == 0)
s=Cyc_PP_text(({const char*_Tmp5="return;";_tag_fat(_Tmp5,sizeof(char),8U);}));else{
# 1483
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="return ";_tag_fat(_Tmp7,sizeof(char),8U);}));_Tmp5[0]=_Tmp6;}),
eopt == 0?({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[1]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(eopt);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 1488
int print_else;
{void*_stmttmp1A=s2->r;void*_Tmp5=_stmttmp1A;if(*((int*)_Tmp5)== 0){
print_else=0;goto _LL23;}else{
print_else=1;goto _LL23;}_LL23:;}
# 1493
s=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="if (";_tag_fat(_Tmp7,sizeof(char),5U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=") ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[3]=_Tmp6;}),
print_else?({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9="else ";_tag_fat(_Tmp9,sizeof(char),6U);}));_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});
# 1498
_Tmp5[4]=_Tmp6;}):({
# 1501
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 5: _Tmp4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1).f1;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1504
s=({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="while (";_tag_fat(_Tmp7,sizeof(char),8U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=") ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 6:
 s=Cyc_PP_text(({const char*_Tmp5="break;";_tag_fat(_Tmp5,sizeof(char),7U);}));goto _LL0;case 7:
 s=Cyc_PP_text(({const char*_Tmp5="continue;";_tag_fat(_Tmp5,sizeof(char),10U);}));goto _LL0;case 8: _Tmp4=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct _fat_ptr*x=_Tmp4;
s=Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*x;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="goto %s;";_tag_fat(_Tmp7,sizeof(char),9U);}),_tag_fat(_Tmp6,sizeof(void*),1));}));goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;_Tmp2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3).f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 1513
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="for(";_tag_fat(_Tmp7,sizeof(char),5U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e1);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="; ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e2);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="; ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e3);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=") ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1523
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="switch (";_tag_fat(_Tmp7,sizeof(char),9U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=") ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_switchclauses2doc(ss);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1 == 0){
s=Cyc_PP_text(({const char*_Tmp5="fallthru;";_tag_fat(_Tmp5,sizeof(char),10U);}));goto _LL0;}else{_Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 1534
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="fallthru(";_tag_fat(_Tmp7,sizeof(char),10U);}));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exps2doc_prec(20,es);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=");";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1536
struct _tuple14 _stmttmp1B=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple14 _Tmp5=_stmttmp1B;void*_Tmp6;int _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{int newblock=_Tmp7;struct Cyc_List_List*newvarsinblock=_Tmp6;
s=({struct Cyc_PP_Doc*_Tmp8[3];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_decl2doc(d);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_line_doc();_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_Tmp8[2]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),3));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
goto _LL0;}}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*x=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1541
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(x);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=": ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1544
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(x);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=": ";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2).f1;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1547
s=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="do ";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" while (";_tag_fat(_Tmp7,sizeof(char),9U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=");";_tag_fat(_Tmp7,sizeof(char),3U);}));_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1554
s=({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="try ";_tag_fat(_Tmp7,sizeof(char),5U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" catch ";_tag_fat(_Tmp7,sizeof(char),8U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}}_LL0:;}
# 1560
return s;}
# 1563
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_stmttmp1C=p->r;void*_Tmp0=_stmttmp1C;union Cyc_Absyn_AggrInfo _Tmp1;void*_Tmp2;void*_Tmp3;struct _fat_ptr _Tmp4;char _Tmp5;int _Tmp6;enum Cyc_Absyn_Sign _Tmp7;switch(*((int*)_Tmp0)){case 0:
 s=Cyc_PP_text(({const char*_Tmp8="_";_tag_fat(_Tmp8,sizeof(char),2U);}));goto _LL0;case 9:
 s=Cyc_PP_text(({const char*_Tmp8="NULL";_tag_fat(_Tmp8,sizeof(char),5U);}));goto _LL0;case 10: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign sg=_Tmp7;int i=_Tmp6;
# 1569
if((int)sg != (int)1U)
s=Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)i;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="%d";_tag_fat(_TmpA,sizeof(char),3U);}),_tag_fat(_Tmp9,sizeof(void*),1));}));else{
s=Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned)i;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="%u";_tag_fat(_TmpA,sizeof(char),3U);}),_tag_fat(_Tmp9,sizeof(void*),1));}));}
goto _LL0;}case 11: _Tmp5=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char ch=_Tmp5;
s=Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_char_escape(ch);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="'%s'";_tag_fat(_TmpA,sizeof(char),5U);}),_tag_fat(_Tmp9,sizeof(void*),1));}));goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr x=_Tmp4;
s=Cyc_PP_text(x);goto _LL0;}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)== 0){_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1576
s=Cyc_Absynpp_qvar2doc(vd->name);goto _LL0;}}else{_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 1578
s=({struct Cyc_PP_Doc*_Tmp8[3];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_qvar2doc(vd->name);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=" as ";_tag_fat(_TmpA,sizeof(char),5U);}));_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_pat2doc(p2);_Tmp8[2]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}}case 2: _Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp3;struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1580
s=({struct Cyc_PP_Doc*_Tmp8[5];({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="alias";_tag_fat(_TmpA,sizeof(char),6U);}));_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=" <";_tag_fat(_TmpA,sizeof(char),3U);}));_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_tvar2doc(tv);_Tmp8[2]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="> ";_tag_fat(_TmpA,sizeof(char),3U);}));_Tmp8[3]=_Tmp9;}),({
struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_vardecl2doc(vd);_Tmp8[4]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 4: _Tmp3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp3;struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1584
s=({struct Cyc_PP_Doc*_Tmp8[4];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_qvar2doc(vd->name);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="<";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_tvar2doc(tv);_Tmp8[2]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=">";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[3]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 5: _Tmp3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp3;int dots=_Tmp6;
# 1587
s=({struct Cyc_PP_Doc*_Tmp8[4];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_dollar();_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="(";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;})(Cyc_Absynpp_pat2doc,({const char*_TmpA=",";_tag_fat(_TmpA,sizeof(char),2U);}),ts);_Tmp8[2]=_Tmp9;}),
dots?({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=", ...)";_tag_fat(_TmpA,sizeof(char),7U);}));_Tmp8[3]=_Tmp9;}):({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=")";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[3]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 6: _Tmp3=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp3;
# 1591
s=({struct Cyc_PP_Doc*_Tmp8[2];({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="&";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_pat2doc(p2);_Tmp8[1]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)== 0){_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1594
s=({struct Cyc_PP_Doc*_Tmp8[2];({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="*";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_qvar2doc(vd->name);_Tmp8[1]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}}else{_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 1597
s=({struct Cyc_PP_Doc*_Tmp8[4];({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA="*";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_qvar2doc(vd->name);_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=" as ";_tag_fat(_TmpA,sizeof(char),5U);}));_Tmp8[2]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_pat2doc(p2);_Tmp8[3]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}}case 15: _Tmp3=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*q=_Tmp3;
# 1600
s=Cyc_Absynpp_qvar2doc(q);
goto _LL0;}case 16: _Tmp3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct _tuple0*q=_Tmp3;struct Cyc_List_List*ps=_Tmp2;int dots=_Tmp6;
# 1603
struct _fat_ptr term=dots?({const char*_Tmp8=", ...)";_tag_fat(_Tmp8,sizeof(char),7U);}):({const char*_Tmp8=")";_tag_fat(_Tmp8,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_Tmp8[2];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_qvar2doc(q);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=({struct _fat_ptr _TmpA=({const char*_TmpB="(";_tag_fat(_TmpB,sizeof(char),2U);});struct _fat_ptr _TmpB=term;struct _fat_ptr _TmpC=({const char*_TmpD=",";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_PP_group(_TmpA,_TmpB,_TmpC,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_pat2doc,ps));});_Tmp8[1]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1 != 0){_Tmp1=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{union Cyc_Absyn_AggrInfo info=_Tmp1;struct Cyc_List_List*exists=_Tmp3;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp6;
# 1607
struct _fat_ptr term=dots?({const char*_Tmp8=", ...}";_tag_fat(_Tmp8,sizeof(char),7U);}):({const char*_Tmp8="}";_tag_fat(_Tmp8,sizeof(char),2U);});
struct _tuple11 _stmttmp1D=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _Tmp8=_stmttmp1D;void*_Tmp9;_Tmp9=_Tmp8.f2;{struct _tuple0*n=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpA[4];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_qvar2doc(n);_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_lb();_TmpA[1]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=({struct _fat_ptr _TmpC=({const char*_TmpD="[";_tag_fat(_TmpD,sizeof(char),2U);});struct _fat_ptr _TmpD=({const char*_TmpE="]";_tag_fat(_TmpE,sizeof(char),2U);});struct _fat_ptr _TmpE=({const char*_TmpF=",";_tag_fat(_TmpF,sizeof(char),2U);});Cyc_PP_egroup(_TmpC,_TmpD,_TmpE,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,exists));});_TmpA[2]=_TmpB;}),({
struct Cyc_PP_Doc*_TmpB=({struct _fat_ptr _TmpC=({const char*_TmpD="";_tag_fat(_TmpD,sizeof(char),1U);});struct _fat_ptr _TmpD=term;struct _fat_ptr _TmpE=({const char*_TmpF=",";_tag_fat(_TmpF,sizeof(char),2U);});Cyc_PP_group(_TmpC,_TmpD,_TmpE,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_dp2doc,dps));});_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}}}else{_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_List_List*exists=_Tmp3;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp6;
# 1614
struct _fat_ptr term=dots?({const char*_Tmp8=", ...}";_tag_fat(_Tmp8,sizeof(char),7U);}):({const char*_Tmp8="}";_tag_fat(_Tmp8,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_Tmp8[3];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_lb();_Tmp8[0]=_Tmp9;}),({
struct Cyc_PP_Doc*_Tmp9=({struct _fat_ptr _TmpA=({const char*_TmpB="[";_tag_fat(_TmpB,sizeof(char),2U);});struct _fat_ptr _TmpB=({const char*_TmpC="]";_tag_fat(_TmpC,sizeof(char),2U);});struct _fat_ptr _TmpC=({const char*_TmpD=",";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_PP_egroup(_TmpA,_TmpB,_TmpC,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,exists));});_Tmp8[1]=_Tmp9;}),({
struct Cyc_PP_Doc*_Tmp9=({struct _fat_ptr _TmpA=({const char*_TmpB="";_tag_fat(_TmpB,sizeof(char),1U);});struct _fat_ptr _TmpB=term;struct _fat_ptr _TmpC=({const char*_TmpD=",";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_PP_group(_TmpA,_TmpB,_TmpC,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_dp2doc,dps));});_Tmp8[2]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}}case 13: _Tmp3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp3;
_Tmp3=ef;goto _LL26;}case 14: _Tmp3=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_Tmp3;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 8: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3 == 0){_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;
s=Cyc_Absynpp_qvar2doc(tuf->name);goto _LL0;}}else{_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;struct Cyc_List_List*ps=_Tmp2;int dots=_Tmp6;
# 1623
struct _fat_ptr term=dots?({const char*_Tmp8=", ...)";_tag_fat(_Tmp8,sizeof(char),7U);}):({const char*_Tmp8=")";_tag_fat(_Tmp8,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_Tmp8[2];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_qvar2doc(tuf->name);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=({struct _fat_ptr _TmpA=({const char*_TmpB="(";_tag_fat(_TmpB,sizeof(char),2U);});struct _fat_ptr _TmpB=term;struct _fat_ptr _TmpC=({const char*_TmpD=",";_tag_fat(_TmpD,sizeof(char),2U);});Cyc_PP_egroup(_TmpA,_TmpB,_TmpC,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_pat2doc,ps));});_Tmp8[1]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}}default: _Tmp3=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 1627
s=Cyc_Absynpp_exp2doc(e);goto _LL0;}}_LL0:;}
# 1629
return s;}
# 1632
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple15*dp){
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);});struct _fat_ptr _Tmp3=({const char*_Tmp4="=";_tag_fat(_Tmp4,sizeof(char),2U);});struct _fat_ptr _Tmp4=({const char*_Tmp5="=";_tag_fat(_Tmp5,sizeof(char),2U);});Cyc_PP_egroup(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_designator2doc,(*dp).f1));});_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc((*dp).f2);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});}
# 1637
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1639
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="default: ";_tag_fat(_Tmp2,sizeof(char),10U);}));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="case ";_tag_fat(_Tmp2,sizeof(char),6U);}));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc(c->pattern);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=": ";_tag_fat(_Tmp2,sizeof(char),3U);}));_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1651
return({struct Cyc_PP_Doc*_Tmp0[6];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="case ";_tag_fat(_Tmp2,sizeof(char),6U);}));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc(c->pattern);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" && ";_tag_fat(_Tmp2,sizeof(char),5U);}));_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(_check_null(c->where_clause));_Tmp0[3]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=": ";_tag_fat(_Tmp2,sizeof(char),3U);}));_Tmp0[4]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[5]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),6));});}}}
# 1659
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_switchclause2doc,({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),cs);}
# 1663
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1667
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2doc(f->name);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2=" = ";_tag_fat(_Tmp2,sizeof(char),4U);}));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(_check_null(f->tag));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1670
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_enumfield2doc,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),fs);}
# 1674
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1678
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;})(Cyc_Absynpp_id2doc,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),vds);}
# 1682
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_Tmp0=vd;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;unsigned _Tmp5;void*_Tmp6;enum Cyc_Absyn_Scope _Tmp7;_Tmp7=_Tmp0->sc;_Tmp6=_Tmp0->name;_Tmp5=_Tmp0->varloc;_Tmp4=_Tmp0->tq;_Tmp3=_Tmp0->type;_Tmp2=_Tmp0->initializer;_Tmp1=_Tmp0->attributes;{enum Cyc_Absyn_Scope sc=_Tmp7;struct _tuple0*name=_Tmp6;unsigned varloc=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*type=_Tmp3;struct Cyc_Absyn_Exp*initializer=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _Tmp8=Cyc_Flags_c_compiler;if(_Tmp8 == Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL3;}else{
# 1691
{void*_stmttmp1E=Cyc_Absyn_compress(type);void*_Tmp9=_stmttmp1E;void*_TmpA;if(*((int*)_Tmp9)== 5){_TmpA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp9)->f1).attributes;{struct Cyc_List_List*atts2=_TmpA;
# 1693
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{
beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1697
goto _LL3;}_LL3:;}{
# 1700
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Flags_C_Compilers _Tmp8=Cyc_Flags_c_compiler;if(_Tmp8 == Cyc_Flags_Gcc_c){
tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{
tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1705
s=({struct Cyc_PP_Doc*_Tmp8[5];_Tmp8[0]=tmp_doc,({
# 1707
struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_scope2doc(sc);_Tmp8[1]=_Tmp9;}),({
struct Cyc_PP_Doc*_Tmp9=({struct Cyc_Absyn_Tqual _TmpA=tq;void*_TmpB=type;Cyc_Absynpp_tqtd2doc(_TmpA,_TmpB,({struct Cyc_Core_Opt*_TmpC=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_TmpD=({struct Cyc_PP_Doc*_TmpE[2];_TmpE[0]=beforenamedoc,_TmpE[1]=sn;Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),2));});_TmpC->v=_TmpD;});_TmpC;}));});_Tmp8[2]=_Tmp9;}),
initializer == 0?({
struct Cyc_PP_Doc*_Tmp9=Cyc_PP_nil_doc();_Tmp8[3]=_Tmp9;}):({
struct Cyc_PP_Doc*_Tmp9=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(({const char*_TmpC=" = ";_tag_fat(_TmpC,sizeof(char),4U);}));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(initializer);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});_Tmp8[3]=_Tmp9;}),({
struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(({const char*_TmpA=";";_tag_fat(_TmpA,sizeof(char),2U);}));_Tmp8[4]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),5));});
return s;}}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1716
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
struct _tuple20 _stmttmp1F=*x;struct _tuple20 _Tmp0=_stmttmp1F;void*_Tmp1;_Tmp1=_Tmp0.f2;{struct _tuple0*v=_Tmp1;
return Cyc_Absynpp_qvar2doc(v);}}
# 1721
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(ad->sc);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_aggr_kind2doc(ad->kind);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2bolddoc(ad->name);_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->tvs);_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1728
return({struct Cyc_PP_Doc*_Tmp0[12];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(ad->sc);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_aggr_kind2doc(ad->kind);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2bolddoc(ad->name);_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->tvs);_Tmp0[3]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[5]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc((_check_null(ad->impl))->exist_vars);_Tmp0[6]=_Tmp1;}),
(_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[7]=_Tmp1;}):({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(({const char*_Tmp4=":";_tag_fat(_Tmp4,sizeof(char),2U);}));_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_rgnpo2doc((_check_null(ad->impl))->rgn_po);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));});_Tmp0[7]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_aggrfields2doc((_check_null(ad->impl))->fields);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[8]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[9]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[10]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(ad->attributes);_Tmp0[11]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),12));});}}
# 1742
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_Tmp0=dd;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Scope _Tmp5;_Tmp5=_Tmp0->sc;_Tmp4=_Tmp0->name;_Tmp3=_Tmp0->tvs;_Tmp2=_Tmp0->fields;_Tmp1=_Tmp0->is_extensible;{enum Cyc_Absyn_Scope sc=_Tmp5;struct _tuple0*name=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_Core_Opt*fields=_Tmp2;int is_x=_Tmp1;
if(fields == 0)
return({struct Cyc_PP_Doc*_Tmp6[5];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_scope2doc(sc);_Tmp6[0]=_Tmp7;}),
is_x?({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(({const char*_Tmp8="@extensible ";_tag_fat(_Tmp8,sizeof(char),13U);}));_Tmp6[1]=_Tmp7;}):({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_blank_doc();_Tmp6[1]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(({const char*_Tmp8="datatype ";_tag_fat(_Tmp8,sizeof(char),10U);}));_Tmp6[2]=_Tmp7;}),
is_x?({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_qvar2bolddoc(name);_Tmp6[3]=_Tmp7;}):({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_typedef_name2bolddoc(name);_Tmp6[3]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ktvars2doc(tvs);_Tmp6[4]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1751
return({struct Cyc_PP_Doc*_Tmp6[10];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_scope2doc(sc);_Tmp6[0]=_Tmp7;}),
is_x?({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(({const char*_Tmp8="@extensible ";_tag_fat(_Tmp8,sizeof(char),13U);}));_Tmp6[1]=_Tmp7;}):({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_blank_doc();_Tmp6[1]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(({const char*_Tmp8="datatype ";_tag_fat(_Tmp8,sizeof(char),10U);}));_Tmp6[2]=_Tmp7;}),
is_x?({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_qvar2bolddoc(name);_Tmp6[3]=_Tmp7;}):({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_typedef_name2bolddoc(name);_Tmp6[3]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ktvars2doc(tvs);_Tmp6[4]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_blank_doc();_Tmp6[5]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_lb();_Tmp6[6]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp8[2];({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_line_doc();_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_Tmp8[1]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp6[7]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_line_doc();_Tmp6[8]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_rb();_Tmp6[9]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),10));});}}}
# 1762
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_Tmp0=ed;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Scope _Tmp3;_Tmp3=_Tmp0->sc;_Tmp2=_Tmp0->name;_Tmp1=_Tmp0->fields;{enum Cyc_Absyn_Scope sc=_Tmp3;struct _tuple0*n=_Tmp2;struct Cyc_Core_Opt*fields=_Tmp1;
if(fields == 0)
return({struct Cyc_PP_Doc*_Tmp4[3];({struct Cyc_PP_Doc*_Tmp5=Cyc_Absynpp_scope2doc(sc);_Tmp4[0]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_PP_text(({const char*_Tmp6="enum ";_tag_fat(_Tmp6,sizeof(char),6U);}));_Tmp4[1]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_Absynpp_typedef_name2bolddoc(n);_Tmp4[2]=_Tmp5;});Cyc_PP_cat(_tag_fat(_Tmp4,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1770
return({struct Cyc_PP_Doc*_Tmp4[8];({struct Cyc_PP_Doc*_Tmp5=Cyc_Absynpp_scope2doc(sc);_Tmp4[0]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_PP_text(({const char*_Tmp6="enum ";_tag_fat(_Tmp6,sizeof(char),6U);}));_Tmp4[1]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_Absynpp_qvar2bolddoc(n);_Tmp4[2]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_PP_blank_doc();_Tmp4[3]=_Tmp5;}),({struct Cyc_PP_Doc*_Tmp5=Cyc_Absynpp_lb();_Tmp4[4]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp6[2];({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_line_doc();_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_Tmp6[1]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp4[5]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_PP_line_doc();_Tmp4[6]=_Tmp5;}),({
struct Cyc_PP_Doc*_Tmp5=Cyc_Absynpp_rb();_Tmp4[7]=_Tmp5;});Cyc_PP_cat(_tag_fat(_Tmp4,sizeof(struct Cyc_PP_Doc*),8));});}}}
# 1779
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_stmttmp20=d->r;void*_Tmp0=_stmttmp20;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1783
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=5,_Tmp5->f1=type_info;_Tmp5;});
if(fd->cached_type != 0){
void*_stmttmp21=Cyc_Absyn_compress(_check_null(fd->cached_type));void*_Tmp5=_stmttmp21;struct Cyc_Absyn_FnInfo _Tmp6;if(*((int*)_Tmp5)== 5){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp6;
# 1789
({struct Cyc_List_List*_Tmp7=Cyc_List_append((fd->i).attributes,i.attributes);(fd->i).attributes=_Tmp7;});goto _LL23;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp7="function has non-function type";_tag_fat(_Tmp7,sizeof(char),31U);}),_tag_fat(0U,sizeof(void*),0));}_LL23:;}{
# 1792
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((fd->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;if(_Tmp5 == Cyc_Flags_Gcc_c){
inlinedoc=Cyc_PP_text(({const char*_Tmp6="inline ";_tag_fat(_Tmp6,sizeof(char),8U);}));goto _LL28;}else{
inlinedoc=Cyc_PP_text(({const char*_Tmp6="__inline ";_tag_fat(_Tmp6,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1800
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;if(_Tmp5 == Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL2D;}else{
beforenamedoc=Cyc_Absynpp_callconv2doc((fd->i).attributes);goto _LL2D;}_LL2D:;}{
# 1807
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);void*_Tmp6=t;Cyc_Absynpp_tqtd2doc(_Tmp5,_Tmp6,({struct Cyc_Core_Opt*_Tmp7=_cycalloc(sizeof(struct Cyc_Core_Opt));({
struct Cyc_PP_Doc*_Tmp8=({struct Cyc_PP_Doc*_Tmp9[2];_Tmp9[0]=beforenamedoc,_Tmp9[1]=namedoc;Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});_Tmp7->v=_Tmp8;});_Tmp7;}));});
# 1815
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp7[2];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_stmt2doc(fd->body,0,0);_Tmp7[1]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
s=({struct Cyc_PP_Doc*_Tmp5[4];_Tmp5[0]=inlinedoc,_Tmp5[1]=scopedoc,_Tmp5[2]=tqtddoc,_Tmp5[3]=bodydoc;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
# 1821
{enum Cyc_Flags_C_Compilers _Tmp5=Cyc_Flags_c_compiler;if(_Tmp5 == Cyc_Flags_Vc_c){
s=({struct Cyc_PP_Doc*_Tmp6[2];_Tmp6[0]=attsdoc,_Tmp6[1]=s;Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),2));});goto _LL32;}else{
goto _LL32;}_LL32:;}
# 1826
goto _LL0;}}}}}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 1829
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_aggrdecl2doc(ad);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 1832
s=Cyc_Absynpp_vardecl2doc(vd);
goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp2;
# 1835
s=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="region";_tag_fat(_Tmp7,sizeof(char),7U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="<";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_tvar2doc(tv);_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=">";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(vd->name);_Tmp5[4]=_Tmp6;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9=" = open(";_tag_fat(_Tmp9,sizeof(char),9U);}));_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_exp2doc(open_exp_opt);_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(({const char*_Tmp9=")";_tag_fat(_Tmp9,sizeof(char),2U);}));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});
# 1840
_Tmp5[5]=_Tmp6;}):({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
# 1845
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_datatypedecl2doc(dd);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1848
s=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="let ";_tag_fat(_Tmp7,sizeof(char),5U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_pat2doc(p);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" = ";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp4;
# 1855
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="let ";_tag_fat(_Tmp7,sizeof(char),5U);}));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ids2doc(vds);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
# 1858
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_enumdecl2doc(ed);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 1861
void*t;
if(td->defn != 0)
t=_check_null(td->defn);else{
# 1865
t=Cyc_Absyn_new_evar(td->kind,0);}
s=({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="typedef ";_tag_fat(_Tmp7,sizeof(char),9U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_Absyn_Tqual _Tmp7=td->tq;void*_Tmp8=t;Cyc_Absynpp_tqtd2doc(_Tmp7,_Tmp8,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));({
# 1869
struct Cyc_PP_Doc*_TmpA=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typedef_name2bolddoc(td->name);_TmpB[0]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_tvars2doc(td->tvs);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});
# 1869
_Tmp9->v=_TmpA;});_Tmp9;}));});
# 1867
_Tmp5[1]=_Tmp6;}),({
# 1872
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_atts2doc(td->atts);_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=";";_tag_fat(_Tmp7,sizeof(char),2U);}));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
# 1875
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1877
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="namespace ";_tag_fat(_Tmp7,sizeof(char),11U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(v);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple0*q=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1888
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="using ";_tag_fat(_Tmp7,sizeof(char),7U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(q);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1897
s=({struct Cyc_PP_Doc*_Tmp5[11];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="/* using ";_tag_fat(_Tmp7,sizeof(char),10U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(q);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" */";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="/* ";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[8]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[9]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" */";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[10]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*tdl=_Tmp4;
# 1910
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="extern \"C\" ";_tag_fat(_Tmp7,sizeof(char),12U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[5]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});else{
# 1918
s=({struct Cyc_PP_Doc*_Tmp5[9];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="/* extern \"C\" ";_tag_fat(_Tmp7,sizeof(char),15U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" */";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="/* ";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" */";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[8]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*tdl=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple10*wc=_Tmp1;
# 1929
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs != 0)
exs_doc=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" export ";_tag_fat(_Tmp7,sizeof(char),9U);}));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct _tuple20*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_export2doc,({const char*_Tmp7=",";_tag_fat(_Tmp7,sizeof(char),2U);}),exs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1937
exs_doc=Cyc_Absynpp_rb();}
if(ovrs != 0)
ovrs_doc=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" cycdef ";_tag_fat(_Tmp7,sizeof(char),9U);}));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),ovrs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1943
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="extern \"C include\" ";_tag_fat(_Tmp7,sizeof(char),20U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),_Tmp5[5]=exs_doc;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});}else{
# 1951
s=({struct Cyc_PP_Doc*_Tmp5[9];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="/* extern \"C include\" ";_tag_fat(_Tmp7,sizeof(char),23U);}));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" */";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);}),tdl);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="/* ";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7=" */";_tag_fat(_Tmp7,sizeof(char),4U);}));_Tmp5[8]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 13:
# 1962
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="__cyclone_port_on__;";_tag_fat(_Tmp7,sizeof(char),21U);}));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;case 14:
# 1965
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="__cyclone_port_off__;";_tag_fat(_Tmp7,sizeof(char),22U);}));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;case 15:
# 1968
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="__tempest_on__;";_tag_fat(_Tmp7,sizeof(char),16U);}));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;default:
# 1971
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(({const char*_Tmp7="__tempest_off__;";_tag_fat(_Tmp7,sizeof(char),17U);}));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}_LL0:;}
# 1975
return s;}
# 1978
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _Tmp0=sc;switch((int)_Tmp0){case Cyc_Absyn_Static:
 return Cyc_PP_text(({const char*_Tmp1="static ";_tag_fat(_Tmp1,sizeof(char),8U);}));case Cyc_Absyn_Public:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
 return Cyc_PP_text(({const char*_Tmp1="extern ";_tag_fat(_Tmp1,sizeof(char),8U);}));case Cyc_Absyn_ExternC:
 return Cyc_PP_text(({const char*_Tmp1="extern \"C\" ";_tag_fat(_Tmp1,sizeof(char),12U);}));case Cyc_Absyn_Abstract:
 return Cyc_PP_text(({const char*_Tmp1="abstract ";_tag_fat(_Tmp1,sizeof(char),10U);}));case Cyc_Absyn_Register:
 return Cyc_PP_text(({const char*_Tmp1="register ";_tag_fat(_Tmp1,sizeof(char),10U);}));default:
 return Cyc_PP_nil_doc();};}}
# 1992
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_stmttmp22=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp22;void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 9){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5:
 return 0;};}
# 2004
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_Tmp0=t;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 7:
 return 1;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 16)
return 1;else{goto _LL7;}case 8: _Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;_Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;if(_Tmp1 != 0){struct Cyc_Absyn_Typedefdecl*td=_Tmp2;void*x=_Tmp1;
# 2011
return Cyc_Absynpp_is_anon_aggrtype(x);}else{goto _LL7;}default: _LL7:
 return 0;};}
# 2018
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2021
if(tms != 0 && tms->tl != 0){
struct _tuple18 _stmttmp23=({struct _tuple18 _Tmp0;_Tmp0.f1=(void*)tms->hd,_Tmp0.f2=(void*)(_check_null(tms->tl))->hd;_Tmp0;});struct _tuple18 _Tmp0=_stmttmp23;if(*((int*)_Tmp0.f1)== 2){if(*((int*)_Tmp0.f2)== 3)
# 2024
return({struct Cyc_List_List*_Tmp1=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)(_check_null(tms->tl))->hd,({struct Cyc_List_List*_Tmp4=Cyc_Absynpp_bubble_attributes(r,atts,(_check_null(tms->tl))->tl);_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1->tl=_Tmp2;});_Tmp1;});else{goto _LL3;}}else{_LL3:
 return({struct Cyc_List_List*_Tmp1=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp1->hd=atts,_Tmp1->tl=tms;_Tmp1;});};}else{
# 2027
return({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=atts,_Tmp0->tl=tms;_Tmp0;});}}
# 2030
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr s=Cyc_strconcat(({const char*_Tmp0="`";_tag_fat(_Tmp0,sizeof(char),2U);}),*t->name);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='t';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr*_Tmp0=({unsigned _Tmp1=1;({struct _fat_ptr*_Tmp2=_cycalloc(_check_times(_Tmp1,sizeof(struct _fat_ptr)));_Tmp2[0]=s;_Tmp2;});});t->name=_Tmp0;});}}
# 2039
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2041
void*_Tmp0=t;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;struct Cyc_Absyn_PtrAtts _Tmp8;unsigned _Tmp9;void*_TmpA;void*_TmpB;struct Cyc_Absyn_Tqual _TmpC;void*_TmpD;switch(*((int*)_Tmp0)){case 4: _TmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_TmpC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).tq;_TmpB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).num_elts;_TmpA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;_Tmp9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zt_loc;{void*t2=_TmpD;struct Cyc_Absyn_Tqual tq2=_TmpC;struct Cyc_Absyn_Exp*e=_TmpB;void*zeroterm=_TmpA;unsigned ztl=_Tmp9;
# 2044
struct _tuple13 _stmttmp24=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _TmpE=_stmttmp24;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;_Tmp11=_TmpE.f1;_Tmp10=_TmpE.f2;_TmpF=_TmpE.f3;{struct Cyc_Absyn_Tqual tq3=_Tmp11;void*t3=_Tmp10;struct Cyc_List_List*tml3=_TmpF;
void*tm;
if(e == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp12=_region_malloc(r,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp12->tag=0,_Tmp12->f1=zeroterm,_Tmp12->f2=ztl;_Tmp12;});else{
# 2049
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp12=_region_malloc(r,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp12->tag=1,_Tmp12->f1=e,_Tmp12->f2=zeroterm,_Tmp12->f3=ztl;_Tmp12;});}
return({struct _tuple13 _Tmp12;_Tmp12.f1=tq3,_Tmp12.f2=t3,({struct Cyc_List_List*_Tmp13=({struct Cyc_List_List*_Tmp14=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp14->hd=tm,_Tmp14->tl=tml3;_Tmp14;});_Tmp12.f3=_Tmp13;});_Tmp12;});}}case 3: _TmpD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_TmpC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_tq;_Tmp8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts;{void*t2=_TmpD;struct Cyc_Absyn_Tqual tq2=_TmpC;struct Cyc_Absyn_PtrAtts ptratts=_Tmp8;
# 2053
struct _tuple13 _stmttmp25=Cyc_Absynpp_to_tms(r,tq2,t2);struct _tuple13 _TmpE=_stmttmp25;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;_Tmp11=_TmpE.f1;_Tmp10=_TmpE.f2;_TmpF=_TmpE.f3;{struct Cyc_Absyn_Tqual tq3=_Tmp11;void*t3=_Tmp10;struct Cyc_List_List*tml3=_TmpF;
tml3=({struct Cyc_List_List*_Tmp12=_region_malloc(r,sizeof(struct Cyc_List_List));({void*_Tmp13=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp14=_region_malloc(r,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp14->tag=2,_Tmp14->f1=ptratts,_Tmp14->f2=tq;_Tmp14;});_Tmp12->hd=_Tmp13;}),_Tmp12->tl=tml3;_Tmp12;});
return({struct _tuple13 _Tmp12;_Tmp12.f1=tq3,_Tmp12.f2=t3,_Tmp12.f3=tml3;_Tmp12;});}}case 5: _TmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).tvars;_TmpB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).effect;_TmpC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_tqual;_TmpA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_type;_Tmp7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).args;_Tmp6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).c_varargs;_Tmp5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).cyc_varargs;_Tmp4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).rgn_po;_Tmp3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).attributes;_Tmp2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).requires_clause;_Tmp1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ensures_clause;{struct Cyc_List_List*typvars=_TmpD;void*effopt=_TmpB;struct Cyc_Absyn_Tqual t2qual=_TmpC;void*t2=_TmpA;struct Cyc_List_List*args=_Tmp7;int c_varargs=_Tmp6;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp5;struct Cyc_List_List*rgn_po=_Tmp4;struct Cyc_List_List*fn_atts=_Tmp3;struct Cyc_Absyn_Exp*req=_Tmp2;struct Cyc_Absyn_Exp*ens=_Tmp1;
# 2059
if(!Cyc_Absynpp_print_all_tvars){
# 2063
if(effopt == 0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2068
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2071
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2076
struct _tuple13 _stmttmp26=Cyc_Absynpp_to_tms(r,t2qual,t2);struct _tuple13 _TmpE=_stmttmp26;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;_Tmp11=_TmpE.f1;_Tmp10=_TmpE.f2;_TmpF=_TmpE.f3;{struct Cyc_Absyn_Tqual tq3=_Tmp11;void*t3=_Tmp10;struct Cyc_List_List*tml3=_TmpF;
struct Cyc_List_List*tms=tml3;
# 2087 "absynpp.cyc"
{enum Cyc_Flags_C_Compilers _Tmp12=Cyc_Flags_c_compiler;if(_Tmp12 == Cyc_Flags_Gcc_c){
# 2089
if(fn_atts != 0)
# 2092
tms=({struct _RegionHandle*_Tmp13=r;void*_Tmp14=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp15->tag=5,_Tmp15->f1=0U,_Tmp15->f2=fn_atts;_Tmp15;});Cyc_Absynpp_bubble_attributes(_Tmp13,_Tmp14,tms);});
# 2094
tms=({struct Cyc_List_List*_Tmp13=_region_malloc(r,sizeof(struct Cyc_List_List));({void*_Tmp14=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));
_Tmp15->tag=3,({void*_Tmp16=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp17=_region_malloc(r,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp17->tag=1,_Tmp17->f1=args,_Tmp17->f2=c_varargs,_Tmp17->f3=cyc_varargs,_Tmp17->f4=effopt,_Tmp17->f5=rgn_po,_Tmp17->f6=req,_Tmp17->f7=ens;_Tmp17;});_Tmp15->f1=_Tmp16;});_Tmp15;});
# 2094
_Tmp13->hd=_Tmp14;}),_Tmp13->tl=tms;_Tmp13;});
# 2098
goto _LL16;}else{
# 2100
tms=({struct Cyc_List_List*_Tmp13=_region_malloc(r,sizeof(struct Cyc_List_List));({void*_Tmp14=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));
_Tmp15->tag=3,({void*_Tmp16=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp17=_region_malloc(r,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp17->tag=1,_Tmp17->f1=args,_Tmp17->f2=c_varargs,_Tmp17->f3=cyc_varargs,_Tmp17->f4=effopt,_Tmp17->f5=rgn_po,_Tmp17->f6=req,_Tmp17->f7=ens;_Tmp17;});_Tmp15->f1=_Tmp16;});_Tmp15;});
# 2100
_Tmp13->hd=_Tmp14;}),_Tmp13->tl=tms;_Tmp13;});
# 2104
for(1;fn_atts != 0;fn_atts=fn_atts->tl){
void*_stmttmp27=(void*)fn_atts->hd;void*_Tmp13=_stmttmp27;switch(*((int*)_Tmp13)){case 1:
 goto _LL1F;case 2: _LL1F:
 goto _LL21;case 3: _LL21:
# 2109
 tms=({struct Cyc_List_List*_Tmp14=_region_malloc(r,sizeof(struct Cyc_List_List));({void*_Tmp15=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp16->tag=5,_Tmp16->f1=0U,({struct Cyc_List_List*_Tmp17=({struct Cyc_List_List*_Tmp18=_cycalloc(sizeof(struct Cyc_List_List));_Tmp18->hd=(void*)fn_atts->hd,_Tmp18->tl=0;_Tmp18;});_Tmp16->f2=_Tmp17;});_Tmp16;});_Tmp14->hd=_Tmp15;}),_Tmp14->tl=tms;_Tmp14;});
goto AfterAtts;default:
 goto _LL1B;}_LL1B:;}
# 2113
goto _LL16;}_LL16:;}
# 2116
AfterAtts:
 if(typvars != 0)
tms=({struct Cyc_List_List*_Tmp12=_region_malloc(r,sizeof(struct Cyc_List_List));({void*_Tmp13=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp14=_region_malloc(r,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp14->tag=4,_Tmp14->f1=typvars,_Tmp14->f2=0U,_Tmp14->f3=1;_Tmp14;});_Tmp12->hd=_Tmp13;}),_Tmp12->tl=tms;_Tmp12;});
return({struct _tuple13 _Tmp12;_Tmp12.f1=tq3,_Tmp12.f2=t3,_Tmp12.f3=tms;_Tmp12;});}}}case 1: _TmpD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;_TmpB=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_Core_Opt*k=_TmpD;void*topt=_TmpB;int i=_Tmp6;
# 2122
if(topt == 0)
return({struct _tuple13 _TmpE;_TmpE.f1=tq,_TmpE.f2=t,_TmpE.f3=0;_TmpE;});else{
# 2125
return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8: _TmpD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_TmpB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_TmpA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;_Tmp7=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;{struct _tuple0*n=_TmpD;struct Cyc_List_List*ts=_TmpB;struct Cyc_Absyn_Typedefdecl*td=_TmpA;void*topt=_Tmp7;
# 2131
if(topt == 0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple13 _TmpE;_TmpE.f1=tq,_TmpE.f2=t,_TmpE.f3=0;_TmpE;});else{
# 2134
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}}default:
# 2139
 return({struct _tuple13 _TmpE;_TmpE.f1=tq,_TmpE.f2=t,_TmpE.f3=0;_TmpE;});};}
# 2143
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2145
void*_Tmp0=t;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;if(_Tmp1 != 0){void*def=_Tmp1;
return Cyc_Absynpp_is_char_ptr(def);}else{goto _LL5;}case 3: _Tmp1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;{void*elt_typ=_Tmp1;
# 2148
L: {
void*_Tmp2=elt_typ;void*_Tmp3;switch(*((int*)_Tmp2)){case 1: _Tmp3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f2;if(_Tmp3 != 0){void*t=_Tmp3;
elt_typ=t;goto L;}else{goto _LLE;}case 8: _Tmp3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f4;if(_Tmp3 != 0){void*t=_Tmp3;
elt_typ=t;goto L;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f2 == Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;};}}default: _LL5:
# 2155
 return 0;};}
# 2159
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct _tuple13 _stmttmp28=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple13 _Tmp1=_stmttmp28;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;struct Cyc_List_List*tms=_Tmp2;
tms=Cyc_List_imp_rev(tms);
if(tms == 0 && dopt == 0){
struct Cyc_PP_Doc*_Tmp5=({struct Cyc_PP_Doc*_Tmp6[2];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_tqual2doc(tq);_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ntyp2doc(t);_Tmp6[1]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _Tmp5;}else{
# 2166
struct Cyc_PP_Doc*_Tmp5=({struct Cyc_PP_Doc*_Tmp6[4];({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_tqual2doc(tq);_Tmp6[0]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ntyp2doc(t);_Tmp6[1]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(({const char*_Tmp8=" ";_tag_fat(_Tmp8,sizeof(char),2U);}));_Tmp6[2]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=({int _Tmp8=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_Tmp9=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_Tmp8,_Tmp9,tms);});_Tmp6[3]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _Tmp5;}}}
# 2161
;_pop_region();}
# 2174
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*req=f->requires_clause;
if((unsigned)req)
requires_doc=({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(({const char*_Tmp2="@requires ";_tag_fat(_Tmp2,sizeof(char),11U);}));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(req);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});else{
# 2180
requires_doc=Cyc_PP_nil_doc();}{
# 2182
enum Cyc_Flags_C_Compilers _Tmp0=Cyc_Flags_c_compiler;if(_Tmp0 == Cyc_Flags_Gcc_c){
# 2185
if(f->width != 0)
return({struct Cyc_PP_Doc*_Tmp1[5];({struct Cyc_PP_Doc*_Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=f->tq;void*_Tmp4=f->type;Cyc_Absynpp_tqtd2doc(_Tmp3,_Tmp4,({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(f->name);_Tmp5->v=_Tmp6;});_Tmp5;}));});_Tmp1[0]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=":";_tag_fat(_Tmp3,sizeof(char),2U);}));_Tmp1[1]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_exp2doc(_check_null(f->width));_Tmp1[2]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_atts2doc(f->attributes);_Tmp1[3]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=";";_tag_fat(_Tmp3,sizeof(char),2U);}));_Tmp1[4]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),5));});
return({struct Cyc_PP_Doc*_Tmp1[4];({struct Cyc_PP_Doc*_Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=f->tq;void*_Tmp4=f->type;Cyc_Absynpp_tqtd2doc(_Tmp3,_Tmp4,({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(f->name);_Tmp5->v=_Tmp6;});_Tmp5;}));});_Tmp1[0]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_atts2doc(f->attributes);_Tmp1[1]=_Tmp2;}),_Tmp1[2]=requires_doc,({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=";";_tag_fat(_Tmp3,sizeof(char),2U);}));_Tmp1[3]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),4));});}else{
# 2192
if(f->width != 0)
return({struct Cyc_PP_Doc*_Tmp1[5];({struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_atts2doc(f->attributes);_Tmp1[0]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=f->tq;void*_Tmp4=f->type;Cyc_Absynpp_tqtd2doc(_Tmp3,_Tmp4,({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(f->name);_Tmp5->v=_Tmp6;});_Tmp5;}));});_Tmp1[1]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=":";_tag_fat(_Tmp3,sizeof(char),2U);}));_Tmp1[2]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_exp2doc(_check_null(f->width));_Tmp1[3]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=";";_tag_fat(_Tmp3,sizeof(char),2U);}));_Tmp1[4]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),5));});
return({struct Cyc_PP_Doc*_Tmp1[4];({struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_atts2doc(f->attributes);_Tmp1[0]=_Tmp2;}),({
struct Cyc_PP_Doc*_Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=f->tq;void*_Tmp4=f->type;Cyc_Absynpp_tqtd2doc(_Tmp3,_Tmp4,({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(f->name);_Tmp5->v=_Tmp6;});_Tmp5;}));});_Tmp1[1]=_Tmp2;}),_Tmp1[2]=requires_doc,({
struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(({const char*_Tmp3=";";_tag_fat(_Tmp3,sizeof(char),2U);}));_Tmp1[3]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),4));});};}}
# 2203
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_aggrfield2doc,({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);}),fields);}
# 2207
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(f->sc);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_typedef_name2doc(f->name);_Tmp0[1]=_Tmp1;}),
f->typs == 0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[2]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_args2doc(f->typs);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 2212
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_datatypefield2doc,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),fields);}
# 2221
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(1;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_Tmp0=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_Tmp0,72,f);});
Cyc_fprintf(f,({const char*_Tmp0="\n";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}}
# 2228
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _Tmp0=({const char*_Tmp1="";_tag_fat(_Tmp1,sizeof(char),1U);});Cyc_PP_seql(_Tmp0,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2231
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
# 2245
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
# 2250
struct _fat_ptr Cyc_Absynpp_get_type_kind(void*t){
if(t == 0)
return({const char*_Tmp0="NULL";_tag_fat(_Tmp0,sizeof(char),5U);});{
void*_Tmp0=t;void*_Tmp1;switch(*((int*)_Tmp0)){case 0:
 return({const char*_Tmp2="AppType";_tag_fat(_Tmp2,sizeof(char),8U);});case 1:
 return({const char*_Tmp2="Evar";_tag_fat(_Tmp2,sizeof(char),5U);});case 2:
 return({const char*_Tmp2="Vartype";_tag_fat(_Tmp2,sizeof(char),8U);});case 3:
 return({const char*_Tmp2="Pointertype";_tag_fat(_Tmp2,sizeof(char),12U);});case 4:
 return({const char*_Tmp2="ArrayType";_tag_fat(_Tmp2,sizeof(char),10U);});case 5:
 return({const char*_Tmp2="FnType";_tag_fat(_Tmp2,sizeof(char),7U);});case 6:
 return({const char*_Tmp2="TupleType";_tag_fat(_Tmp2,sizeof(char),10U);});case 7:
 return({const char*_Tmp2="AnonAggrType";_tag_fat(_Tmp2,sizeof(char),13U);});case 8: _Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 2263
return({const char*_Tmp2="Typedeftype";_tag_fat(_Tmp2,sizeof(char),12U);});}case 9:
 return({const char*_Tmp2="Valueoftype";_tag_fat(_Tmp2,sizeof(char),12U);});case 10:
 return({const char*_Tmp2="Typedecltype";_tag_fat(_Tmp2,sizeof(char),13U);});default:
 return({const char*_Tmp2="Typeoftype";_tag_fat(_Tmp2,sizeof(char),11U);});};}}
