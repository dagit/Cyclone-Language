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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 867
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 874
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 883
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 904
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 906
extern void*Cyc_Absyn_sint_type;
# 911
extern void*Cyc_Absyn_heap_rgn_type;
# 915
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 917
extern void*Cyc_Absyn_void_type;
# 934
void*Cyc_Absyn_exn_type();
# 942
extern void*Cyc_Absyn_fat_bound_type;
# 946
void*Cyc_Absyn_bounds_one();
# 956
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 960
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 962
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 964
void*Cyc_Absyn_strct(struct _fat_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 970
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 975
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 978
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1046
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1051
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1059
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1063
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1068
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1072
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1077
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1114
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1117
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1135
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1137
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1139
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1148
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1153
extern int Cyc_Absyn_no_regions;
# 38 "warn.h"
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 46
void*Cyc_Warn_impos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
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
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 272
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 290
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 301
int Cyc_Tcutil_force_type2bool(int desired,void*);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 34 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 36
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 88 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 58 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 73 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr fmt,struct _fat_ptr ap){
# 75
({struct _fat_ptr _tmp770=(struct _fat_ptr)({struct _fat_ptr _tmp76F=({const char*_tmp0="Toc (unimplemented): ";_tag_fat(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp76F,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp770,ap);});}
# 77
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 79
({struct _fat_ptr _tmp772=(struct _fat_ptr)({struct _fat_ptr _tmp771=({const char*_tmp1="Toc: ";_tag_fat(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp771,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp772,ap);});}
# 82
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 85
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 95
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 97
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 123
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 130
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp9=_region_malloc(d,sizeof(*_tmp9));
({struct Cyc_List_List**_tmp77E=({struct Cyc_List_List**_tmp2=_region_malloc(d,sizeof(*_tmp2));*_tmp2=0;_tmp2;});_tmp9->tuple_types=_tmp77E;}),({
struct Cyc_List_List**_tmp77D=({struct Cyc_List_List**_tmp3=_region_malloc(d,sizeof(*_tmp3));*_tmp3=0;_tmp3;});_tmp9->anon_aggr_types=_tmp77D;}),({
struct Cyc_Dict_Dict*_tmp77C=({struct Cyc_Dict_Dict*_tmp4=_region_malloc(d,sizeof(*_tmp4));({struct Cyc_Dict_Dict _tmp77B=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp4=_tmp77B;});_tmp4;});_tmp9->aggrs_so_far=_tmp77C;}),({
struct Cyc_List_List**_tmp77A=({struct Cyc_List_List**_tmp5=_region_malloc(d,sizeof(*_tmp5));*_tmp5=0;_tmp5;});_tmp9->abs_struct_types=_tmp77A;}),({
struct Cyc_Set_Set**_tmp779=({struct Cyc_Set_Set**_tmp6=_region_malloc(d,sizeof(*_tmp6));({struct Cyc_Set_Set*_tmp778=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp6=_tmp778;});_tmp6;});_tmp9->datatypes_so_far=_tmp779;}),({
struct Cyc_Dict_Dict*_tmp777=({struct Cyc_Dict_Dict*_tmp7=_region_malloc(d,sizeof(*_tmp7));({struct Cyc_Dict_Dict _tmp776=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp7=_tmp776;});_tmp7;});_tmp9->xdatatypes_so_far=_tmp777;}),({
struct Cyc_Dict_Dict*_tmp775=({struct Cyc_Dict_Dict*_tmp8=_region_malloc(d,sizeof(*_tmp8));({struct Cyc_Dict_Dict _tmp774=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp8=_tmp774;});_tmp8;});_tmp9->qvar_tags=_tmp775;}),({
struct Cyc_Xarray_Xarray*_tmp773=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp9->temp_labels=_tmp773;});_tmp9;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 149
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 155
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 158
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpA=ts;struct Cyc_Toc_TocStateWrap*_tmpB=Cyc_Toc_toc_state;ts=_tmpB;Cyc_Toc_toc_state=_tmpA;});{
struct Cyc_Toc_TocStateWrap _tmpC=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmpC;struct Cyc_Toc_TocStateWrap _tmpD=_stmttmp0;struct Cyc_Toc_TocState*_tmpF;struct Cyc_Core_DynamicRegion*_tmpE;_LL1: _tmpE=_tmpD.dyn;_tmpF=_tmpD.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmpE;struct Cyc_Toc_TocState*s=_tmpF;
struct _fat_ptr _tmp10=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp10;
({struct Cyc_Toc_TocStateWrap _tmp77F=({struct Cyc_Toc_TocStateWrap _tmp73E;_tmp73E.dyn=dyn,_tmp73E.state=s;_tmp73E;});*ts=_tmp77F;});
({struct Cyc_Toc_TocStateWrap*_tmp11=ts;struct Cyc_Toc_TocStateWrap*_tmp12=Cyc_Toc_toc_state;ts=_tmp12;Cyc_Toc_toc_state=_tmp11;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp14=({struct _tuple15 _tmp73F;_tmp73F.f1=s,_tmp73F.f2=arg;_tmp73F;});struct _tuple15 env=_tmp14;
res=f(h,& env);}
# 166
;}
# 168
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 172
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 175
struct _tuple17 _tmp15=*env;struct _tuple17 _stmttmp1=_tmp15;struct _tuple17 _tmp16=_stmttmp1;void*(*_tmp19)(struct _tuple1*);struct _tuple1*_tmp18;struct Cyc_Toc_TocState*_tmp17;_LL1: _tmp17=_tmp16.f1;_tmp18=(_tmp16.f2)->f1;_tmp19=(_tmp16.f2)->f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp17;struct _tuple1*q=_tmp18;void*(*type_maker)(struct _tuple1*)=_tmp19;
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 180
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp740;_tmp740.f1=q,_tmp740.f2=type_maker;_tmp740;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1A[5U]="curr";
# 193 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1A,_tmp1A,_tmp1A + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp1B[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp1B,_tmp1B,_tmp1B + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp1C[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp1C,_tmp1C,_tmp1C + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp1D[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp1D,_tmp1D,_tmp1D + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1E[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp1E,_tmp1E,_tmp1E + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1F[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp1F,_tmp1F,_tmp1F + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp20[7U]="_throw";
# 212 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_tmp20,_tmp20,_tmp20 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp22[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_tmp22,_tmp22,_tmp22 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp23[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_tmp23,_tmp23,_tmp23 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp24[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_tmp24,_tmp24,_tmp24 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp25[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_tmp25,_tmp25,_tmp25 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp26[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_tmp26,_tmp26,_tmp26 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp27[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_tmp27,_tmp27,_tmp27 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp28[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp28,_tmp28,_tmp28 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp29[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_tmp29,_tmp29,_tmp29 + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0U,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1U,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _tmp2A[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_tmp2A,_tmp2A,_tmp2A + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0U,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1U,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _tmp2B[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_tmp2B,_tmp2B,_tmp2B + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0U,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1U,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _tmp2C[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_tmp2C,_tmp2C,_tmp2C + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0U,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1U,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _tmp2D[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_tmp2D,_tmp2D,_tmp2D + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0U,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1U,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _tmp2E[19U]="_get_zero_arr_size";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_str={_tmp2E,_tmp2E,_tmp2E + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp2F[24U]="_get_zero_arr_size_char";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp2F,_tmp2F,_tmp2F + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp30[25U]="_get_zero_arr_size_short";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp30,_tmp30,_tmp30 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp31[23U]="_get_zero_arr_size_int";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp31,_tmp31,_tmp31 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp32[25U]="_get_zero_arr_size_float";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp32,_tmp32,_tmp32 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp33[26U]="_get_zero_arr_size_double";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp33,_tmp33,_tmp33 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp34[30U]="_get_zero_arr_size_longdouble";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp34,_tmp34,_tmp34 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp35[28U]="_get_zero_arr_size_voidstar";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp35,_tmp35,_tmp35 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp36[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_tmp36,_tmp36,_tmp36 + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0U,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1U,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _tmp37[15U]="_zero_arr_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_str={_tmp37,_tmp37,_tmp37 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp38[20U]="_zero_arr_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp38,_tmp38,_tmp38 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp39[21U]="_zero_arr_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp39,_tmp39,_tmp39 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp3A[19U]="_zero_arr_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp3A,_tmp3A,_tmp3A + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp3B[21U]="_zero_arr_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp3B,_tmp3B,_tmp3B + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp3C[22U]="_zero_arr_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp3C,_tmp3C,_tmp3C + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp3D[26U]="_zero_arr_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp3D,_tmp3D,_tmp3D + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp3E[24U]="_zero_arr_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp3E,_tmp3E,_tmp3E + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp3F[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_tmp3F,_tmp3F,_tmp3F + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0U,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _tmp40[23U]="_zero_arr_inplace_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp40,_tmp40,_tmp40 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp41[28U]="_zero_arr_inplace_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp41,_tmp41,_tmp41 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp42[29U]="_zero_arr_inplace_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp42,_tmp42,_tmp42 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp43[27U]="_zero_arr_inplace_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp43,_tmp43,_tmp43 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp44[29U]="_zero_arr_inplace_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp44,_tmp44,_tmp44 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp45[30U]="_zero_arr_inplace_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp45,_tmp45,_tmp45 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp46[34U]="_zero_arr_inplace_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp46,_tmp46,_tmp46 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp47[32U]="_zero_arr_inplace_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp47,_tmp47,_tmp47 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp48[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_tmp48,_tmp48,_tmp48 + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _tmp49[28U]="_zero_arr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp49,_tmp49,_tmp49 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp4A[33U]="_zero_arr_inplace_plus_post_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp4A,_tmp4A,_tmp4A + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp4B[34U]="_zero_arr_inplace_plus_post_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp4B,_tmp4B,_tmp4B + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp4C[32U]="_zero_arr_inplace_plus_post_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp4C,_tmp4C,_tmp4C + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp4D[34U]="_zero_arr_inplace_plus_post_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp4D,_tmp4D,_tmp4D + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp4E[35U]="_zero_arr_inplace_plus_post_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp4E,_tmp4E,_tmp4E + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp4F[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp4F,_tmp4F,_tmp4F + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp50[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp50,_tmp50,_tmp50 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp51[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_tmp51,_tmp51,_tmp51 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp52[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_tmp52,_tmp52,_tmp52 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp53[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_tmp53,_tmp53,_tmp53 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp54[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp54,_tmp54,_tmp54 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp55[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_tmp55,_tmp55,_tmp55 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp56[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_tmp56,_tmp56,_tmp56 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp57[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_tmp57,_tmp57,_tmp57 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp58[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_tmp58,_tmp58,_tmp58 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp59[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_tmp59,_tmp59,_tmp59 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp5A[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_tmp5A,_tmp5A,_tmp5A + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp5B[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_tmp5B,_tmp5B,_tmp5B + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp5C[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_tmp5C,_tmp5C,_tmp5C + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0U,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _tmp5D[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_tmp5D,_tmp5D,_tmp5D + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp5E[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_tmp5E,_tmp5E,_tmp5E + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp5F[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_tmp5F,_tmp5F,_tmp5F + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 280
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression(){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 283
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp6A=_cycalloc(sizeof(*_tmp6A));((_tmp6A->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp784=({struct _fat_ptr*_tmp65[1U];({struct _fat_ptr*_tmp783=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));({struct _fat_ptr _tmp782=({const char*_tmp66="Core";_tag_fat(_tmp66,sizeof(char),5U);});*_tmp67=_tmp782;});_tmp67;});_tmp65[0]=_tmp783;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp65,sizeof(struct _fat_ptr*),1U));});((_tmp6A->f1).Abs_n).val=_tmp784;}),({struct _fat_ptr*_tmp781=({struct _fat_ptr*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _fat_ptr _tmp780=({const char*_tmp68="get_exn_thrown";_tag_fat(_tmp68,sizeof(char),15U);});*_tmp69=_tmp780;});_tmp69;});_tmp6A->f2=_tmp781;});_tmp6A;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=0U,_tmp64->f1=qv;_tmp64;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->topt=0,({void*_tmp785=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=1U,_tmp62->f1=bnd;_tmp62;});_tmp63->r=_tmp785;}),_tmp63->loc=0U,_tmp63->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp63;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=10U,_tmp61->f1=fnname,_tmp61->f2=0,_tmp61->f3=0,_tmp61->f4=0;_tmp61;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->topt=0,_tmp60->r=fncall_re,_tmp60->loc=0U,_tmp60->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp60;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 293
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 295
void*Cyc_Toc_void_star_type(){
static void*t=0;
if(t == 0)
t=({void*_tmp788=Cyc_Absyn_void_type;void*_tmp787=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp786=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_star_type(_tmp788,_tmp787,_tmp786,Cyc_Absyn_false_type);});
return(void*)_check_null(t);}
# 301
static void*Cyc_Toc_fat_ptr_type(){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__fat_ptr_pr,0),0);
return(void*)_check_null(t);}
# 307
static void*Cyc_Toc_rgn_type(){
static void*r=0;
if(r == 0)
r=({void*_tmp789=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp789,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 314
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 318
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct Cyc_List_List*_tmp78B=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));({void*_tmp78A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=1U,_tmp6B->f1=name;_tmp6B;});_tmp6C->hd=_tmp78A;}),_tmp6C->tl=0;_tmp6C;});_tmp6D->f1=_tmp78B;}),_tmp6D->f2=e;_tmp6D;});}
# 322
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp78C=f;Cyc_Absyn_fncall_exp(_tmp78C,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
# 325
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp6E=e->r;void*_stmttmp2=_tmp6E;void*_tmp6F=_stmttmp2;struct Cyc_Absyn_Exp*_tmp70;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp70=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F)->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp70;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 333
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=14U,_tmp71->f1=t,_tmp71->f2=e,_tmp71->f3=0,_tmp71->f4=Cyc_Absyn_No_coercion;_tmp71;});}
# 336
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=20U,_tmp72->f1=e;_tmp72;});}
# 339
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=23U,_tmp73->f1=e1,_tmp73->f2=e2;_tmp73;});}
# 342
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=37U,_tmp74->f1=s;_tmp74;});}
# 345
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=17U,_tmp75->f1=t;_tmp75;});}
# 348
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=10U,_tmp76->f1=e,({struct Cyc_List_List*_tmp78D=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp76->f2=_tmp78D;}),_tmp76->f3=0,_tmp76->f4=1;_tmp76;});}
# 351
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=2U,_tmp77->f1=s1,_tmp77->f2=s2;_tmp77;});}
# 354
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=6U,_tmp78->f1=e1,_tmp78->f2=e2,_tmp78->f3=e3;_tmp78;});}
# 357
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=21U,_tmp79->f1=e,_tmp79->f2=n,_tmp79->f3=0,_tmp79->f4=0;_tmp79;});}
# 360
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 363
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=36U,_tmp7A->f1=tdopt,_tmp7A->f2=ds;_tmp7A;});}
# 365
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=8U,_tmp7B->f1=v;_tmp7B;});}
# 368
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 372
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp7C=e->r;void*_stmttmp3=_tmp7C;void*_tmp7D=_stmttmp3;struct Cyc_Absyn_Exp*_tmp7E;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7D)->tag == 20U){_LL1: _tmp7E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7D)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp7E;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 400 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 406
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp7F=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp7F;void*_tmp80=_stmttmp4;switch(*((int*)_tmp80)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp80)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp80)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1: _LL2:
 return fS->fchar;case Cyc_Absyn_Short_sz: _LL3: _LL4:
 return fS->fshort;case Cyc_Absyn_Int_sz: _LL5: _LL6:
 return fS->fint;default: goto _LLF;}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp80)->f1)->f1){case 0U: _LL7: _LL8:
 return fS->ffloat;case 1U: _LL9: _LLA:
 return fS->fdouble;default: _LLB: _LLC:
 return fS->flongdouble;}default: goto _LLF;}case 3U: _LLD: _LLE:
 return fS->fvoidstar;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0U,({struct _fat_ptr _tmp78E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp741.f1=_tmp78E;});_tmp741;});void*_tmp81[1U];_tmp81[0]=& _tmp83;({struct _fat_ptr _tmp78F=({const char*_tmp82="expression type %s (not int, float, double, or pointer)";_tag_fat(_tmp82,sizeof(char),56U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp78F,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 418
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp84=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp84;void*_tmp85=_stmttmp5;void*_tmp86;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp85)->tag == 3U){_LL1: _tmp86=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp85)->f1).elt_type;_LL2: {void*et=_tmp86;
return Cyc_Toc_getFunctionType(fS,et);}}else{_LL3: _LL4:
({void*_tmp87=0U;({struct _fat_ptr _tmp790=({const char*_tmp88="impossible type (not pointer)";_tag_fat(_tmp88,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp790,_tag_fat(_tmp87,sizeof(void*),0U));});});}_LL0:;}
# 428
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp89=e->r;void*_stmttmp6=_tmp89;void*_tmp8A=_stmttmp6;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;long long _tmp91;int _tmp92;short _tmp93;struct _fat_ptr _tmp94;char _tmp95;switch(*((int*)_tmp8A)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Null_c).tag){case 2U: _LL1: _tmp95=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Char_c).val).f2;_LL2: {char c=_tmp95;
return(int)c == (int)'\000';}case 3U: _LL3: _tmp94=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp94;
# 432
unsigned long _tmp96=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp96;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 442
return 0;}}case 4U: _LL5: _tmp93=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Short_c).val).f2;_LL6: {short i=_tmp93;
return(int)i == 0;}case 5U: _LL7: _tmp92=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Int_c).val).f2;_LL8: {int i=_tmp92;
return i == 0;}case 6U: _LL9: _tmp91=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).LongLong_c).val).f2;_LLA: {long long i=_tmp91;
return i == (long long)0;}case 1U: _LLD: _LLE:
# 447
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 446
 goto _LLE;case 14U: _LLF: _tmp90=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp90;
