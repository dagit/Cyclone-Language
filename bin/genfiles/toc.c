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
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
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
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1049
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1054
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1062
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1071
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1075
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1080
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1117
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1120
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1138
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1140
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1142
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1151
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1156
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
({struct _fat_ptr _tmp76E=(struct _fat_ptr)({struct _fat_ptr _tmp76D=({const char*_tmp0="Toc (unimplemented): ";_tag_fat(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp76D,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp76E,ap);});}
# 77
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 79
({struct _fat_ptr _tmp770=(struct _fat_ptr)({struct _fat_ptr _tmp76F=({const char*_tmp1="Toc: ";_tag_fat(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp76F,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp770,ap);});}
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
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 123
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 130
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp9=_region_malloc(d,sizeof(*_tmp9));
({struct Cyc_List_List**_tmp77C=({struct Cyc_List_List**_tmp2=_region_malloc(d,sizeof(*_tmp2));*_tmp2=0;_tmp2;});_tmp9->tuple_types=_tmp77C;}),({
struct Cyc_List_List**_tmp77B=({struct Cyc_List_List**_tmp3=_region_malloc(d,sizeof(*_tmp3));*_tmp3=0;_tmp3;});_tmp9->anon_aggr_types=_tmp77B;}),({
struct Cyc_Dict_Dict*_tmp77A=({struct Cyc_Dict_Dict*_tmp4=_region_malloc(d,sizeof(*_tmp4));({struct Cyc_Dict_Dict _tmp779=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp4=_tmp779;});_tmp4;});_tmp9->aggrs_so_far=_tmp77A;}),({
struct Cyc_List_List**_tmp778=({struct Cyc_List_List**_tmp5=_region_malloc(d,sizeof(*_tmp5));*_tmp5=0;_tmp5;});_tmp9->abs_struct_types=_tmp778;}),({
struct Cyc_Set_Set**_tmp777=({struct Cyc_Set_Set**_tmp6=_region_malloc(d,sizeof(*_tmp6));({struct Cyc_Set_Set*_tmp776=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp6=_tmp776;});_tmp6;});_tmp9->datatypes_so_far=_tmp777;}),({
struct Cyc_Dict_Dict*_tmp775=({struct Cyc_Dict_Dict*_tmp7=_region_malloc(d,sizeof(*_tmp7));({struct Cyc_Dict_Dict _tmp774=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp7=_tmp774;});_tmp7;});_tmp9->xdatatypes_so_far=_tmp775;}),({
struct Cyc_Dict_Dict*_tmp773=({struct Cyc_Dict_Dict*_tmp8=_region_malloc(d,sizeof(*_tmp8));({struct Cyc_Dict_Dict _tmp772=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp8=_tmp772;});_tmp8;});_tmp9->qvar_tags=_tmp773;}),({
struct Cyc_Xarray_Xarray*_tmp771=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp9->temp_labels=_tmp771;});_tmp9;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 149
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 155
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 158
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpA=ts;struct Cyc_Toc_TocStateWrap*_tmpB=Cyc_Toc_toc_state;ts=_tmpB;Cyc_Toc_toc_state=_tmpA;});{
struct Cyc_Toc_TocStateWrap _tmpC=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmpD=_tmpC;struct Cyc_Core_DynamicRegion*_tmp14;struct Cyc_Toc_TocState*_tmp13;_LL1: _tmp14=_tmpD.dyn;_tmp13=_tmpD.state;_LL2:;{
struct _fat_ptr _tmpE=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(_tmp14,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp77D=({struct Cyc_Toc_TocStateWrap _tmp73C;_tmp73C.dyn=_tmp14,_tmp73C.state=_tmp13;_tmp73C;});*ts=_tmp77D;});
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(_tmpE,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp12=({struct _tuple16 _tmp73D;_tmp73D.f1=_tmp13,_tmp73D.f2=arg;_tmp73D;});
res=f(h,& _tmp12);}
# 166
;}
# 168
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(_tmpE,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;};};};}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 172
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 175
struct _tuple18 _tmp15=*env;struct _tuple18 _tmp16=_tmp15;struct Cyc_Toc_TocState*_tmp19;struct _tuple1*_tmp18;void*(*_tmp17)(struct _tuple1*);_LL1: _tmp19=_tmp16.f1;_tmp18=(_tmp16.f2)->f1;_tmp17=(_tmp16.f2)->f2;_LL2:;{
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp19->aggrs_so_far,_tmp18);
return v == 0?_tmp17(_tmp18):(*(*v)).f2;};}
# 180
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp73E;_tmp73E.f1=q,_tmp73E.f2=type_maker;_tmp73E;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1A[5U]="curr";
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
struct _tuple1*qv=({struct _tuple1*_tmp6A=_cycalloc(sizeof(*_tmp6A));((_tmp6A->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp782=({struct _fat_ptr*_tmp65[1U];({struct _fat_ptr*_tmp781=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));({struct _fat_ptr _tmp780=({const char*_tmp66="Core";_tag_fat(_tmp66,sizeof(char),5U);});*_tmp67=_tmp780;});_tmp67;});_tmp65[0]=_tmp781;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp65,sizeof(struct _fat_ptr*),1U));});((_tmp6A->f1).Abs_n).val=_tmp782;}),({struct _fat_ptr*_tmp77F=({struct _fat_ptr*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _fat_ptr _tmp77E=({const char*_tmp68="get_exn_thrown";_tag_fat(_tmp68,sizeof(char),15U);});*_tmp69=_tmp77E;});_tmp69;});_tmp6A->f2=_tmp77F;});_tmp6A;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=0U,_tmp64->f1=qv;_tmp64;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->topt=0,({void*_tmp783=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=1U,_tmp62->f1=bnd;_tmp62;});_tmp63->r=_tmp783;}),_tmp63->loc=0U,_tmp63->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp63;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=10U,_tmp61->f1=fnname,_tmp61->f2=0,_tmp61->f3=0,_tmp61->f4=0;_tmp61;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->topt=0,_tmp60->r=fncall_re,_tmp60->loc=0U,_tmp60->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp60;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);};}
# 293
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 295
void*Cyc_Toc_void_star_type(){
static void*t=0;
if(t == 0)
t=({void*_tmp786=Cyc_Absyn_void_type;void*_tmp785=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp784=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_star_type(_tmp786,_tmp785,_tmp784,Cyc_Absyn_false_type);});
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
r=({void*_tmp787=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp787,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 314
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 318
static struct _tuple20*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct Cyc_List_List*_tmp789=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));({void*_tmp788=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=1U,_tmp6B->f1=name;_tmp6B;});_tmp6C->hd=_tmp788;}),_tmp6C->tl=0;_tmp6C;});_tmp6D->f1=_tmp789;}),_tmp6D->f2=e;_tmp6D;});}
# 322
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp78A=f;Cyc_Absyn_fncall_exp(_tmp78A,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
# 325
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp6E=e->r;void*_tmp6F=_tmp6E;struct Cyc_Absyn_Exp*_tmp70;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp70=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp70);}else{goto _LL3;}}else{_LL3: _LL4:
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
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=10U,_tmp76->f1=e,({struct Cyc_List_List*_tmp78B=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp76->f2=_tmp78B;}),_tmp76->f3=0,_tmp76->f4=1;_tmp76;});}
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
void*_tmp7C=e->r;void*_tmp7D=_tmp7C;struct Cyc_Absyn_Exp*_tmp7E;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7D)->tag == 20U){_LL1: _tmp7E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7D)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp7E,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 400 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 406
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp7F=Cyc_Tcutil_compress(t);void*_tmp80=_tmp7F;switch(*((int*)_tmp80)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp80)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp80)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1: _LL2:
 return fS->fchar;case Cyc_Absyn_Short_sz: _LL3: _LL4:
 return fS->fshort;case Cyc_Absyn_Int_sz: _LL5: _LL6:
 return fS->fint;default: goto _LLF;}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp80)->f1)->f1){case 0U: _LL7: _LL8:
 return fS->ffloat;case 1U: _LL9: _LLA:
 return fS->fdouble;default: _LLB: _LLC:
 return fS->flongdouble;}default: goto _LLF;}case 3U: _LLD: _LLE:
 return fS->fvoidstar;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp73F;_tmp73F.tag=0U,({struct _fat_ptr _tmp78C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp73F.f1=_tmp78C;});_tmp73F;});void*_tmp81[1U];_tmp81[0]=& _tmp83;({struct _fat_ptr _tmp78D=({const char*_tmp82="expression type %s (not int, float, double, or pointer)";_tag_fat(_tmp82,sizeof(char),56U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp78D,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 418
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp84=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp85=_tmp84;void*_tmp88;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp85)->tag == 3U){_LL1: _tmp88=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp85)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp88);}else{_LL3: _LL4:
({void*_tmp86=0U;({struct _fat_ptr _tmp78E=({const char*_tmp87="impossible type (not pointer)";_tag_fat(_tmp87,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp78E,_tag_fat(_tmp86,sizeof(void*),0U));});});}_LL0:;}
# 428
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp89=e->r;void*_tmp8A=_tmp89;struct Cyc_List_List*_tmp96;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp93;struct Cyc_List_List*_tmp92;struct Cyc_Absyn_Exp*_tmp91;long long _tmp90;int _tmp8F;short _tmp8E;struct _fat_ptr _tmp8D;char _tmp8C;switch(*((int*)_tmp8A)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Null_c).tag){case 2U: _LL1: _tmp8C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Char_c).val).f2;_LL2:
 return(int)_tmp8C == (int)'\000';case 3U: _LL3: _tmp8D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Wchar_c).val;_LL4: {
# 432
unsigned long _tmp8B=Cyc_strlen((struct _fat_ptr)_tmp8D);
int i=0;
if(_tmp8B >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(_tmp8D,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(_tmp8D,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)_tmp8D.curr)[1]== (int)'x' && _tmp8B >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(_tmp8D,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp8B;++ i){
if((int)*((const char*)_check_fat_subscript(_tmp8D,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 442
return 0;}}case 4U: _LL5: _tmp8E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Short_c).val).f2;_LL6:
 return(int)_tmp8E == 0;case 5U: _LL7: _tmp8F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).Int_c).val).f2;_LL8:
 return _tmp8F == 0;case 6U: _LL9: _tmp90=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A)->f1).LongLong_c).val).f2;_LLA:
 return _tmp90 == (long long)0;case 1U: _LLD: _LLE:
# 447
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 446
 goto _LLE;case 14U: _LLF: _tmp91=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL10:
# 448
 return Cyc_Toc_is_zero(_tmp91);case 24U: _LL11: _tmp92=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp92);case 26U: _LL13: _tmp93=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8A)->f1;_LL14:
 _tmp94=_tmp93;goto _LL16;case 29U: _LL15: _tmp94=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8A)->f3;_LL16:
 _tmp95=_tmp94;goto _LL18;case 25U: _LL17: _tmp95=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL18:
 _tmp96=_tmp95;goto _LL1A;case 36U: _LL19: _tmp96=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8A)->f2;_LL1A:
# 454
 for(0;_tmp96 != 0;_tmp96=_tmp96->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmp96->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 462
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmp97=x;union Cyc_Absyn_Nmspace _tmpA9;struct _fat_ptr*_tmpA8;_LL1: _tmpA9=_tmp97->f1;_tmpA8=_tmp97->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp98=_tmpA9;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;struct Cyc_List_List*_tmpA5;switch((_tmp98.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpA5=0;goto _LL7;case 1U: _LL6: _tmpA5=(_tmp98.Rel_n).val;_LL7:
 _tmpA6=_tmpA5;goto _LL9;case 2U: _LL8: _tmpA6=(_tmp98.Abs_n).val;_LL9:
 _tmpA7=_tmpA6;goto _LLB;default: _LLA: _tmpA7=(_tmp98.C_n).val;_LLB:
# 471
 if(_tmpA7 == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp9B=({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0U,_tmp741.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp741;});struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp740;_tmp740.tag=0U,_tmp740.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmpA8);_tmp740;});void*_tmp99[2U];_tmp99[0]=& _tmp9B,_tmp99[1]=& _tmp9C;({struct _fat_ptr _tmp78F=({const char*_tmp9A="%s_%s_struct";_tag_fat(_tmp9A,sizeof(char),13U);});Cyc_aprintf(_tmp78F,_tag_fat(_tmp99,sizeof(void*),2U));});});{
struct _RegionHandle _tmp9D=_new_region("r");struct _RegionHandle*r=& _tmp9D;_push_region(r);
{struct _fat_ptr _tmpA4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp744;_tmp744.tag=0U,_tmp744.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp744;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp743;_tmp743.tag=0U,({struct _fat_ptr _tmp792=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp791=r;struct Cyc_List_List*_tmp790=_tmpA7;Cyc_rstr_sepstr(_tmp791,_tmp790,({const char*_tmpA3="_";_tag_fat(_tmpA3,sizeof(char),2U);}));}));_tmp743.f1=_tmp792;});_tmp743;});struct Cyc_String_pa_PrintArg_struct _tmpA2=({struct Cyc_String_pa_PrintArg_struct _tmp742;_tmp742.tag=0U,_tmp742.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmpA8);_tmp742;});void*_tmp9E[3U];_tmp9E[0]=& _tmpA0,_tmp9E[1]=& _tmpA1,_tmp9E[2]=& _tmpA2;({struct _fat_ptr _tmp793=({const char*_tmp9F="%s_%s_%s_struct";_tag_fat(_tmp9F,sizeof(char),16U);});Cyc_aprintf(_tmp793,_tag_fat(_tmp9E,sizeof(void*),3U));});});_npop_handler(0U);return _tmpA4;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 484
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 487
struct _tuple21 _tmpAA=*env;struct _tuple21 _tmpAB=_tmpAA;struct Cyc_Dict_Dict*_tmpBE;struct _tuple15*_tmpBD;_LL1: _tmpBE=(_tmpAB.f1)->qvar_tags;_tmpBD=_tmpAB.f2;_LL2:;{
struct _tuple15 _tmpAC=*_tmpBD;struct _tuple15 _tmpAD=_tmpAC;struct _tuple1*_tmpBC;struct _tuple1*_tmpBB;_LL4: _tmpBC=_tmpAD.f1;_tmpBB=_tmpAD.f2;_LL5:;{
struct _handler_cons _tmpAE;_push_handler(& _tmpAE);{int _tmpB0=0;if(setjmp(_tmpAE.handler))_tmpB0=1;if(!_tmpB0){{struct _tuple1*_tmpB1=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpBE,Cyc_Toc_qvar_tag_cmp,_tmpBD);_npop_handler(0U);return _tmpB1;};_pop_handler();}else{void*_tmpAF=(void*)Cyc_Core_get_exn_thrown();void*_tmpB2=_tmpAF;void*_tmpBA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB2)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 491
struct _tuple15*_tmpB3=({struct _tuple15*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->f1=_tmpBC,_tmpB9->f2=_tmpBB;_tmpB9;});
struct _tuple1*_tmpB4=_tmpBC;union Cyc_Absyn_Nmspace _tmpB8;struct _fat_ptr*_tmpB7;_LLC: _tmpB8=_tmpB4->f1;_tmpB7=_tmpB4->f2;_LLD:;{
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(_tmpB7,_tmpBB);
struct _tuple1*res=({struct _tuple1*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->f1=_tmpB8,({struct _fat_ptr*_tmp794=({struct _fat_ptr*_tmpB5=_cycalloc(sizeof(*_tmpB5));*_tmpB5=newvar;_tmpB5;});_tmpB6->f2=_tmp794;});_tmpB6;});
({struct Cyc_Dict_Dict _tmp795=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpBE,_tmpB3,res);*_tmpBE=_tmp795;});
return res;};}}else{_LL9: _tmpBA=_tmpB2;_LLA:(int)_rethrow(_tmpBA);}_LL6:;}};};};}
# 499
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp745;_tmp745.f1=fieldname,_tmp745.f2=dtname;_tmp745;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 506
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->kind=Cyc_Absyn_StructA,_tmpC1->sc=Cyc_Absyn_Public,_tmpC1->tvs=0,_tmpC1->attributes=0,_tmpC1->expected_mem_kind=0,({
# 509
struct _tuple1*_tmp798=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));({union Cyc_Absyn_Nmspace _tmp797=Cyc_Absyn_Rel_n(0);_tmpBF->f1=_tmp797;}),_tmpBF->f2=name;_tmpBF;});_tmpC1->name=_tmp798;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp796=({struct Cyc_Absyn_AggrdeclImpl*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->exist_vars=0,_tmpC0->rgn_po=0,_tmpC0->fields=fs,_tmpC0->tagged=0;_tmpC0;});_tmpC1->impl=_tmp796;});_tmpC1;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 515
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 518
struct _tuple22 _tmpC2=*env;struct _tuple22 _tmpC3=_tmpC2;struct Cyc_List_List**_tmpD9;struct Cyc_List_List*_tmpD8;_LL1: _tmpD9=(_tmpC3.f1)->tuple_types;_tmpD8=_tmpC3.f2;_LL2:;
# 520
{struct Cyc_List_List*_tmpC4=*_tmpD9;for(0;_tmpC4 != 0;_tmpC4=_tmpC4->tl){
struct _tuple23*_tmpC5=(struct _tuple23*)_tmpC4->hd;struct _tuple23*_tmpC6=_tmpC5;void*_tmpC9;struct Cyc_List_List*_tmpC8;_LL4: _tmpC9=_tmpC6->f1;_tmpC8=_tmpC6->f2;_LL5:;{
struct Cyc_List_List*_tmpC7=_tmpD8;
for(0;_tmpC7 != 0 && _tmpC8 != 0;(_tmpC7=_tmpC7->tl,_tmpC8=_tmpC8->tl)){
if(!Cyc_Unify_unify((*((struct _tuple13*)_tmpC7->hd)).f2,(void*)_tmpC8->hd))
break;}
if(_tmpC7 == 0 && _tmpC8 == 0)
return _tmpC9;};}}{
# 531
struct _fat_ptr*xname=({struct _fat_ptr*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _fat_ptr _tmp79A=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD6=({struct Cyc_Int_pa_PrintArg_struct _tmp746;_tmp746.tag=1U,_tmp746.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp746;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({struct _fat_ptr _tmp799=({const char*_tmpD5="_tuple%d";_tag_fat(_tmpD5,sizeof(char),9U);});Cyc_aprintf(_tmp799,_tag_fat(_tmpD4,sizeof(void*),1U));});});*_tmpD7=_tmp79A;});_tmpD7;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpD8);
struct Cyc_List_List*_tmpCA=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmpCA=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));({struct Cyc_Absyn_Aggrfield*_tmp79C=({struct Cyc_Absyn_Aggrfield*_tmpCB=_cycalloc(sizeof(*_tmpCB));({struct _fat_ptr*_tmp79B=Cyc_Absyn_fieldname(i);_tmpCB->name=_tmp79B;}),_tmpCB->tq=Cyc_Toc_mt_tq,_tmpCB->type=(void*)ts2->hd,_tmpCB->width=0,_tmpCB->attributes=0,_tmpCB->requires_clause=0;_tmpCB;});_tmpCC->hd=_tmp79C;}),_tmpCC->tl=_tmpCA;_tmpCC;});}}
_tmpCA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCA);{
struct Cyc_Absyn_Aggrdecl*_tmpCD=Cyc_Toc_make_c_struct_defn(xname,_tmpCA);
void*_tmpCE=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpD3=_cycalloc(sizeof(*_tmpD3));*_tmpD3=_tmpCD;_tmpD3;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpD0=_cycalloc(sizeof(*_tmpD0));({struct Cyc_Absyn_Decl*_tmp79D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=5U,_tmpCF->f1=_tmpCD;_tmpCF;}),0U);_tmpD0->hd=_tmp79D;}),_tmpD0->tl=Cyc_Toc_result_decls;_tmpD0;});
({struct Cyc_List_List*_tmp79F=({struct Cyc_List_List*_tmpD2=_region_malloc(d,sizeof(*_tmpD2));({struct _tuple23*_tmp79E=({struct _tuple23*_tmpD1=_region_malloc(d,sizeof(*_tmpD1));_tmpD1->f1=_tmpCE,_tmpD1->f2=ts;_tmpD1;});_tmpD2->hd=_tmp79E;}),_tmpD2->tl=*_tmpD9;_tmpD2;});*_tmpD9=_tmp79F;});
return _tmpCE;};};}
# 544
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 549
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 552
struct _tuple25*_tmpDA=env;struct Cyc_List_List**_tmpEE;enum Cyc_Absyn_AggrKind _tmpED;struct Cyc_List_List*_tmpEC;_LL1: _tmpEE=(_tmpDA->f1)->anon_aggr_types;_tmpED=(_tmpDA->f2)->f1;_tmpEC=(_tmpDA->f2)->f2;_LL2:;
# 554
{struct Cyc_List_List*_tmpDB=*_tmpEE;for(0;_tmpDB != 0;_tmpDB=_tmpDB->tl){
struct _tuple26*_tmpDC=(struct _tuple26*)_tmpDB->hd;struct _tuple26*_tmpDD=_tmpDC;void*_tmpE0;enum Cyc_Absyn_AggrKind _tmpDF;struct Cyc_List_List*_tmpDE;_LL4: _tmpE0=_tmpDD->f1;_tmpDF=_tmpDD->f2;_tmpDE=_tmpDD->f3;_LL5:;
if((int)_tmpED != (int)_tmpDF)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpDE,_tmpEC))
return _tmpE0;}}{
# 563
struct _fat_ptr*xname=({struct _fat_ptr*_tmpEB=_cycalloc(sizeof(*_tmpEB));({struct _fat_ptr _tmp7A1=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpEA=({struct Cyc_Int_pa_PrintArg_struct _tmp747;_tmp747.tag=1U,_tmp747.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp747;});void*_tmpE8[1U];_tmpE8[0]=& _tmpEA;({struct _fat_ptr _tmp7A0=({const char*_tmpE9="_tuple%d";_tag_fat(_tmpE9,sizeof(char),9U);});Cyc_aprintf(_tmp7A0,_tag_fat(_tmpE8,sizeof(void*),1U));});});*_tmpEB=_tmp7A1;});_tmpEB;});
struct Cyc_Absyn_Aggrdecl*_tmpE1=Cyc_Toc_make_c_struct_defn(xname,_tmpEC);
_tmpE1->kind=_tmpED;{
void*_tmpE2=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE7=_cycalloc(sizeof(*_tmpE7));*_tmpE7=_tmpE1;_tmpE7;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct Cyc_Absyn_Decl*_tmp7A2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->tag=5U,_tmpE3->f1=_tmpE1;_tmpE3;}),0U);_tmpE4->hd=_tmp7A2;}),_tmpE4->tl=Cyc_Toc_result_decls;_tmpE4;});
({struct Cyc_List_List*_tmp7A4=({struct Cyc_List_List*_tmpE6=_region_malloc(d,sizeof(*_tmpE6));({struct _tuple26*_tmp7A3=({struct _tuple26*_tmpE5=_region_malloc(d,sizeof(*_tmpE5));_tmpE5->f1=_tmpE2,_tmpE5->f2=_tmpED,_tmpE5->f3=_tmpEC;_tmpE5;});_tmpE6->hd=_tmp7A3;}),_tmpE6->tl=*_tmpEE;_tmpE6;});*_tmpEE=_tmp7A4;});
return _tmpE2;};};}
# 571
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->f1=ak,_tmpEF->f2=fs;_tmpEF;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 580
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 589
struct _tuple28 _tmpF0=*env;struct _tuple28 _tmpF1=_tmpF0;struct Cyc_List_List**_tmp114;struct _tuple1*_tmp113;struct Cyc_List_List*_tmp112;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp110;_LL1: _tmp114=(_tmpF1.f1)->abs_struct_types;_tmp113=(_tmpF1.f2)->f1;_tmp112=(_tmpF1.f2)->f2;_tmp111=(_tmpF1.f2)->f3;_tmp110=(_tmpF1.f2)->f4;_LL2:;
# 593
{struct Cyc_List_List*_tmpF2=*_tmp114;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
struct _tuple29*_tmpF3=(struct _tuple29*)_tmpF2->hd;struct _tuple29*_tmpF4=_tmpF3;struct _tuple1*_tmpFC;struct Cyc_List_List*_tmpFB;void*_tmpFA;_LL4: _tmpFC=_tmpF4->f1;_tmpFB=_tmpF4->f2;_tmpFA=_tmpF4->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmpFC,_tmp113)== 0 &&({
int _tmp7A5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp111);_tmp7A5 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFB);})){
int okay=1;
{struct Cyc_List_List*_tmpF5=_tmp111;for(0;_tmpF5 != 0;(_tmpF5=_tmpF5->tl,_tmpFB=_tmpFB->tl)){
void*_tmpF6=(void*)_tmpF5->hd;
void*_tmpF7=(void*)((struct Cyc_List_List*)_check_null(_tmpFB))->hd;
{struct Cyc_Absyn_Kind*_tmpF8=Cyc_Tcutil_type_kind(_tmpF6);struct Cyc_Absyn_Kind*_tmpF9=_tmpF8;switch(((struct Cyc_Absyn_Kind*)_tmpF9)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 606
 if(Cyc_Unify_unify(_tmpF6,_tmpF7)||({void*_tmp7A6=Cyc_Toc_typ_to_c(_tmpF6);Cyc_Unify_unify(_tmp7A6,Cyc_Toc_typ_to_c(_tmpF7));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 611
break;}}
# 613
if(okay)
return _tmpFA;}}}{
# 620
struct _fat_ptr*xname=({struct _fat_ptr*_tmp10F=_cycalloc(sizeof(*_tmp10F));({struct _fat_ptr _tmp7A8=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp10E=({struct Cyc_Int_pa_PrintArg_struct _tmp748;_tmp748.tag=1U,_tmp748.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp748;});void*_tmp10C[1U];_tmp10C[0]=& _tmp10E;({struct _fat_ptr _tmp7A7=({const char*_tmp10D="_tuple%d";_tag_fat(_tmp10D,sizeof(char),9U);});Cyc_aprintf(_tmp7A7,_tag_fat(_tmp10C,sizeof(void*),1U));});});*_tmp10F=_tmp7A8;});_tmp10F;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmpFD=0;
# 624
({struct Cyc_List_List*_tmp7AA=({struct Cyc_List_List*_tmpFF=_region_malloc(d,sizeof(*_tmpFF));({struct _tuple29*_tmp7A9=({struct _tuple29*_tmpFE=_region_malloc(d,sizeof(*_tmpFE));_tmpFE->f1=_tmp113,_tmpFE->f2=_tmp111,_tmpFE->f3=x;_tmpFE;});_tmpFF->hd=_tmp7A9;}),_tmpFF->tl=*_tmp114;_tmpFF;});*_tmp114=_tmp7AA;});{
# 627
struct _RegionHandle _tmp100=_new_region("r");struct _RegionHandle*r=& _tmp100;_push_region(r);
{struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp112,_tmp111);
for(0;_tmp110 != 0;_tmp110=_tmp110->tl){
struct Cyc_Absyn_Aggrfield*_tmp102=(struct Cyc_Absyn_Aggrfield*)_tmp110->hd;
void*t=_tmp102->type;
struct Cyc_List_List*atts=_tmp102->attributes;
# 637
if((_tmp110->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp104=_cycalloc(sizeof(*_tmp104));({void*_tmp7AB=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=6U,_tmp103->f1=0;_tmp103;});_tmp104->hd=_tmp7AB;}),_tmp104->tl=atts;_tmp104;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp101,t));
# 644
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=4U,({void*_tmp7AE=Cyc_Toc_void_star_type();(_tmp105->f1).elt_type=_tmp7AE;}),({struct Cyc_Absyn_Tqual _tmp7AD=Cyc_Absyn_empty_tqual(0U);(_tmp105->f1).tq=_tmp7AD;}),({
struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Absyn_uint_exp(0U,0U);(_tmp105->f1).num_elts=_tmp7AC;}),(_tmp105->f1).zero_term=Cyc_Absyn_false_type,(_tmp105->f1).zt_loc=0U;_tmp105;});
# 648
_tmpFD=({struct Cyc_List_List*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_Aggrfield*_tmp7AF=({struct Cyc_Absyn_Aggrfield*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->name=_tmp102->name,_tmp106->tq=Cyc_Toc_mt_tq,_tmp106->type=t,_tmp106->width=_tmp102->width,_tmp106->attributes=atts,_tmp106->requires_clause=0;_tmp106;});_tmp107->hd=_tmp7AF;}),_tmp107->tl=_tmpFD;_tmp107;});}
# 650
_tmpFD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpFD);{
struct Cyc_Absyn_Aggrdecl*_tmp108=Cyc_Toc_make_c_struct_defn(xname,_tmpFD);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp10A=_cycalloc(sizeof(*_tmp10A));({struct Cyc_Absyn_Decl*_tmp7B0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=5U,_tmp109->f1=_tmp108;_tmp109;}),0U);_tmp10A->hd=_tmp7B0;}),_tmp10A->tl=Cyc_Toc_result_decls;_tmp10A;});{
void*_tmp10B=x;_npop_handler(0U);return _tmp10B;};};}
# 628
;_pop_region(r);};};}
# 656
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 660
struct _tuple27 env=({struct _tuple27 _tmp749;_tmp749.f1=struct_name,_tmp749.f2=type_vars,_tmp749.f3=type_args,_tmp749.f4=fields;_tmp749;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 668
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp7B3=({struct _fat_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _fat_ptr _tmp7B2=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp74A;_tmp74A.tag=1U,_tmp74A.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp74A;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _fat_ptr _tmp7B1=({const char*_tmp116="_tmp%X";_tag_fat(_tmp116,sizeof(char),7U);});Cyc_aprintf(_tmp7B1,_tag_fat(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp7B2;});_tmp118;});_tmp119->f2=_tmp7B3;});_tmp119;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 671
struct _tuple30 Cyc_Toc_temp_var_and_exp(){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp74B;_tmp74B.f1=v,({struct Cyc_Absyn_Exp*_tmp7B4=Cyc_Absyn_var_exp(v,0U);_tmp74B.f2=_tmp7B4;});_tmp74B;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 678
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp11A=*env;struct _tuple31 _tmp11B=_tmp11A;struct Cyc_Xarray_Xarray*_tmp123;_LL1: _tmp123=(_tmp11B.f1)->temp_labels;_LL2:;{
int _tmp11C=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7B5=_tmp11C;_tmp7B5 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp123);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp123,_tmp11C);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp122=_cycalloc(sizeof(*_tmp122));({struct _fat_ptr _tmp7B7=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp121=({struct Cyc_Int_pa_PrintArg_struct _tmp74C;_tmp74C.tag=1U,_tmp74C.f1=(unsigned)_tmp11C;_tmp74C;});void*_tmp11F[1U];_tmp11F[0]=& _tmp121;({struct _fat_ptr _tmp7B6=({const char*_tmp120="_LL%X";_tag_fat(_tmp120,sizeof(char),6U);});Cyc_aprintf(_tmp7B6,_tag_fat(_tmp11F,sizeof(void*),1U));});});*_tmp122=_tmp7B7;});_tmp122;});
if(({int _tmp7B8=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(_tmp123,res);_tmp7B8 != _tmp11C;}))
({void*_tmp11D=0U;({struct _fat_ptr _tmp7B9=({const char*_tmp11E="fresh_label: add_ind returned bad index...";_tag_fat(_tmp11E,sizeof(char),43U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7B9,_tag_fat(_tmp11D,sizeof(void*),0U));});});
return res;};};}
# 688
static struct _fat_ptr*Cyc_Toc_fresh_label(){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 694
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp124=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp124))->hd)->name)!= 0){
++ ans;
_tmp124=_tmp124->tl;}
# 701
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 707
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=(*a).f1,_tmp125->f2=(*a).f2,({void*_tmp7BA=Cyc_Toc_typ_to_c((*a).f3);_tmp125->f3=_tmp7BA;});_tmp125;});}
# 729 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp126=Cyc_Tcutil_compress(t);void*_tmp127=_tmp126;struct Cyc_Absyn_ArrayInfo _tmp128;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp127)->tag == 4U){_LL1: _tmp128=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp127)->f1;_LL2:
 return({void*_tmp7BB=Cyc_Toc_typ_to_c_array(_tmp128.elt_type);Cyc_Absyn_cstar_type(_tmp7BB,_tmp128.tq);});}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 736
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 738
struct Cyc_Absyn_Aggrfield*_tmp129=({struct Cyc_Absyn_Aggrfield*_tmp12A=_cycalloc(sizeof(*_tmp12A));*_tmp12A=*f;_tmp12A;});
_tmp129->type=new_type;
_tmp129->requires_clause=0;
_tmp129->tq=Cyc_Toc_mt_tq;
return _tmp129;}
# 745
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 747
return;}
# 750
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp12B=Cyc_Tcutil_compress(t);void*_tmp12C=_tmp12B;struct Cyc_Absyn_Tvar*_tmp12D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12C)->tag == 2U){_LL1: _tmp12D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp12C)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 756
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp12E=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp12F=_tmp12E;if(((struct Cyc_Absyn_Kind*)_tmp12F)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 762
static int Cyc_Toc_is_void_star(void*t){
void*_tmp130=Cyc_Tcutil_compress(t);void*_tmp131=_tmp130;void*_tmp132;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->tag == 3U){_LL1: _tmp132=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp132);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 768
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 771
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 775
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp133=t;struct Cyc_Absyn_Datatypedecl*_tmp17C;void**_tmp17B;struct Cyc_Absyn_Enumdecl*_tmp17A;struct Cyc_Absyn_Aggrdecl*_tmp179;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*_tmp177;struct _tuple1*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Typedefdecl*_tmp174;void*_tmp173;enum Cyc_Absyn_AggrKind _tmp172;struct Cyc_List_List*_tmp171;struct Cyc_List_List*_tmp170;struct Cyc_Absyn_Tqual _tmp16F;void*_tmp16E;struct Cyc_List_List*_tmp16D;int _tmp16C;struct Cyc_Absyn_VarargInfo*_tmp16B;struct Cyc_List_List*_tmp16A;void*_tmp169;struct Cyc_Absyn_Tqual _tmp168;struct Cyc_Absyn_Exp*_tmp167;unsigned _tmp166;void*_tmp165;struct Cyc_Absyn_Tqual _tmp164;void*_tmp163;struct Cyc_Absyn_Tvar*_tmp162;void**_tmp161;struct Cyc_List_List*_tmp160;struct _tuple1*_tmp15F;void*_tmp15E;union Cyc_Absyn_AggrInfo _tmp15D;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Datatypedecl*_tmp15B;struct Cyc_Absyn_Datatypefield*_tmp15A;switch(*((int*)_tmp133)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 788
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp15B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1).KnownDatatypefield).val).f1;_tmp15A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 790
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp15A->name,_tmp15B->name));}else{_LLB: _LLC:
# 792
({void*_tmp134=0U;({struct _fat_ptr _tmp7BC=({const char*_tmp135="unresolved DatatypeFieldType";_tag_fat(_tmp135,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7BC,_tag_fat(_tmp134,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 801
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp15E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1;_tmp15D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1;_tmp15C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f2;_LL1C:
# 848
{union Cyc_Absyn_AggrInfo _tmp145=_tmp15D;if((_tmp145.UnknownAggr).tag == 1){_LL45: _LL46:
# 850
 if(_tmp15C == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=0U,_tmp146->f1=_tmp15E,_tmp146->f2=0;_tmp146;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 854
struct Cyc_Absyn_Aggrdecl*_tmp147=Cyc_Absyn_get_known_aggrdecl(_tmp15D);
if(_tmp147->expected_mem_kind){
# 857
if(_tmp147->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp14A=({struct Cyc_String_pa_PrintArg_struct _tmp74E;_tmp74E.tag=0U,({
struct _fat_ptr _tmp7BD=(struct _fat_ptr)((int)_tmp147->kind == (int)Cyc_Absyn_UnionA?({const char*_tmp14C="union";_tag_fat(_tmp14C,sizeof(char),6U);}):({const char*_tmp14D="struct";_tag_fat(_tmp14D,sizeof(char),7U);}));_tmp74E.f1=_tmp7BD;});_tmp74E;});struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp74D;_tmp74D.tag=0U,({
struct _fat_ptr _tmp7BE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp147->name));_tmp74D.f1=_tmp7BE;});_tmp74D;});void*_tmp148[2U];_tmp148[0]=& _tmp14A,_tmp148[1]=& _tmp14B;({struct _fat_ptr _tmp7BF=({const char*_tmp149="%s %s was never defined.";_tag_fat(_tmp149,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp7BF,_tag_fat(_tmp148,sizeof(void*),2U));});});}
# 863
if((int)_tmp147->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp147->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp14E=_tmp147->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp147->impl))->fields;
if(_tmp14E == 0)return Cyc_Toc_aggrdecl_type(_tmp147->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp14E))->tl != 0;_tmp14E=_tmp14E->tl){;}{
void*_tmp14F=((struct Cyc_Absyn_Aggrfield*)_tmp14E->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp14F))){
if(_tmp147->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp74F;_tmp74F.tag=0U,({struct _fat_ptr _tmp7C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp147->name));_tmp74F.f1=_tmp7C0;});_tmp74F;});void*_tmp150[1U];_tmp150[0]=& _tmp152;({struct _fat_ptr _tmp7C1=({const char*_tmp151="struct %s ended up being abstract.";_tag_fat(_tmp151,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp7C1,_tag_fat(_tmp150,sizeof(void*),1U));});});{
# 876
struct _RegionHandle _tmp153=_new_region("r");struct _RegionHandle*r=& _tmp153;_push_region(r);
{struct Cyc_List_List*_tmp154=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp147->tvs,_tmp15C);
void*_tmp155=Cyc_Tcutil_rsubstitute(r,_tmp154,_tmp14F);
if(Cyc_Toc_is_abstract_type(_tmp155)){void*_tmp156=Cyc_Toc_aggrdecl_type(_tmp147->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp156;}{
void*_tmp157=Cyc_Toc_add_struct_type(_tmp147->name,_tmp147->tvs,_tmp15C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp147->impl))->fields);_npop_handler(0U);return _tmp157;};}
# 877
;_pop_region(r);};}
# 882
return Cyc_Toc_aggrdecl_type(_tmp147->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp15F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp160=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp133)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp160);return t;case 4U: _LL23: _LL24:
# 894
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 899
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp161=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp133)->f2;_LL4:
# 779
 if(*_tmp161 == 0){
*_tmp161=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 783
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp161));case 2U: _LL5: _tmp162=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp133)->f1;_LL6:
# 785
 if((int)(Cyc_Tcutil_tvar_kind(_tmp162,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;else{
return Cyc_Toc_void_star_type();}case 3U: _LLD: _tmp165=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_type;_tmp164=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).elt_tq;_tmp163=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp133)->f1).ptr_atts).bounds;_LLE:
# 796
 _tmp165=Cyc_Toc_typ_to_c(_tmp165);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp163)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 800
