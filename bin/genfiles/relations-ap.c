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
/* should be size_t but int is fine */
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
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
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
      _curr != (unsigned char*)0) \
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
void cyc_vfree(void*);
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

# 168 "core.h"
 extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 86 "list.h"
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 469 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 494
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 508
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 874 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 905
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 930
extern void*Cyc_Absyn_fat_bound_type;
# 934
void*Cyc_Absyn_bounds_one (void);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple11{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*,union Cyc_Relations_RelnOp*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 40 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 221
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 33 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 8 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
extern struct Cyc_AP_T*Cyc_AP_fromint(long);
# 14
extern struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*);
# 16
extern struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*,struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*,struct Cyc_AP_T*);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned val;};struct _union_Node_NParamNumelts{int tag;unsigned val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt-ap.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
# 63
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NReturn (void);struct Cyc_Pratt_Graph;
# 76
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph (void);
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 81
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*,union Cyc_Pratt_Node,union Cyc_Pratt_Node,struct Cyc_AP_T*);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 64 "string.h"
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 35 "relations-ap.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned i){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RConst.tag=1U,_Tmp0.RConst.val=i;_Tmp0;});}
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RVar.tag=2U,_Tmp0.RVar.val=v;_Tmp0;});}
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RNumelts.tag=3U,_Tmp0.RNumelts.val=v;_Tmp0;});}
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RType.tag=4U,({void*_Tmp1=Cyc_Absyn_compress(t);_Tmp0.RType.val=_Tmp1;});_Tmp0;});}
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned i){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RParam.tag=5U,_Tmp0.RParam.val=i;_Tmp0;});}
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned i){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RParamNumelts.tag=6U,_Tmp0.RParamNumelts.val=i;_Tmp0;});}
union Cyc_Relations_RelnOp Cyc_Relations_RReturn (void){return({union Cyc_Relations_RelnOp _Tmp0;_Tmp0.RReturn.tag=7U,_Tmp0.RReturn.val=0U;_Tmp0;});}struct _tuple13{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 43
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f1=r1,_Tmp1.f2=r2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;switch(_Tmp0.f1.RParamNumelts.tag){case 1: if(_Tmp0.f2.RConst.tag==1){_Tmp4=_Tmp0.f1.RConst.val;_Tmp3=_Tmp0.f2.RConst.val;{unsigned c1=_Tmp4;unsigned c2=_Tmp3;
return c1==c2;}}else{goto _LLF;}case 2: if(_Tmp0.f2.RVar.tag==2){_Tmp2=_Tmp0.f1.RVar.val;_Tmp1=_Tmp0.f2.RVar.val;{struct Cyc_Absyn_Vardecl*x1=_Tmp2;struct Cyc_Absyn_Vardecl*x2=_Tmp1;
return x1==x2;}}else{goto _LLF;}case 3: if(_Tmp0.f2.RNumelts.tag==3){_Tmp2=_Tmp0.f1.RNumelts.val;_Tmp1=_Tmp0.f2.RNumelts.val;{struct Cyc_Absyn_Vardecl*x1=_Tmp2;struct Cyc_Absyn_Vardecl*x2=_Tmp1;
return x1==x2;}}else{goto _LLF;}case 4: if(_Tmp0.f2.RType.tag==4){_Tmp2=_Tmp0.f1.RType.val;_Tmp1=_Tmp0.f2.RType.val;{void*t1=_Tmp2;void*t2=_Tmp1;
return Cyc_Unify_unify(t1,t2);}}else{goto _LLF;}case 5: if(_Tmp0.f2.RParam.tag==5){_Tmp4=_Tmp0.f1.RParam.val;_Tmp3=_Tmp0.f2.RParam.val;{unsigned i=_Tmp4;unsigned j=_Tmp3;
return i==j;}}else{goto _LLF;}case 6: if(_Tmp0.f2.RParamNumelts.tag==6){_Tmp4=_Tmp0.f1.RParamNumelts.val;_Tmp3=_Tmp0.f2.RParamNumelts.val;{unsigned i=_Tmp4;unsigned j=_Tmp3;
return i==j;}}else{goto _LLF;}default: if(_Tmp0.f2.RReturn.tag==7)
return 1;else{_LLF:
 return 0;}};}
# 56
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 60
{struct Cyc_List_List*rs=relns;for(0;rs!=0;rs=rs->tl){
struct Cyc_Relations_Reln*r=(struct Cyc_Relations_Reln*)rs->hd;
if((Cyc_Relations_same_relop(r->rop1,rop1)&&(int)r->relation==(int)relation)&&
 Cyc_Relations_same_relop(r->rop2,rop2))return relns;}}
