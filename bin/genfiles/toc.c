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
extern void Cyc_Core_ufree(void*ptr);
# 188
extern struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
extern struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 255 "core.h"
extern void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 57
extern struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
extern struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
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
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
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
# 912
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 914
extern void*Cyc_Absyn_sint_type;
# 919
extern void*Cyc_Absyn_heap_rgn_type;
# 923
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 925
extern void*Cyc_Absyn_void_type;
# 942
void*Cyc_Absyn_exn_type (void);
# 950
extern void*Cyc_Absyn_fat_bound_type;
# 954
void*Cyc_Absyn_bounds_one (void);
# 962
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 966
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 968
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 970
void*Cyc_Absyn_strct(struct _fat_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 976
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 981
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 992
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1056
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1061
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1078
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1082
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1087
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1125
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1130
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1146
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1148
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1150
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1159
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1167
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
# 188
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 223
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 230
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**ptr_type,int*is_fat,void**elt_type);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 277
void*Cyc_Tcutil_snd_tqt(struct _tuple13*);
# 295
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 306
int Cyc_Tcutil_force_type2bool(int desired,void*);struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp78E;_tmp78E.tag=0U,_tmp78E.f1=str;_tmp78E;});void*_tmp0[1U];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1U));});}
# 74
static void*Cyc_Toc_toc_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 76
({struct _fat_ptr _tmp807=(struct _fat_ptr)({struct _fat_ptr _tmp806=({const char*_tmp2="Toc: ";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_strconcat(_tmp806,(struct _fat_ptr)fmt);});Cyc_Warn_vimpos(_tmp807,ap);});}
# 78
static void*Cyc_Toc_toc_impos2(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=0U,({struct _fat_ptr _tmp808=({const char*_tmp5="Toc: ";_tag_fat(_tmp5,sizeof(char),6U);});_tmp6->f1=_tmp808;});_tmp6;});
struct _fat_ptr args=({unsigned _tmp4=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned _tmp78F=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;unsigned i;for(i=0;i < _tmp78F;++ i){i == (unsigned)0?_tmp3[i]=toc:(_tmp3[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - (unsigned)1))));}}0;});_tag_fat(_tmp3,sizeof(void*),_tmp4);});
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
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 125
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 132
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmpE=_region_malloc(d,sizeof(*_tmpE));
({struct Cyc_List_List**_tmp814=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpE->tuple_types=_tmp814;}),({
struct Cyc_List_List**_tmp813=({struct Cyc_List_List**_tmp8=_region_malloc(d,sizeof(*_tmp8));*_tmp8=0;_tmp8;});_tmpE->anon_aggr_types=_tmp813;}),({
struct Cyc_Dict_Dict*_tmp812=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));({struct Cyc_Dict_Dict _tmp811=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp9=_tmp811;});_tmp9;});_tmpE->aggrs_so_far=_tmp812;}),({
struct Cyc_List_List**_tmp810=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmpE->abs_struct_types=_tmp810;}),({
struct Cyc_Set_Set**_tmp80F=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Set_Set*_tmp80E=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp80E;});_tmpB;});_tmpE->datatypes_so_far=_tmp80F;}),({
struct Cyc_Dict_Dict*_tmp80D=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp80C=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp80C;});_tmpC;});_tmpE->xdatatypes_so_far=_tmp80D;}),({
struct Cyc_Dict_Dict*_tmp80B=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));({struct Cyc_Dict_Dict _tmp80A=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmpD=_tmp80A;});_tmpD;});_tmpE->qvar_tags=_tmp80B;}),({
struct Cyc_Xarray_Xarray*_tmp809=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpE->temp_labels=_tmp809;});_tmpE;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 151
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 157
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 160
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpF=ts;struct Cyc_Toc_TocStateWrap*_tmp10=Cyc_Toc_toc_state;ts=_tmp10;Cyc_Toc_toc_state=_tmpF;});{
struct Cyc_Toc_TocStateWrap _tmp11=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp11;struct Cyc_Toc_TocStateWrap _tmp12=_stmttmp0;struct Cyc_Toc_TocState*_tmp14;struct Cyc_Core_DynamicRegion*_tmp13;_LL1: _tmp13=_tmp12.dyn;_tmp14=_tmp12.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp13;struct Cyc_Toc_TocState*s=_tmp14;
struct _fat_ptr _tmp15=((struct _fat_ptr(*)(struct _fat_ptr ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp15;
({struct Cyc_Toc_TocStateWrap _tmp815=({struct Cyc_Toc_TocStateWrap _tmp790;_tmp790.dyn=dyn,_tmp790.state=s;_tmp790;});*ts=_tmp815;});
({struct Cyc_Toc_TocStateWrap*_tmp16=ts;struct Cyc_Toc_TocStateWrap*_tmp17=Cyc_Toc_toc_state;ts=_tmp17;Cyc_Toc_toc_state=_tmp16;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp19=({struct _tuple16 _tmp791;_tmp791.f1=s,_tmp791.f2=arg;_tmp791;});struct _tuple16 env=_tmp19;
res=f(h,& env);}
# 168
;}
# 170
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 174
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 177
struct _tuple18 _tmp1A=*env;struct _tuple18 _stmttmp1=_tmp1A;struct _tuple18 _tmp1B=_stmttmp1;void*(*_tmp1E)(struct _tuple1*);struct _tuple1*_tmp1D;struct Cyc_Toc_TocState*_tmp1C;_LL1: _tmp1C=_tmp1B.f1;_tmp1D=(_tmp1B.f2)->f1;_tmp1E=(_tmp1B.f2)->f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp1C;struct _tuple1*q=_tmp1D;void*(*type_maker)(struct _tuple1*)=_tmp1E;
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 182
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp792;_tmp792.f1=q,_tmp792.f2=type_maker;_tmp792;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1F[5U]="curr";
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
struct _tuple1*qv=({struct _tuple1*_tmp6F=_cycalloc(sizeof(*_tmp6F));((_tmp6F->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp81A=({struct _fat_ptr*_tmp6A[1U];({struct _fat_ptr*_tmp819=({struct _fat_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _fat_ptr _tmp818=({const char*_tmp6B="Core";_tag_fat(_tmp6B,sizeof(char),5U);});*_tmp6C=_tmp818;});_tmp6C;});_tmp6A[0]=_tmp819;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6A,sizeof(struct _fat_ptr*),1U));});((_tmp6F->f1).Abs_n).val=_tmp81A;}),({struct _fat_ptr*_tmp817=({struct _fat_ptr*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct _fat_ptr _tmp816=({const char*_tmp6D="get_exn_thrown";_tag_fat(_tmp6D,sizeof(char),15U);});*_tmp6E=_tmp816;});_tmp6E;});_tmp6F->f2=_tmp817;});_tmp6F;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=0U,_tmp69->f1=qv;_tmp69;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->topt=0,({void*_tmp81B=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=1U,_tmp67->f1=bnd;_tmp67;});_tmp68->r=_tmp81B;}),_tmp68->loc=0U,_tmp68->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp68;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=10U,_tmp66->f1=fnname,_tmp66->f2=0,_tmp66->f3=0,_tmp66->f4=0;_tmp66;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->topt=0,_tmp65->r=fncall_re,_tmp65->loc=0U,_tmp65->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp65;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 295
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 297
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_tmp81E=Cyc_Absyn_void_type;void*_tmp81D=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp81C=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_star_type(_tmp81E,_tmp81D,_tmp81C,Cyc_Absyn_false_type);});
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
r=({void*_tmp81F=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp81F,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 316
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 320
static struct _tuple20*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp72=_cycalloc(sizeof(*_tmp72));({struct Cyc_List_List*_tmp821=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));({void*_tmp820=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=1U,_tmp70->f1=name;_tmp70;});_tmp71->hd=_tmp820;}),_tmp71->tl=0;_tmp71;});_tmp72->f1=_tmp821;}),_tmp72->f2=e;_tmp72;});}
# 324
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp822=f;Cyc_Absyn_fncall_exp(_tmp822,((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(args),0U);});}
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
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=10U,_tmp7B->f1=e,({struct Cyc_List_List*_tmp823=((struct Cyc_List_List*(*)(struct _fat_ptr arr))Cyc_List_from_array)(es);_tmp7B->f2=_tmp823;}),_tmp7B->f3=0,_tmp7B->f4=1;_tmp7B;});}
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp795;_tmp795.tag=0U,({struct _fat_ptr _tmp824=({const char*_tmp8B="expression type ";_tag_fat(_tmp8B,sizeof(char),17U);});_tmp795.f1=_tmp824;});_tmp795;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp88=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp794;_tmp794.tag=2U,_tmp794.f1=(void*)t;_tmp794;});struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp793;_tmp793.tag=0U,({struct _fat_ptr _tmp825=({const char*_tmp8A=" (not int, float, double, or pointer)";_tag_fat(_tmp8A,sizeof(char),38U);});_tmp793.f1=_tmp825;});_tmp793;});void*_tmp86[3U];_tmp86[0]=& _tmp87,_tmp86[1]=& _tmp88,_tmp86[2]=& _tmp89;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp86,sizeof(void*),3U));});}_LL0:;}
# 422
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp8C;void*_tmp8D=_stmttmp5;void*_tmp8E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 3U){_LL1: _tmp8E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_type;_LL2: {void*et=_tmp8E;
return Cyc_Toc_getFunctionType(fS,et);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp796;_tmp796.tag=0U,({struct _fat_ptr _tmp826=({const char*_tmp91="impossible type (not pointer)";_tag_fat(_tmp91,sizeof(char),30U);});_tmp796.f1=_tmp826;});_tmp796;});void*_tmp8F[1U];_tmp8F[0]=& _tmp90;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp8F,sizeof(void*),1U));});}_LL0:;}
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
if(!Cyc_Toc_is_zero((*((struct _tuple20*)dles->hd)).f2))return 0;}
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
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp798;_tmp798.tag=0U,_tmp798.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp798;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U,_tmp797.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp797;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({struct _fat_ptr _tmp827=({const char*_tmpA8="%s_%s_struct";_tag_fat(_tmpA8,sizeof(char),13U);});Cyc_aprintf(_tmp827,_tag_fat(_tmpA7,sizeof(void*),2U));});});{
struct _RegionHandle _tmpAB=_new_region("r");struct _RegionHandle*r=& _tmpAB;_push_region(r);
{struct _fat_ptr _tmpB2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,_tmp79B.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp79B;});struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp79A;_tmp79A.tag=0U,({struct _fat_ptr _tmp82A=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp829=r;struct Cyc_List_List*_tmp828=vs;Cyc_rstr_sepstr(_tmp829,_tmp828,({const char*_tmpB1="_";_tag_fat(_tmpB1,sizeof(char),2U);}));}));_tmp79A.f1=_tmp82A;});_tmp79A;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,_tmp799.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp799;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAE,_tmpAC[1]=& _tmpAF,_tmpAC[2]=& _tmpB0;({struct _fat_ptr _tmp82B=({const char*_tmpAD="%s_%s_%s_struct";_tag_fat(_tmpAD,sizeof(char),16U);});Cyc_aprintf(_tmp82B,_tag_fat(_tmpAC,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB2;};_pop_region();}}}_LL3:;}}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 488
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 491
struct _tuple21 _tmpB3=*env;struct _tuple21 _stmttmp7=_tmpB3;struct _tuple21 _tmpB4=_stmttmp7;struct _tuple15*_tmpB6;struct Cyc_Dict_Dict*_tmpB5;_LL1: _tmpB5=(_tmpB4.f1)->qvar_tags;_tmpB6=_tmpB4.f2;_LL2: {struct Cyc_Dict_Dict*qvs=_tmpB5;struct _tuple15*pair=_tmpB6;
struct _tuple15 _tmpB7=*pair;struct _tuple15 _stmttmp8=_tmpB7;struct _tuple15 _tmpB8=_stmttmp8;struct _tuple1*_tmpBA;struct _tuple1*_tmpB9;_LL4: _tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;_LL5: {struct _tuple1*fieldname=_tmpB9;struct _tuple1*dtname=_tmpBA;
struct _handler_cons _tmpBB;_push_handler(& _tmpBB);{int _tmpBD=0;if(setjmp(_tmpBB.handler))_tmpBD=1;if(!_tmpBD){{struct _tuple1*_tmpBE=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0U);return _tmpBE;};_pop_handler();}else{void*_tmpBC=(void*)Cyc_Core_get_exn_thrown();void*_tmpBF=_tmpBC;void*_tmpC0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBF)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 495
struct _tuple15*_tmpC1=({struct _tuple15*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=fieldname,_tmpC7->f2=dtname;_tmpC7;});struct _tuple15*new_pair=_tmpC1;
struct _tuple1*_tmpC2=fieldname;struct _fat_ptr*_tmpC4;union Cyc_Absyn_Nmspace _tmpC3;_LLC: _tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_LLD: {union Cyc_Absyn_Nmspace nmspace=_tmpC3;struct _fat_ptr*fieldvar=_tmpC4;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->f1=nmspace,({struct _fat_ptr*_tmp82C=({struct _fat_ptr*_tmpC5=_cycalloc(sizeof(*_tmpC5));*_tmpC5=newvar;_tmpC5;});_tmpC6->f2=_tmp82C;});_tmpC6;});
({struct Cyc_Dict_Dict _tmp82D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp82D;});
return res;}}}else{_LL9: _tmpC0=_tmpBF;_LLA: {void*exn=_tmpC0;(int)_rethrow(exn);}}_LL6:;}}}}}
# 503
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp79C;_tmp79C.f1=fieldname,_tmp79C.f2=dtname;_tmp79C;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 510
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 513
struct _tuple1*_tmp830=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp82F=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp82F;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp830;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp82E=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->fields=fs,_tmpC9->tagged=0;_tmpC9;});_tmpCA->impl=_tmp82E;});_tmpCA;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 519
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 522
struct _tuple22 _tmpCB=*env;struct _tuple22 _stmttmp9=_tmpCB;struct _tuple22 _tmpCC=_stmttmp9;struct Cyc_List_List*_tmpCE;struct Cyc_List_List**_tmpCD;_LL1: _tmpCD=(_tmpCC.f1)->tuple_types;_tmpCE=_tmpCC.f2;_LL2: {struct Cyc_List_List**tuple_types=_tmpCD;struct Cyc_List_List*tqs0=_tmpCE;
# 524
{struct Cyc_List_List*_tmpCF=*tuple_types;struct Cyc_List_List*tts=_tmpCF;for(0;tts != 0;tts=tts->tl){
struct _tuple23*_tmpD0=(struct _tuple23*)tts->hd;struct _tuple23*_stmttmpA=_tmpD0;struct _tuple23*_tmpD1=_stmttmpA;struct Cyc_List_List*_tmpD3;void*_tmpD2;_LL4: _tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;_LL5: {void*x=_tmpD2;struct Cyc_List_List*ts=_tmpD3;
struct Cyc_List_List*_tmpD4=tqs0;struct Cyc_List_List*tqs=_tmpD4;
for(0;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple13*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 535
struct _fat_ptr*xname=({struct _fat_ptr*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _fat_ptr _tmp832=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp79D;_tmp79D.tag=1U,_tmp79D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79D;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _fat_ptr _tmp831=({const char*_tmpE0="_tuple%d";_tag_fat(_tmpE0,sizeof(char),9U);});Cyc_aprintf(_tmp831,_tag_fat(_tmpDF,sizeof(void*),1U));});});*_tmpE2=_tmp832;});_tmpE2;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmpD5=0;struct Cyc_List_List*fs=_tmpD5;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
fs=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct Cyc_Absyn_Aggrfield*_tmp834=({struct Cyc_Absyn_Aggrfield*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct _fat_ptr*_tmp833=Cyc_Absyn_fieldname(i);_tmpD6->name=_tmp833;}),_tmpD6->tq=Cyc_Toc_mt_tq,_tmpD6->type=(void*)ts2->hd,_tmpD6->width=0,_tmpD6->attributes=0,_tmpD6->requires_clause=0;_tmpD6;});_tmpD7->hd=_tmp834;}),_tmpD7->tl=fs;_tmpD7;});}}
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmpD8=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpD8;
void*_tmpD9=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDE=_cycalloc(sizeof(*_tmpDE));*_tmpDE=sd;_tmpDE;})),0);void*ans=_tmpD9;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct Cyc_Absyn_Decl*_tmp835=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->tag=5U,_tmpDA->f1=sd;_tmpDA;}),0U);_tmpDB->hd=_tmp835;}),_tmpDB->tl=Cyc_Toc_result_decls;_tmpDB;});
({struct Cyc_List_List*_tmp837=({struct Cyc_List_List*_tmpDD=_region_malloc(d,sizeof(*_tmpDD));({struct _tuple23*_tmp836=({struct _tuple23*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));_tmpDC->f1=ans,_tmpDC->f2=ts;_tmpDC;});_tmpDD->hd=_tmp836;}),_tmpDD->tl=*tuple_types;_tmpDD;});*tuple_types=_tmp837;});
return ans;}}}}
# 548
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 553
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 556
struct _tuple25*_tmpE3=env;struct Cyc_List_List*_tmpE6;enum Cyc_Absyn_AggrKind _tmpE5;struct Cyc_List_List**_tmpE4;_LL1: _tmpE4=(_tmpE3->f1)->anon_aggr_types;_tmpE5=(_tmpE3->f2)->f1;_tmpE6=(_tmpE3->f2)->f2;_LL2: {struct Cyc_List_List**anon_aggr_types=_tmpE4;enum Cyc_Absyn_AggrKind ak=_tmpE5;struct Cyc_List_List*fs=_tmpE6;
# 558
{struct Cyc_List_List*_tmpE7=*anon_aggr_types;struct Cyc_List_List*ts=_tmpE7;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpE8=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpE8;struct _tuple26*_tmpE9=_stmttmpB;struct Cyc_List_List*_tmpEC;enum Cyc_Absyn_AggrKind _tmpEB;void*_tmpEA;_LL4: _tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=_tmpE9->f3;_LL5: {void*x=_tmpEA;enum Cyc_Absyn_AggrKind ak2=_tmpEB;struct Cyc_List_List*fs2=_tmpEC;
if((int)ak != (int)ak2)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 567
struct _fat_ptr*xname=({struct _fat_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _fat_ptr _tmp839=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp79E;_tmp79E.tag=1U,_tmp79E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79E;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _fat_ptr _tmp838=({const char*_tmpF5="_tuple%d";_tag_fat(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp838,_tag_fat(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp839;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpED=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpED;
sd->kind=ak;{
void*_tmpEE=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF3=_cycalloc(sizeof(*_tmpF3));*_tmpF3=sd;_tmpF3;})),0);void*ans=_tmpEE;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_Decl*_tmp83A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=5U,_tmpEF->f1=sd;_tmpEF;}),0U);_tmpF0->hd=_tmp83A;}),_tmpF0->tl=Cyc_Toc_result_decls;_tmpF0;});
({struct Cyc_List_List*_tmp83C=({struct Cyc_List_List*_tmpF2=_region_malloc(d,sizeof(*_tmpF2));({struct _tuple26*_tmp83B=({struct _tuple26*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));_tmpF1->f1=ans,_tmpF1->f2=ak,_tmpF1->f3=fs;_tmpF1;});_tmpF2->hd=_tmp83B;}),_tmpF2->tl=*anon_aggr_types;_tmpF2;});*anon_aggr_types=_tmp83C;});
return ans;}}}}
# 575
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=ak,_tmpF8->f2=fs;_tmpF8;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 584
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 593
struct _tuple28 _tmpF9=*env;struct _tuple28 _stmttmpC=_tmpF9;struct _tuple28 _tmpFA=_stmttmpC;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;struct _tuple1*_tmpFC;struct Cyc_List_List**_tmpFB;_LL1: _tmpFB=(_tmpFA.f1)->abs_struct_types;_tmpFC=(_tmpFA.f2)->f1;_tmpFD=(_tmpFA.f2)->f2;_tmpFE=(_tmpFA.f2)->f3;_tmpFF=(_tmpFA.f2)->f4;_LL2: {struct Cyc_List_List**abs_struct_types=_tmpFB;struct _tuple1*struct_name=_tmpFC;struct Cyc_List_List*type_vars=_tmpFD;struct Cyc_List_List*type_args=_tmpFE;struct Cyc_List_List*fields=_tmpFF;
# 597
{struct Cyc_List_List*_tmp100=*abs_struct_types;struct Cyc_List_List*tts=_tmp100;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmp101=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmp101;struct _tuple29*_tmp102=_stmttmpD;void*_tmp105;struct Cyc_List_List*_tmp104;struct _tuple1*_tmp103;_LL4: _tmp103=_tmp102->f1;_tmp104=_tmp102->f2;_tmp105=_tmp102->f3;_LL5: {struct _tuple1*x=_tmp103;struct Cyc_List_List*ts2=_tmp104;void*t=_tmp105;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp83D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args);_tmp83D == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmp106=type_args;struct Cyc_List_List*ts=_tmp106;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmp107=(void*)ts->hd;void*t=_tmp107;
void*_tmp108=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmp108;
{struct Cyc_Absyn_Kind*_tmp109=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp109;struct Cyc_Absyn_Kind*_tmp10A=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp10A)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
 continue;default: _LLB: _LLC:
# 610
 if(Cyc_Unify_unify(t,t2)||({void*_tmp83E=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp83E,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 615
break;}}
# 617
if(okay)
return t;}}}}{
# 624
struct _fat_ptr*xname=({struct _fat_ptr*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct _fat_ptr _tmp840=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11C=({struct Cyc_Int_pa_PrintArg_struct _tmp79F;_tmp79F.tag=1U,_tmp79F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp79F;});void*_tmp11A[1U];_tmp11A[0]=& _tmp11C;({struct _fat_ptr _tmp83F=({const char*_tmp11B="_tuple%d";_tag_fat(_tmp11B,sizeof(char),9U);});Cyc_aprintf(_tmp83F,_tag_fat(_tmp11A,sizeof(void*),1U));});});*_tmp11D=_tmp840;});_tmp11D;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp10B=0;struct Cyc_List_List*fs=_tmp10B;
# 628
({struct Cyc_List_List*_tmp842=({struct Cyc_List_List*_tmp10D=_region_malloc(d,sizeof(*_tmp10D));({struct _tuple29*_tmp841=({struct _tuple29*_tmp10C=_region_malloc(d,sizeof(*_tmp10C));_tmp10C->f1=struct_name,_tmp10C->f2=type_args,_tmp10C->f3=x;_tmp10C;});_tmp10D->hd=_tmp841;}),_tmp10D->tl=*abs_struct_types;_tmp10D;});*abs_struct_types=_tmp842;});{
# 631
struct _RegionHandle _tmp10E=_new_region("r");struct _RegionHandle*r=& _tmp10E;_push_region(r);
{struct Cyc_List_List*_tmp10F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp10F;
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp110=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp110;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 641
if((fields->tl == 0 &&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));({void*_tmp843=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=6U,_tmp111->f1=0;_tmp111;});_tmp112->hd=_tmp843;}),_tmp112->tl=atts;_tmp112;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 648
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=4U,({void*_tmp846=Cyc_Toc_void_star_type();(_tmp113->f1).elt_type=_tmp846;}),({struct Cyc_Absyn_Tqual _tmp845=Cyc_Absyn_empty_tqual(0U);(_tmp113->f1).tq=_tmp845;}),({
struct Cyc_Absyn_Exp*_tmp844=Cyc_Absyn_uint_exp(0U,0U);(_tmp113->f1).num_elts=_tmp844;}),(_tmp113->f1).zero_term=Cyc_Absyn_false_type,(_tmp113->f1).zt_loc=0U;_tmp113;});
# 652
fs=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Aggrfield*_tmp847=({struct Cyc_Absyn_Aggrfield*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->name=f->name,_tmp114->tq=Cyc_Toc_mt_tq,_tmp114->type=t,_tmp114->width=f->width,_tmp114->attributes=atts,_tmp114->requires_clause=0;_tmp114;});_tmp115->hd=_tmp847;}),_tmp115->tl=fs;_tmp115;});}
# 654
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp116=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp116;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));({struct Cyc_Absyn_Decl*_tmp848=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->tag=5U,_tmp117->f1=sd;_tmp117;}),0U);_tmp118->hd=_tmp848;}),_tmp118->tl=Cyc_Toc_result_decls;_tmp118;});{
void*_tmp119=x;_npop_handler(0U);return _tmp119;}}}
# 632
;_pop_region();}}}}
# 660
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 664
struct _tuple27 env=({struct _tuple27 _tmp7A0;_tmp7A0.f1=struct_name,_tmp7A0.f2=type_vars,_tmp7A0.f3=type_args,_tmp7A0.f4=fields;_tmp7A0;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 672
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp84B=({struct _fat_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _fat_ptr _tmp84A=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp7A1;_tmp7A1.tag=1U,_tmp7A1.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp7A1;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _fat_ptr _tmp849=({const char*_tmp11F="_tmp%X";_tag_fat(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp849,_tag_fat(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp84A;});_tmp121;});_tmp122->f2=_tmp84B;});_tmp122;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 675
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp7A2;_tmp7A2.f1=v,({struct Cyc_Absyn_Exp*_tmp84C=Cyc_Absyn_var_exp(v,0U);_tmp7A2.f2=_tmp84C;});_tmp7A2;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 682
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp123=*env;struct _tuple31 _stmttmpF=_tmp123;struct _tuple31 _tmp124=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp125;_LL1: _tmp125=(_tmp124.f1)->temp_labels;_LL2: {struct Cyc_Xarray_Xarray*temp_labels=_tmp125;
int _tmp126=Cyc_Toc_fresh_label_counter ++;int i=_tmp126;
if(({int _tmp84D=i;_tmp84D < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct _fat_ptr _tmp84F=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12C=({struct Cyc_Int_pa_PrintArg_struct _tmp7A4;_tmp7A4.tag=1U,_tmp7A4.f1=(unsigned)i;_tmp7A4;});void*_tmp12A[1U];_tmp12A[0]=& _tmp12C;({struct _fat_ptr _tmp84E=({const char*_tmp12B="_LL%X";_tag_fat(_tmp12B,sizeof(char),6U);});Cyc_aprintf(_tmp84E,_tag_fat(_tmp12A,sizeof(void*),1U));});});*_tmp12D=_tmp84F;});_tmp12D;});
if(({int _tmp850=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp850 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp128=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A3;_tmp7A3.tag=0U,({struct _fat_ptr _tmp851=({const char*_tmp129="fresh_label: add_ind returned bad index...";_tag_fat(_tmp129,sizeof(char),43U);});_tmp7A3.f1=_tmp851;});_tmp7A3;});void*_tmp127[1U];_tmp127[0]=& _tmp128;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp127,sizeof(void*),1U));});
return res;}}}
# 692
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return((struct _fat_ptr*(*)(int arg,struct _fat_ptr*(*f)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 698
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp12E;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0){
++ ans;
fs=fs->tl;}
# 705
return Cyc_Absyn_uint_exp((unsigned)ans,0U);}
# 711
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->f1=(*a).f1,_tmp12F->f2=(*a).f2,({void*_tmp852=Cyc_Toc_typ_to_c((*a).f3);_tmp12F->f3=_tmp852;});_tmp12F;});}
# 733 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp130=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp130;void*_tmp131=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp132;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->tag == 4U){_LL1: _tmp132=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1;_LL2: {struct Cyc_Absyn_ArrayInfo ai=_tmp132;
return({void*_tmp853=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp853,ai.tq);});}}else{_LL3: _LL4:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 740
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 742
struct Cyc_Absyn_Aggrfield*_tmp133=({struct Cyc_Absyn_Aggrfield*_tmp134=_cycalloc(sizeof(*_tmp134));*_tmp134=*f;_tmp134;});struct Cyc_Absyn_Aggrfield*ans=_tmp133;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 749
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 751
return;}
# 754
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp135=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp135;void*_tmp136=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp137;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp136)->tag == 2U){_LL1: _tmp137=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp136)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp137;
return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 760
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp138=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp138;struct Cyc_Absyn_Kind*_tmp139=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp139)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 766
static int Cyc_Toc_is_void_star(void*t){
void*_tmp13A=Cyc_Tcutil_compress(t);void*_stmttmp13=_tmp13A;void*_tmp13B=_stmttmp13;void*_tmp13C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B)->tag == 3U){_LL1: _tmp13C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B)->f1).elt_type;_LL2: {void*t2=_tmp13C;
return Cyc_Tcutil_is_void_type(t2);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 772
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 775
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 779
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e);
# 781
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp13D=t;void**_tmp13F;struct Cyc_Absyn_Datatypedecl*_tmp13E;struct Cyc_Absyn_Enumdecl*_tmp140;struct Cyc_Absyn_Aggrdecl*_tmp141;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp143;void*_tmp147;struct Cyc_Absyn_Typedefdecl*_tmp146;struct Cyc_List_List*_tmp145;struct _tuple1*_tmp144;struct Cyc_List_List*_tmp149;enum Cyc_Absyn_AggrKind _tmp148;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp150;struct Cyc_Absyn_VarargInfo*_tmp14F;int _tmp14E;struct Cyc_List_List*_tmp14D;void*_tmp14C;struct Cyc_Absyn_Tqual _tmp14B;unsigned _tmp154;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Tqual _tmp152;void*_tmp151;void*_tmp157;struct Cyc_Absyn_Tqual _tmp156;void*_tmp155;struct Cyc_Absyn_Tvar*_tmp158;void**_tmp159;struct Cyc_List_List*_tmp15A;struct _tuple1*_tmp15B;struct Cyc_List_List*_tmp15E;union Cyc_Absyn_AggrInfo _tmp15D;void*_tmp15C;struct Cyc_Absyn_Datatypefield*_tmp160;struct Cyc_Absyn_Datatypedecl*_tmp15F;switch(*((int*)_tmp13D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 794
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp15F=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1).KnownDatatypefield).val).f1;_tmp160=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1).KnownDatatypefield).val).f2;_LLA: {struct Cyc_Absyn_Datatypedecl*tud=_tmp15F;struct Cyc_Absyn_Datatypefield*tuf=_tmp160;
# 796
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{_LLB: _LLC:
# 798
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A5;_tmp7A5.tag=0U,({struct _fat_ptr _tmp854=({const char*_tmp163="unresolved DatatypeFieldType";_tag_fat(_tmp163,sizeof(char),29U);});_tmp7A5.f1=_tmp854;});_tmp7A5;});void*_tmp161[1U];_tmp161[0]=& _tmp162;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp161,sizeof(void*),1U));});}case 1U: _LLF: _LL10:
# 807
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp15C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1;_tmp15D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_tmp15E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL1C: {void*c=_tmp15C;union Cyc_Absyn_AggrInfo info=_tmp15D;struct Cyc_List_List*ts=_tmp15E;
# 854
{union Cyc_Absyn_AggrInfo _tmp173=info;if((_tmp173.UnknownAggr).tag == 1){_LL45: _LL46:
# 856
 if(ts == 0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->tag=0U,_tmp174->f1=c,_tmp174->f2=0;_tmp174;});}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 860
struct Cyc_Absyn_Aggrdecl*_tmp175=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp175;
if(ad->expected_mem_kind){
# 863
if(ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp177=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7A7;_tmp7A7.tag=6U,_tmp7A7.f1=ad;_tmp7A7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A6;_tmp7A6.tag=0U,({struct _fat_ptr _tmp855=({const char*_tmp179=" was never defined.";_tag_fat(_tmp179,sizeof(char),20U);});_tmp7A6.f1=_tmp855;});_tmp7A6;});void*_tmp176[2U];_tmp176[0]=& _tmp177,_tmp176[1]=& _tmp178;Cyc_Warn_warn2(0U,_tag_fat(_tmp176,sizeof(void*),2U));});}
# 867
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp17A=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp17A;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp17B=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp17B;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp7A9;_tmp7A9.tag=6U,_tmp7A9.f1=ad;_tmp7A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A8;_tmp7A8.tag=0U,({struct _fat_ptr _tmp856=({const char*_tmp17F=" ended up being abstract.";_tag_fat(_tmp17F,sizeof(char),26U);});_tmp7A8.f1=_tmp856;});_tmp7A8;});void*_tmp17C[2U];_tmp17C[0]=& _tmp17D,_tmp17C[1]=& _tmp17E;Cyc_Warn_warn2(0U,_tag_fat(_tmp17C,sizeof(void*),2U));});{
# 879
struct _RegionHandle _tmp180=_new_region("r");struct _RegionHandle*r=& _tmp180;_push_region(r);
{struct Cyc_List_List*_tmp181=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp181;
void*_tmp182=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp182;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp183=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp183;}{
void*_tmp184=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0U);return _tmp184;}}
# 880
;_pop_region();}}
# 885
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15U: _LL1D: _tmp15B=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_LL1E: {struct _tuple1*tdn=_tmp15B;
return t;}case 16U: _LL1F: _tmp15A=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)->f1;_LL20: {struct Cyc_List_List*fs=_tmp15A;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4U: _LL23: _LL24:
# 897
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 902
 return Cyc_Toc_void_star_type();}case 1U: _LL3: _tmp159=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13D)->f2;_LL4: {void**t2=_tmp159;
# 785
if(*t2 == 0){
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 789
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));}case 2U: _LL5: _tmp158=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13D)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv=_tmp158;
# 791
if((int)(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;else{
return Cyc_Toc_void_star_type();}}case 3U: _LLD: _tmp155=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).elt_type;_tmp156=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).elt_tq;_tmp157=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D)->f1).ptr_atts).bounds;_LLE: {void*t2=_tmp155;struct Cyc_Absyn_Tqual tq=_tmp156;void*bnds=_tmp157;
# 802
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();else{
# 806
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}}case 4U: _LL13: _tmp151=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).elt_type;_tmp152=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).tq;_tmp153=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).num_elts;_tmp154=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13D)->f1).zt_loc;_LL14: {void*t2=_tmp151;struct Cyc_Absyn_Tqual tq=_tmp152;struct Cyc_Absyn_Exp*e=_tmp153;unsigned ztl=_tmp154;
# 810
return({void*_tmp85A=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp859=tq;struct Cyc_Absyn_Exp*_tmp858=e;void*_tmp857=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp85A,_tmp859,_tmp858,_tmp857,ztl);});}case 5U: _LL15: _tmp14B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).ret_tqual;_tmp14C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).ret_type;_tmp14D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).args;_tmp14E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).c_varargs;_tmp14F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).cyc_varargs;_tmp150=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13D)->f1).attributes;_LL16: {struct Cyc_Absyn_Tqual tq2=_tmp14B;void*t2=_tmp14C;struct Cyc_List_List*args=_tmp14D;int c_vararg=_tmp14E;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp14F;struct Cyc_List_List*atts=_tmp150;
# 816
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
# 827
struct Cyc_List_List*_tmp168=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp168;
if(cyc_vararg != 0){
# 830
void*_tmp169=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));void*t=_tmp169;
struct _tuple9*_tmp16A=({struct _tuple9*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->f1=cyc_vararg->name,_tmp16C->f2=cyc_vararg->tq,_tmp16C->f3=t;_tmp16C;});struct _tuple9*vararg=_tmp16A;
new_args=({struct Cyc_List_List*_tmp85B=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp85B,({struct Cyc_List_List*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->hd=vararg,_tmp16B->tl=0;_tmp16B;}));});}
# 834
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->tag=5U,(_tmp16D->f1).tvars=0,(_tmp16D->f1).effect=0,(_tmp16D->f1).ret_tqual=tq2,({void*_tmp85C=Cyc_Toc_typ_to_c(t2);(_tmp16D->f1).ret_type=_tmp85C;}),(_tmp16D->f1).args=new_args,(_tmp16D->f1).c_varargs=c_vararg,(_tmp16D->f1).cyc_varargs=0,(_tmp16D->f1).rgn_po=0,(_tmp16D->f1).attributes=new_atts,(_tmp16D->f1).requires_clause=0,(_tmp16D->f1).requires_relns=0,(_tmp16D->f1).ensures_clause=0,(_tmp16D->f1).ensures_relns=0,(_tmp16D->f1).return_value=0;_tmp16D;});}}case 6U: _LL17: _tmp14A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp13D)->f1;_LL18: {struct Cyc_List_List*tqs=_tmp14A;
# 839
struct Cyc_List_List*_tmp16E=0;struct Cyc_List_List*tqs2=_tmp16E;
for(0;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp170=_cycalloc(sizeof(*_tmp170));({struct _tuple13*_tmp85E=({struct _tuple13*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=(*((struct _tuple13*)tqs->hd)).f1,({void*_tmp85D=Cyc_Toc_typ_to_c((*((struct _tuple13*)tqs->hd)).f2);_tmp16F->f2=_tmp85D;});_tmp16F;});_tmp170->hd=_tmp85E;}),_tmp170->tl=tqs2;_tmp170;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tqs2));}case 7U: _LL19: _tmp148=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13D)->f1;_tmp149=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13D)->f2;_LL1A: {enum Cyc_Absyn_AggrKind k=_tmp148;struct Cyc_List_List*fs=_tmp149;
# 848
struct Cyc_List_List*_tmp171=0;struct Cyc_List_List*fs2=_tmp171;
for(0;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp172=_cycalloc(sizeof(*_tmp172));({struct Cyc_Absyn_Aggrfield*_tmp860=({struct Cyc_Absyn_Aggrfield*_tmp85F=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp85F,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp172->hd=_tmp860;}),_tmp172->tl=fs2;_tmp172;});}
return({enum Cyc_Absyn_AggrKind _tmp861=k;Cyc_Toc_add_anon_aggr_type(_tmp861,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs2));});}case 8U: _LL21: _tmp144=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f1;_tmp145=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f2;_tmp146=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f3;_tmp147=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp13D)->f4;_LL22: {struct _tuple1*tdn=_tmp144;struct Cyc_List_List*ts=_tmp145;struct Cyc_Absyn_Typedefdecl*td=_tmp146;void*topt=_tmp147;
# 891
if(topt == 0){
if(ts != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->tag=8U,_tmp185->f1=tdn,_tmp185->f2=0,_tmp185->f3=td,_tmp185->f4=0;_tmp185;});else{
return t;}}else{
# 896
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->tag=8U,_tmp186->f1=tdn,_tmp186->f2=0,_tmp186->f3=td,({void*_tmp862=Cyc_Toc_typ_to_c(topt);_tmp186->f4=_tmp862;});_tmp186;});}}case 9U: _LL2B: _tmp143=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp13D)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp143;
# 907
return t;}case 11U: _LL2D: _tmp142=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp13D)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp142;
# 912
Cyc_Toc_exptypes_to_c(e);
if(e->topt != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)){case 0U: _LL2F: _tmp141=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)->f1;_LL30: {struct Cyc_Absyn_Aggrdecl*ad=_tmp141;
# 917
Cyc_Toc_aggrdecl_to_c(ad);
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp140=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)->f1;_LL32: {struct Cyc_Absyn_Enumdecl*ed=_tmp140;
# 922
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _LL33: _tmp13E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f1)->r)->f1;_tmp13F=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13D)->f2;_LL34: {struct Cyc_Absyn_Datatypedecl*dd=_tmp13E;void**t=_tmp13F;
# 925
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}}_LL0:;}
# 930
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp187=t;struct Cyc_Absyn_Tqual _tmp189;void*_tmp188;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->tag == 4U){_LL1: _tmp188=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->f1).elt_type;_tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp187)->f1).tq;_LL2: {void*t2=_tmp188;struct Cyc_Absyn_Tqual tq=_tmp189;
# 933
return({void*_tmp863=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp863,e);});}}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 940
static int Cyc_Toc_atomic_type(void*t){
void*_tmp18A=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp18A;void*_tmp18B=_stmttmp15;struct Cyc_List_List*_tmp18C;struct Cyc_List_List*_tmp18D;void*_tmp18E;struct Cyc_List_List*_tmp190;void*_tmp18F;switch(*((int*)_tmp18B)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp18F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f1;_tmp190=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2;_LL4: {void*c=_tmp18F;struct Cyc_List_List*ts=_tmp190;
# 944
void*_tmp191=c;struct Cyc_Absyn_Datatypefield*_tmp193;struct Cyc_Absyn_Datatypedecl*_tmp192;union Cyc_Absyn_AggrInfo _tmp194;switch(*((int*)_tmp191)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D: return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp194=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp191)->f1;_LL25: {union Cyc_Absyn_AggrInfo info=_tmp194;
# 951
{union Cyc_Absyn_AggrInfo _tmp195=info;if((_tmp195.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 955
struct Cyc_Absyn_Aggrdecl*_tmp196=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp196;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp197=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp197;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp191)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp192=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp191)->f1).KnownDatatypefield).val).f1;_tmp193=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp191)->f1).KnownDatatypefield).val).f2;_LL27: {struct Cyc_Absyn_Datatypedecl*tud=_tmp192;struct Cyc_Absyn_Datatypefield*tuf=_tmp193;
# 962
{struct Cyc_List_List*_tmp198=tuf->typs;struct Cyc_List_List*tqs=_tmp198;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple13*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AB;_tmp7AB.tag=0U,({struct _fat_ptr _tmp864=({const char*_tmp19C="atomic_typ: bad type ";_tag_fat(_tmp19C,sizeof(char),22U);});_tmp7AB.f1=_tmp864;});_tmp7AB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AA;_tmp7AA.tag=2U,_tmp7AA.f1=(void*)t;_tmp7AA;});void*_tmp199[2U];_tmp199[0]=& _tmp19A,_tmp199[1]=& _tmp19B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp199,sizeof(void*),2U));});}_LL11:;}case 5U: _LL5: _LL6:
# 967
 return 1;case 4U: _LL7: _tmp18E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18B)->f1).elt_type;_LL8: {void*t=_tmp18E;
return Cyc_Toc_atomic_type(t);}case 7U: _LL9: _tmp18D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18B)->f2;_LLA: {struct Cyc_List_List*fs=_tmp18D;
# 975
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6U: _LLB: _tmp18C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18B)->f1;_LLC: {struct Cyc_List_List*tqs=_tmp18C;
# 979
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple13*)tqs->hd)).f2))return 0;}
return 1;}case 3U: _LLD: _LLE:
# 984
 return 0;default: _LLF: _LL10:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AD;_tmp7AD.tag=0U,({struct _fat_ptr _tmp865=({const char*_tmp1A0="atomic_typ:  bad type ";_tag_fat(_tmp1A0,sizeof(char),23U);});_tmp7AD.f1=_tmp865;});_tmp7AD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7AC;_tmp7AC.tag=2U,_tmp7AC.f1=(void*)t;_tmp7AC;});void*_tmp19D[2U];_tmp19D[0]=& _tmp19E,_tmp19D[1]=& _tmp19F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp19D,sizeof(void*),2U));});}_LL0:;}
