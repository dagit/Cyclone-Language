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
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;
# 175
void Cyc_Core_ufree(void*ptr);
# 188
struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 255 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 328
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 67
struct _fat_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 693 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 869
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 876
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 906
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 908
extern void*Cyc_Absyn_sint_type;
# 913
extern void*Cyc_Absyn_heap_rgn_type;
# 917
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;
# 936
void*Cyc_Absyn_exn_type();
# 944
extern void*Cyc_Absyn_fat_bound_type;
# 948
void*Cyc_Absyn_bounds_one();
# 958
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 962
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 964
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 966
void*Cyc_Absyn_strct(struct _fat_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 972
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 977
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1050
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1063
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1067
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1072
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1076
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1081
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1119
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1122
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1140
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1142
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1144
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1153
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1161
extern int Cyc_Absyn_no_regions;
# 38 "warn.h"
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 46
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 68
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
void*Cyc_Warn_vimpos2(struct _fat_ptr);
void*Cyc_Warn_impos2(struct _fat_ptr);
# 72
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 31 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 44
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 49
int Cyc_Tcutil_is_fat_ptr(void*);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 54
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 64
void*Cyc_Tcutil_pointer_elt_type(void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 77
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 79
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 97
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 146
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 183
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 222
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 229
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 276
void*Cyc_Tcutil_snd_tqt(struct _tuple13*);
# 294
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 34 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 36
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 41
struct _tuple1*Cyc_Toc_temp_var();
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_init();extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 55 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 70 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp784;_tmp784.tag=0U,_tmp784.f1=str;_tmp784;});void*_tmp0[1U];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1U));});}
# 73
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 75
({struct _fat_ptr _tmp7FD=(struct _fat_ptr)({struct _fat_ptr _tmp7FC=({const char*_tmp2="Toc: ";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_strconcat(_tmp7FC,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp7FD,ap);});}
# 77
static void*Cyc_Toc_toc_impos2(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=0U,({struct _fat_ptr _tmp7FE=({const char*_tmp5="Toc: ";_tag_fat(_tmp5,sizeof(char),6U);});_tmp6->f1=_tmp7FE;});_tmp6;});
struct _fat_ptr args=({unsigned _tmp4=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned _tmp785=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;unsigned i;for(i=0;i < _tmp785;++ i){i == (unsigned)0?_tmp3[i]=toc:(_tmp3[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - (unsigned)1))));}}0;});_tag_fat(_tmp3,sizeof(void*),_tmp4);});
Cyc_Warn_vimpos2(args);}
# 83
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 86
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 96
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 98
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 124
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 131
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmpE=_region_malloc(d,sizeof(*_tmpE));
({struct Cyc_List_List**_tmp80A=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpE->tuple_types=_tmp80A;}),({
struct Cyc_List_List**_tmp809=({struct Cyc_List_List**_tmp8=_region_malloc(d,sizeof(*_tmp8));*_tmp8=0;_tmp8;});_tmpE->anon_aggr_types=_tmp809;}),({
struct Cyc_Dict_Dict*_tmp808=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));({struct Cyc_Dict_Dict _tmp807=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp9=_tmp807;});_tmp9;});_tmpE->aggrs_so_far=_tmp808;}),({
struct Cyc_List_List**_tmp806=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmpE->abs_struct_types=_tmp806;}),({
struct Cyc_Set_Set**_tmp805=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Set_Set*_tmp804=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp804;});_tmpB;});_tmpE->datatypes_so_far=_tmp805;}),({
struct Cyc_Dict_Dict*_tmp803=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp802=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp802;});_tmpC;});_tmpE->xdatatypes_so_far=_tmp803;}),({
struct Cyc_Dict_Dict*_tmp801=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Dict_Dict _tmp800=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmpD=_tmp800;});_tmpD;});_tmpE->qvar_tags=_tmp801;}),({
struct Cyc_Xarray_Xarray*_tmp7FF=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpE->temp_labels=_tmp7FF;});_tmpE;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 150
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 156
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 159
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
struct Cyc_Toc_TocStateWrap _tmp11=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp11;struct Cyc_Toc_TocStateWrap _tmp12=_stmttmp0;struct Cyc_Toc_TocState*_tmp14;struct Cyc_Core_DynamicRegion*_tmp13;_LL1: _tmp13=_tmp12.dyn;_tmp14=_tmp12.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp13;struct Cyc_Toc_TocState*s=_tmp14;
struct _fat_ptr _tmp15=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp15;
({struct Cyc_Toc_TocStateWrap _tmp80B=({struct Cyc_Toc_TocStateWrap _tmp786;_tmp786.dyn=dyn,_tmp786.state=s;_tmp786;});*ts=_tmp80B;});
({struct Cyc_Toc_TocStateWrap*_tmp16=ts;struct Cyc_Toc_TocStateWrap*_tmp17=Cyc_Toc_toc_state;ts=_tmp17;Cyc_Toc_toc_state=_tmp16;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp19=({struct _tuple16 _tmp787;_tmp787.f1=s,_tmp787.f2=arg;_tmp787;});struct _tuple16 env=_tmp19;
res=f(h,& env);}
# 167
;}
# 169
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 173
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 176
struct _tuple18 _tmp1A=*env;struct _tuple18 _stmttmp1=_tmp1A;struct _tuple18 _tmp1B=_stmttmp1;void*(*_tmp1E)(struct _tuple1*);struct _tuple1*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1C=_tmp1B.f1;_tmp1D=(_tmp1B.f2)->f1;_tmp1E=(_tmp1B.f2)->f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp1C;struct _tuple1*q=_tmp1D;void*(*type_maker)(struct _tuple1*)=_tmp1E;
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 181
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp788;_tmp788.f1=q,_tmp788.f2=type_maker;_tmp788;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1F[5U]="curr";
# 194 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1F,_tmp1F,_tmp1F + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp20[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp20,_tmp20,_tmp20 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp21[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp21,_tmp21,_tmp21 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp22[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp22,_tmp22,_tmp22 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp23[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp23,_tmp23,_tmp23 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp24[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp24,_tmp24,_tmp24 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp25[7U]="_throw";
# 213 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_tmp25,_tmp25,_tmp25 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp26[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_tmp26,_tmp26,_tmp26 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp27[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_tmp27,_tmp27,_tmp27 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp28[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_tmp28,_tmp28,_tmp28 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp29[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_tmp29,_tmp29,_tmp29 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp2A[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2B[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_tmp2B,_tmp2B,_tmp2B + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2C[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2C,_tmp2C,_tmp2C + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2D[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2D,_tmp2D,_tmp2D + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp2E[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_tmp2E,_tmp2E,_tmp2E + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0U,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1U,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _tmp2F[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_tmp2F,_tmp2F,_tmp2F + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0U,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1U,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _tmp30[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_tmp30,_tmp30,_tmp30 + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0U,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1U,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _tmp31[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_tmp31,_tmp31,_tmp31 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0U,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1U,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _tmp32[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0U,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1U,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _tmp33[19U]="_get_zero_arr_size";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_str={_tmp33,_tmp33,_tmp33 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp34[24U]="_get_zero_arr_size_char";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp34,_tmp34,_tmp34 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp35[25U]="_get_zero_arr_size_short";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp35,_tmp35,_tmp35 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp36[23U]="_get_zero_arr_size_int";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp36,_tmp36,_tmp36 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp37[25U]="_get_zero_arr_size_float";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp37,_tmp37,_tmp37 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp38[26U]="_get_zero_arr_size_double";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp38,_tmp38,_tmp38 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp39[30U]="_get_zero_arr_size_longdouble";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp39,_tmp39,_tmp39 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp3A[28U]="_get_zero_arr_size_voidstar";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp3A,_tmp3A,_tmp3A + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp3B[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_tmp3B,_tmp3B,_tmp3B + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0U,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1U,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _tmp3C[15U]="_zero_arr_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_str={_tmp3C,_tmp3C,_tmp3C + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp3D[20U]="_zero_arr_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp3D,_tmp3D,_tmp3D + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp3E[21U]="_zero_arr_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp3E,_tmp3E,_tmp3E + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp3F[19U]="_zero_arr_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp3F,_tmp3F,_tmp3F + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp40[21U]="_zero_arr_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp40,_tmp40,_tmp40 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp41[22U]="_zero_arr_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp41,_tmp41,_tmp41 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp42[26U]="_zero_arr_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp42,_tmp42,_tmp42 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp43[24U]="_zero_arr_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp43,_tmp43,_tmp43 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp44[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_tmp44,_tmp44,_tmp44 + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0U,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _tmp45[23U]="_zero_arr_inplace_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp45,_tmp45,_tmp45 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp46[28U]="_zero_arr_inplace_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp46,_tmp46,_tmp46 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp47[29U]="_zero_arr_inplace_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp47,_tmp47,_tmp47 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp48[27U]="_zero_arr_inplace_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp48,_tmp48,_tmp48 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp49[29U]="_zero_arr_inplace_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp49,_tmp49,_tmp49 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp4A[30U]="_zero_arr_inplace_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp4A,_tmp4A,_tmp4A + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp4B[34U]="_zero_arr_inplace_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp4B,_tmp4B,_tmp4B + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp4C[32U]="_zero_arr_inplace_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp4C,_tmp4C,_tmp4C + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp4D[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_tmp4D,_tmp4D,_tmp4D + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _tmp4E[28U]="_zero_arr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp4E,_tmp4E,_tmp4E + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp4F[33U]="_zero_arr_inplace_plus_post_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp4F,_tmp4F,_tmp4F + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp50[34U]="_zero_arr_inplace_plus_post_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp50,_tmp50,_tmp50 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp51[32U]="_zero_arr_inplace_plus_post_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp51,_tmp51,_tmp51 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp52[34U]="_zero_arr_inplace_plus_post_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp52,_tmp52,_tmp52 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp53[35U]="_zero_arr_inplace_plus_post_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp53,_tmp53,_tmp53 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp54[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp54,_tmp54,_tmp54 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp55[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp55,_tmp55,_tmp55 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp56[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_tmp56,_tmp56,_tmp56 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp57[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_tmp57,_tmp57,_tmp57 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp58[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_tmp58,_tmp58,_tmp58 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp59[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp59,_tmp59,_tmp59 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp5A[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_tmp5A,_tmp5A,_tmp5A + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp5B[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_tmp5B,_tmp5B,_tmp5B + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp5C[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_tmp5C,_tmp5C,_tmp5C + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp5D[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_tmp5D,_tmp5D,_tmp5D + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp5E[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_tmp5E,_tmp5E,_tmp5E + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp5F[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_tmp5F,_tmp5F,_tmp5F + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp60[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_tmp60,_tmp60,_tmp60 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp61[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_tmp61,_tmp61,_tmp61 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0U,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _tmp62[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_tmp62,_tmp62,_tmp62 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp63[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_tmp63,_tmp63,_tmp63 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp64[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_tmp64,_tmp64,_tmp64 + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 281
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression(){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 284
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp6F=_cycalloc(sizeof(*_tmp6F));((_tmp6F->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp810=({struct _fat_ptr*_tmp6A[1U];({struct _fat_ptr*_tmp80F=({struct _fat_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _fat_ptr _tmp80E=({const char*_tmp6B="Core";_tag_fat(_tmp6B,sizeof(char),5U);});*_tmp6C=_tmp80E;});_tmp6C;});_tmp6A[0]=_tmp80F;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6A,sizeof(struct _fat_ptr*),1U));});((_tmp6F->f1).Abs_n).val=_tmp810;}),({struct _fat_ptr*_tmp80D=({struct _fat_ptr*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct _fat_ptr _tmp80C=({const char*_tmp6D="get_exn_thrown";_tag_fat(_tmp6D,sizeof(char),15U);});*_tmp6E=_tmp80C;});_tmp6E;});_tmp6F->f2=_tmp80D;});_tmp6F;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=0U,_tmp69->f1=qv;_tmp69;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->topt=0,({void*_tmp811=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=1U,_tmp67->f1=bnd;_tmp67;});_tmp68->r=_tmp811;}),_tmp68->loc=0U,_tmp68->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp68;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=10U,_tmp66->f1=fnname,_tmp66->f2=0,_tmp66->f3=0,_tmp66->f4=0;_tmp66;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->topt=0,_tmp65->r=fncall_re,_tmp65->loc=0U,_tmp65->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp65;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 294
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 296
void*Cyc_Toc_void_star_type(){
static void*t=0;
if(t == 0)
t=({void*_tmp814=Cyc_Absyn_void_type;void*_tmp813=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp812=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_star_type(_tmp814,_tmp813,_tmp812,Cyc_Absyn_false_type);});
return(void*)_check_null(t);}
# 302
static void*Cyc_Toc_fat_ptr_type(){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__fat_ptr_pr,0),0);
return(void*)_check_null(t);}
# 308
static void*Cyc_Toc_rgn_type(){
static void*r=0;
if(r == 0)
r=({void*_tmp815=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp815,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 315
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 319
static struct _tuple20*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp72=_cycalloc(sizeof(*_tmp72));({struct Cyc_List_List*_tmp817=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));({void*_tmp816=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,_tmp70->f1=name;_tmp70;});_tmp71->hd=_tmp816;}),_tmp71->tl=0;_tmp71;});_tmp72->f1=_tmp817;}),_tmp72->f2=e;_tmp72;});}
# 323
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp818=f;Cyc_Absyn_fncall_exp(_tmp818,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
# 326
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp73=e->r;void*_stmttmp2=_tmp73;void*_tmp74=_stmttmp2;struct Cyc_Absyn_Exp*_tmp75;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp75=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp75;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 334
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=14U,_tmp76->f1=t,_tmp76->f2=e,_tmp76->f3=0,_tmp76->f4=Cyc_Absyn_No_coercion;_tmp76;});}
# 337
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=20U,_tmp77->f1=e;_tmp77;});}
# 340
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=23U,_tmp78->f1=e1,_tmp78->f2=e2;_tmp78;});}
# 343
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=37U,_tmp79->f1=s;_tmp79;});}
# 346
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=17U,_tmp7A->f1=t;_tmp7A;});}
# 349
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=10U,_tmp7B->f1=e,({struct Cyc_List_List*_tmp819=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp7B->f2=_tmp819;}),_tmp7B->f3=0,_tmp7B->f4=1;_tmp7B;});}
# 352
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=2U,_tmp7C->f1=s1,_tmp7C->f2=s2;_tmp7C;});}
# 355
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=6U,_tmp7D->f1=e1,_tmp7D->f2=e2,_tmp7D->f3=e3;_tmp7D;});}
# 358
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=21U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 361
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 364
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 366
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 369
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 373
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp81=e->r;void*_stmttmp3=_tmp81;void*_tmp82=_stmttmp3;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp83;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 401 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 407
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp84=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp84;void*_tmp85=_stmttmp4;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1: _LL2:
 return fS->fchar;case Cyc_Absyn_Short_sz: _LL3: _LL4:
 return fS->fshort;case Cyc_Absyn_Int_sz: _LL5: _LL6:
 return fS->fint;default: goto _LLF;}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL7: _LL8:
 return fS->ffloat;case 1U: _LL9: _LLA:
 return fS->fdouble;default: _LLB: _LLC:
 return fS->flongdouble;}default: goto _LLF;}case 3U: _LLD: _LLE:
 return fS->fvoidstar;default: _LLF: _LL10:
# 417
({struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp78B;_tmp78B.tag=0U,({struct _fat_ptr _tmp81A=({const char*_tmp8B="expression type ";_tag_fat(_tmp8B,sizeof(char),17U);});_tmp78B.f1=_tmp81A;});_tmp78B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp88=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp78A;_tmp78A.tag=2U,_tmp78A.f1=(void*)t;_tmp78A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp789;_tmp789.tag=0U,({struct _fat_ptr _tmp81B=({const char*_tmp8A=" (not int, float, double, or pointer)";_tag_fat(_tmp8A,sizeof(char),38U);});_tmp789.f1=_tmp81B;});_tmp789;});void*_tmp86[3U];_tmp86[0]=& _tmp87,_tmp86[1]=& _tmp88,_tmp86[2]=& _tmp89;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp86,sizeof(void*),3U));});}_LL0:;}
# 420
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp8C;void*_tmp8D=_stmttmp5;void*_tmp8E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 3U){_LL1: _tmp8E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_type;_LL2: {void*et=_tmp8E;
return Cyc_Toc_getFunctionType(fS,et);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp78C;_tmp78C.tag=0U,({struct _fat_ptr _tmp81C=({const char*_tmp91="impossible type (not pointer)";_tag_fat(_tmp91,sizeof(char),30U);});_tmp78C.f1=_tmp81C;});_tmp78C;});void*_tmp8F[1U];_tmp8F[0]=& _tmp90;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp8F,sizeof(void*),1U));});}_LL0:;}
# 430
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_stmttmp6=_tmp92;void*_tmp93=_stmttmp6;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Exp*_tmp99;long long _tmp9A;int _tmp9B;short _tmp9C;struct _fat_ptr _tmp9D;char _tmp9E;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp9E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2: {char c=_tmp9E;
return(int)c == (int)'\000';}case 3U: _LL3: _tmp9D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp9D;
# 434
unsigned long _tmp9F=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp9F;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 444
return 0;}}case 4U: _LL5: _tmp9C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6: {short i=_tmp9C;
return(int)i == 0;}case 5U: _LL7: _tmp9B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8: {int i=_tmp9B;
return i == 0;}case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA: {long long i=_tmp9A;
return i == (long long)0;}case 1U: _LLD: _LLE:
# 449
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 448
 goto _LLE;case 14U: _LLF: _tmp99=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp99;
# 450
return Cyc_Toc_is_zero(e1);}case 24U: _LL11: _tmp98=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12: {struct Cyc_List_List*es=_tmp98;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,es);}case 26U: _LL13: _tmp97=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14: {struct Cyc_List_List*dles=_tmp97;
_tmp96=dles;goto _LL16;}case 29U: _LL15: _tmp96=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16: {struct Cyc_List_List*dles=_tmp96;
_tmp95=dles;goto _LL18;}case 25U: _LL17: _tmp95=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18: {struct Cyc_List_List*dles=_tmp95;
_tmp94=dles;goto _LL1A;}case 36U: _LL19: _tmp94=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A: {struct Cyc_List_List*dles=_tmp94;
# 456
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)dles->hd)).f2))return 0;}
return 1;}default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 464
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;struct _fat_ptr*_tmpA2;union Cyc_Absyn_Nmspace _tmpA1;_LL1: _tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmpA1;struct _fat_ptr*v=_tmpA2;
union Cyc_Absyn_Nmspace _tmpA3=ns;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA6;switch((_tmpA3.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpA6=0;goto _LL7;case 1U: _LL6: _tmpA6=(_tmpA3.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmpA6;
_tmpA5=vs;goto _LL9;}case 2U: _LL8: _tmpA5=(_tmpA3.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmpA5;
_tmpA4=vs;goto _LLB;}default: _LLA: _tmpA4=(_tmpA3.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmpA4;
# 473
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp78E;_tmp78E.tag=0U,_tmp78E.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp78E;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,_tmp78D.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp78D;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({struct _fat_ptr _tmp81D=({const char*_tmpA8="%s_%s_struct";_tag_fat(_tmpA8,sizeof(char),13U);});Cyc_aprintf(_tmp81D,_tag_fat(_tmpA7,sizeof(void*),2U));});});{
struct _RegionHandle _tmpAB=_new_region("r");struct _RegionHandle*r=& _tmpAB;_push_region(r);
{struct _fat_ptr _tmpB2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,_tmp791.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp791;});struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,({struct _fat_ptr _tmp820=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp81F=r;struct Cyc_List_List*_tmp81E=vs;Cyc_rstr_sepstr(_tmp81F,_tmp81E,({const char*_tmpB1="_";_tag_fat(_tmpB1,sizeof(char),2U);}));}));_tmp790.f1=_tmp820;});_tmp790;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp78F;_tmp78F.tag=0U,_tmp78F.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp78F;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAE,_tmpAC[1]=& _tmpAF,_tmpAC[2]=& _tmpB0;({struct _fat_ptr _tmp821=({const char*_tmpAD="%s_%s_%s_struct";_tag_fat(_tmpAD,sizeof(char),16U);});Cyc_aprintf(_tmp821,_tag_fat(_tmpAC,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB2;};_pop_region(r);}}}_LL3:;}}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 486
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 489
struct _tuple21 _tmpB3=*env;struct _tuple21 _stmttmp7=_tmpB3;struct _tuple21 _tmpB4=_stmttmp7;struct _tuple15*_tmpB6;struct Cyc_Dict_Dict*_tmpB5;_LL1: _tmpB5=(_tmpB4.f1)->qvar_tags;_tmpB6=_tmpB4.f2;_LL2: {struct Cyc_Dict_Dict*qvs=_tmpB5;struct _tuple15*pair=_tmpB6;
struct _tuple15 _tmpB7=*pair;struct _tuple15 _stmttmp8=_tmpB7;struct _tuple15 _tmpB8=_stmttmp8;struct _tuple1*_tmpBA;struct _tuple1*_tmpB9;_LL4: _tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;_LL5: {struct _tuple1*fieldname=_tmpB9;struct _tuple1*dtname=_tmpBA;
struct _handler_cons _tmpBB;_push_handler(& _tmpBB);{int _tmpBD=0;if(setjmp(_tmpBB.handler))_tmpBD=1;if(!_tmpBD){{struct _tuple1*_tmpBE=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0U);return _tmpBE;};_pop_handler();}else{void*_tmpBC=(void*)Cyc_Core_get_exn_thrown();void*_tmpBF=_tmpBC;void*_tmpC0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBF)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 493
struct _tuple15*_tmpC1=({struct _tuple15*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=fieldname,_tmpC7->f2=dtname;_tmpC7;});struct _tuple15*new_pair=_tmpC1;
struct _tuple1*_tmpC2=fieldname;struct _fat_ptr*_tmpC4;union Cyc_Absyn_Nmspace _tmpC3;_LLC: _tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_LLD: {union Cyc_Absyn_Nmspace nmspace=_tmpC3;struct _fat_ptr*fieldvar=_tmpC4;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->f1=nmspace,({struct _fat_ptr*_tmp822=({struct _fat_ptr*_tmpC5=_cycalloc(sizeof(*_tmpC5));*_tmpC5=newvar;_tmpC5;});_tmpC6->f2=_tmp822;});_tmpC6;});
({struct Cyc_Dict_Dict _tmp823=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp823;});
return res;}}}else{_LL9: _tmpC0=_tmpBF;_LLA: {void*exn=_tmpC0;(int)_rethrow(exn);}}_LL6:;}}}}}
# 501
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp792;_tmp792.f1=fieldname,_tmp792.f2=dtname;_tmp792;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 508
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 511
struct _tuple1*_tmp826=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp825=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp825;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp826;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp824=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->fields=fs,_tmpC9->tagged=0;_tmpC9;});_tmpCA->impl=_tmp824;});_tmpCA;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 517
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 520
struct _tuple22 _tmpCB=*env;struct _tuple22 _stmttmp9=_tmpCB;struct _tuple22 _tmpCC=_stmttmp9;struct Cyc_List_List*_tmpCE;struct Cyc_List_List**_tmpCD;_LL1: _tmpCD=(_tmpCC.f1)->tuple_types;_tmpCE=_tmpCC.f2;_LL2: {struct Cyc_List_List**tuple_types=_tmpCD;struct Cyc_List_List*tqs0=_tmpCE;
# 522
{struct Cyc_List_List*_tmpCF=*tuple_types;struct Cyc_List_List*tts=_tmpCF;for(0;tts != 0;tts=tts->tl){
struct _tuple23*_tmpD0=(struct _tuple23*)tts->hd;struct _tuple23*_stmttmpA=_tmpD0;struct _tuple23*_tmpD1=_stmttmpA;struct Cyc_List_List*_tmpD3;void*_tmpD2;_LL4: _tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_LL5: {void*x=_tmpD2;struct Cyc_List_List*ts=_tmpD3;
struct Cyc_List_List*_tmpD4=tqs0;struct Cyc_List_List*tqs=_tmpD4;
for(0;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple13*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 533
struct _fat_ptr*xname=({struct _fat_ptr*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _fat_ptr _tmp828=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp793;_tmp793.tag=1U,_tmp793.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp793;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _fat_ptr _tmp827=({const char*_tmpE0="_tuple%d";_tag_fat(_tmpE0,sizeof(char),9U);});Cyc_aprintf(_tmp827,_tag_fat(_tmpDF,sizeof(void*),1U));});});*_tmpE2=_tmp828;});_tmpE2;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmpD5=0;struct Cyc_List_List*fs=_tmpD5;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
fs=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct Cyc_Absyn_Aggrfield*_tmp82A=({struct Cyc_Absyn_Aggrfield*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct _fat_ptr*_tmp829=Cyc_Absyn_fieldname(i);_tmpD6->name=_tmp829;}),_tmpD6->tq=Cyc_Toc_mt_tq,_tmpD6->type=(void*)ts2->hd,_tmpD6->width=0,_tmpD6->attributes=0,_tmpD6->requires_clause=0;_tmpD6;});_tmpD7->hd=_tmp82A;}),_tmpD7->tl=fs;_tmpD7;});}}
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmpD8=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpD8;
void*_tmpD9=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDE=_cycalloc(sizeof(*_tmpDE));*_tmpDE=sd;_tmpDE;})),0);void*ans=_tmpD9;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct Cyc_Absyn_Decl*_tmp82B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->tag=5U,_tmpDA->f1=sd;_tmpDA;}),0U);_tmpDB->hd=_tmp82B;}),_tmpDB->tl=Cyc_Toc_result_decls;_tmpDB;});
({struct Cyc_List_List*_tmp82D=({struct Cyc_List_List*_tmpDD=_region_malloc(d,sizeof(*_tmpDD));({struct _tuple23*_tmp82C=({struct _tuple23*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));_tmpDC->f1=ans,_tmpDC->f2=ts;_tmpDC;});_tmpDD->hd=_tmp82C;}),_tmpDD->tl=*tuple_types;_tmpDD;});*tuple_types=_tmp82D;});
return ans;}}}}
# 546
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 551
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 554
struct _tuple25*_tmpE3=env;struct Cyc_List_List*_tmpE6;enum Cyc_Absyn_AggrKind _tmpE5;struct Cyc_List_List**_tmpE4;_LL1: _tmpE4=(_tmpE3->f1)->anon_aggr_types;_tmpE5=(_tmpE3->f2)->f1;_tmpE6=(_tmpE3->f2)->f2;_LL2: {struct Cyc_List_List**anon_aggr_types=_tmpE4;enum Cyc_Absyn_AggrKind ak=_tmpE5;struct Cyc_List_List*fs=_tmpE6;
# 556
{struct Cyc_List_List*_tmpE7=*anon_aggr_types;struct Cyc_List_List*ts=_tmpE7;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpE8=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpE8;struct _tuple26*_tmpE9=_stmttmpB;struct Cyc_List_List*_tmpEC;enum Cyc_Absyn_AggrKind _tmpEB;void*_tmpEA;_LL4: _tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=_tmpE9->f3;_LL5: {void*x=_tmpEA;enum Cyc_Absyn_AggrKind ak2=_tmpEB;struct Cyc_List_List*fs2=_tmpEC;
if((int)ak != (int)ak2)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 565
struct _fat_ptr*xname=({struct _fat_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _fat_ptr _tmp82F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp794;_tmp794.tag=1U,_tmp794.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp794;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _fat_ptr _tmp82E=({const char*_tmpF5="_tuple%d";_tag_fat(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp82E,_tag_fat(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp82F;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpED=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpED;
sd->kind=ak;{
void*_tmpEE=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF3=_cycalloc(sizeof(*_tmpF3));*_tmpF3=sd;_tmpF3;})),0);void*ans=_tmpEE;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_Decl*_tmp830=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=5U,_tmpEF->f1=sd;_tmpEF;}),0U);_tmpF0->hd=_tmp830;}),_tmpF0->tl=Cyc_Toc_result_decls;_tmpF0;});
({struct Cyc_List_List*_tmp832=({struct Cyc_List_List*_tmpF2=_region_malloc(d,sizeof(*_tmpF2));({struct _tuple26*_tmp831=({struct _tuple26*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));_tmpF1->f1=ans,_tmpF1->f2=ak,_tmpF1->f3=fs;_tmpF1;});_tmpF2->hd=_tmp831;}),_tmpF2->tl=*anon_aggr_types;_tmpF2;});*anon_aggr_types=_tmp832;});
return ans;}}}}
# 573
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=ak,_tmpF8->f2=fs;_tmpF8;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 582
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 591
struct _tuple28 _tmpF9=*env;struct _tuple28 _stmttmpC=_tmpF9;struct _tuple28 _tmpFA=_stmttmpC;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;struct _tuple1*_tmpFC;struct Cyc_List_List**_tmpFB;_LL1: _tmpFB=(_tmpFA.f1)->abs_struct_types;_tmpFC=(_tmpFA.f2)->f1;_tmpFD=(_tmpFA.f2)->f2;_tmpFE=(_tmpFA.f2)->f3;_tmpFF=(_tmpFA.f2)->f4;_LL2: {struct Cyc_List_List**abs_struct_types=_tmpFB;struct _tuple1*struct_name=_tmpFC;struct Cyc_List_List*type_vars=_tmpFD;struct Cyc_List_List*type_args=_tmpFE;struct Cyc_List_List*fields=_tmpFF;
# 595
{struct Cyc_List_List*_tmp100=*abs_struct_types;struct Cyc_List_List*tts=_tmp100;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmp101=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmp101;struct _tuple29*_tmp102=_stmttmpD;void*_tmp105;struct Cyc_List_List*_tmp104;struct _tuple1*_tmp103;_LL4: _tmp103=_tmp102->f1;_tmp104=_tmp102->f2;_tmp105=_tmp102->f3;_LL5: {struct _tuple1*x=_tmp103;struct Cyc_List_List*ts2=_tmp104;void*t=_tmp105;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp833=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args);_tmp833 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmp106=type_args;struct Cyc_List_List*ts=_tmp106;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmp107=(void*)ts->hd;void*t=_tmp107;
void*_tmp108=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmp108;
{struct Cyc_Absyn_Kind*_tmp109=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp109;struct Cyc_Absyn_Kind*_tmp10A=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp10A)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 608
 if(Cyc_Unify_unify(t,t2)||({void*_tmp834=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp834,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 613
break;}}
# 615
if(okay)
return t;}}}}{
# 622
struct _fat_ptr*xname=({struct _fat_ptr*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct _fat_ptr _tmp836=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11C=({struct Cyc_Int_pa_PrintArg_struct _tmp795;_tmp795.tag=1U,_tmp795.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp795;});void*_tmp11A[1U];_tmp11A[0]=& _tmp11C;({struct _fat_ptr _tmp835=({const char*_tmp11B="_tuple%d";_tag_fat(_tmp11B,sizeof(char),9U);});Cyc_aprintf(_tmp835,_tag_fat(_tmp11A,sizeof(void*),1U));});});*_tmp11D=_tmp836;});_tmp11D;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp10B=0;struct Cyc_List_List*fs=_tmp10B;
# 626
({struct Cyc_List_List*_tmp838=({struct Cyc_List_List*_tmp10D=_region_malloc(d,sizeof(*_tmp10D));({struct _tuple29*_tmp837=({struct _tuple29*_tmp10C=_region_malloc(d,sizeof(*_tmp10C));_tmp10C->f1=struct_name,_tmp10C->f2=type_args,_tmp10C->f3=x;_tmp10C;});_tmp10D->hd=_tmp837;}),_tmp10D->tl=*abs_struct_types;_tmp10D;});*abs_struct_types=_tmp838;});{
# 629
struct _RegionHandle _tmp10E=_new_region("r");struct _RegionHandle*r=& _tmp10E;_push_region(r);
{struct Cyc_List_List*_tmp10F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp10F;
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp110=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp110;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 639
if((fields->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));({void*_tmp839=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=6U,_tmp111->f1=0;_tmp111;});_tmp112->hd=_tmp839;}),_tmp112->tl=atts;_tmp112;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 646
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=4U,({void*_tmp83C=Cyc_Toc_void_star_type();(_tmp113->f1).elt_type=_tmp83C;}),({struct Cyc_Absyn_Tqual _tmp83B=Cyc_Absyn_empty_tqual(0U);(_tmp113->f1).tq=_tmp83B;}),({
struct Cyc_Absyn_Exp*_tmp83A=Cyc_Absyn_uint_exp(0U,0U);(_tmp113->f1).num_elts=_tmp83A;}),(_tmp113->f1).zero_term=Cyc_Absyn_false_type,(_tmp113->f1).zt_loc=0U;_tmp113;});
# 650
fs=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Aggrfield*_tmp83D=({struct Cyc_Absyn_Aggrfield*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->name=f->name,_tmp114->tq=Cyc_Toc_mt_tq,_tmp114->type=t,_tmp114->width=f->width,_tmp114->attributes=atts,_tmp114->requires_clause=0;_tmp114;});_tmp115->hd=_tmp83D;}),_tmp115->tl=fs;_tmp115;});}
# 652
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp116=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp116;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));({struct Cyc_Absyn_Decl*_tmp83E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->tag=5U,_tmp117->f1=sd;_tmp117;}),0U);_tmp118->hd=_tmp83E;}),_tmp118->tl=Cyc_Toc_result_decls;_tmp118;});{
void*_tmp119=x;_npop_handler(0U);return _tmp119;}}}
# 630
;_pop_region(r);}}}}
# 658
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 662
struct _tuple27 env=({struct _tuple27 _tmp796;_tmp796.f1=struct_name,_tmp796.f2=type_vars,_tmp796.f3=type_args,_tmp796.f4=fields;_tmp796;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 670
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp841=({struct _fat_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _fat_ptr _tmp840=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp797;_tmp797.tag=1U,_tmp797.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp797;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _fat_ptr _tmp83F=({const char*_tmp11F="_tmp%X";_tag_fat(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp83F,_tag_fat(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp840;});_tmp121;});_tmp122->f2=_tmp841;});_tmp122;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 673
struct _tuple30 Cyc_Toc_temp_var_and_exp(){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp798;_tmp798.f1=v,({struct Cyc_Absyn_Exp*_tmp842=Cyc_Absyn_var_exp(v,0U);_tmp798.f2=_tmp842;});_tmp798;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 680
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp123=*env;struct _tuple31 _stmttmpF=_tmp123;struct _tuple31 _tmp124=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp125;_LL1: _tmp125=(_tmp124.f1)->temp_labels;_LL2: {struct Cyc_Xarray_Xarray*temp_labels=_tmp125;
int _tmp126=Cyc_Toc_fresh_label_counter ++;int i=_tmp126;
if(({int _tmp843=i;_tmp843 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct _fat_ptr _tmp845=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12C=({struct Cyc_Int_pa_PrintArg_struct _tmp79A;_tmp79A.tag=1U,_tmp79A.f1=(unsigned)i;_tmp79A;});void*_tmp12A[1U];_tmp12A[0]=& _tmp12C;({struct _fat_ptr _tmp844=({const char*_tmp12B="_LL%X";_tag_fat(_tmp12B,sizeof(char),6U);});Cyc_aprintf(_tmp844,_tag_fat(_tmp12A,sizeof(void*),1U));});});*_tmp12D=_tmp845;});_tmp12D;});
if(({int _tmp846=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp846 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp128=({struct Cyc_Warn_String_Warn_Warg_struct _tmp799;_tmp799.tag=0U,({struct _fat_ptr _tmp847=({const char*_tmp129="fresh_label: add_ind returned bad index...";_tag_fat(_tmp129,sizeof(char),43U);});_tmp799.f1=_tmp847;});_tmp799;});void*_tmp127[1U];_tmp127[0]=& _tmp128;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp127,sizeof(void*),1U));});
return res;}}}
# 690
static struct _fat_ptr*Cyc_Toc_fresh_label(){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 696
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp12E;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 703
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 709
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->f1=(*a).f1,_tmp12F->f2=(*a).f2,({void*_tmp848=Cyc_Toc_typ_to_c((*a).f3);_tmp12F->f3=_tmp848;});_tmp12F;});}
# 731 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp130=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp130;void*_tmp131=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp132;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->tag == 4U){_LL1: _tmp132=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1;_LL2: {struct Cyc_Absyn_ArrayInfo ai=_tmp132;
return({void*_tmp849=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp849,ai.tq);});}}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 738
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 740
struct Cyc_Absyn_Aggrfield*_tmp133=({struct Cyc_Absyn_Aggrfield*_tmp134=_cycalloc(sizeof(*_tmp134));*_tmp134=*f;_tmp134;});struct Cyc_Absyn_Aggrfield*ans=_tmp133;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 747
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 749
return;}
# 752
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp135=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp135;void*_tmp136=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp137;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp136)->tag == 2U){_LL1: _tmp137=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp136)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp137;
return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 758
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp138=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp138;struct Cyc_Absyn_Kind*_tmp139=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp139)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 764
static int Cyc_Toc_is_void_star(void*t){
void*_tmp13A=Cyc_Tcutil_compress(t);void*_stmttmp13=_tmp13A;void*_tmp13B=_stmttmp13;void*_tmp13C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B)->tag == 3U){_LL1: _tmp13C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B)->f1).elt_type;_LL2: {void*t2=_tmp13C;
return Cyc_Tcutil_is_void_type(t2);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 770
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 773
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 777
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp13D=t;void**_tmp13F;struct Cyc_Absyn_Datatypedecl*_tmp13E;struct Cyc_Absyn_Enumdecl*_tmp140;struct Cyc_Absyn_Aggrdecl*_tmp141;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp143;void*_tmp147;struct Cyc_Absyn_Typedefdecl*_tmp146;struct Cyc_List_List*_tmp145;struct _tuple1*_tmp144;struct Cyc_List_List*_tmp149;enum Cyc_Absyn_AggrKind _tmp148;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp150;struct Cyc_Absyn_VarargInfo*_tmp14F;int _tmp14E;struct Cyc_List_List*_tmp14D;void*_tmp14C;struct Cyc_Absyn_Tqual _tmp14B;unsigned _tmp154;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Tqual _tmp152;void*_tmp151;void*_tmp157;struct Cyc_Absyn_Tqual _tmp156;void*_tmp155;struct Cyc_Absyn_Tvar*_tmp158;void**_tmp159;struct Cyc_List_List*_tmp15A;struct _tuple1*_tmp15B;struct Cyc_List_List*_tmp15E;union Cyc_Absyn_AggrInfo _tmp15D;void*_tmp15C;struct Cyc_Absyn_Datatypefield*_tmp160;struct Cyc_Absyn_Datatypedecl*_tmp15F;switch(*((int*)_tmp13D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 790
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp15F=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1).KnownDatatypefield).val).f1;_tmp160=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1).KnownDatatypefield).val).f2;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmp15F;struct Cyc_Absyn_Datatypefield*tuf=_tmp160;
# 792
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{_LLB: _LLC:
# 794
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79B;_tmp79B.tag=0U,({struct _fat_ptr _tmp84A=({const char*_tmp163="unresolved DatatypeFieldType";_tag_fat(_tmp163,sizeof(char),29U);});_tmp79B.f1=_tmp84A;});_tmp79B;});void*_tmp161[1U];_tmp161[0]=& _tmp162;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp161,sizeof(void*),1U));});}case 1U: _LLF: _LL10:
# 803
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp15C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1;_tmp15D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_tmp15E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL1C: {void*c=_tmp15C;union Cyc_Absyn_AggrInfo info=_tmp15D;struct Cyc_List_List*ts=_tmp15E;
# 850
{union Cyc_Absyn_AggrInfo _tmp173=info;if((_tmp173.UnknownAggr).tag == 1){_LL45: _LL46:
# 852
 if(ts == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->tag=0U,_tmp174->f1=c,_tmp174->f2=0;_tmp174;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 856
struct Cyc_Absyn_Aggrdecl*_tmp175=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp175;
if(ad->expected_mem_kind){
# 859
if(ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp177=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp79D;_tmp79D.tag=5U,_tmp79D.f1=ad;_tmp79D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79C;_tmp79C.tag=0U,({struct _fat_ptr _tmp84B=({const char*_tmp179=" was never defined.";_tag_fat(_tmp179,sizeof(char),20U);});_tmp79C.f1=_tmp84B;});_tmp79C;});void*_tmp176[2U];_tmp176[0]=& _tmp177,_tmp176[1]=& _tmp178;Cyc_Warn_warn2(0U,_tag_fat(_tmp176,sizeof(void*),2U));});}
# 863
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp17A=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp17A;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp17B=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp17B;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp79F;_tmp79F.tag=5U,_tmp79F.f1=ad;_tmp79F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79E;_tmp79E.tag=0U,({struct _fat_ptr _tmp84C=({const char*_tmp17F=" ended up being abstract.";_tag_fat(_tmp17F,sizeof(char),26U);});_tmp79E.f1=_tmp84C;});_tmp79E;});void*_tmp17C[2U];_tmp17C[0]=& _tmp17D,_tmp17C[1]=& _tmp17E;Cyc_Warn_warn2(0U,_tag_fat(_tmp17C,sizeof(void*),2U));});{
# 875
struct _RegionHandle _tmp180=_new_region("r");struct _RegionHandle*r=& _tmp180;_push_region(r);
{struct Cyc_List_List*_tmp181=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp181;
void*_tmp182=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp182;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp183=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp183;}{
void*_tmp184=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0U);return _tmp184;}}
# 876
;_pop_region(r);}}
# 881
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15U: _LL1D: _tmp15B=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_LL1E: {struct _tuple1*tdn=_tmp15B;
return t;}case 16U: _LL1F: _tmp15A=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_LL20: {struct Cyc_List_List*fs=_tmp15A;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4U: _LL23: _LL24:
# 893
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 898
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp159=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13D)->f2;_LL4: {void**t2=_tmp159;
# 781
if(*t2 == 0){
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 785
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));}case 2U: _LL5: _tmp158=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13D)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp158;
# 787
if((int)(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;else{
return Cyc_Toc_void_star_type();}}case 3U: _LLD: _tmp155=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).elt_type;_tmp156=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).elt_tq;_tmp157=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).ptr_atts).bounds;_LLE: {void*t2=_tmp155;struct Cyc_Absyn_Tqual tq=_tmp156;void*bnds=_tmp157;
# 798
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 802
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}}case 4U: _LL13: _tmp151=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).elt_type;_tmp152=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).tq;_tmp153=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).num_elts;_tmp154=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).zt_loc;_LL14: {void*t2=_tmp151;struct Cyc_Absyn_Tqual tq=_tmp152;struct Cyc_Absyn_Exp*e=_tmp153;unsigned ztl=_tmp154;
# 806
return({void*_tmp850=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp84F=tq;struct Cyc_Absyn_Exp*_tmp84E=e;void*_tmp84D=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp850,_tmp84F,_tmp84E,_tmp84D,ztl);});}case 5U: _LL15: _tmp14B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).ret_tqual;_tmp14C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).ret_type;_tmp14D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).args;_tmp14E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).c_varargs;_tmp14F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).cyc_varargs;_tmp150=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).attributes;_LL16: {struct Cyc_Absyn_Tqual tq2=_tmp14B;void*t2=_tmp14C;struct Cyc_List_List*args=_tmp14D;int c_vararg=_tmp14E;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp14F;struct Cyc_List_List*atts=_tmp150;
# 812
struct Cyc_List_List*_tmp164=0;struct Cyc_List_List*new_atts=_tmp164;
for(0;atts != 0;atts=atts->tl){
void*_tmp165=(void*)atts->hd;void*_stmttmp14=_tmp165;void*_tmp166=_stmttmp14;switch(*((int*)_tmp166)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
 new_atts=({struct Cyc_List_List*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->hd=(void*)atts->hd,_tmp167->tl=new_atts;_tmp167;});goto _LL35;}_LL35:;}{
# 823
struct Cyc_List_List*_tmp168=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp168;
if(cyc_vararg != 0){
# 826
void*_tmp169=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));void*t=_tmp169;
struct _tuple9*_tmp16A=({struct _tuple9*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->f1=cyc_vararg->name,_tmp16C->f2=cyc_vararg->tq,_tmp16C->f3=t;_tmp16C;});struct _tuple9*vararg=_tmp16A;
new_args=({struct Cyc_List_List*_tmp851=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp851,({struct Cyc_List_List*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->hd=vararg,_tmp16B->tl=0;_tmp16B;}));});}
# 830
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->tag=5U,(_tmp16D->f1).tvars=0,(_tmp16D->f1).effect=0,(_tmp16D->f1).ret_tqual=tq2,({void*_tmp852=Cyc_Toc_typ_to_c(t2);(_tmp16D->f1).ret_type=_tmp852;}),(_tmp16D->f1).args=new_args,(_tmp16D->f1).c_varargs=c_vararg,(_tmp16D->f1).cyc_varargs=0,(_tmp16D->f1).rgn_po=0,(_tmp16D->f1).attributes=new_atts,(_tmp16D->f1).requires_clause=0,(_tmp16D->f1).requires_relns=0,(_tmp16D->f1).ensures_clause=0,(_tmp16D->f1).ensures_relns=0;_tmp16D;});}}case 6U: _LL17: _tmp14A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13D)->f1;_LL18: {struct Cyc_List_List*tqs=_tmp14A;
# 835
struct Cyc_List_List*_tmp16E=0;struct Cyc_List_List*tqs2=_tmp16E;
for(0;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp170=_cycalloc(sizeof(*_tmp170));({struct _tuple13*_tmp854=({struct _tuple13*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=(*((struct _tuple13*)tqs->hd)).f1,({void*_tmp853=Cyc_Toc_typ_to_c((*((struct _tuple13*)tqs->hd)).f2);_tmp16F->f2=_tmp853;});_tmp16F;});_tmp170->hd=_tmp854;}),_tmp170->tl=tqs2;_tmp170;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tqs2));}case 7U: _LL19: _tmp148=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13D)->f1;_tmp149=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13D)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp148;struct Cyc_List_List*fs=_tmp149;
# 844
struct Cyc_List_List*_tmp171=0;struct Cyc_List_List*fs2=_tmp171;
for(0;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp172=_cycalloc(sizeof(*_tmp172));({struct Cyc_Absyn_Aggrfield*_tmp856=({struct Cyc_Absyn_Aggrfield*_tmp855=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp855,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp172->hd=_tmp856;}),_tmp172->tl=fs2;_tmp172;});}
return({enum Cyc_Absyn_AggrKind _tmp857=k;Cyc_Toc_add_anon_aggr_type(_tmp857,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs2));});}case 8U: _LL21: _tmp144=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f1;_tmp145=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f2;_tmp146=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f3;_tmp147=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f4;_LL22: {struct _tuple1*tdn=_tmp144;struct Cyc_List_List*ts=_tmp145;struct Cyc_Absyn_Typedefdecl*td=_tmp146;void*topt=_tmp147;
# 887
if(topt == 0){
if(ts != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->tag=8U,_tmp185->f1=tdn,_tmp185->f2=0,_tmp185->f3=td,_tmp185->f4=0;_tmp185;});else{
return t;}}else{
# 892
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->tag=8U,_tmp186->f1=tdn,_tmp186->f2=0,_tmp186->f3=td,({void*_tmp858=Cyc_Toc_typ_to_c(topt);_tmp186->f4=_tmp858;});_tmp186;});}}case 9U: _LL2B: _tmp143=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp13D)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp143;
# 903
return t;}case 11U: _LL2D: _tmp142=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp13D)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp142;
# 908
if(e->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)){case 0U: _LL2F: _tmp141=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)->f1;_LL30: {struct Cyc_Absyn_Aggrdecl*ad=_tmp141;
# 912
Cyc_Toc_aggrdecl_to_c(ad);
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp140=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)->f1;_LL32: {struct Cyc_Absyn_Enumdecl*ed=_tmp140;
# 917
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _LL33: _tmp13E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)->f1;_tmp13F=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f2;_LL34: {struct Cyc_Absyn_Datatypedecl*dd=_tmp13E;void**t=_tmp13F;
# 920
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}}_LL0:;}
# 925
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp187=t;struct Cyc_Absyn_Tqual _tmp189;void*_tmp188;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->tag == 4U){_LL1: _tmp188=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->f1).elt_type;_tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->f1).tq;_LL2: {void*t2=_tmp188;struct Cyc_Absyn_Tqual tq=_tmp189;
# 928
return({void*_tmp859=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp859,e);});}}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 935
static int Cyc_Toc_atomic_type(void*t){
void*_tmp18A=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp18A;void*_tmp18B=_stmttmp15;struct Cyc_List_List*_tmp18C;struct Cyc_List_List*_tmp18D;void*_tmp18E;struct Cyc_List_List*_tmp190;void*_tmp18F;switch(*((int*)_tmp18B)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp18F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f1;_tmp190=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2;_LL4: {void*c=_tmp18F;struct Cyc_List_List*ts=_tmp190;
# 939
void*_tmp191=c;struct Cyc_Absyn_Datatypefield*_tmp193;struct Cyc_Absyn_Datatypedecl*_tmp192;union Cyc_Absyn_AggrInfo _tmp194;switch(*((int*)_tmp191)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp194=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp191)->f1;_LL25: {union Cyc_Absyn_AggrInfo info=_tmp194;
# 946
{union Cyc_Absyn_AggrInfo _tmp195=info;if((_tmp195.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 950
struct Cyc_Absyn_Aggrdecl*_tmp196=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp196;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp197=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp197;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp191)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp192=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp191)->f1).KnownDatatypefield).val).f1;_tmp193=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp191)->f1).KnownDatatypefield).val).f2;_LL27: {struct Cyc_Absyn_Datatypedecl*tud=_tmp192;struct Cyc_Absyn_Datatypefield*tuf=_tmp193;
# 957
{struct Cyc_List_List*_tmp198=tuf->typs;struct Cyc_List_List*tqs=_tmp198;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple13*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A1;_tmp7A1.tag=0U,({struct _fat_ptr _tmp85A=({const char*_tmp19C="atomic_typ: bad type ";_tag_fat(_tmp19C,sizeof(char),22U);});_tmp7A1.f1=_tmp85A;});_tmp7A1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7A0;_tmp7A0.tag=2U,_tmp7A0.f1=(void*)t;_tmp7A0;});void*_tmp199[2U];_tmp199[0]=& _tmp19A,_tmp199[1]=& _tmp19B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp199,sizeof(void*),2U));});}_LL11:;}case 5U: _LL5: _LL6:
# 962
 return 1;case 4U: _LL7: _tmp18E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18B)->f1).elt_type;_LL8: {void*t=_tmp18E;
return Cyc_Toc_atomic_type(t);}case 7U: _LL9: _tmp18D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18B)->f2;_LLA: {struct Cyc_List_List*fs=_tmp18D;
# 970
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6U: _LLB: _tmp18C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18B)->f1;_LLC: {struct Cyc_List_List*tqs=_tmp18C;
# 974
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple13*)tqs->hd)).f2))return 0;}
return 1;}case 3U: _LLD: _LLE:
# 979
 return 0;default: _LLF: _LL10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A3;_tmp7A3.tag=0U,({struct _fat_ptr _tmp85B=({const char*_tmp1A0="atomic_typ:  bad type ";_tag_fat(_tmp1A0,sizeof(char),23U);});_tmp7A3.f1=_tmp85B;});_tmp7A3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7A2;_tmp7A2.tag=2U,_tmp7A2.f1=(void*)t;_tmp7A2;});void*_tmp19D[2U];_tmp19D[0]=& _tmp19E,_tmp19D[1]=& _tmp19F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp19D,sizeof(void*),2U));});}_LL0:;}