# 65
return({struct Cyc_List_List*_Tmp0=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp1=({struct Cyc_Relations_Reln*_Tmp2=_region_malloc(rgn,sizeof(struct Cyc_Relations_Reln));_Tmp2->rop1=rop1,_Tmp2->relation=relation,_Tmp2->rop2=rop2;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=relns;_Tmp0;});}
# 68
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns){
# 71
union Cyc_Relations_RelnOp rop_old=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop_new=Cyc_Relations_RConst(0U);
if(!Cyc_Relations_exp2relnop(e_old,& rop_old))return relns;
if(!Cyc_Relations_exp2relnop(e_new,& rop_new)){
Cyc_Warn_warn(e_new->loc,({const char*_Tmp0="Could not construct relation";_tag_fat(_Tmp0,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));
return relns;}
# 78
{struct Cyc_List_List*rs=relns;for(0;rs!=0;rs=rs->tl){
struct Cyc_Relations_Reln*r=(struct Cyc_Relations_Reln*)rs->hd;
union Cyc_Relations_RelnOp rop1=r->rop1;
union Cyc_Relations_RelnOp rop2=r->rop2;
int addIt=0;
if(Cyc_Relations_same_relop(rop1,rop_old)){
addIt=1;
rop1=rop_new;}
# 87
if(Cyc_Relations_same_relop(rop2,rop_old)){
addIt=1;
rop2=rop_new;}
# 91
if(addIt)
relns=({struct Cyc_List_List*_Tmp0=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp1=({struct Cyc_Relations_Reln*_Tmp2=_region_malloc(rgn,sizeof(struct Cyc_Relations_Reln));_Tmp2->rop1=rop1,_Tmp2->relation=r->relation,_Tmp2->rop2=rop2;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=relns;_Tmp0;});}}
# 94
return relns;}
# 97
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s==r2s)return 1;
# 101
for(1;r1s!=0;r1s=r1s->tl){
struct Cyc_Relations_Reln*r1=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_Tmp0=r2s;struct Cyc_List_List*r2s=_Tmp0;for(0;r2s!=0;r2s=r2s->tl){
struct Cyc_Relations_Reln*r2=(struct Cyc_Relations_Reln*)r2s->hd;
if(r1==r2 ||(Cyc_Relations_same_relop(r1->rop1,r2->rop1)&&(int)r1->relation==(int)r2->relation)&&
# 108
 Cyc_Relations_same_relop(r1->rop2,r2->rop2)){
found=1;
break;}}}
# 113
if(!found)
return 0;}
# 116
return 1;}
# 119
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s==r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_Tmp0=r1s;struct Cyc_List_List*r1s=_Tmp0;for(0;r1s!=0;r1s=r1s->tl){
struct Cyc_Relations_Reln*r1=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_Tmp1=r2s;struct Cyc_List_List*r2s=_Tmp1;for(0;r2s!=0;r2s=r2s->tl){
struct Cyc_Relations_Reln*r2=(struct Cyc_Relations_Reln*)r2s->hd;
if(r1==r2 ||(Cyc_Relations_same_relop(r1->rop1,r2->rop1)&&(int)r1->relation==(int)r2->relation)&&
# 130
 Cyc_Relations_same_relop(r1->rop2,r2->rop2)){
res=({struct Cyc_List_List*_Tmp2=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp2->hd=r1,_Tmp2->tl=res;_Tmp2;});
found=1;
break;}}}
# 136
if(!found)diff=1;}}
# 138
if(!diff)res=r1s;
return res;}}
# 142
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
void*_Tmp0;switch(r.RReturn.tag){case 2: _Tmp0=r.RVar.val;{struct Cyc_Absyn_Vardecl*v2=_Tmp0;
return v==v2;}case 3: _Tmp0=r.RNumelts.val;{struct Cyc_Absyn_Vardecl*v2=_Tmp0;
return v==v2;}case 4:
 goto _LL8;case 5: _LL8:
 goto _LLA;case 6: _LLA:
 goto _LLC;case 7: _LLC:
 goto _LLE;default: _LLE:
 return 0;};}