# 990
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp1A1=Cyc_Tcutil_compress(t);void*_stmttmp16=_tmp1A1;void*_tmp1A2=_stmttmp16;struct Cyc_List_List*_tmp1A3;union Cyc_Absyn_AggrInfo _tmp1A4;switch(*((int*)_tmp1A2)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f1)->tag == 20U){_LL1: _tmp1A4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp1A4;
# 993
struct Cyc_Absyn_Aggrdecl*_tmp1A5=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A5;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AE;_tmp7AE.tag=0U,({struct _fat_ptr _tmp866=({const char*_tmp1A8="is_poly_field: type missing fields";_tag_fat(_tmp1A8,sizeof(char),35U);});_tmp7AE.f1=_tmp866;});_tmp7AE;});void*_tmp1A6[1U];_tmp1A6[0]=& _tmp1A7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1A6,sizeof(void*),1U));});
_tmp1A3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1A3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A2)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A3;
# 998
struct Cyc_Absyn_Aggrfield*_tmp1A9=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp1A9;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B0;_tmp7B0.tag=0U,({struct _fat_ptr _tmp867=({const char*_tmp1AD="is_poly_field: bad field ";_tag_fat(_tmp1AD,sizeof(char),26U);});_tmp7B0.f1=_tmp867;});_tmp7B0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AF;_tmp7AF.tag=0U,_tmp7AF.f1=*f;_tmp7AF;});void*_tmp1AA[2U];_tmp1AA[0]=& _tmp1AB,_tmp1AA[1]=& _tmp1AC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AA,sizeof(void*),2U));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B2;_tmp7B2.tag=0U,({struct _fat_ptr _tmp868=({const char*_tmp1B1="is_poly_field: bad type ";_tag_fat(_tmp1B1,sizeof(char),25U);});_tmp7B2.f1=_tmp868;});_tmp7B2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B1;_tmp7B1.tag=2U,_tmp7B1.f1=(void*)t;_tmp7B1;});void*_tmp1AE[2U];_tmp1AE[0]=& _tmp1AF,_tmp1AE[1]=& _tmp1B0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1AE,sizeof(void*),2U));});}_LL0:;}
# 1009
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B2=e->r;void*_stmttmp17=_tmp1B2;void*_tmp1B3=_stmttmp17;struct _fat_ptr*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct _fat_ptr*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;switch(*((int*)_tmp1B3)){case 21U: _LL1: _tmp1B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B3)->f1;_tmp1B7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B3)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp1B6;struct _fat_ptr*f=_tmp1B7;
# 1012
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22U: _LL3: _tmp1B4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B3)->f1;_tmp1B5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B3)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1B4;struct _fat_ptr*f=_tmp1B5;
# 1015
void*_tmp1B8=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp18=_tmp1B8;void*_tmp1B9=_stmttmp18;void*_tmp1BA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->tag == 3U){_LL8: _tmp1BA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->f1).elt_type;_LL9: {void*t=_tmp1BA;
# 1017
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B4;_tmp7B4.tag=0U,({struct _fat_ptr _tmp869=({const char*_tmp1BE="is_poly_project: bad type ";_tag_fat(_tmp1BE,sizeof(char),27U);});_tmp7B4.f1=_tmp869;});_tmp7B4;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1BD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B3;_tmp7B3.tag=2U,_tmp7B3.f1=(void*)_check_null(e1->topt);_tmp7B3;});void*_tmp1BB[2U];_tmp1BB[0]=& _tmp1BC,_tmp1BB[1]=& _tmp1BD;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp1BB,sizeof(void*),2U));});}_LL7:;}default: _LL5: _LL6:
# 1020
 return 0;}_LL0:;}
# 1025
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1BF[1U];_tmp1BF[0]=s;({struct Cyc_Absyn_Exp*_tmp86A=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86A,_tag_fat(_tmp1BF,sizeof(struct Cyc_Absyn_Exp*),1U));});});}
# 1030
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C0[2U];_tmp1C0[0]=s,_tmp1C0[1]=n;({struct Cyc_Absyn_Exp*_tmp86B=fn_e;Cyc_Toc_fncall_exp_dl(_tmp86B,_tag_fat(_tmp1C0,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1035
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C1[2U];_tmp1C1[0]=rgn,_tmp1C1[1]=s;({struct Cyc_Absyn_Exp*_tmp86C=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp86C,_tag_fat(_tmp1C1,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C2[2U];_tmp1C2[0]=rgn,_tmp1C2[1]=s;({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp86D,_tag_fat(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 1043
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C3[3U];_tmp1C3[0]=rgn,_tmp1C3[1]=s,_tmp1C3[2]=n;({struct Cyc_Absyn_Exp*_tmp86E=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp86E,_tag_fat(_tmp1C3,sizeof(struct Cyc_Absyn_Exp*),3U));});});}
# 1047
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(({void*_tmp1C4=0U;({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp86F,_tag_fat(_tmp1C4,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);}
# 1052
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1060
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C5=e->r;void*_stmttmp19=_tmp1C5;void*_tmp1C6=_stmttmp19;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C6)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C6)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4: {
# 1065
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));({struct Cyc_Absyn_Decl*_tmp870=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->tag=0U,_tmp1C7->f1=vd;_tmp1C7;}),0U);_tmp1C8->hd=_tmp870;}),_tmp1C8->tl=Cyc_Toc_result_decls;_tmp1C8;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
# 1073
 xexp=({void*_tmp871=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp871,e);});
# 1075
xplussz=({void*_tmp872=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp872,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1078
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1C9[3U];({struct _tuple20*_tmp875=({struct _tuple20*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->f1=0,_tmp1CA->f2=xexp;_tmp1CA;});_tmp1C9[0]=_tmp875;}),({
struct _tuple20*_tmp874=({struct _tuple20*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->f1=0,_tmp1CB->f2=xexp;_tmp1CB;});_tmp1C9[1]=_tmp874;}),({
struct _tuple20*_tmp873=({struct _tuple20*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=0,_tmp1CC->f2=xplussz;_tmp1CC;});_tmp1C9[2]=_tmp873;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C9,sizeof(struct _tuple20*),3U));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1113 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1CE=_region_malloc(r,sizeof(*_tmp1CE));_tmp1CE->break_lab=0,_tmp1CE->continue_lab=0,_tmp1CE->fallthru_info=0,_tmp1CE->toplevel=1,({int*_tmp876=({int*_tmp1CD=_region_malloc(r,sizeof(*_tmp1CD));*_tmp1CD=0;_tmp1CD;});_tmp1CE->in_lhs=_tmp876;}),_tmp1CE->rgn=r;_tmp1CE;});}
# 1117
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1CF=nv;int _tmp1D0;_LL1: _tmp1D0=_tmp1CF->toplevel;_LL2: {int t=_tmp1D0;
return t;}}
# 1121
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D1=e;int*_tmp1D6;int _tmp1D5;struct Cyc_Toc_FallthruInfo*_tmp1D4;struct _fat_ptr**_tmp1D3;struct _fat_ptr**_tmp1D2;_LL1: _tmp1D2=_tmp1D1->break_lab;_tmp1D3=_tmp1D1->continue_lab;_tmp1D4=_tmp1D1->fallthru_info;_tmp1D5=_tmp1D1->toplevel;_tmp1D6=_tmp1D1->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D2;struct _fat_ptr**c=_tmp1D3;struct Cyc_Toc_FallthruInfo*f=_tmp1D4;int t=_tmp1D5;int*lhs=_tmp1D6;
return({struct Cyc_Toc_Env*_tmp1D7=_region_malloc(r,sizeof(*_tmp1D7));_tmp1D7->break_lab=b,_tmp1D7->continue_lab=c,_tmp1D7->fallthru_info=f,_tmp1D7->toplevel=0,_tmp1D7->in_lhs=lhs,_tmp1D7->rgn=r;_tmp1D7;});}}
# 1125
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D8=e;int*_tmp1DD;int _tmp1DC;struct Cyc_Toc_FallthruInfo*_tmp1DB;struct _fat_ptr**_tmp1DA;struct _fat_ptr**_tmp1D9;_LL1: _tmp1D9=_tmp1D8->break_lab;_tmp1DA=_tmp1D8->continue_lab;_tmp1DB=_tmp1D8->fallthru_info;_tmp1DC=_tmp1D8->toplevel;_tmp1DD=_tmp1D8->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1D9;struct _fat_ptr**c=_tmp1DA;struct Cyc_Toc_FallthruInfo*f=_tmp1DB;int t=_tmp1DC;int*lhs=_tmp1DD;
return({struct Cyc_Toc_Env*_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));_tmp1DE->break_lab=b,_tmp1DE->continue_lab=c,_tmp1DE->fallthru_info=f,_tmp1DE->toplevel=1,_tmp1DE->in_lhs=lhs,_tmp1DE->rgn=r;_tmp1DE;});}}
# 1129
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1DF=nv;int*_tmp1E0;_LL1: _tmp1E0=_tmp1DF->in_lhs;_LL2: {int*b=_tmp1E0;
return*b;}}
# 1133
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E1=e;int*_tmp1E2;_LL1: _tmp1E2=_tmp1E1->in_lhs;_LL2: {int*lhs=_tmp1E2;
*lhs=b;}}
# 1138
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E3=e;int*_tmp1E8;int _tmp1E7;struct Cyc_Toc_FallthruInfo*_tmp1E6;struct _fat_ptr**_tmp1E5;struct _fat_ptr**_tmp1E4;_LL1: _tmp1E4=_tmp1E3->break_lab;_tmp1E5=_tmp1E3->continue_lab;_tmp1E6=_tmp1E3->fallthru_info;_tmp1E7=_tmp1E3->toplevel;_tmp1E8=_tmp1E3->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1E4;struct _fat_ptr**c=_tmp1E5;struct Cyc_Toc_FallthruInfo*f=_tmp1E6;int t=_tmp1E7;int*lhs=_tmp1E8;
return({struct Cyc_Toc_Env*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9->break_lab=b,_tmp1E9->continue_lab=c,_tmp1E9->fallthru_info=f,_tmp1E9->toplevel=t,_tmp1E9->in_lhs=lhs,_tmp1E9->rgn=r;_tmp1E9;});}}
# 1145
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EA=e;int*_tmp1EF;int _tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1ED;struct _fat_ptr**_tmp1EC;struct _fat_ptr**_tmp1EB;_LL1: _tmp1EB=_tmp1EA->break_lab;_tmp1EC=_tmp1EA->continue_lab;_tmp1ED=_tmp1EA->fallthru_info;_tmp1EE=_tmp1EA->toplevel;_tmp1EF=_tmp1EA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1EB;struct _fat_ptr**c=_tmp1EC;struct Cyc_Toc_FallthruInfo*f=_tmp1ED;int t=_tmp1EE;int*lhs=_tmp1EF;
return({struct Cyc_Toc_Env*_tmp1F0=_region_malloc(r,sizeof(*_tmp1F0));_tmp1F0->break_lab=0,_tmp1F0->continue_lab=0,_tmp1F0->fallthru_info=f,_tmp1F0->toplevel=t,_tmp1F0->in_lhs=lhs,_tmp1F0->rgn=r;_tmp1F0;});}}
# 1151
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1155
struct Cyc_Toc_Env*_tmp1F1=e;int*_tmp1F6;int _tmp1F5;struct Cyc_Toc_FallthruInfo*_tmp1F4;struct _fat_ptr**_tmp1F3;struct _fat_ptr**_tmp1F2;_LL1: _tmp1F2=_tmp1F1->break_lab;_tmp1F3=_tmp1F1->continue_lab;_tmp1F4=_tmp1F1->fallthru_info;_tmp1F5=_tmp1F1->toplevel;_tmp1F6=_tmp1F1->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1F2;struct _fat_ptr**c=_tmp1F3;struct Cyc_Toc_FallthruInfo*f=_tmp1F4;int t=_tmp1F5;int*lhs=_tmp1F6;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));
_tmp1F9->label=fallthru_l,_tmp1F9->binders=fallthru_binders;_tmp1F9;});
return({struct Cyc_Toc_Env*_tmp1F8=_region_malloc(r,sizeof(*_tmp1F8));({struct _fat_ptr**_tmp877=({struct _fat_ptr**_tmp1F7=_region_malloc(r,sizeof(*_tmp1F7));*_tmp1F7=break_l;_tmp1F7;});_tmp1F8->break_lab=_tmp877;}),_tmp1F8->continue_lab=c,_tmp1F8->fallthru_info=fi,_tmp1F8->toplevel=t,_tmp1F8->in_lhs=lhs,_tmp1F8->rgn=r;_tmp1F8;});}}
# 1160
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1162
struct Cyc_Toc_Env*_tmp1FA=e;int*_tmp1FF;int _tmp1FE;struct Cyc_Toc_FallthruInfo*_tmp1FD;struct _fat_ptr**_tmp1FC;struct _fat_ptr**_tmp1FB;_LL1: _tmp1FB=_tmp1FA->break_lab;_tmp1FC=_tmp1FA->continue_lab;_tmp1FD=_tmp1FA->fallthru_info;_tmp1FE=_tmp1FA->toplevel;_tmp1FF=_tmp1FA->in_lhs;_LL2: {struct _fat_ptr**b=_tmp1FB;struct _fat_ptr**c=_tmp1FC;struct Cyc_Toc_FallthruInfo*f=_tmp1FD;int t=_tmp1FE;int*lhs=_tmp1FF;
return({struct Cyc_Toc_Env*_tmp201=_region_malloc(r,sizeof(*_tmp201));({struct _fat_ptr**_tmp878=({struct _fat_ptr**_tmp200=_region_malloc(r,sizeof(*_tmp200));*_tmp200=break_l;_tmp200;});_tmp201->break_lab=_tmp878;}),_tmp201->continue_lab=c,_tmp201->fallthru_info=0,_tmp201->toplevel=t,_tmp201->in_lhs=lhs,_tmp201->rgn=r;_tmp201;});}}
# 1169
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1171
struct Cyc_Toc_Env*_tmp202=e;int*_tmp207;int _tmp206;struct Cyc_Toc_FallthruInfo*_tmp205;struct _fat_ptr**_tmp204;struct _fat_ptr**_tmp203;_LL1: _tmp203=_tmp202->break_lab;_tmp204=_tmp202->continue_lab;_tmp205=_tmp202->fallthru_info;_tmp206=_tmp202->toplevel;_tmp207=_tmp202->in_lhs;_LL2: {struct _fat_ptr**b=_tmp203;struct _fat_ptr**c=_tmp204;struct Cyc_Toc_FallthruInfo*f=_tmp205;int t=_tmp206;int*lhs=_tmp207;
return({struct Cyc_Toc_Env*_tmp209=_region_malloc(r,sizeof(*_tmp209));_tmp209->break_lab=0,_tmp209->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp879=({struct Cyc_Toc_FallthruInfo*_tmp208=_region_malloc(r,sizeof(*_tmp208));_tmp208->label=next_l,_tmp208->binders=0;_tmp208;});_tmp209->fallthru_info=_tmp879;}),_tmp209->toplevel=t,_tmp209->in_lhs=lhs,_tmp209->rgn=r;_tmp209;});}}
# 1184 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1187
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp20A=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp1A=_tmp20A;struct _tuple32*_tmp20B=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp20C;_LL1: _tmp20C=_tmp20B->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp20C;
Cyc_Toc_exptypes_to_c(e);
l=l->tl;}}}
# 1195
static void Cyc_Toc_asm_iolist_toc(struct Cyc_Toc_Env*nv,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple32*_tmp20D=(struct _tuple32*)l->hd;struct _tuple32*_stmttmp1B=_tmp20D;struct _tuple32*_tmp20E=_stmttmp1B;struct Cyc_Absyn_Exp*_tmp20F;_LL1: _tmp20F=_tmp20E->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp20F;
Cyc_Toc_exp_to_c(nv,e);
l=l->tl;}}}
# 1203
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp210=e->annot;void*_stmttmp1C=_tmp210;void*_tmp211=_stmttmp1C;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp211)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp213=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B5;_tmp7B5.tag=0U,({struct _fat_ptr _tmp87A=({const char*_tmp214="Toc: do_null_check";_tag_fat(_tmp214,sizeof(char),19U);});_tmp7B5.f1=_tmp87A;});_tmp7B5;});void*_tmp212[1U];_tmp212[0]=& _tmp213;({unsigned _tmp87B=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp87B,_tag_fat(_tmp212,sizeof(void*),1U));});});}}}}}}_LL0:;}
# 1219
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1221
int ans;
int _tmp215=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp215;
void*_tmp216=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp216;
void*_tmp217=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp217;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp218=old_typ;void*_tmp21C;void*_tmp21B;struct Cyc_Absyn_Tqual _tmp21A;void*_tmp219;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->tag == 3U){_LL1: _tmp219=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).elt_type;_tmp21A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).elt_tq;_tmp21B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).ptr_atts).bounds;_tmp21C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp218)->f1).ptr_atts).zero_term;_LL2: {void*ta=_tmp219;struct Cyc_Absyn_Tqual tq=_tmp21A;void*b=_tmp21B;void*zt=_tmp21C;
# 1232
{void*_tmp21D=annot;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp21D)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1234
 if(!((unsigned)({void*_tmp87C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp87C,b);}))){
# 1237
void*_tmp220=({void*_tmp87D=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp87D,tq);});void*newt=_tmp220;
({void*_tmp880=({void*_tmp87F=newt;Cyc_Toc_cast_it_r(_tmp87F,({struct Cyc_Absyn_Exp*_tmp87E=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp87E,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp880;});
ptr->topt=newt;}
# 1241
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp21D)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1244
 if(!((unsigned)({void*_tmp881=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp881,b);}))){
# 1246
void*_tmp221=({void*_tmp882=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp882,tq);});void*newt=_tmp221;
({void*_tmp885=({void*_tmp884=newt;Cyc_Toc_cast_it_r(_tmp884,({struct Cyc_Absyn_Exp*_tmp883=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp883,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp885;});
ptr->topt=newt;
# 1253
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp223=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B6;_tmp7B6.tag=0U,({struct _fat_ptr _tmp886=({const char*_tmp224="subscript of ? with no bounds check but need null check";_tag_fat(_tmp224,sizeof(char),56U);});_tmp7B6.f1=_tmp886;});_tmp7B6;});void*_tmp222[1U];_tmp222[0]=& _tmp223;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp222,sizeof(void*),1U));});}
# 1257
({void*_tmp88A=({void*_tmp889=new_typ;Cyc_Toc_cast_it_r(_tmp889,({struct Cyc_Absyn_Exp*_tmp225[1U];({struct Cyc_Absyn_Exp*_tmp887=Cyc_Absyn_new_exp(ptr->r,0U);_tmp225[0]=_tmp887;});({struct Cyc_Absyn_Exp*_tmp888=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp888,_tag_fat(_tmp225,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});ptr->r=_tmp88A;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp21D)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp21D)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1263
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp88F=({void*_tmp88E=newt;Cyc_Toc_cast_it_r(_tmp88E,({struct Cyc_Absyn_Exp*_tmp226[3U];({
struct Cyc_Absyn_Exp*_tmp88C=Cyc_Absyn_new_exp(ptr->r,0U);_tmp226[0]=_tmp88C;}),({
struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp226[1]=_tmp88B;}),_tmp226[2]=ind;({struct Cyc_Absyn_Exp*_tmp88D=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp88D,_tag_fat(_tmp226,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1266
ptr->r=_tmp88F;});
# 1270
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp21D)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp21F=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp21D)->f1;_LLF: {struct Cyc_Absyn_Exp*bd=_tmp21F;
# 1274
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp21E=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp21D)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp21E=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp21D)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp21E;
# 1277
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1282
struct _tuple14 _tmp227=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple14 _stmttmp1D=_tmp227;struct _tuple14 _tmp228=_stmttmp1D;int _tmp22A;unsigned _tmp229;_LL15: _tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;_LL16: {unsigned i=_tmp229;int valid=_tmp22A;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1285
({void*_tmp894=({void*_tmp893=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp893,({struct Cyc_Absyn_Exp*_tmp22B[4U];({
struct Cyc_Absyn_Exp*_tmp891=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22B[0]=_tmp891;}),_tmp22B[1]=bd,({
struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp22B[2]=_tmp890;}),_tmp22B[3]=ind;({struct Cyc_Absyn_Exp*_tmp892=fn_e;Cyc_Toc_fncall_exp_dl(_tmp892,_tag_fat(_tmp22B,sizeof(struct Cyc_Absyn_Exp*),4U));});}));});
# 1285
ptr->r=_tmp894;});
# 1288
ans=1;}else{
# 1291
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1294
({void*_tmp898=({void*_tmp897=new_typ;Cyc_Toc_cast_it_r(_tmp897,({struct Cyc_Absyn_Exp*_tmp22C[1U];({struct Cyc_Absyn_Exp*_tmp895=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp22C[0]=_tmp895;});({struct Cyc_Absyn_Exp*_tmp896=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp896,_tag_fat(_tmp22C,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 1294
ptr->r=_tmp898;});
# 1296
ans=0;}else{
# 1300
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp89C=({void*_tmp89B=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp89B,({struct Cyc_Absyn_Exp*_tmp22D[3U];({
struct Cyc_Absyn_Exp*_tmp899=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22D[0]=_tmp899;}),_tmp22D[1]=bd,_tmp22D[2]=ind;({struct Cyc_Absyn_Exp*_tmp89A=fn_e;Cyc_Toc_fncall_exp_dl(_tmp89A,_tag_fat(_tmp22D,sizeof(struct Cyc_Absyn_Exp*),3U));});}));});
# 1301
ptr->r=_tmp89C;});
# 1303
ans=1;}}
# 1306
goto _LL5;}}}else{_LL12: _LL13:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B7;_tmp7B7.tag=0U,({struct _fat_ptr _tmp89D=({const char*_tmp230="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp230,sizeof(char),34U);});_tmp7B7.f1=_tmp89D;});_tmp7B7;});void*_tmp22E[1U];_tmp22E[0]=& _tmp22F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp22E,sizeof(void*),1U));});}}}}}}_LL5:;}
# 1309
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp232=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B8;_tmp7B8.tag=0U,({struct _fat_ptr _tmp89E=({const char*_tmp233="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp233,sizeof(char),31U);});_tmp7B8.f1=_tmp89E;});_tmp7B8;});void*_tmp231[1U];_tmp231[0]=& _tmp232;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp231,sizeof(void*),1U));});}_LL0:;}}
# 1315
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B9;_tmp7B9.tag=0U,({struct _fat_ptr _tmp89F=({const char*_tmp236="Missing type in primop ";_tag_fat(_tmp236,sizeof(char),24U);});_tmp7B9.f1=_tmp89F;});_tmp7B9;});void*_tmp234[1U];_tmp234[0]=& _tmp235;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp234,sizeof(void*),1U));});
return(void*)_check_null(e->topt);}
# 1319
static struct _tuple13*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple13*_tmp237=_cycalloc(sizeof(*_tmp237));_tmp237->f1=Cyc_Toc_mt_tq,({void*_tmp8A0=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp237->f2=_tmp8A0;});_tmp237;});}
# 1324
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp238=e->r;void*_stmttmp1E=_tmp238;void*_tmp239=_stmttmp1E;int _tmp23B;struct Cyc_Absyn_Exp*_tmp23A;int _tmp23D;struct Cyc_Absyn_Exp*_tmp23C;struct Cyc_List_List*_tmp23E;switch(*((int*)_tmp239)){case 26U: _LL1: _tmp23E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL2: {struct Cyc_List_List*dles=_tmp23E;
# 1327
{struct Cyc_List_List*_tmp23F=dles;struct Cyc_List_List*dles2=_tmp23F;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple20*)dles2->hd)).f1 != 0)
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp240="array designators for abstract-field initialization";_tag_fat(_tmp240,sizeof(char),52U);}));}}
_tmp23C=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles),0U);_tmp23D=0;goto _LL4;}case 27U: _LL3: _tmp23C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_tmp23D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp239)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp23C;int zt=_tmp23D;
_tmp23A=bd;_tmp23B=zt;goto _LL6;}case 28U: _LL5: _tmp23A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp23B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp239)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp23A;int zt=_tmp23B;
# 1333
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp8A1=bd;Cyc_Absyn_add_exp(_tmp8A1,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1342
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1350
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp241=e->r;void*_stmttmp1F=_tmp241;void*_tmp242=_stmttmp1F;struct Cyc_List_List*_tmp243;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp242)->tag == 29U){_LL1: _tmp243=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp242)->f3;_LL2: {struct Cyc_List_List*dles2=_tmp243;
dles=dles2;goto _LL0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1356
{void*_tmp244=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp20=_tmp244;void*_tmp245=_stmttmp20;struct Cyc_Absyn_Aggrdecl*_tmp246;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp246=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp246;
# 1358
if(ad->impl == 0)
return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1366
 return 0;}_LL5:;}