# 985
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp1A1=Cyc_Tcutil_compress(t);void*_stmttmp16=_tmp1A1;void*_tmp1A2=_stmttmp16;struct Cyc_List_List*_tmp1A3;union Cyc_Absyn_AggrInfo _tmp1A4;switch(*((int*)_tmp1A2)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f1)->tag == 20U){_LL1: _tmp1A4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp1A4;
# 988
struct Cyc_Absyn_Aggrdecl*_tmp1A5=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A5;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A4;_tmp7A4.tag=0U,({struct _fat_ptr _tmp85C=({const char*_tmp1A8="is_poly_field: type missing fields";_tag_fat(_tmp1A8,sizeof(char),35U);});_tmp7A4.f1=_tmp85C;});_tmp7A4;});void*_tmp1A6[1U];_tmp1A6[0]=& _tmp1A7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A6,sizeof(void*),1U));});
_tmp1A3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A2)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A3;
# 993
struct Cyc_Absyn_Aggrfield*_tmp1A9=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp1A9;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A6;_tmp7A6.tag=0U,({struct _fat_ptr _tmp85D=({const char*_tmp1AD="is_poly_field: bad field ";_tag_fat(_tmp1AD,sizeof(char),26U);});_tmp7A6.f1=_tmp85D;});_tmp7A6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A5;_tmp7A5.tag=0U,_tmp7A5.f1=*f;_tmp7A5;});void*_tmp1AA[2U];_tmp1AA[0]=& _tmp1AB,_tmp1AA[1]=& _tmp1AC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AA,sizeof(void*),2U));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A8;_tmp7A8.tag=0U,({struct _fat_ptr _tmp85E=({const char*_tmp1B1="is_poly_field: bad type ";_tag_fat(_tmp1B1,sizeof(char),25U);});_tmp7A8.f1=_tmp85E;});_tmp7A8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7A7;_tmp7A7.tag=2U,_tmp7A7.f1=(void*)t;_tmp7A7;});void*_tmp1AE[2U];_tmp1AE[0]=& _tmp1AF,_tmp1AE[1]=& _tmp1B0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AE,sizeof(void*),2U));});}_LL0:;}
# 1004
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B2=e->r;void*_stmttmp17=_tmp1B2;void*_tmp1B3=_stmttmp17;struct _fat_ptr*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct _fat_ptr*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;switch(*((int*)_tmp1B3)){case 21U: _LL1: _tmp1B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B3)->f1;_tmp1B7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B3)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp1B6;struct _fat_ptr*f=_tmp1B7;
# 1007
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22U: _LL3: _tmp1B4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B3)->f1;_tmp1B5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B3)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1B4;struct _fat_ptr*f=_tmp1B5;
# 1010
void*_tmp1B8=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp18=_tmp1B8;void*_tmp1B9=_stmttmp18;void*_tmp1BA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->tag == 3U){_LL8: _tmp1BA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->f1).elt_type;_LL9: {void*t=_tmp1BA;
# 1012
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AA;_tmp7AA.tag=0U,({struct _fat_ptr _tmp85F=({const char*_tmp1BE="is_poly_project: bad type ";_tag_fat(_tmp1BE,sizeof(char),27U);});_tmp7AA.f1=_tmp85F;});_tmp7AA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1BD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7A9;_tmp7A9.tag=2U,_tmp7A9.f1=(void*)_check_null(e1->topt);_tmp7A9;});void*_tmp1BB[2U];_tmp1BB[0]=& _tmp1BC,_tmp1BB[1]=& _tmp1BD;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1BB,sizeof(void*),2U));});}_LL7:;}default: _LL5: _LL6:
# 1015
 return 0;}_LL0:;}
