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
void Cyc_Core_ufree(void*);
# 191
struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 255
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*,const char*,int);
# 273 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 57
extern struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 251
extern int Cyc_List_forall(int(*)(void*),struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);
# 328
extern void*Cyc_List_assoc(struct Cyc_List_List*,void*);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 67
extern struct _fat_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 69
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*,struct Cyc_Set_Set*,void*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict,int(*)(void*,void*),void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 90 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 307
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 852
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 859
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 869
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 875
void*Cyc_Absyn_compress(void*);
# 894
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 896
extern void*Cyc_Absyn_sint_type;
# 901
extern void*Cyc_Absyn_heap_rgn_type;
# 905
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 907
extern void*Cyc_Absyn_void_type;
# 924
void*Cyc_Absyn_exn_type (void);
# 932
extern void*Cyc_Absyn_fat_bound_type;
# 936
void*Cyc_Absyn_bounds_one (void);
# 944
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 948
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 950
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 952
void*Cyc_Absyn_strct(struct _fat_ptr*);
void*Cyc_Absyn_strctq(struct _tuple1*);
void*Cyc_Absyn_unionq_type(struct _tuple1*);
# 958
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 963
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 968
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 979
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 987
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 992
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1043
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1051
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1060
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1064
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1069
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1107
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1112
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1122
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1124
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1126
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1135
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 46 "warn.h"
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
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
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
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
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 39
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 84 "attributes.h"
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp77A;_tmp77A.tag=0,_tmp77A.f1=str;_tmp77A;});void*_tmp0[1];_tmp0[0]=& _tmp1;Cyc_Warn_impos2(_tag_fat(_tmp0,sizeof(void*),1));});}
# 71
static void*Cyc_Toc_toc_impos(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=0,({struct _fat_ptr _tmp7F1=({const char*_tmp4="Toc: ";_tag_fat(_tmp4,sizeof(char),6U);});_tmp5->f1=_tmp7F1;});_tmp5;});
struct _fat_ptr args=({unsigned _tmp3=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;void**_tmp2=_cycalloc(_check_times(_tmp3,sizeof(void*)));({{unsigned _tmp77B=_get_fat_size(ap,sizeof(void*))+ (unsigned)1;unsigned i;for(i=0;i < _tmp77B;++ i){i == (unsigned)0?_tmp2[i]=toc:(_tmp2[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - (unsigned)1))));}}0;});_tag_fat(_tmp2,sizeof(void*),_tmp3);});
Cyc_Warn_vimpos2(args);}
# 77
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 80
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 90
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 92
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 117
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp((*x).f2,(*y).f2);}
# 124
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmpD=_region_malloc(d,sizeof(*_tmpD));
({struct Cyc_List_List**_tmp7FD=({struct Cyc_List_List**_tmp6=_region_malloc(d,sizeof(*_tmp6));*_tmp6=0;_tmp6;});_tmpD->tuple_types=_tmp7FD;}),({
struct Cyc_List_List**_tmp7FC=({struct Cyc_List_List**_tmp7=_region_malloc(d,sizeof(*_tmp7));*_tmp7=0;_tmp7;});_tmpD->anon_aggr_types=_tmp7FC;}),({
struct Cyc_Dict_Dict*_tmp7FB=({struct Cyc_Dict_Dict*_tmp8=_region_malloc(d,sizeof(*_tmp8));({struct Cyc_Dict_Dict _tmp7FA=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmp8=_tmp7FA;});_tmp8;});_tmpD->aggrs_so_far=_tmp7FB;}),({
struct Cyc_List_List**_tmp7F9=({struct Cyc_List_List**_tmp9=_region_malloc(d,sizeof(*_tmp9));*_tmp9=0;_tmp9;});_tmpD->abs_struct_types=_tmp7F9;}),({
struct Cyc_Set_Set**_tmp7F8=({struct Cyc_Set_Set**_tmpA=_region_malloc(d,sizeof(*_tmpA));({struct Cyc_Set_Set*_tmp7F7=((struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpA=_tmp7F7;});_tmpA;});_tmpD->datatypes_so_far=_tmp7F8;}),({
struct Cyc_Dict_Dict*_tmp7F6=({struct Cyc_Dict_Dict*_tmpB=_region_malloc(d,sizeof(*_tmpB));({struct Cyc_Dict_Dict _tmp7F5=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpB=_tmp7F5;});_tmpB;});_tmpD->xdatatypes_so_far=_tmp7F6;}),({
struct Cyc_Dict_Dict*_tmp7F4=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7F3=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmpC=_tmp7F3;});_tmpC;});_tmpD->qvar_tags=_tmp7F4;}),({
struct Cyc_Xarray_Xarray*_tmp7F2=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmpD->temp_labels=_tmp7F2;});_tmpD;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 143
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 149
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 152
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmpE=ts;struct Cyc_Toc_TocStateWrap*_tmpF=Cyc_Toc_toc_state;ts=_tmpF;Cyc_Toc_toc_state=_tmpE;});{
struct Cyc_Toc_TocStateWrap _tmp10=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp0=_tmp10;struct Cyc_Toc_TocStateWrap _tmp11=_stmttmp0;struct Cyc_Toc_TocState*_tmp13;struct Cyc_Core_DynamicRegion*_tmp12;_tmp12=_tmp11.dyn;_tmp13=_tmp11.state;{struct Cyc_Core_DynamicRegion*dyn=_tmp12;struct Cyc_Toc_TocState*s=_tmp13;
struct _fat_ptr _tmp14=((struct _fat_ptr(*)(struct _fat_ptr))Cyc_Core_alias_refptr)(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));struct _fat_ptr dyn2=_tmp14;
({struct Cyc_Toc_TocStateWrap _tmp7FE=({struct Cyc_Toc_TocStateWrap _tmp77C;_tmp77C.dyn=dyn,_tmp77C.state=s;_tmp77C;});*ts=_tmp7FE;});
({struct Cyc_Toc_TocStateWrap*_tmp15=ts;struct Cyc_Toc_TocStateWrap*_tmp16=Cyc_Toc_toc_state;ts=_tmp16;Cyc_Toc_toc_state=_tmp15;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp18=({struct _tuple15 _tmp77D;_tmp77D.f1=s,_tmp77D.f2=arg;_tmp77D;});struct _tuple15 env=_tmp18;
res=f(h,& env);}
# 160
;}
# 162
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(dyn2,sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 166
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 169
struct _tuple17 _tmp19=*env;struct _tuple17 _stmttmp1=_tmp19;struct _tuple17 _tmp1A=_stmttmp1;void*(*_tmp1D)(struct _tuple1*);struct _tuple1*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_tmp1B=_tmp1A.f1;_tmp1C=(_tmp1A.f2)->f1;_tmp1D=(_tmp1A.f2)->f2;{struct Cyc_Toc_TocState*s=_tmp1B;struct _tuple1*q=_tmp1C;void*(*type_maker)(struct _tuple1*)=_tmp1D;
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt)(*s->aggrs_so_far,q);
return v == 0?type_maker(q):(*(*v)).f2;}}
# 174
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp77E;_tmp77E.f1=q,_tmp77E.f2=type_maker;_tmp77E;});
return((void*(*)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp1E[5U]="curr";
# 187 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_tmp1E,_tmp1E,_tmp1E + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp1F[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_tmp1F,_tmp1F,_tmp1F + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp20[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_tmp20,_tmp20,_tmp20 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp21[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_tmp21,_tmp21,_tmp21 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp22[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_tmp22,_tmp22,_tmp22 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp23[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_tmp23,_tmp23,_tmp23 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp24[12U]="_PoolHandle";
static struct _fat_ptr Cyc_Toc__PoolHandle_string={_tmp24,_tmp24,_tmp24 + 12U};static struct _fat_ptr*Cyc_Toc__PoolHandle_sp=& Cyc_Toc__PoolHandle_string;static char _tmp25[7U]="_throw";
# 207 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_tmp25,_tmp25,_tmp25 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp26[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_tmp26,_tmp26,_tmp26 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp27[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_tmp27,_tmp27,_tmp27 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp28[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_tmp28,_tmp28,_tmp28 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp29[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_tmp29,_tmp29,_tmp29 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp2A[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2B[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_tmp2B,_tmp2B,_tmp2B + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2C[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2C,_tmp2C,_tmp2C + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2D[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2D,_tmp2D,_tmp2D + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp2E[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_tmp2E,_tmp2E,_tmp2E + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _tmp2F[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_tmp2F,_tmp2F,_tmp2F + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _tmp30[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_tmp30,_tmp30,_tmp30 + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _tmp31[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_tmp31,_tmp31,_tmp31 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _tmp32[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _tmp33[19U]="_get_zero_arr_size";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_str={_tmp33,_tmp33,_tmp33 + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp34[24U]="_get_zero_arr_size_char";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp34,_tmp34,_tmp34 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp35[25U]="_get_zero_arr_size_short";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp35,_tmp35,_tmp35 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp36[23U]="_get_zero_arr_size_int";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp36,_tmp36,_tmp36 + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp37[25U]="_get_zero_arr_size_float";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp37,_tmp37,_tmp37 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp38[26U]="_get_zero_arr_size_double";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp38,_tmp38,_tmp38 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp39[30U]="_get_zero_arr_size_longdouble";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp39,_tmp39,_tmp39 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp3A[28U]="_get_zero_arr_size_voidstar";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp3A,_tmp3A,_tmp3A + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp3B[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_tmp3B,_tmp3B,_tmp3B + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _tmp3C[15U]="_zero_arr_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_str={_tmp3C,_tmp3C,_tmp3C + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp3D[20U]="_zero_arr_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp3D,_tmp3D,_tmp3D + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp3E[21U]="_zero_arr_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp3E,_tmp3E,_tmp3E + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp3F[19U]="_zero_arr_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp3F,_tmp3F,_tmp3F + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp40[21U]="_zero_arr_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp40,_tmp40,_tmp40 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp41[22U]="_zero_arr_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp41,_tmp41,_tmp41 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp42[26U]="_zero_arr_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp42,_tmp42,_tmp42 + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp43[24U]="_zero_arr_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp43,_tmp43,_tmp43 + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp44[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_tmp44,_tmp44,_tmp44 + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _tmp45[23U]="_zero_arr_inplace_plus";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp45,_tmp45,_tmp45 + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp46[28U]="_zero_arr_inplace_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp46,_tmp46,_tmp46 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp47[29U]="_zero_arr_inplace_plus_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp47,_tmp47,_tmp47 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp48[27U]="_zero_arr_inplace_plus_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp48,_tmp48,_tmp48 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp49[29U]="_zero_arr_inplace_plus_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp49,_tmp49,_tmp49 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp4A[30U]="_zero_arr_inplace_plus_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp4A,_tmp4A,_tmp4A + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp4B[34U]="_zero_arr_inplace_plus_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp4B,_tmp4B,_tmp4B + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp4C[32U]="_zero_arr_inplace_plus_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp4C,_tmp4C,_tmp4C + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp4D[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_tmp4D,_tmp4D,_tmp4D + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _tmp4E[28U]="_zero_arr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp4E,_tmp4E,_tmp4E + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp4F[33U]="_zero_arr_inplace_plus_post_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp4F,_tmp4F,_tmp4F + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp50[34U]="_zero_arr_inplace_plus_post_short";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp50,_tmp50,_tmp50 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp51[32U]="_zero_arr_inplace_plus_post_int";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp51,_tmp51,_tmp51 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp52[34U]="_zero_arr_inplace_plus_post_float";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp52,_tmp52,_tmp52 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp53[35U]="_zero_arr_inplace_plus_post_double";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp53,_tmp53,_tmp53 + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp54[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp54,_tmp54,_tmp54 + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp55[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp55,_tmp55,_tmp55 + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp56[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_tmp56,_tmp56,_tmp56 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp57[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_tmp57,_tmp57,_tmp57 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp58[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_tmp58,_tmp58,_tmp58 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp59[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp59,_tmp59,_tmp59 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp5A[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_tmp5A,_tmp5A,_tmp5A + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp5B[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_tmp5B,_tmp5B,_tmp5B + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp5C[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_tmp5C,_tmp5C,_tmp5C + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp5D[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_tmp5D,_tmp5D,_tmp5D + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp5E[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_tmp5E,_tmp5E,_tmp5E + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp5F[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_tmp5F,_tmp5F,_tmp5F + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp60[10U]="_new_pool";
static struct _fat_ptr Cyc_Toc__new_pool_str={_tmp60,_tmp60,_tmp60 + 10U};static struct _tuple1 Cyc_Toc__new_pool_pr={{.Loc_n={4,0}},& Cyc_Toc__new_pool_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_pool_bnd={0,& Cyc_Toc__new_pool_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_pool_re={1,(void*)& Cyc_Toc__new_pool_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_pool_ev={0,(void*)& Cyc_Toc__new_pool_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_pool_e=& Cyc_Toc__new_pool_ev;static char _tmp61[11U]="_push_pool";
static struct _fat_ptr Cyc_Toc__push_pool_str={_tmp61,_tmp61,_tmp61 + 11U};static struct _tuple1 Cyc_Toc__push_pool_pr={{.Loc_n={4,0}},& Cyc_Toc__push_pool_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_pool_bnd={0,& Cyc_Toc__push_pool_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_pool_re={1,(void*)& Cyc_Toc__push_pool_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_pool_ev={0,(void*)& Cyc_Toc__push_pool_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_pool_e=& Cyc_Toc__push_pool_ev;static char _tmp62[10U]="_pop_pool";
static struct _fat_ptr Cyc_Toc__pop_pool_str={_tmp62,_tmp62,_tmp62 + 10U};static struct _tuple1 Cyc_Toc__pop_pool_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_pool_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_pool_bnd={0,& Cyc_Toc__pop_pool_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_pool_re={1,(void*)& Cyc_Toc__pop_pool_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_pool_ev={0,(void*)& Cyc_Toc__pop_pool_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_pool_e=& Cyc_Toc__pop_pool_ev;static char _tmp63[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_tmp63,_tmp63,_tmp63 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp64[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_tmp64,_tmp64,_tmp64 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _tmp65[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_tmp65,_tmp65,_tmp65 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp66[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_tmp66,_tmp66,_tmp66 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp67[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_tmp67,_tmp67,_tmp67 + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 278
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 281
if((unsigned)_get_exn_thrown_e)
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);{
struct _tuple1*qv=({struct _tuple1*_tmp72=_cycalloc(sizeof(*_tmp72));((_tmp72->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp803=({struct _fat_ptr*_tmp6D[1];({struct _fat_ptr*_tmp802=({struct _fat_ptr*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct _fat_ptr _tmp801=({const char*_tmp6E="Core";_tag_fat(_tmp6E,sizeof(char),5U);});*_tmp6F=_tmp801;});_tmp6F;});_tmp6D[0]=_tmp802;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp6D,sizeof(struct _fat_ptr*),1));});((_tmp72->f1).Abs_n).val=_tmp803;}),({struct _fat_ptr*_tmp800=({struct _fat_ptr*_tmp71=_cycalloc(sizeof(*_tmp71));({struct _fat_ptr _tmp7FF=({const char*_tmp70="get_exn_thrown";_tag_fat(_tmp70,sizeof(char),15U);});*_tmp71=_tmp7FF;});_tmp71;});_tmp72->f2=_tmp800;});_tmp72;});
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=0,_tmp6C->f1=qv;_tmp6C;});
struct Cyc_Absyn_Exp*fnname=({struct Cyc_Absyn_Exp*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->topt=0,({void*_tmp804=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=1,_tmp6A->f1=bnd;_tmp6A;});_tmp6B->r=_tmp804;}),_tmp6B->loc=0U,_tmp6B->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp6B;});
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=10,_tmp69->f1=fnname,_tmp69->f2=0,_tmp69->f3=0,_tmp69->f4=0;_tmp69;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->topt=0,_tmp68->r=fncall_re,_tmp68->loc=0U,_tmp68->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp68;});
return(struct Cyc_Absyn_Exp*)_check_null(_get_exn_thrown_e);}}
# 291
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 293
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t == 0)
t=({void*_tmp808=Cyc_Absyn_void_type;void*_tmp807=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp806=Cyc_Absyn_empty_tqual(0U);void*_tmp805=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_tmp808,_tmp807,_tmp806,_tmp805,Cyc_Absyn_false_type);});
return(void*)_check_null(t);}
# 299
static void*Cyc_Toc_fat_ptr_type (void){
static void*t=0;
if(t == 0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__fat_ptr_pr,0),0);
return(void*)_check_null(t);}
# 305
static void*Cyc_Toc_rgn_type (void){
static void*r=0;
if(r == 0)
r=({void*_tmp809=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp809,Cyc_Toc_mt_tq);});
return(void*)_check_null(r);}
# 312
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 316
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp75=_cycalloc(sizeof(*_tmp75));({struct Cyc_List_List*_tmp80B=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));({void*_tmp80A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=1,_tmp73->f1=name;_tmp73;});_tmp74->hd=_tmp80A;}),_tmp74->tl=0;_tmp74;});_tmp75->f1=_tmp80B;}),_tmp75->f2=e;_tmp75;});}
# 320
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
return({struct Cyc_Absyn_Exp*_tmp80C=f;Cyc_Absyn_fncall_exp(_tmp80C,((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array)(args),0U);});}
# 323
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp76=e->r;void*_stmttmp2=_tmp76;void*_tmp77=_stmttmp2;struct Cyc_Absyn_Exp*_tmp78;if(*((int*)_tmp77)== 14){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f4 == Cyc_Absyn_No_coercion){_tmp78=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2;{struct Cyc_Absyn_Exp*e=_tmp78;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);};}
# 332
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=14,_tmp79->f1=t,_tmp79->f2=e,_tmp79->f3=0,_tmp79->f4=Cyc_Absyn_No_coercion;_tmp79;});}
# 335
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=20,_tmp7A->f1=e;_tmp7A;});}
# 338
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=23,_tmp7B->f1=e1,_tmp7B->f2=e2;_tmp7B;});}
# 341
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=37,_tmp7C->f1=s;_tmp7C;});}
# 344
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=17,_tmp7D->f1=t;_tmp7D;});}
# 347
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=10,_tmp7E->f1=e,({struct Cyc_List_List*_tmp80D=((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_from_array)(es);_tmp7E->f2=_tmp80D;}),_tmp7E->f3=0,_tmp7E->f4=1;_tmp7E;});}
# 350
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=2,_tmp7F->f1=s1,_tmp7F->f2=s2;_tmp7F;});}
# 353
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=6,_tmp80->f1=e1,_tmp80->f2=e2,_tmp80->f3=e3;_tmp80;});}
# 356
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=21,_tmp81->f1=e,_tmp81->f2=n,_tmp81->f3=0,_tmp81->f4=0;_tmp81;});}
# 359
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 362
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=36,_tmp82->f1=tdopt,_tmp82->f2=ds;_tmp82;});}
# 364
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=8,_tmp83->f1=v;_tmp83;});}
# 368
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 372
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp84=e->r;void*_stmttmp3=_tmp84;void*_tmp85=_stmttmp3;struct Cyc_Absyn_Exp*_tmp86;if(*((int*)_tmp85)== 20){_tmp86=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->f1;{struct Cyc_Absyn_Exp*e1=_tmp86;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 400 "toc.cyc"
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 406
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
void*_tmp87=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp87;void*_tmp88=_stmttmp4;switch(*((int*)_tmp88)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)){case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f2){case Cyc_Absyn_Char_sz:
 return fS->fchar;case Cyc_Absyn_Short_sz:
 return fS->fshort;case Cyc_Absyn_Int_sz:
 return fS->fint;default: goto _LLF;}case 2: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88)->f1)->f1){case 0:
 return fS->ffloat;case 1:
 return fS->fdouble;default:
 return fS->flongdouble;}default: goto _LLF;}case 3:
 return fS->fvoidstar;default: _LLF:
# 416
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp781;_tmp781.tag=0,({struct _fat_ptr _tmp80E=({const char*_tmp8E="expression type ";_tag_fat(_tmp8E,sizeof(char),17U);});_tmp781.f1=_tmp80E;});_tmp781;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp8B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp780;_tmp780.tag=2,_tmp780.f1=(void*)t;_tmp780;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp77F;_tmp77F.tag=0,({struct _fat_ptr _tmp80F=({const char*_tmp8D=" (not int, float, double, or pointer)";_tag_fat(_tmp8D,sizeof(char),38U);});_tmp77F.f1=_tmp80F;});_tmp77F;});void*_tmp89[3];_tmp89[0]=& _tmp8A,_tmp89[1]=& _tmp8B,_tmp89[2]=& _tmp8C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp89,sizeof(void*),3));});};}
# 419
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8F=Cyc_Absyn_compress((void*)_check_null(arr->topt));void*_stmttmp5=_tmp8F;void*_tmp90=_stmttmp5;void*_tmp91;if(*((int*)_tmp90)== 3){_tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).elt_type;{void*et=_tmp91;
return Cyc_Toc_getFunctionType(fS,et);}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmp782;_tmp782.tag=0,({struct _fat_ptr _tmp810=({const char*_tmp94="impossible type (not pointer)";_tag_fat(_tmp94,sizeof(char),30U);});_tmp782.f1=_tmp810;});_tmp782;});void*_tmp92[1];_tmp92[0]=& _tmp93;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp92,sizeof(void*),1));});};}
# 429
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp95=e->r;void*_stmttmp6=_tmp95;void*_tmp96=_stmttmp6;struct Cyc_List_List*_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_List_List*_tmp99;struct Cyc_List_List*_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9C;long long _tmp9D;int _tmp9E;short _tmp9F;struct _fat_ptr _tmpA0;char _tmpA1;switch(*((int*)_tmp96)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Null_c).tag){case 2: _tmpA1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Char_c).val).f2;{char c=_tmpA1;
return(int)c == (int)'\000';}case 3: _tmpA0=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Wchar_c).val;{struct _fat_ptr s=_tmpA0;
# 433
unsigned long _tmpA2=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmpA2;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(1;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 443
return 0;}case 4: _tmp9F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Short_c).val).f2;{short i=_tmp9F;
return(int)i == 0;}case 5: _tmp9E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).Int_c).val).f2;{int i=_tmp9E;
return i == 0;}case 6: _tmp9D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96)->f1).LongLong_c).val).f2;{long long i=_tmp9D;
return i == (long long)0;}case 1: _LLE:
# 448
 return 1;default: goto _LL1B;}case 2:
# 447
 goto _LLE;case 14: _tmp9C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96)->f2;{struct Cyc_Absyn_Exp*e1=_tmp9C;
# 449
return Cyc_Toc_is_zero(e1);}case 24: _tmp9B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp96)->f1;{struct Cyc_List_List*es=_tmp9B;
return((int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall)(Cyc_Toc_is_zero,es);}case 26: _tmp9A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp96)->f1;{struct Cyc_List_List*dles=_tmp9A;
_tmp99=dles;goto _LL16;}case 29: _tmp99=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp96)->f3;_LL16: {struct Cyc_List_List*dles=_tmp99;
_tmp98=dles;goto _LL18;}case 25: _tmp98=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL18: {struct Cyc_List_List*dles=_tmp98;
_tmp97=dles;goto _LL1A;}case 36: _tmp97=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp96)->f2;_LL1A: {struct Cyc_List_List*dles=_tmp97;
# 455
for(1;dles != 0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f2))return 0;}
return 1;}default: _LL1B:
 return 0;};}
# 463
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA3=x;struct _fat_ptr*_tmpA5;union Cyc_Absyn_Nmspace _tmpA4;_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;{union Cyc_Absyn_Nmspace ns=_tmpA4;struct _fat_ptr*v=_tmpA5;
union Cyc_Absyn_Nmspace _tmpA6=ns;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA9;switch((_tmpA6.Abs_n).tag){case 4:
 _tmpA9=0;goto _LL7;case 1: _tmpA9=(_tmpA6.Rel_n).val;_LL7: {struct Cyc_List_List*vs=_tmpA9;
_tmpA8=vs;goto _LL9;}case 2: _tmpA8=(_tmpA6.Abs_n).val;_LL9: {struct Cyc_List_List*vs=_tmpA8;
_tmpA7=vs;goto _LLB;}default: _tmpA7=(_tmpA6.C_n).val;_LLB: {struct Cyc_List_List*vs=_tmpA7;
# 472
if(vs == 0)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp784;_tmp784.tag=0,_tmp784.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp784;});struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp783;_tmp783.tag=0,_tmp783.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp783;});void*_tmpAA[2];_tmpAA[0]=& _tmpAC,_tmpAA[1]=& _tmpAD;({struct _fat_ptr _tmp811=({const char*_tmpAB="%s_%s_struct";_tag_fat(_tmpAB,sizeof(char),13U);});Cyc_aprintf(_tmp811,_tag_fat(_tmpAA,sizeof(void*),2));});});{
struct _RegionHandle _tmpAE=_new_region("r");struct _RegionHandle*r=& _tmpAE;_push_region(r);
{struct _fat_ptr _tmpB5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB1=({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0,_tmp787.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp787;});struct Cyc_String_pa_PrintArg_struct _tmpB2=({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0,({struct _fat_ptr _tmp814=(struct _fat_ptr)((struct _fat_ptr)({struct _RegionHandle*_tmp813=r;struct Cyc_List_List*_tmp812=vs;Cyc_rstr_sepstr(_tmp813,_tmp812,({const char*_tmpB4="_";_tag_fat(_tmpB4,sizeof(char),2U);}));}));_tmp786.f1=_tmp814;});_tmp786;});struct Cyc_String_pa_PrintArg_struct _tmpB3=({struct Cyc_String_pa_PrintArg_struct _tmp785;_tmp785.tag=0,_tmp785.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp785;});void*_tmpAF[3];_tmpAF[0]=& _tmpB1,_tmpAF[1]=& _tmpB2,_tmpAF[2]=& _tmpB3;({struct _fat_ptr _tmp815=({const char*_tmpB0="%s_%s_%s_struct";_tag_fat(_tmpB0,sizeof(char),16U);});Cyc_aprintf(_tmp815,_tag_fat(_tmpAF,sizeof(void*),3));});});_npop_handler(0);return _tmpB5;};_pop_region();}}};}}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 485
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 488
struct _tuple20 _tmpB6=*env;struct _tuple20 _stmttmp7=_tmpB6;struct _tuple20 _tmpB7=_stmttmp7;struct _tuple14*_tmpB9;struct Cyc_Dict_Dict*_tmpB8;_tmpB8=(_tmpB7.f1)->qvar_tags;_tmpB9=_tmpB7.f2;{struct Cyc_Dict_Dict*qvs=_tmpB8;struct _tuple14*pair=_tmpB9;
struct _tuple14 _tmpBA=*pair;struct _tuple14 _stmttmp8=_tmpBA;struct _tuple14 _tmpBB=_stmttmp8;struct _tuple1*_tmpBD;struct _tuple1*_tmpBC;_tmpBC=_tmpBB.f1;_tmpBD=_tmpBB.f2;{struct _tuple1*fieldname=_tmpBC;struct _tuple1*dtname=_tmpBD;
struct _handler_cons _tmpBE;_push_handler(& _tmpBE);{int _tmpC0=0;if(setjmp(_tmpBE.handler))_tmpC0=1;if(!_tmpC0){{struct _tuple1*_tmpC1=((struct _tuple1*(*)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*))Cyc_Dict_lookup_other)(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0);return _tmpC1;};_pop_handler();}else{void*_tmpBF=(void*)Cyc_Core_get_exn_thrown();void*_tmpC2=_tmpBF;void*_tmpC3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC2)->tag == Cyc_Dict_Absent){
# 492
struct _tuple14*_tmpC4=({struct _tuple14*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->f1=fieldname,_tmpCA->f2=dtname;_tmpCA;});struct _tuple14*new_pair=_tmpC4;
struct _tuple1*_tmpC5=fieldname;struct _fat_ptr*_tmpC7;union Cyc_Absyn_Nmspace _tmpC6;_tmpC6=_tmpC5->f1;_tmpC7=_tmpC5->f2;{union Cyc_Absyn_Nmspace nmspace=_tmpC6;struct _fat_ptr*fieldvar=_tmpC7;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->f1=nmspace,({struct _fat_ptr*_tmp816=({struct _fat_ptr*_tmpC8=_cycalloc(sizeof(*_tmpC8));*_tmpC8=newvar;_tmpC8;});_tmpC9->f2=_tmp816;});_tmpC9;});
({struct Cyc_Dict_Dict _tmp817=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*))Cyc_Dict_insert)(*qvs,new_pair,res);*qvs=_tmp817;});
return res;}}else{_tmpC3=_tmpC2;{void*exn=_tmpC3;(int)_rethrow(exn);}};}}}}}
# 500
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp788;_tmp788.f1=fieldname,_tmp788.f2=dtname;_tmp788;});
return((struct _tuple1*(*)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 507
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->kind=Cyc_Absyn_StructA,_tmpCD->sc=Cyc_Absyn_Public,_tmpCD->tvs=0,_tmpCD->attributes=0,_tmpCD->expected_mem_kind=0,({
# 510
struct _tuple1*_tmp81A=({struct _tuple1*_tmpCB=_cycalloc(sizeof(*_tmpCB));({union Cyc_Absyn_Nmspace _tmp819=Cyc_Absyn_Rel_n(0);_tmpCB->f1=_tmp819;}),_tmpCB->f2=name;_tmpCB;});_tmpCD->name=_tmp81A;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp818=({struct Cyc_Absyn_AggrdeclImpl*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->exist_vars=0,_tmpCC->rgn_po=0,_tmpCC->fields=fs,_tmpCC->tagged=0;_tmpCC;});_tmpCD->impl=_tmp818;});_tmpCD;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};struct _tuple23{struct Cyc_Absyn_Tqual f1;void*f2;};
# 516
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 519
struct _tuple21 _tmpCE=*env;struct _tuple21 _stmttmp9=_tmpCE;struct _tuple21 _tmpCF=_stmttmp9;struct Cyc_List_List*_tmpD1;struct Cyc_List_List**_tmpD0;_tmpD0=(_tmpCF.f1)->tuple_types;_tmpD1=_tmpCF.f2;{struct Cyc_List_List**tuple_types=_tmpD0;struct Cyc_List_List*tqs0=_tmpD1;
# 521
{struct Cyc_List_List*_tmpD2=*tuple_types;struct Cyc_List_List*tts=_tmpD2;for(0;tts != 0;tts=tts->tl){
struct _tuple22*_tmpD3=(struct _tuple22*)tts->hd;struct _tuple22*_stmttmpA=_tmpD3;struct _tuple22*_tmpD4=_stmttmpA;struct Cyc_List_List*_tmpD6;void*_tmpD5;_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;{void*x=_tmpD5;struct Cyc_List_List*ts=_tmpD6;
struct Cyc_List_List*_tmpD7=tqs0;struct Cyc_List_List*tqs=_tmpD7;
for(1;tqs != 0 && ts != 0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f2,(void*)ts->hd))
break;}
if(tqs == 0 && ts == 0)
return x;}}}{
# 532
struct _fat_ptr*xname=({struct _fat_ptr*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct _fat_ptr _tmp81C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE6=({struct Cyc_Int_pa_PrintArg_struct _tmp789;_tmp789.tag=1,_tmp789.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp789;});void*_tmpE4[1];_tmpE4[0]=& _tmpE6;({struct _fat_ptr _tmp81B=({const char*_tmpE5="_tuple%d";_tag_fat(_tmpE5,sizeof(char),9U);});Cyc_aprintf(_tmp81B,_tag_fat(_tmpE4,sizeof(void*),1));});});*_tmpE7=_tmp81C;});_tmpE7;});
struct Cyc_List_List*_tmpD8=0;struct Cyc_List_List*fs=_tmpD8;
struct Cyc_List_List*_tmpD9=0;struct Cyc_List_List*ts=_tmpD9;
{int i=1;for(0;tqs0 != 0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f2;
fs=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct Cyc_Absyn_Aggrfield*_tmp81E=({struct Cyc_Absyn_Aggrfield*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct _fat_ptr*_tmp81D=Cyc_Absyn_fieldname(i);_tmpDA->name=_tmp81D;}),_tmpDA->tq=Cyc_Toc_mt_tq,_tmpDA->type=t,_tmpDA->width=0,_tmpDA->attributes=0,_tmpDA->requires_clause=0;_tmpDA;});_tmpDB->hd=_tmp81E;}),_tmpDB->tl=fs;_tmpDB;});
ts=({struct Cyc_List_List*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));_tmpDC->hd=t,_tmpDC->tl=ts;_tmpDC;});}}
# 540
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fs);
ts=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ts);{
struct Cyc_Absyn_Aggrdecl*_tmpDD=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpDD;
void*_tmpDE=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE3=_cycalloc(sizeof(*_tmpE3));*_tmpE3=sd;_tmpE3;})),0);void*ans=_tmpDE;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct Cyc_Absyn_Decl*_tmp81F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->tag=5,_tmpDF->f1=sd;_tmpDF;}),0U);_tmpE0->hd=_tmp81F;}),_tmpE0->tl=Cyc_Toc_result_decls;_tmpE0;});
({struct Cyc_List_List*_tmp821=({struct Cyc_List_List*_tmpE2=_region_malloc(d,sizeof(*_tmpE2));({struct _tuple22*_tmp820=({struct _tuple22*_tmpE1=_region_malloc(d,sizeof(*_tmpE1));_tmpE1->f1=ans,_tmpE1->f2=ts;_tmpE1;});_tmpE2->hd=_tmp820;}),_tmpE2->tl=*tuple_types;_tmpE2;});*tuple_types=_tmp821;});
return ans;}}}}
# 548
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 553
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 556
struct _tuple25*_tmpE8=env;struct Cyc_List_List*_tmpEB;enum Cyc_Absyn_AggrKind _tmpEA;struct Cyc_List_List**_tmpE9;_tmpE9=(_tmpE8->f1)->anon_aggr_types;_tmpEA=(_tmpE8->f2)->f1;_tmpEB=(_tmpE8->f2)->f2;{struct Cyc_List_List**anon_aggr_types=_tmpE9;enum Cyc_Absyn_AggrKind ak=_tmpEA;struct Cyc_List_List*fs=_tmpEB;
# 558
{struct Cyc_List_List*_tmpEC=*anon_aggr_types;struct Cyc_List_List*ts=_tmpEC;for(0;ts != 0;ts=ts->tl){
struct _tuple26*_tmpED=(struct _tuple26*)ts->hd;struct _tuple26*_stmttmpB=_tmpED;struct _tuple26*_tmpEE=_stmttmpB;struct Cyc_List_List*_tmpF1;enum Cyc_Absyn_AggrKind _tmpF0;void*_tmpEF;_tmpEF=_tmpEE->f1;_tmpF0=_tmpEE->f2;_tmpF1=_tmpEE->f3;{void*x=_tmpEF;enum Cyc_Absyn_AggrKind ak2=_tmpF0;struct Cyc_List_List*fs2=_tmpF1;
if((int)ak == (int)ak2 && !((int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 565
struct _fat_ptr*xname=({struct _fat_ptr*_tmpFC=_cycalloc(sizeof(*_tmpFC));({struct _fat_ptr _tmp823=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFB=({struct Cyc_Int_pa_PrintArg_struct _tmp78A;_tmp78A.tag=1,_tmp78A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp78A;});void*_tmpF9[1];_tmpF9[0]=& _tmpFB;({struct _fat_ptr _tmp822=({const char*_tmpFA="_tuple%d";_tag_fat(_tmpFA,sizeof(char),9U);});Cyc_aprintf(_tmp822,_tag_fat(_tmpF9,sizeof(void*),1));});});*_tmpFC=_tmp823;});_tmpFC;});
struct Cyc_Absyn_Aggrdecl*_tmpF2=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmpF2;
sd->kind=ak;{
void*_tmpF3=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF8=_cycalloc(sizeof(*_tmpF8));*_tmpF8=sd;_tmpF8;})),0);void*ans=_tmpF3;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));({struct Cyc_Absyn_Decl*_tmp824=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=5,_tmpF4->f1=sd;_tmpF4;}),0U);_tmpF5->hd=_tmp824;}),_tmpF5->tl=Cyc_Toc_result_decls;_tmpF5;});
({struct Cyc_List_List*_tmp826=({struct Cyc_List_List*_tmpF7=_region_malloc(d,sizeof(*_tmpF7));({struct _tuple26*_tmp825=({struct _tuple26*_tmpF6=_region_malloc(d,sizeof(*_tmpF6));_tmpF6->f1=ans,_tmpF6->f2=ak,_tmpF6->f3=fs;_tmpF6;});_tmpF7->hd=_tmp825;}),_tmpF7->tl=*anon_aggr_types;_tmpF7;});*anon_aggr_types=_tmp826;});
return ans;}}}}
# 573
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->f1=ak,_tmpFD->f2=fs;_tmpFD;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 582
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 591
struct _tuple28 _tmpFE=*env;struct _tuple28 _stmttmpC=_tmpFE;struct _tuple28 _tmpFF=_stmttmpC;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp103;struct Cyc_List_List*_tmp102;struct _tuple1*_tmp101;struct Cyc_List_List**_tmp100;_tmp100=(_tmpFF.f1)->abs_struct_types;_tmp101=(_tmpFF.f2)->f1;_tmp102=(_tmpFF.f2)->f2;_tmp103=(_tmpFF.f2)->f3;_tmp104=(_tmpFF.f2)->f4;{struct Cyc_List_List**abs_struct_types=_tmp100;struct _tuple1*struct_name=_tmp101;struct Cyc_List_List*type_vars=_tmp102;struct Cyc_List_List*type_args=_tmp103;struct Cyc_List_List*fields=_tmp104;
# 595
{struct Cyc_List_List*_tmp105=*abs_struct_types;struct Cyc_List_List*tts=_tmp105;for(0;tts != 0;tts=tts->tl){
struct _tuple29*_tmp106=(struct _tuple29*)tts->hd;struct _tuple29*_stmttmpD=_tmp106;struct _tuple29*_tmp107=_stmttmpD;void*_tmp10A;struct Cyc_List_List*_tmp109;struct _tuple1*_tmp108;_tmp108=_tmp107->f1;_tmp109=_tmp107->f2;_tmp10A=_tmp107->f3;{struct _tuple1*x=_tmp108;struct Cyc_List_List*ts2=_tmp109;void*t=_tmp10A;
if(Cyc_Absyn_qvar_cmp(x,struct_name)== 0 &&({
int _tmp827=((int(*)(struct Cyc_List_List*))Cyc_List_length)(type_args);_tmp827 == ((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts2);})){
int okay=1;
{struct Cyc_List_List*_tmp10B=type_args;struct Cyc_List_List*ts=_tmp10B;for(0;ts != 0;(ts=ts->tl,ts2=ts2->tl)){
void*_tmp10C=(void*)ts->hd;void*t=_tmp10C;
void*_tmp10D=(void*)((struct Cyc_List_List*)_check_null(ts2))->hd;void*t2=_tmp10D;
{struct Cyc_Absyn_Kind*_tmp10E=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmpE=_tmp10E;struct Cyc_Absyn_Kind*_tmp10F=_stmttmpE;switch(((struct Cyc_Absyn_Kind*)_tmp10F)->kind){case Cyc_Absyn_EffKind:
 goto _LLA;case Cyc_Absyn_RgnKind: _LLA:
 continue;default:
# 608
 if(Cyc_Unify_unify(t,t2)||({void*_tmp828=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_tmp828,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 613
break;}}
# 615
if(okay)
return t;}}}}{
# 621
struct _fat_ptr*xname=({struct _fat_ptr*_tmp122=_cycalloc(sizeof(*_tmp122));({struct _fat_ptr _tmp82A=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp121=({struct Cyc_Int_pa_PrintArg_struct _tmp78B;_tmp78B.tag=1,_tmp78B.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp78B;});void*_tmp11F[1];_tmp11F[0]=& _tmp121;({struct _fat_ptr _tmp829=({const char*_tmp120="_tuple%d";_tag_fat(_tmp120,sizeof(char),9U);});Cyc_aprintf(_tmp829,_tag_fat(_tmp11F,sizeof(void*),1));});});*_tmp122=_tmp82A;});_tmp122;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp110=0;struct Cyc_List_List*fs=_tmp110;
# 625
({struct Cyc_List_List*_tmp82C=({struct Cyc_List_List*_tmp112=_region_malloc(d,sizeof(*_tmp112));({struct _tuple29*_tmp82B=({struct _tuple29*_tmp111=_region_malloc(d,sizeof(*_tmp111));_tmp111->f1=struct_name,_tmp111->f2=type_args,_tmp111->f3=x;_tmp111;});_tmp112->hd=_tmp82B;}),_tmp112->tl=*abs_struct_types;_tmp112;});*abs_struct_types=_tmp82C;});{
# 628
struct _RegionHandle _tmp113=_new_region("r");struct _RegionHandle*r=& _tmp113;_push_region(r);
{struct Cyc_List_List*_tmp114=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(r,r,type_vars,type_args);struct Cyc_List_List*inst=_tmp114;
for(1;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp115=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp115;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 638
if((fields->tl == 0 &&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({void*_tmp82D=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=6,_tmp116->f1=0;_tmp116;});_tmp117->hd=_tmp82D;}),_tmp117->tl=atts;_tmp117;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 645
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=4,({void*_tmp82F=Cyc_Toc_void_star_type();(_tmp118->f1).elt_type=_tmp82F;}),(_tmp118->f1).tq=Cyc_Toc_mt_tq,({
struct Cyc_Absyn_Exp*_tmp82E=Cyc_Absyn_uint_exp(0U,0U);(_tmp118->f1).num_elts=_tmp82E;}),(_tmp118->f1).zero_term=Cyc_Absyn_false_type,(_tmp118->f1).zt_loc=0U;_tmp118;});
fs=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct Cyc_Absyn_Aggrfield*_tmp830=({struct Cyc_Absyn_Aggrfield*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->name=f->name,_tmp119->tq=Cyc_Toc_mt_tq,_tmp119->type=t,_tmp119->width=f->width,_tmp119->attributes=atts,_tmp119->requires_clause=0;_tmp119;});_tmp11A->hd=_tmp830;}),_tmp11A->tl=fs;_tmp11A;});}
# 650
fs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fs);{
struct Cyc_Absyn_Aggrdecl*_tmp11B=Cyc_Toc_make_c_struct_defn(xname,fs);struct Cyc_Absyn_Aggrdecl*sd=_tmp11B;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Decl*_tmp831=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=5,_tmp11C->f1=sd;_tmp11C;}),0U);_tmp11D->hd=_tmp831;}),_tmp11D->tl=Cyc_Toc_result_decls;_tmp11D;});{
void*_tmp11E=x;_npop_handler(0);return _tmp11E;}}}
# 629
;_pop_region();}}}}
# 656
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 660
struct _tuple27 env=({struct _tuple27 _tmp78C;_tmp78C.f1=struct_name,_tmp78C.f2=type_vars,_tmp78C.f3=type_args,_tmp78C.f4=fields;_tmp78C;});
return((void*(*)(struct _tuple27*,void*(*)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 668
struct _tuple1*Cyc_Toc_temp_var (void){
return({struct _tuple1*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp834=({struct _fat_ptr*_tmp126=_cycalloc(sizeof(*_tmp126));({struct _fat_ptr _tmp833=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp125=({struct Cyc_Int_pa_PrintArg_struct _tmp78D;_tmp78D.tag=1,_tmp78D.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_tmp78D;});void*_tmp123[1];_tmp123[0]=& _tmp125;({struct _fat_ptr _tmp832=({const char*_tmp124="_tmp%X";_tag_fat(_tmp124,sizeof(char),7U);});Cyc_aprintf(_tmp832,_tag_fat(_tmp123,sizeof(void*),1));});});*_tmp126=_tmp833;});_tmp126;});_tmp127->f2=_tmp834;});_tmp127;});}struct _tuple30{struct _tuple1*f1;struct Cyc_Absyn_Exp*f2;};
# 671
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
return({struct _tuple30 _tmp78E;_tmp78E.f1=v,({struct Cyc_Absyn_Exp*_tmp835=Cyc_Absyn_var_exp(v,0U);_tmp78E.f2=_tmp835;});_tmp78E;});}struct _tuple31{struct Cyc_Toc_TocState*f1;int f2;};
# 678
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _tmp128=*env;struct _tuple31 _stmttmpF=_tmp128;struct _tuple31 _tmp129=_stmttmpF;struct Cyc_Xarray_Xarray*_tmp12A;_tmp12A=(_tmp129.f1)->temp_labels;{struct Cyc_Xarray_Xarray*temp_labels=_tmp12A;
int _tmp12B=Cyc_Toc_fresh_label_counter ++;int i=_tmp12B;
if(({int _tmp836=i;_tmp836 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(temp_labels);}))
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(temp_labels,i);{
struct _fat_ptr*res=({struct _fat_ptr*_tmp132=_cycalloc(sizeof(*_tmp132));({struct _fat_ptr _tmp838=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp131=({struct Cyc_Int_pa_PrintArg_struct _tmp790;_tmp790.tag=1,_tmp790.f1=(unsigned)i;_tmp790;});void*_tmp12F[1];_tmp12F[0]=& _tmp131;({struct _fat_ptr _tmp837=({const char*_tmp130="_LL%X";_tag_fat(_tmp130,sizeof(char),6U);});Cyc_aprintf(_tmp837,_tag_fat(_tmp12F,sizeof(void*),1));});});*_tmp132=_tmp838;});_tmp132;});
if(({int _tmp839=((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(temp_labels,res);_tmp839 != i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp78F;_tmp78F.tag=0,({struct _fat_ptr _tmp83A=({const char*_tmp12E="fresh_label: add_ind returned bad index...";_tag_fat(_tmp12E,sizeof(char),43U);});_tmp78F.f1=_tmp83A;});_tmp78F;});void*_tmp12C[1];_tmp12C[0]=& _tmp12D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp12C,sizeof(void*),1));});
return res;}}}
# 688
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return((struct _fat_ptr*(*)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 694
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
{struct Cyc_List_List*_tmp133=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;struct Cyc_List_List*fs=_tmp133;for(0;Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(fs))->hd)->name)!= 0;fs=fs->tl){
++ ans;}}
return Cyc_Absyn_signed_int_exp(ans,0U);}
# 704
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*);
# 710
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
return({struct _tuple9*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->f1=0,_tmp134->f2=(*a).f2,({void*_tmp83B=Cyc_Toc_typ_to_c((*a).f3);_tmp134->f3=_tmp83B;});_tmp134;});}
# 729 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp135=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp135;void*_tmp136=_stmttmp10;struct Cyc_Absyn_ArrayInfo _tmp137;if(*((int*)_tmp136)== 4){_tmp137=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp136)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp137;
return({void*_tmp83C=Cyc_Toc_typ_to_c_array(ai.elt_type);Cyc_Absyn_cstar_type(_tmp83C,ai.tq);});}}else{
return Cyc_Toc_typ_to_c(t);};}
# 736
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 738
struct Cyc_Absyn_Aggrfield*_tmp138=({struct Cyc_Absyn_Aggrfield*_tmp139=_cycalloc(sizeof(*_tmp139));*_tmp139=*f;_tmp139;});struct Cyc_Absyn_Aggrfield*ans=_tmp138;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 745
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}
# 749
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp13A=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp13A;void*_tmp13B=_stmttmp11;struct Cyc_Absyn_Tvar*_tmp13C;if(*((int*)_tmp13B)== 2){_tmp13C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13B)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp13C;
return Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tbk);}}else{
return 0;};}
# 755
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp13D=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp12=_tmp13D;struct Cyc_Absyn_Kind*_tmp13E=_stmttmp12;if(((struct Cyc_Absyn_Kind*)_tmp13E)->kind == Cyc_Absyn_AnyKind)
return 1;else{
return 0;};}
# 761
static int Cyc_Toc_is_void_star(void*t){
void*_tmp13F=Cyc_Absyn_compress(t);void*_stmttmp13=_tmp13F;void*_tmp140=_stmttmp13;void*_tmp141;if(*((int*)_tmp140)== 3){_tmp141=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140)->f1).elt_type;{void*t2=_tmp141;
return Cyc_Tcutil_is_void_type(t2);}}else{
return 0;};}
# 767
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 770
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 774
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp142=t;void**_tmp144;struct Cyc_Absyn_Datatypedecl*_tmp143;struct Cyc_Absyn_Enumdecl*_tmp145;struct Cyc_Absyn_Aggrdecl*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp148;void*_tmp14C;struct Cyc_Absyn_Typedefdecl*_tmp14B;struct Cyc_List_List*_tmp14A;struct _tuple1*_tmp149;struct Cyc_List_List*_tmp14E;enum Cyc_Absyn_AggrKind _tmp14D;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp155;struct Cyc_Absyn_VarargInfo*_tmp154;int _tmp153;struct Cyc_List_List*_tmp152;void*_tmp151;struct Cyc_Absyn_Tqual _tmp150;unsigned _tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Tqual _tmp157;void*_tmp156;void*_tmp15C;struct Cyc_Absyn_Tqual _tmp15B;void*_tmp15A;struct Cyc_Absyn_Tvar*_tmp15D;void**_tmp15E;struct Cyc_List_List*_tmp15F;struct _tuple1*_tmp160;struct Cyc_List_List*_tmp163;union Cyc_Absyn_AggrInfo _tmp162;void*_tmp161;struct Cyc_Absyn_Datatypefield*_tmp165;struct Cyc_Absyn_Datatypedecl*_tmp164;switch(*((int*)_tmp142)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)){case 0:
 return t;case 18:
# 786
 return Cyc_Absyn_void_type;case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1).KnownDatatypefield).tag == 2){_tmp164=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1).KnownDatatypefield).val).f1;_tmp165=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp164;struct Cyc_Absyn_Datatypefield*tuf=_tmp165;
# 788
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{
# 790
({struct Cyc_Warn_String_Warn_Warg_struct _tmp167=({struct Cyc_Warn_String_Warn_Warg_struct _tmp791;_tmp791.tag=0,({struct _fat_ptr _tmp83D=({const char*_tmp168="unresolved DatatypeFieldType";_tag_fat(_tmp168,sizeof(char),29U);});_tmp791.f1=_tmp83D;});_tmp791;});void*_tmp166[1];_tmp166[0]=& _tmp167;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp166,sizeof(void*),1));});}case 1:
# 798
 goto _LL12;case 2: _LL12:
 return t;case 20: _tmp161=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1;_tmp162=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1;_tmp163=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f2;{void*c=_tmp161;union Cyc_Absyn_AggrInfo info=_tmp162;struct Cyc_List_List*ts=_tmp163;
# 833
{union Cyc_Absyn_AggrInfo _tmp175=info;if((_tmp175.UnknownAggr).tag == 1)
# 835
return ts == 0?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=0,_tmp176->f1=c,_tmp176->f2=0;_tmp176;});else{
goto _LL35;}_LL35:;}{
# 838
struct Cyc_Absyn_Aggrdecl*_tmp177=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp177;
# 840
if(ad->expected_mem_kind && ad->impl == 0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp179=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp793;_tmp793.tag=6,_tmp793.f1=ad;_tmp793;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp792;_tmp792.tag=0,({struct _fat_ptr _tmp83E=({const char*_tmp17B=" was never defined.";_tag_fat(_tmp17B,sizeof(char),20U);});_tmp792.f1=_tmp83E;});_tmp792;});void*_tmp178[2];_tmp178[0]=& _tmp179,_tmp178[1]=& _tmp17A;Cyc_Warn_warn2(0U,_tag_fat(_tmp178,sizeof(void*),2));});
# 843
if((int)ad->kind == (int)Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp17C=ad->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp17C;
if(fs == 0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(1;((struct Cyc_List_List*)_check_null(fs))->tl != 0;fs=fs->tl){;}{
void*_tmp17D=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;void*last_type=_tmp17D;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp795;_tmp795.tag=6,_tmp795.f1=ad;_tmp795;});struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp794;_tmp794.tag=0,({struct _fat_ptr _tmp83F=({const char*_tmp181=" ended up being abstract.";_tag_fat(_tmp181,sizeof(char),26U);});_tmp794.f1=_tmp83F;});_tmp794;});void*_tmp17E[2];_tmp17E[0]=& _tmp17F,_tmp17E[1]=& _tmp180;Cyc_Warn_warn2(0U,_tag_fat(_tmp17E,sizeof(void*),2));});{
# 854
struct _RegionHandle _tmp182=_new_region("r");struct _RegionHandle*r=& _tmp182;_push_region(r);
{struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp183;
void*_tmp184=Cyc_Tcutil_rsubstitute(r,inst,last_type);void*t=_tmp184;
if(Cyc_Toc_is_abstract_type(t)){void*_tmp185=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp185;}{
void*_tmp186=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_npop_handler(0);return _tmp186;}}
# 855
;_pop_region();}}
# 860
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 15: _tmp160=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1;{struct _tuple1*tdn=_tmp160;
return t;}case 16: _tmp15F=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp142)->f1)->f1;{struct Cyc_List_List*fs=_tmp15F;
Cyc_Toc_enumfields_to_c(fs);return t;}case 4:
# 870
 return Cyc_Absyn_uint_type;case 3:
 return Cyc_Toc_rgn_type();case 17:
 return t;default:
# 875
 return Cyc_Toc_void_star_type();}case 1: _tmp15E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp142)->f2;{void**t2=_tmp15E;
# 778
if(*t2 != 0)
return Cyc_Toc_typ_to_c((void*)_check_null(*t2));
*t2=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}case 2: _tmp15D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp15D;
# 783
if((int)(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind == (int)Cyc_Absyn_AnyKind)
return Cyc_Absyn_void_type;
return Cyc_Toc_void_star_type();}case 3: _tmp15A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->f1).elt_type;_tmp15B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->f1).elt_tq;_tmp15C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->f1).ptr_atts).bounds;{void*t2=_tmp15A;struct Cyc_Absyn_Tqual tq=_tmp15B;void*bnds=_tmp15C;
# 794
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)== 0)
return Cyc_Toc_fat_ptr_type();
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}case 4: _tmp156=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp142)->f1).elt_type;_tmp157=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp142)->f1).tq;_tmp158=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp142)->f1).num_elts;_tmp159=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp142)->f1).zt_loc;{void*t2=_tmp156;struct Cyc_Absyn_Tqual tq=_tmp157;struct Cyc_Absyn_Exp*e=_tmp158;unsigned ztl=_tmp159;
# 801
return({void*_tmp843=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _tmp842=tq;struct Cyc_Absyn_Exp*_tmp841=e;void*_tmp840=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp843,_tmp842,_tmp841,_tmp840,ztl);});}case 5: _tmp150=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142)->f1).ret_tqual;_tmp151=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142)->f1).ret_type;_tmp152=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142)->f1).args;_tmp153=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142)->f1).c_varargs;_tmp154=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142)->f1).cyc_varargs;_tmp155=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142)->f1).attributes;{struct Cyc_Absyn_Tqual tq2=_tmp150;void*t2=_tmp151;struct Cyc_List_List*args=_tmp152;int c_vararg=_tmp153;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp154;struct Cyc_List_List*atts=_tmp155;
# 805
struct Cyc_List_List*_tmp169=Cyc_Atts_atts2c(atts);struct Cyc_List_List*new_atts=_tmp169;
struct Cyc_List_List*_tmp16A=((struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Toc_arg_to_c,args);struct Cyc_List_List*new_args=_tmp16A;
if(cyc_vararg != 0){
# 809
void*_tmp16B=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));void*t=_tmp16B;
struct _tuple9*_tmp16C=({struct _tuple9*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=cyc_vararg->name,_tmp16E->f2=cyc_vararg->tq,_tmp16E->f3=t;_tmp16E;});struct _tuple9*vararg=_tmp16C;
new_args=({struct Cyc_List_List*_tmp844=new_args;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp844,({struct Cyc_List_List*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->hd=vararg,_tmp16D->tl=0;_tmp16D;}));});}
# 813
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=5,(_tmp16F->f1).tvars=0,(_tmp16F->f1).effect=0,(_tmp16F->f1).ret_tqual=tq2,({void*_tmp845=Cyc_Toc_typ_to_c(t2);(_tmp16F->f1).ret_type=_tmp845;}),(_tmp16F->f1).args=new_args,(_tmp16F->f1).c_varargs=c_vararg,(_tmp16F->f1).cyc_varargs=0,(_tmp16F->f1).rgn_po=0,(_tmp16F->f1).attributes=new_atts,(_tmp16F->f1).requires_clause=0,(_tmp16F->f1).requires_relns=0,(_tmp16F->f1).ensures_clause=0,(_tmp16F->f1).ensures_relns=0,(_tmp16F->f1).return_value=0;_tmp16F;});}case 6: _tmp14F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp142)->f1;{struct Cyc_List_List*tqs=_tmp14F;
# 818
struct Cyc_List_List*_tmp170=0;struct Cyc_List_List*tqs2=_tmp170;
for(1;tqs != 0;tqs=tqs->tl){
tqs2=({struct Cyc_List_List*_tmp172=_cycalloc(sizeof(*_tmp172));({struct _tuple23*_tmp847=({struct _tuple23*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->f1=(*((struct _tuple23*)tqs->hd)).f1,({void*_tmp846=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqs->hd)).f2);_tmp171->f2=_tmp846;});_tmp171;});_tmp172->hd=_tmp847;}),_tmp172->tl=tqs2;_tmp172;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(tqs2));}case 7: _tmp14D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp142)->f1;_tmp14E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp142)->f2;{enum Cyc_Absyn_AggrKind k=_tmp14D;struct Cyc_List_List*fs=_tmp14E;
# 827
struct Cyc_List_List*_tmp173=0;struct Cyc_List_List*fs2=_tmp173;
for(1;fs != 0;fs=fs->tl){
fs2=({struct Cyc_List_List*_tmp174=_cycalloc(sizeof(*_tmp174));({struct Cyc_Absyn_Aggrfield*_tmp849=({struct Cyc_Absyn_Aggrfield*_tmp848=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_tmp848,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_tmp174->hd=_tmp849;}),_tmp174->tl=fs2;_tmp174;});}
return({enum Cyc_Absyn_AggrKind _tmp84A=k;Cyc_Toc_add_anon_aggr_type(_tmp84A,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fs2));});}case 8: _tmp149=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp142)->f1;_tmp14A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp142)->f2;_tmp14B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp142)->f3;_tmp14C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp142)->f4;{struct _tuple1*tdn=_tmp149;struct Cyc_List_List*ts=_tmp14A;struct Cyc_Absyn_Typedefdecl*td=_tmp14B;void*topt=_tmp14C;
# 866
if(topt == 0)
return ts == 0?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->tag=8,_tmp187->f1=tdn,_tmp187->f2=0,_tmp187->f3=td,_tmp187->f4=0;_tmp187;});else{
# 869
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->tag=8,_tmp188->f1=tdn,_tmp188->f2=0,_tmp188->f3=td,({void*_tmp84B=Cyc_Toc_typ_to_c(topt);_tmp188->f4=_tmp84B;});_tmp188;});}}case 9: _tmp148=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp142)->f1;{struct Cyc_Absyn_Exp*e=_tmp148;
# 880
return t;}case 11: _tmp147=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp142)->f1;{struct Cyc_Absyn_Exp*e=_tmp147;
# 885
Cyc_Toc_exptypes_to_c(e);
return e->topt == 0?t: Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp142)->f1)->r)){case 0: _tmp146=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp142)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp146;
# 888
Cyc_Toc_aggrdecl_to_c(ad);
return Cyc_Toc_aggrdecl_type(ad->name,(int)ad->kind == (int)Cyc_Absyn_UnionA?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq);}case 1: _tmp145=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp142)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp145;
# 891
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _tmp143=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp142)->f1)->r)->f1;_tmp144=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp142)->f2;{struct Cyc_Absyn_Datatypedecl*dd=_tmp143;void**t=_tmp144;
# 894
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*((void**)_check_null(t)));}}};}
# 899
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
void*_tmp189=t;struct Cyc_Absyn_Tqual _tmp18B;void*_tmp18A;if(*((int*)_tmp189)== 4){_tmp18A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189)->f1).elt_type;_tmp18B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189)->f1).tq;{void*t2=_tmp18A;struct Cyc_Absyn_Tqual tq=_tmp18B;
# 902
return({void*_tmp84C=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp84C,e);});}}else{
return Cyc_Toc_cast_it(t,e);};}
# 909
static int Cyc_Toc_atomic_type(void*t){
void*_tmp18C=Cyc_Absyn_compress(t);void*_stmttmp14=_tmp18C;void*_tmp18D=_stmttmp14;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18F;void*_tmp190;struct Cyc_List_List*_tmp192;void*_tmp191;switch(*((int*)_tmp18D)){case 2:
 return 0;case 0: _tmp191=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f1;_tmp192=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18D)->f2;{void*c=_tmp191;struct Cyc_List_List*ts=_tmp192;
# 913
void*_tmp193=c;struct Cyc_Absyn_Datatypefield*_tmp195;struct Cyc_Absyn_Datatypedecl*_tmp194;union Cyc_Absyn_AggrInfo _tmp196;switch(*((int*)_tmp193)){case 0:
 goto _LL15;case 1: _LL15: goto _LL17;case 2: _LL17: goto _LL19;case 4: _LL19:
 goto _LL1B;case 15: _LL1B: goto _LL1D;case 16: _LL1D: return 1;case 18:
 goto _LL21;case 3: _LL21:
 goto _LL23;case 17: _LL23:
 return 0;case 20: _tmp196=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp193)->f1;{union Cyc_Absyn_AggrInfo info=_tmp196;
# 920
{union Cyc_Absyn_AggrInfo _tmp197=info;if((_tmp197.UnknownAggr).tag == 1)
return 0;else{
goto _LL2A;}_LL2A:;}{
# 924
struct Cyc_Absyn_Aggrdecl*_tmp198=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp198;
if(ad->impl == 0)
return 0;
{struct Cyc_List_List*_tmp199=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp199;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).tag == 2){_tmp194=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f1;_tmp195=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp194;struct Cyc_Absyn_Datatypefield*tuf=_tmp195;
# 931
{struct Cyc_List_List*_tmp19A=tuf->typs;struct Cyc_List_List*tqs=_tmp19A;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}}
return 1;}}else{goto _LL28;}default: _LL28:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp797;_tmp797.tag=0,({struct _fat_ptr _tmp84D=({const char*_tmp19E="atomic_typ: bad type ";_tag_fat(_tmp19E,sizeof(char),22U);});_tmp797.f1=_tmp84D;});_tmp797;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp796;_tmp796.tag=2,_tmp796.f1=(void*)t;_tmp796;});void*_tmp19B[2];_tmp19B[0]=& _tmp19C,_tmp19B[1]=& _tmp19D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp19B,sizeof(void*),2));});};}case 5:
# 936
 return 1;case 4: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18D)->f1).elt_type;{void*t=_tmp190;
return Cyc_Toc_atomic_type(t);}case 7: _tmp18F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18D)->f2;{struct Cyc_List_List*fs=_tmp18F;
# 944
for(1;fs != 0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 6: _tmp18E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18D)->f1;{struct Cyc_List_List*tqs=_tmp18E;
# 948
for(1;tqs != 0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f2))return 0;}
return 1;}case 3:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp799;_tmp799.tag=0,({struct _fat_ptr _tmp84E=({const char*_tmp1A2="atomic_typ:  bad type ";_tag_fat(_tmp1A2,sizeof(char),23U);});_tmp799.f1=_tmp84E;});_tmp799;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp798;_tmp798.tag=2,_tmp798.f1=(void*)t;_tmp798;});void*_tmp19F[2];_tmp19F[0]=& _tmp1A0,_tmp19F[1]=& _tmp1A1;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp19F,sizeof(void*),2));});};}
# 957
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_tmp1A3=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp1A3;void*_tmp1A4=_stmttmp15;struct Cyc_List_List*_tmp1A5;union Cyc_Absyn_AggrInfo _tmp1A6;switch(*((int*)_tmp1A4)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)== 20){_tmp1A6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp1A6;
# 960
struct Cyc_Absyn_Aggrdecl*_tmp1A7=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp1A7;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79A;_tmp79A.tag=0,({struct _fat_ptr _tmp84F=({const char*_tmp1AA="is_poly_field: type missing fields";_tag_fat(_tmp1AA,sizeof(char),35U);});_tmp79A.f1=_tmp84F;});_tmp79A;});void*_tmp1A8[1];_tmp1A8[0]=& _tmp1A9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp1A8,sizeof(void*),1));});
_tmp1A5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7: _tmp1A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A4)->f2;_LL4: {struct Cyc_List_List*fs=_tmp1A5;
# 965
struct Cyc_Absyn_Aggrfield*_tmp1AB=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*field=_tmp1AB;
if(field == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79C;_tmp79C.tag=0,({struct _fat_ptr _tmp850=({const char*_tmp1AF="is_poly_field: bad field ";_tag_fat(_tmp1AF,sizeof(char),26U);});_tmp79C.f1=_tmp850;});_tmp79C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79B;_tmp79B.tag=0,_tmp79B.f1=*f;_tmp79B;});void*_tmp1AC[2];_tmp1AC[0]=& _tmp1AD,_tmp1AC[1]=& _tmp1AE;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp1AC,sizeof(void*),2));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp79E;_tmp79E.tag=0,({struct _fat_ptr _tmp851=({const char*_tmp1B3="is_poly_field: bad type ";_tag_fat(_tmp1B3,sizeof(char),25U);});_tmp79E.f1=_tmp851;});_tmp79E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1B2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp79D;_tmp79D.tag=2,_tmp79D.f1=(void*)t;_tmp79D;});void*_tmp1B0[2];_tmp1B0[0]=& _tmp1B1,_tmp1B0[1]=& _tmp1B2;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp1B0,sizeof(void*),2));});};}
# 976
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B4=e->r;void*_stmttmp16=_tmp1B4;void*_tmp1B5=_stmttmp16;struct _fat_ptr*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct _fat_ptr*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;switch(*((int*)_tmp1B5)){case 21: _tmp1B8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B5)->f1;_tmp1B9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B5)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1B8;struct _fat_ptr*f=_tmp1B9;
# 979
return Cyc_Toc_is_poly_field((void*)_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}case 22: _tmp1B6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B5)->f1;_tmp1B7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B5)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1B6;struct _fat_ptr*f=_tmp1B7;
# 982
void*_tmp1BA=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp17=_tmp1BA;void*_tmp1BB=_stmttmp17;void*_tmp1BC;if(*((int*)_tmp1BB)== 3){_tmp1BC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BB)->f1).elt_type;{void*t=_tmp1BC;
# 984
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A0;_tmp7A0.tag=0,({struct _fat_ptr _tmp852=({const char*_tmp1C0="is_poly_project: bad type ";_tag_fat(_tmp1C0,sizeof(char),27U);});_tmp7A0.f1=_tmp852;});_tmp7A0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp79F;_tmp79F.tag=2,_tmp79F.f1=(void*)_check_null(e1->topt);_tmp79F;});void*_tmp1BD[2];_tmp1BD[0]=& _tmp1BE,_tmp1BD[1]=& _tmp1BF;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp1BD,sizeof(void*),2));});};}default:
# 987
 return 0;};}
# 992
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C1[1];_tmp1C1[0]=s;({struct Cyc_Absyn_Exp*_tmp853=fn_e;Cyc_Toc_fncall_exp_dl(_tmp853,_tag_fat(_tmp1C1,sizeof(struct Cyc_Absyn_Exp*),1));});});}
# 997
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
return({struct Cyc_Absyn_Exp*_tmp1C2[2];_tmp1C2[0]=s,_tmp1C2[1]=n;({struct Cyc_Absyn_Exp*_tmp854=fn_e;Cyc_Toc_fncall_exp_dl(_tmp854,_tag_fat(_tmp1C2,sizeof(struct Cyc_Absyn_Exp*),2));});});}
# 1002
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C3[2];_tmp1C3[0]=rgn,_tmp1C3[1]=s;({struct Cyc_Absyn_Exp*_tmp855=Cyc_Toc__region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp855,_tag_fat(_tmp1C3,sizeof(struct Cyc_Absyn_Exp*),2));});});}
# 1006
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp1C4[2];_tmp1C4[0]=rgn,_tmp1C4[1]=s;({struct Cyc_Absyn_Exp*_tmp856=Cyc_Toc__fast_region_malloc_e;Cyc_Toc_fncall_exp_dl(_tmp856,_tag_fat(_tmp1C4,sizeof(struct Cyc_Absyn_Exp*),2));});});}
# 1010
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp1C5[3];_tmp1C5[0]=rgn,_tmp1C5[1]=s,_tmp1C5[2]=n;({struct Cyc_Absyn_Exp*_tmp857=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp857,_tag_fat(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),3));});});}
# 1014
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(({void*_tmp1C6=0U;({struct Cyc_Absyn_Exp*_tmp858=Cyc_Toc__throw_match_e;Cyc_Toc_fncall_exp_dl(_tmp858,_tag_fat(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),0));});}),0U);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1027
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_tmp1C7=e->r;void*_stmttmp18=_tmp1C7;void*_tmp1C8=_stmttmp18;if(*((int*)_tmp1C8)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1C8)->f1).Wstring_c).tag){case 8:
 goto _LL4;case 9: _LL4: {
# 1032
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({struct Cyc_Absyn_Decl*_tmp859=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->tag=0,_tmp1C9->f1=vd;_tmp1C9;}),0U);_tmp1CA->hd=_tmp859;}),_tmp1CA->tl=Cyc_Toc_result_decls;_tmp1CA;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5:
# 1040
 xexp=({void*_tmp85A=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp85A,e);});
# 1042
xplussz=({void*_tmp85B=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp85B,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 1045
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1CB[3];({struct _tuple19*_tmp85E=({struct _tuple19*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->f1=0,_tmp1CC->f2=xexp;_tmp1CC;});_tmp1CB[0]=_tmp85E;}),({
struct _tuple19*_tmp85D=({struct _tuple19*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=0,_tmp1CD->f2=xexp;_tmp1CD;});_tmp1CB[1]=_tmp85D;}),({
struct _tuple19*_tmp85C=({struct _tuple19*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->f1=0,_tmp1CE->f2=xplussz;_tmp1CE;});_tmp1CB[2]=_tmp85C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CB,sizeof(struct _tuple19*),3));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1080 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1D0=_region_malloc(r,sizeof(*_tmp1D0));_tmp1D0->break_lab=0,_tmp1D0->continue_lab=0,_tmp1D0->fallthru_info=0,_tmp1D0->toplevel=1,({int*_tmp85F=({int*_tmp1CF=_region_malloc(r,sizeof(*_tmp1CF));*_tmp1CF=0;_tmp1CF;});_tmp1D0->in_lhs=_tmp85F;}),_tmp1D0->rgn=r;_tmp1D0;});}
# 1084
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D1=nv;int _tmp1D2;_tmp1D2=_tmp1D1->toplevel;{int t=_tmp1D2;
return t;}}
# 1088
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1D3=e;int*_tmp1D8;int _tmp1D7;struct Cyc_Toc_FallthruInfo*_tmp1D6;struct _fat_ptr**_tmp1D5;struct _fat_ptr**_tmp1D4;_tmp1D4=_tmp1D3->break_lab;_tmp1D5=_tmp1D3->continue_lab;_tmp1D6=_tmp1D3->fallthru_info;_tmp1D7=_tmp1D3->toplevel;_tmp1D8=_tmp1D3->in_lhs;{struct _fat_ptr**b=_tmp1D4;struct _fat_ptr**c=_tmp1D5;struct Cyc_Toc_FallthruInfo*f=_tmp1D6;int t=_tmp1D7;int*lhs=_tmp1D8;
return({struct Cyc_Toc_Env*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));_tmp1D9->break_lab=b,_tmp1D9->continue_lab=c,_tmp1D9->fallthru_info=f,_tmp1D9->toplevel=0,_tmp1D9->in_lhs=lhs,_tmp1D9->rgn=r;_tmp1D9;});}}
# 1092
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DA=e;int*_tmp1DF;int _tmp1DE;struct Cyc_Toc_FallthruInfo*_tmp1DD;struct _fat_ptr**_tmp1DC;struct _fat_ptr**_tmp1DB;_tmp1DB=_tmp1DA->break_lab;_tmp1DC=_tmp1DA->continue_lab;_tmp1DD=_tmp1DA->fallthru_info;_tmp1DE=_tmp1DA->toplevel;_tmp1DF=_tmp1DA->in_lhs;{struct _fat_ptr**b=_tmp1DB;struct _fat_ptr**c=_tmp1DC;struct Cyc_Toc_FallthruInfo*f=_tmp1DD;int t=_tmp1DE;int*lhs=_tmp1DF;
return({struct Cyc_Toc_Env*_tmp1E0=_region_malloc(r,sizeof(*_tmp1E0));_tmp1E0->break_lab=b,_tmp1E0->continue_lab=c,_tmp1E0->fallthru_info=f,_tmp1E0->toplevel=1,_tmp1E0->in_lhs=lhs,_tmp1E0->rgn=r;_tmp1E0;});}}
# 1096
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1E1=nv;int*_tmp1E2;_tmp1E2=_tmp1E1->in_lhs;{int*b=_tmp1E2;
return*b;}}
# 1100
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1E3=e;int*_tmp1E4;_tmp1E4=_tmp1E3->in_lhs;{int*lhs=_tmp1E4;
*lhs=b;}}
# 1105
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E5=e;int*_tmp1EA;int _tmp1E9;struct Cyc_Toc_FallthruInfo*_tmp1E8;struct _fat_ptr**_tmp1E7;struct _fat_ptr**_tmp1E6;_tmp1E6=_tmp1E5->break_lab;_tmp1E7=_tmp1E5->continue_lab;_tmp1E8=_tmp1E5->fallthru_info;_tmp1E9=_tmp1E5->toplevel;_tmp1EA=_tmp1E5->in_lhs;{struct _fat_ptr**b=_tmp1E6;struct _fat_ptr**c=_tmp1E7;struct Cyc_Toc_FallthruInfo*f=_tmp1E8;int t=_tmp1E9;int*lhs=_tmp1EA;
return({struct Cyc_Toc_Env*_tmp1EB=_region_malloc(r,sizeof(*_tmp1EB));_tmp1EB->break_lab=b,_tmp1EB->continue_lab=c,_tmp1EB->fallthru_info=f,_tmp1EB->toplevel=t,_tmp1EB->in_lhs=lhs,_tmp1EB->rgn=r;_tmp1EB;});}}
# 1112
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;int*_tmp1F1;int _tmp1F0;struct Cyc_Toc_FallthruInfo*_tmp1EF;struct _fat_ptr**_tmp1EE;struct _fat_ptr**_tmp1ED;_tmp1ED=_tmp1EC->break_lab;_tmp1EE=_tmp1EC->continue_lab;_tmp1EF=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1F1=_tmp1EC->in_lhs;{struct _fat_ptr**b=_tmp1ED;struct _fat_ptr**c=_tmp1EE;struct Cyc_Toc_FallthruInfo*f=_tmp1EF;int t=_tmp1F0;int*lhs=_tmp1F1;
return({struct Cyc_Toc_Env*_tmp1F2=_region_malloc(r,sizeof(*_tmp1F2));_tmp1F2->break_lab=0,_tmp1F2->continue_lab=0,_tmp1F2->fallthru_info=f,_tmp1F2->toplevel=t,_tmp1F2->in_lhs=lhs,_tmp1F2->rgn=r;_tmp1F2;});}}
# 1118
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1122
struct Cyc_Toc_Env*_tmp1F3=e;int*_tmp1F8;int _tmp1F7;struct Cyc_Toc_FallthruInfo*_tmp1F6;struct _fat_ptr**_tmp1F5;struct _fat_ptr**_tmp1F4;_tmp1F4=_tmp1F3->break_lab;_tmp1F5=_tmp1F3->continue_lab;_tmp1F6=_tmp1F3->fallthru_info;_tmp1F7=_tmp1F3->toplevel;_tmp1F8=_tmp1F3->in_lhs;{struct _fat_ptr**b=_tmp1F4;struct _fat_ptr**c=_tmp1F5;struct Cyc_Toc_FallthruInfo*f=_tmp1F6;int t=_tmp1F7;int*lhs=_tmp1F8;
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));
_tmp1FB->label=fallthru_l,_tmp1FB->binders=fallthru_binders;_tmp1FB;});
return({struct Cyc_Toc_Env*_tmp1FA=_region_malloc(r,sizeof(*_tmp1FA));({struct _fat_ptr**_tmp860=({struct _fat_ptr**_tmp1F9=_region_malloc(r,sizeof(*_tmp1F9));*_tmp1F9=break_l;_tmp1F9;});_tmp1FA->break_lab=_tmp860;}),_tmp1FA->continue_lab=c,_tmp1FA->fallthru_info=fi,_tmp1FA->toplevel=t,_tmp1FA->in_lhs=lhs,_tmp1FA->rgn=r;_tmp1FA;});}}
# 1127
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1129
struct Cyc_Toc_Env*_tmp1FC=e;int*_tmp201;int _tmp200;struct Cyc_Toc_FallthruInfo*_tmp1FF;struct _fat_ptr**_tmp1FE;struct _fat_ptr**_tmp1FD;_tmp1FD=_tmp1FC->break_lab;_tmp1FE=_tmp1FC->continue_lab;_tmp1FF=_tmp1FC->fallthru_info;_tmp200=_tmp1FC->toplevel;_tmp201=_tmp1FC->in_lhs;{struct _fat_ptr**b=_tmp1FD;struct _fat_ptr**c=_tmp1FE;struct Cyc_Toc_FallthruInfo*f=_tmp1FF;int t=_tmp200;int*lhs=_tmp201;
return({struct Cyc_Toc_Env*_tmp203=_region_malloc(r,sizeof(*_tmp203));({struct _fat_ptr**_tmp861=({struct _fat_ptr**_tmp202=_region_malloc(r,sizeof(*_tmp202));*_tmp202=break_l;_tmp202;});_tmp203->break_lab=_tmp861;}),_tmp203->continue_lab=c,_tmp203->fallthru_info=0,_tmp203->toplevel=t,_tmp203->in_lhs=lhs,_tmp203->rgn=r;_tmp203;});}}
# 1136
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*next_l){
# 1138
struct Cyc_Toc_Env*_tmp204=e;int*_tmp209;int _tmp208;struct Cyc_Toc_FallthruInfo*_tmp207;struct _fat_ptr**_tmp206;struct _fat_ptr**_tmp205;_tmp205=_tmp204->break_lab;_tmp206=_tmp204->continue_lab;_tmp207=_tmp204->fallthru_info;_tmp208=_tmp204->toplevel;_tmp209=_tmp204->in_lhs;{struct _fat_ptr**b=_tmp205;struct _fat_ptr**c=_tmp206;struct Cyc_Toc_FallthruInfo*f=_tmp207;int t=_tmp208;int*lhs=_tmp209;
return({struct Cyc_Toc_Env*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->break_lab=0,_tmp20B->continue_lab=c,({struct Cyc_Toc_FallthruInfo*_tmp862=({struct Cyc_Toc_FallthruInfo*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));_tmp20A->label=next_l,_tmp20A->binders=0;_tmp20A;});_tmp20B->fallthru_info=_tmp862;}),_tmp20B->toplevel=t,_tmp20B->in_lhs=lhs,_tmp20B->rgn=r;_tmp20B;});}}
# 1151 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*);
# 1155
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Stmt*);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Fndecl*,int);struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 1158
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
for(1;l != 0;l=l->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple32*)l->hd)).f2);}}
# 1162
static void Cyc_Toc_asm_iolist_toc(struct Cyc_Toc_Env*nv,struct Cyc_List_List*l){
for(1;l != 0;l=l->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple32*)l->hd)).f2);}}
# 1167
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp20C=e->annot;void*_stmttmp19=_tmp20C;void*_tmp20D=_stmttmp19;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NoCheck)
return 0;else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NullOnly)
goto _LL6;else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NullAndFatBound){_LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_NullAndThinBound){_LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_FatBound)
goto _LLC;else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp20D)->tag == Cyc_InsertChecks_ThinBound){_LLC:
 return 0;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A1;_tmp7A1.tag=0,({struct _fat_ptr _tmp863=({const char*_tmp210="Toc: do_null_check";_tag_fat(_tmp210,sizeof(char),19U);});_tmp7A1.f1=_tmp863;});_tmp7A1;});void*_tmp20E[1];_tmp20E[0]=& _tmp20F;({unsigned _tmp864=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp864,_tag_fat(_tmp20E,sizeof(void*),1));});});}}}}}};}
# 1183
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1185
int ans;
int _tmp211=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp211;
void*_tmp212=Cyc_Absyn_compress((void*)_check_null(ptr->topt));void*old_typ=_tmp212;
void*_tmp213=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp213;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp214=old_typ;void*_tmp218;void*_tmp217;struct Cyc_Absyn_Tqual _tmp216;void*_tmp215;if(*((int*)_tmp214)== 3){_tmp215=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).elt_type;_tmp216=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).elt_tq;_tmp217=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).ptr_atts).bounds;_tmp218=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).ptr_atts).zero_term;{void*ta=_tmp215;struct Cyc_Absyn_Tqual tq=_tmp216;void*b=_tmp217;void*zt=_tmp218;
# 1196
{void*_tmp219=annot;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NoCheck){
# 1198
if(!((unsigned)({void*_tmp865=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp865,b);}))){
# 1201
void*_tmp21C=({void*_tmp866=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp866,tq);});void*newt=_tmp21C;
({void*_tmp869=({void*_tmp868=newt;Cyc_Toc_cast_it_r(_tmp868,({struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp867,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp869;});
ptr->topt=newt;}
# 1205
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullOnly){
# 1208
if(!((unsigned)({void*_tmp86A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp86A,b);}))){
# 1210
void*_tmp21D=({void*_tmp86B=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_tmp86B,tq);});void*newt=_tmp21D;
({void*_tmp86E=({void*_tmp86D=newt;Cyc_Toc_cast_it_r(_tmp86D,({struct Cyc_Absyn_Exp*_tmp86C=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp86C,Cyc_Toc_curr_sp,0U);}));});ptr->r=_tmp86E;});
ptr->topt=newt;
# 1217
if(index != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A2;_tmp7A2.tag=0,({struct _fat_ptr _tmp86F=({const char*_tmp220="subscript of ? with no bounds check but need null check";_tag_fat(_tmp220,sizeof(char),56U);});_tmp7A2.f1=_tmp86F;});_tmp7A2;});void*_tmp21E[1];_tmp21E[0]=& _tmp21F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp21E,sizeof(void*),1));});}
# 1221
({void*_tmp873=({void*_tmp872=new_typ;Cyc_Toc_cast_it_r(_tmp872,({struct Cyc_Absyn_Exp*_tmp221[1];({struct Cyc_Absyn_Exp*_tmp870=Cyc_Absyn_new_exp(ptr->r,0U);_tmp221[0]=_tmp870;});({struct Cyc_Absyn_Exp*_tmp871=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp871,_tag_fat(_tmp221,sizeof(struct Cyc_Absyn_Exp*),1));});}));});ptr->r=_tmp873;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullAndFatBound)
goto _LLD;else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_FatBound){_LLD: {
# 1227
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp878=({void*_tmp877=newt;Cyc_Toc_cast_it_r(_tmp877,({struct Cyc_Absyn_Exp*_tmp222[3];({
struct Cyc_Absyn_Exp*_tmp875=Cyc_Absyn_new_exp(ptr->r,0U);_tmp222[0]=_tmp875;}),({
struct Cyc_Absyn_Exp*_tmp874=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp222[1]=_tmp874;}),_tmp222[2]=ind;({struct Cyc_Absyn_Exp*_tmp876=Cyc_Toc__check_fat_subscript_e;Cyc_Toc_fncall_exp_dl(_tmp876,_tag_fat(_tmp222,sizeof(struct Cyc_Absyn_Exp*),3));});}));});
# 1230
ptr->r=_tmp878;});
# 1234
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_NullAndThinBound){_tmp21B=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->f1;{struct Cyc_Absyn_Exp*bd=_tmp21B;
# 1238
fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp21A=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->tag == Cyc_InsertChecks_ThinBound){_tmp21A=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp219)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_tmp21A;
# 1241
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1246
struct _tuple13 _tmp223=Cyc_Evexp_eval_const_uint_exp(bd);struct _tuple13 _stmttmp1A=_tmp223;struct _tuple13 _tmp224=_stmttmp1A;int _tmp226;unsigned _tmp225;_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;{unsigned i=_tmp225;int valid=_tmp226;
if((!valid || i != (unsigned)1)|| !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1249
({void*_tmp87D=({void*_tmp87C=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp87C,({struct Cyc_Absyn_Exp*_tmp227[4];({
struct Cyc_Absyn_Exp*_tmp87A=Cyc_Absyn_new_exp(ptr->r,0U);_tmp227[0]=_tmp87A;}),_tmp227[1]=bd,({
struct Cyc_Absyn_Exp*_tmp879=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp227[2]=_tmp879;}),_tmp227[3]=ind;({struct Cyc_Absyn_Exp*_tmp87B=fn_e;Cyc_Toc_fncall_exp_dl(_tmp87B,_tag_fat(_tmp227,sizeof(struct Cyc_Absyn_Exp*),4));});}));});
# 1249
ptr->r=_tmp87D;});
# 1252
ans=1;}else{
# 1255
if(Cyc_Toc_is_zero(bd)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1258
({void*_tmp881=({void*_tmp880=new_typ;Cyc_Toc_cast_it_r(_tmp880,({struct Cyc_Absyn_Exp*_tmp228[1];({struct Cyc_Absyn_Exp*_tmp87E=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp228[0]=_tmp87E;});({struct Cyc_Absyn_Exp*_tmp87F=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp87F,_tag_fat(_tmp228,sizeof(struct Cyc_Absyn_Exp*),1));});}));});
# 1258
ptr->r=_tmp881;});
# 1260
ans=0;}else{
# 1264
fn_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);
({void*_tmp885=({void*_tmp884=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_tmp884,({struct Cyc_Absyn_Exp*_tmp229[3];({
struct Cyc_Absyn_Exp*_tmp882=Cyc_Absyn_new_exp(ptr->r,0U);_tmp229[0]=_tmp882;}),_tmp229[1]=bd,_tmp229[2]=ind;({struct Cyc_Absyn_Exp*_tmp883=fn_e;Cyc_Toc_fncall_exp_dl(_tmp883,_tag_fat(_tmp229,sizeof(struct Cyc_Absyn_Exp*),3));});}));});
# 1265
ptr->r=_tmp885;});
# 1267
ans=1;}}
# 1270
goto _LL5;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A3;_tmp7A3.tag=0,({struct _fat_ptr _tmp886=({const char*_tmp22C="FIX: ptr_use_to_c, bad annotation";_tag_fat(_tmp22C,sizeof(char),34U);});_tmp7A3.f1=_tmp886;});_tmp7A3;});void*_tmp22A[1];_tmp22A[0]=& _tmp22B;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp22A,sizeof(void*),1));});}}}}}}_LL5:;}
# 1273
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A4;_tmp7A4.tag=0,({struct _fat_ptr _tmp887=({const char*_tmp22F="ptr_use_to_c: non-pointer-type";_tag_fat(_tmp22F,sizeof(char),31U);});_tmp7A4.f1=_tmp887;});_tmp7A4;});void*_tmp22D[1];_tmp22D[0]=& _tmp22E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp22D,sizeof(void*),1));});};}}
# 1279
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp231=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A5;_tmp7A5.tag=0,({struct _fat_ptr _tmp888=({const char*_tmp232="Missing type in primop ";_tag_fat(_tmp232,sizeof(char),24U);});_tmp7A5.f1=_tmp888;});_tmp7A5;});void*_tmp230[1];_tmp230[0]=& _tmp231;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp230,sizeof(void*),1));});
return(void*)_check_null(e->topt);}
# 1283
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple23*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233->f1=Cyc_Toc_mt_tq,({void*_tmp889=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp233->f2=_tmp889;});_tmp233;});}
# 1288
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp234=e->r;void*_stmttmp1B=_tmp234;void*_tmp235=_stmttmp1B;int _tmp237;struct Cyc_Absyn_Exp*_tmp236;int _tmp239;struct Cyc_Absyn_Exp*_tmp238;struct Cyc_List_List*_tmp23A;switch(*((int*)_tmp235)){case 26: _tmp23A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp235)->f1;{struct Cyc_List_List*dles=_tmp23A;
# 1291
{struct Cyc_List_List*_tmp23B=dles;struct Cyc_List_List*dles2=_tmp23B;for(0;dles2 != 0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f1 != 0)
((int(*)(struct _fat_ptr))Cyc_Toc_unimp)(({const char*_tmp23C="array designators for abstract-field initialization";_tag_fat(_tmp23C,sizeof(char),52U);}));}}
_tmp238=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*))Cyc_List_length)(dles),0U);_tmp239=0;goto _LL4;}case 27: _tmp238=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp235)->f2;_tmp239=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp235)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_tmp238;int zt=_tmp239;
_tmp236=bd;_tmp237=zt;goto _LL6;}case 28: _tmp236=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp235)->f1;_tmp237=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp235)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_tmp236;int zt=_tmp237;
# 1297
bd=Cyc_Absyn_copy_exp(bd);
return zt?({struct Cyc_Absyn_Exp*_tmp88A=bd;Cyc_Absyn_add_exp(_tmp88A,Cyc_Absyn_uint_exp(1U,0U),0U);}): bd;}default:
 return 0;};}
# 1306
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1314
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp23D=e->r;void*_stmttmp1C=_tmp23D;void*_tmp23E=_stmttmp1C;struct Cyc_List_List*_tmp23F;if(*((int*)_tmp23E)== 29){_tmp23F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23E)->f3;{struct Cyc_List_List*dles2=_tmp23F;
dles=dles2;goto _LL0;}}else{
return 0;}_LL0:;}
# 1320
{void*_tmp240=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp1D=_tmp240;void*_tmp241=_stmttmp1D;struct Cyc_Absyn_Aggrdecl*_tmp242;if(*((int*)_tmp241)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)->f1).KnownAggr).tag == 2){_tmp242=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp242;
# 1322
if(ad->impl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 1328
 return 0;}_LL5:;}
# 1330
if(field_types == 0)
return 0;
for(1;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp243=(struct Cyc_Absyn_Aggrfield*)field_types->hd;struct Cyc_Absyn_Aggrfield*last_type_field=_tmp243;
for(1;dles != 0;dles=dles->tl){
struct _tuple19*_tmp244=(struct _tuple19*)dles->hd;struct _tuple19*_stmttmp1E=_tmp244;struct _tuple19*_tmp245=_stmttmp1E;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_List_List*_tmp246;_tmp246=_tmp245->f1;_tmp247=_tmp245->f2;{struct Cyc_List_List*ds=_tmp246;struct Cyc_Absyn_Exp*e2=_tmp247;
struct _fat_ptr*_tmp248=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp248;
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*_tmp249=Cyc_Toc_get_varsizeexp(e2);struct Cyc_Absyn_Exp*nested_ans=_tmp249;
if(nested_ans != 0)
return nested_ans;{
void*_tmp24A=Cyc_Absyn_compress(last_type_field->type);void*_stmttmp1F=_tmp24A;void*_tmp24B=_stmttmp1F;struct Cyc_Absyn_Exp*_tmp24D;void*_tmp24C;if(*((int*)_tmp24B)== 4){_tmp24C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24B)->f1).elt_type;_tmp24D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24B)->f1).num_elts;{void*elt_type=_tmp24C;struct Cyc_Absyn_Exp*type_bd=_tmp24D;
# 1345
if(type_bd == 0 || !Cyc_Toc_is_zero(type_bd))
return 0;
# 1351
return({struct Cyc_Absyn_Exp*_tmp88D=({struct Cyc_Absyn_Exp*_tmp24E[2];_tmp24E[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(e2)),({
# 1353
struct Cyc_Absyn_Exp*_tmp88B=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_tmp24E[1]=_tmp88B;});({struct Cyc_Absyn_Exp*_tmp88C=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp88C,_tag_fat(_tmp24E,sizeof(struct Cyc_Absyn_Exp*),2));});});
# 1351
Cyc_Absyn_add_exp(_tmp88D,
# 1354
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}}else{
return 0;};}}}}
# 1359
({struct Cyc_Warn_String_Warn_Warg_struct _tmp250=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A6;_tmp7A6.tag=0,({struct _fat_ptr _tmp88E=({const char*_tmp251="get_varsizeexp: did not find last struct field";_tag_fat(_tmp251,sizeof(char),47U);});_tmp7A6.f1=_tmp88E;});_tmp7A6;});void*_tmp24F[1];_tmp24F[0]=& _tmp250;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp24F,sizeof(void*),1));});}}
# 1362
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
struct Cyc_List_List*_tmp252=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp252;
{int i=1;for(0;fs != 0;(fs=fs->tl,++ i)){
if(Cyc_strcmp((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,(struct _fat_ptr)*f)== 0)
return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp254=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A9;_tmp7A9.tag=0,({struct _fat_ptr _tmp88F=({const char*_tmp258="get_member_offset ";_tag_fat(_tmp258,sizeof(char),19U);});_tmp7A9.f1=_tmp88F;});_tmp7A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp255=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A8;_tmp7A8.tag=0,_tmp7A8.f1=*f;_tmp7A8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp256=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A7;_tmp7A7.tag=0,({struct _fat_ptr _tmp890=({const char*_tmp257=" failed";_tag_fat(_tmp257,sizeof(char),8U);});_tmp7A7.f1=_tmp890;});_tmp7A7;});void*_tmp253[3];_tmp253[0]=& _tmp254,_tmp253[1]=& _tmp255,_tmp253[2]=& _tmp256;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp253,sizeof(void*),3));});}struct _tuple33{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1371
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1374
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1377
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->tag=5,_tmp259->f1=e1,_tmp259->f2=incr;_tmp259;}),0U);}
# 1381
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,void*),void*);
# 1390
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1392
void*_tmp25A=e1->r;void*_stmttmp20=_tmp25A;void*_tmp25B=_stmttmp20;int _tmp25F;int _tmp25E;struct _fat_ptr*_tmp25D;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Exp*_tmp261;void*_tmp260;struct Cyc_Absyn_Stmt*_tmp262;switch(*((int*)_tmp25B)){case 37: _tmp262=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp25B)->f1;{struct Cyc_Absyn_Stmt*s=_tmp262;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14: _tmp260=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25B)->f1;_tmp261=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25B)->f2;{void*t=_tmp260;struct Cyc_Absyn_Exp*e=_tmp261;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21: _tmp25C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25B)->f1;_tmp25D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25B)->f2;_tmp25E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25B)->f3;_tmp25F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25B)->f4;{struct Cyc_Absyn_Exp*e=_tmp25C;struct _fat_ptr*fld=_tmp25D;int is_tagged=_tmp25E;int is_read=_tmp25F;
# 1397
e1->r=e->r;
({struct Cyc_Absyn_Exp*_tmp893=e1;struct Cyc_List_List*_tmp892=({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=fld,_tmp263->tl=fs;_tmp263;});struct Cyc_Absyn_Exp*(*_tmp891)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp893,_tmp892,_tmp891,f_env);});
goto _LL0;}default:  {
# 1405
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1407
for(1;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_tmp894=(f(e1_copy,f_env))->r;e1->r=_tmp894;});
goto _LL0;}}_LL0:;}
# 1413
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1415
void*_tmp264=s->r;void*_stmttmp21=_tmp264;void*_tmp265=_stmttmp21;struct Cyc_Absyn_Stmt*_tmp266;struct Cyc_Absyn_Stmt*_tmp268;struct Cyc_Absyn_Decl*_tmp267;struct Cyc_Absyn_Exp*_tmp269;switch(*((int*)_tmp265)){case 1: _tmp269=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp265)->f1;{struct Cyc_Absyn_Exp*e1=_tmp269;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12: _tmp267=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp265)->f1;_tmp268=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp265)->f2;{struct Cyc_Absyn_Decl*d=_tmp267;struct Cyc_Absyn_Stmt*s2=_tmp268;
# 1418
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2: _tmp266=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp265)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp266;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AB;_tmp7AB.tag=0,({struct _fat_ptr _tmp895=({const char*_tmp26D="lvalue_assign_stmt: ";_tag_fat(_tmp26D,sizeof(char),21U);});_tmp7AB.f1=_tmp895;});_tmp7AB;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp26C=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7AA;_tmp7AA.tag=5,_tmp7AA.f1=s;_tmp7AA;});void*_tmp26A[2];_tmp26A[0]=& _tmp26B,_tmp26A[1]=& _tmp26C;Cyc_Toc_toc_impos(_tag_fat(_tmp26A,sizeof(void*),2));});}_LL0:;}
# 1424
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*);
# 1428
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp26E=e->r;void*_stmttmp22=_tmp26E;void*_tmp26F=_stmttmp22;struct Cyc_Absyn_Stmt*_tmp270;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp**_tmp273;void**_tmp272;switch(*((int*)_tmp26F)){case 14: _tmp272=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26F)->f1;_tmp273=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26F)->f2;{void**t=_tmp272;struct Cyc_Absyn_Exp**e1=_tmp273;
# 1431
({struct Cyc_Absyn_Exp*_tmp896=Cyc_Toc_push_address_exp(*e1);*e1=_tmp896;});
({void*_tmp897=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_tmp897;});
return e;}case 20: _tmp271=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp26F)->f1;{struct Cyc_Absyn_Exp*e1=_tmp271;
# 1435
return e1;}case 37: _tmp270=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26F)->f1;{struct Cyc_Absyn_Stmt*s=_tmp270;
# 1439
Cyc_Toc_push_address_stmt(s);
return e;}default:
# 1442
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp275=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AD;_tmp7AD.tag=0,({struct _fat_ptr _tmp898=({const char*_tmp277="can't take & of exp ";_tag_fat(_tmp277,sizeof(char),21U);});_tmp7AD.f1=_tmp898;});_tmp7AD;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp276=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7AC;_tmp7AC.tag=4,_tmp7AC.f1=e;_tmp7AC;});void*_tmp274[2];_tmp274[0]=& _tmp275,_tmp274[1]=& _tmp276;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp274,sizeof(void*),2));});};}
# 1446
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp278=s->r;void*_stmttmp23=_tmp278;void*_tmp279=_stmttmp23;struct Cyc_Absyn_Exp**_tmp27A;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Absyn_Stmt*_tmp27C;switch(*((int*)_tmp279)){case 2: _tmp27C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp279)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp27C;
_tmp27B=s2;goto _LL4;}case 12: _tmp27B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp279)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp27B;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1: _tmp27A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp279)->f1;{struct Cyc_Absyn_Exp**e=_tmp27A;
({struct Cyc_Absyn_Exp*_tmp899=Cyc_Toc_push_address_exp(*e);*e=_tmp899;});goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7AF;_tmp7AF.tag=0,({struct _fat_ptr _tmp89A=({const char*_tmp280="can't take & of stmt ";_tag_fat(_tmp280,sizeof(char),22U);});_tmp7AF.f1=_tmp89A;});_tmp7AF;});struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_Stmt_Warn_Warg_struct _tmp7AE;_tmp7AE.tag=5,_tmp7AE.f1=s;_tmp7AE;});void*_tmp27D[2];_tmp27D[0]=& _tmp27E,_tmp27D[1]=& _tmp27F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp27D,sizeof(void*),2));});}_LL0:;}
# 1458
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1470
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type,Cyc_Absyn_false_type);
void*_tmp281=Cyc_Toc_typ_to_c(elt_type);void*c_elt_type=_tmp281;
void*_tmp282=Cyc_Toc_typ_to_c(fat_ptr_type);void*c_fat_ptr_type=_tmp282;
void*_tmp283=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);void*c_ptr_type=_tmp283;
struct Cyc_Core_Opt*_tmp284=({struct Cyc_Core_Opt*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->v=c_ptr_type;_tmp2B7;});struct Cyc_Core_Opt*c_ptr_type_opt=_tmp284;
struct Cyc_Absyn_Exp*xinit;
{void*_tmp285=e1->r;void*_stmttmp24=_tmp285;void*_tmp286=_stmttmp24;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp289;switch(*((int*)_tmp286)){case 20: _tmp289=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp286)->f1;{struct Cyc_Absyn_Exp*ea=_tmp289;
# 1478
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1483
Cyc_Toc_exp_to_c(nv,ea);xinit=ea;goto _LL0;}case 23: _tmp287=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp286)->f1;_tmp288=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp286)->f2;{struct Cyc_Absyn_Exp*ea=_tmp287;struct Cyc_Absyn_Exp*eb=_tmp288;
# 1485
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,Cyc_Absyn_Other_coercion,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1490
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_tmp28A[3];_tmp28A[0]=ea,({
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp28A[1]=_tmp89B;}),_tmp28A[2]=eb;({struct Cyc_Absyn_Exp*_tmp89C=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_dl(_tmp89C,_tag_fat(_tmp28A,sizeof(struct Cyc_Absyn_Exp*),3));});});
goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B0;_tmp7B0.tag=0,({struct _fat_ptr _tmp89D=({const char*_tmp28D="found bad lhs for zero-terminated pointer assignment";_tag_fat(_tmp28D,sizeof(char),53U);});_tmp7B0.f1=_tmp89D;});_tmp7B0;});void*_tmp28B[1];_tmp28B[0]=& _tmp28C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp28B,sizeof(void*),1));});}_LL0:;}{
# 1496
struct _tuple1*_tmp28E=Cyc_Toc_temp_var();struct _tuple1*x=_tmp28E;
struct _RegionHandle _tmp28F=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp28F;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp290=({struct Cyc_Absyn_Vardecl*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->sc=Cyc_Absyn_Public,_tmp2B6->name=x,_tmp2B6->varloc=0U,_tmp2B6->tq=Cyc_Toc_mt_tq,_tmp2B6->type=c_fat_ptr_type,_tmp2B6->initializer=xinit,_tmp2B6->rgn=0,_tmp2B6->attributes=0,_tmp2B6->escapes=0,_tmp2B6->is_proto=0;_tmp2B6;});struct Cyc_Absyn_Vardecl*x_vd=_tmp290;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp291=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->tag=4,_tmp2B5->f1=x_vd;_tmp2B5;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd=_tmp291;
struct Cyc_Absyn_Exp*_tmp292=Cyc_Absyn_varb_exp((void*)x_bnd,0U);struct Cyc_Absyn_Exp*x_exp=_tmp292;
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp293=Cyc_Absyn_deref_exp(x_exp,0U);struct Cyc_Absyn_Exp*deref_x=_tmp293;
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*_tmp294=Cyc_Toc_temp_var();struct _tuple1*y=_tmp294;
struct Cyc_Absyn_Vardecl*_tmp295=({struct Cyc_Absyn_Vardecl*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4->sc=Cyc_Absyn_Public,_tmp2B4->name=y,_tmp2B4->varloc=0U,_tmp2B4->tq=Cyc_Toc_mt_tq,_tmp2B4->type=c_elt_type,_tmp2B4->initializer=deref_x,_tmp2B4->rgn=0,_tmp2B4->attributes=0,_tmp2B4->escapes=0,_tmp2B4->is_proto=0;_tmp2B4;});struct Cyc_Absyn_Vardecl*y_vd=_tmp295;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp296=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->tag=4,_tmp2B3->f1=y_vd;_tmp2B3;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd=_tmp296;
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp297=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y_exp=_tmp297;
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _tmp89F=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_tmp89E=y_exp;Cyc_Absyn_prim2_exp(_tmp89F,_tmp89E,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1517
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp298=Cyc_Toc_temp_var();struct _tuple1*z=_tmp298;
struct Cyc_Absyn_Vardecl*_tmp299=({struct Cyc_Absyn_Vardecl*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->sc=Cyc_Absyn_Public,_tmp2B2->name=z,_tmp2B2->varloc=0U,_tmp2B2->tq=Cyc_Toc_mt_tq,_tmp2B2->type=c_elt_type,_tmp2B2->initializer=z_init,_tmp2B2->rgn=0,_tmp2B2->attributes=0,_tmp2B2->escapes=0,_tmp2B2->is_proto=0;_tmp2B2;});struct Cyc_Absyn_Vardecl*z_vd=_tmp299;
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp29A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=4,_tmp2B1->f1=z_vd;_tmp2B1;});struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd=_tmp29A;
# 1523
struct Cyc_Absyn_Exp*_tmp29B=Cyc_Absyn_varb_exp((void*)y_bnd,0U);struct Cyc_Absyn_Exp*y2_exp=_tmp29B;y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero1_exp=_tmp29C;
struct Cyc_Absyn_Exp*_tmp29D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,y2_exp,zero1_exp,0U);struct Cyc_Absyn_Exp*comp1_exp=_tmp29D;
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1530
struct Cyc_Absyn_Exp*_tmp29E=Cyc_Absyn_varb_exp((void*)z_bnd,0U);struct Cyc_Absyn_Exp*z_exp=_tmp29E;z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*_tmp29F=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero2_exp=_tmp29F;
struct Cyc_Absyn_Exp*_tmp2A0=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,z_exp,zero2_exp,0U);struct Cyc_Absyn_Exp*comp2_exp=_tmp2A0;
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1537
struct Cyc_List_List*_tmp2A1=({struct Cyc_Absyn_Exp*_tmp2B0[2];({struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_tmp2B0[0]=_tmp8A1;}),({
struct Cyc_Absyn_Exp*_tmp8A0=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2B0[1]=_tmp8A0;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2B0,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1537
struct Cyc_List_List*xsizeargs=_tmp2A1;
# 1539
struct Cyc_Absyn_Exp*_tmp2A2=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*oneexp=_tmp2A2;
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp8A2,oneexp,0U);});{
# 1544
struct Cyc_Absyn_Exp*_tmp2A3=({struct Cyc_Absyn_Exp*_tmp8A3=xsize;Cyc_Absyn_and_exp(_tmp8A3,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});struct Cyc_Absyn_Exp*comp_exp=_tmp2A3;
struct Cyc_Absyn_Stmt*_tmp2A4=Cyc_Absyn_exp_stmt(({void*_tmp2AF=0U;({struct Cyc_Absyn_Exp*_tmp8A4=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp8A4,_tag_fat(_tmp2AF,sizeof(struct Cyc_Absyn_Exp*),0));});}),0U);struct Cyc_Absyn_Stmt*thr_stmt=_tmp2A4;
struct Cyc_Absyn_Exp*_tmp2A5=({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_tmp8A5,Cyc_Toc_curr_sp,0U);});struct Cyc_Absyn_Exp*xcurr=_tmp2A5;
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*_tmp2A6=Cyc_Absyn_deref_exp(xcurr,0U);struct Cyc_Absyn_Exp*deref_xcurr=_tmp2A6;
struct Cyc_Absyn_Exp*_tmp2A7=({struct Cyc_Absyn_Exp*_tmp8A6=deref_xcurr;Cyc_Absyn_assign_exp(_tmp8A6,Cyc_Absyn_var_exp(z,0U),0U);});struct Cyc_Absyn_Exp*asn_exp=_tmp2A7;
struct Cyc_Absyn_Stmt*_tmp2A8=Cyc_Absyn_exp_stmt(asn_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp2A8;
s=({struct Cyc_Absyn_Stmt*_tmp8A9=({struct Cyc_Absyn_Exp*_tmp8A8=comp_exp;struct Cyc_Absyn_Stmt*_tmp8A7=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_tmp8A8,_tmp8A7,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp8A9,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8AB=({struct Cyc_Absyn_Decl*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));({void*_tmp8AA=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->tag=0,_tmp2A9->f1=z_vd;_tmp2A9;});_tmp2AA->r=_tmp8AA;}),_tmp2AA->loc=0U;_tmp2AA;});Cyc_Absyn_decl_stmt(_tmp8AB,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8AD=({struct Cyc_Absyn_Decl*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));({void*_tmp8AC=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=0,_tmp2AB->f1=y_vd;_tmp2AB;});_tmp2AC->r=_tmp8AC;}),_tmp2AC->loc=0U;_tmp2AC;});Cyc_Absyn_decl_stmt(_tmp8AD,s,0U);});
s=({struct Cyc_Absyn_Decl*_tmp8AF=({struct Cyc_Absyn_Decl*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));({void*_tmp8AE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->tag=0,_tmp2AD->f1=x_vd;_tmp2AD;});_tmp2AE->r=_tmp8AE;}),_tmp2AE->loc=0U;_tmp2AE;});Cyc_Absyn_decl_stmt(_tmp8AF,s,0U);});
({void*_tmp8B0=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8B0;});}}}}}}}}}}
# 1498
;_pop_region();}}
# 1570 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1574
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2B8=Cyc_Absyn_compress(aggrtype);void*_stmttmp25=_tmp2B8;void*_tmp2B9=_stmttmp25;union Cyc_Absyn_AggrInfo _tmp2BA;if(*((int*)_tmp2B9)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B9)->f1)== 20){_tmp2BA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B9)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp2BA;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B3;_tmp7B3.tag=0,({struct _fat_ptr _tmp8B1=({const char*_tmp2C0="expecting union but found ";_tag_fat(_tmp2C0,sizeof(char),27U);});_tmp7B3.f1=_tmp8B1;});_tmp7B3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B2;_tmp7B2.tag=2,_tmp7B2.f1=(void*)aggrtype;_tmp7B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B1;_tmp7B1.tag=0,({
struct _fat_ptr _tmp8B2=({const char*_tmp2BF=" in check_tagged_union";_tag_fat(_tmp2BF,sizeof(char),23U);});_tmp7B1.f1=_tmp8B2;});_tmp7B1;});void*_tmp2BB[3];_tmp2BB[0]=& _tmp2BC,_tmp2BB[1]=& _tmp2BD,_tmp2BB[2]=& _tmp2BE;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp2BB,sizeof(void*),3));});}_LL0:;}{
# 1580
struct _tuple30 _tmp2C1=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp26=_tmp2C1;struct _tuple30 _tmp2C2=_stmttmp26;struct Cyc_Absyn_Exp*_tmp2C4;struct _tuple1*_tmp2C3;_tmp2C3=_tmp2C2.f1;_tmp2C4=_tmp2C2.f2;{struct _tuple1*temp=_tmp2C3;struct Cyc_Absyn_Exp*temp_exp=_tmp2C4;
struct Cyc_Absyn_Exp*_tmp2C5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp2C5;
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2C6=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*temp_f_tag=_tmp2C6;
struct Cyc_Absyn_Exp*_tmp2C7=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2C7;
struct Cyc_Absyn_Exp*_tmp2C8=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);struct Cyc_Absyn_Exp*temp_f_val=_tmp2C8;
struct Cyc_Absyn_Stmt*_tmp2C9=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);struct Cyc_Absyn_Stmt*sres=_tmp2C9;
struct Cyc_Absyn_Stmt*_tmp2CA=({struct Cyc_Absyn_Exp*_tmp8B4=test_exp;struct Cyc_Absyn_Stmt*_tmp8B3=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8B4,_tmp8B3,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2CA;
void*_tmp2CB=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);void*e1_p_type=_tmp2CB;
struct Cyc_Absyn_Exp*_tmp2CC=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);struct Cyc_Absyn_Exp*e1_f=_tmp2CC;
struct Cyc_Absyn_Stmt*_tmp2CD=({struct _tuple1*_tmp8B7=temp;void*_tmp8B6=e1_p_type;struct Cyc_Absyn_Exp*_tmp8B5=e1_f;Cyc_Absyn_declare_stmt(_tmp8B7,_tmp8B6,_tmp8B5,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2CD;
return Cyc_Toc_stmt_exp_r(s);}else{
# 1593
struct Cyc_Absyn_Exp*_tmp2CE=({struct Cyc_Absyn_Exp*_tmp8B8=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8B8,Cyc_Toc_tag_sp,0U);});struct Cyc_Absyn_Exp*temp_f_tag=_tmp2CE;
struct Cyc_Absyn_Exp*_tmp2CF=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2CF;
struct Cyc_Absyn_Exp*_tmp2D0=({struct Cyc_Absyn_Exp*_tmp8B9=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_tmp8B9,Cyc_Toc_val_sp,0U);});struct Cyc_Absyn_Exp*temp_f_val=_tmp2D0;
struct Cyc_Absyn_Stmt*_tmp2D1=Cyc_Absyn_exp_stmt(temp_f_val,0U);struct Cyc_Absyn_Stmt*sres=_tmp2D1;
struct Cyc_Absyn_Stmt*_tmp2D2=({struct Cyc_Absyn_Exp*_tmp8BB=test_exp;struct Cyc_Absyn_Stmt*_tmp8BA=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8BB,_tmp8BA,Cyc_Toc_skip_stmt_dl(),0U);});struct Cyc_Absyn_Stmt*ifs=_tmp2D2;
struct Cyc_Absyn_Stmt*_tmp2D3=({struct _tuple1*_tmp8BE=temp;void*_tmp8BD=e1_c_type;struct Cyc_Absyn_Exp*_tmp8BC=e1;Cyc_Absyn_declare_stmt(_tmp8BE,_tmp8BD,_tmp8BC,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});struct Cyc_Absyn_Stmt*s=_tmp2D3;
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1603
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1606
void*_tmp2D4=Cyc_Absyn_compress(t);void*_stmttmp27=_tmp2D4;void*_tmp2D5=_stmttmp27;union Cyc_Absyn_AggrInfo _tmp2D6;if(*((int*)_tmp2D5)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5)->f1)== 20){_tmp2D6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp2D6;
# 1608
struct Cyc_Absyn_Aggrdecl*_tmp2D7=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp2D7;
({int _tmp8BF=Cyc_Toc_get_member_offset(ad,f);*f_tag=_tmp8BF;});{
# 1611
struct _fat_ptr str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DB=({struct Cyc_String_pa_PrintArg_struct _tmp7B5;_tmp7B5.tag=0,_tmp7B5.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7B5;});struct Cyc_String_pa_PrintArg_struct _tmp2DC=({struct Cyc_String_pa_PrintArg_struct _tmp7B4;_tmp7B4.tag=0,_tmp7B4.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7B4;});void*_tmp2D9[2];_tmp2D9[0]=& _tmp2DB,_tmp2D9[1]=& _tmp2DC;({struct _fat_ptr _tmp8C0=({const char*_tmp2DA="_union_%s_%s";_tag_fat(_tmp2DA,sizeof(char),13U);});Cyc_aprintf(_tmp8C0,_tag_fat(_tmp2D9,sizeof(void*),2));});});
({void*_tmp8C1=Cyc_Absyn_strct(({struct _fat_ptr*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));*_tmp2D8=str;_tmp2D8;}));*tagged_member_type=_tmp8C1;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;}}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1622
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1625
void*_tmp2DD=e->r;void*_stmttmp28=_tmp2DD;void*_tmp2DE=_stmttmp28;int*_tmp2E1;struct _fat_ptr*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;int*_tmp2E4;struct _fat_ptr*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E5;switch(*((int*)_tmp2DE)){case 14: _tmp2E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2DE)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2E5;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B6;_tmp7B6.tag=0,({struct _fat_ptr _tmp8C2=({const char*_tmp2E8="cast on lhs!";_tag_fat(_tmp2E8,sizeof(char),13U);});_tmp7B6.f1=_tmp8C2;});_tmp7B6;});void*_tmp2E6[1];_tmp2E6[0]=& _tmp2E7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp2E6,sizeof(void*),1));});}case 21: _tmp2E2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DE)->f1;_tmp2E3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DE)->f2;_tmp2E4=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2DE)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2E2;struct _fat_ptr*f=_tmp2E3;int*is_read=_tmp2E4;
# 1628
return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22: _tmp2DF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DE)->f1;_tmp2E0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DE)->f2;_tmp2E1=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2DE)->f4;{struct Cyc_Absyn_Exp*e1=_tmp2DF;struct _fat_ptr*f=_tmp2E0;int*is_read=_tmp2E1;
# 1631
void*_tmp2E9=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp29=_tmp2E9;void*_tmp2EA=_stmttmp29;void*_tmp2EB;if(*((int*)_tmp2EA)== 3){_tmp2EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EA)->f1).elt_type;{void*et=_tmp2EB;
# 1633
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{
# 1635
return 0;};}default:
# 1637
 return 0;};}
# 1650 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1654
struct _tuple30 _tmp2EC=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp2A=_tmp2EC;struct _tuple30 _tmp2ED=_stmttmp2A;struct Cyc_Absyn_Exp*_tmp2EF;struct _tuple1*_tmp2EE;_tmp2EE=_tmp2ED.f1;_tmp2EF=_tmp2ED.f2;{struct _tuple1*temp=_tmp2EE;struct Cyc_Absyn_Exp*temp_exp=_tmp2EF;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1663
struct Cyc_Absyn_Exp*_tmp2F0=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);struct Cyc_Absyn_Exp*test_exp=_tmp2F0;
s2=({struct Cyc_Absyn_Exp*_tmp8C4=test_exp;struct Cyc_Absyn_Stmt*_tmp8C3=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp8C4,_tmp8C3,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1666
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8C7=temp;void*_tmp8C6=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8C5=
Cyc_Toc_push_address_exp(e1);
# 1666
Cyc_Absyn_declare_stmt(_tmp8C7,_tmp8C6,_tmp8C5,
# 1668
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1673
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B8;_tmp7B8.tag=0,({struct _fat_ptr _tmp8C8=({const char*_tmp2F4="exp_to_c: no type for ";_tag_fat(_tmp2F4,sizeof(char),23U);});_tmp7B8.f1=_tmp8C8;});_tmp7B8;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7B7;_tmp7B7.tag=4,_tmp7B7.f1=e;_tmp7B7;});void*_tmp2F1[2];_tmp2F1[0]=& _tmp2F2,_tmp2F1[1]=& _tmp2F3;({unsigned _tmp8C9=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8C9,_tag_fat(_tmp2F1,sizeof(void*),2));});});{
void*old_typ=(void*)_check_null(e->topt);
# 1678
int did_inserted_checks=0;
{void*_tmp2F5=e->annot;void*_stmttmp2B=_tmp2F5;void*_tmp2F6=_stmttmp2B;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F6)->tag == Cyc_Absyn_EmptyAnnot)
goto _LL4;else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp2F6)->tag == Cyc_InsertChecks_NoCheck){_LL4:
 did_inserted_checks=1;goto _LL0;}else{
goto _LL0;}}_LL0:;}
# 1684
{void*_tmp2F7=e->r;void*_stmttmp2C=_tmp2F7;void*_tmp2F8=_stmttmp2C;struct Cyc_List_List*_tmp2FA;struct Cyc_List_List*_tmp2F9;struct Cyc_Absyn_Stmt*_tmp2FB;struct _fat_ptr*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Exp*_tmp2FE;int _tmp305;int _tmp304;struct Cyc_Absyn_Exp*_tmp303;void**_tmp302;struct Cyc_Absyn_Exp*_tmp301;int _tmp300;struct Cyc_Absyn_Datatypefield*_tmp308;struct Cyc_Absyn_Datatypedecl*_tmp307;struct Cyc_List_List*_tmp306;struct Cyc_Absyn_Aggrdecl*_tmp30C;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30A;struct _tuple1**_tmp309;struct Cyc_List_List*_tmp30E;void*_tmp30D;int _tmp311;void*_tmp310;struct Cyc_Absyn_Exp*_tmp30F;int _tmp315;struct Cyc_Absyn_Exp*_tmp314;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_Absyn_Vardecl*_tmp312;struct Cyc_List_List*_tmp316;struct Cyc_List_List*_tmp317;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp31A;int _tmp31E;int _tmp31D;struct _fat_ptr*_tmp31C;struct Cyc_Absyn_Exp*_tmp31B;int _tmp322;int _tmp321;struct _fat_ptr*_tmp320;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_List_List*_tmp324;void*_tmp323;void*_tmp325;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_Absyn_Exp*_tmp328;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Exp*_tmp329;enum Cyc_Absyn_Coercion _tmp32D;int _tmp32C;struct Cyc_Absyn_Exp*_tmp32B;void**_tmp32A;struct Cyc_List_List*_tmp32F;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp330;int _tmp332;struct Cyc_Absyn_Exp*_tmp331;struct Cyc_Absyn_VarargInfo*_tmp337;struct Cyc_List_List*_tmp336;int _tmp335;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_List_List*_tmp339;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_Absyn_Exp*_tmp33B;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Core_Opt*_tmp344;struct Cyc_Absyn_Exp*_tmp343;enum Cyc_Absyn_Incrementor _tmp347;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_List_List*_tmp349;enum Cyc_Absyn_Primop _tmp348;struct Cyc_Absyn_Exp*_tmp34A;switch(*((int*)_tmp2F8)){case 2:
# 1686
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).Null_c).tag == 1){
# 1692
struct Cyc_Absyn_Exp*_tmp34B=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*zero=_tmp34B;
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8CA=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero))->r;e->r=_tmp8CA;});else{
# 1697
({void*_tmp8CC=({struct Cyc_Absyn_Exp*_tmp34C[3];_tmp34C[0]=zero,_tmp34C[1]=zero,_tmp34C[2]=zero;({struct Cyc_Absyn_Exp*_tmp8CB=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp8CB,_tag_fat(_tmp34C,sizeof(struct Cyc_Absyn_Exp*),3));});});e->r=_tmp8CC;});}}else{
# 1699
e->r=(void*)& Cyc_Toc_zero_exp;}
goto _LL7;}else{
goto _LL7;}case 1:
 goto _LL7;case 42:
({void*_tmp8CD=(Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U))->r;e->r=_tmp8CD;});goto _LL7;case 41: _tmp34A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_Absyn_Exp*e1=_tmp34A;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3: _tmp348=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp349=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{enum Cyc_Absyn_Primop p=_tmp348;struct Cyc_List_List*es=_tmp349;
# 1707
struct Cyc_List_List*_tmp34D=((struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Toc_get_cyc_type,es);struct Cyc_List_List*old_types=_tmp34D;
# 1709
((void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
{enum Cyc_Absyn_Primop _tmp34E=p;switch(_tmp34E){case Cyc_Absyn_Numelts:  {
# 1712
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
{void*_tmp34F=Cyc_Absyn_compress((void*)_check_null(arg->topt));void*_stmttmp2D=_tmp34F;void*_tmp350=_stmttmp2D;void*_tmp354;void*_tmp353;void*_tmp352;void*_tmp351;if(*((int*)_tmp350)== 3){_tmp351=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp350)->f1).elt_type;_tmp352=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp350)->f1).ptr_atts).nullable;_tmp353=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp350)->f1).ptr_atts).bounds;_tmp354=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp350)->f1).ptr_atts).zero_term;{void*elt_type=_tmp351;void*nbl=_tmp352;void*bound=_tmp353;void*zt=_tmp354;
# 1715
struct Cyc_Absyn_Exp*_tmp355=({void*_tmp8CE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8CE,bound);});struct Cyc_Absyn_Exp*eopt=_tmp355;
if(eopt == 0)
# 1718
({void*_tmp8D1=({struct Cyc_Absyn_Exp*_tmp356[2];_tmp356[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_tmp8CF=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp356[1]=_tmp8CF;});({struct Cyc_Absyn_Exp*_tmp8D0=Cyc_Toc__get_fat_size_e;Cyc_Toc_fncall_exp_r(_tmp8D0,_tag_fat(_tmp356,sizeof(struct Cyc_Absyn_Exp*),2));});});
# 1718
e->r=_tmp8D1;});else{
# 1722
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)es->hd);
# 1726
({void*_tmp8D3=({struct Cyc_Absyn_Exp*_tmp357[2];_tmp357[0]=(struct Cyc_Absyn_Exp*)es->hd,_tmp357[1]=eopt;({struct Cyc_Absyn_Exp*_tmp8D2=function_e;Cyc_Toc_fncall_exp_r(_tmp8D2,_tag_fat(_tmp357,sizeof(struct Cyc_Absyn_Exp*),2));});});e->r=_tmp8D3;});}else{
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(eopt))
({void*_tmp358=0U;({unsigned _tmp8D5=e->loc;struct _fat_ptr _tmp8D4=({const char*_tmp359="can't calculate numelts";_tag_fat(_tmp359,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8D5,_tmp8D4,_tag_fat(_tmp358,sizeof(void*),0));});});
# 1731
({void*_tmp8D8=({struct Cyc_Absyn_Exp*_tmp8D7=arg;struct Cyc_Absyn_Exp*_tmp8D6=eopt;Cyc_Toc_conditional_exp_r(_tmp8D7,_tmp8D6,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8D8;});}else{
# 1733
e->r=eopt->r;}}}
# 1735
goto _LL77;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp35B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BA;_tmp7BA.tag=0,({struct _fat_ptr _tmp8D9=({const char*_tmp35D="numelts primop applied to non-pointer ";_tag_fat(_tmp35D,sizeof(char),39U);});_tmp7BA.f1=_tmp8D9;});_tmp7BA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp35C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B9;_tmp7B9.tag=2,_tmp7B9.f1=(void*)_check_null(arg->topt);_tmp7B9;});void*_tmp35A[2];_tmp35A[0]=& _tmp35B,_tmp35A[1]=& _tmp35C;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp35A,sizeof(void*),2));});}_LL77:;}
# 1739
goto _LL62;}case Cyc_Absyn_Plus:
# 1744
 if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr))Cyc_Toc_unimp)(({const char*_tmp35E="can't do pointer arithmetic at top-level";_tag_fat(_tmp35E,sizeof(char),41U);}));
{void*_tmp35F=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(old_types))->hd);void*_stmttmp2E=_tmp35F;void*_tmp360=_stmttmp2E;void*_tmp363;void*_tmp362;void*_tmp361;if(*((int*)_tmp360)== 3){_tmp361=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360)->f1).elt_type;_tmp362=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360)->f1).ptr_atts).bounds;_tmp363=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp360)->f1).ptr_atts).zero_term;{void*elt_type=_tmp361;void*b=_tmp362;void*zt=_tmp363;
# 1748
struct Cyc_Absyn_Exp*_tmp364=({void*_tmp8DA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8DA,b);});struct Cyc_Absyn_Exp*eopt=_tmp364;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(eopt == 0)
({void*_tmp8DD=({struct Cyc_Absyn_Exp*_tmp365[3];_tmp365[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp365[1]=_tmp8DB;}),_tmp365[2]=e2;({struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8DC,_tag_fat(_tmp365,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 1752
e->r=_tmp8DD;});else{
# 1754
if(Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp8DF=({struct Cyc_Absyn_Exp*_tmp366[3];_tmp366[0]=e1,_tmp366[1]=eopt,_tmp366[2]=e2;({struct Cyc_Absyn_Exp*_tmp8DE=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp8DE,_tag_fat(_tmp366,sizeof(struct Cyc_Absyn_Exp*),3));});});e->r=_tmp8DF;});}
goto _LL7C;}}else{
goto _LL7C;}_LL7C:;}
# 1759
goto _LL62;case Cyc_Absyn_Minus:  {
# 1764
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(old_types))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd)){
({void*_tmp8E1=({struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8E0,Cyc_Toc_curr_sp);});e1->r=_tmp8E1;});
({void*_tmp8E3=({struct Cyc_Absyn_Exp*_tmp8E2=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8E2,Cyc_Toc_curr_sp);});e2->r=_tmp8E3;});
({void*_tmp8E5=({void*_tmp8E4=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp8E4;});e1->topt=_tmp8E5;});
({void*_tmp8E7=({struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8E6,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1772
e->r=_tmp8E7;});}else{
# 1775
({void*_tmp8EB=({struct Cyc_Absyn_Exp*_tmp367[3];_tmp367[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8E9=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp367[1]=_tmp8E9;}),({
struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp367[2]=_tmp8E8;});({struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Toc__fat_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8EA,_tag_fat(_tmp367,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 1775
e->r=_tmp8EB;});}}
# 1779
goto _LL62;}case Cyc_Absyn_Eq:
 goto _LL6C;case Cyc_Absyn_Neq: _LL6C: goto _LL6E;case Cyc_Absyn_Gt: _LL6E: goto _LL70;case Cyc_Absyn_Gte: _LL70: goto _LL72;case Cyc_Absyn_Lt: _LL72: goto _LL74;case Cyc_Absyn_Lte: _LL74: {
# 1782
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(old_types))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(old_types->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*_tmp368=({void*_tmp8EC=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8EC,Cyc_Toc_mt_tq);});void*t=_tmp368;
({void*_tmp8EF=({void*_tmp8EE=t;Cyc_Toc_cast_it_r(_tmp8EE,({struct Cyc_Absyn_Exp*_tmp8ED=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8ED,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8EF;});
e1->topt=t;}
# 1792
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*_tmp369=({void*_tmp8F0=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp8F0,Cyc_Toc_mt_tq);});void*t=_tmp369;
({void*_tmp8F3=({void*_tmp8F2=t;Cyc_Toc_cast_it_r(_tmp8F2,({struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8F1,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8F3;});
e2->topt=t;}
# 1797
goto _LL62;}default:
 goto _LL62;}_LL62:;}
# 1800
goto _LL7;}case 5: _tmp346=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp347=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e2=_tmp346;enum Cyc_Absyn_Incrementor incr=_tmp347;
# 1802
void*e2_cyc_typ=(void*)_check_null(e2->topt);
# 1811 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=({const char*_tmp386="increment";_tag_fat(_tmp386,sizeof(char),10U);});
if((int)incr == (int)2U ||(int)incr == (int)3U)incr_str=({const char*_tmp36A="decrement";_tag_fat(_tmp36A,sizeof(char),10U);});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BE;_tmp7BE.tag=0,({struct _fat_ptr _tmp8F4=({const char*_tmp372="in-place ";_tag_fat(_tmp372,sizeof(char),10U);});_tmp7BE.f1=_tmp8F4;});_tmp7BE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BD;_tmp7BD.tag=0,_tmp7BD.f1=incr_str;_tmp7BD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BC;_tmp7BC.tag=0,({struct _fat_ptr _tmp8F5=({const char*_tmp371=" is not supported when ";_tag_fat(_tmp371,sizeof(char),24U);});_tmp7BC.f1=_tmp8F5;});_tmp7BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp36F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BB;_tmp7BB.tag=0,({
struct _fat_ptr _tmp8F6=({const char*_tmp370="dereferencing a zero-terminated pointer";_tag_fat(_tmp370,sizeof(char),40U);});_tmp7BB.f1=_tmp8F6;});_tmp7BB;});void*_tmp36B[4];_tmp36B[0]=& _tmp36C,_tmp36B[1]=& _tmp36D,_tmp36B[2]=& _tmp36E,_tmp36B[3]=& _tmp36F;({unsigned _tmp8F7=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8F7,_tag_fat(_tmp36B,sizeof(void*),4));});});
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp373=incr;switch(_tmp373){case Cyc_Absyn_PreInc:
 op=0U;goto _LL81;case Cyc_Absyn_PreDec:
 op=2U;goto _LL81;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp375=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C1;_tmp7C1.tag=0,({struct _fat_ptr _tmp8F8=({const char*_tmp379="in-place post";_tag_fat(_tmp379,sizeof(char),14U);});_tmp7C1.f1=_tmp8F8;});_tmp7C1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp376=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C0;_tmp7C0.tag=0,_tmp7C0.f1=incr_str;_tmp7C0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp377=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7BF;_tmp7BF.tag=0,({
struct _fat_ptr _tmp8F9=({const char*_tmp378=" is not supported on @tagged union members";_tag_fat(_tmp378,sizeof(char),43U);});_tmp7BF.f1=_tmp8F9;});_tmp7BF;});void*_tmp374[3];_tmp374[0]=& _tmp375,_tmp374[1]=& _tmp376,_tmp374[2]=& _tmp377;({unsigned _tmp8FA=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp8FA,_tag_fat(_tmp374,sizeof(void*),3));});});}_LL81:;}
# 1829
({void*_tmp8FC=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->tag=4,_tmp37B->f1=e2,({struct Cyc_Core_Opt*_tmp8FB=({struct Cyc_Core_Opt*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->v=(void*)op;_tmp37A;});_tmp37B->f2=_tmp8FB;}),_tmp37B->f3=one;_tmp37B;});e->r=_tmp8FC;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1833
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1838
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
int change=1;
struct Cyc_Absyn_Exp*_tmp37C=((int)incr == (int)1U ||(int)incr == (int)3U)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp37C;
# 1843
if((int)incr == (int)2U ||(int)incr == (int)3U)
change=-1;
({void*_tmp901=({struct Cyc_Absyn_Exp*_tmp37D[3];({struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Toc_push_address_exp(e2);_tmp37D[0]=_tmp8FF;}),({
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp37D[1]=_tmp8FE;}),({
struct Cyc_Absyn_Exp*_tmp8FD=Cyc_Absyn_signed_int_exp(change,0U);_tmp37D[2]=_tmp8FD;});({struct Cyc_Absyn_Exp*_tmp900=fn_e;Cyc_Toc_fncall_exp_r(_tmp900,_tag_fat(_tmp37D,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 1845
e->r=_tmp901;});}else{
# 1848
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1854
did_inserted_checks=1;
if((int)incr != (int)1U){
struct _tuple1*_tmp37E=Cyc_Toc_temp_var();struct _tuple1*x=_tmp37E;
void*_tmp37F=({void*_tmp902=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp902,Cyc_Toc_mt_tq);});void*t=_tmp37F;
struct Cyc_Absyn_Exp*_tmp380=Cyc_Toc_push_address_exp(e2);struct Cyc_Absyn_Exp*xexp=_tmp380;
struct Cyc_Absyn_Exp*_tmp381=({struct Cyc_Absyn_Exp*_tmp903=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp903,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1859
struct Cyc_Absyn_Exp*testexp=_tmp381;
# 1861
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_tmp905=({struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_tmp904,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 1862
Cyc_Absyn_and_exp(_tmp905,testexp,0U);});{
# 1864
struct Cyc_Absyn_Stmt*_tmp382=({struct Cyc_Absyn_Exp*_tmp908=testexp;struct Cyc_Absyn_Stmt*_tmp907=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),Cyc_Absyn_PreInc,0U),0U);
# 1864
Cyc_Absyn_ifthenelse_stmt(_tmp908,_tmp907,
# 1866
Cyc_Absyn_exp_stmt(({void*_tmp383=0U;({struct Cyc_Absyn_Exp*_tmp906=Cyc_Toc__throw_arraybounds_e;Cyc_Toc_fncall_exp_dl(_tmp906,_tag_fat(_tmp383,sizeof(struct Cyc_Absyn_Exp*),0));});}),0U),0U);});
# 1864
struct Cyc_Absyn_Stmt*s=_tmp382;
# 1867
s=({struct Cyc_Absyn_Stmt*_tmp909=s;Cyc_Absyn_seq_stmt(_tmp909,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_tmp90A=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U))->r;e->r=_tmp90A;});}}else{
# 1871
struct Cyc_Toc_functionSet*_tmp384=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;struct Cyc_Toc_functionSet*fnSet=_tmp384;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(fnSet,e2);
({void*_tmp90E=({struct Cyc_Absyn_Exp*_tmp385[2];({struct Cyc_Absyn_Exp*_tmp90C=Cyc_Toc_push_address_exp(e2);_tmp385[0]=_tmp90C;}),({struct Cyc_Absyn_Exp*_tmp90B=Cyc_Absyn_signed_int_exp(1,0U);_tmp385[1]=_tmp90B;});({struct Cyc_Absyn_Exp*_tmp90D=fn_e;Cyc_Toc_fncall_exp_r(_tmp90D,_tag_fat(_tmp385,sizeof(struct Cyc_Absyn_Exp*),2));});});e->r=_tmp90E;});}}else{
# 1875
if(elt_typ == Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
((void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor))Cyc_Toc_lvalue_assign)(e2,0,Cyc_Toc_incr_lvalue,incr);
e->r=e2->r;}}}
# 1879
goto _LL7;}}case 4: _tmp343=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp344=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp345=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;{struct Cyc_Absyn_Exp*e1=_tmp343;struct Cyc_Core_Opt*popt=_tmp344;struct Cyc_Absyn_Exp*e2=_tmp345;
# 1898 "toc.cyc"
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_tmp90F=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp90F;});
# 1909
goto _LL7;}{
# 1911
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
# 1917
return;}{
# 1921
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1927
int done=0;
if(popt != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp387=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp2F=_tmp387;enum Cyc_Absyn_Primop _tmp388=_stmttmp2F;switch(_tmp388){case Cyc_Absyn_Plus:
 change=e2;goto _LL88;case Cyc_Absyn_Minus:
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);goto _LL88;default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C2;_tmp7C2.tag=0,({struct _fat_ptr _tmp910=({const char*_tmp38B="bad t ? pointer arithmetic";_tag_fat(_tmp38B,sizeof(char),27U);});_tmp7C2.f1=_tmp910;});_tmp7C2;});void*_tmp389[1];_tmp389[0]=& _tmp38A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp389,sizeof(void*),1));});}_LL88:;}
# 1937
done=1;{
# 1939
struct Cyc_Absyn_Exp*_tmp38C=Cyc_Toc__fat_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*fn_e=_tmp38C;
({void*_tmp914=({struct Cyc_Absyn_Exp*_tmp38D[3];({struct Cyc_Absyn_Exp*_tmp912=Cyc_Toc_push_address_exp(e1);_tmp38D[0]=_tmp912;}),({
struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp38D[1]=_tmp911;}),_tmp38D[2]=change;({struct Cyc_Absyn_Exp*_tmp913=fn_e;Cyc_Toc_fncall_exp_r(_tmp913,_tag_fat(_tmp38D,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 1940
e->r=_tmp914;});}}else{
# 1943
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1946
enum Cyc_Absyn_Primop _tmp38E=(enum Cyc_Absyn_Primop)popt->v;enum Cyc_Absyn_Primop _stmttmp30=_tmp38E;enum Cyc_Absyn_Primop _tmp38F=_stmttmp30;if(_tmp38F == Cyc_Absyn_Plus){
# 1948
done=1;
({void*_tmp916=({struct Cyc_Absyn_Exp*_tmp390[2];_tmp390[0]=e1,_tmp390[1]=e2;({struct Cyc_Absyn_Exp*_tmp915=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,e1);Cyc_Toc_fncall_exp_r(_tmp915,_tag_fat(_tmp390,sizeof(struct Cyc_Absyn_Exp*),2));});});e->r=_tmp916;});
goto _LL8F;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp392=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C3;_tmp7C3.tag=0,({struct _fat_ptr _tmp917=({const char*_tmp393="bad zero-terminated pointer arithmetic";_tag_fat(_tmp393,sizeof(char),39U);});_tmp7C3.f1=_tmp917;});_tmp7C3;});void*_tmp391[1];_tmp391[0]=& _tmp392;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp391,sizeof(void*),1));});}_LL8F:;}}}
# 1955
if(!done){
# 1957
if(e1_poly)
({void*_tmp919=({void*_tmp918=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp918,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_tmp919;});
# 1963
if(!Cyc_Absyn_is_lvalue(e1)){
({struct Cyc_Absyn_Exp*_tmp91A=e1;((void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*))Cyc_Toc_lvalue_assign)(_tmp91A,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->f1=popt,_tmp394->f2=e2;_tmp394;}));});
e->r=e1->r;}}
# 1968
goto _LL7;}}}}case 6: _tmp340=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp341=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp342=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;{struct Cyc_Absyn_Exp*e1=_tmp340;struct Cyc_Absyn_Exp*e2=_tmp341;struct Cyc_Absyn_Exp*e3=_tmp342;
# 1970
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7: _tmp33E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp33F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp33E;struct Cyc_Absyn_Exp*e2=_tmp33F;
_tmp33C=e1;_tmp33D=e2;goto _LL1F;}case 8: _tmp33C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp33D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL1F: {struct Cyc_Absyn_Exp*e1=_tmp33C;struct Cyc_Absyn_Exp*e2=_tmp33D;
_tmp33A=e1;_tmp33B=e2;goto _LL21;}case 9: _tmp33A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp33B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_tmp33A;struct Cyc_Absyn_Exp*e2=_tmp33B;
# 1977
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3 == 0){_tmp338=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp339=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp338;struct Cyc_List_List*es=_tmp339;
# 1981
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 1986
({void*_tmp91E=({void*_tmp91D=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp91D,({struct Cyc_Absyn_Exp*_tmp395[1];({struct Cyc_Absyn_Exp*_tmp91B=
Cyc_Absyn_copy_exp(e1);_tmp395[0]=_tmp91B;});({struct Cyc_Absyn_Exp*_tmp91C=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp91C,_tag_fat(_tmp395,sizeof(struct Cyc_Absyn_Exp*),1));});}));});
# 1986
e1->r=_tmp91E;});
# 1988
((void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_tmp333=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp334=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp335=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->num_varargs;_tmp336=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->injectors;_tmp337=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3)->vai;{struct Cyc_Absyn_Exp*e1=_tmp333;struct Cyc_List_List*es=_tmp334;int num_varargs=_tmp335;struct Cyc_List_List*injectors=_tmp336;struct Cyc_Absyn_VarargInfo*vai=_tmp337;
# 1999 "toc.cyc"
struct _RegionHandle _tmp396=_new_region("r");struct _RegionHandle*r=& _tmp396;_push_region(r);{
struct _tuple30 _tmp397=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp31=_tmp397;struct _tuple30 _tmp398=_stmttmp31;struct Cyc_Absyn_Exp*_tmp39A;struct _tuple1*_tmp399;_tmp399=_tmp398.f1;_tmp39A=_tmp398.f2;{struct _tuple1*argv=_tmp399;struct Cyc_Absyn_Exp*argvexp=_tmp39A;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_signed_int_exp(num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2006
int num_args=((int(*)(struct Cyc_List_List*))Cyc_List_length)(es);
int num_normargs=num_args - num_varargs;
# 2010
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
new_args=({struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp39B->tl=new_args;_tmp39B;});}}
# 2015
new_args=({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct Cyc_Absyn_Exp*_tmp921=({struct Cyc_Absyn_Exp*_tmp39C[3];_tmp39C[0]=argvexp,({
# 2017
struct Cyc_Absyn_Exp*_tmp91F=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp39C[1]=_tmp91F;}),_tmp39C[2]=num_varargs_exp;({struct Cyc_Absyn_Exp*_tmp920=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp920,_tag_fat(_tmp39C,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 2015
_tmp39D->hd=_tmp921;}),_tmp39D->tl=new_args;_tmp39D;});
# 2020
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(new_args);{
# 2022
void*e1_typ=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
# 2027
({void*_tmp925=({void*_tmp924=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp924,({struct Cyc_Absyn_Exp*_tmp39E[1];({struct Cyc_Absyn_Exp*_tmp922=
Cyc_Absyn_copy_exp(e1);_tmp39E[0]=_tmp922;});({struct Cyc_Absyn_Exp*_tmp923=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp923,_tag_fat(_tmp39E,sizeof(struct Cyc_Absyn_Exp*),1));});}));});
# 2027
e1->r=_tmp925;});{
# 2029
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 2032
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp39F=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_stmttmp32=_tmp39F;void*_tmp3A0=_stmttmp32;struct Cyc_Absyn_Datatypedecl*_tmp3A1;if(*((int*)_tmp3A0)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A0)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A0)->f1)->f1).KnownDatatype).tag == 2){_tmp3A1=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A0)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*x=_tmp3A1;
tud=x;goto _LL97;}}else{goto _LL9A;}}else{goto _LL9A;}}else{_LL9A:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C4;_tmp7C4.tag=0,({struct _fat_ptr _tmp926=({const char*_tmp3A4="unknown datatype in vararg with inject";_tag_fat(_tmp3A4,sizeof(char),39U);});_tmp7C4.f1=_tmp926;});_tmp7C4;});void*_tmp3A2[1];_tmp3A2[0]=& _tmp3A3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp3A2,sizeof(void*),1));});}_LL97:;}{
# 2038
struct _fat_ptr vs=({unsigned _tmp3B0=(unsigned)num_varargs;struct _tuple1**_tmp3AF=({struct _RegionHandle*_tmp927=r;_region_malloc(_tmp927,_check_times(_tmp3B0,sizeof(struct _tuple1*)));});({{unsigned _tmp7C5=(unsigned)num_varargs;unsigned i;for(i=0;i < _tmp7C5;++ i){({struct _tuple1*_tmp928=Cyc_Toc_temp_var();_tmp3AF[i]=_tmp928;});}}0;});_tag_fat(_tmp3AF,sizeof(struct _tuple1*),_tmp3B0);});
# 2040
if(num_varargs != 0){
# 2044
struct Cyc_List_List*_tmp3A5=0;struct Cyc_List_List*array_args=_tmp3A5;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct Cyc_Absyn_Exp*_tmp929=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp3A6->hd=_tmp929;}),_tmp3A6->tl=array_args;_tmp3A6;});}}
s=({struct _tuple1*_tmp92C=argv;void*_tmp92B=arr_type;struct Cyc_Absyn_Exp*_tmp92A=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_tmp92C,_tmp92B,_tmp92A,s,0U);});
# 2049
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(es);
injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(injectors);{
int i=((int(*)(struct Cyc_List_List*))Cyc_List_length)(es)- 1;
for(1;es != 0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp3A7=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(injectors))->hd;struct Cyc_Absyn_Datatypefield*_stmttmp33=_tmp3A7;struct Cyc_Absyn_Datatypefield*_tmp3A8=_stmttmp33;struct Cyc_List_List*_tmp3AA;struct _tuple1*_tmp3A9;_tmp3A9=_tmp3A8->name;_tmp3AA=_tmp3A8->typs;{struct _tuple1*qv=_tmp3A9;struct Cyc_List_List*tqts=_tmp3AA;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2063
struct _tuple1*_tmp3AB=Cyc_Toc_collapse_qvars(qv,tud->name);struct _tuple1*tdn=_tmp3AB;
struct Cyc_List_List*_tmp3AC=({struct _tuple19*_tmp3AE[2];({struct _tuple19*_tmp930=({struct _fat_ptr*_tmp92F=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp92F,Cyc_Toc_datatype_tag(tud,qv));});_tmp3AE[0]=_tmp930;}),({
struct _tuple19*_tmp92E=({struct _fat_ptr*_tmp92D=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp92D,arg);});_tmp3AE[1]=_tmp92E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3AE,sizeof(struct _tuple19*),2));});
# 2064
struct Cyc_List_List*dles=_tmp3AC;
# 2066
s=({struct _tuple1*_tmp933=var;void*_tmp932=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_tmp931=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->tag=29,_tmp3AD->f1=tdn,_tmp3AD->f2=0,_tmp3AD->f3=dles,_tmp3AD->f4=0;_tmp3AD;}),0U);
# 2066
Cyc_Absyn_declare_stmt(_tmp933,_tmp932,_tmp931,s,0U);});}}}}}else{
# 2074
s=({struct _tuple1*_tmp936=argv;void*_tmp935=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp934=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp936,_tmp935,_tmp934,s,0U);});}}}else{
# 2079
if(num_varargs != 0){
struct Cyc_List_List*array_args=0;
for(1;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->hd=(struct Cyc_Absyn_Exp*)es->hd,_tmp3B1->tl=array_args;_tmp3B1;});}{
# 2085
struct Cyc_Absyn_Exp*_tmp3B2=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(array_args),0U);struct Cyc_Absyn_Exp*init=_tmp3B2;
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}else{
# 2088
s=({struct _tuple1*_tmp939=argv;void*_tmp938=Cyc_Toc_void_star_type();struct Cyc_Absyn_Exp*_tmp937=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp939,_tmp938,_tmp937,s,0U);});}}
# 2091
({void*_tmp93A=Cyc_Toc_stmt_exp_r(s);e->r=_tmp93A;});}}}}
# 2093
_npop_handler(0);goto _LL7;
# 1999
;_pop_region();}}case 11: _tmp331=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp332=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp331;int b=_tmp332;
# 2096
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_tmp93D=({void*_tmp93C=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp93C,({struct Cyc_Absyn_Exp*_tmp3B3[1];_tmp3B3[0]=e1;({struct Cyc_Absyn_Exp*_tmp93B=fn_e;Cyc_Toc_fncall_exp_dl(_tmp93B,_tag_fat(_tmp3B3,sizeof(struct Cyc_Absyn_Exp*),1));});}),0U);})->r;e->r=_tmp93D;});
goto _LL7;}}case 12: _tmp330=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_Absyn_Exp*e1=_tmp330;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13: _tmp32E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp32F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp32E;struct Cyc_List_List*ts=_tmp32F;
# 2102
Cyc_Toc_exp_to_c(nv,e1);
# 2111 "toc.cyc"
for(1;ts != 0;ts=ts->tl){
enum Cyc_Absyn_KindQual _tmp3B4=(Cyc_Tcutil_type_kind((void*)ts->hd))->kind;enum Cyc_Absyn_KindQual k=_tmp3B4;
if((int)k != (int)4U &&(int)k != (int)3U){
{void*_tmp3B5=Cyc_Absyn_compress((void*)ts->hd);void*_stmttmp34=_tmp3B5;void*_tmp3B6=_stmttmp34;switch(*((int*)_tmp3B6)){case 2:
 goto _LLA3;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f1)== 18){_LLA3:
 continue;}else{goto _LLA4;}default: _LLA4:
# 2118
({void*_tmp93F=({void*_tmp93E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp93E,e1,0U);})->r;e->r=_tmp93F;});
goto _LL9F;}_LL9F:;}
# 2121
break;}}
# 2124
goto _LL7;}case 14: _tmp32A=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp32B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp32C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp32D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;{void**t=_tmp32A;struct Cyc_Absyn_Exp*e1=_tmp32B;int user_inserted=_tmp32C;enum Cyc_Absyn_Coercion coercion=_tmp32D;
# 2126
void*old_t2=(void*)_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2133
{struct _tuple34 _tmp3B7=({struct _tuple34 _tmp7C7;({void*_tmp941=Cyc_Absyn_compress(old_t2);_tmp7C7.f1=_tmp941;}),({void*_tmp940=Cyc_Absyn_compress(new_typ);_tmp7C7.f2=_tmp940;});_tmp7C7;});struct _tuple34 _stmttmp35=_tmp3B7;struct _tuple34 _tmp3B8=_stmttmp35;struct Cyc_Absyn_PtrInfo _tmp3B9;struct Cyc_Absyn_PtrInfo _tmp3BB;struct Cyc_Absyn_PtrInfo _tmp3BA;if(*((int*)_tmp3B8.f1)== 3)switch(*((int*)_tmp3B8.f2)){case 3: _tmp3BA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8.f1)->f1;_tmp3BB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8.f2)->f1;{struct Cyc_Absyn_PtrInfo p1=_tmp3BA;struct Cyc_Absyn_PtrInfo p2=_tmp3BB;
# 2135
struct Cyc_Absyn_Exp*_tmp3BC=({void*_tmp942=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp942,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b1=_tmp3BC;
struct Cyc_Absyn_Exp*_tmp3BD=({void*_tmp943=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp943,(p2.ptr_atts).bounds);});struct Cyc_Absyn_Exp*b2=_tmp3BD;
int _tmp3BE=Cyc_Tcutil_force_type2bool(0,(p1.ptr_atts).zero_term);int zt1=_tmp3BE;
int _tmp3BF=Cyc_Tcutil_force_type2bool(0,(p2.ptr_atts).zero_term);int zt2=_tmp3BF;
{struct _tuple35 _tmp3C0=({struct _tuple35 _tmp7C6;_tmp7C6.f1=b1,_tmp7C6.f2=b2;_tmp7C6;});struct _tuple35 _stmttmp36=_tmp3C0;struct _tuple35 _tmp3C1=_stmttmp36;if(_tmp3C1.f1 != 0){if(_tmp3C1.f2 != 0){
# 2142
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp946=({void*_tmp945=*t;Cyc_Toc_cast_it_r(_tmp945,({struct Cyc_Absyn_Exp*_tmp3C2[1];_tmp3C2[0]=e1;({struct Cyc_Absyn_Exp*_tmp944=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_dl(_tmp944,_tag_fat(_tmp3C2,sizeof(struct Cyc_Absyn_Exp*),1));});}));});e->r=_tmp946;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;}
# 2151
goto _LLAD;}else{
# 2153
struct Cyc_Absyn_Exp*_tmp3C3=(struct Cyc_Absyn_Exp*)_check_null(b1);struct Cyc_Absyn_Exp*e2=_tmp3C3;
struct _tuple13 _tmp3C4=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp37=_tmp3C4;struct _tuple13 _tmp3C5=_stmttmp37;int _tmp3C7;unsigned _tmp3C6;_tmp3C6=_tmp3C5.f1;_tmp3C7=_tmp3C5.f2;{unsigned i=_tmp3C6;int valid=_tmp3C7;
if(Cyc_Toc_is_toplevel(nv)){
# 2159
if((zt1 && !(p2.elt_tq).real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_tmp947=e2;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp947,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_tmp948=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1))->r;e->r=_tmp948;});}else{
# 2164
if(zt1){
# 2169
struct _tuple30 _tmp3C8=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp38=_tmp3C8;struct _tuple30 _tmp3C9=_stmttmp38;struct Cyc_Absyn_Exp*_tmp3CB;struct _tuple1*_tmp3CA;_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;{struct _tuple1*x=_tmp3CA;struct Cyc_Absyn_Exp*x_exp=_tmp3CB;
struct Cyc_Absyn_Exp*arg3;
# 2173
{void*_tmp3CC=e1->r;void*_stmttmp39=_tmp3CC;void*_tmp3CD=_stmttmp39;struct Cyc_Absyn_Vardecl*_tmp3CE;struct Cyc_Absyn_Vardecl*_tmp3CF;switch(*((int*)_tmp3CD)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3CD)->f1).Wstring_c).tag){case 8:
 arg3=e2;goto _LLBC;case 9:
 arg3=e2;goto _LLBC;default: goto _LLC5;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD)->f1)){case 1: _tmp3CF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp3CF;
_tmp3CE=vd;goto _LLC4;}case 4: _tmp3CE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CD)->f1)->f1;_LLC4: {struct Cyc_Absyn_Vardecl*vd=_tmp3CE;
# 2178
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC6;
arg3=e2;
goto _LLBC;}default: goto _LLC5;}default: _LLC5: _LLC6:
# 2185
 if(valid && i != (unsigned)1)
arg3=e2;else{
# 2188
arg3=({struct Cyc_Absyn_Exp*_tmp3D0[2];({
struct Cyc_Absyn_Exp*_tmp94A=({void*_tmp949=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_tmp949,x_exp);});_tmp3D0[0]=_tmp94A;}),_tmp3D0[1]=e2;({struct Cyc_Absyn_Exp*_tmp94B=
# 2188
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,e1);Cyc_Toc_fncall_exp_dl(_tmp94B,_tag_fat(_tmp3D0,sizeof(struct Cyc_Absyn_Exp*),2));});});}
# 2190
goto _LLBC;}_LLBC:;}
# 2192
if(!zt2 && !(p2.elt_tq).real_const)
# 2195
arg3=({struct Cyc_Absyn_Exp*_tmp94C=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp94C,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2197
struct Cyc_Absyn_Exp*_tmp3D1=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);struct Cyc_Absyn_Exp*arg2=_tmp3D1;
struct Cyc_Absyn_Exp*_tmp3D2=({struct Cyc_Absyn_Exp*_tmp3D4[3];_tmp3D4[0]=x_exp,_tmp3D4[1]=arg2,_tmp3D4[2]=arg3;({struct Cyc_Absyn_Exp*_tmp94D=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp94D,_tag_fat(_tmp3D4,sizeof(struct Cyc_Absyn_Exp*),3));});});struct Cyc_Absyn_Exp*tg_exp=_tmp3D2;
struct Cyc_Absyn_Stmt*_tmp3D3=Cyc_Absyn_exp_stmt(tg_exp,0U);struct Cyc_Absyn_Stmt*s=_tmp3D3;
s=({struct _tuple1*_tmp950=x;void*_tmp94F=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp94E=e1;Cyc_Absyn_declare_stmt(_tmp950,_tmp94F,_tmp94E,s,0U);});
({void*_tmp951=Cyc_Toc_stmt_exp_r(s);e->r=_tmp951;});}}}else{
# 2204
({void*_tmp954=({struct Cyc_Absyn_Exp*_tmp3D5[3];_tmp3D5[0]=e1,({
# 2206
struct Cyc_Absyn_Exp*_tmp952=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_tmp3D5[1]=_tmp952;}),_tmp3D5[2]=e2;({struct Cyc_Absyn_Exp*_tmp953=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_r(_tmp953,_tag_fat(_tmp3D5,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 2204
e->r=_tmp954;});}}
# 2209
goto _LLAD;}}}else{if(_tmp3C1.f2 != 0){
# 2219 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3D6=(struct Cyc_Absyn_Exp*)_check_null(b2);struct Cyc_Absyn_Exp*new_e2=_tmp3D6;
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_tmp955=b2;Cyc_Absyn_add_exp(_tmp955,Cyc_Absyn_uint_exp(1U,0U),0U);});{
# 2226
struct Cyc_Absyn_Exp*_tmp3D7=({struct Cyc_Absyn_Exp*_tmp3D9[3];_tmp3D9[0]=e1,({
# 2228
struct Cyc_Absyn_Exp*_tmp956=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3D9[1]=_tmp956;}),_tmp3D9[2]=new_e2;({struct Cyc_Absyn_Exp*_tmp957=Cyc_Toc__untag_fat_ptr_e;Cyc_Toc_fncall_exp_dl(_tmp957,_tag_fat(_tmp3D9,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 2226
struct Cyc_Absyn_Exp*ptr_exp=_tmp3D7;
# 2230
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_tmp95A=({struct Cyc_Absyn_Exp*_tmp3D8[1];({struct Cyc_Absyn_Exp*_tmp958=Cyc_Absyn_copy_exp(ptr_exp);_tmp3D8[0]=_tmp958;});({struct Cyc_Absyn_Exp*_tmp959=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp959,_tag_fat(_tmp3D8,sizeof(struct Cyc_Absyn_Exp*),1));});});ptr_exp->r=_tmp95A;});
({void*_tmp95B=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_tmp95B;});
goto _LLAD;}}else{
# 2238
if((zt1 && !zt2)&& !(p2.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
((int(*)(struct _fat_ptr))Cyc_Toc_unimp)(({const char*_tmp3DA="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_fat(_tmp3DA,sizeof(char),70U);}));
({void*_tmp95F=({struct Cyc_Absyn_Exp*_tmp3DB[3];_tmp3DB[0]=e1,({
struct Cyc_Absyn_Exp*_tmp95D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_tmp3DB[1]=_tmp95D;}),({
struct Cyc_Absyn_Exp*_tmp95C=Cyc_Absyn_uint_exp(1U,0U);_tmp3DB[2]=_tmp95C;});({struct Cyc_Absyn_Exp*_tmp95E=Cyc_Toc__fat_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp95E,_tag_fat(_tmp3DB,sizeof(struct Cyc_Absyn_Exp*),3));});});
# 2241
e->r=_tmp95F;});}
# 2245
goto _LLAD;}}_LLAD:;}
# 2247
goto _LLA6;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B8.f2)->f1)== 1){_tmp3B9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8.f1)->f1;{struct Cyc_Absyn_PtrInfo p1=_tmp3B9;
# 2249
{struct Cyc_Absyn_Exp*_tmp3DC=({void*_tmp960=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp960,(p1.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_stmttmp3A=_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD=_stmttmp3A;if(_tmp3DD == 0){
# 2251
({void*_tmp962=({struct Cyc_Absyn_Exp*_tmp961=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_tmp961,Cyc_Toc_curr_sp);});e1->r=_tmp962;});
e1->topt=new_typ_c;
goto _LLC7;}else{
goto _LLC7;}_LLC7:;}
# 2256
goto _LLA6;}}else{goto _LLAB;}default: goto _LLAB;}else{_LLAB:
# 2258
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
e->r=e1->r;
goto _LLA6;}_LLA6:;}
# 2262
goto _LL7;}case 15: _tmp329=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_Absyn_Exp*e1=_tmp329;
# 2265
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
((void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int))Cyc_Toc_lvalue_assign)(e1,0,Cyc_Toc_address_lvalue,1);
# 2271
({void*_tmp964=({void*_tmp963=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp963,e1);});e->r=_tmp964;});}else{
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind((void*)_check_null(e1->topt))))
# 2275
({void*_tmp966=({void*_tmp965=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp965,e1);});e->r=_tmp966;});}
# 2277
goto _LL7;}case 16: _tmp327=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp328=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*rgnopt=_tmp327;struct Cyc_Absyn_Exp*e1=_tmp328;
# 2284
Cyc_Toc_exp_to_c(nv,e1);{
# 2286
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(e1->topt));
struct _tuple30 _tmp3DE=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp3B=_tmp3DE;struct _tuple30 _tmp3DF=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp3E1;struct _tuple1*_tmp3E0;_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;{struct _tuple1*x=_tmp3E0;struct Cyc_Absyn_Exp*xexp=_tmp3E1;
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3E2=Cyc_Absyn_compress(elt_typ);void*_stmttmp3C=_tmp3E2;void*_tmp3E3=_stmttmp3C;void*_tmp3E4;if(*((int*)_tmp3E3)== 4){_tmp3E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E3)->f1).elt_type;{void*t2=_tmp3E4;
# 2291
elt_typ=Cyc_Toc_typ_to_c(t2);
lhs=Cyc_Absyn_copy_exp(xexp);
goto _LLCF;}}else{
# 2295
lhs=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(xexp),0U);
goto _LLCF;}_LLCF:;}{
# 2298
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*_tmp3E5=(unsigned)array_len?Cyc_Toc_temp_var(): 0;struct _tuple1*lenvar=_tmp3E5;
struct Cyc_Absyn_Exp*_tmp3E6=(unsigned)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(lenvar),0U): 0;struct Cyc_Absyn_Exp*lenexp=_tmp3E6;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_tmp3E7[2];_tmp3E7[0]=lenexp,({struct Cyc_Absyn_Exp*_tmp967=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3E7[1]=_tmp967;});({struct Cyc_Absyn_Exp*_tmp968=Cyc_Toc__check_times_e;Cyc_Toc_fncall_exp_dl(_tmp968,_tag_fat(_tmp3E7,sizeof(struct Cyc_Absyn_Exp*),2));});});else{
# 2305
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(xexp,0U),0U);}{
# 2308
struct Cyc_Absyn_Exp*_tmp3E8=Cyc_Toc_get_varsizeexp(e1);struct Cyc_Absyn_Exp*vse=_tmp3E8;
if(vse != 0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2312
if(rgnopt == 0 || Cyc_Flags_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2315
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=Cyc_Toc_rmalloc_exp(rgnopt,mexp);}{
# 2318
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_fat_ptr(old_typ))
answer=({struct Cyc_Absyn_Exp*_tmp3E9[3];({
struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_copy_exp(xexp);_tmp3E9[0]=_tmp96B;}),({
struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp3E9[1]=_tmp96A;}),
(unsigned)lenexp?_tmp3E9[2]=lenexp:({struct Cyc_Absyn_Exp*_tmp969=Cyc_Absyn_uint_exp(1U,0U);_tmp3E9[2]=_tmp969;});({struct Cyc_Absyn_Exp*_tmp96C=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp96C,_tag_fat(_tmp3E9,sizeof(struct Cyc_Absyn_Exp*),3));});});else{
# 2325
answer=Cyc_Absyn_copy_exp(xexp);}
({void*_tmp96D=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->tag=Cyc_Toc_Dest,_tmp3EA->f1=xexp;_tmp3EA;});e->annot=_tmp96D;});{
struct Cyc_Absyn_Stmt*_tmp3EB=({struct _tuple1*_tmp971=x;void*_tmp970=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp96F=mexp;Cyc_Absyn_declare_stmt(_tmp971,_tmp970,_tmp96F,({
struct Cyc_Absyn_Stmt*_tmp96E=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp96E,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2327
struct Cyc_Absyn_Stmt*s=_tmp3EB;
# 2331
if((unsigned)array_len)
s=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(lenvar),Cyc_Absyn_uint_type,array_len,s,0U);
({void*_tmp972=Cyc_Toc_stmt_exp_r(s);e->r=_tmp972;});
if(vse != 0)
({void*_tmp974=({void*_tmp973=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp973,Cyc_Absyn_copy_exp(e));});e->r=_tmp974;});
goto _LL7;}}}}}}}case 18: _tmp326=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_Absyn_Exp*e1=_tmp326;
# 2339
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17: _tmp325=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{void*t=_tmp325;
({void*_tmp976=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->tag=17,({void*_tmp975=Cyc_Toc_typ_to_c(t);_tmp3EC->f1=_tmp975;});_tmp3EC;});e->r=_tmp976;});goto _LL7;}case 19: _tmp323=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp324=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{void*t=_tmp323;struct Cyc_List_List*fs=_tmp324;
# 2342
void*_tmp3ED=t;void*t2=_tmp3ED;
struct Cyc_List_List*_tmp3EE=fs;struct Cyc_List_List*l=_tmp3EE;
for(1;l != 0;l=l->tl){
void*_tmp3EF=(void*)l->hd;void*_stmttmp3D=_tmp3EF;void*_tmp3F0=_stmttmp3D;unsigned _tmp3F1;struct _fat_ptr*_tmp3F2;if(*((int*)_tmp3F0)== 0){_tmp3F2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3F0)->f1;{struct _fat_ptr*n=_tmp3F2;
goto _LLD4;}}else{_tmp3F1=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3F0)->f1;{unsigned n=_tmp3F1;
# 2348
{void*_tmp3F3=Cyc_Absyn_compress(t2);void*_stmttmp3E=_tmp3F3;void*_tmp3F4=_stmttmp3E;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_Absyn_Datatypefield*_tmp3F7;union Cyc_Absyn_AggrInfo _tmp3F8;switch(*((int*)_tmp3F4)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F4)->f1)){case 20: _tmp3F8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F4)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp3F8;
# 2350
struct Cyc_Absyn_Aggrdecl*_tmp3F9=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp3F9;
if(ad->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C8;_tmp7C8.tag=0,({struct _fat_ptr _tmp977=({const char*_tmp3FC="struct fields must be known";_tag_fat(_tmp3FC,sizeof(char),28U);});_tmp7C8.f1=_tmp977;});_tmp7C8;});void*_tmp3FA[1];_tmp3FA[0]=& _tmp3FB;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp3FA,sizeof(void*),1));});
_tmp3F6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LLDD;}case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F4)->f1)->f1).KnownDatatypefield).tag == 2){_tmp3F7=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F4)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*tuf=_tmp3F7;
# 2364
if(n == (unsigned)0)
({void*_tmp978=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->tag=0,_tmp400->f1=Cyc_Toc_tag_sp;_tmp400;}));l->hd=_tmp978;});else{
# 2367
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;
({void*_tmp97A=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->tag=0,({struct _fat_ptr*_tmp979=Cyc_Absyn_fieldname((int)n);_tmp401->f1=_tmp979;});_tmp401;}));l->hd=_tmp97A;});}
# 2370
goto _LLD9;}}else{goto _LLE2;}default: goto _LLE2;}case 7: _tmp3F6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F4)->f2;_LLDD: {struct Cyc_List_List*fields=_tmp3F6;
# 2355
struct Cyc_Absyn_Aggrfield*_tmp3FD=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(fields,(int)n);struct Cyc_Absyn_Aggrfield*nth_field=_tmp3FD;
({void*_tmp97B=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->tag=0,_tmp3FE->f1=nth_field->name;_tmp3FE;}));l->hd=_tmp97B;});
t2=nth_field->type;
goto _LLD9;}case 6: _tmp3F5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F4)->f1;{struct Cyc_List_List*ts=_tmp3F5;
# 2360
({void*_tmp97D=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->tag=0,({struct _fat_ptr*_tmp97C=Cyc_Absyn_fieldname((int)(n + (unsigned)1));_tmp3FF->f1=_tmp97C;});_tmp3FF;}));l->hd=_tmp97D;});
t2=(*((struct _tuple23*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(ts,(int)n)).f2;
goto _LLD9;}default: _LLE2:
# 2371
({struct Cyc_Warn_String_Warn_Warg_struct _tmp403=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7C9;_tmp7C9.tag=0,({struct _fat_ptr _tmp97E=({const char*_tmp404="impossible type for offsetof tuple index";_tag_fat(_tmp404,sizeof(char),41U);});_tmp7C9.f1=_tmp97E;});_tmp7C9;});void*_tmp402[1];_tmp402[0]=& _tmp403;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp402,sizeof(void*),1));});}_LLD9:;}
# 2373
goto _LLD4;}}_LLD4:;}
# 2376
({void*_tmp980=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->tag=19,({void*_tmp97F=Cyc_Toc_typ_to_c(t);_tmp405->f1=_tmp97F;}),_tmp405->f2=fs;_tmp405;});e->r=_tmp980;});
goto _LL7;}case 21: _tmp31F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp320=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp321=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp322=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;{struct Cyc_Absyn_Exp*e1=_tmp31F;struct _fat_ptr*f=_tmp320;int is_tagged=_tmp321;int is_read=_tmp322;
# 2379
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_tmp985=({struct Cyc_Absyn_Exp*_tmp984=e1;void*_tmp983=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp982=e1_cyc_type;struct _fat_ptr*_tmp981=f;Cyc_Toc_check_tagged_union(_tmp984,_tmp983,_tmp982,_tmp981,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2383
e->r=_tmp985;});
# 2385
if(is_poly)
({void*_tmp987=({void*_tmp986=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp986,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp987;});
goto _LL7;}case 22: _tmp31B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp31C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp31D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp31E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;{struct Cyc_Absyn_Exp*e1=_tmp31B;struct _fat_ptr*f=_tmp31C;int is_tagged=_tmp31D;int is_read=_tmp31E;
# 2389
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(e1->topt);
void*ta;
{void*_tmp406=Cyc_Absyn_compress(e1typ);void*_stmttmp3F=_tmp406;void*_tmp407=_stmttmp3F;struct Cyc_Absyn_PtrInfo _tmp408;if(*((int*)_tmp407)== 3){_tmp408=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp407)->f1;{struct Cyc_Absyn_PtrInfo p=_tmp408;
ta=p.elt_type;goto _LLE4;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CA;_tmp7CA.tag=0,({struct _fat_ptr _tmp988=({const char*_tmp40B="get_ptr_typ: not a pointer!";_tag_fat(_tmp40B,sizeof(char),28U);});_tmp7CA.f1=_tmp988;});_tmp7CA;});void*_tmp409[1];_tmp409[0]=& _tmp40A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp409,sizeof(void*),1));});}_LLE4:;}
# 2396
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_tmp98D=({struct Cyc_Absyn_Exp*_tmp98C=e1;void*_tmp98B=Cyc_Toc_typ_to_c(e1typ);void*_tmp98A=ta;struct _fat_ptr*_tmp989=f;Cyc_Toc_check_tagged_union(_tmp98C,_tmp98B,_tmp98A,_tmp989,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp98D;});
if(is_poly && is_read)
({void*_tmp98F=({void*_tmp98E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp98E,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp98F;});
goto _LL7;}case 20: _tmp31A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_Absyn_Exp*e1=_tmp31A;
# 2404
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23: _tmp318=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp319=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp318;struct Cyc_Absyn_Exp*e2=_tmp319;
# 2408
{void*_tmp40C=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp40=_tmp40C;void*_tmp40D=_stmttmp40;struct Cyc_List_List*_tmp40E;if(*((int*)_tmp40D)== 6){_tmp40E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40D)->f1;{struct Cyc_List_List*ts=_tmp40E;
# 2410
Cyc_Toc_exp_to_c(nv,e1);{
int _tmp40F=Cyc_Toc_in_lhs(nv);int old_lhs=_tmp40F;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
struct _tuple13 _tmp410=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp41=_tmp410;struct _tuple13 _tmp411=_stmttmp41;int _tmp413;unsigned _tmp412;_tmp412=_tmp411.f1;_tmp413=_tmp411.f2;{unsigned i=_tmp412;int known=_tmp413;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp415=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CB;_tmp7CB.tag=0,({struct _fat_ptr _tmp990=({const char*_tmp416="unknown tuple subscript in translation to C";_tag_fat(_tmp416,sizeof(char),44U);});_tmp7CB.f1=_tmp990;});_tmp7CB;});void*_tmp414[1];_tmp414[0]=& _tmp415;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp414,sizeof(void*),1));});
({void*_tmp992=({struct Cyc_Absyn_Exp*_tmp991=e1;Cyc_Toc_aggrmember_exp_r(_tmp991,Cyc_Absyn_fieldname((int)(i + (unsigned)1)));});e->r=_tmp992;});
goto _LLE9;}}}}}else{
# 2420
did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_tmp993=Cyc_Toc_deref_exp_r(e1);e->r=_tmp993;});
goto _LLE9;}}_LLE9:;}
# 2426
goto _LL7;}case 24: _tmp317=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_List_List*es=_tmp317;
# 2428
if(!Cyc_Toc_is_toplevel(nv)){
# 2430
void*_tmp417=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Toc_tup_to_c,es));void*strct_typ=_tmp417;
{void*_tmp418=strct_typ;union Cyc_Absyn_AggrInfo _tmp419;if(*((int*)_tmp418)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp418)->f1)== 20){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp418)->f2 == 0){_tmp419=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp418)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_tmp419;
# 2433
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp41A=_cycalloc(sizeof(*_tmp41A));({struct _tuple19*_tmp995=({struct _fat_ptr*_tmp994=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp994,(struct Cyc_Absyn_Exp*)es->hd);});_tmp41A->hd=_tmp995;}),_tmp41A->tl=dles;_tmp41A;});}}
# 2439
({void*_tmp997=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->tag=29,_tmp41B->f1=sd->name,_tmp41B->f2=0,({struct Cyc_List_List*_tmp996=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);_tmp41B->f3=_tmp996;}),_tmp41B->f4=sd;_tmp41B;});e->r=_tmp997;});
e->topt=strct_typ;
goto _LLF1;}}else{goto _LLF4;}}else{goto _LLF4;}}else{_LLF4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp41D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CC;_tmp7CC.tag=0,({struct _fat_ptr _tmp998=({const char*_tmp41E="tuple type not an aggregate";_tag_fat(_tmp41E,sizeof(char),28U);});_tmp7CC.f1=_tmp998;});_tmp7CC;});void*_tmp41C[1];_tmp41C[0]=& _tmp41D;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp41C,sizeof(void*),1));});}_LLF1:;}
# 2444
goto _LL7;}else{
# 2448
struct Cyc_List_List*dles=0;
for(1;es != 0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
dles=({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));({struct _tuple19*_tmp999=({struct _tuple19*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->f1=0,_tmp41F->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp41F;});_tmp420->hd=_tmp999;}),_tmp420->tl=dles;_tmp420;});}
# 2453
({void*_tmp99A=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles));e->r=_tmp99A;});}
# 2455
goto _LL7;}case 26: _tmp316=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_List_List*dles0=_tmp316;
# 2461
{struct Cyc_List_List*_tmp421=dles0;struct Cyc_List_List*dles=_tmp421;for(0;dles != 0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp99B=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_tmp99B;});
goto _LL7;}case 27: _tmp312=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp313=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp314=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp315=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp312;struct Cyc_Absyn_Exp*e1=_tmp313;struct Cyc_Absyn_Exp*e2=_tmp314;int iszeroterm=_tmp315;
# 2469
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp422=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple13 _stmttmp42=_tmp422;struct _tuple13 _tmp423=_stmttmp42;int _tmp425;unsigned _tmp424;_tmp424=_tmp423.f1;_tmp425=_tmp423.f2;{unsigned sz=_tmp424;int known=_tmp425;
void*_tmp426=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));void*elt_typ=_tmp426;
struct Cyc_List_List*es=0;
# 2476
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({void*_tmp427=0U;({unsigned _tmp99D=e1->loc;struct _fat_ptr _tmp99C=({const char*_tmp428="cannot determine value of constant";_tag_fat(_tmp428,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp99D,_tmp99C,_tag_fat(_tmp427,sizeof(void*),0));});});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_tmp42A=_cycalloc(sizeof(*_tmp42A));({struct _tuple19*_tmp99E=({struct _tuple19*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->f1=0,_tmp429->f2=e2;_tmp429;});_tmp42A->hd=_tmp99E;}),_tmp42A->tl=es;_tmp42A;});}}
# 2482
if(iszeroterm){
struct Cyc_Absyn_Exp*_tmp42B=({void*_tmp99F=elt_typ;Cyc_Toc_cast_it(_tmp99F,Cyc_Absyn_uint_exp(0U,0U));});struct Cyc_Absyn_Exp*ezero=_tmp42B;
es=({struct Cyc_List_List*_tmp9A1=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp9A1,({struct Cyc_List_List*_tmp42D=_cycalloc(sizeof(*_tmp42D));({struct _tuple19*_tmp9A0=({struct _tuple19*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->f1=0,_tmp42C->f2=ezero;_tmp42C;});_tmp42D->hd=_tmp9A0;}),_tmp42D->tl=0;_tmp42D;}));});}}
# 2487
({void*_tmp9A2=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9A2;});}}
# 2489
goto _LL7;}case 28: _tmp30F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp310=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp311=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;{struct Cyc_Absyn_Exp*e1=_tmp30F;void*t1=_tmp310;int iszeroterm=_tmp311;
# 2496
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9A3=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9A3;});else{
# 2499
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 30: _tmp30D=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp30E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{void*st=_tmp30D;struct Cyc_List_List*dles=_tmp30E;
# 2503
{struct Cyc_List_List*_tmp42E=dles;struct Cyc_List_List*dles2=_tmp42E;for(0;dles2 != 0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f2);}}{
void*_tmp42F=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));void*strct_typ=_tmp42F;
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp430=Cyc_Absyn_compress(strct_typ);void*_stmttmp43=_tmp430;void*_tmp431=_stmttmp43;union Cyc_Absyn_AggrInfo _tmp432;if(*((int*)_tmp431)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->f1)== 20){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->f2 == 0){_tmp432=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp431)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_tmp432;
# 2510
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_tmp9A4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=29,_tmp433->f1=sd->name,_tmp433->f2=0,_tmp433->f3=dles,_tmp433->f4=sd;_tmp433;});e->r=_tmp9A4;});
e->topt=strct_typ;
goto _LLF9;}}else{goto _LLFC;}}else{goto _LLFC;}}else{_LLFC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp435=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CD;_tmp7CD.tag=0,({struct _fat_ptr _tmp9A5=({const char*_tmp436="anonStruct type not an aggregate";_tag_fat(_tmp436,sizeof(char),33U);});_tmp7CD.f1=_tmp9A5;});_tmp7CD;});void*_tmp434[1];_tmp434[0]=& _tmp435;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp434,sizeof(void*),1));});}_LLF9:;}else{
# 2517
({void*_tmp9A6=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_tmp9A6;});}
goto _LL7;}}case 29: _tmp309=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp30A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp30B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp30C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;{struct _tuple1**tdn=_tmp309;struct Cyc_List_List*exist_ts=_tmp30A;struct Cyc_List_List*dles=_tmp30B;struct Cyc_Absyn_Aggrdecl*sd=_tmp30C;
# 2523
if(sd == 0 || sd->impl == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp438=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CE;_tmp7CE.tag=0,({struct _fat_ptr _tmp9A7=({const char*_tmp439="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_fat(_tmp439,sizeof(char),58U);});_tmp7CE.f1=_tmp9A7;});_tmp7CE;});void*_tmp437[1];_tmp437[0]=& _tmp438;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp437,sizeof(void*),1));});{
void*_tmp43A=Cyc_Toc_typ_to_c(old_typ);void*new_typ=_tmp43A;
{void*_tmp43B=Cyc_Absyn_compress(new_typ);void*_stmttmp44=_tmp43B;void*_tmp43C=_stmttmp44;union Cyc_Absyn_AggrInfo _tmp43D;if(*((int*)_tmp43C)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43C)->f1)== 20){_tmp43D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43C)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_tmp43D;
({struct _tuple1*_tmp9A8=(Cyc_Absyn_aggr_kinded_name(info)).f2;*tdn=_tmp9A8;});goto _LLFE;}}else{goto _LL101;}}else{_LL101:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7CF;_tmp7CF.tag=0,({struct _fat_ptr _tmp9A9=({const char*_tmp440="exp_to_c, Aggregate_e: bad type translation";_tag_fat(_tmp440,sizeof(char),44U);});_tmp7CF.f1=_tmp9A9;});_tmp7CF;});void*_tmp43E[1];_tmp43E[0]=& _tmp43F;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp43E,sizeof(void*),1));});}_LLFE:;}{
# 2534
struct Cyc_List_List*_tmp441=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*typ_fields=_tmp441;
if(typ_fields == 0)goto _LL7;
for(1;((struct Cyc_List_List*)_check_null(typ_fields))->tl != 0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp442=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;struct Cyc_Absyn_Aggrfield*last_typ_field=_tmp442;
struct Cyc_List_List*_tmp443=((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,dles,sd->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields);struct Cyc_List_List*fields=_tmp443;
# 2541
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp444=Cyc_Absyn_compress(old_typ);void*_stmttmp45=_tmp444;void*_tmp445=_stmttmp45;struct Cyc_List_List*_tmp446;if(*((int*)_tmp445)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f1)== 20){_tmp446=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f2;{struct Cyc_List_List*param_ts=_tmp446;
# 2545
{struct Cyc_List_List*_tmp447=fields;struct Cyc_List_List*fields2=_tmp447;for(0;fields2 != 0;fields2=fields2->tl){
struct _tuple36*_tmp448=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp46=_tmp448;struct _tuple36*_tmp449=_stmttmp46;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Aggrfield*_tmp44A;_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;{struct Cyc_Absyn_Aggrfield*aggrfield=_tmp44A;struct Cyc_Absyn_Exp*fieldexp=_tmp44B;
void*_tmp44C=fieldexp->topt;void*old_field_typ=_tmp44C;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2550
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(fieldexp->topt)))
({void*_tmp9AB=({void*_tmp9AA=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_tmp9AA,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2552
fieldexp->r=_tmp9AB;});
# 2555
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
struct _fat_ptr*_tmp44D=aggrfield->name;struct _fat_ptr*n=_tmp44D;
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);struct Cyc_Absyn_Exp*tag_exp=_tmp44E;
struct _tuple19*_tmp44F=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp44F;
struct _tuple19*_tmp450=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);struct _tuple19*val_dle=_tmp450;
struct _tuple1*s=({struct _tuple1*_tmp45B=_cycalloc(sizeof(*_tmp45B));({union Cyc_Absyn_Nmspace _tmp9AF=Cyc_Absyn_Abs_n(0,1);_tmp45B->f1=_tmp9AF;}),({
struct _fat_ptr*_tmp9AE=({struct _fat_ptr*_tmp45A=_cycalloc(sizeof(*_tmp45A));({struct _fat_ptr _tmp9AD=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp458=({struct Cyc_String_pa_PrintArg_struct _tmp7D1;_tmp7D1.tag=0,_tmp7D1.f1=(struct _fat_ptr)((struct _fat_ptr)*(*sd->name).f2);_tmp7D1;});struct Cyc_String_pa_PrintArg_struct _tmp459=({struct Cyc_String_pa_PrintArg_struct _tmp7D0;_tmp7D0.tag=0,_tmp7D0.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp7D0;});void*_tmp456[2];_tmp456[0]=& _tmp458,_tmp456[1]=& _tmp459;({struct _fat_ptr _tmp9AC=({const char*_tmp457="_union_%s_%s";_tag_fat(_tmp457,sizeof(char),13U);});Cyc_aprintf(_tmp9AC,_tag_fat(_tmp456,sizeof(void*),2));});});*_tmp45A=_tmp9AD;});_tmp45A;});_tmp45B->f2=_tmp9AE;});_tmp45B;});
# 2563
struct _tuple19*_tmp451=({
struct _fat_ptr*_tmp9B1=n;Cyc_Toc_make_field(_tmp9B1,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->tag=29,_tmp455->f1=s,_tmp455->f2=0,({struct Cyc_List_List*_tmp9B0=({struct _tuple19*_tmp454[2];_tmp454[0]=tag_dle,_tmp454[1]=val_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp454,sizeof(struct _tuple19*),2));});_tmp455->f3=_tmp9B0;}),_tmp455->f4=0;_tmp455;}),0U));});
# 2563
struct _tuple19*u_dle=_tmp451;
# 2566
({void*_tmp9B3=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->tag=29,_tmp453->f1=*tdn,_tmp453->f2=0,({struct Cyc_List_List*_tmp9B2=({struct _tuple19*_tmp452[1];_tmp452[0]=u_dle;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp452,sizeof(struct _tuple19*),1));});_tmp453->f3=_tmp9B2;}),_tmp453->f4=sd;_tmp453;});e->r=_tmp9B3;});}
# 2572
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field == aggrfield){
struct Cyc_List_List*_tmp45C=({struct Cyc_List_List*_tmp9B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->exist_vars,exist_ts);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp9B4,
((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip)(sd->tvs,param_ts));});
# 2573
struct Cyc_List_List*inst=_tmp45C;
# 2575
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp45D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*fs=_tmp45D;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp45E=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*old_f=_tmp45E;
void*_tmp45F=Cyc_Tcutil_substitute(inst,old_f->type);void*old_ftyp=_tmp45F;
struct Cyc_Absyn_Aggrfield*_tmp460=({struct Cyc_Absyn_Aggrfield*_tmp9B5=old_f;Cyc_Toc_aggrfield_to_c(_tmp9B5,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
# 2579
struct Cyc_Absyn_Aggrfield*new_f=_tmp460;
# 2581
new_fields=({struct Cyc_List_List*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->hd=new_f,_tmp461->tl=new_fields;_tmp461;});
# 2587
if(fs->tl == 0){
{void*_tmp462=Cyc_Absyn_compress(new_f->type);void*_stmttmp47=_tmp462;void*_tmp463=_stmttmp47;struct Cyc_Absyn_ArrayInfo _tmp464;if(*((int*)_tmp463)== 4){_tmp464=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp463)->f1;{struct Cyc_Absyn_ArrayInfo ai=_tmp464;
# 2590
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp465=ai;struct Cyc_Absyn_ArrayInfo ai2=_tmp465;
({struct Cyc_Absyn_Exp*_tmp9B6=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_tmp9B6;});
({void*_tmp9B7=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->tag=4,_tmp466->f1=ai2;_tmp466;});new_f->type=_tmp9B7;});}
# 2595
goto _LL10B;}}else{
# 2599
if(fieldexp->topt == 0)
goto _LL10B;
{void*_tmp467=Cyc_Absyn_compress((void*)_check_null(fieldexp->topt));void*_stmttmp48=_tmp467;void*_tmp468=_stmttmp48;if(*((int*)_tmp468)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp468)->f1)== 20){
# 2603
new_f->type=(void*)_check_null(fieldexp->topt);goto _LL110;}else{goto _LL113;}}else{_LL113:
 goto _LL110;}_LL110:;}
# 2606
goto _LL10B;}_LL10B:;}
# 2610
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_tmp9B9=({struct Cyc_List_List*_tmp46A=_cycalloc(sizeof(*_tmp46A));({void*_tmp9B8=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->tag=6,_tmp469->f1=0;_tmp469;});_tmp46A->hd=_tmp9B8;}),_tmp46A->tl=new_f->attributes;_tmp46A;});new_f->attributes=_tmp9B9;});}}}
# 2616
sd=({struct _fat_ptr*_tmp9BC=({struct _fat_ptr*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct _fat_ptr _tmp9BB=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp46D=({struct Cyc_Int_pa_PrintArg_struct _tmp7D2;_tmp7D2.tag=1,_tmp7D2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7D2;});void*_tmp46B[1];_tmp46B[0]=& _tmp46D;({struct _fat_ptr _tmp9BA=({const char*_tmp46C="_genStruct%d";_tag_fat(_tmp46C,sizeof(char),13U);});Cyc_aprintf(_tmp9BA,_tag_fat(_tmp46B,sizeof(void*),1));});});*_tmp46E=_tmp9BB;});_tmp46E;});Cyc_Toc_make_c_struct_defn(_tmp9BC,
# 2618
((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2622
({void*_tmp9BD=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp46F=_cycalloc(sizeof(*_tmp46F));*_tmp46F=sd;_tmp46F;})),0);e->topt=_tmp9BD;});}}}}
# 2625
goto _LL103;}}else{goto _LL106;}}else{_LL106:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp471=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D3;_tmp7D3.tag=0,({struct _fat_ptr _tmp9BE=({const char*_tmp472="exp_to_c, Aggregate_e: bad struct type";_tag_fat(_tmp472,sizeof(char),39U);});_tmp7D3.f1=_tmp9BE;});_tmp7D3;});void*_tmp470[1];_tmp470[0]=& _tmp471;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp470,sizeof(void*),1));});}_LL103:;}else{
# 2634
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->tagged){
# 2636
struct _tuple36*_tmp473=(struct _tuple36*)((struct Cyc_List_List*)_check_null(fields))->hd;struct _tuple36*_stmttmp49=_tmp473;struct _tuple36*_tmp474=_stmttmp49;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Aggrfield*_tmp475;_tmp475=_tmp474->f1;_tmp476=_tmp474->f2;{struct Cyc_Absyn_Aggrfield*field=_tmp475;struct Cyc_Absyn_Exp*fieldexp=_tmp476;
void*_tmp477=(void*)_check_null(fieldexp->topt);void*fieldexp_type=_tmp477;
void*_tmp478=field->type;void*fieldtyp=_tmp478;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_tmp9C0=({
void*_tmp9BF=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_tmp9BF,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2642
fieldexp->r=_tmp9C0;});{
# 2645
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp479=({struct _tuple19*_tmp47B[2];({struct _tuple19*_tmp9C2=({struct _tuple19*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->f1=0,_tmp47C->f2=field_tag_exp;_tmp47C;});_tmp47B[0]=_tmp9C2;}),({struct _tuple19*_tmp9C1=({struct _tuple19*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->f1=0,_tmp47D->f2=fieldexp;_tmp47D;});_tmp47B[1]=_tmp9C1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp47B,sizeof(struct _tuple19*),2));});struct Cyc_List_List*newdles=_tmp479;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_tmp9C4=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp47A[1];({struct _tuple19*_tmp9C3=Cyc_Toc_make_field(field->name,umem);_tmp47A[0]=_tmp9C3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp47A,sizeof(struct _tuple19*),1));}));e->r=_tmp9C4;});}}}else{
# 2652
struct Cyc_List_List*_tmp47E=0;struct Cyc_List_List*newdles=_tmp47E;
struct Cyc_List_List*_tmp47F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd->impl))->fields;struct Cyc_List_List*sdfields=_tmp47F;
for(1;sdfields != 0;sdfields=sdfields->tl){
struct Cyc_List_List*_tmp480=fields;struct Cyc_List_List*fields2=_tmp480;for(0;fields2 != 0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_tmp481=(struct _tuple36*)fields2->hd;struct _tuple36*_stmttmp4A=_tmp481;struct _tuple36*_tmp482=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Aggrfield*_tmp483;_tmp483=_tmp482->f1;_tmp484=_tmp482->f2;{struct Cyc_Absyn_Aggrfield*field=_tmp483;struct Cyc_Absyn_Exp*fieldexp=_tmp484;
void*_tmp485=Cyc_Toc_typ_to_c(field->type);void*fieldtyp=_tmp485;
void*_tmp486=Cyc_Toc_typ_to_c((void*)_check_null(fieldexp->topt));void*fieldexp_typ=_tmp486;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2662
if(!Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2664
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_tmp9C5=fieldtyp;Cyc_Toc_cast_it(_tmp9C5,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_tmp488=_cycalloc(sizeof(*_tmp488));({struct _tuple19*_tmp9C6=({struct _tuple19*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->f1=0,_tmp487->f2=fieldexp;_tmp487;});_tmp488->hd=_tmp9C6;}),_tmp488->tl=newdles;_tmp488;});
break;}}}}
# 2671
({void*_tmp9C7=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(newdles));e->r=_tmp9C7;});}}
# 2674
goto _LL7;}}}}case 31: _tmp306=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp307=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;_tmp308=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;{struct Cyc_List_List*es=_tmp306;struct Cyc_Absyn_Datatypedecl*tud=_tmp307;struct Cyc_Absyn_Datatypefield*tuf=_tmp308;
# 2678
struct Cyc_List_List*_tmp489=tuf->typs;struct Cyc_List_List*tqts=_tmp489;
{struct Cyc_List_List*_tmp48A=es;struct Cyc_List_List*es2=_tmp48A;for(0;es2 != 0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*_tmp48B=(void*)_check_null(cur_e->topt);void*cur_e_typ=_tmp48B;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tqts))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_tmp9C9=({void*_tmp9C8=field_typ;Cyc_Toc_cast_it(_tmp9C8,Cyc_Absyn_copy_exp(cur_e));})->r;cur_e->r=_tmp9C9;});}}{
# 2689
struct Cyc_Absyn_Exp*_tmp48C=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2689
struct Cyc_Absyn_Exp*tag_exp=_tmp48C;
# 2692
if(!Cyc_Toc_is_toplevel(nv)){
# 2694
struct Cyc_List_List*dles=0;
{int i=1;for(0;es != 0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_tmp48D=_cycalloc(sizeof(*_tmp48D));({struct _tuple19*_tmp9CB=({struct _fat_ptr*_tmp9CA=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9CA,(struct Cyc_Absyn_Exp*)es->hd);});_tmp48D->hd=_tmp9CB;}),_tmp48D->tl=dles;_tmp48D;});}}{
# 2700
struct _tuple19*_tmp48E=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);struct _tuple19*tag_dle=_tmp48E;
({void*_tmp9CF=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->tag=29,({struct _tuple1*_tmp9CE=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_tmp490->f1=_tmp9CE;}),_tmp490->f2=0,({
struct Cyc_List_List*_tmp9CD=({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*_tmp48F));_tmp48F->hd=tag_dle,({struct Cyc_List_List*_tmp9CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);_tmp48F->tl=_tmp9CC;});_tmp48F;});_tmp490->f3=_tmp9CD;}),_tmp490->f4=0;_tmp490;});
# 2701
e->r=_tmp9CF;});}}else{
# 2706
struct Cyc_List_List*_tmp491=0;struct Cyc_List_List*dles=_tmp491;
for(1;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));({struct _tuple19*_tmp9D0=({struct _tuple19*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->f1=0,_tmp492->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp492;});_tmp493->hd=_tmp9D0;}),_tmp493->tl=dles;_tmp493;});}
({void*_tmp9D3=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _tuple19*_tmp9D2=({struct _tuple19*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->f1=0,_tmp494->f2=tag_exp;_tmp494;});_tmp495->hd=_tmp9D2;}),({
struct Cyc_List_List*_tmp9D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);_tmp495->tl=_tmp9D1;});_tmp495;}));
# 2709
e->r=_tmp9D3;});}
# 2712
goto _LL7;}}case 32:
# 2714
 goto _LL51;case 33: _LL51:
 goto _LL7;case 34: _tmp300=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).is_calloc;_tmp301=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).rgn;_tmp302=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).elt_type;_tmp303=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).num_elts;_tmp304=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).fat_result;_tmp305=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1).inline_call;{int is_calloc=_tmp300;struct Cyc_Absyn_Exp*rgnopt=_tmp301;void**topt=_tmp302;struct Cyc_Absyn_Exp*num_elts=_tmp303;int is_fat=_tmp304;int inline_call=_tmp305;
# 2718
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(topt)));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2727
if(is_calloc){
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_tmp496[3];_tmp496[0]=rgnopt,({
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp496[1]=_tmp9D4;}),_tmp496[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9D5=Cyc_Toc__region_calloc_e;Cyc_Toc_fncall_exp_dl(_tmp9D5,_tag_fat(_tmp496,sizeof(struct Cyc_Absyn_Exp*),3));});});}else{
# 2733
pexp=({void*_tmp9D7=*topt;struct Cyc_Absyn_Exp*_tmp9D6=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9D7,_tmp9D6,num_elts2);});}}else{
# 2735
if(rgnopt != 0 && !Cyc_Flags_no_regions){
Cyc_Toc_exp_to_c(nv,rgnopt);{
struct Cyc_Absyn_Exp*(*_tmp497)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=inline_call?Cyc_Toc_rmalloc_inline_exp: Cyc_Toc_rmalloc_exp;struct Cyc_Absyn_Exp*(*fn_e)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=_tmp497;
pexp=fn_e(rgnopt,num_elts2);}}else{
# 2740
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);}}
# 2742
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=is_calloc?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_tmp498[3];_tmp498[0]=pexp,_tmp498[1]=elt_sz,_tmp498[2]=num_elts2;({struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Toc__tag_fat_e;Cyc_Toc_fncall_exp_dl(_tmp9D8,_tag_fat(_tmp498,sizeof(struct Cyc_Absyn_Exp*),3));});});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_tmp9DB=(struct _tuple1*)_check_null(x);void*_tmp9DA=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp9D9=num_elts;Cyc_Absyn_declare_stmt(_tmp9DB,_tmp9DA,_tmp9D9,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_tmp9DC=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9DC;});}else{
# 2749
e->r=pexp->r;}
goto _LL7;}}case 35: _tmp2FE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp2FF=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2FE;struct Cyc_Absyn_Exp*e2=_tmp2FF;
# 2759
void*e1_old_typ=(void*)_check_null(e1->topt);
void*e2_old_typ=(void*)_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D4;_tmp7D4.tag=0,({struct _fat_ptr _tmp9DD=({const char*_tmp49B="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_fat(_tmp49B,sizeof(char),57U);});_tmp7D4.f1=_tmp9DD;});_tmp7D4;});void*_tmp499[1];_tmp499[0]=& _tmp49A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp499,sizeof(void*),1));});{
# 2767
unsigned _tmp49C=e->loc;unsigned loc=_tmp49C;
struct _tuple1*_tmp49D=Cyc_Toc_temp_var();struct _tuple1*v1=_tmp49D;
struct Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_var_exp(v1,loc);struct Cyc_Absyn_Exp*v1e=_tmp49E;v1e->topt=e1_old_typ;{
struct _tuple1*_tmp49F=Cyc_Toc_temp_var();struct _tuple1*v2=_tmp49F;
struct Cyc_Absyn_Exp*_tmp4A0=Cyc_Absyn_var_exp(v2,loc);struct Cyc_Absyn_Exp*v2e=_tmp4A0;v2e->topt=e2_old_typ;{
# 2773
struct Cyc_Absyn_Exp*_tmp4A1=({struct Cyc_Absyn_Exp*_tmp9DF=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_tmp9DE=v2e;Cyc_Absyn_assign_exp(_tmp9DF,_tmp9DE,loc);});struct Cyc_Absyn_Exp*s1e=_tmp4A1;s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A2=Cyc_Absyn_exp_stmt(s1e,loc);struct Cyc_Absyn_Stmt*s1=_tmp4A2;
struct Cyc_Absyn_Exp*_tmp4A3=({struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_tmp9E0=v1e;Cyc_Absyn_assign_exp(_tmp9E1,_tmp9E0,loc);});struct Cyc_Absyn_Exp*s2e=_tmp4A3;s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4A4=Cyc_Absyn_exp_stmt(s2e,loc);struct Cyc_Absyn_Stmt*s2=_tmp4A4;
# 2778
struct Cyc_Absyn_Stmt*_tmp4A5=({struct _tuple1*_tmp9E9=v1;void*_tmp9E8=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9E7=e1;struct Cyc_Absyn_Stmt*_tmp9E6=({
struct _tuple1*_tmp9E5=v2;void*_tmp9E4=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9E3=e2;struct Cyc_Absyn_Stmt*_tmp9E2=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2779
Cyc_Absyn_declare_stmt(_tmp9E5,_tmp9E4,_tmp9E3,_tmp9E2,loc);});
# 2778
Cyc_Absyn_declare_stmt(_tmp9E9,_tmp9E8,_tmp9E7,_tmp9E6,loc);});struct Cyc_Absyn_Stmt*s=_tmp4A5;
# 2781
Cyc_Toc_stmt_to_c(nv,s);
({void*_tmp9EA=Cyc_Toc_stmt_exp_r(s);e->r=_tmp9EA;});
goto _LL7;}}}}}}case 38: _tmp2FC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;_tmp2FD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp2F8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2FC;struct _fat_ptr*f=_tmp2FD;
# 2786
void*_tmp4A6=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*e1_typ=_tmp4A6;
Cyc_Toc_exp_to_c(nv,e1);
{void*_tmp4A7=e1_typ;struct Cyc_Absyn_Aggrdecl*_tmp4A8;if(*((int*)_tmp4A7)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A7)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A7)->f1)->f1).KnownAggr).tag == 2){_tmp4A8=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A7)->f1)->f1).KnownAggr).val;{struct Cyc_Absyn_Aggrdecl*ad=_tmp4A8;
# 2790
struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);struct Cyc_Absyn_Exp*f_tag=_tmp4A9;
struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Toc_member_exp(e1,f,0U);struct Cyc_Absyn_Exp*e1_f=_tmp4AA;
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);struct Cyc_Absyn_Exp*e1_f_tag=_tmp4AB;
({void*_tmp9EB=(Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U))->r;e->r=_tmp9EB;});
goto _LL11B;}}else{goto _LL11E;}}else{goto _LL11E;}}else{_LL11E:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D5;_tmp7D5.tag=0,({struct _fat_ptr _tmp9EC=({const char*_tmp4AE="non-aggregate type in tagcheck";_tag_fat(_tmp4AE,sizeof(char),31U);});_tmp7D5.f1=_tmp9EC;});_tmp7D5;});void*_tmp4AC[1];_tmp4AC[0]=& _tmp4AD;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp4AC,sizeof(void*),1));});}_LL11B:;}
# 2797
goto _LL7;}case 37: _tmp2FB=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp2F8)->f1;{struct Cyc_Absyn_Stmt*s=_tmp2FB;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 36:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D6;_tmp7D6.tag=0,({struct _fat_ptr _tmp9ED=({const char*_tmp4B1="UnresolvedMem";_tag_fat(_tmp4B1,sizeof(char),14U);});_tmp7D6.f1=_tmp9ED;});_tmp7D6;});void*_tmp4AF[1];_tmp4AF[0]=& _tmp4B0;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp4AF,sizeof(void*),1));});case 25:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D7;_tmp7D7.tag=0,({struct _fat_ptr _tmp9EE=({const char*_tmp4B4="compoundlit";_tag_fat(_tmp4B4,sizeof(char),12U);});_tmp7D7.f1=_tmp9EE;});_tmp7D7;});void*_tmp4B2[1];_tmp4B2[0]=& _tmp4B3;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp4B2,sizeof(void*),1));});case 39:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D8;_tmp7D8.tag=0,({struct _fat_ptr _tmp9EF=({const char*_tmp4B7="valueof(-)";_tag_fat(_tmp4B7,sizeof(char),11U);});_tmp7D8.f1=_tmp9EF;});_tmp7D8;});void*_tmp4B5[1];_tmp4B5[0]=& _tmp4B6;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp4B5,sizeof(void*),1));});default: _tmp2F9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2F8)->f3;_tmp2FA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp2F8)->f4;{struct Cyc_List_List*o=_tmp2F9;struct Cyc_List_List*i=_tmp2FA;
# 2803
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2807
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DA;_tmp7DA.tag=0,({struct _fat_ptr _tmp9F0=({const char*_tmp4BB="Toc did not examine an inserted check: ";_tag_fat(_tmp4BB,sizeof(char),40U);});_tmp7DA.f1=_tmp9F0;});_tmp7DA;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4BA=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp7D9;_tmp7D9.tag=4,_tmp7D9.f1=e;_tmp7D9;});void*_tmp4B8[2];_tmp4B8[0]=& _tmp4B9,_tmp4B8[1]=& _tmp4BA;({unsigned _tmp9F1=e->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp9F1,_tag_fat(_tmp4B8,sizeof(void*),2));});});}}struct _tuple37{int f1;struct _fat_ptr*f2;struct _fat_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 2836 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2838
return({struct _tuple37*_tmp4BC=_region_malloc(r,sizeof(*_tmp4BC));_tmp4BC->f1=0,({struct _fat_ptr*_tmp9F3=Cyc_Toc_fresh_label();_tmp4BC->f2=_tmp9F3;}),({struct _fat_ptr*_tmp9F2=Cyc_Toc_fresh_label();_tmp4BC->f3=_tmp9F2;}),_tmp4BC->f4=sc;_tmp4BC;});}
# 2844
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(1;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4BD=(struct Cyc_Tcpat_PathNode*)ps->hd;struct Cyc_Tcpat_PathNode*p=_tmp4BD;
# 2850
if((int)(((p->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4C0=p->orig_pat;if((_tmp4C0.pattern).tag != 1)_throw_match();(_tmp4C0.pattern).val;})->topt);
void*_tmp4BE=Cyc_Absyn_compress(Cyc_Toc_typ_to_c_array(t));void*t2=_tmp4BE;
void*_tmp4BF=t2;switch(*((int*)_tmp4BF)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BF)->f1)){case 0:
# 2855
 goto _LL4;case 20: _LL4:
 goto _LL6;default: goto _LL7;}case 7: _LL6:
 goto _LL0;default: _LL7:
 v=({void*_tmp9F4=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp9F4,v);});goto _LL0;}_LL0:;}{
# 2861
void*_tmp4C1=p->access;void*_stmttmp4B=_tmp4C1;void*_tmp4C2=_stmttmp4B;struct _fat_ptr*_tmp4C4;int _tmp4C3;unsigned _tmp4C5;unsigned _tmp4C6;switch(*((int*)_tmp4C2)){case 0:
# 2865
 goto _LL9;case 1:
# 2870
 if(ps->tl != 0){
void*_tmp4C7=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_stmttmp4C=_tmp4C7;void*_tmp4C8=_stmttmp4C;unsigned _tmp4CB;struct Cyc_Absyn_Datatypefield*_tmp4CA;struct Cyc_Absyn_Datatypedecl*_tmp4C9;if(*((int*)_tmp4C8)== 3){_tmp4C9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C8)->f1;_tmp4CA=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C8)->f2;_tmp4CB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C8)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_tmp4C9;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CA;unsigned i=_tmp4CB;
# 2873
ps=ps->tl;
v=({void*_tmp9F6=({void*_tmp9F5=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmp9F5,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9F6,v);});
v=({struct Cyc_Absyn_Exp*_tmp9F7=v;Cyc_Absyn_aggrarrow_exp(_tmp9F7,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});
continue;}}else{
goto _LL14;}_LL14:;}
# 2879
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 3: _tmp4C6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4C2)->f3;{unsigned i=_tmp4C6;
_tmp4C5=i;goto _LL11;}case 2: _tmp4C5=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4C2)->f1;_LL11: {unsigned i=_tmp4C5;
v=({struct Cyc_Absyn_Exp*_tmp9F8=v;Cyc_Toc_member_exp(_tmp9F8,Cyc_Absyn_fieldname((int)(i + (unsigned)1)),0U);});goto _LL9;}default: _tmp4C3=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C2)->f1;_tmp4C4=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4C2)->f2;{int tagged=_tmp4C3;struct _fat_ptr*f=_tmp4C4;
# 2884
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2890
return v;}
# 2895
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4CC=t;struct Cyc_Absyn_Datatypefield*_tmp4CE;struct Cyc_Absyn_Datatypedecl*_tmp4CD;int _tmp4D0;struct _fat_ptr*_tmp4CF;struct Cyc_Absyn_Datatypefield*_tmp4D3;struct Cyc_Absyn_Datatypedecl*_tmp4D2;int _tmp4D1;unsigned _tmp4D4;int _tmp4D6;struct _fat_ptr _tmp4D5;struct Cyc_Absyn_Enumfield*_tmp4D8;void*_tmp4D7;struct Cyc_Absyn_Enumfield*_tmp4DA;struct Cyc_Absyn_Enumdecl*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;switch(*((int*)_tmp4CC)){case 0: _tmp4DB=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4CC)->f1;{struct Cyc_Absyn_Exp*e=_tmp4DB;
return e == 0?v: e;}case 1:
 return({struct Cyc_Absyn_Exp*_tmp9F9=v;Cyc_Absyn_eq_exp(_tmp9F9,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2:
 return({struct Cyc_Absyn_Exp*_tmp9FA=v;Cyc_Absyn_neq_exp(_tmp9FA,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3: _tmp4D9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CC)->f1;_tmp4DA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4CC)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp4D9;struct Cyc_Absyn_Enumfield*ef=_tmp4DA;
return({struct Cyc_Absyn_Exp*_tmp9FB=v;Cyc_Absyn_eq_exp(_tmp9FB,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->tag=32,_tmp4DC->f1=ed,_tmp4DC->f2=ef;_tmp4DC;}),0U),0U);});}case 4: _tmp4D7=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CC)->f1;_tmp4D8=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4CC)->f2;{void*t=_tmp4D7;struct Cyc_Absyn_Enumfield*ef=_tmp4D8;
return({struct Cyc_Absyn_Exp*_tmp9FC=v;Cyc_Absyn_eq_exp(_tmp9FC,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->tag=33,_tmp4DD->f1=t,_tmp4DD->f2=ef;_tmp4DD;}),0U),0U);});}case 5: _tmp4D5=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4CC)->f1;_tmp4D6=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4CC)->f2;{struct _fat_ptr s=_tmp4D5;int i=_tmp4D6;
return({struct Cyc_Absyn_Exp*_tmp9FD=v;Cyc_Absyn_eq_exp(_tmp9FD,Cyc_Absyn_float_exp(s,i,0U),0U);});}case 6: _tmp4D4=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4CC)->f1;{unsigned i=_tmp4D4;
return({struct Cyc_Absyn_Exp*_tmp9FE=v;Cyc_Absyn_eq_exp(_tmp9FE,Cyc_Absyn_signed_int_exp((int)i,0U),0U);});}case 7: _tmp4D1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CC)->f1;_tmp4D2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CC)->f2;_tmp4D3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4CC)->f3;{int i=_tmp4D1;struct Cyc_Absyn_Datatypedecl*tud=_tmp4D2;struct Cyc_Absyn_Datatypefield*tuf=_tmp4D3;
# 2907
LOOP1: {
void*_tmp4DE=v->r;void*_stmttmp4D=_tmp4DE;void*_tmp4DF=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;switch(*((int*)_tmp4DF)){case 14: _tmp4E1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4DF)->f2;{struct Cyc_Absyn_Exp*e=_tmp4E1;
v=e;goto LOOP1;}case 20: _tmp4E0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4DF)->f1;{struct Cyc_Absyn_Exp*e=_tmp4E0;
v=e;goto _LL15;}default:
 goto _LL15;}_LL15:;}
# 2917
v=({void*_tmp9FF=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmp9FF,v);});
return({struct Cyc_Absyn_Exp*_tmpA00=Cyc_Absyn_deref_exp(v,0U);Cyc_Absyn_eq_exp(_tmpA00,Cyc_Absyn_signed_int_exp(i,0U),0U);});}case 8: _tmp4CF=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CC)->f1;_tmp4D0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4CC)->f2;{struct _fat_ptr*f=_tmp4CF;int i=_tmp4D0;
# 2920
return({struct Cyc_Absyn_Exp*_tmpA02=({struct Cyc_Absyn_Exp*_tmpA01=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_tmpA01,Cyc_Toc_tag_sp,0U);});Cyc_Absyn_eq_exp(_tmpA02,
Cyc_Absyn_signed_int_exp(i,0U),0U);});}default: _tmp4CD=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4CC)->f1;_tmp4CE=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4CC)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_tmp4CD;struct Cyc_Absyn_Datatypefield*tuf=_tmp4CE;
# 2924
LOOP2: {
void*_tmp4E2=v->r;void*_stmttmp4E=_tmp4E2;void*_tmp4E3=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;switch(*((int*)_tmp4E3)){case 14: _tmp4E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E3)->f2;{struct Cyc_Absyn_Exp*e=_tmp4E5;
v=e;goto LOOP2;}case 20: _tmp4E4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4E3)->f1;{struct Cyc_Absyn_Exp*e=_tmp4E4;
v=e;goto _LL1C;}default:
 goto _LL1C;}_LL1C:;}
# 2931
v=({void*_tmpA04=({void*_tmpA03=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_tmpA03,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA04,v);});
return({struct Cyc_Absyn_Exp*_tmpA05=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA05,Cyc_Absyn_var_exp(tuf->name,0U),0U);});}};}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2943
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple38{int f1;void*f2;};
# 2950
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2952
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(1;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _tmp4E6=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp4F=_tmp4E6;struct _tuple34 _tmp4E7=_stmttmp4F;void*_tmp4E8;_tmp4E8=_tmp4E7.f1;{void*ptest=_tmp4E8;
void*_tmp4E9=ptest;struct Cyc_Absyn_Exp*_tmp4EA;struct _fat_ptr*_tmp4EB;switch(*((int*)_tmp4E9)){case 3:
# 2958
 goto _LL7;case 4: _LL7:
 goto _LL9;case 6: _LL9:
 continue;case 8: _tmp4EB=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4E9)->f1;{struct _fat_ptr*f=_tmp4EB;
# 2962
if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->tag=3,_tmp4EC->f1=f;_tmp4EC;});
continue;}case 7:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0: _tmp4EA=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4E9)->f1;if(_tmp4EA != 0){struct Cyc_Absyn_Exp*e=_tmp4EA;
# 2967
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->tag=2,_tmp4ED->f1=e;_tmp4ED;});return({struct _tuple38 _tmp7DB;_tmp7DB.f1=0,_tmp7DB.f2=k;_tmp7DB;});}else{
goto _LL13;}case 1: _LL13:
 goto _LL15;case 2: _LL15:
 goto _LL17;case 5: _LL17:
 goto _LL19;default: _LL19:
 return({struct _tuple38 _tmp7DC;_tmp7DC.f1=0,_tmp7DC.f2=k;_tmp7DC;});};}}
# 2975
return({struct _tuple38 _tmp7DD;_tmp7DD.f1=c,_tmp7DD.f2=k;_tmp7DD;});}
# 2980
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4EE=p;int _tmp4EF;int _tmp4F0;unsigned _tmp4F1;struct Cyc_Absyn_Enumfield*_tmp4F3;void*_tmp4F2;struct Cyc_Absyn_Enumfield*_tmp4F5;struct Cyc_Absyn_Enumdecl*_tmp4F4;switch(*((int*)_tmp4EE)){case 3: _tmp4F4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4EE)->f1;_tmp4F5=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4EE)->f2;{struct Cyc_Absyn_Enumdecl*ed=_tmp4F4;struct Cyc_Absyn_Enumfield*ef=_tmp4F5;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->tag=32,_tmp4F6->f1=ed,_tmp4F6->f2=ef;_tmp4F6;}),0U);goto _LL0;}case 4: _tmp4F2=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4EE)->f1;_tmp4F3=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4EE)->f2;{void*t=_tmp4F2;struct Cyc_Absyn_Enumfield*ef=_tmp4F3;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->tag=33,_tmp4F7->f1=t,_tmp4F7->f2=ef;_tmp4F7;}),0U);goto _LL0;}case 6: _tmp4F1=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4EE)->f1;{unsigned i=_tmp4F1;
_tmp4F0=(int)i;goto _LL8;}case 7: _tmp4F0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4EE)->f1;_LL8: {int i=_tmp4F0;
_tmp4EF=i;goto _LLA;}case 8: _tmp4EF=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4EE)->f2;_LLA: {int i=_tmp4EF;
e=Cyc_Absyn_signed_int_exp(i,0U);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DE;_tmp7DE.tag=0,({struct _fat_ptr _tmpA06=({const char*_tmp4FA="compile_pat_test_as_case!";_tag_fat(_tmp4FA,sizeof(char),26U);});_tmp7DE.f1=_tmpA06;});_tmp7DE;});void*_tmp4F8[1];_tmp4F8[0]=& _tmp4F9;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp4F8,sizeof(void*),1));});}_LL0:;}
# 2990
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->tag=17,_tmp4FB->f1=e;_tmp4FB;}),0U);}
# 2994
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};struct _tuple40{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3005
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3008
void*t=(void*)_check_null(p->topt);
void*_tmp4FC=p->r;void*_stmttmp50=_tmp4FC;void*_tmp4FD=_stmttmp50;struct Cyc_List_List*_tmp4FF;union Cyc_Absyn_AggrInfo _tmp4FE;struct Cyc_List_List*_tmp500;struct Cyc_List_List*_tmp501;struct Cyc_Absyn_Pat*_tmp502;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Datatypefield*_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp503;struct Cyc_Absyn_Pat*_tmp507;struct Cyc_Absyn_Vardecl*_tmp506;struct Cyc_Absyn_Vardecl*_tmp508;struct Cyc_Absyn_Pat*_tmp50A;struct Cyc_Absyn_Vardecl*_tmp509;struct Cyc_Absyn_Vardecl*_tmp50B;switch(*((int*)_tmp4FD)){case 2: _tmp50B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4FD)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp50B;
# 3011
struct Cyc_Absyn_Pat*_tmp50C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);struct Cyc_Absyn_Pat*p2=_tmp50C;
p2->topt=p->topt;
_tmp509=vd;_tmp50A=p2;goto _LL4;}case 1: _tmp509=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1;_tmp50A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4FD)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp509;struct Cyc_Absyn_Pat*p2=_tmp50A;
# 3016
struct Cyc_Absyn_Vardecl*_tmp50D=({struct _tuple1*_tmpA07=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA07,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp50D;
({struct Cyc_List_List*_tmpA09=({struct Cyc_List_List*_tmp50F=_region_malloc(rgn,sizeof(*_tmp50F));({struct _tuple39*_tmpA08=({struct _tuple39*_tmp50E=_region_malloc(rgn,sizeof(*_tmp50E));_tmp50E->f1=vd,_tmp50E->f2=new_vd;_tmp50E;});_tmp50F->hd=_tmpA08;}),_tmp50F->tl=*decls;_tmp50F;});*decls=_tmpA09;});{
struct Cyc_Absyn_Stmt*_tmp510=({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->tag=4,_tmp511->f1=new_vd;_tmp511;}),0U);Cyc_Absyn_assign_stmt(_tmpA0A,Cyc_Absyn_copy_exp(path),0U);});struct Cyc_Absyn_Stmt*s=_tmp510;
return({struct Cyc_Absyn_Stmt*_tmpA0B=s;Cyc_Toc_seq_stmt_opt(_tmpA0B,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}case 4: _tmp508=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4FD)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp508;
# 3022
struct Cyc_Absyn_Vardecl*_tmp512=({struct _tuple1*_tmpA0C=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA0C,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp512;
({struct Cyc_List_List*_tmpA0E=({struct Cyc_List_List*_tmp514=_region_malloc(rgn,sizeof(*_tmp514));({struct _tuple39*_tmpA0D=({struct _tuple39*_tmp513=_region_malloc(rgn,sizeof(*_tmp513));_tmp513->f1=vd,_tmp513->f2=new_vd;_tmp513;});_tmp514->hd=_tmpA0D;}),_tmp514->tl=*decls;_tmp514;});*decls=_tmpA0E;});
return({struct Cyc_Absyn_Exp*_tmpA0F=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->tag=4,_tmp515->f1=new_vd;_tmp515;}),0U);Cyc_Absyn_assign_stmt(_tmpA0F,Cyc_Absyn_copy_exp(path),0U);});}case 0:
 return 0;case 3: _tmp506=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1;_tmp507=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4FD)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp506;struct Cyc_Absyn_Pat*p2=_tmp507;
# 3028
({void*_tmpA10=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_tmpA10;});{
struct Cyc_Absyn_Vardecl*_tmp516=({struct _tuple1*_tmpA11=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_tmpA11,Cyc_Toc_typ_to_c_array(vd->type),0);});struct Cyc_Absyn_Vardecl*new_vd=_tmp516;
({struct Cyc_List_List*_tmpA13=({struct Cyc_List_List*_tmp518=_region_malloc(rgn,sizeof(*_tmp518));({struct _tuple39*_tmpA12=({struct _tuple39*_tmp517=_region_malloc(rgn,sizeof(*_tmp517));_tmp517->f1=vd,_tmp517->f2=new_vd;_tmp517;});_tmp518->hd=_tmpA12;}),_tmp518->tl=*decls;_tmp518;});*decls=_tmpA13;});{
# 3032
struct Cyc_Absyn_Stmt*_tmp519=({struct Cyc_Absyn_Exp*_tmpA16=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->tag=4,_tmp51A->f1=new_vd;_tmp51A;}),0U);Cyc_Absyn_assign_stmt(_tmpA16,({
void*_tmpA15=({void*_tmpA14=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmpA14,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA15,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
# 3032
struct Cyc_Absyn_Stmt*s=_tmp519;
# 3035
return({struct Cyc_Absyn_Stmt*_tmpA17=s;Cyc_Toc_seq_stmt_opt(_tmpA17,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,p2));});}}}case 9:
# 3037
 goto _LLE;case 10: _LLE:
 goto _LL10;case 11: _LL10:
 goto _LL12;case 12: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 14: _LL16:
 return 0;case 6: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1)->r)== 8){_tmp503=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1)->r)->f1;_tmp504=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1)->r)->f2;_tmp505=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1)->r)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_tmp503;struct Cyc_Absyn_Datatypefield*tuf=_tmp504;struct Cyc_List_List*ps=_tmp505;
# 3046
if(ps == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*_tmp51B=({void*_tmpA18=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmpA18,Cyc_Toc_mt_tq);});void*field_ptr_typ=_tmp51B;
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*_tmp51C=tuf->typs;struct Cyc_List_List*tuf_tqts=_tmp51C;
struct Cyc_Absyn_Stmt*s=0;
for(1;ps != 0;(ps=ps->tl,tuf_tqts=((struct Cyc_List_List*)_check_null(tuf_tqts))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp51D=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp51D;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp51E=(*((struct _tuple23*)((struct Cyc_List_List*)_check_null(tuf_tqts))->hd)).f2;void*tuf_typ=_tmp51E;
void*_tmp51F=(void*)_check_null(p2->topt);void*t2=_tmp51F;
void*_tmp520=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp520;
struct Cyc_Absyn_Exp*_tmp521=({struct Cyc_Absyn_Exp*_tmpA19=path;Cyc_Absyn_aggrarrow_exp(_tmpA19,Cyc_Absyn_fieldname(cnt),0U);});struct Cyc_Absyn_Exp*arrow_exp=_tmp521;
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_tmpA1A=s;Cyc_Toc_seq_stmt_opt(_tmpA1A,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,arrow_exp,p2));});}}
# 3064
return s;}}}}else{_tmp502=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1;{struct Cyc_Absyn_Pat*p2=_tmp502;
# 3109
return({struct _RegionHandle*_tmpA1E=rgn;struct Cyc_Toc_Env**_tmpA1D=nv;struct Cyc_List_List**_tmpA1C=decls;struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA1E,_tmpA1D,_tmpA1C,_tmpA1B,p2);});}}case 8: _tmp501=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4FD)->f3;{struct Cyc_List_List*ps=_tmp501;
# 3066
_tmp500=ps;goto _LL1C;}case 5: _tmp500=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1;_LL1C: {struct Cyc_List_List*ps=_tmp500;
# 3068
struct Cyc_Absyn_Stmt*s=0;
{int cnt=1;for(0;ps != 0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp522=(struct Cyc_Absyn_Pat*)ps->hd;struct Cyc_Absyn_Pat*p2=_tmp522;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp523=(void*)_check_null(p2->topt);void*t2=_tmp523;
struct _fat_ptr*_tmp524=Cyc_Absyn_fieldname(cnt);struct _fat_ptr*f=_tmp524;
s=({struct Cyc_Absyn_Stmt*_tmpA23=s;Cyc_Toc_seq_stmt_opt(_tmpA23,({struct _RegionHandle*_tmpA22=rgn;struct Cyc_Toc_Env**_tmpA21=nv;struct Cyc_List_List**_tmpA20=decls;struct Cyc_Absyn_Exp*_tmpA1F=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_tmpA22,_tmpA21,_tmpA20,_tmpA1F,p2);}));});}}}
# 3077
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp526=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7DF;_tmp7DF.tag=0,({struct _fat_ptr _tmpA24=({const char*_tmp527="unresolved aggregate pattern!";_tag_fat(_tmp527,sizeof(char),30U);});_tmp7DF.f1=_tmpA24;});_tmp7DF;});void*_tmp525[1];_tmp525[0]=& _tmp526;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp525,sizeof(void*),1));});else{_tmp4FE=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4FD)->f1;_tmp4FF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4FD)->f3;{union Cyc_Absyn_AggrInfo info=_tmp4FE;struct Cyc_List_List*dlps=_tmp4FF;
# 3081
struct Cyc_Absyn_Aggrdecl*_tmp528=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp528;
struct Cyc_Absyn_Stmt*s=0;
for(1;dlps != 0;dlps=dlps->tl){
struct _tuple40*_tmp529=(struct _tuple40*)dlps->hd;struct _tuple40*tup=_tmp529;
struct Cyc_Absyn_Pat*_tmp52A=(*tup).f2;struct Cyc_Absyn_Pat*p2=_tmp52A;
if(p2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*_tmp52B=Cyc_Absyn_designatorlist_to_fieldname((*tup).f1);struct _fat_ptr*f=_tmp52B;
void*_tmp52C=(void*)_check_null(p2->topt);void*t2=_tmp52C;
void*_tmp52D=Cyc_Toc_typ_to_c_array(t2);void*t2c=_tmp52D;
struct Cyc_Absyn_Exp*_tmp52E=Cyc_Toc_member_exp(path,f,0U);struct Cyc_Absyn_Exp*memexp=_tmp52E;
# 3093
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*_tmp52F=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,f)))->type;void*ftype=_tmp52F;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(ftype)))
# 3101
memexp=Cyc_Absyn_deref_exp(({void*_tmpA25=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA25,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3104
s=({struct Cyc_Absyn_Stmt*_tmpA26=s;Cyc_Toc_seq_stmt_opt(_tmpA26,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,memexp,p2));});}}}
# 3106
return s;}}case 15:
# 3111
({struct Cyc_Warn_String_Warn_Warg_struct _tmp531=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E0;_tmp7E0.tag=0,({struct _fat_ptr _tmpA27=({const char*_tmp532="unknownid pat";_tag_fat(_tmp532,sizeof(char),14U);});_tmp7E0.f1=_tmpA27;});_tmp7E0;});void*_tmp530[1];_tmp530[0]=& _tmp531;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp530,sizeof(void*),1));});case 16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp534=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E1;_tmp7E1.tag=0,({struct _fat_ptr _tmpA28=({const char*_tmp535="unknowncall pat";_tag_fat(_tmp535,sizeof(char),16U);});_tmp7E1.f1=_tmpA28;});_tmp7E1;});void*_tmp533[1];_tmp533[0]=& _tmp534;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp533,sizeof(void*),1));});default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp537=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E2;_tmp7E2.tag=0,({struct _fat_ptr _tmpA29=({const char*_tmp538="exp pat";_tag_fat(_tmp538,sizeof(char),8U);});_tmp7E2.f1=_tmpA29;});_tmp7E2;});void*_tmp536[1];_tmp536[0]=& _tmp537;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp536,sizeof(void*),1));});};}
# 3120
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_tmp539=e->r;void*_stmttmp51=_tmp539;void*_tmp53A=_stmttmp51;struct Cyc_List_List*_tmp53B;struct Cyc_List_List*_tmp53C;struct Cyc_List_List*_tmp53D;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp540;struct Cyc_List_List*_tmp541;struct Cyc_List_List*_tmp542;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp546;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp547;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Vardecl*_tmp560;struct Cyc_Absyn_Vardecl*_tmp561;switch(*((int*)_tmp53A)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53A)->f1)){case 5: _tmp561=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53A)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp561;
_tmp560=vd;goto _LL4;}case 4: _tmp560=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53A)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp560;
# 3124
{struct _handler_cons _tmp562;_push_handler(& _tmp562);{int _tmp564=0;if(setjmp(_tmp562.handler))_tmp564=1;if(!_tmp564){({void*_tmpA2C=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566->tag=1,({void*_tmpA2B=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->tag=4,({struct Cyc_Absyn_Vardecl*_tmpA2A=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc)(env,vd);_tmp565->f1=_tmpA2A;});_tmp565;});_tmp566->f1=_tmpA2B;});_tmp566;});e->r=_tmpA2C;});;_pop_handler();}else{void*_tmp563=(void*)Cyc_Core_get_exn_thrown();void*_tmp567=_tmp563;void*_tmp568;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp567)->tag == Cyc_Core_Not_found)
goto _LL3F;else{_tmp568=_tmp567;{void*exn=_tmp568;(int)_rethrow(exn);}}_LL3F:;}}}
goto _LL0;}default: goto _LL3D;}case 6: _tmp55D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp55E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_tmp55F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53A)->f3;{struct Cyc_Absyn_Exp*e1=_tmp55D;struct Cyc_Absyn_Exp*e2=_tmp55E;struct Cyc_Absyn_Exp*e3=_tmp55F;
# 3128
Cyc_Toc_subst_pattern_vars(env,e1);_tmp55B=e2;_tmp55C=e3;goto _LL8;}case 27: _tmp55B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_tmp55C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp53A)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp55B;struct Cyc_Absyn_Exp*e2=_tmp55C;
_tmp559=e1;_tmp55A=e2;goto _LLA;}case 7: _tmp559=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp55A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp559;struct Cyc_Absyn_Exp*e2=_tmp55A;
_tmp557=e1;_tmp558=e2;goto _LLC;}case 8: _tmp557=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp558=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp557;struct Cyc_Absyn_Exp*e2=_tmp558;
_tmp555=e1;_tmp556=e2;goto _LLE;}case 23: _tmp555=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp556=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp555;struct Cyc_Absyn_Exp*e2=_tmp556;
_tmp553=e1;_tmp554=e2;goto _LL10;}case 9: _tmp553=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp554=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp553;struct Cyc_Absyn_Exp*e2=_tmp554;
# 3134
Cyc_Toc_subst_pattern_vars(env,e1);_tmp552=e2;goto _LL12;}case 41: _tmp552=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp552;
_tmp551=e;goto _LL14;}case 38: _tmp551=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp551;
_tmp550=e;goto _LL16;}case 11: _tmp550=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp550;
_tmp54F=e;goto _LL18;}case 12: _tmp54F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp54F;
_tmp54E=e;goto _LL1A;}case 13: _tmp54E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp54E;
_tmp54D=e;goto _LL1C;}case 14: _tmp54D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp54D;
_tmp54C=e;goto _LL1E;}case 18: _tmp54C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp54C;
_tmp54B=e;goto _LL20;}case 20: _tmp54B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp54B;
_tmp54A=e;goto _LL22;}case 21: _tmp54A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp54A;
_tmp549=e;goto _LL24;}case 22: _tmp549=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp549;
_tmp548=e;goto _LL26;}case 28: _tmp548=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL26: {struct Cyc_Absyn_Exp*e=_tmp548;
_tmp547=e;goto _LL28;}case 15: _tmp547=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL28: {struct Cyc_Absyn_Exp*e=_tmp547;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 34: _tmp545=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp53A)->f1).rgn;_tmp546=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp53A)->f1).num_elts;{struct Cyc_Absyn_Exp*eopt=_tmp545;struct Cyc_Absyn_Exp*e=_tmp546;
_tmp543=eopt;_tmp544=e;goto _LL2C;}case 16: _tmp543=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_tmp544=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LL2C: {struct Cyc_Absyn_Exp*eopt=_tmp543;struct Cyc_Absyn_Exp*e=_tmp544;
# 3149
if(eopt != 0)Cyc_Toc_subst_pattern_vars(env,eopt);
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 3: _tmp542=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;{struct Cyc_List_List*es=_tmp542;
_tmp541=es;goto _LL30;}case 31: _tmp541=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL30: {struct Cyc_List_List*es=_tmp541;
_tmp540=es;goto _LL32;}case 24: _tmp540=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL32: {struct Cyc_List_List*es=_tmp540;
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 36: _tmp53F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;{struct Cyc_List_List*dles=_tmp53F;
_tmp53E=dles;goto _LL36;}case 29: _tmp53E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp53A)->f3;_LL36: {struct Cyc_List_List*dles=_tmp53E;
_tmp53D=dles;goto _LL38;}case 30: _tmp53D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LL38: {struct Cyc_List_List*dles=_tmp53D;
_tmp53C=dles;goto _LL3A;}case 26: _tmp53C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp53A)->f1;_LL3A: {struct Cyc_List_List*dles=_tmp53C;
_tmp53B=dles;goto _LL3C;}case 25: _tmp53B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp53A)->f2;_LL3C: {struct Cyc_List_List*dles=_tmp53B;
# 3159
for(1;dles != 0;dles=dles->tl){
Cyc_Toc_subst_pattern_vars(env,(*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}default: _LL3D:
 goto _LL0;}_LL0:;}struct _tuple41{struct Cyc_Toc_Env*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3171
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3179
void*_tmp569=dopt;void*_tmp56C;struct Cyc_List_List*_tmp56B;struct Cyc_List_List*_tmp56A;struct Cyc_Tcpat_Rhs*_tmp56D;if(_tmp569 == 0)
# 3181
return Cyc_Absyn_skip_stmt(0U);else{switch(*((int*)_tmp569)){case 0:
 return Cyc_Toc_throw_match_stmt();case 1: _tmp56D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp569)->f1;{struct Cyc_Tcpat_Rhs*rhs=_tmp56D;
# 3185
for(1;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp56E=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp52=_tmp56E;struct _tuple37*_tmp56F=_stmttmp52;struct Cyc_Absyn_Switch_clause*_tmp573;struct _fat_ptr*_tmp572;struct _fat_ptr*_tmp571;int*_tmp570;_tmp570=(int*)& _tmp56F->f1;_tmp571=_tmp56F->f2;_tmp572=_tmp56F->f3;_tmp573=_tmp56F->f4;{int*already_emitted=_tmp570;struct _fat_ptr*init_lab=_tmp571;struct _fat_ptr*code_lab=_tmp572;struct Cyc_Absyn_Switch_clause*sc=_tmp573;
struct Cyc_Absyn_Stmt*_tmp574=sc->body;struct Cyc_Absyn_Stmt*body=_tmp574;
if(body == rhs->rhs){
# 3190
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3195
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA2E=rgn;struct Cyc_Absyn_Exp*_tmpA2D=
Cyc_Absyn_var_exp(v,0U);
# 3195
Cyc_Toc_extract_pattern_vars(_tmpA2E,& nv,& newdecls,_tmpA2D,sc->pattern);});
# 3199
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*_tmp575=newdecls;struct Cyc_List_List*ds=_tmp575;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp576=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp53=_tmp576;struct _tuple39 _tmp577=_stmttmp53;struct Cyc_Absyn_Vardecl*_tmp579;struct Cyc_Absyn_Vardecl*_tmp578;_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;{struct Cyc_Absyn_Vardecl*oldv=_tmp578;struct Cyc_Absyn_Vardecl*newv=_tmp579;
({struct Cyc_List_List*_tmpA30=({struct Cyc_List_List*_tmp57B=_region_malloc(rgn,sizeof(*_tmp57B));({struct _tuple39*_tmpA2F=({struct _tuple39*_tmp57A=_region_malloc(rgn,sizeof(*_tmp57A));_tmp57A->f1=oldv,_tmp57A->f2=newv;_tmp57A;});_tmp57B->hd=_tmpA2F;}),_tmp57B->tl=*decls;_tmp57B;});*decls=_tmpA30;});
({struct Cyc_Absyn_Exp*_tmpA31=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->tag=4,_tmp57C->f1=newv;_tmp57C;}),0U);oldv->initializer=_tmpA31;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
res=({struct Cyc_Absyn_Decl*_tmpA33=({struct Cyc_Absyn_Decl*_tmp57E=_cycalloc(sizeof(*_tmp57E));({void*_tmpA32=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=0,_tmp57D->f1=oldv;_tmp57D;});_tmp57E->r=_tmpA32;}),_tmp57E->loc=0U;_tmp57E;});Cyc_Absyn_decl_stmt(_tmpA33,res,0U);});}}}
# 3208
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_tmpA35=({struct Cyc_List_List*_tmp580=_region_malloc(rgn,sizeof(*_tmp580));({struct _tuple41*_tmpA34=({struct _tuple41*_tmp57F=_region_malloc(rgn,sizeof(*_tmp57F));_tmp57F->f1=nv,_tmp57F->f2=code_lab,_tmp57F->f3=body;_tmp57F;});_tmp580->hd=_tmpA34;}),_tmp580->tl=*bodies;_tmp580;});*bodies=_tmpA35;});
return res;}}}}
# 3216
({struct Cyc_Warn_String_Warn_Warg_struct _tmp582=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E3;_tmp7E3.tag=0,({struct _fat_ptr _tmpA36=({const char*_tmp583="couldn't find rhs!";_tag_fat(_tmp583,sizeof(char),19U);});_tmp7E3.f1=_tmpA36;});_tmp7E3;});void*_tmp581[1];_tmp581[0]=& _tmp582;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp581,sizeof(void*),1));});}default: _tmp56A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp569)->f1;_tmp56B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp569)->f2;_tmp56C=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp569)->f3;{struct Cyc_List_List*symbolic_path=_tmp56A;struct Cyc_List_List*switches=_tmp56B;void*other_decision=_tmp56C;
# 3219
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3221
struct Cyc_Absyn_Exp*_tmp584=({struct Cyc_List_List*_tmpA37=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(symbolic_path);Cyc_Toc_compile_path(_tmpA37,Cyc_Absyn_var_exp(v,0U));});struct Cyc_Absyn_Exp*p=_tmp584;
struct Cyc_List_List*_tmp585=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(switches);struct Cyc_List_List*ss=_tmp585;
# 3224
struct _tuple38 _tmp586=Cyc_Toc_admits_switch(ss);struct _tuple38 _stmttmp54=_tmp586;struct _tuple38 _tmp587=_stmttmp54;void*_tmp589;int _tmp588;_tmp588=_tmp587.f1;_tmp589=_tmp587.f2;{int allows_switch=_tmp588;void*test_kind=_tmp589;
if(allows_switch > 1){
# 3228
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));
({struct Cyc_Absyn_Switch_clause*_tmpA39=({struct Cyc_Absyn_Switch_clause*_tmp599=_cycalloc(sizeof(*_tmp599));({struct Cyc_Absyn_Pat*_tmpA38=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp599->pattern=_tmpA38;}),_tmp599->pat_vars=0,_tmp599->where_clause=0,_tmp599->body=res,_tmp599->loc=0U;_tmp599;});_tmp59A->hd=_tmpA39;}),_tmp59A->tl=0;_tmp59A;});
# 3231
for(1;ss != 0;ss=ss->tl){
struct _tuple34 _tmp58A=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp55=_tmp58A;struct _tuple34 _tmp58B=_stmttmp55;void*_tmp58D;void*_tmp58C;_tmp58C=_tmp58B.f1;_tmp58D=_tmp58B.f2;{void*pat_test=_tmp58C;void*dec_tree=_tmp58D;
# 3234
struct Cyc_Absyn_Pat*_tmp58E=Cyc_Toc_compile_pat_test_as_case(pat_test);struct Cyc_Absyn_Pat*case_exp=_tmp58E;
# 3236
struct Cyc_Absyn_Stmt*_tmp58F=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp58F;
# 3238
new_lscs=({struct Cyc_List_List*_tmp591=_cycalloc(sizeof(*_tmp591));({struct Cyc_Absyn_Switch_clause*_tmpA3A=({struct Cyc_Absyn_Switch_clause*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->pattern=case_exp,_tmp590->pat_vars=0,_tmp590->where_clause=0,_tmp590->body=s,_tmp590->loc=0U;_tmp590;});_tmp591->hd=_tmpA3A;}),_tmp591->tl=new_lscs;_tmp591;});}}
# 3240
{void*_tmp592=test_kind;struct _fat_ptr*_tmp593;switch(*((int*)_tmp592)){case 1:
# 3242
 LOOP1: {
void*_tmp594=p->r;void*_stmttmp56=_tmp594;void*_tmp595=_stmttmp56;struct Cyc_Absyn_Exp*_tmp596;struct Cyc_Absyn_Exp*_tmp597;switch(*((int*)_tmp595)){case 14: _tmp597=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp595)->f2;{struct Cyc_Absyn_Exp*e=_tmp597;
p=e;goto LOOP1;}case 20: _tmp596=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp595)->f1;{struct Cyc_Absyn_Exp*e=_tmp596;
p=e;goto _LL1E;}default:
 goto _LL1E;}_LL1E:;}
# 3248
p=Cyc_Absyn_deref_exp(({void*_tmpA3B=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA3B,p);}),0U);goto _LL15;case 3: _tmp593=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp592)->f1;{struct _fat_ptr*f=_tmp593;
# 3250
p=({struct Cyc_Absyn_Exp*_tmpA3C=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_tmpA3C,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2:
 goto _LL1D;default: _LL1D:
 goto _LL15;}_LL15:;}
# 3254
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->tag=10,_tmp598->f1=p,_tmp598->f2=new_lscs,_tmp598->f3=0;_tmp598;}),0U);}else{
# 3258
void*_tmp59B=test_kind;struct Cyc_Absyn_Exp*_tmp59C;if(*((int*)_tmp59B)== 2){_tmp59C=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp59B)->f1;{struct Cyc_Absyn_Exp*e=_tmp59C;
# 3260
struct Cyc_List_List*_tmp59D=ss;struct Cyc_Tcpat_Rhs*_tmp59F;void*_tmp59E;if(_tmp59D != 0){if(*((int*)((struct _tuple34*)((struct Cyc_List_List*)_tmp59D)->hd)->f2)== 1){if(((struct Cyc_List_List*)_tmp59D)->tl == 0){_tmp59E=((struct _tuple34*)_tmp59D->hd)->f1;_tmp59F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)_tmp59D->hd)->f2)->f1;{void*pat_test=_tmp59E;struct Cyc_Tcpat_Rhs*rhs=_tmp59F;
# 3266
for(1;lscs != 0;lscs=lscs->tl){
struct _tuple37*_tmp5A0=(struct _tuple37*)lscs->hd;struct _tuple37*_stmttmp57=_tmp5A0;struct _tuple37*_tmp5A1=_stmttmp57;struct Cyc_Absyn_Switch_clause*_tmp5A5;struct _fat_ptr*_tmp5A4;struct _fat_ptr*_tmp5A3;int*_tmp5A2;_tmp5A2=(int*)& _tmp5A1->f1;_tmp5A3=_tmp5A1->f2;_tmp5A4=_tmp5A1->f3;_tmp5A5=_tmp5A1->f4;{int*already_emitted=_tmp5A2;struct _fat_ptr*init_lab=_tmp5A3;struct _fat_ptr*code_lab=_tmp5A4;struct Cyc_Absyn_Switch_clause*sc=_tmp5A5;
struct Cyc_Absyn_Stmt*_tmp5A6=sc->body;struct Cyc_Absyn_Stmt*body=_tmp5A6;
if(body == rhs->rhs){
# 3271
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3276
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA3E=rgn;struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA3E,& nv,& newdecls,_tmpA3D,sc->pattern);});
# 3280
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*_tmp5A7=newdecls;struct Cyc_List_List*ds=_tmp5A7;for(0;ds != 0;ds=ds->tl){
struct _tuple39 _tmp5A8=*((struct _tuple39*)ds->hd);struct _tuple39 _stmttmp58=_tmp5A8;struct _tuple39 _tmp5A9=_stmttmp58;struct Cyc_Absyn_Vardecl*_tmp5AB;struct Cyc_Absyn_Vardecl*_tmp5AA;_tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;{struct Cyc_Absyn_Vardecl*oldv=_tmp5AA;struct Cyc_Absyn_Vardecl*newv=_tmp5AB;
({struct Cyc_List_List*_tmpA40=({struct Cyc_List_List*_tmp5AD=_region_malloc(rgn,sizeof(*_tmp5AD));({struct _tuple39*_tmpA3F=({struct _tuple39*_tmp5AC=_region_malloc(rgn,sizeof(*_tmp5AC));_tmp5AC->f1=oldv,_tmp5AC->f2=newv;_tmp5AC;});_tmp5AD->hd=_tmpA3F;}),_tmp5AD->tl=*decls;_tmp5AD;});*decls=_tmpA40;});
({struct Cyc_Absyn_Exp*_tmpA41=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->tag=4,_tmp5AE->f1=newv;_tmp5AE;}),0U);oldv->initializer=_tmpA41;});
((struct Cyc_Absyn_Exp*)_check_null(oldv->initializer))->topt=newv->type;
oldv->type=newv->type;
r=({struct Cyc_Absyn_Decl*_tmpA43=({struct Cyc_Absyn_Decl*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));({void*_tmpA42=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->tag=0,_tmp5AF->f1=oldv;_tmp5AF;});_tmp5B0->r=_tmpA42;}),_tmp5B0->loc=0U;_tmp5B0;});Cyc_Absyn_decl_stmt(_tmpA43,r,0U);});}}}
# 3289
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3293
Cyc_Toc_subst_pattern_vars(*decls,e);
Cyc_Toc_exp_to_c(nv,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_tmpA45=({struct Cyc_List_List*_tmp5B2=_region_malloc(rgn,sizeof(*_tmp5B2));({struct _tuple41*_tmpA44=({struct _tuple41*_tmp5B1=_region_malloc(rgn,sizeof(*_tmp5B1));_tmp5B1->f1=nv,_tmp5B1->f2=code_lab,_tmp5B1->f3=body;_tmp5B1;});_tmp5B2->hd=_tmpA44;}),_tmp5B2->tl=*bodies;_tmp5B2;});*bodies=_tmpA45;});
return r;}}}}
# 3303
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E4;_tmp7E4.tag=0,({struct _fat_ptr _tmpA46=({const char*_tmp5B5="couldn't find rhs!";_tag_fat(_tmp5B5,sizeof(char),19U);});_tmp7E4.f1=_tmpA46;});_tmp7E4;});void*_tmp5B3[1];_tmp5B3[0]=& _tmp5B4;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp5B3,sizeof(void*),1));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E5;_tmp7E5.tag=0,({struct _fat_ptr _tmpA47=({const char*_tmp5B8="bad where clause in match compiler";_tag_fat(_tmp5B8,sizeof(char),35U);});_tmp7E5.f1=_tmpA47;});_tmp7E5;});void*_tmp5B6[1];_tmp5B6[0]=& _tmp5B7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp5B6,sizeof(void*),1));});};}}else{
# 3308
for(1;ss != 0;ss=ss->tl){
struct _tuple34 _tmp5B9=*((struct _tuple34*)ss->hd);struct _tuple34 _stmttmp59=_tmp5B9;struct _tuple34 _tmp5BA=_stmttmp59;void*_tmp5BC;void*_tmp5BB;_tmp5BB=_tmp5BA.f1;_tmp5BC=_tmp5BA.f2;{void*pat_test=_tmp5BB;void*dec_tree=_tmp5BC;
struct Cyc_Absyn_Exp*_tmp5BD=Cyc_Toc_compile_pat_test(p,pat_test);struct Cyc_Absyn_Exp*test_exp=_tmp5BD;
struct Cyc_Absyn_Stmt*_tmp5BE=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);struct Cyc_Absyn_Stmt*s=_tmp5BE;
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}};}
# 3316
return res;}}}};}
# 3326
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3328
for(1;bodies != 0;bodies=bodies->tl){
struct _tuple41*_tmp5BF=(struct _tuple41*)bodies->hd;struct _tuple41*_stmttmp5A=_tmp5BF;struct _tuple41*_tmp5C0=_stmttmp5A;struct Cyc_Absyn_Stmt*_tmp5C2;struct Cyc_Toc_Env**_tmp5C1;_tmp5C1=(struct Cyc_Toc_Env**)& _tmp5C0->f1;_tmp5C2=_tmp5C0->f3;{struct Cyc_Toc_Env**nv=_tmp5C1;struct Cyc_Absyn_Stmt*s2=_tmp5C2;
if(s2 == s)return nv;}}
# 3334
return 0;}
# 3338
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3341
void*_tmp5C3=(void*)_check_null(e->topt);void*t=_tmp5C3;
Cyc_Toc_exp_to_c(nv,e);{
# 3344
struct _tuple30 _tmp5C4=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp5B=_tmp5C4;struct _tuple30 _tmp5C5=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp5C7;struct _tuple1*_tmp5C6;_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;{struct _tuple1*v=_tmp5C6;struct Cyc_Absyn_Exp*path=_tmp5C7;
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5C8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5C8;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5C9=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp5C9;
# 3350
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3355
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3363
{struct Cyc_List_List*_tmp5CA=lscs;struct Cyc_List_List*lscs2=_tmp5CA;for(0;lscs2 != 0;lscs2=lscs2->tl){
struct _tuple37*_tmp5CB=(struct _tuple37*)lscs2->hd;struct _tuple37*_stmttmp5C=_tmp5CB;struct _tuple37*_tmp5CC=_stmttmp5C;struct Cyc_Absyn_Switch_clause*_tmp5CE;struct _fat_ptr*_tmp5CD;_tmp5CD=_tmp5CC->f3;_tmp5CE=_tmp5CC->f4;{struct _fat_ptr*body_lab=_tmp5CD;struct Cyc_Absyn_Switch_clause*body_sc=_tmp5CE;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3367
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3370
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5CF=*envp;struct Cyc_Toc_Env*env=_tmp5CF;
# 3373
if(lscs2->tl != 0){
struct _tuple37*_tmp5D0=(struct _tuple37*)((struct Cyc_List_List*)_check_null(lscs2->tl))->hd;struct _tuple37*_stmttmp5D=_tmp5D0;struct _tuple37*_tmp5D1=_stmttmp5D;struct Cyc_Absyn_Switch_clause*_tmp5D3;struct _fat_ptr*_tmp5D2;_tmp5D2=_tmp5D1->f3;_tmp5D3=_tmp5D1->f4;{struct _fat_ptr*fallthru_lab=_tmp5D2;struct Cyc_Absyn_Switch_clause*next_sc=_tmp5D3;
# 3377
struct Cyc_Toc_Env**_tmp5D4=Cyc_Toc_find_case_env(mybodies,next_sc->body);struct Cyc_Toc_Env**next_case_env=_tmp5D4;
# 3382
if(next_case_env == 0)
({struct Cyc_Toc_Env*_tmpA48=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA48,s);});else{
# 3386
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(next_sc->pat_vars))->v)).f1);
vs=({struct Cyc_List_List*_tmpA49=mydecls;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c)((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc,_tmpA49,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(vs));});}
# 3391
({struct Cyc_Toc_Env*_tmpA4A=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_tmpA4A,s);});}}}else{
# 3395
({struct Cyc_Toc_Env*_tmpA4B=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_tmpA4B,s);});}}}}}{
# 3399
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA4D=test_tree;Cyc_Absyn_seq_stmt(_tmpA4D,({struct _fat_ptr*_tmpA4C=end_l;Cyc_Absyn_label_stmt(_tmpA4C,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3401
for(1;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp5D5=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp5E=_tmp5D5;struct _tuple39 _tmp5D6=_stmttmp5E;struct Cyc_Absyn_Vardecl*_tmp5D7;_tmp5D7=_tmp5D6.f2;{struct Cyc_Absyn_Vardecl*vd=_tmp5D7;
res=({struct Cyc_Absyn_Decl*_tmpA4E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->tag=0,_tmp5D8->f1=vd;_tmp5D8;}),0U);Cyc_Absyn_decl_stmt(_tmpA4E,res,0U);});}}
# 3406
({void*_tmpA52=({struct _tuple1*_tmpA51=v;void*_tmpA50=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA4F=e;Cyc_Absyn_declare_stmt(_tmpA51,_tmpA50,_tmpA4F,res,0U);})->r;whole_s->r=_tmpA52;});
_npop_handler(0);return;}}
# 3347
;_pop_region();}}}
# 3414
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp5D9[1];({struct Cyc_Absyn_Exp*_tmpA53=Cyc_Absyn_signed_int_exp(n - 1,0U);_tmp5D9[0]=_tmpA53;});({struct Cyc_Absyn_Exp*_tmpA54=Cyc_Toc__npop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA54,_tag_fat(_tmp5D9,sizeof(struct Cyc_Absyn_Exp*),1));});}),0U);}
# 3417
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA56=({struct Cyc_Absyn_Stmt*_tmpA55=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA55,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA56;});}
# 3422
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3424
while(1){
void*_tmp5DA=s->r;void*_stmttmp5F=_tmp5DA;void*_tmp5DB=_stmttmp5F;void*_tmp5DE;struct Cyc_List_List*_tmp5DD;struct Cyc_Absyn_Stmt*_tmp5DC;struct Cyc_Absyn_Stmt*_tmp5E0;struct Cyc_Absyn_Decl*_tmp5DF;struct Cyc_Absyn_Switch_clause**_tmp5E2;struct Cyc_List_List*_tmp5E1;void*_tmp5E5;struct Cyc_List_List*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Stmt*_tmp5E7;struct _fat_ptr*_tmp5E6;struct Cyc_Absyn_Stmt*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5EF;struct Cyc_Absyn_Exp*_tmp5EE;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5F2;struct Cyc_Absyn_Stmt*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Stmt*_tmp5F4;struct Cyc_Absyn_Stmt*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;switch(*((int*)_tmp5DB)){case 0:
 return;case 1: _tmp5F6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;{struct Cyc_Absyn_Exp*e=_tmp5F6;
Cyc_Toc_exp_to_c(nv,e);return;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1 == 0){
({int _tmpA57=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA57,s);});return;}else{_tmp5F5=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;{struct Cyc_Absyn_Exp*e=_tmp5F5;
# 3431
void*t=Cyc_Toc_typ_to_c((void*)_check_null(((struct Cyc_Absyn_Exp*)_check_null(e))->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*_tmp5F7=Cyc_Toc_temp_var();struct _tuple1*x=_tmp5F7;
struct Cyc_Absyn_Stmt*_tmp5F8=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);struct Cyc_Absyn_Stmt*retn_stmt=_tmp5F8;
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_tmpA58=(Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U))->r;s->r=_tmpA58;});}
# 3440
return;}}}case 2: _tmp5F3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5F4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp5F3;struct Cyc_Absyn_Stmt*s2=_tmp5F4;
# 3442
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4: _tmp5F0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5F1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;_tmp5F2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f3;{struct Cyc_Absyn_Exp*e=_tmp5F0;struct Cyc_Absyn_Stmt*s1=_tmp5F1;struct Cyc_Absyn_Stmt*s2=_tmp5F2;
# 3446
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9: _tmp5EC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5ED=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2).f1;_tmp5EE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f3).f1;_tmp5EF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f4;{struct Cyc_Absyn_Exp*e1=_tmp5EC;struct Cyc_Absyn_Exp*e2=_tmp5ED;struct Cyc_Absyn_Exp*e3=_tmp5EE;struct Cyc_Absyn_Stmt*s2=_tmp5EF;
# 3451
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_tmp5EA=s2;_tmp5EB=e3;goto _LL10;}case 14: _tmp5EA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5EB=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2).f1;_LL10: {struct Cyc_Absyn_Stmt*s2=_tmp5EA;struct Cyc_Absyn_Exp*e=_tmp5EB;
_tmp5E8=e;_tmp5E9=s2;goto _LL12;}case 5: _tmp5E8=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1).f1;_tmp5E9=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_tmp5E8;struct Cyc_Absyn_Stmt*s2=_tmp5E9;
# 3456
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _tmp5F9=_new_region("temp");struct _RegionHandle*temp=& _tmp5F9;_push_region(temp);({struct Cyc_Toc_Env*_tmpA59=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA59,s2);});
_npop_handler(0);return;
# 3457
;_pop_region();}}case 6:  {
# 3460
struct Cyc_Toc_Env*_tmp5FA=nv;struct _fat_ptr**_tmp5FB;_tmp5FB=_tmp5FA->break_lab;{struct _fat_ptr**b=_tmp5FB;
if(b != 0)
({void*_tmpA5A=Cyc_Toc_goto_stmt_r(*b);s->r=_tmpA5A;});
# 3464
({int _tmpA5B=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA5B,s);});
return;}}case 7:  {
# 3467
struct Cyc_Toc_Env*_tmp5FC=nv;struct _fat_ptr**_tmp5FD;_tmp5FD=_tmp5FC->continue_lab;{struct _fat_ptr**c=_tmp5FD;
if(c != 0)
({void*_tmpA5C=Cyc_Toc_goto_stmt_r(*c);s->r=_tmpA5C;});
goto _LL18;}}case 8: _LL18:
# 3472
({int _tmpA5D=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA5D,s);});
return;case 13: _tmp5E6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5E7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;{struct _fat_ptr*lab=_tmp5E6;struct Cyc_Absyn_Stmt*s1=_tmp5E7;
s=s1;continue;}case 10: _tmp5E3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5E4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;_tmp5E5=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f3;{struct Cyc_Absyn_Exp*e=_tmp5E3;struct Cyc_List_List*scs=_tmp5E4;void*dec_tree_opt=_tmp5E5;
# 3476
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11: _tmp5E1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5E2=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;{struct Cyc_List_List*es=_tmp5E1;struct Cyc_Absyn_Switch_clause**dest_clause=_tmp5E2;
# 3479
struct Cyc_Toc_Env*_tmp5FE=nv;struct Cyc_Toc_FallthruInfo*_tmp5FF;_tmp5FF=_tmp5FE->fallthru_info;{struct Cyc_Toc_FallthruInfo*fi=_tmp5FF;
if(fi == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp601=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E6;_tmp7E6.tag=0,({struct _fat_ptr _tmpA5E=({const char*_tmp602="fallthru in unexpected place";_tag_fat(_tmp602,sizeof(char),29U);});_tmp7E6.f1=_tmpA5E;});_tmp7E6;});void*_tmp600[1];_tmp600[0]=& _tmp601;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp600,sizeof(void*),1));});{
struct Cyc_Toc_FallthruInfo _tmp603=*fi;struct Cyc_Toc_FallthruInfo _stmttmp60=_tmp603;struct Cyc_Toc_FallthruInfo _tmp604=_stmttmp60;struct Cyc_List_List*_tmp606;struct _fat_ptr*_tmp605;_tmp605=_tmp604.label;_tmp606=_tmp604.binders;{struct _fat_ptr*l=_tmp605;struct Cyc_List_List*vs=_tmp606;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3485
({int _tmpA5F=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA5F,s2);});{
struct Cyc_List_List*_tmp607=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(vs);struct Cyc_List_List*vs2=_tmp607;
struct Cyc_List_List*_tmp608=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(es);struct Cyc_List_List*es2=_tmp608;
for(1;vs2 != 0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd);
s2=({struct Cyc_Absyn_Stmt*_tmpA61=({struct Cyc_Absyn_Exp*_tmpA60=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->tag=5,_tmp609->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_tmp609;}),0U);Cyc_Absyn_assign_stmt(_tmpA60,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA61,s2,0U);});}
# 3493
s->r=s2->r;
return;}}}}}case 12: _tmp5DF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5E0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;{struct Cyc_Absyn_Decl*d=_tmp5DF;struct Cyc_Absyn_Stmt*s1=_tmp5E0;
# 3499
{void*_tmp60A=d->r;void*_stmttmp61=_tmp60A;void*_tmp60B=_stmttmp61;struct Cyc_Absyn_Exp*_tmp60E;struct Cyc_Absyn_Vardecl*_tmp60D;struct Cyc_Absyn_Tvar*_tmp60C;struct Cyc_Absyn_Fndecl*_tmp60F;struct Cyc_List_List*_tmp610;void*_tmp613;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_Absyn_Pat*_tmp611;struct Cyc_Absyn_Vardecl*_tmp614;switch(*((int*)_tmp60B)){case 0: _tmp614=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp60B)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp614;
Cyc_Toc_local_decl_to_c(nv,vd,s1);goto _LL2F;}case 2: _tmp611=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60B)->f1;_tmp612=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60B)->f3;_tmp613=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60B)->f4;{struct Cyc_Absyn_Pat*p=_tmp611;struct Cyc_Absyn_Exp*e=_tmp612;void*dec_tree=_tmp613;
# 3505
{void*_tmp615=p->r;void*_stmttmp62=_tmp615;void*_tmp616=_stmttmp62;struct Cyc_Absyn_Vardecl*_tmp617;if(*((int*)_tmp616)== 1){if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp616)->f2)->r)== 0){_tmp617=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp616)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp617;
# 3507
struct Cyc_Absyn_Vardecl*_tmp618=({struct _tuple1*_tmpA63=Cyc_Toc_temp_var();void*_tmpA62=vd->type;Cyc_Absyn_new_vardecl(0U,_tmpA63,_tmpA62,e);});struct Cyc_Absyn_Vardecl*new_vd=_tmp618;
({struct Cyc_Absyn_Exp*_tmpA64=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=4,_tmp619->f1=new_vd;_tmp619;}),0U);vd->initializer=_tmpA64;});
((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->topt=new_vd->type;
({void*_tmpA6B=({struct Cyc_Absyn_Decl*_tmpA6A=({void*_tmpA65=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=0,_tmp61A->f1=new_vd;_tmp61A;});Cyc_Absyn_new_decl(_tmpA65,s->loc);});struct Cyc_Absyn_Stmt*_tmpA69=({
struct Cyc_Absyn_Decl*_tmpA68=({void*_tmpA66=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->tag=0,_tmp61B->f1=vd;_tmp61B;});Cyc_Absyn_new_decl(_tmpA66,s->loc);});struct Cyc_Absyn_Stmt*_tmpA67=s1;Cyc_Absyn_decl_stmt(_tmpA68,_tmpA67,s->loc);});
# 3510
Cyc_Absyn_decl_stmt(_tmpA6A,_tmpA69,s->loc);})->r;s->r=_tmpA6B;});
# 3513
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F:
# 3519
({void*_tmpA6C=(Cyc_Toc_letdecl_to_c(nv,p,dec_tree,(void*)_check_null(e->topt),e,s1))->r;s->r=_tmpA6C;});
goto _LL3C;}_LL3C:;}
# 3522
goto _LL2F;}case 3: _tmp610=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp60B)->f1;{struct Cyc_List_List*vds=_tmp610;
# 3526
struct Cyc_List_List*_tmp61C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(vds);struct Cyc_List_List*rvds=_tmp61C;
if(rvds == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E7;_tmp7E7.tag=0,({struct _fat_ptr _tmpA6D=({const char*_tmp61F="empty Letv_d";_tag_fat(_tmp61F,sizeof(char),13U);});_tmp7E7.f1=_tmpA6D;});_tmp7E7;});void*_tmp61D[1];_tmp61D[0]=& _tmp61E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp61D,sizeof(void*),1));});
({void*_tmpA6E=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=0,_tmp620->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp620;});d->r=_tmpA6E;});
rvds=rvds->tl;
for(1;rvds != 0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*_tmp621=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->tag=0,_tmp622->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_tmp622;}),0U);struct Cyc_Absyn_Decl*d2=_tmp621;
({void*_tmpA70=({struct Cyc_Absyn_Decl*_tmpA6F=d2;Cyc_Absyn_decl_stmt(_tmpA6F,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA70;});}
# 3535
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1: _tmp60F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp60B)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp60F;
# 3538
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,s1);
goto _LL2F;}case 4: _tmp60C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60B)->f1;_tmp60D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60B)->f2;_tmp60E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60B)->f3;{struct Cyc_Absyn_Tvar*tv=_tmp60C;struct Cyc_Absyn_Vardecl*vd=_tmp60D;struct Cyc_Absyn_Exp*open_exp_opt=_tmp60E;
# 3542
struct Cyc_Absyn_Stmt*_tmp623=s1;struct Cyc_Absyn_Stmt*body=_tmp623;
# 3544
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _tmp624=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp63=_tmp624;struct _tuple30 _tmp625=_stmttmp63;struct Cyc_Absyn_Exp*_tmp627;struct _tuple1*_tmp626;_tmp626=_tmp625.f1;_tmp627=_tmp625.f2;{struct _tuple1*rh_var=_tmp626;struct Cyc_Absyn_Exp*rh_exp=_tmp627;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3550
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Flags_no_regions)
({void*_tmpA74=({struct _tuple1*_tmpA73=x_var;void*_tmpA72=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA71=
Cyc_Absyn_signed_int_exp(0,0U);
# 3552
Cyc_Absyn_declare_stmt(_tmpA73,_tmpA72,_tmpA71,body,0U);})->r;s->r=_tmpA74;});else{
# 3554
if((unsigned)open_exp_opt){
# 3556
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA76=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_tmpA76,({struct _fat_ptr*_tmp629=_cycalloc(sizeof(*_tmp629));({struct _fat_ptr _tmpA75=({const char*_tmp628="h";_tag_fat(_tmp628,sizeof(char),2U);});*_tmp629=_tmpA75;});_tmp629;}),0U);}),0U);
({void*_tmpA77=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U))->r;s->r=_tmpA77;});}}else{
# 3565
({void*_tmpA84=({struct _tuple1*_tmpA83=rh_var;void*_tmpA82=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA81=({struct Cyc_Absyn_Exp*_tmp62A[1];({struct Cyc_Absyn_Exp*_tmpA78=
Cyc_Absyn_string_exp(*(*x_var).f2,0U);_tmp62A[0]=_tmpA78;});({struct Cyc_Absyn_Exp*_tmpA79=Cyc_Toc__new_region_e;Cyc_Toc_fncall_exp_dl(_tmpA79,_tag_fat(_tmp62A,sizeof(struct Cyc_Absyn_Exp*),1));});});
# 3565
Cyc_Absyn_declare_stmt(_tmpA83,_tmpA82,_tmpA81,({
# 3567
struct _tuple1*_tmpA80=x_var;void*_tmpA7F=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA80,_tmpA7F,_tmpA7E,({
struct Cyc_Absyn_Stmt*_tmpA7D=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp62B[1];_tmp62B[0]=x_exp;({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Toc__push_region_e;Cyc_Toc_fncall_exp_dl(_tmpA7A,_tag_fat(_tmp62B,sizeof(struct Cyc_Absyn_Exp*),1));});}),0U);Cyc_Absyn_seq_stmt(_tmpA7D,({
struct Cyc_Absyn_Stmt*_tmpA7C=body;Cyc_Absyn_seq_stmt(_tmpA7C,
Cyc_Absyn_exp_stmt(({void*_tmp62C=0U;({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Toc__pop_region_e;Cyc_Toc_fncall_exp_dl(_tmpA7B,_tag_fat(_tmp62C,sizeof(struct Cyc_Absyn_Exp*),0));});}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3565
s->r=_tmpA84;});}}
# 3573
return;}}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E8;_tmp7E8.tag=0,({struct _fat_ptr _tmpA85=({const char*_tmp62F="bad nested declaration within function";_tag_fat(_tmp62F,sizeof(char),39U);});_tmp7E8.f1=_tmpA85;});_tmp7E8;});void*_tmp62D[1];_tmp62D[0]=& _tmp62E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp62D,sizeof(void*),1));});}_LL2F:;}
# 3576
return;}default: _tmp5DC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f1;_tmp5DD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f2;_tmp5DE=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5DB)->f3;{struct Cyc_Absyn_Stmt*body=_tmp5DC;struct Cyc_List_List*scs=_tmp5DD;void*dec_tree=_tmp5DE;
# 3590 "toc.cyc"
struct _tuple30 _tmp630=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp64=_tmp630;struct _tuple30 _tmp631=_stmttmp64;struct Cyc_Absyn_Exp*_tmp633;struct _tuple1*_tmp632;_tmp632=_tmp631.f1;_tmp633=_tmp631.f2;{struct _tuple1*h_var=_tmp632;struct Cyc_Absyn_Exp*h_exp=_tmp633;
struct _tuple30 _tmp634=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp65=_tmp634;struct _tuple30 _tmp635=_stmttmp65;struct Cyc_Absyn_Exp*_tmp637;struct _tuple1*_tmp636;_tmp636=_tmp635.f1;_tmp637=_tmp635.f2;{struct _tuple1*e_var=_tmp636;struct Cyc_Absyn_Exp*e_exp=_tmp637;
struct _tuple30 _tmp638=Cyc_Toc_temp_var_and_exp();struct _tuple30 _stmttmp66=_tmp638;struct _tuple30 _tmp639=_stmttmp66;struct Cyc_Absyn_Exp*_tmp63B;struct _tuple1*_tmp63A;_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;{struct _tuple1*was_thrown_var=_tmp63A;struct Cyc_Absyn_Exp*was_thrown_exp=_tmp63B;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3597
e_exp->topt=e_typ;{
struct _RegionHandle _tmp63C=_new_region("temp");struct _RegionHandle*temp=& _tmp63C;_push_region(temp);
# 3600
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*_tmp63D=({struct Cyc_Absyn_Stmt*_tmpA87=body;Cyc_Absyn_seq_stmt(_tmpA87,
Cyc_Absyn_exp_stmt(({void*_tmp646=0U;({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Toc__pop_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA86,_tag_fat(_tmp646,sizeof(struct Cyc_Absyn_Exp*),0));});}),0U),0U);});
# 3601
struct Cyc_Absyn_Stmt*tryandpop_stmt=_tmp63D;
# 3604
struct Cyc_Absyn_Stmt*_tmp63E=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tag=10,_tmp645->f1=e_exp,_tmp645->f2=scs,_tmp645->f3=dec_tree;_tmp645;}),0U);struct Cyc_Absyn_Stmt*handler_stmt=_tmp63E;
# 3606
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3609
struct Cyc_Absyn_Exp*_tmp63F=({struct Cyc_Absyn_Exp*_tmp644[1];({struct Cyc_Absyn_Exp*_tmpA88=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp644[0]=_tmpA88;});({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Toc_setjmp_e;Cyc_Toc_fncall_exp_dl(_tmpA89,_tag_fat(_tmp644,sizeof(struct Cyc_Absyn_Exp*),1));});});
# 3609
struct Cyc_Absyn_Exp*setjmp_call=_tmp63F;
# 3611
struct Cyc_Absyn_Stmt*_tmp640=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp643[1];({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_Absyn_address_exp(h_exp,0U);_tmp643[0]=_tmpA8A;});({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Toc__push_handler_e;Cyc_Toc_fncall_exp_dl(_tmpA8B,_tag_fat(_tmp643,sizeof(struct Cyc_Absyn_Exp*),1));});}),0U);
# 3611
struct Cyc_Absyn_Stmt*pushhandler_call=_tmp640;
# 3613
struct Cyc_Absyn_Exp*_tmp641=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);struct Cyc_Absyn_Exp*zero_exp=_tmp641;
struct Cyc_Absyn_Exp*_tmp642=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);struct Cyc_Absyn_Exp*one_exp=_tmp642;
({void*_tmpA9B=({
struct _tuple1*_tmpA9A=h_var;void*_tmpA99=h_typ;Cyc_Absyn_declare_stmt(_tmpA9A,_tmpA99,0,({
struct Cyc_Absyn_Stmt*_tmpA98=pushhandler_call;Cyc_Absyn_seq_stmt(_tmpA98,({
struct _tuple1*_tmpA97=was_thrown_var;void*_tmpA96=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpA95=zero_exp;Cyc_Absyn_declare_stmt(_tmpA97,_tmpA96,_tmpA95,({
struct Cyc_Absyn_Stmt*_tmpA94=({struct Cyc_Absyn_Exp*_tmpA8D=setjmp_call;struct Cyc_Absyn_Stmt*_tmpA8C=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3619
Cyc_Absyn_ifthenelse_stmt(_tmpA8D,_tmpA8C,
# 3621
Cyc_Toc_skip_stmt_dl(),0U);});
# 3619
Cyc_Absyn_seq_stmt(_tmpA94,({
# 3622
struct Cyc_Absyn_Exp*_tmpA93=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpA92=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_tmpA93,_tmpA92,({
# 3624
struct _tuple1*_tmpA91=e_var;void*_tmpA90=e_typ;struct Cyc_Absyn_Exp*_tmpA8F=({
void*_tmpA8E=e_typ;Cyc_Toc_cast_it(_tmpA8E,Cyc_Toc_get_exn_thrown_expression());});
# 3624
Cyc_Absyn_declare_stmt(_tmpA91,_tmpA90,_tmpA8F,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3615
s->r=_tmpA9B;});}}
# 3628
_npop_handler(0);return;
# 3598
;_pop_region();}}}}}};}}
# 3636
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
(f->i).tvars=0;
(f->i).effect=0;
(f->i).rgn_po=0;
(f->i).requires_clause=0;
(f->i).ensures_clause=0;
({void*_tmpA9C=Cyc_Toc_typ_to_c((f->i).ret_type);(f->i).ret_type=_tmpA9C;});
({void*_tmpA9D=Cyc_Toc_typ_to_c((void*)_check_null(f->cached_type));f->cached_type=_tmpA9D;});{
struct _RegionHandle _tmp647=_new_region("frgn");struct _RegionHandle*frgn=& _tmp647;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp648=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_tmp648;
{struct Cyc_List_List*_tmp649=(f->i).args;struct Cyc_List_List*args=_tmp649;for(0;args != 0;args=args->tl){
({void*_tmpA9E=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpA9E;});}}
# 3651
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 3655
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_Hashtable_Table*_tmpA9F=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp64A=_tmpA9F;});_tmp64A;});
if((unsigned)(f->i).cyc_varargs &&((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp64B=*((struct Cyc_Absyn_VarargInfo*)_check_null((f->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp67=_tmp64B;struct Cyc_Absyn_VarargInfo _tmp64C=_stmttmp67;int _tmp650;void*_tmp64F;struct Cyc_Absyn_Tqual _tmp64E;struct _fat_ptr*_tmp64D;_tmp64D=_tmp64C.name;_tmp64E=_tmp64C.tq;_tmp64F=_tmp64C.type;_tmp650=_tmp64C.inject;{struct _fat_ptr*n=_tmp64D;struct Cyc_Absyn_Tqual tq=_tmp64E;void*t=_tmp64F;int i=_tmp650;
void*_tmp651=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));void*t2=_tmp651;
struct _tuple1*_tmp652=({struct _tuple1*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->f1=Cyc_Absyn_Loc_n,_tmp655->f2=(struct _fat_ptr*)_check_null(n);_tmp655;});struct _tuple1*x2=_tmp652;
({struct Cyc_List_List*_tmpAA2=({struct Cyc_List_List*_tmpAA1=(f->i).args;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmpAA1,({struct Cyc_List_List*_tmp654=_cycalloc(sizeof(*_tmp654));({struct _tuple9*_tmpAA0=({struct _tuple9*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->f1=n,_tmp653->f2=tq,_tmp653->f3=t2;_tmp653;});_tmp654->hd=_tmpAA0;}),_tmp654->tl=0;_tmp654;}));});(f->i).args=_tmpAA2;});
(f->i).cyc_varargs=0;}}
# 3664
{struct Cyc_List_List*_tmp656=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;struct Cyc_List_List*arg_vds=_tmp656;for(0;arg_vds != 0;arg_vds=arg_vds->tl){
({void*_tmpAA3=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_tmpAA3;});}}
({struct Cyc_Toc_Env*_tmpAA4=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_tmpAA4,f->body);});}
# 3646
;_pop_region();}}
# 3669
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp657=s;switch(_tmp657){case Cyc_Absyn_Abstract:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC:
 return Cyc_Absyn_Extern;default:
 return s;};}struct _tuple42{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3685 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple42*env){
# 3687
struct _tuple42 _tmp658=*env;struct _tuple42 _stmttmp68=_tmp658;struct _tuple42 _tmp659=_stmttmp68;struct Cyc_Absyn_Aggrdecl*_tmp65B;struct Cyc_Toc_TocState*_tmp65A;_tmp65A=_tmp659.f1;_tmp65B=*_tmp659.f2;{struct Cyc_Toc_TocState*s=_tmp65A;struct Cyc_Absyn_Aggrdecl*ad=_tmp65B;
struct _tuple1*_tmp65C=ad->name;struct _tuple1*n=_tmp65C;
struct Cyc_Toc_TocState _tmp65D=*s;struct Cyc_Toc_TocState _stmttmp69=_tmp65D;struct Cyc_Toc_TocState _tmp65E=_stmttmp69;struct Cyc_Dict_Dict*_tmp65F;_tmp65F=_tmp65E.aggrs_so_far;{struct Cyc_Dict_Dict*aggrs_so_far=_tmp65F;
int seen_defn_before;
struct _tuple18**_tmp660=((struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt)(*aggrs_so_far,n);struct _tuple18**dopt=_tmp660;
if(dopt == 0){
seen_defn_before=0;{
struct _tuple18*v=({struct _tuple18*_tmp661=_region_malloc(d,sizeof(*_tmp661));
_tmp661->f1=ad,(int)ad->kind == (int)Cyc_Absyn_StructA?({void*_tmpAA6=Cyc_Absyn_strctq(n);_tmp661->f2=_tmpAA6;}):({void*_tmpAA5=Cyc_Absyn_unionq_type(n);_tmp661->f2=_tmpAA5;});_tmp661;});
({struct Cyc_Dict_Dict _tmpAA7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert)(*aggrs_so_far,n,v);*aggrs_so_far=_tmpAA7;});}}else{
# 3698
struct _tuple18*_tmp662=*dopt;struct _tuple18*_stmttmp6A=_tmp662;struct _tuple18*_tmp663=_stmttmp6A;void*_tmp665;struct Cyc_Absyn_Aggrdecl*_tmp664;_tmp664=_tmp663->f1;_tmp665=_tmp663->f2;{struct Cyc_Absyn_Aggrdecl*ad2=_tmp664;void*t=_tmp665;
if(ad2->impl == 0){
({struct Cyc_Dict_Dict _tmpAAA=({struct Cyc_Dict_Dict _tmpAA9=*aggrs_so_far;struct _tuple1*_tmpAA8=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert)(_tmpAA9,_tmpAA8,({struct _tuple18*_tmp666=_region_malloc(d,sizeof(*_tmp666));_tmp666->f1=ad,_tmp666->f2=t;_tmp666;}));});*aggrs_so_far=_tmpAAA;});
seen_defn_before=0;}else{
# 3703
seen_defn_before=1;}}}{
# 3705
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B->kind=ad->kind,_tmp68B->sc=Cyc_Absyn_Public,_tmp68B->name=ad->name,_tmp68B->tvs=0,_tmp68B->impl=0,_tmp68B->expected_mem_kind=0,_tmp68B->attributes=ad->attributes;_tmp68B;});
# 3712
if(ad->impl != 0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpAAB=({struct Cyc_Absyn_AggrdeclImpl*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->exist_vars=0,_tmp667->rgn_po=0,_tmp667->fields=0,_tmp667->tagged=0;_tmp667;});new_ad->impl=_tmpAAB;});{
# 3717
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp668=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp668;for(0;fields != 0;fields=fields->tl){
# 3721
struct Cyc_Absyn_Aggrfield*_tmp669=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*old_field=_tmp669;
void*_tmp66A=old_field->type;void*old_type=_tmp66A;
struct Cyc_List_List*_tmp66B=old_field->attributes;struct Cyc_List_List*old_atts=_tmp66B;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind == (int)Cyc_Absyn_StructA && fields->tl == 0 ||(int)ad->kind == (int)Cyc_Absyn_UnionA)){
# 3735 "toc.cyc"
void*_tmp66C=Cyc_Absyn_compress(old_type);void*_stmttmp6B=_tmp66C;void*_tmp66D=_stmttmp6B;unsigned _tmp671;void*_tmp670;struct Cyc_Absyn_Tqual _tmp66F;void*_tmp66E;if(*((int*)_tmp66D)== 4){_tmp66E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp66D)->f1).elt_type;_tmp66F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp66D)->f1).tq;_tmp670=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp66D)->f1).zero_term;_tmp671=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp66D)->f1).zt_loc;{void*et=_tmp66E;struct Cyc_Absyn_Tqual tq=_tmp66F;void*zt=_tmp670;unsigned ztl=_tmp671;
# 3738
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->tag=4,(_tmp672->f1).elt_type=et,(_tmp672->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Absyn_uint_exp(0U,0U);(_tmp672->f1).num_elts=_tmpAAC;}),(_tmp672->f1).zero_term=zt,(_tmp672->f1).zt_loc=ztl;_tmp672;});
goto _LL9;}}else{
# 3741
old_atts=({struct Cyc_List_List*_tmp674=_cycalloc(sizeof(*_tmp674));({void*_tmpAAD=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->tag=6,_tmp673->f1=0;_tmp673;});_tmp674->hd=_tmpAAD;}),_tmp674->tl=old_atts;_tmp674;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=4,({void*_tmpAAF=Cyc_Toc_void_star_type();(_tmp675->f1).elt_type=_tmpAAF;}),(_tmp675->f1).tq=Cyc_Toc_mt_tq,({
# 3744
struct Cyc_Absyn_Exp*_tmpAAE=Cyc_Absyn_uint_exp(0U,0U);(_tmp675->f1).num_elts=_tmpAAE;}),(_tmp675->f1).zero_term=Cyc_Absyn_false_type,(_tmp675->f1).zt_loc=0U;_tmp675;});}_LL9:;}{
# 3748
struct Cyc_Absyn_Aggrfield*_tmp676=({struct Cyc_Absyn_Aggrfield*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->name=old_field->name,_tmp687->tq=Cyc_Toc_mt_tq,({
# 3750
void*_tmpAB1=Cyc_Toc_typ_to_c(old_type);_tmp687->type=_tmpAB1;}),_tmp687->width=old_field->width,({
# 3752
struct Cyc_List_List*_tmpAB0=Cyc_Atts_atts2c(old_atts);_tmp687->attributes=_tmpAB0;}),_tmp687->requires_clause=0;_tmp687;});
# 3748
struct Cyc_Absyn_Aggrfield*new_field=_tmp676;
# 3758
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp677=new_field->type;void*T=_tmp677;
struct _fat_ptr*_tmp678=new_field->name;struct _fat_ptr*f=_tmp678;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp684=({struct Cyc_String_pa_PrintArg_struct _tmp7EA;_tmp7EA.tag=0,_tmp7EA.f1=(struct _fat_ptr)((struct _fat_ptr)*(*ad->name).f2);_tmp7EA;});struct Cyc_String_pa_PrintArg_struct _tmp685=({struct Cyc_String_pa_PrintArg_struct _tmp7E9;_tmp7E9.tag=0,_tmp7E9.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp7E9;});void*_tmp682[2];_tmp682[0]=& _tmp684,_tmp682[1]=& _tmp685;({struct _fat_ptr _tmpAB2=({const char*_tmp683="_union_%s_%s";_tag_fat(_tmp683,sizeof(char),13U);});Cyc_aprintf(_tmpAB2,_tag_fat(_tmp682,sizeof(void*),2));});});
struct _fat_ptr*str=({struct _fat_ptr*_tmp681=_cycalloc(sizeof(*_tmp681));*_tmp681=s;_tmp681;});
struct Cyc_Absyn_Aggrfield*_tmp679=({struct Cyc_Absyn_Aggrfield*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680->name=Cyc_Toc_val_sp,_tmp680->tq=Cyc_Toc_mt_tq,_tmp680->type=T,_tmp680->width=0,_tmp680->attributes=0,_tmp680->requires_clause=0;_tmp680;});struct Cyc_Absyn_Aggrfield*value_field=_tmp679;
struct Cyc_Absyn_Aggrfield*_tmp67A=({struct Cyc_Absyn_Aggrfield*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->name=Cyc_Toc_tag_sp,_tmp67F->tq=Cyc_Toc_mt_tq,_tmp67F->type=Cyc_Absyn_sint_type,_tmp67F->width=0,_tmp67F->attributes=0,_tmp67F->requires_clause=0;_tmp67F;});struct Cyc_Absyn_Aggrfield*tag_field=_tmp67A;
struct Cyc_Absyn_Aggrdecl*_tmp67B=({struct _fat_ptr*_tmpAB3=str;Cyc_Toc_make_c_struct_defn(_tmpAB3,({struct Cyc_Absyn_Aggrfield*_tmp67E[2];_tmp67E[0]=tag_field,_tmp67E[1]=value_field;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp67E,sizeof(struct Cyc_Absyn_Aggrfield*),2));}));});struct Cyc_Absyn_Aggrdecl*ad2=_tmp67B;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));({struct Cyc_Absyn_Decl*_tmpAB4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->tag=5,_tmp67C->f1=ad2;_tmp67C;}),0U);_tmp67D->hd=_tmpAB4;}),_tmp67D->tl=Cyc_Toc_result_decls;_tmp67D;});
({void*_tmpAB5=Cyc_Absyn_strct(str);new_field->type=_tmpAB5;});}
# 3769
new_fields=({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->hd=new_field,_tmp686->tl=new_fields;_tmp686;});}}}
# 3771
({struct Cyc_List_List*_tmpAB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpAB6;});}}
# 3774
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp68A=_cycalloc(sizeof(*_tmp68A));({struct Cyc_Absyn_Decl*_tmpAB8=({struct Cyc_Absyn_Decl*_tmp689=_cycalloc(sizeof(*_tmp689));({void*_tmpAB7=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->tag=5,_tmp688->f1=new_ad;_tmp688;});_tmp689->r=_tmpAB7;}),_tmp689->loc=0U;_tmp689;});_tmp68A->hd=_tmpAB8;}),_tmp68A->tl=Cyc_Toc_result_decls;_tmp68A;});
return 0;}}}}
# 3778
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple42*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple43{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3806 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3809
struct _tuple43 _tmp68C=*env;struct _tuple43 _stmttmp6C=_tmp68C;struct _tuple43 _tmp68D=_stmttmp6C;struct Cyc_Absyn_Datatypedecl*_tmp68F;struct Cyc_Set_Set**_tmp68E;_tmp68E=(_tmp68D.f1)->datatypes_so_far;_tmp68F=_tmp68D.f2;{struct Cyc_Set_Set**datatypes_so_far=_tmp68E;struct Cyc_Absyn_Datatypedecl*tud=_tmp68F;
struct _tuple1*_tmp690=tud->name;struct _tuple1*n=_tmp690;
if(tud->fields == 0 ||((int(*)(struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_member)(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_tmpAB9=((struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_rinsert)(d,*datatypes_so_far,n);*datatypes_so_far=_tmpAB9;});
{struct Cyc_List_List*_tmp691=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fields=_tmp691;for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3817
struct Cyc_List_List*_tmp692=0;struct Cyc_List_List*fs=_tmp692;
int i=1;
{struct Cyc_List_List*_tmp693=f->typs;struct Cyc_List_List*ts=_tmp693;for(0;ts != 0;(ts=ts->tl,i ++)){
struct _fat_ptr*_tmp694=Cyc_Absyn_fieldname(i);struct _fat_ptr*fname=_tmp694;
struct Cyc_Absyn_Aggrfield*_tmp695=({struct Cyc_Absyn_Aggrfield*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->name=fname,_tmp697->tq=(*((struct _tuple23*)ts->hd)).f1,({
void*_tmpABA=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f2);_tmp697->type=_tmpABA;}),_tmp697->width=0,_tmp697->attributes=0,_tmp697->requires_clause=0;_tmp697;});
# 3821
struct Cyc_Absyn_Aggrfield*f=_tmp695;
# 3823
fs=({struct Cyc_List_List*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->hd=f,_tmp696->tl=fs;_tmp696;});}}
# 3825
fs=({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));({struct Cyc_Absyn_Aggrfield*_tmpABC=({struct Cyc_Absyn_Aggrfield*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->name=Cyc_Toc_tag_sp,_tmp698->tq=Cyc_Toc_mt_tq,_tmp698->type=Cyc_Absyn_sint_type,_tmp698->width=0,_tmp698->attributes=0,_tmp698->requires_clause=0;_tmp698;});_tmp699->hd=_tmpABC;}),({
struct Cyc_List_List*_tmpABB=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fs);_tmp699->tl=_tmpABB;});_tmp699;});{
struct Cyc_Absyn_Aggrdecl*_tmp69A=({struct _fat_ptr*_tmpABE=({struct _fat_ptr*_tmp69E=_cycalloc(sizeof(*_tmp69E));({struct _fat_ptr _tmpABD=({const char*_tmp69D="";_tag_fat(_tmp69D,sizeof(char),1U);});*_tmp69E=_tmpABD;});_tmp69E;});Cyc_Toc_make_c_struct_defn(_tmpABE,fs);});struct Cyc_Absyn_Aggrdecl*ad=_tmp69A;
({struct _tuple1*_tmpABF=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_tmpABF;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp69C=_cycalloc(sizeof(*_tmp69C));({struct Cyc_Absyn_Decl*_tmpAC0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->tag=5,_tmp69B->f1=ad;_tmp69B;}),0U);_tmp69C->hd=_tmpAC0;}),_tmp69C->tl=Cyc_Toc_result_decls;_tmp69C;});}}}
# 3831
return 0;}}
# 3834
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3853 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3856
struct _tuple43 _tmp69F=*env;struct _tuple43 _stmttmp6D=_tmp69F;struct _tuple43 _tmp6A0=_stmttmp6D;struct Cyc_Absyn_Datatypedecl*_tmp6A2;struct Cyc_Toc_TocState*_tmp6A1;_tmp6A1=_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;{struct Cyc_Toc_TocState*s=_tmp6A1;struct Cyc_Absyn_Datatypedecl*xd=_tmp6A2;
if(xd->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6A3=*s;struct Cyc_Toc_TocState _stmttmp6E=_tmp6A3;struct Cyc_Toc_TocState _tmp6A4=_stmttmp6E;struct Cyc_Dict_Dict*_tmp6A5;_tmp6A5=_tmp6A4.xdatatypes_so_far;{struct Cyc_Dict_Dict*xdatatypes_so_far=_tmp6A5;
struct _tuple1*_tmp6A6=xd->name;struct _tuple1*n=_tmp6A6;
{struct Cyc_List_List*_tmp6A7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;struct Cyc_List_List*fs=_tmp6A7;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6A8=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);struct Cyc_Absyn_Exp*sz_exp=_tmp6A8;
void*_tmp6A9=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);void*tag_typ=_tmp6A9;
# 3867
int*_tmp6AA=((int*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt)(*xdatatypes_so_far,f->name);int*_stmttmp6F=_tmp6AA;int*_tmp6AB=_stmttmp6F;if(_tmp6AB == 0){
# 3869
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc != (int)Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
# 3873
struct Cyc_Absyn_Vardecl*_tmp6AC=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6AC;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct Cyc_Absyn_Decl*_tmpAC1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tag=0,_tmp6AD->f1=tag_decl;_tmp6AD;}),0U);_tmp6AE->hd=_tmpAC1;}),_tmp6AE->tl=Cyc_Toc_result_decls;_tmp6AE;});
({struct Cyc_Dict_Dict _tmpAC2=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,(int)f->sc != (int)Cyc_Absyn_Extern);
# 3876
*xdatatypes_so_far=_tmpAC2;});{
# 3878
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6AF=f->typs;struct Cyc_List_List*tqts=_tmp6AF;for(0;tqts != 0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*_tmp6B0=Cyc_Absyn_fieldname(i);struct _fat_ptr*field_n=_tmp6B0;
struct Cyc_Absyn_Aggrfield*_tmp6B1=({struct Cyc_Absyn_Aggrfield*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));_tmp6B3->name=field_n,_tmp6B3->tq=(*((struct _tuple23*)tqts->hd)).f1,({
void*_tmpAC3=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f2);_tmp6B3->type=_tmpAC3;}),_tmp6B3->width=0,_tmp6B3->attributes=0,_tmp6B3->requires_clause=0;_tmp6B3;});
# 3882
struct Cyc_Absyn_Aggrfield*newf=_tmp6B1;
# 3884
fields=({struct Cyc_List_List*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=newf,_tmp6B2->tl=fields;_tmp6B2;});}}
# 3886
fields=({struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));({struct Cyc_Absyn_Aggrfield*_tmpAC6=({struct Cyc_Absyn_Aggrfield*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->name=Cyc_Toc_tag_sp,_tmp6B4->tq=Cyc_Toc_mt_tq,({
void*_tmpAC5=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp6B4->type=_tmpAC5;}),_tmp6B4->width=0,_tmp6B4->attributes=0,_tmp6B4->requires_clause=0;_tmp6B4;});
# 3886
_tmp6B5->hd=_tmpAC6;}),({
# 3888
struct Cyc_List_List*_tmpAC4=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(fields);_tmp6B5->tl=_tmpAC4;});_tmp6B5;});{
struct Cyc_Absyn_Aggrdecl*_tmp6B6=({struct _fat_ptr*_tmpAC8=({struct _fat_ptr*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct _fat_ptr _tmpAC7=({const char*_tmp6B9="";_tag_fat(_tmp6B9,sizeof(char),1U);});*_tmp6BA=_tmpAC7;});_tmp6BA;});Cyc_Toc_make_c_struct_defn(_tmpAC8,fields);});struct Cyc_Absyn_Aggrdecl*strct_decl=_tmp6B6;
({struct _tuple1*_tmpAC9=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_tmpAC9;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));({struct Cyc_Absyn_Decl*_tmpACA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->tag=5,_tmp6B7->f1=strct_decl;_tmp6B7;}),0U);_tmp6B8->hd=_tmpACA;}),_tmp6B8->tl=Cyc_Toc_result_decls;_tmp6B8;});
goto _LL6;}}}}else{if(*((int*)_tmp6AB)== 0){
# 3894
if((int)f->sc != (int)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6BB=Cyc_Absyn_string_exp(*fn,0U);struct Cyc_Absyn_Exp*initopt=_tmp6BB;
struct Cyc_Absyn_Vardecl*_tmp6BC=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);struct Cyc_Absyn_Vardecl*tag_decl=_tmp6BC;
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));({struct Cyc_Absyn_Decl*_tmpACB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD->tag=0,_tmp6BD->f1=tag_decl;_tmp6BD;}),0U);_tmp6BE->hd=_tmpACB;}),_tmp6BE->tl=Cyc_Toc_result_decls;_tmp6BE;});
({struct Cyc_Dict_Dict _tmpACC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert)(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_tmpACC;});}
# 3901
goto _LL6;}else{
goto _LL6;}}_LL6:;}}
# 3905
return 0;}}}}
# 3908
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3912
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 3918
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpACD=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpACD;});
# 3922
if((int)vd->sc == (int)Cyc_Absyn_Register && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if((int)vd->sc == (int)Cyc_Absyn_Static){
# 3931
struct _RegionHandle _tmp6BF=_new_region("temp");struct _RegionHandle*temp=& _tmp6BF;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6C0=Cyc_Toc_set_toplevel(temp,nv);struct Cyc_Toc_Env*nv2=_tmp6C0;
Cyc_Toc_exp_to_c(nv2,init);}
# 3932
;_pop_region();}else{
# 3936
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3939
void*_tmp6C1=Cyc_Absyn_compress(old_typ);void*_stmttmp70=_tmp6C1;void*_tmp6C2=_stmttmp70;void*_tmp6C5;struct Cyc_Absyn_Exp*_tmp6C4;void*_tmp6C3;if(*((int*)_tmp6C2)== 4){_tmp6C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C2)->f1).elt_type;_tmp6C4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C2)->f1).num_elts;_tmp6C5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C2)->f1).zero_term;{void*et=_tmp6C3;struct Cyc_Absyn_Exp*num_elts_opt=_tmp6C4;void*zt=_tmp6C5;
# 3941
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EB;_tmp7EB.tag=0,({struct _fat_ptr _tmpACE=({const char*_tmp6C8="can't initialize zero-terminated array -- size unknown";_tag_fat(_tmp6C8,sizeof(char),55U);});_tmp7EB.f1=_tmpACE;});_tmp7EB;});void*_tmp6C6[1];_tmp6C6[0]=& _tmp6C7;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp6C6,sizeof(void*),1));});{
struct Cyc_Absyn_Exp*_tmp6C9=({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpAD0,({
struct Cyc_Absyn_Exp*_tmpACF=num_elts_opt;Cyc_Absyn_add_exp(_tmpACF,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 3944
struct Cyc_Absyn_Exp*lhs=_tmp6C9;
# 3947
struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Absyn_signed_int_exp(0,0U);struct Cyc_Absyn_Exp*rhs=_tmp6CA;
({void*_tmpAD2=({struct Cyc_Absyn_Stmt*_tmpAD1=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAD1,
Cyc_Absyn_new_stmt(s->r,0U));});
# 3948
s->r=_tmpAD2;});}}
# 3951
goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 3959
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6CB=d;void**_tmp6CE;struct Cyc_List_List*_tmp6CD;struct Cyc_List_List*_tmp6CC;struct Cyc_Tcpat_Rhs*_tmp6CF;switch(*((int*)_tmp6CB)){case 0:
 return d;case 1: _tmp6CF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6CB)->f1;{struct Cyc_Tcpat_Rhs*rhs=_tmp6CF;
rhs->rhs=success;return d;}default: _tmp6CC=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CB)->f1;_tmp6CD=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CB)->f2;_tmp6CE=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CB)->f3;{struct Cyc_List_List*path=_tmp6CC;struct Cyc_List_List*sws=_tmp6CD;void**d2=_tmp6CE;
# 3964
({void*_tmpAD3=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAD3;});
for(1;sws != 0;sws=sws->tl){
struct _tuple34*_tmp6D0=(struct _tuple34*)sws->hd;struct _tuple34*_stmttmp71=_tmp6D0;struct _tuple34*_tmp6D1=_stmttmp71;void**_tmp6D2;_tmp6D2=(void**)& _tmp6D1->f2;{void**d2=_tmp6D2;
({void*_tmpAD4=Cyc_Toc_rewrite_decision(*d2,success);*d2=_tmpAD4;});}}
# 3969
return d;}};}
# 3980 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 3982
struct _RegionHandle _tmp6D3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6D3;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6D4=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_tmp6D4;
void*_tmp6D5=(void*)_check_null(e->topt);void*t=_tmp6D5;
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 3992
struct Cyc_Absyn_Stmt*_tmp6D6=s;struct Cyc_Absyn_Stmt*succ_stmt=_tmp6D6;
if(dopt != 0)
# 3996
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 3998
struct Cyc_Absyn_Switch_clause*_tmp6D7=({struct Cyc_Absyn_Switch_clause*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->pattern=p,_tmp6E6->pat_vars=0,_tmp6E6->where_clause=0,_tmp6E6->body=succ_stmt,_tmp6E6->loc=0U;_tmp6E6;});struct Cyc_Absyn_Switch_clause*c1=_tmp6D7;
struct Cyc_List_List*_tmp6D8=({struct _RegionHandle*_tmpAD7=rgn;struct _RegionHandle*_tmpAD6=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c)(_tmpAD7,Cyc_Toc_gen_labels,_tmpAD6,({struct Cyc_Absyn_Switch_clause*_tmp6E5[1];_tmp6E5[0]=c1;({struct _RegionHandle*_tmpAD5=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _fat_ptr))Cyc_List_rlist)(_tmpAD5,_tag_fat(_tmp6E5,sizeof(struct Cyc_Absyn_Switch_clause*),1));});}));});struct Cyc_List_List*lscs=_tmp6D8;
# 4001
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4005
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4008
struct Cyc_Toc_Env*senv;
for(1;1;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
if(mybodies == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EC;_tmp7EC.tag=0,({struct _fat_ptr _tmpAD8=({const char*_tmp6DB="letdecl_to_c: couldn't find env!";_tag_fat(_tmp6DB,sizeof(char),33U);});_tmp7EC.f1=_tmpAD8;});_tmp7EC;});void*_tmp6D9[1];_tmp6D9[0]=& _tmp6DA;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp6D9,sizeof(void*),1));});{
struct _tuple41*_tmp6DC=(struct _tuple41*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple41*_stmttmp72=_tmp6DC;struct _tuple41*_tmp6DD=_stmttmp72;struct Cyc_Absyn_Stmt*_tmp6DF;struct Cyc_Toc_Env*_tmp6DE;_tmp6DE=_tmp6DD->f1;_tmp6DF=_tmp6DD->f3;{struct Cyc_Toc_Env*env=_tmp6DE;struct Cyc_Absyn_Stmt*st=_tmp6DF;
if(st == succ_stmt){senv=env;break;}}}}
# 4017
Cyc_Toc_stmt_to_c(senv,s);{
# 4019
struct Cyc_Absyn_Stmt*res=test_tree;
# 4021
for(1;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple39 _tmp6E0=*((struct _tuple39*)((struct Cyc_List_List*)_check_null(mydecls))->hd);struct _tuple39 _stmttmp73=_tmp6E0;struct _tuple39 _tmp6E1=_stmttmp73;struct Cyc_Absyn_Vardecl*_tmp6E2;_tmp6E2=_tmp6E1.f2;{struct Cyc_Absyn_Vardecl*vd=_tmp6E2;
res=({struct Cyc_Absyn_Decl*_tmpAD9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3->tag=0,_tmp6E3->f1=vd;_tmp6E3;}),0U);Cyc_Absyn_decl_stmt(_tmpAD9,res,0U);});}}
# 4026
res=({struct _tuple1*_tmpADC=v;void*_tmpADB=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_tmpADA=e;Cyc_Absyn_declare_stmt(_tmpADC,_tmpADB,_tmpADA,res,0U);});{
struct Cyc_Absyn_Stmt*_tmp6E4=res;_npop_handler(0);return _tmp6E4;}}}}}
# 3983
;_pop_region();}
# 4035
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6E7=e->r;void*_stmttmp74=_tmp6E7;void*_tmp6E8=_stmttmp74;struct Cyc_List_List*_tmp6EA;struct Cyc_List_List*_tmp6E9;struct Cyc_Absyn_MallocInfo*_tmp6EB;struct Cyc_Absyn_Stmt*_tmp6EC;void**_tmp6ED;void**_tmp6EE;struct Cyc_List_List*_tmp6EF;struct Cyc_List_List*_tmp6F0;struct Cyc_List_List*_tmp6F2;void**_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F4;void**_tmp6F3;struct Cyc_List_List*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F9;struct Cyc_Absyn_Exp*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_List_List*_tmp706;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70F;switch(*((int*)_tmp6E8)){case 41: _tmp70F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;{struct Cyc_Absyn_Exp*e=_tmp70F;
_tmp70E=e;goto _LL4;}case 20: _tmp70E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp70E;
_tmp70D=e;goto _LL6;}case 21: _tmp70D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL6: {struct Cyc_Absyn_Exp*e=_tmp70D;
_tmp70C=e;goto _LL8;}case 22: _tmp70C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp70C;
_tmp70B=e;goto _LLA;}case 15: _tmp70B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp70B;
_tmp70A=e;goto _LLC;}case 11: _tmp70A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp70A;
_tmp709=e;goto _LLE;}case 12: _tmp709=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp709;
_tmp708=e;goto _LL10;}case 18: _tmp708=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp708;
_tmp707=e;goto _LL12;}case 5: _tmp707=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp707;
Cyc_Toc_exptypes_to_c(e1);goto _LL0;}case 3: _tmp706=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;{struct Cyc_List_List*es=_tmp706;
((void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 7: _tmp704=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp705=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;{struct Cyc_Absyn_Exp*e1=_tmp704;struct Cyc_Absyn_Exp*e2=_tmp705;
_tmp702=e1;_tmp703=e2;goto _LL18;}case 8: _tmp702=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp703=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp702;struct Cyc_Absyn_Exp*e2=_tmp703;
_tmp700=e1;_tmp701=e2;goto _LL1A;}case 9: _tmp700=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp701=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp700;struct Cyc_Absyn_Exp*e2=_tmp701;
_tmp6FE=e1;_tmp6FF=e2;goto _LL1C;}case 23: _tmp6FE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp6FF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp6FE;struct Cyc_Absyn_Exp*e2=_tmp6FF;
_tmp6FC=e1;_tmp6FD=e2;goto _LL1E;}case 35: _tmp6FC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp6FD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp6FC;struct Cyc_Absyn_Exp*e2=_tmp6FD;
_tmp6FA=e1;_tmp6FB=e2;goto _LL20;}case 4: _tmp6FA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp6FB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6E8)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp6FA;struct Cyc_Absyn_Exp*e2=_tmp6FB;
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);goto _LL0;}case 6: _tmp6F7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp6F8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;_tmp6F9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E8)->f3;{struct Cyc_Absyn_Exp*e1=_tmp6F7;struct Cyc_Absyn_Exp*e2=_tmp6F8;struct Cyc_Absyn_Exp*e3=_tmp6F9;
# 4054
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);goto _LL0;}case 10: _tmp6F5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp6F6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;{struct Cyc_Absyn_Exp*e=_tmp6F5;struct Cyc_List_List*es=_tmp6F6;
# 4056
Cyc_Toc_exptypes_to_c(e);((void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,es);goto _LL0;}case 14: _tmp6F3=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_tmp6F4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;{void**t=_tmp6F3;struct Cyc_Absyn_Exp*e=_tmp6F4;
({void*_tmpADD=Cyc_Toc_typ_to_c(*t);*t=_tmpADD;});Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 25: _tmp6F1=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1)->f3;_tmp6F2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;{void**t=_tmp6F1;struct Cyc_List_List*dles=_tmp6F2;
# 4059
({void*_tmpADE=Cyc_Toc_typ_to_c(*t);*t=_tmpADE;});
_tmp6F0=dles;goto _LL2A;}case 36: _tmp6F0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6E8)->f2;_LL2A: {struct Cyc_List_List*dles=_tmp6F0;
_tmp6EF=dles;goto _LL2C;}case 26: _tmp6EF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL2C: {struct Cyc_List_List*dles=_tmp6EF;
# 4063
for(1;dles != 0;dles=dles->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple19*)dles->hd)).f2);}
goto _LL0;}case 19: _tmp6EE=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;{void**t=_tmp6EE;
_tmp6ED=t;goto _LL30;}case 17: _tmp6ED=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;_LL30: {void**t=_tmp6ED;
({void*_tmpADF=Cyc_Toc_typ_to_c(*t);*t=_tmpADF;});goto _LL0;}case 37: _tmp6EC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;{struct Cyc_Absyn_Stmt*s=_tmp6EC;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 34: _tmp6EB=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6E8)->f1;{struct Cyc_Absyn_MallocInfo*m=_tmp6EB;
# 4070
if(m->elt_type != 0)
({void**_tmpAE1=({void**_tmp710=_cycalloc(sizeof(*_tmp710));({void*_tmpAE0=Cyc_Toc_typ_to_c(*((void**)_check_null(m->elt_type)));*_tmp710=_tmpAE0;});_tmp710;});m->elt_type=_tmpAE1;});
Cyc_Toc_exptypes_to_c(m->num_elts);
goto _LL0;}case 40: _tmp6E9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6E8)->f3;_tmp6EA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp6E8)->f4;{struct Cyc_List_List*o=_tmp6E9;struct Cyc_List_List*i=_tmp6EA;
# 4075
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL0;}case 0:
 goto _LL3A;case 1: _LL3A:
 goto _LL3C;case 32: _LL3C:
 goto _LL3E;case 33: _LL3E:
 goto _LL0;case 42:
# 4083
 goto _LL42;case 2: _LL42:
 goto _LL44;case 30: _LL44:
 goto _LL46;case 31: _LL46:
 goto _LL48;case 29: _LL48:
 goto _LL4A;case 27: _LL4A:
 goto _LL4C;case 28: _LL4C:
 goto _LL4E;case 24: _LL4E:
 goto _LL50;case 13: _LL50:
 goto _LL52;case 16: _LL52:
 goto _LL54;case 39: _LL54:
 goto _LL56;default: _LL56:
({void*_tmp711=0U;({unsigned _tmpAE3=e->loc;struct _fat_ptr _tmpAE2=({const char*_tmp712="Cyclone expression in C code";_tag_fat(_tmp712,sizeof(char),29U);});((int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAE3,_tmpAE2,_tag_fat(_tmp711,sizeof(void*),0));});});}_LL0:;}
# 4098
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp713=d->r;void*_stmttmp75=_tmp713;void*_tmp714=_stmttmp75;struct Cyc_Absyn_Typedefdecl*_tmp715;struct Cyc_Absyn_Enumdecl*_tmp716;struct Cyc_Absyn_Aggrdecl*_tmp717;struct Cyc_Absyn_Fndecl*_tmp718;struct Cyc_Absyn_Vardecl*_tmp719;switch(*((int*)_tmp714)){case 0: _tmp719=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp714)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp719;
# 4101
({void*_tmpAE4=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpAE4;});
if(vd->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1: _tmp718=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp714)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp718;
# 4105
({void*_tmpAE5=Cyc_Toc_typ_to_c((fd->i).ret_type);(fd->i).ret_type=_tmpAE5;});
{struct Cyc_List_List*_tmp71A=(fd->i).args;struct Cyc_List_List*args=_tmp71A;for(0;args != 0;args=args->tl){
({void*_tmpAE6=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f3);(*((struct _tuple9*)args->hd)).f3=_tmpAE6;});}}
goto _LL0;}case 5: _tmp717=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp714)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp717;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL0;}case 7: _tmp716=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp714)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp716;
# 4111
if(ed->fields != 0){
struct Cyc_List_List*_tmp71B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp71B;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp71C=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp71C;
if(f->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(f->tag));}}
# 4116
goto _LL0;}case 8: _tmp715=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp714)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp715;
({void*_tmpAE7=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpAE7;});goto _LL0;}case 2:
 goto _LLE;case 3: _LLE:
 goto _LL10;case 6: _LL10:
 goto _LL12;case 9: _LL12:
 goto _LL14;case 10: _LL14:
 goto _LL16;case 11: _LL16:
 goto _LL18;case 12: _LL18:
 goto _LL1A;case 4: _LL1A:
# 4126
({void*_tmp71D=0U;({unsigned _tmpAE9=d->loc;struct _fat_ptr _tmpAE8=({const char*_tmp71E="Cyclone declaration in C code";_tag_fat(_tmp71E,sizeof(char),30U);});((int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAE9,_tmpAE8,_tag_fat(_tmp71D,sizeof(void*),0));});});case 13:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 goto _LL0;}_LL0:;}
# 4134
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp71F=s->r;void*_stmttmp76=_tmp71F;void*_tmp720=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp721;struct Cyc_Absyn_Exp*_tmp723;struct Cyc_Absyn_Stmt*_tmp722;struct Cyc_Absyn_Stmt*_tmp725;struct Cyc_Absyn_Decl*_tmp724;void*_tmp728;struct Cyc_List_List*_tmp727;struct Cyc_Absyn_Exp*_tmp726;struct Cyc_Absyn_Stmt*_tmp72C;struct Cyc_Absyn_Exp*_tmp72B;struct Cyc_Absyn_Exp*_tmp72A;struct Cyc_Absyn_Exp*_tmp729;struct Cyc_Absyn_Stmt*_tmp72E;struct Cyc_Absyn_Exp*_tmp72D;struct Cyc_Absyn_Stmt*_tmp731;struct Cyc_Absyn_Stmt*_tmp730;struct Cyc_Absyn_Exp*_tmp72F;struct Cyc_Absyn_Exp*_tmp732;struct Cyc_Absyn_Stmt*_tmp734;struct Cyc_Absyn_Stmt*_tmp733;struct Cyc_Absyn_Exp*_tmp735;switch(*((int*)_tmp720)){case 1: _tmp735=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;{struct Cyc_Absyn_Exp*e=_tmp735;
Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 2: _tmp733=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;_tmp734=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp720)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp733;struct Cyc_Absyn_Stmt*s2=_tmp734;
Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 3: _tmp732=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp732;
if(eopt != 0)Cyc_Toc_exptypes_to_c(eopt);goto _LL0;}case 4: _tmp72F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;_tmp730=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp720)->f2;_tmp731=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp720)->f3;{struct Cyc_Absyn_Exp*e=_tmp72F;struct Cyc_Absyn_Stmt*s1=_tmp730;struct Cyc_Absyn_Stmt*s2=_tmp731;
# 4140
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s1);Cyc_Toc_stmttypes_to_c(s2);goto _LL0;}case 5: _tmp72D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp720)->f1).f1;_tmp72E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp720)->f2;{struct Cyc_Absyn_Exp*e=_tmp72D;struct Cyc_Absyn_Stmt*s=_tmp72E;
# 4142
Cyc_Toc_exptypes_to_c(e);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 9: _tmp729=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;_tmp72A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp720)->f2).f1;_tmp72B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp720)->f3).f1;_tmp72C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp720)->f4;{struct Cyc_Absyn_Exp*e1=_tmp729;struct Cyc_Absyn_Exp*e2=_tmp72A;struct Cyc_Absyn_Exp*e3=_tmp72B;struct Cyc_Absyn_Stmt*s=_tmp72C;
# 4144
Cyc_Toc_exptypes_to_c(e1);Cyc_Toc_exptypes_to_c(e2);Cyc_Toc_exptypes_to_c(e3);
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 10: _tmp726=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;_tmp727=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp720)->f2;_tmp728=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp720)->f3;{struct Cyc_Absyn_Exp*e=_tmp726;struct Cyc_List_List*scs=_tmp727;void*dec_tree=_tmp728;
# 4147
Cyc_Toc_exptypes_to_c(e);
for(1;scs != 0;scs=scs->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
goto _LL0;}case 12: _tmp724=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;_tmp725=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp720)->f2;{struct Cyc_Absyn_Decl*d=_tmp724;struct Cyc_Absyn_Stmt*s=_tmp725;
Cyc_Toc_decltypes_to_c(d);Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 14: _tmp722=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp720)->f1;_tmp723=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp720)->f2).f1;{struct Cyc_Absyn_Stmt*s=_tmp722;struct Cyc_Absyn_Exp*e=_tmp723;
Cyc_Toc_stmttypes_to_c(s);Cyc_Toc_exptypes_to_c(e);goto _LL0;}case 13: _tmp721=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp720)->f2;{struct Cyc_Absyn_Stmt*s=_tmp721;
Cyc_Toc_stmttypes_to_c(s);goto _LL0;}case 0:
 goto _LL18;case 6: _LL18:
 goto _LL1A;case 7: _LL1A:
 goto _LL1C;case 8: _LL1C:
 goto _LL0;case 11:
# 4159
({void*_tmpAEA=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736->tag=0;_tmp736;});s->r=_tmpAEA;});goto _LL0;default:
({void*_tmp737=0U;({unsigned _tmpAEC=s->loc;struct _fat_ptr _tmpAEB=({const char*_tmp738="Cyclone statement in C code";_tag_fat(_tmp738,sizeof(char),28U);});((int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc)(_tmpAEC,_tmpAEB,_tag_fat(_tmp737,sizeof(void*),0));});});}_LL0:;}
# 4167
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
# 4169
for(1;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7ED;_tmp7ED.tag=0,({struct _fat_ptr _tmpAED=({const char*_tmp73B="decls_to_c: not at toplevel!";_tag_fat(_tmp73B,sizeof(char),29U);});_tmp7ED.f1=_tmpAED;});_tmp7ED;});void*_tmp739[1];_tmp739[0]=& _tmp73A;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp739,sizeof(void*),1));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp73C=d->r;void*_stmttmp77=_tmp73C;void*_tmp73D=_stmttmp77;struct Cyc_List_List*_tmp73E;struct Cyc_List_List*_tmp73F;struct Cyc_List_List*_tmp740;struct Cyc_List_List*_tmp741;struct Cyc_Absyn_Typedefdecl*_tmp742;struct Cyc_Absyn_Enumdecl*_tmp743;struct Cyc_Absyn_Datatypedecl*_tmp744;struct Cyc_Absyn_Aggrdecl*_tmp745;struct Cyc_Absyn_Fndecl*_tmp746;struct Cyc_Absyn_Vardecl*_tmp747;switch(*((int*)_tmp73D)){case 0: _tmp747=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp747;
# 4176
struct _tuple1*_tmp748=vd->name;struct _tuple1*c_name=_tmp748;
# 4178
if((int)vd->sc == (int)Cyc_Absyn_ExternC)
c_name=({struct _tuple1*_tmp749=_cycalloc(sizeof(*_tmp749));({union Cyc_Absyn_Nmspace _tmpAEE=Cyc_Absyn_Abs_n(0,1);_tmp749->f1=_tmpAEE;}),_tmp749->f2=(*c_name).f2;_tmp749;});
if(vd->initializer != 0){
if((int)vd->sc == (int)Cyc_Absyn_ExternC)vd->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(vd->initializer));else{
# 4185
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));}}
# 4187
vd->name=c_name;
({enum Cyc_Absyn_Scope _tmpAEF=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_tmpAEF;});
({void*_tmpAF0=Cyc_Toc_typ_to_c(vd->type);vd->type=_tmpAF0;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->hd=d,_tmp74A->tl=Cyc_Toc_result_decls;_tmp74A;});
goto _LL0;}case 1: _tmp746=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp746;
# 4194
if((int)fd->sc == (int)Cyc_Absyn_ExternC){
({struct _tuple1*_tmpAF2=({struct _tuple1*_tmp74B=_cycalloc(sizeof(*_tmp74B));({union Cyc_Absyn_Nmspace _tmpAF1=Cyc_Absyn_Abs_n(0,1);_tmp74B->f1=_tmpAF1;}),_tmp74B->f2=(*fd->name).f2;_tmp74B;});fd->name=_tmpAF2;});
fd->sc=Cyc_Absyn_Public;}
# 4198
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->hd=d,_tmp74C->tl=Cyc_Toc_result_decls;_tmp74C;});
goto _LL0;}case 2:
 goto _LL8;case 3: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EE;_tmp7EE.tag=0,({struct _fat_ptr _tmpAF3=({const char*_tmp74F="letdecl at toplevel";_tag_fat(_tmp74F,sizeof(char),20U);});_tmp7EE.f1=_tmpAF3;});_tmp7EE;});void*_tmp74D[1];_tmp74D[0]=& _tmp74E;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp74D,sizeof(void*),1));});case 4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp751=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7EF;_tmp7EF.tag=0,({struct _fat_ptr _tmpAF4=({const char*_tmp752="region decl at toplevel";_tag_fat(_tmp752,sizeof(char),24U);});_tmp7EF.f1=_tmpAF4;});_tmp7EF;});void*_tmp750[1];_tmp750[0]=& _tmp751;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp750,sizeof(void*),1));});case 5: _tmp745=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_Absyn_Aggrdecl*sd=_tmp745;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6: _tmp744=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_tmp744;
# 4206
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7: _tmp743=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp743;
# 4209
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753->hd=d,_tmp753->tl=Cyc_Toc_result_decls;_tmp753;});
goto _LL0;}case 8: _tmp742=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp742;
# 4213
td->tvs=0;
if(td->defn != 0){
({void*_tmpAF5=Cyc_Toc_typ_to_c((void*)_check_null(td->defn));td->defn=_tmpAF5;});{
# 4218
struct Cyc_Absyn_Decl*ed;
{void*_tmp754=td->defn;void*_stmttmp78=_tmp754;void*_tmp755=_stmttmp78;unsigned _tmp757;struct Cyc_Absyn_Enumdecl*_tmp756;if(_tmp755 != 0){if(*((int*)_tmp755)== 10){if(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->f1)->r)== 1){_tmp756=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->f1)->r)->f1;_tmp757=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp755)->f1)->loc;{struct Cyc_Absyn_Enumdecl*ed2=_tmp756;unsigned loc=_tmp757;
# 4221
ed=({struct Cyc_Absyn_Decl*_tmp759=_cycalloc(sizeof(*_tmp759));({void*_tmpAF6=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758->tag=7,_tmp758->f1=ed2;_tmp758;});_tmp759->r=_tmpAF6;}),_tmp759->loc=loc;_tmp759;});goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26:
 ed=0;}_LL23:;}
# 4224
if(ed != 0){
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp75A=_cycalloc(sizeof(*_tmp75A));_tmp75A->hd=ed,_tmp75A->tl=Cyc_Toc_result_decls;_tmp75A;});{
void*_tmp75B=ed->r;void*_stmttmp79=_tmp75B;void*_tmp75C=_stmttmp79;struct Cyc_Absyn_Enumdecl*_tmp75D;if(*((int*)_tmp75C)== 7){_tmp75D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp75C)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp75D;
# 4228
({void*_tmpAF8=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->tag=0,({void*_tmpAF7=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->tag=15,_tmp75E->f1=ed->name,_tmp75E->f2=ed;_tmp75E;});_tmp75F->f1=_tmpAF7;}),_tmp75F->f2=0;_tmp75F;});td->defn=_tmpAF8;});goto _LL28;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp761=({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F0;_tmp7F0.tag=0,({struct _fat_ptr _tmpAF9=({const char*_tmp762="enumdecl to name";_tag_fat(_tmp762,sizeof(char),17U);});_tmp7F0.f1=_tmpAF9;});_tmp7F0;});void*_tmp760[1];_tmp760[0]=& _tmp761;((int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos)(_tag_fat(_tmp760,sizeof(void*),1));});}_LL28:;}}}}else{
# 4233
enum Cyc_Absyn_KindQual _tmp763=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind;enum Cyc_Absyn_KindQual _stmttmp7A=_tmp763;enum Cyc_Absyn_KindQual _tmp764=_stmttmp7A;if(_tmp764 == Cyc_Absyn_BoxKind){
({void*_tmpAFA=Cyc_Toc_void_star_type();td->defn=_tmpAFA;});goto _LL2D;}else{
td->defn=Cyc_Absyn_void_type;goto _LL2D;}_LL2D:;}
# 4240
if(Cyc_Flags_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765->hd=d,_tmp765->tl=Cyc_Toc_result_decls;_tmp765;});
goto _LL0;}case 13:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL0;case 9: _tmp741=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp73D)->f2;{struct Cyc_List_List*ds2=_tmp741;
_tmp740=ds2;goto _LL1E;}case 10: _tmp740=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp73D)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp740;
_tmp73F=ds2;goto _LL20;}case 11: _tmp73F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp73F;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,cinclude);goto _LL0;}default: _tmp73E=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp73D)->f1;{struct Cyc_List_List*ds2=_tmp73E;
nv=Cyc_Toc_decls_to_c(r,nv,ds2,1);goto _LL0;}}_LL0:;}}
# 4253
return nv;}
# 4258
void Cyc_Toc_init (void){
struct Cyc_Core_NewDynamicRegion _tmp766=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp7B=_tmp766;struct Cyc_Core_NewDynamicRegion _tmp767=_stmttmp7B;struct Cyc_Core_DynamicRegion*_tmp768;_tmp768=_tmp767.key;{struct Cyc_Core_DynamicRegion*dyn=_tmp768;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp76A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp76A));_tmp76A->dyn=dyn,_tmp76A->state=ts;_tmp76A;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _tmp76C=34;struct _fat_ptr**_tmp76B=_cycalloc(_check_times(_tmp76C,sizeof(struct _fat_ptr*)));_tmp76B[0]=& Cyc_Toc__throw_str,_tmp76B[1]=& Cyc_Toc_setjmp_str,_tmp76B[2]=& Cyc_Toc__push_handler_str,_tmp76B[3]=& Cyc_Toc__pop_handler_str,_tmp76B[4]=& Cyc_Toc__exn_thrown_str,_tmp76B[5]=& Cyc_Toc__npop_handler_str,_tmp76B[6]=& Cyc_Toc__check_null_str,_tmp76B[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp76B[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp76B[9]=& Cyc_Toc__check_fat_subscript_str,_tmp76B[10]=& Cyc_Toc__tag_fat_str,_tmp76B[11]=& Cyc_Toc__untag_fat_ptr_str,_tmp76B[12]=& Cyc_Toc__get_fat_size_str,_tmp76B[13]=& Cyc_Toc__get_zero_arr_size_str,_tmp76B[14]=& Cyc_Toc__fat_ptr_plus_str,_tmp76B[15]=& Cyc_Toc__zero_arr_plus_str,_tmp76B[16]=& Cyc_Toc__fat_ptr_inplace_plus_str,_tmp76B[17]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp76B[18]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_tmp76B[19]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp76B[20]=& Cyc_Toc__cycalloc_str,_tmp76B[21]=& Cyc_Toc__cyccalloc_str,_tmp76B[22]=& Cyc_Toc__cycalloc_atomic_str,_tmp76B[23]=& Cyc_Toc__cyccalloc_atomic_str,_tmp76B[24]=& Cyc_Toc__region_malloc_str,_tmp76B[25]=& Cyc_Toc__region_calloc_str,_tmp76B[26]=& Cyc_Toc__check_times_str,_tmp76B[27]=& Cyc_Toc__new_region_str,_tmp76B[28]=& Cyc_Toc__push_region_str,_tmp76B[29]=& Cyc_Toc__pop_region_str,_tmp76B[30]=& Cyc_Toc__throw_arraybounds_str,_tmp76B[31]=& Cyc_Toc__fat_ptr_decrease_size_str,_tmp76B[32]=& Cyc_Toc__throw_match_str,_tmp76B[33]=& Cyc_Toc__fast_region_malloc_str;_tmp76B;}),sizeof(struct _fat_ptr*),34U);}}
# 4306
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp76D=ts;struct Cyc_Toc_TocStateWrap*_tmp76E=Cyc_Toc_toc_state;ts=_tmp76E;Cyc_Toc_toc_state=_tmp76D;});{
struct Cyc_Toc_TocStateWrap _tmp76F=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _stmttmp7C=_tmp76F;struct Cyc_Toc_TocStateWrap _tmp770=_stmttmp7C;struct Cyc_Toc_TocState*_tmp772;struct Cyc_Core_DynamicRegion*_tmp771;_tmp771=_tmp770.dyn;_tmp772=_tmp770.state;{struct Cyc_Core_DynamicRegion*dyn=_tmp771;struct Cyc_Toc_TocState*s=_tmp772;
# 4311
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _tmp774=*s;struct Cyc_Toc_TocState _stmttmp7D=_tmp774;struct Cyc_Toc_TocState _tmp775=_stmttmp7D;struct Cyc_Xarray_Xarray*_tmp776;_tmp776=_tmp775.temp_labels;{struct Cyc_Xarray_Xarray*tls=_tmp776;
((void(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_reuse)(tls);}}
# 4312
;}
# 4315
Cyc_Core_free_rckey(dyn);
((void(*)(struct Cyc_Toc_TocStateWrap*))Cyc_Core_ufree)(ts);
# 4318
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4324
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4326
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp777=_cycalloc(sizeof(*_tmp777));*_tmp777=pop_tables;_tmp777;});
Cyc_Toc_init();{
struct _RegionHandle _tmp778=_new_region("start");struct _RegionHandle*start=& _tmp778;_push_region(start);
({struct _RegionHandle*_tmpAFC=start;struct Cyc_Toc_Env*_tmpAFB=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpAFC,_tmpAFB,ds,0);});{
struct Cyc_List_List*_tmp779=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp779;}
# 4329
;_pop_region();}}
