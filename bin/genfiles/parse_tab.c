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

/* Pools */
struct _pool; // defined in runtime_memory.c
struct _PoolHandle {
  struct _RuntimeStack s;
  struct _pool *p;
};
struct _PoolHandle _new_pool(void);
void _free_pool(struct _PoolHandle *h);
void _push_pool(struct _PoolHandle * r);
void _pop_pool(void);

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
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
extern void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 52
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 54
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 60
extern struct _fat_ptr Cyc_strcat(struct _fat_ptr dest,struct _fat_ptr src);
# 71
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr dest,struct _fat_ptr src);
# 109 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 291
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 304
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 311
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};
# 452
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 459
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Absyn_No_throw_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 699 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 862
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 892
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 902
void*Cyc_Absyn_compress(void*);
# 916
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 918
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 921
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 923
extern void*Cyc_Absyn_sint_type;
# 925
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 928
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 932
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 934
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 959
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 963
void*Cyc_Absyn_bounds_one (void);
# 965
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 985
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 988
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1065
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1087
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1091
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_pool_decl(struct Cyc_Absyn_Tvar*tv,unsigned loc);
# 1097
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1099
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1106
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1116
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1121
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 34
extern int Cyc_Flags_no_register;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
# 47
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 49
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 51
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 70
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr,unsigned);
# 78
void*Cyc_Kinds_compress_kb(void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 46 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 228
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 41 "attributes.h"
int Cyc_Atts_fntype_att(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 31
void*Cyc_CurRgn_curr_rgn_type (void);
# 33
struct _fat_ptr Cyc_CurRgn_curr_poolrgn_name;
# 35
void*Cyc_CurRgn_curr_poolrgn_type (void);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 98 "parse.y"
int Cyc_Parse_parsing_tempest=0;struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 104
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;{
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 115
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};
# 133
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple11{struct _tuple11*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _tuple13*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 170
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple13*ds,struct _tuple11**decls,struct Cyc_List_List**es){
# 173
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple12 _tmp0=ds->hd;struct _tuple12 _stmttmp0=_tmp0;struct _tuple12 _tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp3;struct Cyc_Parse_Declarator _tmp2;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2: {struct Cyc_Parse_Declarator d=_tmp2;struct Cyc_Absyn_Exp*e=_tmp3;
declarators=({struct _tuple11*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->tl=declarators,_tmp4->hd=d;_tmp4;});
exprs=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=e,_tmp5->tl=exprs;_tmp5;});}}
# 180
({struct Cyc_List_List*_tmp76F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp76F;});
({struct _tuple11*_tmp770=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp770;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 190
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 196
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 198
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 200
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=Cyc_Parse_Exit;_tmp6;}));}
# 204
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 206
struct _tuple8*_tmp7=tqt;void*_tmp9;struct Cyc_Absyn_Tqual _tmp8;_LL1: _tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp8;void*t=_tmp9;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc != (unsigned)0)loc=tq.loc;
({void*_tmpA=0U;({unsigned _tmp772=loc;struct _fat_ptr _tmp771=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp772,_tmp771,_tag_fat(_tmpA,sizeof(void*),0U));});});}
# 211
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;};
# 214
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 220
void*zeroterm=Cyc_Tcutil_any_bool(0);
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_stmttmp1=_tmpC;void*_tmpD=_stmttmp1;void*_tmpE;struct Cyc_Absyn_Exp*_tmpF;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7U: _LL5: _LL6:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 6U: _LL7: _LL8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3U: _LL9: _LLA:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2U: _LLB: _LLC:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0U: _LLD: _tmpF=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmpF;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _LLF: _tmpE=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10: {void*t=_tmpE;
rgn=t;goto _LL0;}}_LL0:;}
# 232
return({struct _tuple15 _tmp6CB;_tmp6CB.f1=nullable,_tmp6CB.f2=bound,_tmp6CB.f3=zeroterm,_tmp6CB.f4=rgn;_tmp6CB;});}
# 238
struct _tuple0*Cyc_Parse_gensym_enum (void){
# 240
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp776=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp776;}),({
struct _fat_ptr*_tmp775=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp774=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6CC;_tmp6CC.tag=1U,_tmp6CC.f1=(unsigned long)enum_counter ++;_tmp6CC;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp773=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp773,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp774;});_tmp13;});_tmp14->f2=_tmp775;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 245
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 250
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp778,_tmp777,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp77A,_tmp779,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 260
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6CD;_tmp6CD.Signed_spec=0,_tmp6CD.Unsigned_spec=0,_tmp6CD.Short_spec=0,_tmp6CD.Long_spec=0,_tmp6CD.Long_Long_spec=0,_tmp6CD.Valid_type_spec=0,_tmp6CD.Type_spec=Cyc_Absyn_sint_type,_tmp6CD.loc=loc;_tmp6CD;});}
# 271
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp23;
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 277
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp24;
s.Signed_spec=1;
return s;}
# 282
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp25;
s.Unsigned_spec=1;
return s;}
# 287
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp26;
s.Short_spec=1;
return s;}
# 292
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp27;
s.Long_spec=1;
return s;}
# 299
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 301
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp77B=t;Cyc_Tcutil_promote_array(_tmp77B,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 314 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 319
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 323
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp77D=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6CE;_tmp6CE.tag=0U,_tmp6CE.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6CE;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp77C=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp77C,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp77D;});_tmp36;});
({void*_tmp77F=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp77E=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Kinds_ik;_tmp31;});_tmp32->kind=_tmp77E;});_tmp32;}));*z=_tmp77F;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 328
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp780=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp780;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 332
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp782=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6CF;_tmp6CF.tag=0U,_tmp6CF.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6CF;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp781=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp781,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp782;});_tmp3E;});
({void*_tmp784=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp783=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Kinds_rk;_tmp39;});_tmp3A->kind=_tmp783;});_tmp3A;}));*z=_tmp784;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 337
return res;}
# 341
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;struct Cyc_List_List*res=_tmp3F;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_stmttmp3=_tmp40;void*_tmp41=_stmttmp3;void*_tmp42;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp42=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2: {void*i=_tmp42;
# 346
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp785=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp785;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 349
return res;}
# 353
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp45=e->r;void*_stmttmp4=_tmp45;void*_tmp46=_stmttmp4;struct _fat_ptr*_tmp47;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp47=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f2;_LL2: {struct _fat_ptr*y=_tmp47;
# 356
{struct Cyc_List_List*_tmp48=tags;struct Cyc_List_List*ts=_tmp48;for(0;ts != 0;ts=ts->tl){
struct _tuple19*_tmp49=(struct _tuple19*)ts->hd;struct _tuple19*_stmttmp5=_tmp49;struct _tuple19*_tmp4A=_stmttmp5;void*_tmp4C;struct _fat_ptr*_tmp4B;_LL6: _tmp4B=_tmp4A->f1;_tmp4C=_tmp4A->f2;_LL7: {struct _fat_ptr*x=_tmp4B;void*i=_tmp4C;
if(Cyc_strptrcmp(x,y)== 0)
return({void*_tmp787=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp786=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp786;});_tmp4D;});Cyc_Absyn_new_exp(_tmp787,e->loc);});}}}
# 361
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 364
return e;}
# 369
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp4F;void*_tmp50;struct Cyc_Absyn_PtrLoc*_tmp57;void*_tmp56;void*_tmp55;void*_tmp54;void*_tmp53;struct Cyc_Absyn_Tqual _tmp52;void*_tmp51;unsigned _tmp5C;void*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Tqual _tmp59;void*_tmp58;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp5A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp5B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp5C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {void*et=_tmp58;struct Cyc_Absyn_Tqual tq=_tmp59;struct Cyc_Absyn_Exp*nelts=_tmp5A;void*zt=_tmp5B;unsigned ztloc=_tmp5C;
# 372
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*_tmp5D=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5D;
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3U: _LL3: _tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp56=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp57=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_LL4: {void*et=_tmp51;struct Cyc_Absyn_Tqual tq=_tmp52;void*r=_tmp53;void*n=_tmp54;void*b=_tmp55;void*zt=_tmp56;struct Cyc_Absyn_PtrLoc*pl=_tmp57;
# 380
void*_tmp5E=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5E;
void*_tmp5F=Cyc_Parse_substitute_tags(tags,b);void*b2=_tmp5F;
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D0;_tmp6D0.elt_type=et2,_tmp6D0.elt_tq=tq,(_tmp6D0.ptr_atts).rgn=r,(_tmp6D0.ptr_atts).nullable=n,(_tmp6D0.ptr_atts).bounds=b2,(_tmp6D0.ptr_atts).zero_term=zt,(_tmp6D0.ptr_atts).ptrloc=pl;_tmp6D0;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp50=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {void*t=_tmp50;
# 386
void*_tmp60=Cyc_Parse_substitute_tags(tags,t);void*t2=_tmp60;
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp4F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp4F;
# 390
struct Cyc_Absyn_Exp*_tmp61=Cyc_Parse_substitute_tags_exp(tags,e);struct Cyc_Absyn_Exp*e2=_tmp61;
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 395
return t;}
# 400
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp788=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp788;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 407
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple8*t){
# 409
return({struct _tuple20*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=(*t).f2,_tmp62->f2=(*t).f3;_tmp62;});}
# 412
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp63=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp63)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 421
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp789=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp789,({const char*_tmp64="`H";_tag_fat(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp78A=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78A,({const char*_tmp65="`U";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp78B=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78B,({const char*_tmp66="`RC";_tag_fat(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_poolrgn_name)== 0)
return Cyc_CurRgn_curr_poolrgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr*_tmp78C=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp78C;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}
# 435
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp78D=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78D,({const char*_tmp69="`H";_tag_fat(_tmp69,sizeof(char),3U);}));})== 0)
({void*_tmp6A=0U;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp6B="bad occurrence of heap region";_tag_fat(_tmp6B,sizeof(char),30U);});Cyc_Warn_err(_tmp78F,_tmp78E,_tag_fat(_tmp6A,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp790=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp790,({const char*_tmp6C="`U";_tag_fat(_tmp6C,sizeof(char),3U);}));})== 0)
({void*_tmp6D=0U;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp6E="bad occurrence of unique region";_tag_fat(_tmp6E,sizeof(char),32U);});Cyc_Warn_err(_tmp792,_tmp791,_tag_fat(_tmp6D,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp793=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp793,({const char*_tmp6F="`RC";_tag_fat(_tmp6F,sizeof(char),4U);}));})== 0)
({void*_tmp70=0U;({unsigned _tmp795=loc;struct _fat_ptr _tmp794=({const char*_tmp71="bad occurrence of refcounted region";_tag_fat(_tmp71,sizeof(char),36U);});Cyc_Warn_err(_tmp795,_tmp794,_tag_fat(_tmp70,sizeof(void*),0U));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp72=0U;({unsigned _tmp797=loc;struct _fat_ptr _tmp796=({const char*_tmp73="bad occurrence of \"current\" region";_tag_fat(_tmp73,sizeof(char),35U);});Cyc_Warn_err(_tmp797,_tmp796,_tag_fat(_tmp72,sizeof(void*),0U));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_poolrgn_name)== 0)
({void*_tmp74=0U;({unsigned _tmp799=loc;struct _fat_ptr _tmp798=({const char*_tmp75="bad occurrence of \"current pool\" region";_tag_fat(_tmp75,sizeof(char),40U);});Cyc_Warn_err(_tmp799,_tmp798,_tag_fat(_tmp74,sizeof(void*),0U));});});}
# 452
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp76=t;struct Cyc_Absyn_Tvar*_tmp77;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp76)->tag == 2U){_LL1: _tmp77=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp76)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp77;
return pr;}}else{_LL3: _LL4:
({void*_tmp78=0U;({unsigned _tmp79B=loc;struct _fat_ptr _tmp79A=({const char*_tmp79="expecting a list of type variables, not types";_tag_fat(_tmp79,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79B,_tmp79A,_tag_fat(_tmp78,sizeof(void*),0U));});});}_LL0:;}
# 460
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp7A=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp7A;void*_tmp7B=_stmttmp6;void**_tmp7C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B)->tag == 2U){_LL1: _tmp7C=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B)->f1)->kind;_LL2: {void**cptr=_tmp7C;
# 463
void*_tmp7D=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp7=_tmp7D;void*_tmp7E=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7E)->tag == 1U){_LL6: _LL7:
# 465
({void*_tmp79C=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=2U,_tmp7F->f1=0,_tmp7F->f2=k;_tmp7F;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp79C;});
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 469
 return;}_LL0:;}
# 474
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 482
if(tms == 0)return 0;{
# 484
void*_tmp80=(void*)tms->hd;void*_stmttmp8=_tmp80;void*_tmp81=_stmttmp8;void*_tmp82;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp81)->tag == 3U){_LL1: _tmp82=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp81)->f1;_LL2: {void*args=_tmp82;
# 487
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 490
void*_tmp83=args;struct Cyc_List_List*_tmp84;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp83)->tag == 1U){_LL6: _LL7:
# 492
({void*_tmp85=0U;({unsigned _tmp79E=loc;struct _fat_ptr _tmp79D=({const char*_tmp86="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp86,sizeof(char),93U);});Cyc_Warn_warn(_tmp79E,_tmp79D,_tag_fat(_tmp85,sizeof(void*),0U));});});
# 494
return tms;}else{_LL8: _tmp84=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp83)->f1;_LL9: {struct Cyc_List_List*ids=_tmp84;
# 496
if(({int _tmp79F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp79F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp87=0U;({unsigned _tmp7A1=loc;struct _fat_ptr _tmp7A0=({const char*_tmp88="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp88,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A1,_tmp7A0,_tag_fat(_tmp87,sizeof(void*),0U));});});{
# 500
struct Cyc_List_List*rev_new_params=0;
for(0;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(0;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp89=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp89;
void*_tmp8A=x->r;void*_stmttmp9=_tmp8A;void*_tmp8B=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp8C;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B)->tag == 0U){_LLB: _tmp8C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp8C;
# 507
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({void*_tmp8D=0U;({unsigned _tmp7A3=x->loc;struct _fat_ptr _tmp7A2=({const char*_tmp8E="initializer found in parameter declaration";_tag_fat(_tmp8E,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A3,_tmp7A2,_tag_fat(_tmp8D,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp8F=0U;({unsigned _tmp7A5=x->loc;struct _fat_ptr _tmp7A4=({const char*_tmp90="namespaces forbidden in parameter declarations";_tag_fat(_tmp90,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A5,_tmp7A4,_tag_fat(_tmp8F,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(*_tmp92));({struct _tuple8*_tmp7A6=({struct _tuple8*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->f1=(*vd->name).f2,_tmp91->f2=vd->tq,_tmp91->f3=vd->type;_tmp91;});_tmp92->hd=_tmp7A6;}),_tmp92->tl=rev_new_params;_tmp92;});
# 515
goto L;}}else{_LLD: _LLE:
({void*_tmp93=0U;({unsigned _tmp7A8=x->loc;struct _fat_ptr _tmp7A7=({const char*_tmp94="nonvariable declaration in parameter type";_tag_fat(_tmp94,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A8,_tmp7A7,_tag_fat(_tmp93,sizeof(void*),0U));});});}_LLA:;}
# 519
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp97=({struct Cyc_String_pa_PrintArg_struct _tmp6D1;_tmp6D1.tag=0U,_tmp6D1.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6D1;});void*_tmp95[1U];_tmp95[0]=& _tmp97;({unsigned _tmp7AA=loc;struct _fat_ptr _tmp7A9=({const char*_tmp96="%s is not given a type";_tag_fat(_tmp96,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AA,_tmp7A9,_tag_fat(_tmp95,sizeof(void*),1U));});});}
# 522
return({struct Cyc_List_List*_tmp9A=_region_malloc(yy,sizeof(*_tmp9A));
({void*_tmp7AD=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp99=_region_malloc(yy,sizeof(*_tmp99));_tmp99->tag=3U,({void*_tmp7AC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp98=_region_malloc(yy,sizeof(*_tmp98));_tmp98->tag=1U,({struct Cyc_List_List*_tmp7AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp98->f1=_tmp7AB;}),_tmp98->f2=0,_tmp98->f3=0,_tmp98->f4=0,_tmp98->f5=0,_tmp98->f6=0,_tmp98->f7=0;_tmp98;});_tmp99->f1=_tmp7AC;});_tmp99;});_tmp9A->hd=_tmp7AD;}),_tmp9A->tl=0;_tmp9A;});}}}_LL5:;}
# 529
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp9B=_region_malloc(yy,sizeof(*_tmp9B));_tmp9B->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7AE=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp9B->tl=_tmp7AE;});_tmp9B;});}_LL0:;}}
# 537
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 541
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6D2;_tmp6D2.id=d.id,_tmp6D2.varloc=d.varloc,({struct Cyc_List_List*_tmp7AF=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6D2.tms=_tmp7AF;});_tmp6D2;});{
# 544
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 550
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 556
enum Cyc_Parse_Storage_class _tmp9C=dso->sc;enum Cyc_Parse_Storage_class _stmttmpA=_tmp9C;enum Cyc_Parse_Storage_class _tmp9D=_stmttmpA;switch(_tmp9D){case Cyc_Parse_None_sc: _LL1: _LL2:
 goto _LL0;case Cyc_Parse_Extern_sc: _LL3: _LL4:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL5: _LL6:
 sc=0U;goto _LL0;default: _LL7: _LL8:
({void*_tmp9E=0U;({unsigned _tmp7B1=loc;struct _fat_ptr _tmp7B0=({const char*_tmp9F="bad storage class on function";_tag_fat(_tmp9F,sizeof(char),30U);});Cyc_Warn_err(_tmp7B1,_tmp7B0,_tag_fat(_tmp9E,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 563
void*_tmpA0=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmpA0;
struct _tuple14 _tmpA1=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmpA1;struct _tuple14 _tmpA2=_stmttmpB;struct Cyc_List_List*_tmpA6;struct Cyc_List_List*_tmpA5;void*_tmpA4;struct Cyc_Absyn_Tqual _tmpA3;_LLA: _tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;_tmpA5=_tmpA2.f3;_tmpA6=_tmpA2.f4;_LLB: {struct Cyc_Absyn_Tqual fn_tqual=_tmpA3;void*fn_type=_tmpA4;struct Cyc_List_List*x=_tmpA5;struct Cyc_List_List*out_atts=_tmpA6;
# 568
if(x != 0)
({void*_tmpA7=0U;({unsigned _tmp7B3=loc;struct _fat_ptr _tmp7B2=({const char*_tmpA8="bad type params, ignoring";_tag_fat(_tmpA8,sizeof(char),26U);});Cyc_Warn_warn(_tmp7B3,_tmp7B2,_tag_fat(_tmpA7,sizeof(void*),0U));});});{
# 571
void*_tmpA9=fn_type;struct Cyc_Absyn_FnInfo _tmpAA;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA9)->tag == 5U){_LLD: _tmpAA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA9)->f1;_LLE: {struct Cyc_Absyn_FnInfo i=_tmpAA;
# 573
{struct Cyc_List_List*_tmpAB=i.args;struct Cyc_List_List*args2=_tmpAB;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpAC=0U;({unsigned _tmp7B5=loc;struct _fat_ptr _tmp7B4=({const char*_tmpAD="missing argument variable in function prototype";_tag_fat(_tmpAD,sizeof(char),48U);});Cyc_Warn_err(_tmp7B5,_tmp7B4,_tag_fat(_tmpAC,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7B7=({struct _fat_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));({struct _fat_ptr _tmp7B6=({const char*_tmpAE="?";_tag_fat(_tmpAE,sizeof(char),2U);});*_tmpAF=_tmp7B6;});_tmpAF;});(*((struct _tuple8*)args2->hd)).f1=_tmp7B7;});}}}
# 580
({struct Cyc_List_List*_tmp7B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7B8;});
return({struct Cyc_Absyn_Fndecl*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->sc=sc,_tmpB0->is_inline=is_inline,_tmpB0->name=d.id,_tmpB0->body=body,_tmpB0->i=i,_tmpB0->cached_type=0,_tmpB0->param_vardecls=0,_tmpB0->fn_vardecl=0,_tmpB0->orig_scope=sc;_tmpB0;});}}else{_LLF: _LL10:
# 584
({void*_tmpB1=0U;({unsigned _tmp7BA=loc;struct _fat_ptr _tmp7B9=({const char*_tmpB2="declarator is not a function prototype";_tag_fat(_tmpB2,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7BA,_tmp7B9,_tag_fat(_tmpB1,sizeof(void*),0U));});});}_LLC:;}}}}}static char _tmpB3[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 588
static struct _fat_ptr Cyc_Parse_msg1={_tmpB3,_tmpB3,_tmpB3 + 76U};static char _tmpB4[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 590
static struct _fat_ptr Cyc_Parse_msg2={_tmpB4,_tmpB4,_tmpB4 + 84U};
# 597
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 600
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpB5=0U;({unsigned _tmp7BC=loc;struct _fat_ptr _tmp7BB=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7BC,_tmp7BB,_tag_fat(_tmpB5,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpB6=0U;({unsigned _tmp7BE=loc;struct _fat_ptr _tmp7BD=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7BE,_tmp7BD,_tag_fat(_tmpB6,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpB7=0U;({unsigned _tmp7C0=loc;struct _fat_ptr _tmp7BF=({const char*_tmpB8="too many occurrences of short in specifiers";_tag_fat(_tmpB8,sizeof(char),44U);});Cyc_Warn_warn(_tmp7C0,_tmp7BF,_tag_fat(_tmpB7,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpB9=0U;({unsigned _tmp7C2=loc;struct _fat_ptr _tmp7C1=({const char*_tmpBA="too many occurrences of long in specifiers";_tag_fat(_tmpBA,sizeof(char),43U);});Cyc_Warn_warn(_tmp7C2,_tmp7C1,_tag_fat(_tmpB9,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpBB=0U;({unsigned _tmp7C4=loc;struct _fat_ptr _tmp7C3=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7C4,_tmp7C3,_tag_fat(_tmpBB,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 622
return s1;}
# 628
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 636
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpBC=0U;({unsigned _tmp7C6=loc;struct _fat_ptr _tmp7C5=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7C6,_tmp7C5,_tag_fat(_tmpBC,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpBD=0U;({unsigned _tmp7C8=loc;struct _fat_ptr _tmp7C7=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7C8,_tmp7C7,_tag_fat(_tmpBD,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 649
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpBE=0U;({unsigned _tmp7CA=loc;struct _fat_ptr _tmp7C9=({const char*_tmpBF="missing type within specifier";_tag_fat(_tmpBF,sizeof(char),30U);});Cyc_Warn_warn(_tmp7CA,_tmp7C9,_tag_fat(_tmpBE,sizeof(void*),0U));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 654
{void*_tmpC0=t;enum Cyc_Absyn_Size_of _tmpC2;enum Cyc_Absyn_Sign _tmpC1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)){case 1U: _LL1: _tmpC1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)->f1;_tmpC2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpC1;enum Cyc_Absyn_Size_of sz2=_tmpC2;
# 656
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 660
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2U: _LL3: _LL4:
# 664
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
# 668
 if(seen_sign)
({void*_tmpC3=0U;({unsigned _tmp7CC=loc;struct _fat_ptr _tmp7CB=({const char*_tmpC4="sign specification on non-integral type";_tag_fat(_tmpC4,sizeof(char),40U);});Cyc_Warn_err(_tmp7CC,_tmp7CB,_tag_fat(_tmpC3,sizeof(void*),0U));});});
if(seen_size)
({void*_tmpC5=0U;({unsigned _tmp7CE=loc;struct _fat_ptr _tmp7CD=({const char*_tmpC6="size qualifier on non-integral type";_tag_fat(_tmpC6,sizeof(char),36U);});Cyc_Warn_err(_tmp7CE,_tmp7CD,_tag_fat(_tmpC5,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 674
return t;}
# 677
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 681
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpC7=d.id;struct _tuple0*q=_tmpC7;
unsigned _tmpC8=d.varloc;unsigned varloc=_tmpC8;
struct _tuple14 _tmpC9=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpC9;struct _tuple14 _tmpCA=_stmttmpC;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCD;void*_tmpCC;struct Cyc_Absyn_Tqual _tmpCB;_LL1: _tmpCB=_tmpCA.f1;_tmpCC=_tmpCA.f2;_tmpCD=_tmpCA.f3;_tmpCE=_tmpCA.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpCB;void*new_typ=_tmpCC;struct Cyc_List_List*tvs=_tmpCD;struct Cyc_List_List*atts=_tmpCE;
# 688
struct Cyc_List_List*_tmpCF=ds->tl == 0?0:({struct _RegionHandle*_tmp7D2=r;struct Cyc_Absyn_Tqual _tmp7D1=tq;void*_tmp7D0=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7CF=ds->tl;Cyc_Parse_apply_tmss(_tmp7D2,_tmp7D1,_tmp7D0,_tmp7CF,shared_atts);});struct Cyc_List_List*tl=_tmpCF;
return({struct Cyc_List_List*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));({struct _tuple16*_tmp7D3=({struct _tuple16*_tmpD0=_region_malloc(r,sizeof(*_tmpD0));_tmpD0->f1=varloc,_tmpD0->f2=q,_tmpD0->f3=tq2,_tmpD0->f4=new_typ,_tmpD0->f5=tvs,_tmpD0->f6=atts;_tmpD0;});_tmpD1->hd=_tmp7D3;}),_tmpD1->tl=tl;_tmpD1;});}}}
# 692
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 695
if(tms == 0)return({struct _tuple14 _tmp6D3;_tmp6D3.f1=tq,_tmp6D3.f2=t,_tmp6D3.f3=0,_tmp6D3.f4=atts;_tmp6D3;});{
void*_tmpD2=(void*)tms->hd;void*_stmttmpD=_tmpD2;void*_tmpD3=_stmttmpD;struct Cyc_List_List*_tmpD5;unsigned _tmpD4;struct Cyc_Absyn_Tqual _tmpD7;struct Cyc_Absyn_PtrAtts _tmpD6;unsigned _tmpD9;struct Cyc_List_List*_tmpD8;void*_tmpDA;unsigned _tmpDD;void*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;unsigned _tmpDF;void*_tmpDE;switch(*((int*)_tmpD3)){case 0U: _LL1: _tmpDE=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD3)->f1;_tmpDF=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD3)->f2;_LL2: {void*zeroterm=_tmpDE;unsigned ztloc=_tmpDF;
# 698
return({struct Cyc_Absyn_Tqual _tmp7D6=Cyc_Absyn_empty_tqual(0U);void*_tmp7D5=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 698
struct Cyc_List_List*_tmp7D4=atts;Cyc_Parse_apply_tms(_tmp7D6,_tmp7D5,_tmp7D4,tms->tl);});}case 1U: _LL3: _tmpDB=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD3)->f1;_tmpDC=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD3)->f2;_tmpDD=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD3)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpDB;void*zeroterm=_tmpDC;unsigned ztloc=_tmpDD;
# 701
return({struct Cyc_Absyn_Tqual _tmp7D9=Cyc_Absyn_empty_tqual(0U);void*_tmp7D8=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 701
struct Cyc_List_List*_tmp7D7=atts;Cyc_Parse_apply_tms(_tmp7D9,_tmp7D8,_tmp7D7,tms->tl);});}case 3U: _LL5: _tmpDA=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD3)->f1;_LL6: {void*args=_tmpDA;
# 704
void*_tmpE0=args;unsigned _tmpE1;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_List_List*_tmpE6;void*_tmpE5;struct Cyc_Absyn_VarargInfo*_tmpE4;int _tmpE3;struct Cyc_List_List*_tmpE2;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->tag == 1U){_LLE: _tmpE2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f1;_tmpE3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f2;_tmpE4=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f3;_tmpE5=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f4;_tmpE6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f5;_tmpE7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f6;_tmpE8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE0)->f7;_LLF: {struct Cyc_List_List*args2=_tmpE2;int c_vararg=_tmpE3;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpE4;void*eff=_tmpE5;struct Cyc_List_List*rgn_po=_tmpE6;struct Cyc_Absyn_Exp*req=_tmpE7;struct Cyc_Absyn_Exp*ens=_tmpE8;
# 706
struct Cyc_List_List*typvars=0;
# 708
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=(void*)as->hd,_tmpE9->tl=fn_atts;_tmpE9;});else{
# 713
new_atts=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)as->hd,_tmpEA->tl=new_atts;_tmpEA;});}}}
# 715
if(tms->tl != 0){
void*_tmpEB=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpEB;void*_tmpEC=_stmttmpE;struct Cyc_List_List*_tmpED;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEC)->tag == 4U){_LL13: _tmpED=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEC)->f1;_LL14: {struct Cyc_List_List*ts=_tmpED;
# 718
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;}
# 724
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 729
args2=0;{
# 732
struct Cyc_List_List*_tmpEE=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpEE;
# 734
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 739
{struct Cyc_List_List*_tmpEF=args2;struct Cyc_List_List*a=_tmpEF;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpF0=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpF0;struct _tuple8*_tmpF1=_stmttmpF;void**_tmpF4;struct Cyc_Absyn_Tqual _tmpF3;struct _fat_ptr*_tmpF2;_LL18: _tmpF2=_tmpF1->f1;_tmpF3=_tmpF1->f2;_tmpF4=(void**)& _tmpF1->f3;_LL19: {struct _fat_ptr*vopt=_tmpF2;struct Cyc_Absyn_Tqual tq=_tmpF3;void**t=_tmpF4;
if(tags != 0)
({void*_tmp7DA=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7DA;});
({void*_tmp7DB=Cyc_Parse_array2ptr(*t,1);*t=_tmp7DB;});}}}
# 751
return({struct Cyc_Absyn_Tqual _tmp7DE=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7DD=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 751
struct Cyc_List_List*_tmp7DC=new_atts;Cyc_Parse_apply_tms(_tmp7DE,_tmp7DD,_tmp7DC,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpE1=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpE0)->f2;_LL11: {unsigned loc=_tmpE1;
# 758
({void*_tmpF5=0U;({unsigned _tmp7E0=loc;struct _fat_ptr _tmp7DF=({const char*_tmpF6="function declaration without parameter types";_tag_fat(_tmpF6,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7E0,_tmp7DF,_tag_fat(_tmpF5,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD3)->f1;_tmpD9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD3)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD8;unsigned loc=_tmpD9;
# 765
if(tms->tl == 0)
return({struct _tuple14 _tmp6D4;_tmp6D4.f1=tq,_tmp6D4.f2=t,_tmp6D4.f3=ts,_tmp6D4.f4=atts;_tmp6D4;});
# 770
({void*_tmpF7=0U;({unsigned _tmp7E2=loc;struct _fat_ptr _tmp7E1=({const char*_tmpF8="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF8,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7E2,_tmp7E1,_tag_fat(_tmpF7,sizeof(void*),0U));});});}case 2U: _LL9: _tmpD6=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD3)->f1;_tmpD7=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD3)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpD6;struct Cyc_Absyn_Tqual tq2=_tmpD7;
# 773
return({struct Cyc_Absyn_Tqual _tmp7E5=tq2;void*_tmp7E4=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D5;_tmp6D5.elt_type=t,_tmp6D5.elt_tq=tq,_tmp6D5.ptr_atts=ptratts;_tmp6D5;}));struct Cyc_List_List*_tmp7E3=atts;Cyc_Parse_apply_tms(_tmp7E5,_tmp7E4,_tmp7E3,tms->tl);});}default: _LLB: _tmpD4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD3)->f1;_tmpD5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD3)->f2;_LLC: {unsigned loc=_tmpD4;struct Cyc_List_List*atts2=_tmpD5;
# 778
return({struct Cyc_Absyn_Tqual _tmp7E8=tq;void*_tmp7E7=t;struct Cyc_List_List*_tmp7E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7E8,_tmp7E7,_tmp7E6,tms->tl);});}}_LL0:;}}
# 784
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 793
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpF9=t;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmpFE;void*_tmpFD;struct Cyc_Absyn_Tqual _tmpFC;struct _tuple0*_tmpFB;unsigned _tmpFA;_LL1: _tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;_tmpFC=_tmpF9->f3;_tmpFD=_tmpF9->f4;_tmpFE=_tmpF9->f5;_tmpFF=_tmpF9->f6;_LL2: {unsigned varloc=_tmpFA;struct _tuple0*x=_tmpFB;struct Cyc_Absyn_Tqual tq=_tmpFC;void*typ=_tmpFD;struct Cyc_List_List*tvs=_tmpFE;struct Cyc_List_List*atts=_tmpFF;
# 796
Cyc_Lex_register_typedef(x);{
# 798
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp100=typ;struct Cyc_Core_Opt*_tmp101;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp100)->tag == 1U){_LL4: _tmp101=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp100)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmp101;
# 802
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 807
return({void*_tmp7EA=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=9U,({struct Cyc_Absyn_Typedefdecl*_tmp7E9=({struct Cyc_Absyn_Typedefdecl*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->name=x,_tmp102->tvs=tvs,_tmp102->kind=kind,_tmp102->defn=type,_tmp102->atts=atts,_tmp102->tq=tq,_tmp102->extern_c=0;_tmp102;});_tmp103->f1=_tmp7E9;});_tmp103;});Cyc_Absyn_new_decl(_tmp7EA,loc);});}}}
# 814
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7EB=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=12U,_tmp104->f1=d,_tmp104->f2=s;_tmp104;});Cyc_Absyn_new_stmt(_tmp7EB,d->loc);});}
# 818
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 827
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 830
struct _RegionHandle _tmp105=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp105;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp106=ds;struct Cyc_List_List*_tmp109;struct Cyc_Parse_Type_specifier _tmp108;struct Cyc_Absyn_Tqual _tmp107;_LL1: _tmp107=_tmp106.tq;_tmp108=_tmp106.type_specs;_tmp109=_tmp106.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp107;struct Cyc_Parse_Type_specifier tss=_tmp108;struct Cyc_List_List*atts=_tmp109;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp10A=0U;({unsigned _tmp7ED=loc;struct _fat_ptr _tmp7EC=({const char*_tmp10B="inline qualifier on non-function definition";_tag_fat(_tmp10B,sizeof(char),44U);});Cyc_Warn_warn(_tmp7ED,_tmp7EC,_tag_fat(_tmp10A,sizeof(void*),0U));});});{
# 836
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmp10C=ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp10C;enum Cyc_Parse_Storage_class _tmp10D=_stmttmp10;switch(_tmp10D){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 s=Cyc_Flags_no_register?Cyc_Absyn_Public: Cyc_Absyn_Register;goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 s=1U;goto _LL3;default: _LL12: _LL13:
 goto _LL3;}_LL3:;}{
# 852
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 856
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 864
void*_tmp10E=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp10E;
if(declarators == 0){
# 868
void*_tmp10F=base_type;struct Cyc_List_List*_tmp110;struct _tuple0*_tmp111;struct Cyc_List_List*_tmp114;int _tmp113;struct _tuple0*_tmp112;struct Cyc_Absyn_Datatypedecl**_tmp115;struct Cyc_List_List*_tmp118;struct _tuple0*_tmp117;enum Cyc_Absyn_AggrKind _tmp116;struct Cyc_Absyn_Datatypedecl*_tmp119;struct Cyc_Absyn_Enumdecl*_tmp11A;struct Cyc_Absyn_Aggrdecl*_tmp11B;switch(*((int*)_tmp10F)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10F)->f1)->r)){case 0U: _LL15: _tmp11B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10F)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp11B;
# 870
({struct Cyc_List_List*_tmp7EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7EE;});
ad->sc=s;{
struct Cyc_List_List*_tmp11E=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Decl*_tmp7F0=({void*_tmp7EF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=6U,_tmp11C->f1=ad;_tmp11C;});Cyc_Absyn_new_decl(_tmp7EF,loc);});_tmp11D->hd=_tmp7F0;}),_tmp11D->tl=0;_tmp11D;});_npop_handler(0U);return _tmp11E;}}case 1U: _LL17: _tmp11A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10F)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp11A;
# 874
if(atts != 0)({void*_tmp11F=0U;({unsigned _tmp7F2=loc;struct _fat_ptr _tmp7F1=({const char*_tmp120="attributes on enum not supported";_tag_fat(_tmp120,sizeof(char),33U);});Cyc_Warn_err(_tmp7F2,_tmp7F1,_tag_fat(_tmp11F,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp123=({struct Cyc_List_List*_tmp122=_cycalloc(sizeof(*_tmp122));({struct Cyc_Absyn_Decl*_tmp7F4=({void*_tmp7F3=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->tag=8U,_tmp121->f1=ed;_tmp121;});Cyc_Absyn_new_decl(_tmp7F3,loc);});_tmp122->hd=_tmp7F4;}),_tmp122->tl=0;_tmp122;});_npop_handler(0U);return _tmp123;}}default: _LL19: _tmp119=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10F)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp119;
# 878
if(atts != 0)({void*_tmp124=0U;({unsigned _tmp7F6=loc;struct _fat_ptr _tmp7F5=({const char*_tmp125="attributes on datatypes not supported";_tag_fat(_tmp125,sizeof(char),38U);});Cyc_Warn_err(_tmp7F6,_tmp7F5,_tag_fat(_tmp124,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp128=({struct Cyc_List_List*_tmp127=_cycalloc(sizeof(*_tmp127));({struct Cyc_Absyn_Decl*_tmp7F8=({void*_tmp7F7=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=7U,_tmp126->f1=dd;_tmp126;});Cyc_Absyn_new_decl(_tmp7F7,loc);});_tmp127->hd=_tmp7F8;}),_tmp127->tl=0;_tmp127;});_npop_handler(0U);return _tmp128;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp116=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).UnknownAggr).val).f1;_tmp117=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).UnknownAggr).val).f2;_tmp118=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp116;struct _tuple0*n=_tmp117;struct Cyc_List_List*ts=_tmp118;
# 882
struct Cyc_List_List*_tmp129=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp129;
struct Cyc_Absyn_Aggrdecl*_tmp12A=({struct Cyc_Absyn_Aggrdecl*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->kind=k,_tmp130->sc=s,_tmp130->name=n,_tmp130->tvs=ts2,_tmp130->impl=0,_tmp130->attributes=0,_tmp130->expected_mem_kind=0;_tmp130;});struct Cyc_Absyn_Aggrdecl*ad=_tmp12A;
if(atts != 0)({void*_tmp12B=0U;({unsigned _tmp7FA=loc;struct _fat_ptr _tmp7F9=({const char*_tmp12C="bad attributes on type declaration";_tag_fat(_tmp12C,sizeof(char),35U);});Cyc_Warn_err(_tmp7FA,_tmp7F9,_tag_fat(_tmp12B,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12F=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct Cyc_Absyn_Decl*_tmp7FC=({void*_tmp7FB=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=6U,_tmp12D->f1=ad;_tmp12D;});Cyc_Absyn_new_decl(_tmp7FB,loc);});_tmp12E->hd=_tmp7FC;}),_tmp12E->tl=0;_tmp12E;});_npop_handler(0U);return _tmp12F;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp115=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp115;
# 887
if(atts != 0)({void*_tmp131=0U;({unsigned _tmp7FE=loc;struct _fat_ptr _tmp7FD=({const char*_tmp132="bad attributes on datatype";_tag_fat(_tmp132,sizeof(char),27U);});Cyc_Warn_err(_tmp7FE,_tmp7FD,_tag_fat(_tmp131,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp135=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));({struct Cyc_Absyn_Decl*_tmp800=({void*_tmp7FF=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=7U,_tmp133->f1=*tudp;_tmp133;});Cyc_Absyn_new_decl(_tmp7FF,loc);});_tmp134->hd=_tmp800;}),_tmp134->tl=0;_tmp134;});_npop_handler(0U);return _tmp135;}}}else{_LL1F: _tmp112=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).UnknownDatatype).val).name;_tmp113=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp114=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f2;_LL20: {struct _tuple0*n=_tmp112;int isx=_tmp113;struct Cyc_List_List*ts=_tmp114;
# 890
struct Cyc_List_List*_tmp136=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp136;
struct Cyc_Absyn_Decl*_tmp137=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp137;
if(atts != 0)({void*_tmp138=0U;({unsigned _tmp802=loc;struct _fat_ptr _tmp801=({const char*_tmp139="bad attributes on datatype";_tag_fat(_tmp139,sizeof(char),27U);});Cyc_Warn_err(_tmp802,_tmp801,_tag_fat(_tmp138,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13B=({struct Cyc_List_List*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->hd=tud,_tmp13A->tl=0;_tmp13A;});_npop_handler(0U);return _tmp13B;}}}case 15U: _LL21: _tmp111=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1;_LL22: {struct _tuple0*n=_tmp111;
# 895
struct Cyc_Absyn_Enumdecl*_tmp13C=({struct Cyc_Absyn_Enumdecl*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->sc=s,_tmp143->name=n,_tmp143->fields=0;_tmp143;});struct Cyc_Absyn_Enumdecl*ed=_tmp13C;
if(atts != 0)({void*_tmp13D=0U;({unsigned _tmp804=loc;struct _fat_ptr _tmp803=({const char*_tmp13E="bad attributes on enum";_tag_fat(_tmp13E,sizeof(char),23U);});Cyc_Warn_err(_tmp804,_tmp803,_tag_fat(_tmp13D,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_Absyn_Decl*_tmp806=({struct Cyc_Absyn_Decl*_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp805=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->tag=8U,_tmp13F->f1=ed;_tmp13F;});_tmp140->r=_tmp805;}),_tmp140->loc=loc;_tmp140;});_tmp141->hd=_tmp806;}),_tmp141->tl=0;_tmp141;});_npop_handler(0U);return _tmp142;}}case 16U: _LL23: _tmp110=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10F)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp110;
# 901
struct Cyc_Absyn_Enumdecl*_tmp144=({struct Cyc_Absyn_Enumdecl*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->sc=s,({struct _tuple0*_tmp808=Cyc_Parse_gensym_enum();_tmp14C->name=_tmp808;}),({struct Cyc_Core_Opt*_tmp807=({struct Cyc_Core_Opt*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->v=fs;_tmp14B;});_tmp14C->fields=_tmp807;});_tmp14C;});struct Cyc_Absyn_Enumdecl*ed=_tmp144;
if(atts != 0)({void*_tmp145=0U;({unsigned _tmp80A=loc;struct _fat_ptr _tmp809=({const char*_tmp146="bad attributes on enum";_tag_fat(_tmp146,sizeof(char),23U);});Cyc_Warn_err(_tmp80A,_tmp809,_tag_fat(_tmp145,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp14A=({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));({struct Cyc_Absyn_Decl*_tmp80C=({struct Cyc_Absyn_Decl*_tmp148=_cycalloc(sizeof(*_tmp148));({void*_tmp80B=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->tag=8U,_tmp147->f1=ed;_tmp147;});_tmp148->r=_tmp80B;}),_tmp148->loc=loc;_tmp148;});_tmp149->hd=_tmp80C;}),_tmp149->tl=0;_tmp149;});_npop_handler(0U);return _tmp14A;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp14D=0U;({unsigned _tmp80E=loc;struct _fat_ptr _tmp80D=({const char*_tmp14E="missing declarator";_tag_fat(_tmp14E,sizeof(char),19U);});Cyc_Warn_err(_tmp80E,_tmp80D,_tag_fat(_tmp14D,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp14F=0;_npop_handler(0U);return _tmp14F;}}_LL14:;}{
# 908
struct Cyc_List_List*_tmp150=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp150;
if(istypedef){
# 913
if(!exps_empty)
({void*_tmp151=0U;({unsigned _tmp810=loc;struct _fat_ptr _tmp80F=({const char*_tmp152="initializer in typedef declaration";_tag_fat(_tmp152,sizeof(char),35U);});Cyc_Warn_err(_tmp810,_tmp80F,_tag_fat(_tmp151,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp153=decls;_npop_handler(0U);return _tmp153;}}{
# 919
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp154=fields;struct Cyc_List_List*ds=_tmp154;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp155=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp155;struct _tuple16*_tmp156=_stmttmp11;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp15B;void*_tmp15A;struct Cyc_Absyn_Tqual _tmp159;struct _tuple0*_tmp158;unsigned _tmp157;_LL28: _tmp157=_tmp156->f1;_tmp158=_tmp156->f2;_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;_tmp15B=_tmp156->f5;_tmp15C=_tmp156->f6;_LL29: {unsigned varloc=_tmp157;struct _tuple0*x=_tmp158;struct Cyc_Absyn_Tqual tq2=_tmp159;void*t2=_tmp15A;struct Cyc_List_List*tvs2=_tmp15B;struct Cyc_List_List*atts2=_tmp15C;
if(tvs2 != 0)
({void*_tmp15D=0U;({unsigned _tmp812=loc;struct _fat_ptr _tmp811=({const char*_tmp15E="bad type params, ignoring";_tag_fat(_tmp15E,sizeof(char),26U);});Cyc_Warn_warn(_tmp812,_tmp811,_tag_fat(_tmp15D,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp15F=0U;({unsigned _tmp814=loc;struct _fat_ptr _tmp813=({const char*_tmp160="unexpected NULL in parse!";_tag_fat(_tmp160,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp814,_tmp813,_tag_fat(_tmp15F,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp161=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp161;
struct Cyc_Absyn_Vardecl*_tmp162=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp162;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp163=({struct Cyc_Absyn_Decl*_tmp166=_cycalloc(sizeof(*_tmp166));({void*_tmp815=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=0U,_tmp165->f1=vd;_tmp165;});_tmp166->r=_tmp815;}),_tmp166->loc=loc;_tmp166;});struct Cyc_Absyn_Decl*d=_tmp163;
decls=({struct Cyc_List_List*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->hd=d,_tmp164->tl=decls;_tmp164;});}}}}}{
# 934
struct Cyc_List_List*_tmp167=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp167;}}}}}}}}}
# 831
;_pop_region();}
# 938
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp168=e->r;void*_stmttmp12=_tmp168;void*_tmp169=_stmttmp12;int _tmp16A;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp169)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp169)->f1).Int_c).tag == 5){_LL1: _tmp16A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp169)->f1).Int_c).val).f2;_LL2: {int i=_tmp16A;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 942
({void*_tmp16B=0U;({unsigned _tmp817=loc;struct _fat_ptr _tmp816=({const char*_tmp16C="expecting integer constant";_tag_fat(_tmp16C,sizeof(char),27U);});Cyc_Warn_err(_tmp817,_tmp816,_tag_fat(_tmp16B,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 948
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16D=e->r;void*_stmttmp13=_tmp16D;void*_tmp16E=_stmttmp13;struct _fat_ptr _tmp16F;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16E)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16E)->f1).String_c).tag == 8){_LL1: _tmp16F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16E)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp16F;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 952
({void*_tmp170=0U;({unsigned _tmp819=loc;struct _fat_ptr _tmp818=({const char*_tmp171="expecting string constant";_tag_fat(_tmp171,sizeof(char),26U);});Cyc_Warn_err(_tmp819,_tmp818,_tag_fat(_tmp170,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 958
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp172=x;long long _tmp173;char _tmp174;int _tmp175;switch((_tmp172.LongLong_c).tag){case 5U: _LL1: _tmp175=((_tmp172.Int_c).val).f2;_LL2: {int i=_tmp175;
return(unsigned)i;}case 2U: _LL3: _tmp174=((_tmp172.Char_c).val).f2;_LL4: {char c=_tmp174;
return(unsigned)c;}case 6U: _LL5: _tmp173=((_tmp172.LongLong_c).val).f2;_LL6: {long long x=_tmp173;
# 963
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp176=0U;({unsigned _tmp81B=loc;struct _fat_ptr _tmp81A=({const char*_tmp177="integer constant too large";_tag_fat(_tmp177,sizeof(char),27U);});Cyc_Warn_err(_tmp81B,_tmp81A,_tag_fat(_tmp176,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 968
({struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp6D6;_tmp6D6.tag=0U,({struct _fat_ptr _tmp81C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6D6.f1=_tmp81C;});_tmp6D6;});void*_tmp178[1U];_tmp178[0]=& _tmp17A;({unsigned _tmp81E=loc;struct _fat_ptr _tmp81D=({const char*_tmp179="expected integer constant but found %s";_tag_fat(_tmp179,sizeof(char),39U);});Cyc_Warn_err(_tmp81E,_tmp81D,_tag_fat(_tmp178,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 974
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17B=p->r;void*_stmttmp14=_tmp17B;void*_tmp17C=_stmttmp14;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_List_List*_tmp17F;struct _tuple0*_tmp17E;int _tmp181;struct _fat_ptr _tmp180;char _tmp182;int _tmp184;enum Cyc_Absyn_Sign _tmp183;struct Cyc_Absyn_Pat*_tmp185;struct Cyc_Absyn_Vardecl*_tmp186;struct _tuple0*_tmp187;switch(*((int*)_tmp17C)){case 15U: _LL1: _tmp187=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_LL2: {struct _tuple0*x=_tmp187;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17C)->f2)->r)->tag == 0U){_LL3: _tmp186=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp186;
# 978
return({struct Cyc_Absyn_Exp*_tmp81F=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp81F,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp185=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp185;
return({struct Cyc_Absyn_Exp*_tmp820=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp820,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp183=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_tmp184=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17C)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp183;int i=_tmp184;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp182=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_LLC: {char c=_tmp182;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp180=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_tmp181=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17C)->f2;_LLE: {struct _fat_ptr s=_tmp180;int i=_tmp181;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17C)->f3 == 0){_LLF: _tmp17E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17C)->f2;_LL10: {struct _tuple0*x=_tmp17E;struct Cyc_List_List*ps=_tmp17F;
# 985
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp17D=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17C)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp17D;
return e;}default: _LL13: _LL14:
# 990
({void*_tmp188=0U;({unsigned _tmp822=p->loc;struct _fat_ptr _tmp821=({const char*_tmp189="cannot mix patterns and expressions in case";_tag_fat(_tmp189,sizeof(char),44U);});Cyc_Warn_err(_tmp822,_tmp821,_tag_fat(_tmp188,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1075
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->tag=Cyc_Core_Failure,_tmp18A->f1=s;_tmp18A;}));}static char _tmp18D[7U]="cnst_t";
# 1047 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18D,_tmp18D,_tmp18D + 7U};
union Cyc_YYSTYPE*_tmp18B=yy1;union Cyc_Absyn_Cnst _tmp18C;if((((union Cyc_YYSTYPE*)_tmp18B)->Int_tok).tag == 1){_LL1: _tmp18C=(_tmp18B->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp18C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1054
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.Int_tok).tag=1U,(_tmp6D7.Int_tok).val=yy1;_tmp6D7;});}static char _tmp190[5U]="char";
# 1048 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp190,_tmp190,_tmp190 + 5U};
union Cyc_YYSTYPE*_tmp18E=yy1;char _tmp18F;if((((union Cyc_YYSTYPE*)_tmp18E)->Char_tok).tag == 2){_LL1: _tmp18F=(_tmp18E->Char_tok).val;_LL2: {char yy=_tmp18F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1055
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.Char_tok).tag=2U,(_tmp6D8.Char_tok).val=yy1;_tmp6D8;});}static char _tmp193[13U]="string_t<`H>";
# 1049 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp193,_tmp193,_tmp193 + 13U};
union Cyc_YYSTYPE*_tmp191=yy1;struct _fat_ptr _tmp192;if((((union Cyc_YYSTYPE*)_tmp191)->String_tok).tag == 3){_LL1: _tmp192=(_tmp191->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp192;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1056
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.String_tok).tag=3U,(_tmp6D9.String_tok).val=yy1;_tmp6D9;});}static char _tmp196[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1052 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp196,_tmp196,_tmp196 + 35U};
union Cyc_YYSTYPE*_tmp194=yy1;struct _tuple22*_tmp195;if((((union Cyc_YYSTYPE*)_tmp194)->YY1).tag == 9){_LL1: _tmp195=(_tmp194->YY1).val;_LL2: {struct _tuple22*yy=_tmp195;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1059
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.YY1).tag=9U,(_tmp6DA.YY1).val=yy1;_tmp6DA;});}static char _tmp199[11U]="ptrbound_t";
# 1053 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp199,_tmp199,_tmp199 + 11U};
union Cyc_YYSTYPE*_tmp197=yy1;void*_tmp198;if((((union Cyc_YYSTYPE*)_tmp197)->YY2).tag == 10){_LL1: _tmp198=(_tmp197->YY2).val;_LL2: {void*yy=_tmp198;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1060
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY2).tag=10U,(_tmp6DB.YY2).val=yy1;_tmp6DB;});}static char _tmp19C[28U]="list_t<offsetof_field_t,`H>";
# 1054 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19C,_tmp19C,_tmp19C + 28U};
union Cyc_YYSTYPE*_tmp19A=yy1;struct Cyc_List_List*_tmp19B;if((((union Cyc_YYSTYPE*)_tmp19A)->YY3).tag == 11){_LL1: _tmp19B=(_tmp19A->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp19B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1061
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY3).tag=11U,(_tmp6DC.YY3).val=yy1;_tmp6DC;});}static char _tmp19F[6U]="exp_t";
# 1055 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19F,_tmp19F,_tmp19F + 6U};
union Cyc_YYSTYPE*_tmp19D=yy1;struct Cyc_Absyn_Exp*_tmp19E;if((((union Cyc_YYSTYPE*)_tmp19D)->Exp_tok).tag == 7){_LL1: _tmp19E=(_tmp19D->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp19E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1062
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.Exp_tok).tag=7U,(_tmp6DD.Exp_tok).val=yy1;_tmp6DD;});}static char _tmp1A2[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A2,_tmp1A2,_tmp1A2 + 17U};
union Cyc_YYSTYPE*_tmp1A0=yy1;struct Cyc_List_List*_tmp1A1;if((((union Cyc_YYSTYPE*)_tmp1A0)->YY4).tag == 12){_LL1: _tmp1A1=(_tmp1A0->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp1A1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1070
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY4).tag=12U,(_tmp6DE.YY4).val=yy1;_tmp6DE;});}static char _tmp1A5[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1064 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A5,_tmp1A5,_tmp1A5 + 47U};
union Cyc_YYSTYPE*_tmp1A3=yy1;struct Cyc_List_List*_tmp1A4;if((((union Cyc_YYSTYPE*)_tmp1A3)->YY5).tag == 13){_LL1: _tmp1A4=(_tmp1A3->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1A4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1071
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY5).tag=13U,(_tmp6DF.YY5).val=yy1;_tmp6DF;});}static char _tmp1A8[9U]="primop_t";
# 1065 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A8,_tmp1A8,_tmp1A8 + 9U};
union Cyc_YYSTYPE*_tmp1A6=yy1;enum Cyc_Absyn_Primop _tmp1A7;if((((union Cyc_YYSTYPE*)_tmp1A6)->YY6).tag == 14){_LL1: _tmp1A7=(_tmp1A6->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1A7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1072
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY6).tag=14U,(_tmp6E0.YY6).val=yy1;_tmp6E0;});}static char _tmp1AB[19U]="opt_t<primop_t,`H>";
# 1066 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AB,_tmp1AB,_tmp1AB + 19U};
union Cyc_YYSTYPE*_tmp1A9=yy1;struct Cyc_Core_Opt*_tmp1AA;if((((union Cyc_YYSTYPE*)_tmp1A9)->YY7).tag == 15){_LL1: _tmp1AA=(_tmp1A9->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1AA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1073
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY7).tag=15U,(_tmp6E1.YY7).val=yy1;_tmp6E1;});}static char _tmp1AE[7U]="qvar_t";
# 1067 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AE,_tmp1AE,_tmp1AE + 7U};
union Cyc_YYSTYPE*_tmp1AC=yy1;struct _tuple0*_tmp1AD;if((((union Cyc_YYSTYPE*)_tmp1AC)->QualId_tok).tag == 5){_LL1: _tmp1AD=(_tmp1AC->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1AD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1074
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.QualId_tok).tag=5U,(_tmp6E2.QualId_tok).val=yy1;_tmp6E2;});}static char _tmp1B1[7U]="stmt_t";
# 1070 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B1,_tmp1B1,_tmp1B1 + 7U};
union Cyc_YYSTYPE*_tmp1AF=yy1;struct Cyc_Absyn_Stmt*_tmp1B0;if((((union Cyc_YYSTYPE*)_tmp1AF)->Stmt_tok).tag == 8){_LL1: _tmp1B0=(_tmp1AF->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1B0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1077
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.Stmt_tok).tag=8U,(_tmp6E3.Stmt_tok).val=yy1;_tmp6E3;});}static char _tmp1B4[27U]="list_t<switch_clause_t,`H>";
# 1074 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B4,_tmp1B4,_tmp1B4 + 27U};
union Cyc_YYSTYPE*_tmp1B2=yy1;struct Cyc_List_List*_tmp1B3;if((((union Cyc_YYSTYPE*)_tmp1B2)->YY8).tag == 16){_LL1: _tmp1B3=(_tmp1B2->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1B3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1081
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY8).tag=16U,(_tmp6E4.YY8).val=yy1;_tmp6E4;});}static char _tmp1B7[6U]="pat_t";
# 1075 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B7,_tmp1B7,_tmp1B7 + 6U};
union Cyc_YYSTYPE*_tmp1B5=yy1;struct Cyc_Absyn_Pat*_tmp1B6;if((((union Cyc_YYSTYPE*)_tmp1B5)->YY9).tag == 17){_LL1: _tmp1B6=(_tmp1B5->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1B6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1082
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY9).tag=17U,(_tmp6E5.YY9).val=yy1;_tmp6E5;});}static char _tmp1BA[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1080 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BA,_tmp1BA,_tmp1BA + 28U};
union Cyc_YYSTYPE*_tmp1B8=yy1;struct _tuple23*_tmp1B9;if((((union Cyc_YYSTYPE*)_tmp1B8)->YY10).tag == 18){_LL1: _tmp1B9=(_tmp1B8->YY10).val;_LL2: {struct _tuple23*yy=_tmp1B9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1087
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY10).tag=18U,(_tmp6E6.YY10).val=yy1;_tmp6E6;});}static char _tmp1BD[17U]="list_t<pat_t,`H>";
# 1081 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BD,_tmp1BD,_tmp1BD + 17U};
union Cyc_YYSTYPE*_tmp1BB=yy1;struct Cyc_List_List*_tmp1BC;if((((union Cyc_YYSTYPE*)_tmp1BB)->YY11).tag == 19){_LL1: _tmp1BC=(_tmp1BB->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1BC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1088
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY11).tag=19U,(_tmp6E7.YY11).val=yy1;_tmp6E7;});}static char _tmp1C0[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1082 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C0,_tmp1C0,_tmp1C0 + 36U};
union Cyc_YYSTYPE*_tmp1BE=yy1;struct _tuple24*_tmp1BF;if((((union Cyc_YYSTYPE*)_tmp1BE)->YY12).tag == 20){_LL1: _tmp1BF=(_tmp1BE->YY12).val;_LL2: {struct _tuple24*yy=_tmp1BF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1089
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY12).tag=20U,(_tmp6E8.YY12).val=yy1;_tmp6E8;});}static char _tmp1C3[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1083 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C3,_tmp1C3,_tmp1C3 + 47U};
union Cyc_YYSTYPE*_tmp1C1=yy1;struct Cyc_List_List*_tmp1C2;if((((union Cyc_YYSTYPE*)_tmp1C1)->YY13).tag == 21){_LL1: _tmp1C2=(_tmp1C1->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1C2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1090
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY13).tag=21U,(_tmp6E9.YY13).val=yy1;_tmp6E9;});}static char _tmp1C6[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1084 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C6,_tmp1C6,_tmp1C6 + 58U};
union Cyc_YYSTYPE*_tmp1C4=yy1;struct _tuple23*_tmp1C5;if((((union Cyc_YYSTYPE*)_tmp1C4)->YY14).tag == 22){_LL1: _tmp1C5=(_tmp1C4->YY14).val;_LL2: {struct _tuple23*yy=_tmp1C5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1091
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY14).tag=22U,(_tmp6EA.YY14).val=yy1;_tmp6EA;});}static char _tmp1C9[9U]="fndecl_t";
# 1085 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C9,_tmp1C9,_tmp1C9 + 9U};
union Cyc_YYSTYPE*_tmp1C7=yy1;struct Cyc_Absyn_Fndecl*_tmp1C8;if((((union Cyc_YYSTYPE*)_tmp1C7)->YY15).tag == 23){_LL1: _tmp1C8=(_tmp1C7->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1C8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1092
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY15).tag=23U,(_tmp6EB.YY15).val=yy1;_tmp6EB;});}static char _tmp1CC[18U]="list_t<decl_t,`H>";
# 1086 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CC,_tmp1CC,_tmp1CC + 18U};
union Cyc_YYSTYPE*_tmp1CA=yy1;struct Cyc_List_List*_tmp1CB;if((((union Cyc_YYSTYPE*)_tmp1CA)->YY16).tag == 24){_LL1: _tmp1CB=(_tmp1CA->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1CB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1093
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY16).tag=24U,(_tmp6EC.YY16).val=yy1;_tmp6EC;});}static char _tmp1CF[12U]="decl_spec_t";
# 1089 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CF,_tmp1CF,_tmp1CF + 12U};
union Cyc_YYSTYPE*_tmp1CD=yy1;struct Cyc_Parse_Declaration_spec _tmp1CE;if((((union Cyc_YYSTYPE*)_tmp1CD)->YY17).tag == 25){_LL1: _tmp1CE=(_tmp1CD->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1CE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1096
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY17).tag=25U,(_tmp6ED.YY17).val=yy1;_tmp6ED;});}static char _tmp1D2[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1090 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D2,_tmp1D2,_tmp1D2 + 31U};
union Cyc_YYSTYPE*_tmp1D0=yy1;struct _tuple12 _tmp1D1;if((((union Cyc_YYSTYPE*)_tmp1D0)->YY18).tag == 26){_LL1: _tmp1D1=(_tmp1D0->YY18).val;_LL2: {struct _tuple12 yy=_tmp1D1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1097
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY18).tag=26U,(_tmp6EE.YY18).val=yy1;_tmp6EE;});}static char _tmp1D5[23U]="declarator_list_t<`yy>";
# 1091 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D5,_tmp1D5,_tmp1D5 + 23U};
union Cyc_YYSTYPE*_tmp1D3=yy1;struct _tuple13*_tmp1D4;if((((union Cyc_YYSTYPE*)_tmp1D3)->YY19).tag == 27){_LL1: _tmp1D4=(_tmp1D3->YY19).val;_LL2: {struct _tuple13*yy=_tmp1D4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1098
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY19).tag=27U,(_tmp6EF.YY19).val=yy1;_tmp6EF;});}static char _tmp1D8[16U]="storage_class_t";
# 1092 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D8,_tmp1D8,_tmp1D8 + 16U};
union Cyc_YYSTYPE*_tmp1D6=yy1;enum Cyc_Parse_Storage_class _tmp1D7;if((((union Cyc_YYSTYPE*)_tmp1D6)->YY20).tag == 28){_LL1: _tmp1D7=(_tmp1D6->YY20).val;_LL2: {enum Cyc_Parse_Storage_class yy=_tmp1D7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1099
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY20).tag=28U,(_tmp6F0.YY20).val=yy1;_tmp6F0;});}static char _tmp1DB[17U]="type_specifier_t";
# 1093 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DB,_tmp1DB,_tmp1DB + 17U};
union Cyc_YYSTYPE*_tmp1D9=yy1;struct Cyc_Parse_Type_specifier _tmp1DA;if((((union Cyc_YYSTYPE*)_tmp1D9)->YY21).tag == 29){_LL1: _tmp1DA=(_tmp1D9->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1DA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1100
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY21).tag=29U,(_tmp6F1.YY21).val=yy1;_tmp6F1;});}static char _tmp1DE[12U]="aggr_kind_t";
# 1095 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DE,_tmp1DE,_tmp1DE + 12U};
union Cyc_YYSTYPE*_tmp1DC=yy1;enum Cyc_Absyn_AggrKind _tmp1DD;if((((union Cyc_YYSTYPE*)_tmp1DC)->YY22).tag == 30){_LL1: _tmp1DD=(_tmp1DC->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1DD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1102
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY22).tag=30U,(_tmp6F2.YY22).val=yy1;_tmp6F2;});}static char _tmp1E1[8U]="tqual_t";
# 1096 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E1,_tmp1E1,_tmp1E1 + 8U};
union Cyc_YYSTYPE*_tmp1DF=yy1;struct Cyc_Absyn_Tqual _tmp1E0;if((((union Cyc_YYSTYPE*)_tmp1DF)->YY23).tag == 31){_LL1: _tmp1E0=(_tmp1DF->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1E0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY23).tag=31U,(_tmp6F3.YY23).val=yy1;_tmp6F3;});}static char _tmp1E4[23U]="list_t<aggrfield_t,`H>";
# 1097 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E4,_tmp1E4,_tmp1E4 + 23U};
union Cyc_YYSTYPE*_tmp1E2=yy1;struct Cyc_List_List*_tmp1E3;if((((union Cyc_YYSTYPE*)_tmp1E2)->YY24).tag == 32){_LL1: _tmp1E3=(_tmp1E2->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1E3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY24).tag=32U,(_tmp6F4.YY24).val=yy1;_tmp6F4;});}static char _tmp1E7[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1098 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E7,_tmp1E7,_tmp1E7 + 34U};
union Cyc_YYSTYPE*_tmp1E5=yy1;struct Cyc_List_List*_tmp1E6;if((((union Cyc_YYSTYPE*)_tmp1E5)->YY25).tag == 33){_LL1: _tmp1E6=(_tmp1E5->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1E6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY25).tag=33U,(_tmp6F5.YY25).val=yy1;_tmp6F5;});}static char _tmp1EA[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1099 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EA,_tmp1EA,_tmp1EA + 33U};
union Cyc_YYSTYPE*_tmp1E8=yy1;struct Cyc_List_List*_tmp1E9;if((((union Cyc_YYSTYPE*)_tmp1E8)->YY26).tag == 34){_LL1: _tmp1E9=(_tmp1E8->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1E9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1106
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY26).tag=34U,(_tmp6F6.YY26).val=yy1;_tmp6F6;});}static char _tmp1ED[18U]="declarator_t<`yy>";
# 1100 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1ED,_tmp1ED,_tmp1ED + 18U};
union Cyc_YYSTYPE*_tmp1EB=yy1;struct Cyc_Parse_Declarator _tmp1EC;if((((union Cyc_YYSTYPE*)_tmp1EB)->YY27).tag == 35){_LL1: _tmp1EC=(_tmp1EB->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1EC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1107
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY27).tag=35U,(_tmp6F7.YY27).val=yy1;_tmp6F7;});}static char _tmp1F0[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1101 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F0,_tmp1F0,_tmp1F0 + 45U};
union Cyc_YYSTYPE*_tmp1EE=yy1;struct _tuple25*_tmp1EF;if((((union Cyc_YYSTYPE*)_tmp1EE)->YY28).tag == 36){_LL1: _tmp1EF=(_tmp1EE->YY28).val;_LL2: {struct _tuple25*yy=_tmp1EF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY28).tag=36U,(_tmp6F8.YY28).val=yy1;_tmp6F8;});}static char _tmp1F3[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1102 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F3,_tmp1F3,_tmp1F3 + 57U};
union Cyc_YYSTYPE*_tmp1F1=yy1;struct Cyc_List_List*_tmp1F2;if((((union Cyc_YYSTYPE*)_tmp1F1)->YY29).tag == 37){_LL1: _tmp1F2=(_tmp1F1->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1F2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY29).tag=37U,(_tmp6F9.YY29).val=yy1;_tmp6F9;});}static char _tmp1F6[26U]="abstractdeclarator_t<`yy>";
# 1103 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F6,_tmp1F6,_tmp1F6 + 26U};
union Cyc_YYSTYPE*_tmp1F4=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F5;if((((union Cyc_YYSTYPE*)_tmp1F4)->YY30).tag == 38){_LL1: _tmp1F5=(_tmp1F4->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1F5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY30).tag=38U,(_tmp6FA.YY30).val=yy1;_tmp6FA;});}static char _tmp1F9[5U]="bool";
# 1104 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F9,_tmp1F9,_tmp1F9 + 5U};
union Cyc_YYSTYPE*_tmp1F7=yy1;int _tmp1F8;if((((union Cyc_YYSTYPE*)_tmp1F7)->YY31).tag == 39){_LL1: _tmp1F8=(_tmp1F7->YY31).val;_LL2: {int yy=_tmp1F8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY31).tag=39U,(_tmp6FB.YY31).val=yy1;_tmp6FB;});}static char _tmp1FC[8U]="scope_t";
# 1105 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FC,_tmp1FC,_tmp1FC + 8U};
union Cyc_YYSTYPE*_tmp1FA=yy1;enum Cyc_Absyn_Scope _tmp1FB;if((((union Cyc_YYSTYPE*)_tmp1FA)->YY32).tag == 40){_LL1: _tmp1FB=(_tmp1FA->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1FB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1112
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY32).tag=40U,(_tmp6FC.YY32).val=yy1;_tmp6FC;});}static char _tmp1FF[16U]="datatypefield_t";
# 1106 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FF,_tmp1FF,_tmp1FF + 16U};
union Cyc_YYSTYPE*_tmp1FD=yy1;struct Cyc_Absyn_Datatypefield*_tmp1FE;if((((union Cyc_YYSTYPE*)_tmp1FD)->YY33).tag == 41){_LL1: _tmp1FE=(_tmp1FD->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp1FE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1113
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY33).tag=41U,(_tmp6FD.YY33).val=yy1;_tmp6FD;});}static char _tmp202[27U]="list_t<datatypefield_t,`H>";
# 1107 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp202,_tmp202,_tmp202 + 27U};
union Cyc_YYSTYPE*_tmp200=yy1;struct Cyc_List_List*_tmp201;if((((union Cyc_YYSTYPE*)_tmp200)->YY34).tag == 42){_LL1: _tmp201=(_tmp200->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp201;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1114
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY34).tag=42U,(_tmp6FE.YY34).val=yy1;_tmp6FE;});}static char _tmp205[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1108 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp205,_tmp205,_tmp205 + 41U};
union Cyc_YYSTYPE*_tmp203=yy1;struct _tuple26 _tmp204;if((((union Cyc_YYSTYPE*)_tmp203)->YY35).tag == 43){_LL1: _tmp204=(_tmp203->YY35).val;_LL2: {struct _tuple26 yy=_tmp204;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1115
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY35).tag=43U,(_tmp6FF.YY35).val=yy1;_tmp6FF;});}static char _tmp208[17U]="list_t<var_t,`H>";
# 1109 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp208,_tmp208,_tmp208 + 17U};
union Cyc_YYSTYPE*_tmp206=yy1;struct Cyc_List_List*_tmp207;if((((union Cyc_YYSTYPE*)_tmp206)->YY36).tag == 44){_LL1: _tmp207=(_tmp206->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp207;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1116
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY36).tag=44U,(_tmp700.YY36).val=yy1;_tmp700;});}static char _tmp20B[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1110 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20B,_tmp20B,_tmp20B + 31U};
union Cyc_YYSTYPE*_tmp209=yy1;struct _tuple8*_tmp20A;if((((union Cyc_YYSTYPE*)_tmp209)->YY37).tag == 45){_LL1: _tmp20A=(_tmp209->YY37).val;_LL2: {struct _tuple8*yy=_tmp20A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1117
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY37).tag=45U,(_tmp701.YY37).val=yy1;_tmp701;});}static char _tmp20E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1111 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20E,_tmp20E,_tmp20E + 42U};
union Cyc_YYSTYPE*_tmp20C=yy1;struct Cyc_List_List*_tmp20D;if((((union Cyc_YYSTYPE*)_tmp20C)->YY38).tag == 46){_LL1: _tmp20D=(_tmp20C->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp20D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1118
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY38).tag=46U,(_tmp702.YY38).val=yy1;_tmp702;});}static char _tmp211[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1112 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp211,_tmp211,_tmp211 + 115U};
union Cyc_YYSTYPE*_tmp20F=yy1;struct _tuple27*_tmp210;if((((union Cyc_YYSTYPE*)_tmp20F)->YY39).tag == 47){_LL1: _tmp210=(_tmp20F->YY39).val;_LL2: {struct _tuple27*yy=_tmp210;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY39).tag=47U,(_tmp703.YY39).val=yy1;_tmp703;});}static char _tmp214[8U]="types_t";
# 1113 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp214,_tmp214,_tmp214 + 8U};
union Cyc_YYSTYPE*_tmp212=yy1;struct Cyc_List_List*_tmp213;if((((union Cyc_YYSTYPE*)_tmp212)->YY40).tag == 48){_LL1: _tmp213=(_tmp212->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp213;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY40).tag=48U,(_tmp704.YY40).val=yy1;_tmp704;});}static char _tmp217[24U]="list_t<designator_t,`H>";
# 1115 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp217,_tmp217,_tmp217 + 24U};
union Cyc_YYSTYPE*_tmp215=yy1;struct Cyc_List_List*_tmp216;if((((union Cyc_YYSTYPE*)_tmp215)->YY41).tag == 49){_LL1: _tmp216=(_tmp215->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp216;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY41).tag=49U,(_tmp705.YY41).val=yy1;_tmp705;});}static char _tmp21A[13U]="designator_t";
# 1116 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21A,_tmp21A,_tmp21A + 13U};
union Cyc_YYSTYPE*_tmp218=yy1;void*_tmp219;if((((union Cyc_YYSTYPE*)_tmp218)->YY42).tag == 50){_LL1: _tmp219=(_tmp218->YY42).val;_LL2: {void*yy=_tmp219;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY42).tag=50U,(_tmp706.YY42).val=yy1;_tmp706;});}static char _tmp21D[7U]="kind_t";
# 1117 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21D,_tmp21D,_tmp21D + 7U};
union Cyc_YYSTYPE*_tmp21B=yy1;struct Cyc_Absyn_Kind*_tmp21C;if((((union Cyc_YYSTYPE*)_tmp21B)->YY43).tag == 51){_LL1: _tmp21C=(_tmp21B->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp21C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1124
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY43).tag=51U,(_tmp707.YY43).val=yy1;_tmp707;});}static char _tmp220[7U]="type_t";
# 1118 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp220,_tmp220,_tmp220 + 7U};
union Cyc_YYSTYPE*_tmp21E=yy1;void*_tmp21F;if((((union Cyc_YYSTYPE*)_tmp21E)->YY44).tag == 52){_LL1: _tmp21F=(_tmp21E->YY44).val;_LL2: {void*yy=_tmp21F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1125
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY44).tag=52U,(_tmp708.YY44).val=yy1;_tmp708;});}static char _tmp223[23U]="list_t<attribute_t,`H>";
# 1119 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp223,_tmp223,_tmp223 + 23U};
union Cyc_YYSTYPE*_tmp221=yy1;struct Cyc_List_List*_tmp222;if((((union Cyc_YYSTYPE*)_tmp221)->YY45).tag == 53){_LL1: _tmp222=(_tmp221->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp222;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1126
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY45).tag=53U,(_tmp709.YY45).val=yy1;_tmp709;});}static char _tmp226[12U]="attribute_t";
# 1120 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp226,_tmp226,_tmp226 + 12U};
union Cyc_YYSTYPE*_tmp224=yy1;void*_tmp225;if((((union Cyc_YYSTYPE*)_tmp224)->YY46).tag == 54){_LL1: _tmp225=(_tmp224->YY46).val;_LL2: {void*yy=_tmp225;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1127
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY46).tag=54U,(_tmp70A.YY46).val=yy1;_tmp70A;});}static char _tmp229[12U]="enumfield_t";
# 1121 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp229,_tmp229,_tmp229 + 12U};
union Cyc_YYSTYPE*_tmp227=yy1;struct Cyc_Absyn_Enumfield*_tmp228;if((((union Cyc_YYSTYPE*)_tmp227)->YY47).tag == 55){_LL1: _tmp228=(_tmp227->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp228;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1128
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY47).tag=55U,(_tmp70B.YY47).val=yy1;_tmp70B;});}static char _tmp22C[23U]="list_t<enumfield_t,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22C,_tmp22C,_tmp22C + 23U};
union Cyc_YYSTYPE*_tmp22A=yy1;struct Cyc_List_List*_tmp22B;if((((union Cyc_YYSTYPE*)_tmp22A)->YY48).tag == 56){_LL1: _tmp22B=(_tmp22A->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp22B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1129
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY48).tag=56U,(_tmp70C.YY48).val=yy1;_tmp70C;});}static char _tmp22F[11U]="type_opt_t";
# 1123 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22F,_tmp22F,_tmp22F + 11U};
union Cyc_YYSTYPE*_tmp22D=yy1;void*_tmp22E;if((((union Cyc_YYSTYPE*)_tmp22D)->YY49).tag == 57){_LL1: _tmp22E=(_tmp22D->YY49).val;_LL2: {void*yy=_tmp22E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY49).tag=57U,(_tmp70D.YY49).val=yy1;_tmp70D;});}static char _tmp232[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp232,_tmp232,_tmp232 + 31U};
union Cyc_YYSTYPE*_tmp230=yy1;struct Cyc_List_List*_tmp231;if((((union Cyc_YYSTYPE*)_tmp230)->YY50).tag == 58){_LL1: _tmp231=(_tmp230->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp231;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY50).tag=58U,(_tmp70E.YY50).val=yy1;_tmp70E;});}static char _tmp235[11U]="booltype_t";
# 1125 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp235,_tmp235,_tmp235 + 11U};
union Cyc_YYSTYPE*_tmp233=yy1;void*_tmp234;if((((union Cyc_YYSTYPE*)_tmp233)->YY51).tag == 59){_LL1: _tmp234=(_tmp233->YY51).val;_LL2: {void*yy=_tmp234;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1132
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY51).tag=59U,(_tmp70F.YY51).val=yy1;_tmp70F;});}static char _tmp238[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1126 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp238,_tmp238,_tmp238 + 35U};
union Cyc_YYSTYPE*_tmp236=yy1;struct Cyc_List_List*_tmp237;if((((union Cyc_YYSTYPE*)_tmp236)->YY52).tag == 60){_LL1: _tmp237=(_tmp236->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp237;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1133
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY52).tag=60U,(_tmp710.YY52).val=yy1;_tmp710;});}static char _tmp23B[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1127 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23B,_tmp23B,_tmp23B + 48U};
union Cyc_YYSTYPE*_tmp239=yy1;struct _tuple28*_tmp23A;if((((union Cyc_YYSTYPE*)_tmp239)->YY53).tag == 61){_LL1: _tmp23A=(_tmp239->YY53).val;_LL2: {struct _tuple28*yy=_tmp23A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1134
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY53).tag=61U,(_tmp711.YY53).val=yy1;_tmp711;});}static char _tmp23E[18U]="list_t<qvar_t,`H>";
# 1128 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23E,_tmp23E,_tmp23E + 18U};
union Cyc_YYSTYPE*_tmp23C=yy1;struct Cyc_List_List*_tmp23D;if((((union Cyc_YYSTYPE*)_tmp23C)->YY54).tag == 62){_LL1: _tmp23D=(_tmp23C->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp23D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1135
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY54).tag=62U,(_tmp712.YY54).val=yy1;_tmp712;});}static char _tmp241[20U]="pointer_qual_t<`yy>";
# 1129 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp241,_tmp241,_tmp241 + 20U};
union Cyc_YYSTYPE*_tmp23F=yy1;void*_tmp240;if((((union Cyc_YYSTYPE*)_tmp23F)->YY55).tag == 63){_LL1: _tmp240=(_tmp23F->YY55).val;_LL2: {void*yy=_tmp240;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1136
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY55).tag=63U,(_tmp713.YY55).val=yy1;_tmp713;});}static char _tmp244[21U]="pointer_quals_t<`yy>";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp244,_tmp244,_tmp244 + 21U};
union Cyc_YYSTYPE*_tmp242=yy1;struct Cyc_List_List*_tmp243;if((((union Cyc_YYSTYPE*)_tmp242)->YY56).tag == 64){_LL1: _tmp243=(_tmp242->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp243;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY56).tag=64U,(_tmp714.YY56).val=yy1;_tmp714;});}static char _tmp247[10U]="exp_opt_t";
# 1131 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp247,_tmp247,_tmp247 + 10U};
union Cyc_YYSTYPE*_tmp245=yy1;struct Cyc_Absyn_Exp*_tmp246;if((((union Cyc_YYSTYPE*)_tmp245)->YY57).tag == 65){_LL1: _tmp246=(_tmp245->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp246;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY57).tag=65U,(_tmp715.YY57).val=yy1;_tmp715;});}static char _tmp24A[10U]="raw_exp_t";
# 1132 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24A,_tmp24A,_tmp24A + 10U};
union Cyc_YYSTYPE*_tmp248=yy1;void*_tmp249;if((((union Cyc_YYSTYPE*)_tmp248)->YY58).tag == 66){_LL1: _tmp249=(_tmp248->YY58).val;_LL2: {void*yy=_tmp249;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1139
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY58).tag=66U,(_tmp716.YY58).val=yy1;_tmp716;});}static char _tmp24D[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1134 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24D,_tmp24D,_tmp24D + 112U};
union Cyc_YYSTYPE*_tmp24B=yy1;struct _tuple29*_tmp24C;if((((union Cyc_YYSTYPE*)_tmp24B)->YY59).tag == 67){_LL1: _tmp24C=(_tmp24B->YY59).val;_LL2: {struct _tuple29*yy=_tmp24C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY59).tag=67U,(_tmp717.YY59).val=yy1;_tmp717;});}static char _tmp250[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1135 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp250,_tmp250,_tmp250 + 73U};
union Cyc_YYSTYPE*_tmp24E=yy1;struct _tuple30*_tmp24F;if((((union Cyc_YYSTYPE*)_tmp24E)->YY60).tag == 68){_LL1: _tmp24F=(_tmp24E->YY60).val;_LL2: {struct _tuple30*yy=_tmp24F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY60).tag=68U,(_tmp718.YY60).val=yy1;_tmp718;});}static char _tmp253[28U]="list_t<string_t<`H>@`H, `H>";
# 1136 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp253,_tmp253,_tmp253 + 28U};
union Cyc_YYSTYPE*_tmp251=yy1;struct Cyc_List_List*_tmp252;if((((union Cyc_YYSTYPE*)_tmp251)->YY61).tag == 69){_LL1: _tmp252=(_tmp251->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp252;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1143
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp719;(_tmp719.YY61).tag=69U,(_tmp719.YY61).val=yy1;_tmp719;});}static char _tmp256[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1137 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp256,_tmp256,_tmp256 + 38U};
union Cyc_YYSTYPE*_tmp254=yy1;struct Cyc_List_List*_tmp255;if((((union Cyc_YYSTYPE*)_tmp254)->YY62).tag == 70){_LL1: _tmp255=(_tmp254->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp255;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1144
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71A;(_tmp71A.YY62).tag=70U,(_tmp71A.YY62).val=yy1;_tmp71A;});}static char _tmp259[26U]="$(string_t<`H>, exp_t)@`H";
# 1138 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp259,_tmp259,_tmp259 + 26U};
union Cyc_YYSTYPE*_tmp257=yy1;struct _tuple31*_tmp258;if((((union Cyc_YYSTYPE*)_tmp257)->YY63).tag == 71){_LL1: _tmp258=(_tmp257->YY63).val;_LL2: {struct _tuple31*yy=_tmp258;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1145
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp71B;(_tmp71B.YY63).tag=71U,(_tmp71B.YY63).val=yy1;_tmp71B;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1161
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp71C;_tmp71C.timestamp=0,_tmp71C.first_line=0,_tmp71C.first_column=0,_tmp71C.last_line=0,_tmp71C.last_column=0;_tmp71C;});}
# 1164
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1175 "parse.y"
static short Cyc_yytranslate[381U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,152,2,2,137,150,147,2,134,135,130,144,129,148,139,149,2,2,2,2,2,2,2,2,2,2,138,126,132,131,133,143,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,2,141,146,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,127,145,128,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125};static char _tmp25A[2U]="$";static char _tmp25B[6U]="error";static char _tmp25C[12U]="$undefined.";static char _tmp25D[5U]="AUTO";static char _tmp25E[9U]="REGISTER";static char _tmp25F[7U]="STATIC";static char _tmp260[7U]="EXTERN";static char _tmp261[8U]="TYPEDEF";static char _tmp262[5U]="VOID";static char _tmp263[5U]="CHAR";static char _tmp264[6U]="SHORT";static char _tmp265[4U]="INT";static char _tmp266[5U]="LONG";static char _tmp267[6U]="FLOAT";static char _tmp268[7U]="DOUBLE";static char _tmp269[7U]="SIGNED";static char _tmp26A[9U]="UNSIGNED";static char _tmp26B[6U]="CONST";static char _tmp26C[9U]="VOLATILE";static char _tmp26D[9U]="RESTRICT";static char _tmp26E[7U]="STRUCT";static char _tmp26F[6U]="UNION";static char _tmp270[5U]="CASE";static char _tmp271[8U]="DEFAULT";static char _tmp272[7U]="INLINE";static char _tmp273[7U]="SIZEOF";static char _tmp274[9U]="OFFSETOF";static char _tmp275[3U]="IF";static char _tmp276[5U]="ELSE";static char _tmp277[7U]="SWITCH";static char _tmp278[6U]="WHILE";static char _tmp279[3U]="DO";static char _tmp27A[4U]="FOR";static char _tmp27B[5U]="GOTO";static char _tmp27C[9U]="CONTINUE";static char _tmp27D[6U]="BREAK";static char _tmp27E[7U]="RETURN";static char _tmp27F[5U]="ENUM";static char _tmp280[7U]="TYPEOF";static char _tmp281[16U]="BUILTIN_VA_LIST";static char _tmp282[10U]="EXTENSION";static char _tmp283[8U]="NULL_kw";static char _tmp284[4U]="LET";static char _tmp285[6U]="THROW";static char _tmp286[4U]="TRY";static char _tmp287[6U]="CATCH";static char _tmp288[7U]="EXPORT";static char _tmp289[9U]="OVERRIDE";static char _tmp28A[5U]="HIDE";static char _tmp28B[4U]="NEW";static char _tmp28C[9U]="ABSTRACT";static char _tmp28D[9U]="FALLTHRU";static char _tmp28E[6U]="USING";static char _tmp28F[10U]="NAMESPACE";static char _tmp290[9U]="DATATYPE";static char _tmp291[7U]="MALLOC";static char _tmp292[8U]="RMALLOC";static char _tmp293[15U]="RMALLOC_INLINE";static char _tmp294[7U]="CALLOC";static char _tmp295[8U]="RCALLOC";static char _tmp296[5U]="SWAP";static char _tmp297[5U]="POOL";static char _tmp298[9U]="REGION_T";static char _tmp299[6U]="TAG_T";static char _tmp29A[7U]="REGION";static char _tmp29B[5U]="RNEW";static char _tmp29C[8U]="REGIONS";static char _tmp29D[7U]="PORTON";static char _tmp29E[8U]="PORTOFF";static char _tmp29F[7U]="PRAGMA";static char _tmp2A0[10U]="TEMPESTON";static char _tmp2A1[11U]="TEMPESTOFF";static char _tmp2A2[8U]="NUMELTS";static char _tmp2A3[8U]="VALUEOF";static char _tmp2A4[10U]="VALUEOF_T";static char _tmp2A5[9U]="TAGCHECK";static char _tmp2A6[13U]="NUMELTS_QUAL";static char _tmp2A7[10U]="THIN_QUAL";static char _tmp2A8[9U]="FAT_QUAL";static char _tmp2A9[13U]="NOTNULL_QUAL";static char _tmp2AA[14U]="NULLABLE_QUAL";static char _tmp2AB[14U]="REQUIRES_QUAL";static char _tmp2AC[13U]="ENSURES_QUAL";static char _tmp2AD[12U]="REGION_QUAL";static char _tmp2AE[16U]="NOZEROTERM_QUAL";static char _tmp2AF[14U]="ZEROTERM_QUAL";static char _tmp2B0[12U]="TAGGED_QUAL";static char _tmp2B1[12U]="ASSERT_QUAL";static char _tmp2B2[16U]="EXTENSIBLE_QUAL";static char _tmp2B3[7U]="PTR_OP";static char _tmp2B4[7U]="INC_OP";static char _tmp2B5[7U]="DEC_OP";static char _tmp2B6[8U]="LEFT_OP";static char _tmp2B7[9U]="RIGHT_OP";static char _tmp2B8[6U]="LE_OP";static char _tmp2B9[6U]="GE_OP";static char _tmp2BA[6U]="EQ_OP";static char _tmp2BB[6U]="NE_OP";static char _tmp2BC[7U]="AND_OP";static char _tmp2BD[6U]="OR_OP";static char _tmp2BE[11U]="MUL_ASSIGN";static char _tmp2BF[11U]="DIV_ASSIGN";static char _tmp2C0[11U]="MOD_ASSIGN";static char _tmp2C1[11U]="ADD_ASSIGN";static char _tmp2C2[11U]="SUB_ASSIGN";static char _tmp2C3[12U]="LEFT_ASSIGN";static char _tmp2C4[13U]="RIGHT_ASSIGN";static char _tmp2C5[11U]="AND_ASSIGN";static char _tmp2C6[11U]="XOR_ASSIGN";static char _tmp2C7[10U]="OR_ASSIGN";static char _tmp2C8[9U]="ELLIPSIS";static char _tmp2C9[11U]="LEFT_RIGHT";static char _tmp2CA[12U]="COLON_COLON";static char _tmp2CB[11U]="IDENTIFIER";static char _tmp2CC[17U]="INTEGER_CONSTANT";static char _tmp2CD[7U]="STRING";static char _tmp2CE[8U]="WSTRING";static char _tmp2CF[19U]="CHARACTER_CONSTANT";static char _tmp2D0[20U]="WCHARACTER_CONSTANT";static char _tmp2D1[18U]="FLOATING_CONSTANT";static char _tmp2D2[9U]="TYPE_VAR";static char _tmp2D3[13U]="TYPEDEF_NAME";static char _tmp2D4[16U]="QUAL_IDENTIFIER";static char _tmp2D5[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D6[10U]="ATTRIBUTE";static char _tmp2D7[8U]="ASM_TOK";static char _tmp2D8[4U]="';'";static char _tmp2D9[4U]="'{'";static char _tmp2DA[4U]="'}'";static char _tmp2DB[4U]="','";static char _tmp2DC[4U]="'*'";static char _tmp2DD[4U]="'='";static char _tmp2DE[4U]="'<'";static char _tmp2DF[4U]="'>'";static char _tmp2E0[4U]="'('";static char _tmp2E1[4U]="')'";static char _tmp2E2[4U]="'_'";static char _tmp2E3[4U]="'$'";static char _tmp2E4[4U]="':'";static char _tmp2E5[4U]="'.'";static char _tmp2E6[4U]="'['";static char _tmp2E7[4U]="']'";static char _tmp2E8[4U]="'@'";static char _tmp2E9[4U]="'?'";static char _tmp2EA[4U]="'+'";static char _tmp2EB[4U]="'|'";static char _tmp2EC[4U]="'^'";static char _tmp2ED[4U]="'&'";static char _tmp2EE[4U]="'-'";static char _tmp2EF[4U]="'/'";static char _tmp2F0[4U]="'%'";static char _tmp2F1[4U]="'~'";static char _tmp2F2[4U]="'!'";static char _tmp2F3[5U]="prog";static char _tmp2F4[17U]="translation_unit";static char _tmp2F5[18U]="tempest_on_action";static char _tmp2F6[19U]="tempest_off_action";static char _tmp2F7[16U]="extern_c_action";static char _tmp2F8[13U]="end_extern_c";static char _tmp2F9[14U]="hide_list_opt";static char _tmp2FA[17U]="hide_list_values";static char _tmp2FB[16U]="export_list_opt";static char _tmp2FC[12U]="export_list";static char _tmp2FD[19U]="export_list_values";static char _tmp2FE[13U]="override_opt";static char _tmp2FF[21U]="external_declaration";static char _tmp300[15U]="optional_comma";static char _tmp301[20U]="function_definition";static char _tmp302[21U]="function_definition2";static char _tmp303[13U]="using_action";static char _tmp304[15U]="unusing_action";static char _tmp305[17U]="namespace_action";static char _tmp306[19U]="unnamespace_action";static char _tmp307[12U]="declaration";static char _tmp308[17U]="declaration_list";static char _tmp309[23U]="declaration_specifiers";static char _tmp30A[24U]="storage_class_specifier";static char _tmp30B[15U]="attributes_opt";static char _tmp30C[11U]="attributes";static char _tmp30D[15U]="attribute_list";static char _tmp30E[10U]="attribute";static char _tmp30F[15U]="type_specifier";static char _tmp310[25U]="type_specifier_notypedef";static char _tmp311[5U]="kind";static char _tmp312[15U]="type_qualifier";static char _tmp313[15U]="enum_specifier";static char _tmp314[11U]="enum_field";static char _tmp315[22U]="enum_declaration_list";static char _tmp316[26U]="struct_or_union_specifier";static char _tmp317[16U]="type_params_opt";static char _tmp318[16U]="struct_or_union";static char _tmp319[24U]="struct_declaration_list";static char _tmp31A[25U]="struct_declaration_list0";static char _tmp31B[21U]="init_declarator_list";static char _tmp31C[22U]="init_declarator_list0";static char _tmp31D[16U]="init_declarator";static char _tmp31E[19U]="struct_declaration";static char _tmp31F[25U]="specifier_qualifier_list";static char _tmp320[35U]="notypedef_specifier_qualifier_list";static char _tmp321[23U]="struct_declarator_list";static char _tmp322[24U]="struct_declarator_list0";static char _tmp323[18U]="struct_declarator";static char _tmp324[20U]="requires_clause_opt";static char _tmp325[19U]="ensures_clause_opt";static char _tmp326[19U]="datatype_specifier";static char _tmp327[14U]="qual_datatype";static char _tmp328[19U]="datatypefield_list";static char _tmp329[20U]="datatypefield_scope";static char _tmp32A[14U]="datatypefield";static char _tmp32B[11U]="declarator";static char _tmp32C[23U]="declarator_withtypedef";static char _tmp32D[18U]="direct_declarator";static char _tmp32E[30U]="direct_declarator_withtypedef";static char _tmp32F[8U]="pointer";static char _tmp330[12U]="one_pointer";static char _tmp331[14U]="pointer_quals";static char _tmp332[13U]="pointer_qual";static char _tmp333[23U]="pointer_null_and_bound";static char _tmp334[14U]="pointer_bound";static char _tmp335[18U]="zeroterm_qual_opt";static char _tmp336[8U]="rgn_opt";static char _tmp337[11U]="tqual_list";static char _tmp338[20U]="parameter_type_list";static char _tmp339[9U]="type_var";static char _tmp33A[16U]="optional_effect";static char _tmp33B[19U]="optional_rgn_order";static char _tmp33C[10U]="rgn_order";static char _tmp33D[16U]="optional_inject";static char _tmp33E[11U]="effect_set";static char _tmp33F[14U]="atomic_effect";static char _tmp340[11U]="region_set";static char _tmp341[15U]="parameter_list";static char _tmp342[22U]="parameter_declaration";static char _tmp343[16U]="identifier_list";static char _tmp344[17U]="identifier_list0";static char _tmp345[12U]="initializer";static char _tmp346[18U]="array_initializer";static char _tmp347[17U]="initializer_list";static char _tmp348[12U]="designation";static char _tmp349[16U]="designator_list";static char _tmp34A[11U]="designator";static char _tmp34B[10U]="type_name";static char _tmp34C[14U]="any_type_name";static char _tmp34D[15U]="type_name_list";static char _tmp34E[20U]="abstract_declarator";static char _tmp34F[27U]="direct_abstract_declarator";static char _tmp350[10U]="statement";static char _tmp351[18U]="labeled_statement";static char _tmp352[21U]="expression_statement";static char _tmp353[19U]="compound_statement";static char _tmp354[16U]="block_item_list";static char _tmp355[20U]="selection_statement";static char _tmp356[15U]="switch_clauses";static char _tmp357[20U]="iteration_statement";static char _tmp358[15U]="jump_statement";static char _tmp359[12U]="exp_pattern";static char _tmp35A[20U]="conditional_pattern";static char _tmp35B[19U]="logical_or_pattern";static char _tmp35C[20U]="logical_and_pattern";static char _tmp35D[21U]="inclusive_or_pattern";static char _tmp35E[21U]="exclusive_or_pattern";static char _tmp35F[12U]="and_pattern";static char _tmp360[17U]="equality_pattern";static char _tmp361[19U]="relational_pattern";static char _tmp362[14U]="shift_pattern";static char _tmp363[17U]="additive_pattern";static char _tmp364[23U]="multiplicative_pattern";static char _tmp365[13U]="cast_pattern";static char _tmp366[14U]="unary_pattern";static char _tmp367[16U]="postfix_pattern";static char _tmp368[16U]="primary_pattern";static char _tmp369[8U]="pattern";static char _tmp36A[19U]="tuple_pattern_list";static char _tmp36B[20U]="tuple_pattern_list0";static char _tmp36C[14U]="field_pattern";static char _tmp36D[19U]="field_pattern_list";static char _tmp36E[20U]="field_pattern_list0";static char _tmp36F[11U]="expression";static char _tmp370[22U]="assignment_expression";static char _tmp371[20U]="assignment_operator";static char _tmp372[23U]="conditional_expression";static char _tmp373[20U]="constant_expression";static char _tmp374[22U]="logical_or_expression";static char _tmp375[23U]="logical_and_expression";static char _tmp376[24U]="inclusive_or_expression";static char _tmp377[24U]="exclusive_or_expression";static char _tmp378[15U]="and_expression";static char _tmp379[20U]="equality_expression";static char _tmp37A[22U]="relational_expression";static char _tmp37B[17U]="shift_expression";static char _tmp37C[20U]="additive_expression";static char _tmp37D[26U]="multiplicative_expression";static char _tmp37E[16U]="cast_expression";static char _tmp37F[17U]="unary_expression";static char _tmp380[9U]="asm_expr";static char _tmp381[13U]="volatile_opt";static char _tmp382[12U]="asm_out_opt";static char _tmp383[12U]="asm_outlist";static char _tmp384[11U]="asm_in_opt";static char _tmp385[11U]="asm_inlist";static char _tmp386[11U]="asm_io_elt";static char _tmp387[16U]="asm_clobber_opt";static char _tmp388[17U]="asm_clobber_list";static char _tmp389[15U]="unary_operator";static char _tmp38A[19U]="postfix_expression";static char _tmp38B[17U]="field_expression";static char _tmp38C[19U]="primary_expression";static char _tmp38D[25U]="argument_expression_list";static char _tmp38E[26U]="argument_expression_list0";static char _tmp38F[9U]="constant";static char _tmp390[20U]="qual_opt_identifier";static char _tmp391[17U]="qual_opt_typedef";static char _tmp392[18U]="struct_union_name";static char _tmp393[11U]="field_name";static char _tmp394[12U]="right_angle";
# 1547 "parse.y"
static struct _fat_ptr Cyc_yytname[315U]={{_tmp25A,_tmp25A,_tmp25A + 2U},{_tmp25B,_tmp25B,_tmp25B + 6U},{_tmp25C,_tmp25C,_tmp25C + 12U},{_tmp25D,_tmp25D,_tmp25D + 5U},{_tmp25E,_tmp25E,_tmp25E + 9U},{_tmp25F,_tmp25F,_tmp25F + 7U},{_tmp260,_tmp260,_tmp260 + 7U},{_tmp261,_tmp261,_tmp261 + 8U},{_tmp262,_tmp262,_tmp262 + 5U},{_tmp263,_tmp263,_tmp263 + 5U},{_tmp264,_tmp264,_tmp264 + 6U},{_tmp265,_tmp265,_tmp265 + 4U},{_tmp266,_tmp266,_tmp266 + 5U},{_tmp267,_tmp267,_tmp267 + 6U},{_tmp268,_tmp268,_tmp268 + 7U},{_tmp269,_tmp269,_tmp269 + 7U},{_tmp26A,_tmp26A,_tmp26A + 9U},{_tmp26B,_tmp26B,_tmp26B + 6U},{_tmp26C,_tmp26C,_tmp26C + 9U},{_tmp26D,_tmp26D,_tmp26D + 9U},{_tmp26E,_tmp26E,_tmp26E + 7U},{_tmp26F,_tmp26F,_tmp26F + 6U},{_tmp270,_tmp270,_tmp270 + 5U},{_tmp271,_tmp271,_tmp271 + 8U},{_tmp272,_tmp272,_tmp272 + 7U},{_tmp273,_tmp273,_tmp273 + 7U},{_tmp274,_tmp274,_tmp274 + 9U},{_tmp275,_tmp275,_tmp275 + 3U},{_tmp276,_tmp276,_tmp276 + 5U},{_tmp277,_tmp277,_tmp277 + 7U},{_tmp278,_tmp278,_tmp278 + 6U},{_tmp279,_tmp279,_tmp279 + 3U},{_tmp27A,_tmp27A,_tmp27A + 4U},{_tmp27B,_tmp27B,_tmp27B + 5U},{_tmp27C,_tmp27C,_tmp27C + 9U},{_tmp27D,_tmp27D,_tmp27D + 6U},{_tmp27E,_tmp27E,_tmp27E + 7U},{_tmp27F,_tmp27F,_tmp27F + 5U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 16U},{_tmp282,_tmp282,_tmp282 + 10U},{_tmp283,_tmp283,_tmp283 + 8U},{_tmp284,_tmp284,_tmp284 + 4U},{_tmp285,_tmp285,_tmp285 + 6U},{_tmp286,_tmp286,_tmp286 + 4U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 7U},{_tmp289,_tmp289,_tmp289 + 9U},{_tmp28A,_tmp28A,_tmp28A + 5U},{_tmp28B,_tmp28B,_tmp28B + 4U},{_tmp28C,_tmp28C,_tmp28C + 9U},{_tmp28D,_tmp28D,_tmp28D + 9U},{_tmp28E,_tmp28E,_tmp28E + 6U},{_tmp28F,_tmp28F,_tmp28F + 10U},{_tmp290,_tmp290,_tmp290 + 9U},{_tmp291,_tmp291,_tmp291 + 7U},{_tmp292,_tmp292,_tmp292 + 8U},{_tmp293,_tmp293,_tmp293 + 15U},{_tmp294,_tmp294,_tmp294 + 7U},{_tmp295,_tmp295,_tmp295 + 8U},{_tmp296,_tmp296,_tmp296 + 5U},{_tmp297,_tmp297,_tmp297 + 5U},{_tmp298,_tmp298,_tmp298 + 9U},{_tmp299,_tmp299,_tmp299 + 6U},{_tmp29A,_tmp29A,_tmp29A + 7U},{_tmp29B,_tmp29B,_tmp29B + 5U},{_tmp29C,_tmp29C,_tmp29C + 8U},{_tmp29D,_tmp29D,_tmp29D + 7U},{_tmp29E,_tmp29E,_tmp29E + 8U},{_tmp29F,_tmp29F,_tmp29F + 7U},{_tmp2A0,_tmp2A0,_tmp2A0 + 10U},{_tmp2A1,_tmp2A1,_tmp2A1 + 11U},{_tmp2A2,_tmp2A2,_tmp2A2 + 8U},{_tmp2A3,_tmp2A3,_tmp2A3 + 8U},{_tmp2A4,_tmp2A4,_tmp2A4 + 10U},{_tmp2A5,_tmp2A5,_tmp2A5 + 9U},{_tmp2A6,_tmp2A6,_tmp2A6 + 13U},{_tmp2A7,_tmp2A7,_tmp2A7 + 10U},{_tmp2A8,_tmp2A8,_tmp2A8 + 9U},{_tmp2A9,_tmp2A9,_tmp2A9 + 13U},{_tmp2AA,_tmp2AA,_tmp2AA + 14U},{_tmp2AB,_tmp2AB,_tmp2AB + 14U},{_tmp2AC,_tmp2AC,_tmp2AC + 13U},{_tmp2AD,_tmp2AD,_tmp2AD + 12U},{_tmp2AE,_tmp2AE,_tmp2AE + 16U},{_tmp2AF,_tmp2AF,_tmp2AF + 14U},{_tmp2B0,_tmp2B0,_tmp2B0 + 12U},{_tmp2B1,_tmp2B1,_tmp2B1 + 12U},{_tmp2B2,_tmp2B2,_tmp2B2 + 16U},{_tmp2B3,_tmp2B3,_tmp2B3 + 7U},{_tmp2B4,_tmp2B4,_tmp2B4 + 7U},{_tmp2B5,_tmp2B5,_tmp2B5 + 7U},{_tmp2B6,_tmp2B6,_tmp2B6 + 8U},{_tmp2B7,_tmp2B7,_tmp2B7 + 9U},{_tmp2B8,_tmp2B8,_tmp2B8 + 6U},{_tmp2B9,_tmp2B9,_tmp2B9 + 6U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 6U},{_tmp2BC,_tmp2BC,_tmp2BC + 7U},{_tmp2BD,_tmp2BD,_tmp2BD + 6U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 11U},{_tmp2C1,_tmp2C1,_tmp2C1 + 11U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 12U},{_tmp2C4,_tmp2C4,_tmp2C4 + 13U},{_tmp2C5,_tmp2C5,_tmp2C5 + 11U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 10U},{_tmp2C8,_tmp2C8,_tmp2C8 + 9U},{_tmp2C9,_tmp2C9,_tmp2C9 + 11U},{_tmp2CA,_tmp2CA,_tmp2CA + 12U},{_tmp2CB,_tmp2CB,_tmp2CB + 11U},{_tmp2CC,_tmp2CC,_tmp2CC + 17U},{_tmp2CD,_tmp2CD,_tmp2CD + 7U},{_tmp2CE,_tmp2CE,_tmp2CE + 8U},{_tmp2CF,_tmp2CF,_tmp2CF + 19U},{_tmp2D0,_tmp2D0,_tmp2D0 + 20U},{_tmp2D1,_tmp2D1,_tmp2D1 + 18U},{_tmp2D2,_tmp2D2,_tmp2D2 + 9U},{_tmp2D3,_tmp2D3,_tmp2D3 + 13U},{_tmp2D4,_tmp2D4,_tmp2D4 + 16U},{_tmp2D5,_tmp2D5,_tmp2D5 + 18U},{_tmp2D6,_tmp2D6,_tmp2D6 + 10U},{_tmp2D7,_tmp2D7,_tmp2D7 + 8U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 5U},{_tmp2F4,_tmp2F4,_tmp2F4 + 17U},{_tmp2F5,_tmp2F5,_tmp2F5 + 18U},{_tmp2F6,_tmp2F6,_tmp2F6 + 19U},{_tmp2F7,_tmp2F7,_tmp2F7 + 16U},{_tmp2F8,_tmp2F8,_tmp2F8 + 13U},{_tmp2F9,_tmp2F9,_tmp2F9 + 14U},{_tmp2FA,_tmp2FA,_tmp2FA + 17U},{_tmp2FB,_tmp2FB,_tmp2FB + 16U},{_tmp2FC,_tmp2FC,_tmp2FC + 12U},{_tmp2FD,_tmp2FD,_tmp2FD + 19U},{_tmp2FE,_tmp2FE,_tmp2FE + 13U},{_tmp2FF,_tmp2FF,_tmp2FF + 21U},{_tmp300,_tmp300,_tmp300 + 15U},{_tmp301,_tmp301,_tmp301 + 20U},{_tmp302,_tmp302,_tmp302 + 21U},{_tmp303,_tmp303,_tmp303 + 13U},{_tmp304,_tmp304,_tmp304 + 15U},{_tmp305,_tmp305,_tmp305 + 17U},{_tmp306,_tmp306,_tmp306 + 19U},{_tmp307,_tmp307,_tmp307 + 12U},{_tmp308,_tmp308,_tmp308 + 17U},{_tmp309,_tmp309,_tmp309 + 23U},{_tmp30A,_tmp30A,_tmp30A + 24U},{_tmp30B,_tmp30B,_tmp30B + 15U},{_tmp30C,_tmp30C,_tmp30C + 11U},{_tmp30D,_tmp30D,_tmp30D + 15U},{_tmp30E,_tmp30E,_tmp30E + 10U},{_tmp30F,_tmp30F,_tmp30F + 15U},{_tmp310,_tmp310,_tmp310 + 25U},{_tmp311,_tmp311,_tmp311 + 5U},{_tmp312,_tmp312,_tmp312 + 15U},{_tmp313,_tmp313,_tmp313 + 15U},{_tmp314,_tmp314,_tmp314 + 11U},{_tmp315,_tmp315,_tmp315 + 22U},{_tmp316,_tmp316,_tmp316 + 26U},{_tmp317,_tmp317,_tmp317 + 16U},{_tmp318,_tmp318,_tmp318 + 16U},{_tmp319,_tmp319,_tmp319 + 24U},{_tmp31A,_tmp31A,_tmp31A + 25U},{_tmp31B,_tmp31B,_tmp31B + 21U},{_tmp31C,_tmp31C,_tmp31C + 22U},{_tmp31D,_tmp31D,_tmp31D + 16U},{_tmp31E,_tmp31E,_tmp31E + 19U},{_tmp31F,_tmp31F,_tmp31F + 25U},{_tmp320,_tmp320,_tmp320 + 35U},{_tmp321,_tmp321,_tmp321 + 23U},{_tmp322,_tmp322,_tmp322 + 24U},{_tmp323,_tmp323,_tmp323 + 18U},{_tmp324,_tmp324,_tmp324 + 20U},{_tmp325,_tmp325,_tmp325 + 19U},{_tmp326,_tmp326,_tmp326 + 19U},{_tmp327,_tmp327,_tmp327 + 14U},{_tmp328,_tmp328,_tmp328 + 19U},{_tmp329,_tmp329,_tmp329 + 20U},{_tmp32A,_tmp32A,_tmp32A + 14U},{_tmp32B,_tmp32B,_tmp32B + 11U},{_tmp32C,_tmp32C,_tmp32C + 23U},{_tmp32D,_tmp32D,_tmp32D + 18U},{_tmp32E,_tmp32E,_tmp32E + 30U},{_tmp32F,_tmp32F,_tmp32F + 8U},{_tmp330,_tmp330,_tmp330 + 12U},{_tmp331,_tmp331,_tmp331 + 14U},{_tmp332,_tmp332,_tmp332 + 13U},{_tmp333,_tmp333,_tmp333 + 23U},{_tmp334,_tmp334,_tmp334 + 14U},{_tmp335,_tmp335,_tmp335 + 18U},{_tmp336,_tmp336,_tmp336 + 8U},{_tmp337,_tmp337,_tmp337 + 11U},{_tmp338,_tmp338,_tmp338 + 20U},{_tmp339,_tmp339,_tmp339 + 9U},{_tmp33A,_tmp33A,_tmp33A + 16U},{_tmp33B,_tmp33B,_tmp33B + 19U},{_tmp33C,_tmp33C,_tmp33C + 10U},{_tmp33D,_tmp33D,_tmp33D + 16U},{_tmp33E,_tmp33E,_tmp33E + 11U},{_tmp33F,_tmp33F,_tmp33F + 14U},{_tmp340,_tmp340,_tmp340 + 11U},{_tmp341,_tmp341,_tmp341 + 15U},{_tmp342,_tmp342,_tmp342 + 22U},{_tmp343,_tmp343,_tmp343 + 16U},{_tmp344,_tmp344,_tmp344 + 17U},{_tmp345,_tmp345,_tmp345 + 12U},{_tmp346,_tmp346,_tmp346 + 18U},{_tmp347,_tmp347,_tmp347 + 17U},{_tmp348,_tmp348,_tmp348 + 12U},{_tmp349,_tmp349,_tmp349 + 16U},{_tmp34A,_tmp34A,_tmp34A + 11U},{_tmp34B,_tmp34B,_tmp34B + 10U},{_tmp34C,_tmp34C,_tmp34C + 14U},{_tmp34D,_tmp34D,_tmp34D + 15U},{_tmp34E,_tmp34E,_tmp34E + 20U},{_tmp34F,_tmp34F,_tmp34F + 27U},{_tmp350,_tmp350,_tmp350 + 10U},{_tmp351,_tmp351,_tmp351 + 18U},{_tmp352,_tmp352,_tmp352 + 21U},{_tmp353,_tmp353,_tmp353 + 19U},{_tmp354,_tmp354,_tmp354 + 16U},{_tmp355,_tmp355,_tmp355 + 20U},{_tmp356,_tmp356,_tmp356 + 15U},{_tmp357,_tmp357,_tmp357 + 20U},{_tmp358,_tmp358,_tmp358 + 15U},{_tmp359,_tmp359,_tmp359 + 12U},{_tmp35A,_tmp35A,_tmp35A + 20U},{_tmp35B,_tmp35B,_tmp35B + 19U},{_tmp35C,_tmp35C,_tmp35C + 20U},{_tmp35D,_tmp35D,_tmp35D + 21U},{_tmp35E,_tmp35E,_tmp35E + 21U},{_tmp35F,_tmp35F,_tmp35F + 12U},{_tmp360,_tmp360,_tmp360 + 17U},{_tmp361,_tmp361,_tmp361 + 19U},{_tmp362,_tmp362,_tmp362 + 14U},{_tmp363,_tmp363,_tmp363 + 17U},{_tmp364,_tmp364,_tmp364 + 23U},{_tmp365,_tmp365,_tmp365 + 13U},{_tmp366,_tmp366,_tmp366 + 14U},{_tmp367,_tmp367,_tmp367 + 16U},{_tmp368,_tmp368,_tmp368 + 16U},{_tmp369,_tmp369,_tmp369 + 8U},{_tmp36A,_tmp36A,_tmp36A + 19U},{_tmp36B,_tmp36B,_tmp36B + 20U},{_tmp36C,_tmp36C,_tmp36C + 14U},{_tmp36D,_tmp36D,_tmp36D + 19U},{_tmp36E,_tmp36E,_tmp36E + 20U},{_tmp36F,_tmp36F,_tmp36F + 11U},{_tmp370,_tmp370,_tmp370 + 22U},{_tmp371,_tmp371,_tmp371 + 20U},{_tmp372,_tmp372,_tmp372 + 23U},{_tmp373,_tmp373,_tmp373 + 20U},{_tmp374,_tmp374,_tmp374 + 22U},{_tmp375,_tmp375,_tmp375 + 23U},{_tmp376,_tmp376,_tmp376 + 24U},{_tmp377,_tmp377,_tmp377 + 24U},{_tmp378,_tmp378,_tmp378 + 15U},{_tmp379,_tmp379,_tmp379 + 20U},{_tmp37A,_tmp37A,_tmp37A + 22U},{_tmp37B,_tmp37B,_tmp37B + 17U},{_tmp37C,_tmp37C,_tmp37C + 20U},{_tmp37D,_tmp37D,_tmp37D + 26U},{_tmp37E,_tmp37E,_tmp37E + 16U},{_tmp37F,_tmp37F,_tmp37F + 17U},{_tmp380,_tmp380,_tmp380 + 9U},{_tmp381,_tmp381,_tmp381 + 13U},{_tmp382,_tmp382,_tmp382 + 12U},{_tmp383,_tmp383,_tmp383 + 12U},{_tmp384,_tmp384,_tmp384 + 11U},{_tmp385,_tmp385,_tmp385 + 11U},{_tmp386,_tmp386,_tmp386 + 11U},{_tmp387,_tmp387,_tmp387 + 16U},{_tmp388,_tmp388,_tmp388 + 17U},{_tmp389,_tmp389,_tmp389 + 15U},{_tmp38A,_tmp38A,_tmp38A + 19U},{_tmp38B,_tmp38B,_tmp38B + 17U},{_tmp38C,_tmp38C,_tmp38C + 19U},{_tmp38D,_tmp38D,_tmp38D + 25U},{_tmp38E,_tmp38E,_tmp38E + 26U},{_tmp38F,_tmp38F,_tmp38F + 9U},{_tmp390,_tmp390,_tmp390 + 20U},{_tmp391,_tmp391,_tmp391 + 17U},{_tmp392,_tmp392,_tmp392 + 18U},{_tmp393,_tmp393,_tmp393 + 11U},{_tmp394,_tmp394,_tmp394 + 12U}};
# 1605
static short Cyc_yyr1[563U]={0,153,154,154,154,154,154,154,154,154,154,154,154,155,156,157,158,159,159,160,160,160,161,161,162,162,162,163,163,163,164,164,165,165,165,166,166,167,167,167,167,168,168,169,170,171,172,173,173,173,173,173,173,173,173,174,174,175,175,175,175,175,175,175,175,175,175,175,176,176,176,176,176,176,176,177,177,178,179,179,180,180,180,180,181,181,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,183,184,184,184,185,185,185,186,186,187,187,187,188,188,188,188,188,189,189,190,190,191,191,192,192,193,194,194,195,195,196,197,197,197,197,197,197,198,198,198,198,198,198,199,200,200,201,201,201,201,202,202,203,203,204,204,204,205,205,206,206,206,206,207,207,207,208,208,209,209,210,210,211,211,211,211,211,211,211,211,211,211,212,212,212,212,212,212,212,212,212,212,212,213,213,214,215,215,216,216,216,216,216,216,216,216,217,217,217,218,218,219,219,219,220,220,220,221,221,222,222,222,222,223,223,224,224,225,225,226,226,227,227,228,228,229,229,229,229,230,230,231,231,232,232,232,233,234,234,235,235,236,236,236,236,236,237,237,237,237,238,238,239,239,240,240,241,241,242,242,242,242,242,243,243,244,244,244,245,245,245,245,245,245,245,245,245,245,245,246,246,246,246,246,246,247,248,248,249,249,250,250,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,253,253,253,253,253,253,253,253,253,253,253,253,253,253,254,254,254,254,254,254,254,254,255,256,256,257,257,258,258,259,259,260,260,261,261,262,262,262,263,263,263,263,263,264,264,264,265,265,265,266,266,266,266,267,267,268,268,268,268,268,268,269,270,271,271,271,271,271,271,271,271,271,271,271,271,271,271,271,271,271,272,272,272,273,273,274,274,275,275,275,276,276,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,279,280,280,280,280,280,280,280,281,282,282,283,283,284,284,285,285,286,286,287,287,287,288,288,288,288,288,289,289,289,290,290,290,291,291,291,291,292,292,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,294,295,295,296,296,296,297,297,298,298,298,299,299,300,301,301,301,302,302,303,303,303,304,304,304,304,304,304,304,304,304,304,304,305,305,305,305,306,306,306,306,306,306,306,306,306,306,306,307,308,308,309,309,309,309,309,310,310,311,311,312,312,313,313,314,314};
# 1665
static short Cyc_yyr2[563U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1725
static short Cyc_yydefact[1142U]={0,34,68,69,70,71,73,86,87,88,89,90,91,92,93,94,110,111,112,128,129,64,0,0,98,0,0,74,0,0,166,0,105,107,0,0,0,13,14,0,0,0,553,232,555,554,556,0,218,0,101,0,218,217,1,0,0,0,0,32,0,0,33,0,57,66,60,84,62,95,96,0,99,0,0,177,0,202,205,100,181,126,72,71,65,0,114,0,59,552,0,553,548,549,550,551,0,126,0,0,392,0,0,0,255,0,394,395,43,45,0,0,0,0,0,0,0,0,0,167,0,0,0,215,0,0,0,0,216,0,0,0,2,0,0,0,0,47,0,134,135,137,58,67,61,63,130,557,558,126,126,0,55,0,0,36,0,234,0,190,178,203,0,209,210,213,214,0,212,211,223,205,0,85,72,118,0,116,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,537,538,498,0,0,0,0,0,518,516,517,0,421,423,437,445,447,449,451,453,455,458,463,466,469,473,0,475,519,536,534,553,404,0,0,0,0,405,0,0,403,50,0,0,126,0,0,0,0,144,140,142,275,277,0,0,52,0,0,8,9,0,126,559,560,233,109,0,0,0,182,102,253,0,250,10,11,0,3,0,5,0,48,0,0,0,36,0,131,132,157,125,0,164,0,0,0,0,0,0,0,0,0,0,0,0,553,305,307,0,315,309,0,313,298,299,300,0,301,302,303,0,56,36,137,35,37,282,0,240,256,0,0,236,234,0,220,0,0,0,225,75,224,206,0,119,115,0,0,0,483,0,0,495,439,473,0,440,441,0,0,0,0,0,0,0,0,0,0,0,476,477,499,494,0,479,0,0,0,0,480,478,0,97,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,427,428,429,430,431,432,433,434,435,436,426,0,481,0,525,526,0,0,0,540,0,126,396,0,0,0,418,553,560,0,0,0,0,271,414,419,0,416,0,0,393,411,412,0,409,257,0,0,0,0,0,278,0,248,145,150,146,148,141,143,234,0,284,276,285,562,561,0,104,106,0,0,108,124,81,80,0,78,219,183,234,252,179,284,254,191,192,0,103,16,30,44,0,46,0,136,138,259,258,36,38,121,133,0,0,0,152,153,160,0,126,126,172,0,0,0,0,0,553,0,0,0,344,345,346,0,0,348,0,0,0,316,310,137,314,308,306,39,0,189,241,0,0,0,247,235,242,160,0,0,0,236,188,222,221,184,220,0,0,226,76,127,120,444,117,113,0,0,0,0,553,260,265,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,539,546,0,545,422,446,0,448,450,452,454,456,457,461,462,459,460,464,465,467,468,470,471,472,425,424,524,521,0,523,0,0,0,407,408,0,274,0,415,269,272,402,0,270,406,399,0,49,0,400,54,0,279,0,151,147,149,0,236,0,220,0,286,0,234,0,297,281,0,0,126,0,0,0,144,0,126,0,234,0,201,180,251,0,22,4,6,40,0,156,139,157,0,0,155,236,165,174,173,0,0,168,0,0,0,323,0,0,0,0,0,0,343,347,0,0,0,311,304,0,41,283,234,0,244,0,0,162,237,0,160,240,228,185,207,208,226,204,482,0,0,0,261,0,266,485,0,0,0,0,0,535,490,493,0,0,496,500,0,0,474,542,0,0,522,520,0,0,0,0,273,417,420,410,413,401,280,249,160,0,287,288,220,0,0,236,220,0,0,51,236,553,0,77,79,0,193,0,0,236,0,220,0,0,0,17,23,154,0,158,130,163,175,172,172,0,0,0,0,0,0,0,0,0,0,0,0,0,323,349,0,312,42,236,0,245,243,0,186,0,162,236,0,227,531,0,530,0,262,267,0,0,0,0,0,442,443,524,523,505,0,544,527,0,547,438,541,543,0,397,398,162,160,290,296,160,0,289,220,0,130,0,82,194,200,160,0,199,195,220,0,0,0,0,0,0,0,171,170,317,323,0,0,0,0,0,0,351,352,354,356,358,360,362,364,367,372,375,378,382,384,390,391,0,0,320,329,0,0,0,0,0,0,0,0,0,0,350,230,246,0,238,187,229,234,484,0,0,268,486,487,0,0,492,491,0,511,505,501,503,497,528,0,293,162,162,160,291,53,0,0,162,160,196,31,25,0,0,27,0,7,159,123,0,0,0,323,0,388,0,0,385,323,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,386,323,0,331,0,0,0,339,0,0,0,0,0,0,322,0,0,236,533,532,0,0,0,0,512,511,508,506,0,502,529,292,295,162,122,0,197,162,26,24,28,0,0,19,176,318,319,0,0,0,0,323,325,355,0,357,359,361,363,365,366,370,371,368,369,373,374,376,377,379,380,381,0,324,330,332,333,0,341,340,0,335,0,0,0,161,239,231,0,0,0,0,0,514,513,0,507,504,294,0,198,29,18,20,0,321,387,0,383,326,0,323,334,342,336,337,0,264,263,488,0,510,0,509,83,21,0,353,323,327,338,0,515,389,328,489,0,0,0};
# 1843
static short Cyc_yydefgoto[162U]={1139,54,55,56,57,491,883,1045,795,796,967,674,58,323,59,307,60,493,61,495,62,153,63,64,560,246,477,478,247,67,262,248,69,175,176,70,173,71,284,285,138,139,140,286,249,459,506,507,508,684,828,72,73,689,690,691,74,509,75,483,76,77,170,171,78,123,556,338,727,647,79,648,550,718,542,546,547,453,331,271,103,104,573,498,574,432,433,434,250,324,325,649,465,310,311,312,313,314,315,810,316,317,896,897,898,899,900,901,902,903,904,905,906,907,908,909,910,911,435,444,445,436,437,438,318,209,412,210,565,211,212,213,214,215,216,217,218,219,220,221,222,370,371,850,947,948,1028,949,1030,1097,223,224,835,225,592,593,226,227,81,968,439,469};
# 1863
static short Cyc_yypact[1142U]={3082,- -32768,- -32768,- -32768,- -32768,- 75,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3864,154,- 65,- -32768,3864,1496,- -32768,193,15,- -32768,24,106,153,98,192,205,- -32768,- -32768,223,281,293,- -32768,262,- -32768,- -32768,- -32768,251,288,656,310,299,288,- -32768,- -32768,326,336,395,2938,- -32768,476,486,- -32768,253,3864,3864,3864,- -32768,3864,- -32768,- -32768,477,- -32768,193,3773,140,314,187,767,- -32768,- -32768,414,412,455,- -32768,193,504,7339,- -32768,- -32768,1587,540,- -32768,- -32768,- -32768,- -32768,574,414,547,7339,- -32768,531,1587,590,592,598,- -32768,23,- -32768,- -32768,627,4082,4082,353,629,2938,2938,7339,332,- -32768,307,625,7339,- -32768,317,633,307,4567,- -32768,2938,2938,3225,- -32768,2938,3225,2938,3225,- -32768,651,671,- -32768,3638,- -32768,- -32768,- -32768,- -32768,4567,- -32768,- -32768,414,213,1895,- -32768,3773,253,696,4082,3952,5418,- -32768,140,- -32768,670,- -32768,- -32768,- -32768,- -32768,692,- -32768,- -32768,- 41,767,4082,- -32768,- -32768,699,657,717,193,7746,724,7844,7339,7550,728,730,734,746,764,766,771,777,778,779,782,7844,7844,- -32768,- -32768,876,7942,2640,785,7942,7942,- -32768,- -32768,- -32768,243,- -32768,- -32768,38,811,783,774,776,594,194,589,- 105,174,- -32768,984,7942,144,63,- -32768,807,238,- -32768,1587,80,817,808,818,343,1266,- -32768,- -32768,830,7339,414,1266,820,822,4303,4567,4655,4567,545,- -32768,47,47,- -32768,846,831,- -32768,- -32768,422,414,- -32768,- -32768,- -32768,- -32768,71,833,834,- -32768,- -32768,614,446,- -32768,- -32768,- -32768,837,- -32768,838,- -32768,840,- -32768,317,5531,3773,696,844,4567,- -32768,653,847,193,848,845,221,849,4725,850,867,855,862,5644,2495,4725,375,851,- -32768,- -32768,856,2045,2045,253,2045,- -32768,- -32768,- -32768,863,- -32768,- -32768,- -32768,142,- -32768,696,866,- -32768,- -32768,854,26,881,- -32768,51,865,868,365,869,679,870,7339,4082,- -32768,879,- -32768,- -32768,26,193,- -32768,7339,880,2640,- -32768,4567,2640,- -32768,- -32768,- -32768,4838,- -32768,911,7339,7339,7339,7339,7339,7339,899,7339,4567,702,7339,- -32768,- -32768,- -32768,- -32768,883,- -32768,2045,878,466,7339,- -32768,- -32768,7339,- -32768,7942,7339,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7339,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7339,- -32768,307,- -32768,- -32768,5757,307,7339,- -32768,886,414,- -32768,882,887,888,- -32768,118,574,307,7339,1587,87,- -32768,- -32768,- -32768,891,894,889,1587,- -32768,- -32768,- -32768,896,895,- -32768,418,808,898,908,4082,- -32768,913,907,- -32768,4655,4655,4655,- -32768,- -32768,3361,5870,500,- -32768,260,- -32768,- -32768,51,- -32768,- -32768,909,929,- -32768,918,- -32768,912,914,919,- -32768,- -32768,1429,- -32768,289,268,- -32768,- -32768,- -32768,4567,- -32768,- -32768,1000,- -32768,2938,- -32768,2938,- -32768,- -32768,- -32768,- -32768,696,- -32768,- -32768,- -32768,828,7339,926,924,- -32768,5,348,414,414,802,7339,7339,920,928,7339,922,1026,2345,931,- -32768,- -32768,- -32768,432,1013,- -32768,5983,2195,2785,- -32768,- -32768,3638,- -32768,- -32768,- -32768,- -32768,4082,- -32768,- -32768,4567,944,4391,- -32768,- -32768,936,1001,51,946,4479,868,- -32768,- -32768,- -32768,- -32768,679,959,178,858,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,960,967,962,986,964,- -32768,- -32768,684,5531,965,979,982,983,987,522,978,985,988,152,990,1004,998,7648,- -32768,- -32768,992,1003,- -32768,811,287,783,774,776,594,194,194,589,589,589,589,- 105,- 105,174,174,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,999,- -32768,81,4082,5302,4567,- -32768,4567,- -32768,994,- -32768,- -32768,- -32768,- -32768,1174,- -32768,- -32768,- -32768,1411,- -32768,1011,- -32768,- -32768,341,- -32768,4567,- -32768,- -32768,- -32768,1005,868,1006,679,995,260,4082,4173,6096,- -32768,- -32768,7339,1017,414,7452,1010,71,3497,1012,414,4082,3952,6209,- -32768,289,- -32768,1021,1103,- -32768,- -32768,- -32768,1008,- -32768,- -32768,653,1018,7339,- -32768,868,- -32768,- -32768,- -32768,1025,193,575,533,549,7339,832,563,4725,1020,6322,6435,663,- -32768,- -32768,1028,1031,1023,2045,- -32768,3773,- -32768,854,1033,4082,- -32768,1034,51,1079,- -32768,1032,1001,111,- -32768,- -32768,- -32768,- -32768,858,- -32768,1042,403,1042,1039,- -32768,4954,- -32768,- -32768,7339,7339,1147,7339,7550,- -32768,- -32768,- -32768,307,307,- -32768,1036,1040,5070,- -32768,- -32768,7339,7339,- -32768,- -32768,26,742,1064,1065,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1001,1044,- -32768,- -32768,679,26,1045,868,679,1043,573,- -32768,868,1052,1050,- -32768,- -32768,1051,- -32768,26,1054,868,1056,679,1046,3225,1066,1144,- -32768,- -32768,7339,- -32768,4567,- -32768,1061,97,802,4725,1069,1067,1092,1070,1075,4725,7339,6548,676,6661,706,6774,832,- -32768,1086,- -32768,- -32768,868,342,- -32768,- -32768,1082,- -32768,1098,1079,868,4567,- -32768,- -32768,528,- -32768,7339,- -32768,- -32768,5531,1088,1089,1087,1096,- -32768,911,1095,1097,6,1099,- -32768,- -32768,760,- -32768,- -32768,- -32768,- -32768,5302,- -32768,- -32768,1079,1001,- -32768,- -32768,1001,1100,- -32768,679,1101,4567,1117,- -32768,- -32768,- -32768,1001,1102,- -32768,- -32768,679,1105,385,1115,2938,1111,1119,4567,- -32768,- -32768,1220,832,1122,8040,1116,2785,7942,1113,- -32768,43,- -32768,1154,1109,744,796,204,804,555,219,- -32768,- -32768,- -32768,- -32768,1158,7942,2045,- -32768,- -32768,582,4725,584,6887,4725,585,7000,7113,711,1129,- -32768,- -32768,- -32768,7339,1130,- -32768,- -32768,1033,- -32768,475,367,- -32768,- -32768,- -32768,4567,1235,- -32768,- -32768,1127,39,373,- -32768,- -32768,- -32768,- -32768,5186,- -32768,1079,1079,1001,- -32768,- -32768,1134,1135,1079,1001,- -32768,- -32768,- -32768,1137,1138,712,332,- -32768,- -32768,- -32768,595,4725,1140,832,2640,- -32768,4567,1128,- -32768,1745,7942,7339,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7942,7339,- -32768,832,1143,- -32768,4725,4725,597,- -32768,4725,4725,616,4725,617,7226,- -32768,1141,51,868,- -32768,- -32768,2785,1142,1136,7339,1163,394,- -32768,- -32768,1164,- -32768,- -32768,- -32768,- -32768,1079,- -32768,1159,- -32768,1079,- -32768,- -32768,- -32768,332,1152,727,- -32768,- -32768,- -32768,1153,1148,1157,7942,832,- -32768,811,400,783,774,774,594,194,194,589,589,589,589,- 105,- 105,174,174,- -32768,- -32768,- -32768,407,- -32768,- -32768,- -32768,- -32768,4725,- -32768,- -32768,4725,- -32768,4725,4725,659,- -32768,- -32768,- -32768,1161,775,1155,4567,668,- -32768,1165,1164,- -32768,- -32768,- -32768,1162,- -32768,- -32768,- -32768,- -32768,332,- -32768,1042,403,- -32768,- -32768,7339,1745,- -32768,- -32768,- -32768,- -32768,4725,- -32768,- -32768,- -32768,1167,- -32768,1183,- -32768,- -32768,- -32768,603,- -32768,832,- -32768,- -32768,1170,- -32768,- -32768,- -32768,- -32768,1282,1299,- -32768};
# 1981
static short Cyc_yypgoto[162U]={- -32768,- 53,- -32768,- -32768,- -32768,- -32768,- -32768,199,- -32768,- -32768,265,- -32768,- -32768,- 213,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,53,- 125,46,- -32768,- -32768,0,649,- -32768,124,- 163,1189,32,- -32768,- -32768,- 123,- -32768,- 45,1276,- 744,- -32768,- -32768,- -32768,1037,1038,859,428,- -32768,- -32768,637,- 189,- 692,- -32768,- -32768,104,- -32768,- -32768,166,- 260,1243,- 465,- 17,- -32768,1151,- -32768,- -32768,1272,- 405,- -32768,599,- 135,- 152,- 128,- 382,308,605,613,- 460,- 518,- 121,- 429,- 133,- -32768,- 234,- 171,- 537,- 221,- -32768,897,16,- 39,- 111,- 56,- 305,18,- -32768,- -32768,- 54,- 272,- -32768,- 781,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,146,1090,- -32768,701,890,- -32768,351,841,- -32768,- 180,- 387,- 168,- 378,- 376,- 338,949,- 332,- 360,- 298,- 363,- 354,- 190,678,- -32768,- -32768,- -32768,- -32768,384,- -32768,- 923,309,- -32768,534,971,229,- -32768,- 383,- -32768,447,492,- 67,- 70,- 109,- 202};
# 2005
static short Cyc_yytable[8193U]={65,149,351,595,148,132,270,597,657,482,372,263,354,377,378,355,282,263,339,671,155,65,329,1029,332,65,715,601,602,330,607,608,68,413,618,532,533,926,535,395,82,609,610,396,627,671,598,497,259,124,470,148,233,68,600,345,885,68,65,672,161,341,256,257,65,65,65,84,65,87,501,88,251,252,65,159,272,273,274,43,275,276,277,278,457,757,682,283,475,719,68,603,604,605,606,337,68,68,68,320,68,588,687,688,288,290,68,538,1100,975,142,143,144,712,145,65,65,543,679,466,154,945,672,540,66,767,959,152,109,65,65,65,575,65,65,65,65,381,932,562,466,65,983,683,946,66,706,68,68,66,241,65,723,65,945,539,110,242,652,467,159,68,68,68,564,68,68,68,68,953,722,43,105,68,420,1126,545,1027,544,652,467,382,66,68,476,68,984,154,66,66,66,468,66,464,152,1050,448,309,66,154,424,1055,487,552,308,421,319,611,612,613,379,113,853,485,474,125,425,374,629,563,487,665,755,1076,541,- 169,430,431,500,141,114,230,463,414,415,416,229,328,111,66,66,744,415,416,665,771,- 234,456,237,- 234,231,564,484,66,66,66,- 559,66,66,66,66,454,1034,1035,47,66,769,42,537,1039,510,379,156,1112,157,66,45,66,417,458,158,85,65,418,419,112,417,677,389,390,266,745,419,457,457,457,799,559,991,992,65,19,20,800,397,616,42,65,65,619,65,807,520,725,68,45,545,48,115,527,321,626,468,398,399,564,391,392,154,52,53,116,68,1132,42,319,993,994,561,68,68,734,68,45,1101,172,88,119,1103,999,1137,230,289,309,309,515,309,117,516,717,308,308,567,308,568,569,42,863,1000,1001,231,867,379,65,120,45,423,622,380,137,584,42,443,48,47,121,878,49,443,44,45,46,653,866,654,52,53,545,870,750,655,575,481,934,785,68,66,876,462,709,270,884,641,47,666,122,379,487,785,309,260,667,126,668,66,753,308,42,261,669,42,66,66,127,66,821,45,487,208,45,675,928,676,487,463,42,321,48,49,933,235,49,129,44,45,46,456,456,456,52,53,42,130,957,484,656,685,686,258,44,45,46,379,106,963,534,766,929,441,253,710,664,504,670,254,468,468,510,458,458,458,328,80,65,551,65,379,66,42,839,711,528,1031,564,678,1023,44,45,46,529,756,946,840,965,86,966,260,834,107,774,108,65,131,1098,261,68,775,68,575,379,65,830,1027,788,65,790,379,106,1113,- 15,789,80,773,1018,637,1114,172,379,708,106,80,379,375,68,856,80,787,473,703,719,454,379,68,147,545,150,68,154,80,845,174,864,846,855,700,488,154,177,628,861,154,489,107,707,823,874,634,152,260,1021,42,447,726,831,107,379,261,666,44,45,46,590,133,134,146,1056,938,80,80,1058,147,487,135,136,487,780,80,66,564,66,836,80,80,80,839,80,80,80,80,1062,1063,178,1068,1069,461,847,848,840,1090,758,462,759,1004,1070,1071,66,80,510,1059,1060,526,379,656,230,66,822,1061,740,66,454,234,510,379,935,510,236,- 256,936,805,- 256,177,670,231,954,824,48,955,106,379,461,106,393,394,106,806,462,961,52,53,106,387,388,379,1064,1065,1066,1067,375,811,997,375,803,379,998,804,981,232,65,869,65,1054,379,581,379,379,708,238,1005,839,1007,1011,239,107,1003,488,107,379,42,107,240,1047,840,1080,596,107,44,45,46,1136,68,880,68,936,89,48,379,379,243,481,255,564,1083,1085,309,462,154,52,53,726,264,308,486,319,554,555,973,42,1036,267,42,620,190,80,1040,44,45,46,279,45,486,47,512,764,48,517,343,48,504,379,817,49,505,379,65,1119,52,53,379,52,53,280,80,920,1124,335,379,1020,687,688,1072,1073,1074,732,733,148,42,92,197,198,93,94,95,322,889,45,68,336,1022,342,916,970,66,923,66,177,379,349,1016,1043,202,379,1044,1131,162,163,164,165,166,344,89,167,168,169,1106,808,809,1107,347,348,350,352,352,356,1111,357,692,693,545,358,696,857,858,701,367,368,16,17,18,352,106,359,352,352,65,644,645,646,106,951,952,987,988,989,990,369,106,995,996,360,1046,361,352,148,1121,379,362,90,887,888,383,980,363,364,365,65,68,366,66,427,376,385,428,92,386,107,93,94,95,384,429,45,440,107,1130,422,97,1006,426,98,1010,107,42,99,446,100,101,68,430,431,44,45,46,47,450,912,102,451,1024,48,471,309,479,504,265,472,490,492,308,494,480,52,53,502,486,511,513,486,148,350,514,522,523,65,518,521,80,269,80,524,530,531,536,1048,1051,541,1052,486,281,468,334,548,836,486,47,553,287,549,66,566,778,381,557,582,589,68,623,269,587,621,631,624,625,352,632,636,1078,1079,633,309,1081,1082,635,1084,639,640,308,643,1046,66,1091,148,642,659,658,400,660,661,673,663,662,814,816,680,681,694,695,698,702,704,352,697,352,352,352,352,352,352,352,352,352,352,352,352,352,352,352,352,352,713,106,716,720,682,106,401,402,403,404,405,406,407,408,409,410,724,728,729,730,1115,731,735,1116,- 559,1117,1118,455,66,460,736,352,1123,737,738,741,65,411,739,892,893,747,742,42,499,743,107,746,748,751,107,44,45,46,752,89,754,760,772,1133,48,765,768,770,504,779,287,783,68,786,793,794,52,53,798,801,812,818,486,819,820,328,309,827,825,917,919,829,922,308,925,749,486,837,843,486,849,851,558,859,860,862,865,871,802,352,868,872,873,879,937,875,90,877,882,881,499,886,890,576,577,578,579,580,891,915,583,228,92,586,914,93,94,95,927,96,45,89,930,591,931,97,594,941,98,939,940,942,894,958,100,101,943,960,944,964,950,956,895,962,66,102,205,614,969,206,207,235,971,972,974,976,979,982,985,615,986,106,1002,1017,591,1019,1025,1026,1037,1053,1038,1041,1042,352,1049,1077,1094,1009,90,1102,1013,1015,1088,1093,1096,945,1105,1108,1140,1109,761,80,1110,428,92,1120,1122,93,94,95,1125,429,45,1127,1135,1141,107,97,1134,651,98,1138,1128,89,99,1104,100,101,784,430,431,268,118,496,797,160,269,102,340,503,128,833,832,1089,375,826,630,1032,449,762,599,1057,585,1099,638,1129,269,0,913,0,0,0,0,269,0,0,0,0,0,1075,0,0,0,0,0,0,0,352,0,0,90,0,0,1087,0,0,591,0,591,147,1092,80,442,1095,0,228,92,0,0,93,94,95,0,96,45,0,269,0,0,97,0,0,98,0,0,0,99,269,100,101,0,0,0,0,0,0,269,0,0,102,0,0,499,0,352,0,0,0,0,0,0,0,0,0,0,0,0,352,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,89,0,0,0,0,0,0,0,0,147,0,499,0,0,22,23,24,0,0,0,0,0,0,0,352,0,0,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,777,0,0,0,0,0,782,39,0,0,0,0,0,90,792,0,0,269,0,40,0,41,0,0,0,763,0,455,228,92,0,269,93,94,95,0,96,45,0,591,147,89,97,326,0,98,42,0,0,99,0,100,101,43,44,45,46,47,0,328,0,0,102,48,0,0,0,481,0,50,51,0,0,462,978,52,53,352,499,0,0,841,842,0,844,0,0,0,0,0,0,0,0,0,499,352,0,854,90,0,0,0,0,147,0,0,0,0,0,0,0,0,352,91,92,0,0,93,94,95,0,96,45,0,0,0,0,97,0,0,98,0,89,0,99,0,100,101,0,0,0,0,0,0,0,0,0,102,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,287,0,352,0,352,352,352,352,352,352,352,352,352,352,352,352,352,352,352,352,352,0,499,0,0,0,90,0,0,0,0,0,269,0,0,0,0,0,0,0,499,228,92,0,0,93,94,95,0,96,45,0,0,0,0,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,287,0,352,0,0,102,0,0,0,0,0,0,0,0,0,0,269,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,808,809,21,179,180,291,0,292,293,294,295,296,297,298,299,22,23,24,300,89,26,182,301,0,352,0,499,183,27,302,0,0,30,184,185,186,187,188,0,31,32,33,34,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,303,92,197,198,93,94,95,43,44,45,46,47,199,304,151,0,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,179,180,291,0,292,293,294,295,296,297,298,299,22,23,24,300,89,26,182,301,0,0,0,0,183,27,302,0,0,30,184,185,186,187,188,0,31,32,33,34,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,303,92,197,198,93,94,95,43,44,45,46,47,199,304,151,305,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,179,180,291,0,292,293,294,295,296,297,298,299,22,23,24,300,89,26,182,301,0,0,0,0,183,27,302,0,0,30,184,185,186,187,188,0,31,32,33,34,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,303,92,197,198,93,94,95,43,44,45,46,47,199,304,151,0,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,179,180,291,0,292,293,294,295,296,297,298,299,22,23,24,300,89,26,182,301,0,0,0,0,183,27,302,0,0,30,184,185,186,187,188,0,31,32,33,34,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,519,92,197,198,93,94,95,43,44,45,46,47,199,304,151,0,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,179,180,0,0,0,0,0,0,0,0,0,0,22,23,24,300,89,26,182,0,0,0,0,0,183,27,0,0,0,30,184,185,186,187,188,0,31,32,33,34,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,43,44,45,46,47,199,699,0,0,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,179,180,0,0,0,0,0,0,0,0,0,0,22,23,24,300,89,0,0,0,0,0,0,0,0,27,0,0,0,30,184,185,186,187,188,0,0,32,33,0,0,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,43,44,45,46,47,199,0,0,0,0,200,0,0,0,349,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,22,23,24,181,89,0,182,0,0,0,0,0,183,0,0,0,0,30,184,185,186,187,188,0,0,32,33,0,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,43,44,45,46,47,199,0,373,0,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,22,23,24,181,89,0,182,0,0,0,0,0,183,0,0,0,0,30,184,185,186,187,188,0,0,32,33,0,189,0,0,0,190,0,0,191,192,39,193,0,0,0,0,0,0,0,0,0,0,40,194,41,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,43,44,45,46,47,199,0,0,0,0,200,0,0,0,201,0,50,306,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,34,0,0,35,36,0,37,38,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,- 12,0,48,0,0,0,49,0,50,51,0,0,0,0,52,53,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,34,0,0,35,36,0,37,38,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,0,0,48,0,0,0,49,0,50,51,0,0,0,0,52,53,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,34,0,0,35,36,0,37,38,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,- 12,0,48,0,0,0,49,0,50,51,0,0,0,0,52,53,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,326,0,0,0,0,0,0,0,0,0,43,44,0,46,47,0,328,0,0,0,48,0,0,0,461,0,50,51,0,0,462,0,52,53,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,0,0,48,0,0,0,504,0,50,51,0,0,0,0,52,53,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,34,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,44,0,46,47,0,0,151,0,0,0,281,0,0,0,0,50,51,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,34,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,43,44,0,46,47,0,0,151,22,23,24,25,0,0,0,0,50,51,0,0,0,27,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,43,44,0,46,47,22,23,24,0,0,0,0,0,0,0,0,50,51,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,326,0,0,327,0,0,0,0,0,0,43,44,0,46,47,0,328,0,0,0,0,0,0,0,0,0,50,51,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,244,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,43,44,0,46,47,0,0,245,22,23,24,0,0,0,0,0,50,51,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,326,0,0,0,0,0,0,0,0,0,43,44,0,46,47,0,328,0,0,0,0,0,0,0,0,0,50,51,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,43,44,0,46,47,22,23,24,452,0,0,0,0,0,0,0,50,51,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,43,44,0,46,47,22,23,24,714,0,0,0,0,0,0,0,50,51,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,721,0,0,0,0,0,0,0,0,0,43,44,0,46,47,22,23,24,0,0,0,0,0,0,0,0,50,51,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,43,44,0,46,47,22,23,24,0,0,0,0,0,0,0,0,50,51,0,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,179,180,291,0,292,293,294,295,296,297,298,299,0,0,0,181,89,0,182,301,0,0,0,0,183,43,302,0,0,47,184,185,186,187,188,0,0,0,0,0,189,50,51,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,519,92,197,198,93,94,95,0,0,45,0,0,199,304,151,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,570,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,261,45,0,0,199,0,353,572,0,200,0,0,0,201,0,0,202,0,430,431,179,180,0,203,0,0,204,205,0,0,206,207,0,0,0,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,261,45,0,0,199,0,353,838,0,200,0,0,0,201,0,0,202,0,430,431,179,180,0,203,0,0,204,205,0,0,206,207,0,0,0,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,261,45,0,0,199,0,353,852,0,200,0,0,0,201,0,0,202,0,430,431,179,180,0,203,0,0,204,205,0,0,206,207,0,0,0,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,261,45,0,0,199,0,353,1033,0,200,0,0,0,201,0,0,202,0,430,431,179,180,0,203,0,0,204,205,0,0,206,207,0,0,0,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,261,45,0,0,199,0,353,0,0,200,0,0,0,201,0,0,202,0,430,431,179,180,0,203,0,0,204,205,0,0,206,207,0,0,0,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,179,180,0,333,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,353,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,525,0,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,617,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,179,180,0,650,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,705,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,179,180,0,776,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,179,180,0,791,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,813,0,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,815,0,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,918,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,921,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,924,0,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,1008,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,1012,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,1014,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,1086,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,179,180,0,0,0,0,203,0,0,204,205,0,0,206,207,181,89,0,182,0,0,0,0,0,183,0,0,0,0,0,184,185,186,187,188,0,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,781,92,197,198,93,94,95,0,0,45,179,180,199,0,0,0,0,200,0,0,0,201,0,0,202,181,89,0,0,0,0,203,0,0,204,205,0,0,206,207,184,185,186,187,188,0,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,179,180,199,0,353,0,0,200,0,0,0,201,0,0,202,181,89,0,0,0,0,203,0,0,204,205,0,0,206,207,184,185,186,187,188,0,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,179,180,199,0,749,0,0,200,0,0,0,201,0,0,202,181,89,0,0,0,0,203,0,0,204,205,0,0,206,207,184,185,186,187,188,0,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,179,180,199,0,0,0,0,200,0,0,0,346,0,0,202,181,89,0,0,0,0,203,0,0,204,205,0,0,206,207,184,185,186,187,188,0,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,179,180,199,0,0,0,0,200,0,0,0,349,0,0,202,181,89,0,0,0,0,203,0,0,204,205,0,0,206,207,184,185,186,187,188,0,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,179,180,199,0,0,0,0,200,0,0,0,201,0,0,202,181,89,0,0,0,0,203,0,0,204,205,0,0,206,207,184,185,186,187,188,0,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,197,198,93,94,95,0,0,45,0,0,199,0,0,0,0,200,0,0,0,977,0,0,202,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207};
# 2828
static short Cyc_yycheck[8193U]={0,71,182,381,71,58,127,383,468,269,200,120,183,203,204,183,141,126,170,484,74,21,157,946,157,25,544,387,388,157,393,394,0,223,417,307,308,818,310,144,115,395,396,148,431,510,384,281,118,49,252,118,97,21,386,178,800,25,58,488,77,172,115,116,64,65,66,21,68,134,283,25,111,112,74,75,129,130,131,120,133,134,135,136,247,622,81,141,17,549,58,389,390,391,392,136,64,65,66,153,68,373,5,6,149,150,74,320,1031,890,64,65,66,542,68,115,116,66,505,93,74,115,551,325,0,643,870,74,113,129,130,131,353,133,134,135,136,99,830,341,93,141,99,138,138,21,529,115,116,25,127,151,557,153,115,129,132,134,463,133,160,129,130,131,344,133,134,135,136,861,552,120,26,141,111,1098,328,138,127,484,133,143,58,151,113,153,143,141,64,65,66,144,68,249,141,976,241,151,74,153,120,982,269,331,151,142,153,397,398,399,129,113,749,269,259,49,136,201,131,342,287,481,141,1004,113,128,139,140,282,63,132,113,249,89,90,91,90,126,132,115,116,89,90,91,504,650,135,247,102,138,132,431,269,129,130,131,138,133,134,135,136,245,954,955,124,141,648,113,126,961,287,129,132,1054,134,151,122,153,134,247,140,127,282,139,140,132,134,500,94,95,124,139,140,456,457,458,683,336,94,95,300,20,21,685,130,414,113,307,308,418,310,694,294,135,282,122,468,130,126,301,154,430,144,149,150,505,132,133,282,142,143,126,300,1114,113,282,132,133,338,307,308,575,310,122,1036,132,300,54,1040,130,1131,113,139,307,308,134,310,134,137,548,307,308,346,310,348,349,113,772,149,150,132,776,129,373,112,122,230,422,135,126,364,113,236,130,124,134,791,134,242,121,122,123,132,775,134,142,143,549,780,589,140,622,134,832,664,373,282,789,140,534,531,798,451,124,481,127,129,484,678,373,113,132,112,134,300,138,373,113,121,140,113,307,308,134,310,707,122,504,87,122,493,823,495,510,461,113,280,130,134,831,99,134,126,121,122,123,456,457,458,142,143,113,126,868,481,465,511,512,117,121,122,123,129,26,879,309,135,135,135,126,534,481,134,483,131,144,144,504,456,457,458,126,0,493,129,495,129,373,113,733,539,126,129,683,504,138,121,122,123,134,621,138,733,128,22,130,113,114,26,654,28,521,127,129,121,493,654,495,749,129,530,720,138,668,534,668,129,90,138,127,668,49,653,930,126,138,132,129,530,102,58,129,201,521,756,63,667,135,126,1019,544,129,530,71,716,73,534,521,76,740,115,773,740,753,521,129,530,85,432,768,534,135,90,530,712,787,440,534,113,114,113,240,560,721,102,129,121,664,121,122,123,135,126,127,127,983,840,115,116,985,118,678,126,127,681,660,124,493,798,495,729,129,130,131,858,133,134,135,136,989,990,127,995,996,134,744,745,858,1020,623,140,625,914,997,998,521,154,664,986,987,299,129,652,113,530,709,988,135,534,643,113,678,129,135,681,134,126,139,135,129,178,671,132,862,713,130,865,230,129,134,233,92,93,236,135,140,875,142,143,242,96,97,129,991,992,993,994,346,135,144,349,126,129,148,129,895,132,707,135,709,982,129,361,129,129,697,126,135,952,135,135,129,230,913,129,233,129,113,236,131,135,952,135,382,242,121,122,123,135,707,793,709,139,41,130,129,129,120,134,120,930,135,135,707,140,709,142,143,726,134,707,269,709,84,85,886,113,956,135,113,419,69,280,962,121,122,123,126,122,287,124,289,636,130,292,128,130,134,129,126,134,138,129,793,135,142,143,129,142,143,129,309,126,135,134,129,934,5,6,999,1000,1001,128,129,881,113,114,115,116,117,118,119,126,805,122,793,134,936,129,811,883,707,126,709,342,129,134,126,126,137,129,129,1114,76,77,78,79,80,131,41,83,84,85,126,22,23,129,179,134,181,182,183,134,1053,134,514,515,1019,134,518,128,129,521,195,196,17,18,19,200,432,134,203,204,883,456,457,458,440,128,129,146,147,96,97,18,448,92,93,134,969,134,223,969,128,129,134,98,803,804,98,894,134,134,134,914,883,134,793,110,134,146,113,114,147,432,117,118,119,145,121,122,113,440,1113,127,127,918,120,130,921,448,113,134,113,136,137,914,139,140,121,122,123,124,133,808,147,134,941,130,113,914,128,134,122,133,128,128,914,128,135,142,143,128,481,127,127,484,1044,300,134,113,126,982,134,134,493,127,495,126,138,134,128,974,977,113,979,504,131,144,158,135,1110,510,124,135,146,138,883,128,658,99,141,113,135,982,133,157,134,132,128,133,133,344,129,129,1007,1008,138,982,1011,1012,135,1014,135,126,982,129,1107,914,1023,1107,128,113,134,60,127,134,47,129,135,699,700,126,129,134,127,30,126,45,381,138,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,134,632,144,135,81,636,100,101,102,103,104,105,106,107,108,109,135,135,129,135,1080,113,135,1083,138,1085,1086,246,982,248,129,431,1094,129,129,135,1114,131,129,25,26,115,135,113,281,135,632,135,128,135,636,121,122,123,129,41,135,141,141,1119,130,128,135,135,134,126,285,135,1114,135,127,46,142,143,134,128,134,127,664,126,135,126,1114,82,128,812,813,133,815,1114,817,127,678,132,25,681,138,135,335,113,113,135,135,129,690,505,141,135,135,141,837,135,98,135,48,127,353,134,127,356,357,358,359,360,135,128,363,113,114,366,138,117,118,119,126,121,122,41,134,376,120,127,379,134,130,135,135,129,134,126,136,137,135,114,135,128,135,135,144,135,1114,147,148,400,127,151,152,894,135,128,28,127,134,138,98,412,145,808,98,128,417,129,25,134,128,135,129,128,128,589,128,126,134,920,98,114,923,924,135,135,115,115,128,128,0,135,110,793,129,113,114,128,135,117,118,119,129,121,122,135,115,0,808,127,135,462,130,135,1107,41,134,1044,136,137,663,139,140,126,40,280,681,76,461,147,171,285,52,726,721,1019,977,716,433,947,242,632,385,984,365,1028,448,1110,481,- 1,808,- 1,- 1,- 1,- 1,488,- 1,- 1,- 1,- 1,- 1,1002,- 1,- 1,- 1,- 1,- 1,- 1,- 1,683,- 1,- 1,98,- 1,- 1,1016,- 1,- 1,529,- 1,531,881,1023,883,110,1026,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,531,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,542,136,137,- 1,- 1,- 1,- 1,- 1,- 1,551,- 1,- 1,147,- 1,- 1,575,- 1,740,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,753,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,969,- 1,622,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,798,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,655,- 1,- 1,- 1,- 1,- 1,661,74,- 1,- 1,- 1,- 1,- 1,98,669,- 1,- 1,654,- 1,86,- 1,88,- 1,- 1,- 1,110,- 1,664,113,114,- 1,668,117,118,119,- 1,121,122,- 1,694,1044,41,127,110,- 1,130,113,- 1,- 1,134,- 1,136,137,120,121,122,123,124,- 1,126,- 1,- 1,147,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,892,142,143,895,733,- 1,- 1,736,737,- 1,739,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,749,913,- 1,752,98,- 1,- 1,- 1,- 1,1107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,930,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,41,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,800,- 1,983,- 1,985,986,987,988,989,990,991,992,993,994,995,996,997,998,999,1000,1001,- 1,840,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,832,- 1,- 1,- 1,- 1,- 1,- 1,- 1,858,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,870,- 1,1053,- 1,- 1,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,886,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,1113,- 1,952,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,- 1,62,63,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,- 1,62,63,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,67,68,- 1,70,71,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,67,68,- 1,70,71,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,67,68,- 1,70,71,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,131,- 1,- 1,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,37,38,39,40,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,37,38,39,- 1,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,37,38,39,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,37,38,39,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,120,51,- 1,- 1,124,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,136,137,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,32,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp423[8U]="stdcall";static char _tmp424[6U]="cdecl";static char _tmp425[9U]="fastcall";static char _tmp426[9U]="noreturn";static char _tmp427[6U]="const";static char _tmp428[8U]="aligned";static char _tmp429[7U]="packed";static char _tmp42A[7U]="shared";static char _tmp42B[7U]="unused";static char _tmp42C[5U]="weak";static char _tmp42D[10U]="dllimport";static char _tmp42E[10U]="dllexport";static char _tmp42F[23U]="no_instrument_function";static char _tmp430[12U]="constructor";static char _tmp431[11U]="destructor";static char _tmp432[22U]="no_check_memory_usage";static char _tmp433[5U]="pure";static char _tmp434[14U]="always_inline";static char _tmp435[9U]="no_throw";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp395=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp395;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp768;(_tmp768.YYINITIALSVAL).tag=72U,(_tmp768.YYINITIALSVAL).val=0;_tmp768;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp6AC=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp6AC),sizeof(short),_tmp6AC);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp6AB=200U;struct Cyc_Yystacktype*_tmp6AA=({struct _RegionHandle*_tmp823=yyregion;_region_malloc(_tmp823,_check_times(_tmp6AB,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp767=200U;unsigned i;for(i=0;i < _tmp767;++ i){(_tmp6AA[i]).v=yylval,({struct Cyc_Yyltype _tmp824=Cyc_yynewloc();(_tmp6AA[i]).l=_tmp824;});}}0;});_tmp6AA;}),sizeof(struct Cyc_Yystacktype),200U);
# 174
struct Cyc_Yystacktype*yyyvsp;
# 177
int yystacksize=200;
# 179
union Cyc_YYSTYPE yyval=yylval;
# 183
int yylen;
# 190
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 200
yyssp_offset=-1;
yyvsp_offset=0;
# 206
yynewstate:
# 208
*((short*)_check_fat_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 210
if(yyssp_offset >= (yystacksize - 1)- 12){
# 212
if(yystacksize >= 10000){
({struct _fat_ptr _tmp826=({const char*_tmp396="parser stack overflow";_tag_fat(_tmp396,sizeof(char),22U);});int _tmp825=yystate;Cyc_yyerror(_tmp826,_tmp825,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp39A=(unsigned)yystacksize;short*_tmp399=({struct _RegionHandle*_tmp827=yyregion;_region_malloc(_tmp827,_check_times(_tmp39A,sizeof(short)));});({{unsigned _tmp71E=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp71E;++ i){
i <= (unsigned)yyssp_offset?_tmp399[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp399[i]=0);}}0;});_tag_fat(_tmp399,sizeof(short),_tmp39A);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp398=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp397=({struct _RegionHandle*_tmp828=yyregion;_region_malloc(_tmp828,_check_times(_tmp398,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp71D=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp71D;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp397[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp397[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp397,sizeof(struct Cyc_Yystacktype),_tmp398);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1142U,sizeof(short),yystate));
if(yyn == -32768)goto yydefault;
# 261
if(yychar == -2)
# 267
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 271
if(yychar <= 0){
# 273
yychar1=0;
yychar=0;}else{
# 282
yychar1=yychar > 0 && yychar <= 380?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,381U,sizeof(short),yychar)): 315;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 8192)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8193U,sizeof(short),yyn))!= yychar1)goto yydefault;
# 302
yyn=(int)Cyc_yytable[yyn];
# 309
if(yyn < 0){
# 311
if(yyn == -32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn == 0)goto yyerrlab;}
# 317
if(yyn == 1141){
int _tmp39B=0;_npop_handler(0U);return _tmp39B;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp829=({struct Cyc_Yystacktype _tmp71F;_tmp71F.v=yylval,_tmp71F.l=yylloc;_tmp71F;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp829;});
# 338
if(yyerrstatus != 0)-- yyerrstatus;
# 340
yystate=yyn;
goto yynewstate;
# 344
yydefault:
# 346
 yyn=(int)Cyc_yydefact[yystate];
if(yyn == 0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,563U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp39C=yyn;switch(_tmp39C){case 1U: _LL1: _LL2:
# 1145 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1148
goto _LL0;case 2U: _LL3: _LL4:
# 1151 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp82A=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp82A,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1155 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct Cyc_Absyn_Decl*_tmp82E=({struct Cyc_Absyn_Decl*_tmp39E=_cycalloc(sizeof(*_tmp39E));({void*_tmp82D=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->tag=11U,({struct _tuple0*_tmp82C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39D->f1=_tmp82C;}),({struct Cyc_List_List*_tmp82B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39D->f2=_tmp82B;});_tmp39D;});_tmp39E->r=_tmp82D;}),_tmp39E->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39E;});_tmp39F->hd=_tmp82E;}),_tmp39F->tl=0;_tmp39F;}));
Cyc_Lex_leave_using();
# 1158
goto _LL0;case 4U: _LL7: _LL8:
# 1159 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct Cyc_Absyn_Decl*_tmp833=({struct Cyc_Absyn_Decl*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({void*_tmp832=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->tag=11U,({struct _tuple0*_tmp831=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A0->f1=_tmp831;}),({struct Cyc_List_List*_tmp830=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A0->f2=_tmp830;});_tmp3A0;});_tmp3A1->r=_tmp832;}),_tmp3A1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A1;});_tmp3A2->hd=_tmp833;}),({struct Cyc_List_List*_tmp82F=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A2->tl=_tmp82F;});_tmp3A2;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1162
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct Cyc_Absyn_Decl*_tmp838=({struct Cyc_Absyn_Decl*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({void*_tmp837=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->tag=10U,({struct _fat_ptr*_tmp836=({struct _fat_ptr*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct _fat_ptr _tmp835=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A3=_tmp835;});_tmp3A3;});_tmp3A4->f1=_tmp836;}),({struct Cyc_List_List*_tmp834=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A4->f2=_tmp834;});_tmp3A4;});_tmp3A5->r=_tmp837;}),_tmp3A5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A5;});_tmp3A6->hd=_tmp838;}),_tmp3A6->tl=0;_tmp3A6;}));
Cyc_Lex_leave_namespace();
# 1165
goto _LL0;case 6U: _LLB: _LLC:
# 1166 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct Cyc_Absyn_Decl*_tmp83E=({struct Cyc_Absyn_Decl*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({void*_tmp83D=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=10U,({struct _fat_ptr*_tmp83C=({struct _fat_ptr*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct _fat_ptr _tmp83B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A7=_tmp83B;});_tmp3A7;});_tmp3A8->f1=_tmp83C;}),({struct Cyc_List_List*_tmp83A=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A8->f2=_tmp83A;});_tmp3A8;});_tmp3A9->r=_tmp83D;}),_tmp3A9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A9;});_tmp3AA->hd=_tmp83E;}),({struct Cyc_List_List*_tmp839=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3AA->tl=_tmp839;});_tmp3AA;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1168 "parse.y"
int _tmp3AB=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3AB;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3AC=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp15=_tmp3AC;struct _tuple28*_tmp3AD=_stmttmp15;unsigned _tmp3AF;struct Cyc_List_List*_tmp3AE;_LL464: _tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_LL465: {struct Cyc_List_List*exs=_tmp3AE;unsigned wc=_tmp3AF;
struct Cyc_List_List*_tmp3B0=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3B0;
if(exs != 0 && hides != 0)
({void*_tmp3B1=0U;({unsigned _tmp840=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp83F=({const char*_tmp3B2="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3B2,sizeof(char),65U);});Cyc_Warn_err(_tmp840,_tmp83F,_tag_fat(_tmp3B1,sizeof(void*),0U));});});
# 1175
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1178
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3B3=0U;({unsigned _tmp842=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp841=({const char*_tmp3B4="expecting \"C include\"";_tag_fat(_tmp3B4,sizeof(char),22U);});Cyc_Warn_err(_tmp842,_tmp841,_tag_fat(_tmp3B3,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct Cyc_Absyn_Decl*_tmp847=({struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({void*_tmp846=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=13U,({struct Cyc_List_List*_tmp845=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B6->f1=_tmp845;}),_tmp3B6->f2=cycdecls,_tmp3B6->f3=exs,({struct _tuple10*_tmp844=({struct _tuple10*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->f1=wc,_tmp3B5->f2=hides;_tmp3B5;});_tmp3B6->f4=_tmp844;});_tmp3B6;});_tmp3B7->r=_tmp846;}),_tmp3B7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B7;});_tmp3B8->hd=_tmp847;}),({struct Cyc_List_List*_tmp843=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B8->tl=_tmp843;});_tmp3B8;}));}else{
# 1184
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_Absyn_Decl*_tmp84B=({struct Cyc_Absyn_Decl*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({void*_tmp84A=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=12U,({struct Cyc_List_List*_tmp849=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B9->f1=_tmp849;});_tmp3B9;});_tmp3BA->r=_tmp84A;}),_tmp3BA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BA;});_tmp3BB->hd=_tmp84B;}),({struct Cyc_List_List*_tmp848=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BB->tl=_tmp848;});_tmp3BB;}));}}else{
# 1188
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct Cyc_Absyn_Decl*_tmp850=({struct Cyc_Absyn_Decl*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({void*_tmp84F=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=13U,({struct Cyc_List_List*_tmp84E=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BD->f1=_tmp84E;}),_tmp3BD->f2=cycdecls,_tmp3BD->f3=exs,({struct _tuple10*_tmp84D=({struct _tuple10*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->f1=wc,_tmp3BC->f2=hides;_tmp3BC;});_tmp3BD->f4=_tmp84D;});_tmp3BD;});_tmp3BE->r=_tmp84F;}),_tmp3BE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BE;});_tmp3BF->hd=_tmp850;}),({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BF->tl=_tmp84C;});_tmp3BF;}));}
# 1191
goto _LL0;}}case 8U: _LLF: _LL10:
# 1192 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({struct Cyc_Absyn_Decl*_tmp852=({struct Cyc_Absyn_Decl*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C0;});_tmp3C1->hd=_tmp852;}),({struct Cyc_List_List*_tmp851=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C1->tl=_tmp851;});_tmp3C1;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1194 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct Cyc_Absyn_Decl*_tmp854=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C2;});_tmp3C3->hd=_tmp854;}),({struct Cyc_List_List*_tmp853=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C3->tl=_tmp853;});_tmp3C3;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1196 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct Cyc_Absyn_Decl*_tmp856=({struct Cyc_Absyn_Decl*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C4;});_tmp3C5->hd=_tmp856;}),({struct Cyc_List_List*_tmp855=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C5->tl=_tmp855;});_tmp3C5;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1198 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct Cyc_Absyn_Decl*_tmp858=({struct Cyc_Absyn_Decl*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3C6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C6;});_tmp3C7->hd=_tmp858;}),({struct Cyc_List_List*_tmp857=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C7->tl=_tmp857;});_tmp3C7;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1199 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1204 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1209 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1214 "parse.y"
struct _fat_ptr _tmp3C8=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3C8;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp859=(struct _fat_ptr)two;Cyc_strcmp(_tmp859,({const char*_tmp3C9="C";_tag_fat(_tmp3C9,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp85A=(struct _fat_ptr)two;Cyc_strcmp(_tmp85A,({const char*_tmp3CA="C include";_tag_fat(_tmp3CA,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1221
({void*_tmp3CB=0U;({unsigned _tmp85C=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp85B=({const char*_tmp3CC="expecting \"C\" or \"C include\"";_tag_fat(_tmp3CC,sizeof(char),29U);});Cyc_Warn_err(_tmp85C,_tmp85B,_tag_fat(_tmp3CB,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1225
goto _LL0;}case 16U: _LL1F: _LL20:
# 1228 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1232 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1233 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1237 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct _tuple0*_tmp85D=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CD->hd=_tmp85D;}),_tmp3CD->tl=0;_tmp3CD;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1238 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct _tuple0*_tmp85E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CE->hd=_tmp85E;}),_tmp3CE->tl=0;_tmp3CE;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1240 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct _tuple0*_tmp860=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CF->hd=_tmp860;}),({struct Cyc_List_List*_tmp85F=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3CF->tl=_tmp85F;});_tmp3CF;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1244 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->f1=0,_tmp3D0->f2=0U;_tmp3D0;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1245 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1249 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct Cyc_List_List*_tmp861=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D1->f1=_tmp861;}),_tmp3D1->f2=0U;_tmp3D1;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1250 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=0,_tmp3D2->f2=0U;_tmp3D2;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1251 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=0,_tmp3D3->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D3;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1255 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct _tuple32*_tmp863=({struct _tuple32*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp862=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D4->f2=_tmp862;}),_tmp3D4->f3=0;_tmp3D4;});_tmp3D5->hd=_tmp863;}),_tmp3D5->tl=0;_tmp3D5;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1256 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct _tuple32*_tmp865=({struct _tuple32*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp864=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D6->f2=_tmp864;}),_tmp3D6->f3=0;_tmp3D6;});_tmp3D7->hd=_tmp865;}),_tmp3D7->tl=0;_tmp3D7;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1258 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct _tuple32*_tmp868=({struct _tuple32*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp867=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D8->f2=_tmp867;}),_tmp3D8->f3=0;_tmp3D8;});_tmp3D9->hd=_tmp868;}),({struct Cyc_List_List*_tmp866=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D9->tl=_tmp866;});_tmp3D9;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1262 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1263 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1267 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({struct Cyc_Absyn_Decl*_tmp86B=({void*_tmp86A=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp869=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3DA->f1=_tmp869;});_tmp3DA;});Cyc_Absyn_new_decl(_tmp86A,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3DB->hd=_tmp86B;}),_tmp3DB->tl=0;_tmp3DB;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1268 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1269 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1278 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp86E=yyr;struct Cyc_Parse_Declarator _tmp86D=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp86C=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp86E,0,_tmp86D,0,_tmp86C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1280 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DC;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp871=yyr;struct Cyc_Parse_Declarator _tmp870=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp86F=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp871,& d,_tmp870,0,_tmp86F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1293 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp875=yyr;struct Cyc_Parse_Declarator _tmp874=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp873=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp872=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp875,0,_tmp874,_tmp873,_tmp872,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1295 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DD=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DD;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp879=yyr;struct Cyc_Parse_Declarator _tmp878=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp877=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp876=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp879,& d,_tmp878,_tmp877,_tmp876,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1303 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DE=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DE;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp87C=yyr;struct Cyc_Parse_Declarator _tmp87B=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp87A=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp87C,& d,_tmp87B,0,_tmp87A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1306 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DF=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DF;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp880=yyr;struct Cyc_Parse_Declarator _tmp87F=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp87E=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp87D=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp880,& d,_tmp87F,_tmp87E,_tmp87D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1311 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1314
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1317
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _fat_ptr _tmp881=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3E0=_tmp881;});_tmp3E0;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1320
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1326 "parse.y"
int _tmp3E1=((yyyvsp[0]).l).first_line;int location=_tmp3E1;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp883=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp882=(unsigned)location;Cyc_Parse_make_declarations(_tmp883,0,_tmp882,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1329 "parse.y"
int _tmp3E2=((yyyvsp[0]).l).first_line;int location=_tmp3E2;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp886=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp885=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp884=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp886,_tmp885,_tmp884,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1333
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));({struct Cyc_Absyn_Decl*_tmp889=({struct Cyc_Absyn_Pat*_tmp888=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp887=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp888,_tmp887,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E3->hd=_tmp889;}),_tmp3E3->tl=0;_tmp3E3;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1335 "parse.y"
struct Cyc_List_List*_tmp3E4=0;struct Cyc_List_List*vds=_tmp3E4;
{struct Cyc_List_List*_tmp3E5=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3E5;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3E6=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3E6;
struct _tuple0*qv=({struct _tuple0*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({union Cyc_Absyn_Nmspace _tmp88A=Cyc_Absyn_Rel_n(0);_tmp3E9->f1=_tmp88A;}),_tmp3E9->f2=id;_tmp3E9;});
struct Cyc_Absyn_Vardecl*_tmp3E7=({struct _tuple0*_tmp88B=qv;Cyc_Absyn_new_vardecl(0U,_tmp88B,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3E7;
vds=({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8->hd=vd,_tmp3E8->tl=vds;_tmp3E8;});}}
# 1342
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({struct Cyc_Absyn_Decl*_tmp88C=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EA->hd=_tmp88C;}),_tmp3EA->tl=0;_tmp3EA;}));
# 1345
goto _LL0;}case 51U: _LL61: _LL62: {
# 1348 "parse.y"
struct _fat_ptr _tmp3EB=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3EB;
Cyc_Parse_tvar_ok(three,(unsigned)((yyyvsp[2]).l).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));({struct _fat_ptr*_tmp88E=({struct _fat_ptr*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));*_tmp3EF=three;_tmp3EF;});_tmp3F0->name=_tmp88E;}),_tmp3F0->identity=- 1,({void*_tmp88D=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3F0->kind=_tmp88D;});_tmp3F0;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp892=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp891=({struct _tuple0*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp890=({struct _fat_ptr*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));({struct _fat_ptr _tmp88F=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3ED=_tmp88F;});_tmp3ED;});_tmp3EE->f2=_tmp890;});_tmp3EE;});Cyc_Absyn_new_vardecl(_tmp892,_tmp891,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct Cyc_Absyn_Decl*_tmp893=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EC->hd=_tmp893;}),_tmp3EC->tl=0;_tmp3EC;}));
# 1355
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1357
struct _fat_ptr _tmp3F1=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3F1;
if(({struct _fat_ptr _tmp894=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp894,({const char*_tmp3F2="H";_tag_fat(_tmp3F2,sizeof(char),2U);}));})== 0)
({void*_tmp3F3=0U;({unsigned _tmp896=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp895=({const char*_tmp3F4="bad occurrence of heap region `H";_tag_fat(_tmp3F4,sizeof(char),33U);});Cyc_Warn_err(_tmp896,_tmp895,_tag_fat(_tmp3F3,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp897=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp897,({const char*_tmp3F5="U";_tag_fat(_tmp3F5,sizeof(char),2U);}));})== 0)
({void*_tmp3F6=0U;({unsigned _tmp899=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp898=({const char*_tmp3F7="bad occurrence of unique region `U";_tag_fat(_tmp3F7,sizeof(char),35U);});Cyc_Warn_err(_tmp899,_tmp898,_tag_fat(_tmp3F6,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));({struct _fat_ptr*_tmp89D=({struct _fat_ptr*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct _fat_ptr _tmp89C=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FD=({struct Cyc_String_pa_PrintArg_struct _tmp720;_tmp720.tag=0U,_tmp720.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp720;});void*_tmp3FB[1U];_tmp3FB[0]=& _tmp3FD;({struct _fat_ptr _tmp89B=({const char*_tmp3FC="`%s";_tag_fat(_tmp3FC,sizeof(char),4U);});Cyc_aprintf(_tmp89B,_tag_fat(_tmp3FB,sizeof(void*),1U));});});*_tmp3FE=_tmp89C;});_tmp3FE;});_tmp3FF->name=_tmp89D;}),_tmp3FF->identity=- 1,({
void*_tmp89A=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3FF->kind=_tmp89A;});_tmp3FF;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A0=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp89F=({struct _tuple0*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp89E=({struct _fat_ptr*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));*_tmp3F9=two;_tmp3F9;});_tmp3FA->f2=_tmp89E;});_tmp3FA;});Cyc_Absyn_new_vardecl(_tmp8A0,_tmp89F,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));({struct Cyc_Absyn_Decl*_tmp8A1=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F8->hd=_tmp8A1;}),_tmp3F8->tl=0;_tmp3F8;}));
# 1368
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1370
struct _fat_ptr _tmp400=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp400;
struct _fat_ptr _tmp401=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp401;
struct Cyc_Absyn_Exp*_tmp402=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp402;
if(({struct _fat_ptr _tmp8A2=(struct _fat_ptr)four;Cyc_strcmp(_tmp8A2,({const char*_tmp403="open";_tag_fat(_tmp403,sizeof(char),5U);}));})!= 0)({void*_tmp404=0U;({unsigned _tmp8A4=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp8A3=({const char*_tmp405="expecting `open'";_tag_fat(_tmp405,sizeof(char),17U);});Cyc_Warn_err(_tmp8A4,_tmp8A3,_tag_fat(_tmp404,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp40D=_cycalloc(sizeof(*_tmp40D));({struct _fat_ptr*_tmp8A8=({struct _fat_ptr*_tmp40C=_cycalloc(sizeof(*_tmp40C));({struct _fat_ptr _tmp8A7=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmp721;_tmp721.tag=0U,_tmp721.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp721;});void*_tmp409[1U];_tmp409[0]=& _tmp40B;({struct _fat_ptr _tmp8A6=({const char*_tmp40A="`%s";_tag_fat(_tmp40A,sizeof(char),4U);});Cyc_aprintf(_tmp8A6,_tag_fat(_tmp409,sizeof(void*),1U));});});*_tmp40C=_tmp8A7;});_tmp40C;});_tmp40D->name=_tmp8A8;}),_tmp40D->identity=- 1,({
void*_tmp8A5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp40D->kind=_tmp8A5;});_tmp40D;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8AB=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8AA=({struct _tuple0*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A9=({struct _fat_ptr*_tmp407=_cycalloc(sizeof(*_tmp407));*_tmp407=two;_tmp407;});_tmp408->f2=_tmp8A9;});_tmp408;});Cyc_Absyn_new_vardecl(_tmp8AB,_tmp8AA,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp406=_cycalloc(sizeof(*_tmp406));({struct Cyc_Absyn_Decl*_tmp8AC=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp406->hd=_tmp8AC;}),_tmp406->tl=0;_tmp406;}));
# 1380
goto _LL0;}}case 54U: _LL67: _LL68: {
# 1383 "parse.y"
struct _fat_ptr _tmp40E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp40E;
Cyc_Parse_tvar_ok(three,(unsigned)((yyyvsp[2]).l).first_line);{
# 1386
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp411=_cycalloc(sizeof(*_tmp411));({struct _fat_ptr*_tmp8AE=({struct _fat_ptr*_tmp410=_cycalloc(sizeof(*_tmp410));*_tmp410=three;_tmp410;});_tmp411->name=_tmp8AE;}),_tmp411->identity=- 1,({void*_tmp8AD=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp411->kind=_tmp8AD;});_tmp411;});
void*t=Cyc_Absyn_var_type(tv);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp40F=_cycalloc(sizeof(*_tmp40F));({struct Cyc_Absyn_Decl*_tmp8AF=Cyc_Absyn_pool_decl(tv,(unsigned)((yyyvsp[0]).l).first_line);_tmp40F->hd=_tmp8AF;}),_tmp40F->tl=0;_tmp40F;}));
# 1390
goto _LL0;}}case 55U: _LL69: _LL6A:
# 1394 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 56U: _LL6B: _LL6C:
# 1396 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8B0=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8B0,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 57U: _LL6D: _LL6E:
# 1402 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp722;({enum Cyc_Parse_Storage_class _tmp8B3=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp722.sc=_tmp8B3;}),({struct Cyc_Absyn_Tqual _tmp8B2=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp722.tq=_tmp8B2;}),({
struct Cyc_Parse_Type_specifier _tmp8B1=Cyc_Parse_empty_spec(0U);_tmp722.type_specs=_tmp8B1;}),_tmp722.is_inline=0,_tmp722.attributes=0;_tmp722;}));
goto _LL0;case 58U: _LL6F: _LL70: {
# 1405 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp412=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp412;
if((int)two.sc != (int)Cyc_Parse_None_sc)
({void*_tmp413=0U;({unsigned _tmp8B5=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8B4=({const char*_tmp414="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp414,sizeof(char),73U);});Cyc_Warn_warn(_tmp8B5,_tmp8B4,_tag_fat(_tmp413,sizeof(void*),0U));});});
# 1409
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp723;({enum Cyc_Parse_Storage_class _tmp8B6=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp723.sc=_tmp8B6;}),_tmp723.tq=two.tq,_tmp723.type_specs=two.type_specs,_tmp723.is_inline=two.is_inline,_tmp723.attributes=two.attributes;_tmp723;}));
# 1413
goto _LL0;}case 59U: _LL71: _LL72:
# 1414 "parse.y"
({void*_tmp415=0U;({unsigned _tmp8B8=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8B7=({const char*_tmp416="__extension__ keyword ignored in declaration";_tag_fat(_tmp416,sizeof(char),45U);});Cyc_Warn_warn(_tmp8B8,_tmp8B7,_tag_fat(_tmp415,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1417
goto _LL0;case 60U: _LL73: _LL74:
# 1418 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp724;_tmp724.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8BA=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp724.tq=_tmp8BA;}),({
struct Cyc_Parse_Type_specifier _tmp8B9=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp724.type_specs=_tmp8B9;}),_tmp724.is_inline=0,_tmp724.attributes=0;_tmp724;}));
goto _LL0;case 61U: _LL75: _LL76: {
# 1421 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp417=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp417;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp725;_tmp725.sc=two.sc,_tmp725.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8BD=({unsigned _tmp8BC=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8BB=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8BC,_tmp8BB,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp725.type_specs=_tmp8BD;}),_tmp725.is_inline=two.is_inline,_tmp725.attributes=two.attributes;_tmp725;}));
# 1427
goto _LL0;}case 62U: _LL77: _LL78:
# 1428 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp726;_tmp726.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8BF=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp726.tq=_tmp8BF;}),({struct Cyc_Parse_Type_specifier _tmp8BE=Cyc_Parse_empty_spec(0U);_tmp726.type_specs=_tmp8BE;}),_tmp726.is_inline=0,_tmp726.attributes=0;_tmp726;}));
goto _LL0;case 63U: _LL79: _LL7A: {
# 1430 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp418=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp418;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp727;_tmp727.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8C1=({struct Cyc_Absyn_Tqual _tmp8C0=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8C0,two.tq);});_tmp727.tq=_tmp8C1;}),_tmp727.type_specs=two.type_specs,_tmp727.is_inline=two.is_inline,_tmp727.attributes=two.attributes;_tmp727;}));
# 1435
goto _LL0;}case 64U: _LL7B: _LL7C:
# 1436 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp728;_tmp728.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8C3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp728.tq=_tmp8C3;}),({
struct Cyc_Parse_Type_specifier _tmp8C2=Cyc_Parse_empty_spec(0U);_tmp728.type_specs=_tmp8C2;}),_tmp728.is_inline=1,_tmp728.attributes=0;_tmp728;}));
goto _LL0;case 65U: _LL7D: _LL7E: {
# 1439 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp419=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp419;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp729;_tmp729.sc=two.sc,_tmp729.tq=two.tq,_tmp729.type_specs=two.type_specs,_tmp729.is_inline=1,_tmp729.attributes=two.attributes;_tmp729;}));
# 1443
goto _LL0;}case 66U: _LL7F: _LL80:
# 1444 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72A;_tmp72A.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8C6=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72A.tq=_tmp8C6;}),({
struct Cyc_Parse_Type_specifier _tmp8C5=Cyc_Parse_empty_spec(0U);_tmp72A.type_specs=_tmp8C5;}),_tmp72A.is_inline=0,({struct Cyc_List_List*_tmp8C4=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp72A.attributes=_tmp8C4;});_tmp72A;}));
goto _LL0;case 67U: _LL81: _LL82: {
# 1447 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41A=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41A;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72B;_tmp72B.sc=two.sc,_tmp72B.tq=two.tq,_tmp72B.type_specs=two.type_specs,_tmp72B.is_inline=two.is_inline,({
# 1450
struct Cyc_List_List*_tmp8C8=({struct Cyc_List_List*_tmp8C7=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8C7,two.attributes);});_tmp72B.attributes=_tmp8C8;});_tmp72B;}));
goto _LL0;}case 68U: _LL83: _LL84:
# 1454 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Auto_sc);
goto _LL0;case 69U: _LL85: _LL86:
# 1455 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Register_sc);
goto _LL0;case 70U: _LL87: _LL88:
# 1456 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Static_sc);
goto _LL0;case 71U: _LL89: _LL8A:
# 1457 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Extern_sc);
goto _LL0;case 72U: _LL8B: _LL8C:
# 1459 "parse.y"
 if(({struct _fat_ptr _tmp8C9=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8C9,({const char*_tmp41B="C";_tag_fat(_tmp41B,sizeof(char),2U);}));})!= 0)
({void*_tmp41C=0U;({unsigned _tmp8CB=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8CA=({const char*_tmp41D="only extern or extern \"C\" is allowed";_tag_fat(_tmp41D,sizeof(char),37U);});Cyc_Warn_err(_tmp8CB,_tmp8CA,_tag_fat(_tmp41C,sizeof(void*),0U));});});
yyval=Cyc_YY20(Cyc_Parse_ExternC_sc);
# 1463
goto _LL0;case 73U: _LL8D: _LL8E:
# 1463 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Typedef_sc);
goto _LL0;case 74U: _LL8F: _LL90:
# 1465 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Abstract_sc);
goto _LL0;case 75U: _LL91: _LL92:
# 1470 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 76U: _LL93: _LL94:
# 1471 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1476 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 78U: _LL97: _LL98:
# 1480 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp41E=_cycalloc(sizeof(*_tmp41E));({void*_tmp8CC=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp41E->hd=_tmp8CC;}),_tmp41E->tl=0;_tmp41E;}));
goto _LL0;case 79U: _LL99: _LL9A:
# 1481 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp41F=_cycalloc(sizeof(*_tmp41F));({void*_tmp8CE=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp41F->hd=_tmp8CE;}),({struct Cyc_List_List*_tmp8CD=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp41F->tl=_tmp8CD;});_tmp41F;}));
goto _LL0;case 80U: _LL9B: _LL9C: {
# 1486 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[19U]={{{_tmp423,_tmp423,_tmp423 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp424,_tmp424,_tmp424 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp425,_tmp425,_tmp425 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp426,_tmp426,_tmp426 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp427,_tmp427,_tmp427 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp428,_tmp428,_tmp428 + 8U},(void*)& att_aligned},{{_tmp429,_tmp429,_tmp429 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp42A,_tmp42A,_tmp42A + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp42B,_tmp42B,_tmp42B + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp42C,_tmp42C,_tmp42C + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp42D,_tmp42D,_tmp42D + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp42E,_tmp42E,_tmp42E + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp42F,_tmp42F,_tmp42F + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp430,_tmp430,_tmp430 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp431,_tmp431,_tmp431 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp432,_tmp432,_tmp432 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp433,_tmp433,_tmp433 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp434,_tmp434,_tmp434 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val},{{_tmp435,_tmp435,_tmp435 + 9U},(void*)& Cyc_Absyn_No_throw_att_val}};
# 1508
struct _fat_ptr _tmp420=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp420;
# 1510
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1512
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 19U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,19U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1519
if((unsigned)i == 19U){
({void*_tmp421=0U;({unsigned _tmp8D0=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8CF=({const char*_tmp422="unrecognized attribute";_tag_fat(_tmp422,sizeof(char),23U);});Cyc_Warn_err(_tmp8D0,_tmp8CF,_tag_fat(_tmp421,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1524
goto _LL0;}}case 81U: _LL9D: _LL9E:
# 1524 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 82U: _LL9F: _LLA0: {
# 1526 "parse.y"
struct _fat_ptr _tmp436=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp436;
struct Cyc_Absyn_Exp*_tmp437=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp437;
void*a;
if(({struct _fat_ptr _tmp8D2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D2,({const char*_tmp438="aligned";_tag_fat(_tmp438,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D1,({const char*_tmp439="__aligned__";_tag_fat(_tmp439,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=6U,_tmp43A->f1=e;_tmp43A;});else{
if(({struct _fat_ptr _tmp8D4=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D4,({const char*_tmp43B="section";_tag_fat(_tmp43B,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D3,({const char*_tmp43C="__section__";_tag_fat(_tmp43C,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->tag=8U,({struct _fat_ptr _tmp8D5=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp43D->f1=_tmp8D5;});_tmp43D;});else{
if(({struct _fat_ptr _tmp8D6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D6,({const char*_tmp43E="__mode__";_tag_fat(_tmp43E,sizeof(char),9U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=24U,({struct _fat_ptr _tmp8D7=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp43F->f1=_tmp8D7;});_tmp43F;});else{
if(({struct _fat_ptr _tmp8D8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D8,({const char*_tmp440="alias";_tag_fat(_tmp440,sizeof(char),6U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->tag=25U,({struct _fat_ptr _tmp8D9=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp441->f1=_tmp8D9;});_tmp441;});else{
# 1538
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8DB=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DB,({const char*_tmp442="regparm";_tag_fat(_tmp442,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8DA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DA,({const char*_tmp443="__regparm__";_tag_fat(_tmp443,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp444=0U;({unsigned _tmp8DD=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8DC=({const char*_tmp445="regparm requires value between 0 and 3";_tag_fat(_tmp445,sizeof(char),39U);});Cyc_Warn_err(_tmp8DD,_tmp8DC,_tag_fat(_tmp444,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->tag=0U,_tmp446->f1=n;_tmp446;});}else{
if(({struct _fat_ptr _tmp8DF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DF,({const char*_tmp447="initializes";_tag_fat(_tmp447,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8DE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DE,({const char*_tmp448="__initializes__";_tag_fat(_tmp448,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->tag=20U,_tmp449->f1=n;_tmp449;});else{
if(({struct _fat_ptr _tmp8E1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E1,({const char*_tmp44A="noliveunique";_tag_fat(_tmp44A,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8E0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E0,({const char*_tmp44B="__noliveunique__";_tag_fat(_tmp44B,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->tag=21U,_tmp44C->f1=n;_tmp44C;});else{
if(({struct _fat_ptr _tmp8E3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E3,({const char*_tmp44D="consume";_tag_fat(_tmp44D,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8E2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E2,({const char*_tmp44E="__consume__";_tag_fat(_tmp44E,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->tag=22U,_tmp44F->f1=n;_tmp44F;});else{
# 1550
({void*_tmp450=0U;({unsigned _tmp8E5=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8E4=({const char*_tmp451="unrecognized attribute";_tag_fat(_tmp451,sizeof(char),23U);});Cyc_Warn_err(_tmp8E5,_tmp8E4,_tag_fat(_tmp450,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1554
yyval=Cyc_YY46(a);
# 1556
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1557 "parse.y"
struct _fat_ptr _tmp452=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp452;
struct _fat_ptr _tmp453=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp453;
unsigned _tmp454=({unsigned _tmp8E6=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8E6,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp454;
unsigned _tmp455=({unsigned _tmp8E7=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8E7,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp455;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8E9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E9,({const char*_tmp456="format";_tag_fat(_tmp456,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8E8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E8,({const char*_tmp457="__format__";_tag_fat(_tmp457,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8EB=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8EB,({const char*_tmp458="printf";_tag_fat(_tmp458,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8EA=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8EA,({const char*_tmp459="__printf__";_tag_fat(_tmp459,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=19U,_tmp45A->f1=Cyc_Absyn_Printf_ft,_tmp45A->f2=(int)n,_tmp45A->f3=(int)m;_tmp45A;});else{
if(({struct _fat_ptr _tmp8ED=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8ED,({const char*_tmp45B="scanf";_tag_fat(_tmp45B,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8EC=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8EC,({const char*_tmp45C="__scanf__";_tag_fat(_tmp45C,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->tag=19U,_tmp45D->f1=Cyc_Absyn_Scanf_ft,_tmp45D->f2=(int)n,_tmp45D->f3=(int)m;_tmp45D;});else{
# 1568
({void*_tmp45E=0U;({unsigned _tmp8EF=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8EE=({const char*_tmp45F="unrecognized format type";_tag_fat(_tmp45F,sizeof(char),25U);});Cyc_Warn_err(_tmp8EF,_tmp8EE,_tag_fat(_tmp45E,sizeof(void*),0U));});});}}}else{
# 1570
({void*_tmp460=0U;({unsigned _tmp8F1=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8F0=({const char*_tmp461="unrecognized attribute";_tag_fat(_tmp461,sizeof(char),23U);});Cyc_Warn_err(_tmp8F1,_tmp8F0,_tag_fat(_tmp460,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1573
goto _LL0;}case 84U: _LLA3: _LLA4:
# 1582 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 85U: _LLA5: _LLA6:
# 1584 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F3=({struct _tuple0*_tmp8F2=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp8F2,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp8F3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1588 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1589 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1590 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1591 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1592 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1593 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1594 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1595 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1596 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 95U: _LLB9: _LLBA:
# 1597 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1598 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 97U: _LLBD: _LLBE:
# 1601
 yyval=Cyc_YY21(({void*_tmp8F4=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1603 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F5=Cyc_Absyn_builtin_type(({const char*_tmp462="__builtin_va_list";_tag_fat(_tmp462,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp8F5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 99U: _LLC1: _LLC2:
# 1605 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 100U: _LLC3: _LLC4:
# 1607 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F6=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp8F6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1609 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F7=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8F7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1611 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F8=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp8F8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 103U: _LLC9: _LLCA:
# 1613 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FB=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->tag=6U,({struct Cyc_List_List*_tmp8FA=({unsigned _tmp8F9=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8F9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});_tmp463->f1=_tmp8FA;});_tmp463;});Cyc_Parse_type_spec(_tmp8FB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1615
goto _LL0;case 104U: _LLCB: _LLCC:
# 1616 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FC=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8FC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 105U: _LLCD: _LLCE:
# 1618 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FD=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp8FD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1620
goto _LL0;case 106U: _LLCF: _LLD0:
# 1621 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FE=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8FE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1623 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FF=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp8FF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1625 "parse.y"
 yyval=Cyc_YY21(({void*_tmp900=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp900,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6:
# 1630 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp901=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp901,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 110U: _LLD7: _LLD8: {
# 1634 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp72C;_tmp72C.print_const=1,_tmp72C.q_volatile=0,_tmp72C.q_restrict=0,_tmp72C.real_const=1,_tmp72C.loc=loc;_tmp72C;}));
goto _LL0;}case 111U: _LLD9: _LLDA:
# 1636 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp72D;_tmp72D.print_const=0,_tmp72D.q_volatile=1,_tmp72D.q_restrict=0,_tmp72D.real_const=0,_tmp72D.loc=0U;_tmp72D;}));
goto _LL0;case 112U: _LLDB: _LLDC:
# 1637 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp72E;_tmp72E.print_const=0,_tmp72E.q_volatile=0,_tmp72E.q_restrict=1,_tmp72E.real_const=0,_tmp72E.loc=0U;_tmp72E;}));
goto _LL0;case 113U: _LLDD: _LLDE: {
# 1643 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp464=({struct Cyc_Absyn_TypeDecl*_tmp469=_cycalloc(sizeof(*_tmp469));({void*_tmp906=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp905=({struct Cyc_Absyn_Enumdecl*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp904=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp467->name=_tmp904;}),({struct Cyc_Core_Opt*_tmp903=({struct Cyc_Core_Opt*_tmp466=_cycalloc(sizeof(*_tmp466));({struct Cyc_List_List*_tmp902=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp466->v=_tmp902;});_tmp466;});_tmp467->fields=_tmp903;});_tmp467;});_tmp468->f1=_tmp905;});_tmp468;});_tmp469->r=_tmp906;}),_tmp469->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp469;});struct Cyc_Absyn_TypeDecl*ed=_tmp464;
# 1645
yyval=Cyc_YY21(({void*_tmp907=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->tag=10U,_tmp465->f1=ed,_tmp465->f2=0;_tmp465;});Cyc_Parse_type_spec(_tmp907,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1647
goto _LL0;}case 114U: _LLDF: _LLE0:
# 1648 "parse.y"
 yyval=Cyc_YY21(({void*_tmp908=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp908,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1650 "parse.y"
 yyval=Cyc_YY21(({void*_tmp909=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp909,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1656 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46A=_cycalloc(sizeof(*_tmp46A));({struct _tuple0*_tmp90A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46A->name=_tmp90A;}),_tmp46A->tag=0,_tmp46A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46A;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1658 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct _tuple0*_tmp90C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46B->name=_tmp90C;}),({struct Cyc_Absyn_Exp*_tmp90B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp46B->tag=_tmp90B;}),_tmp46B->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46B;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1662 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46C=_cycalloc(sizeof(*_tmp46C));({struct Cyc_Absyn_Enumfield*_tmp90D=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46C->hd=_tmp90D;}),_tmp46C->tl=0;_tmp46C;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1663 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46D=_cycalloc(sizeof(*_tmp46D));({struct Cyc_Absyn_Enumfield*_tmp90E=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46D->hd=_tmp90E;}),_tmp46D->tl=0;_tmp46D;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1664 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct Cyc_Absyn_Enumfield*_tmp910=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46E->hd=_tmp910;}),({struct Cyc_List_List*_tmp90F=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp46E->tl=_tmp90F;});_tmp46E;}));
goto _LL0;case 121U: _LLED: _LLEE:
# 1670 "parse.y"
 yyval=Cyc_YY21(({void*_tmp913=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->tag=7U,({enum Cyc_Absyn_AggrKind _tmp912=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp46F->f1=_tmp912;}),({struct Cyc_List_List*_tmp911=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp46F->f2=_tmp911;});_tmp46F;});Cyc_Parse_type_spec(_tmp913,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 122U: _LLEF: _LLF0: {
# 1676 "parse.y"
struct Cyc_List_List*_tmp470=({unsigned _tmp914=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp914,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp470;
struct Cyc_List_List*_tmp471=({unsigned _tmp915=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp915,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp471;
struct Cyc_Absyn_TypeDecl*_tmp472=({enum Cyc_Absyn_AggrKind _tmp91B=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp91A=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp919=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp918=({
struct Cyc_List_List*_tmp917=exist_ts;struct Cyc_List_List*_tmp916=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp917,_tmp916,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1678
Cyc_Absyn_aggr_tdecl(_tmp91B,Cyc_Absyn_Public,_tmp91A,_tmp919,_tmp918,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp472;
# 1681
yyval=Cyc_YY21(({void*_tmp91C=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->tag=10U,_tmp473->f1=td,_tmp473->f2=0;_tmp473;});Cyc_Parse_type_spec(_tmp91C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1683
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1687 "parse.y"
struct Cyc_List_List*_tmp474=({unsigned _tmp91D=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp91D,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp474;
struct Cyc_List_List*_tmp475=({unsigned _tmp91E=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp91E,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp475;
struct Cyc_Absyn_TypeDecl*_tmp476=({enum Cyc_Absyn_AggrKind _tmp924=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp923=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp922=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp921=({
struct Cyc_List_List*_tmp920=exist_ts;struct Cyc_List_List*_tmp91F=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp920,_tmp91F,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1689
Cyc_Absyn_aggr_tdecl(_tmp924,Cyc_Absyn_Public,_tmp923,_tmp922,_tmp921,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp476;
# 1692
yyval=Cyc_YY21(({void*_tmp925=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=10U,_tmp477->f1=td,_tmp477->f2=0;_tmp477;});Cyc_Parse_type_spec(_tmp925,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1694
goto _LL0;}case 124U: _LLF3: _LLF4:
# 1695 "parse.y"
 yyval=Cyc_YY21(({void*_tmp929=({union Cyc_Absyn_AggrInfo _tmp928=({enum Cyc_Absyn_AggrKind _tmp927=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp926=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp927,_tmp926,({struct Cyc_Core_Opt*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->v=(void*)1;_tmp478;}));});Cyc_Absyn_aggr_type(_tmp928,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp929,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1698
goto _LL0;case 125U: _LLF5: _LLF6:
# 1699 "parse.y"
 yyval=Cyc_YY21(({void*_tmp92C=({union Cyc_Absyn_AggrInfo _tmp92B=({enum Cyc_Absyn_AggrKind _tmp92A=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp92A,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp92B,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp92C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 126U: _LLF7: _LLF8:
# 1704 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 127U: _LLF9: _LLFA:
# 1706 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 128U: _LLFB: _LLFC:
# 1710 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1711 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 130U: _LLFF: _LL100:
# 1716 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 131U: _LL101: _LL102: {
# 1718 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp479=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp479;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1722
struct Cyc_List_List*_tmp47A=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp47A;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1727
goto _LL0;}}case 132U: _LL103: _LL104:
# 1732 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47B=_cycalloc(sizeof(*_tmp47B));({struct Cyc_List_List*_tmp92D=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp47B->hd=_tmp92D;}),_tmp47B->tl=0;_tmp47B;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1734 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct Cyc_List_List*_tmp92F=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp47C->hd=_tmp92F;}),({struct Cyc_List_List*_tmp92E=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp47C->tl=_tmp92E;});_tmp47C;}));
goto _LL0;case 134U: _LL107: _LL108:
# 1738 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 135U: _LL109: _LL10A:
# 1744 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp47D=_region_malloc(yyr,sizeof(*_tmp47D));_tmp47D->tl=0,({struct _tuple12 _tmp930=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp47D->hd=_tmp930;});_tmp47D;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1746 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp47E=_region_malloc(yyr,sizeof(*_tmp47E));({struct _tuple13*_tmp932=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp47E->tl=_tmp932;}),({struct _tuple12 _tmp931=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp47E->hd=_tmp931;});_tmp47E;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1751 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp72F;({struct Cyc_Parse_Declarator _tmp933=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp72F.f1=_tmp933;}),_tmp72F.f2=0;_tmp72F;}));
goto _LL0;case 138U: _LL10F: _LL110:
# 1753 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp730;({struct Cyc_Parse_Declarator _tmp935=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp730.f1=_tmp935;}),({struct Cyc_Absyn_Exp*_tmp934=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp730.f2=_tmp934;});_tmp730;}));
goto _LL0;case 139U: _LL111: _LL112: {
# 1759 "parse.y"
struct _RegionHandle _tmp47F=_new_region("temp");struct _RegionHandle*temp=& _tmp47F;_push_region(temp);
{struct _tuple26 _tmp480=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp16=_tmp480;struct _tuple26 _tmp481=_stmttmp16;struct Cyc_List_List*_tmp484;struct Cyc_Parse_Type_specifier _tmp483;struct Cyc_Absyn_Tqual _tmp482;_LL467: _tmp482=_tmp481.f1;_tmp483=_tmp481.f2;_tmp484=_tmp481.f3;_LL468: {struct Cyc_Absyn_Tqual tq=_tmp482;struct Cyc_Parse_Type_specifier tspecs=_tmp483;struct Cyc_List_List*atts=_tmp484;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp485=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp485;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp486=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp17=_tmp486;struct _tuple25*_tmp487=_stmttmp17;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Parse_Declarator _tmp488;_LL46A: _tmp488=_tmp487->f1;_tmp489=_tmp487->f2;_tmp48A=_tmp487->f3;_LL46B: {struct Cyc_Parse_Declarator d=_tmp488;struct Cyc_Absyn_Exp*wd=_tmp489;struct Cyc_Absyn_Exp*wh=_tmp48A;
decls=({struct _tuple11*_tmp48B=_region_malloc(temp,sizeof(*_tmp48B));_tmp48B->tl=decls,_tmp48B->hd=d;_tmp48B;});
widths_and_reqs=({struct Cyc_List_List*_tmp48D=_region_malloc(temp,sizeof(*_tmp48D));
({struct _tuple17*_tmp936=({struct _tuple17*_tmp48C=_region_malloc(temp,sizeof(*_tmp48C));_tmp48C->f1=wd,_tmp48C->f2=wh;_tmp48C;});_tmp48D->hd=_tmp936;}),_tmp48D->tl=widths_and_reqs;_tmp48D;});}}}
# 1770
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp48E=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp48E;
struct Cyc_List_List*_tmp48F=({struct _RegionHandle*_tmp939=temp;struct _RegionHandle*_tmp938=temp;struct Cyc_List_List*_tmp937=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1773
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp939,_tmp938,_tmp937,widths_and_reqs);});struct Cyc_List_List*info=_tmp48F;
# 1776
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1778
_npop_handler(0U);goto _LL0;}}}}
# 1760
;_pop_region();}case 140U: _LL113: _LL114:
# 1786 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp731;({struct Cyc_Absyn_Tqual _tmp93B=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp731.f1=_tmp93B;}),({struct Cyc_Parse_Type_specifier _tmp93A=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp731.f2=_tmp93A;}),_tmp731.f3=0;_tmp731;}));
goto _LL0;case 141U: _LL115: _LL116: {
# 1788 "parse.y"
struct _tuple26 _tmp490=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp490;yyval=Cyc_YY35(({struct _tuple26 _tmp732;_tmp732.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp93E=({unsigned _tmp93D=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp93C=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp93D,_tmp93C,two.f2);});_tmp732.f2=_tmp93E;}),_tmp732.f3=two.f3;_tmp732;}));
goto _LL0;}case 142U: _LL117: _LL118:
# 1790 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp733;({struct Cyc_Absyn_Tqual _tmp940=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp733.f1=_tmp940;}),({struct Cyc_Parse_Type_specifier _tmp93F=Cyc_Parse_empty_spec(0U);_tmp733.f2=_tmp93F;}),_tmp733.f3=0;_tmp733;}));
goto _LL0;case 143U: _LL119: _LL11A: {
# 1792 "parse.y"
struct _tuple26 _tmp491=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp491;
yyval=Cyc_YY35(({struct _tuple26 _tmp734;({struct Cyc_Absyn_Tqual _tmp942=({struct Cyc_Absyn_Tqual _tmp941=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp941,two.f1);});_tmp734.f1=_tmp942;}),_tmp734.f2=two.f2,_tmp734.f3=two.f3;_tmp734;}));
goto _LL0;}case 144U: _LL11B: _LL11C:
# 1795 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp735;({struct Cyc_Absyn_Tqual _tmp945=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp735.f1=_tmp945;}),({struct Cyc_Parse_Type_specifier _tmp944=Cyc_Parse_empty_spec(0U);_tmp735.f2=_tmp944;}),({struct Cyc_List_List*_tmp943=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp735.f3=_tmp943;});_tmp735;}));
goto _LL0;case 145U: _LL11D: _LL11E: {
# 1797 "parse.y"
struct _tuple26 _tmp492=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp492;yyval=Cyc_YY35(({struct _tuple26 _tmp736;_tmp736.f1=two.f1,_tmp736.f2=two.f2,({struct Cyc_List_List*_tmp947=({struct Cyc_List_List*_tmp946=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp946,two.f3);});_tmp736.f3=_tmp947;});_tmp736;}));
goto _LL0;}case 146U: _LL11F: _LL120:
# 1803 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp737;({struct Cyc_Absyn_Tqual _tmp949=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp737.f1=_tmp949;}),({struct Cyc_Parse_Type_specifier _tmp948=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp737.f2=_tmp948;}),_tmp737.f3=0;_tmp737;}));
goto _LL0;case 147U: _LL121: _LL122: {
# 1805 "parse.y"
struct _tuple26 _tmp493=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp493;yyval=Cyc_YY35(({struct _tuple26 _tmp738;_tmp738.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp94C=({unsigned _tmp94B=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp94A=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp94B,_tmp94A,two.f2);});_tmp738.f2=_tmp94C;}),_tmp738.f3=two.f3;_tmp738;}));
goto _LL0;}case 148U: _LL123: _LL124:
# 1807 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp739;({struct Cyc_Absyn_Tqual _tmp94E=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp739.f1=_tmp94E;}),({struct Cyc_Parse_Type_specifier _tmp94D=Cyc_Parse_empty_spec(0U);_tmp739.f2=_tmp94D;}),_tmp739.f3=0;_tmp739;}));
goto _LL0;case 149U: _LL125: _LL126: {
# 1809 "parse.y"
struct _tuple26 _tmp494=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp494;
yyval=Cyc_YY35(({struct _tuple26 _tmp73A;({struct Cyc_Absyn_Tqual _tmp950=({struct Cyc_Absyn_Tqual _tmp94F=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp94F,two.f1);});_tmp73A.f1=_tmp950;}),_tmp73A.f2=two.f2,_tmp73A.f3=two.f3;_tmp73A;}));
goto _LL0;}case 150U: _LL127: _LL128:
# 1812 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73B;({struct Cyc_Absyn_Tqual _tmp953=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73B.f1=_tmp953;}),({struct Cyc_Parse_Type_specifier _tmp952=Cyc_Parse_empty_spec(0U);_tmp73B.f2=_tmp952;}),({struct Cyc_List_List*_tmp951=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp73B.f3=_tmp951;});_tmp73B;}));
goto _LL0;case 151U: _LL129: _LL12A: {
# 1814 "parse.y"
struct _tuple26 _tmp495=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp495;yyval=Cyc_YY35(({struct _tuple26 _tmp73C;_tmp73C.f1=two.f1,_tmp73C.f2=two.f2,({struct Cyc_List_List*_tmp955=({struct Cyc_List_List*_tmp954=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp954,two.f3);});_tmp73C.f3=_tmp955;});_tmp73C;}));
goto _LL0;}case 152U: _LL12B: _LL12C:
# 1818 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1824 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp496=_region_malloc(yyr,sizeof(*_tmp496));({struct _tuple25*_tmp956=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp496->hd=_tmp956;}),_tmp496->tl=0;_tmp496;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1826 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp497=_region_malloc(yyr,sizeof(*_tmp497));({struct _tuple25*_tmp958=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp497->hd=_tmp958;}),({struct Cyc_List_List*_tmp957=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp497->tl=_tmp957;});_tmp497;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1831 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));({struct Cyc_Parse_Declarator _tmp95A=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp498->f1=_tmp95A;}),_tmp498->f2=0,({struct Cyc_Absyn_Exp*_tmp959=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp498->f3=_tmp959;});_tmp498;}));
goto _LL0;case 156U: _LL133: _LL134:
# 1835 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49C=_region_malloc(yyr,sizeof(*_tmp49C));({struct _tuple0*_tmp95F=({struct _tuple0*_tmp49B=_cycalloc(sizeof(*_tmp49B));({union Cyc_Absyn_Nmspace _tmp95E=Cyc_Absyn_Rel_n(0);_tmp49B->f1=_tmp95E;}),({struct _fat_ptr*_tmp95D=({struct _fat_ptr*_tmp49A=_cycalloc(sizeof(*_tmp49A));({struct _fat_ptr _tmp95C=({const char*_tmp499="";_tag_fat(_tmp499,sizeof(char),1U);});*_tmp49A=_tmp95C;});_tmp49A;});_tmp49B->f2=_tmp95D;});_tmp49B;});(_tmp49C->f1).id=_tmp95F;}),(_tmp49C->f1).varloc=0U,(_tmp49C->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp95B=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp49C->f2=_tmp95B;}),_tmp49C->f3=0;_tmp49C;}));
# 1837
goto _LL0;case 157U: _LL135: _LL136:
# 1840 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A0=_region_malloc(yyr,sizeof(*_tmp4A0));({struct _tuple0*_tmp963=({struct _tuple0*_tmp49F=_cycalloc(sizeof(*_tmp49F));({union Cyc_Absyn_Nmspace _tmp962=Cyc_Absyn_Rel_n(0);_tmp49F->f1=_tmp962;}),({struct _fat_ptr*_tmp961=({struct _fat_ptr*_tmp49E=_cycalloc(sizeof(*_tmp49E));({struct _fat_ptr _tmp960=({const char*_tmp49D="";_tag_fat(_tmp49D,sizeof(char),1U);});*_tmp49E=_tmp960;});_tmp49E;});_tmp49F->f2=_tmp961;});_tmp49F;});(_tmp4A0->f1).id=_tmp963;}),(_tmp4A0->f1).varloc=0U,(_tmp4A0->f1).tms=0,_tmp4A0->f2=0,_tmp4A0->f3=0;_tmp4A0;}));
# 1842
goto _LL0;case 158U: _LL137: _LL138:
# 1843 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A1=_region_malloc(yyr,sizeof(*_tmp4A1));({struct Cyc_Parse_Declarator _tmp965=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp4A1->f1=_tmp965;}),({struct Cyc_Absyn_Exp*_tmp964=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4A1->f2=_tmp964;}),_tmp4A1->f3=0;_tmp4A1;}));
goto _LL0;case 159U: _LL139: _LL13A:
# 1847 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 160U: _LL13B: _LL13C:
# 1848 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 161U: _LL13D: _LL13E:
# 1852 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 162U: _LL13F: _LL140:
# 1853 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 163U: _LL141: _LL142: {
# 1859 "parse.y"
int _tmp4A2=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A2;
struct Cyc_List_List*_tmp4A3=({unsigned _tmp966=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp966,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp4A3;
struct Cyc_Absyn_TypeDecl*_tmp4A4=({struct _tuple0*_tmp96B=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp96A=ts;struct Cyc_Core_Opt*_tmp969=({struct Cyc_Core_Opt*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));({struct Cyc_List_List*_tmp967=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp4A6->v=_tmp967;});_tmp4A6;});int _tmp968=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp96B,_tmp96A,_tmp969,_tmp968,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp4A4;
# 1863
yyval=Cyc_YY21(({void*_tmp96C=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->tag=10U,_tmp4A5->f1=dd,_tmp4A5->f2=0;_tmp4A5;});Cyc_Parse_type_spec(_tmp96C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1865
goto _LL0;}case 164U: _LL143: _LL144: {
# 1866 "parse.y"
int _tmp4A7=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A7;
yyval=Cyc_YY21(({void*_tmp96F=({union Cyc_Absyn_DatatypeInfo _tmp96E=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp73D;({struct _tuple0*_tmp96D=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp73D.name=_tmp96D;}),_tmp73D.is_extensible=is_extensible;_tmp73D;}));Cyc_Absyn_datatype_type(_tmp96E,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp96F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1869
goto _LL0;}case 165U: _LL145: _LL146: {
# 1870 "parse.y"
int _tmp4A8=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A8;
yyval=Cyc_YY21(({void*_tmp973=({union Cyc_Absyn_DatatypeFieldInfo _tmp972=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp73E;({struct _tuple0*_tmp971=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp73E.datatype_name=_tmp971;}),({struct _tuple0*_tmp970=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp73E.field_name=_tmp970;}),_tmp73E.is_extensible=is_extensible;_tmp73E;}));Cyc_Absyn_datatype_field_type(_tmp972,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp973,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1873
goto _LL0;}case 166U: _LL147: _LL148:
# 1876 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 167U: _LL149: _LL14A:
# 1877 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 168U: _LL14B: _LL14C:
# 1881 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));({struct Cyc_Absyn_Datatypefield*_tmp974=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A9->hd=_tmp974;}),_tmp4A9->tl=0;_tmp4A9;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1882 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({struct Cyc_Absyn_Datatypefield*_tmp975=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AA->hd=_tmp975;}),_tmp4AA->tl=0;_tmp4AA;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1883 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct Cyc_Absyn_Datatypefield*_tmp977=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AB->hd=_tmp977;}),({struct Cyc_List_List*_tmp976=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AB->tl=_tmp976;});_tmp4AB;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1884 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));({struct Cyc_Absyn_Datatypefield*_tmp979=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AC->hd=_tmp979;}),({struct Cyc_List_List*_tmp978=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AC->tl=_tmp978;});_tmp4AC;}));
goto _LL0;case 172U: _LL153: _LL154:
# 1888 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 173U: _LL155: _LL156:
# 1889 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 174U: _LL157: _LL158:
# 1890 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 175U: _LL159: _LL15A:
# 1894 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct _tuple0*_tmp97B=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4AD->name=_tmp97B;}),_tmp4AD->typs=0,_tmp4AD->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp97A=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4AD->sc=_tmp97A;});_tmp4AD;}));
goto _LL0;case 176U: _LL15B: _LL15C: {
# 1896 "parse.y"
struct Cyc_List_List*_tmp4AE=({unsigned _tmp97C=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp97C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4AE;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({struct _tuple0*_tmp97E=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4AF->name=_tmp97E;}),_tmp4AF->typs=typs,_tmp4AF->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp97D=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4AF->sc=_tmp97D;});_tmp4AF;}));
goto _LL0;}case 177U: _LL15D: _LL15E:
# 1902 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 178U: _LL15F: _LL160: {
# 1904 "parse.y"
struct Cyc_Parse_Declarator _tmp4B0=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B0;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73F;_tmp73F.id=two.id,_tmp73F.varloc=two.varloc,({struct Cyc_List_List*_tmp980=({struct Cyc_List_List*_tmp97F=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp97F,two.tms);});_tmp73F.tms=_tmp980;});_tmp73F;}));
goto _LL0;}case 179U: _LL161: _LL162:
# 1911 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 180U: _LL163: _LL164: {
# 1913 "parse.y"
struct Cyc_Parse_Declarator _tmp4B1=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp740;_tmp740.id=two.id,_tmp740.varloc=two.varloc,({struct Cyc_List_List*_tmp982=({struct Cyc_List_List*_tmp981=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp981,two.tms);});_tmp740.tms=_tmp982;});_tmp740;}));
goto _LL0;}case 181U: _LL165: _LL166:
# 1919 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp741;({struct _tuple0*_tmp983=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp741.id=_tmp983;}),_tmp741.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp741.tms=0;_tmp741;}));
goto _LL0;case 182U: _LL167: _LL168:
# 1921 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 183U: _LL169: _LL16A: {
# 1925 "parse.y"
struct Cyc_Parse_Declarator _tmp4B2=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4B2;
({struct Cyc_List_List*_tmp986=({struct Cyc_List_List*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));({void*_tmp985=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B3=_region_malloc(yyr,sizeof(*_tmp4B3));_tmp4B3->tag=5U,_tmp4B3->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp984=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4B3->f2=_tmp984;});_tmp4B3;});_tmp4B4->hd=_tmp985;}),_tmp4B4->tl=d.tms;_tmp4B4;});d.tms=_tmp986;});
yyval=(yyyvsp[2]).v;
# 1929
goto _LL0;}case 184U: _LL16B: _LL16C:
# 1930 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp742;({struct _tuple0*_tmp98C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp742.id=_tmp98C;}),({unsigned _tmp98B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp742.varloc=_tmp98B;}),({struct Cyc_List_List*_tmp98A=({struct Cyc_List_List*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));({void*_tmp989=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));_tmp4B5->tag=0U,({void*_tmp988=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4B5->f1=_tmp988;}),_tmp4B5->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4B5;});_tmp4B6->hd=_tmp989;}),({struct Cyc_List_List*_tmp987=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B6->tl=_tmp987;});_tmp4B6;});_tmp742.tms=_tmp98A;});_tmp742;}));
goto _LL0;case 185U: _LL16D: _LL16E:
# 1932 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp743;({struct _tuple0*_tmp993=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp743.id=_tmp993;}),({unsigned _tmp992=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp743.varloc=_tmp992;}),({
struct Cyc_List_List*_tmp991=({struct Cyc_List_List*_tmp4B8=_region_malloc(yyr,sizeof(*_tmp4B8));({void*_tmp990=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));_tmp4B7->tag=1U,({struct Cyc_Absyn_Exp*_tmp98F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4B7->f1=_tmp98F;}),({void*_tmp98E=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B7->f2=_tmp98E;}),_tmp4B7->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4B7;});_tmp4B8->hd=_tmp990;}),({struct Cyc_List_List*_tmp98D=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B8->tl=_tmp98D;});_tmp4B8;});_tmp743.tms=_tmp991;});_tmp743;}));
goto _LL0;case 186U: _LL16F: _LL170: {
# 1935 "parse.y"
struct _tuple27*_tmp4B9=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp18=_tmp4B9;struct _tuple27*_tmp4BA=_stmttmp18;struct Cyc_List_List*_tmp4BF;void*_tmp4BE;struct Cyc_Absyn_VarargInfo*_tmp4BD;int _tmp4BC;struct Cyc_List_List*_tmp4BB;_LL46D: _tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;_tmp4BD=_tmp4BA->f3;_tmp4BE=_tmp4BA->f4;_tmp4BF=_tmp4BA->f5;_LL46E: {struct Cyc_List_List*lis=_tmp4BB;int b=_tmp4BC;struct Cyc_Absyn_VarargInfo*c=_tmp4BD;void*eff=_tmp4BE;struct Cyc_List_List*po=_tmp4BF;
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4C0;
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4C1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp744;({struct _tuple0*_tmp999=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp744.id=_tmp999;}),({unsigned _tmp998=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp744.varloc=_tmp998;}),({struct Cyc_List_List*_tmp997=({struct Cyc_List_List*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));({void*_tmp996=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));_tmp4C3->tag=3U,({void*_tmp995=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C2=_region_malloc(yyr,sizeof(*_tmp4C2));_tmp4C2->tag=1U,_tmp4C2->f1=lis,_tmp4C2->f2=b,_tmp4C2->f3=c,_tmp4C2->f4=eff,_tmp4C2->f5=po,_tmp4C2->f6=req,_tmp4C2->f7=ens;_tmp4C2;});_tmp4C3->f1=_tmp995;});_tmp4C3;});_tmp4C4->hd=_tmp996;}),({struct Cyc_List_List*_tmp994=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C4->tl=_tmp994;});_tmp4C4;});_tmp744.tms=_tmp997;});_tmp744;}));
# 1940
goto _LL0;}}case 187U: _LL171: _LL172:
# 1941 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;({struct _tuple0*_tmp9A3=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp745.id=_tmp9A3;}),({unsigned _tmp9A2=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp745.varloc=_tmp9A2;}),({
struct Cyc_List_List*_tmp9A1=({struct Cyc_List_List*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));({void*_tmp9A0=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));_tmp4C6->tag=3U,({void*_tmp99F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));_tmp4C5->tag=1U,_tmp4C5->f1=0,_tmp4C5->f2=0,_tmp4C5->f3=0,({
# 1944
void*_tmp99E=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4C5->f4=_tmp99E;}),({struct Cyc_List_List*_tmp99D=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4C5->f5=_tmp99D;}),({struct Cyc_Absyn_Exp*_tmp99C=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4C5->f6=_tmp99C;}),({struct Cyc_Absyn_Exp*_tmp99B=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4C5->f7=_tmp99B;});_tmp4C5;});
# 1942
_tmp4C6->f1=_tmp99F;});_tmp4C6;});_tmp4C7->hd=_tmp9A0;}),({
# 1945
struct Cyc_List_List*_tmp99A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C7->tl=_tmp99A;});_tmp4C7;});
# 1942
_tmp745.tms=_tmp9A1;});_tmp745;}));
# 1947
goto _LL0;case 188U: _LL173: _LL174:
# 1948 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;({struct _tuple0*_tmp9AA=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp746.id=_tmp9AA;}),({unsigned _tmp9A9=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp746.varloc=_tmp9A9;}),({struct Cyc_List_List*_tmp9A8=({struct Cyc_List_List*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));({void*_tmp9A7=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));_tmp4C9->tag=3U,({void*_tmp9A6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));_tmp4C8->tag=0U,({struct Cyc_List_List*_tmp9A5=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4C8->f1=_tmp9A5;}),_tmp4C8->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4C8;});_tmp4C9->f1=_tmp9A6;});_tmp4C9;});_tmp4CA->hd=_tmp9A7;}),({struct Cyc_List_List*_tmp9A4=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CA->tl=_tmp9A4;});_tmp4CA;});_tmp746.tms=_tmp9A8;});_tmp746;}));
goto _LL0;case 189U: _LL175: _LL176: {
# 1951
struct Cyc_List_List*_tmp4CB=({unsigned _tmp9AB=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9AB,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4CB;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;({struct _tuple0*_tmp9B0=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp747.id=_tmp9B0;}),({unsigned _tmp9AF=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp747.varloc=_tmp9AF;}),({struct Cyc_List_List*_tmp9AE=({struct Cyc_List_List*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));({void*_tmp9AD=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));_tmp4CC->tag=4U,_tmp4CC->f1=ts,_tmp4CC->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4CC->f3=0;_tmp4CC;});_tmp4CD->hd=_tmp9AD;}),({struct Cyc_List_List*_tmp9AC=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CD->tl=_tmp9AC;});_tmp4CD;});_tmp747.tms=_tmp9AE;});_tmp747;}));
# 1954
goto _LL0;}case 190U: _LL177: _LL178:
# 1955 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;({struct _tuple0*_tmp9B6=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp748.id=_tmp9B6;}),({unsigned _tmp9B5=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp748.varloc=_tmp9B5;}),({struct Cyc_List_List*_tmp9B4=({struct Cyc_List_List*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));({void*_tmp9B3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));_tmp4CE->tag=5U,_tmp4CE->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B2=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4CE->f2=_tmp9B2;});_tmp4CE;});_tmp4CF->hd=_tmp9B3;}),({
struct Cyc_List_List*_tmp9B1=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CF->tl=_tmp9B1;});_tmp4CF;});
# 1955
_tmp748.tms=_tmp9B4;});_tmp748;}));
# 1958
goto _LL0;case 191U: _LL179: _LL17A:
# 1963 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;({struct _tuple0*_tmp9B7=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp749.id=_tmp9B7;}),_tmp749.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp749.tms=0;_tmp749;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 1965 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;({struct _tuple0*_tmp9B8=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp74A.id=_tmp9B8;}),_tmp74A.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp74A.tms=0;_tmp74A;}));
goto _LL0;case 193U: _LL17D: _LL17E:
# 1967 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 194U: _LL17F: _LL180: {
# 1971 "parse.y"
struct Cyc_Parse_Declarator _tmp4D0=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4D0;
({struct Cyc_List_List*_tmp9BB=({struct Cyc_List_List*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));({void*_tmp9BA=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));_tmp4D1->tag=5U,_tmp4D1->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B9=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D1->f2=_tmp9B9;});_tmp4D1;});_tmp4D2->hd=_tmp9BA;}),_tmp4D2->tl=d.tms;_tmp4D2;});d.tms=_tmp9BB;});
yyval=(yyyvsp[2]).v;
# 1975
goto _LL0;}case 195U: _LL181: _LL182: {
# 1976 "parse.y"
struct Cyc_Parse_Declarator _tmp4D3=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;_tmp74B.id=one.id,_tmp74B.varloc=one.varloc,({
struct Cyc_List_List*_tmp9BE=({struct Cyc_List_List*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));({void*_tmp9BD=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));_tmp4D4->tag=0U,({void*_tmp9BC=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4D4->f1=_tmp9BC;}),_tmp4D4->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4D4;});_tmp4D5->hd=_tmp9BD;}),_tmp4D5->tl=one.tms;_tmp4D5;});_tmp74B.tms=_tmp9BE;});_tmp74B;}));
goto _LL0;}case 196U: _LL183: _LL184: {
# 1980 "parse.y"
struct Cyc_Parse_Declarator _tmp4D6=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D6;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74C;_tmp74C.id=one.id,_tmp74C.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C2=({struct Cyc_List_List*_tmp4D8=_region_malloc(yyr,sizeof(*_tmp4D8));({void*_tmp9C1=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));_tmp4D7->tag=1U,({struct Cyc_Absyn_Exp*_tmp9C0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4D7->f1=_tmp9C0;}),({void*_tmp9BF=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4D7->f2=_tmp9BF;}),_tmp4D7->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4D7;});_tmp4D8->hd=_tmp9C1;}),_tmp4D8->tl=one.tms;_tmp4D8;});_tmp74C.tms=_tmp9C2;});_tmp74C;}));
# 1984
goto _LL0;}case 197U: _LL185: _LL186: {
# 1985 "parse.y"
struct _tuple27*_tmp4D9=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp19=_tmp4D9;struct _tuple27*_tmp4DA=_stmttmp19;struct Cyc_List_List*_tmp4DF;void*_tmp4DE;struct Cyc_Absyn_VarargInfo*_tmp4DD;int _tmp4DC;struct Cyc_List_List*_tmp4DB;_LL470: _tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;_tmp4DD=_tmp4DA->f3;_tmp4DE=_tmp4DA->f4;_tmp4DF=_tmp4DA->f5;_LL471: {struct Cyc_List_List*lis=_tmp4DB;int b=_tmp4DC;struct Cyc_Absyn_VarargInfo*c=_tmp4DD;void*eff=_tmp4DE;struct Cyc_List_List*po=_tmp4DF;
struct Cyc_Absyn_Exp*_tmp4E0=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4E0;
struct Cyc_Absyn_Exp*_tmp4E1=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4E1;
struct Cyc_Parse_Declarator _tmp4E2=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74D;_tmp74D.id=one.id,_tmp74D.varloc=one.varloc,({struct Cyc_List_List*_tmp9C5=({struct Cyc_List_List*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));({void*_tmp9C4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));_tmp4E4->tag=3U,({void*_tmp9C3=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E3=_region_malloc(yyr,sizeof(*_tmp4E3));_tmp4E3->tag=1U,_tmp4E3->f1=lis,_tmp4E3->f2=b,_tmp4E3->f3=c,_tmp4E3->f4=eff,_tmp4E3->f5=po,_tmp4E3->f6=req,_tmp4E3->f7=ens;_tmp4E3;});_tmp4E4->f1=_tmp9C3;});_tmp4E4;});_tmp4E5->hd=_tmp9C4;}),_tmp4E5->tl=one.tms;_tmp4E5;});_tmp74D.tms=_tmp9C5;});_tmp74D;}));
# 1991
goto _LL0;}}case 198U: _LL187: _LL188: {
# 1992 "parse.y"
struct Cyc_Parse_Declarator _tmp4E6=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E6;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74E;_tmp74E.id=one.id,_tmp74E.varloc=one.varloc,({
struct Cyc_List_List*_tmp9CC=({struct Cyc_List_List*_tmp4E9=_region_malloc(yyr,sizeof(*_tmp4E9));({void*_tmp9CB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));_tmp4E8->tag=3U,({void*_tmp9CA=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));_tmp4E7->tag=1U,_tmp4E7->f1=0,_tmp4E7->f2=0,_tmp4E7->f3=0,({
# 1996
void*_tmp9C9=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4E7->f4=_tmp9C9;}),({struct Cyc_List_List*_tmp9C8=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4E7->f5=_tmp9C8;}),({struct Cyc_Absyn_Exp*_tmp9C7=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4E7->f6=_tmp9C7;}),({struct Cyc_Absyn_Exp*_tmp9C6=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4E7->f7=_tmp9C6;});_tmp4E7;});
# 1994
_tmp4E8->f1=_tmp9CA;});_tmp4E8;});_tmp4E9->hd=_tmp9CB;}),_tmp4E9->tl=one.tms;_tmp4E9;});_tmp74E.tms=_tmp9CC;});_tmp74E;}));
# 1999
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2000 "parse.y"
struct Cyc_Parse_Declarator _tmp4EA=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EA;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74F;_tmp74F.id=one.id,_tmp74F.varloc=one.varloc,({struct Cyc_List_List*_tmp9D0=({struct Cyc_List_List*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));({void*_tmp9CF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EC=_region_malloc(yyr,sizeof(*_tmp4EC));_tmp4EC->tag=3U,({void*_tmp9CE=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));_tmp4EB->tag=0U,({struct Cyc_List_List*_tmp9CD=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4EB->f1=_tmp9CD;}),_tmp4EB->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4EB;});_tmp4EC->f1=_tmp9CE;});_tmp4EC;});_tmp4ED->hd=_tmp9CF;}),_tmp4ED->tl=one.tms;_tmp4ED;});_tmp74F.tms=_tmp9D0;});_tmp74F;}));
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2004
struct Cyc_List_List*_tmp4EE=({unsigned _tmp9D1=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9D1,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4EE;
struct Cyc_Parse_Declarator _tmp4EF=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EF;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp750;_tmp750.id=one.id,_tmp750.varloc=one.varloc,({struct Cyc_List_List*_tmp9D3=({struct Cyc_List_List*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));({void*_tmp9D2=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F0=_region_malloc(yyr,sizeof(*_tmp4F0));_tmp4F0->tag=4U,_tmp4F0->f1=ts,_tmp4F0->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4F0->f3=0;_tmp4F0;});_tmp4F1->hd=_tmp9D2;}),_tmp4F1->tl=one.tms;_tmp4F1;});_tmp750.tms=_tmp9D3;});_tmp750;}));
# 2008
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2009 "parse.y"
struct Cyc_Parse_Declarator _tmp4F2=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp751;_tmp751.id=one.id,_tmp751.varloc=one.varloc,({struct Cyc_List_List*_tmp9D6=({struct Cyc_List_List*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));({void*_tmp9D5=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));_tmp4F3->tag=5U,_tmp4F3->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9D4=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4F3->f2=_tmp9D4;});_tmp4F3;});_tmp4F4->hd=_tmp9D5;}),_tmp4F4->tl=one.tms;_tmp4F4;});_tmp751.tms=_tmp9D6;});_tmp751;}));
# 2012
goto _LL0;}case 202U: _LL18F: _LL190:
# 2016 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 203U: _LL191: _LL192:
# 2017 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9D7=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9D7,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 204U: _LL193: _LL194: {
# 2021 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4F6=_region_malloc(yyr,sizeof(*_tmp4F6));({void*_tmp9D9=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));_tmp4F5->tag=5U,_tmp4F5->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9D8=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4F5->f2=_tmp9D8;});_tmp4F5;});_tmp4F6->hd=_tmp9D9;}),_tmp4F6->tl=ans;_tmp4F6;});{
# 2025
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4F7=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1A=_tmp4F7;struct _tuple22 _tmp4F8=_stmttmp1A;void*_tmp4FB;void*_tmp4FA;unsigned _tmp4F9;_LL473: _tmp4F9=_tmp4F8.f1;_tmp4FA=_tmp4F8.f2;_tmp4FB=_tmp4F8.f3;_LL474: {unsigned ploc=_tmp4F9;void*nullable=_tmp4FA;void*bound=_tmp4FB;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->ptr_loc=ploc,_tmp4FC->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp4FC->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp4FC;});{
struct _tuple15 _tmp4FD=({unsigned _tmp9DD=ploc;void*_tmp9DC=nullable;void*_tmp9DB=bound;void*_tmp9DA=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9DD,_tmp9DC,_tmp9DB,_tmp9DA,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1B=_tmp4FD;struct _tuple15 _tmp4FE=_stmttmp1B;void*_tmp502;void*_tmp501;void*_tmp500;void*_tmp4FF;_LL476: _tmp4FF=_tmp4FE.f1;_tmp500=_tmp4FE.f2;_tmp501=_tmp4FE.f3;_tmp502=_tmp4FE.f4;_LL477: {void*nullable=_tmp4FF;void*bound=_tmp500;void*zeroterm=_tmp501;void*rgn_opt=_tmp502;
ans=({struct Cyc_List_List*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));({void*_tmp9DF=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));_tmp503->tag=2U,(_tmp503->f1).rgn=rgn_opt,(_tmp503->f1).nullable=nullable,(_tmp503->f1).bounds=bound,(_tmp503->f1).zero_term=zeroterm,(_tmp503->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp9DE=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp503->f2=_tmp9DE;});_tmp503;});_tmp504->hd=_tmp9DF;}),_tmp504->tl=ans;_tmp504;});
yyval=Cyc_YY26(ans);
# 2033
goto _LL0;}}}}}case 205U: _LL195: _LL196:
# 2035
 yyval=Cyc_YY56(0);
goto _LL0;case 206U: _LL197: _LL198:
# 2036 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp505=_region_malloc(yyr,sizeof(*_tmp505));({void*_tmp9E1=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp505->hd=_tmp9E1;}),({struct Cyc_List_List*_tmp9E0=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp505->tl=_tmp9E0;});_tmp505;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2041 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=0U,({struct Cyc_Absyn_Exp*_tmp9E2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp506->f1=_tmp9E2;});_tmp506;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2043 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));_tmp507->tag=1U,({void*_tmp9E3=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp507->f1=_tmp9E3;});_tmp507;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2044 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));_tmp508->tag=2U;_tmp508;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2045 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=3U;_tmp509;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2046 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));_tmp50A->tag=4U;_tmp50A;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2047 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=5U;_tmp50B;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2048 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp50C=_region_malloc(yyr,sizeof(*_tmp50C));_tmp50C->tag=6U;_tmp50C;}));
goto _LL0;case 214U: _LL1A7: _LL1A8:
# 2049 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp50D=_region_malloc(yyr,sizeof(*_tmp50D));_tmp50D->tag=7U;_tmp50D;}));
goto _LL0;case 215U: _LL1A9: _LL1AA:
# 2055 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp50E->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp50E->f3=Cyc_Absyn_fat_bound_type:({void*_tmp9E4=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp50E->f3=_tmp9E4;});_tmp50E;}));
# 2057
goto _LL0;case 216U: _LL1AB: _LL1AC:
# 2057 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp50F->f2=Cyc_Absyn_false_type,({void*_tmp9E5=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp50F->f3=_tmp9E5;});_tmp50F;}));
goto _LL0;case 217U: _LL1AD: _LL1AE:
# 2058 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp510->f2=Cyc_Absyn_true_type,_tmp510->f3=Cyc_Absyn_fat_bound_type;_tmp510;}));
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2061
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2062 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2065
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2066 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2067 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2072 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2073 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2074 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2078 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2079 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9E6=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9E6,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2084 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp511=_cycalloc(sizeof(*_tmp511));({struct Cyc_List_List*_tmp9E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp511->f1=_tmp9E9;}),_tmp511->f2=0,_tmp511->f3=0,({void*_tmp9E8=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp511->f4=_tmp9E8;}),({struct Cyc_List_List*_tmp9E7=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp511->f5=_tmp9E7;});_tmp511;}));
goto _LL0;case 229U: _LL1C5: _LL1C6:
# 2086 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp512=_cycalloc(sizeof(*_tmp512));({struct Cyc_List_List*_tmp9EC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp512->f1=_tmp9EC;}),_tmp512->f2=1,_tmp512->f3=0,({void*_tmp9EB=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp512->f4=_tmp9EB;}),({struct Cyc_List_List*_tmp9EA=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp512->f5=_tmp9EA;});_tmp512;}));
goto _LL0;case 230U: _LL1C7: _LL1C8: {
# 2089
struct _tuple8*_tmp513=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1C=_tmp513;struct _tuple8*_tmp514=_stmttmp1C;void*_tmp517;struct Cyc_Absyn_Tqual _tmp516;struct _fat_ptr*_tmp515;_LL479: _tmp515=_tmp514->f1;_tmp516=_tmp514->f2;_tmp517=_tmp514->f3;_LL47A: {struct _fat_ptr*n=_tmp515;struct Cyc_Absyn_Tqual tq=_tmp516;void*t=_tmp517;
struct Cyc_Absyn_VarargInfo*_tmp518=({struct Cyc_Absyn_VarargInfo*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->name=n,_tmp51A->tq=tq,_tmp51A->type=t,({int _tmp9ED=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp51A->inject=_tmp9ED;});_tmp51A;});struct Cyc_Absyn_VarargInfo*v=_tmp518;
yyval=Cyc_YY39(({struct _tuple27*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->f1=0,_tmp519->f2=0,_tmp519->f3=v,({void*_tmp9EF=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp519->f4=_tmp9EF;}),({struct Cyc_List_List*_tmp9EE=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp519->f5=_tmp9EE;});_tmp519;}));
# 2093
goto _LL0;}}case 231U: _LL1C9: _LL1CA: {
# 2095
struct _tuple8*_tmp51B=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1D=_tmp51B;struct _tuple8*_tmp51C=_stmttmp1D;void*_tmp51F;struct Cyc_Absyn_Tqual _tmp51E;struct _fat_ptr*_tmp51D;_LL47C: _tmp51D=_tmp51C->f1;_tmp51E=_tmp51C->f2;_tmp51F=_tmp51C->f3;_LL47D: {struct _fat_ptr*n=_tmp51D;struct Cyc_Absyn_Tqual tq=_tmp51E;void*t=_tmp51F;
struct Cyc_Absyn_VarargInfo*_tmp520=({struct Cyc_Absyn_VarargInfo*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->name=n,_tmp522->tq=tq,_tmp522->type=t,({int _tmp9F0=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp522->inject=_tmp9F0;});_tmp522;});struct Cyc_Absyn_VarargInfo*v=_tmp520;
yyval=Cyc_YY39(({struct _tuple27*_tmp521=_cycalloc(sizeof(*_tmp521));({struct Cyc_List_List*_tmp9F3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp521->f1=_tmp9F3;}),_tmp521->f2=0,_tmp521->f3=v,({void*_tmp9F2=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp521->f4=_tmp9F2;}),({struct Cyc_List_List*_tmp9F1=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp521->f5=_tmp9F1;});_tmp521;}));
# 2099
goto _LL0;}}case 232U: _LL1CB: _LL1CC:
# 2103 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9F4=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9F4,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->tag=1U,_tmp523->f1=0;_tmp523;}));}));
goto _LL0;case 233U: _LL1CD: _LL1CE:
# 2104 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9F5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9F5,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2107
 yyval=Cyc_YY49(0);
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2108 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2112 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 237U: _LL1D5: _LL1D6:
# 2113 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 238U: _LL1D7: _LL1D8: {
# 2121 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp524=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528->tag=2U,_tmp528->f1=0,_tmp528->f2=& Cyc_Kinds_trk;_tmp528;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp524;
void*_tmp525=({struct _fat_ptr _tmp9F6=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9F6,(void*)kb);});void*t=_tmp525;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp527=_cycalloc(sizeof(*_tmp527));({struct _tuple34*_tmp9F8=({struct _tuple34*_tmp526=_cycalloc(sizeof(*_tmp526));({void*_tmp9F7=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp526->f1=_tmp9F7;}),_tmp526->f2=t;_tmp526;});_tmp527->hd=_tmp9F8;}),_tmp527->tl=0;_tmp527;}));
# 2125
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2127 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp529=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->tag=2U,_tmp52D->f1=0,_tmp52D->f2=& Cyc_Kinds_trk;_tmp52D;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp529;
void*_tmp52A=({struct _fat_ptr _tmp9F9=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9F9,(void*)kb);});void*t=_tmp52A;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp52C=_cycalloc(sizeof(*_tmp52C));({struct _tuple34*_tmp9FC=({struct _tuple34*_tmp52B=_cycalloc(sizeof(*_tmp52B));({void*_tmp9FB=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp52B->f1=_tmp9FB;}),_tmp52B->f2=t;_tmp52B;});_tmp52C->hd=_tmp9FC;}),({struct Cyc_List_List*_tmp9FA=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp52C->tl=_tmp9FA;});_tmp52C;}));
# 2131
goto _LL0;}case 240U: _LL1DB: _LL1DC:
# 2135 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2137 "parse.y"
 if(({struct _fat_ptr _tmp9FD=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp9FD,({const char*_tmp52E="inject";_tag_fat(_tmp52E,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp530=({struct Cyc_Warn_String_Warn_Warg_struct _tmp752;_tmp752.tag=0U,({struct _fat_ptr _tmp9FE=({const char*_tmp531="missing type in function declaration";_tag_fat(_tmp531,sizeof(char),37U);});_tmp752.f1=_tmp9FE;});_tmp752;});void*_tmp52F[1U];_tmp52F[0]=& _tmp530;({unsigned _tmp9FF=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp9FF,_tag_fat(_tmp52F,sizeof(void*),1U));});});
yyval=Cyc_YY31(1);
# 2141
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2144 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2145 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmpA00=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA00,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2149 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2150 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2152 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp532=_cycalloc(sizeof(*_tmp532));({void*_tmpA01=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp532->hd=_tmpA01;}),_tmp532->tl=0;_tmp532;}));
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2154 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp533=_cycalloc(sizeof(*_tmp533));({void*_tmpA02=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp533->hd=_tmpA02;}),_tmp533->tl=0;_tmp533;}));
# 2157
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2162 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp534=_cycalloc(sizeof(*_tmp534));({void*_tmpA04=Cyc_Absyn_access_eff(({struct _tuple8*_tmpA03=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA03,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp534->hd=_tmpA04;}),_tmp534->tl=0;_tmp534;}));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2164 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp535=_cycalloc(sizeof(*_tmp535));({void*_tmpA07=Cyc_Absyn_access_eff(({struct _tuple8*_tmpA06=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA06,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp535->hd=_tmpA07;}),({struct Cyc_List_List*_tmpA05=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp535->tl=_tmpA05;});_tmp535;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2170 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({struct _tuple8*_tmpA08=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp536->hd=_tmpA08;}),_tmp536->tl=0;_tmp536;}));
goto _LL0;case 251U: _LL1F1: _LL1F2:
# 2172 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));({struct _tuple8*_tmpA0A=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp537->hd=_tmpA0A;}),({struct Cyc_List_List*_tmpA09=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp537->tl=_tmpA09;});_tmp537;}));
goto _LL0;case 252U: _LL1F3: _LL1F4: {
# 2178 "parse.y"
struct _tuple26 _tmp538=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp1E=_tmp538;struct _tuple26 _tmp539=_stmttmp1E;struct Cyc_List_List*_tmp53C;struct Cyc_Parse_Type_specifier _tmp53B;struct Cyc_Absyn_Tqual _tmp53A;_LL47F: _tmp53A=_tmp539.f1;_tmp53B=_tmp539.f2;_tmp53C=_tmp539.f3;_LL480: {struct Cyc_Absyn_Tqual tq=_tmp53A;struct Cyc_Parse_Type_specifier tspecs=_tmp53B;struct Cyc_List_List*atts=_tmp53C;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp53D=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1F=_tmp53D;struct Cyc_Parse_Declarator _tmp53E=_stmttmp1F;struct Cyc_List_List*_tmp541;unsigned _tmp540;struct _tuple0*_tmp53F;_LL482: _tmp53F=_tmp53E.id;_tmp540=_tmp53E.varloc;_tmp541=_tmp53E.tms;_LL483: {struct _tuple0*qv=_tmp53F;unsigned varloc=_tmp540;struct Cyc_List_List*tms=_tmp541;
void*_tmp542=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp542;
struct _tuple14 _tmp543=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp20=_tmp543;struct _tuple14 _tmp544=_stmttmp20;struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp547;void*_tmp546;struct Cyc_Absyn_Tqual _tmp545;_LL485: _tmp545=_tmp544.f1;_tmp546=_tmp544.f2;_tmp547=_tmp544.f3;_tmp548=_tmp544.f4;_LL486: {struct Cyc_Absyn_Tqual tq2=_tmp545;void*t2=_tmp546;struct Cyc_List_List*tvs=_tmp547;struct Cyc_List_List*atts2=_tmp548;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp753;_tmp753.tag=0U,({struct _fat_ptr _tmpA0B=({const char*_tmp54B="parameter with bad type params";_tag_fat(_tmp54B,sizeof(char),31U);});_tmp753.f1=_tmpA0B;});_tmp753;});void*_tmp549[1U];_tmp549[0]=& _tmp54A;({unsigned _tmpA0C=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmpA0C,_tag_fat(_tmp549,sizeof(void*),1U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp754;_tmp754.tag=0U,({struct _fat_ptr _tmpA0D=({const char*_tmp54E="parameter cannot be qualified with a namespace";_tag_fat(_tmp54E,sizeof(char),47U);});_tmp754.f1=_tmpA0D;});_tmp754;});void*_tmp54C[1U];_tmp54C[0]=& _tmp54D;({unsigned _tmpA0E=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA0E,_tag_fat(_tmp54C,sizeof(void*),1U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp550=({struct Cyc_Warn_String_Warn_Warg_struct _tmp755;_tmp755.tag=0U,({struct _fat_ptr _tmpA0F=({const char*_tmp551="extra attributes on parameter, ignoring";_tag_fat(_tmp551,sizeof(char),40U);});_tmp755.f1=_tmpA0F;});_tmp755;});void*_tmp54F[1U];_tmp54F[0]=& _tmp550;({unsigned _tmpA10=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA10,_tag_fat(_tmp54F,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->f1=idopt,_tmp552->f2=tq2,_tmp552->f3=t2;_tmp552;}));
# 2192
goto _LL0;}}}}}}case 253U: _LL1F5: _LL1F6: {
# 2193 "parse.y"
struct _tuple26 _tmp553=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp553;struct _tuple26 _tmp554=_stmttmp21;struct Cyc_List_List*_tmp557;struct Cyc_Parse_Type_specifier _tmp556;struct Cyc_Absyn_Tqual _tmp555;_LL488: _tmp555=_tmp554.f1;_tmp556=_tmp554.f2;_tmp557=_tmp554.f3;_LL489: {struct Cyc_Absyn_Tqual tq=_tmp555;struct Cyc_Parse_Type_specifier tspecs=_tmp556;struct Cyc_List_List*atts=_tmp557;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp558=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp558;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp55A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp756;_tmp756.tag=0U,({struct _fat_ptr _tmpA11=({const char*_tmp55B="bad attributes on parameter, ignoring";_tag_fat(_tmp55B,sizeof(char),38U);});_tmp756.f1=_tmpA11;});_tmp756;});void*_tmp559[1U];_tmp559[0]=& _tmp55A;({unsigned _tmpA12=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA12,_tag_fat(_tmp559,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->f1=0,_tmp55C->f2=tq,_tmp55C->f3=t;_tmp55C;}));
# 2200
goto _LL0;}}}case 254U: _LL1F7: _LL1F8: {
# 2201 "parse.y"
struct _tuple26 _tmp55D=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp22=_tmp55D;struct _tuple26 _tmp55E=_stmttmp22;struct Cyc_List_List*_tmp561;struct Cyc_Parse_Type_specifier _tmp560;struct Cyc_Absyn_Tqual _tmp55F;_LL48B: _tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;_tmp561=_tmp55E.f3;_LL48C: {struct Cyc_Absyn_Tqual tq=_tmp55F;struct Cyc_Parse_Type_specifier tspecs=_tmp560;struct Cyc_List_List*atts=_tmp561;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp562=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp562;
struct Cyc_List_List*_tmp563=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp563;
struct _tuple14 _tmp564=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp564;struct _tuple14 _tmp565=_stmttmp23;struct Cyc_List_List*_tmp569;struct Cyc_List_List*_tmp568;void*_tmp567;struct Cyc_Absyn_Tqual _tmp566;_LL48E: _tmp566=_tmp565.f1;_tmp567=_tmp565.f2;_tmp568=_tmp565.f3;_tmp569=_tmp565.f4;_LL48F: {struct Cyc_Absyn_Tqual tq2=_tmp566;void*t2=_tmp567;struct Cyc_List_List*tvs=_tmp568;struct Cyc_List_List*atts2=_tmp569;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp757;_tmp757.tag=0U,({
struct _fat_ptr _tmpA13=({const char*_tmp56C="bad type parameters on formal argument, ignoring";_tag_fat(_tmp56C,sizeof(char),49U);});_tmp757.f1=_tmpA13;});_tmp757;});void*_tmp56A[1U];_tmp56A[0]=& _tmp56B;({unsigned _tmpA14=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA14,_tag_fat(_tmp56A,sizeof(void*),1U));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp758;_tmp758.tag=0U,({struct _fat_ptr _tmpA15=({const char*_tmp56F="bad attributes on parameter, ignoring";_tag_fat(_tmp56F,sizeof(char),38U);});_tmp758.f1=_tmpA15;});_tmp758;});void*_tmp56D[1U];_tmp56D[0]=& _tmp56E;({unsigned _tmpA16=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA16,_tag_fat(_tmp56D,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->f1=0,_tmp570->f2=tq2,_tmp570->f3=t2;_tmp570;}));
# 2213
goto _LL0;}}}}case 255U: _LL1F9: _LL1FA:
# 2217 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2221 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));({struct _fat_ptr*_tmpA18=({struct _fat_ptr*_tmp571=_cycalloc(sizeof(*_tmp571));({struct _fat_ptr _tmpA17=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp571=_tmpA17;});_tmp571;});_tmp572->hd=_tmpA18;}),_tmp572->tl=0;_tmp572;}));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2223 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp574=_cycalloc(sizeof(*_tmp574));({struct _fat_ptr*_tmpA1B=({struct _fat_ptr*_tmp573=_cycalloc(sizeof(*_tmp573));({struct _fat_ptr _tmpA1A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp573=_tmpA1A;});_tmp573;});_tmp574->hd=_tmpA1B;}),({struct Cyc_List_List*_tmpA19=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp574->tl=_tmpA19;});_tmp574;}));
goto _LL0;case 258U: _LL1FF: _LL200:
# 2227 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 259U: _LL201: _LL202:
# 2228 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 260U: _LL203: _LL204:
# 2233 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA1C=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->tag=36U,_tmp575->f1=0,_tmp575->f2=0;_tmp575;});Cyc_Absyn_new_exp(_tmpA1C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 261U: _LL205: _LL206:
# 2235 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA1E=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=36U,_tmp576->f1=0,({struct Cyc_List_List*_tmpA1D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp576->f2=_tmpA1D;});_tmp576;});Cyc_Absyn_new_exp(_tmpA1E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208:
# 2237 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA20=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->tag=36U,_tmp577->f1=0,({struct Cyc_List_List*_tmpA1F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp577->f2=_tmpA1F;});_tmp577;});Cyc_Absyn_new_exp(_tmpA20,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 263U: _LL209: _LL20A: {
# 2239 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp578=({unsigned _tmpA25=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA24=({struct _tuple0*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA22=({struct _fat_ptr*_tmp57A=_cycalloc(sizeof(*_tmp57A));({struct _fat_ptr _tmpA21=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp57A=_tmpA21;});_tmp57A;});_tmp57B->f2=_tmpA22;});_tmp57B;});void*_tmpA23=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA25,_tmpA24,_tmpA23,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2239
struct Cyc_Absyn_Vardecl*vd=_tmp578;
# 2242
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA28=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=27U,_tmp579->f1=vd,({struct Cyc_Absyn_Exp*_tmpA27=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp579->f2=_tmpA27;}),({struct Cyc_Absyn_Exp*_tmpA26=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp579->f3=_tmpA26;}),_tmp579->f4=0;_tmp579;});Cyc_Absyn_new_exp(_tmpA28,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2245
goto _LL0;}case 264U: _LL20B: _LL20C: {
# 2247 "parse.y"
void*_tmp57C=({struct _tuple8*_tmpA29=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA29,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp57C;
yyval=Cyc_Exp_tok(({void*_tmpA2B=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=28U,({struct Cyc_Absyn_Exp*_tmpA2A=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp57D->f1=_tmpA2A;}),_tmp57D->f2=t,_tmp57D->f3=0;_tmp57D;});Cyc_Absyn_new_exp(_tmpA2B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2250
goto _LL0;}case 265U: _LL20D: _LL20E:
# 2255 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(*_tmp57F));({struct _tuple35*_tmpA2D=({struct _tuple35*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->f1=0,({struct Cyc_Absyn_Exp*_tmpA2C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp57E->f2=_tmpA2C;});_tmp57E;});_tmp57F->hd=_tmpA2D;}),_tmp57F->tl=0;_tmp57F;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2257 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp581=_cycalloc(sizeof(*_tmp581));({struct _tuple35*_tmpA30=({struct _tuple35*_tmp580=_cycalloc(sizeof(*_tmp580));({struct Cyc_List_List*_tmpA2F=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp580->f1=_tmpA2F;}),({struct Cyc_Absyn_Exp*_tmpA2E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp580->f2=_tmpA2E;});_tmp580;});_tmp581->hd=_tmpA30;}),_tmp581->tl=0;_tmp581;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2259 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));({struct _tuple35*_tmpA33=({struct _tuple35*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->f1=0,({struct Cyc_Absyn_Exp*_tmpA32=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp582->f2=_tmpA32;});_tmp582;});_tmp583->hd=_tmpA33;}),({struct Cyc_List_List*_tmpA31=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp583->tl=_tmpA31;});_tmp583;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2261 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));({struct _tuple35*_tmpA37=({struct _tuple35*_tmp584=_cycalloc(sizeof(*_tmp584));({struct Cyc_List_List*_tmpA36=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp584->f1=_tmpA36;}),({struct Cyc_Absyn_Exp*_tmpA35=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp584->f2=_tmpA35;});_tmp584;});_tmp585->hd=_tmpA37;}),({struct Cyc_List_List*_tmpA34=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp585->tl=_tmpA34;});_tmp585;}));
goto _LL0;case 269U: _LL215: _LL216:
# 2265 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 270U: _LL217: _LL218:
# 2266 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));({void*_tmpA3A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->tag=1U,({struct _fat_ptr*_tmpA39=({struct _fat_ptr*_tmp586=_cycalloc(sizeof(*_tmp586));({struct _fat_ptr _tmpA38=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp586=_tmpA38;});_tmp586;});_tmp587->f1=_tmpA39;});_tmp587;});_tmp588->hd=_tmpA3A;}),_tmp588->tl=0;_tmp588;}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2271 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));({void*_tmpA3B=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp589->hd=_tmpA3B;}),_tmp589->tl=0;_tmp589;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2272 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));({void*_tmpA3D=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp58A->hd=_tmpA3D;}),({struct Cyc_List_List*_tmpA3C=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp58A->tl=_tmpA3C;});_tmp58A;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2276 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->tag=0U,({struct Cyc_Absyn_Exp*_tmpA3E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp58B->f1=_tmpA3E;});_tmp58B;}));
goto _LL0;case 274U: _LL21F: _LL220:
# 2277 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->tag=1U,({struct _fat_ptr*_tmpA40=({struct _fat_ptr*_tmp58C=_cycalloc(sizeof(*_tmp58C));({struct _fat_ptr _tmpA3F=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp58C=_tmpA3F;});_tmp58C;});_tmp58D->f1=_tmpA40;});_tmp58D;}));
goto _LL0;case 275U: _LL221: _LL222: {
# 2282 "parse.y"
struct _tuple26 _tmp58E=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp58E;struct _tuple26 _tmp58F=_stmttmp24;struct Cyc_List_List*_tmp592;struct Cyc_Parse_Type_specifier _tmp591;struct Cyc_Absyn_Tqual _tmp590;_LL491: _tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;_tmp592=_tmp58F.f3;_LL492: {struct Cyc_Absyn_Tqual tq=_tmp590;struct Cyc_Parse_Type_specifier tss=_tmp591;struct Cyc_List_List*atts=_tmp592;
void*_tmp593=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp593;
if(atts != 0)
({void*_tmp594=0U;({unsigned _tmpA42=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA41=({const char*_tmp595="ignoring attributes in type";_tag_fat(_tmp595,sizeof(char),28U);});Cyc_Warn_warn(_tmpA42,_tmpA41,_tag_fat(_tmp594,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->f1=0,_tmp596->f2=tq,_tmp596->f3=t;_tmp596;}));
# 2288
goto _LL0;}}case 276U: _LL223: _LL224: {
# 2289 "parse.y"
struct _tuple26 _tmp597=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp597;struct _tuple26 _tmp598=_stmttmp25;struct Cyc_List_List*_tmp59B;struct Cyc_Parse_Type_specifier _tmp59A;struct Cyc_Absyn_Tqual _tmp599;_LL494: _tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;_tmp59B=_tmp598.f3;_LL495: {struct Cyc_Absyn_Tqual tq=_tmp599;struct Cyc_Parse_Type_specifier tss=_tmp59A;struct Cyc_List_List*atts=_tmp59B;
void*_tmp59C=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp59C;
struct Cyc_List_List*_tmp59D=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp59D;
struct _tuple14 _tmp59E=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp59E;
if(t_info.f3 != 0)
# 2295
({void*_tmp59F=0U;({unsigned _tmpA44=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA43=({const char*_tmp5A0="bad type params, ignoring";_tag_fat(_tmp5A0,sizeof(char),26U);});Cyc_Warn_warn(_tmpA44,_tmpA43,_tag_fat(_tmp59F,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp5A1=0U;({unsigned _tmpA46=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA45=({const char*_tmp5A2="bad specifiers, ignoring";_tag_fat(_tmp5A2,sizeof(char),25U);});Cyc_Warn_warn(_tmpA46,_tmpA45,_tag_fat(_tmp5A1,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->f1=0,_tmp5A3->f2=t_info.f1,_tmp5A3->f3=t_info.f2;_tmp5A3;}));
# 2300
goto _LL0;}}case 277U: _LL225: _LL226:
# 2303 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA47=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA47,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 278U: _LL227: _LL228:
# 2304 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 279U: _LL229: _LL22A:
# 2305 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2306 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2307 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));({void*_tmpA49=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A4->hd=_tmpA49;}),({struct Cyc_List_List*_tmpA48=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp5A4->tl=_tmpA48;});_tmp5A4;})));
goto _LL0;case 282U: _LL22F: _LL230:
# 2313 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));({void*_tmpA4A=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A5->hd=_tmpA4A;}),_tmp5A5->tl=0;_tmp5A5;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2314 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({void*_tmpA4C=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp5A6->hd=_tmpA4C;}),({struct Cyc_List_List*_tmpA4B=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp5A6->tl=_tmpA4B;});_tmp5A6;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2319 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp759;({struct Cyc_List_List*_tmpA4D=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp759.tms=_tmpA4D;});_tmp759;}));
goto _LL0;case 285U: _LL235: _LL236:
# 2321 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 286U: _LL237: _LL238:
# 2323 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75A;({struct Cyc_List_List*_tmpA4F=({struct Cyc_List_List*_tmpA4E=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA4E,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp75A.tms=_tmpA4F;});_tmp75A;}));
goto _LL0;case 287U: _LL239: _LL23A:
# 2328 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 288U: _LL23B: _LL23C:
# 2330 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75B;({struct Cyc_List_List*_tmpA52=({struct Cyc_List_List*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));({void*_tmpA51=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));_tmp5A7->tag=0U,({void*_tmpA50=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5A7->f1=_tmpA50;}),_tmp5A7->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5A7;});_tmp5A8->hd=_tmpA51;}),_tmp5A8->tl=0;_tmp5A8;});_tmp75B.tms=_tmpA52;});_tmp75B;}));
goto _LL0;case 289U: _LL23D: _LL23E:
# 2332 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75C;({struct Cyc_List_List*_tmpA56=({struct Cyc_List_List*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));({void*_tmpA55=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));_tmp5A9->tag=0U,({void*_tmpA54=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A9->f1=_tmpA54;}),_tmp5A9->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A9;});_tmp5AA->hd=_tmpA55;}),({struct Cyc_List_List*_tmpA53=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5AA->tl=_tmpA53;});_tmp5AA;});_tmp75C.tms=_tmpA56;});_tmp75C;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2334 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpA5A=({struct Cyc_List_List*_tmp5AC=_region_malloc(yyr,sizeof(*_tmp5AC));({void*_tmpA59=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5AB=_region_malloc(yyr,sizeof(*_tmp5AB));_tmp5AB->tag=1U,({struct Cyc_Absyn_Exp*_tmpA58=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5AB->f1=_tmpA58;}),({void*_tmpA57=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5AB->f2=_tmpA57;}),_tmp5AB->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5AB;});_tmp5AC->hd=_tmpA59;}),_tmp5AC->tl=0;_tmp5AC;});_tmp75D.tms=_tmpA5A;});_tmp75D;}));
goto _LL0;case 291U: _LL241: _LL242:
# 2336 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75E;({struct Cyc_List_List*_tmpA5F=({struct Cyc_List_List*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));({void*_tmpA5E=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5AD=_region_malloc(yyr,sizeof(*_tmp5AD));_tmp5AD->tag=1U,({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5AD->f1=_tmpA5D;}),({void*_tmpA5C=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5AD->f2=_tmpA5C;}),_tmp5AD->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5AD;});_tmp5AE->hd=_tmpA5E;}),({
struct Cyc_List_List*_tmpA5B=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5AE->tl=_tmpA5B;});_tmp5AE;});
# 2336
_tmp75E.tms=_tmpA5F;});_tmp75E;}));
# 2339
goto _LL0;case 292U: _LL243: _LL244:
# 2340 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75F;({struct Cyc_List_List*_tmpA66=({struct Cyc_List_List*_tmp5B1=_region_malloc(yyr,sizeof(*_tmp5B1));({void*_tmpA65=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B0=_region_malloc(yyr,sizeof(*_tmp5B0));_tmp5B0->tag=3U,({void*_tmpA64=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));_tmp5AF->tag=1U,_tmp5AF->f1=0,_tmp5AF->f2=0,_tmp5AF->f3=0,({void*_tmpA63=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5AF->f4=_tmpA63;}),({struct Cyc_List_List*_tmpA62=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5AF->f5=_tmpA62;}),({struct Cyc_Absyn_Exp*_tmpA61=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5AF->f6=_tmpA61;}),({struct Cyc_Absyn_Exp*_tmpA60=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5AF->f7=_tmpA60;});_tmp5AF;});_tmp5B0->f1=_tmpA64;});_tmp5B0;});_tmp5B1->hd=_tmpA65;}),_tmp5B1->tl=0;_tmp5B1;});_tmp75F.tms=_tmpA66;});_tmp75F;}));
# 2342
goto _LL0;case 293U: _LL245: _LL246: {
# 2343 "parse.y"
struct _tuple27*_tmp5B2=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp26=_tmp5B2;struct _tuple27*_tmp5B3=_stmttmp26;struct Cyc_List_List*_tmp5B8;void*_tmp5B7;struct Cyc_Absyn_VarargInfo*_tmp5B6;int _tmp5B5;struct Cyc_List_List*_tmp5B4;_LL497: _tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B3->f2;_tmp5B6=_tmp5B3->f3;_tmp5B7=_tmp5B3->f4;_tmp5B8=_tmp5B3->f5;_LL498: {struct Cyc_List_List*lis=_tmp5B4;int b=_tmp5B5;struct Cyc_Absyn_VarargInfo*c=_tmp5B6;void*eff=_tmp5B7;struct Cyc_List_List*po=_tmp5B8;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp760;({struct Cyc_List_List*_tmpA6B=({struct Cyc_List_List*_tmp5BB=_region_malloc(yyr,sizeof(*_tmp5BB));({void*_tmpA6A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BA=_region_malloc(yyr,sizeof(*_tmp5BA));_tmp5BA->tag=3U,({void*_tmpA69=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B9=_region_malloc(yyr,sizeof(*_tmp5B9));_tmp5B9->tag=1U,_tmp5B9->f1=lis,_tmp5B9->f2=b,_tmp5B9->f3=c,_tmp5B9->f4=eff,_tmp5B9->f5=po,({struct Cyc_Absyn_Exp*_tmpA68=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5B9->f6=_tmpA68;}),({struct Cyc_Absyn_Exp*_tmpA67=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B9->f7=_tmpA67;});_tmp5B9;});_tmp5BA->f1=_tmpA69;});_tmp5BA;});_tmp5BB->hd=_tmpA6A;}),_tmp5BB->tl=0;_tmp5BB;});_tmp760.tms=_tmpA6B;});_tmp760;}));
# 2346
goto _LL0;}}case 294U: _LL247: _LL248:
# 2347 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp761;({struct Cyc_List_List*_tmpA73=({struct Cyc_List_List*_tmp5BE=_region_malloc(yyr,sizeof(*_tmp5BE));({void*_tmpA72=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BD=_region_malloc(yyr,sizeof(*_tmp5BD));_tmp5BD->tag=3U,({void*_tmpA71=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5BC=_region_malloc(yyr,sizeof(*_tmp5BC));_tmp5BC->tag=1U,_tmp5BC->f1=0,_tmp5BC->f2=0,_tmp5BC->f3=0,({void*_tmpA70=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5BC->f4=_tmpA70;}),({struct Cyc_List_List*_tmpA6F=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5BC->f5=_tmpA6F;}),({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5BC->f6=_tmpA6E;}),({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5BC->f7=_tmpA6D;});_tmp5BC;});_tmp5BD->f1=_tmpA71;});_tmp5BD;});_tmp5BE->hd=_tmpA72;}),({
struct Cyc_List_List*_tmpA6C=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5BE->tl=_tmpA6C;});_tmp5BE;});
# 2347
_tmp761.tms=_tmpA73;});_tmp761;}));
# 2350
goto _LL0;case 295U: _LL249: _LL24A: {
# 2351 "parse.y"
struct _tuple27*_tmp5BF=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp27=_tmp5BF;struct _tuple27*_tmp5C0=_stmttmp27;struct Cyc_List_List*_tmp5C5;void*_tmp5C4;struct Cyc_Absyn_VarargInfo*_tmp5C3;int _tmp5C2;struct Cyc_List_List*_tmp5C1;_LL49A: _tmp5C1=_tmp5C0->f1;_tmp5C2=_tmp5C0->f2;_tmp5C3=_tmp5C0->f3;_tmp5C4=_tmp5C0->f4;_tmp5C5=_tmp5C0->f5;_LL49B: {struct Cyc_List_List*lis=_tmp5C1;int b=_tmp5C2;struct Cyc_Absyn_VarargInfo*c=_tmp5C3;void*eff=_tmp5C4;struct Cyc_List_List*po=_tmp5C5;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp762;({struct Cyc_List_List*_tmpA79=({struct Cyc_List_List*_tmp5C8=_region_malloc(yyr,sizeof(*_tmp5C8));({void*_tmpA78=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));_tmp5C7->tag=3U,({void*_tmpA77=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));_tmp5C6->tag=1U,_tmp5C6->f1=lis,_tmp5C6->f2=b,_tmp5C6->f3=c,_tmp5C6->f4=eff,_tmp5C6->f5=po,({
struct Cyc_Absyn_Exp*_tmpA76=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5C6->f6=_tmpA76;}),({struct Cyc_Absyn_Exp*_tmpA75=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5C6->f7=_tmpA75;});_tmp5C6;});
# 2352
_tmp5C7->f1=_tmpA77;});_tmp5C7;});_tmp5C8->hd=_tmpA78;}),({
struct Cyc_List_List*_tmpA74=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C8->tl=_tmpA74;});_tmp5C8;});
# 2352
_tmp762.tms=_tmpA79;});_tmp762;}));
# 2355
goto _LL0;}}case 296U: _LL24B: _LL24C: {
# 2357
struct Cyc_List_List*_tmp5C9=({unsigned _tmpA7A=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA7A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5C9;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp763;({struct Cyc_List_List*_tmpA7D=({struct Cyc_List_List*_tmp5CB=_region_malloc(yyr,sizeof(*_tmp5CB));({void*_tmpA7C=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));_tmp5CA->tag=4U,_tmp5CA->f1=ts,_tmp5CA->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5CA->f3=0;_tmp5CA;});_tmp5CB->hd=_tmpA7C;}),({
struct Cyc_List_List*_tmpA7B=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CB->tl=_tmpA7B;});_tmp5CB;});
# 2358
_tmp763.tms=_tmpA7D;});_tmp763;}));
# 2361
goto _LL0;}case 297U: _LL24D: _LL24E:
# 2362 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp764;({struct Cyc_List_List*_tmpA81=({struct Cyc_List_List*_tmp5CD=_region_malloc(yyr,sizeof(*_tmp5CD));({void*_tmpA80=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5CC=_region_malloc(yyr,sizeof(*_tmp5CC));_tmp5CC->tag=5U,_tmp5CC->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA7F=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5CC->f2=_tmpA7F;});_tmp5CC;});_tmp5CD->hd=_tmpA80;}),({struct Cyc_List_List*_tmpA7E=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CD->tl=_tmpA7E;});_tmp5CD;});_tmp764.tms=_tmpA81;});_tmp764;}));
# 2364
goto _LL0;case 298U: _LL24F: _LL250:
# 2368 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299U: _LL251: _LL252:
# 2369 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2370 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2371 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2372 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2373 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 304U: _LL25B: _LL25C:
# 2379 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA85=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->tag=13U,({struct _fat_ptr*_tmpA84=({struct _fat_ptr*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));({struct _fat_ptr _tmpA83=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CE=_tmpA83;});_tmp5CE;});_tmp5CF->f1=_tmpA84;}),({struct Cyc_Absyn_Stmt*_tmpA82=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5CF->f2=_tmpA82;});_tmp5CF;});Cyc_Absyn_new_stmt(_tmpA85,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 305U: _LL25D: _LL25E:
# 2383 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 306U: _LL25F: _LL260:
# 2384 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA86=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA86,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 307U: _LL261: _LL262:
# 2389 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 308U: _LL263: _LL264:
# 2390 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 309U: _LL265: _LL266:
# 2395 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA87=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA87,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2396 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA88=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA88,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 311U: _LL269: _LL26A:
# 2397 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA8D=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->tag=13U,({struct _fat_ptr*_tmpA8C=({struct _fat_ptr*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));({struct _fat_ptr _tmpA8B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D0=_tmpA8B;});_tmp5D0;});_tmp5D1->f1=_tmpA8C;}),({struct Cyc_Absyn_Stmt*_tmpA8A=({struct Cyc_List_List*_tmpA89=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA89,Cyc_Absyn_skip_stmt(0U));});_tmp5D1->f2=_tmpA8A;});_tmp5D1;});Cyc_Absyn_new_stmt(_tmpA8D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2399
goto _LL0;case 312U: _LL26B: _LL26C:
# 2399 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA92=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->tag=13U,({struct _fat_ptr*_tmpA91=({struct _fat_ptr*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));({struct _fat_ptr _tmpA90=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D2=_tmpA90;});_tmp5D2;});_tmp5D3->f1=_tmpA91;}),({struct Cyc_Absyn_Stmt*_tmpA8F=({struct Cyc_List_List*_tmpA8E=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA8E,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5D3->f2=_tmpA8F;});_tmp5D3;});Cyc_Absyn_new_stmt(_tmpA92,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2401
goto _LL0;case 313U: _LL26D: _LL26E:
# 2401 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 314U: _LL26F: _LL270:
# 2402 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA94=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA93=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA94,_tmpA93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 315U: _LL271: _LL272:
# 2403 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA97=({void*_tmpA96=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA95=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D4->f1=_tmpA95;});_tmp5D4;});Cyc_Absyn_new_decl(_tmpA96,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA97,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 316U: _LL273: _LL274:
# 2406 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA9A=({void*_tmpA99=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA98=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D5->f1=_tmpA98;});_tmp5D5;});Cyc_Absyn_new_decl(_tmpA99,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA9A,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 317U: _LL275: _LL276:
# 2411 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA9C=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA9B=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA9D,_tmpA9C,_tmpA9B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318U: _LL277: _LL278:
# 2413 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA9F=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA9E=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpAA0,_tmpA9F,_tmpA9E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A:
# 2419 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpAA1=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpAA2,_tmpAA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320U: _LL27B: _LL27C: {
# 2422
struct Cyc_Absyn_Exp*_tmp5D6=({struct _tuple0*_tmpAA3=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpAA3,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D6;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA5=e;struct Cyc_List_List*_tmpAA4=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpAA5,_tmpAA4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2426
struct Cyc_Absyn_Exp*_tmp5D7=({struct Cyc_List_List*_tmpAA6=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpAA6,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D7;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA8=e;struct Cyc_List_List*_tmpAA7=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpAA8,_tmpAA7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2429
goto _LL0;}case 322U: _LL27F: _LL280:
# 2432 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAAA=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpAA9=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpAAA,_tmpAA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 323U: _LL281: _LL282:
# 2446 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 324U: _LL283: _LL284:
# 2449 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct Cyc_Absyn_Switch_clause*_tmpAAE=({struct Cyc_Absyn_Switch_clause*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct Cyc_Absyn_Pat*_tmpAAD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5D8->pattern=_tmpAAD;}),_tmp5D8->pat_vars=0,_tmp5D8->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAAC=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D8->body=_tmpAAC;}),_tmp5D8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D8;});
# 2449
_tmp5D9->hd=_tmpAAE;}),({
# 2451
struct Cyc_List_List*_tmpAAB=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D9->tl=_tmpAAB;});_tmp5D9;}));
goto _LL0;case 325U: _LL285: _LL286:
# 2453 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct Cyc_Absyn_Switch_clause*_tmpAB2=({struct Cyc_Absyn_Switch_clause*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct Cyc_Absyn_Pat*_tmpAB1=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DA->pattern=_tmpAB1;}),_tmp5DA->pat_vars=0,_tmp5DA->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAB0=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5DA->body=_tmpAB0;}),_tmp5DA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DA;});
# 2453
_tmp5DB->hd=_tmpAB2;}),({
# 2455
struct Cyc_List_List*_tmpAAF=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5DB->tl=_tmpAAF;});_tmp5DB;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2457 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct Cyc_Absyn_Switch_clause*_tmpAB6=({struct Cyc_Absyn_Switch_clause*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct Cyc_Absyn_Pat*_tmpAB5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DC->pattern=_tmpAB5;}),_tmp5DC->pat_vars=0,_tmp5DC->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAB4=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5DC->body=_tmpAB4;}),_tmp5DC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DC;});_tmp5DD->hd=_tmpAB6;}),({struct Cyc_List_List*_tmpAB3=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5DD->tl=_tmpAB3;});_tmp5DD;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2459 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));({struct Cyc_Absyn_Switch_clause*_tmpABB=({struct Cyc_Absyn_Switch_clause*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));({struct Cyc_Absyn_Pat*_tmpABA=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DE->pattern=_tmpABA;}),_tmp5DE->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAB9=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DE->where_clause=_tmpAB9;}),({
struct Cyc_Absyn_Stmt*_tmpAB8=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5DE->body=_tmpAB8;}),_tmp5DE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DE;});
# 2459
_tmp5DF->hd=_tmpABB;}),({
# 2461
struct Cyc_List_List*_tmpAB7=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5DF->tl=_tmpAB7;});_tmp5DF;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2463 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));({struct Cyc_Absyn_Switch_clause*_tmpAC0=({struct Cyc_Absyn_Switch_clause*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));({struct Cyc_Absyn_Pat*_tmpABF=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E0->pattern=_tmpABF;}),_tmp5E0->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5E0->where_clause=_tmpABE;}),({struct Cyc_Absyn_Stmt*_tmpABD=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5E0->body=_tmpABD;}),_tmp5E0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E0;});_tmp5E1->hd=_tmpAC0;}),({struct Cyc_List_List*_tmpABC=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5E1->tl=_tmpABC;});_tmp5E1;}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2470 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAC1=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpAC2,_tmpAC1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2474 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAC4=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAC3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpAC4,_tmpAC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 331U: _LL291: _LL292:
# 2478 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC7=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC5=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2478
Cyc_Absyn_for_stmt(_tmpAC8,_tmpAC7,_tmpAC6,_tmpAC5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2480
goto _LL0;case 332U: _LL293: _LL294:
# 2481 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpACB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACA=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAC9=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2481
Cyc_Absyn_for_stmt(_tmpACC,_tmpACB,_tmpACA,_tmpAC9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2483
goto _LL0;case 333U: _LL295: _LL296:
# 2484 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpACF=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpACE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpACD=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2484
Cyc_Absyn_for_stmt(_tmpAD0,_tmpACF,_tmpACE,_tmpACD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2486
goto _LL0;case 334U: _LL297: _LL298:
# 2487 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAD2=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAD1=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2487
Cyc_Absyn_for_stmt(_tmpAD4,_tmpAD3,_tmpAD2,_tmpAD1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2489
goto _LL0;case 335U: _LL299: _LL29A:
# 2490 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD7=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAD6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD5=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2490
Cyc_Absyn_for_stmt(_tmpAD8,_tmpAD7,_tmpAD6,_tmpAD5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2492
goto _LL0;case 336U: _LL29B: _LL29C:
# 2493 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpADC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpADB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADA=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAD9=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2493
Cyc_Absyn_for_stmt(_tmpADC,_tmpADB,_tmpADA,_tmpAD9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2495
goto _LL0;case 337U: _LL29D: _LL29E:
# 2496 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpADF=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADD=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2496
Cyc_Absyn_for_stmt(_tmpAE0,_tmpADF,_tmpADE,_tmpADD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2498
goto _LL0;case 338U: _LL29F: _LL2A0:
# 2499 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAE3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAE2=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAE1=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2499
Cyc_Absyn_for_stmt(_tmpAE4,_tmpAE3,_tmpAE2,_tmpAE1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2501
goto _LL0;case 339U: _LL2A1: _LL2A2: {
# 2502 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E2=({struct Cyc_Absyn_Exp*_tmpAE8=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE7=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAE5=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2502
Cyc_Absyn_for_stmt(_tmpAE8,_tmpAE7,_tmpAE6,_tmpAE5,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E2;
# 2504
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAE9=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAE9,s);}));
# 2506
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2507 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E3=({struct Cyc_Absyn_Exp*_tmpAED=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAEC=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAEB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAEA=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAED,_tmpAEC,_tmpAEB,_tmpAEA,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E3;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAEE=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAEE,s);}));
# 2510
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2511 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E4=({struct Cyc_Absyn_Exp*_tmpAF2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAF1=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAF0=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAEF=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAF2,_tmpAF1,_tmpAF0,_tmpAEF,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E4;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF3=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAF3,s);}));
# 2514
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2515 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E5=({struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAF6=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAF5=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAF4=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpAF7,_tmpAF6,_tmpAF5,_tmpAF4,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E5;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF8=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAF8,s);}));
# 2518
goto _LL0;}case 343U: _LL2A9: _LL2AA:
# 2521 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpAFA=({struct _fat_ptr*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));({struct _fat_ptr _tmpAF9=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5E6=_tmpAF9;});_tmp5E6;});Cyc_Absyn_goto_stmt(_tmpAFA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2522 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2523 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2524 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2525 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpAFB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2527 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2528 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2530 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAFC=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpAFC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2539 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2542
 yyval=(yyyvsp[0]).v;
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2544 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAFD=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAFF,_tmpAFE,_tmpAFD,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2547
 yyval=(yyyvsp[0]).v;
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2549 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB00=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB01,_tmpB00,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2552
 yyval=(yyyvsp[0]).v;
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2554 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB02=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpB03,_tmpB02,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2557
 yyval=(yyyvsp[0]).v;
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2559 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB05,_tmpB04,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2562
 yyval=(yyyvsp[0]).v;
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2564 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB06=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB07,_tmpB06,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2567
 yyval=(yyyvsp[0]).v;
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2569 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB08=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB09,_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2572
 yyval=(yyyvsp[0]).v;
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2574 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpB0B,_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2576 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpB0D,_tmpB0C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2579
 yyval=(yyyvsp[0]).v;
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2581 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2583 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB11,_tmpB10,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2585 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB12=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB13,_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2587 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB15,_tmpB14,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2590
 yyval=(yyyvsp[0]).v;
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2592 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB17,_tmpB16,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2594 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB19,_tmpB18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2597
 yyval=(yyyvsp[0]).v;
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2599 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB1B,_tmpB1A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2601 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB1D,_tmpB1C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2604
 yyval=(yyyvsp[0]).v;
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2606 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB1F,_tmpB1E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2608 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB21=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB20=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB21,_tmpB20,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2610 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB23=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB22=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB23,_tmpB22,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 382U: _LL2F7: _LL2F8:
# 2613
 yyval=(yyyvsp[0]).v;
goto _LL0;case 383U: _LL2F9: _LL2FA: {
# 2615 "parse.y"
void*_tmp5E7=({struct _tuple8*_tmpB24=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB24,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5E7;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB26=t;struct Cyc_Absyn_Exp*_tmpB25=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB26,_tmpB25,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2618
goto _LL0;}case 384U: _LL2FB: _LL2FC:
# 2621 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2624
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 386U: _LL2FF: _LL300:
# 2626 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB28=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB27=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB28,_tmpB27,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 387U: _LL301: _LL302: {
# 2628 "parse.y"
void*_tmp5E8=({struct _tuple8*_tmpB29=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB29,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5E8;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2631
goto _LL0;}case 388U: _LL303: _LL304:
# 2632 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB2A=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB2A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 389U: _LL305: _LL306:
# 2634 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB2C=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB2C,_tmpB2B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 390U: _LL307: _LL308:
# 2639 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 391U: _LL309: _LL30A:
# 2647 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 392U: _LL30B: _LL30C:
# 2652 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 393U: _LL30D: _LL30E:
# 2654 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 394U: _LL30F: _LL310: {
# 2656 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5E9=e->r;void*_stmttmp28=_tmp5E9;void*_tmp5EA=_stmttmp28;int _tmp5EC;struct _fat_ptr _tmp5EB;int _tmp5EE;enum Cyc_Absyn_Sign _tmp5ED;short _tmp5F0;enum Cyc_Absyn_Sign _tmp5EF;char _tmp5F2;enum Cyc_Absyn_Sign _tmp5F1;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).LongLong_c).tag){case 2U: _LL49D: _tmp5F1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Char_c).val).f1;_tmp5F2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Char_c).val).f2;_LL49E: {enum Cyc_Absyn_Sign s=_tmp5F1;char i=_tmp5F2;
# 2660
yyval=Cyc_YY9(({void*_tmpB2D=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->tag=11U,_tmp5F3->f1=i;_tmp5F3;});Cyc_Absyn_new_pat(_tmpB2D,e->loc);}));goto _LL49C;}case 4U: _LL49F: _tmp5EF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Short_c).val).f1;_tmp5F0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Short_c).val).f2;_LL4A0: {enum Cyc_Absyn_Sign s=_tmp5EF;short i=_tmp5F0;
# 2662
yyval=Cyc_YY9(({void*_tmpB2E=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->tag=10U,_tmp5F4->f1=s,_tmp5F4->f2=(int)i;_tmp5F4;});Cyc_Absyn_new_pat(_tmpB2E,e->loc);}));goto _LL49C;}case 5U: _LL4A1: _tmp5ED=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Int_c).val).f1;_tmp5EE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Int_c).val).f2;_LL4A2: {enum Cyc_Absyn_Sign s=_tmp5ED;int i=_tmp5EE;
# 2664
yyval=Cyc_YY9(({void*_tmpB2F=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=10U,_tmp5F5->f1=s,_tmp5F5->f2=i;_tmp5F5;});Cyc_Absyn_new_pat(_tmpB2F,e->loc);}));goto _LL49C;}case 7U: _LL4A3: _tmp5EB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Float_c).val).f1;_tmp5EC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EA)->f1).Float_c).val).f2;_LL4A4: {struct _fat_ptr s=_tmp5EB;int i=_tmp5EC;
# 2666
yyval=Cyc_YY9(({void*_tmpB30=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=12U,_tmp5F6->f1=s,_tmp5F6->f2=i;_tmp5F6;});Cyc_Absyn_new_pat(_tmpB30,e->loc);}));goto _LL49C;}case 1U: _LL4A5: _LL4A6:
# 2668
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL49C;case 8U: _LL4A7: _LL4A8:
 goto _LL4AA;case 9U: _LL4A9: _LL4AA:
# 2671
({void*_tmp5F7=0U;({unsigned _tmpB32=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB31=({const char*_tmp5F8="strings cannot occur within patterns";_tag_fat(_tmp5F8,sizeof(char),37U);});Cyc_Warn_err(_tmpB32,_tmpB31,_tag_fat(_tmp5F7,sizeof(void*),0U));});});goto _LL49C;case 6U: _LL4AB: _LL4AC:
# 2673
({void*_tmp5F9=0U;({unsigned _tmpB34=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB33=({const char*_tmp5FA="long long's in patterns not yet implemented";_tag_fat(_tmp5FA,sizeof(char),44U);});Cyc_Warn_err(_tmpB34,_tmpB33,_tag_fat(_tmp5F9,sizeof(void*),0U));});});goto _LL49C;default: goto _LL4AD;}else{_LL4AD: _LL4AE:
# 2675
({void*_tmp5FB=0U;({unsigned _tmpB36=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB35=({const char*_tmp5FC="bad constant in case";_tag_fat(_tmp5FC,sizeof(char),21U);});Cyc_Warn_err(_tmpB36,_tmpB35,_tag_fat(_tmp5FB,sizeof(void*),0U));});});}_LL49C:;}
# 2678
goto _LL0;}case 395U: _LL311: _LL312:
# 2679 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB38=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->tag=15U,({struct _tuple0*_tmpB37=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp5FD->f1=_tmpB37;});_tmp5FD;});Cyc_Absyn_new_pat(_tmpB38,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 396U: _LL313: _LL314:
# 2681 "parse.y"
 if(({struct _fat_ptr _tmpB39=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB39,({const char*_tmp5FE="as";_tag_fat(_tmp5FE,sizeof(char),3U);}));})!= 0)
({void*_tmp5FF=0U;({unsigned _tmpB3B=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB3A=({const char*_tmp600="expecting `as'";_tag_fat(_tmp600,sizeof(char),15U);});Cyc_Warn_err(_tmpB3B,_tmpB3A,_tag_fat(_tmp5FF,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB42=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB41=({unsigned _tmpB40=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB3F=({struct _tuple0*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB3E=({struct _fat_ptr*_tmp601=_cycalloc(sizeof(*_tmp601));({struct _fat_ptr _tmpB3D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp601=_tmpB3D;});_tmp601;});_tmp602->f2=_tmpB3E;});_tmp602;});Cyc_Absyn_new_vardecl(_tmpB40,_tmpB3F,Cyc_Absyn_void_type,0);});_tmp603->f1=_tmpB41;}),({
struct Cyc_Absyn_Pat*_tmpB3C=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp603->f2=_tmpB3C;});_tmp603;});
# 2683
Cyc_Absyn_new_pat(_tmpB42,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2686
goto _LL0;case 397U: _LL315: _LL316:
# 2687 "parse.y"
 if(({struct _fat_ptr _tmpB43=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB43,({const char*_tmp604="alias";_tag_fat(_tmp604,sizeof(char),6U);}));})!= 0)
({void*_tmp605=0U;({unsigned _tmpB45=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB44=({const char*_tmp606="expecting `alias'";_tag_fat(_tmp606,sizeof(char),18U);});Cyc_Warn_err(_tmpB45,_tmpB44,_tag_fat(_tmp605,sizeof(void*),0U));});});{
int _tmp607=((yyyvsp[0]).l).first_line;int location=_tmp607;
({struct _fat_ptr _tmpB46=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB46,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct _fat_ptr*_tmpB49=({struct _fat_ptr*_tmp60B=_cycalloc(sizeof(*_tmp60B));({struct _fat_ptr _tmpB48=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60B=_tmpB48;});_tmp60B;});_tmp60D->name=_tmpB49;}),_tmp60D->identity=- 1,({void*_tmpB47=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=0U,_tmp60C->f1=& Cyc_Kinds_rk;_tmp60C;});_tmp60D->kind=_tmpB47;});_tmp60D;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB4E=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB4D=({struct _tuple0*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB4B=({struct _fat_ptr*_tmp609=_cycalloc(sizeof(*_tmp609));({struct _fat_ptr _tmpB4A=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp609=_tmpB4A;});_tmp609;});_tmp60A->f2=_tmpB4B;});_tmp60A;});Cyc_Absyn_new_vardecl(_tmpB4E,_tmpB4D,({
struct _tuple8*_tmpB4C=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB4C,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB4F=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->tag=2U,_tmp608->f1=tv,_tmp608->f2=vd;_tmp608;});Cyc_Absyn_new_pat(_tmpB4F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2696
goto _LL0;}}case 398U: _LL317: _LL318:
# 2697 "parse.y"
 if(({struct _fat_ptr _tmpB50=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB50,({const char*_tmp60E="alias";_tag_fat(_tmp60E,sizeof(char),6U);}));})!= 0)
({void*_tmp60F=0U;({unsigned _tmpB52=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB51=({const char*_tmp610="expecting `alias'";_tag_fat(_tmp610,sizeof(char),18U);});Cyc_Warn_err(_tmpB52,_tmpB51,_tag_fat(_tmp60F,sizeof(void*),0U));});});{
int _tmp611=((yyyvsp[0]).l).first_line;int location=_tmp611;
({struct _fat_ptr _tmpB53=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB53,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp617=_cycalloc(sizeof(*_tmp617));({struct _fat_ptr*_tmpB56=({struct _fat_ptr*_tmp615=_cycalloc(sizeof(*_tmp615));({struct _fat_ptr _tmpB55=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp615=_tmpB55;});_tmp615;});_tmp617->name=_tmpB56;}),_tmp617->identity=- 1,({void*_tmpB54=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=0U,_tmp616->f1=& Cyc_Kinds_rk;_tmp616;});_tmp617->kind=_tmpB54;});_tmp617;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB5B=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB5A=({struct _tuple0*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB58=({struct _fat_ptr*_tmp613=_cycalloc(sizeof(*_tmp613));({struct _fat_ptr _tmpB57=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp613=_tmpB57;});_tmp613;});_tmp614->f2=_tmpB58;});_tmp614;});Cyc_Absyn_new_vardecl(_tmpB5B,_tmpB5A,({
struct _tuple8*_tmpB59=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB59,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB5C=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->tag=2U,_tmp612->f1=tv,_tmp612->f2=vd;_tmp612;});Cyc_Absyn_new_pat(_tmpB5C,(unsigned)location);}));
# 2706
goto _LL0;}}case 399U: _LL319: _LL31A: {
# 2707 "parse.y"
struct _tuple23 _tmp618=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp29=_tmp618;struct _tuple23 _tmp619=_stmttmp29;int _tmp61B;struct Cyc_List_List*_tmp61A;_LL4B0: _tmp61A=_tmp619.f1;_tmp61B=_tmp619.f2;_LL4B1: {struct Cyc_List_List*ps=_tmp61A;int dots=_tmp61B;
yyval=Cyc_YY9(({void*_tmpB5D=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tag=5U,_tmp61C->f1=ps,_tmp61C->f2=dots;_tmp61C;});Cyc_Absyn_new_pat(_tmpB5D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2710
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2711 "parse.y"
struct _tuple23 _tmp61D=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp61D;struct _tuple23 _tmp61E=_stmttmp2A;int _tmp620;struct Cyc_List_List*_tmp61F;_LL4B3: _tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_LL4B4: {struct Cyc_List_List*ps=_tmp61F;int dots=_tmp620;
yyval=Cyc_YY9(({void*_tmpB5F=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->tag=16U,({struct _tuple0*_tmpB5E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp621->f1=_tmpB5E;}),_tmp621->f2=ps,_tmp621->f3=dots;_tmp621;});Cyc_Absyn_new_pat(_tmpB5F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2714
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2715 "parse.y"
struct _tuple23 _tmp622=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2B=_tmp622;struct _tuple23 _tmp623=_stmttmp2B;int _tmp625;struct Cyc_List_List*_tmp624;_LL4B6: _tmp624=_tmp623.f1;_tmp625=_tmp623.f2;_LL4B7: {struct Cyc_List_List*fps=_tmp624;int dots=_tmp625;
struct Cyc_List_List*_tmp626=({unsigned _tmpB60=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB60,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp626;
yyval=Cyc_YY9(({void*_tmpB63=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB62=({union Cyc_Absyn_AggrInfo*_tmp627=_cycalloc(sizeof(*_tmp627));({union Cyc_Absyn_AggrInfo _tmpB61=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp627=_tmpB61;});_tmp627;});_tmp628->f1=_tmpB62;}),_tmp628->f2=exist_ts,_tmp628->f3=fps,_tmp628->f4=dots;_tmp628;});Cyc_Absyn_new_pat(_tmpB63,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2720
goto _LL0;}}case 402U: _LL31F: _LL320: {
# 2721 "parse.y"
struct _tuple23 _tmp629=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp629;struct _tuple23 _tmp62A=_stmttmp2C;int _tmp62C;struct Cyc_List_List*_tmp62B;_LL4B9: _tmp62B=_tmp62A.f1;_tmp62C=_tmp62A.f2;_LL4BA: {struct Cyc_List_List*fps=_tmp62B;int dots=_tmp62C;
struct Cyc_List_List*_tmp62D=({unsigned _tmpB64=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB64,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp62D;
yyval=Cyc_YY9(({void*_tmpB65=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->tag=7U,_tmp62E->f1=0,_tmp62E->f2=exist_ts,_tmp62E->f3=fps,_tmp62E->f4=dots;_tmp62E;});Cyc_Absyn_new_pat(_tmpB65,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2725
goto _LL0;}}case 403U: _LL321: _LL322:
# 2726 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB67=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=6U,({struct Cyc_Absyn_Pat*_tmpB66=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp62F->f1=_tmpB66;});_tmp62F;});Cyc_Absyn_new_pat(_tmpB67,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404U: _LL323: _LL324:
# 2728 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB6B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=6U,({struct Cyc_Absyn_Pat*_tmpB6A=({void*_tmpB69=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->tag=6U,({struct Cyc_Absyn_Pat*_tmpB68=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp630->f1=_tmpB68;});_tmp630;});Cyc_Absyn_new_pat(_tmpB69,(unsigned)((yyyvsp[0]).l).first_line);});_tmp631->f1=_tmpB6A;});_tmp631;});Cyc_Absyn_new_pat(_tmpB6B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 405U: _LL325: _LL326:
# 2730 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB72=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB71=({unsigned _tmpB70=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB6F=({struct _tuple0*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB6E=({struct _fat_ptr*_tmp632=_cycalloc(sizeof(*_tmp632));({struct _fat_ptr _tmpB6D=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp632=_tmpB6D;});_tmp632;});_tmp633->f2=_tmpB6E;});_tmp633;});Cyc_Absyn_new_vardecl(_tmpB70,_tmpB6F,Cyc_Absyn_void_type,0);});_tmp634->f1=_tmpB71;}),({
# 2732
struct Cyc_Absyn_Pat*_tmpB6C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp634->f2=_tmpB6C;});_tmp634;});
# 2730
Cyc_Absyn_new_pat(_tmpB72,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2734
goto _LL0;case 406U: _LL327: _LL328:
# 2735 "parse.y"
 if(({struct _fat_ptr _tmpB73=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB73,({const char*_tmp635="as";_tag_fat(_tmp635,sizeof(char),3U);}));})!= 0)
({void*_tmp636=0U;({unsigned _tmpB75=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB74=({const char*_tmp637="expecting `as'";_tag_fat(_tmp637,sizeof(char),15U);});Cyc_Warn_err(_tmpB75,_tmpB74,_tag_fat(_tmp636,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB7C=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB7B=({unsigned _tmpB7A=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB79=({struct _tuple0*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB78=({struct _fat_ptr*_tmp638=_cycalloc(sizeof(*_tmp638));({struct _fat_ptr _tmpB77=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp638=_tmpB77;});_tmp638;});_tmp639->f2=_tmpB78;});_tmp639;});Cyc_Absyn_new_vardecl(_tmpB7A,_tmpB79,Cyc_Absyn_void_type,0);});_tmp63A->f1=_tmpB7B;}),({
# 2739
struct Cyc_Absyn_Pat*_tmpB76=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp63A->f2=_tmpB76;});_tmp63A;});
# 2737
Cyc_Absyn_new_pat(_tmpB7C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2741
goto _LL0;case 407U: _LL329: _LL32A: {
# 2742 "parse.y"
void*_tmp63B=({struct _fat_ptr _tmpB7D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB7D,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp63B;
yyval=Cyc_YY9(({void*_tmpB84=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB83=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp63E->f1=_tmpB83;}),({
struct Cyc_Absyn_Vardecl*_tmpB82=({unsigned _tmpB81=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB80=({struct _tuple0*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB7F=({struct _fat_ptr*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct _fat_ptr _tmpB7E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp63C=_tmpB7E;});_tmp63C;});_tmp63D->f2=_tmpB7F;});_tmp63D;});Cyc_Absyn_new_vardecl(_tmpB81,_tmpB80,
Cyc_Absyn_tag_type(tag),0);});
# 2744
_tmp63E->f2=_tmpB82;});_tmp63E;});
# 2743
Cyc_Absyn_new_pat(_tmpB84,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2747
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2748 "parse.y"
struct Cyc_Absyn_Tvar*_tmp63F=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp63F;
yyval=Cyc_YY9(({void*_tmpB8A=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->tag=4U,_tmp642->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB89=({unsigned _tmpB88=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB87=({struct _tuple0*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB86=({struct _fat_ptr*_tmp640=_cycalloc(sizeof(*_tmp640));({struct _fat_ptr _tmpB85=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp640=_tmpB85;});_tmp640;});_tmp641->f2=_tmpB86;});_tmp641;});Cyc_Absyn_new_vardecl(_tmpB88,_tmpB87,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2750
_tmp642->f2=_tmpB89;});_tmp642;});
# 2749
Cyc_Absyn_new_pat(_tmpB8A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2753
goto _LL0;}case 409U: _LL32D: _LL32E:
# 2756 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp643=_cycalloc(sizeof(*_tmp643));({struct Cyc_List_List*_tmpB8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp643->f1=_tmpB8B;}),_tmp643->f2=0;_tmp643;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2757 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_List_List*_tmpB8C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp644->f1=_tmpB8C;}),_tmp644->f2=1;_tmp644;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2758 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=0,_tmp645->f2=1;_tmp645;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2763 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));({struct Cyc_Absyn_Pat*_tmpB8D=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp646->hd=_tmpB8D;}),_tmp646->tl=0;_tmp646;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2765 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp647=_cycalloc(sizeof(*_tmp647));({struct Cyc_Absyn_Pat*_tmpB8F=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp647->hd=_tmpB8F;}),({struct Cyc_List_List*_tmpB8E=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp647->tl=_tmpB8E;});_tmp647;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2770 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=0,({struct Cyc_Absyn_Pat*_tmpB90=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp648->f2=_tmpB90;});_tmp648;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2772 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp649=_cycalloc(sizeof(*_tmp649));({struct Cyc_List_List*_tmpB92=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp649->f1=_tmpB92;}),({struct Cyc_Absyn_Pat*_tmpB91=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp649->f2=_tmpB91;});_tmp649;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2775
 yyval=Cyc_YY14(({struct _tuple23*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_List_List*_tmpB93=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64A->f1=_tmpB93;}),_tmp64A->f2=0;_tmp64A;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2776 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp64B=_cycalloc(sizeof(*_tmp64B));({struct Cyc_List_List*_tmpB94=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64B->f1=_tmpB94;}),_tmp64B->f2=1;_tmp64B;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2777 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=0,_tmp64C->f2=1;_tmp64C;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2782 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct _tuple24*_tmpB95=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp64D->hd=_tmpB95;}),_tmp64D->tl=0;_tmp64D;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2784 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct _tuple24*_tmpB97=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp64E->hd=_tmpB97;}),({struct Cyc_List_List*_tmpB96=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp64E->tl=_tmpB96;});_tmp64E;}));
goto _LL0;case 421U: _LL345: _LL346:
# 2790 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 422U: _LL347: _LL348:
# 2792 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB98=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB99,_tmpB98,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 423U: _LL349: _LL34A:
# 2797 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 424U: _LL34B: _LL34C:
# 2799 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB9B=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpB9C,_tmpB9B,_tmpB9A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2801 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpB9E,_tmpB9D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 426U: _LL34F: _LL350:
# 2805 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 427U: _LL351: _LL352:
# 2806 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->v=(void*)Cyc_Absyn_Times;_tmp64F;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2807 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->v=(void*)Cyc_Absyn_Div;_tmp650;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2808 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->v=(void*)Cyc_Absyn_Mod;_tmp651;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2809 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->v=(void*)Cyc_Absyn_Plus;_tmp652;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2810 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->v=(void*)Cyc_Absyn_Minus;_tmp653;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2811 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)Cyc_Absyn_Bitlshift;_tmp654;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2812 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Absyn_Bitlrshift;_tmp655;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2813 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->v=(void*)Cyc_Absyn_Bitand;_tmp656;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2814 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->v=(void*)Cyc_Absyn_Bitxor;_tmp657;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2815 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->v=(void*)Cyc_Absyn_Bitor;_tmp658;}));
goto _LL0;case 437U: _LL365: _LL366:
# 2820 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 438U: _LL367: _LL368:
# 2822 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpBA1,_tmpBA0,_tmpB9F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 439U: _LL369: _LL36A:
# 2825
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpBA2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2828
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpBA3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2830 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpBA4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2832 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBA6,_tmpBA5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2834 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBA8,_tmpBA7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 444U: _LL373: _LL374:
# 2838 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2842 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446U: _LL377: _LL378:
# 2844 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpBAA,_tmpBA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 447U: _LL379: _LL37A:
# 2848 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 448U: _LL37B: _LL37C:
# 2850 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpBAC,_tmpBAB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 449U: _LL37D: _LL37E:
# 2854 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 450U: _LL37F: _LL380:
# 2856 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpBAE,_tmpBAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 451U: _LL381: _LL382:
# 2860 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 452U: _LL383: _LL384:
# 2862 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBB0,_tmpBAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 453U: _LL385: _LL386:
# 2866 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 454U: _LL387: _LL388:
# 2868 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBB2,_tmpBB1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 455U: _LL389: _LL38A:
# 2872 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 456U: _LL38B: _LL38C:
# 2874 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBB4,_tmpBB3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 457U: _LL38D: _LL38E:
# 2876 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBB6,_tmpBB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 458U: _LL38F: _LL390:
# 2880 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 459U: _LL391: _LL392:
# 2882 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBB8,_tmpBB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 460U: _LL393: _LL394:
# 2884 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBBA,_tmpBB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2886 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBBC,_tmpBBB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2888 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBBE,_tmpBBD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463U: _LL399: _LL39A:
# 2892 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 464U: _LL39B: _LL39C:
# 2894 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBC0,_tmpBBF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2896 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBC2,_tmpBC1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2900 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2902 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBC4,_tmpBC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 2904 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBC6,_tmpBC5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 2908 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 2910 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBC8,_tmpBC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 2912 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBCA,_tmpBC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 2914 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBCC,_tmpBCB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 473U: _LL3AD: _LL3AE:
# 2918 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 474U: _LL3AF: _LL3B0: {
# 2920 "parse.y"
void*_tmp659=({struct _tuple8*_tmpBCD=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBCD,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp659;
yyval=Cyc_Exp_tok(({void*_tmpBCF=t;struct Cyc_Absyn_Exp*_tmpBCE=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBCF,_tmpBCE,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2923
goto _LL0;}case 475U: _LL3B1: _LL3B2:
# 2926 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 476U: _LL3B3: _LL3B4:
# 2927 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBD0,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 2928 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD1=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBD1,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 2929 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBD2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 2930 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBD3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 2931 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBD4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE:
# 2932 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBD6=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBD6,_tmpBD5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 482U: _LL3BF: _LL3C0: {
# 2934 "parse.y"
void*_tmp65A=({struct _tuple8*_tmpBD7=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBD7,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65A;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2937
goto _LL0;}case 483U: _LL3C1: _LL3C2:
# 2937 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBD8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 484U: _LL3C3: _LL3C4: {
# 2939 "parse.y"
void*_tmp65B=({struct _tuple8*_tmpBD9=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBD9,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65B;
yyval=Cyc_Exp_tok(({void*_tmpBDB=t;struct Cyc_List_List*_tmpBDA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBDB,_tmpBDA,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2942
goto _LL0;}case 485U: _LL3C5: _LL3C6:
# 2944
 yyval=Cyc_Exp_tok(({void*_tmpBDD=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tag=34U,(_tmp65C->f1).is_calloc=0,(_tmp65C->f1).rgn=0,(_tmp65C->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBDC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65C->f1).num_elts=_tmpBDC;}),(_tmp65C->f1).fat_result=0,(_tmp65C->f1).inline_call=0;_tmp65C;});Cyc_Absyn_new_exp(_tmpBDD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2946
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 2947 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->tag=34U,(_tmp65D->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBDF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65D->f1).rgn=_tmpBDF;}),(_tmp65D->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBDE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp65D->f1).num_elts=_tmpBDE;}),(_tmp65D->f1).fat_result=0,(_tmp65D->f1).inline_call=0;_tmp65D;});Cyc_Absyn_new_exp(_tmpBE0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2949
goto _LL0;case 487U: _LL3C9: _LL3CA:
# 2950 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE3=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->tag=34U,(_tmp65E->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBE2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65E->f1).rgn=_tmpBE2;}),(_tmp65E->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBE1=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp65E->f1).num_elts=_tmpBE1;}),(_tmp65E->f1).fat_result=0,(_tmp65E->f1).inline_call=1;_tmp65E;});Cyc_Absyn_new_exp(_tmpBE3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2952
goto _LL0;case 488U: _LL3CB: _LL3CC: {
# 2953 "parse.y"
void*_tmp65F=({struct _tuple8*_tmpBE4=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBE4,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp65F;
yyval=Cyc_Exp_tok(({void*_tmpBE7=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=34U,(_tmp661->f1).is_calloc=1,(_tmp661->f1).rgn=0,({void**_tmpBE6=({void**_tmp660=_cycalloc(sizeof(*_tmp660));*_tmp660=t;_tmp660;});(_tmp661->f1).elt_type=_tmpBE6;}),({struct Cyc_Absyn_Exp*_tmpBE5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp661->f1).num_elts=_tmpBE5;}),(_tmp661->f1).fat_result=0,(_tmp661->f1).inline_call=0;_tmp661;});Cyc_Absyn_new_exp(_tmpBE7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2956
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 2958
void*_tmp662=({struct _tuple8*_tmpBE8=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBE8,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp662;
yyval=Cyc_Exp_tok(({void*_tmpBEC=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=34U,(_tmp664->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBEB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp664->f1).rgn=_tmpBEB;}),({void**_tmpBEA=({void**_tmp663=_cycalloc(sizeof(*_tmp663));*_tmp663=t;_tmp663;});(_tmp664->f1).elt_type=_tmpBEA;}),({struct Cyc_Absyn_Exp*_tmpBE9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp664->f1).num_elts=_tmpBE9;}),(_tmp664->f1).fat_result=0,(_tmp664->f1).inline_call=0;_tmp664;});Cyc_Absyn_new_exp(_tmpBEC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2961
goto _LL0;}case 490U: _LL3CF: _LL3D0:
# 2962 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBEE=({struct Cyc_Absyn_Exp*_tmp665[1U];({struct Cyc_Absyn_Exp*_tmpBED=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp665[0]=_tmpBED;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp665,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBEE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 2964 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF2=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=38U,({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp667->f1=_tmpBF1;}),({struct _fat_ptr*_tmpBF0=({struct _fat_ptr*_tmp666=_cycalloc(sizeof(*_tmp666));({struct _fat_ptr _tmpBEF=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp666=_tmpBEF;});_tmp666;});_tmp667->f2=_tmpBF0;});_tmp667;});Cyc_Absyn_new_exp(_tmpBF2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4:
# 2966 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF7=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=38U,({struct Cyc_Absyn_Exp*_tmpBF6=({struct Cyc_Absyn_Exp*_tmpBF5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpBF5,(unsigned)((yyyvsp[2]).l).first_line);});_tmp669->f1=_tmpBF6;}),({struct _fat_ptr*_tmpBF4=({struct _fat_ptr*_tmp668=_cycalloc(sizeof(*_tmp668));({struct _fat_ptr _tmpBF3=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp668=_tmpBF3;});_tmp668;});_tmp669->f2=_tmpBF4;});_tmp669;});Cyc_Absyn_new_exp(_tmpBF7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 493U: _LL3D5: _LL3D6: {
# 2968 "parse.y"
void*_tmp66A=({struct _tuple8*_tmpBF8=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBF8,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp66A;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 494U: _LL3D7: _LL3D8:
# 2971 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF9=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpBF9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA:
# 2972 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBFA=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpBFA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC:
# 2974 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBFB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpBFB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 497U: _LL3DD: _LL3DE: {
# 2979 "parse.y"
struct _tuple29*_tmp66B=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp2D=_tmp66B;struct _tuple29*_tmp66C=_stmttmp2D;struct Cyc_List_List*_tmp66F;struct Cyc_List_List*_tmp66E;struct Cyc_List_List*_tmp66D;_LL4BC: _tmp66D=_tmp66C->f1;_tmp66E=_tmp66C->f2;_tmp66F=_tmp66C->f3;_LL4BD: {struct Cyc_List_List*outlist=_tmp66D;struct Cyc_List_List*inlist=_tmp66E;struct Cyc_List_List*clobbers=_tmp66F;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->tag=40U,({int _tmpBFD=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp670->f1=_tmpBFD;}),({struct _fat_ptr _tmpBFC=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp670->f2=_tmpBFC;}),_tmp670->f3=outlist,_tmp670->f4=inlist,_tmp670->f5=clobbers;_tmp670;}));
goto _LL0;}}case 498U: _LL3DF: _LL3E0:
# 2984 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 499U: _LL3E1: _LL3E2:
# 2985 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 500U: _LL3E3: _LL3E4:
# 2989 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->f1=0,_tmp671->f2=0,_tmp671->f3=0;_tmp671;}));
goto _LL0;case 501U: _LL3E5: _LL3E6: {
# 2991 "parse.y"
struct _tuple30*_tmp672=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp2E=_tmp672;struct _tuple30*_tmp673=_stmttmp2E;struct Cyc_List_List*_tmp675;struct Cyc_List_List*_tmp674;_LL4BF: _tmp674=_tmp673->f1;_tmp675=_tmp673->f2;_LL4C0: {struct Cyc_List_List*inlist=_tmp674;struct Cyc_List_List*clobbers=_tmp675;
yyval=Cyc_YY59(({struct _tuple29*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->f1=0,_tmp676->f2=inlist,_tmp676->f3=clobbers;_tmp676;}));
goto _LL0;}}case 502U: _LL3E7: _LL3E8: {
# 2994 "parse.y"
struct _tuple30*_tmp677=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp2F=_tmp677;struct _tuple30*_tmp678=_stmttmp2F;struct Cyc_List_List*_tmp67A;struct Cyc_List_List*_tmp679;_LL4C2: _tmp679=_tmp678->f1;_tmp67A=_tmp678->f2;_LL4C3: {struct Cyc_List_List*inlist=_tmp679;struct Cyc_List_List*clobbers=_tmp67A;
yyval=Cyc_YY59(({struct _tuple29*_tmp67B=_cycalloc(sizeof(*_tmp67B));({struct Cyc_List_List*_tmpBFE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp67B->f1=_tmpBFE;}),_tmp67B->f2=inlist,_tmp67B->f3=clobbers;_tmp67B;}));
goto _LL0;}}case 503U: _LL3E9: _LL3EA:
# 2999 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*_tmp67C));({struct _tuple31*_tmpBFF=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp67C->hd=_tmpBFF;}),_tmp67C->tl=0;_tmp67C;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 3000 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));({struct _tuple31*_tmpC01=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp67D->hd=_tmpC01;}),({struct Cyc_List_List*_tmpC00=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp67D->tl=_tmpC00;});_tmp67D;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 3004 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->f1=0,_tmp67E->f2=0;_tmp67E;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 3006 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->f1=0,({struct Cyc_List_List*_tmpC02=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp67F->f2=_tmpC02;});_tmp67F;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 3008 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp680=_cycalloc(sizeof(*_tmp680));({struct Cyc_List_List*_tmpC04=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp680->f1=_tmpC04;}),({struct Cyc_List_List*_tmpC03=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp680->f2=_tmpC03;});_tmp680;}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 3012 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));({struct _tuple31*_tmpC05=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp681->hd=_tmpC05;}),_tmp681->tl=0;_tmp681;}));
goto _LL0;case 509U: _LL3F5: _LL3F6:
# 3013 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp682=_cycalloc(sizeof(*_tmp682));({struct _tuple31*_tmpC07=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp682->hd=_tmpC07;}),({struct Cyc_List_List*_tmpC06=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp682->tl=_tmpC06;});_tmp682;}));
goto _LL0;case 510U: _LL3F7: _LL3F8: {
# 3018 "parse.y"
struct Cyc_Absyn_Exp*_tmp683=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp683;
yyval=Cyc_YY63(({struct _tuple31*_tmp684=_cycalloc(sizeof(*_tmp684));({struct _fat_ptr _tmpC09=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp684->f1=_tmpC09;}),({struct Cyc_Absyn_Exp*_tmpC08=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp684->f2=_tmpC08;});_tmp684;}));
goto _LL0;}case 511U: _LL3F9: _LL3FA:
# 3024 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3025 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3026 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3030 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));({struct _fat_ptr*_tmpC0B=({struct _fat_ptr*_tmp685=_cycalloc(sizeof(*_tmp685));({struct _fat_ptr _tmpC0A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp685=_tmpC0A;});_tmp685;});_tmp686->hd=_tmpC0B;}),_tmp686->tl=0;_tmp686;}));
goto _LL0;case 515U: _LL401: _LL402:
# 3031 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp688=_cycalloc(sizeof(*_tmp688));({struct _fat_ptr*_tmpC0E=({struct _fat_ptr*_tmp687=_cycalloc(sizeof(*_tmp687));({struct _fat_ptr _tmpC0D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp687=_tmpC0D;});_tmp687;});_tmp688->hd=_tmpC0E;}),({struct Cyc_List_List*_tmpC0C=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp688->tl=_tmpC0C;});_tmp688;}));
goto _LL0;case 516U: _LL403: _LL404:
# 3035 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 517U: _LL405: _LL406:
# 3036 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 518U: _LL407: _LL408:
# 3037 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 519U: _LL409: _LL40A:
# 3042 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 520U: _LL40B: _LL40C:
# 3044 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC10=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpC0F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC10,_tmpC0F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3046 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC11=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC11,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3048 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC13=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC12=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC13,_tmpC12,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3050 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC16=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC15=({struct _fat_ptr*_tmp689=_cycalloc(sizeof(*_tmp689));({struct _fat_ptr _tmpC14=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp689=_tmpC14;});_tmp689;});Cyc_Absyn_aggrmember_exp(_tmpC16,_tmpC15,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3052 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC19=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC18=({struct _fat_ptr*_tmp68A=_cycalloc(sizeof(*_tmp68A));({struct _fat_ptr _tmpC17=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68A=_tmpC17;});_tmp68A;});Cyc_Absyn_aggrarrow_exp(_tmpC19,_tmpC18,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3054 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC1A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC1A,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3056 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC1B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC1B,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3058 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC1D=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B->tag=25U,({struct _tuple8*_tmpC1C=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68B->f1=_tmpC1C;}),_tmp68B->f2=0;_tmp68B;});Cyc_Absyn_new_exp(_tmpC1D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3060 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC20=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->tag=25U,({struct _tuple8*_tmpC1F=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68C->f1=_tmpC1F;}),({struct Cyc_List_List*_tmpC1E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp68C->f2=_tmpC1E;});_tmp68C;});Cyc_Absyn_new_exp(_tmpC20,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3062 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC23=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->tag=25U,({struct _tuple8*_tmpC22=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68D->f1=_tmpC22;}),({struct Cyc_List_List*_tmpC21=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp68D->f2=_tmpC21;});_tmp68D;});Cyc_Absyn_new_exp(_tmpC23,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3067 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp690=_cycalloc(sizeof(*_tmp690));({void*_tmpC26=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=0U,({struct _fat_ptr*_tmpC25=({struct _fat_ptr*_tmp68E=_cycalloc(sizeof(*_tmp68E));({struct _fat_ptr _tmpC24=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp68E=_tmpC24;});_tmp68E;});_tmp68F->f1=_tmpC25;});_tmp68F;});_tmp690->hd=_tmpC26;}),_tmp690->tl=0;_tmp690;}));
goto _LL0;case 531U: _LL421: _LL422:
# 3070
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp692=_cycalloc(sizeof(*_tmp692));({void*_tmpC29=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->tag=1U,({unsigned _tmpC28=({unsigned _tmpC27=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC27,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp691->f1=_tmpC28;});_tmp691;});_tmp692->hd=_tmpC29;}),_tmp692->tl=0;_tmp692;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3072 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));({void*_tmpC2D=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->tag=0U,({struct _fat_ptr*_tmpC2C=({struct _fat_ptr*_tmp693=_cycalloc(sizeof(*_tmp693));({struct _fat_ptr _tmpC2B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp693=_tmpC2B;});_tmp693;});_tmp694->f1=_tmpC2C;});_tmp694;});_tmp695->hd=_tmpC2D;}),({struct Cyc_List_List*_tmpC2A=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp695->tl=_tmpC2A;});_tmp695;}));
goto _LL0;case 533U: _LL425: _LL426:
# 3075
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp697=_cycalloc(sizeof(*_tmp697));({void*_tmpC31=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->tag=1U,({unsigned _tmpC30=({unsigned _tmpC2F=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC2F,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp696->f1=_tmpC30;});_tmp696;});_tmp697->hd=_tmpC31;}),({struct Cyc_List_List*_tmpC2E=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp697->tl=_tmpC2E;});_tmp697;}));
goto _LL0;case 534U: _LL427: _LL428:
# 3081 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC32=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC32,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535U: _LL429: _LL42A:
# 3083 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC33=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC33,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536U: _LL42B: _LL42C:
# 3085 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 537U: _LL42D: _LL42E:
# 3087 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC34=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC34,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 538U: _LL42F: _LL430:
# 3089 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC35=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC35,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 539U: _LL431: _LL432:
# 3091 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 540U: _LL433: _LL434:
# 3096 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC36=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC36,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 541U: _LL435: _LL436:
# 3098 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC38=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC37=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC38,_tmpC37,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3101
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC39=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC39,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3104
 yyval=Cyc_Exp_tok(({void*_tmpC3D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tag=29U,({struct _tuple0*_tmpC3C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp698->f1=_tmpC3C;}),({struct Cyc_List_List*_tmpC3B=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp698->f2=_tmpC3B;}),({struct Cyc_List_List*_tmpC3A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp698->f3=_tmpC3A;}),_tmp698->f4=0;_tmp698;});Cyc_Absyn_new_exp(_tmpC3D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3107
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC3E=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC3E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3111 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 546U: _LL43F: _LL440:
# 3117 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));({struct Cyc_Absyn_Exp*_tmpC3F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp699->hd=_tmpC3F;}),_tmp699->tl=0;_tmp699;}));
goto _LL0;case 547U: _LL441: _LL442:
# 3119 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));({struct Cyc_Absyn_Exp*_tmpC41=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp69A->hd=_tmpC41;}),({struct Cyc_List_List*_tmpC40=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp69A->tl=_tmpC40;});_tmp69A;}));
goto _LL0;case 548U: _LL443: _LL444:
# 3125 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC42=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC42,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 549U: _LL445: _LL446:
# 3126 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC43=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC43,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 550U: _LL447: _LL448:
# 3127 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC44=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC44,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 551U: _LL449: _LL44A: {
# 3129 "parse.y"
struct _fat_ptr _tmp69B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp69B;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3137
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3139
goto _LL0;}case 552U: _LL44B: _LL44C:
# 3140 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 553U: _LL44D: _LL44E:
# 3144 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp69D=_cycalloc(sizeof(*_tmp69D));({union Cyc_Absyn_Nmspace _tmpC47=Cyc_Absyn_Rel_n(0);_tmp69D->f1=_tmpC47;}),({struct _fat_ptr*_tmpC46=({struct _fat_ptr*_tmp69C=_cycalloc(sizeof(*_tmp69C));({struct _fat_ptr _tmpC45=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp69C=_tmpC45;});_tmp69C;});_tmp69D->f2=_tmpC46;});_tmp69D;}));
goto _LL0;case 554U: _LL44F: _LL450:
# 3145 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 555U: _LL451: _LL452:
# 3148
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp69F=_cycalloc(sizeof(*_tmp69F));({union Cyc_Absyn_Nmspace _tmpC4A=Cyc_Absyn_Rel_n(0);_tmp69F->f1=_tmpC4A;}),({struct _fat_ptr*_tmpC49=({struct _fat_ptr*_tmp69E=_cycalloc(sizeof(*_tmp69E));({struct _fat_ptr _tmpC48=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp69E=_tmpC48;});_tmp69E;});_tmp69F->f2=_tmpC49;});_tmp69F;}));
goto _LL0;case 556U: _LL453: _LL454:
# 3149 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 557U: _LL455: _LL456:
# 3154 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3155 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3158
 yyval=(yyyvsp[0]).v;
goto _LL0;case 560U: _LL45B: _LL45C:
# 3159 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 561U: _LL45D: _LL45E:
# 3164 "parse.y"
 goto _LL0;case 562U: _LL45F: _LL460:
# 3164 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL461: _LL462:
# 3168
 goto _LL0;}_LL0:;}
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 389 "cycbison.simple"
(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))).v=yyval;
# 392
if(yylen == 0){
struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset - 1),sizeof(struct Cyc_Yystacktype),2U));
((p[1]).l).first_line=yylloc.first_line;
((p[1]).l).first_column=yylloc.first_column;
((p[1]).l).last_line=((p[0]).l).last_line;
((p[1]).l).last_column=((p[0]).l).last_column;}else{
# 399
({int _tmpC4B=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC4B;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC4C=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 153));_tmpC4C + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 8192)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8193U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,162U,sizeof(short),yyn - 153));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus == 0){
# 424
++ yynerrs;
# 427
yyn=(int)Cyc_yypact[yystate];
# 429
if(yyn > - 32768 && yyn < 8192){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 315U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8193U,sizeof(short),x + yyn))== x)
({unsigned long _tmpC4D=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,315U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC4D;}),count ++;}
msg=({unsigned _tmp6A1=(unsigned)(sze + 15)+ 1U;char*_tmp6A0=({struct _RegionHandle*_tmpC4E=yyregion;_region_malloc(_tmpC4E,_check_times(_tmp6A1,sizeof(char)));});({{unsigned _tmp765=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp765;++ i){_tmp6A0[i]='\000';}_tmp6A0[_tmp765]=0;}0;});_tag_fat(_tmp6A0,sizeof(char),_tmp6A1);});
({struct _fat_ptr _tmpC4F=msg;Cyc_strcpy(_tmpC4F,({const char*_tmp6A2="parse error";_tag_fat(_tmp6A2,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 315U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8193U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC50=msg;Cyc_strcat(_tmpC50,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp6A3=", expecting `";_tag_fat(_tmp6A3,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp6A4=" or `";_tag_fat(_tmp6A4,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,315U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC51=msg;Cyc_strcat(_tmpC51,({const char*_tmp6A5="'";_tag_fat(_tmp6A5,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC53=({const char*_tmp6A6="parse error";_tag_fat(_tmp6A6,sizeof(char),12U);});int _tmpC52=yystate;Cyc_yyerror(_tmpC53,_tmpC52,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp6A7=1;_npop_handler(0U);return _tmp6A7;}
# 483
yychar=-2;}
# 489
yyerrstatus=3;
# 491
goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset == 0){int _tmp6A8=1;_npop_handler(0U);return _tmp6A8;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1142U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 8192)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,8193U,sizeof(short),yyn))!= 1)goto yyerrdefault;
# 528
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 531
if(yyn == -32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn == 0)goto yyerrpop;}
# 537
if(yyn == 1141){
int _tmp6A9=0;_npop_handler(0U);return _tmp6A9;}
# 546
({struct Cyc_Yystacktype _tmpC54=({struct Cyc_Yystacktype _tmp766;_tmp766.v=yylval,_tmp766.l=yylloc;_tmp766;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC54;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3167 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6AD=v;struct Cyc_Absyn_Stmt*_tmp6AE;struct Cyc_Absyn_Exp*_tmp6AF;struct _tuple0*_tmp6B0;struct _fat_ptr _tmp6B1;char _tmp6B2;union Cyc_Absyn_Cnst _tmp6B3;switch((_tmp6AD.Stmt_tok).tag){case 1U: _LL1: _tmp6B3=(_tmp6AD.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6B3;
({struct Cyc_String_pa_PrintArg_struct _tmp6B6=({struct Cyc_String_pa_PrintArg_struct _tmp769;_tmp769.tag=0U,({struct _fat_ptr _tmpC55=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp769.f1=_tmpC55;});_tmp769;});void*_tmp6B4[1U];_tmp6B4[0]=& _tmp6B6;({struct Cyc___cycFILE*_tmpC57=Cyc_stderr;struct _fat_ptr _tmpC56=({const char*_tmp6B5="%s";_tag_fat(_tmp6B5,sizeof(char),3U);});Cyc_fprintf(_tmpC57,_tmpC56,_tag_fat(_tmp6B4,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6B2=(_tmp6AD.Char_tok).val;_LL4: {char c=_tmp6B2;
({struct Cyc_Int_pa_PrintArg_struct _tmp6B9=({struct Cyc_Int_pa_PrintArg_struct _tmp76A;_tmp76A.tag=1U,_tmp76A.f1=(unsigned long)((int)c);_tmp76A;});void*_tmp6B7[1U];_tmp6B7[0]=& _tmp6B9;({struct Cyc___cycFILE*_tmpC59=Cyc_stderr;struct _fat_ptr _tmpC58=({const char*_tmp6B8="%c";_tag_fat(_tmp6B8,sizeof(char),3U);});Cyc_fprintf(_tmpC59,_tmpC58,_tag_fat(_tmp6B7,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6B1=(_tmp6AD.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6B1;
({struct Cyc_String_pa_PrintArg_struct _tmp6BC=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,_tmp76B.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp76B;});void*_tmp6BA[1U];_tmp6BA[0]=& _tmp6BC;({struct Cyc___cycFILE*_tmpC5B=Cyc_stderr;struct _fat_ptr _tmpC5A=({const char*_tmp6BB="\"%s\"";_tag_fat(_tmp6BB,sizeof(char),5U);});Cyc_fprintf(_tmpC5B,_tmpC5A,_tag_fat(_tmp6BA,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6B0=(_tmp6AD.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6B0;
({struct Cyc_String_pa_PrintArg_struct _tmp6BF=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,({struct _fat_ptr _tmpC5C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp76C.f1=_tmpC5C;});_tmp76C;});void*_tmp6BD[1U];_tmp6BD[0]=& _tmp6BF;({struct Cyc___cycFILE*_tmpC5E=Cyc_stderr;struct _fat_ptr _tmpC5D=({const char*_tmp6BE="%s";_tag_fat(_tmp6BE,sizeof(char),3U);});Cyc_fprintf(_tmpC5E,_tmpC5D,_tag_fat(_tmp6BD,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6AF=(_tmp6AD.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6AF;
({struct Cyc_String_pa_PrintArg_struct _tmp6C2=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,({struct _fat_ptr _tmpC5F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp76D.f1=_tmpC5F;});_tmp76D;});void*_tmp6C0[1U];_tmp6C0[0]=& _tmp6C2;({struct Cyc___cycFILE*_tmpC61=Cyc_stderr;struct _fat_ptr _tmpC60=({const char*_tmp6C1="%s";_tag_fat(_tmp6C1,sizeof(char),3U);});Cyc_fprintf(_tmpC61,_tmpC60,_tag_fat(_tmp6C0,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6AE=(_tmp6AD.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6AE;
({struct Cyc_String_pa_PrintArg_struct _tmp6C5=({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0U,({struct _fat_ptr _tmpC62=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp76E.f1=_tmpC62;});_tmp76E;});void*_tmp6C3[1U];_tmp6C3[0]=& _tmp6C5;({struct Cyc___cycFILE*_tmpC64=Cyc_stderr;struct _fat_ptr _tmpC63=({const char*_tmp6C4="%s";_tag_fat(_tmp6C4,sizeof(char),3U);});Cyc_fprintf(_tmpC64,_tmpC63,_tag_fat(_tmp6C3,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6C6=0U;({struct Cyc___cycFILE*_tmpC66=Cyc_stderr;struct _fat_ptr _tmpC65=({const char*_tmp6C7="?";_tag_fat(_tmp6C7,sizeof(char),2U);});Cyc_fprintf(_tmpC66,_tmpC65,_tag_fat(_tmp6C6,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3179
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6C8="end-of-file";_tag_fat(_tmp6C8,sizeof(char),12U);});
if(token == 368)
return Cyc_Lex_token_string;
if(token == 377)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 380?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,381U,sizeof(short),token)): 315;
if((unsigned)z < 315U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 3193
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6C9=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6C9;_push_region(yyr);
({struct _RegionHandle*_tmpC67=yyr;Cyc_yyparse(_tmpC67,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6CA=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6CA;}
# 3196
;_pop_region();}}
