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
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;
# 180
extern void Cyc_Core_ufree(void*ptr);
# 193
extern struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 242
extern struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 260 "core.h"
extern void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 311
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 696 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 875
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 882
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 891
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 916
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 918
extern void*Cyc_Absyn_sint_type;
# 923
extern void*Cyc_Absyn_heap_rgn_type;
# 927
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 929
extern void*Cyc_Absyn_void_type;
# 946
void*Cyc_Absyn_exn_type (void);
# 954
extern void*Cyc_Absyn_fat_bound_type;
# 958
void*Cyc_Absyn_bounds_one (void);
# 966
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 970
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 972
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 974
void*Cyc_Absyn_strct(struct _fat_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 980
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 985
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1060
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1065
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1077
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1082
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1086
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1091
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1129
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1134
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1144
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1146
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1148
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1157
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1165
extern int Cyc_Absyn_no_regions;
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
# 99
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 182
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 184
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 222
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 229
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);
# 280
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 291
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 57 "attributes.h"
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 34 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 36
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 41
struct _tuple1*Cyc_Toc_temp_var (void);
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_init (void);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 57 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 72 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp78D;_tmp78D.tag=0U,_tmp78D.f1=str;_tmp78D;});void*_tmp0[1U];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1U));});}
# 75
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 77
({struct _fat_ptr _tmp806=(struct _fat_ptr)({struct _fat_ptr _tmp805=({const char*_tmp2="Toc: ";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_strconcat(_tmp805,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp806,ap);});}
# 79
static void*Cyc_Toc_toc_impos2(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=0U,({struct _fat_ptr _tmp807=({const char*_tmp5="Toc: ";_tag_fat(_tmp5,sizeof(char),6U);});_tmp6->f1=_tmp807;});_tmp6;});
struct _fat_ptr args=({unsigned _tmp4=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned _tmp78E=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;unsigned i;for(i=0;i < _tmp78E;++ i){i == (unsigned)0?_tmp3[i]=toc:(_tmp3[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - (unsigned)1))));}}0;});_tag_fat(_tmp3,sizeof(void*),_tmp4);});
Cyc_Warn_vimpos2(args);}
# 85
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 88
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 98
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 100
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 126
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 133
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmpE=_region_malloc(d,sizeof(*_tmpE));
({struct Cyc_List_List**_tmp813=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpE->tuple_types=_tmp813;}),({
struct Cyc_List_List**_tmp812=({struct Cyc_List_List**_tmp8=_region_malloc(d,sizeof(*_tmp8));*_tmp8=0;_tmp8;});_tmpE->anon_aggr_types=_tmp812;}),({
struct Cyc_Dict_Dict*_tmp811=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));({struct Cyc_Dict_Dict _tmp810=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp9=_tmp810;});_tmp9;});_tmpE->aggrs_so_far=_tmp811;}),({
struct Cyc_List_List**_tmp80F=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmpE->abs_struct_types=_tmp80F;}),({
struct Cyc_Set_Set**_tmp80E=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Set_Set*_tmp80D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp80D;});_tmpB;});_tmpE->datatypes_so_far=_tmp80E;}),({
struct Cyc_Dict_Dict*_tmp80C=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp80B=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp80B;});_tmpC;});_tmpE->xdatatypes_so_far=_tmp80C;}),({
struct Cyc_Dict_Dict*_tmp80A=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Dict_Dict _tmp809=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmpD=_tmp809;});_tmpD;});_tmpE->qvar_tags=_tmp80A;}),({
struct Cyc_Xarray_Xarray*_tmp808=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpE->temp_labels=_tmp808;});_tmpE;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 152
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 158
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 161
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
struct Cyc_Toc_TocStateWrap _tmp11=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp11;struct Cyc_Toc_TocStateWrap _tmp12=_stmttmp0;struct Cyc_Toc_TocState*_tmp14;struct Cyc_Core_DynamicRegion*_tmp13;_LL1: _tmp13=_tmp12.dyn;_tmp14=_tmp12.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp13;struct Cyc_Toc_TocState*s=_tmp14;
struct _fat_ptr _tmp15=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp15;
({struct Cyc_Toc_TocStateWrap _tmp814=({struct Cyc_Toc_TocStateWrap _tmp78F;_tmp78F.dyn=dyn,_tmp78F.state=s;_tmp78F;});*ts=_tmp814;});
({struct Cyc_Toc_TocStateWrap*_tmp16=ts;struct Cyc_Toc_TocStateWrap*_tmp17=Cyc_Toc_toc_state;ts=_tmp17;Cyc_Toc_toc_state=_tmp16;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp19=({struct _tuple15 _tmp790;_tmp790.f1=s,_tmp790.f2=arg;_tmp790;});struct _tuple15 env=_tmp19;
res=f(h,& env);}
# 169
;}
# 171
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 175
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 178
struct _tuple17 _tmp1A=*env;struct _tuple17 _stmttmp1=_tmp1A;struct _tuple17 _tmp1B=_stmttmp1;void*(*_tmp1E)(struct _tuple1*);struct _tuple1*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1C=_tmp1B.f1;_tmp1D=(_tmp1B.f2)->f1;_tmp1E=(_tmp1B.f2)->f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp1C;struct _tuple1*q=_tmp1D;void*(*type_maker)(struct _tuple1*)=_tmp1E;
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 183
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp791;_tmp791.f1=q,_tmp791.f2=type_maker;_tmp791;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1F[5U]="curr";
# 196 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1F,_tmp1F,_tmp1F + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp20[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp20,_tmp20,_tmp20 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp21[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp21,_tmp21,_tmp21 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp22[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp22,_tmp22,_tmp22 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp23[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp23,_tmp23,_tmp23 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp24[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp24,_tmp24,_tmp24 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp25[7U]="_throw";
# 215 "toc.cyc"
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
# 283
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 286
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp6F=_cycalloc(sizeof(*_tmp6F));((_tmp6F->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp819=({struct _fat_ptr*_tmp6A[1U];({struct _fat_ptr*_tmp818=({struct _fat_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _fat_ptr _tmp817=({const char*_tmp6B="Core";_tag_fat(_tmp6B,sizeof(char),5U);});*_tmp6C=_tmp817;});_tmp6C;});_tmp6A[0]=_tmp818;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6A,sizeof(struct _fat_ptr*),1U));});((_tmp6F->f1).Abs_n).val=_tmp819;}),({struct _fat_ptr*_tmp816=({struct _fat_ptr*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct _fat_ptr _tmp815=({const char*_tmp6D="get_exn_thrown";_tag_fat(_tmp6D,sizeof(char),15U);});*_tmp6E=_tmp815;});_tmp6E;});_tmp6F->f2=_tmp816;});_tmp6F;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=0U,_tmp69->f1=qv;_tmp69;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->topt=0,({void*_tmp81A=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=1U,_tmp67->f1=bnd;_tmp67;});_tmp68->r=_tmp81A;}),_tmp68->loc=0U,_tmp68->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp68;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=10U,_tmp66->f1=fnname,_tmp66->f2=0,_tmp66->f3=0,_tmp66->f4=0;_tmp66;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->topt=0,_tmp65->r=fncall_re,_tmp65->loc=0U,_tmp65->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp65;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 296
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 298
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_tmp81D=Cyc_Absyn_void_type;void*_tmp81C=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp81B=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_star_type(_tmp81D,_tmp81C,_tmp81B,Cyc_Absyn_false_type);});
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
r=({void*_tmp81E=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp81E,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 317
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 321
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp72=_cycalloc(sizeof(*_tmp72));({struct Cyc_List_List*_tmp820=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));({void*_tmp81F=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,_tmp70->f1=name;_tmp70;});_tmp71->hd=_tmp81F;}),_tmp71->tl=0;_tmp71;});_tmp72->f1=_tmp820;}),_tmp72->f2=e;_tmp72;});}
# 325
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp821=f;Cyc_Absyn_fncall_exp(_tmp821,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
# 328
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp73=e->r;void*_stmttmp2=_tmp73;void*_tmp74=_stmttmp2;struct Cyc_Absyn_Exp*_tmp75;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp75=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp75;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 336
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=14U,_tmp76->f1=t,_tmp76->f2=e,_tmp76->f3=0,_tmp76->f4=Cyc_Absyn_No_coercion;_tmp76;});}
# 339
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=20U,_tmp77->f1=e;_tmp77;});}
# 342
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=23U,_tmp78->f1=e1,_tmp78->f2=e2;_tmp78;});}
# 345
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=37U,_tmp79->f1=s;_tmp79;});}
# 348
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=17U,_tmp7A->f1=t;_tmp7A;});}
# 351
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=10U,_tmp7B->f1=e,({struct Cyc_List_List*_tmp822=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp7B->f2=_tmp822;}),_tmp7B->f3=0,_tmp7B->f4=1;_tmp7B;});}
# 354
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=2U,_tmp7C->f1=s1,_tmp7C->f2=s2;_tmp7C;});}
# 357
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=6U,_tmp7D->f1=e1,_tmp7D->f2=e2,_tmp7D->f3=e3;_tmp7D;});}
# 360
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=21U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 363
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 366
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 368
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 372
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 376
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp81=e->r;void*_stmttmp3=_tmp81;void*_tmp82=_stmttmp3;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp83;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 404 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 410
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp84=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp84;void*_tmp85=_stmttmp4;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1: _LL2:
 return fS->fchar;case Cyc_Absyn_Short_sz: _LL3: _LL4:
 return fS->fshort;case Cyc_Absyn_Int_sz: _LL5: _LL6:
 return fS->fint;default: goto _LLF;}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL7: _LL8:
 return fS->ffloat;case 1U: _LL9: _LLA:
 return fS->fdouble;default: _LLB: _LLC:
 return fS->flongdouble;}default: goto _LLF;}case 3U: _LLD: _LLE:
 return fS->fvoidstar;default: _LLF: _LL10:
# 420
({struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp794;_tmp794.tag=0U,({struct _fat_ptr _tmp823=({const char*_tmp8B="expression type ";_tag_fat(_tmp8B,sizeof(char),17U);});_tmp794.f1=_tmp823;});_tmp794;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp88=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp793;_tmp793.tag=2U,_tmp793.f1=(void*)t;_tmp793;});struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp792;_tmp792.tag=0U,({struct _fat_ptr _tmp824=({const char*_tmp8A=" (not int, float, double, or pointer)";_tag_fat(_tmp8A,sizeof(char),38U);});_tmp792.f1=_tmp824;});_tmp792;});void*_tmp86[3U];_tmp86[0]=& _tmp87,_tmp86[1]=& _tmp88,_tmp86[2]=& _tmp89;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp86,sizeof(void*),3U));});}_LL0:;}
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp8C;void*_tmp8D=_stmttmp5;void*_tmp8E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 3U){_LL1: _tmp8E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_type;_LL2: {void*et=_tmp8E;
return Cyc_Toc_getFunctionType(fS,et);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp795;_tmp795.tag=0U,({struct _fat_ptr _tmp825=({const char*_tmp91="impossible type (not pointer)";_tag_fat(_tmp91,sizeof(char),30U);});_tmp795.f1=_tmp825;});_tmp795;});void*_tmp8F[1U];_tmp8F[0]=& _tmp90;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp8F,sizeof(void*),1U));});}_LL0:;}
# 433
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_stmttmp6=_tmp92;void*_tmp93=_stmttmp6;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Exp*_tmp99;long long _tmp9A;int _tmp9B;short _tmp9C;struct _fat_ptr _tmp9D;char _tmp9E;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp9E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2: {char c=_tmp9E;
return(int)c == (int)'\000';}case 3U: _LL3: _tmp9D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp9D;
# 437
unsigned long _tmp9F=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp9F;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 447
return 0;}}case 4U: _LL5: _tmp9C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6: {short i=_tmp9C;
return(int)i == 0;}case 5U: _LL7: _tmp9B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8: {int i=_tmp9B;
return i == 0;}case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA: {long long i=_tmp9A;
return i == (long long)0;}case 1U: _LLD: _LLE:
# 452
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 451
 goto _LLE;case 14U: _LLF: _tmp99=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp99;
# 453
return Cyc_Toc_is_zero(e1);}case 24U: _LL11: _tmp98=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12: {struct Cyc_List_List*es=_tmp98;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,es);}case 26U: _LL13: _tmp97=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14: {struct Cyc_List_List*dles=_tmp97;
_tmp96=dles;goto _LL16;}case 29U: _LL15: _tmp96=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16: {struct Cyc_List_List*dles=_tmp96;
_tmp95=dles;goto _LL18;}case 25U: _LL17: _tmp95=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18: {struct Cyc_List_List*dles=_tmp95;
_tmp94=dles;goto _LL1A;}case 36U: _LL19: _tmp94=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A: {struct Cyc_List_List*dles=_tmp94;
# 459
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 467
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;struct _fat_ptr*_tmpA2;union Cyc_Absyn_Nmspace _tmpA1;_LL1: _tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmpA1;struct _fat_ptr*v=_tmpA2;
union Cyc_Absyn_Nmspace _tmpA3=ns;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA6;switch((_tmpA3.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpA6=0;goto _LL7;case 1U: _LL6: _tmpA6=(_tmpA3.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmpA6;
_tmpA5=vs;goto _LL9;}case 2U: _LL8: _tmpA5=(_tmpA3.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmpA5;
_tmpA4=vs;goto _LLB;}default: _LLA: _tmpA4=(_tmpA3.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmpA4;
# 476
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U,_tmp797.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp797;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp796;_tmp796.tag=0U,_tmp796.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp796;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({struct _fat_ptr _tmp826=({const char*_tmpA8="%s_%s_struct";_tag_fat(_tmpA8,sizeof(char),13U);});Cyc_aprintf(_tmp826,_tag_fat(_tmpA7,sizeof(void*),2U));});});{
struct _RegionHandle _tmpAB=_new_region("r");struct _RegionHandle*r=& _tmpAB;_push_region(r);
{struct _fat_ptr _tmpB2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,_tmp79A.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp79A;});struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,({struct _fat_ptr _tmp829=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp828=r;struct Cyc_List_List*_tmp827=vs;Cyc_rstr_sepstr(_tmp828,_tmp827,({const char*_tmpB1="_";_tag_fat(_tmpB1,sizeof(char),2U);}));}));_tmp799.f1=_tmp829;});_tmp799;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp798;_tmp798.tag=0U,_tmp798.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp798;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAE,_tmpAC[1]=& _tmpAF,_tmpAC[2]=& _tmpB0;({struct _fat_ptr _tmp82A=({const char*_tmpAD="%s_%s_%s_struct";_tag_fat(_tmpAD,sizeof(char),16U);});Cyc_aprintf(_tmp82A,_tag_fat(_tmpAC,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB2;};_pop_region();}}}_LL3:;}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 489
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 492
struct _tuple20 _tmpB3=*env;struct _tuple20 _stmttmp7=_tmpB3;struct _tuple20 _tmpB4=_stmttmp7;struct _tuple14*_tmpB6;struct Cyc_Dict_Dict*_tmpB5;_LL1: _tmpB5=(_tmpB4.f1)->qvar_tags;_tmpB6=_tmpB4.f2;_LL2: {struct Cyc_Dict_Dict*qvs=_tmpB5;struct _tuple14*pair=_tmpB6;
struct _tuple14 _tmpB7=*pair;struct _tuple14 _stmttmp8=_tmpB7;struct _tuple14 _tmpB8=_stmttmp8;struct _tuple1*_tmpBA;struct _tuple1*_tmpB9;_LL4: _tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;_LL5: {struct _tuple1*fieldname=_tmpB9;struct _tuple1*dtname=_tmpBA;
struct _handler_cons _tmpBB;_push_handler(& _tmpBB);{int _tmpBD=0;if(setjmp(_tmpBB.handler))_tmpBD=1;if(!_tmpBD){{struct _tuple1*_tmpBE=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0U);return _tmpBE;};_pop_handler();}else{void*_tmpBC=(void*)Cyc_Core_get_exn_thrown();void*_tmpBF=_tmpBC;void*_tmpC0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBF)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 496
struct _tuple14*_tmpC1=({struct _tuple14*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=fieldname,_tmpC7->f2=dtname;_tmpC7;});struct _tuple14*new_pair=_tmpC1;
struct _tuple1*_tmpC2=fieldname;struct _fat_ptr*_tmpC4;union Cyc_Absyn_Nmspace _tmpC3;_LLC: _tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_LLD: {union Cyc_Absyn_Nmspace nmspace=_tmpC3;struct _fat_ptr*fieldvar=_tmpC4;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->f1=nmspace,({struct _fat_ptr*_tmp82B=({struct _fat_ptr*_tmpC5=_cycalloc(sizeof(*_tmpC5));*_tmpC5=newvar;_tmpC5;});_tmpC6->f2=_tmp82B;});_tmpC6;});
({struct Cyc_Dict_Dict _tmp82C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp82C;});
return res;}}}else{_LL9: _tmpC0=_tmpBF;_LLA: {void*exn=_tmpC0;(int)_rethrow(exn);}}_LL6:;}}}}}
# 504
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp79B;_tmp79B.f1=fieldname,_tmp79B.f2=dtname;_tmp79B;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 511
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 514
struct _tuple1*_tmp82F=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp82E=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp82E;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp82F;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp82D=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->fields=fs,_tmpC9->tagged=0;_tmpC9;});_tmpCA->impl=_tmp82D;});_tmpCA;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 520
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 523
struct _tuple21 _tmpCB=*env;struct _tuple21 _stmttmp9=_tmpCB;struct _tuple21 _tmpCC=_stmttmp9;struct Cyc_List_List*_tmpCE;struct Cyc_List_List**_tmpCD;_LL1: _tmpCD=(_tmpCC.f1)->tuple_types;_tmpCE=_tmpCC.f2;_LL2: {struct Cyc_List_List**tuple_types=_tmpCD;struct Cyc_List_List*tqs0=_tmpCE;
# 525
{struct Cyc_List_List*_tmpCF=*tuple_types;struct Cyc_List_List*tts=_tmpCF;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_tmpD0=(struct _tuple22*)tts->hd;struct _tuple22*_stmttmpA=_tmpD0;struct _tuple22*_tmpD1=_stmttmpA;struct Cyc_List_List*_tmpD3;void*_tmpD2;_LL4: _tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_LL5: {void*x=_tmpD2;struct Cyc_List_List*ts=_tmpD3;
struct Cyc_List_List*_tmpD4=tqs0;struct Cyc_List_List*tqs=_tmpD4;
for(0;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 536
struct _fat_ptr*xname=({struct _fat_ptr*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct _fat_ptr _tmp831=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE3=({struct Cyc_Int_pa_PrintArg_struct _tmp79C;_tmp79C.tag=1U,_tmp79C.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79C;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE3;({struct _fat_ptr _tmp830=({const char*_tmpE2="_tuple%d";_tag_fat(_tmpE2,sizeof(char),9U);});Cyc_aprintf(_tmp830,_tag_fat(_tmpE1,sizeof(void*),1U));});});*_tmpE4=_tmp831;});_tmpE4;});
struct Cyc_List_List*_tmpD5=0;struct Cyc_List_List*fs=_tmpD5;
struct Cyc_List_List*_tmpD6=0;struct Cyc_List_List*ts=_tmpD6;
{int i=1;for(0;tqs0 != 0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f2;
fs=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_Absyn_Aggrfield*_tmp833=({struct Cyc_Absyn_Aggrfield*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _fat_ptr*_tmp832=Cyc_Absyn_fieldname(i);_tmpD7->name=_tmp832;}),_tmpD7->tq=Cyc_Toc_mt_tq,_tmpD7->type=t,_tmpD7->width=0,_tmpD7->attributes=0,_tmpD7->requires_clause=0;_tmpD7;});_tmpD8->hd=_tmp833;}),_tmpD8->tl=fs;_tmpD8;});
ts=({struct Cyc_List_List*_tmpD9=_region_malloc(d,sizeof(*_tmpD9));_tmpD9->hd=t,_tmpD9->tl=ts;_tmpD9;});}}
# 544
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);
ts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts);{
struct Cyc_Absyn_Aggrdecl*_tmpDA=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpDA;
void*_tmpDB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE0=_cycalloc(sizeof(*_tmpE0));*_tmpE0=sd;_tmpE0;})),0);void*ans=_tmpDB;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_Absyn_Decl*_tmp834=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->tag=5U,_tmpDC->f1=sd;_tmpDC;}),0U);_tmpDD->hd=_tmp834;}),_tmpDD->tl=Cyc_Toc_result_decls;_tmpDD;});
({struct Cyc_List_List*_tmp836=({struct Cyc_List_List*_tmpDF=_region_malloc(d,sizeof(*_tmpDF));({struct _tuple22*_tmp835=({struct _tuple22*_tmpDE=_region_malloc(d,sizeof(*_tmpDE));_tmpDE->f1=ans,_tmpDE->f2=ts;_tmpDE;});_tmpDF->hd=_tmp835;}),_tmpDF->tl=*tuple_types;_tmpDF;});*tuple_types=_tmp836;});
return ans;}}}}
# 552
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 557
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 560
struct _tuple25*_tmpE5=env;struct Cyc_List_List*_tmpE8;enum Cyc_Absyn_AggrKind _tmpE7;struct Cyc_List_List**_tmpE6;_LL1: _tmpE6=(_tmpE5->f1)->anon_aggr_types;_tmpE7=(_tmpE5->f2)->f1;_tmpE8=(_tmpE5->f2)->f2;_LL2: {struct Cyc_List_List**anon_aggr_types=_tmpE6;enum Cyc_Absyn_AggrKind ak=_tmpE7;struct Cyc_List_List*fs=_tmpE8;
# 562
{struct Cyc_List_List*_tmpE9=*anon_aggr_types;struct Cyc_List_List*ts=_tmpE9;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpEA=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpEA;struct _tuple26*_tmpEB=_stmttmpB;struct Cyc_List_List*_tmpEE;enum Cyc_Absyn_AggrKind _tmpED;void*_tmpEC;_LL4: _tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;_tmpEE=_tmpEB->f3;_LL5: {void*x=_tmpEC;enum Cyc_Absyn_AggrKind ak2=_tmpED;struct Cyc_List_List*fs2=_tmpEE;
if((int)ak != (int)ak2)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 571
struct _fat_ptr*xname=({struct _fat_ptr*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct _fat_ptr _tmp838=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF8=({struct Cyc_Int_pa_PrintArg_struct _tmp79D;_tmp79D.tag=1U,_tmp79D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79D;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct _fat_ptr _tmp837=({const char*_tmpF7="_tuple%d";_tag_fat(_tmpF7,sizeof(char),9U);});Cyc_aprintf(_tmp837,_tag_fat(_tmpF6,sizeof(void*),1U));});});*_tmpF9=_tmp838;});_tmpF9;});
struct Cyc_Absyn_Aggrdecl*_tmpEF=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpEF;
sd->kind=ak;{
void*_tmpF0=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF5=_cycalloc(sizeof(*_tmpF5));*_tmpF5=sd;_tmpF5;})),0);void*ans=_tmpF0;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct Cyc_Absyn_Decl*_tmp839=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->tag=5U,_tmpF1->f1=sd;_tmpF1;}),0U);_tmpF2->hd=_tmp839;}),_tmpF2->tl=Cyc_Toc_result_decls;_tmpF2;});
({struct Cyc_List_List*_tmp83B=({struct Cyc_List_List*_tmpF4=_region_malloc(d,sizeof(*_tmpF4));({struct _tuple26*_tmp83A=({struct _tuple26*_tmpF3=_region_malloc(d,sizeof(*_tmpF3));_tmpF3->f1=ans,_tmpF3->f2=ak,_tmpF3->f3=fs;_tmpF3;});_tmpF4->hd=_tmp83A;}),_tmpF4->tl=*anon_aggr_types;_tmpF4;});*anon_aggr_types=_tmp83B;});
return ans;}}}}
# 579
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->f1=ak,_tmpFA->f2=fs;_tmpFA;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 588
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 597
struct _tuple28 _tmpFB=*env;struct _tuple28 _stmttmpC=_tmpFB;struct _tuple28 _tmpFC=_stmttmpC;struct Cyc_List_List*_tmp101;struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmpFF;struct _tuple1*_tmpFE;struct Cyc_List_List**_tmpFD;_LL1: _tmpFD=(_tmpFC.f1)->abs_struct_types;_tmpFE=(_tmpFC.f2)->f1;_tmpFF=(_tmpFC.f2)->f2;_tmp100=(_tmpFC.f2)->f3;_tmp101=(_tmpFC.f2)->f4;_LL2: {struct Cyc_List_List**abs_struct_types=_tmpFD;struct _tuple1*struct_name=_tmpFE;struct Cyc_List_List*type_vars=_tmpFF;struct Cyc_List_List*type_args=_tmp100;struct Cyc_List_List*fields=_tmp101;
# 601
{struct Cyc_List_List*_tmp102=*abs_struct_types;struct Cyc_List_List*tts=_tmp102;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmp103=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmp103;struct _tuple29*_tmp104=_stmttmpD;void*_tmp107;struct Cyc_List_List*_tmp106;struct _tuple1*_tmp105;_LL4: _tmp105=_tmp104->f1;_tmp106=_tmp104->f2;_tmp107=_tmp104->f3;_LL5: {struct _tuple1*x=_tmp105;struct Cyc_List_List*ts2=_tmp106;void*t=_tmp107;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp83C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args);_tmp83C == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmp108=type_args;struct Cyc_List_List*ts=_tmp108;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmp109=(void*)ts->hd;void*t=_tmp109;
void*_tmp10A=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmp10A;
{struct Cyc_Absyn_Kind*_tmp10B=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp10B;struct Cyc_Absyn_Kind*_tmp10C=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp10C)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 614
 if(Cyc_Unify_unify(t,t2)||({void*_tmp83D=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp83D,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 619
break;}}
# 621
if(okay)
return t;}}}}{
# 628
struct _fat_ptr*xname=({struct _fat_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct _fat_ptr _tmp83F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11E=({struct Cyc_Int_pa_PrintArg_struct _tmp79E;_tmp79E.tag=1U,_tmp79E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79E;});void*_tmp11C[1U];_tmp11C[0]=& _tmp11E;({struct _fat_ptr _tmp83E=({const char*_tmp11D="_tuple%d";_tag_fat(_tmp11D,sizeof(char),9U);});Cyc_aprintf(_tmp83E,_tag_fat(_tmp11C,sizeof(void*),1U));});});*_tmp11F=_tmp83F;});_tmp11F;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp10D=0;struct Cyc_List_List*fs=_tmp10D;
# 632
({struct Cyc_List_List*_tmp841=({struct Cyc_List_List*_tmp10F=_region_malloc(d,sizeof(*_tmp10F));({struct _tuple29*_tmp840=({struct _tuple29*_tmp10E=_region_malloc(d,sizeof(*_tmp10E));_tmp10E->f1=struct_name,_tmp10E->f2=type_args,_tmp10E->f3=x;_tmp10E;});_tmp10F->hd=_tmp840;}),_tmp10F->tl=*abs_struct_types;_tmp10F;});*abs_struct_types=_tmp841;});{
# 635
struct _RegionHandle _tmp110=_new_region("r");struct _RegionHandle*r=& _tmp110;_push_region(r);
{struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp111;
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp112=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp112;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 645
if((fields->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));({void*_tmp842=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=6U,_tmp113->f1=0;_tmp113;});_tmp114->hd=_tmp842;}),_tmp114->tl=atts;_tmp114;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 652
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=4U,({void*_tmp845=Cyc_Toc_void_star_type();(_tmp115->f1).elt_type=_tmp845;}),({struct Cyc_Absyn_Tqual _tmp844=Cyc_Absyn_empty_tqual(0U);(_tmp115->f1).tq=_tmp844;}),({
struct Cyc_Absyn_Exp*_tmp843=Cyc_Absyn_uint_exp(0U,0U);(_tmp115->f1).num_elts=_tmp843;}),(_tmp115->f1).zero_term=Cyc_Absyn_false_type,(_tmp115->f1).zt_loc=0U;_tmp115;});
# 656
fs=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Aggrfield*_tmp846=({struct Cyc_Absyn_Aggrfield*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->name=f->name,_tmp116->tq=Cyc_Toc_mt_tq,_tmp116->type=t,_tmp116->width=f->width,_tmp116->attributes=atts,_tmp116->requires_clause=0;_tmp116;});_tmp117->hd=_tmp846;}),_tmp117->tl=fs;_tmp117;});}
# 658
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp118=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp118;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct Cyc_Absyn_Decl*_tmp847=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=5U,_tmp119->f1=sd;_tmp119;}),0U);_tmp11A->hd=_tmp847;}),_tmp11A->tl=Cyc_Toc_result_decls;_tmp11A;});{
void*_tmp11B=x;_npop_handler(0U);return _tmp11B;}}}
# 636
;_pop_region();}}}}
# 664
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 668
struct _tuple27 env=({struct _tuple27 _tmp79F;_tmp79F.f1=struct_name,_tmp79F.f2=type_vars,_tmp79F.f3=type_args,_tmp79F.f4=fields;_tmp79F;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 676
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp84A=({struct _fat_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _fat_ptr _tmp849=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122=({struct Cyc_Int_pa_PrintArg_struct _tmp7A0;_tmp7A0.tag=1U,_tmp7A0.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp7A0;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _fat_ptr _tmp848=({const char*_tmp121="_tmp%X";_tag_fat(_tmp121,sizeof(char),7U);});Cyc_aprintf(_tmp848,_tag_fat(_tmp120,sizeof(void*),1U));});});*_tmp123=_tmp849;});_tmp123;});_tmp124->f2=_tmp84A;});_tmp124;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 679
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp7A1;_tmp7A1.f1=v,({struct Cyc_Absyn_Exp*_tmp84B=Cyc_Absyn_var_exp(v,0U);_tmp7A1.f2=_tmp84B;});_tmp7A1;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 686
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp125=*env;struct _tuple31 _stmttmpF=_tmp125;struct _tuple31 _tmp126=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp127;_LL1: _tmp127=(_tmp126.f1)->temp_labels;_LL2: {struct Cyc_Xarray_Xarray*temp_labels=_tmp127;
int _tmp128=Cyc_Toc_fresh_label_counter ++;int i=_tmp128;
if(({int _tmp84C=i;_tmp84C < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct _fat_ptr _tmp84E=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12E=({struct Cyc_Int_pa_PrintArg_struct _tmp7A3;_tmp7A3.tag=1U,_tmp7A3.f1=(unsigned)i;_tmp7A3;});void*_tmp12C[1U];_tmp12C[0]=& _tmp12E;({struct _fat_ptr _tmp84D=({const char*_tmp12D="_LL%X";_tag_fat(_tmp12D,sizeof(char),6U);});Cyc_aprintf(_tmp84D,_tag_fat(_tmp12C,sizeof(void*),1U));});});*_tmp12F=_tmp84E;});_tmp12F;});
if(({int _tmp84F=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp84F != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A2;_tmp7A2.tag=0U,({struct _fat_ptr _tmp850=({const char*_tmp12B="fresh_label: add_ind returned bad index...";_tag_fat(_tmp12B,sizeof(char),43U);});_tmp7A2.f1=_tmp850;});_tmp7A2;});void*_tmp129[1U];_tmp129[0]=& _tmp12A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp129,sizeof(void*),1U));});
return res;}}}
# 696
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 702
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp130;
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
return({struct _tuple9*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->f1=(*a).f1,_tmp131->f2=(*a).f2,({void*_tmp851=Cyc_Toc_typ_to_c((*a).f3);_tmp131->f3=_tmp851;});_tmp131;});}
# 737 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp132=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp132;void*_tmp133=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp134;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->tag == 4U){_LL1: _tmp134=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1;_LL2: {struct Cyc_Absyn_ArrayInfo ai=_tmp134;
return({void*_tmp852=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp852,ai.tq);});}}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 744
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 746
struct Cyc_Absyn_Aggrfield*_tmp135=({struct Cyc_Absyn_Aggrfield*_tmp136=_cycalloc(sizeof(*_tmp136));*_tmp136=*f;_tmp136;});struct Cyc_Absyn_Aggrfield*ans=_tmp135;
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
void*_tmp137=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp137;void*_tmp138=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp139;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp138)->tag == 2U){_LL1: _tmp139=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp138)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp139;
return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 764
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp13A=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp13A;struct Cyc_Absyn_Kind*_tmp13B=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp13B)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 770
static int Cyc_Toc_is_void_star(void*t){
void*_tmp13C=Cyc_Tcutil_compress(t);void*_stmttmp13=_tmp13C;void*_tmp13D=_stmttmp13;void*_tmp13E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->tag == 3U){_LL1: _tmp13E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).elt_type;_LL2: {void*t2=_tmp13E;
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
void*_tmp13F=t;void**_tmp141;struct Cyc_Absyn_Datatypedecl*_tmp140;struct Cyc_Absyn_Enumdecl*_tmp142;struct Cyc_Absyn_Aggrdecl*_tmp143;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp145;void*_tmp149;struct Cyc_Absyn_Typedefdecl*_tmp148;struct Cyc_List_List*_tmp147;struct _tuple1*_tmp146;struct Cyc_List_List*_tmp14B;enum Cyc_Absyn_AggrKind _tmp14A;struct Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_VarargInfo*_tmp151;int _tmp150;struct Cyc_List_List*_tmp14F;void*_tmp14E;struct Cyc_Absyn_Tqual _tmp14D;unsigned _tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Tqual _tmp154;void*_tmp153;void*_tmp159;struct Cyc_Absyn_Tqual _tmp158;void*_tmp157;struct Cyc_Absyn_Tvar*_tmp15A;void**_tmp15B;struct Cyc_List_List*_tmp15C;struct _tuple1*_tmp15D;struct Cyc_List_List*_tmp160;union Cyc_Absyn_AggrInfo _tmp15F;void*_tmp15E;struct Cyc_Absyn_Datatypefield*_tmp162;struct Cyc_Absyn_Datatypedecl*_tmp161;switch(*((int*)_tmp13F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 798
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp161=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1).KnownDatatypefield).val).f1;_tmp162=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1).KnownDatatypefield).val).f2;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmp161;struct Cyc_Absyn_Datatypefield*tuf=_tmp162;
# 800
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{_LLB: _LLC:
# 802
({struct Cyc_Warn_String_Warn_Warg_struct _tmp164=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A4;_tmp7A4.tag=0U,({struct _fat_ptr _tmp853=({const char*_tmp165="unresolved DatatypeFieldType";_tag_fat(_tmp165,sizeof(char),29U);});_tmp7A4.f1=_tmp853;});_tmp7A4;});void*_tmp163[1U];_tmp163[0]=& _tmp164;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp163,sizeof(void*),1U));});}case 1U: _LLF: _LL10:
# 811
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp15E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1;_tmp15F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1;_tmp160=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f2;_LL1C: {void*c=_tmp15E;union Cyc_Absyn_AggrInfo info=_tmp15F;struct Cyc_List_List*ts=_tmp160;
# 846
{union Cyc_Absyn_AggrInfo _tmp172=info;if((_tmp172.UnknownAggr).tag == 1){_LL36: _LL37:
# 848
 if(ts == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->tag=0U,_tmp173->f1=c,_tmp173->f2=0;_tmp173;});}}else{_LL38: _LL39:
 goto _LL35;}_LL35:;}{
# 852
struct Cyc_Absyn_Aggrdecl*_tmp174=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp174;
if(ad->expected_mem_kind){
# 855
if(ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp176=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7A6;_tmp7A6.tag=6U,_tmp7A6.f1=ad;_tmp7A6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp177=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A5;_tmp7A5.tag=0U,({struct _fat_ptr _tmp854=({const char*_tmp178=" was never defined.";_tag_fat(_tmp178,sizeof(char),20U);});_tmp7A5.f1=_tmp854;});_tmp7A5;});void*_tmp175[2U];_tmp175[0]=& _tmp176,_tmp175[1]=& _tmp177;Cyc_Warn_warn2(0U,_tag_fat(_tmp175,sizeof(void*),2U));});}
# 859
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp179=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp179;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp17A=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp17A;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7A8;_tmp7A8.tag=6U,_tmp7A8.f1=ad;_tmp7A8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A7;_tmp7A7.tag=0U,({struct _fat_ptr _tmp855=({const char*_tmp17E=" ended up being abstract.";_tag_fat(_tmp17E,sizeof(char),26U);});_tmp7A7.f1=_tmp855;});_tmp7A7;});void*_tmp17B[2U];_tmp17B[0]=& _tmp17C,_tmp17B[1]=& _tmp17D;Cyc_Warn_warn2(0U,_tag_fat(_tmp17B,sizeof(void*),2U));});{
# 871
struct _RegionHandle _tmp17F=_new_region("r");struct _RegionHandle*r=& _tmp17F;_push_region(r);
{struct Cyc_List_List*_tmp180=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp180;
void*_tmp181=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp181;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp182=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp182;}{
void*_tmp183=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0U);return _tmp183;}}
# 872
;_pop_region();}}
# 877
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15U: _LL1D: _tmp15D=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1;_LL1E: {struct _tuple1*tdn=_tmp15D;
return t;}case 16U: _LL1F: _tmp15C=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1;_LL20: {struct Cyc_List_List*fs=_tmp15C;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4U: _LL23: _LL24:
# 889
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 894
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp15B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13F)->f2;_LL4: {void**t2=_tmp15B;
# 789
if(*t2 == 0){
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 793
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));}case 2U: _LL5: _tmp15A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13F)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp15A;
# 795
if((int)(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;else{
return Cyc_Toc_void_star_type();}}case 3U: _LLD: _tmp157=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).elt_type;_tmp158=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).elt_tq;_tmp159=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).ptr_atts).bounds;_LLE: {void*t2=_tmp157;struct Cyc_Absyn_Tqual tq=_tmp158;void*bnds=_tmp159;
# 806
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 810
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}}case 4U: _LL13: _tmp153=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).elt_type;_tmp154=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).tq;_tmp155=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).num_elts;_tmp156=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).zt_loc;_LL14: {void*t2=_tmp153;struct Cyc_Absyn_Tqual tq=_tmp154;struct Cyc_Absyn_Exp*e=_tmp155;unsigned ztl=_tmp156;
# 814
return({void*_tmp859=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp858=tq;struct Cyc_Absyn_Exp*_tmp857=e;void*_tmp856=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp859,_tmp858,_tmp857,_tmp856,ztl);});}case 5U: _LL15: _tmp14D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).ret_tqual;_tmp14E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).ret_type;_tmp14F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).args;_tmp150=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).c_varargs;_tmp151=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).cyc_varargs;_tmp152=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).attributes;_LL16: {struct Cyc_Absyn_Tqual tq2=_tmp14D;void*t2=_tmp14E;struct Cyc_List_List*args=_tmp14F;int c_vararg=_tmp150;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp151;struct Cyc_List_List*atts=_tmp152;
# 818
struct Cyc_List_List*_tmp166=Cyc_Atts_atts2c(atts);struct Cyc_List_List*new_atts=_tmp166;
struct Cyc_List_List*_tmp167=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp167;
if(cyc_vararg != 0){
# 822
void*_tmp168=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));void*t=_tmp168;
struct _tuple9*_tmp169=({struct _tuple9*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->f1=cyc_vararg->name,_tmp16B->f2=cyc_vararg->tq,_tmp16B->f3=t;_tmp16B;});struct _tuple9*vararg=_tmp169;
new_args=({struct Cyc_List_List*_tmp85A=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp85A,({struct Cyc_List_List*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->hd=vararg,_tmp16A->tl=0;_tmp16A;}));});}
# 826
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->tag=5U,(_tmp16C->f1).tvars=0,(_tmp16C->f1).effect=0,(_tmp16C->f1).ret_tqual=tq2,({void*_tmp85B=Cyc_Toc_typ_to_c(t2);(_tmp16C->f1).ret_type=_tmp85B;}),(_tmp16C->f1).args=new_args,(_tmp16C->f1).c_varargs=c_vararg,(_tmp16C->f1).cyc_varargs=0,(_tmp16C->f1).rgn_po=0,(_tmp16C->f1).attributes=new_atts,(_tmp16C->f1).requires_clause=0,(_tmp16C->f1).requires_relns=0,(_tmp16C->f1).ensures_clause=0,(_tmp16C->f1).ensures_relns=0,(_tmp16C->f1).return_value=0;_tmp16C;});}case 6U: _LL17: _tmp14C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13F)->f1;_LL18: {struct Cyc_List_List*tqs=_tmp14C;
# 831
struct Cyc_List_List*_tmp16D=0;struct Cyc_List_List*tqs2=_tmp16D;
for(0;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp16F=_cycalloc(sizeof(*_tmp16F));({struct _tuple23*_tmp85D=({struct _tuple23*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=(*((struct _tuple23*)tqs->hd)).f1,({void*_tmp85C=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqs->hd)).f2);_tmp16E->f2=_tmp85C;});_tmp16E;});_tmp16F->hd=_tmp85D;}),_tmp16F->tl=tqs2;_tmp16F;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tqs2));}case 7U: _LL19: _tmp14A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13F)->f1;_tmp14B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13F)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp14A;struct Cyc_List_List*fs=_tmp14B;
# 840
struct Cyc_List_List*_tmp170=0;struct Cyc_List_List*fs2=_tmp170;
for(0;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp171=_cycalloc(sizeof(*_tmp171));({struct Cyc_Absyn_Aggrfield*_tmp85F=({struct Cyc_Absyn_Aggrfield*_tmp85E=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp85E,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp171->hd=_tmp85F;}),_tmp171->tl=fs2;_tmp171;});}
return({enum Cyc_Absyn_AggrKind _tmp860=k;Cyc_Toc_add_anon_aggr_type(_tmp860,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs2));});}case 8U: _LL21: _tmp146=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f1;_tmp147=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f2;_tmp148=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f3;_tmp149=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f4;_LL22: {struct _tuple1*tdn=_tmp146;struct Cyc_List_List*ts=_tmp147;struct Cyc_Absyn_Typedefdecl*td=_tmp148;void*topt=_tmp149;
# 883
if(topt == 0){
if(ts != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->tag=8U,_tmp184->f1=tdn,_tmp184->f2=0,_tmp184->f3=td,_tmp184->f4=0;_tmp184;});else{
return t;}}else{
# 888
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->tag=8U,_tmp185->f1=tdn,_tmp185->f2=0,_tmp185->f3=td,({void*_tmp861=Cyc_Toc_typ_to_c(topt);_tmp185->f4=_tmp861;});_tmp185;});}}case 9U: _LL2B: _tmp145=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp13F)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp145;
# 899
return t;}case 11U: _LL2D: _tmp144=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp13F)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp144;
# 904
Cyc_Toc_exptypes_to_c(e);
if(e->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)){case 0U: _LL2F: _tmp143=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)->f1;_LL30: {struct Cyc_Absyn_Aggrdecl*ad=_tmp143;
# 909
Cyc_Toc_aggrdecl_to_c(ad);
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp142=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)->f1;_LL32: {struct Cyc_Absyn_Enumdecl*ed=_tmp142;
# 914
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _LL33: _tmp140=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)->f1;_tmp141=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f2;_LL34: {struct Cyc_Absyn_Datatypedecl*dd=_tmp140;void**t=_tmp141;
# 917
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}}_LL0:;}
# 922
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp186=t;struct Cyc_Absyn_Tqual _tmp188;void*_tmp187;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp186)->tag == 4U){_LL1: _tmp187=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp186)->f1).elt_type;_tmp188=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp186)->f1).tq;_LL2: {void*t2=_tmp187;struct Cyc_Absyn_Tqual tq=_tmp188;
# 925
return({void*_tmp862=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp862,e);});}}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 932
static int Cyc_Toc_atomic_type(void*t){
void*_tmp189=Cyc_Tcutil_compress(t);void*_stmttmp14=_tmp189;void*_tmp18A=_stmttmp14;struct Cyc_List_List*_tmp18B;struct Cyc_List_List*_tmp18C;void*_tmp18D;struct Cyc_List_List*_tmp18F;void*_tmp18E;switch(*((int*)_tmp18A)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp18E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18A)->f1;_tmp18F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18A)->f2;_LL4: {void*c=_tmp18E;struct Cyc_List_List*ts=_tmp18F;
# 936
void*_tmp190=c;struct Cyc_Absyn_Datatypefield*_tmp192;struct Cyc_Absyn_Datatypedecl*_tmp191;union Cyc_Absyn_AggrInfo _tmp193;switch(*((int*)_tmp190)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp193=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp190)->f1;_LL25: {union Cyc_Absyn_AggrInfo info=_tmp193;
# 943
{union Cyc_Absyn_AggrInfo _tmp194=info;if((_tmp194.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 947
struct Cyc_Absyn_Aggrdecl*_tmp195=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp195;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp196=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp196;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp191=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownDatatypefield).val).f1;_tmp192=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownDatatypefield).val).f2;_LL27: {struct Cyc_Absyn_Datatypedecl*tud=_tmp191;struct Cyc_Absyn_Datatypefield*tuf=_tmp192;
# 954
{struct Cyc_List_List*_tmp197=tuf->typs;struct Cyc_List_List*tqs=_tmp197;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp199=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AA;_tmp7AA.tag=0U,({struct _fat_ptr _tmp863=({const char*_tmp19B="atomic_typ: bad type ";_tag_fat(_tmp19B,sizeof(char),22U);});_tmp7AA.f1=_tmp863;});_tmp7AA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7A9;_tmp7A9.tag=2U,_tmp7A9.f1=(void*)t;_tmp7A9;});void*_tmp198[2U];_tmp198[0]=& _tmp199,_tmp198[1]=& _tmp19A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp198,sizeof(void*),2U));});}_LL11:;}case 5U: _LL5: _LL6:
# 959
 return 1;case 4U: _LL7: _tmp18D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18A)->f1).elt_type;_LL8: {void*t=_tmp18D;
return Cyc_Toc_atomic_type(t);}case 7U: _LL9: _tmp18C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18A)->f2;_LLA: {struct Cyc_List_List*fs=_tmp18C;
# 967
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6U: _LLB: _tmp18B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18A)->f1;_LLC: {struct Cyc_List_List*tqs=_tmp18B;
# 971
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}
return 1;}case 3U: _LLD: _LLE:
# 976
 return 0;default: _LLF: _LL10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AC;_tmp7AC.tag=0U,({struct _fat_ptr _tmp864=({const char*_tmp19F="atomic_typ:  bad type ";_tag_fat(_tmp19F,sizeof(char),23U);});_tmp7AC.f1=_tmp864;});_tmp7AC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AB;_tmp7AB.tag=2U,_tmp7AB.f1=(void*)t;_tmp7AB;});void*_tmp19C[2U];_tmp19C[0]=& _tmp19D,_tmp19C[1]=& _tmp19E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp19C,sizeof(void*),2U));});}_LL0:;}
