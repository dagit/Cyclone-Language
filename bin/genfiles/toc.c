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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;
# 178
void Cyc_Core_ufree(void*ptr);
# 191
struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 255
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 273 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 57
extern struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
extern int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 328
extern void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 67
extern struct _fat_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 700 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 879
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 886
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress(void*);
# 921
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 923
extern void*Cyc_Absyn_sint_type;
# 928
extern void*Cyc_Absyn_heap_rgn_type;
# 932
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 934
extern void*Cyc_Absyn_void_type;
# 951
void*Cyc_Absyn_exn_type (void);
# 959
extern void*Cyc_Absyn_fat_bound_type;
# 963
void*Cyc_Absyn_bounds_one (void);
# 971
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel);
# 975
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 977
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt,void*rel);
# 979
void*Cyc_Absyn_strct(struct _fat_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 985
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 990
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1065
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1087
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1091
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1096
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1134
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1139
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1149
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1151
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1153
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1162
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 38 "warn.h"
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 46
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
void*Cyc_Warn_vimpos2(struct _fat_ptr);
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 30 "flags.h"
extern int Cyc_Flags_no_regions;
# 66
extern int Cyc_Flags_noexpand_r;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
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
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 50
int Cyc_Tcutil_is_fat_ptr(void*);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 55
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 126
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 173
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 235
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 39
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 57 "attributes.h"
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 37
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 41
struct _tuple1*Cyc_Toc_temp_var (void);
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_init (void);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};char Cyc_Toc_Dest[5U]="Dest";
# 68 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp791;_tmp791.tag=0U,_tmp791.f1=str;_tmp791;});void*_tmp0[1U];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1U));});}
# 71
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 73
({struct _fat_ptr _tmp80A=(struct _fat_ptr)({struct _fat_ptr _tmp809=({const char*_tmp2="Toc: ";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_strconcat(_tmp809,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp80A,ap);});}
# 75
static void*Cyc_Toc_toc_impos2(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=0U,({struct _fat_ptr _tmp80B=({const char*_tmp5="Toc: ";_tag_fat(_tmp5,sizeof(char),6U);});_tmp6->f1=_tmp80B;});_tmp6;});
struct _fat_ptr args=({unsigned _tmp4=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned _tmp792=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;unsigned i;for(i=0;i < _tmp792;++ i){i == (unsigned)0?_tmp3[i]=toc:(_tmp3[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - (unsigned)1))));}}0;});_tag_fat(_tmp3,sizeof(void*),_tmp4);});
Cyc_Warn_vimpos2(args);}
# 81
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 84
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 94
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 96
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 122
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 129
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmpE=_region_malloc(d,sizeof(*_tmpE));
({struct Cyc_List_List**_tmp817=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpE->tuple_types=_tmp817;}),({
struct Cyc_List_List**_tmp816=({struct Cyc_List_List**_tmp8=_region_malloc(d,sizeof(*_tmp8));*_tmp8=0;_tmp8;});_tmpE->anon_aggr_types=_tmp816;}),({
struct Cyc_Dict_Dict*_tmp815=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));({struct Cyc_Dict_Dict _tmp814=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp9=_tmp814;});_tmp9;});_tmpE->aggrs_so_far=_tmp815;}),({
struct Cyc_List_List**_tmp813=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmpE->abs_struct_types=_tmp813;}),({
struct Cyc_Set_Set**_tmp812=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Set_Set*_tmp811=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp811;});_tmpB;});_tmpE->datatypes_so_far=_tmp812;}),({
struct Cyc_Dict_Dict*_tmp810=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp80F=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp80F;});_tmpC;});_tmpE->xdatatypes_so_far=_tmp810;}),({
struct Cyc_Dict_Dict*_tmp80E=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Dict_Dict _tmp80D=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmpD=_tmp80D;});_tmpD;});_tmpE->qvar_tags=_tmp80E;}),({
struct Cyc_Xarray_Xarray*_tmp80C=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpE->temp_labels=_tmp80C;});_tmpE;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 148
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 154
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 157
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
struct Cyc_Toc_TocStateWrap _tmp11=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp11;struct Cyc_Toc_TocStateWrap _tmp12=_stmttmp0;struct Cyc_Toc_TocState*_tmp14;struct Cyc_Core_DynamicRegion*_tmp13;_LL1: _tmp13=_tmp12.dyn;_tmp14=_tmp12.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp13;struct Cyc_Toc_TocState*s=_tmp14;
struct _fat_ptr _tmp15=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp15;
({struct Cyc_Toc_TocStateWrap _tmp818=({struct Cyc_Toc_TocStateWrap _tmp793;_tmp793.dyn=dyn,_tmp793.state=s;_tmp793;});*ts=_tmp818;});
({struct Cyc_Toc_TocStateWrap*_tmp16=ts;struct Cyc_Toc_TocStateWrap*_tmp17=Cyc_Toc_toc_state;ts=_tmp17;Cyc_Toc_toc_state=_tmp16;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp19=({struct _tuple15 _tmp794;_tmp794.f1=s,_tmp794.f2=arg;_tmp794;});struct _tuple15 env=_tmp19;
res=f(h,& env);}
# 165
;}
# 167
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 171
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 174
struct _tuple17 _tmp1A=*env;struct _tuple17 _stmttmp1=_tmp1A;struct _tuple17 _tmp1B=_stmttmp1;void*(*_tmp1E)(struct _tuple1*);struct _tuple1*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1C=_tmp1B.f1;_tmp1D=(_tmp1B.f2)->f1;_tmp1E=(_tmp1B.f2)->f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp1C;struct _tuple1*q=_tmp1D;void*(*type_maker)(struct _tuple1*)=_tmp1E;
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 179
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp795;_tmp795.f1=q,_tmp795.f2=type_maker;_tmp795;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1F[5U]="curr";
# 192 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1F,_tmp1F,_tmp1F + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp20[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp20,_tmp20,_tmp20 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp21[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp21,_tmp21,_tmp21 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp22[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp22,_tmp22,_tmp22 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp23[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp23,_tmp23,_tmp23 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp24[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp24,_tmp24,_tmp24 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp25[12U]="_PoolHandle";
static struct _fat_ptr Cyc_Toc__PoolHandle_string={_tmp25,_tmp25,_tmp25 + 12U};static struct _fat_ptr*Cyc_Toc__PoolHandle_sp=& Cyc_Toc__PoolHandle_string;static char _tmp26[7U]="_throw";
# 212 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_tmp26,_tmp26,_tmp26 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp27[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_tmp27,_tmp27,_tmp27 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp28[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_tmp28,_tmp28,_tmp28 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp29[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_tmp29,_tmp29,_tmp29 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp2A[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_tmp2A,_tmp2A,_tmp2A + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp2B[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_tmp2B,_tmp2B,_tmp2B + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2C[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_tmp2C,_tmp2C,_tmp2C + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2D[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,_tmp2D + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2E[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2E,_tmp2E,_tmp2E + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp2F[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_tmp2F,_tmp2F,_tmp2F + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0U,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1U,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _tmp30[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_tmp30,_tmp30,_tmp30 + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0U,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1U,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _tmp31[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_tmp31,_tmp31,_tmp31 + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0U,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1U,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _tmp32[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_tmp32,_tmp32,_tmp32 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0U,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1U,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _tmp33[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_tmp33,_tmp33,_tmp33 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0U,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1U,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _tmp34[19U]="_get_zero_arr_size";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_str={_tmp34,_tmp34,_tmp34 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp35[24U]="_get_zero_arr_size_char";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp35,_tmp35,_tmp35 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp36[25U]="_get_zero_arr_size_short";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp36,_tmp36,_tmp36 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp37[23U]="_get_zero_arr_size_int";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp37,_tmp37,_tmp37 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp38[25U]="_get_zero_arr_size_float";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp38,_tmp38,_tmp38 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp39[26U]="_get_zero_arr_size_double";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp39,_tmp39,_tmp39 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp3A[30U]="_get_zero_arr_size_longdouble";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp3A,_tmp3A,_tmp3A + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp3B[28U]="_get_zero_arr_size_voidstar";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp3B,_tmp3B,_tmp3B + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp3C[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_tmp3C,_tmp3C,_tmp3C + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0U,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1U,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _tmp3D[15U]="_zero_arr_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_str={_tmp3D,_tmp3D,_tmp3D + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp3E[20U]="_zero_arr_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp3E,_tmp3E,_tmp3E + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp3F[21U]="_zero_arr_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp3F,_tmp3F,_tmp3F + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp40[19U]="_zero_arr_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp40,_tmp40,_tmp40 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp41[21U]="_zero_arr_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp41,_tmp41,_tmp41 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp42[22U]="_zero_arr_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp42,_tmp42,_tmp42 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp43[26U]="_zero_arr_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp43,_tmp43,_tmp43 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp44[24U]="_zero_arr_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp44,_tmp44,_tmp44 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp45[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_tmp45,_tmp45,_tmp45 + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0U,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _tmp46[23U]="_zero_arr_inplace_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp46,_tmp46,_tmp46 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp47[28U]="_zero_arr_inplace_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp47,_tmp47,_tmp47 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp48[29U]="_zero_arr_inplace_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp48,_tmp48,_tmp48 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp49[27U]="_zero_arr_inplace_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp49,_tmp49,_tmp49 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp4A[29U]="_zero_arr_inplace_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp4A,_tmp4A,_tmp4A + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp4B[30U]="_zero_arr_inplace_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp4B,_tmp4B,_tmp4B + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp4C[34U]="_zero_arr_inplace_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp4C,_tmp4C,_tmp4C + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp4D[32U]="_zero_arr_inplace_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp4D,_tmp4D,_tmp4D + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp4E[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_tmp4E,_tmp4E,_tmp4E + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _tmp4F[28U]="_zero_arr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp4F,_tmp4F,_tmp4F + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp50[33U]="_zero_arr_inplace_plus_post_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp50,_tmp50,_tmp50 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp51[34U]="_zero_arr_inplace_plus_post_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp51,_tmp51,_tmp51 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp52[32U]="_zero_arr_inplace_plus_post_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp52,_tmp52,_tmp52 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp53[34U]="_zero_arr_inplace_plus_post_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp53,_tmp53,_tmp53 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp54[35U]="_zero_arr_inplace_plus_post_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp54,_tmp54,_tmp54 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp55[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp55,_tmp55,_tmp55 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp56[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp56,_tmp56,_tmp56 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp57[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_tmp57,_tmp57,_tmp57 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp58[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_tmp58,_tmp58,_tmp58 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp59[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_tmp59,_tmp59,_tmp59 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp5A[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp5A,_tmp5A,_tmp5A + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp5B[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_tmp5B,_tmp5B,_tmp5B + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp5C[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_tmp5C,_tmp5C,_tmp5C + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp5D[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_tmp5D,_tmp5D,_tmp5D + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp5E[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_tmp5E,_tmp5E,_tmp5E + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp5F[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_tmp5F,_tmp5F,_tmp5F + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp60[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_tmp60,_tmp60,_tmp60 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp61[10U]="_new_pool";
static struct _fat_ptr Cyc_Toc__new_pool_str={_tmp61,_tmp61,_tmp61 + 10U};static struct _tuple1 Cyc_Toc__new_pool_pr={{.Loc_n={4,0}},& Cyc_Toc__new_pool_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_pool_bnd={0U,& Cyc_Toc__new_pool_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_pool_re={1U,(void*)& Cyc_Toc__new_pool_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_pool_ev={0,(void*)& Cyc_Toc__new_pool_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_pool_e=& Cyc_Toc__new_pool_ev;static char _tmp62[11U]="_push_pool";
static struct _fat_ptr Cyc_Toc__push_pool_str={_tmp62,_tmp62,_tmp62 + 11U};static struct _tuple1 Cyc_Toc__push_pool_pr={{.Loc_n={4,0}},& Cyc_Toc__push_pool_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_pool_bnd={0U,& Cyc_Toc__push_pool_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_pool_re={1U,(void*)& Cyc_Toc__push_pool_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_pool_ev={0,(void*)& Cyc_Toc__push_pool_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_pool_e=& Cyc_Toc__push_pool_ev;static char _tmp63[10U]="_pop_pool";
static struct _fat_ptr Cyc_Toc__pop_pool_str={_tmp63,_tmp63,_tmp63 + 10U};static struct _tuple1 Cyc_Toc__pop_pool_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_pool_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_pool_bnd={0U,& Cyc_Toc__pop_pool_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_pool_re={1U,(void*)& Cyc_Toc__pop_pool_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_pool_ev={0,(void*)& Cyc_Toc__pop_pool_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_pool_e=& Cyc_Toc__pop_pool_ev;static char _tmp64[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_tmp64,_tmp64,_tmp64 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp65[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_tmp65,_tmp65,_tmp65 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0U,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _tmp66[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_tmp66,_tmp66,_tmp66 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp67[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_tmp67,_tmp67,_tmp67 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp68[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_tmp68,_tmp68,_tmp68 + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 283
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 286
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp73=_cycalloc(sizeof(*_tmp73));((_tmp73->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp81D=({struct _fat_ptr*_tmp6E[1U];({struct _fat_ptr*_tmp81C=({struct _fat_ptr*_tmp70=_cycalloc(sizeof(*_tmp70));({struct _fat_ptr _tmp81B=({const char*_tmp6F="Core";_tag_fat(_tmp6F,sizeof(char),5U);});*_tmp70=_tmp81B;});_tmp70;});_tmp6E[0]=_tmp81C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6E,sizeof(struct _fat_ptr*),1U));});((_tmp73->f1).Abs_n).val=_tmp81D;}),({struct _fat_ptr*_tmp81A=({struct _fat_ptr*_tmp72=_cycalloc(sizeof(*_tmp72));({struct _fat_ptr _tmp819=({const char*_tmp71="get_exn_thrown";_tag_fat(_tmp71,sizeof(char),15U);});*_tmp72=_tmp819;});_tmp72;});_tmp73->f2=_tmp81A;});_tmp73;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=0U,_tmp6D->f1=qv;_tmp6D;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->topt=0,({void*_tmp81E=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=1U,_tmp6B->f1=bnd;_tmp6B;});_tmp6C->r=_tmp81E;}),_tmp6C->loc=0U,_tmp6C->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp6C;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=10U,_tmp6A->f1=fnname,_tmp6A->f2=0,_tmp6A->f3=0,_tmp6A->f4=0;_tmp6A;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->topt=0,_tmp69->r=fncall_re,_tmp69->loc=0U,_tmp69->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp69;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 296
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 298
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_tmp822=Cyc_Absyn_void_type;void*_tmp821=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp820=Cyc_Absyn_empty_tqual(0U);void*_tmp81F=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp822,_tmp821,_tmp820,_tmp81F,Cyc_Absyn_false_type);});
return(void*)_check_null(t);}
# 304
static void*Cyc_Toc_fat_ptr_type (void){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__fat_ptr_pr,0),0);
return(void*)_check_null(t);}
# 310
static void*Cyc_Toc_rgn_type (void){
static void*r=0;
if(r == 0)
r=({void*_tmp823=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp823,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 317
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 321
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp76=_cycalloc(sizeof(*_tmp76));({struct Cyc_List_List*_tmp825=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(*_tmp75));({void*_tmp824=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=1U,_tmp74->f1=name;_tmp74;});_tmp75->hd=_tmp824;}),_tmp75->tl=0;_tmp75;});_tmp76->f1=_tmp825;}),_tmp76->f2=e;_tmp76;});}
# 325
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp826=f;Cyc_Absyn_fncall_exp(_tmp826,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
# 328
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp77=e->r;void*_stmttmp2=_tmp77;void*_tmp78=_stmttmp2;struct Cyc_Absyn_Exp*_tmp79;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp78)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp78)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp79=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp78)->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp79;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 336
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=14U,_tmp7A->f1=t,_tmp7A->f2=e,_tmp7A->f3=0,_tmp7A->f4=Cyc_Absyn_No_coercion;_tmp7A;});}
# 339
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=20U,_tmp7B->f1=e;_tmp7B;});}
# 342
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=23U,_tmp7C->f1=e1,_tmp7C->f2=e2;_tmp7C;});}
# 345
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=37U,_tmp7D->f1=s;_tmp7D;});}
# 348
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=17U,_tmp7E->f1=t;_tmp7E;});}
# 351
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=10U,_tmp7F->f1=e,({struct Cyc_List_List*_tmp827=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp7F->f2=_tmp827;}),_tmp7F->f3=0,_tmp7F->f4=1;_tmp7F;});}
# 354
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=2U,_tmp80->f1=s1,_tmp80->f2=s2;_tmp80;});}
# 357
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=6U,_tmp81->f1=e1,_tmp81->f2=e2,_tmp81->f3=e3;_tmp81;});}
# 360
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=21U,_tmp82->f1=e,_tmp82->f2=n,_tmp82->f3=0,_tmp82->f4=0;_tmp82;});}
# 363
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 366
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=36U,_tmp83->f1=tdopt,_tmp83->f2=ds;_tmp83;});}
# 368
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=8U,_tmp84->f1=v;_tmp84;});}
# 372
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 376
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp85=e->r;void*_stmttmp3=_tmp85;void*_tmp86=_stmttmp3;struct Cyc_Absyn_Exp*_tmp87;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp86)->tag == 20U){_LL1: _tmp87=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp86)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp87;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 404 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 410
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp88=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp88;void*_tmp89=_stmttmp4;switch(*((int*)_tmp89)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1: _LL2:
 return fS->fchar;case Cyc_Absyn_Short_sz: _LL3: _LL4:
 return fS->fshort;case Cyc_Absyn_Int_sz: _LL5: _LL6:
 return fS->fint;default: goto _LLF;}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp89)->f1)->f1){case 0U: _LL7: _LL8:
 return fS->ffloat;case 1U: _LL9: _LLA:
 return fS->fdouble;default: _LLB: _LLC:
 return fS->flongdouble;}default: goto _LLF;}case 3U: _LLD: _LLE:
 return fS->fvoidstar;default: _LLF: _LL10:
# 420
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp798;_tmp798.tag=0U,({struct _fat_ptr _tmp828=({const char*_tmp8F="expression type ";_tag_fat(_tmp8F,sizeof(char),17U);});_tmp798.f1=_tmp828;});_tmp798;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp8C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp797;_tmp797.tag=2U,_tmp797.f1=(void*)t;_tmp797;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp796;_tmp796.tag=0U,({struct _fat_ptr _tmp829=({const char*_tmp8E=" (not int, float, double, or pointer)";_tag_fat(_tmp8E,sizeof(char),38U);});_tmp796.f1=_tmp829;});_tmp796;});void*_tmp8A[3U];_tmp8A[0]=& _tmp8B,_tmp8A[1]=& _tmp8C,_tmp8A[2]=& _tmp8D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp8A,sizeof(void*),3U));});}_LL0:;}
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp90=Cyc_Absyn_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp90;void*_tmp91=_stmttmp5;void*_tmp92;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 3U){_LL1: _tmp92=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_type;_LL2: {void*et=_tmp92;
return Cyc_Toc_getFunctionType(fS,et);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp94=({struct Cyc_Warn_String_Warn_Warg_struct _tmp799;_tmp799.tag=0U,({struct _fat_ptr _tmp82A=({const char*_tmp95="impossible type (not pointer)";_tag_fat(_tmp95,sizeof(char),30U);});_tmp799.f1=_tmp82A;});_tmp799;});void*_tmp93[1U];_tmp93[0]=& _tmp94;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp93,sizeof(void*),1U));});}_LL0:;}
# 433
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp96=e->r;void*_stmttmp6=_tmp96;void*_tmp97=_stmttmp6;struct Cyc_List_List*_tmp98;struct Cyc_List_List*_tmp99;struct Cyc_List_List*_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;long long _tmp9E;int _tmp9F;short _tmpA0;struct _fat_ptr _tmpA1;char _tmpA2;switch(*((int*)_tmp97)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97)->f1).Null_c).tag){case 2U: _LL1: _tmpA2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97)->f1).Char_c).val).f2;_LL2: {char c=_tmpA2;
return(int)c == (int)'\000';}case 3U: _LL3: _tmpA1=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmpA1;
# 437
unsigned long _tmpA3=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmpA3;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 447
return 0;}}case 4U: _LL5: _tmpA0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97)->f1).Short_c).val).f2;_LL6: {short i=_tmpA0;
return(int)i == 0;}case 5U: _LL7: _tmp9F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97)->f1).Int_c).val).f2;_LL8: {int i=_tmp9F;
return i == 0;}case 6U: _LL9: _tmp9E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97)->f1).LongLong_c).val).f2;_LLA: {long long i=_tmp9E;
return i == (long long)0;}case 1U: _LLD: _LLE:
# 452
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 451
 goto _LLE;case 14U: _LLF: _tmp9D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp97)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp9D;