# 1368
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp247=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp247;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp248=(struct _tuple20*)dles->hd;struct _tuple20*_stmttmp21=_tmp248;struct _tuple20*_tmp249=_stmttmp21;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_List_List*_tmp24A;_LLB: _tmp24A=_tmp249->f1;_tmp24B=_tmp249->f2;_LLC: {struct Cyc_List_List*ds=_tmp24A;struct Cyc_Absyn_Exp*e2=_tmp24B;
struct _fat_ptr*_tmp24C=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp24C;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp24D=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp24D;
if(nested_ans != 0)
return nested_ans;{
void*_tmp24E=Cyc_Tcutil_compress(last_type_field->type);void*_stmttmp22=_tmp24E;void*_tmp24F=_stmttmp22;struct Cyc_Absyn_Exp*_tmp251;void*_tmp250;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F)->tag == 4U){_LLE: _tmp250=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F)->f1).elt_type;_tmp251=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F)->f1).num_elts;_LLF: {void*elt_type=_tmp250;struct Cyc_Absyn_Exp*type_bd=_tmp251;
# 1383
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1389
return({struct Cyc_Absyn_Exp*_tmp8A4=({struct Cyc_Absyn_Exp*_tmp252[2U];_tmp252[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1391
struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp252[1]=_tmp8A2;});({struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp8A3,_tag_fat(_tmp252,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1389
Cyc_Absyn_add_exp(_tmp8A4,
# 1392
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{_LL10: _LL11:
 return 0;}_LLD:;}}}}
# 1397
({struct Cyc_Warn_String_Warn_Warg_struct _tmp254=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BA;_tmp7BA.tag=0U,({struct _fat_ptr _tmp8A5=({const char*_tmp255="get_varsizeexp: did not find last struct field";_tag_fat(_tmp255,sizeof(char),47U);});_tmp7BA.f1=_tmp8A5;});_tmp7BA;});void*_tmp253[1U];_tmp253[0]=& _tmp254;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp253,sizeof(void*),1U));});}}
# 1400
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp256=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp256;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp257=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*field=_tmp257;
if(Cyc_strcmp((struct _fat_ptr)*field->name,(struct _fat_ptr)*f)== 0)return i;
++ i;}}
# 1407
({struct Cyc_Warn_String_Warn_Warg_struct _tmp259=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BD;_tmp7BD.tag=0U,({struct _fat_ptr _tmp8A6=({const char*_tmp25D="get_member_offset ";_tag_fat(_tmp25D,sizeof(char),19U);});_tmp7BD.f1=_tmp8A6;});_tmp7BD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BC;_tmp7BC.tag=0U,_tmp7BC.f1=*f;_tmp7BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BB;_tmp7BB.tag=0U,({struct _fat_ptr _tmp8A7=({const char*_tmp25C=" failed";_tag_fat(_tmp25C,sizeof(char),8U);});_tmp7BB.f1=_tmp8A7;});_tmp7BB;});void*_tmp258[3U];_tmp258[0]=& _tmp259,_tmp258[1]=& _tmp25A,_tmp258[2]=& _tmp25B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp258,sizeof(void*),3U));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1411
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1414
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1417
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->tag=5U,_tmp25E->f1=e1,_tmp25E->f2=incr;_tmp25E;}),0U);}
# 1421
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1430
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1432
void*_tmp25F=e1->r;void*_stmttmp23=_tmp25F;void*_tmp260=_stmttmp23;int _tmp264;int _tmp263;struct _fat_ptr*_tmp262;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp266;void*_tmp265;struct Cyc_Absyn_Stmt*_tmp267;switch(*((int*)_tmp260)){case 37U: _LL1: _tmp267=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp260)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp267;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14U: _LL3: _tmp265=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp260)->f1;_tmp266=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp260)->f2;_LL4: {void*t=_tmp265;struct Cyc_Absyn_Exp*e=_tmp266;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21U: _LL5: _tmp261=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp260)->f1;_tmp262=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp260)->f2;_tmp263=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp260)->f3;_tmp264=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp260)->f4;_LL6: {struct Cyc_Absyn_Exp*e=_tmp261;struct _fat_ptr*fld=_tmp262;int is_tagged=_tmp263;int is_read=_tmp264;
# 1437
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp8AA=e1;struct Cyc_List_List*_tmp8A9=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=fld,_tmp268->tl=fs;_tmp268;});struct Cyc_Absyn_Exp*(*_tmp8A8)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp8AA,_tmp8A9,_tmp8A8,f_env);});
goto _LL0;}default: _LL7: _LL8: {
# 1445
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1447
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp8AB=(f(e1_copy,f_env))->r;e1->r=_tmp8AB;});
goto _LL0;}}_LL0:;}
# 1453
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1455
void*_tmp269=s->r;void*_stmttmp24=_tmp269;void*_tmp26A=_stmttmp24;struct Cyc_Absyn_Stmt*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26D;struct Cyc_Absyn_Decl*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;switch(*((int*)_tmp26A)){case 1U: _LL1: _tmp26E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp26A)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp26E;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12U: _LL3: _tmp26C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26A)->f1;_tmp26D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26A)->f2;_LL4: {struct Cyc_Absyn_Decl*d=_tmp26C;struct Cyc_Absyn_Stmt*s2=_tmp26D;
# 1458
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2U: _LL5: _tmp26B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp26A)->f2;_LL6: {struct Cyc_Absyn_Stmt*s2=_tmp26B;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp270=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BF;_tmp7BF.tag=0U,({struct _fat_ptr _tmp8AC=({const char*_tmp272="lvalue_assign_stmt: ";_tag_fat(_tmp272,sizeof(char),21U);});_tmp7BF.f1=_tmp8AC;});_tmp7BF;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp271=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7BE;_tmp7BE.tag=5U,_tmp7BE.f1=s;_tmp7BE;});void*_tmp26F[2U];_tmp26F[0]=& _tmp270,_tmp26F[1]=& _tmp271;Cyc_Toc_toc_impos2(_tag_fat(_tmp26F,sizeof(void*),2U));});}_LL0:;}
# 1464
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1468
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp273=e->r;void*_stmttmp25=_tmp273;void*_tmp274=_stmttmp25;struct Cyc_Absyn_Stmt*_tmp275;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp**_tmp278;void**_tmp277;switch(*((int*)_tmp274)){case 14U: _LL1: _tmp277=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp274)->f1;_tmp278=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp274)->f2;_LL2: {void**t=_tmp277;struct Cyc_Absyn_Exp**e1=_tmp278;
# 1471
({struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Toc_push_address_exp(*e1);*e1=_tmp8AD;});
({void*_tmp8AE=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp8AE;});
return e;}case 20U: _LL3: _tmp276=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp274)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp276;
# 1475
return e1;}case 37U: _LL5: _tmp275=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp274)->f1;_LL6: {struct Cyc_Absyn_Stmt*s=_tmp275;
# 1479
Cyc_Toc_push_address_stmt(s);
return e;}default: _LL7: _LL8:
# 1482
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C1;_tmp7C1.tag=0U,({struct _fat_ptr _tmp8AF=({const char*_tmp27C="can't take & of exp ";_tag_fat(_tmp27C,sizeof(char),21U);});_tmp7C1.f1=_tmp8AF;});_tmp7C1;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp27B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7C0;_tmp7C0.tag=4U,_tmp7C0.f1=e;_tmp7C0;});void*_tmp279[2U];_tmp279[0]=& _tmp27A,_tmp279[1]=& _tmp27B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp279,sizeof(void*),2U));});}_LL0:;}
# 1486
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp27D=s->r;void*_stmttmp26=_tmp27D;void*_tmp27E=_stmttmp26;struct Cyc_Absyn_Exp**_tmp27F;struct Cyc_Absyn_Stmt*_tmp280;struct Cyc_Absyn_Stmt*_tmp281;switch(*((int*)_tmp27E)){case 2U: _LL1: _tmp281=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp27E)->f2;_LL2: {struct Cyc_Absyn_Stmt*s2=_tmp281;
_tmp280=s2;goto _LL4;}case 12U: _LL3: _tmp280=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp27E)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp280;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1U: _LL5: _tmp27F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp27E)->f1;_LL6: {struct Cyc_Absyn_Exp**e=_tmp27F;
({struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Toc_push_address_exp(*e);*e=_tmp8B0;});goto _LL0;}default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp283=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C3;_tmp7C3.tag=0U,({struct _fat_ptr _tmp8B1=({const char*_tmp285="can't take & of stmt ";_tag_fat(_tmp285,sizeof(char),22U);});_tmp7C3.f1=_tmp8B1;});_tmp7C3;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp284=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7C2;_tmp7C2.tag=5U,_tmp7C2.f1=s;_tmp7C2;});void*_tmp282[2U];_tmp282[0]=& _tmp283,_tmp282[1]=& _tmp284;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp282,sizeof(void*),2U));});}_LL0:;}
# 1498
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1510
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp286=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp286;
void*_tmp287=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp287;
void*_tmp288=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp288;
struct Cyc_Core_Opt*_tmp289=({struct Cyc_Core_Opt*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->v=c_ptr_type;_tmp2BC;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp289;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp28A=e1->r;void*_stmttmp27=_tmp28A;void*_tmp28B=_stmttmp27;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28E;switch(*((int*)_tmp28B)){case 20U: _LL1: _tmp28E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp28B)->f1;_LL2: {struct Cyc_Absyn_Exp*ea=_tmp28E;
# 1518
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1523
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23U: _LL3: _tmp28C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28B)->f1;_tmp28D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp28B)->f2;_LL4: {struct Cyc_Absyn_Exp*ea=_tmp28C;struct Cyc_Absyn_Exp*eb=_tmp28D;
# 1525
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1530
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp28F[3U];_tmp28F[0]=ea,({
struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp28F[1]=_tmp8B2;}),_tmp28F[2]=eb;({struct Cyc_Absyn_Exp*_tmp8B3=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp8B3,_tag_fat(_tmp28F,sizeof(struct Cyc_Absyn_Exp*),3U));});});
goto _LL0;}default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp291=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C4;_tmp7C4.tag=0U,({struct _fat_ptr _tmp8B4=({const char*_tmp292="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp292,sizeof(char),53U);});_tmp7C4.f1=_tmp8B4;});_tmp7C4;});void*_tmp290[1U];_tmp290[0]=& _tmp291;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp290,sizeof(void*),1U));});}_LL0:;}{
# 1536
struct _tuple1*_tmp293=Cyc_Toc_temp_var();struct _tuple1*x=_tmp293;
struct _RegionHandle _tmp294=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp294;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp295=({struct Cyc_Absyn_Vardecl*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->sc=Cyc_Absyn_Public,_tmp2BB->name=x,_tmp2BB->varloc=0U,_tmp2BB->tq=Cyc_Toc_mt_tq,_tmp2BB->type=c_fat_ptr_type,_tmp2BB->initializer=xinit,_tmp2BB->rgn=0,_tmp2BB->attributes=0,_tmp2BB->escapes=0,_tmp2BB->is_proto=0;_tmp2BB;});struct Cyc_Absyn_Vardecl*x_vd=_tmp295;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp296=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->tag=4U,_tmp2BA->f1=x_vd;_tmp2BA;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp296;
struct Cyc_Absyn_Exp*_tmp297=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp297;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp298=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp298;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp299=Cyc_Toc_temp_var();struct _tuple1*y=_tmp299;
struct Cyc_Absyn_Vardecl*_tmp29A=({struct Cyc_Absyn_Vardecl*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->sc=Cyc_Absyn_Public,_tmp2B9->name=y,_tmp2B9->varloc=0U,_tmp2B9->tq=Cyc_Toc_mt_tq,_tmp2B9->type=c_elt_type,_tmp2B9->initializer=deref_x,_tmp2B9->rgn=0,_tmp2B9->attributes=0,_tmp2B9->escapes=0,_tmp2B9->is_proto=0;_tmp2B9;});struct Cyc_Absyn_Vardecl*y_vd=_tmp29A;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29B=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->tag=4U,_tmp2B8->f1=y_vd;_tmp2B8;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp29B;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp29C;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp8B6=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp8B5=y_exp;Cyc_Absyn_prim2_exp(_tmp8B6,_tmp8B5,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1557
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp29D=Cyc_Toc_temp_var();struct _tuple1*z=_tmp29D;
struct Cyc_Absyn_Vardecl*_tmp29E=({struct Cyc_Absyn_Vardecl*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->sc=Cyc_Absyn_Public,_tmp2B7->name=z,_tmp2B7->varloc=0U,_tmp2B7->tq=Cyc_Toc_mt_tq,_tmp2B7->type=c_elt_type,_tmp2B7->initializer=z_init,_tmp2B7->rgn=0,_tmp2B7->attributes=0,_tmp2B7->escapes=0,_tmp2B7->is_proto=0;_tmp2B7;});struct Cyc_Absyn_Vardecl*z_vd=_tmp29E;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29F=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->tag=4U,_tmp2B6->f1=z_vd;_tmp2B6;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp29F;
# 1563
struct Cyc_Absyn_Exp*_tmp2A0=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp2A0;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A1=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp2A1;
struct Cyc_Absyn_Exp*_tmp2A2=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp2A2;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1570
struct Cyc_Absyn_Exp*_tmp2A3=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp2A3;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp2A4=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp2A4;
struct Cyc_Absyn_Exp*_tmp2A5=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp2A5;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1577
struct Cyc_List_List*_tmp2A6=({struct Cyc_Absyn_Exp*_tmp2B5[2U];({struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp2B5[0]=_tmp8B8;}),({
struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B5[1]=_tmp8B7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2B5,sizeof(struct Cyc_Absyn_Exp*),2U));});
# 1577
struct Cyc_List_List*xsizeargs=_tmp2A6;
# 1579
struct Cyc_Absyn_Exp*_tmp2A7=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp2A7;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp8B9,oneexp,0U);});{
# 1584
struct Cyc_Absyn_Exp*_tmp2A8=({struct Cyc_Absyn_Exp*_tmp8BA=xsize;Cyc_Absyn_and_exp(_tmp8BA,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp2A8;
struct Cyc_Absyn_Stmt*_tmp2A9=Cyc_Absyn_exp_stmt(({void*_tmp2B4=0U;({struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp8BB,_tag_fat(_tmp2B4,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp2A9;
struct Cyc_Absyn_Exp*_tmp2AA=({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp8BC,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp2AA;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp2AB=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp2AB;
struct Cyc_Absyn_Exp*_tmp2AC=({struct Cyc_Absyn_Exp*_tmp8BD=deref_xcurr;Cyc_Absyn_assign_exp(_tmp8BD,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp2AC;
struct Cyc_Absyn_Stmt*_tmp2AD=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp2AD;
s=({struct Cyc_Absyn_Stmt*_tmp8C0=({struct Cyc_Absyn_Exp*_tmp8BF=comp_exp;struct Cyc_Absyn_Stmt*_tmp8BE=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp8BF,_tmp8BE,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp8C0,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C2=({struct Cyc_Absyn_Decl*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));({void*_tmp8C1=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->tag=0U,_tmp2AE->f1=z_vd;_tmp2AE;});_tmp2AF->r=_tmp8C1;}),_tmp2AF->loc=0U;_tmp2AF;});Cyc_Absyn_decl_stmt(_tmp8C2,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C4=({struct Cyc_Absyn_Decl*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));({void*_tmp8C3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->tag=0U,_tmp2B0->f1=y_vd;_tmp2B0;});_tmp2B1->r=_tmp8C3;}),_tmp2B1->loc=0U;_tmp2B1;});Cyc_Absyn_decl_stmt(_tmp8C4,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8C6=({struct Cyc_Absyn_Decl*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));({void*_tmp8C5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->tag=0U,_tmp2B2->f1=x_vd;_tmp2B2;});_tmp2B3->r=_tmp8C5;}),_tmp2B3->loc=0U;_tmp2B3;});Cyc_Absyn_decl_stmt(_tmp8C6,s,0U);});
({void*_tmp8C7=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8C7;});}}}}}}}}}}
# 1538
;_pop_region();}}
# 1610 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1614
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2BD=Cyc_Tcutil_compress(aggrtype);void*_stmttmp28=_tmp2BD;void*_tmp2BE=_stmttmp28;union Cyc_Absyn_AggrInfo _tmp2BF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BE)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BE)->f1)->tag == 20U){_LL1: _tmp2BF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BE)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2BF;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C7;_tmp7C7.tag=0U,({struct _fat_ptr _tmp8C8=({const char*_tmp2C5="expecting union but found ";_tag_fat(_tmp2C5,sizeof(char),27U);});_tmp7C7.f1=_tmp8C8;});_tmp7C7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C6;_tmp7C6.tag=2U,_tmp7C6.f1=(void*)aggrtype;_tmp7C6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C5;_tmp7C5.tag=0U,({
struct _fat_ptr _tmp8C9=({const char*_tmp2C4=" in check_tagged_union";_tag_fat(_tmp2C4,sizeof(char),23U);});_tmp7C5.f1=_tmp8C9;});_tmp7C5;});void*_tmp2C0[3U];_tmp2C0[0]=& _tmp2C1,_tmp2C0[1]=& _tmp2C2,_tmp2C0[2]=& _tmp2C3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C0,sizeof(void*),3U));});}_LL0:;}{
# 1620
struct _tuple30 _tmp2C6=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp29=_tmp2C6;struct _tuple30 _tmp2C7=_stmttmp29;struct Cyc_Absyn_Exp*_tmp2C9;struct _tuple1*_tmp2C8;_LL6: _tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;_LL7: {struct _tuple1*temp=_tmp2C8;struct Cyc_Absyn_Exp*temp_exp=_tmp2C9;
struct Cyc_Absyn_Exp*_tmp2CA=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2CA;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2CB=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2CB;
struct Cyc_Absyn_Exp*_tmp2CC=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2CC;
struct Cyc_Absyn_Exp*_tmp2CD=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2CD;
struct Cyc_Absyn_Stmt*_tmp2CE=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2CE;
struct Cyc_Absyn_Stmt*_tmp2CF=({struct Cyc_Absyn_Exp*_tmp8CB=test_exp;struct Cyc_Absyn_Stmt*_tmp8CA=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8CB,_tmp8CA,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2CF;
void*_tmp2D0=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2D0;
struct Cyc_Absyn_Exp*_tmp2D1=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2D1;
struct Cyc_Absyn_Stmt*_tmp2D2=({struct _tuple1*_tmp8CE=temp;void*_tmp8CD=e1_p_type;struct Cyc_Absyn_Exp*_tmp8CC=e1_f;Cyc_Absyn_declare_stmt(_tmp8CE,_tmp8CD,_tmp8CC,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D2;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1633
struct Cyc_Absyn_Exp*_tmp2D3=({struct Cyc_Absyn_Exp*_tmp8CF=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8CF,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2D3;
struct Cyc_Absyn_Exp*_tmp2D4=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2D4;
struct Cyc_Absyn_Exp*_tmp2D5=({struct Cyc_Absyn_Exp*_tmp8D0=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8D0,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2D5;
struct Cyc_Absyn_Stmt*_tmp2D6=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2D6;
struct Cyc_Absyn_Stmt*_tmp2D7=({struct Cyc_Absyn_Exp*_tmp8D2=test_exp;struct Cyc_Absyn_Stmt*_tmp8D1=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8D2,_tmp8D1,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D7;
struct Cyc_Absyn_Stmt*_tmp2D8=({struct _tuple1*_tmp8D5=temp;void*_tmp8D4=e1_c_type;struct Cyc_Absyn_Exp*_tmp8D3=e1;Cyc_Absyn_declare_stmt(_tmp8D5,_tmp8D4,_tmp8D3,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D8;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1643
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1646
void*_tmp2D9=Cyc_Tcutil_compress(t);void*_stmttmp2A=_tmp2D9;void*_tmp2DA=_stmttmp2A;union Cyc_Absyn_AggrInfo _tmp2DB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->tag == 20U){_LL1: _tmp2DB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DA)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp2DB;
# 1648
struct Cyc_Absyn_Aggrdecl*_tmp2DC=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2DC;
({int _tmp8D6=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp8D6;});{
# 1651
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp7C9;_tmp7C9.tag=0U,_tmp7C9.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7C9;});struct Cyc_String_pa_PrintArg_struct _tmp2E1=({struct Cyc_String_pa_PrintArg_struct _tmp7C8;_tmp7C8.tag=0U,_tmp7C8.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7C8;});void*_tmp2DE[2U];_tmp2DE[0]=& _tmp2E0,_tmp2DE[1]=& _tmp2E1;({struct _fat_ptr _tmp8D7=({const char*_tmp2DF="_union_%s_%s";_tag_fat(_tmp2DF,sizeof(char),13U);});Cyc_aprintf(_tmp8D7,_tag_fat(_tmp2DE,sizeof(void*),2U));});});
({void*_tmp8D8=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));*_tmp2DD=str;_tmp2DD;}));*tagged_member_type=_tmp8D8;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1662
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1665
void*_tmp2E2=e->r;void*_stmttmp2B=_tmp2E2;void*_tmp2E3=_stmttmp2B;int*_tmp2E6;struct _fat_ptr*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E4;int*_tmp2E9;struct _fat_ptr*_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2EA;switch(*((int*)_tmp2E3)){case 14U: _LL1: _tmp2EA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp2EA;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CA;_tmp7CA.tag=0U,({struct _fat_ptr _tmp8D9=({const char*_tmp2ED="cast on lhs!";_tag_fat(_tmp2ED,sizeof(char),13U);});_tmp7CA.f1=_tmp8D9;});_tmp7CA;});void*_tmp2EB[1U];_tmp2EB[0]=& _tmp2EC;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp2EB,sizeof(void*),1U));});}case 21U: _LL3: _tmp2E7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f1;_tmp2E8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_tmp2E9=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2E3)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2E7;struct _fat_ptr*f=_tmp2E8;int*is_read=_tmp2E9;
# 1668
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22U: _LL5: _tmp2E4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f1;_tmp2E5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f2;_tmp2E6=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2E3)->f4;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp2E4;struct _fat_ptr*f=_tmp2E5;int*is_read=_tmp2E6;
# 1671
void*_tmp2EE=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp2C=_tmp2EE;void*_tmp2EF=_stmttmp2C;void*_tmp2F0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF)->tag == 3U){_LLA: _tmp2F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EF)->f1).elt_type;_LLB: {void*et=_tmp2F0;
# 1673
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{_LLC: _LLD:
# 1675
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1677
 return 0;}_LL0:;}
# 1690 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1694
struct _tuple30 _tmp2F1=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2D=_tmp2F1;struct _tuple30 _tmp2F2=_stmttmp2D;struct Cyc_Absyn_Exp*_tmp2F4;struct _tuple1*_tmp2F3;_LL1: _tmp2F3=_tmp2F2.f1;_tmp2F4=_tmp2F2.f2;_LL2: {struct _tuple1*temp=_tmp2F3;struct Cyc_Absyn_Exp*temp_exp=_tmp2F4;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1703
struct Cyc_Absyn_Exp*_tmp2F5=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2F5;
s2=({struct Cyc_Absyn_Exp*_tmp8DB=test_exp;struct Cyc_Absyn_Stmt*_tmp8DA=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8DB,_tmp8DA,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1706
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8DE=temp;void*_tmp8DD=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8DC=
Cyc_Toc_push_address_exp(e1);
# 1706
Cyc_Absyn_declare_stmt(_tmp8DE,_tmp8DD,_tmp8DC,
# 1708
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1713
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CC;_tmp7CC.tag=0U,({struct _fat_ptr _tmp8DF=({const char*_tmp2F9="exp_to_c: no type for ";_tag_fat(_tmp2F9,sizeof(char),23U);});_tmp7CC.f1=_tmp8DF;});_tmp7CC;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2F8=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7CB;_tmp7CB.tag=4U,_tmp7CB.f1=e;_tmp7CB;});void*_tmp2F6[2U];_tmp2F6[0]=& _tmp2F7,_tmp2F6[1]=& _tmp2F8;({unsigned _tmp8E0=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8E0,_tag_fat(_tmp2F6,sizeof(void*),2U));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1718
int did_inserted_checks=0;
{void*_tmp2FA=e->annot;void*_stmttmp2E=_tmp2FA;void*_tmp2FB=_stmttmp2E;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2FB)->tag == Cyc_Absyn_EmptyAnnot){_LL1: _LL2:
 goto _LL4;}else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2FB)->tag == Cyc_InsertChecks_NoCheck){_LL3: _LL4:
 did_inserted_checks=1;goto _LL0;}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 1724
{void*_tmp2FC=e->r;void*_stmttmp2F=_tmp2FC;void*_tmp2FD=_stmttmp2F;struct Cyc_List_List*_tmp2FF;struct Cyc_List_List*_tmp2FE;struct Cyc_Absyn_Stmt*_tmp300;struct _fat_ptr*_tmp302;struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp303;int _tmp30A;int _tmp309;struct Cyc_Absyn_Exp*_tmp308;void**_tmp307;struct Cyc_Absyn_Exp*_tmp306;int _tmp305;struct Cyc_Absyn_Datatypefield*_tmp30D;struct Cyc_Absyn_Datatypedecl*_tmp30C;struct Cyc_List_List*_tmp30B;struct Cyc_Absyn_Aggrdecl*_tmp311;struct Cyc_List_List*_tmp310;struct Cyc_List_List*_tmp30F;struct _tuple1**_tmp30E;struct Cyc_List_List*_tmp313;void*_tmp312;int _tmp316;void*_tmp315;struct Cyc_Absyn_Exp*_tmp314;int _tmp31A;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Vardecl*_tmp317;struct Cyc_List_List*_tmp31B;struct Cyc_List_List*_tmp31C;struct Cyc_Absyn_Exp*_tmp31E;struct Cyc_Absyn_Exp*_tmp31D;struct Cyc_Absyn_Exp*_tmp31F;int _tmp323;int _tmp322;struct _fat_ptr*_tmp321;struct Cyc_Absyn_Exp*_tmp320;int _tmp327;int _tmp326;struct _fat_ptr*_tmp325;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_List_List*_tmp329;void*_tmp328;void*_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32C;struct Cyc_Absyn_Exp*_tmp32E;enum Cyc_Absyn_Coercion _tmp332;int _tmp331;struct Cyc_Absyn_Exp*_tmp330;void**_tmp32F;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_Absyn_Exp*_tmp335;int _tmp337;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_VarargInfo*_tmp33C;struct Cyc_List_List*_tmp33B;int _tmp33A;struct Cyc_List_List*_tmp339;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_List_List*_tmp33E;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Core_Opt*_tmp349;struct Cyc_Absyn_Exp*_tmp348;enum Cyc_Absyn_Incrementor _tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_List_List*_tmp34E;enum Cyc_Absyn_Primop _tmp34D;struct Cyc_Absyn_Exp*_tmp34F;switch(*((int*)_tmp2FD)){case 2U: _LL8: _LL9:
# 1726
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).Null_c).tag == 1){_LLA: _LLB: {
# 1733
struct Cyc_Absyn_Exp*_tmp350=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp350;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8E1=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp8E1;});else{
# 1738
({void*_tmp8E3=({struct Cyc_Absyn_Exp*_tmp351[3U];_tmp351[0]=zero,_tmp351[1]=zero,_tmp351[2]=zero;({struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8E2,_tag_fat(_tmp351,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8E3;});}}else{
# 1740
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1742
goto _LL7;}}else{_LLC: _LLD:
 goto _LL7;}case 1U: _LLE: _LLF:
 goto _LL7;case 42U: _LL10: _LL11:
({void*_tmp8E4=(Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U))->r;e->r=_tmp8E4;});goto _LL7;case 41U: _LL12: _tmp34F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL13: {struct Cyc_Absyn_Exp*e1=_tmp34F;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3U: _LL14: _tmp34D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp34E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL15: {enum Cyc_Absyn_Primop p=_tmp34D;struct Cyc_List_List*es=_tmp34E;
# 1749
struct Cyc_List_List*_tmp352=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp352;
# 1751
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp353=p;switch(_tmp353){case Cyc_Absyn_Numelts: _LL63: _LL64: {
# 1754
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp354=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_stmttmp30=_tmp354;void*_tmp355=_stmttmp30;void*_tmp359;void*_tmp358;void*_tmp357;void*_tmp356;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355)->tag == 3U){_LL78: _tmp356=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355)->f1).elt_type;_tmp357=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355)->f1).ptr_atts).nullable;_tmp358=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355)->f1).ptr_atts).bounds;_tmp359=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355)->f1).ptr_atts).zero_term;_LL79: {void*elt_type=_tmp356;void*nbl=_tmp357;void*bound=_tmp358;void*zt=_tmp359;
# 1757
struct Cyc_Absyn_Exp*_tmp35A=({void*_tmp8E5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8E5,bound);});struct Cyc_Absyn_Exp*eopt=_tmp35A;
if(eopt == 0)
# 1760
({void*_tmp8E8=({struct Cyc_Absyn_Exp*_tmp35B[2U];_tmp35B[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp35B[1]=_tmp8E6;});({struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp8E7,_tag_fat(_tmp35B,sizeof(struct Cyc_Absyn_Exp*),2U));});});
# 1760
e->r=_tmp8E8;});else{
# 1763
struct Cyc_Absyn_Exp*_tmp35C=eopt;struct Cyc_Absyn_Exp*e2=_tmp35C;
# 1765
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1769
({void*_tmp8EA=({struct Cyc_Absyn_Exp*_tmp35D[2U];_tmp35D[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp35D[1]=e2;({struct Cyc_Absyn_Exp*_tmp8E9=function_e;Cyc_Toc_fncall_exp_r(_tmp8E9,_tag_fat(_tmp35D,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp8EA;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(e2))
({void*_tmp35E=0U;({unsigned _tmp8EC=e->loc;struct _fat_ptr _tmp8EB=({const char*_tmp35F="can't calculate numelts";_tag_fat(_tmp35F,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8EC,_tmp8EB,_tag_fat(_tmp35E,sizeof(void*),0U));});});
# 1774
({void*_tmp8EF=({struct Cyc_Absyn_Exp*_tmp8EE=arg;struct Cyc_Absyn_Exp*_tmp8ED=e2;Cyc_Toc_conditional_exp_r(_tmp8EE,_tmp8ED,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8EF;});}else{
# 1776
e->r=e2->r;goto _LL77;}}}
# 1779
goto _LL77;}}else{_LL7A: _LL7B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp361=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CE;_tmp7CE.tag=0U,({struct _fat_ptr _tmp8F0=({const char*_tmp363="numelts primop applied to non-pointer ";_tag_fat(_tmp363,sizeof(char),39U);});_tmp7CE.f1=_tmp8F0;});_tmp7CE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp362=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7CD;_tmp7CD.tag=2U,_tmp7CD.f1=(void*)_check_null(arg->topt);_tmp7CD;});void*_tmp360[2U];_tmp360[0]=& _tmp361,_tmp360[1]=& _tmp362;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp360,sizeof(void*),2U));});}_LL77:;}
# 1783
goto _LL62;}case Cyc_Absyn_Plus: _LL65: _LL66:
# 1788
 if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp364="can't do pointer arithmetic at top-level";_tag_fat(_tmp364,sizeof(char),41U);}));
{void*_tmp365=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp31=_tmp365;void*_tmp366=_stmttmp31;void*_tmp369;void*_tmp368;void*_tmp367;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp366)->tag == 3U){_LL7D: _tmp367=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp366)->f1).elt_type;_tmp368=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp366)->f1).ptr_atts).bounds;_tmp369=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp366)->f1).ptr_atts).zero_term;_LL7E: {void*elt_type=_tmp367;void*b=_tmp368;void*zt=_tmp369;
# 1792
struct Cyc_Absyn_Exp*_tmp36A=({void*_tmp8F1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8F1,b);});struct Cyc_Absyn_Exp*eopt=_tmp36A;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp8F4=({struct Cyc_Absyn_Exp*_tmp36B[3U];_tmp36B[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36B[1]=_tmp8F2;}),_tmp36B[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8F3,_tag_fat(_tmp36B,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1796
e->r=_tmp8F4;});else{
# 1798
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp8F6=({struct Cyc_Absyn_Exp*_tmp36C[3U];_tmp36C[0]=e1,_tmp36C[1]=eopt,_tmp36C[2]=e2;({struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8F5,_tag_fat(_tmp36C,sizeof(struct Cyc_Absyn_Exp*),3U));});});e->r=_tmp8F6;});}
goto _LL7C;}}else{_LL7F: _LL80:
 goto _LL7C;}_LL7C:;}