# 982
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp1A0=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp1A0;void*_tmp1A1=_stmttmp15;struct Cyc_List_List*_tmp1A2;union Cyc_Absyn_AggrInfo _tmp1A3;switch(*((int*)_tmp1A1)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->tag == 20U){_LL1: _tmp1A3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp1A3;
# 985
struct Cyc_Absyn_Aggrdecl*_tmp1A4=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A4;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AD;_tmp7AD.tag=0U,({struct _fat_ptr _tmp865=({const char*_tmp1A7="is_poly_field: type missing fields";_tag_fat(_tmp1A7,sizeof(char),35U);});_tmp7AD.f1=_tmp865;});_tmp7AD;});void*_tmp1A5[1U];_tmp1A5[0]=& _tmp1A6;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A5,sizeof(void*),1U));});
_tmp1A2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A1)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A2;
# 990
struct Cyc_Absyn_Aggrfield*_tmp1A8=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp1A8;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AF;_tmp7AF.tag=0U,({struct _fat_ptr _tmp866=({const char*_tmp1AC="is_poly_field: bad field ";_tag_fat(_tmp1AC,sizeof(char),26U);});_tmp7AF.f1=_tmp866;});_tmp7AF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AE;_tmp7AE.tag=0U,_tmp7AE.f1=*f;_tmp7AE;});void*_tmp1A9[2U];_tmp1A9[0]=& _tmp1AA,_tmp1A9[1]=& _tmp1AB;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A9,sizeof(void*),2U));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B1;_tmp7B1.tag=0U,({struct _fat_ptr _tmp867=({const char*_tmp1B0="is_poly_field: bad type ";_tag_fat(_tmp1B0,sizeof(char),25U);});_tmp7B1.f1=_tmp867;});_tmp7B1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B0;_tmp7B0.tag=2U,_tmp7B0.f1=(void*)t;_tmp7B0;});void*_tmp1AD[2U];_tmp1AD[0]=& _tmp1AE,_tmp1AD[1]=& _tmp1AF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AD,sizeof(void*),2U));});}_LL0:;}
# 1001
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B1=e->r;void*_stmttmp16=_tmp1B1;void*_tmp1B2=_stmttmp16;struct _fat_ptr*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B3;struct _fat_ptr*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B5;switch(*((int*)_tmp1B2)){case 21U: _LL1: _tmp1B5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B2)->f1;_tmp1B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B2)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp1B5;struct _fat_ptr*f=_tmp1B6;
# 1004
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22U: _LL3: _tmp1B3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B2)->f1;_tmp1B4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B2)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1B3;struct _fat_ptr*f=_tmp1B4;
# 1007
void*_tmp1B7=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp17=_tmp1B7;void*_tmp1B8=_stmttmp17;void*_tmp1B9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B8)->tag == 3U){_LL8: _tmp1B9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B8)->f1).elt_type;_LL9: {void*t=_tmp1B9;
# 1009
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B3;_tmp7B3.tag=0U,({struct _fat_ptr _tmp868=({const char*_tmp1BD="is_poly_project: bad type ";_tag_fat(_tmp1BD,sizeof(char),27U);});_tmp7B3.f1=_tmp868;});_tmp7B3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1BC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B2;_tmp7B2.tag=2U,_tmp7B2.f1=(void*)_check_null(e1->topt);_tmp7B2;});void*_tmp1BA[2U];_tmp1BA[0]=& _tmp1BB,_tmp1BA[1]=& _tmp1BC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1BA,sizeof(void*),2U));});}_LL7:;}default: _LL5: _LL6:
# 1012
 return 0;}_LL0:;}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1BE[1U];_tmp1BE[0]=s;({struct Cyc_Absyn_Exp*_tmp869=fn_e;Cyc_Toc_fncall_exp_dl(_tmp869,_tag_fat(_tmp1BE,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1022
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1BF[2U];_tmp1BF[0]=s,_tmp1BF[1]=n;({struct Cyc_Absyn_Exp*_tmp86A=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86A,_tag_fat(_tmp1BF,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C0[2U];_tmp1C0[0]=rgn,_tmp1C0[1]=s;({struct Cyc_Absyn_Exp*_tmp86B=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp86B,_tag_fat(_tmp1C0,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1031
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C1[2U];_tmp1C1[0]=rgn,_tmp1C1[1]=s;({struct Cyc_Absyn_Exp*_tmp86C=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp86C,_tag_fat(_tmp1C1,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1035
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C2[3U];_tmp1C2[0]=rgn,_tmp1C2[1]=s,_tmp1C2[2]=n;({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp86D,_tag_fat(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1039
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(({void*_tmp1C3=0U;({struct Cyc_Absyn_Exp*_tmp86E=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp86E,_tag_fat(_tmp1C3,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1044
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1052
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C4=e->r;void*_stmttmp18=_tmp1C4;void*_tmp1C5=_stmttmp18;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C5)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C5)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1057
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({struct Cyc_Absyn_Decl*_tmp86F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->tag=0U,_tmp1C6->f1=vd;_tmp1C6;}),0U);_tmp1C7->hd=_tmp86F;}),_tmp1C7->tl=Cyc_Toc_result_decls;_tmp1C7;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1065
 xexp=({void*_tmp870=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp870,e);});
# 1067
xplussz=({void*_tmp871=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp871,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1070
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1C8[3U];({struct _tuple19*_tmp874=({struct _tuple19*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->f1=0,_tmp1C9->f2=xexp;_tmp1C9;});_tmp1C8[0]=_tmp874;}),({
struct _tuple19*_tmp873=({struct _tuple19*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->f1=0,_tmp1CA->f2=xexp;_tmp1CA;});_tmp1C8[1]=_tmp873;}),({
struct _tuple19*_tmp872=({struct _tuple19*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->f1=0,_tmp1CB->f2=xplussz;_tmp1CB;});_tmp1C8[2]=_tmp872;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C8,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1105 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1CD=_region_malloc(r,sizeof(*_tmp1CD));_tmp1CD->break_lab=0,_tmp1CD->continue_lab=0,_tmp1CD->fallthru_info=0,_tmp1CD->toplevel=1,({int*_tmp875=({int*_tmp1CC=_region_malloc(r,sizeof(*_tmp1CC));*_tmp1CC=0;_tmp1CC;});_tmp1CD->in_lhs=_tmp875;}),_tmp1CD->rgn=r;_tmp1CD;});}
# 1109
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1CE=nv;int _tmp1CF;_LL1: _tmp1CF=_tmp1CE->toplevel;_LL2: {int t=_tmp1CF;
return t;}}
# 1113
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D0=e;int*_tmp1D5;int _tmp1D4;struct Cyc_Toc_FallthruInfo*_tmp1D3;struct _fat_ptr**_tmp1D2;struct _fat_ptr**_tmp1D1;_LL1: _tmp1D1=_tmp1D0->break_lab;_tmp1D2=_tmp1D0->continue_lab;_tmp1D3=_tmp1D0->fallthru_info;_tmp1D4=_tmp1D0->toplevel;_tmp1D5=_tmp1D0->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D1;struct _fat_ptr**c=_tmp1D2;struct Cyc_Toc_FallthruInfo*f=_tmp1D3;int t=_tmp1D4;int*lhs=_tmp1D5;
return({struct Cyc_Toc_Env*_tmp1D6=_region_malloc(r,sizeof(*_tmp1D6));_tmp1D6->break_lab=b,_tmp1D6->continue_lab=c,_tmp1D6->fallthru_info=f,_tmp1D6->toplevel=0,_tmp1D6->in_lhs=lhs,_tmp1D6->rgn=r;_tmp1D6;});}}
# 1117
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D7=e;int*_tmp1DC;int _tmp1DB;struct Cyc_Toc_FallthruInfo*_tmp1DA;struct _fat_ptr**_tmp1D9;struct _fat_ptr**_tmp1D8;_LL1: _tmp1D8=_tmp1D7->break_lab;_tmp1D9=_tmp1D7->continue_lab;_tmp1DA=_tmp1D7->fallthru_info;_tmp1DB=_tmp1D7->toplevel;_tmp1DC=_tmp1D7->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D8;struct _fat_ptr**c=_tmp1D9;struct Cyc_Toc_FallthruInfo*f=_tmp1DA;int t=_tmp1DB;int*lhs=_tmp1DC;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=b,_tmp1DD->continue_lab=c,_tmp1DD->fallthru_info=f,_tmp1DD->toplevel=1,_tmp1DD->in_lhs=lhs,_tmp1DD->rgn=r;_tmp1DD;});}}
# 1121
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1DE=nv;int*_tmp1DF;_LL1: _tmp1DF=_tmp1DE->in_lhs;_LL2: {int*b=_tmp1DF;
return*b;}}
# 1125
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E0=e;int*_tmp1E1;_LL1: _tmp1E1=_tmp1E0->in_lhs;_LL2: {int*lhs=_tmp1E1;
*lhs=b;}}
# 1130
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E2=e;int*_tmp1E7;int _tmp1E6;struct Cyc_Toc_FallthruInfo*_tmp1E5;struct _fat_ptr**_tmp1E4;struct _fat_ptr**_tmp1E3;_LL1: _tmp1E3=_tmp1E2->break_lab;_tmp1E4=_tmp1E2->continue_lab;_tmp1E5=_tmp1E2->fallthru_info;_tmp1E6=_tmp1E2->toplevel;_tmp1E7=_tmp1E2->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1E3;struct _fat_ptr**c=_tmp1E4;struct Cyc_Toc_FallthruInfo*f=_tmp1E5;int t=_tmp1E6;int*lhs=_tmp1E7;
return({struct Cyc_Toc_Env*_tmp1E8=_region_malloc(r,sizeof(*_tmp1E8));_tmp1E8->break_lab=b,_tmp1E8->continue_lab=c,_tmp1E8->fallthru_info=f,_tmp1E8->toplevel=t,_tmp1E8->in_lhs=lhs,_tmp1E8->rgn=r;_tmp1E8;});}}
# 1137
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E9=e;int*_tmp1EE;int _tmp1ED;struct Cyc_Toc_FallthruInfo*_tmp1EC;struct _fat_ptr**_tmp1EB;struct _fat_ptr**_tmp1EA;_LL1: _tmp1EA=_tmp1E9->break_lab;_tmp1EB=_tmp1E9->continue_lab;_tmp1EC=_tmp1E9->fallthru_info;_tmp1ED=_tmp1E9->toplevel;_tmp1EE=_tmp1E9->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1EA;struct _fat_ptr**c=_tmp1EB;struct Cyc_Toc_FallthruInfo*f=_tmp1EC;int t=_tmp1ED;int*lhs=_tmp1EE;
return({struct Cyc_Toc_Env*_tmp1EF=_region_malloc(r,sizeof(*_tmp1EF));_tmp1EF->break_lab=0,_tmp1EF->continue_lab=0,_tmp1EF->fallthru_info=f,_tmp1EF->toplevel=t,_tmp1EF->in_lhs=lhs,_tmp1EF->rgn=r;_tmp1EF;});}}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1147
struct Cyc_Toc_Env*_tmp1F0=e;int*_tmp1F5;int _tmp1F4;struct Cyc_Toc_FallthruInfo*_tmp1F3;struct _fat_ptr**_tmp1F2;struct _fat_ptr**_tmp1F1;_LL1: _tmp1F1=_tmp1F0->break_lab;_tmp1F2=_tmp1F0->continue_lab;_tmp1F3=_tmp1F0->fallthru_info;_tmp1F4=_tmp1F0->toplevel;_tmp1F5=_tmp1F0->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1F1;struct _fat_ptr**c=_tmp1F2;struct Cyc_Toc_FallthruInfo*f=_tmp1F3;int t=_tmp1F4;int*lhs=_tmp1F5;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));
_tmp1F8->label=fallthru_l,_tmp1F8->binders=fallthru_binders;_tmp1F8;});
return({struct Cyc_Toc_Env*_tmp1F7=_region_malloc(r,sizeof(*_tmp1F7));({struct _fat_ptr**_tmp876=({struct _fat_ptr**_tmp1F6=_region_malloc(r,sizeof(*_tmp1F6));*_tmp1F6=break_l;_tmp1F6;});_tmp1F7->break_lab=_tmp876;}),_tmp1F7->continue_lab=c,_tmp1F7->fallthru_info=fi,_tmp1F7->toplevel=t,_tmp1F7->in_lhs=lhs,_tmp1F7->rgn=r;_tmp1F7;});}}
# 1152
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1154
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FE;int _tmp1FD;struct Cyc_Toc_FallthruInfo*_tmp1FC;struct _fat_ptr**_tmp1FB;struct _fat_ptr**_tmp1FA;_LL1: _tmp1FA=_tmp1F9->break_lab;_tmp1FB=_tmp1F9->continue_lab;_tmp1FC=_tmp1F9->fallthru_info;_tmp1FD=_tmp1F9->toplevel;_tmp1FE=_tmp1F9->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1FA;struct _fat_ptr**c=_tmp1FB;struct Cyc_Toc_FallthruInfo*f=_tmp1FC;int t=_tmp1FD;int*lhs=_tmp1FE;
return({struct Cyc_Toc_Env*_tmp200=_region_malloc(r,sizeof(*_tmp200));({struct _fat_ptr**_tmp877=({struct _fat_ptr**_tmp1FF=_region_malloc(r,sizeof(*_tmp1FF));*_tmp1FF=break_l;_tmp1FF;});_tmp200->break_lab=_tmp877;}),_tmp200->continue_lab=c,_tmp200->fallthru_info=0,_tmp200->toplevel=t,_tmp200->in_lhs=lhs,_tmp200->rgn=r;_tmp200;});}}
# 1161
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1163
struct Cyc_Toc_Env*_tmp201=e;int*_tmp206;int _tmp205;struct Cyc_Toc_FallthruInfo*_tmp204;struct _fat_ptr**_tmp203;struct _fat_ptr**_tmp202;_LL1: _tmp202=_tmp201->break_lab;_tmp203=_tmp201->continue_lab;_tmp204=_tmp201->fallthru_info;_tmp205=_tmp201->toplevel;_tmp206=_tmp201->in_lhs;_LL2: {struct _fat_ptr**b=_tmp202;struct _fat_ptr**c=_tmp203;struct Cyc_Toc_FallthruInfo*f=_tmp204;int t=_tmp205;int*lhs=_tmp206;
return({struct Cyc_Toc_Env*_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208->break_lab=0,_tmp208->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp878=({struct Cyc_Toc_FallthruInfo*_tmp207=_region_malloc(r,sizeof(*_tmp207));_tmp207->label=next_l,_tmp207->binders=0;_tmp207;});_tmp208->fallthru_info=_tmp878;}),_tmp208->toplevel=t,_tmp208->in_lhs=lhs,_tmp208->rgn=r;_tmp208;});}}
# 1176 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1179
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp209=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp19=_tmp209;struct _tuple32*_tmp20A=_stmttmp19;struct Cyc_Absyn_Exp*_tmp20B;_LL1: _tmp20B=_tmp20A->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp20B;
Cyc_Toc_exptypes_to_c(e);
l=l->tl;}}}
# 1187
static void Cyc_Toc_asm_iolist_toc(struct Cyc_Toc_Env*nv,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp20C=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp1A=_tmp20C;struct _tuple32*_tmp20D=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp20E;_LL1: _tmp20E=_tmp20D->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp20E;
Cyc_Toc_exp_to_c(nv,e);
l=l->tl;}}}
# 1195
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp20F=e->annot;void*_stmttmp1B=_tmp20F;void*_tmp210=_stmttmp1B;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp210)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp210)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp210)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp210)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp210)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp210)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp212=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B4;_tmp7B4.tag=0U,({struct _fat_ptr _tmp879=({const char*_tmp213="Toc: do_null_check";_tag_fat(_tmp213,sizeof(char),19U);});_tmp7B4.f1=_tmp879;});_tmp7B4;});void*_tmp211[1U];_tmp211[0]=& _tmp212;({unsigned _tmp87A=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp87A,_tag_fat(_tmp211,sizeof(void*),1U));});});}}}}}}_LL0:;}
# 1211
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1213
int ans;
int _tmp214=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp214;
void*_tmp215=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp215;
void*_tmp216=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp216;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp217=old_typ;void*_tmp21B;void*_tmp21A;struct Cyc_Absyn_Tqual _tmp219;void*_tmp218;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->tag == 3U){_LL1: _tmp218=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).elt_type;_tmp219=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).elt_tq;_tmp21A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).ptr_atts).bounds;_tmp21B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp217)->f1).ptr_atts).zero_term;_LL2: {void*ta=_tmp218;struct Cyc_Absyn_Tqual tq=_tmp219;void*b=_tmp21A;void*zt=_tmp21B;
# 1224
{void*_tmp21C=annot;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21E;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp21C)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1226
 if(!((unsigned)({void*_tmp87B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87B,b);}))){
# 1229
void*_tmp21F=({void*_tmp87C=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp87C,tq);});void*newt=_tmp21F;
({void*_tmp87F=({void*_tmp87E=newt;Cyc_Toc_cast_it_r(_tmp87E,({struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp87D,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp87F;});
ptr->topt=newt;}
# 1233
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp21C)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1236
 if(!((unsigned)({void*_tmp880=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp880,b);}))){
# 1238
void*_tmp220=({void*_tmp881=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp881,tq);});void*newt=_tmp220;
({void*_tmp884=({void*_tmp883=newt;Cyc_Toc_cast_it_r(_tmp883,({struct Cyc_Absyn_Exp*_tmp882=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp882,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp884;});
ptr->topt=newt;
# 1245
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp222=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B5;_tmp7B5.tag=0U,({struct _fat_ptr _tmp885=({const char*_tmp223="subscript of ? with no bounds check but need null check";_tag_fat(_tmp223,sizeof(char),56U);});_tmp7B5.f1=_tmp885;});_tmp7B5;});void*_tmp221[1U];_tmp221[0]=& _tmp222;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp221,sizeof(void*),1U));});}
# 1249
({void*_tmp889=({void*_tmp888=new_typ;Cyc_Toc_cast_it_r(_tmp888,({struct Cyc_Absyn_Exp*_tmp224[1U];({struct Cyc_Absyn_Exp*_tmp886=Cyc_Absyn_new_exp(ptr->r,0U);_tmp224[0]=_tmp886;});({struct Cyc_Absyn_Exp*_tmp887=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp887,_tag_fat(_tmp224,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp889;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp21C)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp21C)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1255
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp88E=({void*_tmp88D=newt;Cyc_Toc_cast_it_r(_tmp88D,({struct Cyc_Absyn_Exp*_tmp225[3U];({
struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_new_exp(ptr->r,0U);_tmp225[0]=_tmp88B;}),({
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp225[1]=_tmp88A;}),_tmp225[2]=ind;({struct Cyc_Absyn_Exp*_tmp88C=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp88C,_tag_fat(_tmp225,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1258
ptr->r=_tmp88E;});
# 1262
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp21C)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp21E=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp21C)->f1;_LLF: {struct Cyc_Absyn_Exp*bd=_tmp21E;
# 1266
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp21D=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp21C)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp21D=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp21C)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp21D;
# 1269
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1274
struct _tuple13 _tmp226=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _stmttmp1C=_tmp226;struct _tuple13 _tmp227=_stmttmp1C;int _tmp229;unsigned _tmp228;_LL15: _tmp228=_tmp227.f1;_tmp229=_tmp227.f2;_LL16: {unsigned i=_tmp228;int valid=_tmp229;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1277
({void*_tmp893=({void*_tmp892=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp892,({struct Cyc_Absyn_Exp*_tmp22A[4U];({
struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22A[0]=_tmp890;}),_tmp22A[1]=bd,({
struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp22A[2]=_tmp88F;}),_tmp22A[3]=ind;({struct Cyc_Absyn_Exp*_tmp891=fn_e;Cyc_Toc_fncall_exp_dl(_tmp891,_tag_fat(_tmp22A,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1277
ptr->r=_tmp893;});
# 1280
ans=1;}else{
# 1283
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1286
({void*_tmp897=({void*_tmp896=new_typ;Cyc_Toc_cast_it_r(_tmp896,({struct Cyc_Absyn_Exp*_tmp22B[1U];({struct Cyc_Absyn_Exp*_tmp894=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp22B[0]=_tmp894;});({struct Cyc_Absyn_Exp*_tmp895=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp895,_tag_fat(_tmp22B,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1286
ptr->r=_tmp897;});
# 1288
ans=0;}else{
# 1292
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp89B=({void*_tmp89A=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp89A,({struct Cyc_Absyn_Exp*_tmp22C[3U];({
struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22C[0]=_tmp898;}),_tmp22C[1]=bd,_tmp22C[2]=ind;({struct Cyc_Absyn_Exp*_tmp899=fn_e;Cyc_Toc_fncall_exp_dl(_tmp899,_tag_fat(_tmp22C,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1293
ptr->r=_tmp89B;});
# 1295
ans=1;}}
# 1298
goto _LL5;}}}else{_LL12: _LL13:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B6;_tmp7B6.tag=0U,({struct _fat_ptr _tmp89C=({const char*_tmp22F="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp22F,sizeof(char),34U);});_tmp7B6.f1=_tmp89C;});_tmp7B6;});void*_tmp22D[1U];_tmp22D[0]=& _tmp22E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp22D,sizeof(void*),1U));});}}}}}}_LL5:;}
# 1301
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp231=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B7;_tmp7B7.tag=0U,({struct _fat_ptr _tmp89D=({const char*_tmp232="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp232,sizeof(char),31U);});_tmp7B7.f1=_tmp89D;});_tmp7B7;});void*_tmp230[1U];_tmp230[0]=& _tmp231;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp230,sizeof(void*),1U));});}_LL0:;}}
# 1307
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp234=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B8;_tmp7B8.tag=0U,({struct _fat_ptr _tmp89E=({const char*_tmp235="Missing type in primop ";_tag_fat(_tmp235,sizeof(char),24U);});_tmp7B8.f1=_tmp89E;});_tmp7B8;});void*_tmp233[1U];_tmp233[0]=& _tmp234;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp233,sizeof(void*),1U));});
return(void*)_check_null(e->topt);}
# 1311
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple23*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->f1=Cyc_Toc_mt_tq,({void*_tmp89F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp236->f2=_tmp89F;});_tmp236;});}
# 1316
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp237=e->r;void*_stmttmp1D=_tmp237;void*_tmp238=_stmttmp1D;int _tmp23A;struct Cyc_Absyn_Exp*_tmp239;int _tmp23C;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_List_List*_tmp23D;switch(*((int*)_tmp238)){case 26U: _LL1: _tmp23D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_LL2: {struct Cyc_List_List*dles=_tmp23D;
# 1319
{struct Cyc_List_List*_tmp23E=dles;struct Cyc_List_List*dles2=_tmp23E;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp23F="array designators for abstract-field initialization";_tag_fat(_tmp23F,sizeof(char),52U);}));}}
_tmp23B=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles),0U);_tmp23C=0;goto _LL4;}case 27U: _LL3: _tmp23B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp238)->f2;_tmp23C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp238)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp23B;int zt=_tmp23C;
_tmp239=bd;_tmp23A=zt;goto _LL6;}case 28U: _LL5: _tmp239=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp238)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp239;int zt=_tmp23A;
# 1325
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp8A0=bd;Cyc_Absyn_add_exp(_tmp8A0,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1334
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1342
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp240=e->r;void*_stmttmp1E=_tmp240;void*_tmp241=_stmttmp1E;struct Cyc_List_List*_tmp242;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp241)->tag == 29U){_LL1: _tmp242=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp241)->f3;_LL2: {struct Cyc_List_List*dles2=_tmp242;
dles=dles2;goto _LL0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1348
{void*_tmp243=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp1F=_tmp243;void*_tmp244=_stmttmp1F;struct Cyc_Absyn_Aggrdecl*_tmp245;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp245=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp245;
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
struct Cyc_Absyn_Aggrfield*_tmp246=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp246;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp247=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp20=_tmp247;struct _tuple19*_tmp248=_stmttmp20;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_List_List*_tmp249;_LLB: _tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;_LLC: {struct Cyc_List_List*ds=_tmp249;struct Cyc_Absyn_Exp*e2=_tmp24A;
struct _fat_ptr*_tmp24B=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp24B;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp24C=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp24C;
if(nested_ans != 0)
return nested_ans;{
void*_tmp24D=Cyc_Tcutil_compress(last_type_field->type);void*_stmttmp21=_tmp24D;void*_tmp24E=_stmttmp21;struct Cyc_Absyn_Exp*_tmp250;void*_tmp24F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E)->tag == 4U){_LLE: _tmp24F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E)->f1).elt_type;_tmp250=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E)->f1).num_elts;_LLF: {void*elt_type=_tmp24F;struct Cyc_Absyn_Exp*type_bd=_tmp250;
# 1375
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1381
return({struct Cyc_Absyn_Exp*_tmp8A3=({struct Cyc_Absyn_Exp*_tmp251[2U];_tmp251[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1383
struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp251[1]=_tmp8A1;});({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8A2,_tag_fat(_tmp251,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1381
Cyc_Absyn_add_exp(_tmp8A3,
# 1384
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{_LL10: _LL11:
 return 0;}_LLD:;}}}}
# 1389
({struct Cyc_Warn_String_Warn_Warg_struct _tmp253=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B9;_tmp7B9.tag=0U,({struct _fat_ptr _tmp8A4=({const char*_tmp254="get_varsizeexp: did not find last struct field";_tag_fat(_tmp254,sizeof(char),47U);});_tmp7B9.f1=_tmp8A4;});_tmp7B9;});void*_tmp252[1U];_tmp252[0]=& _tmp253;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp252,sizeof(void*),1U));});}}
# 1392
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp255=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp255;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp256=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp256;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1399
({struct Cyc_Warn_String_Warn_Warg_struct _tmp258=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BC;_tmp7BC.tag=0U,({struct _fat_ptr _tmp8A5=({const char*_tmp25C="get_member_offset ";_tag_fat(_tmp25C,sizeof(char),19U);});_tmp7BC.f1=_tmp8A5;});_tmp7BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp259=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BB;_tmp7BB.tag=0U,_tmp7BB.f1=*f;_tmp7BB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BA;_tmp7BA.tag=0U,({struct _fat_ptr _tmp8A6=({const char*_tmp25B=" failed";_tag_fat(_tmp25B,sizeof(char),8U);});_tmp7BA.f1=_tmp8A6;});_tmp7BA;});void*_tmp257[3U];_tmp257[0]=& _tmp258,_tmp257[1]=& _tmp259,_tmp257[2]=& _tmp25A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp257,sizeof(void*),3U));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1403
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1406
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1409
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->tag=5U,_tmp25D->f1=e1,_tmp25D->f2=incr;_tmp25D;}),0U);}
# 1413
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1422
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1424
void*_tmp25E=e1->r;void*_stmttmp22=_tmp25E;void*_tmp25F=_stmttmp22;int _tmp263;int _tmp262;struct _fat_ptr*_tmp261;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp265;void*_tmp264;struct Cyc_Absyn_Stmt*_tmp266;switch(*((int*)_tmp25F)){case 37U: _LL1: _tmp266=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp25F)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp266;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14U: _LL3: _tmp264=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25F)->f1;_tmp265=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25F)->f2;_LL4: {void*t=_tmp264;struct Cyc_Absyn_Exp*e=_tmp265;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21U: _LL5: _tmp260=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25F)->f1;_tmp261=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25F)->f2;_tmp262=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25F)->f3;_tmp263=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25F)->f4;_LL6: {struct Cyc_Absyn_Exp*e=_tmp260;struct _fat_ptr*fld=_tmp261;int is_tagged=_tmp262;int is_read=_tmp263;
# 1429
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp8A9=e1;struct Cyc_List_List*_tmp8A8=({struct Cyc_List_List*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->hd=fld,_tmp267->tl=fs;_tmp267;});struct Cyc_Absyn_Exp*(*_tmp8A7)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp8A9,_tmp8A8,_tmp8A7,f_env);});
goto _LL0;}default: _LL7: _LL8: {
# 1437
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1439
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp8AA=(f(e1_copy,f_env))->r;e1->r=_tmp8AA;});
goto _LL0;}}_LL0:;}
# 1445
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1447
void*_tmp268=s->r;void*_stmttmp23=_tmp268;void*_tmp269=_stmttmp23;struct Cyc_Absyn_Stmt*_tmp26A;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Absyn_Decl*_tmp26B;struct Cyc_Absyn_Exp*_tmp26D;switch(*((int*)_tmp269)){case 1U: _LL1: _tmp26D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp269)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp26D;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12U: _LL3: _tmp26B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp269)->f1;_tmp26C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp269)->f2;_LL4: {struct Cyc_Absyn_Decl*d=_tmp26B;struct Cyc_Absyn_Stmt*s2=_tmp26C;
# 1450
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2U: _LL5: _tmp26A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp269)->f2;_LL6: {struct Cyc_Absyn_Stmt*s2=_tmp26A;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BE;_tmp7BE.tag=0U,({struct _fat_ptr _tmp8AB=({const char*_tmp271="lvalue_assign_stmt: ";_tag_fat(_tmp271,sizeof(char),21U);});_tmp7BE.f1=_tmp8AB;});_tmp7BE;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp270=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7BD;_tmp7BD.tag=5U,_tmp7BD.f1=s;_tmp7BD;});void*_tmp26E[2U];_tmp26E[0]=& _tmp26F,_tmp26E[1]=& _tmp270;Cyc_Toc_toc_impos2(_tag_fat(_tmp26E,sizeof(void*),2U));});}_LL0:;}
# 1456
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1460
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp272=e->r;void*_stmttmp24=_tmp272;void*_tmp273=_stmttmp24;struct Cyc_Absyn_Stmt*_tmp274;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp**_tmp277;void**_tmp276;switch(*((int*)_tmp273)){case 14U: _LL1: _tmp276=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp273)->f1;_tmp277=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp273)->f2;_LL2: {void**t=_tmp276;struct Cyc_Absyn_Exp**e1=_tmp277;
# 1463
({struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Toc_push_address_exp(*e1);*e1=_tmp8AC;});
({void*_tmp8AD=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp8AD;});
return e;}case 20U: _LL3: _tmp275=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp273)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp275;
# 1467
return e1;}case 37U: _LL5: _tmp274=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp273)->f1;_LL6: {struct Cyc_Absyn_Stmt*s=_tmp274;
# 1471
Cyc_Toc_push_address_stmt(s);
return e;}default: _LL7: _LL8:
# 1474
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp279=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C0;_tmp7C0.tag=0U,({struct _fat_ptr _tmp8AE=({const char*_tmp27B="can't take & of exp ";_tag_fat(_tmp27B,sizeof(char),21U);});_tmp7C0.f1=_tmp8AE;});_tmp7C0;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7BF;_tmp7BF.tag=4U,_tmp7BF.f1=e;_tmp7BF;});void*_tmp278[2U];_tmp278[0]=& _tmp279,_tmp278[1]=& _tmp27A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp278,sizeof(void*),2U));});}_LL0:;}
# 1478
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp27C=s->r;void*_stmttmp25=_tmp27C;void*_tmp27D=_stmttmp25;struct Cyc_Absyn_Exp**_tmp27E;struct Cyc_Absyn_Stmt*_tmp27F;struct Cyc_Absyn_Stmt*_tmp280;switch(*((int*)_tmp27D)){case 2U: _LL1: _tmp280=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp27D)->f2;_LL2: {struct Cyc_Absyn_Stmt*s2=_tmp280;
_tmp27F=s2;goto _LL4;}case 12U: _LL3: _tmp27F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp27D)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp27F;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1U: _LL5: _tmp27E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp27D)->f1;_LL6: {struct Cyc_Absyn_Exp**e=_tmp27E;
({struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Toc_push_address_exp(*e);*e=_tmp8AF;});goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp282=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C2;_tmp7C2.tag=0U,({struct _fat_ptr _tmp8B0=({const char*_tmp284="can't take & of stmt ";_tag_fat(_tmp284,sizeof(char),22U);});_tmp7C2.f1=_tmp8B0;});_tmp7C2;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp283=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7C1;_tmp7C1.tag=5U,_tmp7C1.f1=s;_tmp7C1;});void*_tmp281[2U];_tmp281[0]=& _tmp282,_tmp281[1]=& _tmp283;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp281,sizeof(void*),2U));});}_LL0:;}
# 1490
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1502
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp285=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp285;
void*_tmp286=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp286;
void*_tmp287=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp287;
struct Cyc_Core_Opt*_tmp288=({struct Cyc_Core_Opt*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->v=c_ptr_type;_tmp2BB;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp288;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp289=e1->r;void*_stmttmp26=_tmp289;void*_tmp28A=_stmttmp26;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Exp*_tmp28D;switch(*((int*)_tmp28A)){case 20U: _LL1: _tmp28D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp28A)->f1;_LL2: {struct Cyc_Absyn_Exp*ea=_tmp28D;
# 1510
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1515
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23U: _LL3: _tmp28B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28A)->f1;_tmp28C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28A)->f2;_LL4: {struct Cyc_Absyn_Exp*ea=_tmp28B;struct Cyc_Absyn_Exp*eb=_tmp28C;
# 1517
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1522
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp28E[3U];_tmp28E[0]=ea,({
struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp28E[1]=_tmp8B1;}),_tmp28E[2]=eb;({struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp8B2,_tag_fat(_tmp28E,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp290=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C3;_tmp7C3.tag=0U,({struct _fat_ptr _tmp8B3=({const char*_tmp291="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp291,sizeof(char),53U);});_tmp7C3.f1=_tmp8B3;});_tmp7C3;});void*_tmp28F[1U];_tmp28F[0]=& _tmp290;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp28F,sizeof(void*),1U));});}_LL0:;}{
# 1528
struct _tuple1*_tmp292=Cyc_Toc_temp_var();struct _tuple1*x=_tmp292;
struct _RegionHandle _tmp293=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp293;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp294=({struct Cyc_Absyn_Vardecl*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->sc=Cyc_Absyn_Public,_tmp2BA->name=x,_tmp2BA->varloc=0U,_tmp2BA->tq=Cyc_Toc_mt_tq,_tmp2BA->type=c_fat_ptr_type,_tmp2BA->initializer=xinit,_tmp2BA->rgn=0,_tmp2BA->attributes=0,_tmp2BA->escapes=0,_tmp2BA->is_proto=0;_tmp2BA;});struct Cyc_Absyn_Vardecl*x_vd=_tmp294;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp295=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->tag=4U,_tmp2B9->f1=x_vd;_tmp2B9;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp295;
struct Cyc_Absyn_Exp*_tmp296=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp296;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp297=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp297;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp298=Cyc_Toc_temp_var();struct _tuple1*y=_tmp298;
struct Cyc_Absyn_Vardecl*_tmp299=({struct Cyc_Absyn_Vardecl*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->sc=Cyc_Absyn_Public,_tmp2B8->name=y,_tmp2B8->varloc=0U,_tmp2B8->tq=Cyc_Toc_mt_tq,_tmp2B8->type=c_elt_type,_tmp2B8->initializer=deref_x,_tmp2B8->rgn=0,_tmp2B8->attributes=0,_tmp2B8->escapes=0,_tmp2B8->is_proto=0;_tmp2B8;});struct Cyc_Absyn_Vardecl*y_vd=_tmp299;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->tag=4U,_tmp2B7->f1=y_vd;_tmp2B7;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp29A;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp29B=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp29B;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp8B5=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp8B4=y_exp;Cyc_Absyn_prim2_exp(_tmp8B5,_tmp8B4,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1549
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp29C=Cyc_Toc_temp_var();struct _tuple1*z=_tmp29C;
struct Cyc_Absyn_Vardecl*_tmp29D=({struct Cyc_Absyn_Vardecl*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->sc=Cyc_Absyn_Public,_tmp2B6->name=z,_tmp2B6->varloc=0U,_tmp2B6->tq=Cyc_Toc_mt_tq,_tmp2B6->type=c_elt_type,_tmp2B6->initializer=z_init,_tmp2B6->rgn=0,_tmp2B6->attributes=0,_tmp2B6->escapes=0,_tmp2B6->is_proto=0;_tmp2B6;});struct Cyc_Absyn_Vardecl*z_vd=_tmp29D;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29E=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->tag=4U,_tmp2B5->f1=z_vd;_tmp2B5;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp29E;
# 1555
struct Cyc_Absyn_Exp*_tmp29F=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp29F;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A0=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp2A0;
struct Cyc_Absyn_Exp*_tmp2A1=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp2A1;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1562
struct Cyc_Absyn_Exp*_tmp2A2=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp2A2;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A3=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp2A3;
struct Cyc_Absyn_Exp*_tmp2A4=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp2A4;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1569
struct Cyc_List_List*_tmp2A5=({struct Cyc_Absyn_Exp*_tmp2B4[2U];({struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp2B4[0]=_tmp8B7;}),({
struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B4[1]=_tmp8B6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2B4,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 1569
struct Cyc_List_List*xsizeargs=_tmp2A5;
# 1571
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp2A6;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp8B8,oneexp,0U);});{
# 1576
struct Cyc_Absyn_Exp*_tmp2A7=({struct Cyc_Absyn_Exp*_tmp8B9=xsize;Cyc_Absyn_and_exp(_tmp8B9,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp2A7;
struct Cyc_Absyn_Stmt*_tmp2A8=Cyc_Absyn_exp_stmt(({void*_tmp2B3=0U;({struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp8BA,_tag_fat(_tmp2B3,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp2A8;
struct Cyc_Absyn_Exp*_tmp2A9=({struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp8BB,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp2A9;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp2AA=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp2AA;
struct Cyc_Absyn_Exp*_tmp2AB=({struct Cyc_Absyn_Exp*_tmp8BC=deref_xcurr;Cyc_Absyn_assign_exp(_tmp8BC,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp2AB;
struct Cyc_Absyn_Stmt*_tmp2AC=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp2AC;
s=({struct Cyc_Absyn_Stmt*_tmp8BF=({struct Cyc_Absyn_Exp*_tmp8BE=comp_exp;struct Cyc_Absyn_Stmt*_tmp8BD=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp8BE,_tmp8BD,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp8BF,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C1=({struct Cyc_Absyn_Decl*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));({void*_tmp8C0=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->tag=0U,_tmp2AD->f1=z_vd;_tmp2AD;});_tmp2AE->r=_tmp8C0;}),_tmp2AE->loc=0U;_tmp2AE;});Cyc_Absyn_decl_stmt(_tmp8C1,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C3=({struct Cyc_Absyn_Decl*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));({void*_tmp8C2=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF->tag=0U,_tmp2AF->f1=y_vd;_tmp2AF;});_tmp2B0->r=_tmp8C2;}),_tmp2B0->loc=0U;_tmp2B0;});Cyc_Absyn_decl_stmt(_tmp8C3,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C5=({struct Cyc_Absyn_Decl*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));({void*_tmp8C4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=0U,_tmp2B1->f1=x_vd;_tmp2B1;});_tmp2B2->r=_tmp8C4;}),_tmp2B2->loc=0U;_tmp2B2;});Cyc_Absyn_decl_stmt(_tmp8C5,s,0U);});
({void*_tmp8C6=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8C6;});}}}}}}}}}}
# 1530
;_pop_region();}}
# 1602 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1606
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2BC=Cyc_Tcutil_compress(aggrtype);void*_stmttmp27=_tmp2BC;void*_tmp2BD=_stmttmp27;union Cyc_Absyn_AggrInfo _tmp2BE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BD)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BD)->f1)->tag == 20U){_LL1: _tmp2BE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2BE;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C6;_tmp7C6.tag=0U,({struct _fat_ptr _tmp8C7=({const char*_tmp2C4="expecting union but found ";_tag_fat(_tmp2C4,sizeof(char),27U);});_tmp7C6.f1=_tmp8C7;});_tmp7C6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C5;_tmp7C5.tag=2U,_tmp7C5.f1=(void*)aggrtype;_tmp7C5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C4;_tmp7C4.tag=0U,({
struct _fat_ptr _tmp8C8=({const char*_tmp2C3=" in check_tagged_union";_tag_fat(_tmp2C3,sizeof(char),23U);});_tmp7C4.f1=_tmp8C8;});_tmp7C4;});void*_tmp2BF[3U];_tmp2BF[0]=& _tmp2C0,_tmp2BF[1]=& _tmp2C1,_tmp2BF[2]=& _tmp2C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2BF,sizeof(void*),3U));});}_LL0:;}{
# 1612
struct _tuple30 _tmp2C5=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp28=_tmp2C5;struct _tuple30 _tmp2C6=_stmttmp28;struct Cyc_Absyn_Exp*_tmp2C8;struct _tuple1*_tmp2C7;_LL6: _tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;_LL7: {struct _tuple1*temp=_tmp2C7;struct Cyc_Absyn_Exp*temp_exp=_tmp2C8;
struct Cyc_Absyn_Exp*_tmp2C9=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2C9;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2CA=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2CA;
struct Cyc_Absyn_Exp*_tmp2CB=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2CB;
struct Cyc_Absyn_Exp*_tmp2CC=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2CC;
struct Cyc_Absyn_Stmt*_tmp2CD=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2CD;
struct Cyc_Absyn_Stmt*_tmp2CE=({struct Cyc_Absyn_Exp*_tmp8CA=test_exp;struct Cyc_Absyn_Stmt*_tmp8C9=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8CA,_tmp8C9,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2CE;
void*_tmp2CF=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2CF;
struct Cyc_Absyn_Exp*_tmp2D0=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2D0;
struct Cyc_Absyn_Stmt*_tmp2D1=({struct _tuple1*_tmp8CD=temp;void*_tmp8CC=e1_p_type;struct Cyc_Absyn_Exp*_tmp8CB=e1_f;Cyc_Absyn_declare_stmt(_tmp8CD,_tmp8CC,_tmp8CB,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D1;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1625
struct Cyc_Absyn_Exp*_tmp2D2=({struct Cyc_Absyn_Exp*_tmp8CE=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8CE,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2D2;
struct Cyc_Absyn_Exp*_tmp2D3=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2D3;
struct Cyc_Absyn_Exp*_tmp2D4=({struct Cyc_Absyn_Exp*_tmp8CF=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8CF,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2D4;
struct Cyc_Absyn_Stmt*_tmp2D5=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2D5;
struct Cyc_Absyn_Stmt*_tmp2D6=({struct Cyc_Absyn_Exp*_tmp8D1=test_exp;struct Cyc_Absyn_Stmt*_tmp8D0=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8D1,_tmp8D0,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D6;
struct Cyc_Absyn_Stmt*_tmp2D7=({struct _tuple1*_tmp8D4=temp;void*_tmp8D3=e1_c_type;struct Cyc_Absyn_Exp*_tmp8D2=e1;Cyc_Absyn_declare_stmt(_tmp8D4,_tmp8D3,_tmp8D2,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D7;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1635
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1638
void*_tmp2D8=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp2D8;void*_tmp2D9=_stmttmp29;union Cyc_Absyn_AggrInfo _tmp2DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f1)->tag == 20U){_LL1: _tmp2DA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2DA;
# 1640
struct Cyc_Absyn_Aggrdecl*_tmp2DB=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2DB;
({int _tmp8D5=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp8D5;});{
# 1643
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DF=({struct Cyc_String_pa_PrintArg_struct _tmp7C8;_tmp7C8.tag=0U,_tmp7C8.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7C8;});struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp7C7;_tmp7C7.tag=0U,_tmp7C7.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7C7;});void*_tmp2DD[2U];_tmp2DD[0]=& _tmp2DF,_tmp2DD[1]=& _tmp2E0;({struct _fat_ptr _tmp8D6=({const char*_tmp2DE="_union_%s_%s";_tag_fat(_tmp2DE,sizeof(char),13U);});Cyc_aprintf(_tmp8D6,_tag_fat(_tmp2DD,sizeof(void*),2U));});});
({void*_tmp8D7=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));*_tmp2DC=str;_tmp2DC;}));*tagged_member_type=_tmp8D7;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1654
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1657
void*_tmp2E1=e->r;void*_stmttmp2A=_tmp2E1;void*_tmp2E2=_stmttmp2A;int*_tmp2E5;struct _fat_ptr*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E3;int*_tmp2E8;struct _fat_ptr*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E9;switch(*((int*)_tmp2E2)){case 14U: _LL1: _tmp2E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp2E9;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C9;_tmp7C9.tag=0U,({struct _fat_ptr _tmp8D8=({const char*_tmp2EC="cast on lhs!";_tag_fat(_tmp2EC,sizeof(char),13U);});_tmp7C9.f1=_tmp8D8;});_tmp7C9;});void*_tmp2EA[1U];_tmp2EA[0]=& _tmp2EB;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp2EA,sizeof(void*),1U));});}case 21U: _LL3: _tmp2E6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E2)->f1;_tmp2E7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2;_tmp2E8=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E2)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2E6;struct _fat_ptr*f=_tmp2E7;int*is_read=_tmp2E8;
# 1660
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22U: _LL5: _tmp2E3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E2)->f1;_tmp2E4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E2)->f2;_tmp2E5=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E2)->f4;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp2E3;struct _fat_ptr*f=_tmp2E4;int*is_read=_tmp2E5;
# 1663
void*_tmp2ED=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp2B=_tmp2ED;void*_tmp2EE=_stmttmp2B;void*_tmp2EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EE)->tag == 3U){_LLA: _tmp2EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EE)->f1).elt_type;_LLB: {void*et=_tmp2EF;
# 1665
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{_LLC: _LLD:
# 1667
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1669
 return 0;}_LL0:;}
