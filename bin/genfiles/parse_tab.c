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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 292
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 305
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};
# 453
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 460
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
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 700 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 863
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 893
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 903
void*Cyc_Absyn_compress(void*);
# 917
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 919
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 922
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 924
extern void*Cyc_Absyn_sint_type;
# 926
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 929
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 933
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 935
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 960
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 964
void*Cyc_Absyn_bounds_one (void);
# 966
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 986
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 989
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1038
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
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1088
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1092
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_pool_decl(struct Cyc_Absyn_Tvar*tv,unsigned loc);
# 1098
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1100
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1107
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1114
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1117
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1122
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
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple11{struct _tuple11*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _tuple13*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 171
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple13*ds,struct _tuple11**decls,struct Cyc_List_List**es){
# 174
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple12 _tmp0=ds->hd;struct _tuple12 _stmttmp0=_tmp0;struct _tuple12 _tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp3;struct Cyc_Parse_Declarator _tmp2;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2: {struct Cyc_Parse_Declarator d=_tmp2;struct Cyc_Absyn_Exp*e=_tmp3;
declarators=({struct _tuple11*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->tl=declarators,_tmp4->hd=d;_tmp4;});
exprs=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=e,_tmp5->tl=exprs;_tmp5;});}}
# 181
({struct Cyc_List_List*_tmp773=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp773;});
({struct _tuple11*_tmp774=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp774;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 191
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 197
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 199
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 201
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=Cyc_Parse_Exit;_tmp6;}));}
# 205
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 207
struct _tuple8*_tmp7=tqt;void*_tmp9;struct Cyc_Absyn_Tqual _tmp8;_LL1: _tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp8;void*t=_tmp9;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc != (unsigned)0)loc=tq.loc;
({void*_tmpA=0U;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp776,_tmp775,_tag_fat(_tmpA,sizeof(void*),0U));});});}
# 212
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 215
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 221
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*released=Cyc_Tcutil_any_bool(0);
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_stmttmp1=_tmpC;void*_tmpD=_stmttmp1;void*_tmpE;struct Cyc_Absyn_Exp*_tmpF;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6U: _LL5: _LL6:
 released=Cyc_Absyn_true_type;goto _LL0;case 8U: _LL7: _LL8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7U: _LL9: _LLA:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3U: _LLB: _LLC:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2U: _LLD: _LLE:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0U: _LLF: _tmpF=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmpF;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _LL11: _tmpE=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL12: {void*t=_tmpE;
rgn=t;goto _LL0;}}_LL0:;}
# 235
return({struct _tuple15 _tmp6CF;_tmp6CF.f1=nullable,_tmp6CF.f2=bound,_tmp6CF.f3=zeroterm,_tmp6CF.f4=rgn,_tmp6CF.f5=released;_tmp6CF;});}
# 241
struct _tuple0*Cyc_Parse_gensym_enum (void){
# 243
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp77A=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp77A;}),({
struct _fat_ptr*_tmp779=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp778=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6D0;_tmp6D0.tag=1U,_tmp6D0.f1=(unsigned long)enum_counter ++;_tmp6D0;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp777=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp777,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp778;});_tmp13;});_tmp14->f2=_tmp779;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 248
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 253
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp77C=loc;struct _fat_ptr _tmp77B=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp77C,_tmp77B,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp77E=loc;struct _fat_ptr _tmp77D=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp77E,_tmp77D,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 263
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6D1;_tmp6D1.Signed_spec=0,_tmp6D1.Unsigned_spec=0,_tmp6D1.Short_spec=0,_tmp6D1.Long_spec=0,_tmp6D1.Long_Long_spec=0,_tmp6D1.Valid_type_spec=0,_tmp6D1.Type_spec=Cyc_Absyn_sint_type,_tmp6D1.loc=loc;_tmp6D1;});}
# 274
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp23;
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 280
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp24;
s.Signed_spec=1;
return s;}
# 285
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp25;
s.Unsigned_spec=1;
return s;}
# 290
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp26;
s.Short_spec=1;
return s;}
# 295
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp27;
s.Long_spec=1;
return s;}
# 302
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 304
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp77F=t;Cyc_Tcutil_promote_array(_tmp77F,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 317 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 322
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 326
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp781=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6D2;_tmp6D2.tag=0U,_tmp6D2.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6D2;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp780=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp780,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp781;});_tmp36;});
({void*_tmp783=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp782=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Kinds_ik;_tmp31;});_tmp32->kind=_tmp782;});_tmp32;}));*z=_tmp783;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 331
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp784=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp784;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 335
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp786=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6D3;_tmp6D3.tag=0U,_tmp6D3.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6D3;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp785=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp785,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp786;});_tmp3E;});
({void*_tmp788=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp787=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Kinds_rk;_tmp39;});_tmp3A->kind=_tmp787;});_tmp3A;}));*z=_tmp788;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 340
return res;}
# 344
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;struct Cyc_List_List*res=_tmp3F;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_stmttmp3=_tmp40;void*_tmp41=_stmttmp3;void*_tmp42;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp42=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2: {void*i=_tmp42;
# 349
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp789=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp789;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 352
return res;}
# 356
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp45=e->r;void*_stmttmp4=_tmp45;void*_tmp46=_stmttmp4;struct _fat_ptr*_tmp47;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp47=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f2;_LL2: {struct _fat_ptr*y=_tmp47;
# 359
{struct Cyc_List_List*_tmp48=tags;struct Cyc_List_List*ts=_tmp48;for(0;ts != 0;ts=ts->tl){
struct _tuple19*_tmp49=(struct _tuple19*)ts->hd;struct _tuple19*_stmttmp5=_tmp49;struct _tuple19*_tmp4A=_stmttmp5;void*_tmp4C;struct _fat_ptr*_tmp4B;_LL6: _tmp4B=_tmp4A->f1;_tmp4C=_tmp4A->f2;_LL7: {struct _fat_ptr*x=_tmp4B;void*i=_tmp4C;
if(Cyc_strptrcmp(x,y)== 0)
return({void*_tmp78B=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp78A=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp78A;});_tmp4D;});Cyc_Absyn_new_exp(_tmp78B,e->loc);});}}}
# 364
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 367
return e;}
# 372
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp4F;void*_tmp50;void*_tmp58;struct Cyc_Absyn_PtrLoc*_tmp57;void*_tmp56;void*_tmp55;void*_tmp54;void*_tmp53;struct Cyc_Absyn_Tqual _tmp52;void*_tmp51;unsigned _tmp5D;void*_tmp5C;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Tqual _tmp5A;void*_tmp59;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp5A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp5B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp5C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp5D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {void*et=_tmp59;struct Cyc_Absyn_Tqual tq=_tmp5A;struct Cyc_Absyn_Exp*nelts=_tmp5B;void*zt=_tmp5C;unsigned ztloc=_tmp5D;
# 375
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*_tmp5E=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5E;
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3U: _LL3: _tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp56=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp57=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_tmp58=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).released;_LL4: {void*et=_tmp51;struct Cyc_Absyn_Tqual tq=_tmp52;void*r=_tmp53;void*n=_tmp54;void*b=_tmp55;void*zt=_tmp56;struct Cyc_Absyn_PtrLoc*pl=_tmp57;void*rel=_tmp58;
# 383
void*_tmp5F=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5F;
void*_tmp60=Cyc_Parse_substitute_tags(tags,b);void*b2=_tmp60;
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D4;_tmp6D4.elt_type=et2,_tmp6D4.elt_tq=tq,(_tmp6D4.ptr_atts).rgn=r,(_tmp6D4.ptr_atts).nullable=n,(_tmp6D4.ptr_atts).bounds=b2,(_tmp6D4.ptr_atts).zero_term=zt,(_tmp6D4.ptr_atts).ptrloc=pl,(_tmp6D4.ptr_atts).released=rel;_tmp6D4;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp50=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {void*t=_tmp50;
# 389
void*_tmp61=Cyc_Parse_substitute_tags(tags,t);void*t2=_tmp61;
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp4F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp4F;
# 393
struct Cyc_Absyn_Exp*_tmp62=Cyc_Parse_substitute_tags_exp(tags,e);struct Cyc_Absyn_Exp*e2=_tmp62;
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 398
return t;}
# 403
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp78C=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp78C;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 410
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple8*t){
# 412
return({struct _tuple20*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=(*t).f2,_tmp63->f2=(*t).f3;_tmp63;});}
# 415
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp64=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp64)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 424
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp78D=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78D,({const char*_tmp65="`H";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp78E=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78E,({const char*_tmp66="`U";_tag_fat(_tmp66,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp78F=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78F,({const char*_tmp67="`RC";_tag_fat(_tmp67,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_poolrgn_name)== 0)
return Cyc_CurRgn_curr_poolrgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _fat_ptr*_tmp790=({struct _fat_ptr*_tmp68=_cycalloc(sizeof(*_tmp68));*_tmp68=s;_tmp68;});_tmp69->name=_tmp790;}),_tmp69->identity=- 1,_tmp69->kind=k;_tmp69;}));}
# 438
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp791=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp791,({const char*_tmp6A="`H";_tag_fat(_tmp6A,sizeof(char),3U);}));})== 0)
({void*_tmp6B=0U;({unsigned _tmp793=loc;struct _fat_ptr _tmp792=({const char*_tmp6C="bad occurrence of heap region";_tag_fat(_tmp6C,sizeof(char),30U);});Cyc_Warn_err(_tmp793,_tmp792,_tag_fat(_tmp6B,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp794=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp794,({const char*_tmp6D="`U";_tag_fat(_tmp6D,sizeof(char),3U);}));})== 0)
({void*_tmp6E=0U;({unsigned _tmp796=loc;struct _fat_ptr _tmp795=({const char*_tmp6F="bad occurrence of unique region";_tag_fat(_tmp6F,sizeof(char),32U);});Cyc_Warn_err(_tmp796,_tmp795,_tag_fat(_tmp6E,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp797=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp797,({const char*_tmp70="`RC";_tag_fat(_tmp70,sizeof(char),4U);}));})== 0)
({void*_tmp71=0U;({unsigned _tmp799=loc;struct _fat_ptr _tmp798=({const char*_tmp72="bad occurrence of refcounted region";_tag_fat(_tmp72,sizeof(char),36U);});Cyc_Warn_err(_tmp799,_tmp798,_tag_fat(_tmp71,sizeof(void*),0U));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp73=0U;({unsigned _tmp79B=loc;struct _fat_ptr _tmp79A=({const char*_tmp74="bad occurrence of \"current\" region";_tag_fat(_tmp74,sizeof(char),35U);});Cyc_Warn_err(_tmp79B,_tmp79A,_tag_fat(_tmp73,sizeof(void*),0U));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_poolrgn_name)== 0)
({void*_tmp75=0U;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp76="bad occurrence of \"current pool\" region";_tag_fat(_tmp76,sizeof(char),40U);});Cyc_Warn_err(_tmp79D,_tmp79C,_tag_fat(_tmp75,sizeof(void*),0U));});});}
# 455
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp77=t;struct Cyc_Absyn_Tvar*_tmp78;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77)->tag == 2U){_LL1: _tmp78=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp78;
return pr;}}else{_LL3: _LL4:
({void*_tmp79=0U;({unsigned _tmp79F=loc;struct _fat_ptr _tmp79E=({const char*_tmp7A="expecting a list of type variables, not types";_tag_fat(_tmp7A,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79F,_tmp79E,_tag_fat(_tmp79,sizeof(void*),0U));});});}_LL0:;}
# 463
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp7B=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp7B;void*_tmp7C=_stmttmp6;void**_tmp7D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7C)->tag == 2U){_LL1: _tmp7D=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7C)->f1)->kind;_LL2: {void**cptr=_tmp7D;
# 466
void*_tmp7E=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp7=_tmp7E;void*_tmp7F=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7F)->tag == 1U){_LL6: _LL7:
# 468
({void*_tmp7A0=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=2U,_tmp80->f1=0,_tmp80->f2=k;_tmp80;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp7A0;});
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 472
 return;}_LL0:;}
# 477
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 485
if(tms == 0)return 0;{
# 487
void*_tmp81=(void*)tms->hd;void*_stmttmp8=_tmp81;void*_tmp82=_stmttmp8;void*_tmp83;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp82)->tag == 3U){_LL1: _tmp83=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp82)->f1;_LL2: {void*args=_tmp83;
# 490
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 493
void*_tmp84=args;struct Cyc_List_List*_tmp85;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp84)->tag == 1U){_LL6: _LL7:
# 495
({void*_tmp86=0U;({unsigned _tmp7A2=loc;struct _fat_ptr _tmp7A1=({const char*_tmp87="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp87,sizeof(char),93U);});Cyc_Warn_warn(_tmp7A2,_tmp7A1,_tag_fat(_tmp86,sizeof(void*),0U));});});
# 497
return tms;}else{_LL8: _tmp85=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp84)->f1;_LL9: {struct Cyc_List_List*ids=_tmp85;
# 499
if(({int _tmp7A3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp7A3 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp88=0U;({unsigned _tmp7A5=loc;struct _fat_ptr _tmp7A4=({const char*_tmp89="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp89,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A5,_tmp7A4,_tag_fat(_tmp88,sizeof(void*),0U));});});{
# 503
struct Cyc_List_List*rev_new_params=0;
for(0;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(0;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp8A=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp8A;
void*_tmp8B=x->r;void*_stmttmp9=_tmp8B;void*_tmp8C=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp8D;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8C)->tag == 0U){_LLB: _tmp8D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8C)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp8D;
# 510
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({void*_tmp8E=0U;({unsigned _tmp7A7=x->loc;struct _fat_ptr _tmp7A6=({const char*_tmp8F="initializer found in parameter declaration";_tag_fat(_tmp8F,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A7,_tmp7A6,_tag_fat(_tmp8E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp90=0U;({unsigned _tmp7A9=x->loc;struct _fat_ptr _tmp7A8=({const char*_tmp91="namespaces forbidden in parameter declarations";_tag_fat(_tmp91,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A9,_tmp7A8,_tag_fat(_tmp90,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp93=_cycalloc(sizeof(*_tmp93));({struct _tuple8*_tmp7AA=({struct _tuple8*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->f1=(*vd->name).f2,_tmp92->f2=vd->tq,_tmp92->f3=vd->type;_tmp92;});_tmp93->hd=_tmp7AA;}),_tmp93->tl=rev_new_params;_tmp93;});
# 518
goto L;}}else{_LLD: _LLE:
({void*_tmp94=0U;({unsigned _tmp7AC=x->loc;struct _fat_ptr _tmp7AB=({const char*_tmp95="nonvariable declaration in parameter type";_tag_fat(_tmp95,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AC,_tmp7AB,_tag_fat(_tmp94,sizeof(void*),0U));});});}_LLA:;}
# 522
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp98=({struct Cyc_String_pa_PrintArg_struct _tmp6D5;_tmp6D5.tag=0U,_tmp6D5.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6D5;});void*_tmp96[1U];_tmp96[0]=& _tmp98;({unsigned _tmp7AE=loc;struct _fat_ptr _tmp7AD=({const char*_tmp97="%s is not given a type";_tag_fat(_tmp97,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AE,_tmp7AD,_tag_fat(_tmp96,sizeof(void*),1U));});});}
# 525
return({struct Cyc_List_List*_tmp9B=_region_malloc(yy,sizeof(*_tmp9B));
({void*_tmp7B1=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp9A=_region_malloc(yy,sizeof(*_tmp9A));_tmp9A->tag=3U,({void*_tmp7B0=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp99=_region_malloc(yy,sizeof(*_tmp99));_tmp99->tag=1U,({struct Cyc_List_List*_tmp7AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp99->f1=_tmp7AF;}),_tmp99->f2=0,_tmp99->f3=0,_tmp99->f4=0,_tmp99->f5=0,_tmp99->f6=0,_tmp99->f7=0;_tmp99;});_tmp9A->f1=_tmp7B0;});_tmp9A;});_tmp9B->hd=_tmp7B1;}),_tmp9B->tl=0;_tmp9B;});}}}_LL5:;}
# 532
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp9C=_region_malloc(yy,sizeof(*_tmp9C));_tmp9C->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7B2=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp9C->tl=_tmp7B2;});_tmp9C;});}_LL0:;}}
# 540
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 544
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6D6;_tmp6D6.id=d.id,_tmp6D6.varloc=d.varloc,({struct Cyc_List_List*_tmp7B3=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6D6.tms=_tmp7B3;});_tmp6D6;});{
# 547
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 553
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 559
enum Cyc_Parse_Storage_class _tmp9D=dso->sc;enum Cyc_Parse_Storage_class _stmttmpA=_tmp9D;enum Cyc_Parse_Storage_class _tmp9E=_stmttmpA;switch(_tmp9E){case Cyc_Parse_None_sc: _LL1: _LL2:
 goto _LL0;case Cyc_Parse_Extern_sc: _LL3: _LL4:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL5: _LL6:
 sc=0U;goto _LL0;default: _LL7: _LL8:
({void*_tmp9F=0U;({unsigned _tmp7B5=loc;struct _fat_ptr _tmp7B4=({const char*_tmpA0="bad storage class on function";_tag_fat(_tmpA0,sizeof(char),30U);});Cyc_Warn_err(_tmp7B5,_tmp7B4,_tag_fat(_tmp9F,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 566
void*_tmpA1=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmpA1;
struct _tuple14 _tmpA2=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmpA2;struct _tuple14 _tmpA3=_stmttmpB;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;_LLA: _tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;_tmpA6=_tmpA3.f3;_tmpA7=_tmpA3.f4;_LLB: {struct Cyc_Absyn_Tqual fn_tqual=_tmpA4;void*fn_type=_tmpA5;struct Cyc_List_List*x=_tmpA6;struct Cyc_List_List*out_atts=_tmpA7;
# 571
if(x != 0)
({void*_tmpA8=0U;({unsigned _tmp7B7=loc;struct _fat_ptr _tmp7B6=({const char*_tmpA9="bad type params, ignoring";_tag_fat(_tmpA9,sizeof(char),26U);});Cyc_Warn_warn(_tmp7B7,_tmp7B6,_tag_fat(_tmpA8,sizeof(void*),0U));});});{
# 574
void*_tmpAA=fn_type;struct Cyc_Absyn_FnInfo _tmpAB;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAA)->tag == 5U){_LLD: _tmpAB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAA)->f1;_LLE: {struct Cyc_Absyn_FnInfo i=_tmpAB;
# 576
{struct Cyc_List_List*_tmpAC=i.args;struct Cyc_List_List*args2=_tmpAC;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpAD=0U;({unsigned _tmp7B9=loc;struct _fat_ptr _tmp7B8=({const char*_tmpAE="missing argument variable in function prototype";_tag_fat(_tmpAE,sizeof(char),48U);});Cyc_Warn_err(_tmp7B9,_tmp7B8,_tag_fat(_tmpAD,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7BB=({struct _fat_ptr*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct _fat_ptr _tmp7BA=({const char*_tmpAF="?";_tag_fat(_tmpAF,sizeof(char),2U);});*_tmpB0=_tmp7BA;});_tmpB0;});(*((struct _tuple8*)args2->hd)).f1=_tmp7BB;});}}}
# 583
({struct Cyc_List_List*_tmp7BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7BC;});
return({struct Cyc_Absyn_Fndecl*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->sc=sc,_tmpB1->is_inline=is_inline,_tmpB1->name=d.id,_tmpB1->body=body,_tmpB1->i=i,_tmpB1->cached_type=0,_tmpB1->param_vardecls=0,_tmpB1->fn_vardecl=0,_tmpB1->orig_scope=sc;_tmpB1;});}}else{_LLF: _LL10:
# 587
({void*_tmpB2=0U;({unsigned _tmp7BE=loc;struct _fat_ptr _tmp7BD=({const char*_tmpB3="declarator is not a function prototype";_tag_fat(_tmpB3,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7BE,_tmp7BD,_tag_fat(_tmpB2,sizeof(void*),0U));});});}_LLC:;}}}}}static char _tmpB4[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 591
static struct _fat_ptr Cyc_Parse_msg1={_tmpB4,_tmpB4,_tmpB4 + 76U};static char _tmpB5[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 593
static struct _fat_ptr Cyc_Parse_msg2={_tmpB5,_tmpB5,_tmpB5 + 84U};
# 600
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 603
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpB6=0U;({unsigned _tmp7C0=loc;struct _fat_ptr _tmp7BF=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7C0,_tmp7BF,_tag_fat(_tmpB6,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpB7=0U;({unsigned _tmp7C2=loc;struct _fat_ptr _tmp7C1=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7C2,_tmp7C1,_tag_fat(_tmpB7,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpB8=0U;({unsigned _tmp7C4=loc;struct _fat_ptr _tmp7C3=({const char*_tmpB9="too many occurrences of short in specifiers";_tag_fat(_tmpB9,sizeof(char),44U);});Cyc_Warn_warn(_tmp7C4,_tmp7C3,_tag_fat(_tmpB8,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpBA=0U;({unsigned _tmp7C6=loc;struct _fat_ptr _tmp7C5=({const char*_tmpBB="too many occurrences of long in specifiers";_tag_fat(_tmpBB,sizeof(char),43U);});Cyc_Warn_warn(_tmp7C6,_tmp7C5,_tag_fat(_tmpBA,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpBC=0U;({unsigned _tmp7C8=loc;struct _fat_ptr _tmp7C7=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7C8,_tmp7C7,_tag_fat(_tmpBC,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 625
return s1;}
# 631
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 639
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpBD=0U;({unsigned _tmp7CA=loc;struct _fat_ptr _tmp7C9=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7CA,_tmp7C9,_tag_fat(_tmpBD,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpBE=0U;({unsigned _tmp7CC=loc;struct _fat_ptr _tmp7CB=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7CC,_tmp7CB,_tag_fat(_tmpBE,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 652
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpBF=0U;({unsigned _tmp7CE=loc;struct _fat_ptr _tmp7CD=({const char*_tmpC0="missing type within specifier";_tag_fat(_tmpC0,sizeof(char),30U);});Cyc_Warn_warn(_tmp7CE,_tmp7CD,_tag_fat(_tmpBF,sizeof(void*),0U));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 657
{void*_tmpC1=t;enum Cyc_Absyn_Size_of _tmpC3;enum Cyc_Absyn_Sign _tmpC2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC1)->f1)){case 1U: _LL1: _tmpC2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC1)->f1)->f1;_tmpC3=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC1)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpC2;enum Cyc_Absyn_Size_of sz2=_tmpC3;
# 659
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 663
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2U: _LL3: _LL4:
# 667
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
# 671
 if(seen_sign)
({void*_tmpC4=0U;({unsigned _tmp7D0=loc;struct _fat_ptr _tmp7CF=({const char*_tmpC5="sign specification on non-integral type";_tag_fat(_tmpC5,sizeof(char),40U);});Cyc_Warn_err(_tmp7D0,_tmp7CF,_tag_fat(_tmpC4,sizeof(void*),0U));});});
if(seen_size)
({void*_tmpC6=0U;({unsigned _tmp7D2=loc;struct _fat_ptr _tmp7D1=({const char*_tmpC7="size qualifier on non-integral type";_tag_fat(_tmpC7,sizeof(char),36U);});Cyc_Warn_err(_tmp7D2,_tmp7D1,_tag_fat(_tmpC6,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 677
return t;}
# 680
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 684
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpC8=d.id;struct _tuple0*q=_tmpC8;
unsigned _tmpC9=d.varloc;unsigned varloc=_tmpC9;
struct _tuple14 _tmpCA=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpCA;struct _tuple14 _tmpCB=_stmttmpC;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct Cyc_Absyn_Tqual _tmpCC;_LL1: _tmpCC=_tmpCB.f1;_tmpCD=_tmpCB.f2;_tmpCE=_tmpCB.f3;_tmpCF=_tmpCB.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpCC;void*new_typ=_tmpCD;struct Cyc_List_List*tvs=_tmpCE;struct Cyc_List_List*atts=_tmpCF;
# 691
struct Cyc_List_List*_tmpD0=ds->tl == 0?0:({struct _RegionHandle*_tmp7D6=r;struct Cyc_Absyn_Tqual _tmp7D5=tq;void*_tmp7D4=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7D3=ds->tl;Cyc_Parse_apply_tmss(_tmp7D6,_tmp7D5,_tmp7D4,_tmp7D3,shared_atts);});struct Cyc_List_List*tl=_tmpD0;
return({struct Cyc_List_List*_tmpD2=_region_malloc(r,sizeof(*_tmpD2));({struct _tuple16*_tmp7D7=({struct _tuple16*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));_tmpD1->f1=varloc,_tmpD1->f2=q,_tmpD1->f3=tq2,_tmpD1->f4=new_typ,_tmpD1->f5=tvs,_tmpD1->f6=atts;_tmpD1;});_tmpD2->hd=_tmp7D7;}),_tmpD2->tl=tl;_tmpD2;});}}}
# 695
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 698
if(tms == 0)return({struct _tuple14 _tmp6D7;_tmp6D7.f1=tq,_tmp6D7.f2=t,_tmp6D7.f3=0,_tmp6D7.f4=atts;_tmp6D7;});{
void*_tmpD3=(void*)tms->hd;void*_stmttmpD=_tmpD3;void*_tmpD4=_stmttmpD;struct Cyc_List_List*_tmpD6;unsigned _tmpD5;struct Cyc_Absyn_Tqual _tmpD8;struct Cyc_Absyn_PtrAtts _tmpD7;unsigned _tmpDA;struct Cyc_List_List*_tmpD9;void*_tmpDB;unsigned _tmpDE;void*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;unsigned _tmpE0;void*_tmpDF;switch(*((int*)_tmpD4)){case 0U: _LL1: _tmpDF=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1;_tmpE0=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD4)->f2;_LL2: {void*zeroterm=_tmpDF;unsigned ztloc=_tmpE0;
# 701
return({struct Cyc_Absyn_Tqual _tmp7DA=Cyc_Absyn_empty_tqual(0U);void*_tmp7D9=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 701
struct Cyc_List_List*_tmp7D8=atts;Cyc_Parse_apply_tms(_tmp7DA,_tmp7D9,_tmp7D8,tms->tl);});}case 1U: _LL3: _tmpDC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1;_tmpDD=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD4)->f2;_tmpDE=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD4)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpDC;void*zeroterm=_tmpDD;unsigned ztloc=_tmpDE;
# 704
return({struct Cyc_Absyn_Tqual _tmp7DD=Cyc_Absyn_empty_tqual(0U);void*_tmp7DC=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 704
struct Cyc_List_List*_tmp7DB=atts;Cyc_Parse_apply_tms(_tmp7DD,_tmp7DC,_tmp7DB,tms->tl);});}case 3U: _LL5: _tmpDB=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1;_LL6: {void*args=_tmpDB;
# 707
void*_tmpE1=args;unsigned _tmpE2;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_List_List*_tmpE7;void*_tmpE6;struct Cyc_Absyn_VarargInfo*_tmpE5;int _tmpE4;struct Cyc_List_List*_tmpE3;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->tag == 1U){_LLE: _tmpE3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f1;_tmpE4=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f2;_tmpE5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f3;_tmpE6=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f4;_tmpE7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f5;_tmpE8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f6;_tmpE9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE1)->f7;_LLF: {struct Cyc_List_List*args2=_tmpE3;int c_vararg=_tmpE4;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpE5;void*eff=_tmpE6;struct Cyc_List_List*rgn_po=_tmpE7;struct Cyc_Absyn_Exp*req=_tmpE8;struct Cyc_Absyn_Exp*ens=_tmpE9;
# 709
struct Cyc_List_List*typvars=0;
# 711
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)as->hd,_tmpEA->tl=fn_atts;_tmpEA;});else{
# 716
new_atts=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(void*)as->hd,_tmpEB->tl=new_atts;_tmpEB;});}}}
# 718
if(tms->tl != 0){
void*_tmpEC=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpEC;void*_tmpED=_stmttmpE;struct Cyc_List_List*_tmpEE;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpED)->tag == 4U){_LL13: _tmpEE=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpED)->f1;_LL14: {struct Cyc_List_List*ts=_tmpEE;
# 721
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;}
# 727
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 732
args2=0;{
# 735
struct Cyc_List_List*_tmpEF=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpEF;
# 737
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 742
{struct Cyc_List_List*_tmpF0=args2;struct Cyc_List_List*a=_tmpF0;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpF1=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpF1;struct _tuple8*_tmpF2=_stmttmpF;void**_tmpF5;struct Cyc_Absyn_Tqual _tmpF4;struct _fat_ptr*_tmpF3;_LL18: _tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;_tmpF5=(void**)& _tmpF2->f3;_LL19: {struct _fat_ptr*vopt=_tmpF3;struct Cyc_Absyn_Tqual tq=_tmpF4;void**t=_tmpF5;
if(tags != 0)
({void*_tmp7DE=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7DE;});
({void*_tmp7DF=Cyc_Parse_array2ptr(*t,1);*t=_tmp7DF;});}}}
# 754
return({struct Cyc_Absyn_Tqual _tmp7E2=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7E1=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 754
struct Cyc_List_List*_tmp7E0=new_atts;Cyc_Parse_apply_tms(_tmp7E2,_tmp7E1,_tmp7E0,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpE2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpE1)->f2;_LL11: {unsigned loc=_tmpE2;
# 761
({void*_tmpF6=0U;({unsigned _tmp7E4=loc;struct _fat_ptr _tmp7E3=({const char*_tmpF7="function declaration without parameter types";_tag_fat(_tmpF7,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7E4,_tmp7E3,_tag_fat(_tmpF6,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1;_tmpDA=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD4)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD9;unsigned loc=_tmpDA;
# 768
if(tms->tl == 0)
return({struct _tuple14 _tmp6D8;_tmp6D8.f1=tq,_tmp6D8.f2=t,_tmp6D8.f3=ts,_tmp6D8.f4=atts;_tmp6D8;});
# 773
({void*_tmpF8=0U;({unsigned _tmp7E6=loc;struct _fat_ptr _tmp7E5=({const char*_tmpF9="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF9,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7E6,_tmp7E5,_tag_fat(_tmpF8,sizeof(void*),0U));});});}case 2U: _LL9: _tmpD7=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1;_tmpD8=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD4)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpD7;struct Cyc_Absyn_Tqual tq2=_tmpD8;
# 776
return({struct Cyc_Absyn_Tqual _tmp7E9=tq2;void*_tmp7E8=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D9;_tmp6D9.elt_type=t,_tmp6D9.elt_tq=tq,_tmp6D9.ptr_atts=ptratts;_tmp6D9;}));struct Cyc_List_List*_tmp7E7=atts;Cyc_Parse_apply_tms(_tmp7E9,_tmp7E8,_tmp7E7,tms->tl);});}default: _LLB: _tmpD5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD4)->f1;_tmpD6=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD4)->f2;_LLC: {unsigned loc=_tmpD5;struct Cyc_List_List*atts2=_tmpD6;
# 781
return({struct Cyc_Absyn_Tqual _tmp7EC=tq;void*_tmp7EB=t;struct Cyc_List_List*_tmp7EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7EC,_tmp7EB,_tmp7EA,tms->tl);});}}_LL0:;}}
# 787
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 796
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpFA=t;struct Cyc_List_List*_tmp100;struct Cyc_List_List*_tmpFF;void*_tmpFE;struct Cyc_Absyn_Tqual _tmpFD;struct _tuple0*_tmpFC;unsigned _tmpFB;_LL1: _tmpFB=_tmpFA->f1;_tmpFC=_tmpFA->f2;_tmpFD=_tmpFA->f3;_tmpFE=_tmpFA->f4;_tmpFF=_tmpFA->f5;_tmp100=_tmpFA->f6;_LL2: {unsigned varloc=_tmpFB;struct _tuple0*x=_tmpFC;struct Cyc_Absyn_Tqual tq=_tmpFD;void*typ=_tmpFE;struct Cyc_List_List*tvs=_tmpFF;struct Cyc_List_List*atts=_tmp100;
# 799
Cyc_Lex_register_typedef(x);{
# 801
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp101=typ;struct Cyc_Core_Opt*_tmp102;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp101)->tag == 1U){_LL4: _tmp102=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp101)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmp102;
# 805
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 810
return({void*_tmp7EE=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=9U,({struct Cyc_Absyn_Typedefdecl*_tmp7ED=({struct Cyc_Absyn_Typedefdecl*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->name=x,_tmp103->tvs=tvs,_tmp103->kind=kind,_tmp103->defn=type,_tmp103->atts=atts,_tmp103->tq=tq,_tmp103->extern_c=0;_tmp103;});_tmp104->f1=_tmp7ED;});_tmp104;});Cyc_Absyn_new_decl(_tmp7EE,loc);});}}}
# 817
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7EF=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=12U,_tmp105->f1=d,_tmp105->f2=s;_tmp105;});Cyc_Absyn_new_stmt(_tmp7EF,d->loc);});}
# 821
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 830
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 833
struct _RegionHandle _tmp106=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp106;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp107=ds;struct Cyc_List_List*_tmp10A;struct Cyc_Parse_Type_specifier _tmp109;struct Cyc_Absyn_Tqual _tmp108;_LL1: _tmp108=_tmp107.tq;_tmp109=_tmp107.type_specs;_tmp10A=_tmp107.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp108;struct Cyc_Parse_Type_specifier tss=_tmp109;struct Cyc_List_List*atts=_tmp10A;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp10B=0U;({unsigned _tmp7F1=loc;struct _fat_ptr _tmp7F0=({const char*_tmp10C="inline qualifier on non-function definition";_tag_fat(_tmp10C,sizeof(char),44U);});Cyc_Warn_warn(_tmp7F1,_tmp7F0,_tag_fat(_tmp10B,sizeof(void*),0U));});});{
# 839
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmp10D=ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp10D;enum Cyc_Parse_Storage_class _tmp10E=_stmttmp10;switch(_tmp10E){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 s=Cyc_Flags_no_register?Cyc_Absyn_Public: Cyc_Absyn_Register;goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 s=1U;goto _LL3;default: _LL12: _LL13:
 goto _LL3;}_LL3:;}{
# 855
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 859
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 867
void*_tmp10F=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp10F;
if(declarators == 0){
# 871
void*_tmp110=base_type;struct Cyc_List_List*_tmp111;struct _tuple0*_tmp112;struct Cyc_List_List*_tmp115;int _tmp114;struct _tuple0*_tmp113;struct Cyc_Absyn_Datatypedecl**_tmp116;struct Cyc_List_List*_tmp119;struct _tuple0*_tmp118;enum Cyc_Absyn_AggrKind _tmp117;struct Cyc_Absyn_Datatypedecl*_tmp11A;struct Cyc_Absyn_Enumdecl*_tmp11B;struct Cyc_Absyn_Aggrdecl*_tmp11C;switch(*((int*)_tmp110)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp110)->f1)->r)){case 0U: _LL15: _tmp11C=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp110)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp11C;
# 873
({struct Cyc_List_List*_tmp7F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7F2;});
ad->sc=s;{
struct Cyc_List_List*_tmp11F=({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));({struct Cyc_Absyn_Decl*_tmp7F4=({void*_tmp7F3=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=6U,_tmp11D->f1=ad;_tmp11D;});Cyc_Absyn_new_decl(_tmp7F3,loc);});_tmp11E->hd=_tmp7F4;}),_tmp11E->tl=0;_tmp11E;});_npop_handler(0U);return _tmp11F;}}case 1U: _LL17: _tmp11B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp110)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp11B;
# 877
if(atts != 0)({void*_tmp120=0U;({unsigned _tmp7F6=loc;struct _fat_ptr _tmp7F5=({const char*_tmp121="attributes on enum not supported";_tag_fat(_tmp121,sizeof(char),33U);});Cyc_Warn_err(_tmp7F6,_tmp7F5,_tag_fat(_tmp120,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp124=({struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));({struct Cyc_Absyn_Decl*_tmp7F8=({void*_tmp7F7=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=8U,_tmp122->f1=ed;_tmp122;});Cyc_Absyn_new_decl(_tmp7F7,loc);});_tmp123->hd=_tmp7F8;}),_tmp123->tl=0;_tmp123;});_npop_handler(0U);return _tmp124;}}default: _LL19: _tmp11A=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp110)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp11A;
# 881
if(atts != 0)({void*_tmp125=0U;({unsigned _tmp7FA=loc;struct _fat_ptr _tmp7F9=({const char*_tmp126="attributes on datatypes not supported";_tag_fat(_tmp126,sizeof(char),38U);});Cyc_Warn_err(_tmp7FA,_tmp7F9,_tag_fat(_tmp125,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp129=({struct Cyc_List_List*_tmp128=_cycalloc(sizeof(*_tmp128));({struct Cyc_Absyn_Decl*_tmp7FC=({void*_tmp7FB=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=7U,_tmp127->f1=dd;_tmp127;});Cyc_Absyn_new_decl(_tmp7FB,loc);});_tmp128->hd=_tmp7FC;}),_tmp128->tl=0;_tmp128;});_npop_handler(0U);return _tmp129;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp117=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).UnknownAggr).val).f1;_tmp118=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).UnknownAggr).val).f2;_tmp119=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp117;struct _tuple0*n=_tmp118;struct Cyc_List_List*ts=_tmp119;
# 885
struct Cyc_List_List*_tmp12A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp12A;
struct Cyc_Absyn_Aggrdecl*_tmp12B=({struct Cyc_Absyn_Aggrdecl*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->kind=k,_tmp131->sc=s,_tmp131->name=n,_tmp131->tvs=ts2,_tmp131->impl=0,_tmp131->attributes=0,_tmp131->expected_mem_kind=0;_tmp131;});struct Cyc_Absyn_Aggrdecl*ad=_tmp12B;
if(atts != 0)({void*_tmp12C=0U;({unsigned _tmp7FE=loc;struct _fat_ptr _tmp7FD=({const char*_tmp12D="bad attributes on type declaration";_tag_fat(_tmp12D,sizeof(char),35U);});Cyc_Warn_err(_tmp7FE,_tmp7FD,_tag_fat(_tmp12C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp130=({struct Cyc_List_List*_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct Cyc_Absyn_Decl*_tmp800=({void*_tmp7FF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=6U,_tmp12E->f1=ad;_tmp12E;});Cyc_Absyn_new_decl(_tmp7FF,loc);});_tmp12F->hd=_tmp800;}),_tmp12F->tl=0;_tmp12F;});_npop_handler(0U);return _tmp130;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp116=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp116;
# 890
if(atts != 0)({void*_tmp132=0U;({unsigned _tmp802=loc;struct _fat_ptr _tmp801=({const char*_tmp133="bad attributes on datatype";_tag_fat(_tmp133,sizeof(char),27U);});Cyc_Warn_err(_tmp802,_tmp801,_tag_fat(_tmp132,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp136=({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));({struct Cyc_Absyn_Decl*_tmp804=({void*_tmp803=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=7U,_tmp134->f1=*tudp;_tmp134;});Cyc_Absyn_new_decl(_tmp803,loc);});_tmp135->hd=_tmp804;}),_tmp135->tl=0;_tmp135;});_npop_handler(0U);return _tmp136;}}}else{_LL1F: _tmp113=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).UnknownDatatype).val).name;_tmp114=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp115=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f2;_LL20: {struct _tuple0*n=_tmp113;int isx=_tmp114;struct Cyc_List_List*ts=_tmp115;
# 893
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp137;
struct Cyc_Absyn_Decl*_tmp138=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp138;
if(atts != 0)({void*_tmp139=0U;({unsigned _tmp806=loc;struct _fat_ptr _tmp805=({const char*_tmp13A="bad attributes on datatype";_tag_fat(_tmp13A,sizeof(char),27U);});Cyc_Warn_err(_tmp806,_tmp805,_tag_fat(_tmp139,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13C=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->hd=tud,_tmp13B->tl=0;_tmp13B;});_npop_handler(0U);return _tmp13C;}}}case 15U: _LL21: _tmp112=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1;_LL22: {struct _tuple0*n=_tmp112;
# 898
struct Cyc_Absyn_Enumdecl*_tmp13D=({struct Cyc_Absyn_Enumdecl*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->sc=s,_tmp144->name=n,_tmp144->fields=0;_tmp144;});struct Cyc_Absyn_Enumdecl*ed=_tmp13D;
if(atts != 0)({void*_tmp13E=0U;({unsigned _tmp808=loc;struct _fat_ptr _tmp807=({const char*_tmp13F="bad attributes on enum";_tag_fat(_tmp13F,sizeof(char),23U);});Cyc_Warn_err(_tmp808,_tmp807,_tag_fat(_tmp13E,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp143=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));({struct Cyc_Absyn_Decl*_tmp80A=({struct Cyc_Absyn_Decl*_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp809=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->tag=8U,_tmp140->f1=ed;_tmp140;});_tmp141->r=_tmp809;}),_tmp141->loc=loc;_tmp141;});_tmp142->hd=_tmp80A;}),_tmp142->tl=0;_tmp142;});_npop_handler(0U);return _tmp143;}}case 16U: _LL23: _tmp111=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp110)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp111;
# 904
struct Cyc_Absyn_Enumdecl*_tmp145=({struct Cyc_Absyn_Enumdecl*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->sc=s,({struct _tuple0*_tmp80C=Cyc_Parse_gensym_enum();_tmp14D->name=_tmp80C;}),({struct Cyc_Core_Opt*_tmp80B=({struct Cyc_Core_Opt*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->v=fs;_tmp14C;});_tmp14D->fields=_tmp80B;});_tmp14D;});struct Cyc_Absyn_Enumdecl*ed=_tmp145;
if(atts != 0)({void*_tmp146=0U;({unsigned _tmp80E=loc;struct _fat_ptr _tmp80D=({const char*_tmp147="bad attributes on enum";_tag_fat(_tmp147,sizeof(char),23U);});Cyc_Warn_err(_tmp80E,_tmp80D,_tag_fat(_tmp146,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp14B=({struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*_tmp14A));({struct Cyc_Absyn_Decl*_tmp810=({struct Cyc_Absyn_Decl*_tmp149=_cycalloc(sizeof(*_tmp149));({void*_tmp80F=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->tag=8U,_tmp148->f1=ed;_tmp148;});_tmp149->r=_tmp80F;}),_tmp149->loc=loc;_tmp149;});_tmp14A->hd=_tmp810;}),_tmp14A->tl=0;_tmp14A;});_npop_handler(0U);return _tmp14B;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp14E=0U;({unsigned _tmp812=loc;struct _fat_ptr _tmp811=({const char*_tmp14F="missing declarator";_tag_fat(_tmp14F,sizeof(char),19U);});Cyc_Warn_err(_tmp812,_tmp811,_tag_fat(_tmp14E,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp150=0;_npop_handler(0U);return _tmp150;}}_LL14:;}{
# 911
struct Cyc_List_List*_tmp151=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp151;
if(istypedef){
# 916
if(!exps_empty)
({void*_tmp152=0U;({unsigned _tmp814=loc;struct _fat_ptr _tmp813=({const char*_tmp153="initializer in typedef declaration";_tag_fat(_tmp153,sizeof(char),35U);});Cyc_Warn_err(_tmp814,_tmp813,_tag_fat(_tmp152,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp154=decls;_npop_handler(0U);return _tmp154;}}{
# 922
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp155=fields;struct Cyc_List_List*ds=_tmp155;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp156=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp156;struct _tuple16*_tmp157=_stmttmp11;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15C;void*_tmp15B;struct Cyc_Absyn_Tqual _tmp15A;struct _tuple0*_tmp159;unsigned _tmp158;_LL28: _tmp158=_tmp157->f1;_tmp159=_tmp157->f2;_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;_tmp15C=_tmp157->f5;_tmp15D=_tmp157->f6;_LL29: {unsigned varloc=_tmp158;struct _tuple0*x=_tmp159;struct Cyc_Absyn_Tqual tq2=_tmp15A;void*t2=_tmp15B;struct Cyc_List_List*tvs2=_tmp15C;struct Cyc_List_List*atts2=_tmp15D;
if(tvs2 != 0)
({void*_tmp15E=0U;({unsigned _tmp816=loc;struct _fat_ptr _tmp815=({const char*_tmp15F="bad type params, ignoring";_tag_fat(_tmp15F,sizeof(char),26U);});Cyc_Warn_warn(_tmp816,_tmp815,_tag_fat(_tmp15E,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp160=0U;({unsigned _tmp818=loc;struct _fat_ptr _tmp817=({const char*_tmp161="unexpected NULL in parse!";_tag_fat(_tmp161,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp818,_tmp817,_tag_fat(_tmp160,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp162=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp162;
struct Cyc_Absyn_Vardecl*_tmp163=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp163;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp164=({struct Cyc_Absyn_Decl*_tmp167=_cycalloc(sizeof(*_tmp167));({void*_tmp819=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->tag=0U,_tmp166->f1=vd;_tmp166;});_tmp167->r=_tmp819;}),_tmp167->loc=loc;_tmp167;});struct Cyc_Absyn_Decl*d=_tmp164;
decls=({struct Cyc_List_List*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->hd=d,_tmp165->tl=decls;_tmp165;});}}}}}{
# 937
struct Cyc_List_List*_tmp168=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp168;}}}}}}}}}
# 834
;_pop_region();}
# 941
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp169=e->r;void*_stmttmp12=_tmp169;void*_tmp16A=_stmttmp12;int _tmp16B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16A)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16A)->f1).Int_c).tag == 5){_LL1: _tmp16B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16A)->f1).Int_c).val).f2;_LL2: {int i=_tmp16B;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 945
({void*_tmp16C=0U;({unsigned _tmp81B=loc;struct _fat_ptr _tmp81A=({const char*_tmp16D="expecting integer constant";_tag_fat(_tmp16D,sizeof(char),27U);});Cyc_Warn_err(_tmp81B,_tmp81A,_tag_fat(_tmp16C,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 951
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16E=e->r;void*_stmttmp13=_tmp16E;void*_tmp16F=_stmttmp13;struct _fat_ptr _tmp170;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16F)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16F)->f1).String_c).tag == 8){_LL1: _tmp170=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16F)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp170;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 955
({void*_tmp171=0U;({unsigned _tmp81D=loc;struct _fat_ptr _tmp81C=({const char*_tmp172="expecting string constant";_tag_fat(_tmp172,sizeof(char),26U);});Cyc_Warn_err(_tmp81D,_tmp81C,_tag_fat(_tmp171,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 961
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp173=x;long long _tmp174;char _tmp175;int _tmp176;switch((_tmp173.LongLong_c).tag){case 5U: _LL1: _tmp176=((_tmp173.Int_c).val).f2;_LL2: {int i=_tmp176;
return(unsigned)i;}case 2U: _LL3: _tmp175=((_tmp173.Char_c).val).f2;_LL4: {char c=_tmp175;
return(unsigned)c;}case 6U: _LL5: _tmp174=((_tmp173.LongLong_c).val).f2;_LL6: {long long x=_tmp174;
# 966
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp177=0U;({unsigned _tmp81F=loc;struct _fat_ptr _tmp81E=({const char*_tmp178="integer constant too large";_tag_fat(_tmp178,sizeof(char),27U);});Cyc_Warn_err(_tmp81F,_tmp81E,_tag_fat(_tmp177,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 971
({struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp6DA;_tmp6DA.tag=0U,({struct _fat_ptr _tmp820=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6DA.f1=_tmp820;});_tmp6DA;});void*_tmp179[1U];_tmp179[0]=& _tmp17B;({unsigned _tmp822=loc;struct _fat_ptr _tmp821=({const char*_tmp17A="expected integer constant but found %s";_tag_fat(_tmp17A,sizeof(char),39U);});Cyc_Warn_err(_tmp822,_tmp821,_tag_fat(_tmp179,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 977
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17C=p->r;void*_stmttmp14=_tmp17C;void*_tmp17D=_stmttmp14;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_List_List*_tmp180;struct _tuple0*_tmp17F;int _tmp182;struct _fat_ptr _tmp181;char _tmp183;int _tmp185;enum Cyc_Absyn_Sign _tmp184;struct Cyc_Absyn_Pat*_tmp186;struct Cyc_Absyn_Vardecl*_tmp187;struct _tuple0*_tmp188;switch(*((int*)_tmp17D)){case 15U: _LL1: _tmp188=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_LL2: {struct _tuple0*x=_tmp188;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17D)->f2)->r)->tag == 0U){_LL3: _tmp187=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp187;
# 981
return({struct Cyc_Absyn_Exp*_tmp823=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp823,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp186=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp186;
return({struct Cyc_Absyn_Exp*_tmp824=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp824,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp184=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_tmp185=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17D)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp184;int i=_tmp185;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp183=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_LLC: {char c=_tmp183;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp181=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_tmp182=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17D)->f2;_LLE: {struct _fat_ptr s=_tmp181;int i=_tmp182;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17D)->f3 == 0){_LLF: _tmp17F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_tmp180=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17D)->f2;_LL10: {struct _tuple0*x=_tmp17F;struct Cyc_List_List*ps=_tmp180;
# 988
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp17E=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17D)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp17E;
return e;}default: _LL13: _LL14:
# 993
({void*_tmp189=0U;({unsigned _tmp826=p->loc;struct _fat_ptr _tmp825=({const char*_tmp18A="cannot mix patterns and expressions in case";_tag_fat(_tmp18A,sizeof(char),44U);});Cyc_Warn_err(_tmp826,_tmp825,_tag_fat(_tmp189,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1078
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->tag=Cyc_Core_Failure,_tmp18B->f1=s;_tmp18B;}));}static char _tmp18E[7U]="cnst_t";
# 1050 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18E,_tmp18E,_tmp18E + 7U};
union Cyc_YYSTYPE*_tmp18C=yy1;union Cyc_Absyn_Cnst _tmp18D;if((((union Cyc_YYSTYPE*)_tmp18C)->Int_tok).tag == 1){_LL1: _tmp18D=(_tmp18C->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp18D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1057
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.Int_tok).tag=1U,(_tmp6DB.Int_tok).val=yy1;_tmp6DB;});}static char _tmp191[5U]="char";
# 1051 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp191,_tmp191,_tmp191 + 5U};
union Cyc_YYSTYPE*_tmp18F=yy1;char _tmp190;if((((union Cyc_YYSTYPE*)_tmp18F)->Char_tok).tag == 2){_LL1: _tmp190=(_tmp18F->Char_tok).val;_LL2: {char yy=_tmp190;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1058
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.Char_tok).tag=2U,(_tmp6DC.Char_tok).val=yy1;_tmp6DC;});}static char _tmp194[13U]="string_t<`H>";
# 1052 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp194,_tmp194,_tmp194 + 13U};
union Cyc_YYSTYPE*_tmp192=yy1;struct _fat_ptr _tmp193;if((((union Cyc_YYSTYPE*)_tmp192)->String_tok).tag == 3){_LL1: _tmp193=(_tmp192->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp193;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1059
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.String_tok).tag=3U,(_tmp6DD.String_tok).val=yy1;_tmp6DD;});}static char _tmp197[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1055 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp197,_tmp197,_tmp197 + 35U};
union Cyc_YYSTYPE*_tmp195=yy1;struct _tuple22*_tmp196;if((((union Cyc_YYSTYPE*)_tmp195)->YY1).tag == 9){_LL1: _tmp196=(_tmp195->YY1).val;_LL2: {struct _tuple22*yy=_tmp196;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1062
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY1).tag=9U,(_tmp6DE.YY1).val=yy1;_tmp6DE;});}static char _tmp19A[11U]="ptrbound_t";
# 1056 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19A,_tmp19A,_tmp19A + 11U};
union Cyc_YYSTYPE*_tmp198=yy1;void*_tmp199;if((((union Cyc_YYSTYPE*)_tmp198)->YY2).tag == 10){_LL1: _tmp199=(_tmp198->YY2).val;_LL2: {void*yy=_tmp199;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1063
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY2).tag=10U,(_tmp6DF.YY2).val=yy1;_tmp6DF;});}static char _tmp19D[28U]="list_t<offsetof_field_t,`H>";
# 1057 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19D,_tmp19D,_tmp19D + 28U};
union Cyc_YYSTYPE*_tmp19B=yy1;struct Cyc_List_List*_tmp19C;if((((union Cyc_YYSTYPE*)_tmp19B)->YY3).tag == 11){_LL1: _tmp19C=(_tmp19B->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp19C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1064
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY3).tag=11U,(_tmp6E0.YY3).val=yy1;_tmp6E0;});}static char _tmp1A0[6U]="exp_t";
# 1058 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A0,_tmp1A0,_tmp1A0 + 6U};
union Cyc_YYSTYPE*_tmp19E=yy1;struct Cyc_Absyn_Exp*_tmp19F;if((((union Cyc_YYSTYPE*)_tmp19E)->Exp_tok).tag == 7){_LL1: _tmp19F=(_tmp19E->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp19F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1065
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.Exp_tok).tag=7U,(_tmp6E1.Exp_tok).val=yy1;_tmp6E1;});}static char _tmp1A3[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A3,_tmp1A3,_tmp1A3 + 17U};
union Cyc_YYSTYPE*_tmp1A1=yy1;struct Cyc_List_List*_tmp1A2;if((((union Cyc_YYSTYPE*)_tmp1A1)->YY4).tag == 12){_LL1: _tmp1A2=(_tmp1A1->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp1A2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1073
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY4).tag=12U,(_tmp6E2.YY4).val=yy1;_tmp6E2;});}static char _tmp1A6[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1067 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A6,_tmp1A6,_tmp1A6 + 47U};
union Cyc_YYSTYPE*_tmp1A4=yy1;struct Cyc_List_List*_tmp1A5;if((((union Cyc_YYSTYPE*)_tmp1A4)->YY5).tag == 13){_LL1: _tmp1A5=(_tmp1A4->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1A5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1074
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY5).tag=13U,(_tmp6E3.YY5).val=yy1;_tmp6E3;});}static char _tmp1A9[9U]="primop_t";
# 1068 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A9,_tmp1A9,_tmp1A9 + 9U};
union Cyc_YYSTYPE*_tmp1A7=yy1;enum Cyc_Absyn_Primop _tmp1A8;if((((union Cyc_YYSTYPE*)_tmp1A7)->YY6).tag == 14){_LL1: _tmp1A8=(_tmp1A7->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1A8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1075
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY6).tag=14U,(_tmp6E4.YY6).val=yy1;_tmp6E4;});}static char _tmp1AC[19U]="opt_t<primop_t,`H>";
# 1069 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AC,_tmp1AC,_tmp1AC + 19U};
union Cyc_YYSTYPE*_tmp1AA=yy1;struct Cyc_Core_Opt*_tmp1AB;if((((union Cyc_YYSTYPE*)_tmp1AA)->YY7).tag == 15){_LL1: _tmp1AB=(_tmp1AA->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1AB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1076
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY7).tag=15U,(_tmp6E5.YY7).val=yy1;_tmp6E5;});}static char _tmp1AF[7U]="qvar_t";
# 1070 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AF,_tmp1AF,_tmp1AF + 7U};
union Cyc_YYSTYPE*_tmp1AD=yy1;struct _tuple0*_tmp1AE;if((((union Cyc_YYSTYPE*)_tmp1AD)->QualId_tok).tag == 5){_LL1: _tmp1AE=(_tmp1AD->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1AE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1077
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.QualId_tok).tag=5U,(_tmp6E6.QualId_tok).val=yy1;_tmp6E6;});}static char _tmp1B2[7U]="stmt_t";
# 1073 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B2,_tmp1B2,_tmp1B2 + 7U};
union Cyc_YYSTYPE*_tmp1B0=yy1;struct Cyc_Absyn_Stmt*_tmp1B1;if((((union Cyc_YYSTYPE*)_tmp1B0)->Stmt_tok).tag == 8){_LL1: _tmp1B1=(_tmp1B0->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1B1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1080
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.Stmt_tok).tag=8U,(_tmp6E7.Stmt_tok).val=yy1;_tmp6E7;});}static char _tmp1B5[27U]="list_t<switch_clause_t,`H>";
# 1077 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B5,_tmp1B5,_tmp1B5 + 27U};
union Cyc_YYSTYPE*_tmp1B3=yy1;struct Cyc_List_List*_tmp1B4;if((((union Cyc_YYSTYPE*)_tmp1B3)->YY8).tag == 16){_LL1: _tmp1B4=(_tmp1B3->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1B4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1084
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY8).tag=16U,(_tmp6E8.YY8).val=yy1;_tmp6E8;});}static char _tmp1B8[6U]="pat_t";
# 1078 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B8,_tmp1B8,_tmp1B8 + 6U};
union Cyc_YYSTYPE*_tmp1B6=yy1;struct Cyc_Absyn_Pat*_tmp1B7;if((((union Cyc_YYSTYPE*)_tmp1B6)->YY9).tag == 17){_LL1: _tmp1B7=(_tmp1B6->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1B7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1085
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY9).tag=17U,(_tmp6E9.YY9).val=yy1;_tmp6E9;});}static char _tmp1BB[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1083 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BB,_tmp1BB,_tmp1BB + 28U};
union Cyc_YYSTYPE*_tmp1B9=yy1;struct _tuple23*_tmp1BA;if((((union Cyc_YYSTYPE*)_tmp1B9)->YY10).tag == 18){_LL1: _tmp1BA=(_tmp1B9->YY10).val;_LL2: {struct _tuple23*yy=_tmp1BA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1090
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY10).tag=18U,(_tmp6EA.YY10).val=yy1;_tmp6EA;});}static char _tmp1BE[17U]="list_t<pat_t,`H>";
# 1084 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BE,_tmp1BE,_tmp1BE + 17U};
union Cyc_YYSTYPE*_tmp1BC=yy1;struct Cyc_List_List*_tmp1BD;if((((union Cyc_YYSTYPE*)_tmp1BC)->YY11).tag == 19){_LL1: _tmp1BD=(_tmp1BC->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1BD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1091
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY11).tag=19U,(_tmp6EB.YY11).val=yy1;_tmp6EB;});}static char _tmp1C1[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1085 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C1,_tmp1C1,_tmp1C1 + 36U};
union Cyc_YYSTYPE*_tmp1BF=yy1;struct _tuple24*_tmp1C0;if((((union Cyc_YYSTYPE*)_tmp1BF)->YY12).tag == 20){_LL1: _tmp1C0=(_tmp1BF->YY12).val;_LL2: {struct _tuple24*yy=_tmp1C0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1092
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY12).tag=20U,(_tmp6EC.YY12).val=yy1;_tmp6EC;});}static char _tmp1C4[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1086 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C4,_tmp1C4,_tmp1C4 + 47U};
union Cyc_YYSTYPE*_tmp1C2=yy1;struct Cyc_List_List*_tmp1C3;if((((union Cyc_YYSTYPE*)_tmp1C2)->YY13).tag == 21){_LL1: _tmp1C3=(_tmp1C2->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1C3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1093
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY13).tag=21U,(_tmp6ED.YY13).val=yy1;_tmp6ED;});}static char _tmp1C7[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1087 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C7,_tmp1C7,_tmp1C7 + 58U};
union Cyc_YYSTYPE*_tmp1C5=yy1;struct _tuple23*_tmp1C6;if((((union Cyc_YYSTYPE*)_tmp1C5)->YY14).tag == 22){_LL1: _tmp1C6=(_tmp1C5->YY14).val;_LL2: {struct _tuple23*yy=_tmp1C6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1094
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY14).tag=22U,(_tmp6EE.YY14).val=yy1;_tmp6EE;});}static char _tmp1CA[9U]="fndecl_t";
# 1088 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CA,_tmp1CA,_tmp1CA + 9U};
union Cyc_YYSTYPE*_tmp1C8=yy1;struct Cyc_Absyn_Fndecl*_tmp1C9;if((((union Cyc_YYSTYPE*)_tmp1C8)->YY15).tag == 23){_LL1: _tmp1C9=(_tmp1C8->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1C9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1095
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY15).tag=23U,(_tmp6EF.YY15).val=yy1;_tmp6EF;});}static char _tmp1CD[18U]="list_t<decl_t,`H>";
# 1089 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CD,_tmp1CD,_tmp1CD + 18U};
union Cyc_YYSTYPE*_tmp1CB=yy1;struct Cyc_List_List*_tmp1CC;if((((union Cyc_YYSTYPE*)_tmp1CB)->YY16).tag == 24){_LL1: _tmp1CC=(_tmp1CB->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1CC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1096
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY16).tag=24U,(_tmp6F0.YY16).val=yy1;_tmp6F0;});}static char _tmp1D0[12U]="decl_spec_t";
# 1092 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D0,_tmp1D0,_tmp1D0 + 12U};
union Cyc_YYSTYPE*_tmp1CE=yy1;struct Cyc_Parse_Declaration_spec _tmp1CF;if((((union Cyc_YYSTYPE*)_tmp1CE)->YY17).tag == 25){_LL1: _tmp1CF=(_tmp1CE->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1CF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1099
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY17).tag=25U,(_tmp6F1.YY17).val=yy1;_tmp6F1;});}static char _tmp1D3[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1093 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D3,_tmp1D3,_tmp1D3 + 31U};
union Cyc_YYSTYPE*_tmp1D1=yy1;struct _tuple12 _tmp1D2;if((((union Cyc_YYSTYPE*)_tmp1D1)->YY18).tag == 26){_LL1: _tmp1D2=(_tmp1D1->YY18).val;_LL2: {struct _tuple12 yy=_tmp1D2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1100
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY18).tag=26U,(_tmp6F2.YY18).val=yy1;_tmp6F2;});}static char _tmp1D6[23U]="declarator_list_t<`yy>";
# 1094 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D6,_tmp1D6,_tmp1D6 + 23U};
union Cyc_YYSTYPE*_tmp1D4=yy1;struct _tuple13*_tmp1D5;if((((union Cyc_YYSTYPE*)_tmp1D4)->YY19).tag == 27){_LL1: _tmp1D5=(_tmp1D4->YY19).val;_LL2: {struct _tuple13*yy=_tmp1D5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1101
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY19).tag=27U,(_tmp6F3.YY19).val=yy1;_tmp6F3;});}static char _tmp1D9[16U]="storage_class_t";
# 1095 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D9,_tmp1D9,_tmp1D9 + 16U};
union Cyc_YYSTYPE*_tmp1D7=yy1;enum Cyc_Parse_Storage_class _tmp1D8;if((((union Cyc_YYSTYPE*)_tmp1D7)->YY20).tag == 28){_LL1: _tmp1D8=(_tmp1D7->YY20).val;_LL2: {enum Cyc_Parse_Storage_class yy=_tmp1D8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1102
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY20).tag=28U,(_tmp6F4.YY20).val=yy1;_tmp6F4;});}static char _tmp1DC[17U]="type_specifier_t";
# 1096 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DC,_tmp1DC,_tmp1DC + 17U};
union Cyc_YYSTYPE*_tmp1DA=yy1;struct Cyc_Parse_Type_specifier _tmp1DB;if((((union Cyc_YYSTYPE*)_tmp1DA)->YY21).tag == 29){_LL1: _tmp1DB=(_tmp1DA->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1DB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY21).tag=29U,(_tmp6F5.YY21).val=yy1;_tmp6F5;});}static char _tmp1DF[12U]="aggr_kind_t";
# 1098 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DF,_tmp1DF,_tmp1DF + 12U};
union Cyc_YYSTYPE*_tmp1DD=yy1;enum Cyc_Absyn_AggrKind _tmp1DE;if((((union Cyc_YYSTYPE*)_tmp1DD)->YY22).tag == 30){_LL1: _tmp1DE=(_tmp1DD->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1DE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY22).tag=30U,(_tmp6F6.YY22).val=yy1;_tmp6F6;});}static char _tmp1E2[8U]="tqual_t";
# 1099 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E2,_tmp1E2,_tmp1E2 + 8U};
union Cyc_YYSTYPE*_tmp1E0=yy1;struct Cyc_Absyn_Tqual _tmp1E1;if((((union Cyc_YYSTYPE*)_tmp1E0)->YY23).tag == 31){_LL1: _tmp1E1=(_tmp1E0->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1E1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1106
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY23).tag=31U,(_tmp6F7.YY23).val=yy1;_tmp6F7;});}static char _tmp1E5[23U]="list_t<aggrfield_t,`H>";
# 1100 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E5,_tmp1E5,_tmp1E5 + 23U};
union Cyc_YYSTYPE*_tmp1E3=yy1;struct Cyc_List_List*_tmp1E4;if((((union Cyc_YYSTYPE*)_tmp1E3)->YY24).tag == 32){_LL1: _tmp1E4=(_tmp1E3->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1E4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1107
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY24).tag=32U,(_tmp6F8.YY24).val=yy1;_tmp6F8;});}static char _tmp1E8[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1101 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E8,_tmp1E8,_tmp1E8 + 34U};
union Cyc_YYSTYPE*_tmp1E6=yy1;struct Cyc_List_List*_tmp1E7;if((((union Cyc_YYSTYPE*)_tmp1E6)->YY25).tag == 33){_LL1: _tmp1E7=(_tmp1E6->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1E7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY25).tag=33U,(_tmp6F9.YY25).val=yy1;_tmp6F9;});}static char _tmp1EB[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1102 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EB,_tmp1EB,_tmp1EB + 33U};
union Cyc_YYSTYPE*_tmp1E9=yy1;struct Cyc_List_List*_tmp1EA;if((((union Cyc_YYSTYPE*)_tmp1E9)->YY26).tag == 34){_LL1: _tmp1EA=(_tmp1E9->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1EA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY26).tag=34U,(_tmp6FA.YY26).val=yy1;_tmp6FA;});}static char _tmp1EE[18U]="declarator_t<`yy>";
# 1103 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EE,_tmp1EE,_tmp1EE + 18U};
union Cyc_YYSTYPE*_tmp1EC=yy1;struct Cyc_Parse_Declarator _tmp1ED;if((((union Cyc_YYSTYPE*)_tmp1EC)->YY27).tag == 35){_LL1: _tmp1ED=(_tmp1EC->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1ED;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY27).tag=35U,(_tmp6FB.YY27).val=yy1;_tmp6FB;});}static char _tmp1F1[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1104 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F1,_tmp1F1,_tmp1F1 + 45U};
union Cyc_YYSTYPE*_tmp1EF=yy1;struct _tuple25*_tmp1F0;if((((union Cyc_YYSTYPE*)_tmp1EF)->YY28).tag == 36){_LL1: _tmp1F0=(_tmp1EF->YY28).val;_LL2: {struct _tuple25*yy=_tmp1F0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY28).tag=36U,(_tmp6FC.YY28).val=yy1;_tmp6FC;});}static char _tmp1F4[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1105 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F4,_tmp1F4,_tmp1F4 + 57U};
union Cyc_YYSTYPE*_tmp1F2=yy1;struct Cyc_List_List*_tmp1F3;if((((union Cyc_YYSTYPE*)_tmp1F2)->YY29).tag == 37){_LL1: _tmp1F3=(_tmp1F2->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1F3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1112
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY29).tag=37U,(_tmp6FD.YY29).val=yy1;_tmp6FD;});}static char _tmp1F7[26U]="abstractdeclarator_t<`yy>";
# 1106 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F7,_tmp1F7,_tmp1F7 + 26U};
union Cyc_YYSTYPE*_tmp1F5=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F6;if((((union Cyc_YYSTYPE*)_tmp1F5)->YY30).tag == 38){_LL1: _tmp1F6=(_tmp1F5->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1F6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1113
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY30).tag=38U,(_tmp6FE.YY30).val=yy1;_tmp6FE;});}static char _tmp1FA[5U]="bool";
# 1107 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FA,_tmp1FA,_tmp1FA + 5U};
union Cyc_YYSTYPE*_tmp1F8=yy1;int _tmp1F9;if((((union Cyc_YYSTYPE*)_tmp1F8)->YY31).tag == 39){_LL1: _tmp1F9=(_tmp1F8->YY31).val;_LL2: {int yy=_tmp1F9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1114
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY31).tag=39U,(_tmp6FF.YY31).val=yy1;_tmp6FF;});}static char _tmp1FD[8U]="scope_t";
# 1108 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FD,_tmp1FD,_tmp1FD + 8U};
union Cyc_YYSTYPE*_tmp1FB=yy1;enum Cyc_Absyn_Scope _tmp1FC;if((((union Cyc_YYSTYPE*)_tmp1FB)->YY32).tag == 40){_LL1: _tmp1FC=(_tmp1FB->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1FC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1115
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY32).tag=40U,(_tmp700.YY32).val=yy1;_tmp700;});}static char _tmp200[16U]="datatypefield_t";
# 1109 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp200,_tmp200,_tmp200 + 16U};
union Cyc_YYSTYPE*_tmp1FE=yy1;struct Cyc_Absyn_Datatypefield*_tmp1FF;if((((union Cyc_YYSTYPE*)_tmp1FE)->YY33).tag == 41){_LL1: _tmp1FF=(_tmp1FE->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp1FF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1116
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY33).tag=41U,(_tmp701.YY33).val=yy1;_tmp701;});}static char _tmp203[27U]="list_t<datatypefield_t,`H>";
# 1110 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp203,_tmp203,_tmp203 + 27U};
union Cyc_YYSTYPE*_tmp201=yy1;struct Cyc_List_List*_tmp202;if((((union Cyc_YYSTYPE*)_tmp201)->YY34).tag == 42){_LL1: _tmp202=(_tmp201->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp202;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1117
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY34).tag=42U,(_tmp702.YY34).val=yy1;_tmp702;});}static char _tmp206[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1111 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp206,_tmp206,_tmp206 + 41U};
union Cyc_YYSTYPE*_tmp204=yy1;struct _tuple26 _tmp205;if((((union Cyc_YYSTYPE*)_tmp204)->YY35).tag == 43){_LL1: _tmp205=(_tmp204->YY35).val;_LL2: {struct _tuple26 yy=_tmp205;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1118
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY35).tag=43U,(_tmp703.YY35).val=yy1;_tmp703;});}static char _tmp209[17U]="list_t<var_t,`H>";
# 1112 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp209,_tmp209,_tmp209 + 17U};
union Cyc_YYSTYPE*_tmp207=yy1;struct Cyc_List_List*_tmp208;if((((union Cyc_YYSTYPE*)_tmp207)->YY36).tag == 44){_LL1: _tmp208=(_tmp207->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp208;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY36).tag=44U,(_tmp704.YY36).val=yy1;_tmp704;});}static char _tmp20C[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1113 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20C,_tmp20C,_tmp20C + 31U};
union Cyc_YYSTYPE*_tmp20A=yy1;struct _tuple8*_tmp20B;if((((union Cyc_YYSTYPE*)_tmp20A)->YY37).tag == 45){_LL1: _tmp20B=(_tmp20A->YY37).val;_LL2: {struct _tuple8*yy=_tmp20B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY37).tag=45U,(_tmp705.YY37).val=yy1;_tmp705;});}static char _tmp20F[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1114 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20F,_tmp20F,_tmp20F + 42U};
union Cyc_YYSTYPE*_tmp20D=yy1;struct Cyc_List_List*_tmp20E;if((((union Cyc_YYSTYPE*)_tmp20D)->YY38).tag == 46){_LL1: _tmp20E=(_tmp20D->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp20E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY38).tag=46U,(_tmp706.YY38).val=yy1;_tmp706;});}static char _tmp212[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1115 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp212,_tmp212,_tmp212 + 115U};
union Cyc_YYSTYPE*_tmp210=yy1;struct _tuple27*_tmp211;if((((union Cyc_YYSTYPE*)_tmp210)->YY39).tag == 47){_LL1: _tmp211=(_tmp210->YY39).val;_LL2: {struct _tuple27*yy=_tmp211;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY39).tag=47U,(_tmp707.YY39).val=yy1;_tmp707;});}static char _tmp215[8U]="types_t";
# 1116 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp215,_tmp215,_tmp215 + 8U};
union Cyc_YYSTYPE*_tmp213=yy1;struct Cyc_List_List*_tmp214;if((((union Cyc_YYSTYPE*)_tmp213)->YY40).tag == 48){_LL1: _tmp214=(_tmp213->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp214;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY40).tag=48U,(_tmp708.YY40).val=yy1;_tmp708;});}static char _tmp218[24U]="list_t<designator_t,`H>";
# 1118 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp218,_tmp218,_tmp218 + 24U};
union Cyc_YYSTYPE*_tmp216=yy1;struct Cyc_List_List*_tmp217;if((((union Cyc_YYSTYPE*)_tmp216)->YY41).tag == 49){_LL1: _tmp217=(_tmp216->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp217;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1125
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY41).tag=49U,(_tmp709.YY41).val=yy1;_tmp709;});}static char _tmp21B[13U]="designator_t";
# 1119 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21B,_tmp21B,_tmp21B + 13U};
union Cyc_YYSTYPE*_tmp219=yy1;void*_tmp21A;if((((union Cyc_YYSTYPE*)_tmp219)->YY42).tag == 50){_LL1: _tmp21A=(_tmp219->YY42).val;_LL2: {void*yy=_tmp21A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1126
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY42).tag=50U,(_tmp70A.YY42).val=yy1;_tmp70A;});}static char _tmp21E[7U]="kind_t";
# 1120 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21E,_tmp21E,_tmp21E + 7U};
union Cyc_YYSTYPE*_tmp21C=yy1;struct Cyc_Absyn_Kind*_tmp21D;if((((union Cyc_YYSTYPE*)_tmp21C)->YY43).tag == 51){_LL1: _tmp21D=(_tmp21C->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp21D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1127
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY43).tag=51U,(_tmp70B.YY43).val=yy1;_tmp70B;});}static char _tmp221[7U]="type_t";
# 1121 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp221,_tmp221,_tmp221 + 7U};
union Cyc_YYSTYPE*_tmp21F=yy1;void*_tmp220;if((((union Cyc_YYSTYPE*)_tmp21F)->YY44).tag == 52){_LL1: _tmp220=(_tmp21F->YY44).val;_LL2: {void*yy=_tmp220;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1128
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY44).tag=52U,(_tmp70C.YY44).val=yy1;_tmp70C;});}static char _tmp224[23U]="list_t<attribute_t,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp224,_tmp224,_tmp224 + 23U};
union Cyc_YYSTYPE*_tmp222=yy1;struct Cyc_List_List*_tmp223;if((((union Cyc_YYSTYPE*)_tmp222)->YY45).tag == 53){_LL1: _tmp223=(_tmp222->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp223;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1129
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY45).tag=53U,(_tmp70D.YY45).val=yy1;_tmp70D;});}static char _tmp227[12U]="attribute_t";
# 1123 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp227,_tmp227,_tmp227 + 12U};
union Cyc_YYSTYPE*_tmp225=yy1;void*_tmp226;if((((union Cyc_YYSTYPE*)_tmp225)->YY46).tag == 54){_LL1: _tmp226=(_tmp225->YY46).val;_LL2: {void*yy=_tmp226;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY46).tag=54U,(_tmp70E.YY46).val=yy1;_tmp70E;});}static char _tmp22A[12U]="enumfield_t";
# 1124 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22A,_tmp22A,_tmp22A + 12U};
union Cyc_YYSTYPE*_tmp228=yy1;struct Cyc_Absyn_Enumfield*_tmp229;if((((union Cyc_YYSTYPE*)_tmp228)->YY47).tag == 55){_LL1: _tmp229=(_tmp228->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp229;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY47).tag=55U,(_tmp70F.YY47).val=yy1;_tmp70F;});}static char _tmp22D[23U]="list_t<enumfield_t,`H>";
# 1125 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22D,_tmp22D,_tmp22D + 23U};
union Cyc_YYSTYPE*_tmp22B=yy1;struct Cyc_List_List*_tmp22C;if((((union Cyc_YYSTYPE*)_tmp22B)->YY48).tag == 56){_LL1: _tmp22C=(_tmp22B->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp22C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1132
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY48).tag=56U,(_tmp710.YY48).val=yy1;_tmp710;});}static char _tmp230[11U]="type_opt_t";
# 1126 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp230,_tmp230,_tmp230 + 11U};
union Cyc_YYSTYPE*_tmp22E=yy1;void*_tmp22F;if((((union Cyc_YYSTYPE*)_tmp22E)->YY49).tag == 57){_LL1: _tmp22F=(_tmp22E->YY49).val;_LL2: {void*yy=_tmp22F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1133
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY49).tag=57U,(_tmp711.YY49).val=yy1;_tmp711;});}static char _tmp233[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1127 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp233,_tmp233,_tmp233 + 31U};
union Cyc_YYSTYPE*_tmp231=yy1;struct Cyc_List_List*_tmp232;if((((union Cyc_YYSTYPE*)_tmp231)->YY50).tag == 58){_LL1: _tmp232=(_tmp231->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp232;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1134
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY50).tag=58U,(_tmp712.YY50).val=yy1;_tmp712;});}static char _tmp236[11U]="booltype_t";
# 1128 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp236,_tmp236,_tmp236 + 11U};
union Cyc_YYSTYPE*_tmp234=yy1;void*_tmp235;if((((union Cyc_YYSTYPE*)_tmp234)->YY51).tag == 59){_LL1: _tmp235=(_tmp234->YY51).val;_LL2: {void*yy=_tmp235;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1135
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY51).tag=59U,(_tmp713.YY51).val=yy1;_tmp713;});}static char _tmp239[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1129 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp239,_tmp239,_tmp239 + 35U};
union Cyc_YYSTYPE*_tmp237=yy1;struct Cyc_List_List*_tmp238;if((((union Cyc_YYSTYPE*)_tmp237)->YY52).tag == 60){_LL1: _tmp238=(_tmp237->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp238;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1136
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY52).tag=60U,(_tmp714.YY52).val=yy1;_tmp714;});}static char _tmp23C[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1130 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23C,_tmp23C,_tmp23C + 48U};
union Cyc_YYSTYPE*_tmp23A=yy1;struct _tuple28*_tmp23B;if((((union Cyc_YYSTYPE*)_tmp23A)->YY53).tag == 61){_LL1: _tmp23B=(_tmp23A->YY53).val;_LL2: {struct _tuple28*yy=_tmp23B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY53).tag=61U,(_tmp715.YY53).val=yy1;_tmp715;});}static char _tmp23F[18U]="list_t<qvar_t,`H>";
# 1131 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23F,_tmp23F,_tmp23F + 18U};
union Cyc_YYSTYPE*_tmp23D=yy1;struct Cyc_List_List*_tmp23E;if((((union Cyc_YYSTYPE*)_tmp23D)->YY54).tag == 62){_LL1: _tmp23E=(_tmp23D->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp23E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY54).tag=62U,(_tmp716.YY54).val=yy1;_tmp716;});}static char _tmp242[20U]="pointer_qual_t<`yy>";
# 1132 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp242,_tmp242,_tmp242 + 20U};
union Cyc_YYSTYPE*_tmp240=yy1;void*_tmp241;if((((union Cyc_YYSTYPE*)_tmp240)->YY55).tag == 63){_LL1: _tmp241=(_tmp240->YY55).val;_LL2: {void*yy=_tmp241;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1139
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY55).tag=63U,(_tmp717.YY55).val=yy1;_tmp717;});}static char _tmp245[21U]="pointer_quals_t<`yy>";
# 1133 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp245,_tmp245,_tmp245 + 21U};
union Cyc_YYSTYPE*_tmp243=yy1;struct Cyc_List_List*_tmp244;if((((union Cyc_YYSTYPE*)_tmp243)->YY56).tag == 64){_LL1: _tmp244=(_tmp243->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp244;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1140
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY56).tag=64U,(_tmp718.YY56).val=yy1;_tmp718;});}static char _tmp248[10U]="exp_opt_t";
# 1134 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp248,_tmp248,_tmp248 + 10U};
union Cyc_YYSTYPE*_tmp246=yy1;struct Cyc_Absyn_Exp*_tmp247;if((((union Cyc_YYSTYPE*)_tmp246)->YY57).tag == 65){_LL1: _tmp247=(_tmp246->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp247;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp719;(_tmp719.YY57).tag=65U,(_tmp719.YY57).val=yy1;_tmp719;});}static char _tmp24B[10U]="raw_exp_t";
# 1135 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24B,_tmp24B,_tmp24B + 10U};
union Cyc_YYSTYPE*_tmp249=yy1;void*_tmp24A;if((((union Cyc_YYSTYPE*)_tmp249)->YY58).tag == 66){_LL1: _tmp24A=(_tmp249->YY58).val;_LL2: {void*yy=_tmp24A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp71A;(_tmp71A.YY58).tag=66U,(_tmp71A.YY58).val=yy1;_tmp71A;});}static char _tmp24E[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1137 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24E,_tmp24E,_tmp24E + 112U};
union Cyc_YYSTYPE*_tmp24C=yy1;struct _tuple29*_tmp24D;if((((union Cyc_YYSTYPE*)_tmp24C)->YY59).tag == 67){_LL1: _tmp24D=(_tmp24C->YY59).val;_LL2: {struct _tuple29*yy=_tmp24D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1144
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp71B;(_tmp71B.YY59).tag=67U,(_tmp71B.YY59).val=yy1;_tmp71B;});}static char _tmp251[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1138 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp251,_tmp251,_tmp251 + 73U};
union Cyc_YYSTYPE*_tmp24F=yy1;struct _tuple30*_tmp250;if((((union Cyc_YYSTYPE*)_tmp24F)->YY60).tag == 68){_LL1: _tmp250=(_tmp24F->YY60).val;_LL2: {struct _tuple30*yy=_tmp250;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1145
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp71C;(_tmp71C.YY60).tag=68U,(_tmp71C.YY60).val=yy1;_tmp71C;});}static char _tmp254[28U]="list_t<string_t<`H>@`H, `H>";
# 1139 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp254,_tmp254,_tmp254 + 28U};
union Cyc_YYSTYPE*_tmp252=yy1;struct Cyc_List_List*_tmp253;if((((union Cyc_YYSTYPE*)_tmp252)->YY61).tag == 69){_LL1: _tmp253=(_tmp252->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp253;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1146
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71D;(_tmp71D.YY61).tag=69U,(_tmp71D.YY61).val=yy1;_tmp71D;});}static char _tmp257[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1140 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp257,_tmp257,_tmp257 + 38U};
union Cyc_YYSTYPE*_tmp255=yy1;struct Cyc_List_List*_tmp256;if((((union Cyc_YYSTYPE*)_tmp255)->YY62).tag == 70){_LL1: _tmp256=(_tmp255->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp256;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1147
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71E;(_tmp71E.YY62).tag=70U,(_tmp71E.YY62).val=yy1;_tmp71E;});}static char _tmp25A[26U]="$(string_t<`H>, exp_t)@`H";
# 1141 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp25A,_tmp25A,_tmp25A + 26U};
union Cyc_YYSTYPE*_tmp258=yy1;struct _tuple31*_tmp259;if((((union Cyc_YYSTYPE*)_tmp258)->YY63).tag == 71){_LL1: _tmp259=(_tmp258->YY63).val;_LL2: {struct _tuple31*yy=_tmp259;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1148
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp71F;(_tmp71F.YY63).tag=71U,(_tmp71F.YY63).val=yy1;_tmp71F;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1164
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp720;_tmp720.timestamp=0,_tmp720.first_line=0,_tmp720.first_column=0,_tmp720.last_line=0,_tmp720.last_column=0;_tmp720;});}
# 1167
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1178 "parse.y"
static short Cyc_yytranslate[382U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,153,2,2,138,151,148,2,135,136,131,145,130,149,140,150,2,2,2,2,2,2,2,2,2,2,139,127,133,132,134,144,143,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,141,2,142,147,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,128,146,129,152,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126};static char _tmp25B[2U]="$";static char _tmp25C[6U]="error";static char _tmp25D[12U]="$undefined.";static char _tmp25E[5U]="AUTO";static char _tmp25F[9U]="REGISTER";static char _tmp260[7U]="STATIC";static char _tmp261[7U]="EXTERN";static char _tmp262[8U]="TYPEDEF";static char _tmp263[5U]="VOID";static char _tmp264[5U]="CHAR";static char _tmp265[6U]="SHORT";static char _tmp266[4U]="INT";static char _tmp267[5U]="LONG";static char _tmp268[6U]="FLOAT";static char _tmp269[7U]="DOUBLE";static char _tmp26A[7U]="SIGNED";static char _tmp26B[9U]="UNSIGNED";static char _tmp26C[6U]="CONST";static char _tmp26D[9U]="VOLATILE";static char _tmp26E[9U]="RESTRICT";static char _tmp26F[7U]="STRUCT";static char _tmp270[6U]="UNION";static char _tmp271[5U]="CASE";static char _tmp272[8U]="DEFAULT";static char _tmp273[7U]="INLINE";static char _tmp274[7U]="SIZEOF";static char _tmp275[9U]="OFFSETOF";static char _tmp276[3U]="IF";static char _tmp277[5U]="ELSE";static char _tmp278[7U]="SWITCH";static char _tmp279[6U]="WHILE";static char _tmp27A[3U]="DO";static char _tmp27B[4U]="FOR";static char _tmp27C[5U]="GOTO";static char _tmp27D[9U]="CONTINUE";static char _tmp27E[6U]="BREAK";static char _tmp27F[7U]="RETURN";static char _tmp280[5U]="ENUM";static char _tmp281[7U]="TYPEOF";static char _tmp282[16U]="BUILTIN_VA_LIST";static char _tmp283[10U]="EXTENSION";static char _tmp284[8U]="NULL_kw";static char _tmp285[4U]="LET";static char _tmp286[6U]="THROW";static char _tmp287[4U]="TRY";static char _tmp288[6U]="CATCH";static char _tmp289[7U]="EXPORT";static char _tmp28A[9U]="OVERRIDE";static char _tmp28B[5U]="HIDE";static char _tmp28C[4U]="NEW";static char _tmp28D[9U]="ABSTRACT";static char _tmp28E[9U]="FALLTHRU";static char _tmp28F[6U]="USING";static char _tmp290[10U]="NAMESPACE";static char _tmp291[9U]="DATATYPE";static char _tmp292[7U]="MALLOC";static char _tmp293[8U]="RMALLOC";static char _tmp294[15U]="RMALLOC_INLINE";static char _tmp295[7U]="CALLOC";static char _tmp296[8U]="RCALLOC";static char _tmp297[5U]="SWAP";static char _tmp298[5U]="POOL";static char _tmp299[9U]="REGION_T";static char _tmp29A[6U]="TAG_T";static char _tmp29B[7U]="REGION";static char _tmp29C[5U]="RNEW";static char _tmp29D[8U]="REGIONS";static char _tmp29E[7U]="PORTON";static char _tmp29F[8U]="PORTOFF";static char _tmp2A0[7U]="PRAGMA";static char _tmp2A1[10U]="TEMPESTON";static char _tmp2A2[11U]="TEMPESTOFF";static char _tmp2A3[8U]="NUMELTS";static char _tmp2A4[8U]="VALUEOF";static char _tmp2A5[10U]="VALUEOF_T";static char _tmp2A6[9U]="TAGCHECK";static char _tmp2A7[13U]="NUMELTS_QUAL";static char _tmp2A8[10U]="THIN_QUAL";static char _tmp2A9[9U]="FAT_QUAL";static char _tmp2AA[13U]="NOTNULL_QUAL";static char _tmp2AB[14U]="NULLABLE_QUAL";static char _tmp2AC[14U]="REQUIRES_QUAL";static char _tmp2AD[13U]="ENSURES_QUAL";static char _tmp2AE[12U]="REGION_QUAL";static char _tmp2AF[16U]="NOZEROTERM_QUAL";static char _tmp2B0[14U]="ZEROTERM_QUAL";static char _tmp2B1[12U]="TAGGED_QUAL";static char _tmp2B2[12U]="ASSERT_QUAL";static char _tmp2B3[16U]="EXTENSIBLE_QUAL";static char _tmp2B4[18U]="AUTORELEASED_QUAL";static char _tmp2B5[7U]="PTR_OP";static char _tmp2B6[7U]="INC_OP";static char _tmp2B7[7U]="DEC_OP";static char _tmp2B8[8U]="LEFT_OP";static char _tmp2B9[9U]="RIGHT_OP";static char _tmp2BA[6U]="LE_OP";static char _tmp2BB[6U]="GE_OP";static char _tmp2BC[6U]="EQ_OP";static char _tmp2BD[6U]="NE_OP";static char _tmp2BE[7U]="AND_OP";static char _tmp2BF[6U]="OR_OP";static char _tmp2C0[11U]="MUL_ASSIGN";static char _tmp2C1[11U]="DIV_ASSIGN";static char _tmp2C2[11U]="MOD_ASSIGN";static char _tmp2C3[11U]="ADD_ASSIGN";static char _tmp2C4[11U]="SUB_ASSIGN";static char _tmp2C5[12U]="LEFT_ASSIGN";static char _tmp2C6[13U]="RIGHT_ASSIGN";static char _tmp2C7[11U]="AND_ASSIGN";static char _tmp2C8[11U]="XOR_ASSIGN";static char _tmp2C9[10U]="OR_ASSIGN";static char _tmp2CA[9U]="ELLIPSIS";static char _tmp2CB[11U]="LEFT_RIGHT";static char _tmp2CC[12U]="COLON_COLON";static char _tmp2CD[11U]="IDENTIFIER";static char _tmp2CE[17U]="INTEGER_CONSTANT";static char _tmp2CF[7U]="STRING";static char _tmp2D0[8U]="WSTRING";static char _tmp2D1[19U]="CHARACTER_CONSTANT";static char _tmp2D2[20U]="WCHARACTER_CONSTANT";static char _tmp2D3[18U]="FLOATING_CONSTANT";static char _tmp2D4[9U]="TYPE_VAR";static char _tmp2D5[13U]="TYPEDEF_NAME";static char _tmp2D6[16U]="QUAL_IDENTIFIER";static char _tmp2D7[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D8[10U]="ATTRIBUTE";static char _tmp2D9[8U]="ASM_TOK";static char _tmp2DA[4U]="';'";static char _tmp2DB[4U]="'{'";static char _tmp2DC[4U]="'}'";static char _tmp2DD[4U]="','";static char _tmp2DE[4U]="'*'";static char _tmp2DF[4U]="'='";static char _tmp2E0[4U]="'<'";static char _tmp2E1[4U]="'>'";static char _tmp2E2[4U]="'('";static char _tmp2E3[4U]="')'";static char _tmp2E4[4U]="'_'";static char _tmp2E5[4U]="'$'";static char _tmp2E6[4U]="':'";static char _tmp2E7[4U]="'.'";static char _tmp2E8[4U]="'['";static char _tmp2E9[4U]="']'";static char _tmp2EA[4U]="'@'";static char _tmp2EB[4U]="'?'";static char _tmp2EC[4U]="'+'";static char _tmp2ED[4U]="'|'";static char _tmp2EE[4U]="'^'";static char _tmp2EF[4U]="'&'";static char _tmp2F0[4U]="'-'";static char _tmp2F1[4U]="'/'";static char _tmp2F2[4U]="'%'";static char _tmp2F3[4U]="'~'";static char _tmp2F4[4U]="'!'";static char _tmp2F5[5U]="prog";static char _tmp2F6[17U]="translation_unit";static char _tmp2F7[18U]="tempest_on_action";static char _tmp2F8[19U]="tempest_off_action";static char _tmp2F9[16U]="extern_c_action";static char _tmp2FA[13U]="end_extern_c";static char _tmp2FB[14U]="hide_list_opt";static char _tmp2FC[17U]="hide_list_values";static char _tmp2FD[16U]="export_list_opt";static char _tmp2FE[12U]="export_list";static char _tmp2FF[19U]="export_list_values";static char _tmp300[13U]="override_opt";static char _tmp301[21U]="external_declaration";static char _tmp302[15U]="optional_comma";static char _tmp303[20U]="function_definition";static char _tmp304[21U]="function_definition2";static char _tmp305[13U]="using_action";static char _tmp306[15U]="unusing_action";static char _tmp307[17U]="namespace_action";static char _tmp308[19U]="unnamespace_action";static char _tmp309[12U]="declaration";static char _tmp30A[17U]="declaration_list";static char _tmp30B[23U]="declaration_specifiers";static char _tmp30C[24U]="storage_class_specifier";static char _tmp30D[15U]="attributes_opt";static char _tmp30E[11U]="attributes";static char _tmp30F[15U]="attribute_list";static char _tmp310[10U]="attribute";static char _tmp311[15U]="type_specifier";static char _tmp312[25U]="type_specifier_notypedef";static char _tmp313[5U]="kind";static char _tmp314[15U]="type_qualifier";static char _tmp315[15U]="enum_specifier";static char _tmp316[11U]="enum_field";static char _tmp317[22U]="enum_declaration_list";static char _tmp318[26U]="struct_or_union_specifier";static char _tmp319[16U]="type_params_opt";static char _tmp31A[16U]="struct_or_union";static char _tmp31B[24U]="struct_declaration_list";static char _tmp31C[25U]="struct_declaration_list0";static char _tmp31D[21U]="init_declarator_list";static char _tmp31E[22U]="init_declarator_list0";static char _tmp31F[16U]="init_declarator";static char _tmp320[19U]="struct_declaration";static char _tmp321[25U]="specifier_qualifier_list";static char _tmp322[35U]="notypedef_specifier_qualifier_list";static char _tmp323[23U]="struct_declarator_list";static char _tmp324[24U]="struct_declarator_list0";static char _tmp325[18U]="struct_declarator";static char _tmp326[20U]="requires_clause_opt";static char _tmp327[19U]="ensures_clause_opt";static char _tmp328[19U]="datatype_specifier";static char _tmp329[14U]="qual_datatype";static char _tmp32A[19U]="datatypefield_list";static char _tmp32B[20U]="datatypefield_scope";static char _tmp32C[14U]="datatypefield";static char _tmp32D[11U]="declarator";static char _tmp32E[23U]="declarator_withtypedef";static char _tmp32F[18U]="direct_declarator";static char _tmp330[30U]="direct_declarator_withtypedef";static char _tmp331[8U]="pointer";static char _tmp332[12U]="one_pointer";static char _tmp333[14U]="pointer_quals";static char _tmp334[13U]="pointer_qual";static char _tmp335[23U]="pointer_null_and_bound";static char _tmp336[14U]="pointer_bound";static char _tmp337[18U]="zeroterm_qual_opt";static char _tmp338[8U]="rgn_opt";static char _tmp339[11U]="tqual_list";static char _tmp33A[20U]="parameter_type_list";static char _tmp33B[9U]="type_var";static char _tmp33C[16U]="optional_effect";static char _tmp33D[19U]="optional_rgn_order";static char _tmp33E[10U]="rgn_order";static char _tmp33F[16U]="optional_inject";static char _tmp340[11U]="effect_set";static char _tmp341[14U]="atomic_effect";static char _tmp342[11U]="region_set";static char _tmp343[15U]="parameter_list";static char _tmp344[22U]="parameter_declaration";static char _tmp345[16U]="identifier_list";static char _tmp346[17U]="identifier_list0";static char _tmp347[12U]="initializer";static char _tmp348[18U]="array_initializer";static char _tmp349[17U]="initializer_list";static char _tmp34A[12U]="designation";static char _tmp34B[16U]="designator_list";static char _tmp34C[11U]="designator";static char _tmp34D[10U]="type_name";static char _tmp34E[14U]="any_type_name";static char _tmp34F[15U]="type_name_list";static char _tmp350[20U]="abstract_declarator";static char _tmp351[27U]="direct_abstract_declarator";static char _tmp352[10U]="statement";static char _tmp353[18U]="labeled_statement";static char _tmp354[21U]="expression_statement";static char _tmp355[19U]="compound_statement";static char _tmp356[16U]="block_item_list";static char _tmp357[20U]="selection_statement";static char _tmp358[15U]="switch_clauses";static char _tmp359[20U]="iteration_statement";static char _tmp35A[15U]="jump_statement";static char _tmp35B[12U]="exp_pattern";static char _tmp35C[20U]="conditional_pattern";static char _tmp35D[19U]="logical_or_pattern";static char _tmp35E[20U]="logical_and_pattern";static char _tmp35F[21U]="inclusive_or_pattern";static char _tmp360[21U]="exclusive_or_pattern";static char _tmp361[12U]="and_pattern";static char _tmp362[17U]="equality_pattern";static char _tmp363[19U]="relational_pattern";static char _tmp364[14U]="shift_pattern";static char _tmp365[17U]="additive_pattern";static char _tmp366[23U]="multiplicative_pattern";static char _tmp367[13U]="cast_pattern";static char _tmp368[14U]="unary_pattern";static char _tmp369[16U]="postfix_pattern";static char _tmp36A[16U]="primary_pattern";static char _tmp36B[8U]="pattern";static char _tmp36C[19U]="tuple_pattern_list";static char _tmp36D[20U]="tuple_pattern_list0";static char _tmp36E[14U]="field_pattern";static char _tmp36F[19U]="field_pattern_list";static char _tmp370[20U]="field_pattern_list0";static char _tmp371[11U]="expression";static char _tmp372[22U]="assignment_expression";static char _tmp373[20U]="assignment_operator";static char _tmp374[23U]="conditional_expression";static char _tmp375[20U]="constant_expression";static char _tmp376[22U]="logical_or_expression";static char _tmp377[23U]="logical_and_expression";static char _tmp378[24U]="inclusive_or_expression";static char _tmp379[24U]="exclusive_or_expression";static char _tmp37A[15U]="and_expression";static char _tmp37B[20U]="equality_expression";static char _tmp37C[22U]="relational_expression";static char _tmp37D[17U]="shift_expression";static char _tmp37E[20U]="additive_expression";static char _tmp37F[26U]="multiplicative_expression";static char _tmp380[16U]="cast_expression";static char _tmp381[17U]="unary_expression";static char _tmp382[9U]="asm_expr";static char _tmp383[13U]="volatile_opt";static char _tmp384[12U]="asm_out_opt";static char _tmp385[12U]="asm_outlist";static char _tmp386[11U]="asm_in_opt";static char _tmp387[11U]="asm_inlist";static char _tmp388[11U]="asm_io_elt";static char _tmp389[16U]="asm_clobber_opt";static char _tmp38A[17U]="asm_clobber_list";static char _tmp38B[15U]="unary_operator";static char _tmp38C[19U]="postfix_expression";static char _tmp38D[17U]="field_expression";static char _tmp38E[19U]="primary_expression";static char _tmp38F[25U]="argument_expression_list";static char _tmp390[26U]="argument_expression_list0";static char _tmp391[9U]="constant";static char _tmp392[20U]="qual_opt_identifier";static char _tmp393[17U]="qual_opt_typedef";static char _tmp394[18U]="struct_union_name";static char _tmp395[11U]="field_name";static char _tmp396[12U]="right_angle";
# 1551 "parse.y"
static struct _fat_ptr Cyc_yytname[316U]={{_tmp25B,_tmp25B,_tmp25B + 2U},{_tmp25C,_tmp25C,_tmp25C + 6U},{_tmp25D,_tmp25D,_tmp25D + 12U},{_tmp25E,_tmp25E,_tmp25E + 5U},{_tmp25F,_tmp25F,_tmp25F + 9U},{_tmp260,_tmp260,_tmp260 + 7U},{_tmp261,_tmp261,_tmp261 + 7U},{_tmp262,_tmp262,_tmp262 + 8U},{_tmp263,_tmp263,_tmp263 + 5U},{_tmp264,_tmp264,_tmp264 + 5U},{_tmp265,_tmp265,_tmp265 + 6U},{_tmp266,_tmp266,_tmp266 + 4U},{_tmp267,_tmp267,_tmp267 + 5U},{_tmp268,_tmp268,_tmp268 + 6U},{_tmp269,_tmp269,_tmp269 + 7U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 9U},{_tmp26C,_tmp26C,_tmp26C + 6U},{_tmp26D,_tmp26D,_tmp26D + 9U},{_tmp26E,_tmp26E,_tmp26E + 9U},{_tmp26F,_tmp26F,_tmp26F + 7U},{_tmp270,_tmp270,_tmp270 + 6U},{_tmp271,_tmp271,_tmp271 + 5U},{_tmp272,_tmp272,_tmp272 + 8U},{_tmp273,_tmp273,_tmp273 + 7U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 9U},{_tmp276,_tmp276,_tmp276 + 3U},{_tmp277,_tmp277,_tmp277 + 5U},{_tmp278,_tmp278,_tmp278 + 7U},{_tmp279,_tmp279,_tmp279 + 6U},{_tmp27A,_tmp27A,_tmp27A + 3U},{_tmp27B,_tmp27B,_tmp27B + 4U},{_tmp27C,_tmp27C,_tmp27C + 5U},{_tmp27D,_tmp27D,_tmp27D + 9U},{_tmp27E,_tmp27E,_tmp27E + 6U},{_tmp27F,_tmp27F,_tmp27F + 7U},{_tmp280,_tmp280,_tmp280 + 5U},{_tmp281,_tmp281,_tmp281 + 7U},{_tmp282,_tmp282,_tmp282 + 16U},{_tmp283,_tmp283,_tmp283 + 10U},{_tmp284,_tmp284,_tmp284 + 8U},{_tmp285,_tmp285,_tmp285 + 4U},{_tmp286,_tmp286,_tmp286 + 6U},{_tmp287,_tmp287,_tmp287 + 4U},{_tmp288,_tmp288,_tmp288 + 6U},{_tmp289,_tmp289,_tmp289 + 7U},{_tmp28A,_tmp28A,_tmp28A + 9U},{_tmp28B,_tmp28B,_tmp28B + 5U},{_tmp28C,_tmp28C,_tmp28C + 4U},{_tmp28D,_tmp28D,_tmp28D + 9U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 6U},{_tmp290,_tmp290,_tmp290 + 10U},{_tmp291,_tmp291,_tmp291 + 9U},{_tmp292,_tmp292,_tmp292 + 7U},{_tmp293,_tmp293,_tmp293 + 8U},{_tmp294,_tmp294,_tmp294 + 15U},{_tmp295,_tmp295,_tmp295 + 7U},{_tmp296,_tmp296,_tmp296 + 8U},{_tmp297,_tmp297,_tmp297 + 5U},{_tmp298,_tmp298,_tmp298 + 5U},{_tmp299,_tmp299,_tmp299 + 9U},{_tmp29A,_tmp29A,_tmp29A + 6U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 5U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 8U},{_tmp2A0,_tmp2A0,_tmp2A0 + 7U},{_tmp2A1,_tmp2A1,_tmp2A1 + 10U},{_tmp2A2,_tmp2A2,_tmp2A2 + 11U},{_tmp2A3,_tmp2A3,_tmp2A3 + 8U},{_tmp2A4,_tmp2A4,_tmp2A4 + 8U},{_tmp2A5,_tmp2A5,_tmp2A5 + 10U},{_tmp2A6,_tmp2A6,_tmp2A6 + 9U},{_tmp2A7,_tmp2A7,_tmp2A7 + 13U},{_tmp2A8,_tmp2A8,_tmp2A8 + 10U},{_tmp2A9,_tmp2A9,_tmp2A9 + 9U},{_tmp2AA,_tmp2AA,_tmp2AA + 13U},{_tmp2AB,_tmp2AB,_tmp2AB + 14U},{_tmp2AC,_tmp2AC,_tmp2AC + 14U},{_tmp2AD,_tmp2AD,_tmp2AD + 13U},{_tmp2AE,_tmp2AE,_tmp2AE + 12U},{_tmp2AF,_tmp2AF,_tmp2AF + 16U},{_tmp2B0,_tmp2B0,_tmp2B0 + 14U},{_tmp2B1,_tmp2B1,_tmp2B1 + 12U},{_tmp2B2,_tmp2B2,_tmp2B2 + 12U},{_tmp2B3,_tmp2B3,_tmp2B3 + 16U},{_tmp2B4,_tmp2B4,_tmp2B4 + 18U},{_tmp2B5,_tmp2B5,_tmp2B5 + 7U},{_tmp2B6,_tmp2B6,_tmp2B6 + 7U},{_tmp2B7,_tmp2B7,_tmp2B7 + 7U},{_tmp2B8,_tmp2B8,_tmp2B8 + 8U},{_tmp2B9,_tmp2B9,_tmp2B9 + 9U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 6U},{_tmp2BC,_tmp2BC,_tmp2BC + 6U},{_tmp2BD,_tmp2BD,_tmp2BD + 6U},{_tmp2BE,_tmp2BE,_tmp2BE + 7U},{_tmp2BF,_tmp2BF,_tmp2BF + 6U},{_tmp2C0,_tmp2C0,_tmp2C0 + 11U},{_tmp2C1,_tmp2C1,_tmp2C1 + 11U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 11U},{_tmp2C5,_tmp2C5,_tmp2C5 + 12U},{_tmp2C6,_tmp2C6,_tmp2C6 + 13U},{_tmp2C7,_tmp2C7,_tmp2C7 + 11U},{_tmp2C8,_tmp2C8,_tmp2C8 + 11U},{_tmp2C9,_tmp2C9,_tmp2C9 + 10U},{_tmp2CA,_tmp2CA,_tmp2CA + 9U},{_tmp2CB,_tmp2CB,_tmp2CB + 11U},{_tmp2CC,_tmp2CC,_tmp2CC + 12U},{_tmp2CD,_tmp2CD,_tmp2CD + 11U},{_tmp2CE,_tmp2CE,_tmp2CE + 17U},{_tmp2CF,_tmp2CF,_tmp2CF + 7U},{_tmp2D0,_tmp2D0,_tmp2D0 + 8U},{_tmp2D1,_tmp2D1,_tmp2D1 + 19U},{_tmp2D2,_tmp2D2,_tmp2D2 + 20U},{_tmp2D3,_tmp2D3,_tmp2D3 + 18U},{_tmp2D4,_tmp2D4,_tmp2D4 + 9U},{_tmp2D5,_tmp2D5,_tmp2D5 + 13U},{_tmp2D6,_tmp2D6,_tmp2D6 + 16U},{_tmp2D7,_tmp2D7,_tmp2D7 + 18U},{_tmp2D8,_tmp2D8,_tmp2D8 + 10U},{_tmp2D9,_tmp2D9,_tmp2D9 + 8U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 4U},{_tmp2F4,_tmp2F4,_tmp2F4 + 4U},{_tmp2F5,_tmp2F5,_tmp2F5 + 5U},{_tmp2F6,_tmp2F6,_tmp2F6 + 17U},{_tmp2F7,_tmp2F7,_tmp2F7 + 18U},{_tmp2F8,_tmp2F8,_tmp2F8 + 19U},{_tmp2F9,_tmp2F9,_tmp2F9 + 16U},{_tmp2FA,_tmp2FA,_tmp2FA + 13U},{_tmp2FB,_tmp2FB,_tmp2FB + 14U},{_tmp2FC,_tmp2FC,_tmp2FC + 17U},{_tmp2FD,_tmp2FD,_tmp2FD + 16U},{_tmp2FE,_tmp2FE,_tmp2FE + 12U},{_tmp2FF,_tmp2FF,_tmp2FF + 19U},{_tmp300,_tmp300,_tmp300 + 13U},{_tmp301,_tmp301,_tmp301 + 21U},{_tmp302,_tmp302,_tmp302 + 15U},{_tmp303,_tmp303,_tmp303 + 20U},{_tmp304,_tmp304,_tmp304 + 21U},{_tmp305,_tmp305,_tmp305 + 13U},{_tmp306,_tmp306,_tmp306 + 15U},{_tmp307,_tmp307,_tmp307 + 17U},{_tmp308,_tmp308,_tmp308 + 19U},{_tmp309,_tmp309,_tmp309 + 12U},{_tmp30A,_tmp30A,_tmp30A + 17U},{_tmp30B,_tmp30B,_tmp30B + 23U},{_tmp30C,_tmp30C,_tmp30C + 24U},{_tmp30D,_tmp30D,_tmp30D + 15U},{_tmp30E,_tmp30E,_tmp30E + 11U},{_tmp30F,_tmp30F,_tmp30F + 15U},{_tmp310,_tmp310,_tmp310 + 10U},{_tmp311,_tmp311,_tmp311 + 15U},{_tmp312,_tmp312,_tmp312 + 25U},{_tmp313,_tmp313,_tmp313 + 5U},{_tmp314,_tmp314,_tmp314 + 15U},{_tmp315,_tmp315,_tmp315 + 15U},{_tmp316,_tmp316,_tmp316 + 11U},{_tmp317,_tmp317,_tmp317 + 22U},{_tmp318,_tmp318,_tmp318 + 26U},{_tmp319,_tmp319,_tmp319 + 16U},{_tmp31A,_tmp31A,_tmp31A + 16U},{_tmp31B,_tmp31B,_tmp31B + 24U},{_tmp31C,_tmp31C,_tmp31C + 25U},{_tmp31D,_tmp31D,_tmp31D + 21U},{_tmp31E,_tmp31E,_tmp31E + 22U},{_tmp31F,_tmp31F,_tmp31F + 16U},{_tmp320,_tmp320,_tmp320 + 19U},{_tmp321,_tmp321,_tmp321 + 25U},{_tmp322,_tmp322,_tmp322 + 35U},{_tmp323,_tmp323,_tmp323 + 23U},{_tmp324,_tmp324,_tmp324 + 24U},{_tmp325,_tmp325,_tmp325 + 18U},{_tmp326,_tmp326,_tmp326 + 20U},{_tmp327,_tmp327,_tmp327 + 19U},{_tmp328,_tmp328,_tmp328 + 19U},{_tmp329,_tmp329,_tmp329 + 14U},{_tmp32A,_tmp32A,_tmp32A + 19U},{_tmp32B,_tmp32B,_tmp32B + 20U},{_tmp32C,_tmp32C,_tmp32C + 14U},{_tmp32D,_tmp32D,_tmp32D + 11U},{_tmp32E,_tmp32E,_tmp32E + 23U},{_tmp32F,_tmp32F,_tmp32F + 18U},{_tmp330,_tmp330,_tmp330 + 30U},{_tmp331,_tmp331,_tmp331 + 8U},{_tmp332,_tmp332,_tmp332 + 12U},{_tmp333,_tmp333,_tmp333 + 14U},{_tmp334,_tmp334,_tmp334 + 13U},{_tmp335,_tmp335,_tmp335 + 23U},{_tmp336,_tmp336,_tmp336 + 14U},{_tmp337,_tmp337,_tmp337 + 18U},{_tmp338,_tmp338,_tmp338 + 8U},{_tmp339,_tmp339,_tmp339 + 11U},{_tmp33A,_tmp33A,_tmp33A + 20U},{_tmp33B,_tmp33B,_tmp33B + 9U},{_tmp33C,_tmp33C,_tmp33C + 16U},{_tmp33D,_tmp33D,_tmp33D + 19U},{_tmp33E,_tmp33E,_tmp33E + 10U},{_tmp33F,_tmp33F,_tmp33F + 16U},{_tmp340,_tmp340,_tmp340 + 11U},{_tmp341,_tmp341,_tmp341 + 14U},{_tmp342,_tmp342,_tmp342 + 11U},{_tmp343,_tmp343,_tmp343 + 15U},{_tmp344,_tmp344,_tmp344 + 22U},{_tmp345,_tmp345,_tmp345 + 16U},{_tmp346,_tmp346,_tmp346 + 17U},{_tmp347,_tmp347,_tmp347 + 12U},{_tmp348,_tmp348,_tmp348 + 18U},{_tmp349,_tmp349,_tmp349 + 17U},{_tmp34A,_tmp34A,_tmp34A + 12U},{_tmp34B,_tmp34B,_tmp34B + 16U},{_tmp34C,_tmp34C,_tmp34C + 11U},{_tmp34D,_tmp34D,_tmp34D + 10U},{_tmp34E,_tmp34E,_tmp34E + 14U},{_tmp34F,_tmp34F,_tmp34F + 15U},{_tmp350,_tmp350,_tmp350 + 20U},{_tmp351,_tmp351,_tmp351 + 27U},{_tmp352,_tmp352,_tmp352 + 10U},{_tmp353,_tmp353,_tmp353 + 18U},{_tmp354,_tmp354,_tmp354 + 21U},{_tmp355,_tmp355,_tmp355 + 19U},{_tmp356,_tmp356,_tmp356 + 16U},{_tmp357,_tmp357,_tmp357 + 20U},{_tmp358,_tmp358,_tmp358 + 15U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 15U},{_tmp35B,_tmp35B,_tmp35B + 12U},{_tmp35C,_tmp35C,_tmp35C + 20U},{_tmp35D,_tmp35D,_tmp35D + 19U},{_tmp35E,_tmp35E,_tmp35E + 20U},{_tmp35F,_tmp35F,_tmp35F + 21U},{_tmp360,_tmp360,_tmp360 + 21U},{_tmp361,_tmp361,_tmp361 + 12U},{_tmp362,_tmp362,_tmp362 + 17U},{_tmp363,_tmp363,_tmp363 + 19U},{_tmp364,_tmp364,_tmp364 + 14U},{_tmp365,_tmp365,_tmp365 + 17U},{_tmp366,_tmp366,_tmp366 + 23U},{_tmp367,_tmp367,_tmp367 + 13U},{_tmp368,_tmp368,_tmp368 + 14U},{_tmp369,_tmp369,_tmp369 + 16U},{_tmp36A,_tmp36A,_tmp36A + 16U},{_tmp36B,_tmp36B,_tmp36B + 8U},{_tmp36C,_tmp36C,_tmp36C + 19U},{_tmp36D,_tmp36D,_tmp36D + 20U},{_tmp36E,_tmp36E,_tmp36E + 14U},{_tmp36F,_tmp36F,_tmp36F + 19U},{_tmp370,_tmp370,_tmp370 + 20U},{_tmp371,_tmp371,_tmp371 + 11U},{_tmp372,_tmp372,_tmp372 + 22U},{_tmp373,_tmp373,_tmp373 + 20U},{_tmp374,_tmp374,_tmp374 + 23U},{_tmp375,_tmp375,_tmp375 + 20U},{_tmp376,_tmp376,_tmp376 + 22U},{_tmp377,_tmp377,_tmp377 + 23U},{_tmp378,_tmp378,_tmp378 + 24U},{_tmp379,_tmp379,_tmp379 + 24U},{_tmp37A,_tmp37A,_tmp37A + 15U},{_tmp37B,_tmp37B,_tmp37B + 20U},{_tmp37C,_tmp37C,_tmp37C + 22U},{_tmp37D,_tmp37D,_tmp37D + 17U},{_tmp37E,_tmp37E,_tmp37E + 20U},{_tmp37F,_tmp37F,_tmp37F + 26U},{_tmp380,_tmp380,_tmp380 + 16U},{_tmp381,_tmp381,_tmp381 + 17U},{_tmp382,_tmp382,_tmp382 + 9U},{_tmp383,_tmp383,_tmp383 + 13U},{_tmp384,_tmp384,_tmp384 + 12U},{_tmp385,_tmp385,_tmp385 + 12U},{_tmp386,_tmp386,_tmp386 + 11U},{_tmp387,_tmp387,_tmp387 + 11U},{_tmp388,_tmp388,_tmp388 + 11U},{_tmp389,_tmp389,_tmp389 + 16U},{_tmp38A,_tmp38A,_tmp38A + 17U},{_tmp38B,_tmp38B,_tmp38B + 15U},{_tmp38C,_tmp38C,_tmp38C + 19U},{_tmp38D,_tmp38D,_tmp38D + 17U},{_tmp38E,_tmp38E,_tmp38E + 19U},{_tmp38F,_tmp38F,_tmp38F + 25U},{_tmp390,_tmp390,_tmp390 + 26U},{_tmp391,_tmp391,_tmp391 + 9U},{_tmp392,_tmp392,_tmp392 + 20U},{_tmp393,_tmp393,_tmp393 + 17U},{_tmp394,_tmp394,_tmp394 + 18U},{_tmp395,_tmp395,_tmp395 + 11U},{_tmp396,_tmp396,_tmp396 + 12U}};
# 1609
static short Cyc_yyr1[564U]={0,154,155,155,155,155,155,155,155,155,155,155,155,156,157,158,159,160,160,161,161,161,162,162,163,163,163,164,164,164,165,165,166,166,166,167,167,168,168,168,168,169,169,170,171,172,173,174,174,174,174,174,174,174,174,175,175,176,176,176,176,176,176,176,176,176,176,176,177,177,177,177,177,177,177,178,178,179,180,180,181,181,181,181,182,182,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,185,185,185,186,186,186,187,187,188,188,188,189,189,189,189,189,190,190,191,191,192,192,193,193,194,195,195,196,196,197,198,198,198,198,198,198,199,199,199,199,199,199,200,201,201,202,202,202,202,203,203,204,204,205,205,205,206,206,207,207,207,207,208,208,208,209,209,210,210,211,211,212,212,212,212,212,212,212,212,212,212,213,213,213,213,213,213,213,213,213,213,213,214,214,215,216,216,217,217,217,217,217,217,217,217,217,218,218,218,219,219,220,220,220,221,221,221,222,222,223,223,223,223,224,224,225,225,226,226,227,227,228,228,229,229,230,230,230,230,231,231,232,232,233,233,233,234,235,235,236,236,237,237,237,237,237,238,238,238,238,239,239,240,240,241,241,242,242,243,243,243,243,243,244,244,245,245,245,246,246,246,246,246,246,246,246,246,246,246,247,247,247,247,247,247,248,249,249,250,250,251,251,251,251,251,251,251,251,252,252,252,252,252,252,253,253,253,253,253,253,254,254,254,254,254,254,254,254,254,254,254,254,254,254,255,255,255,255,255,255,255,255,256,257,257,258,258,259,259,260,260,261,261,262,262,263,263,263,264,264,264,264,264,265,265,265,266,266,266,267,267,267,267,268,268,269,269,269,269,269,269,270,271,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,273,273,273,274,274,275,275,276,276,276,277,277,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,280,281,281,281,281,281,281,281,282,283,283,284,284,285,285,286,286,287,287,288,288,288,289,289,289,289,289,290,290,290,291,291,291,292,292,292,292,293,293,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,294,295,296,296,297,297,297,298,298,299,299,299,300,300,301,302,302,302,303,303,304,304,304,305,305,305,305,305,305,305,305,305,305,305,306,306,306,306,307,307,307,307,307,307,307,307,307,307,307,308,309,309,310,310,310,310,310,311,311,312,312,313,313,314,314,315,315};
# 1669
static short Cyc_yyr2[564U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1729
static short Cyc_yydefact[1143U]={0,34,68,69,70,71,73,86,87,88,89,90,91,92,93,94,110,111,112,128,129,64,0,0,98,0,0,74,0,0,166,0,105,107,0,0,0,13,14,0,0,0,554,233,556,555,557,0,219,0,101,0,219,218,1,0,0,0,0,32,0,0,33,0,57,66,60,84,62,95,96,0,99,0,0,177,0,202,205,100,181,126,72,71,65,0,114,0,59,553,0,554,549,550,551,552,0,126,0,0,393,0,0,0,256,0,395,396,43,45,0,0,0,0,0,0,0,0,0,167,0,0,0,216,0,0,0,0,217,0,0,0,2,0,0,0,0,47,0,134,135,137,58,67,61,63,130,558,559,126,126,0,55,0,0,36,0,235,0,190,178,203,0,209,210,214,215,0,213,212,211,224,205,0,85,72,118,0,116,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,538,539,499,0,0,0,0,0,519,517,518,0,422,424,438,446,448,450,452,454,456,459,464,467,470,474,0,476,520,537,535,554,405,0,0,0,0,406,0,0,404,50,0,0,126,0,0,0,0,144,140,142,276,278,0,0,52,0,0,8,9,0,126,560,561,234,109,0,0,0,182,102,254,0,251,10,11,0,3,0,5,0,48,0,0,0,36,0,131,132,157,125,0,164,0,0,0,0,0,0,0,0,0,0,0,0,554,306,308,0,316,310,0,314,299,300,301,0,302,303,304,0,56,36,137,35,37,283,0,241,257,0,0,237,235,0,221,0,0,0,226,75,225,206,0,119,115,0,0,0,484,0,0,496,440,474,0,441,442,0,0,0,0,0,0,0,0,0,0,0,477,478,500,495,0,480,0,0,0,0,481,479,0,97,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,428,429,430,431,432,433,434,435,436,437,427,0,482,0,526,527,0,0,0,541,0,126,397,0,0,0,419,554,561,0,0,0,0,272,415,420,0,417,0,0,394,412,413,0,410,258,0,0,0,0,0,279,0,249,145,150,146,148,141,143,235,0,285,277,286,563,562,0,104,106,0,0,108,124,81,80,0,78,220,183,235,253,179,285,255,191,192,0,103,16,30,44,0,46,0,136,138,260,259,36,38,121,133,0,0,0,152,153,160,0,126,126,172,0,0,0,0,0,554,0,0,0,345,346,347,0,0,349,0,0,0,317,311,137,315,309,307,39,0,189,242,0,0,0,248,236,243,160,0,0,0,237,188,223,222,184,221,0,0,227,76,127,120,445,117,113,0,0,0,0,554,261,266,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,540,547,0,546,423,447,0,449,451,453,455,457,458,462,463,460,461,465,466,468,469,471,472,473,426,425,525,522,0,524,0,0,0,408,409,0,275,0,416,270,273,403,0,271,407,400,0,49,0,401,54,0,280,0,151,147,149,0,237,0,221,0,287,0,235,0,298,282,0,0,126,0,0,0,144,0,126,0,235,0,201,180,252,0,22,4,6,40,0,156,139,157,0,0,155,237,165,174,173,0,0,168,0,0,0,324,0,0,0,0,0,0,344,348,0,0,0,312,305,0,41,284,235,0,245,0,0,162,238,0,160,241,229,185,207,208,227,204,483,0,0,0,262,0,267,486,0,0,0,0,0,536,491,494,0,0,497,501,0,0,475,543,0,0,523,521,0,0,0,0,274,418,421,411,414,402,281,250,160,0,288,289,221,0,0,237,221,0,0,51,237,554,0,77,79,0,193,0,0,237,0,221,0,0,0,17,23,154,0,158,130,163,175,172,172,0,0,0,0,0,0,0,0,0,0,0,0,0,324,350,0,313,42,237,0,246,244,0,186,0,162,237,0,228,532,0,531,0,263,268,0,0,0,0,0,443,444,525,524,506,0,545,528,0,548,439,542,544,0,398,399,162,160,291,297,160,0,290,221,0,130,0,82,194,200,160,0,199,195,221,0,0,0,0,0,0,0,171,170,318,324,0,0,0,0,0,0,352,353,355,357,359,361,363,365,368,373,376,379,383,385,391,392,0,0,321,330,0,0,0,0,0,0,0,0,0,0,351,231,247,0,239,187,230,235,485,0,0,269,487,488,0,0,493,492,0,512,506,502,504,498,529,0,294,162,162,160,292,53,0,0,162,160,196,31,25,0,0,27,0,7,159,123,0,0,0,324,0,389,0,0,386,324,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,387,324,0,332,0,0,0,340,0,0,0,0,0,0,323,0,0,237,534,533,0,0,0,0,513,512,509,507,0,503,530,293,296,162,122,0,197,162,26,24,28,0,0,19,176,319,320,0,0,0,0,324,326,356,0,358,360,362,364,366,367,371,372,369,370,374,375,377,378,380,381,382,0,325,331,333,334,0,342,341,0,336,0,0,0,161,240,232,0,0,0,0,0,515,514,0,508,505,295,0,198,29,18,20,0,322,388,0,384,327,0,324,335,343,337,338,0,265,264,489,0,511,0,510,83,21,0,354,324,328,339,0,516,390,329,490,0,0,0};
# 1847
static short Cyc_yydefgoto[162U]={1140,54,55,56,57,492,884,1046,796,797,968,675,58,324,59,308,60,494,61,496,62,153,63,64,561,247,478,479,248,67,263,249,69,176,177,70,174,71,285,286,138,139,140,287,250,460,507,508,509,685,829,72,73,690,691,692,74,510,75,484,76,77,171,172,78,123,557,339,728,648,79,649,551,719,543,547,548,454,332,272,103,104,574,499,575,433,434,435,251,325,326,650,466,311,312,313,314,315,316,811,317,318,897,898,899,900,901,902,903,904,905,906,907,908,909,910,911,912,436,445,446,437,438,439,319,210,413,211,566,212,213,214,215,216,217,218,219,220,221,222,223,371,372,851,948,949,1029,950,1031,1098,224,225,836,226,593,594,227,228,81,969,440,470};
# 1867
static short Cyc_yypact[1143U]={3073,- -32768,- -32768,- -32768,- -32768,- 107,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3724,206,34,- -32768,3724,3283,- -32768,236,88,- -32768,- 48,101,114,108,142,152,- -32768,- -32768,146,374,244,- -32768,192,- -32768,- -32768,- -32768,183,202,147,225,218,202,- -32768,- -32768,248,282,288,2928,- -32768,432,524,- -32768,253,3724,3724,3724,- -32768,3724,- -32768,- -32768,577,- -32768,236,3632,258,162,333,1025,- -32768,- -32768,306,321,338,- -32768,236,352,7080,- -32768,- -32768,3335,442,- -32768,- -32768,- -32768,- -32768,373,306,399,7080,- -32768,383,3335,405,446,451,- -32768,164,- -32768,- -32768,502,3886,3886,117,513,2928,2928,7080,241,- -32768,123,503,7080,- -32768,214,545,123,4291,- -32768,2928,2928,3217,- -32768,2928,3217,2928,3217,- -32768,561,567,- -32768,3496,- -32768,- -32768,- -32768,- -32768,4291,- -32768,- -32768,306,228,1878,- -32768,3632,253,580,3886,3805,5142,- -32768,258,- -32768,610,- -32768,- -32768,- -32768,- -32768,627,- -32768,- -32768,- -32768,- 87,1025,3886,- -32768,- -32768,584,601,650,236,7491,649,7590,7080,7293,651,656,675,683,702,711,718,723,725,757,762,7590,7590,- -32768,- -32768,780,7689,2628,763,7689,7689,- -32768,- -32768,- -32768,262,- -32768,- -32768,- 39,736,722,752,753,677,83,735,343,- 24,- -32768,1171,7689,320,11,- -32768,776,153,- -32768,3335,- 49,786,1123,792,337,3034,- -32768,- -32768,795,7080,306,3034,774,777,4048,4291,4372,4291,542,- -32768,- 35,- 35,- -32768,799,783,- -32768,- -32768,357,306,- -32768,- -32768,- -32768,- -32768,25,790,778,- -32768,- -32768,613,456,- -32768,- -32768,- -32768,793,- -32768,796,- -32768,797,- -32768,214,5256,3632,580,798,4291,- -32768,673,801,236,802,788,92,789,4443,805,807,804,806,5370,2482,4443,168,803,- -32768,- -32768,808,2029,2029,253,2029,- -32768,- -32768,- -32768,812,- -32768,- -32768,- -32768,74,- -32768,580,800,- -32768,- -32768,809,77,831,- -32768,29,810,813,297,815,759,811,7080,3886,- -32768,823,- -32768,- -32768,77,236,- -32768,7080,826,2628,- -32768,4291,2628,- -32768,- -32768,- -32768,4557,- -32768,857,7080,7080,7080,7080,7080,7080,846,7080,4291,953,7080,- -32768,- -32768,- -32768,- -32768,827,- -32768,2029,825,491,7080,- -32768,- -32768,7080,- -32768,7689,7080,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7080,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7080,- -32768,123,- -32768,- -32768,5484,123,7080,- -32768,830,306,- -32768,835,836,841,- -32768,60,373,123,7080,3335,249,- -32768,- -32768,- -32768,839,844,837,3335,- -32768,- -32768,- -32768,843,852,- -32768,335,1123,845,858,3886,- -32768,855,860,- -32768,4372,4372,4372,- -32768,- -32768,1464,5598,498,- -32768,295,- -32768,- -32768,29,- -32768,- -32768,863,873,- -32768,865,- -32768,864,859,871,- -32768,- -32768,1300,- -32768,312,328,- -32768,- -32768,- -32768,4291,- -32768,- -32768,955,- -32768,2928,- -32768,2928,- -32768,- -32768,- -32768,- -32768,580,- -32768,- -32768,- -32768,646,7080,876,874,- -32768,47,544,306,306,851,7080,7080,870,879,7080,872,979,2331,885,- -32768,- -32768,- -32768,381,968,- -32768,5712,2180,2774,- -32768,- -32768,3496,- -32768,- -32768,- -32768,- -32768,3886,- -32768,- -32768,4291,882,4129,- -32768,- -32768,869,938,29,884,4210,813,- -32768,- -32768,- -32768,- -32768,759,887,242,822,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,891,899,894,907,892,- -32768,- -32768,744,5256,897,905,906,910,913,534,901,908,912,410,930,958,946,7392,- -32768,- -32768,941,949,- -32768,736,292,722,752,753,677,83,83,735,735,735,735,343,343,- 24,- 24,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,944,- -32768,67,3886,5025,4291,- -32768,4291,- -32768,939,- -32768,- -32768,- -32768,- -32768,2889,- -32768,- -32768,- -32768,3178,- -32768,954,- -32768,- -32768,336,- -32768,4291,- -32768,- -32768,- -32768,948,813,956,759,940,295,3886,3967,5826,- -32768,- -32768,7080,966,306,7194,959,25,3354,960,306,3886,3805,5940,- -32768,312,- -32768,957,1048,- -32768,- -32768,- -32768,728,- -32768,- -32768,673,963,7080,- -32768,813,- -32768,- -32768,- -32768,971,236,565,535,546,7080,842,588,4443,976,6054,6168,579,- -32768,- -32768,984,990,982,2029,- -32768,3632,- -32768,809,992,3886,- -32768,993,29,1039,- -32768,989,938,208,- -32768,- -32768,- -32768,- -32768,822,- -32768,997,467,997,994,- -32768,4674,- -32768,- -32768,7080,7080,1104,7080,7293,- -32768,- -32768,- -32768,123,123,- -32768,991,999,4791,- -32768,- -32768,7080,7080,- -32768,- -32768,77,747,1019,1022,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,938,1001,- -32768,- -32768,759,77,1003,813,759,998,595,- -32768,813,1011,1006,- -32768,- -32768,1007,- -32768,77,1008,813,1010,759,1009,3217,1024,1102,- -32768,- -32768,7080,- -32768,4291,- -32768,1018,103,851,4443,1026,1020,1495,1016,1028,4443,7080,6282,620,6396,676,6510,842,- -32768,1032,- -32768,- -32768,813,367,- -32768,- -32768,1027,- -32768,1040,1039,813,4291,- -32768,- -32768,394,- -32768,7080,- -32768,- -32768,5256,1029,1030,1033,1037,- -32768,857,1036,1042,- 14,1043,- -32768,- -32768,749,- -32768,- -32768,- -32768,- -32768,5025,- -32768,- -32768,1039,938,- -32768,- -32768,938,1044,- -32768,759,1046,4291,1054,- -32768,- -32768,- -32768,938,1045,- -32768,- -32768,759,1034,678,1047,2928,1049,1053,4291,- -32768,- -32768,1148,842,1055,7788,1061,2774,7689,1050,- -32768,- 7,- -32768,1085,1051,733,785,130,794,439,143,- -32768,- -32768,- -32768,- -32768,1091,7689,2029,- -32768,- -32768,598,4443,603,6624,4443,616,6738,6852,693,1070,- -32768,- -32768,- -32768,7080,1072,- -32768,- -32768,992,- -32768,589,386,- -32768,- -32768,- -32768,4291,1175,- -32768,- -32768,1068,84,407,- -32768,- -32768,- -32768,- -32768,4908,- -32768,1039,1039,938,- -32768,- -32768,1075,1076,1039,938,- -32768,- -32768,- -32768,1078,1079,700,241,- -32768,- -32768,- -32768,619,4443,1080,842,2628,- -32768,4291,1069,- -32768,1727,7689,7080,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7689,7080,- -32768,842,1083,- -32768,4443,4443,628,- -32768,4443,4443,642,4443,643,6966,- -32768,1077,29,813,- -32768,- -32768,2774,1081,1084,7080,1096,418,- -32768,- -32768,1099,- -32768,- -32768,- -32768,- -32768,1039,- -32768,1101,- -32768,1039,- -32768,- -32768,- -32768,241,1089,706,- -32768,- -32768,- -32768,1094,1090,1098,7689,842,- -32768,736,457,722,752,752,677,83,83,735,735,735,735,343,343,- 24,- 24,- -32768,- -32768,- -32768,464,- -32768,- -32768,- -32768,- -32768,4443,- -32768,- -32768,4443,- -32768,4443,4443,663,- -32768,- -32768,- -32768,1100,761,1097,4291,689,- -32768,1106,1099,- -32768,- -32768,- -32768,1108,- -32768,- -32768,- -32768,- -32768,241,- -32768,997,467,- -32768,- -32768,7080,1727,- -32768,- -32768,- -32768,- -32768,4443,- -32768,- -32768,- -32768,1112,- -32768,1114,- -32768,- -32768,- -32768,514,- -32768,842,- -32768,- -32768,1113,- -32768,- -32768,- -32768,- -32768,1232,1240,- -32768};
# 1985
static short Cyc_yypgoto[162U]={- -32768,- 53,- -32768,- -32768,- -32768,- -32768,- -32768,145,- -32768,- -32768,205,- -32768,- -32768,- 217,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,43,- 133,5,- -32768,- -32768,0,592,- -32768,124,- 207,1131,32,- -32768,- -32768,- 156,- -32768,452,1219,- 749,- -32768,- -32768,- -32768,985,981,840,149,- -32768,- -32768,583,- 52,- 720,- -32768,- -32768,91,- -32768,- -32768,165,- 239,1192,- 341,37,- -32768,1110,- -32768,- -32768,1217,- 446,- -32768,543,- 140,- 165,- 122,- 376,263,562,568,- 456,- 501,- 124,- 451,- 130,- -32768,- 264,- 168,- 612,- 321,- -32768,853,54,- 66,- 144,- 214,- 273,212,- -32768,- -32768,- 50,- 222,- -32768,- 759,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,129,1052,- -32768,653,856,- -32768,121,834,- -32768,- 164,- 422,- 162,- 375,- 372,- 383,902,- 367,- 374,- 272,- 355,- 342,- 29,665,- -32768,- -32768,- -32768,- -32768,342,- -32768,- 919,265,- -32768,482,926,189,- -32768,- 371,- -32768,453,495,- 67,- 70,- 83,- 113};
# 2009
static short Cyc_yytable[7942U]={65,149,599,271,148,132,340,596,283,82,628,758,598,658,602,603,355,330,498,352,601,65,356,346,155,65,84,333,1030,342,88,483,68,576,43,331,465,264,673,608,609,458,476,264,716,252,253,619,260,124,338,148,886,68,610,611,486,68,65,467,927,382,257,258,65,65,65,502,65,142,143,144,425,145,65,159,273,274,275,154,276,277,278,279,680,110,533,534,426,536,68,284,713,984,720,544,68,68,68,468,68,673,946,321,539,383,68,398,688,689,469,933,724,1101,161,65,65,152,604,605,606,607,960,421,66,947,399,400,683,65,65,65,976,65,65,65,65,985,854,477,471,65,954,768,672,66,154,68,68,66,43,65,589,65,422,105,310,545,154,707,159,68,68,68,546,68,68,68,68,87,672,467,373,68,231,378,379,723,390,391,1127,565,66,68,152,68,684,564,66,66,66,653,66,232,309,414,320,380,66,- 560,946,538,109,488,380,772,42,540,209,756,553,468,653,541,125,45,392,393,1051,230,236,488,113,1028,1056,992,993,516,141,563,517,238,- 169,501,111,1035,1036,261,259,66,66,114,1040,666,254,262,1077,112,457,255,458,458,458,66,66,66,375,66,66,66,66,42,800,994,995,66,666,231,565,115,45,560,47,770,1000,66,42,66,48,116,459,117,49,65,678,45,232,464,154,267,52,53,242,1001,1002,529,1113,49,119,243,455,65,576,530,546,120,88,485,65,65,801,65,735,310,310,68,310,1102,121,322,42,1104,542,376,808,511,320,864,42,45,122,868,617,68,85,329,620,45,126,562,68,68,565,68,- 235,48,879,- 235,627,49,42,309,309,127,309,42,1133,52,53,45,424,173,448,44,45,46,444,42,290,612,613,614,444,1138,65,129,45,885,726,310,137,630,935,47,48,546,642,469,49,431,432,156,380,157,19,20,52,53,381,158,867,568,710,569,570,871,68,66,271,130,415,416,417,841,877,667,131,309,488,585,47,527,380,958,329,66,786,552,654,576,655,754,66,66,964,66,656,47,488,173,786,676,42,677,488,668,322,669,929,- 15,44,45,46,670,175,418,934,457,457,457,419,420,638,482,48,380,657,380,376,463,840,376,767,442,712,535,52,53,757,106,179,469,665,582,671,711,822,380,396,459,459,459,397,474,65,80,65,718,66,464,745,416,417,930,597,679,233,521,704,1019,774,380,469,235,528,775,380,86,237,485,565,107,65,108,788,1024,68,154,68,789,936,65,239,776,937,65,154,1032,841,791,154,621,511,106,80,418,947,790,1099,234,746,420,546,80,68,106,231,1028,80,133,134,751,629,68,720,701,147,68,150,- 257,635,80,- 257,846,708,232,240,939,152,847,178,261,835,241,998,107,489,380,999,262,856,824,490,727,380,840,1114,107,667,455,832,289,291,1115,1060,1061,645,646,647,1057,80,80,488,147,1059,488,1063,1064,66,80,66,380,1062,244,80,80,80,591,80,80,80,80,841,462,256,565,693,694,265,463,697,1069,1070,702,857,1091,66,837,825,80,1137,135,136,657,937,66,1071,1072,42,66,823,865,848,849,380,380,44,45,46,831,741,806,671,48,178,875,380,462,759,505,760,268,807,463,106,52,53,106,280,840,106,42,804,1005,449,805,106,281,455,44,45,46,511,261,1022,146,818,323,65,380,65,262,475,310,343,154,511,862,380,511,1065,1066,1067,1068,812,380,107,42,380,107,344,870,107,380,1006,44,45,46,107,1008,68,881,68,709,48,336,380,921,482,489,380,309,1012,320,463,1048,52,53,380,727,42,1055,337,974,1081,487,765,565,44,45,46,47,380,380,388,389,80,48,1084,1086,779,505,345,487,349,513,357,42,518,52,53,358,42,380,65,44,45,46,370,1120,44,45,46,924,48,80,380,966,505,967,359,955,506,1021,956,148,52,53,360,380,1017,815,817,380,962,1125,68,1044,394,395,1045,971,66,1107,66,384,1108,361,178,16,17,18,42,555,556,348,362,351,353,353,44,45,46,363,1023,546,688,689,364,48,365,368,369,505,809,810,353,982,385,353,353,52,53,733,734,623,858,859,952,953,988,989,990,991,65,1004,106,996,997,353,1122,380,366,1132,106,888,889,367,377,386,1047,387,106,148,423,1037,441,427,451,447,709,1041,452,472,481,65,68,473,66,480,310,523,491,515,519,493,495,503,107,512,514,524,282,525,918,920,107,923,913,926,522,537,531,532,107,542,549,68,47,981,1131,554,550,558,469,567,266,382,309,938,583,590,588,622,686,687,351,270,632,624,625,1073,1074,1075,633,626,634,487,148,636,487,640,637,65,643,641,288,660,310,80,644,80,335,661,89,663,1025,270,659,662,487,664,674,681,682,695,487,696,66,699,353,698,703,705,717,68,236,714,890,683,721,732,191,725,917,1112,309,729,837,730,731,- 560,1052,736,1053,737,738,742,1047,66,739,148,1010,740,743,1014,1016,353,744,353,353,353,353,353,353,353,353,353,353,353,353,353,353,353,353,353,747,42,92,198,199,93,94,95,748,749,45,752,1092,753,755,761,773,766,769,794,106,456,350,461,106,203,771,780,795,784,787,353,799,376,802,162,163,164,165,166,1058,66,167,168,169,813,819,781,170,65,500,820,821,329,310,828,826,830,1076,750,288,838,107,844,850,1007,107,860,1011,852,861,863,1088,866,869,872,873,874,876,1093,878,68,1096,1124,883,880,882,887,891,915,892,916,309,928,487,932,931,965,89,940,941,943,942,961,559,353,944,959,487,970,975,487,945,951,957,963,973,977,986,972,803,1049,500,983,1003,577,578,579,580,581,980,987,584,1018,1026,587,1020,1027,1038,1054,1039,1042,1043,1050,1078,592,1097,1089,595,946,1103,1094,1106,1095,1079,1080,90,1109,1082,1083,1110,1085,1111,1121,1136,401,1141,1123,428,615,1126,429,92,66,1142,93,94,95,1128,430,45,616,1135,1139,1105,97,592,1129,98,353,785,269,99,118,100,101,106,431,432,798,497,504,160,128,834,102,402,403,404,405,406,407,408,409,410,411,341,1090,833,827,763,631,600,80,1033,914,586,1116,1100,450,1117,652,1118,1119,1130,0,270,412,107,639,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,270,0,0,0,0,0,0,270,0,0,1134,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,353,0,0,0,0,30,0,0,0,0,0,0,0,32,33,592,0,592,0,0,0,0,0,270,0,39,0,0,147,0,80,0,0,0,270,0,0,40,0,41,0,0,0,270,0,0,0,0,0,0,0,0,0,0,0,0,0,353,0,0,0,500,327,0,0,42,0,0,0,0,353,0,43,44,45,46,47,0,329,0,0,0,48,0,0,0,482,0,50,51,0,0,463,0,52,53,0,0,0,0,0,0,0,0,0,0,0,0,500,0,0,0,0,0,0,353,147,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,778,0,0,0,0,270,783,0,0,0,0,22,23,24,793,456,0,0,0,270,0,0,0,0,0,0,0,0,30,0,893,894,0,0,0,0,32,33,0,592,0,0,0,0,0,0,89,0,39,0,147,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,979,0,0,353,0,0,0,0,0,0,500,0,0,842,843,0,845,327,0,0,0,353,0,0,0,0,500,43,44,855,46,47,0,329,0,0,90,48,353,0,0,462,0,50,51,147,0,463,0,52,53,229,92,0,0,93,94,95,0,96,45,0,0,0,0,97,0,0,98,0,0,0,895,0,100,101,0,0,0,0,0,0,896,288,0,102,206,0,0,207,208,353,0,353,353,353,353,353,353,353,353,353,353,353,353,353,353,353,353,353,0,0,0,0,0,270,0,500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,0,0,0,0,0,0,0,353,0,0,0,0,0,0,0,270,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,809,810,21,180,181,292,0,293,294,295,296,297,298,299,300,22,23,24,301,89,26,183,302,0,0,0,0,184,27,303,353,0,30,185,186,187,188,189,500,31,32,33,34,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,304,92,198,199,93,94,95,43,44,45,46,47,200,305,151,0,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,180,181,292,0,293,294,295,296,297,298,299,300,22,23,24,301,89,26,183,302,0,0,0,0,184,27,303,0,0,30,185,186,187,188,189,0,31,32,33,34,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,304,92,198,199,93,94,95,43,44,45,46,47,200,305,151,306,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,180,181,292,0,293,294,295,296,297,298,299,300,22,23,24,301,89,26,183,302,0,0,0,0,184,27,303,0,0,30,185,186,187,188,189,0,31,32,33,34,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,304,92,198,199,93,94,95,43,44,45,46,47,200,305,151,0,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,180,181,292,0,293,294,295,296,297,298,299,300,22,23,24,301,89,26,183,302,0,0,0,0,184,27,303,0,0,30,185,186,187,188,189,0,31,32,33,34,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,520,92,198,199,93,94,95,43,44,45,46,47,200,305,151,0,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,180,181,0,0,0,0,0,0,0,0,0,0,22,23,24,301,89,26,183,0,0,0,0,0,184,27,0,0,0,30,185,186,187,188,189,0,31,32,33,34,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,43,44,45,46,47,200,700,0,0,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,180,181,0,0,0,0,0,0,0,0,0,0,22,23,24,301,89,0,0,0,0,0,0,0,0,27,0,0,0,30,185,186,187,188,189,0,0,32,33,0,0,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,43,44,45,46,47,200,0,0,0,0,201,0,0,0,350,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,180,181,0,0,0,0,0,0,0,0,0,0,22,23,24,182,89,0,183,0,0,0,0,0,184,0,0,0,0,30,185,186,187,188,189,0,0,32,33,0,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,43,44,45,46,47,200,0,374,0,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,180,181,0,0,0,0,0,0,0,0,0,0,22,23,24,182,89,0,183,0,0,0,0,0,184,0,0,0,0,30,185,186,187,188,189,0,0,32,33,0,190,0,0,0,191,0,0,192,193,39,194,0,0,0,0,0,0,0,0,0,0,40,195,41,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,43,44,45,46,47,200,0,0,0,0,201,0,0,0,202,0,50,307,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,90,31,32,33,34,0,0,35,36,0,37,38,762,0,39,429,92,0,0,93,94,95,0,430,45,0,40,0,41,97,0,0,98,0,0,0,99,0,100,101,0,431,432,0,0,0,0,0,0,102,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,- 12,0,48,0,0,0,49,0,50,51,0,0,0,0,52,53,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,90,31,32,33,34,0,0,35,36,0,37,38,443,0,39,229,92,0,0,93,94,95,0,96,45,0,40,0,41,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,0,0,102,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,0,0,48,0,0,0,49,0,50,51,0,0,0,0,52,53,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,90,31,32,33,34,0,0,35,36,0,37,38,764,0,39,229,92,0,0,93,94,95,0,96,45,0,40,0,41,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,89,0,102,0,0,0,0,42,0,0,0,0,0,0,43,44,45,46,47,0,0,0,- 12,0,48,0,0,0,49,0,50,51,0,0,0,0,52,53,7,8,9,10,11,12,13,14,15,16,17,18,19,20,89,0,0,0,0,0,90,0,0,0,0,0,0,0,0,22,23,24,0,0,0,91,92,0,0,93,94,95,0,96,45,0,30,0,0,97,0,0,98,0,32,33,99,0,100,101,0,0,0,0,0,0,39,0,0,102,0,0,90,0,0,0,0,0,40,0,41,0,0,0,0,0,0,229,92,0,0,93,94,95,0,96,45,0,0,0,0,97,0,0,98,0,42,0,99,0,100,101,0,43,44,45,46,47,0,0,0,102,0,48,0,0,0,505,0,50,51,0,0,0,0,52,53,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,34,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,44,0,46,47,0,0,151,0,0,0,282,0,0,0,0,50,51,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,34,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,43,44,0,46,47,0,0,151,22,23,24,25,0,0,0,0,50,51,0,0,0,27,0,0,0,30,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,0,0,0,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,327,0,0,328,0,0,0,22,23,24,43,44,0,46,47,0,329,0,0,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,245,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,0,246,0,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,327,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,329,0,0,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,0,0,453,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,0,0,715,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,722,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,0,0,0,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,43,44,0,46,47,0,0,0,0,0,0,0,0,0,30,0,50,51,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,40,0,41,0,0,0,0,0,0,0,180,181,292,0,293,294,295,296,297,298,299,300,0,0,0,182,89,0,183,302,0,0,0,0,184,43,303,0,0,47,185,186,187,188,189,0,0,0,0,0,190,50,51,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,520,92,198,199,93,94,95,0,0,45,0,0,200,305,151,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,571,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,572,92,198,199,93,94,95,0,262,45,0,0,200,0,354,573,0,201,0,0,0,202,0,0,203,0,431,432,180,181,0,204,0,0,205,206,0,0,207,208,0,0,0,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,572,92,198,199,93,94,95,0,262,45,0,0,200,0,354,839,0,201,0,0,0,202,0,0,203,0,431,432,180,181,0,204,0,0,205,206,0,0,207,208,0,0,0,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,572,92,198,199,93,94,95,0,262,45,0,0,200,0,354,853,0,201,0,0,0,202,0,0,203,0,431,432,180,181,0,204,0,0,205,206,0,0,207,208,0,0,0,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,572,92,198,199,93,94,95,0,262,45,0,0,200,0,354,1034,0,201,0,0,0,202,0,0,203,0,431,432,180,181,0,204,0,0,205,206,0,0,207,208,0,0,0,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,572,92,198,199,93,94,95,0,262,45,0,0,200,0,354,0,0,201,0,0,0,202,0,0,203,0,431,432,180,181,0,204,0,0,205,206,0,0,207,208,0,0,0,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,0,0,203,180,181,0,334,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,354,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,526,0,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,618,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,0,0,203,180,181,0,651,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,706,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,0,0,203,180,181,0,777,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,0,0,203,180,181,0,792,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,814,0,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,816,0,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,919,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,922,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,925,0,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,1009,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,1013,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,1015,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,1087,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,202,0,0,203,180,181,0,0,0,0,204,0,0,205,206,0,0,207,208,182,89,0,183,0,0,0,0,0,184,0,0,0,0,0,185,186,187,188,189,0,0,0,0,0,190,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,782,92,198,199,93,94,95,0,0,45,180,181,200,0,0,0,0,201,0,0,0,202,0,0,203,182,89,0,0,0,0,204,0,0,205,206,0,0,207,208,185,186,187,188,189,0,0,0,0,0,0,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,180,181,200,0,354,0,0,201,0,0,0,202,0,0,203,182,89,0,0,0,0,204,0,0,205,206,0,0,207,208,185,186,187,188,189,0,0,0,0,0,0,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,180,181,200,0,750,0,0,201,0,0,0,202,0,0,203,182,89,0,0,0,0,204,0,0,205,206,0,0,207,208,185,186,187,188,189,0,0,0,0,0,0,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,180,181,200,0,0,0,0,201,0,0,0,347,0,0,203,182,89,0,0,0,0,204,0,0,205,206,0,0,207,208,185,186,187,188,189,0,0,0,0,0,0,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,180,181,200,0,0,0,0,201,0,0,0,350,0,0,203,182,89,0,0,0,0,204,0,0,205,206,0,0,207,208,185,186,187,188,189,0,0,0,0,0,0,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,180,181,200,0,0,0,0,201,0,0,0,202,0,0,203,182,89,0,0,0,0,204,0,0,205,206,0,0,207,208,185,186,187,188,189,0,0,0,0,0,0,0,0,0,191,0,0,192,193,0,194,0,0,0,0,0,0,0,0,0,0,0,195,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,92,198,199,93,94,95,0,0,45,0,0,200,0,0,0,0,201,0,0,0,978,0,0,203,0,0,0,0,0,0,204,0,0,205,206,0,0,207,208};
# 2807
static short Cyc_yycheck[7942U]={0,71,385,127,71,58,171,382,141,116,432,623,384,469,388,389,184,157,282,183,387,21,184,179,74,25,21,157,947,173,25,270,0,354,121,157,250,120,489,394,395,248,17,126,545,111,112,418,118,49,137,118,801,21,396,397,270,25,58,94,819,100,115,116,64,65,66,284,68,64,65,66,121,68,74,75,129,130,131,74,133,134,135,136,506,133,308,309,137,311,58,141,543,100,550,66,64,65,66,134,68,552,116,153,321,144,74,131,5,6,145,831,558,1032,77,115,116,74,390,391,392,393,871,112,0,139,150,151,81,129,130,131,891,133,134,135,136,144,750,114,253,141,862,644,485,21,141,115,116,25,121,151,374,153,143,26,151,128,153,530,160,129,130,131,329,133,134,135,136,135,511,94,201,141,114,204,205,553,95,96,1099,345,58,151,141,153,139,343,64,65,66,464,68,133,151,224,153,130,74,139,116,127,114,270,130,651,114,130,87,142,332,134,485,326,49,123,133,134,977,90,99,288,114,139,983,95,96,135,63,342,138,102,129,283,133,955,956,114,117,115,116,133,962,482,127,122,1005,133,248,132,457,458,459,129,130,131,202,133,134,135,136,114,684,133,134,141,505,114,432,127,123,337,125,649,131,151,114,153,131,127,248,135,135,283,501,123,133,250,283,124,143,144,128,150,151,127,1055,135,54,135,246,301,623,135,469,113,301,270,308,309,686,311,576,308,309,283,311,1037,135,154,114,1041,114,202,695,288,283,773,114,123,128,777,415,301,128,127,419,123,113,339,308,309,506,311,136,131,792,139,431,135,114,308,309,135,311,114,1115,143,144,123,231,133,241,122,123,124,237,114,140,398,399,400,243,1132,374,127,123,799,136,374,127,132,833,125,131,550,452,145,135,140,141,133,130,135,20,21,143,144,136,141,776,347,535,349,350,781,374,283,532,127,90,91,92,734,790,482,128,374,485,365,125,300,130,869,127,301,665,130,133,750,135,139,308,309,880,311,141,125,505,133,679,494,114,496,511,133,281,135,824,128,122,123,124,141,116,135,832,457,458,459,140,141,127,135,131,130,466,130,347,141,734,350,136,136,540,310,143,144,622,26,128,145,482,362,484,535,708,130,145,457,458,459,149,136,494,0,496,549,374,462,90,91,92,136,383,505,133,295,127,931,654,130,145,114,302,655,130,22,135,482,684,26,522,28,668,139,494,522,496,669,136,531,127,655,140,535,531,130,859,669,535,420,505,90,49,135,139,669,130,97,140,141,717,58,522,102,114,139,63,127,128,590,433,531,1020,522,71,535,73,127,441,76,130,741,531,133,130,841,535,741,85,114,115,132,145,90,130,130,149,122,754,713,136,561,130,859,139,102,665,545,722,149,150,139,987,988,457,458,459,984,115,116,679,118,986,682,990,991,494,124,496,130,989,121,129,130,131,136,133,134,135,136,953,135,121,799,515,516,135,141,519,996,997,522,757,1021,522,730,714,154,136,127,128,653,140,531,998,999,114,535,710,774,745,746,130,130,122,123,124,721,136,136,672,131,179,788,130,135,624,135,626,136,136,141,231,143,144,234,127,953,237,114,127,915,242,130,243,130,644,122,123,124,665,114,115,128,127,127,708,130,710,122,260,708,130,710,679,769,130,682,992,993,994,995,136,130,231,114,130,234,129,136,237,130,136,122,123,124,243,136,708,794,710,531,131,135,130,127,135,130,130,708,136,710,141,136,143,144,130,727,114,983,135,887,136,270,637,931,122,123,124,125,130,130,97,98,281,131,136,136,659,135,132,288,135,290,135,114,293,143,144,135,114,130,794,122,123,124,18,136,122,123,124,127,131,310,130,129,135,131,135,863,139,935,866,882,143,144,135,130,127,700,701,130,876,136,794,127,93,94,130,884,708,127,710,99,130,135,343,17,18,19,114,84,85,180,135,182,183,184,122,123,124,135,937,1020,5,6,135,131,135,196,197,135,22,23,201,896,146,204,205,143,144,129,130,423,129,130,129,130,147,148,97,98,884,914,433,93,94,224,129,130,135,1115,441,804,805,135,135,147,970,148,449,970,128,957,114,121,134,114,698,963,135,114,136,915,884,134,794,129,915,114,129,135,135,129,129,129,433,128,128,127,132,127,813,814,441,816,809,818,135,129,139,135,449,114,136,915,125,895,1114,136,139,142,145,129,122,100,915,838,114,136,135,133,512,513,301,127,129,134,134,1000,1001,1002,130,134,139,482,1045,136,485,136,130,983,129,127,146,114,983,494,130,496,158,128,41,136,942,157,135,135,505,130,47,127,130,135,511,128,884,30,345,139,127,45,145,983,895,135,806,81,136,114,69,136,812,1054,983,136,1111,130,136,139,978,136,980,130,130,136,1108,915,130,1108,921,130,136,924,925,382,136,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,136,114,115,116,117,118,119,120,116,129,123,136,1024,130,136,142,142,129,136,128,633,247,135,249,637,138,136,127,46,136,136,432,135,978,129,76,77,78,79,80,985,983,83,84,85,135,128,661,89,1115,282,127,136,127,1115,82,129,134,1003,128,286,133,633,25,139,919,637,114,922,136,114,136,1017,136,142,130,136,136,136,1024,136,1115,1027,1095,48,142,128,135,128,139,136,129,1115,127,665,121,135,129,41,136,136,130,135,115,336,506,136,127,679,128,28,682,136,136,136,136,129,128,99,136,691,975,354,139,99,357,358,359,360,361,135,146,364,129,25,367,130,135,129,136,130,129,129,129,127,377,116,136,380,116,115,136,129,135,1008,1009,99,129,1012,1013,136,1015,130,129,116,60,0,136,111,401,130,114,115,1115,0,118,119,120,136,122,123,413,136,136,1045,128,418,1108,131,590,664,126,135,40,137,138,809,140,141,682,281,286,76,52,727,148,101,102,103,104,105,106,107,108,109,110,172,1020,722,717,633,434,386,794,948,809,366,1081,1029,243,1084,463,1086,1087,1111,- 1,462,132,809,449,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,482,- 1,- 1,- 1,- 1,- 1,- 1,489,- 1,- 1,1120,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,684,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,530,- 1,532,- 1,- 1,- 1,- 1,- 1,532,- 1,74,- 1,- 1,882,- 1,884,- 1,- 1,- 1,543,- 1,- 1,86,- 1,88,- 1,- 1,- 1,552,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,741,- 1,- 1,- 1,576,111,- 1,- 1,114,- 1,- 1,- 1,- 1,754,- 1,121,122,123,124,125,- 1,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,141,- 1,143,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,623,- 1,- 1,- 1,- 1,- 1,- 1,799,970,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,656,- 1,- 1,- 1,- 1,655,662,- 1,- 1,- 1,- 1,37,38,39,670,665,- 1,- 1,- 1,669,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,25,26,- 1,- 1,- 1,- 1,62,63,- 1,695,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,74,- 1,1045,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,893,- 1,- 1,896,- 1,- 1,- 1,- 1,- 1,- 1,734,- 1,- 1,737,738,- 1,740,111,- 1,- 1,- 1,914,- 1,- 1,- 1,- 1,750,121,122,753,124,125,- 1,127,- 1,- 1,99,131,931,- 1,- 1,135,- 1,137,138,1108,- 1,141,- 1,143,144,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,801,- 1,148,149,- 1,- 1,152,153,984,- 1,986,987,988,989,990,991,992,993,994,995,996,997,998,999,1000,1001,1002,- 1,- 1,- 1,- 1,- 1,833,- 1,841,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,859,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,871,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1054,- 1,- 1,- 1,- 1,- 1,- 1,- 1,887,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,1114,- 1,54,55,56,57,58,59,953,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,- 1,62,63,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,- 1,62,63,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,99,61,62,63,64,- 1,- 1,67,68,- 1,70,71,111,- 1,74,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,86,- 1,88,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,140,141,- 1,- 1,- 1,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,129,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,99,61,62,63,64,- 1,- 1,67,68,- 1,70,71,111,- 1,74,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,86,- 1,88,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,99,61,62,63,64,- 1,- 1,67,68,- 1,70,71,111,- 1,74,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,86,- 1,88,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,148,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,129,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,54,- 1,- 1,128,- 1,- 1,131,- 1,62,63,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,148,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,114,- 1,135,- 1,137,138,- 1,121,122,123,124,125,- 1,- 1,- 1,148,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,122,- 1,124,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,121,122,- 1,124,125,- 1,- 1,128,37,38,39,40,- 1,- 1,- 1,- 1,137,138,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,121,51,- 1,- 1,125,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,137,138,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,32,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp425[8U]="stdcall";static char _tmp426[6U]="cdecl";static char _tmp427[9U]="fastcall";static char _tmp428[9U]="noreturn";static char _tmp429[6U]="const";static char _tmp42A[8U]="aligned";static char _tmp42B[7U]="packed";static char _tmp42C[7U]="shared";static char _tmp42D[7U]="unused";static char _tmp42E[5U]="weak";static char _tmp42F[10U]="dllimport";static char _tmp430[10U]="dllexport";static char _tmp431[23U]="no_instrument_function";static char _tmp432[12U]="constructor";static char _tmp433[11U]="destructor";static char _tmp434[22U]="no_check_memory_usage";static char _tmp435[5U]="pure";static char _tmp436[14U]="always_inline";static char _tmp437[9U]="no_throw";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp397=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp397;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp76C;(_tmp76C.YYINITIALSVAL).tag=72U,(_tmp76C.YYINITIALSVAL).val=0;_tmp76C;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp6B0=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp6B0),sizeof(short),_tmp6B0);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp6AF=200U;struct Cyc_Yystacktype*_tmp6AE=({struct _RegionHandle*_tmp827=yyregion;_region_malloc(_tmp827,_check_times(_tmp6AF,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp76B=200U;unsigned i;for(i=0;i < _tmp76B;++ i){(_tmp6AE[i]).v=yylval,({struct Cyc_Yyltype _tmp828=Cyc_yynewloc();(_tmp6AE[i]).l=_tmp828;});}}0;});_tmp6AE;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp82A=({const char*_tmp398="parser stack overflow";_tag_fat(_tmp398,sizeof(char),22U);});int _tmp829=yystate;Cyc_yyerror(_tmp82A,_tmp829,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp39C=(unsigned)yystacksize;short*_tmp39B=({struct _RegionHandle*_tmp82B=yyregion;_region_malloc(_tmp82B,_check_times(_tmp39C,sizeof(short)));});({{unsigned _tmp722=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp722;++ i){
i <= (unsigned)yyssp_offset?_tmp39B[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp39B[i]=0);}}0;});_tag_fat(_tmp39B,sizeof(short),_tmp39C);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp39A=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp399=({struct _RegionHandle*_tmp82C=yyregion;_region_malloc(_tmp82C,_check_times(_tmp39A,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp721=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp721;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp399[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp399[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp399,sizeof(struct Cyc_Yystacktype),_tmp39A);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1143U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 381?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,382U,sizeof(short),yychar)): 316;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7941)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7942U,sizeof(short),yyn))!= yychar1)goto yydefault;
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
if(yyn == 1142){
int _tmp39D=0;_npop_handler(0U);return _tmp39D;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp82D=({struct Cyc_Yystacktype _tmp723;_tmp723.v=yylval,_tmp723.l=yylloc;_tmp723;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp82D;});
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
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,564U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp39E=yyn;switch(_tmp39E){case 1U: _LL1: _LL2:
# 1148 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1151
goto _LL0;case 2U: _LL3: _LL4:
# 1154 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp82E=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp82E,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1158 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_Decl*_tmp832=({struct Cyc_Absyn_Decl*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({void*_tmp831=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->tag=11U,({struct _tuple0*_tmp830=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39F->f1=_tmp830;}),({struct Cyc_List_List*_tmp82F=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39F->f2=_tmp82F;});_tmp39F;});_tmp3A0->r=_tmp831;}),_tmp3A0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A0;});_tmp3A1->hd=_tmp832;}),_tmp3A1->tl=0;_tmp3A1;}));
Cyc_Lex_leave_using();
# 1161
goto _LL0;case 4U: _LL7: _LL8:
# 1162 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct Cyc_Absyn_Decl*_tmp837=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({void*_tmp836=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->tag=11U,({struct _tuple0*_tmp835=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A2->f1=_tmp835;}),({struct Cyc_List_List*_tmp834=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A2->f2=_tmp834;});_tmp3A2;});_tmp3A3->r=_tmp836;}),_tmp3A3->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A3;});_tmp3A4->hd=_tmp837;}),({struct Cyc_List_List*_tmp833=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A4->tl=_tmp833;});_tmp3A4;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1165
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct Cyc_Absyn_Decl*_tmp83C=({struct Cyc_Absyn_Decl*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({void*_tmp83B=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->tag=10U,({struct _fat_ptr*_tmp83A=({struct _fat_ptr*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct _fat_ptr _tmp839=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A5=_tmp839;});_tmp3A5;});_tmp3A6->f1=_tmp83A;}),({struct Cyc_List_List*_tmp838=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A6->f2=_tmp838;});_tmp3A6;});_tmp3A7->r=_tmp83B;}),_tmp3A7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A7;});_tmp3A8->hd=_tmp83C;}),_tmp3A8->tl=0;_tmp3A8;}));
Cyc_Lex_leave_namespace();
# 1168
goto _LL0;case 6U: _LLB: _LLC:
# 1169 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({struct Cyc_Absyn_Decl*_tmp842=({struct Cyc_Absyn_Decl*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({void*_tmp841=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->tag=10U,({struct _fat_ptr*_tmp840=({struct _fat_ptr*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({struct _fat_ptr _tmp83F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A9=_tmp83F;});_tmp3A9;});_tmp3AA->f1=_tmp840;}),({struct Cyc_List_List*_tmp83E=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3AA->f2=_tmp83E;});_tmp3AA;});_tmp3AB->r=_tmp841;}),_tmp3AB->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AB;});_tmp3AC->hd=_tmp842;}),({struct Cyc_List_List*_tmp83D=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3AC->tl=_tmp83D;});_tmp3AC;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1171 "parse.y"
int _tmp3AD=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3AD;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3AE=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp15=_tmp3AE;struct _tuple28*_tmp3AF=_stmttmp15;unsigned _tmp3B1;struct Cyc_List_List*_tmp3B0;_LL466: _tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3AF->f2;_LL467: {struct Cyc_List_List*exs=_tmp3B0;unsigned wc=_tmp3B1;
struct Cyc_List_List*_tmp3B2=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3B2;
if(exs != 0 && hides != 0)
({void*_tmp3B3=0U;({unsigned _tmp844=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp843=({const char*_tmp3B4="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3B4,sizeof(char),65U);});Cyc_Warn_err(_tmp844,_tmp843,_tag_fat(_tmp3B3,sizeof(void*),0U));});});
# 1178
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1181
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3B5=0U;({unsigned _tmp846=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp845=({const char*_tmp3B6="expecting \"C include\"";_tag_fat(_tmp3B6,sizeof(char),22U);});Cyc_Warn_err(_tmp846,_tmp845,_tag_fat(_tmp3B5,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({struct Cyc_Absyn_Decl*_tmp84B=({struct Cyc_Absyn_Decl*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({void*_tmp84A=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->tag=13U,({struct Cyc_List_List*_tmp849=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B8->f1=_tmp849;}),_tmp3B8->f2=cycdecls,_tmp3B8->f3=exs,({struct _tuple10*_tmp848=({struct _tuple10*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->f1=wc,_tmp3B7->f2=hides;_tmp3B7;});_tmp3B8->f4=_tmp848;});_tmp3B8;});_tmp3B9->r=_tmp84A;}),_tmp3B9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B9;});_tmp3BA->hd=_tmp84B;}),({struct Cyc_List_List*_tmp847=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BA->tl=_tmp847;});_tmp3BA;}));}else{
# 1187
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct Cyc_Absyn_Decl*_tmp84F=({struct Cyc_Absyn_Decl*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({void*_tmp84E=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->tag=12U,({struct Cyc_List_List*_tmp84D=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BB->f1=_tmp84D;});_tmp3BB;});_tmp3BC->r=_tmp84E;}),_tmp3BC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BC;});_tmp3BD->hd=_tmp84F;}),({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BD->tl=_tmp84C;});_tmp3BD;}));}}else{
# 1191
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({struct Cyc_Absyn_Decl*_tmp854=({struct Cyc_Absyn_Decl*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({void*_tmp853=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->tag=13U,({struct Cyc_List_List*_tmp852=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BF->f1=_tmp852;}),_tmp3BF->f2=cycdecls,_tmp3BF->f3=exs,({struct _tuple10*_tmp851=({struct _tuple10*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=wc,_tmp3BE->f2=hides;_tmp3BE;});_tmp3BF->f4=_tmp851;});_tmp3BF;});_tmp3C0->r=_tmp853;}),_tmp3C0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C0;});_tmp3C1->hd=_tmp854;}),({struct Cyc_List_List*_tmp850=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3C1->tl=_tmp850;});_tmp3C1;}));}
# 1194
goto _LL0;}}case 8U: _LLF: _LL10:
# 1195 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct Cyc_Absyn_Decl*_tmp856=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C2;});_tmp3C3->hd=_tmp856;}),({struct Cyc_List_List*_tmp855=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C3->tl=_tmp855;});_tmp3C3;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1197 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct Cyc_Absyn_Decl*_tmp858=({struct Cyc_Absyn_Decl*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C4;});_tmp3C5->hd=_tmp858;}),({struct Cyc_List_List*_tmp857=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C5->tl=_tmp857;});_tmp3C5;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1199 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct Cyc_Absyn_Decl*_tmp85A=({struct Cyc_Absyn_Decl*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C6;});_tmp3C7->hd=_tmp85A;}),({struct Cyc_List_List*_tmp859=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C7->tl=_tmp859;});_tmp3C7;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1201 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct Cyc_Absyn_Decl*_tmp85C=({struct Cyc_Absyn_Decl*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3C8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C8;});_tmp3C9->hd=_tmp85C;}),({struct Cyc_List_List*_tmp85B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C9->tl=_tmp85B;});_tmp3C9;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1202 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1207 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1212 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1217 "parse.y"
struct _fat_ptr _tmp3CA=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3CA;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp85D=(struct _fat_ptr)two;Cyc_strcmp(_tmp85D,({const char*_tmp3CB="C";_tag_fat(_tmp3CB,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp85E=(struct _fat_ptr)two;Cyc_strcmp(_tmp85E,({const char*_tmp3CC="C include";_tag_fat(_tmp3CC,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1224
({void*_tmp3CD=0U;({unsigned _tmp860=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp85F=({const char*_tmp3CE="expecting \"C\" or \"C include\"";_tag_fat(_tmp3CE,sizeof(char),29U);});Cyc_Warn_err(_tmp860,_tmp85F,_tag_fat(_tmp3CD,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1228
goto _LL0;}case 16U: _LL1F: _LL20:
# 1231 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1235 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1236 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1240 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct _tuple0*_tmp861=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CF->hd=_tmp861;}),_tmp3CF->tl=0;_tmp3CF;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1241 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct _tuple0*_tmp862=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D0->hd=_tmp862;}),_tmp3D0->tl=0;_tmp3D0;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1243 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct _tuple0*_tmp864=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D1->hd=_tmp864;}),({struct Cyc_List_List*_tmp863=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3D1->tl=_tmp863;});_tmp3D1;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1247 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=0,_tmp3D2->f2=0U;_tmp3D2;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1248 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1252 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct Cyc_List_List*_tmp865=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D3->f1=_tmp865;}),_tmp3D3->f2=0U;_tmp3D3;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1253 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=0,_tmp3D4->f2=0U;_tmp3D4;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1254 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=0,_tmp3D5->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D5;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1258 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct _tuple32*_tmp867=({struct _tuple32*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp866=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D6->f2=_tmp866;}),_tmp3D6->f3=0;_tmp3D6;});_tmp3D7->hd=_tmp867;}),_tmp3D7->tl=0;_tmp3D7;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1259 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct _tuple32*_tmp869=({struct _tuple32*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp868=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D8->f2=_tmp868;}),_tmp3D8->f3=0;_tmp3D8;});_tmp3D9->hd=_tmp869;}),_tmp3D9->tl=0;_tmp3D9;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1261 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({struct _tuple32*_tmp86C=({struct _tuple32*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp86B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3DA->f2=_tmp86B;}),_tmp3DA->f3=0;_tmp3DA;});_tmp3DB->hd=_tmp86C;}),({struct Cyc_List_List*_tmp86A=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3DB->tl=_tmp86A;});_tmp3DB;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1265 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1266 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1270 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct Cyc_Absyn_Decl*_tmp86F=({void*_tmp86E=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp86D=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3DC->f1=_tmp86D;});_tmp3DC;});Cyc_Absyn_new_decl(_tmp86E,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3DD->hd=_tmp86F;}),_tmp3DD->tl=0;_tmp3DD;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1271 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1272 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1281 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp872=yyr;struct Cyc_Parse_Declarator _tmp871=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp870=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp872,0,_tmp871,0,_tmp870,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1283 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DE=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DE;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp875=yyr;struct Cyc_Parse_Declarator _tmp874=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp873=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp875,& d,_tmp874,0,_tmp873,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1296 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp879=yyr;struct Cyc_Parse_Declarator _tmp878=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp877=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp876=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp879,0,_tmp878,_tmp877,_tmp876,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1298 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DF=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DF;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp87D=yyr;struct Cyc_Parse_Declarator _tmp87C=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp87B=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp87A=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp87D,& d,_tmp87C,_tmp87B,_tmp87A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1306 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E0=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3E0;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp880=yyr;struct Cyc_Parse_Declarator _tmp87F=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp87E=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp880,& d,_tmp87F,0,_tmp87E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1309 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E1=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3E1;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp884=yyr;struct Cyc_Parse_Declarator _tmp883=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp882=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp881=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp884,& d,_tmp883,_tmp882,_tmp881,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1314 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1317
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1320
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct _fat_ptr _tmp885=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3E2=_tmp885;});_tmp3E2;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1323
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1329 "parse.y"
int _tmp3E3=((yyyvsp[0]).l).first_line;int location=_tmp3E3;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp887=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp886=(unsigned)location;Cyc_Parse_make_declarations(_tmp887,0,_tmp886,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1332 "parse.y"
int _tmp3E4=((yyyvsp[0]).l).first_line;int location=_tmp3E4;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp88A=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp889=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp888=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp88A,_tmp889,_tmp888,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1336
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({struct Cyc_Absyn_Decl*_tmp88D=({struct Cyc_Absyn_Pat*_tmp88C=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp88B=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp88C,_tmp88B,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E5->hd=_tmp88D;}),_tmp3E5->tl=0;_tmp3E5;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1338 "parse.y"
struct Cyc_List_List*_tmp3E6=0;struct Cyc_List_List*vds=_tmp3E6;
{struct Cyc_List_List*_tmp3E7=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3E7;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3E8=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3E8;
struct _tuple0*qv=({struct _tuple0*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({union Cyc_Absyn_Nmspace _tmp88E=Cyc_Absyn_Rel_n(0);_tmp3EB->f1=_tmp88E;}),_tmp3EB->f2=id;_tmp3EB;});
struct Cyc_Absyn_Vardecl*_tmp3E9=({struct _tuple0*_tmp88F=qv;Cyc_Absyn_new_vardecl(0U,_tmp88F,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3E9;
vds=({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->hd=vd,_tmp3EA->tl=vds;_tmp3EA;});}}
# 1345
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct Cyc_Absyn_Decl*_tmp890=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EC->hd=_tmp890;}),_tmp3EC->tl=0;_tmp3EC;}));
# 1348
goto _LL0;}case 51U: _LL61: _LL62: {
# 1351 "parse.y"
struct _fat_ptr _tmp3ED=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3ED;
Cyc_Parse_tvar_ok(three,(unsigned)((yyyvsp[2]).l).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));({struct _fat_ptr*_tmp892=({struct _fat_ptr*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));*_tmp3F1=three;_tmp3F1;});_tmp3F2->name=_tmp892;}),_tmp3F2->identity=- 1,({void*_tmp891=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3F2->kind=_tmp891;});_tmp3F2;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp896=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp895=({struct _tuple0*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp894=({struct _fat_ptr*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({struct _fat_ptr _tmp893=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3EF=_tmp893;});_tmp3EF;});_tmp3F0->f2=_tmp894;});_tmp3F0;});Cyc_Absyn_new_vardecl(_tmp896,_tmp895,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));({struct Cyc_Absyn_Decl*_tmp897=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EE->hd=_tmp897;}),_tmp3EE->tl=0;_tmp3EE;}));
# 1358
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1360
struct _fat_ptr _tmp3F3=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3F3;
if(({struct _fat_ptr _tmp898=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp898,({const char*_tmp3F4="H";_tag_fat(_tmp3F4,sizeof(char),2U);}));})== 0)
({void*_tmp3F5=0U;({unsigned _tmp89A=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp899=({const char*_tmp3F6="bad occurrence of heap region `H";_tag_fat(_tmp3F6,sizeof(char),33U);});Cyc_Warn_err(_tmp89A,_tmp899,_tag_fat(_tmp3F5,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp89B=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp89B,({const char*_tmp3F7="U";_tag_fat(_tmp3F7,sizeof(char),2U);}));})== 0)
({void*_tmp3F8=0U;({unsigned _tmp89D=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp89C=({const char*_tmp3F9="bad occurrence of unique region `U";_tag_fat(_tmp3F9,sizeof(char),35U);});Cyc_Warn_err(_tmp89D,_tmp89C,_tag_fat(_tmp3F8,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp401=_cycalloc(sizeof(*_tmp401));({struct _fat_ptr*_tmp8A1=({struct _fat_ptr*_tmp400=_cycalloc(sizeof(*_tmp400));({struct _fat_ptr _tmp8A0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FF=({struct Cyc_String_pa_PrintArg_struct _tmp724;_tmp724.tag=0U,_tmp724.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp724;});void*_tmp3FD[1U];_tmp3FD[0]=& _tmp3FF;({struct _fat_ptr _tmp89F=({const char*_tmp3FE="`%s";_tag_fat(_tmp3FE,sizeof(char),4U);});Cyc_aprintf(_tmp89F,_tag_fat(_tmp3FD,sizeof(void*),1U));});});*_tmp400=_tmp8A0;});_tmp400;});_tmp401->name=_tmp8A1;}),_tmp401->identity=- 1,({
void*_tmp89E=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp401->kind=_tmp89E;});_tmp401;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A4=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp8A3=({struct _tuple0*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A2=({struct _fat_ptr*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));*_tmp3FB=two;_tmp3FB;});_tmp3FC->f2=_tmp8A2;});_tmp3FC;});Cyc_Absyn_new_vardecl(_tmp8A4,_tmp8A3,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));({struct Cyc_Absyn_Decl*_tmp8A5=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3FA->hd=_tmp8A5;}),_tmp3FA->tl=0;_tmp3FA;}));
# 1371
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1373
struct _fat_ptr _tmp402=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp402;
struct _fat_ptr _tmp403=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp403;
struct Cyc_Absyn_Exp*_tmp404=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp404;
if(({struct _fat_ptr _tmp8A6=(struct _fat_ptr)four;Cyc_strcmp(_tmp8A6,({const char*_tmp405="open";_tag_fat(_tmp405,sizeof(char),5U);}));})!= 0)({void*_tmp406=0U;({unsigned _tmp8A8=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp8A7=({const char*_tmp407="expecting `open'";_tag_fat(_tmp407,sizeof(char),17U);});Cyc_Warn_err(_tmp8A8,_tmp8A7,_tag_fat(_tmp406,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp40F=_cycalloc(sizeof(*_tmp40F));({struct _fat_ptr*_tmp8AC=({struct _fat_ptr*_tmp40E=_cycalloc(sizeof(*_tmp40E));({struct _fat_ptr _tmp8AB=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp40D=({struct Cyc_String_pa_PrintArg_struct _tmp725;_tmp725.tag=0U,_tmp725.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp725;});void*_tmp40B[1U];_tmp40B[0]=& _tmp40D;({struct _fat_ptr _tmp8AA=({const char*_tmp40C="`%s";_tag_fat(_tmp40C,sizeof(char),4U);});Cyc_aprintf(_tmp8AA,_tag_fat(_tmp40B,sizeof(void*),1U));});});*_tmp40E=_tmp8AB;});_tmp40E;});_tmp40F->name=_tmp8AC;}),_tmp40F->identity=- 1,({
void*_tmp8A9=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp40F->kind=_tmp8A9;});_tmp40F;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8AF=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8AE=({struct _tuple0*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8AD=({struct _fat_ptr*_tmp409=_cycalloc(sizeof(*_tmp409));*_tmp409=two;_tmp409;});_tmp40A->f2=_tmp8AD;});_tmp40A;});Cyc_Absyn_new_vardecl(_tmp8AF,_tmp8AE,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp408=_cycalloc(sizeof(*_tmp408));({struct Cyc_Absyn_Decl*_tmp8B0=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp408->hd=_tmp8B0;}),_tmp408->tl=0;_tmp408;}));
# 1383
goto _LL0;}}case 54U: _LL67: _LL68: {
# 1386 "parse.y"
struct _fat_ptr _tmp410=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp410;
Cyc_Parse_tvar_ok(three,(unsigned)((yyyvsp[2]).l).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp413=_cycalloc(sizeof(*_tmp413));({struct _fat_ptr*_tmp8B2=({struct _fat_ptr*_tmp412=_cycalloc(sizeof(*_tmp412));*_tmp412=three;_tmp412;});_tmp413->name=_tmp8B2;}),_tmp413->identity=- 1,({void*_tmp8B1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp413->kind=_tmp8B1;});_tmp413;});
void*t=Cyc_Absyn_var_type(tv);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp411=_cycalloc(sizeof(*_tmp411));({struct Cyc_Absyn_Decl*_tmp8B3=Cyc_Absyn_pool_decl(tv,(unsigned)((yyyvsp[0]).l).first_line);_tmp411->hd=_tmp8B3;}),_tmp411->tl=0;_tmp411;}));
# 1392
goto _LL0;}}case 55U: _LL69: _LL6A:
# 1396 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 56U: _LL6B: _LL6C:
# 1398 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8B4=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8B4,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 57U: _LL6D: _LL6E:
# 1404 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp726;({enum Cyc_Parse_Storage_class _tmp8B7=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp726.sc=_tmp8B7;}),({struct Cyc_Absyn_Tqual _tmp8B6=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp726.tq=_tmp8B6;}),({
struct Cyc_Parse_Type_specifier _tmp8B5=Cyc_Parse_empty_spec(0U);_tmp726.type_specs=_tmp8B5;}),_tmp726.is_inline=0,_tmp726.attributes=0;_tmp726;}));
goto _LL0;case 58U: _LL6F: _LL70: {
# 1407 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp414=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp414;
if((int)two.sc != (int)Cyc_Parse_None_sc)
({void*_tmp415=0U;({unsigned _tmp8B9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8B8=({const char*_tmp416="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp416,sizeof(char),73U);});Cyc_Warn_warn(_tmp8B9,_tmp8B8,_tag_fat(_tmp415,sizeof(void*),0U));});});
# 1411
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp727;({enum Cyc_Parse_Storage_class _tmp8BA=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp727.sc=_tmp8BA;}),_tmp727.tq=two.tq,_tmp727.type_specs=two.type_specs,_tmp727.is_inline=two.is_inline,_tmp727.attributes=two.attributes;_tmp727;}));
# 1415
goto _LL0;}case 59U: _LL71: _LL72:
# 1416 "parse.y"
({void*_tmp417=0U;({unsigned _tmp8BC=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8BB=({const char*_tmp418="__extension__ keyword ignored in declaration";_tag_fat(_tmp418,sizeof(char),45U);});Cyc_Warn_warn(_tmp8BC,_tmp8BB,_tag_fat(_tmp417,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1419
goto _LL0;case 60U: _LL73: _LL74:
# 1420 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp728;_tmp728.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8BE=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp728.tq=_tmp8BE;}),({
struct Cyc_Parse_Type_specifier _tmp8BD=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp728.type_specs=_tmp8BD;}),_tmp728.is_inline=0,_tmp728.attributes=0;_tmp728;}));
goto _LL0;case 61U: _LL75: _LL76: {
# 1423 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp419=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp419;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp729;_tmp729.sc=two.sc,_tmp729.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8C1=({unsigned _tmp8C0=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8BF=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8C0,_tmp8BF,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp729.type_specs=_tmp8C1;}),_tmp729.is_inline=two.is_inline,_tmp729.attributes=two.attributes;_tmp729;}));
# 1429
goto _LL0;}case 62U: _LL77: _LL78:
# 1430 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72A;_tmp72A.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8C3=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp72A.tq=_tmp8C3;}),({struct Cyc_Parse_Type_specifier _tmp8C2=Cyc_Parse_empty_spec(0U);_tmp72A.type_specs=_tmp8C2;}),_tmp72A.is_inline=0,_tmp72A.attributes=0;_tmp72A;}));
goto _LL0;case 63U: _LL79: _LL7A: {
# 1432 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41A=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41A;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72B;_tmp72B.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8C5=({struct Cyc_Absyn_Tqual _tmp8C4=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8C4,two.tq);});_tmp72B.tq=_tmp8C5;}),_tmp72B.type_specs=two.type_specs,_tmp72B.is_inline=two.is_inline,_tmp72B.attributes=two.attributes;_tmp72B;}));
# 1437
goto _LL0;}case 64U: _LL7B: _LL7C:
# 1438 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72C;_tmp72C.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8C7=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72C.tq=_tmp8C7;}),({
struct Cyc_Parse_Type_specifier _tmp8C6=Cyc_Parse_empty_spec(0U);_tmp72C.type_specs=_tmp8C6;}),_tmp72C.is_inline=1,_tmp72C.attributes=0;_tmp72C;}));
goto _LL0;case 65U: _LL7D: _LL7E: {
# 1441 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41B=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41B;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72D;_tmp72D.sc=two.sc,_tmp72D.tq=two.tq,_tmp72D.type_specs=two.type_specs,_tmp72D.is_inline=1,_tmp72D.attributes=two.attributes;_tmp72D;}));
# 1445
goto _LL0;}case 66U: _LL7F: _LL80:
# 1446 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72E;_tmp72E.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8CA=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72E.tq=_tmp8CA;}),({
struct Cyc_Parse_Type_specifier _tmp8C9=Cyc_Parse_empty_spec(0U);_tmp72E.type_specs=_tmp8C9;}),_tmp72E.is_inline=0,({struct Cyc_List_List*_tmp8C8=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp72E.attributes=_tmp8C8;});_tmp72E;}));
goto _LL0;case 67U: _LL81: _LL82: {
# 1449 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41C=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41C;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72F;_tmp72F.sc=two.sc,_tmp72F.tq=two.tq,_tmp72F.type_specs=two.type_specs,_tmp72F.is_inline=two.is_inline,({
# 1452
struct Cyc_List_List*_tmp8CC=({struct Cyc_List_List*_tmp8CB=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8CB,two.attributes);});_tmp72F.attributes=_tmp8CC;});_tmp72F;}));
goto _LL0;}case 68U: _LL83: _LL84:
# 1456 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Auto_sc);
goto _LL0;case 69U: _LL85: _LL86:
# 1457 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Register_sc);
goto _LL0;case 70U: _LL87: _LL88:
# 1458 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Static_sc);
goto _LL0;case 71U: _LL89: _LL8A:
# 1459 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Extern_sc);
goto _LL0;case 72U: _LL8B: _LL8C:
# 1461 "parse.y"
 if(({struct _fat_ptr _tmp8CD=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8CD,({const char*_tmp41D="C";_tag_fat(_tmp41D,sizeof(char),2U);}));})!= 0)
({void*_tmp41E=0U;({unsigned _tmp8CF=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8CE=({const char*_tmp41F="only extern or extern \"C\" is allowed";_tag_fat(_tmp41F,sizeof(char),37U);});Cyc_Warn_err(_tmp8CF,_tmp8CE,_tag_fat(_tmp41E,sizeof(void*),0U));});});
yyval=Cyc_YY20(Cyc_Parse_ExternC_sc);
# 1465
goto _LL0;case 73U: _LL8D: _LL8E:
# 1465 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Typedef_sc);
goto _LL0;case 74U: _LL8F: _LL90:
# 1467 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Abstract_sc);
goto _LL0;case 75U: _LL91: _LL92:
# 1472 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 76U: _LL93: _LL94:
# 1473 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1478 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 78U: _LL97: _LL98:
# 1482 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));({void*_tmp8D0=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp420->hd=_tmp8D0;}),_tmp420->tl=0;_tmp420;}));
goto _LL0;case 79U: _LL99: _LL9A:
# 1483 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));({void*_tmp8D2=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp421->hd=_tmp8D2;}),({struct Cyc_List_List*_tmp8D1=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp421->tl=_tmp8D1;});_tmp421;}));
goto _LL0;case 80U: _LL9B: _LL9C: {
# 1488 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[19U]={{{_tmp425,_tmp425,_tmp425 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp426,_tmp426,_tmp426 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp427,_tmp427,_tmp427 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp428,_tmp428,_tmp428 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp429,_tmp429,_tmp429 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp42A,_tmp42A,_tmp42A + 8U},(void*)& att_aligned},{{_tmp42B,_tmp42B,_tmp42B + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp42C,_tmp42C,_tmp42C + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp42D,_tmp42D,_tmp42D + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp42E,_tmp42E,_tmp42E + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp42F,_tmp42F,_tmp42F + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp430,_tmp430,_tmp430 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp431,_tmp431,_tmp431 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp432,_tmp432,_tmp432 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp433,_tmp433,_tmp433 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp434,_tmp434,_tmp434 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp435,_tmp435,_tmp435 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp436,_tmp436,_tmp436 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val},{{_tmp437,_tmp437,_tmp437 + 9U},(void*)& Cyc_Absyn_No_throw_att_val}};
# 1510
struct _fat_ptr _tmp422=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp422;
# 1512
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1514
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 19U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,19U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1521
if((unsigned)i == 19U){
({void*_tmp423=0U;({unsigned _tmp8D4=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8D3=({const char*_tmp424="unrecognized attribute";_tag_fat(_tmp424,sizeof(char),23U);});Cyc_Warn_err(_tmp8D4,_tmp8D3,_tag_fat(_tmp423,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1526
goto _LL0;}}case 81U: _LL9D: _LL9E:
# 1526 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 82U: _LL9F: _LLA0: {
# 1528 "parse.y"
struct _fat_ptr _tmp438=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp438;
struct Cyc_Absyn_Exp*_tmp439=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp439;
void*a;
if(({struct _fat_ptr _tmp8D6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D6,({const char*_tmp43A="aligned";_tag_fat(_tmp43A,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D5=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D5,({const char*_tmp43B="__aligned__";_tag_fat(_tmp43B,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=6U,_tmp43C->f1=e;_tmp43C;});else{
if(({struct _fat_ptr _tmp8D8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D8,({const char*_tmp43D="section";_tag_fat(_tmp43D,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D7,({const char*_tmp43E="__section__";_tag_fat(_tmp43E,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=8U,({struct _fat_ptr _tmp8D9=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp43F->f1=_tmp8D9;});_tmp43F;});else{
if(({struct _fat_ptr _tmp8DA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DA,({const char*_tmp440="__mode__";_tag_fat(_tmp440,sizeof(char),9U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->tag=24U,({struct _fat_ptr _tmp8DB=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp441->f1=_tmp8DB;});_tmp441;});else{
if(({struct _fat_ptr _tmp8DC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DC,({const char*_tmp442="alias";_tag_fat(_tmp442,sizeof(char),6U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->tag=25U,({struct _fat_ptr _tmp8DD=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp443->f1=_tmp8DD;});_tmp443;});else{
# 1540
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8DF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DF,({const char*_tmp444="regparm";_tag_fat(_tmp444,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8DE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DE,({const char*_tmp445="__regparm__";_tag_fat(_tmp445,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp446=0U;({unsigned _tmp8E1=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8E0=({const char*_tmp447="regparm requires value between 0 and 3";_tag_fat(_tmp447,sizeof(char),39U);});Cyc_Warn_err(_tmp8E1,_tmp8E0,_tag_fat(_tmp446,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->tag=0U,_tmp448->f1=n;_tmp448;});}else{
if(({struct _fat_ptr _tmp8E3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E3,({const char*_tmp449="initializes";_tag_fat(_tmp449,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8E2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E2,({const char*_tmp44A="__initializes__";_tag_fat(_tmp44A,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=20U,_tmp44B->f1=n;_tmp44B;});else{
if(({struct _fat_ptr _tmp8E5=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E5,({const char*_tmp44C="noliveunique";_tag_fat(_tmp44C,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8E4=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E4,({const char*_tmp44D="__noliveunique__";_tag_fat(_tmp44D,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=21U,_tmp44E->f1=n;_tmp44E;});else{
if(({struct _fat_ptr _tmp8E7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E7,({const char*_tmp44F="consume";_tag_fat(_tmp44F,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8E6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E6,({const char*_tmp450="__consume__";_tag_fat(_tmp450,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=22U,_tmp451->f1=n;_tmp451;});else{
# 1552
({void*_tmp452=0U;({unsigned _tmp8E9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8E8=({const char*_tmp453="unrecognized attribute";_tag_fat(_tmp453,sizeof(char),23U);});Cyc_Warn_err(_tmp8E9,_tmp8E8,_tag_fat(_tmp452,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1556
yyval=Cyc_YY46(a);
# 1558
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1559 "parse.y"
struct _fat_ptr _tmp454=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp454;
struct _fat_ptr _tmp455=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp455;
unsigned _tmp456=({unsigned _tmp8EA=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8EA,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp456;
unsigned _tmp457=({unsigned _tmp8EB=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8EB,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp457;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8ED=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8ED,({const char*_tmp458="format";_tag_fat(_tmp458,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8EC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8EC,({const char*_tmp459="__format__";_tag_fat(_tmp459,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8EF=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8EF,({const char*_tmp45A="printf";_tag_fat(_tmp45A,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8EE=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8EE,({const char*_tmp45B="__printf__";_tag_fat(_tmp45B,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=19U,_tmp45C->f1=Cyc_Absyn_Printf_ft,_tmp45C->f2=(int)n,_tmp45C->f3=(int)m;_tmp45C;});else{
if(({struct _fat_ptr _tmp8F1=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8F1,({const char*_tmp45D="scanf";_tag_fat(_tmp45D,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8F0=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8F0,({const char*_tmp45E="__scanf__";_tag_fat(_tmp45E,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=19U,_tmp45F->f1=Cyc_Absyn_Scanf_ft,_tmp45F->f2=(int)n,_tmp45F->f3=(int)m;_tmp45F;});else{
# 1570
({void*_tmp460=0U;({unsigned _tmp8F3=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8F2=({const char*_tmp461="unrecognized format type";_tag_fat(_tmp461,sizeof(char),25U);});Cyc_Warn_err(_tmp8F3,_tmp8F2,_tag_fat(_tmp460,sizeof(void*),0U));});});}}}else{
# 1572
({void*_tmp462=0U;({unsigned _tmp8F5=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8F4=({const char*_tmp463="unrecognized attribute";_tag_fat(_tmp463,sizeof(char),23U);});Cyc_Warn_err(_tmp8F5,_tmp8F4,_tag_fat(_tmp462,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1575
goto _LL0;}case 84U: _LLA3: _LLA4:
# 1584 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 85U: _LLA5: _LLA6:
# 1586 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F7=({struct _tuple0*_tmp8F6=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp8F6,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp8F7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1590 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1591 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1592 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1593 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1594 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1595 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1596 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1597 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1598 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 95U: _LLB9: _LLBA:
# 1599 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1600 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 97U: _LLBD: _LLBE:
# 1603
 yyval=Cyc_YY21(({void*_tmp8F8=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1605 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F9=Cyc_Absyn_builtin_type(({const char*_tmp464="__builtin_va_list";_tag_fat(_tmp464,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp8F9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 99U: _LLC1: _LLC2:
# 1607 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 100U: _LLC3: _LLC4:
# 1609 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FA=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp8FA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1611 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FB=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8FB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1613 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FC=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp8FC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 103U: _LLC9: _LLCA:
# 1615 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FF=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->tag=6U,({struct Cyc_List_List*_tmp8FE=({unsigned _tmp8FD=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8FD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});_tmp465->f1=_tmp8FE;});_tmp465;});Cyc_Parse_type_spec(_tmp8FF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1617
goto _LL0;case 104U: _LLCB: _LLCC:
# 1618 "parse.y"
 yyval=Cyc_YY21(({void*_tmp900=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp900,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 105U: _LLCD: _LLCE:
# 1620 "parse.y"
 yyval=Cyc_YY21(({void*_tmp901=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp901,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1622
goto _LL0;case 106U: _LLCF: _LLD0:
# 1623 "parse.y"
 yyval=Cyc_YY21(({void*_tmp902=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp902,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1625 "parse.y"
 yyval=Cyc_YY21(({void*_tmp903=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp903,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1627 "parse.y"
 yyval=Cyc_YY21(({void*_tmp904=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp904,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6:
# 1632 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp905=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp905,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 110U: _LLD7: _LLD8: {
# 1636 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp730;_tmp730.print_const=1,_tmp730.q_volatile=0,_tmp730.q_restrict=0,_tmp730.real_const=1,_tmp730.loc=loc;_tmp730;}));
goto _LL0;}case 111U: _LLD9: _LLDA:
# 1638 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp731;_tmp731.print_const=0,_tmp731.q_volatile=1,_tmp731.q_restrict=0,_tmp731.real_const=0,_tmp731.loc=0U;_tmp731;}));
goto _LL0;case 112U: _LLDB: _LLDC:
# 1639 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp732;_tmp732.print_const=0,_tmp732.q_volatile=0,_tmp732.q_restrict=1,_tmp732.real_const=0,_tmp732.loc=0U;_tmp732;}));
goto _LL0;case 113U: _LLDD: _LLDE: {
# 1645 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp466=({struct Cyc_Absyn_TypeDecl*_tmp46B=_cycalloc(sizeof(*_tmp46B));({void*_tmp90A=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp909=({struct Cyc_Absyn_Enumdecl*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp908=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp469->name=_tmp908;}),({struct Cyc_Core_Opt*_tmp907=({struct Cyc_Core_Opt*_tmp468=_cycalloc(sizeof(*_tmp468));({struct Cyc_List_List*_tmp906=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp468->v=_tmp906;});_tmp468;});_tmp469->fields=_tmp907;});_tmp469;});_tmp46A->f1=_tmp909;});_tmp46A;});_tmp46B->r=_tmp90A;}),_tmp46B->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46B;});struct Cyc_Absyn_TypeDecl*ed=_tmp466;
# 1647
yyval=Cyc_YY21(({void*_tmp90B=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=10U,_tmp467->f1=ed,_tmp467->f2=0;_tmp467;});Cyc_Parse_type_spec(_tmp90B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1649
goto _LL0;}case 114U: _LLDF: _LLE0:
# 1650 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90C=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp90C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1652 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90D=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp90D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1658 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46C=_cycalloc(sizeof(*_tmp46C));({struct _tuple0*_tmp90E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46C->name=_tmp90E;}),_tmp46C->tag=0,_tmp46C->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46C;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1660 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46D=_cycalloc(sizeof(*_tmp46D));({struct _tuple0*_tmp910=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46D->name=_tmp910;}),({struct Cyc_Absyn_Exp*_tmp90F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp46D->tag=_tmp90F;}),_tmp46D->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46D;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1664 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct Cyc_Absyn_Enumfield*_tmp911=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46E->hd=_tmp911;}),_tmp46E->tl=0;_tmp46E;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1665 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));({struct Cyc_Absyn_Enumfield*_tmp912=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46F->hd=_tmp912;}),_tmp46F->tl=0;_tmp46F;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1666 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp470=_cycalloc(sizeof(*_tmp470));({struct Cyc_Absyn_Enumfield*_tmp914=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp470->hd=_tmp914;}),({struct Cyc_List_List*_tmp913=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp470->tl=_tmp913;});_tmp470;}));
goto _LL0;case 121U: _LLED: _LLEE:
# 1672 "parse.y"
 yyval=Cyc_YY21(({void*_tmp917=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=7U,({enum Cyc_Absyn_AggrKind _tmp916=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp471->f1=_tmp916;}),({struct Cyc_List_List*_tmp915=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp471->f2=_tmp915;});_tmp471;});Cyc_Parse_type_spec(_tmp917,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 122U: _LLEF: _LLF0: {
# 1678 "parse.y"
struct Cyc_List_List*_tmp472=({unsigned _tmp918=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp918,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp472;
struct Cyc_List_List*_tmp473=({unsigned _tmp919=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp919,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp473;
struct Cyc_Absyn_TypeDecl*_tmp474=({enum Cyc_Absyn_AggrKind _tmp91F=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp91E=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp91D=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp91C=({
struct Cyc_List_List*_tmp91B=exist_ts;struct Cyc_List_List*_tmp91A=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp91B,_tmp91A,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1680
Cyc_Absyn_aggr_tdecl(_tmp91F,Cyc_Absyn_Public,_tmp91E,_tmp91D,_tmp91C,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp474;
# 1683
yyval=Cyc_YY21(({void*_tmp920=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=10U,_tmp475->f1=td,_tmp475->f2=0;_tmp475;});Cyc_Parse_type_spec(_tmp920,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1685
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1689 "parse.y"
struct Cyc_List_List*_tmp476=({unsigned _tmp921=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp921,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp476;
struct Cyc_List_List*_tmp477=({unsigned _tmp922=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp922,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp477;
struct Cyc_Absyn_TypeDecl*_tmp478=({enum Cyc_Absyn_AggrKind _tmp928=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp927=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp926=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp925=({
struct Cyc_List_List*_tmp924=exist_ts;struct Cyc_List_List*_tmp923=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp924,_tmp923,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1691
Cyc_Absyn_aggr_tdecl(_tmp928,Cyc_Absyn_Public,_tmp927,_tmp926,_tmp925,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp478;
# 1694
yyval=Cyc_YY21(({void*_tmp929=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->tag=10U,_tmp479->f1=td,_tmp479->f2=0;_tmp479;});Cyc_Parse_type_spec(_tmp929,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1696
goto _LL0;}case 124U: _LLF3: _LLF4:
# 1697 "parse.y"
 yyval=Cyc_YY21(({void*_tmp92D=({union Cyc_Absyn_AggrInfo _tmp92C=({enum Cyc_Absyn_AggrKind _tmp92B=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp92A=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp92B,_tmp92A,({struct Cyc_Core_Opt*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->v=(void*)1;_tmp47A;}));});Cyc_Absyn_aggr_type(_tmp92C,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp92D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1700
goto _LL0;case 125U: _LLF5: _LLF6:
# 1701 "parse.y"
 yyval=Cyc_YY21(({void*_tmp930=({union Cyc_Absyn_AggrInfo _tmp92F=({enum Cyc_Absyn_AggrKind _tmp92E=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp92E,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp92F,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp930,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 126U: _LLF7: _LLF8:
# 1706 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 127U: _LLF9: _LLFA:
# 1708 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 128U: _LLFB: _LLFC:
# 1712 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1713 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 130U: _LLFF: _LL100:
# 1718 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 131U: _LL101: _LL102: {
# 1720 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp47B=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp47B;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1724
struct Cyc_List_List*_tmp47C=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp47C;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1729
goto _LL0;}}case 132U: _LL103: _LL104:
# 1734 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct Cyc_List_List*_tmp931=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp47D->hd=_tmp931;}),_tmp47D->tl=0;_tmp47D;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1736 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct Cyc_List_List*_tmp933=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp47E->hd=_tmp933;}),({struct Cyc_List_List*_tmp932=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp47E->tl=_tmp932;});_tmp47E;}));
goto _LL0;case 134U: _LL107: _LL108:
# 1740 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 135U: _LL109: _LL10A:
# 1746 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp47F=_region_malloc(yyr,sizeof(*_tmp47F));_tmp47F->tl=0,({struct _tuple12 _tmp934=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp47F->hd=_tmp934;});_tmp47F;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1748 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp480=_region_malloc(yyr,sizeof(*_tmp480));({struct _tuple13*_tmp936=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp480->tl=_tmp936;}),({struct _tuple12 _tmp935=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp480->hd=_tmp935;});_tmp480;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1753 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp733;({struct Cyc_Parse_Declarator _tmp937=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp733.f1=_tmp937;}),_tmp733.f2=0;_tmp733;}));
goto _LL0;case 138U: _LL10F: _LL110:
# 1755 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp734;({struct Cyc_Parse_Declarator _tmp939=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp734.f1=_tmp939;}),({struct Cyc_Absyn_Exp*_tmp938=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp734.f2=_tmp938;});_tmp734;}));
goto _LL0;case 139U: _LL111: _LL112: {
# 1761 "parse.y"
struct _RegionHandle _tmp481=_new_region("temp");struct _RegionHandle*temp=& _tmp481;_push_region(temp);
{struct _tuple26 _tmp482=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp16=_tmp482;struct _tuple26 _tmp483=_stmttmp16;struct Cyc_List_List*_tmp486;struct Cyc_Parse_Type_specifier _tmp485;struct Cyc_Absyn_Tqual _tmp484;_LL469: _tmp484=_tmp483.f1;_tmp485=_tmp483.f2;_tmp486=_tmp483.f3;_LL46A: {struct Cyc_Absyn_Tqual tq=_tmp484;struct Cyc_Parse_Type_specifier tspecs=_tmp485;struct Cyc_List_List*atts=_tmp486;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp487=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp487;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp488=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp17=_tmp488;struct _tuple25*_tmp489=_stmttmp17;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Parse_Declarator _tmp48A;_LL46C: _tmp48A=_tmp489->f1;_tmp48B=_tmp489->f2;_tmp48C=_tmp489->f3;_LL46D: {struct Cyc_Parse_Declarator d=_tmp48A;struct Cyc_Absyn_Exp*wd=_tmp48B;struct Cyc_Absyn_Exp*wh=_tmp48C;
decls=({struct _tuple11*_tmp48D=_region_malloc(temp,sizeof(*_tmp48D));_tmp48D->tl=decls,_tmp48D->hd=d;_tmp48D;});
widths_and_reqs=({struct Cyc_List_List*_tmp48F=_region_malloc(temp,sizeof(*_tmp48F));
({struct _tuple17*_tmp93A=({struct _tuple17*_tmp48E=_region_malloc(temp,sizeof(*_tmp48E));_tmp48E->f1=wd,_tmp48E->f2=wh;_tmp48E;});_tmp48F->hd=_tmp93A;}),_tmp48F->tl=widths_and_reqs;_tmp48F;});}}}
# 1772
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp490=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp490;
struct Cyc_List_List*_tmp491=({struct _RegionHandle*_tmp93D=temp;struct _RegionHandle*_tmp93C=temp;struct Cyc_List_List*_tmp93B=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1775
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp93D,_tmp93C,_tmp93B,widths_and_reqs);});struct Cyc_List_List*info=_tmp491;
# 1778
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1780
_npop_handler(0U);goto _LL0;}}}}
# 1762
;_pop_region();}case 140U: _LL113: _LL114:
# 1788 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp735;({struct Cyc_Absyn_Tqual _tmp93F=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp735.f1=_tmp93F;}),({struct Cyc_Parse_Type_specifier _tmp93E=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp735.f2=_tmp93E;}),_tmp735.f3=0;_tmp735;}));
goto _LL0;case 141U: _LL115: _LL116: {
# 1790 "parse.y"
struct _tuple26 _tmp492=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp492;yyval=Cyc_YY35(({struct _tuple26 _tmp736;_tmp736.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp942=({unsigned _tmp941=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp940=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp941,_tmp940,two.f2);});_tmp736.f2=_tmp942;}),_tmp736.f3=two.f3;_tmp736;}));
goto _LL0;}case 142U: _LL117: _LL118:
# 1792 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp737;({struct Cyc_Absyn_Tqual _tmp944=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp737.f1=_tmp944;}),({struct Cyc_Parse_Type_specifier _tmp943=Cyc_Parse_empty_spec(0U);_tmp737.f2=_tmp943;}),_tmp737.f3=0;_tmp737;}));
goto _LL0;case 143U: _LL119: _LL11A: {
# 1794 "parse.y"
struct _tuple26 _tmp493=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp493;
yyval=Cyc_YY35(({struct _tuple26 _tmp738;({struct Cyc_Absyn_Tqual _tmp946=({struct Cyc_Absyn_Tqual _tmp945=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp945,two.f1);});_tmp738.f1=_tmp946;}),_tmp738.f2=two.f2,_tmp738.f3=two.f3;_tmp738;}));
goto _LL0;}case 144U: _LL11B: _LL11C:
# 1797 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp739;({struct Cyc_Absyn_Tqual _tmp949=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp739.f1=_tmp949;}),({struct Cyc_Parse_Type_specifier _tmp948=Cyc_Parse_empty_spec(0U);_tmp739.f2=_tmp948;}),({struct Cyc_List_List*_tmp947=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp739.f3=_tmp947;});_tmp739;}));
goto _LL0;case 145U: _LL11D: _LL11E: {
# 1799 "parse.y"
struct _tuple26 _tmp494=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp494;yyval=Cyc_YY35(({struct _tuple26 _tmp73A;_tmp73A.f1=two.f1,_tmp73A.f2=two.f2,({struct Cyc_List_List*_tmp94B=({struct Cyc_List_List*_tmp94A=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp94A,two.f3);});_tmp73A.f3=_tmp94B;});_tmp73A;}));
goto _LL0;}case 146U: _LL11F: _LL120:
# 1805 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73B;({struct Cyc_Absyn_Tqual _tmp94D=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73B.f1=_tmp94D;}),({struct Cyc_Parse_Type_specifier _tmp94C=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp73B.f2=_tmp94C;}),_tmp73B.f3=0;_tmp73B;}));
goto _LL0;case 147U: _LL121: _LL122: {
# 1807 "parse.y"
struct _tuple26 _tmp495=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp495;yyval=Cyc_YY35(({struct _tuple26 _tmp73C;_tmp73C.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp950=({unsigned _tmp94F=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp94E=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp94F,_tmp94E,two.f2);});_tmp73C.f2=_tmp950;}),_tmp73C.f3=two.f3;_tmp73C;}));
goto _LL0;}case 148U: _LL123: _LL124:
# 1809 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73D;({struct Cyc_Absyn_Tqual _tmp952=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp73D.f1=_tmp952;}),({struct Cyc_Parse_Type_specifier _tmp951=Cyc_Parse_empty_spec(0U);_tmp73D.f2=_tmp951;}),_tmp73D.f3=0;_tmp73D;}));
goto _LL0;case 149U: _LL125: _LL126: {
# 1811 "parse.y"
struct _tuple26 _tmp496=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp496;
yyval=Cyc_YY35(({struct _tuple26 _tmp73E;({struct Cyc_Absyn_Tqual _tmp954=({struct Cyc_Absyn_Tqual _tmp953=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp953,two.f1);});_tmp73E.f1=_tmp954;}),_tmp73E.f2=two.f2,_tmp73E.f3=two.f3;_tmp73E;}));
goto _LL0;}case 150U: _LL127: _LL128:
# 1814 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73F;({struct Cyc_Absyn_Tqual _tmp957=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73F.f1=_tmp957;}),({struct Cyc_Parse_Type_specifier _tmp956=Cyc_Parse_empty_spec(0U);_tmp73F.f2=_tmp956;}),({struct Cyc_List_List*_tmp955=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp73F.f3=_tmp955;});_tmp73F;}));
goto _LL0;case 151U: _LL129: _LL12A: {
# 1816 "parse.y"
struct _tuple26 _tmp497=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp497;yyval=Cyc_YY35(({struct _tuple26 _tmp740;_tmp740.f1=two.f1,_tmp740.f2=two.f2,({struct Cyc_List_List*_tmp959=({struct Cyc_List_List*_tmp958=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp958,two.f3);});_tmp740.f3=_tmp959;});_tmp740;}));
goto _LL0;}case 152U: _LL12B: _LL12C:
# 1820 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1826 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));({struct _tuple25*_tmp95A=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp498->hd=_tmp95A;}),_tmp498->tl=0;_tmp498;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1828 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp499=_region_malloc(yyr,sizeof(*_tmp499));({struct _tuple25*_tmp95C=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp499->hd=_tmp95C;}),({struct Cyc_List_List*_tmp95B=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp499->tl=_tmp95B;});_tmp499;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1833 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49A=_region_malloc(yyr,sizeof(*_tmp49A));({struct Cyc_Parse_Declarator _tmp95E=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp49A->f1=_tmp95E;}),_tmp49A->f2=0,({struct Cyc_Absyn_Exp*_tmp95D=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp49A->f3=_tmp95D;});_tmp49A;}));
goto _LL0;case 156U: _LL133: _LL134:
# 1837 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));({struct _tuple0*_tmp963=({struct _tuple0*_tmp49D=_cycalloc(sizeof(*_tmp49D));({union Cyc_Absyn_Nmspace _tmp962=Cyc_Absyn_Rel_n(0);_tmp49D->f1=_tmp962;}),({struct _fat_ptr*_tmp961=({struct _fat_ptr*_tmp49C=_cycalloc(sizeof(*_tmp49C));({struct _fat_ptr _tmp960=({const char*_tmp49B="";_tag_fat(_tmp49B,sizeof(char),1U);});*_tmp49C=_tmp960;});_tmp49C;});_tmp49D->f2=_tmp961;});_tmp49D;});(_tmp49E->f1).id=_tmp963;}),(_tmp49E->f1).varloc=0U,(_tmp49E->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp95F=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp49E->f2=_tmp95F;}),_tmp49E->f3=0;_tmp49E;}));
# 1839
goto _LL0;case 157U: _LL135: _LL136:
# 1842 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A2=_region_malloc(yyr,sizeof(*_tmp4A2));({struct _tuple0*_tmp967=({struct _tuple0*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));({union Cyc_Absyn_Nmspace _tmp966=Cyc_Absyn_Rel_n(0);_tmp4A1->f1=_tmp966;}),({struct _fat_ptr*_tmp965=({struct _fat_ptr*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));({struct _fat_ptr _tmp964=({const char*_tmp49F="";_tag_fat(_tmp49F,sizeof(char),1U);});*_tmp4A0=_tmp964;});_tmp4A0;});_tmp4A1->f2=_tmp965;});_tmp4A1;});(_tmp4A2->f1).id=_tmp967;}),(_tmp4A2->f1).varloc=0U,(_tmp4A2->f1).tms=0,_tmp4A2->f2=0,_tmp4A2->f3=0;_tmp4A2;}));
# 1844
goto _LL0;case 158U: _LL137: _LL138:
# 1845 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A3=_region_malloc(yyr,sizeof(*_tmp4A3));({struct Cyc_Parse_Declarator _tmp969=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp4A3->f1=_tmp969;}),({struct Cyc_Absyn_Exp*_tmp968=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4A3->f2=_tmp968;}),_tmp4A3->f3=0;_tmp4A3;}));
goto _LL0;case 159U: _LL139: _LL13A:
# 1849 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 160U: _LL13B: _LL13C:
# 1850 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 161U: _LL13D: _LL13E:
# 1854 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 162U: _LL13F: _LL140:
# 1855 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 163U: _LL141: _LL142: {
# 1861 "parse.y"
int _tmp4A4=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A4;
struct Cyc_List_List*_tmp4A5=({unsigned _tmp96A=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp96A,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp4A5;
struct Cyc_Absyn_TypeDecl*_tmp4A6=({struct _tuple0*_tmp96F=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp96E=ts;struct Cyc_Core_Opt*_tmp96D=({struct Cyc_Core_Opt*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));({struct Cyc_List_List*_tmp96B=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp4A8->v=_tmp96B;});_tmp4A8;});int _tmp96C=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp96F,_tmp96E,_tmp96D,_tmp96C,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp4A6;
# 1865
yyval=Cyc_YY21(({void*_tmp970=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->tag=10U,_tmp4A7->f1=dd,_tmp4A7->f2=0;_tmp4A7;});Cyc_Parse_type_spec(_tmp970,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1867
goto _LL0;}case 164U: _LL143: _LL144: {
# 1868 "parse.y"
int _tmp4A9=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A9;
yyval=Cyc_YY21(({void*_tmp973=({union Cyc_Absyn_DatatypeInfo _tmp972=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp741;({struct _tuple0*_tmp971=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp741.name=_tmp971;}),_tmp741.is_extensible=is_extensible;_tmp741;}));Cyc_Absyn_datatype_type(_tmp972,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp973,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1871
goto _LL0;}case 165U: _LL145: _LL146: {
# 1872 "parse.y"
int _tmp4AA=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4AA;
yyval=Cyc_YY21(({void*_tmp977=({union Cyc_Absyn_DatatypeFieldInfo _tmp976=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp742;({struct _tuple0*_tmp975=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp742.datatype_name=_tmp975;}),({struct _tuple0*_tmp974=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp742.field_name=_tmp974;}),_tmp742.is_extensible=is_extensible;_tmp742;}));Cyc_Absyn_datatype_field_type(_tmp976,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp977,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1875
goto _LL0;}case 166U: _LL147: _LL148:
# 1878 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 167U: _LL149: _LL14A:
# 1879 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 168U: _LL14B: _LL14C:
# 1883 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct Cyc_Absyn_Datatypefield*_tmp978=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AB->hd=_tmp978;}),_tmp4AB->tl=0;_tmp4AB;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1884 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));({struct Cyc_Absyn_Datatypefield*_tmp979=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AC->hd=_tmp979;}),_tmp4AC->tl=0;_tmp4AC;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1885 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct Cyc_Absyn_Datatypefield*_tmp97B=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AD->hd=_tmp97B;}),({struct Cyc_List_List*_tmp97A=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AD->tl=_tmp97A;});_tmp4AD;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1886 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));({struct Cyc_Absyn_Datatypefield*_tmp97D=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AE->hd=_tmp97D;}),({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AE->tl=_tmp97C;});_tmp4AE;}));
goto _LL0;case 172U: _LL153: _LL154:
# 1890 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 173U: _LL155: _LL156:
# 1891 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 174U: _LL157: _LL158:
# 1892 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 175U: _LL159: _LL15A:
# 1896 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({struct _tuple0*_tmp97F=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4AF->name=_tmp97F;}),_tmp4AF->typs=0,_tmp4AF->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp97E=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4AF->sc=_tmp97E;});_tmp4AF;}));
goto _LL0;case 176U: _LL15B: _LL15C: {
# 1898 "parse.y"
struct Cyc_List_List*_tmp4B0=({unsigned _tmp980=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp980,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4B0;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct _tuple0*_tmp982=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4B1->name=_tmp982;}),_tmp4B1->typs=typs,_tmp4B1->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp981=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4B1->sc=_tmp981;});_tmp4B1;}));
goto _LL0;}case 177U: _LL15D: _LL15E:
# 1904 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 178U: _LL15F: _LL160: {
# 1906 "parse.y"
struct Cyc_Parse_Declarator _tmp4B2=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp743;_tmp743.id=two.id,_tmp743.varloc=two.varloc,({struct Cyc_List_List*_tmp984=({struct Cyc_List_List*_tmp983=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp983,two.tms);});_tmp743.tms=_tmp984;});_tmp743;}));
goto _LL0;}case 179U: _LL161: _LL162:
# 1913 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 180U: _LL163: _LL164: {
# 1915 "parse.y"
struct Cyc_Parse_Declarator _tmp4B3=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp744;_tmp744.id=two.id,_tmp744.varloc=two.varloc,({struct Cyc_List_List*_tmp986=({struct Cyc_List_List*_tmp985=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp985,two.tms);});_tmp744.tms=_tmp986;});_tmp744;}));
goto _LL0;}case 181U: _LL165: _LL166:
# 1921 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;({struct _tuple0*_tmp987=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp745.id=_tmp987;}),_tmp745.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp745.tms=0;_tmp745;}));
goto _LL0;case 182U: _LL167: _LL168:
# 1923 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 183U: _LL169: _LL16A: {
# 1927 "parse.y"
struct Cyc_Parse_Declarator _tmp4B4=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4B4;
({struct Cyc_List_List*_tmp98A=({struct Cyc_List_List*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));({void*_tmp989=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));_tmp4B5->tag=5U,_tmp4B5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp988=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4B5->f2=_tmp988;});_tmp4B5;});_tmp4B6->hd=_tmp989;}),_tmp4B6->tl=d.tms;_tmp4B6;});d.tms=_tmp98A;});
yyval=(yyyvsp[2]).v;
# 1931
goto _LL0;}case 184U: _LL16B: _LL16C:
# 1932 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;({struct _tuple0*_tmp990=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp746.id=_tmp990;}),({unsigned _tmp98F=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp746.varloc=_tmp98F;}),({struct Cyc_List_List*_tmp98E=({struct Cyc_List_List*_tmp4B8=_region_malloc(yyr,sizeof(*_tmp4B8));({void*_tmp98D=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));_tmp4B7->tag=0U,({void*_tmp98C=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4B7->f1=_tmp98C;}),_tmp4B7->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4B7;});_tmp4B8->hd=_tmp98D;}),({struct Cyc_List_List*_tmp98B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B8->tl=_tmp98B;});_tmp4B8;});_tmp746.tms=_tmp98E;});_tmp746;}));
goto _LL0;case 185U: _LL16D: _LL16E:
# 1934 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;({struct _tuple0*_tmp997=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp747.id=_tmp997;}),({unsigned _tmp996=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp747.varloc=_tmp996;}),({
struct Cyc_List_List*_tmp995=({struct Cyc_List_List*_tmp4BA=_region_malloc(yyr,sizeof(*_tmp4BA));({void*_tmp994=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B9=_region_malloc(yyr,sizeof(*_tmp4B9));_tmp4B9->tag=1U,({struct Cyc_Absyn_Exp*_tmp993=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4B9->f1=_tmp993;}),({void*_tmp992=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B9->f2=_tmp992;}),_tmp4B9->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4B9;});_tmp4BA->hd=_tmp994;}),({struct Cyc_List_List*_tmp991=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4BA->tl=_tmp991;});_tmp4BA;});_tmp747.tms=_tmp995;});_tmp747;}));
goto _LL0;case 186U: _LL16F: _LL170: {
# 1937 "parse.y"
struct _tuple27*_tmp4BB=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp18=_tmp4BB;struct _tuple27*_tmp4BC=_stmttmp18;struct Cyc_List_List*_tmp4C1;void*_tmp4C0;struct Cyc_Absyn_VarargInfo*_tmp4BF;int _tmp4BE;struct Cyc_List_List*_tmp4BD;_LL46F: _tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BC->f2;_tmp4BF=_tmp4BC->f3;_tmp4C0=_tmp4BC->f4;_tmp4C1=_tmp4BC->f5;_LL470: {struct Cyc_List_List*lis=_tmp4BD;int b=_tmp4BE;struct Cyc_Absyn_VarargInfo*c=_tmp4BF;void*eff=_tmp4C0;struct Cyc_List_List*po=_tmp4C1;
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4C2;
struct Cyc_Absyn_Exp*_tmp4C3=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4C3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;({struct _tuple0*_tmp99D=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp748.id=_tmp99D;}),({unsigned _tmp99C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp748.varloc=_tmp99C;}),({struct Cyc_List_List*_tmp99B=({struct Cyc_List_List*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));({void*_tmp99A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));_tmp4C5->tag=3U,({void*_tmp999=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));_tmp4C4->tag=1U,_tmp4C4->f1=lis,_tmp4C4->f2=b,_tmp4C4->f3=c,_tmp4C4->f4=eff,_tmp4C4->f5=po,_tmp4C4->f6=req,_tmp4C4->f7=ens;_tmp4C4;});_tmp4C5->f1=_tmp999;});_tmp4C5;});_tmp4C6->hd=_tmp99A;}),({struct Cyc_List_List*_tmp998=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C6->tl=_tmp998;});_tmp4C6;});_tmp748.tms=_tmp99B;});_tmp748;}));
# 1942
goto _LL0;}}case 187U: _LL171: _LL172:
# 1943 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;({struct _tuple0*_tmp9A7=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp749.id=_tmp9A7;}),({unsigned _tmp9A6=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp749.varloc=_tmp9A6;}),({
struct Cyc_List_List*_tmp9A5=({struct Cyc_List_List*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));({void*_tmp9A4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));_tmp4C8->tag=3U,({void*_tmp9A3=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=1U,_tmp4C7->f1=0,_tmp4C7->f2=0,_tmp4C7->f3=0,({
# 1946
void*_tmp9A2=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4C7->f4=_tmp9A2;}),({struct Cyc_List_List*_tmp9A1=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4C7->f5=_tmp9A1;}),({struct Cyc_Absyn_Exp*_tmp9A0=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4C7->f6=_tmp9A0;}),({struct Cyc_Absyn_Exp*_tmp99F=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4C7->f7=_tmp99F;});_tmp4C7;});
# 1944
_tmp4C8->f1=_tmp9A3;});_tmp4C8;});_tmp4C9->hd=_tmp9A4;}),({
# 1947
struct Cyc_List_List*_tmp99E=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C9->tl=_tmp99E;});_tmp4C9;});
# 1944
_tmp749.tms=_tmp9A5;});_tmp749;}));
# 1949
goto _LL0;case 188U: _LL173: _LL174:
# 1950 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;({struct _tuple0*_tmp9AE=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74A.id=_tmp9AE;}),({unsigned _tmp9AD=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74A.varloc=_tmp9AD;}),({struct Cyc_List_List*_tmp9AC=({struct Cyc_List_List*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));({void*_tmp9AB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));_tmp4CB->tag=3U,({void*_tmp9AA=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=0U,({struct Cyc_List_List*_tmp9A9=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4CA->f1=_tmp9A9;}),_tmp4CA->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4CA;});_tmp4CB->f1=_tmp9AA;});_tmp4CB;});_tmp4CC->hd=_tmp9AB;}),({struct Cyc_List_List*_tmp9A8=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CC->tl=_tmp9A8;});_tmp4CC;});_tmp74A.tms=_tmp9AC;});_tmp74A;}));
goto _LL0;case 189U: _LL175: _LL176: {
# 1953
struct Cyc_List_List*_tmp4CD=({unsigned _tmp9AF=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9AF,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4CD;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;({struct _tuple0*_tmp9B4=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74B.id=_tmp9B4;}),({unsigned _tmp9B3=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74B.varloc=_tmp9B3;}),({struct Cyc_List_List*_tmp9B2=({struct Cyc_List_List*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));({void*_tmp9B1=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));_tmp4CE->tag=4U,_tmp4CE->f1=ts,_tmp4CE->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4CE->f3=0;_tmp4CE;});_tmp4CF->hd=_tmp9B1;}),({struct Cyc_List_List*_tmp9B0=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CF->tl=_tmp9B0;});_tmp4CF;});_tmp74B.tms=_tmp9B2;});_tmp74B;}));
# 1956
goto _LL0;}case 190U: _LL177: _LL178:
# 1957 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74C;({struct _tuple0*_tmp9BA=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74C.id=_tmp9BA;}),({unsigned _tmp9B9=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74C.varloc=_tmp9B9;}),({struct Cyc_List_List*_tmp9B8=({struct Cyc_List_List*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));({void*_tmp9B7=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));_tmp4D0->tag=5U,_tmp4D0->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B6=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D0->f2=_tmp9B6;});_tmp4D0;});_tmp4D1->hd=_tmp9B7;}),({
struct Cyc_List_List*_tmp9B5=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4D1->tl=_tmp9B5;});_tmp4D1;});
# 1957
_tmp74C.tms=_tmp9B8;});_tmp74C;}));
# 1960
goto _LL0;case 191U: _LL179: _LL17A:
# 1965 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74D;({struct _tuple0*_tmp9BB=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp74D.id=_tmp9BB;}),_tmp74D.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp74D.tms=0;_tmp74D;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 1967 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74E;({struct _tuple0*_tmp9BC=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp74E.id=_tmp9BC;}),_tmp74E.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp74E.tms=0;_tmp74E;}));
goto _LL0;case 193U: _LL17D: _LL17E:
# 1969 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 194U: _LL17F: _LL180: {
# 1973 "parse.y"
struct Cyc_Parse_Declarator _tmp4D2=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4D2;
({struct Cyc_List_List*_tmp9BF=({struct Cyc_List_List*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));({void*_tmp9BE=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));_tmp4D3->tag=5U,_tmp4D3->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9BD=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D3->f2=_tmp9BD;});_tmp4D3;});_tmp4D4->hd=_tmp9BE;}),_tmp4D4->tl=d.tms;_tmp4D4;});d.tms=_tmp9BF;});
yyval=(yyyvsp[2]).v;
# 1977
goto _LL0;}case 195U: _LL181: _LL182: {
# 1978 "parse.y"
struct Cyc_Parse_Declarator _tmp4D5=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D5;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74F;_tmp74F.id=one.id,_tmp74F.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C2=({struct Cyc_List_List*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));({void*_tmp9C1=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));_tmp4D6->tag=0U,({void*_tmp9C0=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4D6->f1=_tmp9C0;}),_tmp4D6->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4D6;});_tmp4D7->hd=_tmp9C1;}),_tmp4D7->tl=one.tms;_tmp4D7;});_tmp74F.tms=_tmp9C2;});_tmp74F;}));
goto _LL0;}case 196U: _LL183: _LL184: {
# 1982 "parse.y"
struct Cyc_Parse_Declarator _tmp4D8=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D8;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp750;_tmp750.id=one.id,_tmp750.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C6=({struct Cyc_List_List*_tmp4DA=_region_malloc(yyr,sizeof(*_tmp4DA));({void*_tmp9C5=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));_tmp4D9->tag=1U,({struct Cyc_Absyn_Exp*_tmp9C4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4D9->f1=_tmp9C4;}),({void*_tmp9C3=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4D9->f2=_tmp9C3;}),_tmp4D9->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4D9;});_tmp4DA->hd=_tmp9C5;}),_tmp4DA->tl=one.tms;_tmp4DA;});_tmp750.tms=_tmp9C6;});_tmp750;}));
# 1986
goto _LL0;}case 197U: _LL185: _LL186: {
# 1987 "parse.y"
struct _tuple27*_tmp4DB=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp19=_tmp4DB;struct _tuple27*_tmp4DC=_stmttmp19;struct Cyc_List_List*_tmp4E1;void*_tmp4E0;struct Cyc_Absyn_VarargInfo*_tmp4DF;int _tmp4DE;struct Cyc_List_List*_tmp4DD;_LL472: _tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;_tmp4DF=_tmp4DC->f3;_tmp4E0=_tmp4DC->f4;_tmp4E1=_tmp4DC->f5;_LL473: {struct Cyc_List_List*lis=_tmp4DD;int b=_tmp4DE;struct Cyc_Absyn_VarargInfo*c=_tmp4DF;void*eff=_tmp4E0;struct Cyc_List_List*po=_tmp4E1;
struct Cyc_Absyn_Exp*_tmp4E2=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4E2;
struct Cyc_Absyn_Exp*_tmp4E3=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4E3;
struct Cyc_Parse_Declarator _tmp4E4=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E4;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp751;_tmp751.id=one.id,_tmp751.varloc=one.varloc,({struct Cyc_List_List*_tmp9C9=({struct Cyc_List_List*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));({void*_tmp9C8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E6=_region_malloc(yyr,sizeof(*_tmp4E6));_tmp4E6->tag=3U,({void*_tmp9C7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));_tmp4E5->tag=1U,_tmp4E5->f1=lis,_tmp4E5->f2=b,_tmp4E5->f3=c,_tmp4E5->f4=eff,_tmp4E5->f5=po,_tmp4E5->f6=req,_tmp4E5->f7=ens;_tmp4E5;});_tmp4E6->f1=_tmp9C7;});_tmp4E6;});_tmp4E7->hd=_tmp9C8;}),_tmp4E7->tl=one.tms;_tmp4E7;});_tmp751.tms=_tmp9C9;});_tmp751;}));
# 1993
goto _LL0;}}case 198U: _LL187: _LL188: {
# 1994 "parse.y"
struct Cyc_Parse_Declarator _tmp4E8=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E8;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp752;_tmp752.id=one.id,_tmp752.varloc=one.varloc,({
struct Cyc_List_List*_tmp9D0=({struct Cyc_List_List*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));({void*_tmp9CF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));_tmp4EA->tag=3U,({void*_tmp9CE=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E9=_region_malloc(yyr,sizeof(*_tmp4E9));_tmp4E9->tag=1U,_tmp4E9->f1=0,_tmp4E9->f2=0,_tmp4E9->f3=0,({
# 1998
void*_tmp9CD=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4E9->f4=_tmp9CD;}),({struct Cyc_List_List*_tmp9CC=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4E9->f5=_tmp9CC;}),({struct Cyc_Absyn_Exp*_tmp9CB=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4E9->f6=_tmp9CB;}),({struct Cyc_Absyn_Exp*_tmp9CA=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4E9->f7=_tmp9CA;});_tmp4E9;});
# 1996
_tmp4EA->f1=_tmp9CE;});_tmp4EA;});_tmp4EB->hd=_tmp9CF;}),_tmp4EB->tl=one.tms;_tmp4EB;});_tmp752.tms=_tmp9D0;});_tmp752;}));
# 2001
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2002 "parse.y"
struct Cyc_Parse_Declarator _tmp4EC=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EC;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp753;_tmp753.id=one.id,_tmp753.varloc=one.varloc,({struct Cyc_List_List*_tmp9D4=({struct Cyc_List_List*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));({void*_tmp9D3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));_tmp4EE->tag=3U,({void*_tmp9D2=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));_tmp4ED->tag=0U,({struct Cyc_List_List*_tmp9D1=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4ED->f1=_tmp9D1;}),_tmp4ED->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4ED;});_tmp4EE->f1=_tmp9D2;});_tmp4EE;});_tmp4EF->hd=_tmp9D3;}),_tmp4EF->tl=one.tms;_tmp4EF;});_tmp753.tms=_tmp9D4;});_tmp753;}));
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2006
struct Cyc_List_List*_tmp4F0=({unsigned _tmp9D5=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9D5,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4F0;
struct Cyc_Parse_Declarator _tmp4F1=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp754;_tmp754.id=one.id,_tmp754.varloc=one.varloc,({struct Cyc_List_List*_tmp9D7=({struct Cyc_List_List*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));({void*_tmp9D6=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));_tmp4F2->tag=4U,_tmp4F2->f1=ts,_tmp4F2->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4F2->f3=0;_tmp4F2;});_tmp4F3->hd=_tmp9D6;}),_tmp4F3->tl=one.tms;_tmp4F3;});_tmp754.tms=_tmp9D7;});_tmp754;}));
# 2010
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2011 "parse.y"
struct Cyc_Parse_Declarator _tmp4F4=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F4;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp755;_tmp755.id=one.id,_tmp755.varloc=one.varloc,({struct Cyc_List_List*_tmp9DA=({struct Cyc_List_List*_tmp4F6=_region_malloc(yyr,sizeof(*_tmp4F6));({void*_tmp9D9=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));_tmp4F5->tag=5U,_tmp4F5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9D8=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4F5->f2=_tmp9D8;});_tmp4F5;});_tmp4F6->hd=_tmp9D9;}),_tmp4F6->tl=one.tms;_tmp4F6;});_tmp755.tms=_tmp9DA;});_tmp755;}));
# 2014
goto _LL0;}case 202U: _LL18F: _LL190:
# 2018 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 203U: _LL191: _LL192:
# 2019 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9DB=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9DB,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 204U: _LL193: _LL194: {
# 2023 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4F8=_region_malloc(yyr,sizeof(*_tmp4F8));({void*_tmp9DD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F7=_region_malloc(yyr,sizeof(*_tmp4F7));_tmp4F7->tag=5U,_tmp4F7->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9DC=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4F7->f2=_tmp9DC;});_tmp4F7;});_tmp4F8->hd=_tmp9DD;}),_tmp4F8->tl=ans;_tmp4F8;});{
# 2027
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4F9=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1A=_tmp4F9;struct _tuple22 _tmp4FA=_stmttmp1A;void*_tmp4FD;void*_tmp4FC;unsigned _tmp4FB;_LL475: _tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;_tmp4FD=_tmp4FA.f3;_LL476: {unsigned ploc=_tmp4FB;void*nullable=_tmp4FC;void*bound=_tmp4FD;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->ptr_loc=ploc,_tmp4FE->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp4FE->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp4FE;});{
struct _tuple15 _tmp4FF=({unsigned _tmp9E1=ploc;void*_tmp9E0=nullable;void*_tmp9DF=bound;void*_tmp9DE=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9E1,_tmp9E0,_tmp9DF,_tmp9DE,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1B=_tmp4FF;struct _tuple15 _tmp500=_stmttmp1B;void*_tmp505;void*_tmp504;void*_tmp503;void*_tmp502;void*_tmp501;_LL478: _tmp501=_tmp500.f1;_tmp502=_tmp500.f2;_tmp503=_tmp500.f3;_tmp504=_tmp500.f4;_tmp505=_tmp500.f5;_LL479: {void*nullable=_tmp501;void*bound=_tmp502;void*zeroterm=_tmp503;void*rgn_opt=_tmp504;void*released=_tmp505;
ans=({struct Cyc_List_List*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));({void*_tmp9E3=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=2U,(_tmp506->f1).rgn=rgn_opt,(_tmp506->f1).nullable=nullable,(_tmp506->f1).bounds=bound,(_tmp506->f1).zero_term=zeroterm,(_tmp506->f1).ptrloc=ptrloc,(_tmp506->f1).released=released,({struct Cyc_Absyn_Tqual _tmp9E2=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp506->f2=_tmp9E2;});_tmp506;});_tmp507->hd=_tmp9E3;}),_tmp507->tl=ans;_tmp507;});
yyval=Cyc_YY26(ans);
# 2035
goto _LL0;}}}}}case 205U: _LL195: _LL196:
# 2037
 yyval=Cyc_YY56(0);
goto _LL0;case 206U: _LL197: _LL198:
# 2038 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));({void*_tmp9E5=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp508->hd=_tmp9E5;}),({struct Cyc_List_List*_tmp9E4=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp508->tl=_tmp9E4;});_tmp508;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2043 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=0U,({struct Cyc_Absyn_Exp*_tmp9E6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp509->f1=_tmp9E6;});_tmp509;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2045 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));_tmp50A->tag=1U,({void*_tmp9E7=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp50A->f1=_tmp9E7;});_tmp50A;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2046 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=2U;_tmp50B;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2047 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp50C=_region_malloc(yyr,sizeof(*_tmp50C));_tmp50C->tag=3U;_tmp50C;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2048 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_tmp50D=_region_malloc(yyr,sizeof(*_tmp50D));_tmp50D->tag=6U;_tmp50D;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2049 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50E=_region_malloc(yyr,sizeof(*_tmp50E));_tmp50E->tag=4U;_tmp50E;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2050 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50F=_region_malloc(yyr,sizeof(*_tmp50F));_tmp50F->tag=5U;_tmp50F;}));
goto _LL0;case 214U: _LL1A7: _LL1A8:
# 2051 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp510=_region_malloc(yyr,sizeof(*_tmp510));_tmp510->tag=7U;_tmp510;}));
goto _LL0;case 215U: _LL1A9: _LL1AA:
# 2052 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp511=_region_malloc(yyr,sizeof(*_tmp511));_tmp511->tag=8U;_tmp511;}));
goto _LL0;case 216U: _LL1AB: _LL1AC:
# 2058 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp512->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp512->f3=Cyc_Absyn_fat_bound_type:({void*_tmp9E8=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp512->f3=_tmp9E8;});_tmp512;}));
# 2060
goto _LL0;case 217U: _LL1AD: _LL1AE:
# 2060 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp513->f2=Cyc_Absyn_false_type,({void*_tmp9E9=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp513->f3=_tmp9E9;});_tmp513;}));
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2061 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp514->f2=Cyc_Absyn_true_type,_tmp514->f3=Cyc_Absyn_fat_bound_type;_tmp514;}));
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2064
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2065 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2068
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2069 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2070 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2075 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2076 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2077 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2081 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2082 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9EA=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9EA,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 229U: _LL1C5: _LL1C6:
# 2087 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp515=_cycalloc(sizeof(*_tmp515));({struct Cyc_List_List*_tmp9ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp515->f1=_tmp9ED;}),_tmp515->f2=0,_tmp515->f3=0,({void*_tmp9EC=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp515->f4=_tmp9EC;}),({struct Cyc_List_List*_tmp9EB=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp515->f5=_tmp9EB;});_tmp515;}));
goto _LL0;case 230U: _LL1C7: _LL1C8:
# 2089 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp516=_cycalloc(sizeof(*_tmp516));({struct Cyc_List_List*_tmp9F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp516->f1=_tmp9F0;}),_tmp516->f2=1,_tmp516->f3=0,({void*_tmp9EF=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp516->f4=_tmp9EF;}),({struct Cyc_List_List*_tmp9EE=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp516->f5=_tmp9EE;});_tmp516;}));
goto _LL0;case 231U: _LL1C9: _LL1CA: {
# 2092
struct _tuple8*_tmp517=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1C=_tmp517;struct _tuple8*_tmp518=_stmttmp1C;void*_tmp51B;struct Cyc_Absyn_Tqual _tmp51A;struct _fat_ptr*_tmp519;_LL47B: _tmp519=_tmp518->f1;_tmp51A=_tmp518->f2;_tmp51B=_tmp518->f3;_LL47C: {struct _fat_ptr*n=_tmp519;struct Cyc_Absyn_Tqual tq=_tmp51A;void*t=_tmp51B;
struct Cyc_Absyn_VarargInfo*_tmp51C=({struct Cyc_Absyn_VarargInfo*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->name=n,_tmp51E->tq=tq,_tmp51E->type=t,({int _tmp9F1=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp51E->inject=_tmp9F1;});_tmp51E;});struct Cyc_Absyn_VarargInfo*v=_tmp51C;
yyval=Cyc_YY39(({struct _tuple27*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->f1=0,_tmp51D->f2=0,_tmp51D->f3=v,({void*_tmp9F3=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp51D->f4=_tmp9F3;}),({struct Cyc_List_List*_tmp9F2=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp51D->f5=_tmp9F2;});_tmp51D;}));
# 2096
goto _LL0;}}case 232U: _LL1CB: _LL1CC: {
# 2098
struct _tuple8*_tmp51F=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1D=_tmp51F;struct _tuple8*_tmp520=_stmttmp1D;void*_tmp523;struct Cyc_Absyn_Tqual _tmp522;struct _fat_ptr*_tmp521;_LL47E: _tmp521=_tmp520->f1;_tmp522=_tmp520->f2;_tmp523=_tmp520->f3;_LL47F: {struct _fat_ptr*n=_tmp521;struct Cyc_Absyn_Tqual tq=_tmp522;void*t=_tmp523;
struct Cyc_Absyn_VarargInfo*_tmp524=({struct Cyc_Absyn_VarargInfo*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->name=n,_tmp526->tq=tq,_tmp526->type=t,({int _tmp9F4=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp526->inject=_tmp9F4;});_tmp526;});struct Cyc_Absyn_VarargInfo*v=_tmp524;
yyval=Cyc_YY39(({struct _tuple27*_tmp525=_cycalloc(sizeof(*_tmp525));({struct Cyc_List_List*_tmp9F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp525->f1=_tmp9F7;}),_tmp525->f2=0,_tmp525->f3=v,({void*_tmp9F6=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp525->f4=_tmp9F6;}),({struct Cyc_List_List*_tmp9F5=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp525->f5=_tmp9F5;});_tmp525;}));
# 2102
goto _LL0;}}case 233U: _LL1CD: _LL1CE:
# 2106 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9F8=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9F8,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->tag=1U,_tmp527->f1=0;_tmp527;}));}));
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2107 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9F9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9F9,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2110
 yyval=Cyc_YY49(0);
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2111 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 237U: _LL1D5: _LL1D6:
# 2115 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 238U: _LL1D7: _LL1D8:
# 2116 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 239U: _LL1D9: _LL1DA: {
# 2124 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp528=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->tag=2U,_tmp52C->f1=0,_tmp52C->f2=& Cyc_Kinds_trk;_tmp52C;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp528;
void*_tmp529=({struct _fat_ptr _tmp9FA=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9FA,(void*)kb);});void*t=_tmp529;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp52B=_cycalloc(sizeof(*_tmp52B));({struct _tuple34*_tmp9FC=({struct _tuple34*_tmp52A=_cycalloc(sizeof(*_tmp52A));({void*_tmp9FB=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp52A->f1=_tmp9FB;}),_tmp52A->f2=t;_tmp52A;});_tmp52B->hd=_tmp9FC;}),_tmp52B->tl=0;_tmp52B;}));
# 2128
goto _LL0;}case 240U: _LL1DB: _LL1DC: {
# 2130 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->tag=2U,_tmp531->f1=0,_tmp531->f2=& Cyc_Kinds_trk;_tmp531;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp52D;
void*_tmp52E=({struct _fat_ptr _tmp9FD=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9FD,(void*)kb);});void*t=_tmp52E;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));({struct _tuple34*_tmpA00=({struct _tuple34*_tmp52F=_cycalloc(sizeof(*_tmp52F));({void*_tmp9FF=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp52F->f1=_tmp9FF;}),_tmp52F->f2=t;_tmp52F;});_tmp530->hd=_tmpA00;}),({struct Cyc_List_List*_tmp9FE=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp530->tl=_tmp9FE;});_tmp530;}));
# 2134
goto _LL0;}case 241U: _LL1DD: _LL1DE:
# 2138 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2140 "parse.y"
 if(({struct _fat_ptr _tmpA01=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmpA01,({const char*_tmp532="inject";_tag_fat(_tmp532,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp534=({struct Cyc_Warn_String_Warn_Warg_struct _tmp756;_tmp756.tag=0U,({struct _fat_ptr _tmpA02=({const char*_tmp535="missing type in function declaration";_tag_fat(_tmp535,sizeof(char),37U);});_tmp756.f1=_tmpA02;});_tmp756;});void*_tmp533[1U];_tmp533[0]=& _tmp534;({unsigned _tmpA03=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA03,_tag_fat(_tmp533,sizeof(void*),1U));});});
yyval=Cyc_YY31(1);
# 2144
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2147 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2148 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmpA04=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA04,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2152 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2153 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2155 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({void*_tmpA05=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp536->hd=_tmpA05;}),_tmp536->tl=0;_tmp536;}));
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2157 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));({void*_tmpA06=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp537->hd=_tmpA06;}),_tmp537->tl=0;_tmp537;}));
# 2160
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2165 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp538=_cycalloc(sizeof(*_tmp538));({void*_tmpA08=Cyc_Absyn_access_eff(({struct _tuple8*_tmpA07=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA07,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp538->hd=_tmpA08;}),_tmp538->tl=0;_tmp538;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2167 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp539=_cycalloc(sizeof(*_tmp539));({void*_tmpA0B=Cyc_Absyn_access_eff(({struct _tuple8*_tmpA0A=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA0A,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp539->hd=_tmpA0B;}),({struct Cyc_List_List*_tmpA09=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp539->tl=_tmpA09;});_tmp539;}));
goto _LL0;case 251U: _LL1F1: _LL1F2:
# 2173 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*_tmp53A));({struct _tuple8*_tmpA0C=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp53A->hd=_tmpA0C;}),_tmp53A->tl=0;_tmp53A;}));
goto _LL0;case 252U: _LL1F3: _LL1F4:
# 2175 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*_tmp53B));({struct _tuple8*_tmpA0E=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp53B->hd=_tmpA0E;}),({struct Cyc_List_List*_tmpA0D=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp53B->tl=_tmpA0D;});_tmp53B;}));
goto _LL0;case 253U: _LL1F5: _LL1F6: {
# 2181 "parse.y"
struct _tuple26 _tmp53C=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp1E=_tmp53C;struct _tuple26 _tmp53D=_stmttmp1E;struct Cyc_List_List*_tmp540;struct Cyc_Parse_Type_specifier _tmp53F;struct Cyc_Absyn_Tqual _tmp53E;_LL481: _tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;_tmp540=_tmp53D.f3;_LL482: {struct Cyc_Absyn_Tqual tq=_tmp53E;struct Cyc_Parse_Type_specifier tspecs=_tmp53F;struct Cyc_List_List*atts=_tmp540;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp541=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1F=_tmp541;struct Cyc_Parse_Declarator _tmp542=_stmttmp1F;struct Cyc_List_List*_tmp545;unsigned _tmp544;struct _tuple0*_tmp543;_LL484: _tmp543=_tmp542.id;_tmp544=_tmp542.varloc;_tmp545=_tmp542.tms;_LL485: {struct _tuple0*qv=_tmp543;unsigned varloc=_tmp544;struct Cyc_List_List*tms=_tmp545;
void*_tmp546=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp546;
struct _tuple14 _tmp547=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp20=_tmp547;struct _tuple14 _tmp548=_stmttmp20;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54B;void*_tmp54A;struct Cyc_Absyn_Tqual _tmp549;_LL487: _tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;_tmp54B=_tmp548.f3;_tmp54C=_tmp548.f4;_LL488: {struct Cyc_Absyn_Tqual tq2=_tmp549;void*t2=_tmp54A;struct Cyc_List_List*tvs=_tmp54B;struct Cyc_List_List*atts2=_tmp54C;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp54E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp757;_tmp757.tag=0U,({struct _fat_ptr _tmpA0F=({const char*_tmp54F="parameter with bad type params";_tag_fat(_tmp54F,sizeof(char),31U);});_tmp757.f1=_tmpA0F;});_tmp757;});void*_tmp54D[1U];_tmp54D[0]=& _tmp54E;({unsigned _tmpA10=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmpA10,_tag_fat(_tmp54D,sizeof(void*),1U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp551=({struct Cyc_Warn_String_Warn_Warg_struct _tmp758;_tmp758.tag=0U,({struct _fat_ptr _tmpA11=({const char*_tmp552="parameter cannot be qualified with a namespace";_tag_fat(_tmp552,sizeof(char),47U);});_tmp758.f1=_tmpA11;});_tmp758;});void*_tmp550[1U];_tmp550[0]=& _tmp551;({unsigned _tmpA12=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA12,_tag_fat(_tmp550,sizeof(void*),1U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp554=({struct Cyc_Warn_String_Warn_Warg_struct _tmp759;_tmp759.tag=0U,({struct _fat_ptr _tmpA13=({const char*_tmp555="extra attributes on parameter, ignoring";_tag_fat(_tmp555,sizeof(char),40U);});_tmp759.f1=_tmpA13;});_tmp759;});void*_tmp553[1U];_tmp553[0]=& _tmp554;({unsigned _tmpA14=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA14,_tag_fat(_tmp553,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->f1=idopt,_tmp556->f2=tq2,_tmp556->f3=t2;_tmp556;}));
# 2195
goto _LL0;}}}}}}case 254U: _LL1F7: _LL1F8: {
# 2196 "parse.y"
struct _tuple26 _tmp557=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp557;struct _tuple26 _tmp558=_stmttmp21;struct Cyc_List_List*_tmp55B;struct Cyc_Parse_Type_specifier _tmp55A;struct Cyc_Absyn_Tqual _tmp559;_LL48A: _tmp559=_tmp558.f1;_tmp55A=_tmp558.f2;_tmp55B=_tmp558.f3;_LL48B: {struct Cyc_Absyn_Tqual tq=_tmp559;struct Cyc_Parse_Type_specifier tspecs=_tmp55A;struct Cyc_List_List*atts=_tmp55B;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp55C=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp55C;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp55E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp75A;_tmp75A.tag=0U,({struct _fat_ptr _tmpA15=({const char*_tmp55F="bad attributes on parameter, ignoring";_tag_fat(_tmp55F,sizeof(char),38U);});_tmp75A.f1=_tmpA15;});_tmp75A;});void*_tmp55D[1U];_tmp55D[0]=& _tmp55E;({unsigned _tmpA16=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA16,_tag_fat(_tmp55D,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->f1=0,_tmp560->f2=tq,_tmp560->f3=t;_tmp560;}));
# 2203
goto _LL0;}}}case 255U: _LL1F9: _LL1FA: {
# 2204 "parse.y"
struct _tuple26 _tmp561=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp22=_tmp561;struct _tuple26 _tmp562=_stmttmp22;struct Cyc_List_List*_tmp565;struct Cyc_Parse_Type_specifier _tmp564;struct Cyc_Absyn_Tqual _tmp563;_LL48D: _tmp563=_tmp562.f1;_tmp564=_tmp562.f2;_tmp565=_tmp562.f3;_LL48E: {struct Cyc_Absyn_Tqual tq=_tmp563;struct Cyc_Parse_Type_specifier tspecs=_tmp564;struct Cyc_List_List*atts=_tmp565;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp566=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp566;
struct Cyc_List_List*_tmp567=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp567;
struct _tuple14 _tmp568=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp568;struct _tuple14 _tmp569=_stmttmp23;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*_tmp56C;void*_tmp56B;struct Cyc_Absyn_Tqual _tmp56A;_LL490: _tmp56A=_tmp569.f1;_tmp56B=_tmp569.f2;_tmp56C=_tmp569.f3;_tmp56D=_tmp569.f4;_LL491: {struct Cyc_Absyn_Tqual tq2=_tmp56A;void*t2=_tmp56B;struct Cyc_List_List*tvs=_tmp56C;struct Cyc_List_List*atts2=_tmp56D;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp75B;_tmp75B.tag=0U,({
struct _fat_ptr _tmpA17=({const char*_tmp570="bad type parameters on formal argument, ignoring";_tag_fat(_tmp570,sizeof(char),49U);});_tmp75B.f1=_tmpA17;});_tmp75B;});void*_tmp56E[1U];_tmp56E[0]=& _tmp56F;({unsigned _tmpA18=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA18,_tag_fat(_tmp56E,sizeof(void*),1U));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp572=({struct Cyc_Warn_String_Warn_Warg_struct _tmp75C;_tmp75C.tag=0U,({struct _fat_ptr _tmpA19=({const char*_tmp573="bad attributes on parameter, ignoring";_tag_fat(_tmp573,sizeof(char),38U);});_tmp75C.f1=_tmpA19;});_tmp75C;});void*_tmp571[1U];_tmp571[0]=& _tmp572;({unsigned _tmpA1A=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA1A,_tag_fat(_tmp571,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->f1=0,_tmp574->f2=tq2,_tmp574->f3=t2;_tmp574;}));
# 2216
goto _LL0;}}}}case 256U: _LL1FB: _LL1FC:
# 2220 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2224 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp576=_cycalloc(sizeof(*_tmp576));({struct _fat_ptr*_tmpA1C=({struct _fat_ptr*_tmp575=_cycalloc(sizeof(*_tmp575));({struct _fat_ptr _tmpA1B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp575=_tmpA1B;});_tmp575;});_tmp576->hd=_tmpA1C;}),_tmp576->tl=0;_tmp576;}));
goto _LL0;case 258U: _LL1FF: _LL200:
# 2226 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp578=_cycalloc(sizeof(*_tmp578));({struct _fat_ptr*_tmpA1F=({struct _fat_ptr*_tmp577=_cycalloc(sizeof(*_tmp577));({struct _fat_ptr _tmpA1E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp577=_tmpA1E;});_tmp577;});_tmp578->hd=_tmpA1F;}),({struct Cyc_List_List*_tmpA1D=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp578->tl=_tmpA1D;});_tmp578;}));
goto _LL0;case 259U: _LL201: _LL202:
# 2230 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 260U: _LL203: _LL204:
# 2231 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 261U: _LL205: _LL206:
# 2236 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA20=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=36U,_tmp579->f1=0,_tmp579->f2=0;_tmp579;});Cyc_Absyn_new_exp(_tmpA20,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208:
# 2238 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA22=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->tag=36U,_tmp57A->f1=0,({struct Cyc_List_List*_tmpA21=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp57A->f2=_tmpA21;});_tmp57A;});Cyc_Absyn_new_exp(_tmpA22,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 263U: _LL209: _LL20A:
# 2240 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA24=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=36U,_tmp57B->f1=0,({struct Cyc_List_List*_tmpA23=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp57B->f2=_tmpA23;});_tmp57B;});Cyc_Absyn_new_exp(_tmpA24,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 264U: _LL20B: _LL20C: {
# 2242 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp57C=({unsigned _tmpA29=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA28=({struct _tuple0*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA26=({struct _fat_ptr*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct _fat_ptr _tmpA25=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp57E=_tmpA25;});_tmp57E;});_tmp57F->f2=_tmpA26;});_tmp57F;});void*_tmpA27=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA29,_tmpA28,_tmpA27,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2242
struct Cyc_Absyn_Vardecl*vd=_tmp57C;
# 2245
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA2C=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=27U,_tmp57D->f1=vd,({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp57D->f2=_tmpA2B;}),({struct Cyc_Absyn_Exp*_tmpA2A=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp57D->f3=_tmpA2A;}),_tmp57D->f4=0;_tmp57D;});Cyc_Absyn_new_exp(_tmpA2C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2248
goto _LL0;}case 265U: _LL20D: _LL20E: {
# 2250 "parse.y"
void*_tmp580=({struct _tuple8*_tmpA2D=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA2D,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp580;
yyval=Cyc_Exp_tok(({void*_tmpA2F=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=28U,({struct Cyc_Absyn_Exp*_tmpA2E=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp581->f1=_tmpA2E;}),_tmp581->f2=t,_tmp581->f3=0;_tmp581;});Cyc_Absyn_new_exp(_tmpA2F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2253
goto _LL0;}case 266U: _LL20F: _LL210:
# 2258 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));({struct _tuple35*_tmpA31=({struct _tuple35*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->f1=0,({struct Cyc_Absyn_Exp*_tmpA30=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp582->f2=_tmpA30;});_tmp582;});_tmp583->hd=_tmpA31;}),_tmp583->tl=0;_tmp583;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2260 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));({struct _tuple35*_tmpA34=({struct _tuple35*_tmp584=_cycalloc(sizeof(*_tmp584));({struct Cyc_List_List*_tmpA33=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp584->f1=_tmpA33;}),({struct Cyc_Absyn_Exp*_tmpA32=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp584->f2=_tmpA32;});_tmp584;});_tmp585->hd=_tmpA34;}),_tmp585->tl=0;_tmp585;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2262 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({struct _tuple35*_tmpA37=({struct _tuple35*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->f1=0,({struct Cyc_Absyn_Exp*_tmpA36=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp586->f2=_tmpA36;});_tmp586;});_tmp587->hd=_tmpA37;}),({struct Cyc_List_List*_tmpA35=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp587->tl=_tmpA35;});_tmp587;}));
goto _LL0;case 269U: _LL215: _LL216:
# 2264 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));({struct _tuple35*_tmpA3B=({struct _tuple35*_tmp588=_cycalloc(sizeof(*_tmp588));({struct Cyc_List_List*_tmpA3A=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp588->f1=_tmpA3A;}),({struct Cyc_Absyn_Exp*_tmpA39=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp588->f2=_tmpA39;});_tmp588;});_tmp589->hd=_tmpA3B;}),({struct Cyc_List_List*_tmpA38=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp589->tl=_tmpA38;});_tmp589;}));
goto _LL0;case 270U: _LL217: _LL218:
# 2268 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 271U: _LL219: _LL21A:
# 2269 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp58C=_cycalloc(sizeof(*_tmp58C));({void*_tmpA3E=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->tag=1U,({struct _fat_ptr*_tmpA3D=({struct _fat_ptr*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct _fat_ptr _tmpA3C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp58A=_tmpA3C;});_tmp58A;});_tmp58B->f1=_tmpA3D;});_tmp58B;});_tmp58C->hd=_tmpA3E;}),_tmp58C->tl=0;_tmp58C;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2274 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp58D=_cycalloc(sizeof(*_tmp58D));({void*_tmpA3F=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp58D->hd=_tmpA3F;}),_tmp58D->tl=0;_tmp58D;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2275 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp58E=_cycalloc(sizeof(*_tmp58E));({void*_tmpA41=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp58E->hd=_tmpA41;}),({struct Cyc_List_List*_tmpA40=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp58E->tl=_tmpA40;});_tmp58E;}));
goto _LL0;case 274U: _LL21F: _LL220:
# 2279 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->tag=0U,({struct Cyc_Absyn_Exp*_tmpA42=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp58F->f1=_tmpA42;});_tmp58F;}));
goto _LL0;case 275U: _LL221: _LL222:
# 2280 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->tag=1U,({struct _fat_ptr*_tmpA44=({struct _fat_ptr*_tmp590=_cycalloc(sizeof(*_tmp590));({struct _fat_ptr _tmpA43=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp590=_tmpA43;});_tmp590;});_tmp591->f1=_tmpA44;});_tmp591;}));
goto _LL0;case 276U: _LL223: _LL224: {
# 2285 "parse.y"
struct _tuple26 _tmp592=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp592;struct _tuple26 _tmp593=_stmttmp24;struct Cyc_List_List*_tmp596;struct Cyc_Parse_Type_specifier _tmp595;struct Cyc_Absyn_Tqual _tmp594;_LL493: _tmp594=_tmp593.f1;_tmp595=_tmp593.f2;_tmp596=_tmp593.f3;_LL494: {struct Cyc_Absyn_Tqual tq=_tmp594;struct Cyc_Parse_Type_specifier tss=_tmp595;struct Cyc_List_List*atts=_tmp596;
void*_tmp597=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp597;
if(atts != 0)
({void*_tmp598=0U;({unsigned _tmpA46=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA45=({const char*_tmp599="ignoring attributes in type";_tag_fat(_tmp599,sizeof(char),28U);});Cyc_Warn_warn(_tmpA46,_tmpA45,_tag_fat(_tmp598,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->f1=0,_tmp59A->f2=tq,_tmp59A->f3=t;_tmp59A;}));
# 2291
goto _LL0;}}case 277U: _LL225: _LL226: {
# 2292 "parse.y"
struct _tuple26 _tmp59B=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp59B;struct _tuple26 _tmp59C=_stmttmp25;struct Cyc_List_List*_tmp59F;struct Cyc_Parse_Type_specifier _tmp59E;struct Cyc_Absyn_Tqual _tmp59D;_LL496: _tmp59D=_tmp59C.f1;_tmp59E=_tmp59C.f2;_tmp59F=_tmp59C.f3;_LL497: {struct Cyc_Absyn_Tqual tq=_tmp59D;struct Cyc_Parse_Type_specifier tss=_tmp59E;struct Cyc_List_List*atts=_tmp59F;
void*_tmp5A0=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp5A0;
struct Cyc_List_List*_tmp5A1=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp5A1;
struct _tuple14 _tmp5A2=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp5A2;
if(t_info.f3 != 0)
# 2298
({void*_tmp5A3=0U;({unsigned _tmpA48=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA47=({const char*_tmp5A4="bad type params, ignoring";_tag_fat(_tmp5A4,sizeof(char),26U);});Cyc_Warn_warn(_tmpA48,_tmpA47,_tag_fat(_tmp5A3,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp5A5=0U;({unsigned _tmpA4A=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA49=({const char*_tmp5A6="bad specifiers, ignoring";_tag_fat(_tmp5A6,sizeof(char),25U);});Cyc_Warn_warn(_tmpA4A,_tmpA49,_tag_fat(_tmp5A5,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->f1=0,_tmp5A7->f2=t_info.f1,_tmp5A7->f3=t_info.f2;_tmp5A7;}));
# 2303
goto _LL0;}}case 278U: _LL227: _LL228:
# 2306 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA4B=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA4B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 279U: _LL229: _LL22A:
# 2307 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2308 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2309 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 282U: _LL22F: _LL230:
# 2310 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));({void*_tmpA4D=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A8->hd=_tmpA4D;}),({struct Cyc_List_List*_tmpA4C=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp5A8->tl=_tmpA4C;});_tmp5A8;})));
goto _LL0;case 283U: _LL231: _LL232:
# 2316 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));({void*_tmpA4E=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A9->hd=_tmpA4E;}),_tmp5A9->tl=0;_tmp5A9;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2317 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));({void*_tmpA50=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp5AA->hd=_tmpA50;}),({struct Cyc_List_List*_tmpA4F=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp5AA->tl=_tmpA4F;});_tmp5AA;}));
goto _LL0;case 285U: _LL235: _LL236:
# 2322 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpA51=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp75D.tms=_tmpA51;});_tmp75D;}));
goto _LL0;case 286U: _LL237: _LL238:
# 2324 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 287U: _LL239: _LL23A:
# 2326 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75E;({struct Cyc_List_List*_tmpA53=({struct Cyc_List_List*_tmpA52=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA52,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp75E.tms=_tmpA53;});_tmp75E;}));
goto _LL0;case 288U: _LL23B: _LL23C:
# 2331 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 289U: _LL23D: _LL23E:
# 2333 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75F;({struct Cyc_List_List*_tmpA56=({struct Cyc_List_List*_tmp5AC=_region_malloc(yyr,sizeof(*_tmp5AC));({void*_tmpA55=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5AB=_region_malloc(yyr,sizeof(*_tmp5AB));_tmp5AB->tag=0U,({void*_tmpA54=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5AB->f1=_tmpA54;}),_tmp5AB->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5AB;});_tmp5AC->hd=_tmpA55;}),_tmp5AC->tl=0;_tmp5AC;});_tmp75F.tms=_tmpA56;});_tmp75F;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2335 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp760;({struct Cyc_List_List*_tmpA5A=({struct Cyc_List_List*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));({void*_tmpA59=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5AD=_region_malloc(yyr,sizeof(*_tmp5AD));_tmp5AD->tag=0U,({void*_tmpA58=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5AD->f1=_tmpA58;}),_tmp5AD->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5AD;});_tmp5AE->hd=_tmpA59;}),({struct Cyc_List_List*_tmpA57=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5AE->tl=_tmpA57;});_tmp5AE;});_tmp760.tms=_tmpA5A;});_tmp760;}));
goto _LL0;case 291U: _LL241: _LL242:
# 2337 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp761;({struct Cyc_List_List*_tmpA5E=({struct Cyc_List_List*_tmp5B0=_region_malloc(yyr,sizeof(*_tmp5B0));({void*_tmpA5D=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));_tmp5AF->tag=1U,({struct Cyc_Absyn_Exp*_tmpA5C=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5AF->f1=_tmpA5C;}),({void*_tmpA5B=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5AF->f2=_tmpA5B;}),_tmp5AF->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5AF;});_tmp5B0->hd=_tmpA5D;}),_tmp5B0->tl=0;_tmp5B0;});_tmp761.tms=_tmpA5E;});_tmp761;}));
goto _LL0;case 292U: _LL243: _LL244:
# 2339 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp762;({struct Cyc_List_List*_tmpA63=({struct Cyc_List_List*_tmp5B2=_region_malloc(yyr,sizeof(*_tmp5B2));({void*_tmpA62=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5B1=_region_malloc(yyr,sizeof(*_tmp5B1));_tmp5B1->tag=1U,({struct Cyc_Absyn_Exp*_tmpA61=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5B1->f1=_tmpA61;}),({void*_tmpA60=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5B1->f2=_tmpA60;}),_tmp5B1->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5B1;});_tmp5B2->hd=_tmpA62;}),({
struct Cyc_List_List*_tmpA5F=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5B2->tl=_tmpA5F;});_tmp5B2;});
# 2339
_tmp762.tms=_tmpA63;});_tmp762;}));
# 2342
goto _LL0;case 293U: _LL245: _LL246:
# 2343 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp763;({struct Cyc_List_List*_tmpA6A=({struct Cyc_List_List*_tmp5B5=_region_malloc(yyr,sizeof(*_tmp5B5));({void*_tmpA69=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B4=_region_malloc(yyr,sizeof(*_tmp5B4));_tmp5B4->tag=3U,({void*_tmpA68=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B3=_region_malloc(yyr,sizeof(*_tmp5B3));_tmp5B3->tag=1U,_tmp5B3->f1=0,_tmp5B3->f2=0,_tmp5B3->f3=0,({void*_tmpA67=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5B3->f4=_tmpA67;}),({struct Cyc_List_List*_tmpA66=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5B3->f5=_tmpA66;}),({struct Cyc_Absyn_Exp*_tmpA65=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B3->f6=_tmpA65;}),({struct Cyc_Absyn_Exp*_tmpA64=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5B3->f7=_tmpA64;});_tmp5B3;});_tmp5B4->f1=_tmpA68;});_tmp5B4;});_tmp5B5->hd=_tmpA69;}),_tmp5B5->tl=0;_tmp5B5;});_tmp763.tms=_tmpA6A;});_tmp763;}));
# 2345
goto _LL0;case 294U: _LL247: _LL248: {
# 2346 "parse.y"
struct _tuple27*_tmp5B6=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp26=_tmp5B6;struct _tuple27*_tmp5B7=_stmttmp26;struct Cyc_List_List*_tmp5BC;void*_tmp5BB;struct Cyc_Absyn_VarargInfo*_tmp5BA;int _tmp5B9;struct Cyc_List_List*_tmp5B8;_LL499: _tmp5B8=_tmp5B7->f1;_tmp5B9=_tmp5B7->f2;_tmp5BA=_tmp5B7->f3;_tmp5BB=_tmp5B7->f4;_tmp5BC=_tmp5B7->f5;_LL49A: {struct Cyc_List_List*lis=_tmp5B8;int b=_tmp5B9;struct Cyc_Absyn_VarargInfo*c=_tmp5BA;void*eff=_tmp5BB;struct Cyc_List_List*po=_tmp5BC;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp764;({struct Cyc_List_List*_tmpA6F=({struct Cyc_List_List*_tmp5BF=_region_malloc(yyr,sizeof(*_tmp5BF));({void*_tmpA6E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BE=_region_malloc(yyr,sizeof(*_tmp5BE));_tmp5BE->tag=3U,({void*_tmpA6D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5BD=_region_malloc(yyr,sizeof(*_tmp5BD));_tmp5BD->tag=1U,_tmp5BD->f1=lis,_tmp5BD->f2=b,_tmp5BD->f3=c,_tmp5BD->f4=eff,_tmp5BD->f5=po,({struct Cyc_Absyn_Exp*_tmpA6C=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5BD->f6=_tmpA6C;}),({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5BD->f7=_tmpA6B;});_tmp5BD;});_tmp5BE->f1=_tmpA6D;});_tmp5BE;});_tmp5BF->hd=_tmpA6E;}),_tmp5BF->tl=0;_tmp5BF;});_tmp764.tms=_tmpA6F;});_tmp764;}));
# 2349
goto _LL0;}}case 295U: _LL249: _LL24A:
# 2350 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp765;({struct Cyc_List_List*_tmpA77=({struct Cyc_List_List*_tmp5C2=_region_malloc(yyr,sizeof(*_tmp5C2));({void*_tmpA76=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C1=_region_malloc(yyr,sizeof(*_tmp5C1));_tmp5C1->tag=3U,({void*_tmpA75=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C0=_region_malloc(yyr,sizeof(*_tmp5C0));_tmp5C0->tag=1U,_tmp5C0->f1=0,_tmp5C0->f2=0,_tmp5C0->f3=0,({void*_tmpA74=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5C0->f4=_tmpA74;}),({struct Cyc_List_List*_tmpA73=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5C0->f5=_tmpA73;}),({struct Cyc_Absyn_Exp*_tmpA72=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5C0->f6=_tmpA72;}),({struct Cyc_Absyn_Exp*_tmpA71=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5C0->f7=_tmpA71;});_tmp5C0;});_tmp5C1->f1=_tmpA75;});_tmp5C1;});_tmp5C2->hd=_tmpA76;}),({
struct Cyc_List_List*_tmpA70=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C2->tl=_tmpA70;});_tmp5C2;});
# 2350
_tmp765.tms=_tmpA77;});_tmp765;}));
# 2353
goto _LL0;case 296U: _LL24B: _LL24C: {
# 2354 "parse.y"
struct _tuple27*_tmp5C3=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp27=_tmp5C3;struct _tuple27*_tmp5C4=_stmttmp27;struct Cyc_List_List*_tmp5C9;void*_tmp5C8;struct Cyc_Absyn_VarargInfo*_tmp5C7;int _tmp5C6;struct Cyc_List_List*_tmp5C5;_LL49C: _tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;_tmp5C7=_tmp5C4->f3;_tmp5C8=_tmp5C4->f4;_tmp5C9=_tmp5C4->f5;_LL49D: {struct Cyc_List_List*lis=_tmp5C5;int b=_tmp5C6;struct Cyc_Absyn_VarargInfo*c=_tmp5C7;void*eff=_tmp5C8;struct Cyc_List_List*po=_tmp5C9;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp766;({struct Cyc_List_List*_tmpA7D=({struct Cyc_List_List*_tmp5CC=_region_malloc(yyr,sizeof(*_tmp5CC));({void*_tmpA7C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CB=_region_malloc(yyr,sizeof(*_tmp5CB));_tmp5CB->tag=3U,({void*_tmpA7B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));_tmp5CA->tag=1U,_tmp5CA->f1=lis,_tmp5CA->f2=b,_tmp5CA->f3=c,_tmp5CA->f4=eff,_tmp5CA->f5=po,({
struct Cyc_Absyn_Exp*_tmpA7A=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5CA->f6=_tmpA7A;}),({struct Cyc_Absyn_Exp*_tmpA79=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5CA->f7=_tmpA79;});_tmp5CA;});
# 2355
_tmp5CB->f1=_tmpA7B;});_tmp5CB;});_tmp5CC->hd=_tmpA7C;}),({
struct Cyc_List_List*_tmpA78=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CC->tl=_tmpA78;});_tmp5CC;});
# 2355
_tmp766.tms=_tmpA7D;});_tmp766;}));
# 2358
goto _LL0;}}case 297U: _LL24D: _LL24E: {
# 2360
struct Cyc_List_List*_tmp5CD=({unsigned _tmpA7E=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA7E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5CD;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp767;({struct Cyc_List_List*_tmpA81=({struct Cyc_List_List*_tmp5CF=_region_malloc(yyr,sizeof(*_tmp5CF));({void*_tmpA80=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5CE=_region_malloc(yyr,sizeof(*_tmp5CE));_tmp5CE->tag=4U,_tmp5CE->f1=ts,_tmp5CE->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5CE->f3=0;_tmp5CE;});_tmp5CF->hd=_tmpA80;}),({
struct Cyc_List_List*_tmpA7F=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CF->tl=_tmpA7F;});_tmp5CF;});
# 2361
_tmp767.tms=_tmpA81;});_tmp767;}));
# 2364
goto _LL0;}case 298U: _LL24F: _LL250:
# 2365 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp768;({struct Cyc_List_List*_tmpA85=({struct Cyc_List_List*_tmp5D1=_region_malloc(yyr,sizeof(*_tmp5D1));({void*_tmpA84=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5D0=_region_malloc(yyr,sizeof(*_tmp5D0));_tmp5D0->tag=5U,_tmp5D0->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA83=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5D0->f2=_tmpA83;});_tmp5D0;});_tmp5D1->hd=_tmpA84;}),({struct Cyc_List_List*_tmpA82=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5D1->tl=_tmpA82;});_tmp5D1;});_tmp768.tms=_tmpA85;});_tmp768;}));
# 2367
goto _LL0;case 299U: _LL251: _LL252:
# 2371 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2372 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2373 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2374 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2375 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 304U: _LL25B: _LL25C:
# 2376 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 305U: _LL25D: _LL25E:
# 2382 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA89=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->tag=13U,({struct _fat_ptr*_tmpA88=({struct _fat_ptr*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));({struct _fat_ptr _tmpA87=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D2=_tmpA87;});_tmp5D2;});_tmp5D3->f1=_tmpA88;}),({struct Cyc_Absyn_Stmt*_tmpA86=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D3->f2=_tmpA86;});_tmp5D3;});Cyc_Absyn_new_stmt(_tmpA89,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 306U: _LL25F: _LL260:
# 2386 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 307U: _LL261: _LL262:
# 2387 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA8A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 308U: _LL263: _LL264:
# 2392 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 309U: _LL265: _LL266:
# 2393 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 310U: _LL267: _LL268:
# 2398 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA8B=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA8B,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 311U: _LL269: _LL26A:
# 2399 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA8C=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA8C,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 312U: _LL26B: _LL26C:
# 2400 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA91=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->tag=13U,({struct _fat_ptr*_tmpA90=({struct _fat_ptr*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));({struct _fat_ptr _tmpA8F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D4=_tmpA8F;});_tmp5D4;});_tmp5D5->f1=_tmpA90;}),({struct Cyc_Absyn_Stmt*_tmpA8E=({struct Cyc_List_List*_tmpA8D=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA8D,Cyc_Absyn_skip_stmt(0U));});_tmp5D5->f2=_tmpA8E;});_tmp5D5;});Cyc_Absyn_new_stmt(_tmpA91,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2402
goto _LL0;case 313U: _LL26D: _LL26E:
# 2402 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA96=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->tag=13U,({struct _fat_ptr*_tmpA95=({struct _fat_ptr*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct _fat_ptr _tmpA94=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D6=_tmpA94;});_tmp5D6;});_tmp5D7->f1=_tmpA95;}),({struct Cyc_Absyn_Stmt*_tmpA93=({struct Cyc_List_List*_tmpA92=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA92,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5D7->f2=_tmpA93;});_tmp5D7;});Cyc_Absyn_new_stmt(_tmpA96,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2404
goto _LL0;case 314U: _LL26F: _LL270:
# 2404 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 315U: _LL271: _LL272:
# 2405 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA98=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA97=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA98,_tmpA97,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 316U: _LL273: _LL274:
# 2406 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA9B=({void*_tmpA9A=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA99=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D8->f1=_tmpA99;});_tmp5D8;});Cyc_Absyn_new_decl(_tmpA9A,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA9B,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 317U: _LL275: _LL276:
# 2409 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA9E=({void*_tmpA9D=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA9C=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D9->f1=_tmpA9C;});_tmp5D9;});Cyc_Absyn_new_decl(_tmpA9D,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA9E,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 318U: _LL277: _LL278:
# 2414 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAA0=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA9F=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpAA1,_tmpAA0,_tmpA9F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A:
# 2416 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAA3=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAA2=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpAA4,_tmpAA3,_tmpAA2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320U: _LL27B: _LL27C:
# 2422 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpAA5=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpAA6,_tmpAA5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 321U: _LL27D: _LL27E: {
# 2425
struct Cyc_Absyn_Exp*_tmp5DA=({struct _tuple0*_tmpAA7=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpAA7,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5DA;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA9=e;struct Cyc_List_List*_tmpAA8=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpAA9,_tmpAA8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2429
struct Cyc_Absyn_Exp*_tmp5DB=({struct Cyc_List_List*_tmpAAA=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpAAA,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5DB;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAAC=e;struct Cyc_List_List*_tmpAAB=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpAAC,_tmpAAB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2432
goto _LL0;}case 323U: _LL281: _LL282:
# 2435 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAAE=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpAAD=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpAAE,_tmpAAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 324U: _LL283: _LL284:
# 2449 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 325U: _LL285: _LL286:
# 2452 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct Cyc_Absyn_Switch_clause*_tmpAB2=({struct Cyc_Absyn_Switch_clause*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct Cyc_Absyn_Pat*_tmpAB1=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5DC->pattern=_tmpAB1;}),_tmp5DC->pat_vars=0,_tmp5DC->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAB0=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5DC->body=_tmpAB0;}),_tmp5DC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DC;});
# 2452
_tmp5DD->hd=_tmpAB2;}),({
# 2454
struct Cyc_List_List*_tmpAAF=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5DD->tl=_tmpAAF;});_tmp5DD;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2456 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));({struct Cyc_Absyn_Switch_clause*_tmpAB6=({struct Cyc_Absyn_Switch_clause*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));({struct Cyc_Absyn_Pat*_tmpAB5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DE->pattern=_tmpAB5;}),_tmp5DE->pat_vars=0,_tmp5DE->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAB4=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5DE->body=_tmpAB4;}),_tmp5DE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DE;});
# 2456
_tmp5DF->hd=_tmpAB6;}),({
# 2458
struct Cyc_List_List*_tmpAB3=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5DF->tl=_tmpAB3;});_tmp5DF;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2460 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));({struct Cyc_Absyn_Switch_clause*_tmpABA=({struct Cyc_Absyn_Switch_clause*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));({struct Cyc_Absyn_Pat*_tmpAB9=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E0->pattern=_tmpAB9;}),_tmp5E0->pat_vars=0,_tmp5E0->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAB8=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5E0->body=_tmpAB8;}),_tmp5E0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E0;});_tmp5E1->hd=_tmpABA;}),({struct Cyc_List_List*_tmpAB7=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5E1->tl=_tmpAB7;});_tmp5E1;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2462 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));({struct Cyc_Absyn_Switch_clause*_tmpABF=({struct Cyc_Absyn_Switch_clause*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));({struct Cyc_Absyn_Pat*_tmpABE=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E2->pattern=_tmpABE;}),_tmp5E2->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpABD=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5E2->where_clause=_tmpABD;}),({
struct Cyc_Absyn_Stmt*_tmpABC=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5E2->body=_tmpABC;}),_tmp5E2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E2;});
# 2462
_tmp5E3->hd=_tmpABF;}),({
# 2464
struct Cyc_List_List*_tmpABB=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5E3->tl=_tmpABB;});_tmp5E3;}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2466 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));({struct Cyc_Absyn_Switch_clause*_tmpAC4=({struct Cyc_Absyn_Switch_clause*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));({struct Cyc_Absyn_Pat*_tmpAC3=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E4->pattern=_tmpAC3;}),_tmp5E4->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5E4->where_clause=_tmpAC2;}),({struct Cyc_Absyn_Stmt*_tmpAC1=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5E4->body=_tmpAC1;}),_tmp5E4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E4;});_tmp5E5->hd=_tmpAC4;}),({struct Cyc_List_List*_tmpAC0=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5E5->tl=_tmpAC0;});_tmp5E5;}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2473 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAC5=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpAC6,_tmpAC5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 331U: _LL291: _LL292:
# 2477 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAC8=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpAC8,_tmpAC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 332U: _LL293: _LL294:
# 2481 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpACB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC9=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2481
Cyc_Absyn_for_stmt(_tmpACC,_tmpACB,_tmpACA,_tmpAC9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2483
goto _LL0;case 333U: _LL295: _LL296:
# 2484 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpACF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpACD=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2484
Cyc_Absyn_for_stmt(_tmpAD0,_tmpACF,_tmpACE,_tmpACD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2486
goto _LL0;case 334U: _LL297: _LL298:
# 2487 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAD2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD1=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2487
Cyc_Absyn_for_stmt(_tmpAD4,_tmpAD3,_tmpAD2,_tmpAD1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2489
goto _LL0;case 335U: _LL299: _LL29A:
# 2490 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAD6=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAD5=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2490
Cyc_Absyn_for_stmt(_tmpAD8,_tmpAD7,_tmpAD6,_tmpAD5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2492
goto _LL0;case 336U: _LL29B: _LL29C:
# 2493 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpADC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpADB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD9=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2493
Cyc_Absyn_for_stmt(_tmpADC,_tmpADB,_tmpADA,_tmpAD9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2495
goto _LL0;case 337U: _LL29D: _LL29E:
# 2496 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpADF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADE=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpADD=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2496
Cyc_Absyn_for_stmt(_tmpAE0,_tmpADF,_tmpADE,_tmpADD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2498
goto _LL0;case 338U: _LL29F: _LL2A0:
# 2499 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAE3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAE2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAE1=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2499
Cyc_Absyn_for_stmt(_tmpAE4,_tmpAE3,_tmpAE2,_tmpAE1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2501
goto _LL0;case 339U: _LL2A1: _LL2A2:
# 2502 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAE7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAE6=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAE5=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2502
Cyc_Absyn_for_stmt(_tmpAE8,_tmpAE7,_tmpAE6,_tmpAE5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2504
goto _LL0;case 340U: _LL2A3: _LL2A4: {
# 2505 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E6=({struct Cyc_Absyn_Exp*_tmpAEC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAEB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAE9=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2505
Cyc_Absyn_for_stmt(_tmpAEC,_tmpAEB,_tmpAEA,_tmpAE9,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E6;
# 2507
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAED=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAED,s);}));
# 2509
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2510 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E7=({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAF0=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAEF=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAEE=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAF1,_tmpAF0,_tmpAEF,_tmpAEE,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E7;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF2=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAF2,s);}));
# 2513
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2514 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E8=({struct Cyc_Absyn_Exp*_tmpAF6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAF5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAF4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAF3=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAF6,_tmpAF5,_tmpAF4,_tmpAF3,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E8;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF7=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAF7,s);}));
# 2517
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2518 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E9=({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAFA=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAF9=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAF8=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpAFB,_tmpAFA,_tmpAF9,_tmpAF8,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E9;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAFC=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAFC,s);}));
# 2521
goto _LL0;}case 344U: _LL2AB: _LL2AC:
# 2524 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpAFE=({struct _fat_ptr*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));({struct _fat_ptr _tmpAFD=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5EA=_tmpAFD;});_tmp5EA;});Cyc_Absyn_goto_stmt(_tmpAFE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2525 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2526 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2527 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2528 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAFF=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpAFF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2530 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2531 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2533 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpB00=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpB00,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2542 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2545
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2547 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB02=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB01=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB03,_tmpB02,_tmpB01,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2550
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2552 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB05,_tmpB04,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2555
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2557 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB06=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpB07,_tmpB06,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2560
 yyval=(yyyvsp[0]).v;
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2562 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB08=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB09,_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2565
 yyval=(yyyvsp[0]).v;
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2567 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB0B,_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2570
 yyval=(yyyvsp[0]).v;
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2572 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB0D,_tmpB0C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2575
 yyval=(yyyvsp[0]).v;
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2577 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2579 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpB11,_tmpB10,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2582
 yyval=(yyyvsp[0]).v;
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2584 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB12=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB13,_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2586 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB15,_tmpB14,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2588 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB17,_tmpB16,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2590 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB19,_tmpB18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2593
 yyval=(yyyvsp[0]).v;
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2595 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB1B,_tmpB1A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2597 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB1D,_tmpB1C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2600
 yyval=(yyyvsp[0]).v;
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2602 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB1F,_tmpB1E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2604 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB21=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB20=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB21,_tmpB20,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2607
 yyval=(yyyvsp[0]).v;
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2609 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB23=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB22=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB23,_tmpB22,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2611 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB25=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB24=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB25,_tmpB24,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 382U: _LL2F7: _LL2F8:
# 2613 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB27=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB26=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB27,_tmpB26,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 383U: _LL2F9: _LL2FA:
# 2616
 yyval=(yyyvsp[0]).v;
goto _LL0;case 384U: _LL2FB: _LL2FC: {
# 2618 "parse.y"
void*_tmp5EB=({struct _tuple8*_tmpB28=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB28,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5EB;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB2A=t;struct Cyc_Absyn_Exp*_tmpB29=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB2A,_tmpB29,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2621
goto _LL0;}case 385U: _LL2FD: _LL2FE:
# 2624 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 386U: _LL2FF: _LL300:
# 2627
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 387U: _LL301: _LL302:
# 2629 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB2C=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB2B=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB2C,_tmpB2B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 388U: _LL303: _LL304: {
# 2631 "parse.y"
void*_tmp5EC=({struct _tuple8*_tmpB2D=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB2D,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5EC;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2634
goto _LL0;}case 389U: _LL305: _LL306:
# 2635 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB2E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB2E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 390U: _LL307: _LL308:
# 2637 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB30=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB2F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB30,_tmpB2F,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 391U: _LL309: _LL30A:
# 2642 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 392U: _LL30B: _LL30C:
# 2650 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 393U: _LL30D: _LL30E:
# 2655 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 394U: _LL30F: _LL310:
# 2657 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 395U: _LL311: _LL312: {
# 2659 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5ED=e->r;void*_stmttmp28=_tmp5ED;void*_tmp5EE=_stmttmp28;int _tmp5F0;struct _fat_ptr _tmp5EF;int _tmp5F2;enum Cyc_Absyn_Sign _tmp5F1;short _tmp5F4;enum Cyc_Absyn_Sign _tmp5F3;char _tmp5F6;enum Cyc_Absyn_Sign _tmp5F5;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).LongLong_c).tag){case 2U: _LL49F: _tmp5F5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Char_c).val).f1;_tmp5F6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Char_c).val).f2;_LL4A0: {enum Cyc_Absyn_Sign s=_tmp5F5;char i=_tmp5F6;
# 2663
yyval=Cyc_YY9(({void*_tmpB31=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->tag=11U,_tmp5F7->f1=i;_tmp5F7;});Cyc_Absyn_new_pat(_tmpB31,e->loc);}));goto _LL49E;}case 4U: _LL4A1: _tmp5F3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Short_c).val).f1;_tmp5F4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Short_c).val).f2;_LL4A2: {enum Cyc_Absyn_Sign s=_tmp5F3;short i=_tmp5F4;
# 2665
yyval=Cyc_YY9(({void*_tmpB32=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=10U,_tmp5F8->f1=s,_tmp5F8->f2=(int)i;_tmp5F8;});Cyc_Absyn_new_pat(_tmpB32,e->loc);}));goto _LL49E;}case 5U: _LL4A3: _tmp5F1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Int_c).val).f1;_tmp5F2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Int_c).val).f2;_LL4A4: {enum Cyc_Absyn_Sign s=_tmp5F1;int i=_tmp5F2;
# 2667
yyval=Cyc_YY9(({void*_tmpB33=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->tag=10U,_tmp5F9->f1=s,_tmp5F9->f2=i;_tmp5F9;});Cyc_Absyn_new_pat(_tmpB33,e->loc);}));goto _LL49E;}case 7U: _LL4A5: _tmp5EF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Float_c).val).f1;_tmp5F0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EE)->f1).Float_c).val).f2;_LL4A6: {struct _fat_ptr s=_tmp5EF;int i=_tmp5F0;
# 2669
yyval=Cyc_YY9(({void*_tmpB34=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->tag=12U,_tmp5FA->f1=s,_tmp5FA->f2=i;_tmp5FA;});Cyc_Absyn_new_pat(_tmpB34,e->loc);}));goto _LL49E;}case 1U: _LL4A7: _LL4A8:
# 2671
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL49E;case 8U: _LL4A9: _LL4AA:
 goto _LL4AC;case 9U: _LL4AB: _LL4AC:
# 2674
({void*_tmp5FB=0U;({unsigned _tmpB36=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB35=({const char*_tmp5FC="strings cannot occur within patterns";_tag_fat(_tmp5FC,sizeof(char),37U);});Cyc_Warn_err(_tmpB36,_tmpB35,_tag_fat(_tmp5FB,sizeof(void*),0U));});});goto _LL49E;case 6U: _LL4AD: _LL4AE:
# 2676
({void*_tmp5FD=0U;({unsigned _tmpB38=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB37=({const char*_tmp5FE="long long's in patterns not yet implemented";_tag_fat(_tmp5FE,sizeof(char),44U);});Cyc_Warn_err(_tmpB38,_tmpB37,_tag_fat(_tmp5FD,sizeof(void*),0U));});});goto _LL49E;default: goto _LL4AF;}else{_LL4AF: _LL4B0:
# 2678
({void*_tmp5FF=0U;({unsigned _tmpB3A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB39=({const char*_tmp600="bad constant in case";_tag_fat(_tmp600,sizeof(char),21U);});Cyc_Warn_err(_tmpB3A,_tmpB39,_tag_fat(_tmp5FF,sizeof(void*),0U));});});}_LL49E:;}
# 2681
goto _LL0;}case 396U: _LL313: _LL314:
# 2682 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB3C=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=15U,({struct _tuple0*_tmpB3B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp601->f1=_tmpB3B;});_tmp601;});Cyc_Absyn_new_pat(_tmpB3C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 397U: _LL315: _LL316:
# 2684 "parse.y"
 if(({struct _fat_ptr _tmpB3D=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB3D,({const char*_tmp602="as";_tag_fat(_tmp602,sizeof(char),3U);}));})!= 0)
({void*_tmp603=0U;({unsigned _tmpB3F=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB3E=({const char*_tmp604="expecting `as'";_tag_fat(_tmp604,sizeof(char),15U);});Cyc_Warn_err(_tmpB3F,_tmpB3E,_tag_fat(_tmp603,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB46=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB45=({unsigned _tmpB44=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB43=({struct _tuple0*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB42=({struct _fat_ptr*_tmp605=_cycalloc(sizeof(*_tmp605));({struct _fat_ptr _tmpB41=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp605=_tmpB41;});_tmp605;});_tmp606->f2=_tmpB42;});_tmp606;});Cyc_Absyn_new_vardecl(_tmpB44,_tmpB43,Cyc_Absyn_void_type,0);});_tmp607->f1=_tmpB45;}),({
struct Cyc_Absyn_Pat*_tmpB40=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp607->f2=_tmpB40;});_tmp607;});
# 2686
Cyc_Absyn_new_pat(_tmpB46,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2689
goto _LL0;case 398U: _LL317: _LL318:
# 2690 "parse.y"
 if(({struct _fat_ptr _tmpB47=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB47,({const char*_tmp608="alias";_tag_fat(_tmp608,sizeof(char),6U);}));})!= 0)
({void*_tmp609=0U;({unsigned _tmpB49=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB48=({const char*_tmp60A="expecting `alias'";_tag_fat(_tmp60A,sizeof(char),18U);});Cyc_Warn_err(_tmpB49,_tmpB48,_tag_fat(_tmp609,sizeof(void*),0U));});});{
int _tmp60B=((yyyvsp[0]).l).first_line;int location=_tmp60B;
({struct _fat_ptr _tmpB4A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB4A,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp611=_cycalloc(sizeof(*_tmp611));({struct _fat_ptr*_tmpB4D=({struct _fat_ptr*_tmp60F=_cycalloc(sizeof(*_tmp60F));({struct _fat_ptr _tmpB4C=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60F=_tmpB4C;});_tmp60F;});_tmp611->name=_tmpB4D;}),_tmp611->identity=- 1,({void*_tmpB4B=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=0U,_tmp610->f1=& Cyc_Kinds_rk;_tmp610;});_tmp611->kind=_tmpB4B;});_tmp611;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB52=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB51=({struct _tuple0*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB4F=({struct _fat_ptr*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct _fat_ptr _tmpB4E=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp60D=_tmpB4E;});_tmp60D;});_tmp60E->f2=_tmpB4F;});_tmp60E;});Cyc_Absyn_new_vardecl(_tmpB52,_tmpB51,({
struct _tuple8*_tmpB50=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB50,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB53=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=2U,_tmp60C->f1=tv,_tmp60C->f2=vd;_tmp60C;});Cyc_Absyn_new_pat(_tmpB53,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2699
goto _LL0;}}case 399U: _LL319: _LL31A:
# 2700 "parse.y"
 if(({struct _fat_ptr _tmpB54=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB54,({const char*_tmp612="alias";_tag_fat(_tmp612,sizeof(char),6U);}));})!= 0)
({void*_tmp613=0U;({unsigned _tmpB56=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB55=({const char*_tmp614="expecting `alias'";_tag_fat(_tmp614,sizeof(char),18U);});Cyc_Warn_err(_tmpB56,_tmpB55,_tag_fat(_tmp613,sizeof(void*),0U));});});{
int _tmp615=((yyyvsp[0]).l).first_line;int location=_tmp615;
({struct _fat_ptr _tmpB57=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB57,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp61B=_cycalloc(sizeof(*_tmp61B));({struct _fat_ptr*_tmpB5A=({struct _fat_ptr*_tmp619=_cycalloc(sizeof(*_tmp619));({struct _fat_ptr _tmpB59=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp619=_tmpB59;});_tmp619;});_tmp61B->name=_tmpB5A;}),_tmp61B->identity=- 1,({void*_tmpB58=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=0U,_tmp61A->f1=& Cyc_Kinds_rk;_tmp61A;});_tmp61B->kind=_tmpB58;});_tmp61B;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB5F=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB5E=({struct _tuple0*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB5C=({struct _fat_ptr*_tmp617=_cycalloc(sizeof(*_tmp617));({struct _fat_ptr _tmpB5B=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp617=_tmpB5B;});_tmp617;});_tmp618->f2=_tmpB5C;});_tmp618;});Cyc_Absyn_new_vardecl(_tmpB5F,_tmpB5E,({
struct _tuple8*_tmpB5D=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB5D,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB60=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=2U,_tmp616->f1=tv,_tmp616->f2=vd;_tmp616;});Cyc_Absyn_new_pat(_tmpB60,(unsigned)location);}));
# 2709
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2710 "parse.y"
struct _tuple23 _tmp61C=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp29=_tmp61C;struct _tuple23 _tmp61D=_stmttmp29;int _tmp61F;struct Cyc_List_List*_tmp61E;_LL4B2: _tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;_LL4B3: {struct Cyc_List_List*ps=_tmp61E;int dots=_tmp61F;
yyval=Cyc_YY9(({void*_tmpB61=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=5U,_tmp620->f1=ps,_tmp620->f2=dots;_tmp620;});Cyc_Absyn_new_pat(_tmpB61,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2713
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2714 "parse.y"
struct _tuple23 _tmp621=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp621;struct _tuple23 _tmp622=_stmttmp2A;int _tmp624;struct Cyc_List_List*_tmp623;_LL4B5: _tmp623=_tmp622.f1;_tmp624=_tmp622.f2;_LL4B6: {struct Cyc_List_List*ps=_tmp623;int dots=_tmp624;
yyval=Cyc_YY9(({void*_tmpB63=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=16U,({struct _tuple0*_tmpB62=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp625->f1=_tmpB62;}),_tmp625->f2=ps,_tmp625->f3=dots;_tmp625;});Cyc_Absyn_new_pat(_tmpB63,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2717
goto _LL0;}}case 402U: _LL31F: _LL320: {
# 2718 "parse.y"
struct _tuple23 _tmp626=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2B=_tmp626;struct _tuple23 _tmp627=_stmttmp2B;int _tmp629;struct Cyc_List_List*_tmp628;_LL4B8: _tmp628=_tmp627.f1;_tmp629=_tmp627.f2;_LL4B9: {struct Cyc_List_List*fps=_tmp628;int dots=_tmp629;
struct Cyc_List_List*_tmp62A=({unsigned _tmpB64=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB64,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp62A;
yyval=Cyc_YY9(({void*_tmpB67=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB66=({union Cyc_Absyn_AggrInfo*_tmp62B=_cycalloc(sizeof(*_tmp62B));({union Cyc_Absyn_AggrInfo _tmpB65=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp62B=_tmpB65;});_tmp62B;});_tmp62C->f1=_tmpB66;}),_tmp62C->f2=exist_ts,_tmp62C->f3=fps,_tmp62C->f4=dots;_tmp62C;});Cyc_Absyn_new_pat(_tmpB67,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2723
goto _LL0;}}case 403U: _LL321: _LL322: {
# 2724 "parse.y"
struct _tuple23 _tmp62D=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp62D;struct _tuple23 _tmp62E=_stmttmp2C;int _tmp630;struct Cyc_List_List*_tmp62F;_LL4BB: _tmp62F=_tmp62E.f1;_tmp630=_tmp62E.f2;_LL4BC: {struct Cyc_List_List*fps=_tmp62F;int dots=_tmp630;
struct Cyc_List_List*_tmp631=({unsigned _tmpB68=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB68,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp631;
yyval=Cyc_YY9(({void*_tmpB69=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=7U,_tmp632->f1=0,_tmp632->f2=exist_ts,_tmp632->f3=fps,_tmp632->f4=dots;_tmp632;});Cyc_Absyn_new_pat(_tmpB69,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2728
goto _LL0;}}case 404U: _LL323: _LL324:
# 2729 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB6B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->tag=6U,({struct Cyc_Absyn_Pat*_tmpB6A=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp633->f1=_tmpB6A;});_tmp633;});Cyc_Absyn_new_pat(_tmpB6B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 405U: _LL325: _LL326:
# 2731 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB6F=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->tag=6U,({struct Cyc_Absyn_Pat*_tmpB6E=({void*_tmpB6D=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=6U,({struct Cyc_Absyn_Pat*_tmpB6C=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp634->f1=_tmpB6C;});_tmp634;});Cyc_Absyn_new_pat(_tmpB6D,(unsigned)((yyyvsp[0]).l).first_line);});_tmp635->f1=_tmpB6E;});_tmp635;});Cyc_Absyn_new_pat(_tmpB6F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 406U: _LL327: _LL328:
# 2733 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB76=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB75=({unsigned _tmpB74=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB73=({struct _tuple0*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB72=({struct _fat_ptr*_tmp636=_cycalloc(sizeof(*_tmp636));({struct _fat_ptr _tmpB71=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp636=_tmpB71;});_tmp636;});_tmp637->f2=_tmpB72;});_tmp637;});Cyc_Absyn_new_vardecl(_tmpB74,_tmpB73,Cyc_Absyn_void_type,0);});_tmp638->f1=_tmpB75;}),({
# 2735
struct Cyc_Absyn_Pat*_tmpB70=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp638->f2=_tmpB70;});_tmp638;});
# 2733
Cyc_Absyn_new_pat(_tmpB76,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2737
goto _LL0;case 407U: _LL329: _LL32A:
# 2738 "parse.y"
 if(({struct _fat_ptr _tmpB77=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB77,({const char*_tmp639="as";_tag_fat(_tmp639,sizeof(char),3U);}));})!= 0)
({void*_tmp63A=0U;({unsigned _tmpB79=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB78=({const char*_tmp63B="expecting `as'";_tag_fat(_tmp63B,sizeof(char),15U);});Cyc_Warn_err(_tmpB79,_tmpB78,_tag_fat(_tmp63A,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB80=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB7F=({unsigned _tmpB7E=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB7D=({struct _tuple0*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB7C=({struct _fat_ptr*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct _fat_ptr _tmpB7B=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp63C=_tmpB7B;});_tmp63C;});_tmp63D->f2=_tmpB7C;});_tmp63D;});Cyc_Absyn_new_vardecl(_tmpB7E,_tmpB7D,Cyc_Absyn_void_type,0);});_tmp63E->f1=_tmpB7F;}),({
# 2742
struct Cyc_Absyn_Pat*_tmpB7A=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp63E->f2=_tmpB7A;});_tmp63E;});
# 2740
Cyc_Absyn_new_pat(_tmpB80,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2744
goto _LL0;case 408U: _LL32B: _LL32C: {
# 2745 "parse.y"
void*_tmp63F=({struct _fat_ptr _tmpB81=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB81,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp63F;
yyval=Cyc_YY9(({void*_tmpB88=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB87=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp642->f1=_tmpB87;}),({
struct Cyc_Absyn_Vardecl*_tmpB86=({unsigned _tmpB85=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB84=({struct _tuple0*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB83=({struct _fat_ptr*_tmp640=_cycalloc(sizeof(*_tmp640));({struct _fat_ptr _tmpB82=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp640=_tmpB82;});_tmp640;});_tmp641->f2=_tmpB83;});_tmp641;});Cyc_Absyn_new_vardecl(_tmpB85,_tmpB84,
Cyc_Absyn_tag_type(tag),0);});
# 2747
_tmp642->f2=_tmpB86;});_tmp642;});
# 2746
Cyc_Absyn_new_pat(_tmpB88,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2750
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2751 "parse.y"
struct Cyc_Absyn_Tvar*_tmp643=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp643;
yyval=Cyc_YY9(({void*_tmpB8E=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->tag=4U,_tmp646->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB8D=({unsigned _tmpB8C=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB8B=({struct _tuple0*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB8A=({struct _fat_ptr*_tmp644=_cycalloc(sizeof(*_tmp644));({struct _fat_ptr _tmpB89=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp644=_tmpB89;});_tmp644;});_tmp645->f2=_tmpB8A;});_tmp645;});Cyc_Absyn_new_vardecl(_tmpB8C,_tmpB8B,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2753
_tmp646->f2=_tmpB8D;});_tmp646;});
# 2752
Cyc_Absyn_new_pat(_tmpB8E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2756
goto _LL0;}case 410U: _LL32F: _LL330:
# 2759 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp647=_cycalloc(sizeof(*_tmp647));({struct Cyc_List_List*_tmpB8F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp647->f1=_tmpB8F;}),_tmp647->f2=0;_tmp647;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2760 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp648=_cycalloc(sizeof(*_tmp648));({struct Cyc_List_List*_tmpB90=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp648->f1=_tmpB90;}),_tmp648->f2=1;_tmp648;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2761 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=0,_tmp649->f2=1;_tmp649;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2766 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_Absyn_Pat*_tmpB91=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp64A->hd=_tmpB91;}),_tmp64A->tl=0;_tmp64A;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2768 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp64B=_cycalloc(sizeof(*_tmp64B));({struct Cyc_Absyn_Pat*_tmpB93=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp64B->hd=_tmpB93;}),({struct Cyc_List_List*_tmpB92=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp64B->tl=_tmpB92;});_tmp64B;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2773 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=0,({struct Cyc_Absyn_Pat*_tmpB94=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp64C->f2=_tmpB94;});_tmp64C;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2775 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct Cyc_List_List*_tmpB96=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp64D->f1=_tmpB96;}),({struct Cyc_Absyn_Pat*_tmpB95=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp64D->f2=_tmpB95;});_tmp64D;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2778
 yyval=Cyc_YY14(({struct _tuple23*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct Cyc_List_List*_tmpB97=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64E->f1=_tmpB97;}),_tmp64E->f2=0;_tmp64E;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2779 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp64F=_cycalloc(sizeof(*_tmp64F));({struct Cyc_List_List*_tmpB98=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64F->f1=_tmpB98;}),_tmp64F->f2=1;_tmp64F;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2780 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->f1=0,_tmp650->f2=1;_tmp650;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2785 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp651=_cycalloc(sizeof(*_tmp651));({struct _tuple24*_tmpB99=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp651->hd=_tmpB99;}),_tmp651->tl=0;_tmp651;}));
goto _LL0;case 421U: _LL345: _LL346:
# 2787 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp652=_cycalloc(sizeof(*_tmp652));({struct _tuple24*_tmpB9B=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp652->hd=_tmpB9B;}),({struct Cyc_List_List*_tmpB9A=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp652->tl=_tmpB9A;});_tmp652;}));
goto _LL0;case 422U: _LL347: _LL348:
# 2793 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 423U: _LL349: _LL34A:
# 2795 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB9D,_tmpB9C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424U: _LL34B: _LL34C:
# 2800 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 425U: _LL34D: _LL34E:
# 2802 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB9F=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpBA0,_tmpB9F,_tmpB9E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 426U: _LL34F: _LL350:
# 2804 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpBA2,_tmpBA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 427U: _LL351: _LL352:
# 2808 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 428U: _LL353: _LL354:
# 2809 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->v=(void*)Cyc_Absyn_Times;_tmp653;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2810 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)Cyc_Absyn_Div;_tmp654;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2811 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Absyn_Mod;_tmp655;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2812 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->v=(void*)Cyc_Absyn_Plus;_tmp656;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2813 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->v=(void*)Cyc_Absyn_Minus;_tmp657;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2814 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->v=(void*)Cyc_Absyn_Bitlshift;_tmp658;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2815 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->v=(void*)Cyc_Absyn_Bitlrshift;_tmp659;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2816 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->v=(void*)Cyc_Absyn_Bitand;_tmp65A;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2817 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->v=(void*)Cyc_Absyn_Bitxor;_tmp65B;}));
goto _LL0;case 437U: _LL365: _LL366:
# 2818 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->v=(void*)Cyc_Absyn_Bitor;_tmp65C;}));
goto _LL0;case 438U: _LL367: _LL368:
# 2823 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 439U: _LL369: _LL36A:
# 2825 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpBA5,_tmpBA4,_tmpBA3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2828
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpBA6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2831
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpBA7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2833 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpBA8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2835 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBAA,_tmpBA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 444U: _LL373: _LL374:
# 2837 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBAC,_tmpBAB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 445U: _LL375: _LL376:
# 2841 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446U: _LL377: _LL378:
# 2845 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 447U: _LL379: _LL37A:
# 2847 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpBAE,_tmpBAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 448U: _LL37B: _LL37C:
# 2851 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 449U: _LL37D: _LL37E:
# 2853 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpBB0,_tmpBAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 450U: _LL37F: _LL380:
# 2857 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 451U: _LL381: _LL382:
# 2859 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpBB2,_tmpBB1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 452U: _LL383: _LL384:
# 2863 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 453U: _LL385: _LL386:
# 2865 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBB4,_tmpBB3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 454U: _LL387: _LL388:
# 2869 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 455U: _LL389: _LL38A:
# 2871 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBB6,_tmpBB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 456U: _LL38B: _LL38C:
# 2875 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 457U: _LL38D: _LL38E:
# 2877 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBB8,_tmpBB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 458U: _LL38F: _LL390:
# 2879 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBBA,_tmpBB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 459U: _LL391: _LL392:
# 2883 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 460U: _LL393: _LL394:
# 2885 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBBC,_tmpBBB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2887 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBBE,_tmpBBD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2889 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBC0,_tmpBBF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463U: _LL399: _LL39A:
# 2891 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBC2,_tmpBC1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464U: _LL39B: _LL39C:
# 2895 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 465U: _LL39D: _LL39E:
# 2897 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBC4,_tmpBC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2899 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBC6,_tmpBC5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2903 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 2905 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBC8,_tmpBC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 2907 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBCA,_tmpBC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 2911 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 2913 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBCC,_tmpBCB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 2915 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBCE,_tmpBCD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 473U: _LL3AD: _LL3AE:
# 2917 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBD0,_tmpBCF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 474U: _LL3AF: _LL3B0:
# 2921 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 475U: _LL3B1: _LL3B2: {
# 2923 "parse.y"
void*_tmp65D=({struct _tuple8*_tmpBD1=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBD1,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp65D;
yyval=Cyc_Exp_tok(({void*_tmpBD3=t;struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBD3,_tmpBD2,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2926
goto _LL0;}case 476U: _LL3B3: _LL3B4:
# 2929 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 2930 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBD4,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 2931 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBD5,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 2932 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBD6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 2933 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBD7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE:
# 2934 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBD8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 482U: _LL3BF: _LL3C0:
# 2935 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBDA=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBDA,_tmpBD9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 483U: _LL3C1: _LL3C2: {
# 2937 "parse.y"
void*_tmp65E=({struct _tuple8*_tmpBDB=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBDB,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65E;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2940
goto _LL0;}case 484U: _LL3C3: _LL3C4:
# 2940 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBDC=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBDC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 485U: _LL3C5: _LL3C6: {
# 2942 "parse.y"
void*_tmp65F=({struct _tuple8*_tmpBDD=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBDD,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65F;
yyval=Cyc_Exp_tok(({void*_tmpBDF=t;struct Cyc_List_List*_tmpBDE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBDF,_tmpBDE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2945
goto _LL0;}case 486U: _LL3C7: _LL3C8:
# 2947
 yyval=Cyc_Exp_tok(({void*_tmpBE1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->tag=34U,(_tmp660->f1).is_calloc=0,(_tmp660->f1).rgn=0,(_tmp660->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBE0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp660->f1).num_elts=_tmpBE0;}),(_tmp660->f1).fat_result=0,(_tmp660->f1).inline_call=0;_tmp660;});Cyc_Absyn_new_exp(_tmpBE1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2949
goto _LL0;case 487U: _LL3C9: _LL3CA:
# 2950 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=34U,(_tmp661->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBE3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp661->f1).rgn=_tmpBE3;}),(_tmp661->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBE2=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp661->f1).num_elts=_tmpBE2;}),(_tmp661->f1).fat_result=0,(_tmp661->f1).inline_call=0;_tmp661;});Cyc_Absyn_new_exp(_tmpBE4,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2952
goto _LL0;case 488U: _LL3CB: _LL3CC:
# 2953 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE7=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->tag=34U,(_tmp662->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBE6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp662->f1).rgn=_tmpBE6;}),(_tmp662->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBE5=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp662->f1).num_elts=_tmpBE5;}),(_tmp662->f1).fat_result=0,(_tmp662->f1).inline_call=1;_tmp662;});Cyc_Absyn_new_exp(_tmpBE7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2955
goto _LL0;case 489U: _LL3CD: _LL3CE: {
# 2956 "parse.y"
void*_tmp663=({struct _tuple8*_tmpBE8=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBE8,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp663;
yyval=Cyc_Exp_tok(({void*_tmpBEB=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->tag=34U,(_tmp665->f1).is_calloc=1,(_tmp665->f1).rgn=0,({void**_tmpBEA=({void**_tmp664=_cycalloc(sizeof(*_tmp664));*_tmp664=t;_tmp664;});(_tmp665->f1).elt_type=_tmpBEA;}),({struct Cyc_Absyn_Exp*_tmpBE9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp665->f1).num_elts=_tmpBE9;}),(_tmp665->f1).fat_result=0,(_tmp665->f1).inline_call=0;_tmp665;});Cyc_Absyn_new_exp(_tmpBEB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2959
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 2961
void*_tmp666=({struct _tuple8*_tmpBEC=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBEC,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp666;
yyval=Cyc_Exp_tok(({void*_tmpBF0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->tag=34U,(_tmp668->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBEF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp668->f1).rgn=_tmpBEF;}),({void**_tmpBEE=({void**_tmp667=_cycalloc(sizeof(*_tmp667));*_tmp667=t;_tmp667;});(_tmp668->f1).elt_type=_tmpBEE;}),({struct Cyc_Absyn_Exp*_tmpBED=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp668->f1).num_elts=_tmpBED;}),(_tmp668->f1).fat_result=0,(_tmp668->f1).inline_call=0;_tmp668;});Cyc_Absyn_new_exp(_tmpBF0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2964
goto _LL0;}case 491U: _LL3D1: _LL3D2:
# 2965 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBF2=({struct Cyc_Absyn_Exp*_tmp669[1U];({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp669[0]=_tmpBF1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp669,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBF2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4:
# 2967 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF6=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->tag=38U,({struct Cyc_Absyn_Exp*_tmpBF5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp66B->f1=_tmpBF5;}),({struct _fat_ptr*_tmpBF4=({struct _fat_ptr*_tmp66A=_cycalloc(sizeof(*_tmp66A));({struct _fat_ptr _tmpBF3=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp66A=_tmpBF3;});_tmp66A;});_tmp66B->f2=_tmpBF4;});_tmp66B;});Cyc_Absyn_new_exp(_tmpBF6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 493U: _LL3D5: _LL3D6:
# 2969 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBFB=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->tag=38U,({struct Cyc_Absyn_Exp*_tmpBFA=({struct Cyc_Absyn_Exp*_tmpBF9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpBF9,(unsigned)((yyyvsp[2]).l).first_line);});_tmp66D->f1=_tmpBFA;}),({struct _fat_ptr*_tmpBF8=({struct _fat_ptr*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct _fat_ptr _tmpBF7=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp66C=_tmpBF7;});_tmp66C;});_tmp66D->f2=_tmpBF8;});_tmp66D;});Cyc_Absyn_new_exp(_tmpBFB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 494U: _LL3D7: _LL3D8: {
# 2971 "parse.y"
void*_tmp66E=({struct _tuple8*_tmpBFC=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBFC,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp66E;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 495U: _LL3D9: _LL3DA:
# 2974 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBFD=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpBFD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC:
# 2975 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBFE=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpBFE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 497U: _LL3DD: _LL3DE:
# 2977 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBFF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpBFF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 498U: _LL3DF: _LL3E0: {
# 2982 "parse.y"
struct _tuple29*_tmp66F=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp2D=_tmp66F;struct _tuple29*_tmp670=_stmttmp2D;struct Cyc_List_List*_tmp673;struct Cyc_List_List*_tmp672;struct Cyc_List_List*_tmp671;_LL4BE: _tmp671=_tmp670->f1;_tmp672=_tmp670->f2;_tmp673=_tmp670->f3;_LL4BF: {struct Cyc_List_List*outlist=_tmp671;struct Cyc_List_List*inlist=_tmp672;struct Cyc_List_List*clobbers=_tmp673;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->tag=40U,({int _tmpC01=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp674->f1=_tmpC01;}),({struct _fat_ptr _tmpC00=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp674->f2=_tmpC00;}),_tmp674->f3=outlist,_tmp674->f4=inlist,_tmp674->f5=clobbers;_tmp674;}));
goto _LL0;}}case 499U: _LL3E1: _LL3E2:
# 2987 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 500U: _LL3E3: _LL3E4:
# 2988 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 501U: _LL3E5: _LL3E6:
# 2992 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->f1=0,_tmp675->f2=0,_tmp675->f3=0;_tmp675;}));
goto _LL0;case 502U: _LL3E7: _LL3E8: {
# 2994 "parse.y"
struct _tuple30*_tmp676=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp2E=_tmp676;struct _tuple30*_tmp677=_stmttmp2E;struct Cyc_List_List*_tmp679;struct Cyc_List_List*_tmp678;_LL4C1: _tmp678=_tmp677->f1;_tmp679=_tmp677->f2;_LL4C2: {struct Cyc_List_List*inlist=_tmp678;struct Cyc_List_List*clobbers=_tmp679;
yyval=Cyc_YY59(({struct _tuple29*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->f1=0,_tmp67A->f2=inlist,_tmp67A->f3=clobbers;_tmp67A;}));
goto _LL0;}}case 503U: _LL3E9: _LL3EA: {
# 2997 "parse.y"
struct _tuple30*_tmp67B=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp2F=_tmp67B;struct _tuple30*_tmp67C=_stmttmp2F;struct Cyc_List_List*_tmp67E;struct Cyc_List_List*_tmp67D;_LL4C4: _tmp67D=_tmp67C->f1;_tmp67E=_tmp67C->f2;_LL4C5: {struct Cyc_List_List*inlist=_tmp67D;struct Cyc_List_List*clobbers=_tmp67E;
yyval=Cyc_YY59(({struct _tuple29*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct Cyc_List_List*_tmpC02=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp67F->f1=_tmpC02;}),_tmp67F->f2=inlist,_tmp67F->f3=clobbers;_tmp67F;}));
goto _LL0;}}case 504U: _LL3EB: _LL3EC:
# 3002 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp680=_cycalloc(sizeof(*_tmp680));({struct _tuple31*_tmpC03=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp680->hd=_tmpC03;}),_tmp680->tl=0;_tmp680;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 3003 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));({struct _tuple31*_tmpC05=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp681->hd=_tmpC05;}),({struct Cyc_List_List*_tmpC04=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp681->tl=_tmpC04;});_tmp681;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 3007 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->f1=0,_tmp682->f2=0;_tmp682;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 3009 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->f1=0,({struct Cyc_List_List*_tmpC06=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp683->f2=_tmpC06;});_tmp683;}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 3011 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp684=_cycalloc(sizeof(*_tmp684));({struct Cyc_List_List*_tmpC08=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp684->f1=_tmpC08;}),({struct Cyc_List_List*_tmpC07=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp684->f2=_tmpC07;});_tmp684;}));
goto _LL0;case 509U: _LL3F5: _LL3F6:
# 3015 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct _tuple31*_tmpC09=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp685->hd=_tmpC09;}),_tmp685->tl=0;_tmp685;}));
goto _LL0;case 510U: _LL3F7: _LL3F8:
# 3016 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));({struct _tuple31*_tmpC0B=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp686->hd=_tmpC0B;}),({struct Cyc_List_List*_tmpC0A=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp686->tl=_tmpC0A;});_tmp686;}));
goto _LL0;case 511U: _LL3F9: _LL3FA: {
# 3021 "parse.y"
struct Cyc_Absyn_Exp*_tmp687=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp687;
yyval=Cyc_YY63(({struct _tuple31*_tmp688=_cycalloc(sizeof(*_tmp688));({struct _fat_ptr _tmpC0D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp688->f1=_tmpC0D;}),({struct Cyc_Absyn_Exp*_tmpC0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp688->f2=_tmpC0C;});_tmp688;}));
goto _LL0;}case 512U: _LL3FB: _LL3FC:
# 3027 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3028 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 514U: _LL3FF: _LL400:
# 3029 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 515U: _LL401: _LL402:
# 3033 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp68A=_cycalloc(sizeof(*_tmp68A));({struct _fat_ptr*_tmpC0F=({struct _fat_ptr*_tmp689=_cycalloc(sizeof(*_tmp689));({struct _fat_ptr _tmpC0E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp689=_tmpC0E;});_tmp689;});_tmp68A->hd=_tmpC0F;}),_tmp68A->tl=0;_tmp68A;}));
goto _LL0;case 516U: _LL403: _LL404:
# 3034 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct _fat_ptr*_tmpC12=({struct _fat_ptr*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct _fat_ptr _tmpC11=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68B=_tmpC11;});_tmp68B;});_tmp68C->hd=_tmpC12;}),({struct Cyc_List_List*_tmpC10=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp68C->tl=_tmpC10;});_tmp68C;}));
goto _LL0;case 517U: _LL405: _LL406:
# 3038 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 518U: _LL407: _LL408:
# 3039 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 519U: _LL409: _LL40A:
# 3040 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 520U: _LL40B: _LL40C:
# 3045 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 521U: _LL40D: _LL40E:
# 3047 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC14=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpC13=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC14,_tmpC13,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3049 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC15=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC15,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3051 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC17=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC16=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC17,_tmpC16,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3053 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC1A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC19=({struct _fat_ptr*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct _fat_ptr _tmpC18=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68D=_tmpC18;});_tmp68D;});Cyc_Absyn_aggrmember_exp(_tmpC1A,_tmpC19,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3055 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC1D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC1C=({struct _fat_ptr*_tmp68E=_cycalloc(sizeof(*_tmp68E));({struct _fat_ptr _tmpC1B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68E=_tmpC1B;});_tmp68E;});Cyc_Absyn_aggrarrow_exp(_tmpC1D,_tmpC1C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3057 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC1E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC1E,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3059 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC1F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC1F,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3061 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC21=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=25U,({struct _tuple8*_tmpC20=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68F->f1=_tmpC20;}),_tmp68F->f2=0;_tmp68F;});Cyc_Absyn_new_exp(_tmpC21,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3063 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC24=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->tag=25U,({struct _tuple8*_tmpC23=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp690->f1=_tmpC23;}),({struct Cyc_List_List*_tmpC22=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp690->f2=_tmpC22;});_tmp690;});Cyc_Absyn_new_exp(_tmpC24,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3065 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC27=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->tag=25U,({struct _tuple8*_tmpC26=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp691->f1=_tmpC26;}),({struct Cyc_List_List*_tmpC25=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp691->f2=_tmpC25;});_tmp691;});Cyc_Absyn_new_exp(_tmpC27,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 531U: _LL421: _LL422:
# 3070 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));({void*_tmpC2A=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->tag=0U,({struct _fat_ptr*_tmpC29=({struct _fat_ptr*_tmp692=_cycalloc(sizeof(*_tmp692));({struct _fat_ptr _tmpC28=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp692=_tmpC28;});_tmp692;});_tmp693->f1=_tmpC29;});_tmp693;});_tmp694->hd=_tmpC2A;}),_tmp694->tl=0;_tmp694;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3073
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp696=_cycalloc(sizeof(*_tmp696));({void*_tmpC2D=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->tag=1U,({unsigned _tmpC2C=({unsigned _tmpC2B=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC2B,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp695->f1=_tmpC2C;});_tmp695;});_tmp696->hd=_tmpC2D;}),_tmp696->tl=0;_tmp696;}));
goto _LL0;case 533U: _LL425: _LL426:
# 3075 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));({void*_tmpC31=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tag=0U,({struct _fat_ptr*_tmpC30=({struct _fat_ptr*_tmp697=_cycalloc(sizeof(*_tmp697));({struct _fat_ptr _tmpC2F=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp697=_tmpC2F;});_tmp697;});_tmp698->f1=_tmpC30;});_tmp698;});_tmp699->hd=_tmpC31;}),({struct Cyc_List_List*_tmpC2E=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp699->tl=_tmpC2E;});_tmp699;}));
goto _LL0;case 534U: _LL427: _LL428:
# 3078
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));({void*_tmpC35=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->tag=1U,({unsigned _tmpC34=({unsigned _tmpC33=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC33,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp69A->f1=_tmpC34;});_tmp69A;});_tmp69B->hd=_tmpC35;}),({struct Cyc_List_List*_tmpC32=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp69B->tl=_tmpC32;});_tmp69B;}));
goto _LL0;case 535U: _LL429: _LL42A:
# 3084 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC36=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC36,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536U: _LL42B: _LL42C:
# 3086 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC37=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC37,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 537U: _LL42D: _LL42E:
# 3088 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 538U: _LL42F: _LL430:
# 3090 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC38=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC38,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 539U: _LL431: _LL432:
# 3092 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC39=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC39,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 540U: _LL433: _LL434:
# 3094 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 541U: _LL435: _LL436:
# 3099 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC3A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC3A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3101 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC3C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC3B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC3C,_tmpC3B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3104
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC3D=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC3D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3107
 yyval=Cyc_Exp_tok(({void*_tmpC41=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->tag=29U,({struct _tuple0*_tmpC40=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp69C->f1=_tmpC40;}),({struct Cyc_List_List*_tmpC3F=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp69C->f2=_tmpC3F;}),({struct Cyc_List_List*_tmpC3E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp69C->f3=_tmpC3E;}),_tmp69C->f4=0;_tmp69C;});Cyc_Absyn_new_exp(_tmpC41,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3110
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC42=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC42,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 546U: _LL43F: _LL440:
# 3114 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 547U: _LL441: _LL442:
# 3120 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));({struct Cyc_Absyn_Exp*_tmpC43=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp69D->hd=_tmpC43;}),_tmp69D->tl=0;_tmp69D;}));
goto _LL0;case 548U: _LL443: _LL444:
# 3122 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));({struct Cyc_Absyn_Exp*_tmpC45=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp69E->hd=_tmpC45;}),({struct Cyc_List_List*_tmpC44=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp69E->tl=_tmpC44;});_tmp69E;}));
goto _LL0;case 549U: _LL445: _LL446:
# 3128 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC46=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC46,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 550U: _LL447: _LL448:
# 3129 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC47=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC47,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 551U: _LL449: _LL44A:
# 3130 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC48=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC48,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 552U: _LL44B: _LL44C: {
# 3132 "parse.y"
struct _fat_ptr _tmp69F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp69F;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3140
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3142
goto _LL0;}case 553U: _LL44D: _LL44E:
# 3143 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 554U: _LL44F: _LL450:
# 3147 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({union Cyc_Absyn_Nmspace _tmpC4B=Cyc_Absyn_Rel_n(0);_tmp6A1->f1=_tmpC4B;}),({struct _fat_ptr*_tmpC4A=({struct _fat_ptr*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));({struct _fat_ptr _tmpC49=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6A0=_tmpC49;});_tmp6A0;});_tmp6A1->f2=_tmpC4A;});_tmp6A1;}));
goto _LL0;case 555U: _LL451: _LL452:
# 3148 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 556U: _LL453: _LL454:
# 3151
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));({union Cyc_Absyn_Nmspace _tmpC4E=Cyc_Absyn_Rel_n(0);_tmp6A3->f1=_tmpC4E;}),({struct _fat_ptr*_tmpC4D=({struct _fat_ptr*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));({struct _fat_ptr _tmpC4C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6A2=_tmpC4C;});_tmp6A2;});_tmp6A3->f2=_tmpC4D;});_tmp6A3;}));
goto _LL0;case 557U: _LL455: _LL456:
# 3152 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3157 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3158 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 560U: _LL45B: _LL45C:
# 3161
 yyval=(yyyvsp[0]).v;
goto _LL0;case 561U: _LL45D: _LL45E:
# 3162 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 562U: _LL45F: _LL460:
# 3167 "parse.y"
 goto _LL0;case 563U: _LL461: _LL462:
# 3167 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL463: _LL464:
# 3171
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
({int _tmpC4F=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC4F;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC50=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 154));_tmpC50 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7941)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7942U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,162U,sizeof(short),yyn - 154));}
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
if(yyn > - 32768 && yyn < 7941){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 316U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7942U,sizeof(short),x + yyn))== x)
({unsigned long _tmpC51=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,316U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC51;}),count ++;}
msg=({unsigned _tmp6A5=(unsigned)(sze + 15)+ 1U;char*_tmp6A4=({struct _RegionHandle*_tmpC52=yyregion;_region_malloc(_tmpC52,_check_times(_tmp6A5,sizeof(char)));});({{unsigned _tmp769=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp769;++ i){_tmp6A4[i]='\000';}_tmp6A4[_tmp769]=0;}0;});_tag_fat(_tmp6A4,sizeof(char),_tmp6A5);});
({struct _fat_ptr _tmpC53=msg;Cyc_strcpy(_tmpC53,({const char*_tmp6A6="parse error";_tag_fat(_tmp6A6,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 316U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7942U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC54=msg;Cyc_strcat(_tmpC54,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp6A7=", expecting `";_tag_fat(_tmp6A7,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp6A8=" or `";_tag_fat(_tmp6A8,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,316U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC55=msg;Cyc_strcat(_tmpC55,({const char*_tmp6A9="'";_tag_fat(_tmp6A9,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC57=({const char*_tmp6AA="parse error";_tag_fat(_tmp6AA,sizeof(char),12U);});int _tmpC56=yystate;Cyc_yyerror(_tmpC57,_tmpC56,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp6AB=1;_npop_handler(0U);return _tmp6AB;}
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
 if(yyssp_offset == 0){int _tmp6AC=1;_npop_handler(0U);return _tmp6AC;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1143U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7941)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7942U,sizeof(short),yyn))!= 1)goto yyerrdefault;
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
if(yyn == 1142){
int _tmp6AD=0;_npop_handler(0U);return _tmp6AD;}
# 546
({struct Cyc_Yystacktype _tmpC58=({struct Cyc_Yystacktype _tmp76A;_tmp76A.v=yylval,_tmp76A.l=yylloc;_tmp76A;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC58;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3170 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6B1=v;struct Cyc_Absyn_Stmt*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B3;struct _tuple0*_tmp6B4;struct _fat_ptr _tmp6B5;char _tmp6B6;union Cyc_Absyn_Cnst _tmp6B7;switch((_tmp6B1.Stmt_tok).tag){case 1U: _LL1: _tmp6B7=(_tmp6B1.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6B7;
({struct Cyc_String_pa_PrintArg_struct _tmp6BA=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,({struct _fat_ptr _tmpC59=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp76D.f1=_tmpC59;});_tmp76D;});void*_tmp6B8[1U];_tmp6B8[0]=& _tmp6BA;({struct Cyc___cycFILE*_tmpC5B=Cyc_stderr;struct _fat_ptr _tmpC5A=({const char*_tmp6B9="%s";_tag_fat(_tmp6B9,sizeof(char),3U);});Cyc_fprintf(_tmpC5B,_tmpC5A,_tag_fat(_tmp6B8,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6B6=(_tmp6B1.Char_tok).val;_LL4: {char c=_tmp6B6;
({struct Cyc_Int_pa_PrintArg_struct _tmp6BD=({struct Cyc_Int_pa_PrintArg_struct _tmp76E;_tmp76E.tag=1U,_tmp76E.f1=(unsigned long)((int)c);_tmp76E;});void*_tmp6BB[1U];_tmp6BB[0]=& _tmp6BD;({struct Cyc___cycFILE*_tmpC5D=Cyc_stderr;struct _fat_ptr _tmpC5C=({const char*_tmp6BC="%c";_tag_fat(_tmp6BC,sizeof(char),3U);});Cyc_fprintf(_tmpC5D,_tmpC5C,_tag_fat(_tmp6BB,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6B5=(_tmp6B1.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6B5;
({struct Cyc_String_pa_PrintArg_struct _tmp6C0=({struct Cyc_String_pa_PrintArg_struct _tmp76F;_tmp76F.tag=0U,_tmp76F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp76F;});void*_tmp6BE[1U];_tmp6BE[0]=& _tmp6C0;({struct Cyc___cycFILE*_tmpC5F=Cyc_stderr;struct _fat_ptr _tmpC5E=({const char*_tmp6BF="\"%s\"";_tag_fat(_tmp6BF,sizeof(char),5U);});Cyc_fprintf(_tmpC5F,_tmpC5E,_tag_fat(_tmp6BE,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6B4=(_tmp6B1.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6B4;
({struct Cyc_String_pa_PrintArg_struct _tmp6C3=({struct Cyc_String_pa_PrintArg_struct _tmp770;_tmp770.tag=0U,({struct _fat_ptr _tmpC60=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp770.f1=_tmpC60;});_tmp770;});void*_tmp6C1[1U];_tmp6C1[0]=& _tmp6C3;({struct Cyc___cycFILE*_tmpC62=Cyc_stderr;struct _fat_ptr _tmpC61=({const char*_tmp6C2="%s";_tag_fat(_tmp6C2,sizeof(char),3U);});Cyc_fprintf(_tmpC62,_tmpC61,_tag_fat(_tmp6C1,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6B3=(_tmp6B1.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6B3;
({struct Cyc_String_pa_PrintArg_struct _tmp6C6=({struct Cyc_String_pa_PrintArg_struct _tmp771;_tmp771.tag=0U,({struct _fat_ptr _tmpC63=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp771.f1=_tmpC63;});_tmp771;});void*_tmp6C4[1U];_tmp6C4[0]=& _tmp6C6;({struct Cyc___cycFILE*_tmpC65=Cyc_stderr;struct _fat_ptr _tmpC64=({const char*_tmp6C5="%s";_tag_fat(_tmp6C5,sizeof(char),3U);});Cyc_fprintf(_tmpC65,_tmpC64,_tag_fat(_tmp6C4,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6B2=(_tmp6B1.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6B2;
({struct Cyc_String_pa_PrintArg_struct _tmp6C9=({struct Cyc_String_pa_PrintArg_struct _tmp772;_tmp772.tag=0U,({struct _fat_ptr _tmpC66=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp772.f1=_tmpC66;});_tmp772;});void*_tmp6C7[1U];_tmp6C7[0]=& _tmp6C9;({struct Cyc___cycFILE*_tmpC68=Cyc_stderr;struct _fat_ptr _tmpC67=({const char*_tmp6C8="%s";_tag_fat(_tmp6C8,sizeof(char),3U);});Cyc_fprintf(_tmpC68,_tmpC67,_tag_fat(_tmp6C7,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6CA=0U;({struct Cyc___cycFILE*_tmpC6A=Cyc_stderr;struct _fat_ptr _tmpC69=({const char*_tmp6CB="?";_tag_fat(_tmp6CB,sizeof(char),2U);});Cyc_fprintf(_tmpC6A,_tmpC69,_tag_fat(_tmp6CA,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3182
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6CC="end-of-file";_tag_fat(_tmp6CC,sizeof(char),12U);});
if(token == 369)
return Cyc_Lex_token_string;
if(token == 378)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 381?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,382U,sizeof(short),token)): 316;
if((unsigned)z < 316U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 3196
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6CD=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6CD;_push_region(yyr);
({struct _RegionHandle*_tmpC6B=yyr;Cyc_yyparse(_tmpC6B,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6CE=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6CE;}
# 3199
;_pop_region();}}