# 1803
goto _LL62;case Cyc_Absyn_Minus: _LL67: _LL68: {
# 1808
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd)){
({void*_tmp8F8=({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8F7,Cyc_Toc_curr_sp);});e1->r=_tmp8F8;});
({void*_tmp8FA=({struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8F9,Cyc_Toc_curr_sp);});e2->r=_tmp8FA;});
({void*_tmp8FC=({void*_tmp8FB=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8FB;});e1->topt=_tmp8FC;});
({void*_tmp8FE=({struct Cyc_Absyn_Exp*_tmp8FD=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8FD,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1816
e->r=_tmp8FE;});}else{
# 1819
({void*_tmp902=({struct Cyc_Absyn_Exp*_tmp36D[3U];_tmp36D[0]=e1,({
struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp36D[1]=_tmp900;}),({
struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp36D[2]=_tmp8FF;});({struct Cyc_Absyn_Exp*_tmp901=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp901,_tag_fat(_tmp36D,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1819
e->r=_tmp902;});}}
# 1823
goto _LL62;}case Cyc_Absyn_Eq: _LL69: _LL6A:
 goto _LL6C;case Cyc_Absyn_Neq: _LL6B: _LL6C: goto _LL6E;case Cyc_Absyn_Gt: _LL6D: _LL6E: goto _LL70;case Cyc_Absyn_Gte: _LL6F: _LL70: goto _LL72;case Cyc_Absyn_Lt: _LL71: _LL72: goto _LL74;case Cyc_Absyn_Lte: _LL73: _LL74: {
# 1826
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(old_types))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp36E=({void*_tmp903=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp903,Cyc_Toc_mt_tq);});void*t=_tmp36E;
({void*_tmp906=({void*_tmp905=t;Cyc_Toc_cast_it_r(_tmp905,({struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp904,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp906;});
e1->topt=t;}
# 1836
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp36F=({void*_tmp907=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp907,Cyc_Toc_mt_tq);});void*t=_tmp36F;
({void*_tmp90A=({void*_tmp909=t;Cyc_Toc_cast_it_r(_tmp909,({struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp908,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp90A;});
e2->topt=t;}
# 1841
goto _LL62;}default: _LL75: _LL76:
 goto _LL62;}_LL62:;}
# 1844
goto _LL7;}case 5U: _LL16: _tmp34B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp34C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL17: {struct Cyc_Absyn_Exp*e2=_tmp34B;enum Cyc_Absyn_Incrementor incr=_tmp34C;
# 1846
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1855 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp38B="increment";_tag_fat(_tmp38B,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp370="decrement";_tag_fat(_tmp370,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp372=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D2;_tmp7D2.tag=0U,({struct _fat_ptr _tmp90B=({const char*_tmp378="in-place ";_tag_fat(_tmp378,sizeof(char),10U);});_tmp7D2.f1=_tmp90B;});_tmp7D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp373=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D1;_tmp7D1.tag=0U,_tmp7D1.f1=incr_str;_tmp7D1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp374=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D0;_tmp7D0.tag=0U,({struct _fat_ptr _tmp90C=({const char*_tmp377=" is not supported when ";_tag_fat(_tmp377,sizeof(char),24U);});_tmp7D0.f1=_tmp90C;});_tmp7D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CF;_tmp7CF.tag=0U,({
struct _fat_ptr _tmp90D=({const char*_tmp376="dereferencing a zero-terminated pointer";_tag_fat(_tmp376,sizeof(char),40U);});_tmp7CF.f1=_tmp90D;});_tmp7CF;});void*_tmp371[4U];_tmp371[0]=& _tmp372,_tmp371[1]=& _tmp373,_tmp371[2]=& _tmp374,_tmp371[3]=& _tmp375;({unsigned _tmp90E=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp90E,_tag_fat(_tmp371,sizeof(void*),4U));});});
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp379=incr;switch(_tmp379){case Cyc_Absyn_PreInc: _LL82: _LL83:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec: _LL84: _LL85:
 op=2U;goto _LL81;default: _LL86: _LL87:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp37B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D5;_tmp7D5.tag=0U,({struct _fat_ptr _tmp90F=({const char*_tmp37F="in-place post";_tag_fat(_tmp37F,sizeof(char),14U);});_tmp7D5.f1=_tmp90F;});_tmp7D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D4;_tmp7D4.tag=0U,_tmp7D4.f1=incr_str;_tmp7D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp37D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D3;_tmp7D3.tag=0U,({
struct _fat_ptr _tmp910=({const char*_tmp37E=" is not supported on @tagged union members";_tag_fat(_tmp37E,sizeof(char),43U);});_tmp7D3.f1=_tmp910;});_tmp7D3;});void*_tmp37A[3U];_tmp37A[0]=& _tmp37B,_tmp37A[1]=& _tmp37C,_tmp37A[2]=& _tmp37D;({unsigned _tmp911=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp911,_tag_fat(_tmp37A,sizeof(void*),3U));});});}_LL81:;}
# 1873
({void*_tmp913=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381->tag=4U,_tmp381->f1=e2,({struct Cyc_Core_Opt*_tmp912=({struct Cyc_Core_Opt*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->v=(void*)op;_tmp380;});_tmp381->f2=_tmp912;}),_tmp381->f3=one;_tmp381;});e->r=_tmp913;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1877
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1882
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=((int)incr == (int)1U ||(int)incr == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1888
if((int)incr == (int)2U ||(int)incr == (int)3U)
change=-1;
({void*_tmp918=({struct Cyc_Absyn_Exp*_tmp382[3U];({struct Cyc_Absyn_Exp*_tmp916=Cyc_Toc_push_address_exp(e2);_tmp382[0]=_tmp916;}),({
struct Cyc_Absyn_Exp*_tmp915=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp382[1]=_tmp915;}),({
struct Cyc_Absyn_Exp*_tmp914=Cyc_Absyn_signed_int_exp(change,0U);_tmp382[2]=_tmp914;});({struct Cyc_Absyn_Exp*_tmp917=fn_e;Cyc_Toc_fncall_exp_r(_tmp917,_tag_fat(_tmp382,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1890
e->r=_tmp918;});}else{
# 1893
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1899
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp383=Cyc_Toc_temp_var();struct _tuple1*x=_tmp383;
void*_tmp384=({void*_tmp919=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp919,Cyc_Toc_mt_tq);});void*t=_tmp384;
struct Cyc_Absyn_Exp*_tmp385=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp385;
struct Cyc_Absyn_Exp*_tmp386=({struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp91A,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1904
struct Cyc_Absyn_Exp*testexp=_tmp386;
# 1906
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp91C=({struct Cyc_Absyn_Exp*_tmp91B=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp91B,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1907
Cyc_Absyn_and_exp(_tmp91C,testexp,0U);});{
# 1909
struct Cyc_Absyn_Stmt*_tmp387=({struct Cyc_Absyn_Exp*_tmp91F=testexp;struct Cyc_Absyn_Stmt*_tmp91E=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1909
Cyc_Absyn_ifthenelse_stmt(_tmp91F,_tmp91E,
# 1911
Cyc_Absyn_exp_stmt(({void*_tmp388=0U;({struct Cyc_Absyn_Exp*_tmp91D=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp91D,_tag_fat(_tmp388,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 1909
struct Cyc_Absyn_Stmt*s=_tmp387;
# 1912
s=({struct Cyc_Absyn_Stmt*_tmp920=s;Cyc_Absyn_seq_stmt(_tmp920,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp921=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp921;});}}else{
# 1916
struct Cyc_Toc_functionSet*_tmp389=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp389;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp925=({struct Cyc_Absyn_Exp*_tmp38A[2U];({struct Cyc_Absyn_Exp*_tmp923=Cyc_Toc_push_address_exp(e2);_tmp38A[0]=_tmp923;}),({struct Cyc_Absyn_Exp*_tmp922=Cyc_Absyn_signed_int_exp(1,0U);_tmp38A[1]=_tmp922;});({struct Cyc_Absyn_Exp*_tmp924=fn_e;Cyc_Toc_fncall_exp_r(_tmp924,_tag_fat(_tmp38A,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp925;});}}else{
# 1920
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1924
goto _LL7;}}case 4U: _LL18: _tmp348=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp349=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp34A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_LL19: {struct Cyc_Absyn_Exp*e1=_tmp348;struct Cyc_Core_Opt*popt=_tmp349;struct Cyc_Absyn_Exp*e2=_tmp34A;
# 1943 "toc.cyc"
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp926=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp926;});
# 1954
goto _LL7;}{
# 1956
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
# 1962
return;}{
# 1966
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1972
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp38C=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp32=_tmp38C;enum Cyc_Absyn_Primop _tmp38D=_stmttmp32;switch(_tmp38D){case Cyc_Absyn_Plus: _LL89: _LL8A:
 change=e2;goto _LL88;case Cyc_Absyn_Minus: _LL8B: _LL8C:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL88;default: _LL8D: _LL8E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D6;_tmp7D6.tag=0U,({struct _fat_ptr _tmp927=({const char*_tmp390="bad t ? pointer arithmetic";_tag_fat(_tmp390,sizeof(char),27U);});_tmp7D6.f1=_tmp927;});_tmp7D6;});void*_tmp38E[1U];_tmp38E[0]=& _tmp38F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp38E,sizeof(void*),1U));});}_LL88:;}
# 1982
done=1;{
# 1984
struct Cyc_Absyn_Exp*_tmp391=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp391;
({void*_tmp92B=({struct Cyc_Absyn_Exp*_tmp392[3U];({struct Cyc_Absyn_Exp*_tmp929=Cyc_Toc_push_address_exp(e1);_tmp392[0]=_tmp929;}),({
struct Cyc_Absyn_Exp*_tmp928=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp392[1]=_tmp928;}),_tmp392[2]=change;({struct Cyc_Absyn_Exp*_tmp92A=fn_e;Cyc_Toc_fncall_exp_r(_tmp92A,_tag_fat(_tmp392,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 1985
e->r=_tmp92B;});}}else{
# 1988
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1991
enum Cyc_Absyn_Primop _tmp393=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp33=_tmp393;enum Cyc_Absyn_Primop _tmp394=_stmttmp33;if(_tmp394 == Cyc_Absyn_Plus){_LL90: _LL91:
# 1993
 done=1;
({void*_tmp92D=({struct Cyc_Absyn_Exp*_tmp395[2U];_tmp395[0]=e1,_tmp395[1]=e2;({struct Cyc_Absyn_Exp*_tmp92C=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp92C,_tag_fat(_tmp395,sizeof(struct Cyc_Absyn_Exp*),2U));});});e->r=_tmp92D;});
goto _LL8F;}else{_LL92: _LL93:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp397=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D7;_tmp7D7.tag=0U,({struct _fat_ptr _tmp92E=({const char*_tmp398="bad zero-terminated pointer arithmetic";_tag_fat(_tmp398,sizeof(char),39U);});_tmp7D7.f1=_tmp92E;});_tmp7D7;});void*_tmp396[1U];_tmp396[0]=& _tmp397;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp396,sizeof(void*),1U));});}_LL8F:;}}}
# 2000
if(!done){
# 2002
if(e1_poly)
({void*_tmp930=({void*_tmp92F=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp92F,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp930;});
# 2008
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp931=e1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*f_env))Cyc_Toc_lvalue_assign)(_tmp931,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->f1=popt,_tmp399->f2=e2;_tmp399;}));});
e->r=e1->r;}}
# 2013
goto _LL7;}}}}case 6U: _LL1A: _tmp345=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp346=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp347=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_LL1B: {struct Cyc_Absyn_Exp*e1=_tmp345;struct Cyc_Absyn_Exp*e2=_tmp346;struct Cyc_Absyn_Exp*e3=_tmp347;
# 2015
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7U: _LL1C: _tmp343=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp344=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL1D: {struct Cyc_Absyn_Exp*e1=_tmp343;struct Cyc_Absyn_Exp*e2=_tmp344;
_tmp341=e1;_tmp342=e2;goto _LL1F;}case 8U: _LL1E: _tmp341=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp342=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp341;struct Cyc_Absyn_Exp*e2=_tmp342;
_tmp33F=e1;_tmp340=e2;goto _LL21;}case 9U: _LL20: _tmp33F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp340=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp33F;struct Cyc_Absyn_Exp*e2=_tmp340;
# 2022
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3 == 0){_LL22: _tmp33D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp33E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL23: {struct Cyc_Absyn_Exp*e1=_tmp33D;struct Cyc_List_List*es=_tmp33E;
# 2026
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2031
({void*_tmp935=({void*_tmp934=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp934,({struct Cyc_Absyn_Exp*_tmp39A[1U];({struct Cyc_Absyn_Exp*_tmp932=
Cyc_Absyn_copy_exp(e1);_tmp39A[0]=_tmp932;});({struct Cyc_Absyn_Exp*_tmp933=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp933,_tag_fat(_tmp39A,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2031
e1->r=_tmp935;});
# 2033
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_LL24: _tmp338=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp339=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp33A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3)->num_varargs;_tmp33B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3)->injectors;_tmp33C=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3)->vai;_LL25: {struct Cyc_Absyn_Exp*e1=_tmp338;struct Cyc_List_List*es=_tmp339;int num_varargs=_tmp33A;struct Cyc_List_List*injectors=_tmp33B;struct Cyc_Absyn_VarargInfo*vai=_tmp33C;
# 2044 "toc.cyc"
struct _RegionHandle _tmp39B=_new_region("r");struct _RegionHandle*r=& _tmp39B;_push_region(r);{
struct _tuple30 _tmp39C=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp34=_tmp39C;struct _tuple30 _tmp39D=_stmttmp34;struct Cyc_Absyn_Exp*_tmp39F;struct _tuple1*_tmp39E;_LL95: _tmp39E=_tmp39D.f1;_tmp39F=_tmp39D.f2;_LL96: {struct _tuple1*argv=_tmp39E;struct Cyc_Absyn_Exp*argvexp=_tmp39F;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned)num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2051
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);
int num_normargs=num_args - num_varargs;
# 2055
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3A0->tl=new_args;_tmp3A0;});}}
# 2060
new_args=({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct Cyc_Absyn_Exp*_tmp938=({struct Cyc_Absyn_Exp*_tmp3A1[3U];_tmp3A1[0]=argvexp,({
# 2062
struct Cyc_Absyn_Exp*_tmp936=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp3A1[1]=_tmp936;}),_tmp3A1[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp937=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp937,_tag_fat(_tmp3A1,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2060
_tmp3A2->hd=_tmp938;}),_tmp3A2->tl=new_args;_tmp3A2;});
# 2065
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);{
# 2067
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2072
({void*_tmp93C=({void*_tmp93B=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp93B,({struct Cyc_Absyn_Exp*_tmp3A3[1U];({struct Cyc_Absyn_Exp*_tmp939=
Cyc_Absyn_copy_exp(e1);_tmp3A3[0]=_tmp939;});({struct Cyc_Absyn_Exp*_tmp93A=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp93A,_tag_fat(_tmp3A3,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});
# 2072
e1->r=_tmp93C;});{
# 2074
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2077
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp3A4=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp35=_tmp3A4;void*_tmp3A5=_stmttmp35;struct Cyc_Absyn_Datatypedecl*_tmp3A6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->f1)->f1).KnownDatatype).tag == 2){_LL98: _tmp3A6=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->f1)->f1).KnownDatatype).val;_LL99: {struct Cyc_Absyn_Datatypedecl*x=_tmp3A6;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A: _LL9B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D8;_tmp7D8.tag=0U,({struct _fat_ptr _tmp93D=({const char*_tmp3A9="unknown datatype in vararg with inject";_tag_fat(_tmp3A9,sizeof(char),39U);});_tmp7D8.f1=_tmp93D;});_tmp7D8;});void*_tmp3A7[1U];_tmp3A7[0]=& _tmp3A8;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3A7,sizeof(void*),1U));});}_LL97:;}{
# 2083
struct _fat_ptr vs=({unsigned _tmp3B5=(unsigned)num_varargs;struct _tuple1**_tmp3B4=({struct _RegionHandle*_tmp93E=r;_region_malloc(_tmp93E,_check_times(_tmp3B5,sizeof(struct _tuple1*)));});({{unsigned _tmp7D9=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp7D9;++ i){({struct _tuple1*_tmp93F=Cyc_Toc_temp_var();_tmp3B4[i]=_tmp93F;});}}0;});_tag_fat(_tmp3B4,sizeof(struct _tuple1*),_tmp3B5);});
# 2085
if(num_varargs != 0){
# 2089
struct Cyc_List_List*_tmp3AA=0;struct Cyc_List_List*array_args=_tmp3AA;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({struct Cyc_Absyn_Exp*_tmp940=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp3AB->hd=_tmp940;}),_tmp3AB->tl=array_args;_tmp3AB;});}}
s=({struct _tuple1*_tmp943=argv;void*_tmp942=arr_type;struct Cyc_Absyn_Exp*_tmp941=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp943,_tmp942,_tmp941,s,0U);});
# 2094
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)- 1;
for(0;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp3AC=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp36=_tmp3AC;struct Cyc_Absyn_Datatypefield*_tmp3AD=_stmttmp36;struct Cyc_List_List*_tmp3AF;struct _tuple1*_tmp3AE;_LL9D: _tmp3AE=_tmp3AD->name;_tmp3AF=_tmp3AD->typs;_LL9E: {struct _tuple1*qv=_tmp3AE;struct Cyc_List_List*tqts=_tmp3AF;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple13*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2108
struct _tuple1*_tmp3B0=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp3B0;
struct Cyc_List_List*_tmp3B1=({struct _tuple20*_tmp3B3[2U];({struct _tuple20*_tmp947=({struct _fat_ptr*_tmp946=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp946,Cyc_Toc_datatype_tag(tud,qv));});_tmp3B3[0]=_tmp947;}),({
struct _tuple20*_tmp945=({struct _fat_ptr*_tmp944=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp944,arg);});_tmp3B3[1]=_tmp945;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3B3,sizeof(struct _tuple20*),2U));});
# 2109
struct Cyc_List_List*dles=_tmp3B1;
# 2111
s=({struct _tuple1*_tmp94A=var;void*_tmp949=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp948=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->tag=29U,_tmp3B2->f1=tdn,_tmp3B2->f2=0,_tmp3B2->f3=dles,_tmp3B2->f4=0;_tmp3B2;}),0U);
# 2111
Cyc_Absyn_declare_stmt(_tmp94A,_tmp949,_tmp948,s,0U);});}}}}}else{
# 2119
s=({struct _tuple1*_tmp94D=argv;void*_tmp94C=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp94B=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp94D,_tmp94C,_tmp94B,s,0U);});}}}else{
# 2124
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3B6->tl=array_args;_tmp3B6;});}{
# 2130
struct Cyc_Absyn_Exp*_tmp3B7=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp3B7;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2133
s=({struct _tuple1*_tmp950=argv;void*_tmp94F=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp94E=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp950,_tmp94F,_tmp94E,s,0U);});}}
# 2136
({void*_tmp951=Cyc_Toc_stmt_exp_r(s);e->r=_tmp951;});}}}}
# 2138
_npop_handler(0U);goto _LL7;
# 2044
;_pop_region();}}case 11U: _LL26: _tmp336=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp337=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp336;int b=_tmp337;
# 2141
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp954=({void*_tmp953=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp953,({struct Cyc_Absyn_Exp*_tmp3B8[1U];_tmp3B8[0]=e1;({struct Cyc_Absyn_Exp*_tmp952=fn_e;Cyc_Toc_fncall_exp_dl(_tmp952,_tag_fat(_tmp3B8,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);})->r;e->r=_tmp954;});
goto _LL7;}}case 12U: _LL28: _tmp335=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL29: {struct Cyc_Absyn_Exp*e1=_tmp335;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13U: _LL2A: _tmp333=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp334=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL2B: {struct Cyc_Absyn_Exp*e1=_tmp333;struct Cyc_List_List*ts=_tmp334;
# 2147
Cyc_Toc_exp_to_c(nv,e1);
# 2156 "toc.cyc"
for(0;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp3B9=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp3B9;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp3BA=Cyc_Tcutil_compress((void*)ts->hd);void*_stmttmp37=_tmp3BA;void*_tmp3BB=_stmttmp37;switch(*((int*)_tmp3BB)){case 2U: _LLA0: _LLA1:
 goto _LLA3;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BB)->f1)->tag == 18U){_LLA2: _LLA3:
 continue;}else{goto _LLA4;}default: _LLA4: _LLA5:
# 2163
({void*_tmp956=({void*_tmp955=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp955,e1,0U);})->r;e->r=_tmp956;});
goto _LL9F;}_LL9F:;}
# 2166
break;}}
# 2169
goto _LL7;}case 14U: _LL2C: _tmp32F=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp330=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp331=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_tmp332=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FD)->f4;_LL2D: {void**t=_tmp32F;struct Cyc_Absyn_Exp*e1=_tmp330;int user_inserted=_tmp331;enum Cyc_Absyn_Coercion coercion=_tmp332;
# 2171
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2178
{struct _tuple34 _tmp3BC=({struct _tuple34 _tmp7DB;({void*_tmp958=Cyc_Tcutil_compress(old_t2);_tmp7DB.f1=_tmp958;}),({void*_tmp957=Cyc_Tcutil_compress(new_typ);_tmp7DB.f2=_tmp957;});_tmp7DB;});struct _tuple34 _stmttmp38=_tmp3BC;struct _tuple34 _tmp3BD=_stmttmp38;struct Cyc_Absyn_PtrInfo _tmp3BE;struct Cyc_Absyn_PtrInfo _tmp3C0;struct Cyc_Absyn_PtrInfo _tmp3BF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BD.f1)->tag == 3U)switch(*((int*)_tmp3BD.f2)){case 3U: _LLA7: _tmp3BF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BD.f1)->f1;_tmp3C0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BD.f2)->f1;_LLA8: {struct Cyc_Absyn_PtrInfo p1=_tmp3BF;struct Cyc_Absyn_PtrInfo p2=_tmp3C0;
# 2180
struct Cyc_Absyn_Exp*_tmp3C1=({void*_tmp959=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp959,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp3C1;
struct Cyc_Absyn_Exp*_tmp3C2=({void*_tmp95A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp95A,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp3C2;
int _tmp3C3=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp3C3;
int _tmp3C4=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp3C4;
{struct _tuple35 _tmp3C5=({struct _tuple35 _tmp7DA;_tmp7DA.f1=b1,_tmp7DA.f2=b2;_tmp7DA;});struct _tuple35 _stmttmp39=_tmp3C5;struct _tuple35 _tmp3C6=_stmttmp39;if(_tmp3C6.f1 != 0){if(_tmp3C6.f2 != 0){_LLAE: _LLAF:
# 2187
 did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp95D=({void*_tmp95C=*t;Cyc_Toc_cast_it_r(_tmp95C,({struct Cyc_Absyn_Exp*_tmp3C7[1U];_tmp3C7[0]=e1;({struct Cyc_Absyn_Exp*_tmp95B=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp95B,_tag_fat(_tmp3C7,sizeof(struct Cyc_Absyn_Exp*),1U));});}));});e->r=_tmp95D;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2196
goto _LLAD;}else{_LLB0: _LLB1: {
# 2198
struct Cyc_Absyn_Exp*_tmp3C8=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp3C8;
struct _tuple14 _tmp3C9=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple14 _stmttmp3A=_tmp3C9;struct _tuple14 _tmp3CA=_stmttmp3A;int _tmp3CC;unsigned _tmp3CB;_LLB7: _tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_LLB8: {unsigned i=_tmp3CB;int valid=_tmp3CC;
if(Cyc_Toc_is_toplevel(nv)){
# 2204
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp95E=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp95E,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp95F=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp95F;});}else{
# 2209
if(zt1){
# 2214
struct _tuple30 _tmp3CD=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3B=_tmp3CD;struct _tuple30 _tmp3CE=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3D0;struct _tuple1*_tmp3CF;_LLBA: _tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;_LLBB: {struct _tuple1*x=_tmp3CF;struct Cyc_Absyn_Exp*x_exp=_tmp3D0;
struct Cyc_Absyn_Exp*arg3;
# 2218
{void*_tmp3D1=e1->r;void*_stmttmp3C=_tmp3D1;void*_tmp3D2=_stmttmp3C;struct Cyc_Absyn_Vardecl*_tmp3D3;struct Cyc_Absyn_Vardecl*_tmp3D4;switch(*((int*)_tmp3D2)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1).Wstring_c).tag){case 8U: _LLBD: _LLBE:
 arg3=e2;goto _LLBC;case 9U: _LLBF: _LLC0:
 arg3=e2;goto _LLBC;default: goto _LLC5;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1)){case 1U: _LLC1: _tmp3D4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_tmp3D4;
_tmp3D3=vd;goto _LLC4;}case 4U: _LLC3: _tmp3D3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1)->f1;_LLC4: {struct Cyc_Absyn_Vardecl*vd=_tmp3D3;
# 2223
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC6;
arg3=e2;
goto _LLBC;}default: goto _LLC5;}default: _LLC5: _LLC6:
# 2230
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2233
arg3=({struct Cyc_Absyn_Exp*_tmp3D5[2U];({
struct Cyc_Absyn_Exp*_tmp961=({void*_tmp960=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp960,x_exp);});_tmp3D5[0]=_tmp961;}),_tmp3D5[1]=e2;({struct Cyc_Absyn_Exp*_tmp962=
# 2233
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp962,_tag_fat(_tmp3D5,sizeof(struct Cyc_Absyn_Exp*),2U));});});}
# 2235
goto _LLBC;}_LLBC:;}
# 2237
if(!zt2 && !(p2.elt_tq).real_const)
# 2240
arg3=({struct Cyc_Absyn_Exp*_tmp963=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp963,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2242
struct Cyc_Absyn_Exp*_tmp3D6=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3D6;
struct Cyc_Absyn_Exp*_tmp3D7=({struct Cyc_Absyn_Exp*_tmp3D9[3U];_tmp3D9[0]=x_exp,_tmp3D9[1]=arg2,_tmp3D9[2]=arg3;({struct Cyc_Absyn_Exp*_tmp964=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp964,_tag_fat(_tmp3D9,sizeof(struct Cyc_Absyn_Exp*),3U));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3D7;
struct Cyc_Absyn_Stmt*_tmp3D8=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3D8;
s=({struct _tuple1*_tmp967=x;void*_tmp966=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp965=e1;Cyc_Absyn_declare_stmt(_tmp967,_tmp966,_tmp965,s,0U);});
({void*_tmp968=Cyc_Toc_stmt_exp_r(s);e->r=_tmp968;});}}}else{
# 2249
({void*_tmp96B=({struct Cyc_Absyn_Exp*_tmp3DA[3U];_tmp3DA[0]=e1,({
# 2251
struct Cyc_Absyn_Exp*_tmp969=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3DA[1]=_tmp969;}),_tmp3DA[2]=e2;({struct Cyc_Absyn_Exp*_tmp96A=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp96A,_tag_fat(_tmp3DA,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2249
e->r=_tmp96B;});}}
# 2254
goto _LLAD;}}}}else{if(_tmp3C6.f2 != 0){_LLB2: _LLB3: {
# 2264 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3DB=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3DB;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp96C=b2;Cyc_Absyn_add_exp(_tmp96C,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2271
struct Cyc_Absyn_Exp*_tmp3DC=({struct Cyc_Absyn_Exp*_tmp3DE[3U];_tmp3DE[0]=e1,({
# 2273
struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3DE[1]=_tmp96D;}),_tmp3DE[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp96E=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp96E,_tag_fat(_tmp3DE,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2271
struct Cyc_Absyn_Exp*ptr_exp=_tmp3DC;
# 2275
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp971=({struct Cyc_Absyn_Exp*_tmp3DD[1U];({struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_copy_exp(ptr_exp);_tmp3DD[0]=_tmp96F;});({struct Cyc_Absyn_Exp*_tmp970=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp970,_tag_fat(_tmp3DD,sizeof(struct Cyc_Absyn_Exp*),1U));});});ptr_exp->r=_tmp971;});
({void*_tmp972=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp972;});
goto _LLAD;}}}else{_LLB4: _LLB5:
# 2283
 if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr str))Cyc_Toc_unimp)(({const char*_tmp3DF="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3DF,sizeof(char),70U);}));
({void*_tmp976=({struct Cyc_Absyn_Exp*_tmp3E0[3U];_tmp3E0[0]=e1,({
struct Cyc_Absyn_Exp*_tmp974=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3E0[1]=_tmp974;}),({
struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_uint_exp(1U,0U);_tmp3E0[2]=_tmp973;});({struct Cyc_Absyn_Exp*_tmp975=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp975,_tag_fat(_tmp3E0,sizeof(struct Cyc_Absyn_Exp*),3U));});});
# 2286
e->r=_tmp976;});}
# 2290
goto _LLAD;}}_LLAD:;}
# 2292
goto _LLA6;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BD.f2)->f1)->tag == 1U){_LLA9: _tmp3BE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BD.f1)->f1;_LLAA: {struct Cyc_Absyn_PtrInfo p1=_tmp3BE;
# 2294
{struct Cyc_Absyn_Exp*_tmp3E1=({void*_tmp977=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp977,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3D=_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E2=_stmttmp3D;if(_tmp3E2 == 0){_LLC8: _LLC9:
# 2296
({void*_tmp979=({struct Cyc_Absyn_Exp*_tmp978=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp978,Cyc_Toc_curr_sp);});e1->r=_tmp979;});
e1->topt=new_typ_c;
goto _LLC7;}else{_LLCA: _LLCB:
 goto _LLC7;}_LLC7:;}
# 2301
goto _LLA6;}}else{goto _LLAB;}default: goto _LLAB;}else{_LLAB: _LLAC:
# 2303
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLA6;}_LLA6:;}
# 2307
goto _LL7;}case 15U: _LL2E: _tmp32E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL2F: {struct Cyc_Absyn_Exp*e1=_tmp32E;
# 2310
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2316
({void*_tmp97B=({void*_tmp97A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp97A,e1);});e->r=_tmp97B;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2320
({void*_tmp97D=({void*_tmp97C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp97C,e1);});e->r=_tmp97D;});}
# 2322
goto _LL7;}case 16U: _LL30: _tmp32C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp32D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL31: {struct Cyc_Absyn_Exp*rgnopt=_tmp32C;struct Cyc_Absyn_Exp*e1=_tmp32D;
# 2329
Cyc_Toc_exp_to_c(nv,e1);{
# 2331
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple30 _tmp3E3=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3E=_tmp3E3;struct _tuple30 _tmp3E4=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp3E6;struct _tuple1*_tmp3E5;_LLCD: _tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;_LLCE: {struct _tuple1*x=_tmp3E5;struct Cyc_Absyn_Exp*xexp=_tmp3E6;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3E7=Cyc_Tcutil_compress(elt_typ);void*_stmttmp3F=_tmp3E7;void*_tmp3E8=_stmttmp3F;void*_tmp3E9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8)->tag == 4U){_LLD0: _tmp3E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8)->f1).elt_type;_LLD1: {void*t2=_tmp3E9;
# 2336
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCF;}}else{_LLD2: _LLD3:
# 2340
 lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCF;}_LLCF:;}{
# 2343
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3EA=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3EA;
struct Cyc_Absyn_Exp*_tmp3EB=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3EB;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3EC[2U];_tmp3EC[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp97E=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3EC[1]=_tmp97E;});({struct Cyc_Absyn_Exp*_tmp97F=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp97F,_tag_fat(_tmp3EC,sizeof(struct Cyc_Absyn_Exp*),2U));});});else{
# 2350
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2353
struct Cyc_Absyn_Exp*_tmp3ED=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3ED;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2357
if(rgnopt == 0 || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2360
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2363
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3EE[3U];({
struct Cyc_Absyn_Exp*_tmp982=Cyc_Absyn_copy_exp(xexp);_tmp3EE[0]=_tmp982;}),({
struct Cyc_Absyn_Exp*_tmp981=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3EE[1]=_tmp981;}),
(unsigned)lenexp?_tmp3EE[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_uint_exp(1U,0U);_tmp3EE[2]=_tmp980;});({struct Cyc_Absyn_Exp*_tmp983=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp983,_tag_fat(_tmp3EE,sizeof(struct Cyc_Absyn_Exp*),3U));});});else{
# 2370
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp984=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF->tag=Cyc_Toc_Dest,_tmp3EF->f1=xexp;_tmp3EF;});e->annot=_tmp984;});{
struct Cyc_Absyn_Stmt*_tmp3F0=({struct _tuple1*_tmp988=x;void*_tmp987=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp986=mexp;Cyc_Absyn_declare_stmt(_tmp988,_tmp987,_tmp986,({
struct Cyc_Absyn_Stmt*_tmp985=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp985,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2372
struct Cyc_Absyn_Stmt*s=_tmp3F0;
# 2376
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp989=Cyc_Toc_stmt_exp_r(s);e->r=_tmp989;});
if(vse != 0)
({void*_tmp98B=({void*_tmp98A=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp98A,Cyc_Absyn_copy_exp(e));});e->r=_tmp98B;});
goto _LL7;}}}}}}}case 18U: _LL32: _tmp32B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL33: {struct Cyc_Absyn_Exp*e1=_tmp32B;
# 2384
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17U: _LL34: _tmp32A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL35: {void*t=_tmp32A;
({void*_tmp98D=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->tag=17U,({void*_tmp98C=Cyc_Toc_typ_to_c(t);_tmp3F1->f1=_tmp98C;});_tmp3F1;});e->r=_tmp98D;});goto _LL7;}case 19U: _LL36: _tmp328=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp329=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL37: {void*t=_tmp328;struct Cyc_List_List*fs=_tmp329;
# 2387
void*_tmp3F2=t;void*t2=_tmp3F2;
struct Cyc_List_List*_tmp3F3=fs;struct Cyc_List_List*l=_tmp3F3;
for(0;l != 0;l=l->tl){
void*_tmp3F4=(void*)l->hd;void*_stmttmp40=_tmp3F4;void*_tmp3F5=_stmttmp40;unsigned _tmp3F6;struct _fat_ptr*_tmp3F7;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F5)->tag == 0U){_LLD5: _tmp3F7=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F5)->f1;_LLD6: {struct _fat_ptr*n=_tmp3F7;
goto _LLD4;}}else{_LLD7: _tmp3F6=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3F5)->f1;_LLD8: {unsigned n=_tmp3F6;
# 2393
{void*_tmp3F8=Cyc_Tcutil_compress(t2);void*_stmttmp41=_tmp3F8;void*_tmp3F9=_stmttmp41;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FB;struct Cyc_Absyn_Datatypefield*_tmp3FC;union Cyc_Absyn_AggrInfo _tmp3FD;switch(*((int*)_tmp3F9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->f1)){case 20U: _LLDA: _tmp3FD=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->f1)->f1;_LLDB: {union Cyc_Absyn_AggrInfo info=_tmp3FD;
# 2395
struct Cyc_Absyn_Aggrdecl*_tmp3FE=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp3FE;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp400=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DC;_tmp7DC.tag=0U,({struct _fat_ptr _tmp98E=({const char*_tmp401="struct fields must be known";_tag_fat(_tmp401,sizeof(char),28U);});_tmp7DC.f1=_tmp98E;});_tmp7DC;});void*_tmp3FF[1U];_tmp3FF[0]=& _tmp400;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp3FF,sizeof(void*),1U));});
_tmp3FB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->f1)->f1).KnownDatatypefield).tag == 2){_LLE0: _tmp3FC=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F9)->f1)->f1).KnownDatatypefield).val).f2;_LLE1: {struct Cyc_Absyn_Datatypefield*tuf=_tmp3FC;
# 2409
if(n == (unsigned)0)
({void*_tmp98F=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->tag=0U,_tmp405->f1=Cyc_Toc_tag_sp;_tmp405;}));l->hd=_tmp98F;});else{
# 2412
t2=(*((struct _tuple13*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp991=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=0U,({struct _fat_ptr*_tmp990=Cyc_Absyn_fieldname((int)n);_tmp406->f1=_tmp990;});_tmp406;}));l->hd=_tmp991;});}
# 2415
goto _LLD9;}}else{goto _LLE2;}default: goto _LLE2;}case 7U: _LLDC: _tmp3FB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F9)->f2;_LLDD: {struct Cyc_List_List*fields=_tmp3FB;
# 2400
struct Cyc_Absyn_Aggrfield*_tmp402=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp402;
({void*_tmp992=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->tag=0U,_tmp403->f1=nth_field->name;_tmp403;}));l->hd=_tmp992;});
t2=nth_field->type;
goto _LLD9;}case 6U: _LLDE: _tmp3FA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F9)->f1;_LLDF: {struct Cyc_List_List*ts=_tmp3FA;
# 2405
({void*_tmp994=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404->tag=0U,({struct _fat_ptr*_tmp993=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp404->f1=_tmp993;});_tmp404;}));l->hd=_tmp994;});
t2=(*((struct _tuple13*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD9;}default: _LLE2: _LLE3:
# 2416
({struct Cyc_Warn_String_Warn_Warg_struct _tmp408=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DD;_tmp7DD.tag=0U,({struct _fat_ptr _tmp995=({const char*_tmp409="impossible type for offsetof tuple index";_tag_fat(_tmp409,sizeof(char),41U);});_tmp7DD.f1=_tmp995;});_tmp7DD;});void*_tmp407[1U];_tmp407[0]=& _tmp408;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp407,sizeof(void*),1U));});}_LLD9:;}
# 2418
goto _LLD4;}}_LLD4:;}
# 2421
({void*_tmp997=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->tag=19U,({void*_tmp996=Cyc_Toc_typ_to_c(t);_tmp40A->f1=_tmp996;}),_tmp40A->f2=fs;_tmp40A;});e->r=_tmp997;});
goto _LL7;}case 21U: _LL38: _tmp324=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp325=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp326=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_tmp327=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FD)->f4;_LL39: {struct Cyc_Absyn_Exp*e1=_tmp324;struct _fat_ptr*f=_tmp325;int is_tagged=_tmp326;int is_read=_tmp327;
# 2424
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp99C=({struct Cyc_Absyn_Exp*_tmp99B=e1;void*_tmp99A=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp999=e1_cyc_type;struct _fat_ptr*_tmp998=f;Cyc_Toc_check_tagged_union(_tmp99B,_tmp99A,_tmp999,_tmp998,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2428
e->r=_tmp99C;});
# 2430
if(is_poly)
({void*_tmp99E=({void*_tmp99D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99D,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp99E;});
goto _LL7;}case 22U: _LL3A: _tmp320=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp321=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp322=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_tmp323=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FD)->f4;_LL3B: {struct Cyc_Absyn_Exp*e1=_tmp320;struct _fat_ptr*f=_tmp321;int is_tagged=_tmp322;int is_read=_tmp323;
# 2434
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp40B=Cyc_Tcutil_compress(e1typ);void*_stmttmp42=_tmp40B;void*_tmp40C=_stmttmp42;struct Cyc_Absyn_PtrInfo _tmp40D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->tag == 3U){_LLE5: _tmp40D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1;_LLE6: {struct Cyc_Absyn_PtrInfo p=_tmp40D;
ta=p.elt_type;goto _LLE4;}}else{_LLE7: _LLE8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DE;_tmp7DE.tag=0U,({struct _fat_ptr _tmp99F=({const char*_tmp410="get_ptr_typ: not a pointer!";_tag_fat(_tmp410,sizeof(char),28U);});_tmp7DE.f1=_tmp99F;});_tmp7DE;});void*_tmp40E[1U];_tmp40E[0]=& _tmp40F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp40E,sizeof(void*),1U));});}_LLE4:;}
# 2441
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp9A4=({struct Cyc_Absyn_Exp*_tmp9A3=e1;void*_tmp9A2=Cyc_Toc_typ_to_c(e1typ);void*_tmp9A1=ta;struct _fat_ptr*_tmp9A0=f;Cyc_Toc_check_tagged_union(_tmp9A3,_tmp9A2,_tmp9A1,_tmp9A0,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9A4;});
if(is_poly && is_read)
({void*_tmp9A6=({void*_tmp9A5=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9A5,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9A6;});
goto _LL7;}case 20U: _LL3C: _tmp31F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL3D: {struct Cyc_Absyn_Exp*e1=_tmp31F;
# 2449
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23U: _LL3E: _tmp31D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp31E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL3F: {struct Cyc_Absyn_Exp*e1=_tmp31D;struct Cyc_Absyn_Exp*e2=_tmp31E;
# 2453
{void*_tmp411=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp43=_tmp411;void*_tmp412=_stmttmp43;struct Cyc_List_List*_tmp413;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp412)->tag == 6U){_LLEA: _tmp413=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp412)->f1;_LLEB: {struct Cyc_List_List*ts=_tmp413;
# 2455
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp414=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp414;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple14 _tmp415=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple14 _stmttmp44=_tmp415;struct _tuple14 _tmp416=_stmttmp44;int _tmp418;unsigned _tmp417;_LLEF: _tmp417=_tmp416.f1;_tmp418=_tmp416.f2;_LLF0: {unsigned i=_tmp417;int known=_tmp418;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DF;_tmp7DF.tag=0U,({struct _fat_ptr _tmp9A7=({const char*_tmp41B="unknown tuple subscript in translation to C";_tag_fat(_tmp41B,sizeof(char),44U);});_tmp7DF.f1=_tmp9A7;});_tmp7DF;});void*_tmp419[1U];_tmp419[0]=& _tmp41A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp419,sizeof(void*),1U));});
({void*_tmp9A9=({struct Cyc_Absyn_Exp*_tmp9A8=e1;Cyc_Toc_aggrmember_exp_r(_tmp9A8,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp9A9;});
goto _LLE9;}}}}}else{_LLEC: _LLED:
# 2465
 did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp9AA=Cyc_Toc_deref_exp_r(e1);e->r=_tmp9AA;});