# 453
return Cyc_Toc_is_zero(e1);}case 24U: _LL11: _tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_LL12: {struct Cyc_List_List*es=_tmp9C;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,es);}case 26U: _LL13: _tmp9B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp97)->f1;_LL14: {struct Cyc_List_List*dles=_tmp9B;
_tmp9A=dles;goto _LL16;}case 29U: _LL15: _tmp9A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp97)->f3;_LL16: {struct Cyc_List_List*dles=_tmp9A;
_tmp99=dles;goto _LL18;}case 25U: _LL17: _tmp99=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp97)->f2;_LL18: {struct Cyc_List_List*dles=_tmp99;
_tmp98=dles;goto _LL1A;}case 36U: _LL19: _tmp98=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp97)->f2;_LL1A: {struct Cyc_List_List*dles=_tmp98;
# 459
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 467
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA4=x;struct _fat_ptr*_tmpA6;union Cyc_Absyn_Nmspace _tmpA5;_LL1: _tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmpA5;struct _fat_ptr*v=_tmpA6;
union Cyc_Absyn_Nmspace _tmpA7=ns;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA9;struct Cyc_List_List*_tmpAA;switch((_tmpA7.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAA=0;goto _LL7;case 1U: _LL6: _tmpAA=(_tmpA7.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmpAA;
_tmpA9=vs;goto _LL9;}case 2U: _LL8: _tmpA9=(_tmpA7.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmpA9;
_tmpA8=vs;goto _LLB;}default: _LLA: _tmpA8=(_tmpA7.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmpA8;
# 476
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,_tmp79B.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp79B;});struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,_tmp79A.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp79A;});void*_tmpAB[2U];_tmpAB[0]=& _tmpAD,_tmpAB[1]=& _tmpAE;({struct _fat_ptr _tmp82B=({const char*_tmpAC="%s_%s_struct";_tag_fat(_tmpAC,sizeof(char),13U);});Cyc_aprintf(_tmp82B,_tag_fat(_tmpAB,sizeof(void*),2U));});});{
struct _RegionHandle _tmpAF=_new_region("r");struct _RegionHandle*r=& _tmpAF;_push_region(r);
{struct _fat_ptr _tmpB6=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB2=({struct Cyc_String_pa_PrintArg_struct _tmp79E;_tmp79E.tag=0U,_tmp79E.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp79E;});struct Cyc_String_pa_PrintArg_struct _tmpB3=({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0U,({struct _fat_ptr _tmp82E=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp82D=r;struct Cyc_List_List*_tmp82C=vs;Cyc_rstr_sepstr(_tmp82D,_tmp82C,({const char*_tmpB5="_";_tag_fat(_tmpB5,sizeof(char),2U);}));}));_tmp79D.f1=_tmp82E;});_tmp79D;});struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmp79C;_tmp79C.tag=0U,_tmp79C.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp79C;});void*_tmpB0[3U];_tmpB0[0]=& _tmpB2,_tmpB0[1]=& _tmpB3,_tmpB0[2]=& _tmpB4;({struct _fat_ptr _tmp82F=({const char*_tmpB1="%s_%s_%s_struct";_tag_fat(_tmpB1,sizeof(char),16U);});Cyc_aprintf(_tmp82F,_tag_fat(_tmpB0,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB6;};_pop_region();}}}_LL3:;}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 489
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 492
struct _tuple20 _tmpB7=*env;struct _tuple20 _stmttmp7=_tmpB7;struct _tuple20 _tmpB8=_stmttmp7;struct _tuple14*_tmpBA;struct Cyc_Dict_Dict*_tmpB9;_LL1: _tmpB9=(_tmpB8.f1)->qvar_tags;_tmpBA=_tmpB8.f2;_LL2: {struct Cyc_Dict_Dict*qvs=_tmpB9;struct _tuple14*pair=_tmpBA;
struct _tuple14 _tmpBB=*pair;struct _tuple14 _stmttmp8=_tmpBB;struct _tuple14 _tmpBC=_stmttmp8;struct _tuple1*_tmpBE;struct _tuple1*_tmpBD;_LL4: _tmpBD=_tmpBC.f1;_tmpBE=_tmpBC.f2;_LL5: {struct _tuple1*fieldname=_tmpBD;struct _tuple1*dtname=_tmpBE;
struct _handler_cons _tmpBF;_push_handler(& _tmpBF);{int _tmpC1=0;if(setjmp(_tmpBF.handler))_tmpC1=1;if(!_tmpC1){{struct _tuple1*_tmpC2=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0U);return _tmpC2;};_pop_handler();}else{void*_tmpC0=(void*)Cyc_Core_get_exn_thrown();void*_tmpC3=_tmpC0;void*_tmpC4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC3)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 496
struct _tuple14*_tmpC5=({struct _tuple14*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->f1=fieldname,_tmpCB->f2=dtname;_tmpCB;});struct _tuple14*new_pair=_tmpC5;
struct _tuple1*_tmpC6=fieldname;struct _fat_ptr*_tmpC8;union Cyc_Absyn_Nmspace _tmpC7;_LLC: _tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f2;_LLD: {union Cyc_Absyn_Nmspace nmspace=_tmpC7;struct _fat_ptr*fieldvar=_tmpC8;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->f1=nmspace,({struct _fat_ptr*_tmp830=({struct _fat_ptr*_tmpC9=_cycalloc(sizeof(*_tmpC9));*_tmpC9=newvar;_tmpC9;});_tmpCA->f2=_tmp830;});_tmpCA;});
({struct Cyc_Dict_Dict _tmp831=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp831;});
return res;}}}else{_LL9: _tmpC4=_tmpC3;_LLA: {void*exn=_tmpC4;(int)_rethrow(exn);}}_LL6:;}}}}}
# 504
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp79F;_tmp79F.f1=fieldname,_tmp79F.f2=dtname;_tmp79F;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 511
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->kind=Cyc_Absyn_StructA,_tmpCE->sc=Cyc_Absyn_Public,_tmpCE->tvs=0,_tmpCE->attributes=0,_tmpCE->expected_mem_kind=0,({
# 514
struct _tuple1*_tmp834=({struct _tuple1*_tmpCC=_cycalloc(sizeof(*_tmpCC));({union Cyc_Absyn_Nmspace _tmp833=Cyc_Absyn_Rel_n(0);_tmpCC->f1=_tmp833;}),_tmpCC->f2=name;_tmpCC;});_tmpCE->name=_tmp834;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp832=({struct Cyc_Absyn_AggrdeclImpl*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->exist_vars=0,_tmpCD->rgn_po=0,_tmpCD->fields=fs,_tmpCD->tagged=0;_tmpCD;});_tmpCE->impl=_tmp832;});_tmpCE;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 520
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 523
struct _tuple21 _tmpCF=*env;struct _tuple21 _stmttmp9=_tmpCF;struct _tuple21 _tmpD0=_stmttmp9;struct Cyc_List_List*_tmpD2;struct Cyc_List_List**_tmpD1;_LL1: _tmpD1=(_tmpD0.f1)->tuple_types;_tmpD2=_tmpD0.f2;_LL2: {struct Cyc_List_List**tuple_types=_tmpD1;struct Cyc_List_List*tqs0=_tmpD2;
# 525
{struct Cyc_List_List*_tmpD3=*tuple_types;struct Cyc_List_List*tts=_tmpD3;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_tmpD4=(struct _tuple22*)tts->hd;struct _tuple22*_stmttmpA=_tmpD4;struct _tuple22*_tmpD5=_stmttmpA;struct Cyc_List_List*_tmpD7;void*_tmpD6;_LL4: _tmpD6=_tmpD5->f1;_tmpD7=_tmpD5->f2;_LL5: {void*x=_tmpD6;struct Cyc_List_List*ts=_tmpD7;
struct Cyc_List_List*_tmpD8=tqs0;struct Cyc_List_List*tqs=_tmpD8;
for(0;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 536
struct _fat_ptr*xname=({struct _fat_ptr*_tmpE8=_cycalloc(sizeof(*_tmpE8));({struct _fat_ptr _tmp836=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE7=({struct Cyc_Int_pa_PrintArg_struct _tmp7A0;_tmp7A0.tag=1U,_tmp7A0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A0;});void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({struct _fat_ptr _tmp835=({const char*_tmpE6="_tuple%d";_tag_fat(_tmpE6,sizeof(char),9U);});Cyc_aprintf(_tmp835,_tag_fat(_tmpE5,sizeof(void*),1U));});});*_tmpE8=_tmp836;});_tmpE8;});
struct Cyc_List_List*_tmpD9=0;struct Cyc_List_List*fs=_tmpD9;
struct Cyc_List_List*_tmpDA=0;struct Cyc_List_List*ts=_tmpDA;
{int i=1;for(0;tqs0 != 0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f2;
fs=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Absyn_Aggrfield*_tmp838=({struct Cyc_Absyn_Aggrfield*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct _fat_ptr*_tmp837=Cyc_Absyn_fieldname(i);_tmpDB->name=_tmp837;}),_tmpDB->tq=Cyc_Toc_mt_tq,_tmpDB->type=t,_tmpDB->width=0,_tmpDB->attributes=0,_tmpDB->requires_clause=0;_tmpDB;});_tmpDC->hd=_tmp838;}),_tmpDC->tl=fs;_tmpDC;});
ts=({struct Cyc_List_List*_tmpDD=_region_malloc(d,sizeof(*_tmpDD));_tmpDD->hd=t,_tmpDD->tl=ts;_tmpDD;});}}
# 544
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);
ts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts);{
struct Cyc_Absyn_Aggrdecl*_tmpDE=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpDE;
void*_tmpDF=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE4=_cycalloc(sizeof(*_tmpE4));*_tmpE4=sd;_tmpE4;})),0);void*ans=_tmpDF;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));({struct Cyc_Absyn_Decl*_tmp839=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->tag=5U,_tmpE0->f1=sd;_tmpE0;}),0U);_tmpE1->hd=_tmp839;}),_tmpE1->tl=Cyc_Toc_result_decls;_tmpE1;});
({struct Cyc_List_List*_tmp83B=({struct Cyc_List_List*_tmpE3=_region_malloc(d,sizeof(*_tmpE3));({struct _tuple22*_tmp83A=({struct _tuple22*_tmpE2=_region_malloc(d,sizeof(*_tmpE2));_tmpE2->f1=ans,_tmpE2->f2=ts;_tmpE2;});_tmpE3->hd=_tmp83A;}),_tmpE3->tl=*tuple_types;_tmpE3;});*tuple_types=_tmp83B;});
return ans;}}}}
# 552
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 557
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 560
struct _tuple25*_tmpE9=env;struct Cyc_List_List*_tmpEC;enum Cyc_Absyn_AggrKind _tmpEB;struct Cyc_List_List**_tmpEA;_LL1: _tmpEA=(_tmpE9->f1)->anon_aggr_types;_tmpEB=(_tmpE9->f2)->f1;_tmpEC=(_tmpE9->f2)->f2;_LL2: {struct Cyc_List_List**anon_aggr_types=_tmpEA;enum Cyc_Absyn_AggrKind ak=_tmpEB;struct Cyc_List_List*fs=_tmpEC;
# 562
{struct Cyc_List_List*_tmpED=*anon_aggr_types;struct Cyc_List_List*ts=_tmpED;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpEE=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpEE;struct _tuple26*_tmpEF=_stmttmpB;struct Cyc_List_List*_tmpF2;enum Cyc_Absyn_AggrKind _tmpF1;void*_tmpF0;_LL4: _tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;_tmpF2=_tmpEF->f3;_LL5: {void*x=_tmpF0;enum Cyc_Absyn_AggrKind ak2=_tmpF1;struct Cyc_List_List*fs2=_tmpF2;
if((int)ak != (int)ak2)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 571
struct _fat_ptr*xname=({struct _fat_ptr*_tmpFD=_cycalloc(sizeof(*_tmpFD));({struct _fat_ptr _tmp83D=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFC=({struct Cyc_Int_pa_PrintArg_struct _tmp7A1;_tmp7A1.tag=1U,_tmp7A1.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A1;});void*_tmpFA[1U];_tmpFA[0]=& _tmpFC;({struct _fat_ptr _tmp83C=({const char*_tmpFB="_tuple%d";_tag_fat(_tmpFB,sizeof(char),9U);});Cyc_aprintf(_tmp83C,_tag_fat(_tmpFA,sizeof(void*),1U));});});*_tmpFD=_tmp83D;});_tmpFD;});
struct Cyc_Absyn_Aggrdecl*_tmpF3=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpF3;
sd->kind=ak;{
void*_tmpF4=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF9=_cycalloc(sizeof(*_tmpF9));*_tmpF9=sd;_tmpF9;})),0);void*ans=_tmpF4;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct Cyc_Absyn_Decl*_tmp83E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=5U,_tmpF5->f1=sd;_tmpF5;}),0U);_tmpF6->hd=_tmp83E;}),_tmpF6->tl=Cyc_Toc_result_decls;_tmpF6;});
({struct Cyc_List_List*_tmp840=({struct Cyc_List_List*_tmpF8=_region_malloc(d,sizeof(*_tmpF8));({struct _tuple26*_tmp83F=({struct _tuple26*_tmpF7=_region_malloc(d,sizeof(*_tmpF7));_tmpF7->f1=ans,_tmpF7->f2=ak,_tmpF7->f3=fs;_tmpF7;});_tmpF8->hd=_tmp83F;}),_tmpF8->tl=*anon_aggr_types;_tmpF8;});*anon_aggr_types=_tmp840;});
return ans;}}}}
# 579
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->f1=ak,_tmpFE->f2=fs;_tmpFE;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 588
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 597
struct _tuple28 _tmpFF=*env;struct _tuple28 _stmttmpC=_tmpFF;struct _tuple28 _tmp100=_stmttmpC;struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp103;struct _tuple1*_tmp102;struct Cyc_List_List**_tmp101;_LL1: _tmp101=(_tmp100.f1)->abs_struct_types;_tmp102=(_tmp100.f2)->f1;_tmp103=(_tmp100.f2)->f2;_tmp104=(_tmp100.f2)->f3;_tmp105=(_tmp100.f2)->f4;_LL2: {struct Cyc_List_List**abs_struct_types=_tmp101;struct _tuple1*struct_name=_tmp102;struct Cyc_List_List*type_vars=_tmp103;struct Cyc_List_List*type_args=_tmp104;struct Cyc_List_List*fields=_tmp105;
# 601
{struct Cyc_List_List*_tmp106=*abs_struct_types;struct Cyc_List_List*tts=_tmp106;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmp107=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmp107;struct _tuple29*_tmp108=_stmttmpD;void*_tmp10B;struct Cyc_List_List*_tmp10A;struct _tuple1*_tmp109;_LL4: _tmp109=_tmp108->f1;_tmp10A=_tmp108->f2;_tmp10B=_tmp108->f3;_LL5: {struct _tuple1*x=_tmp109;struct Cyc_List_List*ts2=_tmp10A;void*t=_tmp10B;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp841=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args);_tmp841 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmp10C=type_args;struct Cyc_List_List*ts=_tmp10C;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmp10D=(void*)ts->hd;void*t=_tmp10D;
void*_tmp10E=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmp10E;
{struct Cyc_Absyn_Kind*_tmp10F=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp10F;struct Cyc_Absyn_Kind*_tmp110=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp110)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 614
 if(Cyc_Unify_unify(t,t2)||({void*_tmp842=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp842,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 619
break;}}
# 621
if(okay)
return t;}}}}{
# 628
struct _fat_ptr*xname=({struct _fat_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _fat_ptr _tmp844=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122=({struct Cyc_Int_pa_PrintArg_struct _tmp7A2;_tmp7A2.tag=1U,_tmp7A2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A2;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _fat_ptr _tmp843=({const char*_tmp121="_tuple%d";_tag_fat(_tmp121,sizeof(char),9U);});Cyc_aprintf(_tmp843,_tag_fat(_tmp120,sizeof(void*),1U));});});*_tmp123=_tmp844;});_tmp123;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp111=0;struct Cyc_List_List*fs=_tmp111;
# 632
({struct Cyc_List_List*_tmp846=({struct Cyc_List_List*_tmp113=_region_malloc(d,sizeof(*_tmp113));({struct _tuple29*_tmp845=({struct _tuple29*_tmp112=_region_malloc(d,sizeof(*_tmp112));_tmp112->f1=struct_name,_tmp112->f2=type_args,_tmp112->f3=x;_tmp112;});_tmp113->hd=_tmp845;}),_tmp113->tl=*abs_struct_types;_tmp113;});*abs_struct_types=_tmp846;});{
# 635
struct _RegionHandle _tmp114=_new_region("r");struct _RegionHandle*r=& _tmp114;_push_region(r);
{struct Cyc_List_List*_tmp115=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp115;
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp116=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp116;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 645
if((fields->tl == 0 &&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));({void*_tmp847=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->tag=6U,_tmp117->f1=0;_tmp117;});_tmp118->hd=_tmp847;}),_tmp118->tl=atts;_tmp118;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 652
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=4U,({void*_tmp84A=Cyc_Toc_void_star_type();(_tmp119->f1).elt_type=_tmp84A;}),({struct Cyc_Absyn_Tqual _tmp849=Cyc_Absyn_empty_tqual(0U);(_tmp119->f1).tq=_tmp849;}),({
struct Cyc_Absyn_Exp*_tmp848=Cyc_Absyn_uint_exp(0U,0U);(_tmp119->f1).num_elts=_tmp848;}),(_tmp119->f1).zero_term=Cyc_Absyn_false_type,(_tmp119->f1).zt_loc=0U;_tmp119;});
# 656
fs=({struct Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_Absyn_Aggrfield*_tmp84B=({struct Cyc_Absyn_Aggrfield*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->name=f->name,_tmp11A->tq=Cyc_Toc_mt_tq,_tmp11A->type=t,_tmp11A->width=f->width,_tmp11A->attributes=atts,_tmp11A->requires_clause=0;_tmp11A;});_tmp11B->hd=_tmp84B;}),_tmp11B->tl=fs;_tmp11B;});}
# 658
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp11C=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp11C;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));({struct Cyc_Absyn_Decl*_tmp84C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=5U,_tmp11D->f1=sd;_tmp11D;}),0U);_tmp11E->hd=_tmp84C;}),_tmp11E->tl=Cyc_Toc_result_decls;_tmp11E;});{
void*_tmp11F=x;_npop_handler(0U);return _tmp11F;}}}
# 636
;_pop_region();}}}}
# 664
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 668
struct _tuple27 env=({struct _tuple27 _tmp7A3;_tmp7A3.f1=struct_name,_tmp7A3.f2=type_vars,_tmp7A3.f3=type_args,_tmp7A3.f4=fields;_tmp7A3;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 676
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp84F=({struct _fat_ptr*_tmp127=_cycalloc(sizeof(*_tmp127));({struct _fat_ptr _tmp84E=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp126=({struct Cyc_Int_pa_PrintArg_struct _tmp7A4;_tmp7A4.tag=1U,_tmp7A4.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp7A4;});void*_tmp124[1U];_tmp124[0]=& _tmp126;({struct _fat_ptr _tmp84D=({const char*_tmp125="_tmp%X";_tag_fat(_tmp125,sizeof(char),7U);});Cyc_aprintf(_tmp84D,_tag_fat(_tmp124,sizeof(void*),1U));});});*_tmp127=_tmp84E;});_tmp127;});_tmp128->f2=_tmp84F;});_tmp128;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 679
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp7A5;_tmp7A5.f1=v,({struct Cyc_Absyn_Exp*_tmp850=Cyc_Absyn_var_exp(v,0U);_tmp7A5.f2=_tmp850;});_tmp7A5;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 686
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp129=*env;struct _tuple31 _stmttmpF=_tmp129;struct _tuple31 _tmp12A=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp12B;_LL1: _tmp12B=(_tmp12A.f1)->temp_labels;_LL2: {struct Cyc_Xarray_Xarray*temp_labels=_tmp12B;
int _tmp12C=Cyc_Toc_fresh_label_counter ++;int i=_tmp12C;
if(({int _tmp851=i;_tmp851 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp133=_cycalloc(sizeof(*_tmp133));({struct _fat_ptr _tmp853=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp132=({struct Cyc_Int_pa_PrintArg_struct _tmp7A7;_tmp7A7.tag=1U,_tmp7A7.f1=(unsigned)i;_tmp7A7;});void*_tmp130[1U];_tmp130[0]=& _tmp132;({struct _fat_ptr _tmp852=({const char*_tmp131="_LL%X";_tag_fat(_tmp131,sizeof(char),6U);});Cyc_aprintf(_tmp852,_tag_fat(_tmp130,sizeof(void*),1U));});});*_tmp133=_tmp853;});_tmp133;});
if(({int _tmp854=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp854 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A6;_tmp7A6.tag=0U,({struct _fat_ptr _tmp855=({const char*_tmp12F="fresh_label: add_ind returned bad index...";_tag_fat(_tmp12F,sizeof(char),43U);});_tmp7A6.f1=_tmp855;});_tmp7A6;});void*_tmp12D[1U];_tmp12D[0]=& _tmp12E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp12D,sizeof(void*),1U));});
return res;}}}
# 696
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 702
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp134=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp134;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 709
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 715
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->f1=(*a).f1,_tmp135->f2=(*a).f2,({void*_tmp856=Cyc_Toc_typ_to_c((*a).f3);_tmp135->f3=_tmp856;});_tmp135;});}
# 737 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp136=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp136;void*_tmp137=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp138;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->tag == 4U){_LL1: _tmp138=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp137)->f1;_LL2: {struct Cyc_Absyn_ArrayInfo ai=_tmp138;
return({void*_tmp857=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp857,ai.tq);});}}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 744
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 746
struct Cyc_Absyn_Aggrfield*_tmp139=({struct Cyc_Absyn_Aggrfield*_tmp13A=_cycalloc(sizeof(*_tmp13A));*_tmp13A=*f;_tmp13A;});struct Cyc_Absyn_Aggrfield*ans=_tmp139;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 753
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 755
return;}
# 758
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp13B=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp13B;void*_tmp13C=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp13D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13C)->tag == 2U){_LL1: _tmp13D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13C)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp13D;
return Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tbk);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 764
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp13E=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp13E;struct Cyc_Absyn_Kind*_tmp13F=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp13F)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 770
static int Cyc_Toc_is_void_star(void*t){
void*_tmp140=Cyc_Absyn_compress(t);void*_stmttmp13=_tmp140;void*_tmp141=_stmttmp13;void*_tmp142;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp141)->tag == 3U){_LL1: _tmp142=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp141)->f1).elt_type;_LL2: {void*t2=_tmp142;
return Cyc_Tcutil_is_void_type(t2);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 776
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 779
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 783
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e);
# 785
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp143=t;void**_tmp145;struct Cyc_Absyn_Datatypedecl*_tmp144;struct Cyc_Absyn_Enumdecl*_tmp146;struct Cyc_Absyn_Aggrdecl*_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp149;void*_tmp14D;struct Cyc_Absyn_Typedefdecl*_tmp14C;struct Cyc_List_List*_tmp14B;struct _tuple1*_tmp14A;struct Cyc_List_List*_tmp14F;enum Cyc_Absyn_AggrKind _tmp14E;struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_VarargInfo*_tmp155;int _tmp154;struct Cyc_List_List*_tmp153;void*_tmp152;struct Cyc_Absyn_Tqual _tmp151;unsigned _tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Tqual _tmp158;void*_tmp157;void*_tmp15D;struct Cyc_Absyn_Tqual _tmp15C;void*_tmp15B;struct Cyc_Absyn_Tvar*_tmp15E;void**_tmp15F;struct Cyc_List_List*_tmp160;struct _tuple1*_tmp161;struct Cyc_List_List*_tmp164;union Cyc_Absyn_AggrInfo _tmp163;void*_tmp162;struct Cyc_Absyn_Datatypefield*_tmp166;struct Cyc_Absyn_Datatypedecl*_tmp165;switch(*((int*)_tmp143)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 798
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp165=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)->f1).KnownDatatypefield).val).f1;_tmp166=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)->f1).KnownDatatypefield).val).f2;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmp165;struct Cyc_Absyn_Datatypefield*tuf=_tmp166;
# 800
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{_LLB: _LLC:
# 802
({struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A8;_tmp7A8.tag=0U,({struct _fat_ptr _tmp858=({const char*_tmp169="unresolved DatatypeFieldType";_tag_fat(_tmp169,sizeof(char),29U);});_tmp7A8.f1=_tmp858;});_tmp7A8;});void*_tmp167[1U];_tmp167[0]=& _tmp168;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp167,sizeof(void*),1U));});}case 1U: _LLF: _LL10:
# 811
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp162=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1;_tmp163=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)->f1;_tmp164=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f2;_LL1C: {void*c=_tmp162;union Cyc_Absyn_AggrInfo info=_tmp163;struct Cyc_List_List*ts=_tmp164;
# 846
{union Cyc_Absyn_AggrInfo _tmp176=info;if((_tmp176.UnknownAggr).tag == 1){_LL36: _LL37:
# 848
 if(ts == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->tag=0U,_tmp177->f1=c,_tmp177->f2=0;_tmp177;});}}else{_LL38: _LL39:
 goto _LL35;}_LL35:;}{
# 852
struct Cyc_Absyn_Aggrdecl*_tmp178=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp178;
if(ad->expected_mem_kind){
# 855
if(ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7AA;_tmp7AA.tag=6U,_tmp7AA.f1=ad;_tmp7AA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A9;_tmp7A9.tag=0U,({struct _fat_ptr _tmp859=({const char*_tmp17C=" was never defined.";_tag_fat(_tmp17C,sizeof(char),20U);});_tmp7A9.f1=_tmp859;});_tmp7A9;});void*_tmp179[2U];_tmp179[0]=& _tmp17A,_tmp179[1]=& _tmp17B;Cyc_Warn_warn2(0U,_tag_fat(_tmp179,sizeof(void*),2U));});}
# 859
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp17D=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp17D;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp17E=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp17E;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp180=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7AC;_tmp7AC.tag=6U,_tmp7AC.f1=ad;_tmp7AC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp181=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AB;_tmp7AB.tag=0U,({struct _fat_ptr _tmp85A=({const char*_tmp182=" ended up being abstract.";_tag_fat(_tmp182,sizeof(char),26U);});_tmp7AB.f1=_tmp85A;});_tmp7AB;});void*_tmp17F[2U];_tmp17F[0]=& _tmp180,_tmp17F[1]=& _tmp181;Cyc_Warn_warn2(0U,_tag_fat(_tmp17F,sizeof(void*),2U));});{
# 871
struct _RegionHandle _tmp183=_new_region("r");struct _RegionHandle*r=& _tmp183;_push_region(r);
{struct Cyc_List_List*_tmp184=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp184;
void*_tmp185=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp185;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp186=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp186;}{
void*_tmp187=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0U);return _tmp187;}}
# 872
;_pop_region();}}
# 877
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15U: _LL1D: _tmp161=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)->f1;_LL1E: {struct _tuple1*tdn=_tmp161;
return t;}case 16U: _LL1F: _tmp160=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp143)->f1)->f1;_LL20: {struct Cyc_List_List*fs=_tmp160;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4U: _LL23: _LL24:
# 889
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 894
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp15F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143)->f2;_LL4: {void**t2=_tmp15F;
# 789
if(*t2 == 0){
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 793
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));}case 2U: _LL5: _tmp15E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp143)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp15E;
# 795
if((int)(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;
return Cyc_Toc_void_star_type();}case 3U: _LLD: _tmp15B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143)->f1).elt_type;_tmp15C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143)->f1).elt_tq;_tmp15D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143)->f1).ptr_atts).bounds;_LLE: {void*t2=_tmp15B;struct Cyc_Absyn_Tqual tq=_tmp15C;void*bnds=_tmp15D;
# 806
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 810
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}}case 4U: _LL13: _tmp157=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp143)->f1).elt_type;_tmp158=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp143)->f1).tq;_tmp159=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp143)->f1).num_elts;_tmp15A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp143)->f1).zt_loc;_LL14: {void*t2=_tmp157;struct Cyc_Absyn_Tqual tq=_tmp158;struct Cyc_Absyn_Exp*e=_tmp159;unsigned ztl=_tmp15A;
# 814
return({void*_tmp85E=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp85D=tq;struct Cyc_Absyn_Exp*_tmp85C=e;void*_tmp85B=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp85E,_tmp85D,_tmp85C,_tmp85B,ztl);});}case 5U: _LL15: _tmp151=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143)->f1).ret_tqual;_tmp152=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143)->f1).ret_type;_tmp153=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143)->f1).args;_tmp154=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143)->f1).c_varargs;_tmp155=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143)->f1).cyc_varargs;_tmp156=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143)->f1).attributes;_LL16: {struct Cyc_Absyn_Tqual tq2=_tmp151;void*t2=_tmp152;struct Cyc_List_List*args=_tmp153;int c_vararg=_tmp154;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp155;struct Cyc_List_List*atts=_tmp156;
# 818
struct Cyc_List_List*_tmp16A=Cyc_Atts_atts2c(atts);struct Cyc_List_List*new_atts=_tmp16A;
struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp16B;
if(cyc_vararg != 0){
# 822
void*_tmp16C=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));void*t=_tmp16C;
struct _tuple9*_tmp16D=({struct _tuple9*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=cyc_vararg->name,_tmp16F->f2=cyc_vararg->tq,_tmp16F->f3=t;_tmp16F;});struct _tuple9*vararg=_tmp16D;
new_args=({struct Cyc_List_List*_tmp85F=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp85F,({struct Cyc_List_List*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->hd=vararg,_tmp16E->tl=0;_tmp16E;}));});}
# 826
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=5U,(_tmp170->f1).tvars=0,(_tmp170->f1).effect=0,(_tmp170->f1).ret_tqual=tq2,({void*_tmp860=Cyc_Toc_typ_to_c(t2);(_tmp170->f1).ret_type=_tmp860;}),(_tmp170->f1).args=new_args,(_tmp170->f1).c_varargs=c_vararg,(_tmp170->f1).cyc_varargs=0,(_tmp170->f1).rgn_po=0,(_tmp170->f1).attributes=new_atts,(_tmp170->f1).requires_clause=0,(_tmp170->f1).requires_relns=0,(_tmp170->f1).ensures_clause=0,(_tmp170->f1).ensures_relns=0,(_tmp170->f1).return_value=0;_tmp170;});}case 6U: _LL17: _tmp150=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp143)->f1;_LL18: {struct Cyc_List_List*tqs=_tmp150;
# 831
struct Cyc_List_List*_tmp171=0;struct Cyc_List_List*tqs2=_tmp171;
for(0;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp173=_cycalloc(sizeof(*_tmp173));({struct _tuple23*_tmp862=({struct _tuple23*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=(*((struct _tuple23*)tqs->hd)).f1,({void*_tmp861=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqs->hd)).f2);_tmp172->f2=_tmp861;});_tmp172;});_tmp173->hd=_tmp862;}),_tmp173->tl=tqs2;_tmp173;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tqs2));}case 7U: _LL19: _tmp14E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f1;_tmp14F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp143)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp14E;struct Cyc_List_List*fs=_tmp14F;
# 840
struct Cyc_List_List*_tmp174=0;struct Cyc_List_List*fs2=_tmp174;
for(0;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp175=_cycalloc(sizeof(*_tmp175));({struct Cyc_Absyn_Aggrfield*_tmp864=({struct Cyc_Absyn_Aggrfield*_tmp863=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp863,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp175->hd=_tmp864;}),_tmp175->tl=fs2;_tmp175;});}
return({enum Cyc_Absyn_AggrKind _tmp865=k;Cyc_Toc_add_anon_aggr_type(_tmp865,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs2));});}case 8U: _LL21: _tmp14A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f1;_tmp14B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f2;_tmp14C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f3;_tmp14D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp143)->f4;_LL22: {struct _tuple1*tdn=_tmp14A;struct Cyc_List_List*ts=_tmp14B;struct Cyc_Absyn_Typedefdecl*td=_tmp14C;void*topt=_tmp14D;
# 883
if(topt == 0){
if(ts != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->tag=8U,_tmp188->f1=tdn,_tmp188->f2=0,_tmp188->f3=td,_tmp188->f4=0;_tmp188;});else{
return t;}}else{
# 888
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->tag=8U,_tmp189->f1=tdn,_tmp189->f2=0,_tmp189->f3=td,({void*_tmp866=Cyc_Toc_typ_to_c(topt);_tmp189->f4=_tmp866;});_tmp189;});}}case 9U: _LL2B: _tmp149=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp143)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp149;
# 899
return t;}case 11U: _LL2D: _tmp148=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp143)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp148;
# 904
Cyc_Toc_exptypes_to_c(e);
if(e->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)){case 0U: _LL2F: _tmp147=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL30: {struct Cyc_Absyn_Aggrdecl*ad=_tmp147;
# 909
Cyc_Toc_aggrdecl_to_c(ad);
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp146=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_LL32: {struct Cyc_Absyn_Enumdecl*ed=_tmp146;
# 914
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _LL33: _tmp144=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f1)->r)->f1;_tmp145=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp143)->f2;_LL34: {struct Cyc_Absyn_Datatypedecl*dd=_tmp144;void**t=_tmp145;
# 917
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}}_LL0:;}
# 922
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp18A=t;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18A)->tag == 4U){_LL1: _tmp18B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18A)->f1).elt_type;_tmp18C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18A)->f1).tq;_LL2: {void*t2=_tmp18B;struct Cyc_Absyn_Tqual tq=_tmp18C;
# 925
return({void*_tmp867=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp867,e);});}}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 932
static int Cyc_Toc_atomic_type(void*t){
void*_tmp18D=Cyc_Absyn_compress(t);void*_stmttmp14=_tmp18D;void*_tmp18E=_stmttmp14;struct Cyc_List_List*_tmp18F;struct Cyc_List_List*_tmp190;void*_tmp191;struct Cyc_List_List*_tmp193;void*_tmp192;switch(*((int*)_tmp18E)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp192=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18E)->f1;_tmp193=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18E)->f2;_LL4: {void*c=_tmp192;struct Cyc_List_List*ts=_tmp193;
# 936
void*_tmp194=c;struct Cyc_Absyn_Datatypefield*_tmp196;struct Cyc_Absyn_Datatypedecl*_tmp195;union Cyc_Absyn_AggrInfo _tmp197;switch(*((int*)_tmp194)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp197=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp194)->f1;_LL25: {union Cyc_Absyn_AggrInfo info=_tmp197;
# 943
{union Cyc_Absyn_AggrInfo _tmp198=info;if((_tmp198.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 947
struct Cyc_Absyn_Aggrdecl*_tmp199=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp199;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp19A;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp194)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp195=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp194)->f1).KnownDatatypefield).val).f1;_tmp196=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp194)->f1).KnownDatatypefield).val).f2;_LL27: {struct Cyc_Absyn_Datatypedecl*tud=_tmp195;struct Cyc_Absyn_Datatypefield*tuf=_tmp196;
# 954
{struct Cyc_List_List*_tmp19B=tuf->typs;struct Cyc_List_List*tqs=_tmp19B;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AE;_tmp7AE.tag=0U,({struct _fat_ptr _tmp868=({const char*_tmp19F="atomic_typ: bad type ";_tag_fat(_tmp19F,sizeof(char),22U);});_tmp7AE.f1=_tmp868;});_tmp7AE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AD;_tmp7AD.tag=2U,_tmp7AD.f1=(void*)t;_tmp7AD;});void*_tmp19C[2U];_tmp19C[0]=& _tmp19D,_tmp19C[1]=& _tmp19E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp19C,sizeof(void*),2U));});}_LL11:;}case 5U: _LL5: _LL6:
# 959
 return 1;case 4U: _LL7: _tmp191=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).elt_type;_LL8: {void*t=_tmp191;
return Cyc_Toc_atomic_type(t);}case 7U: _LL9: _tmp190=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18E)->f2;_LLA: {struct Cyc_List_List*fs=_tmp190;
# 967
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6U: _LLB: _tmp18F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18E)->f1;_LLC: {struct Cyc_List_List*tqs=_tmp18F;
# 971
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}
return 1;}case 3U: _LLD: _LLE:
# 976
 return 0;default: _LLF: _LL10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B0;_tmp7B0.tag=0U,({struct _fat_ptr _tmp869=({const char*_tmp1A3="atomic_typ:  bad type ";_tag_fat(_tmp1A3,sizeof(char),23U);});_tmp7B0.f1=_tmp869;});_tmp7B0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AF;_tmp7AF.tag=2U,_tmp7AF.f1=(void*)t;_tmp7AF;});void*_tmp1A0[2U];_tmp1A0[0]=& _tmp1A1,_tmp1A0[1]=& _tmp1A2;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A0,sizeof(void*),2U));});}_LL0:;}
