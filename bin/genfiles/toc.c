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
# 1150
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1152
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1154
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1163
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1171
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
# 65
void*Cyc_Tcutil_pointer_elt_type(void*);
# 73
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 78
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 80
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 85
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 98
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 101
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 106
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 184
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 186
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 189
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 224
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 231
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);
# 293
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 304
int Cyc_Tcutil_force_type2bool(int desired,void*);struct _tuple13{unsigned f1;int f2;};
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
# 56 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 71 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp790;_tmp790.tag=0U,_tmp790.f1=str;_tmp790;});void*_tmp0[1U];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1U));});}
# 74
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 76
({struct _fat_ptr _tmp809=(struct _fat_ptr)({struct _fat_ptr _tmp808=({const char*_tmp2="Toc: ";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_strconcat(_tmp808,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp809,ap);});}
# 78
static void*Cyc_Toc_toc_impos2(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=0U,({struct _fat_ptr _tmp80A=({const char*_tmp5="Toc: ";_tag_fat(_tmp5,sizeof(char),6U);});_tmp6->f1=_tmp80A;});_tmp6;});
struct _fat_ptr args=({unsigned _tmp4=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned _tmp791=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;unsigned i;for(i=0;i < _tmp791;++ i){i == (unsigned)0?_tmp3[i]=toc:(_tmp3[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - (unsigned)1))));}}0;});_tag_fat(_tmp3,sizeof(void*),_tmp4);});
Cyc_Warn_vimpos2(args);}
# 84
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 87
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 97
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 99
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 125
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 132
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmpE=_region_malloc(d,sizeof(*_tmpE));
({struct Cyc_List_List**_tmp816=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpE->tuple_types=_tmp816;}),({
struct Cyc_List_List**_tmp815=({struct Cyc_List_List**_tmp8=_region_malloc(d,sizeof(*_tmp8));*_tmp8=0;_tmp8;});_tmpE->anon_aggr_types=_tmp815;}),({
struct Cyc_Dict_Dict*_tmp814=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));({struct Cyc_Dict_Dict _tmp813=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp9=_tmp813;});_tmp9;});_tmpE->aggrs_so_far=_tmp814;}),({
struct Cyc_List_List**_tmp812=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmpE->abs_struct_types=_tmp812;}),({
struct Cyc_Set_Set**_tmp811=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Set_Set*_tmp810=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp810;});_tmpB;});_tmpE->datatypes_so_far=_tmp811;}),({
struct Cyc_Dict_Dict*_tmp80F=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp80E=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp80E;});_tmpC;});_tmpE->xdatatypes_so_far=_tmp80F;}),({
struct Cyc_Dict_Dict*_tmp80D=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Dict_Dict _tmp80C=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmpD=_tmp80C;});_tmpD;});_tmpE->qvar_tags=_tmp80D;}),({
struct Cyc_Xarray_Xarray*_tmp80B=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpE->temp_labels=_tmp80B;});_tmpE;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
struct Cyc_Toc_TocStateWrap _tmp11=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp11;struct Cyc_Toc_TocStateWrap _tmp12=_stmttmp0;struct Cyc_Toc_TocState*_tmp14;struct Cyc_Core_DynamicRegion*_tmp13;_LL1: _tmp13=_tmp12.dyn;_tmp14=_tmp12.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp13;struct Cyc_Toc_TocState*s=_tmp14;
struct _fat_ptr _tmp15=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp15;
({struct Cyc_Toc_TocStateWrap _tmp817=({struct Cyc_Toc_TocStateWrap _tmp792;_tmp792.dyn=dyn,_tmp792.state=s;_tmp792;});*ts=_tmp817;});
({struct Cyc_Toc_TocStateWrap*_tmp16=ts;struct Cyc_Toc_TocStateWrap*_tmp17=Cyc_Toc_toc_state;ts=_tmp17;Cyc_Toc_toc_state=_tmp16;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp19=({struct _tuple15 _tmp793;_tmp793.f1=s,_tmp793.f2=arg;_tmp793;});struct _tuple15 env=_tmp19;
res=f(h,& env);}
# 168
;}
# 170
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 174
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 177
struct _tuple17 _tmp1A=*env;struct _tuple17 _stmttmp1=_tmp1A;struct _tuple17 _tmp1B=_stmttmp1;void*(*_tmp1E)(struct _tuple1*);struct _tuple1*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1C=_tmp1B.f1;_tmp1D=(_tmp1B.f2)->f1;_tmp1E=(_tmp1B.f2)->f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp1C;struct _tuple1*q=_tmp1D;void*(*type_maker)(struct _tuple1*)=_tmp1E;
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 182
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp794;_tmp794.f1=q,_tmp794.f2=type_maker;_tmp794;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1F[5U]="curr";
# 195 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1F,_tmp1F,_tmp1F + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp20[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp20,_tmp20,_tmp20 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp21[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp21,_tmp21,_tmp21 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp22[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp22,_tmp22,_tmp22 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp23[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp23,_tmp23,_tmp23 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp24[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp24,_tmp24,_tmp24 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp25[7U]="_throw";
# 214 "toc.cyc"
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
# 282
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 285
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp6F=_cycalloc(sizeof(*_tmp6F));((_tmp6F->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp81C=({struct _fat_ptr*_tmp6A[1U];({struct _fat_ptr*_tmp81B=({struct _fat_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _fat_ptr _tmp81A=({const char*_tmp6B="Core";_tag_fat(_tmp6B,sizeof(char),5U);});*_tmp6C=_tmp81A;});_tmp6C;});_tmp6A[0]=_tmp81B;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6A,sizeof(struct _fat_ptr*),1U));});((_tmp6F->f1).Abs_n).val=_tmp81C;}),({struct _fat_ptr*_tmp819=({struct _fat_ptr*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct _fat_ptr _tmp818=({const char*_tmp6D="get_exn_thrown";_tag_fat(_tmp6D,sizeof(char),15U);});*_tmp6E=_tmp818;});_tmp6E;});_tmp6F->f2=_tmp819;});_tmp6F;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=0U,_tmp69->f1=qv;_tmp69;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->topt=0,({void*_tmp81D=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=1U,_tmp67->f1=bnd;_tmp67;});_tmp68->r=_tmp81D;}),_tmp68->loc=0U,_tmp68->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp68;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=10U,_tmp66->f1=fnname,_tmp66->f2=0,_tmp66->f3=0,_tmp66->f4=0;_tmp66;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->topt=0,_tmp65->r=fncall_re,_tmp65->loc=0U,_tmp65->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp65;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 295
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 297
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_tmp820=Cyc_Absyn_void_type;void*_tmp81F=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp81E=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_star_type(_tmp820,_tmp81F,_tmp81E,Cyc_Absyn_false_type);});
return(void*)_check_null(t);}
# 303
static void*Cyc_Toc_fat_ptr_type (void){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__fat_ptr_pr,0),0);
return(void*)_check_null(t);}
# 309
static void*Cyc_Toc_rgn_type (void){
static void*r=0;
if(r == 0)
r=({void*_tmp821=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp821,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 316
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 320
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp72=_cycalloc(sizeof(*_tmp72));({struct Cyc_List_List*_tmp823=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));({void*_tmp822=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,_tmp70->f1=name;_tmp70;});_tmp71->hd=_tmp822;}),_tmp71->tl=0;_tmp71;});_tmp72->f1=_tmp823;}),_tmp72->f2=e;_tmp72;});}
# 324
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp824=f;Cyc_Absyn_fncall_exp(_tmp824,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
# 327
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp73=e->r;void*_stmttmp2=_tmp73;void*_tmp74=_stmttmp2;struct Cyc_Absyn_Exp*_tmp75;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp75=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp74)->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp75;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 335
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=14U,_tmp76->f1=t,_tmp76->f2=e,_tmp76->f3=0,_tmp76->f4=Cyc_Absyn_No_coercion;_tmp76;});}
# 338
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=20U,_tmp77->f1=e;_tmp77;});}
# 341
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=23U,_tmp78->f1=e1,_tmp78->f2=e2;_tmp78;});}
# 344
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=37U,_tmp79->f1=s;_tmp79;});}
# 347
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=17U,_tmp7A->f1=t;_tmp7A;});}
# 350
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=10U,_tmp7B->f1=e,({struct Cyc_List_List*_tmp825=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp7B->f2=_tmp825;}),_tmp7B->f3=0,_tmp7B->f4=1;_tmp7B;});}
# 353
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=2U,_tmp7C->f1=s1,_tmp7C->f2=s2;_tmp7C;});}
# 356
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=6U,_tmp7D->f1=e1,_tmp7D->f2=e2,_tmp7D->f3=e3;_tmp7D;});}
# 359
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=21U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 362
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 365
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 367
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 371
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 375
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp81=e->r;void*_stmttmp3=_tmp81;void*_tmp82=_stmttmp3;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp83;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 403 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 409
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp84=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp84;void*_tmp85=_stmttmp4;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1: _LL2:
 return fS->fchar;case Cyc_Absyn_Short_sz: _LL3: _LL4:
 return fS->fshort;case Cyc_Absyn_Int_sz: _LL5: _LL6:
 return fS->fint;default: goto _LLF;}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL7: _LL8:
 return fS->ffloat;case 1U: _LL9: _LLA:
 return fS->fdouble;default: _LLB: _LLC:
 return fS->flongdouble;}default: goto _LLF;}case 3U: _LLD: _LLE:
 return fS->fvoidstar;default: _LLF: _LL10:
# 419
({struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp797;_tmp797.tag=0U,({struct _fat_ptr _tmp826=({const char*_tmp8B="expression type ";_tag_fat(_tmp8B,sizeof(char),17U);});_tmp797.f1=_tmp826;});_tmp797;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp88=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp796;_tmp796.tag=2U,_tmp796.f1=(void*)t;_tmp796;});struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp795;_tmp795.tag=0U,({struct _fat_ptr _tmp827=({const char*_tmp8A=" (not int, float, double, or pointer)";_tag_fat(_tmp8A,sizeof(char),38U);});_tmp795.f1=_tmp827;});_tmp795;});void*_tmp86[3U];_tmp86[0]=& _tmp87,_tmp86[1]=& _tmp88,_tmp86[2]=& _tmp89;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp86,sizeof(void*),3U));});}_LL0:;}
# 422
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp8C;void*_tmp8D=_stmttmp5;void*_tmp8E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 3U){_LL1: _tmp8E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_type;_LL2: {void*et=_tmp8E;
return Cyc_Toc_getFunctionType(fS,et);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp798;_tmp798.tag=0U,({struct _fat_ptr _tmp828=({const char*_tmp91="impossible type (not pointer)";_tag_fat(_tmp91,sizeof(char),30U);});_tmp798.f1=_tmp828;});_tmp798;});void*_tmp8F[1U];_tmp8F[0]=& _tmp90;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp8F,sizeof(void*),1U));});}_LL0:;}
# 432
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_stmttmp6=_tmp92;void*_tmp93=_stmttmp6;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Exp*_tmp99;long long _tmp9A;int _tmp9B;short _tmp9C;struct _fat_ptr _tmp9D;char _tmp9E;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp9E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2: {char c=_tmp9E;
return(int)c == (int)'\000';}case 3U: _LL3: _tmp9D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp9D;
# 436
unsigned long _tmp9F=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp9F;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 446
return 0;}}case 4U: _LL5: _tmp9C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6: {short i=_tmp9C;
return(int)i == 0;}case 5U: _LL7: _tmp9B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8: {int i=_tmp9B;
return i == 0;}case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA: {long long i=_tmp9A;
return i == (long long)0;}case 1U: _LLD: _LLE:
# 451
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 450
 goto _LLE;case 14U: _LLF: _tmp99=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp99;
# 452
return Cyc_Toc_is_zero(e1);}case 24U: _LL11: _tmp98=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12: {struct Cyc_List_List*es=_tmp98;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,es);}case 26U: _LL13: _tmp97=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14: {struct Cyc_List_List*dles=_tmp97;
_tmp96=dles;goto _LL16;}case 29U: _LL15: _tmp96=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16: {struct Cyc_List_List*dles=_tmp96;
_tmp95=dles;goto _LL18;}case 25U: _LL17: _tmp95=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18: {struct Cyc_List_List*dles=_tmp95;
_tmp94=dles;goto _LL1A;}case 36U: _LL19: _tmp94=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A: {struct Cyc_List_List*dles=_tmp94;
# 458
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 466
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;struct _fat_ptr*_tmpA2;union Cyc_Absyn_Nmspace _tmpA1;_LL1: _tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmpA1;struct _fat_ptr*v=_tmpA2;
union Cyc_Absyn_Nmspace _tmpA3=ns;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA6;switch((_tmpA3.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpA6=0;goto _LL7;case 1U: _LL6: _tmpA6=(_tmpA3.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmpA6;
_tmpA5=vs;goto _LL9;}case 2U: _LL8: _tmpA5=(_tmpA3.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmpA5;
_tmpA4=vs;goto _LLB;}default: _LLA: _tmpA4=(_tmpA3.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmpA4;
# 475
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,_tmp79A.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp79A;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,_tmp799.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp799;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({struct _fat_ptr _tmp829=({const char*_tmpA8="%s_%s_struct";_tag_fat(_tmpA8,sizeof(char),13U);});Cyc_aprintf(_tmp829,_tag_fat(_tmpA7,sizeof(void*),2U));});});{
struct _RegionHandle _tmpAB=_new_region("r");struct _RegionHandle*r=& _tmpAB;_push_region(r);
{struct _fat_ptr _tmpB2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0U,_tmp79D.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp79D;});struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp79C;_tmp79C.tag=0U,({struct _fat_ptr _tmp82C=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp82B=r;struct Cyc_List_List*_tmp82A=vs;Cyc_rstr_sepstr(_tmp82B,_tmp82A,({const char*_tmpB1="_";_tag_fat(_tmpB1,sizeof(char),2U);}));}));_tmp79C.f1=_tmp82C;});_tmp79C;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,_tmp79B.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp79B;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAE,_tmpAC[1]=& _tmpAF,_tmpAC[2]=& _tmpB0;({struct _fat_ptr _tmp82D=({const char*_tmpAD="%s_%s_%s_struct";_tag_fat(_tmpAD,sizeof(char),16U);});Cyc_aprintf(_tmp82D,_tag_fat(_tmpAC,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB2;};_pop_region();}}}_LL3:;}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 488
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 491
struct _tuple20 _tmpB3=*env;struct _tuple20 _stmttmp7=_tmpB3;struct _tuple20 _tmpB4=_stmttmp7;struct _tuple14*_tmpB6;struct Cyc_Dict_Dict*_tmpB5;_LL1: _tmpB5=(_tmpB4.f1)->qvar_tags;_tmpB6=_tmpB4.f2;_LL2: {struct Cyc_Dict_Dict*qvs=_tmpB5;struct _tuple14*pair=_tmpB6;
struct _tuple14 _tmpB7=*pair;struct _tuple14 _stmttmp8=_tmpB7;struct _tuple14 _tmpB8=_stmttmp8;struct _tuple1*_tmpBA;struct _tuple1*_tmpB9;_LL4: _tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;_LL5: {struct _tuple1*fieldname=_tmpB9;struct _tuple1*dtname=_tmpBA;
struct _handler_cons _tmpBB;_push_handler(& _tmpBB);{int _tmpBD=0;if(setjmp(_tmpBB.handler))_tmpBD=1;if(!_tmpBD){{struct _tuple1*_tmpBE=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0U);return _tmpBE;};_pop_handler();}else{void*_tmpBC=(void*)Cyc_Core_get_exn_thrown();void*_tmpBF=_tmpBC;void*_tmpC0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBF)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 495
struct _tuple14*_tmpC1=({struct _tuple14*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=fieldname,_tmpC7->f2=dtname;_tmpC7;});struct _tuple14*new_pair=_tmpC1;
struct _tuple1*_tmpC2=fieldname;struct _fat_ptr*_tmpC4;union Cyc_Absyn_Nmspace _tmpC3;_LLC: _tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_LLD: {union Cyc_Absyn_Nmspace nmspace=_tmpC3;struct _fat_ptr*fieldvar=_tmpC4;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->f1=nmspace,({struct _fat_ptr*_tmp82E=({struct _fat_ptr*_tmpC5=_cycalloc(sizeof(*_tmpC5));*_tmpC5=newvar;_tmpC5;});_tmpC6->f2=_tmp82E;});_tmpC6;});
({struct Cyc_Dict_Dict _tmp82F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp82F;});
return res;}}}else{_LL9: _tmpC0=_tmpBF;_LLA: {void*exn=_tmpC0;(int)_rethrow(exn);}}_LL6:;}}}}}
# 503
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp79E;_tmp79E.f1=fieldname,_tmp79E.f2=dtname;_tmp79E;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 510
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 513
struct _tuple1*_tmp832=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp831=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp831;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp832;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp830=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->fields=fs,_tmpC9->tagged=0;_tmpC9;});_tmpCA->impl=_tmp830;});_tmpCA;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 519
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 522
struct _tuple21 _tmpCB=*env;struct _tuple21 _stmttmp9=_tmpCB;struct _tuple21 _tmpCC=_stmttmp9;struct Cyc_List_List*_tmpCE;struct Cyc_List_List**_tmpCD;_LL1: _tmpCD=(_tmpCC.f1)->tuple_types;_tmpCE=_tmpCC.f2;_LL2: {struct Cyc_List_List**tuple_types=_tmpCD;struct Cyc_List_List*tqs0=_tmpCE;
# 524
{struct Cyc_List_List*_tmpCF=*tuple_types;struct Cyc_List_List*tts=_tmpCF;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_tmpD0=(struct _tuple22*)tts->hd;struct _tuple22*_stmttmpA=_tmpD0;struct _tuple22*_tmpD1=_stmttmpA;struct Cyc_List_List*_tmpD3;void*_tmpD2;_LL4: _tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_LL5: {void*x=_tmpD2;struct Cyc_List_List*ts=_tmpD3;
struct Cyc_List_List*_tmpD4=tqs0;struct Cyc_List_List*tqs=_tmpD4;
for(0;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 535
struct _fat_ptr*xname=({struct _fat_ptr*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct _fat_ptr _tmp834=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE3=({struct Cyc_Int_pa_PrintArg_struct _tmp79F;_tmp79F.tag=1U,_tmp79F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79F;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE3;({struct _fat_ptr _tmp833=({const char*_tmpE2="_tuple%d";_tag_fat(_tmpE2,sizeof(char),9U);});Cyc_aprintf(_tmp833,_tag_fat(_tmpE1,sizeof(void*),1U));});});*_tmpE4=_tmp834;});_tmpE4;});
struct Cyc_List_List*_tmpD5=0;struct Cyc_List_List*fs=_tmpD5;
struct Cyc_List_List*_tmpD6=0;struct Cyc_List_List*ts=_tmpD6;
{int i=1;for(0;tqs0 != 0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f2;
fs=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_Absyn_Aggrfield*_tmp836=({struct Cyc_Absyn_Aggrfield*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct _fat_ptr*_tmp835=Cyc_Absyn_fieldname(i);_tmpD7->name=_tmp835;}),_tmpD7->tq=Cyc_Toc_mt_tq,_tmpD7->type=t,_tmpD7->width=0,_tmpD7->attributes=0,_tmpD7->requires_clause=0;_tmpD7;});_tmpD8->hd=_tmp836;}),_tmpD8->tl=fs;_tmpD8;});
ts=({struct Cyc_List_List*_tmpD9=_region_malloc(d,sizeof(*_tmpD9));_tmpD9->hd=t,_tmpD9->tl=ts;_tmpD9;});}}
# 543
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);
ts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts);{
struct Cyc_Absyn_Aggrdecl*_tmpDA=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpDA;
void*_tmpDB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE0=_cycalloc(sizeof(*_tmpE0));*_tmpE0=sd;_tmpE0;})),0);void*ans=_tmpDB;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_Absyn_Decl*_tmp837=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->tag=5U,_tmpDC->f1=sd;_tmpDC;}),0U);_tmpDD->hd=_tmp837;}),_tmpDD->tl=Cyc_Toc_result_decls;_tmpDD;});
({struct Cyc_List_List*_tmp839=({struct Cyc_List_List*_tmpDF=_region_malloc(d,sizeof(*_tmpDF));({struct _tuple22*_tmp838=({struct _tuple22*_tmpDE=_region_malloc(d,sizeof(*_tmpDE));_tmpDE->f1=ans,_tmpDE->f2=ts;_tmpDE;});_tmpDF->hd=_tmp838;}),_tmpDF->tl=*tuple_types;_tmpDF;});*tuple_types=_tmp839;});
return ans;}}}}
# 551
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 556
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 559
struct _tuple25*_tmpE5=env;struct Cyc_List_List*_tmpE8;enum Cyc_Absyn_AggrKind _tmpE7;struct Cyc_List_List**_tmpE6;_LL1: _tmpE6=(_tmpE5->f1)->anon_aggr_types;_tmpE7=(_tmpE5->f2)->f1;_tmpE8=(_tmpE5->f2)->f2;_LL2: {struct Cyc_List_List**anon_aggr_types=_tmpE6;enum Cyc_Absyn_AggrKind ak=_tmpE7;struct Cyc_List_List*fs=_tmpE8;
# 561
{struct Cyc_List_List*_tmpE9=*anon_aggr_types;struct Cyc_List_List*ts=_tmpE9;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpEA=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpEA;struct _tuple26*_tmpEB=_stmttmpB;struct Cyc_List_List*_tmpEE;enum Cyc_Absyn_AggrKind _tmpED;void*_tmpEC;_LL4: _tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;_tmpEE=_tmpEB->f3;_LL5: {void*x=_tmpEC;enum Cyc_Absyn_AggrKind ak2=_tmpED;struct Cyc_List_List*fs2=_tmpEE;
if((int)ak != (int)ak2)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 570
struct _fat_ptr*xname=({struct _fat_ptr*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct _fat_ptr _tmp83B=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF8=({struct Cyc_Int_pa_PrintArg_struct _tmp7A0;_tmp7A0.tag=1U,_tmp7A0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A0;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct _fat_ptr _tmp83A=({const char*_tmpF7="_tuple%d";_tag_fat(_tmpF7,sizeof(char),9U);});Cyc_aprintf(_tmp83A,_tag_fat(_tmpF6,sizeof(void*),1U));});});*_tmpF9=_tmp83B;});_tmpF9;});
struct Cyc_Absyn_Aggrdecl*_tmpEF=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpEF;
sd->kind=ak;{
void*_tmpF0=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF5=_cycalloc(sizeof(*_tmpF5));*_tmpF5=sd;_tmpF5;})),0);void*ans=_tmpF0;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF2=_cycalloc(sizeof(*_tmpF2));({struct Cyc_Absyn_Decl*_tmp83C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->tag=5U,_tmpF1->f1=sd;_tmpF1;}),0U);_tmpF2->hd=_tmp83C;}),_tmpF2->tl=Cyc_Toc_result_decls;_tmpF2;});
({struct Cyc_List_List*_tmp83E=({struct Cyc_List_List*_tmpF4=_region_malloc(d,sizeof(*_tmpF4));({struct _tuple26*_tmp83D=({struct _tuple26*_tmpF3=_region_malloc(d,sizeof(*_tmpF3));_tmpF3->f1=ans,_tmpF3->f2=ak,_tmpF3->f3=fs;_tmpF3;});_tmpF4->hd=_tmp83D;}),_tmpF4->tl=*anon_aggr_types;_tmpF4;});*anon_aggr_types=_tmp83E;});
return ans;}}}}
# 578
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->f1=ak,_tmpFA->f2=fs;_tmpFA;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 587
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 596
struct _tuple28 _tmpFB=*env;struct _tuple28 _stmttmpC=_tmpFB;struct _tuple28 _tmpFC=_stmttmpC;struct Cyc_List_List*_tmp101;struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmpFF;struct _tuple1*_tmpFE;struct Cyc_List_List**_tmpFD;_LL1: _tmpFD=(_tmpFC.f1)->abs_struct_types;_tmpFE=(_tmpFC.f2)->f1;_tmpFF=(_tmpFC.f2)->f2;_tmp100=(_tmpFC.f2)->f3;_tmp101=(_tmpFC.f2)->f4;_LL2: {struct Cyc_List_List**abs_struct_types=_tmpFD;struct _tuple1*struct_name=_tmpFE;struct Cyc_List_List*type_vars=_tmpFF;struct Cyc_List_List*type_args=_tmp100;struct Cyc_List_List*fields=_tmp101;
# 600
{struct Cyc_List_List*_tmp102=*abs_struct_types;struct Cyc_List_List*tts=_tmp102;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmp103=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmp103;struct _tuple29*_tmp104=_stmttmpD;void*_tmp107;struct Cyc_List_List*_tmp106;struct _tuple1*_tmp105;_LL4: _tmp105=_tmp104->f1;_tmp106=_tmp104->f2;_tmp107=_tmp104->f3;_LL5: {struct _tuple1*x=_tmp105;struct Cyc_List_List*ts2=_tmp106;void*t=_tmp107;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp83F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args);_tmp83F == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmp108=type_args;struct Cyc_List_List*ts=_tmp108;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmp109=(void*)ts->hd;void*t=_tmp109;
void*_tmp10A=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmp10A;
{struct Cyc_Absyn_Kind*_tmp10B=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp10B;struct Cyc_Absyn_Kind*_tmp10C=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp10C)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 613
 if(Cyc_Unify_unify(t,t2)||({void*_tmp840=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp840,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 618
break;}}
# 620
if(okay)
return t;}}}}{
# 627
struct _fat_ptr*xname=({struct _fat_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct _fat_ptr _tmp842=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11E=({struct Cyc_Int_pa_PrintArg_struct _tmp7A1;_tmp7A1.tag=1U,_tmp7A1.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A1;});void*_tmp11C[1U];_tmp11C[0]=& _tmp11E;({struct _fat_ptr _tmp841=({const char*_tmp11D="_tuple%d";_tag_fat(_tmp11D,sizeof(char),9U);});Cyc_aprintf(_tmp841,_tag_fat(_tmp11C,sizeof(void*),1U));});});*_tmp11F=_tmp842;});_tmp11F;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp10D=0;struct Cyc_List_List*fs=_tmp10D;
# 631
({struct Cyc_List_List*_tmp844=({struct Cyc_List_List*_tmp10F=_region_malloc(d,sizeof(*_tmp10F));({struct _tuple29*_tmp843=({struct _tuple29*_tmp10E=_region_malloc(d,sizeof(*_tmp10E));_tmp10E->f1=struct_name,_tmp10E->f2=type_args,_tmp10E->f3=x;_tmp10E;});_tmp10F->hd=_tmp843;}),_tmp10F->tl=*abs_struct_types;_tmp10F;});*abs_struct_types=_tmp844;});{
# 634
struct _RegionHandle _tmp110=_new_region("r");struct _RegionHandle*r=& _tmp110;_push_region(r);
{struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp111;
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp112=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp112;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 644
if((fields->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));({void*_tmp845=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=6U,_tmp113->f1=0;_tmp113;});_tmp114->hd=_tmp845;}),_tmp114->tl=atts;_tmp114;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 651
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=4U,({void*_tmp848=Cyc_Toc_void_star_type();(_tmp115->f1).elt_type=_tmp848;}),({struct Cyc_Absyn_Tqual _tmp847=Cyc_Absyn_empty_tqual(0U);(_tmp115->f1).tq=_tmp847;}),({
struct Cyc_Absyn_Exp*_tmp846=Cyc_Absyn_uint_exp(0U,0U);(_tmp115->f1).num_elts=_tmp846;}),(_tmp115->f1).zero_term=Cyc_Absyn_false_type,(_tmp115->f1).zt_loc=0U;_tmp115;});
# 655
fs=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Aggrfield*_tmp849=({struct Cyc_Absyn_Aggrfield*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->name=f->name,_tmp116->tq=Cyc_Toc_mt_tq,_tmp116->type=t,_tmp116->width=f->width,_tmp116->attributes=atts,_tmp116->requires_clause=0;_tmp116;});_tmp117->hd=_tmp849;}),_tmp117->tl=fs;_tmp117;});}
# 657
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp118=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp118;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct Cyc_Absyn_Decl*_tmp84A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=5U,_tmp119->f1=sd;_tmp119;}),0U);_tmp11A->hd=_tmp84A;}),_tmp11A->tl=Cyc_Toc_result_decls;_tmp11A;});{
void*_tmp11B=x;_npop_handler(0U);return _tmp11B;}}}
# 635
;_pop_region();}}}}
# 663
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 667
struct _tuple27 env=({struct _tuple27 _tmp7A2;_tmp7A2.f1=struct_name,_tmp7A2.f2=type_vars,_tmp7A2.f3=type_args,_tmp7A2.f4=fields;_tmp7A2;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 675
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp84D=({struct _fat_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _fat_ptr _tmp84C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122=({struct Cyc_Int_pa_PrintArg_struct _tmp7A3;_tmp7A3.tag=1U,_tmp7A3.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp7A3;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _fat_ptr _tmp84B=({const char*_tmp121="_tmp%X";_tag_fat(_tmp121,sizeof(char),7U);});Cyc_aprintf(_tmp84B,_tag_fat(_tmp120,sizeof(void*),1U));});});*_tmp123=_tmp84C;});_tmp123;});_tmp124->f2=_tmp84D;});_tmp124;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 678
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp7A4;_tmp7A4.f1=v,({struct Cyc_Absyn_Exp*_tmp84E=Cyc_Absyn_var_exp(v,0U);_tmp7A4.f2=_tmp84E;});_tmp7A4;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 685
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp125=*env;struct _tuple31 _stmttmpF=_tmp125;struct _tuple31 _tmp126=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp127;_LL1: _tmp127=(_tmp126.f1)->temp_labels;_LL2: {struct Cyc_Xarray_Xarray*temp_labels=_tmp127;
int _tmp128=Cyc_Toc_fresh_label_counter ++;int i=_tmp128;
if(({int _tmp84F=i;_tmp84F < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct _fat_ptr _tmp851=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12E=({struct Cyc_Int_pa_PrintArg_struct _tmp7A6;_tmp7A6.tag=1U,_tmp7A6.f1=(unsigned)i;_tmp7A6;});void*_tmp12C[1U];_tmp12C[0]=& _tmp12E;({struct _fat_ptr _tmp850=({const char*_tmp12D="_LL%X";_tag_fat(_tmp12D,sizeof(char),6U);});Cyc_aprintf(_tmp850,_tag_fat(_tmp12C,sizeof(void*),1U));});});*_tmp12F=_tmp851;});_tmp12F;});
if(({int _tmp852=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp852 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A5;_tmp7A5.tag=0U,({struct _fat_ptr _tmp853=({const char*_tmp12B="fresh_label: add_ind returned bad index...";_tag_fat(_tmp12B,sizeof(char),43U);});_tmp7A5.f1=_tmp853;});_tmp7A5;});void*_tmp129[1U];_tmp129[0]=& _tmp12A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp129,sizeof(void*),1U));});
return res;}}}
# 695
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 701
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp130;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 708
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 714
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->f1=(*a).f1,_tmp131->f2=(*a).f2,({void*_tmp854=Cyc_Toc_typ_to_c((*a).f3);_tmp131->f3=_tmp854;});_tmp131;});}
# 736 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp132=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp132;void*_tmp133=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp134;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->tag == 4U){_LL1: _tmp134=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp133)->f1;_LL2: {struct Cyc_Absyn_ArrayInfo ai=_tmp134;
return({void*_tmp855=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp855,ai.tq);});}}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 743
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 745
struct Cyc_Absyn_Aggrfield*_tmp135=({struct Cyc_Absyn_Aggrfield*_tmp136=_cycalloc(sizeof(*_tmp136));*_tmp136=*f;_tmp136;});struct Cyc_Absyn_Aggrfield*ans=_tmp135;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 752
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 754
return;}
# 757
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp137;void*_tmp138=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp139;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp138)->tag == 2U){_LL1: _tmp139=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp138)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp139;
return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 763
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp13A=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp13A;struct Cyc_Absyn_Kind*_tmp13B=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp13B)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 769
static int Cyc_Toc_is_void_star(void*t){
void*_tmp13C=Cyc_Tcutil_compress(t);void*_stmttmp13=_tmp13C;void*_tmp13D=_stmttmp13;void*_tmp13E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->tag == 3U){_LL1: _tmp13E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).elt_type;_LL2: {void*t2=_tmp13E;
return Cyc_Tcutil_is_void_type(t2);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 775
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 778
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 782
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e);
# 784
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp13F=t;void**_tmp141;struct Cyc_Absyn_Datatypedecl*_tmp140;struct Cyc_Absyn_Enumdecl*_tmp142;struct Cyc_Absyn_Aggrdecl*_tmp143;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp145;void*_tmp149;struct Cyc_Absyn_Typedefdecl*_tmp148;struct Cyc_List_List*_tmp147;struct _tuple1*_tmp146;struct Cyc_List_List*_tmp14B;enum Cyc_Absyn_AggrKind _tmp14A;struct Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_VarargInfo*_tmp151;int _tmp150;struct Cyc_List_List*_tmp14F;void*_tmp14E;struct Cyc_Absyn_Tqual _tmp14D;unsigned _tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Tqual _tmp154;void*_tmp153;void*_tmp159;struct Cyc_Absyn_Tqual _tmp158;void*_tmp157;struct Cyc_Absyn_Tvar*_tmp15A;void**_tmp15B;struct Cyc_List_List*_tmp15C;struct _tuple1*_tmp15D;struct Cyc_List_List*_tmp160;union Cyc_Absyn_AggrInfo _tmp15F;void*_tmp15E;struct Cyc_Absyn_Datatypefield*_tmp162;struct Cyc_Absyn_Datatypedecl*_tmp161;switch(*((int*)_tmp13F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 797
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp161=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1).KnownDatatypefield).val).f1;_tmp162=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1).KnownDatatypefield).val).f2;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmp161;struct Cyc_Absyn_Datatypefield*tuf=_tmp162;
# 799
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{_LLB: _LLC:
# 801
({struct Cyc_Warn_String_Warn_Warg_struct _tmp164=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A7;_tmp7A7.tag=0U,({struct _fat_ptr _tmp856=({const char*_tmp165="unresolved DatatypeFieldType";_tag_fat(_tmp165,sizeof(char),29U);});_tmp7A7.f1=_tmp856;});_tmp7A7;});void*_tmp163[1U];_tmp163[0]=& _tmp164;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp163,sizeof(void*),1U));});}case 1U: _LLF: _LL10:
# 810
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp15E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1;_tmp15F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1;_tmp160=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f2;_LL1C: {void*c=_tmp15E;union Cyc_Absyn_AggrInfo info=_tmp15F;struct Cyc_List_List*ts=_tmp160;
# 857
{union Cyc_Absyn_AggrInfo _tmp175=info;if((_tmp175.UnknownAggr).tag == 1){_LL45: _LL46:
# 859
 if(ts == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=0U,_tmp176->f1=c,_tmp176->f2=0;_tmp176;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 863
struct Cyc_Absyn_Aggrdecl*_tmp177=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp177;
if(ad->expected_mem_kind){
# 866
if(ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp179=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7A9;_tmp7A9.tag=6U,_tmp7A9.f1=ad;_tmp7A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A8;_tmp7A8.tag=0U,({struct _fat_ptr _tmp857=({const char*_tmp17B=" was never defined.";_tag_fat(_tmp17B,sizeof(char),20U);});_tmp7A8.f1=_tmp857;});_tmp7A8;});void*_tmp178[2U];_tmp178[0]=& _tmp179,_tmp178[1]=& _tmp17A;Cyc_Warn_warn2(0U,_tag_fat(_tmp178,sizeof(void*),2U));});}
# 870
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp17C=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp17C;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp17D=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp17D;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7AB;_tmp7AB.tag=6U,_tmp7AB.f1=ad;_tmp7AB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AA;_tmp7AA.tag=0U,({struct _fat_ptr _tmp858=({const char*_tmp181=" ended up being abstract.";_tag_fat(_tmp181,sizeof(char),26U);});_tmp7AA.f1=_tmp858;});_tmp7AA;});void*_tmp17E[2U];_tmp17E[0]=& _tmp17F,_tmp17E[1]=& _tmp180;Cyc_Warn_warn2(0U,_tag_fat(_tmp17E,sizeof(void*),2U));});{
# 882
struct _RegionHandle _tmp182=_new_region("r");struct _RegionHandle*r=& _tmp182;_push_region(r);
{struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp183;
void*_tmp184=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp184;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp185=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp185;}{
void*_tmp186=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0U);return _tmp186;}}
# 883
;_pop_region();}}
# 888
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15U: _LL1D: _tmp15D=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1;_LL1E: {struct _tuple1*tdn=_tmp15D;
return t;}case 16U: _LL1F: _tmp15C=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13F)->f1)->f1;_LL20: {struct Cyc_List_List*fs=_tmp15C;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4U: _LL23: _LL24:
# 900
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 905
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp15B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13F)->f2;_LL4: {void**t2=_tmp15B;
# 788
if(*t2 == 0){
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 792
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));}case 2U: _LL5: _tmp15A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13F)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp15A;
# 794
if((int)(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;else{
return Cyc_Toc_void_star_type();}}case 3U: _LLD: _tmp157=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).elt_type;_tmp158=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).elt_tq;_tmp159=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).ptr_atts).bounds;_LLE: {void*t2=_tmp157;struct Cyc_Absyn_Tqual tq=_tmp158;void*bnds=_tmp159;
# 805
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 809
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}}case 4U: _LL13: _tmp153=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).elt_type;_tmp154=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).tq;_tmp155=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).num_elts;_tmp156=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->f1).zt_loc;_LL14: {void*t2=_tmp153;struct Cyc_Absyn_Tqual tq=_tmp154;struct Cyc_Absyn_Exp*e=_tmp155;unsigned ztl=_tmp156;
# 813
return({void*_tmp85C=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp85B=tq;struct Cyc_Absyn_Exp*_tmp85A=e;void*_tmp859=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp85C,_tmp85B,_tmp85A,_tmp859,ztl);});}case 5U: _LL15: _tmp14D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).ret_tqual;_tmp14E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).ret_type;_tmp14F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).args;_tmp150=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).c_varargs;_tmp151=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).cyc_varargs;_tmp152=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13F)->f1).attributes;_LL16: {struct Cyc_Absyn_Tqual tq2=_tmp14D;void*t2=_tmp14E;struct Cyc_List_List*args=_tmp14F;int c_vararg=_tmp150;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp151;struct Cyc_List_List*atts=_tmp152;
# 819
struct Cyc_List_List*_tmp166=0;struct Cyc_List_List*new_atts=_tmp166;
for(0;atts != 0;atts=atts->tl){
void*_tmp167=(void*)atts->hd;void*_stmttmp14=_tmp167;void*_tmp168=_stmttmp14;switch(*((int*)_tmp168)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
 new_atts=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=(void*)atts->hd,_tmp169->tl=new_atts;_tmp169;});goto _LL35;}_LL35:;}{
# 830
struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp16A;
if(cyc_vararg != 0){
# 833
void*_tmp16B=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));void*t=_tmp16B;
struct _tuple9*_tmp16C=({struct _tuple9*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=cyc_vararg->name,_tmp16E->f2=cyc_vararg->tq,_tmp16E->f3=t;_tmp16E;});struct _tuple9*vararg=_tmp16C;
new_args=({struct Cyc_List_List*_tmp85D=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp85D,({struct Cyc_List_List*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->hd=vararg,_tmp16D->tl=0;_tmp16D;}));});}
# 837
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=5U,(_tmp16F->f1).tvars=0,(_tmp16F->f1).effect=0,(_tmp16F->f1).ret_tqual=tq2,({void*_tmp85E=Cyc_Toc_typ_to_c(t2);(_tmp16F->f1).ret_type=_tmp85E;}),(_tmp16F->f1).args=new_args,(_tmp16F->f1).c_varargs=c_vararg,(_tmp16F->f1).cyc_varargs=0,(_tmp16F->f1).rgn_po=0,(_tmp16F->f1).attributes=new_atts,(_tmp16F->f1).requires_clause=0,(_tmp16F->f1).requires_relns=0,(_tmp16F->f1).ensures_clause=0,(_tmp16F->f1).ensures_relns=0,(_tmp16F->f1).return_value=0;_tmp16F;});}}case 6U: _LL17: _tmp14C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13F)->f1;_LL18: {struct Cyc_List_List*tqs=_tmp14C;
# 842
struct Cyc_List_List*_tmp170=0;struct Cyc_List_List*tqs2=_tmp170;
for(0;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp172=_cycalloc(sizeof(*_tmp172));({struct _tuple23*_tmp860=({struct _tuple23*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->f1=(*((struct _tuple23*)tqs->hd)).f1,({void*_tmp85F=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqs->hd)).f2);_tmp171->f2=_tmp85F;});_tmp171;});_tmp172->hd=_tmp860;}),_tmp172->tl=tqs2;_tmp172;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tqs2));}case 7U: _LL19: _tmp14A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13F)->f1;_tmp14B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13F)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp14A;struct Cyc_List_List*fs=_tmp14B;
# 851
struct Cyc_List_List*_tmp173=0;struct Cyc_List_List*fs2=_tmp173;
for(0;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp174=_cycalloc(sizeof(*_tmp174));({struct Cyc_Absyn_Aggrfield*_tmp862=({struct Cyc_Absyn_Aggrfield*_tmp861=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp861,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp174->hd=_tmp862;}),_tmp174->tl=fs2;_tmp174;});}
return({enum Cyc_Absyn_AggrKind _tmp863=k;Cyc_Toc_add_anon_aggr_type(_tmp863,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs2));});}case 8U: _LL21: _tmp146=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f1;_tmp147=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f2;_tmp148=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f3;_tmp149=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13F)->f4;_LL22: {struct _tuple1*tdn=_tmp146;struct Cyc_List_List*ts=_tmp147;struct Cyc_Absyn_Typedefdecl*td=_tmp148;void*topt=_tmp149;
# 894
if(topt == 0){
if(ts != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->tag=8U,_tmp187->f1=tdn,_tmp187->f2=0,_tmp187->f3=td,_tmp187->f4=0;_tmp187;});else{
return t;}}else{
# 899
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->tag=8U,_tmp188->f1=tdn,_tmp188->f2=0,_tmp188->f3=td,({void*_tmp864=Cyc_Toc_typ_to_c(topt);_tmp188->f4=_tmp864;});_tmp188;});}}case 9U: _LL2B: _tmp145=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp13F)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp145;
# 910
return t;}case 11U: _LL2D: _tmp144=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp13F)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp144;
# 915
Cyc_Toc_exptypes_to_c(e);
if(e->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)){case 0U: _LL2F: _tmp143=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)->f1;_LL30: {struct Cyc_Absyn_Aggrdecl*ad=_tmp143;
# 920
Cyc_Toc_aggrdecl_to_c(ad);
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp142=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)->f1;_LL32: {struct Cyc_Absyn_Enumdecl*ed=_tmp142;
# 925
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _LL33: _tmp140=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f1)->r)->f1;_tmp141=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13F)->f2;_LL34: {struct Cyc_Absyn_Datatypedecl*dd=_tmp140;void**t=_tmp141;
# 928
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}}_LL0:;}
# 933
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp189=t;struct Cyc_Absyn_Tqual _tmp18B;void*_tmp18A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189)->tag == 4U){_LL1: _tmp18A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189)->f1).elt_type;_tmp18B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189)->f1).tq;_LL2: {void*t2=_tmp18A;struct Cyc_Absyn_Tqual tq=_tmp18B;
# 936
return({void*_tmp865=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp865,e);});}}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 943
static int Cyc_Toc_atomic_type(void*t){
void*_tmp18C=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp18C;void*_tmp18D=_stmttmp15;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;void*_tmp190;struct Cyc_List_List*_tmp192;void*_tmp191;switch(*((int*)_tmp18D)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp191=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f1;_tmp192=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f2;_LL4: {void*c=_tmp191;struct Cyc_List_List*ts=_tmp192;
# 947
void*_tmp193=c;struct Cyc_Absyn_Datatypefield*_tmp195;struct Cyc_Absyn_Datatypedecl*_tmp194;union Cyc_Absyn_AggrInfo _tmp196;switch(*((int*)_tmp193)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp196=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp193)->f1;_LL25: {union Cyc_Absyn_AggrInfo info=_tmp196;
# 954
{union Cyc_Absyn_AggrInfo _tmp197=info;if((_tmp197.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 958
struct Cyc_Absyn_Aggrdecl*_tmp198=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp198;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp199=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp199;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp194=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f1;_tmp195=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f2;_LL27: {struct Cyc_Absyn_Datatypedecl*tud=_tmp194;struct Cyc_Absyn_Datatypefield*tuf=_tmp195;
# 965
{struct Cyc_List_List*_tmp19A=tuf->typs;struct Cyc_List_List*tqs=_tmp19A;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AD;_tmp7AD.tag=0U,({struct _fat_ptr _tmp866=({const char*_tmp19E="atomic_typ: bad type ";_tag_fat(_tmp19E,sizeof(char),22U);});_tmp7AD.f1=_tmp866;});_tmp7AD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AC;_tmp7AC.tag=2U,_tmp7AC.f1=(void*)t;_tmp7AC;});void*_tmp19B[2U];_tmp19B[0]=& _tmp19C,_tmp19B[1]=& _tmp19D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp19B,sizeof(void*),2U));});}_LL11:;}case 5U: _LL5: _LL6:
# 970
 return 1;case 4U: _LL7: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18D)->f1).elt_type;_LL8: {void*t=_tmp190;
return Cyc_Toc_atomic_type(t);}case 7U: _LL9: _tmp18F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18D)->f2;_LLA: {struct Cyc_List_List*fs=_tmp18F;
# 978
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6U: _LLB: _tmp18E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18D)->f1;_LLC: {struct Cyc_List_List*tqs=_tmp18E;
# 982
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}
return 1;}case 3U: _LLD: _LLE:
# 987
 return 0;default: _LLF: _LL10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AF;_tmp7AF.tag=0U,({struct _fat_ptr _tmp867=({const char*_tmp1A2="atomic_typ:  bad type ";_tag_fat(_tmp1A2,sizeof(char),23U);});_tmp7AF.f1=_tmp867;});_tmp7AF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AE;_tmp7AE.tag=2U,_tmp7AE.f1=(void*)t;_tmp7AE;});void*_tmp19F[2U];_tmp19F[0]=& _tmp1A0,_tmp19F[1]=& _tmp1A1;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp19F,sizeof(void*),2U));});}_LL0:;}