goto _LLE9;}}_LLE9:;}
# 2471
goto _LL7;}case 24U: _LL40: _tmp31C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL41: {struct Cyc_List_List*es=_tmp31C;
# 2473
if(!Cyc_Toc_is_toplevel(nv)){
# 2475
void*_tmp41C=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp41C;
{void*_tmp41D=strct_typ;union Cyc_Absyn_AggrInfo _tmp41E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->f2 == 0){_LLF2: _tmp41E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41D)->f1)->f1;_LLF3: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp41E;
# 2478
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp41F=_cycalloc(sizeof(*_tmp41F));({struct _tuple20*_tmp9AC=({struct _fat_ptr*_tmp9AB=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9AB,(struct Cyc_Absyn_Exp*)es->hd);});_tmp41F->hd=_tmp9AC;}),_tmp41F->tl=dles;_tmp41F;});}}
# 2484
({void*_tmp9AE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=29U,_tmp420->f1=sd->name,_tmp420->f2=0,({struct Cyc_List_List*_tmp9AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp420->f3=_tmp9AD;}),_tmp420->f4=sd;_tmp420;});e->r=_tmp9AE;});
e->topt=strct_typ;
goto _LLF1;}}else{goto _LLF4;}}else{goto _LLF4;}}else{_LLF4: _LLF5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp422=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E0;_tmp7E0.tag=0U,({struct _fat_ptr _tmp9AF=({const char*_tmp423="tuple type not an aggregate";_tag_fat(_tmp423,sizeof(char),28U);});_tmp7E0.f1=_tmp9AF;});_tmp7E0;});void*_tmp421[1U];_tmp421[0]=& _tmp422;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp421,sizeof(void*),1U));});}_LLF1:;}
# 2489
goto _LL7;}else{
# 2493
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp425=_cycalloc(sizeof(*_tmp425));({struct _tuple20*_tmp9B0=({struct _tuple20*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->f1=0,_tmp424->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp424;});_tmp425->hd=_tmp9B0;}),_tmp425->tl=dles;_tmp425;});}
# 2498
({void*_tmp9B1=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9B1;});}
# 2500
goto _LL7;}case 26U: _LL42: _tmp31B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL43: {struct Cyc_List_List*dles0=_tmp31B;
# 2506
{struct Cyc_List_List*_tmp426=dles0;struct Cyc_List_List*dles=_tmp426;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9B2=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp9B2;});
goto _LL7;}case 27U: _LL44: _tmp317=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp318=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp319=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_tmp31A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2FD)->f4;_LL45: {struct Cyc_Absyn_Vardecl*vd=_tmp317;struct Cyc_Absyn_Exp*e1=_tmp318;struct Cyc_Absyn_Exp*e2=_tmp319;int iszeroterm=_tmp31A;
# 2514
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple14 _tmp427=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple14 _stmttmp45=_tmp427;struct _tuple14 _tmp428=_stmttmp45;int _tmp42A;unsigned _tmp429;_LLF7: _tmp429=_tmp428.f1;_tmp42A=_tmp428.f2;_LLF8: {unsigned sz=_tmp429;int known=_tmp42A;
void*_tmp42B=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp42B;
struct Cyc_List_List*es=0;
# 2521
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp42C=0U;({unsigned _tmp9B4=e1->loc;struct _fat_ptr _tmp9B3=({const char*_tmp42D="cannot determine value of constant";_tag_fat(_tmp42D,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9B4,_tmp9B3,_tag_fat(_tmp42C,sizeof(void*),0U));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp42F=_cycalloc(sizeof(*_tmp42F));({struct _tuple20*_tmp9B5=({struct _tuple20*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->f1=0,_tmp42E->f2=e2;_tmp42E;});_tmp42F->hd=_tmp9B5;}),_tmp42F->tl=es;_tmp42F;});}}
# 2527
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp430=({void*_tmp9B6=elt_typ;Cyc_Toc_cast_it(_tmp9B6,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp430;
es=({struct Cyc_List_List*_tmp9B8=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9B8,({struct Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));({struct _tuple20*_tmp9B7=({struct _tuple20*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->f1=0,_tmp431->f2=ezero;_tmp431;});_tmp432->hd=_tmp9B7;}),_tmp432->tl=0;_tmp432;}));});}}
# 2532
({void*_tmp9B9=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9B9;});}}
# 2534
goto _LL7;}case 28U: _LL46: _tmp314=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp315=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp316=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_LL47: {struct Cyc_Absyn_Exp*e1=_tmp314;void*t1=_tmp315;int iszeroterm=_tmp316;
# 2541
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9BA=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9BA;});else{
# 2544
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30U: _LL48: _tmp312=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp313=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL49: {void*st=_tmp312;struct Cyc_List_List*dles=_tmp313;
# 2548
{struct Cyc_List_List*_tmp433=dles;struct Cyc_List_List*dles2=_tmp433;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)dles2->hd)).f2);}}{
void*_tmp434=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp434;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp435=Cyc_Tcutil_compress(strct_typ);void*_stmttmp46=_tmp435;void*_tmp436=_stmttmp46;union Cyc_Absyn_AggrInfo _tmp437;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f2 == 0){_LLFA: _tmp437=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp436)->f1)->f1;_LLFB: {union Cyc_Absyn_AggrInfo aggrinfo=_tmp437;
# 2555
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp9BB=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=29U,_tmp438->f1=sd->name,_tmp438->f2=0,_tmp438->f3=dles,_tmp438->f4=sd;_tmp438;});e->r=_tmp9BB;});
e->topt=strct_typ;
goto _LLF9;}}else{goto _LLFC;}}else{goto _LLFC;}}else{_LLFC: _LLFD:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E1;_tmp7E1.tag=0U,({struct _fat_ptr _tmp9BC=({const char*_tmp43B="anonStruct type not an aggregate";_tag_fat(_tmp43B,sizeof(char),33U);});_tmp7E1.f1=_tmp9BC;});_tmp7E1;});void*_tmp439[1U];_tmp439[0]=& _tmp43A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp439,sizeof(void*),1U));});}_LLF9:;}else{
# 2562
({void*_tmp9BD=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp9BD;});}
goto _LL7;}}case 29U: _LL4A: _tmp30E=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp30F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp310=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_tmp311=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2FD)->f4;_LL4B: {struct _tuple1**tdn=_tmp30E;struct Cyc_List_List*exist_ts=_tmp30F;struct Cyc_List_List*dles=_tmp310;struct Cyc_Absyn_Aggrdecl*sd=_tmp311;
# 2568
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E2;_tmp7E2.tag=0U,({struct _fat_ptr _tmp9BE=({const char*_tmp43E="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp43E,sizeof(char),58U);});_tmp7E2.f1=_tmp9BE;});_tmp7E2;});void*_tmp43C[1U];_tmp43C[0]=& _tmp43D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp43C,sizeof(void*),1U));});{
void*_tmp43F=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp43F;
{void*_tmp440=Cyc_Tcutil_compress(new_typ);void*_stmttmp47=_tmp440;void*_tmp441=_stmttmp47;union Cyc_Absyn_AggrInfo _tmp442;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp441)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp441)->f1)->tag == 20U){_LLFF: _tmp442=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp441)->f1)->f1;_LL100: {union Cyc_Absyn_AggrInfo info=_tmp442;
({struct _tuple1*_tmp9BF=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp9BF;});goto _LLFE;}}else{goto _LL101;}}else{_LL101: _LL102:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp444=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E3;_tmp7E3.tag=0U,({struct _fat_ptr _tmp9C0=({const char*_tmp445="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp445,sizeof(char),44U);});_tmp7E3.f1=_tmp9C0;});_tmp7E3;});void*_tmp443[1U];_tmp443[0]=& _tmp444;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp443,sizeof(void*),1U));});}_LLFE:;}{
# 2579
struct Cyc_List_List*_tmp446=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp446;
if(typ_fields == 0)goto _LL7;
for(0;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp447=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp447;
struct Cyc_List_List*_tmp448=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp448;
# 2586
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp449=Cyc_Tcutil_compress(old_typ);void*_stmttmp48=_tmp449;void*_tmp44A=_stmttmp48;struct Cyc_List_List*_tmp44B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->f1)->tag == 20U){_LL104: _tmp44B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44A)->f2;_LL105: {struct Cyc_List_List*param_ts=_tmp44B;
# 2590
{struct Cyc_List_List*_tmp44C=fields;struct Cyc_List_List*fields2=_tmp44C;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_tmp44D=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp49=_tmp44D;struct _tuple36*_tmp44E=_stmttmp49;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Aggrfield*_tmp44F;_LL109: _tmp44F=_tmp44E->f1;_tmp450=_tmp44E->f2;_LL10A: {struct Cyc_Absyn_Aggrfield*aggrfield=_tmp44F;struct Cyc_Absyn_Exp*fieldexp=_tmp450;
void*_tmp451=fieldexp->topt;void*old_field_typ=_tmp451;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2595
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp9C2=({void*_tmp9C1=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp9C1,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2597
fieldexp->r=_tmp9C2;});
# 2600
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp452=aggrfield->name;struct _fat_ptr*n=_tmp452;
struct Cyc_Absyn_Exp*_tmp453=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp453;
struct _tuple20*_tmp454=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple20*tag_dle=_tmp454;
struct _tuple20*_tmp455=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple20*val_dle=_tmp455;
struct _tuple1*s=({struct _tuple1*_tmp460=_cycalloc(sizeof(*_tmp460));({union Cyc_Absyn_Nmspace _tmp9C6=Cyc_Absyn_Abs_n(0,1);_tmp460->f1=_tmp9C6;}),({
struct _fat_ptr*_tmp9C5=({struct _fat_ptr*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct _fat_ptr _tmp9C4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45D=({struct Cyc_String_pa_PrintArg_struct _tmp7E5;_tmp7E5.tag=0U,_tmp7E5.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp7E5;});struct Cyc_String_pa_PrintArg_struct _tmp45E=({struct Cyc_String_pa_PrintArg_struct _tmp7E4;_tmp7E4.tag=0U,_tmp7E4.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp7E4;});void*_tmp45B[2U];_tmp45B[0]=& _tmp45D,_tmp45B[1]=& _tmp45E;({struct _fat_ptr _tmp9C3=({const char*_tmp45C="_union_%s_%s";_tag_fat(_tmp45C,sizeof(char),13U);});Cyc_aprintf(_tmp9C3,_tag_fat(_tmp45B,sizeof(void*),2U));});});*_tmp45F=_tmp9C4;});_tmp45F;});_tmp460->f2=_tmp9C5;});_tmp460;});
# 2608
struct _tuple20*_tmp456=({
struct _fat_ptr*_tmp9C8=n;Cyc_Toc_make_field(_tmp9C8,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=29U,_tmp45A->f1=s,_tmp45A->f2=0,({struct Cyc_List_List*_tmp9C7=({struct _tuple20*_tmp459[2U];_tmp459[0]=tag_dle,_tmp459[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp459,sizeof(struct _tuple20*),2U));});_tmp45A->f3=_tmp9C7;}),_tmp45A->f4=0;_tmp45A;}),0U));});
# 2608
struct _tuple20*u_dle=_tmp456;
# 2611
({void*_tmp9CA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->tag=29U,_tmp458->f1=*tdn,_tmp458->f2=0,({struct Cyc_List_List*_tmp9C9=({struct _tuple20*_tmp457[1U];_tmp457[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp457,sizeof(struct _tuple20*),1U));});_tmp458->f3=_tmp9C9;}),_tmp458->f4=sd;_tmp458;});e->r=_tmp9CA;});}
# 2617
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp461=({struct Cyc_List_List*_tmp9CB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9CB,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(sd->tvs,param_ts));});
# 2618
struct Cyc_List_List*inst=_tmp461;
# 2620
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp462=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp462;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp463=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp463;
void*_tmp464=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp464;
struct Cyc_Absyn_Aggrfield*_tmp465=({struct Cyc_Absyn_Aggrfield*_tmp9CC=old_f;Cyc_Toc_aggrfield_to_c(_tmp9CC,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2624
struct Cyc_Absyn_Aggrfield*new_f=_tmp465;
# 2626
new_fields=({struct Cyc_List_List*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->hd=new_f,_tmp466->tl=new_fields;_tmp466;});
# 2632
if(fs->tl == 0){
{void*_tmp467=Cyc_Tcutil_compress(new_f->type);void*_stmttmp4A=_tmp467;void*_tmp468=_stmttmp4A;struct Cyc_Absyn_ArrayInfo _tmp469;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->tag == 4U){_LL10C: _tmp469=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->f1;_LL10D: {struct Cyc_Absyn_ArrayInfo ai=_tmp469;
# 2635
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp46A=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp46A;
({struct Cyc_Absyn_Exp*_tmp9CD=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp9CD;});
({void*_tmp9CE=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B->tag=4U,_tmp46B->f1=ai2;_tmp46B;});new_f->type=_tmp9CE;});}
# 2640
goto _LL10B;}}else{_LL10E: _LL10F:
# 2644
 if(fieldexp->topt == 0)
goto _LL10B;
{void*_tmp46C=Cyc_Tcutil_compress((void*)_check_null(fieldexp->topt));void*_stmttmp4B=_tmp46C;void*_tmp46D=_stmttmp4B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp46D)->f1)->tag == 20U){_LL111: _LL112:
# 2648
 new_f->type=(void*)_check_null(fieldexp->topt);goto _LL110;}else{goto _LL113;}}else{_LL113: _LL114:
 goto _LL110;}_LL110:;}
# 2651
goto _LL10B;}_LL10B:;}
# 2655
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp9D0=({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));({void*_tmp9CF=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=6U,_tmp46E->f1=0;_tmp46E;});_tmp46F->hd=_tmp9CF;}),_tmp46F->tl=new_f->attributes;_tmp46F;});new_f->attributes=_tmp9D0;});}}}
# 2661
sd=({struct _fat_ptr*_tmp9D3=({struct _fat_ptr*_tmp473=_cycalloc(sizeof(*_tmp473));({struct _fat_ptr _tmp9D2=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp472=({struct Cyc_Int_pa_PrintArg_struct _tmp7E6;_tmp7E6.tag=1U,_tmp7E6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7E6;});void*_tmp470[1U];_tmp470[0]=& _tmp472;({struct _fat_ptr _tmp9D1=({const char*_tmp471="_genStruct%d";_tag_fat(_tmp471,sizeof(char),13U);});Cyc_aprintf(_tmp9D1,_tag_fat(_tmp470,sizeof(void*),1U));});});*_tmp473=_tmp9D2;});_tmp473;});Cyc_Toc_make_c_struct_defn(_tmp9D3,
# 2663
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2667
({void*_tmp9D4=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp474=_cycalloc(sizeof(*_tmp474));*_tmp474=sd;_tmp474;})),0);e->topt=_tmp9D4;});}}}}
# 2670
goto _LL103;}}else{goto _LL106;}}else{_LL106: _LL107:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp476=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E7;_tmp7E7.tag=0U,({struct _fat_ptr _tmp9D5=({const char*_tmp477="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp477,sizeof(char),39U);});_tmp7E7.f1=_tmp9D5;});_tmp7E7;});void*_tmp475[1U];_tmp475[0]=& _tmp476;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp475,sizeof(void*),1U));});}_LL103:;}else{
# 2679
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2681
struct _tuple36*_tmp478=(struct _tuple36*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple36*_stmttmp4C=_tmp478;struct _tuple36*_tmp479=_stmttmp4C;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Aggrfield*_tmp47A;_LL116: _tmp47A=_tmp479->f1;_tmp47B=_tmp479->f2;_LL117: {struct Cyc_Absyn_Aggrfield*field=_tmp47A;struct Cyc_Absyn_Exp*fieldexp=_tmp47B;
void*_tmp47C=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp47C;
void*_tmp47D=field->type;void*fieldtyp=_tmp47D;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp9D7=({
void*_tmp9D6=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp9D6,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2687
fieldexp->r=_tmp9D7;});{
# 2690
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp47E=({struct _tuple20*_tmp480[2U];({struct _tuple20*_tmp9D9=({struct _tuple20*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->f1=0,_tmp481->f2=field_tag_exp;_tmp481;});_tmp480[0]=_tmp9D9;}),({struct _tuple20*_tmp9D8=({struct _tuple20*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->f1=0,_tmp482->f2=fieldexp;_tmp482;});_tmp480[1]=_tmp9D8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp480,sizeof(struct _tuple20*),2U));});struct Cyc_List_List*newdles=_tmp47E;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp9DB=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp47F[1U];({struct _tuple20*_tmp9DA=Cyc_Toc_make_field(field->name,umem);_tmp47F[0]=_tmp9DA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp47F,sizeof(struct _tuple20*),1U));}));e->r=_tmp9DB;});}}}else{
# 2697
struct Cyc_List_List*_tmp483=0;struct Cyc_List_List*newdles=_tmp483;
struct Cyc_List_List*_tmp484=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp484;
for(0;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp485=fields;struct Cyc_List_List*fields2=_tmp485;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_tmp486=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp4D=_tmp486;struct _tuple36*_tmp487=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Aggrfield*_tmp488;_LL119: _tmp488=_tmp487->f1;_tmp489=_tmp487->f2;_LL11A: {struct Cyc_Absyn_Aggrfield*field=_tmp488;struct Cyc_Absyn_Exp*fieldexp=_tmp489;
void*_tmp48A=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp48A;
void*_tmp48B=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp48B;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2707
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2709
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp9DC=fieldtyp;Cyc_Toc_cast_it(_tmp9DC,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp48D=_cycalloc(sizeof(*_tmp48D));({struct _tuple20*_tmp9DD=({struct _tuple20*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->f1=0,_tmp48C->f2=fieldexp;_tmp48C;});_tmp48D->hd=_tmp9DD;}),_tmp48D->tl=newdles;_tmp48D;});
break;}}}}
# 2716
({void*_tmp9DE=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(newdles));e->r=_tmp9DE;});}}
# 2719
goto _LL7;}}}}case 31U: _LL4C: _tmp30B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp30C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_tmp30D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_LL4D: {struct Cyc_List_List*es=_tmp30B;struct Cyc_Absyn_Datatypedecl*tud=_tmp30C;struct Cyc_Absyn_Datatypefield*tuf=_tmp30D;
# 2723
struct Cyc_List_List*_tmp48E=tuf->typs;struct Cyc_List_List*tqts=_tmp48E;
{struct Cyc_List_List*_tmp48F=es;struct Cyc_List_List*es2=_tmp48F;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp490=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp490;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple13*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp9DF=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp9DF;});}}{
# 2734
struct Cyc_Absyn_Exp*_tmp491=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2734
struct Cyc_Absyn_Exp*tag_exp=_tmp491;
# 2737
if(!Cyc_Toc_is_toplevel(nv)){
# 2739
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));({struct _tuple20*_tmp9E1=({struct _fat_ptr*_tmp9E0=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9E0,(struct Cyc_Absyn_Exp*)es->hd);});_tmp492->hd=_tmp9E1;}),_tmp492->tl=dles;_tmp492;});}}{
# 2745
struct _tuple20*_tmp493=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple20*tag_dle=_tmp493;
({void*_tmp9E5=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->tag=29U,({struct _tuple1*_tmp9E4=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp495->f1=_tmp9E4;}),_tmp495->f2=0,({
struct Cyc_List_List*_tmp9E3=({struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->hd=tag_dle,({struct Cyc_List_List*_tmp9E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp494->tl=_tmp9E2;});_tmp494;});_tmp495->f3=_tmp9E3;}),_tmp495->f4=0;_tmp495;});
# 2746
e->r=_tmp9E5;});}}else{
# 2751
struct Cyc_List_List*_tmp496=0;struct Cyc_List_List*dles=_tmp496;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp498=_cycalloc(sizeof(*_tmp498));({struct _tuple20*_tmp9E6=({struct _tuple20*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->f1=0,_tmp497->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp497;});_tmp498->hd=_tmp9E6;}),_tmp498->tl=dles;_tmp498;});}
({void*_tmp9E9=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp49A=_cycalloc(sizeof(*_tmp49A));({struct _tuple20*_tmp9E8=({struct _tuple20*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->f1=0,_tmp499->f2=tag_exp;_tmp499;});_tmp49A->hd=_tmp9E8;}),({
struct Cyc_List_List*_tmp9E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp49A->tl=_tmp9E7;});_tmp49A;}));
# 2754
e->r=_tmp9E9;});}
# 2757
goto _LL7;}}case 32U: _LL4E: _LL4F:
# 2759
 goto _LL51;case 33U: _LL50: _LL51:
 goto _LL7;case 34U: _LL52: _tmp305=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).is_calloc;_tmp306=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).rgn;_tmp307=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).elt_type;_tmp308=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).num_elts;_tmp309=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).fat_result;_tmp30A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1).inline_call;_LL53: {int is_calloc=_tmp305;struct Cyc_Absyn_Exp*rgnopt=_tmp306;void**topt=_tmp307;struct Cyc_Absyn_Exp*num_elts=_tmp308;int is_fat=_tmp309;int inline_call=_tmp30A;
# 2763
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(topt)));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2772
if(is_calloc){
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp49B[3U];_tmp49B[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp9EA=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp49B[1]=_tmp9EA;}),_tmp49B[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9EB=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp9EB,_tag_fat(_tmp49B,sizeof(struct Cyc_Absyn_Exp*),3U));});});}else{
# 2778
pexp=({void*_tmp9ED=*topt;struct Cyc_Absyn_Exp*_tmp9EC=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9ED,_tmp9EC,num_elts2);});}}else{
# 2780
if(rgnopt != 0 && !Cyc_Absyn_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
if(inline_call)
pexp=Cyc_Toc_rmalloc_inline_exp(rgnopt,num_elts2);else{
# 2785
pexp=Cyc_Toc_rmalloc_exp(rgnopt,num_elts2);}}else{
# 2787
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2789
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp49C[3U];_tmp49C[0]=pexp,_tmp49C[1]=elt_sz,_tmp49C[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9EE=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp9EE,_tag_fat(_tmp49C,sizeof(struct Cyc_Absyn_Exp*),3U));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp9F1=(struct _tuple1*)_check_null(x);void*_tmp9F0=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9EF=num_elts;Cyc_Absyn_declare_stmt(_tmp9F1,_tmp9F0,_tmp9EF,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp9F2=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9F2;});}else{
# 2796
e->r=pexp->r;}
goto _LL7;}}case 35U: _LL54: _tmp303=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp304=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL55: {struct Cyc_Absyn_Exp*e1=_tmp303;struct Cyc_Absyn_Exp*e2=_tmp304;
# 2806
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E8;_tmp7E8.tag=0U,({struct _fat_ptr _tmp9F3=({const char*_tmp49F="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp49F,sizeof(char),57U);});_tmp7E8.f1=_tmp9F3;});_tmp7E8;});void*_tmp49D[1U];_tmp49D[0]=& _tmp49E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp49D,sizeof(void*),1U));});{
# 2814
unsigned _tmp4A0=e->loc;unsigned loc=_tmp4A0;
struct _tuple1*_tmp4A1=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp4A1;
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp4A2;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp4A3=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp4A3;
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp4A4;v2e->topt=e2_old_typ;{
# 2820
struct Cyc_Absyn_Exp*_tmp4A5=({struct Cyc_Absyn_Exp*_tmp9F5=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp9F4=v2e;Cyc_Absyn_assign_exp(_tmp9F5,_tmp9F4,loc);});struct Cyc_Absyn_Exp*s1e=_tmp4A5;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A6=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp4A6;
struct Cyc_Absyn_Exp*_tmp4A7=({struct Cyc_Absyn_Exp*_tmp9F7=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp9F6=v1e;Cyc_Absyn_assign_exp(_tmp9F7,_tmp9F6,loc);});struct Cyc_Absyn_Exp*s2e=_tmp4A7;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp4A8;
# 2825
struct Cyc_Absyn_Stmt*_tmp4A9=({struct _tuple1*_tmp9FF=v1;void*_tmp9FE=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9FD=e1;struct Cyc_Absyn_Stmt*_tmp9FC=({
struct _tuple1*_tmp9FB=v2;void*_tmp9FA=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9F9=e2;struct Cyc_Absyn_Stmt*_tmp9F8=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2826
Cyc_Absyn_declare_stmt(_tmp9FB,_tmp9FA,_tmp9F9,_tmp9F8,loc);});
# 2825
Cyc_Absyn_declare_stmt(_tmp9FF,_tmp9FE,_tmp9FD,_tmp9FC,loc);});struct Cyc_Absyn_Stmt*s=_tmp4A9;
# 2828
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmpA00=Cyc_Toc_stmt_exp_r(s);e->r=_tmpA00;});
goto _LL7;}}}}}}case 38U: _LL56: _tmp301=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_tmp302=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2FD)->f2;_LL57: {struct Cyc_Absyn_Exp*e1=_tmp301;struct _fat_ptr*f=_tmp302;
# 2833
void*_tmp4AA=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp4AA;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp4AB=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp4AC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)->f1).KnownAggr).tag == 2){_LL11C: _tmp4AC=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AB)->f1)->f1).KnownAggr).val;_LL11D: {struct Cyc_Absyn_Aggrdecl*ad=_tmp4AC;
# 2837
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp4AD;
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp4AE;
struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp4AF;
({void*_tmpA01=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmpA01;});
goto _LL11B;}}else{goto _LL11E;}}else{goto _LL11E;}}else{_LL11E: _LL11F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E9;_tmp7E9.tag=0U,({struct _fat_ptr _tmpA02=({const char*_tmp4B2="non-aggregate type in tagcheck";_tag_fat(_tmp4B2,sizeof(char),31U);});_tmp7E9.f1=_tmpA02;});_tmp7E9;});void*_tmp4B0[1U];_tmp4B0[0]=& _tmp4B1;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B0,sizeof(void*),1U));});}_LL11B:;}
# 2844
goto _LL7;}case 37U: _LL58: _tmp300=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2FD)->f1;_LL59: {struct Cyc_Absyn_Stmt*s=_tmp300;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36U: _LL5A: _LL5B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EA;_tmp7EA.tag=0U,({struct _fat_ptr _tmpA03=({const char*_tmp4B5="UnresolvedMem";_tag_fat(_tmp4B5,sizeof(char),14U);});_tmp7EA.f1=_tmpA03;});_tmp7EA;});void*_tmp4B3[1U];_tmp4B3[0]=& _tmp4B4;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B3,sizeof(void*),1U));});case 25U: _LL5C: _LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EB;_tmp7EB.tag=0U,({struct _fat_ptr _tmpA04=({const char*_tmp4B8="compoundlit";_tag_fat(_tmp4B8,sizeof(char),12U);});_tmp7EB.f1=_tmpA04;});_tmp7EB;});void*_tmp4B6[1U];_tmp4B6[0]=& _tmp4B7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B6,sizeof(void*),1U));});case 39U: _LL5E: _LL5F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EC;_tmp7EC.tag=0U,({struct _fat_ptr _tmpA05=({const char*_tmp4BB="valueof(-)";_tag_fat(_tmp4BB,sizeof(char),11U);});_tmp7EC.f1=_tmpA05;});_tmp7EC;});void*_tmp4B9[1U];_tmp4B9[0]=& _tmp4BA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4B9,sizeof(void*),1U));});default: _LL60: _tmp2FE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2FD)->f3;_tmp2FF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2FD)->f4;_LL61: {struct Cyc_List_List*o=_tmp2FE;struct Cyc_List_List*i=_tmp2FF;
# 2850
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2854
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EE;_tmp7EE.tag=0U,({struct _fat_ptr _tmpA06=({const char*_tmp4BF="Toc did not examine an inserted check: ";_tag_fat(_tmp4BF,sizeof(char),40U);});_tmp7EE.f1=_tmpA06;});_tmp7EE;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7ED;_tmp7ED.tag=4U,_tmp7ED.f1=e;_tmp7ED;});void*_tmp4BC[2U];_tmp4BC[0]=& _tmp4BD,_tmp4BC[1]=& _tmp4BE;({unsigned _tmpA07=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmpA07,_tag_fat(_tmp4BC,sizeof(void*),2U));});});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2883 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2885
return({struct _tuple37*_tmp4C0=_region_malloc(r,sizeof(*_tmp4C0));_tmp4C0->f1=0,({struct _fat_ptr*_tmpA09=Cyc_Toc_fresh_label();_tmp4C0->f2=_tmpA09;}),({struct _fat_ptr*_tmpA08=Cyc_Toc_fresh_label();_tmp4C0->f3=_tmpA08;}),_tmp4C0->f4=sc;_tmp4C0;});}
# 2891
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4C1=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp4C1;
# 2897
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4C4=p->orig_pat;if((_tmp4C4.pattern).tag != 1)_throw_match();(_tmp4C4.pattern).val;})->topt);
void*_tmp4C2=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp4C2;
void*_tmp4C3=t2;switch(*((int*)_tmp4C3)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->f1)){case 0U: _LL1: _LL2:
# 2902
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 v=({void*_tmpA0A=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA0A,v);});goto _LL0;}_LL0:;}{
# 2908
void*_tmp4C5=p->access;void*_stmttmp4E=_tmp4C5;void*_tmp4C6=_stmttmp4E;struct _fat_ptr*_tmp4C8;int _tmp4C7;unsigned _tmp4C9;unsigned _tmp4CA;switch(*((int*)_tmp4C6)){case 0U: _LLA: _LLB:
# 2912
 goto _LL9;case 1U: _LLC: _LLD:
# 2917
 if(ps->tl != 0){
void*_tmp4CB=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4F=_tmp4CB;void*_tmp4CC=_stmttmp4F;unsigned _tmp4CF;struct Cyc_Absyn_Datatypefield*_tmp4CE;struct Cyc_Absyn_Datatypedecl*_tmp4CD;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CC)->tag == 3U){_LL15: _tmp4CD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CC)->f1;_tmp4CE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CC)->f2;_tmp4CF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4CC)->f3;_LL16: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4CD;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CE;unsigned i=_tmp4CF;
# 2920
ps=ps->tl;
v=({void*_tmpA0C=({void*_tmpA0B=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA0B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA0C,v);});
v=({struct Cyc_Absyn_Exp*_tmpA0D=v;Cyc_Absyn_aggrarrow_exp(_tmpA0D,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}
# 2927
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3U: _LLE: _tmp4CA=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C6)->f3;_LLF: {unsigned i=_tmp4CA;
_tmp4C9=i;goto _LL11;}case 2U: _LL10: _tmp4C9=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4C6)->f1;_LL11: {unsigned i=_tmp4C9;
v=({struct Cyc_Absyn_Exp*_tmpA0E=v;Cyc_Toc_member_exp(_tmpA0E,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _LL12: _tmp4C7=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C6)->f1;_tmp4C8=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C6)->f2;_LL13: {int tagged=_tmp4C7;struct _fat_ptr*f=_tmp4C8;
# 2932
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2938
return v;}
# 2943
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4D0=t;struct Cyc_Absyn_Datatypefield*_tmp4D2;struct Cyc_Absyn_Datatypedecl*_tmp4D1;int _tmp4D4;struct _fat_ptr*_tmp4D3;struct Cyc_Absyn_Datatypefield*_tmp4D7;struct Cyc_Absyn_Datatypedecl*_tmp4D6;int _tmp4D5;unsigned _tmp4D8;int _tmp4DA;struct _fat_ptr _tmp4D9;struct Cyc_Absyn_Enumfield*_tmp4DC;void*_tmp4DB;struct Cyc_Absyn_Enumfield*_tmp4DE;struct Cyc_Absyn_Enumdecl*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DF;switch(*((int*)_tmp4D0)){case 0U: _LL1: _tmp4DF=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4D0)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp4DF;
return e == 0?v: e;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA0F=v;Cyc_Absyn_eq_exp(_tmpA0F,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA10=v;Cyc_Absyn_neq_exp(_tmpA10,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4DD=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4DE=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp4DD;struct Cyc_Absyn_Enumfield*ef=_tmp4DE;
return({struct Cyc_Absyn_Exp*_tmpA11=v;Cyc_Absyn_eq_exp(_tmpA11,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->tag=32U,_tmp4E0->f1=ed,_tmp4E0->f2=ef;_tmp4E0;}),0U),0U);});}case 4U: _LL9: _tmp4DB=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4DC=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LLA: {void*t=_tmp4DB;struct Cyc_Absyn_Enumfield*ef=_tmp4DC;
return({struct Cyc_Absyn_Exp*_tmpA12=v;Cyc_Absyn_eq_exp(_tmpA12,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->tag=33U,_tmp4E1->f1=t,_tmp4E1->f2=ef;_tmp4E1;}),0U),0U);});}case 5U: _LLB: _tmp4D9=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4DA=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LLC: {struct _fat_ptr s=_tmp4D9;int i=_tmp4DA;
return({struct Cyc_Absyn_Exp*_tmpA13=v;Cyc_Absyn_eq_exp(_tmpA13,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6U: _LLD: _tmp4D8=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4D0)->f1;_LLE: {unsigned i=_tmp4D8;
return({struct Cyc_Absyn_Exp*_tmpA14=v;Cyc_Absyn_eq_exp(_tmpA14,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7U: _LLF: _tmp4D5=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4D6=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D0)->f2;_tmp4D7=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4D0)->f3;_LL10: {int i=_tmp4D5;struct Cyc_Absyn_Datatypedecl*tud=_tmp4D6;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D7;
# 2955
LOOP1: {
void*_tmp4E2=v->r;void*_stmttmp50=_tmp4E2;void*_tmp4E3=_stmttmp50;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;switch(*((int*)_tmp4E3)){case 14U: _LL16: _tmp4E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E3)->f2;_LL17: {struct Cyc_Absyn_Exp*e=_tmp4E5;
v=e;goto LOOP1;}case 20U: _LL18: _tmp4E4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E3)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp4E4;
v=e;goto _LL15;}default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 2962
v=({void*_tmpA16=({void*_tmpA15=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA15,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA16,v);});return({struct Cyc_Absyn_Exp*_tmpA17=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA17,Cyc_Absyn_uint_exp((unsigned)i,0U),0U);});}case 8U: _LL11: _tmp4D3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4D4=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LL12: {struct _fat_ptr*f=_tmp4D3;int i=_tmp4D4;
# 2966
return({struct Cyc_Absyn_Exp*_tmpA19=({struct Cyc_Absyn_Exp*_tmpA18=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmpA18,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmpA19,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _LL13: _tmp4D1=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4D0)->f1;_tmp4D2=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4D0)->f2;_LL14: {struct Cyc_Absyn_Datatypedecl*tud=_tmp4D1;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D2;
# 2970
LOOP2: {
void*_tmp4E6=v->r;void*_stmttmp51=_tmp4E6;void*_tmp4E7=_stmttmp51;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;switch(*((int*)_tmp4E7)){case 14U: _LL1D: _tmp4E9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E7)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp4E9;
v=e;goto LOOP2;}case 20U: _LL1F: _tmp4E8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E7)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp4E8;
v=e;goto _LL1C;}default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 2977
v=({void*_tmpA1B=({void*_tmpA1A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA1A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA1B,v);});
return({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA1C,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2989
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple38{int f1;void*f2;};
# 2996
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2998
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _tmp4EA=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp52=_tmp4EA;struct _tuple34 _tmp4EB=_stmttmp52;void*_tmp4EC;_LL1: _tmp4EC=_tmp4EB.f1;_LL2: {void*ptest=_tmp4EC;
void*_tmp4ED=ptest;struct Cyc_Absyn_Exp*_tmp4EE;struct _fat_ptr*_tmp4EF;switch(*((int*)_tmp4ED)){case 3U: _LL4: _LL5:
# 3004
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
 continue;case 8U: _LLA: _tmp4EF=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4ED)->f1;_LLB: {struct _fat_ptr*f=_tmp4EF;
# 3008
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->tag=3U,_tmp4F0->f1=f;_tmp4F0;});
continue;}case 7U: _LLC: _LLD:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0U: _LLE: _tmp4EE=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4ED)->f1;if(_tmp4EE != 0){_LLF: {struct Cyc_Absyn_Exp*e=_tmp4EE;
# 3013
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->tag=2U,_tmp4F1->f1=e;_tmp4F1;});return({struct _tuple38 _tmp7EF;_tmp7EF.f1=0,_tmp7EF.f2=k;_tmp7EF;});}}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple38 _tmp7F0;_tmp7F0.f1=0,_tmp7F0.f2=k;_tmp7F0;});}_LL3:;}}
# 3021
return({struct _tuple38 _tmp7F1;_tmp7F1.f1=c,_tmp7F1.f2=k;_tmp7F1;});}
# 3026
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4F2=p;int _tmp4F3;int _tmp4F4;unsigned _tmp4F5;struct Cyc_Absyn_Enumfield*_tmp4F7;void*_tmp4F6;struct Cyc_Absyn_Enumfield*_tmp4F9;struct Cyc_Absyn_Enumdecl*_tmp4F8;switch(*((int*)_tmp4F2)){case 3U: _LL1: _tmp4F8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F2)->f1;_tmp4F9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4F2)->f2;_LL2: {struct Cyc_Absyn_Enumdecl*ed=_tmp4F8;struct Cyc_Absyn_Enumfield*ef=_tmp4F9;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->tag=32U,_tmp4FA->f1=ed,_tmp4FA->f2=ef;_tmp4FA;}),0U);goto _LL0;}case 4U: _LL3: _tmp4F6=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F2)->f1;_tmp4F7=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4F2)->f2;_LL4: {void*t=_tmp4F6;struct Cyc_Absyn_Enumfield*ef=_tmp4F7;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->tag=33U,_tmp4FB->f1=t,_tmp4FB->f2=ef;_tmp4FB;}),0U);goto _LL0;}case 6U: _LL5: _tmp4F5=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4F2)->f1;_LL6: {unsigned i=_tmp4F5;
_tmp4F4=(int)i;goto _LL8;}case 7U: _LL7: _tmp4F4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4F2)->f1;_LL8: {int i=_tmp4F4;
_tmp4F3=i;goto _LLA;}case 8U: _LL9: _tmp4F3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4F2)->f2;_LLA: {int i=_tmp4F3;
e=Cyc_Absyn_uint_exp((unsigned)i,0U);goto _LL0;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F2;_tmp7F2.tag=0U,({struct _fat_ptr _tmpA1D=({const char*_tmp4FE="compile_pat_test_as_case!";_tag_fat(_tmp4FE,sizeof(char),26U);});_tmp7F2.f1=_tmpA1D;});_tmp7F2;});void*_tmp4FC[1U];_tmp4FC[0]=& _tmp4FD;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp4FC,sizeof(void*),1U));});}_LL0:;}
# 3036
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->tag=17U,_tmp4FF->f1=e;_tmp4FF;}),0U);}
# 3040
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple40{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3051
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3055
void*t=(void*)_check_null(p->topt);
void*_tmp500=p->r;void*_stmttmp53=_tmp500;void*_tmp501=_stmttmp53;struct Cyc_List_List*_tmp503;union Cyc_Absyn_AggrInfo _tmp502;struct Cyc_List_List*_tmp504;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Pat*_tmp506;struct Cyc_List_List*_tmp509;struct Cyc_Absyn_Datatypefield*_tmp508;struct Cyc_Absyn_Datatypedecl*_tmp507;struct Cyc_Absyn_Pat*_tmp50B;struct Cyc_Absyn_Vardecl*_tmp50A;struct Cyc_Absyn_Vardecl*_tmp50C;struct Cyc_Absyn_Pat*_tmp50E;struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50F;switch(*((int*)_tmp501)){case 2U: _LL1: _tmp50F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp501)->f2;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp50F;
# 3058
struct Cyc_Absyn_Pat*_tmp510=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp510;
p2->topt=p->topt;
_tmp50D=vd;_tmp50E=p2;goto _LL4;}case 1U: _LL3: _tmp50D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp501)->f1;_tmp50E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp501)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp50D;struct Cyc_Absyn_Pat*p2=_tmp50E;
# 3063
struct Cyc_Absyn_Vardecl*_tmp511=({struct _tuple1*_tmpA1E=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA1E,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp511;
({struct Cyc_List_List*_tmpA20=({struct Cyc_List_List*_tmp513=_region_malloc(rgn,sizeof(*_tmp513));({struct _tuple39*_tmpA1F=({struct _tuple39*_tmp512=_region_malloc(rgn,sizeof(*_tmp512));_tmp512->f1=vd,_tmp512->f2=new_vd;_tmp512;});_tmp513->hd=_tmpA1F;}),_tmp513->tl=*decls;_tmp513;});*decls=_tmpA20;});{
struct Cyc_Absyn_Stmt*_tmp514=({struct Cyc_Absyn_Exp*_tmpA21=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->tag=4U,_tmp515->f1=new_vd;_tmp515;}),0U);Cyc_Absyn_assign_stmt(_tmpA21,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp514;
return({struct Cyc_Absyn_Stmt*_tmpA22=s;Cyc_Toc_seq_stmt_opt(_tmpA22,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4U: _LL5: _tmp50C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp501)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp50C;
# 3069
struct Cyc_Absyn_Vardecl*_tmp516=({struct _tuple1*_tmpA23=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA23,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp516;
({struct Cyc_List_List*_tmpA25=({struct Cyc_List_List*_tmp518=_region_malloc(rgn,sizeof(*_tmp518));({struct _tuple39*_tmpA24=({struct _tuple39*_tmp517=_region_malloc(rgn,sizeof(*_tmp517));_tmp517->f1=vd,_tmp517->f2=new_vd;_tmp517;});_tmp518->hd=_tmpA24;}),_tmp518->tl=*decls;_tmp518;});*decls=_tmpA25;});
return({struct Cyc_Absyn_Exp*_tmpA26=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->tag=4U,_tmp519->f1=new_vd;_tmp519;}),0U);Cyc_Absyn_assign_stmt(_tmpA26,Cyc_Absyn_copy_exp(path),0U);});}case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp50A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp501)->f1;_tmp50B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp501)->f2;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp50A;struct Cyc_Absyn_Pat*p2=_tmp50B;
# 3075
({void*_tmpA27=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmpA27;});{
struct Cyc_Absyn_Vardecl*_tmp51A=({struct _tuple1*_tmpA28=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA28,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp51A;
({struct Cyc_List_List*_tmpA2A=({struct Cyc_List_List*_tmp51C=_region_malloc(rgn,sizeof(*_tmp51C));({struct _tuple39*_tmpA29=({struct _tuple39*_tmp51B=_region_malloc(rgn,sizeof(*_tmp51B));_tmp51B->f1=vd,_tmp51B->f2=new_vd;_tmp51B;});_tmp51C->hd=_tmpA29;}),_tmp51C->tl=*decls;_tmp51C;});*decls=_tmpA2A;});{
# 3079
struct Cyc_Absyn_Stmt*_tmp51D=({struct Cyc_Absyn_Exp*_tmpA2D=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->tag=4U,_tmp51E->f1=new_vd;_tmp51E;}),0U);Cyc_Absyn_assign_stmt(_tmpA2D,({
void*_tmpA2C=({void*_tmpA2B=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA2B,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA2C,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3079
struct Cyc_Absyn_Stmt*s=_tmp51D;
# 3082
return({struct Cyc_Absyn_Stmt*_tmpA2E=s;Cyc_Toc_seq_stmt_opt(_tmpA2E,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9U: _LLB: _LLC:
# 3084
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp501)->f1)->r)->tag == 8U){_LL17: _tmp507=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp501)->f1)->r)->f1;_tmp508=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp501)->f1)->r)->f2;_tmp509=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp501)->f1)->r)->f3;_LL18: {struct Cyc_Absyn_Datatypedecl*tud=_tmp507;struct Cyc_Absyn_Datatypefield*tuf=_tmp508;struct Cyc_List_List*ps=_tmp509;
# 3093
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp51F=({void*_tmpA2F=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA2F,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp51F;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp520=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp520;
struct Cyc_Absyn_Stmt*s=0;
for(0;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp521=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp521;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp522=(*((struct _tuple13*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp522;
void*_tmp523=(void*)_check_null(p2->topt);void*t2=_tmp523;
void*_tmp524=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp524;
struct Cyc_Absyn_Exp*_tmp525=({struct Cyc_Absyn_Exp*_tmpA30=path;Cyc_Absyn_aggrarrow_exp(_tmpA30,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp525;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmpA31=s;Cyc_Toc_seq_stmt_opt(_tmpA31,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3111
return s;}}}}else{_LL21: _tmp506=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp501)->f1;_LL22: {struct Cyc_Absyn_Pat*p2=_tmp506;
# 3156
return({struct _RegionHandle*_tmpA35=rgn;struct Cyc_Toc_Env**_tmpA34=nv;struct Cyc_List_List**_tmpA33=decls;struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA35,_tmpA34,_tmpA33,_tmpA32,p2);});}}case 8U: _LL19: _tmp505=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp501)->f3;_LL1A: {struct Cyc_List_List*ps=_tmp505;
# 3113
_tmp504=ps;goto _LL1C;}case 5U: _LL1B: _tmp504=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp501)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp504;
# 3115
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp526=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp526;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp527=(void*)_check_null(p2->topt);void*t2=_tmp527;
struct _fat_ptr*_tmp528=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp528;
s=({struct Cyc_Absyn_Stmt*_tmpA3A=s;Cyc_Toc_seq_stmt_opt(_tmpA3A,({struct _RegionHandle*_tmpA39=rgn;struct Cyc_Toc_Env**_tmpA38=nv;struct Cyc_List_List**_tmpA37=decls;struct Cyc_Absyn_Exp*_tmpA36=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmpA39,_tmpA38,_tmpA37,_tmpA36,p2);}));});}}
# 3125
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp501)->f1 == 0){_LL1D: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F3;_tmp7F3.tag=0U,({struct _fat_ptr _tmpA3B=({const char*_tmp52B="unresolved aggregate pattern!";_tag_fat(_tmp52B,sizeof(char),30U);});_tmp7F3.f1=_tmpA3B;});_tmp7F3;});void*_tmp529[1U];_tmp529[0]=& _tmp52A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp529,sizeof(void*),1U));});}else{_LL1F: _tmp502=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp501)->f1;_tmp503=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp501)->f3;_LL20: {union Cyc_Absyn_AggrInfo info=_tmp502;struct Cyc_List_List*dlps=_tmp503;
# 3129
struct Cyc_Absyn_Aggrdecl*_tmp52C=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp52C;
struct Cyc_Absyn_Stmt*s=0;
for(0;dlps != 0;dlps=dlps->tl){
struct _tuple40*_tmp52D=(struct _tuple40*)dlps->hd;struct _tuple40*tup=_tmp52D;
struct Cyc_Absyn_Pat*_tmp52E=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp52E;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp52F=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp52F;
void*_tmp530=(void*)_check_null(p2->topt);void*t2=_tmp530;
void*_tmp531=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp531;
struct Cyc_Absyn_Exp*_tmp532=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp532;
# 3141
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp533=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp533;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&& Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(ftype)))
# 3148
memexp=Cyc_Absyn_deref_exp(({void*_tmpA3C=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA3C,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3151
s=({struct Cyc_Absyn_Stmt*_tmpA3D=s;Cyc_Toc_seq_stmt_opt(_tmpA3D,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3153
return s;}}case 15U: _LL23: _LL24:
# 3158
({struct Cyc_Warn_String_Warn_Warg_struct _tmp535=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F4;_tmp7F4.tag=0U,({struct _fat_ptr _tmpA3E=({const char*_tmp536="unknownid pat";_tag_fat(_tmp536,sizeof(char),14U);});_tmp7F4.f1=_tmpA3E;});_tmp7F4;});void*_tmp534[1U];_tmp534[0]=& _tmp535;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp534,sizeof(void*),1U));});case 16U: _LL25: _LL26:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp538=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F5;_tmp7F5.tag=0U,({struct _fat_ptr _tmpA3F=({const char*_tmp539="unknowncall pat";_tag_fat(_tmp539,sizeof(char),16U);});_tmp7F5.f1=_tmpA3F;});_tmp7F5;});void*_tmp537[1U];_tmp537[0]=& _tmp538;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp537,sizeof(void*),1U));});default: _LL27: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F6;_tmp7F6.tag=0U,({struct _fat_ptr _tmpA40=({const char*_tmp53C="exp pat";_tag_fat(_tmp53C,sizeof(char),8U);});_tmp7F6.f1=_tmpA40;});_tmp7F6;});void*_tmp53A[1U];_tmp53A[0]=& _tmp53B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp53A,sizeof(void*),1U));});}_LL0:;}
# 3164
static struct Cyc_Absyn_Vardecl*Cyc_Toc_find_new_var(struct Cyc_List_List*vs,struct Cyc_Absyn_Vardecl*v){
for(0;vs != 0;vs=vs->tl){
struct _tuple39 _tmp53D=*((struct _tuple39*)vs->hd);struct _tuple39 _stmttmp54=_tmp53D;struct _tuple39 _tmp53E=_stmttmp54;struct Cyc_Absyn_Vardecl*_tmp540;struct Cyc_Absyn_Vardecl*_tmp53F;_LL1: _tmp53F=_tmp53E.f1;_tmp540=_tmp53E.f2;_LL2: {struct Cyc_Absyn_Vardecl*oldv=_tmp53F;struct Cyc_Absyn_Vardecl*newv=_tmp540;
if(oldv == newv)return newv;}}
# 3169
({void*_tmp541=0U;({struct _fat_ptr _tmpA41=({const char*_tmp542="find_new_var";_tag_fat(_tmp542,sizeof(char),13U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tmpA41,_tag_fat(_tmp541,sizeof(void*),0U));});});}
# 3175
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp543=e->r;void*_stmttmp55=_tmp543;void*_tmp544=_stmttmp55;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp546;struct Cyc_List_List*_tmp547;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54C;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_Absyn_Exp*_tmp565;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Vardecl*_tmp56A;struct Cyc_Absyn_Vardecl*_tmp56B;switch(*((int*)_tmp544)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp544)->f1)){case 5U: _LL1: _tmp56B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp544)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp56B;
_tmp56A=vd;goto _LL4;}case 4U: _LL3: _tmp56A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp544)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp56A;
# 3179
{struct _handler_cons _tmp56C;_push_handler(& _tmp56C);{int _tmp56E=0;if(setjmp(_tmp56C.handler))_tmp56E=1;if(!_tmp56E){
({void*_tmpA44=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->tag=1U,({void*_tmpA43=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=4U,({struct Cyc_Absyn_Vardecl*_tmpA42=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc)(env,vd);_tmp56F->f1=_tmpA42;});_tmp56F;});_tmp570->f1=_tmpA43;});_tmp570;});e->r=_tmpA44;});;_pop_handler();}else{void*_tmp56D=(void*)Cyc_Core_get_exn_thrown();void*_tmp571=_tmp56D;void*_tmp572;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp571)->tag == Cyc_Core_Not_found){_LL40: _LL41:
# 3182
 goto _LL3F;}else{_LL42: _tmp572=_tmp571;_LL43: {void*exn=_tmp572;(int)_rethrow(exn);}}_LL3F:;}}}