# 982
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp1A4=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp1A4;void*_tmp1A5=_stmttmp15;struct Cyc_List_List*_tmp1A6;union Cyc_Absyn_AggrInfo _tmp1A7;switch(*((int*)_tmp1A5)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f1)->tag == 20U){_LL1: _tmp1A7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp1A7;
# 985
struct Cyc_Absyn_Aggrdecl*_tmp1A8=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A8;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B1;_tmp7B1.tag=0U,({struct _fat_ptr _tmp86A=({const char*_tmp1AB="is_poly_field: type missing fields";_tag_fat(_tmp1AB,sizeof(char),35U);});_tmp7B1.f1=_tmp86A;});_tmp7B1;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A9,sizeof(void*),1U));});
_tmp1A6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A6;
# 990
struct Cyc_Absyn_Aggrfield*_tmp1AC=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp1AC;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B3;_tmp7B3.tag=0U,({struct _fat_ptr _tmp86B=({const char*_tmp1B0="is_poly_field: bad field ";_tag_fat(_tmp1B0,sizeof(char),26U);});_tmp7B3.f1=_tmp86B;});_tmp7B3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B2;_tmp7B2.tag=0U,_tmp7B2.f1=*f;_tmp7B2;});void*_tmp1AD[2U];_tmp1AD[0]=& _tmp1AE,_tmp1AD[1]=& _tmp1AF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AD,sizeof(void*),2U));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B5;_tmp7B5.tag=0U,({struct _fat_ptr _tmp86C=({const char*_tmp1B4="is_poly_field: bad type ";_tag_fat(_tmp1B4,sizeof(char),25U);});_tmp7B5.f1=_tmp86C;});_tmp7B5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1B3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B4;_tmp7B4.tag=2U,_tmp7B4.f1=(void*)t;_tmp7B4;});void*_tmp1B1[2U];_tmp1B1[0]=& _tmp1B2,_tmp1B1[1]=& _tmp1B3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1B1,sizeof(void*),2U));});}_LL0:;}
# 1001
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B5=e->r;void*_stmttmp16=_tmp1B5;void*_tmp1B6=_stmttmp16;struct _fat_ptr*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct _fat_ptr*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1B9;switch(*((int*)_tmp1B6)){case 21U: _LL1: _tmp1B9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B6)->f1;_tmp1BA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B6)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp1B9;struct _fat_ptr*f=_tmp1BA;
# 1004
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22U: _LL3: _tmp1B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B6)->f1;_tmp1B8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B6)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1B7;struct _fat_ptr*f=_tmp1B8;
# 1007
void*_tmp1BB=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp17=_tmp1BB;void*_tmp1BC=_stmttmp17;void*_tmp1BD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->tag == 3U){_LL8: _tmp1BD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->f1).elt_type;_LL9: {void*t=_tmp1BD;
# 1009
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B7;_tmp7B7.tag=0U,({struct _fat_ptr _tmp86D=({const char*_tmp1C1="is_poly_project: bad type ";_tag_fat(_tmp1C1,sizeof(char),27U);});_tmp7B7.f1=_tmp86D;});_tmp7B7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1C0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B6;_tmp7B6.tag=2U,_tmp7B6.f1=(void*)_check_null(e1->topt);_tmp7B6;});void*_tmp1BE[2U];_tmp1BE[0]=& _tmp1BF,_tmp1BE[1]=& _tmp1C0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1BE,sizeof(void*),2U));});}_LL7:;}default: _LL5: _LL6:
# 1012
 return 0;}_LL0:;}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C2[1U];_tmp1C2[0]=s;({struct Cyc_Absyn_Exp*_tmp86E=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86E,_tag_fat(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1022
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C3[2U];_tmp1C3[0]=s,_tmp1C3[1]=n;({struct Cyc_Absyn_Exp*_tmp86F=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86F,_tag_fat(_tmp1C3,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C4[2U];_tmp1C4[0]=rgn,_tmp1C4[1]=s;({struct Cyc_Absyn_Exp*_tmp870=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp870,_tag_fat(_tmp1C4,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1031
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;({struct Cyc_Absyn_Exp*_tmp871=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp871,_tag_fat(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1035
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C6[3U];_tmp1C6[0]=rgn,_tmp1C6[1]=s,_tmp1C6[2]=n;({struct Cyc_Absyn_Exp*_tmp872=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp872,_tag_fat(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1039
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(({void*_tmp1C7=0U;({struct Cyc_Absyn_Exp*_tmp873=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp873,_tag_fat(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1044
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1052
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C8=e->r;void*_stmttmp18=_tmp1C8;void*_tmp1C9=_stmttmp18;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C9)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C9)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1057
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));({struct Cyc_Absyn_Decl*_tmp874=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->tag=0U,_tmp1CA->f1=vd;_tmp1CA;}),0U);_tmp1CB->hd=_tmp874;}),_tmp1CB->tl=Cyc_Toc_result_decls;_tmp1CB;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1065
 xexp=({void*_tmp875=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp875,e);});
# 1067
xplussz=({void*_tmp876=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp876,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1070
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1CC[3U];({struct _tuple19*_tmp879=({struct _tuple19*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=0,_tmp1CD->f2=xexp;_tmp1CD;});_tmp1CC[0]=_tmp879;}),({
struct _tuple19*_tmp878=({struct _tuple19*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->f1=0,_tmp1CE->f2=xexp;_tmp1CE;});_tmp1CC[1]=_tmp878;}),({
struct _tuple19*_tmp877=({struct _tuple19*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->f1=0,_tmp1CF->f2=xplussz;_tmp1CF;});_tmp1CC[2]=_tmp877;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CC,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1105 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D1=_region_malloc(r,sizeof(*_tmp1D1));_tmp1D1->break_lab=0,_tmp1D1->continue_lab=0,_tmp1D1->fallthru_info=0,_tmp1D1->toplevel=1,({int*_tmp87A=({int*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));*_tmp1D0=0;_tmp1D0;});_tmp1D1->in_lhs=_tmp87A;}),_tmp1D1->rgn=r;_tmp1D1;});}
# 1109
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D2=nv;int _tmp1D3;_LL1: _tmp1D3=_tmp1D2->toplevel;_LL2: {int t=_tmp1D3;
return t;}}
# 1113
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D4=e;int*_tmp1D9;int _tmp1D8;struct Cyc_Toc_FallthruInfo*_tmp1D7;struct _fat_ptr**_tmp1D6;struct _fat_ptr**_tmp1D5;_LL1: _tmp1D5=_tmp1D4->break_lab;_tmp1D6=_tmp1D4->continue_lab;_tmp1D7=_tmp1D4->fallthru_info;_tmp1D8=_tmp1D4->toplevel;_tmp1D9=_tmp1D4->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D5;struct _fat_ptr**c=_tmp1D6;struct Cyc_Toc_FallthruInfo*f=_tmp1D7;int t=_tmp1D8;int*lhs=_tmp1D9;
return({struct Cyc_Toc_Env*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));_tmp1DA->break_lab=b,_tmp1DA->continue_lab=c,_tmp1DA->fallthru_info=f,_tmp1DA->toplevel=0,_tmp1DA->in_lhs=lhs,_tmp1DA->rgn=r;_tmp1DA;});}}
# 1117
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DB=e;int*_tmp1E0;int _tmp1DF;struct Cyc_Toc_FallthruInfo*_tmp1DE;struct _fat_ptr**_tmp1DD;struct _fat_ptr**_tmp1DC;_LL1: _tmp1DC=_tmp1DB->break_lab;_tmp1DD=_tmp1DB->continue_lab;_tmp1DE=_tmp1DB->fallthru_info;_tmp1DF=_tmp1DB->toplevel;_tmp1E0=_tmp1DB->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1DC;struct _fat_ptr**c=_tmp1DD;struct Cyc_Toc_FallthruInfo*f=_tmp1DE;int t=_tmp1DF;int*lhs=_tmp1E0;
return({struct Cyc_Toc_Env*_tmp1E1=_region_malloc(r,sizeof(*_tmp1E1));_tmp1E1->break_lab=b,_tmp1E1->continue_lab=c,_tmp1E1->fallthru_info=f,_tmp1E1->toplevel=1,_tmp1E1->in_lhs=lhs,_tmp1E1->rgn=r;_tmp1E1;});}}
# 1121
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E2=nv;int*_tmp1E3;_LL1: _tmp1E3=_tmp1E2->in_lhs;_LL2: {int*b=_tmp1E3;
return*b;}}
# 1125
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E4=e;int*_tmp1E5;_LL1: _tmp1E5=_tmp1E4->in_lhs;_LL2: {int*lhs=_tmp1E5;
*lhs=b;}}
# 1130
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E6=e;int*_tmp1EB;int _tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;struct _fat_ptr**_tmp1E8;struct _fat_ptr**_tmp1E7;_LL1: _tmp1E7=_tmp1E6->break_lab;_tmp1E8=_tmp1E6->continue_lab;_tmp1E9=_tmp1E6->fallthru_info;_tmp1EA=_tmp1E6->toplevel;_tmp1EB=_tmp1E6->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1E7;struct _fat_ptr**c=_tmp1E8;struct Cyc_Toc_FallthruInfo*f=_tmp1E9;int t=_tmp1EA;int*lhs=_tmp1EB;
return({struct Cyc_Toc_Env*_tmp1EC=_region_malloc(r,sizeof(*_tmp1EC));_tmp1EC->break_lab=b,_tmp1EC->continue_lab=c,_tmp1EC->fallthru_info=f,_tmp1EC->toplevel=t,_tmp1EC->in_lhs=lhs,_tmp1EC->rgn=r;_tmp1EC;});}}
# 1137
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1ED=e;int*_tmp1F2;int _tmp1F1;struct Cyc_Toc_FallthruInfo*_tmp1F0;struct _fat_ptr**_tmp1EF;struct _fat_ptr**_tmp1EE;_LL1: _tmp1EE=_tmp1ED->break_lab;_tmp1EF=_tmp1ED->continue_lab;_tmp1F0=_tmp1ED->fallthru_info;_tmp1F1=_tmp1ED->toplevel;_tmp1F2=_tmp1ED->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1EE;struct _fat_ptr**c=_tmp1EF;struct Cyc_Toc_FallthruInfo*f=_tmp1F0;int t=_tmp1F1;int*lhs=_tmp1F2;
return({struct Cyc_Toc_Env*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->break_lab=0,_tmp1F3->continue_lab=0,_tmp1F3->fallthru_info=f,_tmp1F3->toplevel=t,_tmp1F3->in_lhs=lhs,_tmp1F3->rgn=r;_tmp1F3;});}}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1147
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F9;int _tmp1F8;struct Cyc_Toc_FallthruInfo*_tmp1F7;struct _fat_ptr**_tmp1F6;struct _fat_ptr**_tmp1F5;_LL1: _tmp1F5=_tmp1F4->break_lab;_tmp1F6=_tmp1F4->continue_lab;_tmp1F7=_tmp1F4->fallthru_info;_tmp1F8=_tmp1F4->toplevel;_tmp1F9=_tmp1F4->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1F5;struct _fat_ptr**c=_tmp1F6;struct Cyc_Toc_FallthruInfo*f=_tmp1F7;int t=_tmp1F8;int*lhs=_tmp1F9;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));
_tmp1FC->label=fallthru_l,_tmp1FC->binders=fallthru_binders;_tmp1FC;});
return({struct Cyc_Toc_Env*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));({struct _fat_ptr**_tmp87B=({struct _fat_ptr**_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));*_tmp1FA=break_l;_tmp1FA;});_tmp1FB->break_lab=_tmp87B;}),_tmp1FB->continue_lab=c,_tmp1FB->fallthru_info=fi,_tmp1FB->toplevel=t,_tmp1FB->in_lhs=lhs,_tmp1FB->rgn=r;_tmp1FB;});}}
# 1152
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1154
struct Cyc_Toc_Env*_tmp1FD=e;int*_tmp202;int _tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;struct _fat_ptr**_tmp1FF;struct _fat_ptr**_tmp1FE;_LL1: _tmp1FE=_tmp1FD->break_lab;_tmp1FF=_tmp1FD->continue_lab;_tmp200=_tmp1FD->fallthru_info;_tmp201=_tmp1FD->toplevel;_tmp202=_tmp1FD->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1FE;struct _fat_ptr**c=_tmp1FF;struct Cyc_Toc_FallthruInfo*f=_tmp200;int t=_tmp201;int*lhs=_tmp202;
return({struct Cyc_Toc_Env*_tmp204=_region_malloc(r,sizeof(*_tmp204));({struct _fat_ptr**_tmp87C=({struct _fat_ptr**_tmp203=_region_malloc(r,sizeof(*_tmp203));*_tmp203=break_l;_tmp203;});_tmp204->break_lab=_tmp87C;}),_tmp204->continue_lab=c,_tmp204->fallthru_info=0,_tmp204->toplevel=t,_tmp204->in_lhs=lhs,_tmp204->rgn=r;_tmp204;});}}
# 1161
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1163
struct Cyc_Toc_Env*_tmp205=e;int*_tmp20A;int _tmp209;struct Cyc_Toc_FallthruInfo*_tmp208;struct _fat_ptr**_tmp207;struct _fat_ptr**_tmp206;_LL1: _tmp206=_tmp205->break_lab;_tmp207=_tmp205->continue_lab;_tmp208=_tmp205->fallthru_info;_tmp209=_tmp205->toplevel;_tmp20A=_tmp205->in_lhs;_LL2: {struct _fat_ptr**b=_tmp206;struct _fat_ptr**c=_tmp207;struct Cyc_Toc_FallthruInfo*f=_tmp208;int t=_tmp209;int*lhs=_tmp20A;
return({struct Cyc_Toc_Env*_tmp20C=_region_malloc(r,sizeof(*_tmp20C));_tmp20C->break_lab=0,_tmp20C->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp87D=({struct Cyc_Toc_FallthruInfo*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->label=next_l,_tmp20B->binders=0;_tmp20B;});_tmp20C->fallthru_info=_tmp87D;}),_tmp20C->toplevel=t,_tmp20C->in_lhs=lhs,_tmp20C->rgn=r;_tmp20C;});}}
# 1176 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1179
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp20D=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp19=_tmp20D;struct _tuple32*_tmp20E=_stmttmp19;struct Cyc_Absyn_Exp*_tmp20F;_LL1: _tmp20F=_tmp20E->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp20F;
Cyc_Toc_exptypes_to_c(e);
l=l->tl;}}}
# 1187
static void Cyc_Toc_asm_iolist_toc(struct Cyc_Toc_Env*nv,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp210=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp1A=_tmp210;struct _tuple32*_tmp211=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp212;_LL1: _tmp212=_tmp211->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp212;
Cyc_Toc_exp_to_c(nv,e);
l=l->tl;}}}
# 1195
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp213=e->annot;void*_stmttmp1B=_tmp213;void*_tmp214=_stmttmp1B;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp214)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp214)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp214)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp214)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp214)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp214)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp216=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B8;_tmp7B8.tag=0U,({struct _fat_ptr _tmp87E=({const char*_tmp217="Toc: do_null_check";_tag_fat(_tmp217,sizeof(char),19U);});_tmp7B8.f1=_tmp87E;});_tmp7B8;});void*_tmp215[1U];_tmp215[0]=& _tmp216;({unsigned _tmp87F=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp87F,_tag_fat(_tmp215,sizeof(void*),1U));});});}}}}}}_LL0:;}
# 1211
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1213
int ans;
int _tmp218=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp218;
void*_tmp219=Cyc_Absyn_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp219;
void*_tmp21A=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp21A;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp21B=old_typ;void*_tmp21F;void*_tmp21E;struct Cyc_Absyn_Tqual _tmp21D;void*_tmp21C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->tag == 3U){_LL1: _tmp21C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).elt_type;_tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).elt_tq;_tmp21E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).ptr_atts).bounds;_tmp21F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21B)->f1).ptr_atts).zero_term;_LL2: {void*ta=_tmp21C;struct Cyc_Absyn_Tqual tq=_tmp21D;void*b=_tmp21E;void*zt=_tmp21F;
# 1224
{void*_tmp220=annot;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Absyn_Exp*_tmp222;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1226
 if(!((unsigned)({void*_tmp880=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp880,b);}))){
# 1229
void*_tmp223=({void*_tmp881=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp881,tq);});void*newt=_tmp223;
({void*_tmp884=({void*_tmp883=newt;Cyc_Toc_cast_it_r(_tmp883,({struct Cyc_Absyn_Exp*_tmp882=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp882,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp884;});
ptr->topt=newt;}
# 1233
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1236
 if(!((unsigned)({void*_tmp885=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp885,b);}))){
# 1238
void*_tmp224=({void*_tmp886=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp886,tq);});void*newt=_tmp224;
({void*_tmp889=({void*_tmp888=newt;Cyc_Toc_cast_it_r(_tmp888,({struct Cyc_Absyn_Exp*_tmp887=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp887,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp889;});
ptr->topt=newt;
# 1245
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp226=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B9;_tmp7B9.tag=0U,({struct _fat_ptr _tmp88A=({const char*_tmp227="subscript of ? with no bounds check but need null check";_tag_fat(_tmp227,sizeof(char),56U);});_tmp7B9.f1=_tmp88A;});_tmp7B9;});void*_tmp225[1U];_tmp225[0]=& _tmp226;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp225,sizeof(void*),1U));});}
# 1249
({void*_tmp88E=({void*_tmp88D=new_typ;Cyc_Toc_cast_it_r(_tmp88D,({struct Cyc_Absyn_Exp*_tmp228[1U];({struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_new_exp(ptr->r,0U);_tmp228[0]=_tmp88B;});({struct Cyc_Absyn_Exp*_tmp88C=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp88C,_tag_fat(_tmp228,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp88E;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1255
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp893=({void*_tmp892=newt;Cyc_Toc_cast_it_r(_tmp892,({struct Cyc_Absyn_Exp*_tmp229[3U];({
struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_new_exp(ptr->r,0U);_tmp229[0]=_tmp890;}),({
struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp229[1]=_tmp88F;}),_tmp229[2]=ind;({struct Cyc_Absyn_Exp*_tmp891=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp891,_tag_fat(_tmp229,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1258
ptr->r=_tmp893;});
# 1262
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp222=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->f1;_LLF: {struct Cyc_Absyn_Exp*bd=_tmp222;
# 1266
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp221=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp221=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp221;
# 1269
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1274
struct _tuple13 _tmp22A=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _stmttmp1C=_tmp22A;struct _tuple13 _tmp22B=_stmttmp1C;int _tmp22D;unsigned _tmp22C;_LL15: _tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;_LL16: {unsigned i=_tmp22C;int valid=_tmp22D;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1277
({void*_tmp898=({void*_tmp897=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp897,({struct Cyc_Absyn_Exp*_tmp22E[4U];({
struct Cyc_Absyn_Exp*_tmp895=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22E[0]=_tmp895;}),_tmp22E[1]=bd,({
struct Cyc_Absyn_Exp*_tmp894=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp22E[2]=_tmp894;}),_tmp22E[3]=ind;({struct Cyc_Absyn_Exp*_tmp896=fn_e;Cyc_Toc_fncall_exp_dl(_tmp896,_tag_fat(_tmp22E,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1277
ptr->r=_tmp898;});
# 1280
ans=1;}else{
# 1283
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1286
({void*_tmp89C=({void*_tmp89B=new_typ;Cyc_Toc_cast_it_r(_tmp89B,({struct Cyc_Absyn_Exp*_tmp22F[1U];({struct Cyc_Absyn_Exp*_tmp899=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp22F[0]=_tmp899;});({struct Cyc_Absyn_Exp*_tmp89A=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp89A,_tag_fat(_tmp22F,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1286
ptr->r=_tmp89C;});
# 1288
ans=0;}else{
# 1292
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp8A0=({void*_tmp89F=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp89F,({struct Cyc_Absyn_Exp*_tmp230[3U];({
struct Cyc_Absyn_Exp*_tmp89D=Cyc_Absyn_new_exp(ptr->r,0U);_tmp230[0]=_tmp89D;}),_tmp230[1]=bd,_tmp230[2]=ind;({struct Cyc_Absyn_Exp*_tmp89E=fn_e;Cyc_Toc_fncall_exp_dl(_tmp89E,_tag_fat(_tmp230,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1293
ptr->r=_tmp8A0;});
# 1295
ans=1;}}
# 1298
goto _LL5;}}}else{_LL12: _LL13:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp232=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BA;_tmp7BA.tag=0U,({struct _fat_ptr _tmp8A1=({const char*_tmp233="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp233,sizeof(char),34U);});_tmp7BA.f1=_tmp8A1;});_tmp7BA;});void*_tmp231[1U];_tmp231[0]=& _tmp232;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp231,sizeof(void*),1U));});}}}}}}_LL5:;}
# 1301
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BB;_tmp7BB.tag=0U,({struct _fat_ptr _tmp8A2=({const char*_tmp236="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp236,sizeof(char),31U);});_tmp7BB.f1=_tmp8A2;});_tmp7BB;});void*_tmp234[1U];_tmp234[0]=& _tmp235;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp234,sizeof(void*),1U));});}_LL0:;}}
# 1307
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp238=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BC;_tmp7BC.tag=0U,({struct _fat_ptr _tmp8A3=({const char*_tmp239="Missing type in primop ";_tag_fat(_tmp239,sizeof(char),24U);});_tmp7BC.f1=_tmp8A3;});_tmp7BC;});void*_tmp237[1U];_tmp237[0]=& _tmp238;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp237,sizeof(void*),1U));});
return(void*)_check_null(e->topt);}
# 1311
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple23*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=Cyc_Toc_mt_tq,({void*_tmp8A4=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp23A->f2=_tmp8A4;});_tmp23A;});}
# 1316
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp23B=e->r;void*_stmttmp1D=_tmp23B;void*_tmp23C=_stmttmp1D;int _tmp23E;struct Cyc_Absyn_Exp*_tmp23D;int _tmp240;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_List_List*_tmp241;switch(*((int*)_tmp23C)){case 26U: _LL1: _tmp241=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_LL2: {struct Cyc_List_List*dles=_tmp241;
# 1319
{struct Cyc_List_List*_tmp242=dles;struct Cyc_List_List*dles2=_tmp242;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp243="array designators for abstract-field initialization";_tag_fat(_tmp243,sizeof(char),52U);}));}}
_tmp23F=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles),0U);_tmp240=0;goto _LL4;}case 27U: _LL3: _tmp23F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23C)->f2;_tmp240=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23C)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp23F;int zt=_tmp240;
_tmp23D=bd;_tmp23E=zt;goto _LL6;}case 28U: _LL5: _tmp23D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_tmp23E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23C)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp23D;int zt=_tmp23E;
# 1325
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp8A5=bd;Cyc_Absyn_add_exp(_tmp8A5,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1334
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1342
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp244=e->r;void*_stmttmp1E=_tmp244;void*_tmp245=_stmttmp1E;struct Cyc_List_List*_tmp246;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp245)->tag == 29U){_LL1: _tmp246=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp245)->f3;_LL2: {struct Cyc_List_List*dles2=_tmp246;
dles=dles2;goto _LL0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1348
{void*_tmp247=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp1F=_tmp247;void*_tmp248=_stmttmp1F;struct Cyc_Absyn_Aggrdecl*_tmp249;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp249=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp249;
# 1350
if(ad->impl == 0)
return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1358
 return 0;}_LL5:;}
# 1360
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp24A=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp24A;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp24B=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp20=_tmp24B;struct _tuple19*_tmp24C=_stmttmp20;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_List_List*_tmp24D;_LLB: _tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;_LLC: {struct Cyc_List_List*ds=_tmp24D;struct Cyc_Absyn_Exp*e2=_tmp24E;
struct _fat_ptr*_tmp24F=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp24F;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp250=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp250;
if(nested_ans != 0)
return nested_ans;{
void*_tmp251=Cyc_Absyn_compress(last_type_field->type);void*_stmttmp21=_tmp251;void*_tmp252=_stmttmp21;struct Cyc_Absyn_Exp*_tmp254;void*_tmp253;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp252)->tag == 4U){_LLE: _tmp253=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp252)->f1).elt_type;_tmp254=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp252)->f1).num_elts;_LLF: {void*elt_type=_tmp253;struct Cyc_Absyn_Exp*type_bd=_tmp254;
# 1375
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1381
return({struct Cyc_Absyn_Exp*_tmp8A8=({struct Cyc_Absyn_Exp*_tmp255[2U];_tmp255[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1383
struct Cyc_Absyn_Exp*_tmp8A6=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp255[1]=_tmp8A6;});({struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8A7,_tag_fat(_tmp255,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1381
Cyc_Absyn_add_exp(_tmp8A8,
# 1384
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{_LL10: _LL11:
 return 0;}_LLD:;}}}}
# 1389
({struct Cyc_Warn_String_Warn_Warg_struct _tmp257=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BD;_tmp7BD.tag=0U,({struct _fat_ptr _tmp8A9=({const char*_tmp258="get_varsizeexp: did not find last struct field";_tag_fat(_tmp258,sizeof(char),47U);});_tmp7BD.f1=_tmp8A9;});_tmp7BD;});void*_tmp256[1U];_tmp256[0]=& _tmp257;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp256,sizeof(void*),1U));});}}
# 1392
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp259=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp259;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp25A=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp25A;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1399
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C0;_tmp7C0.tag=0U,({struct _fat_ptr _tmp8AA=({const char*_tmp260="get_member_offset ";_tag_fat(_tmp260,sizeof(char),19U);});_tmp7C0.f1=_tmp8AA;});_tmp7C0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BF;_tmp7BF.tag=0U,_tmp7BF.f1=*f;_tmp7BF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BE;_tmp7BE.tag=0U,({struct _fat_ptr _tmp8AB=({const char*_tmp25F=" failed";_tag_fat(_tmp25F,sizeof(char),8U);});_tmp7BE.f1=_tmp8AB;});_tmp7BE;});void*_tmp25B[3U];_tmp25B[0]=& _tmp25C,_tmp25B[1]=& _tmp25D,_tmp25B[2]=& _tmp25E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp25B,sizeof(void*),3U));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1403
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1406
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1409
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261->tag=5U,_tmp261->f1=e1,_tmp261->f2=incr;_tmp261;}),0U);}
# 1413
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1422
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1424
void*_tmp262=e1->r;void*_stmttmp22=_tmp262;void*_tmp263=_stmttmp22;int _tmp267;int _tmp266;struct _fat_ptr*_tmp265;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp269;void*_tmp268;struct Cyc_Absyn_Stmt*_tmp26A;switch(*((int*)_tmp263)){case 37U: _LL1: _tmp26A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp263)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp26A;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14U: _LL3: _tmp268=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp263)->f1;_tmp269=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp263)->f2;_LL4: {void*t=_tmp268;struct Cyc_Absyn_Exp*e=_tmp269;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21U: _LL5: _tmp264=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp263)->f1;_tmp265=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp263)->f2;_tmp266=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp263)->f3;_tmp267=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp263)->f4;_LL6: {struct Cyc_Absyn_Exp*e=_tmp264;struct _fat_ptr*fld=_tmp265;int is_tagged=_tmp266;int is_read=_tmp267;
# 1429
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp8AE=e1;struct Cyc_List_List*_tmp8AD=({struct Cyc_List_List*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->hd=fld,_tmp26B->tl=fs;_tmp26B;});struct Cyc_Absyn_Exp*(*_tmp8AC)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp8AE,_tmp8AD,_tmp8AC,f_env);});
goto _LL0;}default: _LL7: _LL8: {
# 1437
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1439
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp8AF=(f(e1_copy,f_env))->r;e1->r=_tmp8AF;});
goto _LL0;}}_LL0:;}
# 1445
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1447
void*_tmp26C=s->r;void*_stmttmp23=_tmp26C;void*_tmp26D=_stmttmp23;struct Cyc_Absyn_Stmt*_tmp26E;struct Cyc_Absyn_Stmt*_tmp270;struct Cyc_Absyn_Decl*_tmp26F;struct Cyc_Absyn_Exp*_tmp271;switch(*((int*)_tmp26D)){case 1U: _LL1: _tmp271=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp26D)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp271;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12U: _LL3: _tmp26F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26D)->f1;_tmp270=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26D)->f2;_LL4: {struct Cyc_Absyn_Decl*d=_tmp26F;struct Cyc_Absyn_Stmt*s2=_tmp270;
# 1450
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2U: _LL5: _tmp26E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp26D)->f2;_LL6: {struct Cyc_Absyn_Stmt*s2=_tmp26E;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp273=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C2;_tmp7C2.tag=0U,({struct _fat_ptr _tmp8B0=({const char*_tmp275="lvalue_assign_stmt: ";_tag_fat(_tmp275,sizeof(char),21U);});_tmp7C2.f1=_tmp8B0;});_tmp7C2;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp274=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7C1;_tmp7C1.tag=5U,_tmp7C1.f1=s;_tmp7C1;});void*_tmp272[2U];_tmp272[0]=& _tmp273,_tmp272[1]=& _tmp274;Cyc_Toc_toc_impos2(_tag_fat(_tmp272,sizeof(void*),2U));});}_LL0:;}
# 1456
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1460
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp276=e->r;void*_stmttmp24=_tmp276;void*_tmp277=_stmttmp24;struct Cyc_Absyn_Stmt*_tmp278;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp**_tmp27B;void**_tmp27A;switch(*((int*)_tmp277)){case 14U: _LL1: _tmp27A=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp277)->f1;_tmp27B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp277)->f2;_LL2: {void**t=_tmp27A;struct Cyc_Absyn_Exp**e1=_tmp27B;
# 1463
({struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Toc_push_address_exp(*e1);*e1=_tmp8B1;});
({void*_tmp8B2=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp8B2;});
return e;}case 20U: _LL3: _tmp279=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp277)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp279;
# 1467
return e1;}case 37U: _LL5: _tmp278=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp277)->f1;_LL6: {struct Cyc_Absyn_Stmt*s=_tmp278;
# 1471
Cyc_Toc_push_address_stmt(s);
return e;}default: _LL7: _LL8:
# 1474
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C4;_tmp7C4.tag=0U,({struct _fat_ptr _tmp8B3=({const char*_tmp27F="can't take & of exp ";_tag_fat(_tmp27F,sizeof(char),21U);});_tmp7C4.f1=_tmp8B3;});_tmp7C4;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp27E=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7C3;_tmp7C3.tag=4U,_tmp7C3.f1=e;_tmp7C3;});void*_tmp27C[2U];_tmp27C[0]=& _tmp27D,_tmp27C[1]=& _tmp27E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp27C,sizeof(void*),2U));});}_LL0:;}
# 1478
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp280=s->r;void*_stmttmp25=_tmp280;void*_tmp281=_stmttmp25;struct Cyc_Absyn_Exp**_tmp282;struct Cyc_Absyn_Stmt*_tmp283;struct Cyc_Absyn_Stmt*_tmp284;switch(*((int*)_tmp281)){case 2U: _LL1: _tmp284=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp281)->f2;_LL2: {struct Cyc_Absyn_Stmt*s2=_tmp284;
_tmp283=s2;goto _LL4;}case 12U: _LL3: _tmp283=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp281)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp283;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1U: _LL5: _tmp282=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp281)->f1;_LL6: {struct Cyc_Absyn_Exp**e=_tmp282;
({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Toc_push_address_exp(*e);*e=_tmp8B4;});goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp286=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C6;_tmp7C6.tag=0U,({struct _fat_ptr _tmp8B5=({const char*_tmp288="can't take & of stmt ";_tag_fat(_tmp288,sizeof(char),22U);});_tmp7C6.f1=_tmp8B5;});_tmp7C6;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp287=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7C5;_tmp7C5.tag=5U,_tmp7C5.f1=s;_tmp7C5;});void*_tmp285[2U];_tmp285[0]=& _tmp286,_tmp285[1]=& _tmp287;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp285,sizeof(void*),2U));});}_LL0:;}
# 1490
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1502
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type,Cyc_Absyn_false_type);
void*_tmp289=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp289;
void*_tmp28A=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp28A;
void*_tmp28B=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp28B;
struct Cyc_Core_Opt*_tmp28C=({struct Cyc_Core_Opt*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->v=c_ptr_type;_tmp2BF;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp28C;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp28D=e1->r;void*_stmttmp26=_tmp28D;void*_tmp28E=_stmttmp26;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp291;switch(*((int*)_tmp28E)){case 20U: _LL1: _tmp291=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp28E)->f1;_LL2: {struct Cyc_Absyn_Exp*ea=_tmp291;
# 1510
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1515
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23U: _LL3: _tmp28F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28E)->f1;_tmp290=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28E)->f2;_LL4: {struct Cyc_Absyn_Exp*ea=_tmp28F;struct Cyc_Absyn_Exp*eb=_tmp290;
# 1517
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1522
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp292[3U];_tmp292[0]=ea,({
struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp292[1]=_tmp8B6;}),_tmp292[2]=eb;({struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp8B7,_tag_fat(_tmp292,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp294=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C7;_tmp7C7.tag=0U,({struct _fat_ptr _tmp8B8=({const char*_tmp295="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp295,sizeof(char),53U);});_tmp7C7.f1=_tmp8B8;});_tmp7C7;});void*_tmp293[1U];_tmp293[0]=& _tmp294;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp293,sizeof(void*),1U));});}_LL0:;}{
# 1528
struct _tuple1*_tmp296=Cyc_Toc_temp_var();struct _tuple1*x=_tmp296;
struct _RegionHandle _tmp297=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp297;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp298=({struct Cyc_Absyn_Vardecl*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->sc=Cyc_Absyn_Public,_tmp2BE->name=x,_tmp2BE->varloc=0U,_tmp2BE->tq=Cyc_Toc_mt_tq,_tmp2BE->type=c_fat_ptr_type,_tmp2BE->initializer=xinit,_tmp2BE->rgn=0,_tmp2BE->attributes=0,_tmp2BE->escapes=0,_tmp2BE->is_proto=0;_tmp2BE;});struct Cyc_Absyn_Vardecl*x_vd=_tmp298;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp299=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->tag=4U,_tmp2BD->f1=x_vd;_tmp2BD;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp299;
struct Cyc_Absyn_Exp*_tmp29A=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp29A;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp29B=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp29B;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp29C=Cyc_Toc_temp_var();struct _tuple1*y=_tmp29C;
struct Cyc_Absyn_Vardecl*_tmp29D=({struct Cyc_Absyn_Vardecl*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->sc=Cyc_Absyn_Public,_tmp2BC->name=y,_tmp2BC->varloc=0U,_tmp2BC->tq=Cyc_Toc_mt_tq,_tmp2BC->type=c_elt_type,_tmp2BC->initializer=deref_x,_tmp2BC->rgn=0,_tmp2BC->attributes=0,_tmp2BC->escapes=0,_tmp2BC->is_proto=0;_tmp2BC;});struct Cyc_Absyn_Vardecl*y_vd=_tmp29D;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29E=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->tag=4U,_tmp2BB->f1=y_vd;_tmp2BB;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp29E;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp29F=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp29F;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp8BA=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp8B9=y_exp;Cyc_Absyn_prim2_exp(_tmp8BA,_tmp8B9,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1549
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2A0=Cyc_Toc_temp_var();struct _tuple1*z=_tmp2A0;
struct Cyc_Absyn_Vardecl*_tmp2A1=({struct Cyc_Absyn_Vardecl*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->sc=Cyc_Absyn_Public,_tmp2BA->name=z,_tmp2BA->varloc=0U,_tmp2BA->tq=Cyc_Toc_mt_tq,_tmp2BA->type=c_elt_type,_tmp2BA->initializer=z_init,_tmp2BA->rgn=0,_tmp2BA->attributes=0,_tmp2BA->escapes=0,_tmp2BA->is_proto=0;_tmp2BA;});struct Cyc_Absyn_Vardecl*z_vd=_tmp2A1;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A2=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->tag=4U,_tmp2B9->f1=z_vd;_tmp2B9;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp2A2;
# 1555
struct Cyc_Absyn_Exp*_tmp2A3=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp2A3;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A4=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp2A4;
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp2A5;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1562
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp2A6;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp2A7;
struct Cyc_Absyn_Exp*_tmp2A8=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp2A8;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1569
struct Cyc_List_List*_tmp2A9=({struct Cyc_Absyn_Exp*_tmp2B8[2U];({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp2B8[0]=_tmp8BC;}),({
struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B8[1]=_tmp8BB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2B8,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 1569
struct Cyc_List_List*xsizeargs=_tmp2A9;
# 1571
struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp2AA;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp8BD,oneexp,0U);});{
# 1576
struct Cyc_Absyn_Exp*_tmp2AB=({struct Cyc_Absyn_Exp*_tmp8BE=xsize;Cyc_Absyn_and_exp(_tmp8BE,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp2AB;
struct Cyc_Absyn_Stmt*_tmp2AC=Cyc_Absyn_exp_stmt(({void*_tmp2B7=0U;({struct Cyc_Absyn_Exp*_tmp8BF=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp8BF,_tag_fat(_tmp2B7,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp2AC;
struct Cyc_Absyn_Exp*_tmp2AD=({struct Cyc_Absyn_Exp*_tmp8C0=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp8C0,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp2AD;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp2AE=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp2AE;
struct Cyc_Absyn_Exp*_tmp2AF=({struct Cyc_Absyn_Exp*_tmp8C1=deref_xcurr;Cyc_Absyn_assign_exp(_tmp8C1,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp2AF;
struct Cyc_Absyn_Stmt*_tmp2B0=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp2B0;
s=({struct Cyc_Absyn_Stmt*_tmp8C4=({struct Cyc_Absyn_Exp*_tmp8C3=comp_exp;struct Cyc_Absyn_Stmt*_tmp8C2=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp8C3,_tmp8C2,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp8C4,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C6=({struct Cyc_Absyn_Decl*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));({void*_tmp8C5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=0U,_tmp2B1->f1=z_vd;_tmp2B1;});_tmp2B2->r=_tmp8C5;}),_tmp2B2->loc=0U;_tmp2B2;});Cyc_Absyn_decl_stmt(_tmp8C6,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C8=({struct Cyc_Absyn_Decl*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));({void*_tmp8C7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->tag=0U,_tmp2B3->f1=y_vd;_tmp2B3;});_tmp2B4->r=_tmp8C7;}),_tmp2B4->loc=0U;_tmp2B4;});Cyc_Absyn_decl_stmt(_tmp8C8,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8CA=({struct Cyc_Absyn_Decl*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));({void*_tmp8C9=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->tag=0U,_tmp2B5->f1=x_vd;_tmp2B5;});_tmp2B6->r=_tmp8C9;}),_tmp2B6->loc=0U;_tmp2B6;});Cyc_Absyn_decl_stmt(_tmp8CA,s,0U);});
({void*_tmp8CB=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8CB;});}}}}}}}}}}
# 1530
;_pop_region();}}
# 1602 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1606
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2C0=Cyc_Absyn_compress(aggrtype);void*_stmttmp27=_tmp2C0;void*_tmp2C1=_stmttmp27;union Cyc_Absyn_AggrInfo _tmp2C2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1)->f1)->tag == 20U){_LL1: _tmp2C2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C1)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2C2;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CA;_tmp7CA.tag=0U,({struct _fat_ptr _tmp8CC=({const char*_tmp2C8="expecting union but found ";_tag_fat(_tmp2C8,sizeof(char),27U);});_tmp7CA.f1=_tmp8CC;});_tmp7CA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C9;_tmp7C9.tag=2U,_tmp7C9.f1=(void*)aggrtype;_tmp7C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C8;_tmp7C8.tag=0U,({
struct _fat_ptr _tmp8CD=({const char*_tmp2C7=" in check_tagged_union";_tag_fat(_tmp2C7,sizeof(char),23U);});_tmp7C8.f1=_tmp8CD;});_tmp7C8;});void*_tmp2C3[3U];_tmp2C3[0]=& _tmp2C4,_tmp2C3[1]=& _tmp2C5,_tmp2C3[2]=& _tmp2C6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C3,sizeof(void*),3U));});}_LL0:;}{
# 1612
struct _tuple30 _tmp2C9=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp28=_tmp2C9;struct _tuple30 _tmp2CA=_stmttmp28;struct Cyc_Absyn_Exp*_tmp2CC;struct _tuple1*_tmp2CB;_LL6: _tmp2CB=_tmp2CA.f1;_tmp2CC=_tmp2CA.f2;_LL7: {struct _tuple1*temp=_tmp2CB;struct Cyc_Absyn_Exp*temp_exp=_tmp2CC;
struct Cyc_Absyn_Exp*_tmp2CD=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2CD;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2CE;
struct Cyc_Absyn_Exp*_tmp2CF=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2CF;
struct Cyc_Absyn_Exp*_tmp2D0=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2D0;
struct Cyc_Absyn_Stmt*_tmp2D1=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2D1;
struct Cyc_Absyn_Stmt*_tmp2D2=({struct Cyc_Absyn_Exp*_tmp8CF=test_exp;struct Cyc_Absyn_Stmt*_tmp8CE=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8CF,_tmp8CE,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D2;
void*_tmp2D3=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2D3;
struct Cyc_Absyn_Exp*_tmp2D4=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2D4;
struct Cyc_Absyn_Stmt*_tmp2D5=({struct _tuple1*_tmp8D2=temp;void*_tmp8D1=e1_p_type;struct Cyc_Absyn_Exp*_tmp8D0=e1_f;Cyc_Absyn_declare_stmt(_tmp8D2,_tmp8D1,_tmp8D0,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D5;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1625
struct Cyc_Absyn_Exp*_tmp2D6=({struct Cyc_Absyn_Exp*_tmp8D3=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8D3,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2D6;
struct Cyc_Absyn_Exp*_tmp2D7=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2D7;
struct Cyc_Absyn_Exp*_tmp2D8=({struct Cyc_Absyn_Exp*_tmp8D4=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8D4,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2D8;
struct Cyc_Absyn_Stmt*_tmp2D9=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2D9;
struct Cyc_Absyn_Stmt*_tmp2DA=({struct Cyc_Absyn_Exp*_tmp8D6=test_exp;struct Cyc_Absyn_Stmt*_tmp8D5=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8D6,_tmp8D5,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2DA;
struct Cyc_Absyn_Stmt*_tmp2DB=({struct _tuple1*_tmp8D9=temp;void*_tmp8D8=e1_c_type;struct Cyc_Absyn_Exp*_tmp8D7=e1;Cyc_Absyn_declare_stmt(_tmp8D9,_tmp8D8,_tmp8D7,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2DB;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1635
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1638
void*_tmp2DC=Cyc_Absyn_compress(t);void*_stmttmp29=_tmp2DC;void*_tmp2DD=_stmttmp29;union Cyc_Absyn_AggrInfo _tmp2DE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DD)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DD)->f1)->tag == 20U){_LL1: _tmp2DE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2DE;
# 1640
struct Cyc_Absyn_Aggrdecl*_tmp2DF=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2DF;
({int _tmp8DA=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp8DA;});{
# 1643
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E3=({struct Cyc_String_pa_PrintArg_struct _tmp7CC;_tmp7CC.tag=0U,_tmp7CC.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7CC;});struct Cyc_String_pa_PrintArg_struct _tmp2E4=({struct Cyc_String_pa_PrintArg_struct _tmp7CB;_tmp7CB.tag=0U,_tmp7CB.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7CB;});void*_tmp2E1[2U];_tmp2E1[0]=& _tmp2E3,_tmp2E1[1]=& _tmp2E4;({struct _fat_ptr _tmp8DB=({const char*_tmp2E2="_union_%s_%s";_tag_fat(_tmp2E2,sizeof(char),13U);});Cyc_aprintf(_tmp8DB,_tag_fat(_tmp2E1,sizeof(void*),2U));});});
({void*_tmp8DC=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));*_tmp2E0=str;_tmp2E0;}));*tagged_member_type=_tmp8DC;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1654
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1657
void*_tmp2E5=e->r;void*_stmttmp2A=_tmp2E5;void*_tmp2E6=_stmttmp2A;int*_tmp2E9;struct _fat_ptr*_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E7;int*_tmp2EC;struct _fat_ptr*_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2ED;switch(*((int*)_tmp2E6)){case 14U: _LL1: _tmp2ED=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E6)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp2ED;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CD;_tmp7CD.tag=0U,({struct _fat_ptr _tmp8DD=({const char*_tmp2F0="cast on lhs!";_tag_fat(_tmp2F0,sizeof(char),13U);});_tmp7CD.f1=_tmp8DD;});_tmp7CD;});void*_tmp2EE[1U];_tmp2EE[0]=& _tmp2EF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp2EE,sizeof(void*),1U));});}case 21U: _LL3: _tmp2EA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E6)->f1;_tmp2EB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E6)->f2;_tmp2EC=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E6)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2EA;struct _fat_ptr*f=_tmp2EB;int*is_read=_tmp2EC;
# 1660
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22U: _LL5: _tmp2E7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E6)->f1;_tmp2E8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E6)->f2;_tmp2E9=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E6)->f4;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp2E7;struct _fat_ptr*f=_tmp2E8;int*is_read=_tmp2E9;
# 1663
void*_tmp2F1=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp2B=_tmp2F1;void*_tmp2F2=_stmttmp2B;void*_tmp2F3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F2)->tag == 3U){_LLA: _tmp2F3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F2)->f1).elt_type;_LLB: {void*et=_tmp2F3;
# 1665
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{_LLC: _LLD:
# 1667
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1669
 return 0;}_LL0:;}
# 1682 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1686
struct _tuple30 _tmp2F4=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2C=_tmp2F4;struct _tuple30 _tmp2F5=_stmttmp2C;struct Cyc_Absyn_Exp*_tmp2F7;struct _tuple1*_tmp2F6;_LL1: _tmp2F6=_tmp2F5.f1;_tmp2F7=_tmp2F5.f2;_LL2: {struct _tuple1*temp=_tmp2F6;struct Cyc_Absyn_Exp*temp_exp=_tmp2F7;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1695
struct Cyc_Absyn_Exp*_tmp2F8=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2F8;
s2=({struct Cyc_Absyn_Exp*_tmp8DF=test_exp;struct Cyc_Absyn_Stmt*_tmp8DE=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8DF,_tmp8DE,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1698
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8E2=temp;void*_tmp8E1=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8E0=
Cyc_Toc_push_address_exp(e1);
# 1698
Cyc_Absyn_declare_stmt(_tmp8E2,_tmp8E1,_tmp8E0,
# 1700
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1705
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CF;_tmp7CF.tag=0U,({struct _fat_ptr _tmp8E3=({const char*_tmp2FC="exp_to_c: no type for ";_tag_fat(_tmp2FC,sizeof(char),23U);});_tmp7CF.f1=_tmp8E3;});_tmp7CF;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7CE;_tmp7CE.tag=4U,_tmp7CE.f1=e;_tmp7CE;});void*_tmp2F9[2U];_tmp2F9[0]=& _tmp2FA,_tmp2F9[1]=& _tmp2FB;({unsigned _tmp8E4=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8E4,_tag_fat(_tmp2F9,sizeof(void*),2U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1710
int did_inserted_checks=0;
{void*_tmp2FD=e->annot;void*_stmttmp2D=_tmp2FD;void*_tmp2FE=_stmttmp2D;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2FE)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2FE)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1716
{void*_tmp2FF=e->r;void*_stmttmp2E=_tmp2FF;void*_tmp300=_stmttmp2E;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp301;struct Cyc_Absyn_Stmt*_tmp303;struct _fat_ptr*_tmp305;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp307;struct Cyc_Absyn_Exp*_tmp306;int _tmp30D;int _tmp30C;struct Cyc_Absyn_Exp*_tmp30B;void**_tmp30A;struct Cyc_Absyn_Exp*_tmp309;int _tmp308;struct Cyc_Absyn_Datatypefield*_tmp310;struct Cyc_Absyn_Datatypedecl*_tmp30F;struct Cyc_List_List*_tmp30E;struct Cyc_Absyn_Aggrdecl*_tmp314;struct Cyc_List_List*_tmp313;struct Cyc_List_List*_tmp312;struct _tuple1**_tmp311;struct Cyc_List_List*_tmp316;void*_tmp315;int _tmp319;void*_tmp318;struct Cyc_Absyn_Exp*_tmp317;int _tmp31D;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Vardecl*_tmp31A;struct Cyc_List_List*_tmp31E;struct Cyc_List_List*_tmp31F;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp320;struct Cyc_Absyn_Exp*_tmp322;int _tmp326;int _tmp325;struct _fat_ptr*_tmp324;struct Cyc_Absyn_Exp*_tmp323;int _tmp32A;int _tmp329;struct _fat_ptr*_tmp328;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_List_List*_tmp32C;void*_tmp32B;void*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp330;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*_tmp331;enum Cyc_Absyn_Coercion _tmp335;int _tmp334;struct Cyc_Absyn_Exp*_tmp333;void**_tmp332;struct Cyc_List_List*_tmp337;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_Exp*_tmp338;int _tmp33A;struct Cyc_Absyn_Exp*_tmp339;struct Cyc_Absyn_VarargInfo*_tmp33F;struct Cyc_List_List*_tmp33E;int _tmp33D;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Absyn_Exp*_tmp348;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Core_Opt*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;enum Cyc_Absyn_Incrementor _tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_List_List*_tmp351;enum Cyc_Absyn_Primop _tmp350;struct Cyc_Absyn_Exp*_tmp352;switch(*((int*)_tmp300)){case 2U: _LL8: _LL9:
# 1718
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp300)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1725
struct Cyc_Absyn_Exp*_tmp353=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp353;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8E5=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp8E5;});else{
# 1730
({void*_tmp8E7=({struct Cyc_Absyn_Exp*_tmp354[3U];_tmp354[0]=zero,_tmp354[1]=zero,_tmp354[2]=zero;({struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8E6,_tag_fat(_tmp354,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8E7;});}}else{
# 1732
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1734
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 42U: _LL10: _LL11:
({void*_tmp8E8=(Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U))->r;e->r=_tmp8E8;});goto _LL7;case 41U: _LL12: _tmp352=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp352;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3U: _LL14: _tmp350=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp351=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL15: {enum Cyc_Absyn_Primop p=_tmp350;struct Cyc_List_List*es=_tmp351;
# 1741
struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp355;
# 1743
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp356=p;switch(_tmp356){case Cyc_Absyn_Numelts: _LL63: _LL64: {
# 1746
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp357=Cyc_Absyn_compress((void*)_check_null(arg->topt));void*_stmttmp2F=_tmp357;void*_tmp358=_stmttmp2F;void*_tmp35C;void*_tmp35B;void*_tmp35A;void*_tmp359;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->tag == 3U){_LL78: _tmp359=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).elt_type;_tmp35A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).nullable;_tmp35B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).bounds;_tmp35C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1).ptr_atts).zero_term;_LL79: {void*elt_type=_tmp359;void*nbl=_tmp35A;void*bound=_tmp35B;void*zt=_tmp35C;
# 1749
struct Cyc_Absyn_Exp*_tmp35D=({void*_tmp8E9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E9,bound);});struct Cyc_Absyn_Exp*eopt=_tmp35D;
if(eopt == 0)
# 1752
({void*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp35E[2U];_tmp35E[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp35E[1]=_tmp8EA;});({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp8EB,_tag_fat(_tmp35E,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1752
e->r=_tmp8EC;});else{
# 1755
struct Cyc_Absyn_Exp*_tmp35F=eopt;struct Cyc_Absyn_Exp*e2=_tmp35F;
# 1757
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1761
({void*_tmp8EE=({struct Cyc_Absyn_Exp*_tmp360[2U];_tmp360[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp360[1]=e2;({struct Cyc_Absyn_Exp*_tmp8ED=function_e;Cyc_Toc_fncall_exp_r(_tmp8ED,_tag_fat(_tmp360,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8EE;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(e2))
({void*_tmp361=0U;({unsigned _tmp8F0=e->loc;struct _fat_ptr _tmp8EF=({const char*_tmp362="can't calculate numelts";_tag_fat(_tmp362,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8F0,_tmp8EF,_tag_fat(_tmp361,sizeof(void*),0U));});});
# 1766
({void*_tmp8F3=({struct Cyc_Absyn_Exp*_tmp8F2=arg;struct Cyc_Absyn_Exp*_tmp8F1=e2;Cyc_Toc_conditional_exp_r(_tmp8F2,_tmp8F1,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8F3;});}else{
# 1768
e->r=e2->r;goto _LL77;}}}
# 1771
goto _LL77;}}else{_LL7A: _LL7B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp364=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D1;_tmp7D1.tag=0U,({struct _fat_ptr _tmp8F4=({const char*_tmp366="numelts primop applied to non-pointer ";_tag_fat(_tmp366,sizeof(char),39U);});_tmp7D1.f1=_tmp8F4;});_tmp7D1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp365=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7D0;_tmp7D0.tag=2U,_tmp7D0.f1=(void*)_check_null(arg->topt);_tmp7D0;});void*_tmp363[2U];_tmp363[0]=& _tmp364,_tmp363[1]=& _tmp365;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp363,sizeof(void*),2U));});}_LL77:;}
# 1775
goto _LL62;}case Cyc_Absyn_Plus: _LL65: _LL66:
# 1780
 if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp367="can't do pointer arithmetic at top-level";_tag_fat(_tmp367,sizeof(char),41U);}));
{void*_tmp368=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp30=_tmp368;void*_tmp369=_stmttmp30;void*_tmp36C;void*_tmp36B;void*_tmp36A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp369)->tag == 3U){_LL7D: _tmp36A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp369)->f1).elt_type;_tmp36B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp369)->f1).ptr_atts).bounds;_tmp36C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp369)->f1).ptr_atts).zero_term;_LL7E: {void*elt_type=_tmp36A;void*b=_tmp36B;void*zt=_tmp36C;
# 1784
struct Cyc_Absyn_Exp*_tmp36D=({void*_tmp8F5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8F5,b);});struct Cyc_Absyn_Exp*eopt=_tmp36D;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp8F8=({struct Cyc_Absyn_Exp*_tmp36E[3U];_tmp36E[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36E[1]=_tmp8F6;}),_tmp36E[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8F7,_tag_fat(_tmp36E,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1788
e->r=_tmp8F8;});else{
# 1790
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp8FA=({struct Cyc_Absyn_Exp*_tmp36F[3U];_tmp36F[0]=e1,_tmp36F[1]=eopt,_tmp36F[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8F9,_tag_fat(_tmp36F,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8FA;});}
goto _LL7C;}}else{_LL7F: _LL80:
 goto _LL7C;}_LL7C:;}