return Cyc_Absyn_star_type(_tmp165,Cyc_Absyn_heap_rgn_type,_tmp164,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp169=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).elt_type;_tmp168=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).tq;_tmp167=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).num_elts;_tmp166=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1).zt_loc;_LL14:
# 804
 return({void*_tmp7C5=Cyc_Toc_typ_to_c(_tmp169);struct Cyc_Absyn_Tqual _tmp7C4=_tmp168;struct Cyc_Absyn_Exp*_tmp7C3=_tmp167;void*_tmp7C2=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp7C5,_tmp7C4,_tmp7C3,_tmp7C2,_tmp166);});case 5U: _LL15: _tmp16F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).ret_tqual;_tmp16E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).ret_type;_tmp16D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).args;_tmp16C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).c_varargs;_tmp16B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).cyc_varargs;_tmp16A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp133)->f1).attributes;_LL16: {
# 810
struct Cyc_List_List*_tmp136=0;
for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
void*_tmp137=(void*)_tmp16A->hd;void*_tmp138=_tmp137;switch(*((int*)_tmp138)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
 _tmp136=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=(void*)_tmp16A->hd,_tmp139->tl=_tmp136;_tmp139;});goto _LL35;}_LL35:;}{
# 821
struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp16D);
if(_tmp16B != 0){
# 824
void*_tmp13B=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp16B->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp13C=({struct _tuple9*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->f1=_tmp16B->name,_tmp13E->f2=_tmp16B->tq,_tmp13E->f3=_tmp13B;_tmp13E;});
_tmp13A=({struct Cyc_List_List*_tmp7C6=_tmp13A;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7C6,({struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=_tmp13C,_tmp13D->tl=0;_tmp13D;}));});}
# 828
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->tag=5U,(_tmp13F->f1).tvars=0,(_tmp13F->f1).effect=0,(_tmp13F->f1).ret_tqual=_tmp16F,({void*_tmp7C7=Cyc_Toc_typ_to_c(_tmp16E);(_tmp13F->f1).ret_type=_tmp7C7;}),(_tmp13F->f1).args=_tmp13A,(_tmp13F->f1).c_varargs=_tmp16C,(_tmp13F->f1).cyc_varargs=0,(_tmp13F->f1).rgn_po=0,(_tmp13F->f1).attributes=_tmp136,(_tmp13F->f1).requires_clause=0,(_tmp13F->f1).requires_relns=0,(_tmp13F->f1).ensures_clause=0,(_tmp13F->f1).ensures_relns=0;_tmp13F;});};}case 6U: _LL17: _tmp170=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp133)->f1;_LL18: {
# 833
struct Cyc_List_List*_tmp140=0;
for(0;_tmp170 != 0;_tmp170=_tmp170->tl){
_tmp140=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));({struct _tuple13*_tmp7C9=({struct _tuple13*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=(*((struct _tuple13*)_tmp170->hd)).f1,({void*_tmp7C8=Cyc_Toc_typ_to_c((*((struct _tuple13*)_tmp170->hd)).f2);_tmp141->f2=_tmp7C8;});_tmp141;});_tmp142->hd=_tmp7C9;}),_tmp142->tl=_tmp140;_tmp142;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp140));}case 7U: _LL19: _tmp172=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp133)->f1;_tmp171=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp133)->f2;_LL1A: {
# 842
struct Cyc_List_List*_tmp143=0;
for(0;_tmp171 != 0;_tmp171=_tmp171->tl){
_tmp143=({struct Cyc_List_List*_tmp144=_cycalloc(sizeof(*_tmp144));({struct Cyc_Absyn_Aggrfield*_tmp7CB=({struct Cyc_Absyn_Aggrfield*_tmp7CA=(struct Cyc_Absyn_Aggrfield*)_tmp171->hd;Cyc_Toc_aggrfield_to_c(_tmp7CA,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp171->hd)->type));});_tmp144->hd=_tmp7CB;}),_tmp144->tl=_tmp143;_tmp144;});}
return({enum Cyc_Absyn_AggrKind _tmp7CC=_tmp172;Cyc_Toc_add_anon_aggr_type(_tmp7CC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp143));});}case 8U: _LL21: _tmp176=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f1;_tmp175=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f2;_tmp174=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f3;_tmp173=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp133)->f4;_LL22:
# 888
 if(_tmp173 == 0){
if(_tmp175 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->tag=8U,_tmp158->f1=_tmp176,_tmp158->f2=0,_tmp158->f3=_tmp174,_tmp158->f4=0;_tmp158;});else{
return t;}}else{
# 893
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->tag=8U,_tmp159->f1=_tmp176,_tmp159->f2=0,_tmp159->f3=_tmp174,({void*_tmp7CD=Cyc_Toc_typ_to_c(_tmp173);_tmp159->f4=_tmp7CD;});_tmp159;});}case 9U: _LL2B: _tmp177=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp133)->f1;_LL2C:
# 904
 return t;case 11U: _LL2D: _tmp178=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp133)->f1;_LL2E:
# 909
 if(_tmp178->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(_tmp178->topt));
return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)){case 0U: _LL2F: _tmp179=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)->f1;_LL30:
# 913
 Cyc_Toc_aggrdecl_to_c(_tmp179);
if((int)_tmp179->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp179->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(_tmp179->name,Cyc_Absyn_strctq);case 1U: _LL31: _tmp17A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)->f1;_LL32:
# 918
 Cyc_Toc_enumdecl_to_c(_tmp17A);
return t;default: _LL33: _tmp17C=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f1)->r)->f1;_tmp17B=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp133)->f2;_LL34:
# 921
 Cyc_Toc_datatypedecl_to_c(_tmp17C);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp17B)));}}_LL0:;}