# 1020
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1BF[1U];_tmp1BF[0]=s;({struct Cyc_Absyn_Exp*_tmp860=fn_e;Cyc_Toc_fncall_exp_dl(_tmp860,_tag_fat(_tmp1BF,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1025
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C0[2U];_tmp1C0[0]=s,_tmp1C0[1]=n;({struct Cyc_Absyn_Exp*_tmp861=fn_e;Cyc_Toc_fncall_exp_dl(_tmp861,_tag_fat(_tmp1C0,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1030
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C1[2U];_tmp1C1[0]=rgn,_tmp1C1[1]=s;({struct Cyc_Absyn_Exp*_tmp862=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp862,_tag_fat(_tmp1C1,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C2[2U];_tmp1C2[0]=rgn,_tmp1C2[1]=s;({struct Cyc_Absyn_Exp*_tmp863=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp863,_tag_fat(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1038
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C3[3U];_tmp1C3[0]=rgn,_tmp1C3[1]=s,_tmp1C3[2]=n;({struct Cyc_Absyn_Exp*_tmp864=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp864,_tag_fat(_tmp1C3,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1042
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({void*_tmp1C4=0U;({struct Cyc_Absyn_Exp*_tmp865=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp865,_tag_fat(_tmp1C4,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1047
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1055
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C5=e->r;void*_stmttmp19=_tmp1C5;void*_tmp1C6=_stmttmp19;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C6)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C6)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1060
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));({struct Cyc_Absyn_Decl*_tmp866=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->tag=0U,_tmp1C7->f1=vd;_tmp1C7;}),0U);_tmp1C8->hd=_tmp866;}),_tmp1C8->tl=Cyc_Toc_result_decls;_tmp1C8;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1068
 xexp=({void*_tmp867=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp867,e);});
# 1070
xplussz=({void*_tmp868=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp868,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1073
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1C9[3U];({struct _tuple20*_tmp86B=({struct _tuple20*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->f1=0,_tmp1CA->f2=xexp;_tmp1CA;});_tmp1C9[0]=_tmp86B;}),({
struct _tuple20*_tmp86A=({struct _tuple20*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->f1=0,_tmp1CB->f2=xexp;_tmp1CB;});_tmp1C9[1]=_tmp86A;}),({
struct _tuple20*_tmp869=({struct _tuple20*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=0,_tmp1CC->f2=xplussz;_tmp1CC;});_tmp1C9[2]=_tmp869;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C9,sizeof(struct _tuple20*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1108 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE->break_lab=0,_tmp1CE->continue_lab=0,_tmp1CE->fallthru_info=0,_tmp1CE->toplevel=1,({int*_tmp86C=({int*_tmp1CD=_region_malloc(r,sizeof(*_tmp1CD));*_tmp1CD=0;_tmp1CD;});_tmp1CE->in_lhs=_tmp86C;}),_tmp1CE->rgn=r;_tmp1CE;});}
# 1112
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1CF=nv;int _tmp1D0;_LL1: _tmp1D0=_tmp1CF->toplevel;_LL2: {int t=_tmp1D0;
return t;}}
# 1116
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D1=e;int*_tmp1D6;int _tmp1D5;struct Cyc_Toc_FallthruInfo*_tmp1D4;struct _fat_ptr**_tmp1D3;struct _fat_ptr**_tmp1D2;_LL1: _tmp1D2=_tmp1D1->break_lab;_tmp1D3=_tmp1D1->continue_lab;_tmp1D4=_tmp1D1->fallthru_info;_tmp1D5=_tmp1D1->toplevel;_tmp1D6=_tmp1D1->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D2;struct _fat_ptr**c=_tmp1D3;struct Cyc_Toc_FallthruInfo*f=_tmp1D4;int t=_tmp1D5;int*lhs=_tmp1D6;
return({struct Cyc_Toc_Env*_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));_tmp1D7->break_lab=b,_tmp1D7->continue_lab=c,_tmp1D7->fallthru_info=f,_tmp1D7->toplevel=0,_tmp1D7->in_lhs=lhs,_tmp1D7->rgn=r;_tmp1D7;});}}
# 1120
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D8=e;int*_tmp1DD;int _tmp1DC;struct Cyc_Toc_FallthruInfo*_tmp1DB;struct _fat_ptr**_tmp1DA;struct _fat_ptr**_tmp1D9;_LL1: _tmp1D9=_tmp1D8->break_lab;_tmp1DA=_tmp1D8->continue_lab;_tmp1DB=_tmp1D8->fallthru_info;_tmp1DC=_tmp1D8->toplevel;_tmp1DD=_tmp1D8->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D9;struct _fat_ptr**c=_tmp1DA;struct Cyc_Toc_FallthruInfo*f=_tmp1DB;int t=_tmp1DC;int*lhs=_tmp1DD;
return({struct Cyc_Toc_Env*_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE->break_lab=b,_tmp1DE->continue_lab=c,_tmp1DE->fallthru_info=f,_tmp1DE->toplevel=1,_tmp1DE->in_lhs=lhs,_tmp1DE->rgn=r;_tmp1DE;});}}
# 1124
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1DF=nv;int*_tmp1E0;_LL1: _tmp1E0=_tmp1DF->in_lhs;_LL2: {int*b=_tmp1E0;
return*b;}}
# 1128
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E1=e;int*_tmp1E2;_LL1: _tmp1E2=_tmp1E1->in_lhs;_LL2: {int*lhs=_tmp1E2;
*lhs=b;}}
# 1133
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E3=e;int*_tmp1E8;int _tmp1E7;struct Cyc_Toc_FallthruInfo*_tmp1E6;struct _fat_ptr**_tmp1E5;struct _fat_ptr**_tmp1E4;_LL1: _tmp1E4=_tmp1E3->break_lab;_tmp1E5=_tmp1E3->continue_lab;_tmp1E6=_tmp1E3->fallthru_info;_tmp1E7=_tmp1E3->toplevel;_tmp1E8=_tmp1E3->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1E4;struct _fat_ptr**c=_tmp1E5;struct Cyc_Toc_FallthruInfo*f=_tmp1E6;int t=_tmp1E7;int*lhs=_tmp1E8;
return({struct Cyc_Toc_Env*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9->break_lab=b,_tmp1E9->continue_lab=c,_tmp1E9->fallthru_info=f,_tmp1E9->toplevel=t,_tmp1E9->in_lhs=lhs,_tmp1E9->rgn=r;_tmp1E9;});}}
# 1140
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EA=e;int*_tmp1EF;int _tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1ED;struct _fat_ptr**_tmp1EC;struct _fat_ptr**_tmp1EB;_LL1: _tmp1EB=_tmp1EA->break_lab;_tmp1EC=_tmp1EA->continue_lab;_tmp1ED=_tmp1EA->fallthru_info;_tmp1EE=_tmp1EA->toplevel;_tmp1EF=_tmp1EA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1EB;struct _fat_ptr**c=_tmp1EC;struct Cyc_Toc_FallthruInfo*f=_tmp1ED;int t=_tmp1EE;int*lhs=_tmp1EF;
return({struct Cyc_Toc_Env*_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0->break_lab=0,_tmp1F0->continue_lab=0,_tmp1F0->fallthru_info=f,_tmp1F0->toplevel=t,_tmp1F0->in_lhs=lhs,_tmp1F0->rgn=r;_tmp1F0;});}}
# 1146
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1150
struct Cyc_Toc_Env*_tmp1F1=e;int*_tmp1F6;int _tmp1F5;struct Cyc_Toc_FallthruInfo*_tmp1F4;struct _fat_ptr**_tmp1F3;struct _fat_ptr**_tmp1F2;_LL1: _tmp1F2=_tmp1F1->break_lab;_tmp1F3=_tmp1F1->continue_lab;_tmp1F4=_tmp1F1->fallthru_info;_tmp1F5=_tmp1F1->toplevel;_tmp1F6=_tmp1F1->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1F2;struct _fat_ptr**c=_tmp1F3;struct Cyc_Toc_FallthruInfo*f=_tmp1F4;int t=_tmp1F5;int*lhs=_tmp1F6;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));
_tmp1F9->label=fallthru_l,_tmp1F9->binders=fallthru_binders;_tmp1F9;});
return({struct Cyc_Toc_Env*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));({struct _fat_ptr**_tmp86D=({struct _fat_ptr**_tmp1F7=_region_malloc(r,sizeof(*_tmp1F7));*_tmp1F7=break_l;_tmp1F7;});_tmp1F8->break_lab=_tmp86D;}),_tmp1F8->continue_lab=c,_tmp1F8->fallthru_info=fi,_tmp1F8->toplevel=t,_tmp1F8->in_lhs=lhs,_tmp1F8->rgn=r;_tmp1F8;});}}
# 1155
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1157
struct Cyc_Toc_Env*_tmp1FA=e;int*_tmp1FF;int _tmp1FE;struct Cyc_Toc_FallthruInfo*_tmp1FD;struct _fat_ptr**_tmp1FC;struct _fat_ptr**_tmp1FB;_LL1: _tmp1FB=_tmp1FA->break_lab;_tmp1FC=_tmp1FA->continue_lab;_tmp1FD=_tmp1FA->fallthru_info;_tmp1FE=_tmp1FA->toplevel;_tmp1FF=_tmp1FA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1FB;struct _fat_ptr**c=_tmp1FC;struct Cyc_Toc_FallthruInfo*f=_tmp1FD;int t=_tmp1FE;int*lhs=_tmp1FF;
return({struct Cyc_Toc_Env*_tmp201=_region_malloc(r,sizeof(*_tmp201));({struct _fat_ptr**_tmp86E=({struct _fat_ptr**_tmp200=_region_malloc(r,sizeof(*_tmp200));*_tmp200=break_l;_tmp200;});_tmp201->break_lab=_tmp86E;}),_tmp201->continue_lab=c,_tmp201->fallthru_info=0,_tmp201->toplevel=t,_tmp201->in_lhs=lhs,_tmp201->rgn=r;_tmp201;});}}
# 1164
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1166
struct Cyc_Toc_Env*_tmp202=e;int*_tmp207;int _tmp206;struct Cyc_Toc_FallthruInfo*_tmp205;struct _fat_ptr**_tmp204;struct _fat_ptr**_tmp203;_LL1: _tmp203=_tmp202->break_lab;_tmp204=_tmp202->continue_lab;_tmp205=_tmp202->fallthru_info;_tmp206=_tmp202->toplevel;_tmp207=_tmp202->in_lhs;_LL2: {struct _fat_ptr**b=_tmp203;struct _fat_ptr**c=_tmp204;struct Cyc_Toc_FallthruInfo*f=_tmp205;int t=_tmp206;int*lhs=_tmp207;
return({struct Cyc_Toc_Env*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209->break_lab=0,_tmp209->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp86F=({struct Cyc_Toc_FallthruInfo*_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208->label=next_l,_tmp208->binders=0;_tmp208;});_tmp209->fallthru_info=_tmp86F;}),_tmp209->toplevel=t,_tmp209->in_lhs=lhs,_tmp209->rgn=r;_tmp209;});}}
# 1179 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1182
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp20A=e->annot;void*_stmttmp1A=_tmp20A;void*_tmp20B=_stmttmp1A;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp20B)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp20B)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp20B)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp20B)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp20B)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp20B)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AB;_tmp7AB.tag=0U,({struct _fat_ptr _tmp870=({const char*_tmp20E="Toc: do_null_check";_tag_fat(_tmp20E,sizeof(char),19U);});_tmp7AB.f1=_tmp870;});_tmp7AB;});void*_tmp20C[1U];_tmp20C[0]=& _tmp20D;({unsigned _tmp871=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp871,_tag_fat(_tmp20C,sizeof(void*),1U));});});}}}}}}_LL0:;}
# 1198
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1200
int ans;
int _tmp20F=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp20F;
void*_tmp210=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp210;
void*_tmp211=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp211;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp212=old_typ;void*_tmp216;void*_tmp215;struct Cyc_Absyn_Tqual _tmp214;void*_tmp213;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp212)->tag == 3U){_LL1: _tmp213=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp212)->f1).elt_type;_tmp214=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp212)->f1).elt_tq;_tmp215=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp212)->f1).ptr_atts).bounds;_tmp216=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp212)->f1).ptr_atts).zero_term;_LL2: {void*ta=_tmp213;struct Cyc_Absyn_Tqual tq=_tmp214;void*b=_tmp215;void*zt=_tmp216;
# 1211
{void*_tmp217=annot;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_Absyn_Exp*_tmp219;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp217)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1213
 if(!((unsigned)({void*_tmp872=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp872,b);}))){
# 1216
void*_tmp21A=({void*_tmp873=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp873,tq);});void*newt=_tmp21A;
({void*_tmp876=({void*_tmp875=newt;Cyc_Toc_cast_it_r(_tmp875,({struct Cyc_Absyn_Exp*_tmp874=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp874,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp876;});
ptr->topt=newt;}
# 1220
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp217)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1223
 if(!((unsigned)({void*_tmp877=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp877,b);}))){
# 1225
void*_tmp21B=({void*_tmp878=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp878,tq);});void*newt=_tmp21B;
({void*_tmp87B=({void*_tmp87A=newt;Cyc_Toc_cast_it_r(_tmp87A,({struct Cyc_Absyn_Exp*_tmp879=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp879,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp87B;});
ptr->topt=newt;
# 1232
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AC;_tmp7AC.tag=0U,({struct _fat_ptr _tmp87C=({const char*_tmp21E="subscript of ? with no bounds check but need null check";_tag_fat(_tmp21E,sizeof(char),56U);});_tmp7AC.f1=_tmp87C;});_tmp7AC;});void*_tmp21C[1U];_tmp21C[0]=& _tmp21D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp21C,sizeof(void*),1U));});}
# 1236
({void*_tmp880=({void*_tmp87F=new_typ;Cyc_Toc_cast_it_r(_tmp87F,({struct Cyc_Absyn_Exp*_tmp21F[1U];({struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_new_exp(ptr->r,0U);_tmp21F[0]=_tmp87D;});({struct Cyc_Absyn_Exp*_tmp87E=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp87E,_tag_fat(_tmp21F,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp880;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp217)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp217)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1242
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp885=({void*_tmp884=newt;Cyc_Toc_cast_it_r(_tmp884,({struct Cyc_Absyn_Exp*_tmp220[3U];({
struct Cyc_Absyn_Exp*_tmp882=Cyc_Absyn_new_exp(ptr->r,0U);_tmp220[0]=_tmp882;}),({
struct Cyc_Absyn_Exp*_tmp881=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp220[1]=_tmp881;}),_tmp220[2]=ind;({struct Cyc_Absyn_Exp*_tmp883=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp883,_tag_fat(_tmp220,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1245
ptr->r=_tmp885;});
# 1249
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp217)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp219=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp217)->f1;_LLF: {struct Cyc_Absyn_Exp*bd=_tmp219;
# 1253
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp218=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp217)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp218=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp217)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp218;
# 1256
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1261
struct _tuple14 _tmp221=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple14 _stmttmp1B=_tmp221;struct _tuple14 _tmp222=_stmttmp1B;int _tmp224;unsigned _tmp223;_LL15: _tmp223=_tmp222.f1;_tmp224=_tmp222.f2;_LL16: {unsigned i=_tmp223;int valid=_tmp224;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1264
({void*_tmp88A=({void*_tmp889=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp889,({struct Cyc_Absyn_Exp*_tmp225[4U];({
struct Cyc_Absyn_Exp*_tmp887=Cyc_Absyn_new_exp(ptr->r,0U);_tmp225[0]=_tmp887;}),_tmp225[1]=bd,({
struct Cyc_Absyn_Exp*_tmp886=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp225[2]=_tmp886;}),_tmp225[3]=ind;({struct Cyc_Absyn_Exp*_tmp888=fn_e;Cyc_Toc_fncall_exp_dl(_tmp888,_tag_fat(_tmp225,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1264
ptr->r=_tmp88A;});
# 1267
ans=1;}else{
# 1270
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1273
({void*_tmp88E=({void*_tmp88D=new_typ;Cyc_Toc_cast_it_r(_tmp88D,({struct Cyc_Absyn_Exp*_tmp226[1U];({struct Cyc_Absyn_Exp*_tmp88B=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp226[0]=_tmp88B;});({struct Cyc_Absyn_Exp*_tmp88C=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp88C,_tag_fat(_tmp226,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1273
ptr->r=_tmp88E;});
# 1275
ans=0;}else{
# 1279
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp892=({void*_tmp891=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp891,({struct Cyc_Absyn_Exp*_tmp227[3U];({
struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_new_exp(ptr->r,0U);_tmp227[0]=_tmp88F;}),_tmp227[1]=bd,_tmp227[2]=ind;({struct Cyc_Absyn_Exp*_tmp890=fn_e;Cyc_Toc_fncall_exp_dl(_tmp890,_tag_fat(_tmp227,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1280
ptr->r=_tmp892;});
# 1282
ans=1;}}
# 1285
goto _LL5;}}}else{_LL12: _LL13:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp229=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AD;_tmp7AD.tag=0U,({struct _fat_ptr _tmp893=({const char*_tmp22A="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp22A,sizeof(char),34U);});_tmp7AD.f1=_tmp893;});_tmp7AD;});void*_tmp228[1U];_tmp228[0]=& _tmp229;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp228,sizeof(void*),1U));});}}}}}}_LL5:;}
# 1288
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AE;_tmp7AE.tag=0U,({struct _fat_ptr _tmp894=({const char*_tmp22D="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp22D,sizeof(char),31U);});_tmp7AE.f1=_tmp894;});_tmp7AE;});void*_tmp22B[1U];_tmp22B[0]=& _tmp22C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp22B,sizeof(void*),1U));});}_LL0:;}}
# 1294
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp22F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AF;_tmp7AF.tag=0U,({struct _fat_ptr _tmp895=({const char*_tmp230="Missing type in primop ";_tag_fat(_tmp230,sizeof(char),24U);});_tmp7AF.f1=_tmp895;});_tmp7AF;});void*_tmp22E[1U];_tmp22E[0]=& _tmp22F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp22E,sizeof(void*),1U));});
return(void*)_check_null(e->topt);}
# 1298
static struct _tuple13*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple13*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->f1=Cyc_Toc_mt_tq,({void*_tmp896=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp231->f2=_tmp896;});_tmp231;});}
# 1303
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp232=e->r;void*_stmttmp1C=_tmp232;void*_tmp233=_stmttmp1C;int _tmp235;struct Cyc_Absyn_Exp*_tmp234;int _tmp237;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_List_List*_tmp238;switch(*((int*)_tmp233)){case 26U: _LL1: _tmp238=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_LL2: {struct Cyc_List_List*dles=_tmp238;
# 1306
{struct Cyc_List_List*_tmp239=dles;struct Cyc_List_List*dles2=_tmp239;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple20*)dles2->hd)).f1 != 0)
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp23A="array designators for abstract-field initialization";_tag_fat(_tmp23A,sizeof(char),52U);}));}}
_tmp236=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles),0U);_tmp237=0;goto _LL4;}case 27U: _LL3: _tmp236=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp233)->f2;_tmp237=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp233)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp236;int zt=_tmp237;
_tmp234=bd;_tmp235=zt;goto _LL6;}case 28U: _LL5: _tmp234=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_tmp235=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp233)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp234;int zt=_tmp235;
# 1312
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp897=bd;Cyc_Absyn_add_exp(_tmp897,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1321
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1329
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp23B=e->r;void*_stmttmp1D=_tmp23B;void*_tmp23C=_stmttmp1D;struct Cyc_List_List*_tmp23D;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23C)->tag == 29U){_LL1: _tmp23D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23C)->f3;_LL2: {struct Cyc_List_List*dles2=_tmp23D;
dles=dles2;goto _LL0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1335
{void*_tmp23E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp1E=_tmp23E;void*_tmp23F=_stmttmp1E;struct Cyc_Absyn_Aggrdecl*_tmp240;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23F)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp240=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23F)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp240;
# 1337
if(ad->impl == 0)
return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1345
 return 0;}_LL5:;}
# 1347
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp241=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp241;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp242=(struct _tuple20*)dles->hd;struct _tuple20*_stmttmp1F=_tmp242;struct _tuple20*_tmp243=_stmttmp1F;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_List_List*_tmp244;_LLB: _tmp244=_tmp243->f1;_tmp245=_tmp243->f2;_LLC: {struct Cyc_List_List*ds=_tmp244;struct Cyc_Absyn_Exp*e2=_tmp245;
struct _fat_ptr*_tmp246=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp246;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp247=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp247;
if(nested_ans != 0)
return nested_ans;{
void*_tmp248=Cyc_Tcutil_compress(last_type_field->type);void*_stmttmp20=_tmp248;void*_tmp249=_stmttmp20;struct Cyc_Absyn_Exp*_tmp24B;void*_tmp24A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249)->tag == 4U){_LLE: _tmp24A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249)->f1).elt_type;_tmp24B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249)->f1).num_elts;_LLF: {void*elt_type=_tmp24A;struct Cyc_Absyn_Exp*type_bd=_tmp24B;
# 1362
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1368
return({struct Cyc_Absyn_Exp*_tmp89A=({struct Cyc_Absyn_Exp*_tmp24C[2U];_tmp24C[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1370
struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp24C[1]=_tmp898;});({struct Cyc_Absyn_Exp*_tmp899=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp899,_tag_fat(_tmp24C,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1368
Cyc_Absyn_add_exp(_tmp89A,
# 1371
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{_LL10: _LL11:
 return 0;}_LLD:;}}}}
# 1376
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B0;_tmp7B0.tag=0U,({struct _fat_ptr _tmp89B=({const char*_tmp24F="get_varsizeexp: did not find last struct field";_tag_fat(_tmp24F,sizeof(char),47U);});_tmp7B0.f1=_tmp89B;});_tmp7B0;});void*_tmp24D[1U];_tmp24D[0]=& _tmp24E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp24D,sizeof(void*),1U));});}}
# 1379
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp250=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp250;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp251=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp251;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1386
({struct Cyc_Warn_String_Warn_Warg_struct _tmp253=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B3;_tmp7B3.tag=0U,({struct _fat_ptr _tmp89C=({const char*_tmp257="get_member_offset ";_tag_fat(_tmp257,sizeof(char),19U);});_tmp7B3.f1=_tmp89C;});_tmp7B3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp254=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B2;_tmp7B2.tag=0U,_tmp7B2.f1=*f;_tmp7B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp255=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B1;_tmp7B1.tag=0U,({struct _fat_ptr _tmp89D=({const char*_tmp256=" failed";_tag_fat(_tmp256,sizeof(char),8U);});_tmp7B1.f1=_tmp89D;});_tmp7B1;});void*_tmp252[3U];_tmp252[0]=& _tmp253,_tmp252[1]=& _tmp254,_tmp252[2]=& _tmp255;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp252,sizeof(void*),3U));});}struct _tuple32{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1390
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple32*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1393
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1396
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258->tag=5U,_tmp258->f1=e1,_tmp258->f2=incr;_tmp258;}),0U);}
# 1400
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1409
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1411
void*_tmp259=e1->r;void*_stmttmp21=_tmp259;void*_tmp25A=_stmttmp21;int _tmp25E;int _tmp25D;struct _fat_ptr*_tmp25C;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp260;void*_tmp25F;struct Cyc_Absyn_Stmt*_tmp261;switch(*((int*)_tmp25A)){case 37U: _LL1: _tmp261=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp25A)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp261;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14U: _LL3: _tmp25F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25A)->f1;_tmp260=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25A)->f2;_LL4: {void*t=_tmp25F;struct Cyc_Absyn_Exp*e=_tmp260;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21U: _LL5: _tmp25B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25A)->f1;_tmp25C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25A)->f2;_tmp25D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25A)->f3;_tmp25E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25A)->f4;_LL6: {struct Cyc_Absyn_Exp*e=_tmp25B;struct _fat_ptr*fld=_tmp25C;int is_tagged=_tmp25D;int is_read=_tmp25E;
# 1416
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp8A0=e1;struct Cyc_List_List*_tmp89F=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=fld,_tmp262->tl=fs;_tmp262;});struct Cyc_Absyn_Exp*(*_tmp89E)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp8A0,_tmp89F,_tmp89E,f_env);});
goto _LL0;}default: _LL7: _LL8: {
# 1424
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1426
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp8A1=(f(e1_copy,f_env))->r;e1->r=_tmp8A1;});
goto _LL0;}}_LL0:;}
# 1432
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1434
void*_tmp263=s->r;void*_stmttmp22=_tmp263;void*_tmp264=_stmttmp22;struct Cyc_Absyn_Stmt*_tmp265;struct Cyc_Absyn_Stmt*_tmp267;struct Cyc_Absyn_Decl*_tmp266;struct Cyc_Absyn_Exp*_tmp268;switch(*((int*)_tmp264)){case 1U: _LL1: _tmp268=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp264)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp268;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12U: _LL3: _tmp266=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp264)->f1;_tmp267=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp264)->f2;_LL4: {struct Cyc_Absyn_Decl*d=_tmp266;struct Cyc_Absyn_Stmt*s2=_tmp267;
# 1437
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2U: _LL5: _tmp265=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp264)->f2;_LL6: {struct Cyc_Absyn_Stmt*s2=_tmp265;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B5;_tmp7B5.tag=0U,({struct _fat_ptr _tmp8A2=({const char*_tmp26C="lvalue_assign_stmt: ";_tag_fat(_tmp26C,sizeof(char),21U);});_tmp7B5.f1=_tmp8A2;});_tmp7B5;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp26B=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7B4;_tmp7B4.tag=4U,_tmp7B4.f1=s;_tmp7B4;});void*_tmp269[2U];_tmp269[0]=& _tmp26A,_tmp269[1]=& _tmp26B;Cyc_Toc_toc_impos2(_tag_fat(_tmp269,sizeof(void*),2U));});}_LL0:;}
# 1443
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1447
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp26D=e->r;void*_stmttmp23=_tmp26D;void*_tmp26E=_stmttmp23;struct Cyc_Absyn_Stmt*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp**_tmp272;void**_tmp271;switch(*((int*)_tmp26E)){case 14U: _LL1: _tmp271=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_tmp272=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26E)->f2;_LL2: {void**t=_tmp271;struct Cyc_Absyn_Exp**e1=_tmp272;
# 1450
({struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Toc_push_address_exp(*e1);*e1=_tmp8A3;});
({void*_tmp8A4=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp8A4;});
return e;}case 20U: _LL3: _tmp270=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp270;
# 1454
return e1;}case 37U: _LL5: _tmp26F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26E)->f1;_LL6: {struct Cyc_Absyn_Stmt*s=_tmp26F;
# 1458
Cyc_Toc_push_address_stmt(s);
return e;}default: _LL7: _LL8:
# 1461
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp274=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B7;_tmp7B7.tag=0U,({struct _fat_ptr _tmp8A5=({const char*_tmp276="can't take & of exp ";_tag_fat(_tmp276,sizeof(char),21U);});_tmp7B7.f1=_tmp8A5;});_tmp7B7;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp275=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7B6;_tmp7B6.tag=3U,_tmp7B6.f1=e;_tmp7B6;});void*_tmp273[2U];_tmp273[0]=& _tmp274,_tmp273[1]=& _tmp275;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp273,sizeof(void*),2U));});}_LL0:;}
# 1465
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp277=s->r;void*_stmttmp24=_tmp277;void*_tmp278=_stmttmp24;struct Cyc_Absyn_Exp**_tmp279;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Absyn_Stmt*_tmp27B;switch(*((int*)_tmp278)){case 2U: _LL1: _tmp27B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp278)->f2;_LL2: {struct Cyc_Absyn_Stmt*s2=_tmp27B;
_tmp27A=s2;goto _LL4;}case 12U: _LL3: _tmp27A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp278)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp27A;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1U: _LL5: _tmp279=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp278)->f1;_LL6: {struct Cyc_Absyn_Exp**e=_tmp279;
({struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Toc_push_address_exp(*e);*e=_tmp8A6;});goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B9;_tmp7B9.tag=0U,({struct _fat_ptr _tmp8A7=({const char*_tmp27F="can't take & of stmt ";_tag_fat(_tmp27F,sizeof(char),22U);});_tmp7B9.f1=_tmp8A7;});_tmp7B9;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp27E=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7B8;_tmp7B8.tag=4U,_tmp7B8.f1=s;_tmp7B8;});void*_tmp27C[2U];_tmp27C[0]=& _tmp27D,_tmp27C[1]=& _tmp27E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp27C,sizeof(void*),2U));});}_LL0:;}
# 1477
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1489
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp280=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp280;
void*_tmp281=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp281;
void*_tmp282=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp282;
struct Cyc_Core_Opt*_tmp283=({struct Cyc_Core_Opt*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->v=c_ptr_type;_tmp2B6;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp283;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp284=e1->r;void*_stmttmp25=_tmp284;void*_tmp285=_stmttmp25;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_Absyn_Exp*_tmp288;switch(*((int*)_tmp285)){case 20U: _LL1: _tmp288=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp285)->f1;_LL2: {struct Cyc_Absyn_Exp*ea=_tmp288;
# 1497
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1502
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23U: _LL3: _tmp286=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp285)->f1;_tmp287=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp285)->f2;_LL4: {struct Cyc_Absyn_Exp*ea=_tmp286;struct Cyc_Absyn_Exp*eb=_tmp287;
# 1504
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1509
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp289[3U];_tmp289[0]=ea,({
struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp289[1]=_tmp8A8;}),_tmp289[2]=eb;({struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp8A9,_tag_fat(_tmp289,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BA;_tmp7BA.tag=0U,({struct _fat_ptr _tmp8AA=({const char*_tmp28C="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp28C,sizeof(char),53U);});_tmp7BA.f1=_tmp8AA;});_tmp7BA;});void*_tmp28A[1U];_tmp28A[0]=& _tmp28B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp28A,sizeof(void*),1U));});}_LL0:;}{
# 1515
struct _tuple1*_tmp28D=Cyc_Toc_temp_var();struct _tuple1*x=_tmp28D;
struct _RegionHandle _tmp28E=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp28E;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp28F=({struct Cyc_Absyn_Vardecl*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->sc=Cyc_Absyn_Public,_tmp2B5->name=x,_tmp2B5->varloc=0U,_tmp2B5->tq=Cyc_Toc_mt_tq,_tmp2B5->type=c_fat_ptr_type,_tmp2B5->initializer=xinit,_tmp2B5->rgn=0,_tmp2B5->attributes=0,_tmp2B5->escapes=0;_tmp2B5;});struct Cyc_Absyn_Vardecl*x_vd=_tmp28F;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp290=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->tag=4U,_tmp2B4->f1=x_vd;_tmp2B4;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp290;
struct Cyc_Absyn_Exp*_tmp291=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp291;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp292=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp292;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp293=Cyc_Toc_temp_var();struct _tuple1*y=_tmp293;
struct Cyc_Absyn_Vardecl*_tmp294=({struct Cyc_Absyn_Vardecl*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->sc=Cyc_Absyn_Public,_tmp2B3->name=y,_tmp2B3->varloc=0U,_tmp2B3->tq=Cyc_Toc_mt_tq,_tmp2B3->type=c_elt_type,_tmp2B3->initializer=deref_x,_tmp2B3->rgn=0,_tmp2B3->attributes=0,_tmp2B3->escapes=0;_tmp2B3;});struct Cyc_Absyn_Vardecl*y_vd=_tmp294;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp295=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->tag=4U,_tmp2B2->f1=y_vd;_tmp2B2;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp295;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp296=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp296;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp8AC=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp8AB=y_exp;Cyc_Absyn_prim2_exp(_tmp8AC,_tmp8AB,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1536
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp297=Cyc_Toc_temp_var();struct _tuple1*z=_tmp297;
struct Cyc_Absyn_Vardecl*_tmp298=({struct Cyc_Absyn_Vardecl*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->sc=Cyc_Absyn_Public,_tmp2B1->name=z,_tmp2B1->varloc=0U,_tmp2B1->tq=Cyc_Toc_mt_tq,_tmp2B1->type=c_elt_type,_tmp2B1->initializer=z_init,_tmp2B1->rgn=0,_tmp2B1->attributes=0,_tmp2B1->escapes=0;_tmp2B1;});struct Cyc_Absyn_Vardecl*z_vd=_tmp298;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp299=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->tag=4U,_tmp2B0->f1=z_vd;_tmp2B0;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp299;
# 1542
struct Cyc_Absyn_Exp*_tmp29A=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp29A;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp29B=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp29B;
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp29C;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1549
struct Cyc_Absyn_Exp*_tmp29D=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp29D;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp29E=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp29E;
struct Cyc_Absyn_Exp*_tmp29F=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp29F;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1556
struct Cyc_List_List*_tmp2A0=({struct Cyc_Absyn_Exp*_tmp2AF[2U];({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp2AF[0]=_tmp8AE;}),({
struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2AF[1]=_tmp8AD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2AF,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 1556
struct Cyc_List_List*xsizeargs=_tmp2A0;
# 1558
struct Cyc_Absyn_Exp*_tmp2A1=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp2A1;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp8AF,oneexp,0U);});{
# 1563
struct Cyc_Absyn_Exp*_tmp2A2=({struct Cyc_Absyn_Exp*_tmp8B0=xsize;Cyc_Absyn_and_exp(_tmp8B0,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp2A2;
struct Cyc_Absyn_Stmt*_tmp2A3=Cyc_Absyn_exp_stmt(({void*_tmp2AE=0U;({struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp8B1,_tag_fat(_tmp2AE,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp2A3;
struct Cyc_Absyn_Exp*_tmp2A4=({struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp8B2,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp2A4;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp2A5;
struct Cyc_Absyn_Exp*_tmp2A6=({struct Cyc_Absyn_Exp*_tmp8B3=deref_xcurr;Cyc_Absyn_assign_exp(_tmp8B3,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp2A6;
struct Cyc_Absyn_Stmt*_tmp2A7=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp2A7;
s=({struct Cyc_Absyn_Stmt*_tmp8B6=({struct Cyc_Absyn_Exp*_tmp8B5=comp_exp;struct Cyc_Absyn_Stmt*_tmp8B4=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp8B5,_tmp8B4,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp8B6,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8B8=({struct Cyc_Absyn_Decl*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));({void*_tmp8B7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->tag=0U,_tmp2A8->f1=z_vd;_tmp2A8;});_tmp2A9->r=_tmp8B7;}),_tmp2A9->loc=0U;_tmp2A9;});Cyc_Absyn_decl_stmt(_tmp8B8,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8BA=({struct Cyc_Absyn_Decl*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));({void*_tmp8B9=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->tag=0U,_tmp2AA->f1=y_vd;_tmp2AA;});_tmp2AB->r=_tmp8B9;}),_tmp2AB->loc=0U;_tmp2AB;});Cyc_Absyn_decl_stmt(_tmp8BA,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8BC=({struct Cyc_Absyn_Decl*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));({void*_tmp8BB=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->tag=0U,_tmp2AC->f1=x_vd;_tmp2AC;});_tmp2AD->r=_tmp8BB;}),_tmp2AD->loc=0U;_tmp2AD;});Cyc_Absyn_decl_stmt(_tmp8BC,s,0U);});
({void*_tmp8BD=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8BD;});}}}}}}}}}}
# 1517
;_pop_region(rgn2);}}
# 1589 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1593
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2B7=Cyc_Tcutil_compress(aggrtype);void*_stmttmp26=_tmp2B7;void*_tmp2B8=_stmttmp26;union Cyc_Absyn_AggrInfo _tmp2B9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f1)->tag == 20U){_LL1: _tmp2B9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2B9;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BD;_tmp7BD.tag=0U,({struct _fat_ptr _tmp8BE=({const char*_tmp2BF="expecting union but found ";_tag_fat(_tmp2BF,sizeof(char),27U);});_tmp7BD.f1=_tmp8BE;});_tmp7BD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7BC;_tmp7BC.tag=2U,_tmp7BC.f1=(void*)aggrtype;_tmp7BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BB;_tmp7BB.tag=0U,({
struct _fat_ptr _tmp8BF=({const char*_tmp2BE=" in check_tagged_union";_tag_fat(_tmp2BE,sizeof(char),23U);});_tmp7BB.f1=_tmp8BF;});_tmp7BB;});void*_tmp2BA[3U];_tmp2BA[0]=& _tmp2BB,_tmp2BA[1]=& _tmp2BC,_tmp2BA[2]=& _tmp2BD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2BA,sizeof(void*),3U));});}_LL0:;}{
# 1599
struct _tuple30 _tmp2C0=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp27=_tmp2C0;struct _tuple30 _tmp2C1=_stmttmp27;struct Cyc_Absyn_Exp*_tmp2C3;struct _tuple1*_tmp2C2;_LL6: _tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;_LL7: {struct _tuple1*temp=_tmp2C2;struct Cyc_Absyn_Exp*temp_exp=_tmp2C3;
struct Cyc_Absyn_Exp*_tmp2C4=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2C4;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2C5=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2C5;
struct Cyc_Absyn_Exp*_tmp2C6=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2C6;
struct Cyc_Absyn_Exp*_tmp2C7=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2C7;
struct Cyc_Absyn_Stmt*_tmp2C8=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2C8;
struct Cyc_Absyn_Stmt*_tmp2C9=({struct Cyc_Absyn_Exp*_tmp8C1=test_exp;struct Cyc_Absyn_Stmt*_tmp8C0=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8C1,_tmp8C0,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2C9;
void*_tmp2CA=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2CA;
struct Cyc_Absyn_Exp*_tmp2CB=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2CB;
struct Cyc_Absyn_Stmt*_tmp2CC=({struct _tuple1*_tmp8C4=temp;void*_tmp8C3=e1_p_type;struct Cyc_Absyn_Exp*_tmp8C2=e1_f;Cyc_Absyn_declare_stmt(_tmp8C4,_tmp8C3,_tmp8C2,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2CC;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1612
struct Cyc_Absyn_Exp*_tmp2CD=({struct Cyc_Absyn_Exp*_tmp8C5=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8C5,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2CD;
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2CE;
struct Cyc_Absyn_Exp*_tmp2CF=({struct Cyc_Absyn_Exp*_tmp8C6=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8C6,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2CF;
struct Cyc_Absyn_Stmt*_tmp2D0=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2D0;
struct Cyc_Absyn_Stmt*_tmp2D1=({struct Cyc_Absyn_Exp*_tmp8C8=test_exp;struct Cyc_Absyn_Stmt*_tmp8C7=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8C8,_tmp8C7,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D1;
struct Cyc_Absyn_Stmt*_tmp2D2=({struct _tuple1*_tmp8CB=temp;void*_tmp8CA=e1_c_type;struct Cyc_Absyn_Exp*_tmp8C9=e1;Cyc_Absyn_declare_stmt(_tmp8CB,_tmp8CA,_tmp8C9,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D2;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1622
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1625
void*_tmp2D3=Cyc_Tcutil_compress(t);void*_stmttmp28=_tmp2D3;void*_tmp2D4=_stmttmp28;union Cyc_Absyn_AggrInfo _tmp2D5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4)->f1)->tag == 20U){_LL1: _tmp2D5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2D5;
# 1627
struct Cyc_Absyn_Aggrdecl*_tmp2D6=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2D6;
({int _tmp8CC=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp8CC;});{
# 1630
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp7BF;_tmp7BF.tag=0U,_tmp7BF.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7BF;});struct Cyc_String_pa_PrintArg_struct _tmp2DB=({struct Cyc_String_pa_PrintArg_struct _tmp7BE;_tmp7BE.tag=0U,_tmp7BE.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7BE;});void*_tmp2D8[2U];_tmp2D8[0]=& _tmp2DA,_tmp2D8[1]=& _tmp2DB;({struct _fat_ptr _tmp8CD=({const char*_tmp2D9="_union_%s_%s";_tag_fat(_tmp2D9,sizeof(char),13U);});Cyc_aprintf(_tmp8CD,_tag_fat(_tmp2D8,sizeof(void*),2U));});});
({void*_tmp8CE=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));*_tmp2D7=str;_tmp2D7;}));*tagged_member_type=_tmp8CE;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1641
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1644
void*_tmp2DC=e->r;void*_stmttmp29=_tmp2DC;void*_tmp2DD=_stmttmp29;int*_tmp2E0;struct _fat_ptr*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;int*_tmp2E3;struct _fat_ptr*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E4;switch(*((int*)_tmp2DD)){case 14U: _LL1: _tmp2E4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DD)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp2E4;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C0;_tmp7C0.tag=0U,({struct _fat_ptr _tmp8CF=({const char*_tmp2E7="cast on lhs!";_tag_fat(_tmp2E7,sizeof(char),13U);});_tmp7C0.f1=_tmp8CF;});_tmp7C0;});void*_tmp2E5[1U];_tmp2E5[0]=& _tmp2E6;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp2E5,sizeof(void*),1U));});}case 21U: _LL3: _tmp2E1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1;_tmp2E2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DD)->f2;_tmp2E3=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DD)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2E1;struct _fat_ptr*f=_tmp2E2;int*is_read=_tmp2E3;
# 1647
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22U: _LL5: _tmp2DE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DD)->f1;_tmp2DF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DD)->f2;_tmp2E0=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DD)->f4;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp2DE;struct _fat_ptr*f=_tmp2DF;int*is_read=_tmp2E0;
# 1650
void*_tmp2E8=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp2A=_tmp2E8;void*_tmp2E9=_stmttmp2A;void*_tmp2EA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->tag == 3U){_LLA: _tmp2EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->f1).elt_type;_LLB: {void*et=_tmp2EA;
# 1652
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{_LLC: _LLD:
# 1654
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1656
 return 0;}_LL0:;}