# 1795
goto _LL62;case Cyc_Absyn_Minus: _LL67: _LL68: {
# 1800
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd)){
({void*_tmp8FC=({struct Cyc_Absyn_Exp*_tmp8FB=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8FB,Cyc_Toc_curr_sp);});e1->r=_tmp8FC;});
({void*_tmp8FE=({struct Cyc_Absyn_Exp*_tmp8FD=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8FD,Cyc_Toc_curr_sp);});e2->r=_tmp8FE;});
({void*_tmp900=({void*_tmp8FF=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8FF;});e1->topt=_tmp900;});
({void*_tmp902=({struct Cyc_Absyn_Exp*_tmp901=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp901,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1808
e->r=_tmp902;});}else{
# 1811
({void*_tmp906=({struct Cyc_Absyn_Exp*_tmp370[3U];_tmp370[0]=e1,({
struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp370[1]=_tmp904;}),({
struct Cyc_Absyn_Exp*_tmp903=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp370[2]=_tmp903;});({struct Cyc_Absyn_Exp*_tmp905=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp905,_tag_fat(_tmp370,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1811
e->r=_tmp906;});}}
# 1815
goto _LL62;}case Cyc_Absyn_Eq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Neq: _LL6B: _LL6C: goto _LL6E;case Cyc_Absyn_Gt: _LL6D: _LL6E: goto _LL70;case Cyc_Absyn_Gte: _LL6F: _LL70: goto _LL72;case Cyc_Absyn_Lt: _LL71: _LL72: goto _LL74;case Cyc_Absyn_Lte: _LL73: _LL74: {
# 1818
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(old_types))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp371=({void*_tmp907=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp907,Cyc_Toc_mt_tq);});void*t=_tmp371;
({void*_tmp90A=({void*_tmp909=t;Cyc_Toc_cast_it_r(_tmp909,({struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp908,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp90A;});
e1->topt=t;}
# 1828
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp372=({void*_tmp90B=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp90B,Cyc_Toc_mt_tq);});void*t=_tmp372;
({void*_tmp90E=({void*_tmp90D=t;Cyc_Toc_cast_it_r(_tmp90D,({struct Cyc_Absyn_Exp*_tmp90C=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp90C,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp90E;});
e2->topt=t;}
# 1833
goto _LL62;}default: _LL75: _LL76:
 goto _LL62;}_LL62:;}
# 1836
goto _LL7;}case 5U: _LL16: _tmp34E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp34F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL17: {struct Cyc_Absyn_Exp*e2=_tmp34E;enum Cyc_Absyn_Incrementor incr=_tmp34F;
# 1838
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1847 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp38E="increment";_tag_fat(_tmp38E,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp373="decrement";_tag_fat(_tmp373,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D5;_tmp7D5.tag=0U,({struct _fat_ptr _tmp90F=({const char*_tmp37B="in-place ";_tag_fat(_tmp37B,sizeof(char),10U);});_tmp7D5.f1=_tmp90F;});_tmp7D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp376=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D4;_tmp7D4.tag=0U,_tmp7D4.f1=incr_str;_tmp7D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp377=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D3;_tmp7D3.tag=0U,({struct _fat_ptr _tmp910=({const char*_tmp37A=" is not supported when ";_tag_fat(_tmp37A,sizeof(char),24U);});_tmp7D3.f1=_tmp910;});_tmp7D3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp378=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D2;_tmp7D2.tag=0U,({
struct _fat_ptr _tmp911=({const char*_tmp379="dereferencing a zero-terminated pointer";_tag_fat(_tmp379,sizeof(char),40U);});_tmp7D2.f1=_tmp911;});_tmp7D2;});void*_tmp374[4U];_tmp374[0]=& _tmp375,_tmp374[1]=& _tmp376,_tmp374[2]=& _tmp377,_tmp374[3]=& _tmp378;({unsigned _tmp912=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp912,_tag_fat(_tmp374,sizeof(void*),4U));});});
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp37C=incr;switch(_tmp37C){case Cyc_Absyn_PreInc: _LL82: _LL83:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec: _LL84: _LL85:
 op=2U;goto _LL81;default: _LL86: _LL87:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D8;_tmp7D8.tag=0U,({struct _fat_ptr _tmp913=({const char*_tmp382="in-place post";_tag_fat(_tmp382,sizeof(char),14U);});_tmp7D8.f1=_tmp913;});_tmp7D8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D7;_tmp7D7.tag=0U,_tmp7D7.f1=incr_str;_tmp7D7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp380=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D6;_tmp7D6.tag=0U,({
struct _fat_ptr _tmp914=({const char*_tmp381=" is not supported on @tagged union members";_tag_fat(_tmp381,sizeof(char),43U);});_tmp7D6.f1=_tmp914;});_tmp7D6;});void*_tmp37D[3U];_tmp37D[0]=& _tmp37E,_tmp37D[1]=& _tmp37F,_tmp37D[2]=& _tmp380;({unsigned _tmp915=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp915,_tag_fat(_tmp37D,sizeof(void*),3U));});});}_LL81:;}
# 1865
({void*_tmp917=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->tag=4U,_tmp384->f1=e2,({struct Cyc_Core_Opt*_tmp916=({struct Cyc_Core_Opt*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->v=(void*)op;_tmp383;});_tmp384->f2=_tmp916;}),_tmp384->f3=one;_tmp384;});e->r=_tmp917;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1869
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1874
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)incr == (int)1U ||(int)incr == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1880
if((int)incr == (int)2U ||(int)incr == (int)3U)
change=-1;
({void*_tmp91C=({struct Cyc_Absyn_Exp*_tmp385[3U];({struct Cyc_Absyn_Exp*_tmp91A=Cyc_Toc_push_address_exp(e2);_tmp385[0]=_tmp91A;}),({
struct Cyc_Absyn_Exp*_tmp919=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp385[1]=_tmp919;}),({
struct Cyc_Absyn_Exp*_tmp918=Cyc_Absyn_signed_int_exp(change,0U);_tmp385[2]=_tmp918;});({struct Cyc_Absyn_Exp*_tmp91B=fn_e;Cyc_Toc_fncall_exp_r(_tmp91B,_tag_fat(_tmp385,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1882
e->r=_tmp91C;});}else{
# 1885
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1891
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp386=Cyc_Toc_temp_var();struct _tuple1*x=_tmp386;
void*_tmp387=({void*_tmp91D=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp91D,Cyc_Toc_mt_tq);});void*t=_tmp387;
struct Cyc_Absyn_Exp*_tmp388=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp388;
struct Cyc_Absyn_Exp*_tmp389=({struct Cyc_Absyn_Exp*_tmp91E=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp91E,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1896
struct Cyc_Absyn_Exp*testexp=_tmp389;
# 1898
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp920=({struct Cyc_Absyn_Exp*_tmp91F=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp91F,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1899
Cyc_Absyn_and_exp(_tmp920,testexp,0U);});{
# 1901
struct Cyc_Absyn_Stmt*_tmp38A=({struct Cyc_Absyn_Exp*_tmp923=testexp;struct Cyc_Absyn_Stmt*_tmp922=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1901
Cyc_Absyn_ifthenelse_stmt(_tmp923,_tmp922,
# 1903
Cyc_Absyn_exp_stmt(({void*_tmp38B=0U;({struct Cyc_Absyn_Exp*_tmp921=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp921,_tag_fat(_tmp38B,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 1901
struct Cyc_Absyn_Stmt*s=_tmp38A;
# 1904
s=({struct Cyc_Absyn_Stmt*_tmp924=s;Cyc_Absyn_seq_stmt(_tmp924,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp925=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp925;});}}else{
# 1908
struct Cyc_Toc_functionSet*_tmp38C=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp38C;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp929=({struct Cyc_Absyn_Exp*_tmp38D[2U];({struct Cyc_Absyn_Exp*_tmp927=Cyc_Toc_push_address_exp(e2);_tmp38D[0]=_tmp927;}),({struct Cyc_Absyn_Exp*_tmp926=Cyc_Absyn_signed_int_exp(1,0U);_tmp38D[1]=_tmp926;});({struct Cyc_Absyn_Exp*_tmp928=fn_e;Cyc_Toc_fncall_exp_r(_tmp928,_tag_fat(_tmp38D,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp929;});}}else{
# 1912
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1916
goto _LL7;}}case 4U: _LL18: _tmp34B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp34C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp34D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp34B;struct Cyc_Core_Opt*popt=_tmp34C;struct Cyc_Absyn_Exp*e2=_tmp34D;
# 1935 "toc.cyc"
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp92A=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp92A;});
# 1946
goto _LL7;}{
# 1948
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
# 1954
return;}{
# 1958
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1964
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp38F=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp31=_tmp38F;enum Cyc_Absyn_Primop _tmp390=_stmttmp31;switch(_tmp390){case Cyc_Absyn_Plus: _LL89: _LL8A:
 change=e2;goto _LL88;case Cyc_Absyn_Minus: _LL8B: _LL8C:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL88;default: _LL8D: _LL8E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp392=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D9;_tmp7D9.tag=0U,({struct _fat_ptr _tmp92B=({const char*_tmp393="bad t ? pointer arithmetic";_tag_fat(_tmp393,sizeof(char),27U);});_tmp7D9.f1=_tmp92B;});_tmp7D9;});void*_tmp391[1U];_tmp391[0]=& _tmp392;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp391,sizeof(void*),1U));});}_LL88:;}
# 1974
done=1;{
# 1976
struct Cyc_Absyn_Exp*_tmp394=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp394;
({void*_tmp92F=({struct Cyc_Absyn_Exp*_tmp395[3U];({struct Cyc_Absyn_Exp*_tmp92D=Cyc_Toc_push_address_exp(e1);_tmp395[0]=_tmp92D;}),({
struct Cyc_Absyn_Exp*_tmp92C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp395[1]=_tmp92C;}),_tmp395[2]=change;({struct Cyc_Absyn_Exp*_tmp92E=fn_e;Cyc_Toc_fncall_exp_r(_tmp92E,_tag_fat(_tmp395,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1977
e->r=_tmp92F;});}}else{
# 1980
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1983
enum Cyc_Absyn_Primop _tmp396=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp32=_tmp396;enum Cyc_Absyn_Primop _tmp397=_stmttmp32;if(_tmp397 == Cyc_Absyn_Plus){_LL90: _LL91:
# 1985
 done=1;
({void*_tmp931=({struct Cyc_Absyn_Exp*_tmp398[2U];_tmp398[0]=e1,_tmp398[1]=e2;({struct Cyc_Absyn_Exp*_tmp930=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp930,_tag_fat(_tmp398,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp931;});
goto _LL8F;}else{_LL92: _LL93:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp39A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DA;_tmp7DA.tag=0U,({struct _fat_ptr _tmp932=({const char*_tmp39B="bad zero-terminated pointer arithmetic";_tag_fat(_tmp39B,sizeof(char),39U);});_tmp7DA.f1=_tmp932;});_tmp7DA;});void*_tmp399[1U];_tmp399[0]=& _tmp39A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp399,sizeof(void*),1U));});}_LL8F:;}}}
# 1992
if(!done){
# 1994
if(e1_poly)
({void*_tmp934=({void*_tmp933=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp933,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp934;});
# 2000
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp935=e1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*f_env))Cyc_Toc_lvalue_assign)(_tmp935,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->f1=popt,_tmp39C->f2=e2;_tmp39C;}));});
e->r=e1->r;}}
# 2005
goto _LL7;}}}}case 6U: _LL1A: _tmp348=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp349=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp34A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp348;struct Cyc_Absyn_Exp*e2=_tmp349;struct Cyc_Absyn_Exp*e3=_tmp34A;
# 2007
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7U: _LL1C: _tmp346=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp347=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL1D: {struct Cyc_Absyn_Exp*e1=_tmp346;struct Cyc_Absyn_Exp*e2=_tmp347;
_tmp344=e1;_tmp345=e2;goto _LL1F;}case 8U: _LL1E: _tmp344=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp345=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp344;struct Cyc_Absyn_Exp*e2=_tmp345;
_tmp342=e1;_tmp343=e2;goto _LL21;}case 9U: _LL20: _tmp342=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp343=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp342;struct Cyc_Absyn_Exp*e2=_tmp343;
# 2014
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f3 == 0){_LL22: _tmp340=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp341=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL23: {struct Cyc_Absyn_Exp*e1=_tmp340;struct Cyc_List_List*es=_tmp341;
# 2018
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2023
({void*_tmp939=({void*_tmp938=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp938,({struct Cyc_Absyn_Exp*_tmp39D[1U];({struct Cyc_Absyn_Exp*_tmp936=
Cyc_Absyn_copy_exp(e1);_tmp39D[0]=_tmp936;});({struct Cyc_Absyn_Exp*_tmp937=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp937,_tag_fat(_tmp39D,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2023
e1->r=_tmp939;});
# 2025
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_LL24: _tmp33B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp33C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp33D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f3)->num_varargs;_tmp33E=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f3)->injectors;_tmp33F=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp300)->f3)->vai;_LL25: {struct Cyc_Absyn_Exp*e1=_tmp33B;struct Cyc_List_List*es=_tmp33C;int num_varargs=_tmp33D;struct Cyc_List_List*injectors=_tmp33E;struct Cyc_Absyn_VarargInfo*vai=_tmp33F;
# 2036 "toc.cyc"
struct _RegionHandle _tmp39E=_new_region("r");struct _RegionHandle*r=& _tmp39E;_push_region(r);{
struct _tuple30 _tmp39F=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp33=_tmp39F;struct _tuple30 _tmp3A0=_stmttmp33;struct Cyc_Absyn_Exp*_tmp3A2;struct _tuple1*_tmp3A1;_LL95: _tmp3A1=_tmp3A0.f1;_tmp3A2=_tmp3A0.f2;_LL96: {struct _tuple1*argv=_tmp3A1;struct Cyc_Absyn_Exp*argvexp=_tmp3A2;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2043
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
int num_normargs=num_args - num_varargs;
# 2047
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3A3->tl=new_args;_tmp3A3;});}}
# 2052
new_args=({struct Cyc_List_List*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct Cyc_Absyn_Exp*_tmp93C=({struct Cyc_Absyn_Exp*_tmp3A4[3U];_tmp3A4[0]=argvexp,({
# 2054
struct Cyc_Absyn_Exp*_tmp93A=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp3A4[1]=_tmp93A;}),_tmp3A4[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp93B=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp93B,_tag_fat(_tmp3A4,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2052
_tmp3A5->hd=_tmp93C;}),_tmp3A5->tl=new_args;_tmp3A5;});
# 2057
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2059
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2064
({void*_tmp940=({void*_tmp93F=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp93F,({struct Cyc_Absyn_Exp*_tmp3A6[1U];({struct Cyc_Absyn_Exp*_tmp93D=
Cyc_Absyn_copy_exp(e1);_tmp3A6[0]=_tmp93D;});({struct Cyc_Absyn_Exp*_tmp93E=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp93E,_tag_fat(_tmp3A6,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2064
e1->r=_tmp940;});{
# 2066
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2069
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp3A7=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp34=_tmp3A7;void*_tmp3A8=_stmttmp34;struct Cyc_Absyn_Datatypedecl*_tmp3A9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)->f1).KnownDatatype).tag == 2){_LL98: _tmp3A9=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)->f1).KnownDatatype).val;_LL99: {struct Cyc_Absyn_Datatypedecl*x=_tmp3A9;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A: _LL9B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DB;_tmp7DB.tag=0U,({struct _fat_ptr _tmp941=({const char*_tmp3AC="unknown datatype in vararg with inject";_tag_fat(_tmp3AC,sizeof(char),39U);});_tmp7DB.f1=_tmp941;});_tmp7DB;});void*_tmp3AA[1U];_tmp3AA[0]=& _tmp3AB;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3AA,sizeof(void*),1U));});}_LL97:;}{
# 2075
struct _fat_ptr vs=({unsigned _tmp3B8=(unsigned)num_varargs;struct _tuple1**_tmp3B7=({struct _RegionHandle*_tmp942=r;_region_malloc(_tmp942,_check_times(_tmp3B8,sizeof(struct _tuple1*)));});({{unsigned _tmp7DC=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp7DC;++ i){({struct _tuple1*_tmp943=Cyc_Toc_temp_var();_tmp3B7[i]=_tmp943;});}}0;});_tag_fat(_tmp3B7,sizeof(struct _tuple1*),_tmp3B8);});
# 2077
if(num_varargs != 0){
# 2081
struct Cyc_List_List*_tmp3AD=0;struct Cyc_List_List*array_args=_tmp3AD;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));({struct Cyc_Absyn_Exp*_tmp944=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp3AE->hd=_tmp944;}),_tmp3AE->tl=array_args;_tmp3AE;});}}
s=({struct _tuple1*_tmp947=argv;void*_tmp946=arr_type;struct Cyc_Absyn_Exp*_tmp945=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp947,_tmp946,_tmp945,s,0U);});
# 2086
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)- 1;
for(0;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp3AF=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp35=_tmp3AF;struct Cyc_Absyn_Datatypefield*_tmp3B0=_stmttmp35;struct Cyc_List_List*_tmp3B2;struct _tuple1*_tmp3B1;_LL9D: _tmp3B1=_tmp3B0->name;_tmp3B2=_tmp3B0->typs;_LL9E: {struct _tuple1*qv=_tmp3B1;struct Cyc_List_List*tqts=_tmp3B2;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2100
struct _tuple1*_tmp3B3=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp3B3;
struct Cyc_List_List*_tmp3B4=({struct _tuple19*_tmp3B6[2U];({struct _tuple19*_tmp94B=({struct _fat_ptr*_tmp94A=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp94A,Cyc_Toc_datatype_tag(tud,qv));});_tmp3B6[0]=_tmp94B;}),({
struct _tuple19*_tmp949=({struct _fat_ptr*_tmp948=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp948,arg);});_tmp3B6[1]=_tmp949;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3B6,sizeof(struct _tuple19*),2U));});
# 2101
struct Cyc_List_List*dles=_tmp3B4;
# 2103
s=({struct _tuple1*_tmp94E=var;void*_tmp94D=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp94C=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->tag=29U,_tmp3B5->f1=tdn,_tmp3B5->f2=0,_tmp3B5->f3=dles,_tmp3B5->f4=0;_tmp3B5;}),0U);
# 2103
Cyc_Absyn_declare_stmt(_tmp94E,_tmp94D,_tmp94C,s,0U);});}}}}}else{
# 2111
s=({struct _tuple1*_tmp951=argv;void*_tmp950=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp94F=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp951,_tmp950,_tmp94F,s,0U);});}}}else{
# 2116
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3B9->tl=array_args;_tmp3B9;});}{
# 2122
struct Cyc_Absyn_Exp*_tmp3BA=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp3BA;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2125
s=({struct _tuple1*_tmp954=argv;void*_tmp953=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp952=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp954,_tmp953,_tmp952,s,0U);});}}
# 2128
({void*_tmp955=Cyc_Toc_stmt_exp_r(s);e->r=_tmp955;});}}}}
# 2130
_npop_handler(0U);goto _LL7;
# 2036
;_pop_region();}}case 11U: _LL26: _tmp339=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp33A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp339;int b=_tmp33A;
# 2133
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp958=({void*_tmp957=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp957,({struct Cyc_Absyn_Exp*_tmp3BB[1U];_tmp3BB[0]=e1;({struct Cyc_Absyn_Exp*_tmp956=fn_e;Cyc_Toc_fncall_exp_dl(_tmp956,_tag_fat(_tmp3BB,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp958;});
goto _LL7;}}case 12U: _LL28: _tmp338=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL29: {struct Cyc_Absyn_Exp*e1=_tmp338;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13U: _LL2A: _tmp336=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp337=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL2B: {struct Cyc_Absyn_Exp*e1=_tmp336;struct Cyc_List_List*ts=_tmp337;
# 2139
Cyc_Toc_exp_to_c(nv,e1);
# 2148 "toc.cyc"
for(0;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp3BC=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp3BC;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp3BD=Cyc_Absyn_compress((void*)ts->hd);void*_stmttmp36=_tmp3BD;void*_tmp3BE=_stmttmp36;switch(*((int*)_tmp3BE)){case 2U: _LLA0: _LLA1:
 goto _LLA3;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f1)->tag == 18U){_LLA2: _LLA3:
 continue;}else{goto _LLA4;}default: _LLA4: _LLA5:
# 2155
({void*_tmp95A=({void*_tmp959=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp959,e1,0U);})->r;e->r=_tmp95A;});
goto _LL9F;}_LL9F:;}
# 2158
break;}}
# 2161
goto _LL7;}case 14U: _LL2C: _tmp332=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp333=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp334=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp335=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL2D: {void**t=_tmp332;struct Cyc_Absyn_Exp*e1=_tmp333;int user_inserted=_tmp334;enum Cyc_Absyn_Coercion coercion=_tmp335;
# 2163
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2170
{struct _tuple34 _tmp3BF=({struct _tuple34 _tmp7DE;({void*_tmp95C=Cyc_Absyn_compress(old_t2);_tmp7DE.f1=_tmp95C;}),({void*_tmp95B=Cyc_Absyn_compress(new_typ);_tmp7DE.f2=_tmp95B;});_tmp7DE;});struct _tuple34 _stmttmp37=_tmp3BF;struct _tuple34 _tmp3C0=_stmttmp37;struct Cyc_Absyn_PtrInfo _tmp3C1;struct Cyc_Absyn_PtrInfo _tmp3C3;struct Cyc_Absyn_PtrInfo _tmp3C2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0.f1)->tag == 3U)switch(*((int*)_tmp3C0.f2)){case 3U: _LLA7: _tmp3C2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0.f1)->f1;_tmp3C3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0.f2)->f1;_LLA8: {struct Cyc_Absyn_PtrInfo p1=_tmp3C2;struct Cyc_Absyn_PtrInfo p2=_tmp3C3;
# 2172
struct Cyc_Absyn_Exp*_tmp3C4=({void*_tmp95D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95D,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp3C4;
struct Cyc_Absyn_Exp*_tmp3C5=({void*_tmp95E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95E,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp3C5;
int _tmp3C6=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp3C6;
int _tmp3C7=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp3C7;
{struct _tuple35 _tmp3C8=({struct _tuple35 _tmp7DD;_tmp7DD.f1=b1,_tmp7DD.f2=b2;_tmp7DD;});struct _tuple35 _stmttmp38=_tmp3C8;struct _tuple35 _tmp3C9=_stmttmp38;if(_tmp3C9.f1 != 0){if(_tmp3C9.f2 != 0){_LLAE: _LLAF:
# 2179
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp961=({void*_tmp960=*t;Cyc_Toc_cast_it_r(_tmp960,({struct Cyc_Absyn_Exp*_tmp3CA[1U];_tmp3CA[0]=e1;({struct Cyc_Absyn_Exp*_tmp95F=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp95F,_tag_fat(_tmp3CA,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp961;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2188
goto _LLAD;}else{_LLB0: _LLB1: {
# 2190
struct Cyc_Absyn_Exp*_tmp3CB=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp3CB;
struct _tuple13 _tmp3CC=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp39=_tmp3CC;struct _tuple13 _tmp3CD=_stmttmp39;int _tmp3CF;unsigned _tmp3CE;_LLB7: _tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;_LLB8: {unsigned i=_tmp3CE;int valid=_tmp3CF;
if(Cyc_Toc_is_toplevel(nv)){
# 2196
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp962=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp962,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp963=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp963;});}else{
# 2201
if(zt1){
# 2206
struct _tuple30 _tmp3D0=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3A=_tmp3D0;struct _tuple30 _tmp3D1=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3D3;struct _tuple1*_tmp3D2;_LLBA: _tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;_LLBB: {struct _tuple1*x=_tmp3D2;struct Cyc_Absyn_Exp*x_exp=_tmp3D3;
struct Cyc_Absyn_Exp*arg3;
# 2210
{void*_tmp3D4=e1->r;void*_stmttmp3B=_tmp3D4;void*_tmp3D5=_stmttmp3B;struct Cyc_Absyn_Vardecl*_tmp3D6;struct Cyc_Absyn_Vardecl*_tmp3D7;switch(*((int*)_tmp3D5)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1).Wstring_c).tag){case 8U: _LLBD: _LLBE:
 arg3=e2;goto _LLBC;case 9U: _LLBF: _LLC0:
 arg3=e2;goto _LLBC;default: goto _LLC5;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1)){case 1U: _LLC1: _tmp3D7=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_tmp3D7;
_tmp3D6=vd;goto _LLC4;}case 4U: _LLC3: _tmp3D6=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D5)->f1)->f1;_LLC4: {struct Cyc_Absyn_Vardecl*vd=_tmp3D6;
# 2215
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC6;
arg3=e2;
goto _LLBC;}default: goto _LLC5;}default: _LLC5: _LLC6:
# 2222
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2225
arg3=({struct Cyc_Absyn_Exp*_tmp3D8[2U];({
struct Cyc_Absyn_Exp*_tmp965=({void*_tmp964=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp964,x_exp);});_tmp3D8[0]=_tmp965;}),_tmp3D8[1]=e2;({struct Cyc_Absyn_Exp*_tmp966=
# 2225
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp966,_tag_fat(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2227
goto _LLBC;}_LLBC:;}
# 2229
if(!zt2 && !(p2.elt_tq).real_const)
# 2232
arg3=({struct Cyc_Absyn_Exp*_tmp967=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp967,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2234
struct Cyc_Absyn_Exp*_tmp3D9=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3D9;
struct Cyc_Absyn_Exp*_tmp3DA=({struct Cyc_Absyn_Exp*_tmp3DC[3U];_tmp3DC[0]=x_exp,_tmp3DC[1]=arg2,_tmp3DC[2]=arg3;({struct Cyc_Absyn_Exp*_tmp968=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp968,_tag_fat(_tmp3DC,sizeof(struct Cyc_Absyn_Exp*),3U));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3DA;
struct Cyc_Absyn_Stmt*_tmp3DB=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3DB;
s=({struct _tuple1*_tmp96B=x;void*_tmp96A=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp969=e1;Cyc_Absyn_declare_stmt(_tmp96B,_tmp96A,_tmp969,s,0U);});
({void*_tmp96C=Cyc_Toc_stmt_exp_r(s);e->r=_tmp96C;});}}}else{
# 2241
({void*_tmp96F=({struct Cyc_Absyn_Exp*_tmp3DD[3U];_tmp3DD[0]=e1,({
# 2243
struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3DD[1]=_tmp96D;}),_tmp3DD[2]=e2;({struct Cyc_Absyn_Exp*_tmp96E=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp96E,_tag_fat(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2241
e->r=_tmp96F;});}}
# 2246
goto _LLAD;}}}}else{if(_tmp3C9.f2 != 0){_LLB2: _LLB3: {
# 2256 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3DE=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3DE;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp970=b2;Cyc_Absyn_add_exp(_tmp970,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2263
struct Cyc_Absyn_Exp*_tmp3DF=({struct Cyc_Absyn_Exp*_tmp3E1[3U];_tmp3E1[0]=e1,({
# 2265
struct Cyc_Absyn_Exp*_tmp971=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3E1[1]=_tmp971;}),_tmp3E1[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp972=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp972,_tag_fat(_tmp3E1,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2263
struct Cyc_Absyn_Exp*ptr_exp=_tmp3DF;
# 2267
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp975=({struct Cyc_Absyn_Exp*_tmp3E0[1U];({struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_copy_exp(ptr_exp);_tmp3E0[0]=_tmp973;});({struct Cyc_Absyn_Exp*_tmp974=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp974,_tag_fat(_tmp3E0,sizeof(struct Cyc_Absyn_Exp*),1U));});});ptr_exp->r=_tmp975;});
({void*_tmp976=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp976;});
goto _LLAD;}}}else{_LLB4: _LLB5:
# 2275
 if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp3E2="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3E2,sizeof(char),70U);}));
({void*_tmp97A=({struct Cyc_Absyn_Exp*_tmp3E3[3U];_tmp3E3[0]=e1,({
struct Cyc_Absyn_Exp*_tmp978=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3E3[1]=_tmp978;}),({
struct Cyc_Absyn_Exp*_tmp977=Cyc_Absyn_uint_exp(1U,0U);_tmp3E3[2]=_tmp977;});({struct Cyc_Absyn_Exp*_tmp979=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp979,_tag_fat(_tmp3E3,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2278
e->r=_tmp97A;});}
# 2282
goto _LLAD;}}_LLAD:;}
# 2284
goto _LLA6;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0.f2)->f1)->tag == 1U){_LLA9: _tmp3C1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0.f1)->f1;_LLAA: {struct Cyc_Absyn_PtrInfo p1=_tmp3C1;
# 2286
{struct Cyc_Absyn_Exp*_tmp3E4=({void*_tmp97B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp97B,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3C=_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E5=_stmttmp3C;if(_tmp3E5 == 0){_LLC8: _LLC9:
# 2288
({void*_tmp97D=({struct Cyc_Absyn_Exp*_tmp97C=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp97C,Cyc_Toc_curr_sp);});e1->r=_tmp97D;});
e1->topt=new_typ_c;
goto _LLC7;}else{_LLCA: _LLCB:
 goto _LLC7;}_LLC7:;}
# 2293
goto _LLA6;}}else{goto _LLAB;}default: goto _LLAB;}else{_LLAB: _LLAC:
# 2295
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLA6;}_LLA6:;}
# 2299
goto _LL7;}case 15U: _LL2E: _tmp331=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL2F: {struct Cyc_Absyn_Exp*e1=_tmp331;
# 2302
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2308
({void*_tmp97F=({void*_tmp97E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp97E,e1);});e->r=_tmp97F;});}else{
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2312
({void*_tmp981=({void*_tmp980=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp980,e1);});e->r=_tmp981;});}
# 2314
goto _LL7;}case 16U: _LL30: _tmp32F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp330=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL31: {struct Cyc_Absyn_Exp*rgnopt=_tmp32F;struct Cyc_Absyn_Exp*e1=_tmp330;
# 2321
Cyc_Toc_exp_to_c(nv,e1);{
# 2323
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple30 _tmp3E6=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3D=_tmp3E6;struct _tuple30 _tmp3E7=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp3E9;struct _tuple1*_tmp3E8;_LLCD: _tmp3E8=_tmp3E7.f1;_tmp3E9=_tmp3E7.f2;_LLCE: {struct _tuple1*x=_tmp3E8;struct Cyc_Absyn_Exp*xexp=_tmp3E9;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3EA=Cyc_Absyn_compress(elt_typ);void*_stmttmp3E=_tmp3EA;void*_tmp3EB=_stmttmp3E;void*_tmp3EC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB)->tag == 4U){_LLD0: _tmp3EC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB)->f1).elt_type;_LLD1: {void*t2=_tmp3EC;
# 2328
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCF;}}else{_LLD2: _LLD3:
# 2332
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCF;}_LLCF:;}{
# 2335
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3ED=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3ED;
struct Cyc_Absyn_Exp*_tmp3EE=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3EE;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3EF[2U];_tmp3EF[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp982=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3EF[1]=_tmp982;});({struct Cyc_Absyn_Exp*_tmp983=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp983,_tag_fat(_tmp3EF,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2342
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2345
struct Cyc_Absyn_Exp*_tmp3F0=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3F0;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2349
if(rgnopt == 0 || Cyc_Flags_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2352
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2355
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3F1[3U];({
struct Cyc_Absyn_Exp*_tmp986=Cyc_Absyn_copy_exp(xexp);_tmp3F1[0]=_tmp986;}),({
struct Cyc_Absyn_Exp*_tmp985=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3F1[1]=_tmp985;}),
(unsigned)lenexp?_tmp3F1[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp984=Cyc_Absyn_uint_exp(1U,0U);_tmp3F1[2]=_tmp984;});({struct Cyc_Absyn_Exp*_tmp987=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp987,_tag_fat(_tmp3F1,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2362
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp988=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->tag=Cyc_Toc_Dest,_tmp3F2->f1=xexp;_tmp3F2;});e->annot=_tmp988;});{
struct Cyc_Absyn_Stmt*_tmp3F3=({struct _tuple1*_tmp98C=x;void*_tmp98B=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp98A=mexp;Cyc_Absyn_declare_stmt(_tmp98C,_tmp98B,_tmp98A,({
struct Cyc_Absyn_Stmt*_tmp989=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp989,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2364
struct Cyc_Absyn_Stmt*s=_tmp3F3;
# 2368
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp98D=Cyc_Toc_stmt_exp_r(s);e->r=_tmp98D;});
if(vse != 0)
({void*_tmp98F=({void*_tmp98E=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp98E,Cyc_Absyn_copy_exp(e));});e->r=_tmp98F;});
goto _LL7;}}}}}}}case 18U: _LL32: _tmp32E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL33: {struct Cyc_Absyn_Exp*e1=_tmp32E;
# 2376
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17U: _LL34: _tmp32D=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL35: {void*t=_tmp32D;
({void*_tmp991=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->tag=17U,({void*_tmp990=Cyc_Toc_typ_to_c(t);_tmp3F4->f1=_tmp990;});_tmp3F4;});e->r=_tmp991;});goto _LL7;}case 19U: _LL36: _tmp32B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp32C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL37: {void*t=_tmp32B;struct Cyc_List_List*fs=_tmp32C;
# 2379
void*_tmp3F5=t;void*t2=_tmp3F5;
struct Cyc_List_List*_tmp3F6=fs;struct Cyc_List_List*l=_tmp3F6;
for(0;l != 0;l=l->tl){
void*_tmp3F7=(void*)l->hd;void*_stmttmp3F=_tmp3F7;void*_tmp3F8=_stmttmp3F;unsigned _tmp3F9;struct _fat_ptr*_tmp3FA;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F8)->tag == 0U){_LLD5: _tmp3FA=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F8)->f1;_LLD6: {struct _fat_ptr*n=_tmp3FA;
goto _LLD4;}}else{_LLD7: _tmp3F9=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3F8)->f1;_LLD8: {unsigned n=_tmp3F9;
# 2385
{void*_tmp3FB=Cyc_Absyn_compress(t2);void*_stmttmp40=_tmp3FB;void*_tmp3FC=_stmttmp40;struct Cyc_List_List*_tmp3FD;struct Cyc_List_List*_tmp3FE;struct Cyc_Absyn_Datatypefield*_tmp3FF;union Cyc_Absyn_AggrInfo _tmp400;switch(*((int*)_tmp3FC)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f1)){case 20U: _LLDA: _tmp400=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f1)->f1;_LLDB: {union Cyc_Absyn_AggrInfo info=_tmp400;
# 2387
struct Cyc_Absyn_Aggrdecl*_tmp401=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp401;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp403=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DF;_tmp7DF.tag=0U,({struct _fat_ptr _tmp992=({const char*_tmp404="struct fields must be known";_tag_fat(_tmp404,sizeof(char),28U);});_tmp7DF.f1=_tmp992;});_tmp7DF;});void*_tmp402[1U];_tmp402[0]=& _tmp403;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp402,sizeof(void*),1U));});
_tmp3FE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f1)->f1).KnownDatatypefield).tag == 2){_LLE0: _tmp3FF=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FC)->f1)->f1).KnownDatatypefield).val).f2;_LLE1: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3FF;
# 2401
if(n == (unsigned)0)
({void*_tmp993=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->tag=0U,_tmp408->f1=Cyc_Toc_tag_sp;_tmp408;}));l->hd=_tmp993;});else{
# 2404
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp995=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->tag=0U,({struct _fat_ptr*_tmp994=Cyc_Absyn_fieldname((int)n);_tmp409->f1=_tmp994;});_tmp409;}));l->hd=_tmp995;});}
# 2407
goto _LLD9;}}else{goto _LLE2;}default: goto _LLE2;}case 7U: _LLDC: _tmp3FE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3FC)->f2;_LLDD: {struct Cyc_List_List*fields=_tmp3FE;
# 2392
struct Cyc_Absyn_Aggrfield*_tmp405=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp405;
({void*_tmp996=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=0U,_tmp406->f1=nth_field->name;_tmp406;}));l->hd=_tmp996;});
t2=nth_field->type;
goto _LLD9;}case 6U: _LLDE: _tmp3FD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3FC)->f1;_LLDF: {struct Cyc_List_List*ts=_tmp3FD;
# 2397
({void*_tmp998=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->tag=0U,({struct _fat_ptr*_tmp997=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp407->f1=_tmp997;});_tmp407;}));l->hd=_tmp998;});
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD9;}default: _LLE2: _LLE3:
# 2408
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E0;_tmp7E0.tag=0U,({struct _fat_ptr _tmp999=({const char*_tmp40C="impossible type for offsetof tuple index";_tag_fat(_tmp40C,sizeof(char),41U);});_tmp7E0.f1=_tmp999;});_tmp7E0;});void*_tmp40A[1U];_tmp40A[0]=& _tmp40B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp40A,sizeof(void*),1U));});}_LLD9:;}
# 2410
goto _LLD4;}}_LLD4:;}
# 2413
({void*_tmp99B=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=19U,({void*_tmp99A=Cyc_Toc_typ_to_c(t);_tmp40D->f1=_tmp99A;}),_tmp40D->f2=fs;_tmp40D;});e->r=_tmp99B;});
goto _LL7;}case 21U: _LL38: _tmp327=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp328=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp329=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp32A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL39: {struct Cyc_Absyn_Exp*e1=_tmp327;struct _fat_ptr*f=_tmp328;int is_tagged=_tmp329;int is_read=_tmp32A;
# 2416
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp9A0=({struct Cyc_Absyn_Exp*_tmp99F=e1;void*_tmp99E=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp99D=e1_cyc_type;struct _fat_ptr*_tmp99C=f;Cyc_Toc_check_tagged_union(_tmp99F,_tmp99E,_tmp99D,_tmp99C,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2420
e->r=_tmp9A0;});
# 2422
if(is_poly)
({void*_tmp9A2=({void*_tmp9A1=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9A1,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9A2;});
goto _LL7;}case 22U: _LL3A: _tmp323=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp324=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp325=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp326=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL3B: {struct Cyc_Absyn_Exp*e1=_tmp323;struct _fat_ptr*f=_tmp324;int is_tagged=_tmp325;int is_read=_tmp326;
# 2426
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp40E=Cyc_Absyn_compress(e1typ);void*_stmttmp41=_tmp40E;void*_tmp40F=_stmttmp41;struct Cyc_Absyn_PtrInfo _tmp410;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40F)->tag == 3U){_LLE5: _tmp410=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40F)->f1;_LLE6: {struct Cyc_Absyn_PtrInfo p=_tmp410;
ta=p.elt_type;goto _LLE4;}}else{_LLE7: _LLE8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp412=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E1;_tmp7E1.tag=0U,({struct _fat_ptr _tmp9A3=({const char*_tmp413="get_ptr_typ: not a pointer!";_tag_fat(_tmp413,sizeof(char),28U);});_tmp7E1.f1=_tmp9A3;});_tmp7E1;});void*_tmp411[1U];_tmp411[0]=& _tmp412;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp411,sizeof(void*),1U));});}_LLE4:;}
# 2433
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp9A8=({struct Cyc_Absyn_Exp*_tmp9A7=e1;void*_tmp9A6=Cyc_Toc_typ_to_c(e1typ);void*_tmp9A5=ta;struct _fat_ptr*_tmp9A4=f;Cyc_Toc_check_tagged_union(_tmp9A7,_tmp9A6,_tmp9A5,_tmp9A4,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9A8;});
if(is_poly && is_read)
({void*_tmp9AA=({void*_tmp9A9=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9A9,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9AA;});
goto _LL7;}case 20U: _LL3C: _tmp322=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL3D: {struct Cyc_Absyn_Exp*e1=_tmp322;
# 2441
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23U: _LL3E: _tmp320=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp321=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL3F: {struct Cyc_Absyn_Exp*e1=_tmp320;struct Cyc_Absyn_Exp*e2=_tmp321;
# 2445
{void*_tmp414=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp42=_tmp414;void*_tmp415=_stmttmp42;struct Cyc_List_List*_tmp416;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp415)->tag == 6U){_LLEA: _tmp416=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp415)->f1;_LLEB: {struct Cyc_List_List*ts=_tmp416;
# 2447
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp417=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp417;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _tmp418=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp43=_tmp418;struct _tuple13 _tmp419=_stmttmp43;int _tmp41B;unsigned _tmp41A;_LLEF: _tmp41A=_tmp419.f1;_tmp41B=_tmp419.f2;_LLF0: {unsigned i=_tmp41A;int known=_tmp41B;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E2;_tmp7E2.tag=0U,({struct _fat_ptr _tmp9AB=({const char*_tmp41E="unknown tuple subscript in translation to C";_tag_fat(_tmp41E,sizeof(char),44U);});_tmp7E2.f1=_tmp9AB;});_tmp7E2;});void*_tmp41C[1U];_tmp41C[0]=& _tmp41D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp41C,sizeof(void*),1U));});
({void*_tmp9AD=({struct Cyc_Absyn_Exp*_tmp9AC=e1;Cyc_Toc_aggrmember_exp_r(_tmp9AC,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp9AD;});
goto _LLE9;}}}}}else{_LLEC: _LLED:
# 2457
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp9AE=Cyc_Toc_deref_exp_r(e1);e->r=_tmp9AE;});
goto _LLE9;}}_LLE9:;}
# 2463
goto _LL7;}case 24U: _LL40: _tmp31F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL41: {struct Cyc_List_List*es=_tmp31F;
# 2465
if(!Cyc_Toc_is_toplevel(nv)){
# 2467
void*_tmp41F=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple23*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp41F;
{void*_tmp420=strct_typ;union Cyc_Absyn_AggrInfo _tmp421;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->f2 == 0){_LLF2: _tmp421=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp420)->f1)->f1;_LLF3: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp421;
# 2470
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));({struct _tuple19*_tmp9B0=({struct _fat_ptr*_tmp9AF=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9AF,(struct Cyc_Absyn_Exp*)es->hd);});_tmp422->hd=_tmp9B0;}),_tmp422->tl=dles;_tmp422;});}}
# 2476
({void*_tmp9B2=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=29U,_tmp423->f1=sd->name,_tmp423->f2=0,({struct Cyc_List_List*_tmp9B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp423->f3=_tmp9B1;}),_tmp423->f4=sd;_tmp423;});e->r=_tmp9B2;});
e->topt=strct_typ;
goto _LLF1;}}else{goto _LLF4;}}else{goto _LLF4;}}else{_LLF4: _LLF5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp425=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E3;_tmp7E3.tag=0U,({struct _fat_ptr _tmp9B3=({const char*_tmp426="tuple type not an aggregate";_tag_fat(_tmp426,sizeof(char),28U);});_tmp7E3.f1=_tmp9B3;});_tmp7E3;});void*_tmp424[1U];_tmp424[0]=& _tmp425;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp424,sizeof(void*),1U));});}_LLF1:;}
# 2481
goto _LL7;}else{
# 2485
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));({struct _tuple19*_tmp9B4=({struct _tuple19*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->f1=0,_tmp427->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp427;});_tmp428->hd=_tmp9B4;}),_tmp428->tl=dles;_tmp428;});}
# 2490
({void*_tmp9B5=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9B5;});}
# 2492
goto _LL7;}case 26U: _LL42: _tmp31E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL43: {struct Cyc_List_List*dles0=_tmp31E;
# 2498
{struct Cyc_List_List*_tmp429=dles0;struct Cyc_List_List*dles=_tmp429;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9B6=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp9B6;});
goto _LL7;}case 27U: _LL44: _tmp31A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp31D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL45: {struct Cyc_Absyn_Vardecl*vd=_tmp31A;struct Cyc_Absyn_Exp*e1=_tmp31B;struct Cyc_Absyn_Exp*e2=_tmp31C;int iszeroterm=_tmp31D;
# 2506
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp42A=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _stmttmp44=_tmp42A;struct _tuple13 _tmp42B=_stmttmp44;int _tmp42D;unsigned _tmp42C;_LLF7: _tmp42C=_tmp42B.f1;_tmp42D=_tmp42B.f2;_LLF8: {unsigned sz=_tmp42C;int known=_tmp42D;
void*_tmp42E=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp42E;
struct Cyc_List_List*es=0;
# 2513
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp42F=0U;({unsigned _tmp9B8=e1->loc;struct _fat_ptr _tmp9B7=({const char*_tmp430="cannot determine value of constant";_tag_fat(_tmp430,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9B8,_tmp9B7,_tag_fat(_tmp42F,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));({struct _tuple19*_tmp9B9=({struct _tuple19*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->f1=0,_tmp431->f2=e2;_tmp431;});_tmp432->hd=_tmp9B9;}),_tmp432->tl=es;_tmp432;});}}
# 2519
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp433=({void*_tmp9BA=elt_typ;Cyc_Toc_cast_it(_tmp9BA,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp433;
es=({struct Cyc_List_List*_tmp9BC=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9BC,({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*_tmp435));({struct _tuple19*_tmp9BB=({struct _tuple19*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->f1=0,_tmp434->f2=ezero;_tmp434;});_tmp435->hd=_tmp9BB;}),_tmp435->tl=0;_tmp435;}));});}}
# 2524
({void*_tmp9BD=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9BD;});}}
# 2526
goto _LL7;}case 28U: _LL46: _tmp317=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp318=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp319=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_LL47: {struct Cyc_Absyn_Exp*e1=_tmp317;void*t1=_tmp318;int iszeroterm=_tmp319;
# 2533
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9BE=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9BE;});else{
# 2536
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30U: _LL48: _tmp315=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp316=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL49: {void*st=_tmp315;struct Cyc_List_List*dles=_tmp316;
# 2540
{struct Cyc_List_List*_tmp436=dles;struct Cyc_List_List*dles2=_tmp436;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*_tmp437=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp437;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp438=Cyc_Absyn_compress(strct_typ);void*_stmttmp45=_tmp438;void*_tmp439=_stmttmp45;union Cyc_Absyn_AggrInfo _tmp43A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->f2 == 0){_LLFA: _tmp43A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp439)->f1)->f1;_LLFB: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp43A;
# 2547
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp9BF=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->tag=29U,_tmp43B->f1=sd->name,_tmp43B->f2=0,_tmp43B->f3=dles,_tmp43B->f4=sd;_tmp43B;});e->r=_tmp9BF;});
e->topt=strct_typ;
goto _LLF9;}}else{goto _LLFC;}}else{goto _LLFC;}}else{_LLFC: _LLFD:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E4;_tmp7E4.tag=0U,({struct _fat_ptr _tmp9C0=({const char*_tmp43E="anonStruct type not an aggregate";_tag_fat(_tmp43E,sizeof(char),33U);});_tmp7E4.f1=_tmp9C0;});_tmp7E4;});void*_tmp43C[1U];_tmp43C[0]=& _tmp43D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43C,sizeof(void*),1U));});}_LLF9:;}else{
# 2554
({void*_tmp9C1=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp9C1;});}
goto _LL7;}}case 29U: _LL4A: _tmp311=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp312=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp313=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp314=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL4B: {struct _tuple1**tdn=_tmp311;struct Cyc_List_List*exist_ts=_tmp312;struct Cyc_List_List*dles=_tmp313;struct Cyc_Absyn_Aggrdecl*sd=_tmp314;
# 2560
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp440=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E5;_tmp7E5.tag=0U,({struct _fat_ptr _tmp9C2=({const char*_tmp441="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp441,sizeof(char),58U);});_tmp7E5.f1=_tmp9C2;});_tmp7E5;});void*_tmp43F[1U];_tmp43F[0]=& _tmp440;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43F,sizeof(void*),1U));});{
void*_tmp442=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp442;
{void*_tmp443=Cyc_Absyn_compress(new_typ);void*_stmttmp46=_tmp443;void*_tmp444=_stmttmp46;union Cyc_Absyn_AggrInfo _tmp445;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->f1)->tag == 20U){_LLFF: _tmp445=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp444)->f1)->f1;_LL100: {union Cyc_Absyn_AggrInfo info=_tmp445;
({struct _tuple1*_tmp9C3=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp9C3;});goto _LLFE;}}else{goto _LL101;}}else{_LL101: _LL102:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp447=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E6;_tmp7E6.tag=0U,({struct _fat_ptr _tmp9C4=({const char*_tmp448="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp448,sizeof(char),44U);});_tmp7E6.f1=_tmp9C4;});_tmp7E6;});void*_tmp446[1U];_tmp446[0]=& _tmp447;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp446,sizeof(void*),1U));});}_LLFE:;}{
# 2571
struct Cyc_List_List*_tmp449=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp449;
if(typ_fields == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp44A=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp44A;
struct Cyc_List_List*_tmp44B=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp44B;
# 2578
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp44C=Cyc_Absyn_compress(old_typ);void*_stmttmp47=_tmp44C;void*_tmp44D=_stmttmp47;struct Cyc_List_List*_tmp44E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44D)->f1)->tag == 20U){_LL104: _tmp44E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44D)->f2;_LL105: {struct Cyc_List_List*param_ts=_tmp44E;
# 2582
{struct Cyc_List_List*_tmp44F=fields;struct Cyc_List_List*fields2=_tmp44F;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_tmp450=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp48=_tmp450;struct _tuple36*_tmp451=_stmttmp48;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Aggrfield*_tmp452;_LL109: _tmp452=_tmp451->f1;_tmp453=_tmp451->f2;_LL10A: {struct Cyc_Absyn_Aggrfield*aggrfield=_tmp452;struct Cyc_Absyn_Exp*fieldexp=_tmp453;
void*_tmp454=fieldexp->topt;void*old_field_typ=_tmp454;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2587
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp9C6=({void*_tmp9C5=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp9C5,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2589
fieldexp->r=_tmp9C6;});
# 2592
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp455=aggrfield->name;struct _fat_ptr*n=_tmp455;
struct Cyc_Absyn_Exp*_tmp456=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp456;
struct _tuple19*_tmp457=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp457;
struct _tuple19*_tmp458=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple19*val_dle=_tmp458;
struct _tuple1*s=({struct _tuple1*_tmp463=_cycalloc(sizeof(*_tmp463));({union Cyc_Absyn_Nmspace _tmp9CA=Cyc_Absyn_Abs_n(0,1);_tmp463->f1=_tmp9CA;}),({
struct _fat_ptr*_tmp9C9=({struct _fat_ptr*_tmp462=_cycalloc(sizeof(*_tmp462));({struct _fat_ptr _tmp9C8=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp460=({struct Cyc_String_pa_PrintArg_struct _tmp7E8;_tmp7E8.tag=0U,_tmp7E8.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp7E8;});struct Cyc_String_pa_PrintArg_struct _tmp461=({struct Cyc_String_pa_PrintArg_struct _tmp7E7;_tmp7E7.tag=0U,_tmp7E7.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp7E7;});void*_tmp45E[2U];_tmp45E[0]=& _tmp460,_tmp45E[1]=& _tmp461;({struct _fat_ptr _tmp9C7=({const char*_tmp45F="_union_%s_%s";_tag_fat(_tmp45F,sizeof(char),13U);});Cyc_aprintf(_tmp9C7,_tag_fat(_tmp45E,sizeof(void*),2U));});});*_tmp462=_tmp9C8;});_tmp462;});_tmp463->f2=_tmp9C9;});_tmp463;});
# 2600
struct _tuple19*_tmp459=({
struct _fat_ptr*_tmp9CC=n;Cyc_Toc_make_field(_tmp9CC,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->tag=29U,_tmp45D->f1=s,_tmp45D->f2=0,({struct Cyc_List_List*_tmp9CB=({struct _tuple19*_tmp45C[2U];_tmp45C[0]=tag_dle,_tmp45C[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp45C,sizeof(struct _tuple19*),2U));});_tmp45D->f3=_tmp9CB;}),_tmp45D->f4=0;_tmp45D;}),0U));});
# 2600
struct _tuple19*u_dle=_tmp459;
# 2603
({void*_tmp9CE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=29U,_tmp45B->f1=*tdn,_tmp45B->f2=0,({struct Cyc_List_List*_tmp9CD=({struct _tuple19*_tmp45A[1U];_tmp45A[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp45A,sizeof(struct _tuple19*),1U));});_tmp45B->f3=_tmp9CD;}),_tmp45B->f4=sd;_tmp45B;});e->r=_tmp9CE;});}
# 2609
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp464=({struct Cyc_List_List*_tmp9CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9CF,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(sd->tvs,param_ts));});
# 2610
struct Cyc_List_List*inst=_tmp464;
# 2612
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp465=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp465;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp466=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp466;
void*_tmp467=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp467;
struct Cyc_Absyn_Aggrfield*_tmp468=({struct Cyc_Absyn_Aggrfield*_tmp9D0=old_f;Cyc_Toc_aggrfield_to_c(_tmp9D0,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2616
struct Cyc_Absyn_Aggrfield*new_f=_tmp468;
# 2618
new_fields=({struct Cyc_List_List*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->hd=new_f,_tmp469->tl=new_fields;_tmp469;});
# 2624
if(fs->tl == 0){
{void*_tmp46A=Cyc_Absyn_compress(new_f->type);void*_stmttmp49=_tmp46A;void*_tmp46B=_stmttmp49;struct Cyc_Absyn_ArrayInfo _tmp46C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46B)->tag == 4U){_LL10C: _tmp46C=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46B)->f1;_LL10D: {struct Cyc_Absyn_ArrayInfo ai=_tmp46C;
# 2627
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp46D=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp46D;
({struct Cyc_Absyn_Exp*_tmp9D1=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp9D1;});
({void*_tmp9D2=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=4U,_tmp46E->f1=ai2;_tmp46E;});new_f->type=_tmp9D2;});}
# 2632
goto _LL10B;}}else{_LL10E: _LL10F:
# 2636
 if(fieldexp->topt == 0)
goto _LL10B;
{void*_tmp46F=Cyc_Absyn_compress((void*)_check_null(fieldexp->topt));void*_stmttmp4A=_tmp46F;void*_tmp470=_stmttmp4A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->tag == 20U){_LL111: _LL112:
# 2640
 new_f->type=(void*)_check_null(fieldexp->topt);goto _LL110;}else{goto _LL113;}}else{_LL113: _LL114:
 goto _LL110;}_LL110:;}