# 926
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp17D=t;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp17E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->tag == 4U){_LL1: _tmp17F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->f1).elt_type;_tmp17E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->f1).tq;_LL2:
# 929
 return({void*_tmp7CE=Cyc_Absyn_star_type(_tmp17F,Cyc_Absyn_heap_rgn_type,_tmp17E,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp7CE,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 936
static int Cyc_Toc_atomic_type(void*t){
void*_tmp180=Cyc_Tcutil_compress(t);void*_tmp181=_tmp180;struct Cyc_List_List*_tmp194;struct Cyc_List_List*_tmp193;void*_tmp192;void*_tmp191;struct Cyc_List_List*_tmp190;switch(*((int*)_tmp181)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp191=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f1;_tmp190=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp181)->f2;_LL4: {
# 940
void*_tmp182=_tmp191;struct Cyc_Absyn_Datatypedecl*_tmp18C;struct Cyc_Absyn_Datatypefield*_tmp18B;union Cyc_Absyn_AggrInfo _tmp18A;switch(*((int*)_tmp182)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp18A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp182)->f1;_LL25:
# 947
{union Cyc_Absyn_AggrInfo _tmp183=_tmp18A;if((_tmp183.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 951
struct Cyc_Absyn_Aggrdecl*_tmp184=Cyc_Absyn_get_known_aggrdecl(_tmp18A);
if(_tmp184->impl == 0)
return 0;
{struct Cyc_List_List*_tmp185=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp184->impl))->fields;for(0;_tmp185 != 0;_tmp185=_tmp185->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp185->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp182)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp18C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp182)->f1).KnownDatatypefield).val).f1;_tmp18B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp182)->f1).KnownDatatypefield).val).f2;_LL27:
# 958
{struct Cyc_List_List*_tmp186=_tmp18B->typs;for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple13*)_tmp186->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp750;_tmp750.tag=0U,({struct _fat_ptr _tmp7CF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp750.f1=_tmp7CF;});_tmp750;});void*_tmp187[1U];_tmp187[0]=& _tmp189;({struct _fat_ptr _tmp7D0=({const char*_tmp188="atomic_typ:  bad type %s";_tag_fat(_tmp188,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D0,_tag_fat(_tmp187,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 963
 return 1;case 4U: _LL7: _tmp192=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp181)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp192);case 7U: _LL9: _tmp193=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp181)->f2;_LLA:
# 971
 for(0;_tmp193 != 0;_tmp193=_tmp193->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp193->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp194=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp181)->f1;_LLC:
# 975
 for(0;_tmp194 != 0;_tmp194=_tmp194->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple13*)_tmp194->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 980
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp18F=({struct Cyc_String_pa_PrintArg_struct _tmp751;_tmp751.tag=0U,({struct _fat_ptr _tmp7D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp751.f1=_tmp7D1;});_tmp751;});void*_tmp18D[1U];_tmp18D[0]=& _tmp18F;({struct _fat_ptr _tmp7D2=({const char*_tmp18E="atomic_typ:  bad type %s";_tag_fat(_tmp18E,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D2,_tag_fat(_tmp18D,sizeof(void*),1U));});});}_LL0:;}
# 986
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp195=Cyc_Tcutil_compress(t);void*_tmp196=_tmp195;struct Cyc_List_List*_tmp1A2;union Cyc_Absyn_AggrInfo _tmp1A1;switch(*((int*)_tmp196)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f1)->tag == 20U){_LL1: _tmp1A1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f1)->f1;_LL2: {
# 989
struct Cyc_Absyn_Aggrdecl*_tmp197=Cyc_Absyn_get_known_aggrdecl(_tmp1A1);
if(_tmp197->impl == 0)
({void*_tmp198=0U;({struct _fat_ptr _tmp7D3=({const char*_tmp199="is_poly_field: type missing fields";_tag_fat(_tmp199,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D3,_tag_fat(_tmp198,sizeof(void*),0U));});});
_tmp1A2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp197->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp196)->f2;_LL4: {
# 994
struct Cyc_Absyn_Aggrfield*_tmp19A=Cyc_Absyn_lookup_field(_tmp1A2,f);
if(_tmp19A == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp19D=({struct Cyc_String_pa_PrintArg_struct _tmp752;_tmp752.tag=0U,_tmp752.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp752;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19D;({struct _fat_ptr _tmp7D4=({const char*_tmp19C="is_poly_field: bad field %s";_tag_fat(_tmp19C,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D4,_tag_fat(_tmp19B,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp19A->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp753;_tmp753.tag=0U,({struct _fat_ptr _tmp7D5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp753.f1=_tmp7D5;});_tmp753;});void*_tmp19E[1U];_tmp19E[0]=& _tmp1A0;({struct _fat_ptr _tmp7D6=({const char*_tmp19F="is_poly_field: bad type %s";_tag_fat(_tmp19F,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D6,_tag_fat(_tmp19E,sizeof(void*),1U));});});}_LL0:;}
# 1005
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1A3=e->r;void*_tmp1A4=_tmp1A3;struct Cyc_Absyn_Exp*_tmp1AE;struct _fat_ptr*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct _fat_ptr*_tmp1AB;switch(*((int*)_tmp1A4)){case 21U: _LL1: _tmp1AC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A4)->f1;_tmp1AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A4)->f2;_LL2:
# 1008
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1AC->topt),_tmp1AB)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1AE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A4)->f1;_tmp1AD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A4)->f2;_LL4: {
# 1011
void*_tmp1A5=Cyc_Tcutil_compress((void*)_check_null(_tmp1AE->topt));void*_tmp1A6=_tmp1A5;void*_tmp1AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A6)->tag == 3U){_LL8: _tmp1AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A6)->f1).elt_type;_LL9:
# 1013
 return Cyc_Toc_is_poly_field(_tmp1AA,_tmp1AD)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1A9=({struct Cyc_String_pa_PrintArg_struct _tmp754;_tmp754.tag=0U,({struct _fat_ptr _tmp7D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1AE->topt)));_tmp754.f1=_tmp7D7;});_tmp754;});void*_tmp1A7[1U];_tmp1A7[0]=& _tmp1A9;({struct _fat_ptr _tmp7D8=({const char*_tmp1A8="is_poly_project: bad type %s";_tag_fat(_tmp1A8,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7D8,_tag_fat(_tmp1A7,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1016
 return 0;}_LL0:;}
# 1021
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1AF[1U];_tmp1AF[0]=s;({struct Cyc_Absyn_Exp*_tmp7D9=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7D9,_tag_fat(_tmp1AF,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1026
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1B0[2U];_tmp1B0[0]=s,_tmp1B0[1]=n;({struct Cyc_Absyn_Exp*_tmp7DA=fn_e;Cyc_Toc_fncall_exp_dl(_tmp7DA,_tag_fat(_tmp1B0,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1031
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1B1[2U];_tmp1B1[0]=rgn,_tmp1B1[1]=s;({struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7DB,_tag_fat(_tmp1B1,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1035
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1B2[2U];_tmp1B2[0]=rgn,_tmp1B2[1]=s;({struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp7DC,_tag_fat(_tmp1B2,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1B3[3U];_tmp1B3[0]=rgn,_tmp1B3[1]=s,_tmp1B3[2]=n;({struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp7DD,_tag_fat(_tmp1B3,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1043
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({void*_tmp1B4=0U;({struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp7DE,_tag_fat(_tmp1B4,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1048
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1056
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1B5=e->r;void*_tmp1B6=_tmp1B5;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1B6)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1B6)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1061
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));({struct Cyc_Absyn_Decl*_tmp7DF=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->tag=0U,_tmp1B7->f1=vd;_tmp1B7;}),0U);_tmp1B8->hd=_tmp7DF;}),_tmp1B8->tl=Cyc_Toc_result_decls;_tmp1B8;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1069
 xexp=({void*_tmp7E0=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7E0,e);});
# 1071
xplussz=({void*_tmp7E1=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp7E1,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1074
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1B9[3U];({struct _tuple20*_tmp7E4=({struct _tuple20*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->f1=0,_tmp1BA->f2=xexp;_tmp1BA;});_tmp1B9[0]=_tmp7E4;}),({
struct _tuple20*_tmp7E3=({struct _tuple20*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB->f1=0,_tmp1BB->f2=xexp;_tmp1BB;});_tmp1B9[1]=_tmp7E3;}),({
struct _tuple20*_tmp7E2=({struct _tuple20*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->f1=0,_tmp1BC->f2=xplussz;_tmp1BC;});_tmp1B9[2]=_tmp7E2;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B9,sizeof(struct _tuple20*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1109 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE->break_lab=0,_tmp1BE->continue_lab=0,_tmp1BE->fallthru_info=0,_tmp1BE->toplevel=1,({int*_tmp7E5=({int*_tmp1BD=_region_malloc(r,sizeof(*_tmp1BD));*_tmp1BD=0;_tmp1BD;});_tmp1BE->in_lhs=_tmp7E5;}),_tmp1BE->rgn=r;_tmp1BE;});}
# 1113
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1BF=nv;int _tmp1C0;_LL1: _tmp1C0=_tmp1BF->toplevel;_LL2:;
return _tmp1C0;}
# 1117
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1C1=e;struct _fat_ptr**_tmp1C7;struct _fat_ptr**_tmp1C6;struct Cyc_Toc_FallthruInfo*_tmp1C5;int _tmp1C4;int*_tmp1C3;_LL1: _tmp1C7=_tmp1C1->break_lab;_tmp1C6=_tmp1C1->continue_lab;_tmp1C5=_tmp1C1->fallthru_info;_tmp1C4=_tmp1C1->toplevel;_tmp1C3=_tmp1C1->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1C2=_region_malloc(r,sizeof(*_tmp1C2));_tmp1C2->break_lab=_tmp1C7,_tmp1C2->continue_lab=_tmp1C6,_tmp1C2->fallthru_info=_tmp1C5,_tmp1C2->toplevel=0,_tmp1C2->in_lhs=_tmp1C3,_tmp1C2->rgn=r;_tmp1C2;});}
# 1121
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1C8=e;struct _fat_ptr**_tmp1CE;struct _fat_ptr**_tmp1CD;struct Cyc_Toc_FallthruInfo*_tmp1CC;int _tmp1CB;int*_tmp1CA;_LL1: _tmp1CE=_tmp1C8->break_lab;_tmp1CD=_tmp1C8->continue_lab;_tmp1CC=_tmp1C8->fallthru_info;_tmp1CB=_tmp1C8->toplevel;_tmp1CA=_tmp1C8->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1C9=_region_malloc(r,sizeof(*_tmp1C9));_tmp1C9->break_lab=_tmp1CE,_tmp1C9->continue_lab=_tmp1CD,_tmp1C9->fallthru_info=_tmp1CC,_tmp1C9->toplevel=1,_tmp1C9->in_lhs=_tmp1CA,_tmp1C9->rgn=r;_tmp1C9;});}
# 1125
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1CF=nv;int*_tmp1D0;_LL1: _tmp1D0=_tmp1CF->in_lhs;_LL2:;
return*_tmp1D0;}
# 1129
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1D1=e;int*_tmp1D2;_LL1: _tmp1D2=_tmp1D1->in_lhs;_LL2:;
*_tmp1D2=b;}
# 1134
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D3=e;struct _fat_ptr**_tmp1D9;struct _fat_ptr**_tmp1D8;struct Cyc_Toc_FallthruInfo*_tmp1D7;int _tmp1D6;int*_tmp1D5;_LL1: _tmp1D9=_tmp1D3->break_lab;_tmp1D8=_tmp1D3->continue_lab;_tmp1D7=_tmp1D3->fallthru_info;_tmp1D6=_tmp1D3->toplevel;_tmp1D5=_tmp1D3->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1D4=_region_malloc(r,sizeof(*_tmp1D4));_tmp1D4->break_lab=_tmp1D9,_tmp1D4->continue_lab=_tmp1D8,_tmp1D4->fallthru_info=_tmp1D7,_tmp1D4->toplevel=_tmp1D6,_tmp1D4->in_lhs=_tmp1D5,_tmp1D4->rgn=r;_tmp1D4;});}
# 1141
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DA=e;struct _fat_ptr**_tmp1E0;struct _fat_ptr**_tmp1DF;struct Cyc_Toc_FallthruInfo*_tmp1DE;int _tmp1DD;int*_tmp1DC;_LL1: _tmp1E0=_tmp1DA->break_lab;_tmp1DF=_tmp1DA->continue_lab;_tmp1DE=_tmp1DA->fallthru_info;_tmp1DD=_tmp1DA->toplevel;_tmp1DC=_tmp1DA->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=_tmp1DE,_tmp1DB->toplevel=_tmp1DD,_tmp1DB->in_lhs=_tmp1DC,_tmp1DB->rgn=r;_tmp1DB;});}
# 1147
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1151
struct Cyc_Toc_Env*_tmp1E1=e;struct _fat_ptr**_tmp1E9;struct _fat_ptr**_tmp1E8;struct Cyc_Toc_FallthruInfo*_tmp1E7;int _tmp1E6;int*_tmp1E5;_LL1: _tmp1E9=_tmp1E1->break_lab;_tmp1E8=_tmp1E1->continue_lab;_tmp1E7=_tmp1E1->fallthru_info;_tmp1E6=_tmp1E1->toplevel;_tmp1E5=_tmp1E1->in_lhs;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1E4=_region_malloc(r,sizeof(*_tmp1E4));
_tmp1E4->label=fallthru_l,_tmp1E4->binders=fallthru_binders;_tmp1E4;});
return({struct Cyc_Toc_Env*_tmp1E3=_region_malloc(r,sizeof(*_tmp1E3));({struct _fat_ptr**_tmp7E6=({struct _fat_ptr**_tmp1E2=_region_malloc(r,sizeof(*_tmp1E2));*_tmp1E2=break_l;_tmp1E2;});_tmp1E3->break_lab=_tmp7E6;}),_tmp1E3->continue_lab=_tmp1E8,_tmp1E3->fallthru_info=fi,_tmp1E3->toplevel=_tmp1E6,_tmp1E3->in_lhs=_tmp1E5,_tmp1E3->rgn=r;_tmp1E3;});};}
# 1156
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1158
struct Cyc_Toc_Env*_tmp1EA=e;struct _fat_ptr**_tmp1F1;struct _fat_ptr**_tmp1F0;struct Cyc_Toc_FallthruInfo*_tmp1EF;int _tmp1EE;int*_tmp1ED;_LL1: _tmp1F1=_tmp1EA->break_lab;_tmp1F0=_tmp1EA->continue_lab;_tmp1EF=_tmp1EA->fallthru_info;_tmp1EE=_tmp1EA->toplevel;_tmp1ED=_tmp1EA->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1EC=_region_malloc(r,sizeof(*_tmp1EC));({struct _fat_ptr**_tmp7E7=({struct _fat_ptr**_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));*_tmp1EB=break_l;_tmp1EB;});_tmp1EC->break_lab=_tmp7E7;}),_tmp1EC->continue_lab=_tmp1F0,_tmp1EC->fallthru_info=0,_tmp1EC->toplevel=_tmp1EE,_tmp1EC->in_lhs=_tmp1ED,_tmp1EC->rgn=r;_tmp1EC;});}
# 1165
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1167
struct Cyc_Toc_Env*_tmp1F2=e;struct _fat_ptr**_tmp1F9;struct _fat_ptr**_tmp1F8;struct Cyc_Toc_FallthruInfo*_tmp1F7;int _tmp1F6;int*_tmp1F5;_LL1: _tmp1F9=_tmp1F2->break_lab;_tmp1F8=_tmp1F2->continue_lab;_tmp1F7=_tmp1F2->fallthru_info;_tmp1F6=_tmp1F2->toplevel;_tmp1F5=_tmp1F2->in_lhs;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F4=_region_malloc(r,sizeof(*_tmp1F4));_tmp1F4->break_lab=0,_tmp1F4->continue_lab=_tmp1F8,({struct Cyc_Toc_FallthruInfo*_tmp7E8=({struct Cyc_Toc_FallthruInfo*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->label=next_l,_tmp1F3->binders=0;_tmp1F3;});_tmp1F4->fallthru_info=_tmp7E8;}),_tmp1F4->toplevel=_tmp1F6,_tmp1F4->in_lhs=_tmp1F5,_tmp1F4->rgn=r;_tmp1F4;});}
# 1180 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1183
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->annot;void*_tmp1FB=_tmp1FA;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp1FB)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp1FC=0U;({unsigned _tmp7EA=e->loc;struct _fat_ptr _tmp7E9=({const char*_tmp1FD="Toc: do_null_check";_tag_fat(_tmp1FD,sizeof(char),19U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp7EA,_tmp7E9,_tag_fat(_tmp1FC,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1199
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1201
int ans;
int _tmp1FE=Cyc_Toc_in_lhs(nv);
void*_tmp1FF=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp200=Cyc_Toc_typ_to_c(_tmp1FF);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp201=_tmp1FF;void*_tmp219;struct Cyc_Absyn_Tqual _tmp218;void*_tmp217;void*_tmp216;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->tag == 3U){_LL1: _tmp219=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).elt_type;_tmp218=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).elt_tq;_tmp217=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).ptr_atts).bounds;_tmp216=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201)->f1).ptr_atts).zero_term;_LL2:
# 1212
{void*_tmp202=annot;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp212;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp202)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1214
 if(!((unsigned)({void*_tmp7EB=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7EB,_tmp217);}))){
# 1217
void*_tmp203=({void*_tmp7EC=Cyc_Toc_typ_to_c(_tmp219);Cyc_Absyn_cstar_type(_tmp7EC,_tmp218);});
({void*_tmp7EF=({void*_tmp7EE=_tmp203;Cyc_Toc_cast_it_r(_tmp7EE,({struct Cyc_Absyn_Exp*_tmp7ED=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7ED,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp7EF;});
ptr->topt=_tmp203;}
# 1221
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp202)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1224
 if(!((unsigned)({void*_tmp7F0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7F0,_tmp217);}))){
# 1226
void*_tmp204=({void*_tmp7F1=Cyc_Toc_typ_to_c(_tmp219);Cyc_Absyn_cstar_type(_tmp7F1,_tmp218);});
({void*_tmp7F4=({void*_tmp7F3=_tmp204;Cyc_Toc_cast_it_r(_tmp7F3,({struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7F2,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp7F4;});
ptr->topt=_tmp204;
# 1233
if(index != 0)
({void*_tmp205=0U;({struct _fat_ptr _tmp7F5=({const char*_tmp206="subscript of ? with no bounds check but need null check";_tag_fat(_tmp206,sizeof(char),56U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp7F5,_tag_fat(_tmp205,sizeof(void*),0U));});});}
# 1237
({void*_tmp7F9=({void*_tmp7F8=_tmp200;Cyc_Toc_cast_it_r(_tmp7F8,({struct Cyc_Absyn_Exp*_tmp207[1U];({struct Cyc_Absyn_Exp*_tmp7F6=Cyc_Absyn_new_exp(ptr->r,0U);_tmp207[0]=_tmp7F6;});({struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp7F7,_tag_fat(_tmp207,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp7F9;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp202)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp202)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1243
void*ta1=Cyc_Toc_typ_to_c(_tmp219);
void*newt=Cyc_Absyn_cstar_type(ta1,_tmp218);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7FE=({void*_tmp7FD=newt;Cyc_Toc_cast_it_r(_tmp7FD,({struct Cyc_Absyn_Exp*_tmp208[3U];({
struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Absyn_new_exp(ptr->r,0U);_tmp208[0]=_tmp7FB;}),({
struct Cyc_Absyn_Exp*_tmp7FA=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp208[1]=_tmp7FA;}),_tmp208[2]=ind;({struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp7FC,_tag_fat(_tmp208,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1246
ptr->r=_tmp7FE;});
# 1250
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp202)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp212=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp202)->f1;_LLF:
# 1254
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp213=_tmp212;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp202)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp213=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp202)->f1;_LL11: {
# 1257
void*ta1=Cyc_Toc_typ_to_c(_tmp219);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1262
struct _tuple14 _tmp209=Cyc_Evexp_eval_const_uint_exp(_tmp213);struct _tuple14 _tmp20A=_tmp209;unsigned _tmp20F;int _tmp20E;_LL15: _tmp20F=_tmp20A.f1;_tmp20E=_tmp20A.f2;_LL16:;
if((!_tmp20E || _tmp20F != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1265
({void*_tmp803=({void*_tmp802=Cyc_Absyn_cstar_type(ta1,_tmp218);Cyc_Toc_cast_it_r(_tmp802,({struct Cyc_Absyn_Exp*_tmp20B[4U];({
struct Cyc_Absyn_Exp*_tmp800=Cyc_Absyn_new_exp(ptr->r,0U);_tmp20B[0]=_tmp800;}),_tmp20B[1]=_tmp213,({
struct Cyc_Absyn_Exp*_tmp7FF=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp20B[2]=_tmp7FF;}),_tmp20B[3]=ind;({struct Cyc_Absyn_Exp*_tmp801=fn_e;Cyc_Toc_fncall_exp_dl(_tmp801,_tag_fat(_tmp20B,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1265
ptr->r=_tmp803;});
# 1268
ans=1;}else{
# 1271
if(Cyc_Toc_is_zero(_tmp213)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1274
({void*_tmp807=({void*_tmp806=_tmp200;Cyc_Toc_cast_it_r(_tmp806,({struct Cyc_Absyn_Exp*_tmp20C[1U];({struct Cyc_Absyn_Exp*_tmp804=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp20C[0]=_tmp804;});({struct Cyc_Absyn_Exp*_tmp805=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp805,_tag_fat(_tmp20C,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1274
ptr->r=_tmp807;});
# 1276
ans=0;}else{
# 1280
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp80B=({void*_tmp80A=Cyc_Absyn_cstar_type(ta1,_tmp218);Cyc_Toc_cast_it_r(_tmp80A,({struct Cyc_Absyn_Exp*_tmp20D[3U];({
struct Cyc_Absyn_Exp*_tmp808=Cyc_Absyn_new_exp(ptr->r,0U);_tmp20D[0]=_tmp808;}),_tmp20D[1]=_tmp213,_tmp20D[2]=ind;({struct Cyc_Absyn_Exp*_tmp809=fn_e;Cyc_Toc_fncall_exp_dl(_tmp809,_tag_fat(_tmp20D,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1281
ptr->r=_tmp80B;});
# 1283
ans=1;}}
# 1286
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp210=0U;({struct _fat_ptr _tmp80C=({const char*_tmp211="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp211,sizeof(char),34U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp80C,_tag_fat(_tmp210,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1289
Cyc_Toc_set_lhs(nv,_tmp1FE);
return ans;}else{_LL3: _LL4:
({void*_tmp214=0U;({struct _fat_ptr _tmp80D=({const char*_tmp215="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp215,sizeof(char),31U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp80D,_tag_fat(_tmp214,sizeof(void*),0U));});});}_LL0:;};}
# 1295
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp21A=0U;({struct _fat_ptr _tmp80E=({const char*_tmp21B="Missing type in primop ";_tag_fat(_tmp21B,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp80E,_tag_fat(_tmp21A,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1299
static struct _tuple13*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple13*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->f1=Cyc_Toc_mt_tq,({void*_tmp80F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp21C->f2=_tmp80F;});_tmp21C;});}
# 1304
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp21D=e->r;void*_tmp21E=_tmp21D;struct Cyc_Absyn_Exp*_tmp226;int _tmp225;struct Cyc_Absyn_Exp*_tmp224;int _tmp223;struct Cyc_List_List*_tmp222;switch(*((int*)_tmp21E)){case 26U: _LL1: _tmp222=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp21E)->f1;_LL2:
# 1307
{struct Cyc_List_List*_tmp21F=_tmp222;for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){
if((*((struct _tuple20*)_tmp21F->hd)).f1 != 0)
({void*_tmp220=0U;({struct _fat_ptr _tmp810=({const char*_tmp221="array designators for abstract-field initialization";_tag_fat(_tmp221,sizeof(char),52U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_unimp)(_tmp810,_tag_fat(_tmp220,sizeof(void*),0U));});});}}
_tmp224=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp222),0U);_tmp223=0;goto _LL4;case 27U: _LL3: _tmp224=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21E)->f2;_tmp223=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21E)->f4;_LL4:
 _tmp226=_tmp224;_tmp225=_tmp223;goto _LL6;case 28U: _LL5: _tmp226=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp21E)->f1;_tmp225=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp21E)->f3;_LL6:
# 1313
 _tmp226=Cyc_Absyn_copy_exp(_tmp226);
return _tmp225?({struct Cyc_Absyn_Exp*_tmp811=_tmp226;Cyc_Absyn_add_exp(_tmp811,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp226;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1322
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1330
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp227=e->r;void*_tmp228=_tmp227;struct Cyc_List_List*_tmp229;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->tag == 29U){_LL1: _tmp229=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp228)->f3;_LL2:
 dles=_tmp229;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1336
{void*_tmp22A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp22B=_tmp22A;struct Cyc_Absyn_Aggrdecl*_tmp22C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp22C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22B)->f1)->f1).KnownAggr).val;_LL7:
# 1338
 if(_tmp22C->impl == 0)
return 0;
if((int)_tmp22C->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22C->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1346
 return 0;}_LL5:;}
# 1348
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp22D=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp22E=(struct _tuple20*)dles->hd;struct _tuple20*_tmp22F=_tmp22E;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Exp*_tmp237;_LLB: _tmp238=_tmp22F->f1;_tmp237=_tmp22F->f2;_LLC:;{
struct _fat_ptr*_tmp230=Cyc_Absyn_designatorlist_to_fieldname(_tmp238);
if(!Cyc_strptrcmp(_tmp230,_tmp22D->name)){
struct Cyc_Absyn_Exp*_tmp231=Cyc_Toc_get_varsizeexp(_tmp237);
if(_tmp231 != 0)
return _tmp231;{
void*_tmp232=Cyc_Tcutil_compress(_tmp22D->type);void*_tmp233=_tmp232;void*_tmp236;struct Cyc_Absyn_Exp*_tmp235;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp233)->tag == 4U){_LLE: _tmp236=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp233)->f1).elt_type;_tmp235=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp233)->f1).num_elts;_LLF:
# 1363
 if(_tmp235 == 0 || !Cyc_Toc_is_zero(_tmp235))
return 0;
# 1369
return({struct Cyc_Absyn_Exp*_tmp814=({struct Cyc_Absyn_Exp*_tmp234[2U];_tmp234[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp237)),({
# 1371
struct Cyc_Absyn_Exp*_tmp812=Cyc_Absyn_sizeoftype_exp(_tmp236,0U);_tmp234[1]=_tmp812;});({struct Cyc_Absyn_Exp*_tmp813=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp813,_tag_fat(_tmp234,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1369
Cyc_Absyn_add_exp(_tmp814,
# 1372
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1377
({void*_tmp239=0U;({struct _fat_ptr _tmp815=({const char*_tmp23A="get_varsizeexp: did not find last struct field";_tag_fat(_tmp23A,sizeof(char),47U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp815,_tag_fat(_tmp239,sizeof(void*),0U));});});};}
# 1380
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp23B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
struct Cyc_Absyn_Aggrfield*_tmp23C=(struct Cyc_Absyn_Aggrfield*)_tmp23B->hd;
if(Cyc_strcmp((struct _fat_ptr)*_tmp23C->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1387
({void*_tmp23D=0U;({struct _fat_ptr _tmp817=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp240=({struct Cyc_String_pa_PrintArg_struct _tmp755;_tmp755.tag=0U,_tmp755.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp755;});void*_tmp23E[1U];_tmp23E[0]=& _tmp240;({struct _fat_ptr _tmp816=({const char*_tmp23F="get_member_offset %s failed";_tag_fat(_tmp23F,sizeof(char),28U);});Cyc_aprintf(_tmp816,_tag_fat(_tmp23E,sizeof(void*),1U));});});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp817,_tag_fat(_tmp23D,sizeof(void*),0U));});});}struct _tuple32{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1391
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple32*pr){
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
void*_tmp242=e1->r;void*_tmp243=_tmp242;struct Cyc_Absyn_Exp*_tmp24B;struct _fat_ptr*_tmp24A;int _tmp249;int _tmp248;void*_tmp247;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Stmt*_tmp245;switch(*((int*)_tmp243)){case 37U: _LL1: _tmp245=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp245,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp247=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_tmp246=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp243)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp246,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp24B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f1;_tmp24A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f2;_tmp249=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f3;_tmp248=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp243)->f4;_LL6:
# 1417
 e1->r=_tmp24B->r;
({struct Cyc_Absyn_Exp*_tmp81A=e1;struct Cyc_List_List*_tmp819=({struct Cyc_List_List*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244->hd=_tmp24A,_tmp244->tl=fs;_tmp244;});struct Cyc_Absyn_Exp*(*_tmp818)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp81A,_tmp819,_tmp818,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1425
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1427
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp81B=(f(e1_copy,f_env))->r;e1->r=_tmp81B;});
goto _LL0;}}_LL0:;}
# 1433
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1435
void*_tmp24C=s->r;void*_tmp24D=_tmp24C;struct Cyc_Absyn_Stmt*_tmp254;struct Cyc_Absyn_Decl*_tmp253;struct Cyc_Absyn_Stmt*_tmp252;struct Cyc_Absyn_Exp*_tmp251;switch(*((int*)_tmp24D)){case 1U: _LL1: _tmp251=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp24D)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp251,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp253=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp24D)->f1;_tmp252=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp24D)->f2;_LL4:
# 1438
 Cyc_Toc_lvalue_assign_stmt(_tmp252,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp254=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp24D)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp254,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp250=({struct Cyc_String_pa_PrintArg_struct _tmp756;_tmp756.tag=0U,({struct _fat_ptr _tmp81C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp756.f1=_tmp81C;});_tmp756;});void*_tmp24E[1U];_tmp24E[0]=& _tmp250;({struct _fat_ptr _tmp81D=({const char*_tmp24F="lvalue_assign_stmt: %s";_tag_fat(_tmp24F,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp81D,_tag_fat(_tmp24E,sizeof(void*),1U));});});}_LL0:;}
# 1444
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1448
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp255=e->r;void*_tmp256=_tmp255;struct Cyc_Absyn_Stmt*_tmp25D;struct Cyc_Absyn_Exp*_tmp25C;void**_tmp25B;struct Cyc_Absyn_Exp**_tmp25A;switch(*((int*)_tmp256)){case 14U: _LL1: _tmp25B=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_tmp25A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp256)->f2;_LL2:
# 1451
({struct Cyc_Absyn_Exp*_tmp81E=Cyc_Toc_push_address_exp(*_tmp25A);*_tmp25A=_tmp81E;});
({void*_tmp81F=Cyc_Absyn_cstar_type(*_tmp25B,Cyc_Toc_mt_tq);*_tmp25B=_tmp81F;});
return e;case 20U: _LL3: _tmp25C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL4:
# 1455
 return _tmp25C;case 37U: _LL5: _tmp25D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp256)->f1;_LL6:
# 1459
 Cyc_Toc_push_address_stmt(_tmp25D);
return e;default: _LL7: _LL8:
# 1462
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp259=({struct Cyc_String_pa_PrintArg_struct _tmp757;_tmp757.tag=0U,({struct _fat_ptr _tmp820=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp757.f1=_tmp820;});_tmp757;});void*_tmp257[1U];_tmp257[0]=& _tmp259;({struct _fat_ptr _tmp821=({const char*_tmp258="can't take & of exp %s";_tag_fat(_tmp258,sizeof(char),23U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp821,_tag_fat(_tmp257,sizeof(void*),1U));});});}_LL0:;}
# 1466
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp25E=s->r;void*_tmp25F=_tmp25E;struct Cyc_Absyn_Exp**_tmp265;struct Cyc_Absyn_Stmt*_tmp264;struct Cyc_Absyn_Stmt*_tmp263;switch(*((int*)_tmp25F)){case 2U: _LL1: _tmp263=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp25F)->f2;_LL2:
 _tmp264=_tmp263;goto _LL4;case 12U: _LL3: _tmp264=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp25F)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp264);goto _LL0;case 1U: _LL5: _tmp265=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp25F)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp822=Cyc_Toc_push_address_exp(*_tmp265);*_tmp265=_tmp822;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp262=({struct Cyc_String_pa_PrintArg_struct _tmp758;_tmp758.tag=0U,({struct _fat_ptr _tmp823=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp758.f1=_tmp823;});_tmp758;});void*_tmp260[1U];_tmp260[0]=& _tmp262;({struct _fat_ptr _tmp824=({const char*_tmp261="can't take & of stmt %s";_tag_fat(_tmp261,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp824,_tag_fat(_tmp260,sizeof(void*),1U));});});}_LL0:;}
# 1478
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1490
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp266=Cyc_Toc_typ_to_c(elt_type);
void*_tmp267=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp268=Cyc_Absyn_cstar_type(_tmp266,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp269=({struct Cyc_Core_Opt*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->v=_tmp268;_tmp29B;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp26A=e1->r;void*_tmp26B=_tmp26A;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;switch(*((int*)_tmp26B)){case 20U: _LL1: _tmp26F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26B)->f1;_LL2:
# 1498
 if(!is_fat){
_tmp26F=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp26F,0,Cyc_Absyn_Other_coercion,0U);
_tmp26F->topt=fat_ptr_type;
_tmp26F->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1503
Cyc_Toc_exp_to_c(nv,_tmp26F);xinit=_tmp26F;goto _LL0;case 23U: _LL3: _tmp271=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26B)->f1;_tmp270=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp26B)->f2;_LL4:
# 1505
 if(!is_fat){
_tmp271=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp271,0,Cyc_Absyn_Other_coercion,0U);
_tmp271->topt=fat_ptr_type;
_tmp271->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1510
Cyc_Toc_exp_to_c(nv,_tmp271);Cyc_Toc_exp_to_c(nv,_tmp270);
xinit=({struct Cyc_Absyn_Exp*_tmp26C[3U];_tmp26C[0]=_tmp271,({
struct Cyc_Absyn_Exp*_tmp825=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp26C[1]=_tmp825;}),_tmp26C[2]=_tmp270;({struct Cyc_Absyn_Exp*_tmp826=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp826,_tag_fat(_tmp26C,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;default: _LL5: _LL6:
({void*_tmp26D=0U;({struct _fat_ptr _tmp827=({const char*_tmp26E="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp26E,sizeof(char),53U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp827,_tag_fat(_tmp26D,sizeof(void*),0U));});});}_LL0:;}{
# 1516
struct _tuple1*_tmp272=Cyc_Toc_temp_var();
struct _RegionHandle _tmp273=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp273;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp274=({struct Cyc_Absyn_Vardecl*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->sc=Cyc_Absyn_Public,_tmp29A->name=_tmp272,_tmp29A->varloc=0U,_tmp29A->tq=Cyc_Toc_mt_tq,_tmp29A->type=_tmp267,_tmp29A->initializer=xinit,_tmp29A->rgn=0,_tmp29A->attributes=0,_tmp29A->escapes=0;_tmp29A;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp275=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->tag=4U,_tmp299->f1=_tmp274;_tmp299;});
struct Cyc_Absyn_Exp*_tmp276=Cyc_Absyn_varb_exp((void*)_tmp275,0U);
_tmp276->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp277=Cyc_Absyn_deref_exp(_tmp276,0U);
_tmp277->topt=elt_type;
_tmp277->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp277);{
struct _tuple1*_tmp278=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp279=({struct Cyc_Absyn_Vardecl*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->sc=Cyc_Absyn_Public,_tmp298->name=_tmp278,_tmp298->varloc=0U,_tmp298->tq=Cyc_Toc_mt_tq,_tmp298->type=_tmp266,_tmp298->initializer=_tmp277,_tmp298->rgn=0,_tmp298->attributes=0,_tmp298->escapes=0;_tmp298;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp27A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->tag=4U,_tmp297->f1=_tmp279;_tmp297;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp27B=Cyc_Absyn_varb_exp((void*)_tmp27A,0U);
_tmp27B->topt=_tmp277->topt;
z_init=({enum Cyc_Absyn_Primop _tmp829=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp828=_tmp27B;Cyc_Absyn_prim2_exp(_tmp829,_tmp828,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=_tmp27B->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1537
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp27C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp27D=({struct Cyc_Absyn_Vardecl*_tmp296=_cycalloc(sizeof(*_tmp296));_tmp296->sc=Cyc_Absyn_Public,_tmp296->name=_tmp27C,_tmp296->varloc=0U,_tmp296->tq=Cyc_Toc_mt_tq,_tmp296->type=_tmp266,_tmp296->initializer=z_init,_tmp296->rgn=0,_tmp296->attributes=0,_tmp296->escapes=0;_tmp296;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp27E=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->tag=4U,_tmp295->f1=_tmp27D;_tmp295;});
# 1543
struct Cyc_Absyn_Exp*_tmp27F=Cyc_Absyn_varb_exp((void*)_tmp27A,0U);_tmp27F->topt=_tmp277->topt;{
struct Cyc_Absyn_Exp*_tmp280=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp281=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp27F,_tmp280,0U);
_tmp280->topt=Cyc_Absyn_sint_type;
_tmp281->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp281);{
# 1550
struct Cyc_Absyn_Exp*_tmp282=Cyc_Absyn_varb_exp((void*)_tmp27E,0U);_tmp282->topt=_tmp277->topt;{
struct Cyc_Absyn_Exp*_tmp283=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*_tmp284=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp282,_tmp283,0U);
_tmp283->topt=Cyc_Absyn_sint_type;
_tmp284->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp284);{
# 1557
struct Cyc_List_List*_tmp285=({struct Cyc_Absyn_Exp*_tmp294[2U];({struct Cyc_Absyn_Exp*_tmp82B=Cyc_Absyn_varb_exp((void*)_tmp275,0U);_tmp294[0]=_tmp82B;}),({
struct Cyc_Absyn_Exp*_tmp82A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp294[1]=_tmp82A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp294,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp286=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,_tmp285,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp82C,_tmp286,0U);});{
# 1564
struct Cyc_Absyn_Exp*_tmp287=({struct Cyc_Absyn_Exp*_tmp82D=xsize;Cyc_Absyn_and_exp(_tmp82D,Cyc_Absyn_and_exp(_tmp281,_tmp284,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp288=Cyc_Absyn_exp_stmt(({void*_tmp293=0U;({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp82E,_tag_fat(_tmp293,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);
struct Cyc_Absyn_Exp*_tmp289=({struct Cyc_Absyn_Exp*_tmp82F=Cyc_Absyn_varb_exp((void*)_tmp275,0U);Cyc_Toc_member_exp(_tmp82F,Cyc_Toc_curr_sp,0U);});
_tmp289=Cyc_Toc_cast_it(_tmp268,_tmp289);{
struct Cyc_Absyn_Exp*_tmp28A=Cyc_Absyn_deref_exp(_tmp289,0U);
struct Cyc_Absyn_Exp*_tmp28B=({struct Cyc_Absyn_Exp*_tmp830=_tmp28A;Cyc_Absyn_assign_exp(_tmp830,Cyc_Absyn_var_exp(_tmp27C,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp28C=Cyc_Absyn_exp_stmt(_tmp28B,0U);
_tmp28C=({struct Cyc_Absyn_Stmt*_tmp833=({struct Cyc_Absyn_Exp*_tmp832=_tmp287;struct Cyc_Absyn_Stmt*_tmp831=_tmp288;Cyc_Absyn_ifthenelse_stmt(_tmp832,_tmp831,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp833,_tmp28C,0U);});
_tmp28C=({struct Cyc_Absyn_Decl*_tmp835=({struct Cyc_Absyn_Decl*_tmp28E=_cycalloc(sizeof(*_tmp28E));({void*_tmp834=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D->tag=0U,_tmp28D->f1=_tmp27D;_tmp28D;});_tmp28E->r=_tmp834;}),_tmp28E->loc=0U;_tmp28E;});Cyc_Absyn_decl_stmt(_tmp835,_tmp28C,0U);});
_tmp28C=({struct Cyc_Absyn_Decl*_tmp837=({struct Cyc_Absyn_Decl*_tmp290=_cycalloc(sizeof(*_tmp290));({void*_tmp836=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->tag=0U,_tmp28F->f1=_tmp279;_tmp28F;});_tmp290->r=_tmp836;}),_tmp290->loc=0U;_tmp290;});Cyc_Absyn_decl_stmt(_tmp837,_tmp28C,0U);});
_tmp28C=({struct Cyc_Absyn_Decl*_tmp839=({struct Cyc_Absyn_Decl*_tmp292=_cycalloc(sizeof(*_tmp292));({void*_tmp838=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->tag=0U,_tmp291->f1=_tmp274;_tmp291;});_tmp292->r=_tmp838;}),_tmp292->loc=0U;_tmp292;});Cyc_Absyn_decl_stmt(_tmp839,_tmp28C,0U);});
({void*_tmp83A=Cyc_Toc_stmt_exp_r(_tmp28C);e->r=_tmp83A;});};};};};};};};};};}
# 1518
;_pop_region(rgn2);};}
# 1590 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1594
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp29C=Cyc_Tcutil_compress(aggrtype);void*_tmp29D=_tmp29C;union Cyc_Absyn_AggrInfo _tmp2A2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29D)->f1)->tag == 20U){_LL1: _tmp2A2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29D)->f1)->f1;_LL2:
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp2A2);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp29E=0U;({struct _fat_ptr _tmp83D=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A1=({struct Cyc_String_pa_PrintArg_struct _tmp759;_tmp759.tag=0U,({
struct _fat_ptr _tmp83B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp759.f1=_tmp83B;});_tmp759;});void*_tmp29F[1U];_tmp29F[0]=& _tmp2A1;({struct _fat_ptr _tmp83C=({const char*_tmp2A0="expecting union but found %s in check_tagged_union";_tag_fat(_tmp2A0,sizeof(char),51U);});Cyc_aprintf(_tmp83C,_tag_fat(_tmp29F,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp83D,_tag_fat(_tmp29E,sizeof(void*),0U));});});}_LL0:;}{
# 1600
struct _tuple30 _tmp2A3=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp2A4=_tmp2A3;struct _tuple1*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;_LL6: _tmp2B5=_tmp2A4.f1;_tmp2B4=_tmp2A4.f2;_LL7:;{
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Absyn_aggrarrow_exp(_tmp2B4,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_neq_exp(_tmp2A6,_tmp2A5,0U);
struct Cyc_Absyn_Exp*_tmp2A8=Cyc_Absyn_aggrarrow_exp(_tmp2B4,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2A9=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2A8,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2AA=({struct Cyc_Absyn_Exp*_tmp83F=_tmp2A7;struct Cyc_Absyn_Stmt*_tmp83E=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp83F,_tmp83E,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2AB=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2AD=({struct _tuple1*_tmp842=_tmp2B5;void*_tmp841=_tmp2AB;struct Cyc_Absyn_Exp*_tmp840=_tmp2AC;Cyc_Absyn_declare_stmt(_tmp842,_tmp841,_tmp840,Cyc_Absyn_seq_stmt(_tmp2AA,_tmp2A9,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2AD);}else{
# 1613
struct Cyc_Absyn_Exp*_tmp2AE=({struct Cyc_Absyn_Exp*_tmp843=aggrproj(_tmp2B4,f,0U);Cyc_Toc_member_exp(_tmp843,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2AF=Cyc_Absyn_neq_exp(_tmp2AE,_tmp2A5,0U);
struct Cyc_Absyn_Exp*_tmp2B0=({struct Cyc_Absyn_Exp*_tmp844=aggrproj(_tmp2B4,f,0U);Cyc_Toc_member_exp(_tmp844,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2B1=Cyc_Absyn_exp_stmt(_tmp2B0,0U);
struct Cyc_Absyn_Stmt*_tmp2B2=({struct Cyc_Absyn_Exp*_tmp846=_tmp2AF;struct Cyc_Absyn_Stmt*_tmp845=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp846,_tmp845,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2B3=({struct _tuple1*_tmp849=_tmp2B5;void*_tmp848=e1_c_type;struct Cyc_Absyn_Exp*_tmp847=e1;Cyc_Absyn_declare_stmt(_tmp849,_tmp848,_tmp847,Cyc_Absyn_seq_stmt(_tmp2B2,_tmp2B1,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2B3);}};};}
# 1623
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1626
void*_tmp2B6=Cyc_Tcutil_compress(t);void*_tmp2B7=_tmp2B6;union Cyc_Absyn_AggrInfo _tmp2BE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f1)->tag == 20U){_LL1: _tmp2BE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f1)->f1;_LL2: {
# 1628
struct Cyc_Absyn_Aggrdecl*_tmp2B8=Cyc_Absyn_get_known_aggrdecl(_tmp2BE);
({int _tmp84A=Cyc_Toc_get_member_offset(_tmp2B8,f);*f_tag=_tmp84A;});{
# 1631
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp75B;_tmp75B.tag=0U,_tmp75B.f1=(struct _fat_ptr)((struct _fat_ptr)*(*_tmp2B8->name).f2);_tmp75B;});struct Cyc_String_pa_PrintArg_struct _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp75A;_tmp75A.tag=0U,_tmp75A.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp75A;});void*_tmp2BA[2U];_tmp2BA[0]=& _tmp2BC,_tmp2BA[1]=& _tmp2BD;({struct _fat_ptr _tmp84B=({const char*_tmp2BB="_union_%s_%s";_tag_fat(_tmp2BB,sizeof(char),13U);});Cyc_aprintf(_tmp84B,_tag_fat(_tmp2BA,sizeof(void*),2U));});});
({void*_tmp84C=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));*_tmp2B9=str;_tmp2B9;}));*tagged_member_type=_tmp84C;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B8->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1642
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1645
void*_tmp2BF=e->r;void*_tmp2C0=_tmp2BF;struct Cyc_Absyn_Exp*_tmp2CC;struct _fat_ptr*_tmp2CB;int*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;struct _fat_ptr*_tmp2C8;int*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;switch(*((int*)_tmp2C0)){case 14U: _LL1: _tmp2C6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C0)->f2;_LL2:
({void*_tmp2C1=0U;({struct _fat_ptr _tmp84D=({const char*_tmp2C2="cast on lhs!";_tag_fat(_tmp2C2,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp84D,_tag_fat(_tmp2C1,sizeof(void*),0U));});});case 21U: _LL3: _tmp2C9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C0)->f1;_tmp2C8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C0)->f2;_tmp2C7=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C0)->f4;_LL4:
# 1648
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2C9->topt),_tmp2C8,f_tag,tagged_member_type,clear_read,_tmp2C7);case 22U: _LL5: _tmp2CC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C0)->f1;_tmp2CB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C0)->f2;_tmp2CA=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C0)->f4;_LL6: {
# 1651
void*_tmp2C3=Cyc_Tcutil_compress((void*)_check_null(_tmp2CC->topt));void*_tmp2C4=_tmp2C3;void*_tmp2C5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4)->tag == 3U){_LLA: _tmp2C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4)->f1).elt_type;_LLB:
# 1653
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2C5,_tmp2CB,f_tag,tagged_member_type,clear_read,_tmp2CA);}else{_LLC: _LLD:
# 1655
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1657
 return 0;}_LL0:;}
# 1670 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1674
struct _tuple30 _tmp2CD=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp2CE=_tmp2CD;struct _tuple1*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;_LL1: _tmp2D1=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;_LL2:;{
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(_tmp2D0,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(_tmp2D0,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1683
struct Cyc_Absyn_Exp*_tmp2CF=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
s2=({struct Cyc_Absyn_Exp*_tmp84F=_tmp2CF;struct Cyc_Absyn_Stmt*_tmp84E=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp84F,_tmp84E,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1686
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp852=_tmp2D1;void*_tmp851=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp850=
Cyc_Toc_push_address_exp(e1);
# 1686
Cyc_Absyn_declare_stmt(_tmp852,_tmp851,_tmp850,
# 1688
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};};}struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1692
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp2D4=({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0U,({struct _fat_ptr _tmp853=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp75C.f1=_tmp853;});_tmp75C;});void*_tmp2D2[1U];_tmp2D2[0]=& _tmp2D4;({unsigned _tmp855=e->loc;struct _fat_ptr _tmp854=({const char*_tmp2D3="exp_to_c: no type for %s";_tag_fat(_tmp2D3,sizeof(char),25U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp855,_tmp854,_tag_fat(_tmp2D2,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1697
int did_inserted_checks=0;
{void*_tmp2D5=e->annot;void*_tmp2D6=_tmp2D5;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D6)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2D6)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1703
{void*_tmp2D7=e->r;void*_tmp2D8=_tmp2D7;struct Cyc_Absyn_Stmt*_tmp47C;struct Cyc_Absyn_Exp*_tmp47B;struct _fat_ptr*_tmp47A;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*_tmp478;int _tmp477;struct Cyc_Absyn_Exp*_tmp476;void**_tmp475;struct Cyc_Absyn_Exp*_tmp474;int _tmp473;int _tmp472;struct Cyc_List_List*_tmp471;struct Cyc_Absyn_Datatypedecl*_tmp470;struct Cyc_Absyn_Datatypefield*_tmp46F;struct _tuple1**_tmp46E;struct Cyc_List_List*_tmp46D;struct Cyc_List_List*_tmp46C;struct Cyc_Absyn_Aggrdecl*_tmp46B;void*_tmp46A;struct Cyc_List_List*_tmp469;struct Cyc_Absyn_Exp*_tmp468;void*_tmp467;int _tmp466;struct Cyc_Absyn_Vardecl*_tmp465;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Exp*_tmp463;int _tmp462;struct Cyc_List_List*_tmp461;struct Cyc_List_List*_tmp460;struct Cyc_Absyn_Exp*_tmp45F;struct Cyc_Absyn_Exp*_tmp45E;struct Cyc_Absyn_Exp*_tmp45D;struct Cyc_Absyn_Exp*_tmp45C;struct _fat_ptr*_tmp45B;int _tmp45A;int _tmp459;struct Cyc_Absyn_Exp*_tmp458;struct _fat_ptr*_tmp457;int _tmp456;int _tmp455;void*_tmp454;struct Cyc_List_List*_tmp453;void*_tmp452;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;void**_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;int _tmp44B;enum Cyc_Absyn_Coercion _tmp44A;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_List_List*_tmp448;struct Cyc_Absyn_Exp*_tmp447;struct Cyc_Absyn_Exp*_tmp446;int _tmp445;struct Cyc_Absyn_Exp*_tmp444;struct Cyc_List_List*_tmp443;int _tmp442;struct Cyc_List_List*_tmp441;struct Cyc_Absyn_VarargInfo*_tmp440;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_List_List*_tmp43E;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*_tmp439;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp*_tmp437;struct Cyc_Absyn_Exp*_tmp436;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Exp*_tmp434;struct Cyc_Core_Opt*_tmp433;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Exp*_tmp431;enum Cyc_Absyn_Incrementor _tmp430;enum Cyc_Absyn_Primop _tmp42F;struct Cyc_List_List*_tmp42E;struct Cyc_Absyn_Exp*_tmp42D;switch(*((int*)_tmp2D8)){case 2U: _LL8: _LL9:
# 1705
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1712
struct Cyc_Absyn_Exp*_tmp2D9=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp856=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp2D9,_tmp2D9))->r;e->r=_tmp856;});else{
# 1717
({void*_tmp858=({struct Cyc_Absyn_Exp*_tmp2DA[3U];_tmp2DA[0]=_tmp2D9,_tmp2DA[1]=_tmp2D9,_tmp2DA[2]=_tmp2D9;({struct Cyc_Absyn_Exp*_tmp857=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp857,_tag_fat(_tmp2DA,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp858;});}}else{
# 1719
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1721
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 41U: _LL10: _tmp42D=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL11:
 Cyc_Toc_exp_to_c(nv,_tmp42D);goto _LL7;case 3U: _LL12: _tmp42F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp42E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL13: {
# 1727
struct Cyc_List_List*_tmp2DB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp42E);
# 1729
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp42E);
{enum Cyc_Absyn_Primop _tmp2DC=_tmp42F;switch(_tmp2DC){case Cyc_Absyn_Numelts: _LL61: _LL62: {
# 1732
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E))->hd;
{void*_tmp2DD=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp2DE=_tmp2DD;void*_tmp2EB;void*_tmp2EA;void*_tmp2E9;void*_tmp2E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->tag == 3U){_LL76: _tmp2EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->f1).elt_type;_tmp2EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->f1).ptr_atts).nullable;_tmp2E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->f1).ptr_atts).bounds;_tmp2E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->f1).ptr_atts).zero_term;_LL77: {
# 1735
struct Cyc_Absyn_Exp*_tmp2DF=({void*_tmp859=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp859,_tmp2E9);});
if(_tmp2DF == 0)
# 1738
({void*_tmp85C=({struct Cyc_Absyn_Exp*_tmp2E0[2U];_tmp2E0[0]=(struct Cyc_Absyn_Exp*)_tmp42E->hd,({
struct Cyc_Absyn_Exp*_tmp85A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp2EB),0U);_tmp2E0[1]=_tmp85A;});({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp85B,_tag_fat(_tmp2E0,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1738
e->r=_tmp85C;});else{
# 1741
struct Cyc_Absyn_Exp*_tmp2E1=_tmp2DF;
# 1743
if(Cyc_Tcutil_force_type2bool(0,_tmp2E8)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp42E->hd);
# 1747
({void*_tmp85E=({struct Cyc_Absyn_Exp*_tmp2E2[2U];_tmp2E2[0]=(struct Cyc_Absyn_Exp*)_tmp42E->hd,_tmp2E2[1]=_tmp2E1;({struct Cyc_Absyn_Exp*_tmp85D=function_e;Cyc_Toc_fncall_exp_r(_tmp85D,_tag_fat(_tmp2E2,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp85E;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp2EA)){
if(!Cyc_Evexp_c_can_eval(_tmp2E1))
({void*_tmp2E3=0U;({unsigned _tmp860=e->loc;struct _fat_ptr _tmp85F=({const char*_tmp2E4="can't calculate numelts";_tag_fat(_tmp2E4,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp860,_tmp85F,_tag_fat(_tmp2E3,sizeof(void*),0U));});});
# 1752
({void*_tmp863=({struct Cyc_Absyn_Exp*_tmp862=arg;struct Cyc_Absyn_Exp*_tmp861=_tmp2E1;Cyc_Toc_conditional_exp_r(_tmp862,_tmp861,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp863;});}else{
# 1754
e->r=_tmp2E1->r;goto _LL75;}}}
# 1757
goto _LL75;}}else{_LL78: _LL79:
({struct Cyc_String_pa_PrintArg_struct _tmp2E7=({struct Cyc_String_pa_PrintArg_struct _tmp75D;_tmp75D.tag=0U,({
struct _fat_ptr _tmp864=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp75D.f1=_tmp864;});_tmp75D;});void*_tmp2E5[1U];_tmp2E5[0]=& _tmp2E7;({struct _fat_ptr _tmp865=({const char*_tmp2E6="numelts primop applied to non-pointer %s";_tag_fat(_tmp2E6,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp865,_tag_fat(_tmp2E5,sizeof(void*),1U));});});}_LL75:;}
# 1761
goto _LL60;}case Cyc_Absyn_Plus: _LL63: _LL64:
# 1766
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp2EC=0U;({struct _fat_ptr _tmp866=({const char*_tmp2ED="can't do pointer arithmetic at top-level";_tag_fat(_tmp2ED,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_unimp)(_tmp866,_tag_fat(_tmp2EC,sizeof(void*),0U));});});
{void*_tmp2EE=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp2DB))->hd);void*_tmp2EF=_tmp2EE;void*_tmp2F5;void*_tmp2F4;void*_tmp2F3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF)->tag == 3U){_LL7B: _tmp2F5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF)->f1).elt_type;_tmp2F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF)->f1).ptr_atts).bounds;_tmp2F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF)->f1).ptr_atts).zero_term;_LL7C: {
# 1770
struct Cyc_Absyn_Exp*_tmp2F0=({void*_tmp867=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp867,_tmp2F4);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E->tl))->hd;
if(_tmp2F0 == 0)
({void*_tmp86A=({struct Cyc_Absyn_Exp*_tmp2F1[3U];_tmp2F1[0]=e1,({
struct Cyc_Absyn_Exp*_tmp868=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp2F5),0U);_tmp2F1[1]=_tmp868;}),_tmp2F1[2]=e2;({struct Cyc_Absyn_Exp*_tmp869=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp869,_tag_fat(_tmp2F1,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1774
e->r=_tmp86A;});else{
# 1776
if(Cyc_Tcutil_force_type2bool(0,_tmp2F3))
({void*_tmp86C=({struct Cyc_Absyn_Exp*_tmp2F2[3U];_tmp2F2[0]=e1,_tmp2F2[1]=_tmp2F0,_tmp2F2[2]=e2;({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp86B,_tag_fat(_tmp2F2,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp86C;});}
goto _LL7A;}}else{_LL7D: _LL7E:
 goto _LL7A;}_LL7A:;}
# 1781
goto _LL60;case Cyc_Absyn_Minus: _LL65: _LL66: {
# 1786
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp2DB))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp2DB->tl))->hd)){
({void*_tmp86E=({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp86D,Cyc_Toc_curr_sp);});e1->r=_tmp86E;});
({void*_tmp870=({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp86F,Cyc_Toc_curr_sp);});e2->r=_tmp870;});
({void*_tmp872=({void*_tmp871=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp871;});e1->topt=_tmp872;});
({void*_tmp874=({struct Cyc_Absyn_Exp*_tmp873=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp873,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1794
e->r=_tmp874;});}else{
# 1797
({void*_tmp878=({struct Cyc_Absyn_Exp*_tmp2F6[3U];_tmp2F6[0]=e1,({
struct Cyc_Absyn_Exp*_tmp876=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2F6[1]=_tmp876;}),({
struct Cyc_Absyn_Exp*_tmp875=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp2F6[2]=_tmp875;});({struct Cyc_Absyn_Exp*_tmp877=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp877,_tag_fat(_tmp2F6,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1797
e->r=_tmp878;});}}
# 1801
goto _LL60;}case Cyc_Absyn_Eq: _LL67: _LL68:
 goto _LL6A;case Cyc_Absyn_Neq: _LL69: _LL6A: goto _LL6C;case Cyc_Absyn_Gt: _LL6B: _LL6C: goto _LL6E;case Cyc_Absyn_Gte: _LL6D: _LL6E: goto _LL70;case Cyc_Absyn_Lt: _LL6F: _LL70: goto _LL72;case Cyc_Absyn_Lte: _LL71: _LL72: {
# 1804
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp42E->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp2DB))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp2DB->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp2F7=({void*_tmp879=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp879,Cyc_Toc_mt_tq);});
({void*_tmp87C=({void*_tmp87B=_tmp2F7;Cyc_Toc_cast_it_r(_tmp87B,({struct Cyc_Absyn_Exp*_tmp87A=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp87A,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp87C;});
e1->topt=_tmp2F7;}
# 1814
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp2F8=({void*_tmp87D=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp87D,Cyc_Toc_mt_tq);});
({void*_tmp880=({void*_tmp87F=_tmp2F8;Cyc_Toc_cast_it_r(_tmp87F,({struct Cyc_Absyn_Exp*_tmp87E=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp87E,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp880;});
e2->topt=_tmp2F8;}
# 1819
goto _LL60;}default: _LL73: _LL74:
 goto _LL60;}_LL60:;}
# 1822
goto _LL7;}case 5U: _LL14: _tmp431=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp430=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL15: {
# 1824
void*e2_cyc_typ=(void*)_check_null(_tmp431->topt);
# 1833 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp30C="increment";_tag_fat(_tmp30C,sizeof(char),10U);});
if((int)_tmp430 == (int)2U ||(int)_tmp430 == (int)3U)incr_str=({const char*_tmp2F9="decrement";_tag_fat(_tmp2F9,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp431,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp2FC=({struct Cyc_String_pa_PrintArg_struct _tmp75E;_tmp75E.tag=0U,_tmp75E.f1=(struct _fat_ptr)((struct _fat_ptr)incr_str);_tmp75E;});void*_tmp2FA[1U];_tmp2FA[0]=& _tmp2FC;({unsigned _tmp882=e->loc;struct _fat_ptr _tmp881=({const char*_tmp2FB="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_fat(_tmp2FB,sizeof(char),74U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp882,_tmp881,_tag_fat(_tmp2FA,sizeof(void*),1U));});});
# 1841
if(Cyc_Toc_is_tagged_union_project(_tmp431,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp2FD=_tmp430;switch(_tmp2FD){case Cyc_Absyn_PreInc: _LL80: _LL81:
 op=0U;goto _LL7F;case Cyc_Absyn_PreDec: _LL82: _LL83:
 op=2U;goto _LL7F;default: _LL84: _LL85:
({struct Cyc_String_pa_PrintArg_struct _tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp75F;_tmp75F.tag=0U,_tmp75F.f1=(struct _fat_ptr)((struct _fat_ptr)incr_str);_tmp75F;});void*_tmp2FE[1U];_tmp2FE[0]=& _tmp300;({unsigned _tmp884=e->loc;struct _fat_ptr _tmp883=({const char*_tmp2FF="in-place post%s is not supported on @tagged union members";_tag_fat(_tmp2FF,sizeof(char),58U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp884,_tmp883,_tag_fat(_tmp2FE,sizeof(void*),1U));});});}_LL7F:;}
# 1851
({void*_tmp886=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302->tag=4U,_tmp302->f1=_tmp431,({struct Cyc_Core_Opt*_tmp885=({struct Cyc_Core_Opt*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->v=(void*)op;_tmp301;});_tmp302->f2=_tmp885;}),_tmp302->f3=one;_tmp302;});e->r=_tmp886;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1855
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp431);
Cyc_Toc_set_lhs(nv,0);{
# 1860
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)_tmp430 == (int)1U ||(int)_tmp430 == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1866
if((int)_tmp430 == (int)2U ||(int)_tmp430 == (int)3U)
change=-1;
({void*_tmp88B=({struct Cyc_Absyn_Exp*_tmp303[3U];({struct Cyc_Absyn_Exp*_tmp889=Cyc_Toc_push_address_exp(_tmp431);_tmp303[0]=_tmp889;}),({
struct Cyc_Absyn_Exp*_tmp888=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp303[1]=_tmp888;}),({
struct Cyc_Absyn_Exp*_tmp887=Cyc_Absyn_signed_int_exp(change,0U);_tmp303[2]=_tmp887;});({struct Cyc_Absyn_Exp*_tmp88A=fn_e;Cyc_Toc_fncall_exp_r(_tmp88A,_tag_fat(_tmp303,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1868
e->r=_tmp88B;});}else{
# 1871
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1877
did_inserted_checks=1;
if((int)_tmp430 != (int)1U){
struct _tuple1*_tmp304=Cyc_Toc_temp_var();
void*_tmp305=({void*_tmp88C=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp88C,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp306=Cyc_Toc_push_address_exp(_tmp431);
struct Cyc_Absyn_Exp*_tmp307=({struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp304,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp88D,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
_tmp307=({struct Cyc_Absyn_Exp*_tmp88F=({struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp304,0U),0U);Cyc_Absyn_neq_exp(_tmp88E,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1885
Cyc_Absyn_and_exp(_tmp88F,_tmp307,0U);});{
# 1887
struct Cyc_Absyn_Stmt*_tmp308=({struct Cyc_Absyn_Exp*_tmp892=_tmp307;struct Cyc_Absyn_Stmt*_tmp891=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp304,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1887
Cyc_Absyn_ifthenelse_stmt(_tmp892,_tmp891,
# 1889
Cyc_Absyn_exp_stmt(({void*_tmp309=0U;({struct Cyc_Absyn_Exp*_tmp890=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp890,_tag_fat(_tmp309,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
_tmp308=({struct Cyc_Absyn_Stmt*_tmp893=_tmp308;Cyc_Absyn_seq_stmt(_tmp893,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp304,0U),0U),0U),0U);});
({void*_tmp894=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp304,_tmp305,_tmp306,_tmp308,0U),0U))->r;e->r=_tmp894;});};}else{
# 1894
struct Cyc_Toc_functionSet*_tmp30A=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp30A,_tmp431);
({void*_tmp898=({struct Cyc_Absyn_Exp*_tmp30B[2U];({struct Cyc_Absyn_Exp*_tmp896=Cyc_Toc_push_address_exp(_tmp431);_tmp30B[0]=_tmp896;}),({struct Cyc_Absyn_Exp*_tmp895=Cyc_Absyn_signed_int_exp(1,0U);_tmp30B[1]=_tmp895;});({struct Cyc_Absyn_Exp*_tmp897=fn_e;Cyc_Toc_fncall_exp_r(_tmp897,_tag_fat(_tmp30B,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp898;});}}else{
# 1898
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(_tmp431)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp431,0,Cyc_Toc_incr_lvalue,_tmp430);
e->r=_tmp431->r;}}}
# 1902
goto _LL7;};}case 4U: _LL16: _tmp434=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp433=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp432=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL17: {
# 1921 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp434->topt);
void*e2_old_typ=(void*)_check_null(_tmp432->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp434,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp434);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp432);
({void*_tmp899=Cyc_Toc_tagged_union_assignop(_tmp434,e1_old_typ,_tmp433,_tmp432,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp899;});
# 1932
goto _LL7;}{
# 1934
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp434,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp434,_tmp433,_tmp432,ptr_type,is_fat,elt_type);
# 1940
return;}{
# 1944
int e1_poly=Cyc_Toc_is_poly_project(_tmp434);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp434);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp432);{
# 1950
int done=0;
if(_tmp433 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp30D=(enum Cyc_Absyn_Primop)_tmp433->v;enum Cyc_Absyn_Primop _tmp30E=_tmp30D;switch(_tmp30E){case Cyc_Absyn_Plus: _LL87: _LL88:
 change=_tmp432;goto _LL86;case Cyc_Absyn_Minus: _LL89: _LL8A:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp432,0U);goto _LL86;default: _LL8B: _LL8C:
({void*_tmp30F=0U;({struct _fat_ptr _tmp89A=({const char*_tmp310="bad t ? pointer arithmetic";_tag_fat(_tmp310,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp89A,_tag_fat(_tmp30F,sizeof(void*),0U));});});}_LL86:;}
# 1960
done=1;{
# 1962
struct Cyc_Absyn_Exp*_tmp311=Cyc_Toc__fat_ptr_inplace_plus_e;
({void*_tmp89E=({struct Cyc_Absyn_Exp*_tmp312[3U];({struct Cyc_Absyn_Exp*_tmp89C=Cyc_Toc_push_address_exp(_tmp434);_tmp312[0]=_tmp89C;}),({
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp312[1]=_tmp89B;}),_tmp312[2]=change;({struct Cyc_Absyn_Exp*_tmp89D=_tmp311;Cyc_Toc_fncall_exp_r(_tmp89D,_tag_fat(_tmp312,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1963
e->r=_tmp89E;});};}else{
# 1966
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1969
enum Cyc_Absyn_Primop _tmp313=(enum Cyc_Absyn_Primop)_tmp433->v;enum Cyc_Absyn_Primop _tmp314=_tmp313;if(_tmp314 == Cyc_Absyn_Plus){_LL8E: _LL8F:
# 1971
 done=1;
({void*_tmp8A0=({struct Cyc_Absyn_Exp*_tmp315[2U];_tmp315[0]=_tmp434,_tmp315[1]=_tmp432;({struct Cyc_Absyn_Exp*_tmp89F=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp434);Cyc_Toc_fncall_exp_r(_tmp89F,_tag_fat(_tmp315,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8A0;});
goto _LL8D;}else{_LL90: _LL91:
({void*_tmp316=0U;({struct _fat_ptr _tmp8A1=({const char*_tmp317="bad zero-terminated pointer arithmetic";_tag_fat(_tmp317,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp8A1,_tag_fat(_tmp316,sizeof(void*),0U));});});}_LL8D:;}}}
# 1978
if(!done){
# 1980
if(e1_poly)
({void*_tmp8A3=({void*_tmp8A2=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp8A2,Cyc_Absyn_new_exp(_tmp432->r,0U));});_tmp432->r=_tmp8A3;});
# 1986
if(!Cyc_Absyn_is_lvalue(_tmp434)){
({struct Cyc_Absyn_Exp*_tmp8A4=_tmp434;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple32*),struct _tuple32*f_env))Cyc_Toc_lvalue_assign)(_tmp8A4,0,Cyc_Toc_assignop_lvalue,({struct _tuple32*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->f1=_tmp433,_tmp318->f2=_tmp432;_tmp318;}));});
e->r=_tmp434->r;}}
# 1991
goto _LL7;};};};}case 6U: _LL18: _tmp437=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp436=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp435=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL19:
# 1993
 Cyc_Toc_exp_to_c(nv,_tmp437);
Cyc_Toc_exp_to_c(nv,_tmp436);
Cyc_Toc_exp_to_c(nv,_tmp435);
goto _LL7;case 7U: _LL1A: _tmp439=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp438=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL1B:
 _tmp43B=_tmp439;_tmp43A=_tmp438;goto _LL1D;case 8U: _LL1C: _tmp43B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp43A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL1D:
 _tmp43D=_tmp43B;_tmp43C=_tmp43A;goto _LL1F;case 9U: _LL1E: _tmp43D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp43C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL1F:
# 2000
 Cyc_Toc_exp_to_c(nv,_tmp43D);
Cyc_Toc_exp_to_c(nv,_tmp43C);
goto _LL7;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3 == 0){_LL20: _tmp43F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp43E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL21: {
# 2004
void*e1_typ=(void*)_check_null(_tmp43F->topt);
Cyc_Toc_exp_to_c(nv,_tmp43F);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2009
({void*_tmp8A8=({void*_tmp8A7=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8A7,({struct Cyc_Absyn_Exp*_tmp319[1U];({struct Cyc_Absyn_Exp*_tmp8A5=
Cyc_Absyn_copy_exp(_tmp43F);_tmp319[0]=_tmp8A5;});({struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp8A6,_tag_fat(_tmp319,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2009
_tmp43F->r=_tmp8A8;});
# 2011
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp43E);
goto _LL7;}}else{_LL22: _tmp444=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp443=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp442=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3)->num_varargs;_tmp441=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3)->injectors;_tmp440=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3)->vai;_LL23: {
# 2022 "toc.cyc"
struct _RegionHandle _tmp31A=_new_region("r");struct _RegionHandle*r=& _tmp31A;_push_region(r);{
struct _tuple30 _tmp31B=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp31C=_tmp31B;struct _tuple1*_tmp335;struct Cyc_Absyn_Exp*_tmp334;_LL93: _tmp335=_tmp31C.f1;_tmp334=_tmp31C.f2;_LL94:;{
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)_tmp442,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp440->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2029
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp443);
int num_normargs=num_args - _tmp442;
# 2033
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp443=_tmp443->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp443))->hd);
new_args=({struct Cyc_List_List*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->hd=(struct Cyc_Absyn_Exp*)_tmp443->hd,_tmp31D->tl=new_args;_tmp31D;});}}
# 2038
new_args=({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(*_tmp31F));({struct Cyc_Absyn_Exp*_tmp8AB=({struct Cyc_Absyn_Exp*_tmp31E[3U];_tmp31E[0]=_tmp334,({
# 2040
struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp31E[1]=_tmp8A9;}),_tmp31E[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp8AA,_tag_fat(_tmp31E,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2038
_tmp31F->hd=_tmp8AB;}),_tmp31F->tl=new_args;_tmp31F;});
# 2043
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2045
void*e1_typ=(void*)_check_null(_tmp444->topt);
Cyc_Toc_exp_to_c(nv,_tmp444);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2050
({void*_tmp8AF=({void*_tmp8AE=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8AE,({struct Cyc_Absyn_Exp*_tmp320[1U];({struct Cyc_Absyn_Exp*_tmp8AC=
Cyc_Absyn_copy_exp(_tmp444);_tmp320[0]=_tmp8AC;});({struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp8AD,_tag_fat(_tmp320,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2050
_tmp444->r=_tmp8AF;});{
# 2052
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp444,new_args,0U),0U);
# 2055
if(_tmp440->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp321=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp440->type));void*_tmp322=_tmp321;struct Cyc_Absyn_Datatypedecl*_tmp325;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp322)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp322)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp322)->f1)->f1).KnownDatatype).tag == 2){_LL96: _tmp325=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp322)->f1)->f1).KnownDatatype).val;_LL97:
 tud=_tmp325;goto _LL95;}else{goto _LL98;}}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp323=0U;({struct _fat_ptr _tmp8B0=({const char*_tmp324="toc: unknown datatype in vararg with inject";_tag_fat(_tmp324,sizeof(char),44U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp8B0,_tag_fat(_tmp323,sizeof(void*),0U));});});}_LL95:;}{
# 2061
struct _fat_ptr vs=({unsigned _tmp331=(unsigned)_tmp442;struct _tuple1**_tmp330=({struct _RegionHandle*_tmp8B1=r;_region_malloc(_tmp8B1,_check_times(_tmp331,sizeof(struct _tuple1*)));});({{unsigned _tmp760=(unsigned)_tmp442;unsigned i;for(i=0;i < _tmp760;++ i){({struct _tuple1*_tmp8B2=Cyc_Toc_temp_var();_tmp330[i]=_tmp8B2;});}}0;});_tag_fat(_tmp330,sizeof(struct _tuple1*),_tmp331);});
# 2063
if(_tmp442 != 0){
# 2067
struct Cyc_List_List*_tmp326=0;
{int i=_tmp442 - 1;for(0;i >= 0;-- i){
_tmp326=({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));({struct Cyc_Absyn_Exp*_tmp8B3=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp327->hd=_tmp8B3;}),_tmp327->tl=_tmp326;_tmp327;});}}
s=({struct _tuple1*_tmp8B6=_tmp335;void*_tmp8B5=arr_type;struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_array_exp(_tmp326,0U);Cyc_Absyn_declare_stmt(_tmp8B6,_tmp8B5,_tmp8B4,s,0U);});
# 2072
_tmp443=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp443);
_tmp441=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp441);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp443)- 1;
for(0;_tmp443 != 0;(_tmp443=_tmp443->tl,_tmp441=_tmp441->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp443->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp328=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp441))->hd;struct Cyc_Absyn_Datatypefield*_tmp329=_tmp328;struct _tuple1*_tmp32F;struct Cyc_List_List*_tmp32E;_LL9B: _tmp32F=_tmp329->name;_tmp32E=_tmp329->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple13*)((struct Cyc_List_List*)_check_null(_tmp32E))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2086
struct _tuple1*_tmp32A=Cyc_Toc_collapse_qvars(_tmp32F,tud->name);
struct Cyc_List_List*_tmp32B=({struct _tuple20*_tmp32D[2U];({struct _tuple20*_tmp8BA=({struct _fat_ptr*_tmp8B9=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp8B9,Cyc_Toc_datatype_tag(tud,_tmp32F));});_tmp32D[0]=_tmp8BA;}),({
struct _tuple20*_tmp8B8=({struct _fat_ptr*_tmp8B7=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8B7,arg);});_tmp32D[1]=_tmp8B8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp32D,sizeof(struct _tuple20*),2U));});
s=({struct _tuple1*_tmp8BD=var;void*_tmp8BC=Cyc_Absyn_strctq(_tmp32A);struct Cyc_Absyn_Exp*_tmp8BB=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->tag=29U,_tmp32C->f1=_tmp32A,_tmp32C->f2=0,_tmp32C->f3=_tmp32B,_tmp32C->f4=0;_tmp32C;}),0U);
# 2089
Cyc_Absyn_declare_stmt(_tmp8BD,_tmp8BC,_tmp8BB,s,0U);});};};}};}else{
# 2097
s=({struct _tuple1*_tmp8C0=_tmp335;void*_tmp8BF=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8C0,_tmp8BF,_tmp8BE,s,0U);});}};}else{
# 2102
if(_tmp442 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp443 != 0;_tmp443=_tmp443->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp443->hd);
array_args=({struct Cyc_List_List*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332->hd=(struct Cyc_Absyn_Exp*)_tmp443->hd,_tmp332->tl=array_args;_tmp332;});}{
# 2108
struct Cyc_Absyn_Exp*_tmp333=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
s=Cyc_Absyn_declare_stmt(_tmp335,arr_type,_tmp333,s,0U);};}else{
# 2111
s=({struct _tuple1*_tmp8C3=_tmp335;void*_tmp8C2=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8C3,_tmp8C2,_tmp8C1,s,0U);});}}
# 2114
({void*_tmp8C4=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8C4;});};};};}
# 2116
_npop_handler(0U);goto _LL7;
# 2022
;_pop_region(r);}}case 11U: _LL24: _tmp446=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp445=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL25:
# 2119
 Cyc_Toc_exp_to_c(nv,_tmp446);{
struct Cyc_Absyn_Exp*fn_e=_tmp445?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp8C7=({void*_tmp8C6=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp8C6,({struct Cyc_Absyn_Exp*_tmp336[1U];_tmp336[0]=_tmp446;({struct Cyc_Absyn_Exp*_tmp8C5=fn_e;Cyc_Toc_fncall_exp_dl(_tmp8C5,_tag_fat(_tmp336,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp8C7;});
goto _LL7;};case 12U: _LL26: _tmp447=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL27:
 Cyc_Toc_exp_to_c(nv,_tmp447);goto _LL7;case 13U: _LL28: _tmp449=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp448=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL29:
# 2125
 Cyc_Toc_exp_to_c(nv,_tmp449);
# 2134 "toc.cyc"
for(0;_tmp448 != 0;_tmp448=_tmp448->tl){
enum Cyc_Absyn_KindQual _tmp337=(Cyc_Tcutil_type_kind((void*)_tmp448->hd))->kind;
if((int)_tmp337 != (int)4U &&(int)_tmp337 != (int)3U){
{void*_tmp338=Cyc_Tcutil_compress((void*)_tmp448->hd);void*_tmp339=_tmp338;switch(*((int*)_tmp339)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)->tag == 18U){_LLA0: _LLA1:
 continue;}else{goto _LLA2;}default: _LLA2: _LLA3:
# 2141
({void*_tmp8C9=({void*_tmp8C8=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8C8,_tmp449,0U);})->r;e->r=_tmp8C9;});
goto _LL9D;}_LL9D:;}
# 2144
break;}}
# 2147
goto _LL7;case 14U: _LL2A: _tmp44D=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp44C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp44B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp44A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL2B: {
# 2149
void*old_t2=(void*)_check_null(_tmp44C->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp44D;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp44D=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp44C);
# 2156
{struct _tuple33 _tmp33A=({struct _tuple33 _tmp762;({void*_tmp8CB=Cyc_Tcutil_compress(old_t2);_tmp762.f1=_tmp8CB;}),({void*_tmp8CA=Cyc_Tcutil_compress(new_typ);_tmp762.f2=_tmp8CA;});_tmp762;});struct _tuple33 _tmp33B=_tmp33A;struct Cyc_Absyn_PtrInfo _tmp361;struct Cyc_Absyn_PtrInfo _tmp360;struct Cyc_Absyn_PtrInfo _tmp35F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33B.f1)->tag == 3U)switch(*((int*)_tmp33B.f2)){case 3U: _LLA5: _tmp360=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33B.f1)->f1;_tmp35F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33B.f2)->f1;_LLA6: {
# 2158
struct Cyc_Absyn_Exp*_tmp33C=({void*_tmp8CC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8CC,(_tmp360.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp33D=({void*_tmp8CD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8CD,(_tmp35F.ptr_atts).bounds);});
int _tmp33E=Cyc_Tcutil_force_type2bool(0,(_tmp360.ptr_atts).zero_term);
int _tmp33F=Cyc_Tcutil_force_type2bool(0,(_tmp35F.ptr_atts).zero_term);
{struct _tuple34 _tmp340=({struct _tuple34 _tmp761;_tmp761.f1=_tmp33C,_tmp761.f2=_tmp33D;_tmp761;});struct _tuple34 _tmp341=_tmp340;if(_tmp341.f1 != 0){if(_tmp341.f2 != 0){_LLAC: _LLAD:
# 2165
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8D0=({void*_tmp8CF=*_tmp44D;Cyc_Toc_cast_it_r(_tmp8CF,({struct Cyc_Absyn_Exp*_tmp342[1U];_tmp342[0]=_tmp44C;({struct Cyc_Absyn_Exp*_tmp8CE=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp8CE,_tag_fat(_tmp342,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp8D0;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=_tmp44C->r;}
# 2174
goto _LLAB;}else{_LLAE: _LLAF: {
# 2176
struct Cyc_Absyn_Exp*_tmp343=(struct Cyc_Absyn_Exp*)_check_null(_tmp33C);
struct _tuple14 _tmp344=Cyc_Evexp_eval_const_uint_exp(_tmp343);struct _tuple14 _tmp345=_tmp344;unsigned _tmp355;int _tmp354;_LLB5: _tmp355=_tmp345.f1;_tmp354=_tmp345.f2;_LLB6:;
if(Cyc_Toc_is_toplevel(nv)){
# 2182
if((_tmp33E && !(_tmp35F.elt_tq).real_const)&& !_tmp33F)
_tmp343=({struct Cyc_Absyn_Exp*_tmp8D1=_tmp343;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8D1,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp8D2=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp343,_tmp44C))->r;e->r=_tmp8D2;});}else{
# 2187
if(_tmp33E){
# 2192
struct _tuple30 _tmp346=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp347=_tmp346;struct _tuple1*_tmp352;struct Cyc_Absyn_Exp*_tmp351;_LLB8: _tmp352=_tmp347.f1;_tmp351=_tmp347.f2;_LLB9:;{
struct Cyc_Absyn_Exp*arg3;
# 2196
{void*_tmp348=_tmp44C->r;void*_tmp349=_tmp348;struct Cyc_Absyn_Vardecl*_tmp34C;struct Cyc_Absyn_Vardecl*_tmp34B;switch(*((int*)_tmp349)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp349)->f1).Wstring_c).tag){case 8U: _LLBB: _LLBC:
 arg3=_tmp343;goto _LLBA;case 9U: _LLBD: _LLBE:
 arg3=_tmp343;goto _LLBA;default: goto _LLC3;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp349)->f1)){case 1U: _LLBF: _tmp34B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp349)->f1)->f1;_LLC0:
 _tmp34C=_tmp34B;goto _LLC2;case 4U: _LLC1: _tmp34C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp349)->f1)->f1;_LLC2:
# 2201
 if(!Cyc_Tcutil_is_array_type(_tmp34C->type))
goto _LLC4;
arg3=_tmp343;
goto _LLBA;default: goto _LLC3;}default: _LLC3: _LLC4:
# 2208
 if(_tmp354 && _tmp355 != (unsigned)1)
arg3=_tmp343;else{
# 2211
arg3=({struct Cyc_Absyn_Exp*_tmp34A[2U];({
struct Cyc_Absyn_Exp*_tmp8D4=({void*_tmp8D3=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp8D3,_tmp351);});_tmp34A[0]=_tmp8D4;}),_tmp34A[1]=_tmp343;({struct Cyc_Absyn_Exp*_tmp8D5=
# 2211
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp44C);Cyc_Toc_fncall_exp_dl(_tmp8D5,_tag_fat(_tmp34A,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2213
goto _LLBA;}_LLBA:;}
# 2215
if(!_tmp33F && !(_tmp35F.elt_tq).real_const)
# 2218
arg3=({struct Cyc_Absyn_Exp*_tmp8D6=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8D6,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2220
struct Cyc_Absyn_Exp*_tmp34D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp35F.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp34E=({struct Cyc_Absyn_Exp*_tmp350[3U];_tmp350[0]=_tmp351,_tmp350[1]=_tmp34D,_tmp350[2]=arg3;({struct Cyc_Absyn_Exp*_tmp8D7=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp8D7,_tag_fat(_tmp350,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*_tmp34F=Cyc_Absyn_exp_stmt(_tmp34E,0U);
_tmp34F=({struct _tuple1*_tmp8DA=_tmp352;void*_tmp8D9=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp8D8=_tmp44C;Cyc_Absyn_declare_stmt(_tmp8DA,_tmp8D9,_tmp8D8,_tmp34F,0U);});
({void*_tmp8DB=Cyc_Toc_stmt_exp_r(_tmp34F);e->r=_tmp8DB;});};};}else{
# 2227
({void*_tmp8DE=({struct Cyc_Absyn_Exp*_tmp353[3U];_tmp353[0]=_tmp44C,({
# 2229
struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp35F.elt_type),0U);_tmp353[1]=_tmp8DC;}),_tmp353[2]=_tmp343;({struct Cyc_Absyn_Exp*_tmp8DD=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8DD,_tag_fat(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2227
e->r=_tmp8DE;});}}
# 2232
goto _LLAB;}}}else{if(_tmp341.f2 != 0){_LLB0: _LLB1: {
# 2242 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp356=(struct Cyc_Absyn_Exp*)_check_null(_tmp33D);
if(_tmp33E && !_tmp33F)
_tmp356=({struct Cyc_Absyn_Exp*_tmp8DF=_tmp33D;Cyc_Absyn_add_exp(_tmp8DF,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2249
struct Cyc_Absyn_Exp*_tmp357=({struct Cyc_Absyn_Exp*_tmp359[3U];_tmp359[0]=_tmp44C,({
# 2251
struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp360.elt_type),0U);_tmp359[1]=_tmp8E0;}),_tmp359[2]=_tmp356;({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp8E1,_tag_fat(_tmp359,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2253
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp8E4=({struct Cyc_Absyn_Exp*_tmp358[1U];({struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_copy_exp(_tmp357);_tmp358[0]=_tmp8E2;});({struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8E3,_tag_fat(_tmp358,sizeof(struct Cyc_Absyn_Exp*),1U));});});_tmp357->r=_tmp8E4;});
({void*_tmp8E5=Cyc_Toc_cast_it_r(*_tmp44D,_tmp357);e->r=_tmp8E5;});
goto _LLAB;};}}else{_LLB2: _LLB3:
# 2261
 if((_tmp33E && !_tmp33F)&& !(_tmp35F.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp35A=0U;({struct _fat_ptr _tmp8E6=({const char*_tmp35B="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp35B,sizeof(char),70U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_unimp)(_tmp8E6,_tag_fat(_tmp35A,sizeof(void*),0U));});});
({void*_tmp8EA=({struct Cyc_Absyn_Exp*_tmp35C[3U];_tmp35C[0]=_tmp44C,({
struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp360.elt_type),0U);_tmp35C[1]=_tmp8E8;}),({
struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_uint_exp(1U,0U);_tmp35C[2]=_tmp8E7;});({struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8E9,_tag_fat(_tmp35C,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2264
e->r=_tmp8EA;});}
# 2268
goto _LLAB;}}_LLAB:;}
# 2270
goto _LLA4;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33B.f2)->f1)->tag == 1U){_LLA7: _tmp361=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33B.f1)->f1;_LLA8:
# 2272
{struct Cyc_Absyn_Exp*_tmp35D=({void*_tmp8EB=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8EB,(_tmp361.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp35E=_tmp35D;if(_tmp35E == 0){_LLC6: _LLC7:
# 2274
({void*_tmp8ED=({struct Cyc_Absyn_Exp*_tmp8EC=Cyc_Absyn_new_exp(_tmp44C->r,_tmp44C->loc);Cyc_Toc_aggrmember_exp_r(_tmp8EC,Cyc_Toc_curr_sp);});_tmp44C->r=_tmp8ED;});
_tmp44C->topt=new_typ_c;
goto _LLC5;}else{_LLC8: _LLC9:
 goto _LLC5;}_LLC5:;}
# 2279
goto _LLA4;}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9: _LLAA:
# 2281
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=_tmp44C->r;
goto _LLA4;}_LLA4:;}
# 2285
goto _LL7;}case 15U: _LL2C: _tmp44E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL2D:
# 2288
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp44E);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp44E)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp44E,0,Cyc_Toc_address_lvalue,1);
# 2294
({void*_tmp8EF=({void*_tmp8EE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8EE,_tmp44E);});e->r=_tmp8EF;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp44E->topt))))
# 2298
({void*_tmp8F1=({void*_tmp8F0=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp8F0,_tmp44E);});e->r=_tmp8F1;});}
# 2300
goto _LL7;case 16U: _LL2E: _tmp450=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp44F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL2F:
# 2307
 Cyc_Toc_exp_to_c(nv,_tmp44F);{
# 2309
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp44F->topt));
struct _tuple30 _tmp362=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp363=_tmp362;struct _tuple1*_tmp36F;struct Cyc_Absyn_Exp*_tmp36E;_LLCB: _tmp36F=_tmp363.f1;_tmp36E=_tmp363.f2;_LLCC:;{
struct Cyc_Absyn_Exp*lhs;
{void*_tmp364=Cyc_Tcutil_compress(elt_typ);void*_tmp365=_tmp364;void*_tmp366;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->tag == 4U){_LLCE: _tmp366=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp365)->f1).elt_type;_LLCF:
# 2314
 elt_typ=Cyc_Toc_typ_to_c(_tmp366);
lhs=Cyc_Absyn_copy_exp(_tmp36E);
goto _LLCD;}else{_LLD0: _LLD1:
# 2318
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp36E),0U);
goto _LLCD;}_LLCD:;}{
# 2321
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp44F);
struct _tuple1*_tmp367=(unsigned)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp368=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp367),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)_tmp368)
mexp=({struct Cyc_Absyn_Exp*_tmp369[2U];_tmp369[0]=_tmp368,({struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp369[1]=_tmp8F2;});({struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8F3,_tag_fat(_tmp369,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2328
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp36E,0U),0U);}{
# 2331
struct Cyc_Absyn_Exp*_tmp36A=Cyc_Toc_get_varsizeexp(_tmp44F);
if(_tmp36A != 0)
mexp=Cyc_Absyn_add_exp(mexp,_tmp36A,0U);
# 2335
if(_tmp450 == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2338
Cyc_Toc_exp_to_c(nv,_tmp450);
mexp=Cyc_Toc_rmalloc_exp(_tmp450,mexp);}{
# 2341
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp36B[3U];({
struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_copy_exp(_tmp36E);_tmp36B[0]=_tmp8F6;}),({
struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp36B[1]=_tmp8F5;}),
(unsigned)_tmp368?_tmp36B[2]=_tmp368:({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Absyn_uint_exp(1U,0U);_tmp36B[2]=_tmp8F4;});({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp8F7,_tag_fat(_tmp36B,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2348
answer=Cyc_Absyn_copy_exp(_tmp36E);}
({void*_tmp8F8=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->tag=Cyc_Toc_Dest,_tmp36C->f1=_tmp36E;_tmp36C;});e->annot=_tmp8F8;});{
struct Cyc_Absyn_Stmt*_tmp36D=({struct _tuple1*_tmp8FC=_tmp36F;void*_tmp8FB=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8FA=mexp;Cyc_Absyn_declare_stmt(_tmp8FC,_tmp8FB,_tmp8FA,({
struct Cyc_Absyn_Stmt*_tmp8F9=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp8F9,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2354
if((unsigned)array_len)
_tmp36D=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp367),Cyc_Absyn_uint_type,array_len,_tmp36D,0U);
({void*_tmp8FD=Cyc_Toc_stmt_exp_r(_tmp36D);e->r=_tmp8FD;});
if(_tmp36A != 0)
({void*_tmp8FF=({void*_tmp8FE=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp8FE,Cyc_Absyn_copy_exp(e));});e->r=_tmp8FF;});
goto _LL7;};};};};};};case 18U: _LL30: _tmp451=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL31:
# 2362
 Cyc_Toc_exp_to_c(nv,_tmp451);goto _LL7;case 17U: _LL32: _tmp452=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL33:
({void*_tmp901=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->tag=17U,({void*_tmp900=Cyc_Toc_typ_to_c(_tmp452);_tmp370->f1=_tmp900;});_tmp370;});e->r=_tmp901;});goto _LL7;case 19U: _LL34: _tmp454=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp453=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL35: {
# 2365
void*_tmp371=_tmp454;
struct Cyc_List_List*_tmp372=_tmp453;
for(0;_tmp372 != 0;_tmp372=_tmp372->tl){
void*_tmp373=(void*)_tmp372->hd;void*_tmp374=_tmp373;unsigned _tmp386;struct _fat_ptr*_tmp385;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp374)->tag == 0U){_LLD3: _tmp385=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp374)->f1;_LLD4:
 goto _LLD2;}else{_LLD5: _tmp386=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp374)->f1;_LLD6:
# 2371
{void*_tmp375=Cyc_Tcutil_compress(_tmp371);void*_tmp376=_tmp375;struct Cyc_List_List*_tmp384;struct Cyc_List_List*_tmp383;struct Cyc_Absyn_Datatypefield*_tmp382;union Cyc_Absyn_AggrInfo _tmp381;switch(*((int*)_tmp376)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f1)){case 20U: _LLD8: _tmp381=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f1)->f1;_LLD9: {
# 2373
struct Cyc_Absyn_Aggrdecl*_tmp377=Cyc_Absyn_get_known_aggrdecl(_tmp381);
if(_tmp377->impl == 0)
({void*_tmp378=0U;({struct _fat_ptr _tmp902=({const char*_tmp379="struct fields must be known";_tag_fat(_tmp379,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp902,_tag_fat(_tmp378,sizeof(void*),0U));});});
_tmp383=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp377->impl))->fields;goto _LLDB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f1)->f1).KnownDatatypefield).tag == 2){_LLDE: _tmp382=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376)->f1)->f1).KnownDatatypefield).val).f2;_LLDF:
# 2387
 if(_tmp386 == (unsigned)0)
({void*_tmp903=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->tag=0U,_tmp37D->f1=Cyc_Toc_tag_sp;_tmp37D;}));_tmp372->hd=_tmp903;});else{
# 2390
_tmp371=(*((struct _tuple13*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp382->typs,(int)(_tmp386 - (unsigned)1))).f2;
({void*_tmp905=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E->tag=0U,({struct _fat_ptr*_tmp904=Cyc_Absyn_fieldname((int)_tmp386);_tmp37E->f1=_tmp904;});_tmp37E;}));_tmp372->hd=_tmp905;});}
# 2393
goto _LLD7;}else{goto _LLE0;}default: goto _LLE0;}case 7U: _LLDA: _tmp383=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp376)->f2;_LLDB: {
# 2378
struct Cyc_Absyn_Aggrfield*_tmp37A=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp383,(int)_tmp386);
({void*_tmp906=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->tag=0U,_tmp37B->f1=_tmp37A->name;_tmp37B;}));_tmp372->hd=_tmp906;});
_tmp371=_tmp37A->type;
goto _LLD7;}case 6U: _LLDC: _tmp384=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp376)->f1;_LLDD:
# 2383
({void*_tmp908=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->tag=0U,({struct _fat_ptr*_tmp907=Cyc_Absyn_fieldname((int)(_tmp386 + (unsigned)1));_tmp37C->f1=_tmp907;});_tmp37C;}));_tmp372->hd=_tmp908;});
_tmp371=(*((struct _tuple13*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp384,(int)_tmp386)).f2;
goto _LLD7;default: _LLE0: _LLE1:
# 2394
({void*_tmp37F=0U;({struct _fat_ptr _tmp909=({const char*_tmp380="impossible type for offsetof tuple index";_tag_fat(_tmp380,sizeof(char),41U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp909,_tag_fat(_tmp37F,sizeof(void*),0U));});});}_LLD7:;}
# 2396
goto _LLD2;}_LLD2:;}
# 2399
({void*_tmp90B=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->tag=19U,({void*_tmp90A=Cyc_Toc_typ_to_c(_tmp454);_tmp387->f1=_tmp90A;}),_tmp387->f2=_tmp453;_tmp387;});e->r=_tmp90B;});
goto _LL7;}case 21U: _LL36: _tmp458=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp457=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp456=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp455=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL37: {
# 2402
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp458->topt);
Cyc_Toc_exp_to_c(nv,_tmp458);
if(_tmp456 && _tmp455)
({void*_tmp910=({struct Cyc_Absyn_Exp*_tmp90F=_tmp458;void*_tmp90E=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp90D=e1_cyc_type;struct _fat_ptr*_tmp90C=_tmp457;Cyc_Toc_check_tagged_union(_tmp90F,_tmp90E,_tmp90D,_tmp90C,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2406
e->r=_tmp910;});
# 2408
if(is_poly)
({void*_tmp912=({void*_tmp911=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp911,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp912;});
goto _LL7;}case 22U: _LL38: _tmp45C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp45B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp45A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp459=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL39: {
# 2412
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp45C->topt);
void*ta;
{void*_tmp388=Cyc_Tcutil_compress(e1typ);void*_tmp389=_tmp388;struct Cyc_Absyn_PtrInfo _tmp38C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp389)->tag == 3U){_LLE3: _tmp38C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp389)->f1;_LLE4:
 ta=_tmp38C.elt_type;goto _LLE2;}else{_LLE5: _LLE6:
({void*_tmp38A=0U;({struct _fat_ptr _tmp913=({const char*_tmp38B="get_ptr_typ: not a pointer!";_tag_fat(_tmp38B,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp913,_tag_fat(_tmp38A,sizeof(void*),0U));});});}_LLE2:;}
# 2419
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp45C,e->annot,0);
if(_tmp45A && _tmp459)
({void*_tmp918=({struct Cyc_Absyn_Exp*_tmp917=_tmp45C;void*_tmp916=Cyc_Toc_typ_to_c(e1typ);void*_tmp915=ta;struct _fat_ptr*_tmp914=_tmp45B;Cyc_Toc_check_tagged_union(_tmp917,_tmp916,_tmp915,_tmp914,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp918;});
if(is_poly && _tmp459)
({void*_tmp91A=({void*_tmp919=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp919,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp91A;});
goto _LL7;}case 20U: _LL3A: _tmp45D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL3B:
# 2427
 did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,_tmp45D,e->annot,0);
goto _LL7;case 23U: _LL3C: _tmp45F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp45E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL3D:
# 2431
{void*_tmp38D=Cyc_Tcutil_compress((void*)_check_null(_tmp45F->topt));void*_tmp38E=_tmp38D;struct Cyc_List_List*_tmp396;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38E)->tag == 6U){_LLE8: _tmp396=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38E)->f1;_LLE9:
# 2433
 Cyc_Toc_exp_to_c(nv,_tmp45F);{
int _tmp38F=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp45E);{
struct _tuple14 _tmp390=Cyc_Evexp_eval_const_uint_exp(_tmp45E);struct _tuple14 _tmp391=_tmp390;unsigned _tmp395;int _tmp394;_LLED: _tmp395=_tmp391.f1;_tmp394=_tmp391.f2;_LLEE:;
if(!_tmp394)
({void*_tmp392=0U;({struct _fat_ptr _tmp91B=({const char*_tmp393="unknown tuple subscript in translation to C";_tag_fat(_tmp393,sizeof(char),44U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp91B,_tag_fat(_tmp392,sizeof(void*),0U));});});
({void*_tmp91D=({struct Cyc_Absyn_Exp*_tmp91C=_tmp45F;Cyc_Toc_aggrmember_exp_r(_tmp91C,Cyc_Absyn_fieldname((int)(_tmp395 + (unsigned)1)));});e->r=_tmp91D;});
goto _LLE7;};};}else{_LLEA: _LLEB:
# 2443
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp45F,e->annot,_tmp45E);
if(index_used)
({void*_tmp91E=Cyc_Toc_deref_exp_r(_tmp45F);e->r=_tmp91E;});
goto _LLE7;};}_LLE7:;}
# 2449
goto _LL7;case 24U: _LL3E: _tmp460=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL3F:
# 2451
 if(!Cyc_Toc_is_toplevel(nv)){
# 2453
void*_tmp397=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp460));
{void*_tmp398=_tmp397;union Cyc_Absyn_AggrInfo _tmp39D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp398)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp398)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp398)->f2 == 0){_LLF0: _tmp39D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp398)->f1)->f1;_LLF1: {
# 2456
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp39D);
{int i=1;for(0;_tmp460 != 0;(_tmp460=_tmp460->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp460->hd);
dles=({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));({struct _tuple20*_tmp920=({struct _fat_ptr*_tmp91F=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp91F,(struct Cyc_Absyn_Exp*)_tmp460->hd);});_tmp399->hd=_tmp920;}),_tmp399->tl=dles;_tmp399;});}}
# 2462
({void*_tmp922=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->tag=29U,_tmp39A->f1=sd->name,_tmp39A->f2=0,({struct Cyc_List_List*_tmp921=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp39A->f3=_tmp921;}),_tmp39A->f4=sd;_tmp39A;});e->r=_tmp922;});
e->topt=_tmp397;
goto _LLEF;}}else{goto _LLF2;}}else{goto _LLF2;}}else{_LLF2: _LLF3:
({void*_tmp39B=0U;({struct _fat_ptr _tmp923=({const char*_tmp39C="tuple type not an aggregate";_tag_fat(_tmp39C,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp923,_tag_fat(_tmp39B,sizeof(void*),0U));});});}_LLEF:;}
# 2467
goto _LL7;}else{
# 2471
struct Cyc_List_List*dles=0;
for(0;_tmp460 != 0;_tmp460=_tmp460->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp460->hd);
dles=({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct _tuple20*_tmp924=({struct _tuple20*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->f1=0,_tmp39E->f2=(struct Cyc_Absyn_Exp*)_tmp460->hd;_tmp39E;});_tmp39F->hd=_tmp924;}),_tmp39F->tl=dles;_tmp39F;});}
# 2476
({void*_tmp925=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp925;});}
# 2478
goto _LL7;case 26U: _LL40: _tmp461=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL41:
# 2484
{struct Cyc_List_List*_tmp3A0=_tmp461;for(0;_tmp3A0 != 0;_tmp3A0=_tmp3A0->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp3A0->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp926=Cyc_Toc_unresolvedmem_exp_r(0,_tmp461);e->r=_tmp926;});
goto _LL7;case 27U: _LL42: _tmp465=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp464=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp463=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp462=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL43:
# 2492
 Cyc_Toc_exp_to_c(nv,_tmp464);
Cyc_Toc_exp_to_c(nv,_tmp463);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple14 _tmp3A1=Cyc_Evexp_eval_const_uint_exp(_tmp464);struct _tuple14 _tmp3A2=_tmp3A1;unsigned _tmp3AC;int _tmp3AB;_LLF5: _tmp3AC=_tmp3A2.f1;_tmp3AB=_tmp3A2.f2;_LLF6:;{
void*_tmp3A3=Cyc_Toc_typ_to_c((void*)_check_null(_tmp463->topt));
struct Cyc_List_List*es=0;
# 2499
if(!Cyc_Toc_is_zero(_tmp463)){
if(!_tmp3AB)
({void*_tmp3A4=0U;({unsigned _tmp928=_tmp464->loc;struct _fat_ptr _tmp927=({const char*_tmp3A5="cannot determine value of constant";_tag_fat(_tmp3A5,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp928,_tmp927,_tag_fat(_tmp3A4,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < _tmp3AC;++ i){
es=({struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct _tuple20*_tmp929=({struct _tuple20*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->f1=0,_tmp3A6->f2=_tmp463;_tmp3A6;});_tmp3A7->hd=_tmp929;}),_tmp3A7->tl=es;_tmp3A7;});}}
# 2505
if(_tmp462){
struct Cyc_Absyn_Exp*_tmp3A8=({void*_tmp92A=_tmp3A3;Cyc_Toc_cast_it(_tmp92A,Cyc_Absyn_uint_exp(0U,0U));});
es=({struct Cyc_List_List*_tmp92C=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp92C,({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct _tuple20*_tmp92B=({struct _tuple20*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->f1=0,_tmp3A9->f2=_tmp3A8;_tmp3A9;});_tmp3AA->hd=_tmp92B;}),_tmp3AA->tl=0;_tmp3AA;}));});}}
# 2510
({void*_tmp92D=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp92D;});};}
# 2512
goto _LL7;case 28U: _LL44: _tmp468=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp467=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp466=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL45:
# 2519
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp92E=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp92E;});else{
# 2522
Cyc_Toc_exp_to_c(nv,_tmp468);}
goto _LL7;case 30U: _LL46: _tmp46A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp469=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL47:
# 2526
{struct Cyc_List_List*_tmp3AD=_tmp469;for(0;_tmp3AD != 0;_tmp3AD=_tmp3AD->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp3AD->hd)).f2);}}{
void*_tmp3AE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3AE;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3AF=Cyc_Tcutil_compress(_tmp3AE);void*_tmp3B0=_tmp3AF;union Cyc_Absyn_AggrInfo _tmp3B4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f2 == 0){_LLF8: _tmp3B4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B0)->f1)->f1;_LLF9: {
# 2533
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3B4);
({void*_tmp92F=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->tag=29U,_tmp3B1->f1=sd->name,_tmp3B1->f2=0,_tmp3B1->f3=_tmp469,_tmp3B1->f4=sd;_tmp3B1;});e->r=_tmp92F;});
e->topt=_tmp3AE;
goto _LLF7;}}else{goto _LLFA;}}else{goto _LLFA;}}else{_LLFA: _LLFB:
({void*_tmp3B2=0U;({struct _fat_ptr _tmp930=({const char*_tmp3B3="anonStruct type not an aggregate";_tag_fat(_tmp3B3,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp930,_tag_fat(_tmp3B2,sizeof(void*),0U));});});}_LLF7:;}else{
# 2540
({void*_tmp931=Cyc_Toc_unresolvedmem_exp_r(0,_tmp469);e->r=_tmp931;});}
goto _LL7;};case 29U: _LL48: _tmp46E=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp46D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp46C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_tmp46B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2D8)->f4;_LL49:
# 2546
 if(_tmp46B == 0 || _tmp46B->impl == 0)