# 1669 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1673
struct _tuple30 _tmp2EB=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2B=_tmp2EB;struct _tuple30 _tmp2EC=_stmttmp2B;struct Cyc_Absyn_Exp*_tmp2EE;struct _tuple1*_tmp2ED;_LL1: _tmp2ED=_tmp2EC.f1;_tmp2EE=_tmp2EC.f2;_LL2: {struct _tuple1*temp=_tmp2ED;struct Cyc_Absyn_Exp*temp_exp=_tmp2EE;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1682
struct Cyc_Absyn_Exp*_tmp2EF=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2EF;
s2=({struct Cyc_Absyn_Exp*_tmp8D1=test_exp;struct Cyc_Absyn_Stmt*_tmp8D0=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8D1,_tmp8D0,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1685
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8D4=temp;void*_tmp8D3=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8D2=
Cyc_Toc_push_address_exp(e1);
# 1685
Cyc_Absyn_declare_stmt(_tmp8D4,_tmp8D3,_tmp8D2,
# 1687
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1691
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C2;_tmp7C2.tag=0U,({struct _fat_ptr _tmp8D5=({const char*_tmp2F3="exp_to_c: no type for ";_tag_fat(_tmp2F3,sizeof(char),23U);});_tmp7C2.f1=_tmp8D5;});_tmp7C2;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7C1;_tmp7C1.tag=3U,_tmp7C1.f1=e;_tmp7C1;});void*_tmp2F0[2U];_tmp2F0[0]=& _tmp2F1,_tmp2F0[1]=& _tmp2F2;({unsigned _tmp8D6=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8D6,_tag_fat(_tmp2F0,sizeof(void*),2U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1696
int did_inserted_checks=0;
{void*_tmp2F4=e->annot;void*_stmttmp2C=_tmp2F4;void*_tmp2F5=_stmttmp2C;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F5)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2F5)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1702
{void*_tmp2F6=e->r;void*_stmttmp2D=_tmp2F6;void*_tmp2F7=_stmttmp2D;struct Cyc_Absyn_Stmt*_tmp2F8;struct _fat_ptr*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2F9;struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FB;int _tmp302;int _tmp301;struct Cyc_Absyn_Exp*_tmp300;void**_tmp2FF;struct Cyc_Absyn_Exp*_tmp2FE;int _tmp2FD;struct Cyc_Absyn_Datatypefield*_tmp305;struct Cyc_Absyn_Datatypedecl*_tmp304;struct Cyc_List_List*_tmp303;struct Cyc_Absyn_Aggrdecl*_tmp309;struct Cyc_List_List*_tmp308;struct Cyc_List_List*_tmp307;struct _tuple1**_tmp306;struct Cyc_List_List*_tmp30B;void*_tmp30A;int _tmp30E;void*_tmp30D;struct Cyc_Absyn_Exp*_tmp30C;int _tmp312;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Exp*_tmp310;struct Cyc_Absyn_Vardecl*_tmp30F;struct Cyc_List_List*_tmp313;struct Cyc_List_List*_tmp314;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Absyn_Exp*_tmp315;struct Cyc_Absyn_Exp*_tmp317;int _tmp31B;int _tmp31A;struct _fat_ptr*_tmp319;struct Cyc_Absyn_Exp*_tmp318;int _tmp31F;int _tmp31E;struct _fat_ptr*_tmp31D;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_List_List*_tmp321;void*_tmp320;void*_tmp322;struct Cyc_Absyn_Exp*_tmp323;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*_tmp326;enum Cyc_Absyn_Coercion _tmp32A;int _tmp329;struct Cyc_Absyn_Exp*_tmp328;void**_tmp327;struct Cyc_List_List*_tmp32C;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Exp*_tmp32D;int _tmp32F;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_VarargInfo*_tmp334;struct Cyc_List_List*_tmp333;int _tmp332;struct Cyc_List_List*_tmp331;struct Cyc_Absyn_Exp*_tmp330;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_Absyn_Exp*_tmp339;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Core_Opt*_tmp341;struct Cyc_Absyn_Exp*_tmp340;enum Cyc_Absyn_Incrementor _tmp344;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_List_List*_tmp346;enum Cyc_Absyn_Primop _tmp345;struct Cyc_Absyn_Exp*_tmp347;switch(*((int*)_tmp2F7)){case 2U: _LL8: _LL9:
# 1704
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1711
struct Cyc_Absyn_Exp*_tmp348=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp348;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8D7=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp8D7;});else{
# 1716
({void*_tmp8D9=({struct Cyc_Absyn_Exp*_tmp349[3U];_tmp349[0]=zero,_tmp349[1]=zero,_tmp349[2]=zero;({struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8D8,_tag_fat(_tmp349,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8D9;});}}else{
# 1718
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1720
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp347=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp347;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3U: _LL12: _tmp345=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp346=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL13: {enum Cyc_Absyn_Primop p=_tmp345;struct Cyc_List_List*es=_tmp346;
# 1726
struct Cyc_List_List*_tmp34A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp34A;
# 1728
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp34B=p;switch(_tmp34B){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1731
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp34C=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_stmttmp2E=_tmp34C;void*_tmp34D=_stmttmp2E;void*_tmp351;void*_tmp350;void*_tmp34F;void*_tmp34E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D)->tag == 3U){_LL76: _tmp34E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D)->f1).elt_type;_tmp34F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D)->f1).ptr_atts).nullable;_tmp350=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D)->f1).ptr_atts).bounds;_tmp351=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D)->f1).ptr_atts).zero_term;_LL77: {void*elt_type=_tmp34E;void*nbl=_tmp34F;void*bound=_tmp350;void*zt=_tmp351;
# 1734
struct Cyc_Absyn_Exp*_tmp352=({void*_tmp8DA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8DA,bound);});struct Cyc_Absyn_Exp*eopt=_tmp352;
if(eopt == 0)
# 1737
({void*_tmp8DD=({struct Cyc_Absyn_Exp*_tmp353[2U];_tmp353[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp353[1]=_tmp8DB;});({struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp8DC,_tag_fat(_tmp353,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1737
e->r=_tmp8DD;});else{
# 1740
struct Cyc_Absyn_Exp*_tmp354=eopt;struct Cyc_Absyn_Exp*e2=_tmp354;
# 1742
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1746
({void*_tmp8DF=({struct Cyc_Absyn_Exp*_tmp355[2U];_tmp355[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp355[1]=e2;({struct Cyc_Absyn_Exp*_tmp8DE=function_e;Cyc_Toc_fncall_exp_r(_tmp8DE,_tag_fat(_tmp355,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8DF;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(e2))
({void*_tmp356=0U;({unsigned _tmp8E1=e->loc;struct _fat_ptr _tmp8E0=({const char*_tmp357="can't calculate numelts";_tag_fat(_tmp357,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8E1,_tmp8E0,_tag_fat(_tmp356,sizeof(void*),0U));});});
# 1751
({void*_tmp8E4=({struct Cyc_Absyn_Exp*_tmp8E3=arg;struct Cyc_Absyn_Exp*_tmp8E2=e2;Cyc_Toc_conditional_exp_r(_tmp8E3,_tmp8E2,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8E4;});}else{
# 1753
e->r=e2->r;goto _LL75;}}}
# 1756
goto _LL75;}}else{_LL78: _LL79:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp359=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C4;_tmp7C4.tag=0U,({struct _fat_ptr _tmp8E5=({const char*_tmp35B="numelts primop applied to non-pointer ";_tag_fat(_tmp35B,sizeof(char),39U);});_tmp7C4.f1=_tmp8E5;});_tmp7C4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp35A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C3;_tmp7C3.tag=2U,_tmp7C3.f1=(void*)_check_null(arg->topt);_tmp7C3;});void*_tmp358[2U];_tmp358[0]=& _tmp359,_tmp358[1]=& _tmp35A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp358,sizeof(void*),2U));});}_LL75:;}
# 1760
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1765
 if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp35C="can't do pointer arithmetic at top-level";_tag_fat(_tmp35C,sizeof(char),41U);}));
{void*_tmp35D=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp2F=_tmp35D;void*_tmp35E=_stmttmp2F;void*_tmp361;void*_tmp360;void*_tmp35F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E)->tag == 3U){_LL7B: _tmp35F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E)->f1).elt_type;_tmp360=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E)->f1).ptr_atts).bounds;_tmp361=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E)->f1).ptr_atts).zero_term;_LL7C: {void*elt_type=_tmp35F;void*b=_tmp360;void*zt=_tmp361;
# 1769
struct Cyc_Absyn_Exp*_tmp362=({void*_tmp8E6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E6,b);});struct Cyc_Absyn_Exp*eopt=_tmp362;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp8E9=({struct Cyc_Absyn_Exp*_tmp363[3U];_tmp363[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp363[1]=_tmp8E7;}),_tmp363[2]=e2;({struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8E8,_tag_fat(_tmp363,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1773
e->r=_tmp8E9;});else{
# 1775
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp8EB=({struct Cyc_Absyn_Exp*_tmp364[3U];_tmp364[0]=e1,_tmp364[1]=eopt,_tmp364[2]=e2;({struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8EA,_tag_fat(_tmp364,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8EB;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1780
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1785
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd)){
({void*_tmp8ED=({struct Cyc_Absyn_Exp*_tmp8EC=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8EC,Cyc_Toc_curr_sp);});e1->r=_tmp8ED;});
({void*_tmp8EF=({struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8EE,Cyc_Toc_curr_sp);});e2->r=_tmp8EF;});
({void*_tmp8F1=({void*_tmp8F0=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8F0;});e1->topt=_tmp8F1;});
({void*_tmp8F3=({struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8F2,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1793
e->r=_tmp8F3;});}else{
# 1796
({void*_tmp8F7=({struct Cyc_Absyn_Exp*_tmp365[3U];_tmp365[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp365[1]=_tmp8F5;}),({
struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp365[2]=_tmp8F4;});({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8F6,_tag_fat(_tmp365,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1796
e->r=_tmp8F7;});}}
# 1800
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A: goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C: goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E: goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70: goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1803
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(old_types))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp366=({void*_tmp8F8=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8F8,Cyc_Toc_mt_tq);});void*t=_tmp366;
({void*_tmp8FB=({void*_tmp8FA=t;Cyc_Toc_cast_it_r(_tmp8FA,({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8F9,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8FB;});
e1->topt=t;}
# 1813
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp367=({void*_tmp8FC=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8FC,Cyc_Toc_mt_tq);});void*t=_tmp367;
({void*_tmp8FF=({void*_tmp8FE=t;Cyc_Toc_cast_it_r(_tmp8FE,({struct Cyc_Absyn_Exp*_tmp8FD=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8FD,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8FF;});
e2->topt=t;}
# 1818
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1821
goto _LL7;}case 5U: _LL14: _tmp343=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp344=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL15: {struct Cyc_Absyn_Exp*e2=_tmp343;enum Cyc_Absyn_Incrementor incr=_tmp344;
# 1823
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1832 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp383="increment";_tag_fat(_tmp383,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp368="decrement";_tag_fat(_tmp368,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C8;_tmp7C8.tag=0U,({struct _fat_ptr _tmp900=({const char*_tmp370="in-place ";_tag_fat(_tmp370,sizeof(char),10U);});_tmp7C8.f1=_tmp900;});_tmp7C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C7;_tmp7C7.tag=0U,_tmp7C7.f1=incr_str;_tmp7C7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C6;_tmp7C6.tag=0U,({struct _fat_ptr _tmp901=({const char*_tmp36F=" is not supported when ";_tag_fat(_tmp36F,sizeof(char),24U);});_tmp7C6.f1=_tmp901;});_tmp7C6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C5;_tmp7C5.tag=0U,({
struct _fat_ptr _tmp902=({const char*_tmp36E="dereferencing a zero-terminated pointer";_tag_fat(_tmp36E,sizeof(char),40U);});_tmp7C5.f1=_tmp902;});_tmp7C5;});void*_tmp369[4U];_tmp369[0]=& _tmp36A,_tmp369[1]=& _tmp36B,_tmp369[2]=& _tmp36C,_tmp369[3]=& _tmp36D;({unsigned _tmp903=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp903,_tag_fat(_tmp369,sizeof(void*),4U));});});
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp371=incr;switch(_tmp371){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp373=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CB;_tmp7CB.tag=0U,({struct _fat_ptr _tmp904=({const char*_tmp377="in-place post";_tag_fat(_tmp377,sizeof(char),14U);});_tmp7CB.f1=_tmp904;});_tmp7CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp374=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CA;_tmp7CA.tag=0U,_tmp7CA.f1=incr_str;_tmp7CA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C9;_tmp7C9.tag=0U,({
struct _fat_ptr _tmp905=({const char*_tmp376=" is not supported on @tagged union members";_tag_fat(_tmp376,sizeof(char),43U);});_tmp7C9.f1=_tmp905;});_tmp7C9;});void*_tmp372[3U];_tmp372[0]=& _tmp373,_tmp372[1]=& _tmp374,_tmp372[2]=& _tmp375;({unsigned _tmp906=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp906,_tag_fat(_tmp372,sizeof(void*),3U));});});}_LL7F:;}
# 1850
({void*_tmp908=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->tag=4U,_tmp379->f1=e2,({struct Cyc_Core_Opt*_tmp907=({struct Cyc_Core_Opt*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->v=(void*)op;_tmp378;});_tmp379->f2=_tmp907;}),_tmp379->f3=one;_tmp379;});e->r=_tmp908;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1854
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1859
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)incr == (int)1U ||(int)incr == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1865
if((int)incr == (int)2U ||(int)incr == (int)3U)
change=-1;
({void*_tmp90D=({struct Cyc_Absyn_Exp*_tmp37A[3U];({struct Cyc_Absyn_Exp*_tmp90B=Cyc_Toc_push_address_exp(e2);_tmp37A[0]=_tmp90B;}),({
struct Cyc_Absyn_Exp*_tmp90A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp37A[1]=_tmp90A;}),({
struct Cyc_Absyn_Exp*_tmp909=Cyc_Absyn_signed_int_exp(change,0U);_tmp37A[2]=_tmp909;});({struct Cyc_Absyn_Exp*_tmp90C=fn_e;Cyc_Toc_fncall_exp_r(_tmp90C,_tag_fat(_tmp37A,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1867
e->r=_tmp90D;});}else{
# 1870
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1876
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp37B=Cyc_Toc_temp_var();struct _tuple1*x=_tmp37B;
void*_tmp37C=({void*_tmp90E=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp90E,Cyc_Toc_mt_tq);});void*t=_tmp37C;
struct Cyc_Absyn_Exp*_tmp37D=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp37D;
struct Cyc_Absyn_Exp*_tmp37E=({struct Cyc_Absyn_Exp*_tmp90F=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp90F,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1881
struct Cyc_Absyn_Exp*testexp=_tmp37E;
# 1883
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp911=({struct Cyc_Absyn_Exp*_tmp910=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp910,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1884
Cyc_Absyn_and_exp(_tmp911,testexp,0U);});{
# 1886
struct Cyc_Absyn_Stmt*_tmp37F=({struct Cyc_Absyn_Exp*_tmp914=testexp;struct Cyc_Absyn_Stmt*_tmp913=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1886
Cyc_Absyn_ifthenelse_stmt(_tmp914,_tmp913,
# 1888
Cyc_Absyn_exp_stmt(({void*_tmp380=0U;({struct Cyc_Absyn_Exp*_tmp912=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp912,_tag_fat(_tmp380,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 1886
struct Cyc_Absyn_Stmt*s=_tmp37F;
# 1889
s=({struct Cyc_Absyn_Stmt*_tmp915=s;Cyc_Absyn_seq_stmt(_tmp915,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp916=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp916;});}}else{
# 1893
struct Cyc_Toc_functionSet*_tmp381=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp381;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp91A=({struct Cyc_Absyn_Exp*_tmp382[2U];({struct Cyc_Absyn_Exp*_tmp918=Cyc_Toc_push_address_exp(e2);_tmp382[0]=_tmp918;}),({struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_signed_int_exp(1,0U);_tmp382[1]=_tmp917;});({struct Cyc_Absyn_Exp*_tmp919=fn_e;Cyc_Toc_fncall_exp_r(_tmp919,_tag_fat(_tmp382,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp91A;});}}else{
# 1897
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1901
goto _LL7;}}case 4U: _LL16: _tmp340=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp341=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp342=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp340;struct Cyc_Core_Opt*popt=_tmp341;struct Cyc_Absyn_Exp*e2=_tmp342;
# 1920 "toc.cyc"
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp91B=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp91B;});
# 1931
goto _LL7;}{
# 1933
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
# 1939
return;}{
# 1943
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1949
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp384=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp30=_tmp384;enum Cyc_Absyn_Primop _tmp385=_stmttmp30;switch(_tmp385){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=e2;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL86;default: _LL8B: _LL8C:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp387=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CC;_tmp7CC.tag=0U,({struct _fat_ptr _tmp91C=({const char*_tmp388="bad t ? pointer arithmetic";_tag_fat(_tmp388,sizeof(char),27U);});_tmp7CC.f1=_tmp91C;});_tmp7CC;});void*_tmp386[1U];_tmp386[0]=& _tmp387;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp386,sizeof(void*),1U));});}_LL86:;}
# 1959
done=1;{
# 1961
struct Cyc_Absyn_Exp*_tmp389=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp389;
({void*_tmp920=({struct Cyc_Absyn_Exp*_tmp38A[3U];({struct Cyc_Absyn_Exp*_tmp91E=Cyc_Toc_push_address_exp(e1);_tmp38A[0]=_tmp91E;}),({
struct Cyc_Absyn_Exp*_tmp91D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp38A[1]=_tmp91D;}),_tmp38A[2]=change;({struct Cyc_Absyn_Exp*_tmp91F=fn_e;Cyc_Toc_fncall_exp_r(_tmp91F,_tag_fat(_tmp38A,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1962
e->r=_tmp920;});}}else{
# 1965
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1968
enum Cyc_Absyn_Primop _tmp38B=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp31=_tmp38B;enum Cyc_Absyn_Primop _tmp38C=_stmttmp31;if(_tmp38C == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 1970
 done=1;
({void*_tmp922=({struct Cyc_Absyn_Exp*_tmp38D[2U];_tmp38D[0]=e1,_tmp38D[1]=e2;({struct Cyc_Absyn_Exp*_tmp921=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp921,_tag_fat(_tmp38D,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp922;});
goto _LL8D;}else{_LL90: _LL91:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CD;_tmp7CD.tag=0U,({struct _fat_ptr _tmp923=({const char*_tmp390="bad zero-terminated pointer arithmetic";_tag_fat(_tmp390,sizeof(char),39U);});_tmp7CD.f1=_tmp923;});_tmp7CD;});void*_tmp38E[1U];_tmp38E[0]=& _tmp38F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp38E,sizeof(void*),1U));});}_LL8D:;}}}
# 1977
if(!done){
# 1979
if(e1_poly)
({void*_tmp925=({void*_tmp924=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp924,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp925;});
# 1985
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp926=e1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple32*),struct _tuple32*f_env))Cyc_Toc_lvalue_assign)(_tmp926,0,Cyc_Toc_assignop_lvalue,({struct _tuple32*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->f1=popt,_tmp391->f2=e2;_tmp391;}));});
e->r=e1->r;}}
# 1990
goto _LL7;}}}}case 6U: _LL18: _tmp33D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp33E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp33F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp33D;struct Cyc_Absyn_Exp*e2=_tmp33E;struct Cyc_Absyn_Exp*e3=_tmp33F;
# 1992
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7U: _LL1A: _tmp33B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp33C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp33B;struct Cyc_Absyn_Exp*e2=_tmp33C;
_tmp339=e1;_tmp33A=e2;goto _LL1D;}case 8U: _LL1C: _tmp339=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp33A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL1D: {struct Cyc_Absyn_Exp*e1=_tmp339;struct Cyc_Absyn_Exp*e2=_tmp33A;
_tmp337=e1;_tmp338=e2;goto _LL1F;}case 9U: _LL1E: _tmp337=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp338=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp337;struct Cyc_Absyn_Exp*e2=_tmp338;
# 1999
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3 == 0){_LL20: _tmp335=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp336=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp335;struct Cyc_List_List*es=_tmp336;
# 2003
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2008
({void*_tmp92A=({void*_tmp929=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp929,({struct Cyc_Absyn_Exp*_tmp392[1U];({struct Cyc_Absyn_Exp*_tmp927=
Cyc_Absyn_copy_exp(e1);_tmp392[0]=_tmp927;});({struct Cyc_Absyn_Exp*_tmp928=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp928,_tag_fat(_tmp392,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2008
e1->r=_tmp92A;});
# 2010
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_LL22: _tmp330=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp331=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp332=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3)->num_varargs;_tmp333=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3)->injectors;_tmp334=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3)->vai;_LL23: {struct Cyc_Absyn_Exp*e1=_tmp330;struct Cyc_List_List*es=_tmp331;int num_varargs=_tmp332;struct Cyc_List_List*injectors=_tmp333;struct Cyc_Absyn_VarargInfo*vai=_tmp334;
# 2021 "toc.cyc"
struct _RegionHandle _tmp393=_new_region("r");struct _RegionHandle*r=& _tmp393;_push_region(r);{
struct _tuple30 _tmp394=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp32=_tmp394;struct _tuple30 _tmp395=_stmttmp32;struct Cyc_Absyn_Exp*_tmp397;struct _tuple1*_tmp396;_LL93: _tmp396=_tmp395.f1;_tmp397=_tmp395.f2;_LL94: {struct _tuple1*argv=_tmp396;struct Cyc_Absyn_Exp*argvexp=_tmp397;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2028
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
int num_normargs=num_args - num_varargs;
# 2032
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp398->tl=new_args;_tmp398;});}}
# 2037
new_args=({struct Cyc_List_List*_tmp39A=_cycalloc(sizeof(*_tmp39A));({struct Cyc_Absyn_Exp*_tmp92D=({struct Cyc_Absyn_Exp*_tmp399[3U];_tmp399[0]=argvexp,({
# 2039
struct Cyc_Absyn_Exp*_tmp92B=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp399[1]=_tmp92B;}),_tmp399[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp92C=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp92C,_tag_fat(_tmp399,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2037
_tmp39A->hd=_tmp92D;}),_tmp39A->tl=new_args;_tmp39A;});
# 2042
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2044
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2049
({void*_tmp931=({void*_tmp930=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp930,({struct Cyc_Absyn_Exp*_tmp39B[1U];({struct Cyc_Absyn_Exp*_tmp92E=
Cyc_Absyn_copy_exp(e1);_tmp39B[0]=_tmp92E;});({struct Cyc_Absyn_Exp*_tmp92F=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp92F,_tag_fat(_tmp39B,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2049
e1->r=_tmp931;});{
# 2051
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2054
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp39C=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp33=_tmp39C;void*_tmp39D=_stmttmp33;struct Cyc_Absyn_Datatypedecl*_tmp39E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->f1)->f1).KnownDatatype).tag == 2){_LL96: _tmp39E=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->f1)->f1).KnownDatatype).val;_LL97: {struct Cyc_Absyn_Datatypedecl*x=_tmp39E;
tud=x;goto _LL95;}}else{goto _LL98;}}else{goto _LL98;}}else{_LL98: _LL99:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CE;_tmp7CE.tag=0U,({struct _fat_ptr _tmp932=({const char*_tmp3A1="unknown datatype in vararg with inject";_tag_fat(_tmp3A1,sizeof(char),39U);});_tmp7CE.f1=_tmp932;});_tmp7CE;});void*_tmp39F[1U];_tmp39F[0]=& _tmp3A0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp39F,sizeof(void*),1U));});}_LL95:;}{
# 2060
struct _fat_ptr vs=({unsigned _tmp3AD=(unsigned)num_varargs;struct _tuple1**_tmp3AC=({struct _RegionHandle*_tmp933=r;_region_malloc(_tmp933,_check_times(_tmp3AD,sizeof(struct _tuple1*)));});({{unsigned _tmp7CF=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp7CF;++ i){({struct _tuple1*_tmp934=Cyc_Toc_temp_var();_tmp3AC[i]=_tmp934;});}}0;});_tag_fat(_tmp3AC,sizeof(struct _tuple1*),_tmp3AD);});
# 2062
if(num_varargs != 0){
# 2066
struct Cyc_List_List*_tmp3A2=0;struct Cyc_List_List*array_args=_tmp3A2;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_Absyn_Exp*_tmp935=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp3A3->hd=_tmp935;}),_tmp3A3->tl=array_args;_tmp3A3;});}}
s=({struct _tuple1*_tmp938=argv;void*_tmp937=arr_type;struct Cyc_Absyn_Exp*_tmp936=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp938,_tmp937,_tmp936,s,0U);});
# 2071
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)- 1;
for(0;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp3A4=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp34=_tmp3A4;struct Cyc_Absyn_Datatypefield*_tmp3A5=_stmttmp34;struct Cyc_List_List*_tmp3A7;struct _tuple1*_tmp3A6;_LL9B: _tmp3A6=_tmp3A5->name;_tmp3A7=_tmp3A5->typs;_LL9C: {struct _tuple1*qv=_tmp3A6;struct Cyc_List_List*tqts=_tmp3A7;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple13*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2085
struct _tuple1*_tmp3A8=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp3A8;
struct Cyc_List_List*_tmp3A9=({struct _tuple20*_tmp3AB[2U];({struct _tuple20*_tmp93C=({struct _fat_ptr*_tmp93B=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp93B,Cyc_Toc_datatype_tag(tud,qv));});_tmp3AB[0]=_tmp93C;}),({
struct _tuple20*_tmp93A=({struct _fat_ptr*_tmp939=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp939,arg);});_tmp3AB[1]=_tmp93A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3AB,sizeof(struct _tuple20*),2U));});
# 2086
struct Cyc_List_List*dles=_tmp3A9;
# 2088
s=({struct _tuple1*_tmp93F=var;void*_tmp93E=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp93D=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->tag=29U,_tmp3AA->f1=tdn,_tmp3AA->f2=0,_tmp3AA->f3=dles,_tmp3AA->f4=0;_tmp3AA;}),0U);
# 2088
Cyc_Absyn_declare_stmt(_tmp93F,_tmp93E,_tmp93D,s,0U);});}}}}}else{
# 2096
s=({struct _tuple1*_tmp942=argv;void*_tmp941=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp940=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp942,_tmp941,_tmp940,s,0U);});}}}else{
# 2101
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3AE->tl=array_args;_tmp3AE;});}{
# 2107
struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp3AF;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2110
s=({struct _tuple1*_tmp945=argv;void*_tmp944=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp943=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp945,_tmp944,_tmp943,s,0U);});}}
# 2113
({void*_tmp946=Cyc_Toc_stmt_exp_r(s);e->r=_tmp946;});}}}}
# 2115
_npop_handler(0U);goto _LL7;
# 2021
;_pop_region(r);}}case 11U: _LL24: _tmp32E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp32F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL25: {struct Cyc_Absyn_Exp*e1=_tmp32E;int b=_tmp32F;
# 2118
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp949=({void*_tmp948=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp948,({struct Cyc_Absyn_Exp*_tmp3B0[1U];_tmp3B0[0]=e1;({struct Cyc_Absyn_Exp*_tmp947=fn_e;Cyc_Toc_fncall_exp_dl(_tmp947,_tag_fat(_tmp3B0,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp949;});
goto _LL7;}}case 12U: _LL26: _tmp32D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp32D;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13U: _LL28: _tmp32B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp32C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL29: {struct Cyc_Absyn_Exp*e1=_tmp32B;struct Cyc_List_List*ts=_tmp32C;
# 2124
Cyc_Toc_exp_to_c(nv,e1);
# 2133 "toc.cyc"
for(0;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp3B1=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp3B1;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp3B2=Cyc_Tcutil_compress((void*)ts->hd);void*_stmttmp35=_tmp3B2;void*_tmp3B3=_stmttmp35;switch(*((int*)_tmp3B3)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B3)->f1)->tag == 18U){_LLA0: _LLA1:
 continue;}else{goto _LLA2;}default: _LLA2: _LLA3:
# 2140
({void*_tmp94B=({void*_tmp94A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp94A,e1,0U);})->r;e->r=_tmp94B;});
goto _LL9D;}_LL9D:;}
# 2143
break;}}
# 2146
goto _LL7;}case 14U: _LL2A: _tmp327=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp328=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp329=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp32A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL2B: {void**t=_tmp327;struct Cyc_Absyn_Exp*e1=_tmp328;int user_inserted=_tmp329;enum Cyc_Absyn_Coercion coercion=_tmp32A;
# 2148
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2155
{struct _tuple33 _tmp3B4=({struct _tuple33 _tmp7D1;({void*_tmp94D=Cyc_Tcutil_compress(old_t2);_tmp7D1.f1=_tmp94D;}),({void*_tmp94C=Cyc_Tcutil_compress(new_typ);_tmp7D1.f2=_tmp94C;});_tmp7D1;});struct _tuple33 _stmttmp36=_tmp3B4;struct _tuple33 _tmp3B5=_stmttmp36;struct Cyc_Absyn_PtrInfo _tmp3B6;struct Cyc_Absyn_PtrInfo _tmp3B8;struct Cyc_Absyn_PtrInfo _tmp3B7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5.f1)->tag == 3U)switch(*((int*)_tmp3B5.f2)){case 3U: _LLA5: _tmp3B7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5.f1)->f1;_tmp3B8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5.f2)->f1;_LLA6: {struct Cyc_Absyn_PtrInfo p1=_tmp3B7;struct Cyc_Absyn_PtrInfo p2=_tmp3B8;
# 2157
struct Cyc_Absyn_Exp*_tmp3B9=({void*_tmp94E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp94E,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp3B9;
struct Cyc_Absyn_Exp*_tmp3BA=({void*_tmp94F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp94F,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp3BA;
int _tmp3BB=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp3BB;
int _tmp3BC=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp3BC;
{struct _tuple34 _tmp3BD=({struct _tuple34 _tmp7D0;_tmp7D0.f1=b1,_tmp7D0.f2=b2;_tmp7D0;});struct _tuple34 _stmttmp37=_tmp3BD;struct _tuple34 _tmp3BE=_stmttmp37;if(_tmp3BE.f1 != 0){if(_tmp3BE.f2 != 0){_LLAC: _LLAD:
# 2164
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp952=({void*_tmp951=*t;Cyc_Toc_cast_it_r(_tmp951,({struct Cyc_Absyn_Exp*_tmp3BF[1U];_tmp3BF[0]=e1;({struct Cyc_Absyn_Exp*_tmp950=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp950,_tag_fat(_tmp3BF,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp952;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2173
goto _LLAB;}else{_LLAE: _LLAF: {
# 2175
struct Cyc_Absyn_Exp*_tmp3C0=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp3C0;
struct _tuple14 _tmp3C1=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple14 _stmttmp38=_tmp3C1;struct _tuple14 _tmp3C2=_stmttmp38;int _tmp3C4;unsigned _tmp3C3;_LLB5: _tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;_LLB6: {unsigned i=_tmp3C3;int valid=_tmp3C4;
if(Cyc_Toc_is_toplevel(nv)){
# 2181
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp953=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp953,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp954=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp954;});}else{
# 2186
if(zt1){
# 2191
struct _tuple30 _tmp3C5=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp39=_tmp3C5;struct _tuple30 _tmp3C6=_stmttmp39;struct Cyc_Absyn_Exp*_tmp3C8;struct _tuple1*_tmp3C7;_LLB8: _tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C6.f2;_LLB9: {struct _tuple1*x=_tmp3C7;struct Cyc_Absyn_Exp*x_exp=_tmp3C8;
struct Cyc_Absyn_Exp*arg3;
# 2195
{void*_tmp3C9=e1->r;void*_stmttmp3A=_tmp3C9;void*_tmp3CA=_stmttmp3A;struct Cyc_Absyn_Vardecl*_tmp3CB;struct Cyc_Absyn_Vardecl*_tmp3CC;switch(*((int*)_tmp3CA)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1).Wstring_c).tag){case 8U: _LLBB: _LLBC:
 arg3=e2;goto _LLBA;case 9U: _LLBD: _LLBE:
 arg3=e2;goto _LLBA;default: goto _LLC3;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)){case 1U: _LLBF: _tmp3CC=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)->f1;_LLC0: {struct Cyc_Absyn_Vardecl*vd=_tmp3CC;
_tmp3CB=vd;goto _LLC2;}case 4U: _LLC1: _tmp3CB=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_tmp3CB;
# 2200
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC4;
arg3=e2;
goto _LLBA;}default: goto _LLC3;}default: _LLC3: _LLC4:
# 2207
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2210
arg3=({struct Cyc_Absyn_Exp*_tmp3CD[2U];({
struct Cyc_Absyn_Exp*_tmp956=({void*_tmp955=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp955,x_exp);});_tmp3CD[0]=_tmp956;}),_tmp3CD[1]=e2;({struct Cyc_Absyn_Exp*_tmp957=
# 2210
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp957,_tag_fat(_tmp3CD,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2212
goto _LLBA;}_LLBA:;}
# 2214
if(!zt2 && !(p2.elt_tq).real_const)
# 2217
arg3=({struct Cyc_Absyn_Exp*_tmp958=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp958,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2219
struct Cyc_Absyn_Exp*_tmp3CE=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3CE;
struct Cyc_Absyn_Exp*_tmp3CF=({struct Cyc_Absyn_Exp*_tmp3D1[3U];_tmp3D1[0]=x_exp,_tmp3D1[1]=arg2,_tmp3D1[2]=arg3;({struct Cyc_Absyn_Exp*_tmp959=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp959,_tag_fat(_tmp3D1,sizeof(struct Cyc_Absyn_Exp*),3U));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3CF;
struct Cyc_Absyn_Stmt*_tmp3D0=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3D0;
s=({struct _tuple1*_tmp95C=x;void*_tmp95B=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp95A=e1;Cyc_Absyn_declare_stmt(_tmp95C,_tmp95B,_tmp95A,s,0U);});
({void*_tmp95D=Cyc_Toc_stmt_exp_r(s);e->r=_tmp95D;});}}}else{
# 2226
({void*_tmp960=({struct Cyc_Absyn_Exp*_tmp3D2[3U];_tmp3D2[0]=e1,({
# 2228
struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3D2[1]=_tmp95E;}),_tmp3D2[2]=e2;({struct Cyc_Absyn_Exp*_tmp95F=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp95F,_tag_fat(_tmp3D2,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2226
e->r=_tmp960;});}}
# 2231
goto _LLAB;}}}}else{if(_tmp3BE.f2 != 0){_LLB0: _LLB1: {
# 2241 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3D3=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3D3;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp961=b2;Cyc_Absyn_add_exp(_tmp961,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2248
struct Cyc_Absyn_Exp*_tmp3D4=({struct Cyc_Absyn_Exp*_tmp3D6[3U];_tmp3D6[0]=e1,({
# 2250
struct Cyc_Absyn_Exp*_tmp962=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3D6[1]=_tmp962;}),_tmp3D6[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp963=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp963,_tag_fat(_tmp3D6,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2248
struct Cyc_Absyn_Exp*ptr_exp=_tmp3D4;
# 2252
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp966=({struct Cyc_Absyn_Exp*_tmp3D5[1U];({struct Cyc_Absyn_Exp*_tmp964=Cyc_Absyn_copy_exp(ptr_exp);_tmp3D5[0]=_tmp964;});({struct Cyc_Absyn_Exp*_tmp965=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp965,_tag_fat(_tmp3D5,sizeof(struct Cyc_Absyn_Exp*),1U));});});ptr_exp->r=_tmp966;});
({void*_tmp967=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp967;});
goto _LLAB;}}}else{_LLB2: _LLB3:
# 2260
 if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp3D7="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3D7,sizeof(char),70U);}));
({void*_tmp96B=({struct Cyc_Absyn_Exp*_tmp3D8[3U];_tmp3D8[0]=e1,({
struct Cyc_Absyn_Exp*_tmp969=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3D8[1]=_tmp969;}),({
struct Cyc_Absyn_Exp*_tmp968=Cyc_Absyn_uint_exp(1U,0U);_tmp3D8[2]=_tmp968;});({struct Cyc_Absyn_Exp*_tmp96A=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp96A,_tag_fat(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2263
e->r=_tmp96B;});}
# 2267
goto _LLAB;}}_LLAB:;}
# 2269
goto _LLA4;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5.f2)->f1)->tag == 1U){_LLA7: _tmp3B6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5.f1)->f1;_LLA8: {struct Cyc_Absyn_PtrInfo p1=_tmp3B6;
# 2271
{struct Cyc_Absyn_Exp*_tmp3D9=({void*_tmp96C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp96C,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3B=_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA=_stmttmp3B;if(_tmp3DA == 0){_LLC6: _LLC7:
# 2273
({void*_tmp96E=({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp96D,Cyc_Toc_curr_sp);});e1->r=_tmp96E;});
e1->topt=new_typ_c;
goto _LLC5;}else{_LLC8: _LLC9:
 goto _LLC5;}_LLC5:;}
# 2278
goto _LLA4;}}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9: _LLAA:
# 2280
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLA4;}_LLA4:;}
# 2284
goto _LL7;}case 15U: _LL2C: _tmp326=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL2D: {struct Cyc_Absyn_Exp*e1=_tmp326;
# 2287
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2293
({void*_tmp970=({void*_tmp96F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp96F,e1);});e->r=_tmp970;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2297
({void*_tmp972=({void*_tmp971=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp971,e1);});e->r=_tmp972;});}
# 2299
goto _LL7;}case 16U: _LL2E: _tmp324=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp325=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL2F: {struct Cyc_Absyn_Exp*rgnopt=_tmp324;struct Cyc_Absyn_Exp*e1=_tmp325;
# 2306
Cyc_Toc_exp_to_c(nv,e1);{
# 2308
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple30 _tmp3DB=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3C=_tmp3DB;struct _tuple30 _tmp3DC=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp3DE;struct _tuple1*_tmp3DD;_LLCB: _tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_LLCC: {struct _tuple1*x=_tmp3DD;struct Cyc_Absyn_Exp*xexp=_tmp3DE;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3DF=Cyc_Tcutil_compress(elt_typ);void*_stmttmp3D=_tmp3DF;void*_tmp3E0=_stmttmp3D;void*_tmp3E1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E0)->tag == 4U){_LLCE: _tmp3E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E0)->f1).elt_type;_LLCF: {void*t2=_tmp3E1;
# 2313
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCD;}}else{_LLD0: _LLD1:
# 2317
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCD;}_LLCD:;}{
# 2320
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3E2=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3E2;
struct Cyc_Absyn_Exp*_tmp3E3=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3E3;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3E4[2U];_tmp3E4[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3E4[1]=_tmp973;});({struct Cyc_Absyn_Exp*_tmp974=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp974,_tag_fat(_tmp3E4,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2327
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2330
struct Cyc_Absyn_Exp*_tmp3E5=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3E5;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2334
if(rgnopt == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2337
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2340
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3E6[3U];({
struct Cyc_Absyn_Exp*_tmp977=Cyc_Absyn_copy_exp(xexp);_tmp3E6[0]=_tmp977;}),({
struct Cyc_Absyn_Exp*_tmp976=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3E6[1]=_tmp976;}),
(unsigned)lenexp?_tmp3E6[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp975=Cyc_Absyn_uint_exp(1U,0U);_tmp3E6[2]=_tmp975;});({struct Cyc_Absyn_Exp*_tmp978=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp978,_tag_fat(_tmp3E6,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2347
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp979=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->tag=Cyc_Toc_Dest,_tmp3E7->f1=xexp;_tmp3E7;});e->annot=_tmp979;});{
struct Cyc_Absyn_Stmt*_tmp3E8=({struct _tuple1*_tmp97D=x;void*_tmp97C=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp97B=mexp;Cyc_Absyn_declare_stmt(_tmp97D,_tmp97C,_tmp97B,({
struct Cyc_Absyn_Stmt*_tmp97A=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp97A,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2349
struct Cyc_Absyn_Stmt*s=_tmp3E8;
# 2353
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp97E=Cyc_Toc_stmt_exp_r(s);e->r=_tmp97E;});
if(vse != 0)
({void*_tmp980=({void*_tmp97F=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp97F,Cyc_Absyn_copy_exp(e));});e->r=_tmp980;});
goto _LL7;}}}}}}}case 18U: _LL30: _tmp323=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL31: {struct Cyc_Absyn_Exp*e1=_tmp323;
# 2361
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17U: _LL32: _tmp322=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL33: {void*t=_tmp322;
({void*_tmp982=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->tag=17U,({void*_tmp981=Cyc_Toc_typ_to_c(t);_tmp3E9->f1=_tmp981;});_tmp3E9;});e->r=_tmp982;});goto _LL7;}case 19U: _LL34: _tmp320=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp321=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL35: {void*t=_tmp320;struct Cyc_List_List*fs=_tmp321;
# 2364
void*_tmp3EA=t;void*t2=_tmp3EA;
struct Cyc_List_List*_tmp3EB=fs;struct Cyc_List_List*l=_tmp3EB;
for(0;l != 0;l=l->tl){
void*_tmp3EC=(void*)l->hd;void*_stmttmp3E=_tmp3EC;void*_tmp3ED=_stmttmp3E;unsigned _tmp3EE;struct _fat_ptr*_tmp3EF;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3ED)->tag == 0U){_LLD3: _tmp3EF=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3ED)->f1;_LLD4: {struct _fat_ptr*n=_tmp3EF;
goto _LLD2;}}else{_LLD5: _tmp3EE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3ED)->f1;_LLD6: {unsigned n=_tmp3EE;
# 2370
{void*_tmp3F0=Cyc_Tcutil_compress(t2);void*_stmttmp3F=_tmp3F0;void*_tmp3F1=_stmttmp3F;struct Cyc_List_List*_tmp3F2;struct Cyc_List_List*_tmp3F3;struct Cyc_Absyn_Datatypefield*_tmp3F4;union Cyc_Absyn_AggrInfo _tmp3F5;switch(*((int*)_tmp3F1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F1)->f1)){case 20U: _LLD8: _tmp3F5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F1)->f1)->f1;_LLD9: {union Cyc_Absyn_AggrInfo info=_tmp3F5;
# 2372
struct Cyc_Absyn_Aggrdecl*_tmp3F6=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp3F6;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D2;_tmp7D2.tag=0U,({struct _fat_ptr _tmp983=({const char*_tmp3F9="struct fields must be known";_tag_fat(_tmp3F9,sizeof(char),28U);});_tmp7D2.f1=_tmp983;});_tmp7D2;});void*_tmp3F7[1U];_tmp3F7[0]=& _tmp3F8;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3F7,sizeof(void*),1U));});
_tmp3F3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F1)->f1)->f1).KnownDatatypefield).tag == 2){_LLDE: _tmp3F4=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F1)->f1)->f1).KnownDatatypefield).val).f2;_LLDF: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3F4;
# 2386
if(n == (unsigned)0)
({void*_tmp984=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->tag=0U,_tmp3FD->f1=Cyc_Toc_tag_sp;_tmp3FD;}));l->hd=_tmp984;});else{
# 2389
t2=(*((struct _tuple13*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp986=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->tag=0U,({struct _fat_ptr*_tmp985=Cyc_Absyn_fieldname((int)n);_tmp3FE->f1=_tmp985;});_tmp3FE;}));l->hd=_tmp986;});}
# 2392
goto _LLD7;}}else{goto _LLE0;}default: goto _LLE0;}case 7U: _LLDA: _tmp3F3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1)->f2;_LLDB: {struct Cyc_List_List*fields=_tmp3F3;
# 2377
struct Cyc_Absyn_Aggrfield*_tmp3FA=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp3FA;
({void*_tmp987=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->tag=0U,_tmp3FB->f1=nth_field->name;_tmp3FB;}));l->hd=_tmp987;});
t2=nth_field->type;
goto _LLD7;}case 6U: _LLDC: _tmp3F2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F1)->f1;_LLDD: {struct Cyc_List_List*ts=_tmp3F2;
# 2382
({void*_tmp989=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->tag=0U,({struct _fat_ptr*_tmp988=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp3FC->f1=_tmp988;});_tmp3FC;}));l->hd=_tmp989;});
t2=(*((struct _tuple13*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD7;}default: _LLE0: _LLE1:
# 2393
({struct Cyc_Warn_String_Warn_Warg_struct _tmp400=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D3;_tmp7D3.tag=0U,({struct _fat_ptr _tmp98A=({const char*_tmp401="impossible type for offsetof tuple index";_tag_fat(_tmp401,sizeof(char),41U);});_tmp7D3.f1=_tmp98A;});_tmp7D3;});void*_tmp3FF[1U];_tmp3FF[0]=& _tmp400;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3FF,sizeof(void*),1U));});}_LLD7:;}
# 2395
goto _LLD2;}}_LLD2:;}
# 2398
({void*_tmp98C=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->tag=19U,({void*_tmp98B=Cyc_Toc_typ_to_c(t);_tmp402->f1=_tmp98B;}),_tmp402->f2=fs;_tmp402;});e->r=_tmp98C;});
goto _LL7;}case 21U: _LL36: _tmp31C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp31D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp31E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp31F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL37: {struct Cyc_Absyn_Exp*e1=_tmp31C;struct _fat_ptr*f=_tmp31D;int is_tagged=_tmp31E;int is_read=_tmp31F;
# 2401
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp991=({struct Cyc_Absyn_Exp*_tmp990=e1;void*_tmp98F=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp98E=e1_cyc_type;struct _fat_ptr*_tmp98D=f;Cyc_Toc_check_tagged_union(_tmp990,_tmp98F,_tmp98E,_tmp98D,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2405
e->r=_tmp991;});
# 2407
if(is_poly)
({void*_tmp993=({void*_tmp992=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp992,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp993;});
goto _LL7;}case 22U: _LL38: _tmp318=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp319=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp31A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp31B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL39: {struct Cyc_Absyn_Exp*e1=_tmp318;struct _fat_ptr*f=_tmp319;int is_tagged=_tmp31A;int is_read=_tmp31B;
# 2411
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp403=Cyc_Tcutil_compress(e1typ);void*_stmttmp40=_tmp403;void*_tmp404=_stmttmp40;struct Cyc_Absyn_PtrInfo _tmp405;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404)->tag == 3U){_LLE3: _tmp405=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404)->f1;_LLE4: {struct Cyc_Absyn_PtrInfo p=_tmp405;
ta=p.elt_type;goto _LLE2;}}else{_LLE5: _LLE6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp407=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D4;_tmp7D4.tag=0U,({struct _fat_ptr _tmp994=({const char*_tmp408="get_ptr_typ: not a pointer!";_tag_fat(_tmp408,sizeof(char),28U);});_tmp7D4.f1=_tmp994;});_tmp7D4;});void*_tmp406[1U];_tmp406[0]=& _tmp407;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp406,sizeof(void*),1U));});}_LLE2:;}
# 2418
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp999=({struct Cyc_Absyn_Exp*_tmp998=e1;void*_tmp997=Cyc_Toc_typ_to_c(e1typ);void*_tmp996=ta;struct _fat_ptr*_tmp995=f;Cyc_Toc_check_tagged_union(_tmp998,_tmp997,_tmp996,_tmp995,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp999;});
if(is_poly && is_read)
({void*_tmp99B=({void*_tmp99A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99A,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp99B;});
goto _LL7;}case 20U: _LL3A: _tmp317=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL3B: {struct Cyc_Absyn_Exp*e1=_tmp317;
# 2426
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23U: _LL3C: _tmp315=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp316=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL3D: {struct Cyc_Absyn_Exp*e1=_tmp315;struct Cyc_Absyn_Exp*e2=_tmp316;
# 2430
{void*_tmp409=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp41=_tmp409;void*_tmp40A=_stmttmp41;struct Cyc_List_List*_tmp40B;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40A)->tag == 6U){_LLE8: _tmp40B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40A)->f1;_LLE9: {struct Cyc_List_List*ts=_tmp40B;
# 2432
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp40C=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp40C;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple14 _tmp40D=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple14 _stmttmp42=_tmp40D;struct _tuple14 _tmp40E=_stmttmp42;int _tmp410;unsigned _tmp40F;_LLED: _tmp40F=_tmp40E.f1;_tmp410=_tmp40E.f2;_LLEE: {unsigned i=_tmp40F;int known=_tmp410;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp412=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D5;_tmp7D5.tag=0U,({struct _fat_ptr _tmp99C=({const char*_tmp413="unknown tuple subscript in translation to C";_tag_fat(_tmp413,sizeof(char),44U);});_tmp7D5.f1=_tmp99C;});_tmp7D5;});void*_tmp411[1U];_tmp411[0]=& _tmp412;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp411,sizeof(void*),1U));});
({void*_tmp99E=({struct Cyc_Absyn_Exp*_tmp99D=e1;Cyc_Toc_aggrmember_exp_r(_tmp99D,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp99E;});
goto _LLE7;}}}}}else{_LLEA: _LLEB:
# 2442
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp99F=Cyc_Toc_deref_exp_r(e1);e->r=_tmp99F;});
goto _LLE7;}}_LLE7:;}
# 2448
goto _LL7;}case 24U: _LL3E: _tmp314=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL3F: {struct Cyc_List_List*es=_tmp314;
# 2450
if(!Cyc_Toc_is_toplevel(nv)){
# 2452
void*_tmp414=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp414;
{void*_tmp415=strct_typ;union Cyc_Absyn_AggrInfo _tmp416;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp415)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp415)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp415)->f2 == 0){_LLF0: _tmp416=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp415)->f1)->f1;_LLF1: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp416;
# 2455
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp417=_cycalloc(sizeof(*_tmp417));({struct _tuple20*_tmp9A1=({struct _fat_ptr*_tmp9A0=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9A0,(struct Cyc_Absyn_Exp*)es->hd);});_tmp417->hd=_tmp9A1;}),_tmp417->tl=dles;_tmp417;});}}
# 2461
({void*_tmp9A3=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->tag=29U,_tmp418->f1=sd->name,_tmp418->f2=0,({struct Cyc_List_List*_tmp9A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp418->f3=_tmp9A2;}),_tmp418->f4=sd;_tmp418;});e->r=_tmp9A3;});
e->topt=strct_typ;
goto _LLEF;}}else{goto _LLF2;}}else{goto _LLF2;}}else{_LLF2: _LLF3:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D6;_tmp7D6.tag=0U,({struct _fat_ptr _tmp9A4=({const char*_tmp41B="tuple type not an aggregate";_tag_fat(_tmp41B,sizeof(char),28U);});_tmp7D6.f1=_tmp9A4;});_tmp7D6;});void*_tmp419[1U];_tmp419[0]=& _tmp41A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp419,sizeof(void*),1U));});}_LLEF:;}
# 2466
goto _LL7;}else{
# 2470
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp41D=_cycalloc(sizeof(*_tmp41D));({struct _tuple20*_tmp9A5=({struct _tuple20*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->f1=0,_tmp41C->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp41C;});_tmp41D->hd=_tmp9A5;}),_tmp41D->tl=dles;_tmp41D;});}
# 2475
({void*_tmp9A6=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9A6;});}
# 2477
goto _LL7;}case 26U: _LL40: _tmp313=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL41: {struct Cyc_List_List*dles0=_tmp313;
# 2483
{struct Cyc_List_List*_tmp41E=dles0;struct Cyc_List_List*dles=_tmp41E;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9A7=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp9A7;});
goto _LL7;}case 27U: _LL42: _tmp30F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp310=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp311=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp312=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL43: {struct Cyc_Absyn_Vardecl*vd=_tmp30F;struct Cyc_Absyn_Exp*e1=_tmp310;struct Cyc_Absyn_Exp*e2=_tmp311;int iszeroterm=_tmp312;
# 2491
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple14 _tmp41F=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple14 _stmttmp43=_tmp41F;struct _tuple14 _tmp420=_stmttmp43;int _tmp422;unsigned _tmp421;_LLF5: _tmp421=_tmp420.f1;_tmp422=_tmp420.f2;_LLF6: {unsigned sz=_tmp421;int known=_tmp422;
void*_tmp423=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp423;
struct Cyc_List_List*es=0;
# 2498
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp424=0U;({unsigned _tmp9A9=e1->loc;struct _fat_ptr _tmp9A8=({const char*_tmp425="cannot determine value of constant";_tag_fat(_tmp425,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9A9,_tmp9A8,_tag_fat(_tmp424,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp427=_cycalloc(sizeof(*_tmp427));({struct _tuple20*_tmp9AA=({struct _tuple20*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->f1=0,_tmp426->f2=e2;_tmp426;});_tmp427->hd=_tmp9AA;}),_tmp427->tl=es;_tmp427;});}}
# 2504
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp428=({void*_tmp9AB=elt_typ;Cyc_Toc_cast_it(_tmp9AB,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp428;
es=({struct Cyc_List_List*_tmp9AD=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9AD,({struct Cyc_List_List*_tmp42A=_cycalloc(sizeof(*_tmp42A));({struct _tuple20*_tmp9AC=({struct _tuple20*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->f1=0,_tmp429->f2=ezero;_tmp429;});_tmp42A->hd=_tmp9AC;}),_tmp42A->tl=0;_tmp42A;}));});}}
# 2509
({void*_tmp9AE=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9AE;});}}
# 2511
goto _LL7;}case 28U: _LL44: _tmp30C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp30D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp30E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL45: {struct Cyc_Absyn_Exp*e1=_tmp30C;void*t1=_tmp30D;int iszeroterm=_tmp30E;
# 2518
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9AF=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9AF;});else{
# 2521
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30U: _LL46: _tmp30A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp30B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL47: {void*st=_tmp30A;struct Cyc_List_List*dles=_tmp30B;
# 2525
{struct Cyc_List_List*_tmp42B=dles;struct Cyc_List_List*dles2=_tmp42B;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)dles2->hd)).f2);}}{
void*_tmp42C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp42C;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp42D=Cyc_Tcutil_compress(strct_typ);void*_stmttmp44=_tmp42D;void*_tmp42E=_stmttmp44;union Cyc_Absyn_AggrInfo _tmp42F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->f2 == 0){_LLF8: _tmp42F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42E)->f1)->f1;_LLF9: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp42F;
# 2532
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp9B0=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430->tag=29U,_tmp430->f1=sd->name,_tmp430->f2=0,_tmp430->f3=dles,_tmp430->f4=sd;_tmp430;});e->r=_tmp9B0;});
e->topt=strct_typ;
goto _LLF7;}}else{goto _LLFA;}}else{goto _LLFA;}}else{_LLFA: _LLFB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp432=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D7;_tmp7D7.tag=0U,({struct _fat_ptr _tmp9B1=({const char*_tmp433="anonStruct type not an aggregate";_tag_fat(_tmp433,sizeof(char),33U);});_tmp7D7.f1=_tmp9B1;});_tmp7D7;});void*_tmp431[1U];_tmp431[0]=& _tmp432;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp431,sizeof(void*),1U));});}_LLF7:;}else{
# 2539
({void*_tmp9B2=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp9B2;});}
goto _LL7;}}case 29U: _LL48: _tmp306=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp307=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp308=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_tmp309=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F7)->f4;_LL49: {struct _tuple1**tdn=_tmp306;struct Cyc_List_List*exist_ts=_tmp307;struct Cyc_List_List*dles=_tmp308;struct Cyc_Absyn_Aggrdecl*sd=_tmp309;
# 2545
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp435=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D8;_tmp7D8.tag=0U,({struct _fat_ptr _tmp9B3=({const char*_tmp436="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp436,sizeof(char),58U);});_tmp7D8.f1=_tmp9B3;});_tmp7D8;});void*_tmp434[1U];_tmp434[0]=& _tmp435;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp434,sizeof(void*),1U));});{
void*_tmp437=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp437;
{void*_tmp438=Cyc_Tcutil_compress(new_typ);void*_stmttmp45=_tmp438;void*_tmp439=_stmttmp45;union Cyc_Absyn_AggrInfo _tmp43A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->f1)->tag == 20U){_LLFD: _tmp43A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->f1)->f1;_LLFE: {union Cyc_Absyn_AggrInfo info=_tmp43A;
({struct _tuple1*_tmp9B4=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp9B4;});goto _LLFC;}}else{goto _LLFF;}}else{_LLFF: _LL100:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D9;_tmp7D9.tag=0U,({struct _fat_ptr _tmp9B5=({const char*_tmp43D="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp43D,sizeof(char),44U);});_tmp7D9.f1=_tmp9B5;});_tmp7D9;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43B,sizeof(void*),1U));});}_LLFC:;}{
# 2556
struct Cyc_List_List*_tmp43E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp43E;
if(typ_fields == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp43F=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp43F;
struct Cyc_List_List*_tmp440=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp440;
# 2563
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp441=Cyc_Tcutil_compress(old_typ);void*_stmttmp46=_tmp441;void*_tmp442=_stmttmp46;struct Cyc_List_List*_tmp443;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp442)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp442)->f1)->tag == 20U){_LL102: _tmp443=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp442)->f2;_LL103: {struct Cyc_List_List*param_ts=_tmp443;
# 2567
{struct Cyc_List_List*_tmp444=fields;struct Cyc_List_List*fields2=_tmp444;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple35*_tmp445=(struct _tuple35*)fields2->hd;struct _tuple35*_stmttmp47=_tmp445;struct _tuple35*_tmp446=_stmttmp47;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Aggrfield*_tmp447;_LL107: _tmp447=_tmp446->f1;_tmp448=_tmp446->f2;_LL108: {struct Cyc_Absyn_Aggrfield*aggrfield=_tmp447;struct Cyc_Absyn_Exp*fieldexp=_tmp448;
void*_tmp449=fieldexp->topt;void*old_field_typ=_tmp449;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2572
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp9B7=({void*_tmp9B6=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp9B6,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2574
fieldexp->r=_tmp9B7;});
# 2577
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp44A=aggrfield->name;struct _fat_ptr*n=_tmp44A;
struct Cyc_Absyn_Exp*_tmp44B=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp44B;
struct _tuple20*_tmp44C=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple20*tag_dle=_tmp44C;
struct _tuple20*_tmp44D=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple20*val_dle=_tmp44D;
struct _tuple1*s=({struct _tuple1*_tmp458=_cycalloc(sizeof(*_tmp458));({union Cyc_Absyn_Nmspace _tmp9BB=Cyc_Absyn_Abs_n(0,1);_tmp458->f1=_tmp9BB;}),({
struct _fat_ptr*_tmp9BA=({struct _fat_ptr*_tmp457=_cycalloc(sizeof(*_tmp457));({struct _fat_ptr _tmp9B9=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp455=({struct Cyc_String_pa_PrintArg_struct _tmp7DB;_tmp7DB.tag=0U,_tmp7DB.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp7DB;});struct Cyc_String_pa_PrintArg_struct _tmp456=({struct Cyc_String_pa_PrintArg_struct _tmp7DA;_tmp7DA.tag=0U,_tmp7DA.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp7DA;});void*_tmp453[2U];_tmp453[0]=& _tmp455,_tmp453[1]=& _tmp456;({struct _fat_ptr _tmp9B8=({const char*_tmp454="_union_%s_%s";_tag_fat(_tmp454,sizeof(char),13U);});Cyc_aprintf(_tmp9B8,_tag_fat(_tmp453,sizeof(void*),2U));});});*_tmp457=_tmp9B9;});_tmp457;});_tmp458->f2=_tmp9BA;});_tmp458;});
# 2585
struct _tuple20*_tmp44E=({
struct _fat_ptr*_tmp9BD=n;Cyc_Toc_make_field(_tmp9BD,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->tag=29U,_tmp452->f1=s,_tmp452->f2=0,({struct Cyc_List_List*_tmp9BC=({struct _tuple20*_tmp451[2U];_tmp451[0]=tag_dle,_tmp451[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp451,sizeof(struct _tuple20*),2U));});_tmp452->f3=_tmp9BC;}),_tmp452->f4=0;_tmp452;}),0U));});
# 2585
struct _tuple20*u_dle=_tmp44E;
# 2588
({void*_tmp9BF=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->tag=29U,_tmp450->f1=*tdn,_tmp450->f2=0,({struct Cyc_List_List*_tmp9BE=({struct _tuple20*_tmp44F[1U];_tmp44F[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp44F,sizeof(struct _tuple20*),1U));});_tmp450->f3=_tmp9BE;}),_tmp450->f4=sd;_tmp450;});e->r=_tmp9BF;});}
# 2594
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp459=({struct Cyc_List_List*_tmp9C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9C0,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(sd->tvs,param_ts));});
# 2595
struct Cyc_List_List*inst=_tmp459;
# 2597
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp45A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp45A;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp45B=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp45B;
void*_tmp45C=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp45C;
struct Cyc_Absyn_Aggrfield*_tmp45D=({struct Cyc_Absyn_Aggrfield*_tmp9C1=old_f;Cyc_Toc_aggrfield_to_c(_tmp9C1,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2601
struct Cyc_Absyn_Aggrfield*new_f=_tmp45D;
# 2603
new_fields=({struct Cyc_List_List*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->hd=new_f,_tmp45E->tl=new_fields;_tmp45E;});
# 2609
if(fs->tl == 0){
{void*_tmp45F=Cyc_Tcutil_compress(new_f->type);void*_stmttmp48=_tmp45F;void*_tmp460=_stmttmp48;struct Cyc_Absyn_ArrayInfo _tmp461;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp460)->tag == 4U){_LL10A: _tmp461=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp460)->f1;_LL10B: {struct Cyc_Absyn_ArrayInfo ai=_tmp461;
# 2612
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp462=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp462;
({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp9C2;});
({void*_tmp9C3=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->tag=4U,_tmp463->f1=ai2;_tmp463;});new_f->type=_tmp9C3;});}
# 2617
goto _LL109;}}else{_LL10C: _LL10D:
# 2621
 if(fieldexp->topt == 0)
goto _LL109;
{void*_tmp464=Cyc_Tcutil_compress((void*)_check_null(fieldexp->topt));void*_stmttmp49=_tmp464;void*_tmp465=_stmttmp49;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp465)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp465)->f1)->tag == 20U){_LL10F: _LL110:
# 2625
 new_f->type=(void*)_check_null(fieldexp->topt);goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
 goto _LL10E;}_LL10E:;}