# 3184
goto _LL0;}default: goto _LL3D;}case 6U: _LL5: _tmp567=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp568=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_tmp569=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp544)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp567;struct Cyc_Absyn_Exp*e2=_tmp568;struct Cyc_Absyn_Exp*e3=_tmp569;
# 3186
Cyc_Toc_subst_pattern_vars(env,e1);_tmp565=e2;_tmp566=e3;goto _LL8;}case 27U: _LL7: _tmp565=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_tmp566=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp544)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp565;struct Cyc_Absyn_Exp*e2=_tmp566;
_tmp563=e1;_tmp564=e2;goto _LLA;}case 7U: _LL9: _tmp563=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp564=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp563;struct Cyc_Absyn_Exp*e2=_tmp564;
_tmp561=e1;_tmp562=e2;goto _LLC;}case 8U: _LLB: _tmp561=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp562=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp561;struct Cyc_Absyn_Exp*e2=_tmp562;
_tmp55F=e1;_tmp560=e2;goto _LLE;}case 23U: _LLD: _tmp55F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp560=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp55F;struct Cyc_Absyn_Exp*e2=_tmp560;
_tmp55D=e1;_tmp55E=e2;goto _LL10;}case 9U: _LLF: _tmp55D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp55E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp55D;struct Cyc_Absyn_Exp*e2=_tmp55E;
# 3192
Cyc_Toc_subst_pattern_vars(env,e1);_tmp55C=e2;goto _LL12;}case 41U: _LL11: _tmp55C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp55C;
_tmp55B=e;goto _LL14;}case 38U: _LL13: _tmp55B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp55B;
_tmp55A=e;goto _LL16;}case 11U: _LL15: _tmp55A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp55A;
_tmp559=e;goto _LL18;}case 12U: _LL17: _tmp559=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp559;
_tmp558=e;goto _LL1A;}case 13U: _LL19: _tmp558=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp558;
_tmp557=e;goto _LL1C;}case 14U: _LL1B: _tmp557=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp557;
_tmp556=e;goto _LL1E;}case 18U: _LL1D: _tmp556=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp556;
_tmp555=e;goto _LL20;}case 20U: _LL1F: _tmp555=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp555;
_tmp554=e;goto _LL22;}case 21U: _LL21: _tmp554=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp554;
_tmp553=e;goto _LL24;}case 22U: _LL23: _tmp553=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp553;
_tmp552=e;goto _LL26;}case 28U: _LL25: _tmp552=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp552;
_tmp551=e;goto _LL28;}case 15U: _LL27: _tmp551=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp551;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34U: _LL29: _tmp54F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp544)->f1).rgn;_tmp550=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp544)->f1).num_elts;_LL2A: {struct Cyc_Absyn_Exp*eopt=_tmp54F;struct Cyc_Absyn_Exp*e=_tmp550;
_tmp54D=eopt;_tmp54E=e;goto _LL2C;}case 16U: _LL2B: _tmp54D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_tmp54E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp54D;struct Cyc_Absyn_Exp*e=_tmp54E;
# 3207
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3U: _LL2D: _tmp54C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL2E: {struct Cyc_List_List*es=_tmp54C;
_tmp54B=es;goto _LL30;}case 31U: _LL2F: _tmp54B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL30: {struct Cyc_List_List*es=_tmp54B;
_tmp54A=es;goto _LL32;}case 24U: _LL31: _tmp54A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL32: {struct Cyc_List_List*es=_tmp54A;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36U: _LL33: _tmp549=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL34: {struct Cyc_List_List*dles=_tmp549;
_tmp548=dles;goto _LL36;}case 29U: _LL35: _tmp548=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp544)->f3;_LL36: {struct Cyc_List_List*dles=_tmp548;
_tmp547=dles;goto _LL38;}case 30U: _LL37: _tmp547=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL38: {struct Cyc_List_List*dles=_tmp547;
_tmp546=dles;goto _LL3A;}case 26U: _LL39: _tmp546=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp544)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp546;
_tmp545=dles;goto _LL3C;}case 25U: _LL3B: _tmp545=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp544)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp545;
# 3217
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp573=(struct _tuple20*)dles->hd;struct _tuple20*_stmttmp56=_tmp573;struct _tuple20*_tmp574=_stmttmp56;struct Cyc_Absyn_Exp*_tmp575;_LL45: _tmp575=_tmp574->f2;_LL46: {struct Cyc_Absyn_Exp*e=_tmp575;
Cyc_Toc_subst_pattern_vars(env,e);}}
# 3221
goto _LL0;}default: _LL3D: _LL3E:
 goto _LL0;}_LL0:;}struct _tuple41{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3231
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3239
void*_tmp576=dopt;void*_tmp579;struct Cyc_List_List*_tmp578;struct Cyc_List_List*_tmp577;struct Cyc_Tcpat_Rhs*_tmp57A;if(_tmp576 == 0){_LL1: _LL2:
# 3241
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp576)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp57A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp576)->f1;_LL6: {struct Cyc_Tcpat_Rhs*rhs=_tmp57A;
# 3245
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp57B=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp57=_tmp57B;struct _tuple37*_tmp57C=_stmttmp57;struct Cyc_Absyn_Switch_clause*_tmp580;struct _fat_ptr*_tmp57F;struct _fat_ptr*_tmp57E;int*_tmp57D;_LLA: _tmp57D=(int*)& _tmp57C->f1;_tmp57E=_tmp57C->f2;_tmp57F=_tmp57C->f3;_tmp580=_tmp57C->f4;_LLB: {int*already_emitted=_tmp57D;struct _fat_ptr*init_lab=_tmp57E;struct _fat_ptr*code_lab=_tmp57F;struct Cyc_Absyn_Switch_clause*sc=_tmp580;
struct Cyc_Absyn_Stmt*_tmp581=sc->body;struct Cyc_Absyn_Stmt*body=_tmp581;
if(body == rhs->rhs){
# 3250
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3255
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA46=rgn;struct Cyc_Absyn_Exp*_tmpA45=
Cyc_Absyn_var_exp(v,0U);
# 3255
Cyc_Toc_extract_pattern_vars(_tmpA46,& nv,& newdecls,_tmpA45,sc->pattern);});
# 3259
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp582=newdecls;struct Cyc_List_List*ds=_tmp582;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp583=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp58=_tmp583;struct _tuple39 _tmp584=_stmttmp58;struct Cyc_Absyn_Vardecl*_tmp586;struct Cyc_Absyn_Vardecl*_tmp585;_LLD: _tmp585=_tmp584.f1;_tmp586=_tmp584.f2;_LLE: {struct Cyc_Absyn_Vardecl*oldv=_tmp585;struct Cyc_Absyn_Vardecl*newv=_tmp586;
({struct Cyc_List_List*_tmpA48=({struct Cyc_List_List*_tmp588=_region_malloc(rgn,sizeof(*_tmp588));({struct _tuple39*_tmpA47=({struct _tuple39*_tmp587=_region_malloc(rgn,sizeof(*_tmp587));_tmp587->f1=oldv,_tmp587->f2=newv;_tmp587;});_tmp588->hd=_tmpA47;}),_tmp588->tl=*decls;_tmp588;});*decls=_tmpA48;});
({struct Cyc_Absyn_Exp*_tmpA49=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->tag=4U,_tmp589->f1=newv;_tmp589;}),0U);oldv->initializer=_tmpA49;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmpA4B=({struct Cyc_Absyn_Decl*_tmp58B=_cycalloc(sizeof(*_tmp58B));({void*_tmpA4A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->tag=0U,_tmp58A->f1=oldv;_tmp58A;});_tmp58B->r=_tmpA4A;}),_tmp58B->loc=0U;_tmp58B;});Cyc_Absyn_decl_stmt(_tmpA4B,res,0U);});}}}
# 3268
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmpA4D=({struct Cyc_List_List*_tmp58D=_region_malloc(rgn,sizeof(*_tmp58D));({struct _tuple41*_tmpA4C=({struct _tuple41*_tmp58C=_region_malloc(rgn,sizeof(*_tmp58C));_tmp58C->f1=nv,_tmp58C->f2=code_lab,_tmp58C->f3=body;_tmp58C;});_tmp58D->hd=_tmpA4C;}),_tmp58D->tl=*bodies;_tmp58D;});*bodies=_tmpA4D;});
return res;}}}}
# 3276
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F7;_tmp7F7.tag=0U,({struct _fat_ptr _tmpA4E=({const char*_tmp590="couldn't find rhs!";_tag_fat(_tmp590,sizeof(char),19U);});_tmp7F7.f1=_tmpA4E;});_tmp7F7;});void*_tmp58E[1U];_tmp58E[0]=& _tmp58F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp58E,sizeof(void*),1U));});}default: _LL7: _tmp577=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp576)->f1;_tmp578=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp576)->f2;_tmp579=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp576)->f3;_LL8: {struct Cyc_List_List*symbolic_path=_tmp577;struct Cyc_List_List*switches=_tmp578;void*other_decision=_tmp579;
# 3279
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3281
struct Cyc_Absyn_Exp*_tmp591=({struct Cyc_List_List*_tmpA4F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmpA4F,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp591;
struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp592;
# 3284
struct _tuple38 _tmp593=Cyc_Toc_admits_switch(ss);struct _tuple38 _stmttmp59=_tmp593;struct _tuple38 _tmp594=_stmttmp59;void*_tmp596;int _tmp595;_LL10: _tmp595=_tmp594.f1;_tmp596=_tmp594.f2;_LL11: {int allows_switch=_tmp595;void*test_kind=_tmp596;
if(allows_switch > 1){
# 3288
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));
({struct Cyc_Absyn_Switch_clause*_tmpA51=({struct Cyc_Absyn_Switch_clause*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({struct Cyc_Absyn_Pat*_tmpA50=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5A6->pattern=_tmpA50;}),_tmp5A6->pat_vars=0,_tmp5A6->where_clause=0,_tmp5A6->body=res,_tmp5A6->loc=0U;_tmp5A6;});_tmp5A7->hd=_tmpA51;}),_tmp5A7->tl=0;_tmp5A7;});
# 3291
for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp597=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5A=_tmp597;struct _tuple34 _tmp598=_stmttmp5A;void*_tmp59A;void*_tmp599;_LL13: _tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;_LL14: {void*pat_test=_tmp599;void*dec_tree=_tmp59A;
# 3294
struct Cyc_Absyn_Pat*_tmp59B=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp59B;
# 3296
struct Cyc_Absyn_Stmt*_tmp59C=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp59C;
# 3298
new_lscs=({struct Cyc_List_List*_tmp59E=_cycalloc(sizeof(*_tmp59E));({struct Cyc_Absyn_Switch_clause*_tmpA52=({struct Cyc_Absyn_Switch_clause*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->pattern=case_exp,_tmp59D->pat_vars=0,_tmp59D->where_clause=0,_tmp59D->body=s,_tmp59D->loc=0U;_tmp59D;});_tmp59E->hd=_tmpA52;}),_tmp59E->tl=new_lscs;_tmp59E;});}}
# 3300
{void*_tmp59F=test_kind;struct _fat_ptr*_tmp5A0;switch(*((int*)_tmp59F)){case 1U: _LL16: _LL17:
# 3302
 LOOP1: {
void*_tmp5A1=p->r;void*_stmttmp5B=_tmp5A1;void*_tmp5A2=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A4;switch(*((int*)_tmp5A2)){case 14U: _LL1F: _tmp5A4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A2)->f2;_LL20: {struct Cyc_Absyn_Exp*e=_tmp5A4;
p=e;goto LOOP1;}case 20U: _LL21: _tmp5A3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5A2)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5A3;
p=e;goto _LL1E;}default: _LL23: _LL24:
 goto _LL1E;}_LL1E:;}