({void*_tmp3B5=0U;({struct _fat_ptr _tmp932=({const char*_tmp3B6="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp3B6,sizeof(char),58U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp932,_tag_fat(_tmp3B5,sizeof(void*),0U));});});{
void*_tmp3B7=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3B8=Cyc_Tcutil_compress(_tmp3B7);void*_tmp3B9=_tmp3B8;union Cyc_Absyn_AggrInfo _tmp3BC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->f1)->tag == 20U){_LLFD: _tmp3BC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->f1)->f1;_LLFE:
({struct _tuple1*_tmp933=(Cyc_Absyn_aggr_kinded_name(_tmp3BC)).f2;*_tmp46E=_tmp933;});goto _LLFC;}else{goto _LLFF;}}else{_LLFF: _LL100:
({void*_tmp3BA=0U;({struct _fat_ptr _tmp934=({const char*_tmp3BB="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp3BB,sizeof(char),44U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp934,_tag_fat(_tmp3BA,sizeof(void*),0U));});});}_LLFC:;}{
# 2557
struct Cyc_List_List*_tmp3BD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->fields;
if(_tmp3BD == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(_tmp3BD))->tl != 0;_tmp3BD=_tmp3BD->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3BE=(struct Cyc_Absyn_Aggrfield*)_tmp3BD->hd;
struct Cyc_List_List*_tmp3BF=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp46C,_tmp46B->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->fields);
# 2564
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3C0=Cyc_Tcutil_compress(old_typ);void*_tmp3C1=_tmp3C0;struct Cyc_List_List*_tmp3ED;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1)->f1)->tag == 20U){_LL102: _tmp3ED=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1)->f2;_LL103:
# 2568
{struct Cyc_List_List*_tmp3C2=_tmp3BF;for(0;_tmp3C2 != 0;_tmp3C2=_tmp3C2->tl){
struct _tuple35*_tmp3C3=(struct _tuple35*)_tmp3C2->hd;struct _tuple35*_tmp3C4=_tmp3C3;struct Cyc_Absyn_Aggrfield*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3E9;_LL107: _tmp3EA=_tmp3C4->f1;_tmp3E9=_tmp3C4->f2;_LL108:;{
void*_tmp3C5=_tmp3E9->topt;
Cyc_Toc_exp_to_c(nv,_tmp3E9);
# 2573
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3EA->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3E9->topt)))
({void*_tmp936=({void*_tmp935=Cyc_Toc_typ_to_c(_tmp3EA->type);Cyc_Toc_cast_it(_tmp935,
Cyc_Absyn_copy_exp(_tmp3E9));})->r;
# 2575
_tmp3E9->r=_tmp936;});
# 2578
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->tagged){
struct _fat_ptr*_tmp3C6=_tmp3EA->name;
struct Cyc_Absyn_Exp*_tmp3C7=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(_tmp46B,_tmp3C6),0U);
struct _tuple20*_tmp3C8=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3C7);
struct _tuple20*_tmp3C9=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp3E9);
struct _tuple1*s=({struct _tuple1*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({union Cyc_Absyn_Nmspace _tmp93A=Cyc_Absyn_Abs_n(0,1);_tmp3D4->f1=_tmp93A;}),({
struct _fat_ptr*_tmp939=({struct _fat_ptr*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct _fat_ptr _tmp938=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D1=({struct Cyc_String_pa_PrintArg_struct _tmp764;_tmp764.tag=0U,_tmp764.f1=(struct _fat_ptr)((struct _fat_ptr)*(*_tmp46B->name).f2);_tmp764;});struct Cyc_String_pa_PrintArg_struct _tmp3D2=({struct Cyc_String_pa_PrintArg_struct _tmp763;_tmp763.tag=0U,_tmp763.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp3C6);_tmp763;});void*_tmp3CF[2U];_tmp3CF[0]=& _tmp3D1,_tmp3CF[1]=& _tmp3D2;({struct _fat_ptr _tmp937=({const char*_tmp3D0="_union_%s_%s";_tag_fat(_tmp3D0,sizeof(char),13U);});Cyc_aprintf(_tmp937,_tag_fat(_tmp3CF,sizeof(void*),2U));});});*_tmp3D3=_tmp938;});_tmp3D3;});_tmp3D4->f2=_tmp939;});_tmp3D4;});
# 2586
struct _tuple20*_tmp3CA=({
struct _fat_ptr*_tmp93C=_tmp3C6;Cyc_Toc_make_field(_tmp93C,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->tag=29U,_tmp3CE->f1=s,_tmp3CE->f2=0,({struct Cyc_List_List*_tmp93B=({struct _tuple20*_tmp3CD[2U];_tmp3CD[0]=_tmp3C8,_tmp3CD[1]=_tmp3C9;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3CD,sizeof(struct _tuple20*),2U));});_tmp3CE->f3=_tmp93B;}),_tmp3CE->f4=0;_tmp3CE;}),0U));});
# 2589
({void*_tmp93E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->tag=29U,_tmp3CC->f1=*_tmp46E,_tmp3CC->f2=0,({struct Cyc_List_List*_tmp93D=({struct _tuple20*_tmp3CB[1U];_tmp3CB[0]=_tmp3CA;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3CB,sizeof(struct _tuple20*),1U));});_tmp3CC->f3=_tmp93D;}),_tmp3CC->f4=_tmp46B;_tmp3CC;});e->r=_tmp93E;});}
# 2595
if(Cyc_Toc_is_abstract_type(old_typ)&& _tmp3BE == _tmp3EA){
struct Cyc_List_List*_tmp3D5=({struct Cyc_List_List*_tmp93F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->exist_vars,_tmp46D);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp93F,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp46B->tvs,_tmp3ED));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp3D6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->fields;for(0;_tmp3D6 != 0;_tmp3D6=_tmp3D6->tl){
struct Cyc_Absyn_Aggrfield*_tmp3D7=(struct Cyc_Absyn_Aggrfield*)_tmp3D6->hd;
void*_tmp3D8=Cyc_Tcutil_substitute(_tmp3D5,_tmp3D7->type);
struct Cyc_Absyn_Aggrfield*_tmp3D9=({struct Cyc_Absyn_Aggrfield*_tmp940=_tmp3D7;Cyc_Toc_aggrfield_to_c(_tmp940,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp3D5,_tmp3D8)));});
new_fields=({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->hd=_tmp3D9,_tmp3DA->tl=new_fields;_tmp3DA;});
# 2610
if(_tmp3D6->tl == 0){
{void*_tmp3DB=Cyc_Tcutil_compress(_tmp3D9->type);void*_tmp3DC=_tmp3DB;struct Cyc_Absyn_ArrayInfo _tmp3E1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DC)->tag == 4U){_LL10A: _tmp3E1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DC)->f1;_LL10B:
# 2613
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3E1.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp3DD=_tmp3E1;
({struct Cyc_Absyn_Exp*_tmp941=Cyc_Absyn_uint_exp(0U,0U);_tmp3DD.num_elts=_tmp941;});
({void*_tmp942=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->tag=4U,_tmp3DE->f1=_tmp3DD;_tmp3DE;});_tmp3D9->type=_tmp942;});}
# 2618
goto _LL109;}else{_LL10C: _LL10D:
# 2622
 if(_tmp3E9->topt == 0)
goto _LL109;
{void*_tmp3DF=Cyc_Tcutil_compress((void*)_check_null(_tmp3E9->topt));void*_tmp3E0=_tmp3DF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->f1)->tag == 20U){_LL10F: _LL110:
# 2626
 _tmp3D9->type=(void*)_check_null(_tmp3E9->topt);goto _LL10E;}else{goto _LL111;}}else{_LL111: _LL112:
 goto _LL10E;}_LL10E:;}