# 2643
goto _LL10B;}_LL10B:;}
# 2647
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp9D4=({struct Cyc_List_List*_tmp472=_cycalloc(sizeof(*_tmp472));({void*_tmp9D3=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=6U,_tmp471->f1=0;_tmp471;});_tmp472->hd=_tmp9D3;}),_tmp472->tl=new_f->attributes;_tmp472;});new_f->attributes=_tmp9D4;});}}}
# 2653
sd=({struct _fat_ptr*_tmp9D7=({struct _fat_ptr*_tmp476=_cycalloc(sizeof(*_tmp476));({struct _fat_ptr _tmp9D6=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp475=({struct Cyc_Int_pa_PrintArg_struct _tmp7E9;_tmp7E9.tag=1U,_tmp7E9.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7E9;});void*_tmp473[1U];_tmp473[0]=& _tmp475;({struct _fat_ptr _tmp9D5=({const char*_tmp474="_genStruct%d";_tag_fat(_tmp474,sizeof(char),13U);});Cyc_aprintf(_tmp9D5,_tag_fat(_tmp473,sizeof(void*),1U));});});*_tmp476=_tmp9D6;});_tmp476;});Cyc_Toc_make_c_struct_defn(_tmp9D7,
# 2655
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2659
({void*_tmp9D8=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp477=_cycalloc(sizeof(*_tmp477));*_tmp477=sd;_tmp477;})),0);e->topt=_tmp9D8;});}}}}
# 2662
goto _LL103;}}else{goto _LL106;}}else{_LL106: _LL107:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp479=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EA;_tmp7EA.tag=0U,({struct _fat_ptr _tmp9D9=({const char*_tmp47A="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp47A,sizeof(char),39U);});_tmp7EA.f1=_tmp9D9;});_tmp7EA;});void*_tmp478[1U];_tmp478[0]=& _tmp479;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp478,sizeof(void*),1U));});}_LL103:;}else{
# 2671
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2673
struct _tuple36*_tmp47B=(struct _tuple36*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple36*_stmttmp4B=_tmp47B;struct _tuple36*_tmp47C=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Aggrfield*_tmp47D;_LL116: _tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;_LL117: {struct Cyc_Absyn_Aggrfield*field=_tmp47D;struct Cyc_Absyn_Exp*fieldexp=_tmp47E;
void*_tmp47F=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp47F;
void*_tmp480=field->type;void*fieldtyp=_tmp480;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp9DB=({
void*_tmp9DA=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp9DA,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2679
fieldexp->r=_tmp9DB;});{
# 2682
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp481=({struct _tuple19*_tmp483[2U];({struct _tuple19*_tmp9DD=({struct _tuple19*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->f1=0,_tmp484->f2=field_tag_exp;_tmp484;});_tmp483[0]=_tmp9DD;}),({struct _tuple19*_tmp9DC=({struct _tuple19*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->f1=0,_tmp485->f2=fieldexp;_tmp485;});_tmp483[1]=_tmp9DC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp483,sizeof(struct _tuple19*),2U));});struct Cyc_List_List*newdles=_tmp481;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp9DF=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp482[1U];({struct _tuple19*_tmp9DE=Cyc_Toc_make_field(field->name,umem);_tmp482[0]=_tmp9DE;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp482,sizeof(struct _tuple19*),1U));}));e->r=_tmp9DF;});}}}else{
# 2689
struct Cyc_List_List*_tmp486=0;struct Cyc_List_List*newdles=_tmp486;
struct Cyc_List_List*_tmp487=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp487;
for(0;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp488=fields;struct Cyc_List_List*fields2=_tmp488;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_tmp489=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp4C=_tmp489;struct _tuple36*_tmp48A=_stmttmp4C;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Aggrfield*_tmp48B;_LL119: _tmp48B=_tmp48A->f1;_tmp48C=_tmp48A->f2;_LL11A: {struct Cyc_Absyn_Aggrfield*field=_tmp48B;struct Cyc_Absyn_Exp*fieldexp=_tmp48C;
void*_tmp48D=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp48D;
void*_tmp48E=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp48E;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2699
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2701
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp9E0=fieldtyp;Cyc_Toc_cast_it(_tmp9E0,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp490=_cycalloc(sizeof(*_tmp490));({struct _tuple19*_tmp9E1=({struct _tuple19*_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->f1=0,_tmp48F->f2=fieldexp;_tmp48F;});_tmp490->hd=_tmp9E1;}),_tmp490->tl=newdles;_tmp490;});
break;}}}}
# 2708
({void*_tmp9E2=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(newdles));e->r=_tmp9E2;});}}
# 2711
goto _LL7;}}}}case 31U: _LL4C: _tmp30E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp30F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp310=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_LL4D: {struct Cyc_List_List*es=_tmp30E;struct Cyc_Absyn_Datatypedecl*tud=_tmp30F;struct Cyc_Absyn_Datatypefield*tuf=_tmp310;
# 2715
struct Cyc_List_List*_tmp491=tuf->typs;struct Cyc_List_List*tqts=_tmp491;
{struct Cyc_List_List*_tmp492=es;struct Cyc_List_List*es2=_tmp492;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp493=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp493;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp9E4=({void*_tmp9E3=field_typ;Cyc_Toc_cast_it(_tmp9E3,Cyc_Absyn_copy_exp(cur_e));})->r;cur_e->r=_tmp9E4;});}}{
# 2726
struct Cyc_Absyn_Exp*_tmp494=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2726
struct Cyc_Absyn_Exp*tag_exp=_tmp494;
# 2729
if(!Cyc_Toc_is_toplevel(nv)){
# 2731
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _tuple19*_tmp9E6=({struct _fat_ptr*_tmp9E5=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9E5,(struct Cyc_Absyn_Exp*)es->hd);});_tmp495->hd=_tmp9E6;}),_tmp495->tl=dles;_tmp495;});}}{
# 2737
struct _tuple19*_tmp496=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp496;
({void*_tmp9EA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->tag=29U,({struct _tuple1*_tmp9E9=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp498->f1=_tmp9E9;}),_tmp498->f2=0,({
struct Cyc_List_List*_tmp9E8=({struct Cyc_List_List*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->hd=tag_dle,({struct Cyc_List_List*_tmp9E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp497->tl=_tmp9E7;});_tmp497;});_tmp498->f3=_tmp9E8;}),_tmp498->f4=0;_tmp498;});
# 2738
e->r=_tmp9EA;});}}else{
# 2743
struct Cyc_List_List*_tmp499=0;struct Cyc_List_List*dles=_tmp499;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));({struct _tuple19*_tmp9EB=({struct _tuple19*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->f1=0,_tmp49A->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp49A;});_tmp49B->hd=_tmp9EB;}),_tmp49B->tl=dles;_tmp49B;});}
({void*_tmp9EE=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp49D=_cycalloc(sizeof(*_tmp49D));({struct _tuple19*_tmp9ED=({struct _tuple19*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->f1=0,_tmp49C->f2=tag_exp;_tmp49C;});_tmp49D->hd=_tmp9ED;}),({
struct Cyc_List_List*_tmp9EC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp49D->tl=_tmp9EC;});_tmp49D;}));
# 2746
e->r=_tmp9EE;});}
# 2749
goto _LL7;}}case 32U: _LL4E: _LL4F:
# 2751
 goto _LL51;case 33U: _LL50: _LL51:
 goto _LL7;case 34U: _LL52: _tmp308=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp300)->f1).is_calloc;_tmp309=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp300)->f1).rgn;_tmp30A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp300)->f1).elt_type;_tmp30B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp300)->f1).num_elts;_tmp30C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp300)->f1).fat_result;_tmp30D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp300)->f1).inline_call;_LL53: {int is_calloc=_tmp308;struct Cyc_Absyn_Exp*rgnopt=_tmp309;void**topt=_tmp30A;struct Cyc_Absyn_Exp*num_elts=_tmp30B;int is_fat=_tmp30C;int inline_call=_tmp30D;
# 2755
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(topt)));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2764
if(is_calloc){
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp49E[3U];_tmp49E[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp9EF=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp49E[1]=_tmp9EF;}),_tmp49E[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9F0=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp9F0,_tag_fat(_tmp49E,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2770
pexp=({void*_tmp9F2=*topt;struct Cyc_Absyn_Exp*_tmp9F1=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9F2,_tmp9F1,num_elts2);});}}else{
# 2772
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
if(inline_call)
pexp=Cyc_Toc_rmalloc_inline_exp(rgnopt,num_elts2);else{
# 2777
pexp=Cyc_Toc_rmalloc_exp(rgnopt,num_elts2);}}else{
# 2779
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2781
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp49F[3U];_tmp49F[0]=pexp,_tmp49F[1]=elt_sz,_tmp49F[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9F3=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp9F3,_tag_fat(_tmp49F,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp9F6=(struct _tuple1*)_check_null(x);void*_tmp9F5=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9F4=num_elts;Cyc_Absyn_declare_stmt(_tmp9F6,_tmp9F5,_tmp9F4,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp9F7=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9F7;});}else{
# 2788
e->r=pexp->r;}
goto _LL7;}}case 35U: _LL54: _tmp306=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp307=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp306;struct Cyc_Absyn_Exp*e2=_tmp307;
# 2798
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EB;_tmp7EB.tag=0U,({struct _fat_ptr _tmp9F8=({const char*_tmp4A2="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp4A2,sizeof(char),57U);});_tmp7EB.f1=_tmp9F8;});_tmp7EB;});void*_tmp4A0[1U];_tmp4A0[0]=& _tmp4A1;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4A0,sizeof(void*),1U));});{
# 2806
unsigned _tmp4A3=e->loc;unsigned loc=_tmp4A3;
struct _tuple1*_tmp4A4=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp4A4;
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp4A5;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp4A6=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp4A6;
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp4A7;v2e->topt=e2_old_typ;{
# 2812
struct Cyc_Absyn_Exp*_tmp4A8=({struct Cyc_Absyn_Exp*_tmp9FA=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp9F9=v2e;Cyc_Absyn_assign_exp(_tmp9FA,_tmp9F9,loc);});struct Cyc_Absyn_Exp*s1e=_tmp4A8;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A9=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp4A9;
struct Cyc_Absyn_Exp*_tmp4AA=({struct Cyc_Absyn_Exp*_tmp9FC=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp9FB=v1e;Cyc_Absyn_assign_exp(_tmp9FC,_tmp9FB,loc);});struct Cyc_Absyn_Exp*s2e=_tmp4AA;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4AB=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp4AB;
# 2817
struct Cyc_Absyn_Stmt*_tmp4AC=({struct _tuple1*_tmpA04=v1;void*_tmpA03=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA02=e1;struct Cyc_Absyn_Stmt*_tmpA01=({
struct _tuple1*_tmpA00=v2;void*_tmp9FF=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9FE=e2;struct Cyc_Absyn_Stmt*_tmp9FD=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2818
Cyc_Absyn_declare_stmt(_tmpA00,_tmp9FF,_tmp9FE,_tmp9FD,loc);});
# 2817
Cyc_Absyn_declare_stmt(_tmpA04,_tmpA03,_tmpA02,_tmpA01,loc);});struct Cyc_Absyn_Stmt*s=_tmp4AC;
# 2820
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmpA05=Cyc_Toc_stmt_exp_r(s);e->r=_tmpA05;});
goto _LL7;}}}}}}case 38U: _LL56: _tmp304=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp305=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL57: {struct Cyc_Absyn_Exp*e1=_tmp304;struct _fat_ptr*f=_tmp305;
# 2825
void*_tmp4AD=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp4AD;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp4AE=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp4AF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AE)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AE)->f1)->f1).KnownAggr).tag == 2){_LL11C: _tmp4AF=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AE)->f1)->f1).KnownAggr).val;_LL11D: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4AF;
# 2829
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp4B0;
struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp4B1;
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp4B2;
({void*_tmpA06=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmpA06;});
goto _LL11B;}}else{goto _LL11E;}}else{goto _LL11E;}}else{_LL11E: _LL11F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EC;_tmp7EC.tag=0U,({struct _fat_ptr _tmpA07=({const char*_tmp4B5="non-aggregate type in tagcheck";_tag_fat(_tmp4B5,sizeof(char),31U);});_tmp7EC.f1=_tmpA07;});_tmp7EC;});void*_tmp4B3[1U];_tmp4B3[0]=& _tmp4B4;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B3,sizeof(void*),1U));});}_LL11B:;}
# 2836
goto _LL7;}case 37U: _LL58: _tmp303=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL59: {struct Cyc_Absyn_Stmt*s=_tmp303;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36U: _LL5A: _LL5B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7ED;_tmp7ED.tag=0U,({struct _fat_ptr _tmpA08=({const char*_tmp4B8="UnresolvedMem";_tag_fat(_tmp4B8,sizeof(char),14U);});_tmp7ED.f1=_tmpA08;});_tmp7ED;});void*_tmp4B6[1U];_tmp4B6[0]=& _tmp4B7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B6,sizeof(void*),1U));});case 25U: _LL5C: _LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EE;_tmp7EE.tag=0U,({struct _fat_ptr _tmpA09=({const char*_tmp4BB="compoundlit";_tag_fat(_tmp4BB,sizeof(char),12U);});_tmp7EE.f1=_tmpA09;});_tmp7EE;});void*_tmp4B9[1U];_tmp4B9[0]=& _tmp4BA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B9,sizeof(void*),1U));});case 39U: _LL5E: _LL5F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EF;_tmp7EF.tag=0U,({struct _fat_ptr _tmpA0A=({const char*_tmp4BE="valueof(-)";_tag_fat(_tmp4BE,sizeof(char),11U);});_tmp7EF.f1=_tmpA0A;});_tmp7EF;});void*_tmp4BC[1U];_tmp4BC[0]=& _tmp4BD;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4BC,sizeof(void*),1U));});default: _LL60: _tmp301=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp302=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL61: {struct Cyc_List_List*o=_tmp301;struct Cyc_List_List*i=_tmp302;
# 2842
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2846
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F1;_tmp7F1.tag=0U,({struct _fat_ptr _tmpA0B=({const char*_tmp4C2="Toc did not examine an inserted check: ";_tag_fat(_tmp4C2,sizeof(char),40U);});_tmp7F1.f1=_tmpA0B;});_tmp7F1;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4C1=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7F0;_tmp7F0.tag=4U,_tmp7F0.f1=e;_tmp7F0;});void*_tmp4BF[2U];_tmp4BF[0]=& _tmp4C0,_tmp4BF[1]=& _tmp4C1;({unsigned _tmpA0C=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmpA0C,_tag_fat(_tmp4BF,sizeof(void*),2U));});});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2875 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2877
return({struct _tuple37*_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3));_tmp4C3->f1=0,({struct _fat_ptr*_tmpA0E=Cyc_Toc_fresh_label();_tmp4C3->f2=_tmpA0E;}),({struct _fat_ptr*_tmpA0D=Cyc_Toc_fresh_label();_tmp4C3->f3=_tmpA0D;}),_tmp4C3->f4=sc;_tmp4C3;});}
# 2883
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4C4=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp4C4;
# 2889
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4C7=p->orig_pat;if((_tmp4C7.pattern).tag != 1)_throw_match();(_tmp4C7.pattern).val;})->topt);
void*_tmp4C5=Cyc_Absyn_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp4C5;
void*_tmp4C6=t2;switch(*((int*)_tmp4C6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C6)->f1)){case 0U: _LL1: _LL2:
# 2894
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmpA0F=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA0F,v);});goto _LL0;}_LL0:;}{
# 2900
void*_tmp4C8=p->access;void*_stmttmp4D=_tmp4C8;void*_tmp4C9=_stmttmp4D;struct _fat_ptr*_tmp4CB;int _tmp4CA;unsigned _tmp4CC;unsigned _tmp4CD;switch(*((int*)_tmp4C9)){case 0U: _LLA: _LLB:
# 2904
 goto _LL9;case 1U: _LLC: _LLD:
# 2909
 if(ps->tl != 0){
void*_tmp4CE=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4E=_tmp4CE;void*_tmp4CF=_stmttmp4E;unsigned _tmp4D2;struct Cyc_Absyn_Datatypefield*_tmp4D1;struct Cyc_Absyn_Datatypedecl*_tmp4D0;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CF)->tag == 3U){_LL15: _tmp4D0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CF)->f1;_tmp4D1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CF)->f2;_tmp4D2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CF)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4D0;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D1;unsigned i=_tmp4D2;
# 2912
ps=ps->tl;
v=({void*_tmpA11=({void*_tmpA10=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA10,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA11,v);});
v=({struct Cyc_Absyn_Exp*_tmpA12=v;Cyc_Absyn_aggrarrow_exp(_tmpA12,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2919
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp4CD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C9)->f3;_LLF: {unsigned i=_tmp4CD;
_tmp4CC=i;goto _LL11;}case 2U: _LL10: _tmp4CC=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4C9)->f1;_LL11: {unsigned i=_tmp4CC;
v=({struct Cyc_Absyn_Exp*_tmpA13=v;Cyc_Toc_member_exp(_tmpA13,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _LL12: _tmp4CA=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C9)->f1;_tmp4CB=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C9)->f2;_LL13: {int tagged=_tmp4CA;struct _fat_ptr*f=_tmp4CB;
# 2924
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2930
return v;}
# 2935
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4D3=t;struct Cyc_Absyn_Datatypefield*_tmp4D5;struct Cyc_Absyn_Datatypedecl*_tmp4D4;int _tmp4D7;struct _fat_ptr*_tmp4D6;struct Cyc_Absyn_Datatypefield*_tmp4DA;struct Cyc_Absyn_Datatypedecl*_tmp4D9;int _tmp4D8;unsigned _tmp4DB;int _tmp4DD;struct _fat_ptr _tmp4DC;struct Cyc_Absyn_Enumfield*_tmp4DF;void*_tmp4DE;struct Cyc_Absyn_Enumfield*_tmp4E1;struct Cyc_Absyn_Enumdecl*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E2;switch(*((int*)_tmp4D3)){case 0U: _LL1: _tmp4E2=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4D3)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp4E2;
return e == 0?v: e;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA14=v;Cyc_Absyn_eq_exp(_tmpA14,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA15=v;Cyc_Absyn_neq_exp(_tmpA15,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4E0=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D3)->f1;_tmp4E1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D3)->f2;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp4E0;struct Cyc_Absyn_Enumfield*ef=_tmp4E1;
return({struct Cyc_Absyn_Exp*_tmpA16=v;Cyc_Absyn_eq_exp(_tmpA16,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->tag=32U,_tmp4E3->f1=ed,_tmp4E3->f2=ef;_tmp4E3;}),0U),0U);});}case 4U: _LL9: _tmp4DE=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D3)->f1;_tmp4DF=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D3)->f2;_LLA: {void*t=_tmp4DE;struct Cyc_Absyn_Enumfield*ef=_tmp4DF;
return({struct Cyc_Absyn_Exp*_tmpA17=v;Cyc_Absyn_eq_exp(_tmpA17,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->tag=33U,_tmp4E4->f1=t,_tmp4E4->f2=ef;_tmp4E4;}),0U),0U);});}case 5U: _LLB: _tmp4DC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4D3)->f1;_tmp4DD=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4D3)->f2;_LLC: {struct _fat_ptr s=_tmp4DC;int i=_tmp4DD;
return({struct Cyc_Absyn_Exp*_tmpA18=v;Cyc_Absyn_eq_exp(_tmpA18,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6U: _LLD: _tmp4DB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4D3)->f1;_LLE: {unsigned i=_tmp4DB;
return({struct Cyc_Absyn_Exp*_tmpA19=v;Cyc_Absyn_eq_exp(_tmpA19,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7U: _LLF: _tmp4D8=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D3)->f1;_tmp4D9=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D3)->f2;_tmp4DA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D3)->f3;_LL10: {int i=_tmp4D8;struct Cyc_Absyn_Datatypedecl*tud=_tmp4D9;struct Cyc_Absyn_Datatypefield*tuf=_tmp4DA;
# 2947
LOOP1: {
void*_tmp4E5=v->r;void*_stmttmp4F=_tmp4E5;void*_tmp4E6=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;switch(*((int*)_tmp4E6)){case 14U: _LL16: _tmp4E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E6)->f2;_LL17: {struct Cyc_Absyn_Exp*e=_tmp4E8;
v=e;goto LOOP1;}case 20U: _LL18: _tmp4E7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E6)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp4E7;
v=e;goto _LL15;}default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2954
v=({void*_tmpA1B=({void*_tmpA1A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA1A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA1B,v);});return({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA1C,Cyc_Absyn_uint_exp((unsigned)i,0U),0U);});}case 8U: _LL11: _tmp4D6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D3)->f1;_tmp4D7=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D3)->f2;_LL12: {struct _fat_ptr*f=_tmp4D6;int i=_tmp4D7;
# 2958
return({struct Cyc_Absyn_Exp*_tmpA1E=({struct Cyc_Absyn_Exp*_tmpA1D=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmpA1D,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmpA1E,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _LL13: _tmp4D4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4D3)->f1;_tmp4D5=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4D3)->f2;_LL14: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4D4;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D5;
# 2962
LOOP2: {
void*_tmp4E9=v->r;void*_stmttmp50=_tmp4E9;void*_tmp4EA=_stmttmp50;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;switch(*((int*)_tmp4EA)){case 14U: _LL1D: _tmp4EC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4EA)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4EC;
v=e;goto LOOP2;}case 20U: _LL1F: _tmp4EB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4EA)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4EB;
v=e;goto _LL1C;}default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2969
v=({void*_tmpA20=({void*_tmpA1F=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA1F,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA20,v);});
return({struct Cyc_Absyn_Exp*_tmpA21=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA21,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2981
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple38{int f1;void*f2;};
# 2988
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2990
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _tmp4ED=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp51=_tmp4ED;struct _tuple34 _tmp4EE=_stmttmp51;void*_tmp4EF;_LL1: _tmp4EF=_tmp4EE.f1;_LL2: {void*ptest=_tmp4EF;
void*_tmp4F0=ptest;struct Cyc_Absyn_Exp*_tmp4F1;struct _fat_ptr*_tmp4F2;switch(*((int*)_tmp4F0)){case 3U: _LL4: _LL5:
# 2996
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4F2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4F0)->f1;_LLB: {struct _fat_ptr*f=_tmp4F2;
# 3000
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->tag=3U,_tmp4F3->f1=f;_tmp4F3;});
continue;}case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4F1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4F0)->f1;if(_tmp4F1 != 0){_LLF: {struct Cyc_Absyn_Exp*e=_tmp4F1;
# 3005
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->tag=2U,_tmp4F4->f1=e;_tmp4F4;});return({struct _tuple38 _tmp7F2;_tmp7F2.f1=0,_tmp7F2.f2=k;_tmp7F2;});}}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple38 _tmp7F3;_tmp7F3.f1=0,_tmp7F3.f2=k;_tmp7F3;});}_LL3:;}}
# 3013
return({struct _tuple38 _tmp7F4;_tmp7F4.f1=c,_tmp7F4.f2=k;_tmp7F4;});}
# 3018
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4F5=p;int _tmp4F6;int _tmp4F7;unsigned _tmp4F8;struct Cyc_Absyn_Enumfield*_tmp4FA;void*_tmp4F9;struct Cyc_Absyn_Enumfield*_tmp4FC;struct Cyc_Absyn_Enumdecl*_tmp4FB;switch(*((int*)_tmp4F5)){case 3U: _LL1: _tmp4FB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F5)->f1;_tmp4FC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F5)->f2;_LL2: {struct Cyc_Absyn_Enumdecl*ed=_tmp4FB;struct Cyc_Absyn_Enumfield*ef=_tmp4FC;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->tag=32U,_tmp4FD->f1=ed,_tmp4FD->f2=ef;_tmp4FD;}),0U);goto _LL0;}case 4U: _LL3: _tmp4F9=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F5)->f1;_tmp4FA=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F5)->f2;_LL4: {void*t=_tmp4F9;struct Cyc_Absyn_Enumfield*ef=_tmp4FA;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->tag=33U,_tmp4FE->f1=t,_tmp4FE->f2=ef;_tmp4FE;}),0U);goto _LL0;}case 6U: _LL5: _tmp4F8=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4F5)->f1;_LL6: {unsigned i=_tmp4F8;
_tmp4F7=(int)i;goto _LL8;}case 7U: _LL7: _tmp4F7=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4F5)->f1;_LL8: {int i=_tmp4F7;
_tmp4F6=i;goto _LLA;}case 8U: _LL9: _tmp4F6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4F5)->f2;_LLA: {int i=_tmp4F6;
e=Cyc_Absyn_uint_exp((unsigned)i,0U);goto _LL0;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp500=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F5;_tmp7F5.tag=0U,({struct _fat_ptr _tmpA22=({const char*_tmp501="compile_pat_test_as_case!";_tag_fat(_tmp501,sizeof(char),26U);});_tmp7F5.f1=_tmpA22;});_tmp7F5;});void*_tmp4FF[1U];_tmp4FF[0]=& _tmp500;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4FF,sizeof(void*),1U));});}_LL0:;}
# 3028
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->tag=17U,_tmp502->f1=e;_tmp502;}),0U);}
# 3032
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple40{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3043
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3047
void*t=(void*)_check_null(p->topt);
void*_tmp503=p->r;void*_stmttmp52=_tmp503;void*_tmp504=_stmttmp52;struct Cyc_List_List*_tmp506;union Cyc_Absyn_AggrInfo _tmp505;struct Cyc_List_List*_tmp507;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Pat*_tmp509;struct Cyc_List_List*_tmp50C;struct Cyc_Absyn_Datatypefield*_tmp50B;struct Cyc_Absyn_Datatypedecl*_tmp50A;struct Cyc_Absyn_Pat*_tmp50E;struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50F;struct Cyc_Absyn_Pat*_tmp511;struct Cyc_Absyn_Vardecl*_tmp510;struct Cyc_Absyn_Vardecl*_tmp512;switch(*((int*)_tmp504)){case 2U: _LL1: _tmp512=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp504)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp512;
# 3050
struct Cyc_Absyn_Pat*_tmp513=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp513;
p2->topt=p->topt;
_tmp510=vd;_tmp511=p2;goto _LL4;}case 1U: _LL3: _tmp510=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp504)->f1;_tmp511=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp504)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp510;struct Cyc_Absyn_Pat*p2=_tmp511;
# 3055
struct Cyc_Absyn_Vardecl*_tmp514=({struct _tuple1*_tmpA23=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA23,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp514;
({struct Cyc_List_List*_tmpA25=({struct Cyc_List_List*_tmp516=_region_malloc(rgn,sizeof(*_tmp516));({struct _tuple39*_tmpA24=({struct _tuple39*_tmp515=_region_malloc(rgn,sizeof(*_tmp515));_tmp515->f1=vd,_tmp515->f2=new_vd;_tmp515;});_tmp516->hd=_tmpA24;}),_tmp516->tl=*decls;_tmp516;});*decls=_tmpA25;});{
struct Cyc_Absyn_Stmt*_tmp517=({struct Cyc_Absyn_Exp*_tmpA26=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->tag=4U,_tmp518->f1=new_vd;_tmp518;}),0U);Cyc_Absyn_assign_stmt(_tmpA26,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp517;
return({struct Cyc_Absyn_Stmt*_tmpA27=s;Cyc_Toc_seq_stmt_opt(_tmpA27,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4U: _LL5: _tmp50F=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp504)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp50F;
# 3061
struct Cyc_Absyn_Vardecl*_tmp519=({struct _tuple1*_tmpA28=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA28,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp519;
({struct Cyc_List_List*_tmpA2A=({struct Cyc_List_List*_tmp51B=_region_malloc(rgn,sizeof(*_tmp51B));({struct _tuple39*_tmpA29=({struct _tuple39*_tmp51A=_region_malloc(rgn,sizeof(*_tmp51A));_tmp51A->f1=vd,_tmp51A->f2=new_vd;_tmp51A;});_tmp51B->hd=_tmpA29;}),_tmp51B->tl=*decls;_tmp51B;});*decls=_tmpA2A;});
return({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->tag=4U,_tmp51C->f1=new_vd;_tmp51C;}),0U);Cyc_Absyn_assign_stmt(_tmpA2B,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp50D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp504)->f1;_tmp50E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp504)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp50D;struct Cyc_Absyn_Pat*p2=_tmp50E;
# 3067
({void*_tmpA2C=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmpA2C;});{
struct Cyc_Absyn_Vardecl*_tmp51D=({struct _tuple1*_tmpA2D=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA2D,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp51D;
({struct Cyc_List_List*_tmpA2F=({struct Cyc_List_List*_tmp51F=_region_malloc(rgn,sizeof(*_tmp51F));({struct _tuple39*_tmpA2E=({struct _tuple39*_tmp51E=_region_malloc(rgn,sizeof(*_tmp51E));_tmp51E->f1=vd,_tmp51E->f2=new_vd;_tmp51E;});_tmp51F->hd=_tmpA2E;}),_tmp51F->tl=*decls;_tmp51F;});*decls=_tmpA2F;});{
# 3071
struct Cyc_Absyn_Stmt*_tmp520=({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->tag=4U,_tmp521->f1=new_vd;_tmp521;}),0U);Cyc_Absyn_assign_stmt(_tmpA32,({
void*_tmpA31=({void*_tmpA30=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA30,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA31,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3071
struct Cyc_Absyn_Stmt*s=_tmp520;
# 3074
return({struct Cyc_Absyn_Stmt*_tmpA33=s;Cyc_Toc_seq_stmt_opt(_tmpA33,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9U: _LLB: _LLC:
# 3076
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp504)->f1)->r)->tag == 8U){_LL17: _tmp50A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp504)->f1)->r)->f1;_tmp50B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp504)->f1)->r)->f2;_tmp50C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp504)->f1)->r)->f3;_LL18: {struct Cyc_Absyn_Datatypedecl*tud=_tmp50A;struct Cyc_Absyn_Datatypefield*tuf=_tmp50B;struct Cyc_List_List*ps=_tmp50C;
# 3085
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp522=({void*_tmpA34=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA34,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp522;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp523=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp523;
struct Cyc_Absyn_Stmt*s=0;
for(0;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp524=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp524;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp525=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp525;
void*_tmp526=(void*)_check_null(p2->topt);void*t2=_tmp526;
void*_tmp527=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp527;
struct Cyc_Absyn_Exp*_tmp528=({struct Cyc_Absyn_Exp*_tmpA35=path;Cyc_Absyn_aggrarrow_exp(_tmpA35,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp528;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmpA36=s;Cyc_Toc_seq_stmt_opt(_tmpA36,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3103
return s;}}}}else{_LL21: _tmp509=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp504)->f1;_LL22: {struct Cyc_Absyn_Pat*p2=_tmp509;
# 3149
return({struct _RegionHandle*_tmpA3A=rgn;struct Cyc_Toc_Env**_tmpA39=nv;struct Cyc_List_List**_tmpA38=decls;struct Cyc_Absyn_Exp*_tmpA37=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA3A,_tmpA39,_tmpA38,_tmpA37,p2);});}}case 8U: _LL19: _tmp508=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp504)->f3;_LL1A: {struct Cyc_List_List*ps=_tmp508;
# 3105
_tmp507=ps;goto _LL1C;}case 5U: _LL1B: _tmp507=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp504)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp507;
# 3107
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp529=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp529;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp52A=(void*)_check_null(p2->topt);void*t2=_tmp52A;
struct _fat_ptr*_tmp52B=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp52B;
s=({struct Cyc_Absyn_Stmt*_tmpA3F=s;Cyc_Toc_seq_stmt_opt(_tmpA3F,({struct _RegionHandle*_tmpA3E=rgn;struct Cyc_Toc_Env**_tmpA3D=nv;struct Cyc_List_List**_tmpA3C=decls;struct Cyc_Absyn_Exp*_tmpA3B=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmpA3E,_tmpA3D,_tmpA3C,_tmpA3B,p2);}));});}}
# 3117
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp504)->f1 == 0){_LL1D: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F6;_tmp7F6.tag=0U,({struct _fat_ptr _tmpA40=({const char*_tmp52E="unresolved aggregate pattern!";_tag_fat(_tmp52E,sizeof(char),30U);});_tmp7F6.f1=_tmpA40;});_tmp7F6;});void*_tmp52C[1U];_tmp52C[0]=& _tmp52D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp52C,sizeof(void*),1U));});}else{_LL1F: _tmp505=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp504)->f1;_tmp506=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp504)->f3;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp505;struct Cyc_List_List*dlps=_tmp506;
# 3121
struct Cyc_Absyn_Aggrdecl*_tmp52F=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp52F;
struct Cyc_Absyn_Stmt*s=0;
for(0;dlps != 0;dlps=dlps->tl){
struct _tuple40*_tmp530=(struct _tuple40*)dlps->hd;struct _tuple40*tup=_tmp530;
struct Cyc_Absyn_Pat*_tmp531=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp531;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp532=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp532;
void*_tmp533=(void*)_check_null(p2->topt);void*t2=_tmp533;
void*_tmp534=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp534;
struct Cyc_Absyn_Exp*_tmp535=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp535;
# 3133
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp536=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp536;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(ftype)))
# 3141
memexp=Cyc_Absyn_deref_exp(({void*_tmpA41=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA41,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3144
s=({struct Cyc_Absyn_Stmt*_tmpA42=s;Cyc_Toc_seq_stmt_opt(_tmpA42,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3146
return s;}}case 15U: _LL23: _LL24:
# 3151
({struct Cyc_Warn_String_Warn_Warg_struct _tmp538=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F7;_tmp7F7.tag=0U,({struct _fat_ptr _tmpA43=({const char*_tmp539="unknownid pat";_tag_fat(_tmp539,sizeof(char),14U);});_tmp7F7.f1=_tmpA43;});_tmp7F7;});void*_tmp537[1U];_tmp537[0]=& _tmp538;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp537,sizeof(void*),1U));});case 16U: _LL25: _LL26:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F8;_tmp7F8.tag=0U,({struct _fat_ptr _tmpA44=({const char*_tmp53C="unknowncall pat";_tag_fat(_tmp53C,sizeof(char),16U);});_tmp7F8.f1=_tmpA44;});_tmp7F8;});void*_tmp53A[1U];_tmp53A[0]=& _tmp53B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp53A,sizeof(void*),1U));});default: _LL27: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F9;_tmp7F9.tag=0U,({struct _fat_ptr _tmpA45=({const char*_tmp53F="exp pat";_tag_fat(_tmp53F,sizeof(char),8U);});_tmp7F9.f1=_tmpA45;});_tmp7F9;});void*_tmp53D[1U];_tmp53D[0]=& _tmp53E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp53D,sizeof(void*),1U));});}_LL0:;}
# 3157
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple39 _tmp540=*((struct _tuple39*)vs->hd);struct _tuple39 _stmttmp53=_tmp540;struct _tuple39 _tmp541=_stmttmp53;struct Cyc_Absyn_Vardecl*_tmp543;struct Cyc_Absyn_Vardecl*_tmp542;_LL1: _tmp542=_tmp541.f1;_tmp543=_tmp541.f2;_LL2: {struct Cyc_Absyn_Vardecl*oldv=_tmp542;struct Cyc_Absyn_Vardecl*newv=_tmp543;
if(oldv == newv)return newv;}}
# 3162
({void*_tmp544=0U;({struct _fat_ptr _tmpA46=({const char*_tmp545="find_new_var";_tag_fat(_tmp545,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA46,_tag_fat(_tmp544,sizeof(void*),0U));});});}
# 3168
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp546=e->r;void*_stmttmp54=_tmp546;void*_tmp547=_stmttmp54;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54D;struct Cyc_List_List*_tmp54E;struct Cyc_List_List*_tmp54F;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp565;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Vardecl*_tmp56D;struct Cyc_Absyn_Vardecl*_tmp56E;switch(*((int*)_tmp547)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp547)->f1)){case 5U: _LL1: _tmp56E=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp547)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp56E;
_tmp56D=vd;goto _LL4;}case 4U: _LL3: _tmp56D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp547)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp56D;
# 3172
{struct _handler_cons _tmp56F;_push_handler(& _tmp56F);{int _tmp571=0;if(setjmp(_tmp56F.handler))_tmp571=1;if(!_tmp571){
({void*_tmpA49=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=1U,({void*_tmpA48=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=4U,({struct Cyc_Absyn_Vardecl*_tmpA47=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,vd);_tmp572->f1=_tmpA47;});_tmp572;});_tmp573->f1=_tmpA48;});_tmp573;});e->r=_tmpA49;});;_pop_handler();}else{void*_tmp570=(void*)Cyc_Core_get_exn_thrown();void*_tmp574=_tmp570;void*_tmp575;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp574)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3175
 goto _LL3F;}else{_LL42: _tmp575=_tmp574;_LL43: {void*exn=_tmp575;(int)_rethrow(exn);}}_LL3F:;}}}