# 2628
goto _LL109;}_LL109:;}
# 2632
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp9C5=({struct Cyc_List_List*_tmp467=_cycalloc(sizeof(*_tmp467));({void*_tmp9C4=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->tag=6U,_tmp466->f1=0;_tmp466;});_tmp467->hd=_tmp9C4;}),_tmp467->tl=new_f->attributes;_tmp467;});new_f->attributes=_tmp9C5;});}}}
# 2638
sd=({struct _fat_ptr*_tmp9C8=({struct _fat_ptr*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct _fat_ptr _tmp9C7=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp46A=({struct Cyc_Int_pa_PrintArg_struct _tmp7DC;_tmp7DC.tag=1U,_tmp7DC.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7DC;});void*_tmp468[1U];_tmp468[0]=& _tmp46A;({struct _fat_ptr _tmp9C6=({const char*_tmp469="_genStruct%d";_tag_fat(_tmp469,sizeof(char),13U);});Cyc_aprintf(_tmp9C6,_tag_fat(_tmp468,sizeof(void*),1U));});});*_tmp46B=_tmp9C7;});_tmp46B;});Cyc_Toc_make_c_struct_defn(_tmp9C8,
# 2640
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2644
({void*_tmp9C9=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp46C=_cycalloc(sizeof(*_tmp46C));*_tmp46C=sd;_tmp46C;})),0);e->topt=_tmp9C9;});}}}}
# 2647
goto _LL101;}}else{goto _LL104;}}else{_LL104: _LL105:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp46E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DD;_tmp7DD.tag=0U,({struct _fat_ptr _tmp9CA=({const char*_tmp46F="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp46F,sizeof(char),39U);});_tmp7DD.f1=_tmp9CA;});_tmp7DD;});void*_tmp46D[1U];_tmp46D[0]=& _tmp46E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp46D,sizeof(void*),1U));});}_LL101:;}else{
# 2656
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2658
struct _tuple35*_tmp470=(struct _tuple35*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple35*_stmttmp4A=_tmp470;struct _tuple35*_tmp471=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Aggrfield*_tmp472;_LL114: _tmp472=_tmp471->f1;_tmp473=_tmp471->f2;_LL115: {struct Cyc_Absyn_Aggrfield*field=_tmp472;struct Cyc_Absyn_Exp*fieldexp=_tmp473;
void*_tmp474=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp474;
void*_tmp475=field->type;void*fieldtyp=_tmp475;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp9CC=({
void*_tmp9CB=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp9CB,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2664
fieldexp->r=_tmp9CC;});{
# 2667
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp476=({struct _tuple20*_tmp478[2U];({struct _tuple20*_tmp9CE=({struct _tuple20*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->f1=0,_tmp479->f2=field_tag_exp;_tmp479;});_tmp478[0]=_tmp9CE;}),({struct _tuple20*_tmp9CD=({struct _tuple20*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->f1=0,_tmp47A->f2=fieldexp;_tmp47A;});_tmp478[1]=_tmp9CD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp478,sizeof(struct _tuple20*),2U));});struct Cyc_List_List*newdles=_tmp476;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp9D0=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp477[1U];({struct _tuple20*_tmp9CF=Cyc_Toc_make_field(field->name,umem);_tmp477[0]=_tmp9CF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp477,sizeof(struct _tuple20*),1U));}));e->r=_tmp9D0;});}}}else{
# 2674
struct Cyc_List_List*_tmp47B=0;struct Cyc_List_List*newdles=_tmp47B;
struct Cyc_List_List*_tmp47C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp47C;
for(0;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp47D=fields;struct Cyc_List_List*fields2=_tmp47D;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple35*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple35*_tmp47E=(struct _tuple35*)fields2->hd;struct _tuple35*_stmttmp4B=_tmp47E;struct _tuple35*_tmp47F=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Aggrfield*_tmp480;_LL117: _tmp480=_tmp47F->f1;_tmp481=_tmp47F->f2;_LL118: {struct Cyc_Absyn_Aggrfield*field=_tmp480;struct Cyc_Absyn_Exp*fieldexp=_tmp481;
void*_tmp482=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp482;
void*_tmp483=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp483;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2684
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2686
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp9D1=fieldtyp;Cyc_Toc_cast_it(_tmp9D1,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp485=_cycalloc(sizeof(*_tmp485));({struct _tuple20*_tmp9D2=({struct _tuple20*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->f1=0,_tmp484->f2=fieldexp;_tmp484;});_tmp485->hd=_tmp9D2;}),_tmp485->tl=newdles;_tmp485;});
break;}}}}
# 2693
({void*_tmp9D3=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(newdles));e->r=_tmp9D3;});}}
# 2696
goto _LL7;}}}}case 31U: _LL4A: _tmp303=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp304=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_tmp305=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F7)->f3;_LL4B: {struct Cyc_List_List*es=_tmp303;struct Cyc_Absyn_Datatypedecl*tud=_tmp304;struct Cyc_Absyn_Datatypefield*tuf=_tmp305;
# 2700
struct Cyc_List_List*_tmp486=tuf->typs;struct Cyc_List_List*tqts=_tmp486;
{struct Cyc_List_List*_tmp487=es;struct Cyc_List_List*es2=_tmp487;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp488=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp488;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple13*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp9D4=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp9D4;});}}{
# 2711
struct Cyc_Absyn_Exp*_tmp489=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2711
struct Cyc_Absyn_Exp*tag_exp=_tmp489;
# 2714
if(!Cyc_Toc_is_toplevel(nv)){
# 2716
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp48A=_cycalloc(sizeof(*_tmp48A));({struct _tuple20*_tmp9D6=({struct _fat_ptr*_tmp9D5=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9D5,(struct Cyc_Absyn_Exp*)es->hd);});_tmp48A->hd=_tmp9D6;}),_tmp48A->tl=dles;_tmp48A;});}}{
# 2722
struct _tuple20*_tmp48B=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple20*tag_dle=_tmp48B;
({void*_tmp9DA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->tag=29U,({struct _tuple1*_tmp9D9=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp48D->f1=_tmp9D9;}),_tmp48D->f2=0,({
struct Cyc_List_List*_tmp9D8=({struct Cyc_List_List*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->hd=tag_dle,({struct Cyc_List_List*_tmp9D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp48C->tl=_tmp9D7;});_tmp48C;});_tmp48D->f3=_tmp9D8;}),_tmp48D->f4=0;_tmp48D;});
# 2723
e->r=_tmp9DA;});}}else{
# 2728
struct Cyc_List_List*_tmp48E=0;struct Cyc_List_List*dles=_tmp48E;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp490=_cycalloc(sizeof(*_tmp490));({struct _tuple20*_tmp9DB=({struct _tuple20*_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->f1=0,_tmp48F->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp48F;});_tmp490->hd=_tmp9DB;}),_tmp490->tl=dles;_tmp490;});}
({void*_tmp9DE=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));({struct _tuple20*_tmp9DD=({struct _tuple20*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->f1=0,_tmp491->f2=tag_exp;_tmp491;});_tmp492->hd=_tmp9DD;}),({
struct Cyc_List_List*_tmp9DC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp492->tl=_tmp9DC;});_tmp492;}));
# 2731
e->r=_tmp9DE;});}
# 2734
goto _LL7;}}case 32U: _LL4C: _LL4D:
# 2736
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp2FD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).is_calloc;_tmp2FE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).rgn;_tmp2FF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).elt_type;_tmp300=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).num_elts;_tmp301=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).fat_result;_tmp302=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1).inline_call;_LL51: {int is_calloc=_tmp2FD;struct Cyc_Absyn_Exp*rgnopt=_tmp2FE;void**topt=_tmp2FF;struct Cyc_Absyn_Exp*num_elts=_tmp300;int is_fat=_tmp301;int inline_call=_tmp302;
# 2740
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(topt)));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2749
if(is_calloc){
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp493[3U];_tmp493[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp9DF=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp493[1]=_tmp9DF;}),_tmp493[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9E0=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp9E0,_tag_fat(_tmp493,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2755
pexp=({void*_tmp9E2=*topt;struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9E2,_tmp9E1,num_elts2);});}}else{
# 2757
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
if(inline_call)
pexp=Cyc_Toc_rmalloc_inline_exp(rgnopt,num_elts2);else{
# 2762
pexp=Cyc_Toc_rmalloc_exp(rgnopt,num_elts2);}}else{
# 2764
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2766
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp494[3U];_tmp494[0]=pexp,_tmp494[1]=elt_sz,_tmp494[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9E3=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp9E3,_tag_fat(_tmp494,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp9E6=(struct _tuple1*)_check_null(x);void*_tmp9E5=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9E4=num_elts;Cyc_Absyn_declare_stmt(_tmp9E6,_tmp9E5,_tmp9E4,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp9E7=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9E7;});}else{
# 2773
e->r=pexp->r;}
goto _LL7;}}case 35U: _LL52: _tmp2FB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp2FC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL53: {struct Cyc_Absyn_Exp*e1=_tmp2FB;struct Cyc_Absyn_Exp*e2=_tmp2FC;
# 2783
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp496=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DE;_tmp7DE.tag=0U,({struct _fat_ptr _tmp9E8=({const char*_tmp497="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp497,sizeof(char),57U);});_tmp7DE.f1=_tmp9E8;});_tmp7DE;});void*_tmp495[1U];_tmp495[0]=& _tmp496;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp495,sizeof(void*),1U));});{
# 2791
unsigned _tmp498=e->loc;unsigned loc=_tmp498;
struct _tuple1*_tmp499=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp499;
struct Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp49A;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp49B=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp49B;
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp49C;v2e->topt=e2_old_typ;{
# 2797
struct Cyc_Absyn_Exp*_tmp49D=({struct Cyc_Absyn_Exp*_tmp9EA=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp9E9=v2e;Cyc_Absyn_assign_exp(_tmp9EA,_tmp9E9,loc);});struct Cyc_Absyn_Exp*s1e=_tmp49D;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp49E=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp49E;
struct Cyc_Absyn_Exp*_tmp49F=({struct Cyc_Absyn_Exp*_tmp9EC=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp9EB=v1e;Cyc_Absyn_assign_exp(_tmp9EC,_tmp9EB,loc);});struct Cyc_Absyn_Exp*s2e=_tmp49F;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A0=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp4A0;
# 2802
struct Cyc_Absyn_Stmt*_tmp4A1=({struct _tuple1*_tmp9F4=v1;void*_tmp9F3=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9F2=e1;struct Cyc_Absyn_Stmt*_tmp9F1=({
struct _tuple1*_tmp9F0=v2;void*_tmp9EF=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9EE=e2;struct Cyc_Absyn_Stmt*_tmp9ED=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2803
Cyc_Absyn_declare_stmt(_tmp9F0,_tmp9EF,_tmp9EE,_tmp9ED,loc);});
# 2802
Cyc_Absyn_declare_stmt(_tmp9F4,_tmp9F3,_tmp9F2,_tmp9F1,loc);});struct Cyc_Absyn_Stmt*s=_tmp4A1;
# 2805
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmp9F5=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9F5;});
goto _LL7;}}}}}}case 38U: _LL54: _tmp2F9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_tmp2FA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F7)->f2;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp2F9;struct _fat_ptr*f=_tmp2FA;
# 2810
void*_tmp4A2=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp4A2;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp4A3=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp4A4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A3)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A3)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A3)->f1)->f1).KnownAggr).tag == 2){_LL11A: _tmp4A4=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A3)->f1)->f1).KnownAggr).val;_LL11B: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4A4;
# 2814
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp4A5;
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp4A6;
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp4A7;
({void*_tmp9F6=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmp9F6;});
goto _LL119;}}else{goto _LL11C;}}else{goto _LL11C;}}else{_LL11C: _LL11D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DF;_tmp7DF.tag=0U,({struct _fat_ptr _tmp9F7=({const char*_tmp4AA="non-aggregate type in tagcheck";_tag_fat(_tmp4AA,sizeof(char),31U);});_tmp7DF.f1=_tmp9F7;});_tmp7DF;});void*_tmp4A8[1U];_tmp4A8[0]=& _tmp4A9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4A8,sizeof(void*),1U));});}_LL119:;}
# 2821
goto _LL7;}case 37U: _LL56: _tmp2F8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2F7)->f1;_LL57: {struct Cyc_Absyn_Stmt*s=_tmp2F8;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36U: _LL58: _LL59:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E0;_tmp7E0.tag=0U,({struct _fat_ptr _tmp9F8=({const char*_tmp4AD="UnresolvedMem";_tag_fat(_tmp4AD,sizeof(char),14U);});_tmp7E0.f1=_tmp9F8;});_tmp7E0;});void*_tmp4AB[1U];_tmp4AB[0]=& _tmp4AC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4AB,sizeof(void*),1U));});case 25U: _LL5A: _LL5B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E1;_tmp7E1.tag=0U,({struct _fat_ptr _tmp9F9=({const char*_tmp4B0="compoundlit";_tag_fat(_tmp4B0,sizeof(char),12U);});_tmp7E1.f1=_tmp9F9;});_tmp7E1;});void*_tmp4AE[1U];_tmp4AE[0]=& _tmp4AF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4AE,sizeof(void*),1U));});case 39U: _LL5C: _LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E2;_tmp7E2.tag=0U,({struct _fat_ptr _tmp9FA=({const char*_tmp4B3="valueof(-)";_tag_fat(_tmp4B3,sizeof(char),11U);});_tmp7E2.f1=_tmp9FA;});_tmp7E2;});void*_tmp4B1[1U];_tmp4B1[0]=& _tmp4B2;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B1,sizeof(void*),1U));});default: _LL5E: _LL5F:
 goto _LL7;}_LL7:;}