# 2629
goto _LL109;}_LL109:;}
# 2633
if(!Cyc_Tcutil_is_array_type(_tmp3D7->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3D7->type)))
({struct Cyc_List_List*_tmp944=({struct Cyc_List_List*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));({void*_tmp943=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->tag=6U,_tmp3E2->f1=0;_tmp3E2;});_tmp3E3->hd=_tmp943;}),_tmp3E3->tl=_tmp3D9->attributes;_tmp3E3;});_tmp3D9->attributes=_tmp944;});}}}
# 2639
_tmp46B=({struct _fat_ptr*_tmp947=({struct _fat_ptr*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({struct _fat_ptr _tmp946=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3E6=({struct Cyc_Int_pa_PrintArg_struct _tmp765;_tmp765.tag=1U,_tmp765.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp765;});void*_tmp3E4[1U];_tmp3E4[0]=& _tmp3E6;({struct _fat_ptr _tmp945=({const char*_tmp3E5="_genStruct%d";_tag_fat(_tmp3E5,sizeof(char),13U);});Cyc_aprintf(_tmp945,_tag_fat(_tmp3E4,sizeof(void*),1U));});});*_tmp3E7=_tmp946;});_tmp3E7;});Cyc_Toc_make_c_struct_defn(_tmp947,
# 2641
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*_tmp46E=_tmp46B->name;
Cyc_Toc_aggrdecl_to_c(_tmp46B);
# 2645
({void*_tmp948=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3E8=_cycalloc(sizeof(*_tmp3E8));*_tmp3E8=_tmp46B;_tmp3E8;})),0);e->topt=_tmp948;});}};}}
# 2648
goto _LL101;}else{goto _LL104;}}else{_LL104: _LL105:
({void*_tmp3EB=0U;({struct _fat_ptr _tmp949=({const char*_tmp3EC="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp3EC,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp949,_tag_fat(_tmp3EB,sizeof(void*),0U));});});}_LL101:;}else{
# 2657
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->tagged){
# 2659
struct _tuple35*_tmp3EE=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp3BF))->hd;struct _tuple35*_tmp3EF=_tmp3EE;struct Cyc_Absyn_Aggrfield*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3F7;_LL114: _tmp3F8=_tmp3EF->f1;_tmp3F7=_tmp3EF->f2;_LL115:;{
void*_tmp3F0=(void*)_check_null(_tmp3F7->topt);
void*_tmp3F1=_tmp3F8->type;
Cyc_Toc_exp_to_c(nv,_tmp3F7);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3F1)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp3F0))
({void*_tmp94B=({
void*_tmp94A=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp94A,Cyc_Absyn_new_exp(_tmp3F7->r,0U));});
# 2665
_tmp3F7->r=_tmp94B;});{
# 2668
int i=Cyc_Toc_get_member_offset(_tmp46B,_tmp3F8->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp3F2=({struct _tuple20*_tmp3F4[2U];({struct _tuple20*_tmp94D=({struct _tuple20*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->f1=0,_tmp3F5->f2=field_tag_exp;_tmp3F5;});_tmp3F4[0]=_tmp94D;}),({struct _tuple20*_tmp94C=({struct _tuple20*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6->f1=0,_tmp3F6->f2=_tmp3F7;_tmp3F6;});_tmp3F4[1]=_tmp94C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F4,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp3F2,0U);
({void*_tmp94F=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp3F3[1U];({struct _tuple20*_tmp94E=Cyc_Toc_make_field(_tmp3F8->name,umem);_tmp3F3[0]=_tmp94E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F3,sizeof(struct _tuple20*),1U));}));e->r=_tmp94F;});};};}else{
# 2675
struct Cyc_List_List*_tmp3F9=0;
struct Cyc_List_List*_tmp3FA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46B->impl))->fields;
for(0;_tmp3FA != 0;_tmp3FA=_tmp3FA->tl){
struct Cyc_List_List*_tmp3FB=_tmp3BF;for(0;_tmp3FB != 0;_tmp3FB=_tmp3FB->tl){
if((*((struct _tuple35*)_tmp3FB->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp3FA->hd){
struct _tuple35*_tmp3FC=(struct _tuple35*)_tmp3FB->hd;struct _tuple35*_tmp3FD=_tmp3FC;struct Cyc_Absyn_Aggrfield*_tmp403;struct Cyc_Absyn_Exp*_tmp402;_LL117: _tmp403=_tmp3FD->f1;_tmp402=_tmp3FD->f2;_LL118:;{
void*_tmp3FE=Cyc_Toc_typ_to_c(_tmp403->type);
void*_tmp3FF=Cyc_Toc_typ_to_c((void*)_check_null(_tmp402->topt));
Cyc_Toc_exp_to_c(nv,_tmp402);
# 2685
if(!Cyc_Unify_unify(_tmp3FE,_tmp3FF)){
# 2687
if(!Cyc_Tcutil_is_arithmetic_type(_tmp3FE)|| !
Cyc_Tcutil_is_arithmetic_type(_tmp3FF))
_tmp402=({void*_tmp950=_tmp3FE;Cyc_Toc_cast_it(_tmp950,Cyc_Absyn_copy_exp(_tmp402));});}
_tmp3F9=({struct Cyc_List_List*_tmp401=_cycalloc(sizeof(*_tmp401));({struct _tuple20*_tmp951=({struct _tuple20*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->f1=0,_tmp400->f2=_tmp402;_tmp400;});_tmp401->hd=_tmp951;}),_tmp401->tl=_tmp3F9;_tmp401;});
break;};}}}
# 2694
({void*_tmp952=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F9));e->r=_tmp952;});}}
# 2697
goto _LL7;};};};case 31U: _LL4A: _tmp471=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp470=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_tmp46F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2D8)->f3;_LL4B: {
# 2701
struct Cyc_List_List*_tmp404=_tmp46F->typs;
{struct Cyc_List_List*_tmp405=_tmp471;for(0;_tmp405 != 0;(_tmp405=_tmp405->tl,_tmp404=_tmp404->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp405->hd;
void*_tmp406=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple13*)((struct Cyc_List_List*)_check_null(_tmp404))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp406))
({void*_tmp953=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp953;});}}{
# 2712
struct Cyc_Absyn_Exp*_tmp407=
_tmp470->is_extensible?Cyc_Absyn_var_exp(_tmp46F->name,0U): Cyc_Toc_datatype_tag(_tmp470,_tmp46F->name);
# 2715
if(!Cyc_Toc_is_toplevel(nv)){
# 2717
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp471 != 0;(_tmp471=_tmp471->tl,++ i)){
dles=({struct Cyc_List_List*_tmp408=_cycalloc(sizeof(*_tmp408));({struct _tuple20*_tmp955=({struct _fat_ptr*_tmp954=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp954,(struct Cyc_Absyn_Exp*)_tmp471->hd);});_tmp408->hd=_tmp955;}),_tmp408->tl=dles;_tmp408;});}}{
# 2723
struct _tuple20*_tmp409=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp407);
({void*_tmp959=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->tag=29U,({struct _tuple1*_tmp958=Cyc_Toc_collapse_qvars(_tmp46F->name,_tmp470->name);_tmp40B->f1=_tmp958;}),_tmp40B->f2=0,({
struct Cyc_List_List*_tmp957=({struct Cyc_List_List*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->hd=_tmp409,({struct Cyc_List_List*_tmp956=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp40A->tl=_tmp956;});_tmp40A;});_tmp40B->f3=_tmp957;}),_tmp40B->f4=0;_tmp40B;});
# 2724
e->r=_tmp959;});};}else{
# 2729
struct Cyc_List_List*_tmp40C=0;
for(0;_tmp471 != 0;_tmp471=_tmp471->tl){
_tmp40C=({struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(*_tmp40E));({struct _tuple20*_tmp95A=({struct _tuple20*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->f1=0,_tmp40D->f2=(struct Cyc_Absyn_Exp*)_tmp471->hd;_tmp40D;});_tmp40E->hd=_tmp95A;}),_tmp40E->tl=_tmp40C;_tmp40E;});}
({void*_tmp95D=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp410=_cycalloc(sizeof(*_tmp410));({struct _tuple20*_tmp95C=({struct _tuple20*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->f1=0,_tmp40F->f2=_tmp407;_tmp40F;});_tmp410->hd=_tmp95C;}),({
struct Cyc_List_List*_tmp95B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp40C);_tmp410->tl=_tmp95B;});_tmp410;}));
# 2732
e->r=_tmp95D;});}
# 2735
goto _LL7;};}case 32U: _LL4C: _LL4D:
# 2737
 goto _LL4F;case 33U: _LL4E: _LL4F:
 goto _LL7;case 34U: _LL50: _tmp477=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).is_calloc;_tmp476=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).rgn;_tmp475=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).elt_type;_tmp474=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).num_elts;_tmp473=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).fat_result;_tmp472=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).inline_call;_LL51: {
# 2741
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp475)));
Cyc_Toc_exp_to_c(nv,_tmp474);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=_tmp474;
if(_tmp473){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2750
if(_tmp477){
if(_tmp476 != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,_tmp476);
pexp=({struct Cyc_Absyn_Exp*_tmp411[3U];_tmp411[0]=_tmp476,({
struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp411[1]=_tmp95E;}),_tmp411[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp95F=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp95F,_tag_fat(_tmp411,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2756
pexp=({void*_tmp961=*_tmp475;struct Cyc_Absyn_Exp*_tmp960=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp961,_tmp960,num_elts2);});}}else{
# 2758
if(_tmp476 != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,_tmp476);
if(_tmp472)
pexp=Cyc_Toc_rmalloc_inline_exp(_tmp476,num_elts2);else{
# 2763
pexp=Cyc_Toc_rmalloc_exp(_tmp476,num_elts2);}}else{
# 2765
pexp=Cyc_Toc_malloc_exp(*_tmp475,num_elts2);}}
# 2767
if(_tmp473){
struct Cyc_Absyn_Exp*elt_sz=_tmp477?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp412[3U];_tmp412[0]=pexp,_tmp412[1]=elt_sz,_tmp412[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp962=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp962,_tag_fat(_tmp412,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp965=(struct _tuple1*)_check_null(x);void*_tmp964=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp963=_tmp474;Cyc_Absyn_declare_stmt(_tmp965,_tmp964,_tmp963,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp966=Cyc_Toc_stmt_exp_r(s);e->r=_tmp966;});}else{
# 2774
e->r=pexp->r;}
goto _LL7;};}case 35U: _LL52: _tmp479=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp478=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL53: {
# 2784
void*e1_old_typ=(void*)_check_null(_tmp479->topt);
void*e2_old_typ=(void*)_check_null(_tmp478->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp413=0U;({struct _fat_ptr _tmp967=({const char*_tmp414="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp414,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp967,_tag_fat(_tmp413,sizeof(void*),0U));});});{
# 2792
unsigned _tmp415=e->loc;
struct _tuple1*_tmp416=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp417=Cyc_Absyn_var_exp(_tmp416,_tmp415);_tmp417->topt=e1_old_typ;{
struct _tuple1*_tmp418=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp419=Cyc_Absyn_var_exp(_tmp418,_tmp415);_tmp419->topt=e2_old_typ;{
# 2798
struct Cyc_Absyn_Exp*_tmp41A=({struct Cyc_Absyn_Exp*_tmp969=Cyc_Tcutil_deep_copy_exp(1,_tmp479);struct Cyc_Absyn_Exp*_tmp968=_tmp419;Cyc_Absyn_assign_exp(_tmp969,_tmp968,_tmp415);});_tmp41A->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp41B=Cyc_Absyn_exp_stmt(_tmp41A,_tmp415);
struct Cyc_Absyn_Exp*_tmp41C=({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Tcutil_deep_copy_exp(1,_tmp478);struct Cyc_Absyn_Exp*_tmp96A=_tmp417;Cyc_Absyn_assign_exp(_tmp96B,_tmp96A,_tmp415);});_tmp41C->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp41D=Cyc_Absyn_exp_stmt(_tmp41C,_tmp415);
# 2803
struct Cyc_Absyn_Stmt*_tmp41E=({struct _tuple1*_tmp973=_tmp416;void*_tmp972=e1_old_typ;struct Cyc_Absyn_Exp*_tmp971=_tmp479;struct Cyc_Absyn_Stmt*_tmp970=({
struct _tuple1*_tmp96F=_tmp418;void*_tmp96E=e2_old_typ;struct Cyc_Absyn_Exp*_tmp96D=_tmp478;struct Cyc_Absyn_Stmt*_tmp96C=
Cyc_Absyn_seq_stmt(_tmp41B,_tmp41D,_tmp415);
# 2804
Cyc_Absyn_declare_stmt(_tmp96F,_tmp96E,_tmp96D,_tmp96C,_tmp415);});
# 2803
Cyc_Absyn_declare_stmt(_tmp973,_tmp972,_tmp971,_tmp970,_tmp415);});
# 2806
Cyc_Toc_stmt_to_c(nv,_tmp41E);
({void*_tmp974=Cyc_Toc_stmt_exp_r(_tmp41E);e->r=_tmp974;});
goto _LL7;};};};};};}case 38U: _LL54: _tmp47B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp47A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LL55: {
# 2811
void*_tmp41F=Cyc_Tcutil_compress((void*)_check_null(_tmp47B->topt));
Cyc_Toc_exp_to_c(nv,_tmp47B);
{void*_tmp420=_tmp41F;struct Cyc_Absyn_Aggrdecl*_tmp426;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->f1)->f1).KnownAggr).tag == 2){_LL11A: _tmp426=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->f1)->f1).KnownAggr).val;_LL11B: {
# 2815
struct Cyc_Absyn_Exp*_tmp421=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp426,_tmp47A),0U);
struct Cyc_Absyn_Exp*_tmp422=Cyc_Toc_member_exp(_tmp47B,_tmp47A,0U);
struct Cyc_Absyn_Exp*_tmp423=Cyc_Toc_member_exp(_tmp422,Cyc_Toc_tag_sp,0U);
({void*_tmp975=(Cyc_Absyn_eq_exp(_tmp423,_tmp421,0U))->r;e->r=_tmp975;});
goto _LL119;}}else{goto _LL11C;}}else{goto _LL11C;}}else{_LL11C: _LL11D:
({void*_tmp424=0U;({struct _fat_ptr _tmp976=({const char*_tmp425="non-aggregate type in tagcheck";_tag_fat(_tmp425,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp976,_tag_fat(_tmp424,sizeof(void*),0U));});});}_LL119:;}
# 2822
goto _LL7;}case 37U: _LL56: _tmp47C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_LL57:
 Cyc_Toc_stmt_to_c(nv,_tmp47C);goto _LL7;case 40U: _LL58: _LL59:
 goto _LL7;case 36U: _LL5A: _LL5B:
({void*_tmp427=0U;({struct _fat_ptr _tmp977=({const char*_tmp428="UnresolvedMem";_tag_fat(_tmp428,sizeof(char),14U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp977,_tag_fat(_tmp427,sizeof(void*),0U));});});case 25U: _LL5C: _LL5D:
({void*_tmp429=0U;({struct _fat_ptr _tmp978=({const char*_tmp42A="compoundlit";_tag_fat(_tmp42A,sizeof(char),12U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp978,_tag_fat(_tmp429,sizeof(void*),0U));});});default: _LL5E: _LL5F:
({void*_tmp42B=0U;({struct _fat_ptr _tmp979=({const char*_tmp42C="valueof(-)";_tag_fat(_tmp42C,sizeof(char),11U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp979,_tag_fat(_tmp42B,sizeof(void*),0U));});});}_LL7:;}
# 2829
if(!did_inserted_checks)
({void*_tmp47D=0U;({unsigned _tmp97D=e->loc;struct _fat_ptr _tmp97C=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp480=({struct Cyc_String_pa_PrintArg_struct _tmp766;_tmp766.tag=0U,({
struct _fat_ptr _tmp97A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp766.f1=_tmp97A;});_tmp766;});void*_tmp47E[1U];_tmp47E[0]=& _tmp480;({struct _fat_ptr _tmp97B=({const char*_tmp47F="Toc did not examine an inserted check: %s";_tag_fat(_tmp47F,sizeof(char),42U);});Cyc_aprintf(_tmp97B,_tag_fat(_tmp47E,sizeof(void*),1U));});});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmp97D,_tmp97C,_tag_fat(_tmp47D,sizeof(void*),0U));});});};}struct _tuple36{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2858 "toc.cyc"
static struct _tuple36*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2860
return({struct _tuple36*_tmp481=_region_malloc(r,sizeof(*_tmp481));_tmp481->f1=0,({struct _fat_ptr*_tmp97F=Cyc_Toc_fresh_label();_tmp481->f2=_tmp97F;}),({struct _fat_ptr*_tmp97E=Cyc_Toc_fresh_label();_tmp481->f3=_tmp97E;}),_tmp481->f4=sc;_tmp481;});}
# 2866
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp482=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2872
if((int)(((_tmp482->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp485=_tmp482->orig_pat;if((_tmp485.pattern).tag != 1)_throw_match();(_tmp485.pattern).val;})->topt);
void*_tmp483=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp484=_tmp483;switch(*((int*)_tmp484)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp484)->f1)){case 0U: _LL1: _LL2:
# 2877
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmp980=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp980,v);});goto _LL0;}_LL0:;}{
# 2883
void*_tmp486=_tmp482->access;void*_tmp487=_tmp486;int _tmp490;struct _fat_ptr*_tmp48F;unsigned _tmp48E;unsigned _tmp48D;switch(*((int*)_tmp487)){case 0U: _LLA: _LLB:
# 2887
 goto _LL9;case 1U: _LLC: _LLD:
# 2892
 if(ps->tl != 0){
void*_tmp488=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp489=_tmp488;struct Cyc_Absyn_Datatypedecl*_tmp48C;struct Cyc_Absyn_Datatypefield*_tmp48B;unsigned _tmp48A;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp489)->tag == 3U){_LL15: _tmp48C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp489)->f1;_tmp48B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp489)->f2;_tmp48A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp489)->f3;_LL16:
# 2895
 ps=ps->tl;
v=({void*_tmp982=({void*_tmp981=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp48B->name,_tmp48C->name));Cyc_Absyn_cstar_type(_tmp981,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp982,v);});
v=({struct Cyc_Absyn_Exp*_tmp983=v;Cyc_Absyn_aggrarrow_exp(_tmp983,Cyc_Absyn_fieldname((int)(_tmp48A + (unsigned)1)),0U);});
continue;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2902
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp48D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp487)->f3;_LLF:
 _tmp48E=_tmp48D;goto _LL11;case 2U: _LL10: _tmp48E=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp487)->f1;_LL11:
 v=({struct Cyc_Absyn_Exp*_tmp984=v;Cyc_Toc_member_exp(_tmp984,Cyc_Absyn_fieldname((int)(_tmp48E + (unsigned)1)),0U);});goto _LL9;default: _LL12: _tmp490=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp487)->f1;_tmp48F=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp487)->f2;_LL13:
# 2907
 v=Cyc_Toc_member_exp(v,_tmp48F,0U);
if(_tmp490)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}_LL9:;};}
# 2913
return v;}
# 2918
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp491=t;struct Cyc_Absyn_Datatypedecl*_tmp4AA;struct Cyc_Absyn_Datatypefield*_tmp4A9;struct _fat_ptr*_tmp4A8;int _tmp4A7;int _tmp4A6;struct Cyc_Absyn_Datatypedecl*_tmp4A5;struct Cyc_Absyn_Datatypefield*_tmp4A4;unsigned _tmp4A3;struct _fat_ptr _tmp4A2;int _tmp4A1;void*_tmp4A0;struct Cyc_Absyn_Enumfield*_tmp49F;struct Cyc_Absyn_Enumdecl*_tmp49E;struct Cyc_Absyn_Enumfield*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;switch(*((int*)_tmp491)){case 0U: _LL1: _tmp49C=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp491)->f1;_LL2:
 return _tmp49C == 0?v: _tmp49C;case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp985=v;Cyc_Absyn_eq_exp(_tmp985,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp986=v;Cyc_Absyn_neq_exp(_tmp986,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp49E=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp491)->f1;_tmp49D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp491)->f2;_LL8:
 return({struct Cyc_Absyn_Exp*_tmp987=v;Cyc_Absyn_eq_exp(_tmp987,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->tag=32U,_tmp492->f1=_tmp49E,_tmp492->f2=_tmp49D;_tmp492;}),0U),0U);});case 4U: _LL9: _tmp4A0=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp491)->f1;_tmp49F=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp491)->f2;_LLA:
 return({struct Cyc_Absyn_Exp*_tmp988=v;Cyc_Absyn_eq_exp(_tmp988,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->tag=33U,_tmp493->f1=_tmp4A0,_tmp493->f2=_tmp49F;_tmp493;}),0U),0U);});case 5U: _LLB: _tmp4A2=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp491)->f1;_tmp4A1=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp491)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp989=v;Cyc_Absyn_eq_exp(_tmp989,Cyc_Absyn_float_exp(_tmp4A2,_tmp4A1,0U),0U);});case 6U: _LLD: _tmp4A3=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp491)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp98A=v;Cyc_Absyn_eq_exp(_tmp98A,Cyc_Absyn_signed_int_exp((int)_tmp4A3,0U),0U);});case 7U: _LLF: _tmp4A6=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp491)->f1;_tmp4A5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp491)->f2;_tmp4A4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp491)->f3;_LL10:
# 2930
 LOOP1: {
void*_tmp494=v->r;void*_tmp495=_tmp494;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp496;switch(*((int*)_tmp495)){case 14U: _LL16: _tmp496=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL17:
 v=_tmp496;goto LOOP1;case 20U: _LL18: _tmp497=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL19:
 v=_tmp497;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2937
v=({void*_tmp98C=({void*_tmp98B=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4A4->name,_tmp4A5->name));Cyc_Absyn_cstar_type(_tmp98B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp98C,v);});return({struct Cyc_Absyn_Exp*_tmp98D=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp98D,Cyc_Absyn_uint_exp((unsigned)_tmp4A6,0U),0U);});case 8U: _LL11: _tmp4A8=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp491)->f1;_tmp4A7=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp491)->f2;_LL12:
# 2941
 return({struct Cyc_Absyn_Exp*_tmp98F=({struct Cyc_Absyn_Exp*_tmp98E=Cyc_Toc_member_exp(v,_tmp4A8,0U);Cyc_Toc_member_exp(_tmp98E,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmp98F,
Cyc_Absyn_signed_int_exp(_tmp4A7,0U),0U);});default: _LL13: _tmp4AA=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp491)->f1;_tmp4A9=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp491)->f2;_LL14:
# 2945
 LOOP2: {
void*_tmp498=v->r;void*_tmp499=_tmp498;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;switch(*((int*)_tmp499)){case 14U: _LL1D: _tmp49A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL1E:
 v=_tmp49A;goto LOOP2;case 20U: _LL1F: _tmp49B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp499)->f1;_LL20:
 v=_tmp49B;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2952
v=({void*_tmp991=({void*_tmp990=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4A9->name,_tmp4AA->name));Cyc_Absyn_cstar_type(_tmp990,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp991,v);});
return({struct Cyc_Absyn_Exp*_tmp992=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp992,Cyc_Absyn_var_exp(_tmp4A9->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2964
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple37{int f1;void*f2;};
# 2971
static struct _tuple37 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2973
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp4AB=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp4AC=_tmp4AB;void*_tmp4B2;_LL1: _tmp4B2=_tmp4AC.f1;_LL2:;{
void*_tmp4AD=_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;struct _fat_ptr*_tmp4B0;switch(*((int*)_tmp4AD)){case 3U: _LL4: _LL5:
# 2979
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4B0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4AD)->f1;_LLB:
# 2983
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->tag=3U,_tmp4AE->f1=_tmp4B0;_tmp4AE;});
continue;case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4B1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4AD)->f1;if(_tmp4B1 != 0){_LLF:
# 2988
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->tag=2U,_tmp4AF->f1=_tmp4B1;_tmp4AF;});return({struct _tuple37 _tmp767;_tmp767.f1=0,_tmp767.f2=k;_tmp767;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple37 _tmp768;_tmp768.f1=0,_tmp768.f2=k;_tmp768;});}_LL3:;};}
# 2996
return({struct _tuple37 _tmp769;_tmp769.f1=c,_tmp769.f2=k;_tmp769;});}
# 3001
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4B3=p;int _tmp4BE;int _tmp4BD;unsigned _tmp4BC;void*_tmp4BB;struct Cyc_Absyn_Enumfield*_tmp4BA;struct Cyc_Absyn_Enumdecl*_tmp4B9;struct Cyc_Absyn_Enumfield*_tmp4B8;switch(*((int*)_tmp4B3)){case 3U: _LL1: _tmp4B9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4B3)->f1;_tmp4B8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4B3)->f2;_LL2:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->tag=32U,_tmp4B4->f1=_tmp4B9,_tmp4B4->f2=_tmp4B8;_tmp4B4;}),0U);goto _LL0;case 4U: _LL3: _tmp4BB=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4B3)->f1;_tmp4BA=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4B3)->f2;_LL4:
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->tag=33U,_tmp4B5->f1=_tmp4BB,_tmp4B5->f2=_tmp4BA;_tmp4B5;}),0U);goto _LL0;case 6U: _LL5: _tmp4BC=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4B3)->f1;_LL6:
 _tmp4BD=(int)_tmp4BC;goto _LL8;case 7U: _LL7: _tmp4BD=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4B3)->f1;_LL8:
 _tmp4BE=_tmp4BD;goto _LLA;case 8U: _LL9: _tmp4BE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4B3)->f2;_LLA:
 e=Cyc_Absyn_uint_exp((unsigned)_tmp4BE,0U);goto _LL0;default: _LLB: _LLC:
({void*_tmp4B6=0U;({struct _fat_ptr _tmp993=({const char*_tmp4B7="compile_pat_test_as_case!";_tag_fat(_tmp4B7,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp993,_tag_fat(_tmp4B6,sizeof(void*),0U));});});}_LL0:;}
# 3011
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->tag=17U,_tmp4BF->f1=e;_tmp4BF;}),0U);}
# 3015
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple38{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple39{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3026
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3030
void*t=(void*)_check_null(p->topt);
void*_tmp4C0=p->r;void*_tmp4C1=_tmp4C0;union Cyc_Absyn_AggrInfo _tmp4F8;struct Cyc_List_List*_tmp4F7;struct Cyc_List_List*_tmp4F6;struct Cyc_List_List*_tmp4F5;struct Cyc_Absyn_Pat*_tmp4F4;struct Cyc_Absyn_Datatypedecl*_tmp4F3;struct Cyc_Absyn_Datatypefield*_tmp4F2;struct Cyc_List_List*_tmp4F1;struct Cyc_Absyn_Vardecl*_tmp4F0;struct Cyc_Absyn_Pat*_tmp4EF;struct Cyc_Absyn_Vardecl*_tmp4EE;struct Cyc_Absyn_Vardecl*_tmp4ED;struct Cyc_Absyn_Pat*_tmp4EC;struct Cyc_Absyn_Vardecl*_tmp4EB;switch(*((int*)_tmp4C1)){case 2U: _LL1: _tmp4EB=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4C1)->f2;_LL2: {
# 3033
struct Cyc_Absyn_Pat*_tmp4C2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4C2->topt=p->topt;
_tmp4ED=_tmp4EB;_tmp4EC=_tmp4C2;goto _LL4;}case 1U: _LL3: _tmp4ED=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1;_tmp4EC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4C1)->f2;_LL4: {
# 3038
struct Cyc_Absyn_Vardecl*_tmp4C3=({struct _tuple1*_tmp994=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp994,Cyc_Toc_typ_to_c_array(_tmp4ED->type),0);});
({struct Cyc_List_List*_tmp996=({struct Cyc_List_List*_tmp4C5=_region_malloc(rgn,sizeof(*_tmp4C5));({struct _tuple38*_tmp995=({struct _tuple38*_tmp4C4=_region_malloc(rgn,sizeof(*_tmp4C4));_tmp4C4->f1=_tmp4ED,_tmp4C4->f2=_tmp4C3;_tmp4C4;});_tmp4C5->hd=_tmp995;}),_tmp4C5->tl=*decls;_tmp4C5;});*decls=_tmp996;});{
struct Cyc_Absyn_Stmt*_tmp4C6=({struct Cyc_Absyn_Exp*_tmp997=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7->tag=4U,_tmp4C7->f1=_tmp4C3;_tmp4C7;}),0U);Cyc_Absyn_assign_stmt(_tmp997,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp998=_tmp4C6;Cyc_Toc_seq_stmt_opt(_tmp998,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp4EC));});};}case 4U: _LL5: _tmp4EE=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4C1)->f2;_LL6: {
# 3044
struct Cyc_Absyn_Vardecl*_tmp4C8=({struct _tuple1*_tmp999=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp999,Cyc_Toc_typ_to_c_array(_tmp4EE->type),0);});
({struct Cyc_List_List*_tmp99B=({struct Cyc_List_List*_tmp4CA=_region_malloc(rgn,sizeof(*_tmp4CA));({struct _tuple38*_tmp99A=({struct _tuple38*_tmp4C9=_region_malloc(rgn,sizeof(*_tmp4C9));_tmp4C9->f1=_tmp4EE,_tmp4C9->f2=_tmp4C8;_tmp4C9;});_tmp4CA->hd=_tmp99A;}),_tmp4CA->tl=*decls;_tmp4CA;});*decls=_tmp99B;});
return({struct Cyc_Absyn_Exp*_tmp99C=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->tag=4U,_tmp4CB->f1=_tmp4C8;_tmp4CB;}),0U);Cyc_Absyn_assign_stmt(_tmp99C,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp4F0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1;_tmp4EF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4C1)->f2;_LLA:
# 3050
({void*_tmp99D=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp4F0->type=_tmp99D;});{
struct Cyc_Absyn_Vardecl*_tmp4CC=({struct _tuple1*_tmp99E=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp99E,Cyc_Toc_typ_to_c_array(_tmp4F0->type),0);});
({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp4CE=_region_malloc(rgn,sizeof(*_tmp4CE));({struct _tuple38*_tmp99F=({struct _tuple38*_tmp4CD=_region_malloc(rgn,sizeof(*_tmp4CD));_tmp4CD->f1=_tmp4F0,_tmp4CD->f2=_tmp4CC;_tmp4CD;});_tmp4CE->hd=_tmp99F;}),_tmp4CE->tl=*decls;_tmp4CE;});*decls=_tmp9A0;});{
# 3054
struct Cyc_Absyn_Stmt*_tmp4CF=({struct Cyc_Absyn_Exp*_tmp9A3=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->tag=4U,_tmp4D0->f1=_tmp4CC;_tmp4D0;}),0U);Cyc_Absyn_assign_stmt(_tmp9A3,({
void*_tmp9A2=({void*_tmp9A1=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp9A1,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9A2,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9A4=_tmp4CF;Cyc_Toc_seq_stmt_opt(_tmp9A4,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp4EF));});};};case 9U: _LLB: _LLC:
# 3059
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1)->r)->tag == 8U){_LL17: _tmp4F3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1)->r)->f1;_tmp4F2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1)->r)->f2;_tmp4F1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1)->r)->f3;_LL18:
# 3068
 if(_tmp4F1 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp4F2->name,_tmp4F3->name);
void*_tmp4D1=({void*_tmp9A5=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp9A5,Cyc_Toc_mt_tq);});
path=Cyc_Toc_cast_it(_tmp4D1,path);{
int cnt=1;
struct Cyc_List_List*_tmp4D2=_tmp4F2->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp4F1 != 0;(_tmp4F1=_tmp4F1->tl,_tmp4D2=((struct Cyc_List_List*)_check_null(_tmp4D2))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4D3=(struct Cyc_Absyn_Pat*)_tmp4F1->hd;
if(_tmp4D3->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4D4=(*((struct _tuple13*)((struct Cyc_List_List*)_check_null(_tmp4D2))->hd)).f2;
void*_tmp4D5=(void*)_check_null(_tmp4D3->topt);
void*_tmp4D6=Cyc_Toc_typ_to_c_array(_tmp4D5);
struct Cyc_Absyn_Exp*_tmp4D7=({struct Cyc_Absyn_Exp*_tmp9A6=path;Cyc_Absyn_aggrarrow_exp(_tmp9A6,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4D4)))
_tmp4D7=Cyc_Toc_cast_it(_tmp4D6,_tmp4D7);
s=({struct Cyc_Absyn_Stmt*_tmp9A7=s;Cyc_Toc_seq_stmt_opt(_tmp9A7,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4D7,_tmp4D3));});};}
# 3086
return s;};};}else{_LL21: _tmp4F4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1;_LL22:
# 3131
 return({struct _RegionHandle*_tmp9AB=rgn;struct Cyc_Toc_Env**_tmp9AA=nv;struct Cyc_List_List**_tmp9A9=decls;struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp9AB,_tmp9AA,_tmp9A9,_tmp9A8,_tmp4F4);});}case 8U: _LL19: _tmp4F5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4C1)->f3;_LL1A:
# 3088
 _tmp4F6=_tmp4F5;goto _LL1C;case 5U: _LL1B: _tmp4F6=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1;_LL1C: {
# 3090
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp4F6 != 0;(_tmp4F6=_tmp4F6->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4D8=(struct Cyc_Absyn_Pat*)_tmp4F6->hd;
if(_tmp4D8->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp4D9=(void*)_check_null(_tmp4D8->topt);
struct _fat_ptr*_tmp4DA=Cyc_Absyn_fieldname(cnt);
s=({struct Cyc_Absyn_Stmt*_tmp9B0=s;Cyc_Toc_seq_stmt_opt(_tmp9B0,({struct _RegionHandle*_tmp9AF=rgn;struct Cyc_Toc_Env**_tmp9AE=nv;struct Cyc_List_List**_tmp9AD=decls;struct Cyc_Absyn_Exp*_tmp9AC=Cyc_Toc_member_exp(path,_tmp4DA,0U);Cyc_Toc_extract_pattern_vars(_tmp9AF,_tmp9AE,_tmp9AD,_tmp9AC,_tmp4D8);}));});};}
# 3100
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1 == 0){_LL1D: _LL1E:
({void*_tmp4DB=0U;({struct _fat_ptr _tmp9B1=({const char*_tmp4DC="unresolved aggregate pattern!";_tag_fat(_tmp4DC,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B1,_tag_fat(_tmp4DB,sizeof(void*),0U));});});}else{_LL1F: _tmp4F8=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C1)->f1;_tmp4F7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4C1)->f3;_LL20: {
# 3104
struct Cyc_Absyn_Aggrdecl*_tmp4DD=Cyc_Absyn_get_known_aggrdecl(_tmp4F8);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp4F7 != 0;_tmp4F7=_tmp4F7->tl){
struct _tuple39*_tmp4DE=(struct _tuple39*)_tmp4F7->hd;
struct Cyc_Absyn_Pat*_tmp4DF=(*_tmp4DE).f2;
if(_tmp4DF->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp4E0=Cyc_Absyn_designatorlist_to_fieldname((*_tmp4DE).f1);
void*_tmp4E1=(void*)_check_null(_tmp4DF->topt);
void*_tmp4E2=Cyc_Toc_typ_to_c_array(_tmp4E1);
struct Cyc_Absyn_Exp*_tmp4E3=Cyc_Toc_member_exp(path,_tmp4E0,0U);
# 3116
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DD->impl))->tagged)_tmp4E3=Cyc_Toc_member_exp(_tmp4E3,Cyc_Toc_val_sp,0U);{
void*_tmp4E4=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DD->impl))->fields,_tmp4E0)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp4E4))
_tmp4E3=Cyc_Toc_cast_it(_tmp4E2,_tmp4E3);else{
if(!Cyc_Tcutil_is_array_type(_tmp4E4)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4E4)))
# 3123
_tmp4E3=Cyc_Absyn_deref_exp(({void*_tmp9B2=Cyc_Absyn_cstar_type(_tmp4E2,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9B2,
Cyc_Absyn_address_exp(_tmp4E3,0U));}),0U);}
# 3126
s=({struct Cyc_Absyn_Stmt*_tmp9B3=s;Cyc_Toc_seq_stmt_opt(_tmp9B3,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4E3,_tmp4DF));});};};}
# 3128
return s;}}case 15U: _LL23: _LL24:
# 3133
({void*_tmp4E5=0U;({struct _fat_ptr _tmp9B4=({const char*_tmp4E6="unknownid pat";_tag_fat(_tmp4E6,sizeof(char),14U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B4,_tag_fat(_tmp4E5,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp4E7=0U;({struct _fat_ptr _tmp9B5=({const char*_tmp4E8="unknowncall pat";_tag_fat(_tmp4E8,sizeof(char),16U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B5,_tag_fat(_tmp4E7,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp4E9=0U;({struct _fat_ptr _tmp9B6=({const char*_tmp4EA="exp pat";_tag_fat(_tmp4EA,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B6,_tag_fat(_tmp4E9,sizeof(void*),0U));});});}_LL0:;}
# 3139
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple38 _tmp4F9=*((struct _tuple38*)vs->hd);struct _tuple38 _tmp4FA=_tmp4F9;struct Cyc_Absyn_Vardecl*_tmp4FC;struct Cyc_Absyn_Vardecl*_tmp4FB;_LL1: _tmp4FC=_tmp4FA.f1;_tmp4FB=_tmp4FA.f2;_LL2:;
if(_tmp4FC == _tmp4FB)return _tmp4FB;}
# 3144
({void*_tmp4FD=0U;({struct _fat_ptr _tmp9B7=({const char*_tmp4FE="find_new_var";_tag_fat(_tmp4FE,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9B7,_tag_fat(_tmp4FD,sizeof(void*),0U));});});}
# 3150
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp4FF=e->r;void*_tmp500=_tmp4FF;struct Cyc_List_List*_tmp531;struct Cyc_List_List*_tmp530;struct Cyc_List_List*_tmp52F;struct Cyc_List_List*_tmp52E;struct Cyc_List_List*_tmp52D;struct Cyc_List_List*_tmp52C;struct Cyc_List_List*_tmp52B;struct Cyc_List_List*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50B;switch(*((int*)_tmp500)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp500)->f1)){case 5U: _LL1: _tmp50B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp500)->f1)->f1;_LL2:
 _tmp50C=_tmp50B;goto _LL4;case 4U: _LL3: _tmp50C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp500)->f1)->f1;_LL4:
# 3154
{struct _handler_cons _tmp501;_push_handler(& _tmp501);{int _tmp503=0;if(setjmp(_tmp501.handler))_tmp503=1;if(!_tmp503){
({void*_tmp9BA=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->tag=1U,({void*_tmp9B9=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->tag=4U,({struct Cyc_Absyn_Vardecl*_tmp9B8=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,_tmp50C);_tmp504->f1=_tmp9B8;});_tmp504;});_tmp505->f1=_tmp9B9;});_tmp505;});e->r=_tmp9BA;});;_pop_handler();}else{void*_tmp502=(void*)Cyc_Core_get_exn_thrown();void*_tmp506=_tmp502;void*_tmp507;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp506)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3157
 goto _LL3F;}else{_LL42: _tmp507=_tmp506;_LL43:(int)_rethrow(_tmp507);}_LL3F:;}};}
# 3159
goto _LL0;default: goto _LL3D;}case 6U: _LL5: _tmp50F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_tmp50E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_tmp50D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp500)->f3;_LL6:
# 3161
 Cyc_Toc_subst_pattern_vars(env,_tmp50F);_tmp511=_tmp50E;_tmp510=_tmp50D;goto _LL8;case 27U: _LL7: _tmp511=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_tmp510=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp500)->f3;_LL8:
 _tmp513=_tmp511;_tmp512=_tmp510;goto _LLA;case 7U: _LL9: _tmp513=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_tmp512=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LLA:
 _tmp515=_tmp513;_tmp514=_tmp512;goto _LLC;case 8U: _LLB: _tmp515=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_tmp514=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LLC:
 _tmp517=_tmp515;_tmp516=_tmp514;goto _LLE;case 23U: _LLD: _tmp517=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_tmp516=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LLE:
 _tmp519=_tmp517;_tmp518=_tmp516;goto _LL10;case 9U: _LLF: _tmp519=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_tmp518=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL10:
# 3167
 Cyc_Toc_subst_pattern_vars(env,_tmp519);_tmp51A=_tmp518;goto _LL12;case 41U: _LL11: _tmp51A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL12:
 _tmp51B=_tmp51A;goto _LL14;case 38U: _LL13: _tmp51B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL14:
 _tmp51C=_tmp51B;goto _LL16;case 11U: _LL15: _tmp51C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL16:
 _tmp51D=_tmp51C;goto _LL18;case 12U: _LL17: _tmp51D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL18:
 _tmp51E=_tmp51D;goto _LL1A;case 13U: _LL19: _tmp51E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL1A:
 _tmp51F=_tmp51E;goto _LL1C;case 14U: _LL1B: _tmp51F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL1C:
 _tmp520=_tmp51F;goto _LL1E;case 18U: _LL1D: _tmp520=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL1E:
 _tmp521=_tmp520;goto _LL20;case 20U: _LL1F: _tmp521=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL20:
 _tmp522=_tmp521;goto _LL22;case 21U: _LL21: _tmp522=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL22:
 _tmp523=_tmp522;goto _LL24;case 22U: _LL23: _tmp523=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL24:
 _tmp524=_tmp523;goto _LL26;case 28U: _LL25: _tmp524=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL26:
 _tmp525=_tmp524;goto _LL28;case 15U: _LL27: _tmp525=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL28:
 Cyc_Toc_subst_pattern_vars(env,_tmp525);goto _LL0;case 34U: _LL29: _tmp527=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp500)->f1).rgn;_tmp526=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp500)->f1).num_elts;_LL2A:
 _tmp529=_tmp527;_tmp528=_tmp526;goto _LL2C;case 16U: _LL2B: _tmp529=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_tmp528=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL2C:
# 3182
 if(_tmp529 != 0)Cyc_Toc_subst_pattern_vars(env,_tmp529);
Cyc_Toc_subst_pattern_vars(env,_tmp528);goto _LL0;case 3U: _LL2D: _tmp52A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL2E:
 _tmp52B=_tmp52A;goto _LL30;case 31U: _LL2F: _tmp52B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL30:
 _tmp52C=_tmp52B;goto _LL32;case 24U: _LL31: _tmp52C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL32:
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,_tmp52C);goto _LL0;case 36U: _LL33: _tmp52D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL34:
 _tmp52E=_tmp52D;goto _LL36;case 29U: _LL35: _tmp52E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp500)->f3;_LL36:
 _tmp52F=_tmp52E;goto _LL38;case 30U: _LL37: _tmp52F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL38:
 _tmp530=_tmp52F;goto _LL3A;case 26U: _LL39: _tmp530=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp500)->f1;_LL3A:
 _tmp531=_tmp530;goto _LL3C;case 25U: _LL3B: _tmp531=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp500)->f2;_LL3C:
# 3192
 for(0;_tmp531 != 0;_tmp531=_tmp531->tl){
struct _tuple20*_tmp508=(struct _tuple20*)_tmp531->hd;struct _tuple20*_tmp509=_tmp508;struct Cyc_Absyn_Exp*_tmp50A;_LL45: _tmp50A=_tmp509->f2;_LL46:;
Cyc_Toc_subst_pattern_vars(env,_tmp50A);}
# 3196
goto _LL0;default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple40{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3206
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3214
void*_tmp532=dopt;struct Cyc_List_List*_tmp584;struct Cyc_List_List*_tmp583;void*_tmp582;struct Cyc_Tcpat_Rhs*_tmp581;if(_tmp532 == 0){_LL1: _LL2:
# 3216
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp532)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp581=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp532)->f1;_LL6:
# 3220
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp533=(struct _tuple36*)lscs->hd;struct _tuple36*_tmp534=_tmp533;int*_tmp545;struct _fat_ptr*_tmp544;struct _fat_ptr*_tmp543;struct Cyc_Absyn_Switch_clause*_tmp542;_LLA: _tmp545=(int*)& _tmp534->f1;_tmp544=_tmp534->f2;_tmp543=_tmp534->f3;_tmp542=_tmp534->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp535=_tmp542->body;
if(_tmp535 == _tmp581->rhs){
# 3225
if(*_tmp545)
return Cyc_Absyn_goto_stmt(_tmp544,0U);
*_tmp545=1;{
struct Cyc_List_List*newdecls=0;
# 3230
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9BC=rgn;struct Cyc_Absyn_Exp*_tmp9BB=
Cyc_Absyn_var_exp(v,0U);
# 3230
Cyc_Toc_extract_pattern_vars(_tmp9BC,& nv,& newdecls,_tmp9BB,_tmp542->pattern);});
# 3234
struct Cyc_Absyn_Stmt*res=_tmp542->body;
{struct Cyc_List_List*_tmp536=newdecls;for(0;_tmp536 != 0;_tmp536=_tmp536->tl){
struct _tuple38 _tmp537=*((struct _tuple38*)_tmp536->hd);struct _tuple38 _tmp538=_tmp537;struct Cyc_Absyn_Vardecl*_tmp53F;struct Cyc_Absyn_Vardecl*_tmp53E;_LLD: _tmp53F=_tmp538.f1;_tmp53E=_tmp538.f2;_LLE:;
({struct Cyc_List_List*_tmp9BE=({struct Cyc_List_List*_tmp53A=_region_malloc(rgn,sizeof(*_tmp53A));({struct _tuple38*_tmp9BD=({struct _tuple38*_tmp539=_region_malloc(rgn,sizeof(*_tmp539));_tmp539->f1=_tmp53F,_tmp539->f2=_tmp53E;_tmp539;});_tmp53A->hd=_tmp9BD;}),_tmp53A->tl=*decls;_tmp53A;});*decls=_tmp9BE;});
({struct Cyc_Absyn_Exp*_tmp9BF=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->tag=4U,_tmp53B->f1=_tmp53E;_tmp53B;}),0U);_tmp53F->initializer=_tmp9BF;});
((struct Cyc_Absyn_Exp*)_check_null(_tmp53F->initializer))->topt=_tmp53E->type;
_tmp53F->type=_tmp53E->type;
res=({struct Cyc_Absyn_Decl*_tmp9C1=({struct Cyc_Absyn_Decl*_tmp53D=_cycalloc(sizeof(*_tmp53D));({void*_tmp9C0=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->tag=0U,_tmp53C->f1=_tmp53F;_tmp53C;});_tmp53D->r=_tmp9C0;}),_tmp53D->loc=0U;_tmp53D;});Cyc_Absyn_decl_stmt(_tmp9C1,res,0U);});}}
# 3243
res=Cyc_Absyn_label_stmt(_tmp543,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(_tmp544,res,0U);
({struct Cyc_List_List*_tmp9C3=({struct Cyc_List_List*_tmp541=_region_malloc(rgn,sizeof(*_tmp541));({struct _tuple40*_tmp9C2=({struct _tuple40*_tmp540=_region_malloc(rgn,sizeof(*_tmp540));_tmp540->f1=nv,_tmp540->f2=_tmp543,_tmp540->f3=_tmp535;_tmp540;});_tmp541->hd=_tmp9C2;}),_tmp541->tl=*bodies;_tmp541;});*bodies=_tmp9C3;});
return res;};}};}
# 3251
({void*_tmp546=0U;({struct _fat_ptr _tmp9C4=({const char*_tmp547="couldn't find rhs!";_tag_fat(_tmp547,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9C4,_tag_fat(_tmp546,sizeof(void*),0U));});});default: _LL7: _tmp584=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp532)->f1;_tmp583=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp532)->f2;_tmp582=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp532)->f3;_LL8: {
# 3254
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp582,lscs,v);
# 3256
struct Cyc_Absyn_Exp*_tmp548=({struct Cyc_List_List*_tmp9C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp584);Cyc_Toc_compile_path(_tmp9C5,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp583);
# 3259
struct _tuple37 _tmp54A=Cyc_Toc_admits_switch(_tmp549);struct _tuple37 _tmp54B=_tmp54A;int _tmp580;void*_tmp57F;_LL10: _tmp580=_tmp54B.f1;_tmp57F=_tmp54B.f2;_LL11:;
if(_tmp580 > 1){
# 3263
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp55C=_cycalloc(sizeof(*_tmp55C));
({struct Cyc_Absyn_Switch_clause*_tmp9C7=({struct Cyc_Absyn_Switch_clause*_tmp55B=_cycalloc(sizeof(*_tmp55B));({struct Cyc_Absyn_Pat*_tmp9C6=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp55B->pattern=_tmp9C6;}),_tmp55B->pat_vars=0,_tmp55B->where_clause=0,_tmp55B->body=res,_tmp55B->loc=0U;_tmp55B;});_tmp55C->hd=_tmp9C7;}),_tmp55C->tl=0;_tmp55C;});
# 3266
for(0;_tmp549 != 0;_tmp549=_tmp549->tl){
struct _tuple33 _tmp54C=*((struct _tuple33*)_tmp549->hd);struct _tuple33 _tmp54D=_tmp54C;void*_tmp553;void*_tmp552;_LL13: _tmp553=_tmp54D.f1;_tmp552=_tmp54D.f2;_LL14:;{
# 3269
struct Cyc_Absyn_Pat*_tmp54E=Cyc_Toc_compile_pat_test_as_case(_tmp553);
# 3271
struct Cyc_Absyn_Stmt*_tmp54F=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp552,lscs,v);
# 3273
new_lscs=({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));({struct Cyc_Absyn_Switch_clause*_tmp9C8=({struct Cyc_Absyn_Switch_clause*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->pattern=_tmp54E,_tmp550->pat_vars=0,_tmp550->where_clause=0,_tmp550->body=_tmp54F,_tmp550->loc=0U;_tmp550;});_tmp551->hd=_tmp9C8;}),_tmp551->tl=new_lscs;_tmp551;});};}
# 3275
{void*_tmp554=_tmp57F;struct _fat_ptr*_tmp559;switch(*((int*)_tmp554)){case 1U: _LL16: _LL17:
# 3277
 LOOP1: {
void*_tmp555=_tmp548->r;void*_tmp556=_tmp555;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp557;switch(*((int*)_tmp556)){case 14U: _LL1F: _tmp557=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp556)->f2;_LL20:
 _tmp548=_tmp557;goto LOOP1;case 20U: _LL21: _tmp558=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp556)->f1;_LL22:
 _tmp548=_tmp558;goto _LL1E;default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3283
_tmp548=Cyc_Absyn_deref_exp(({void*_tmp9C9=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9C9,_tmp548);}),0U);goto _LL15;case 3U: _LL18: _tmp559=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp554)->f1;_LL19:
# 3285
 _tmp548=({struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Toc_member_exp(_tmp548,_tmp559,0U);Cyc_Toc_member_exp(_tmp9CA,Cyc_Toc_tag_sp,0U);});goto _LL15;case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3289
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->tag=10U,_tmp55A->f1=_tmp548,_tmp55A->f2=new_lscs,_tmp55A->f3=0;_tmp55A;}),0U);}else{
# 3293
void*_tmp55D=_tmp57F;struct Cyc_Absyn_Exp*_tmp57E;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp55D)->tag == 2U){_LL26: _tmp57E=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp55D)->f1;_LL27: {
# 3295
struct Cyc_List_List*_tmp55E=_tmp549;void*_tmp577;struct Cyc_Tcpat_Rhs*_tmp576;if(_tmp55E != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp55E)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp55E)->tl == 0){_LL2B: _tmp577=((struct _tuple33*)_tmp55E->hd)->f1;_tmp576=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp55E->hd)->f2)->f1;_LL2C:
# 3301
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple36*_tmp55F=(struct _tuple36*)lscs->hd;struct _tuple36*_tmp560=_tmp55F;int*_tmp571;struct _fat_ptr*_tmp570;struct _fat_ptr*_tmp56F;struct Cyc_Absyn_Switch_clause*_tmp56E;_LL30: _tmp571=(int*)& _tmp560->f1;_tmp570=_tmp560->f2;_tmp56F=_tmp560->f3;_tmp56E=_tmp560->f4;_LL31:;{
struct Cyc_Absyn_Stmt*_tmp561=_tmp56E->body;
if(_tmp561 == _tmp576->rhs){
# 3306
if(*_tmp571)
return Cyc_Absyn_goto_stmt(_tmp570,0U);
*_tmp571=1;{
struct Cyc_List_List*newdecls=0;
# 3311
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmp9CC=rgn;struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmp9CC,& nv,& newdecls,_tmp9CB,_tmp56E->pattern);});
# 3315
struct Cyc_Absyn_Stmt*r=_tmp56E->body;
{struct Cyc_List_List*_tmp562=newdecls;for(0;_tmp562 != 0;_tmp562=_tmp562->tl){
struct _tuple38 _tmp563=*((struct _tuple38*)_tmp562->hd);struct _tuple38 _tmp564=_tmp563;struct Cyc_Absyn_Vardecl*_tmp56B;struct Cyc_Absyn_Vardecl*_tmp56A;_LL33: _tmp56B=_tmp564.f1;_tmp56A=_tmp564.f2;_LL34:;
({struct Cyc_List_List*_tmp9CE=({struct Cyc_List_List*_tmp566=_region_malloc(rgn,sizeof(*_tmp566));({struct _tuple38*_tmp9CD=({struct _tuple38*_tmp565=_region_malloc(rgn,sizeof(*_tmp565));_tmp565->f1=_tmp56B,_tmp565->f2=_tmp56A;_tmp565;});_tmp566->hd=_tmp9CD;}),_tmp566->tl=*decls;_tmp566;});*decls=_tmp9CE;});
({struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->tag=4U,_tmp567->f1=_tmp56A;_tmp567;}),0U);_tmp56B->initializer=_tmp9CF;});
((struct Cyc_Absyn_Exp*)_check_null(_tmp56B->initializer))->topt=_tmp56A->type;
_tmp56B->type=_tmp56A->type;
r=({struct Cyc_Absyn_Decl*_tmp9D1=({struct Cyc_Absyn_Decl*_tmp569=_cycalloc(sizeof(*_tmp569));({void*_tmp9D0=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=0U,_tmp568->f1=_tmp56B;_tmp568;});_tmp569->r=_tmp9D0;}),_tmp569->loc=0U;_tmp569;});Cyc_Absyn_decl_stmt(_tmp9D1,r,0U);});}}
# 3324
r=Cyc_Absyn_label_stmt(_tmp56F,r,0U);
# 3328
Cyc_Toc_subst_pattern_vars(*decls,_tmp57E);
Cyc_Toc_exp_to_c(nv,_tmp57E);
r=Cyc_Absyn_ifthenelse_stmt(_tmp57E,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(_tmp570,r,0U);
({struct Cyc_List_List*_tmp9D3=({struct Cyc_List_List*_tmp56D=_region_malloc(rgn,sizeof(*_tmp56D));({struct _tuple40*_tmp9D2=({struct _tuple40*_tmp56C=_region_malloc(rgn,sizeof(*_tmp56C));_tmp56C->f1=nv,_tmp56C->f2=_tmp56F,_tmp56C->f3=_tmp561;_tmp56C;});_tmp56D->hd=_tmp9D2;}),_tmp56D->tl=*bodies;_tmp56D;});*bodies=_tmp9D3;});
return r;};}};}
# 3338
({void*_tmp572=0U;({struct _fat_ptr _tmp9D4=({const char*_tmp573="couldn't find rhs!";_tag_fat(_tmp573,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9D4,_tag_fat(_tmp572,sizeof(void*),0U));});});}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({void*_tmp574=0U;({struct _fat_ptr _tmp9D5=({const char*_tmp575="bad where clause in match compiler";_tag_fat(_tmp575,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9D5,_tag_fat(_tmp574,sizeof(void*),0U));});});}_LL2A:;}}else{_LL28: _LL29:
# 3343
 for(0;_tmp549 != 0;_tmp549=_tmp549->tl){
struct _tuple33 _tmp578=*((struct _tuple33*)_tmp549->hd);struct _tuple33 _tmp579=_tmp578;void*_tmp57D;void*_tmp57C;_LL36: _tmp57D=_tmp579.f1;_tmp57C=_tmp579.f2;_LL37:;{
struct Cyc_Absyn_Exp*_tmp57A=Cyc_Toc_compile_pat_test(_tmp548,_tmp57D);
struct Cyc_Absyn_Stmt*_tmp57B=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp57C,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp57A,_tmp57B,res,0U);};}}_LL25:;}
# 3351
return res;}}}_LL0:;}
# 3361
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3363
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple40*_tmp585=(struct _tuple40*)bodies->hd;struct _tuple40*_tmp586=_tmp585;struct Cyc_Toc_Env**_tmp588;struct Cyc_Absyn_Stmt*_tmp587;_LL1: _tmp588=(struct Cyc_Toc_Env**)& _tmp586->f1;_tmp587=_tmp586->f3;_LL2:;
if(_tmp587 == s)return _tmp588;}
# 3369
return 0;}
# 3373
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3376
void*_tmp589=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3379
struct _tuple30 _tmp58A=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp58B=_tmp58A;struct _tuple1*_tmp59E;struct Cyc_Absyn_Exp*_tmp59D;_LL1: _tmp59E=_tmp58B.f1;_tmp59D=_tmp58B.f2;_LL2:;{
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp58C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp58C;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp58D=Cyc_Toc_share_env(rgn,nv);
# 3385
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3390
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp58D,& mydecls,& mybodies,dopt,lscs,_tmp59E);
# 3398
{struct Cyc_List_List*_tmp58E=lscs;for(0;_tmp58E != 0;_tmp58E=_tmp58E->tl){
struct _tuple36*_tmp58F=(struct _tuple36*)_tmp58E->hd;struct _tuple36*_tmp590=_tmp58F;struct _fat_ptr*_tmp598;struct Cyc_Absyn_Switch_clause*_tmp597;_LL4: _tmp598=_tmp590->f3;_tmp597=_tmp590->f4;_LL5:;{
struct Cyc_Absyn_Stmt*s=_tmp597->body;
# 3402
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3405
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp591=*envp;
# 3408
if(_tmp58E->tl != 0){
struct _tuple36*_tmp592=(struct _tuple36*)((struct Cyc_List_List*)_check_null(_tmp58E->tl))->hd;struct _tuple36*_tmp593=_tmp592;struct _fat_ptr*_tmp596;struct Cyc_Absyn_Switch_clause*_tmp595;_LL7: _tmp596=_tmp593->f3;_tmp595=_tmp593->f4;_LL8:;{
# 3412
struct Cyc_Toc_Env**_tmp594=Cyc_Toc_find_case_env(mybodies,_tmp595->body);
# 3417
if(_tmp594 == 0)
({struct Cyc_Toc_Env*_tmp9D6=Cyc_Toc_last_switchclause_env(rgn,_tmp591,end_l);Cyc_Toc_stmt_to_c(_tmp9D6,s);});else{
# 3421
struct Cyc_List_List*vs=0;
if(_tmp595->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp595->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmp9D7=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmp9D7,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3426
({struct Cyc_Toc_Env*_tmp9D8=Cyc_Toc_non_last_switchclause_env(rgn,_tmp591,end_l,_tmp596,vs);Cyc_Toc_stmt_to_c(_tmp9D8,s);});}};}else{
# 3430
({struct Cyc_Toc_Env*_tmp9D9=Cyc_Toc_last_switchclause_env(rgn,_tmp591,end_l);Cyc_Toc_stmt_to_c(_tmp9D9,s);});}};};}}{
# 3434
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmp9DB=test_tree;Cyc_Absyn_seq_stmt(_tmp9DB,({struct _fat_ptr*_tmp9DA=end_l;Cyc_Absyn_label_stmt(_tmp9DA,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3436
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple38 _tmp599=*((struct _tuple38*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple38 _tmp59A=_tmp599;struct Cyc_Absyn_Vardecl*_tmp59C;_LLA: _tmp59C=_tmp59A.f2;_LLB:;
res=({struct Cyc_Absyn_Decl*_tmp9DC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->tag=0U,_tmp59B->f1=_tmp59C;_tmp59B;}),0U);Cyc_Absyn_decl_stmt(_tmp9DC,res,0U);});}
# 3441
({void*_tmp9E0=({struct _tuple1*_tmp9DF=_tmp59E;void*_tmp9DE=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmp9DD=e;Cyc_Absyn_declare_stmt(_tmp9DF,_tmp9DE,_tmp9DD,res,0U);})->r;whole_s->r=_tmp9E0;});
_npop_handler(0U);return;};}
# 3382
;_pop_region(rgn);};};}
# 3447
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3449
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3454
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3456
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp59F[1U];({struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp59F[0]=_tmp9E1;});({struct Cyc_Absyn_Exp*_tmp9E2=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmp9E2,_tag_fat(_tmp59F,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3459
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmp9E4=({struct Cyc_Absyn_Stmt*_tmp9E3=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmp9E3,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmp9E4;});}
# 3464
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3466
while(1){
void*_tmp5A0=s->r;void*_tmp5A1=_tmp5A0;struct Cyc_Absyn_Stmt*_tmp609;struct Cyc_List_List*_tmp608;void*_tmp607;struct Cyc_Absyn_Decl*_tmp606;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_List_List*_tmp604;struct Cyc_Absyn_Switch_clause**_tmp603;struct Cyc_Absyn_Exp*_tmp602;struct Cyc_List_List*_tmp601;void*_tmp600;struct _fat_ptr*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Stmt*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F4;struct Cyc_Absyn_Stmt*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5EF;switch(*((int*)_tmp5A1)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp5EF=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_LL4:
 Cyc_Toc_exp_to_c(nv,_tmp5EF);return;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1 == 0){_LL5: _LL6:
({int _tmp9E5=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9E5,s);});return;}else{_LL7: _tmp5F0=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_LL8: {
# 3473
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(_tmp5F0))->topt));
Cyc_Toc_exp_to_c(nv,_tmp5F0);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp5A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5A3=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5A2,0U),0U);
Cyc_Toc_do_npop_before(npop,_tmp5A3);
({void*_tmp9E6=(Cyc_Absyn_declare_stmt(_tmp5A2,t,_tmp5F0,_tmp5A3,0U))->r;s->r=_tmp9E6;});}
# 3482
return;};}}case 2U: _LL9: _tmp5F2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp5F1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_LLA:
# 3484
 Cyc_Toc_stmt_to_c(nv,_tmp5F2);