# 154
union Cyc_Relations_RelnOp Cyc_Relations_subst_rop_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
void*_Tmp0;switch(r.RReturn.tag){case 2: _Tmp0=r.RVar.val;{struct Cyc_Absyn_Vardecl*v2=_Tmp0;
return v==v2?new_rop: r;}case 3: _Tmp0=r.RNumelts.val;{struct Cyc_Absyn_Vardecl*v2=_Tmp0;
return v==v2?new_rop: r;}case 4:
 goto _LL8;case 5: _LL8:
 goto _LLA;case 6: _LLA:
 goto _LLC;case 7: _LLC:
 goto _LLE;default: _LLE:
 return r;};}
# 166
struct Cyc_Relations_Reln*Cyc_Relations_substitute_rop_var(struct _RegionHandle*rgn,struct Cyc_Relations_Reln*reln,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
# 168
return({struct Cyc_Relations_Reln*_Tmp0=_region_malloc(rgn,sizeof(struct Cyc_Relations_Reln));({union Cyc_Relations_RelnOp _Tmp1=Cyc_Relations_subst_rop_var(reln->rop1,v,new_rop);_Tmp0->rop1=_Tmp1;}),_Tmp0->relation=reln->relation,({
union Cyc_Relations_RelnOp _Tmp1=Cyc_Relations_subst_rop_var(reln->rop2,v,new_rop);_Tmp0->rop2=_Tmp1;});_Tmp0;});}
# 171
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found && p!=0;p=p->tl){
struct Cyc_Relations_Reln*r=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(r->rop1,v)|| Cyc_Relations_rop_contains_var(r->rop2,v)){
found=1;
break;}}
# 181
if(!found)return rs;{
# 183
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
union Cyc_Relations_RelnOp new_rop=Cyc_Relations_RType(Cyc_Absyn_var_type(tv));
struct Cyc_List_List*new_rs=0;
for(p=rs;p!=0;p=p->tl){
struct Cyc_Relations_Reln*r=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(r->rop1,v)|| Cyc_Relations_rop_contains_var(r->rop2,v))
new_rs=({struct Cyc_List_List*_Tmp0=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp1=Cyc_Relations_substitute_rop_var(rgn,r,v,new_rop);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=new_rs;_Tmp0;});else{
# 191
new_rs=({struct Cyc_List_List*_Tmp0=_region_malloc(rgn,sizeof(struct Cyc_List_List));_Tmp0->hd=r,_Tmp0->tl=new_rs;_Tmp0;});}}
# 194
return new_rs;}}
# 200
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 203
RETRY:
 if(e->topt!=0){
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 207
({union Cyc_Relations_RelnOp _Tmp2=Cyc_Relations_RType(t);*p=_Tmp2;});return 1;
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}case 9: _Tmp1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*type_exp=_Tmp1;
# 210
e=type_exp;
goto _LL0;}default: _LL5:
 goto _LL0;}_LL0:;}
# 215
{void*_Tmp0=e->r;int _Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp2;
return Cyc_Relations_exp2relnop(e,p);}case 1: _Tmp2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp2;
# 218
struct Cyc_Absyn_Vardecl*v=Cyc_Tcutil_nonesc_vardecl(b);
if(v!=0){
({union Cyc_Relations_RelnOp _Tmp3=Cyc_Relations_RVar(v);*p=_Tmp3;});
return 1;}
# 223
goto _LL7;}case 0: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.tag==5){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f2;{int i=_Tmp1;
# 225
({union Cyc_Relations_RelnOp _Tmp3=Cyc_Relations_RConst((unsigned)i);*p=_Tmp3;});
return 1;}}else{goto _LL12;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Numelts){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->hd)->r)==1){_Tmp2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd)->r)->f1;{void*b=_Tmp2;
# 228
struct Cyc_Absyn_Vardecl*v=Cyc_Tcutil_nonesc_vardecl(b);
if(v!=0){
({union Cyc_Relations_RelnOp _Tmp3=Cyc_Relations_RNumelts(v);*p=_Tmp3;});
return 1;}
# 233
goto _LL7;}}else{goto _LL12;}}else{goto _LL12;}}else{goto _LL12;}case 39: _Tmp2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp2;
# 235
{void*_Tmp3=Cyc_Absyn_compress(t);void*_Tmp4;if(*((int*)_Tmp3)==9){_Tmp4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Exp*type_exp=_Tmp4;
# 237
e=type_exp;
goto RETRY;}}else{
# 240
({union Cyc_Relations_RelnOp _Tmp5=Cyc_Relations_RType(t);*p=_Tmp5;});
return 1;};}
# 243
goto _LL7;}default: _LL12:
 goto _LL7;}_LL7:;}