# 2828
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E4;_tmp7E4.tag=0U,({struct _fat_ptr _tmp9FB=({const char*_tmp4B7="Toc did not examine an inserted check: ";_tag_fat(_tmp4B7,sizeof(char),40U);});_tmp7E4.f1=_tmp9FB;});_tmp7E4;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7E3;_tmp7E3.tag=3U,_tmp7E3.f1=e;_tmp7E3;});void*_tmp4B4[2U];_tmp4B4[0]=& _tmp4B5,_tmp4B4[1]=& _tmp4B6;({unsigned _tmp9FC=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp9FC,_tag_fat(_tmp4B4,sizeof(void*),2U));});});}}struct _tuple36{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2856 "toc.cyc"
static struct _tuple36*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2858
return({struct _tuple36*_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8));_tmp4B8->f1=0,({struct _fat_ptr*_tmp9FE=Cyc_Toc_fresh_label();_tmp4B8->f2=_tmp9FE;}),({struct _fat_ptr*_tmp9FD=Cyc_Toc_fresh_label();_tmp4B8->f3=_tmp9FD;}),_tmp4B8->f4=sc;_tmp4B8;});}
# 2864
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4B9=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp4B9;
# 2870
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4BC=p->orig_pat;if((_tmp4BC.pattern).tag != 1)_throw_match();(_tmp4BC.pattern).val;})->topt);
void*_tmp4BA=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp4BA;
void*_tmp4BB=t2;switch(*((int*)_tmp4BB)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BB)->f1)){case 0U: _LL1: _LL2:
# 2875
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmp9FF=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp9FF,v);});goto _LL0;}_LL0:;}{
# 2881
void*_tmp4BD=p->access;void*_stmttmp4C=_tmp4BD;void*_tmp4BE=_stmttmp4C;struct _fat_ptr*_tmp4C0;int _tmp4BF;unsigned _tmp4C1;unsigned _tmp4C2;switch(*((int*)_tmp4BE)){case 0U: _LLA: _LLB:
# 2885
 goto _LL9;case 1U: _LLC: _LLD:
# 2890
 if(ps->tl != 0){
void*_tmp4C3=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4D=_tmp4C3;void*_tmp4C4=_stmttmp4D;unsigned _tmp4C7;struct Cyc_Absyn_Datatypefield*_tmp4C6;struct Cyc_Absyn_Datatypedecl*_tmp4C5;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C4)->tag == 3U){_LL15: _tmp4C5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C4)->f1;_tmp4C6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C4)->f2;_tmp4C7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C4)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4C5;struct Cyc_Absyn_Datatypefield*tuf=_tmp4C6;unsigned i=_tmp4C7;
# 2893
ps=ps->tl;
v=({void*_tmpA01=({void*_tmpA00=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA00,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA01,v);});
v=({struct Cyc_Absyn_Exp*_tmpA02=v;Cyc_Absyn_aggrarrow_exp(_tmpA02,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2900
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp4C2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4BE)->f3;_LLF: {unsigned i=_tmp4C2;
_tmp4C1=i;goto _LL11;}case 2U: _LL10: _tmp4C1=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4BE)->f1;_LL11: {unsigned i=_tmp4C1;
v=({struct Cyc_Absyn_Exp*_tmpA03=v;Cyc_Toc_member_exp(_tmpA03,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _LL12: _tmp4BF=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4BE)->f1;_tmp4C0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4BE)->f2;_LL13: {int tagged=_tmp4BF;struct _fat_ptr*f=_tmp4C0;
# 2905
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2911
return v;}
# 2916
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4C8=t;struct Cyc_Absyn_Datatypefield*_tmp4CA;struct Cyc_Absyn_Datatypedecl*_tmp4C9;int _tmp4CC;struct _fat_ptr*_tmp4CB;struct Cyc_Absyn_Datatypefield*_tmp4CF;struct Cyc_Absyn_Datatypedecl*_tmp4CE;int _tmp4CD;unsigned _tmp4D0;int _tmp4D2;struct _fat_ptr _tmp4D1;struct Cyc_Absyn_Enumfield*_tmp4D4;void*_tmp4D3;struct Cyc_Absyn_Enumfield*_tmp4D6;struct Cyc_Absyn_Enumdecl*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D7;switch(*((int*)_tmp4C8)){case 0U: _LL1: _tmp4D7=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4C8)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp4D7;
return e == 0?v: e;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA04=v;Cyc_Absyn_eq_exp(_tmpA04,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA05=v;Cyc_Absyn_neq_exp(_tmpA05,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4D5=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4C8)->f1;_tmp4D6=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4C8)->f2;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp4D5;struct Cyc_Absyn_Enumfield*ef=_tmp4D6;
return({struct Cyc_Absyn_Exp*_tmpA06=v;Cyc_Absyn_eq_exp(_tmpA06,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->tag=32U,_tmp4D8->f1=ed,_tmp4D8->f2=ef;_tmp4D8;}),0U),0U);});}case 4U: _LL9: _tmp4D3=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4C8)->f1;_tmp4D4=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4C8)->f2;_LLA: {void*t=_tmp4D3;struct Cyc_Absyn_Enumfield*ef=_tmp4D4;
return({struct Cyc_Absyn_Exp*_tmpA07=v;Cyc_Absyn_eq_exp(_tmpA07,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->tag=33U,_tmp4D9->f1=t,_tmp4D9->f2=ef;_tmp4D9;}),0U),0U);});}case 5U: _LLB: _tmp4D1=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4C8)->f1;_tmp4D2=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4C8)->f2;_LLC: {struct _fat_ptr s=_tmp4D1;int i=_tmp4D2;
return({struct Cyc_Absyn_Exp*_tmpA08=v;Cyc_Absyn_eq_exp(_tmpA08,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6U: _LLD: _tmp4D0=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4C8)->f1;_LLE: {unsigned i=_tmp4D0;
return({struct Cyc_Absyn_Exp*_tmpA09=v;Cyc_Absyn_eq_exp(_tmpA09,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7U: _LLF: _tmp4CD=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4C8)->f1;_tmp4CE=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4C8)->f2;_tmp4CF=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4C8)->f3;_LL10: {int i=_tmp4CD;struct Cyc_Absyn_Datatypedecl*tud=_tmp4CE;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CF;
# 2928
LOOP1: {
void*_tmp4DA=v->r;void*_stmttmp4E=_tmp4DA;void*_tmp4DB=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;switch(*((int*)_tmp4DB)){case 14U: _LL16: _tmp4DD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4DB)->f2;_LL17: {struct Cyc_Absyn_Exp*e=_tmp4DD;
v=e;goto LOOP1;}case 20U: _LL18: _tmp4DC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4DB)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp4DC;
v=e;goto _LL15;}default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2935
v=({void*_tmpA0B=({void*_tmpA0A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA0A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA0B,v);});return({struct Cyc_Absyn_Exp*_tmpA0C=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA0C,Cyc_Absyn_uint_exp((unsigned)i,0U),0U);});}case 8U: _LL11: _tmp4CB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4C8)->f1;_tmp4CC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4C8)->f2;_LL12: {struct _fat_ptr*f=_tmp4CB;int i=_tmp4CC;
# 2939
return({struct Cyc_Absyn_Exp*_tmpA0E=({struct Cyc_Absyn_Exp*_tmpA0D=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmpA0D,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmpA0E,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _LL13: _tmp4C9=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4C8)->f1;_tmp4CA=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4C8)->f2;_LL14: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4C9;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CA;
# 2943
LOOP2: {
void*_tmp4DE=v->r;void*_stmttmp4F=_tmp4DE;void*_tmp4DF=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;switch(*((int*)_tmp4DF)){case 14U: _LL1D: _tmp4E1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4DF)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4E1;
v=e;goto LOOP2;}case 20U: _LL1F: _tmp4E0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4DF)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4E0;
v=e;goto _LL1C;}default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2950
v=({void*_tmpA10=({void*_tmpA0F=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA0F,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA10,v);});
return({struct Cyc_Absyn_Exp*_tmpA11=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA11,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2962
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple37{int f1;void*f2;};
# 2969
static struct _tuple37 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2971
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp4E2=*((struct _tuple33*)ss->hd);struct _tuple33 _stmttmp50=_tmp4E2;struct _tuple33 _tmp4E3=_stmttmp50;void*_tmp4E4;_LL1: _tmp4E4=_tmp4E3.f1;_LL2: {void*ptest=_tmp4E4;
void*_tmp4E5=ptest;struct Cyc_Absyn_Exp*_tmp4E6;struct _fat_ptr*_tmp4E7;switch(*((int*)_tmp4E5)){case 3U: _LL4: _LL5:
# 2977
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4E7=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4E5)->f1;_LLB: {struct _fat_ptr*f=_tmp4E7;
# 2981
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->tag=3U,_tmp4E8->f1=f;_tmp4E8;});
continue;}case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4E6=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4E5)->f1;if(_tmp4E6 != 0){_LLF: {struct Cyc_Absyn_Exp*e=_tmp4E6;
# 2986
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->tag=2U,_tmp4E9->f1=e;_tmp4E9;});return({struct _tuple37 _tmp7E5;_tmp7E5.f1=0,_tmp7E5.f2=k;_tmp7E5;});}}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple37 _tmp7E6;_tmp7E6.f1=0,_tmp7E6.f2=k;_tmp7E6;});}_LL3:;}}
# 2994
return({struct _tuple37 _tmp7E7;_tmp7E7.f1=c,_tmp7E7.f2=k;_tmp7E7;});}
# 2999
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4EA=p;int _tmp4EB;int _tmp4EC;unsigned _tmp4ED;struct Cyc_Absyn_Enumfield*_tmp4EF;void*_tmp4EE;struct Cyc_Absyn_Enumfield*_tmp4F1;struct Cyc_Absyn_Enumdecl*_tmp4F0;switch(*((int*)_tmp4EA)){case 3U: _LL1: _tmp4F0=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4EA)->f1;_tmp4F1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4EA)->f2;_LL2: {struct Cyc_Absyn_Enumdecl*ed=_tmp4F0;struct Cyc_Absyn_Enumfield*ef=_tmp4F1;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->tag=32U,_tmp4F2->f1=ed,_tmp4F2->f2=ef;_tmp4F2;}),0U);goto _LL0;}case 4U: _LL3: _tmp4EE=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4EA)->f1;_tmp4EF=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4EA)->f2;_LL4: {void*t=_tmp4EE;struct Cyc_Absyn_Enumfield*ef=_tmp4EF;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->tag=33U,_tmp4F3->f1=t,_tmp4F3->f2=ef;_tmp4F3;}),0U);goto _LL0;}case 6U: _LL5: _tmp4ED=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4EA)->f1;_LL6: {unsigned i=_tmp4ED;
_tmp4EC=(int)i;goto _LL8;}case 7U: _LL7: _tmp4EC=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4EA)->f1;_LL8: {int i=_tmp4EC;
_tmp4EB=i;goto _LLA;}case 8U: _LL9: _tmp4EB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4EA)->f2;_LLA: {int i=_tmp4EB;
e=Cyc_Absyn_uint_exp((unsigned)i,0U);goto _LL0;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E8;_tmp7E8.tag=0U,({struct _fat_ptr _tmpA12=({const char*_tmp4F6="compile_pat_test_as_case!";_tag_fat(_tmp4F6,sizeof(char),26U);});_tmp7E8.f1=_tmpA12;});_tmp7E8;});void*_tmp4F4[1U];_tmp4F4[0]=& _tmp4F5;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4F4,sizeof(void*),1U));});}_LL0:;}
# 3009
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->tag=17U,_tmp4F7->f1=e;_tmp4F7;}),0U);}
# 3013
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple38{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple39{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3024
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3028
void*t=(void*)_check_null(p->topt);
void*_tmp4F8=p->r;void*_stmttmp51=_tmp4F8;void*_tmp4F9=_stmttmp51;struct Cyc_List_List*_tmp4FB;union Cyc_Absyn_AggrInfo _tmp4FA;struct Cyc_List_List*_tmp4FC;struct Cyc_List_List*_tmp4FD;struct Cyc_Absyn_Pat*_tmp4FE;struct Cyc_List_List*_tmp501;struct Cyc_Absyn_Datatypefield*_tmp500;struct Cyc_Absyn_Datatypedecl*_tmp4FF;struct Cyc_Absyn_Pat*_tmp503;struct Cyc_Absyn_Vardecl*_tmp502;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Pat*_tmp506;struct Cyc_Absyn_Vardecl*_tmp505;struct Cyc_Absyn_Vardecl*_tmp507;switch(*((int*)_tmp4F9)){case 2U: _LL1: _tmp507=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4F9)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp507;
# 3031
struct Cyc_Absyn_Pat*_tmp508=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp508;
p2->topt=p->topt;
_tmp505=vd;_tmp506=p2;goto _LL4;}case 1U: _LL3: _tmp505=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1;_tmp506=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4F9)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp505;struct Cyc_Absyn_Pat*p2=_tmp506;
# 3036
struct Cyc_Absyn_Vardecl*_tmp509=({struct _tuple1*_tmpA13=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA13,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp509;
({struct Cyc_List_List*_tmpA15=({struct Cyc_List_List*_tmp50B=_region_malloc(rgn,sizeof(*_tmp50B));({struct _tuple38*_tmpA14=({struct _tuple38*_tmp50A=_region_malloc(rgn,sizeof(*_tmp50A));_tmp50A->f1=vd,_tmp50A->f2=new_vd;_tmp50A;});_tmp50B->hd=_tmpA14;}),_tmp50B->tl=*decls;_tmp50B;});*decls=_tmpA15;});{
struct Cyc_Absyn_Stmt*_tmp50C=({struct Cyc_Absyn_Exp*_tmpA16=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->tag=4U,_tmp50D->f1=new_vd;_tmp50D;}),0U);Cyc_Absyn_assign_stmt(_tmpA16,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp50C;
return({struct Cyc_Absyn_Stmt*_tmpA17=s;Cyc_Toc_seq_stmt_opt(_tmpA17,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4U: _LL5: _tmp504=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4F9)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp504;
# 3042
struct Cyc_Absyn_Vardecl*_tmp50E=({struct _tuple1*_tmpA18=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA18,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp50E;
({struct Cyc_List_List*_tmpA1A=({struct Cyc_List_List*_tmp510=_region_malloc(rgn,sizeof(*_tmp510));({struct _tuple38*_tmpA19=({struct _tuple38*_tmp50F=_region_malloc(rgn,sizeof(*_tmp50F));_tmp50F->f1=vd,_tmp50F->f2=new_vd;_tmp50F;});_tmp510->hd=_tmpA19;}),_tmp510->tl=*decls;_tmp510;});*decls=_tmpA1A;});
return({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->tag=4U,_tmp511->f1=new_vd;_tmp511;}),0U);Cyc_Absyn_assign_stmt(_tmpA1B,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp502=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1;_tmp503=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4F9)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp502;struct Cyc_Absyn_Pat*p2=_tmp503;
# 3048
({void*_tmpA1C=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmpA1C;});{
struct Cyc_Absyn_Vardecl*_tmp512=({struct _tuple1*_tmpA1D=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA1D,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp512;
({struct Cyc_List_List*_tmpA1F=({struct Cyc_List_List*_tmp514=_region_malloc(rgn,sizeof(*_tmp514));({struct _tuple38*_tmpA1E=({struct _tuple38*_tmp513=_region_malloc(rgn,sizeof(*_tmp513));_tmp513->f1=vd,_tmp513->f2=new_vd;_tmp513;});_tmp514->hd=_tmpA1E;}),_tmp514->tl=*decls;_tmp514;});*decls=_tmpA1F;});{
# 3052
struct Cyc_Absyn_Stmt*_tmp515=({struct Cyc_Absyn_Exp*_tmpA22=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->tag=4U,_tmp516->f1=new_vd;_tmp516;}),0U);Cyc_Absyn_assign_stmt(_tmpA22,({
void*_tmpA21=({void*_tmpA20=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA20,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA21,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3052
struct Cyc_Absyn_Stmt*s=_tmp515;
# 3055
return({struct Cyc_Absyn_Stmt*_tmpA23=s;Cyc_Toc_seq_stmt_opt(_tmpA23,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9U: _LLB: _LLC:
# 3057
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1)->r)->tag == 8U){_LL17: _tmp4FF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1)->r)->f1;_tmp500=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1)->r)->f2;_tmp501=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1)->r)->f3;_LL18: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4FF;struct Cyc_Absyn_Datatypefield*tuf=_tmp500;struct Cyc_List_List*ps=_tmp501;
# 3066
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp517=({void*_tmpA24=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA24,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp517;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp518=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp518;
struct Cyc_Absyn_Stmt*s=0;
for(0;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp519=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp519;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp51A=(*((struct _tuple13*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp51A;
void*_tmp51B=(void*)_check_null(p2->topt);void*t2=_tmp51B;
void*_tmp51C=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp51C;
struct Cyc_Absyn_Exp*_tmp51D=({struct Cyc_Absyn_Exp*_tmpA25=path;Cyc_Absyn_aggrarrow_exp(_tmpA25,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp51D;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmpA26=s;Cyc_Toc_seq_stmt_opt(_tmpA26,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3084
return s;}}}}else{_LL21: _tmp4FE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1;_LL22: {struct Cyc_Absyn_Pat*p2=_tmp4FE;
# 3129
return({struct _RegionHandle*_tmpA2A=rgn;struct Cyc_Toc_Env**_tmpA29=nv;struct Cyc_List_List**_tmpA28=decls;struct Cyc_Absyn_Exp*_tmpA27=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA2A,_tmpA29,_tmpA28,_tmpA27,p2);});}}case 8U: _LL19: _tmp4FD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4F9)->f3;_LL1A: {struct Cyc_List_List*ps=_tmp4FD;
# 3086
_tmp4FC=ps;goto _LL1C;}case 5U: _LL1B: _tmp4FC=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp4FC;
# 3088
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp51E=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp51E;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp51F=(void*)_check_null(p2->topt);void*t2=_tmp51F;
struct _fat_ptr*_tmp520=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp520;
s=({struct Cyc_Absyn_Stmt*_tmpA2F=s;Cyc_Toc_seq_stmt_opt(_tmpA2F,({struct _RegionHandle*_tmpA2E=rgn;struct Cyc_Toc_Env**_tmpA2D=nv;struct Cyc_List_List**_tmpA2C=decls;struct Cyc_Absyn_Exp*_tmpA2B=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmpA2E,_tmpA2D,_tmpA2C,_tmpA2B,p2);}));});}}
# 3098
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1 == 0){_LL1D: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp522=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E9;_tmp7E9.tag=0U,({struct _fat_ptr _tmpA30=({const char*_tmp523="unresolved aggregate pattern!";_tag_fat(_tmp523,sizeof(char),30U);});_tmp7E9.f1=_tmpA30;});_tmp7E9;});void*_tmp521[1U];_tmp521[0]=& _tmp522;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp521,sizeof(void*),1U));});}else{_LL1F: _tmp4FA=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4F9)->f1;_tmp4FB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4F9)->f3;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp4FA;struct Cyc_List_List*dlps=_tmp4FB;
# 3102
struct Cyc_Absyn_Aggrdecl*_tmp524=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp524;
struct Cyc_Absyn_Stmt*s=0;
for(0;dlps != 0;dlps=dlps->tl){
struct _tuple39*_tmp525=(struct _tuple39*)dlps->hd;struct _tuple39*tup=_tmp525;
struct Cyc_Absyn_Pat*_tmp526=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp526;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp527=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp527;
void*_tmp528=(void*)_check_null(p2->topt);void*t2=_tmp528;
void*_tmp529=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp529;
struct Cyc_Absyn_Exp*_tmp52A=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp52A;
# 3114
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp52B=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp52B;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(ftype)))
# 3121
memexp=Cyc_Absyn_deref_exp(({void*_tmpA31=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA31,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3124
s=({struct Cyc_Absyn_Stmt*_tmpA32=s;Cyc_Toc_seq_stmt_opt(_tmpA32,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3126
return s;}}case 15U: _LL23: _LL24:
# 3131
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EA;_tmp7EA.tag=0U,({struct _fat_ptr _tmpA33=({const char*_tmp52E="unknownid pat";_tag_fat(_tmp52E,sizeof(char),14U);});_tmp7EA.f1=_tmpA33;});_tmp7EA;});void*_tmp52C[1U];_tmp52C[0]=& _tmp52D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp52C,sizeof(void*),1U));});case 16U: _LL25: _LL26:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp530=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EB;_tmp7EB.tag=0U,({struct _fat_ptr _tmpA34=({const char*_tmp531="unknowncall pat";_tag_fat(_tmp531,sizeof(char),16U);});_tmp7EB.f1=_tmpA34;});_tmp7EB;});void*_tmp52F[1U];_tmp52F[0]=& _tmp530;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp52F,sizeof(void*),1U));});default: _LL27: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp533=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EC;_tmp7EC.tag=0U,({struct _fat_ptr _tmpA35=({const char*_tmp534="exp pat";_tag_fat(_tmp534,sizeof(char),8U);});_tmp7EC.f1=_tmpA35;});_tmp7EC;});void*_tmp532[1U];_tmp532[0]=& _tmp533;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp532,sizeof(void*),1U));});}_LL0:;}
# 3137
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple38 _tmp535=*((struct _tuple38*)vs->hd);struct _tuple38 _stmttmp52=_tmp535;struct _tuple38 _tmp536=_stmttmp52;struct Cyc_Absyn_Vardecl*_tmp538;struct Cyc_Absyn_Vardecl*_tmp537;_LL1: _tmp537=_tmp536.f1;_tmp538=_tmp536.f2;_LL2: {struct Cyc_Absyn_Vardecl*oldv=_tmp537;struct Cyc_Absyn_Vardecl*newv=_tmp538;
if(oldv == newv)return newv;}}
# 3142
({void*_tmp539=0U;({struct _fat_ptr _tmpA36=({const char*_tmp53A="find_new_var";_tag_fat(_tmp53A,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA36,_tag_fat(_tmp539,sizeof(void*),0U));});});}
# 3148
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp53B=e->r;void*_stmttmp53=_tmp53B;void*_tmp53C=_stmttmp53;struct Cyc_List_List*_tmp53D;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp540;struct Cyc_List_List*_tmp541;struct Cyc_List_List*_tmp542;struct Cyc_List_List*_tmp543;struct Cyc_List_List*_tmp544;struct Cyc_Absyn_Exp*_tmp546;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Exp*_tmp547;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Vardecl*_tmp562;struct Cyc_Absyn_Vardecl*_tmp563;switch(*((int*)_tmp53C)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53C)->f1)){case 5U: _LL1: _tmp563=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53C)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp563;
_tmp562=vd;goto _LL4;}case 4U: _LL3: _tmp562=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53C)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp562;
# 3152
{struct _handler_cons _tmp564;_push_handler(& _tmp564);{int _tmp566=0;if(setjmp(_tmp564.handler))_tmp566=1;if(!_tmp566){
({void*_tmpA39=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=1U,({void*_tmpA38=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->tag=4U,({struct Cyc_Absyn_Vardecl*_tmpA37=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,vd);_tmp567->f1=_tmpA37;});_tmp567;});_tmp568->f1=_tmpA38;});_tmp568;});e->r=_tmpA39;});;_pop_handler();}else{void*_tmp565=(void*)Cyc_Core_get_exn_thrown();void*_tmp569=_tmp565;void*_tmp56A;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp569)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3155
 goto _LL3F;}else{_LL42: _tmp56A=_tmp569;_LL43: {void*exn=_tmp56A;(int)_rethrow(exn);}}_LL3F:;}}}