# 993
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp1A3=Cyc_Tcutil_compress(t);void*_stmttmp16=_tmp1A3;void*_tmp1A4=_stmttmp16;struct Cyc_List_List*_tmp1A5;union Cyc_Absyn_AggrInfo _tmp1A6;switch(*((int*)_tmp1A4)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)->tag == 20U){_LL1: _tmp1A6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp1A6;
# 996
struct Cyc_Absyn_Aggrdecl*_tmp1A7=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A7;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B0;_tmp7B0.tag=0U,({struct _fat_ptr _tmp868=({const char*_tmp1AA="is_poly_field: type missing fields";_tag_fat(_tmp1AA,sizeof(char),35U);});_tmp7B0.f1=_tmp868;});_tmp7B0;});void*_tmp1A8[1U];_tmp1A8[0]=& _tmp1A9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A8,sizeof(void*),1U));});
_tmp1A5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A4)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A5;
# 1001
struct Cyc_Absyn_Aggrfield*_tmp1AB=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp1AB;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B2;_tmp7B2.tag=0U,({struct _fat_ptr _tmp869=({const char*_tmp1AF="is_poly_field: bad field ";_tag_fat(_tmp1AF,sizeof(char),26U);});_tmp7B2.f1=_tmp869;});_tmp7B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B1;_tmp7B1.tag=0U,_tmp7B1.f1=*f;_tmp7B1;});void*_tmp1AC[2U];_tmp1AC[0]=& _tmp1AD,_tmp1AC[1]=& _tmp1AE;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AC,sizeof(void*),2U));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B4;_tmp7B4.tag=0U,({struct _fat_ptr _tmp86A=({const char*_tmp1B3="is_poly_field: bad type ";_tag_fat(_tmp1B3,sizeof(char),25U);});_tmp7B4.f1=_tmp86A;});_tmp7B4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1B2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B3;_tmp7B3.tag=2U,_tmp7B3.f1=(void*)t;_tmp7B3;});void*_tmp1B0[2U];_tmp1B0[0]=& _tmp1B1,_tmp1B0[1]=& _tmp1B2;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1B0,sizeof(void*),2U));});}_LL0:;}
# 1012
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B4=e->r;void*_stmttmp17=_tmp1B4;void*_tmp1B5=_stmttmp17;struct _fat_ptr*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct _fat_ptr*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;switch(*((int*)_tmp1B5)){case 21U: _LL1: _tmp1B8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B5)->f1;_tmp1B9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B5)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp1B8;struct _fat_ptr*f=_tmp1B9;
# 1015
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22U: _LL3: _tmp1B6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B5)->f1;_tmp1B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B5)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1B6;struct _fat_ptr*f=_tmp1B7;
# 1018
void*_tmp1BA=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp18=_tmp1BA;void*_tmp1BB=_stmttmp18;void*_tmp1BC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BB)->tag == 3U){_LL8: _tmp1BC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BB)->f1).elt_type;_LL9: {void*t=_tmp1BC;
# 1020
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B6;_tmp7B6.tag=0U,({struct _fat_ptr _tmp86B=({const char*_tmp1C0="is_poly_project: bad type ";_tag_fat(_tmp1C0,sizeof(char),27U);});_tmp7B6.f1=_tmp86B;});_tmp7B6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B5;_tmp7B5.tag=2U,_tmp7B5.f1=(void*)_check_null(e1->topt);_tmp7B5;});void*_tmp1BD[2U];_tmp1BD[0]=& _tmp1BE,_tmp1BD[1]=& _tmp1BF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1BD,sizeof(void*),2U));});}_LL7:;}default: _LL5: _LL6:
# 1023
 return 0;}_LL0:;}