# 247
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned t1=_Tmp2;int known=_Tmp1;
if(known){
({union Cyc_Relations_RelnOp _Tmp3=Cyc_Relations_RConst(t1);*p=_Tmp3;});
return 1;}}}
# 254
return 0;}
# 257
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 260
struct Cyc_Absyn_Vardecl*v=Cyc_Tcutil_nonesc_vardecl(b);
if(v!=0)
return Cyc_Relations_reln_kill_var(rgn,r,v);else{
return r;}}}else{
return r;};}
# 268
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
return({struct Cyc_Relations_Reln*_Tmp0=_region_malloc(r2,sizeof(struct Cyc_Relations_Reln));*_Tmp0=*r;_Tmp0;});}
# 272
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;})(r2,Cyc_Relations_copy_reln,r2,relns);}
# 276
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(1;r1!=0 && r2!=0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*x1=(struct Cyc_Relations_Reln*)r1->hd;
struct Cyc_Relations_Reln*x2=(struct Cyc_Relations_Reln*)r2->hd;
if((!Cyc_Relations_same_relop(x1->rop1,x2->rop1)||(int)x1->relation!=(int)x2->relation)|| !
# 282
Cyc_Relations_same_relop(x1->rop2,x2->rop2))return 0;}
# 284
if(r1!=0 || r2!=0)return 0;else{
return 1;}}
# 288
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 292
if(v->escapes)return r;
# 295
r=Cyc_Relations_reln_kill_var(rgn,r,v);
# 300
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==34){if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.fat_result==1){_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;{struct Cyc_Absyn_Exp*e2=_Tmp1;
# 303
malloc_loop: {
void*_Tmp2=e2->r;void*_Tmp3;switch(*((int*)_Tmp2)){case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Exp*e3=_Tmp3;
e2=e3;goto malloc_loop;}case 1: _Tmp3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp2)->f1;{void*b=_Tmp3;
# 307
struct Cyc_Absyn_Vardecl*vd=Cyc_Tcutil_nonesc_vardecl(b);
if(vd!=0)
return({struct _RegionHandle*_Tmp4=rgn;union Cyc_Relations_RelnOp _Tmp5=Cyc_Relations_RVar(vd);union Cyc_Relations_RelnOp _Tmp6=Cyc_Relations_RNumelts(v);Cyc_Relations_add_relation(_Tmp4,_Tmp5,0U,_Tmp6,r);});else{
return r;}}default:
 return r;};}}}else{goto _LL3;}}else{_LL3:
# 314
 goto _LL0;}_LL0:;}
# 317
if(!Cyc_Tcutil_is_integral_type(v->type))return r;{
# 319
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e,& eop))
return({struct _RegionHandle*_Tmp0=rgn;union Cyc_Relations_RelnOp _Tmp1=Cyc_Relations_RVar(v);union Cyc_Relations_RelnOp _Tmp2=eop;Cyc_Relations_add_relation(_Tmp0,_Tmp1,0U,_Tmp2,r);});
# 323
return r;}}
# 326
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_Tmp0=e1->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 329
struct Cyc_Absyn_Vardecl*v=Cyc_Tcutil_nonesc_vardecl(b);
if(v!=0)
return Cyc_Relations_reln_assign_var(rgn,r,v,e2);else{
return r;}}}else{
return r;};}
# 338
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
unsigned _Tmp0;void*_Tmp1;switch(r.RReturn.tag){case 2: _Tmp1=r.RVar.val;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
return Cyc_Pratt_NVar(x);}case 3: _Tmp1=r.RNumelts.val;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
return Cyc_Pratt_NNumelts(x);}case 4: _Tmp1=r.RType.val;{void*x=_Tmp1;
return Cyc_Pratt_NType(x);}case 5: _Tmp0=r.RParam.val;{unsigned i=_Tmp0;
return Cyc_Pratt_NParam(i);}case 6: _Tmp0=r.RParamNumelts.val;{unsigned i=_Tmp0;
return Cyc_Pratt_NParamNumelts(i);}case 7:
 return Cyc_Pratt_NReturn();default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp2="rop2node";_tag_fat(_Tmp2,sizeof(char),9U);}),_tag_fat(0U,sizeof(void*),0));};}
# 350
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
switch((int)r){case Cyc_Relations_Req:
 return({const char*_Tmp0="==";_tag_fat(_Tmp0,sizeof(char),3U);});case Cyc_Relations_Rneq:
 return({const char*_Tmp0="!=";_tag_fat(_Tmp0,sizeof(char),3U);});case Cyc_Relations_Rlt:
 return({const char*_Tmp0="<";_tag_fat(_Tmp0,sizeof(char),2U);});case Cyc_Relations_Rlte:
 goto _LLA;default: _LLA: return({const char*_Tmp0="<=";_tag_fat(_Tmp0,sizeof(char),3U);});};}