# 3157
goto _LL0;}default: goto _LL3D;}case 6U: _LL5: _tmp55F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp560=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_tmp561=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53C)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp55F;struct Cyc_Absyn_Exp*e2=_tmp560;struct Cyc_Absyn_Exp*e3=_tmp561;
# 3159
Cyc_Toc_subst_pattern_vars(env,e1);_tmp55D=e2;_tmp55E=e3;goto _LL8;}case 27U: _LL7: _tmp55D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_tmp55E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53C)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp55D;struct Cyc_Absyn_Exp*e2=_tmp55E;
_tmp55B=e1;_tmp55C=e2;goto _LLA;}case 7U: _LL9: _tmp55B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp55C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp55B;struct Cyc_Absyn_Exp*e2=_tmp55C;
_tmp559=e1;_tmp55A=e2;goto _LLC;}case 8U: _LLB: _tmp559=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp55A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp559;struct Cyc_Absyn_Exp*e2=_tmp55A;
_tmp557=e1;_tmp558=e2;goto _LLE;}case 23U: _LLD: _tmp557=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp558=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp557;struct Cyc_Absyn_Exp*e2=_tmp558;
_tmp555=e1;_tmp556=e2;goto _LL10;}case 9U: _LLF: _tmp555=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp556=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp555;struct Cyc_Absyn_Exp*e2=_tmp556;
# 3165
Cyc_Toc_subst_pattern_vars(env,e1);_tmp554=e2;goto _LL12;}case 41U: _LL11: _tmp554=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp554;
_tmp553=e;goto _LL14;}case 38U: _LL13: _tmp553=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp553;
_tmp552=e;goto _LL16;}case 11U: _LL15: _tmp552=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp552;
_tmp551=e;goto _LL18;}case 12U: _LL17: _tmp551=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp551;
_tmp550=e;goto _LL1A;}case 13U: _LL19: _tmp550=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp550;
_tmp54F=e;goto _LL1C;}case 14U: _LL1B: _tmp54F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp54F;
_tmp54E=e;goto _LL1E;}case 18U: _LL1D: _tmp54E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp54E;
_tmp54D=e;goto _LL20;}case 20U: _LL1F: _tmp54D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp54D;
_tmp54C=e;goto _LL22;}case 21U: _LL21: _tmp54C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp54C;
_tmp54B=e;goto _LL24;}case 22U: _LL23: _tmp54B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp54B;
_tmp54A=e;goto _LL26;}case 28U: _LL25: _tmp54A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp54A;
_tmp549=e;goto _LL28;}case 15U: _LL27: _tmp549=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp549;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34U: _LL29: _tmp547=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp53C)->f1).rgn;_tmp548=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp53C)->f1).num_elts;_LL2A: {struct Cyc_Absyn_Exp*eopt=_tmp547;struct Cyc_Absyn_Exp*e=_tmp548;
_tmp545=eopt;_tmp546=e;goto _LL2C;}case 16U: _LL2B: _tmp545=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_tmp546=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp545;struct Cyc_Absyn_Exp*e=_tmp546;
# 3180
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3U: _LL2D: _tmp544=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL2E: {struct Cyc_List_List*es=_tmp544;
_tmp543=es;goto _LL30;}case 31U: _LL2F: _tmp543=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL30: {struct Cyc_List_List*es=_tmp543;
_tmp542=es;goto _LL32;}case 24U: _LL31: _tmp542=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL32: {struct Cyc_List_List*es=_tmp542;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36U: _LL33: _tmp541=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL34: {struct Cyc_List_List*dles=_tmp541;
_tmp540=dles;goto _LL36;}case 29U: _LL35: _tmp540=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp53C)->f3;_LL36: {struct Cyc_List_List*dles=_tmp540;
_tmp53F=dles;goto _LL38;}case 30U: _LL37: _tmp53F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL38: {struct Cyc_List_List*dles=_tmp53F;
_tmp53E=dles;goto _LL3A;}case 26U: _LL39: _tmp53E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp53E;
_tmp53D=dles;goto _LL3C;}case 25U: _LL3B: _tmp53D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp53D;
# 3190
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp56B=(struct _tuple20*)dles->hd;struct _tuple20*_stmttmp54=_tmp56B;struct _tuple20*_tmp56C=_stmttmp54;struct Cyc_Absyn_Exp*_tmp56D;_LL45: _tmp56D=_tmp56C->f2;_LL46: {struct Cyc_Absyn_Exp*e=_tmp56D;
Cyc_Toc_subst_pattern_vars(env,e);}}
# 3194
goto _LL0;}default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple40{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3204
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3212
void*_tmp56E=dopt;void*_tmp571;struct Cyc_List_List*_tmp570;struct Cyc_List_List*_tmp56F;struct Cyc_Tcpat_Rhs*_tmp572;if(_tmp56E == 0){_LL1: _LL2:
# 3214
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp56E)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp572=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp56E)->f1;_LL6: {struct Cyc_Tcpat_Rhs*rhs=_tmp572;
# 3218
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp573=(struct _tuple36*)lscs->hd;struct _tuple36*_stmttmp55=_tmp573;struct _tuple36*_tmp574=_stmttmp55;struct Cyc_Absyn_Switch_clause*_tmp578;struct _fat_ptr*_tmp577;struct _fat_ptr*_tmp576;int*_tmp575;_LLA: _tmp575=(int*)& _tmp574->f1;_tmp576=_tmp574->f2;_tmp577=_tmp574->f3;_tmp578=_tmp574->f4;_LLB: {int*already_emitted=_tmp575;struct _fat_ptr*init_lab=_tmp576;struct _fat_ptr*code_lab=_tmp577;struct Cyc_Absyn_Switch_clause*sc=_tmp578;
struct Cyc_Absyn_Stmt*_tmp579=sc->body;struct Cyc_Absyn_Stmt*body=_tmp579;
if(body == rhs->rhs){
# 3223
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3228
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA3B=rgn;struct Cyc_Absyn_Exp*_tmpA3A=
Cyc_Absyn_var_exp(v,0U);
# 3228
Cyc_Toc_extract_pattern_vars(_tmpA3B,& nv,& newdecls,_tmpA3A,sc->pattern);});
# 3232
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp57A=newdecls;struct Cyc_List_List*ds=_tmp57A;for(0;ds != 0;ds=ds->tl){
struct _tuple38 _tmp57B=*((struct _tuple38*)ds->hd);struct _tuple38 _stmttmp56=_tmp57B;struct _tuple38 _tmp57C=_stmttmp56;struct Cyc_Absyn_Vardecl*_tmp57E;struct Cyc_Absyn_Vardecl*_tmp57D;_LLD: _tmp57D=_tmp57C.f1;_tmp57E=_tmp57C.f2;_LLE: {struct Cyc_Absyn_Vardecl*oldv=_tmp57D;struct Cyc_Absyn_Vardecl*newv=_tmp57E;
({struct Cyc_List_List*_tmpA3D=({struct Cyc_List_List*_tmp580=_region_malloc(rgn,sizeof(*_tmp580));({struct _tuple38*_tmpA3C=({struct _tuple38*_tmp57F=_region_malloc(rgn,sizeof(*_tmp57F));_tmp57F->f1=oldv,_tmp57F->f2=newv;_tmp57F;});_tmp580->hd=_tmpA3C;}),_tmp580->tl=*decls;_tmp580;});*decls=_tmpA3D;});
({struct Cyc_Absyn_Exp*_tmpA3E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=4U,_tmp581->f1=newv;_tmp581;}),0U);oldv->initializer=_tmpA3E;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmpA40=({struct Cyc_Absyn_Decl*_tmp583=_cycalloc(sizeof(*_tmp583));({void*_tmpA3F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->tag=0U,_tmp582->f1=oldv;_tmp582;});_tmp583->r=_tmpA3F;}),_tmp583->loc=0U;_tmp583;});Cyc_Absyn_decl_stmt(_tmpA40,res,0U);});}}}
# 3241
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmpA42=({struct Cyc_List_List*_tmp585=_region_malloc(rgn,sizeof(*_tmp585));({struct _tuple40*_tmpA41=({struct _tuple40*_tmp584=_region_malloc(rgn,sizeof(*_tmp584));_tmp584->f1=nv,_tmp584->f2=code_lab,_tmp584->f3=body;_tmp584;});_tmp585->hd=_tmpA41;}),_tmp585->tl=*bodies;_tmp585;});*bodies=_tmpA42;});
return res;}}}}
# 3249
({struct Cyc_Warn_String_Warn_Warg_struct _tmp587=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7ED;_tmp7ED.tag=0U,({struct _fat_ptr _tmpA43=({const char*_tmp588="couldn't find rhs!";_tag_fat(_tmp588,sizeof(char),19U);});_tmp7ED.f1=_tmpA43;});_tmp7ED;});void*_tmp586[1U];_tmp586[0]=& _tmp587;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp586,sizeof(void*),1U));});}default: _LL7: _tmp56F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp56E)->f1;_tmp570=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp56E)->f2;_tmp571=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp56E)->f3;_LL8: {struct Cyc_List_List*symbolic_path=_tmp56F;struct Cyc_List_List*switches=_tmp570;void*other_decision=_tmp571;
# 3252
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3254
struct Cyc_Absyn_Exp*_tmp589=({struct Cyc_List_List*_tmpA44=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmpA44,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp589;
struct Cyc_List_List*_tmp58A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp58A;
# 3257
struct _tuple37 _tmp58B=Cyc_Toc_admits_switch(ss);struct _tuple37 _stmttmp57=_tmp58B;struct _tuple37 _tmp58C=_stmttmp57;void*_tmp58E;int _tmp58D;_LL10: _tmp58D=_tmp58C.f1;_tmp58E=_tmp58C.f2;_LL11: {int allows_switch=_tmp58D;void*test_kind=_tmp58E;
if(allows_switch > 1){
# 3261
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp59F=_cycalloc(sizeof(*_tmp59F));
({struct Cyc_Absyn_Switch_clause*_tmpA46=({struct Cyc_Absyn_Switch_clause*_tmp59E=_cycalloc(sizeof(*_tmp59E));({struct Cyc_Absyn_Pat*_tmpA45=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp59E->pattern=_tmpA45;}),_tmp59E->pat_vars=0,_tmp59E->where_clause=0,_tmp59E->body=res,_tmp59E->loc=0U;_tmp59E;});_tmp59F->hd=_tmpA46;}),_tmp59F->tl=0;_tmp59F;});
# 3264
for(0;ss != 0;ss=ss->tl){
struct _tuple33 _tmp58F=*((struct _tuple33*)ss->hd);struct _tuple33 _stmttmp58=_tmp58F;struct _tuple33 _tmp590=_stmttmp58;void*_tmp592;void*_tmp591;_LL13: _tmp591=_tmp590.f1;_tmp592=_tmp590.f2;_LL14: {void*pat_test=_tmp591;void*dec_tree=_tmp592;
# 3267
struct Cyc_Absyn_Pat*_tmp593=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp593;
# 3269
struct Cyc_Absyn_Stmt*_tmp594=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp594;
# 3271
new_lscs=({struct Cyc_List_List*_tmp596=_cycalloc(sizeof(*_tmp596));({struct Cyc_Absyn_Switch_clause*_tmpA47=({struct Cyc_Absyn_Switch_clause*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->pattern=case_exp,_tmp595->pat_vars=0,_tmp595->where_clause=0,_tmp595->body=s,_tmp595->loc=0U;_tmp595;});_tmp596->hd=_tmpA47;}),_tmp596->tl=new_lscs;_tmp596;});}}
# 3273
{void*_tmp597=test_kind;struct _fat_ptr*_tmp598;switch(*((int*)_tmp597)){case 1U: _LL16: _LL17:
# 3275
 LOOP1: {
void*_tmp599=p->r;void*_stmttmp59=_tmp599;void*_tmp59A=_stmttmp59;struct Cyc_Absyn_Exp*_tmp59B;struct Cyc_Absyn_Exp*_tmp59C;switch(*((int*)_tmp59A)){case 14U: _LL1F: _tmp59C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_LL20: {struct Cyc_Absyn_Exp*e=_tmp59C;
p=e;goto LOOP1;}case 20U: _LL21: _tmp59B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp59B;
p=e;goto _LL1E;}default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3281
p=Cyc_Absyn_deref_exp(({void*_tmpA48=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA48,p);}),0U);goto _LL15;case 3U: _LL18: _tmp598=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp597)->f1;_LL19: {struct _fat_ptr*f=_tmp598;
# 3283
p=({struct Cyc_Absyn_Exp*_tmpA49=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmpA49,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3287
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->tag=10U,_tmp59D->f1=p,_tmp59D->f2=new_lscs,_tmp59D->f3=0;_tmp59D;}),0U);}else{
# 3291
void*_tmp5A0=test_kind;struct Cyc_Absyn_Exp*_tmp5A1;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A0)->tag == 2U){_LL26: _tmp5A1=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A0)->f1;_LL27: {struct Cyc_Absyn_Exp*e=_tmp5A1;
# 3293
struct Cyc_List_List*_tmp5A2=ss;struct Cyc_Tcpat_Rhs*_tmp5A4;void*_tmp5A3;if(_tmp5A2 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5A2)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5A2)->tl == 0){_LL2B: _tmp5A3=((struct _tuple33*)_tmp5A2->hd)->f1;_tmp5A4=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5A2->hd)->f2)->f1;_LL2C: {void*pat_test=_tmp5A3;struct Cyc_Tcpat_Rhs*rhs=_tmp5A4;
# 3299
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp5A5=(struct _tuple36*)lscs->hd;struct _tuple36*_stmttmp5A=_tmp5A5;struct _tuple36*_tmp5A6=_stmttmp5A;struct Cyc_Absyn_Switch_clause*_tmp5AA;struct _fat_ptr*_tmp5A9;struct _fat_ptr*_tmp5A8;int*_tmp5A7;_LL30: _tmp5A7=(int*)& _tmp5A6->f1;_tmp5A8=_tmp5A6->f2;_tmp5A9=_tmp5A6->f3;_tmp5AA=_tmp5A6->f4;_LL31: {int*already_emitted=_tmp5A7;struct _fat_ptr*init_lab=_tmp5A8;struct _fat_ptr*code_lab=_tmp5A9;struct Cyc_Absyn_Switch_clause*sc=_tmp5AA;
struct Cyc_Absyn_Stmt*_tmp5AB=sc->body;struct Cyc_Absyn_Stmt*body=_tmp5AB;
if(body == rhs->rhs){
# 3304
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3309
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA4B=rgn;struct Cyc_Absyn_Exp*_tmpA4A=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA4B,& nv,& newdecls,_tmpA4A,sc->pattern);});
# 3313
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp5AC=newdecls;struct Cyc_List_List*ds=_tmp5AC;for(0;ds != 0;ds=ds->tl){
struct _tuple38 _tmp5AD=*((struct _tuple38*)ds->hd);struct _tuple38 _stmttmp5B=_tmp5AD;struct _tuple38 _tmp5AE=_stmttmp5B;struct Cyc_Absyn_Vardecl*_tmp5B0;struct Cyc_Absyn_Vardecl*_tmp5AF;_LL33: _tmp5AF=_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;_LL34: {struct Cyc_Absyn_Vardecl*oldv=_tmp5AF;struct Cyc_Absyn_Vardecl*newv=_tmp5B0;
({struct Cyc_List_List*_tmpA4D=({struct Cyc_List_List*_tmp5B2=_region_malloc(rgn,sizeof(*_tmp5B2));({struct _tuple38*_tmpA4C=({struct _tuple38*_tmp5B1=_region_malloc(rgn,sizeof(*_tmp5B1));_tmp5B1->f1=oldv,_tmp5B1->f2=newv;_tmp5B1;});_tmp5B2->hd=_tmpA4C;}),_tmp5B2->tl=*decls;_tmp5B2;});*decls=_tmpA4D;});
({struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->tag=4U,_tmp5B3->f1=newv;_tmp5B3;}),0U);oldv->initializer=_tmpA4E;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmpA50=({struct Cyc_Absyn_Decl*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));({void*_tmpA4F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->tag=0U,_tmp5B4->f1=oldv;_tmp5B4;});_tmp5B5->r=_tmpA4F;}),_tmp5B5->loc=0U;_tmp5B5;});Cyc_Absyn_decl_stmt(_tmpA50,r,0U);});}}}
# 3322
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3326
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmpA52=({struct Cyc_List_List*_tmp5B7=_region_malloc(rgn,sizeof(*_tmp5B7));({struct _tuple40*_tmpA51=({struct _tuple40*_tmp5B6=_region_malloc(rgn,sizeof(*_tmp5B6));_tmp5B6->f1=nv,_tmp5B6->f2=code_lab,_tmp5B6->f3=body;_tmp5B6;});_tmp5B7->hd=_tmpA51;}),_tmp5B7->tl=*bodies;_tmp5B7;});*bodies=_tmpA52;});
return r;}}}}
# 3336
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EE;_tmp7EE.tag=0U,({struct _fat_ptr _tmpA53=({const char*_tmp5BA="couldn't find rhs!";_tag_fat(_tmp5BA,sizeof(char),19U);});_tmp7EE.f1=_tmpA53;});_tmp7EE;});void*_tmp5B8[1U];_tmp5B8[0]=& _tmp5B9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5B8,sizeof(void*),1U));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EF;_tmp7EF.tag=0U,({struct _fat_ptr _tmpA54=({const char*_tmp5BD="bad where clause in match compiler";_tag_fat(_tmp5BD,sizeof(char),35U);});_tmp7EF.f1=_tmpA54;});_tmp7EF;});void*_tmp5BB[1U];_tmp5BB[0]=& _tmp5BC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5BB,sizeof(void*),1U));});}_LL2A:;}}else{_LL28: _LL29:
# 3341
 for(0;ss != 0;ss=ss->tl){
struct _tuple33 _tmp5BE=*((struct _tuple33*)ss->hd);struct _tuple33 _stmttmp5C=_tmp5BE;struct _tuple33 _tmp5BF=_stmttmp5C;void*_tmp5C1;void*_tmp5C0;_LL36: _tmp5C0=_tmp5BF.f1;_tmp5C1=_tmp5BF.f2;_LL37: {void*pat_test=_tmp5C0;void*dec_tree=_tmp5C1;
struct Cyc_Absyn_Exp*_tmp5C2=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp5C2;
struct Cyc_Absyn_Stmt*_tmp5C3=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp5C3;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}}_LL25:;}
# 3349
return res;}}}}_LL0:;}
# 3359
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3361
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple40*_tmp5C4=(struct _tuple40*)bodies->hd;struct _tuple40*_stmttmp5D=_tmp5C4;struct _tuple40*_tmp5C5=_stmttmp5D;struct Cyc_Absyn_Stmt*_tmp5C7;struct Cyc_Toc_Env**_tmp5C6;_LL1: _tmp5C6=(struct Cyc_Toc_Env**)& _tmp5C5->f1;_tmp5C7=_tmp5C5->f3;_LL2: {struct Cyc_Toc_Env**nv=_tmp5C6;struct Cyc_Absyn_Stmt*s2=_tmp5C7;
if(s2 == s)return nv;}}
# 3367
return 0;}
# 3371
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3374
void*_tmp5C8=(void*)_check_null(e->topt);void*t=_tmp5C8;
Cyc_Toc_exp_to_c(nv,e);{
# 3377
struct _tuple30 _tmp5C9=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp5E=_tmp5C9;struct _tuple30 _tmp5CA=_stmttmp5E;struct Cyc_Absyn_Exp*_tmp5CC;struct _tuple1*_tmp5CB;_LL1: _tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;_LL2: {struct _tuple1*v=_tmp5CB;struct Cyc_Absyn_Exp*path=_tmp5CC;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5CD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5CD;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5CE=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp5CE;
# 3383
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3388
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3396
{struct Cyc_List_List*_tmp5CF=lscs;struct Cyc_List_List*lscs2=_tmp5CF;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple36*_tmp5D0=(struct _tuple36*)lscs2->hd;struct _tuple36*_stmttmp5F=_tmp5D0;struct _tuple36*_tmp5D1=_stmttmp5F;struct Cyc_Absyn_Switch_clause*_tmp5D3;struct _fat_ptr*_tmp5D2;_LL4: _tmp5D2=_tmp5D1->f3;_tmp5D3=_tmp5D1->f4;_LL5: {struct _fat_ptr*body_lab=_tmp5D2;struct Cyc_Absyn_Switch_clause*body_sc=_tmp5D3;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3400
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3403
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5D4=*envp;struct Cyc_Toc_Env*env=_tmp5D4;
# 3406
if(lscs2->tl != 0){
struct _tuple36*_tmp5D5=(struct _tuple36*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple36*_stmttmp60=_tmp5D5;struct _tuple36*_tmp5D6=_stmttmp60;struct Cyc_Absyn_Switch_clause*_tmp5D8;struct _fat_ptr*_tmp5D7;_LL7: _tmp5D7=_tmp5D6->f3;_tmp5D8=_tmp5D6->f4;_LL8: {struct _fat_ptr*fallthru_lab=_tmp5D7;struct Cyc_Absyn_Switch_clause*next_sc=_tmp5D8;
# 3410
struct Cyc_Toc_Env**_tmp5D9=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp5D9;
# 3415
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmpA55=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA55,s);});else{
# 3419
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmpA56=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmpA56,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3424
({struct Cyc_Toc_Env*_tmpA57=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmpA57,s);});}}}else{
# 3428
({struct Cyc_Toc_Env*_tmpA58=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA58,s);});}}}}}{
# 3432
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA5A=test_tree;Cyc_Absyn_seq_stmt(_tmpA5A,({struct _fat_ptr*_tmpA59=end_l;Cyc_Absyn_label_stmt(_tmpA59,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3434
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple38 _tmp5DA=*((struct _tuple38*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple38 _stmttmp61=_tmp5DA;struct _tuple38 _tmp5DB=_stmttmp61;struct Cyc_Absyn_Vardecl*_tmp5DC;_LLA: _tmp5DC=_tmp5DB.f2;_LLB: {struct Cyc_Absyn_Vardecl*vd=_tmp5DC;
res=({struct Cyc_Absyn_Decl*_tmpA5B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->tag=0U,_tmp5DD->f1=vd;_tmp5DD;}),0U);Cyc_Absyn_decl_stmt(_tmpA5B,res,0U);});}}
# 3439
({void*_tmpA5F=({struct _tuple1*_tmpA5E=v;void*_tmpA5D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA5C=e;Cyc_Absyn_declare_stmt(_tmpA5E,_tmpA5D,_tmpA5C,res,0U);})->r;whole_s->r=_tmpA5F;});
_npop_handler(0U);return;}}
# 3380
;_pop_region(rgn);}}}
# 3445
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3447
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3452
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3454
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5DE[1U];({struct Cyc_Absyn_Exp*_tmpA60=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp5DE[0]=_tmpA60;});({struct Cyc_Absyn_Exp*_tmpA61=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA61,_tag_fat(_tmp5DE,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3457
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA63=({struct Cyc_Absyn_Stmt*_tmpA62=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA62,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA63;});}
# 3462
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3464
while(1){
void*_tmp5DF=s->r;void*_stmttmp62=_tmp5DF;void*_tmp5E0=_stmttmp62;void*_tmp5E3;struct Cyc_List_List*_tmp5E2;struct Cyc_Absyn_Stmt*_tmp5E1;struct Cyc_Absyn_Stmt*_tmp5E5;struct Cyc_Absyn_Decl*_tmp5E4;struct Cyc_Absyn_Switch_clause**_tmp5E7;struct Cyc_List_List*_tmp5E6;void*_tmp5EA;struct Cyc_List_List*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Stmt*_tmp5EC;struct _fat_ptr*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5EE;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Stmt*_tmp5EF;struct Cyc_Absyn_Stmt*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Stmt*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5FB;switch(*((int*)_tmp5E0)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp5FB=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5FB;
Cyc_Toc_exp_to_c(nv,e);return;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1 == 0){_LL5: _LL6:
({int _tmpA64=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA64,s);});return;}else{_LL7: _tmp5FA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp5FA;
# 3471
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp5FC=Cyc_Toc_temp_var();struct _tuple1*x=_tmp5FC;
struct Cyc_Absyn_Stmt*_tmp5FD=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp5FD;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmpA65=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmpA65;});}
# 3480
return;}}}case 2U: _LL9: _tmp5F8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5F9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_LLA: {struct Cyc_Absyn_Stmt*s1=_tmp5F8;struct Cyc_Absyn_Stmt*s2=_tmp5F9;
# 3482
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4U: _LLB: _tmp5F5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5F6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_tmp5F7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5F5;struct Cyc_Absyn_Stmt*s1=_tmp5F6;struct Cyc_Absyn_Stmt*s2=_tmp5F7;
# 3486
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9U: _LLD: _tmp5F1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5F2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2).f1;_tmp5F3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f3).f1;_tmp5F4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5F1;struct Cyc_Absyn_Exp*e2=_tmp5F2;struct Cyc_Absyn_Exp*e3=_tmp5F3;struct Cyc_Absyn_Stmt*s2=_tmp5F4;
# 3491
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5EF=s2;_tmp5F0=e3;goto _LL10;}case 14U: _LLF: _tmp5EF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5F0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5EF;struct Cyc_Absyn_Exp*e=_tmp5F0;
_tmp5ED=e;_tmp5EE=s2;goto _LL12;}case 5U: _LL11: _tmp5ED=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1).f1;_tmp5EE=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5ED;struct Cyc_Absyn_Stmt*s2=_tmp5EE;
# 3496
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp5FE=_new_region("temp");struct _RegionHandle*temp=& _tmp5FE;_push_region(temp);({struct Cyc_Toc_Env*_tmpA66=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA66,s2);});
_npop_handler(0U);return;
# 3497
;_pop_region(temp);}}case 6U: _LL13: _LL14: {
# 3500
struct Cyc_Toc_Env*_tmp5FF=nv;struct _fat_ptr**_tmp600;_LL24: _tmp600=_tmp5FF->break_lab;_LL25: {struct _fat_ptr**b=_tmp600;
if(b != 0)
({void*_tmpA67=Cyc_Toc_goto_stmt_r(*b);s->r=_tmpA67;});
# 3504
({int _tmpA68=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA68,s);});
return;}}case 7U: _LL15: _LL16: {
# 3507
struct Cyc_Toc_Env*_tmp601=nv;struct _fat_ptr**_tmp602;_LL27: _tmp602=_tmp601->continue_lab;_LL28: {struct _fat_ptr**c=_tmp602;
if(c != 0)
({void*_tmpA69=Cyc_Toc_goto_stmt_r(*c);s->r=_tmpA69;});
goto _LL18;}}case 8U: _LL17: _LL18:
# 3512
({int _tmpA6A=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA6A,s);});
return;case 13U: _LL19: _tmp5EB=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5EC=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_LL1A: {struct _fat_ptr*lab=_tmp5EB;struct Cyc_Absyn_Stmt*s1=_tmp5EC;
s=s1;continue;}case 10U: _LL1B: _tmp5E8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5E9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_tmp5EA=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f3;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5E8;struct Cyc_List_List*scs=_tmp5E9;void*dec_tree_opt=_tmp5EA;
# 3516
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11U: _LL1D: _tmp5E6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5E7=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_LL1E: {struct Cyc_List_List*es=_tmp5E6;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5E7;
# 3519
struct Cyc_Toc_Env*_tmp603=nv;struct Cyc_Toc_FallthruInfo*_tmp604;_LL2A: _tmp604=_tmp603->fallthru_info;_LL2B: {struct Cyc_Toc_FallthruInfo*fi=_tmp604;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp606=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F0;_tmp7F0.tag=0U,({struct _fat_ptr _tmpA6B=({const char*_tmp607="fallthru in unexpected place";_tag_fat(_tmp607,sizeof(char),29U);});_tmp7F0.f1=_tmpA6B;});_tmp7F0;});void*_tmp605[1U];_tmp605[0]=& _tmp606;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp605,sizeof(void*),1U));});{
struct Cyc_Toc_FallthruInfo _tmp608=*fi;struct Cyc_Toc_FallthruInfo _stmttmp63=_tmp608;struct Cyc_Toc_FallthruInfo _tmp609=_stmttmp63;struct Cyc_List_List*_tmp60B;struct _fat_ptr*_tmp60A;_LL2D: _tmp60A=_tmp609.label;_tmp60B=_tmp609.binders;_LL2E: {struct _fat_ptr*l=_tmp60A;struct Cyc_List_List*vs=_tmp60B;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3525
({int _tmpA6C=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA6C,s2);});{
struct Cyc_List_List*_tmp60C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp60C;
struct Cyc_List_List*_tmp60D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp60D;
for(0;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmpA6E=({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->tag=5U,_tmp60E->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp60E;}),0U);Cyc_Absyn_assign_stmt(_tmpA6D,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA6E,s2,0U);});}
# 3533
s->r=s2->r;
return;}}}}}case 12U: _LL1F: _tmp5E4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5E5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp5E4;struct Cyc_Absyn_Stmt*s1=_tmp5E5;
# 3539
{void*_tmp60F=d->r;void*_stmttmp64=_tmp60F;void*_tmp610=_stmttmp64;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Vardecl*_tmp612;struct Cyc_Absyn_Tvar*_tmp611;struct Cyc_Absyn_Fndecl*_tmp614;struct Cyc_List_List*_tmp615;void*_tmp618;struct Cyc_Absyn_Exp*_tmp617;struct Cyc_Absyn_Pat*_tmp616;struct Cyc_Absyn_Vardecl*_tmp619;switch(*((int*)_tmp610)){case 0U: _LL30: _tmp619=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp610)->f1;_LL31: {struct Cyc_Absyn_Vardecl*vd=_tmp619;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2U: _LL32: _tmp616=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp610)->f1;_tmp617=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp610)->f3;_tmp618=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp610)->f4;_LL33: {struct Cyc_Absyn_Pat*p=_tmp616;struct Cyc_Absyn_Exp*e=_tmp617;void*dec_tree=_tmp618;
# 3547
{void*_tmp61A=p->r;void*_stmttmp65=_tmp61A;void*_tmp61B=_stmttmp65;struct Cyc_Absyn_Vardecl*_tmp61C;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp61B)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp61B)->f2)->r)->tag == 0U){_LL3D: _tmp61C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp61B)->f1;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp61C;
# 3549
struct Cyc_Absyn_Vardecl*_tmp61D=({struct _tuple1*_tmpA70=Cyc_Toc_temp_var();void*_tmpA6F=vd->type;Cyc_Absyn_new_vardecl(0U,_tmpA70,_tmpA6F,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp61D;
({struct Cyc_Absyn_Exp*_tmpA71=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tag=4U,_tmp61E->f1=new_vd;_tmp61E;}),0U);vd->initializer=_tmpA71;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmpA78=({struct Cyc_Absyn_Decl*_tmpA77=({void*_tmpA72=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->tag=0U,_tmp61F->f1=new_vd;_tmp61F;});Cyc_Absyn_new_decl(_tmpA72,s->loc);});struct Cyc_Absyn_Stmt*_tmpA76=({
struct Cyc_Absyn_Decl*_tmpA75=({void*_tmpA73=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=0U,_tmp620->f1=vd;_tmp620;});Cyc_Absyn_new_decl(_tmpA73,s->loc);});struct Cyc_Absyn_Stmt*_tmpA74=s1;Cyc_Absyn_decl_stmt(_tmpA75,_tmpA74,s->loc);});
# 3552
Cyc_Absyn_decl_stmt(_tmpA77,_tmpA76,s->loc);})->r;s->r=_tmpA78;});
# 3555
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3561
({void*_tmpA79=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmpA79;});
goto _LL3C;}_LL3C:;}
# 3564
goto _LL2F;}case 3U: _LL34: _tmp615=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp610)->f1;_LL35: {struct Cyc_List_List*vds=_tmp615;
# 3568
struct Cyc_List_List*_tmp621=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp621;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp623=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F1;_tmp7F1.tag=0U,({struct _fat_ptr _tmpA7A=({const char*_tmp624="empty Letv_d";_tag_fat(_tmp624,sizeof(char),13U);});_tmp7F1.f1=_tmpA7A;});_tmp7F1;});void*_tmp622[1U];_tmp622[0]=& _tmp623;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp622,sizeof(void*),1U));});
({void*_tmpA7B=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=0U,_tmp625->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp625;});d->r=_tmpA7B;});
rvds=rvds->tl;
for(0;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp626=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=0U,_tmp627->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp627;}),0U);struct Cyc_Absyn_Decl*d2=_tmp626;
({void*_tmpA7D=({struct Cyc_Absyn_Decl*_tmpA7C=d2;Cyc_Absyn_decl_stmt(_tmpA7C,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA7D;});}
# 3577
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp614=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp610)->f1;_LL37: {struct Cyc_Absyn_Fndecl*fd=_tmp614;
# 3580
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4U: _LL38: _tmp611=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp610)->f1;_tmp612=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp610)->f2;_tmp613=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp610)->f3;_LL39: {struct Cyc_Absyn_Tvar*tv=_tmp611;struct Cyc_Absyn_Vardecl*vd=_tmp612;struct Cyc_Absyn_Exp*open_exp_opt=_tmp613;
# 3584
struct Cyc_Absyn_Stmt*_tmp628=s1;struct Cyc_Absyn_Stmt*body=_tmp628;
# 3586
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp629=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp66=_tmp629;struct _tuple30 _tmp62A=_stmttmp66;struct Cyc_Absyn_Exp*_tmp62C;struct _tuple1*_tmp62B;_LL42: _tmp62B=_tmp62A.f1;_tmp62C=_tmp62A.f2;_LL43: {struct _tuple1*rh_var=_tmp62B;struct Cyc_Absyn_Exp*rh_exp=_tmp62C;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3592
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Absyn_no_regions)
({void*_tmpA81=({struct _tuple1*_tmpA80=x_var;void*_tmpA7F=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA7E=
Cyc_Absyn_uint_exp(0U,0U);
# 3594
Cyc_Absyn_declare_stmt(_tmpA80,_tmpA7F,_tmpA7E,body,0U);})->r;s->r=_tmpA81;});else{
# 3596
if((unsigned)open_exp_opt){
# 3598
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA83=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA83,({struct _fat_ptr*_tmp62E=_cycalloc(sizeof(*_tmp62E));({struct _fat_ptr _tmpA82=({const char*_tmp62D="h";_tag_fat(_tmp62D,sizeof(char),2U);});*_tmp62E=_tmpA82;});_tmp62E;}),0U);}),0U);
({void*_tmpA84=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA84;});}}else{
# 3607
({void*_tmpA91=({struct _tuple1*_tmpA90=rh_var;void*_tmpA8F=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA8E=({struct Cyc_Absyn_Exp*_tmp62F[1U];({struct Cyc_Absyn_Exp*_tmpA85=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp62F[0]=_tmpA85;});({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA86,_tag_fat(_tmp62F,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3607
Cyc_Absyn_declare_stmt(_tmpA90,_tmpA8F,_tmpA8E,({
# 3609
struct _tuple1*_tmpA8D=x_var;void*_tmpA8C=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA8D,_tmpA8C,_tmpA8B,({
struct Cyc_Absyn_Stmt*_tmpA8A=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp630[1U];_tmp630[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA87=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA87,_tag_fat(_tmp630,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA8A,({
struct Cyc_Absyn_Stmt*_tmpA89=body;Cyc_Absyn_seq_stmt(_tmpA89,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp631[1U];_tmp631[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA88=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA88,_tag_fat(_tmp631,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3607
s->r=_tmpA91;});}}
# 3615
return;}}default: _LL3A: _LL3B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp633=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F2;_tmp7F2.tag=0U,({struct _fat_ptr _tmpA92=({const char*_tmp634="bad nested declaration within function";_tag_fat(_tmp634,sizeof(char),39U);});_tmp7F2.f1=_tmpA92;});_tmp7F2;});void*_tmp632[1U];_tmp632[0]=& _tmp633;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp632,sizeof(void*),1U));});}_LL2F:;}
# 3618
return;}default: _LL21: _tmp5E1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f1;_tmp5E2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f2;_tmp5E3=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E0)->f3;_LL22: {struct Cyc_Absyn_Stmt*body=_tmp5E1;struct Cyc_List_List*scs=_tmp5E2;void*dec_tree=_tmp5E3;
# 3632 "toc.cyc"
struct _tuple30 _tmp635=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp67=_tmp635;struct _tuple30 _tmp636=_stmttmp67;struct Cyc_Absyn_Exp*_tmp638;struct _tuple1*_tmp637;_LL45: _tmp637=_tmp636.f1;_tmp638=_tmp636.f2;_LL46: {struct _tuple1*h_var=_tmp637;struct Cyc_Absyn_Exp*h_exp=_tmp638;
struct _tuple30 _tmp639=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp68=_tmp639;struct _tuple30 _tmp63A=_stmttmp68;struct Cyc_Absyn_Exp*_tmp63C;struct _tuple1*_tmp63B;_LL48: _tmp63B=_tmp63A.f1;_tmp63C=_tmp63A.f2;_LL49: {struct _tuple1*e_var=_tmp63B;struct Cyc_Absyn_Exp*e_exp=_tmp63C;
struct _tuple30 _tmp63D=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp69=_tmp63D;struct _tuple30 _tmp63E=_stmttmp69;struct Cyc_Absyn_Exp*_tmp640;struct _tuple1*_tmp63F;_LL4B: _tmp63F=_tmp63E.f1;_tmp640=_tmp63E.f2;_LL4C: {struct _tuple1*was_thrown_var=_tmp63F;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp640;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3639
e_exp->topt=e_typ;{
struct _RegionHandle _tmp641=_new_region("temp");struct _RegionHandle*temp=& _tmp641;_push_region(temp);
# 3642
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp642=({struct Cyc_Absyn_Stmt*_tmpA94=body;Cyc_Absyn_seq_stmt(_tmpA94,
Cyc_Absyn_exp_stmt(({void*_tmp64B=0U;({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA93,_tag_fat(_tmp64B,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3643
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp642;
# 3646
struct Cyc_Absyn_Stmt*_tmp643=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->tag=10U,_tmp64A->f1=e_exp,_tmp64A->f2=scs,_tmp64A->f3=dec_tree;_tmp64A;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp643;
# 3648
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3651
struct Cyc_Absyn_Exp*_tmp644=({struct Cyc_Absyn_Exp*_tmp649[1U];({struct Cyc_Absyn_Exp*_tmpA95=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp649[0]=_tmpA95;});({struct Cyc_Absyn_Exp*_tmpA96=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpA96,_tag_fat(_tmp649,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3651
struct Cyc_Absyn_Exp*setjmp_call=_tmp644;
# 3653
struct Cyc_Absyn_Stmt*_tmp645=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp648[1U];({struct Cyc_Absyn_Exp*_tmpA97=Cyc_Absyn_address_exp(h_exp,0U);_tmp648[0]=_tmpA97;});({struct Cyc_Absyn_Exp*_tmpA98=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA98,_tag_fat(_tmp648,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
# 3653
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp645;
# 3655
struct Cyc_Absyn_Exp*_tmp646=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp646;
struct Cyc_Absyn_Exp*_tmp647=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp647;
({void*_tmpAA8=({
struct _tuple1*_tmpAA7=h_var;void*_tmpAA6=h_typ;Cyc_Absyn_declare_stmt(_tmpAA7,_tmpAA6,0,({
struct Cyc_Absyn_Stmt*_tmpAA5=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpAA5,({
struct _tuple1*_tmpAA4=was_thrown_var;void*_tmpAA3=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpAA2=zero_exp;Cyc_Absyn_declare_stmt(_tmpAA4,_tmpAA3,_tmpAA2,({
struct Cyc_Absyn_Stmt*_tmpAA1=({struct Cyc_Absyn_Exp*_tmpA9A=setjmp_call;struct Cyc_Absyn_Stmt*_tmpA99=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3661
Cyc_Absyn_ifthenelse_stmt(_tmpA9A,_tmpA99,
# 3663
Cyc_Toc_skip_stmt_dl(),0U);});
# 3661
Cyc_Absyn_seq_stmt(_tmpAA1,({
# 3664
struct Cyc_Absyn_Exp*_tmpAA0=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpA9F=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpAA0,_tmpA9F,({
# 3666
struct _tuple1*_tmpA9E=e_var;void*_tmpA9D=e_typ;struct Cyc_Absyn_Exp*_tmpA9C=({
void*_tmpA9B=e_typ;Cyc_Toc_cast_it(_tmpA9B,Cyc_Toc_get_exn_thrown_expression());});
# 3666
Cyc_Absyn_declare_stmt(_tmpA9E,_tmpA9D,_tmpA9C,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3657
s->r=_tmpAA8;});}}
# 3670
_npop_handler(0U);return;
# 3640
;_pop_region(temp);}}}}}}_LL0:;}}
# 3679
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpAA9=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpAA9;});
({void*_tmpAAA=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpAAA;});{
struct _RegionHandle _tmp64C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp64C;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp64D=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp64D;
{struct Cyc_List_List*_tmp64E=(f->i).args;struct Cyc_List_List*args=_tmp64E;for(0;args != 0;args=args->tl){
struct _tuple1*_tmp64F=({struct _tuple1*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->f1=Cyc_Absyn_Loc_n,_tmp650->f2=(*((struct _tuple9*)args->hd)).f1;_tmp650;});struct _tuple1*x=_tmp64F;
({void*_tmpAAB=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAAB;});}}
# 3696
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3700
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp651=_cycalloc(sizeof(*_tmp651));({struct Cyc_Hashtable_Table*_tmpAAC=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp651=_tmpAAC;});_tmp651;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp652=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6A=_tmp652;struct Cyc_Absyn_VarargInfo _tmp653=_stmttmp6A;int _tmp657;void*_tmp656;struct Cyc_Absyn_Tqual _tmp655;struct _fat_ptr*_tmp654;_LL1: _tmp654=_tmp653.name;_tmp655=_tmp653.tq;_tmp656=_tmp653.type;_tmp657=_tmp653.inject;_LL2: {struct _fat_ptr*n=_tmp654;struct Cyc_Absyn_Tqual tq=_tmp655;void*t=_tmp656;int i=_tmp657;
void*_tmp658=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type));void*t2=_tmp658;
struct _tuple1*_tmp659=({struct _tuple1*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->f1=Cyc_Absyn_Loc_n,_tmp65C->f2=(struct _fat_ptr*)_check_null(n);_tmp65C;});struct _tuple1*x2=_tmp659;
({struct Cyc_List_List*_tmpAAF=({struct Cyc_List_List*_tmpAAE=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAAE,({struct Cyc_List_List*_tmp65B=_cycalloc(sizeof(*_tmp65B));({struct _tuple9*_tmpAAD=({struct _tuple9*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->f1=n,_tmp65A->f2=tq,_tmp65A->f3=t2;_tmp65A;});_tmp65B->hd=_tmpAAD;}),_tmp65B->tl=0;_tmp65B;}));});(f->i).args=_tmpAAF;});
(f->i).cyc_varargs=0;}}
# 3709
{struct Cyc_List_List*_tmp65D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp65D;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpAB0=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpAB0;});}}
({struct Cyc_Toc_Env*_tmpAB1=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpAB1,f->body);});}
# 3689
;_pop_region(frgn);}}
# 3714
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp65E=s;switch(_tmp65E){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3730 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 3732
struct _tuple41 _tmp65F=*env;struct _tuple41 _stmttmp6B=_tmp65F;struct _tuple41 _tmp660=_stmttmp6B;struct Cyc_Absyn_Aggrdecl*_tmp662;struct Cyc_Toc_TocState*_tmp661;_LL1: _tmp661=_tmp660.f1;_tmp662=*_tmp660.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp661;struct Cyc_Absyn_Aggrdecl*ad=_tmp662;
struct _tuple1*_tmp663=ad->name;struct _tuple1*n=_tmp663;
struct Cyc_Toc_TocState _tmp664=*s;struct Cyc_Toc_TocState _stmttmp6C=_tmp664;struct Cyc_Toc_TocState _tmp665=_stmttmp6C;struct Cyc_Dict_Dict*_tmp666;_LL4: _tmp666=_tmp665.aggrs_so_far;_LL5: {struct Cyc_Dict_Dict*aggrs_so_far=_tmp666;
int seen_defn_before;
struct _tuple19**_tmp667=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple19**dopt=_tmp667;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple19*v;
if((int)ad->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple19*_tmp668=_region_malloc(d,sizeof(*_tmp668));_tmp668->f1=ad,({void*_tmpAB2=Cyc_Absyn_strctq(n);_tmp668->f2=_tmpAB2;});_tmp668;});else{
# 3743
v=({struct _tuple19*_tmp669=_region_malloc(d,sizeof(*_tmp669));_tmp669->f1=ad,({void*_tmpAB3=Cyc_Absyn_unionq_type(n);_tmp669->f2=_tmpAB3;});_tmp669;});}
({struct Cyc_Dict_Dict _tmpAB4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpAB4;});}}else{
# 3746
struct _tuple19*_tmp66A=*dopt;struct _tuple19*_stmttmp6D=_tmp66A;struct _tuple19*_tmp66B=_stmttmp6D;void*_tmp66D;struct Cyc_Absyn_Aggrdecl*_tmp66C;_LL7: _tmp66C=_tmp66B->f1;_tmp66D=_tmp66B->f2;_LL8: {struct Cyc_Absyn_Aggrdecl*ad2=_tmp66C;void*t=_tmp66D;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpAB7=({struct Cyc_Dict_Dict _tmpAB6=*aggrs_so_far;struct _tuple1*_tmpAB5=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpAB6,_tmpAB5,({struct _tuple19*_tmp66E=_region_malloc(d,sizeof(*_tmp66E));_tmp66E->f1=ad,_tmp66E->f2=t;_tmp66E;}));});*aggrs_so_far=_tmpAB7;});
seen_defn_before=0;}else{
# 3751
seen_defn_before=1;}}}{
# 3753
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->kind=ad->kind,_tmp693->sc=Cyc_Absyn_Public,_tmp693->name=ad->name,_tmp693->tvs=0,_tmp693->impl=0,_tmp693->expected_mem_kind=0,_tmp693->attributes=ad->attributes;_tmp693;});
# 3760
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpAB8=({struct Cyc_Absyn_AggrdeclImpl*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->exist_vars=0,_tmp66F->rgn_po=0,_tmp66F->fields=0,_tmp66F->tagged=0;_tmp66F;});new_ad->impl=_tmpAB8;});{
# 3765
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp670=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp670;for(0;fields != 0;fields=fields->tl){
# 3769
struct Cyc_Absyn_Aggrfield*_tmp671=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp671;
void*_tmp672=old_field->type;void*old_type=_tmp672;
struct Cyc_List_List*_tmp673=old_field->attributes;struct Cyc_List_List*old_atts=_tmp673;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3783 "toc.cyc"
void*_tmp674=Cyc_Tcutil_compress(old_type);void*_stmttmp6E=_tmp674;void*_tmp675=_stmttmp6E;unsigned _tmp679;void*_tmp678;struct Cyc_Absyn_Tqual _tmp677;void*_tmp676;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->tag == 4U){_LLA: _tmp676=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).elt_type;_tmp677=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).tq;_tmp678=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).zero_term;_tmp679=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp675)->f1).zt_loc;_LLB: {void*et=_tmp676;struct Cyc_Absyn_Tqual tq=_tmp677;void*zt=_tmp678;unsigned ztl=_tmp679;
# 3786
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->tag=4U,(_tmp67A->f1).elt_type=et,(_tmp67A->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpAB9=Cyc_Absyn_uint_exp(0U,0U);(_tmp67A->f1).num_elts=_tmpAB9;}),(_tmp67A->f1).zero_term=zt,(_tmp67A->f1).zt_loc=ztl;_tmp67A;});
goto _LL9;}}else{_LLC: _LLD:
# 3789
 old_atts=({struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*_tmp67C));({void*_tmpABA=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->tag=6U,_tmp67B->f1=0;_tmp67B;});_tmp67C->hd=_tmpABA;}),_tmp67C->tl=old_atts;_tmp67C;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->tag=4U,({void*_tmpABD=Cyc_Toc_void_star_type();(_tmp67D->f1).elt_type=_tmpABD;}),({
struct Cyc_Absyn_Tqual _tmpABC=Cyc_Absyn_empty_tqual(0U);(_tmp67D->f1).tq=_tmpABC;}),({
struct Cyc_Absyn_Exp*_tmpABB=Cyc_Absyn_uint_exp(0U,0U);(_tmp67D->f1).num_elts=_tmpABB;}),(_tmp67D->f1).zero_term=Cyc_Absyn_false_type,(_tmp67D->f1).zt_loc=0U;_tmp67D;});}_LL9:;}{
# 3796
struct Cyc_Absyn_Aggrfield*_tmp67E=({struct Cyc_Absyn_Aggrfield*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->name=old_field->name,_tmp68F->tq=Cyc_Toc_mt_tq,({
# 3798
void*_tmpABE=Cyc_Toc_typ_to_c(old_type);_tmp68F->type=_tmpABE;}),_tmp68F->width=old_field->width,_tmp68F->attributes=old_atts,_tmp68F->requires_clause=0;_tmp68F;});
# 3796
struct Cyc_Absyn_Aggrfield*new_field=_tmp67E;
# 3806
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp67F=new_field->type;void*T=_tmp67F;
struct _fat_ptr*_tmp680=new_field->name;struct _fat_ptr*f=_tmp680;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68C=({struct Cyc_String_pa_PrintArg_struct _tmp7F4;_tmp7F4.tag=0U,_tmp7F4.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7F4;});struct Cyc_String_pa_PrintArg_struct _tmp68D=({struct Cyc_String_pa_PrintArg_struct _tmp7F3;_tmp7F3.tag=0U,_tmp7F3.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7F3;});void*_tmp68A[2U];_tmp68A[0]=& _tmp68C,_tmp68A[1]=& _tmp68D;({struct _fat_ptr _tmpABF=({const char*_tmp68B="_union_%s_%s";_tag_fat(_tmp68B,sizeof(char),13U);});Cyc_aprintf(_tmpABF,_tag_fat(_tmp68A,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp689=_cycalloc(sizeof(*_tmp689));*_tmp689=s;_tmp689;});
struct Cyc_Absyn_Aggrfield*_tmp681=({struct Cyc_Absyn_Aggrfield*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->name=Cyc_Toc_val_sp,_tmp688->tq=Cyc_Toc_mt_tq,_tmp688->type=T,_tmp688->width=0,_tmp688->attributes=0,_tmp688->requires_clause=0;_tmp688;});struct Cyc_Absyn_Aggrfield*value_field=_tmp681;
struct Cyc_Absyn_Aggrfield*_tmp682=({struct Cyc_Absyn_Aggrfield*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->name=Cyc_Toc_tag_sp,_tmp687->tq=Cyc_Toc_mt_tq,_tmp687->type=Cyc_Absyn_sint_type,_tmp687->width=0,_tmp687->attributes=0,_tmp687->requires_clause=0;_tmp687;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp682;
struct Cyc_Absyn_Aggrdecl*_tmp683=({struct _fat_ptr*_tmpAC0=str;Cyc_Toc_make_c_struct_defn(_tmpAC0,({struct Cyc_Absyn_Aggrfield*_tmp686[2U];_tmp686[0]=tag_field,_tmp686[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp686,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp683;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct Cyc_Absyn_Decl*_tmpAC1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->tag=5U,_tmp684->f1=ad2;_tmp684;}),0U);_tmp685->hd=_tmpAC1;}),_tmp685->tl=Cyc_Toc_result_decls;_tmp685;});
({void*_tmpAC2=Cyc_Absyn_strct(str);new_field->type=_tmpAC2;});}
# 3817
new_fields=({struct Cyc_List_List*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->hd=new_field,_tmp68E->tl=new_fields;_tmp68E;});}}}
# 3819
({struct Cyc_List_List*_tmpAC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpAC3;});}}
# 3822
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp692=_cycalloc(sizeof(*_tmp692));({struct Cyc_Absyn_Decl*_tmpAC5=({struct Cyc_Absyn_Decl*_tmp691=_cycalloc(sizeof(*_tmp691));({void*_tmpAC4=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->tag=5U,_tmp690->f1=new_ad;_tmp690;});_tmp691->r=_tmpAC4;}),_tmp691->loc=0U;_tmp691;});_tmp692->hd=_tmpAC5;}),_tmp692->tl=Cyc_Toc_result_decls;_tmp692;});
return 0;}}}}
# 3826
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3854 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3857
struct _tuple42 _tmp694=*env;struct _tuple42 _stmttmp6F=_tmp694;struct _tuple42 _tmp695=_stmttmp6F;struct Cyc_Absyn_Datatypedecl*_tmp697;struct Cyc_Set_Set**_tmp696;_LL1: _tmp696=(_tmp695.f1)->datatypes_so_far;_tmp697=_tmp695.f2;_LL2: {struct Cyc_Set_Set**datatypes_so_far=_tmp696;struct Cyc_Absyn_Datatypedecl*tud=_tmp697;
struct _tuple1*_tmp698=tud->name;struct _tuple1*n=_tmp698;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpAC6=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpAC6;});
{struct Cyc_List_List*_tmp699=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp699;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3865
struct Cyc_List_List*_tmp69A=0;struct Cyc_List_List*fs=_tmp69A;
int i=1;
{struct Cyc_List_List*_tmp69B=f->typs;struct Cyc_List_List*ts=_tmp69B;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp69C=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp69C;
struct Cyc_Absyn_Aggrfield*_tmp69D=({struct Cyc_Absyn_Aggrfield*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->name=fname,_tmp69F->tq=(*((struct _tuple13*)ts->hd)).f1,({
void*_tmpAC7=Cyc_Toc_typ_to_c((*((struct _tuple13*)ts->hd)).f2);_tmp69F->type=_tmpAC7;}),_tmp69F->width=0,_tmp69F->attributes=0,_tmp69F->requires_clause=0;_tmp69F;});
# 3869
struct Cyc_Absyn_Aggrfield*f=_tmp69D;
# 3871
fs=({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->hd=f,_tmp69E->tl=fs;_tmp69E;});}}
# 3873
fs=({struct Cyc_List_List*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({struct Cyc_Absyn_Aggrfield*_tmpAC9=({struct Cyc_Absyn_Aggrfield*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->name=Cyc_Toc_tag_sp,_tmp6A0->tq=Cyc_Toc_mt_tq,_tmp6A0->type=Cyc_Absyn_sint_type,_tmp6A0->width=0,_tmp6A0->attributes=0,_tmp6A0->requires_clause=0;_tmp6A0;});_tmp6A1->hd=_tmpAC9;}),({
struct Cyc_List_List*_tmpAC8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);_tmp6A1->tl=_tmpAC8;});_tmp6A1;});{
struct Cyc_Absyn_Aggrdecl*_tmp6A2=({struct _fat_ptr*_tmpACB=({struct _fat_ptr*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));({struct _fat_ptr _tmpACA=({const char*_tmp6A5="";_tag_fat(_tmp6A5,sizeof(char),1U);});*_tmp6A6=_tmpACA;});_tmp6A6;});Cyc_Toc_make_c_struct_defn(_tmpACB,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp6A2;
({struct _tuple1*_tmpACC=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpACC;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));({struct Cyc_Absyn_Decl*_tmpACD=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->tag=5U,_tmp6A3->f1=ad;_tmp6A3;}),0U);_tmp6A4->hd=_tmpACD;}),_tmp6A4->tl=Cyc_Toc_result_decls;_tmp6A4;});}}}
# 3879
return 0;}}
# 3882
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3901 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3904
struct _tuple42 _tmp6A7=*env;struct _tuple42 _stmttmp70=_tmp6A7;struct _tuple42 _tmp6A8=_stmttmp70;struct Cyc_Absyn_Datatypedecl*_tmp6AA;struct Cyc_Toc_TocState*_tmp6A9;_LL1: _tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp6A9;struct Cyc_Absyn_Datatypedecl*xd=_tmp6AA;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6AB=*s;struct Cyc_Toc_TocState _stmttmp71=_tmp6AB;struct Cyc_Toc_TocState _tmp6AC=_stmttmp71;struct Cyc_Dict_Dict*_tmp6AD;_LL4: _tmp6AD=_tmp6AC.xdatatypes_so_far;_LL5: {struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp6AD;
struct _tuple1*_tmp6AE=xd->name;struct _tuple1*n=_tmp6AE;
{struct Cyc_List_List*_tmp6AF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp6AF;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B0=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp6B0;
void*_tmp6B1=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp6B1;
# 3915
int*_tmp6B2=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp72=_tmp6B2;int*_tmp6B3=_stmttmp72;if(_tmp6B3 == 0){_LL7: _LL8: {
# 3917
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3921
struct Cyc_Absyn_Vardecl*_tmp6B4=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6B4;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));({struct Cyc_Absyn_Decl*_tmpACE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->tag=0U,_tmp6B5->f1=tag_decl;_tmp6B5;}),0U);_tmp6B6->hd=_tmpACE;}),_tmp6B6->tl=Cyc_Toc_result_decls;_tmp6B6;});
({struct Cyc_Dict_Dict _tmpACF=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3924
*xdatatypes_so_far=_tmpACF;});{
# 3926
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6B7=f->typs;struct Cyc_List_List*tqts=_tmp6B7;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp6B8=({struct _fat_ptr*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));({struct _fat_ptr _tmpAD1=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6BE=({struct Cyc_Int_pa_PrintArg_struct _tmp7F5;_tmp7F5.tag=1U,_tmp7F5.f1=(unsigned long)i;_tmp7F5;});void*_tmp6BC[1U];_tmp6BC[0]=& _tmp6BE;({struct _fat_ptr _tmpAD0=({const char*_tmp6BD="f%d";_tag_fat(_tmp6BD,sizeof(char),4U);});Cyc_aprintf(_tmpAD0,_tag_fat(_tmp6BC,sizeof(void*),1U));});});*_tmp6BF=_tmpAD1;});_tmp6BF;});struct _fat_ptr*field_n=_tmp6B8;
struct Cyc_Absyn_Aggrfield*_tmp6B9=({struct Cyc_Absyn_Aggrfield*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->name=field_n,_tmp6BB->tq=(*((struct _tuple13*)tqts->hd)).f1,({
void*_tmpAD2=Cyc_Toc_typ_to_c((*((struct _tuple13*)tqts->hd)).f2);_tmp6BB->type=_tmpAD2;}),_tmp6BB->width=0,_tmp6BB->attributes=0,_tmp6BB->requires_clause=0;_tmp6BB;});
# 3930
struct Cyc_Absyn_Aggrfield*newf=_tmp6B9;
# 3932
fields=({struct Cyc_List_List*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->hd=newf,_tmp6BA->tl=fields;_tmp6BA;});}}
# 3934
fields=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));({struct Cyc_Absyn_Aggrfield*_tmpAD5=({struct Cyc_Absyn_Aggrfield*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->name=Cyc_Toc_tag_sp,_tmp6C0->tq=Cyc_Toc_mt_tq,({
void*_tmpAD4=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6C0->type=_tmpAD4;}),_tmp6C0->width=0,_tmp6C0->attributes=0,_tmp6C0->requires_clause=0;_tmp6C0;});
# 3934
_tmp6C1->hd=_tmpAD5;}),({
# 3936
struct Cyc_List_List*_tmpAD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C1->tl=_tmpAD3;});_tmp6C1;});{
struct Cyc_Absyn_Aggrdecl*_tmp6C2=({struct _fat_ptr*_tmpAD7=({struct _fat_ptr*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));({struct _fat_ptr _tmpAD6=({const char*_tmp6C5="";_tag_fat(_tmp6C5,sizeof(char),1U);});*_tmp6C6=_tmpAD6;});_tmp6C6;});Cyc_Toc_make_c_struct_defn(_tmpAD7,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp6C2;
({struct _tuple1*_tmpAD8=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpAD8;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));({struct Cyc_Absyn_Decl*_tmpAD9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->tag=5U,_tmp6C3->f1=strct_decl;_tmp6C3;}),0U);_tmp6C4->hd=_tmpAD9;}),_tmp6C4->tl=Cyc_Toc_result_decls;_tmp6C4;});
goto _LL6;}}}}}else{if(*((int*)_tmp6B3)== 0){_LL9: _LLA:
# 3942
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6C7=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp6C7;
struct Cyc_Absyn_Vardecl*_tmp6C8=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6C8;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));({struct Cyc_Absyn_Decl*_tmpADA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->tag=0U,_tmp6C9->f1=tag_decl;_tmp6C9;}),0U);_tmp6CA->hd=_tmpADA;}),_tmp6CA->tl=Cyc_Toc_result_decls;_tmp6CA;});
({struct Cyc_Dict_Dict _tmpADB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpADB;});}
# 3949
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3953
return 0;}}}}
# 3956
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3960
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3966
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpADC=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpADC;});
# 3970
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3979
struct _RegionHandle _tmp6CB=_new_region("temp");struct _RegionHandle*temp=& _tmp6CB;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6CC=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp6CC;
Cyc_Toc_exp_to_c(nv2,init);}
# 3980
;_pop_region(temp);}else{
# 3984
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3987
void*_tmp6CD=Cyc_Tcutil_compress(old_typ);void*_stmttmp73=_tmp6CD;void*_tmp6CE=_stmttmp73;void*_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D0;void*_tmp6CF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->tag == 4U){_LL1: _tmp6CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).elt_type;_tmp6D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).num_elts;_tmp6D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).zero_term;_LL2: {void*et=_tmp6CF;struct Cyc_Absyn_Exp*num_elts_opt=_tmp6D0;void*zt=_tmp6D1;
# 3989
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F6;_tmp7F6.tag=0U,({struct _fat_ptr _tmpADD=({const char*_tmp6D4="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp6D4,sizeof(char),55U);});_tmp7F6.f1=_tmpADD;});_tmp7F6;});void*_tmp6D2[1U];_tmp6D2[0]=& _tmp6D3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6D2,sizeof(void*),1U));});{
struct Cyc_Absyn_Exp*num_elts=num_elts_opt;
struct Cyc_Absyn_Exp*_tmp6D5=({struct Cyc_Absyn_Exp*_tmpADF=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpADF,({
struct Cyc_Absyn_Exp*_tmpADE=num_elts;Cyc_Absyn_add_exp(_tmpADE,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3993
struct Cyc_Absyn_Exp*lhs=_tmp6D5;
# 3996
struct Cyc_Absyn_Exp*_tmp6D6=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp6D6;
({void*_tmpAE1=({struct Cyc_Absyn_Stmt*_tmpAE0=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAE0,
Cyc_Absyn_new_stmt(s->r,0U));});
# 3997
s->r=_tmpAE1;});}}
# 4000
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4008
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6D7=d;void**_tmp6DA;struct Cyc_List_List*_tmp6D9;struct Cyc_List_List*_tmp6D8;struct Cyc_Tcpat_Rhs*_tmp6DB;switch(*((int*)_tmp6D7)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6DB=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6D7)->f1;_LL4: {struct Cyc_Tcpat_Rhs*rhs=_tmp6DB;
rhs->rhs=success;return d;}default: _LL5: _tmp6D8=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6D7)->f1;_tmp6D9=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6D7)->f2;_tmp6DA=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6D7)->f3;_LL6: {struct Cyc_List_List*path=_tmp6D8;struct Cyc_List_List*sws=_tmp6D9;void**d2=_tmp6DA;
# 4013
({void*_tmpAE2=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAE2;});
for(0;sws != 0;sws=sws->tl){
struct _tuple33*_tmp6DC=(struct _tuple33*)sws->hd;struct _tuple33*_stmttmp74=_tmp6DC;struct _tuple33*_tmp6DD=_stmttmp74;void**_tmp6DE;_LL8: _tmp6DE=(void**)& _tmp6DD->f2;_LL9: {void**d2=_tmp6DE;
({void*_tmpAE3=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAE3;});}}
# 4018
return d;}}_LL0:;}
# 4029 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4031
struct _RegionHandle _tmp6DF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6DF;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E0=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp6E0;
void*_tmp6E1=(void*)_check_null(e->topt);void*t=_tmp6E1;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4041
struct Cyc_Absyn_Stmt*_tmp6E2=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6E2;
if(dopt != 0)
# 4045
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4047
struct Cyc_Absyn_Switch_clause*_tmp6E3=({struct Cyc_Absyn_Switch_clause*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->pattern=p,_tmp6F2->pat_vars=0,_tmp6F2->where_clause=0,_tmp6F2->body=succ_stmt,_tmp6F2->loc=0U;_tmp6F2;});struct Cyc_Absyn_Switch_clause*c1=_tmp6E3;
struct Cyc_List_List*_tmp6E4=({struct _RegionHandle*_tmpAE6=rgn;struct _RegionHandle*_tmpAE5=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpAE6,Cyc_Toc_gen_labels,_tmpAE5,({struct Cyc_Absyn_Switch_clause*_tmp6F1[1U];_tmp6F1[0]=c1;({struct _RegionHandle*_tmpAE4=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpAE4,_tag_fat(_tmp6F1,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});struct Cyc_List_List*lscs=_tmp6E4;
# 4050
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4054
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4057
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple40*_tmp6E5=(struct _tuple40*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple40*_stmttmp75=_tmp6E5;struct _tuple40*_tmp6E6=_stmttmp75;struct Cyc_Absyn_Stmt*_tmp6E8;struct Cyc_Toc_Env*_tmp6E7;_LL1: _tmp6E7=_tmp6E6->f1;_tmp6E8=_tmp6E6->f3;_LL2: {struct Cyc_Toc_Env*env=_tmp6E7;struct Cyc_Absyn_Stmt*st=_tmp6E8;
if(st == succ_stmt){senv=env;goto FOUND_ENV;}}}
# 4062
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F7;_tmp7F7.tag=0U,({struct _fat_ptr _tmpAE7=({const char*_tmp6EB="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6EB,sizeof(char),33U);});_tmp7F7.f1=_tmpAE7;});_tmp7F7;});void*_tmp6E9[1U];_tmp6E9[0]=& _tmp6EA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6E9,sizeof(void*),1U));});
FOUND_ENV:
# 4066
 Cyc_Toc_stmt_to_c(senv,s);{
# 4068
struct Cyc_Absyn_Stmt*res=test_tree;
# 4070
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple38 _tmp6EC=*((struct _tuple38*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple38 _stmttmp76=_tmp6EC;struct _tuple38 _tmp6ED=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp6EE;_LL4: _tmp6EE=_tmp6ED.f2;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp6EE;
res=({struct Cyc_Absyn_Decl*_tmpAE8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->tag=0U,_tmp6EF->f1=vd;_tmp6EF;}),0U);Cyc_Absyn_decl_stmt(_tmpAE8,res,0U);});}}
# 4075
res=({struct _tuple1*_tmpAEB=v;void*_tmpAEA=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpAE9=e;Cyc_Absyn_declare_stmt(_tmpAEB,_tmpAEA,_tmpAE9,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6F0=res;_npop_handler(0U);return _tmp6F0;}}}}}
# 4032
;_pop_region(rgn);}
# 4083
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6F3=e->r;void*_stmttmp77=_tmp6F3;void*_tmp6F4=_stmttmp77;struct Cyc_Absyn_MallocInfo*_tmp6F5;struct Cyc_Absyn_Stmt*_tmp6F6;void**_tmp6F7;void**_tmp6F8;struct Cyc_List_List*_tmp6F9;struct Cyc_List_List*_tmp6FA;struct Cyc_List_List*_tmp6FC;void**_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FE;void**_tmp6FD;struct Cyc_List_List*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_List_List*_tmp710;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp*_tmp719;switch(*((int*)_tmp6F4)){case 41U: _LL1: _tmp719=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp719;
_tmp718=e;goto _LL4;}case 20U: _LL3: _tmp718=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp718;
_tmp717=e;goto _LL6;}case 21U: _LL5: _tmp717=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp717;
_tmp716=e;goto _LL8;}case 22U: _LL7: _tmp716=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp716;
_tmp715=e;goto _LLA;}case 15U: _LL9: _tmp715=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp715;
_tmp714=e;goto _LLC;}case 11U: _LLB: _tmp714=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp714;
_tmp713=e;goto _LLE;}case 12U: _LLD: _tmp713=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp713;
_tmp712=e;goto _LL10;}case 18U: _LLF: _tmp712=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp712;
_tmp711=e;goto _LL12;}case 5U: _LL11: _tmp711=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp711;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3U: _LL13: _tmp710=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL14: {struct Cyc_List_List*es=_tmp710;
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7U: _LL15: _tmp70E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp70F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp70E;struct Cyc_Absyn_Exp*e2=_tmp70F;
_tmp70C=e1;_tmp70D=e2;goto _LL18;}case 8U: _LL17: _tmp70C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp70D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp70C;struct Cyc_Absyn_Exp*e2=_tmp70D;
_tmp70A=e1;_tmp70B=e2;goto _LL1A;}case 9U: _LL19: _tmp70A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp70B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp70A;struct Cyc_Absyn_Exp*e2=_tmp70B;
_tmp708=e1;_tmp709=e2;goto _LL1C;}case 23U: _LL1B: _tmp708=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp709=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp708;struct Cyc_Absyn_Exp*e2=_tmp709;
_tmp706=e1;_tmp707=e2;goto _LL1E;}case 35U: _LL1D: _tmp706=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp707=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp706;struct Cyc_Absyn_Exp*e2=_tmp707;
_tmp704=e1;_tmp705=e2;goto _LL20;}case 4U: _LL1F: _tmp704=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp705=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp704;struct Cyc_Absyn_Exp*e2=_tmp705;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6U: _LL21: _tmp701=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp702=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_tmp703=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F4)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp701;struct Cyc_Absyn_Exp*e2=_tmp702;struct Cyc_Absyn_Exp*e3=_tmp703;
# 4102
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10U: _LL23: _tmp6FF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp700=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp6FF;struct Cyc_List_List*es=_tmp700;
# 4104
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14U: _LL25: _tmp6FD=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp6FE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL26: {void**t=_tmp6FD;struct Cyc_Absyn_Exp*e=_tmp6FE;
({void*_tmpAEC=Cyc_Toc_typ_to_c(*t);*t=_tmpAEC;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25U: _LL27: _tmp6FB=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1)->f3;_tmp6FC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL28: {void**t=_tmp6FB;struct Cyc_List_List*dles=_tmp6FC;
# 4107
({void*_tmpAED=Cyc_Toc_typ_to_c(*t);*t=_tmpAED;});
_tmp6FA=dles;goto _LL2A;}case 36U: _LL29: _tmp6FA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp6FA;
_tmp6F9=dles;goto _LL2C;}case 26U: _LL2B: _tmp6F9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp6F9;
# 4111
for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple20*)dles->hd)).f2);}
goto _LL0;}case 19U: _LL2D: _tmp6F8=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2E: {void**t=_tmp6F8;
_tmp6F7=t;goto _LL30;}case 17U: _LL2F: _tmp6F7=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL30: {void**t=_tmp6F7;
({void*_tmpAEE=Cyc_Toc_typ_to_c(*t);*t=_tmpAEE;});goto _LL0;}case 37U: _LL31: _tmp6F6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL32: {struct Cyc_Absyn_Stmt*s=_tmp6F6;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34U: _LL33: _tmp6F5=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL34: {struct Cyc_Absyn_MallocInfo*m=_tmp6F5;
# 4118
if(m->elt_type != 0)
({void**_tmpAF0=({void**_tmp71A=_cycalloc(sizeof(*_tmp71A));({void*_tmpAEF=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp71A=_tmpAEF;});_tmp71A;});m->elt_type=_tmpAF0;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4128
 goto _LL42;case 30U: _LL41: _LL42:
 goto _LL44;case 31U: _LL43: _LL44:
 goto _LL46;case 29U: _LL45: _LL46:
 goto _LL48;case 27U: _LL47: _LL48:
 goto _LL4A;case 28U: _LL49: _LL4A:
 goto _LL4C;case 24U: _LL4B: _LL4C:
 goto _LL4E;case 13U: _LL4D: _LL4E:
 goto _LL50;case 16U: _LL4F: _LL50:
 goto _LL52;case 39U: _LL51: _LL52:
 goto _LL54;default: _LL53: _LL54:
({void*_tmp71B=0U;({unsigned _tmpAF2=e->loc;struct _fat_ptr _tmpAF1=({const char*_tmp71C="Cyclone expression in C code";_tag_fat(_tmp71C,sizeof(char),29U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAF2,_tmpAF1,_tag_fat(_tmp71B,sizeof(void*),0U));});});}_LL0:;}
# 4142
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp71D=d->r;void*_stmttmp78=_tmp71D;void*_tmp71E=_stmttmp78;struct Cyc_Absyn_Typedefdecl*_tmp71F;struct Cyc_Absyn_Enumdecl*_tmp720;struct Cyc_Absyn_Aggrdecl*_tmp721;struct Cyc_Absyn_Fndecl*_tmp722;struct Cyc_Absyn_Vardecl*_tmp723;switch(*((int*)_tmp71E)){case 0U: _LL1: _tmp723=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp723;
# 4145
({void*_tmpAF3=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpAF3;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp722=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp722;
# 4149
({void*_tmpAF4=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpAF4;});
{struct Cyc_List_List*_tmp724=(fd->i).args;struct Cyc_List_List*args=_tmp724;for(0;args != 0;args=args->tl){
({void*_tmpAF5=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAF5;});}}
goto _LL0;}case 5U: _LL5: _tmp721=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp721;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7U: _LL7: _tmp720=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp720;
# 4155
if(ed->fields != 0){
struct Cyc_List_List*_tmp725=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp725;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp726=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp726;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4160
goto _LL0;}case 8U: _LL9: _tmp71F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp71E)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp71F;
({void*_tmpAF6=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpAF6;});goto _LL0;}case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4170
({void*_tmp727=0U;({unsigned _tmpAF8=d->loc;struct _fat_ptr _tmpAF7=({const char*_tmp728="Cyclone declaration in C code";_tag_fat(_tmp728,sizeof(char),30U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAF8,_tmpAF7,_tag_fat(_tmp727,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4178
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp729=s->r;void*_stmttmp79=_tmp729;void*_tmp72A=_stmttmp79;struct Cyc_Absyn_Stmt*_tmp72B;struct Cyc_Absyn_Exp*_tmp72D;struct Cyc_Absyn_Stmt*_tmp72C;struct Cyc_Absyn_Stmt*_tmp72F;struct Cyc_Absyn_Decl*_tmp72E;void*_tmp732;struct Cyc_List_List*_tmp731;struct Cyc_Absyn_Exp*_tmp730;struct Cyc_Absyn_Stmt*_tmp736;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_Absyn_Exp*_tmp734;struct Cyc_Absyn_Exp*_tmp733;struct Cyc_Absyn_Stmt*_tmp738;struct Cyc_Absyn_Exp*_tmp737;struct Cyc_Absyn_Stmt*_tmp73B;struct Cyc_Absyn_Stmt*_tmp73A;struct Cyc_Absyn_Exp*_tmp739;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Stmt*_tmp73E;struct Cyc_Absyn_Stmt*_tmp73D;struct Cyc_Absyn_Exp*_tmp73F;switch(*((int*)_tmp72A)){case 1U: _LL1: _tmp73F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp73F;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2U: _LL3: _tmp73D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp73E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp73D;struct Cyc_Absyn_Stmt*s2=_tmp73E;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3U: _LL5: _tmp73C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_LL6: {struct Cyc_Absyn_Exp*eopt=_tmp73C;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4U: _LL7: _tmp739=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp73A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_tmp73B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72A)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp739;struct Cyc_Absyn_Stmt*s1=_tmp73A;struct Cyc_Absyn_Stmt*s2=_tmp73B;
# 4184
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5U: _LL9: _tmp737=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1).f1;_tmp738=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp737;struct Cyc_Absyn_Stmt*s=_tmp738;
# 4186
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9U: _LLB: _tmp733=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp734=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2).f1;_tmp735=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f3).f1;_tmp736=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72A)->f4;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp733;struct Cyc_Absyn_Exp*e2=_tmp734;struct Cyc_Absyn_Exp*e3=_tmp735;struct Cyc_Absyn_Stmt*s=_tmp736;
# 4188
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10U: _LLD: _tmp730=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp731=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_tmp732=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72A)->f3;_LLE: {struct Cyc_Absyn_Exp*e=_tmp730;struct Cyc_List_List*scs=_tmp731;void*dec_tree=_tmp732;
# 4191
Cyc_Toc_exptypes_to_c(e);
for(0;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12U: _LLF: _tmp72E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp72F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LL10: {struct Cyc_Absyn_Decl*d=_tmp72E;struct Cyc_Absyn_Stmt*s=_tmp72F;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14U: _LL11: _tmp72C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72A)->f1;_tmp72D=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp72C;struct Cyc_Absyn_Exp*e=_tmp72D;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13U: _LL13: _tmp72B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp72A)->f2;_LL14: {struct Cyc_Absyn_Stmt*s=_tmp72B;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4203
({void*_tmpAF9=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->tag=0U;_tmp740;});s->r=_tmpAF9;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp741=0U;({unsigned _tmpAFB=s->loc;struct _fat_ptr _tmpAFA=({const char*_tmp742="Cyclone statement in C code";_tag_fat(_tmp742,sizeof(char),28U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAFB,_tmpAFA,_tag_fat(_tmp741,sizeof(void*),0U));});});}_LL0:;}
# 4211
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4213
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp744=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F8;_tmp7F8.tag=0U,({struct _fat_ptr _tmpAFC=({const char*_tmp745="decls_to_c: not at toplevel!";_tag_fat(_tmp745,sizeof(char),29U);});_tmp7F8.f1=_tmpAFC;});_tmp7F8;});void*_tmp743[1U];_tmp743[0]=& _tmp744;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp743,sizeof(void*),1U));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp746=d->r;void*_stmttmp7A=_tmp746;void*_tmp747=_stmttmp7A;struct Cyc_List_List*_tmp748;struct Cyc_List_List*_tmp749;struct Cyc_List_List*_tmp74A;struct Cyc_List_List*_tmp74B;struct Cyc_Absyn_Typedefdecl*_tmp74C;struct Cyc_Absyn_Enumdecl*_tmp74D;struct Cyc_Absyn_Datatypedecl*_tmp74E;struct Cyc_Absyn_Aggrdecl*_tmp74F;struct Cyc_Absyn_Fndecl*_tmp750;struct Cyc_Absyn_Vardecl*_tmp751;switch(*((int*)_tmp747)){case 0U: _LL1: _tmp751=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp751;
# 4220
struct _tuple1*_tmp752=vd->name;struct _tuple1*c_name=_tmp752;
# 4222
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp753=_cycalloc(sizeof(*_tmp753));({union Cyc_Absyn_Nmspace _tmpAFD=Cyc_Absyn_Abs_n(0,1);_tmp753->f1=_tmpAFD;}),_tmp753->f2=(*c_name).f2;_tmp753;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4229
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4231
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpAFE=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpAFE;});
({void*_tmpAFF=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpAFF;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754->hd=d,_tmp754->tl=Cyc_Toc_result_decls;_tmp754;});
goto _LL0;}case 1U: _LL3: _tmp750=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp750;
# 4238
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB01=({struct _tuple1*_tmp755=_cycalloc(sizeof(*_tmp755));({union Cyc_Absyn_Nmspace _tmpB00=Cyc_Absyn_Abs_n(0,1);_tmp755->f1=_tmpB00;}),_tmp755->f2=(*fd->name).f2;_tmp755;});fd->name=_tmpB01;});
fd->sc=Cyc_Absyn_Public;}
# 4242
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->hd=d,_tmp756->tl=Cyc_Toc_result_decls;_tmp756;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp758=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F9;_tmp7F9.tag=0U,({struct _fat_ptr _tmpB02=({const char*_tmp759="letdecl at toplevel";_tag_fat(_tmp759,sizeof(char),20U);});_tmp7F9.f1=_tmpB02;});_tmp7F9;});void*_tmp757[1U];_tmp757[0]=& _tmp758;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp757,sizeof(void*),1U));});case 4U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp75B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FA;_tmp7FA.tag=0U,({struct _fat_ptr _tmpB03=({const char*_tmp75C="region decl at toplevel";_tag_fat(_tmp75C,sizeof(char),24U);});_tmp7FA.f1=_tmpB03;});_tmp7FA;});void*_tmp75A[1U];_tmp75A[0]=& _tmp75B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp75A,sizeof(void*),1U));});case 5U: _LLB: _tmp74F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*sd=_tmp74F;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6U: _LLD: _tmp74E=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LLE: {struct Cyc_Absyn_Datatypedecl*tud=_tmp74E;
# 4250
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7U: _LLF: _tmp74D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp74D;
# 4253
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=d,_tmp75D->tl=Cyc_Toc_result_decls;_tmp75D;});
goto _LL0;}case 8U: _LL11: _tmp74C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp74C;
# 4257
td->tvs=0;
if(td->defn != 0){
({void*_tmpB04=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB04;});{
# 4262
struct Cyc_Absyn_Decl*ed;
{void*_tmp75E=td->defn;void*_stmttmp7B=_tmp75E;void*_tmp75F=_stmttmp7B;unsigned _tmp761;struct Cyc_Absyn_Enumdecl*_tmp760;if(_tmp75F != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75F)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75F)->f1)->r)->tag == 1U){_LL24: _tmp760=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75F)->f1)->r)->f1;_tmp761=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp75F)->f1)->loc;_LL25: {struct Cyc_Absyn_Enumdecl*ed2=_tmp760;unsigned loc=_tmp761;
# 4265
ed=({struct Cyc_Absyn_Decl*_tmp763=_cycalloc(sizeof(*_tmp763));({void*_tmpB05=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->tag=7U,_tmp762->f1=ed2;_tmp762;});_tmp763->r=_tmpB05;}),_tmp763->loc=loc;_tmp763;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4268
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->hd=ed,_tmp764->tl=Cyc_Toc_result_decls;_tmp764;});{
void*_tmp765=ed->r;void*_stmttmp7C=_tmp765;void*_tmp766=_stmttmp7C;struct Cyc_Absyn_Enumdecl*_tmp767;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp766)->tag == 7U){_LL29: _tmp767=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp766)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp767;
# 4272
({void*_tmpB07=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->tag=0U,({void*_tmpB06=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->tag=15U,_tmp768->f1=ed->name,_tmp768->f2=ed;_tmp768;});_tmp769->f1=_tmpB06;}),_tmp769->f2=0;_tmp769;});td->defn=_tmpB07;});goto _LL28;}}else{_LL2B: _LL2C:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp76B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FB;_tmp7FB.tag=0U,({struct _fat_ptr _tmpB08=({const char*_tmp76C="Toc: enumdecl to name";_tag_fat(_tmp76C,sizeof(char),22U);});_tmp7FB.f1=_tmpB08;});_tmp7FB;});void*_tmp76A[1U];_tmp76A[0]=& _tmp76B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp76A,sizeof(void*),1U));});}_LL28:;}}}}else{
# 4277
enum Cyc_Absyn_KindQual _tmp76D=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7D=_tmp76D;enum Cyc_Absyn_KindQual _tmp76E=_stmttmp7D;if(_tmp76E == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpB09=Cyc_Toc_void_star_type();td->defn=_tmpB09;});goto _LL2D;}else{_LL30: _LL31:
 td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4285
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F->hd=d,_tmp76F->tl=Cyc_Toc_result_decls;_tmp76F;});
goto _LL0;}case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp74B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp747)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp74B;
_tmp74A=ds2;goto _LL1E;}case 10U: _LL1D: _tmp74A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp747)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp74A;
_tmp749=ds2;goto _LL20;}case 11U: _LL1F: _tmp749=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp749;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _LL21: _tmp748=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp747)->f1;_LL22: {struct Cyc_List_List*ds2=_tmp748;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4298
return nv;}
# 4303
void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp770=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp7E=_tmp770;struct Cyc_Core_NewDynamicRegion _tmp771=_stmttmp7E;struct Cyc_Core_DynamicRegion*_tmp772;_LL1: _tmp772=_tmp771.key;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp772;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp774=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp774));_tmp774->dyn=dyn,_tmp774->state=ts;_tmp774;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp776=34;struct _fat_ptr**_tmp775=_cycalloc(_check_times(_tmp776,sizeof(struct _fat_ptr*)));_tmp775[0]=& Cyc_Toc__throw_str,_tmp775[1]=& Cyc_Toc_setjmp_str,_tmp775[2]=& Cyc_Toc__push_handler_str,_tmp775[3]=& Cyc_Toc__pop_handler_str,_tmp775[4]=& Cyc_Toc__exn_thrown_str,_tmp775[5]=& Cyc_Toc__npop_handler_str,_tmp775[6]=& Cyc_Toc__check_null_str,_tmp775[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp775[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp775[9]=& Cyc_Toc__check_fat_subscript_str,_tmp775[10]=& Cyc_Toc__tag_fat_str,_tmp775[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp775[12]=& Cyc_Toc__get_fat_size_str,_tmp775[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp775[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp775[15]=& Cyc_Toc__zero_arr_plus_str,_tmp775[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp775[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp775[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp775[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp775[20]=& Cyc_Toc__cycalloc_str,_tmp775[21]=& Cyc_Toc__cyccalloc_str,_tmp775[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp775[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp775[24]=& Cyc_Toc__region_malloc_str,_tmp775[25]=& Cyc_Toc__region_calloc_str,_tmp775[26]=& Cyc_Toc__check_times_str,_tmp775[27]=& Cyc_Toc__new_region_str,_tmp775[28]=& Cyc_Toc__push_region_str,_tmp775[29]=& Cyc_Toc__pop_region_str,_tmp775[30]=& Cyc_Toc__throw_arraybounds_str,_tmp775[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp775[32]=& Cyc_Toc__throw_match_str,_tmp775[33]=& Cyc_Toc__fast_region_malloc_str;_tmp775;}),sizeof(struct _fat_ptr*),34U);}}
# 4351
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp777=ts;struct Cyc_Toc_TocStateWrap*_tmp778=Cyc_Toc_toc_state;ts=_tmp778;Cyc_Toc_toc_state=_tmp777;});{
struct Cyc_Toc_TocStateWrap _tmp779=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp7F=_tmp779;struct Cyc_Toc_TocStateWrap _tmp77A=_stmttmp7F;struct Cyc_Toc_TocState*_tmp77C;struct Cyc_Core_DynamicRegion*_tmp77B;_LL1: _tmp77B=_tmp77A.dyn;_tmp77C=_tmp77A.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp77B;struct Cyc_Toc_TocState*s=_tmp77C;
# 4356
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp77E=*s;struct Cyc_Toc_TocState _stmttmp80=_tmp77E;struct Cyc_Toc_TocState _tmp77F=_stmttmp80;struct Cyc_Xarray_Xarray*_tmp780;_LL4: _tmp780=_tmp77F.temp_labels;_LL5: {struct Cyc_Xarray_Xarray*tls=_tmp780;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(tls);}}
# 4357
;}
# 4360
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4363
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4369
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4371
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp781=_cycalloc(sizeof(*_tmp781));*_tmp781=pop_tables;_tmp781;});
Cyc_Toc_init();{
struct _RegionHandle _tmp782=_new_region("start");struct _RegionHandle*start=& _tmp782;_push_region(start);
({struct _RegionHandle*_tmpB0B=start;struct Cyc_Toc_Env*_tmpB0A=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB0B,_tmpB0A,ds,0);});{
struct Cyc_List_List*_tmp783=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp783;}
# 4374
;_pop_region(start);}}