s=_tmp5F1;
continue;case 4U: _LLB: _tmp5F5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp5F4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_tmp5F3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f3;_LLC:
# 3488
 Cyc_Toc_exp_to_c(nv,_tmp5F5);
Cyc_Toc_stmt_to_c(nv,_tmp5F4);
s=_tmp5F3;
continue;case 9U: _LLD: _tmp5F9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp5F8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2).f1;_tmp5F7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f3).f1;_tmp5F6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f4;_LLE:
# 3493
 Cyc_Toc_exp_to_c(nv,_tmp5F9);
Cyc_Toc_exp_to_c(nv,_tmp5F8);
_tmp5FB=_tmp5F6;_tmp5FA=_tmp5F7;goto _LL10;case 14U: _LLF: _tmp5FB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp5FA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2).f1;_LL10:
 _tmp5FD=_tmp5FA;_tmp5FC=_tmp5FB;goto _LL12;case 5U: _LL11: _tmp5FD=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1).f1;_tmp5FC=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_LL12:
# 3498
 Cyc_Toc_exp_to_c(nv,_tmp5FD);{
struct _RegionHandle _tmp5A4=_new_region("temp");struct _RegionHandle*temp=& _tmp5A4;_push_region(temp);({struct Cyc_Toc_Env*_tmp9E7=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmp9E7,_tmp5FC);});
_npop_handler(0U);return;
# 3499
;_pop_region(temp);};case 6U: _LL13: _LL14: {
# 3502
struct Cyc_Toc_Env*_tmp5A5=nv;struct _fat_ptr**_tmp5A6;_LL24: _tmp5A6=_tmp5A5->break_lab;_LL25:;
if(_tmp5A6 != 0)
({void*_tmp9E8=Cyc_Toc_goto_stmt_r(*_tmp5A6);s->r=_tmp9E8;});
# 3506
({int _tmp9E9=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9E9,s);});
return;}case 7U: _LL15: _LL16: {
# 3509
struct Cyc_Toc_Env*_tmp5A7=nv;struct _fat_ptr**_tmp5A8;_LL27: _tmp5A8=_tmp5A7->continue_lab;_LL28:;
if(_tmp5A8 != 0)
({void*_tmp9EA=Cyc_Toc_goto_stmt_r(*_tmp5A8);s->r=_tmp9EA;});
goto _LL18;}case 8U: _LL17: _LL18:
# 3514
({int _tmp9EB=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9EB,s);});
return;case 13U: _LL19: _tmp5FF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp5FE=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_LL1A:
 s=_tmp5FE;continue;case 10U: _LL1B: _tmp602=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp601=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_tmp600=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f3;_LL1C:
# 3518
 Cyc_Toc_xlate_switch(nv,s,_tmp602,_tmp601,_tmp600);
return;case 11U: _LL1D: _tmp604=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp603=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_LL1E: {
# 3521
struct Cyc_Toc_Env*_tmp5A9=nv;struct Cyc_Toc_FallthruInfo*_tmp5B3;_LL2A: _tmp5B3=_tmp5A9->fallthru_info;_LL2B:;
if(_tmp5B3 == 0)
({void*_tmp5AA=0U;({struct _fat_ptr _tmp9EC=({const char*_tmp5AB="fallthru in unexpected place";_tag_fat(_tmp5AB,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmp9EC,_tag_fat(_tmp5AA,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp5AC=*_tmp5B3;struct Cyc_Toc_FallthruInfo _tmp5AD=_tmp5AC;struct _fat_ptr*_tmp5B2;struct Cyc_List_List*_tmp5B1;_LL2D: _tmp5B2=_tmp5AD.label;_tmp5B1=_tmp5AD.binders;_LL2E:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5B2,0U);
# 3527
({int _tmp9ED=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmp9ED,s2);});{
struct Cyc_List_List*_tmp5AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5B1);
struct Cyc_List_List*_tmp5AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp604);
for(0;_tmp5AE != 0;(_tmp5AE=_tmp5AE->tl,_tmp5AF=_tmp5AF->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5AF))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmp9EF=({struct Cyc_Absyn_Exp*_tmp9EE=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->tag=5U,_tmp5B0->f1=(struct Cyc_Absyn_Vardecl*)_tmp5AE->hd;_tmp5B0;}),0U);Cyc_Absyn_assign_stmt(_tmp9EE,(struct Cyc_Absyn_Exp*)_tmp5AF->hd,0U);});Cyc_Absyn_seq_stmt(_tmp9EF,s2,0U);});}
# 3535
s->r=s2->r;
return;};};};}case 12U: _LL1F: _tmp606=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp605=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_LL20:
# 3541
{void*_tmp5B4=_tmp606->r;void*_tmp5B5=_tmp5B4;struct Cyc_Absyn_Tvar*_tmp5D7;struct Cyc_Absyn_Vardecl*_tmp5D6;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Fndecl*_tmp5D4;struct Cyc_List_List*_tmp5D3;struct Cyc_Absyn_Pat*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D1;void*_tmp5D0;struct Cyc_Absyn_Vardecl*_tmp5CF;switch(*((int*)_tmp5B5)){case 0U: _LL30: _tmp5CF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5B5)->f1;_LL31:
 Cyc_Toc_local_decl_to_c(nv,_tmp5CF,_tmp605);goto _LL2F;case 2U: _LL32: _tmp5D2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5B5)->f1;_tmp5D1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5B5)->f3;_tmp5D0=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5B5)->f4;_LL33:
# 3549
{void*_tmp5B6=_tmp5D2->r;void*_tmp5B7=_tmp5B6;struct Cyc_Absyn_Vardecl*_tmp5BC;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5B7)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5B7)->f2)->r)->tag == 0U){_LL3D: _tmp5BC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5B7)->f1;_LL3E: {
# 3551
struct Cyc_Absyn_Vardecl*_tmp5B8=({struct _tuple1*_tmp9F1=Cyc_Toc_temp_var();void*_tmp9F0=_tmp5BC->type;Cyc_Absyn_new_vardecl(0U,_tmp9F1,_tmp9F0,_tmp5D1);});
({struct Cyc_Absyn_Exp*_tmp9F2=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->tag=4U,_tmp5B9->f1=_tmp5B8;_tmp5B9;}),0U);_tmp5BC->initializer=_tmp9F2;});
((struct Cyc_Absyn_Exp*)_check_null(_tmp5BC->initializer))->topt=_tmp5B8->type;
({void*_tmp9F9=({struct Cyc_Absyn_Decl*_tmp9F8=({void*_tmp9F3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->tag=0U,_tmp5BA->f1=_tmp5B8;_tmp5BA;});Cyc_Absyn_new_decl(_tmp9F3,s->loc);});struct Cyc_Absyn_Stmt*_tmp9F7=({
struct Cyc_Absyn_Decl*_tmp9F6=({void*_tmp9F4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=0U,_tmp5BB->f1=_tmp5BC;_tmp5BB;});Cyc_Absyn_new_decl(_tmp9F4,s->loc);});struct Cyc_Absyn_Stmt*_tmp9F5=_tmp605;Cyc_Absyn_decl_stmt(_tmp9F6,_tmp9F5,s->loc);});
# 3554
Cyc_Absyn_decl_stmt(_tmp9F8,_tmp9F7,s->loc);})->r;s->r=_tmp9F9;});
# 3557
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3563
({void*_tmp9FA=(Cyc_Toc_letdecl_to_c(nv,_tmp5D2,_tmp5D0,(void*)_check_null(_tmp5D1->topt),_tmp5D1,_tmp605))->r;s->r=_tmp9FA;});
goto _LL3C;}_LL3C:;}
# 3566
goto _LL2F;case 3U: _LL34: _tmp5D3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5B5)->f1;_LL35: {
# 3570
struct Cyc_List_List*_tmp5BD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5D3);
if(_tmp5BD == 0)
({void*_tmp5BE=0U;({struct _fat_ptr _tmp9FB=({const char*_tmp5BF="empty Letv_d";_tag_fat(_tmp5BF,sizeof(char),13U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp9FB,_tag_fat(_tmp5BE,sizeof(void*),0U));});});
({void*_tmp9FC=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->tag=0U,_tmp5C0->f1=(struct Cyc_Absyn_Vardecl*)_tmp5BD->hd;_tmp5C0;});_tmp606->r=_tmp9FC;});
_tmp5BD=_tmp5BD->tl;
for(0;_tmp5BD != 0;_tmp5BD=_tmp5BD->tl){
struct Cyc_Absyn_Decl*_tmp5C1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->tag=0U,_tmp5C2->f1=(struct Cyc_Absyn_Vardecl*)_tmp5BD->hd;_tmp5C2;}),0U);
({void*_tmp9FE=({struct Cyc_Absyn_Decl*_tmp9FD=_tmp5C1;Cyc_Absyn_decl_stmt(_tmp9FD,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmp9FE;});}
# 3579
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp5D4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5B5)->f1;_LL37:
# 3582
 Cyc_Toc_fndecl_to_c(nv,_tmp5D4,0);
Cyc_Toc_stmt_to_c(nv,_tmp605);
goto _LL2F;case 4U: _LL38: _tmp5D7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5B5)->f1;_tmp5D6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5B5)->f2;_tmp5D5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5B5)->f3;_LL39: {
# 3586
struct Cyc_Absyn_Stmt*_tmp5C3=_tmp605;
# 3588
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp5C4=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp5C5=_tmp5C4;struct _tuple1*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CB;_LL42: _tmp5CC=_tmp5C5.f1;_tmp5CB=_tmp5C5.f2;_LL43:;{
struct _tuple1*x_var=_tmp5D6->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3594
Cyc_Toc_stmt_to_c(nv,_tmp5C3);
if(Cyc_Absyn_no_regions)
({void*_tmpA02=({struct _tuple1*_tmpA01=x_var;void*_tmpA00=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmp9FF=
Cyc_Absyn_uint_exp(0U,0U);
# 3596
Cyc_Absyn_declare_stmt(_tmpA01,_tmpA00,_tmp9FF,_tmp5C3,0U);})->r;s->r=_tmpA02;});else{
# 3598
if((unsigned)_tmp5D5){
# 3600
Cyc_Toc_exp_to_c(nv,_tmp5D5);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA04=_tmp5D5;Cyc_Absyn_aggrarrow_exp(_tmpA04,({struct _fat_ptr*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));({struct _fat_ptr _tmpA03=({const char*_tmp5C6="h";_tag_fat(_tmp5C6,sizeof(char),2U);});*_tmp5C7=_tmpA03;});_tmp5C7;}),0U);}),0U);
({void*_tmpA05=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp5C3,0U))->r;s->r=_tmpA05;});};}else{
# 3609
({void*_tmpA12=({struct _tuple1*_tmpA11=_tmp5CC;void*_tmpA10=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA0F=({struct Cyc_Absyn_Exp*_tmp5C8[1U];({struct Cyc_Absyn_Exp*_tmpA06=
# 3611
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp5C8[0]=_tmpA06;});({struct Cyc_Absyn_Exp*_tmpA07=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA07,_tag_fat(_tmp5C8,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3609
Cyc_Absyn_declare_stmt(_tmpA11,_tmpA10,_tmpA0F,({
# 3612
struct _tuple1*_tmpA0E=x_var;void*_tmpA0D=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA0C=Cyc_Absyn_address_exp(_tmp5CB,0U);Cyc_Absyn_declare_stmt(_tmpA0E,_tmpA0D,_tmpA0C,({
struct Cyc_Absyn_Stmt*_tmpA0B=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5C9[1U];_tmp5C9[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA08=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA08,_tag_fat(_tmp5C9,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA0B,({
struct Cyc_Absyn_Stmt*_tmpA0A=_tmp5C3;Cyc_Absyn_seq_stmt(_tmpA0A,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5CA[1U];_tmp5CA[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA09=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA09,_tag_fat(_tmp5CA,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3609
s->r=_tmpA12;});}}
# 3618
return;};}default: _LL3A: _LL3B:
({void*_tmp5CD=0U;({struct _fat_ptr _tmpA13=({const char*_tmp5CE="bad nested declaration within function";_tag_fat(_tmp5CE,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpA13,_tag_fat(_tmp5CD,sizeof(void*),0U));});});}_LL2F:;}
# 3621
return;default: _LL21: _tmp609=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f1;_tmp608=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f2;_tmp607=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A1)->f3;_LL22: {
# 3635 "toc.cyc"
struct _tuple30 _tmp5D8=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp5D9=_tmp5D8;struct _tuple1*_tmp5EE;struct Cyc_Absyn_Exp*_tmp5ED;_LL45: _tmp5EE=_tmp5D9.f1;_tmp5ED=_tmp5D9.f2;_LL46:;{
struct _tuple30 _tmp5DA=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp5DB=_tmp5DA;struct _tuple1*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5EB;_LL48: _tmp5EC=_tmp5DB.f1;_tmp5EB=_tmp5DB.f2;_LL49:;{
struct _tuple30 _tmp5DC=Cyc_Toc_temp_var_and_exp();struct _tuple30 _tmp5DD=_tmp5DC;struct _tuple1*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5E9;_LL4B: _tmp5EA=_tmp5DD.f1;_tmp5E9=_tmp5DD.f2;_LL4C:;{
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3642
_tmp5EB->topt=e_typ;{
struct _RegionHandle _tmp5DE=_new_region("temp");struct _RegionHandle*temp=& _tmp5DE;_push_region(temp);
# 3645
Cyc_Toc_stmt_to_c(nv,_tmp609);{
struct Cyc_Absyn_Stmt*_tmp5DF=({struct Cyc_Absyn_Stmt*_tmpA15=_tmp609;Cyc_Absyn_seq_stmt(_tmpA15,
Cyc_Absyn_exp_stmt(({void*_tmp5E8=0U;({struct Cyc_Absyn_Exp*_tmpA14=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA14,_tag_fat(_tmp5E8,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3649
struct Cyc_Absyn_Stmt*_tmp5E0=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->tag=10U,_tmp5E7->f1=_tmp5EB,_tmp5E7->f2=_tmp608,_tmp5E7->f3=_tmp607;_tmp5E7;}),0U);
# 3651
Cyc_Toc_stmt_to_c(nv,_tmp5E0);{
# 3654
struct Cyc_Absyn_Exp*_tmp5E1=({struct Cyc_Absyn_Exp*_tmp5E6[1U];({struct Cyc_Absyn_Exp*_tmpA16=
Cyc_Toc_member_exp(_tmp5ED,Cyc_Toc_handler_sp,0U);_tmp5E6[0]=_tmpA16;});({struct Cyc_Absyn_Exp*_tmpA17=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpA17,_tag_fat(_tmp5E6,sizeof(struct Cyc_Absyn_Exp*),1U));});});
struct Cyc_Absyn_Stmt*_tmp5E2=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5E5[1U];({struct Cyc_Absyn_Exp*_tmpA18=Cyc_Absyn_address_exp(_tmp5ED,0U);_tmp5E5[0]=_tmpA18;});({struct Cyc_Absyn_Exp*_tmpA19=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA19,_tag_fat(_tmp5E5,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
struct Cyc_Absyn_Exp*_tmp5E3=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp5E4=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpA29=({
struct _tuple1*_tmpA28=_tmp5EE;void*_tmpA27=h_typ;Cyc_Absyn_declare_stmt(_tmpA28,_tmpA27,0,({
struct Cyc_Absyn_Stmt*_tmpA26=_tmp5E2;Cyc_Absyn_seq_stmt(_tmpA26,({
struct _tuple1*_tmpA25=_tmp5EA;void*_tmpA24=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpA23=_tmp5E3;Cyc_Absyn_declare_stmt(_tmpA25,_tmpA24,_tmpA23,({
struct Cyc_Absyn_Stmt*_tmpA22=({struct Cyc_Absyn_Exp*_tmpA1B=_tmp5E1;struct Cyc_Absyn_Stmt*_tmpA1A=
Cyc_Absyn_assign_stmt(_tmp5E9,_tmp5E4,0U);
# 3664
Cyc_Absyn_ifthenelse_stmt(_tmpA1B,_tmpA1A,
# 3666
Cyc_Toc_skip_stmt_dl(),0U);});
# 3664
Cyc_Absyn_seq_stmt(_tmpA22,({
# 3667
struct Cyc_Absyn_Exp*_tmpA21=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp5E9,0U);struct Cyc_Absyn_Stmt*_tmpA20=_tmp5DF;Cyc_Absyn_ifthenelse_stmt(_tmpA21,_tmpA20,({
# 3669
struct _tuple1*_tmpA1F=_tmp5EC;void*_tmpA1E=e_typ;struct Cyc_Absyn_Exp*_tmpA1D=({
void*_tmpA1C=e_typ;Cyc_Toc_cast_it(_tmpA1C,Cyc_Toc_get_exn_thrown_expression());});
# 3669
Cyc_Absyn_declare_stmt(_tmpA1F,_tmpA1E,_tmpA1D,_tmp5E0,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3660
s->r=_tmpA29;});};};
# 3673
_npop_handler(0U);return;
# 3643
;_pop_region(temp);};};};};}}_LL0:;}}
# 3682
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpA2A=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpA2A;});
({void*_tmpA2B=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpA2B;});{
struct _RegionHandle _tmp60A=_new_region("frgn");struct _RegionHandle*frgn=& _tmp60A;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp60B=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp60C=(f->i).args;for(0;_tmp60C != 0;_tmp60C=_tmp60C->tl){
struct _tuple1*_tmp60D=({struct _tuple1*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=Cyc_Absyn_Loc_n,_tmp60E->f2=(*((struct _tuple9*)_tmp60C->hd)).f1;_tmp60E;});
({void*_tmpA2C=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp60C->hd)).f3);(*((struct _tuple9*)_tmp60C->hd)).f3=_tmpA2C;});}}
# 3699
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3703
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp60F=_cycalloc(sizeof(*_tmp60F));({struct Cyc_Hashtable_Table*_tmpA2D=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp60F=_tmpA2D;});_tmp60F;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp610=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp611=_tmp610;struct _fat_ptr*_tmp61A;struct Cyc_Absyn_Tqual _tmp619;void*_tmp618;int _tmp617;_LL1: _tmp61A=_tmp611.name;_tmp619=_tmp611.tq;_tmp618=_tmp611.type;_tmp617=_tmp611.inject;_LL2:;{
void*_tmp612=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp618,Cyc_Absyn_heap_rgn_type,_tmp619,Cyc_Absyn_false_type));
struct _tuple1*_tmp613=({struct _tuple1*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->f1=Cyc_Absyn_Loc_n,_tmp616->f2=(struct _fat_ptr*)_check_null(_tmp61A);_tmp616;});
({struct Cyc_List_List*_tmpA30=({struct Cyc_List_List*_tmpA2F=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA2F,({struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));({struct _tuple9*_tmpA2E=({struct _tuple9*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->f1=_tmp61A,_tmp614->f2=_tmp619,_tmp614->f3=_tmp612;_tmp614;});_tmp615->hd=_tmpA2E;}),_tmp615->tl=0;_tmp615;}));});(f->i).args=_tmpA30;});
(f->i).cyc_varargs=0;};}
# 3712
{struct Cyc_List_List*_tmp61B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp61B != 0;_tmp61B=_tmp61B->tl){
({void*_tmpA31=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp61B->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp61B->hd)->type=_tmpA31;});}}
({struct Cyc_Toc_Env*_tmpA32=Cyc_Toc_clear_toplevel(frgn,_tmp60B);Cyc_Toc_stmt_to_c(_tmpA32,f->body);});}
# 3692
;_pop_region(frgn);};}
# 3717
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp61C=s;switch(_tmp61C){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3733 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 3735
struct _tuple41 _tmp61D=*env;struct _tuple41 _tmp61E=_tmp61D;struct Cyc_Toc_TocState*_tmp651;struct Cyc_Absyn_Aggrdecl*_tmp650;_LL1: _tmp651=_tmp61E.f1;_tmp650=*_tmp61E.f2;_LL2:;{
struct _tuple1*_tmp61F=_tmp650->name;
struct Cyc_Toc_TocState _tmp620=*_tmp651;struct Cyc_Toc_TocState _tmp621=_tmp620;struct Cyc_Dict_Dict*_tmp64F;_LL4: _tmp64F=_tmp621.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp622=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp64F,_tmp61F);
if(_tmp622 == 0){
seen_defn_before=0;{
struct _tuple19*v;
if((int)_tmp650->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple19*_tmp623=_region_malloc(d,sizeof(*_tmp623));_tmp623->f1=_tmp650,({void*_tmpA33=Cyc_Absyn_strctq(_tmp61F);_tmp623->f2=_tmpA33;});_tmp623;});else{
# 3746
v=({struct _tuple19*_tmp624=_region_malloc(d,sizeof(*_tmp624));_tmp624->f1=_tmp650,({void*_tmpA34=Cyc_Absyn_unionq_type(_tmp61F);_tmp624->f2=_tmpA34;});_tmp624;});}
({struct Cyc_Dict_Dict _tmpA35=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp64F,_tmp61F,v);*_tmp64F=_tmpA35;});};}else{
# 3749
struct _tuple19*_tmp625=*_tmp622;struct _tuple19*_tmp626=_tmp625;struct Cyc_Absyn_Aggrdecl*_tmp629;void*_tmp628;_LL7: _tmp629=_tmp626->f1;_tmp628=_tmp626->f2;_LL8:;
if(_tmp629->impl == 0){
({struct Cyc_Dict_Dict _tmpA38=({struct Cyc_Dict_Dict _tmpA37=*_tmp64F;struct _tuple1*_tmpA36=_tmp61F;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpA37,_tmpA36,({struct _tuple19*_tmp627=_region_malloc(d,sizeof(*_tmp627));_tmp627->f1=_tmp650,_tmp627->f2=_tmp628;_tmp627;}));});*_tmp64F=_tmpA38;});
seen_defn_before=0;}else{
# 3754
seen_defn_before=1;}}{
# 3756
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->kind=_tmp650->kind,_tmp64E->sc=Cyc_Absyn_Public,_tmp64E->name=_tmp650->name,_tmp64E->tvs=0,_tmp64E->impl=0,_tmp64E->expected_mem_kind=0,_tmp64E->attributes=_tmp650->attributes;_tmp64E;});
# 3763
if(_tmp650->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpA39=({struct Cyc_Absyn_AggrdeclImpl*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->exist_vars=0,_tmp62A->rgn_po=0,_tmp62A->fields=0,_tmp62A->tagged=0;_tmp62A;});new_ad->impl=_tmpA39;});{
# 3768
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp62B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp650->impl))->fields;for(0;_tmp62B != 0;_tmp62B=_tmp62B->tl){
# 3772
struct Cyc_Absyn_Aggrfield*_tmp62C=(struct Cyc_Absyn_Aggrfield*)_tmp62B->hd;
void*_tmp62D=_tmp62C->type;
struct Cyc_List_List*_tmp62E=_tmp62C->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp62D))&&(
(int)_tmp650->kind == (int)Cyc_Absyn_StructA && _tmp62B->tl == 0 ||(int)_tmp650->kind == (int)Cyc_Absyn_UnionA)){
# 3786 "toc.cyc"
void*_tmp62F=Cyc_Tcutil_compress(_tmp62D);void*_tmp630=_tmp62F;void*_tmp638;struct Cyc_Absyn_Tqual _tmp637;void*_tmp636;unsigned _tmp635;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630)->tag == 4U){_LLA: _tmp638=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630)->f1).elt_type;_tmp637=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630)->f1).tq;_tmp636=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630)->f1).zero_term;_tmp635=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630)->f1).zt_loc;_LLB:
# 3789
 _tmp62D=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=4U,(_tmp631->f1).elt_type=_tmp638,(_tmp631->f1).tq=_tmp637,({struct Cyc_Absyn_Exp*_tmpA3A=Cyc_Absyn_uint_exp(0U,0U);(_tmp631->f1).num_elts=_tmpA3A;}),(_tmp631->f1).zero_term=_tmp636,(_tmp631->f1).zt_loc=_tmp635;_tmp631;});
goto _LL9;}else{_LLC: _LLD:
# 3792
 _tmp62E=({struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));({void*_tmpA3B=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=6U,_tmp632->f1=0;_tmp632;});_tmp633->hd=_tmpA3B;}),_tmp633->tl=_tmp62E;_tmp633;});