# 359
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
void*_Tmp0;unsigned _Tmp1;switch(r.RParamNumelts.tag){case 1: _Tmp1=r.RConst.val;{unsigned c=_Tmp1;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)((int)c);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="%d";_tag_fat(_Tmp4,sizeof(char),3U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}case 2: _Tmp0=r.RVar.val;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_qvar2string(vd->name);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="%s";_tag_fat(_Tmp4,sizeof(char),3U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}case 4: _Tmp0=r.RType.val;{void*t=_Tmp0;
return Cyc_Absynpp_typ2string(t);}case 3: _Tmp0=r.RNumelts.val;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_qvar2string(vd->name);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="numelts(%s)";_tag_fat(_Tmp4,sizeof(char),12U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}case 5: _Tmp1=r.RParam.val;{unsigned i=_Tmp1;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)((int)i);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="param(%d)";_tag_fat(_Tmp4,sizeof(char),10U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}case 6: _Tmp1=r.RParamNumelts.val;{unsigned i=_Tmp1;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)((int)i);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="numelts(param(%d))";_tag_fat(_Tmp4,sizeof(char),19U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}default:
 return Cyc_aprintf(({const char*_Tmp2="return_value";_tag_fat(_Tmp2,sizeof(char),13U);}),_tag_fat(0U,sizeof(void*),0));};}