# 3177
goto _LL0;}default: goto _LL3D;}case 6U: _LL5: _tmp56A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_tmp56B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_tmp56C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp547)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp56A;struct Cyc_Absyn_Exp*e2=_tmp56B;struct Cyc_Absyn_Exp*e3=_tmp56C;
# 3179
Cyc_Toc_subst_pattern_vars(env,e1);_tmp568=e2;_tmp569=e3;goto _LL8;}case 27U: _LL7: _tmp568=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_tmp569=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp547)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp568;struct Cyc_Absyn_Exp*e2=_tmp569;
_tmp566=e1;_tmp567=e2;goto _LLA;}case 7U: _LL9: _tmp566=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_tmp567=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp566;struct Cyc_Absyn_Exp*e2=_tmp567;
_tmp564=e1;_tmp565=e2;goto _LLC;}case 8U: _LLB: _tmp564=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_tmp565=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp564;struct Cyc_Absyn_Exp*e2=_tmp565;
_tmp562=e1;_tmp563=e2;goto _LLE;}case 23U: _LLD: _tmp562=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_tmp563=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp562;struct Cyc_Absyn_Exp*e2=_tmp563;
_tmp560=e1;_tmp561=e2;goto _LL10;}case 9U: _LLF: _tmp560=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_tmp561=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp560;struct Cyc_Absyn_Exp*e2=_tmp561;
# 3185
Cyc_Toc_subst_pattern_vars(env,e1);_tmp55F=e2;goto _LL12;}case 41U: _LL11: _tmp55F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp55F;
_tmp55E=e;goto _LL14;}case 38U: _LL13: _tmp55E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp55E;
_tmp55D=e;goto _LL16;}case 11U: _LL15: _tmp55D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp55D;
_tmp55C=e;goto _LL18;}case 12U: _LL17: _tmp55C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp55C;
_tmp55B=e;goto _LL1A;}case 13U: _LL19: _tmp55B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp55B;
_tmp55A=e;goto _LL1C;}case 14U: _LL1B: _tmp55A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp55A;
_tmp559=e;goto _LL1E;}case 18U: _LL1D: _tmp559=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp559;
_tmp558=e;goto _LL20;}case 20U: _LL1F: _tmp558=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp558;
_tmp557=e;goto _LL22;}case 21U: _LL21: _tmp557=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp557;
_tmp556=e;goto _LL24;}case 22U: _LL23: _tmp556=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp556;
_tmp555=e;goto _LL26;}case 28U: _LL25: _tmp555=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp555;
_tmp554=e;goto _LL28;}case 15U: _LL27: _tmp554=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp554;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34U: _LL29: _tmp552=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp547)->f1).rgn;_tmp553=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp547)->f1).num_elts;_LL2A: {struct Cyc_Absyn_Exp*eopt=_tmp552;struct Cyc_Absyn_Exp*e=_tmp553;
_tmp550=eopt;_tmp551=e;goto _LL2C;}case 16U: _LL2B: _tmp550=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_tmp551=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp550;struct Cyc_Absyn_Exp*e=_tmp551;
# 3200
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3U: _LL2D: _tmp54F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL2E: {struct Cyc_List_List*es=_tmp54F;
_tmp54E=es;goto _LL30;}case 31U: _LL2F: _tmp54E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL30: {struct Cyc_List_List*es=_tmp54E;
_tmp54D=es;goto _LL32;}case 24U: _LL31: _tmp54D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL32: {struct Cyc_List_List*es=_tmp54D;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36U: _LL33: _tmp54C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL34: {struct Cyc_List_List*dles=_tmp54C;
_tmp54B=dles;goto _LL36;}case 29U: _LL35: _tmp54B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp547)->f3;_LL36: {struct Cyc_List_List*dles=_tmp54B;
_tmp54A=dles;goto _LL38;}case 30U: _LL37: _tmp54A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL38: {struct Cyc_List_List*dles=_tmp54A;
_tmp549=dles;goto _LL3A;}case 26U: _LL39: _tmp549=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp549;
_tmp548=dles;goto _LL3C;}case 25U: _LL3B: _tmp548=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp548;
# 3210
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp576=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp55=_tmp576;struct _tuple19*_tmp577=_stmttmp55;struct Cyc_Absyn_Exp*_tmp578;_LL45: _tmp578=_tmp577->f2;_LL46: {struct Cyc_Absyn_Exp*e=_tmp578;
Cyc_Toc_subst_pattern_vars(env,e);}}
# 3214
goto _LL0;}default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple41{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3224
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3232
void*_tmp579=dopt;void*_tmp57C;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*_tmp57A;struct Cyc_Tcpat_Rhs*_tmp57D;if(_tmp579 == 0){_LL1: _LL2:
# 3234
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp579)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp57D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp579)->f1;_LL6: {struct Cyc_Tcpat_Rhs*rhs=_tmp57D;
# 3238
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp57E=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp56=_tmp57E;struct _tuple37*_tmp57F=_stmttmp56;struct Cyc_Absyn_Switch_clause*_tmp583;struct _fat_ptr*_tmp582;struct _fat_ptr*_tmp581;int*_tmp580;_LLA: _tmp580=(int*)& _tmp57F->f1;_tmp581=_tmp57F->f2;_tmp582=_tmp57F->f3;_tmp583=_tmp57F->f4;_LLB: {int*already_emitted=_tmp580;struct _fat_ptr*init_lab=_tmp581;struct _fat_ptr*code_lab=_tmp582;struct Cyc_Absyn_Switch_clause*sc=_tmp583;
struct Cyc_Absyn_Stmt*_tmp584=sc->body;struct Cyc_Absyn_Stmt*body=_tmp584;
if(body == rhs->rhs){
# 3243
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3248
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA4B=rgn;struct Cyc_Absyn_Exp*_tmpA4A=
Cyc_Absyn_var_exp(v,0U);
# 3248
Cyc_Toc_extract_pattern_vars(_tmpA4B,& nv,& newdecls,_tmpA4A,sc->pattern);});
# 3252
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp585=newdecls;struct Cyc_List_List*ds=_tmp585;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp586=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp57=_tmp586;struct _tuple39 _tmp587=_stmttmp57;struct Cyc_Absyn_Vardecl*_tmp589;struct Cyc_Absyn_Vardecl*_tmp588;_LLD: _tmp588=_tmp587.f1;_tmp589=_tmp587.f2;_LLE: {struct Cyc_Absyn_Vardecl*oldv=_tmp588;struct Cyc_Absyn_Vardecl*newv=_tmp589;
({struct Cyc_List_List*_tmpA4D=({struct Cyc_List_List*_tmp58B=_region_malloc(rgn,sizeof(*_tmp58B));({struct _tuple39*_tmpA4C=({struct _tuple39*_tmp58A=_region_malloc(rgn,sizeof(*_tmp58A));_tmp58A->f1=oldv,_tmp58A->f2=newv;_tmp58A;});_tmp58B->hd=_tmpA4C;}),_tmp58B->tl=*decls;_tmp58B;});*decls=_tmpA4D;});
({struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->tag=4U,_tmp58C->f1=newv;_tmp58C;}),0U);oldv->initializer=_tmpA4E;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmpA50=({struct Cyc_Absyn_Decl*_tmp58E=_cycalloc(sizeof(*_tmp58E));({void*_tmpA4F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->tag=0U,_tmp58D->f1=oldv;_tmp58D;});_tmp58E->r=_tmpA4F;}),_tmp58E->loc=0U;_tmp58E;});Cyc_Absyn_decl_stmt(_tmpA50,res,0U);});}}}
# 3261
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmpA52=({struct Cyc_List_List*_tmp590=_region_malloc(rgn,sizeof(*_tmp590));({struct _tuple41*_tmpA51=({struct _tuple41*_tmp58F=_region_malloc(rgn,sizeof(*_tmp58F));_tmp58F->f1=nv,_tmp58F->f2=code_lab,_tmp58F->f3=body;_tmp58F;});_tmp590->hd=_tmpA51;}),_tmp590->tl=*bodies;_tmp590;});*bodies=_tmpA52;});
return res;}}}}
# 3269
({struct Cyc_Warn_String_Warn_Warg_struct _tmp592=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FA;_tmp7FA.tag=0U,({struct _fat_ptr _tmpA53=({const char*_tmp593="couldn't find rhs!";_tag_fat(_tmp593,sizeof(char),19U);});_tmp7FA.f1=_tmpA53;});_tmp7FA;});void*_tmp591[1U];_tmp591[0]=& _tmp592;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp591,sizeof(void*),1U));});}default: _LL7: _tmp57A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp579)->f1;_tmp57B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp579)->f2;_tmp57C=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp579)->f3;_LL8: {struct Cyc_List_List*symbolic_path=_tmp57A;struct Cyc_List_List*switches=_tmp57B;void*other_decision=_tmp57C;
# 3272
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3274
struct Cyc_Absyn_Exp*_tmp594=({struct Cyc_List_List*_tmpA54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmpA54,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp594;
struct Cyc_List_List*_tmp595=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp595;
# 3277
struct _tuple38 _tmp596=Cyc_Toc_admits_switch(ss);struct _tuple38 _stmttmp58=_tmp596;struct _tuple38 _tmp597=_stmttmp58;void*_tmp599;int _tmp598;_LL10: _tmp598=_tmp597.f1;_tmp599=_tmp597.f2;_LL11: {int allows_switch=_tmp598;void*test_kind=_tmp599;
if(allows_switch > 1){
# 3281
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));
({struct Cyc_Absyn_Switch_clause*_tmpA56=({struct Cyc_Absyn_Switch_clause*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));({struct Cyc_Absyn_Pat*_tmpA55=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5A9->pattern=_tmpA55;}),_tmp5A9->pat_vars=0,_tmp5A9->where_clause=0,_tmp5A9->body=res,_tmp5A9->loc=0U;_tmp5A9;});_tmp5AA->hd=_tmpA56;}),_tmp5AA->tl=0;_tmp5AA;});
# 3284
for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp59A=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp59=_tmp59A;struct _tuple34 _tmp59B=_stmttmp59;void*_tmp59D;void*_tmp59C;_LL13: _tmp59C=_tmp59B.f1;_tmp59D=_tmp59B.f2;_LL14: {void*pat_test=_tmp59C;void*dec_tree=_tmp59D;
# 3287
struct Cyc_Absyn_Pat*_tmp59E=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp59E;
# 3289
struct Cyc_Absyn_Stmt*_tmp59F=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp59F;
# 3291
new_lscs=({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));({struct Cyc_Absyn_Switch_clause*_tmpA57=({struct Cyc_Absyn_Switch_clause*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->pattern=case_exp,_tmp5A0->pat_vars=0,_tmp5A0->where_clause=0,_tmp5A0->body=s,_tmp5A0->loc=0U;_tmp5A0;});_tmp5A1->hd=_tmpA57;}),_tmp5A1->tl=new_lscs;_tmp5A1;});}}
# 3293
{void*_tmp5A2=test_kind;struct _fat_ptr*_tmp5A3;switch(*((int*)_tmp5A2)){case 1U: _LL16: _LL17:
# 3295
 LOOP1: {
void*_tmp5A4=p->r;void*_stmttmp5A=_tmp5A4;void*_tmp5A5=_stmttmp5A;struct Cyc_Absyn_Exp*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A7;switch(*((int*)_tmp5A5)){case 14U: _LL1F: _tmp5A7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A5)->f2;_LL20: {struct Cyc_Absyn_Exp*e=_tmp5A7;
p=e;goto LOOP1;}case 20U: _LL21: _tmp5A6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5A5)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5A6;
p=e;goto _LL1E;}default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3301
p=Cyc_Absyn_deref_exp(({void*_tmpA58=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA58,p);}),0U);goto _LL15;case 3U: _LL18: _tmp5A3=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5A2)->f1;_LL19: {struct _fat_ptr*f=_tmp5A3;
# 3303
p=({struct Cyc_Absyn_Exp*_tmpA59=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmpA59,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3307
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->tag=10U,_tmp5A8->f1=p,_tmp5A8->f2=new_lscs,_tmp5A8->f3=0;_tmp5A8;}),0U);}else{
# 3311
void*_tmp5AB=test_kind;struct Cyc_Absyn_Exp*_tmp5AC;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5AB)->tag == 2U){_LL26: _tmp5AC=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5AB)->f1;_LL27: {struct Cyc_Absyn_Exp*e=_tmp5AC;
# 3313
struct Cyc_List_List*_tmp5AD=ss;struct Cyc_Tcpat_Rhs*_tmp5AF;void*_tmp5AE;if(_tmp5AD != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)((struct Cyc_List_List*)_tmp5AD)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5AD)->tl == 0){_LL2B: _tmp5AE=((struct _tuple34*)_tmp5AD->hd)->f1;_tmp5AF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_tmp5AD->hd)->f2)->f1;_LL2C: {void*pat_test=_tmp5AE;struct Cyc_Tcpat_Rhs*rhs=_tmp5AF;
# 3319
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp5B0=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp5B=_tmp5B0;struct _tuple37*_tmp5B1=_stmttmp5B;struct Cyc_Absyn_Switch_clause*_tmp5B5;struct _fat_ptr*_tmp5B4;struct _fat_ptr*_tmp5B3;int*_tmp5B2;_LL30: _tmp5B2=(int*)& _tmp5B1->f1;_tmp5B3=_tmp5B1->f2;_tmp5B4=_tmp5B1->f3;_tmp5B5=_tmp5B1->f4;_LL31: {int*already_emitted=_tmp5B2;struct _fat_ptr*init_lab=_tmp5B3;struct _fat_ptr*code_lab=_tmp5B4;struct Cyc_Absyn_Switch_clause*sc=_tmp5B5;
struct Cyc_Absyn_Stmt*_tmp5B6=sc->body;struct Cyc_Absyn_Stmt*body=_tmp5B6;
if(body == rhs->rhs){
# 3324
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3329
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA5B=rgn;struct Cyc_Absyn_Exp*_tmpA5A=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA5B,& nv,& newdecls,_tmpA5A,sc->pattern);});
# 3333
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp5B7=newdecls;struct Cyc_List_List*ds=_tmp5B7;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp5B8=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp5C=_tmp5B8;struct _tuple39 _tmp5B9=_stmttmp5C;struct Cyc_Absyn_Vardecl*_tmp5BB;struct Cyc_Absyn_Vardecl*_tmp5BA;_LL33: _tmp5BA=_tmp5B9.f1;_tmp5BB=_tmp5B9.f2;_LL34: {struct Cyc_Absyn_Vardecl*oldv=_tmp5BA;struct Cyc_Absyn_Vardecl*newv=_tmp5BB;
({struct Cyc_List_List*_tmpA5D=({struct Cyc_List_List*_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD));({struct _tuple39*_tmpA5C=({struct _tuple39*_tmp5BC=_region_malloc(rgn,sizeof(*_tmp5BC));_tmp5BC->f1=oldv,_tmp5BC->f2=newv;_tmp5BC;});_tmp5BD->hd=_tmpA5C;}),_tmp5BD->tl=*decls;_tmp5BD;});*decls=_tmpA5D;});
({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->tag=4U,_tmp5BE->f1=newv;_tmp5BE;}),0U);oldv->initializer=_tmpA5E;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmpA60=({struct Cyc_Absyn_Decl*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({void*_tmpA5F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->tag=0U,_tmp5BF->f1=oldv;_tmp5BF;});_tmp5C0->r=_tmpA5F;}),_tmp5C0->loc=0U;_tmp5C0;});Cyc_Absyn_decl_stmt(_tmpA60,r,0U);});}}}
# 3342
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3346
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmpA62=({struct Cyc_List_List*_tmp5C2=_region_malloc(rgn,sizeof(*_tmp5C2));({struct _tuple41*_tmpA61=({struct _tuple41*_tmp5C1=_region_malloc(rgn,sizeof(*_tmp5C1));_tmp5C1->f1=nv,_tmp5C1->f2=code_lab,_tmp5C1->f3=body;_tmp5C1;});_tmp5C2->hd=_tmpA61;}),_tmp5C2->tl=*bodies;_tmp5C2;});*bodies=_tmpA62;});
return r;}}}}
# 3356
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FB;_tmp7FB.tag=0U,({struct _fat_ptr _tmpA63=({const char*_tmp5C5="couldn't find rhs!";_tag_fat(_tmp5C5,sizeof(char),19U);});_tmp7FB.f1=_tmpA63;});_tmp7FB;});void*_tmp5C3[1U];_tmp5C3[0]=& _tmp5C4;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C3,sizeof(void*),1U));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FC;_tmp7FC.tag=0U,({struct _fat_ptr _tmpA64=({const char*_tmp5C8="bad where clause in match compiler";_tag_fat(_tmp5C8,sizeof(char),35U);});_tmp7FC.f1=_tmpA64;});_tmp7FC;});void*_tmp5C6[1U];_tmp5C6[0]=& _tmp5C7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C6,sizeof(void*),1U));});}_LL2A:;}}else{_LL28: _LL29:
# 3361
 for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp5C9=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5D=_tmp5C9;struct _tuple34 _tmp5CA=_stmttmp5D;void*_tmp5CC;void*_tmp5CB;_LL36: _tmp5CB=_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;_LL37: {void*pat_test=_tmp5CB;void*dec_tree=_tmp5CC;
struct Cyc_Absyn_Exp*_tmp5CD=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp5CD;
struct Cyc_Absyn_Stmt*_tmp5CE=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp5CE;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}}_LL25:;}
# 3369
return res;}}}}_LL0:;}
# 3379
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3381
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple41*_tmp5CF=(struct _tuple41*)bodies->hd;struct _tuple41*_stmttmp5E=_tmp5CF;struct _tuple41*_tmp5D0=_stmttmp5E;struct Cyc_Absyn_Stmt*_tmp5D2;struct Cyc_Toc_Env**_tmp5D1;_LL1: _tmp5D1=(struct Cyc_Toc_Env**)& _tmp5D0->f1;_tmp5D2=_tmp5D0->f3;_LL2: {struct Cyc_Toc_Env**nv=_tmp5D1;struct Cyc_Absyn_Stmt*s2=_tmp5D2;
if(s2 == s)return nv;}}
# 3387
return 0;}
# 3391
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3394
void*_tmp5D3=(void*)_check_null(e->topt);void*t=_tmp5D3;
Cyc_Toc_exp_to_c(nv,e);{
# 3397
struct _tuple30 _tmp5D4=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp5F=_tmp5D4;struct _tuple30 _tmp5D5=_stmttmp5F;struct Cyc_Absyn_Exp*_tmp5D7;struct _tuple1*_tmp5D6;_LL1: _tmp5D6=_tmp5D5.f1;_tmp5D7=_tmp5D5.f2;_LL2: {struct _tuple1*v=_tmp5D6;struct Cyc_Absyn_Exp*path=_tmp5D7;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5D8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D8;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5D9=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp5D9;
# 3403
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3408
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3416
{struct Cyc_List_List*_tmp5DA=lscs;struct Cyc_List_List*lscs2=_tmp5DA;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_tmp5DB=(struct _tuple37*)lscs2->hd;struct _tuple37*_stmttmp60=_tmp5DB;struct _tuple37*_tmp5DC=_stmttmp60;struct Cyc_Absyn_Switch_clause*_tmp5DE;struct _fat_ptr*_tmp5DD;_LL4: _tmp5DD=_tmp5DC->f3;_tmp5DE=_tmp5DC->f4;_LL5: {struct _fat_ptr*body_lab=_tmp5DD;struct Cyc_Absyn_Switch_clause*body_sc=_tmp5DE;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3420
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3423
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5DF=*envp;struct Cyc_Toc_Env*env=_tmp5DF;
# 3426
if(lscs2->tl != 0){
struct _tuple37*_tmp5E0=(struct _tuple37*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple37*_stmttmp61=_tmp5E0;struct _tuple37*_tmp5E1=_stmttmp61;struct Cyc_Absyn_Switch_clause*_tmp5E3;struct _fat_ptr*_tmp5E2;_LL7: _tmp5E2=_tmp5E1->f3;_tmp5E3=_tmp5E1->f4;_LL8: {struct _fat_ptr*fallthru_lab=_tmp5E2;struct Cyc_Absyn_Switch_clause*next_sc=_tmp5E3;
# 3430
struct Cyc_Toc_Env**_tmp5E4=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp5E4;
# 3435
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmpA65=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA65,s);});else{
# 3439
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmpA66=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmpA66,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3444
({struct Cyc_Toc_Env*_tmpA67=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmpA67,s);});}}}else{
# 3448
({struct Cyc_Toc_Env*_tmpA68=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA68,s);});}}}}}{
# 3452
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA6A=test_tree;Cyc_Absyn_seq_stmt(_tmpA6A,({struct _fat_ptr*_tmpA69=end_l;Cyc_Absyn_label_stmt(_tmpA69,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3454
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp5E5=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp62=_tmp5E5;struct _tuple39 _tmp5E6=_stmttmp62;struct Cyc_Absyn_Vardecl*_tmp5E7;_LLA: _tmp5E7=_tmp5E6.f2;_LLB: {struct Cyc_Absyn_Vardecl*vd=_tmp5E7;
res=({struct Cyc_Absyn_Decl*_tmpA6B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=0U,_tmp5E8->f1=vd;_tmp5E8;}),0U);Cyc_Absyn_decl_stmt(_tmpA6B,res,0U);});}}
# 3459
({void*_tmpA6F=({struct _tuple1*_tmpA6E=v;void*_tmpA6D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA6C=e;Cyc_Absyn_declare_stmt(_tmpA6E,_tmpA6D,_tmpA6C,res,0U);})->r;whole_s->r=_tmpA6F;});
_npop_handler(0U);return;}}
# 3400
;_pop_region();}}}
# 3465
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3467
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3472
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3474
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5E9[1U];({struct Cyc_Absyn_Exp*_tmpA70=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp5E9[0]=_tmpA70;});({struct Cyc_Absyn_Exp*_tmpA71=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA71,_tag_fat(_tmp5E9,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3477
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA73=({struct Cyc_Absyn_Stmt*_tmpA72=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA72,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA73;});}
# 3482
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3484
while(1){
void*_tmp5EA=s->r;void*_stmttmp63=_tmp5EA;void*_tmp5EB=_stmttmp63;void*_tmp5EE;struct Cyc_List_List*_tmp5ED;struct Cyc_Absyn_Stmt*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5F0;struct Cyc_Absyn_Decl*_tmp5EF;struct Cyc_Absyn_Switch_clause**_tmp5F2;struct Cyc_List_List*_tmp5F1;void*_tmp5F5;struct Cyc_List_List*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F7;struct _fat_ptr*_tmp5F6;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Stmt*_tmp5FA;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Exp*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp602;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_Absyn_Exp*_tmp600;struct Cyc_Absyn_Stmt*_tmp604;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Exp*_tmp606;switch(*((int*)_tmp5EB)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp606=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp606;
Cyc_Toc_exp_to_c(nv,e);return;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1 == 0){_LL5: _LL6:
({int _tmpA74=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA74,s);});return;}else{_LL7: _tmp605=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp605;
# 3491
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp607=Cyc_Toc_temp_var();struct _tuple1*x=_tmp607;
struct Cyc_Absyn_Stmt*_tmp608=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp608;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmpA75=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmpA75;});}
# 3500
return;}}}case 2U: _LL9: _tmp603=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp604=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LLA: {struct Cyc_Absyn_Stmt*s1=_tmp603;struct Cyc_Absyn_Stmt*s2=_tmp604;
# 3502
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4U: _LLB: _tmp600=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp601=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_tmp602=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp600;struct Cyc_Absyn_Stmt*s1=_tmp601;struct Cyc_Absyn_Stmt*s2=_tmp602;
# 3506
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9U: _LLD: _tmp5FC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5FD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2).f1;_tmp5FE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3).f1;_tmp5FF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5FC;struct Cyc_Absyn_Exp*e2=_tmp5FD;struct Cyc_Absyn_Exp*e3=_tmp5FE;struct Cyc_Absyn_Stmt*s2=_tmp5FF;
# 3511
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5FA=s2;_tmp5FB=e3;goto _LL10;}case 14U: _LLF: _tmp5FA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5FB=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5FA;struct Cyc_Absyn_Exp*e=_tmp5FB;
_tmp5F8=e;_tmp5F9=s2;goto _LL12;}case 5U: _LL11: _tmp5F8=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1).f1;_tmp5F9=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5F8;struct Cyc_Absyn_Stmt*s2=_tmp5F9;
# 3516
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp609=_new_region("temp");struct _RegionHandle*temp=& _tmp609;_push_region(temp);({struct Cyc_Toc_Env*_tmpA76=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA76,s2);});
_npop_handler(0U);return;
# 3517
;_pop_region();}}case 6U: _LL13: _LL14: {
# 3520
struct Cyc_Toc_Env*_tmp60A=nv;struct _fat_ptr**_tmp60B;_LL24: _tmp60B=_tmp60A->break_lab;_LL25: {struct _fat_ptr**b=_tmp60B;
if(b != 0)
({void*_tmpA77=Cyc_Toc_goto_stmt_r(*b);s->r=_tmpA77;});
# 3524
({int _tmpA78=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA78,s);});
return;}}case 7U: _LL15: _LL16: {
# 3527
struct Cyc_Toc_Env*_tmp60C=nv;struct _fat_ptr**_tmp60D;_LL27: _tmp60D=_tmp60C->continue_lab;_LL28: {struct _fat_ptr**c=_tmp60D;
if(c != 0)
({void*_tmpA79=Cyc_Toc_goto_stmt_r(*c);s->r=_tmpA79;});
goto _LL18;}}case 8U: _LL17: _LL18:
# 3532
({int _tmpA7A=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA7A,s);});
return;case 13U: _LL19: _tmp5F6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5F7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL1A: {struct _fat_ptr*lab=_tmp5F6;struct Cyc_Absyn_Stmt*s1=_tmp5F7;
s=s1;continue;}case 10U: _LL1B: _tmp5F3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5F4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_tmp5F5=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5F3;struct Cyc_List_List*scs=_tmp5F4;void*dec_tree_opt=_tmp5F5;
# 3536
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11U: _LL1D: _tmp5F1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5F2=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL1E: {struct Cyc_List_List*es=_tmp5F1;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5F2;
# 3539
struct Cyc_Toc_Env*_tmp60E=nv;struct Cyc_Toc_FallthruInfo*_tmp60F;_LL2A: _tmp60F=_tmp60E->fallthru_info;_LL2B: {struct Cyc_Toc_FallthruInfo*fi=_tmp60F;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp611=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FD;_tmp7FD.tag=0U,({struct _fat_ptr _tmpA7B=({const char*_tmp612="fallthru in unexpected place";_tag_fat(_tmp612,sizeof(char),29U);});_tmp7FD.f1=_tmpA7B;});_tmp7FD;});void*_tmp610[1U];_tmp610[0]=& _tmp611;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp610,sizeof(void*),1U));});{
struct Cyc_Toc_FallthruInfo _tmp613=*fi;struct Cyc_Toc_FallthruInfo _stmttmp64=_tmp613;struct Cyc_Toc_FallthruInfo _tmp614=_stmttmp64;struct Cyc_List_List*_tmp616;struct _fat_ptr*_tmp615;_LL2D: _tmp615=_tmp614.label;_tmp616=_tmp614.binders;_LL2E: {struct _fat_ptr*l=_tmp615;struct Cyc_List_List*vs=_tmp616;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3545
({int _tmpA7C=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA7C,s2);});{
struct Cyc_List_List*_tmp617=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp617;
struct Cyc_List_List*_tmp618=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp618;
for(0;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmpA7E=({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=5U,_tmp619->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp619;}),0U);Cyc_Absyn_assign_stmt(_tmpA7D,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA7E,s2,0U);});}
# 3553
s->r=s2->r;
return;}}}}}case 12U: _LL1F: _tmp5EF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5F0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp5EF;struct Cyc_Absyn_Stmt*s1=_tmp5F0;
# 3559
{void*_tmp61A=d->r;void*_stmttmp65=_tmp61A;void*_tmp61B=_stmttmp65;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Vardecl*_tmp61D;struct Cyc_Absyn_Tvar*_tmp61C;struct Cyc_Absyn_Fndecl*_tmp61F;struct Cyc_List_List*_tmp620;void*_tmp623;struct Cyc_Absyn_Exp*_tmp622;struct Cyc_Absyn_Pat*_tmp621;struct Cyc_Absyn_Vardecl*_tmp624;switch(*((int*)_tmp61B)){case 0U: _LL30: _tmp624=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp61B)->f1;_LL31: {struct Cyc_Absyn_Vardecl*vd=_tmp624;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2U: _LL32: _tmp621=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp61B)->f1;_tmp622=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp61B)->f3;_tmp623=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp61B)->f4;_LL33: {struct Cyc_Absyn_Pat*p=_tmp621;struct Cyc_Absyn_Exp*e=_tmp622;void*dec_tree=_tmp623;
# 3567
{void*_tmp625=p->r;void*_stmttmp66=_tmp625;void*_tmp626=_stmttmp66;struct Cyc_Absyn_Vardecl*_tmp627;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp626)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp626)->f2)->r)->tag == 0U){_LL3D: _tmp627=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp626)->f1;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp627;
# 3569
struct Cyc_Absyn_Vardecl*_tmp628=({struct _tuple1*_tmpA80=Cyc_Toc_temp_var();void*_tmpA7F=vd->type;Cyc_Absyn_new_vardecl(0U,_tmpA80,_tmpA7F,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp628;
({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->tag=4U,_tmp629->f1=new_vd;_tmp629;}),0U);vd->initializer=_tmpA81;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmpA88=({struct Cyc_Absyn_Decl*_tmpA87=({void*_tmpA82=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tag=0U,_tmp62A->f1=new_vd;_tmp62A;});Cyc_Absyn_new_decl(_tmpA82,s->loc);});struct Cyc_Absyn_Stmt*_tmpA86=({
struct Cyc_Absyn_Decl*_tmpA85=({void*_tmpA83=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->tag=0U,_tmp62B->f1=vd;_tmp62B;});Cyc_Absyn_new_decl(_tmpA83,s->loc);});struct Cyc_Absyn_Stmt*_tmpA84=s1;Cyc_Absyn_decl_stmt(_tmpA85,_tmpA84,s->loc);});
# 3572
Cyc_Absyn_decl_stmt(_tmpA87,_tmpA86,s->loc);})->r;s->r=_tmpA88;});
# 3575
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3581
({void*_tmpA89=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmpA89;});
goto _LL3C;}_LL3C:;}
# 3584
goto _LL2F;}case 3U: _LL34: _tmp620=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp61B)->f1;_LL35: {struct Cyc_List_List*vds=_tmp620;
# 3588
struct Cyc_List_List*_tmp62C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp62C;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FE;_tmp7FE.tag=0U,({struct _fat_ptr _tmpA8A=({const char*_tmp62F="empty Letv_d";_tag_fat(_tmp62F,sizeof(char),13U);});_tmp7FE.f1=_tmpA8A;});_tmp7FE;});void*_tmp62D[1U];_tmp62D[0]=& _tmp62E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp62D,sizeof(void*),1U));});
({void*_tmpA8B=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->tag=0U,_tmp630->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp630;});d->r=_tmpA8B;});
rvds=rvds->tl;
for(0;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp631=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=0U,_tmp632->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp632;}),0U);struct Cyc_Absyn_Decl*d2=_tmp631;
({void*_tmpA8D=({struct Cyc_Absyn_Decl*_tmpA8C=d2;Cyc_Absyn_decl_stmt(_tmpA8C,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA8D;});}
# 3597
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp61F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp61B)->f1;_LL37: {struct Cyc_Absyn_Fndecl*fd=_tmp61F;
# 3600
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4U: _LL38: _tmp61C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61B)->f1;_tmp61D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61B)->f2;_tmp61E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61B)->f3;_LL39: {struct Cyc_Absyn_Tvar*tv=_tmp61C;struct Cyc_Absyn_Vardecl*vd=_tmp61D;struct Cyc_Absyn_Exp*open_exp_opt=_tmp61E;
# 3604
struct Cyc_Absyn_Stmt*_tmp633=s1;struct Cyc_Absyn_Stmt*body=_tmp633;
# 3606
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp634=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp67=_tmp634;struct _tuple30 _tmp635=_stmttmp67;struct Cyc_Absyn_Exp*_tmp637;struct _tuple1*_tmp636;_LL42: _tmp636=_tmp635.f1;_tmp637=_tmp635.f2;_LL43: {struct _tuple1*rh_var=_tmp636;struct Cyc_Absyn_Exp*rh_exp=_tmp637;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3612
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Flags_no_regions)
({void*_tmpA91=({struct _tuple1*_tmpA90=x_var;void*_tmpA8F=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA8E=
Cyc_Absyn_uint_exp(0U,0U);
# 3614
Cyc_Absyn_declare_stmt(_tmpA90,_tmpA8F,_tmpA8E,body,0U);})->r;s->r=_tmpA91;});else{
# 3616
if((unsigned)open_exp_opt){
# 3618
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA93=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA93,({struct _fat_ptr*_tmp639=_cycalloc(sizeof(*_tmp639));({struct _fat_ptr _tmpA92=({const char*_tmp638="h";_tag_fat(_tmp638,sizeof(char),2U);});*_tmp639=_tmpA92;});_tmp639;}),0U);}),0U);
({void*_tmpA94=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA94;});}}else{
# 3627
({void*_tmpAA1=({struct _tuple1*_tmpAA0=rh_var;void*_tmpA9F=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA9E=({struct Cyc_Absyn_Exp*_tmp63A[1U];({struct Cyc_Absyn_Exp*_tmpA95=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp63A[0]=_tmpA95;});({struct Cyc_Absyn_Exp*_tmpA96=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA96,_tag_fat(_tmp63A,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3627
Cyc_Absyn_declare_stmt(_tmpAA0,_tmpA9F,_tmpA9E,({
# 3629
struct _tuple1*_tmpA9D=x_var;void*_tmpA9C=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA9D,_tmpA9C,_tmpA9B,({
struct Cyc_Absyn_Stmt*_tmpA9A=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp63B[1U];_tmp63B[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA97=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA97,_tag_fat(_tmp63B,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA9A,({
struct Cyc_Absyn_Stmt*_tmpA99=body;Cyc_Absyn_seq_stmt(_tmpA99,
Cyc_Absyn_exp_stmt(({void*_tmp63C=0U;({struct Cyc_Absyn_Exp*_tmpA98=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA98,_tag_fat(_tmp63C,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3627
s->r=_tmpAA1;});}}
# 3635
return;}}default: _LL3A: _LL3B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FF;_tmp7FF.tag=0U,({struct _fat_ptr _tmpAA2=({const char*_tmp63F="bad nested declaration within function";_tag_fat(_tmp63F,sizeof(char),39U);});_tmp7FF.f1=_tmpAA2;});_tmp7FF;});void*_tmp63D[1U];_tmp63D[0]=& _tmp63E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp63D,sizeof(void*),1U));});}_LL2F:;}
# 3638
return;}default: _LL21: _tmp5EC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp5ED=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_tmp5EE=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3;_LL22: {struct Cyc_Absyn_Stmt*body=_tmp5EC;struct Cyc_List_List*scs=_tmp5ED;void*dec_tree=_tmp5EE;
# 3652 "toc.cyc"
struct _tuple30 _tmp640=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp68=_tmp640;struct _tuple30 _tmp641=_stmttmp68;struct Cyc_Absyn_Exp*_tmp643;struct _tuple1*_tmp642;_LL45: _tmp642=_tmp641.f1;_tmp643=_tmp641.f2;_LL46: {struct _tuple1*h_var=_tmp642;struct Cyc_Absyn_Exp*h_exp=_tmp643;
struct _tuple30 _tmp644=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp69=_tmp644;struct _tuple30 _tmp645=_stmttmp69;struct Cyc_Absyn_Exp*_tmp647;struct _tuple1*_tmp646;_LL48: _tmp646=_tmp645.f1;_tmp647=_tmp645.f2;_LL49: {struct _tuple1*e_var=_tmp646;struct Cyc_Absyn_Exp*e_exp=_tmp647;
struct _tuple30 _tmp648=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6A=_tmp648;struct _tuple30 _tmp649=_stmttmp6A;struct Cyc_Absyn_Exp*_tmp64B;struct _tuple1*_tmp64A;_LL4B: _tmp64A=_tmp649.f1;_tmp64B=_tmp649.f2;_LL4C: {struct _tuple1*was_thrown_var=_tmp64A;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp64B;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3659
e_exp->topt=e_typ;{
struct _RegionHandle _tmp64C=_new_region("temp");struct _RegionHandle*temp=& _tmp64C;_push_region(temp);
# 3662
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp64D=({struct Cyc_Absyn_Stmt*_tmpAA4=body;Cyc_Absyn_seq_stmt(_tmpAA4,
Cyc_Absyn_exp_stmt(({void*_tmp656=0U;({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpAA3,_tag_fat(_tmp656,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3663
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp64D;
# 3666
struct Cyc_Absyn_Stmt*_tmp64E=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->tag=10U,_tmp655->f1=e_exp,_tmp655->f2=scs,_tmp655->f3=dec_tree;_tmp655;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp64E;
# 3668
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3671
struct Cyc_Absyn_Exp*_tmp64F=({struct Cyc_Absyn_Exp*_tmp654[1U];({struct Cyc_Absyn_Exp*_tmpAA5=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp654[0]=_tmpAA5;});({struct Cyc_Absyn_Exp*_tmpAA6=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpAA6,_tag_fat(_tmp654,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3671
struct Cyc_Absyn_Exp*setjmp_call=_tmp64F;
# 3673
struct Cyc_Absyn_Stmt*_tmp650=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp653[1U];({struct Cyc_Absyn_Exp*_tmpAA7=Cyc_Absyn_address_exp(h_exp,0U);_tmp653[0]=_tmpAA7;});({struct Cyc_Absyn_Exp*_tmpAA8=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpAA8,_tag_fat(_tmp653,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
# 3673
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp650;
# 3675
struct Cyc_Absyn_Exp*_tmp651=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp651;
struct Cyc_Absyn_Exp*_tmp652=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp652;
({void*_tmpAB8=({
struct _tuple1*_tmpAB7=h_var;void*_tmpAB6=h_typ;Cyc_Absyn_declare_stmt(_tmpAB7,_tmpAB6,0,({
struct Cyc_Absyn_Stmt*_tmpAB5=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpAB5,({
struct _tuple1*_tmpAB4=was_thrown_var;void*_tmpAB3=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpAB2=zero_exp;Cyc_Absyn_declare_stmt(_tmpAB4,_tmpAB3,_tmpAB2,({
struct Cyc_Absyn_Stmt*_tmpAB1=({struct Cyc_Absyn_Exp*_tmpAAA=setjmp_call;struct Cyc_Absyn_Stmt*_tmpAA9=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3681
Cyc_Absyn_ifthenelse_stmt(_tmpAAA,_tmpAA9,
# 3683
Cyc_Toc_skip_stmt_dl(),0U);});
# 3681
Cyc_Absyn_seq_stmt(_tmpAB1,({
# 3684
struct Cyc_Absyn_Exp*_tmpAB0=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpAAF=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpAB0,_tmpAAF,({
# 3686
struct _tuple1*_tmpAAE=e_var;void*_tmpAAD=e_typ;struct Cyc_Absyn_Exp*_tmpAAC=({
void*_tmpAAB=e_typ;Cyc_Toc_cast_it(_tmpAAB,Cyc_Toc_get_exn_thrown_expression());});
# 3686
Cyc_Absyn_declare_stmt(_tmpAAE,_tmpAAD,_tmpAAC,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3677
s->r=_tmpAB8;});}}
# 3690
_npop_handler(0U);return;
# 3660
;_pop_region();}}}}}}_LL0:;}}
# 3699
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpAB9=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpAB9;});
({void*_tmpABA=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpABA;});{
struct _RegionHandle _tmp657=_new_region("frgn");struct _RegionHandle*frgn=& _tmp657;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp658=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp658;
{struct Cyc_List_List*_tmp659=(f->i).args;struct Cyc_List_List*args=_tmp659;for(0;args != 0;args=args->tl){
struct _tuple1*_tmp65A=({struct _tuple1*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=Cyc_Absyn_Loc_n,_tmp65B->f2=(*((struct _tuple9*)args->hd)).f1;_tmp65B;});struct _tuple1*x=_tmp65A;
({void*_tmpABB=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpABB;});}}
# 3716
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3720
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp65C=_cycalloc(sizeof(*_tmp65C));({struct Cyc_Hashtable_Table*_tmpABC=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp65C=_tmpABC;});_tmp65C;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp65D=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6B=_tmp65D;struct Cyc_Absyn_VarargInfo _tmp65E=_stmttmp6B;int _tmp662;void*_tmp661;struct Cyc_Absyn_Tqual _tmp660;struct _fat_ptr*_tmp65F;_LL1: _tmp65F=_tmp65E.name;_tmp660=_tmp65E.tq;_tmp661=_tmp65E.type;_tmp662=_tmp65E.inject;_LL2: {struct _fat_ptr*n=_tmp65F;struct Cyc_Absyn_Tqual tq=_tmp660;void*t=_tmp661;int i=_tmp662;
void*_tmp663=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));void*t2=_tmp663;
struct _tuple1*_tmp664=({struct _tuple1*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->f1=Cyc_Absyn_Loc_n,_tmp667->f2=(struct _fat_ptr*)_check_null(n);_tmp667;});struct _tuple1*x2=_tmp664;
({struct Cyc_List_List*_tmpABF=({struct Cyc_List_List*_tmpABE=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpABE,({struct Cyc_List_List*_tmp666=_cycalloc(sizeof(*_tmp666));({struct _tuple9*_tmpABD=({struct _tuple9*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->f1=n,_tmp665->f2=tq,_tmp665->f3=t2;_tmp665;});_tmp666->hd=_tmpABD;}),_tmp666->tl=0;_tmp666;}));});(f->i).args=_tmpABF;});
(f->i).cyc_varargs=0;}}
# 3729
{struct Cyc_List_List*_tmp668=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp668;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpAC0=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpAC0;});}}
({struct Cyc_Toc_Env*_tmpAC1=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpAC1,f->body);});}
# 3709
;_pop_region();}}
# 3734
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp669=s;switch(_tmp669){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3750 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3752
struct _tuple42 _tmp66A=*env;struct _tuple42 _stmttmp6C=_tmp66A;struct _tuple42 _tmp66B=_stmttmp6C;struct Cyc_Absyn_Aggrdecl*_tmp66D;struct Cyc_Toc_TocState*_tmp66C;_LL1: _tmp66C=_tmp66B.f1;_tmp66D=*_tmp66B.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp66C;struct Cyc_Absyn_Aggrdecl*ad=_tmp66D;
struct _tuple1*_tmp66E=ad->name;struct _tuple1*n=_tmp66E;
struct Cyc_Toc_TocState _tmp66F=*s;struct Cyc_Toc_TocState _stmttmp6D=_tmp66F;struct Cyc_Toc_TocState _tmp670=_stmttmp6D;struct Cyc_Dict_Dict*_tmp671;_LL4: _tmp671=_tmp670.aggrs_so_far;_LL5: {struct Cyc_Dict_Dict*aggrs_so_far=_tmp671;
int seen_defn_before;
struct _tuple18**_tmp672=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple18**dopt=_tmp672;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)ad->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp673=_region_malloc(d,sizeof(*_tmp673));_tmp673->f1=ad,({void*_tmpAC2=Cyc_Absyn_strctq(n);_tmp673->f2=_tmpAC2;});_tmp673;});else{
# 3763
v=({struct _tuple18*_tmp674=_region_malloc(d,sizeof(*_tmp674));_tmp674->f1=ad,({void*_tmpAC3=Cyc_Absyn_unionq_type(n);_tmp674->f2=_tmpAC3;});_tmp674;});}
({struct Cyc_Dict_Dict _tmpAC4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpAC4;});}}else{
# 3766
struct _tuple18*_tmp675=*dopt;struct _tuple18*_stmttmp6E=_tmp675;struct _tuple18*_tmp676=_stmttmp6E;void*_tmp678;struct Cyc_Absyn_Aggrdecl*_tmp677;_LL7: _tmp677=_tmp676->f1;_tmp678=_tmp676->f2;_LL8: {struct Cyc_Absyn_Aggrdecl*ad2=_tmp677;void*t=_tmp678;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpAC7=({struct Cyc_Dict_Dict _tmpAC6=*aggrs_so_far;struct _tuple1*_tmpAC5=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpAC6,_tmpAC5,({struct _tuple18*_tmp679=_region_malloc(d,sizeof(*_tmp679));_tmp679->f1=ad,_tmp679->f2=t;_tmp679;}));});*aggrs_so_far=_tmpAC7;});
seen_defn_before=0;}else{
# 3771
seen_defn_before=1;}}}{
# 3773
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E->kind=ad->kind,_tmp69E->sc=Cyc_Absyn_Public,_tmp69E->name=ad->name,_tmp69E->tvs=0,_tmp69E->impl=0,_tmp69E->expected_mem_kind=0,_tmp69E->attributes=ad->attributes;_tmp69E;});
# 3780
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpAC8=({struct Cyc_Absyn_AggrdeclImpl*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->exist_vars=0,_tmp67A->rgn_po=0,_tmp67A->fields=0,_tmp67A->tagged=0;_tmp67A;});new_ad->impl=_tmpAC8;});{
# 3785
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp67B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp67B;for(0;fields != 0;fields=fields->tl){
# 3789
struct Cyc_Absyn_Aggrfield*_tmp67C=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp67C;
void*_tmp67D=old_field->type;void*old_type=_tmp67D;
struct Cyc_List_List*_tmp67E=old_field->attributes;struct Cyc_List_List*old_atts=_tmp67E;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3803 "toc.cyc"
void*_tmp67F=Cyc_Absyn_compress(old_type);void*_stmttmp6F=_tmp67F;void*_tmp680=_stmttmp6F;unsigned _tmp684;void*_tmp683;struct Cyc_Absyn_Tqual _tmp682;void*_tmp681;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp680)->tag == 4U){_LLA: _tmp681=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp680)->f1).elt_type;_tmp682=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp680)->f1).tq;_tmp683=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp680)->f1).zero_term;_tmp684=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp680)->f1).zt_loc;_LLB: {void*et=_tmp681;struct Cyc_Absyn_Tqual tq=_tmp682;void*zt=_tmp683;unsigned ztl=_tmp684;
# 3806
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->tag=4U,(_tmp685->f1).elt_type=et,(_tmp685->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpAC9=Cyc_Absyn_uint_exp(0U,0U);(_tmp685->f1).num_elts=_tmpAC9;}),(_tmp685->f1).zero_term=zt,(_tmp685->f1).zt_loc=ztl;_tmp685;});
goto _LL9;}}else{_LLC: _LLD:
# 3809
 old_atts=({struct Cyc_List_List*_tmp687=_cycalloc(sizeof(*_tmp687));({void*_tmpACA=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->tag=6U,_tmp686->f1=0;_tmp686;});_tmp687->hd=_tmpACA;}),_tmp687->tl=old_atts;_tmp687;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->tag=4U,({void*_tmpACD=Cyc_Toc_void_star_type();(_tmp688->f1).elt_type=_tmpACD;}),({
struct Cyc_Absyn_Tqual _tmpACC=Cyc_Absyn_empty_tqual(0U);(_tmp688->f1).tq=_tmpACC;}),({
struct Cyc_Absyn_Exp*_tmpACB=Cyc_Absyn_uint_exp(0U,0U);(_tmp688->f1).num_elts=_tmpACB;}),(_tmp688->f1).zero_term=Cyc_Absyn_false_type,(_tmp688->f1).zt_loc=0U;_tmp688;});}_LL9:;}{
# 3816
struct Cyc_Absyn_Aggrfield*_tmp689=({struct Cyc_Absyn_Aggrfield*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->name=old_field->name,_tmp69A->tq=Cyc_Toc_mt_tq,({
# 3818
void*_tmpACF=Cyc_Toc_typ_to_c(old_type);_tmp69A->type=_tmpACF;}),_tmp69A->width=old_field->width,({
# 3820
struct Cyc_List_List*_tmpACE=Cyc_Atts_atts2c(old_atts);_tmp69A->attributes=_tmpACE;}),_tmp69A->requires_clause=0;_tmp69A;});
# 3816
struct Cyc_Absyn_Aggrfield*new_field=_tmp689;
# 3826
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp68A=new_field->type;void*T=_tmp68A;
struct _fat_ptr*_tmp68B=new_field->name;struct _fat_ptr*f=_tmp68B;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp697=({struct Cyc_String_pa_PrintArg_struct _tmp801;_tmp801.tag=0U,_tmp801.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp801;});struct Cyc_String_pa_PrintArg_struct _tmp698=({struct Cyc_String_pa_PrintArg_struct _tmp800;_tmp800.tag=0U,_tmp800.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp800;});void*_tmp695[2U];_tmp695[0]=& _tmp697,_tmp695[1]=& _tmp698;({struct _fat_ptr _tmpAD0=({const char*_tmp696="_union_%s_%s";_tag_fat(_tmp696,sizeof(char),13U);});Cyc_aprintf(_tmpAD0,_tag_fat(_tmp695,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp694=_cycalloc(sizeof(*_tmp694));*_tmp694=s;_tmp694;});
struct Cyc_Absyn_Aggrfield*_tmp68C=({struct Cyc_Absyn_Aggrfield*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->name=Cyc_Toc_val_sp,_tmp693->tq=Cyc_Toc_mt_tq,_tmp693->type=T,_tmp693->width=0,_tmp693->attributes=0,_tmp693->requires_clause=0;_tmp693;});struct Cyc_Absyn_Aggrfield*value_field=_tmp68C;
struct Cyc_Absyn_Aggrfield*_tmp68D=({struct Cyc_Absyn_Aggrfield*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->name=Cyc_Toc_tag_sp,_tmp692->tq=Cyc_Toc_mt_tq,_tmp692->type=Cyc_Absyn_sint_type,_tmp692->width=0,_tmp692->attributes=0,_tmp692->requires_clause=0;_tmp692;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp68D;
struct Cyc_Absyn_Aggrdecl*_tmp68E=({struct _fat_ptr*_tmpAD1=str;Cyc_Toc_make_c_struct_defn(_tmpAD1,({struct Cyc_Absyn_Aggrfield*_tmp691[2U];_tmp691[0]=tag_field,_tmp691[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp691,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp68E;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp690=_cycalloc(sizeof(*_tmp690));({struct Cyc_Absyn_Decl*_tmpAD2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=5U,_tmp68F->f1=ad2;_tmp68F;}),0U);_tmp690->hd=_tmpAD2;}),_tmp690->tl=Cyc_Toc_result_decls;_tmp690;});
({void*_tmpAD3=Cyc_Absyn_strct(str);new_field->type=_tmpAD3;});}
# 3837
new_fields=({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->hd=new_field,_tmp699->tl=new_fields;_tmp699;});}}}
# 3839
({struct Cyc_List_List*_tmpAD4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpAD4;});}}
# 3842
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));({struct Cyc_Absyn_Decl*_tmpAD6=({struct Cyc_Absyn_Decl*_tmp69C=_cycalloc(sizeof(*_tmp69C));({void*_tmpAD5=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->tag=5U,_tmp69B->f1=new_ad;_tmp69B;});_tmp69C->r=_tmpAD5;}),_tmp69C->loc=0U;_tmp69C;});_tmp69D->hd=_tmpAD6;}),_tmp69D->tl=Cyc_Toc_result_decls;_tmp69D;});
return 0;}}}}
# 3846
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3874 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3877
struct _tuple43 _tmp69F=*env;struct _tuple43 _stmttmp70=_tmp69F;struct _tuple43 _tmp6A0=_stmttmp70;struct Cyc_Absyn_Datatypedecl*_tmp6A2;struct Cyc_Set_Set**_tmp6A1;_LL1: _tmp6A1=(_tmp6A0.f1)->datatypes_so_far;_tmp6A2=_tmp6A0.f2;_LL2: {struct Cyc_Set_Set**datatypes_so_far=_tmp6A1;struct Cyc_Absyn_Datatypedecl*tud=_tmp6A2;
struct _tuple1*_tmp6A3=tud->name;struct _tuple1*n=_tmp6A3;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpAD7=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpAD7;});
{struct Cyc_List_List*_tmp6A4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp6A4;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3885
struct Cyc_List_List*_tmp6A5=0;struct Cyc_List_List*fs=_tmp6A5;
int i=1;
{struct Cyc_List_List*_tmp6A6=f->typs;struct Cyc_List_List*ts=_tmp6A6;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp6A7=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp6A7;
struct Cyc_Absyn_Aggrfield*_tmp6A8=({struct Cyc_Absyn_Aggrfield*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->name=fname,_tmp6AA->tq=(*((struct _tuple23*)ts->hd)).f1,({
void*_tmpAD8=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f2);_tmp6AA->type=_tmpAD8;}),_tmp6AA->width=0,_tmp6AA->attributes=0,_tmp6AA->requires_clause=0;_tmp6AA;});
# 3889
struct Cyc_Absyn_Aggrfield*f=_tmp6A8;
# 3891
fs=({struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=f,_tmp6A9->tl=fs;_tmp6A9;});}}
# 3893
fs=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));({struct Cyc_Absyn_Aggrfield*_tmpADA=({struct Cyc_Absyn_Aggrfield*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->name=Cyc_Toc_tag_sp,_tmp6AB->tq=Cyc_Toc_mt_tq,_tmp6AB->type=Cyc_Absyn_sint_type,_tmp6AB->width=0,_tmp6AB->attributes=0,_tmp6AB->requires_clause=0;_tmp6AB;});_tmp6AC->hd=_tmpADA;}),({
struct Cyc_List_List*_tmpAD9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);_tmp6AC->tl=_tmpAD9;});_tmp6AC;});{
struct Cyc_Absyn_Aggrdecl*_tmp6AD=({struct _fat_ptr*_tmpADC=({struct _fat_ptr*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));({struct _fat_ptr _tmpADB=({const char*_tmp6B0="";_tag_fat(_tmp6B0,sizeof(char),1U);});*_tmp6B1=_tmpADB;});_tmp6B1;});Cyc_Toc_make_c_struct_defn(_tmpADC,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp6AD;
({struct _tuple1*_tmpADD=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpADD;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));({struct Cyc_Absyn_Decl*_tmpADE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE->tag=5U,_tmp6AE->f1=ad;_tmp6AE;}),0U);_tmp6AF->hd=_tmpADE;}),_tmp6AF->tl=Cyc_Toc_result_decls;_tmp6AF;});}}}
# 3899
return 0;}}
# 3902
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3921 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3924
struct _tuple43 _tmp6B2=*env;struct _tuple43 _stmttmp71=_tmp6B2;struct _tuple43 _tmp6B3=_stmttmp71;struct Cyc_Absyn_Datatypedecl*_tmp6B5;struct Cyc_Toc_TocState*_tmp6B4;_LL1: _tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp6B4;struct Cyc_Absyn_Datatypedecl*xd=_tmp6B5;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6B6=*s;struct Cyc_Toc_TocState _stmttmp72=_tmp6B6;struct Cyc_Toc_TocState _tmp6B7=_stmttmp72;struct Cyc_Dict_Dict*_tmp6B8;_LL4: _tmp6B8=_tmp6B7.xdatatypes_so_far;_LL5: {struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp6B8;
struct _tuple1*_tmp6B9=xd->name;struct _tuple1*n=_tmp6B9;
{struct Cyc_List_List*_tmp6BA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp6BA;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6BB=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp6BB;
void*_tmp6BC=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp6BC;
# 3935
int*_tmp6BD=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp73=_tmp6BD;int*_tmp6BE=_stmttmp73;if(_tmp6BE == 0){_LL7: _LL8: {
# 3937
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3941
struct Cyc_Absyn_Vardecl*_tmp6BF=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6BF;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));({struct Cyc_Absyn_Decl*_tmpADF=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0->tag=0U,_tmp6C0->f1=tag_decl;_tmp6C0;}),0U);_tmp6C1->hd=_tmpADF;}),_tmp6C1->tl=Cyc_Toc_result_decls;_tmp6C1;});
({struct Cyc_Dict_Dict _tmpAE0=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3944
*xdatatypes_so_far=_tmpAE0;});{
# 3946
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6C2=f->typs;struct Cyc_List_List*tqts=_tmp6C2;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp6C3=({struct _fat_ptr*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));({struct _fat_ptr _tmpAE2=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C9=({struct Cyc_Int_pa_PrintArg_struct _tmp802;_tmp802.tag=1U,_tmp802.f1=(unsigned long)i;_tmp802;});void*_tmp6C7[1U];_tmp6C7[0]=& _tmp6C9;({struct _fat_ptr _tmpAE1=({const char*_tmp6C8="f%d";_tag_fat(_tmp6C8,sizeof(char),4U);});Cyc_aprintf(_tmpAE1,_tag_fat(_tmp6C7,sizeof(void*),1U));});});*_tmp6CA=_tmpAE2;});_tmp6CA;});struct _fat_ptr*field_n=_tmp6C3;
struct Cyc_Absyn_Aggrfield*_tmp6C4=({struct Cyc_Absyn_Aggrfield*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->name=field_n,_tmp6C6->tq=(*((struct _tuple23*)tqts->hd)).f1,({
void*_tmpAE3=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f2);_tmp6C6->type=_tmpAE3;}),_tmp6C6->width=0,_tmp6C6->attributes=0,_tmp6C6->requires_clause=0;_tmp6C6;});
# 3950
struct Cyc_Absyn_Aggrfield*newf=_tmp6C4;
# 3952
fields=({struct Cyc_List_List*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->hd=newf,_tmp6C5->tl=fields;_tmp6C5;});}}
# 3954
fields=({struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));({struct Cyc_Absyn_Aggrfield*_tmpAE6=({struct Cyc_Absyn_Aggrfield*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->name=Cyc_Toc_tag_sp,_tmp6CB->tq=Cyc_Toc_mt_tq,({
void*_tmpAE5=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6CB->type=_tmpAE5;}),_tmp6CB->width=0,_tmp6CB->attributes=0,_tmp6CB->requires_clause=0;_tmp6CB;});
# 3954
_tmp6CC->hd=_tmpAE6;}),({
# 3956
struct Cyc_List_List*_tmpAE4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6CC->tl=_tmpAE4;});_tmp6CC;});{
struct Cyc_Absyn_Aggrdecl*_tmp6CD=({struct _fat_ptr*_tmpAE8=({struct _fat_ptr*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({struct _fat_ptr _tmpAE7=({const char*_tmp6D0="";_tag_fat(_tmp6D0,sizeof(char),1U);});*_tmp6D1=_tmpAE7;});_tmp6D1;});Cyc_Toc_make_c_struct_defn(_tmpAE8,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp6CD;
({struct _tuple1*_tmpAE9=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpAE9;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));({struct Cyc_Absyn_Decl*_tmpAEA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->tag=5U,_tmp6CE->f1=strct_decl;_tmp6CE;}),0U);_tmp6CF->hd=_tmpAEA;}),_tmp6CF->tl=Cyc_Toc_result_decls;_tmp6CF;});
goto _LL6;}}}}}else{if(*((int*)_tmp6BE)== 0){_LL9: _LLA:
# 3962
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6D2=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp6D2;
struct Cyc_Absyn_Vardecl*_tmp6D3=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6D3;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));({struct Cyc_Absyn_Decl*_tmpAEB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->tag=0U,_tmp6D4->f1=tag_decl;_tmp6D4;}),0U);_tmp6D5->hd=_tmpAEB;}),_tmp6D5->tl=Cyc_Toc_result_decls;_tmp6D5;});
({struct Cyc_Dict_Dict _tmpAEC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpAEC;});}
# 3969
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3973
return 0;}}}}
# 3976
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3980
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3986
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpAED=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpAED;});
# 3990
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3999
struct _RegionHandle _tmp6D6=_new_region("temp");struct _RegionHandle*temp=& _tmp6D6;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D7=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp6D7;
Cyc_Toc_exp_to_c(nv2,init);}
# 4000
;_pop_region();}else{
# 4004
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4007
void*_tmp6D8=Cyc_Absyn_compress(old_typ);void*_stmttmp74=_tmp6D8;void*_tmp6D9=_stmttmp74;void*_tmp6DC;struct Cyc_Absyn_Exp*_tmp6DB;void*_tmp6DA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D9)->tag == 4U){_LL1: _tmp6DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D9)->f1).elt_type;_tmp6DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D9)->f1).num_elts;_tmp6DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D9)->f1).zero_term;_LL2: {void*et=_tmp6DA;struct Cyc_Absyn_Exp*num_elts_opt=_tmp6DB;void*zt=_tmp6DC;
# 4009
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp803;_tmp803.tag=0U,({struct _fat_ptr _tmpAEE=({const char*_tmp6DF="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp6DF,sizeof(char),55U);});_tmp803.f1=_tmpAEE;});_tmp803;});void*_tmp6DD[1U];_tmp6DD[0]=& _tmp6DE;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6DD,sizeof(void*),1U));});{
struct Cyc_Absyn_Exp*num_elts=num_elts_opt;
struct Cyc_Absyn_Exp*_tmp6E0=({struct Cyc_Absyn_Exp*_tmpAF0=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpAF0,({
struct Cyc_Absyn_Exp*_tmpAEF=num_elts;Cyc_Absyn_add_exp(_tmpAEF,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4013
struct Cyc_Absyn_Exp*lhs=_tmp6E0;
# 4016
struct Cyc_Absyn_Exp*_tmp6E1=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp6E1;
({void*_tmpAF2=({struct Cyc_Absyn_Stmt*_tmpAF1=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAF1,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4017
s->r=_tmpAF2;});}}
# 4020
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4028
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6E2=d;void**_tmp6E5;struct Cyc_List_List*_tmp6E4;struct Cyc_List_List*_tmp6E3;struct Cyc_Tcpat_Rhs*_tmp6E6;switch(*((int*)_tmp6E2)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6E6=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6E2)->f1;_LL4: {struct Cyc_Tcpat_Rhs*rhs=_tmp6E6;
rhs->rhs=success;return d;}default: _LL5: _tmp6E3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E2)->f1;_tmp6E4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E2)->f2;_tmp6E5=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E2)->f3;_LL6: {struct Cyc_List_List*path=_tmp6E3;struct Cyc_List_List*sws=_tmp6E4;void**d2=_tmp6E5;
# 4033
({void*_tmpAF3=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAF3;});
for(0;sws != 0;sws=sws->tl){
struct _tuple34*_tmp6E7=(struct _tuple34*)sws->hd;struct _tuple34*_stmttmp75=_tmp6E7;struct _tuple34*_tmp6E8=_stmttmp75;void**_tmp6E9;_LL8: _tmp6E9=(void**)& _tmp6E8->f2;_LL9: {void**d2=_tmp6E9;
({void*_tmpAF4=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAF4;});}}
# 4038
return d;}}_LL0:;}
# 4049 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4051
struct _RegionHandle _tmp6EA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6EA;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6EB=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp6EB;
void*_tmp6EC=(void*)_check_null(e->topt);void*t=_tmp6EC;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4061
struct Cyc_Absyn_Stmt*_tmp6ED=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6ED;
if(dopt != 0)
# 4065
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4067
struct Cyc_Absyn_Switch_clause*_tmp6EE=({struct Cyc_Absyn_Switch_clause*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD->pattern=p,_tmp6FD->pat_vars=0,_tmp6FD->where_clause=0,_tmp6FD->body=succ_stmt,_tmp6FD->loc=0U;_tmp6FD;});struct Cyc_Absyn_Switch_clause*c1=_tmp6EE;
struct Cyc_List_List*_tmp6EF=({struct _RegionHandle*_tmpAF7=rgn;struct _RegionHandle*_tmpAF6=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpAF7,Cyc_Toc_gen_labels,_tmpAF6,({struct Cyc_Absyn_Switch_clause*_tmp6FC[1U];_tmp6FC[0]=c1;({struct _RegionHandle*_tmpAF5=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpAF5,_tag_fat(_tmp6FC,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});struct Cyc_List_List*lscs=_tmp6EF;
# 4070
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4074
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4077
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple41*_tmp6F0=(struct _tuple41*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple41*_stmttmp76=_tmp6F0;struct _tuple41*_tmp6F1=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp6F3;struct Cyc_Toc_Env*_tmp6F2;_LL1: _tmp6F2=_tmp6F1->f1;_tmp6F3=_tmp6F1->f3;_LL2: {struct Cyc_Toc_Env*env=_tmp6F2;struct Cyc_Absyn_Stmt*st=_tmp6F3;
if(st == succ_stmt){senv=env;goto FOUND_ENV;}}}
# 4082
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp804;_tmp804.tag=0U,({struct _fat_ptr _tmpAF8=({const char*_tmp6F6="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6F6,sizeof(char),33U);});_tmp804.f1=_tmpAF8;});_tmp804;});void*_tmp6F4[1U];_tmp6F4[0]=& _tmp6F5;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6F4,sizeof(void*),1U));});
FOUND_ENV:
# 4086
 Cyc_Toc_stmt_to_c(senv,s);{
# 4088
struct Cyc_Absyn_Stmt*res=test_tree;
# 4090
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp6F7=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp77=_tmp6F7;struct _tuple39 _tmp6F8=_stmttmp77;struct Cyc_Absyn_Vardecl*_tmp6F9;_LL4: _tmp6F9=_tmp6F8.f2;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp6F9;
res=({struct Cyc_Absyn_Decl*_tmpAF9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->tag=0U,_tmp6FA->f1=vd;_tmp6FA;}),0U);Cyc_Absyn_decl_stmt(_tmpAF9,res,0U);});}}
# 4095
res=({struct _tuple1*_tmpAFC=v;void*_tmpAFB=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpAFA=e;Cyc_Absyn_declare_stmt(_tmpAFC,_tmpAFB,_tmpAFA,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6FB=res;_npop_handler(0U);return _tmp6FB;}}}}}
# 4052
;_pop_region();}
# 4104
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6FE=e->r;void*_stmttmp78=_tmp6FE;void*_tmp6FF=_stmttmp78;struct Cyc_List_List*_tmp701;struct Cyc_List_List*_tmp700;struct Cyc_Absyn_MallocInfo*_tmp702;struct Cyc_Absyn_Stmt*_tmp703;void**_tmp704;void**_tmp705;struct Cyc_List_List*_tmp706;struct Cyc_List_List*_tmp707;struct Cyc_List_List*_tmp709;void**_tmp708;struct Cyc_Absyn_Exp*_tmp70B;void**_tmp70A;struct Cyc_List_List*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp71A;struct Cyc_Absyn_Exp*_tmp719;struct Cyc_Absyn_Exp*_tmp71C;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_List_List*_tmp71D;struct Cyc_Absyn_Exp*_tmp71E;struct Cyc_Absyn_Exp*_tmp71F;struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp721;struct Cyc_Absyn_Exp*_tmp722;struct Cyc_Absyn_Exp*_tmp723;struct Cyc_Absyn_Exp*_tmp724;struct Cyc_Absyn_Exp*_tmp725;struct Cyc_Absyn_Exp*_tmp726;switch(*((int*)_tmp6FF)){case 41U: _LL1: _tmp726=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp726;
_tmp725=e;goto _LL4;}case 20U: _LL3: _tmp725=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp725;
_tmp724=e;goto _LL6;}case 21U: _LL5: _tmp724=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp724;
_tmp723=e;goto _LL8;}case 22U: _LL7: _tmp723=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp723;
_tmp722=e;goto _LLA;}case 15U: _LL9: _tmp722=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp722;
_tmp721=e;goto _LLC;}case 11U: _LLB: _tmp721=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp721;
_tmp720=e;goto _LLE;}case 12U: _LLD: _tmp720=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp720;
_tmp71F=e;goto _LL10;}case 18U: _LLF: _tmp71F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp71F;
_tmp71E=e;goto _LL12;}case 5U: _LL11: _tmp71E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp71E;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3U: _LL13: _tmp71D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL14: {struct Cyc_List_List*es=_tmp71D;
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7U: _LL15: _tmp71B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp71C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp71B;struct Cyc_Absyn_Exp*e2=_tmp71C;
_tmp719=e1;_tmp71A=e2;goto _LL18;}case 8U: _LL17: _tmp719=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp71A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp719;struct Cyc_Absyn_Exp*e2=_tmp71A;
_tmp717=e1;_tmp718=e2;goto _LL1A;}case 9U: _LL19: _tmp717=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp718=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp717;struct Cyc_Absyn_Exp*e2=_tmp718;
_tmp715=e1;_tmp716=e2;goto _LL1C;}case 23U: _LL1B: _tmp715=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp716=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp715;struct Cyc_Absyn_Exp*e2=_tmp716;
_tmp713=e1;_tmp714=e2;goto _LL1E;}case 35U: _LL1D: _tmp713=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp714=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp713;struct Cyc_Absyn_Exp*e2=_tmp714;
_tmp711=e1;_tmp712=e2;goto _LL20;}case 4U: _LL1F: _tmp711=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp712=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FF)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp711;struct Cyc_Absyn_Exp*e2=_tmp712;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6U: _LL21: _tmp70E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp70F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_tmp710=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FF)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp70E;struct Cyc_Absyn_Exp*e2=_tmp70F;struct Cyc_Absyn_Exp*e3=_tmp710;
# 4123
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10U: _LL23: _tmp70C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp70D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp70C;struct Cyc_List_List*es=_tmp70D;
# 4125
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14U: _LL25: _tmp70A=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_tmp70B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL26: {void**t=_tmp70A;struct Cyc_Absyn_Exp*e=_tmp70B;
({void*_tmpAFD=Cyc_Toc_typ_to_c(*t);*t=_tmpAFD;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25U: _LL27: _tmp708=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1)->f3;_tmp709=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL28: {void**t=_tmp708;struct Cyc_List_List*dles=_tmp709;
# 4128
({void*_tmpAFE=Cyc_Toc_typ_to_c(*t);*t=_tmpAFE;});
_tmp707=dles;goto _LL2A;}case 36U: _LL29: _tmp707=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6FF)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp707;
_tmp706=dles;goto _LL2C;}case 26U: _LL2B: _tmp706=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp706;
# 4132
for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19U: _LL2D: _tmp705=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL2E: {void**t=_tmp705;
_tmp704=t;goto _LL30;}case 17U: _LL2F: _tmp704=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL30: {void**t=_tmp704;
({void*_tmpAFF=Cyc_Toc_typ_to_c(*t);*t=_tmpAFF;});goto _LL0;}case 37U: _LL31: _tmp703=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL32: {struct Cyc_Absyn_Stmt*s=_tmp703;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34U: _LL33: _tmp702=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6FF)->f1;_LL34: {struct Cyc_Absyn_MallocInfo*m=_tmp702;
# 4139
if(m->elt_type != 0)
({void**_tmpB01=({void**_tmp727=_cycalloc(sizeof(*_tmp727));({void*_tmpB00=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp727=_tmpB00;});_tmp727;});m->elt_type=_tmpB01;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40U: _LL35: _tmp700=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FF)->f3;_tmp701=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FF)->f4;_LL36: {struct Cyc_List_List*o=_tmp700;struct Cyc_List_List*i=_tmp701;
# 4144
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0U: _LL37: _LL38:
 goto _LL3A;case 1U: _LL39: _LL3A:
 goto _LL3C;case 32U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 42U: _LL3F: _LL40:
# 4152
 goto _LL42;case 2U: _LL41: _LL42:
 goto _LL44;case 30U: _LL43: _LL44:
 goto _LL46;case 31U: _LL45: _LL46:
 goto _LL48;case 29U: _LL47: _LL48:
 goto _LL4A;case 27U: _LL49: _LL4A:
 goto _LL4C;case 28U: _LL4B: _LL4C:
 goto _LL4E;case 24U: _LL4D: _LL4E:
 goto _LL50;case 13U: _LL4F: _LL50:
 goto _LL52;case 16U: _LL51: _LL52:
 goto _LL54;case 39U: _LL53: _LL54:
 goto _LL56;default: _LL55: _LL56:
({void*_tmp728=0U;({unsigned _tmpB03=e->loc;struct _fat_ptr _tmpB02=({const char*_tmp729="Cyclone expression in C code";_tag_fat(_tmp729,sizeof(char),29U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB03,_tmpB02,_tag_fat(_tmp728,sizeof(void*),0U));});});}_LL0:;}
# 4167
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp72A=d->r;void*_stmttmp79=_tmp72A;void*_tmp72B=_stmttmp79;struct Cyc_Absyn_Typedefdecl*_tmp72C;struct Cyc_Absyn_Enumdecl*_tmp72D;struct Cyc_Absyn_Aggrdecl*_tmp72E;struct Cyc_Absyn_Fndecl*_tmp72F;struct Cyc_Absyn_Vardecl*_tmp730;switch(*((int*)_tmp72B)){case 0U: _LL1: _tmp730=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp72B)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp730;
# 4170
({void*_tmpB04=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpB04;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp72F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp72B)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp72F;
# 4174
({void*_tmpB05=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpB05;});
{struct Cyc_List_List*_tmp731=(fd->i).args;struct Cyc_List_List*args=_tmp731;for(0;args != 0;args=args->tl){
({void*_tmpB06=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpB06;});}}
goto _LL0;}case 5U: _LL5: _tmp72E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp72B)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp72E;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7U: _LL7: _tmp72D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp72B)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp72D;
# 4180
if(ed->fields != 0){
struct Cyc_List_List*_tmp732=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp732;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp733=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp733;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4185
goto _LL0;}case 8U: _LL9: _tmp72C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp72B)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp72C;
({void*_tmpB07=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB07;});goto _LL0;}case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4195
({void*_tmp734=0U;({unsigned _tmpB09=d->loc;struct _fat_ptr _tmpB08=({const char*_tmp735="Cyclone declaration in C code";_tag_fat(_tmp735,sizeof(char),30U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB09,_tmpB08,_tag_fat(_tmp734,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4203
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp736=s->r;void*_stmttmp7A=_tmp736;void*_tmp737=_stmttmp7A;struct Cyc_Absyn_Stmt*_tmp738;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Stmt*_tmp739;struct Cyc_Absyn_Stmt*_tmp73C;struct Cyc_Absyn_Decl*_tmp73B;void*_tmp73F;struct Cyc_List_List*_tmp73E;struct Cyc_Absyn_Exp*_tmp73D;struct Cyc_Absyn_Stmt*_tmp743;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Exp*_tmp741;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_Absyn_Stmt*_tmp745;struct Cyc_Absyn_Exp*_tmp744;struct Cyc_Absyn_Stmt*_tmp748;struct Cyc_Absyn_Stmt*_tmp747;struct Cyc_Absyn_Exp*_tmp746;struct Cyc_Absyn_Exp*_tmp749;struct Cyc_Absyn_Stmt*_tmp74B;struct Cyc_Absyn_Stmt*_tmp74A;struct Cyc_Absyn_Exp*_tmp74C;switch(*((int*)_tmp737)){case 1U: _LL1: _tmp74C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp74C;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2U: _LL3: _tmp74A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_tmp74B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp737)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp74A;struct Cyc_Absyn_Stmt*s2=_tmp74B;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3U: _LL5: _tmp749=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_LL6: {struct Cyc_Absyn_Exp*eopt=_tmp749;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4U: _LL7: _tmp746=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_tmp747=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp737)->f2;_tmp748=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp737)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp746;struct Cyc_Absyn_Stmt*s1=_tmp747;struct Cyc_Absyn_Stmt*s2=_tmp748;
# 4209
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5U: _LL9: _tmp744=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp737)->f1).f1;_tmp745=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp737)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp744;struct Cyc_Absyn_Stmt*s=_tmp745;
# 4211
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9U: _LLB: _tmp740=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_tmp741=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp737)->f2).f1;_tmp742=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp737)->f3).f1;_tmp743=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp737)->f4;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp740;struct Cyc_Absyn_Exp*e2=_tmp741;struct Cyc_Absyn_Exp*e3=_tmp742;struct Cyc_Absyn_Stmt*s=_tmp743;
# 4213
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10U: _LLD: _tmp73D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_tmp73E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp737)->f2;_tmp73F=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp737)->f3;_LLE: {struct Cyc_Absyn_Exp*e=_tmp73D;struct Cyc_List_List*scs=_tmp73E;void*dec_tree=_tmp73F;
# 4216
Cyc_Toc_exptypes_to_c(e);
for(0;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12U: _LLF: _tmp73B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_tmp73C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp737)->f2;_LL10: {struct Cyc_Absyn_Decl*d=_tmp73B;struct Cyc_Absyn_Stmt*s=_tmp73C;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14U: _LL11: _tmp739=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp737)->f1;_tmp73A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp737)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp739;struct Cyc_Absyn_Exp*e=_tmp73A;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13U: _LL13: _tmp738=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp737)->f2;_LL14: {struct Cyc_Absyn_Stmt*s=_tmp738;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4228
({void*_tmpB0A=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->tag=0U;_tmp74D;});s->r=_tmpB0A;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp74E=0U;({unsigned _tmpB0C=s->loc;struct _fat_ptr _tmpB0B=({const char*_tmp74F="Cyclone statement in C code";_tag_fat(_tmp74F,sizeof(char),28U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB0C,_tmpB0B,_tag_fat(_tmp74E,sizeof(void*),0U));});});}_LL0:;}
# 4236
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4238
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp751=({struct Cyc_Warn_String_Warn_Warg_struct _tmp805;_tmp805.tag=0U,({struct _fat_ptr _tmpB0D=({const char*_tmp752="decls_to_c: not at toplevel!";_tag_fat(_tmp752,sizeof(char),29U);});_tmp805.f1=_tmpB0D;});_tmp805;});void*_tmp750[1U];_tmp750[0]=& _tmp751;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp750,sizeof(void*),1U));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp753=d->r;void*_stmttmp7B=_tmp753;void*_tmp754=_stmttmp7B;struct Cyc_List_List*_tmp755;struct Cyc_List_List*_tmp756;struct Cyc_List_List*_tmp757;struct Cyc_List_List*_tmp758;struct Cyc_Absyn_Typedefdecl*_tmp759;struct Cyc_Absyn_Enumdecl*_tmp75A;struct Cyc_Absyn_Datatypedecl*_tmp75B;struct Cyc_Absyn_Aggrdecl*_tmp75C;struct Cyc_Absyn_Fndecl*_tmp75D;struct Cyc_Absyn_Vardecl*_tmp75E;switch(*((int*)_tmp754)){case 0U: _LL1: _tmp75E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp75E;
# 4245
struct _tuple1*_tmp75F=vd->name;struct _tuple1*c_name=_tmp75F;
# 4247
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp760=_cycalloc(sizeof(*_tmp760));({union Cyc_Absyn_Nmspace _tmpB0E=Cyc_Absyn_Abs_n(0,1);_tmp760->f1=_tmpB0E;}),_tmp760->f2=(*c_name).f2;_tmp760;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4254
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4256
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpB0F=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpB0F;});
({void*_tmpB10=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpB10;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761->hd=d,_tmp761->tl=Cyc_Toc_result_decls;_tmp761;});
goto _LL0;}case 1U: _LL3: _tmp75D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp75D;
# 4263
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB12=({struct _tuple1*_tmp762=_cycalloc(sizeof(*_tmp762));({union Cyc_Absyn_Nmspace _tmpB11=Cyc_Absyn_Abs_n(0,1);_tmp762->f1=_tmpB11;}),_tmp762->f2=(*fd->name).f2;_tmp762;});fd->name=_tmpB12;});
fd->sc=Cyc_Absyn_Public;}
# 4267
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763->hd=d,_tmp763->tl=Cyc_Toc_result_decls;_tmp763;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp765=({struct Cyc_Warn_String_Warn_Warg_struct _tmp806;_tmp806.tag=0U,({struct _fat_ptr _tmpB13=({const char*_tmp766="letdecl at toplevel";_tag_fat(_tmp766,sizeof(char),20U);});_tmp806.f1=_tmpB13;});_tmp806;});void*_tmp764[1U];_tmp764[0]=& _tmp765;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp764,sizeof(void*),1U));});case 4U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp768=({struct Cyc_Warn_String_Warn_Warg_struct _tmp807;_tmp807.tag=0U,({struct _fat_ptr _tmpB14=({const char*_tmp769="region decl at toplevel";_tag_fat(_tmp769,sizeof(char),24U);});_tmp807.f1=_tmpB14;});_tmp807;});void*_tmp767[1U];_tmp767[0]=& _tmp768;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp767,sizeof(void*),1U));});case 5U: _LLB: _tmp75C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*sd=_tmp75C;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6U: _LLD: _tmp75B=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LLE: {struct Cyc_Absyn_Datatypedecl*tud=_tmp75B;
# 4275
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7U: _LLF: _tmp75A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp75A;
# 4278
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A->hd=d,_tmp76A->tl=Cyc_Toc_result_decls;_tmp76A;});
goto _LL0;}case 8U: _LL11: _tmp759=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp759;
# 4282
td->tvs=0;
if(td->defn != 0){
({void*_tmpB15=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB15;});{
# 4287
struct Cyc_Absyn_Decl*ed;
{void*_tmp76B=td->defn;void*_stmttmp7C=_tmp76B;void*_tmp76C=_stmttmp7C;unsigned _tmp76E;struct Cyc_Absyn_Enumdecl*_tmp76D;if(_tmp76C != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76C)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76C)->f1)->r)->tag == 1U){_LL24: _tmp76D=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76C)->f1)->r)->f1;_tmp76E=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76C)->f1)->loc;_LL25: {struct Cyc_Absyn_Enumdecl*ed2=_tmp76D;unsigned loc=_tmp76E;
# 4290
ed=({struct Cyc_Absyn_Decl*_tmp770=_cycalloc(sizeof(*_tmp770));({void*_tmpB16=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F->tag=7U,_tmp76F->f1=ed2;_tmp76F;});_tmp770->r=_tmpB16;}),_tmp770->loc=loc;_tmp770;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4293
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->hd=ed,_tmp771->tl=Cyc_Toc_result_decls;_tmp771;});{
void*_tmp772=ed->r;void*_stmttmp7D=_tmp772;void*_tmp773=_stmttmp7D;struct Cyc_Absyn_Enumdecl*_tmp774;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp773)->tag == 7U){_LL29: _tmp774=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp773)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp774;
# 4297
({void*_tmpB18=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776->tag=0U,({void*_tmpB17=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775->tag=15U,_tmp775->f1=ed->name,_tmp775->f2=ed;_tmp775;});_tmp776->f1=_tmpB17;}),_tmp776->f2=0;_tmp776;});td->defn=_tmpB18;});goto _LL28;}}else{_LL2B: _LL2C:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp778=({struct Cyc_Warn_String_Warn_Warg_struct _tmp808;_tmp808.tag=0U,({struct _fat_ptr _tmpB19=({const char*_tmp779="Toc: enumdecl to name";_tag_fat(_tmp779,sizeof(char),22U);});_tmp808.f1=_tmpB19;});_tmp808;});void*_tmp777[1U];_tmp777[0]=& _tmp778;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp777,sizeof(void*),1U));});}_LL28:;}}}}else{
# 4302
enum Cyc_Absyn_KindQual _tmp77A=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7E=_tmp77A;enum Cyc_Absyn_KindQual _tmp77B=_stmttmp7E;if(_tmp77B == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpB1A=Cyc_Toc_void_star_type();td->defn=_tmpB1A;});goto _LL2D;}else{_LL30: _LL31:
 td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4310
if(Cyc_Flags_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C->hd=d,_tmp77C->tl=Cyc_Toc_result_decls;_tmp77C;});
goto _LL0;}case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp758=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp754)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp758;
_tmp757=ds2;goto _LL1E;}case 10U: _LL1D: _tmp757=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp754)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp757;
_tmp756=ds2;goto _LL20;}case 11U: _LL1F: _tmp756=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp756;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _LL21: _tmp755=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp754)->f1;_LL22: {struct Cyc_List_List*ds2=_tmp755;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4323
return nv;}
# 4328
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _tmp77D=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp7F=_tmp77D;struct Cyc_Core_NewDynamicRegion _tmp77E=_stmttmp7F;struct Cyc_Core_DynamicRegion*_tmp77F;_LL1: _tmp77F=_tmp77E.key;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp77F;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp781=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp781));_tmp781->dyn=dyn,_tmp781->state=ts;_tmp781;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp783=34;struct _fat_ptr**_tmp782=_cycalloc(_check_times(_tmp783,sizeof(struct _fat_ptr*)));_tmp782[0]=& Cyc_Toc__throw_str,_tmp782[1]=& Cyc_Toc_setjmp_str,_tmp782[2]=& Cyc_Toc__push_handler_str,_tmp782[3]=& Cyc_Toc__pop_handler_str,_tmp782[4]=& Cyc_Toc__exn_thrown_str,_tmp782[5]=& Cyc_Toc__npop_handler_str,_tmp782[6]=& Cyc_Toc__check_null_str,_tmp782[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp782[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp782[9]=& Cyc_Toc__check_fat_subscript_str,_tmp782[10]=& Cyc_Toc__tag_fat_str,_tmp782[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp782[12]=& Cyc_Toc__get_fat_size_str,_tmp782[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp782[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp782[15]=& Cyc_Toc__zero_arr_plus_str,_tmp782[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp782[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp782[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp782[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp782[20]=& Cyc_Toc__cycalloc_str,_tmp782[21]=& Cyc_Toc__cyccalloc_str,_tmp782[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp782[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp782[24]=& Cyc_Toc__region_malloc_str,_tmp782[25]=& Cyc_Toc__region_calloc_str,_tmp782[26]=& Cyc_Toc__check_times_str,_tmp782[27]=& Cyc_Toc__new_region_str,_tmp782[28]=& Cyc_Toc__push_region_str,_tmp782[29]=& Cyc_Toc__pop_region_str,_tmp782[30]=& Cyc_Toc__throw_arraybounds_str,_tmp782[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp782[32]=& Cyc_Toc__throw_match_str,_tmp782[33]=& Cyc_Toc__fast_region_malloc_str;_tmp782;}),sizeof(struct _fat_ptr*),34U);}}
# 4376
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp784=ts;struct Cyc_Toc_TocStateWrap*_tmp785=Cyc_Toc_toc_state;ts=_tmp785;Cyc_Toc_toc_state=_tmp784;});{
struct Cyc_Toc_TocStateWrap _tmp786=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp80=_tmp786;struct Cyc_Toc_TocStateWrap _tmp787=_stmttmp80;struct Cyc_Toc_TocState*_tmp789;struct Cyc_Core_DynamicRegion*_tmp788;_LL1: _tmp788=_tmp787.dyn;_tmp789=_tmp787.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp788;struct Cyc_Toc_TocState*s=_tmp789;
# 4381
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp78B=*s;struct Cyc_Toc_TocState _stmttmp81=_tmp78B;struct Cyc_Toc_TocState _tmp78C=_stmttmp81;struct Cyc_Xarray_Xarray*_tmp78D;_LL4: _tmp78D=_tmp78C.temp_labels;_LL5: {struct Cyc_Xarray_Xarray*tls=_tmp78D;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(tls);}}
# 4382
;}
# 4385
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4388
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4394
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4396
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp78E=_cycalloc(sizeof(*_tmp78E));*_tmp78E=pop_tables;_tmp78E;});
Cyc_Toc_init();{
struct _RegionHandle _tmp78F=_new_region("start");struct _RegionHandle*start=& _tmp78F;_push_region(start);
({struct _RegionHandle*_tmpB1C=start;struct Cyc_Toc_Env*_tmpB1B=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB1C,_tmpB1B,ds,0);});{
struct Cyc_List_List*_tmp790=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp790;}
# 4399
;_pop_region();}}