# 1028
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C1[1U];_tmp1C1[0]=s;({struct Cyc_Absyn_Exp*_tmp86C=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86C,_tag_fat(_tmp1C1,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1033
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C2[2U];_tmp1C2[0]=s,_tmp1C2[1]=n;({struct Cyc_Absyn_Exp*_tmp86D=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86D,_tag_fat(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1038
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C3[2U];_tmp1C3[0]=rgn,_tmp1C3[1]=s;({struct Cyc_Absyn_Exp*_tmp86E=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp86E,_tag_fat(_tmp1C3,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1042
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C4[2U];_tmp1C4[0]=rgn,_tmp1C4[1]=s;({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp86F,_tag_fat(_tmp1C4,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1046
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C5[3U];_tmp1C5[0]=rgn,_tmp1C5[1]=s,_tmp1C5[2]=n;({struct Cyc_Absyn_Exp*_tmp870=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp870,_tag_fat(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1050
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(({void*_tmp1C6=0U;({struct Cyc_Absyn_Exp*_tmp871=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp871,_tag_fat(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1055
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1063
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C7=e->r;void*_stmttmp19=_tmp1C7;void*_tmp1C8=_stmttmp19;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C8)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C8)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1068
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({struct Cyc_Absyn_Decl*_tmp872=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->tag=0U,_tmp1C9->f1=vd;_tmp1C9;}),0U);_tmp1CA->hd=_tmp872;}),_tmp1CA->tl=Cyc_Toc_result_decls;_tmp1CA;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1076
 xexp=({void*_tmp873=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp873,e);});
# 1078
xplussz=({void*_tmp874=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp874,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1081
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1CB[3U];({struct _tuple19*_tmp877=({struct _tuple19*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=0,_tmp1CC->f2=xexp;_tmp1CC;});_tmp1CB[0]=_tmp877;}),({
struct _tuple19*_tmp876=({struct _tuple19*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=0,_tmp1CD->f2=xexp;_tmp1CD;});_tmp1CB[1]=_tmp876;}),({
struct _tuple19*_tmp875=({struct _tuple19*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->f1=0,_tmp1CE->f2=xplussz;_tmp1CE;});_tmp1CB[2]=_tmp875;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CB,sizeof(struct _tuple19*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1116 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));_tmp1D0->break_lab=0,_tmp1D0->continue_lab=0,_tmp1D0->fallthru_info=0,_tmp1D0->toplevel=1,({int*_tmp878=({int*_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));*_tmp1CF=0;_tmp1CF;});_tmp1D0->in_lhs=_tmp878;}),_tmp1D0->rgn=r;_tmp1D0;});}
# 1120
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D1=nv;int _tmp1D2;_LL1: _tmp1D2=_tmp1D1->toplevel;_LL2: {int t=_tmp1D2;
return t;}}
# 1124
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D3=e;int*_tmp1D8;int _tmp1D7;struct Cyc_Toc_FallthruInfo*_tmp1D6;struct _fat_ptr**_tmp1D5;struct _fat_ptr**_tmp1D4;_LL1: _tmp1D4=_tmp1D3->break_lab;_tmp1D5=_tmp1D3->continue_lab;_tmp1D6=_tmp1D3->fallthru_info;_tmp1D7=_tmp1D3->toplevel;_tmp1D8=_tmp1D3->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D4;struct _fat_ptr**c=_tmp1D5;struct Cyc_Toc_FallthruInfo*f=_tmp1D6;int t=_tmp1D7;int*lhs=_tmp1D8;
return({struct Cyc_Toc_Env*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));_tmp1D9->break_lab=b,_tmp1D9->continue_lab=c,_tmp1D9->fallthru_info=f,_tmp1D9->toplevel=0,_tmp1D9->in_lhs=lhs,_tmp1D9->rgn=r;_tmp1D9;});}}
# 1128
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DA=e;int*_tmp1DF;int _tmp1DE;struct Cyc_Toc_FallthruInfo*_tmp1DD;struct _fat_ptr**_tmp1DC;struct _fat_ptr**_tmp1DB;_LL1: _tmp1DB=_tmp1DA->break_lab;_tmp1DC=_tmp1DA->continue_lab;_tmp1DD=_tmp1DA->fallthru_info;_tmp1DE=_tmp1DA->toplevel;_tmp1DF=_tmp1DA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1DB;struct _fat_ptr**c=_tmp1DC;struct Cyc_Toc_FallthruInfo*f=_tmp1DD;int t=_tmp1DE;int*lhs=_tmp1DF;
return({struct Cyc_Toc_Env*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->break_lab=b,_tmp1E0->continue_lab=c,_tmp1E0->fallthru_info=f,_tmp1E0->toplevel=1,_tmp1E0->in_lhs=lhs,_tmp1E0->rgn=r;_tmp1E0;});}}
# 1132
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E1=nv;int*_tmp1E2;_LL1: _tmp1E2=_tmp1E1->in_lhs;_LL2: {int*b=_tmp1E2;
return*b;}}
# 1136
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E3=e;int*_tmp1E4;_LL1: _tmp1E4=_tmp1E3->in_lhs;_LL2: {int*lhs=_tmp1E4;
*lhs=b;}}
# 1141
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E5=e;int*_tmp1EA;int _tmp1E9;struct Cyc_Toc_FallthruInfo*_tmp1E8;struct _fat_ptr**_tmp1E7;struct _fat_ptr**_tmp1E6;_LL1: _tmp1E6=_tmp1E5->break_lab;_tmp1E7=_tmp1E5->continue_lab;_tmp1E8=_tmp1E5->fallthru_info;_tmp1E9=_tmp1E5->toplevel;_tmp1EA=_tmp1E5->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1E6;struct _fat_ptr**c=_tmp1E7;struct Cyc_Toc_FallthruInfo*f=_tmp1E8;int t=_tmp1E9;int*lhs=_tmp1EA;
return({struct Cyc_Toc_Env*_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB->break_lab=b,_tmp1EB->continue_lab=c,_tmp1EB->fallthru_info=f,_tmp1EB->toplevel=t,_tmp1EB->in_lhs=lhs,_tmp1EB->rgn=r;_tmp1EB;});}}
# 1148
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;int*_tmp1F1;int _tmp1F0;struct Cyc_Toc_FallthruInfo*_tmp1EF;struct _fat_ptr**_tmp1EE;struct _fat_ptr**_tmp1ED;_LL1: _tmp1ED=_tmp1EC->break_lab;_tmp1EE=_tmp1EC->continue_lab;_tmp1EF=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1F1=_tmp1EC->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1ED;struct _fat_ptr**c=_tmp1EE;struct Cyc_Toc_FallthruInfo*f=_tmp1EF;int t=_tmp1F0;int*lhs=_tmp1F1;
return({struct Cyc_Toc_Env*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->break_lab=0,_tmp1F2->continue_lab=0,_tmp1F2->fallthru_info=f,_tmp1F2->toplevel=t,_tmp1F2->in_lhs=lhs,_tmp1F2->rgn=r;_tmp1F2;});}}
# 1154
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1158
struct Cyc_Toc_Env*_tmp1F3=e;int*_tmp1F8;int _tmp1F7;struct Cyc_Toc_FallthruInfo*_tmp1F6;struct _fat_ptr**_tmp1F5;struct _fat_ptr**_tmp1F4;_LL1: _tmp1F4=_tmp1F3->break_lab;_tmp1F5=_tmp1F3->continue_lab;_tmp1F6=_tmp1F3->fallthru_info;_tmp1F7=_tmp1F3->toplevel;_tmp1F8=_tmp1F3->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1F4;struct _fat_ptr**c=_tmp1F5;struct Cyc_Toc_FallthruInfo*f=_tmp1F6;int t=_tmp1F7;int*lhs=_tmp1F8;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));
_tmp1FB->label=fallthru_l,_tmp1FB->binders=fallthru_binders;_tmp1FB;});
return({struct Cyc_Toc_Env*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));({struct _fat_ptr**_tmp879=({struct _fat_ptr**_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));*_tmp1F9=break_l;_tmp1F9;});_tmp1FA->break_lab=_tmp879;}),_tmp1FA->continue_lab=c,_tmp1FA->fallthru_info=fi,_tmp1FA->toplevel=t,_tmp1FA->in_lhs=lhs,_tmp1FA->rgn=r;_tmp1FA;});}}
# 1163
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1165
struct Cyc_Toc_Env*_tmp1FC=e;int*_tmp201;int _tmp200;struct Cyc_Toc_FallthruInfo*_tmp1FF;struct _fat_ptr**_tmp1FE;struct _fat_ptr**_tmp1FD;_LL1: _tmp1FD=_tmp1FC->break_lab;_tmp1FE=_tmp1FC->continue_lab;_tmp1FF=_tmp1FC->fallthru_info;_tmp200=_tmp1FC->toplevel;_tmp201=_tmp1FC->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1FD;struct _fat_ptr**c=_tmp1FE;struct Cyc_Toc_FallthruInfo*f=_tmp1FF;int t=_tmp200;int*lhs=_tmp201;
return({struct Cyc_Toc_Env*_tmp203=_region_malloc(r,sizeof(*_tmp203));({struct _fat_ptr**_tmp87A=({struct _fat_ptr**_tmp202=_region_malloc(r,sizeof(*_tmp202));*_tmp202=break_l;_tmp202;});_tmp203->break_lab=_tmp87A;}),_tmp203->continue_lab=c,_tmp203->fallthru_info=0,_tmp203->toplevel=t,_tmp203->in_lhs=lhs,_tmp203->rgn=r;_tmp203;});}}
# 1172
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1174
struct Cyc_Toc_Env*_tmp204=e;int*_tmp209;int _tmp208;struct Cyc_Toc_FallthruInfo*_tmp207;struct _fat_ptr**_tmp206;struct _fat_ptr**_tmp205;_LL1: _tmp205=_tmp204->break_lab;_tmp206=_tmp204->continue_lab;_tmp207=_tmp204->fallthru_info;_tmp208=_tmp204->toplevel;_tmp209=_tmp204->in_lhs;_LL2: {struct _fat_ptr**b=_tmp205;struct _fat_ptr**c=_tmp206;struct Cyc_Toc_FallthruInfo*f=_tmp207;int t=_tmp208;int*lhs=_tmp209;
return({struct Cyc_Toc_Env*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->break_lab=0,_tmp20B->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp87B=({struct Cyc_Toc_FallthruInfo*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->label=next_l,_tmp20A->binders=0;_tmp20A;});_tmp20B->fallthru_info=_tmp87B;}),_tmp20B->toplevel=t,_tmp20B->in_lhs=lhs,_tmp20B->rgn=r;_tmp20B;});}}
# 1187 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1190
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp20C=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp1A=_tmp20C;struct _tuple32*_tmp20D=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp20E;_LL1: _tmp20E=_tmp20D->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp20E;
Cyc_Toc_exptypes_to_c(e);
l=l->tl;}}}
# 1198
static void Cyc_Toc_asm_iolist_toc(struct Cyc_Toc_Env*nv,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp20F=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp1B=_tmp20F;struct _tuple32*_tmp210=_stmttmp1B;struct Cyc_Absyn_Exp*_tmp211;_LL1: _tmp211=_tmp210->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp211;
Cyc_Toc_exp_to_c(nv,e);
l=l->tl;}}}
# 1206
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp212=e->annot;void*_stmttmp1C=_tmp212;void*_tmp213=_stmttmp1C;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp213)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp213)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp213)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp213)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp213)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp213)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp215=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B7;_tmp7B7.tag=0U,({struct _fat_ptr _tmp87C=({const char*_tmp216="Toc: do_null_check";_tag_fat(_tmp216,sizeof(char),19U);});_tmp7B7.f1=_tmp87C;});_tmp7B7;});void*_tmp214[1U];_tmp214[0]=& _tmp215;({unsigned _tmp87D=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp87D,_tag_fat(_tmp214,sizeof(void*),1U));});});}}}}}}_LL0:;}
# 1222
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1224
int ans;
int _tmp217=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp217;
void*_tmp218=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp218;
void*_tmp219=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp219;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp21A=old_typ;void*_tmp21E;void*_tmp21D;struct Cyc_Absyn_Tqual _tmp21C;void*_tmp21B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A)->tag == 3U){_LL1: _tmp21B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A)->f1).elt_type;_tmp21C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A)->f1).elt_tq;_tmp21D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A)->f1).ptr_atts).bounds;_tmp21E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A)->f1).ptr_atts).zero_term;_LL2: {void*ta=_tmp21B;struct Cyc_Absyn_Tqual tq=_tmp21C;void*b=_tmp21D;void*zt=_tmp21E;
# 1235
{void*_tmp21F=annot;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp221;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1237
 if(!((unsigned)({void*_tmp87E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87E,b);}))){
# 1240
void*_tmp222=({void*_tmp87F=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp87F,tq);});void*newt=_tmp222;
({void*_tmp882=({void*_tmp881=newt;Cyc_Toc_cast_it_r(_tmp881,({struct Cyc_Absyn_Exp*_tmp880=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp880,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp882;});
ptr->topt=newt;}
# 1244
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1247
 if(!((unsigned)({void*_tmp883=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp883,b);}))){
# 1249
void*_tmp223=({void*_tmp884=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp884,tq);});void*newt=_tmp223;
({void*_tmp887=({void*_tmp886=newt;Cyc_Toc_cast_it_r(_tmp886,({struct Cyc_Absyn_Exp*_tmp885=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp885,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp887;});
ptr->topt=newt;
# 1256
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp225=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B8;_tmp7B8.tag=0U,({struct _fat_ptr _tmp888=({const char*_tmp226="subscript of ? with no bounds check but need null check";_tag_fat(_tmp226,sizeof(char),56U);});_tmp7B8.f1=_tmp888;});_tmp7B8;});void*_tmp224[1U];_tmp224[0]=& _tmp225;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp224,sizeof(void*),1U));});}
# 1260
({void*_tmp88C=({void*_tmp88B=new_typ;Cyc_Toc_cast_it_r(_tmp88B,({struct Cyc_Absyn_Exp*_tmp227[1U];({struct Cyc_Absyn_Exp*_tmp889=Cyc_Absyn_new_exp(ptr->r,0U);_tmp227[0]=_tmp889;});({struct Cyc_Absyn_Exp*_tmp88A=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp88A,_tag_fat(_tmp227,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp88C;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1266
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp891=({void*_tmp890=newt;Cyc_Toc_cast_it_r(_tmp890,({struct Cyc_Absyn_Exp*_tmp228[3U];({
struct Cyc_Absyn_Exp*_tmp88E=Cyc_Absyn_new_exp(ptr->r,0U);_tmp228[0]=_tmp88E;}),({
struct Cyc_Absyn_Exp*_tmp88D=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp228[1]=_tmp88D;}),_tmp228[2]=ind;({struct Cyc_Absyn_Exp*_tmp88F=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp88F,_tag_fat(_tmp228,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1269
ptr->r=_tmp891;});
# 1273
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp221=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp21F)->f1;_LLF: {struct Cyc_Absyn_Exp*bd=_tmp221;
# 1277
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp220=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp21F)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp220=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp21F)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp220;
# 1280
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1285
struct _tuple13 _tmp229=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _stmttmp1D=_tmp229;struct _tuple13 _tmp22A=_stmttmp1D;int _tmp22C;unsigned _tmp22B;_LL15: _tmp22B=_tmp22A.f1;_tmp22C=_tmp22A.f2;_LL16: {unsigned i=_tmp22B;int valid=_tmp22C;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1288
({void*_tmp896=({void*_tmp895=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp895,({struct Cyc_Absyn_Exp*_tmp22D[4U];({
struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22D[0]=_tmp893;}),_tmp22D[1]=bd,({
struct Cyc_Absyn_Exp*_tmp892=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp22D[2]=_tmp892;}),_tmp22D[3]=ind;({struct Cyc_Absyn_Exp*_tmp894=fn_e;Cyc_Toc_fncall_exp_dl(_tmp894,_tag_fat(_tmp22D,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1288
ptr->r=_tmp896;});
# 1291
ans=1;}else{
# 1294
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1297
({void*_tmp89A=({void*_tmp899=new_typ;Cyc_Toc_cast_it_r(_tmp899,({struct Cyc_Absyn_Exp*_tmp22E[1U];({struct Cyc_Absyn_Exp*_tmp897=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp22E[0]=_tmp897;});({struct Cyc_Absyn_Exp*_tmp898=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp898,_tag_fat(_tmp22E,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1297
ptr->r=_tmp89A;});
# 1299
ans=0;}else{
# 1303
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp89E=({void*_tmp89D=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp89D,({struct Cyc_Absyn_Exp*_tmp22F[3U];({
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22F[0]=_tmp89B;}),_tmp22F[1]=bd,_tmp22F[2]=ind;({struct Cyc_Absyn_Exp*_tmp89C=fn_e;Cyc_Toc_fncall_exp_dl(_tmp89C,_tag_fat(_tmp22F,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1304
ptr->r=_tmp89E;});
# 1306
ans=1;}}
# 1309
goto _LL5;}}}else{_LL12: _LL13:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp231=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B9;_tmp7B9.tag=0U,({struct _fat_ptr _tmp89F=({const char*_tmp232="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp232,sizeof(char),34U);});_tmp7B9.f1=_tmp89F;});_tmp7B9;});void*_tmp230[1U];_tmp230[0]=& _tmp231;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp230,sizeof(void*),1U));});}}}}}}_LL5:;}
# 1312
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp234=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BA;_tmp7BA.tag=0U,({struct _fat_ptr _tmp8A0=({const char*_tmp235="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp235,sizeof(char),31U);});_tmp7BA.f1=_tmp8A0;});_tmp7BA;});void*_tmp233[1U];_tmp233[0]=& _tmp234;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp233,sizeof(void*),1U));});}_LL0:;}}
# 1318
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp237=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BB;_tmp7BB.tag=0U,({struct _fat_ptr _tmp8A1=({const char*_tmp238="Missing type in primop ";_tag_fat(_tmp238,sizeof(char),24U);});_tmp7BB.f1=_tmp8A1;});_tmp7BB;});void*_tmp236[1U];_tmp236[0]=& _tmp237;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp236,sizeof(void*),1U));});
return(void*)_check_null(e->topt);}
# 1322
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple23*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->f1=Cyc_Toc_mt_tq,({void*_tmp8A2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp239->f2=_tmp8A2;});_tmp239;});}
# 1327
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp23A=e->r;void*_stmttmp1E=_tmp23A;void*_tmp23B=_stmttmp1E;int _tmp23D;struct Cyc_Absyn_Exp*_tmp23C;int _tmp23F;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_List_List*_tmp240;switch(*((int*)_tmp23B)){case 26U: _LL1: _tmp240=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23B)->f1;_LL2: {struct Cyc_List_List*dles=_tmp240;
# 1330
{struct Cyc_List_List*_tmp241=dles;struct Cyc_List_List*dles2=_tmp241;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp242="array designators for abstract-field initialization";_tag_fat(_tmp242,sizeof(char),52U);}));}}
_tmp23E=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles),0U);_tmp23F=0;goto _LL4;}case 27U: _LL3: _tmp23E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23B)->f2;_tmp23F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23B)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp23E;int zt=_tmp23F;
_tmp23C=bd;_tmp23D=zt;goto _LL6;}case 28U: _LL5: _tmp23C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23B)->f1;_tmp23D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23B)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp23C;int zt=_tmp23D;
# 1336
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp8A3=bd;Cyc_Absyn_add_exp(_tmp8A3,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1345
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1353
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp243=e->r;void*_stmttmp1F=_tmp243;void*_tmp244=_stmttmp1F;struct Cyc_List_List*_tmp245;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp244)->tag == 29U){_LL1: _tmp245=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp244)->f3;_LL2: {struct Cyc_List_List*dles2=_tmp245;
dles=dles2;goto _LL0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1359
{void*_tmp246=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp20=_tmp246;void*_tmp247=_stmttmp20;struct Cyc_Absyn_Aggrdecl*_tmp248;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp248=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp248;
# 1361
if(ad->impl == 0)
return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1369
 return 0;}_LL5:;}
# 1371
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp249=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp249;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp24A=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp21=_tmp24A;struct _tuple19*_tmp24B=_stmttmp21;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_List_List*_tmp24C;_LLB: _tmp24C=_tmp24B->f1;_tmp24D=_tmp24B->f2;_LLC: {struct Cyc_List_List*ds=_tmp24C;struct Cyc_Absyn_Exp*e2=_tmp24D;
struct _fat_ptr*_tmp24E=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp24E;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp24F=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp24F;
if(nested_ans != 0)
return nested_ans;{
void*_tmp250=Cyc_Tcutil_compress(last_type_field->type);void*_stmttmp22=_tmp250;void*_tmp251=_stmttmp22;struct Cyc_Absyn_Exp*_tmp253;void*_tmp252;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp251)->tag == 4U){_LLE: _tmp252=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp251)->f1).elt_type;_tmp253=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp251)->f1).num_elts;_LLF: {void*elt_type=_tmp252;struct Cyc_Absyn_Exp*type_bd=_tmp253;
# 1386
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1392
return({struct Cyc_Absyn_Exp*_tmp8A6=({struct Cyc_Absyn_Exp*_tmp254[2U];_tmp254[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1394
struct Cyc_Absyn_Exp*_tmp8A4=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp254[1]=_tmp8A4;});({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8A5,_tag_fat(_tmp254,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1392
Cyc_Absyn_add_exp(_tmp8A6,
# 1395
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{_LL10: _LL11:
 return 0;}_LLD:;}}}}
# 1400
({struct Cyc_Warn_String_Warn_Warg_struct _tmp256=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BC;_tmp7BC.tag=0U,({struct _fat_ptr _tmp8A7=({const char*_tmp257="get_varsizeexp: did not find last struct field";_tag_fat(_tmp257,sizeof(char),47U);});_tmp7BC.f1=_tmp8A7;});_tmp7BC;});void*_tmp255[1U];_tmp255[0]=& _tmp256;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp255,sizeof(void*),1U));});}}
# 1403
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp258=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp258;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp259=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp259;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1410
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BF;_tmp7BF.tag=0U,({struct _fat_ptr _tmp8A8=({const char*_tmp25F="get_member_offset ";_tag_fat(_tmp25F,sizeof(char),19U);});_tmp7BF.f1=_tmp8A8;});_tmp7BF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BE;_tmp7BE.tag=0U,_tmp7BE.f1=*f;_tmp7BE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BD;_tmp7BD.tag=0U,({struct _fat_ptr _tmp8A9=({const char*_tmp25E=" failed";_tag_fat(_tmp25E,sizeof(char),8U);});_tmp7BD.f1=_tmp8A9;});_tmp7BD;});void*_tmp25A[3U];_tmp25A[0]=& _tmp25B,_tmp25A[1]=& _tmp25C,_tmp25A[2]=& _tmp25D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp25A,sizeof(void*),3U));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1414
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1417
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1420
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->tag=5U,_tmp260->f1=e1,_tmp260->f2=incr;_tmp260;}),0U);}
# 1424
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1433
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1435
void*_tmp261=e1->r;void*_stmttmp23=_tmp261;void*_tmp262=_stmttmp23;int _tmp266;int _tmp265;struct _fat_ptr*_tmp264;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp268;void*_tmp267;struct Cyc_Absyn_Stmt*_tmp269;switch(*((int*)_tmp262)){case 37U: _LL1: _tmp269=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp262)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp269;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14U: _LL3: _tmp267=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp262)->f1;_tmp268=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp262)->f2;_LL4: {void*t=_tmp267;struct Cyc_Absyn_Exp*e=_tmp268;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21U: _LL5: _tmp263=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp262)->f1;_tmp264=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp262)->f2;_tmp265=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp262)->f3;_tmp266=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp262)->f4;_LL6: {struct Cyc_Absyn_Exp*e=_tmp263;struct _fat_ptr*fld=_tmp264;int is_tagged=_tmp265;int is_read=_tmp266;
# 1440
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp8AC=e1;struct Cyc_List_List*_tmp8AB=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=fld,_tmp26A->tl=fs;_tmp26A;});struct Cyc_Absyn_Exp*(*_tmp8AA)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp8AC,_tmp8AB,_tmp8AA,f_env);});
goto _LL0;}default: _LL7: _LL8: {
# 1448
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1450
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp8AD=(f(e1_copy,f_env))->r;e1->r=_tmp8AD;});
goto _LL0;}}_LL0:;}
# 1456
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1458
void*_tmp26B=s->r;void*_stmttmp24=_tmp26B;void*_tmp26C=_stmttmp24;struct Cyc_Absyn_Stmt*_tmp26D;struct Cyc_Absyn_Stmt*_tmp26F;struct Cyc_Absyn_Decl*_tmp26E;struct Cyc_Absyn_Exp*_tmp270;switch(*((int*)_tmp26C)){case 1U: _LL1: _tmp270=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp26C)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp270;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12U: _LL3: _tmp26E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26C)->f1;_tmp26F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26C)->f2;_LL4: {struct Cyc_Absyn_Decl*d=_tmp26E;struct Cyc_Absyn_Stmt*s2=_tmp26F;
# 1461
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2U: _LL5: _tmp26D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp26C)->f2;_LL6: {struct Cyc_Absyn_Stmt*s2=_tmp26D;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp272=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C1;_tmp7C1.tag=0U,({struct _fat_ptr _tmp8AE=({const char*_tmp274="lvalue_assign_stmt: ";_tag_fat(_tmp274,sizeof(char),21U);});_tmp7C1.f1=_tmp8AE;});_tmp7C1;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp273=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7C0;_tmp7C0.tag=5U,_tmp7C0.f1=s;_tmp7C0;});void*_tmp271[2U];_tmp271[0]=& _tmp272,_tmp271[1]=& _tmp273;Cyc_Toc_toc_impos2(_tag_fat(_tmp271,sizeof(void*),2U));});}_LL0:;}
# 1467
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1471
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp275=e->r;void*_stmttmp25=_tmp275;void*_tmp276=_stmttmp25;struct Cyc_Absyn_Stmt*_tmp277;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp**_tmp27A;void**_tmp279;switch(*((int*)_tmp276)){case 14U: _LL1: _tmp279=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp276)->f1;_tmp27A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp276)->f2;_LL2: {void**t=_tmp279;struct Cyc_Absyn_Exp**e1=_tmp27A;
# 1474
({struct Cyc_Absyn_Exp*_tmp8AF=Cyc_Toc_push_address_exp(*e1);*e1=_tmp8AF;});
({void*_tmp8B0=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp8B0;});
return e;}case 20U: _LL3: _tmp278=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp276)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp278;
# 1478
return e1;}case 37U: _LL5: _tmp277=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp276)->f1;_LL6: {struct Cyc_Absyn_Stmt*s=_tmp277;
# 1482
Cyc_Toc_push_address_stmt(s);
return e;}default: _LL7: _LL8:
# 1485
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C3;_tmp7C3.tag=0U,({struct _fat_ptr _tmp8B1=({const char*_tmp27E="can't take & of exp ";_tag_fat(_tmp27E,sizeof(char),21U);});_tmp7C3.f1=_tmp8B1;});_tmp7C3;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp27D=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7C2;_tmp7C2.tag=4U,_tmp7C2.f1=e;_tmp7C2;});void*_tmp27B[2U];_tmp27B[0]=& _tmp27C,_tmp27B[1]=& _tmp27D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp27B,sizeof(void*),2U));});}_LL0:;}
# 1489
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp27F=s->r;void*_stmttmp26=_tmp27F;void*_tmp280=_stmttmp26;struct Cyc_Absyn_Exp**_tmp281;struct Cyc_Absyn_Stmt*_tmp282;struct Cyc_Absyn_Stmt*_tmp283;switch(*((int*)_tmp280)){case 2U: _LL1: _tmp283=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp280)->f2;_LL2: {struct Cyc_Absyn_Stmt*s2=_tmp283;
_tmp282=s2;goto _LL4;}case 12U: _LL3: _tmp282=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp280)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp282;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1U: _LL5: _tmp281=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp280)->f1;_LL6: {struct Cyc_Absyn_Exp**e=_tmp281;
({struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Toc_push_address_exp(*e);*e=_tmp8B2;});goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp285=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C5;_tmp7C5.tag=0U,({struct _fat_ptr _tmp8B3=({const char*_tmp287="can't take & of stmt ";_tag_fat(_tmp287,sizeof(char),22U);});_tmp7C5.f1=_tmp8B3;});_tmp7C5;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp286=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7C4;_tmp7C4.tag=5U,_tmp7C4.f1=s;_tmp7C4;});void*_tmp284[2U];_tmp284[0]=& _tmp285,_tmp284[1]=& _tmp286;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp284,sizeof(void*),2U));});}_LL0:;}
# 1501
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1513
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp288=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp288;
void*_tmp289=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp289;
void*_tmp28A=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp28A;
struct Cyc_Core_Opt*_tmp28B=({struct Cyc_Core_Opt*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->v=c_ptr_type;_tmp2BE;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp28B;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp28C=e1->r;void*_stmttmp27=_tmp28C;void*_tmp28D=_stmttmp27;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;switch(*((int*)_tmp28D)){case 20U: _LL1: _tmp290=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp28D)->f1;_LL2: {struct Cyc_Absyn_Exp*ea=_tmp290;
# 1521
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1526
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23U: _LL3: _tmp28E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28D)->f1;_tmp28F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28D)->f2;_LL4: {struct Cyc_Absyn_Exp*ea=_tmp28E;struct Cyc_Absyn_Exp*eb=_tmp28F;
# 1528
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1533
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp291[3U];_tmp291[0]=ea,({
struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp291[1]=_tmp8B4;}),_tmp291[2]=eb;({struct Cyc_Absyn_Exp*_tmp8B5=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp8B5,_tag_fat(_tmp291,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp293=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C6;_tmp7C6.tag=0U,({struct _fat_ptr _tmp8B6=({const char*_tmp294="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp294,sizeof(char),53U);});_tmp7C6.f1=_tmp8B6;});_tmp7C6;});void*_tmp292[1U];_tmp292[0]=& _tmp293;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp292,sizeof(void*),1U));});}_LL0:;}{
# 1539
struct _tuple1*_tmp295=Cyc_Toc_temp_var();struct _tuple1*x=_tmp295;
struct _RegionHandle _tmp296=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp296;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp297=({struct Cyc_Absyn_Vardecl*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->sc=Cyc_Absyn_Public,_tmp2BD->name=x,_tmp2BD->varloc=0U,_tmp2BD->tq=Cyc_Toc_mt_tq,_tmp2BD->type=c_fat_ptr_type,_tmp2BD->initializer=xinit,_tmp2BD->rgn=0,_tmp2BD->attributes=0,_tmp2BD->escapes=0,_tmp2BD->is_proto=0;_tmp2BD;});struct Cyc_Absyn_Vardecl*x_vd=_tmp297;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp298=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->tag=4U,_tmp2BC->f1=x_vd;_tmp2BC;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp298;
struct Cyc_Absyn_Exp*_tmp299=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp299;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp29A=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp29A;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp29B=Cyc_Toc_temp_var();struct _tuple1*y=_tmp29B;
struct Cyc_Absyn_Vardecl*_tmp29C=({struct Cyc_Absyn_Vardecl*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->sc=Cyc_Absyn_Public,_tmp2BB->name=y,_tmp2BB->varloc=0U,_tmp2BB->tq=Cyc_Toc_mt_tq,_tmp2BB->type=c_elt_type,_tmp2BB->initializer=deref_x,_tmp2BB->rgn=0,_tmp2BB->attributes=0,_tmp2BB->escapes=0,_tmp2BB->is_proto=0;_tmp2BB;});struct Cyc_Absyn_Vardecl*y_vd=_tmp29C;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29D=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->tag=4U,_tmp2BA->f1=y_vd;_tmp2BA;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp29D;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp29E=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp29E;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp8B8=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp8B7=y_exp;Cyc_Absyn_prim2_exp(_tmp8B8,_tmp8B7,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1560
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp29F=Cyc_Toc_temp_var();struct _tuple1*z=_tmp29F;
struct Cyc_Absyn_Vardecl*_tmp2A0=({struct Cyc_Absyn_Vardecl*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->sc=Cyc_Absyn_Public,_tmp2B9->name=z,_tmp2B9->varloc=0U,_tmp2B9->tq=Cyc_Toc_mt_tq,_tmp2B9->type=c_elt_type,_tmp2B9->initializer=z_init,_tmp2B9->rgn=0,_tmp2B9->attributes=0,_tmp2B9->escapes=0,_tmp2B9->is_proto=0;_tmp2B9;});struct Cyc_Absyn_Vardecl*z_vd=_tmp2A0;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2A1=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->tag=4U,_tmp2B8->f1=z_vd;_tmp2B8;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp2A1;
# 1566
struct Cyc_Absyn_Exp*_tmp2A2=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp2A2;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A3=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp2A3;
struct Cyc_Absyn_Exp*_tmp2A4=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp2A4;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1573
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp2A5;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp2A6;
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp2A7;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1580
struct Cyc_List_List*_tmp2A8=({struct Cyc_Absyn_Exp*_tmp2B7[2U];({struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp2B7[0]=_tmp8BA;}),({
struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B7[1]=_tmp8B9;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2B7,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 1580
struct Cyc_List_List*xsizeargs=_tmp2A8;
# 1582
struct Cyc_Absyn_Exp*_tmp2A9=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp2A9;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp8BB,oneexp,0U);});{
# 1587
struct Cyc_Absyn_Exp*_tmp2AA=({struct Cyc_Absyn_Exp*_tmp8BC=xsize;Cyc_Absyn_and_exp(_tmp8BC,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp2AA;
struct Cyc_Absyn_Stmt*_tmp2AB=Cyc_Absyn_exp_stmt(({void*_tmp2B6=0U;({struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp8BD,_tag_fat(_tmp2B6,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp2AB;
struct Cyc_Absyn_Exp*_tmp2AC=({struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp8BE,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp2AC;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp2AD=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp2AD;
struct Cyc_Absyn_Exp*_tmp2AE=({struct Cyc_Absyn_Exp*_tmp8BF=deref_xcurr;Cyc_Absyn_assign_exp(_tmp8BF,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp2AE;
struct Cyc_Absyn_Stmt*_tmp2AF=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp2AF;
s=({struct Cyc_Absyn_Stmt*_tmp8C2=({struct Cyc_Absyn_Exp*_tmp8C1=comp_exp;struct Cyc_Absyn_Stmt*_tmp8C0=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp8C1,_tmp8C0,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp8C2,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C4=({struct Cyc_Absyn_Decl*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));({void*_tmp8C3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->tag=0U,_tmp2B0->f1=z_vd;_tmp2B0;});_tmp2B1->r=_tmp8C3;}),_tmp2B1->loc=0U;_tmp2B1;});Cyc_Absyn_decl_stmt(_tmp8C4,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C6=({struct Cyc_Absyn_Decl*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));({void*_tmp8C5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->tag=0U,_tmp2B2->f1=y_vd;_tmp2B2;});_tmp2B3->r=_tmp8C5;}),_tmp2B3->loc=0U;_tmp2B3;});Cyc_Absyn_decl_stmt(_tmp8C6,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C8=({struct Cyc_Absyn_Decl*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));({void*_tmp8C7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->tag=0U,_tmp2B4->f1=x_vd;_tmp2B4;});_tmp2B5->r=_tmp8C7;}),_tmp2B5->loc=0U;_tmp2B5;});Cyc_Absyn_decl_stmt(_tmp8C8,s,0U);});
({void*_tmp8C9=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8C9;});}}}}}}}}}}
# 1541
;_pop_region();}}
# 1613 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1617
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2BF=Cyc_Tcutil_compress(aggrtype);void*_stmttmp28=_tmp2BF;void*_tmp2C0=_stmttmp28;union Cyc_Absyn_AggrInfo _tmp2C1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0)->f1)->tag == 20U){_LL1: _tmp2C1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2C1;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C9;_tmp7C9.tag=0U,({struct _fat_ptr _tmp8CA=({const char*_tmp2C7="expecting union but found ";_tag_fat(_tmp2C7,sizeof(char),27U);});_tmp7C9.f1=_tmp8CA;});_tmp7C9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C8;_tmp7C8.tag=2U,_tmp7C8.f1=(void*)aggrtype;_tmp7C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C7;_tmp7C7.tag=0U,({
struct _fat_ptr _tmp8CB=({const char*_tmp2C6=" in check_tagged_union";_tag_fat(_tmp2C6,sizeof(char),23U);});_tmp7C7.f1=_tmp8CB;});_tmp7C7;});void*_tmp2C2[3U];_tmp2C2[0]=& _tmp2C3,_tmp2C2[1]=& _tmp2C4,_tmp2C2[2]=& _tmp2C5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C2,sizeof(void*),3U));});}_LL0:;}{
# 1623
struct _tuple30 _tmp2C8=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp29=_tmp2C8;struct _tuple30 _tmp2C9=_stmttmp29;struct Cyc_Absyn_Exp*_tmp2CB;struct _tuple1*_tmp2CA;_LL6: _tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;_LL7: {struct _tuple1*temp=_tmp2CA;struct Cyc_Absyn_Exp*temp_exp=_tmp2CB;
struct Cyc_Absyn_Exp*_tmp2CC=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2CC;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2CD=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2CD;
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2CE;
struct Cyc_Absyn_Exp*_tmp2CF=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2CF;
struct Cyc_Absyn_Stmt*_tmp2D0=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2D0;
struct Cyc_Absyn_Stmt*_tmp2D1=({struct Cyc_Absyn_Exp*_tmp8CD=test_exp;struct Cyc_Absyn_Stmt*_tmp8CC=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8CD,_tmp8CC,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D1;
void*_tmp2D2=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2D2;
struct Cyc_Absyn_Exp*_tmp2D3=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2D3;
struct Cyc_Absyn_Stmt*_tmp2D4=({struct _tuple1*_tmp8D0=temp;void*_tmp8CF=e1_p_type;struct Cyc_Absyn_Exp*_tmp8CE=e1_f;Cyc_Absyn_declare_stmt(_tmp8D0,_tmp8CF,_tmp8CE,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D4;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1636
struct Cyc_Absyn_Exp*_tmp2D5=({struct Cyc_Absyn_Exp*_tmp8D1=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8D1,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2D5;
struct Cyc_Absyn_Exp*_tmp2D6=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2D6;
struct Cyc_Absyn_Exp*_tmp2D7=({struct Cyc_Absyn_Exp*_tmp8D2=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8D2,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2D7;
struct Cyc_Absyn_Stmt*_tmp2D8=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2D8;
struct Cyc_Absyn_Stmt*_tmp2D9=({struct Cyc_Absyn_Exp*_tmp8D4=test_exp;struct Cyc_Absyn_Stmt*_tmp8D3=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8D4,_tmp8D3,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D9;
struct Cyc_Absyn_Stmt*_tmp2DA=({struct _tuple1*_tmp8D7=temp;void*_tmp8D6=e1_c_type;struct Cyc_Absyn_Exp*_tmp8D5=e1;Cyc_Absyn_declare_stmt(_tmp8D7,_tmp8D6,_tmp8D5,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2DA;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1646
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1649
void*_tmp2DB=Cyc_Tcutil_compress(t);void*_stmttmp2A=_tmp2DB;void*_tmp2DC=_stmttmp2A;union Cyc_Absyn_AggrInfo _tmp2DD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DC)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DC)->f1)->tag == 20U){_LL1: _tmp2DD=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DC)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2DD;
# 1651
struct Cyc_Absyn_Aggrdecl*_tmp2DE=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2DE;
({int _tmp8D8=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp8D8;});{
# 1654
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E2=({struct Cyc_String_pa_PrintArg_struct _tmp7CB;_tmp7CB.tag=0U,_tmp7CB.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7CB;});struct Cyc_String_pa_PrintArg_struct _tmp2E3=({struct Cyc_String_pa_PrintArg_struct _tmp7CA;_tmp7CA.tag=0U,_tmp7CA.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7CA;});void*_tmp2E0[2U];_tmp2E0[0]=& _tmp2E2,_tmp2E0[1]=& _tmp2E3;({struct _fat_ptr _tmp8D9=({const char*_tmp2E1="_union_%s_%s";_tag_fat(_tmp2E1,sizeof(char),13U);});Cyc_aprintf(_tmp8D9,_tag_fat(_tmp2E0,sizeof(void*),2U));});});
({void*_tmp8DA=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));*_tmp2DF=str;_tmp2DF;}));*tagged_member_type=_tmp8DA;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1665
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1668
void*_tmp2E4=e->r;void*_stmttmp2B=_tmp2E4;void*_tmp2E5=_stmttmp2B;int*_tmp2E8;struct _fat_ptr*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;int*_tmp2EB;struct _fat_ptr*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EC;switch(*((int*)_tmp2E5)){case 14U: _LL1: _tmp2EC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E5)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp2EC;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CC;_tmp7CC.tag=0U,({struct _fat_ptr _tmp8DB=({const char*_tmp2EF="cast on lhs!";_tag_fat(_tmp2EF,sizeof(char),13U);});_tmp7CC.f1=_tmp8DB;});_tmp7CC;});void*_tmp2ED[1U];_tmp2ED[0]=& _tmp2EE;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp2ED,sizeof(void*),1U));});}case 21U: _LL3: _tmp2E9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E5)->f1;_tmp2EA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E5)->f2;_tmp2EB=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E5)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2E9;struct _fat_ptr*f=_tmp2EA;int*is_read=_tmp2EB;
# 1671
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22U: _LL5: _tmp2E6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E5)->f1;_tmp2E7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E5)->f2;_tmp2E8=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E5)->f4;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp2E6;struct _fat_ptr*f=_tmp2E7;int*is_read=_tmp2E8;
# 1674
void*_tmp2F0=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp2C=_tmp2F0;void*_tmp2F1=_stmttmp2C;void*_tmp2F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->tag == 3U){_LLA: _tmp2F2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).elt_type;_LLB: {void*et=_tmp2F2;
# 1676
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{_LLC: _LLD:
# 1678
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1680
 return 0;}_LL0:;}
# 1693 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1697
struct _tuple30 _tmp2F3=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2D=_tmp2F3;struct _tuple30 _tmp2F4=_stmttmp2D;struct Cyc_Absyn_Exp*_tmp2F6;struct _tuple1*_tmp2F5;_LL1: _tmp2F5=_tmp2F4.f1;_tmp2F6=_tmp2F4.f2;_LL2: {struct _tuple1*temp=_tmp2F5;struct Cyc_Absyn_Exp*temp_exp=_tmp2F6;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1706
struct Cyc_Absyn_Exp*_tmp2F7=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2F7;
s2=({struct Cyc_Absyn_Exp*_tmp8DD=test_exp;struct Cyc_Absyn_Stmt*_tmp8DC=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8DD,_tmp8DC,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1709
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8E0=temp;void*_tmp8DF=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8DE=
Cyc_Toc_push_address_exp(e1);
# 1709
Cyc_Absyn_declare_stmt(_tmp8E0,_tmp8DF,_tmp8DE,
# 1711
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1716
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CE;_tmp7CE.tag=0U,({struct _fat_ptr _tmp8E1=({const char*_tmp2FB="exp_to_c: no type for ";_tag_fat(_tmp2FB,sizeof(char),23U);});_tmp7CE.f1=_tmp8E1;});_tmp7CE;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2FA=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7CD;_tmp7CD.tag=4U,_tmp7CD.f1=e;_tmp7CD;});void*_tmp2F8[2U];_tmp2F8[0]=& _tmp2F9,_tmp2F8[1]=& _tmp2FA;({unsigned _tmp8E2=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8E2,_tag_fat(_tmp2F8,sizeof(void*),2U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1721
int did_inserted_checks=0;
{void*_tmp2FC=e->annot;void*_stmttmp2E=_tmp2FC;void*_tmp2FD=_stmttmp2E;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2FD)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2FD)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1727
{void*_tmp2FE=e->r;void*_stmttmp2F=_tmp2FE;void*_tmp2FF=_stmttmp2F;struct Cyc_List_List*_tmp301;struct Cyc_List_List*_tmp300;struct Cyc_Absyn_Stmt*_tmp302;struct _fat_ptr*_tmp304;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_Absyn_Exp*_tmp306;struct Cyc_Absyn_Exp*_tmp305;int _tmp30C;int _tmp30B;struct Cyc_Absyn_Exp*_tmp30A;void**_tmp309;struct Cyc_Absyn_Exp*_tmp308;int _tmp307;struct Cyc_Absyn_Datatypefield*_tmp30F;struct Cyc_Absyn_Datatypedecl*_tmp30E;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Aggrdecl*_tmp313;struct Cyc_List_List*_tmp312;struct Cyc_List_List*_tmp311;struct _tuple1**_tmp310;struct Cyc_List_List*_tmp315;void*_tmp314;int _tmp318;void*_tmp317;struct Cyc_Absyn_Exp*_tmp316;int _tmp31C;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp319;struct Cyc_List_List*_tmp31D;struct Cyc_List_List*_tmp31E;struct Cyc_Absyn_Exp*_tmp320;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_Absyn_Exp*_tmp321;int _tmp325;int _tmp324;struct _fat_ptr*_tmp323;struct Cyc_Absyn_Exp*_tmp322;int _tmp329;int _tmp328;struct _fat_ptr*_tmp327;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_List_List*_tmp32B;void*_tmp32A;void*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32F;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp330;enum Cyc_Absyn_Coercion _tmp334;int _tmp333;struct Cyc_Absyn_Exp*_tmp332;void**_tmp331;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Exp*_tmp337;int _tmp339;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_Absyn_VarargInfo*_tmp33E;struct Cyc_List_List*_tmp33D;int _tmp33C;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Absyn_Exp*_tmp348;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Core_Opt*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;enum Cyc_Absyn_Incrementor _tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_List_List*_tmp350;enum Cyc_Absyn_Primop _tmp34F;struct Cyc_Absyn_Exp*_tmp351;switch(*((int*)_tmp2FF)){case 2U: _LL8: _LL9:
# 1729
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1736
struct Cyc_Absyn_Exp*_tmp352=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp352;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8E3=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp8E3;});else{
# 1741
({void*_tmp8E5=({struct Cyc_Absyn_Exp*_tmp353[3U];_tmp353[0]=zero,_tmp353[1]=zero,_tmp353[2]=zero;({struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8E4,_tag_fat(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8E5;});}}else{
# 1743
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1745
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 42U: _LL10: _LL11:
({void*_tmp8E6=(Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U))->r;e->r=_tmp8E6;});goto _LL7;case 41U: _LL12: _tmp351=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp351;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3U: _LL14: _tmp34F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp350=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL15: {enum Cyc_Absyn_Primop p=_tmp34F;struct Cyc_List_List*es=_tmp350;
# 1752
struct Cyc_List_List*_tmp354=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp354;
# 1754
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp355=p;switch(_tmp355){case Cyc_Absyn_Numelts: _LL63: _LL64: {
# 1757
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp356=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_stmttmp30=_tmp356;void*_tmp357=_stmttmp30;void*_tmp35B;void*_tmp35A;void*_tmp359;void*_tmp358;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357)->tag == 3U){_LL78: _tmp358=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357)->f1).elt_type;_tmp359=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357)->f1).ptr_atts).nullable;_tmp35A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357)->f1).ptr_atts).bounds;_tmp35B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357)->f1).ptr_atts).zero_term;_LL79: {void*elt_type=_tmp358;void*nbl=_tmp359;void*bound=_tmp35A;void*zt=_tmp35B;
# 1760
struct Cyc_Absyn_Exp*_tmp35C=({void*_tmp8E7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E7,bound);});struct Cyc_Absyn_Exp*eopt=_tmp35C;
if(eopt == 0)
# 1763
({void*_tmp8EA=({struct Cyc_Absyn_Exp*_tmp35D[2U];_tmp35D[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp35D[1]=_tmp8E8;});({struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp8E9,_tag_fat(_tmp35D,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1763
e->r=_tmp8EA;});else{
# 1766
struct Cyc_Absyn_Exp*_tmp35E=eopt;struct Cyc_Absyn_Exp*e2=_tmp35E;
# 1768
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1772
({void*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp35F[2U];_tmp35F[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp35F[1]=e2;({struct Cyc_Absyn_Exp*_tmp8EB=function_e;Cyc_Toc_fncall_exp_r(_tmp8EB,_tag_fat(_tmp35F,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8EC;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(e2))
({void*_tmp360=0U;({unsigned _tmp8EE=e->loc;struct _fat_ptr _tmp8ED=({const char*_tmp361="can't calculate numelts";_tag_fat(_tmp361,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8EE,_tmp8ED,_tag_fat(_tmp360,sizeof(void*),0U));});});
# 1777
({void*_tmp8F1=({struct Cyc_Absyn_Exp*_tmp8F0=arg;struct Cyc_Absyn_Exp*_tmp8EF=e2;Cyc_Toc_conditional_exp_r(_tmp8F0,_tmp8EF,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8F1;});}else{
# 1779
e->r=e2->r;goto _LL77;}}}
# 1782
goto _LL77;}}else{_LL7A: _LL7B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp363=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D0;_tmp7D0.tag=0U,({struct _fat_ptr _tmp8F2=({const char*_tmp365="numelts primop applied to non-pointer ";_tag_fat(_tmp365,sizeof(char),39U);});_tmp7D0.f1=_tmp8F2;});_tmp7D0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp364=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7CF;_tmp7CF.tag=2U,_tmp7CF.f1=(void*)_check_null(arg->topt);_tmp7CF;});void*_tmp362[2U];_tmp362[0]=& _tmp363,_tmp362[1]=& _tmp364;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp362,sizeof(void*),2U));});}_LL77:;}
# 1786
goto _LL62;}case Cyc_Absyn_Plus: _LL65: _LL66:
# 1791
 if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp366="can't do pointer arithmetic at top-level";_tag_fat(_tmp366,sizeof(char),41U);}));
{void*_tmp367=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp31=_tmp367;void*_tmp368=_stmttmp31;void*_tmp36B;void*_tmp36A;void*_tmp369;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368)->tag == 3U){_LL7D: _tmp369=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368)->f1).elt_type;_tmp36A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368)->f1).ptr_atts).bounds;_tmp36B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368)->f1).ptr_atts).zero_term;_LL7E: {void*elt_type=_tmp369;void*b=_tmp36A;void*zt=_tmp36B;
# 1795
struct Cyc_Absyn_Exp*_tmp36C=({void*_tmp8F3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8F3,b);});struct Cyc_Absyn_Exp*eopt=_tmp36C;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp8F6=({struct Cyc_Absyn_Exp*_tmp36D[3U];_tmp36D[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36D[1]=_tmp8F4;}),_tmp36D[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8F5,_tag_fat(_tmp36D,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1799
e->r=_tmp8F6;});else{
# 1801
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp8F8=({struct Cyc_Absyn_Exp*_tmp36E[3U];_tmp36E[0]=e1,_tmp36E[1]=eopt,_tmp36E[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8F7,_tag_fat(_tmp36E,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8F8;});}
goto _LL7C;}}else{_LL7F: _LL80:
 goto _LL7C;}_LL7C:;}