# 371
struct _fat_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
return({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Relations_rop2string(r->rop1);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Relations_relation2string(r->relation);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=Cyc_Relations_rop2string(r->rop2);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="%s %s %s";_tag_fat(_Tmp4,sizeof(char),9U);}),_tag_fat(_Tmp3,sizeof(void*),3));});}
# 376
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs==0)return({const char*_Tmp0="true";_tag_fat(_Tmp0,sizeof(char),5U);});{
struct Cyc_List_List*ss=0;
for(1;rs!=0;rs=rs->tl){
ss=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=ss;_Tmp0;});
if(rs->tl!=0)ss=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3=" && ";_tag_fat(_Tmp3,sizeof(char),5U);});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=ss;_Tmp0;});}
# 383
return Cyc_strconcat_l(ss);}}
# 386
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(1;r!=0;r=r->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(f,({const char*_Tmp2="%s";_tag_fat(_Tmp2,sizeof(char),3U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
if(r->tl!=0)Cyc_fprintf(f,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}}
# 393
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{void*_Tmp0;switch(rop.RNumelts.tag){case 4: _Tmp0=rop.RType.val;{void*t=_Tmp0;
# 396
{void*_Tmp1=Cyc_Absyn_compress(t);void*_Tmp2;if(*((int*)_Tmp1)==9){_Tmp2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp1)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 398
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(e,& rop2))return rop2;
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 403
goto _LL0;}case 3: _Tmp0=rop.RNumelts.val;{struct Cyc_Absyn_Vardecl*v=_Tmp0;
# 405
{void*_Tmp1=Cyc_Absyn_compress(v->type);void*_Tmp2;if(*((int*)_Tmp1)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp1)->f1.ptr_atts.bounds;{void*b=_Tmp2;
# 409
if(({void*_Tmp3=b;_Tmp3==Cyc_Absyn_bounds_one();}))goto _LLC;{
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt!=0){
struct Cyc_Absyn_Exp*e=eopt;
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(e,& rop2))return rop2;}
# 416
goto _LLC;}}}else{
goto _LLC;}_LLC:;}
# 419
goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 422
return rop;}
# 427
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(1;(unsigned)G && rlns!=0;rlns=rlns->tl){
struct Cyc_Relations_Reln*reln=(struct Cyc_Relations_Reln*)rlns->hd;
({union Cyc_Relations_RelnOp _Tmp0=Cyc_Relations_reduce_relnop(reln->rop1);reln->rop1=_Tmp0;});
({union Cyc_Relations_RelnOp _Tmp0=Cyc_Relations_reduce_relnop(reln->rop2);reln->rop2=_Tmp0;});{
# 437
union Cyc_Relations_RelnOp _Tmp0;union Cyc_Relations_RelnOp _Tmp1;unsigned _Tmp2;enum Cyc_Relations_Relation _Tmp3;unsigned _Tmp4;if(((struct Cyc_Relations_Reln*)reln)->rop1.RConst.tag==1){if(((struct Cyc_Relations_Reln*)reln)->rop2.RConst.tag==1){_Tmp4=reln->rop1.RConst.val;_Tmp3=reln->relation;_Tmp2=reln->rop2.RConst.val;{unsigned i=_Tmp4;enum Cyc_Relations_Relation reln=_Tmp3;unsigned j=_Tmp2;
# 439
switch((int)reln){case Cyc_Relations_Req:
 if(i!=j)return 0;goto _LLF;case Cyc_Relations_Rneq:
 if(i==j)return 0;goto _LLF;case Cyc_Relations_Rlt:
 if(i >= j)return 0;goto _LLF;case Cyc_Relations_Rlte:
 goto _LL19;default: _LL19: if(i > j)return 0;goto _LLF;}_LLF:;
# 445
goto _LL0;}}else{if(((struct Cyc_Relations_Reln*)reln)->relation==Cyc_Relations_Rneq){_Tmp4=reln->rop1.RConst.val;_Tmp1=reln->rop2;_LL6: {unsigned i=_Tmp4;union Cyc_Relations_RelnOp rop=_Tmp1;
# 452
union Cyc_Pratt_Node n=Cyc_Relations_rop2node(rop);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_Tmp5=G2;union Cyc_Pratt_Node _Tmp6=n;union Cyc_Pratt_Node _Tmp7=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,({struct Cyc_AP_T*_Tmp8=Cyc_AP_fromint((long)i);Cyc_AP_sub(_Tmp8,Cyc_AP_one);}));});
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _Tmp7=n;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_neg(({struct Cyc_AP_T*_Tmp8=Cyc_AP_fromint((long)i);Cyc_AP_add(_Tmp8,Cyc_AP_one);})));});
# 462
if(G!=0 && G2!=0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G==0)
G=G2;}
goto _LL0;}}else{_Tmp4=reln->rop1.RConst.val;_Tmp3=reln->relation;_Tmp1=reln->rop2;{unsigned i=_Tmp4;enum Cyc_Relations_Relation reln=_Tmp3;union Cyc_Relations_RelnOp rop=_Tmp1;
# 496
union Cyc_Pratt_Node n=Cyc_Relations_rop2node(rop);
if((int)reln==3)i=i + 1U;
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _Tmp7=n;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_neg(Cyc_AP_fromint((long)i)));});
# 501
if((unsigned)G &&(int)reln==0)
# 506
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=n;union Cyc_Pratt_Node _Tmp7=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_fromint((long)i));});
# 513
goto _LL0;}}}}else{if(((struct Cyc_Relations_Reln*)reln)->relation==Cyc_Relations_Rneq){if(((struct Cyc_Relations_Reln*)reln)->rop2.RConst.tag==1){_Tmp1=reln->rop1;_Tmp4=reln->rop2.RConst.val;{union Cyc_Relations_RelnOp rop=_Tmp1;unsigned i=_Tmp4;
# 450
_Tmp4=i;_Tmp1=rop;goto _LL6;}}else{_Tmp1=reln->rop1;_Tmp0=reln->rop2;{union Cyc_Relations_RelnOp rop1=_Tmp1;union Cyc_Relations_RelnOp rop2=_Tmp0;
# 469
union Cyc_Pratt_Node n1=Cyc_Relations_rop2node(rop1);
union Cyc_Pratt_Node n2=Cyc_Relations_rop2node(rop2);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_Tmp5=G2;union Cyc_Pratt_Node _Tmp6=n1;union Cyc_Pratt_Node _Tmp7=n2;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_neg(Cyc_AP_one));});
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=n2;union Cyc_Pratt_Node _Tmp7=n1;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_neg(Cyc_AP_one));});
if(G!=0 && G2!=0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G==0)
G=G2;}
goto _LL0;}}}else{if(((struct Cyc_Relations_Reln*)reln)->rop2.RConst.tag==1){_Tmp1=reln->rop1;_Tmp3=reln->relation;_Tmp4=reln->rop2.RConst.val;{union Cyc_Relations_RelnOp rop=_Tmp1;enum Cyc_Relations_Relation reln=_Tmp3;unsigned i=_Tmp4;
# 483
union Cyc_Pratt_Node n=Cyc_Relations_rop2node(rop);
if((int)reln==3)i=i - 1U;
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=n;union Cyc_Pratt_Node _Tmp7=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_fromint((long)i));});
# 487
if((unsigned)G &&(int)reln==0)
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _Tmp7=n;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_neg(Cyc_AP_fromint((long)i)));});
goto _LL0;}}else{_Tmp1=reln->rop1;_Tmp3=reln->relation;_Tmp0=reln->rop2;{union Cyc_Relations_RelnOp rop1=_Tmp1;enum Cyc_Relations_Relation reln=_Tmp3;union Cyc_Relations_RelnOp rop2=_Tmp0;
# 516
union Cyc_Pratt_Node n1=Cyc_Relations_rop2node(rop1);
union Cyc_Pratt_Node n2=Cyc_Relations_rop2node(rop2);
int i=(int)reln==3?-1: 0;
# 521
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=n1;union Cyc_Pratt_Node _Tmp7=n2;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_fromint(i));});
# 523
if((unsigned)G &&(int)reln==0)
G=({struct Cyc_Pratt_Graph*_Tmp5=G;union Cyc_Pratt_Node _Tmp6=n2;union Cyc_Pratt_Node _Tmp7=n1;Cyc_Pratt_add_edge(_Tmp5,_Tmp6,_Tmp7,Cyc_AP_fromint(i));});
goto _LL0;}}}}_LL0:;}}
# 528
if(G!=0)return 1;else{return 0;}}
# 536
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 542
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 548
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(1;r2!=0;r2=r2->tl){
struct Cyc_Relations_Reln*r=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*relns;relns=_cycalloc(sizeof(struct Cyc_List_List)),relns->hd=r,relns->tl=r1;
if(Cyc_Relations_consistent_relations(relns))return 0;}
# 554
return 1;}
# 560
struct _tuple11 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 562
switch((int)p){case Cyc_Absyn_Eq:
 return({struct _tuple11 _Tmp0;_Tmp0.f1=e1,_Tmp0.f2=0U,_Tmp0.f3=e2;_Tmp0;});case Cyc_Absyn_Neq:
 return({struct _tuple11 _Tmp0;_Tmp0.f1=e1,_Tmp0.f2=1U,_Tmp0.f3=e2;_Tmp0;});case Cyc_Absyn_Lt:
 return({struct _tuple11 _Tmp0;_Tmp0.f1=e1,_Tmp0.f2=3U,_Tmp0.f3=e2;_Tmp0;});case Cyc_Absyn_Lte:
 return({struct _tuple11 _Tmp0;_Tmp0.f1=e1,_Tmp0.f2=2U,_Tmp0.f3=e2;_Tmp0;});case Cyc_Absyn_Gt:
 return({struct _tuple11 _Tmp0;_Tmp0.f1=e2,_Tmp0.f2=3U,_Tmp0.f3=e1;_Tmp0;});case Cyc_Absyn_Gte:
 return({struct _tuple11 _Tmp0;_Tmp0.f1=e2,_Tmp0.f2=2U,_Tmp0.f3=e1;_Tmp0;});default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp0="primop2relation";_tag_fat(_Tmp0,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));};}