_tmp62D=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=4U,({void*_tmpA3E=Cyc_Toc_void_star_type();(_tmp634->f1).elt_type=_tmpA3E;}),({
struct Cyc_Absyn_Tqual _tmpA3D=Cyc_Absyn_empty_tqual(0U);(_tmp634->f1).tq=_tmpA3D;}),({
struct Cyc_Absyn_Exp*_tmpA3C=Cyc_Absyn_uint_exp(0U,0U);(_tmp634->f1).num_elts=_tmpA3C;}),(_tmp634->f1).zero_term=Cyc_Absyn_false_type,(_tmp634->f1).zt_loc=0U;_tmp634;});}_LL9:;}{
# 3799
struct Cyc_Absyn_Aggrfield*_tmp639=({struct Cyc_Absyn_Aggrfield*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->name=_tmp62C->name,_tmp64A->tq=Cyc_Toc_mt_tq,({
# 3801
void*_tmpA3F=Cyc_Toc_typ_to_c(_tmp62D);_tmp64A->type=_tmpA3F;}),_tmp64A->width=_tmp62C->width,_tmp64A->attributes=_tmp62E,_tmp64A->requires_clause=0;_tmp64A;});
# 3809
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp650->impl))->tagged){
void*_tmp63A=_tmp639->type;
struct _fat_ptr*_tmp63B=_tmp639->name;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp647=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,_tmp76B.f1=(struct _fat_ptr)((struct _fat_ptr)*(*_tmp650->name).f2);_tmp76B;});struct Cyc_String_pa_PrintArg_struct _tmp648=({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0U,_tmp76A.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp63B);_tmp76A;});void*_tmp645[2U];_tmp645[0]=& _tmp647,_tmp645[1]=& _tmp648;({struct _fat_ptr _tmpA40=({const char*_tmp646="_union_%s_%s";_tag_fat(_tmp646,sizeof(char),13U);});Cyc_aprintf(_tmpA40,_tag_fat(_tmp645,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp644=_cycalloc(sizeof(*_tmp644));*_tmp644=s;_tmp644;});
struct Cyc_Absyn_Aggrfield*_tmp63C=({struct Cyc_Absyn_Aggrfield*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->name=Cyc_Toc_val_sp,_tmp643->tq=Cyc_Toc_mt_tq,_tmp643->type=_tmp63A,_tmp643->width=0,_tmp643->attributes=0,_tmp643->requires_clause=0;_tmp643;});
struct Cyc_Absyn_Aggrfield*_tmp63D=({struct Cyc_Absyn_Aggrfield*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->name=Cyc_Toc_tag_sp,_tmp642->tq=Cyc_Toc_mt_tq,_tmp642->type=Cyc_Absyn_sint_type,_tmp642->width=0,_tmp642->attributes=0,_tmp642->requires_clause=0;_tmp642;});
struct Cyc_Absyn_Aggrdecl*_tmp63E=({struct _fat_ptr*_tmpA41=str;Cyc_Toc_make_c_struct_defn(_tmpA41,({struct Cyc_Absyn_Aggrfield*_tmp641[2U];_tmp641[0]=_tmp63D,_tmp641[1]=_tmp63C;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp641,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));({struct Cyc_Absyn_Decl*_tmpA42=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->tag=5U,_tmp63F->f1=_tmp63E;_tmp63F;}),0U);_tmp640->hd=_tmpA42;}),_tmp640->tl=Cyc_Toc_result_decls;_tmp640;});
({void*_tmpA43=Cyc_Absyn_strct(str);_tmp639->type=_tmpA43;});}
# 3820
new_fields=({struct Cyc_List_List*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->hd=_tmp639,_tmp649->tl=new_fields;_tmp649;});};}}
# 3822
({struct Cyc_List_List*_tmpA44=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpA44;});};}
# 3825
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct Cyc_Absyn_Decl*_tmpA46=({struct Cyc_Absyn_Decl*_tmp64C=_cycalloc(sizeof(*_tmp64C));({void*_tmpA45=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->tag=5U,_tmp64B->f1=new_ad;_tmp64B;});_tmp64C->r=_tmpA45;}),_tmp64C->loc=0U;_tmp64C;});_tmp64D->hd=_tmpA46;}),_tmp64D->tl=Cyc_Toc_result_decls;_tmp64D;});
return 0;};};};}
# 3829
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3857 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3860
struct _tuple42 _tmp652=*env;struct _tuple42 _tmp653=_tmp652;struct Cyc_Set_Set**_tmp664;struct Cyc_Absyn_Datatypedecl*_tmp663;_LL1: _tmp664=(_tmp653.f1)->datatypes_so_far;_tmp663=_tmp653.f2;_LL2:;{
struct _tuple1*_tmp654=_tmp663->name;
if(_tmp663->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp664,_tmp654))
return 0;
({struct Cyc_Set_Set*_tmpA47=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp664,_tmp654);*_tmp664=_tmpA47;});
{struct Cyc_List_List*_tmp655=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp663->fields))->v;for(0;_tmp655 != 0;_tmp655=_tmp655->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp655->hd;
# 3868
struct Cyc_List_List*_tmp656=0;
int i=1;
{struct Cyc_List_List*_tmp657=f->typs;for(0;_tmp657 != 0;(_tmp657=_tmp657->tl,i ++)){
struct _fat_ptr*_tmp658=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp659=({struct Cyc_Absyn_Aggrfield*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->name=_tmp658,_tmp65B->tq=(*((struct _tuple13*)_tmp657->hd)).f1,({
void*_tmpA48=Cyc_Toc_typ_to_c((*((struct _tuple13*)_tmp657->hd)).f2);_tmp65B->type=_tmpA48;}),_tmp65B->width=0,_tmp65B->attributes=0,_tmp65B->requires_clause=0;_tmp65B;});
_tmp656=({struct Cyc_List_List*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->hd=_tmp659,_tmp65A->tl=_tmp656;_tmp65A;});}}
# 3876
_tmp656=({struct Cyc_List_List*_tmp65D=_cycalloc(sizeof(*_tmp65D));({struct Cyc_Absyn_Aggrfield*_tmpA4A=({struct Cyc_Absyn_Aggrfield*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->name=Cyc_Toc_tag_sp,_tmp65C->tq=Cyc_Toc_mt_tq,_tmp65C->type=Cyc_Absyn_sint_type,_tmp65C->width=0,_tmp65C->attributes=0,_tmp65C->requires_clause=0;_tmp65C;});_tmp65D->hd=_tmpA4A;}),({
struct Cyc_List_List*_tmpA49=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp656);_tmp65D->tl=_tmpA49;});_tmp65D;});{
struct Cyc_Absyn_Aggrdecl*_tmp65E=({struct _fat_ptr*_tmpA4C=({struct _fat_ptr*_tmp662=_cycalloc(sizeof(*_tmp662));({struct _fat_ptr _tmpA4B=({const char*_tmp661="";_tag_fat(_tmp661,sizeof(char),1U);});*_tmp662=_tmpA4B;});_tmp662;});Cyc_Toc_make_c_struct_defn(_tmpA4C,_tmp656);});
({struct _tuple1*_tmpA4D=Cyc_Toc_collapse_qvars(f->name,_tmp663->name);_tmp65E->name=_tmpA4D;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp660=_cycalloc(sizeof(*_tmp660));({struct Cyc_Absyn_Decl*_tmpA4E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->tag=5U,_tmp65F->f1=_tmp65E;_tmp65F;}),0U);_tmp660->hd=_tmpA4E;}),_tmp660->tl=Cyc_Toc_result_decls;_tmp660;});};}}
# 3882
return 0;};}
# 3885
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3904 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3907
struct _tuple42 _tmp665=*env;struct _tuple42 _tmp666=_tmp665;struct Cyc_Toc_TocState*_tmp688;struct Cyc_Absyn_Datatypedecl*_tmp687;_LL1: _tmp688=_tmp666.f1;_tmp687=_tmp666.f2;_LL2:;
if(_tmp687->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp667=*_tmp688;struct Cyc_Toc_TocState _tmp668=_tmp667;struct Cyc_Dict_Dict*_tmp686;_LL4: _tmp686=_tmp668.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp669=_tmp687->name;
{struct Cyc_List_List*_tmp66A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp687->fields))->v;for(0;_tmp66A != 0;_tmp66A=_tmp66A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp66A->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp66B=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);
void*_tmp66C=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp66B,Cyc_Absyn_false_type,0U);
# 3918
int*_tmp66D=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp686,f->name);int*_tmp66E=_tmp66D;if(_tmp66E == 0){_LL7: _LL8: {
# 3920
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3924
struct Cyc_Absyn_Vardecl*_tmp66F=Cyc_Absyn_new_vardecl(0U,f->name,_tmp66C,initopt);
_tmp66F->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp671=_cycalloc(sizeof(*_tmp671));({struct Cyc_Absyn_Decl*_tmpA4F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->tag=0U,_tmp670->f1=_tmp66F;_tmp670;}),0U);_tmp671->hd=_tmpA4F;}),_tmp671->tl=Cyc_Toc_result_decls;_tmp671;});
({struct Cyc_Dict_Dict _tmpA50=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp686,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3927
*_tmp686=_tmpA50;});{
# 3929
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp672=f->typs;for(0;_tmp672 != 0;(_tmp672=_tmp672->tl,i ++)){
struct _fat_ptr*_tmp673=({struct _fat_ptr*_tmp67A=_cycalloc(sizeof(*_tmp67A));({struct _fat_ptr _tmpA52=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp679=({struct Cyc_Int_pa_PrintArg_struct _tmp76C;_tmp76C.tag=1U,_tmp76C.f1=(unsigned long)i;_tmp76C;});void*_tmp677[1U];_tmp677[0]=& _tmp679;({struct _fat_ptr _tmpA51=({const char*_tmp678="f%d";_tag_fat(_tmp678,sizeof(char),4U);});Cyc_aprintf(_tmpA51,_tag_fat(_tmp677,sizeof(void*),1U));});});*_tmp67A=_tmpA52;});_tmp67A;});
struct Cyc_Absyn_Aggrfield*_tmp674=({struct Cyc_Absyn_Aggrfield*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->name=_tmp673,_tmp676->tq=(*((struct _tuple13*)_tmp672->hd)).f1,({
void*_tmpA53=Cyc_Toc_typ_to_c((*((struct _tuple13*)_tmp672->hd)).f2);_tmp676->type=_tmpA53;}),_tmp676->width=0,_tmp676->attributes=0,_tmp676->requires_clause=0;_tmp676;});
fields=({struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->hd=_tmp674,_tmp675->tl=fields;_tmp675;});}}
# 3937
fields=({struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*_tmp67C));({struct Cyc_Absyn_Aggrfield*_tmpA56=({struct Cyc_Absyn_Aggrfield*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->name=Cyc_Toc_tag_sp,_tmp67B->tq=Cyc_Toc_mt_tq,({
void*_tmpA55=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp67B->type=_tmpA55;}),_tmp67B->width=0,_tmp67B->attributes=0,_tmp67B->requires_clause=0;_tmp67B;});
# 3937
_tmp67C->hd=_tmpA56;}),({
# 3939
struct Cyc_List_List*_tmpA54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp67C->tl=_tmpA54;});_tmp67C;});{
struct Cyc_Absyn_Aggrdecl*_tmp67D=({struct _fat_ptr*_tmpA58=({struct _fat_ptr*_tmp681=_cycalloc(sizeof(*_tmp681));({struct _fat_ptr _tmpA57=({const char*_tmp680="";_tag_fat(_tmp680,sizeof(char),1U);});*_tmp681=_tmpA57;});_tmp681;});Cyc_Toc_make_c_struct_defn(_tmpA58,fields);});
({struct _tuple1*_tmpA59=Cyc_Toc_collapse_qvars(f->name,_tmp687->name);_tmp67D->name=_tmpA59;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct Cyc_Absyn_Decl*_tmpA5A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->tag=5U,_tmp67E->f1=_tmp67D;_tmp67E;}),0U);_tmp67F->hd=_tmpA5A;}),_tmp67F->tl=Cyc_Toc_result_decls;_tmp67F;});
goto _LL6;};};};}}else{if(*((int*)_tmp66E)== 0){_LL9: _LLA:
# 3945
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp682=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp683=Cyc_Absyn_new_vardecl(0U,f->name,_tmp66C,_tmp682);
_tmp683->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct Cyc_Absyn_Decl*_tmpA5B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->tag=0U,_tmp684->f1=_tmp683;_tmp684;}),0U);_tmp685->hd=_tmpA5B;}),_tmp685->tl=Cyc_Toc_result_decls;_tmp685;});
({struct Cyc_Dict_Dict _tmpA5C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp686,f->name,1);*_tmp686=_tmpA5C;});}
# 3952
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3956
return 0;};};}
# 3959
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3963
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3969
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpA5D=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpA5D;});
# 3973
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3982
struct _RegionHandle _tmp689=_new_region("temp");struct _RegionHandle*temp=& _tmp689;_push_region(temp);
{struct Cyc_Toc_Env*_tmp68A=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp68A,init);}
# 3983
;_pop_region(temp);}else{
# 3987
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3990
void*_tmp68B=Cyc_Tcutil_compress(old_typ);void*_tmp68C=_tmp68B;void*_tmp693;struct Cyc_Absyn_Exp*_tmp692;void*_tmp691;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68C)->tag == 4U){_LL1: _tmp693=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68C)->f1).elt_type;_tmp692=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68C)->f1).num_elts;_tmp691=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68C)->f1).zero_term;_LL2:
# 3992
 if(Cyc_Tcutil_force_type2bool(0,_tmp691)){
if(_tmp692 == 0)
({void*_tmp68D=0U;({struct _fat_ptr _tmpA5E=({const char*_tmp68E="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp68E,sizeof(char),55U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA5E,_tag_fat(_tmp68D,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp692;
struct Cyc_Absyn_Exp*_tmp68F=({struct Cyc_Absyn_Exp*_tmpA60=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpA60,({
struct Cyc_Absyn_Exp*_tmpA5F=num_elts;Cyc_Absyn_add_exp(_tmpA5F,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3999
struct Cyc_Absyn_Exp*_tmp690=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpA62=({struct Cyc_Absyn_Stmt*_tmpA61=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp68F,_tmp690,0U),0U);Cyc_Toc_seq_stmt_r(_tmpA61,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4000
s->r=_tmpA62;});};}
# 4003
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4011
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp694=d;struct Cyc_List_List*_tmp69B;struct Cyc_List_List*_tmp69A;void**_tmp699;struct Cyc_Tcpat_Rhs*_tmp698;switch(*((int*)_tmp694)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp698=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp694)->f1;_LL4:
 _tmp698->rhs=success;return d;default: _LL5: _tmp69B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp694)->f1;_tmp69A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp694)->f2;_tmp699=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp694)->f3;_LL6:
# 4016
({void*_tmpA63=Cyc_Toc_rewrite_decision(*_tmp699,success);*_tmp699=_tmpA63;});
for(0;_tmp69A != 0;_tmp69A=_tmp69A->tl){
struct _tuple33*_tmp695=(struct _tuple33*)_tmp69A->hd;struct _tuple33*_tmp696=_tmp695;void**_tmp697;_LL8: _tmp697=(void**)& _tmp696->f2;_LL9:;
({void*_tmpA64=Cyc_Toc_rewrite_decision(*_tmp697,success);*_tmp697=_tmpA64;});}
# 4021
return d;}_LL0:;}
# 4032 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4034
struct _RegionHandle _tmp69C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp69C;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp69D=Cyc_Toc_share_env(rgn,nv);
void*_tmp69E=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp69D,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4044
struct Cyc_Absyn_Stmt*_tmp69F=s;
if(dopt != 0)
# 4048
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp69F);{
# 4050
struct Cyc_Absyn_Switch_clause*_tmp6A0=({struct Cyc_Absyn_Switch_clause*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE->pattern=p,_tmp6AE->pat_vars=0,_tmp6AE->where_clause=0,_tmp6AE->body=_tmp69F,_tmp6AE->loc=0U;_tmp6AE;});
struct Cyc_List_List*_tmp6A1=({struct _RegionHandle*_tmpA67=rgn;struct _RegionHandle*_tmpA66=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple36*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpA67,Cyc_Toc_gen_labels,_tmpA66,({struct Cyc_Absyn_Switch_clause*_tmp6AD[1U];_tmp6AD[0]=_tmp6A0;({struct _RegionHandle*_tmpA65=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpA65,_tag_fat(_tmp6AD,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4053
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4057
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp69D,& mydecls,& mybodies,dopt,_tmp6A1,v);
# 4060
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple40*_tmp6A2=(struct _tuple40*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple40*_tmp6A3=_tmp6A2;struct Cyc_Toc_Env*_tmp6A5;struct Cyc_Absyn_Stmt*_tmp6A4;_LL1: _tmp6A5=_tmp6A3->f1;_tmp6A4=_tmp6A3->f3;_LL2:;
if(_tmp6A4 == _tmp69F){senv=_tmp6A5;goto FOUND_ENV;}}
# 4065
({void*_tmp6A6=0U;({struct _fat_ptr _tmpA68=({const char*_tmp6A7="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6A7,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA68,_tag_fat(_tmp6A6,sizeof(void*),0U));});});
FOUND_ENV:
# 4069
 Cyc_Toc_stmt_to_c(senv,s);{
# 4071
struct Cyc_Absyn_Stmt*res=test_tree;
# 4073
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple38 _tmp6A8=*((struct _tuple38*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple38 _tmp6A9=_tmp6A8;struct Cyc_Absyn_Vardecl*_tmp6AB;_LL4: _tmp6AB=_tmp6A9.f2;_LL5:;
res=({struct Cyc_Absyn_Decl*_tmpA69=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->tag=0U,_tmp6AA->f1=_tmp6AB;_tmp6AA;}),0U);Cyc_Absyn_decl_stmt(_tmpA69,res,0U);});}
# 4078
res=({struct _tuple1*_tmpA6C=v;void*_tmpA6B=Cyc_Toc_typ_to_c(_tmp69E);struct Cyc_Absyn_Exp*_tmpA6A=e;Cyc_Absyn_declare_stmt(_tmpA6C,_tmpA6B,_tmpA6A,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6AC=res;_npop_handler(0U);return _tmp6AC;};};};};}
# 4035
;_pop_region(rgn);}
# 4086
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6AF=e->r;void*_tmp6B0=_tmp6AF;struct Cyc_Absyn_MallocInfo*_tmp6D8;struct Cyc_Absyn_Stmt*_tmp6D7;void**_tmp6D6;void**_tmp6D5;struct Cyc_List_List*_tmp6D4;struct Cyc_List_List*_tmp6D3;void**_tmp6D2;struct Cyc_List_List*_tmp6D1;void**_tmp6D0;struct Cyc_Absyn_Exp*_tmp6CF;struct Cyc_Absyn_Exp*_tmp6CE;struct Cyc_List_List*_tmp6CD;struct Cyc_Absyn_Exp*_tmp6CC;struct Cyc_Absyn_Exp*_tmp6CB;struct Cyc_Absyn_Exp*_tmp6CA;struct Cyc_Absyn_Exp*_tmp6C9;struct Cyc_Absyn_Exp*_tmp6C8;struct Cyc_Absyn_Exp*_tmp6C7;struct Cyc_Absyn_Exp*_tmp6C6;struct Cyc_Absyn_Exp*_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C4;struct Cyc_Absyn_Exp*_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C1;struct Cyc_Absyn_Exp*_tmp6C0;struct Cyc_Absyn_Exp*_tmp6BF;struct Cyc_Absyn_Exp*_tmp6BE;struct Cyc_List_List*_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BB;struct Cyc_Absyn_Exp*_tmp6BA;struct Cyc_Absyn_Exp*_tmp6B9;struct Cyc_Absyn_Exp*_tmp6B8;struct Cyc_Absyn_Exp*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Exp*_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B4;switch(*((int*)_tmp6B0)){case 41U: _LL1: _tmp6B4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL2:
 _tmp6B5=_tmp6B4;goto _LL4;case 20U: _LL3: _tmp6B5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL4:
 _tmp6B6=_tmp6B5;goto _LL6;case 21U: _LL5: _tmp6B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL6:
 _tmp6B7=_tmp6B6;goto _LL8;case 22U: _LL7: _tmp6B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL8:
 _tmp6B8=_tmp6B7;goto _LLA;case 15U: _LL9: _tmp6B8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LLA:
 _tmp6B9=_tmp6B8;goto _LLC;case 11U: _LLB: _tmp6B9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LLC:
 _tmp6BA=_tmp6B9;goto _LLE;case 12U: _LLD: _tmp6BA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LLE:
 _tmp6BB=_tmp6BA;goto _LL10;case 18U: _LLF: _tmp6BB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL10:
 _tmp6BC=_tmp6BB;goto _LL12;case 5U: _LL11: _tmp6BC=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp6BC);goto _LL0;case 3U: _LL13: _tmp6BD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6BD);goto _LL0;case 7U: _LL15: _tmp6BF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6BE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL16:
 _tmp6C1=_tmp6BF;_tmp6C0=_tmp6BE;goto _LL18;case 8U: _LL17: _tmp6C1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6C0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL18:
 _tmp6C3=_tmp6C1;_tmp6C2=_tmp6C0;goto _LL1A;case 9U: _LL19: _tmp6C3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6C2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL1A:
 _tmp6C5=_tmp6C3;_tmp6C4=_tmp6C2;goto _LL1C;case 23U: _LL1B: _tmp6C5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6C4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL1C:
 _tmp6C7=_tmp6C5;_tmp6C6=_tmp6C4;goto _LL1E;case 35U: _LL1D: _tmp6C7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6C6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL1E:
 _tmp6C9=_tmp6C7;_tmp6C8=_tmp6C6;goto _LL20;case 4U: _LL1F: _tmp6C9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6C8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6B0)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp6C9);Cyc_Toc_exptypes_to_c(_tmp6C8);goto _LL0;case 6U: _LL21: _tmp6CC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6CB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_tmp6CA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6B0)->f3;_LL22:
# 4105
 Cyc_Toc_exptypes_to_c(_tmp6CC);Cyc_Toc_exptypes_to_c(_tmp6CB);Cyc_Toc_exptypes_to_c(_tmp6CA);goto _LL0;case 10U: _LL23: _tmp6CE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6CD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL24:
# 4107
 Cyc_Toc_exptypes_to_c(_tmp6CE);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6CD);goto _LL0;case 14U: _LL25: _tmp6D0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_tmp6CF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL26:
({void*_tmpA6D=Cyc_Toc_typ_to_c(*_tmp6D0);*_tmp6D0=_tmpA6D;});Cyc_Toc_exptypes_to_c(_tmp6CF);goto _LL0;case 25U: _LL27: _tmp6D2=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1)->f3;_tmp6D1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL28:
# 4110
({void*_tmpA6E=Cyc_Toc_typ_to_c(*_tmp6D2);*_tmp6D2=_tmpA6E;});
_tmp6D3=_tmp6D1;goto _LL2A;case 36U: _LL29: _tmp6D3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6B0)->f2;_LL2A:
 _tmp6D4=_tmp6D3;goto _LL2C;case 26U: _LL2B: _tmp6D4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL2C:
# 4114
 for(0;_tmp6D4 != 0;_tmp6D4=_tmp6D4->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple20*)_tmp6D4->hd)).f2);}
goto _LL0;case 19U: _LL2D: _tmp6D5=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL2E:
 _tmp6D6=_tmp6D5;goto _LL30;case 17U: _LL2F: _tmp6D6=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL30:
({void*_tmpA6F=Cyc_Toc_typ_to_c(*_tmp6D6);*_tmp6D6=_tmpA6F;});goto _LL0;case 37U: _LL31: _tmp6D7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp6D7);goto _LL0;case 34U: _LL33: _tmp6D8=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6B0)->f1;_LL34:
# 4121
 if(_tmp6D8->elt_type != 0)
({void**_tmpA71=({void**_tmp6B1=_cycalloc(sizeof(*_tmp6B1));({void*_tmpA70=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6D8->elt_type)));*_tmp6B1=_tmpA70;});_tmp6B1;});_tmp6D8->elt_type=_tmpA71;});
Cyc_Toc_exptypes_to_c(_tmp6D8->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4131
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
({void*_tmp6B2=0U;({unsigned _tmpA73=e->loc;struct _fat_ptr _tmpA72=({const char*_tmp6B3="Cyclone expression in C code";_tag_fat(_tmp6B3,sizeof(char),29U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmpA73,_tmpA72,_tag_fat(_tmp6B2,sizeof(void*),0U));});});}_LL0:;}
# 4145
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp6D9=d->r;void*_tmp6DA=_tmp6D9;struct Cyc_Absyn_Typedefdecl*_tmp6E4;struct Cyc_Absyn_Enumdecl*_tmp6E3;struct Cyc_Absyn_Aggrdecl*_tmp6E2;struct Cyc_Absyn_Fndecl*_tmp6E1;struct Cyc_Absyn_Vardecl*_tmp6E0;switch(*((int*)_tmp6DA)){case 0U: _LL1: _tmp6E0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6DA)->f1;_LL2:
# 4148
({void*_tmpA74=Cyc_Toc_typ_to_c(_tmp6E0->type);_tmp6E0->type=_tmpA74;});
if(_tmp6E0->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6E0->initializer));
goto _LL0;case 1U: _LL3: _tmp6E1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6DA)->f1;_LL4:
# 4152
({void*_tmpA75=Cyc_Toc_typ_to_c((_tmp6E1->i).ret_type);(_tmp6E1->i).ret_type=_tmpA75;});
{struct Cyc_List_List*_tmp6DB=(_tmp6E1->i).args;for(0;_tmp6DB != 0;_tmp6DB=_tmp6DB->tl){
({void*_tmpA76=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp6DB->hd)).f3);(*((struct _tuple9*)_tmp6DB->hd)).f3=_tmpA76;});}}
goto _LL0;case 5U: _LL5: _tmp6E2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6DA)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp6E2);goto _LL0;case 7U: _LL7: _tmp6E3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6DA)->f1;_LL8:
# 4158
 if(_tmp6E3->fields != 0){
struct Cyc_List_List*_tmp6DC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6E3->fields))->v;for(0;_tmp6DC != 0;_tmp6DC=_tmp6DC->tl){
struct Cyc_Absyn_Enumfield*_tmp6DD=(struct Cyc_Absyn_Enumfield*)_tmp6DC->hd;
if(_tmp6DD->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6DD->tag));}}
# 4163
goto _LL0;case 8U: _LL9: _tmp6E4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6DA)->f1;_LLA:
({void*_tmpA77=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6E4->defn));_tmp6E4->defn=_tmpA77;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4173
({void*_tmp6DE=0U;({unsigned _tmpA79=d->loc;struct _fat_ptr _tmpA78=({const char*_tmp6DF="Cyclone declaration in C code";_tag_fat(_tmp6DF,sizeof(char),30U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmpA79,_tmpA78,_tag_fat(_tmp6DE,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4181
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp6E5=s->r;void*_tmp6E6=_tmp6E5;struct Cyc_Absyn_Stmt*_tmp6FE;struct Cyc_Absyn_Stmt*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;struct Cyc_Absyn_Decl*_tmp6FB;struct Cyc_Absyn_Stmt*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6F9;struct Cyc_List_List*_tmp6F8;void*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F4;struct Cyc_Absyn_Stmt*_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Stmt*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F0;struct Cyc_Absyn_Stmt*_tmp6EF;struct Cyc_Absyn_Stmt*_tmp6EE;struct Cyc_Absyn_Exp*_tmp6ED;struct Cyc_Absyn_Stmt*_tmp6EC;struct Cyc_Absyn_Stmt*_tmp6EB;struct Cyc_Absyn_Exp*_tmp6EA;switch(*((int*)_tmp6E6)){case 1U: _LL1: _tmp6EA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp6EA);goto _LL0;case 2U: _LL3: _tmp6EC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_tmp6EB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6EC);Cyc_Toc_stmttypes_to_c(_tmp6EB);goto _LL0;case 3U: _LL5: _tmp6ED=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_LL6:
 if(_tmp6ED != 0)Cyc_Toc_exptypes_to_c(_tmp6ED);goto _LL0;case 4U: _LL7: _tmp6F0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_tmp6EF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2;_tmp6EE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f3;_LL8:
# 4187
 Cyc_Toc_exptypes_to_c(_tmp6F0);Cyc_Toc_stmttypes_to_c(_tmp6EF);Cyc_Toc_stmttypes_to_c(_tmp6EE);goto _LL0;case 5U: _LL9: _tmp6F2=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1).f1;_tmp6F1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2;_LLA:
# 4189
 Cyc_Toc_exptypes_to_c(_tmp6F2);Cyc_Toc_stmttypes_to_c(_tmp6F1);goto _LL0;case 9U: _LLB: _tmp6F6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_tmp6F5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2).f1;_tmp6F4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f3).f1;_tmp6F3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f4;_LLC:
# 4191
 Cyc_Toc_exptypes_to_c(_tmp6F6);Cyc_Toc_exptypes_to_c(_tmp6F5);Cyc_Toc_exptypes_to_c(_tmp6F4);
Cyc_Toc_stmttypes_to_c(_tmp6F3);goto _LL0;case 10U: _LLD: _tmp6F9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_tmp6F8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2;_tmp6F7=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f3;_LLE:
# 4194
 Cyc_Toc_exptypes_to_c(_tmp6F9);
for(0;_tmp6F8 != 0;_tmp6F8=_tmp6F8->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6F8->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6FB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_tmp6FA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6FB);Cyc_Toc_stmttypes_to_c(_tmp6FA);goto _LL0;case 14U: _LL11: _tmp6FD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f1;_tmp6FC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6FD);Cyc_Toc_exptypes_to_c(_tmp6FC);goto _LL0;case 13U: _LL13: _tmp6FE=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp6E6)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6FE);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4206
({void*_tmpA7A=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->tag=0U;_tmp6E7;});s->r=_tmpA7A;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp6E8=0U;({unsigned _tmpA7C=s->loc;struct _fat_ptr _tmpA7B=({const char*_tmp6E9="Cyclone statement in C code";_tag_fat(_tmp6E9,sizeof(char),28U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos_loc)(_tmpA7C,_tmpA7B,_tag_fat(_tmp6E8,sizeof(void*),0U));});});}_LL0:;}
# 4214
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4216
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6FF=0U;({struct _fat_ptr _tmpA7D=({const char*_tmp700="decls_to_c: not at toplevel!";_tag_fat(_tmp700,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpA7D,_tag_fat(_tmp6FF,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp701=d->r;void*_tmp702=_tmp701;struct Cyc_List_List*_tmp727;struct Cyc_List_List*_tmp726;struct Cyc_List_List*_tmp725;struct Cyc_List_List*_tmp724;struct Cyc_Absyn_Typedefdecl*_tmp723;struct Cyc_Absyn_Enumdecl*_tmp722;struct Cyc_Absyn_Datatypedecl*_tmp721;struct Cyc_Absyn_Aggrdecl*_tmp720;struct Cyc_Absyn_Fndecl*_tmp71F;struct Cyc_Absyn_Vardecl*_tmp71E;switch(*((int*)_tmp702)){case 0U: _LL1: _tmp71E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LL2: {
# 4223
struct _tuple1*_tmp703=_tmp71E->name;
# 4225
if((int)_tmp71E->sc == (int)Cyc_Absyn_ExternC)
_tmp703=({struct _tuple1*_tmp704=_cycalloc(sizeof(*_tmp704));({union Cyc_Absyn_Nmspace _tmpA7E=Cyc_Absyn_Abs_n(0,1);_tmp704->f1=_tmpA7E;}),_tmp704->f2=(*_tmp703).f2;_tmp704;});
if(_tmp71E->initializer != 0){
if((int)_tmp71E->sc == (int)Cyc_Absyn_ExternC)_tmp71E->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp71E->initializer));else{
# 4232
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp71E->initializer));}}
# 4234
_tmp71E->name=_tmp703;
({enum Cyc_Absyn_Scope _tmpA7F=Cyc_Toc_scope_to_c(_tmp71E->sc);_tmp71E->sc=_tmpA7F;});
({void*_tmpA80=Cyc_Toc_typ_to_c(_tmp71E->type);_tmp71E->type=_tmpA80;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705->hd=d,_tmp705->tl=Cyc_Toc_result_decls;_tmp705;});
goto _LL0;}case 1U: _LL3: _tmp71F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LL4:
# 4241
 if((int)_tmp71F->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpA82=({struct _tuple1*_tmp706=_cycalloc(sizeof(*_tmp706));({union Cyc_Absyn_Nmspace _tmpA81=Cyc_Absyn_Abs_n(0,1);_tmp706->f1=_tmpA81;}),_tmp706->f2=(*_tmp71F->name).f2;_tmp706;});_tmp71F->name=_tmpA82;});
_tmp71F->sc=Cyc_Absyn_Public;}
# 4245
Cyc_Toc_fndecl_to_c(nv,_tmp71F,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707->hd=d,_tmp707->tl=Cyc_Toc_result_decls;_tmp707;});
goto _LL0;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp708=0U;({struct _fat_ptr _tmpA83=({const char*_tmp709="letdecl at toplevel";_tag_fat(_tmp709,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA83,_tag_fat(_tmp708,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp70A=0U;({struct _fat_ptr _tmpA84=({const char*_tmp70B="region decl at toplevel";_tag_fat(_tmp70B,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA84,_tag_fat(_tmp70A,sizeof(void*),0U));});});case 5U: _LLB: _tmp720=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LLC:
 Cyc_Toc_aggrdecl_to_c(_tmp720);goto _LL0;case 6U: _LLD: _tmp721=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LLE:
# 4253
 _tmp721->is_extensible?Cyc_Toc_xdatatypedecl_to_c(_tmp721): Cyc_Toc_datatypedecl_to_c(_tmp721);
goto _LL0;case 7U: _LLF: _tmp722=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LL10:
# 4256
 Cyc_Toc_enumdecl_to_c(_tmp722);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C->hd=d,_tmp70C->tl=Cyc_Toc_result_decls;_tmp70C;});
goto _LL0;case 8U: _LL11: _tmp723=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LL12:
# 4260
 _tmp723->tvs=0;
if(_tmp723->defn != 0){
({void*_tmpA85=Cyc_Toc_typ_to_c((void*)_check_null(_tmp723->defn));_tmp723->defn=_tmpA85;});{
# 4265
struct Cyc_Absyn_Decl*ed;
{void*_tmp70D=_tmp723->defn;void*_tmp70E=_tmp70D;struct Cyc_Absyn_Enumdecl*_tmp712;unsigned _tmp711;if(_tmp70E != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70E)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70E)->f1)->r)->tag == 1U){_LL24: _tmp712=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70E)->f1)->r)->f1;_tmp711=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70E)->f1)->loc;_LL25:
# 4268
 ed=({struct Cyc_Absyn_Decl*_tmp710=_cycalloc(sizeof(*_tmp710));({void*_tmpA86=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F->tag=7U,_tmp70F->f1=_tmp712;_tmp70F;});_tmp710->r=_tmpA86;}),_tmp710->loc=_tmp711;_tmp710;});goto _LL23;}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4271
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->hd=ed,_tmp713->tl=Cyc_Toc_result_decls;_tmp713;});{
void*_tmp714=ed->r;void*_tmp715=_tmp714;struct Cyc_Absyn_Enumdecl*_tmp71A;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp715)->tag == 7U){_LL29: _tmp71A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp715)->f1;_LL2A:
# 4275
({void*_tmpA88=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->tag=0U,({void*_tmpA87=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->tag=15U,_tmp716->f1=_tmp71A->name,_tmp716->f2=_tmp71A;_tmp716;});_tmp717->f1=_tmpA87;}),_tmp717->f2=0;_tmp717;});_tmp723->defn=_tmpA88;});goto _LL28;}else{_LL2B: _LL2C:
({void*_tmp718=0U;({struct _fat_ptr _tmpA89=({const char*_tmp719="Toc: enumdecl to name";_tag_fat(_tmp719,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmpA89,_tag_fat(_tmp718,sizeof(void*),0U));});});}_LL28:;};}};}else{
# 4280
enum Cyc_Absyn_KindQual _tmp71B=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp723->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp71C=_tmp71B;if(_tmp71C == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpA8A=Cyc_Toc_void_star_type();_tmp723->defn=_tmpA8A;});goto _LL2D;}else{_LL30: _LL31:
 _tmp723->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4288
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->hd=d,_tmp71D->tl=Cyc_Toc_result_decls;_tmp71D;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp724=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp702)->f2;_LL1C:
 _tmp725=_tmp724;goto _LL1E;case 10U: _LL1D: _tmp725=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp702)->f2;_LL1E:
 _tmp726=_tmp725;goto _LL20;case 11U: _LL1F: _tmp726=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LL20:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp726,cinclude);goto _LL0;default: _LL21: _tmp727=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp702)->f1;_LL22:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp727,1);goto _LL0;}_LL0:;};}
# 4301
return nv;}
# 4305
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp728=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp729=_tmp728;struct Cyc_Core_DynamicRegion*_tmp72E;_LL1: _tmp72E=_tmp729.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp72E->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp72B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp72B));_tmp72B->dyn=_tmp72E,_tmp72B->state=ts;_tmp72B;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp72D=34;struct _fat_ptr**_tmp72C=_cycalloc(_check_times(_tmp72D,sizeof(struct _fat_ptr*)));_tmp72C[0]=& Cyc_Toc__throw_str,_tmp72C[1]=& Cyc_Toc_setjmp_str,_tmp72C[2]=& Cyc_Toc__push_handler_str,_tmp72C[3]=& Cyc_Toc__pop_handler_str,_tmp72C[4]=& Cyc_Toc__exn_thrown_str,_tmp72C[5]=& Cyc_Toc__npop_handler_str,_tmp72C[6]=& Cyc_Toc__check_null_str,_tmp72C[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp72C[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp72C[9]=& Cyc_Toc__check_fat_subscript_str,_tmp72C[10]=& Cyc_Toc__tag_fat_str,_tmp72C[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp72C[12]=& Cyc_Toc__get_fat_size_str,_tmp72C[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp72C[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp72C[15]=& Cyc_Toc__zero_arr_plus_str,_tmp72C[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp72C[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp72C[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp72C[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp72C[20]=& Cyc_Toc__cycalloc_str,_tmp72C[21]=& Cyc_Toc__cyccalloc_str,_tmp72C[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp72C[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp72C[24]=& Cyc_Toc__region_malloc_str,_tmp72C[25]=& Cyc_Toc__region_calloc_str,_tmp72C[26]=& Cyc_Toc__check_times_str,_tmp72C[27]=& Cyc_Toc__new_region_str,_tmp72C[28]=& Cyc_Toc__push_region_str,_tmp72C[29]=& Cyc_Toc__pop_region_str,_tmp72C[30]=& Cyc_Toc__throw_arraybounds_str,_tmp72C[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp72C[32]=& Cyc_Toc__throw_match_str,_tmp72C[33]=& Cyc_Toc__fast_region_malloc_str;_tmp72C;}),sizeof(struct _fat_ptr*),34U);};}
# 4353
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp72F=ts;struct Cyc_Toc_TocStateWrap*_tmp730=Cyc_Toc_toc_state;ts=_tmp730;Cyc_Toc_toc_state=_tmp72F;});{
struct Cyc_Toc_TocStateWrap _tmp731=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp732=_tmp731;struct Cyc_Core_DynamicRegion*_tmp738;struct Cyc_Toc_TocState*_tmp737;_LL1: _tmp738=_tmp732.dyn;_tmp737=_tmp732.state;_LL2:;
# 4358
{struct _RegionHandle*h=& _tmp738->h;
{struct Cyc_Toc_TocState _tmp734=*_tmp737;struct Cyc_Toc_TocState _tmp735=_tmp734;struct Cyc_Xarray_Xarray*_tmp736;_LL4: _tmp736=_tmp735.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp736);}
# 4359
;}
# 4362
Cyc_Core_free_rckey(_tmp738);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4365
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4371
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4373
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp739=_cycalloc(sizeof(*_tmp739));*_tmp739=pop_tables;_tmp739;});
Cyc_Toc_init();{
struct _RegionHandle _tmp73A=_new_region("start");struct _RegionHandle*start=& _tmp73A;_push_region(start);
({struct _RegionHandle*_tmpA8C=start;struct Cyc_Toc_Env*_tmpA8B=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpA8C,_tmpA8B,ds,0);});{
struct Cyc_List_List*_tmp73B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp73B;};
# 4376
;_pop_region(start);};}