# 3308
p=Cyc_Absyn_deref_exp(({void*_tmpA53=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA53,p);}),0U);goto _LL15;case 3U: _LL18: _tmp5A0=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp59F)->f1;_LL19: {struct _fat_ptr*f=_tmp5A0;
# 3310
p=({struct Cyc_Absyn_Exp*_tmpA54=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmpA54,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2U: _LL1A: _LL1B:
 goto _LL1D;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 3314
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->tag=10U,_tmp5A5->f1=p,_tmp5A5->f2=new_lscs,_tmp5A5->f3=0;_tmp5A5;}),0U);}else{
# 3318
void*_tmp5A8=test_kind;struct Cyc_Absyn_Exp*_tmp5A9;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A8)->tag == 2U){_LL26: _tmp5A9=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5A8)->f1;_LL27: {struct Cyc_Absyn_Exp*e=_tmp5A9;
# 3320
struct Cyc_List_List*_tmp5AA=ss;struct Cyc_Tcpat_Rhs*_tmp5AC;void*_tmp5AB;if(_tmp5AA != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)((struct Cyc_List_List*)_tmp5AA)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5AA)->tl == 0){_LL2B: _tmp5AB=((struct _tuple34*)_tmp5AA->hd)->f1;_tmp5AC=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_tmp5AA->hd)->f2)->f1;_LL2C: {void*pat_test=_tmp5AB;struct Cyc_Tcpat_Rhs*rhs=_tmp5AC;
# 3326
for(0;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp5AD=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp5C=_tmp5AD;struct _tuple37*_tmp5AE=_stmttmp5C;struct Cyc_Absyn_Switch_clause*_tmp5B2;struct _fat_ptr*_tmp5B1;struct _fat_ptr*_tmp5B0;int*_tmp5AF;_LL30: _tmp5AF=(int*)& _tmp5AE->f1;_tmp5B0=_tmp5AE->f2;_tmp5B1=_tmp5AE->f3;_tmp5B2=_tmp5AE->f4;_LL31: {int*already_emitted=_tmp5AF;struct _fat_ptr*init_lab=_tmp5B0;struct _fat_ptr*code_lab=_tmp5B1;struct Cyc_Absyn_Switch_clause*sc=_tmp5B2;
struct Cyc_Absyn_Stmt*_tmp5B3=sc->body;struct Cyc_Absyn_Stmt*body=_tmp5B3;
if(body == rhs->rhs){
# 3331
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3336
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA56=rgn;struct Cyc_Absyn_Exp*_tmpA55=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA56,& nv,& newdecls,_tmpA55,sc->pattern);});
# 3340
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp5B4=newdecls;struct Cyc_List_List*ds=_tmp5B4;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp5B5=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp5D=_tmp5B5;struct _tuple39 _tmp5B6=_stmttmp5D;struct Cyc_Absyn_Vardecl*_tmp5B8;struct Cyc_Absyn_Vardecl*_tmp5B7;_LL33: _tmp5B7=_tmp5B6.f1;_tmp5B8=_tmp5B6.f2;_LL34: {struct Cyc_Absyn_Vardecl*oldv=_tmp5B7;struct Cyc_Absyn_Vardecl*newv=_tmp5B8;
({struct Cyc_List_List*_tmpA58=({struct Cyc_List_List*_tmp5BA=_region_malloc(rgn,sizeof(*_tmp5BA));({struct _tuple39*_tmpA57=({struct _tuple39*_tmp5B9=_region_malloc(rgn,sizeof(*_tmp5B9));_tmp5B9->f1=oldv,_tmp5B9->f2=newv;_tmp5B9;});_tmp5BA->hd=_tmpA57;}),_tmp5BA->tl=*decls;_tmp5BA;});*decls=_tmpA58;});
({struct Cyc_Absyn_Exp*_tmpA59=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=4U,_tmp5BB->f1=newv;_tmp5BB;}),0U);oldv->initializer=_tmpA59;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmpA5B=({struct Cyc_Absyn_Decl*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));({void*_tmpA5A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->tag=0U,_tmp5BC->f1=oldv;_tmp5BC;});_tmp5BD->r=_tmpA5A;}),_tmp5BD->loc=0U;_tmp5BD;});Cyc_Absyn_decl_stmt(_tmpA5B,r,0U);});}}}
# 3349
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3353
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmpA5D=({struct Cyc_List_List*_tmp5BF=_region_malloc(rgn,sizeof(*_tmp5BF));({struct _tuple41*_tmpA5C=({struct _tuple41*_tmp5BE=_region_malloc(rgn,sizeof(*_tmp5BE));_tmp5BE->f1=nv,_tmp5BE->f2=code_lab,_tmp5BE->f3=body;_tmp5BE;});_tmp5BF->hd=_tmpA5C;}),_tmp5BF->tl=*bodies;_tmp5BF;});*bodies=_tmpA5D;});
return r;}}}}
# 3363
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F8;_tmp7F8.tag=0U,({struct _fat_ptr _tmpA5E=({const char*_tmp5C2="couldn't find rhs!";_tag_fat(_tmp5C2,sizeof(char),19U);});_tmp7F8.f1=_tmpA5E;});_tmp7F8;});void*_tmp5C0[1U];_tmp5C0[0]=& _tmp5C1;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C0,sizeof(void*),1U));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D: _LL2E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F9;_tmp7F9.tag=0U,({struct _fat_ptr _tmpA5F=({const char*_tmp5C5="bad where clause in match compiler";_tag_fat(_tmp5C5,sizeof(char),35U);});_tmp7F9.f1=_tmpA5F;});_tmp7F9;});void*_tmp5C3[1U];_tmp5C3[0]=& _tmp5C4;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp5C3,sizeof(void*),1U));});}_LL2A:;}}else{_LL28: _LL29:
# 3368
 for(0;ss != 0;ss=ss->tl){
struct _tuple34 _tmp5C6=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp5E=_tmp5C6;struct _tuple34 _tmp5C7=_stmttmp5E;void*_tmp5C9;void*_tmp5C8;_LL36: _tmp5C8=_tmp5C7.f1;_tmp5C9=_tmp5C7.f2;_LL37: {void*pat_test=_tmp5C8;void*dec_tree=_tmp5C9;
struct Cyc_Absyn_Exp*_tmp5CA=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp5CA;
struct Cyc_Absyn_Stmt*_tmp5CB=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp5CB;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}}_LL25:;}
# 3376
return res;}}}}_LL0:;}
# 3386
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3388
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple41*_tmp5CC=(struct _tuple41*)bodies->hd;struct _tuple41*_stmttmp5F=_tmp5CC;struct _tuple41*_tmp5CD=_stmttmp5F;struct Cyc_Absyn_Stmt*_tmp5CF;struct Cyc_Toc_Env**_tmp5CE;_LL1: _tmp5CE=(struct Cyc_Toc_Env**)& _tmp5CD->f1;_tmp5CF=_tmp5CD->f3;_LL2: {struct Cyc_Toc_Env**nv=_tmp5CE;struct Cyc_Absyn_Stmt*s2=_tmp5CF;
if(s2 == s)return nv;}}
# 3394
return 0;}
# 3398
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3401
void*_tmp5D0=(void*)_check_null(e->topt);void*t=_tmp5D0;
Cyc_Toc_exp_to_c(nv,e);{
# 3404
struct _tuple30 _tmp5D1=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp60=_tmp5D1;struct _tuple30 _tmp5D2=_stmttmp60;struct Cyc_Absyn_Exp*_tmp5D4;struct _tuple1*_tmp5D3;_LL1: _tmp5D3=_tmp5D2.f1;_tmp5D4=_tmp5D2.f2;_LL2: {struct _tuple1*v=_tmp5D3;struct Cyc_Absyn_Exp*path=_tmp5D4;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5D5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D5;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5D6=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp5D6;
# 3410
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3415
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3423
{struct Cyc_List_List*_tmp5D7=lscs;struct Cyc_List_List*lscs2=_tmp5D7;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_tmp5D8=(struct _tuple37*)lscs2->hd;struct _tuple37*_stmttmp61=_tmp5D8;struct _tuple37*_tmp5D9=_stmttmp61;struct Cyc_Absyn_Switch_clause*_tmp5DB;struct _fat_ptr*_tmp5DA;_LL4: _tmp5DA=_tmp5D9->f3;_tmp5DB=_tmp5D9->f4;_LL5: {struct _fat_ptr*body_lab=_tmp5DA;struct Cyc_Absyn_Switch_clause*body_sc=_tmp5DB;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3427
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3430
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5DC=*envp;struct Cyc_Toc_Env*env=_tmp5DC;
# 3433
if(lscs2->tl != 0){
struct _tuple37*_tmp5DD=(struct _tuple37*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple37*_stmttmp62=_tmp5DD;struct _tuple37*_tmp5DE=_stmttmp62;struct Cyc_Absyn_Switch_clause*_tmp5E0;struct _fat_ptr*_tmp5DF;_LL7: _tmp5DF=_tmp5DE->f3;_tmp5E0=_tmp5DE->f4;_LL8: {struct _fat_ptr*fallthru_lab=_tmp5DF;struct Cyc_Absyn_Switch_clause*next_sc=_tmp5E0;
# 3437
struct Cyc_Toc_Env**_tmp5E1=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp5E1;
# 3442
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmpA60=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA60,s);});else{
# 3446
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmpA61=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*l,struct Cyc_Absyn_Vardecl*k))Cyc_List_assoc,_tmpA61,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs));});}
# 3451
({struct Cyc_Toc_Env*_tmpA62=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmpA62,s);});}}}else{
# 3455
({struct Cyc_Toc_Env*_tmpA63=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA63,s);});}}}}}{
# 3459
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA65=test_tree;Cyc_Absyn_seq_stmt(_tmpA65,({struct _fat_ptr*_tmpA64=end_l;Cyc_Absyn_label_stmt(_tmpA64,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3461
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp5E2=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp63=_tmp5E2;struct _tuple39 _tmp5E3=_stmttmp63;struct Cyc_Absyn_Vardecl*_tmp5E4;_LLA: _tmp5E4=_tmp5E3.f2;_LLB: {struct Cyc_Absyn_Vardecl*vd=_tmp5E4;
res=({struct Cyc_Absyn_Decl*_tmpA66=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->tag=0U,_tmp5E5->f1=vd;_tmp5E5;}),0U);Cyc_Absyn_decl_stmt(_tmpA66,res,0U);});}}
# 3466
({void*_tmpA6A=({struct _tuple1*_tmpA69=v;void*_tmpA68=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA67=e;Cyc_Absyn_declare_stmt(_tmpA69,_tmpA68,_tmpA67,res,0U);})->r;whole_s->r=_tmpA6A;});
_npop_handler(0U);return;}}
# 3407
;_pop_region();}}}
# 3472
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3474
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3479
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3481
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5E6[1U];({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Absyn_uint_exp((unsigned)(n - 1),0U);_tmp5E6[0]=_tmpA6B;});({struct Cyc_Absyn_Exp*_tmpA6C=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA6C,_tag_fat(_tmp5E6,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);}
# 3484
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA6E=({struct Cyc_Absyn_Stmt*_tmpA6D=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA6D,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA6E;});}
# 3489
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3491
while(1){
void*_tmp5E7=s->r;void*_stmttmp64=_tmp5E7;void*_tmp5E8=_stmttmp64;void*_tmp5EB;struct Cyc_List_List*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5E9;struct Cyc_Absyn_Stmt*_tmp5ED;struct Cyc_Absyn_Decl*_tmp5EC;struct Cyc_Absyn_Switch_clause**_tmp5EF;struct Cyc_List_List*_tmp5EE;void*_tmp5F2;struct Cyc_List_List*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Stmt*_tmp5F4;struct _fat_ptr*_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Stmt*_tmp5F7;struct Cyc_Absyn_Stmt*_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Exp*_tmp602;struct Cyc_Absyn_Exp*_tmp603;switch(*((int*)_tmp5E8)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp603=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp603;
Cyc_Toc_exp_to_c(nv,e);return;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1 == 0){_LL5: _LL6:
({int _tmpA6F=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA6F,s);});return;}else{_LL7: _tmp602=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp602;
# 3498
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp604=Cyc_Toc_temp_var();struct _tuple1*x=_tmp604;
struct Cyc_Absyn_Stmt*_tmp605=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp605;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmpA70=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmpA70;});}
# 3507
return;}}}case 2U: _LL9: _tmp600=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp601=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_LLA: {struct Cyc_Absyn_Stmt*s1=_tmp600;struct Cyc_Absyn_Stmt*s2=_tmp601;
# 3509
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4U: _LLB: _tmp5FD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5FE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_tmp5FF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5FD;struct Cyc_Absyn_Stmt*s1=_tmp5FE;struct Cyc_Absyn_Stmt*s2=_tmp5FF;
# 3513
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9U: _LLD: _tmp5F9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5FA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2).f1;_tmp5FB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f3).f1;_tmp5FC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp5F9;struct Cyc_Absyn_Exp*e2=_tmp5FA;struct Cyc_Absyn_Exp*e3=_tmp5FB;struct Cyc_Absyn_Stmt*s2=_tmp5FC;
# 3518
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5F7=s2;_tmp5F8=e3;goto _LL10;}case 14U: _LLF: _tmp5F7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5F8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5F7;struct Cyc_Absyn_Exp*e=_tmp5F8;
_tmp5F5=e;_tmp5F6=s2;goto _LL12;}case 5U: _LL11: _tmp5F5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1).f1;_tmp5F6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5F5;struct Cyc_Absyn_Stmt*s2=_tmp5F6;
# 3523
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp606=_new_region("temp");struct _RegionHandle*temp=& _tmp606;_push_region(temp);({struct Cyc_Toc_Env*_tmpA71=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA71,s2);});
_npop_handler(0U);return;
# 3524
;_pop_region();}}case 6U: _LL13: _LL14: {
# 3527
struct Cyc_Toc_Env*_tmp607=nv;struct _fat_ptr**_tmp608;_LL24: _tmp608=_tmp607->break_lab;_LL25: {struct _fat_ptr**b=_tmp608;
if(b != 0)
({void*_tmpA72=Cyc_Toc_goto_stmt_r(*b);s->r=_tmpA72;});
# 3531
({int _tmpA73=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA73,s);});
return;}}case 7U: _LL15: _LL16: {
# 3534
struct Cyc_Toc_Env*_tmp609=nv;struct _fat_ptr**_tmp60A;_LL27: _tmp60A=_tmp609->continue_lab;_LL28: {struct _fat_ptr**c=_tmp60A;
if(c != 0)
({void*_tmpA74=Cyc_Toc_goto_stmt_r(*c);s->r=_tmpA74;});
goto _LL18;}}case 8U: _LL17: _LL18:
# 3539
({int _tmpA75=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA75,s);});
return;case 13U: _LL19: _tmp5F3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5F4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_LL1A: {struct _fat_ptr*lab=_tmp5F3;struct Cyc_Absyn_Stmt*s1=_tmp5F4;
s=s1;continue;}case 10U: _LL1B: _tmp5F0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5F1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_tmp5F2=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f3;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5F0;struct Cyc_List_List*scs=_tmp5F1;void*dec_tree_opt=_tmp5F2;
# 3543
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11U: _LL1D: _tmp5EE=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5EF=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_LL1E: {struct Cyc_List_List*es=_tmp5EE;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5EF;
# 3546
struct Cyc_Toc_Env*_tmp60B=nv;struct Cyc_Toc_FallthruInfo*_tmp60C;_LL2A: _tmp60C=_tmp60B->fallthru_info;_LL2B: {struct Cyc_Toc_FallthruInfo*fi=_tmp60C;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FA;_tmp7FA.tag=0U,({struct _fat_ptr _tmpA76=({const char*_tmp60F="fallthru in unexpected place";_tag_fat(_tmp60F,sizeof(char),29U);});_tmp7FA.f1=_tmpA76;});_tmp7FA;});void*_tmp60D[1U];_tmp60D[0]=& _tmp60E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp60D,sizeof(void*),1U));});{
struct Cyc_Toc_FallthruInfo _tmp610=*fi;struct Cyc_Toc_FallthruInfo _stmttmp65=_tmp610;struct Cyc_Toc_FallthruInfo _tmp611=_stmttmp65;struct Cyc_List_List*_tmp613;struct _fat_ptr*_tmp612;_LL2D: _tmp612=_tmp611.label;_tmp613=_tmp611.binders;_LL2E: {struct _fat_ptr*l=_tmp612;struct Cyc_List_List*vs=_tmp613;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3552
({int _tmpA77=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA77,s2);});{
struct Cyc_List_List*_tmp614=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp614;
struct Cyc_List_List*_tmp615=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp615;
for(0;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmpA79=({struct Cyc_Absyn_Exp*_tmpA78=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=5U,_tmp616->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp616;}),0U);Cyc_Absyn_assign_stmt(_tmpA78,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA79,s2,0U);});}
# 3560
s->r=s2->r;
return;}}}}}case 12U: _LL1F: _tmp5EC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5ED=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp5EC;struct Cyc_Absyn_Stmt*s1=_tmp5ED;
# 3566
{void*_tmp617=d->r;void*_stmttmp66=_tmp617;void*_tmp618=_stmttmp66;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Vardecl*_tmp61A;struct Cyc_Absyn_Tvar*_tmp619;struct Cyc_Absyn_Fndecl*_tmp61C;struct Cyc_List_List*_tmp61D;void*_tmp620;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Pat*_tmp61E;struct Cyc_Absyn_Vardecl*_tmp621;switch(*((int*)_tmp618)){case 0U: _LL30: _tmp621=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp618)->f1;_LL31: {struct Cyc_Absyn_Vardecl*vd=_tmp621;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2U: _LL32: _tmp61E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp618)->f1;_tmp61F=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp618)->f3;_tmp620=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp618)->f4;_LL33: {struct Cyc_Absyn_Pat*p=_tmp61E;struct Cyc_Absyn_Exp*e=_tmp61F;void*dec_tree=_tmp620;
# 3574
{void*_tmp622=p->r;void*_stmttmp67=_tmp622;void*_tmp623=_stmttmp67;struct Cyc_Absyn_Vardecl*_tmp624;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp623)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp623)->f2)->r)->tag == 0U){_LL3D: _tmp624=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp623)->f1;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp624;
# 3576
struct Cyc_Absyn_Vardecl*_tmp625=({struct _tuple1*_tmpA7B=Cyc_Toc_temp_var();void*_tmpA7A=vd->type;Cyc_Absyn_new_vardecl(0U,_tmpA7B,_tmpA7A,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp625;
({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tag=4U,_tmp626->f1=new_vd;_tmp626;}),0U);vd->initializer=_tmpA7C;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmpA83=({struct Cyc_Absyn_Decl*_tmpA82=({void*_tmpA7D=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=0U,_tmp627->f1=new_vd;_tmp627;});Cyc_Absyn_new_decl(_tmpA7D,s->loc);});struct Cyc_Absyn_Stmt*_tmpA81=({
struct Cyc_Absyn_Decl*_tmpA80=({void*_tmpA7E=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=0U,_tmp628->f1=vd;_tmp628;});Cyc_Absyn_new_decl(_tmpA7E,s->loc);});struct Cyc_Absyn_Stmt*_tmpA7F=s1;Cyc_Absyn_decl_stmt(_tmpA80,_tmpA7F,s->loc);});
# 3579
Cyc_Absyn_decl_stmt(_tmpA82,_tmpA81,s->loc);})->r;s->r=_tmpA83;});
# 3582
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F: _LL40:
# 3588
({void*_tmpA84=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmpA84;});
goto _LL3C;}_LL3C:;}
# 3591
goto _LL2F;}case 3U: _LL34: _tmp61D=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp618)->f1;_LL35: {struct Cyc_List_List*vds=_tmp61D;
# 3595
struct Cyc_List_List*_tmp629=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp629;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FB;_tmp7FB.tag=0U,({struct _fat_ptr _tmpA85=({const char*_tmp62C="empty Letv_d";_tag_fat(_tmp62C,sizeof(char),13U);});_tmp7FB.f1=_tmpA85;});_tmp7FB;});void*_tmp62A[1U];_tmp62A[0]=& _tmp62B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp62A,sizeof(void*),1U));});
({void*_tmpA86=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->tag=0U,_tmp62D->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp62D;});d->r=_tmpA86;});
rvds=rvds->tl;
for(0;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp62E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=0U,_tmp62F->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp62F;}),0U);struct Cyc_Absyn_Decl*d2=_tmp62E;
({void*_tmpA88=({struct Cyc_Absyn_Decl*_tmpA87=d2;Cyc_Absyn_decl_stmt(_tmpA87,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA88;});}
# 3604
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1U: _LL36: _tmp61C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp618)->f1;_LL37: {struct Cyc_Absyn_Fndecl*fd=_tmp61C;
# 3607
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4U: _LL38: _tmp619=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp618)->f1;_tmp61A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp618)->f2;_tmp61B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp618)->f3;_LL39: {struct Cyc_Absyn_Tvar*tv=_tmp619;struct Cyc_Absyn_Vardecl*vd=_tmp61A;struct Cyc_Absyn_Exp*open_exp_opt=_tmp61B;
# 3611
struct Cyc_Absyn_Stmt*_tmp630=s1;struct Cyc_Absyn_Stmt*body=_tmp630;
# 3613
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp631=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp68=_tmp631;struct _tuple30 _tmp632=_stmttmp68;struct Cyc_Absyn_Exp*_tmp634;struct _tuple1*_tmp633;_LL42: _tmp633=_tmp632.f1;_tmp634=_tmp632.f2;_LL43: {struct _tuple1*rh_var=_tmp633;struct Cyc_Absyn_Exp*rh_exp=_tmp634;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3619
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Absyn_no_regions)
({void*_tmpA8C=({struct _tuple1*_tmpA8B=x_var;void*_tmpA8A=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA89=
Cyc_Absyn_uint_exp(0U,0U);
# 3621
Cyc_Absyn_declare_stmt(_tmpA8B,_tmpA8A,_tmpA89,body,0U);})->r;s->r=_tmpA8C;});else{
# 3623
if((unsigned)open_exp_opt){
# 3625
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA8E=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA8E,({struct _fat_ptr*_tmp636=_cycalloc(sizeof(*_tmp636));({struct _fat_ptr _tmpA8D=({const char*_tmp635="h";_tag_fat(_tmp635,sizeof(char),2U);});*_tmp636=_tmpA8D;});_tmp636;}),0U);}),0U);
({void*_tmpA8F=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA8F;});}}else{
# 3634
({void*_tmpA9C=({struct _tuple1*_tmpA9B=rh_var;void*_tmpA9A=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA99=({struct Cyc_Absyn_Exp*_tmp637[1U];({struct Cyc_Absyn_Exp*_tmpA90=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp637[0]=_tmpA90;});({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA91,_tag_fat(_tmp637,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3634
Cyc_Absyn_declare_stmt(_tmpA9B,_tmpA9A,_tmpA99,({
# 3636
struct _tuple1*_tmpA98=x_var;void*_tmpA97=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA96=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA98,_tmpA97,_tmpA96,({
struct Cyc_Absyn_Stmt*_tmpA95=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp638[1U];_tmp638[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA92=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA92,_tag_fat(_tmp638,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);Cyc_Absyn_seq_stmt(_tmpA95,({
struct Cyc_Absyn_Stmt*_tmpA94=body;Cyc_Absyn_seq_stmt(_tmpA94,
Cyc_Absyn_exp_stmt(({void*_tmp639=0U;({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA93,_tag_fat(_tmp639,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3634
s->r=_tmpA9C;});}}
# 3642
return;}}default: _LL3A: _LL3B:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7FC;_tmp7FC.tag=0U,({struct _fat_ptr _tmpA9D=({const char*_tmp63C="bad nested declaration within function";_tag_fat(_tmp63C,sizeof(char),39U);});_tmp7FC.f1=_tmpA9D;});_tmp7FC;});void*_tmp63A[1U];_tmp63A[0]=& _tmp63B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp63A,sizeof(void*),1U));});}_LL2F:;}
# 3645
return;}default: _LL21: _tmp5E9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f1;_tmp5EA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f2;_tmp5EB=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E8)->f3;_LL22: {struct Cyc_Absyn_Stmt*body=_tmp5E9;struct Cyc_List_List*scs=_tmp5EA;void*dec_tree=_tmp5EB;
# 3659 "toc.cyc"
struct _tuple30 _tmp63D=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp69=_tmp63D;struct _tuple30 _tmp63E=_stmttmp69;struct Cyc_Absyn_Exp*_tmp640;struct _tuple1*_tmp63F;_LL45: _tmp63F=_tmp63E.f1;_tmp640=_tmp63E.f2;_LL46: {struct _tuple1*h_var=_tmp63F;struct Cyc_Absyn_Exp*h_exp=_tmp640;
struct _tuple30 _tmp641=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6A=_tmp641;struct _tuple30 _tmp642=_stmttmp6A;struct Cyc_Absyn_Exp*_tmp644;struct _tuple1*_tmp643;_LL48: _tmp643=_tmp642.f1;_tmp644=_tmp642.f2;_LL49: {struct _tuple1*e_var=_tmp643;struct Cyc_Absyn_Exp*e_exp=_tmp644;
struct _tuple30 _tmp645=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp6B=_tmp645;struct _tuple30 _tmp646=_stmttmp6B;struct Cyc_Absyn_Exp*_tmp648;struct _tuple1*_tmp647;_LL4B: _tmp647=_tmp646.f1;_tmp648=_tmp646.f2;_LL4C: {struct _tuple1*was_thrown_var=_tmp647;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp648;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3666
e_exp->topt=e_typ;{
struct _RegionHandle _tmp649=_new_region("temp");struct _RegionHandle*temp=& _tmp649;_push_region(temp);
# 3669
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp64A=({struct Cyc_Absyn_Stmt*_tmpA9F=body;Cyc_Absyn_seq_stmt(_tmpA9F,
Cyc_Absyn_exp_stmt(({void*_tmp653=0U;({struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA9E,_tag_fat(_tmp653,sizeof(struct Cyc_Absyn_Exp*),0U));});}),0U),0U);});
# 3670
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp64A;
# 3673
struct Cyc_Absyn_Stmt*_tmp64B=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->tag=10U,_tmp652->f1=e_exp,_tmp652->f2=scs,_tmp652->f3=dec_tree;_tmp652;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp64B;
# 3675
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3678
struct Cyc_Absyn_Exp*_tmp64C=({struct Cyc_Absyn_Exp*_tmp651[1U];({struct Cyc_Absyn_Exp*_tmpAA0=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp651[0]=_tmpAA0;});({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpAA1,_tag_fat(_tmp651,sizeof(struct Cyc_Absyn_Exp*),1U));});});
# 3678
struct Cyc_Absyn_Exp*setjmp_call=_tmp64C;
# 3680
struct Cyc_Absyn_Stmt*_tmp64D=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp650[1U];({struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Absyn_address_exp(h_exp,0U);_tmp650[0]=_tmpAA2;});({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpAA3,_tag_fat(_tmp650,sizeof(struct Cyc_Absyn_Exp*),1U));});}),0U);
# 3680
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp64D;
# 3682
struct Cyc_Absyn_Exp*_tmp64E=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp64E;
struct Cyc_Absyn_Exp*_tmp64F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp64F;
({void*_tmpAB3=({
struct _tuple1*_tmpAB2=h_var;void*_tmpAB1=h_typ;Cyc_Absyn_declare_stmt(_tmpAB2,_tmpAB1,0,({
struct Cyc_Absyn_Stmt*_tmpAB0=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpAB0,({
struct _tuple1*_tmpAAF=was_thrown_var;void*_tmpAAE=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpAAD=zero_exp;Cyc_Absyn_declare_stmt(_tmpAAF,_tmpAAE,_tmpAAD,({
struct Cyc_Absyn_Stmt*_tmpAAC=({struct Cyc_Absyn_Exp*_tmpAA5=setjmp_call;struct Cyc_Absyn_Stmt*_tmpAA4=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3688
Cyc_Absyn_ifthenelse_stmt(_tmpAA5,_tmpAA4,
# 3690
Cyc_Toc_skip_stmt_dl(),0U);});
# 3688
Cyc_Absyn_seq_stmt(_tmpAAC,({
# 3691
struct Cyc_Absyn_Exp*_tmpAAB=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpAAA=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpAAB,_tmpAAA,({
# 3693
struct _tuple1*_tmpAA9=e_var;void*_tmpAA8=e_typ;struct Cyc_Absyn_Exp*_tmpAA7=({
void*_tmpAA6=e_typ;Cyc_Toc_cast_it(_tmpAA6,Cyc_Toc_get_exn_thrown_expression());});
# 3693
Cyc_Absyn_declare_stmt(_tmpAA9,_tmpAA8,_tmpAA7,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3684
s->r=_tmpAB3;});}}
# 3697
_npop_handler(0U);return;
# 3667
;_pop_region();}}}}}}_LL0:;}}
# 3706
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpAB4=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpAB4;});
({void*_tmpAB5=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpAB5;});{
struct _RegionHandle _tmp654=_new_region("frgn");struct _RegionHandle*frgn=& _tmp654;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp655=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp655;
{struct Cyc_List_List*_tmp656=(f->i).args;struct Cyc_List_List*args=_tmp656;for(0;args != 0;args=args->tl){
struct _tuple1*_tmp657=({struct _tuple1*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->f1=Cyc_Absyn_Loc_n,_tmp658->f2=(*((struct _tuple9*)args->hd)).f1;_tmp658;});struct _tuple1*x=_tmp657;
({void*_tmpAB6=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAB6;});}}
# 3723
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3727
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp659=_cycalloc(sizeof(*_tmp659));({struct Cyc_Hashtable_Table*_tmpAB7=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp659=_tmpAB7;});_tmp659;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp65A=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6C=_tmp65A;struct Cyc_Absyn_VarargInfo _tmp65B=_stmttmp6C;int _tmp65F;void*_tmp65E;struct Cyc_Absyn_Tqual _tmp65D;struct _fat_ptr*_tmp65C;_LL1: _tmp65C=_tmp65B.name;_tmp65D=_tmp65B.tq;_tmp65E=_tmp65B.type;_tmp65F=_tmp65B.inject;_LL2: {struct _fat_ptr*n=_tmp65C;struct Cyc_Absyn_Tqual tq=_tmp65D;void*t=_tmp65E;int i=_tmp65F;
void*_tmp660=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type));void*t2=_tmp660;
struct _tuple1*_tmp661=({struct _tuple1*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->f1=Cyc_Absyn_Loc_n,_tmp664->f2=(struct _fat_ptr*)_check_null(n);_tmp664;});struct _tuple1*x2=_tmp661;
({struct Cyc_List_List*_tmpABA=({struct Cyc_List_List*_tmpAB9=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAB9,({struct Cyc_List_List*_tmp663=_cycalloc(sizeof(*_tmp663));({struct _tuple9*_tmpAB8=({struct _tuple9*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->f1=n,_tmp662->f2=tq,_tmp662->f3=t2;_tmp662;});_tmp663->hd=_tmpAB8;}),_tmp663->tl=0;_tmp663;}));});(f->i).args=_tmpABA;});
(f->i).cyc_varargs=0;}}
# 3736
{struct Cyc_List_List*_tmp665=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp665;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpABB=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpABB;});}}
({struct Cyc_Toc_Env*_tmpABC=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpABC,f->body);});}
# 3716
;_pop_region();}}
# 3741
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp666=s;switch(_tmp666){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3757 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3759
struct _tuple42 _tmp667=*env;struct _tuple42 _stmttmp6D=_tmp667;struct _tuple42 _tmp668=_stmttmp6D;struct Cyc_Absyn_Aggrdecl*_tmp66A;struct Cyc_Toc_TocState*_tmp669;_LL1: _tmp669=_tmp668.f1;_tmp66A=*_tmp668.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp669;struct Cyc_Absyn_Aggrdecl*ad=_tmp66A;
struct _tuple1*_tmp66B=ad->name;struct _tuple1*n=_tmp66B;
struct Cyc_Toc_TocState _tmp66C=*s;struct Cyc_Toc_TocState _stmttmp6E=_tmp66C;struct Cyc_Toc_TocState _tmp66D=_stmttmp6E;struct Cyc_Dict_Dict*_tmp66E;_LL4: _tmp66E=_tmp66D.aggrs_so_far;_LL5: {struct Cyc_Dict_Dict*aggrs_so_far=_tmp66E;
int seen_defn_before;
struct _tuple19**_tmp66F=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple19**dopt=_tmp66F;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple19*v;
if((int)ad->kind == (int)Cyc_Absyn_StructA)
v=({struct _tuple19*_tmp670=_region_malloc(d,sizeof(*_tmp670));_tmp670->f1=ad,({void*_tmpABD=Cyc_Absyn_strctq(n);_tmp670->f2=_tmpABD;});_tmp670;});else{
# 3770
v=({struct _tuple19*_tmp671=_region_malloc(d,sizeof(*_tmp671));_tmp671->f1=ad,({void*_tmpABE=Cyc_Absyn_unionq_type(n);_tmp671->f2=_tmpABE;});_tmp671;});}
({struct Cyc_Dict_Dict _tmpABF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpABF;});}}else{
# 3773
struct _tuple19*_tmp672=*dopt;struct _tuple19*_stmttmp6F=_tmp672;struct _tuple19*_tmp673=_stmttmp6F;void*_tmp675;struct Cyc_Absyn_Aggrdecl*_tmp674;_LL7: _tmp674=_tmp673->f1;_tmp675=_tmp673->f2;_LL8: {struct Cyc_Absyn_Aggrdecl*ad2=_tmp674;void*t=_tmp675;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpAC2=({struct Cyc_Dict_Dict _tmpAC1=*aggrs_so_far;struct _tuple1*_tmpAC0=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpAC1,_tmpAC0,({struct _tuple19*_tmp676=_region_malloc(d,sizeof(*_tmp676));_tmp676->f1=ad,_tmp676->f2=t;_tmp676;}));});*aggrs_so_far=_tmpAC2;});
seen_defn_before=0;}else{
# 3778
seen_defn_before=1;}}}{
# 3780
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->kind=ad->kind,_tmp69B->sc=Cyc_Absyn_Public,_tmp69B->name=ad->name,_tmp69B->tvs=0,_tmp69B->impl=0,_tmp69B->expected_mem_kind=0,_tmp69B->attributes=ad->attributes;_tmp69B;});
# 3787
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpAC3=({struct Cyc_Absyn_AggrdeclImpl*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->exist_vars=0,_tmp677->rgn_po=0,_tmp677->fields=0,_tmp677->tagged=0;_tmp677;});new_ad->impl=_tmpAC3;});{
# 3792
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp678=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp678;for(0;fields != 0;fields=fields->tl){
# 3796
struct Cyc_Absyn_Aggrfield*_tmp679=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp679;
void*_tmp67A=old_field->type;void*old_type=_tmp67A;
struct Cyc_List_List*_tmp67B=old_field->attributes;struct Cyc_List_List*old_atts=_tmp67B;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3810 "toc.cyc"
void*_tmp67C=Cyc_Tcutil_compress(old_type);void*_stmttmp70=_tmp67C;void*_tmp67D=_stmttmp70;unsigned _tmp681;void*_tmp680;struct Cyc_Absyn_Tqual _tmp67F;void*_tmp67E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67D)->tag == 4U){_LLA: _tmp67E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67D)->f1).elt_type;_tmp67F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67D)->f1).tq;_tmp680=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67D)->f1).zero_term;_tmp681=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67D)->f1).zt_loc;_LLB: {void*et=_tmp67E;struct Cyc_Absyn_Tqual tq=_tmp67F;void*zt=_tmp680;unsigned ztl=_tmp681;
# 3813
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->tag=4U,(_tmp682->f1).elt_type=et,(_tmp682->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_uint_exp(0U,0U);(_tmp682->f1).num_elts=_tmpAC4;}),(_tmp682->f1).zero_term=zt,(_tmp682->f1).zt_loc=ztl;_tmp682;});
goto _LL9;}}else{_LLC: _LLD:
# 3816
 old_atts=({struct Cyc_List_List*_tmp684=_cycalloc(sizeof(*_tmp684));({void*_tmpAC5=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->tag=6U,_tmp683->f1=0;_tmp683;});_tmp684->hd=_tmpAC5;}),_tmp684->tl=old_atts;_tmp684;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->tag=4U,({void*_tmpAC8=Cyc_Toc_void_star_type();(_tmp685->f1).elt_type=_tmpAC8;}),({
struct Cyc_Absyn_Tqual _tmpAC7=Cyc_Absyn_empty_tqual(0U);(_tmp685->f1).tq=_tmpAC7;}),({
struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Absyn_uint_exp(0U,0U);(_tmp685->f1).num_elts=_tmpAC6;}),(_tmp685->f1).zero_term=Cyc_Absyn_false_type,(_tmp685->f1).zt_loc=0U;_tmp685;});}_LL9:;}{
# 3823
struct Cyc_Absyn_Aggrfield*_tmp686=({struct Cyc_Absyn_Aggrfield*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->name=old_field->name,_tmp697->tq=Cyc_Toc_mt_tq,({
# 3825
void*_tmpAC9=Cyc_Toc_typ_to_c(old_type);_tmp697->type=_tmpAC9;}),_tmp697->width=old_field->width,_tmp697->attributes=old_atts,_tmp697->requires_clause=0;_tmp697;});
# 3823
struct Cyc_Absyn_Aggrfield*new_field=_tmp686;
# 3833
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp687=new_field->type;void*T=_tmp687;
struct _fat_ptr*_tmp688=new_field->name;struct _fat_ptr*f=_tmp688;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp694=({struct Cyc_String_pa_PrintArg_struct _tmp7FE;_tmp7FE.tag=0U,_tmp7FE.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7FE;});struct Cyc_String_pa_PrintArg_struct _tmp695=({struct Cyc_String_pa_PrintArg_struct _tmp7FD;_tmp7FD.tag=0U,_tmp7FD.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7FD;});void*_tmp692[2U];_tmp692[0]=& _tmp694,_tmp692[1]=& _tmp695;({struct _fat_ptr _tmpACA=({const char*_tmp693="_union_%s_%s";_tag_fat(_tmp693,sizeof(char),13U);});Cyc_aprintf(_tmpACA,_tag_fat(_tmp692,sizeof(void*),2U));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp691=_cycalloc(sizeof(*_tmp691));*_tmp691=s;_tmp691;});
struct Cyc_Absyn_Aggrfield*_tmp689=({struct Cyc_Absyn_Aggrfield*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->name=Cyc_Toc_val_sp,_tmp690->tq=Cyc_Toc_mt_tq,_tmp690->type=T,_tmp690->width=0,_tmp690->attributes=0,_tmp690->requires_clause=0;_tmp690;});struct Cyc_Absyn_Aggrfield*value_field=_tmp689;
struct Cyc_Absyn_Aggrfield*_tmp68A=({struct Cyc_Absyn_Aggrfield*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->name=Cyc_Toc_tag_sp,_tmp68F->tq=Cyc_Toc_mt_tq,_tmp68F->type=Cyc_Absyn_sint_type,_tmp68F->width=0,_tmp68F->attributes=0,_tmp68F->requires_clause=0;_tmp68F;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp68A;
struct Cyc_Absyn_Aggrdecl*_tmp68B=({struct _fat_ptr*_tmpACB=str;Cyc_Toc_make_c_struct_defn(_tmpACB,({struct Cyc_Absyn_Aggrfield*_tmp68E[2U];_tmp68E[0]=tag_field,_tmp68E[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp68E,sizeof(struct Cyc_Absyn_Aggrfield*),2U));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp68B;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct Cyc_Absyn_Decl*_tmpACC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->tag=5U,_tmp68C->f1=ad2;_tmp68C;}),0U);_tmp68D->hd=_tmpACC;}),_tmp68D->tl=Cyc_Toc_result_decls;_tmp68D;});
({void*_tmpACD=Cyc_Absyn_strct(str);new_field->type=_tmpACD;});}
# 3844
new_fields=({struct Cyc_List_List*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->hd=new_field,_tmp696->tl=new_fields;_tmp696;});}}}
# 3846
({struct Cyc_List_List*_tmpACE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpACE;});}}
# 3849
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));({struct Cyc_Absyn_Decl*_tmpAD0=({struct Cyc_Absyn_Decl*_tmp699=_cycalloc(sizeof(*_tmp699));({void*_tmpACF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tag=5U,_tmp698->f1=new_ad;_tmp698;});_tmp699->r=_tmpACF;}),_tmp699->loc=0U;_tmp699;});_tmp69A->hd=_tmpAD0;}),_tmp69A->tl=Cyc_Toc_result_decls;_tmp69A;});
return 0;}}}}
# 3853
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3881 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3884
struct _tuple43 _tmp69C=*env;struct _tuple43 _stmttmp71=_tmp69C;struct _tuple43 _tmp69D=_stmttmp71;struct Cyc_Absyn_Datatypedecl*_tmp69F;struct Cyc_Set_Set**_tmp69E;_LL1: _tmp69E=(_tmp69D.f1)->datatypes_so_far;_tmp69F=_tmp69D.f2;_LL2: {struct Cyc_Set_Set**datatypes_so_far=_tmp69E;struct Cyc_Absyn_Datatypedecl*tud=_tmp69F;
struct _tuple1*_tmp6A0=tud->name;struct _tuple1*n=_tmp6A0;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpAD1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpAD1;});
{struct Cyc_List_List*_tmp6A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp6A1;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3892
struct Cyc_List_List*_tmp6A2=0;struct Cyc_List_List*fs=_tmp6A2;
int i=1;
{struct Cyc_List_List*_tmp6A3=f->typs;struct Cyc_List_List*ts=_tmp6A3;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp6A4=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp6A4;
struct Cyc_Absyn_Aggrfield*_tmp6A5=({struct Cyc_Absyn_Aggrfield*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->name=fname,_tmp6A7->tq=(*((struct _tuple13*)ts->hd)).f1,({
void*_tmpAD2=Cyc_Toc_typ_to_c((*((struct _tuple13*)ts->hd)).f2);_tmp6A7->type=_tmpAD2;}),_tmp6A7->width=0,_tmp6A7->attributes=0,_tmp6A7->requires_clause=0;_tmp6A7;});
# 3896
struct Cyc_Absyn_Aggrfield*f=_tmp6A5;
# 3898
fs=({struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=f,_tmp6A6->tl=fs;_tmp6A6;});}}
# 3900
fs=({struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));({struct Cyc_Absyn_Aggrfield*_tmpAD4=({struct Cyc_Absyn_Aggrfield*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->name=Cyc_Toc_tag_sp,_tmp6A8->tq=Cyc_Toc_mt_tq,_tmp6A8->type=Cyc_Absyn_sint_type,_tmp6A8->width=0,_tmp6A8->attributes=0,_tmp6A8->requires_clause=0;_tmp6A8;});_tmp6A9->hd=_tmpAD4;}),({
struct Cyc_List_List*_tmpAD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fs);_tmp6A9->tl=_tmpAD3;});_tmp6A9;});{
struct Cyc_Absyn_Aggrdecl*_tmp6AA=({struct _fat_ptr*_tmpAD6=({struct _fat_ptr*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct _fat_ptr _tmpAD5=({const char*_tmp6AD="";_tag_fat(_tmp6AD,sizeof(char),1U);});*_tmp6AE=_tmpAD5;});_tmp6AE;});Cyc_Toc_make_c_struct_defn(_tmpAD6,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp6AA;
({struct _tuple1*_tmpAD7=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpAD7;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));({struct Cyc_Absyn_Decl*_tmpAD8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->tag=5U,_tmp6AB->f1=ad;_tmp6AB;}),0U);_tmp6AC->hd=_tmpAD8;}),_tmp6AC->tl=Cyc_Toc_result_decls;_tmp6AC;});}}}
# 3906
return 0;}}
# 3909
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3928 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3931
struct _tuple43 _tmp6AF=*env;struct _tuple43 _stmttmp72=_tmp6AF;struct _tuple43 _tmp6B0=_stmttmp72;struct Cyc_Absyn_Datatypedecl*_tmp6B2;struct Cyc_Toc_TocState*_tmp6B1;_LL1: _tmp6B1=_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;_LL2: {struct Cyc_Toc_TocState*s=_tmp6B1;struct Cyc_Absyn_Datatypedecl*xd=_tmp6B2;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6B3=*s;struct Cyc_Toc_TocState _stmttmp73=_tmp6B3;struct Cyc_Toc_TocState _tmp6B4=_stmttmp73;struct Cyc_Dict_Dict*_tmp6B5;_LL4: _tmp6B5=_tmp6B4.xdatatypes_so_far;_LL5: {struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp6B5;
struct _tuple1*_tmp6B6=xd->name;struct _tuple1*n=_tmp6B6;
{struct Cyc_List_List*_tmp6B7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp6B7;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B8=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp6B8;
void*_tmp6B9=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp6B9;
# 3942
int*_tmp6BA=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp74=_tmp6BA;int*_tmp6BB=_stmttmp74;if(_tmp6BB == 0){_LL7: _LL8: {
# 3944
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3948
struct Cyc_Absyn_Vardecl*_tmp6BC=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6BC;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));({struct Cyc_Absyn_Decl*_tmpAD9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->tag=0U,_tmp6BD->f1=tag_decl;_tmp6BD;}),0U);_tmp6BE->hd=_tmpAD9;}),_tmp6BE->tl=Cyc_Toc_result_decls;_tmp6BE;});
({struct Cyc_Dict_Dict _tmpADA=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3951
*xdatatypes_so_far=_tmpADA;});{
# 3953
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6BF=f->typs;struct Cyc_List_List*tqts=_tmp6BF;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp6C0=({struct _fat_ptr*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));({struct _fat_ptr _tmpADC=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C6=({struct Cyc_Int_pa_PrintArg_struct _tmp7FF;_tmp7FF.tag=1U,_tmp7FF.f1=(unsigned long)i;_tmp7FF;});void*_tmp6C4[1U];_tmp6C4[0]=& _tmp6C6;({struct _fat_ptr _tmpADB=({const char*_tmp6C5="f%d";_tag_fat(_tmp6C5,sizeof(char),4U);});Cyc_aprintf(_tmpADB,_tag_fat(_tmp6C4,sizeof(void*),1U));});});*_tmp6C7=_tmpADC;});_tmp6C7;});struct _fat_ptr*field_n=_tmp6C0;
struct Cyc_Absyn_Aggrfield*_tmp6C1=({struct Cyc_Absyn_Aggrfield*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->name=field_n,_tmp6C3->tq=(*((struct _tuple13*)tqts->hd)).f1,({
void*_tmpADD=Cyc_Toc_typ_to_c((*((struct _tuple13*)tqts->hd)).f2);_tmp6C3->type=_tmpADD;}),_tmp6C3->width=0,_tmp6C3->attributes=0,_tmp6C3->requires_clause=0;_tmp6C3;});
# 3957
struct Cyc_Absyn_Aggrfield*newf=_tmp6C1;
# 3959
fields=({struct Cyc_List_List*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2->hd=newf,_tmp6C2->tl=fields;_tmp6C2;});}}
# 3961
fields=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));({struct Cyc_Absyn_Aggrfield*_tmpAE0=({struct Cyc_Absyn_Aggrfield*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->name=Cyc_Toc_tag_sp,_tmp6C8->tq=Cyc_Toc_mt_tq,({
void*_tmpADF=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6C8->type=_tmpADF;}),_tmp6C8->width=0,_tmp6C8->attributes=0,_tmp6C8->requires_clause=0;_tmp6C8;});
# 3961
_tmp6C9->hd=_tmpAE0;}),({
# 3963
struct Cyc_List_List*_tmpADE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C9->tl=_tmpADE;});_tmp6C9;});{
struct Cyc_Absyn_Aggrdecl*_tmp6CA=({struct _fat_ptr*_tmpAE2=({struct _fat_ptr*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));({struct _fat_ptr _tmpAE1=({const char*_tmp6CD="";_tag_fat(_tmp6CD,sizeof(char),1U);});*_tmp6CE=_tmpAE1;});_tmp6CE;});Cyc_Toc_make_c_struct_defn(_tmpAE2,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp6CA;
({struct _tuple1*_tmpAE3=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpAE3;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));({struct Cyc_Absyn_Decl*_tmpAE4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->tag=5U,_tmp6CB->f1=strct_decl;_tmp6CB;}),0U);_tmp6CC->hd=_tmpAE4;}),_tmp6CC->tl=Cyc_Toc_result_decls;_tmp6CC;});
goto _LL6;}}}}}else{if(*((int*)_tmp6BB)== 0){_LL9: _LLA:
# 3969
 if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6CF=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp6CF;
struct Cyc_Absyn_Vardecl*_tmp6D0=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6D0;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));({struct Cyc_Absyn_Decl*_tmpAE5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->tag=0U,_tmp6D1->f1=tag_decl;_tmp6D1;}),0U);_tmp6D2->hd=_tmpAE5;}),_tmp6D2->tl=Cyc_Toc_result_decls;_tmp6D2;});
({struct Cyc_Dict_Dict _tmpAE6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpAE6;});}
# 3976
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 3980
return 0;}}}}
# 3983
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3987
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3993
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpAE7=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpAE7;});
# 3997
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 4006
struct _RegionHandle _tmp6D3=_new_region("temp");struct _RegionHandle*temp=& _tmp6D3;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D4=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp6D4;
Cyc_Toc_exp_to_c(nv2,init);}
# 4007
;_pop_region();}else{
# 4011
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4014
void*_tmp6D5=Cyc_Tcutil_compress(old_typ);void*_stmttmp75=_tmp6D5;void*_tmp6D6=_stmttmp75;void*_tmp6D9;struct Cyc_Absyn_Exp*_tmp6D8;void*_tmp6D7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D6)->tag == 4U){_LL1: _tmp6D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D6)->f1).elt_type;_tmp6D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D6)->f1).num_elts;_tmp6D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D6)->f1).zero_term;_LL2: {void*et=_tmp6D7;struct Cyc_Absyn_Exp*num_elts_opt=_tmp6D8;void*zt=_tmp6D9;
# 4016
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp800;_tmp800.tag=0U,({struct _fat_ptr _tmpAE8=({const char*_tmp6DC="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp6DC,sizeof(char),55U);});_tmp800.f1=_tmpAE8;});_tmp800;});void*_tmp6DA[1U];_tmp6DA[0]=& _tmp6DB;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6DA,sizeof(void*),1U));});{
struct Cyc_Absyn_Exp*num_elts=num_elts_opt;
struct Cyc_Absyn_Exp*_tmp6DD=({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpAEA,({
struct Cyc_Absyn_Exp*_tmpAE9=num_elts;Cyc_Absyn_add_exp(_tmpAE9,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4020
struct Cyc_Absyn_Exp*lhs=_tmp6DD;
# 4023
struct Cyc_Absyn_Exp*_tmp6DE=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp6DE;
({void*_tmpAEC=({struct Cyc_Absyn_Stmt*_tmpAEB=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAEB,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4024
s->r=_tmpAEC;});}}
# 4027
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4035
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6DF=d;void**_tmp6E2;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*_tmp6E0;struct Cyc_Tcpat_Rhs*_tmp6E3;switch(*((int*)_tmp6DF)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6E3=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6DF)->f1;_LL4: {struct Cyc_Tcpat_Rhs*rhs=_tmp6E3;
rhs->rhs=success;return d;}default: _LL5: _tmp6E0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DF)->f1;_tmp6E1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DF)->f2;_tmp6E2=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DF)->f3;_LL6: {struct Cyc_List_List*path=_tmp6E0;struct Cyc_List_List*sws=_tmp6E1;void**d2=_tmp6E2;
# 4040
({void*_tmpAED=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAED;});
for(0;sws != 0;sws=sws->tl){
struct _tuple34*_tmp6E4=(struct _tuple34*)sws->hd;struct _tuple34*_stmttmp76=_tmp6E4;struct _tuple34*_tmp6E5=_stmttmp76;void**_tmp6E6;_LL8: _tmp6E6=(void**)& _tmp6E5->f2;_LL9: {void**d2=_tmp6E6;
({void*_tmpAEE=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAEE;});}}
# 4045
return d;}}_LL0:;}
# 4056 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4058
struct _RegionHandle _tmp6E7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E7;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E8=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp6E8;
void*_tmp6E9=(void*)_check_null(e->topt);void*t=_tmp6E9;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4068
struct Cyc_Absyn_Stmt*_tmp6EA=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6EA;
if(dopt != 0)
# 4072
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4074
struct Cyc_Absyn_Switch_clause*_tmp6EB=({struct Cyc_Absyn_Switch_clause*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->pattern=p,_tmp6FA->pat_vars=0,_tmp6FA->where_clause=0,_tmp6FA->body=succ_stmt,_tmp6FA->loc=0U;_tmp6FA;});struct Cyc_Absyn_Switch_clause*c1=_tmp6EB;
struct Cyc_List_List*_tmp6EC=({struct _RegionHandle*_tmpAF1=rgn;struct _RegionHandle*_tmpAF0=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpAF1,Cyc_Toc_gen_labels,_tmpAF0,({struct Cyc_Absyn_Switch_clause*_tmp6F9[1U];_tmp6F9[0]=c1;({struct _RegionHandle*_tmpAEF=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpAEF,_tag_fat(_tmp6F9,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});struct Cyc_List_List*lscs=_tmp6EC;
# 4077
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4081
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4084
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple41*_tmp6ED=(struct _tuple41*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple41*_stmttmp77=_tmp6ED;struct _tuple41*_tmp6EE=_stmttmp77;struct Cyc_Absyn_Stmt*_tmp6F0;struct Cyc_Toc_Env*_tmp6EF;_LL1: _tmp6EF=_tmp6EE->f1;_tmp6F0=_tmp6EE->f3;_LL2: {struct Cyc_Toc_Env*env=_tmp6EF;struct Cyc_Absyn_Stmt*st=_tmp6F0;
if(st == succ_stmt){senv=env;goto FOUND_ENV;}}}
# 4089
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp801;_tmp801.tag=0U,({struct _fat_ptr _tmpAF2=({const char*_tmp6F3="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6F3,sizeof(char),33U);});_tmp801.f1=_tmpAF2;});_tmp801;});void*_tmp6F1[1U];_tmp6F1[0]=& _tmp6F2;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp6F1,sizeof(void*),1U));});
FOUND_ENV:
# 4093
 Cyc_Toc_stmt_to_c(senv,s);{
# 4095
struct Cyc_Absyn_Stmt*res=test_tree;
# 4097
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp6F4=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp78=_tmp6F4;struct _tuple39 _tmp6F5=_stmttmp78;struct Cyc_Absyn_Vardecl*_tmp6F6;_LL4: _tmp6F6=_tmp6F5.f2;_LL5: {struct Cyc_Absyn_Vardecl*vd=_tmp6F6;
res=({struct Cyc_Absyn_Decl*_tmpAF3=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7->tag=0U,_tmp6F7->f1=vd;_tmp6F7;}),0U);Cyc_Absyn_decl_stmt(_tmpAF3,res,0U);});}}
# 4102
res=({struct _tuple1*_tmpAF6=v;void*_tmpAF5=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpAF4=e;Cyc_Absyn_declare_stmt(_tmpAF6,_tmpAF5,_tmpAF4,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6F8=res;_npop_handler(0U);return _tmp6F8;}}}}}
# 4059
;_pop_region();}
# 4111
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6FB=e->r;void*_stmttmp79=_tmp6FB;void*_tmp6FC=_stmttmp79;struct Cyc_List_List*_tmp6FE;struct Cyc_List_List*_tmp6FD;struct Cyc_Absyn_MallocInfo*_tmp6FF;struct Cyc_Absyn_Stmt*_tmp700;void**_tmp701;void**_tmp702;struct Cyc_List_List*_tmp703;struct Cyc_List_List*_tmp704;struct Cyc_List_List*_tmp706;void**_tmp705;struct Cyc_Absyn_Exp*_tmp708;void**_tmp707;struct Cyc_List_List*_tmp70A;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_Absyn_Exp*_tmp717;struct Cyc_Absyn_Exp*_tmp716;struct Cyc_Absyn_Exp*_tmp719;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_List_List*_tmp71A;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_Absyn_Exp*_tmp71C;struct Cyc_Absyn_Exp*_tmp71D;struct Cyc_Absyn_Exp*_tmp71E;struct Cyc_Absyn_Exp*_tmp71F;struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp721;struct Cyc_Absyn_Exp*_tmp722;struct Cyc_Absyn_Exp*_tmp723;switch(*((int*)_tmp6FC)){case 41U: _LL1: _tmp723=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp723;
_tmp722=e;goto _LL4;}case 20U: _LL3: _tmp722=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp722;
_tmp721=e;goto _LL6;}case 21U: _LL5: _tmp721=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp721;
_tmp720=e;goto _LL8;}case 22U: _LL7: _tmp720=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp720;
_tmp71F=e;goto _LLA;}case 15U: _LL9: _tmp71F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp71F;
_tmp71E=e;goto _LLC;}case 11U: _LLB: _tmp71E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp71E;
_tmp71D=e;goto _LLE;}case 12U: _LLD: _tmp71D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp71D;
_tmp71C=e;goto _LL10;}case 18U: _LLF: _tmp71C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp71C;
_tmp71B=e;goto _LL12;}case 5U: _LL11: _tmp71B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp71B;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3U: _LL13: _tmp71A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL14: {struct Cyc_List_List*es=_tmp71A;
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7U: _LL15: _tmp718=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp719=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp718;struct Cyc_Absyn_Exp*e2=_tmp719;
_tmp716=e1;_tmp717=e2;goto _LL18;}case 8U: _LL17: _tmp716=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp717=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp716;struct Cyc_Absyn_Exp*e2=_tmp717;
_tmp714=e1;_tmp715=e2;goto _LL1A;}case 9U: _LL19: _tmp714=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp715=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp714;struct Cyc_Absyn_Exp*e2=_tmp715;
_tmp712=e1;_tmp713=e2;goto _LL1C;}case 23U: _LL1B: _tmp712=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp713=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp712;struct Cyc_Absyn_Exp*e2=_tmp713;
_tmp710=e1;_tmp711=e2;goto _LL1E;}case 35U: _LL1D: _tmp710=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp711=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp710;struct Cyc_Absyn_Exp*e2=_tmp711;
_tmp70E=e1;_tmp70F=e2;goto _LL20;}case 4U: _LL1F: _tmp70E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp70F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6FC)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp70E;struct Cyc_Absyn_Exp*e2=_tmp70F;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6U: _LL21: _tmp70B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp70C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_tmp70D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6FC)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp70B;struct Cyc_Absyn_Exp*e2=_tmp70C;struct Cyc_Absyn_Exp*e3=_tmp70D;
# 4130
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10U: _LL23: _tmp709=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp70A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp709;struct Cyc_List_List*es=_tmp70A;
# 4132
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14U: _LL25: _tmp707=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_tmp708=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL26: {void**t=_tmp707;struct Cyc_Absyn_Exp*e=_tmp708;
({void*_tmpAF7=Cyc_Toc_typ_to_c(*t);*t=_tmpAF7;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25U: _LL27: _tmp705=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1)->f3;_tmp706=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL28: {void**t=_tmp705;struct Cyc_List_List*dles=_tmp706;
# 4135
({void*_tmpAF8=Cyc_Toc_typ_to_c(*t);*t=_tmpAF8;});
_tmp704=dles;goto _LL2A;}case 36U: _LL29: _tmp704=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6FC)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp704;
_tmp703=dles;goto _LL2C;}case 26U: _LL2B: _tmp703=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp703;
# 4139
for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple20*)dles->hd)).f2);}
goto _LL0;}case 19U: _LL2D: _tmp702=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL2E: {void**t=_tmp702;
_tmp701=t;goto _LL30;}case 17U: _LL2F: _tmp701=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL30: {void**t=_tmp701;
({void*_tmpAF9=Cyc_Toc_typ_to_c(*t);*t=_tmpAF9;});goto _LL0;}case 37U: _LL31: _tmp700=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL32: {struct Cyc_Absyn_Stmt*s=_tmp700;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34U: _LL33: _tmp6FF=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6FC)->f1;_LL34: {struct Cyc_Absyn_MallocInfo*m=_tmp6FF;
# 4146
if(m->elt_type != 0)
({void**_tmpAFB=({void**_tmp724=_cycalloc(sizeof(*_tmp724));({void*_tmpAFA=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp724=_tmpAFA;});_tmp724;});m->elt_type=_tmpAFB;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40U: _LL35: _tmp6FD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FC)->f3;_tmp6FE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6FC)->f4;_LL36: {struct Cyc_List_List*o=_tmp6FD;struct Cyc_List_List*i=_tmp6FE;
# 4151
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0U: _LL37: _LL38:
 goto _LL3A;case 1U: _LL39: _LL3A:
 goto _LL3C;case 32U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 42U: _LL3F: _LL40:
# 4159
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
({void*_tmp725=0U;({unsigned _tmpAFD=e->loc;struct _fat_ptr _tmpAFC=({const char*_tmp726="Cyclone expression in C code";_tag_fat(_tmp726,sizeof(char),29U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAFD,_tmpAFC,_tag_fat(_tmp725,sizeof(void*),0U));});});}_LL0:;}
# 4174
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp727=d->r;void*_stmttmp7A=_tmp727;void*_tmp728=_stmttmp7A;struct Cyc_Absyn_Typedefdecl*_tmp729;struct Cyc_Absyn_Enumdecl*_tmp72A;struct Cyc_Absyn_Aggrdecl*_tmp72B;struct Cyc_Absyn_Fndecl*_tmp72C;struct Cyc_Absyn_Vardecl*_tmp72D;switch(*((int*)_tmp728)){case 0U: _LL1: _tmp72D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp728)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp72D;
# 4177
({void*_tmpAFE=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpAFE;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp72C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp728)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp72C;
# 4181
({void*_tmpAFF=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpAFF;});
{struct Cyc_List_List*_tmp72E=(fd->i).args;struct Cyc_List_List*args=_tmp72E;for(0;args != 0;args=args->tl){
({void*_tmpB00=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpB00;});}}
goto _LL0;}case 5U: _LL5: _tmp72B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp728)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp72B;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7U: _LL7: _tmp72A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp728)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*ed=_tmp72A;
# 4187
if(ed->fields != 0){
struct Cyc_List_List*_tmp72F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp72F;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp730=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp730;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4192
goto _LL0;}case 8U: _LL9: _tmp729=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp728)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp729;
({void*_tmpB01=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB01;});goto _LL0;}case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4202
({void*_tmp731=0U;({unsigned _tmpB03=d->loc;struct _fat_ptr _tmpB02=({const char*_tmp732="Cyclone declaration in C code";_tag_fat(_tmp732,sizeof(char),30U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB03,_tmpB02,_tag_fat(_tmp731,sizeof(void*),0U));});});case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 4210
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp733=s->r;void*_stmttmp7B=_tmp733;void*_tmp734=_stmttmp7B;struct Cyc_Absyn_Stmt*_tmp735;struct Cyc_Absyn_Exp*_tmp737;struct Cyc_Absyn_Stmt*_tmp736;struct Cyc_Absyn_Stmt*_tmp739;struct Cyc_Absyn_Decl*_tmp738;void*_tmp73C;struct Cyc_List_List*_tmp73B;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Stmt*_tmp740;struct Cyc_Absyn_Exp*_tmp73F;struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Exp*_tmp73D;struct Cyc_Absyn_Stmt*_tmp742;struct Cyc_Absyn_Exp*_tmp741;struct Cyc_Absyn_Stmt*_tmp745;struct Cyc_Absyn_Stmt*_tmp744;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_Absyn_Exp*_tmp746;struct Cyc_Absyn_Stmt*_tmp748;struct Cyc_Absyn_Stmt*_tmp747;struct Cyc_Absyn_Exp*_tmp749;switch(*((int*)_tmp734)){case 1U: _LL1: _tmp749=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp749;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2U: _LL3: _tmp747=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_tmp748=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp734)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp747;struct Cyc_Absyn_Stmt*s2=_tmp748;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3U: _LL5: _tmp746=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_LL6: {struct Cyc_Absyn_Exp*eopt=_tmp746;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4U: _LL7: _tmp743=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_tmp744=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp734)->f2;_tmp745=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp734)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp743;struct Cyc_Absyn_Stmt*s1=_tmp744;struct Cyc_Absyn_Stmt*s2=_tmp745;
# 4216
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5U: _LL9: _tmp741=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp734)->f1).f1;_tmp742=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp734)->f2;_LLA: {struct Cyc_Absyn_Exp*e=_tmp741;struct Cyc_Absyn_Stmt*s=_tmp742;
# 4218
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9U: _LLB: _tmp73D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_tmp73E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp734)->f2).f1;_tmp73F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp734)->f3).f1;_tmp740=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp734)->f4;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp73D;struct Cyc_Absyn_Exp*e2=_tmp73E;struct Cyc_Absyn_Exp*e3=_tmp73F;struct Cyc_Absyn_Stmt*s=_tmp740;
# 4220
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10U: _LLD: _tmp73A=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_tmp73B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp734)->f2;_tmp73C=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp734)->f3;_LLE: {struct Cyc_Absyn_Exp*e=_tmp73A;struct Cyc_List_List*scs=_tmp73B;void*dec_tree=_tmp73C;
# 4223
Cyc_Toc_exptypes_to_c(e);
for(0;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12U: _LLF: _tmp738=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_tmp739=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp734)->f2;_LL10: {struct Cyc_Absyn_Decl*d=_tmp738;struct Cyc_Absyn_Stmt*s=_tmp739;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14U: _LL11: _tmp736=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp734)->f1;_tmp737=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp734)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp736;struct Cyc_Absyn_Exp*e=_tmp737;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13U: _LL13: _tmp735=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp734)->f2;_LL14: {struct Cyc_Absyn_Stmt*s=_tmp735;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4235
({void*_tmpB04=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->tag=0U;_tmp74A;});s->r=_tmpB04;});goto _LL0;default: _LL1F: _LL20:
({void*_tmp74B=0U;({unsigned _tmpB06=s->loc;struct _fat_ptr _tmpB05=({const char*_tmp74C="Cyclone statement in C code";_tag_fat(_tmp74C,sizeof(char),28U);});((int(*)(unsigned,struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpB06,_tmpB05,_tag_fat(_tmp74B,sizeof(void*),0U));});});}_LL0:;}
# 4243
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4245
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp802;_tmp802.tag=0U,({struct _fat_ptr _tmpB07=({const char*_tmp74F="decls_to_c: not at toplevel!";_tag_fat(_tmp74F,sizeof(char),29U);});_tmp802.f1=_tmpB07;});_tmp802;});void*_tmp74D[1U];_tmp74D[0]=& _tmp74E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp74D,sizeof(void*),1U));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp750=d->r;void*_stmttmp7C=_tmp750;void*_tmp751=_stmttmp7C;struct Cyc_List_List*_tmp752;struct Cyc_List_List*_tmp753;struct Cyc_List_List*_tmp754;struct Cyc_List_List*_tmp755;struct Cyc_Absyn_Typedefdecl*_tmp756;struct Cyc_Absyn_Enumdecl*_tmp757;struct Cyc_Absyn_Datatypedecl*_tmp758;struct Cyc_Absyn_Aggrdecl*_tmp759;struct Cyc_Absyn_Fndecl*_tmp75A;struct Cyc_Absyn_Vardecl*_tmp75B;switch(*((int*)_tmp751)){case 0U: _LL1: _tmp75B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp75B;
# 4252
struct _tuple1*_tmp75C=vd->name;struct _tuple1*c_name=_tmp75C;
# 4254
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp75D=_cycalloc(sizeof(*_tmp75D));({union Cyc_Absyn_Nmspace _tmpB08=Cyc_Absyn_Abs_n(0,1);_tmp75D->f1=_tmpB08;}),_tmp75D->f2=(*c_name).f2;_tmp75D;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4261
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4263
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpB09=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpB09;});
({void*_tmpB0A=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpB0A;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->hd=d,_tmp75E->tl=Cyc_Toc_result_decls;_tmp75E;});
goto _LL0;}case 1U: _LL3: _tmp75A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp75A;
# 4270
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB0C=({struct _tuple1*_tmp75F=_cycalloc(sizeof(*_tmp75F));({union Cyc_Absyn_Nmspace _tmpB0B=Cyc_Absyn_Abs_n(0,1);_tmp75F->f1=_tmpB0B;}),_tmp75F->f2=(*fd->name).f2;_tmp75F;});fd->name=_tmpB0C;});
fd->sc=Cyc_Absyn_Public;}
# 4274
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->hd=d,_tmp760->tl=Cyc_Toc_result_decls;_tmp760;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp762=({struct Cyc_Warn_String_Warn_Warg_struct _tmp803;_tmp803.tag=0U,({struct _fat_ptr _tmpB0D=({const char*_tmp763="letdecl at toplevel";_tag_fat(_tmp763,sizeof(char),20U);});_tmp803.f1=_tmpB0D;});_tmp803;});void*_tmp761[1U];_tmp761[0]=& _tmp762;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp761,sizeof(void*),1U));});case 4U: _LL9: _LLA:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp765=({struct Cyc_Warn_String_Warn_Warg_struct _tmp804;_tmp804.tag=0U,({struct _fat_ptr _tmpB0E=({const char*_tmp766="region decl at toplevel";_tag_fat(_tmp766,sizeof(char),24U);});_tmp804.f1=_tmpB0E;});_tmp804;});void*_tmp764[1U];_tmp764[0]=& _tmp765;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos2)(_tag_fat(_tmp764,sizeof(void*),1U));});case 5U: _LLB: _tmp759=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*sd=_tmp759;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6U: _LLD: _tmp758=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LLE: {struct Cyc_Absyn_Datatypedecl*tud=_tmp758;
# 4282
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7U: _LLF: _tmp757=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp757;
# 4285
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767->hd=d,_tmp767->tl=Cyc_Toc_result_decls;_tmp767;});
goto _LL0;}case 8U: _LL11: _tmp756=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp756;
# 4289
td->tvs=0;
if(td->defn != 0){
({void*_tmpB0F=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpB0F;});{
# 4294
struct Cyc_Absyn_Decl*ed;
{void*_tmp768=td->defn;void*_stmttmp7D=_tmp768;void*_tmp769=_stmttmp7D;unsigned _tmp76B;struct Cyc_Absyn_Enumdecl*_tmp76A;if(_tmp769 != 0){if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp769)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp769)->f1)->r)->tag == 1U){_LL24: _tmp76A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp769)->f1)->r)->f1;_tmp76B=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp769)->f1)->loc;_LL25: {struct Cyc_Absyn_Enumdecl*ed2=_tmp76A;unsigned loc=_tmp76B;
# 4297
ed=({struct Cyc_Absyn_Decl*_tmp76D=_cycalloc(sizeof(*_tmp76D));({void*_tmpB10=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp76C=_cycalloc(sizeof(*_tmp76C));_tmp76C->tag=7U,_tmp76C->f1=ed2;_tmp76C;});_tmp76D->r=_tmpB10;}),_tmp76D->loc=loc;_tmp76D;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26: _LL27:
 ed=0;}_LL23:;}