# 448
return Cyc_Toc_is_zero(e1);}case 24U: _LL11: _tmp8F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_LL12: {struct Cyc_List_List*es=_tmp8F;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,es);}case 26U: _LL13: _tmp8E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_LL14: {struct Cyc_List_List*dles=_tmp8E;
_tmp8D=dles;goto _LL16;}case 29U: _LL15: _tmp8D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8A)->f3;_LL16: {struct Cyc_List_List*dles=_tmp8D;
_tmp8C=dles;goto _LL18;}case 25U: _LL17: _tmp8C=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL18: {struct Cyc_List_List*dles=_tmp8C;
_tmp8B=dles;goto _LL1A;}case 36U: _LL19: _tmp8B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL1A: {struct Cyc_List_List*dles=_tmp8B;
# 454
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 462
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp97=x;struct _fat_ptr*_tmp99;union Cyc_Absyn_Nmspace _tmp98;_LL1: _tmp98=_tmp97->f1;_tmp99=_tmp97->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmp98;struct _fat_ptr*v=_tmp99;
union Cyc_Absyn_Nmspace _tmp9A=ns;struct Cyc_List_List*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9D;switch((_tmp9A.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmp9D=0;goto _LL7;case 1U: _LL6: _tmp9D=(_tmp9A.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmp9D;
_tmp9C=vs;goto _LL9;}case 2U: _LL8: _tmp9C=(_tmp9A.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmp9C;
_tmp9B=vs;goto _LLB;}default: _LLA: _tmp9B=(_tmp9A.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmp9B;
# 471
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp743;_tmp743.tag=0U,_tmp743.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp743;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp742;_tmp742.tag=0U,_tmp742.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp742;});void*_tmp9E[2U];_tmp9E[0]=& _tmpA0,_tmp9E[1]=& _tmpA1;({struct _fat_ptr _tmp791=({const char*_tmp9F="%s_%s_struct";_tag_fat(_tmp9F,sizeof(char),13U);});Cyc_aprintf(_tmp791,_tag_fat(_tmp9E,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA2=_new_region("r");struct _RegionHandle*r=& _tmpA2;_push_region(r);
{struct _fat_ptr _tmpA9=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp746;_tmp746.tag=0U,_tmp746.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp746;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp745;_tmp745.tag=0U,({struct _fat_ptr _tmp794=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp793=r;struct Cyc_List_List*_tmp792=vs;Cyc_rstr_sepstr(_tmp793,_tmp792,({const char*_tmpA8="_";_tag_fat(_tmpA8,sizeof(char),2U);}));}));_tmp745.f1=_tmp794;});_tmp745;});struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp744;_tmp744.tag=0U,_tmp744.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp744;});void*_tmpA3[3U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6,_tmpA3[2]=& _tmpA7;({struct _fat_ptr _tmp795=({const char*_tmpA4="%s_%s_%s_struct";_tag_fat(_tmpA4,sizeof(char),16U);});Cyc_aprintf(_tmp795,_tag_fat(_tmpA3,sizeof(void*),3U));});});_npop_handler(0U);return _tmpA9;};_pop_region(r);}}}_LL3:;}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 484
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 487
struct _tuple20 _tmpAA=*env;struct _tuple20 _stmttmp7=_tmpAA;struct _tuple20 _tmpAB=_stmttmp7;struct _tuple14*_tmpAD;struct Cyc_Dict_Dict*_tmpAC;_LL1: _tmpAC=(_tmpAB.f1)->qvar_tags;_tmpAD=_tmpAB.f2;_LL2: {struct Cyc_Dict_Dict*qvs=_tmpAC;struct _tuple14*pair=_tmpAD;
struct _tuple14 _tmpAE=*pair;struct _tuple14 _stmttmp8=_tmpAE;struct _tuple14 _tmpAF=_stmttmp8;struct _tuple1*_tmpB1;struct _tuple1*_tmpB0;_LL4: _tmpB0=_tmpAF.f1;_tmpB1=_tmpAF.f2;_LL5: {struct _tuple1*fieldname=_tmpB0;struct _tuple1*dtname=_tmpB1;
struct _handler_cons _tmpB2;_push_handler(& _tmpB2);{int _tmpB4=0;if(setjmp(_tmpB2.handler))_tmpB4=1;if(!_tmpB4){{struct _tuple1*_tmpB5=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0U);return _tmpB5;};_pop_handler();}else{void*_tmpB3=(void*)Cyc_Core_get_exn_thrown();void*_tmpB6=_tmpB3;void*_tmpB7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB6)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 491
struct _tuple14*_tmpB8=({struct _tuple14*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=fieldname,_tmpBE->f2=dtname;_tmpBE;});struct _tuple14*new_pair=_tmpB8;
struct _tuple1*_tmpB9=fieldname;struct _fat_ptr*_tmpBB;union Cyc_Absyn_Nmspace _tmpBA;_LLC: _tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;_LLD: {union Cyc_Absyn_Nmspace nmspace=_tmpBA;struct _fat_ptr*fieldvar=_tmpBB;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->f1=nmspace,({struct _fat_ptr*_tmp796=({struct _fat_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));*_tmpBC=newvar;_tmpBC;});_tmpBD->f2=_tmp796;});_tmpBD;});
({struct Cyc_Dict_Dict _tmp797=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp797;});
return res;}}}else{_LL9: _tmpB7=_tmpB6;_LLA: {void*exn=_tmpB7;(int)_rethrow(exn);}}_LL6:;}}}}}
# 499
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp747;_tmp747.f1=fieldname,_tmp747.f2=dtname;_tmp747;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 506
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->kind=Cyc_Absyn_StructA,_tmpC1->sc=Cyc_Absyn_Public,_tmpC1->tvs=0,_tmpC1->attributes=0,_tmpC1->expected_mem_kind=0,({
# 509
struct _tuple1*_tmp79A=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));({union Cyc_Absyn_Nmspace _tmp799=Cyc_Absyn_Rel_n(0);_tmpBF->f1=_tmp799;}),_tmpBF->f2=name;_tmpBF;});_tmpC1->name=_tmp79A;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp798=({struct Cyc_Absyn_AggrdeclImpl*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->exist_vars=0,_tmpC0->rgn_po=0,_tmpC0->fields=fs,_tmpC0->tagged=0;_tmpC0;});_tmpC1->impl=_tmp798;});_tmpC1;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 515
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 518
struct _tuple21 _tmpC2=*env;struct _tuple21 _stmttmp9=_tmpC2;struct _tuple21 _tmpC3=_stmttmp9;struct Cyc_List_List*_tmpC5;struct Cyc_List_List**_tmpC4;_LL1: _tmpC4=(_tmpC3.f1)->tuple_types;_tmpC5=_tmpC3.f2;_LL2: {struct Cyc_List_List**tuple_types=_tmpC4;struct Cyc_List_List*tqs0=_tmpC5;
# 520
{struct Cyc_List_List*_tmpC6=*tuple_types;struct Cyc_List_List*tts=_tmpC6;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_tmpC7=(struct _tuple22*)tts->hd;struct _tuple22*_stmttmpA=_tmpC7;struct _tuple22*_tmpC8=_stmttmpA;struct Cyc_List_List*_tmpCA;void*_tmpC9;_LL4: _tmpC9=_tmpC8->f1;_tmpCA=_tmpC8->f2;_LL5: {void*x=_tmpC9;struct Cyc_List_List*ts=_tmpCA;
struct Cyc_List_List*_tmpCB=tqs0;struct Cyc_List_List*tqs=_tmpCB;
for(0;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple12*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 531
struct _fat_ptr*xname=({struct _fat_ptr*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct _fat_ptr _tmp79C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD8=({struct Cyc_Int_pa_PrintArg_struct _tmp748;_tmp748.tag=1U,_tmp748.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp748;});void*_tmpD6[1U];_tmpD6[0]=& _tmpD8;({struct _fat_ptr _tmp79B=({const char*_tmpD7="_tuple%d";_tag_fat(_tmpD7,sizeof(char),9U);});Cyc_aprintf(_tmp79B,_tag_fat(_tmpD6,sizeof(void*),1U));});});*_tmpD9=_tmp79C;});_tmpD9;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmpCC=0;struct Cyc_List_List*fs=_tmpCC;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
fs=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct Cyc_Absyn_Aggrfield*_tmp79E=({struct Cyc_Absyn_Aggrfield*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct _fat_ptr*_tmp79D=Cyc_Absyn_fieldname(i);_tmpCD->name=_tmp79D;}),_tmpCD->tq=Cyc_Toc_mt_tq,_tmpCD->type=(void*)ts2->hd,_tmpCD->width=0,_tmpCD->attributes=0,_tmpCD->requires_clause=0;_tmpCD;});_tmpCE->hd=_tmp79E;}),_tmpCE->tl=fs;_tmpCE;});}}
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmpCF=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpCF;
void*_tmpD0=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpD5=_cycalloc(sizeof(*_tmpD5));*_tmpD5=sd;_tmpD5;})),0);void*ans=_tmpD0;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));({struct Cyc_Absyn_Decl*_tmp79F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=5U,_tmpD1->f1=sd;_tmpD1;}),0U);_tmpD2->hd=_tmp79F;}),_tmpD2->tl=Cyc_Toc_result_decls;_tmpD2;});
({struct Cyc_List_List*_tmp7A1=({struct Cyc_List_List*_tmpD4=_region_malloc(d,sizeof(*_tmpD4));({struct _tuple22*_tmp7A0=({struct _tuple22*_tmpD3=_region_malloc(d,sizeof(*_tmpD3));_tmpD3->f1=ans,_tmpD3->f2=ts;_tmpD3;});_tmpD4->hd=_tmp7A0;}),_tmpD4->tl=*tuple_types;_tmpD4;});*tuple_types=_tmp7A1;});
return ans;}}}}
# 544
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 549
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 552
struct _tuple24*_tmpDA=env;struct Cyc_List_List*_tmpDD;enum Cyc_Absyn_AggrKind _tmpDC;struct Cyc_List_List**_tmpDB;_LL1: _tmpDB=(_tmpDA->f1)->anon_aggr_types;_tmpDC=(_tmpDA->f2)->f1;_tmpDD=(_tmpDA->f2)->f2;_LL2: {struct Cyc_List_List**anon_aggr_types=_tmpDB;enum Cyc_Absyn_AggrKind ak=_tmpDC;struct Cyc_List_List*fs=_tmpDD;
# 554
{struct Cyc_List_List*_tmpDE=*anon_aggr_types;struct Cyc_List_List*ts=_tmpDE;for(0;ts != 0;ts=ts->tl){
struct _tuple25*_tmpDF=(struct _tuple25*)ts->hd;struct _tuple25*_stmttmpB=_tmpDF;struct _tuple25*_tmpE0=_stmttmpB;struct Cyc_List_List*_tmpE3;enum Cyc_Absyn_AggrKind _tmpE2;void*_tmpE1;_LL4: _tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;_tmpE3=_tmpE0->f3;_LL5: {void*x=_tmpE1;enum Cyc_Absyn_AggrKind ak2=_tmpE2;struct Cyc_List_List*fs2=_tmpE3;
if((int)ak != (int)ak2)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 563
struct _fat_ptr*xname=({struct _fat_ptr*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct _fat_ptr _tmp7A3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpED=({struct Cyc_Int_pa_PrintArg_struct _tmp749;_tmp749.tag=1U,_tmp749.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp749;});void*_tmpEB[1U];_tmpEB[0]=& _tmpED;({struct _fat_ptr _tmp7A2=({const char*_tmpEC="_tuple%d";_tag_fat(_tmpEC,sizeof(char),9U);});Cyc_aprintf(_tmp7A2,_tag_fat(_tmpEB,sizeof(void*),1U));});});*_tmpEE=_tmp7A3;});_tmpEE;});
struct Cyc_Absyn_Aggrdecl*_tmpE4=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpE4;
sd->kind=ak;{
void*_tmpE5=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpEA=_cycalloc(sizeof(*_tmpEA));*_tmpEA=sd;_tmpEA;})),0);void*ans=_tmpE5;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_Decl*_tmp7A4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->tag=5U,_tmpE6->f1=sd;_tmpE6;}),0U);_tmpE7->hd=_tmp7A4;}),_tmpE7->tl=Cyc_Toc_result_decls;_tmpE7;});
({struct Cyc_List_List*_tmp7A6=({struct Cyc_List_List*_tmpE9=_region_malloc(d,sizeof(*_tmpE9));({struct _tuple25*_tmp7A5=({struct _tuple25*_tmpE8=_region_malloc(d,sizeof(*_tmpE8));_tmpE8->f1=ans,_tmpE8->f2=ak,_tmpE8->f3=fs;_tmpE8;});_tmpE9->hd=_tmp7A5;}),_tmpE9->tl=*anon_aggr_types;_tmpE9;});*anon_aggr_types=_tmp7A6;});
return ans;}}}}
# 571
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->f1=ak,_tmpEF->f2=fs;_tmpEF;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 580
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 589
struct _tuple27 _tmpF0=*env;struct _tuple27 _stmttmpC=_tmpF0;struct _tuple27 _tmpF1=_stmttmpC;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF4;struct _tuple1*_tmpF3;struct Cyc_List_List**_tmpF2;_LL1: _tmpF2=(_tmpF1.f1)->abs_struct_types;_tmpF3=(_tmpF1.f2)->f1;_tmpF4=(_tmpF1.f2)->f2;_tmpF5=(_tmpF1.f2)->f3;_tmpF6=(_tmpF1.f2)->f4;_LL2: {struct Cyc_List_List**abs_struct_types=_tmpF2;struct _tuple1*struct_name=_tmpF3;struct Cyc_List_List*type_vars=_tmpF4;struct Cyc_List_List*type_args=_tmpF5;struct Cyc_List_List*fields=_tmpF6;
# 593
{struct Cyc_List_List*_tmpF7=*abs_struct_types;struct Cyc_List_List*tts=_tmpF7;for(0;tts != 0;tts=tts->tl){
struct _tuple28*_tmpF8=(struct _tuple28*)tts->hd;struct _tuple28*_stmttmpD=_tmpF8;struct _tuple28*_tmpF9=_stmttmpD;void*_tmpFC;struct Cyc_List_List*_tmpFB;struct _tuple1*_tmpFA;_LL4: _tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;_tmpFC=_tmpF9->f3;_LL5: {struct _tuple1*x=_tmpFA;struct Cyc_List_List*ts2=_tmpFB;void*t=_tmpFC;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp7A7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args);_tmp7A7 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmpFD=type_args;struct Cyc_List_List*ts=_tmpFD;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmpFE=(void*)ts->hd;void*t=_tmpFE;
void*_tmpFF=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmpFF;
{struct Cyc_Absyn_Kind*_tmp100=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp100;struct Cyc_Absyn_Kind*_tmp101=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp101)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 606
 if(Cyc_Unify_unify(t,t2)||({void*_tmp7A8=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp7A8,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 611
break;}}
# 613
if(okay)
return t;}}}}{
# 620
struct _fat_ptr*xname=({struct _fat_ptr*_tmp114=_cycalloc(sizeof(*_tmp114));({struct _fat_ptr _tmp7AA=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp113=({struct Cyc_Int_pa_PrintArg_struct _tmp74A;_tmp74A.tag=1U,_tmp74A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp74A;});void*_tmp111[1U];_tmp111[0]=& _tmp113;({struct _fat_ptr _tmp7A9=({const char*_tmp112="_tuple%d";_tag_fat(_tmp112,sizeof(char),9U);});Cyc_aprintf(_tmp7A9,_tag_fat(_tmp111,sizeof(void*),1U));});});*_tmp114=_tmp7AA;});_tmp114;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp102=0;struct Cyc_List_List*fs=_tmp102;
# 624
({struct Cyc_List_List*_tmp7AC=({struct Cyc_List_List*_tmp104=_region_malloc(d,sizeof(*_tmp104));({struct _tuple28*_tmp7AB=({struct _tuple28*_tmp103=_region_malloc(d,sizeof(*_tmp103));_tmp103->f1=struct_name,_tmp103->f2=type_args,_tmp103->f3=x;_tmp103;});_tmp104->hd=_tmp7AB;}),_tmp104->tl=*abs_struct_types;_tmp104;});*abs_struct_types=_tmp7AC;});{
# 627
struct _RegionHandle _tmp105=_new_region("r");struct _RegionHandle*r=& _tmp105;_push_region(r);
{struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp106;
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp107=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp107;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 637
if((fields->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp109=_cycalloc(sizeof(*_tmp109));({void*_tmp7AD=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->tag=6U,_tmp108->f1=0;_tmp108;});_tmp109->hd=_tmp7AD;}),_tmp109->tl=atts;_tmp109;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 644
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->tag=4U,({void*_tmp7B0=Cyc_Toc_void_star_type();(_tmp10A->f1).elt_type=_tmp7B0;}),({struct Cyc_Absyn_Tqual _tmp7AF=Cyc_Absyn_empty_tqual(0U);(_tmp10A->f1).tq=_tmp7AF;}),({
struct Cyc_Absyn_Exp*_tmp7AE=Cyc_Absyn_uint_exp(0U,0U);(_tmp10A->f1).num_elts=_tmp7AE;}),(_tmp10A->f1).zero_term=Cyc_Absyn_false_type,(_tmp10A->f1).zt_loc=0U;_tmp10A;});
# 648
fs=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Aggrfield*_tmp7B1=({struct Cyc_Absyn_Aggrfield*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->name=f->name,_tmp10B->tq=Cyc_Toc_mt_tq,_tmp10B->type=t,_tmp10B->width=f->width,_tmp10B->attributes=atts,_tmp10B->requires_clause=0;_tmp10B;});_tmp10C->hd=_tmp7B1;}),_tmp10C->tl=fs;_tmp10C;});}
# 650
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp10D=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp10D;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));({struct Cyc_Absyn_Decl*_tmp7B2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=5U,_tmp10E->f1=sd;_tmp10E;}),0U);_tmp10F->hd=_tmp7B2;}),_tmp10F->tl=Cyc_Toc_result_decls;_tmp10F;});{
void*_tmp110=x;_npop_handler(0U);return _tmp110;}}}
# 628
;_pop_region(r);}}}}
# 656
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 660
struct _tuple26 env=({struct _tuple26 _tmp74B;_tmp74B.f1=struct_name,_tmp74B.f2=type_vars,_tmp74B.f3=type_args,_tmp74B.f4=fields;_tmp74B;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 668
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp7B5=({struct _fat_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _fat_ptr _tmp7B4=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp74C;_tmp74C.tag=1U,_tmp74C.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp74C;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _fat_ptr _tmp7B3=({const char*_tmp116="_tmp%X";_tag_fat(_tmp116,sizeof(char),7U);});Cyc_aprintf(_tmp7B3,_tag_fat(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp7B4;});_tmp118;});_tmp119->f2=_tmp7B5;});_tmp119;});}struct _tuple29{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 671
struct _tuple29 Cyc_Toc_temp_var_and_exp(){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple29 _tmp74D;_tmp74D.f1=v,({struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Absyn_var_exp(v,0U);_tmp74D.f2=_tmp7B6;});_tmp74D;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 678
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp11A=*env;struct _tuple30 _stmttmpF=_tmp11A;struct _tuple30 _tmp11B=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp11C;_LL1: _tmp11C=(_tmp11B.f1)->temp_labels;_LL2: {struct Cyc_Xarray_Xarray*temp_labels=_tmp11C;
int _tmp11D=Cyc_Toc_fresh_label_counter ++;int i=_tmp11D;
if(({int _tmp7B7=i;_tmp7B7 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _fat_ptr _tmp7B9=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122=({struct Cyc_Int_pa_PrintArg_struct _tmp74E;_tmp74E.tag=1U,_tmp74E.f1=(unsigned)i;_tmp74E;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _fat_ptr _tmp7B8=({const char*_tmp121="_LL%X";_tag_fat(_tmp121,sizeof(char),6U);});Cyc_aprintf(_tmp7B8,_tag_fat(_tmp120,sizeof(void*),1U));});});*_tmp123=_tmp7B9;});_tmp123;});
if(({int _tmp7BA=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp7BA != i;}))
({void*_tmp11E=0U;({struct _fat_ptr _tmp7BB=({const char*_tmp11F="fresh_label: add_ind returned bad index...";_tag_fat(_tmp11F,sizeof(char),43U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7BB,_tag_fat(_tmp11E,sizeof(void*),0U));});});
return res;}}}
# 688
static struct _fat_ptr*Cyc_Toc_fresh_label(){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 694
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp124=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp124;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 701
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 707
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=(*a).f1,_tmp125->f2=(*a).f2,({void*_tmp7BC=Cyc_Toc_typ_to_c((*a).f3);_tmp125->f3=_tmp7BC;});_tmp125;});}
# 729 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp126=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp126;void*_tmp127=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp128;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp127)->tag == 4U){_LL1: _tmp128=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp127)->f1;_LL2: {struct Cyc_Absyn_ArrayInfo ai=_tmp128;
return({void*_tmp7BD=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp7BD,ai.tq);});}}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 736
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 738
struct Cyc_Absyn_Aggrfield*_tmp129=({struct Cyc_Absyn_Aggrfield*_tmp12A=_cycalloc(sizeof(*_tmp12A));*_tmp12A=*f;_tmp12A;});struct Cyc_Absyn_Aggrfield*ans=_tmp129;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 745
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 747
return;}
# 750
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp12B=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp12B;void*_tmp12C=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp12D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12C)->tag == 2U){_LL1: _tmp12D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12C)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp12D;
return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 756
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp12E=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp12E;struct Cyc_Absyn_Kind*_tmp12F=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp12F)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 762
static int Cyc_Toc_is_void_star(void*t){
void*_tmp130=Cyc_Tcutil_compress(t);void*_stmttmp13=_tmp130;void*_tmp131=_stmttmp13;void*_tmp132;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->tag == 3U){_LL1: _tmp132=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1).elt_type;_LL2: {void*t2=_tmp132;
return Cyc_Tcutil_is_void_type(t2);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 768
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 771
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 775
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp133=t;void**_tmp135;struct Cyc_Absyn_Datatypedecl*_tmp134;struct Cyc_Absyn_Enumdecl*_tmp136;struct Cyc_Absyn_Aggrdecl*_tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp139;void*_tmp13D;struct Cyc_Absyn_Typedefdecl*_tmp13C;struct Cyc_List_List*_tmp13B;struct _tuple1*_tmp13A;struct Cyc_List_List*_tmp13F;enum Cyc_Absyn_AggrKind _tmp13E;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_VarargInfo*_tmp145;int _tmp144;struct Cyc_List_List*_tmp143;void*_tmp142;struct Cyc_Absyn_Tqual _tmp141;unsigned _tmp14A;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Tqual _tmp148;void*_tmp147;void*_tmp14D;struct Cyc_Absyn_Tqual _tmp14C;void*_tmp14B;struct Cyc_Absyn_Tvar*_tmp14E;void**_tmp14F;struct Cyc_List_List*_tmp150;struct _tuple1*_tmp151;struct Cyc_List_List*_tmp154;union Cyc_Absyn_AggrInfo _tmp153;void*_tmp152;struct Cyc_Absyn_Datatypefield*_tmp156;struct Cyc_Absyn_Datatypedecl*_tmp155;switch(*((int*)_tmp133)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 788
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp155=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1).KnownDatatypefield).val).f1;_tmp156=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1).KnownDatatypefield).val).f2;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmp155;struct Cyc_Absyn_Datatypefield*tuf=_tmp156;
# 790
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{_LLB: _LLC:
# 792
({void*_tmp157=0U;({struct _fat_ptr _tmp7BE=({const char*_tmp158="unresolved DatatypeFieldType";_tag_fat(_tmp158,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7BE,_tag_fat(_tmp157,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 801
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp152=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1;_tmp153=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1;_tmp154=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f2;_LL1C: {void*c=_tmp152;union Cyc_Absyn_AggrInfo info=_tmp153;struct Cyc_List_List*ts=_tmp154;
# 848
{union Cyc_Absyn_AggrInfo _tmp168=info;if((_tmp168.UnknownAggr).tag == 1){_LL45: _LL46:
# 850
 if(ts == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->tag=0U,_tmp169->f1=c,_tmp169->f2=0;_tmp169;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 854
struct Cyc_Absyn_Aggrdecl*_tmp16A=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp16A;
if(ad->expected_mem_kind){
# 857
if(ad->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp16D=({struct Cyc_String_pa_PrintArg_struct _tmp750;_tmp750.tag=0U,({
struct _fat_ptr _tmp7BF=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_UnionA?({const char*_tmp16F="union";_tag_fat(_tmp16F,sizeof(char),6U);}):({const char*_tmp170="struct";_tag_fat(_tmp170,sizeof(char),7U);}));_tmp750.f1=_tmp7BF;});_tmp750;});struct Cyc_String_pa_PrintArg_struct _tmp16E=({struct Cyc_String_pa_PrintArg_struct _tmp74F;_tmp74F.tag=0U,({
struct _fat_ptr _tmp7C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp74F.f1=_tmp7C0;});_tmp74F;});void*_tmp16B[2U];_tmp16B[0]=& _tmp16D,_tmp16B[1]=& _tmp16E;({struct _fat_ptr _tmp7C1=({const char*_tmp16C="%s %s was never defined.";_tag_fat(_tmp16C,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp7C1,_tag_fat(_tmp16B,sizeof(void*),2U));});});}
# 863
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp171=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp171;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp172=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp172;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp175=({struct Cyc_String_pa_PrintArg_struct _tmp751;_tmp751.tag=0U,({struct _fat_ptr _tmp7C2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp751.f1=_tmp7C2;});_tmp751;});void*_tmp173[1U];_tmp173[0]=& _tmp175;({struct _fat_ptr _tmp7C3=({const char*_tmp174="struct %s ended up being abstract.";_tag_fat(_tmp174,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp7C3,_tag_fat(_tmp173,sizeof(void*),1U));});});{
# 876
struct _RegionHandle _tmp176=_new_region("r");struct _RegionHandle*r=& _tmp176;_push_region(r);
{struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp177;
void*_tmp178=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp178;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp179=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp179;}{
void*_tmp17A=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0U);return _tmp17A;}}
# 877
;_pop_region(r);}}
# 882
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15U: _LL1D: _tmp151=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1;_LL1E: {struct _tuple1*tdn=_tmp151;
return t;}case 16U: _LL1F: _tmp150=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1;_LL20: {struct Cyc_List_List*fs=_tmp150;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4U: _LL23: _LL24:
# 894
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 899
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp14F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp133)->f2;_LL4: {void**t2=_tmp14F;
# 779
if(*t2 == 0){
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 783
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));}case 2U: _LL5: _tmp14E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp133)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp14E;
# 785
if((int)(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;else{
return Cyc_Toc_void_star_type();}}case 3U: _LLD: _tmp14B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_type;_tmp14C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_tq;_tmp14D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).ptr_atts).bounds;_LLE: {void*t2=_tmp14B;struct Cyc_Absyn_Tqual tq=_tmp14C;void*bnds=_tmp14D;
# 796
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 800
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}}case 4U: _LL13: _tmp147=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).elt_type;_tmp148=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).tq;_tmp149=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).num_elts;_tmp14A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).zt_loc;_LL14: {void*t2=_tmp147;struct Cyc_Absyn_Tqual tq=_tmp148;struct Cyc_Absyn_Exp*e=_tmp149;unsigned ztl=_tmp14A;
# 804
return({void*_tmp7C7=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp7C6=tq;struct Cyc_Absyn_Exp*_tmp7C5=e;void*_tmp7C4=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp7C7,_tmp7C6,_tmp7C5,_tmp7C4,ztl);});}case 5U: _LL15: _tmp141=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).ret_tqual;_tmp142=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).ret_type;_tmp143=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).args;_tmp144=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).c_varargs;_tmp145=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).cyc_varargs;_tmp146=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).attributes;_LL16: {struct Cyc_Absyn_Tqual tq2=_tmp141;void*t2=_tmp142;struct Cyc_List_List*args=_tmp143;int c_vararg=_tmp144;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp145;struct Cyc_List_List*atts=_tmp146;
# 810
struct Cyc_List_List*_tmp159=0;struct Cyc_List_List*new_atts=_tmp159;
for(0;atts != 0;atts=atts->tl){
void*_tmp15A=(void*)atts->hd;void*_stmttmp14=_tmp15A;void*_tmp15B=_stmttmp14;switch(*((int*)_tmp15B)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
 new_atts=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=(void*)atts->hd,_tmp15C->tl=new_atts;_tmp15C;});goto _LL35;}_LL35:;}{
# 821
struct Cyc_List_List*_tmp15D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp15D;
if(cyc_vararg != 0){
# 824
void*_tmp15E=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));void*t=_tmp15E;
struct _tuple9*_tmp15F=({struct _tuple9*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->f1=cyc_vararg->name,_tmp161->f2=cyc_vararg->tq,_tmp161->f3=t;_tmp161;});struct _tuple9*vararg=_tmp15F;
new_args=({struct Cyc_List_List*_tmp7C8=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7C8,({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->hd=vararg,_tmp160->tl=0;_tmp160;}));});}
# 828
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->tag=5U,(_tmp162->f1).tvars=0,(_tmp162->f1).effect=0,(_tmp162->f1).ret_tqual=tq2,({void*_tmp7C9=Cyc_Toc_typ_to_c(t2);(_tmp162->f1).ret_type=_tmp7C9;}),(_tmp162->f1).args=new_args,(_tmp162->f1).c_varargs=c_vararg,(_tmp162->f1).cyc_varargs=0,(_tmp162->f1).rgn_po=0,(_tmp162->f1).attributes=new_atts,(_tmp162->f1).requires_clause=0,(_tmp162->f1).requires_relns=0,(_tmp162->f1).ensures_clause=0,(_tmp162->f1).ensures_relns=0;_tmp162;});}}case 6U: _LL17: _tmp140=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp133)->f1;_LL18: {struct Cyc_List_List*tqs=_tmp140;
# 833
struct Cyc_List_List*_tmp163=0;struct Cyc_List_List*tqs2=_tmp163;
for(0;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp165=_cycalloc(sizeof(*_tmp165));({struct _tuple12*_tmp7CB=({struct _tuple12*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->f1=(*((struct _tuple12*)tqs->hd)).f1,({void*_tmp7CA=Cyc_Toc_typ_to_c((*((struct _tuple12*)tqs->hd)).f2);_tmp164->f2=_tmp7CA;});_tmp164;});_tmp165->hd=_tmp7CB;}),_tmp165->tl=tqs2;_tmp165;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tqs2));}case 7U: _LL19: _tmp13E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp133)->f1;_tmp13F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp133)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp13E;struct Cyc_List_List*fs=_tmp13F;
# 842
struct Cyc_List_List*_tmp166=0;struct Cyc_List_List*fs2=_tmp166;
for(0;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp167=_cycalloc(sizeof(*_tmp167));({struct Cyc_Absyn_Aggrfield*_tmp7CD=({struct Cyc_Absyn_Aggrfield*_tmp7CC=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp7CC,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp167->hd=_tmp7CD;}),_tmp167->tl=fs2;_tmp167;});}
return({enum Cyc_Absyn_AggrKind _tmp7CE=k;Cyc_Toc_add_anon_aggr_type(_tmp7CE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs2));});}case 8U: _LL21: _tmp13A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f1;_tmp13B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f2;_tmp13C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f3;_tmp13D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f4;_LL22: {struct _tuple1*tdn=_tmp13A;struct Cyc_List_List*ts=_tmp13B;struct Cyc_Absyn_Typedefdecl*td=_tmp13C;void*topt=_tmp13D;
# 888
if(topt == 0){
if(ts != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->tag=8U,_tmp17B->f1=tdn,_tmp17B->f2=0,_tmp17B->f3=td,_tmp17B->f4=0;_tmp17B;});else{
return t;}}else{
# 893
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->tag=8U,_tmp17C->f1=tdn,_tmp17C->f2=0,_tmp17C->f3=td,({void*_tmp7CF=Cyc_Toc_typ_to_c(topt);_tmp17C->f4=_tmp7CF;});_tmp17C;});}}case 9U: _LL2B: _tmp139=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp133)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp139;
# 904
return t;}case 11U: _LL2D: _tmp138=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp133)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp138;
# 909
if(e->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)){case 0U: _LL2F: _tmp137=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)->f1;_LL30: {struct Cyc_Absyn_Aggrdecl*ad=_tmp137;
# 913
Cyc_Toc_aggrdecl_to_c(ad);
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp136=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)->f1;_LL32: {struct Cyc_Absyn_Enumdecl*ed=_tmp136;
# 918
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _LL33: _tmp134=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)->f1;_tmp135=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f2;_LL34: {struct Cyc_Absyn_Datatypedecl*dd=_tmp134;void**t=_tmp135;
# 921
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}}_LL0:;}
# 926
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp17D=t;struct Cyc_Absyn_Tqual _tmp17F;void*_tmp17E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->tag == 4U){_LL1: _tmp17E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->f1).elt_type;_tmp17F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->f1).tq;_LL2: {void*t2=_tmp17E;struct Cyc_Absyn_Tqual tq=_tmp17F;
# 929
return({void*_tmp7D0=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp7D0,e);});}}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 936
static int Cyc_Toc_atomic_type(void*t){
void*_tmp180=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp180;void*_tmp181=_stmttmp15;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp183;void*_tmp184;struct Cyc_List_List*_tmp186;void*_tmp185;switch(*((int*)_tmp181)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp185=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f1;_tmp186=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2;_LL4: {void*c=_tmp185;struct Cyc_List_List*ts=_tmp186;
# 940
void*_tmp187=c;struct Cyc_Absyn_Datatypefield*_tmp189;struct Cyc_Absyn_Datatypedecl*_tmp188;union Cyc_Absyn_AggrInfo _tmp18A;switch(*((int*)_tmp187)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp18A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp187)->f1;_LL25: {union Cyc_Absyn_AggrInfo info=_tmp18A;
# 947
{union Cyc_Absyn_AggrInfo _tmp18B=info;if((_tmp18B.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 951
struct Cyc_Absyn_Aggrdecl*_tmp18C=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp18C;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp18D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp18D;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp187)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp188=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp187)->f1).KnownDatatypefield).val).f1;_tmp189=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp187)->f1).KnownDatatypefield).val).f2;_LL27: {struct Cyc_Absyn_Datatypedecl*tud=_tmp188;struct Cyc_Absyn_Datatypefield*tuf=_tmp189;
# 958
{struct Cyc_List_List*_tmp18E=tuf->typs;struct Cyc_List_List*tqs=_tmp18E;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp191=({struct Cyc_String_pa_PrintArg_struct _tmp752;_tmp752.tag=0U,({struct _fat_ptr _tmp7D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp752.f1=_tmp7D1;});_tmp752;});void*_tmp18F[1U];_tmp18F[0]=& _tmp191;({struct _fat_ptr _tmp7D2=({const char*_tmp190="atomic_typ:  bad type %s";_tag_fat(_tmp190,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D2,_tag_fat(_tmp18F,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 963
 return 1;case 4U: _LL7: _tmp184=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp181)->f1).elt_type;_LL8: {void*t=_tmp184;
return Cyc_Toc_atomic_type(t);}case 7U: _LL9: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp181)->f2;_LLA: {struct Cyc_List_List*fs=_tmp183;
# 971
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6U: _LLB: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp181)->f1;_LLC: {struct Cyc_List_List*tqs=_tmp182;
# 975
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
return 1;}case 3U: _LLD: _LLE:
# 980
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp194=({struct Cyc_String_pa_PrintArg_struct _tmp753;_tmp753.tag=0U,({struct _fat_ptr _tmp7D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp753.f1=_tmp7D3;});_tmp753;});void*_tmp192[1U];_tmp192[0]=& _tmp194;({struct _fat_ptr _tmp7D4=({const char*_tmp193="atomic_typ:  bad type %s";_tag_fat(_tmp193,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D4,_tag_fat(_tmp192,sizeof(void*),1U));});});}_LL0:;}
# 986
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp195=Cyc_Tcutil_compress(t);void*_stmttmp16=_tmp195;void*_tmp196=_stmttmp16;struct Cyc_List_List*_tmp197;union Cyc_Absyn_AggrInfo _tmp198;switch(*((int*)_tmp196)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f1)->tag == 20U){_LL1: _tmp198=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp198;
# 989
struct Cyc_Absyn_Aggrdecl*_tmp199=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp199;
if(ad->impl == 0)
({void*_tmp19A=0U;({struct _fat_ptr _tmp7D5=({const char*_tmp19B="is_poly_field: type missing fields";_tag_fat(_tmp19B,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D5,_tag_fat(_tmp19A,sizeof(void*),0U));});});
_tmp197=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp197=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp196)->f2;_LL4: {struct Cyc_List_List*fs=_tmp197;
# 994
struct Cyc_Absyn_Aggrfield*_tmp19C=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp19C;
if(field == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp754;_tmp754.tag=0U,_tmp754.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp754;});void*_tmp19D[1U];_tmp19D[0]=& _tmp19F;({struct _fat_ptr _tmp7D6=({const char*_tmp19E="is_poly_field: bad field %s";_tag_fat(_tmp19E,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D6,_tag_fat(_tmp19D,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1A2=({struct Cyc_String_pa_PrintArg_struct _tmp755;_tmp755.tag=0U,({struct _fat_ptr _tmp7D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp755.f1=_tmp7D7;});_tmp755;});void*_tmp1A0[1U];_tmp1A0[0]=& _tmp1A2;({struct _fat_ptr _tmp7D8=({const char*_tmp1A1="is_poly_field: bad type %s";_tag_fat(_tmp1A1,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D8,_tag_fat(_tmp1A0,sizeof(void*),1U));});});}_LL0:;}
# 1005
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1A3=e->r;void*_stmttmp17=_tmp1A3;void*_tmp1A4=_stmttmp17;struct _fat_ptr*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A5;struct _fat_ptr*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A7;switch(*((int*)_tmp1A4)){case 21U: _LL1: _tmp1A7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A4)->f1;_tmp1A8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A4)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp1A7;struct _fat_ptr*f=_tmp1A8;
# 1008
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22U: _LL3: _tmp1A5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A4)->f1;_tmp1A6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A4)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1A5;struct _fat_ptr*f=_tmp1A6;
# 1011
void*_tmp1A9=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp18=_tmp1A9;void*_tmp1AA=_stmttmp18;void*_tmp1AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->tag == 3U){_LL8: _tmp1AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->f1).elt_type;_LL9: {void*t=_tmp1AB;
# 1013
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp756;_tmp756.tag=0U,({struct _fat_ptr _tmp7D9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp756.f1=_tmp7D9;});_tmp756;});void*_tmp1AC[1U];_tmp1AC[0]=& _tmp1AE;({struct _fat_ptr _tmp7DA=({const char*_tmp1AD="is_poly_project: bad type %s";_tag_fat(_tmp1AD,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7DA,_tag_fat(_tmp1AC,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1016
 return 0;}_LL0:;}
# 1021
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1AF[1U];_tmp1AF[0]=s;({struct Cyc_Absyn_Exp*_tmp7DB=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7DB,_tag_fat(_tmp1AF,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1026
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1B0[2U];_tmp1B0[0]=s,_tmp1B0[1]=n;({struct Cyc_Absyn_Exp*_tmp7DC=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7DC,_tag_fat(_tmp1B0,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1031
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1B1[2U];_tmp1B1[0]=rgn,_tmp1B1[1]=s;({struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7DD,_tag_fat(_tmp1B1,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1035
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1B2[2U];_tmp1B2[0]=rgn,_tmp1B2[1]=s;({struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7DE,_tag_fat(_tmp1B2,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1B3[3U];_tmp1B3[0]=rgn,_tmp1B3[1]=s,_tmp1B3[2]=n;({struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp7DF,_tag_fat(_tmp1B3,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1043
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({void*_tmp1B4=0U;({struct Cyc_Absyn_Exp*_tmp7E0=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp7E0,_tag_fat(_tmp1B4,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1048
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1056
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1B5=e->r;void*_stmttmp19=_tmp1B5;void*_tmp1B6=_stmttmp19;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1B6)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1B6)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1061
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));({struct Cyc_Absyn_Decl*_tmp7E1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->tag=0U,_tmp1B7->f1=vd;_tmp1B7;}),0U);_tmp1B8->hd=_tmp7E1;}),_tmp1B8->tl=Cyc_Toc_result_decls;_tmp1B8;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1069
 xexp=({void*_tmp7E2=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7E2,e);});
# 1071
xplussz=({void*_tmp7E3=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7E3,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1074
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1B9[3U];({struct _tuple19*_tmp7E6=({struct _tuple19*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->f1=0,_tmp1BA->f2=xexp;_tmp1BA;});_tmp1B9[0]=_tmp7E6;}),({
struct _tuple19*_tmp7E5=({struct _tuple19*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->f1=0,_tmp1BB->f2=xexp;_tmp1BB;});_tmp1B9[1]=_tmp7E5;}),({
struct _tuple19*_tmp7E4=({struct _tuple19*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->f1=0,_tmp1BC->f2=xplussz;_tmp1BC;});_tmp1B9[2]=_tmp7E4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B9,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1109 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE->break_lab=0,_tmp1BE->continue_lab=0,_tmp1BE->fallthru_info=0,_tmp1BE->toplevel=1,({int*_tmp7E7=({int*_tmp1BD=_region_malloc(r,sizeof(*_tmp1BD));*_tmp1BD=0;_tmp1BD;});_tmp1BE->in_lhs=_tmp7E7;}),_tmp1BE->rgn=r;_tmp1BE;});}
# 1113
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1BF=nv;int _tmp1C0;_LL1: _tmp1C0=_tmp1BF->toplevel;_LL2: {int t=_tmp1C0;
return t;}}
# 1117
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1C1=e;int*_tmp1C6;int _tmp1C5;struct Cyc_Toc_FallthruInfo*_tmp1C4;struct _fat_ptr**_tmp1C3;struct _fat_ptr**_tmp1C2;_LL1: _tmp1C2=_tmp1C1->break_lab;_tmp1C3=_tmp1C1->continue_lab;_tmp1C4=_tmp1C1->fallthru_info;_tmp1C5=_tmp1C1->toplevel;_tmp1C6=_tmp1C1->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1C2;struct _fat_ptr**c=_tmp1C3;struct Cyc_Toc_FallthruInfo*f=_tmp1C4;int t=_tmp1C5;int*lhs=_tmp1C6;
return({struct Cyc_Toc_Env*_tmp1C7=_region_malloc(r,sizeof(*_tmp1C7));_tmp1C7->break_lab=b,_tmp1C7->continue_lab=c,_tmp1C7->fallthru_info=f,_tmp1C7->toplevel=0,_tmp1C7->in_lhs=lhs,_tmp1C7->rgn=r;_tmp1C7;});}}
# 1121
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1C8=e;int*_tmp1CD;int _tmp1CC;struct Cyc_Toc_FallthruInfo*_tmp1CB;struct _fat_ptr**_tmp1CA;struct _fat_ptr**_tmp1C9;_LL1: _tmp1C9=_tmp1C8->break_lab;_tmp1CA=_tmp1C8->continue_lab;_tmp1CB=_tmp1C8->fallthru_info;_tmp1CC=_tmp1C8->toplevel;_tmp1CD=_tmp1C8->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1C9;struct _fat_ptr**c=_tmp1CA;struct Cyc_Toc_FallthruInfo*f=_tmp1CB;int t=_tmp1CC;int*lhs=_tmp1CD;
return({struct Cyc_Toc_Env*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE->break_lab=b,_tmp1CE->continue_lab=c,_tmp1CE->fallthru_info=f,_tmp1CE->toplevel=1,_tmp1CE->in_lhs=lhs,_tmp1CE->rgn=r;_tmp1CE;});}}
# 1125
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1CF=nv;int*_tmp1D0;_LL1: _tmp1D0=_tmp1CF->in_lhs;_LL2: {int*b=_tmp1D0;
return*b;}}
# 1129
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1D1=e;int*_tmp1D2;_LL1: _tmp1D2=_tmp1D1->in_lhs;_LL2: {int*lhs=_tmp1D2;
*lhs=b;}}
# 1134
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D3=e;int*_tmp1D8;int _tmp1D7;struct Cyc_Toc_FallthruInfo*_tmp1D6;struct _fat_ptr**_tmp1D5;struct _fat_ptr**_tmp1D4;_LL1: _tmp1D4=_tmp1D3->break_lab;_tmp1D5=_tmp1D3->continue_lab;_tmp1D6=_tmp1D3->fallthru_info;_tmp1D7=_tmp1D3->toplevel;_tmp1D8=_tmp1D3->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D4;struct _fat_ptr**c=_tmp1D5;struct Cyc_Toc_FallthruInfo*f=_tmp1D6;int t=_tmp1D7;int*lhs=_tmp1D8;
return({struct Cyc_Toc_Env*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));_tmp1D9->break_lab=b,_tmp1D9->continue_lab=c,_tmp1D9->fallthru_info=f,_tmp1D9->toplevel=t,_tmp1D9->in_lhs=lhs,_tmp1D9->rgn=r;_tmp1D9;});}}
# 1141
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DA=e;int*_tmp1DF;int _tmp1DE;struct Cyc_Toc_FallthruInfo*_tmp1DD;struct _fat_ptr**_tmp1DC;struct _fat_ptr**_tmp1DB;_LL1: _tmp1DB=_tmp1DA->break_lab;_tmp1DC=_tmp1DA->continue_lab;_tmp1DD=_tmp1DA->fallthru_info;_tmp1DE=_tmp1DA->toplevel;_tmp1DF=_tmp1DA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1DB;struct _fat_ptr**c=_tmp1DC;struct Cyc_Toc_FallthruInfo*f=_tmp1DD;int t=_tmp1DE;int*lhs=_tmp1DF;
return({struct Cyc_Toc_Env*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->break_lab=0,_tmp1E0->continue_lab=0,_tmp1E0->fallthru_info=f,_tmp1E0->toplevel=t,_tmp1E0->in_lhs=lhs,_tmp1E0->rgn=r;_tmp1E0;});}}
# 1147
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1151
struct Cyc_Toc_Env*_tmp1E1=e;int*_tmp1E6;int _tmp1E5;struct Cyc_Toc_FallthruInfo*_tmp1E4;struct _fat_ptr**_tmp1E3;struct _fat_ptr**_tmp1E2;_LL1: _tmp1E2=_tmp1E1->break_lab;_tmp1E3=_tmp1E1->continue_lab;_tmp1E4=_tmp1E1->fallthru_info;_tmp1E5=_tmp1E1->toplevel;_tmp1E6=_tmp1E1->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1E2;struct _fat_ptr**c=_tmp1E3;struct Cyc_Toc_FallthruInfo*f=_tmp1E4;int t=_tmp1E5;int*lhs=_tmp1E6;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));
_tmp1E9->label=fallthru_l,_tmp1E9->binders=fallthru_binders;_tmp1E9;});
return({struct Cyc_Toc_Env*_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));({struct _fat_ptr**_tmp7E8=({struct _fat_ptr**_tmp1E7=_region_malloc(r,sizeof(*_tmp1E7));*_tmp1E7=break_l;_tmp1E7;});_tmp1E8->break_lab=_tmp7E8;}),_tmp1E8->continue_lab=c,_tmp1E8->fallthru_info=fi,_tmp1E8->toplevel=t,_tmp1E8->in_lhs=lhs,_tmp1E8->rgn=r;_tmp1E8;});}}
# 1156
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1158
struct Cyc_Toc_Env*_tmp1EA=e;int*_tmp1EF;int _tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1ED;struct _fat_ptr**_tmp1EC;struct _fat_ptr**_tmp1EB;_LL1: _tmp1EB=_tmp1EA->break_lab;_tmp1EC=_tmp1EA->continue_lab;_tmp1ED=_tmp1EA->fallthru_info;_tmp1EE=_tmp1EA->toplevel;_tmp1EF=_tmp1EA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1EB;struct _fat_ptr**c=_tmp1EC;struct Cyc_Toc_FallthruInfo*f=_tmp1ED;int t=_tmp1EE;int*lhs=_tmp1EF;
return({struct Cyc_Toc_Env*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));({struct _fat_ptr**_tmp7E9=({struct _fat_ptr**_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));*_tmp1F0=break_l;_tmp1F0;});_tmp1F1->break_lab=_tmp7E9;}),_tmp1F1->continue_lab=c,_tmp1F1->fallthru_info=0,_tmp1F1->toplevel=t,_tmp1F1->in_lhs=lhs,_tmp1F1->rgn=r;_tmp1F1;});}}
# 1165
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1167
struct Cyc_Toc_Env*_tmp1F2=e;int*_tmp1F7;int _tmp1F6;struct Cyc_Toc_FallthruInfo*_tmp1F5;struct _fat_ptr**_tmp1F4;struct _fat_ptr**_tmp1F3;_LL1: _tmp1F3=_tmp1F2->break_lab;_tmp1F4=_tmp1F2->continue_lab;_tmp1F5=_tmp1F2->fallthru_info;_tmp1F6=_tmp1F2->toplevel;_tmp1F7=_tmp1F2->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1F3;struct _fat_ptr**c=_tmp1F4;struct Cyc_Toc_FallthruInfo*f=_tmp1F5;int t=_tmp1F6;int*lhs=_tmp1F7;
return({struct Cyc_Toc_Env*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));_tmp1F9->break_lab=0,_tmp1F9->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp7EA=({struct Cyc_Toc_FallthruInfo*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));_tmp1F8->label=next_l,_tmp1F8->binders=0;_tmp1F8;});_tmp1F9->fallthru_info=_tmp7EA;}),_tmp1F9->toplevel=t,_tmp1F9->in_lhs=lhs,_tmp1F9->rgn=r;_tmp1F9;});}}
# 1180 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1183
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->annot;void*_stmttmp1A=_tmp1FA;void*_tmp1FB=_stmttmp1A;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp1FC=0U;({unsigned _tmp7EC=e->loc;struct _fat_ptr _tmp7EB=({const char*_tmp1FD="Toc: do_null_check";_tag_fat(_tmp1FD,sizeof(char),19U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp7EC,_tmp7EB,_tag_fat(_tmp1FC,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1199
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1201
int ans;
int _tmp1FE=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp1FE;
void*_tmp1FF=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp1FF;
void*_tmp200=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp200;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp201=old_typ;void*_tmp205;void*_tmp204;struct Cyc_Absyn_Tqual _tmp203;void*_tmp202;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->tag == 3U){_LL1: _tmp202=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).elt_type;_tmp203=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).elt_tq;_tmp204=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).ptr_atts).bounds;_tmp205=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).ptr_atts).zero_term;_LL2: {void*ta=_tmp202;struct Cyc_Absyn_Tqual tq=_tmp203;void*b=_tmp204;void*zt=_tmp205;
# 1212
{void*_tmp206=annot;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*_tmp208;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1214
 if(!((unsigned)({void*_tmp7ED=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7ED,b);}))){
# 1217
void*_tmp209=({void*_tmp7EE=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp7EE,tq);});void*newt=_tmp209;
({void*_tmp7F1=({void*_tmp7F0=newt;Cyc_Toc_cast_it_r(_tmp7F0,({struct Cyc_Absyn_Exp*_tmp7EF=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7EF,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp7F1;});
ptr->topt=newt;}
# 1221
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1224
 if(!((unsigned)({void*_tmp7F2=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7F2,b);}))){
# 1226
void*_tmp20A=({void*_tmp7F3=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp7F3,tq);});void*newt=_tmp20A;
({void*_tmp7F6=({void*_tmp7F5=newt;Cyc_Toc_cast_it_r(_tmp7F5,({struct Cyc_Absyn_Exp*_tmp7F4=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7F4,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp7F6;});
ptr->topt=newt;
# 1233
if(index != 0)
({void*_tmp20B=0U;({struct _fat_ptr _tmp7F7=({const char*_tmp20C="subscript of ? with no bounds check but need null check";_tag_fat(_tmp20C,sizeof(char),56U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7F7,_tag_fat(_tmp20B,sizeof(void*),0U));});});}
# 1237
({void*_tmp7FB=({void*_tmp7FA=new_typ;Cyc_Toc_cast_it_r(_tmp7FA,({struct Cyc_Absyn_Exp*_tmp20D[1U];({struct Cyc_Absyn_Exp*_tmp7F8=Cyc_Absyn_new_exp(ptr->r,0U);_tmp20D[0]=_tmp7F8;});({struct Cyc_Absyn_Exp*_tmp7F9=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7F9,_tag_fat(_tmp20D,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp7FB;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1243
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp800=({void*_tmp7FF=newt;Cyc_Toc_cast_it_r(_tmp7FF,({struct Cyc_Absyn_Exp*_tmp20E[3U];({
struct Cyc_Absyn_Exp*_tmp7FD=Cyc_Absyn_new_exp(ptr->r,0U);_tmp20E[0]=_tmp7FD;}),({
struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp20E[1]=_tmp7FC;}),_tmp20E[2]=ind;({struct Cyc_Absyn_Exp*_tmp7FE=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp7FE,_tag_fat(_tmp20E,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1246
ptr->r=_tmp800;});
# 1250
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp208=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp206)->f1;_LLF: {struct Cyc_Absyn_Exp*bd=_tmp208;
# 1254
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp207=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp206)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp207=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp206)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp207;
# 1257
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1262
struct _tuple13 _tmp20F=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _stmttmp1B=_tmp20F;struct _tuple13 _tmp210=_stmttmp1B;int _tmp212;unsigned _tmp211;_LL15: _tmp211=_tmp210.f1;_tmp212=_tmp210.f2;_LL16: {unsigned i=_tmp211;int valid=_tmp212;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1265
({void*_tmp805=({void*_tmp804=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp804,({struct Cyc_Absyn_Exp*_tmp213[4U];({
struct Cyc_Absyn_Exp*_tmp802=Cyc_Absyn_new_exp(ptr->r,0U);_tmp213[0]=_tmp802;}),_tmp213[1]=bd,({
struct Cyc_Absyn_Exp*_tmp801=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp213[2]=_tmp801;}),_tmp213[3]=ind;({struct Cyc_Absyn_Exp*_tmp803=fn_e;Cyc_Toc_fncall_exp_dl(_tmp803,_tag_fat(_tmp213,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1265
ptr->r=_tmp805;});
# 1268
ans=1;}else{
# 1271
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1274
({void*_tmp809=({void*_tmp808=new_typ;Cyc_Toc_cast_it_r(_tmp808,({struct Cyc_Absyn_Exp*_tmp214[1U];({struct Cyc_Absyn_Exp*_tmp806=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp214[0]=_tmp806;});({struct Cyc_Absyn_Exp*_tmp807=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp807,_tag_fat(_tmp214,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1274
ptr->r=_tmp809;});
# 1276
ans=0;}else{
# 1280
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp80D=({void*_tmp80C=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp80C,({struct Cyc_Absyn_Exp*_tmp215[3U];({
struct Cyc_Absyn_Exp*_tmp80A=Cyc_Absyn_new_exp(ptr->r,0U);_tmp215[0]=_tmp80A;}),_tmp215[1]=bd,_tmp215[2]=ind;({struct Cyc_Absyn_Exp*_tmp80B=fn_e;Cyc_Toc_fncall_exp_dl(_tmp80B,_tag_fat(_tmp215,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1281
ptr->r=_tmp80D;});
# 1283
ans=1;}}
# 1286
goto _LL5;}}}else{_LL12: _LL13:
({void*_tmp216=0U;({struct _fat_ptr _tmp80E=({const char*_tmp217="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp217,sizeof(char),34U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp80E,_tag_fat(_tmp216,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1289
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{_LL3: _LL4:
({void*_tmp218=0U;({struct _fat_ptr _tmp80F=({const char*_tmp219="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp219,sizeof(char),31U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp80F,_tag_fat(_tmp218,sizeof(void*),0U));});});}_LL0:;}}
# 1295
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp21A=0U;({struct _fat_ptr _tmp810=({const char*_tmp21B="Missing type in primop ";_tag_fat(_tmp21B,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp810,_tag_fat(_tmp21A,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1299
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->f1=Cyc_Toc_mt_tq,({void*_tmp811=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp21C->f2=_tmp811;});_tmp21C;});}
# 1304
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp21D=e->r;void*_stmttmp1C=_tmp21D;void*_tmp21E=_stmttmp1C;int _tmp220;struct Cyc_Absyn_Exp*_tmp21F;int _tmp222;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_List_List*_tmp223;switch(*((int*)_tmp21E)){case 26U: _LL1: _tmp223=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp21E)->f1;_LL2: {struct Cyc_List_List*dles=_tmp223;
# 1307
{struct Cyc_List_List*_tmp224=dles;struct Cyc_List_List*dles2=_tmp224;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
({void*_tmp225=0U;({struct _fat_ptr _tmp812=({const char*_tmp226="array designators for abstract-field initialization";_tag_fat(_tmp226,sizeof(char),52U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_unimp)(_tmp812,_tag_fat(_tmp225,sizeof(void*),0U));});});}}
_tmp221=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles),0U);_tmp222=0;goto _LL4;}case 27U: _LL3: _tmp221=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21E)->f2;_tmp222=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21E)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp221;int zt=_tmp222;
_tmp21F=bd;_tmp220=zt;goto _LL6;}case 28U: _LL5: _tmp21F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp21E)->f1;_tmp220=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp21E)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp21F;int zt=_tmp220;
# 1313
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp813=bd;Cyc_Absyn_add_exp(_tmp813,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1322
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1330
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp227=e->r;void*_stmttmp1D=_tmp227;void*_tmp228=_stmttmp1D;struct Cyc_List_List*_tmp229;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->tag == 29U){_LL1: _tmp229=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL2: {struct Cyc_List_List*dles2=_tmp229;
dles=dles2;goto _LL0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1336
{void*_tmp22A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp1E=_tmp22A;void*_tmp22B=_stmttmp1E;struct Cyc_Absyn_Aggrdecl*_tmp22C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp22C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp22C;
# 1338
if(ad->impl == 0)
return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1346
 return 0;}_LL5:;}
# 1348
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp22D=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp22D;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp22E=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp1F=_tmp22E;struct _tuple19*_tmp22F=_stmttmp1F;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_List_List*_tmp230;_LLB: _tmp230=_tmp22F->f1;_tmp231=_tmp22F->f2;_LLC: {struct Cyc_List_List*ds=_tmp230;struct Cyc_Absyn_Exp*e2=_tmp231;
struct _fat_ptr*_tmp232=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp232;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp233=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp233;
if(nested_ans != 0)
return nested_ans;{
void*_tmp234=Cyc_Tcutil_compress(last_type_field->type);void*_stmttmp20=_tmp234;void*_tmp235=_stmttmp20;struct Cyc_Absyn_Exp*_tmp237;void*_tmp236;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp235)->tag == 4U){_LLE: _tmp236=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp235)->f1).elt_type;_tmp237=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp235)->f1).num_elts;_LLF: {void*elt_type=_tmp236;struct Cyc_Absyn_Exp*type_bd=_tmp237;
# 1363
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1369
return({struct Cyc_Absyn_Exp*_tmp816=({struct Cyc_Absyn_Exp*_tmp238[2U];_tmp238[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1371
struct Cyc_Absyn_Exp*_tmp814=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp238[1]=_tmp814;});({struct Cyc_Absyn_Exp*_tmp815=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp815,_tag_fat(_tmp238,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1369
Cyc_Absyn_add_exp(_tmp816,
# 1372
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{_LL10: _LL11:
 return 0;}_LLD:;}}}}
# 1377
({void*_tmp239=0U;({struct _fat_ptr _tmp817=({const char*_tmp23A="get_varsizeexp: did not find last struct field";_tag_fat(_tmp23A,sizeof(char),47U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp817,_tag_fat(_tmp239,sizeof(void*),0U));});});}}
# 1380
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp23B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp23B;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp23C=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp23C;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1387
({void*_tmp23D=0U;({struct _fat_ptr _tmp819=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp240=({struct Cyc_String_pa_PrintArg_struct _tmp757;_tmp757.tag=0U,_tmp757.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp757;});void*_tmp23E[1U];_tmp23E[0]=& _tmp240;({struct _fat_ptr _tmp818=({const char*_tmp23F="get_member_offset %s failed";_tag_fat(_tmp23F,sizeof(char),28U);});Cyc_aprintf(_tmp818,_tag_fat(_tmp23E,sizeof(void*),1U));});});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp819,_tag_fat(_tmp23D,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1391
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1394
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1397
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->tag=5U,_tmp241->f1=e1,_tmp241->f2=incr;_tmp241;}),0U);}
# 1401
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1410
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1412
void*_tmp242=e1->r;void*_stmttmp21=_tmp242;void*_tmp243=_stmttmp21;int _tmp247;int _tmp246;struct _fat_ptr*_tmp245;struct Cyc_Absyn_Exp*_tmp244;struct Cyc_Absyn_Exp*_tmp249;void*_tmp248;struct Cyc_Absyn_Stmt*_tmp24A;switch(*((int*)_tmp243)){case 37U: _LL1: _tmp24A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp24A;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14U: _LL3: _tmp248=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_tmp249=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp243)->f2;_LL4: {void*t=_tmp248;struct Cyc_Absyn_Exp*e=_tmp249;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21U: _LL5: _tmp244=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_tmp245=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f2;_tmp246=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f3;_tmp247=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f4;_LL6: {struct Cyc_Absyn_Exp*e=_tmp244;struct _fat_ptr*fld=_tmp245;int is_tagged=_tmp246;int is_read=_tmp247;
# 1417
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp81C=e1;struct Cyc_List_List*_tmp81B=({struct Cyc_List_List*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B->hd=fld,_tmp24B->tl=fs;_tmp24B;});struct Cyc_Absyn_Exp*(*_tmp81A)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp81C,_tmp81B,_tmp81A,f_env);});
goto _LL0;}default: _LL7: _LL8: {
# 1425
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1427
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp81D=(f(e1_copy,f_env))->r;e1->r=_tmp81D;});
goto _LL0;}}_LL0:;}
# 1433
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1435
void*_tmp24C=s->r;void*_stmttmp22=_tmp24C;void*_tmp24D=_stmttmp22;struct Cyc_Absyn_Stmt*_tmp24E;struct Cyc_Absyn_Stmt*_tmp250;struct Cyc_Absyn_Decl*_tmp24F;struct Cyc_Absyn_Exp*_tmp251;switch(*((int*)_tmp24D)){case 1U: _LL1: _tmp251=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp24D)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp251;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12U: _LL3: _tmp24F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp24D)->f1;_tmp250=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp24D)->f2;_LL4: {struct Cyc_Absyn_Decl*d=_tmp24F;struct Cyc_Absyn_Stmt*s2=_tmp250;
# 1438
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2U: _LL5: _tmp24E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp24D)->f2;_LL6: {struct Cyc_Absyn_Stmt*s2=_tmp24E;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp254=({struct Cyc_String_pa_PrintArg_struct _tmp758;_tmp758.tag=0U,({struct _fat_ptr _tmp81E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp758.f1=_tmp81E;});_tmp758;});void*_tmp252[1U];_tmp252[0]=& _tmp254;({struct _fat_ptr _tmp81F=({const char*_tmp253="lvalue_assign_stmt: %s";_tag_fat(_tmp253,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp81F,_tag_fat(_tmp252,sizeof(void*),1U));});});}_LL0:;}
# 1444
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1448
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp255=e->r;void*_stmttmp23=_tmp255;void*_tmp256=_stmttmp23;struct Cyc_Absyn_Stmt*_tmp257;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Exp**_tmp25A;void**_tmp259;switch(*((int*)_tmp256)){case 14U: _LL1: _tmp259=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp25A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL2: {void**t=_tmp259;struct Cyc_Absyn_Exp**e1=_tmp25A;
# 1451
({struct Cyc_Absyn_Exp*_tmp820=Cyc_Toc_push_address_exp(*e1);*e1=_tmp820;});
({void*_tmp821=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp821;});
return e;}case 20U: _LL3: _tmp258=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp258;
# 1455
return e1;}case 37U: _LL5: _tmp257=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL6: {struct Cyc_Absyn_Stmt*s=_tmp257;
# 1459
Cyc_Toc_push_address_stmt(s);
return e;}default: _LL7: _LL8:
# 1462
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp759;_tmp759.tag=0U,({struct _fat_ptr _tmp822=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp759.f1=_tmp822;});_tmp759;});void*_tmp25B[1U];_tmp25B[0]=& _tmp25D;({struct _fat_ptr _tmp823=({const char*_tmp25C="can't take & of exp %s";_tag_fat(_tmp25C,sizeof(char),23U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp823,_tag_fat(_tmp25B,sizeof(void*),1U));});});}_LL0:;}
# 1466
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp25E=s->r;void*_stmttmp24=_tmp25E;void*_tmp25F=_stmttmp24;struct Cyc_Absyn_Exp**_tmp260;struct Cyc_Absyn_Stmt*_tmp261;struct Cyc_Absyn_Stmt*_tmp262;switch(*((int*)_tmp25F)){case 2U: _LL1: _tmp262=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp25F)->f2;_LL2: {struct Cyc_Absyn_Stmt*s2=_tmp262;
_tmp261=s2;goto _LL4;}case 12U: _LL3: _tmp261=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25F)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp261;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1U: _LL5: _tmp260=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp25F)->f1;_LL6: {struct Cyc_Absyn_Exp**e=_tmp260;
({struct Cyc_Absyn_Exp*_tmp824=Cyc_Toc_push_address_exp(*e);*e=_tmp824;});goto _LL0;}default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp265=({struct Cyc_String_pa_PrintArg_struct _tmp75A;_tmp75A.tag=0U,({struct _fat_ptr _tmp825=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp75A.f1=_tmp825;});_tmp75A;});void*_tmp263[1U];_tmp263[0]=& _tmp265;({struct _fat_ptr _tmp826=({const char*_tmp264="can't take & of stmt %s";_tag_fat(_tmp264,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp826,_tag_fat(_tmp263,sizeof(void*),1U));});});}_LL0:;}
# 1478
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1490
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp266=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp266;
void*_tmp267=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp267;
void*_tmp268=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp268;
struct Cyc_Core_Opt*_tmp269=({struct Cyc_Core_Opt*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->v=c_ptr_type;_tmp29B;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp269;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp26A=e1->r;void*_stmttmp25=_tmp26A;void*_tmp26B=_stmttmp25;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;switch(*((int*)_tmp26B)){case 20U: _LL1: _tmp26E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26B)->f1;_LL2: {struct Cyc_Absyn_Exp*ea=_tmp26E;
# 1498
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1503
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23U: _LL3: _tmp26C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26B)->f1;_tmp26D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26B)->f2;_LL4: {struct Cyc_Absyn_Exp*ea=_tmp26C;struct Cyc_Absyn_Exp*eb=_tmp26D;
# 1505
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1510
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp26F[3U];_tmp26F[0]=ea,({
struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp26F[1]=_tmp827;}),_tmp26F[2]=eb;({struct Cyc_Absyn_Exp*_tmp828=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp828,_tag_fat(_tmp26F,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;}default: _LL5: _LL6:
({void*_tmp270=0U;({struct _fat_ptr _tmp829=({const char*_tmp271="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp271,sizeof(char),53U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp829,_tag_fat(_tmp270,sizeof(void*),0U));});});}_LL0:;}{
# 1516
struct _tuple1*_tmp272=Cyc_Toc_temp_var();struct _tuple1*x=_tmp272;
struct _RegionHandle _tmp273=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp273;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp274=({struct Cyc_Absyn_Vardecl*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->sc=Cyc_Absyn_Public,_tmp29A->name=x,_tmp29A->varloc=0U,_tmp29A->tq=Cyc_Toc_mt_tq,_tmp29A->type=c_fat_ptr_type,_tmp29A->initializer=xinit,_tmp29A->rgn=0,_tmp29A->attributes=0,_tmp29A->escapes=0;_tmp29A;});struct Cyc_Absyn_Vardecl*x_vd=_tmp274;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp275=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->tag=4U,_tmp299->f1=x_vd;_tmp299;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp275;
struct Cyc_Absyn_Exp*_tmp276=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp276;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp277=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp277;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp278=Cyc_Toc_temp_var();struct _tuple1*y=_tmp278;
struct Cyc_Absyn_Vardecl*_tmp279=({struct Cyc_Absyn_Vardecl*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->sc=Cyc_Absyn_Public,_tmp298->name=y,_tmp298->varloc=0U,_tmp298->tq=Cyc_Toc_mt_tq,_tmp298->type=c_elt_type,_tmp298->initializer=deref_x,_tmp298->rgn=0,_tmp298->attributes=0,_tmp298->escapes=0;_tmp298;});struct Cyc_Absyn_Vardecl*y_vd=_tmp279;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp27A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->tag=4U,_tmp297->f1=y_vd;_tmp297;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp27A;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp27B=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp27B;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp82B=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp82A=y_exp;Cyc_Absyn_prim2_exp(_tmp82B,_tmp82A,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1537
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp27C=Cyc_Toc_temp_var();struct _tuple1*z=_tmp27C;
struct Cyc_Absyn_Vardecl*_tmp27D=({struct Cyc_Absyn_Vardecl*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->sc=Cyc_Absyn_Public,_tmp296->name=z,_tmp296->varloc=0U,_tmp296->tq=Cyc_Toc_mt_tq,_tmp296->type=c_elt_type,_tmp296->initializer=z_init,_tmp296->rgn=0,_tmp296->attributes=0,_tmp296->escapes=0;_tmp296;});struct Cyc_Absyn_Vardecl*z_vd=_tmp27D;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp27E=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->tag=4U,_tmp295->f1=z_vd;_tmp295;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp27E;
# 1543
struct Cyc_Absyn_Exp*_tmp27F=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp27F;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp280=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp280;
struct Cyc_Absyn_Exp*_tmp281=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp281;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1550
struct Cyc_Absyn_Exp*_tmp282=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp282;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp283=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp283;
struct Cyc_Absyn_Exp*_tmp284=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp284;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1557
struct Cyc_List_List*_tmp285=({struct Cyc_Absyn_Exp*_tmp294[2U];({struct Cyc_Absyn_Exp*_tmp82D=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp294[0]=_tmp82D;}),({
struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp294[1]=_tmp82C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp294,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 1557
struct Cyc_List_List*xsizeargs=_tmp285;
# 1559
struct Cyc_Absyn_Exp*_tmp286=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp286;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp82E,oneexp,0U);});{
# 1564
struct Cyc_Absyn_Exp*_tmp287=({struct Cyc_Absyn_Exp*_tmp82F=xsize;Cyc_Absyn_and_exp(_tmp82F,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp287;
struct Cyc_Absyn_Stmt*_tmp288=Cyc_Absyn_exp_stmt(({void*_tmp293=0U;({struct Cyc_Absyn_Exp*_tmp830=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp830,_tag_fat(_tmp293,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp288;
struct Cyc_Absyn_Exp*_tmp289=({struct Cyc_Absyn_Exp*_tmp831=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp831,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp289;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp28A;
struct Cyc_Absyn_Exp*_tmp28B=({struct Cyc_Absyn_Exp*_tmp832=deref_xcurr;Cyc_Absyn_assign_exp(_tmp832,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp28B;
struct Cyc_Absyn_Stmt*_tmp28C=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp28C;
s=({struct Cyc_Absyn_Stmt*_tmp835=({struct Cyc_Absyn_Exp*_tmp834=comp_exp;struct Cyc_Absyn_Stmt*_tmp833=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp834,_tmp833,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp835,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp837=({struct Cyc_Absyn_Decl*_tmp28E=_cycalloc(sizeof(*_tmp28E));({void*_tmp836=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D->tag=0U,_tmp28D->f1=z_vd;_tmp28D;});_tmp28E->r=_tmp836;}),_tmp28E->loc=0U;_tmp28E;});Cyc_Absyn_decl_stmt(_tmp837,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp839=({struct Cyc_Absyn_Decl*_tmp290=_cycalloc(sizeof(*_tmp290));({void*_tmp838=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->tag=0U,_tmp28F->f1=y_vd;_tmp28F;});_tmp290->r=_tmp838;}),_tmp290->loc=0U;_tmp290;});Cyc_Absyn_decl_stmt(_tmp839,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp83B=({struct Cyc_Absyn_Decl*_tmp292=_cycalloc(sizeof(*_tmp292));({void*_tmp83A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->tag=0U,_tmp291->f1=x_vd;_tmp291;});_tmp292->r=_tmp83A;}),_tmp292->loc=0U;_tmp292;});Cyc_Absyn_decl_stmt(_tmp83B,s,0U);});
({void*_tmp83C=Cyc_Toc_stmt_exp_r(s);e->r=_tmp83C;});}}}}}}}}}}
# 1518
;_pop_region(rgn2);}}
# 1590 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1594
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp29C=Cyc_Tcutil_compress(aggrtype);void*_stmttmp26=_tmp29C;void*_tmp29D=_stmttmp26;union Cyc_Absyn_AggrInfo _tmp29E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29D)->f1)->tag == 20U){_LL1: _tmp29E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29D)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp29E;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp29F=0U;({struct _fat_ptr _tmp83F=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A2=({struct Cyc_String_pa_PrintArg_struct _tmp75B;_tmp75B.tag=0U,({
struct _fat_ptr _tmp83D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp75B.f1=_tmp83D;});_tmp75B;});void*_tmp2A0[1U];_tmp2A0[0]=& _tmp2A2;({struct _fat_ptr _tmp83E=({const char*_tmp2A1="expecting union but found %s in check_tagged_union";_tag_fat(_tmp2A1,sizeof(char),51U);});Cyc_aprintf(_tmp83E,_tag_fat(_tmp2A0,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp83F,_tag_fat(_tmp29F,sizeof(void*),0U));});});}_LL0:;}{
# 1600
struct _tuple29 _tmp2A3=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp27=_tmp2A3;struct _tuple29 _tmp2A4=_stmttmp27;struct Cyc_Absyn_Exp*_tmp2A6;struct _tuple1*_tmp2A5;_LL6: _tmp2A5=_tmp2A4.f1;_tmp2A6=_tmp2A4.f2;_LL7: {struct _tuple1*temp=_tmp2A5;struct Cyc_Absyn_Exp*temp_exp=_tmp2A6;
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2A7;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2A8=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2A8;
struct Cyc_Absyn_Exp*_tmp2A9=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2A9;
struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2AA;
struct Cyc_Absyn_Stmt*_tmp2AB=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2AB;
struct Cyc_Absyn_Stmt*_tmp2AC=({struct Cyc_Absyn_Exp*_tmp841=test_exp;struct Cyc_Absyn_Stmt*_tmp840=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp841,_tmp840,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2AC;
void*_tmp2AD=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2AD;
struct Cyc_Absyn_Exp*_tmp2AE=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2AE;
struct Cyc_Absyn_Stmt*_tmp2AF=({struct _tuple1*_tmp844=temp;void*_tmp843=e1_p_type;struct Cyc_Absyn_Exp*_tmp842=e1_f;Cyc_Absyn_declare_stmt(_tmp844,_tmp843,_tmp842,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2AF;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1613
struct Cyc_Absyn_Exp*_tmp2B0=({struct Cyc_Absyn_Exp*_tmp845=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp845,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2B0;
struct Cyc_Absyn_Exp*_tmp2B1=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2B1;
struct Cyc_Absyn_Exp*_tmp2B2=({struct Cyc_Absyn_Exp*_tmp846=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp846,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2B2;
struct Cyc_Absyn_Stmt*_tmp2B3=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2B3;
struct Cyc_Absyn_Stmt*_tmp2B4=({struct Cyc_Absyn_Exp*_tmp848=test_exp;struct Cyc_Absyn_Stmt*_tmp847=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp848,_tmp847,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2B4;
struct Cyc_Absyn_Stmt*_tmp2B5=({struct _tuple1*_tmp84B=temp;void*_tmp84A=e1_c_type;struct Cyc_Absyn_Exp*_tmp849=e1;Cyc_Absyn_declare_stmt(_tmp84B,_tmp84A,_tmp849,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2B5;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1623
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1626
void*_tmp2B6=Cyc_Tcutil_compress(t);void*_stmttmp28=_tmp2B6;void*_tmp2B7=_stmttmp28;union Cyc_Absyn_AggrInfo _tmp2B8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f1)->tag == 20U){_LL1: _tmp2B8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2B8;
# 1628
struct Cyc_Absyn_Aggrdecl*_tmp2B9=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2B9;
({int _tmp84C=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp84C;});{
# 1631
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp75D;_tmp75D.tag=0U,_tmp75D.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp75D;});struct Cyc_String_pa_PrintArg_struct _tmp2BE=({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0U,_tmp75C.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp75C;});void*_tmp2BB[2U];_tmp2BB[0]=& _tmp2BD,_tmp2BB[1]=& _tmp2BE;({struct _fat_ptr _tmp84D=({const char*_tmp2BC="_union_%s_%s";_tag_fat(_tmp2BC,sizeof(char),13U);});Cyc_aprintf(_tmp84D,_tag_fat(_tmp2BB,sizeof(void*),2U));});});
({void*_tmp84E=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));*_tmp2BA=str;_tmp2BA;}));*tagged_member_type=_tmp84E;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1642
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1645
void*_tmp2BF=e->r;void*_stmttmp29=_tmp2BF;void*_tmp2C0=_stmttmp29;int*_tmp2C3;struct _fat_ptr*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;int*_tmp2C6;struct _fat_ptr*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C7;switch(*((int*)_tmp2C0)){case 14U: _LL1: _tmp2C7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C0)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp2C7;
({void*_tmp2C8=0U;({struct _fat_ptr _tmp84F=({const char*_tmp2C9="cast on lhs!";_tag_fat(_tmp2C9,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp84F,_tag_fat(_tmp2C8,sizeof(void*),0U));});});}case 21U: _LL3: _tmp2C4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C0)->f1;_tmp2C5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C0)->f2;_tmp2C6=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C0)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2C4;struct _fat_ptr*f=_tmp2C5;int*is_read=_tmp2C6;
# 1648
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22U: _LL5: _tmp2C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C0)->f1;_tmp2C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C0)->f2;_tmp2C3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C0)->f4;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp2C1;struct _fat_ptr*f=_tmp2C2;int*is_read=_tmp2C3;
# 1651
void*_tmp2CA=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp2A=_tmp2CA;void*_tmp2CB=_stmttmp2A;void*_tmp2CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB)->tag == 3U){_LLA: _tmp2CC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB)->f1).elt_type;_LLB: {void*et=_tmp2CC;
# 1653
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{_LLC: _LLD:
# 1655
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1657
 return 0;}_LL0:;}
# 1670 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1674
struct _tuple29 _tmp2CD=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp2B=_tmp2CD;struct _tuple29 _tmp2CE=_stmttmp2B;struct Cyc_Absyn_Exp*_tmp2D0;struct _tuple1*_tmp2CF;_LL1: _tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;_LL2: {struct _tuple1*temp=_tmp2CF;struct Cyc_Absyn_Exp*temp_exp=_tmp2D0;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1683
struct Cyc_Absyn_Exp*_tmp2D1=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2D1;
s2=({struct Cyc_Absyn_Exp*_tmp851=test_exp;struct Cyc_Absyn_Stmt*_tmp850=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp851,_tmp850,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1686
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp854=temp;void*_tmp853=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp852=
Cyc_Toc_push_address_exp(e1);
# 1686
Cyc_Absyn_declare_stmt(_tmp854,_tmp853,_tmp852,
# 1688
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1692
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2D4=({struct Cyc_String_pa_PrintArg_struct _tmp75E;_tmp75E.tag=0U,({struct _fat_ptr _tmp855=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp75E.f1=_tmp855;});_tmp75E;});void*_tmp2D2[1U];_tmp2D2[0]=& _tmp2D4;({unsigned _tmp857=e->loc;struct _fat_ptr _tmp856=({const char*_tmp2D3="exp_to_c: no type for %s";_tag_fat(_tmp2D3,sizeof(char),25U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp857,_tmp856,_tag_fat(_tmp2D2,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1697
int did_inserted_checks=0;
{void*_tmp2D5=e->annot;void*_stmttmp2C=_tmp2D5;void*_tmp2D6=_stmttmp2C;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D6)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2D6)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1703
{void*_tmp2D7=e->r;void*_stmttmp2D=_tmp2D7;void*_tmp2D8=_stmttmp2D;struct Cyc_Absyn_Stmt*_tmp2D9;struct _fat_ptr*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;int _tmp2E3;int _tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;void**_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;int _tmp2DE;struct Cyc_Absyn_Datatypefield*_tmp2E6;struct Cyc_Absyn_Datatypedecl*_tmp2E5;struct Cyc_List_List*_tmp2E4;struct Cyc_Absyn_Aggrdecl*_tmp2EA;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List*_tmp2E8;struct _tuple1**_tmp2E7;struct Cyc_List_List*_tmp2EC;void*_tmp2EB;int _tmp2EF;void*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2ED;int _tmp2F3;struct Cyc_Absyn_Exp*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2F0;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F8;int _tmp2FC;int _tmp2FB;struct _fat_ptr*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2F9;int _tmp300;int _tmp2FF;struct _fat_ptr*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FD;struct Cyc_List_List*_tmp302;void*_tmp301;void*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp306;struct Cyc_Absyn_Exp*_tmp305;struct Cyc_Absyn_Exp*_tmp307;enum Cyc_Absyn_Coercion _tmp30B;int _tmp30A;struct Cyc_Absyn_Exp*_tmp309;void**_tmp308;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Exp*_tmp30C;struct Cyc_Absyn_Exp*_tmp30E;int _tmp310;struct Cyc_Absyn_Exp*_tmp30F;struct Cyc_Absyn_VarargInfo*_tmp315;struct Cyc_List_List*_tmp314;int _tmp313;struct Cyc_List_List*_tmp312;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_List_List*_tmp317;struct Cyc_Absyn_Exp*_tmp316;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Exp*_tmp31D;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp320;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_Absyn_Exp*_tmp31E;struct Cyc_Absyn_Exp*_tmp323;struct Cyc_Core_Opt*_tmp322;struct Cyc_Absyn_Exp*_tmp321;enum Cyc_Absyn_Incrementor _tmp325;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_List_List*_tmp327;enum Cyc_Absyn_Primop _tmp326;struct Cyc_Absyn_Exp*_tmp328;switch(*((int*)_tmp2D8)){case 2U: _LL8: _LL9:
# 1705
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1712
struct Cyc_Absyn_Exp*_tmp329=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp329;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp858=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp858;});else{
# 1717
({void*_tmp85A=({struct Cyc_Absyn_Exp*_tmp32A[3U];_tmp32A[0]=zero,_tmp32A[1]=zero,_tmp32A[2]=zero;({struct Cyc_Absyn_Exp*_tmp859=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp859,_tag_fat(_tmp32A,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp85A;});}}else{
# 1719
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1721
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp328=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL11: {struct Cyc_Absyn_Exp*e1=_tmp328;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3U: _LL12: _tmp326=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp327=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL13: {enum Cyc_Absyn_Primop p=_tmp326;struct Cyc_List_List*es=_tmp327;
# 1727
struct Cyc_List_List*_tmp32B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp32B;
# 1729
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp32C=p;switch(_tmp32C){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1732
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp32D=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_stmttmp2E=_tmp32D;void*_tmp32E=_stmttmp2E;void*_tmp332;void*_tmp331;void*_tmp330;void*_tmp32F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32E)->tag == 3U){_LL76: _tmp32F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32E)->f1).elt_type;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32E)->f1).ptr_atts).nullable;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32E)->f1).ptr_atts).bounds;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32E)->f1).ptr_atts).zero_term;_LL77: {void*elt_type=_tmp32F;void*nbl=_tmp330;void*bound=_tmp331;void*zt=_tmp332;
# 1735
struct Cyc_Absyn_Exp*_tmp333=({void*_tmp85B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp85B,bound);});struct Cyc_Absyn_Exp*eopt=_tmp333;
if(eopt == 0)
# 1738
({void*_tmp85E=({struct Cyc_Absyn_Exp*_tmp334[2U];_tmp334[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp85C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp334[1]=_tmp85C;});({struct Cyc_Absyn_Exp*_tmp85D=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp85D,_tag_fat(_tmp334,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1738
e->r=_tmp85E;});else{
# 1741
struct Cyc_Absyn_Exp*_tmp335=eopt;struct Cyc_Absyn_Exp*e2=_tmp335;
# 1743
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1747
({void*_tmp860=({struct Cyc_Absyn_Exp*_tmp336[2U];_tmp336[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp336[1]=e2;({struct Cyc_Absyn_Exp*_tmp85F=function_e;Cyc_Toc_fncall_exp_r(_tmp85F,_tag_fat(_tmp336,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp860;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(e2))
({void*_tmp337=0U;({unsigned _tmp862=e->loc;struct _fat_ptr _tmp861=({const char*_tmp338="can't calculate numelts";_tag_fat(_tmp338,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp862,_tmp861,_tag_fat(_tmp337,sizeof(void*),0U));});});
# 1752
({void*_tmp865=({struct Cyc_Absyn_Exp*_tmp864=arg;struct Cyc_Absyn_Exp*_tmp863=e2;Cyc_Toc_conditional_exp_r(_tmp864,_tmp863,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp865;});}else{
# 1754
e->r=e2->r;goto _LL75;}}}
# 1757
goto _LL75;}}else{_LL78: _LL79:
({struct Cyc_String_pa_PrintArg_struct _tmp33B=({struct Cyc_String_pa_PrintArg_struct _tmp75F;_tmp75F.tag=0U,({
struct _fat_ptr _tmp866=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp75F.f1=_tmp866;});_tmp75F;});void*_tmp339[1U];_tmp339[0]=& _tmp33B;({struct _fat_ptr _tmp867=({const char*_tmp33A="numelts primop applied to non-pointer %s";_tag_fat(_tmp33A,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp867,_tag_fat(_tmp339,sizeof(void*),1U));});});}_LL75:;}
# 1761
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1766
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp33C=0U;({struct _fat_ptr _tmp868=({const char*_tmp33D="can't do pointer arithmetic at top-level";_tag_fat(_tmp33D,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_unimp)(_tmp868,_tag_fat(_tmp33C,sizeof(void*),0U));});});
{void*_tmp33E=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp2F=_tmp33E;void*_tmp33F=_stmttmp2F;void*_tmp342;void*_tmp341;void*_tmp340;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33F)->tag == 3U){_LL7B: _tmp340=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33F)->f1).elt_type;_tmp341=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33F)->f1).ptr_atts).bounds;_tmp342=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33F)->f1).ptr_atts).zero_term;_LL7C: {void*elt_type=_tmp340;void*b=_tmp341;void*zt=_tmp342;
# 1770
struct Cyc_Absyn_Exp*_tmp343=({void*_tmp869=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp869,b);});struct Cyc_Absyn_Exp*eopt=_tmp343;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp86C=({struct Cyc_Absyn_Exp*_tmp344[3U];_tmp344[0]=e1,({
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp344[1]=_tmp86A;}),_tmp344[2]=e2;({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp86B,_tag_fat(_tmp344,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1774
e->r=_tmp86C;});else{
# 1776
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp86E=({struct Cyc_Absyn_Exp*_tmp345[3U];_tmp345[0]=e1,_tmp345[1]=eopt,_tmp345[2]=e2;({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp86D,_tag_fat(_tmp345,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp86E;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1781
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1786
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd)){
({void*_tmp870=({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp86F,Cyc_Toc_curr_sp);});e1->r=_tmp870;});
({void*_tmp872=({struct Cyc_Absyn_Exp*_tmp871=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp871,Cyc_Toc_curr_sp);});e2->r=_tmp872;});
({void*_tmp874=({void*_tmp873=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp873;});e1->topt=_tmp874;});
({void*_tmp876=({struct Cyc_Absyn_Exp*_tmp875=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp875,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1794
e->r=_tmp876;});}else{
# 1797
({void*_tmp87A=({struct Cyc_Absyn_Exp*_tmp346[3U];_tmp346[0]=e1,({
struct Cyc_Absyn_Exp*_tmp878=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp346[1]=_tmp878;}),({
struct Cyc_Absyn_Exp*_tmp877=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp346[2]=_tmp877;});({struct Cyc_Absyn_Exp*_tmp879=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp879,_tag_fat(_tmp346,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1797
e->r=_tmp87A;});}}
# 1801
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A: goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C: goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E: goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70: goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1804
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(old_types))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp347=({void*_tmp87B=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp87B,Cyc_Toc_mt_tq);});void*t=_tmp347;
({void*_tmp87E=({void*_tmp87D=t;Cyc_Toc_cast_it_r(_tmp87D,({struct Cyc_Absyn_Exp*_tmp87C=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp87C,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp87E;});
e1->topt=t;}
# 1814
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp348=({void*_tmp87F=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp87F,Cyc_Toc_mt_tq);});void*t=_tmp348;
({void*_tmp882=({void*_tmp881=t;Cyc_Toc_cast_it_r(_tmp881,({struct Cyc_Absyn_Exp*_tmp880=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp880,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp882;});
e2->topt=t;}
# 1819
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1822
goto _LL7;}case 5U: _LL14: _tmp324=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp325=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL15: {struct Cyc_Absyn_Exp*e2=_tmp324;enum Cyc_Absyn_Incrementor incr=_tmp325;
# 1824
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1833 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp35C="increment";_tag_fat(_tmp35C,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp349="decrement";_tag_fat(_tmp349,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp34C=({struct Cyc_String_pa_PrintArg_struct _tmp760;_tmp760.tag=0U,_tmp760.f1=(struct _fat_ptr)((struct _fat_ptr)incr_str);_tmp760;});void*_tmp34A[1U];_tmp34A[0]=& _tmp34C;({unsigned _tmp884=e->loc;struct _fat_ptr _tmp883=({const char*_tmp34B="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_fat(_tmp34B,sizeof(char),74U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp884,_tmp883,_tag_fat(_tmp34A,sizeof(void*),1U));});});
# 1841
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp34D=incr;switch(_tmp34D){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_String_pa_PrintArg_struct _tmp350=({struct Cyc_String_pa_PrintArg_struct _tmp761;_tmp761.tag=0U,_tmp761.f1=(struct _fat_ptr)((struct _fat_ptr)incr_str);_tmp761;});void*_tmp34E[1U];_tmp34E[0]=& _tmp350;({unsigned _tmp886=e->loc;struct _fat_ptr _tmp885=({const char*_tmp34F="in-place post%s is not supported on @tagged union members";_tag_fat(_tmp34F,sizeof(char),58U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp886,_tmp885,_tag_fat(_tmp34E,sizeof(void*),1U));});});}_LL7F:;}
# 1851
({void*_tmp888=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->tag=4U,_tmp352->f1=e2,({struct Cyc_Core_Opt*_tmp887=({struct Cyc_Core_Opt*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->v=(void*)op;_tmp351;});_tmp352->f2=_tmp887;}),_tmp352->f3=one;_tmp352;});e->r=_tmp888;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1855
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1860
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)incr == (int)1U ||(int)incr == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1866
if((int)incr == (int)2U ||(int)incr == (int)3U)
change=-1;
({void*_tmp88D=({struct Cyc_Absyn_Exp*_tmp353[3U];({struct Cyc_Absyn_Exp*_tmp88B=Cyc_Toc_push_address_exp(e2);_tmp353[0]=_tmp88B;}),({
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp353[1]=_tmp88A;}),({
struct Cyc_Absyn_Exp*_tmp889=Cyc_Absyn_signed_int_exp(change,0U);_tmp353[2]=_tmp889;});({struct Cyc_Absyn_Exp*_tmp88C=fn_e;Cyc_Toc_fncall_exp_r(_tmp88C,_tag_fat(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1868
e->r=_tmp88D;});}else{
# 1871
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1877
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp354=Cyc_Toc_temp_var();struct _tuple1*x=_tmp354;
void*_tmp355=({void*_tmp88E=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp88E,Cyc_Toc_mt_tq);});void*t=_tmp355;
struct Cyc_Absyn_Exp*_tmp356=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp356;
struct Cyc_Absyn_Exp*_tmp357=({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp88F,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1882
struct Cyc_Absyn_Exp*testexp=_tmp357;
# 1884
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp891=({struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp890,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1885
Cyc_Absyn_and_exp(_tmp891,testexp,0U);});{
# 1887
struct Cyc_Absyn_Stmt*_tmp358=({struct Cyc_Absyn_Exp*_tmp894=testexp;struct Cyc_Absyn_Stmt*_tmp893=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1887
Cyc_Absyn_ifthenelse_stmt(_tmp894,_tmp893,
# 1889
Cyc_Absyn_exp_stmt(({void*_tmp359=0U;({struct Cyc_Absyn_Exp*_tmp892=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp892,_tag_fat(_tmp359,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 1887
struct Cyc_Absyn_Stmt*s=_tmp358;
# 1890
s=({struct Cyc_Absyn_Stmt*_tmp895=s;Cyc_Absyn_seq_stmt(_tmp895,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp896=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp896;});}}else{
# 1894
struct Cyc_Toc_functionSet*_tmp35A=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp35A;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp89A=({struct Cyc_Absyn_Exp*_tmp35B[2U];({struct Cyc_Absyn_Exp*_tmp898=Cyc_Toc_push_address_exp(e2);_tmp35B[0]=_tmp898;}),({struct Cyc_Absyn_Exp*_tmp897=Cyc_Absyn_signed_int_exp(1,0U);_tmp35B[1]=_tmp897;});({struct Cyc_Absyn_Exp*_tmp899=fn_e;Cyc_Toc_fncall_exp_r(_tmp899,_tag_fat(_tmp35B,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp89A;});}}else{
# 1898
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1902
goto _LL7;}}case 4U: _LL16: _tmp321=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp322=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp323=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL17: {struct Cyc_Absyn_Exp*e1=_tmp321;struct Cyc_Core_Opt*popt=_tmp322;struct Cyc_Absyn_Exp*e2=_tmp323;
# 1921 "toc.cyc"
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp89B=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp89B;});
# 1932
goto _LL7;}{
# 1934
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
# 1940
return;}{
# 1944
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1950
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp35D=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp30=_tmp35D;enum Cyc_Absyn_Primop _tmp35E=_stmttmp30;switch(_tmp35E){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=e2;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL86;default: _LL8B: _LL8C:
({void*_tmp35F=0U;({struct _fat_ptr _tmp89C=({const char*_tmp360="bad t ? pointer arithmetic";_tag_fat(_tmp360,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp89C,_tag_fat(_tmp35F,sizeof(void*),0U));});});}_LL86:;}
# 1960
done=1;{
# 1962
struct Cyc_Absyn_Exp*_tmp361=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp361;
({void*_tmp8A0=({struct Cyc_Absyn_Exp*_tmp362[3U];({struct Cyc_Absyn_Exp*_tmp89E=Cyc_Toc_push_address_exp(e1);_tmp362[0]=_tmp89E;}),({
struct Cyc_Absyn_Exp*_tmp89D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp362[1]=_tmp89D;}),_tmp362[2]=change;({struct Cyc_Absyn_Exp*_tmp89F=fn_e;Cyc_Toc_fncall_exp_r(_tmp89F,_tag_fat(_tmp362,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1963
e->r=_tmp8A0;});}}else{
# 1966
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1969
enum Cyc_Absyn_Primop _tmp363=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp31=_tmp363;enum Cyc_Absyn_Primop _tmp364=_stmttmp31;if(_tmp364 == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 1971
 done=1;
({void*_tmp8A2=({struct Cyc_Absyn_Exp*_tmp365[2U];_tmp365[0]=e1,_tmp365[1]=e2;({struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8A1,_tag_fat(_tmp365,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8A2;});
goto _LL8D;}else{_LL90: _LL91:
({void*_tmp366=0U;({struct _fat_ptr _tmp8A3=({const char*_tmp367="bad zero-terminated pointer arithmetic";_tag_fat(_tmp367,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp8A3,_tag_fat(_tmp366,sizeof(void*),0U));});});}_LL8D:;}}}
# 1978
if(!done){
# 1980
if(e1_poly)
({void*_tmp8A5=({void*_tmp8A4=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp8A4,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp8A5;});
# 1986
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp8A6=e1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8A6,0,Cyc_Toc_assignop_lvalue,({struct _tuple31*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->f1=popt,_tmp368->f2=e2;_tmp368;}));});
e->r=e1->r;}}
# 1991
goto _LL7;}}}}case 6U: _LL18: _tmp31E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp31F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp320=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp31E;struct Cyc_Absyn_Exp*e2=_tmp31F;struct Cyc_Absyn_Exp*e3=_tmp320;
# 1993
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7U: _LL1A: _tmp31C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp31D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp31C;struct Cyc_Absyn_Exp*e2=_tmp31D;
_tmp31A=e1;_tmp31B=e2;goto _LL1D;}case 8U: _LL1C: _tmp31A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp31B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL1D: {struct Cyc_Absyn_Exp*e1=_tmp31A;struct Cyc_Absyn_Exp*e2=_tmp31B;
_tmp318=e1;_tmp319=e2;goto _LL1F;}case 9U: _LL1E: _tmp318=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp319=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp318;struct Cyc_Absyn_Exp*e2=_tmp319;
# 2000
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3 == 0){_LL20: _tmp316=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp317=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp316;struct Cyc_List_List*es=_tmp317;
# 2004
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2009
({void*_tmp8AA=({void*_tmp8A9=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8A9,({struct Cyc_Absyn_Exp*_tmp369[1U];({struct Cyc_Absyn_Exp*_tmp8A7=
Cyc_Absyn_copy_exp(e1);_tmp369[0]=_tmp8A7;});({struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp8A8,_tag_fat(_tmp369,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2009
e1->r=_tmp8AA;});
# 2011
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_LL22: _tmp311=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp312=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp313=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3)->num_varargs;_tmp314=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3)->injectors;_tmp315=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3)->vai;_LL23: {struct Cyc_Absyn_Exp*e1=_tmp311;struct Cyc_List_List*es=_tmp312;int num_varargs=_tmp313;struct Cyc_List_List*injectors=_tmp314;struct Cyc_Absyn_VarargInfo*vai=_tmp315;
# 2022 "toc.cyc"
struct _RegionHandle _tmp36A=_new_region("r");struct _RegionHandle*r=& _tmp36A;_push_region(r);{
struct _tuple29 _tmp36B=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp32=_tmp36B;struct _tuple29 _tmp36C=_stmttmp32;struct Cyc_Absyn_Exp*_tmp36E;struct _tuple1*_tmp36D;_LL93: _tmp36D=_tmp36C.f1;_tmp36E=_tmp36C.f2;_LL94: {struct _tuple1*argv=_tmp36D;struct Cyc_Absyn_Exp*argvexp=_tmp36E;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2029
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
int num_normargs=num_args - num_varargs;
# 2033
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp36F->tl=new_args;_tmp36F;});}}
# 2038
new_args=({struct Cyc_List_List*_tmp371=_cycalloc(sizeof(*_tmp371));({struct Cyc_Absyn_Exp*_tmp8AD=({struct Cyc_Absyn_Exp*_tmp370[3U];_tmp370[0]=argvexp,({
# 2040
struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp370[1]=_tmp8AB;}),_tmp370[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp8AC,_tag_fat(_tmp370,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2038
_tmp371->hd=_tmp8AD;}),_tmp371->tl=new_args;_tmp371;});
# 2043
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2045
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2050
({void*_tmp8B1=({void*_tmp8B0=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8B0,({struct Cyc_Absyn_Exp*_tmp372[1U];({struct Cyc_Absyn_Exp*_tmp8AE=
Cyc_Absyn_copy_exp(e1);_tmp372[0]=_tmp8AE;});({struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp8AF,_tag_fat(_tmp372,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2050
e1->r=_tmp8B1;});{
# 2052
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2055
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp373=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp33=_tmp373;void*_tmp374=_stmttmp33;struct Cyc_Absyn_Datatypedecl*_tmp375;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->f1)->f1).KnownDatatype).tag == 2){_LL96: _tmp375=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->f1)->f1).KnownDatatype).val;_LL97: {struct Cyc_Absyn_Datatypedecl*x=_tmp375;
tud=x;goto _LL95;}}else{goto _LL98;}}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp376=0U;({struct _fat_ptr _tmp8B2=({const char*_tmp377="toc: unknown datatype in vararg with inject";_tag_fat(_tmp377,sizeof(char),44U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp8B2,_tag_fat(_tmp376,sizeof(void*),0U));});});}_LL95:;}{
# 2061
struct _fat_ptr vs=({unsigned _tmp383=(unsigned)num_varargs;struct _tuple1**_tmp382=({struct _RegionHandle*_tmp8B3=r;_region_malloc(_tmp8B3,_check_times(_tmp383,sizeof(struct _tuple1*)));});({{unsigned _tmp762=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp762;++ i){({struct _tuple1*_tmp8B4=Cyc_Toc_temp_var();_tmp382[i]=_tmp8B4;});}}0;});_tag_fat(_tmp382,sizeof(struct _tuple1*),_tmp383);});
# 2063
if(num_varargs != 0){
# 2067
struct Cyc_List_List*_tmp378=0;struct Cyc_List_List*array_args=_tmp378;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp379=_cycalloc(sizeof(*_tmp379));({struct Cyc_Absyn_Exp*_tmp8B5=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp379->hd=_tmp8B5;}),_tmp379->tl=array_args;_tmp379;});}}
s=({struct _tuple1*_tmp8B8=argv;void*_tmp8B7=arr_type;struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp8B8,_tmp8B7,_tmp8B6,s,0U);});
# 2072
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)- 1;
for(0;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp37A=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp34=_tmp37A;struct Cyc_Absyn_Datatypefield*_tmp37B=_stmttmp34;struct Cyc_List_List*_tmp37D;struct _tuple1*_tmp37C;_LL9B: _tmp37C=_tmp37B->name;_tmp37D=_tmp37B->typs;_LL9C: {struct _tuple1*qv=_tmp37C;struct Cyc_List_List*tqts=_tmp37D;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2086
struct _tuple1*_tmp37E=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp37E;
struct Cyc_List_List*_tmp37F=({struct _tuple19*_tmp381[2U];({struct _tuple19*_tmp8BC=({struct _fat_ptr*_tmp8BB=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp8BB,Cyc_Toc_datatype_tag(tud,qv));});_tmp381[0]=_tmp8BC;}),({
struct _tuple19*_tmp8BA=({struct _fat_ptr*_tmp8B9=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8B9,arg);});_tmp381[1]=_tmp8BA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp381,sizeof(struct _tuple19*),2U));});
# 2087
struct Cyc_List_List*dles=_tmp37F;
# 2089
s=({struct _tuple1*_tmp8BF=var;void*_tmp8BE=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp8BD=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->tag=29U,_tmp380->f1=tdn,_tmp380->f2=0,_tmp380->f3=dles,_tmp380->f4=0;_tmp380;}),0U);
# 2089
Cyc_Absyn_declare_stmt(_tmp8BF,_tmp8BE,_tmp8BD,s,0U);});}}}}}else{
# 2097
s=({struct _tuple1*_tmp8C2=argv;void*_tmp8C1=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp8C0=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8C2,_tmp8C1,_tmp8C0,s,0U);});}}}else{
# 2102
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp384->tl=array_args;_tmp384;});}{
# 2108
struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp385;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2111
s=({struct _tuple1*_tmp8C5=argv;void*_tmp8C4=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp8C3=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8C5,_tmp8C4,_tmp8C3,s,0U);});}}
# 2114
({void*_tmp8C6=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8C6;});}}}}
# 2116
_npop_handler(0U);goto _LL7;
# 2022
;_pop_region(r);}}case 11U: _LL24: _tmp30F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp310=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL25: {struct Cyc_Absyn_Exp*e1=_tmp30F;int b=_tmp310;
# 2119
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp8C9=({void*_tmp8C8=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp8C8,({struct Cyc_Absyn_Exp*_tmp386[1U];_tmp386[0]=e1;({struct Cyc_Absyn_Exp*_tmp8C7=fn_e;Cyc_Toc_fncall_exp_dl(_tmp8C7,_tag_fat(_tmp386,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp8C9;});
goto _LL7;}}case 12U: _LL26: _tmp30E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp30E;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13U: _LL28: _tmp30C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp30D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL29: {struct Cyc_Absyn_Exp*e1=_tmp30C;struct Cyc_List_List*ts=_tmp30D;
# 2125
Cyc_Toc_exp_to_c(nv,e1);
# 2134 "toc.cyc"
for(0;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp387=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp387;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp388=Cyc_Tcutil_compress((void*)ts->hd);void*_stmttmp35=_tmp388;void*_tmp389=_stmttmp35;switch(*((int*)_tmp389)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp389)->f1)->tag == 18U){_LLA0: _LLA1:
 continue;}else{goto _LLA2;}default: _LLA2: _LLA3:
# 2141
({void*_tmp8CB=({void*_tmp8CA=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8CA,e1,0U);})->r;e->r=_tmp8CB;});
goto _LL9D;}_LL9D:;}
# 2144
break;}}
# 2147
goto _LL7;}case 14U: _LL2A: _tmp308=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp309=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp30A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp30B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL2B: {void**t=_tmp308;struct Cyc_Absyn_Exp*e1=_tmp309;int user_inserted=_tmp30A;enum Cyc_Absyn_Coercion coercion=_tmp30B;
# 2149
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2156
{struct _tuple32 _tmp38A=({struct _tuple32 _tmp764;({void*_tmp8CD=Cyc_Tcutil_compress(old_t2);_tmp764.f1=_tmp8CD;}),({void*_tmp8CC=Cyc_Tcutil_compress(new_typ);_tmp764.f2=_tmp8CC;});_tmp764;});struct _tuple32 _stmttmp36=_tmp38A;struct _tuple32 _tmp38B=_stmttmp36;struct Cyc_Absyn_PtrInfo _tmp38C;struct Cyc_Absyn_PtrInfo _tmp38E;struct Cyc_Absyn_PtrInfo _tmp38D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->tag == 3U)switch(*((int*)_tmp38B.f2)){case 3U: _LLA5: _tmp38D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp38E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LLA6: {struct Cyc_Absyn_PtrInfo p1=_tmp38D;struct Cyc_Absyn_PtrInfo p2=_tmp38E;
# 2158
struct Cyc_Absyn_Exp*_tmp38F=({void*_tmp8CE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8CE,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp38F;
struct Cyc_Absyn_Exp*_tmp390=({void*_tmp8CF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8CF,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp390;
int _tmp391=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp391;
int _tmp392=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp392;
{struct _tuple33 _tmp393=({struct _tuple33 _tmp763;_tmp763.f1=b1,_tmp763.f2=b2;_tmp763;});struct _tuple33 _stmttmp37=_tmp393;struct _tuple33 _tmp394=_stmttmp37;if(_tmp394.f1 != 0){if(_tmp394.f2 != 0){_LLAC: _LLAD:
# 2165
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8D2=({void*_tmp8D1=*t;Cyc_Toc_cast_it_r(_tmp8D1,({struct Cyc_Absyn_Exp*_tmp395[1U];_tmp395[0]=e1;({struct Cyc_Absyn_Exp*_tmp8D0=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp8D0,_tag_fat(_tmp395,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp8D2;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2174
goto _LLAB;}else{_LLAE: _LLAF: {
# 2176
struct Cyc_Absyn_Exp*_tmp396=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp396;
struct _tuple13 _tmp397=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp38=_tmp397;struct _tuple13 _tmp398=_stmttmp38;int _tmp39A;unsigned _tmp399;_LLB5: _tmp399=_tmp398.f1;_tmp39A=_tmp398.f2;_LLB6: {unsigned i=_tmp399;int valid=_tmp39A;
if(Cyc_Toc_is_toplevel(nv)){
# 2182
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp8D3=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8D3,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp8D4=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp8D4;});}else{
# 2187
if(zt1){
# 2192
struct _tuple29 _tmp39B=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp39=_tmp39B;struct _tuple29 _tmp39C=_stmttmp39;struct Cyc_Absyn_Exp*_tmp39E;struct _tuple1*_tmp39D;_LLB8: _tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;_LLB9: {struct _tuple1*x=_tmp39D;struct Cyc_Absyn_Exp*x_exp=_tmp39E;
struct Cyc_Absyn_Exp*arg3;
# 2196
{void*_tmp39F=e1->r;void*_stmttmp3A=_tmp39F;void*_tmp3A0=_stmttmp3A;struct Cyc_Absyn_Vardecl*_tmp3A1;struct Cyc_Absyn_Vardecl*_tmp3A2;switch(*((int*)_tmp3A0)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1).Wstring_c).tag){case 8U: _LLBB: _LLBC:
 arg3=e2;goto _LLBA;case 9U: _LLBD: _LLBE:
 arg3=e2;goto _LLBA;default: goto _LLC3;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1)){case 1U: _LLBF: _tmp3A2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1)->f1;_LLC0: {struct Cyc_Absyn_Vardecl*vd=_tmp3A2;
_tmp3A1=vd;goto _LLC2;}case 4U: _LLC1: _tmp3A1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A0)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_tmp3A1;
# 2201
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC4;
arg3=e2;
goto _LLBA;}default: goto _LLC3;}default: _LLC3: _LLC4:
# 2208
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2211
arg3=({struct Cyc_Absyn_Exp*_tmp3A3[2U];({
struct Cyc_Absyn_Exp*_tmp8D6=({void*_tmp8D5=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp8D5,x_exp);});_tmp3A3[0]=_tmp8D6;}),_tmp3A3[1]=e2;({struct Cyc_Absyn_Exp*_tmp8D7=
# 2211
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp8D7,_tag_fat(_tmp3A3,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2213
goto _LLBA;}_LLBA:;}
# 2215
if(!zt2 && !(p2.elt_tq).real_const)
# 2218
arg3=({struct Cyc_Absyn_Exp*_tmp8D8=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8D8,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2220
struct Cyc_Absyn_Exp*_tmp3A4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3A4;
struct Cyc_Absyn_Exp*_tmp3A5=({struct Cyc_Absyn_Exp*_tmp3A7[3U];_tmp3A7[0]=x_exp,_tmp3A7[1]=arg2,_tmp3A7[2]=arg3;({struct Cyc_Absyn_Exp*_tmp8D9=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp8D9,_tag_fat(_tmp3A7,sizeof(struct Cyc_Absyn_Exp*),3U));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3A5;
struct Cyc_Absyn_Stmt*_tmp3A6=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3A6;
s=({struct _tuple1*_tmp8DC=x;void*_tmp8DB=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp8DA=e1;Cyc_Absyn_declare_stmt(_tmp8DC,_tmp8DB,_tmp8DA,s,0U);});
({void*_tmp8DD=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8DD;});}}}else{
# 2227
({void*_tmp8E0=({struct Cyc_Absyn_Exp*_tmp3A8[3U];_tmp3A8[0]=e1,({
# 2229
struct Cyc_Absyn_Exp*_tmp8DE=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3A8[1]=_tmp8DE;}),_tmp3A8[2]=e2;({struct Cyc_Absyn_Exp*_tmp8DF=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8DF,_tag_fat(_tmp3A8,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2227
e->r=_tmp8E0;});}}
# 2232
goto _LLAB;}}}}else{if(_tmp394.f2 != 0){_LLB0: _LLB1: {
# 2242 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3A9=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3A9;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp8E1=b2;Cyc_Absyn_add_exp(_tmp8E1,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2249
struct Cyc_Absyn_Exp*_tmp3AA=({struct Cyc_Absyn_Exp*_tmp3AC[3U];_tmp3AC[0]=e1,({
# 2251
struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3AC[1]=_tmp8E2;}),_tmp3AC[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp8E3,_tag_fat(_tmp3AC,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2249
struct Cyc_Absyn_Exp*ptr_exp=_tmp3AA;
# 2253
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8E6=({struct Cyc_Absyn_Exp*_tmp3AB[1U];({struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Absyn_copy_exp(ptr_exp);_tmp3AB[0]=_tmp8E4;});({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8E5,_tag_fat(_tmp3AB,sizeof(struct Cyc_Absyn_Exp*),1U));});});ptr_exp->r=_tmp8E6;});
({void*_tmp8E7=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp8E7;});
goto _LLAB;}}}else{_LLB2: _LLB3:
# 2261
 if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3AD=0U;({struct _fat_ptr _tmp8E8=({const char*_tmp3AE="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3AE,sizeof(char),70U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_unimp)(_tmp8E8,_tag_fat(_tmp3AD,sizeof(void*),0U));});});
({void*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp3AF[3U];_tmp3AF[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3AF[1]=_tmp8EA;}),({
struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Absyn_uint_exp(1U,0U);_tmp3AF[2]=_tmp8E9;});({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8EB,_tag_fat(_tmp3AF,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2264
e->r=_tmp8EC;});}
# 2268
goto _LLAB;}}_LLAB:;}
# 2270
goto _LLA4;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38B.f2)->f1)->tag == 1U){_LLA7: _tmp38C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1;_LLA8: {struct Cyc_Absyn_PtrInfo p1=_tmp38C;
# 2272
{struct Cyc_Absyn_Exp*_tmp3B0=({void*_tmp8ED=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8ED,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3B=_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1=_stmttmp3B;if(_tmp3B1 == 0){_LLC6: _LLC7:
# 2274
({void*_tmp8EF=({struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp8EE,Cyc_Toc_curr_sp);});e1->r=_tmp8EF;});
e1->topt=new_typ_c;
goto _LLC5;}else{_LLC8: _LLC9:
 goto _LLC5;}_LLC5:;}
# 2279
goto _LLA4;}}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9: _LLAA:
# 2281
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLA4;}_LLA4:;}
# 2285
goto _LL7;}case 15U: _LL2C: _tmp307=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL2D: {struct Cyc_Absyn_Exp*e1=_tmp307;
# 2288
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2294
({void*_tmp8F1=({void*_tmp8F0=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8F0,e1);});e->r=_tmp8F1;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2298
({void*_tmp8F3=({void*_tmp8F2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8F2,e1);});e->r=_tmp8F3;});}
# 2300
goto _LL7;}case 16U: _LL2E: _tmp305=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp306=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL2F: {struct Cyc_Absyn_Exp*rgnopt=_tmp305;struct Cyc_Absyn_Exp*e1=_tmp306;
# 2307
Cyc_Toc_exp_to_c(nv,e1);{
# 2309
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple29 _tmp3B2=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp3C=_tmp3B2;struct _tuple29 _tmp3B3=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp3B5;struct _tuple1*_tmp3B4;_LLCB: _tmp3B4=_tmp3B3.f1;_tmp3B5=_tmp3B3.f2;_LLCC: {struct _tuple1*x=_tmp3B4;struct Cyc_Absyn_Exp*xexp=_tmp3B5;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3B6=Cyc_Tcutil_compress(elt_typ);void*_stmttmp3D=_tmp3B6;void*_tmp3B7=_stmttmp3D;void*_tmp3B8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B7)->tag == 4U){_LLCE: _tmp3B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B7)->f1).elt_type;_LLCF: {void*t2=_tmp3B8;
# 2314
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCD;}}else{_LLD0: _LLD1:
# 2318
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCD;}_LLCD:;}{
# 2321
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3B9=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3B9;
struct Cyc_Absyn_Exp*_tmp3BA=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3BA;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3BB[2U];_tmp3BB[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3BB[1]=_tmp8F4;});({struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8F5,_tag_fat(_tmp3BB,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2328
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2331
struct Cyc_Absyn_Exp*_tmp3BC=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3BC;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2335
if(rgnopt == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2338
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2341
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3BD[3U];({
struct Cyc_Absyn_Exp*_tmp8F8=Cyc_Absyn_copy_exp(xexp);_tmp3BD[0]=_tmp8F8;}),({
struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3BD[1]=_tmp8F7;}),
(unsigned)lenexp?_tmp3BD[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_uint_exp(1U,0U);_tmp3BD[2]=_tmp8F6;});({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp8F9,_tag_fat(_tmp3BD,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2348
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp8FA=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->tag=Cyc_Toc_Dest,_tmp3BE->f1=xexp;_tmp3BE;});e->annot=_tmp8FA;});{
struct Cyc_Absyn_Stmt*_tmp3BF=({struct _tuple1*_tmp8FE=x;void*_tmp8FD=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8FC=mexp;Cyc_Absyn_declare_stmt(_tmp8FE,_tmp8FD,_tmp8FC,({
struct Cyc_Absyn_Stmt*_tmp8FB=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp8FB,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2350
struct Cyc_Absyn_Stmt*s=_tmp3BF;
# 2354
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp8FF=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8FF;});
if(vse != 0)
({void*_tmp901=({void*_tmp900=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp900,Cyc_Absyn_copy_exp(e));});e->r=_tmp901;});
goto _LL7;}}}}}}}case 18U: _LL30: _tmp304=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL31: {struct Cyc_Absyn_Exp*e1=_tmp304;
# 2362
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17U: _LL32: _tmp303=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL33: {void*t=_tmp303;
({void*_tmp903=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->tag=17U,({void*_tmp902=Cyc_Toc_typ_to_c(t);_tmp3C0->f1=_tmp902;});_tmp3C0;});e->r=_tmp903;});goto _LL7;}case 19U: _LL34: _tmp301=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp302=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL35: {void*t=_tmp301;struct Cyc_List_List*fs=_tmp302;
# 2365
void*_tmp3C1=t;void*t2=_tmp3C1;
struct Cyc_List_List*_tmp3C2=fs;struct Cyc_List_List*l=_tmp3C2;
for(0;l != 0;l=l->tl){
void*_tmp3C3=(void*)l->hd;void*_stmttmp3E=_tmp3C3;void*_tmp3C4=_stmttmp3E;unsigned _tmp3C5;struct _fat_ptr*_tmp3C6;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C4)->tag == 0U){_LLD3: _tmp3C6=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C4)->f1;_LLD4: {struct _fat_ptr*n=_tmp3C6;
goto _LLD2;}}else{_LLD5: _tmp3C5=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3C4)->f1;_LLD6: {unsigned n=_tmp3C5;
# 2371
{void*_tmp3C7=Cyc_Tcutil_compress(t2);void*_stmttmp3F=_tmp3C7;void*_tmp3C8=_stmttmp3F;struct Cyc_List_List*_tmp3C9;struct Cyc_List_List*_tmp3CA;struct Cyc_Absyn_Datatypefield*_tmp3CB;union Cyc_Absyn_AggrInfo _tmp3CC;switch(*((int*)_tmp3C8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)){case 20U: _LLD8: _tmp3CC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)->f1;_LLD9: {union Cyc_Absyn_AggrInfo info=_tmp3CC;
# 2373
struct Cyc_Absyn_Aggrdecl*_tmp3CD=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp3CD;
if(ad->impl == 0)
({void*_tmp3CE=0U;({struct _fat_ptr _tmp904=({const char*_tmp3CF="struct fields must be known";_tag_fat(_tmp3CF,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp904,_tag_fat(_tmp3CE,sizeof(void*),0U));});});
_tmp3CA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)->f1).KnownDatatypefield).tag == 2){_LLDE: _tmp3CB=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C8)->f1)->f1).KnownDatatypefield).val).f2;_LLDF: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3CB;
# 2387
if(n == (unsigned)0)
({void*_tmp905=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->tag=0U,_tmp3D3->f1=Cyc_Toc_tag_sp;_tmp3D3;}));l->hd=_tmp905;});else{
# 2390
t2=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp907=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->tag=0U,({struct _fat_ptr*_tmp906=Cyc_Absyn_fieldname((int)n);_tmp3D4->f1=_tmp906;});_tmp3D4;}));l->hd=_tmp907;});}
# 2393
goto _LLD7;}}else{goto _LLE0;}default: goto _LLE0;}case 7U: _LLDA: _tmp3CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C8)->f2;_LLDB: {struct Cyc_List_List*fields=_tmp3CA;
# 2378
struct Cyc_Absyn_Aggrfield*_tmp3D0=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp3D0;
({void*_tmp908=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->tag=0U,_tmp3D1->f1=nth_field->name;_tmp3D1;}));l->hd=_tmp908;});
t2=nth_field->type;
goto _LLD7;}case 6U: _LLDC: _tmp3C9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C8)->f1;_LLDD: {struct Cyc_List_List*ts=_tmp3C9;
# 2383
({void*_tmp90A=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->tag=0U,({struct _fat_ptr*_tmp909=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp3D2->f1=_tmp909;});_tmp3D2;}));l->hd=_tmp90A;});
t2=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD7;}default: _LLE0: _LLE1:
# 2394
({void*_tmp3D5=0U;({struct _fat_ptr _tmp90B=({const char*_tmp3D6="impossible type for offsetof tuple index";_tag_fat(_tmp3D6,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp90B,_tag_fat(_tmp3D5,sizeof(void*),0U));});});}_LLD7:;}
# 2396
goto _LLD2;}}_LLD2:;}
# 2399
({void*_tmp90D=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->tag=19U,({void*_tmp90C=Cyc_Toc_typ_to_c(t);_tmp3D7->f1=_tmp90C;}),_tmp3D7->f2=fs;_tmp3D7;});e->r=_tmp90D;});
goto _LL7;}case 21U: _LL36: _tmp2FD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2FE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp2FF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp300=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL37: {struct Cyc_Absyn_Exp*e1=_tmp2FD;struct _fat_ptr*f=_tmp2FE;int is_tagged=_tmp2FF;int is_read=_tmp300;
# 2402
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp912=({struct Cyc_Absyn_Exp*_tmp911=e1;void*_tmp910=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp90F=e1_cyc_type;struct _fat_ptr*_tmp90E=f;Cyc_Toc_check_tagged_union(_tmp911,_tmp910,_tmp90F,_tmp90E,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2406
e->r=_tmp912;});
# 2408
if(is_poly)
({void*_tmp914=({void*_tmp913=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp913,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp914;});
goto _LL7;}case 22U: _LL38: _tmp2F9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2FA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp2FB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp2FC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL39: {struct Cyc_Absyn_Exp*e1=_tmp2F9;struct _fat_ptr*f=_tmp2FA;int is_tagged=_tmp2FB;int is_read=_tmp2FC;
# 2412
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp3D8=Cyc_Tcutil_compress(e1typ);void*_stmttmp40=_tmp3D8;void*_tmp3D9=_stmttmp40;struct Cyc_Absyn_PtrInfo _tmp3DA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9)->tag == 3U){_LLE3: _tmp3DA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9)->f1;_LLE4: {struct Cyc_Absyn_PtrInfo p=_tmp3DA;
ta=p.elt_type;goto _LLE2;}}else{_LLE5: _LLE6:
({void*_tmp3DB=0U;({struct _fat_ptr _tmp915=({const char*_tmp3DC="get_ptr_typ: not a pointer!";_tag_fat(_tmp3DC,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp915,_tag_fat(_tmp3DB,sizeof(void*),0U));});});}_LLE2:;}
# 2419
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp91A=({struct Cyc_Absyn_Exp*_tmp919=e1;void*_tmp918=Cyc_Toc_typ_to_c(e1typ);void*_tmp917=ta;struct _fat_ptr*_tmp916=f;Cyc_Toc_check_tagged_union(_tmp919,_tmp918,_tmp917,_tmp916,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp91A;});
if(is_poly && is_read)
({void*_tmp91C=({void*_tmp91B=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp91B,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp91C;});
goto _LL7;}case 20U: _LL3A: _tmp2F8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL3B: {struct Cyc_Absyn_Exp*e1=_tmp2F8;
# 2427
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23U: _LL3C: _tmp2F6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2F7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL3D: {struct Cyc_Absyn_Exp*e1=_tmp2F6;struct Cyc_Absyn_Exp*e2=_tmp2F7;
# 2431
{void*_tmp3DD=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp41=_tmp3DD;void*_tmp3DE=_stmttmp41;struct Cyc_List_List*_tmp3DF;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3DE)->tag == 6U){_LLE8: _tmp3DF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3DE)->f1;_LLE9: {struct Cyc_List_List*ts=_tmp3DF;
# 2433
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp3E0=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp3E0;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _tmp3E1=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp42=_tmp3E1;struct _tuple13 _tmp3E2=_stmttmp42;int _tmp3E4;unsigned _tmp3E3;_LLED: _tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;_LLEE: {unsigned i=_tmp3E3;int known=_tmp3E4;
if(!known)
({void*_tmp3E5=0U;({struct _fat_ptr _tmp91D=({const char*_tmp3E6="unknown tuple subscript in translation to C";_tag_fat(_tmp3E6,sizeof(char),44U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp91D,_tag_fat(_tmp3E5,sizeof(void*),0U));});});
({void*_tmp91F=({struct Cyc_Absyn_Exp*_tmp91E=e1;Cyc_Toc_aggrmember_exp_r(_tmp91E,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp91F;});
goto _LLE7;}}}}}else{_LLEA: _LLEB:
# 2443
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp920=Cyc_Toc_deref_exp_r(e1);e->r=_tmp920;});
goto _LLE7;}}_LLE7:;}
# 2449
goto _LL7;}case 24U: _LL3E: _tmp2F5=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL3F: {struct Cyc_List_List*es=_tmp2F5;
# 2451
if(!Cyc_Toc_is_toplevel(nv)){
# 2453
void*_tmp3E7=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp3E7;
{void*_tmp3E8=strct_typ;union Cyc_Absyn_AggrInfo _tmp3E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f2 == 0){_LLF0: _tmp3E9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f1)->f1;_LLF1: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp3E9;
# 2456
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({struct _tuple19*_tmp922=({struct _fat_ptr*_tmp921=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp921,(struct Cyc_Absyn_Exp*)es->hd);});_tmp3EA->hd=_tmp922;}),_tmp3EA->tl=dles;_tmp3EA;});}}
# 2462
({void*_tmp924=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->tag=29U,_tmp3EB->f1=sd->name,_tmp3EB->f2=0,({struct Cyc_List_List*_tmp923=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3EB->f3=_tmp923;}),_tmp3EB->f4=sd;_tmp3EB;});e->r=_tmp924;});
e->topt=strct_typ;
goto _LLEF;}}else{goto _LLF2;}}else{goto _LLF2;}}else{_LLF2: _LLF3:
({void*_tmp3EC=0U;({struct _fat_ptr _tmp925=({const char*_tmp3ED="tuple type not an aggregate";_tag_fat(_tmp3ED,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp925,_tag_fat(_tmp3EC,sizeof(void*),0U));});});}_LLEF:;}
# 2467
goto _LL7;}else{
# 2471
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({struct _tuple19*_tmp926=({struct _tuple19*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->f1=0,_tmp3EE->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp3EE;});_tmp3EF->hd=_tmp926;}),_tmp3EF->tl=dles;_tmp3EF;});}
# 2476
({void*_tmp927=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp927;});}
# 2478
goto _LL7;}case 26U: _LL40: _tmp2F4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL41: {struct Cyc_List_List*dles0=_tmp2F4;
# 2484
{struct Cyc_List_List*_tmp3F0=dles0;struct Cyc_List_List*dles=_tmp3F0;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp928=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp928;});
goto _LL7;}case 27U: _LL42: _tmp2F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp2F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp2F3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL43: {struct Cyc_Absyn_Vardecl*vd=_tmp2F0;struct Cyc_Absyn_Exp*e1=_tmp2F1;struct Cyc_Absyn_Exp*e2=_tmp2F2;int iszeroterm=_tmp2F3;
# 2492
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3F1=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _stmttmp43=_tmp3F1;struct _tuple13 _tmp3F2=_stmttmp43;int _tmp3F4;unsigned _tmp3F3;_LLF5: _tmp3F3=_tmp3F2.f1;_tmp3F4=_tmp3F2.f2;_LLF6: {unsigned sz=_tmp3F3;int known=_tmp3F4;
void*_tmp3F5=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp3F5;
struct Cyc_List_List*es=0;
# 2499
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp3F6=0U;({unsigned _tmp92A=e1->loc;struct _fat_ptr _tmp929=({const char*_tmp3F7="cannot determine value of constant";_tag_fat(_tmp3F7,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp92A,_tmp929,_tag_fat(_tmp3F6,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({struct _tuple19*_tmp92B=({struct _tuple19*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->f1=0,_tmp3F8->f2=e2;_tmp3F8;});_tmp3F9->hd=_tmp92B;}),_tmp3F9->tl=es;_tmp3F9;});}}
# 2505
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp3FA=({void*_tmp92C=elt_typ;Cyc_Toc_cast_it(_tmp92C,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp3FA;
es=({struct Cyc_List_List*_tmp92E=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp92E,({struct Cyc_List_List*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));({struct _tuple19*_tmp92D=({struct _tuple19*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->f1=0,_tmp3FB->f2=ezero;_tmp3FB;});_tmp3FC->hd=_tmp92D;}),_tmp3FC->tl=0;_tmp3FC;}));});}}
# 2510
({void*_tmp92F=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp92F;});}}
# 2512
goto _LL7;}case 28U: _LL44: _tmp2ED=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2EE=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp2EF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL45: {struct Cyc_Absyn_Exp*e1=_tmp2ED;void*t1=_tmp2EE;int iszeroterm=_tmp2EF;
# 2519
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp930=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp930;});else{
# 2522
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30U: _LL46: _tmp2EB=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2EC=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL47: {void*st=_tmp2EB;struct Cyc_List_List*dles=_tmp2EC;
# 2526
{struct Cyc_List_List*_tmp3FD=dles;struct Cyc_List_List*dles2=_tmp3FD;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*_tmp3FE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp3FE;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3FF=Cyc_Tcutil_compress(strct_typ);void*_stmttmp44=_tmp3FF;void*_tmp400=_stmttmp44;union Cyc_Absyn_AggrInfo _tmp401;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp400)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp400)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp400)->f2 == 0){_LLF8: _tmp401=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp400)->f1)->f1;_LLF9: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp401;
# 2533
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp931=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->tag=29U,_tmp402->f1=sd->name,_tmp402->f2=0,_tmp402->f3=dles,_tmp402->f4=sd;_tmp402;});e->r=_tmp931;});
e->topt=strct_typ;
goto _LLF7;}}else{goto _LLFA;}}else{goto _LLFA;}}else{_LLFA: _LLFB:
({void*_tmp403=0U;({struct _fat_ptr _tmp932=({const char*_tmp404="anonStruct type not an aggregate";_tag_fat(_tmp404,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp932,_tag_fat(_tmp403,sizeof(void*),0U));});});}_LLF7:;}else{
# 2540
({void*_tmp933=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp933;});}
goto _LL7;}}case 29U: _LL48: _tmp2E7=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2E8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp2E9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp2EA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL49: {struct _tuple1**tdn=_tmp2E7;struct Cyc_List_List*exist_ts=_tmp2E8;struct Cyc_List_List*dles=_tmp2E9;struct Cyc_Absyn_Aggrdecl*sd=_tmp2EA;
# 2546
if(sd == 0 || sd->impl == 0)
({void*_tmp405=0U;({struct _fat_ptr _tmp934=({const char*_tmp406="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp406,sizeof(char),58U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp934,_tag_fat(_tmp405,sizeof(void*),0U));});});{
void*_tmp407=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp407;
{void*_tmp408=Cyc_Tcutil_compress(new_typ);void*_stmttmp45=_tmp408;void*_tmp409=_stmttmp45;union Cyc_Absyn_AggrInfo _tmp40A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->tag == 20U){_LLFD: _tmp40A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->f1;_LLFE: {union Cyc_Absyn_AggrInfo info=_tmp40A;
({struct _tuple1*_tmp935=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp935;});goto _LLFC;}}else{goto _LLFF;}}else{_LLFF: _LL100:
({void*_tmp40B=0U;({struct _fat_ptr _tmp936=({const char*_tmp40C="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp40C,sizeof(char),44U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp936,_tag_fat(_tmp40B,sizeof(void*),0U));});});}_LLFC:;}{
# 2557
struct Cyc_List_List*_tmp40D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp40D;
if(typ_fields == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp40E=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp40E;
struct Cyc_List_List*_tmp40F=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp40F;
# 2564
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp410=Cyc_Tcutil_compress(old_typ);void*_stmttmp46=_tmp410;void*_tmp411=_stmttmp46;struct Cyc_List_List*_tmp412;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp411)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp411)->f1)->tag == 20U){_LL102: _tmp412=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp411)->f2;_LL103: {struct Cyc_List_List*param_ts=_tmp412;
# 2568
{struct Cyc_List_List*_tmp413=fields;struct Cyc_List_List*fields2=_tmp413;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple34*_tmp414=(struct _tuple34*)fields2->hd;struct _tuple34*_stmttmp47=_tmp414;struct _tuple34*_tmp415=_stmttmp47;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Aggrfield*_tmp416;_LL107: _tmp416=_tmp415->f1;_tmp417=_tmp415->f2;_LL108: {struct Cyc_Absyn_Aggrfield*aggrfield=_tmp416;struct Cyc_Absyn_Exp*fieldexp=_tmp417;
void*_tmp418=fieldexp->topt;void*old_field_typ=_tmp418;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2573
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp938=({void*_tmp937=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp937,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2575
fieldexp->r=_tmp938;});
# 2578
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp419=aggrfield->name;struct _fat_ptr*n=_tmp419;
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp41A;
struct _tuple19*_tmp41B=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp41B;
struct _tuple19*_tmp41C=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple19*val_dle=_tmp41C;
struct _tuple1*s=({struct _tuple1*_tmp427=_cycalloc(sizeof(*_tmp427));({union Cyc_Absyn_Nmspace _tmp93C=Cyc_Absyn_Abs_n(0,1);_tmp427->f1=_tmp93C;}),({
struct _fat_ptr*_tmp93B=({struct _fat_ptr*_tmp426=_cycalloc(sizeof(*_tmp426));({struct _fat_ptr _tmp93A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp424=({struct Cyc_String_pa_PrintArg_struct _tmp766;_tmp766.tag=0U,_tmp766.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp766;});struct Cyc_String_pa_PrintArg_struct _tmp425=({struct Cyc_String_pa_PrintArg_struct _tmp765;_tmp765.tag=0U,_tmp765.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp765;});void*_tmp422[2U];_tmp422[0]=& _tmp424,_tmp422[1]=& _tmp425;({struct _fat_ptr _tmp939=({const char*_tmp423="_union_%s_%s";_tag_fat(_tmp423,sizeof(char),13U);});Cyc_aprintf(_tmp939,_tag_fat(_tmp422,sizeof(void*),2U));});});*_tmp426=_tmp93A;});_tmp426;});_tmp427->f2=_tmp93B;});_tmp427;});
# 2586
struct _tuple19*_tmp41D=({
struct _fat_ptr*_tmp93E=n;Cyc_Toc_make_field(_tmp93E,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->tag=29U,_tmp421->f1=s,_tmp421->f2=0,({struct Cyc_List_List*_tmp93D=({struct _tuple19*_tmp420[2U];_tmp420[0]=tag_dle,_tmp420[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp420,sizeof(struct _tuple19*),2U));});_tmp421->f3=_tmp93D;}),_tmp421->f4=0;_tmp421;}),0U));});
# 2586
struct _tuple19*u_dle=_tmp41D;
# 2589
({void*_tmp940=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->tag=29U,_tmp41F->f1=*tdn,_tmp41F->f2=0,({struct Cyc_List_List*_tmp93F=({struct _tuple19*_tmp41E[1U];_tmp41E[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp41E,sizeof(struct _tuple19*),1U));});_tmp41F->f3=_tmp93F;}),_tmp41F->f4=sd;_tmp41F;});e->r=_tmp940;});}
# 2595
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp428=({struct Cyc_List_List*_tmp941=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp941,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(sd->tvs,param_ts));});
# 2596
struct Cyc_List_List*inst=_tmp428;
# 2598
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp429=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp429;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp42A=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp42A;
void*_tmp42B=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp42B;
struct Cyc_Absyn_Aggrfield*_tmp42C=({struct Cyc_Absyn_Aggrfield*_tmp942=old_f;Cyc_Toc_aggrfield_to_c(_tmp942,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2602
struct Cyc_Absyn_Aggrfield*new_f=_tmp42C;
# 2604
new_fields=({struct Cyc_List_List*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->hd=new_f,_tmp42D->tl=new_fields;_tmp42D;});
# 2610
if(fs->tl == 0){
{void*_tmp42E=Cyc_Tcutil_compress(new_f->type);void*_stmttmp48=_tmp42E;void*_tmp42F=_stmttmp48;struct Cyc_Absyn_ArrayInfo _tmp430;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42F)->tag == 4U){_LL10A: _tmp430=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42F)->f1;_LL10B: {struct Cyc_Absyn_ArrayInfo ai=_tmp430;
# 2613
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp431=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp431;
({struct Cyc_Absyn_Exp*_tmp943=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp943;});
({void*_tmp944=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->tag=4U,_tmp432->f1=ai2;_tmp432;});new_f->type=_tmp944;});}
# 2618
goto _LL109;}}else{_LL10C: _LL10D:
# 2622
 if(fieldexp->topt == 0)
goto _LL109;
{void*_tmp433=Cyc_Tcutil_compress((void*)_check_null(fieldexp->topt));void*_stmttmp49=_tmp433;void*_tmp434=_stmttmp49;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f1)->tag == 20U){_LL10F: _LL110:
# 2626
 new_f->type=(void*)_check_null(fieldexp->topt);goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
 goto _LL10E;}_LL10E:;}
# 2629
goto _LL109;}_LL109:;}
# 2633
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp946=({struct Cyc_List_List*_tmp436=_cycalloc(sizeof(*_tmp436));({void*_tmp945=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->tag=6U,_tmp435->f1=0;_tmp435;});_tmp436->hd=_tmp945;}),_tmp436->tl=new_f->attributes;_tmp436;});new_f->attributes=_tmp946;});}}}
# 2639
sd=({struct _fat_ptr*_tmp949=({struct _fat_ptr*_tmp43A=_cycalloc(sizeof(*_tmp43A));({struct _fat_ptr _tmp948=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp439=({struct Cyc_Int_pa_PrintArg_struct _tmp767;_tmp767.tag=1U,_tmp767.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp767;});void*_tmp437[1U];_tmp437[0]=& _tmp439;({struct _fat_ptr _tmp947=({const char*_tmp438="_genStruct%d";_tag_fat(_tmp438,sizeof(char),13U);});Cyc_aprintf(_tmp947,_tag_fat(_tmp437,sizeof(void*),1U));});});*_tmp43A=_tmp948;});_tmp43A;});Cyc_Toc_make_c_struct_defn(_tmp949,
# 2641
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2645
({void*_tmp94A=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp43B=_cycalloc(sizeof(*_tmp43B));*_tmp43B=sd;_tmp43B;})),0);e->topt=_tmp94A;});}}}}
# 2648
goto _LL101;}}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp43C=0U;({struct _fat_ptr _tmp94B=({const char*_tmp43D="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp43D,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp94B,_tag_fat(_tmp43C,sizeof(void*),0U));});});}_LL101:;}else{
# 2657
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2659
struct _tuple34*_tmp43E=(struct _tuple34*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple34*_stmttmp4A=_tmp43E;struct _tuple34*_tmp43F=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Aggrfield*_tmp440;_LL114: _tmp440=_tmp43F->f1;_tmp441=_tmp43F->f2;_LL115: {struct Cyc_Absyn_Aggrfield*field=_tmp440;struct Cyc_Absyn_Exp*fieldexp=_tmp441;
void*_tmp442=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp442;
void*_tmp443=field->type;void*fieldtyp=_tmp443;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp94D=({
void*_tmp94C=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp94C,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2665
fieldexp->r=_tmp94D;});{
# 2668
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp444=({struct _tuple19*_tmp446[2U];({struct _tuple19*_tmp94F=({struct _tuple19*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->f1=0,_tmp447->f2=field_tag_exp;_tmp447;});_tmp446[0]=_tmp94F;}),({struct _tuple19*_tmp94E=({struct _tuple19*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->f1=0,_tmp448->f2=fieldexp;_tmp448;});_tmp446[1]=_tmp94E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp446,sizeof(struct _tuple19*),2U));});struct Cyc_List_List*newdles=_tmp444;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp951=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp445[1U];({struct _tuple19*_tmp950=Cyc_Toc_make_field(field->name,umem);_tmp445[0]=_tmp950;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp445,sizeof(struct _tuple19*),1U));}));e->r=_tmp951;});}}}else{
# 2675
struct Cyc_List_List*_tmp449=0;struct Cyc_List_List*newdles=_tmp449;
struct Cyc_List_List*_tmp44A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp44A;
for(0;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp44B=fields;struct Cyc_List_List*fields2=_tmp44B;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple34*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple34*_tmp44C=(struct _tuple34*)fields2->hd;struct _tuple34*_stmttmp4B=_tmp44C;struct _tuple34*_tmp44D=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Aggrfield*_tmp44E;_LL117: _tmp44E=_tmp44D->f1;_tmp44F=_tmp44D->f2;_LL118: {struct Cyc_Absyn_Aggrfield*field=_tmp44E;struct Cyc_Absyn_Exp*fieldexp=_tmp44F;
void*_tmp450=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp450;
void*_tmp451=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp451;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2685
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2687
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp952=fieldtyp;Cyc_Toc_cast_it(_tmp952,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp453=_cycalloc(sizeof(*_tmp453));({struct _tuple19*_tmp953=({struct _tuple19*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->f1=0,_tmp452->f2=fieldexp;_tmp452;});_tmp453->hd=_tmp953;}),_tmp453->tl=newdles;_tmp453;});
break;}}}}
# 2694
({void*_tmp954=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(newdles));e->r=_tmp954;});}}
# 2697
goto _LL7;}}}}case 31U: _LL4A: _tmp2E4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2E5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp2E6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL4B: {struct Cyc_List_List*es=_tmp2E4;struct Cyc_Absyn_Datatypedecl*tud=_tmp2E5;struct Cyc_Absyn_Datatypefield*tuf=_tmp2E6;
# 2701
struct Cyc_List_List*_tmp454=tuf->typs;struct Cyc_List_List*tqts=_tmp454;
{struct Cyc_List_List*_tmp455=es;struct Cyc_List_List*es2=_tmp455;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp456=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp456;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp955=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp955;});}}{
# 2712
struct Cyc_Absyn_Exp*_tmp457=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2712
struct Cyc_Absyn_Exp*tag_exp=_tmp457;
# 2715
if(!Cyc_Toc_is_toplevel(nv)){
# 2717
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));({struct _tuple19*_tmp957=({struct _fat_ptr*_tmp956=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp956,(struct Cyc_Absyn_Exp*)es->hd);});_tmp458->hd=_tmp957;}),_tmp458->tl=dles;_tmp458;});}}{
# 2723
struct _tuple19*_tmp459=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp459;
({void*_tmp95B=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=29U,({struct _tuple1*_tmp95A=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp45B->f1=_tmp95A;}),_tmp45B->f2=0,({
struct Cyc_List_List*_tmp959=({struct Cyc_List_List*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->hd=tag_dle,({struct Cyc_List_List*_tmp958=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp45A->tl=_tmp958;});_tmp45A;});_tmp45B->f3=_tmp959;}),_tmp45B->f4=0;_tmp45B;});
# 2724
e->r=_tmp95B;});}}else{
# 2729
struct Cyc_List_List*_tmp45C=0;struct Cyc_List_List*dles=_tmp45C;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp45E=_cycalloc(sizeof(*_tmp45E));({struct _tuple19*_tmp95C=({struct _tuple19*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->f1=0,_tmp45D->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp45D;});_tmp45E->hd=_tmp95C;}),_tmp45E->tl=dles;_tmp45E;});}
({void*_tmp95F=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp460=_cycalloc(sizeof(*_tmp460));({struct _tuple19*_tmp95E=({struct _tuple19*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->f1=0,_tmp45F->f2=tag_exp;_tmp45F;});_tmp460->hd=_tmp95E;}),({
struct Cyc_List_List*_tmp95D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp460->tl=_tmp95D;});_tmp460;}));
# 2732
e->r=_tmp95F;});}
# 2735
goto _LL7;}}case 32U: _LL4C: _LL4D:
# 2737
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp2DE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).is_calloc;_tmp2DF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).rgn;_tmp2E0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).elt_type;_tmp2E1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).num_elts;_tmp2E2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).fat_result;_tmp2E3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).inline_call;_LL51: {int is_calloc=_tmp2DE;struct Cyc_Absyn_Exp*rgnopt=_tmp2DF;void**topt=_tmp2E0;struct Cyc_Absyn_Exp*num_elts=_tmp2E1;int is_fat=_tmp2E2;int inline_call=_tmp2E3;
# 2741
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(topt)));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2750
if(is_calloc){
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp461[3U];_tmp461[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp960=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp461[1]=_tmp960;}),_tmp461[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp961=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp961,_tag_fat(_tmp461,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2756
pexp=({void*_tmp963=*topt;struct Cyc_Absyn_Exp*_tmp962=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp963,_tmp962,num_elts2);});}}else{
# 2758
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
if(inline_call)
pexp=Cyc_Toc_rmalloc_inline_exp(rgnopt,num_elts2);else{
# 2763
pexp=Cyc_Toc_rmalloc_exp(rgnopt,num_elts2);}}else{
# 2765
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2767
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp462[3U];_tmp462[0]=pexp,_tmp462[1]=elt_sz,_tmp462[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp964=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp964,_tag_fat(_tmp462,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp967=(struct _tuple1*)_check_null(x);void*_tmp966=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp965=num_elts;Cyc_Absyn_declare_stmt(_tmp967,_tmp966,_tmp965,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp968=Cyc_Toc_stmt_exp_r(s);e->r=_tmp968;});}else{
# 2774
e->r=pexp->r;}
goto _LL7;}}case 35U: _LL52: _tmp2DC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2DD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL53: {struct Cyc_Absyn_Exp*e1=_tmp2DC;struct Cyc_Absyn_Exp*e2=_tmp2DD;
# 2784
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp463=0U;({struct _fat_ptr _tmp969=({const char*_tmp464="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp464,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp969,_tag_fat(_tmp463,sizeof(void*),0U));});});{
# 2792
unsigned _tmp465=e->loc;unsigned loc=_tmp465;
struct _tuple1*_tmp466=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp466;
struct Cyc_Absyn_Exp*_tmp467=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp467;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp468=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp468;
struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp469;v2e->topt=e2_old_typ;{
# 2798
struct Cyc_Absyn_Exp*_tmp46A=({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp96A=v2e;Cyc_Absyn_assign_exp(_tmp96B,_tmp96A,loc);});struct Cyc_Absyn_Exp*s1e=_tmp46A;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp46B=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp46B;
struct Cyc_Absyn_Exp*_tmp46C=({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp96C=v1e;Cyc_Absyn_assign_exp(_tmp96D,_tmp96C,loc);});struct Cyc_Absyn_Exp*s2e=_tmp46C;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp46D=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp46D;
# 2803
struct Cyc_Absyn_Stmt*_tmp46E=({struct _tuple1*_tmp975=v1;void*_tmp974=e1_old_typ;struct Cyc_Absyn_Exp*_tmp973=e1;struct Cyc_Absyn_Stmt*_tmp972=({
struct _tuple1*_tmp971=v2;void*_tmp970=e2_old_typ;struct Cyc_Absyn_Exp*_tmp96F=e2;struct Cyc_Absyn_Stmt*_tmp96E=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2804
Cyc_Absyn_declare_stmt(_tmp971,_tmp970,_tmp96F,_tmp96E,loc);});
# 2803
Cyc_Absyn_declare_stmt(_tmp975,_tmp974,_tmp973,_tmp972,loc);});struct Cyc_Absyn_Stmt*s=_tmp46E;
# 2806
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmp976=Cyc_Toc_stmt_exp_r(s);e->r=_tmp976;});
goto _LL7;}}}}}}case 38U: _LL54: _tmp2DA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2DB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp2DA;struct _fat_ptr*f=_tmp2DB;
# 2811
void*_tmp46F=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp46F;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp470=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp471;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->f1).KnownAggr).tag == 2){_LL11A: _tmp471=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->f1).KnownAggr).val;_LL11B: {struct Cyc_Absyn_Aggrdecl*ad=_tmp471;
# 2815
struct Cyc_Absyn_Exp*_tmp472=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp472;
struct Cyc_Absyn_Exp*_tmp473=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp473;
struct Cyc_Absyn_Exp*_tmp474=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp474;
({void*_tmp977=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmp977;});
goto _LL119;}}else{goto _LL11C;}}else{goto _LL11C;}}else{_LL11C: _LL11D:
({void*_tmp475=0U;({struct _fat_ptr _tmp978=({const char*_tmp476="non-aggregate type in tagcheck";_tag_fat(_tmp476,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp978,_tag_fat(_tmp475,sizeof(void*),0U));});});}_LL119:;}
# 2822
goto _LL7;}case 37U: _LL56: _tmp2D9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL57: {struct Cyc_Absyn_Stmt*s=_tmp2D9;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36U: _LL58: _LL59:
# 2825
({void*_tmp477=0U;({struct _fat_ptr _tmp979=({const char*_tmp478="UnresolvedMem";_tag_fat(_tmp478,sizeof(char),14U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp979,_tag_fat(_tmp477,sizeof(void*),0U));});});case 25U: _LL5A: _LL5B:
({void*_tmp479=0U;({struct _fat_ptr _tmp97A=({const char*_tmp47A="compoundlit";_tag_fat(_tmp47A,sizeof(char),12U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp97A,_tag_fat(_tmp479,sizeof(void*),0U));});});case 39U: _LL5C: _LL5D:
({void*_tmp47B=0U;({struct _fat_ptr _tmp97B=({const char*_tmp47C="valueof(-)";_tag_fat(_tmp47C,sizeof(char),11U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp97B,_tag_fat(_tmp47B,sizeof(void*),0U));});});default: _LL5E: _LL5F:
({void*_tmp47D=0U;({struct _fat_ptr _tmp97C=({const char*_tmp47E="__asm__";_tag_fat(_tmp47E,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp97C,_tag_fat(_tmp47D,sizeof(void*),0U));});});}_LL7:;}
# 2830
if(!did_inserted_checks)
({void*_tmp47F=0U;({unsigned _tmp980=e->loc;struct _fat_ptr _tmp97F=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp482=({struct Cyc_String_pa_PrintArg_struct _tmp768;_tmp768.tag=0U,({
struct _fat_ptr _tmp97D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp768.f1=_tmp97D;});_tmp768;});void*_tmp480[1U];_tmp480[0]=& _tmp482;({struct _fat_ptr _tmp97E=({const char*_tmp481="Toc did not examine an inserted check: %s";_tag_fat(_tmp481,sizeof(char),42U);});Cyc_aprintf(_tmp97E,_tag_fat(_tmp480,sizeof(void*),1U));});});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp980,_tmp97F,_tag_fat(_tmp47F,sizeof(void*),0U));});});}}struct _tuple35{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2859 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2861
return({struct _tuple35*_tmp483=_region_malloc(r,sizeof(*_tmp483));_tmp483->f1=0,({struct _fat_ptr*_tmp982=Cyc_Toc_fresh_label();_tmp483->f2=_tmp982;}),({struct _fat_ptr*_tmp981=Cyc_Toc_fresh_label();_tmp483->f3=_tmp981;}),_tmp483->f4=sc;_tmp483;});}
# 2867
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp484=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp484;
# 2873
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp487=p->orig_pat;if((_tmp487.pattern).tag != 1)_throw_match();(_tmp487.pattern).val;})->topt);
void*_tmp485=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp485;
void*_tmp486=t2;switch(*((int*)_tmp486)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp486)->f1)){case 0U: _LL1: _LL2:
# 2878
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmp983=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp983,v);});goto _LL0;}_LL0:;}{
# 2884
void*_tmp488=p->access;void*_stmttmp4C=_tmp488;void*_tmp489=_stmttmp4C;struct _fat_ptr*_tmp48B;int _tmp48A;unsigned _tmp48C;unsigned _tmp48D;switch(*((int*)_tmp489)){case 0U: _LLA: _LLB:
# 2888
 goto _LL9;case 1U: _LLC: _LLD:
# 2893
 if(ps->tl != 0){
void*_tmp48E=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4D=_tmp48E;void*_tmp48F=_stmttmp4D;unsigned _tmp492;struct Cyc_Absyn_Datatypefield*_tmp491;struct Cyc_Absyn_Datatypedecl*_tmp490;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp48F)->tag == 3U){_LL15: _tmp490=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp48F)->f1;_tmp491=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp48F)->f2;_tmp492=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp48F)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp490;struct Cyc_Absyn_Datatypefield*tuf=_tmp491;unsigned i=_tmp492;
# 2896
ps=ps->tl;
v=({void*_tmp985=({void*_tmp984=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmp984,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp985,v);});
v=({struct Cyc_Absyn_Exp*_tmp986=v;Cyc_Absyn_aggrarrow_exp(_tmp986,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2903
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp48D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp489)->f3;_LLF: {unsigned i=_tmp48D;
_tmp48C=i;goto _LL11;}case 2U: _LL10: _tmp48C=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp489)->f1;_LL11: {unsigned i=_tmp48C;
v=({struct Cyc_Absyn_Exp*_tmp987=v;Cyc_Toc_member_exp(_tmp987,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _LL12: _tmp48A=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp489)->f1;_tmp48B=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp489)->f2;_LL13: {int tagged=_tmp48A;struct _fat_ptr*f=_tmp48B;
# 2908
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2914
return v;}
# 2919
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp493=t;struct Cyc_Absyn_Datatypefield*_tmp495;struct Cyc_Absyn_Datatypedecl*_tmp494;int _tmp497;struct _fat_ptr*_tmp496;struct Cyc_Absyn_Datatypefield*_tmp49A;struct Cyc_Absyn_Datatypedecl*_tmp499;int _tmp498;unsigned _tmp49B;int _tmp49D;struct _fat_ptr _tmp49C;struct Cyc_Absyn_Enumfield*_tmp49F;void*_tmp49E;struct Cyc_Absyn_Enumfield*_tmp4A1;struct Cyc_Absyn_Enumdecl*_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A2;switch(*((int*)_tmp493)){case 0U: _LL1: _tmp4A2=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp493)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp4A2;
return e == 0?v: e;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp988=v;Cyc_Absyn_eq_exp(_tmp988,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp989=v;Cyc_Absyn_neq_exp(_tmp989,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4A0=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp493)->f1;_tmp4A1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp493)->f2;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp4A0;struct Cyc_Absyn_Enumfield*ef=_tmp4A1;
return({struct Cyc_Absyn_Exp*_tmp98A=v;Cyc_Absyn_eq_exp(_tmp98A,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->tag=32U,_tmp4A3->f1=ed,_tmp4A3->f2=ef;_tmp4A3;}),0U),0U);});}case 4U: _LL9: _tmp49E=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp493)->f1;_tmp49F=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp493)->f2;_LLA: {void*t=_tmp49E;struct Cyc_Absyn_Enumfield*ef=_tmp49F;
return({struct Cyc_Absyn_Exp*_tmp98B=v;Cyc_Absyn_eq_exp(_tmp98B,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->tag=33U,_tmp4A4->f1=t,_tmp4A4->f2=ef;_tmp4A4;}),0U),0U);});}case 5U: _LLB: _tmp49C=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp493)->f1;_tmp49D=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp493)->f2;_LLC: {struct _fat_ptr s=_tmp49C;int i=_tmp49D;
return({struct Cyc_Absyn_Exp*_tmp98C=v;Cyc_Absyn_eq_exp(_tmp98C,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6U: _LLD: _tmp49B=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp493)->f1;_LLE: {unsigned i=_tmp49B;
return({struct Cyc_Absyn_Exp*_tmp98D=v;Cyc_Absyn_eq_exp(_tmp98D,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7U: _LLF: _tmp498=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp493)->f1;_tmp499=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp493)->f2;_tmp49A=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp493)->f3;_LL10: {int i=_tmp498;struct Cyc_Absyn_Datatypedecl*tud=_tmp499;struct Cyc_Absyn_Datatypefield*tuf=_tmp49A;
# 2931
LOOP1: {
void*_tmp4A5=v->r;void*_stmttmp4E=_tmp4A5;void*_tmp4A6=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;switch(*((int*)_tmp4A6)){case 14U: _LL16: _tmp4A8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4A6)->f2;_LL17: {struct Cyc_Absyn_Exp*e=_tmp4A8;
v=e;goto LOOP1;}case 20U: _LL18: _tmp4A7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4A6)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp4A7;
v=e;goto _LL15;}default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2938
v=({void*_tmp98F=({void*_tmp98E=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmp98E,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp98F,v);});return({struct Cyc_Absyn_Exp*_tmp990=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp990,Cyc_Absyn_uint_exp((unsigned)i,0U),0U);});}case 8U: _LL11: _tmp496=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp493)->f1;_tmp497=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp493)->f2;_LL12: {struct _fat_ptr*f=_tmp496;int i=_tmp497;
# 2942
return({struct Cyc_Absyn_Exp*_tmp992=({struct Cyc_Absyn_Exp*_tmp991=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmp991,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmp992,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _LL13: _tmp494=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp493)->f1;_tmp495=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp493)->f2;_LL14: {struct Cyc_Absyn_Datatypedecl*tud=_tmp494;struct Cyc_Absyn_Datatypefield*tuf=_tmp495;
# 2946
LOOP2: {
void*_tmp4A9=v->r;void*_stmttmp4F=_tmp4A9;void*_tmp4AA=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;switch(*((int*)_tmp4AA)){case 14U: _LL1D: _tmp4AC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AA)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4AC;
v=e;goto LOOP2;}case 20U: _LL1F: _tmp4AB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4AB;
v=e;goto _LL1C;}default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2953
v=({void*_tmp994=({void*_tmp993=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmp993,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp994,v);});
return({struct Cyc_Absyn_Exp*_tmp995=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp995,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2965
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 2972
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2974
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4AD=*((struct _tuple32*)ss->hd);struct _tuple32 _stmttmp50=_tmp4AD;struct _tuple32 _tmp4AE=_stmttmp50;void*_tmp4AF;_LL1: _tmp4AF=_tmp4AE.f1;_LL2: {void*ptest=_tmp4AF;
void*_tmp4B0=ptest;struct Cyc_Absyn_Exp*_tmp4B1;struct _fat_ptr*_tmp4B2;switch(*((int*)_tmp4B0)){case 3U: _LL4: _LL5:
# 2980
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4B2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4B0)->f1;_LLB: {struct _fat_ptr*f=_tmp4B2;
# 2984
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->tag=3U,_tmp4B3->f1=f;_tmp4B3;});
continue;}case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4B1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4B0)->f1;if(_tmp4B1 != 0){_LLF: {struct Cyc_Absyn_Exp*e=_tmp4B1;
# 2989
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->tag=2U,_tmp4B4->f1=e;_tmp4B4;});return({struct _tuple36 _tmp769;_tmp769.f1=0,_tmp769.f2=k;_tmp769;});}}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp76A;_tmp76A.f1=0,_tmp76A.f2=k;_tmp76A;});}_LL3:;}}
# 2997
return({struct _tuple36 _tmp76B;_tmp76B.f1=c,_tmp76B.f2=k;_tmp76B;});}
# 3002
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4B5=p;int _tmp4B6;int _tmp4B7;unsigned _tmp4B8;struct Cyc_Absyn_Enumfield*_tmp4BA;void*_tmp4B9;struct Cyc_Absyn_Enumfield*_tmp4BC;struct Cyc_Absyn_Enumdecl*_tmp4BB;switch(*((int*)_tmp4B5)){case 3U: _LL1: _tmp4BB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4B5)->f1;_tmp4BC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4B5)->f2;_LL2: {struct Cyc_Absyn_Enumdecl*ed=_tmp4BB;struct Cyc_Absyn_Enumfield*ef=_tmp4BC;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->tag=32U,_tmp4BD->f1=ed,_tmp4BD->f2=ef;_tmp4BD;}),0U);goto _LL0;}case 4U: _LL3: _tmp4B9=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4B5)->f1;_tmp4BA=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4B5)->f2;_LL4: {void*t=_tmp4B9;struct Cyc_Absyn_Enumfield*ef=_tmp4BA;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->tag=33U,_tmp4BE->f1=t,_tmp4BE->f2=ef;_tmp4BE;}),0U);goto _LL0;}case 6U: _LL5: _tmp4B8=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4B5)->f1;_LL6: {unsigned i=_tmp4B8;
_tmp4B7=(int)i;goto _LL8;}case 7U: _LL7: _tmp4B7=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4B5)->f1;_LL8: {int i=_tmp4B7;
_tmp4B6=i;goto _LLA;}case 8U: _LL9: _tmp4B6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4B5)->f2;_LLA: {int i=_tmp4B6;
e=Cyc_Absyn_uint_exp((unsigned)i,0U);goto _LL0;}default: _LLB: _LLC:
({void*_tmp4BF=0U;({struct _fat_ptr _tmp996=({const char*_tmp4C0="compile_pat_test_as_case!";_tag_fat(_tmp4C0,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp996,_tag_fat(_tmp4BF,sizeof(void*),0U));});});}_LL0:;}
# 3012
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->tag=17U,_tmp4C1->f1=e;_tmp4C1;}),0U);}
# 3016
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple38{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3027
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3031
void*t=(void*)_check_null(p->topt);
void*_tmp4C2=p->r;void*_stmttmp51=_tmp4C2;void*_tmp4C3=_stmttmp51;struct Cyc_List_List*_tmp4C5;union Cyc_Absyn_AggrInfo _tmp4C4;struct Cyc_List_List*_tmp4C6;struct Cyc_List_List*_tmp4C7;struct Cyc_Absyn_Pat*_tmp4C8;struct Cyc_List_List*_tmp4CB;struct Cyc_Absyn_Datatypefield*_tmp4CA;struct Cyc_Absyn_Datatypedecl*_tmp4C9;struct Cyc_Absyn_Pat*_tmp4CD;struct Cyc_Absyn_Vardecl*_tmp4CC;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Pat*_tmp4D0;struct Cyc_Absyn_Vardecl*_tmp4CF;struct Cyc_Absyn_Vardecl*_tmp4D1;switch(*((int*)_tmp4C3)){case 2U: _LL1: _tmp4D1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4C3)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp4D1;
# 3034
struct Cyc_Absyn_Pat*_tmp4D2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp4D2;
p2->topt=p->topt;
_tmp4CF=vd;_tmp4D0=p2;goto _LL4;}case 1U: _LL3: _tmp4CF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1;_tmp4D0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4C3)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4CF;struct Cyc_Absyn_Pat*p2=_tmp4D0;
# 3039
struct Cyc_Absyn_Vardecl*_tmp4D3=({struct _tuple1*_tmp997=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp997,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp4D3;
({struct Cyc_List_List*_tmp999=({struct Cyc_List_List*_tmp4D5=_region_malloc(rgn,sizeof(*_tmp4D5));({struct _tuple37*_tmp998=({struct _tuple37*_tmp4D4=_region_malloc(rgn,sizeof(*_tmp4D4));_tmp4D4->f1=vd,_tmp4D4->f2=new_vd;_tmp4D4;});_tmp4D5->hd=_tmp998;}),_tmp4D5->tl=*decls;_tmp4D5;});*decls=_tmp999;});{
struct Cyc_Absyn_Stmt*_tmp4D6=({struct Cyc_Absyn_Exp*_tmp99A=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7->tag=4U,_tmp4D7->f1=new_vd;_tmp4D7;}),0U);Cyc_Absyn_assign_stmt(_tmp99A,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp4D6;
return({struct Cyc_Absyn_Stmt*_tmp99B=s;Cyc_Toc_seq_stmt_opt(_tmp99B,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4U: _LL5: _tmp4CE=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4C3)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp4CE;
# 3045
struct Cyc_Absyn_Vardecl*_tmp4D8=({struct _tuple1*_tmp99C=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp99C,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp4D8;
({struct Cyc_List_List*_tmp99E=({struct Cyc_List_List*_tmp4DA=_region_malloc(rgn,sizeof(*_tmp4DA));({struct _tuple37*_tmp99D=({struct _tuple37*_tmp4D9=_region_malloc(rgn,sizeof(*_tmp4D9));_tmp4D9->f1=vd,_tmp4D9->f2=new_vd;_tmp4D9;});_tmp4DA->hd=_tmp99D;}),_tmp4DA->tl=*decls;_tmp4DA;});*decls=_tmp99E;});
return({struct Cyc_Absyn_Exp*_tmp99F=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->tag=4U,_tmp4DB->f1=new_vd;_tmp4DB;}),0U);Cyc_Absyn_assign_stmt(_tmp99F,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp4CC=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1;_tmp4CD=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4C3)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp4CC;struct Cyc_Absyn_Pat*p2=_tmp4CD;
# 3051
({void*_tmp9A0=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmp9A0;});{
struct Cyc_Absyn_Vardecl*_tmp4DC=({struct _tuple1*_tmp9A1=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp9A1,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp4DC;
({struct Cyc_List_List*_tmp9A3=({struct Cyc_List_List*_tmp4DE=_region_malloc(rgn,sizeof(*_tmp4DE));({struct _tuple37*_tmp9A2=({struct _tuple37*_tmp4DD=_region_malloc(rgn,sizeof(*_tmp4DD));_tmp4DD->f1=vd,_tmp4DD->f2=new_vd;_tmp4DD;});_tmp4DE->hd=_tmp9A2;}),_tmp4DE->tl=*decls;_tmp4DE;});*decls=_tmp9A3;});{
# 3055
struct Cyc_Absyn_Stmt*_tmp4DF=({struct Cyc_Absyn_Exp*_tmp9A6=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->tag=4U,_tmp4E0->f1=new_vd;_tmp4E0;}),0U);Cyc_Absyn_assign_stmt(_tmp9A6,({
void*_tmp9A5=({void*_tmp9A4=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp9A4,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9A5,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3055
struct Cyc_Absyn_Stmt*s=_tmp4DF;
# 3058
return({struct Cyc_Absyn_Stmt*_tmp9A7=s;Cyc_Toc_seq_stmt_opt(_tmp9A7,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9U: _LLB: _LLC:
# 3060
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1)->r)->tag == 8U){_LL17: _tmp4C9=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1)->r)->f1;_tmp4CA=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1)->r)->f2;_tmp4CB=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1)->r)->f3;_LL18: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4C9;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CA;struct Cyc_List_List*ps=_tmp4CB;
# 3069
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp4E1=({void*_tmp9A8=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp9A8,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp4E1;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp4E2=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp4E2;
struct Cyc_Absyn_Stmt*s=0;
for(0;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4E3=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp4E3;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4E4=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp4E4;
void*_tmp4E5=(void*)_check_null(p2->topt);void*t2=_tmp4E5;
void*_tmp4E6=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp4E6;
struct Cyc_Absyn_Exp*_tmp4E7=({struct Cyc_Absyn_Exp*_tmp9A9=path;Cyc_Absyn_aggrarrow_exp(_tmp9A9,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp4E7;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmp9AA=s;Cyc_Toc_seq_stmt_opt(_tmp9AA,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3087
return s;}}}}else{_LL21: _tmp4C8=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1;_LL22: {struct Cyc_Absyn_Pat*p2=_tmp4C8;
# 3132
return({struct _RegionHandle*_tmp9AE=rgn;struct Cyc_Toc_Env**_tmp9AD=nv;struct Cyc_List_List**_tmp9AC=decls;struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp9AE,_tmp9AD,_tmp9AC,_tmp9AB,p2);});}}case 8U: _LL19: _tmp4C7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4C3)->f3;_LL1A: {struct Cyc_List_List*ps=_tmp4C7;
# 3089
_tmp4C6=ps;goto _LL1C;}case 5U: _LL1B: _tmp4C6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp4C6;
# 3091
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4E8=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp4E8;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4E9=(void*)_check_null(p2->topt);void*t2=_tmp4E9;
struct _fat_ptr*_tmp4EA=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp4EA;
s=({struct Cyc_Absyn_Stmt*_tmp9B3=s;Cyc_Toc_seq_stmt_opt(_tmp9B3,({struct _RegionHandle*_tmp9B2=rgn;struct Cyc_Toc_Env**_tmp9B1=nv;struct Cyc_List_List**_tmp9B0=decls;struct Cyc_Absyn_Exp*_tmp9AF=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmp9B2,_tmp9B1,_tmp9B0,_tmp9AF,p2);}));});}}
# 3101
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4EB=0U;({struct _fat_ptr _tmp9B4=({const char*_tmp4EC="unresolved aggregate pattern!";_tag_fat(_tmp4EC,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B4,_tag_fat(_tmp4EB,sizeof(void*),0U));});});}else{_LL1F: _tmp4C4=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C3)->f1;_tmp4C5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C3)->f3;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp4C4;struct Cyc_List_List*dlps=_tmp4C5;
# 3105
struct Cyc_Absyn_Aggrdecl*_tmp4ED=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp4ED;
struct Cyc_Absyn_Stmt*s=0;
for(0;dlps != 0;dlps=dlps->tl){
struct _tuple38*_tmp4EE=(struct _tuple38*)dlps->hd;struct _tuple38*tup=_tmp4EE;
struct Cyc_Absyn_Pat*_tmp4EF=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp4EF;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp4F0=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp4F0;
void*_tmp4F1=(void*)_check_null(p2->topt);void*t2=_tmp4F1;
void*_tmp4F2=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp4F2;
struct Cyc_Absyn_Exp*_tmp4F3=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp4F3;
# 3117
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp4F4=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp4F4;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(ftype)))
# 3124
memexp=Cyc_Absyn_deref_exp(({void*_tmp9B5=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9B5,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3127
s=({struct Cyc_Absyn_Stmt*_tmp9B6=s;Cyc_Toc_seq_stmt_opt(_tmp9B6,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3129
return s;}}case 15U: _LL23: _LL24:
# 3134
({void*_tmp4F5=0U;({struct _fat_ptr _tmp9B7=({const char*_tmp4F6="unknownid pat";_tag_fat(_tmp4F6,sizeof(char),14U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B7,_tag_fat(_tmp4F5,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4F7=0U;({struct _fat_ptr _tmp9B8=({const char*_tmp4F8="unknowncall pat";_tag_fat(_tmp4F8,sizeof(char),16U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B8,_tag_fat(_tmp4F7,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp4F9=0U;({struct _fat_ptr _tmp9B9=({const char*_tmp4FA="exp pat";_tag_fat(_tmp4FA,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B9,_tag_fat(_tmp4F9,sizeof(void*),0U));});});}_LL0:;}
# 3140
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple37 _tmp4FB=*((struct _tuple37*)vs->hd);struct _tuple37 _stmttmp52=_tmp4FB;struct _tuple37 _tmp4FC=_stmttmp52;struct Cyc_Absyn_Vardecl*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp4FD;_LL1: _tmp4FD=_tmp4FC.f1;_tmp4FE=_tmp4FC.f2;_LL2: {struct Cyc_Absyn_Vardecl*oldv=_tmp4FD;struct Cyc_Absyn_Vardecl*newv=_tmp4FE;
if(oldv == newv)return newv;}}
# 3145
({void*_tmp4FF=0U;({struct _fat_ptr _tmp9BA=({const char*_tmp500="find_new_var";_tag_fat(_tmp500,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9BA,_tag_fat(_tmp4FF,sizeof(void*),0U));});});}
# 3151
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp501=e->r;void*_stmttmp53=_tmp501;void*_tmp502=_stmttmp53;struct Cyc_List_List*_tmp503;struct Cyc_List_List*_tmp504;struct Cyc_List_List*_tmp505;struct Cyc_List_List*_tmp506;struct Cyc_List_List*_tmp507;struct Cyc_List_List*_tmp508;struct Cyc_List_List*_tmp509;struct Cyc_List_List*_tmp50A;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Vardecl*_tmp528;struct Cyc_Absyn_Vardecl*_tmp529;switch(*((int*)_tmp502)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp502)->f1)){case 5U: _LL1: _tmp529=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp502)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp529;
_tmp528=vd;goto _LL4;}case 4U: _LL3: _tmp528=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp502)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp528;
# 3155
{struct _handler_cons _tmp52A;_push_handler(& _tmp52A);{int _tmp52C=0;if(setjmp(_tmp52A.handler))_tmp52C=1;if(!_tmp52C){
({void*_tmp9BD=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->tag=1U,({void*_tmp9BC=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->tag=4U,({struct Cyc_Absyn_Vardecl*_tmp9BB=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,vd);_tmp52D->f1=_tmp9BB;});_tmp52D;});_tmp52E->f1=_tmp9BC;});_tmp52E;});e->r=_tmp9BD;});;_pop_handler();}else{void*_tmp52B=(void*)Cyc_Core_get_exn_thrown();void*_tmp52F=_tmp52B;void*_tmp530;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp52F)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3158
 goto _LL3F;}else{_LL42: _tmp530=_tmp52F;_LL43: {void*exn=_tmp530;(int)_rethrow(exn);}}_LL3F:;}}}
# 3160
goto _LL0;}default: goto _LL3D;}case 6U: _LL5: _tmp525=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_tmp526=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_tmp527=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp502)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp525;struct Cyc_Absyn_Exp*e2=_tmp526;struct Cyc_Absyn_Exp*e3=_tmp527;
# 3162
Cyc_Toc_subst_pattern_vars(env,e1);_tmp523=e2;_tmp524=e3;goto _LL8;}case 27U: _LL7: _tmp523=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_tmp524=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp502)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp523;struct Cyc_Absyn_Exp*e2=_tmp524;
_tmp521=e1;_tmp522=e2;goto _LLA;}case 7U: _LL9: _tmp521=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_tmp522=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp521;struct Cyc_Absyn_Exp*e2=_tmp522;
_tmp51F=e1;_tmp520=e2;goto _LLC;}case 8U: _LLB: _tmp51F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_tmp520=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp51F;struct Cyc_Absyn_Exp*e2=_tmp520;
_tmp51D=e1;_tmp51E=e2;goto _LLE;}case 23U: _LLD: _tmp51D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_tmp51E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp51D;struct Cyc_Absyn_Exp*e2=_tmp51E;
_tmp51B=e1;_tmp51C=e2;goto _LL10;}case 9U: _LLF: _tmp51B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_tmp51C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp51B;struct Cyc_Absyn_Exp*e2=_tmp51C;
# 3168
Cyc_Toc_subst_pattern_vars(env,e1);_tmp51A=e2;goto _LL12;}case 41U: _LL11: _tmp51A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp51A;
_tmp519=e;goto _LL14;}case 38U: _LL13: _tmp519=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp519;
_tmp518=e;goto _LL16;}case 11U: _LL15: _tmp518=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp518;
_tmp517=e;goto _LL18;}case 12U: _LL17: _tmp517=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp517;
_tmp516=e;goto _LL1A;}case 13U: _LL19: _tmp516=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp516;
_tmp515=e;goto _LL1C;}case 14U: _LL1B: _tmp515=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp515;
_tmp514=e;goto _LL1E;}case 18U: _LL1D: _tmp514=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp514;
_tmp513=e;goto _LL20;}case 20U: _LL1F: _tmp513=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp513;
_tmp512=e;goto _LL22;}case 21U: _LL21: _tmp512=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp512;
_tmp511=e;goto _LL24;}case 22U: _LL23: _tmp511=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp511;
_tmp510=e;goto _LL26;}case 28U: _LL25: _tmp510=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp510;
_tmp50F=e;goto _LL28;}case 15U: _LL27: _tmp50F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp50F;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34U: _LL29: _tmp50D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp502)->f1).rgn;_tmp50E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp502)->f1).num_elts;_LL2A: {struct Cyc_Absyn_Exp*eopt=_tmp50D;struct Cyc_Absyn_Exp*e=_tmp50E;
_tmp50B=eopt;_tmp50C=e;goto _LL2C;}case 16U: _LL2B: _tmp50B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_tmp50C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp50B;struct Cyc_Absyn_Exp*e=_tmp50C;
# 3183
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3U: _LL2D: _tmp50A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL2E: {struct Cyc_List_List*es=_tmp50A;
_tmp509=es;goto _LL30;}case 31U: _LL2F: _tmp509=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL30: {struct Cyc_List_List*es=_tmp509;
_tmp508=es;goto _LL32;}case 24U: _LL31: _tmp508=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL32: {struct Cyc_List_List*es=_tmp508;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36U: _LL33: _tmp507=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL34: {struct Cyc_List_List*dles=_tmp507;
_tmp506=dles;goto _LL36;}case 29U: _LL35: _tmp506=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp502)->f3;_LL36: {struct Cyc_List_List*dles=_tmp506;
_tmp505=dles;goto _LL38;}case 30U: _LL37: _tmp505=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL38: {struct Cyc_List_List*dles=_tmp505;
_tmp504=dles;goto _LL3A;}case 26U: _LL39: _tmp504=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp502)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp504;
_tmp503=dles;goto _LL3C;}case 25U: _LL3B: _tmp503=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp502)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp503;
# 3193
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp531=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp54=_tmp531;struct _tuple19*_tmp532=_stmttmp54;struct Cyc_Absyn_Exp*_tmp533;_LL45: _tmp533=_tmp532->f2;_LL46: {struct Cyc_Absyn_Exp*e=_tmp533;
Cyc_Toc_subst_pattern_vars(env,e);}}
# 3197
goto _LL0;}default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple39{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3207
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3215
void*_tmp534=dopt;void*_tmp537;struct Cyc_List_List*_tmp536;struct Cyc_List_List*_tmp535;struct Cyc_Tcpat_Rhs*_tmp538;if(_tmp534 == 0){_LL1: _LL2:
# 3217
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp534)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp538=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp534)->f1;_LL6: {struct Cyc_Tcpat_Rhs*rhs=_tmp538;
# 3221
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp539=(struct _tuple35*)lscs->hd;struct _tuple35*_stmttmp55=_tmp539;struct _tuple35*_tmp53A=_stmttmp55;struct Cyc_Absyn_Switch_clause*_tmp53E;struct _fat_ptr*_tmp53D;struct _fat_ptr*_tmp53C;int*_tmp53B;_LLA: _tmp53B=(int*)& _tmp53A->f1;_tmp53C=_tmp53A->f2;_tmp53D=_tmp53A->f3;_tmp53E=_tmp53A->f4;_LLB: {int*already_emitted=_tmp53B;struct _fat_ptr*init_lab=_tmp53C;struct _fat_ptr*code_lab=_tmp53D;struct Cyc_Absyn_Switch_clause*sc=_tmp53E;
struct Cyc_Absyn_Stmt*_tmp53F=sc->body;struct Cyc_Absyn_Stmt*body=_tmp53F;
if(body == rhs->rhs){
# 3226
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3231
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9BF=rgn;struct Cyc_Absyn_Exp*_tmp9BE=
Cyc_Absyn_var_exp(v,0U);
# 3231
Cyc_Toc_extract_pattern_vars(_tmp9BF,& nv,& newdecls,_tmp9BE,sc->pattern);});
# 3235
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp540=newdecls;struct Cyc_List_List*ds=_tmp540;for(0;ds != 0;ds=ds->tl){
struct _tuple37 _tmp541=*((struct _tuple37*)ds->hd);struct _tuple37 _stmttmp56=_tmp541;struct _tuple37 _tmp542=_stmttmp56;struct Cyc_Absyn_Vardecl*_tmp544;struct Cyc_Absyn_Vardecl*_tmp543;_LLD: _tmp543=_tmp542.f1;_tmp544=_tmp542.f2;_LLE: {struct Cyc_Absyn_Vardecl*oldv=_tmp543;struct Cyc_Absyn_Vardecl*newv=_tmp544;
({struct Cyc_List_List*_tmp9C1=({struct Cyc_List_List*_tmp546=_region_malloc(rgn,sizeof(*_tmp546));({struct _tuple37*_tmp9C0=({struct _tuple37*_tmp545=_region_malloc(rgn,sizeof(*_tmp545));_tmp545->f1=oldv,_tmp545->f2=newv;_tmp545;});_tmp546->hd=_tmp9C0;}),_tmp546->tl=*decls;_tmp546;});*decls=_tmp9C1;});
({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->tag=4U,_tmp547->f1=newv;_tmp547;}),0U);oldv->initializer=_tmp9C2;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmp9C4=({struct Cyc_Absyn_Decl*_tmp549=_cycalloc(sizeof(*_tmp549));({void*_tmp9C3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548->tag=0U,_tmp548->f1=oldv;_tmp548;});_tmp549->r=_tmp9C3;}),_tmp549->loc=0U;_tmp549;});Cyc_Absyn_decl_stmt(_tmp9C4,res,0U);});}}}
# 3244
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmp9C6=({struct Cyc_List_List*_tmp54B=_region_malloc(rgn,sizeof(*_tmp54B));({struct _tuple39*_tmp9C5=({struct _tuple39*_tmp54A=_region_malloc(rgn,sizeof(*_tmp54A));_tmp54A->f1=nv,_tmp54A->f2=code_lab,_tmp54A->f3=body;_tmp54A;});_tmp54B->hd=_tmp9C5;}),_tmp54B->tl=*bodies;_tmp54B;});*bodies=_tmp9C6;});
return res;}}}}
# 3252
({void*_tmp54C=0U;({struct _fat_ptr _tmp9C7=({const char*_tmp54D="couldn't find rhs!";_tag_fat(_tmp54D,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9C7,_tag_fat(_tmp54C,sizeof(void*),0U));});});}default: _LL7: _tmp535=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp534)->f1;_tmp536=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp534)->f2;_tmp537=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp534)->f3;_LL8: {struct Cyc_List_List*symbolic_path=_tmp535;struct Cyc_List_List*switches=_tmp536;void*other_decision=_tmp537;
# 3255
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3257
struct Cyc_Absyn_Exp*_tmp54E=({struct Cyc_List_List*_tmp9C8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmp9C8,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp54E;
struct Cyc_List_List*_tmp54F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp54F;
# 3260
struct _tuple36 _tmp550=Cyc_Toc_admits_switch(ss);struct _tuple36 _stmttmp57=_tmp550;struct _tuple36 _tmp551=_stmttmp57;void*_tmp553;int _tmp552;_LL10: _tmp552=_tmp551.f1;_tmp553=_tmp551.f2;_LL11: {int allows_switch=_tmp552;void*test_kind=_tmp553;
if(allows_switch > 1){
# 3264
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp564=_cycalloc(sizeof(*_tmp564));
({struct Cyc_Absyn_Switch_clause*_tmp9CA=({struct Cyc_Absyn_Switch_clause*_tmp563=_cycalloc(sizeof(*_tmp563));({struct Cyc_Absyn_Pat*_tmp9C9=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp563->pattern=_tmp9C9;}),_tmp563->pat_vars=0,_tmp563->where_clause=0,_tmp563->body=res,_tmp563->loc=0U;_tmp563;});_tmp564->hd=_tmp9CA;}),_tmp564->tl=0;_tmp564;});
# 3267
for(0;ss != 0;ss=ss->tl){
struct _tuple32 _tmp554=*((struct _tuple32*)ss->hd);struct _tuple32 _stmttmp58=_tmp554;struct _tuple32 _tmp555=_stmttmp58;void*_tmp557;void*_tmp556;_LL13: _tmp556=_tmp555.f1;_tmp557=_tmp555.f2;_LL14: {void*pat_test=_tmp556;void*dec_tree=_tmp557;
# 3270
struct Cyc_Absyn_Pat*_tmp558=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp558;
# 3272
struct Cyc_Absyn_Stmt*_tmp559=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp559;
# 3274
new_lscs=({struct Cyc_List_List*_tmp55B=_cycalloc(sizeof(*_tmp55B));({struct Cyc_Absyn_Switch_clause*_tmp9CB=({struct Cyc_Absyn_Switch_clause*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->pattern=case_exp,_tmp55A->pat_vars=0,_tmp55A->where_clause=0,_tmp55A->body=s,_tmp55A->loc=0U;_tmp55A;});_tmp55B->hd=_tmp9CB;}),_tmp55B->tl=new_lscs;_tmp55B;});}}
# 3276
{void*_tmp55C=test_kind;struct _fat_ptr*_tmp55D;switch(*((int*)_tmp55C)){case 1U: _LL16: _LL17:
# 3278
 LOOP1: {
void*_tmp55E=p->r;void*_stmttmp59=_tmp55E;void*_tmp55F=_stmttmp59;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp561;switch(*((int*)_tmp55F)){case 14U: _LL1F: _tmp561=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp55F)->f2;_LL20: {struct Cyc_Absyn_Exp*e=_tmp561;
p=e;goto LOOP1;}case 20U: _LL21: _tmp560=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55F)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp560;
p=e;goto _LL1E;}default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3284
p=Cyc_Absyn_deref_exp(({void*_tmp9CC=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9CC,p);}),0U);goto _LL15;case 3U: _LL18: _tmp55D=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp55C)->f1;_LL19: {struct _fat_ptr*f=_tmp55D;
# 3286
p=({struct Cyc_Absyn_Exp*_tmp9CD=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmp9CD,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3290
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=10U,_tmp562->f1=p,_tmp562->f2=new_lscs,_tmp562->f3=0;_tmp562;}),0U);}else{
# 3294
void*_tmp565=test_kind;struct Cyc_Absyn_Exp*_tmp566;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp565)->tag == 2U){_LL26: _tmp566=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp565)->f1;_LL27: {struct Cyc_Absyn_Exp*e=_tmp566;
# 3296
struct Cyc_List_List*_tmp567=ss;struct Cyc_Tcpat_Rhs*_tmp569;void*_tmp568;if(_tmp567 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp567)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp567)->tl == 0){_LL2B: _tmp568=((struct _tuple32*)_tmp567->hd)->f1;_tmp569=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp567->hd)->f2)->f1;_LL2C: {void*pat_test=_tmp568;struct Cyc_Tcpat_Rhs*rhs=_tmp569;
# 3302
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp56A=(struct _tuple35*)lscs->hd;struct _tuple35*_stmttmp5A=_tmp56A;struct _tuple35*_tmp56B=_stmttmp5A;struct Cyc_Absyn_Switch_clause*_tmp56F;struct _fat_ptr*_tmp56E;struct _fat_ptr*_tmp56D;int*_tmp56C;_LL30: _tmp56C=(int*)& _tmp56B->f1;_tmp56D=_tmp56B->f2;_tmp56E=_tmp56B->f3;_tmp56F=_tmp56B->f4;_LL31: {int*already_emitted=_tmp56C;struct _fat_ptr*init_lab=_tmp56D;struct _fat_ptr*code_lab=_tmp56E;struct Cyc_Absyn_Switch_clause*sc=_tmp56F;
struct Cyc_Absyn_Stmt*_tmp570=sc->body;struct Cyc_Absyn_Stmt*body=_tmp570;
if(body == rhs->rhs){
# 3307
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3312
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9CF=rgn;struct Cyc_Absyn_Exp*_tmp9CE=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp9CF,& nv,& newdecls,_tmp9CE,sc->pattern);});
# 3316
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp571=newdecls;struct Cyc_List_List*ds=_tmp571;for(0;ds != 0;ds=ds->tl){
struct _tuple37 _tmp572=*((struct _tuple37*)ds->hd);struct _tuple37 _stmttmp5B=_tmp572;struct _tuple37 _tmp573=_stmttmp5B;struct Cyc_Absyn_Vardecl*_tmp575;struct Cyc_Absyn_Vardecl*_tmp574;_LL33: _tmp574=_tmp573.f1;_tmp575=_tmp573.f2;_LL34: {struct Cyc_Absyn_Vardecl*oldv=_tmp574;struct Cyc_Absyn_Vardecl*newv=_tmp575;
({struct Cyc_List_List*_tmp9D1=({struct Cyc_List_List*_tmp577=_region_malloc(rgn,sizeof(*_tmp577));({struct _tuple37*_tmp9D0=({struct _tuple37*_tmp576=_region_malloc(rgn,sizeof(*_tmp576));_tmp576->f1=oldv,_tmp576->f2=newv;_tmp576;});_tmp577->hd=_tmp9D0;}),_tmp577->tl=*decls;_tmp577;});*decls=_tmp9D1;});
({struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->tag=4U,_tmp578->f1=newv;_tmp578;}),0U);oldv->initializer=_tmp9D2;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmp9D4=({struct Cyc_Absyn_Decl*_tmp57A=_cycalloc(sizeof(*_tmp57A));({void*_tmp9D3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=0U,_tmp579->f1=oldv;_tmp579;});_tmp57A->r=_tmp9D3;}),_tmp57A->loc=0U;_tmp57A;});Cyc_Absyn_decl_stmt(_tmp9D4,r,0U);});}}}
# 3325
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3329
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmp9D6=({struct Cyc_List_List*_tmp57C=_region_malloc(rgn,sizeof(*_tmp57C));({struct _tuple39*_tmp9D5=({struct _tuple39*_tmp57B=_region_malloc(rgn,sizeof(*_tmp57B));_tmp57B->f1=nv,_tmp57B->f2=code_lab,_tmp57B->f3=body;_tmp57B;});_tmp57C->hd=_tmp9D5;}),_tmp57C->tl=*bodies;_tmp57C;});*bodies=_tmp9D6;});
return r;}}}}
# 3339
({void*_tmp57D=0U;({struct _fat_ptr _tmp9D7=({const char*_tmp57E="couldn't find rhs!";_tag_fat(_tmp57E,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9D7,_tag_fat(_tmp57D,sizeof(void*),0U));});});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({void*_tmp57F=0U;({struct _fat_ptr _tmp9D8=({const char*_tmp580="bad where clause in match compiler";_tag_fat(_tmp580,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9D8,_tag_fat(_tmp57F,sizeof(void*),0U));});});}_LL2A:;}}else{_LL28: _LL29:
# 3344
 for(0;ss != 0;ss=ss->tl){
struct _tuple32 _tmp581=*((struct _tuple32*)ss->hd);struct _tuple32 _stmttmp5C=_tmp581;struct _tuple32 _tmp582=_stmttmp5C;void*_tmp584;void*_tmp583;_LL36: _tmp583=_tmp582.f1;_tmp584=_tmp582.f2;_LL37: {void*pat_test=_tmp583;void*dec_tree=_tmp584;
struct Cyc_Absyn_Exp*_tmp585=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp585;
struct Cyc_Absyn_Stmt*_tmp586=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp586;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}}_LL25:;}
# 3352
return res;}}}}_LL0:;}
# 3362
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3364
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple39*_tmp587=(struct _tuple39*)bodies->hd;struct _tuple39*_stmttmp5D=_tmp587;struct _tuple39*_tmp588=_stmttmp5D;struct Cyc_Absyn_Stmt*_tmp58A;struct Cyc_Toc_Env**_tmp589;_LL1: _tmp589=(struct Cyc_Toc_Env**)& _tmp588->f1;_tmp58A=_tmp588->f3;_LL2: {struct Cyc_Toc_Env**nv=_tmp589;struct Cyc_Absyn_Stmt*s2=_tmp58A;
if(s2 == s)return nv;}}
# 3370
return 0;}
# 3374
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3377
void*_tmp58B=(void*)_check_null(e->topt);void*t=_tmp58B;
Cyc_Toc_exp_to_c(nv,e);{
# 3380
struct _tuple29 _tmp58C=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp5E=_tmp58C;struct _tuple29 _tmp58D=_stmttmp5E;struct Cyc_Absyn_Exp*_tmp58F;struct _tuple1*_tmp58E;_LL1: _tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;_LL2: {struct _tuple1*v=_tmp58E;struct Cyc_Absyn_Exp*path=_tmp58F;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp590=_new_region("rgn");struct _RegionHandle*rgn=& _tmp590;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp591=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp591;
# 3386
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3391
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3399
{struct Cyc_List_List*_tmp592=lscs;struct Cyc_List_List*lscs2=_tmp592;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple35*_tmp593=(struct _tuple35*)lscs2->hd;struct _tuple35*_stmttmp5F=_tmp593;struct _tuple35*_tmp594=_stmttmp5F;struct Cyc_Absyn_Switch_clause*_tmp596;struct _fat_ptr*_tmp595;_LL4: _tmp595=_tmp594->f3;_tmp596=_tmp594->f4;_LL5: {struct _fat_ptr*body_lab=_tmp595;struct Cyc_Absyn_Switch_clause*body_sc=_tmp596;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3403
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3406
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp597=*envp;struct Cyc_Toc_Env*env=_tmp597;
# 3409
if(lscs2->tl != 0){
struct _tuple35*_tmp598=(struct _tuple35*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple35*_stmttmp60=_tmp598;struct _tuple35*_tmp599=_stmttmp60;struct Cyc_Absyn_Switch_clause*_tmp59B;struct _fat_ptr*_tmp59A;_LL7: _tmp59A=_tmp599->f3;_tmp59B=_tmp599->f4;_LL8: {struct _fat_ptr*fallthru_lab=_tmp59A;struct Cyc_Absyn_Switch_clause*next_sc=_tmp59B;
# 3413
struct Cyc_Toc_Env**_tmp59C=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp59C;
# 3418
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmp9D9=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmp9D9,s);});else{
# 3422
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmp9DA=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmp9DA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3427
({struct Cyc_Toc_Env*_tmp9DB=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmp9DB,s);});}}}else{
# 3431
({struct Cyc_Toc_Env*_tmp9DC=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmp9DC,s);});}}}}}{
# 3435
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmp9DE=test_tree;Cyc_Absyn_seq_stmt(_tmp9DE,({struct _fat_ptr*_tmp9DD=end_l;Cyc_Absyn_label_stmt(_tmp9DD,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3437
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple37 _tmp59D=*((struct _tuple37*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple37 _stmttmp61=_tmp59D;struct _tuple37 _tmp59E=_stmttmp61;struct Cyc_Absyn_Vardecl*_tmp59F;_LLA: _tmp59F=_tmp59E.f2;_LLB: {struct Cyc_Absyn_Vardecl*vd=_tmp59F;
res=({struct Cyc_Absyn_Decl*_tmp9DF=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->tag=0U,_tmp5A0->f1=vd;_tmp5A0;}),0U);Cyc_Absyn_decl_stmt(_tmp9DF,res,0U);});}}
# 3442
({void*_tmp9E3=({struct _tuple1*_tmp9E2=v;void*_tmp9E1=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmp9E0=e;Cyc_Absyn_declare_stmt(_tmp9E2,_tmp9E1,_tmp9E0,res,0U);})->r;whole_s->r=_tmp9E3;});
_npop_handler(0U);return;}}
# 3383
;_pop_region(rgn);}}}
# 3448
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3450
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3455
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3457
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5A1[1U];({struct Cyc_Absyn_Exp*_tmp9E4=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp5A1[0]=_tmp9E4;});({struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9E5,_tag_fat(_tmp5A1,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3460
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmp9E7=({struct Cyc_Absyn_Stmt*_tmp9E6=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmp9E6,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmp9E7;});}
# 3465
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3467
while(1){
void*_tmp5A2=s->r;void*_stmttmp62=_tmp5A2;void*_tmp5A3=_stmttmp62;void*_tmp5A6;struct Cyc_List_List*_tmp5A5;struct Cyc_Absyn_Stmt*_tmp5A4;struct Cyc_Absyn_Stmt*_tmp5A8;struct Cyc_Absyn_Decl*_tmp5A7;struct Cyc_Absyn_Switch_clause**_tmp5AA;struct Cyc_List_List*_tmp5A9;void*_tmp5AD;struct Cyc_List_List*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Stmt*_tmp5AF;struct _fat_ptr*_tmp5AE;struct Cyc_Absyn_Stmt*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B3;struct Cyc_Absyn_Stmt*_tmp5B2;struct Cyc_Absyn_Stmt*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B4;struct Cyc_Absyn_Stmt*_tmp5BA;struct Cyc_Absyn_Stmt*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_Absyn_Stmt*_tmp5BC;struct Cyc_Absyn_Stmt*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BE;switch(*((int*)_tmp5A3)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp5BE=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5BE;
Cyc_Toc_exp_to_c(nv,e);return;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1 == 0){_LL5: _LL6:
({int _tmp9E8=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9E8,s);});return;}else{_LL7: _tmp5BD=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp5BD;
# 3474
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp5BF=Cyc_Toc_temp_var();struct _tuple1*x=_tmp5BF;
struct Cyc_Absyn_Stmt*_tmp5C0=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp5C0;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmp9E9=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmp9E9;});}
# 3483
return;}}}case 2U: _LL9: _tmp5BB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5BC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_LLA: {struct Cyc_Absyn_Stmt*s1=_tmp5BB;struct Cyc_Absyn_Stmt*s2=_tmp5BC;
# 3485
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4U: _LLB: _tmp5B8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5B9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_tmp5BA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5B8;struct Cyc_Absyn_Stmt*s1=_tmp5B9;struct Cyc_Absyn_Stmt*s2=_tmp5BA;
# 3489
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9U: _LLD: _tmp5B4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5B5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2).f1;_tmp5B6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f3).f1;_tmp5B7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5B4;struct Cyc_Absyn_Exp*e2=_tmp5B5;struct Cyc_Absyn_Exp*e3=_tmp5B6;struct Cyc_Absyn_Stmt*s2=_tmp5B7;
# 3494
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5B2=s2;_tmp5B3=e3;goto _LL10;}case 14U: _LLF: _tmp5B2=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5B3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5B2;struct Cyc_Absyn_Exp*e=_tmp5B3;
_tmp5B0=e;_tmp5B1=s2;goto _LL12;}case 5U: _LL11: _tmp5B0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1).f1;_tmp5B1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5B0;struct Cyc_Absyn_Stmt*s2=_tmp5B1;
# 3499
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp5C1=_new_region("temp");struct _RegionHandle*temp=& _tmp5C1;_push_region(temp);({struct Cyc_Toc_Env*_tmp9EA=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9EA,s2);});
_npop_handler(0U);return;
# 3500
;_pop_region(temp);}}case 6U: _LL13: _LL14: {
# 3503
struct Cyc_Toc_Env*_tmp5C2=nv;struct _fat_ptr**_tmp5C3;_LL24: _tmp5C3=_tmp5C2->break_lab;_LL25: {struct _fat_ptr**b=_tmp5C3;
if(b != 0)
({void*_tmp9EB=Cyc_Toc_goto_stmt_r(*b);s->r=_tmp9EB;});
# 3507
({int _tmp9EC=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9EC,s);});
return;}}case 7U: _LL15: _LL16: {
# 3510
struct Cyc_Toc_Env*_tmp5C4=nv;struct _fat_ptr**_tmp5C5;_LL27: _tmp5C5=_tmp5C4->continue_lab;_LL28: {struct _fat_ptr**c=_tmp5C5;
if(c != 0)
({void*_tmp9ED=Cyc_Toc_goto_stmt_r(*c);s->r=_tmp9ED;});
goto _LL18;}}case 8U: _LL17: _LL18:
# 3515
({int _tmp9EE=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9EE,s);});
return;case 13U: _LL19: _tmp5AE=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5AF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_LL1A: {struct _fat_ptr*lab=_tmp5AE;struct Cyc_Absyn_Stmt*s1=_tmp5AF;
s=s1;continue;}case 10U: _LL1B: _tmp5AB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5AC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_tmp5AD=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f3;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5AB;struct Cyc_List_List*scs=_tmp5AC;void*dec_tree_opt=_tmp5AD;
# 3519
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11U: _LL1D: _tmp5A9=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5AA=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_LL1E: {struct Cyc_List_List*es=_tmp5A9;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5AA;
# 3522
struct Cyc_Toc_Env*_tmp5C6=nv;struct Cyc_Toc_FallthruInfo*_tmp5C7;_LL2A: _tmp5C7=_tmp5C6->fallthru_info;_LL2B: {struct Cyc_Toc_FallthruInfo*fi=_tmp5C7;
if(fi == 0)
({void*_tmp5C8=0U;({struct _fat_ptr _tmp9EF=({const char*_tmp5C9="fallthru in unexpected place";_tag_fat(_tmp5C9,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9EF,_tag_fat(_tmp5C8,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp5CA=*fi;struct Cyc_Toc_FallthruInfo _stmttmp63=_tmp5CA;struct Cyc_Toc_FallthruInfo _tmp5CB=_stmttmp63;struct Cyc_List_List*_tmp5CD;struct _fat_ptr*_tmp5CC;_LL2D: _tmp5CC=_tmp5CB.label;_tmp5CD=_tmp5CB.binders;_LL2E: {struct _fat_ptr*l=_tmp5CC;struct Cyc_List_List*vs=_tmp5CD;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3528
({int _tmp9F0=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9F0,s2);});{
struct Cyc_List_List*_tmp5CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp5CE;
struct Cyc_List_List*_tmp5CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp5CF;
for(0;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmp9F2=({struct Cyc_Absyn_Exp*_tmp9F1=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->tag=5U,_tmp5D0->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp5D0;}),0U);Cyc_Absyn_assign_stmt(_tmp9F1,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmp9F2,s2,0U);});}
# 3536
s->r=s2->r;
return;}}}}}case 12U: _LL1F: _tmp5A7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5A8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp5A7;struct Cyc_Absyn_Stmt*s1=_tmp5A8;
# 3542
{void*_tmp5D1=d->r;void*_stmttmp64=_tmp5D1;void*_tmp5D2=_stmttmp64;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Vardecl*_tmp5D4;struct Cyc_Absyn_Tvar*_tmp5D3;struct Cyc_Absyn_Fndecl*_tmp5D6;struct Cyc_List_List*_tmp5D7;void*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Pat*_tmp5D8;struct Cyc_Absyn_Vardecl*_tmp5DB;switch(*((int*)_tmp5D2)){case 0U: _LL30: _tmp5DB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5D2)->f1;_LL31: {struct Cyc_Absyn_Vardecl*vd=_tmp5DB;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2U: _LL32: _tmp5D8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5D2)->f1;_tmp5D9=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5D2)->f3;_tmp5DA=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5D2)->f4;_LL33: {struct Cyc_Absyn_Pat*p=_tmp5D8;struct Cyc_Absyn_Exp*e=_tmp5D9;void*dec_tree=_tmp5DA;
# 3550
{void*_tmp5DC=p->r;void*_stmttmp65=_tmp5DC;void*_tmp5DD=_stmttmp65;struct Cyc_Absyn_Vardecl*_tmp5DE;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5DD)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5DD)->f2)->r)->tag == 0U){_LL3D: _tmp5DE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5DD)->f1;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp5DE;
# 3552
struct Cyc_Absyn_Vardecl*_tmp5DF=({struct _tuple1*_tmp9F4=Cyc_Toc_temp_var();void*_tmp9F3=vd->type;Cyc_Absyn_new_vardecl(0U,_tmp9F4,_tmp9F3,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp5DF;
({struct Cyc_Absyn_Exp*_tmp9F5=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0->tag=4U,_tmp5E0->f1=new_vd;_tmp5E0;}),0U);vd->initializer=_tmp9F5;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmp9FC=({struct Cyc_Absyn_Decl*_tmp9FB=({void*_tmp9F6=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->tag=0U,_tmp5E1->f1=new_vd;_tmp5E1;});Cyc_Absyn_new_decl(_tmp9F6,s->loc);});struct Cyc_Absyn_Stmt*_tmp9FA=({
struct Cyc_Absyn_Decl*_tmp9F9=({void*_tmp9F7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->tag=0U,_tmp5E2->f1=vd;_tmp5E2;});Cyc_Absyn_new_decl(_tmp9F7,s->loc);});struct Cyc_Absyn_Stmt*_tmp9F8=s1;Cyc_Absyn_decl_stmt(_tmp9F9,_tmp9F8,s->loc);});
# 3555
Cyc_Absyn_decl_stmt(_tmp9FB,_tmp9FA,s->loc);})->r;s->r=_tmp9FC;});
# 3558
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3564
({void*_tmp9FD=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmp9FD;});
goto _LL3C;}_LL3C:;}
# 3567
goto _LL2F;}case 3U: _LL34: _tmp5D7=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5D2)->f1;_LL35: {struct Cyc_List_List*vds=_tmp5D7;
# 3571
struct Cyc_List_List*_tmp5E3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp5E3;
if(rvds == 0)
({void*_tmp5E4=0U;({struct _fat_ptr _tmp9FE=({const char*_tmp5E5="empty Letv_d";_tag_fat(_tmp5E5,sizeof(char),13U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp9FE,_tag_fat(_tmp5E4,sizeof(void*),0U));});});
({void*_tmp9FF=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->tag=0U,_tmp5E6->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp5E6;});d->r=_tmp9FF;});
rvds=rvds->tl;
for(0;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp5E7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=0U,_tmp5E8->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp5E8;}),0U);struct Cyc_Absyn_Decl*d2=_tmp5E7;
({void*_tmpA01=({struct Cyc_Absyn_Decl*_tmpA00=d2;Cyc_Absyn_decl_stmt(_tmpA00,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA01;});}
# 3580
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp5D6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5D2)->f1;_LL37: {struct Cyc_Absyn_Fndecl*fd=_tmp5D6;
# 3583
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4U: _LL38: _tmp5D3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5D2)->f1;_tmp5D4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5D2)->f2;_tmp5D5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5D2)->f3;_LL39: {struct Cyc_Absyn_Tvar*tv=_tmp5D3;struct Cyc_Absyn_Vardecl*vd=_tmp5D4;struct Cyc_Absyn_Exp*open_exp_opt=_tmp5D5;
# 3587
struct Cyc_Absyn_Stmt*_tmp5E9=s1;struct Cyc_Absyn_Stmt*body=_tmp5E9;
# 3589
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple29 _tmp5EA=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp66=_tmp5EA;struct _tuple29 _tmp5EB=_stmttmp66;struct Cyc_Absyn_Exp*_tmp5ED;struct _tuple1*_tmp5EC;_LL42: _tmp5EC=_tmp5EB.f1;_tmp5ED=_tmp5EB.f2;_LL43: {struct _tuple1*rh_var=_tmp5EC;struct Cyc_Absyn_Exp*rh_exp=_tmp5ED;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3595
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Absyn_no_regions)
({void*_tmpA05=({struct _tuple1*_tmpA04=x_var;void*_tmpA03=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA02=
Cyc_Absyn_uint_exp(0U,0U);
# 3597
Cyc_Absyn_declare_stmt(_tmpA04,_tmpA03,_tmpA02,body,0U);})->r;s->r=_tmpA05;});else{
# 3599
if((unsigned)open_exp_opt){
# 3601
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA07=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA07,({struct _fat_ptr*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));({struct _fat_ptr _tmpA06=({const char*_tmp5EE="h";_tag_fat(_tmp5EE,sizeof(char),2U);});*_tmp5EF=_tmpA06;});_tmp5EF;}),0U);}),0U);
({void*_tmpA08=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA08;});}}else{
# 3610
({void*_tmpA15=({struct _tuple1*_tmpA14=rh_var;void*_tmpA13=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA12=({struct Cyc_Absyn_Exp*_tmp5F0[1U];({struct Cyc_Absyn_Exp*_tmpA09=
# 3612
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp5F0[0]=_tmpA09;});({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA0A,_tag_fat(_tmp5F0,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3610
Cyc_Absyn_declare_stmt(_tmpA14,_tmpA13,_tmpA12,({
# 3613
struct _tuple1*_tmpA11=x_var;void*_tmpA10=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA0F=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA11,_tmpA10,_tmpA0F,({
struct Cyc_Absyn_Stmt*_tmpA0E=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5F1[1U];_tmp5F1[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA0B=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA0B,_tag_fat(_tmp5F1,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA0E,({
struct Cyc_Absyn_Stmt*_tmpA0D=body;Cyc_Absyn_seq_stmt(_tmpA0D,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5F2[1U];_tmp5F2[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA0C=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA0C,_tag_fat(_tmp5F2,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3610
s->r=_tmpA15;});}}
# 3619
return;}}default: _LL3A: _LL3B:
({void*_tmp5F3=0U;({struct _fat_ptr _tmpA16=({const char*_tmp5F4="bad nested declaration within function";_tag_fat(_tmp5F4,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpA16,_tag_fat(_tmp5F3,sizeof(void*),0U));});});}_LL2F:;}
# 3622
return;}default: _LL21: _tmp5A4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f1;_tmp5A5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f2;_tmp5A6=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A3)->f3;_LL22: {struct Cyc_Absyn_Stmt*body=_tmp5A4;struct Cyc_List_List*scs=_tmp5A5;void*dec_tree=_tmp5A6;
# 3636 "toc.cyc"
struct _tuple29 _tmp5F5=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp67=_tmp5F5;struct _tuple29 _tmp5F6=_stmttmp67;struct Cyc_Absyn_Exp*_tmp5F8;struct _tuple1*_tmp5F7;_LL45: _tmp5F7=_tmp5F6.f1;_tmp5F8=_tmp5F6.f2;_LL46: {struct _tuple1*h_var=_tmp5F7;struct Cyc_Absyn_Exp*h_exp=_tmp5F8;
struct _tuple29 _tmp5F9=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp68=_tmp5F9;struct _tuple29 _tmp5FA=_stmttmp68;struct Cyc_Absyn_Exp*_tmp5FC;struct _tuple1*_tmp5FB;_LL48: _tmp5FB=_tmp5FA.f1;_tmp5FC=_tmp5FA.f2;_LL49: {struct _tuple1*e_var=_tmp5FB;struct Cyc_Absyn_Exp*e_exp=_tmp5FC;
struct _tuple29 _tmp5FD=Cyc_Toc_temp_var_and_exp();struct _tuple29 _stmttmp69=_tmp5FD;struct _tuple29 _tmp5FE=_stmttmp69;struct Cyc_Absyn_Exp*_tmp600;struct _tuple1*_tmp5FF;_LL4B: _tmp5FF=_tmp5FE.f1;_tmp600=_tmp5FE.f2;_LL4C: {struct _tuple1*was_thrown_var=_tmp5FF;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp600;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3643
e_exp->topt=e_typ;{
struct _RegionHandle _tmp601=_new_region("temp");struct _RegionHandle*temp=& _tmp601;_push_region(temp);
# 3646
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp602=({struct Cyc_Absyn_Stmt*_tmpA18=body;Cyc_Absyn_seq_stmt(_tmpA18,
Cyc_Absyn_exp_stmt(({void*_tmp60B=0U;({struct Cyc_Absyn_Exp*_tmpA17=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA17,_tag_fat(_tmp60B,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3647
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp602;
# 3650
struct Cyc_Absyn_Stmt*_tmp603=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->tag=10U,_tmp60A->f1=e_exp,_tmp60A->f2=scs,_tmp60A->f3=dec_tree;_tmp60A;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp603;
# 3652
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3655
struct Cyc_Absyn_Exp*_tmp604=({struct Cyc_Absyn_Exp*_tmp609[1U];({struct Cyc_Absyn_Exp*_tmpA19=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp609[0]=_tmpA19;});({struct Cyc_Absyn_Exp*_tmpA1A=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpA1A,_tag_fat(_tmp609,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3655
struct Cyc_Absyn_Exp*setjmp_call=_tmp604;
# 3657
struct Cyc_Absyn_Stmt*_tmp605=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp608[1U];({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Absyn_address_exp(h_exp,0U);_tmp608[0]=_tmpA1B;});({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA1C,_tag_fat(_tmp608,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
# 3657
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp605;
# 3659
struct Cyc_Absyn_Exp*_tmp606=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp606;
struct Cyc_Absyn_Exp*_tmp607=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp607;
({void*_tmpA2C=({
struct _tuple1*_tmpA2B=h_var;void*_tmpA2A=h_typ;Cyc_Absyn_declare_stmt(_tmpA2B,_tmpA2A,0,({
struct Cyc_Absyn_Stmt*_tmpA29=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpA29,({
struct _tuple1*_tmpA28=was_thrown_var;void*_tmpA27=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpA26=zero_exp;Cyc_Absyn_declare_stmt(_tmpA28,_tmpA27,_tmpA26,({
struct Cyc_Absyn_Stmt*_tmpA25=({struct Cyc_Absyn_Exp*_tmpA1E=setjmp_call;struct Cyc_Absyn_Stmt*_tmpA1D=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3665
Cyc_Absyn_ifthenelse_stmt(_tmpA1E,_tmpA1D,
# 3667
Cyc_Toc_skip_stmt_dl(),0U);});
# 3665
Cyc_Absyn_seq_stmt(_tmpA25,({
# 3668
struct Cyc_Absyn_Exp*_tmpA24=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpA23=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpA24,_tmpA23,({
# 3670
struct _tuple1*_tmpA22=e_var;void*_tmpA21=e_typ;struct Cyc_Absyn_Exp*_tmpA20=({
void*_tmpA1F=e_typ;Cyc_Toc_cast_it(_tmpA1F,Cyc_Toc_get_exn_thrown_expression());});
# 3670
Cyc_Absyn_declare_stmt(_tmpA22,_tmpA21,_tmpA20,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3661
s->r=_tmpA2C;});}}
# 3674
_npop_handler(0U);return;
# 3644
;_pop_region(temp);}}}}}}_LL0:;}}
# 3683
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpA2D=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpA2D;});
({void*_tmpA2E=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpA2E;});{
struct _RegionHandle _tmp60C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp60C;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp60D=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp60D;
{struct Cyc_List_List*_tmp60E=(f->i).args;struct Cyc_List_List*args=_tmp60E;for(0;args != 0;args=args->tl){
struct _tuple1*_tmp60F=({struct _tuple1*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->f1=Cyc_Absyn_Loc_n,_tmp610->f2=(*((struct _tuple9*)args->hd)).f1;_tmp610;});struct _tuple1*x=_tmp60F;
({void*_tmpA2F=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpA2F;});}}
# 3700
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3704
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp611=_cycalloc(sizeof(*_tmp611));({struct Cyc_Hashtable_Table*_tmpA30=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp611=_tmpA30;});_tmp611;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp612=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6A=_tmp612;struct Cyc_Absyn_VarargInfo _tmp613=_stmttmp6A;int _tmp617;void*_tmp616;struct Cyc_Absyn_Tqual _tmp615;struct _fat_ptr*_tmp614;_LL1: _tmp614=_tmp613.name;_tmp615=_tmp613.tq;_tmp616=_tmp613.type;_tmp617=_tmp613.inject;_LL2: {struct _fat_ptr*n=_tmp614;struct Cyc_Absyn_Tqual tq=_tmp615;void*t=_tmp616;int i=_tmp617;
void*_tmp618=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type));void*t2=_tmp618;
struct _tuple1*_tmp619=({struct _tuple1*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->f1=Cyc_Absyn_Loc_n,_tmp61C->f2=(struct _fat_ptr*)_check_null(n);_tmp61C;});struct _tuple1*x2=_tmp619;
({struct Cyc_List_List*_tmpA33=({struct Cyc_List_List*_tmpA32=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA32,({struct Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));({struct _tuple9*_tmpA31=({struct _tuple9*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->f1=n,_tmp61A->f2=tq,_tmp61A->f3=t2;_tmp61A;});_tmp61B->hd=_tmpA31;}),_tmp61B->tl=0;_tmp61B;}));});(f->i).args=_tmpA33;});
(f->i).cyc_varargs=0;}}
# 3713
{struct Cyc_List_List*_tmp61D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp61D;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpA34=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpA34;});}}
({struct Cyc_Toc_Env*_tmpA35=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpA35,f->body);});}
# 3693
;_pop_region(frgn);}}
# 3718
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp61E=s;switch(_tmp61E){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3734 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3736
struct _tuple40 _tmp61F=*env;struct _tuple40 _stmttmp6B=_tmp61F;struct _tuple40 _tmp620=_stmttmp6B;struct Cyc_Absyn_Aggrdecl*_tmp622;struct Cyc_Toc_TocState*_tmp621;_LL1: _tmp621=_tmp620.f1;_tmp622=*_tmp620.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp621;struct Cyc_Absyn_Aggrdecl*ad=_tmp622;
struct _tuple1*_tmp623=ad->name;struct _tuple1*n=_tmp623;
struct Cyc_Toc_TocState _tmp624=*s;struct Cyc_Toc_TocState _stmttmp6C=_tmp624;struct Cyc_Toc_TocState _tmp625=_stmttmp6C;struct Cyc_Dict_Dict*_tmp626;_LL4: _tmp626=_tmp625.aggrs_so_far;_LL5: {struct Cyc_Dict_Dict*aggrs_so_far=_tmp626;
int seen_defn_before;
struct _tuple18**_tmp627=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple18**dopt=_tmp627;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)ad->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp628=_region_malloc(d,sizeof(*_tmp628));_tmp628->f1=ad,({void*_tmpA36=Cyc_Absyn_strctq(n);_tmp628->f2=_tmpA36;});_tmp628;});else{
# 3747
v=({struct _tuple18*_tmp629=_region_malloc(d,sizeof(*_tmp629));_tmp629->f1=ad,({void*_tmpA37=Cyc_Absyn_unionq_type(n);_tmp629->f2=_tmpA37;});_tmp629;});}
({struct Cyc_Dict_Dict _tmpA38=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpA38;});}}else{
# 3750
struct _tuple18*_tmp62A=*dopt;struct _tuple18*_stmttmp6D=_tmp62A;struct _tuple18*_tmp62B=_stmttmp6D;void*_tmp62D;struct Cyc_Absyn_Aggrdecl*_tmp62C;_LL7: _tmp62C=_tmp62B->f1;_tmp62D=_tmp62B->f2;_LL8: {struct Cyc_Absyn_Aggrdecl*ad2=_tmp62C;void*t=_tmp62D;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpA3B=({struct Cyc_Dict_Dict _tmpA3A=*aggrs_so_far;struct _tuple1*_tmpA39=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpA3A,_tmpA39,({struct _tuple18*_tmp62E=_region_malloc(d,sizeof(*_tmp62E));_tmp62E->f1=ad,_tmp62E->f2=t;_tmp62E;}));});*aggrs_so_far=_tmpA3B;});
seen_defn_before=0;}else{
# 3755
seen_defn_before=1;}}}{
# 3757
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->kind=ad->kind,_tmp653->sc=Cyc_Absyn_Public,_tmp653->name=ad->name,_tmp653->tvs=0,_tmp653->impl=0,_tmp653->expected_mem_kind=0,_tmp653->attributes=ad->attributes;_tmp653;});
# 3764
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpA3C=({struct Cyc_Absyn_AggrdeclImpl*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->exist_vars=0,_tmp62F->rgn_po=0,_tmp62F->fields=0,_tmp62F->tagged=0;_tmp62F;});new_ad->impl=_tmpA3C;});{
# 3769
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp630=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp630;for(0;fields != 0;fields=fields->tl){
# 3773
struct Cyc_Absyn_Aggrfield*_tmp631=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp631;
void*_tmp632=old_field->type;void*old_type=_tmp632;
struct Cyc_List_List*_tmp633=old_field->attributes;struct Cyc_List_List*old_atts=_tmp633;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3787 "toc.cyc"
void*_tmp634=Cyc_Tcutil_compress(old_type);void*_stmttmp6E=_tmp634;void*_tmp635=_stmttmp6E;unsigned _tmp639;void*_tmp638;struct Cyc_Absyn_Tqual _tmp637;void*_tmp636;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp635)->tag == 4U){_LLA: _tmp636=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp635)->f1).elt_type;_tmp637=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp635)->f1).tq;_tmp638=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp635)->f1).zero_term;_tmp639=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp635)->f1).zt_loc;_LLB: {void*et=_tmp636;struct Cyc_Absyn_Tqual tq=_tmp637;void*zt=_tmp638;unsigned ztl=_tmp639;
# 3790
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->tag=4U,(_tmp63A->f1).elt_type=et,(_tmp63A->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Absyn_uint_exp(0U,0U);(_tmp63A->f1).num_elts=_tmpA3D;}),(_tmp63A->f1).zero_term=zt,(_tmp63A->f1).zt_loc=ztl;_tmp63A;});
goto _LL9;}}else{_LLC: _LLD:
# 3793
 old_atts=({struct Cyc_List_List*_tmp63C=_cycalloc(sizeof(*_tmp63C));({void*_tmpA3E=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tag=6U,_tmp63B->f1=0;_tmp63B;});_tmp63C->hd=_tmpA3E;}),_tmp63C->tl=old_atts;_tmp63C;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->tag=4U,({void*_tmpA41=Cyc_Toc_void_star_type();(_tmp63D->f1).elt_type=_tmpA41;}),({
struct Cyc_Absyn_Tqual _tmpA40=Cyc_Absyn_empty_tqual(0U);(_tmp63D->f1).tq=_tmpA40;}),({
struct Cyc_Absyn_Exp*_tmpA3F=Cyc_Absyn_uint_exp(0U,0U);(_tmp63D->f1).num_elts=_tmpA3F;}),(_tmp63D->f1).zero_term=Cyc_Absyn_false_type,(_tmp63D->f1).zt_loc=0U;_tmp63D;});}_LL9:;}{
# 3800
struct Cyc_Absyn_Aggrfield*_tmp63E=({struct Cyc_Absyn_Aggrfield*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->name=old_field->name,_tmp64F->tq=Cyc_Toc_mt_tq,({
# 3802
void*_tmpA42=Cyc_Toc_typ_to_c(old_type);_tmp64F->type=_tmpA42;}),_tmp64F->width=old_field->width,_tmp64F->attributes=old_atts,_tmp64F->requires_clause=0;_tmp64F;});
# 3800
struct Cyc_Absyn_Aggrfield*new_field=_tmp63E;
# 3810
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp63F=new_field->type;void*T=_tmp63F;
struct _fat_ptr*_tmp640=new_field->name;struct _fat_ptr*f=_tmp640;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp64C=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,_tmp76D.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp76D;});struct Cyc_String_pa_PrintArg_struct _tmp64D=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,_tmp76C.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp76C;});void*_tmp64A[2U];_tmp64A[0]=& _tmp64C,_tmp64A[1]=& _tmp64D;({struct _fat_ptr _tmpA43=({const char*_tmp64B="_union_%s_%s";_tag_fat(_tmp64B,sizeof(char),13U);});Cyc_aprintf(_tmpA43,_tag_fat(_tmp64A,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp649=_cycalloc(sizeof(*_tmp649));*_tmp649=s;_tmp649;});
struct Cyc_Absyn_Aggrfield*_tmp641=({struct Cyc_Absyn_Aggrfield*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->name=Cyc_Toc_val_sp,_tmp648->tq=Cyc_Toc_mt_tq,_tmp648->type=T,_tmp648->width=0,_tmp648->attributes=0,_tmp648->requires_clause=0;_tmp648;});struct Cyc_Absyn_Aggrfield*value_field=_tmp641;
struct Cyc_Absyn_Aggrfield*_tmp642=({struct Cyc_Absyn_Aggrfield*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->name=Cyc_Toc_tag_sp,_tmp647->tq=Cyc_Toc_mt_tq,_tmp647->type=Cyc_Absyn_sint_type,_tmp647->width=0,_tmp647->attributes=0,_tmp647->requires_clause=0;_tmp647;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp642;
struct Cyc_Absyn_Aggrdecl*_tmp643=({struct _fat_ptr*_tmpA44=str;Cyc_Toc_make_c_struct_defn(_tmpA44,({struct Cyc_Absyn_Aggrfield*_tmp646[2U];_tmp646[0]=tag_field,_tmp646[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp646,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp643;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp645=_cycalloc(sizeof(*_tmp645));({struct Cyc_Absyn_Decl*_tmpA45=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->tag=5U,_tmp644->f1=ad2;_tmp644;}),0U);_tmp645->hd=_tmpA45;}),_tmp645->tl=Cyc_Toc_result_decls;_tmp645;});
({void*_tmpA46=Cyc_Absyn_strct(str);new_field->type=_tmpA46;});}
# 3821
new_fields=({struct Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->hd=new_field,_tmp64E->tl=new_fields;_tmp64E;});}}}
# 3823
({struct Cyc_List_List*_tmpA47=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpA47;});}}
# 3826
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp652=_cycalloc(sizeof(*_tmp652));({struct Cyc_Absyn_Decl*_tmpA49=({struct Cyc_Absyn_Decl*_tmp651=_cycalloc(sizeof(*_tmp651));({void*_tmpA48=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->tag=5U,_tmp650->f1=new_ad;_tmp650;});_tmp651->r=_tmpA48;}),_tmp651->loc=0U;_tmp651;});_tmp652->hd=_tmpA49;}),_tmp652->tl=Cyc_Toc_result_decls;_tmp652;});
return 0;}}}}
# 3830
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3858 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 3861
struct _tuple41 _tmp654=*env;struct _tuple41 _stmttmp6F=_tmp654;struct _tuple41 _tmp655=_stmttmp6F;struct Cyc_Absyn_Datatypedecl*_tmp657;struct Cyc_Set_Set**_tmp656;_LL1: _tmp656=(_tmp655.f1)->datatypes_so_far;_tmp657=_tmp655.f2;_LL2: {struct Cyc_Set_Set**datatypes_so_far=_tmp656;struct Cyc_Absyn_Datatypedecl*tud=_tmp657;
struct _tuple1*_tmp658=tud->name;struct _tuple1*n=_tmp658;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpA4A=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpA4A;});
{struct Cyc_List_List*_tmp659=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp659;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3869
struct Cyc_List_List*_tmp65A=0;struct Cyc_List_List*fs=_tmp65A;
int i=1;
{struct Cyc_List_List*_tmp65B=f->typs;struct Cyc_List_List*ts=_tmp65B;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp65C=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp65C;
struct Cyc_Absyn_Aggrfield*_tmp65D=({struct Cyc_Absyn_Aggrfield*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->name=fname,_tmp65F->tq=(*((struct _tuple12*)ts->hd)).f1,({
void*_tmpA4B=Cyc_Toc_typ_to_c((*((struct _tuple12*)ts->hd)).f2);_tmp65F->type=_tmpA4B;}),_tmp65F->width=0,_tmp65F->attributes=0,_tmp65F->requires_clause=0;_tmp65F;});
# 3873
struct Cyc_Absyn_Aggrfield*f=_tmp65D;
# 3875
fs=({struct Cyc_List_List*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->hd=f,_tmp65E->tl=fs;_tmp65E;});}}
# 3877
fs=({struct Cyc_List_List*_tmp661=_cycalloc(sizeof(*_tmp661));({struct Cyc_Absyn_Aggrfield*_tmpA4D=({struct Cyc_Absyn_Aggrfield*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->name=Cyc_Toc_tag_sp,_tmp660->tq=Cyc_Toc_mt_tq,_tmp660->type=Cyc_Absyn_sint_type,_tmp660->width=0,_tmp660->attributes=0,_tmp660->requires_clause=0;_tmp660;});_tmp661->hd=_tmpA4D;}),({
struct Cyc_List_List*_tmpA4C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);_tmp661->tl=_tmpA4C;});_tmp661;});{
struct Cyc_Absyn_Aggrdecl*_tmp662=({struct _fat_ptr*_tmpA4F=({struct _fat_ptr*_tmp666=_cycalloc(sizeof(*_tmp666));({struct _fat_ptr _tmpA4E=({const char*_tmp665="";_tag_fat(_tmp665,sizeof(char),1U);});*_tmp666=_tmpA4E;});_tmp666;});Cyc_Toc_make_c_struct_defn(_tmpA4F,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp662;
({struct _tuple1*_tmpA50=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpA50;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp664=_cycalloc(sizeof(*_tmp664));({struct Cyc_Absyn_Decl*_tmpA51=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->tag=5U,_tmp663->f1=ad;_tmp663;}),0U);_tmp664->hd=_tmpA51;}),_tmp664->tl=Cyc_Toc_result_decls;_tmp664;});}}}
# 3883
return 0;}}
# 3886
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3905 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 3908
struct _tuple41 _tmp667=*env;struct _tuple41 _stmttmp70=_tmp667;struct _tuple41 _tmp668=_stmttmp70;struct Cyc_Absyn_Datatypedecl*_tmp66A;struct Cyc_Toc_TocState*_tmp669;_LL1: _tmp669=_tmp668.f1;_tmp66A=_tmp668.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp669;struct Cyc_Absyn_Datatypedecl*xd=_tmp66A;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp66B=*s;struct Cyc_Toc_TocState _stmttmp71=_tmp66B;struct Cyc_Toc_TocState _tmp66C=_stmttmp71;struct Cyc_Dict_Dict*_tmp66D;_LL4: _tmp66D=_tmp66C.xdatatypes_so_far;_LL5: {struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp66D;
struct _tuple1*_tmp66E=xd->name;struct _tuple1*n=_tmp66E;
{struct Cyc_List_List*_tmp66F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp66F;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp670=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp670;
void*_tmp671=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp671;
# 3919
int*_tmp672=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp72=_tmp672;int*_tmp673=_stmttmp72;if(_tmp673 == 0){_LL7: _LL8: {
# 3921
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3925
struct Cyc_Absyn_Vardecl*_tmp674=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp674;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp676=_cycalloc(sizeof(*_tmp676));({struct Cyc_Absyn_Decl*_tmpA52=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=0U,_tmp675->f1=tag_decl;_tmp675;}),0U);_tmp676->hd=_tmpA52;}),_tmp676->tl=Cyc_Toc_result_decls;_tmp676;});
({struct Cyc_Dict_Dict _tmpA53=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3928
*xdatatypes_so_far=_tmpA53;});{
# 3930
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp677=f->typs;struct Cyc_List_List*tqts=_tmp677;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp678=({struct _fat_ptr*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct _fat_ptr _tmpA55=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp67E=({struct Cyc_Int_pa_PrintArg_struct _tmp76E;_tmp76E.tag=1U,_tmp76E.f1=(unsigned long)i;_tmp76E;});void*_tmp67C[1U];_tmp67C[0]=& _tmp67E;({struct _fat_ptr _tmpA54=({const char*_tmp67D="f%d";_tag_fat(_tmp67D,sizeof(char),4U);});Cyc_aprintf(_tmpA54,_tag_fat(_tmp67C,sizeof(void*),1U));});});*_tmp67F=_tmpA55;});_tmp67F;});struct _fat_ptr*field_n=_tmp678;
struct Cyc_Absyn_Aggrfield*_tmp679=({struct Cyc_Absyn_Aggrfield*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->name=field_n,_tmp67B->tq=(*((struct _tuple12*)tqts->hd)).f1,({
void*_tmpA56=Cyc_Toc_typ_to_c((*((struct _tuple12*)tqts->hd)).f2);_tmp67B->type=_tmpA56;}),_tmp67B->width=0,_tmp67B->attributes=0,_tmp67B->requires_clause=0;_tmp67B;});
# 3934
struct Cyc_Absyn_Aggrfield*newf=_tmp679;
# 3936
fields=({struct Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->hd=newf,_tmp67A->tl=fields;_tmp67A;});}}
# 3938
fields=({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));({struct Cyc_Absyn_Aggrfield*_tmpA59=({struct Cyc_Absyn_Aggrfield*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680->name=Cyc_Toc_tag_sp,_tmp680->tq=Cyc_Toc_mt_tq,({
void*_tmpA58=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp680->type=_tmpA58;}),_tmp680->width=0,_tmp680->attributes=0,_tmp680->requires_clause=0;_tmp680;});
# 3938
_tmp681->hd=_tmpA59;}),({
# 3940
struct Cyc_List_List*_tmpA57=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp681->tl=_tmpA57;});_tmp681;});{
struct Cyc_Absyn_Aggrdecl*_tmp682=({struct _fat_ptr*_tmpA5B=({struct _fat_ptr*_tmp686=_cycalloc(sizeof(*_tmp686));({struct _fat_ptr _tmpA5A=({const char*_tmp685="";_tag_fat(_tmp685,sizeof(char),1U);});*_tmp686=_tmpA5A;});_tmp686;});Cyc_Toc_make_c_struct_defn(_tmpA5B,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp682;
({struct _tuple1*_tmpA5C=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpA5C;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp684=_cycalloc(sizeof(*_tmp684));({struct Cyc_Absyn_Decl*_tmpA5D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->tag=5U,_tmp683->f1=strct_decl;_tmp683;}),0U);_tmp684->hd=_tmpA5D;}),_tmp684->tl=Cyc_Toc_result_decls;_tmp684;});
goto _LL6;}}}}}else{if(*((int*)_tmp673)== 0){_LL9: _LLA:
# 3946
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp687=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp687;
struct Cyc_Absyn_Vardecl*_tmp688=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp688;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp68A=_cycalloc(sizeof(*_tmp68A));({struct Cyc_Absyn_Decl*_tmpA5E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->tag=0U,_tmp689->f1=tag_decl;_tmp689;}),0U);_tmp68A->hd=_tmpA5E;}),_tmp68A->tl=Cyc_Toc_result_decls;_tmp68A;});
({struct Cyc_Dict_Dict _tmpA5F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpA5F;});}
# 3953
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3957
return 0;}}}}
# 3960
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3964
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3970
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpA60=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpA60;});
# 3974
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3983
struct _RegionHandle _tmp68B=_new_region("temp");struct _RegionHandle*temp=& _tmp68B;_push_region(temp);
{struct Cyc_Toc_Env*_tmp68C=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp68C;
Cyc_Toc_exp_to_c(nv2,init);}
# 3984
;_pop_region(temp);}else{
# 3988
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3991
void*_tmp68D=Cyc_Tcutil_compress(old_typ);void*_stmttmp73=_tmp68D;void*_tmp68E=_stmttmp73;void*_tmp691;struct Cyc_Absyn_Exp*_tmp690;void*_tmp68F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68E)->tag == 4U){_LL1: _tmp68F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68E)->f1).elt_type;_tmp690=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68E)->f1).num_elts;_tmp691=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68E)->f1).zero_term;_LL2: {void*et=_tmp68F;struct Cyc_Absyn_Exp*num_elts_opt=_tmp690;void*zt=_tmp691;
# 3993
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({void*_tmp692=0U;({struct _fat_ptr _tmpA61=({const char*_tmp693="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp693,sizeof(char),55U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA61,_tag_fat(_tmp692,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=num_elts_opt;
struct Cyc_Absyn_Exp*_tmp694=({struct Cyc_Absyn_Exp*_tmpA63=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpA63,({
struct Cyc_Absyn_Exp*_tmpA62=num_elts;Cyc_Absyn_add_exp(_tmpA62,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3997
struct Cyc_Absyn_Exp*lhs=_tmp694;
# 4000
struct Cyc_Absyn_Exp*_tmp695=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp695;
({void*_tmpA65=({struct Cyc_Absyn_Stmt*_tmpA64=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpA64,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4001
s->r=_tmpA65;});}}
# 4004
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4012
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp696=d;void**_tmp699;struct Cyc_List_List*_tmp698;struct Cyc_List_List*_tmp697;struct Cyc_Tcpat_Rhs*_tmp69A;switch(*((int*)_tmp696)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp69A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp696)->f1;_LL4: {struct Cyc_Tcpat_Rhs*rhs=_tmp69A;
rhs->rhs=success;return d;}default: _LL5: _tmp697=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp696)->f1;_tmp698=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp696)->f2;_tmp699=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp696)->f3;_LL6: {struct Cyc_List_List*path=_tmp697;struct Cyc_List_List*sws=_tmp698;void**d2=_tmp699;
# 4017
({void*_tmpA66=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpA66;});
for(0;sws != 0;sws=sws->tl){
struct _tuple32*_tmp69B=(struct _tuple32*)sws->hd;struct _tuple32*_stmttmp74=_tmp69B;struct _tuple32*_tmp69C=_stmttmp74;void**_tmp69D;_LL8: _tmp69D=(void**)& _tmp69C->f2;_LL9: {void**d2=_tmp69D;
({void*_tmpA67=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpA67;});}}
# 4022
return d;}}_LL0:;}
# 4033 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4035
struct _RegionHandle _tmp69E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp69E;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp69F=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp69F;
void*_tmp6A0=(void*)_check_null(e->topt);void*t=_tmp6A0;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4045
struct Cyc_Absyn_Stmt*_tmp6A1=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6A1;
if(dopt != 0)
# 4049
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4051
struct Cyc_Absyn_Switch_clause*_tmp6A2=({struct Cyc_Absyn_Switch_clause*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->pattern=p,_tmp6B0->pat_vars=0,_tmp6B0->where_clause=0,_tmp6B0->body=succ_stmt,_tmp6B0->loc=0U;_tmp6B0;});struct Cyc_Absyn_Switch_clause*c1=_tmp6A2;
struct Cyc_List_List*_tmp6A3=({struct _RegionHandle*_tmpA6A=rgn;struct _RegionHandle*_tmpA69=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpA6A,Cyc_Toc_gen_labels,_tmpA69,({struct Cyc_Absyn_Switch_clause*_tmp6AF[1U];_tmp6AF[0]=c1;({struct _RegionHandle*_tmpA68=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpA68,_tag_fat(_tmp6AF,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});struct Cyc_List_List*lscs=_tmp6A3;
# 4054
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4058
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4061
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple39*_tmp6A4=(struct _tuple39*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple39*_stmttmp75=_tmp6A4;struct _tuple39*_tmp6A5=_stmttmp75;struct Cyc_Absyn_Stmt*_tmp6A7;struct Cyc_Toc_Env*_tmp6A6;_LL1: _tmp6A6=_tmp6A5->f1;_tmp6A7=_tmp6A5->f3;_LL2: {struct Cyc_Toc_Env*env=_tmp6A6;struct Cyc_Absyn_Stmt*st=_tmp6A7;
if(st == succ_stmt){senv=env;goto FOUND_ENV;}}}
# 4066
({void*_tmp6A8=0U;({struct _fat_ptr _tmpA6B=({const char*_tmp6A9="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6A9,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA6B,_tag_fat(_tmp6A8,sizeof(void*),0U));});});
FOUND_ENV:
# 4070
 Cyc_Toc_stmt_to_c(senv,s);{
# 4072
struct Cyc_Absyn_Stmt*res=test_tree;
# 4074
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple37 _tmp6AA=*((struct _tuple37*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple37 _stmttmp76=_tmp6AA;struct _tuple37 _tmp6AB=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp6AC;_LL4: _tmp6AC=_tmp6AB.f2;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp6AC;
res=({struct Cyc_Absyn_Decl*_tmpA6C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tag=0U,_tmp6AD->f1=vd;_tmp6AD;}),0U);Cyc_Absyn_decl_stmt(_tmpA6C,res,0U);});}}
# 4079
res=({struct _tuple1*_tmpA6F=v;void*_tmpA6E=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpA6D=e;Cyc_Absyn_declare_stmt(_tmpA6F,_tmpA6E,_tmpA6D,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6AE=res;_npop_handler(0U);return _tmp6AE;}}}}}
# 4036
;_pop_region(rgn);}
# 4087
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6B1=e->r;void*_stmttmp77=_tmp6B1;void*_tmp6B2=_stmttmp77;struct Cyc_Absyn_MallocInfo*_tmp6B3;struct Cyc_Absyn_Stmt*_tmp6B4;void**_tmp6B5;void**_tmp6B6;struct Cyc_List_List*_tmp6B7;struct Cyc_List_List*_tmp6B8;struct Cyc_List_List*_tmp6BA;void**_tmp6B9;struct Cyc_Absyn_Exp*_tmp6BC;void**_tmp6BB;struct Cyc_List_List*_tmp6BE;struct Cyc_Absyn_Exp*_tmp6BD;struct Cyc_Absyn_Exp*_tmp6C1;struct Cyc_Absyn_Exp*_tmp6C0;struct Cyc_Absyn_Exp*_tmp6BF;struct Cyc_Absyn_Exp*_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C4;struct Cyc_Absyn_Exp*_tmp6C7;struct Cyc_Absyn_Exp*_tmp6C6;struct Cyc_Absyn_Exp*_tmp6C9;struct Cyc_Absyn_Exp*_tmp6C8;struct Cyc_Absyn_Exp*_tmp6CB;struct Cyc_Absyn_Exp*_tmp6CA;struct Cyc_Absyn_Exp*_tmp6CD;struct Cyc_Absyn_Exp*_tmp6CC;struct Cyc_List_List*_tmp6CE;struct Cyc_Absyn_Exp*_tmp6CF;struct Cyc_Absyn_Exp*_tmp6D0;struct Cyc_Absyn_Exp*_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D2;struct Cyc_Absyn_Exp*_tmp6D3;struct Cyc_Absyn_Exp*_tmp6D4;struct Cyc_Absyn_Exp*_tmp6D5;struct Cyc_Absyn_Exp*_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D7;switch(*((int*)_tmp6B2)){case 41U: _LL1: _tmp6D7=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp6D7;
_tmp6D6=e;goto _LL4;}case 20U: _LL3: _tmp6D6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp6D6;
_tmp6D5=e;goto _LL6;}case 21U: _LL5: _tmp6D5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp6D5;
_tmp6D4=e;goto _LL8;}case 22U: _LL7: _tmp6D4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp6D4;
_tmp6D3=e;goto _LLA;}case 15U: _LL9: _tmp6D3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6D3;
_tmp6D2=e;goto _LLC;}case 11U: _LLB: _tmp6D2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp6D2;
_tmp6D1=e;goto _LLE;}case 12U: _LLD: _tmp6D1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp6D1;
_tmp6D0=e;goto _LL10;}case 18U: _LLF: _tmp6D0=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp6D0;
_tmp6CF=e;goto _LL12;}case 5U: _LL11: _tmp6CF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp6CF;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3U: _LL13: _tmp6CE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL14: {struct Cyc_List_List*es=_tmp6CE;
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7U: _LL15: _tmp6CC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6CD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp6CC;struct Cyc_Absyn_Exp*e2=_tmp6CD;
_tmp6CA=e1;_tmp6CB=e2;goto _LL18;}case 8U: _LL17: _tmp6CA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6CB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp6CA;struct Cyc_Absyn_Exp*e2=_tmp6CB;
_tmp6C8=e1;_tmp6C9=e2;goto _LL1A;}case 9U: _LL19: _tmp6C8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6C9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp6C8;struct Cyc_Absyn_Exp*e2=_tmp6C9;
_tmp6C6=e1;_tmp6C7=e2;goto _LL1C;}case 23U: _LL1B: _tmp6C6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6C7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp6C6;struct Cyc_Absyn_Exp*e2=_tmp6C7;
_tmp6C4=e1;_tmp6C5=e2;goto _LL1E;}case 35U: _LL1D: _tmp6C4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6C5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp6C4;struct Cyc_Absyn_Exp*e2=_tmp6C5;
_tmp6C2=e1;_tmp6C3=e2;goto _LL20;}case 4U: _LL1F: _tmp6C2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6C3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6B2)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp6C2;struct Cyc_Absyn_Exp*e2=_tmp6C3;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6U: _LL21: _tmp6BF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6C0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_tmp6C1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6B2)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp6BF;struct Cyc_Absyn_Exp*e2=_tmp6C0;struct Cyc_Absyn_Exp*e3=_tmp6C1;
# 4106
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10U: _LL23: _tmp6BD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6BE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp6BD;struct Cyc_List_List*es=_tmp6BE;
# 4108
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14U: _LL25: _tmp6BB=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_tmp6BC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL26: {void**t=_tmp6BB;struct Cyc_Absyn_Exp*e=_tmp6BC;
({void*_tmpA70=Cyc_Toc_typ_to_c(*t);*t=_tmpA70;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25U: _LL27: _tmp6B9=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1)->f3;_tmp6BA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL28: {void**t=_tmp6B9;struct Cyc_List_List*dles=_tmp6BA;
# 4111
({void*_tmpA71=Cyc_Toc_typ_to_c(*t);*t=_tmpA71;});
_tmp6B8=dles;goto _LL2A;}case 36U: _LL29: _tmp6B8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp6B8;
_tmp6B7=dles;goto _LL2C;}case 26U: _LL2B: _tmp6B7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp6B7;
# 4115
for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19U: _LL2D: _tmp6B6=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL2E: {void**t=_tmp6B6;
_tmp6B5=t;goto _LL30;}case 17U: _LL2F: _tmp6B5=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL30: {void**t=_tmp6B5;
({void*_tmpA72=Cyc_Toc_typ_to_c(*t);*t=_tmpA72;});goto _LL0;}case 37U: _LL31: _tmp6B4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL32: {struct Cyc_Absyn_Stmt*s=_tmp6B4;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34U: _LL33: _tmp6B3=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL34: {struct Cyc_Absyn_MallocInfo*m=_tmp6B3;
# 4122
if(m->elt_type != 0)
({void**_tmpA74=({void**_tmp6D8=_cycalloc(sizeof(*_tmp6D8));({void*_tmpA73=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp6D8=_tmpA73;});_tmp6D8;});m->elt_type=_tmpA74;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4132
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
({void*_tmp6D9=0U;({unsigned _tmpA76=e->loc;struct _fat_ptr _tmpA75=({const char*_tmp6DA="Cyclone expression in C code";_tag_fat(_tmp6DA,sizeof(char),29U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmpA76,_tmpA75,_tag_fat(_tmp6D9,sizeof(void*),0U));});});}_LL0:;}
# 4146
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp6DB=d->r;void*_stmttmp78=_tmp6DB;void*_tmp6DC=_stmttmp78;struct Cyc_Absyn_Typedefdecl*_tmp6DD;struct Cyc_Absyn_Enumdecl*_tmp6DE;struct Cyc_Absyn_Aggrdecl*_tmp6DF;struct Cyc_Absyn_Fndecl*_tmp6E0;struct Cyc_Absyn_Vardecl*_tmp6E1;switch(*((int*)_tmp6DC)){case 0U: _LL1: _tmp6E1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6DC)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp6E1;
# 4149
({void*_tmpA77=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpA77;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp6E0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6DC)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp6E0;
# 4153
({void*_tmpA78=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpA78;});
{struct Cyc_List_List*_tmp6E2=(fd->i).args;struct Cyc_List_List*args=_tmp6E2;for(0;args != 0;args=args->tl){
({void*_tmpA79=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpA79;});}}
goto _LL0;}case 5U: _LL5: _tmp6DF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6DC)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp6DF;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7U: _LL7: _tmp6DE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6DC)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp6DE;
# 4159
if(ed->fields != 0){
struct Cyc_List_List*_tmp6E3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp6E3;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp6E4=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp6E4;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4164
goto _LL0;}case 8U: _LL9: _tmp6DD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6DC)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp6DD;
({void*_tmpA7A=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpA7A;});goto _LL0;}case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4174
({void*_tmp6E5=0U;({unsigned _tmpA7C=d->loc;struct _fat_ptr _tmpA7B=({const char*_tmp6E6="Cyclone declaration in C code";_tag_fat(_tmp6E6,sizeof(char),30U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmpA7C,_tmpA7B,_tag_fat(_tmp6E5,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4182
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp6E7=s->r;void*_stmttmp79=_tmp6E7;void*_tmp6E8=_stmttmp79;struct Cyc_Absyn_Stmt*_tmp6E9;struct Cyc_Absyn_Exp*_tmp6EB;struct Cyc_Absyn_Stmt*_tmp6EA;struct Cyc_Absyn_Stmt*_tmp6ED;struct Cyc_Absyn_Decl*_tmp6EC;void*_tmp6F0;struct Cyc_List_List*_tmp6EF;struct Cyc_Absyn_Exp*_tmp6EE;struct Cyc_Absyn_Stmt*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F1;struct Cyc_Absyn_Stmt*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_Absyn_Stmt*_tmp6F9;struct Cyc_Absyn_Stmt*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6FA;struct Cyc_Absyn_Stmt*_tmp6FC;struct Cyc_Absyn_Stmt*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FD;switch(*((int*)_tmp6E8)){case 1U: _LL1: _tmp6FD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp6FD;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2U: _LL3: _tmp6FB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_tmp6FC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp6FB;struct Cyc_Absyn_Stmt*s2=_tmp6FC;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3U: _LL5: _tmp6FA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_LL6: {struct Cyc_Absyn_Exp*eopt=_tmp6FA;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4U: _LL7: _tmp6F7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_tmp6F8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2;_tmp6F9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp6F7;struct Cyc_Absyn_Stmt*s1=_tmp6F8;struct Cyc_Absyn_Stmt*s2=_tmp6F9;
# 4188
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5U: _LL9: _tmp6F5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1).f1;_tmp6F6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6F5;struct Cyc_Absyn_Stmt*s=_tmp6F6;
# 4190
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9U: _LLB: _tmp6F1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_tmp6F2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2).f1;_tmp6F3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f3).f1;_tmp6F4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f4;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp6F1;struct Cyc_Absyn_Exp*e2=_tmp6F2;struct Cyc_Absyn_Exp*e3=_tmp6F3;struct Cyc_Absyn_Stmt*s=_tmp6F4;
# 4192
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10U: _LLD: _tmp6EE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_tmp6EF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2;_tmp6F0=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f3;_LLE: {struct Cyc_Absyn_Exp*e=_tmp6EE;struct Cyc_List_List*scs=_tmp6EF;void*dec_tree=_tmp6F0;
# 4195
Cyc_Toc_exptypes_to_c(e);
for(0;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12U: _LLF: _tmp6EC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_tmp6ED=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2;_LL10: {struct Cyc_Absyn_Decl*d=_tmp6EC;struct Cyc_Absyn_Stmt*s=_tmp6ED;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14U: _LL11: _tmp6EA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f1;_tmp6EB=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp6EA;struct Cyc_Absyn_Exp*e=_tmp6EB;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13U: _LL13: _tmp6E9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp6E8)->f2;_LL14: {struct Cyc_Absyn_Stmt*s=_tmp6E9;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4207
({void*_tmpA7D=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->tag=0U;_tmp6FE;});s->r=_tmpA7D;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp6FF=0U;({unsigned _tmpA7F=s->loc;struct _fat_ptr _tmpA7E=({const char*_tmp700="Cyclone statement in C code";_tag_fat(_tmp700,sizeof(char),28U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmpA7F,_tmpA7E,_tag_fat(_tmp6FF,sizeof(void*),0U));});});}_LL0:;}
# 4215
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4217
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp701=0U;({struct _fat_ptr _tmpA80=({const char*_tmp702="decls_to_c: not at toplevel!";_tag_fat(_tmp702,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpA80,_tag_fat(_tmp701,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp703=d->r;void*_stmttmp7A=_tmp703;void*_tmp704=_stmttmp7A;struct Cyc_List_List*_tmp705;struct Cyc_List_List*_tmp706;struct Cyc_List_List*_tmp707;struct Cyc_List_List*_tmp708;struct Cyc_Absyn_Typedefdecl*_tmp709;struct Cyc_Absyn_Enumdecl*_tmp70A;struct Cyc_Absyn_Datatypedecl*_tmp70B;struct Cyc_Absyn_Aggrdecl*_tmp70C;struct Cyc_Absyn_Fndecl*_tmp70D;struct Cyc_Absyn_Vardecl*_tmp70E;switch(*((int*)_tmp704)){case 0U: _LL1: _tmp70E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp70E;
# 4224
struct _tuple1*_tmp70F=vd->name;struct _tuple1*c_name=_tmp70F;
# 4226
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp710=_cycalloc(sizeof(*_tmp710));({union Cyc_Absyn_Nmspace _tmpA81=Cyc_Absyn_Abs_n(0,1);_tmp710->f1=_tmpA81;}),_tmp710->f2=(*c_name).f2;_tmp710;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4233
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4235
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpA82=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpA82;});
({void*_tmpA83=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpA83;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->hd=d,_tmp711->tl=Cyc_Toc_result_decls;_tmp711;});
goto _LL0;}case 1U: _LL3: _tmp70D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp70D;
# 4242
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpA85=({struct _tuple1*_tmp712=_cycalloc(sizeof(*_tmp712));({union Cyc_Absyn_Nmspace _tmpA84=Cyc_Absyn_Abs_n(0,1);_tmp712->f1=_tmpA84;}),_tmp712->f2=(*fd->name).f2;_tmp712;});fd->name=_tmpA85;});
fd->sc=Cyc_Absyn_Public;}
# 4246
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->hd=d,_tmp713->tl=Cyc_Toc_result_decls;_tmp713;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp714=0U;({struct _fat_ptr _tmpA86=({const char*_tmp715="letdecl at toplevel";_tag_fat(_tmp715,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA86,_tag_fat(_tmp714,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp716=0U;({struct _fat_ptr _tmpA87=({const char*_tmp717="region decl at toplevel";_tag_fat(_tmp717,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA87,_tag_fat(_tmp716,sizeof(void*),0U));});});case 5U: _LLB: _tmp70C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*sd=_tmp70C;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6U: _LLD: _tmp70B=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LLE: {struct Cyc_Absyn_Datatypedecl*tud=_tmp70B;
# 4254
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7U: _LLF: _tmp70A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp70A;
# 4257
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718->hd=d,_tmp718->tl=Cyc_Toc_result_decls;_tmp718;});
goto _LL0;}case 8U: _LL11: _tmp709=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp709;
# 4261
td->tvs=0;
if(td->defn != 0){
({void*_tmpA88=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpA88;});{
# 4266
struct Cyc_Absyn_Decl*ed;
{void*_tmp719=td->defn;void*_stmttmp7B=_tmp719;void*_tmp71A=_stmttmp7B;unsigned _tmp71C;struct Cyc_Absyn_Enumdecl*_tmp71B;if(_tmp71A != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71A)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71A)->f1)->r)->tag == 1U){_LL24: _tmp71B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71A)->f1)->r)->f1;_tmp71C=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71A)->f1)->loc;_LL25: {struct Cyc_Absyn_Enumdecl*ed2=_tmp71B;unsigned loc=_tmp71C;
# 4269
ed=({struct Cyc_Absyn_Decl*_tmp71E=_cycalloc(sizeof(*_tmp71E));({void*_tmpA89=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->tag=7U,_tmp71D->f1=ed2;_tmp71D;});_tmp71E->r=_tmpA89;}),_tmp71E->loc=loc;_tmp71E;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4272
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->hd=ed,_tmp71F->tl=Cyc_Toc_result_decls;_tmp71F;});{
void*_tmp720=ed->r;void*_stmttmp7C=_tmp720;void*_tmp721=_stmttmp7C;struct Cyc_Absyn_Enumdecl*_tmp722;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp721)->tag == 7U){_LL29: _tmp722=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp721)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp722;
# 4276
({void*_tmpA8B=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->tag=0U,({void*_tmpA8A=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->tag=15U,_tmp723->f1=ed->name,_tmp723->f2=ed;_tmp723;});_tmp724->f1=_tmpA8A;}),_tmp724->f2=0;_tmp724;});td->defn=_tmpA8B;});goto _LL28;}}else{_LL2B: _LL2C:
({void*_tmp725=0U;({struct _fat_ptr _tmpA8C=({const char*_tmp726="Toc: enumdecl to name";_tag_fat(_tmp726,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpA8C,_tag_fat(_tmp725,sizeof(void*),0U));});});}_LL28:;}}}}else{
# 4281
enum Cyc_Absyn_KindQual _tmp727=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7D=_tmp727;enum Cyc_Absyn_KindQual _tmp728=_stmttmp7D;if(_tmp728 == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpA8D=Cyc_Toc_void_star_type();td->defn=_tmpA8D;});goto _LL2D;}else{_LL30: _LL31:
 td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4289
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729->hd=d,_tmp729->tl=Cyc_Toc_result_decls;_tmp729;});
goto _LL0;}case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp708=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp704)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp708;
_tmp707=ds2;goto _LL1E;}case 10U: _LL1D: _tmp707=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp704)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp707;
_tmp706=ds2;goto _LL20;}case 11U: _LL1F: _tmp706=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp706;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _LL21: _tmp705=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp704)->f1;_LL22: {struct Cyc_List_List*ds2=_tmp705;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4302
return nv;}
# 4306
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp72A=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp7E=_tmp72A;struct Cyc_Core_NewDynamicRegion _tmp72B=_stmttmp7E;struct Cyc_Core_DynamicRegion*_tmp72C;_LL1: _tmp72C=_tmp72B.key;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp72C;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp72E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp72E));_tmp72E->dyn=dyn,_tmp72E->state=ts;_tmp72E;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp730=34;struct _fat_ptr**_tmp72F=_cycalloc(_check_times(_tmp730,sizeof(struct _fat_ptr*)));_tmp72F[0]=& Cyc_Toc__throw_str,_tmp72F[1]=& Cyc_Toc_setjmp_str,_tmp72F[2]=& Cyc_Toc__push_handler_str,_tmp72F[3]=& Cyc_Toc__pop_handler_str,_tmp72F[4]=& Cyc_Toc__exn_thrown_str,_tmp72F[5]=& Cyc_Toc__npop_handler_str,_tmp72F[6]=& Cyc_Toc__check_null_str,_tmp72F[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp72F[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp72F[9]=& Cyc_Toc__check_fat_subscript_str,_tmp72F[10]=& Cyc_Toc__tag_fat_str,_tmp72F[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp72F[12]=& Cyc_Toc__get_fat_size_str,_tmp72F[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp72F[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp72F[15]=& Cyc_Toc__zero_arr_plus_str,_tmp72F[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp72F[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp72F[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp72F[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp72F[20]=& Cyc_Toc__cycalloc_str,_tmp72F[21]=& Cyc_Toc__cyccalloc_str,_tmp72F[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp72F[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp72F[24]=& Cyc_Toc__region_malloc_str,_tmp72F[25]=& Cyc_Toc__region_calloc_str,_tmp72F[26]=& Cyc_Toc__check_times_str,_tmp72F[27]=& Cyc_Toc__new_region_str,_tmp72F[28]=& Cyc_Toc__push_region_str,_tmp72F[29]=& Cyc_Toc__pop_region_str,_tmp72F[30]=& Cyc_Toc__throw_arraybounds_str,_tmp72F[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp72F[32]=& Cyc_Toc__throw_match_str,_tmp72F[33]=& Cyc_Toc__fast_region_malloc_str;_tmp72F;}),sizeof(struct _fat_ptr*),34U);}}
# 4354
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp731=ts;struct Cyc_Toc_TocStateWrap*_tmp732=Cyc_Toc_toc_state;ts=_tmp732;Cyc_Toc_toc_state=_tmp731;});{
struct Cyc_Toc_TocStateWrap _tmp733=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp7F=_tmp733;struct Cyc_Toc_TocStateWrap _tmp734=_stmttmp7F;struct Cyc_Toc_TocState*_tmp736;struct Cyc_Core_DynamicRegion*_tmp735;_LL1: _tmp735=_tmp734.dyn;_tmp736=_tmp734.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp735;struct Cyc_Toc_TocState*s=_tmp736;
# 4359
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp738=*s;struct Cyc_Toc_TocState _stmttmp80=_tmp738;struct Cyc_Toc_TocState _tmp739=_stmttmp80;struct Cyc_Xarray_Xarray*_tmp73A;_LL4: _tmp73A=_tmp739.temp_labels;_LL5: {struct Cyc_Xarray_Xarray*tls=_tmp73A;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(tls);}}
# 4360
;}
# 4363
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4366
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4372
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4374
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp73B=_cycalloc(sizeof(*_tmp73B));*_tmp73B=pop_tables;_tmp73B;});
Cyc_Toc_init();{
struct _RegionHandle _tmp73C=_new_region("start");struct _RegionHandle*start=& _tmp73C;_push_region(start);
({struct _RegionHandle*_tmpA8F=start;struct Cyc_Toc_Env*_tmpA8E=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpA8F,_tmpA8E,ds,0);});{
struct Cyc_List_List*_tmp73D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp73D;}
# 4377
;_pop_region(start);}}