# 574
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
switch((int)r){case Cyc_Relations_Req:
 return 1U;case Cyc_Relations_Rneq:
 return 0U;case Cyc_Relations_Rlt:
 return 2U;case Cyc_Relations_Rlte:
 goto _LLA;default: _LLA: return 3U;};}
# 583
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
return({struct Cyc_Relations_Reln*_Tmp0=_region_malloc(r,sizeof(struct Cyc_Relations_Reln));_Tmp0->rop1=rln->rop2,({enum Cyc_Relations_Relation _Tmp1=Cyc_Relations_flip_relation(rln->relation);_Tmp0->relation=_Tmp1;}),_Tmp0->rop2=rln->rop1;_Tmp0;});}
# 594
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 596
{void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 14: _Tmp3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;
# 599
{void*_Tmp4=Cyc_Absyn_compress(t);if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f2==Cyc_Absyn_Int_sz)
return Cyc_Relations_exp2relns(r,e);else{goto _LLE;}}else{goto _LLE;}}else{_LLE:
 goto _LLB;}_LLB:;}
# 603
goto _LL0;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
return({struct _RegionHandle*_Tmp4=r;struct Cyc_List_List*_Tmp5=Cyc_Relations_exp2relns(r,e1);Cyc_List_rappend(_Tmp4,_Tmp5,Cyc_Relations_exp2relns(r,e2));});}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->hd)->r)==3){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->hd)->r)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->hd)->r)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->hd)->r)->f2)->tl)->tl==0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl==0){_Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd)->r)->f1;_Tmp3=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd)->r)->f2->hd;_Tmp2=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd)->r)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 606
switch((int)p){case Cyc_Absyn_Eq:
 goto _LL14;case Cyc_Absyn_Neq: _LL14: goto _LL16;case Cyc_Absyn_Lt: _LL16: goto _LL18;case Cyc_Absyn_Lte: _LL18: goto _LL1A;case Cyc_Absyn_Gt: _LL1A: goto _LL1C;case Cyc_Absyn_Gte: _LL1C: {
struct _tuple11 _Tmp4=Cyc_Relations_primop2relation(e1,p,e2);void*_Tmp5;enum Cyc_Relations_Relation _Tmp6;void*_Tmp7;_Tmp7=_Tmp4.f1;_Tmp6=_Tmp4.f2;_Tmp5=_Tmp4.f3;{struct Cyc_Absyn_Exp*e1=_Tmp7;enum Cyc_Relations_Relation rln=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& rop1)&& Cyc_Relations_exp2relnop(e2,& rop2))
return({struct Cyc_List_List*_Tmp8=_region_malloc(r,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp9=({struct Cyc_Relations_Reln*_TmpA=_region_malloc(r,sizeof(struct Cyc_Relations_Reln));_TmpA->rop1=rop2,({enum Cyc_Relations_Relation _TmpB=Cyc_Relations_flip_relation(rln);_TmpA->relation=_TmpB;}),_TmpA->rop2=rop1;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_Tmp8;});
goto _LL10;}}default:
 goto _LL10;}_LL10:;
# 616
goto _LL0;}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0)goto _LL7;else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0){_LL7: _Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp2=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 618
switch((int)p){case Cyc_Absyn_Eq:
 goto _LL26;case Cyc_Absyn_Neq: _LL26: goto _LL28;case Cyc_Absyn_Lt: _LL28: goto _LL2A;case Cyc_Absyn_Lte: _LL2A: goto _LL2C;case Cyc_Absyn_Gt: _LL2C: goto _LL2E;case Cyc_Absyn_Gte: _LL2E: {
struct _tuple11 _Tmp4=Cyc_Relations_primop2relation(e1,p,e2);void*_Tmp5;enum Cyc_Relations_Relation _Tmp6;void*_Tmp7;_Tmp7=_Tmp4.f1;_Tmp6=_Tmp4.f2;_Tmp5=_Tmp4.f3;{struct Cyc_Absyn_Exp*e1=_Tmp7;enum Cyc_Relations_Relation rln=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e1,& rop1)&& Cyc_Relations_exp2relnop(e2,& rop2))
return({struct Cyc_List_List*_Tmp8=_region_malloc(r,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp9=({struct Cyc_Relations_Reln*_TmpA=_region_malloc(r,sizeof(struct Cyc_Relations_Reln));_TmpA->rop1=rop1,_TmpA->relation=rln,_TmpA->rop2=rop2;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_Tmp8;});
goto _LL22;}}default:
 goto _LL22;}_LL22:;
# 628
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}}default: _LL9:
 goto _LL0;}_LL0:;}