# 1682 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1686
struct _tuple30 _tmp2F0=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2C=_tmp2F0;struct _tuple30 _tmp2F1=_stmttmp2C;struct Cyc_Absyn_Exp*_tmp2F3;struct _tuple1*_tmp2F2;_LL1: _tmp2F2=_tmp2F1.f1;_tmp2F3=_tmp2F1.f2;_LL2: {struct _tuple1*temp=_tmp2F2;struct Cyc_Absyn_Exp*temp_exp=_tmp2F3;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1695
struct Cyc_Absyn_Exp*_tmp2F4=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2F4;
s2=({struct Cyc_Absyn_Exp*_tmp8DA=test_exp;struct Cyc_Absyn_Stmt*_tmp8D9=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8DA,_tmp8D9,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1698
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8DD=temp;void*_tmp8DC=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8DB=
Cyc_Toc_push_address_exp(e1);
# 1698
Cyc_Absyn_declare_stmt(_tmp8DD,_tmp8DC,_tmp8DB,
# 1700
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1705
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CB;_tmp7CB.tag=0U,({struct _fat_ptr _tmp8DE=({const char*_tmp2F8="exp_to_c: no type for ";_tag_fat(_tmp2F8,sizeof(char),23U);});_tmp7CB.f1=_tmp8DE;});_tmp7CB;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2F7=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7CA;_tmp7CA.tag=4U,_tmp7CA.f1=e;_tmp7CA;});void*_tmp2F5[2U];_tmp2F5[0]=& _tmp2F6,_tmp2F5[1]=& _tmp2F7;({unsigned _tmp8DF=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8DF,_tag_fat(_tmp2F5,sizeof(void*),2U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1710
int did_inserted_checks=0;
{void*_tmp2F9=e->annot;void*_stmttmp2D=_tmp2F9;void*_tmp2FA=_stmttmp2D;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2FA)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2FA)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1716
{void*_tmp2FB=e->r;void*_stmttmp2E=_tmp2FB;void*_tmp2FC=_stmttmp2E;struct Cyc_List_List*_tmp2FE;struct Cyc_List_List*_tmp2FD;struct Cyc_Absyn_Stmt*_tmp2FF;struct _fat_ptr*_tmp301;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_Absyn_Exp*_tmp302;int _tmp309;int _tmp308;struct Cyc_Absyn_Exp*_tmp307;void**_tmp306;struct Cyc_Absyn_Exp*_tmp305;int _tmp304;struct Cyc_Absyn_Datatypefield*_tmp30C;struct Cyc_Absyn_Datatypedecl*_tmp30B;struct Cyc_List_List*_tmp30A;struct Cyc_Absyn_Aggrdecl*_tmp310;struct Cyc_List_List*_tmp30F;struct Cyc_List_List*_tmp30E;struct _tuple1**_tmp30D;struct Cyc_List_List*_tmp312;void*_tmp311;int _tmp315;void*_tmp314;struct Cyc_Absyn_Exp*_tmp313;int _tmp319;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_Absyn_Vardecl*_tmp316;struct Cyc_List_List*_tmp31A;struct Cyc_List_List*_tmp31B;struct Cyc_Absyn_Exp*_tmp31D;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31E;int _tmp322;int _tmp321;struct _fat_ptr*_tmp320;struct Cyc_Absyn_Exp*_tmp31F;int _tmp326;int _tmp325;struct _fat_ptr*_tmp324;struct Cyc_Absyn_Exp*_tmp323;struct Cyc_List_List*_tmp328;void*_tmp327;void*_tmp329;struct Cyc_Absyn_Exp*_tmp32A;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Exp*_tmp32D;enum Cyc_Absyn_Coercion _tmp331;int _tmp330;struct Cyc_Absyn_Exp*_tmp32F;void**_tmp32E;struct Cyc_List_List*_tmp333;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Exp*_tmp334;int _tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_VarargInfo*_tmp33B;struct Cyc_List_List*_tmp33A;int _tmp339;struct Cyc_List_List*_tmp338;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_List_List*_tmp33D;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Core_Opt*_tmp348;struct Cyc_Absyn_Exp*_tmp347;enum Cyc_Absyn_Incrementor _tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_List_List*_tmp34D;enum Cyc_Absyn_Primop _tmp34C;struct Cyc_Absyn_Exp*_tmp34E;switch(*((int*)_tmp2FC)){case 2U: _LL8: _LL9:
# 1718
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1725
struct Cyc_Absyn_Exp*_tmp34F=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp34F;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8E0=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp8E0;});else{
# 1730
({void*_tmp8E2=({struct Cyc_Absyn_Exp*_tmp350[3U];_tmp350[0]=zero,_tmp350[1]=zero,_tmp350[2]=zero;({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8E1,_tag_fat(_tmp350,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8E2;});}}else{
# 1732
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1734
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 42U: _LL10: _LL11:
({void*_tmp8E3=(Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U))->r;e->r=_tmp8E3;});goto _LL7;case 41U: _LL12: _tmp34E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp34E;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3U: _LL14: _tmp34C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp34D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL15: {enum Cyc_Absyn_Primop p=_tmp34C;struct Cyc_List_List*es=_tmp34D;
# 1741
struct Cyc_List_List*_tmp351=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp351;
# 1743
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp352=p;switch(_tmp352){case Cyc_Absyn_Numelts: _LL63: _LL64: {
# 1746
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp353=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_stmttmp2F=_tmp353;void*_tmp354=_stmttmp2F;void*_tmp358;void*_tmp357;void*_tmp356;void*_tmp355;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354)->tag == 3U){_LL78: _tmp355=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354)->f1).elt_type;_tmp356=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354)->f1).ptr_atts).nullable;_tmp357=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354)->f1).ptr_atts).bounds;_tmp358=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354)->f1).ptr_atts).zero_term;_LL79: {void*elt_type=_tmp355;void*nbl=_tmp356;void*bound=_tmp357;void*zt=_tmp358;
# 1749
struct Cyc_Absyn_Exp*_tmp359=({void*_tmp8E4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E4,bound);});struct Cyc_Absyn_Exp*eopt=_tmp359;
if(eopt == 0)
# 1752
({void*_tmp8E7=({struct Cyc_Absyn_Exp*_tmp35A[2U];_tmp35A[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp35A[1]=_tmp8E5;});({struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp8E6,_tag_fat(_tmp35A,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1752
e->r=_tmp8E7;});else{
# 1755
struct Cyc_Absyn_Exp*_tmp35B=eopt;struct Cyc_Absyn_Exp*e2=_tmp35B;
# 1757
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1761
({void*_tmp8E9=({struct Cyc_Absyn_Exp*_tmp35C[2U];_tmp35C[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp35C[1]=e2;({struct Cyc_Absyn_Exp*_tmp8E8=function_e;Cyc_Toc_fncall_exp_r(_tmp8E8,_tag_fat(_tmp35C,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8E9;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(e2))
({void*_tmp35D=0U;({unsigned _tmp8EB=e->loc;struct _fat_ptr _tmp8EA=({const char*_tmp35E="can't calculate numelts";_tag_fat(_tmp35E,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8EB,_tmp8EA,_tag_fat(_tmp35D,sizeof(void*),0U));});});
# 1766
({void*_tmp8EE=({struct Cyc_Absyn_Exp*_tmp8ED=arg;struct Cyc_Absyn_Exp*_tmp8EC=e2;Cyc_Toc_conditional_exp_r(_tmp8ED,_tmp8EC,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8EE;});}else{
# 1768
e->r=e2->r;goto _LL77;}}}
# 1771
goto _LL77;}}else{_LL7A: _LL7B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp360=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CD;_tmp7CD.tag=0U,({struct _fat_ptr _tmp8EF=({const char*_tmp362="numelts primop applied to non-pointer ";_tag_fat(_tmp362,sizeof(char),39U);});_tmp7CD.f1=_tmp8EF;});_tmp7CD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp361=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7CC;_tmp7CC.tag=2U,_tmp7CC.f1=(void*)_check_null(arg->topt);_tmp7CC;});void*_tmp35F[2U];_tmp35F[0]=& _tmp360,_tmp35F[1]=& _tmp361;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp35F,sizeof(void*),2U));});}_LL77:;}
# 1775
goto _LL62;}case Cyc_Absyn_Plus: _LL65: _LL66:
# 1780
 if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp363="can't do pointer arithmetic at top-level";_tag_fat(_tmp363,sizeof(char),41U);}));
{void*_tmp364=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp30=_tmp364;void*_tmp365=_stmttmp30;void*_tmp368;void*_tmp367;void*_tmp366;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->tag == 3U){_LL7D: _tmp366=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->f1).elt_type;_tmp367=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->f1).ptr_atts).bounds;_tmp368=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->f1).ptr_atts).zero_term;_LL7E: {void*elt_type=_tmp366;void*b=_tmp367;void*zt=_tmp368;
# 1784
struct Cyc_Absyn_Exp*_tmp369=({void*_tmp8F0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8F0,b);});struct Cyc_Absyn_Exp*eopt=_tmp369;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp8F3=({struct Cyc_Absyn_Exp*_tmp36A[3U];_tmp36A[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36A[1]=_tmp8F1;}),_tmp36A[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8F2,_tag_fat(_tmp36A,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1788
e->r=_tmp8F3;});else{
# 1790
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp8F5=({struct Cyc_Absyn_Exp*_tmp36B[3U];_tmp36B[0]=e1,_tmp36B[1]=eopt,_tmp36B[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8F4,_tag_fat(_tmp36B,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8F5;});}
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
({void*_tmp8F7=({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8F6,Cyc_Toc_curr_sp);});e1->r=_tmp8F7;});
({void*_tmp8F9=({struct Cyc_Absyn_Exp*_tmp8F8=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8F8,Cyc_Toc_curr_sp);});e2->r=_tmp8F9;});
({void*_tmp8FB=({void*_tmp8FA=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8FA;});e1->topt=_tmp8FB;});
({void*_tmp8FD=({struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8FC,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1808
e->r=_tmp8FD;});}else{
# 1811
({void*_tmp901=({struct Cyc_Absyn_Exp*_tmp36C[3U];_tmp36C[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36C[1]=_tmp8FF;}),({
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp36C[2]=_tmp8FE;});({struct Cyc_Absyn_Exp*_tmp900=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp900,_tag_fat(_tmp36C,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1811
e->r=_tmp901;});}}
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
void*_tmp36D=({void*_tmp902=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp902,Cyc_Toc_mt_tq);});void*t=_tmp36D;
({void*_tmp905=({void*_tmp904=t;Cyc_Toc_cast_it_r(_tmp904,({struct Cyc_Absyn_Exp*_tmp903=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp903,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp905;});
e1->topt=t;}
# 1828
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp36E=({void*_tmp906=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp906,Cyc_Toc_mt_tq);});void*t=_tmp36E;
({void*_tmp909=({void*_tmp908=t;Cyc_Toc_cast_it_r(_tmp908,({struct Cyc_Absyn_Exp*_tmp907=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp907,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp909;});
e2->topt=t;}
# 1833
goto _LL62;}default: _LL75: _LL76:
 goto _LL62;}_LL62:;}
# 1836
goto _LL7;}case 5U: _LL16: _tmp34A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp34B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL17: {struct Cyc_Absyn_Exp*e2=_tmp34A;enum Cyc_Absyn_Incrementor incr=_tmp34B;
# 1838
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1847 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp38A="increment";_tag_fat(_tmp38A,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp36F="decrement";_tag_fat(_tmp36F,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp371=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D1;_tmp7D1.tag=0U,({struct _fat_ptr _tmp90A=({const char*_tmp377="in-place ";_tag_fat(_tmp377,sizeof(char),10U);});_tmp7D1.f1=_tmp90A;});_tmp7D1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp372=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D0;_tmp7D0.tag=0U,_tmp7D0.f1=incr_str;_tmp7D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp373=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CF;_tmp7CF.tag=0U,({struct _fat_ptr _tmp90B=({const char*_tmp376=" is not supported when ";_tag_fat(_tmp376,sizeof(char),24U);});_tmp7CF.f1=_tmp90B;});_tmp7CF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp374=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CE;_tmp7CE.tag=0U,({
struct _fat_ptr _tmp90C=({const char*_tmp375="dereferencing a zero-terminated pointer";_tag_fat(_tmp375,sizeof(char),40U);});_tmp7CE.f1=_tmp90C;});_tmp7CE;});void*_tmp370[4U];_tmp370[0]=& _tmp371,_tmp370[1]=& _tmp372,_tmp370[2]=& _tmp373,_tmp370[3]=& _tmp374;({unsigned _tmp90D=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp90D,_tag_fat(_tmp370,sizeof(void*),4U));});});
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp378=incr;switch(_tmp378){case Cyc_Absyn_PreInc: _LL82: _LL83:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec: _LL84: _LL85:
 op=2U;goto _LL81;default: _LL86: _LL87:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D4;_tmp7D4.tag=0U,({struct _fat_ptr _tmp90E=({const char*_tmp37E="in-place post";_tag_fat(_tmp37E,sizeof(char),14U);});_tmp7D4.f1=_tmp90E;});_tmp7D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D3;_tmp7D3.tag=0U,_tmp7D3.f1=incr_str;_tmp7D3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D2;_tmp7D2.tag=0U,({
struct _fat_ptr _tmp90F=({const char*_tmp37D=" is not supported on @tagged union members";_tag_fat(_tmp37D,sizeof(char),43U);});_tmp7D2.f1=_tmp90F;});_tmp7D2;});void*_tmp379[3U];_tmp379[0]=& _tmp37A,_tmp379[1]=& _tmp37B,_tmp379[2]=& _tmp37C;({unsigned _tmp910=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp910,_tag_fat(_tmp379,sizeof(void*),3U));});});}_LL81:;}
# 1865
({void*_tmp912=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->tag=4U,_tmp380->f1=e2,({struct Cyc_Core_Opt*_tmp911=({struct Cyc_Core_Opt*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->v=(void*)op;_tmp37F;});_tmp380->f2=_tmp911;}),_tmp380->f3=one;_tmp380;});e->r=_tmp912;});
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
({void*_tmp917=({struct Cyc_Absyn_Exp*_tmp381[3U];({struct Cyc_Absyn_Exp*_tmp915=Cyc_Toc_push_address_exp(e2);_tmp381[0]=_tmp915;}),({
struct Cyc_Absyn_Exp*_tmp914=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp381[1]=_tmp914;}),({
struct Cyc_Absyn_Exp*_tmp913=Cyc_Absyn_signed_int_exp(change,0U);_tmp381[2]=_tmp913;});({struct Cyc_Absyn_Exp*_tmp916=fn_e;Cyc_Toc_fncall_exp_r(_tmp916,_tag_fat(_tmp381,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1882
e->r=_tmp917;});}else{
# 1885
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1891
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp382=Cyc_Toc_temp_var();struct _tuple1*x=_tmp382;
void*_tmp383=({void*_tmp918=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp918,Cyc_Toc_mt_tq);});void*t=_tmp383;
struct Cyc_Absyn_Exp*_tmp384=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp384;
struct Cyc_Absyn_Exp*_tmp385=({struct Cyc_Absyn_Exp*_tmp919=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp919,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1896
struct Cyc_Absyn_Exp*testexp=_tmp385;
# 1898
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp91B=({struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp91A,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1899
Cyc_Absyn_and_exp(_tmp91B,testexp,0U);});{
# 1901
struct Cyc_Absyn_Stmt*_tmp386=({struct Cyc_Absyn_Exp*_tmp91E=testexp;struct Cyc_Absyn_Stmt*_tmp91D=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1901
Cyc_Absyn_ifthenelse_stmt(_tmp91E,_tmp91D,
# 1903
Cyc_Absyn_exp_stmt(({void*_tmp387=0U;({struct Cyc_Absyn_Exp*_tmp91C=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp91C,_tag_fat(_tmp387,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 1901
struct Cyc_Absyn_Stmt*s=_tmp386;
# 1904
s=({struct Cyc_Absyn_Stmt*_tmp91F=s;Cyc_Absyn_seq_stmt(_tmp91F,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp920=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp920;});}}else{
# 1908
struct Cyc_Toc_functionSet*_tmp388=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp388;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp924=({struct Cyc_Absyn_Exp*_tmp389[2U];({struct Cyc_Absyn_Exp*_tmp922=Cyc_Toc_push_address_exp(e2);_tmp389[0]=_tmp922;}),({struct Cyc_Absyn_Exp*_tmp921=Cyc_Absyn_signed_int_exp(1,0U);_tmp389[1]=_tmp921;});({struct Cyc_Absyn_Exp*_tmp923=fn_e;Cyc_Toc_fncall_exp_r(_tmp923,_tag_fat(_tmp389,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp924;});}}else{
# 1912
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1916
goto _LL7;}}case 4U: _LL18: _tmp347=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp348=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp349=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp347;struct Cyc_Core_Opt*popt=_tmp348;struct Cyc_Absyn_Exp*e2=_tmp349;
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
({void*_tmp925=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp925;});
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
{enum Cyc_Absyn_Primop _tmp38B=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp31=_tmp38B;enum Cyc_Absyn_Primop _tmp38C=_stmttmp31;switch(_tmp38C){case Cyc_Absyn_Plus: _LL89: _LL8A:
 change=e2;goto _LL88;case Cyc_Absyn_Minus: _LL8B: _LL8C:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL88;default: _LL8D: _LL8E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D5;_tmp7D5.tag=0U,({struct _fat_ptr _tmp926=({const char*_tmp38F="bad t ? pointer arithmetic";_tag_fat(_tmp38F,sizeof(char),27U);});_tmp7D5.f1=_tmp926;});_tmp7D5;});void*_tmp38D[1U];_tmp38D[0]=& _tmp38E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp38D,sizeof(void*),1U));});}_LL88:;}
# 1974
done=1;{
# 1976
struct Cyc_Absyn_Exp*_tmp390=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp390;
({void*_tmp92A=({struct Cyc_Absyn_Exp*_tmp391[3U];({struct Cyc_Absyn_Exp*_tmp928=Cyc_Toc_push_address_exp(e1);_tmp391[0]=_tmp928;}),({
struct Cyc_Absyn_Exp*_tmp927=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp391[1]=_tmp927;}),_tmp391[2]=change;({struct Cyc_Absyn_Exp*_tmp929=fn_e;Cyc_Toc_fncall_exp_r(_tmp929,_tag_fat(_tmp391,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1977
e->r=_tmp92A;});}}else{
# 1980
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1983
enum Cyc_Absyn_Primop _tmp392=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp32=_tmp392;enum Cyc_Absyn_Primop _tmp393=_stmttmp32;if(_tmp393 == Cyc_Absyn_Plus){_LL90: _LL91:
# 1985
 done=1;
({void*_tmp92C=({struct Cyc_Absyn_Exp*_tmp394[2U];_tmp394[0]=e1,_tmp394[1]=e2;({struct Cyc_Absyn_Exp*_tmp92B=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp92B,_tag_fat(_tmp394,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp92C;});
goto _LL8F;}else{_LL92: _LL93:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp396=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D6;_tmp7D6.tag=0U,({struct _fat_ptr _tmp92D=({const char*_tmp397="bad zero-terminated pointer arithmetic";_tag_fat(_tmp397,sizeof(char),39U);});_tmp7D6.f1=_tmp92D;});_tmp7D6;});void*_tmp395[1U];_tmp395[0]=& _tmp396;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp395,sizeof(void*),1U));});}_LL8F:;}}}
# 1992
if(!done){
# 1994
if(e1_poly)
({void*_tmp92F=({void*_tmp92E=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp92E,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp92F;});
# 2000
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp930=e1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*f_env))Cyc_Toc_lvalue_assign)(_tmp930,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->f1=popt,_tmp398->f2=e2;_tmp398;}));});
e->r=e1->r;}}
# 2005
goto _LL7;}}}}case 6U: _LL1A: _tmp344=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp345=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp346=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp344;struct Cyc_Absyn_Exp*e2=_tmp345;struct Cyc_Absyn_Exp*e3=_tmp346;
# 2007
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7U: _LL1C: _tmp342=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp343=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL1D: {struct Cyc_Absyn_Exp*e1=_tmp342;struct Cyc_Absyn_Exp*e2=_tmp343;
_tmp340=e1;_tmp341=e2;goto _LL1F;}case 8U: _LL1E: _tmp340=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp341=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp340;struct Cyc_Absyn_Exp*e2=_tmp341;
_tmp33E=e1;_tmp33F=e2;goto _LL21;}case 9U: _LL20: _tmp33E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp33F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp33E;struct Cyc_Absyn_Exp*e2=_tmp33F;
# 2014
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3 == 0){_LL22: _tmp33C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp33D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL23: {struct Cyc_Absyn_Exp*e1=_tmp33C;struct Cyc_List_List*es=_tmp33D;
# 2018
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2023
({void*_tmp934=({void*_tmp933=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp933,({struct Cyc_Absyn_Exp*_tmp399[1U];({struct Cyc_Absyn_Exp*_tmp931=
Cyc_Absyn_copy_exp(e1);_tmp399[0]=_tmp931;});({struct Cyc_Absyn_Exp*_tmp932=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp932,_tag_fat(_tmp399,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2023
e1->r=_tmp934;});
# 2025
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_LL24: _tmp337=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp338=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp339=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3)->num_varargs;_tmp33A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3)->injectors;_tmp33B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3)->vai;_LL25: {struct Cyc_Absyn_Exp*e1=_tmp337;struct Cyc_List_List*es=_tmp338;int num_varargs=_tmp339;struct Cyc_List_List*injectors=_tmp33A;struct Cyc_Absyn_VarargInfo*vai=_tmp33B;
# 2036 "toc.cyc"
struct _RegionHandle _tmp39A=_new_region("r");struct _RegionHandle*r=& _tmp39A;_push_region(r);{
struct _tuple30 _tmp39B=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp33=_tmp39B;struct _tuple30 _tmp39C=_stmttmp33;struct Cyc_Absyn_Exp*_tmp39E;struct _tuple1*_tmp39D;_LL95: _tmp39D=_tmp39C.f1;_tmp39E=_tmp39C.f2;_LL96: {struct _tuple1*argv=_tmp39D;struct Cyc_Absyn_Exp*argvexp=_tmp39E;
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
new_args=({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp39F->tl=new_args;_tmp39F;});}}
# 2052
new_args=({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_Exp*_tmp937=({struct Cyc_Absyn_Exp*_tmp3A0[3U];_tmp3A0[0]=argvexp,({
# 2054
struct Cyc_Absyn_Exp*_tmp935=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp3A0[1]=_tmp935;}),_tmp3A0[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp936=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp936,_tag_fat(_tmp3A0,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2052
_tmp3A1->hd=_tmp937;}),_tmp3A1->tl=new_args;_tmp3A1;});
# 2057
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2059
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2064
({void*_tmp93B=({void*_tmp93A=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp93A,({struct Cyc_Absyn_Exp*_tmp3A2[1U];({struct Cyc_Absyn_Exp*_tmp938=
Cyc_Absyn_copy_exp(e1);_tmp3A2[0]=_tmp938;});({struct Cyc_Absyn_Exp*_tmp939=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp939,_tag_fat(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2064
e1->r=_tmp93B;});{
# 2066
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2069
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp3A3=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp34=_tmp3A3;void*_tmp3A4=_stmttmp34;struct Cyc_Absyn_Datatypedecl*_tmp3A5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A4)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A4)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A4)->f1)->f1).KnownDatatype).tag == 2){_LL98: _tmp3A5=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A4)->f1)->f1).KnownDatatype).val;_LL99: {struct Cyc_Absyn_Datatypedecl*x=_tmp3A5;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A: _LL9B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D7;_tmp7D7.tag=0U,({struct _fat_ptr _tmp93C=({const char*_tmp3A8="unknown datatype in vararg with inject";_tag_fat(_tmp3A8,sizeof(char),39U);});_tmp7D7.f1=_tmp93C;});_tmp7D7;});void*_tmp3A6[1U];_tmp3A6[0]=& _tmp3A7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3A6,sizeof(void*),1U));});}_LL97:;}{
# 2075
struct _fat_ptr vs=({unsigned _tmp3B4=(unsigned)num_varargs;struct _tuple1**_tmp3B3=({struct _RegionHandle*_tmp93D=r;_region_malloc(_tmp93D,_check_times(_tmp3B4,sizeof(struct _tuple1*)));});({{unsigned _tmp7D8=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp7D8;++ i){({struct _tuple1*_tmp93E=Cyc_Toc_temp_var();_tmp3B3[i]=_tmp93E;});}}0;});_tag_fat(_tmp3B3,sizeof(struct _tuple1*),_tmp3B4);});
# 2077
if(num_varargs != 0){
# 2081
struct Cyc_List_List*_tmp3A9=0;struct Cyc_List_List*array_args=_tmp3A9;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct Cyc_Absyn_Exp*_tmp93F=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp3AA->hd=_tmp93F;}),_tmp3AA->tl=array_args;_tmp3AA;});}}
s=({struct _tuple1*_tmp942=argv;void*_tmp941=arr_type;struct Cyc_Absyn_Exp*_tmp940=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp942,_tmp941,_tmp940,s,0U);});
# 2086
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)- 1;
for(0;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp3AB=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp35=_tmp3AB;struct Cyc_Absyn_Datatypefield*_tmp3AC=_stmttmp35;struct Cyc_List_List*_tmp3AE;struct _tuple1*_tmp3AD;_LL9D: _tmp3AD=_tmp3AC->name;_tmp3AE=_tmp3AC->typs;_LL9E: {struct _tuple1*qv=_tmp3AD;struct Cyc_List_List*tqts=_tmp3AE;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2100
struct _tuple1*_tmp3AF=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp3AF;
struct Cyc_List_List*_tmp3B0=({struct _tuple19*_tmp3B2[2U];({struct _tuple19*_tmp946=({struct _fat_ptr*_tmp945=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp945,Cyc_Toc_datatype_tag(tud,qv));});_tmp3B2[0]=_tmp946;}),({
struct _tuple19*_tmp944=({struct _fat_ptr*_tmp943=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp943,arg);});_tmp3B2[1]=_tmp944;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3B2,sizeof(struct _tuple19*),2U));});
# 2101
struct Cyc_List_List*dles=_tmp3B0;
# 2103
s=({struct _tuple1*_tmp949=var;void*_tmp948=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp947=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->tag=29U,_tmp3B1->f1=tdn,_tmp3B1->f2=0,_tmp3B1->f3=dles,_tmp3B1->f4=0;_tmp3B1;}),0U);
# 2103
Cyc_Absyn_declare_stmt(_tmp949,_tmp948,_tmp947,s,0U);});}}}}}else{
# 2111
s=({struct _tuple1*_tmp94C=argv;void*_tmp94B=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp94A=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp94C,_tmp94B,_tmp94A,s,0U);});}}}else{
# 2116
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3B5->tl=array_args;_tmp3B5;});}{
# 2122
struct Cyc_Absyn_Exp*_tmp3B6=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp3B6;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2125
s=({struct _tuple1*_tmp94F=argv;void*_tmp94E=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp94D=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp94F,_tmp94E,_tmp94D,s,0U);});}}
# 2128
({void*_tmp950=Cyc_Toc_stmt_exp_r(s);e->r=_tmp950;});}}}}
# 2130
_npop_handler(0U);goto _LL7;
# 2036
;_pop_region();}}case 11U: _LL26: _tmp335=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp336=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp335;int b=_tmp336;
# 2133
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp953=({void*_tmp952=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp952,({struct Cyc_Absyn_Exp*_tmp3B7[1U];_tmp3B7[0]=e1;({struct Cyc_Absyn_Exp*_tmp951=fn_e;Cyc_Toc_fncall_exp_dl(_tmp951,_tag_fat(_tmp3B7,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp953;});
goto _LL7;}}case 12U: _LL28: _tmp334=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL29: {struct Cyc_Absyn_Exp*e1=_tmp334;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13U: _LL2A: _tmp332=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp333=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL2B: {struct Cyc_Absyn_Exp*e1=_tmp332;struct Cyc_List_List*ts=_tmp333;
# 2139
Cyc_Toc_exp_to_c(nv,e1);
# 2148 "toc.cyc"
for(0;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp3B8=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp3B8;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp3B9=Cyc_Tcutil_compress((void*)ts->hd);void*_stmttmp36=_tmp3B9;void*_tmp3BA=_stmttmp36;switch(*((int*)_tmp3BA)){case 2U: _LLA0: _LLA1:
 goto _LLA3;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BA)->f1)->tag == 18U){_LLA2: _LLA3:
 continue;}else{goto _LLA4;}default: _LLA4: _LLA5:
# 2155
({void*_tmp955=({void*_tmp954=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp954,e1,0U);})->r;e->r=_tmp955;});
goto _LL9F;}_LL9F:;}
# 2158
break;}}
# 2161
goto _LL7;}case 14U: _LL2C: _tmp32E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp32F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp330=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_tmp331=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FC)->f4;_LL2D: {void**t=_tmp32E;struct Cyc_Absyn_Exp*e1=_tmp32F;int user_inserted=_tmp330;enum Cyc_Absyn_Coercion coercion=_tmp331;
# 2163
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2170
{struct _tuple34 _tmp3BB=({struct _tuple34 _tmp7DA;({void*_tmp957=Cyc_Tcutil_compress(old_t2);_tmp7DA.f1=_tmp957;}),({void*_tmp956=Cyc_Tcutil_compress(new_typ);_tmp7DA.f2=_tmp956;});_tmp7DA;});struct _tuple34 _stmttmp37=_tmp3BB;struct _tuple34 _tmp3BC=_stmttmp37;struct Cyc_Absyn_PtrInfo _tmp3BD;struct Cyc_Absyn_PtrInfo _tmp3BF;struct Cyc_Absyn_PtrInfo _tmp3BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BC.f1)->tag == 3U)switch(*((int*)_tmp3BC.f2)){case 3U: _LLA7: _tmp3BE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BC.f1)->f1;_tmp3BF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BC.f2)->f1;_LLA8: {struct Cyc_Absyn_PtrInfo p1=_tmp3BE;struct Cyc_Absyn_PtrInfo p2=_tmp3BF;
# 2172
struct Cyc_Absyn_Exp*_tmp3C0=({void*_tmp958=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp958,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp3C0;
struct Cyc_Absyn_Exp*_tmp3C1=({void*_tmp959=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp959,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp3C1;
int _tmp3C2=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp3C2;
int _tmp3C3=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp3C3;
{struct _tuple35 _tmp3C4=({struct _tuple35 _tmp7D9;_tmp7D9.f1=b1,_tmp7D9.f2=b2;_tmp7D9;});struct _tuple35 _stmttmp38=_tmp3C4;struct _tuple35 _tmp3C5=_stmttmp38;if(_tmp3C5.f1 != 0){if(_tmp3C5.f2 != 0){_LLAE: _LLAF:
# 2179
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp95C=({void*_tmp95B=*t;Cyc_Toc_cast_it_r(_tmp95B,({struct Cyc_Absyn_Exp*_tmp3C6[1U];_tmp3C6[0]=e1;({struct Cyc_Absyn_Exp*_tmp95A=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp95A,_tag_fat(_tmp3C6,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp95C;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2188
goto _LLAD;}else{_LLB0: _LLB1: {
# 2190
struct Cyc_Absyn_Exp*_tmp3C7=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp3C7;
struct _tuple13 _tmp3C8=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp39=_tmp3C8;struct _tuple13 _tmp3C9=_stmttmp39;int _tmp3CB;unsigned _tmp3CA;_LLB7: _tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;_LLB8: {unsigned i=_tmp3CA;int valid=_tmp3CB;
if(Cyc_Toc_is_toplevel(nv)){
# 2196
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp95D=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp95D,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp95E=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp95E;});}else{
# 2201
if(zt1){
# 2206
struct _tuple30 _tmp3CC=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3A=_tmp3CC;struct _tuple30 _tmp3CD=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3CF;struct _tuple1*_tmp3CE;_LLBA: _tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;_LLBB: {struct _tuple1*x=_tmp3CE;struct Cyc_Absyn_Exp*x_exp=_tmp3CF;
struct Cyc_Absyn_Exp*arg3;
# 2210
{void*_tmp3D0=e1->r;void*_stmttmp3B=_tmp3D0;void*_tmp3D1=_stmttmp3B;struct Cyc_Absyn_Vardecl*_tmp3D2;struct Cyc_Absyn_Vardecl*_tmp3D3;switch(*((int*)_tmp3D1)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D1)->f1).Wstring_c).tag){case 8U: _LLBD: _LLBE:
 arg3=e2;goto _LLBC;case 9U: _LLBF: _LLC0:
 arg3=e2;goto _LLBC;default: goto _LLC5;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D1)->f1)){case 1U: _LLC1: _tmp3D3=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D1)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_tmp3D3;
_tmp3D2=vd;goto _LLC4;}case 4U: _LLC3: _tmp3D2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D1)->f1)->f1;_LLC4: {struct Cyc_Absyn_Vardecl*vd=_tmp3D2;
# 2215
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC6;
arg3=e2;
goto _LLBC;}default: goto _LLC5;}default: _LLC5: _LLC6:
# 2222
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2225
arg3=({struct Cyc_Absyn_Exp*_tmp3D4[2U];({
struct Cyc_Absyn_Exp*_tmp960=({void*_tmp95F=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp95F,x_exp);});_tmp3D4[0]=_tmp960;}),_tmp3D4[1]=e2;({struct Cyc_Absyn_Exp*_tmp961=
# 2225
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp961,_tag_fat(_tmp3D4,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2227
goto _LLBC;}_LLBC:;}
# 2229
if(!zt2 && !(p2.elt_tq).real_const)
# 2232
arg3=({struct Cyc_Absyn_Exp*_tmp962=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp962,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2234
struct Cyc_Absyn_Exp*_tmp3D5=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3D5;
struct Cyc_Absyn_Exp*_tmp3D6=({struct Cyc_Absyn_Exp*_tmp3D8[3U];_tmp3D8[0]=x_exp,_tmp3D8[1]=arg2,_tmp3D8[2]=arg3;({struct Cyc_Absyn_Exp*_tmp963=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp963,_tag_fat(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),3U));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3D6;
struct Cyc_Absyn_Stmt*_tmp3D7=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3D7;
s=({struct _tuple1*_tmp966=x;void*_tmp965=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp964=e1;Cyc_Absyn_declare_stmt(_tmp966,_tmp965,_tmp964,s,0U);});
({void*_tmp967=Cyc_Toc_stmt_exp_r(s);e->r=_tmp967;});}}}else{
# 2241
({void*_tmp96A=({struct Cyc_Absyn_Exp*_tmp3D9[3U];_tmp3D9[0]=e1,({
# 2243
struct Cyc_Absyn_Exp*_tmp968=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3D9[1]=_tmp968;}),_tmp3D9[2]=e2;({struct Cyc_Absyn_Exp*_tmp969=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp969,_tag_fat(_tmp3D9,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2241
e->r=_tmp96A;});}}
# 2246
goto _LLAD;}}}}else{if(_tmp3C5.f2 != 0){_LLB2: _LLB3: {
# 2256 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3DA=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3DA;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp96B=b2;Cyc_Absyn_add_exp(_tmp96B,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2263
struct Cyc_Absyn_Exp*_tmp3DB=({struct Cyc_Absyn_Exp*_tmp3DD[3U];_tmp3DD[0]=e1,({
# 2265
struct Cyc_Absyn_Exp*_tmp96C=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3DD[1]=_tmp96C;}),_tmp3DD[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp96D,_tag_fat(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2263
struct Cyc_Absyn_Exp*ptr_exp=_tmp3DB;
# 2267
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp970=({struct Cyc_Absyn_Exp*_tmp3DC[1U];({struct Cyc_Absyn_Exp*_tmp96E=Cyc_Absyn_copy_exp(ptr_exp);_tmp3DC[0]=_tmp96E;});({struct Cyc_Absyn_Exp*_tmp96F=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp96F,_tag_fat(_tmp3DC,sizeof(struct Cyc_Absyn_Exp*),1U));});});ptr_exp->r=_tmp970;});
({void*_tmp971=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp971;});
goto _LLAD;}}}else{_LLB4: _LLB5:
# 2275
 if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp3DE="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3DE,sizeof(char),70U);}));
({void*_tmp975=({struct Cyc_Absyn_Exp*_tmp3DF[3U];_tmp3DF[0]=e1,({
struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3DF[1]=_tmp973;}),({
struct Cyc_Absyn_Exp*_tmp972=Cyc_Absyn_uint_exp(1U,0U);_tmp3DF[2]=_tmp972;});({struct Cyc_Absyn_Exp*_tmp974=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp974,_tag_fat(_tmp3DF,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2278
e->r=_tmp975;});}
# 2282
goto _LLAD;}}_LLAD:;}
# 2284
goto _LLA6;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BC.f2)->f1)->tag == 1U){_LLA9: _tmp3BD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BC.f1)->f1;_LLAA: {struct Cyc_Absyn_PtrInfo p1=_tmp3BD;
# 2286
{struct Cyc_Absyn_Exp*_tmp3E0=({void*_tmp976=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp976,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3C=_tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1=_stmttmp3C;if(_tmp3E1 == 0){_LLC8: _LLC9:
# 2288
({void*_tmp978=({struct Cyc_Absyn_Exp*_tmp977=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp977,Cyc_Toc_curr_sp);});e1->r=_tmp978;});
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
goto _LL7;}case 15U: _LL2E: _tmp32D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL2F: {struct Cyc_Absyn_Exp*e1=_tmp32D;
# 2302
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2308
({void*_tmp97A=({void*_tmp979=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp979,e1);});e->r=_tmp97A;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2312
({void*_tmp97C=({void*_tmp97B=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp97B,e1);});e->r=_tmp97C;});}
# 2314
goto _LL7;}case 16U: _LL30: _tmp32B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp32C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL31: {struct Cyc_Absyn_Exp*rgnopt=_tmp32B;struct Cyc_Absyn_Exp*e1=_tmp32C;
# 2321
Cyc_Toc_exp_to_c(nv,e1);{
# 2323
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple30 _tmp3E2=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3D=_tmp3E2;struct _tuple30 _tmp3E3=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp3E5;struct _tuple1*_tmp3E4;_LLCD: _tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E3.f2;_LLCE: {struct _tuple1*x=_tmp3E4;struct Cyc_Absyn_Exp*xexp=_tmp3E5;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3E6=Cyc_Tcutil_compress(elt_typ);void*_stmttmp3E=_tmp3E6;void*_tmp3E7=_stmttmp3E;void*_tmp3E8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E7)->tag == 4U){_LLD0: _tmp3E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E7)->f1).elt_type;_LLD1: {void*t2=_tmp3E8;
# 2328
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCF;}}else{_LLD2: _LLD3:
# 2332
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCF;}_LLCF:;}{
# 2335
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3E9=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3E9;
struct Cyc_Absyn_Exp*_tmp3EA=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3EA;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3EB[2U];_tmp3EB[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp97D=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3EB[1]=_tmp97D;});({struct Cyc_Absyn_Exp*_tmp97E=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp97E,_tag_fat(_tmp3EB,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2342
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2345
struct Cyc_Absyn_Exp*_tmp3EC=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3EC;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2349
if(rgnopt == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2352
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2355
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3ED[3U];({
struct Cyc_Absyn_Exp*_tmp981=Cyc_Absyn_copy_exp(xexp);_tmp3ED[0]=_tmp981;}),({
struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3ED[1]=_tmp980;}),
(unsigned)lenexp?_tmp3ED[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp97F=Cyc_Absyn_uint_exp(1U,0U);_tmp3ED[2]=_tmp97F;});({struct Cyc_Absyn_Exp*_tmp982=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp982,_tag_fat(_tmp3ED,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2362
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp983=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->tag=Cyc_Toc_Dest,_tmp3EE->f1=xexp;_tmp3EE;});e->annot=_tmp983;});{
struct Cyc_Absyn_Stmt*_tmp3EF=({struct _tuple1*_tmp987=x;void*_tmp986=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp985=mexp;Cyc_Absyn_declare_stmt(_tmp987,_tmp986,_tmp985,({
struct Cyc_Absyn_Stmt*_tmp984=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp984,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2364
struct Cyc_Absyn_Stmt*s=_tmp3EF;
# 2368
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp988=Cyc_Toc_stmt_exp_r(s);e->r=_tmp988;});
if(vse != 0)
({void*_tmp98A=({void*_tmp989=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp989,Cyc_Absyn_copy_exp(e));});e->r=_tmp98A;});
goto _LL7;}}}}}}}case 18U: _LL32: _tmp32A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL33: {struct Cyc_Absyn_Exp*e1=_tmp32A;
# 2376
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17U: _LL34: _tmp329=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL35: {void*t=_tmp329;
({void*_tmp98C=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->tag=17U,({void*_tmp98B=Cyc_Toc_typ_to_c(t);_tmp3F0->f1=_tmp98B;});_tmp3F0;});e->r=_tmp98C;});goto _LL7;}case 19U: _LL36: _tmp327=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp328=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL37: {void*t=_tmp327;struct Cyc_List_List*fs=_tmp328;
# 2379
void*_tmp3F1=t;void*t2=_tmp3F1;
struct Cyc_List_List*_tmp3F2=fs;struct Cyc_List_List*l=_tmp3F2;
for(0;l != 0;l=l->tl){
void*_tmp3F3=(void*)l->hd;void*_stmttmp3F=_tmp3F3;void*_tmp3F4=_stmttmp3F;unsigned _tmp3F5;struct _fat_ptr*_tmp3F6;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F4)->tag == 0U){_LLD5: _tmp3F6=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F4)->f1;_LLD6: {struct _fat_ptr*n=_tmp3F6;
goto _LLD4;}}else{_LLD7: _tmp3F5=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3F4)->f1;_LLD8: {unsigned n=_tmp3F5;
# 2385
{void*_tmp3F7=Cyc_Tcutil_compress(t2);void*_stmttmp40=_tmp3F7;void*_tmp3F8=_stmttmp40;struct Cyc_List_List*_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Datatypefield*_tmp3FB;union Cyc_Absyn_AggrInfo _tmp3FC;switch(*((int*)_tmp3F8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F8)->f1)){case 20U: _LLDA: _tmp3FC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F8)->f1)->f1;_LLDB: {union Cyc_Absyn_AggrInfo info=_tmp3FC;
# 2387
struct Cyc_Absyn_Aggrdecl*_tmp3FD=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp3FD;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DB;_tmp7DB.tag=0U,({struct _fat_ptr _tmp98D=({const char*_tmp400="struct fields must be known";_tag_fat(_tmp400,sizeof(char),28U);});_tmp7DB.f1=_tmp98D;});_tmp7DB;});void*_tmp3FE[1U];_tmp3FE[0]=& _tmp3FF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3FE,sizeof(void*),1U));});
_tmp3FA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F8)->f1)->f1).KnownDatatypefield).tag == 2){_LLE0: _tmp3FB=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F8)->f1)->f1).KnownDatatypefield).val).f2;_LLE1: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3FB;
# 2401
if(n == (unsigned)0)
({void*_tmp98E=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404->tag=0U,_tmp404->f1=Cyc_Toc_tag_sp;_tmp404;}));l->hd=_tmp98E;});else{
# 2404
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp990=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->tag=0U,({struct _fat_ptr*_tmp98F=Cyc_Absyn_fieldname((int)n);_tmp405->f1=_tmp98F;});_tmp405;}));l->hd=_tmp990;});}
# 2407
goto _LLD9;}}else{goto _LLE2;}default: goto _LLE2;}case 7U: _LLDC: _tmp3FA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F8)->f2;_LLDD: {struct Cyc_List_List*fields=_tmp3FA;
# 2392
struct Cyc_Absyn_Aggrfield*_tmp401=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp401;
({void*_tmp991=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->tag=0U,_tmp402->f1=nth_field->name;_tmp402;}));l->hd=_tmp991;});
t2=nth_field->type;
goto _LLD9;}case 6U: _LLDE: _tmp3F9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F8)->f1;_LLDF: {struct Cyc_List_List*ts=_tmp3F9;
# 2397
({void*_tmp993=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->tag=0U,({struct _fat_ptr*_tmp992=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp403->f1=_tmp992;});_tmp403;}));l->hd=_tmp993;});
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD9;}default: _LLE2: _LLE3:
# 2408
({struct Cyc_Warn_String_Warn_Warg_struct _tmp407=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DC;_tmp7DC.tag=0U,({struct _fat_ptr _tmp994=({const char*_tmp408="impossible type for offsetof tuple index";_tag_fat(_tmp408,sizeof(char),41U);});_tmp7DC.f1=_tmp994;});_tmp7DC;});void*_tmp406[1U];_tmp406[0]=& _tmp407;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp406,sizeof(void*),1U));});}_LLD9:;}
# 2410
goto _LLD4;}}_LLD4:;}
# 2413
({void*_tmp996=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->tag=19U,({void*_tmp995=Cyc_Toc_typ_to_c(t);_tmp409->f1=_tmp995;}),_tmp409->f2=fs;_tmp409;});e->r=_tmp996;});
goto _LL7;}case 21U: _LL38: _tmp323=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp324=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp325=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_tmp326=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FC)->f4;_LL39: {struct Cyc_Absyn_Exp*e1=_tmp323;struct _fat_ptr*f=_tmp324;int is_tagged=_tmp325;int is_read=_tmp326;
# 2416
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp99B=({struct Cyc_Absyn_Exp*_tmp99A=e1;void*_tmp999=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp998=e1_cyc_type;struct _fat_ptr*_tmp997=f;Cyc_Toc_check_tagged_union(_tmp99A,_tmp999,_tmp998,_tmp997,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2420
e->r=_tmp99B;});
# 2422
if(is_poly)
({void*_tmp99D=({void*_tmp99C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99C,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp99D;});
goto _LL7;}case 22U: _LL3A: _tmp31F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp320=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp321=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_tmp322=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FC)->f4;_LL3B: {struct Cyc_Absyn_Exp*e1=_tmp31F;struct _fat_ptr*f=_tmp320;int is_tagged=_tmp321;int is_read=_tmp322;
# 2426
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp40A=Cyc_Tcutil_compress(e1typ);void*_stmttmp41=_tmp40A;void*_tmp40B=_stmttmp41;struct Cyc_Absyn_PtrInfo _tmp40C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B)->tag == 3U){_LLE5: _tmp40C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B)->f1;_LLE6: {struct Cyc_Absyn_PtrInfo p=_tmp40C;
ta=p.elt_type;goto _LLE4;}}else{_LLE7: _LLE8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DD;_tmp7DD.tag=0U,({struct _fat_ptr _tmp99E=({const char*_tmp40F="get_ptr_typ: not a pointer!";_tag_fat(_tmp40F,sizeof(char),28U);});_tmp7DD.f1=_tmp99E;});_tmp7DD;});void*_tmp40D[1U];_tmp40D[0]=& _tmp40E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp40D,sizeof(void*),1U));});}_LLE4:;}
# 2433
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp9A3=({struct Cyc_Absyn_Exp*_tmp9A2=e1;void*_tmp9A1=Cyc_Toc_typ_to_c(e1typ);void*_tmp9A0=ta;struct _fat_ptr*_tmp99F=f;Cyc_Toc_check_tagged_union(_tmp9A2,_tmp9A1,_tmp9A0,_tmp99F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9A3;});
if(is_poly && is_read)
({void*_tmp9A5=({void*_tmp9A4=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9A4,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9A5;});
goto _LL7;}case 20U: _LL3C: _tmp31E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL3D: {struct Cyc_Absyn_Exp*e1=_tmp31E;
# 2441
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23U: _LL3E: _tmp31C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp31D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL3F: {struct Cyc_Absyn_Exp*e1=_tmp31C;struct Cyc_Absyn_Exp*e2=_tmp31D;
# 2445
{void*_tmp410=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp42=_tmp410;void*_tmp411=_stmttmp42;struct Cyc_List_List*_tmp412;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp411)->tag == 6U){_LLEA: _tmp412=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp411)->f1;_LLEB: {struct Cyc_List_List*ts=_tmp412;
# 2447
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp413=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp413;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _tmp414=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp43=_tmp414;struct _tuple13 _tmp415=_stmttmp43;int _tmp417;unsigned _tmp416;_LLEF: _tmp416=_tmp415.f1;_tmp417=_tmp415.f2;_LLF0: {unsigned i=_tmp416;int known=_tmp417;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp419=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DE;_tmp7DE.tag=0U,({struct _fat_ptr _tmp9A6=({const char*_tmp41A="unknown tuple subscript in translation to C";_tag_fat(_tmp41A,sizeof(char),44U);});_tmp7DE.f1=_tmp9A6;});_tmp7DE;});void*_tmp418[1U];_tmp418[0]=& _tmp419;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp418,sizeof(void*),1U));});
({void*_tmp9A8=({struct Cyc_Absyn_Exp*_tmp9A7=e1;Cyc_Toc_aggrmember_exp_r(_tmp9A7,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp9A8;});
goto _LLE9;}}}}}else{_LLEC: _LLED:
# 2457
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp9A9=Cyc_Toc_deref_exp_r(e1);e->r=_tmp9A9;});
goto _LLE9;}}_LLE9:;}
# 2463
goto _LL7;}case 24U: _LL40: _tmp31B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL41: {struct Cyc_List_List*es=_tmp31B;
# 2465
if(!Cyc_Toc_is_toplevel(nv)){
# 2467
void*_tmp41B=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple23*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp41B;
{void*_tmp41C=strct_typ;union Cyc_Absyn_AggrInfo _tmp41D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41C)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41C)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41C)->f2 == 0){_LLF2: _tmp41D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41C)->f1)->f1;_LLF3: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp41D;
# 2470
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp41E=_cycalloc(sizeof(*_tmp41E));({struct _tuple19*_tmp9AB=({struct _fat_ptr*_tmp9AA=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9AA,(struct Cyc_Absyn_Exp*)es->hd);});_tmp41E->hd=_tmp9AB;}),_tmp41E->tl=dles;_tmp41E;});}}
# 2476
({void*_tmp9AD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->tag=29U,_tmp41F->f1=sd->name,_tmp41F->f2=0,({struct Cyc_List_List*_tmp9AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp41F->f3=_tmp9AC;}),_tmp41F->f4=sd;_tmp41F;});e->r=_tmp9AD;});
e->topt=strct_typ;
goto _LLF1;}}else{goto _LLF4;}}else{goto _LLF4;}}else{_LLF4: _LLF5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp421=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DF;_tmp7DF.tag=0U,({struct _fat_ptr _tmp9AE=({const char*_tmp422="tuple type not an aggregate";_tag_fat(_tmp422,sizeof(char),28U);});_tmp7DF.f1=_tmp9AE;});_tmp7DF;});void*_tmp420[1U];_tmp420[0]=& _tmp421;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp420,sizeof(void*),1U));});}_LLF1:;}
# 2481
goto _LL7;}else{
# 2485
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp424=_cycalloc(sizeof(*_tmp424));({struct _tuple19*_tmp9AF=({struct _tuple19*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->f1=0,_tmp423->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp423;});_tmp424->hd=_tmp9AF;}),_tmp424->tl=dles;_tmp424;});}
# 2490
({void*_tmp9B0=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9B0;});}
# 2492
goto _LL7;}case 26U: _LL42: _tmp31A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL43: {struct Cyc_List_List*dles0=_tmp31A;
# 2498
{struct Cyc_List_List*_tmp425=dles0;struct Cyc_List_List*dles=_tmp425;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9B1=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp9B1;});
goto _LL7;}case 27U: _LL44: _tmp316=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp317=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp318=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_tmp319=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FC)->f4;_LL45: {struct Cyc_Absyn_Vardecl*vd=_tmp316;struct Cyc_Absyn_Exp*e1=_tmp317;struct Cyc_Absyn_Exp*e2=_tmp318;int iszeroterm=_tmp319;
# 2506
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp426=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _stmttmp44=_tmp426;struct _tuple13 _tmp427=_stmttmp44;int _tmp429;unsigned _tmp428;_LLF7: _tmp428=_tmp427.f1;_tmp429=_tmp427.f2;_LLF8: {unsigned sz=_tmp428;int known=_tmp429;
void*_tmp42A=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp42A;
struct Cyc_List_List*es=0;
# 2513
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp42B=0U;({unsigned _tmp9B3=e1->loc;struct _fat_ptr _tmp9B2=({const char*_tmp42C="cannot determine value of constant";_tag_fat(_tmp42C,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9B3,_tmp9B2,_tag_fat(_tmp42B,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp42E=_cycalloc(sizeof(*_tmp42E));({struct _tuple19*_tmp9B4=({struct _tuple19*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->f1=0,_tmp42D->f2=e2;_tmp42D;});_tmp42E->hd=_tmp9B4;}),_tmp42E->tl=es;_tmp42E;});}}
# 2519
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp42F=({void*_tmp9B5=elt_typ;Cyc_Toc_cast_it(_tmp9B5,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp42F;
es=({struct Cyc_List_List*_tmp9B7=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9B7,({struct Cyc_List_List*_tmp431=_cycalloc(sizeof(*_tmp431));({struct _tuple19*_tmp9B6=({struct _tuple19*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430->f1=0,_tmp430->f2=ezero;_tmp430;});_tmp431->hd=_tmp9B6;}),_tmp431->tl=0;_tmp431;}));});}}
# 2524
({void*_tmp9B8=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9B8;});}}
# 2526
goto _LL7;}case 28U: _LL46: _tmp313=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp314=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp315=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_LL47: {struct Cyc_Absyn_Exp*e1=_tmp313;void*t1=_tmp314;int iszeroterm=_tmp315;
# 2533
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9B9=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9B9;});else{
# 2536
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30U: _LL48: _tmp311=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp312=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL49: {void*st=_tmp311;struct Cyc_List_List*dles=_tmp312;
# 2540
{struct Cyc_List_List*_tmp432=dles;struct Cyc_List_List*dles2=_tmp432;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*_tmp433=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp433;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp434=Cyc_Tcutil_compress(strct_typ);void*_stmttmp45=_tmp434;void*_tmp435=_stmttmp45;union Cyc_Absyn_AggrInfo _tmp436;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp435)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp435)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp435)->f2 == 0){_LLFA: _tmp436=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp435)->f1)->f1;_LLFB: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp436;
# 2547
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp9BA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=29U,_tmp437->f1=sd->name,_tmp437->f2=0,_tmp437->f3=dles,_tmp437->f4=sd;_tmp437;});e->r=_tmp9BA;});
e->topt=strct_typ;
goto _LLF9;}}else{goto _LLFC;}}else{goto _LLFC;}}else{_LLFC: _LLFD:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp439=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E0;_tmp7E0.tag=0U,({struct _fat_ptr _tmp9BB=({const char*_tmp43A="anonStruct type not an aggregate";_tag_fat(_tmp43A,sizeof(char),33U);});_tmp7E0.f1=_tmp9BB;});_tmp7E0;});void*_tmp438[1U];_tmp438[0]=& _tmp439;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp438,sizeof(void*),1U));});}_LLF9:;}else{
# 2554
({void*_tmp9BC=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp9BC;});}
goto _LL7;}}case 29U: _LL4A: _tmp30D=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp30E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp30F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_tmp310=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FC)->f4;_LL4B: {struct _tuple1**tdn=_tmp30D;struct Cyc_List_List*exist_ts=_tmp30E;struct Cyc_List_List*dles=_tmp30F;struct Cyc_Absyn_Aggrdecl*sd=_tmp310;
# 2560
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E1;_tmp7E1.tag=0U,({struct _fat_ptr _tmp9BD=({const char*_tmp43D="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp43D,sizeof(char),58U);});_tmp7E1.f1=_tmp9BD;});_tmp7E1;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43B,sizeof(void*),1U));});{
void*_tmp43E=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp43E;
{void*_tmp43F=Cyc_Tcutil_compress(new_typ);void*_stmttmp46=_tmp43F;void*_tmp440=_stmttmp46;union Cyc_Absyn_AggrInfo _tmp441;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp440)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp440)->f1)->tag == 20U){_LLFF: _tmp441=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp440)->f1)->f1;_LL100: {union Cyc_Absyn_AggrInfo info=_tmp441;
({struct _tuple1*_tmp9BE=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp9BE;});goto _LLFE;}}else{goto _LL101;}}else{_LL101: _LL102:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp443=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E2;_tmp7E2.tag=0U,({struct _fat_ptr _tmp9BF=({const char*_tmp444="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp444,sizeof(char),44U);});_tmp7E2.f1=_tmp9BF;});_tmp7E2;});void*_tmp442[1U];_tmp442[0]=& _tmp443;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp442,sizeof(void*),1U));});}_LLFE:;}{
# 2571
struct Cyc_List_List*_tmp445=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp445;
if(typ_fields == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp446=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp446;
struct Cyc_List_List*_tmp447=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp447;
# 2578
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp448=Cyc_Tcutil_compress(old_typ);void*_stmttmp47=_tmp448;void*_tmp449=_stmttmp47;struct Cyc_List_List*_tmp44A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->f1)->tag == 20U){_LL104: _tmp44A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp449)->f2;_LL105: {struct Cyc_List_List*param_ts=_tmp44A;
# 2582
{struct Cyc_List_List*_tmp44B=fields;struct Cyc_List_List*fields2=_tmp44B;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_tmp44C=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp48=_tmp44C;struct _tuple36*_tmp44D=_stmttmp48;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Aggrfield*_tmp44E;_LL109: _tmp44E=_tmp44D->f1;_tmp44F=_tmp44D->f2;_LL10A: {struct Cyc_Absyn_Aggrfield*aggrfield=_tmp44E;struct Cyc_Absyn_Exp*fieldexp=_tmp44F;
void*_tmp450=fieldexp->topt;void*old_field_typ=_tmp450;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2587
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp9C1=({void*_tmp9C0=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp9C0,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2589
fieldexp->r=_tmp9C1;});
# 2592
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp451=aggrfield->name;struct _fat_ptr*n=_tmp451;
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp452;
struct _tuple19*_tmp453=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp453;
struct _tuple19*_tmp454=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple19*val_dle=_tmp454;
struct _tuple1*s=({struct _tuple1*_tmp45F=_cycalloc(sizeof(*_tmp45F));({union Cyc_Absyn_Nmspace _tmp9C5=Cyc_Absyn_Abs_n(0,1);_tmp45F->f1=_tmp9C5;}),({
struct _fat_ptr*_tmp9C4=({struct _fat_ptr*_tmp45E=_cycalloc(sizeof(*_tmp45E));({struct _fat_ptr _tmp9C3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45C=({struct Cyc_String_pa_PrintArg_struct _tmp7E4;_tmp7E4.tag=0U,_tmp7E4.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp7E4;});struct Cyc_String_pa_PrintArg_struct _tmp45D=({struct Cyc_String_pa_PrintArg_struct _tmp7E3;_tmp7E3.tag=0U,_tmp7E3.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp7E3;});void*_tmp45A[2U];_tmp45A[0]=& _tmp45C,_tmp45A[1]=& _tmp45D;({struct _fat_ptr _tmp9C2=({const char*_tmp45B="_union_%s_%s";_tag_fat(_tmp45B,sizeof(char),13U);});Cyc_aprintf(_tmp9C2,_tag_fat(_tmp45A,sizeof(void*),2U));});});*_tmp45E=_tmp9C3;});_tmp45E;});_tmp45F->f2=_tmp9C4;});_tmp45F;});
# 2600
struct _tuple19*_tmp455=({
struct _fat_ptr*_tmp9C7=n;Cyc_Toc_make_field(_tmp9C7,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->tag=29U,_tmp459->f1=s,_tmp459->f2=0,({struct Cyc_List_List*_tmp9C6=({struct _tuple19*_tmp458[2U];_tmp458[0]=tag_dle,_tmp458[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp458,sizeof(struct _tuple19*),2U));});_tmp459->f3=_tmp9C6;}),_tmp459->f4=0;_tmp459;}),0U));});
# 2600
struct _tuple19*u_dle=_tmp455;
# 2603
({void*_tmp9C9=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->tag=29U,_tmp457->f1=*tdn,_tmp457->f2=0,({struct Cyc_List_List*_tmp9C8=({struct _tuple19*_tmp456[1U];_tmp456[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp456,sizeof(struct _tuple19*),1U));});_tmp457->f3=_tmp9C8;}),_tmp457->f4=sd;_tmp457;});e->r=_tmp9C9;});}
# 2609
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp460=({struct Cyc_List_List*_tmp9CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9CA,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(sd->tvs,param_ts));});
# 2610
struct Cyc_List_List*inst=_tmp460;
# 2612
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp461=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp461;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp462=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp462;
void*_tmp463=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp463;
struct Cyc_Absyn_Aggrfield*_tmp464=({struct Cyc_Absyn_Aggrfield*_tmp9CB=old_f;Cyc_Toc_aggrfield_to_c(_tmp9CB,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2616
struct Cyc_Absyn_Aggrfield*new_f=_tmp464;
# 2618
new_fields=({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->hd=new_f,_tmp465->tl=new_fields;_tmp465;});
# 2624
if(fs->tl == 0){
{void*_tmp466=Cyc_Tcutil_compress(new_f->type);void*_stmttmp49=_tmp466;void*_tmp467=_stmttmp49;struct Cyc_Absyn_ArrayInfo _tmp468;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp467)->tag == 4U){_LL10C: _tmp468=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp467)->f1;_LL10D: {struct Cyc_Absyn_ArrayInfo ai=_tmp468;
# 2627
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp469=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp469;
({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp9CC;});
({void*_tmp9CD=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=4U,_tmp46A->f1=ai2;_tmp46A;});new_f->type=_tmp9CD;});}
# 2632
goto _LL10B;}}else{_LL10E: _LL10F:
# 2636
 if(fieldexp->topt == 0)
goto _LL10B;
{void*_tmp46B=Cyc_Tcutil_compress((void*)_check_null(fieldexp->topt));void*_stmttmp4A=_tmp46B;void*_tmp46C=_stmttmp4A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46C)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46C)->f1)->tag == 20U){_LL111: _LL112:
# 2640
 new_f->type=(void*)_check_null(fieldexp->topt);goto _LL110;}else{goto _LL113;}}else{_LL113: _LL114:
 goto _LL110;}_LL110:;}