# 1806
goto _LL62;case Cyc_Absyn_Minus: _LL67: _LL68: {
# 1811
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd)){
({void*_tmp8FA=({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8F9,Cyc_Toc_curr_sp);});e1->r=_tmp8FA;});
({void*_tmp8FC=({struct Cyc_Absyn_Exp*_tmp8FB=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8FB,Cyc_Toc_curr_sp);});e2->r=_tmp8FC;});
({void*_tmp8FE=({void*_tmp8FD=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8FD;});e1->topt=_tmp8FE;});
({void*_tmp900=({struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8FF,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1819
e->r=_tmp900;});}else{
# 1822
({void*_tmp904=({struct Cyc_Absyn_Exp*_tmp36F[3U];_tmp36F[0]=e1,({
struct Cyc_Absyn_Exp*_tmp902=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36F[1]=_tmp902;}),({
struct Cyc_Absyn_Exp*_tmp901=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp36F[2]=_tmp901;});({struct Cyc_Absyn_Exp*_tmp903=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp903,_tag_fat(_tmp36F,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1822
e->r=_tmp904;});}}
# 1826
goto _LL62;}case Cyc_Absyn_Eq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Neq: _LL6B: _LL6C: goto _LL6E;case Cyc_Absyn_Gt: _LL6D: _LL6E: goto _LL70;case Cyc_Absyn_Gte: _LL6F: _LL70: goto _LL72;case Cyc_Absyn_Lt: _LL71: _LL72: goto _LL74;case Cyc_Absyn_Lte: _LL73: _LL74: {
# 1829
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(old_types))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp370=({void*_tmp905=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp905,Cyc_Toc_mt_tq);});void*t=_tmp370;
({void*_tmp908=({void*_tmp907=t;Cyc_Toc_cast_it_r(_tmp907,({struct Cyc_Absyn_Exp*_tmp906=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp906,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp908;});
e1->topt=t;}
# 1839
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp371=({void*_tmp909=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp909,Cyc_Toc_mt_tq);});void*t=_tmp371;
({void*_tmp90C=({void*_tmp90B=t;Cyc_Toc_cast_it_r(_tmp90B,({struct Cyc_Absyn_Exp*_tmp90A=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp90A,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp90C;});
e2->topt=t;}
# 1844
goto _LL62;}default: _LL75: _LL76:
 goto _LL62;}_LL62:;}
# 1847
goto _LL7;}case 5U: _LL16: _tmp34D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp34E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL17: {struct Cyc_Absyn_Exp*e2=_tmp34D;enum Cyc_Absyn_Incrementor incr=_tmp34E;
# 1849
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1858 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp38D="increment";_tag_fat(_tmp38D,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp372="decrement";_tag_fat(_tmp372,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp374=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D4;_tmp7D4.tag=0U,({struct _fat_ptr _tmp90D=({const char*_tmp37A="in-place ";_tag_fat(_tmp37A,sizeof(char),10U);});_tmp7D4.f1=_tmp90D;});_tmp7D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D3;_tmp7D3.tag=0U,_tmp7D3.f1=incr_str;_tmp7D3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp376=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D2;_tmp7D2.tag=0U,({struct _fat_ptr _tmp90E=({const char*_tmp379=" is not supported when ";_tag_fat(_tmp379,sizeof(char),24U);});_tmp7D2.f1=_tmp90E;});_tmp7D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp377=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D1;_tmp7D1.tag=0U,({
struct _fat_ptr _tmp90F=({const char*_tmp378="dereferencing a zero-terminated pointer";_tag_fat(_tmp378,sizeof(char),40U);});_tmp7D1.f1=_tmp90F;});_tmp7D1;});void*_tmp373[4U];_tmp373[0]=& _tmp374,_tmp373[1]=& _tmp375,_tmp373[2]=& _tmp376,_tmp373[3]=& _tmp377;({unsigned _tmp910=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp910,_tag_fat(_tmp373,sizeof(void*),4U));});});
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp37B=incr;switch(_tmp37B){case Cyc_Absyn_PreInc: _LL82: _LL83:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec: _LL84: _LL85:
 op=2U;goto _LL81;default: _LL86: _LL87:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D7;_tmp7D7.tag=0U,({struct _fat_ptr _tmp911=({const char*_tmp381="in-place post";_tag_fat(_tmp381,sizeof(char),14U);});_tmp7D7.f1=_tmp911;});_tmp7D7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D6;_tmp7D6.tag=0U,_tmp7D6.f1=incr_str;_tmp7D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D5;_tmp7D5.tag=0U,({
struct _fat_ptr _tmp912=({const char*_tmp380=" is not supported on @tagged union members";_tag_fat(_tmp380,sizeof(char),43U);});_tmp7D5.f1=_tmp912;});_tmp7D5;});void*_tmp37C[3U];_tmp37C[0]=& _tmp37D,_tmp37C[1]=& _tmp37E,_tmp37C[2]=& _tmp37F;({unsigned _tmp913=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp913,_tag_fat(_tmp37C,sizeof(void*),3U));});});}_LL81:;}
# 1876
({void*_tmp915=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->tag=4U,_tmp383->f1=e2,({struct Cyc_Core_Opt*_tmp914=({struct Cyc_Core_Opt*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->v=(void*)op;_tmp382;});_tmp383->f2=_tmp914;}),_tmp383->f3=one;_tmp383;});e->r=_tmp915;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1880
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1885
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)incr == (int)1U ||(int)incr == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1891
if((int)incr == (int)2U ||(int)incr == (int)3U)
change=-1;
({void*_tmp91A=({struct Cyc_Absyn_Exp*_tmp384[3U];({struct Cyc_Absyn_Exp*_tmp918=Cyc_Toc_push_address_exp(e2);_tmp384[0]=_tmp918;}),({
struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp384[1]=_tmp917;}),({
struct Cyc_Absyn_Exp*_tmp916=Cyc_Absyn_signed_int_exp(change,0U);_tmp384[2]=_tmp916;});({struct Cyc_Absyn_Exp*_tmp919=fn_e;Cyc_Toc_fncall_exp_r(_tmp919,_tag_fat(_tmp384,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1893
e->r=_tmp91A;});}else{
# 1896
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1902
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp385=Cyc_Toc_temp_var();struct _tuple1*x=_tmp385;
void*_tmp386=({void*_tmp91B=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp91B,Cyc_Toc_mt_tq);});void*t=_tmp386;
struct Cyc_Absyn_Exp*_tmp387=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp387;
struct Cyc_Absyn_Exp*_tmp388=({struct Cyc_Absyn_Exp*_tmp91C=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp91C,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1907
struct Cyc_Absyn_Exp*testexp=_tmp388;
# 1909
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp91E=({struct Cyc_Absyn_Exp*_tmp91D=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp91D,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1910
Cyc_Absyn_and_exp(_tmp91E,testexp,0U);});{
# 1912
struct Cyc_Absyn_Stmt*_tmp389=({struct Cyc_Absyn_Exp*_tmp921=testexp;struct Cyc_Absyn_Stmt*_tmp920=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1912
Cyc_Absyn_ifthenelse_stmt(_tmp921,_tmp920,
# 1914
Cyc_Absyn_exp_stmt(({void*_tmp38A=0U;({struct Cyc_Absyn_Exp*_tmp91F=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp91F,_tag_fat(_tmp38A,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 1912
struct Cyc_Absyn_Stmt*s=_tmp389;
# 1915
s=({struct Cyc_Absyn_Stmt*_tmp922=s;Cyc_Absyn_seq_stmt(_tmp922,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp923=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp923;});}}else{
# 1919
struct Cyc_Toc_functionSet*_tmp38B=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp38B;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp927=({struct Cyc_Absyn_Exp*_tmp38C[2U];({struct Cyc_Absyn_Exp*_tmp925=Cyc_Toc_push_address_exp(e2);_tmp38C[0]=_tmp925;}),({struct Cyc_Absyn_Exp*_tmp924=Cyc_Absyn_signed_int_exp(1,0U);_tmp38C[1]=_tmp924;});({struct Cyc_Absyn_Exp*_tmp926=fn_e;Cyc_Toc_fncall_exp_r(_tmp926,_tag_fat(_tmp38C,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp927;});}}else{
# 1923
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1927
goto _LL7;}}case 4U: _LL18: _tmp34A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp34B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp34C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp34A;struct Cyc_Core_Opt*popt=_tmp34B;struct Cyc_Absyn_Exp*e2=_tmp34C;
# 1946 "toc.cyc"
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp928=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp928;});
# 1957
goto _LL7;}{
# 1959
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
# 1965
return;}{
# 1969
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1975
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp38E=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp32=_tmp38E;enum Cyc_Absyn_Primop _tmp38F=_stmttmp32;switch(_tmp38F){case Cyc_Absyn_Plus: _LL89: _LL8A:
 change=e2;goto _LL88;case Cyc_Absyn_Minus: _LL8B: _LL8C:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL88;default: _LL8D: _LL8E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp391=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D8;_tmp7D8.tag=0U,({struct _fat_ptr _tmp929=({const char*_tmp392="bad t ? pointer arithmetic";_tag_fat(_tmp392,sizeof(char),27U);});_tmp7D8.f1=_tmp929;});_tmp7D8;});void*_tmp390[1U];_tmp390[0]=& _tmp391;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp390,sizeof(void*),1U));});}_LL88:;}
# 1985
done=1;{
# 1987
struct Cyc_Absyn_Exp*_tmp393=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp393;
({void*_tmp92D=({struct Cyc_Absyn_Exp*_tmp394[3U];({struct Cyc_Absyn_Exp*_tmp92B=Cyc_Toc_push_address_exp(e1);_tmp394[0]=_tmp92B;}),({
struct Cyc_Absyn_Exp*_tmp92A=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp394[1]=_tmp92A;}),_tmp394[2]=change;({struct Cyc_Absyn_Exp*_tmp92C=fn_e;Cyc_Toc_fncall_exp_r(_tmp92C,_tag_fat(_tmp394,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1988
e->r=_tmp92D;});}}else{
# 1991
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1994
enum Cyc_Absyn_Primop _tmp395=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp33=_tmp395;enum Cyc_Absyn_Primop _tmp396=_stmttmp33;if(_tmp396 == Cyc_Absyn_Plus){_LL90: _LL91:
# 1996
 done=1;
({void*_tmp92F=({struct Cyc_Absyn_Exp*_tmp397[2U];_tmp397[0]=e1,_tmp397[1]=e2;({struct Cyc_Absyn_Exp*_tmp92E=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp92E,_tag_fat(_tmp397,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp92F;});
goto _LL8F;}else{_LL92: _LL93:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp399=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D9;_tmp7D9.tag=0U,({struct _fat_ptr _tmp930=({const char*_tmp39A="bad zero-terminated pointer arithmetic";_tag_fat(_tmp39A,sizeof(char),39U);});_tmp7D9.f1=_tmp930;});_tmp7D9;});void*_tmp398[1U];_tmp398[0]=& _tmp399;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp398,sizeof(void*),1U));});}_LL8F:;}}}
# 2003
if(!done){
# 2005
if(e1_poly)
({void*_tmp932=({void*_tmp931=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp931,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp932;});
# 2011
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp933=e1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*f_env))Cyc_Toc_lvalue_assign)(_tmp933,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->f1=popt,_tmp39B->f2=e2;_tmp39B;}));});
e->r=e1->r;}}
# 2016
goto _LL7;}}}}case 6U: _LL1A: _tmp347=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp348=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp349=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp347;struct Cyc_Absyn_Exp*e2=_tmp348;struct Cyc_Absyn_Exp*e3=_tmp349;
# 2018
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7U: _LL1C: _tmp345=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp346=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL1D: {struct Cyc_Absyn_Exp*e1=_tmp345;struct Cyc_Absyn_Exp*e2=_tmp346;
_tmp343=e1;_tmp344=e2;goto _LL1F;}case 8U: _LL1E: _tmp343=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp344=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp343;struct Cyc_Absyn_Exp*e2=_tmp344;
_tmp341=e1;_tmp342=e2;goto _LL21;}case 9U: _LL20: _tmp341=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp342=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp341;struct Cyc_Absyn_Exp*e2=_tmp342;
# 2025
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3 == 0){_LL22: _tmp33F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp340=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL23: {struct Cyc_Absyn_Exp*e1=_tmp33F;struct Cyc_List_List*es=_tmp340;
# 2029
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2034
({void*_tmp937=({void*_tmp936=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp936,({struct Cyc_Absyn_Exp*_tmp39C[1U];({struct Cyc_Absyn_Exp*_tmp934=
Cyc_Absyn_copy_exp(e1);_tmp39C[0]=_tmp934;});({struct Cyc_Absyn_Exp*_tmp935=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp935,_tag_fat(_tmp39C,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2034
e1->r=_tmp937;});
# 2036
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_LL24: _tmp33A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp33B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp33C=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3)->num_varargs;_tmp33D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3)->injectors;_tmp33E=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3)->vai;_LL25: {struct Cyc_Absyn_Exp*e1=_tmp33A;struct Cyc_List_List*es=_tmp33B;int num_varargs=_tmp33C;struct Cyc_List_List*injectors=_tmp33D;struct Cyc_Absyn_VarargInfo*vai=_tmp33E;
# 2047 "toc.cyc"
struct _RegionHandle _tmp39D=_new_region("r");struct _RegionHandle*r=& _tmp39D;_push_region(r);{
struct _tuple30 _tmp39E=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp34=_tmp39E;struct _tuple30 _tmp39F=_stmttmp34;struct Cyc_Absyn_Exp*_tmp3A1;struct _tuple1*_tmp3A0;_LL95: _tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;_LL96: {struct _tuple1*argv=_tmp3A0;struct Cyc_Absyn_Exp*argvexp=_tmp3A1;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2054
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
int num_normargs=num_args - num_varargs;
# 2058
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3A2->tl=new_args;_tmp3A2;});}}
# 2063
new_args=({struct Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct Cyc_Absyn_Exp*_tmp93A=({struct Cyc_Absyn_Exp*_tmp3A3[3U];_tmp3A3[0]=argvexp,({
# 2065
struct Cyc_Absyn_Exp*_tmp938=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp3A3[1]=_tmp938;}),_tmp3A3[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp939=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp939,_tag_fat(_tmp3A3,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2063
_tmp3A4->hd=_tmp93A;}),_tmp3A4->tl=new_args;_tmp3A4;});
# 2068
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2070
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2075
({void*_tmp93E=({void*_tmp93D=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp93D,({struct Cyc_Absyn_Exp*_tmp3A5[1U];({struct Cyc_Absyn_Exp*_tmp93B=
Cyc_Absyn_copy_exp(e1);_tmp3A5[0]=_tmp93B;});({struct Cyc_Absyn_Exp*_tmp93C=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp93C,_tag_fat(_tmp3A5,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2075
e1->r=_tmp93E;});{
# 2077
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2080
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp3A6=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp35=_tmp3A6;void*_tmp3A7=_stmttmp35;struct Cyc_Absyn_Datatypedecl*_tmp3A8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->f1)->f1).KnownDatatype).tag == 2){_LL98: _tmp3A8=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7)->f1)->f1).KnownDatatype).val;_LL99: {struct Cyc_Absyn_Datatypedecl*x=_tmp3A8;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A: _LL9B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DA;_tmp7DA.tag=0U,({struct _fat_ptr _tmp93F=({const char*_tmp3AB="unknown datatype in vararg with inject";_tag_fat(_tmp3AB,sizeof(char),39U);});_tmp7DA.f1=_tmp93F;});_tmp7DA;});void*_tmp3A9[1U];_tmp3A9[0]=& _tmp3AA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3A9,sizeof(void*),1U));});}_LL97:;}{
# 2086
struct _fat_ptr vs=({unsigned _tmp3B7=(unsigned)num_varargs;struct _tuple1**_tmp3B6=({struct _RegionHandle*_tmp940=r;_region_malloc(_tmp940,_check_times(_tmp3B7,sizeof(struct _tuple1*)));});({{unsigned _tmp7DB=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp7DB;++ i){({struct _tuple1*_tmp941=Cyc_Toc_temp_var();_tmp3B6[i]=_tmp941;});}}0;});_tag_fat(_tmp3B6,sizeof(struct _tuple1*),_tmp3B7);});
# 2088
if(num_varargs != 0){
# 2092
struct Cyc_List_List*_tmp3AC=0;struct Cyc_List_List*array_args=_tmp3AC;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_Exp*_tmp942=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp3AD->hd=_tmp942;}),_tmp3AD->tl=array_args;_tmp3AD;});}}
s=({struct _tuple1*_tmp945=argv;void*_tmp944=arr_type;struct Cyc_Absyn_Exp*_tmp943=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp945,_tmp944,_tmp943,s,0U);});
# 2097
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)- 1;
for(0;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp3AE=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp36=_tmp3AE;struct Cyc_Absyn_Datatypefield*_tmp3AF=_stmttmp36;struct Cyc_List_List*_tmp3B1;struct _tuple1*_tmp3B0;_LL9D: _tmp3B0=_tmp3AF->name;_tmp3B1=_tmp3AF->typs;_LL9E: {struct _tuple1*qv=_tmp3B0;struct Cyc_List_List*tqts=_tmp3B1;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2111
struct _tuple1*_tmp3B2=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp3B2;
struct Cyc_List_List*_tmp3B3=({struct _tuple19*_tmp3B5[2U];({struct _tuple19*_tmp949=({struct _fat_ptr*_tmp948=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp948,Cyc_Toc_datatype_tag(tud,qv));});_tmp3B5[0]=_tmp949;}),({
struct _tuple19*_tmp947=({struct _fat_ptr*_tmp946=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp946,arg);});_tmp3B5[1]=_tmp947;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3B5,sizeof(struct _tuple19*),2U));});
# 2112
struct Cyc_List_List*dles=_tmp3B3;
# 2114
s=({struct _tuple1*_tmp94C=var;void*_tmp94B=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp94A=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->tag=29U,_tmp3B4->f1=tdn,_tmp3B4->f2=0,_tmp3B4->f3=dles,_tmp3B4->f4=0;_tmp3B4;}),0U);
# 2114
Cyc_Absyn_declare_stmt(_tmp94C,_tmp94B,_tmp94A,s,0U);});}}}}}else{
# 2122
s=({struct _tuple1*_tmp94F=argv;void*_tmp94E=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp94D=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp94F,_tmp94E,_tmp94D,s,0U);});}}}else{
# 2127
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3B8->tl=array_args;_tmp3B8;});}{
# 2133
struct Cyc_Absyn_Exp*_tmp3B9=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp3B9;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2136
s=({struct _tuple1*_tmp952=argv;void*_tmp951=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp950=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp952,_tmp951,_tmp950,s,0U);});}}
# 2139
({void*_tmp953=Cyc_Toc_stmt_exp_r(s);e->r=_tmp953;});}}}}
# 2141
_npop_handler(0U);goto _LL7;
# 2047
;_pop_region();}}case 11U: _LL26: _tmp338=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp339=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp338;int b=_tmp339;
# 2144
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp956=({void*_tmp955=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp955,({struct Cyc_Absyn_Exp*_tmp3BA[1U];_tmp3BA[0]=e1;({struct Cyc_Absyn_Exp*_tmp954=fn_e;Cyc_Toc_fncall_exp_dl(_tmp954,_tag_fat(_tmp3BA,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp956;});
goto _LL7;}}case 12U: _LL28: _tmp337=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL29: {struct Cyc_Absyn_Exp*e1=_tmp337;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13U: _LL2A: _tmp335=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp336=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL2B: {struct Cyc_Absyn_Exp*e1=_tmp335;struct Cyc_List_List*ts=_tmp336;
# 2150
Cyc_Toc_exp_to_c(nv,e1);
# 2159 "toc.cyc"
for(0;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp3BB=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp3BB;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp3BC=Cyc_Tcutil_compress((void*)ts->hd);void*_stmttmp37=_tmp3BC;void*_tmp3BD=_stmttmp37;switch(*((int*)_tmp3BD)){case 2U: _LLA0: _LLA1:
 goto _LLA3;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BD)->f1)->tag == 18U){_LLA2: _LLA3:
 continue;}else{goto _LLA4;}default: _LLA4: _LLA5:
# 2166
({void*_tmp958=({void*_tmp957=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp957,e1,0U);})->r;e->r=_tmp958;});
goto _LL9F;}_LL9F:;}
# 2169
break;}}
# 2172
goto _LL7;}case 14U: _LL2C: _tmp331=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp332=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp333=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_tmp334=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FF)->f4;_LL2D: {void**t=_tmp331;struct Cyc_Absyn_Exp*e1=_tmp332;int user_inserted=_tmp333;enum Cyc_Absyn_Coercion coercion=_tmp334;
# 2174
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2181
{struct _tuple34 _tmp3BE=({struct _tuple34 _tmp7DD;({void*_tmp95A=Cyc_Tcutil_compress(old_t2);_tmp7DD.f1=_tmp95A;}),({void*_tmp959=Cyc_Tcutil_compress(new_typ);_tmp7DD.f2=_tmp959;});_tmp7DD;});struct _tuple34 _stmttmp38=_tmp3BE;struct _tuple34 _tmp3BF=_stmttmp38;struct Cyc_Absyn_PtrInfo _tmp3C0;struct Cyc_Absyn_PtrInfo _tmp3C2;struct Cyc_Absyn_PtrInfo _tmp3C1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BF.f1)->tag == 3U)switch(*((int*)_tmp3BF.f2)){case 3U: _LLA7: _tmp3C1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BF.f1)->f1;_tmp3C2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BF.f2)->f1;_LLA8: {struct Cyc_Absyn_PtrInfo p1=_tmp3C1;struct Cyc_Absyn_PtrInfo p2=_tmp3C2;
# 2183
struct Cyc_Absyn_Exp*_tmp3C3=({void*_tmp95B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95B,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp3C3;
struct Cyc_Absyn_Exp*_tmp3C4=({void*_tmp95C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95C,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp3C4;
int _tmp3C5=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp3C5;
int _tmp3C6=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp3C6;
{struct _tuple35 _tmp3C7=({struct _tuple35 _tmp7DC;_tmp7DC.f1=b1,_tmp7DC.f2=b2;_tmp7DC;});struct _tuple35 _stmttmp39=_tmp3C7;struct _tuple35 _tmp3C8=_stmttmp39;if(_tmp3C8.f1 != 0){if(_tmp3C8.f2 != 0){_LLAE: _LLAF:
# 2190
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp95F=({void*_tmp95E=*t;Cyc_Toc_cast_it_r(_tmp95E,({struct Cyc_Absyn_Exp*_tmp3C9[1U];_tmp3C9[0]=e1;({struct Cyc_Absyn_Exp*_tmp95D=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp95D,_tag_fat(_tmp3C9,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp95F;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2199
goto _LLAD;}else{_LLB0: _LLB1: {
# 2201
struct Cyc_Absyn_Exp*_tmp3CA=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp3CA;
struct _tuple13 _tmp3CB=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp3A=_tmp3CB;struct _tuple13 _tmp3CC=_stmttmp3A;int _tmp3CE;unsigned _tmp3CD;_LLB7: _tmp3CD=_tmp3CC.f1;_tmp3CE=_tmp3CC.f2;_LLB8: {unsigned i=_tmp3CD;int valid=_tmp3CE;
if(Cyc_Toc_is_toplevel(nv)){
# 2207
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp960=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp960,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp961=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp961;});}else{
# 2212
if(zt1){
# 2217
struct _tuple30 _tmp3CF=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3B=_tmp3CF;struct _tuple30 _tmp3D0=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3D2;struct _tuple1*_tmp3D1;_LLBA: _tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;_LLBB: {struct _tuple1*x=_tmp3D1;struct Cyc_Absyn_Exp*x_exp=_tmp3D2;
struct Cyc_Absyn_Exp*arg3;
# 2221
{void*_tmp3D3=e1->r;void*_stmttmp3C=_tmp3D3;void*_tmp3D4=_stmttmp3C;struct Cyc_Absyn_Vardecl*_tmp3D5;struct Cyc_Absyn_Vardecl*_tmp3D6;switch(*((int*)_tmp3D4)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D4)->f1).Wstring_c).tag){case 8U: _LLBD: _LLBE:
 arg3=e2;goto _LLBC;case 9U: _LLBF: _LLC0:
 arg3=e2;goto _LLBC;default: goto _LLC5;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D4)->f1)){case 1U: _LLC1: _tmp3D6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D4)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_tmp3D6;
_tmp3D5=vd;goto _LLC4;}case 4U: _LLC3: _tmp3D5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D4)->f1)->f1;_LLC4: {struct Cyc_Absyn_Vardecl*vd=_tmp3D5;
# 2226
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC6;
arg3=e2;
goto _LLBC;}default: goto _LLC5;}default: _LLC5: _LLC6:
# 2233
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2236
arg3=({struct Cyc_Absyn_Exp*_tmp3D7[2U];({
struct Cyc_Absyn_Exp*_tmp963=({void*_tmp962=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp962,x_exp);});_tmp3D7[0]=_tmp963;}),_tmp3D7[1]=e2;({struct Cyc_Absyn_Exp*_tmp964=
# 2236
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp964,_tag_fat(_tmp3D7,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2238
goto _LLBC;}_LLBC:;}
# 2240
if(!zt2 && !(p2.elt_tq).real_const)
# 2243
arg3=({struct Cyc_Absyn_Exp*_tmp965=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp965,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2245
struct Cyc_Absyn_Exp*_tmp3D8=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3D8;
struct Cyc_Absyn_Exp*_tmp3D9=({struct Cyc_Absyn_Exp*_tmp3DB[3U];_tmp3DB[0]=x_exp,_tmp3DB[1]=arg2,_tmp3DB[2]=arg3;({struct Cyc_Absyn_Exp*_tmp966=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp966,_tag_fat(_tmp3DB,sizeof(struct Cyc_Absyn_Exp*),3U));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3D9;
struct Cyc_Absyn_Stmt*_tmp3DA=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3DA;
s=({struct _tuple1*_tmp969=x;void*_tmp968=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp967=e1;Cyc_Absyn_declare_stmt(_tmp969,_tmp968,_tmp967,s,0U);});
({void*_tmp96A=Cyc_Toc_stmt_exp_r(s);e->r=_tmp96A;});}}}else{
# 2252
({void*_tmp96D=({struct Cyc_Absyn_Exp*_tmp3DC[3U];_tmp3DC[0]=e1,({
# 2254
struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3DC[1]=_tmp96B;}),_tmp3DC[2]=e2;({struct Cyc_Absyn_Exp*_tmp96C=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp96C,_tag_fat(_tmp3DC,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2252
e->r=_tmp96D;});}}
# 2257
goto _LLAD;}}}}else{if(_tmp3C8.f2 != 0){_LLB2: _LLB3: {
# 2267 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3DD=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3DD;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp96E=b2;Cyc_Absyn_add_exp(_tmp96E,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2274
struct Cyc_Absyn_Exp*_tmp3DE=({struct Cyc_Absyn_Exp*_tmp3E0[3U];_tmp3E0[0]=e1,({
# 2276
struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3E0[1]=_tmp96F;}),_tmp3E0[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp970=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp970,_tag_fat(_tmp3E0,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2274
struct Cyc_Absyn_Exp*ptr_exp=_tmp3DE;
# 2278
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp973=({struct Cyc_Absyn_Exp*_tmp3DF[1U];({struct Cyc_Absyn_Exp*_tmp971=Cyc_Absyn_copy_exp(ptr_exp);_tmp3DF[0]=_tmp971;});({struct Cyc_Absyn_Exp*_tmp972=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp972,_tag_fat(_tmp3DF,sizeof(struct Cyc_Absyn_Exp*),1U));});});ptr_exp->r=_tmp973;});
({void*_tmp974=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp974;});
goto _LLAD;}}}else{_LLB4: _LLB5:
# 2286
 if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp3E1="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3E1,sizeof(char),70U);}));
({void*_tmp978=({struct Cyc_Absyn_Exp*_tmp3E2[3U];_tmp3E2[0]=e1,({
struct Cyc_Absyn_Exp*_tmp976=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3E2[1]=_tmp976;}),({
struct Cyc_Absyn_Exp*_tmp975=Cyc_Absyn_uint_exp(1U,0U);_tmp3E2[2]=_tmp975;});({struct Cyc_Absyn_Exp*_tmp977=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp977,_tag_fat(_tmp3E2,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2289
e->r=_tmp978;});}
# 2293
goto _LLAD;}}_LLAD:;}
# 2295
goto _LLA6;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BF.f2)->f1)->tag == 1U){_LLA9: _tmp3C0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BF.f1)->f1;_LLAA: {struct Cyc_Absyn_PtrInfo p1=_tmp3C0;
# 2297
{struct Cyc_Absyn_Exp*_tmp3E3=({void*_tmp979=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp979,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3D=_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4=_stmttmp3D;if(_tmp3E4 == 0){_LLC8: _LLC9:
# 2299
({void*_tmp97B=({struct Cyc_Absyn_Exp*_tmp97A=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp97A,Cyc_Toc_curr_sp);});e1->r=_tmp97B;});
e1->topt=new_typ_c;
goto _LLC7;}else{_LLCA: _LLCB:
 goto _LLC7;}_LLC7:;}
# 2304
goto _LLA6;}}else{goto _LLAB;}default: goto _LLAB;}else{_LLAB: _LLAC:
# 2306
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLA6;}_LLA6:;}
# 2310
goto _LL7;}case 15U: _LL2E: _tmp330=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL2F: {struct Cyc_Absyn_Exp*e1=_tmp330;
# 2313
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2319
({void*_tmp97D=({void*_tmp97C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp97C,e1);});e->r=_tmp97D;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2323
({void*_tmp97F=({void*_tmp97E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp97E,e1);});e->r=_tmp97F;});}
# 2325
goto _LL7;}case 16U: _LL30: _tmp32E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp32F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL31: {struct Cyc_Absyn_Exp*rgnopt=_tmp32E;struct Cyc_Absyn_Exp*e1=_tmp32F;
# 2332
Cyc_Toc_exp_to_c(nv,e1);{
# 2334
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple30 _tmp3E5=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3E=_tmp3E5;struct _tuple30 _tmp3E6=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp3E8;struct _tuple1*_tmp3E7;_LLCD: _tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;_LLCE: {struct _tuple1*x=_tmp3E7;struct Cyc_Absyn_Exp*xexp=_tmp3E8;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3E9=Cyc_Tcutil_compress(elt_typ);void*_stmttmp3F=_tmp3E9;void*_tmp3EA=_stmttmp3F;void*_tmp3EB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA)->tag == 4U){_LLD0: _tmp3EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EA)->f1).elt_type;_LLD1: {void*t2=_tmp3EB;
# 2339
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCF;}}else{_LLD2: _LLD3:
# 2343
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCF;}_LLCF:;}{
# 2346
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3EC=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3EC;
struct Cyc_Absyn_Exp*_tmp3ED=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3ED;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3EE[2U];_tmp3EE[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3EE[1]=_tmp980;});({struct Cyc_Absyn_Exp*_tmp981=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp981,_tag_fat(_tmp3EE,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2353
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2356
struct Cyc_Absyn_Exp*_tmp3EF=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3EF;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2360
if(rgnopt == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2363
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2366
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3F0[3U];({
struct Cyc_Absyn_Exp*_tmp984=Cyc_Absyn_copy_exp(xexp);_tmp3F0[0]=_tmp984;}),({
struct Cyc_Absyn_Exp*_tmp983=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3F0[1]=_tmp983;}),
(unsigned)lenexp?_tmp3F0[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp982=Cyc_Absyn_uint_exp(1U,0U);_tmp3F0[2]=_tmp982;});({struct Cyc_Absyn_Exp*_tmp985=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp985,_tag_fat(_tmp3F0,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2373
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp986=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->tag=Cyc_Toc_Dest,_tmp3F1->f1=xexp;_tmp3F1;});e->annot=_tmp986;});{
struct Cyc_Absyn_Stmt*_tmp3F2=({struct _tuple1*_tmp98A=x;void*_tmp989=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp988=mexp;Cyc_Absyn_declare_stmt(_tmp98A,_tmp989,_tmp988,({
struct Cyc_Absyn_Stmt*_tmp987=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp987,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2375
struct Cyc_Absyn_Stmt*s=_tmp3F2;
# 2379
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp98B=Cyc_Toc_stmt_exp_r(s);e->r=_tmp98B;});
if(vse != 0)
({void*_tmp98D=({void*_tmp98C=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp98C,Cyc_Absyn_copy_exp(e));});e->r=_tmp98D;});
goto _LL7;}}}}}}}case 18U: _LL32: _tmp32D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL33: {struct Cyc_Absyn_Exp*e1=_tmp32D;
# 2387
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17U: _LL34: _tmp32C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL35: {void*t=_tmp32C;
({void*_tmp98F=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->tag=17U,({void*_tmp98E=Cyc_Toc_typ_to_c(t);_tmp3F3->f1=_tmp98E;});_tmp3F3;});e->r=_tmp98F;});goto _LL7;}case 19U: _LL36: _tmp32A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp32B=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL37: {void*t=_tmp32A;struct Cyc_List_List*fs=_tmp32B;
# 2390
void*_tmp3F4=t;void*t2=_tmp3F4;
struct Cyc_List_List*_tmp3F5=fs;struct Cyc_List_List*l=_tmp3F5;
for(0;l != 0;l=l->tl){
void*_tmp3F6=(void*)l->hd;void*_stmttmp40=_tmp3F6;void*_tmp3F7=_stmttmp40;unsigned _tmp3F8;struct _fat_ptr*_tmp3F9;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F7)->tag == 0U){_LLD5: _tmp3F9=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F7)->f1;_LLD6: {struct _fat_ptr*n=_tmp3F9;
goto _LLD4;}}else{_LLD7: _tmp3F8=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3F7)->f1;_LLD8: {unsigned n=_tmp3F8;
# 2396
{void*_tmp3FA=Cyc_Tcutil_compress(t2);void*_stmttmp41=_tmp3FA;void*_tmp3FB=_stmttmp41;struct Cyc_List_List*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Datatypefield*_tmp3FE;union Cyc_Absyn_AggrInfo _tmp3FF;switch(*((int*)_tmp3FB)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FB)->f1)){case 20U: _LLDA: _tmp3FF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FB)->f1)->f1;_LLDB: {union Cyc_Absyn_AggrInfo info=_tmp3FF;
# 2398
struct Cyc_Absyn_Aggrdecl*_tmp400=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp400;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp402=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DE;_tmp7DE.tag=0U,({struct _fat_ptr _tmp990=({const char*_tmp403="struct fields must be known";_tag_fat(_tmp403,sizeof(char),28U);});_tmp7DE.f1=_tmp990;});_tmp7DE;});void*_tmp401[1U];_tmp401[0]=& _tmp402;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp401,sizeof(void*),1U));});
_tmp3FD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FB)->f1)->f1).KnownDatatypefield).tag == 2){_LLE0: _tmp3FE=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FB)->f1)->f1).KnownDatatypefield).val).f2;_LLE1: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3FE;
# 2412
if(n == (unsigned)0)
({void*_tmp991=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->tag=0U,_tmp407->f1=Cyc_Toc_tag_sp;_tmp407;}));l->hd=_tmp991;});else{
# 2415
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp993=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->tag=0U,({struct _fat_ptr*_tmp992=Cyc_Absyn_fieldname((int)n);_tmp408->f1=_tmp992;});_tmp408;}));l->hd=_tmp993;});}
# 2418
goto _LLD9;}}else{goto _LLE2;}default: goto _LLE2;}case 7U: _LLDC: _tmp3FD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3FB)->f2;_LLDD: {struct Cyc_List_List*fields=_tmp3FD;
# 2403
struct Cyc_Absyn_Aggrfield*_tmp404=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp404;
({void*_tmp994=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->tag=0U,_tmp405->f1=nth_field->name;_tmp405;}));l->hd=_tmp994;});
t2=nth_field->type;
goto _LLD9;}case 6U: _LLDE: _tmp3FC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3FB)->f1;_LLDF: {struct Cyc_List_List*ts=_tmp3FC;
# 2408
({void*_tmp996=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=0U,({struct _fat_ptr*_tmp995=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp406->f1=_tmp995;});_tmp406;}));l->hd=_tmp996;});
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD9;}default: _LLE2: _LLE3:
# 2419
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DF;_tmp7DF.tag=0U,({struct _fat_ptr _tmp997=({const char*_tmp40B="impossible type for offsetof tuple index";_tag_fat(_tmp40B,sizeof(char),41U);});_tmp7DF.f1=_tmp997;});_tmp7DF;});void*_tmp409[1U];_tmp409[0]=& _tmp40A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp409,sizeof(void*),1U));});}_LLD9:;}
# 2421
goto _LLD4;}}_LLD4:;}
# 2424
({void*_tmp999=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->tag=19U,({void*_tmp998=Cyc_Toc_typ_to_c(t);_tmp40C->f1=_tmp998;}),_tmp40C->f2=fs;_tmp40C;});e->r=_tmp999;});
goto _LL7;}case 21U: _LL38: _tmp326=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp327=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp328=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_tmp329=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FF)->f4;_LL39: {struct Cyc_Absyn_Exp*e1=_tmp326;struct _fat_ptr*f=_tmp327;int is_tagged=_tmp328;int is_read=_tmp329;
# 2427
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp99E=({struct Cyc_Absyn_Exp*_tmp99D=e1;void*_tmp99C=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp99B=e1_cyc_type;struct _fat_ptr*_tmp99A=f;Cyc_Toc_check_tagged_union(_tmp99D,_tmp99C,_tmp99B,_tmp99A,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2431
e->r=_tmp99E;});
# 2433
if(is_poly)
({void*_tmp9A0=({void*_tmp99F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99F,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9A0;});
goto _LL7;}case 22U: _LL3A: _tmp322=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp323=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp324=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_tmp325=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FF)->f4;_LL3B: {struct Cyc_Absyn_Exp*e1=_tmp322;struct _fat_ptr*f=_tmp323;int is_tagged=_tmp324;int is_read=_tmp325;
# 2437
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp40D=Cyc_Tcutil_compress(e1typ);void*_stmttmp42=_tmp40D;void*_tmp40E=_stmttmp42;struct Cyc_Absyn_PtrInfo _tmp40F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->tag == 3U){_LLE5: _tmp40F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1;_LLE6: {struct Cyc_Absyn_PtrInfo p=_tmp40F;
ta=p.elt_type;goto _LLE4;}}else{_LLE7: _LLE8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp411=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E0;_tmp7E0.tag=0U,({struct _fat_ptr _tmp9A1=({const char*_tmp412="get_ptr_typ: not a pointer!";_tag_fat(_tmp412,sizeof(char),28U);});_tmp7E0.f1=_tmp9A1;});_tmp7E0;});void*_tmp410[1U];_tmp410[0]=& _tmp411;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp410,sizeof(void*),1U));});}_LLE4:;}
# 2444
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp9A6=({struct Cyc_Absyn_Exp*_tmp9A5=e1;void*_tmp9A4=Cyc_Toc_typ_to_c(e1typ);void*_tmp9A3=ta;struct _fat_ptr*_tmp9A2=f;Cyc_Toc_check_tagged_union(_tmp9A5,_tmp9A4,_tmp9A3,_tmp9A2,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9A6;});
if(is_poly && is_read)
({void*_tmp9A8=({void*_tmp9A7=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9A7,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9A8;});
goto _LL7;}case 20U: _LL3C: _tmp321=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL3D: {struct Cyc_Absyn_Exp*e1=_tmp321;
# 2452
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23U: _LL3E: _tmp31F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp320=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL3F: {struct Cyc_Absyn_Exp*e1=_tmp31F;struct Cyc_Absyn_Exp*e2=_tmp320;
# 2456
{void*_tmp413=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp43=_tmp413;void*_tmp414=_stmttmp43;struct Cyc_List_List*_tmp415;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp414)->tag == 6U){_LLEA: _tmp415=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp414)->f1;_LLEB: {struct Cyc_List_List*ts=_tmp415;
# 2458
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp416=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp416;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _tmp417=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp44=_tmp417;struct _tuple13 _tmp418=_stmttmp44;int _tmp41A;unsigned _tmp419;_LLEF: _tmp419=_tmp418.f1;_tmp41A=_tmp418.f2;_LLF0: {unsigned i=_tmp419;int known=_tmp41A;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E1;_tmp7E1.tag=0U,({struct _fat_ptr _tmp9A9=({const char*_tmp41D="unknown tuple subscript in translation to C";_tag_fat(_tmp41D,sizeof(char),44U);});_tmp7E1.f1=_tmp9A9;});_tmp7E1;});void*_tmp41B[1U];_tmp41B[0]=& _tmp41C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp41B,sizeof(void*),1U));});
({void*_tmp9AB=({struct Cyc_Absyn_Exp*_tmp9AA=e1;Cyc_Toc_aggrmember_exp_r(_tmp9AA,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp9AB;});
goto _LLE9;}}}}}else{_LLEC: _LLED:
# 2468
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp9AC=Cyc_Toc_deref_exp_r(e1);e->r=_tmp9AC;});
goto _LLE9;}}_LLE9:;}
# 2474
goto _LL7;}case 24U: _LL40: _tmp31E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL41: {struct Cyc_List_List*es=_tmp31E;
# 2476
if(!Cyc_Toc_is_toplevel(nv)){
# 2478
void*_tmp41E=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple23*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp41E;
{void*_tmp41F=strct_typ;union Cyc_Absyn_AggrInfo _tmp420;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f2 == 0){_LLF2: _tmp420=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41F)->f1)->f1;_LLF3: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp420;
# 2481
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));({struct _tuple19*_tmp9AE=({struct _fat_ptr*_tmp9AD=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9AD,(struct Cyc_Absyn_Exp*)es->hd);});_tmp421->hd=_tmp9AE;}),_tmp421->tl=dles;_tmp421;});}}
# 2487
({void*_tmp9B0=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=29U,_tmp422->f1=sd->name,_tmp422->f2=0,({struct Cyc_List_List*_tmp9AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp422->f3=_tmp9AF;}),_tmp422->f4=sd;_tmp422;});e->r=_tmp9B0;});
e->topt=strct_typ;
goto _LLF1;}}else{goto _LLF4;}}else{goto _LLF4;}}else{_LLF4: _LLF5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp424=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E2;_tmp7E2.tag=0U,({struct _fat_ptr _tmp9B1=({const char*_tmp425="tuple type not an aggregate";_tag_fat(_tmp425,sizeof(char),28U);});_tmp7E2.f1=_tmp9B1;});_tmp7E2;});void*_tmp423[1U];_tmp423[0]=& _tmp424;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp423,sizeof(void*),1U));});}_LLF1:;}
# 2492
goto _LL7;}else{
# 2496
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp427=_cycalloc(sizeof(*_tmp427));({struct _tuple19*_tmp9B2=({struct _tuple19*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->f1=0,_tmp426->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp426;});_tmp427->hd=_tmp9B2;}),_tmp427->tl=dles;_tmp427;});}
# 2501
({void*_tmp9B3=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9B3;});}
# 2503
goto _LL7;}case 26U: _LL42: _tmp31D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL43: {struct Cyc_List_List*dles0=_tmp31D;
# 2509
{struct Cyc_List_List*_tmp428=dles0;struct Cyc_List_List*dles=_tmp428;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9B4=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp9B4;});
goto _LL7;}case 27U: _LL44: _tmp319=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp31A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FF)->f4;_LL45: {struct Cyc_Absyn_Vardecl*vd=_tmp319;struct Cyc_Absyn_Exp*e1=_tmp31A;struct Cyc_Absyn_Exp*e2=_tmp31B;int iszeroterm=_tmp31C;
# 2517
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp429=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _stmttmp45=_tmp429;struct _tuple13 _tmp42A=_stmttmp45;int _tmp42C;unsigned _tmp42B;_LLF7: _tmp42B=_tmp42A.f1;_tmp42C=_tmp42A.f2;_LLF8: {unsigned sz=_tmp42B;int known=_tmp42C;
void*_tmp42D=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp42D;
struct Cyc_List_List*es=0;
# 2524
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp42E=0U;({unsigned _tmp9B6=e1->loc;struct _fat_ptr _tmp9B5=({const char*_tmp42F="cannot determine value of constant";_tag_fat(_tmp42F,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9B6,_tmp9B5,_tag_fat(_tmp42E,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp431=_cycalloc(sizeof(*_tmp431));({struct _tuple19*_tmp9B7=({struct _tuple19*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430->f1=0,_tmp430->f2=e2;_tmp430;});_tmp431->hd=_tmp9B7;}),_tmp431->tl=es;_tmp431;});}}
# 2530
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp432=({void*_tmp9B8=elt_typ;Cyc_Toc_cast_it(_tmp9B8,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp432;
es=({struct Cyc_List_List*_tmp9BA=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9BA,({struct Cyc_List_List*_tmp434=_cycalloc(sizeof(*_tmp434));({struct _tuple19*_tmp9B9=({struct _tuple19*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->f1=0,_tmp433->f2=ezero;_tmp433;});_tmp434->hd=_tmp9B9;}),_tmp434->tl=0;_tmp434;}));});}}
# 2535
({void*_tmp9BB=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9BB;});}}
# 2537
goto _LL7;}case 28U: _LL46: _tmp316=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp317=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp318=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_LL47: {struct Cyc_Absyn_Exp*e1=_tmp316;void*t1=_tmp317;int iszeroterm=_tmp318;
# 2544
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9BC=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9BC;});else{
# 2547
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30U: _LL48: _tmp314=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp315=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL49: {void*st=_tmp314;struct Cyc_List_List*dles=_tmp315;
# 2551
{struct Cyc_List_List*_tmp435=dles;struct Cyc_List_List*dles2=_tmp435;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*_tmp436=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp436;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp437=Cyc_Tcutil_compress(strct_typ);void*_stmttmp46=_tmp437;void*_tmp438=_stmttmp46;union Cyc_Absyn_AggrInfo _tmp439;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp438)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp438)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp438)->f2 == 0){_LLFA: _tmp439=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp438)->f1)->f1;_LLFB: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp439;
# 2558
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp9BD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=29U,_tmp43A->f1=sd->name,_tmp43A->f2=0,_tmp43A->f3=dles,_tmp43A->f4=sd;_tmp43A;});e->r=_tmp9BD;});
e->topt=strct_typ;
goto _LLF9;}}else{goto _LLFC;}}else{goto _LLFC;}}else{_LLFC: _LLFD:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E3;_tmp7E3.tag=0U,({struct _fat_ptr _tmp9BE=({const char*_tmp43D="anonStruct type not an aggregate";_tag_fat(_tmp43D,sizeof(char),33U);});_tmp7E3.f1=_tmp9BE;});_tmp7E3;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43B,sizeof(void*),1U));});}_LLF9:;}else{
# 2565
({void*_tmp9BF=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp9BF;});}
goto _LL7;}}case 29U: _LL4A: _tmp310=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp311=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp312=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_tmp313=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FF)->f4;_LL4B: {struct _tuple1**tdn=_tmp310;struct Cyc_List_List*exist_ts=_tmp311;struct Cyc_List_List*dles=_tmp312;struct Cyc_Absyn_Aggrdecl*sd=_tmp313;
# 2571
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E4;_tmp7E4.tag=0U,({struct _fat_ptr _tmp9C0=({const char*_tmp440="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp440,sizeof(char),58U);});_tmp7E4.f1=_tmp9C0;});_tmp7E4;});void*_tmp43E[1U];_tmp43E[0]=& _tmp43F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43E,sizeof(void*),1U));});{
void*_tmp441=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp441;
{void*_tmp442=Cyc_Tcutil_compress(new_typ);void*_stmttmp47=_tmp442;void*_tmp443=_stmttmp47;union Cyc_Absyn_AggrInfo _tmp444;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->f1)->tag == 20U){_LLFF: _tmp444=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->f1)->f1;_LL100: {union Cyc_Absyn_AggrInfo info=_tmp444;
({struct _tuple1*_tmp9C1=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp9C1;});goto _LLFE;}}else{goto _LL101;}}else{_LL101: _LL102:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp446=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E5;_tmp7E5.tag=0U,({struct _fat_ptr _tmp9C2=({const char*_tmp447="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp447,sizeof(char),44U);});_tmp7E5.f1=_tmp9C2;});_tmp7E5;});void*_tmp445[1U];_tmp445[0]=& _tmp446;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp445,sizeof(void*),1U));});}_LLFE:;}{
# 2582
struct Cyc_List_List*_tmp448=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp448;
if(typ_fields == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp449=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp449;
struct Cyc_List_List*_tmp44A=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp44A;
# 2589
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp44B=Cyc_Tcutil_compress(old_typ);void*_stmttmp48=_tmp44B;void*_tmp44C=_stmttmp48;struct Cyc_List_List*_tmp44D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44C)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44C)->f1)->tag == 20U){_LL104: _tmp44D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44C)->f2;_LL105: {struct Cyc_List_List*param_ts=_tmp44D;
# 2593
{struct Cyc_List_List*_tmp44E=fields;struct Cyc_List_List*fields2=_tmp44E;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_tmp44F=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp49=_tmp44F;struct _tuple36*_tmp450=_stmttmp49;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Aggrfield*_tmp451;_LL109: _tmp451=_tmp450->f1;_tmp452=_tmp450->f2;_LL10A: {struct Cyc_Absyn_Aggrfield*aggrfield=_tmp451;struct Cyc_Absyn_Exp*fieldexp=_tmp452;
void*_tmp453=fieldexp->topt;void*old_field_typ=_tmp453;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2598
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp9C4=({void*_tmp9C3=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp9C3,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2600
fieldexp->r=_tmp9C4;});
# 2603
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp454=aggrfield->name;struct _fat_ptr*n=_tmp454;
struct Cyc_Absyn_Exp*_tmp455=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp455;
struct _tuple19*_tmp456=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp456;
struct _tuple19*_tmp457=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple19*val_dle=_tmp457;
struct _tuple1*s=({struct _tuple1*_tmp462=_cycalloc(sizeof(*_tmp462));({union Cyc_Absyn_Nmspace _tmp9C8=Cyc_Absyn_Abs_n(0,1);_tmp462->f1=_tmp9C8;}),({
struct _fat_ptr*_tmp9C7=({struct _fat_ptr*_tmp461=_cycalloc(sizeof(*_tmp461));({struct _fat_ptr _tmp9C6=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45F=({struct Cyc_String_pa_PrintArg_struct _tmp7E7;_tmp7E7.tag=0U,_tmp7E7.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp7E7;});struct Cyc_String_pa_PrintArg_struct _tmp460=({struct Cyc_String_pa_PrintArg_struct _tmp7E6;_tmp7E6.tag=0U,_tmp7E6.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp7E6;});void*_tmp45D[2U];_tmp45D[0]=& _tmp45F,_tmp45D[1]=& _tmp460;({struct _fat_ptr _tmp9C5=({const char*_tmp45E="_union_%s_%s";_tag_fat(_tmp45E,sizeof(char),13U);});Cyc_aprintf(_tmp9C5,_tag_fat(_tmp45D,sizeof(void*),2U));});});*_tmp461=_tmp9C6;});_tmp461;});_tmp462->f2=_tmp9C7;});_tmp462;});
# 2611
struct _tuple19*_tmp458=({
struct _fat_ptr*_tmp9CA=n;Cyc_Toc_make_field(_tmp9CA,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=29U,_tmp45C->f1=s,_tmp45C->f2=0,({struct Cyc_List_List*_tmp9C9=({struct _tuple19*_tmp45B[2U];_tmp45B[0]=tag_dle,_tmp45B[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp45B,sizeof(struct _tuple19*),2U));});_tmp45C->f3=_tmp9C9;}),_tmp45C->f4=0;_tmp45C;}),0U));});
# 2611
struct _tuple19*u_dle=_tmp458;
# 2614
({void*_tmp9CC=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=29U,_tmp45A->f1=*tdn,_tmp45A->f2=0,({struct Cyc_List_List*_tmp9CB=({struct _tuple19*_tmp459[1U];_tmp459[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp459,sizeof(struct _tuple19*),1U));});_tmp45A->f3=_tmp9CB;}),_tmp45A->f4=sd;_tmp45A;});e->r=_tmp9CC;});}
# 2620
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp463=({struct Cyc_List_List*_tmp9CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9CD,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(sd->tvs,param_ts));});
# 2621
struct Cyc_List_List*inst=_tmp463;
# 2623
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp464=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp464;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp465=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp465;
void*_tmp466=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp466;
struct Cyc_Absyn_Aggrfield*_tmp467=({struct Cyc_Absyn_Aggrfield*_tmp9CE=old_f;Cyc_Toc_aggrfield_to_c(_tmp9CE,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2627
struct Cyc_Absyn_Aggrfield*new_f=_tmp467;
# 2629
new_fields=({struct Cyc_List_List*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->hd=new_f,_tmp468->tl=new_fields;_tmp468;});
# 2635
if(fs->tl == 0){
{void*_tmp469=Cyc_Tcutil_compress(new_f->type);void*_stmttmp4A=_tmp469;void*_tmp46A=_stmttmp4A;struct Cyc_Absyn_ArrayInfo _tmp46B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46A)->tag == 4U){_LL10C: _tmp46B=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46A)->f1;_LL10D: {struct Cyc_Absyn_ArrayInfo ai=_tmp46B;
# 2638
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp46C=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp46C;
({struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp9CF;});
({void*_tmp9D0=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=4U,_tmp46D->f1=ai2;_tmp46D;});new_f->type=_tmp9D0;});}
# 2643
goto _LL10B;}}else{_LL10E: _LL10F:
# 2647
 if(fieldexp->topt == 0)
goto _LL10B;
{void*_tmp46E=Cyc_Tcutil_compress((void*)_check_null(fieldexp->topt));void*_stmttmp4B=_tmp46E;void*_tmp46F=_stmttmp4B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46F)->f1)->tag == 20U){_LL111: _LL112:
# 2651
 new_f->type=(void*)_check_null(fieldexp->topt);goto _LL110;}else{goto _LL113;}}else{_LL113: _LL114:
 goto _LL110;}_LL110:;}