# 632
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned i=_Tmp2;int known=_Tmp1;
if(known){
if((int)i)return 0;else{
# 637
return({struct Cyc_List_List*_Tmp3=_region_malloc(r,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp4=({struct Cyc_Relations_Reln*_Tmp5=_region_malloc(r,sizeof(struct Cyc_Relations_Reln));({union Cyc_Relations_RelnOp _Tmp6=Cyc_Relations_RConst(0U);_Tmp5->rop1=_Tmp6;}),_Tmp5->relation=3U,({union Cyc_Relations_RelnOp _Tmp6=Cyc_Relations_RConst(0U);_Tmp5->rop2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});}}
# 639
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=Cyc_Absynpp_exp2string(e);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(e->loc,({const char*_Tmp5="unable to convert `%s' to static relation";_tag_fat(_Tmp5,sizeof(char),42U);}),_tag_fat(_Tmp4,sizeof(void*),1));});}}
# 643
return({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));({struct Cyc_Relations_Reln*_Tmp1=({struct Cyc_Relations_Reln*_Tmp2=_region_malloc(r,sizeof(struct Cyc_Relations_Reln));({union Cyc_Relations_RelnOp _Tmp3=Cyc_Relations_RConst(0U);_Tmp2->rop1=_Tmp3;}),_Tmp2->relation=3U,({union Cyc_Relations_RelnOp _Tmp3=Cyc_Relations_RConst(0U);_Tmp2->rop2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});}