# 2643
goto _LL10B;}_LL10B:;}
# 2647
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp9CF=({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));({void*_tmp9CE=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=6U,_tmp46D->f1=0;_tmp46D;});_tmp46E->hd=_tmp9CE;}),_tmp46E->tl=new_f->attributes;_tmp46E;});new_f->attributes=_tmp9CF;});}}}
# 2653
sd=({struct _fat_ptr*_tmp9D2=({struct _fat_ptr*_tmp472=_cycalloc(sizeof(*_tmp472));({struct _fat_ptr _tmp9D1=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp471=({struct Cyc_Int_pa_PrintArg_struct _tmp7E5;_tmp7E5.tag=1U,_tmp7E5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7E5;});void*_tmp46F[1U];_tmp46F[0]=& _tmp471;({struct _fat_ptr _tmp9D0=({const char*_tmp470="_genStruct%d";_tag_fat(_tmp470,sizeof(char),13U);});Cyc_aprintf(_tmp9D0,_tag_fat(_tmp46F,sizeof(void*),1U));});});*_tmp472=_tmp9D1;});_tmp472;});Cyc_Toc_make_c_struct_defn(_tmp9D2,
# 2655
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2659
({void*_tmp9D3=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp473=_cycalloc(sizeof(*_tmp473));*_tmp473=sd;_tmp473;})),0);e->topt=_tmp9D3;});}}}}
# 2662
goto _LL103;}}else{goto _LL106;}}else{_LL106: _LL107:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp475=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E6;_tmp7E6.tag=0U,({struct _fat_ptr _tmp9D4=({const char*_tmp476="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp476,sizeof(char),39U);});_tmp7E6.f1=_tmp9D4;});_tmp7E6;});void*_tmp474[1U];_tmp474[0]=& _tmp475;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp474,sizeof(void*),1U));});}_LL103:;}else{
# 2671
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2673
struct _tuple36*_tmp477=(struct _tuple36*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple36*_stmttmp4B=_tmp477;struct _tuple36*_tmp478=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp47A;struct Cyc_Absyn_Aggrfield*_tmp479;_LL116: _tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;_LL117: {struct Cyc_Absyn_Aggrfield*field=_tmp479;struct Cyc_Absyn_Exp*fieldexp=_tmp47A;
void*_tmp47B=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp47B;
void*_tmp47C=field->type;void*fieldtyp=_tmp47C;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp9D6=({
void*_tmp9D5=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp9D5,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2679
fieldexp->r=_tmp9D6;});{
# 2682
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp47D=({struct _tuple19*_tmp47F[2U];({struct _tuple19*_tmp9D8=({struct _tuple19*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->f1=0,_tmp480->f2=field_tag_exp;_tmp480;});_tmp47F[0]=_tmp9D8;}),({struct _tuple19*_tmp9D7=({struct _tuple19*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->f1=0,_tmp481->f2=fieldexp;_tmp481;});_tmp47F[1]=_tmp9D7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp47F,sizeof(struct _tuple19*),2U));});struct Cyc_List_List*newdles=_tmp47D;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp9DA=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp47E[1U];({struct _tuple19*_tmp9D9=Cyc_Toc_make_field(field->name,umem);_tmp47E[0]=_tmp9D9;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp47E,sizeof(struct _tuple19*),1U));}));e->r=_tmp9DA;});}}}else{
# 2689
struct Cyc_List_List*_tmp482=0;struct Cyc_List_List*newdles=_tmp482;
struct Cyc_List_List*_tmp483=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp483;
for(0;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp484=fields;struct Cyc_List_List*fields2=_tmp484;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_tmp485=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp4C=_tmp485;struct _tuple36*_tmp486=_stmttmp4C;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Aggrfield*_tmp487;_LL119: _tmp487=_tmp486->f1;_tmp488=_tmp486->f2;_LL11A: {struct Cyc_Absyn_Aggrfield*field=_tmp487;struct Cyc_Absyn_Exp*fieldexp=_tmp488;
void*_tmp489=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp489;
void*_tmp48A=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp48A;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2699
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2701
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp9DB=fieldtyp;Cyc_Toc_cast_it(_tmp9DB,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp48C=_cycalloc(sizeof(*_tmp48C));({struct _tuple19*_tmp9DC=({struct _tuple19*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->f1=0,_tmp48B->f2=fieldexp;_tmp48B;});_tmp48C->hd=_tmp9DC;}),_tmp48C->tl=newdles;_tmp48C;});
break;}}}}
# 2708
({void*_tmp9DD=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(newdles));e->r=_tmp9DD;});}}
# 2711
goto _LL7;}}}}case 31U: _LL4C: _tmp30A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp30B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_tmp30C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_LL4D: {struct Cyc_List_List*es=_tmp30A;struct Cyc_Absyn_Datatypedecl*tud=_tmp30B;struct Cyc_Absyn_Datatypefield*tuf=_tmp30C;
# 2715
struct Cyc_List_List*_tmp48D=tuf->typs;struct Cyc_List_List*tqts=_tmp48D;
{struct Cyc_List_List*_tmp48E=es;struct Cyc_List_List*es2=_tmp48E;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp48F=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp48F;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp9DE=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp9DE;});}}{
# 2726
struct Cyc_Absyn_Exp*_tmp490=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2726
struct Cyc_Absyn_Exp*tag_exp=_tmp490;
# 2729
if(!Cyc_Toc_is_toplevel(nv)){
# 2731
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));({struct _tuple19*_tmp9E0=({struct _fat_ptr*_tmp9DF=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9DF,(struct Cyc_Absyn_Exp*)es->hd);});_tmp491->hd=_tmp9E0;}),_tmp491->tl=dles;_tmp491;});}}{
# 2737
struct _tuple19*_tmp492=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp492;
({void*_tmp9E4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=29U,({struct _tuple1*_tmp9E3=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp494->f1=_tmp9E3;}),_tmp494->f2=0,({
struct Cyc_List_List*_tmp9E2=({struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->hd=tag_dle,({struct Cyc_List_List*_tmp9E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp493->tl=_tmp9E1;});_tmp493;});_tmp494->f3=_tmp9E2;}),_tmp494->f4=0;_tmp494;});
# 2738
e->r=_tmp9E4;});}}else{
# 2743
struct Cyc_List_List*_tmp495=0;struct Cyc_List_List*dles=_tmp495;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp497=_cycalloc(sizeof(*_tmp497));({struct _tuple19*_tmp9E5=({struct _tuple19*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->f1=0,_tmp496->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp496;});_tmp497->hd=_tmp9E5;}),_tmp497->tl=dles;_tmp497;});}
({void*_tmp9E8=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp499=_cycalloc(sizeof(*_tmp499));({struct _tuple19*_tmp9E7=({struct _tuple19*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->f1=0,_tmp498->f2=tag_exp;_tmp498;});_tmp499->hd=_tmp9E7;}),({
struct Cyc_List_List*_tmp9E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp499->tl=_tmp9E6;});_tmp499;}));
# 2746
e->r=_tmp9E8;});}
# 2749
goto _LL7;}}case 32U: _LL4E: _LL4F:
# 2751
 goto _LL51;case 33U: _LL50: _LL51:
 goto _LL7;case 34U: _LL52: _tmp304=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).is_calloc;_tmp305=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).rgn;_tmp306=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).elt_type;_tmp307=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).num_elts;_tmp308=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).fat_result;_tmp309=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1).inline_call;_LL53: {int is_calloc=_tmp304;struct Cyc_Absyn_Exp*rgnopt=_tmp305;void**topt=_tmp306;struct Cyc_Absyn_Exp*num_elts=_tmp307;int is_fat=_tmp308;int inline_call=_tmp309;
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
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp49A[3U];_tmp49A[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp9E9=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp49A[1]=_tmp9E9;}),_tmp49A[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9EA=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp9EA,_tag_fat(_tmp49A,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2770
pexp=({void*_tmp9EC=*topt;struct Cyc_Absyn_Exp*_tmp9EB=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9EC,_tmp9EB,num_elts2);});}}else{
# 2772
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
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
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp49B[3U];_tmp49B[0]=pexp,_tmp49B[1]=elt_sz,_tmp49B[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp9ED,_tag_fat(_tmp49B,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp9F0=(struct _tuple1*)_check_null(x);void*_tmp9EF=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9EE=num_elts;Cyc_Absyn_declare_stmt(_tmp9F0,_tmp9EF,_tmp9EE,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp9F1=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9F1;});}else{
# 2788
e->r=pexp->r;}
goto _LL7;}}case 35U: _LL54: _tmp302=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp303=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp302;struct Cyc_Absyn_Exp*e2=_tmp303;
# 2798
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E7;_tmp7E7.tag=0U,({struct _fat_ptr _tmp9F2=({const char*_tmp49E="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp49E,sizeof(char),57U);});_tmp7E7.f1=_tmp9F2;});_tmp7E7;});void*_tmp49C[1U];_tmp49C[0]=& _tmp49D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp49C,sizeof(void*),1U));});{
# 2806
unsigned _tmp49F=e->loc;unsigned loc=_tmp49F;
struct _tuple1*_tmp4A0=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp4A0;
struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp4A1;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp4A2=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp4A2;
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp4A3;v2e->topt=e2_old_typ;{
# 2812
struct Cyc_Absyn_Exp*_tmp4A4=({struct Cyc_Absyn_Exp*_tmp9F4=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp9F3=v2e;Cyc_Absyn_assign_exp(_tmp9F4,_tmp9F3,loc);});struct Cyc_Absyn_Exp*s1e=_tmp4A4;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A5=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp4A5;
struct Cyc_Absyn_Exp*_tmp4A6=({struct Cyc_Absyn_Exp*_tmp9F6=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp9F5=v1e;Cyc_Absyn_assign_exp(_tmp9F6,_tmp9F5,loc);});struct Cyc_Absyn_Exp*s2e=_tmp4A6;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A7=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp4A7;
# 2817
struct Cyc_Absyn_Stmt*_tmp4A8=({struct _tuple1*_tmp9FE=v1;void*_tmp9FD=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9FC=e1;struct Cyc_Absyn_Stmt*_tmp9FB=({
struct _tuple1*_tmp9FA=v2;void*_tmp9F9=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9F8=e2;struct Cyc_Absyn_Stmt*_tmp9F7=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2818
Cyc_Absyn_declare_stmt(_tmp9FA,_tmp9F9,_tmp9F8,_tmp9F7,loc);});
# 2817
Cyc_Absyn_declare_stmt(_tmp9FE,_tmp9FD,_tmp9FC,_tmp9FB,loc);});struct Cyc_Absyn_Stmt*s=_tmp4A8;
# 2820
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmp9FF=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9FF;});
goto _LL7;}}}}}}case 38U: _LL56: _tmp300=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_tmp301=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2FC)->f2;_LL57: {struct Cyc_Absyn_Exp*e1=_tmp300;struct _fat_ptr*f=_tmp301;
# 2825
void*_tmp4A9=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp4A9;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp4AA=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp4AB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AA)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AA)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AA)->f1)->f1).KnownAggr).tag == 2){_LL11C: _tmp4AB=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AA)->f1)->f1).KnownAggr).val;_LL11D: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4AB;
# 2829
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp4AC;
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp4AD;
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp4AE;
({void*_tmpA00=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmpA00;});
goto _LL11B;}}else{goto _LL11E;}}else{goto _LL11E;}}else{_LL11E: _LL11F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E8;_tmp7E8.tag=0U,({struct _fat_ptr _tmpA01=({const char*_tmp4B1="non-aggregate type in tagcheck";_tag_fat(_tmp4B1,sizeof(char),31U);});_tmp7E8.f1=_tmpA01;});_tmp7E8;});void*_tmp4AF[1U];_tmp4AF[0]=& _tmp4B0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4AF,sizeof(void*),1U));});}_LL11B:;}
# 2836
goto _LL7;}case 37U: _LL58: _tmp2FF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2FC)->f1;_LL59: {struct Cyc_Absyn_Stmt*s=_tmp2FF;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36U: _LL5A: _LL5B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E9;_tmp7E9.tag=0U,({struct _fat_ptr _tmpA02=({const char*_tmp4B4="UnresolvedMem";_tag_fat(_tmp4B4,sizeof(char),14U);});_tmp7E9.f1=_tmpA02;});_tmp7E9;});void*_tmp4B2[1U];_tmp4B2[0]=& _tmp4B3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B2,sizeof(void*),1U));});case 25U: _LL5C: _LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EA;_tmp7EA.tag=0U,({struct _fat_ptr _tmpA03=({const char*_tmp4B7="compoundlit";_tag_fat(_tmp4B7,sizeof(char),12U);});_tmp7EA.f1=_tmpA03;});_tmp7EA;});void*_tmp4B5[1U];_tmp4B5[0]=& _tmp4B6;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B5,sizeof(void*),1U));});case 39U: _LL5E: _LL5F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EB;_tmp7EB.tag=0U,({struct _fat_ptr _tmpA04=({const char*_tmp4BA="valueof(-)";_tag_fat(_tmp4BA,sizeof(char),11U);});_tmp7EB.f1=_tmpA04;});_tmp7EB;});void*_tmp4B8[1U];_tmp4B8[0]=& _tmp4B9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B8,sizeof(void*),1U));});default: _LL60: _tmp2FD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2FC)->f3;_tmp2FE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2FC)->f4;_LL61: {struct Cyc_List_List*o=_tmp2FD;struct Cyc_List_List*i=_tmp2FE;
# 2842
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2846
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7ED;_tmp7ED.tag=0U,({struct _fat_ptr _tmpA05=({const char*_tmp4BE="Toc did not examine an inserted check: ";_tag_fat(_tmp4BE,sizeof(char),40U);});_tmp7ED.f1=_tmpA05;});_tmp7ED;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4BD=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7EC;_tmp7EC.tag=4U,_tmp7EC.f1=e;_tmp7EC;});void*_tmp4BB[2U];_tmp4BB[0]=& _tmp4BC,_tmp4BB[1]=& _tmp4BD;({unsigned _tmpA06=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmpA06,_tag_fat(_tmp4BB,sizeof(void*),2U));});});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2875 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2877
return({struct _tuple37*_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF));_tmp4BF->f1=0,({struct _fat_ptr*_tmpA08=Cyc_Toc_fresh_label();_tmp4BF->f2=_tmpA08;}),({struct _fat_ptr*_tmpA07=Cyc_Toc_fresh_label();_tmp4BF->f3=_tmpA07;}),_tmp4BF->f4=sc;_tmp4BF;});}
# 2883
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4C0=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp4C0;
# 2889
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4C3=p->orig_pat;if((_tmp4C3.pattern).tag != 1)_throw_match();(_tmp4C3.pattern).val;})->topt);
void*_tmp4C1=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp4C1;
void*_tmp4C2=t2;switch(*((int*)_tmp4C2)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f1)){case 0U: _LL1: _LL2:
# 2894
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmpA09=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA09,v);});goto _LL0;}_LL0:;}{
# 2900
void*_tmp4C4=p->access;void*_stmttmp4D=_tmp4C4;void*_tmp4C5=_stmttmp4D;struct _fat_ptr*_tmp4C7;int _tmp4C6;unsigned _tmp4C8;unsigned _tmp4C9;switch(*((int*)_tmp4C5)){case 0U: _LLA: _LLB:
# 2904
 goto _LL9;case 1U: _LLC: _LLD:
# 2909
 if(ps->tl != 0){
void*_tmp4CA=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4E=_tmp4CA;void*_tmp4CB=_stmttmp4E;unsigned _tmp4CE;struct Cyc_Absyn_Datatypefield*_tmp4CD;struct Cyc_Absyn_Datatypedecl*_tmp4CC;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CB)->tag == 3U){_LL15: _tmp4CC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CB)->f1;_tmp4CD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CB)->f2;_tmp4CE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CB)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4CC;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CD;unsigned i=_tmp4CE;
# 2912
ps=ps->tl;
v=({void*_tmpA0B=({void*_tmpA0A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA0A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA0B,v);});
v=({struct Cyc_Absyn_Exp*_tmpA0C=v;Cyc_Absyn_aggrarrow_exp(_tmpA0C,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2919
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp4C9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C5)->f3;_LLF: {unsigned i=_tmp4C9;
_tmp4C8=i;goto _LL11;}case 2U: _LL10: _tmp4C8=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4C5)->f1;_LL11: {unsigned i=_tmp4C8;
v=({struct Cyc_Absyn_Exp*_tmpA0D=v;Cyc_Toc_member_exp(_tmpA0D,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _LL12: _tmp4C6=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C5)->f1;_tmp4C7=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C5)->f2;_LL13: {int tagged=_tmp4C6;struct _fat_ptr*f=_tmp4C7;
# 2924
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2930
return v;}
# 2935
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4CF=t;struct Cyc_Absyn_Datatypefield*_tmp4D1;struct Cyc_Absyn_Datatypedecl*_tmp4D0;int _tmp4D3;struct _fat_ptr*_tmp4D2;struct Cyc_Absyn_Datatypefield*_tmp4D6;struct Cyc_Absyn_Datatypedecl*_tmp4D5;int _tmp4D4;unsigned _tmp4D7;int _tmp4D9;struct _fat_ptr _tmp4D8;struct Cyc_Absyn_Enumfield*_tmp4DB;void*_tmp4DA;struct Cyc_Absyn_Enumfield*_tmp4DD;struct Cyc_Absyn_Enumdecl*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DE;switch(*((int*)_tmp4CF)){case 0U: _LL1: _tmp4DE=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4CF)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp4DE;
return e == 0?v: e;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA0E=v;Cyc_Absyn_eq_exp(_tmpA0E,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA0F=v;Cyc_Absyn_neq_exp(_tmpA0F,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4DC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CF)->f1;_tmp4DD=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CF)->f2;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp4DC;struct Cyc_Absyn_Enumfield*ef=_tmp4DD;
return({struct Cyc_Absyn_Exp*_tmpA10=v;Cyc_Absyn_eq_exp(_tmpA10,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->tag=32U,_tmp4DF->f1=ed,_tmp4DF->f2=ef;_tmp4DF;}),0U),0U);});}case 4U: _LL9: _tmp4DA=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CF)->f1;_tmp4DB=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CF)->f2;_LLA: {void*t=_tmp4DA;struct Cyc_Absyn_Enumfield*ef=_tmp4DB;
return({struct Cyc_Absyn_Exp*_tmpA11=v;Cyc_Absyn_eq_exp(_tmpA11,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->tag=33U,_tmp4E0->f1=t,_tmp4E0->f2=ef;_tmp4E0;}),0U),0U);});}case 5U: _LLB: _tmp4D8=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4CF)->f1;_tmp4D9=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4CF)->f2;_LLC: {struct _fat_ptr s=_tmp4D8;int i=_tmp4D9;
return({struct Cyc_Absyn_Exp*_tmpA12=v;Cyc_Absyn_eq_exp(_tmpA12,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6U: _LLD: _tmp4D7=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4CF)->f1;_LLE: {unsigned i=_tmp4D7;
return({struct Cyc_Absyn_Exp*_tmpA13=v;Cyc_Absyn_eq_exp(_tmpA13,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7U: _LLF: _tmp4D4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CF)->f1;_tmp4D5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CF)->f2;_tmp4D6=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CF)->f3;_LL10: {int i=_tmp4D4;struct Cyc_Absyn_Datatypedecl*tud=_tmp4D5;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D6;
# 2947
LOOP1: {
void*_tmp4E1=v->r;void*_stmttmp4F=_tmp4E1;void*_tmp4E2=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;switch(*((int*)_tmp4E2)){case 14U: _LL16: _tmp4E4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E2)->f2;_LL17: {struct Cyc_Absyn_Exp*e=_tmp4E4;
v=e;goto LOOP1;}case 20U: _LL18: _tmp4E3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E2)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp4E3;
v=e;goto _LL15;}default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2954
v=({void*_tmpA15=({void*_tmpA14=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA14,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA15,v);});return({struct Cyc_Absyn_Exp*_tmpA16=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA16,Cyc_Absyn_uint_exp((unsigned)i,0U),0U);});}case 8U: _LL11: _tmp4D2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CF)->f1;_tmp4D3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CF)->f2;_LL12: {struct _fat_ptr*f=_tmp4D2;int i=_tmp4D3;
# 2958
return({struct Cyc_Absyn_Exp*_tmpA18=({struct Cyc_Absyn_Exp*_tmpA17=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmpA17,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmpA18,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _LL13: _tmp4D0=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4CF)->f1;_tmp4D1=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4CF)->f2;_LL14: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4D0;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D1;
# 2962
LOOP2: {
void*_tmp4E5=v->r;void*_stmttmp50=_tmp4E5;void*_tmp4E6=_stmttmp50;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;switch(*((int*)_tmp4E6)){case 14U: _LL1D: _tmp4E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E6)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4E8;
v=e;goto LOOP2;}case 20U: _LL1F: _tmp4E7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E6)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4E7;
v=e;goto _LL1C;}default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2969
v=({void*_tmpA1A=({void*_tmpA19=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA19,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA1A,v);});
return({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA1B,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2981
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple38{int f1;void*f2;};
# 2988
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2990
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _tmp4E9=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp51=_tmp4E9;struct _tuple34 _tmp4EA=_stmttmp51;void*_tmp4EB;_LL1: _tmp4EB=_tmp4EA.f1;_LL2: {void*ptest=_tmp4EB;
void*_tmp4EC=ptest;struct Cyc_Absyn_Exp*_tmp4ED;struct _fat_ptr*_tmp4EE;switch(*((int*)_tmp4EC)){case 3U: _LL4: _LL5:
# 2996
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4EE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4EC)->f1;_LLB: {struct _fat_ptr*f=_tmp4EE;
# 3000
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->tag=3U,_tmp4EF->f1=f;_tmp4EF;});
continue;}case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4ED=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4EC)->f1;if(_tmp4ED != 0){_LLF: {struct Cyc_Absyn_Exp*e=_tmp4ED;
# 3005
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->tag=2U,_tmp4F0->f1=e;_tmp4F0;});return({struct _tuple38 _tmp7EE;_tmp7EE.f1=0,_tmp7EE.f2=k;_tmp7EE;});}}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple38 _tmp7EF;_tmp7EF.f1=0,_tmp7EF.f2=k;_tmp7EF;});}_LL3:;}}
# 3013
return({struct _tuple38 _tmp7F0;_tmp7F0.f1=c,_tmp7F0.f2=k;_tmp7F0;});}
# 3018
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4F1=p;int _tmp4F2;int _tmp4F3;unsigned _tmp4F4;struct Cyc_Absyn_Enumfield*_tmp4F6;void*_tmp4F5;struct Cyc_Absyn_Enumfield*_tmp4F8;struct Cyc_Absyn_Enumdecl*_tmp4F7;switch(*((int*)_tmp4F1)){case 3U: _LL1: _tmp4F7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F1)->f1;_tmp4F8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F1)->f2;_LL2: {struct Cyc_Absyn_Enumdecl*ed=_tmp4F7;struct Cyc_Absyn_Enumfield*ef=_tmp4F8;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->tag=32U,_tmp4F9->f1=ed,_tmp4F9->f2=ef;_tmp4F9;}),0U);goto _LL0;}case 4U: _LL3: _tmp4F5=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F1)->f1;_tmp4F6=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F1)->f2;_LL4: {void*t=_tmp4F5;struct Cyc_Absyn_Enumfield*ef=_tmp4F6;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->tag=33U,_tmp4FA->f1=t,_tmp4FA->f2=ef;_tmp4FA;}),0U);goto _LL0;}case 6U: _LL5: _tmp4F4=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4F1)->f1;_LL6: {unsigned i=_tmp4F4;
_tmp4F3=(int)i;goto _LL8;}case 7U: _LL7: _tmp4F3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4F1)->f1;_LL8: {int i=_tmp4F3;
_tmp4F2=i;goto _LLA;}case 8U: _LL9: _tmp4F2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4F1)->f2;_LLA: {int i=_tmp4F2;
e=Cyc_Absyn_uint_exp((unsigned)i,0U);goto _LL0;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F1;_tmp7F1.tag=0U,({struct _fat_ptr _tmpA1C=({const char*_tmp4FD="compile_pat_test_as_case!";_tag_fat(_tmp4FD,sizeof(char),26U);});_tmp7F1.f1=_tmpA1C;});_tmp7F1;});void*_tmp4FB[1U];_tmp4FB[0]=& _tmp4FC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4FB,sizeof(void*),1U));});}_LL0:;}
# 3028
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->tag=17U,_tmp4FE->f1=e;_tmp4FE;}),0U);}
# 3032
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple40{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3043
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3047
void*t=(void*)_check_null(p->topt);
void*_tmp4FF=p->r;void*_stmttmp52=_tmp4FF;void*_tmp500=_stmttmp52;struct Cyc_List_List*_tmp502;union Cyc_Absyn_AggrInfo _tmp501;struct Cyc_List_List*_tmp503;struct Cyc_List_List*_tmp504;struct Cyc_Absyn_Pat*_tmp505;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Datatypefield*_tmp507;struct Cyc_Absyn_Datatypedecl*_tmp506;struct Cyc_Absyn_Pat*_tmp50A;struct Cyc_Absyn_Vardecl*_tmp509;struct Cyc_Absyn_Vardecl*_tmp50B;struct Cyc_Absyn_Pat*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50E;switch(*((int*)_tmp500)){case 2U: _LL1: _tmp50E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp500)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp50E;
# 3050
struct Cyc_Absyn_Pat*_tmp50F=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp50F;
p2->topt=p->topt;
_tmp50C=vd;_tmp50D=p2;goto _LL4;}case 1U: _LL3: _tmp50C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp500)->f1;_tmp50D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp500)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp50C;struct Cyc_Absyn_Pat*p2=_tmp50D;
# 3055
struct Cyc_Absyn_Vardecl*_tmp510=({struct _tuple1*_tmpA1D=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA1D,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp510;
({struct Cyc_List_List*_tmpA1F=({struct Cyc_List_List*_tmp512=_region_malloc(rgn,sizeof(*_tmp512));({struct _tuple39*_tmpA1E=({struct _tuple39*_tmp511=_region_malloc(rgn,sizeof(*_tmp511));_tmp511->f1=vd,_tmp511->f2=new_vd;_tmp511;});_tmp512->hd=_tmpA1E;}),_tmp512->tl=*decls;_tmp512;});*decls=_tmpA1F;});{
struct Cyc_Absyn_Stmt*_tmp513=({struct Cyc_Absyn_Exp*_tmpA20=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->tag=4U,_tmp514->f1=new_vd;_tmp514;}),0U);Cyc_Absyn_assign_stmt(_tmpA20,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp513;
return({struct Cyc_Absyn_Stmt*_tmpA21=s;Cyc_Toc_seq_stmt_opt(_tmpA21,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4U: _LL5: _tmp50B=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp500)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp50B;
# 3061
struct Cyc_Absyn_Vardecl*_tmp515=({struct _tuple1*_tmpA22=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA22,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp515;
({struct Cyc_List_List*_tmpA24=({struct Cyc_List_List*_tmp517=_region_malloc(rgn,sizeof(*_tmp517));({struct _tuple39*_tmpA23=({struct _tuple39*_tmp516=_region_malloc(rgn,sizeof(*_tmp516));_tmp516->f1=vd,_tmp516->f2=new_vd;_tmp516;});_tmp517->hd=_tmpA23;}),_tmp517->tl=*decls;_tmp517;});*decls=_tmpA24;});
return({struct Cyc_Absyn_Exp*_tmpA25=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->tag=4U,_tmp518->f1=new_vd;_tmp518;}),0U);Cyc_Absyn_assign_stmt(_tmpA25,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp509=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp500)->f1;_tmp50A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp500)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp509;struct Cyc_Absyn_Pat*p2=_tmp50A;
# 3067
({void*_tmpA26=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmpA26;});{
struct Cyc_Absyn_Vardecl*_tmp519=({struct _tuple1*_tmpA27=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA27,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp519;
({struct Cyc_List_List*_tmpA29=({struct Cyc_List_List*_tmp51B=_region_malloc(rgn,sizeof(*_tmp51B));({struct _tuple39*_tmpA28=({struct _tuple39*_tmp51A=_region_malloc(rgn,sizeof(*_tmp51A));_tmp51A->f1=vd,_tmp51A->f2=new_vd;_tmp51A;});_tmp51B->hd=_tmpA28;}),_tmp51B->tl=*decls;_tmp51B;});*decls=_tmpA29;});{
# 3071
struct Cyc_Absyn_Stmt*_tmp51C=({struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->tag=4U,_tmp51D->f1=new_vd;_tmp51D;}),0U);Cyc_Absyn_assign_stmt(_tmpA2C,({
void*_tmpA2B=({void*_tmpA2A=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA2A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA2B,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3071
struct Cyc_Absyn_Stmt*s=_tmp51C;
# 3074
return({struct Cyc_Absyn_Stmt*_tmpA2D=s;Cyc_Toc_seq_stmt_opt(_tmpA2D,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9U: _LLB: _LLC:
# 3076
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp500)->f1)->r)->tag == 8U){_LL17: _tmp506=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp500)->f1)->r)->f1;_tmp507=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp500)->f1)->r)->f2;_tmp508=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp500)->f1)->r)->f3;_LL18: {struct Cyc_Absyn_Datatypedecl*tud=_tmp506;struct Cyc_Absyn_Datatypefield*tuf=_tmp507;struct Cyc_List_List*ps=_tmp508;
# 3085
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp51E=({void*_tmpA2E=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA2E,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp51E;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp51F=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp51F;
struct Cyc_Absyn_Stmt*s=0;
for(0;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp520=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp520;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp521=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp521;
void*_tmp522=(void*)_check_null(p2->topt);void*t2=_tmp522;
void*_tmp523=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp523;
struct Cyc_Absyn_Exp*_tmp524=({struct Cyc_Absyn_Exp*_tmpA2F=path;Cyc_Absyn_aggrarrow_exp(_tmpA2F,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp524;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmpA30=s;Cyc_Toc_seq_stmt_opt(_tmpA30,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3103
return s;}}}}else{_LL21: _tmp505=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp500)->f1;_LL22: {struct Cyc_Absyn_Pat*p2=_tmp505;
# 3148
return({struct _RegionHandle*_tmpA34=rgn;struct Cyc_Toc_Env**_tmpA33=nv;struct Cyc_List_List**_tmpA32=decls;struct Cyc_Absyn_Exp*_tmpA31=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA34,_tmpA33,_tmpA32,_tmpA31,p2);});}}case 8U: _LL19: _tmp504=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp500)->f3;_LL1A: {struct Cyc_List_List*ps=_tmp504;
# 3105
_tmp503=ps;goto _LL1C;}case 5U: _LL1B: _tmp503=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp500)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp503;
# 3107
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp525=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp525;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp526=(void*)_check_null(p2->topt);void*t2=_tmp526;
struct _fat_ptr*_tmp527=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp527;
s=({struct Cyc_Absyn_Stmt*_tmpA39=s;Cyc_Toc_seq_stmt_opt(_tmpA39,({struct _RegionHandle*_tmpA38=rgn;struct Cyc_Toc_Env**_tmpA37=nv;struct Cyc_List_List**_tmpA36=decls;struct Cyc_Absyn_Exp*_tmpA35=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmpA38,_tmpA37,_tmpA36,_tmpA35,p2);}));});}}
# 3117
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp500)->f1 == 0){_LL1D: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp529=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F2;_tmp7F2.tag=0U,({struct _fat_ptr _tmpA3A=({const char*_tmp52A="unresolved aggregate pattern!";_tag_fat(_tmp52A,sizeof(char),30U);});_tmp7F2.f1=_tmpA3A;});_tmp7F2;});void*_tmp528[1U];_tmp528[0]=& _tmp529;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp528,sizeof(void*),1U));});}else{_LL1F: _tmp501=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp500)->f1;_tmp502=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp500)->f3;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp501;struct Cyc_List_List*dlps=_tmp502;
# 3121
struct Cyc_Absyn_Aggrdecl*_tmp52B=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp52B;
struct Cyc_Absyn_Stmt*s=0;
for(0;dlps != 0;dlps=dlps->tl){
struct _tuple40*_tmp52C=(struct _tuple40*)dlps->hd;struct _tuple40*tup=_tmp52C;
struct Cyc_Absyn_Pat*_tmp52D=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp52D;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp52E=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp52E;
void*_tmp52F=(void*)_check_null(p2->topt);void*t2=_tmp52F;
void*_tmp530=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp530;
struct Cyc_Absyn_Exp*_tmp531=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp531;
# 3133
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp532=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp532;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(ftype)))
# 3140
memexp=Cyc_Absyn_deref_exp(({void*_tmpA3B=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA3B,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3143
s=({struct Cyc_Absyn_Stmt*_tmpA3C=s;Cyc_Toc_seq_stmt_opt(_tmpA3C,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3145
return s;}}case 15U: _LL23: _LL24:
# 3150
({struct Cyc_Warn_String_Warn_Warg_struct _tmp534=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F3;_tmp7F3.tag=0U,({struct _fat_ptr _tmpA3D=({const char*_tmp535="unknownid pat";_tag_fat(_tmp535,sizeof(char),14U);});_tmp7F3.f1=_tmpA3D;});_tmp7F3;});void*_tmp533[1U];_tmp533[0]=& _tmp534;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp533,sizeof(void*),1U));});case 16U: _LL25: _LL26:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp537=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F4;_tmp7F4.tag=0U,({struct _fat_ptr _tmpA3E=({const char*_tmp538="unknowncall pat";_tag_fat(_tmp538,sizeof(char),16U);});_tmp7F4.f1=_tmpA3E;});_tmp7F4;});void*_tmp536[1U];_tmp536[0]=& _tmp537;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp536,sizeof(void*),1U));});default: _LL27: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F5;_tmp7F5.tag=0U,({struct _fat_ptr _tmpA3F=({const char*_tmp53B="exp pat";_tag_fat(_tmp53B,sizeof(char),8U);});_tmp7F5.f1=_tmpA3F;});_tmp7F5;});void*_tmp539[1U];_tmp539[0]=& _tmp53A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp539,sizeof(void*),1U));});}_LL0:;}
# 3156
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple39 _tmp53C=*((struct _tuple39*)vs->hd);struct _tuple39 _stmttmp53=_tmp53C;struct _tuple39 _tmp53D=_stmttmp53;struct Cyc_Absyn_Vardecl*_tmp53F;struct Cyc_Absyn_Vardecl*_tmp53E;_LL1: _tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;_LL2: {struct Cyc_Absyn_Vardecl*oldv=_tmp53E;struct Cyc_Absyn_Vardecl*newv=_tmp53F;
if(oldv == newv)return newv;}}
# 3161
({void*_tmp540=0U;({struct _fat_ptr _tmpA40=({const char*_tmp541="find_new_var";_tag_fat(_tmp541,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA40,_tag_fat(_tmp540,sizeof(void*),0U));});});}
# 3167
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp542=e->r;void*_stmttmp54=_tmp542;void*_tmp543=_stmttmp54;struct Cyc_List_List*_tmp544;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp546;struct Cyc_List_List*_tmp547;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp565;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_Absyn_Vardecl*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56A;switch(*((int*)_tmp543)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543)->f1)){case 5U: _LL1: _tmp56A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp56A;
_tmp569=vd;goto _LL4;}case 4U: _LL3: _tmp569=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp569;
# 3171
{struct _handler_cons _tmp56B;_push_handler(& _tmp56B);{int _tmp56D=0;if(setjmp(_tmp56B.handler))_tmp56D=1;if(!_tmp56D){
({void*_tmpA43=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=1U,({void*_tmpA42=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->tag=4U,({struct Cyc_Absyn_Vardecl*_tmpA41=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,vd);_tmp56E->f1=_tmpA41;});_tmp56E;});_tmp56F->f1=_tmpA42;});_tmp56F;});e->r=_tmpA43;});;_pop_handler();}else{void*_tmp56C=(void*)Cyc_Core_get_exn_thrown();void*_tmp570=_tmp56C;void*_tmp571;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp570)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3174
 goto _LL3F;}else{_LL42: _tmp571=_tmp570;_LL43: {void*exn=_tmp571;(int)_rethrow(exn);}}_LL3F:;}}}