# 2654
goto _LL10B;}_LL10B:;}
# 2658
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp9D2=({struct Cyc_List_List*_tmp471=_cycalloc(sizeof(*_tmp471));({void*_tmp9D1=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=6U,_tmp470->f1=0;_tmp470;});_tmp471->hd=_tmp9D1;}),_tmp471->tl=new_f->attributes;_tmp471;});new_f->attributes=_tmp9D2;});}}}
# 2664
sd=({struct _fat_ptr*_tmp9D5=({struct _fat_ptr*_tmp475=_cycalloc(sizeof(*_tmp475));({struct _fat_ptr _tmp9D4=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp474=({struct Cyc_Int_pa_PrintArg_struct _tmp7E8;_tmp7E8.tag=1U,_tmp7E8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7E8;});void*_tmp472[1U];_tmp472[0]=& _tmp474;({struct _fat_ptr _tmp9D3=({const char*_tmp473="_genStruct%d";_tag_fat(_tmp473,sizeof(char),13U);});Cyc_aprintf(_tmp9D3,_tag_fat(_tmp472,sizeof(void*),1U));});});*_tmp475=_tmp9D4;});_tmp475;});Cyc_Toc_make_c_struct_defn(_tmp9D5,
# 2666
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2670
({void*_tmp9D6=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp476=_cycalloc(sizeof(*_tmp476));*_tmp476=sd;_tmp476;})),0);e->topt=_tmp9D6;});}}}}
# 2673
goto _LL103;}}else{goto _LL106;}}else{_LL106: _LL107:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp478=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E9;_tmp7E9.tag=0U,({struct _fat_ptr _tmp9D7=({const char*_tmp479="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp479,sizeof(char),39U);});_tmp7E9.f1=_tmp9D7;});_tmp7E9;});void*_tmp477[1U];_tmp477[0]=& _tmp478;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp477,sizeof(void*),1U));});}_LL103:;}else{
# 2682
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2684
struct _tuple36*_tmp47A=(struct _tuple36*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple36*_stmttmp4C=_tmp47A;struct _tuple36*_tmp47B=_stmttmp4C;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Aggrfield*_tmp47C;_LL116: _tmp47C=_tmp47B->f1;_tmp47D=_tmp47B->f2;_LL117: {struct Cyc_Absyn_Aggrfield*field=_tmp47C;struct Cyc_Absyn_Exp*fieldexp=_tmp47D;
void*_tmp47E=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp47E;
void*_tmp47F=field->type;void*fieldtyp=_tmp47F;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp9D9=({
void*_tmp9D8=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp9D8,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2690
fieldexp->r=_tmp9D9;});{
# 2693
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp480=({struct _tuple19*_tmp482[2U];({struct _tuple19*_tmp9DB=({struct _tuple19*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->f1=0,_tmp483->f2=field_tag_exp;_tmp483;});_tmp482[0]=_tmp9DB;}),({struct _tuple19*_tmp9DA=({struct _tuple19*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->f1=0,_tmp484->f2=fieldexp;_tmp484;});_tmp482[1]=_tmp9DA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp482,sizeof(struct _tuple19*),2U));});struct Cyc_List_List*newdles=_tmp480;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp9DD=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp481[1U];({struct _tuple19*_tmp9DC=Cyc_Toc_make_field(field->name,umem);_tmp481[0]=_tmp9DC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp481,sizeof(struct _tuple19*),1U));}));e->r=_tmp9DD;});}}}else{
# 2700
struct Cyc_List_List*_tmp485=0;struct Cyc_List_List*newdles=_tmp485;
struct Cyc_List_List*_tmp486=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp486;
for(0;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp487=fields;struct Cyc_List_List*fields2=_tmp487;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_tmp488=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp4D=_tmp488;struct _tuple36*_tmp489=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Aggrfield*_tmp48A;_LL119: _tmp48A=_tmp489->f1;_tmp48B=_tmp489->f2;_LL11A: {struct Cyc_Absyn_Aggrfield*field=_tmp48A;struct Cyc_Absyn_Exp*fieldexp=_tmp48B;
void*_tmp48C=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp48C;
void*_tmp48D=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp48D;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2710
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2712
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp9DE=fieldtyp;Cyc_Toc_cast_it(_tmp9DE,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*_tmp48F));({struct _tuple19*_tmp9DF=({struct _tuple19*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->f1=0,_tmp48E->f2=fieldexp;_tmp48E;});_tmp48F->hd=_tmp9DF;}),_tmp48F->tl=newdles;_tmp48F;});
break;}}}}
# 2719
({void*_tmp9E0=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(newdles));e->r=_tmp9E0;});}}
# 2722
goto _LL7;}}}}case 31U: _LL4C: _tmp30D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp30E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_tmp30F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_LL4D: {struct Cyc_List_List*es=_tmp30D;struct Cyc_Absyn_Datatypedecl*tud=_tmp30E;struct Cyc_Absyn_Datatypefield*tuf=_tmp30F;
# 2726
struct Cyc_List_List*_tmp490=tuf->typs;struct Cyc_List_List*tqts=_tmp490;
{struct Cyc_List_List*_tmp491=es;struct Cyc_List_List*es2=_tmp491;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp492=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp492;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp9E1=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp9E1;});}}{
# 2737
struct Cyc_Absyn_Exp*_tmp493=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2737
struct Cyc_Absyn_Exp*tag_exp=_tmp493;
# 2740
if(!Cyc_Toc_is_toplevel(nv)){
# 2742
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));({struct _tuple19*_tmp9E3=({struct _fat_ptr*_tmp9E2=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9E2,(struct Cyc_Absyn_Exp*)es->hd);});_tmp494->hd=_tmp9E3;}),_tmp494->tl=dles;_tmp494;});}}{
# 2748
struct _tuple19*_tmp495=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp495;
({void*_tmp9E7=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->tag=29U,({struct _tuple1*_tmp9E6=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp497->f1=_tmp9E6;}),_tmp497->f2=0,({
struct Cyc_List_List*_tmp9E5=({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->hd=tag_dle,({struct Cyc_List_List*_tmp9E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp496->tl=_tmp9E4;});_tmp496;});_tmp497->f3=_tmp9E5;}),_tmp497->f4=0;_tmp497;});
# 2749
e->r=_tmp9E7;});}}else{
# 2754
struct Cyc_List_List*_tmp498=0;struct Cyc_List_List*dles=_tmp498;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp49A=_cycalloc(sizeof(*_tmp49A));({struct _tuple19*_tmp9E8=({struct _tuple19*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->f1=0,_tmp499->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp499;});_tmp49A->hd=_tmp9E8;}),_tmp49A->tl=dles;_tmp49A;});}
({void*_tmp9EB=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp49C=_cycalloc(sizeof(*_tmp49C));({struct _tuple19*_tmp9EA=({struct _tuple19*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->f1=0,_tmp49B->f2=tag_exp;_tmp49B;});_tmp49C->hd=_tmp9EA;}),({
struct Cyc_List_List*_tmp9E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp49C->tl=_tmp9E9;});_tmp49C;}));
# 2757
e->r=_tmp9EB;});}
# 2760
goto _LL7;}}case 32U: _LL4E: _LL4F:
# 2762
 goto _LL51;case 33U: _LL50: _LL51:
 goto _LL7;case 34U: _LL52: _tmp307=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).is_calloc;_tmp308=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).rgn;_tmp309=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).elt_type;_tmp30A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).num_elts;_tmp30B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).fat_result;_tmp30C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).inline_call;_LL53: {int is_calloc=_tmp307;struct Cyc_Absyn_Exp*rgnopt=_tmp308;void**topt=_tmp309;struct Cyc_Absyn_Exp*num_elts=_tmp30A;int is_fat=_tmp30B;int inline_call=_tmp30C;
# 2766
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(topt)));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2775
if(is_calloc){
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp49D[3U];_tmp49D[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp9EC=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp49D[1]=_tmp9EC;}),_tmp49D[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp9ED,_tag_fat(_tmp49D,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2781
pexp=({void*_tmp9EF=*topt;struct Cyc_Absyn_Exp*_tmp9EE=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9EF,_tmp9EE,num_elts2);});}}else{
# 2783
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
if(inline_call)
pexp=Cyc_Toc_rmalloc_inline_exp(rgnopt,num_elts2);else{
# 2788
pexp=Cyc_Toc_rmalloc_exp(rgnopt,num_elts2);}}else{
# 2790
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2792
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp49E[3U];_tmp49E[0]=pexp,_tmp49E[1]=elt_sz,_tmp49E[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9F0=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp9F0,_tag_fat(_tmp49E,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp9F3=(struct _tuple1*)_check_null(x);void*_tmp9F2=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9F1=num_elts;Cyc_Absyn_declare_stmt(_tmp9F3,_tmp9F2,_tmp9F1,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp9F4=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9F4;});}else{
# 2799
e->r=pexp->r;}
goto _LL7;}}case 35U: _LL54: _tmp305=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp306=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp305;struct Cyc_Absyn_Exp*e2=_tmp306;
# 2809
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EA;_tmp7EA.tag=0U,({struct _fat_ptr _tmp9F5=({const char*_tmp4A1="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp4A1,sizeof(char),57U);});_tmp7EA.f1=_tmp9F5;});_tmp7EA;});void*_tmp49F[1U];_tmp49F[0]=& _tmp4A0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp49F,sizeof(void*),1U));});{
# 2817
unsigned _tmp4A2=e->loc;unsigned loc=_tmp4A2;
struct _tuple1*_tmp4A3=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp4A3;
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp4A4;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp4A5=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp4A5;
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp4A6;v2e->topt=e2_old_typ;{
# 2823
struct Cyc_Absyn_Exp*_tmp4A7=({struct Cyc_Absyn_Exp*_tmp9F7=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp9F6=v2e;Cyc_Absyn_assign_exp(_tmp9F7,_tmp9F6,loc);});struct Cyc_Absyn_Exp*s1e=_tmp4A7;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp4A8;
struct Cyc_Absyn_Exp*_tmp4A9=({struct Cyc_Absyn_Exp*_tmp9F9=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp9F8=v1e;Cyc_Absyn_assign_exp(_tmp9F9,_tmp9F8,loc);});struct Cyc_Absyn_Exp*s2e=_tmp4A9;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4AA=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp4AA;
# 2828
struct Cyc_Absyn_Stmt*_tmp4AB=({struct _tuple1*_tmpA01=v1;void*_tmpA00=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9FF=e1;struct Cyc_Absyn_Stmt*_tmp9FE=({
struct _tuple1*_tmp9FD=v2;void*_tmp9FC=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9FB=e2;struct Cyc_Absyn_Stmt*_tmp9FA=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2829
Cyc_Absyn_declare_stmt(_tmp9FD,_tmp9FC,_tmp9FB,_tmp9FA,loc);});
# 2828
Cyc_Absyn_declare_stmt(_tmpA01,_tmpA00,_tmp9FF,_tmp9FE,loc);});struct Cyc_Absyn_Stmt*s=_tmp4AB;
# 2831
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmpA02=Cyc_Toc_stmt_exp_r(s);e->r=_tmpA02;});
goto _LL7;}}}}}}case 38U: _LL56: _tmp303=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp304=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LL57: {struct Cyc_Absyn_Exp*e1=_tmp303;struct _fat_ptr*f=_tmp304;
# 2836
void*_tmp4AC=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp4AC;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp4AD=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp4AE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)->f1).KnownAggr).tag == 2){_LL11C: _tmp4AE=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)->f1).KnownAggr).val;_LL11D: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4AE;
# 2840
struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp4AF;
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp4B0;
struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp4B1;
({void*_tmpA03=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmpA03;});
goto _LL11B;}}else{goto _LL11E;}}else{goto _LL11E;}}else{_LL11E: _LL11F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EB;_tmp7EB.tag=0U,({struct _fat_ptr _tmpA04=({const char*_tmp4B4="non-aggregate type in tagcheck";_tag_fat(_tmp4B4,sizeof(char),31U);});_tmp7EB.f1=_tmpA04;});_tmp7EB;});void*_tmp4B2[1U];_tmp4B2[0]=& _tmp4B3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B2,sizeof(void*),1U));});}_LL11B:;}
# 2847
goto _LL7;}case 37U: _LL58: _tmp302=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_LL59: {struct Cyc_Absyn_Stmt*s=_tmp302;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36U: _LL5A: _LL5B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EC;_tmp7EC.tag=0U,({struct _fat_ptr _tmpA05=({const char*_tmp4B7="UnresolvedMem";_tag_fat(_tmp4B7,sizeof(char),14U);});_tmp7EC.f1=_tmpA05;});_tmp7EC;});void*_tmp4B5[1U];_tmp4B5[0]=& _tmp4B6;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B5,sizeof(void*),1U));});case 25U: _LL5C: _LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7ED;_tmp7ED.tag=0U,({struct _fat_ptr _tmpA06=({const char*_tmp4BA="compoundlit";_tag_fat(_tmp4BA,sizeof(char),12U);});_tmp7ED.f1=_tmpA06;});_tmp7ED;});void*_tmp4B8[1U];_tmp4B8[0]=& _tmp4B9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B8,sizeof(void*),1U));});case 39U: _LL5E: _LL5F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EE;_tmp7EE.tag=0U,({struct _fat_ptr _tmpA07=({const char*_tmp4BD="valueof(-)";_tag_fat(_tmp4BD,sizeof(char),11U);});_tmp7EE.f1=_tmpA07;});_tmp7EE;});void*_tmp4BB[1U];_tmp4BB[0]=& _tmp4BC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4BB,sizeof(void*),1U));});default: _LL60: _tmp300=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2FF)->f3;_tmp301=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2FF)->f4;_LL61: {struct Cyc_List_List*o=_tmp300;struct Cyc_List_List*i=_tmp301;
# 2853
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2857
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F0;_tmp7F0.tag=0U,({struct _fat_ptr _tmpA08=({const char*_tmp4C1="Toc did not examine an inserted check: ";_tag_fat(_tmp4C1,sizeof(char),40U);});_tmp7F0.f1=_tmpA08;});_tmp7F0;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7EF;_tmp7EF.tag=4U,_tmp7EF.f1=e;_tmp7EF;});void*_tmp4BE[2U];_tmp4BE[0]=& _tmp4BF,_tmp4BE[1]=& _tmp4C0;({unsigned _tmpA09=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmpA09,_tag_fat(_tmp4BE,sizeof(void*),2U));});});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2886 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2888
return({struct _tuple37*_tmp4C2=_region_malloc(r,sizeof(*_tmp4C2));_tmp4C2->f1=0,({struct _fat_ptr*_tmpA0B=Cyc_Toc_fresh_label();_tmp4C2->f2=_tmpA0B;}),({struct _fat_ptr*_tmpA0A=Cyc_Toc_fresh_label();_tmp4C2->f3=_tmpA0A;}),_tmp4C2->f4=sc;_tmp4C2;});}
# 2894
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4C3=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp4C3;
# 2900
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4C6=p->orig_pat;if((_tmp4C6.pattern).tag != 1)_throw_match();(_tmp4C6.pattern).val;})->topt);
void*_tmp4C4=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp4C4;
void*_tmp4C5=t2;switch(*((int*)_tmp4C5)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C5)->f1)){case 0U: _LL1: _LL2:
# 2905
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmpA0C=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA0C,v);});goto _LL0;}_LL0:;}{
# 2911
void*_tmp4C7=p->access;void*_stmttmp4E=_tmp4C7;void*_tmp4C8=_stmttmp4E;struct _fat_ptr*_tmp4CA;int _tmp4C9;unsigned _tmp4CB;unsigned _tmp4CC;switch(*((int*)_tmp4C8)){case 0U: _LLA: _LLB:
# 2915
 goto _LL9;case 1U: _LLC: _LLD:
# 2920
 if(ps->tl != 0){
void*_tmp4CD=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4F=_tmp4CD;void*_tmp4CE=_stmttmp4F;unsigned _tmp4D1;struct Cyc_Absyn_Datatypefield*_tmp4D0;struct Cyc_Absyn_Datatypedecl*_tmp4CF;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CE)->tag == 3U){_LL15: _tmp4CF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CE)->f1;_tmp4D0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CE)->f2;_tmp4D1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CE)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4CF;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D0;unsigned i=_tmp4D1;
# 2923
ps=ps->tl;
v=({void*_tmpA0E=({void*_tmpA0D=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA0D,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA0E,v);});
v=({struct Cyc_Absyn_Exp*_tmpA0F=v;Cyc_Absyn_aggrarrow_exp(_tmpA0F,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2930
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp4CC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C8)->f3;_LLF: {unsigned i=_tmp4CC;
_tmp4CB=i;goto _LL11;}case 2U: _LL10: _tmp4CB=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4C8)->f1;_LL11: {unsigned i=_tmp4CB;
v=({struct Cyc_Absyn_Exp*_tmpA10=v;Cyc_Toc_member_exp(_tmpA10,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _LL12: _tmp4C9=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C8)->f1;_tmp4CA=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C8)->f2;_LL13: {int tagged=_tmp4C9;struct _fat_ptr*f=_tmp4CA;
# 2935
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2941
return v;}
# 2946
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4D2=t;struct Cyc_Absyn_Datatypefield*_tmp4D4;struct Cyc_Absyn_Datatypedecl*_tmp4D3;int _tmp4D6;struct _fat_ptr*_tmp4D5;struct Cyc_Absyn_Datatypefield*_tmp4D9;struct Cyc_Absyn_Datatypedecl*_tmp4D8;int _tmp4D7;unsigned _tmp4DA;int _tmp4DC;struct _fat_ptr _tmp4DB;struct Cyc_Absyn_Enumfield*_tmp4DE;void*_tmp4DD;struct Cyc_Absyn_Enumfield*_tmp4E0;struct Cyc_Absyn_Enumdecl*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E1;switch(*((int*)_tmp4D2)){case 0U: _LL1: _tmp4E1=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4D2)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp4E1;
return e == 0?v: e;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA11=v;Cyc_Absyn_eq_exp(_tmpA11,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA12=v;Cyc_Absyn_neq_exp(_tmpA12,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4DF=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D2)->f1;_tmp4E0=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D2)->f2;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp4DF;struct Cyc_Absyn_Enumfield*ef=_tmp4E0;
return({struct Cyc_Absyn_Exp*_tmpA13=v;Cyc_Absyn_eq_exp(_tmpA13,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->tag=32U,_tmp4E2->f1=ed,_tmp4E2->f2=ef;_tmp4E2;}),0U),0U);});}case 4U: _LL9: _tmp4DD=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D2)->f1;_tmp4DE=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D2)->f2;_LLA: {void*t=_tmp4DD;struct Cyc_Absyn_Enumfield*ef=_tmp4DE;
return({struct Cyc_Absyn_Exp*_tmpA14=v;Cyc_Absyn_eq_exp(_tmpA14,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->tag=33U,_tmp4E3->f1=t,_tmp4E3->f2=ef;_tmp4E3;}),0U),0U);});}case 5U: _LLB: _tmp4DB=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4D2)->f1;_tmp4DC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4D2)->f2;_LLC: {struct _fat_ptr s=_tmp4DB;int i=_tmp4DC;
return({struct Cyc_Absyn_Exp*_tmpA15=v;Cyc_Absyn_eq_exp(_tmpA15,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6U: _LLD: _tmp4DA=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4D2)->f1;_LLE: {unsigned i=_tmp4DA;
return({struct Cyc_Absyn_Exp*_tmpA16=v;Cyc_Absyn_eq_exp(_tmpA16,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7U: _LLF: _tmp4D7=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D2)->f1;_tmp4D8=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D2)->f2;_tmp4D9=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D2)->f3;_LL10: {int i=_tmp4D7;struct Cyc_Absyn_Datatypedecl*tud=_tmp4D8;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D9;
# 2958
LOOP1: {
void*_tmp4E4=v->r;void*_stmttmp50=_tmp4E4;void*_tmp4E5=_stmttmp50;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;switch(*((int*)_tmp4E5)){case 14U: _LL16: _tmp4E7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E5)->f2;_LL17: {struct Cyc_Absyn_Exp*e=_tmp4E7;
v=e;goto LOOP1;}case 20U: _LL18: _tmp4E6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E5)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp4E6;
v=e;goto _LL15;}default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2965
v=({void*_tmpA18=({void*_tmpA17=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA17,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA18,v);});return({struct Cyc_Absyn_Exp*_tmpA19=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA19,Cyc_Absyn_uint_exp((unsigned)i,0U),0U);});}case 8U: _LL11: _tmp4D5=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D2)->f1;_tmp4D6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D2)->f2;_LL12: {struct _fat_ptr*f=_tmp4D5;int i=_tmp4D6;
# 2969
return({struct Cyc_Absyn_Exp*_tmpA1B=({struct Cyc_Absyn_Exp*_tmpA1A=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmpA1A,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmpA1B,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _LL13: _tmp4D3=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4D2)->f1;_tmp4D4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4D2)->f2;_LL14: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4D3;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D4;
# 2973
LOOP2: {
void*_tmp4E8=v->r;void*_stmttmp51=_tmp4E8;void*_tmp4E9=_stmttmp51;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;switch(*((int*)_tmp4E9)){case 14U: _LL1D: _tmp4EB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E9)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4EB;
v=e;goto LOOP2;}case 20U: _LL1F: _tmp4EA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E9)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4EA;
v=e;goto _LL1C;}default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2980
v=({void*_tmpA1D=({void*_tmpA1C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA1C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA1D,v);});
return({struct Cyc_Absyn_Exp*_tmpA1E=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA1E,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2992
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple38{int f1;void*f2;};
# 2999
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3001
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _tmp4EC=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp52=_tmp4EC;struct _tuple34 _tmp4ED=_stmttmp52;void*_tmp4EE;_LL1: _tmp4EE=_tmp4ED.f1;_LL2: {void*ptest=_tmp4EE;
void*_tmp4EF=ptest;struct Cyc_Absyn_Exp*_tmp4F0;struct _fat_ptr*_tmp4F1;switch(*((int*)_tmp4EF)){case 3U: _LL4: _LL5:
# 3007
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4F1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4EF)->f1;_LLB: {struct _fat_ptr*f=_tmp4F1;
# 3011
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->tag=3U,_tmp4F2->f1=f;_tmp4F2;});
continue;}case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4F0=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4EF)->f1;if(_tmp4F0 != 0){_LLF: {struct Cyc_Absyn_Exp*e=_tmp4F0;
# 3016
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->tag=2U,_tmp4F3->f1=e;_tmp4F3;});return({struct _tuple38 _tmp7F1;_tmp7F1.f1=0,_tmp7F1.f2=k;_tmp7F1;});}}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple38 _tmp7F2;_tmp7F2.f1=0,_tmp7F2.f2=k;_tmp7F2;});}_LL3:;}}
# 3024
return({struct _tuple38 _tmp7F3;_tmp7F3.f1=c,_tmp7F3.f2=k;_tmp7F3;});}
# 3029
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4F4=p;int _tmp4F5;int _tmp4F6;unsigned _tmp4F7;struct Cyc_Absyn_Enumfield*_tmp4F9;void*_tmp4F8;struct Cyc_Absyn_Enumfield*_tmp4FB;struct Cyc_Absyn_Enumdecl*_tmp4FA;switch(*((int*)_tmp4F4)){case 3U: _LL1: _tmp4FA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F4)->f1;_tmp4FB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F4)->f2;_LL2: {struct Cyc_Absyn_Enumdecl*ed=_tmp4FA;struct Cyc_Absyn_Enumfield*ef=_tmp4FB;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->tag=32U,_tmp4FC->f1=ed,_tmp4FC->f2=ef;_tmp4FC;}),0U);goto _LL0;}case 4U: _LL3: _tmp4F8=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F4)->f1;_tmp4F9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F4)->f2;_LL4: {void*t=_tmp4F8;struct Cyc_Absyn_Enumfield*ef=_tmp4F9;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->tag=33U,_tmp4FD->f1=t,_tmp4FD->f2=ef;_tmp4FD;}),0U);goto _LL0;}case 6U: _LL5: _tmp4F7=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4F4)->f1;_LL6: {unsigned i=_tmp4F7;
_tmp4F6=(int)i;goto _LL8;}case 7U: _LL7: _tmp4F6=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4F4)->f1;_LL8: {int i=_tmp4F6;
_tmp4F5=i;goto _LLA;}case 8U: _LL9: _tmp4F5=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4F4)->f2;_LLA: {int i=_tmp4F5;
e=Cyc_Absyn_uint_exp((unsigned)i,0U);goto _LL0;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F4;_tmp7F4.tag=0U,({struct _fat_ptr _tmpA1F=({const char*_tmp500="compile_pat_test_as_case!";_tag_fat(_tmp500,sizeof(char),26U);});_tmp7F4.f1=_tmpA1F;});_tmp7F4;});void*_tmp4FE[1U];_tmp4FE[0]=& _tmp4FF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4FE,sizeof(void*),1U));});}_LL0:;}
# 3039
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->tag=17U,_tmp501->f1=e;_tmp501;}),0U);}
# 3043
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple40{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3054
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3058
void*t=(void*)_check_null(p->topt);
void*_tmp502=p->r;void*_stmttmp53=_tmp502;void*_tmp503=_stmttmp53;struct Cyc_List_List*_tmp505;union Cyc_Absyn_AggrInfo _tmp504;struct Cyc_List_List*_tmp506;struct Cyc_List_List*_tmp507;struct Cyc_Absyn_Pat*_tmp508;struct Cyc_List_List*_tmp50B;struct Cyc_Absyn_Datatypefield*_tmp50A;struct Cyc_Absyn_Datatypedecl*_tmp509;struct Cyc_Absyn_Pat*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50E;struct Cyc_Absyn_Pat*_tmp510;struct Cyc_Absyn_Vardecl*_tmp50F;struct Cyc_Absyn_Vardecl*_tmp511;switch(*((int*)_tmp503)){case 2U: _LL1: _tmp511=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp503)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp511;
# 3061
struct Cyc_Absyn_Pat*_tmp512=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp512;
p2->topt=p->topt;
_tmp50F=vd;_tmp510=p2;goto _LL4;}case 1U: _LL3: _tmp50F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp503)->f1;_tmp510=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp503)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp50F;struct Cyc_Absyn_Pat*p2=_tmp510;
# 3066
struct Cyc_Absyn_Vardecl*_tmp513=({struct _tuple1*_tmpA20=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA20,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp513;
({struct Cyc_List_List*_tmpA22=({struct Cyc_List_List*_tmp515=_region_malloc(rgn,sizeof(*_tmp515));({struct _tuple39*_tmpA21=({struct _tuple39*_tmp514=_region_malloc(rgn,sizeof(*_tmp514));_tmp514->f1=vd,_tmp514->f2=new_vd;_tmp514;});_tmp515->hd=_tmpA21;}),_tmp515->tl=*decls;_tmp515;});*decls=_tmpA22;});{
struct Cyc_Absyn_Stmt*_tmp516=({struct Cyc_Absyn_Exp*_tmpA23=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->tag=4U,_tmp517->f1=new_vd;_tmp517;}),0U);Cyc_Absyn_assign_stmt(_tmpA23,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp516;
return({struct Cyc_Absyn_Stmt*_tmpA24=s;Cyc_Toc_seq_stmt_opt(_tmpA24,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4U: _LL5: _tmp50E=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp503)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp50E;
# 3072
struct Cyc_Absyn_Vardecl*_tmp518=({struct _tuple1*_tmpA25=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA25,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp518;
({struct Cyc_List_List*_tmpA27=({struct Cyc_List_List*_tmp51A=_region_malloc(rgn,sizeof(*_tmp51A));({struct _tuple39*_tmpA26=({struct _tuple39*_tmp519=_region_malloc(rgn,sizeof(*_tmp519));_tmp519->f1=vd,_tmp519->f2=new_vd;_tmp519;});_tmp51A->hd=_tmpA26;}),_tmp51A->tl=*decls;_tmp51A;});*decls=_tmpA27;});
return({struct Cyc_Absyn_Exp*_tmpA28=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->tag=4U,_tmp51B->f1=new_vd;_tmp51B;}),0U);Cyc_Absyn_assign_stmt(_tmpA28,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp50C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp503)->f1;_tmp50D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp503)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp50C;struct Cyc_Absyn_Pat*p2=_tmp50D;
# 3078
({void*_tmpA29=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmpA29;});{
struct Cyc_Absyn_Vardecl*_tmp51C=({struct _tuple1*_tmpA2A=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA2A,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp51C;
({struct Cyc_List_List*_tmpA2C=({struct Cyc_List_List*_tmp51E=_region_malloc(rgn,sizeof(*_tmp51E));({struct _tuple39*_tmpA2B=({struct _tuple39*_tmp51D=_region_malloc(rgn,sizeof(*_tmp51D));_tmp51D->f1=vd,_tmp51D->f2=new_vd;_tmp51D;});_tmp51E->hd=_tmpA2B;}),_tmp51E->tl=*decls;_tmp51E;});*decls=_tmpA2C;});{
# 3082
struct Cyc_Absyn_Stmt*_tmp51F=({struct Cyc_Absyn_Exp*_tmpA2F=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->tag=4U,_tmp520->f1=new_vd;_tmp520;}),0U);Cyc_Absyn_assign_stmt(_tmpA2F,({
void*_tmpA2E=({void*_tmpA2D=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA2D,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA2E,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3082
struct Cyc_Absyn_Stmt*s=_tmp51F;
# 3085
return({struct Cyc_Absyn_Stmt*_tmpA30=s;Cyc_Toc_seq_stmt_opt(_tmpA30,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9U: _LLB: _LLC:
# 3087
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp503)->f1)->r)->tag == 8U){_LL17: _tmp509=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp503)->f1)->r)->f1;_tmp50A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp503)->f1)->r)->f2;_tmp50B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp503)->f1)->r)->f3;_LL18: {struct Cyc_Absyn_Datatypedecl*tud=_tmp509;struct Cyc_Absyn_Datatypefield*tuf=_tmp50A;struct Cyc_List_List*ps=_tmp50B;
# 3096
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp521=({void*_tmpA31=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA31,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp521;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp522=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp522;
struct Cyc_Absyn_Stmt*s=0;
for(0;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp523=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp523;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp524=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp524;
void*_tmp525=(void*)_check_null(p2->topt);void*t2=_tmp525;
void*_tmp526=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp526;
struct Cyc_Absyn_Exp*_tmp527=({struct Cyc_Absyn_Exp*_tmpA32=path;Cyc_Absyn_aggrarrow_exp(_tmpA32,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp527;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmpA33=s;Cyc_Toc_seq_stmt_opt(_tmpA33,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3114
return s;}}}}else{_LL21: _tmp508=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp503)->f1;_LL22: {struct Cyc_Absyn_Pat*p2=_tmp508;
# 3159
return({struct _RegionHandle*_tmpA37=rgn;struct Cyc_Toc_Env**_tmpA36=nv;struct Cyc_List_List**_tmpA35=decls;struct Cyc_Absyn_Exp*_tmpA34=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA37,_tmpA36,_tmpA35,_tmpA34,p2);});}}case 8U: _LL19: _tmp507=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp503)->f3;_LL1A: {struct Cyc_List_List*ps=_tmp507;
# 3116
_tmp506=ps;goto _LL1C;}case 5U: _LL1B: _tmp506=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp503)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp506;
# 3118
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp528=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp528;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp529=(void*)_check_null(p2->topt);void*t2=_tmp529;
struct _fat_ptr*_tmp52A=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp52A;
s=({struct Cyc_Absyn_Stmt*_tmpA3C=s;Cyc_Toc_seq_stmt_opt(_tmpA3C,({struct _RegionHandle*_tmpA3B=rgn;struct Cyc_Toc_Env**_tmpA3A=nv;struct Cyc_List_List**_tmpA39=decls;struct Cyc_Absyn_Exp*_tmpA38=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmpA3B,_tmpA3A,_tmpA39,_tmpA38,p2);}));});}}
# 3128
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp503)->f1 == 0){_LL1D: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F5;_tmp7F5.tag=0U,({struct _fat_ptr _tmpA3D=({const char*_tmp52D="unresolved aggregate pattern!";_tag_fat(_tmp52D,sizeof(char),30U);});_tmp7F5.f1=_tmpA3D;});_tmp7F5;});void*_tmp52B[1U];_tmp52B[0]=& _tmp52C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp52B,sizeof(void*),1U));});}else{_LL1F: _tmp504=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp503)->f1;_tmp505=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp503)->f3;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp504;struct Cyc_List_List*dlps=_tmp505;
# 3132
struct Cyc_Absyn_Aggrdecl*_tmp52E=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp52E;
struct Cyc_Absyn_Stmt*s=0;
for(0;dlps != 0;dlps=dlps->tl){
struct _tuple40*_tmp52F=(struct _tuple40*)dlps->hd;struct _tuple40*tup=_tmp52F;
struct Cyc_Absyn_Pat*_tmp530=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp530;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp531=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp531;
void*_tmp532=(void*)_check_null(p2->topt);void*t2=_tmp532;
void*_tmp533=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp533;
struct Cyc_Absyn_Exp*_tmp534=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp534;
# 3144
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp535=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp535;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(ftype)))
# 3151
memexp=Cyc_Absyn_deref_exp(({void*_tmpA3E=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA3E,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3154
s=({struct Cyc_Absyn_Stmt*_tmpA3F=s;Cyc_Toc_seq_stmt_opt(_tmpA3F,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3156
return s;}}case 15U: _LL23: _LL24:
# 3161
({struct Cyc_Warn_String_Warn_Warg_struct _tmp537=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F6;_tmp7F6.tag=0U,({struct _fat_ptr _tmpA40=({const char*_tmp538="unknownid pat";_tag_fat(_tmp538,sizeof(char),14U);});_tmp7F6.f1=_tmpA40;});_tmp7F6;});void*_tmp536[1U];_tmp536[0]=& _tmp537;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp536,sizeof(void*),1U));});case 16U: _LL25: _LL26:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F7;_tmp7F7.tag=0U,({struct _fat_ptr _tmpA41=({const char*_tmp53B="unknowncall pat";_tag_fat(_tmp53B,sizeof(char),16U);});_tmp7F7.f1=_tmpA41;});_tmp7F7;});void*_tmp539[1U];_tmp539[0]=& _tmp53A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp539,sizeof(void*),1U));});default: _LL27: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F8;_tmp7F8.tag=0U,({struct _fat_ptr _tmpA42=({const char*_tmp53E="exp pat";_tag_fat(_tmp53E,sizeof(char),8U);});_tmp7F8.f1=_tmpA42;});_tmp7F8;});void*_tmp53C[1U];_tmp53C[0]=& _tmp53D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp53C,sizeof(void*),1U));});}_LL0:;}
# 3167
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple39 _tmp53F=*((struct _tuple39*)vs->hd);struct _tuple39 _stmttmp54=_tmp53F;struct _tuple39 _tmp540=_stmttmp54;struct Cyc_Absyn_Vardecl*_tmp542;struct Cyc_Absyn_Vardecl*_tmp541;_LL1: _tmp541=_tmp540.f1;_tmp542=_tmp540.f2;_LL2: {struct Cyc_Absyn_Vardecl*oldv=_tmp541;struct Cyc_Absyn_Vardecl*newv=_tmp542;
if(oldv == newv)return newv;}}
# 3172
({void*_tmp543=0U;({struct _fat_ptr _tmpA43=({const char*_tmp544="find_new_var";_tag_fat(_tmp544,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA43,_tag_fat(_tmp543,sizeof(void*),0U));});});}
# 3178
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp545=e->r;void*_stmttmp55=_tmp545;void*_tmp546=_stmttmp55;struct Cyc_List_List*_tmp547;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54D;struct Cyc_List_List*_tmp54E;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_Absyn_Exp*_tmp565;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56C;struct Cyc_Absyn_Vardecl*_tmp56D;switch(*((int*)_tmp546)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp546)->f1)){case 5U: _LL1: _tmp56D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp546)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp56D;
_tmp56C=vd;goto _LL4;}case 4U: _LL3: _tmp56C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp546)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp56C;
# 3182
{struct _handler_cons _tmp56E;_push_handler(& _tmp56E);{int _tmp570=0;if(setjmp(_tmp56E.handler))_tmp570=1;if(!_tmp570){
({void*_tmpA46=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=1U,({void*_tmpA45=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->tag=4U,({struct Cyc_Absyn_Vardecl*_tmpA44=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,vd);_tmp571->f1=_tmpA44;});_tmp571;});_tmp572->f1=_tmpA45;});_tmp572;});e->r=_tmpA46;});;_pop_handler();}else{void*_tmp56F=(void*)Cyc_Core_get_exn_thrown();void*_tmp573=_tmp56F;void*_tmp574;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp573)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3185
 goto _LL3F;}else{_LL42: _tmp574=_tmp573;_LL43: {void*exn=_tmp574;(int)_rethrow(exn);}}_LL3F:;}}}