# 4300
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E->hd=ed,_tmp76E->tl=Cyc_Toc_result_decls;_tmp76E;});{
void*_tmp76F=ed->r;void*_stmttmp7E=_tmp76F;void*_tmp770=_stmttmp7E;struct Cyc_Absyn_Enumdecl*_tmp771;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp770)->tag == 7U){_LL29: _tmp771=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp770)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp771;
# 4304
({void*_tmpB12=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773->tag=0U,({void*_tmpB11=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772->tag=15U,_tmp772->f1=ed->name,_tmp772->f2=ed;_tmp772;});_tmp773->f1=_tmpB11;}),_tmp773->f2=0;_tmp773;});td->defn=_tmpB12;});goto _LL28;}}else{_LL2B: _LL2C:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp775=({struct Cyc_Warn_String_Warn_Warg_struct _tmp805;_tmp805.tag=0U,({struct _fat_ptr _tmpB13=({const char*_tmp776="Toc: enumdecl to name";_tag_fat(_tmp776,sizeof(char),22U);});_tmp805.f1=_tmpB13;});_tmp805;});void*_tmp774[1U];_tmp774[0]=& _tmp775;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp774,sizeof(void*),1U));});}_LL28:;}}}}else{
# 4309
enum Cyc_Absyn_KindQual _tmp777=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7F=_tmp777;enum Cyc_Absyn_KindQual _tmp778=_stmttmp7F;if(_tmp778 == Cyc_Absyn_BoxKind){_LL2E: _LL2F:
({void*_tmpB14=Cyc_Toc_void_star_type();td->defn=_tmpB14;});goto _LL2D;}else{_LL30: _LL31:
 td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4317
if(Cyc_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779->hd=d,_tmp779->tl=Cyc_Toc_result_decls;_tmp779;});
goto _LL0;}case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL18;case 15U: _LL17: _LL18:
 goto _LL1A;case 16U: _LL19: _LL1A:
 goto _LL0;case 9U: _LL1B: _tmp755=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp751)->f2;_LL1C: {struct Cyc_List_List*ds2=_tmp755;
_tmp754=ds2;goto _LL1E;}case 10U: _LL1D: _tmp754=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp751)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp754;
_tmp753=ds2;goto _LL20;}case 11U: _LL1F: _tmp753=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp753;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _LL21: _tmp752=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp751)->f1;_LL22: {struct Cyc_List_List*ds2=_tmp752;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4330
return nv;}
# 4335
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _tmp77A=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp80=_tmp77A;struct Cyc_Core_NewDynamicRegion _tmp77B=_stmttmp80;struct Cyc_Core_DynamicRegion*_tmp77C;_LL1: _tmp77C=_tmp77B.key;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp77C;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp77E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp77E));_tmp77E->dyn=dyn,_tmp77E->state=ts;_tmp77E;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp780=34;struct _fat_ptr**_tmp77F=_cycalloc(_check_times(_tmp780,sizeof(struct _fat_ptr*)));_tmp77F[0]=& Cyc_Toc__throw_str,_tmp77F[1]=& Cyc_Toc_setjmp_str,_tmp77F[2]=& Cyc_Toc__push_handler_str,_tmp77F[3]=& Cyc_Toc__pop_handler_str,_tmp77F[4]=& Cyc_Toc__exn_thrown_str,_tmp77F[5]=& Cyc_Toc__npop_handler_str,_tmp77F[6]=& Cyc_Toc__check_null_str,_tmp77F[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp77F[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp77F[9]=& Cyc_Toc__check_fat_subscript_str,_tmp77F[10]=& Cyc_Toc__tag_fat_str,_tmp77F[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp77F[12]=& Cyc_Toc__get_fat_size_str,_tmp77F[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp77F[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp77F[15]=& Cyc_Toc__zero_arr_plus_str,_tmp77F[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp77F[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp77F[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp77F[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp77F[20]=& Cyc_Toc__cycalloc_str,_tmp77F[21]=& Cyc_Toc__cyccalloc_str,_tmp77F[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp77F[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp77F[24]=& Cyc_Toc__region_malloc_str,_tmp77F[25]=& Cyc_Toc__region_calloc_str,_tmp77F[26]=& Cyc_Toc__check_times_str,_tmp77F[27]=& Cyc_Toc__new_region_str,_tmp77F[28]=& Cyc_Toc__push_region_str,_tmp77F[29]=& Cyc_Toc__pop_region_str,_tmp77F[30]=& Cyc_Toc__throw_arraybounds_str,_tmp77F[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp77F[32]=& Cyc_Toc__throw_match_str,_tmp77F[33]=& Cyc_Toc__fast_region_malloc_str;_tmp77F;}),sizeof(struct _fat_ptr*),34U);}}
# 4383
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp781=ts;struct Cyc_Toc_TocStateWrap*_tmp782=Cyc_Toc_toc_state;ts=_tmp782;Cyc_Toc_toc_state=_tmp781;});{
struct Cyc_Toc_TocStateWrap _tmp783=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp81=_tmp783;struct Cyc_Toc_TocStateWrap _tmp784=_stmttmp81;struct Cyc_Toc_TocState*_tmp786;struct Cyc_Core_DynamicRegion*_tmp785;_LL1: _tmp785=_tmp784.dyn;_tmp786=_tmp784.state;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp785;struct Cyc_Toc_TocState*s=_tmp786;
# 4388
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp788=*s;struct Cyc_Toc_TocState _stmttmp82=_tmp788;struct Cyc_Toc_TocState _tmp789=_stmttmp82;struct Cyc_Xarray_Xarray*_tmp78A;_LL4: _tmp78A=_tmp789.temp_labels;_LL5: {struct Cyc_Xarray_Xarray*tls=_tmp78A;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(tls);}}
# 4389
;}
# 4392
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4395
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4401
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4403
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp78B=_cycalloc(sizeof(*_tmp78B));*_tmp78B=pop_tables;_tmp78B;});
Cyc_Toc_init();{
struct _RegionHandle _tmp78C=_new_region("start");struct _RegionHandle*start=& _tmp78C;_push_region(start);
({struct _RegionHandle*_tmpB16=start;struct Cyc_Toc_Env*_tmpB15=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB16,_tmpB15,ds,0);});{
struct Cyc_List_List*_tmp78D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0U);return _tmp78D;}
# 4406
;_pop_region();}}