# 3176
goto _LL0;}default: goto _LL3D;}case 6U: _LL5: _tmp566=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp567=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp568=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp566;struct Cyc_Absyn_Exp*e2=_tmp567;struct Cyc_Absyn_Exp*e3=_tmp568;
# 3178
Cyc_Toc_subst_pattern_vars(env,e1);_tmp564=e2;_tmp565=e3;goto _LL8;}case 27U: _LL7: _tmp564=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp565=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp564;struct Cyc_Absyn_Exp*e2=_tmp565;
_tmp562=e1;_tmp563=e2;goto _LLA;}case 7U: _LL9: _tmp562=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp563=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp562;struct Cyc_Absyn_Exp*e2=_tmp563;
_tmp560=e1;_tmp561=e2;goto _LLC;}case 8U: _LLB: _tmp560=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp561=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp560;struct Cyc_Absyn_Exp*e2=_tmp561;
_tmp55E=e1;_tmp55F=e2;goto _LLE;}case 23U: _LLD: _tmp55E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp55F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp55E;struct Cyc_Absyn_Exp*e2=_tmp55F;
_tmp55C=e1;_tmp55D=e2;goto _LL10;}case 9U: _LLF: _tmp55C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp55D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp55C;struct Cyc_Absyn_Exp*e2=_tmp55D;
# 3184
Cyc_Toc_subst_pattern_vars(env,e1);_tmp55B=e2;goto _LL12;}case 41U: _LL11: _tmp55B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp55B;
_tmp55A=e;goto _LL14;}case 38U: _LL13: _tmp55A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp55A;
_tmp559=e;goto _LL16;}case 11U: _LL15: _tmp559=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp559;
_tmp558=e;goto _LL18;}case 12U: _LL17: _tmp558=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp558;
_tmp557=e;goto _LL1A;}case 13U: _LL19: _tmp557=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp557;
_tmp556=e;goto _LL1C;}case 14U: _LL1B: _tmp556=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp556;
_tmp555=e;goto _LL1E;}case 18U: _LL1D: _tmp555=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp555;
_tmp554=e;goto _LL20;}case 20U: _LL1F: _tmp554=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp554;
_tmp553=e;goto _LL22;}case 21U: _LL21: _tmp553=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp553;
_tmp552=e;goto _LL24;}case 22U: _LL23: _tmp552=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp552;
_tmp551=e;goto _LL26;}case 28U: _LL25: _tmp551=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp551;
_tmp550=e;goto _LL28;}case 15U: _LL27: _tmp550=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp550;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34U: _LL29: _tmp54E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp543)->f1).rgn;_tmp54F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp543)->f1).num_elts;_LL2A: {struct Cyc_Absyn_Exp*eopt=_tmp54E;struct Cyc_Absyn_Exp*e=_tmp54F;
_tmp54C=eopt;_tmp54D=e;goto _LL2C;}case 16U: _LL2B: _tmp54C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp54D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp54C;struct Cyc_Absyn_Exp*e=_tmp54D;
# 3199
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3U: _LL2D: _tmp54B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL2E: {struct Cyc_List_List*es=_tmp54B;
_tmp54A=es;goto _LL30;}case 31U: _LL2F: _tmp54A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL30: {struct Cyc_List_List*es=_tmp54A;
_tmp549=es;goto _LL32;}case 24U: _LL31: _tmp549=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL32: {struct Cyc_List_List*es=_tmp549;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36U: _LL33: _tmp548=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL34: {struct Cyc_List_List*dles=_tmp548;
_tmp547=dles;goto _LL36;}case 29U: _LL35: _tmp547=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LL36: {struct Cyc_List_List*dles=_tmp547;
_tmp546=dles;goto _LL38;}case 30U: _LL37: _tmp546=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL38: {struct Cyc_List_List*dles=_tmp546;
_tmp545=dles;goto _LL3A;}case 26U: _LL39: _tmp545=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp545;
_tmp544=dles;goto _LL3C;}case 25U: _LL3B: _tmp544=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp544;
# 3209
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp572=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp55=_tmp572;struct _tuple19*_tmp573=_stmttmp55;struct Cyc_Absyn_Exp*_tmp574;_LL45: _tmp574=_tmp573->f2;_LL46: {struct Cyc_Absyn_Exp*e=_tmp574;
Cyc_Toc_subst_pattern_vars(env,e);}}
# 3213
goto _LL0;}default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple41{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3223
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3231
void*_tmp575=dopt;void*_tmp578;struct Cyc_List_List*_tmp577;struct Cyc_List_List*_tmp576;struct Cyc_Tcpat_Rhs*_tmp579;if(_tmp575 == 0){_LL1: _LL2:
# 3233
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp575)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp579=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp575)->f1;_LL6: {struct Cyc_Tcpat_Rhs*rhs=_tmp579;
# 3237
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp57A=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp56=_tmp57A;struct _tuple37*_tmp57B=_stmttmp56;struct Cyc_Absyn_Switch_clause*_tmp57F;struct _fat_ptr*_tmp57E;struct _fat_ptr*_tmp57D;int*_tmp57C;_LLA: _tmp57C=(int*)& _tmp57B->f1;_tmp57D=_tmp57B->f2;_tmp57E=_tmp57B->f3;_tmp57F=_tmp57B->f4;_LLB: {int*already_emitted=_tmp57C;struct _fat_ptr*init_lab=_tmp57D;struct _fat_ptr*code_lab=_tmp57E;struct Cyc_Absyn_Switch_clause*sc=_tmp57F;
struct Cyc_Absyn_Stmt*_tmp580=sc->body;struct Cyc_Absyn_Stmt*body=_tmp580;
if(body == rhs->rhs){
# 3242
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3247
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA45=rgn;struct Cyc_Absyn_Exp*_tmpA44=
Cyc_Absyn_var_exp(v,0U);
# 3247
Cyc_Toc_extract_pattern_vars(_tmpA45,& nv,& newdecls,_tmpA44,sc->pattern);});
# 3251
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp581=newdecls;struct Cyc_List_List*ds=_tmp581;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp582=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp57=_tmp582;struct _tuple39 _tmp583=_stmttmp57;struct Cyc_Absyn_Vardecl*_tmp585;struct Cyc_Absyn_Vardecl*_tmp584;_LLD: _tmp584=_tmp583.f1;_tmp585=_tmp583.f2;_LLE: {struct Cyc_Absyn_Vardecl*oldv=_tmp584;struct Cyc_Absyn_Vardecl*newv=_tmp585;
({struct Cyc_List_List*_tmpA47=({struct Cyc_List_List*_tmp587=_region_malloc(rgn,sizeof(*_tmp587));({struct _tuple39*_tmpA46=({struct _tuple39*_tmp586=_region_malloc(rgn,sizeof(*_tmp586));_tmp586->f1=oldv,_tmp586->f2=newv;_tmp586;});_tmp587->hd=_tmpA46;}),_tmp587->tl=*decls;_tmp587;});*decls=_tmpA47;});
({struct Cyc_Absyn_Exp*_tmpA48=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->tag=4U,_tmp588->f1=newv;_tmp588;}),0U);oldv->initializer=_tmpA48;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmpA4A=({struct Cyc_Absyn_Decl*_tmp58A=_cycalloc(sizeof(*_tmp58A));({void*_tmpA49=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->tag=0U,_tmp589->f1=oldv;_tmp589;});_tmp58A->r=_tmpA49;}),_tmp58A->loc=0U;_tmp58A;});Cyc_Absyn_decl_stmt(_tmpA4A,res,0U);});}}}
# 3260
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmpA4C=({struct Cyc_List_List*_tmp58C=_region_malloc(rgn,sizeof(*_tmp58C));({struct _tuple41*_tmpA4B=({struct _tuple41*_tmp58B=_region_malloc(rgn,sizeof(*_tmp58B));_tmp58B->f1=nv,_tmp58B->f2=code_lab,_tmp58B->f3=body;_tmp58B;});_tmp58C->hd=_tmpA4B;}),_tmp58C->tl=*bodies;_tmp58C;});*bodies=_tmpA4C;});
return res;}}}}
# 3268
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F6;_tmp7F6.tag=0U,({struct _fat_ptr _tmpA4D=({const char*_tmp58F="couldn't find rhs!";_tag_fat(_tmp58F,sizeof(char),19U);});_tmp7F6.f1=_tmpA4D;});_tmp7F6;});void*_tmp58D[1U];_tmp58D[0]=& _tmp58E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp58D,sizeof(void*),1U));});}default: _LL7: _tmp576=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp575)->f1;_tmp577=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp575)->f2;_tmp578=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp575)->f3;_LL8: {struct Cyc_List_List*symbolic_path=_tmp576;struct Cyc_List_List*switches=_tmp577;void*other_decision=_tmp578;
# 3271
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3273
struct Cyc_Absyn_Exp*_tmp590=({struct Cyc_List_List*_tmpA4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmpA4E,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp590;
struct Cyc_List_List*_tmp591=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp591;
# 3276
struct _tuple38 _tmp592=Cyc_Toc_admits_switch(ss);struct _tuple38 _stmttmp58=_tmp592;struct _tuple38 _tmp593=_stmttmp58;void*_tmp595;int _tmp594;_LL10: _tmp594=_tmp593.f1;_tmp595=_tmp593.f2;_LL11: {int allows_switch=_tmp594;void*test_kind=_tmp595;
if(allows_switch > 1){
# 3280
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));
({struct Cyc_Absyn_Switch_clause*_tmpA50=({struct Cyc_Absyn_Switch_clause*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));({struct Cyc_Absyn_Pat*_tmpA4F=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5A5->pattern=_tmpA4F;}),_tmp5A5->pat_vars=0,_tmp5A5->where_clause=0,_tmp5A5->body=res,_tmp5A5->loc=0U;_tmp5A5;});_tmp5A6->hd=_tmpA50;}),_tmp5A6->tl=0;_tmp5A6;});
# 3283
for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp596=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp59=_tmp596;struct _tuple34 _tmp597=_stmttmp59;void*_tmp599;void*_tmp598;_LL13: _tmp598=_tmp597.f1;_tmp599=_tmp597.f2;_LL14: {void*pat_test=_tmp598;void*dec_tree=_tmp599;
# 3286
struct Cyc_Absyn_Pat*_tmp59A=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp59A;
# 3288
struct Cyc_Absyn_Stmt*_tmp59B=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp59B;
# 3290
new_lscs=({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));({struct Cyc_Absyn_Switch_clause*_tmpA51=({struct Cyc_Absyn_Switch_clause*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->pattern=case_exp,_tmp59C->pat_vars=0,_tmp59C->where_clause=0,_tmp59C->body=s,_tmp59C->loc=0U;_tmp59C;});_tmp59D->hd=_tmpA51;}),_tmp59D->tl=new_lscs;_tmp59D;});}}
# 3292
{void*_tmp59E=test_kind;struct _fat_ptr*_tmp59F;switch(*((int*)_tmp59E)){case 1U: _LL16: _LL17:
# 3294
 LOOP1: {
void*_tmp5A0=p->r;void*_stmttmp5A=_tmp5A0;void*_tmp5A1=_stmttmp5A;struct Cyc_Absyn_Exp*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;switch(*((int*)_tmp5A1)){case 14U: _LL1F: _tmp5A3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A1)->f2;_LL20: {struct Cyc_Absyn_Exp*e=_tmp5A3;
p=e;goto LOOP1;}case 20U: _LL21: _tmp5A2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5A1)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5A2;
p=e;goto _LL1E;}default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3300
p=Cyc_Absyn_deref_exp(({void*_tmpA52=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA52,p);}),0U);goto _LL15;case 3U: _LL18: _tmp59F=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp59E)->f1;_LL19: {struct _fat_ptr*f=_tmp59F;
# 3302
p=({struct Cyc_Absyn_Exp*_tmpA53=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmpA53,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3306
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->tag=10U,_tmp5A4->f1=p,_tmp5A4->f2=new_lscs,_tmp5A4->f3=0;_tmp5A4;}),0U);}else{
# 3310
void*_tmp5A7=test_kind;struct Cyc_Absyn_Exp*_tmp5A8;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A7)->tag == 2U){_LL26: _tmp5A8=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A7)->f1;_LL27: {struct Cyc_Absyn_Exp*e=_tmp5A8;
# 3312
struct Cyc_List_List*_tmp5A9=ss;struct Cyc_Tcpat_Rhs*_tmp5AB;void*_tmp5AA;if(_tmp5A9 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)((struct Cyc_List_List*)_tmp5A9)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5A9)->tl == 0){_LL2B: _tmp5AA=((struct _tuple34*)_tmp5A9->hd)->f1;_tmp5AB=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_tmp5A9->hd)->f2)->f1;_LL2C: {void*pat_test=_tmp5AA;struct Cyc_Tcpat_Rhs*rhs=_tmp5AB;
# 3318
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp5AC=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp5B=_tmp5AC;struct _tuple37*_tmp5AD=_stmttmp5B;struct Cyc_Absyn_Switch_clause*_tmp5B1;struct _fat_ptr*_tmp5B0;struct _fat_ptr*_tmp5AF;int*_tmp5AE;_LL30: _tmp5AE=(int*)& _tmp5AD->f1;_tmp5AF=_tmp5AD->f2;_tmp5B0=_tmp5AD->f3;_tmp5B1=_tmp5AD->f4;_LL31: {int*already_emitted=_tmp5AE;struct _fat_ptr*init_lab=_tmp5AF;struct _fat_ptr*code_lab=_tmp5B0;struct Cyc_Absyn_Switch_clause*sc=_tmp5B1;
struct Cyc_Absyn_Stmt*_tmp5B2=sc->body;struct Cyc_Absyn_Stmt*body=_tmp5B2;
if(body == rhs->rhs){
# 3323
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3328
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA55=rgn;struct Cyc_Absyn_Exp*_tmpA54=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA55,& nv,& newdecls,_tmpA54,sc->pattern);});
# 3332
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp5B3=newdecls;struct Cyc_List_List*ds=_tmp5B3;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp5B4=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp5C=_tmp5B4;struct _tuple39 _tmp5B5=_stmttmp5C;struct Cyc_Absyn_Vardecl*_tmp5B7;struct Cyc_Absyn_Vardecl*_tmp5B6;_LL33: _tmp5B6=_tmp5B5.f1;_tmp5B7=_tmp5B5.f2;_LL34: {struct Cyc_Absyn_Vardecl*oldv=_tmp5B6;struct Cyc_Absyn_Vardecl*newv=_tmp5B7;
({struct Cyc_List_List*_tmpA57=({struct Cyc_List_List*_tmp5B9=_region_malloc(rgn,sizeof(*_tmp5B9));({struct _tuple39*_tmpA56=({struct _tuple39*_tmp5B8=_region_malloc(rgn,sizeof(*_tmp5B8));_tmp5B8->f1=oldv,_tmp5B8->f2=newv;_tmp5B8;});_tmp5B9->hd=_tmpA56;}),_tmp5B9->tl=*decls;_tmp5B9;});*decls=_tmpA57;});
({struct Cyc_Absyn_Exp*_tmpA58=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->tag=4U,_tmp5BA->f1=newv;_tmp5BA;}),0U);oldv->initializer=_tmpA58;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmpA5A=({struct Cyc_Absyn_Decl*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));({void*_tmpA59=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=0U,_tmp5BB->f1=oldv;_tmp5BB;});_tmp5BC->r=_tmpA59;}),_tmp5BC->loc=0U;_tmp5BC;});Cyc_Absyn_decl_stmt(_tmpA5A,r,0U);});}}}
# 3341
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3345
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmpA5C=({struct Cyc_List_List*_tmp5BE=_region_malloc(rgn,sizeof(*_tmp5BE));({struct _tuple41*_tmpA5B=({struct _tuple41*_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD));_tmp5BD->f1=nv,_tmp5BD->f2=code_lab,_tmp5BD->f3=body;_tmp5BD;});_tmp5BE->hd=_tmpA5B;}),_tmp5BE->tl=*bodies;_tmp5BE;});*bodies=_tmpA5C;});
return r;}}}}
# 3355
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F7;_tmp7F7.tag=0U,({struct _fat_ptr _tmpA5D=({const char*_tmp5C1="couldn't find rhs!";_tag_fat(_tmp5C1,sizeof(char),19U);});_tmp7F7.f1=_tmpA5D;});_tmp7F7;});void*_tmp5BF[1U];_tmp5BF[0]=& _tmp5C0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5BF,sizeof(void*),1U));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F8;_tmp7F8.tag=0U,({struct _fat_ptr _tmpA5E=({const char*_tmp5C4="bad where clause in match compiler";_tag_fat(_tmp5C4,sizeof(char),35U);});_tmp7F8.f1=_tmpA5E;});_tmp7F8;});void*_tmp5C2[1U];_tmp5C2[0]=& _tmp5C3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C2,sizeof(void*),1U));});}_LL2A:;}}else{_LL28: _LL29:
# 3360
 for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp5C5=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5D=_tmp5C5;struct _tuple34 _tmp5C6=_stmttmp5D;void*_tmp5C8;void*_tmp5C7;_LL36: _tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;_LL37: {void*pat_test=_tmp5C7;void*dec_tree=_tmp5C8;
struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp5C9;
struct Cyc_Absyn_Stmt*_tmp5CA=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp5CA;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}}_LL25:;}
# 3368
return res;}}}}_LL0:;}
# 3378
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3380
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple41*_tmp5CB=(struct _tuple41*)bodies->hd;struct _tuple41*_stmttmp5E=_tmp5CB;struct _tuple41*_tmp5CC=_stmttmp5E;struct Cyc_Absyn_Stmt*_tmp5CE;struct Cyc_Toc_Env**_tmp5CD;_LL1: _tmp5CD=(struct Cyc_Toc_Env**)& _tmp5CC->f1;_tmp5CE=_tmp5CC->f3;_LL2: {struct Cyc_Toc_Env**nv=_tmp5CD;struct Cyc_Absyn_Stmt*s2=_tmp5CE;
if(s2 == s)return nv;}}
# 3386
return 0;}
# 3390
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3393
void*_tmp5CF=(void*)_check_null(e->topt);void*t=_tmp5CF;
Cyc_Toc_exp_to_c(nv,e);{
# 3396
struct _tuple30 _tmp5D0=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp5F=_tmp5D0;struct _tuple30 _tmp5D1=_stmttmp5F;struct Cyc_Absyn_Exp*_tmp5D3;struct _tuple1*_tmp5D2;_LL1: _tmp5D2=_tmp5D1.f1;_tmp5D3=_tmp5D1.f2;_LL2: {struct _tuple1*v=_tmp5D2;struct Cyc_Absyn_Exp*path=_tmp5D3;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5D4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D4;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5D5=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp5D5;
# 3402
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3407
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3415
{struct Cyc_List_List*_tmp5D6=lscs;struct Cyc_List_List*lscs2=_tmp5D6;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_tmp5D7=(struct _tuple37*)lscs2->hd;struct _tuple37*_stmttmp60=_tmp5D7;struct _tuple37*_tmp5D8=_stmttmp60;struct Cyc_Absyn_Switch_clause*_tmp5DA;struct _fat_ptr*_tmp5D9;_LL4: _tmp5D9=_tmp5D8->f3;_tmp5DA=_tmp5D8->f4;_LL5: {struct _fat_ptr*body_lab=_tmp5D9;struct Cyc_Absyn_Switch_clause*body_sc=_tmp5DA;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3419
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3422
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5DB=*envp;struct Cyc_Toc_Env*env=_tmp5DB;
# 3425
if(lscs2->tl != 0){
struct _tuple37*_tmp5DC=(struct _tuple37*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple37*_stmttmp61=_tmp5DC;struct _tuple37*_tmp5DD=_stmttmp61;struct Cyc_Absyn_Switch_clause*_tmp5DF;struct _fat_ptr*_tmp5DE;_LL7: _tmp5DE=_tmp5DD->f3;_tmp5DF=_tmp5DD->f4;_LL8: {struct _fat_ptr*fallthru_lab=_tmp5DE;struct Cyc_Absyn_Switch_clause*next_sc=_tmp5DF;
# 3429
struct Cyc_Toc_Env**_tmp5E0=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp5E0;
# 3434
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmpA5F=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA5F,s);});else{
# 3438
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmpA60=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmpA60,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3443
({struct Cyc_Toc_Env*_tmpA61=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmpA61,s);});}}}else{
# 3447
({struct Cyc_Toc_Env*_tmpA62=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA62,s);});}}}}}{
# 3451
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA64=test_tree;Cyc_Absyn_seq_stmt(_tmpA64,({struct _fat_ptr*_tmpA63=end_l;Cyc_Absyn_label_stmt(_tmpA63,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3453
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp5E1=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp62=_tmp5E1;struct _tuple39 _tmp5E2=_stmttmp62;struct Cyc_Absyn_Vardecl*_tmp5E3;_LLA: _tmp5E3=_tmp5E2.f2;_LLB: {struct Cyc_Absyn_Vardecl*vd=_tmp5E3;
res=({struct Cyc_Absyn_Decl*_tmpA65=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=0U,_tmp5E4->f1=vd;_tmp5E4;}),0U);Cyc_Absyn_decl_stmt(_tmpA65,res,0U);});}}
# 3458
({void*_tmpA69=({struct _tuple1*_tmpA68=v;void*_tmpA67=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA66=e;Cyc_Absyn_declare_stmt(_tmpA68,_tmpA67,_tmpA66,res,0U);})->r;whole_s->r=_tmpA69;});
_npop_handler(0U);return;}}
# 3399
;_pop_region();}}}
# 3464
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3466
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3471
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3473
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5E5[1U];({struct Cyc_Absyn_Exp*_tmpA6A=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp5E5[0]=_tmpA6A;});({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA6B,_tag_fat(_tmp5E5,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3476
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA6D=({struct Cyc_Absyn_Stmt*_tmpA6C=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA6C,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA6D;});}
# 3481
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3483
while(1){
void*_tmp5E6=s->r;void*_stmttmp63=_tmp5E6;void*_tmp5E7=_stmttmp63;void*_tmp5EA;struct Cyc_List_List*_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5E8;struct Cyc_Absyn_Stmt*_tmp5EC;struct Cyc_Absyn_Decl*_tmp5EB;struct Cyc_Absyn_Switch_clause**_tmp5EE;struct Cyc_List_List*_tmp5ED;void*_tmp5F1;struct Cyc_List_List*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5EF;struct Cyc_Absyn_Stmt*_tmp5F3;struct _fat_ptr*_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Stmt*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Stmt*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Exp*_tmp602;switch(*((int*)_tmp5E7)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp602=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp602;
Cyc_Toc_exp_to_c(nv,e);return;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1 == 0){_LL5: _LL6:
({int _tmpA6E=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA6E,s);});return;}else{_LL7: _tmp601=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp601;
# 3490
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp603=Cyc_Toc_temp_var();struct _tuple1*x=_tmp603;
struct Cyc_Absyn_Stmt*_tmp604=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp604;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmpA6F=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmpA6F;});}
# 3499
return;}}}case 2U: _LL9: _tmp5FF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp600=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LLA: {struct Cyc_Absyn_Stmt*s1=_tmp5FF;struct Cyc_Absyn_Stmt*s2=_tmp600;
# 3501
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4U: _LLB: _tmp5FC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5FD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_tmp5FE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5FC;struct Cyc_Absyn_Stmt*s1=_tmp5FD;struct Cyc_Absyn_Stmt*s2=_tmp5FE;
# 3505
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9U: _LLD: _tmp5F8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5F9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2).f1;_tmp5FA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3).f1;_tmp5FB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5F8;struct Cyc_Absyn_Exp*e2=_tmp5F9;struct Cyc_Absyn_Exp*e3=_tmp5FA;struct Cyc_Absyn_Stmt*s2=_tmp5FB;
# 3510
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5F6=s2;_tmp5F7=e3;goto _LL10;}case 14U: _LLF: _tmp5F6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5F7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5F6;struct Cyc_Absyn_Exp*e=_tmp5F7;
_tmp5F4=e;_tmp5F5=s2;goto _LL12;}case 5U: _LL11: _tmp5F4=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1).f1;_tmp5F5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5F4;struct Cyc_Absyn_Stmt*s2=_tmp5F5;
# 3515
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp605=_new_region("temp");struct _RegionHandle*temp=& _tmp605;_push_region(temp);({struct Cyc_Toc_Env*_tmpA70=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA70,s2);});
_npop_handler(0U);return;
# 3516
;_pop_region();}}case 6U: _LL13: _LL14: {
# 3519
struct Cyc_Toc_Env*_tmp606=nv;struct _fat_ptr**_tmp607;_LL24: _tmp607=_tmp606->break_lab;_LL25: {struct _fat_ptr**b=_tmp607;
if(b != 0)
({void*_tmpA71=Cyc_Toc_goto_stmt_r(*b);s->r=_tmpA71;});
# 3523
({int _tmpA72=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA72,s);});
return;}}case 7U: _LL15: _LL16: {
# 3526
struct Cyc_Toc_Env*_tmp608=nv;struct _fat_ptr**_tmp609;_LL27: _tmp609=_tmp608->continue_lab;_LL28: {struct _fat_ptr**c=_tmp609;
if(c != 0)
({void*_tmpA73=Cyc_Toc_goto_stmt_r(*c);s->r=_tmpA73;});
goto _LL18;}}case 8U: _LL17: _LL18:
# 3531
({int _tmpA74=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA74,s);});
return;case 13U: _LL19: _tmp5F2=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5F3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL1A: {struct _fat_ptr*lab=_tmp5F2;struct Cyc_Absyn_Stmt*s1=_tmp5F3;
s=s1;continue;}case 10U: _LL1B: _tmp5EF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5F0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_tmp5F1=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5EF;struct Cyc_List_List*scs=_tmp5F0;void*dec_tree_opt=_tmp5F1;
# 3535
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11U: _LL1D: _tmp5ED=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5EE=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL1E: {struct Cyc_List_List*es=_tmp5ED;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5EE;
# 3538
struct Cyc_Toc_Env*_tmp60A=nv;struct Cyc_Toc_FallthruInfo*_tmp60B;_LL2A: _tmp60B=_tmp60A->fallthru_info;_LL2B: {struct Cyc_Toc_FallthruInfo*fi=_tmp60B;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F9;_tmp7F9.tag=0U,({struct _fat_ptr _tmpA75=({const char*_tmp60E="fallthru in unexpected place";_tag_fat(_tmp60E,sizeof(char),29U);});_tmp7F9.f1=_tmpA75;});_tmp7F9;});void*_tmp60C[1U];_tmp60C[0]=& _tmp60D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp60C,sizeof(void*),1U));});{
struct Cyc_Toc_FallthruInfo _tmp60F=*fi;struct Cyc_Toc_FallthruInfo _stmttmp64=_tmp60F;struct Cyc_Toc_FallthruInfo _tmp610=_stmttmp64;struct Cyc_List_List*_tmp612;struct _fat_ptr*_tmp611;_LL2D: _tmp611=_tmp610.label;_tmp612=_tmp610.binders;_LL2E: {struct _fat_ptr*l=_tmp611;struct Cyc_List_List*vs=_tmp612;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3544
({int _tmpA76=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA76,s2);});{
struct Cyc_List_List*_tmp613=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp613;
struct Cyc_List_List*_tmp614=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp614;
for(0;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmpA78=({struct Cyc_Absyn_Exp*_tmpA77=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=5U,_tmp615->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp615;}),0U);Cyc_Absyn_assign_stmt(_tmpA77,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA78,s2,0U);});}
# 3552
s->r=s2->r;
return;}}}}}case 12U: _LL1F: _tmp5EB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5EC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp5EB;struct Cyc_Absyn_Stmt*s1=_tmp5EC;
# 3558
{void*_tmp616=d->r;void*_stmttmp65=_tmp616;void*_tmp617=_stmttmp65;struct Cyc_Absyn_Exp*_tmp61A;struct Cyc_Absyn_Vardecl*_tmp619;struct Cyc_Absyn_Tvar*_tmp618;struct Cyc_Absyn_Fndecl*_tmp61B;struct Cyc_List_List*_tmp61C;void*_tmp61F;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_Absyn_Pat*_tmp61D;struct Cyc_Absyn_Vardecl*_tmp620;switch(*((int*)_tmp617)){case 0U: _LL30: _tmp620=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp617)->f1;_LL31: {struct Cyc_Absyn_Vardecl*vd=_tmp620;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2U: _LL32: _tmp61D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp617)->f1;_tmp61E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp617)->f3;_tmp61F=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp617)->f4;_LL33: {struct Cyc_Absyn_Pat*p=_tmp61D;struct Cyc_Absyn_Exp*e=_tmp61E;void*dec_tree=_tmp61F;
# 3566
{void*_tmp621=p->r;void*_stmttmp66=_tmp621;void*_tmp622=_stmttmp66;struct Cyc_Absyn_Vardecl*_tmp623;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp622)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp622)->f2)->r)->tag == 0U){_LL3D: _tmp623=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp622)->f1;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp623;
# 3568
struct Cyc_Absyn_Vardecl*_tmp624=({struct _tuple1*_tmpA7A=Cyc_Toc_temp_var();void*_tmpA79=vd->type;Cyc_Absyn_new_vardecl(0U,_tmpA7A,_tmpA79,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp624;
({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=4U,_tmp625->f1=new_vd;_tmp625;}),0U);vd->initializer=_tmpA7B;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmpA82=({struct Cyc_Absyn_Decl*_tmpA81=({void*_tmpA7C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tag=0U,_tmp626->f1=new_vd;_tmp626;});Cyc_Absyn_new_decl(_tmpA7C,s->loc);});struct Cyc_Absyn_Stmt*_tmpA80=({
struct Cyc_Absyn_Decl*_tmpA7F=({void*_tmpA7D=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=0U,_tmp627->f1=vd;_tmp627;});Cyc_Absyn_new_decl(_tmpA7D,s->loc);});struct Cyc_Absyn_Stmt*_tmpA7E=s1;Cyc_Absyn_decl_stmt(_tmpA7F,_tmpA7E,s->loc);});
# 3571
Cyc_Absyn_decl_stmt(_tmpA81,_tmpA80,s->loc);})->r;s->r=_tmpA82;});
# 3574
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3580
({void*_tmpA83=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmpA83;});
goto _LL3C;}_LL3C:;}
# 3583
goto _LL2F;}case 3U: _LL34: _tmp61C=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp617)->f1;_LL35: {struct Cyc_List_List*vds=_tmp61C;
# 3587
struct Cyc_List_List*_tmp628=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp628;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FA;_tmp7FA.tag=0U,({struct _fat_ptr _tmpA84=({const char*_tmp62B="empty Letv_d";_tag_fat(_tmp62B,sizeof(char),13U);});_tmp7FA.f1=_tmpA84;});_tmp7FA;});void*_tmp629[1U];_tmp629[0]=& _tmp62A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp629,sizeof(void*),1U));});
({void*_tmpA85=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=0U,_tmp62C->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp62C;});d->r=_tmpA85;});
rvds=rvds->tl;
for(0;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp62D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->tag=0U,_tmp62E->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp62E;}),0U);struct Cyc_Absyn_Decl*d2=_tmp62D;
({void*_tmpA87=({struct Cyc_Absyn_Decl*_tmpA86=d2;Cyc_Absyn_decl_stmt(_tmpA86,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA87;});}
# 3596
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp61B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp617)->f1;_LL37: {struct Cyc_Absyn_Fndecl*fd=_tmp61B;
# 3599
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4U: _LL38: _tmp618=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp617)->f1;_tmp619=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp617)->f2;_tmp61A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp617)->f3;_LL39: {struct Cyc_Absyn_Tvar*tv=_tmp618;struct Cyc_Absyn_Vardecl*vd=_tmp619;struct Cyc_Absyn_Exp*open_exp_opt=_tmp61A;
# 3603
struct Cyc_Absyn_Stmt*_tmp62F=s1;struct Cyc_Absyn_Stmt*body=_tmp62F;
# 3605
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp630=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp67=_tmp630;struct _tuple30 _tmp631=_stmttmp67;struct Cyc_Absyn_Exp*_tmp633;struct _tuple1*_tmp632;_LL42: _tmp632=_tmp631.f1;_tmp633=_tmp631.f2;_LL43: {struct _tuple1*rh_var=_tmp632;struct Cyc_Absyn_Exp*rh_exp=_tmp633;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3611
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Absyn_no_regions)
({void*_tmpA8B=({struct _tuple1*_tmpA8A=x_var;void*_tmpA89=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA88=
Cyc_Absyn_uint_exp(0U,0U);
# 3613
Cyc_Absyn_declare_stmt(_tmpA8A,_tmpA89,_tmpA88,body,0U);})->r;s->r=_tmpA8B;});else{
# 3615
if((unsigned)open_exp_opt){
# 3617
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA8D=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA8D,({struct _fat_ptr*_tmp635=_cycalloc(sizeof(*_tmp635));({struct _fat_ptr _tmpA8C=({const char*_tmp634="h";_tag_fat(_tmp634,sizeof(char),2U);});*_tmp635=_tmpA8C;});_tmp635;}),0U);}),0U);
({void*_tmpA8E=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA8E;});}}else{
# 3626
({void*_tmpA9B=({struct _tuple1*_tmpA9A=rh_var;void*_tmpA99=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA98=({struct Cyc_Absyn_Exp*_tmp636[1U];({struct Cyc_Absyn_Exp*_tmpA8F=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp636[0]=_tmpA8F;});({struct Cyc_Absyn_Exp*_tmpA90=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA90,_tag_fat(_tmp636,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3626
Cyc_Absyn_declare_stmt(_tmpA9A,_tmpA99,_tmpA98,({
# 3628
struct _tuple1*_tmpA97=x_var;void*_tmpA96=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA95=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA97,_tmpA96,_tmpA95,({
struct Cyc_Absyn_Stmt*_tmpA94=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp637[1U];_tmp637[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA91,_tag_fat(_tmp637,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA94,({
struct Cyc_Absyn_Stmt*_tmpA93=body;Cyc_Absyn_seq_stmt(_tmpA93,
Cyc_Absyn_exp_stmt(({void*_tmp638=0U;({struct Cyc_Absyn_Exp*_tmpA92=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA92,_tag_fat(_tmp638,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3626
s->r=_tmpA9B;});}}
# 3634
return;}}default: _LL3A: _LL3B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FB;_tmp7FB.tag=0U,({struct _fat_ptr _tmpA9C=({const char*_tmp63B="bad nested declaration within function";_tag_fat(_tmp63B,sizeof(char),39U);});_tmp7FB.f1=_tmpA9C;});_tmp7FB;});void*_tmp639[1U];_tmp639[0]=& _tmp63A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp639,sizeof(void*),1U));});}_LL2F:;}
# 3637
return;}default: _LL21: _tmp5E8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp5E9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_tmp5EA=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3;_LL22: {struct Cyc_Absyn_Stmt*body=_tmp5E8;struct Cyc_List_List*scs=_tmp5E9;void*dec_tree=_tmp5EA;
# 3651 "toc.cyc"
struct _tuple30 _tmp63C=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp68=_tmp63C;struct _tuple30 _tmp63D=_stmttmp68;struct Cyc_Absyn_Exp*_tmp63F;struct _tuple1*_tmp63E;_LL45: _tmp63E=_tmp63D.f1;_tmp63F=_tmp63D.f2;_LL46: {struct _tuple1*h_var=_tmp63E;struct Cyc_Absyn_Exp*h_exp=_tmp63F;
struct _tuple30 _tmp640=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp69=_tmp640;struct _tuple30 _tmp641=_stmttmp69;struct Cyc_Absyn_Exp*_tmp643;struct _tuple1*_tmp642;_LL48: _tmp642=_tmp641.f1;_tmp643=_tmp641.f2;_LL49: {struct _tuple1*e_var=_tmp642;struct Cyc_Absyn_Exp*e_exp=_tmp643;
struct _tuple30 _tmp644=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6A=_tmp644;struct _tuple30 _tmp645=_stmttmp6A;struct Cyc_Absyn_Exp*_tmp647;struct _tuple1*_tmp646;_LL4B: _tmp646=_tmp645.f1;_tmp647=_tmp645.f2;_LL4C: {struct _tuple1*was_thrown_var=_tmp646;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp647;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3658
e_exp->topt=e_typ;{
struct _RegionHandle _tmp648=_new_region("temp");struct _RegionHandle*temp=& _tmp648;_push_region(temp);
# 3661
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp649=({struct Cyc_Absyn_Stmt*_tmpA9E=body;Cyc_Absyn_seq_stmt(_tmpA9E,
Cyc_Absyn_exp_stmt(({void*_tmp652=0U;({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA9D,_tag_fat(_tmp652,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3662
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp649;
# 3665
struct Cyc_Absyn_Stmt*_tmp64A=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->tag=10U,_tmp651->f1=e_exp,_tmp651->f2=scs,_tmp651->f3=dec_tree;_tmp651;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp64A;
# 3667
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3670
struct Cyc_Absyn_Exp*_tmp64B=({struct Cyc_Absyn_Exp*_tmp650[1U];({struct Cyc_Absyn_Exp*_tmpA9F=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp650[0]=_tmpA9F;});({struct Cyc_Absyn_Exp*_tmpAA0=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpAA0,_tag_fat(_tmp650,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3670
struct Cyc_Absyn_Exp*setjmp_call=_tmp64B;
# 3672
struct Cyc_Absyn_Stmt*_tmp64C=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp64F[1U];({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Absyn_address_exp(h_exp,0U);_tmp64F[0]=_tmpAA1;});({struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpAA2,_tag_fat(_tmp64F,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
# 3672
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp64C;
# 3674
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp64D;
struct Cyc_Absyn_Exp*_tmp64E=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp64E;
({void*_tmpAB2=({
struct _tuple1*_tmpAB1=h_var;void*_tmpAB0=h_typ;Cyc_Absyn_declare_stmt(_tmpAB1,_tmpAB0,0,({
struct Cyc_Absyn_Stmt*_tmpAAF=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpAAF,({
struct _tuple1*_tmpAAE=was_thrown_var;void*_tmpAAD=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpAAC=zero_exp;Cyc_Absyn_declare_stmt(_tmpAAE,_tmpAAD,_tmpAAC,({
struct Cyc_Absyn_Stmt*_tmpAAB=({struct Cyc_Absyn_Exp*_tmpAA4=setjmp_call;struct Cyc_Absyn_Stmt*_tmpAA3=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3680
Cyc_Absyn_ifthenelse_stmt(_tmpAA4,_tmpAA3,
# 3682
Cyc_Toc_skip_stmt_dl(),0U);});
# 3680
Cyc_Absyn_seq_stmt(_tmpAAB,({
# 3683
struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpAA9=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpAAA,_tmpAA9,({
# 3685
struct _tuple1*_tmpAA8=e_var;void*_tmpAA7=e_typ;struct Cyc_Absyn_Exp*_tmpAA6=({
void*_tmpAA5=e_typ;Cyc_Toc_cast_it(_tmpAA5,Cyc_Toc_get_exn_thrown_expression());});
# 3685
Cyc_Absyn_declare_stmt(_tmpAA8,_tmpAA7,_tmpAA6,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3676
s->r=_tmpAB2;});}}
# 3689
_npop_handler(0U);return;
# 3659
;_pop_region();}}}}}}_LL0:;}}
# 3698
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpAB3=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpAB3;});
({void*_tmpAB4=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpAB4;});{
struct _RegionHandle _tmp653=_new_region("frgn");struct _RegionHandle*frgn=& _tmp653;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp654=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp654;
{struct Cyc_List_List*_tmp655=(f->i).args;struct Cyc_List_List*args=_tmp655;for(0;args != 0;args=args->tl){
struct _tuple1*_tmp656=({struct _tuple1*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->f1=Cyc_Absyn_Loc_n,_tmp657->f2=(*((struct _tuple9*)args->hd)).f1;_tmp657;});struct _tuple1*x=_tmp656;
({void*_tmpAB5=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAB5;});}}
# 3715
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3719
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp658=_cycalloc(sizeof(*_tmp658));({struct Cyc_Hashtable_Table*_tmpAB6=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp658=_tmpAB6;});_tmp658;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp659=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6B=_tmp659;struct Cyc_Absyn_VarargInfo _tmp65A=_stmttmp6B;int _tmp65E;void*_tmp65D;struct Cyc_Absyn_Tqual _tmp65C;struct _fat_ptr*_tmp65B;_LL1: _tmp65B=_tmp65A.name;_tmp65C=_tmp65A.tq;_tmp65D=_tmp65A.type;_tmp65E=_tmp65A.inject;_LL2: {struct _fat_ptr*n=_tmp65B;struct Cyc_Absyn_Tqual tq=_tmp65C;void*t=_tmp65D;int i=_tmp65E;
void*_tmp65F=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type));void*t2=_tmp65F;
struct _tuple1*_tmp660=({struct _tuple1*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->f1=Cyc_Absyn_Loc_n,_tmp663->f2=(struct _fat_ptr*)_check_null(n);_tmp663;});struct _tuple1*x2=_tmp660;
({struct Cyc_List_List*_tmpAB9=({struct Cyc_List_List*_tmpAB8=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAB8,({struct Cyc_List_List*_tmp662=_cycalloc(sizeof(*_tmp662));({struct _tuple9*_tmpAB7=({struct _tuple9*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->f1=n,_tmp661->f2=tq,_tmp661->f3=t2;_tmp661;});_tmp662->hd=_tmpAB7;}),_tmp662->tl=0;_tmp662;}));});(f->i).args=_tmpAB9;});
(f->i).cyc_varargs=0;}}
# 3728
{struct Cyc_List_List*_tmp664=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp664;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpABA=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpABA;});}}
({struct Cyc_Toc_Env*_tmpABB=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpABB,f->body);});}
# 3708
;_pop_region();}}
# 3733
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp665=s;switch(_tmp665){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3749 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3751
struct _tuple42 _tmp666=*env;struct _tuple42 _stmttmp6C=_tmp666;struct _tuple42 _tmp667=_stmttmp6C;struct Cyc_Absyn_Aggrdecl*_tmp669;struct Cyc_Toc_TocState*_tmp668;_LL1: _tmp668=_tmp667.f1;_tmp669=*_tmp667.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp668;struct Cyc_Absyn_Aggrdecl*ad=_tmp669;
struct _tuple1*_tmp66A=ad->name;struct _tuple1*n=_tmp66A;
struct Cyc_Toc_TocState _tmp66B=*s;struct Cyc_Toc_TocState _stmttmp6D=_tmp66B;struct Cyc_Toc_TocState _tmp66C=_stmttmp6D;struct Cyc_Dict_Dict*_tmp66D;_LL4: _tmp66D=_tmp66C.aggrs_so_far;_LL5: {struct Cyc_Dict_Dict*aggrs_so_far=_tmp66D;
int seen_defn_before;
struct _tuple18**_tmp66E=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple18**dopt=_tmp66E;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)ad->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp66F=_region_malloc(d,sizeof(*_tmp66F));_tmp66F->f1=ad,({void*_tmpABC=Cyc_Absyn_strctq(n);_tmp66F->f2=_tmpABC;});_tmp66F;});else{
# 3762
v=({struct _tuple18*_tmp670=_region_malloc(d,sizeof(*_tmp670));_tmp670->f1=ad,({void*_tmpABD=Cyc_Absyn_unionq_type(n);_tmp670->f2=_tmpABD;});_tmp670;});}
({struct Cyc_Dict_Dict _tmpABE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpABE;});}}else{
# 3765
struct _tuple18*_tmp671=*dopt;struct _tuple18*_stmttmp6E=_tmp671;struct _tuple18*_tmp672=_stmttmp6E;void*_tmp674;struct Cyc_Absyn_Aggrdecl*_tmp673;_LL7: _tmp673=_tmp672->f1;_tmp674=_tmp672->f2;_LL8: {struct Cyc_Absyn_Aggrdecl*ad2=_tmp673;void*t=_tmp674;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpAC1=({struct Cyc_Dict_Dict _tmpAC0=*aggrs_so_far;struct _tuple1*_tmpABF=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpAC0,_tmpABF,({struct _tuple18*_tmp675=_region_malloc(d,sizeof(*_tmp675));_tmp675->f1=ad,_tmp675->f2=t;_tmp675;}));});*aggrs_so_far=_tmpAC1;});
seen_defn_before=0;}else{
# 3770
seen_defn_before=1;}}}{
# 3772
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->kind=ad->kind,_tmp69A->sc=Cyc_Absyn_Public,_tmp69A->name=ad->name,_tmp69A->tvs=0,_tmp69A->impl=0,_tmp69A->expected_mem_kind=0,_tmp69A->attributes=ad->attributes;_tmp69A;});
# 3779
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpAC2=({struct Cyc_Absyn_AggrdeclImpl*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->exist_vars=0,_tmp676->rgn_po=0,_tmp676->fields=0,_tmp676->tagged=0;_tmp676;});new_ad->impl=_tmpAC2;});{
# 3784
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp677=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp677;for(0;fields != 0;fields=fields->tl){
# 3788
struct Cyc_Absyn_Aggrfield*_tmp678=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp678;
void*_tmp679=old_field->type;void*old_type=_tmp679;
struct Cyc_List_List*_tmp67A=old_field->attributes;struct Cyc_List_List*old_atts=_tmp67A;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3802 "toc.cyc"
void*_tmp67B=Cyc_Tcutil_compress(old_type);void*_stmttmp6F=_tmp67B;void*_tmp67C=_stmttmp6F;unsigned _tmp680;void*_tmp67F;struct Cyc_Absyn_Tqual _tmp67E;void*_tmp67D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67C)->tag == 4U){_LLA: _tmp67D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67C)->f1).elt_type;_tmp67E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67C)->f1).tq;_tmp67F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67C)->f1).zero_term;_tmp680=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67C)->f1).zt_loc;_LLB: {void*et=_tmp67D;struct Cyc_Absyn_Tqual tq=_tmp67E;void*zt=_tmp67F;unsigned ztl=_tmp680;
# 3805
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->tag=4U,(_tmp681->f1).elt_type=et,(_tmp681->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpAC3=Cyc_Absyn_uint_exp(0U,0U);(_tmp681->f1).num_elts=_tmpAC3;}),(_tmp681->f1).zero_term=zt,(_tmp681->f1).zt_loc=ztl;_tmp681;});
goto _LL9;}}else{_LLC: _LLD:
# 3808
 old_atts=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));({void*_tmpAC4=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->tag=6U,_tmp682->f1=0;_tmp682;});_tmp683->hd=_tmpAC4;}),_tmp683->tl=old_atts;_tmp683;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->tag=4U,({void*_tmpAC7=Cyc_Toc_void_star_type();(_tmp684->f1).elt_type=_tmpAC7;}),({
struct Cyc_Absyn_Tqual _tmpAC6=Cyc_Absyn_empty_tqual(0U);(_tmp684->f1).tq=_tmpAC6;}),({
struct Cyc_Absyn_Exp*_tmpAC5=Cyc_Absyn_uint_exp(0U,0U);(_tmp684->f1).num_elts=_tmpAC5;}),(_tmp684->f1).zero_term=Cyc_Absyn_false_type,(_tmp684->f1).zt_loc=0U;_tmp684;});}_LL9:;}{
# 3815
struct Cyc_Absyn_Aggrfield*_tmp685=({struct Cyc_Absyn_Aggrfield*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->name=old_field->name,_tmp696->tq=Cyc_Toc_mt_tq,({
# 3817
void*_tmpAC8=Cyc_Toc_typ_to_c(old_type);_tmp696->type=_tmpAC8;}),_tmp696->width=old_field->width,_tmp696->attributes=old_atts,_tmp696->requires_clause=0;_tmp696;});
# 3815
struct Cyc_Absyn_Aggrfield*new_field=_tmp685;
# 3825
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp686=new_field->type;void*T=_tmp686;
struct _fat_ptr*_tmp687=new_field->name;struct _fat_ptr*f=_tmp687;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp693=({struct Cyc_String_pa_PrintArg_struct _tmp7FD;_tmp7FD.tag=0U,_tmp7FD.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7FD;});struct Cyc_String_pa_PrintArg_struct _tmp694=({struct Cyc_String_pa_PrintArg_struct _tmp7FC;_tmp7FC.tag=0U,_tmp7FC.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7FC;});void*_tmp691[2U];_tmp691[0]=& _tmp693,_tmp691[1]=& _tmp694;({struct _fat_ptr _tmpAC9=({const char*_tmp692="_union_%s_%s";_tag_fat(_tmp692,sizeof(char),13U);});Cyc_aprintf(_tmpAC9,_tag_fat(_tmp691,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp690=_cycalloc(sizeof(*_tmp690));*_tmp690=s;_tmp690;});
struct Cyc_Absyn_Aggrfield*_tmp688=({struct Cyc_Absyn_Aggrfield*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->name=Cyc_Toc_val_sp,_tmp68F->tq=Cyc_Toc_mt_tq,_tmp68F->type=T,_tmp68F->width=0,_tmp68F->attributes=0,_tmp68F->requires_clause=0;_tmp68F;});struct Cyc_Absyn_Aggrfield*value_field=_tmp688;
struct Cyc_Absyn_Aggrfield*_tmp689=({struct Cyc_Absyn_Aggrfield*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->name=Cyc_Toc_tag_sp,_tmp68E->tq=Cyc_Toc_mt_tq,_tmp68E->type=Cyc_Absyn_sint_type,_tmp68E->width=0,_tmp68E->attributes=0,_tmp68E->requires_clause=0;_tmp68E;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp689;
struct Cyc_Absyn_Aggrdecl*_tmp68A=({struct _fat_ptr*_tmpACA=str;Cyc_Toc_make_c_struct_defn(_tmpACA,({struct Cyc_Absyn_Aggrfield*_tmp68D[2U];_tmp68D[0]=tag_field,_tmp68D[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp68D,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp68A;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct Cyc_Absyn_Decl*_tmpACB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B->tag=5U,_tmp68B->f1=ad2;_tmp68B;}),0U);_tmp68C->hd=_tmpACB;}),_tmp68C->tl=Cyc_Toc_result_decls;_tmp68C;});
({void*_tmpACC=Cyc_Absyn_strct(str);new_field->type=_tmpACC;});}
# 3836
new_fields=({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->hd=new_field,_tmp695->tl=new_fields;_tmp695;});}}}
# 3838
({struct Cyc_List_List*_tmpACD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpACD;});}}
# 3841
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));({struct Cyc_Absyn_Decl*_tmpACF=({struct Cyc_Absyn_Decl*_tmp698=_cycalloc(sizeof(*_tmp698));({void*_tmpACE=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->tag=5U,_tmp697->f1=new_ad;_tmp697;});_tmp698->r=_tmpACE;}),_tmp698->loc=0U;_tmp698;});_tmp699->hd=_tmpACF;}),_tmp699->tl=Cyc_Toc_result_decls;_tmp699;});
return 0;}}}}
# 3845
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3873 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3876
struct _tuple43 _tmp69B=*env;struct _tuple43 _stmttmp70=_tmp69B;struct _tuple43 _tmp69C=_stmttmp70;struct Cyc_Absyn_Datatypedecl*_tmp69E;struct Cyc_Set_Set**_tmp69D;_LL1: _tmp69D=(_tmp69C.f1)->datatypes_so_far;_tmp69E=_tmp69C.f2;_LL2: {struct Cyc_Set_Set**datatypes_so_far=_tmp69D;struct Cyc_Absyn_Datatypedecl*tud=_tmp69E;
struct _tuple1*_tmp69F=tud->name;struct _tuple1*n=_tmp69F;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpAD0=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpAD0;});
{struct Cyc_List_List*_tmp6A0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp6A0;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3884
struct Cyc_List_List*_tmp6A1=0;struct Cyc_List_List*fs=_tmp6A1;
int i=1;
{struct Cyc_List_List*_tmp6A2=f->typs;struct Cyc_List_List*ts=_tmp6A2;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp6A3=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp6A3;
struct Cyc_Absyn_Aggrfield*_tmp6A4=({struct Cyc_Absyn_Aggrfield*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->name=fname,_tmp6A6->tq=(*((struct _tuple23*)ts->hd)).f1,({
void*_tmpAD1=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f2);_tmp6A6->type=_tmpAD1;}),_tmp6A6->width=0,_tmp6A6->attributes=0,_tmp6A6->requires_clause=0;_tmp6A6;});
# 3888
struct Cyc_Absyn_Aggrfield*f=_tmp6A4;
# 3890
fs=({struct Cyc_List_List*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=f,_tmp6A5->tl=fs;_tmp6A5;});}}
# 3892
fs=({struct Cyc_List_List*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));({struct Cyc_Absyn_Aggrfield*_tmpAD3=({struct Cyc_Absyn_Aggrfield*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->name=Cyc_Toc_tag_sp,_tmp6A7->tq=Cyc_Toc_mt_tq,_tmp6A7->type=Cyc_Absyn_sint_type,_tmp6A7->width=0,_tmp6A7->attributes=0,_tmp6A7->requires_clause=0;_tmp6A7;});_tmp6A8->hd=_tmpAD3;}),({
struct Cyc_List_List*_tmpAD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);_tmp6A8->tl=_tmpAD2;});_tmp6A8;});{
struct Cyc_Absyn_Aggrdecl*_tmp6A9=({struct _fat_ptr*_tmpAD5=({struct _fat_ptr*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));({struct _fat_ptr _tmpAD4=({const char*_tmp6AC="";_tag_fat(_tmp6AC,sizeof(char),1U);});*_tmp6AD=_tmpAD4;});_tmp6AD;});Cyc_Toc_make_c_struct_defn(_tmpAD5,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp6A9;
({struct _tuple1*_tmpAD6=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpAD6;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));({struct Cyc_Absyn_Decl*_tmpAD7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->tag=5U,_tmp6AA->f1=ad;_tmp6AA;}),0U);_tmp6AB->hd=_tmpAD7;}),_tmp6AB->tl=Cyc_Toc_result_decls;_tmp6AB;});}}}
# 3898
return 0;}}
# 3901
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3920 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3923
struct _tuple43 _tmp6AE=*env;struct _tuple43 _stmttmp71=_tmp6AE;struct _tuple43 _tmp6AF=_stmttmp71;struct Cyc_Absyn_Datatypedecl*_tmp6B1;struct Cyc_Toc_TocState*_tmp6B0;_LL1: _tmp6B0=_tmp6AF.f1;_tmp6B1=_tmp6AF.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp6B0;struct Cyc_Absyn_Datatypedecl*xd=_tmp6B1;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6B2=*s;struct Cyc_Toc_TocState _stmttmp72=_tmp6B2;struct Cyc_Toc_TocState _tmp6B3=_stmttmp72;struct Cyc_Dict_Dict*_tmp6B4;_LL4: _tmp6B4=_tmp6B3.xdatatypes_so_far;_LL5: {struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp6B4;
struct _tuple1*_tmp6B5=xd->name;struct _tuple1*n=_tmp6B5;
{struct Cyc_List_List*_tmp6B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp6B6;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp6B7;
void*_tmp6B8=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp6B8;
# 3934
int*_tmp6B9=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp73=_tmp6B9;int*_tmp6BA=_stmttmp73;if(_tmp6BA == 0){_LL7: _LL8: {
# 3936
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3940
struct Cyc_Absyn_Vardecl*_tmp6BB=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6BB;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({struct Cyc_Absyn_Decl*_tmpAD8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->tag=0U,_tmp6BC->f1=tag_decl;_tmp6BC;}),0U);_tmp6BD->hd=_tmpAD8;}),_tmp6BD->tl=Cyc_Toc_result_decls;_tmp6BD;});
({struct Cyc_Dict_Dict _tmpAD9=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3943
*xdatatypes_so_far=_tmpAD9;});{
# 3945
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6BE=f->typs;struct Cyc_List_List*tqts=_tmp6BE;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp6BF=({struct _fat_ptr*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));({struct _fat_ptr _tmpADB=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C5=({struct Cyc_Int_pa_PrintArg_struct _tmp7FE;_tmp7FE.tag=1U,_tmp7FE.f1=(unsigned long)i;_tmp7FE;});void*_tmp6C3[1U];_tmp6C3[0]=& _tmp6C5;({struct _fat_ptr _tmpADA=({const char*_tmp6C4="f%d";_tag_fat(_tmp6C4,sizeof(char),4U);});Cyc_aprintf(_tmpADA,_tag_fat(_tmp6C3,sizeof(void*),1U));});});*_tmp6C6=_tmpADB;});_tmp6C6;});struct _fat_ptr*field_n=_tmp6BF;
struct Cyc_Absyn_Aggrfield*_tmp6C0=({struct Cyc_Absyn_Aggrfield*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->name=field_n,_tmp6C2->tq=(*((struct _tuple23*)tqts->hd)).f1,({
void*_tmpADC=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f2);_tmp6C2->type=_tmpADC;}),_tmp6C2->width=0,_tmp6C2->attributes=0,_tmp6C2->requires_clause=0;_tmp6C2;});
# 3949
struct Cyc_Absyn_Aggrfield*newf=_tmp6C0;
# 3951
fields=({struct Cyc_List_List*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->hd=newf,_tmp6C1->tl=fields;_tmp6C1;});}}
# 3953
fields=({struct Cyc_List_List*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));({struct Cyc_Absyn_Aggrfield*_tmpADF=({struct Cyc_Absyn_Aggrfield*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->name=Cyc_Toc_tag_sp,_tmp6C7->tq=Cyc_Toc_mt_tq,({
void*_tmpADE=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6C7->type=_tmpADE;}),_tmp6C7->width=0,_tmp6C7->attributes=0,_tmp6C7->requires_clause=0;_tmp6C7;});
# 3953
_tmp6C8->hd=_tmpADF;}),({
# 3955
struct Cyc_List_List*_tmpADD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C8->tl=_tmpADD;});_tmp6C8;});{
struct Cyc_Absyn_Aggrdecl*_tmp6C9=({struct _fat_ptr*_tmpAE1=({struct _fat_ptr*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));({struct _fat_ptr _tmpAE0=({const char*_tmp6CC="";_tag_fat(_tmp6CC,sizeof(char),1U);});*_tmp6CD=_tmpAE0;});_tmp6CD;});Cyc_Toc_make_c_struct_defn(_tmpAE1,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp6C9;
({struct _tuple1*_tmpAE2=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpAE2;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));({struct Cyc_Absyn_Decl*_tmpAE3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->tag=5U,_tmp6CA->f1=strct_decl;_tmp6CA;}),0U);_tmp6CB->hd=_tmpAE3;}),_tmp6CB->tl=Cyc_Toc_result_decls;_tmp6CB;});
goto _LL6;}}}}}else{if(*((int*)_tmp6BA)== 0){_LL9: _LLA:
# 3961
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6CE=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp6CE;
struct Cyc_Absyn_Vardecl*_tmp6CF=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6CF;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({struct Cyc_Absyn_Decl*_tmpAE4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tag=0U,_tmp6D0->f1=tag_decl;_tmp6D0;}),0U);_tmp6D1->hd=_tmpAE4;}),_tmp6D1->tl=Cyc_Toc_result_decls;_tmp6D1;});
({struct Cyc_Dict_Dict _tmpAE5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpAE5;});}
# 3968
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3972
return 0;}}}}
# 3975
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3979
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3985
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpAE6=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpAE6;});
# 3989
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3998
struct _RegionHandle _tmp6D2=_new_region("temp");struct _RegionHandle*temp=& _tmp6D2;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D3=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp6D3;
Cyc_Toc_exp_to_c(nv2,init);}
# 3999
;_pop_region();}else{
# 4003
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4006
void*_tmp6D4=Cyc_Tcutil_compress(old_typ);void*_stmttmp74=_tmp6D4;void*_tmp6D5=_stmttmp74;void*_tmp6D8;struct Cyc_Absyn_Exp*_tmp6D7;void*_tmp6D6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D5)->tag == 4U){_LL1: _tmp6D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D5)->f1).elt_type;_tmp6D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D5)->f1).num_elts;_tmp6D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D5)->f1).zero_term;_LL2: {void*et=_tmp6D6;struct Cyc_Absyn_Exp*num_elts_opt=_tmp6D7;void*zt=_tmp6D8;
# 4008
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FF;_tmp7FF.tag=0U,({struct _fat_ptr _tmpAE7=({const char*_tmp6DB="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp6DB,sizeof(char),55U);});_tmp7FF.f1=_tmpAE7;});_tmp7FF;});void*_tmp6D9[1U];_tmp6D9[0]=& _tmp6DA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6D9,sizeof(void*),1U));});{
struct Cyc_Absyn_Exp*num_elts=num_elts_opt;
struct Cyc_Absyn_Exp*_tmp6DC=({struct Cyc_Absyn_Exp*_tmpAE9=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpAE9,({
struct Cyc_Absyn_Exp*_tmpAE8=num_elts;Cyc_Absyn_add_exp(_tmpAE8,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4012
struct Cyc_Absyn_Exp*lhs=_tmp6DC;
# 4015
struct Cyc_Absyn_Exp*_tmp6DD=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp6DD;
({void*_tmpAEB=({struct Cyc_Absyn_Stmt*_tmpAEA=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAEA,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4016
s->r=_tmpAEB;});}}
# 4019
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4027
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6DE=d;void**_tmp6E1;struct Cyc_List_List*_tmp6E0;struct Cyc_List_List*_tmp6DF;struct Cyc_Tcpat_Rhs*_tmp6E2;switch(*((int*)_tmp6DE)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6E2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6DE)->f1;_LL4: {struct Cyc_Tcpat_Rhs*rhs=_tmp6E2;
rhs->rhs=success;return d;}default: _LL5: _tmp6DF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DE)->f1;_tmp6E0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DE)->f2;_tmp6E1=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DE)->f3;_LL6: {struct Cyc_List_List*path=_tmp6DF;struct Cyc_List_List*sws=_tmp6E0;void**d2=_tmp6E1;
# 4032
({void*_tmpAEC=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAEC;});
for(0;sws != 0;sws=sws->tl){
struct _tuple34*_tmp6E3=(struct _tuple34*)sws->hd;struct _tuple34*_stmttmp75=_tmp6E3;struct _tuple34*_tmp6E4=_stmttmp75;void**_tmp6E5;_LL8: _tmp6E5=(void**)& _tmp6E4->f2;_LL9: {void**d2=_tmp6E5;
({void*_tmpAED=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAED;});}}
# 4037
return d;}}_LL0:;}
# 4048 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4050
struct _RegionHandle _tmp6E6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E6;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E7=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp6E7;
void*_tmp6E8=(void*)_check_null(e->topt);void*t=_tmp6E8;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4060
struct Cyc_Absyn_Stmt*_tmp6E9=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6E9;
if(dopt != 0)
# 4064
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4066
struct Cyc_Absyn_Switch_clause*_tmp6EA=({struct Cyc_Absyn_Switch_clause*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->pattern=p,_tmp6F9->pat_vars=0,_tmp6F9->where_clause=0,_tmp6F9->body=succ_stmt,_tmp6F9->loc=0U;_tmp6F9;});struct Cyc_Absyn_Switch_clause*c1=_tmp6EA;
struct Cyc_List_List*_tmp6EB=({struct _RegionHandle*_tmpAF0=rgn;struct _RegionHandle*_tmpAEF=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpAF0,Cyc_Toc_gen_labels,_tmpAEF,({struct Cyc_Absyn_Switch_clause*_tmp6F8[1U];_tmp6F8[0]=c1;({struct _RegionHandle*_tmpAEE=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpAEE,_tag_fat(_tmp6F8,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});struct Cyc_List_List*lscs=_tmp6EB;
# 4069
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4073
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4076
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple41*_tmp6EC=(struct _tuple41*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple41*_stmttmp76=_tmp6EC;struct _tuple41*_tmp6ED=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp6EF;struct Cyc_Toc_Env*_tmp6EE;_LL1: _tmp6EE=_tmp6ED->f1;_tmp6EF=_tmp6ED->f3;_LL2: {struct Cyc_Toc_Env*env=_tmp6EE;struct Cyc_Absyn_Stmt*st=_tmp6EF;
if(st == succ_stmt){senv=env;goto FOUND_ENV;}}}
# 4081
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp800;_tmp800.tag=0U,({struct _fat_ptr _tmpAF1=({const char*_tmp6F2="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6F2,sizeof(char),33U);});_tmp800.f1=_tmpAF1;});_tmp800;});void*_tmp6F0[1U];_tmp6F0[0]=& _tmp6F1;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6F0,sizeof(void*),1U));});
FOUND_ENV:
# 4085
 Cyc_Toc_stmt_to_c(senv,s);{
# 4087
struct Cyc_Absyn_Stmt*res=test_tree;
# 4089
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp6F3=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp77=_tmp6F3;struct _tuple39 _tmp6F4=_stmttmp77;struct Cyc_Absyn_Vardecl*_tmp6F5;_LL4: _tmp6F5=_tmp6F4.f2;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp6F5;
res=({struct Cyc_Absyn_Decl*_tmpAF2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6->tag=0U,_tmp6F6->f1=vd;_tmp6F6;}),0U);Cyc_Absyn_decl_stmt(_tmpAF2,res,0U);});}}
# 4094
res=({struct _tuple1*_tmpAF5=v;void*_tmpAF4=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpAF3=e;Cyc_Absyn_declare_stmt(_tmpAF5,_tmpAF4,_tmpAF3,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6F7=res;_npop_handler(0U);return _tmp6F7;}}}}}
# 4051
;_pop_region();}
# 4103
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6FA=e->r;void*_stmttmp78=_tmp6FA;void*_tmp6FB=_stmttmp78;struct Cyc_List_List*_tmp6FD;struct Cyc_List_List*_tmp6FC;struct Cyc_Absyn_MallocInfo*_tmp6FE;struct Cyc_Absyn_Stmt*_tmp6FF;void**_tmp700;void**_tmp701;struct Cyc_List_List*_tmp702;struct Cyc_List_List*_tmp703;struct Cyc_List_List*_tmp705;void**_tmp704;struct Cyc_Absyn_Exp*_tmp707;void**_tmp706;struct Cyc_List_List*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_List_List*_tmp719;struct Cyc_Absyn_Exp*_tmp71A;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_Absyn_Exp*_tmp71C;struct Cyc_Absyn_Exp*_tmp71D;struct Cyc_Absyn_Exp*_tmp71E;struct Cyc_Absyn_Exp*_tmp71F;struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp721;struct Cyc_Absyn_Exp*_tmp722;switch(*((int*)_tmp6FB)){case 41U: _LL1: _tmp722=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp722;
_tmp721=e;goto _LL4;}case 20U: _LL3: _tmp721=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp721;
_tmp720=e;goto _LL6;}case 21U: _LL5: _tmp720=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp720;
_tmp71F=e;goto _LL8;}case 22U: _LL7: _tmp71F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp71F;
_tmp71E=e;goto _LLA;}case 15U: _LL9: _tmp71E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp71E;
_tmp71D=e;goto _LLC;}case 11U: _LLB: _tmp71D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp71D;
_tmp71C=e;goto _LLE;}case 12U: _LLD: _tmp71C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp71C;
_tmp71B=e;goto _LL10;}case 18U: _LLF: _tmp71B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp71B;
_tmp71A=e;goto _LL12;}case 5U: _LL11: _tmp71A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp71A;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3U: _LL13: _tmp719=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL14: {struct Cyc_List_List*es=_tmp719;
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7U: _LL15: _tmp717=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp718=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp717;struct Cyc_Absyn_Exp*e2=_tmp718;
_tmp715=e1;_tmp716=e2;goto _LL18;}case 8U: _LL17: _tmp715=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp716=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp715;struct Cyc_Absyn_Exp*e2=_tmp716;
_tmp713=e1;_tmp714=e2;goto _LL1A;}case 9U: _LL19: _tmp713=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp714=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp713;struct Cyc_Absyn_Exp*e2=_tmp714;
_tmp711=e1;_tmp712=e2;goto _LL1C;}case 23U: _LL1B: _tmp711=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp712=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp711;struct Cyc_Absyn_Exp*e2=_tmp712;
_tmp70F=e1;_tmp710=e2;goto _LL1E;}case 35U: _LL1D: _tmp70F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp710=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp70F;struct Cyc_Absyn_Exp*e2=_tmp710;
_tmp70D=e1;_tmp70E=e2;goto _LL20;}case 4U: _LL1F: _tmp70D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp70E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FB)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp70D;struct Cyc_Absyn_Exp*e2=_tmp70E;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6U: _LL21: _tmp70A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp70B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_tmp70C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FB)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp70A;struct Cyc_Absyn_Exp*e2=_tmp70B;struct Cyc_Absyn_Exp*e3=_tmp70C;
# 4122
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10U: _LL23: _tmp708=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp709=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp708;struct Cyc_List_List*es=_tmp709;
# 4124
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14U: _LL25: _tmp706=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_tmp707=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL26: {void**t=_tmp706;struct Cyc_Absyn_Exp*e=_tmp707;
({void*_tmpAF6=Cyc_Toc_typ_to_c(*t);*t=_tmpAF6;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25U: _LL27: _tmp704=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1)->f3;_tmp705=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL28: {void**t=_tmp704;struct Cyc_List_List*dles=_tmp705;
# 4127
({void*_tmpAF7=Cyc_Toc_typ_to_c(*t);*t=_tmpAF7;});
_tmp703=dles;goto _LL2A;}case 36U: _LL29: _tmp703=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6FB)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp703;
_tmp702=dles;goto _LL2C;}case 26U: _LL2B: _tmp702=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp702;
# 4131
for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19U: _LL2D: _tmp701=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL2E: {void**t=_tmp701;
_tmp700=t;goto _LL30;}case 17U: _LL2F: _tmp700=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL30: {void**t=_tmp700;
({void*_tmpAF8=Cyc_Toc_typ_to_c(*t);*t=_tmpAF8;});goto _LL0;}case 37U: _LL31: _tmp6FF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL32: {struct Cyc_Absyn_Stmt*s=_tmp6FF;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34U: _LL33: _tmp6FE=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6FB)->f1;_LL34: {struct Cyc_Absyn_MallocInfo*m=_tmp6FE;
# 4138
if(m->elt_type != 0)
({void**_tmpAFA=({void**_tmp723=_cycalloc(sizeof(*_tmp723));({void*_tmpAF9=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp723=_tmpAF9;});_tmp723;});m->elt_type=_tmpAFA;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40U: _LL35: _tmp6FC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FB)->f3;_tmp6FD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FB)->f4;_LL36: {struct Cyc_List_List*o=_tmp6FC;struct Cyc_List_List*i=_tmp6FD;
# 4143
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0U: _LL37: _LL38:
 goto _LL3A;case 1U: _LL39: _LL3A:
 goto _LL3C;case 32U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 42U: _LL3F: _LL40:
# 4151
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
({void*_tmp724=0U;({unsigned _tmpAFC=e->loc;struct _fat_ptr _tmpAFB=({const char*_tmp725="Cyclone expression in C code";_tag_fat(_tmp725,sizeof(char),29U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAFC,_tmpAFB,_tag_fat(_tmp724,sizeof(void*),0U));});});}_LL0:;}
# 4166
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp726=d->r;void*_stmttmp79=_tmp726;void*_tmp727=_stmttmp79;struct Cyc_Absyn_Typedefdecl*_tmp728;struct Cyc_Absyn_Enumdecl*_tmp729;struct Cyc_Absyn_Aggrdecl*_tmp72A;struct Cyc_Absyn_Fndecl*_tmp72B;struct Cyc_Absyn_Vardecl*_tmp72C;switch(*((int*)_tmp727)){case 0U: _LL1: _tmp72C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp72C;
# 4169
({void*_tmpAFD=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpAFD;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp72B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp72B;
# 4173
({void*_tmpAFE=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpAFE;});
{struct Cyc_List_List*_tmp72D=(fd->i).args;struct Cyc_List_List*args=_tmp72D;for(0;args != 0;args=args->tl){
({void*_tmpAFF=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAFF;});}}
goto _LL0;}case 5U: _LL5: _tmp72A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp72A;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7U: _LL7: _tmp729=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp729;
# 4179
if(ed->fields != 0){
struct Cyc_List_List*_tmp72E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp72E;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp72F=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp72F;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4184
goto _LL0;}case 8U: _LL9: _tmp728=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp727)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp728;
({void*_tmpB00=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB00;});goto _LL0;}case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4194
({void*_tmp730=0U;({unsigned _tmpB02=d->loc;struct _fat_ptr _tmpB01=({const char*_tmp731="Cyclone declaration in C code";_tag_fat(_tmp731,sizeof(char),30U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB02,_tmpB01,_tag_fat(_tmp730,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4202
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp732=s->r;void*_stmttmp7A=_tmp732;void*_tmp733=_stmttmp7A;struct Cyc_Absyn_Stmt*_tmp734;struct Cyc_Absyn_Exp*_tmp736;struct Cyc_Absyn_Stmt*_tmp735;struct Cyc_Absyn_Stmt*_tmp738;struct Cyc_Absyn_Decl*_tmp737;void*_tmp73B;struct Cyc_List_List*_tmp73A;struct Cyc_Absyn_Exp*_tmp739;struct Cyc_Absyn_Stmt*_tmp73F;struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Exp*_tmp73D;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Stmt*_tmp741;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_Absyn_Stmt*_tmp744;struct Cyc_Absyn_Stmt*_tmp743;struct Cyc_Absyn_Exp*_tmp742;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Stmt*_tmp747;struct Cyc_Absyn_Stmt*_tmp746;struct Cyc_Absyn_Exp*_tmp748;switch(*((int*)_tmp733)){case 1U: _LL1: _tmp748=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp748;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2U: _LL3: _tmp746=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp747=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp746;struct Cyc_Absyn_Stmt*s2=_tmp747;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3U: _LL5: _tmp745=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_LL6: {struct Cyc_Absyn_Exp*eopt=_tmp745;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4U: _LL7: _tmp742=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp743=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_tmp744=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp733)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp742;struct Cyc_Absyn_Stmt*s1=_tmp743;struct Cyc_Absyn_Stmt*s2=_tmp744;
# 4208
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5U: _LL9: _tmp740=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp733)->f1).f1;_tmp741=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp740;struct Cyc_Absyn_Stmt*s=_tmp741;
# 4210
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9U: _LLB: _tmp73C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp73D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f2).f1;_tmp73E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f3).f1;_tmp73F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp733)->f4;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp73C;struct Cyc_Absyn_Exp*e2=_tmp73D;struct Cyc_Absyn_Exp*e3=_tmp73E;struct Cyc_Absyn_Stmt*s=_tmp73F;
# 4212
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10U: _LLD: _tmp739=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp73A=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_tmp73B=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp733)->f3;_LLE: {struct Cyc_Absyn_Exp*e=_tmp739;struct Cyc_List_List*scs=_tmp73A;void*dec_tree=_tmp73B;
# 4215
Cyc_Toc_exptypes_to_c(e);
for(0;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12U: _LLF: _tmp737=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp738=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LL10: {struct Cyc_Absyn_Decl*d=_tmp737;struct Cyc_Absyn_Stmt*s=_tmp738;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14U: _LL11: _tmp735=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp733)->f1;_tmp736=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp733)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp735;struct Cyc_Absyn_Exp*e=_tmp736;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13U: _LL13: _tmp734=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp733)->f2;_LL14: {struct Cyc_Absyn_Stmt*s=_tmp734;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4227
({void*_tmpB03=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->tag=0U;_tmp749;});s->r=_tmpB03;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp74A=0U;({unsigned _tmpB05=s->loc;struct _fat_ptr _tmpB04=({const char*_tmp74B="Cyclone statement in C code";_tag_fat(_tmp74B,sizeof(char),28U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB05,_tmpB04,_tag_fat(_tmp74A,sizeof(void*),0U));});});}_LL0:;}
# 4235
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4237
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp801;_tmp801.tag=0U,({struct _fat_ptr _tmpB06=({const char*_tmp74E="decls_to_c: not at toplevel!";_tag_fat(_tmp74E,sizeof(char),29U);});_tmp801.f1=_tmpB06;});_tmp801;});void*_tmp74C[1U];_tmp74C[0]=& _tmp74D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp74C,sizeof(void*),1U));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp74F=d->r;void*_stmttmp7B=_tmp74F;void*_tmp750=_stmttmp7B;struct Cyc_List_List*_tmp751;struct Cyc_List_List*_tmp752;struct Cyc_List_List*_tmp753;struct Cyc_List_List*_tmp754;struct Cyc_Absyn_Typedefdecl*_tmp755;struct Cyc_Absyn_Enumdecl*_tmp756;struct Cyc_Absyn_Datatypedecl*_tmp757;struct Cyc_Absyn_Aggrdecl*_tmp758;struct Cyc_Absyn_Fndecl*_tmp759;struct Cyc_Absyn_Vardecl*_tmp75A;switch(*((int*)_tmp750)){case 0U: _LL1: _tmp75A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp75A;
# 4244
struct _tuple1*_tmp75B=vd->name;struct _tuple1*c_name=_tmp75B;
# 4246
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp75C=_cycalloc(sizeof(*_tmp75C));({union Cyc_Absyn_Nmspace _tmpB07=Cyc_Absyn_Abs_n(0,1);_tmp75C->f1=_tmpB07;}),_tmp75C->f2=(*c_name).f2;_tmp75C;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4253
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4255
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpB08=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpB08;});
({void*_tmpB09=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpB09;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=d,_tmp75D->tl=Cyc_Toc_result_decls;_tmp75D;});
goto _LL0;}case 1U: _LL3: _tmp759=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp759;
# 4262
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB0B=({struct _tuple1*_tmp75E=_cycalloc(sizeof(*_tmp75E));({union Cyc_Absyn_Nmspace _tmpB0A=Cyc_Absyn_Abs_n(0,1);_tmp75E->f1=_tmpB0A;}),_tmp75E->f2=(*fd->name).f2;_tmp75E;});fd->name=_tmpB0B;});
fd->sc=Cyc_Absyn_Public;}
# 4266
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->hd=d,_tmp75F->tl=Cyc_Toc_result_decls;_tmp75F;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp761=({struct Cyc_Warn_String_Warn_Warg_struct _tmp802;_tmp802.tag=0U,({struct _fat_ptr _tmpB0C=({const char*_tmp762="letdecl at toplevel";_tag_fat(_tmp762,sizeof(char),20U);});_tmp802.f1=_tmpB0C;});_tmp802;});void*_tmp760[1U];_tmp760[0]=& _tmp761;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp760,sizeof(void*),1U));});case 4U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp764=({struct Cyc_Warn_String_Warn_Warg_struct _tmp803;_tmp803.tag=0U,({struct _fat_ptr _tmpB0D=({const char*_tmp765="region decl at toplevel";_tag_fat(_tmp765,sizeof(char),24U);});_tmp803.f1=_tmpB0D;});_tmp803;});void*_tmp763[1U];_tmp763[0]=& _tmp764;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp763,sizeof(void*),1U));});case 5U: _LLB: _tmp758=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*sd=_tmp758;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6U: _LLD: _tmp757=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LLE: {struct Cyc_Absyn_Datatypedecl*tud=_tmp757;
# 4274
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7U: _LLF: _tmp756=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp756;
# 4277
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->hd=d,_tmp766->tl=Cyc_Toc_result_decls;_tmp766;});
goto _LL0;}case 8U: _LL11: _tmp755=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp755;
# 4281
td->tvs=0;
if(td->defn != 0){
({void*_tmpB0E=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB0E;});{
# 4286
struct Cyc_Absyn_Decl*ed;
{void*_tmp767=td->defn;void*_stmttmp7C=_tmp767;void*_tmp768=_stmttmp7C;unsigned _tmp76A;struct Cyc_Absyn_Enumdecl*_tmp769;if(_tmp768 != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp768)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp768)->f1)->r)->tag == 1U){_LL24: _tmp769=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp768)->f1)->r)->f1;_tmp76A=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp768)->f1)->loc;_LL25: {struct Cyc_Absyn_Enumdecl*ed2=_tmp769;unsigned loc=_tmp76A;
# 4289
ed=({struct Cyc_Absyn_Decl*_tmp76C=_cycalloc(sizeof(*_tmp76C));({void*_tmpB0F=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp76B=_cycalloc(sizeof(*_tmp76B));_tmp76B->tag=7U,_tmp76B->f1=ed2;_tmp76B;});_tmp76C->r=_tmpB0F;}),_tmp76C->loc=loc;_tmp76C;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4292
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=ed,_tmp76D->tl=Cyc_Toc_result_decls;_tmp76D;});{
void*_tmp76E=ed->r;void*_stmttmp7D=_tmp76E;void*_tmp76F=_stmttmp7D;struct Cyc_Absyn_Enumdecl*_tmp770;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp76F)->tag == 7U){_LL29: _tmp770=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp76F)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp770;
# 4296
({void*_tmpB11=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772->tag=0U,({void*_tmpB10=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->tag=15U,_tmp771->f1=ed->name,_tmp771->f2=ed;_tmp771;});_tmp772->f1=_tmpB10;}),_tmp772->f2=0;_tmp772;});td->defn=_tmpB11;});goto _LL28;}}else{_LL2B: _LL2C:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp774=({struct Cyc_Warn_String_Warn_Warg_struct _tmp804;_tmp804.tag=0U,({struct _fat_ptr _tmpB12=({const char*_tmp775="Toc: enumdecl to name";_tag_fat(_tmp775,sizeof(char),22U);});_tmp804.f1=_tmpB12;});_tmp804;});void*_tmp773[1U];_tmp773[0]=& _tmp774;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp773,sizeof(void*),1U));});}_LL28:;}}}}else{
# 4301
enum Cyc_Absyn_KindQual _tmp776=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7E=_tmp776;enum Cyc_Absyn_KindQual _tmp777=_stmttmp7E;if(_tmp777 == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpB13=Cyc_Toc_void_star_type();td->defn=_tmpB13;});goto _LL2D;}else{_LL30: _LL31:
 td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4309
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778->hd=d,_tmp778->tl=Cyc_Toc_result_decls;_tmp778;});
goto _LL0;}case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp754=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp750)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp754;
_tmp753=ds2;goto _LL1E;}case 10U: _LL1D: _tmp753=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp750)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp753;
_tmp752=ds2;goto _LL20;}case 11U: _LL1F: _tmp752=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp752;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _LL21: _tmp751=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp750)->f1;_LL22: {struct Cyc_List_List*ds2=_tmp751;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4322
return nv;}
# 4327
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _tmp779=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp7F=_tmp779;struct Cyc_Core_NewDynamicRegion _tmp77A=_stmttmp7F;struct Cyc_Core_DynamicRegion*_tmp77B;_LL1: _tmp77B=_tmp77A.key;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp77B;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp77D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp77D));_tmp77D->dyn=dyn,_tmp77D->state=ts;_tmp77D;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp77F=34;struct _fat_ptr**_tmp77E=_cycalloc(_check_times(_tmp77F,sizeof(struct _fat_ptr*)));_tmp77E[0]=& Cyc_Toc__throw_str,_tmp77E[1]=& Cyc_Toc_setjmp_str,_tmp77E[2]=& Cyc_Toc__push_handler_str,_tmp77E[3]=& Cyc_Toc__pop_handler_str,_tmp77E[4]=& Cyc_Toc__exn_thrown_str,_tmp77E[5]=& Cyc_Toc__npop_handler_str,_tmp77E[6]=& Cyc_Toc__check_null_str,_tmp77E[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp77E[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp77E[9]=& Cyc_Toc__check_fat_subscript_str,_tmp77E[10]=& Cyc_Toc__tag_fat_str,_tmp77E[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp77E[12]=& Cyc_Toc__get_fat_size_str,_tmp77E[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp77E[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp77E[15]=& Cyc_Toc__zero_arr_plus_str,_tmp77E[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp77E[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp77E[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp77E[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp77E[20]=& Cyc_Toc__cycalloc_str,_tmp77E[21]=& Cyc_Toc__cyccalloc_str,_tmp77E[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp77E[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp77E[24]=& Cyc_Toc__region_malloc_str,_tmp77E[25]=& Cyc_Toc__region_calloc_str,_tmp77E[26]=& Cyc_Toc__check_times_str,_tmp77E[27]=& Cyc_Toc__new_region_str,_tmp77E[28]=& Cyc_Toc__push_region_str,_tmp77E[29]=& Cyc_Toc__pop_region_str,_tmp77E[30]=& Cyc_Toc__throw_arraybounds_str,_tmp77E[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp77E[32]=& Cyc_Toc__throw_match_str,_tmp77E[33]=& Cyc_Toc__fast_region_malloc_str;_tmp77E;}),sizeof(struct _fat_ptr*),34U);}}
# 4375
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp780=ts;struct Cyc_Toc_TocStateWrap*_tmp781=Cyc_Toc_toc_state;ts=_tmp781;Cyc_Toc_toc_state=_tmp780;});{
struct Cyc_Toc_TocStateWrap _tmp782=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp80=_tmp782;struct Cyc_Toc_TocStateWrap _tmp783=_stmttmp80;struct Cyc_Toc_TocState*_tmp785;struct Cyc_Core_DynamicRegion*_tmp784;_LL1: _tmp784=_tmp783.dyn;_tmp785=_tmp783.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp784;struct Cyc_Toc_TocState*s=_tmp785;
# 4380
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp787=*s;struct Cyc_Toc_TocState _stmttmp81=_tmp787;struct Cyc_Toc_TocState _tmp788=_stmttmp81;struct Cyc_Xarray_Xarray*_tmp789;_LL4: _tmp789=_tmp788.temp_labels;_LL5: {struct Cyc_Xarray_Xarray*tls=_tmp789;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(tls);}}
# 4381
;}
# 4384
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4387
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4393
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4395
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp78A=_cycalloc(sizeof(*_tmp78A));*_tmp78A=pop_tables;_tmp78A;});
Cyc_Toc_init();{
struct _RegionHandle _tmp78B=_new_region("start");struct _RegionHandle*start=& _tmp78B;_push_region(start);
({struct _RegionHandle*_tmpB15=start;struct Cyc_Toc_Env*_tmpB14=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB15,_tmpB14,ds,0);});{
struct Cyc_List_List*_tmp78C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp78C;}
# 4398
;_pop_region();}}