# 3187
goto _LL0;}default: goto _LL3D;}case 6U: _LL5: _tmp569=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp56A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_tmp56B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp546)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp569;struct Cyc_Absyn_Exp*e2=_tmp56A;struct Cyc_Absyn_Exp*e3=_tmp56B;
# 3189
Cyc_Toc_subst_pattern_vars(env,e1);_tmp567=e2;_tmp568=e3;goto _LL8;}case 27U: _LL7: _tmp567=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_tmp568=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp546)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp567;struct Cyc_Absyn_Exp*e2=_tmp568;
_tmp565=e1;_tmp566=e2;goto _LLA;}case 7U: _LL9: _tmp565=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp566=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp565;struct Cyc_Absyn_Exp*e2=_tmp566;
_tmp563=e1;_tmp564=e2;goto _LLC;}case 8U: _LLB: _tmp563=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp564=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp563;struct Cyc_Absyn_Exp*e2=_tmp564;
_tmp561=e1;_tmp562=e2;goto _LLE;}case 23U: _LLD: _tmp561=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp562=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp561;struct Cyc_Absyn_Exp*e2=_tmp562;
_tmp55F=e1;_tmp560=e2;goto _LL10;}case 9U: _LLF: _tmp55F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp560=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp55F;struct Cyc_Absyn_Exp*e2=_tmp560;
# 3195
Cyc_Toc_subst_pattern_vars(env,e1);_tmp55E=e2;goto _LL12;}case 41U: _LL11: _tmp55E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp55E;
_tmp55D=e;goto _LL14;}case 38U: _LL13: _tmp55D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp55D;
_tmp55C=e;goto _LL16;}case 11U: _LL15: _tmp55C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp55C;
_tmp55B=e;goto _LL18;}case 12U: _LL17: _tmp55B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp55B;
_tmp55A=e;goto _LL1A;}case 13U: _LL19: _tmp55A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp55A;
_tmp559=e;goto _LL1C;}case 14U: _LL1B: _tmp559=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp559;
_tmp558=e;goto _LL1E;}case 18U: _LL1D: _tmp558=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp558;
_tmp557=e;goto _LL20;}case 20U: _LL1F: _tmp557=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp557;
_tmp556=e;goto _LL22;}case 21U: _LL21: _tmp556=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp556;
_tmp555=e;goto _LL24;}case 22U: _LL23: _tmp555=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp555;
_tmp554=e;goto _LL26;}case 28U: _LL25: _tmp554=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp554;
_tmp553=e;goto _LL28;}case 15U: _LL27: _tmp553=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp553;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34U: _LL29: _tmp551=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp546)->f1).rgn;_tmp552=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp546)->f1).num_elts;_LL2A: {struct Cyc_Absyn_Exp*eopt=_tmp551;struct Cyc_Absyn_Exp*e=_tmp552;
_tmp54F=eopt;_tmp550=e;goto _LL2C;}case 16U: _LL2B: _tmp54F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp550=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp54F;struct Cyc_Absyn_Exp*e=_tmp550;
# 3210
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3U: _LL2D: _tmp54E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL2E: {struct Cyc_List_List*es=_tmp54E;
_tmp54D=es;goto _LL30;}case 31U: _LL2F: _tmp54D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL30: {struct Cyc_List_List*es=_tmp54D;
_tmp54C=es;goto _LL32;}case 24U: _LL31: _tmp54C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL32: {struct Cyc_List_List*es=_tmp54C;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36U: _LL33: _tmp54B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL34: {struct Cyc_List_List*dles=_tmp54B;
_tmp54A=dles;goto _LL36;}case 29U: _LL35: _tmp54A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp546)->f3;_LL36: {struct Cyc_List_List*dles=_tmp54A;
_tmp549=dles;goto _LL38;}case 30U: _LL37: _tmp549=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL38: {struct Cyc_List_List*dles=_tmp549;
_tmp548=dles;goto _LL3A;}case 26U: _LL39: _tmp548=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp548;
_tmp547=dles;goto _LL3C;}case 25U: _LL3B: _tmp547=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp547;
# 3220
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp575=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp56=_tmp575;struct _tuple19*_tmp576=_stmttmp56;struct Cyc_Absyn_Exp*_tmp577;_LL45: _tmp577=_tmp576->f2;_LL46: {struct Cyc_Absyn_Exp*e=_tmp577;
Cyc_Toc_subst_pattern_vars(env,e);}}
# 3224
goto _LL0;}default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple41{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3234
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3242
void*_tmp578=dopt;void*_tmp57B;struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp579;struct Cyc_Tcpat_Rhs*_tmp57C;if(_tmp578 == 0){_LL1: _LL2:
# 3244
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp578)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp57C=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp578)->f1;_LL6: {struct Cyc_Tcpat_Rhs*rhs=_tmp57C;
# 3248
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp57D=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp57=_tmp57D;struct _tuple37*_tmp57E=_stmttmp57;struct Cyc_Absyn_Switch_clause*_tmp582;struct _fat_ptr*_tmp581;struct _fat_ptr*_tmp580;int*_tmp57F;_LLA: _tmp57F=(int*)& _tmp57E->f1;_tmp580=_tmp57E->f2;_tmp581=_tmp57E->f3;_tmp582=_tmp57E->f4;_LLB: {int*already_emitted=_tmp57F;struct _fat_ptr*init_lab=_tmp580;struct _fat_ptr*code_lab=_tmp581;struct Cyc_Absyn_Switch_clause*sc=_tmp582;
struct Cyc_Absyn_Stmt*_tmp583=sc->body;struct Cyc_Absyn_Stmt*body=_tmp583;
if(body == rhs->rhs){
# 3253
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3258
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA48=rgn;struct Cyc_Absyn_Exp*_tmpA47=
Cyc_Absyn_var_exp(v,0U);
# 3258
Cyc_Toc_extract_pattern_vars(_tmpA48,& nv,& newdecls,_tmpA47,sc->pattern);});
# 3262
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp584=newdecls;struct Cyc_List_List*ds=_tmp584;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp585=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp58=_tmp585;struct _tuple39 _tmp586=_stmttmp58;struct Cyc_Absyn_Vardecl*_tmp588;struct Cyc_Absyn_Vardecl*_tmp587;_LLD: _tmp587=_tmp586.f1;_tmp588=_tmp586.f2;_LLE: {struct Cyc_Absyn_Vardecl*oldv=_tmp587;struct Cyc_Absyn_Vardecl*newv=_tmp588;
({struct Cyc_List_List*_tmpA4A=({struct Cyc_List_List*_tmp58A=_region_malloc(rgn,sizeof(*_tmp58A));({struct _tuple39*_tmpA49=({struct _tuple39*_tmp589=_region_malloc(rgn,sizeof(*_tmp589));_tmp589->f1=oldv,_tmp589->f2=newv;_tmp589;});_tmp58A->hd=_tmpA49;}),_tmp58A->tl=*decls;_tmp58A;});*decls=_tmpA4A;});
({struct Cyc_Absyn_Exp*_tmpA4B=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->tag=4U,_tmp58B->f1=newv;_tmp58B;}),0U);oldv->initializer=_tmpA4B;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmpA4D=({struct Cyc_Absyn_Decl*_tmp58D=_cycalloc(sizeof(*_tmp58D));({void*_tmpA4C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->tag=0U,_tmp58C->f1=oldv;_tmp58C;});_tmp58D->r=_tmpA4C;}),_tmp58D->loc=0U;_tmp58D;});Cyc_Absyn_decl_stmt(_tmpA4D,res,0U);});}}}
# 3271
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmpA4F=({struct Cyc_List_List*_tmp58F=_region_malloc(rgn,sizeof(*_tmp58F));({struct _tuple41*_tmpA4E=({struct _tuple41*_tmp58E=_region_malloc(rgn,sizeof(*_tmp58E));_tmp58E->f1=nv,_tmp58E->f2=code_lab,_tmp58E->f3=body;_tmp58E;});_tmp58F->hd=_tmpA4E;}),_tmp58F->tl=*bodies;_tmp58F;});*bodies=_tmpA4F;});
return res;}}}}
# 3279
({struct Cyc_Warn_String_Warn_Warg_struct _tmp591=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F9;_tmp7F9.tag=0U,({struct _fat_ptr _tmpA50=({const char*_tmp592="couldn't find rhs!";_tag_fat(_tmp592,sizeof(char),19U);});_tmp7F9.f1=_tmpA50;});_tmp7F9;});void*_tmp590[1U];_tmp590[0]=& _tmp591;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp590,sizeof(void*),1U));});}default: _LL7: _tmp579=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp578)->f1;_tmp57A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp578)->f2;_tmp57B=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp578)->f3;_LL8: {struct Cyc_List_List*symbolic_path=_tmp579;struct Cyc_List_List*switches=_tmp57A;void*other_decision=_tmp57B;
# 3282
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3284
struct Cyc_Absyn_Exp*_tmp593=({struct Cyc_List_List*_tmpA51=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmpA51,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp593;
struct Cyc_List_List*_tmp594=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp594;
# 3287
struct _tuple38 _tmp595=Cyc_Toc_admits_switch(ss);struct _tuple38 _stmttmp59=_tmp595;struct _tuple38 _tmp596=_stmttmp59;void*_tmp598;int _tmp597;_LL10: _tmp597=_tmp596.f1;_tmp598=_tmp596.f2;_LL11: {int allows_switch=_tmp597;void*test_kind=_tmp598;
if(allows_switch > 1){
# 3291
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));
({struct Cyc_Absyn_Switch_clause*_tmpA53=({struct Cyc_Absyn_Switch_clause*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));({struct Cyc_Absyn_Pat*_tmpA52=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5A8->pattern=_tmpA52;}),_tmp5A8->pat_vars=0,_tmp5A8->where_clause=0,_tmp5A8->body=res,_tmp5A8->loc=0U;_tmp5A8;});_tmp5A9->hd=_tmpA53;}),_tmp5A9->tl=0;_tmp5A9;});
# 3294
for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp599=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5A=_tmp599;struct _tuple34 _tmp59A=_stmttmp5A;void*_tmp59C;void*_tmp59B;_LL13: _tmp59B=_tmp59A.f1;_tmp59C=_tmp59A.f2;_LL14: {void*pat_test=_tmp59B;void*dec_tree=_tmp59C;
# 3297
struct Cyc_Absyn_Pat*_tmp59D=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp59D;
# 3299
struct Cyc_Absyn_Stmt*_tmp59E=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp59E;
# 3301
new_lscs=({struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));({struct Cyc_Absyn_Switch_clause*_tmpA54=({struct Cyc_Absyn_Switch_clause*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->pattern=case_exp,_tmp59F->pat_vars=0,_tmp59F->where_clause=0,_tmp59F->body=s,_tmp59F->loc=0U;_tmp59F;});_tmp5A0->hd=_tmpA54;}),_tmp5A0->tl=new_lscs;_tmp5A0;});}}
# 3303
{void*_tmp5A1=test_kind;struct _fat_ptr*_tmp5A2;switch(*((int*)_tmp5A1)){case 1U: _LL16: _LL17:
# 3305
 LOOP1: {
void*_tmp5A3=p->r;void*_stmttmp5B=_tmp5A3;void*_tmp5A4=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A6;switch(*((int*)_tmp5A4)){case 14U: _LL1F: _tmp5A6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL20: {struct Cyc_Absyn_Exp*e=_tmp5A6;
p=e;goto LOOP1;}case 20U: _LL21: _tmp5A5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5A5;
p=e;goto _LL1E;}default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3311
p=Cyc_Absyn_deref_exp(({void*_tmpA55=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA55,p);}),0U);goto _LL15;case 3U: _LL18: _tmp5A2=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5A1)->f1;_LL19: {struct _fat_ptr*f=_tmp5A2;
# 3313
p=({struct Cyc_Absyn_Exp*_tmpA56=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmpA56,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3317
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->tag=10U,_tmp5A7->f1=p,_tmp5A7->f2=new_lscs,_tmp5A7->f3=0;_tmp5A7;}),0U);}else{
# 3321
void*_tmp5AA=test_kind;struct Cyc_Absyn_Exp*_tmp5AB;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5AA)->tag == 2U){_LL26: _tmp5AB=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5AA)->f1;_LL27: {struct Cyc_Absyn_Exp*e=_tmp5AB;
# 3323
struct Cyc_List_List*_tmp5AC=ss;struct Cyc_Tcpat_Rhs*_tmp5AE;void*_tmp5AD;if(_tmp5AC != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)((struct Cyc_List_List*)_tmp5AC)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5AC)->tl == 0){_LL2B: _tmp5AD=((struct _tuple34*)_tmp5AC->hd)->f1;_tmp5AE=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_tmp5AC->hd)->f2)->f1;_LL2C: {void*pat_test=_tmp5AD;struct Cyc_Tcpat_Rhs*rhs=_tmp5AE;
# 3329
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp5AF=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp5C=_tmp5AF;struct _tuple37*_tmp5B0=_stmttmp5C;struct Cyc_Absyn_Switch_clause*_tmp5B4;struct _fat_ptr*_tmp5B3;struct _fat_ptr*_tmp5B2;int*_tmp5B1;_LL30: _tmp5B1=(int*)& _tmp5B0->f1;_tmp5B2=_tmp5B0->f2;_tmp5B3=_tmp5B0->f3;_tmp5B4=_tmp5B0->f4;_LL31: {int*already_emitted=_tmp5B1;struct _fat_ptr*init_lab=_tmp5B2;struct _fat_ptr*code_lab=_tmp5B3;struct Cyc_Absyn_Switch_clause*sc=_tmp5B4;
struct Cyc_Absyn_Stmt*_tmp5B5=sc->body;struct Cyc_Absyn_Stmt*body=_tmp5B5;
if(body == rhs->rhs){
# 3334
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3339
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA58=rgn;struct Cyc_Absyn_Exp*_tmpA57=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA58,& nv,& newdecls,_tmpA57,sc->pattern);});
# 3343
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp5B6=newdecls;struct Cyc_List_List*ds=_tmp5B6;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp5B7=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp5D=_tmp5B7;struct _tuple39 _tmp5B8=_stmttmp5D;struct Cyc_Absyn_Vardecl*_tmp5BA;struct Cyc_Absyn_Vardecl*_tmp5B9;_LL33: _tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;_LL34: {struct Cyc_Absyn_Vardecl*oldv=_tmp5B9;struct Cyc_Absyn_Vardecl*newv=_tmp5BA;
({struct Cyc_List_List*_tmpA5A=({struct Cyc_List_List*_tmp5BC=_region_malloc(rgn,sizeof(*_tmp5BC));({struct _tuple39*_tmpA59=({struct _tuple39*_tmp5BB=_region_malloc(rgn,sizeof(*_tmp5BB));_tmp5BB->f1=oldv,_tmp5BB->f2=newv;_tmp5BB;});_tmp5BC->hd=_tmpA59;}),_tmp5BC->tl=*decls;_tmp5BC;});*decls=_tmpA5A;});
({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=4U,_tmp5BD->f1=newv;_tmp5BD;}),0U);oldv->initializer=_tmpA5B;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmpA5D=({struct Cyc_Absyn_Decl*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({void*_tmpA5C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->tag=0U,_tmp5BE->f1=oldv;_tmp5BE;});_tmp5BF->r=_tmpA5C;}),_tmp5BF->loc=0U;_tmp5BF;});Cyc_Absyn_decl_stmt(_tmpA5D,r,0U);});}}}
# 3352
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3356
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmpA5F=({struct Cyc_List_List*_tmp5C1=_region_malloc(rgn,sizeof(*_tmp5C1));({struct _tuple41*_tmpA5E=({struct _tuple41*_tmp5C0=_region_malloc(rgn,sizeof(*_tmp5C0));_tmp5C0->f1=nv,_tmp5C0->f2=code_lab,_tmp5C0->f3=body;_tmp5C0;});_tmp5C1->hd=_tmpA5E;}),_tmp5C1->tl=*bodies;_tmp5C1;});*bodies=_tmpA5F;});
return r;}}}}
# 3366
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FA;_tmp7FA.tag=0U,({struct _fat_ptr _tmpA60=({const char*_tmp5C4="couldn't find rhs!";_tag_fat(_tmp5C4,sizeof(char),19U);});_tmp7FA.f1=_tmpA60;});_tmp7FA;});void*_tmp5C2[1U];_tmp5C2[0]=& _tmp5C3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C2,sizeof(void*),1U));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FB;_tmp7FB.tag=0U,({struct _fat_ptr _tmpA61=({const char*_tmp5C7="bad where clause in match compiler";_tag_fat(_tmp5C7,sizeof(char),35U);});_tmp7FB.f1=_tmpA61;});_tmp7FB;});void*_tmp5C5[1U];_tmp5C5[0]=& _tmp5C6;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C5,sizeof(void*),1U));});}_LL2A:;}}else{_LL28: _LL29:
# 3371
 for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp5C8=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5E=_tmp5C8;struct _tuple34 _tmp5C9=_stmttmp5E;void*_tmp5CB;void*_tmp5CA;_LL36: _tmp5CA=_tmp5C9.f1;_tmp5CB=_tmp5C9.f2;_LL37: {void*pat_test=_tmp5CA;void*dec_tree=_tmp5CB;
struct Cyc_Absyn_Exp*_tmp5CC=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp5CC;
struct Cyc_Absyn_Stmt*_tmp5CD=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp5CD;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}}_LL25:;}
# 3379
return res;}}}}_LL0:;}
# 3389
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3391
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple41*_tmp5CE=(struct _tuple41*)bodies->hd;struct _tuple41*_stmttmp5F=_tmp5CE;struct _tuple41*_tmp5CF=_stmttmp5F;struct Cyc_Absyn_Stmt*_tmp5D1;struct Cyc_Toc_Env**_tmp5D0;_LL1: _tmp5D0=(struct Cyc_Toc_Env**)& _tmp5CF->f1;_tmp5D1=_tmp5CF->f3;_LL2: {struct Cyc_Toc_Env**nv=_tmp5D0;struct Cyc_Absyn_Stmt*s2=_tmp5D1;
if(s2 == s)return nv;}}
# 3397
return 0;}
# 3401
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3404
void*_tmp5D2=(void*)_check_null(e->topt);void*t=_tmp5D2;
Cyc_Toc_exp_to_c(nv,e);{
# 3407
struct _tuple30 _tmp5D3=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp60=_tmp5D3;struct _tuple30 _tmp5D4=_stmttmp60;struct Cyc_Absyn_Exp*_tmp5D6;struct _tuple1*_tmp5D5;_LL1: _tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;_LL2: {struct _tuple1*v=_tmp5D5;struct Cyc_Absyn_Exp*path=_tmp5D6;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5D7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D7;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5D8=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp5D8;
# 3413
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3418
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3426
{struct Cyc_List_List*_tmp5D9=lscs;struct Cyc_List_List*lscs2=_tmp5D9;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_tmp5DA=(struct _tuple37*)lscs2->hd;struct _tuple37*_stmttmp61=_tmp5DA;struct _tuple37*_tmp5DB=_stmttmp61;struct Cyc_Absyn_Switch_clause*_tmp5DD;struct _fat_ptr*_tmp5DC;_LL4: _tmp5DC=_tmp5DB->f3;_tmp5DD=_tmp5DB->f4;_LL5: {struct _fat_ptr*body_lab=_tmp5DC;struct Cyc_Absyn_Switch_clause*body_sc=_tmp5DD;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3430
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3433
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5DE=*envp;struct Cyc_Toc_Env*env=_tmp5DE;
# 3436
if(lscs2->tl != 0){
struct _tuple37*_tmp5DF=(struct _tuple37*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple37*_stmttmp62=_tmp5DF;struct _tuple37*_tmp5E0=_stmttmp62;struct Cyc_Absyn_Switch_clause*_tmp5E2;struct _fat_ptr*_tmp5E1;_LL7: _tmp5E1=_tmp5E0->f3;_tmp5E2=_tmp5E0->f4;_LL8: {struct _fat_ptr*fallthru_lab=_tmp5E1;struct Cyc_Absyn_Switch_clause*next_sc=_tmp5E2;
# 3440
struct Cyc_Toc_Env**_tmp5E3=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp5E3;
# 3445
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmpA62=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA62,s);});else{
# 3449
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmpA63=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmpA63,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3454
({struct Cyc_Toc_Env*_tmpA64=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmpA64,s);});}}}else{
# 3458
({struct Cyc_Toc_Env*_tmpA65=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA65,s);});}}}}}{
# 3462
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA67=test_tree;Cyc_Absyn_seq_stmt(_tmpA67,({struct _fat_ptr*_tmpA66=end_l;Cyc_Absyn_label_stmt(_tmpA66,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3464
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp5E4=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp63=_tmp5E4;struct _tuple39 _tmp5E5=_stmttmp63;struct Cyc_Absyn_Vardecl*_tmp5E6;_LLA: _tmp5E6=_tmp5E5.f2;_LLB: {struct Cyc_Absyn_Vardecl*vd=_tmp5E6;
res=({struct Cyc_Absyn_Decl*_tmpA68=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->tag=0U,_tmp5E7->f1=vd;_tmp5E7;}),0U);Cyc_Absyn_decl_stmt(_tmpA68,res,0U);});}}
# 3469
({void*_tmpA6C=({struct _tuple1*_tmpA6B=v;void*_tmpA6A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA69=e;Cyc_Absyn_declare_stmt(_tmpA6B,_tmpA6A,_tmpA69,res,0U);})->r;whole_s->r=_tmpA6C;});
_npop_handler(0U);return;}}
# 3410
;_pop_region();}}}
# 3475
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3477
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3482
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3484
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5E8[1U];({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp5E8[0]=_tmpA6D;});({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA6E,_tag_fat(_tmp5E8,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3487
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA70=({struct Cyc_Absyn_Stmt*_tmpA6F=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA6F,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA70;});}
# 3492
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3494
while(1){
void*_tmp5E9=s->r;void*_stmttmp64=_tmp5E9;void*_tmp5EA=_stmttmp64;void*_tmp5ED;struct Cyc_List_List*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5EF;struct Cyc_Absyn_Decl*_tmp5EE;struct Cyc_Absyn_Switch_clause**_tmp5F1;struct Cyc_List_List*_tmp5F0;void*_tmp5F4;struct Cyc_List_List*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F6;struct _fat_ptr*_tmp5F5;struct Cyc_Absyn_Stmt*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Stmt*_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Exp*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_Absyn_Stmt*_tmp602;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Exp*_tmp605;switch(*((int*)_tmp5EA)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp605=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp605;
Cyc_Toc_exp_to_c(nv,e);return;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1 == 0){_LL5: _LL6:
({int _tmpA71=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA71,s);});return;}else{_LL7: _tmp604=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp604;
# 3501
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp606=Cyc_Toc_temp_var();struct _tuple1*x=_tmp606;
struct Cyc_Absyn_Stmt*_tmp607=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp607;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmpA72=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmpA72;});}
# 3510
return;}}}case 2U: _LL9: _tmp602=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp603=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_LLA: {struct Cyc_Absyn_Stmt*s1=_tmp602;struct Cyc_Absyn_Stmt*s2=_tmp603;
# 3512
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4U: _LLB: _tmp5FF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp600=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_tmp601=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5FF;struct Cyc_Absyn_Stmt*s1=_tmp600;struct Cyc_Absyn_Stmt*s2=_tmp601;
# 3516
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9U: _LLD: _tmp5FB=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5FC=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2).f1;_tmp5FD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f3).f1;_tmp5FE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5FB;struct Cyc_Absyn_Exp*e2=_tmp5FC;struct Cyc_Absyn_Exp*e3=_tmp5FD;struct Cyc_Absyn_Stmt*s2=_tmp5FE;
# 3521
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5F9=s2;_tmp5FA=e3;goto _LL10;}case 14U: _LLF: _tmp5F9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5FA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5F9;struct Cyc_Absyn_Exp*e=_tmp5FA;
_tmp5F7=e;_tmp5F8=s2;goto _LL12;}case 5U: _LL11: _tmp5F7=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1).f1;_tmp5F8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5F7;struct Cyc_Absyn_Stmt*s2=_tmp5F8;
# 3526
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp608=_new_region("temp");struct _RegionHandle*temp=& _tmp608;_push_region(temp);({struct Cyc_Toc_Env*_tmpA73=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA73,s2);});
_npop_handler(0U);return;
# 3527
;_pop_region();}}case 6U: _LL13: _LL14: {
# 3530
struct Cyc_Toc_Env*_tmp609=nv;struct _fat_ptr**_tmp60A;_LL24: _tmp60A=_tmp609->break_lab;_LL25: {struct _fat_ptr**b=_tmp60A;
if(b != 0)
({void*_tmpA74=Cyc_Toc_goto_stmt_r(*b);s->r=_tmpA74;});
# 3534
({int _tmpA75=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA75,s);});
return;}}case 7U: _LL15: _LL16: {
# 3537
struct Cyc_Toc_Env*_tmp60B=nv;struct _fat_ptr**_tmp60C;_LL27: _tmp60C=_tmp60B->continue_lab;_LL28: {struct _fat_ptr**c=_tmp60C;
if(c != 0)
({void*_tmpA76=Cyc_Toc_goto_stmt_r(*c);s->r=_tmpA76;});
goto _LL18;}}case 8U: _LL17: _LL18:
# 3542
({int _tmpA77=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA77,s);});
return;case 13U: _LL19: _tmp5F5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5F6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_LL1A: {struct _fat_ptr*lab=_tmp5F5;struct Cyc_Absyn_Stmt*s1=_tmp5F6;
s=s1;continue;}case 10U: _LL1B: _tmp5F2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5F3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_tmp5F4=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f3;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5F2;struct Cyc_List_List*scs=_tmp5F3;void*dec_tree_opt=_tmp5F4;
# 3546
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11U: _LL1D: _tmp5F0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5F1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_LL1E: {struct Cyc_List_List*es=_tmp5F0;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5F1;
# 3549
struct Cyc_Toc_Env*_tmp60D=nv;struct Cyc_Toc_FallthruInfo*_tmp60E;_LL2A: _tmp60E=_tmp60D->fallthru_info;_LL2B: {struct Cyc_Toc_FallthruInfo*fi=_tmp60E;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp610=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FC;_tmp7FC.tag=0U,({struct _fat_ptr _tmpA78=({const char*_tmp611="fallthru in unexpected place";_tag_fat(_tmp611,sizeof(char),29U);});_tmp7FC.f1=_tmpA78;});_tmp7FC;});void*_tmp60F[1U];_tmp60F[0]=& _tmp610;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp60F,sizeof(void*),1U));});{
struct Cyc_Toc_FallthruInfo _tmp612=*fi;struct Cyc_Toc_FallthruInfo _stmttmp65=_tmp612;struct Cyc_Toc_FallthruInfo _tmp613=_stmttmp65;struct Cyc_List_List*_tmp615;struct _fat_ptr*_tmp614;_LL2D: _tmp614=_tmp613.label;_tmp615=_tmp613.binders;_LL2E: {struct _fat_ptr*l=_tmp614;struct Cyc_List_List*vs=_tmp615;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3555
({int _tmpA79=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA79,s2);});{
struct Cyc_List_List*_tmp616=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp616;
struct Cyc_List_List*_tmp617=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp617;
for(0;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmpA7B=({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->tag=5U,_tmp618->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp618;}),0U);Cyc_Absyn_assign_stmt(_tmpA7A,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA7B,s2,0U);});}
# 3563
s->r=s2->r;
return;}}}}}case 12U: _LL1F: _tmp5EE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5EF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp5EE;struct Cyc_Absyn_Stmt*s1=_tmp5EF;
# 3569
{void*_tmp619=d->r;void*_stmttmp66=_tmp619;void*_tmp61A=_stmttmp66;struct Cyc_Absyn_Exp*_tmp61D;struct Cyc_Absyn_Vardecl*_tmp61C;struct Cyc_Absyn_Tvar*_tmp61B;struct Cyc_Absyn_Fndecl*_tmp61E;struct Cyc_List_List*_tmp61F;void*_tmp622;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Pat*_tmp620;struct Cyc_Absyn_Vardecl*_tmp623;switch(*((int*)_tmp61A)){case 0U: _LL30: _tmp623=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp61A)->f1;_LL31: {struct Cyc_Absyn_Vardecl*vd=_tmp623;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2U: _LL32: _tmp620=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp61A)->f1;_tmp621=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp61A)->f3;_tmp622=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp61A)->f4;_LL33: {struct Cyc_Absyn_Pat*p=_tmp620;struct Cyc_Absyn_Exp*e=_tmp621;void*dec_tree=_tmp622;
# 3577
{void*_tmp624=p->r;void*_stmttmp67=_tmp624;void*_tmp625=_stmttmp67;struct Cyc_Absyn_Vardecl*_tmp626;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp625)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp625)->f2)->r)->tag == 0U){_LL3D: _tmp626=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp625)->f1;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp626;
# 3579
struct Cyc_Absyn_Vardecl*_tmp627=({struct _tuple1*_tmpA7D=Cyc_Toc_temp_var();void*_tmpA7C=vd->type;Cyc_Absyn_new_vardecl(0U,_tmpA7D,_tmpA7C,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp627;
({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=4U,_tmp628->f1=new_vd;_tmp628;}),0U);vd->initializer=_tmpA7E;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmpA85=({struct Cyc_Absyn_Decl*_tmpA84=({void*_tmpA7F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->tag=0U,_tmp629->f1=new_vd;_tmp629;});Cyc_Absyn_new_decl(_tmpA7F,s->loc);});struct Cyc_Absyn_Stmt*_tmpA83=({
struct Cyc_Absyn_Decl*_tmpA82=({void*_tmpA80=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tag=0U,_tmp62A->f1=vd;_tmp62A;});Cyc_Absyn_new_decl(_tmpA80,s->loc);});struct Cyc_Absyn_Stmt*_tmpA81=s1;Cyc_Absyn_decl_stmt(_tmpA82,_tmpA81,s->loc);});
# 3582
Cyc_Absyn_decl_stmt(_tmpA84,_tmpA83,s->loc);})->r;s->r=_tmpA85;});
# 3585
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3591
({void*_tmpA86=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmpA86;});
goto _LL3C;}_LL3C:;}
# 3594
goto _LL2F;}case 3U: _LL34: _tmp61F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp61A)->f1;_LL35: {struct Cyc_List_List*vds=_tmp61F;
# 3598
struct Cyc_List_List*_tmp62B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp62B;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FD;_tmp7FD.tag=0U,({struct _fat_ptr _tmpA87=({const char*_tmp62E="empty Letv_d";_tag_fat(_tmp62E,sizeof(char),13U);});_tmp7FD.f1=_tmpA87;});_tmp7FD;});void*_tmp62C[1U];_tmp62C[0]=& _tmp62D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp62C,sizeof(void*),1U));});
({void*_tmpA88=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=0U,_tmp62F->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp62F;});d->r=_tmpA88;});
rvds=rvds->tl;
for(0;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp630=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=0U,_tmp631->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp631;}),0U);struct Cyc_Absyn_Decl*d2=_tmp630;
({void*_tmpA8A=({struct Cyc_Absyn_Decl*_tmpA89=d2;Cyc_Absyn_decl_stmt(_tmpA89,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA8A;});}
# 3607
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp61E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp61A)->f1;_LL37: {struct Cyc_Absyn_Fndecl*fd=_tmp61E;
# 3610
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4U: _LL38: _tmp61B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61A)->f1;_tmp61C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61A)->f2;_tmp61D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp61A)->f3;_LL39: {struct Cyc_Absyn_Tvar*tv=_tmp61B;struct Cyc_Absyn_Vardecl*vd=_tmp61C;struct Cyc_Absyn_Exp*open_exp_opt=_tmp61D;
# 3614
struct Cyc_Absyn_Stmt*_tmp632=s1;struct Cyc_Absyn_Stmt*body=_tmp632;
# 3616
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp633=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp68=_tmp633;struct _tuple30 _tmp634=_stmttmp68;struct Cyc_Absyn_Exp*_tmp636;struct _tuple1*_tmp635;_LL42: _tmp635=_tmp634.f1;_tmp636=_tmp634.f2;_LL43: {struct _tuple1*rh_var=_tmp635;struct Cyc_Absyn_Exp*rh_exp=_tmp636;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3622
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Absyn_no_regions)
({void*_tmpA8E=({struct _tuple1*_tmpA8D=x_var;void*_tmpA8C=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA8B=
Cyc_Absyn_uint_exp(0U,0U);
# 3624
Cyc_Absyn_declare_stmt(_tmpA8D,_tmpA8C,_tmpA8B,body,0U);})->r;s->r=_tmpA8E;});else{
# 3626
if((unsigned)open_exp_opt){
# 3628
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA90=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA90,({struct _fat_ptr*_tmp638=_cycalloc(sizeof(*_tmp638));({struct _fat_ptr _tmpA8F=({const char*_tmp637="h";_tag_fat(_tmp637,sizeof(char),2U);});*_tmp638=_tmpA8F;});_tmp638;}),0U);}),0U);
({void*_tmpA91=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA91;});}}else{
# 3637
({void*_tmpA9E=({struct _tuple1*_tmpA9D=rh_var;void*_tmpA9C=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA9B=({struct Cyc_Absyn_Exp*_tmp639[1U];({struct Cyc_Absyn_Exp*_tmpA92=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp639[0]=_tmpA92;});({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA93,_tag_fat(_tmp639,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3637
Cyc_Absyn_declare_stmt(_tmpA9D,_tmpA9C,_tmpA9B,({
# 3639
struct _tuple1*_tmpA9A=x_var;void*_tmpA99=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA98=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA9A,_tmpA99,_tmpA98,({
struct Cyc_Absyn_Stmt*_tmpA97=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp63A[1U];_tmp63A[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA94=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA94,_tag_fat(_tmp63A,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA97,({
struct Cyc_Absyn_Stmt*_tmpA96=body;Cyc_Absyn_seq_stmt(_tmpA96,
Cyc_Absyn_exp_stmt(({void*_tmp63B=0U;({struct Cyc_Absyn_Exp*_tmpA95=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA95,_tag_fat(_tmp63B,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3637
s->r=_tmpA9E;});}}
# 3645
return;}}default: _LL3A: _LL3B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FE;_tmp7FE.tag=0U,({struct _fat_ptr _tmpA9F=({const char*_tmp63E="bad nested declaration within function";_tag_fat(_tmp63E,sizeof(char),39U);});_tmp7FE.f1=_tmpA9F;});_tmp7FE;});void*_tmp63C[1U];_tmp63C[0]=& _tmp63D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp63C,sizeof(void*),1U));});}_LL2F:;}
# 3648
return;}default: _LL21: _tmp5EB=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f1;_tmp5EC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f2;_tmp5ED=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EA)->f3;_LL22: {struct Cyc_Absyn_Stmt*body=_tmp5EB;struct Cyc_List_List*scs=_tmp5EC;void*dec_tree=_tmp5ED;
# 3662 "toc.cyc"
struct _tuple30 _tmp63F=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp69=_tmp63F;struct _tuple30 _tmp640=_stmttmp69;struct Cyc_Absyn_Exp*_tmp642;struct _tuple1*_tmp641;_LL45: _tmp641=_tmp640.f1;_tmp642=_tmp640.f2;_LL46: {struct _tuple1*h_var=_tmp641;struct Cyc_Absyn_Exp*h_exp=_tmp642;
struct _tuple30 _tmp643=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6A=_tmp643;struct _tuple30 _tmp644=_stmttmp6A;struct Cyc_Absyn_Exp*_tmp646;struct _tuple1*_tmp645;_LL48: _tmp645=_tmp644.f1;_tmp646=_tmp644.f2;_LL49: {struct _tuple1*e_var=_tmp645;struct Cyc_Absyn_Exp*e_exp=_tmp646;
struct _tuple30 _tmp647=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6B=_tmp647;struct _tuple30 _tmp648=_stmttmp6B;struct Cyc_Absyn_Exp*_tmp64A;struct _tuple1*_tmp649;_LL4B: _tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;_LL4C: {struct _tuple1*was_thrown_var=_tmp649;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp64A;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3669
e_exp->topt=e_typ;{
struct _RegionHandle _tmp64B=_new_region("temp");struct _RegionHandle*temp=& _tmp64B;_push_region(temp);
# 3672
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp64C=({struct Cyc_Absyn_Stmt*_tmpAA1=body;Cyc_Absyn_seq_stmt(_tmpAA1,
Cyc_Absyn_exp_stmt(({void*_tmp655=0U;({struct Cyc_Absyn_Exp*_tmpAA0=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpAA0,_tag_fat(_tmp655,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3673
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp64C;
# 3676
struct Cyc_Absyn_Stmt*_tmp64D=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->tag=10U,_tmp654->f1=e_exp,_tmp654->f2=scs,_tmp654->f3=dec_tree;_tmp654;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp64D;
# 3678
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3681
struct Cyc_Absyn_Exp*_tmp64E=({struct Cyc_Absyn_Exp*_tmp653[1U];({struct Cyc_Absyn_Exp*_tmpAA2=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp653[0]=_tmpAA2;});({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpAA3,_tag_fat(_tmp653,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3681
struct Cyc_Absyn_Exp*setjmp_call=_tmp64E;
# 3683
struct Cyc_Absyn_Stmt*_tmp64F=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp652[1U];({struct Cyc_Absyn_Exp*_tmpAA4=Cyc_Absyn_address_exp(h_exp,0U);_tmp652[0]=_tmpAA4;});({struct Cyc_Absyn_Exp*_tmpAA5=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpAA5,_tag_fat(_tmp652,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
# 3683
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp64F;
# 3685
struct Cyc_Absyn_Exp*_tmp650=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp650;
struct Cyc_Absyn_Exp*_tmp651=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp651;
({void*_tmpAB5=({
struct _tuple1*_tmpAB4=h_var;void*_tmpAB3=h_typ;Cyc_Absyn_declare_stmt(_tmpAB4,_tmpAB3,0,({
struct Cyc_Absyn_Stmt*_tmpAB2=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpAB2,({
struct _tuple1*_tmpAB1=was_thrown_var;void*_tmpAB0=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpAAF=zero_exp;Cyc_Absyn_declare_stmt(_tmpAB1,_tmpAB0,_tmpAAF,({
struct Cyc_Absyn_Stmt*_tmpAAE=({struct Cyc_Absyn_Exp*_tmpAA7=setjmp_call;struct Cyc_Absyn_Stmt*_tmpAA6=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3691
Cyc_Absyn_ifthenelse_stmt(_tmpAA7,_tmpAA6,
# 3693
Cyc_Toc_skip_stmt_dl(),0U);});
# 3691
Cyc_Absyn_seq_stmt(_tmpAAE,({
# 3694
struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpAAC=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpAAD,_tmpAAC,({
# 3696
struct _tuple1*_tmpAAB=e_var;void*_tmpAAA=e_typ;struct Cyc_Absyn_Exp*_tmpAA9=({
void*_tmpAA8=e_typ;Cyc_Toc_cast_it(_tmpAA8,Cyc_Toc_get_exn_thrown_expression());});
# 3696
Cyc_Absyn_declare_stmt(_tmpAAB,_tmpAAA,_tmpAA9,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3687
s->r=_tmpAB5;});}}
# 3700
_npop_handler(0U);return;
# 3670
;_pop_region();}}}}}}_LL0:;}}
# 3709
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpAB6=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpAB6;});
({void*_tmpAB7=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpAB7;});{
struct _RegionHandle _tmp656=_new_region("frgn");struct _RegionHandle*frgn=& _tmp656;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp657=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp657;
{struct Cyc_List_List*_tmp658=(f->i).args;struct Cyc_List_List*args=_tmp658;for(0;args != 0;args=args->tl){
struct _tuple1*_tmp659=({struct _tuple1*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->f1=Cyc_Absyn_Loc_n,_tmp65A->f2=(*((struct _tuple9*)args->hd)).f1;_tmp65A;});struct _tuple1*x=_tmp659;
({void*_tmpAB8=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAB8;});}}
# 3726
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3730
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp65B=_cycalloc(sizeof(*_tmp65B));({struct Cyc_Hashtable_Table*_tmpAB9=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp65B=_tmpAB9;});_tmp65B;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp65C=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6C=_tmp65C;struct Cyc_Absyn_VarargInfo _tmp65D=_stmttmp6C;int _tmp661;void*_tmp660;struct Cyc_Absyn_Tqual _tmp65F;struct _fat_ptr*_tmp65E;_LL1: _tmp65E=_tmp65D.name;_tmp65F=_tmp65D.tq;_tmp660=_tmp65D.type;_tmp661=_tmp65D.inject;_LL2: {struct _fat_ptr*n=_tmp65E;struct Cyc_Absyn_Tqual tq=_tmp65F;void*t=_tmp660;int i=_tmp661;
void*_tmp662=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type));void*t2=_tmp662;
struct _tuple1*_tmp663=({struct _tuple1*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->f1=Cyc_Absyn_Loc_n,_tmp666->f2=(struct _fat_ptr*)_check_null(n);_tmp666;});struct _tuple1*x2=_tmp663;
({struct Cyc_List_List*_tmpABC=({struct Cyc_List_List*_tmpABB=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpABB,({struct Cyc_List_List*_tmp665=_cycalloc(sizeof(*_tmp665));({struct _tuple9*_tmpABA=({struct _tuple9*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->f1=n,_tmp664->f2=tq,_tmp664->f3=t2;_tmp664;});_tmp665->hd=_tmpABA;}),_tmp665->tl=0;_tmp665;}));});(f->i).args=_tmpABC;});
(f->i).cyc_varargs=0;}}
# 3739
{struct Cyc_List_List*_tmp667=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp667;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpABD=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpABD;});}}
({struct Cyc_Toc_Env*_tmpABE=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpABE,f->body);});}
# 3719
;_pop_region();}}
# 3744
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp668=s;switch(_tmp668){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3760 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3762
struct _tuple42 _tmp669=*env;struct _tuple42 _stmttmp6D=_tmp669;struct _tuple42 _tmp66A=_stmttmp6D;struct Cyc_Absyn_Aggrdecl*_tmp66C;struct Cyc_Toc_TocState*_tmp66B;_LL1: _tmp66B=_tmp66A.f1;_tmp66C=*_tmp66A.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp66B;struct Cyc_Absyn_Aggrdecl*ad=_tmp66C;
struct _tuple1*_tmp66D=ad->name;struct _tuple1*n=_tmp66D;
struct Cyc_Toc_TocState _tmp66E=*s;struct Cyc_Toc_TocState _stmttmp6E=_tmp66E;struct Cyc_Toc_TocState _tmp66F=_stmttmp6E;struct Cyc_Dict_Dict*_tmp670;_LL4: _tmp670=_tmp66F.aggrs_so_far;_LL5: {struct Cyc_Dict_Dict*aggrs_so_far=_tmp670;
int seen_defn_before;
struct _tuple18**_tmp671=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple18**dopt=_tmp671;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v;
if((int)ad->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp672=_region_malloc(d,sizeof(*_tmp672));_tmp672->f1=ad,({void*_tmpABF=Cyc_Absyn_strctq(n);_tmp672->f2=_tmpABF;});_tmp672;});else{
# 3773
v=({struct _tuple18*_tmp673=_region_malloc(d,sizeof(*_tmp673));_tmp673->f1=ad,({void*_tmpAC0=Cyc_Absyn_unionq_type(n);_tmp673->f2=_tmpAC0;});_tmp673;});}
({struct Cyc_Dict_Dict _tmpAC1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpAC1;});}}else{
# 3776
struct _tuple18*_tmp674=*dopt;struct _tuple18*_stmttmp6F=_tmp674;struct _tuple18*_tmp675=_stmttmp6F;void*_tmp677;struct Cyc_Absyn_Aggrdecl*_tmp676;_LL7: _tmp676=_tmp675->f1;_tmp677=_tmp675->f2;_LL8: {struct Cyc_Absyn_Aggrdecl*ad2=_tmp676;void*t=_tmp677;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpAC4=({struct Cyc_Dict_Dict _tmpAC3=*aggrs_so_far;struct _tuple1*_tmpAC2=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpAC3,_tmpAC2,({struct _tuple18*_tmp678=_region_malloc(d,sizeof(*_tmp678));_tmp678->f1=ad,_tmp678->f2=t;_tmp678;}));});*aggrs_so_far=_tmpAC4;});
seen_defn_before=0;}else{
# 3781
seen_defn_before=1;}}}{
# 3783
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->kind=ad->kind,_tmp69D->sc=Cyc_Absyn_Public,_tmp69D->name=ad->name,_tmp69D->tvs=0,_tmp69D->impl=0,_tmp69D->expected_mem_kind=0,_tmp69D->attributes=ad->attributes;_tmp69D;});
# 3790
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpAC5=({struct Cyc_Absyn_AggrdeclImpl*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->exist_vars=0,_tmp679->rgn_po=0,_tmp679->fields=0,_tmp679->tagged=0;_tmp679;});new_ad->impl=_tmpAC5;});{
# 3795
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp67A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp67A;for(0;fields != 0;fields=fields->tl){
# 3799
struct Cyc_Absyn_Aggrfield*_tmp67B=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp67B;
void*_tmp67C=old_field->type;void*old_type=_tmp67C;
struct Cyc_List_List*_tmp67D=old_field->attributes;struct Cyc_List_List*old_atts=_tmp67D;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3813 "toc.cyc"
void*_tmp67E=Cyc_Tcutil_compress(old_type);void*_stmttmp70=_tmp67E;void*_tmp67F=_stmttmp70;unsigned _tmp683;void*_tmp682;struct Cyc_Absyn_Tqual _tmp681;void*_tmp680;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67F)->tag == 4U){_LLA: _tmp680=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67F)->f1).elt_type;_tmp681=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67F)->f1).tq;_tmp682=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67F)->f1).zero_term;_tmp683=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67F)->f1).zt_loc;_LLB: {void*et=_tmp680;struct Cyc_Absyn_Tqual tq=_tmp681;void*zt=_tmp682;unsigned ztl=_tmp683;
# 3816
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->tag=4U,(_tmp684->f1).elt_type=et,(_tmp684->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Absyn_uint_exp(0U,0U);(_tmp684->f1).num_elts=_tmpAC6;}),(_tmp684->f1).zero_term=zt,(_tmp684->f1).zt_loc=ztl;_tmp684;});
goto _LL9;}}else{_LLC: _LLD:
# 3819
 old_atts=({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));({void*_tmpAC7=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->tag=6U,_tmp685->f1=0;_tmp685;});_tmp686->hd=_tmpAC7;}),_tmp686->tl=old_atts;_tmp686;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->tag=4U,({void*_tmpACA=Cyc_Toc_void_star_type();(_tmp687->f1).elt_type=_tmpACA;}),({
struct Cyc_Absyn_Tqual _tmpAC9=Cyc_Absyn_empty_tqual(0U);(_tmp687->f1).tq=_tmpAC9;}),({
struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Absyn_uint_exp(0U,0U);(_tmp687->f1).num_elts=_tmpAC8;}),(_tmp687->f1).zero_term=Cyc_Absyn_false_type,(_tmp687->f1).zt_loc=0U;_tmp687;});}_LL9:;}{
# 3826
struct Cyc_Absyn_Aggrfield*_tmp688=({struct Cyc_Absyn_Aggrfield*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->name=old_field->name,_tmp699->tq=Cyc_Toc_mt_tq,({
# 3828
void*_tmpACB=Cyc_Toc_typ_to_c(old_type);_tmp699->type=_tmpACB;}),_tmp699->width=old_field->width,_tmp699->attributes=old_atts,_tmp699->requires_clause=0;_tmp699;});
# 3826
struct Cyc_Absyn_Aggrfield*new_field=_tmp688;
# 3836
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp689=new_field->type;void*T=_tmp689;
struct _fat_ptr*_tmp68A=new_field->name;struct _fat_ptr*f=_tmp68A;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp696=({struct Cyc_String_pa_PrintArg_struct _tmp800;_tmp800.tag=0U,_tmp800.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp800;});struct Cyc_String_pa_PrintArg_struct _tmp697=({struct Cyc_String_pa_PrintArg_struct _tmp7FF;_tmp7FF.tag=0U,_tmp7FF.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7FF;});void*_tmp694[2U];_tmp694[0]=& _tmp696,_tmp694[1]=& _tmp697;({struct _fat_ptr _tmpACC=({const char*_tmp695="_union_%s_%s";_tag_fat(_tmp695,sizeof(char),13U);});Cyc_aprintf(_tmpACC,_tag_fat(_tmp694,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp693=_cycalloc(sizeof(*_tmp693));*_tmp693=s;_tmp693;});
struct Cyc_Absyn_Aggrfield*_tmp68B=({struct Cyc_Absyn_Aggrfield*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->name=Cyc_Toc_val_sp,_tmp692->tq=Cyc_Toc_mt_tq,_tmp692->type=T,_tmp692->width=0,_tmp692->attributes=0,_tmp692->requires_clause=0;_tmp692;});struct Cyc_Absyn_Aggrfield*value_field=_tmp68B;
struct Cyc_Absyn_Aggrfield*_tmp68C=({struct Cyc_Absyn_Aggrfield*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->name=Cyc_Toc_tag_sp,_tmp691->tq=Cyc_Toc_mt_tq,_tmp691->type=Cyc_Absyn_sint_type,_tmp691->width=0,_tmp691->attributes=0,_tmp691->requires_clause=0;_tmp691;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp68C;
struct Cyc_Absyn_Aggrdecl*_tmp68D=({struct _fat_ptr*_tmpACD=str;Cyc_Toc_make_c_struct_defn(_tmpACD,({struct Cyc_Absyn_Aggrfield*_tmp690[2U];_tmp690[0]=tag_field,_tmp690[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp690,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp68D;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));({struct Cyc_Absyn_Decl*_tmpACE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->tag=5U,_tmp68E->f1=ad2;_tmp68E;}),0U);_tmp68F->hd=_tmpACE;}),_tmp68F->tl=Cyc_Toc_result_decls;_tmp68F;});
({void*_tmpACF=Cyc_Absyn_strct(str);new_field->type=_tmpACF;});}
# 3847
new_fields=({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->hd=new_field,_tmp698->tl=new_fields;_tmp698;});}}}
# 3849
({struct Cyc_List_List*_tmpAD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpAD0;});}}
# 3852
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp69C=_cycalloc(sizeof(*_tmp69C));({struct Cyc_Absyn_Decl*_tmpAD2=({struct Cyc_Absyn_Decl*_tmp69B=_cycalloc(sizeof(*_tmp69B));({void*_tmpAD1=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->tag=5U,_tmp69A->f1=new_ad;_tmp69A;});_tmp69B->r=_tmpAD1;}),_tmp69B->loc=0U;_tmp69B;});_tmp69C->hd=_tmpAD2;}),_tmp69C->tl=Cyc_Toc_result_decls;_tmp69C;});
return 0;}}}}
# 3856
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3884 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3887
struct _tuple43 _tmp69E=*env;struct _tuple43 _stmttmp71=_tmp69E;struct _tuple43 _tmp69F=_stmttmp71;struct Cyc_Absyn_Datatypedecl*_tmp6A1;struct Cyc_Set_Set**_tmp6A0;_LL1: _tmp6A0=(_tmp69F.f1)->datatypes_so_far;_tmp6A1=_tmp69F.f2;_LL2: {struct Cyc_Set_Set**datatypes_so_far=_tmp6A0;struct Cyc_Absyn_Datatypedecl*tud=_tmp6A1;
struct _tuple1*_tmp6A2=tud->name;struct _tuple1*n=_tmp6A2;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpAD3=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpAD3;});
{struct Cyc_List_List*_tmp6A3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp6A3;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3895
struct Cyc_List_List*_tmp6A4=0;struct Cyc_List_List*fs=_tmp6A4;
int i=1;
{struct Cyc_List_List*_tmp6A5=f->typs;struct Cyc_List_List*ts=_tmp6A5;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp6A6=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp6A6;
struct Cyc_Absyn_Aggrfield*_tmp6A7=({struct Cyc_Absyn_Aggrfield*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->name=fname,_tmp6A9->tq=(*((struct _tuple23*)ts->hd)).f1,({
void*_tmpAD4=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f2);_tmp6A9->type=_tmpAD4;}),_tmp6A9->width=0,_tmp6A9->attributes=0,_tmp6A9->requires_clause=0;_tmp6A9;});
# 3899
struct Cyc_Absyn_Aggrfield*f=_tmp6A7;
# 3901
fs=({struct Cyc_List_List*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->hd=f,_tmp6A8->tl=fs;_tmp6A8;});}}
# 3903
fs=({struct Cyc_List_List*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));({struct Cyc_Absyn_Aggrfield*_tmpAD6=({struct Cyc_Absyn_Aggrfield*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->name=Cyc_Toc_tag_sp,_tmp6AA->tq=Cyc_Toc_mt_tq,_tmp6AA->type=Cyc_Absyn_sint_type,_tmp6AA->width=0,_tmp6AA->attributes=0,_tmp6AA->requires_clause=0;_tmp6AA;});_tmp6AB->hd=_tmpAD6;}),({
struct Cyc_List_List*_tmpAD5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);_tmp6AB->tl=_tmpAD5;});_tmp6AB;});{
struct Cyc_Absyn_Aggrdecl*_tmp6AC=({struct _fat_ptr*_tmpAD8=({struct _fat_ptr*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));({struct _fat_ptr _tmpAD7=({const char*_tmp6AF="";_tag_fat(_tmp6AF,sizeof(char),1U);});*_tmp6B0=_tmpAD7;});_tmp6B0;});Cyc_Toc_make_c_struct_defn(_tmpAD8,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp6AC;
({struct _tuple1*_tmpAD9=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpAD9;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct Cyc_Absyn_Decl*_tmpADA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tag=5U,_tmp6AD->f1=ad;_tmp6AD;}),0U);_tmp6AE->hd=_tmpADA;}),_tmp6AE->tl=Cyc_Toc_result_decls;_tmp6AE;});}}}
# 3909
return 0;}}
# 3912
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3931 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3934
struct _tuple43 _tmp6B1=*env;struct _tuple43 _stmttmp72=_tmp6B1;struct _tuple43 _tmp6B2=_stmttmp72;struct Cyc_Absyn_Datatypedecl*_tmp6B4;struct Cyc_Toc_TocState*_tmp6B3;_LL1: _tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp6B3;struct Cyc_Absyn_Datatypedecl*xd=_tmp6B4;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6B5=*s;struct Cyc_Toc_TocState _stmttmp73=_tmp6B5;struct Cyc_Toc_TocState _tmp6B6=_stmttmp73;struct Cyc_Dict_Dict*_tmp6B7;_LL4: _tmp6B7=_tmp6B6.xdatatypes_so_far;_LL5: {struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp6B7;
struct _tuple1*_tmp6B8=xd->name;struct _tuple1*n=_tmp6B8;
{struct Cyc_List_List*_tmp6B9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp6B9;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6BA=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp6BA;
void*_tmp6BB=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp6BB;
# 3945
int*_tmp6BC=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp74=_tmp6BC;int*_tmp6BD=_stmttmp74;if(_tmp6BD == 0){_LL7: _LL8: {
# 3947
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3951
struct Cyc_Absyn_Vardecl*_tmp6BE=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6BE;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));({struct Cyc_Absyn_Decl*_tmpADB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->tag=0U,_tmp6BF->f1=tag_decl;_tmp6BF;}),0U);_tmp6C0->hd=_tmpADB;}),_tmp6C0->tl=Cyc_Toc_result_decls;_tmp6C0;});
({struct Cyc_Dict_Dict _tmpADC=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3954
*xdatatypes_so_far=_tmpADC;});{
# 3956
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6C1=f->typs;struct Cyc_List_List*tqts=_tmp6C1;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp6C2=({struct _fat_ptr*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));({struct _fat_ptr _tmpADE=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C8=({struct Cyc_Int_pa_PrintArg_struct _tmp801;_tmp801.tag=1U,_tmp801.f1=(unsigned long)i;_tmp801;});void*_tmp6C6[1U];_tmp6C6[0]=& _tmp6C8;({struct _fat_ptr _tmpADD=({const char*_tmp6C7="f%d";_tag_fat(_tmp6C7,sizeof(char),4U);});Cyc_aprintf(_tmpADD,_tag_fat(_tmp6C6,sizeof(void*),1U));});});*_tmp6C9=_tmpADE;});_tmp6C9;});struct _fat_ptr*field_n=_tmp6C2;
struct Cyc_Absyn_Aggrfield*_tmp6C3=({struct Cyc_Absyn_Aggrfield*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5->name=field_n,_tmp6C5->tq=(*((struct _tuple23*)tqts->hd)).f1,({
void*_tmpADF=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f2);_tmp6C5->type=_tmpADF;}),_tmp6C5->width=0,_tmp6C5->attributes=0,_tmp6C5->requires_clause=0;_tmp6C5;});
# 3960
struct Cyc_Absyn_Aggrfield*newf=_tmp6C3;
# 3962
fields=({struct Cyc_List_List*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->hd=newf,_tmp6C4->tl=fields;_tmp6C4;});}}
# 3964
fields=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));({struct Cyc_Absyn_Aggrfield*_tmpAE2=({struct Cyc_Absyn_Aggrfield*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->name=Cyc_Toc_tag_sp,_tmp6CA->tq=Cyc_Toc_mt_tq,({
void*_tmpAE1=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6CA->type=_tmpAE1;}),_tmp6CA->width=0,_tmp6CA->attributes=0,_tmp6CA->requires_clause=0;_tmp6CA;});
# 3964
_tmp6CB->hd=_tmpAE2;}),({
# 3966
struct Cyc_List_List*_tmpAE0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6CB->tl=_tmpAE0;});_tmp6CB;});{
struct Cyc_Absyn_Aggrdecl*_tmp6CC=({struct _fat_ptr*_tmpAE4=({struct _fat_ptr*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));({struct _fat_ptr _tmpAE3=({const char*_tmp6CF="";_tag_fat(_tmp6CF,sizeof(char),1U);});*_tmp6D0=_tmpAE3;});_tmp6D0;});Cyc_Toc_make_c_struct_defn(_tmpAE4,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp6CC;
({struct _tuple1*_tmpAE5=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpAE5;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));({struct Cyc_Absyn_Decl*_tmpAE6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->tag=5U,_tmp6CD->f1=strct_decl;_tmp6CD;}),0U);_tmp6CE->hd=_tmpAE6;}),_tmp6CE->tl=Cyc_Toc_result_decls;_tmp6CE;});
goto _LL6;}}}}}else{if(*((int*)_tmp6BD)== 0){_LL9: _LLA:
# 3972
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6D1=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp6D1;
struct Cyc_Absyn_Vardecl*_tmp6D2=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6D2;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));({struct Cyc_Absyn_Decl*_tmpAE7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->tag=0U,_tmp6D3->f1=tag_decl;_tmp6D3;}),0U);_tmp6D4->hd=_tmpAE7;}),_tmp6D4->tl=Cyc_Toc_result_decls;_tmp6D4;});
({struct Cyc_Dict_Dict _tmpAE8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpAE8;});}
# 3979
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3983
return 0;}}}}
# 3986
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3990
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3996
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpAE9=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpAE9;});
# 4000
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 4009
struct _RegionHandle _tmp6D5=_new_region("temp");struct _RegionHandle*temp=& _tmp6D5;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D6=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp6D6;
Cyc_Toc_exp_to_c(nv2,init);}
# 4010
;_pop_region();}else{
# 4014
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4017
void*_tmp6D7=Cyc_Tcutil_compress(old_typ);void*_stmttmp75=_tmp6D7;void*_tmp6D8=_stmttmp75;void*_tmp6DB;struct Cyc_Absyn_Exp*_tmp6DA;void*_tmp6D9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->tag == 4U){_LL1: _tmp6D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).elt_type;_tmp6DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).num_elts;_tmp6DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D8)->f1).zero_term;_LL2: {void*et=_tmp6D9;struct Cyc_Absyn_Exp*num_elts_opt=_tmp6DA;void*zt=_tmp6DB;
# 4019
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp802;_tmp802.tag=0U,({struct _fat_ptr _tmpAEA=({const char*_tmp6DE="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp6DE,sizeof(char),55U);});_tmp802.f1=_tmpAEA;});_tmp802;});void*_tmp6DC[1U];_tmp6DC[0]=& _tmp6DD;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6DC,sizeof(void*),1U));});{
struct Cyc_Absyn_Exp*num_elts=num_elts_opt;
struct Cyc_Absyn_Exp*_tmp6DF=({struct Cyc_Absyn_Exp*_tmpAEC=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpAEC,({
struct Cyc_Absyn_Exp*_tmpAEB=num_elts;Cyc_Absyn_add_exp(_tmpAEB,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4023
struct Cyc_Absyn_Exp*lhs=_tmp6DF;
# 4026
struct Cyc_Absyn_Exp*_tmp6E0=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp6E0;
({void*_tmpAEE=({struct Cyc_Absyn_Stmt*_tmpAED=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAED,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4027
s->r=_tmpAEE;});}}
# 4030
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4038
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6E1=d;void**_tmp6E4;struct Cyc_List_List*_tmp6E3;struct Cyc_List_List*_tmp6E2;struct Cyc_Tcpat_Rhs*_tmp6E5;switch(*((int*)_tmp6E1)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6E5=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6E1)->f1;_LL4: {struct Cyc_Tcpat_Rhs*rhs=_tmp6E5;
rhs->rhs=success;return d;}default: _LL5: _tmp6E2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E1)->f1;_tmp6E3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E1)->f2;_tmp6E4=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6E1)->f3;_LL6: {struct Cyc_List_List*path=_tmp6E2;struct Cyc_List_List*sws=_tmp6E3;void**d2=_tmp6E4;
# 4043
({void*_tmpAEF=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAEF;});
for(0;sws != 0;sws=sws->tl){
struct _tuple34*_tmp6E6=(struct _tuple34*)sws->hd;struct _tuple34*_stmttmp76=_tmp6E6;struct _tuple34*_tmp6E7=_stmttmp76;void**_tmp6E8;_LL8: _tmp6E8=(void**)& _tmp6E7->f2;_LL9: {void**d2=_tmp6E8;
({void*_tmpAF0=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAF0;});}}
# 4048
return d;}}_LL0:;}
# 4059 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4061
struct _RegionHandle _tmp6E9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E9;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6EA=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp6EA;
void*_tmp6EB=(void*)_check_null(e->topt);void*t=_tmp6EB;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4071
struct Cyc_Absyn_Stmt*_tmp6EC=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6EC;
if(dopt != 0)
# 4075
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4077
struct Cyc_Absyn_Switch_clause*_tmp6ED=({struct Cyc_Absyn_Switch_clause*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->pattern=p,_tmp6FC->pat_vars=0,_tmp6FC->where_clause=0,_tmp6FC->body=succ_stmt,_tmp6FC->loc=0U;_tmp6FC;});struct Cyc_Absyn_Switch_clause*c1=_tmp6ED;
struct Cyc_List_List*_tmp6EE=({struct _RegionHandle*_tmpAF3=rgn;struct _RegionHandle*_tmpAF2=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpAF3,Cyc_Toc_gen_labels,_tmpAF2,({struct Cyc_Absyn_Switch_clause*_tmp6FB[1U];_tmp6FB[0]=c1;({struct _RegionHandle*_tmpAF1=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpAF1,_tag_fat(_tmp6FB,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});struct Cyc_List_List*lscs=_tmp6EE;
# 4080
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4084
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4087
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple41*_tmp6EF=(struct _tuple41*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple41*_stmttmp77=_tmp6EF;struct _tuple41*_tmp6F0=_stmttmp77;struct Cyc_Absyn_Stmt*_tmp6F2;struct Cyc_Toc_Env*_tmp6F1;_LL1: _tmp6F1=_tmp6F0->f1;_tmp6F2=_tmp6F0->f3;_LL2: {struct Cyc_Toc_Env*env=_tmp6F1;struct Cyc_Absyn_Stmt*st=_tmp6F2;
if(st == succ_stmt){senv=env;goto FOUND_ENV;}}}
# 4092
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp803;_tmp803.tag=0U,({struct _fat_ptr _tmpAF4=({const char*_tmp6F5="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6F5,sizeof(char),33U);});_tmp803.f1=_tmpAF4;});_tmp803;});void*_tmp6F3[1U];_tmp6F3[0]=& _tmp6F4;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6F3,sizeof(void*),1U));});
FOUND_ENV:
# 4096
 Cyc_Toc_stmt_to_c(senv,s);{
# 4098
struct Cyc_Absyn_Stmt*res=test_tree;
# 4100
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp6F6=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp78=_tmp6F6;struct _tuple39 _tmp6F7=_stmttmp78;struct Cyc_Absyn_Vardecl*_tmp6F8;_LL4: _tmp6F8=_tmp6F7.f2;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp6F8;
res=({struct Cyc_Absyn_Decl*_tmpAF5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->tag=0U,_tmp6F9->f1=vd;_tmp6F9;}),0U);Cyc_Absyn_decl_stmt(_tmpAF5,res,0U);});}}
# 4105
res=({struct _tuple1*_tmpAF8=v;void*_tmpAF7=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpAF6=e;Cyc_Absyn_declare_stmt(_tmpAF8,_tmpAF7,_tmpAF6,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6FA=res;_npop_handler(0U);return _tmp6FA;}}}}}
# 4062
;_pop_region();}
# 4114
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6FD=e->r;void*_stmttmp79=_tmp6FD;void*_tmp6FE=_stmttmp79;struct Cyc_List_List*_tmp700;struct Cyc_List_List*_tmp6FF;struct Cyc_Absyn_MallocInfo*_tmp701;struct Cyc_Absyn_Stmt*_tmp702;void**_tmp703;void**_tmp704;struct Cyc_List_List*_tmp705;struct Cyc_List_List*_tmp706;struct Cyc_List_List*_tmp708;void**_tmp707;struct Cyc_Absyn_Exp*_tmp70A;void**_tmp709;struct Cyc_List_List*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp719;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_Absyn_Exp*_tmp71A;struct Cyc_List_List*_tmp71C;struct Cyc_Absyn_Exp*_tmp71D;struct Cyc_Absyn_Exp*_tmp71E;struct Cyc_Absyn_Exp*_tmp71F;struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp721;struct Cyc_Absyn_Exp*_tmp722;struct Cyc_Absyn_Exp*_tmp723;struct Cyc_Absyn_Exp*_tmp724;struct Cyc_Absyn_Exp*_tmp725;switch(*((int*)_tmp6FE)){case 41U: _LL1: _tmp725=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp725;
_tmp724=e;goto _LL4;}case 20U: _LL3: _tmp724=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp724;
_tmp723=e;goto _LL6;}case 21U: _LL5: _tmp723=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp723;
_tmp722=e;goto _LL8;}case 22U: _LL7: _tmp722=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp722;
_tmp721=e;goto _LLA;}case 15U: _LL9: _tmp721=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp721;
_tmp720=e;goto _LLC;}case 11U: _LLB: _tmp720=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp720;
_tmp71F=e;goto _LLE;}case 12U: _LLD: _tmp71F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp71F;
_tmp71E=e;goto _LL10;}case 18U: _LLF: _tmp71E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp71E;
_tmp71D=e;goto _LL12;}case 5U: _LL11: _tmp71D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp71D;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3U: _LL13: _tmp71C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL14: {struct Cyc_List_List*es=_tmp71C;
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7U: _LL15: _tmp71A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp71B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp71A;struct Cyc_Absyn_Exp*e2=_tmp71B;
_tmp718=e1;_tmp719=e2;goto _LL18;}case 8U: _LL17: _tmp718=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp719=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp718;struct Cyc_Absyn_Exp*e2=_tmp719;
_tmp716=e1;_tmp717=e2;goto _LL1A;}case 9U: _LL19: _tmp716=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp717=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp716;struct Cyc_Absyn_Exp*e2=_tmp717;
_tmp714=e1;_tmp715=e2;goto _LL1C;}case 23U: _LL1B: _tmp714=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp715=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp714;struct Cyc_Absyn_Exp*e2=_tmp715;
_tmp712=e1;_tmp713=e2;goto _LL1E;}case 35U: _LL1D: _tmp712=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp713=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp712;struct Cyc_Absyn_Exp*e2=_tmp713;
_tmp710=e1;_tmp711=e2;goto _LL20;}case 4U: _LL1F: _tmp710=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp711=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FE)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp710;struct Cyc_Absyn_Exp*e2=_tmp711;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6U: _LL21: _tmp70D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp70E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_tmp70F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FE)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp70D;struct Cyc_Absyn_Exp*e2=_tmp70E;struct Cyc_Absyn_Exp*e3=_tmp70F;
# 4133
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10U: _LL23: _tmp70B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp70C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp70B;struct Cyc_List_List*es=_tmp70C;
# 4135
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14U: _LL25: _tmp709=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_tmp70A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL26: {void**t=_tmp709;struct Cyc_Absyn_Exp*e=_tmp70A;
({void*_tmpAF9=Cyc_Toc_typ_to_c(*t);*t=_tmpAF9;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25U: _LL27: _tmp707=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1)->f3;_tmp708=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL28: {void**t=_tmp707;struct Cyc_List_List*dles=_tmp708;
# 4138
({void*_tmpAFA=Cyc_Toc_typ_to_c(*t);*t=_tmpAFA;});
_tmp706=dles;goto _LL2A;}case 36U: _LL29: _tmp706=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6FE)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp706;
_tmp705=dles;goto _LL2C;}case 26U: _LL2B: _tmp705=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp705;
# 4142
for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19U: _LL2D: _tmp704=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL2E: {void**t=_tmp704;
_tmp703=t;goto _LL30;}case 17U: _LL2F: _tmp703=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL30: {void**t=_tmp703;
({void*_tmpAFB=Cyc_Toc_typ_to_c(*t);*t=_tmpAFB;});goto _LL0;}case 37U: _LL31: _tmp702=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL32: {struct Cyc_Absyn_Stmt*s=_tmp702;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34U: _LL33: _tmp701=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6FE)->f1;_LL34: {struct Cyc_Absyn_MallocInfo*m=_tmp701;
# 4149
if(m->elt_type != 0)
({void**_tmpAFD=({void**_tmp726=_cycalloc(sizeof(*_tmp726));({void*_tmpAFC=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp726=_tmpAFC;});_tmp726;});m->elt_type=_tmpAFD;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40U: _LL35: _tmp6FF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FE)->f3;_tmp700=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FE)->f4;_LL36: {struct Cyc_List_List*o=_tmp6FF;struct Cyc_List_List*i=_tmp700;
# 4154
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0U: _LL37: _LL38:
 goto _LL3A;case 1U: _LL39: _LL3A:
 goto _LL3C;case 32U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 42U: _LL3F: _LL40:
# 4162
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
({void*_tmp727=0U;({unsigned _tmpAFF=e->loc;struct _fat_ptr _tmpAFE=({const char*_tmp728="Cyclone expression in C code";_tag_fat(_tmp728,sizeof(char),29U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAFF,_tmpAFE,_tag_fat(_tmp727,sizeof(void*),0U));});});}_LL0:;}
# 4177
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp729=d->r;void*_stmttmp7A=_tmp729;void*_tmp72A=_stmttmp7A;struct Cyc_Absyn_Typedefdecl*_tmp72B;struct Cyc_Absyn_Enumdecl*_tmp72C;struct Cyc_Absyn_Aggrdecl*_tmp72D;struct Cyc_Absyn_Fndecl*_tmp72E;struct Cyc_Absyn_Vardecl*_tmp72F;switch(*((int*)_tmp72A)){case 0U: _LL1: _tmp72F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp72A)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp72F;
# 4180
({void*_tmpB00=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpB00;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp72E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp72A)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp72E;
# 4184
({void*_tmpB01=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpB01;});
{struct Cyc_List_List*_tmp730=(fd->i).args;struct Cyc_List_List*args=_tmp730;for(0;args != 0;args=args->tl){
({void*_tmpB02=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpB02;});}}
goto _LL0;}case 5U: _LL5: _tmp72D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp72A)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp72D;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7U: _LL7: _tmp72C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp72A)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp72C;
# 4190
if(ed->fields != 0){
struct Cyc_List_List*_tmp731=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp731;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp732=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp732;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4195
goto _LL0;}case 8U: _LL9: _tmp72B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp72A)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp72B;
({void*_tmpB03=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB03;});goto _LL0;}case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4205
({void*_tmp733=0U;({unsigned _tmpB05=d->loc;struct _fat_ptr _tmpB04=({const char*_tmp734="Cyclone declaration in C code";_tag_fat(_tmp734,sizeof(char),30U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB05,_tmpB04,_tag_fat(_tmp733,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4213
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp735=s->r;void*_stmttmp7B=_tmp735;void*_tmp736=_stmttmp7B;struct Cyc_Absyn_Stmt*_tmp737;struct Cyc_Absyn_Exp*_tmp739;struct Cyc_Absyn_Stmt*_tmp738;struct Cyc_Absyn_Stmt*_tmp73B;struct Cyc_Absyn_Decl*_tmp73A;void*_tmp73E;struct Cyc_List_List*_tmp73D;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Stmt*_tmp742;struct Cyc_Absyn_Exp*_tmp741;struct Cyc_Absyn_Exp*_tmp740;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Stmt*_tmp744;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_Absyn_Stmt*_tmp747;struct Cyc_Absyn_Stmt*_tmp746;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Exp*_tmp748;struct Cyc_Absyn_Stmt*_tmp74A;struct Cyc_Absyn_Stmt*_tmp749;struct Cyc_Absyn_Exp*_tmp74B;switch(*((int*)_tmp736)){case 1U: _LL1: _tmp74B=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp74B;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2U: _LL3: _tmp749=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_tmp74A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp736)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp749;struct Cyc_Absyn_Stmt*s2=_tmp74A;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3U: _LL5: _tmp748=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_LL6: {struct Cyc_Absyn_Exp*eopt=_tmp748;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4U: _LL7: _tmp745=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_tmp746=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp736)->f2;_tmp747=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp736)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp745;struct Cyc_Absyn_Stmt*s1=_tmp746;struct Cyc_Absyn_Stmt*s2=_tmp747;
# 4219
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5U: _LL9: _tmp743=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp736)->f1).f1;_tmp744=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp736)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp743;struct Cyc_Absyn_Stmt*s=_tmp744;
# 4221
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9U: _LLB: _tmp73F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_tmp740=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp736)->f2).f1;_tmp741=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp736)->f3).f1;_tmp742=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp736)->f4;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp73F;struct Cyc_Absyn_Exp*e2=_tmp740;struct Cyc_Absyn_Exp*e3=_tmp741;struct Cyc_Absyn_Stmt*s=_tmp742;
# 4223
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10U: _LLD: _tmp73C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_tmp73D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp736)->f2;_tmp73E=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp736)->f3;_LLE: {struct Cyc_Absyn_Exp*e=_tmp73C;struct Cyc_List_List*scs=_tmp73D;void*dec_tree=_tmp73E;
# 4226
Cyc_Toc_exptypes_to_c(e);
for(0;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12U: _LLF: _tmp73A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_tmp73B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp736)->f2;_LL10: {struct Cyc_Absyn_Decl*d=_tmp73A;struct Cyc_Absyn_Stmt*s=_tmp73B;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14U: _LL11: _tmp738=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp736)->f1;_tmp739=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp736)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp738;struct Cyc_Absyn_Exp*e=_tmp739;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13U: _LL13: _tmp737=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp736)->f2;_LL14: {struct Cyc_Absyn_Stmt*s=_tmp737;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4238
({void*_tmpB06=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->tag=0U;_tmp74C;});s->r=_tmpB06;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp74D=0U;({unsigned _tmpB08=s->loc;struct _fat_ptr _tmpB07=({const char*_tmp74E="Cyclone statement in C code";_tag_fat(_tmp74E,sizeof(char),28U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB08,_tmpB07,_tag_fat(_tmp74D,sizeof(void*),0U));});});}_LL0:;}
# 4246
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4248
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp750=({struct Cyc_Warn_String_Warn_Warg_struct _tmp804;_tmp804.tag=0U,({struct _fat_ptr _tmpB09=({const char*_tmp751="decls_to_c: not at toplevel!";_tag_fat(_tmp751,sizeof(char),29U);});_tmp804.f1=_tmpB09;});_tmp804;});void*_tmp74F[1U];_tmp74F[0]=& _tmp750;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp74F,sizeof(void*),1U));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp752=d->r;void*_stmttmp7C=_tmp752;void*_tmp753=_stmttmp7C;struct Cyc_List_List*_tmp754;struct Cyc_List_List*_tmp755;struct Cyc_List_List*_tmp756;struct Cyc_List_List*_tmp757;struct Cyc_Absyn_Typedefdecl*_tmp758;struct Cyc_Absyn_Enumdecl*_tmp759;struct Cyc_Absyn_Datatypedecl*_tmp75A;struct Cyc_Absyn_Aggrdecl*_tmp75B;struct Cyc_Absyn_Fndecl*_tmp75C;struct Cyc_Absyn_Vardecl*_tmp75D;switch(*((int*)_tmp753)){case 0U: _LL1: _tmp75D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp75D;
# 4255
struct _tuple1*_tmp75E=vd->name;struct _tuple1*c_name=_tmp75E;
# 4257
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp75F=_cycalloc(sizeof(*_tmp75F));({union Cyc_Absyn_Nmspace _tmpB0A=Cyc_Absyn_Abs_n(0,1);_tmp75F->f1=_tmpB0A;}),_tmp75F->f2=(*c_name).f2;_tmp75F;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4264
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4266
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpB0B=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpB0B;});
({void*_tmpB0C=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpB0C;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->hd=d,_tmp760->tl=Cyc_Toc_result_decls;_tmp760;});
goto _LL0;}case 1U: _LL3: _tmp75C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp75C;
# 4273
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB0E=({struct _tuple1*_tmp761=_cycalloc(sizeof(*_tmp761));({union Cyc_Absyn_Nmspace _tmpB0D=Cyc_Absyn_Abs_n(0,1);_tmp761->f1=_tmpB0D;}),_tmp761->f2=(*fd->name).f2;_tmp761;});fd->name=_tmpB0E;});
fd->sc=Cyc_Absyn_Public;}
# 4277
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->hd=d,_tmp762->tl=Cyc_Toc_result_decls;_tmp762;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp764=({struct Cyc_Warn_String_Warn_Warg_struct _tmp805;_tmp805.tag=0U,({struct _fat_ptr _tmpB0F=({const char*_tmp765="letdecl at toplevel";_tag_fat(_tmp765,sizeof(char),20U);});_tmp805.f1=_tmpB0F;});_tmp805;});void*_tmp763[1U];_tmp763[0]=& _tmp764;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp763,sizeof(void*),1U));});case 4U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp767=({struct Cyc_Warn_String_Warn_Warg_struct _tmp806;_tmp806.tag=0U,({struct _fat_ptr _tmpB10=({const char*_tmp768="region decl at toplevel";_tag_fat(_tmp768,sizeof(char),24U);});_tmp806.f1=_tmpB10;});_tmp806;});void*_tmp766[1U];_tmp766[0]=& _tmp767;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp766,sizeof(void*),1U));});case 5U: _LLB: _tmp75B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*sd=_tmp75B;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6U: _LLD: _tmp75A=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LLE: {struct Cyc_Absyn_Datatypedecl*tud=_tmp75A;
# 4285
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7U: _LLF: _tmp759=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp759;
# 4288
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->hd=d,_tmp769->tl=Cyc_Toc_result_decls;_tmp769;});
goto _LL0;}case 8U: _LL11: _tmp758=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp758;
# 4292
td->tvs=0;
if(td->defn != 0){
({void*_tmpB11=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB11;});{
# 4297
struct Cyc_Absyn_Decl*ed;
{void*_tmp76A=td->defn;void*_stmttmp7D=_tmp76A;void*_tmp76B=_stmttmp7D;unsigned _tmp76D;struct Cyc_Absyn_Enumdecl*_tmp76C;if(_tmp76B != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76B)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76B)->f1)->r)->tag == 1U){_LL24: _tmp76C=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76B)->f1)->r)->f1;_tmp76D=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76B)->f1)->loc;_LL25: {struct Cyc_Absyn_Enumdecl*ed2=_tmp76C;unsigned loc=_tmp76D;
# 4300
ed=({struct Cyc_Absyn_Decl*_tmp76F=_cycalloc(sizeof(*_tmp76F));({void*_tmpB12=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E->tag=7U,_tmp76E->f1=ed2;_tmp76E;});_tmp76F->r=_tmpB12;}),_tmp76F->loc=loc;_tmp76F;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4303
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->hd=ed,_tmp770->tl=Cyc_Toc_result_decls;_tmp770;});{
void*_tmp771=ed->r;void*_stmttmp7E=_tmp771;void*_tmp772=_stmttmp7E;struct Cyc_Absyn_Enumdecl*_tmp773;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp772)->tag == 7U){_LL29: _tmp773=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp772)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp773;
# 4307
({void*_tmpB14=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775->tag=0U,({void*_tmpB13=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->tag=15U,_tmp774->f1=ed->name,_tmp774->f2=ed;_tmp774;});_tmp775->f1=_tmpB13;}),_tmp775->f2=0;_tmp775;});td->defn=_tmpB14;});goto _LL28;}}else{_LL2B: _LL2C:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp777=({struct Cyc_Warn_String_Warn_Warg_struct _tmp807;_tmp807.tag=0U,({struct _fat_ptr _tmpB15=({const char*_tmp778="Toc: enumdecl to name";_tag_fat(_tmp778,sizeof(char),22U);});_tmp807.f1=_tmpB15;});_tmp807;});void*_tmp776[1U];_tmp776[0]=& _tmp777;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp776,sizeof(void*),1U));});}_LL28:;}}}}else{
# 4312
enum Cyc_Absyn_KindQual _tmp779=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7F=_tmp779;enum Cyc_Absyn_KindQual _tmp77A=_stmttmp7F;if(_tmp77A == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpB16=Cyc_Toc_void_star_type();td->defn=_tmpB16;});goto _LL2D;}else{_LL30: _LL31:
 td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4320
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B->hd=d,_tmp77B->tl=Cyc_Toc_result_decls;_tmp77B;});
goto _LL0;}case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp757=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp753)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp757;
_tmp756=ds2;goto _LL1E;}case 10U: _LL1D: _tmp756=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp753)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp756;
_tmp755=ds2;goto _LL20;}case 11U: _LL1F: _tmp755=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp755;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _LL21: _tmp754=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp753)->f1;_LL22: {struct Cyc_List_List*ds2=_tmp754;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4333
return nv;}
# 4338
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _tmp77C=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp80=_tmp77C;struct Cyc_Core_NewDynamicRegion _tmp77D=_stmttmp80;struct Cyc_Core_DynamicRegion*_tmp77E;_LL1: _tmp77E=_tmp77D.key;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp77E;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp780=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp780));_tmp780->dyn=dyn,_tmp780->state=ts;_tmp780;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp782=34;struct _fat_ptr**_tmp781=_cycalloc(_check_times(_tmp782,sizeof(struct _fat_ptr*)));_tmp781[0]=& Cyc_Toc__throw_str,_tmp781[1]=& Cyc_Toc_setjmp_str,_tmp781[2]=& Cyc_Toc__push_handler_str,_tmp781[3]=& Cyc_Toc__pop_handler_str,_tmp781[4]=& Cyc_Toc__exn_thrown_str,_tmp781[5]=& Cyc_Toc__npop_handler_str,_tmp781[6]=& Cyc_Toc__check_null_str,_tmp781[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp781[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp781[9]=& Cyc_Toc__check_fat_subscript_str,_tmp781[10]=& Cyc_Toc__tag_fat_str,_tmp781[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp781[12]=& Cyc_Toc__get_fat_size_str,_tmp781[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp781[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp781[15]=& Cyc_Toc__zero_arr_plus_str,_tmp781[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp781[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp781[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp781[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp781[20]=& Cyc_Toc__cycalloc_str,_tmp781[21]=& Cyc_Toc__cyccalloc_str,_tmp781[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp781[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp781[24]=& Cyc_Toc__region_malloc_str,_tmp781[25]=& Cyc_Toc__region_calloc_str,_tmp781[26]=& Cyc_Toc__check_times_str,_tmp781[27]=& Cyc_Toc__new_region_str,_tmp781[28]=& Cyc_Toc__push_region_str,_tmp781[29]=& Cyc_Toc__pop_region_str,_tmp781[30]=& Cyc_Toc__throw_arraybounds_str,_tmp781[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp781[32]=& Cyc_Toc__throw_match_str,_tmp781[33]=& Cyc_Toc__fast_region_malloc_str;_tmp781;}),sizeof(struct _fat_ptr*),34U);}}
# 4386
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp783=ts;struct Cyc_Toc_TocStateWrap*_tmp784=Cyc_Toc_toc_state;ts=_tmp784;Cyc_Toc_toc_state=_tmp783;});{
struct Cyc_Toc_TocStateWrap _tmp785=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp81=_tmp785;struct Cyc_Toc_TocStateWrap _tmp786=_stmttmp81;struct Cyc_Toc_TocState*_tmp788;struct Cyc_Core_DynamicRegion*_tmp787;_LL1: _tmp787=_tmp786.dyn;_tmp788=_tmp786.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp787;struct Cyc_Toc_TocState*s=_tmp788;
# 4391
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp78A=*s;struct Cyc_Toc_TocState _stmttmp82=_tmp78A;struct Cyc_Toc_TocState _tmp78B=_stmttmp82;struct Cyc_Xarray_Xarray*_tmp78C;_LL4: _tmp78C=_tmp78B.temp_labels;_LL5: {struct Cyc_Xarray_Xarray*tls=_tmp78C;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(tls);}}
# 4392
;}
# 4395
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4398
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4404
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4406
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp78D=_cycalloc(sizeof(*_tmp78D));*_tmp78D=pop_tables;_tmp78D;});
Cyc_Toc_init();{
struct _RegionHandle _tmp78E=_new_region("start");struct _RegionHandle*start=& _tmp78E;_push_region(start);
({struct _RegionHandle*_tmpB18=start;struct Cyc_Toc_Env*_tmpB17=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB18,_tmpB17,ds,0);});{
struct Cyc_List_List*_tmp78F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp78F;}
# 4409
;_pop_region();}}
