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
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
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
# 1096
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1098
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1105
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1115
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1120
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
void*Cyc_CurRgn_curr_rgn_type (void);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
({struct Cyc_List_List*_tmp76C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp76C;});
({struct _tuple11*_tmp76D=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp76D;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmpA=0U;({unsigned _tmp76F=loc;struct _fat_ptr _tmp76E=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp76F,_tmp76E,_tag_fat(_tmpA,sizeof(void*),0U));});});}
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
return({struct _tuple15 _tmp6C8;_tmp6C8.f1=nullable,_tmp6C8.f2=bound,_tmp6C8.f3=zeroterm,_tmp6C8.f4=rgn,_tmp6C8.f5=released;_tmp6C8;});}
# 241
struct _tuple0*Cyc_Parse_gensym_enum (void){
# 243
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp773=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp773;}),({
struct _fat_ptr*_tmp772=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp771=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6C9;_tmp6C9.tag=1U,_tmp6C9.f1=(unsigned long)enum_counter ++;_tmp6C9;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp770=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp770,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp771;});_tmp13;});_tmp14->f2=_tmp772;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 248
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 253
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp775=loc;struct _fat_ptr _tmp774=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp775,_tmp774,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp777=loc;struct _fat_ptr _tmp776=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp777,_tmp776,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 263
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6CA;_tmp6CA.Signed_spec=0,_tmp6CA.Unsigned_spec=0,_tmp6CA.Short_spec=0,_tmp6CA.Long_spec=0,_tmp6CA.Long_Long_spec=0,_tmp6CA.Valid_type_spec=0,_tmp6CA.Type_spec=Cyc_Absyn_sint_type,_tmp6CA.loc=loc;_tmp6CA;});}
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
void*_tmp778=t;Cyc_Tcutil_promote_array(_tmp778,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 317 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 322
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 326
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp77A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6CB;_tmp6CB.tag=0U,_tmp6CB.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6CB;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp779=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp779,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp77A;});_tmp36;});
({void*_tmp77C=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp77B=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Kinds_ik;_tmp31;});_tmp32->kind=_tmp77B;});_tmp32;}));*z=_tmp77C;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 331
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp77D=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp77D;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 335
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp77F=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6CC;_tmp6CC.tag=0U,_tmp6CC.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6CC;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp77E=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp77E,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp77F;});_tmp3E;});
({void*_tmp781=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp780=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Kinds_rk;_tmp39;});_tmp3A->kind=_tmp780;});_tmp3A;}));*z=_tmp781;});
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
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp782=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp782;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
return({void*_tmp784=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp783=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp783;});_tmp4D;});Cyc_Absyn_new_exp(_tmp784,e->loc);});}}}
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
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6CD;_tmp6CD.elt_type=et2,_tmp6CD.elt_tq=tq,(_tmp6CD.ptr_atts).rgn=r,(_tmp6CD.ptr_atts).nullable=n,(_tmp6CD.ptr_atts).bounds=b2,(_tmp6CD.ptr_atts).zero_term=zt,(_tmp6CD.ptr_atts).ptrloc=pl,(_tmp6CD.ptr_atts).released=rel;_tmp6CD;}));
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
({void*_tmp785=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp785;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
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
if(({struct _fat_ptr _tmp786=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp786,({const char*_tmp65="`H";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp787=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp787,({const char*_tmp66="`U";_tag_fat(_tmp66,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp788=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp788,({const char*_tmp67="`RC";_tag_fat(_tmp67,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _fat_ptr*_tmp789=({struct _fat_ptr*_tmp68=_cycalloc(sizeof(*_tmp68));*_tmp68=s;_tmp68;});_tmp69->name=_tmp789;}),_tmp69->identity=- 1,_tmp69->kind=k;_tmp69;}));}
# 436
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp78A=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78A,({const char*_tmp6A="`H";_tag_fat(_tmp6A,sizeof(char),3U);}));})== 0)
({void*_tmp6B=0U;({unsigned _tmp78C=loc;struct _fat_ptr _tmp78B=({const char*_tmp6C="bad occurrence of heap region";_tag_fat(_tmp6C,sizeof(char),30U);});Cyc_Warn_err(_tmp78C,_tmp78B,_tag_fat(_tmp6B,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp78D=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78D,({const char*_tmp6D="`U";_tag_fat(_tmp6D,sizeof(char),3U);}));})== 0)
({void*_tmp6E=0U;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp6F="bad occurrence of unique region";_tag_fat(_tmp6F,sizeof(char),32U);});Cyc_Warn_err(_tmp78F,_tmp78E,_tag_fat(_tmp6E,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp790=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp790,({const char*_tmp70="`RC";_tag_fat(_tmp70,sizeof(char),4U);}));})== 0)
({void*_tmp71=0U;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp72="bad occurrence of refcounted region";_tag_fat(_tmp72,sizeof(char),36U);});Cyc_Warn_err(_tmp792,_tmp791,_tag_fat(_tmp71,sizeof(void*),0U));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp73=0U;({unsigned _tmp794=loc;struct _fat_ptr _tmp793=({const char*_tmp74="bad occurrence of \"current\" region";_tag_fat(_tmp74,sizeof(char),35U);});Cyc_Warn_err(_tmp794,_tmp793,_tag_fat(_tmp73,sizeof(void*),0U));});});}
# 451
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp75=t;struct Cyc_Absyn_Tvar*_tmp76;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->tag == 2U){_LL1: _tmp76=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp76;
return pr;}}else{_LL3: _LL4:
({void*_tmp77=0U;({unsigned _tmp796=loc;struct _fat_ptr _tmp795=({const char*_tmp78="expecting a list of type variables, not types";_tag_fat(_tmp78,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp796,_tmp795,_tag_fat(_tmp77,sizeof(void*),0U));});});}_LL0:;}
# 459
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp79=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp79;void*_tmp7A=_stmttmp6;void**_tmp7B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A)->tag == 2U){_LL1: _tmp7B=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A)->f1)->kind;_LL2: {void**cptr=_tmp7B;
# 462
void*_tmp7C=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp7=_tmp7C;void*_tmp7D=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7D)->tag == 1U){_LL6: _LL7:
# 464
({void*_tmp797=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=2U,_tmp7E->f1=0,_tmp7E->f2=k;_tmp7E;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp797;});
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 468
 return;}_LL0:;}
# 473
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 481
if(tms == 0)return 0;{
# 483
void*_tmp7F=(void*)tms->hd;void*_stmttmp8=_tmp7F;void*_tmp80=_stmttmp8;void*_tmp81;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp80)->tag == 3U){_LL1: _tmp81=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp80)->f1;_LL2: {void*args=_tmp81;
# 486
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 489
void*_tmp82=args;struct Cyc_List_List*_tmp83;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp82)->tag == 1U){_LL6: _LL7:
# 491
({void*_tmp84=0U;({unsigned _tmp799=loc;struct _fat_ptr _tmp798=({const char*_tmp85="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp85,sizeof(char),93U);});Cyc_Warn_warn(_tmp799,_tmp798,_tag_fat(_tmp84,sizeof(void*),0U));});});
# 493
return tms;}else{_LL8: _tmp83=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp82)->f1;_LL9: {struct Cyc_List_List*ids=_tmp83;
# 495
if(({int _tmp79A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp79A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp86=0U;({unsigned _tmp79C=loc;struct _fat_ptr _tmp79B=({const char*_tmp87="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp87,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79C,_tmp79B,_tag_fat(_tmp86,sizeof(void*),0U));});});{
# 499
struct Cyc_List_List*rev_new_params=0;
for(0;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(0;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp88=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp88;
void*_tmp89=x->r;void*_stmttmp9=_tmp89;void*_tmp8A=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp8B;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A)->tag == 0U){_LLB: _tmp8B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp8B;
# 506
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({void*_tmp8C=0U;({unsigned _tmp79E=x->loc;struct _fat_ptr _tmp79D=({const char*_tmp8D="initializer found in parameter declaration";_tag_fat(_tmp8D,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79E,_tmp79D,_tag_fat(_tmp8C,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp8E=0U;({unsigned _tmp7A0=x->loc;struct _fat_ptr _tmp79F=({const char*_tmp8F="namespaces forbidden in parameter declarations";_tag_fat(_tmp8F,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A0,_tmp79F,_tag_fat(_tmp8E,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp91=_cycalloc(sizeof(*_tmp91));({struct _tuple8*_tmp7A1=({struct _tuple8*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=(*vd->name).f2,_tmp90->f2=vd->tq,_tmp90->f3=vd->type;_tmp90;});_tmp91->hd=_tmp7A1;}),_tmp91->tl=rev_new_params;_tmp91;});
# 514
goto L;}}else{_LLD: _LLE:
({void*_tmp92=0U;({unsigned _tmp7A3=x->loc;struct _fat_ptr _tmp7A2=({const char*_tmp93="nonvariable declaration in parameter type";_tag_fat(_tmp93,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A3,_tmp7A2,_tag_fat(_tmp92,sizeof(void*),0U));});});}_LLA:;}
# 518
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp96=({struct Cyc_String_pa_PrintArg_struct _tmp6CE;_tmp6CE.tag=0U,_tmp6CE.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6CE;});void*_tmp94[1U];_tmp94[0]=& _tmp96;({unsigned _tmp7A5=loc;struct _fat_ptr _tmp7A4=({const char*_tmp95="%s is not given a type";_tag_fat(_tmp95,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A5,_tmp7A4,_tag_fat(_tmp94,sizeof(void*),1U));});});}
# 521
return({struct Cyc_List_List*_tmp99=_region_malloc(yy,sizeof(*_tmp99));
({void*_tmp7A8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp98=_region_malloc(yy,sizeof(*_tmp98));_tmp98->tag=3U,({void*_tmp7A7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp97=_region_malloc(yy,sizeof(*_tmp97));_tmp97->tag=1U,({struct Cyc_List_List*_tmp7A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp97->f1=_tmp7A6;}),_tmp97->f2=0,_tmp97->f3=0,_tmp97->f4=0,_tmp97->f5=0,_tmp97->f6=0,_tmp97->f7=0;_tmp97;});_tmp98->f1=_tmp7A7;});_tmp98;});_tmp99->hd=_tmp7A8;}),_tmp99->tl=0;_tmp99;});}}}_LL5:;}
# 528
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp9A=_region_malloc(yy,sizeof(*_tmp9A));_tmp9A->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7A9=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp9A->tl=_tmp7A9;});_tmp9A;});}_LL0:;}}
# 536
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 540
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6CF;_tmp6CF.id=d.id,_tmp6CF.varloc=d.varloc,({struct Cyc_List_List*_tmp7AA=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6CF.tms=_tmp7AA;});_tmp6CF;});{
# 543
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 549
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 555
enum Cyc_Parse_Storage_class _tmp9B=dso->sc;enum Cyc_Parse_Storage_class _stmttmpA=_tmp9B;enum Cyc_Parse_Storage_class _tmp9C=_stmttmpA;switch(_tmp9C){case Cyc_Parse_None_sc: _LL1: _LL2:
 goto _LL0;case Cyc_Parse_Extern_sc: _LL3: _LL4:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL5: _LL6:
 sc=0U;goto _LL0;default: _LL7: _LL8:
({void*_tmp9D=0U;({unsigned _tmp7AC=loc;struct _fat_ptr _tmp7AB=({const char*_tmp9E="bad storage class on function";_tag_fat(_tmp9E,sizeof(char),30U);});Cyc_Warn_err(_tmp7AC,_tmp7AB,_tag_fat(_tmp9D,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 562
void*_tmp9F=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp9F;
struct _tuple14 _tmpA0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmpA0;struct _tuple14 _tmpA1=_stmttmpB;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA4;void*_tmpA3;struct Cyc_Absyn_Tqual _tmpA2;_LLA: _tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmpA1.f4;_LLB: {struct Cyc_Absyn_Tqual fn_tqual=_tmpA2;void*fn_type=_tmpA3;struct Cyc_List_List*x=_tmpA4;struct Cyc_List_List*out_atts=_tmpA5;
# 567
if(x != 0)
({void*_tmpA6=0U;({unsigned _tmp7AE=loc;struct _fat_ptr _tmp7AD=({const char*_tmpA7="bad type params, ignoring";_tag_fat(_tmpA7,sizeof(char),26U);});Cyc_Warn_warn(_tmp7AE,_tmp7AD,_tag_fat(_tmpA6,sizeof(void*),0U));});});{
# 570
void*_tmpA8=fn_type;struct Cyc_Absyn_FnInfo _tmpA9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA8)->tag == 5U){_LLD: _tmpA9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA8)->f1;_LLE: {struct Cyc_Absyn_FnInfo i=_tmpA9;
# 572
{struct Cyc_List_List*_tmpAA=i.args;struct Cyc_List_List*args2=_tmpAA;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpAB=0U;({unsigned _tmp7B0=loc;struct _fat_ptr _tmp7AF=({const char*_tmpAC="missing argument variable in function prototype";_tag_fat(_tmpAC,sizeof(char),48U);});Cyc_Warn_err(_tmp7B0,_tmp7AF,_tag_fat(_tmpAB,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7B2=({struct _fat_ptr*_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct _fat_ptr _tmp7B1=({const char*_tmpAD="?";_tag_fat(_tmpAD,sizeof(char),2U);});*_tmpAE=_tmp7B1;});_tmpAE;});(*((struct _tuple8*)args2->hd)).f1=_tmp7B2;});}}}
# 579
({struct Cyc_List_List*_tmp7B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7B3;});
return({struct Cyc_Absyn_Fndecl*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->sc=sc,_tmpAF->is_inline=is_inline,_tmpAF->name=d.id,_tmpAF->body=body,_tmpAF->i=i,_tmpAF->cached_type=0,_tmpAF->param_vardecls=0,_tmpAF->fn_vardecl=0,_tmpAF->orig_scope=sc;_tmpAF;});}}else{_LLF: _LL10:
# 583
({void*_tmpB0=0U;({unsigned _tmp7B5=loc;struct _fat_ptr _tmp7B4=({const char*_tmpB1="declarator is not a function prototype";_tag_fat(_tmpB1,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7B5,_tmp7B4,_tag_fat(_tmpB0,sizeof(void*),0U));});});}_LLC:;}}}}}static char _tmpB2[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 587
static struct _fat_ptr Cyc_Parse_msg1={_tmpB2,_tmpB2,_tmpB2 + 76U};static char _tmpB3[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 589
static struct _fat_ptr Cyc_Parse_msg2={_tmpB3,_tmpB3,_tmpB3 + 84U};
# 596
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 599
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpB4=0U;({unsigned _tmp7B7=loc;struct _fat_ptr _tmp7B6=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7B7,_tmp7B6,_tag_fat(_tmpB4,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpB5=0U;({unsigned _tmp7B9=loc;struct _fat_ptr _tmp7B8=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7B9,_tmp7B8,_tag_fat(_tmpB5,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpB6=0U;({unsigned _tmp7BB=loc;struct _fat_ptr _tmp7BA=({const char*_tmpB7="too many occurrences of short in specifiers";_tag_fat(_tmpB7,sizeof(char),44U);});Cyc_Warn_warn(_tmp7BB,_tmp7BA,_tag_fat(_tmpB6,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpB8=0U;({unsigned _tmp7BD=loc;struct _fat_ptr _tmp7BC=({const char*_tmpB9="too many occurrences of long in specifiers";_tag_fat(_tmpB9,sizeof(char),43U);});Cyc_Warn_warn(_tmp7BD,_tmp7BC,_tag_fat(_tmpB8,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpBA=0U;({unsigned _tmp7BF=loc;struct _fat_ptr _tmp7BE=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7BF,_tmp7BE,_tag_fat(_tmpBA,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 621
return s1;}
# 627
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 635
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpBB=0U;({unsigned _tmp7C1=loc;struct _fat_ptr _tmp7C0=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7C1,_tmp7C0,_tag_fat(_tmpBB,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpBC=0U;({unsigned _tmp7C3=loc;struct _fat_ptr _tmp7C2=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7C3,_tmp7C2,_tag_fat(_tmpBC,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 648
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpBD=0U;({unsigned _tmp7C5=loc;struct _fat_ptr _tmp7C4=({const char*_tmpBE="missing type within specifier";_tag_fat(_tmpBE,sizeof(char),30U);});Cyc_Warn_warn(_tmp7C5,_tmp7C4,_tag_fat(_tmpBD,sizeof(void*),0U));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 653
{void*_tmpBF=t;enum Cyc_Absyn_Size_of _tmpC1;enum Cyc_Absyn_Sign _tmpC0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->f1)){case 1U: _LL1: _tmpC0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->f1)->f1;_tmpC1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpC0;enum Cyc_Absyn_Size_of sz2=_tmpC1;
# 655
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 659
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2U: _LL3: _LL4:
# 663
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
# 667
 if(seen_sign)
({void*_tmpC2=0U;({unsigned _tmp7C7=loc;struct _fat_ptr _tmp7C6=({const char*_tmpC3="sign specification on non-integral type";_tag_fat(_tmpC3,sizeof(char),40U);});Cyc_Warn_err(_tmp7C7,_tmp7C6,_tag_fat(_tmpC2,sizeof(void*),0U));});});
if(seen_size)
({void*_tmpC4=0U;({unsigned _tmp7C9=loc;struct _fat_ptr _tmp7C8=({const char*_tmpC5="size qualifier on non-integral type";_tag_fat(_tmpC5,sizeof(char),36U);});Cyc_Warn_err(_tmp7C9,_tmp7C8,_tag_fat(_tmpC4,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 673
return t;}
# 676
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 680
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpC6=d.id;struct _tuple0*q=_tmpC6;
unsigned _tmpC7=d.varloc;unsigned varloc=_tmpC7;
struct _tuple14 _tmpC8=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpC8;struct _tuple14 _tmpC9=_stmttmpC;struct Cyc_List_List*_tmpCD;struct Cyc_List_List*_tmpCC;void*_tmpCB;struct Cyc_Absyn_Tqual _tmpCA;_LL1: _tmpCA=_tmpC9.f1;_tmpCB=_tmpC9.f2;_tmpCC=_tmpC9.f3;_tmpCD=_tmpC9.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpCA;void*new_typ=_tmpCB;struct Cyc_List_List*tvs=_tmpCC;struct Cyc_List_List*atts=_tmpCD;
# 687
struct Cyc_List_List*_tmpCE=ds->tl == 0?0:({struct _RegionHandle*_tmp7CD=r;struct Cyc_Absyn_Tqual _tmp7CC=tq;void*_tmp7CB=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7CA=ds->tl;Cyc_Parse_apply_tmss(_tmp7CD,_tmp7CC,_tmp7CB,_tmp7CA,shared_atts);});struct Cyc_List_List*tl=_tmpCE;
return({struct Cyc_List_List*_tmpD0=_region_malloc(r,sizeof(*_tmpD0));({struct _tuple16*_tmp7CE=({struct _tuple16*_tmpCF=_region_malloc(r,sizeof(*_tmpCF));_tmpCF->f1=varloc,_tmpCF->f2=q,_tmpCF->f3=tq2,_tmpCF->f4=new_typ,_tmpCF->f5=tvs,_tmpCF->f6=atts;_tmpCF;});_tmpD0->hd=_tmp7CE;}),_tmpD0->tl=tl;_tmpD0;});}}}
# 691
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 694
if(tms == 0)return({struct _tuple14 _tmp6D0;_tmp6D0.f1=tq,_tmp6D0.f2=t,_tmp6D0.f3=0,_tmp6D0.f4=atts;_tmp6D0;});{
void*_tmpD1=(void*)tms->hd;void*_stmttmpD=_tmpD1;void*_tmpD2=_stmttmpD;struct Cyc_List_List*_tmpD4;unsigned _tmpD3;struct Cyc_Absyn_Tqual _tmpD6;struct Cyc_Absyn_PtrAtts _tmpD5;unsigned _tmpD8;struct Cyc_List_List*_tmpD7;void*_tmpD9;unsigned _tmpDC;void*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;unsigned _tmpDE;void*_tmpDD;switch(*((int*)_tmpD2)){case 0U: _LL1: _tmpDD=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD2)->f1;_tmpDE=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD2)->f2;_LL2: {void*zeroterm=_tmpDD;unsigned ztloc=_tmpDE;
# 697
return({struct Cyc_Absyn_Tqual _tmp7D1=Cyc_Absyn_empty_tqual(0U);void*_tmp7D0=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 697
struct Cyc_List_List*_tmp7CF=atts;Cyc_Parse_apply_tms(_tmp7D1,_tmp7D0,_tmp7CF,tms->tl);});}case 1U: _LL3: _tmpDA=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD2)->f1;_tmpDB=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD2)->f2;_tmpDC=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD2)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpDA;void*zeroterm=_tmpDB;unsigned ztloc=_tmpDC;
# 700
return({struct Cyc_Absyn_Tqual _tmp7D4=Cyc_Absyn_empty_tqual(0U);void*_tmp7D3=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 700
struct Cyc_List_List*_tmp7D2=atts;Cyc_Parse_apply_tms(_tmp7D4,_tmp7D3,_tmp7D2,tms->tl);});}case 3U: _LL5: _tmpD9=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD2)->f1;_LL6: {void*args=_tmpD9;
# 703
void*_tmpDF=args;unsigned _tmpE0;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_List_List*_tmpE5;void*_tmpE4;struct Cyc_Absyn_VarargInfo*_tmpE3;int _tmpE2;struct Cyc_List_List*_tmpE1;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->tag == 1U){_LLE: _tmpE1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f3;_tmpE4=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f5;_tmpE6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f6;_tmpE7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDF)->f7;_LLF: {struct Cyc_List_List*args2=_tmpE1;int c_vararg=_tmpE2;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpE3;void*eff=_tmpE4;struct Cyc_List_List*rgn_po=_tmpE5;struct Cyc_Absyn_Exp*req=_tmpE6;struct Cyc_Absyn_Exp*ens=_tmpE7;
# 705
struct Cyc_List_List*typvars=0;
# 707
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(void*)as->hd,_tmpE8->tl=fn_atts;_tmpE8;});else{
# 712
new_atts=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=(void*)as->hd,_tmpE9->tl=new_atts;_tmpE9;});}}}
# 714
if(tms->tl != 0){
void*_tmpEA=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpEA;void*_tmpEB=_stmttmpE;struct Cyc_List_List*_tmpEC;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEB)->tag == 4U){_LL13: _tmpEC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEB)->f1;_LL14: {struct Cyc_List_List*ts=_tmpEC;
# 717
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;}
# 723
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 728
args2=0;{
# 731
struct Cyc_List_List*_tmpED=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpED;
# 733
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 738
{struct Cyc_List_List*_tmpEE=args2;struct Cyc_List_List*a=_tmpEE;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpEF=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpEF;struct _tuple8*_tmpF0=_stmttmpF;void**_tmpF3;struct Cyc_Absyn_Tqual _tmpF2;struct _fat_ptr*_tmpF1;_LL18: _tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=(void**)& _tmpF0->f3;_LL19: {struct _fat_ptr*vopt=_tmpF1;struct Cyc_Absyn_Tqual tq=_tmpF2;void**t=_tmpF3;
if(tags != 0)
({void*_tmp7D5=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7D5;});
({void*_tmp7D6=Cyc_Parse_array2ptr(*t,1);*t=_tmp7D6;});}}}
# 750
return({struct Cyc_Absyn_Tqual _tmp7D9=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7D8=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 750
struct Cyc_List_List*_tmp7D7=new_atts;Cyc_Parse_apply_tms(_tmp7D9,_tmp7D8,_tmp7D7,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpE0=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpDF)->f2;_LL11: {unsigned loc=_tmpE0;
# 757
({void*_tmpF4=0U;({unsigned _tmp7DB=loc;struct _fat_ptr _tmp7DA=({const char*_tmpF5="function declaration without parameter types";_tag_fat(_tmpF5,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7DB,_tmp7DA,_tag_fat(_tmpF4,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD2)->f1;_tmpD8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD2)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD7;unsigned loc=_tmpD8;
# 764
if(tms->tl == 0)
return({struct _tuple14 _tmp6D1;_tmp6D1.f1=tq,_tmp6D1.f2=t,_tmp6D1.f3=ts,_tmp6D1.f4=atts;_tmp6D1;});
# 769
({void*_tmpF6=0U;({unsigned _tmp7DD=loc;struct _fat_ptr _tmp7DC=({const char*_tmpF7="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF7,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7DD,_tmp7DC,_tag_fat(_tmpF6,sizeof(void*),0U));});});}case 2U: _LL9: _tmpD5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD2)->f1;_tmpD6=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD2)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpD5;struct Cyc_Absyn_Tqual tq2=_tmpD6;
# 772
return({struct Cyc_Absyn_Tqual _tmp7E0=tq2;void*_tmp7DF=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D2;_tmp6D2.elt_type=t,_tmp6D2.elt_tq=tq,_tmp6D2.ptr_atts=ptratts;_tmp6D2;}));struct Cyc_List_List*_tmp7DE=atts;Cyc_Parse_apply_tms(_tmp7E0,_tmp7DF,_tmp7DE,tms->tl);});}default: _LLB: _tmpD3=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD2)->f1;_tmpD4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD2)->f2;_LLC: {unsigned loc=_tmpD3;struct Cyc_List_List*atts2=_tmpD4;
# 777
return({struct Cyc_Absyn_Tqual _tmp7E3=tq;void*_tmp7E2=t;struct Cyc_List_List*_tmp7E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7E3,_tmp7E2,_tmp7E1,tms->tl);});}}_LL0:;}}
# 783
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 792
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpF8=t;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;void*_tmpFC;struct Cyc_Absyn_Tqual _tmpFB;struct _tuple0*_tmpFA;unsigned _tmpF9;_LL1: _tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;_tmpFB=_tmpF8->f3;_tmpFC=_tmpF8->f4;_tmpFD=_tmpF8->f5;_tmpFE=_tmpF8->f6;_LL2: {unsigned varloc=_tmpF9;struct _tuple0*x=_tmpFA;struct Cyc_Absyn_Tqual tq=_tmpFB;void*typ=_tmpFC;struct Cyc_List_List*tvs=_tmpFD;struct Cyc_List_List*atts=_tmpFE;
# 795
Cyc_Lex_register_typedef(x);{
# 797
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpFF=typ;struct Cyc_Core_Opt*_tmp100;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFF)->tag == 1U){_LL4: _tmp100=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFF)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmp100;
# 801
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 806
return({void*_tmp7E5=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7E4=({struct Cyc_Absyn_Typedefdecl*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->name=x,_tmp101->tvs=tvs,_tmp101->kind=kind,_tmp101->defn=type,_tmp101->atts=atts,_tmp101->tq=tq,_tmp101->extern_c=0;_tmp101;});_tmp102->f1=_tmp7E4;});_tmp102;});Cyc_Absyn_new_decl(_tmp7E5,loc);});}}}
# 813
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7E6=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=12U,_tmp103->f1=d,_tmp103->f2=s;_tmp103;});Cyc_Absyn_new_stmt(_tmp7E6,d->loc);});}
# 817
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 826
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 829
struct _RegionHandle _tmp104=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp104;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp105=ds;struct Cyc_List_List*_tmp108;struct Cyc_Parse_Type_specifier _tmp107;struct Cyc_Absyn_Tqual _tmp106;_LL1: _tmp106=_tmp105.tq;_tmp107=_tmp105.type_specs;_tmp108=_tmp105.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp106;struct Cyc_Parse_Type_specifier tss=_tmp107;struct Cyc_List_List*atts=_tmp108;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp109=0U;({unsigned _tmp7E8=loc;struct _fat_ptr _tmp7E7=({const char*_tmp10A="inline qualifier on non-function definition";_tag_fat(_tmp10A,sizeof(char),44U);});Cyc_Warn_warn(_tmp7E8,_tmp7E7,_tag_fat(_tmp109,sizeof(void*),0U));});});{
# 835
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmp10B=ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp10B;enum Cyc_Parse_Storage_class _tmp10C=_stmttmp10;switch(_tmp10C){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 s=Cyc_Flags_no_register?Cyc_Absyn_Public: Cyc_Absyn_Register;goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 s=1U;goto _LL3;default: _LL12: _LL13:
 goto _LL3;}_LL3:;}{
# 851
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 855
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 863
void*_tmp10D=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp10D;
if(declarators == 0){
# 867
void*_tmp10E=base_type;struct Cyc_List_List*_tmp10F;struct _tuple0*_tmp110;struct Cyc_List_List*_tmp113;int _tmp112;struct _tuple0*_tmp111;struct Cyc_Absyn_Datatypedecl**_tmp114;struct Cyc_List_List*_tmp117;struct _tuple0*_tmp116;enum Cyc_Absyn_AggrKind _tmp115;struct Cyc_Absyn_Datatypedecl*_tmp118;struct Cyc_Absyn_Enumdecl*_tmp119;struct Cyc_Absyn_Aggrdecl*_tmp11A;switch(*((int*)_tmp10E)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10E)->f1)->r)){case 0U: _LL15: _tmp11A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10E)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp11A;
# 869
({struct Cyc_List_List*_tmp7E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7E9;});
ad->sc=s;{
struct Cyc_List_List*_tmp11D=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_Absyn_Decl*_tmp7EB=({void*_tmp7EA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=5U,_tmp11B->f1=ad;_tmp11B;});Cyc_Absyn_new_decl(_tmp7EA,loc);});_tmp11C->hd=_tmp7EB;}),_tmp11C->tl=0;_tmp11C;});_npop_handler(0U);return _tmp11D;}}case 1U: _LL17: _tmp119=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10E)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp119;
# 873
if(atts != 0)({void*_tmp11E=0U;({unsigned _tmp7ED=loc;struct _fat_ptr _tmp7EC=({const char*_tmp11F="attributes on enum not supported";_tag_fat(_tmp11F,sizeof(char),33U);});Cyc_Warn_err(_tmp7ED,_tmp7EC,_tag_fat(_tmp11E,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp122=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Absyn_Decl*_tmp7EF=({void*_tmp7EE=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->tag=7U,_tmp120->f1=ed;_tmp120;});Cyc_Absyn_new_decl(_tmp7EE,loc);});_tmp121->hd=_tmp7EF;}),_tmp121->tl=0;_tmp121;});_npop_handler(0U);return _tmp122;}}default: _LL19: _tmp118=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10E)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp118;
# 877
if(atts != 0)({void*_tmp123=0U;({unsigned _tmp7F1=loc;struct _fat_ptr _tmp7F0=({const char*_tmp124="attributes on datatypes not supported";_tag_fat(_tmp124,sizeof(char),38U);});Cyc_Warn_err(_tmp7F1,_tmp7F0,_tag_fat(_tmp123,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp127=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));({struct Cyc_Absyn_Decl*_tmp7F3=({void*_tmp7F2=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=6U,_tmp125->f1=dd;_tmp125;});Cyc_Absyn_new_decl(_tmp7F2,loc);});_tmp126->hd=_tmp7F3;}),_tmp126->tl=0;_tmp126;});_npop_handler(0U);return _tmp127;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp115=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).UnknownAggr).val).f1;_tmp116=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).UnknownAggr).val).f2;_tmp117=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp115;struct _tuple0*n=_tmp116;struct Cyc_List_List*ts=_tmp117;
# 881
struct Cyc_List_List*_tmp128=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp128;
struct Cyc_Absyn_Aggrdecl*_tmp129=({struct Cyc_Absyn_Aggrdecl*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->kind=k,_tmp12F->sc=s,_tmp12F->name=n,_tmp12F->tvs=ts2,_tmp12F->impl=0,_tmp12F->attributes=0,_tmp12F->expected_mem_kind=0;_tmp12F;});struct Cyc_Absyn_Aggrdecl*ad=_tmp129;
if(atts != 0)({void*_tmp12A=0U;({unsigned _tmp7F5=loc;struct _fat_ptr _tmp7F4=({const char*_tmp12B="bad attributes on type declaration";_tag_fat(_tmp12B,sizeof(char),35U);});Cyc_Warn_err(_tmp7F5,_tmp7F4,_tag_fat(_tmp12A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12E=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct Cyc_Absyn_Decl*_tmp7F7=({void*_tmp7F6=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=5U,_tmp12C->f1=ad;_tmp12C;});Cyc_Absyn_new_decl(_tmp7F6,loc);});_tmp12D->hd=_tmp7F7;}),_tmp12D->tl=0;_tmp12D;});_npop_handler(0U);return _tmp12E;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp114=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp114;
# 886
if(atts != 0)({void*_tmp130=0U;({unsigned _tmp7F9=loc;struct _fat_ptr _tmp7F8=({const char*_tmp131="bad attributes on datatype";_tag_fat(_tmp131,sizeof(char),27U);});Cyc_Warn_err(_tmp7F9,_tmp7F8,_tag_fat(_tmp130,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp134=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_Absyn_Decl*_tmp7FB=({void*_tmp7FA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=6U,_tmp132->f1=*tudp;_tmp132;});Cyc_Absyn_new_decl(_tmp7FA,loc);});_tmp133->hd=_tmp7FB;}),_tmp133->tl=0;_tmp133;});_npop_handler(0U);return _tmp134;}}}else{_LL1F: _tmp111=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).UnknownDatatype).val).name;_tmp112=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp113=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2;_LL20: {struct _tuple0*n=_tmp111;int isx=_tmp112;struct Cyc_List_List*ts=_tmp113;
# 889
struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp135;
struct Cyc_Absyn_Decl*_tmp136=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp136;
if(atts != 0)({void*_tmp137=0U;({unsigned _tmp7FD=loc;struct _fat_ptr _tmp7FC=({const char*_tmp138="bad attributes on datatype";_tag_fat(_tmp138,sizeof(char),27U);});Cyc_Warn_err(_tmp7FD,_tmp7FC,_tag_fat(_tmp137,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=tud,_tmp139->tl=0;_tmp139;});_npop_handler(0U);return _tmp13A;}}}case 15U: _LL21: _tmp110=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1;_LL22: {struct _tuple0*n=_tmp110;
# 894
struct Cyc_Absyn_Enumdecl*_tmp13B=({struct Cyc_Absyn_Enumdecl*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->sc=s,_tmp142->name=n,_tmp142->fields=0;_tmp142;});struct Cyc_Absyn_Enumdecl*ed=_tmp13B;
if(atts != 0)({void*_tmp13C=0U;({unsigned _tmp7FF=loc;struct _fat_ptr _tmp7FE=({const char*_tmp13D="bad attributes on enum";_tag_fat(_tmp13D,sizeof(char),23U);});Cyc_Warn_err(_tmp7FF,_tmp7FE,_tag_fat(_tmp13C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_Absyn_Decl*_tmp801=({struct Cyc_Absyn_Decl*_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp800=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->tag=7U,_tmp13E->f1=ed;_tmp13E;});_tmp13F->r=_tmp800;}),_tmp13F->loc=loc;_tmp13F;});_tmp140->hd=_tmp801;}),_tmp140->tl=0;_tmp140;});_npop_handler(0U);return _tmp141;}}case 16U: _LL23: _tmp10F=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp10F;
# 900
struct Cyc_Absyn_Enumdecl*_tmp143=({struct Cyc_Absyn_Enumdecl*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->sc=s,({struct _tuple0*_tmp803=Cyc_Parse_gensym_enum();_tmp14B->name=_tmp803;}),({struct Cyc_Core_Opt*_tmp802=({struct Cyc_Core_Opt*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->v=fs;_tmp14A;});_tmp14B->fields=_tmp802;});_tmp14B;});struct Cyc_Absyn_Enumdecl*ed=_tmp143;
if(atts != 0)({void*_tmp144=0U;({unsigned _tmp805=loc;struct _fat_ptr _tmp804=({const char*_tmp145="bad attributes on enum";_tag_fat(_tmp145,sizeof(char),23U);});Cyc_Warn_err(_tmp805,_tmp804,_tag_fat(_tmp144,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Decl*_tmp807=({struct Cyc_Absyn_Decl*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp806=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=7U,_tmp146->f1=ed;_tmp146;});_tmp147->r=_tmp806;}),_tmp147->loc=loc;_tmp147;});_tmp148->hd=_tmp807;}),_tmp148->tl=0;_tmp148;});_npop_handler(0U);return _tmp149;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp14C=0U;({unsigned _tmp809=loc;struct _fat_ptr _tmp808=({const char*_tmp14D="missing declarator";_tag_fat(_tmp14D,sizeof(char),19U);});Cyc_Warn_err(_tmp809,_tmp808,_tag_fat(_tmp14C,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp14E=0;_npop_handler(0U);return _tmp14E;}}_LL14:;}{
# 907
struct Cyc_List_List*_tmp14F=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp14F;
if(istypedef){
# 912
if(!exps_empty)
({void*_tmp150=0U;({unsigned _tmp80B=loc;struct _fat_ptr _tmp80A=({const char*_tmp151="initializer in typedef declaration";_tag_fat(_tmp151,sizeof(char),35U);});Cyc_Warn_err(_tmp80B,_tmp80A,_tag_fat(_tmp150,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp152=decls;_npop_handler(0U);return _tmp152;}}{
# 918
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp153=fields;struct Cyc_List_List*ds=_tmp153;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp154=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp154;struct _tuple16*_tmp155=_stmttmp11;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15A;void*_tmp159;struct Cyc_Absyn_Tqual _tmp158;struct _tuple0*_tmp157;unsigned _tmp156;_LL28: _tmp156=_tmp155->f1;_tmp157=_tmp155->f2;_tmp158=_tmp155->f3;_tmp159=_tmp155->f4;_tmp15A=_tmp155->f5;_tmp15B=_tmp155->f6;_LL29: {unsigned varloc=_tmp156;struct _tuple0*x=_tmp157;struct Cyc_Absyn_Tqual tq2=_tmp158;void*t2=_tmp159;struct Cyc_List_List*tvs2=_tmp15A;struct Cyc_List_List*atts2=_tmp15B;
if(tvs2 != 0)
({void*_tmp15C=0U;({unsigned _tmp80D=loc;struct _fat_ptr _tmp80C=({const char*_tmp15D="bad type params, ignoring";_tag_fat(_tmp15D,sizeof(char),26U);});Cyc_Warn_warn(_tmp80D,_tmp80C,_tag_fat(_tmp15C,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp15E=0U;({unsigned _tmp80F=loc;struct _fat_ptr _tmp80E=({const char*_tmp15F="unexpected NULL in parse!";_tag_fat(_tmp15F,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp80F,_tmp80E,_tag_fat(_tmp15E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp160=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp160;
struct Cyc_Absyn_Vardecl*_tmp161=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp161;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp162=({struct Cyc_Absyn_Decl*_tmp165=_cycalloc(sizeof(*_tmp165));({void*_tmp810=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=0U,_tmp164->f1=vd;_tmp164;});_tmp165->r=_tmp810;}),_tmp165->loc=loc;_tmp165;});struct Cyc_Absyn_Decl*d=_tmp162;
decls=({struct Cyc_List_List*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->hd=d,_tmp163->tl=decls;_tmp163;});}}}}}{
# 933
struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp166;}}}}}}}}}
# 830
;_pop_region();}
# 937
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp167=e->r;void*_stmttmp12=_tmp167;void*_tmp168=_stmttmp12;int _tmp169;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp168)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp168)->f1).Int_c).tag == 5){_LL1: _tmp169=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp168)->f1).Int_c).val).f2;_LL2: {int i=_tmp169;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 941
({void*_tmp16A=0U;({unsigned _tmp812=loc;struct _fat_ptr _tmp811=({const char*_tmp16B="expecting integer constant";_tag_fat(_tmp16B,sizeof(char),27U);});Cyc_Warn_err(_tmp812,_tmp811,_tag_fat(_tmp16A,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 947
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16C=e->r;void*_stmttmp13=_tmp16C;void*_tmp16D=_stmttmp13;struct _fat_ptr _tmp16E;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16D)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16D)->f1).String_c).tag == 8){_LL1: _tmp16E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16D)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp16E;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 951
({void*_tmp16F=0U;({unsigned _tmp814=loc;struct _fat_ptr _tmp813=({const char*_tmp170="expecting string constant";_tag_fat(_tmp170,sizeof(char),26U);});Cyc_Warn_err(_tmp814,_tmp813,_tag_fat(_tmp16F,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 957
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp171=x;long long _tmp172;char _tmp173;int _tmp174;switch((_tmp171.LongLong_c).tag){case 5U: _LL1: _tmp174=((_tmp171.Int_c).val).f2;_LL2: {int i=_tmp174;
return(unsigned)i;}case 2U: _LL3: _tmp173=((_tmp171.Char_c).val).f2;_LL4: {char c=_tmp173;
return(unsigned)c;}case 6U: _LL5: _tmp172=((_tmp171.LongLong_c).val).f2;_LL6: {long long x=_tmp172;
# 962
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp175=0U;({unsigned _tmp816=loc;struct _fat_ptr _tmp815=({const char*_tmp176="integer constant too large";_tag_fat(_tmp176,sizeof(char),27U);});Cyc_Warn_err(_tmp816,_tmp815,_tag_fat(_tmp175,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 967
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp6D3;_tmp6D3.tag=0U,({struct _fat_ptr _tmp817=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6D3.f1=_tmp817;});_tmp6D3;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({unsigned _tmp819=loc;struct _fat_ptr _tmp818=({const char*_tmp178="expected integer constant but found %s";_tag_fat(_tmp178,sizeof(char),39U);});Cyc_Warn_err(_tmp819,_tmp818,_tag_fat(_tmp177,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 973
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17A=p->r;void*_stmttmp14=_tmp17A;void*_tmp17B=_stmttmp14;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_List_List*_tmp17E;struct _tuple0*_tmp17D;int _tmp180;struct _fat_ptr _tmp17F;char _tmp181;int _tmp183;enum Cyc_Absyn_Sign _tmp182;struct Cyc_Absyn_Pat*_tmp184;struct Cyc_Absyn_Vardecl*_tmp185;struct _tuple0*_tmp186;switch(*((int*)_tmp17B)){case 15U: _LL1: _tmp186=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_LL2: {struct _tuple0*x=_tmp186;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17B)->f2)->r)->tag == 0U){_LL3: _tmp185=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp185;
# 977
return({struct Cyc_Absyn_Exp*_tmp81A=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp81A,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp184=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp184;
return({struct Cyc_Absyn_Exp*_tmp81B=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp81B,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp182=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_tmp183=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17B)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp182;int i=_tmp183;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp181=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_LLC: {char c=_tmp181;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp17F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_tmp180=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17B)->f2;_LLE: {struct _fat_ptr s=_tmp17F;int i=_tmp180;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17B)->f3 == 0){_LLF: _tmp17D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_tmp17E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17B)->f2;_LL10: {struct _tuple0*x=_tmp17D;struct Cyc_List_List*ps=_tmp17E;
# 984
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp17C=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17B)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp17C;
return e;}default: _LL13: _LL14:
# 989
({void*_tmp187=0U;({unsigned _tmp81D=p->loc;struct _fat_ptr _tmp81C=({const char*_tmp188="cannot mix patterns and expressions in case";_tag_fat(_tmp188,sizeof(char),44U);});Cyc_Warn_err(_tmp81D,_tmp81C,_tag_fat(_tmp187,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1074
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->tag=Cyc_Core_Failure,_tmp189->f1=s;_tmp189;}));}static char _tmp18C[7U]="cnst_t";
# 1046 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18C,_tmp18C,_tmp18C + 7U};
union Cyc_YYSTYPE*_tmp18A=yy1;union Cyc_Absyn_Cnst _tmp18B;if((((union Cyc_YYSTYPE*)_tmp18A)->Int_tok).tag == 1){_LL1: _tmp18B=(_tmp18A->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp18B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1053
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6D4;(_tmp6D4.Int_tok).tag=1U,(_tmp6D4.Int_tok).val=yy1;_tmp6D4;});}static char _tmp18F[5U]="char";
# 1047 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18F,_tmp18F,_tmp18F + 5U};
union Cyc_YYSTYPE*_tmp18D=yy1;char _tmp18E;if((((union Cyc_YYSTYPE*)_tmp18D)->Char_tok).tag == 2){_LL1: _tmp18E=(_tmp18D->Char_tok).val;_LL2: {char yy=_tmp18E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1054
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6D5;(_tmp6D5.Char_tok).tag=2U,(_tmp6D5.Char_tok).val=yy1;_tmp6D5;});}static char _tmp192[13U]="string_t<`H>";
# 1048 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp192,_tmp192,_tmp192 + 13U};
union Cyc_YYSTYPE*_tmp190=yy1;struct _fat_ptr _tmp191;if((((union Cyc_YYSTYPE*)_tmp190)->String_tok).tag == 3){_LL1: _tmp191=(_tmp190->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp191;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1055
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6D6;(_tmp6D6.String_tok).tag=3U,(_tmp6D6.String_tok).val=yy1;_tmp6D6;});}static char _tmp195[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1051 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp195,_tmp195,_tmp195 + 35U};
union Cyc_YYSTYPE*_tmp193=yy1;struct _tuple22*_tmp194;if((((union Cyc_YYSTYPE*)_tmp193)->YY1).tag == 9){_LL1: _tmp194=(_tmp193->YY1).val;_LL2: {struct _tuple22*yy=_tmp194;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1058
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.YY1).tag=9U,(_tmp6D7.YY1).val=yy1;_tmp6D7;});}static char _tmp198[11U]="ptrbound_t";
# 1052 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp198,_tmp198,_tmp198 + 11U};
union Cyc_YYSTYPE*_tmp196=yy1;void*_tmp197;if((((union Cyc_YYSTYPE*)_tmp196)->YY2).tag == 10){_LL1: _tmp197=(_tmp196->YY2).val;_LL2: {void*yy=_tmp197;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1059
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.YY2).tag=10U,(_tmp6D8.YY2).val=yy1;_tmp6D8;});}static char _tmp19B[28U]="list_t<offsetof_field_t,`H>";
# 1053 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19B,_tmp19B,_tmp19B + 28U};
union Cyc_YYSTYPE*_tmp199=yy1;struct Cyc_List_List*_tmp19A;if((((union Cyc_YYSTYPE*)_tmp199)->YY3).tag == 11){_LL1: _tmp19A=(_tmp199->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp19A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1060
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.YY3).tag=11U,(_tmp6D9.YY3).val=yy1;_tmp6D9;});}static char _tmp19E[6U]="exp_t";
# 1054 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19E,_tmp19E,_tmp19E + 6U};
union Cyc_YYSTYPE*_tmp19C=yy1;struct Cyc_Absyn_Exp*_tmp19D;if((((union Cyc_YYSTYPE*)_tmp19C)->Exp_tok).tag == 7){_LL1: _tmp19D=(_tmp19C->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp19D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1061
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.Exp_tok).tag=7U,(_tmp6DA.Exp_tok).val=yy1;_tmp6DA;});}static char _tmp1A1[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A1,_tmp1A1,_tmp1A1 + 17U};
union Cyc_YYSTYPE*_tmp19F=yy1;struct Cyc_List_List*_tmp1A0;if((((union Cyc_YYSTYPE*)_tmp19F)->YY4).tag == 12){_LL1: _tmp1A0=(_tmp19F->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp1A0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1069
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY4).tag=12U,(_tmp6DB.YY4).val=yy1;_tmp6DB;});}static char _tmp1A4[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1063 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A4,_tmp1A4,_tmp1A4 + 47U};
union Cyc_YYSTYPE*_tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;if((((union Cyc_YYSTYPE*)_tmp1A2)->YY5).tag == 13){_LL1: _tmp1A3=(_tmp1A2->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1A3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1070
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY5).tag=13U,(_tmp6DC.YY5).val=yy1;_tmp6DC;});}static char _tmp1A7[9U]="primop_t";
# 1064 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A7,_tmp1A7,_tmp1A7 + 9U};
union Cyc_YYSTYPE*_tmp1A5=yy1;enum Cyc_Absyn_Primop _tmp1A6;if((((union Cyc_YYSTYPE*)_tmp1A5)->YY6).tag == 14){_LL1: _tmp1A6=(_tmp1A5->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1A6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1071
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY6).tag=14U,(_tmp6DD.YY6).val=yy1;_tmp6DD;});}static char _tmp1AA[19U]="opt_t<primop_t,`H>";
# 1065 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AA,_tmp1AA,_tmp1AA + 19U};
union Cyc_YYSTYPE*_tmp1A8=yy1;struct Cyc_Core_Opt*_tmp1A9;if((((union Cyc_YYSTYPE*)_tmp1A8)->YY7).tag == 15){_LL1: _tmp1A9=(_tmp1A8->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1A9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1072
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY7).tag=15U,(_tmp6DE.YY7).val=yy1;_tmp6DE;});}static char _tmp1AD[7U]="qvar_t";
# 1066 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AD,_tmp1AD,_tmp1AD + 7U};
union Cyc_YYSTYPE*_tmp1AB=yy1;struct _tuple0*_tmp1AC;if((((union Cyc_YYSTYPE*)_tmp1AB)->QualId_tok).tag == 5){_LL1: _tmp1AC=(_tmp1AB->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1AC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1073
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.QualId_tok).tag=5U,(_tmp6DF.QualId_tok).val=yy1;_tmp6DF;});}static char _tmp1B0[7U]="stmt_t";
# 1069 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B0,_tmp1B0,_tmp1B0 + 7U};
union Cyc_YYSTYPE*_tmp1AE=yy1;struct Cyc_Absyn_Stmt*_tmp1AF;if((((union Cyc_YYSTYPE*)_tmp1AE)->Stmt_tok).tag == 8){_LL1: _tmp1AF=(_tmp1AE->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1AF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1076
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.Stmt_tok).tag=8U,(_tmp6E0.Stmt_tok).val=yy1;_tmp6E0;});}static char _tmp1B3[27U]="list_t<switch_clause_t,`H>";
# 1073 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B3,_tmp1B3,_tmp1B3 + 27U};
union Cyc_YYSTYPE*_tmp1B1=yy1;struct Cyc_List_List*_tmp1B2;if((((union Cyc_YYSTYPE*)_tmp1B1)->YY8).tag == 16){_LL1: _tmp1B2=(_tmp1B1->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1B2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1080
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY8).tag=16U,(_tmp6E1.YY8).val=yy1;_tmp6E1;});}static char _tmp1B6[6U]="pat_t";
# 1074 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B6,_tmp1B6,_tmp1B6 + 6U};
union Cyc_YYSTYPE*_tmp1B4=yy1;struct Cyc_Absyn_Pat*_tmp1B5;if((((union Cyc_YYSTYPE*)_tmp1B4)->YY9).tag == 17){_LL1: _tmp1B5=(_tmp1B4->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1B5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1081
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY9).tag=17U,(_tmp6E2.YY9).val=yy1;_tmp6E2;});}static char _tmp1B9[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1079 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B9,_tmp1B9,_tmp1B9 + 28U};
union Cyc_YYSTYPE*_tmp1B7=yy1;struct _tuple23*_tmp1B8;if((((union Cyc_YYSTYPE*)_tmp1B7)->YY10).tag == 18){_LL1: _tmp1B8=(_tmp1B7->YY10).val;_LL2: {struct _tuple23*yy=_tmp1B8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1086
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY10).tag=18U,(_tmp6E3.YY10).val=yy1;_tmp6E3;});}static char _tmp1BC[17U]="list_t<pat_t,`H>";
# 1080 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BC,_tmp1BC,_tmp1BC + 17U};
union Cyc_YYSTYPE*_tmp1BA=yy1;struct Cyc_List_List*_tmp1BB;if((((union Cyc_YYSTYPE*)_tmp1BA)->YY11).tag == 19){_LL1: _tmp1BB=(_tmp1BA->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1BB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1087
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY11).tag=19U,(_tmp6E4.YY11).val=yy1;_tmp6E4;});}static char _tmp1BF[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1081 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BF,_tmp1BF,_tmp1BF + 36U};
union Cyc_YYSTYPE*_tmp1BD=yy1;struct _tuple24*_tmp1BE;if((((union Cyc_YYSTYPE*)_tmp1BD)->YY12).tag == 20){_LL1: _tmp1BE=(_tmp1BD->YY12).val;_LL2: {struct _tuple24*yy=_tmp1BE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1088
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY12).tag=20U,(_tmp6E5.YY12).val=yy1;_tmp6E5;});}static char _tmp1C2[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1082 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C2,_tmp1C2,_tmp1C2 + 47U};
union Cyc_YYSTYPE*_tmp1C0=yy1;struct Cyc_List_List*_tmp1C1;if((((union Cyc_YYSTYPE*)_tmp1C0)->YY13).tag == 21){_LL1: _tmp1C1=(_tmp1C0->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1C1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1089
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY13).tag=21U,(_tmp6E6.YY13).val=yy1;_tmp6E6;});}static char _tmp1C5[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1083 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C5,_tmp1C5,_tmp1C5 + 58U};
union Cyc_YYSTYPE*_tmp1C3=yy1;struct _tuple23*_tmp1C4;if((((union Cyc_YYSTYPE*)_tmp1C3)->YY14).tag == 22){_LL1: _tmp1C4=(_tmp1C3->YY14).val;_LL2: {struct _tuple23*yy=_tmp1C4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1090
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY14).tag=22U,(_tmp6E7.YY14).val=yy1;_tmp6E7;});}static char _tmp1C8[9U]="fndecl_t";
# 1084 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C8,_tmp1C8,_tmp1C8 + 9U};
union Cyc_YYSTYPE*_tmp1C6=yy1;struct Cyc_Absyn_Fndecl*_tmp1C7;if((((union Cyc_YYSTYPE*)_tmp1C6)->YY15).tag == 23){_LL1: _tmp1C7=(_tmp1C6->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1C7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1091
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY15).tag=23U,(_tmp6E8.YY15).val=yy1;_tmp6E8;});}static char _tmp1CB[18U]="list_t<decl_t,`H>";
# 1085 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CB,_tmp1CB,_tmp1CB + 18U};
union Cyc_YYSTYPE*_tmp1C9=yy1;struct Cyc_List_List*_tmp1CA;if((((union Cyc_YYSTYPE*)_tmp1C9)->YY16).tag == 24){_LL1: _tmp1CA=(_tmp1C9->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1CA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1092
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY16).tag=24U,(_tmp6E9.YY16).val=yy1;_tmp6E9;});}static char _tmp1CE[12U]="decl_spec_t";
# 1088 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CE,_tmp1CE,_tmp1CE + 12U};
union Cyc_YYSTYPE*_tmp1CC=yy1;struct Cyc_Parse_Declaration_spec _tmp1CD;if((((union Cyc_YYSTYPE*)_tmp1CC)->YY17).tag == 25){_LL1: _tmp1CD=(_tmp1CC->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1CD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1095
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY17).tag=25U,(_tmp6EA.YY17).val=yy1;_tmp6EA;});}static char _tmp1D1[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1089 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D1,_tmp1D1,_tmp1D1 + 31U};
union Cyc_YYSTYPE*_tmp1CF=yy1;struct _tuple12 _tmp1D0;if((((union Cyc_YYSTYPE*)_tmp1CF)->YY18).tag == 26){_LL1: _tmp1D0=(_tmp1CF->YY18).val;_LL2: {struct _tuple12 yy=_tmp1D0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1096
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY18).tag=26U,(_tmp6EB.YY18).val=yy1;_tmp6EB;});}static char _tmp1D4[23U]="declarator_list_t<`yy>";
# 1090 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D4,_tmp1D4,_tmp1D4 + 23U};
union Cyc_YYSTYPE*_tmp1D2=yy1;struct _tuple13*_tmp1D3;if((((union Cyc_YYSTYPE*)_tmp1D2)->YY19).tag == 27){_LL1: _tmp1D3=(_tmp1D2->YY19).val;_LL2: {struct _tuple13*yy=_tmp1D3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1097
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY19).tag=27U,(_tmp6EC.YY19).val=yy1;_tmp6EC;});}static char _tmp1D7[16U]="storage_class_t";
# 1091 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D7,_tmp1D7,_tmp1D7 + 16U};
union Cyc_YYSTYPE*_tmp1D5=yy1;enum Cyc_Parse_Storage_class _tmp1D6;if((((union Cyc_YYSTYPE*)_tmp1D5)->YY20).tag == 28){_LL1: _tmp1D6=(_tmp1D5->YY20).val;_LL2: {enum Cyc_Parse_Storage_class yy=_tmp1D6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1098
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY20).tag=28U,(_tmp6ED.YY20).val=yy1;_tmp6ED;});}static char _tmp1DA[17U]="type_specifier_t";
# 1092 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DA,_tmp1DA,_tmp1DA + 17U};
union Cyc_YYSTYPE*_tmp1D8=yy1;struct Cyc_Parse_Type_specifier _tmp1D9;if((((union Cyc_YYSTYPE*)_tmp1D8)->YY21).tag == 29){_LL1: _tmp1D9=(_tmp1D8->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1D9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1099
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY21).tag=29U,(_tmp6EE.YY21).val=yy1;_tmp6EE;});}static char _tmp1DD[12U]="aggr_kind_t";
# 1094 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DD,_tmp1DD,_tmp1DD + 12U};
union Cyc_YYSTYPE*_tmp1DB=yy1;enum Cyc_Absyn_AggrKind _tmp1DC;if((((union Cyc_YYSTYPE*)_tmp1DB)->YY22).tag == 30){_LL1: _tmp1DC=(_tmp1DB->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1DC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1101
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY22).tag=30U,(_tmp6EF.YY22).val=yy1;_tmp6EF;});}static char _tmp1E0[8U]="tqual_t";
# 1095 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E0,_tmp1E0,_tmp1E0 + 8U};
union Cyc_YYSTYPE*_tmp1DE=yy1;struct Cyc_Absyn_Tqual _tmp1DF;if((((union Cyc_YYSTYPE*)_tmp1DE)->YY23).tag == 31){_LL1: _tmp1DF=(_tmp1DE->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1DF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1102
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY23).tag=31U,(_tmp6F0.YY23).val=yy1;_tmp6F0;});}static char _tmp1E3[23U]="list_t<aggrfield_t,`H>";
# 1096 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E3,_tmp1E3,_tmp1E3 + 23U};
union Cyc_YYSTYPE*_tmp1E1=yy1;struct Cyc_List_List*_tmp1E2;if((((union Cyc_YYSTYPE*)_tmp1E1)->YY24).tag == 32){_LL1: _tmp1E2=(_tmp1E1->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1E2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY24).tag=32U,(_tmp6F1.YY24).val=yy1;_tmp6F1;});}static char _tmp1E6[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1097 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E6,_tmp1E6,_tmp1E6 + 34U};
union Cyc_YYSTYPE*_tmp1E4=yy1;struct Cyc_List_List*_tmp1E5;if((((union Cyc_YYSTYPE*)_tmp1E4)->YY25).tag == 33){_LL1: _tmp1E5=(_tmp1E4->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1E5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY25).tag=33U,(_tmp6F2.YY25).val=yy1;_tmp6F2;});}static char _tmp1E9[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1098 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E9,_tmp1E9,_tmp1E9 + 33U};
union Cyc_YYSTYPE*_tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;if((((union Cyc_YYSTYPE*)_tmp1E7)->YY26).tag == 34){_LL1: _tmp1E8=(_tmp1E7->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1E8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY26).tag=34U,(_tmp6F3.YY26).val=yy1;_tmp6F3;});}static char _tmp1EC[18U]="declarator_t<`yy>";
# 1099 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EC,_tmp1EC,_tmp1EC + 18U};
union Cyc_YYSTYPE*_tmp1EA=yy1;struct Cyc_Parse_Declarator _tmp1EB;if((((union Cyc_YYSTYPE*)_tmp1EA)->YY27).tag == 35){_LL1: _tmp1EB=(_tmp1EA->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1EB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1106
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY27).tag=35U,(_tmp6F4.YY27).val=yy1;_tmp6F4;});}static char _tmp1EF[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1100 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EF,_tmp1EF,_tmp1EF + 45U};
union Cyc_YYSTYPE*_tmp1ED=yy1;struct _tuple25*_tmp1EE;if((((union Cyc_YYSTYPE*)_tmp1ED)->YY28).tag == 36){_LL1: _tmp1EE=(_tmp1ED->YY28).val;_LL2: {struct _tuple25*yy=_tmp1EE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1107
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY28).tag=36U,(_tmp6F5.YY28).val=yy1;_tmp6F5;});}static char _tmp1F2[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1101 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F2,_tmp1F2,_tmp1F2 + 57U};
union Cyc_YYSTYPE*_tmp1F0=yy1;struct Cyc_List_List*_tmp1F1;if((((union Cyc_YYSTYPE*)_tmp1F0)->YY29).tag == 37){_LL1: _tmp1F1=(_tmp1F0->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1F1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY29).tag=37U,(_tmp6F6.YY29).val=yy1;_tmp6F6;});}static char _tmp1F5[26U]="abstractdeclarator_t<`yy>";
# 1102 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F5,_tmp1F5,_tmp1F5 + 26U};
union Cyc_YYSTYPE*_tmp1F3=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F4;if((((union Cyc_YYSTYPE*)_tmp1F3)->YY30).tag == 38){_LL1: _tmp1F4=(_tmp1F3->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1F4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY30).tag=38U,(_tmp6F7.YY30).val=yy1;_tmp6F7;});}static char _tmp1F8[5U]="bool";
# 1103 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F8,_tmp1F8,_tmp1F8 + 5U};
union Cyc_YYSTYPE*_tmp1F6=yy1;int _tmp1F7;if((((union Cyc_YYSTYPE*)_tmp1F6)->YY31).tag == 39){_LL1: _tmp1F7=(_tmp1F6->YY31).val;_LL2: {int yy=_tmp1F7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY31).tag=39U,(_tmp6F8.YY31).val=yy1;_tmp6F8;});}static char _tmp1FB[8U]="scope_t";
# 1104 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FB,_tmp1FB,_tmp1FB + 8U};
union Cyc_YYSTYPE*_tmp1F9=yy1;enum Cyc_Absyn_Scope _tmp1FA;if((((union Cyc_YYSTYPE*)_tmp1F9)->YY32).tag == 40){_LL1: _tmp1FA=(_tmp1F9->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1FA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY32).tag=40U,(_tmp6F9.YY32).val=yy1;_tmp6F9;});}static char _tmp1FE[16U]="datatypefield_t";
# 1105 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FE,_tmp1FE,_tmp1FE + 16U};
union Cyc_YYSTYPE*_tmp1FC=yy1;struct Cyc_Absyn_Datatypefield*_tmp1FD;if((((union Cyc_YYSTYPE*)_tmp1FC)->YY33).tag == 41){_LL1: _tmp1FD=(_tmp1FC->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp1FD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1112
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY33).tag=41U,(_tmp6FA.YY33).val=yy1;_tmp6FA;});}static char _tmp201[27U]="list_t<datatypefield_t,`H>";
# 1106 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp201,_tmp201,_tmp201 + 27U};
union Cyc_YYSTYPE*_tmp1FF=yy1;struct Cyc_List_List*_tmp200;if((((union Cyc_YYSTYPE*)_tmp1FF)->YY34).tag == 42){_LL1: _tmp200=(_tmp1FF->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp200;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1113
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY34).tag=42U,(_tmp6FB.YY34).val=yy1;_tmp6FB;});}static char _tmp204[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1107 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp204,_tmp204,_tmp204 + 41U};
union Cyc_YYSTYPE*_tmp202=yy1;struct _tuple26 _tmp203;if((((union Cyc_YYSTYPE*)_tmp202)->YY35).tag == 43){_LL1: _tmp203=(_tmp202->YY35).val;_LL2: {struct _tuple26 yy=_tmp203;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1114
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY35).tag=43U,(_tmp6FC.YY35).val=yy1;_tmp6FC;});}static char _tmp207[17U]="list_t<var_t,`H>";
# 1108 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp207,_tmp207,_tmp207 + 17U};
union Cyc_YYSTYPE*_tmp205=yy1;struct Cyc_List_List*_tmp206;if((((union Cyc_YYSTYPE*)_tmp205)->YY36).tag == 44){_LL1: _tmp206=(_tmp205->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp206;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1115
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY36).tag=44U,(_tmp6FD.YY36).val=yy1;_tmp6FD;});}static char _tmp20A[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1109 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20A,_tmp20A,_tmp20A + 31U};
union Cyc_YYSTYPE*_tmp208=yy1;struct _tuple8*_tmp209;if((((union Cyc_YYSTYPE*)_tmp208)->YY37).tag == 45){_LL1: _tmp209=(_tmp208->YY37).val;_LL2: {struct _tuple8*yy=_tmp209;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1116
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY37).tag=45U,(_tmp6FE.YY37).val=yy1;_tmp6FE;});}static char _tmp20D[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1110 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20D,_tmp20D,_tmp20D + 42U};
union Cyc_YYSTYPE*_tmp20B=yy1;struct Cyc_List_List*_tmp20C;if((((union Cyc_YYSTYPE*)_tmp20B)->YY38).tag == 46){_LL1: _tmp20C=(_tmp20B->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp20C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1117
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY38).tag=46U,(_tmp6FF.YY38).val=yy1;_tmp6FF;});}static char _tmp210[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1111 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp210,_tmp210,_tmp210 + 115U};
union Cyc_YYSTYPE*_tmp20E=yy1;struct _tuple27*_tmp20F;if((((union Cyc_YYSTYPE*)_tmp20E)->YY39).tag == 47){_LL1: _tmp20F=(_tmp20E->YY39).val;_LL2: {struct _tuple27*yy=_tmp20F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1118
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY39).tag=47U,(_tmp700.YY39).val=yy1;_tmp700;});}static char _tmp213[8U]="types_t";
# 1112 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp213,_tmp213,_tmp213 + 8U};
union Cyc_YYSTYPE*_tmp211=yy1;struct Cyc_List_List*_tmp212;if((((union Cyc_YYSTYPE*)_tmp211)->YY40).tag == 48){_LL1: _tmp212=(_tmp211->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp212;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY40).tag=48U,(_tmp701.YY40).val=yy1;_tmp701;});}static char _tmp216[24U]="list_t<designator_t,`H>";
# 1114 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp216,_tmp216,_tmp216 + 24U};
union Cyc_YYSTYPE*_tmp214=yy1;struct Cyc_List_List*_tmp215;if((((union Cyc_YYSTYPE*)_tmp214)->YY41).tag == 49){_LL1: _tmp215=(_tmp214->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp215;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY41).tag=49U,(_tmp702.YY41).val=yy1;_tmp702;});}static char _tmp219[13U]="designator_t";
# 1115 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp219,_tmp219,_tmp219 + 13U};
union Cyc_YYSTYPE*_tmp217=yy1;void*_tmp218;if((((union Cyc_YYSTYPE*)_tmp217)->YY42).tag == 50){_LL1: _tmp218=(_tmp217->YY42).val;_LL2: {void*yy=_tmp218;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY42).tag=50U,(_tmp703.YY42).val=yy1;_tmp703;});}static char _tmp21C[7U]="kind_t";
# 1116 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21C,_tmp21C,_tmp21C + 7U};
union Cyc_YYSTYPE*_tmp21A=yy1;struct Cyc_Absyn_Kind*_tmp21B;if((((union Cyc_YYSTYPE*)_tmp21A)->YY43).tag == 51){_LL1: _tmp21B=(_tmp21A->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp21B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY43).tag=51U,(_tmp704.YY43).val=yy1;_tmp704;});}static char _tmp21F[7U]="type_t";
# 1117 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21F,_tmp21F,_tmp21F + 7U};
union Cyc_YYSTYPE*_tmp21D=yy1;void*_tmp21E;if((((union Cyc_YYSTYPE*)_tmp21D)->YY44).tag == 52){_LL1: _tmp21E=(_tmp21D->YY44).val;_LL2: {void*yy=_tmp21E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1124
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY44).tag=52U,(_tmp705.YY44).val=yy1;_tmp705;});}static char _tmp222[23U]="list_t<attribute_t,`H>";
# 1118 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp222,_tmp222,_tmp222 + 23U};
union Cyc_YYSTYPE*_tmp220=yy1;struct Cyc_List_List*_tmp221;if((((union Cyc_YYSTYPE*)_tmp220)->YY45).tag == 53){_LL1: _tmp221=(_tmp220->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp221;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1125
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY45).tag=53U,(_tmp706.YY45).val=yy1;_tmp706;});}static char _tmp225[12U]="attribute_t";
# 1119 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp225,_tmp225,_tmp225 + 12U};
union Cyc_YYSTYPE*_tmp223=yy1;void*_tmp224;if((((union Cyc_YYSTYPE*)_tmp223)->YY46).tag == 54){_LL1: _tmp224=(_tmp223->YY46).val;_LL2: {void*yy=_tmp224;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1126
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY46).tag=54U,(_tmp707.YY46).val=yy1;_tmp707;});}static char _tmp228[12U]="enumfield_t";
# 1120 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp228,_tmp228,_tmp228 + 12U};
union Cyc_YYSTYPE*_tmp226=yy1;struct Cyc_Absyn_Enumfield*_tmp227;if((((union Cyc_YYSTYPE*)_tmp226)->YY47).tag == 55){_LL1: _tmp227=(_tmp226->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp227;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1127
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY47).tag=55U,(_tmp708.YY47).val=yy1;_tmp708;});}static char _tmp22B[23U]="list_t<enumfield_t,`H>";
# 1121 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22B,_tmp22B,_tmp22B + 23U};
union Cyc_YYSTYPE*_tmp229=yy1;struct Cyc_List_List*_tmp22A;if((((union Cyc_YYSTYPE*)_tmp229)->YY48).tag == 56){_LL1: _tmp22A=(_tmp229->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp22A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1128
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY48).tag=56U,(_tmp709.YY48).val=yy1;_tmp709;});}static char _tmp22E[11U]="type_opt_t";
# 1122 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22E,_tmp22E,_tmp22E + 11U};
union Cyc_YYSTYPE*_tmp22C=yy1;void*_tmp22D;if((((union Cyc_YYSTYPE*)_tmp22C)->YY49).tag == 57){_LL1: _tmp22D=(_tmp22C->YY49).val;_LL2: {void*yy=_tmp22D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1129
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY49).tag=57U,(_tmp70A.YY49).val=yy1;_tmp70A;});}static char _tmp231[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1123 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp231,_tmp231,_tmp231 + 31U};
union Cyc_YYSTYPE*_tmp22F=yy1;struct Cyc_List_List*_tmp230;if((((union Cyc_YYSTYPE*)_tmp22F)->YY50).tag == 58){_LL1: _tmp230=(_tmp22F->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp230;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY50).tag=58U,(_tmp70B.YY50).val=yy1;_tmp70B;});}static char _tmp234[11U]="booltype_t";
# 1124 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp234,_tmp234,_tmp234 + 11U};
union Cyc_YYSTYPE*_tmp232=yy1;void*_tmp233;if((((union Cyc_YYSTYPE*)_tmp232)->YY51).tag == 59){_LL1: _tmp233=(_tmp232->YY51).val;_LL2: {void*yy=_tmp233;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY51).tag=59U,(_tmp70C.YY51).val=yy1;_tmp70C;});}static char _tmp237[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1125 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp237,_tmp237,_tmp237 + 35U};
union Cyc_YYSTYPE*_tmp235=yy1;struct Cyc_List_List*_tmp236;if((((union Cyc_YYSTYPE*)_tmp235)->YY52).tag == 60){_LL1: _tmp236=(_tmp235->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp236;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1132
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY52).tag=60U,(_tmp70D.YY52).val=yy1;_tmp70D;});}static char _tmp23A[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1126 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23A,_tmp23A,_tmp23A + 48U};
union Cyc_YYSTYPE*_tmp238=yy1;struct _tuple28*_tmp239;if((((union Cyc_YYSTYPE*)_tmp238)->YY53).tag == 61){_LL1: _tmp239=(_tmp238->YY53).val;_LL2: {struct _tuple28*yy=_tmp239;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1133
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY53).tag=61U,(_tmp70E.YY53).val=yy1;_tmp70E;});}static char _tmp23D[18U]="list_t<qvar_t,`H>";
# 1127 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23D,_tmp23D,_tmp23D + 18U};
union Cyc_YYSTYPE*_tmp23B=yy1;struct Cyc_List_List*_tmp23C;if((((union Cyc_YYSTYPE*)_tmp23B)->YY54).tag == 62){_LL1: _tmp23C=(_tmp23B->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp23C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1134
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY54).tag=62U,(_tmp70F.YY54).val=yy1;_tmp70F;});}static char _tmp240[20U]="pointer_qual_t<`yy>";
# 1128 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp240,_tmp240,_tmp240 + 20U};
union Cyc_YYSTYPE*_tmp23E=yy1;void*_tmp23F;if((((union Cyc_YYSTYPE*)_tmp23E)->YY55).tag == 63){_LL1: _tmp23F=(_tmp23E->YY55).val;_LL2: {void*yy=_tmp23F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1135
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY55).tag=63U,(_tmp710.YY55).val=yy1;_tmp710;});}static char _tmp243[21U]="pointer_quals_t<`yy>";
# 1129 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp243,_tmp243,_tmp243 + 21U};
union Cyc_YYSTYPE*_tmp241=yy1;struct Cyc_List_List*_tmp242;if((((union Cyc_YYSTYPE*)_tmp241)->YY56).tag == 64){_LL1: _tmp242=(_tmp241->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp242;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1136
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY56).tag=64U,(_tmp711.YY56).val=yy1;_tmp711;});}static char _tmp246[10U]="exp_opt_t";
# 1130 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp246,_tmp246,_tmp246 + 10U};
union Cyc_YYSTYPE*_tmp244=yy1;struct Cyc_Absyn_Exp*_tmp245;if((((union Cyc_YYSTYPE*)_tmp244)->YY57).tag == 65){_LL1: _tmp245=(_tmp244->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp245;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY57).tag=65U,(_tmp712.YY57).val=yy1;_tmp712;});}static char _tmp249[10U]="raw_exp_t";
# 1131 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp249,_tmp249,_tmp249 + 10U};
union Cyc_YYSTYPE*_tmp247=yy1;void*_tmp248;if((((union Cyc_YYSTYPE*)_tmp247)->YY58).tag == 66){_LL1: _tmp248=(_tmp247->YY58).val;_LL2: {void*yy=_tmp248;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY58).tag=66U,(_tmp713.YY58).val=yy1;_tmp713;});}static char _tmp24C[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1133 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24C,_tmp24C,_tmp24C + 112U};
union Cyc_YYSTYPE*_tmp24A=yy1;struct _tuple29*_tmp24B;if((((union Cyc_YYSTYPE*)_tmp24A)->YY59).tag == 67){_LL1: _tmp24B=(_tmp24A->YY59).val;_LL2: {struct _tuple29*yy=_tmp24B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1140
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY59).tag=67U,(_tmp714.YY59).val=yy1;_tmp714;});}static char _tmp24F[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1134 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24F,_tmp24F,_tmp24F + 73U};
union Cyc_YYSTYPE*_tmp24D=yy1;struct _tuple30*_tmp24E;if((((union Cyc_YYSTYPE*)_tmp24D)->YY60).tag == 68){_LL1: _tmp24E=(_tmp24D->YY60).val;_LL2: {struct _tuple30*yy=_tmp24E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY60).tag=68U,(_tmp715.YY60).val=yy1;_tmp715;});}static char _tmp252[28U]="list_t<string_t<`H>@`H, `H>";
# 1135 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp252,_tmp252,_tmp252 + 28U};
union Cyc_YYSTYPE*_tmp250=yy1;struct Cyc_List_List*_tmp251;if((((union Cyc_YYSTYPE*)_tmp250)->YY61).tag == 69){_LL1: _tmp251=(_tmp250->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp251;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY61).tag=69U,(_tmp716.YY61).val=yy1;_tmp716;});}static char _tmp255[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1136 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp255,_tmp255,_tmp255 + 38U};
union Cyc_YYSTYPE*_tmp253=yy1;struct Cyc_List_List*_tmp254;if((((union Cyc_YYSTYPE*)_tmp253)->YY62).tag == 70){_LL1: _tmp254=(_tmp253->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp254;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1143
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY62).tag=70U,(_tmp717.YY62).val=yy1;_tmp717;});}static char _tmp258[26U]="$(string_t<`H>, exp_t)@`H";
# 1137 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp258,_tmp258,_tmp258 + 26U};
union Cyc_YYSTYPE*_tmp256=yy1;struct _tuple31*_tmp257;if((((union Cyc_YYSTYPE*)_tmp256)->YY63).tag == 71){_LL1: _tmp257=(_tmp256->YY63).val;_LL2: {struct _tuple31*yy=_tmp257;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1144
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY63).tag=71U,(_tmp718.YY63).val=yy1;_tmp718;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1160
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp719;_tmp719.timestamp=0,_tmp719.first_line=0,_tmp719.first_column=0,_tmp719.last_line=0,_tmp719.last_column=0;_tmp719;});}
# 1163
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1174 "parse.y"
static short Cyc_yytranslate[381U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,152,2,2,137,150,147,2,134,135,130,144,129,148,139,149,2,2,2,2,2,2,2,2,2,2,138,126,132,131,133,143,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,2,141,146,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,127,145,128,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125};static char _tmp259[2U]="$";static char _tmp25A[6U]="error";static char _tmp25B[12U]="$undefined.";static char _tmp25C[5U]="AUTO";static char _tmp25D[9U]="REGISTER";static char _tmp25E[7U]="STATIC";static char _tmp25F[7U]="EXTERN";static char _tmp260[8U]="TYPEDEF";static char _tmp261[5U]="VOID";static char _tmp262[5U]="CHAR";static char _tmp263[6U]="SHORT";static char _tmp264[4U]="INT";static char _tmp265[5U]="LONG";static char _tmp266[6U]="FLOAT";static char _tmp267[7U]="DOUBLE";static char _tmp268[7U]="SIGNED";static char _tmp269[9U]="UNSIGNED";static char _tmp26A[6U]="CONST";static char _tmp26B[9U]="VOLATILE";static char _tmp26C[9U]="RESTRICT";static char _tmp26D[7U]="STRUCT";static char _tmp26E[6U]="UNION";static char _tmp26F[5U]="CASE";static char _tmp270[8U]="DEFAULT";static char _tmp271[7U]="INLINE";static char _tmp272[7U]="SIZEOF";static char _tmp273[9U]="OFFSETOF";static char _tmp274[3U]="IF";static char _tmp275[5U]="ELSE";static char _tmp276[7U]="SWITCH";static char _tmp277[6U]="WHILE";static char _tmp278[3U]="DO";static char _tmp279[4U]="FOR";static char _tmp27A[5U]="GOTO";static char _tmp27B[9U]="CONTINUE";static char _tmp27C[6U]="BREAK";static char _tmp27D[7U]="RETURN";static char _tmp27E[5U]="ENUM";static char _tmp27F[7U]="TYPEOF";static char _tmp280[16U]="BUILTIN_VA_LIST";static char _tmp281[10U]="EXTENSION";static char _tmp282[8U]="NULL_kw";static char _tmp283[4U]="LET";static char _tmp284[6U]="THROW";static char _tmp285[4U]="TRY";static char _tmp286[6U]="CATCH";static char _tmp287[7U]="EXPORT";static char _tmp288[9U]="OVERRIDE";static char _tmp289[5U]="HIDE";static char _tmp28A[4U]="NEW";static char _tmp28B[9U]="ABSTRACT";static char _tmp28C[9U]="FALLTHRU";static char _tmp28D[6U]="USING";static char _tmp28E[10U]="NAMESPACE";static char _tmp28F[9U]="DATATYPE";static char _tmp290[7U]="MALLOC";static char _tmp291[8U]="RMALLOC";static char _tmp292[15U]="RMALLOC_INLINE";static char _tmp293[7U]="CALLOC";static char _tmp294[8U]="RCALLOC";static char _tmp295[5U]="SWAP";static char _tmp296[9U]="REGION_T";static char _tmp297[6U]="TAG_T";static char _tmp298[7U]="REGION";static char _tmp299[5U]="RNEW";static char _tmp29A[8U]="REGIONS";static char _tmp29B[7U]="PORTON";static char _tmp29C[8U]="PORTOFF";static char _tmp29D[7U]="PRAGMA";static char _tmp29E[10U]="TEMPESTON";static char _tmp29F[11U]="TEMPESTOFF";static char _tmp2A0[8U]="NUMELTS";static char _tmp2A1[8U]="VALUEOF";static char _tmp2A2[10U]="VALUEOF_T";static char _tmp2A3[9U]="TAGCHECK";static char _tmp2A4[13U]="NUMELTS_QUAL";static char _tmp2A5[10U]="THIN_QUAL";static char _tmp2A6[9U]="FAT_QUAL";static char _tmp2A7[13U]="NOTNULL_QUAL";static char _tmp2A8[14U]="NULLABLE_QUAL";static char _tmp2A9[14U]="REQUIRES_QUAL";static char _tmp2AA[13U]="ENSURES_QUAL";static char _tmp2AB[12U]="REGION_QUAL";static char _tmp2AC[16U]="NOZEROTERM_QUAL";static char _tmp2AD[14U]="ZEROTERM_QUAL";static char _tmp2AE[12U]="TAGGED_QUAL";static char _tmp2AF[12U]="ASSERT_QUAL";static char _tmp2B0[16U]="EXTENSIBLE_QUAL";static char _tmp2B1[18U]="AUTORELEASED_QUAL";static char _tmp2B2[7U]="PTR_OP";static char _tmp2B3[7U]="INC_OP";static char _tmp2B4[7U]="DEC_OP";static char _tmp2B5[8U]="LEFT_OP";static char _tmp2B6[9U]="RIGHT_OP";static char _tmp2B7[6U]="LE_OP";static char _tmp2B8[6U]="GE_OP";static char _tmp2B9[6U]="EQ_OP";static char _tmp2BA[6U]="NE_OP";static char _tmp2BB[7U]="AND_OP";static char _tmp2BC[6U]="OR_OP";static char _tmp2BD[11U]="MUL_ASSIGN";static char _tmp2BE[11U]="DIV_ASSIGN";static char _tmp2BF[11U]="MOD_ASSIGN";static char _tmp2C0[11U]="ADD_ASSIGN";static char _tmp2C1[11U]="SUB_ASSIGN";static char _tmp2C2[12U]="LEFT_ASSIGN";static char _tmp2C3[13U]="RIGHT_ASSIGN";static char _tmp2C4[11U]="AND_ASSIGN";static char _tmp2C5[11U]="XOR_ASSIGN";static char _tmp2C6[10U]="OR_ASSIGN";static char _tmp2C7[9U]="ELLIPSIS";static char _tmp2C8[11U]="LEFT_RIGHT";static char _tmp2C9[12U]="COLON_COLON";static char _tmp2CA[11U]="IDENTIFIER";static char _tmp2CB[17U]="INTEGER_CONSTANT";static char _tmp2CC[7U]="STRING";static char _tmp2CD[8U]="WSTRING";static char _tmp2CE[19U]="CHARACTER_CONSTANT";static char _tmp2CF[20U]="WCHARACTER_CONSTANT";static char _tmp2D0[18U]="FLOATING_CONSTANT";static char _tmp2D1[9U]="TYPE_VAR";static char _tmp2D2[13U]="TYPEDEF_NAME";static char _tmp2D3[16U]="QUAL_IDENTIFIER";static char _tmp2D4[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D5[10U]="ATTRIBUTE";static char _tmp2D6[8U]="ASM_TOK";static char _tmp2D7[4U]="';'";static char _tmp2D8[4U]="'{'";static char _tmp2D9[4U]="'}'";static char _tmp2DA[4U]="','";static char _tmp2DB[4U]="'*'";static char _tmp2DC[4U]="'='";static char _tmp2DD[4U]="'<'";static char _tmp2DE[4U]="'>'";static char _tmp2DF[4U]="'('";static char _tmp2E0[4U]="')'";static char _tmp2E1[4U]="'_'";static char _tmp2E2[4U]="'$'";static char _tmp2E3[4U]="':'";static char _tmp2E4[4U]="'.'";static char _tmp2E5[4U]="'['";static char _tmp2E6[4U]="']'";static char _tmp2E7[4U]="'@'";static char _tmp2E8[4U]="'?'";static char _tmp2E9[4U]="'+'";static char _tmp2EA[4U]="'|'";static char _tmp2EB[4U]="'^'";static char _tmp2EC[4U]="'&'";static char _tmp2ED[4U]="'-'";static char _tmp2EE[4U]="'/'";static char _tmp2EF[4U]="'%'";static char _tmp2F0[4U]="'~'";static char _tmp2F1[4U]="'!'";static char _tmp2F2[5U]="prog";static char _tmp2F3[17U]="translation_unit";static char _tmp2F4[18U]="tempest_on_action";static char _tmp2F5[19U]="tempest_off_action";static char _tmp2F6[16U]="extern_c_action";static char _tmp2F7[13U]="end_extern_c";static char _tmp2F8[14U]="hide_list_opt";static char _tmp2F9[17U]="hide_list_values";static char _tmp2FA[16U]="export_list_opt";static char _tmp2FB[12U]="export_list";static char _tmp2FC[19U]="export_list_values";static char _tmp2FD[13U]="override_opt";static char _tmp2FE[21U]="external_declaration";static char _tmp2FF[15U]="optional_comma";static char _tmp300[20U]="function_definition";static char _tmp301[21U]="function_definition2";static char _tmp302[13U]="using_action";static char _tmp303[15U]="unusing_action";static char _tmp304[17U]="namespace_action";static char _tmp305[19U]="unnamespace_action";static char _tmp306[12U]="declaration";static char _tmp307[17U]="declaration_list";static char _tmp308[23U]="declaration_specifiers";static char _tmp309[24U]="storage_class_specifier";static char _tmp30A[15U]="attributes_opt";static char _tmp30B[11U]="attributes";static char _tmp30C[15U]="attribute_list";static char _tmp30D[10U]="attribute";static char _tmp30E[15U]="type_specifier";static char _tmp30F[25U]="type_specifier_notypedef";static char _tmp310[5U]="kind";static char _tmp311[15U]="type_qualifier";static char _tmp312[15U]="enum_specifier";static char _tmp313[11U]="enum_field";static char _tmp314[22U]="enum_declaration_list";static char _tmp315[26U]="struct_or_union_specifier";static char _tmp316[16U]="type_params_opt";static char _tmp317[16U]="struct_or_union";static char _tmp318[24U]="struct_declaration_list";static char _tmp319[25U]="struct_declaration_list0";static char _tmp31A[21U]="init_declarator_list";static char _tmp31B[22U]="init_declarator_list0";static char _tmp31C[16U]="init_declarator";static char _tmp31D[19U]="struct_declaration";static char _tmp31E[25U]="specifier_qualifier_list";static char _tmp31F[35U]="notypedef_specifier_qualifier_list";static char _tmp320[23U]="struct_declarator_list";static char _tmp321[24U]="struct_declarator_list0";static char _tmp322[18U]="struct_declarator";static char _tmp323[20U]="requires_clause_opt";static char _tmp324[19U]="ensures_clause_opt";static char _tmp325[19U]="datatype_specifier";static char _tmp326[14U]="qual_datatype";static char _tmp327[19U]="datatypefield_list";static char _tmp328[20U]="datatypefield_scope";static char _tmp329[14U]="datatypefield";static char _tmp32A[11U]="declarator";static char _tmp32B[23U]="declarator_withtypedef";static char _tmp32C[18U]="direct_declarator";static char _tmp32D[30U]="direct_declarator_withtypedef";static char _tmp32E[8U]="pointer";static char _tmp32F[12U]="one_pointer";static char _tmp330[14U]="pointer_quals";static char _tmp331[13U]="pointer_qual";static char _tmp332[23U]="pointer_null_and_bound";static char _tmp333[14U]="pointer_bound";static char _tmp334[18U]="zeroterm_qual_opt";static char _tmp335[8U]="rgn_opt";static char _tmp336[11U]="tqual_list";static char _tmp337[20U]="parameter_type_list";static char _tmp338[9U]="type_var";static char _tmp339[16U]="optional_effect";static char _tmp33A[19U]="optional_rgn_order";static char _tmp33B[10U]="rgn_order";static char _tmp33C[16U]="optional_inject";static char _tmp33D[11U]="effect_set";static char _tmp33E[14U]="atomic_effect";static char _tmp33F[11U]="region_set";static char _tmp340[15U]="parameter_list";static char _tmp341[22U]="parameter_declaration";static char _tmp342[16U]="identifier_list";static char _tmp343[17U]="identifier_list0";static char _tmp344[12U]="initializer";static char _tmp345[18U]="array_initializer";static char _tmp346[17U]="initializer_list";static char _tmp347[12U]="designation";static char _tmp348[16U]="designator_list";static char _tmp349[11U]="designator";static char _tmp34A[10U]="type_name";static char _tmp34B[14U]="any_type_name";static char _tmp34C[15U]="type_name_list";static char _tmp34D[20U]="abstract_declarator";static char _tmp34E[27U]="direct_abstract_declarator";static char _tmp34F[10U]="statement";static char _tmp350[18U]="labeled_statement";static char _tmp351[21U]="expression_statement";static char _tmp352[19U]="compound_statement";static char _tmp353[16U]="block_item_list";static char _tmp354[20U]="selection_statement";static char _tmp355[15U]="switch_clauses";static char _tmp356[20U]="iteration_statement";static char _tmp357[15U]="jump_statement";static char _tmp358[12U]="exp_pattern";static char _tmp359[20U]="conditional_pattern";static char _tmp35A[19U]="logical_or_pattern";static char _tmp35B[20U]="logical_and_pattern";static char _tmp35C[21U]="inclusive_or_pattern";static char _tmp35D[21U]="exclusive_or_pattern";static char _tmp35E[12U]="and_pattern";static char _tmp35F[17U]="equality_pattern";static char _tmp360[19U]="relational_pattern";static char _tmp361[14U]="shift_pattern";static char _tmp362[17U]="additive_pattern";static char _tmp363[23U]="multiplicative_pattern";static char _tmp364[13U]="cast_pattern";static char _tmp365[14U]="unary_pattern";static char _tmp366[16U]="postfix_pattern";static char _tmp367[16U]="primary_pattern";static char _tmp368[8U]="pattern";static char _tmp369[19U]="tuple_pattern_list";static char _tmp36A[20U]="tuple_pattern_list0";static char _tmp36B[14U]="field_pattern";static char _tmp36C[19U]="field_pattern_list";static char _tmp36D[20U]="field_pattern_list0";static char _tmp36E[11U]="expression";static char _tmp36F[22U]="assignment_expression";static char _tmp370[20U]="assignment_operator";static char _tmp371[23U]="conditional_expression";static char _tmp372[20U]="constant_expression";static char _tmp373[22U]="logical_or_expression";static char _tmp374[23U]="logical_and_expression";static char _tmp375[24U]="inclusive_or_expression";static char _tmp376[24U]="exclusive_or_expression";static char _tmp377[15U]="and_expression";static char _tmp378[20U]="equality_expression";static char _tmp379[22U]="relational_expression";static char _tmp37A[17U]="shift_expression";static char _tmp37B[20U]="additive_expression";static char _tmp37C[26U]="multiplicative_expression";static char _tmp37D[16U]="cast_expression";static char _tmp37E[17U]="unary_expression";static char _tmp37F[9U]="asm_expr";static char _tmp380[13U]="volatile_opt";static char _tmp381[12U]="asm_out_opt";static char _tmp382[12U]="asm_outlist";static char _tmp383[11U]="asm_in_opt";static char _tmp384[11U]="asm_inlist";static char _tmp385[11U]="asm_io_elt";static char _tmp386[16U]="asm_clobber_opt";static char _tmp387[17U]="asm_clobber_list";static char _tmp388[15U]="unary_operator";static char _tmp389[19U]="postfix_expression";static char _tmp38A[17U]="field_expression";static char _tmp38B[19U]="primary_expression";static char _tmp38C[25U]="argument_expression_list";static char _tmp38D[26U]="argument_expression_list0";static char _tmp38E[9U]="constant";static char _tmp38F[20U]="qual_opt_identifier";static char _tmp390[17U]="qual_opt_typedef";static char _tmp391[18U]="struct_union_name";static char _tmp392[11U]="field_name";static char _tmp393[12U]="right_angle";
# 1546 "parse.y"
static struct _fat_ptr Cyc_yytname[315U]={{_tmp259,_tmp259,_tmp259 + 2U},{_tmp25A,_tmp25A,_tmp25A + 6U},{_tmp25B,_tmp25B,_tmp25B + 12U},{_tmp25C,_tmp25C,_tmp25C + 5U},{_tmp25D,_tmp25D,_tmp25D + 9U},{_tmp25E,_tmp25E,_tmp25E + 7U},{_tmp25F,_tmp25F,_tmp25F + 7U},{_tmp260,_tmp260,_tmp260 + 8U},{_tmp261,_tmp261,_tmp261 + 5U},{_tmp262,_tmp262,_tmp262 + 5U},{_tmp263,_tmp263,_tmp263 + 6U},{_tmp264,_tmp264,_tmp264 + 4U},{_tmp265,_tmp265,_tmp265 + 5U},{_tmp266,_tmp266,_tmp266 + 6U},{_tmp267,_tmp267,_tmp267 + 7U},{_tmp268,_tmp268,_tmp268 + 7U},{_tmp269,_tmp269,_tmp269 + 9U},{_tmp26A,_tmp26A,_tmp26A + 6U},{_tmp26B,_tmp26B,_tmp26B + 9U},{_tmp26C,_tmp26C,_tmp26C + 9U},{_tmp26D,_tmp26D,_tmp26D + 7U},{_tmp26E,_tmp26E,_tmp26E + 6U},{_tmp26F,_tmp26F,_tmp26F + 5U},{_tmp270,_tmp270,_tmp270 + 8U},{_tmp271,_tmp271,_tmp271 + 7U},{_tmp272,_tmp272,_tmp272 + 7U},{_tmp273,_tmp273,_tmp273 + 9U},{_tmp274,_tmp274,_tmp274 + 3U},{_tmp275,_tmp275,_tmp275 + 5U},{_tmp276,_tmp276,_tmp276 + 7U},{_tmp277,_tmp277,_tmp277 + 6U},{_tmp278,_tmp278,_tmp278 + 3U},{_tmp279,_tmp279,_tmp279 + 4U},{_tmp27A,_tmp27A,_tmp27A + 5U},{_tmp27B,_tmp27B,_tmp27B + 9U},{_tmp27C,_tmp27C,_tmp27C + 6U},{_tmp27D,_tmp27D,_tmp27D + 7U},{_tmp27E,_tmp27E,_tmp27E + 5U},{_tmp27F,_tmp27F,_tmp27F + 7U},{_tmp280,_tmp280,_tmp280 + 16U},{_tmp281,_tmp281,_tmp281 + 10U},{_tmp282,_tmp282,_tmp282 + 8U},{_tmp283,_tmp283,_tmp283 + 4U},{_tmp284,_tmp284,_tmp284 + 6U},{_tmp285,_tmp285,_tmp285 + 4U},{_tmp286,_tmp286,_tmp286 + 6U},{_tmp287,_tmp287,_tmp287 + 7U},{_tmp288,_tmp288,_tmp288 + 9U},{_tmp289,_tmp289,_tmp289 + 5U},{_tmp28A,_tmp28A,_tmp28A + 4U},{_tmp28B,_tmp28B,_tmp28B + 9U},{_tmp28C,_tmp28C,_tmp28C + 9U},{_tmp28D,_tmp28D,_tmp28D + 6U},{_tmp28E,_tmp28E,_tmp28E + 10U},{_tmp28F,_tmp28F,_tmp28F + 9U},{_tmp290,_tmp290,_tmp290 + 7U},{_tmp291,_tmp291,_tmp291 + 8U},{_tmp292,_tmp292,_tmp292 + 15U},{_tmp293,_tmp293,_tmp293 + 7U},{_tmp294,_tmp294,_tmp294 + 8U},{_tmp295,_tmp295,_tmp295 + 5U},{_tmp296,_tmp296,_tmp296 + 9U},{_tmp297,_tmp297,_tmp297 + 6U},{_tmp298,_tmp298,_tmp298 + 7U},{_tmp299,_tmp299,_tmp299 + 5U},{_tmp29A,_tmp29A,_tmp29A + 8U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 8U},{_tmp29D,_tmp29D,_tmp29D + 7U},{_tmp29E,_tmp29E,_tmp29E + 10U},{_tmp29F,_tmp29F,_tmp29F + 11U},{_tmp2A0,_tmp2A0,_tmp2A0 + 8U},{_tmp2A1,_tmp2A1,_tmp2A1 + 8U},{_tmp2A2,_tmp2A2,_tmp2A2 + 10U},{_tmp2A3,_tmp2A3,_tmp2A3 + 9U},{_tmp2A4,_tmp2A4,_tmp2A4 + 13U},{_tmp2A5,_tmp2A5,_tmp2A5 + 10U},{_tmp2A6,_tmp2A6,_tmp2A6 + 9U},{_tmp2A7,_tmp2A7,_tmp2A7 + 13U},{_tmp2A8,_tmp2A8,_tmp2A8 + 14U},{_tmp2A9,_tmp2A9,_tmp2A9 + 14U},{_tmp2AA,_tmp2AA,_tmp2AA + 13U},{_tmp2AB,_tmp2AB,_tmp2AB + 12U},{_tmp2AC,_tmp2AC,_tmp2AC + 16U},{_tmp2AD,_tmp2AD,_tmp2AD + 14U},{_tmp2AE,_tmp2AE,_tmp2AE + 12U},{_tmp2AF,_tmp2AF,_tmp2AF + 12U},{_tmp2B0,_tmp2B0,_tmp2B0 + 16U},{_tmp2B1,_tmp2B1,_tmp2B1 + 18U},{_tmp2B2,_tmp2B2,_tmp2B2 + 7U},{_tmp2B3,_tmp2B3,_tmp2B3 + 7U},{_tmp2B4,_tmp2B4,_tmp2B4 + 7U},{_tmp2B5,_tmp2B5,_tmp2B5 + 8U},{_tmp2B6,_tmp2B6,_tmp2B6 + 9U},{_tmp2B7,_tmp2B7,_tmp2B7 + 6U},{_tmp2B8,_tmp2B8,_tmp2B8 + 6U},{_tmp2B9,_tmp2B9,_tmp2B9 + 6U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 7U},{_tmp2BC,_tmp2BC,_tmp2BC + 6U},{_tmp2BD,_tmp2BD,_tmp2BD + 11U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 11U},{_tmp2C1,_tmp2C1,_tmp2C1 + 11U},{_tmp2C2,_tmp2C2,_tmp2C2 + 12U},{_tmp2C3,_tmp2C3,_tmp2C3 + 13U},{_tmp2C4,_tmp2C4,_tmp2C4 + 11U},{_tmp2C5,_tmp2C5,_tmp2C5 + 11U},{_tmp2C6,_tmp2C6,_tmp2C6 + 10U},{_tmp2C7,_tmp2C7,_tmp2C7 + 9U},{_tmp2C8,_tmp2C8,_tmp2C8 + 11U},{_tmp2C9,_tmp2C9,_tmp2C9 + 12U},{_tmp2CA,_tmp2CA,_tmp2CA + 11U},{_tmp2CB,_tmp2CB,_tmp2CB + 17U},{_tmp2CC,_tmp2CC,_tmp2CC + 7U},{_tmp2CD,_tmp2CD,_tmp2CD + 8U},{_tmp2CE,_tmp2CE,_tmp2CE + 19U},{_tmp2CF,_tmp2CF,_tmp2CF + 20U},{_tmp2D0,_tmp2D0,_tmp2D0 + 18U},{_tmp2D1,_tmp2D1,_tmp2D1 + 9U},{_tmp2D2,_tmp2D2,_tmp2D2 + 13U},{_tmp2D3,_tmp2D3,_tmp2D3 + 16U},{_tmp2D4,_tmp2D4,_tmp2D4 + 18U},{_tmp2D5,_tmp2D5,_tmp2D5 + 10U},{_tmp2D6,_tmp2D6,_tmp2D6 + 8U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 5U},{_tmp2F3,_tmp2F3,_tmp2F3 + 17U},{_tmp2F4,_tmp2F4,_tmp2F4 + 18U},{_tmp2F5,_tmp2F5,_tmp2F5 + 19U},{_tmp2F6,_tmp2F6,_tmp2F6 + 16U},{_tmp2F7,_tmp2F7,_tmp2F7 + 13U},{_tmp2F8,_tmp2F8,_tmp2F8 + 14U},{_tmp2F9,_tmp2F9,_tmp2F9 + 17U},{_tmp2FA,_tmp2FA,_tmp2FA + 16U},{_tmp2FB,_tmp2FB,_tmp2FB + 12U},{_tmp2FC,_tmp2FC,_tmp2FC + 19U},{_tmp2FD,_tmp2FD,_tmp2FD + 13U},{_tmp2FE,_tmp2FE,_tmp2FE + 21U},{_tmp2FF,_tmp2FF,_tmp2FF + 15U},{_tmp300,_tmp300,_tmp300 + 20U},{_tmp301,_tmp301,_tmp301 + 21U},{_tmp302,_tmp302,_tmp302 + 13U},{_tmp303,_tmp303,_tmp303 + 15U},{_tmp304,_tmp304,_tmp304 + 17U},{_tmp305,_tmp305,_tmp305 + 19U},{_tmp306,_tmp306,_tmp306 + 12U},{_tmp307,_tmp307,_tmp307 + 17U},{_tmp308,_tmp308,_tmp308 + 23U},{_tmp309,_tmp309,_tmp309 + 24U},{_tmp30A,_tmp30A,_tmp30A + 15U},{_tmp30B,_tmp30B,_tmp30B + 11U},{_tmp30C,_tmp30C,_tmp30C + 15U},{_tmp30D,_tmp30D,_tmp30D + 10U},{_tmp30E,_tmp30E,_tmp30E + 15U},{_tmp30F,_tmp30F,_tmp30F + 25U},{_tmp310,_tmp310,_tmp310 + 5U},{_tmp311,_tmp311,_tmp311 + 15U},{_tmp312,_tmp312,_tmp312 + 15U},{_tmp313,_tmp313,_tmp313 + 11U},{_tmp314,_tmp314,_tmp314 + 22U},{_tmp315,_tmp315,_tmp315 + 26U},{_tmp316,_tmp316,_tmp316 + 16U},{_tmp317,_tmp317,_tmp317 + 16U},{_tmp318,_tmp318,_tmp318 + 24U},{_tmp319,_tmp319,_tmp319 + 25U},{_tmp31A,_tmp31A,_tmp31A + 21U},{_tmp31B,_tmp31B,_tmp31B + 22U},{_tmp31C,_tmp31C,_tmp31C + 16U},{_tmp31D,_tmp31D,_tmp31D + 19U},{_tmp31E,_tmp31E,_tmp31E + 25U},{_tmp31F,_tmp31F,_tmp31F + 35U},{_tmp320,_tmp320,_tmp320 + 23U},{_tmp321,_tmp321,_tmp321 + 24U},{_tmp322,_tmp322,_tmp322 + 18U},{_tmp323,_tmp323,_tmp323 + 20U},{_tmp324,_tmp324,_tmp324 + 19U},{_tmp325,_tmp325,_tmp325 + 19U},{_tmp326,_tmp326,_tmp326 + 14U},{_tmp327,_tmp327,_tmp327 + 19U},{_tmp328,_tmp328,_tmp328 + 20U},{_tmp329,_tmp329,_tmp329 + 14U},{_tmp32A,_tmp32A,_tmp32A + 11U},{_tmp32B,_tmp32B,_tmp32B + 23U},{_tmp32C,_tmp32C,_tmp32C + 18U},{_tmp32D,_tmp32D,_tmp32D + 30U},{_tmp32E,_tmp32E,_tmp32E + 8U},{_tmp32F,_tmp32F,_tmp32F + 12U},{_tmp330,_tmp330,_tmp330 + 14U},{_tmp331,_tmp331,_tmp331 + 13U},{_tmp332,_tmp332,_tmp332 + 23U},{_tmp333,_tmp333,_tmp333 + 14U},{_tmp334,_tmp334,_tmp334 + 18U},{_tmp335,_tmp335,_tmp335 + 8U},{_tmp336,_tmp336,_tmp336 + 11U},{_tmp337,_tmp337,_tmp337 + 20U},{_tmp338,_tmp338,_tmp338 + 9U},{_tmp339,_tmp339,_tmp339 + 16U},{_tmp33A,_tmp33A,_tmp33A + 19U},{_tmp33B,_tmp33B,_tmp33B + 10U},{_tmp33C,_tmp33C,_tmp33C + 16U},{_tmp33D,_tmp33D,_tmp33D + 11U},{_tmp33E,_tmp33E,_tmp33E + 14U},{_tmp33F,_tmp33F,_tmp33F + 11U},{_tmp340,_tmp340,_tmp340 + 15U},{_tmp341,_tmp341,_tmp341 + 22U},{_tmp342,_tmp342,_tmp342 + 16U},{_tmp343,_tmp343,_tmp343 + 17U},{_tmp344,_tmp344,_tmp344 + 12U},{_tmp345,_tmp345,_tmp345 + 18U},{_tmp346,_tmp346,_tmp346 + 17U},{_tmp347,_tmp347,_tmp347 + 12U},{_tmp348,_tmp348,_tmp348 + 16U},{_tmp349,_tmp349,_tmp349 + 11U},{_tmp34A,_tmp34A,_tmp34A + 10U},{_tmp34B,_tmp34B,_tmp34B + 14U},{_tmp34C,_tmp34C,_tmp34C + 15U},{_tmp34D,_tmp34D,_tmp34D + 20U},{_tmp34E,_tmp34E,_tmp34E + 27U},{_tmp34F,_tmp34F,_tmp34F + 10U},{_tmp350,_tmp350,_tmp350 + 18U},{_tmp351,_tmp351,_tmp351 + 21U},{_tmp352,_tmp352,_tmp352 + 19U},{_tmp353,_tmp353,_tmp353 + 16U},{_tmp354,_tmp354,_tmp354 + 20U},{_tmp355,_tmp355,_tmp355 + 15U},{_tmp356,_tmp356,_tmp356 + 20U},{_tmp357,_tmp357,_tmp357 + 15U},{_tmp358,_tmp358,_tmp358 + 12U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 19U},{_tmp35B,_tmp35B,_tmp35B + 20U},{_tmp35C,_tmp35C,_tmp35C + 21U},{_tmp35D,_tmp35D,_tmp35D + 21U},{_tmp35E,_tmp35E,_tmp35E + 12U},{_tmp35F,_tmp35F,_tmp35F + 17U},{_tmp360,_tmp360,_tmp360 + 19U},{_tmp361,_tmp361,_tmp361 + 14U},{_tmp362,_tmp362,_tmp362 + 17U},{_tmp363,_tmp363,_tmp363 + 23U},{_tmp364,_tmp364,_tmp364 + 13U},{_tmp365,_tmp365,_tmp365 + 14U},{_tmp366,_tmp366,_tmp366 + 16U},{_tmp367,_tmp367,_tmp367 + 16U},{_tmp368,_tmp368,_tmp368 + 8U},{_tmp369,_tmp369,_tmp369 + 19U},{_tmp36A,_tmp36A,_tmp36A + 20U},{_tmp36B,_tmp36B,_tmp36B + 14U},{_tmp36C,_tmp36C,_tmp36C + 19U},{_tmp36D,_tmp36D,_tmp36D + 20U},{_tmp36E,_tmp36E,_tmp36E + 11U},{_tmp36F,_tmp36F,_tmp36F + 22U},{_tmp370,_tmp370,_tmp370 + 20U},{_tmp371,_tmp371,_tmp371 + 23U},{_tmp372,_tmp372,_tmp372 + 20U},{_tmp373,_tmp373,_tmp373 + 22U},{_tmp374,_tmp374,_tmp374 + 23U},{_tmp375,_tmp375,_tmp375 + 24U},{_tmp376,_tmp376,_tmp376 + 24U},{_tmp377,_tmp377,_tmp377 + 15U},{_tmp378,_tmp378,_tmp378 + 20U},{_tmp379,_tmp379,_tmp379 + 22U},{_tmp37A,_tmp37A,_tmp37A + 17U},{_tmp37B,_tmp37B,_tmp37B + 20U},{_tmp37C,_tmp37C,_tmp37C + 26U},{_tmp37D,_tmp37D,_tmp37D + 16U},{_tmp37E,_tmp37E,_tmp37E + 17U},{_tmp37F,_tmp37F,_tmp37F + 9U},{_tmp380,_tmp380,_tmp380 + 13U},{_tmp381,_tmp381,_tmp381 + 12U},{_tmp382,_tmp382,_tmp382 + 12U},{_tmp383,_tmp383,_tmp383 + 11U},{_tmp384,_tmp384,_tmp384 + 11U},{_tmp385,_tmp385,_tmp385 + 11U},{_tmp386,_tmp386,_tmp386 + 16U},{_tmp387,_tmp387,_tmp387 + 17U},{_tmp388,_tmp388,_tmp388 + 15U},{_tmp389,_tmp389,_tmp389 + 19U},{_tmp38A,_tmp38A,_tmp38A + 17U},{_tmp38B,_tmp38B,_tmp38B + 19U},{_tmp38C,_tmp38C,_tmp38C + 25U},{_tmp38D,_tmp38D,_tmp38D + 26U},{_tmp38E,_tmp38E,_tmp38E + 9U},{_tmp38F,_tmp38F,_tmp38F + 20U},{_tmp390,_tmp390,_tmp390 + 17U},{_tmp391,_tmp391,_tmp391 + 18U},{_tmp392,_tmp392,_tmp392 + 11U},{_tmp393,_tmp393,_tmp393 + 12U}};
# 1604
static short Cyc_yyr1[563U]={0,153,154,154,154,154,154,154,154,154,154,154,154,155,156,157,158,159,159,160,160,160,161,161,162,162,162,163,163,163,164,164,165,165,165,166,166,167,167,167,167,168,168,169,170,171,172,173,173,173,173,173,173,173,174,174,175,175,175,175,175,175,175,175,175,175,175,176,176,176,176,176,176,176,177,177,178,179,179,180,180,180,180,181,181,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,182,183,184,184,184,185,185,185,186,186,187,187,187,188,188,188,188,188,189,189,190,190,191,191,192,192,193,194,194,195,195,196,197,197,197,197,197,197,198,198,198,198,198,198,199,200,200,201,201,201,201,202,202,203,203,204,204,204,205,205,206,206,206,206,207,207,207,208,208,209,209,210,210,211,211,211,211,211,211,211,211,211,211,212,212,212,212,212,212,212,212,212,212,212,213,213,214,215,215,216,216,216,216,216,216,216,216,216,217,217,217,218,218,219,219,219,220,220,220,221,221,222,222,222,222,223,223,224,224,225,225,226,226,227,227,228,228,229,229,229,229,230,230,231,231,232,232,232,233,234,234,235,235,236,236,236,236,236,237,237,237,237,238,238,239,239,240,240,241,241,242,242,242,242,242,243,243,244,244,244,245,245,245,245,245,245,245,245,245,245,245,246,246,246,246,246,246,247,248,248,249,249,250,250,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,253,253,253,253,253,253,253,253,253,253,253,253,253,253,254,254,254,254,254,254,254,254,255,256,256,257,257,258,258,259,259,260,260,261,261,262,262,262,263,263,263,263,263,264,264,264,265,265,265,266,266,266,266,267,267,268,268,268,268,268,268,269,270,271,271,271,271,271,271,271,271,271,271,271,271,271,271,271,271,271,272,272,272,273,273,274,274,275,275,275,276,276,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,279,280,280,280,280,280,280,280,281,282,282,283,283,284,284,285,285,286,286,287,287,287,288,288,288,288,288,289,289,289,290,290,290,291,291,291,291,292,292,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,293,294,295,295,296,296,296,297,297,298,298,298,299,299,300,301,301,301,302,302,303,303,303,304,304,304,304,304,304,304,304,304,304,304,305,305,305,305,306,306,306,306,306,306,306,306,306,306,306,307,308,308,309,309,309,309,309,310,310,311,311,312,312,313,313,314,314};
# 1664
static short Cyc_yyr2[563U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1724
static short Cyc_yydefact[1138U]={0,34,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,127,128,63,0,0,97,0,0,73,0,0,165,104,106,0,0,0,13,14,0,0,0,553,232,555,554,556,0,218,0,100,0,218,217,1,0,0,0,0,32,0,0,33,0,56,65,59,83,61,94,95,0,98,0,0,176,0,201,204,99,180,125,71,70,64,0,113,0,58,552,0,553,548,549,550,551,0,125,0,0,392,0,0,0,255,0,394,395,43,45,0,0,0,0,0,0,0,0,166,0,0,0,215,0,0,0,0,216,0,0,0,2,0,0,0,0,47,0,133,134,136,57,66,60,62,129,557,558,125,125,0,54,0,0,36,0,234,0,189,177,202,0,208,209,213,214,0,212,211,210,223,204,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,537,538,498,0,0,0,0,0,518,516,517,0,421,423,437,445,447,449,451,453,455,458,463,466,469,473,0,475,519,536,534,553,404,0,0,0,0,405,0,0,403,50,0,0,125,0,0,0,143,139,141,275,277,0,0,52,0,0,8,9,0,125,559,560,233,108,0,0,0,181,101,253,0,250,10,11,0,3,0,5,0,48,0,0,0,36,0,130,131,156,124,0,163,0,0,0,0,0,0,0,0,0,0,0,0,553,305,307,0,315,309,0,313,298,299,300,0,301,302,303,0,55,36,136,35,37,282,0,240,256,0,0,236,234,0,220,0,0,0,225,74,224,205,0,118,114,0,0,0,483,0,0,495,439,473,0,440,441,0,0,0,0,0,0,0,0,0,0,0,476,477,499,494,0,479,0,0,0,0,480,478,0,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,427,428,429,430,431,432,433,434,435,436,426,0,481,0,525,526,0,0,0,540,0,125,396,0,0,0,418,553,560,0,0,0,0,271,414,419,0,416,0,0,393,411,412,0,409,257,0,0,0,0,278,0,248,144,149,145,147,140,142,234,0,284,276,285,562,561,0,103,105,0,0,107,123,80,79,0,77,219,182,234,252,178,284,254,190,191,0,102,16,30,44,0,46,0,135,137,259,258,36,38,120,132,0,0,0,151,152,159,0,125,125,171,0,0,0,0,0,553,0,0,0,344,345,346,0,0,348,0,0,0,316,310,136,314,308,306,39,0,188,241,0,0,0,247,235,242,159,0,0,0,236,187,222,221,183,220,0,0,226,75,126,119,444,116,112,0,0,0,0,553,260,265,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,539,546,0,545,422,446,0,448,450,452,454,456,457,461,462,459,460,464,465,467,468,470,471,472,425,424,524,521,0,523,0,0,0,407,408,0,274,0,415,269,272,402,0,270,406,399,0,49,0,400,0,279,0,150,146,148,0,236,0,220,0,286,0,234,0,297,281,0,0,125,0,0,0,143,0,125,0,234,0,200,179,251,0,22,4,6,40,0,155,138,156,0,0,154,236,164,173,172,0,0,167,0,0,0,323,0,0,0,0,0,0,343,347,0,0,0,311,304,0,41,283,234,0,244,0,0,161,237,0,159,240,228,184,206,207,226,203,482,0,0,0,261,0,266,485,0,0,0,0,0,535,490,493,0,0,496,500,0,0,474,542,0,0,522,520,0,0,0,0,273,417,420,410,413,401,280,249,159,0,287,288,220,0,0,236,220,0,0,51,236,553,0,76,78,0,192,0,0,236,0,220,0,0,0,17,23,153,0,157,129,162,174,171,171,0,0,0,0,0,0,0,0,0,0,0,0,0,323,349,0,312,42,236,0,245,243,0,185,0,161,236,0,227,531,0,530,0,262,267,0,0,0,0,0,442,443,524,523,505,0,544,527,0,547,438,541,543,0,397,398,161,159,290,296,159,0,289,220,0,129,0,81,193,199,159,0,198,194,220,0,0,0,0,0,0,0,170,169,317,323,0,0,0,0,0,0,351,352,354,356,358,360,362,364,367,372,375,378,382,384,390,391,0,0,320,329,0,0,0,0,0,0,0,0,0,0,350,230,246,0,238,186,229,234,484,0,0,268,486,487,0,0,492,491,0,511,505,501,503,497,528,0,293,161,161,159,291,53,0,0,161,159,195,31,25,0,0,27,0,7,158,122,0,0,0,323,0,388,0,0,385,323,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,386,323,0,331,0,0,0,339,0,0,0,0,0,0,322,0,0,236,533,532,0,0,0,0,512,511,508,506,0,502,529,292,295,161,121,0,196,161,26,24,28,0,0,19,175,318,319,0,0,0,0,323,325,355,0,357,359,361,363,365,366,370,371,368,369,373,374,376,377,379,380,381,0,324,330,332,333,0,341,340,0,335,0,0,0,160,239,231,0,0,0,0,0,514,513,0,507,504,294,0,197,29,18,20,0,321,387,0,383,326,0,323,334,342,336,337,0,264,263,488,0,510,0,509,82,21,0,353,323,327,338,0,515,389,328,489,0,0,0};
# 1841
static short Cyc_yydefgoto[162U]={1135,53,54,55,56,488,879,1041,791,792,963,670,57,321,58,305,59,490,60,492,61,151,62,63,557,244,474,475,245,66,260,246,68,174,175,69,172,70,282,283,136,137,138,284,247,456,503,504,505,680,824,71,72,685,686,687,73,506,74,480,75,76,169,170,77,121,553,336,723,643,78,644,547,714,539,543,544,450,329,269,102,103,570,495,571,430,431,432,248,322,323,645,462,308,309,310,311,312,313,806,314,315,892,893,894,895,896,897,898,899,900,901,902,903,904,905,906,907,433,442,443,434,435,436,316,208,410,209,562,210,211,212,213,214,215,216,217,218,219,220,221,368,369,846,943,944,1024,945,1026,1093,222,223,831,224,589,590,225,226,80,964,437,466};
# 1861
static short Cyc_yypact[1138U]={2842,- -32768,- -32768,- -32768,- -32768,- 55,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3656,426,- 57,- -32768,3656,3050,- -32768,- 38,- 1,- -32768,21,67,- 2,16,84,- -32768,- -32768,- 9,464,132,- -32768,131,- -32768,- -32768,- -32768,129,152,603,191,195,152,- -32768,- -32768,242,259,276,1316,- -32768,432,559,- -32768,733,3656,3656,3656,- -32768,3656,- -32768,- -32768,137,- -32768,- 38,3565,159,114,307,1215,- -32768,- -32768,292,313,380,- -32768,- 38,411,6980,- -32768,- -32768,3102,526,- -32768,- -32768,- -32768,- -32768,394,292,455,6980,- -32768,444,3102,460,512,518,- -32768,119,- -32768,- -32768,3816,3816,170,531,1316,1316,6980,284,- -32768,35,562,6980,- -32768,690,584,35,4216,- -32768,1316,1316,2985,- -32768,1316,2985,1316,2985,- -32768,609,565,- -32768,3430,- -32768,- -32768,- -32768,- -32768,4216,- -32768,- -32768,292,142,1799,- -32768,3565,733,613,3816,3736,5059,- -32768,159,- -32768,595,- -32768,- -32768,- -32768,- -32768,598,- -32768,- -32768,- -32768,4,1215,3816,- -32768,- -32768,622,636,641,- 38,7295,658,7393,6980,7145,689,691,697,704,710,715,717,728,743,749,756,7393,7393,- -32768,- -32768,767,7445,2544,772,7445,7445,- -32768,- -32768,- -32768,- 28,- -32768,- -32768,- 40,708,725,768,671,567,60,755,239,126,- -32768,952,7445,381,- 6,- -32768,774,58,- -32768,3102,37,793,1096,803,371,2803,- -32768,- -32768,804,6980,292,2803,784,3976,4216,4296,4216,628,- -32768,- 24,- 24,- -32768,806,792,- -32768,- -32768,448,292,- -32768,- -32768,- -32768,- -32768,39,798,802,- -32768,- -32768,781,524,- -32768,- -32768,- -32768,812,- -32768,813,- -32768,814,- -32768,690,5172,3565,613,817,4216,- -32768,568,819,- 38,820,815,110,818,4366,821,838,828,831,5285,2399,4366,199,825,- -32768,- -32768,830,1949,1949,733,1949,- -32768,- -32768,- -32768,840,- -32768,- -32768,- -32768,201,- -32768,613,839,- -32768,- -32768,827,- 7,856,- -32768,- 12,841,836,231,844,770,834,6980,3816,- -32768,858,- -32768,- -32768,- 7,- 38,- -32768,6980,857,2544,- -32768,4216,2544,- -32768,- -32768,- -32768,4479,- -32768,885,6980,6980,6980,6980,6980,6980,873,6980,4216,816,6980,- -32768,- -32768,- -32768,- -32768,853,- -32768,1949,854,525,6980,- -32768,- -32768,6980,- -32768,7445,6980,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,6980,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6980,- -32768,35,- -32768,- -32768,5398,35,6980,- -32768,860,292,- -32768,861,862,863,- -32768,3,394,35,6980,3102,337,- -32768,- -32768,- -32768,870,859,852,3102,- -32768,- -32768,- -32768,864,875,- -32768,385,1096,867,3816,- -32768,877,879,- -32768,4296,4296,4296,- -32768,- -32768,3257,5511,376,- -32768,188,- -32768,- -32768,- 12,- -32768,- -32768,872,896,- -32768,886,- -32768,881,883,887,- -32768,- -32768,3121,- -32768,248,967,- -32768,- -32768,- -32768,4216,- -32768,- -32768,972,- -32768,1316,- -32768,1316,- -32768,- -32768,- -32768,- -32768,613,- -32768,- -32768,- -32768,901,6980,894,892,- -32768,- 19,227,292,292,855,6980,6980,898,899,6980,890,1000,2249,907,- -32768,- -32768,- -32768,447,991,- -32768,5624,2099,2689,- -32768,- -32768,3430,- -32768,- -32768,- -32768,- -32768,3816,- -32768,- -32768,4216,903,4056,- -32768,- -32768,902,958,- 12,905,4136,836,- -32768,- -32768,- -32768,- -32768,770,910,45,326,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,912,913,914,937,924,- -32768,- -32768,736,5172,930,938,939,940,941,533,936,943,946,453,947,957,964,7243,- -32768,- -32768,950,965,- -32768,708,83,725,768,671,567,60,60,755,755,755,755,239,239,126,126,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,951,- -32768,120,3816,4943,4216,- -32768,4216,- -32768,954,- -32768,- -32768,- -32768,- -32768,1428,- -32768,- -32768,- -32768,2946,- -32768,968,- -32768,52,- -32768,4216,- -32768,- -32768,- -32768,962,836,963,770,959,188,3816,3896,5737,- -32768,- -32768,6980,976,292,7093,970,39,3289,971,292,3816,3736,5850,- -32768,248,- -32768,966,1057,- -32768,- -32768,- -32768,653,- -32768,- -32768,568,974,6980,- -32768,836,- -32768,- -32768,- -32768,981,- 38,470,537,544,6980,851,553,4366,977,5963,6076,493,- -32768,- -32768,985,987,979,1949,- -32768,3565,- -32768,827,989,3816,- -32768,990,- 12,1036,- -32768,986,958,62,- -32768,- -32768,- -32768,- -32768,326,- -32768,994,468,994,992,- -32768,4595,- -32768,- -32768,6980,6980,1097,6980,7145,- -32768,- -32768,- -32768,35,35,- -32768,995,988,4711,- -32768,- -32768,6980,6980,- -32768,- -32768,- 7,752,1012,1014,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,958,993,- -32768,- -32768,770,- 7,997,836,770,999,585,- -32768,836,1001,1003,- -32768,- -32768,1007,- -32768,- 7,1011,836,1013,770,1006,2985,1002,1087,- -32768,- -32768,6980,- -32768,4216,- -32768,1015,86,855,4366,1009,1016,1372,1017,1022,4366,6980,6189,583,6302,615,6415,851,- -32768,1026,- -32768,- -32768,836,235,- -32768,- -32768,1019,- -32768,1038,1036,836,4216,- -32768,- -32768,306,- -32768,6980,- -32768,- -32768,5172,1041,1043,1045,1052,- -32768,885,1048,1049,- 43,1053,- -32768,- -32768,757,- -32768,- -32768,- -32768,- -32768,4943,- -32768,- -32768,1036,958,- -32768,- -32768,958,1054,- -32768,770,1061,4216,1076,- -32768,- -32768,- -32768,958,1056,- -32768,- -32768,770,1064,600,1066,1316,1060,1068,4216,- -32768,- -32768,1169,851,1072,7543,1078,2689,7445,1062,- -32768,- 33,- -32768,1118,1074,741,796,75,805,489,165,- -32768,- -32768,- -32768,- -32768,1122,7445,1949,- -32768,- -32768,630,4366,644,6528,4366,651,6641,6754,688,1094,- -32768,- -32768,- -32768,6980,1099,- -32768,- -32768,989,- -32768,635,279,- -32768,- -32768,- -32768,4216,1200,- -32768,- -32768,1095,- 36,360,- -32768,- -32768,- -32768,- -32768,4827,- -32768,1036,1036,958,- -32768,- -32768,1103,1105,1036,958,- -32768,- -32768,- -32768,1109,1110,711,284,- -32768,- -32768,- -32768,670,4366,1112,851,2544,- -32768,4216,1106,- -32768,1599,7445,6980,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,7445,6980,- -32768,851,1116,- -32768,4366,4366,672,- -32768,4366,4366,673,4366,675,6867,- -32768,1114,- 12,836,- -32768,- -32768,2689,1115,1113,6980,1129,384,- -32768,- -32768,1136,- -32768,- -32768,- -32768,- -32768,1036,- -32768,1138,- -32768,1036,- -32768,- -32768,- -32768,284,1125,716,- -32768,- -32768,- -32768,1126,1120,1130,7445,851,- -32768,708,399,725,768,768,567,60,60,755,755,755,755,239,239,126,126,- -32768,- -32768,- -32768,417,- -32768,- -32768,- -32768,- -32768,4366,- -32768,- -32768,4366,- -32768,4366,4366,684,- -32768,- -32768,- -32768,1132,771,1123,4216,686,- -32768,1139,1136,- -32768,- -32768,- -32768,1134,- -32768,- -32768,- -32768,- -32768,284,- -32768,994,468,- -32768,- -32768,6980,1599,- -32768,- -32768,- -32768,- -32768,4366,- -32768,- -32768,- -32768,1137,- -32768,1146,- -32768,- -32768,- -32768,560,- -32768,851,- -32768,- -32768,1140,- -32768,- -32768,- -32768,- -32768,1265,1267,- -32768};
# 1978
static short Cyc_yypgoto[162U]={- -32768,298,- -32768,- -32768,- -32768,- -32768,- -32768,168,- -32768,- -32768,233,- -32768,- -32768,- 242,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 71,- 111,153,- -32768,- -32768,0,617,- -32768,138,- 215,1150,33,- -32768,- -32768,- 157,- -32768,327,1238,- 745,- -32768,- -32768,- -32768,1004,996,606,10,- -32768,- -32768,604,- 354,- 584,- -32768,- -32768,109,- -32768,- -32768,419,- 86,1208,- 419,333,- -32768,1117,- -32768,- -32768,1233,- 330,- -32768,564,- 131,- 147,- 114,- 450,274,578,588,- 459,- 495,- 115,- 454,- 132,- -32768,- 230,- 179,- 569,- 233,- -32768,865,54,- 101,- 88,- 124,- 275,- 210,- -32768,- -32768,- 58,- 301,- -32768,- 393,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,193,1063,- -32768,676,866,- -32768,266,847,- -32768,- 144,- 325,- 164,- 350,- 343,- 368,918,- 344,- 374,- 345,- 375,- 367,- 167,778,- -32768,- -32768,- -32768,- -32768,364,- -32768,- 890,285,- -32768,504,948,204,- -32768,- 381,- -32768,124,262,1,- 69,- 105,- 82};
# 2002
static short Cyc_yytable[7696U]={64,147,150,352,529,530,653,532,249,250,268,598,599,261,595,153,604,605,353,261,343,64,337,330,327,64,606,607,280,592,454,668,370,67,615,375,376,349,594,498,597,328,600,601,602,603,711,257,122,494,753,881,1025,540,67,411,472,64,67,379,81,678,667,64,64,64,979,64,150,463,585,146,941,64,157,41,535,86,306,941,317,281,517,339,44,708,463,715,667,524,67,683,684,318,668,942,67,67,67,718,67,377,1023,380,624,418,67,378,42,464,980,111,108,64,64,541,229,146,572,679,465,955,536,461,42,115,464,64,64,64,112,64,64,64,64,230,419,1096,65,64,335,- 559,113,482,763,702,67,67,258,64,105,64,473,109,387,388,259,422,157,65,67,67,67,65,67,67,67,67,467,987,988,229,67,423,83,538,849,675,87,542,721,479,67,560,67,648,117,762,326,465,230,713,389,390,765,65,465,- 234,561,110,- 234,65,65,65,1122,65,648,989,990,317,114,65,377,105,- 168,549,140,141,142,104,143,749,497,41,719,105,152,41,608,609,610,796,44,556,306,306,44,306,454,454,454,537,928,118,512,453,240,513,48,377,41,65,65,241,372,673,395,559,43,44,45,751,79,119,144,65,65,65,484,65,65,65,65,949,171,396,397,65,455,120,64,287,228,46,85,561,484,65,106,65,107,154,152,155,236,995,251,451,64,156,306,252,307,124,152,64,64,613,64,803,79,616,46,67,996,997,767,704,542,79,649,862,650,623,79,525,866,534,651,125,377,67,145,526,148,872,558,79,67,67,41,67,730,16,17,18,176,637,43,44,45,106,207,105,795,130,105,326,561,105,548,501,826,106,234,105,1030,1031,127,924,925,64,46,1035,930,79,79,929,145,465,663,256,664,393,79,128,572,394,665,79,79,79,661,79,79,79,79,41,564,542,565,566,817,129,67,43,44,45,377,159,857,254,255,268,79,661,581,1019,65,746,705,922,421,232,171,270,271,272,441,273,274,275,276,152,441,707,65,47,859,176,- 15,931,863,65,65,932,65,696,1097,51,52,87,1099,453,453,453,703,874,307,307,150,307,652,640,641,642,373,123,626,880,412,413,414,706,286,288,428,429,660,662,666,139,484,704,19,20,455,455,455,1027,64,106,64,971,106,173,836,106,942,835,377,674,484,106,950,445,439,951,484,65,458,634,572,1094,377,415,459,957,64,770,416,417,1023,67,307,67,231,64,377,483,752,64,786,784,953,561,771,1109,177,41,79,264,740,413,414,959,377,483,44,509,785,67,514,84,105,1110,715,841,131,132,67,769,105,523,67,542,1086,446,233,79,105,319,842,699,781,783,377,377,235,1046,460,258,830,470,471,1051,237,415,781,259,722,885,741,417,819,451,799,912,1032,800,481,1014,176,827,1036,851,934,1072,820,1000,373,1058,1059,373,1055,1056,1064,1065,507,813,832,836,377,625,835,578,1066,1067,65,1052,65,631,306,993,317,843,844,994,1054,229,1057,238,1060,1061,1062,1063,593,818,652,239,561,253,- 256,485,377,- 256,65,1108,230,486,587,662,377,385,386,65,377,666,736,65,852,152,801,377,754,484,755,1050,484,802,152,41,377,617,152,133,134,860,807,43,44,45,106,451,278,1132,262,319,47,932,106,870,501,64,1002,64,502,1006,106,916,51,52,377,41,377,836,41,1128,835,265,865,43,44,45,977,44,531,46,961,333,962,267,334,47,1133,277,67,48,67,320,483,919,999,483,377,51,52,619,258,1017,285,340,79,105,79,722,259,105,47,377,1044,267,458,483,341,1001,41,969,459,483,51,52,342,377,43,44,45,688,689,1003,377,692,561,47,697,367,1007,501,671,64,672,460,346,1074,1075,51,52,1077,1078,485,1080,377,377,41,377,1043,381,1076,1079,1127,1081,481,44,377,1012,377,1016,377,384,1115,47,1120,67,354,48,355,760,1018,1068,1069,1070,356,51,52,507,681,682,1039,357,306,1040,65,1102,65,358,1103,41,391,392,359,452,360,457,551,552,44,307,88,152,135,683,684,361,47,728,729,1111,48,542,1112,382,1113,1114,804,805,51,52,362,146,64,853,854,1107,363,189,947,948,983,984,285,364,106,985,986,41,106,1042,991,992,1117,377,420,43,44,45,1129,374,306,883,884,64,47,67,424,383,478,438,444,448,468,774,459,483,51,52,469,476,65,105,41,91,196,197,92,93,94,483,477,44,483,487,489,491,67,976,499,508,510,798,511,347,519,515,201,520,518,345,521,348,350,350,810,812,527,528,1126,146,263,533,538,279,465,365,366,546,554,545,350,64,550,350,350,46,776,379,563,579,584,629,586,630,1020,618,507,620,621,622,908,628,632,350,832,636,332,633,638,654,507,639,655,507,67,398,656,41,657,659,65,658,669,676,677,43,44,45,46,691,1047,693,1048,694,47,690,698,1042,501,700,709,678,306,716,146,725,51,52,720,712,724,65,726,727,79,399,400,401,402,403,404,405,406,407,408,- 559,307,267,731,106,732,733,734,735,737,743,1087,913,915,348,918,738,921,41,739,742,409,267,747,750,876,43,44,45,267,744,789,748,756,761,764,766,933,768,478,775,790,146,779,782,459,794,797,64,808,814,815,816,326,65,823,821,825,350,745,839,847,833,855,496,856,858,877,867,307,861,845,267,878,886,88,868,145,864,79,869,67,1119,267,871,875,873,882,911,887,923,926,267,910,234,350,927,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,935,966,936,937,555,938,1005,939,940,1009,1011,954,946,952,956,958,960,965,89,967,968,970,496,972,978,573,574,575,576,577,425,350,580,426,91,583,975,92,93,94,981,427,44,982,998,588,1013,96,591,1021,97,145,1015,1022,98,1033,99,100,1034,428,429,1037,1038,373,1045,1049,1073,101,1092,611,1053,1090,65,1084,1089,941,1098,1101,1104,1105,267,612,1118,1106,1116,1131,588,307,1071,1136,452,1137,1121,1123,267,1124,1130,1100,266,1134,780,116,1083,500,350,793,493,158,126,1088,829,338,1091,1085,160,161,162,163,164,828,627,165,166,167,822,596,145,168,447,758,647,1028,909,1095,1125,582,635,0,0,0,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,350,145,27,0,28,29,30,0,0,588,0,588,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,888,889,0,0,39,285,40,0,0,0,0,0,0,0,0,0,88,0,0,0,0,0,496,0,0,0,0,0,0,0,0,0,41,0,0,0,0,267,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,350,51,52,0,0,0,0,0,0,496,0,0,88,89,0,285,0,0,0,0,0,0,0,0,0,0,0,0,227,91,0,267,92,93,94,0,95,44,0,0,0,773,96,0,0,97,0,778,0,890,0,99,100,0,0,788,0,350,0,891,0,0,101,204,0,0,205,206,0,89,350,0,0,0,0,0,0,0,0,0,588,757,0,0,426,91,0,0,92,93,94,0,427,44,0,0,0,0,96,0,0,97,0,0,0,98,0,99,100,0,428,429,0,0,0,350,0,0,101,496,0,0,837,838,0,840,0,0,0,0,0,0,0,0,0,496,0,0,850,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,804,805,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,88,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,974,189,0,350,190,191,38,192,0,0,0,0,0,0,0,0,0,496,39,193,40,350,0,194,195,0,0,0,0,0,0,0,0,0,0,496,0,0,350,0,0,0,0,0,0,0,301,91,196,197,92,93,94,42,43,44,45,46,198,302,149,0,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,0,0,0,0,0,350,0,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,496,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,289,350,290,291,292,293,294,295,296,297,22,23,24,298,88,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,350,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,301,91,196,197,92,93,94,42,43,44,45,46,198,302,149,303,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,88,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,301,91,196,197,92,93,94,42,43,44,45,46,198,302,149,0,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,88,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,516,91,196,197,92,93,94,42,43,44,45,46,198,302,149,0,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,298,88,26,181,0,0,0,0,0,182,27,0,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,42,43,44,45,46,198,695,0,0,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,298,88,0,0,0,0,0,0,0,0,27,0,0,0,30,183,184,185,186,187,0,31,32,0,0,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,42,43,44,45,46,198,0,0,0,0,199,0,0,0,347,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,88,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,0,31,32,0,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,42,43,44,45,46,198,0,371,0,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,88,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,0,31,32,0,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,42,43,44,45,46,198,0,0,0,0,199,0,0,0,200,0,49,304,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,- 12,1,88,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,89,0,31,32,33,0,0,34,35,0,36,37,440,0,38,227,91,0,0,92,93,94,0,95,44,0,39,0,40,96,0,0,97,0,0,0,98,0,99,100,0,0,0,0,0,0,0,0,0,101,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,88,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,89,0,31,32,33,0,0,34,35,0,36,37,759,0,38,227,91,0,0,92,93,94,0,95,44,0,39,0,40,96,0,0,97,0,0,0,98,0,99,100,0,0,0,0,0,0,0,88,0,101,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,0,0,0,89,0,0,0,0,0,0,0,0,0,22,23,24,0,0,90,91,0,0,92,93,94,0,95,44,0,0,30,0,96,0,0,97,0,31,32,98,0,99,100,0,0,0,0,0,0,38,0,0,101,0,0,89,0,0,0,0,0,39,0,40,0,0,0,0,0,0,227,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,324,97,0,41,0,98,0,99,100,0,42,43,44,45,46,0,326,0,101,0,47,0,0,0,478,0,49,50,0,0,459,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,7,8,9,10,11,12,13,14,15,16,17,18,19,20,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,22,23,24,0,38,0,0,0,0,0,0,0,0,0,0,0,39,30,40,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,324,0,0,0,0,0,0,39,0,40,42,43,0,45,46,0,326,0,0,0,47,0,0,0,458,0,49,50,0,0,459,0,51,52,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,501,0,49,50,0,0,0,0,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,279,0,0,0,0,49,50,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,324,0,0,325,0,0,0,22,23,24,42,43,0,45,46,0,326,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,242,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,243,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,324,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,326,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,449,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,710,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,717,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,178,179,289,0,290,291,292,293,294,295,296,297,0,0,0,180,88,0,181,299,0,0,0,0,182,42,300,0,0,46,183,184,185,186,187,0,0,0,0,188,0,49,50,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,516,91,196,197,92,93,94,0,0,44,0,0,198,302,149,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,567,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,568,91,196,197,92,93,94,0,259,44,0,0,198,0,351,569,0,199,0,0,0,200,0,0,201,0,428,429,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,568,91,196,197,92,93,94,0,259,44,0,0,198,0,351,834,0,199,0,0,0,200,0,0,201,0,428,429,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,568,91,196,197,92,93,94,0,259,44,0,0,198,0,351,848,0,199,0,0,0,200,0,0,201,0,428,429,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,568,91,196,197,92,93,94,0,259,44,0,0,198,0,351,1029,0,199,0,0,0,200,0,0,201,0,428,429,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,568,91,196,197,92,93,94,0,259,44,0,0,198,0,351,0,0,199,0,0,0,200,0,0,201,0,428,429,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,331,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,351,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,522,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,614,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,646,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,701,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,772,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,787,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,809,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,811,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,914,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,917,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,920,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,1004,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,1008,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,1010,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,1082,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,88,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,88,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,777,91,196,197,92,93,94,189,0,44,190,191,198,192,0,0,0,199,0,0,0,200,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,178,179,198,0,351,0,0,199,0,0,0,200,0,0,201,180,88,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,88,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,41,91,196,197,92,93,94,189,0,44,190,191,198,192,745,0,0,199,0,0,0,200,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,178,179,198,0,0,0,0,199,0,0,0,344,0,0,201,180,88,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,88,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,41,91,196,197,92,93,94,189,0,44,190,191,198,192,0,0,0,199,0,0,0,347,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,178,179,198,0,0,0,0,199,0,0,0,200,0,0,201,180,88,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,196,197,92,93,94,0,0,44,0,0,198,0,0,0,0,199,0,0,0,973,0,0,201,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206};
# 2775
static short Cyc_yycheck[7696U]={0,70,73,182,305,306,465,308,109,110,125,385,386,118,382,73,391,392,182,124,177,21,169,155,155,25,393,394,139,379,245,485,199,0,415,202,203,181,381,281,384,155,387,388,389,390,541,116,48,279,619,796,942,65,21,222,17,57,25,99,115,80,481,63,64,65,99,67,139,93,371,70,115,73,74,113,318,134,149,115,151,139,292,171,122,539,93,546,507,299,57,5,6,151,548,138,63,64,65,549,67,129,138,143,429,111,73,135,120,133,143,113,113,113,114,127,113,116,351,138,144,866,129,247,120,134,133,127,128,129,132,131,132,133,134,132,142,1027,0,139,136,138,126,267,639,526,113,114,113,149,26,151,113,132,94,95,121,120,158,21,127,128,129,25,131,132,133,134,250,94,95,113,139,136,21,113,745,502,25,326,135,267,149,340,151,460,54,135,126,144,132,545,132,133,644,57,144,135,342,132,138,63,64,65,1094,67,481,132,133,280,126,73,129,89,128,329,63,64,65,26,67,138,280,113,554,101,73,113,395,396,397,681,122,334,305,306,122,308,453,454,455,323,826,112,134,245,127,137,134,129,113,113,114,134,200,497,130,339,121,122,123,141,0,134,127,127,128,129,267,131,132,133,134,857,132,149,150,139,245,127,280,139,89,124,22,429,285,149,26,151,28,132,139,134,101,130,126,243,298,140,371,131,149,112,151,305,306,412,308,690,48,416,124,280,149,150,646,527,465,57,132,771,134,428,62,126,776,126,140,134,129,298,70,134,72,785,336,75,305,306,113,308,572,17,18,19,84,448,121,122,123,89,86,229,679,57,232,126,502,235,129,134,716,101,98,241,950,951,126,819,135,371,124,957,828,113,114,827,116,144,132,115,134,144,122,126,619,148,140,127,128,129,478,131,132,133,134,113,344,546,346,347,703,127,371,121,122,123,129,76,764,113,114,528,152,501,362,138,280,586,531,814,229,96,132,127,128,129,235,131,132,133,134,280,241,536,298,130,768,177,127,135,772,305,306,139,308,518,1032,142,143,298,1036,453,454,455,527,787,305,306,531,308,462,453,454,455,200,48,131,794,89,90,91,531,147,148,139,140,478,478,480,62,481,693,20,21,453,454,455,129,490,229,492,886,232,115,729,235,138,729,129,501,501,241,858,239,135,861,507,371,134,126,745,129,129,134,140,871,518,650,139,140,138,490,371,492,132,527,129,267,618,531,664,664,864,679,650,138,127,113,278,122,89,90,91,875,129,285,122,287,664,518,290,127,430,138,1015,736,126,127,527,649,438,297,531,712,1016,240,113,307,446,152,736,126,660,663,129,129,134,972,247,113,114,135,257,978,126,134,674,121,557,801,139,140,708,541,126,807,952,129,267,926,340,717,958,749,836,1000,709,910,344,985,986,347,982,983,991,992,285,126,725,854,129,430,854,359,993,994,490,979,492,438,703,144,705,740,741,148,981,113,984,129,987,988,989,990,380,705,648,131,794,120,126,129,129,129,518,1050,132,135,135,660,129,96,97,527,129,667,135,531,752,518,135,129,620,674,622,978,677,135,527,113,129,417,531,126,127,769,135,121,122,123,430,639,129,135,134,278,130,139,438,783,134,703,914,705,138,917,446,126,142,143,129,113,129,948,113,1110,948,135,135,121,122,123,891,122,307,124,128,134,130,125,134,130,1127,126,703,134,705,126,478,126,909,481,129,142,143,420,113,114,144,129,490,629,492,722,121,633,130,129,970,155,134,501,128,135,113,882,140,507,142,143,131,129,121,122,123,511,512,135,129,515,926,130,518,18,135,134,490,789,492,458,134,1003,1004,142,143,1007,1008,129,1010,129,129,113,129,135,98,135,135,1110,135,478,122,129,126,129,930,129,147,135,130,135,789,134,134,134,633,932,995,996,997,134,142,143,501,508,509,126,134,910,129,703,126,705,134,129,113,92,93,134,244,134,246,83,84,122,703,41,705,126,5,6,134,130,128,129,1076,134,1015,1079,145,1081,1082,22,23,142,143,134,877,879,128,129,1049,134,68,128,129,146,147,283,134,629,96,97,113,633,965,92,93,128,129,127,121,122,123,1115,134,978,799,800,910,130,879,120,146,134,113,113,134,113,654,140,660,142,143,133,128,789,804,113,114,115,116,117,118,119,674,135,122,677,128,128,128,910,890,128,127,127,686,134,134,113,134,137,126,134,178,126,180,181,182,695,696,138,134,1109,965,120,128,113,131,144,194,195,138,141,135,199,978,135,202,203,124,656,99,128,113,134,129,135,138,937,132,660,133,133,133,804,128,135,222,1106,135,156,129,128,134,674,129,113,677,978,60,127,113,134,129,879,135,47,126,129,121,122,123,124,127,973,138,975,30,130,134,126,1103,134,45,134,80,1110,135,1040,129,142,143,135,144,135,910,135,113,789,100,101,102,103,104,105,106,107,108,109,138,910,458,135,804,129,129,129,129,135,115,1019,808,809,298,811,135,813,113,135,135,131,478,135,135,789,121,122,123,485,128,127,129,141,128,135,135,833,141,134,126,46,1103,135,135,140,134,128,1110,134,127,126,135,126,978,81,128,133,342,127,25,135,132,113,279,113,135,127,129,978,135,138,528,48,127,41,135,877,141,879,135,1110,1090,539,135,141,135,134,128,135,126,134,548,138,890,379,120,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,135,879,135,134,333,129,916,135,135,919,920,126,135,135,114,135,128,127,98,135,128,28,351,127,138,354,355,356,357,358,110,429,361,113,114,364,134,117,118,119,98,121,122,145,98,374,128,127,377,25,130,965,129,134,134,128,136,137,129,139,140,128,128,973,128,135,126,147,115,398,980,134,1110,135,135,115,114,128,128,135,650,410,135,129,128,115,415,1110,998,0,660,0,129,135,664,1103,135,1040,124,135,659,39,1012,283,502,677,278,75,51,1019,722,170,1022,1015,75,76,77,78,79,717,431,82,83,84,712,383,1040,88,241,629,459,943,804,1024,1106,363,446,- 1,- 1,- 1,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,586,1103,50,- 1,52,53,54,- 1,- 1,526,- 1,528,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,85,796,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,572,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,828,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,679,142,143,- 1,- 1,- 1,- 1,- 1,- 1,619,- 1,- 1,41,98,- 1,866,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,882,117,118,119,- 1,121,122,- 1,- 1,- 1,651,127,- 1,- 1,130,- 1,657,- 1,134,- 1,136,137,- 1,- 1,665,- 1,736,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,98,749,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,690,110,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,139,140,- 1,- 1,- 1,794,- 1,- 1,147,729,- 1,- 1,732,733,- 1,735,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,745,- 1,- 1,748,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,888,68,- 1,891,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,836,85,86,87,909,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,854,- 1,- 1,926,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,979,- 1,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,997,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,948,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,1049,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,1109,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,54,- 1,127,- 1,- 1,130,- 1,61,62,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,147,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,110,130,- 1,113,- 1,134,- 1,136,137,- 1,120,121,122,123,124,- 1,126,- 1,147,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,8,9,10,11,12,13,14,15,16,17,18,19,20,21,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,54,87,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,131,- 1,- 1,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,37,38,39,40,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,120,51,- 1,- 1,124,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,136,137,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,32,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp41E[8U]="stdcall";static char _tmp41F[6U]="cdecl";static char _tmp420[9U]="fastcall";static char _tmp421[9U]="noreturn";static char _tmp422[6U]="const";static char _tmp423[8U]="aligned";static char _tmp424[7U]="packed";static char _tmp425[7U]="shared";static char _tmp426[7U]="unused";static char _tmp427[5U]="weak";static char _tmp428[10U]="dllimport";static char _tmp429[10U]="dllexport";static char _tmp42A[23U]="no_instrument_function";static char _tmp42B[12U]="constructor";static char _tmp42C[11U]="destructor";static char _tmp42D[22U]="no_check_memory_usage";static char _tmp42E[5U]="pure";static char _tmp42F[14U]="always_inline";static char _tmp430[9U]="no_throw";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp394=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp394;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp765;(_tmp765.YYINITIALSVAL).tag=72U,(_tmp765.YYINITIALSVAL).val=0;_tmp765;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp6A9=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp6A9),sizeof(short),_tmp6A9);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp6A8=200U;struct Cyc_Yystacktype*_tmp6A7=({struct _RegionHandle*_tmp81E=yyregion;_region_malloc(_tmp81E,_check_times(_tmp6A8,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp764=200U;unsigned i;for(i=0;i < _tmp764;++ i){(_tmp6A7[i]).v=yylval,({struct Cyc_Yyltype _tmp81F=Cyc_yynewloc();(_tmp6A7[i]).l=_tmp81F;});}}0;});_tmp6A7;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp821=({const char*_tmp395="parser stack overflow";_tag_fat(_tmp395,sizeof(char),22U);});int _tmp820=yystate;Cyc_yyerror(_tmp821,_tmp820,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp399=(unsigned)yystacksize;short*_tmp398=({struct _RegionHandle*_tmp822=yyregion;_region_malloc(_tmp822,_check_times(_tmp399,sizeof(short)));});({{unsigned _tmp71B=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp71B;++ i){
i <= (unsigned)yyssp_offset?_tmp398[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp398[i]=0);}}0;});_tag_fat(_tmp398,sizeof(short),_tmp399);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp397=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp396=({struct _RegionHandle*_tmp823=yyregion;_region_malloc(_tmp823,_check_times(_tmp397,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp71A=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp71A;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp396[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp396[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp396,sizeof(struct Cyc_Yystacktype),_tmp397);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1138U,sizeof(short),yystate));
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
if((yyn < 0 || yyn > 7695)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7696U,sizeof(short),yyn))!= yychar1)goto yydefault;
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
if(yyn == 1137){
int _tmp39A=0;_npop_handler(0U);return _tmp39A;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp824=({struct Cyc_Yystacktype _tmp71C;_tmp71C.v=yylval,_tmp71C.l=yylloc;_tmp71C;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp824;});
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
{int _tmp39B=yyn;switch(_tmp39B){case 1U: _LL1: _LL2:
# 1144 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1147
goto _LL0;case 2U: _LL3: _LL4:
# 1150 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp825=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp825,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1154 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39E=_cycalloc(sizeof(*_tmp39E));({struct Cyc_Absyn_Decl*_tmp829=({struct Cyc_Absyn_Decl*_tmp39D=_cycalloc(sizeof(*_tmp39D));({void*_tmp828=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->tag=10U,({struct _tuple0*_tmp827=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39C->f1=_tmp827;}),({struct Cyc_List_List*_tmp826=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39C->f2=_tmp826;});_tmp39C;});_tmp39D->r=_tmp828;}),_tmp39D->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39D;});_tmp39E->hd=_tmp829;}),_tmp39E->tl=0;_tmp39E;}));
Cyc_Lex_leave_using();
# 1157
goto _LL0;case 4U: _LL7: _LL8:
# 1158 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_Decl*_tmp82E=({struct Cyc_Absyn_Decl*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({void*_tmp82D=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->tag=10U,({struct _tuple0*_tmp82C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39F->f1=_tmp82C;}),({struct Cyc_List_List*_tmp82B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39F->f2=_tmp82B;});_tmp39F;});_tmp3A0->r=_tmp82D;}),_tmp3A0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A0;});_tmp3A1->hd=_tmp82E;}),({struct Cyc_List_List*_tmp82A=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A1->tl=_tmp82A;});_tmp3A1;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1161
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct Cyc_Absyn_Decl*_tmp833=({struct Cyc_Absyn_Decl*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({void*_tmp832=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->tag=9U,({struct _fat_ptr*_tmp831=({struct _fat_ptr*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct _fat_ptr _tmp830=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A2=_tmp830;});_tmp3A2;});_tmp3A3->f1=_tmp831;}),({struct Cyc_List_List*_tmp82F=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A3->f2=_tmp82F;});_tmp3A3;});_tmp3A4->r=_tmp832;}),_tmp3A4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A4;});_tmp3A5->hd=_tmp833;}),_tmp3A5->tl=0;_tmp3A5;}));
Cyc_Lex_leave_namespace();
# 1164
goto _LL0;case 6U: _LLB: _LLC:
# 1165 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({struct Cyc_Absyn_Decl*_tmp839=({struct Cyc_Absyn_Decl*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({void*_tmp838=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->tag=9U,({struct _fat_ptr*_tmp837=({struct _fat_ptr*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct _fat_ptr _tmp836=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A6=_tmp836;});_tmp3A6;});_tmp3A7->f1=_tmp837;}),({struct Cyc_List_List*_tmp835=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A7->f2=_tmp835;});_tmp3A7;});_tmp3A8->r=_tmp838;}),_tmp3A8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A8;});_tmp3A9->hd=_tmp839;}),({struct Cyc_List_List*_tmp834=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A9->tl=_tmp834;});_tmp3A9;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1167 "parse.y"
int _tmp3AA=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3AA;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3AB=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp15=_tmp3AB;struct _tuple28*_tmp3AC=_stmttmp15;unsigned _tmp3AE;struct Cyc_List_List*_tmp3AD;_LL464: _tmp3AD=_tmp3AC->f1;_tmp3AE=_tmp3AC->f2;_LL465: {struct Cyc_List_List*exs=_tmp3AD;unsigned wc=_tmp3AE;
struct Cyc_List_List*_tmp3AF=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3AF;
if(exs != 0 && hides != 0)
({void*_tmp3B0=0U;({unsigned _tmp83B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp83A=({const char*_tmp3B1="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3B1,sizeof(char),65U);});Cyc_Warn_err(_tmp83B,_tmp83A,_tag_fat(_tmp3B0,sizeof(void*),0U));});});
# 1174
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1177
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3B2=0U;({unsigned _tmp83D=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp83C=({const char*_tmp3B3="expecting \"C include\"";_tag_fat(_tmp3B3,sizeof(char),22U);});Cyc_Warn_err(_tmp83D,_tmp83C,_tag_fat(_tmp3B2,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct Cyc_Absyn_Decl*_tmp842=({struct Cyc_Absyn_Decl*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({void*_tmp841=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->tag=12U,({struct Cyc_List_List*_tmp840=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B5->f1=_tmp840;}),_tmp3B5->f2=cycdecls,_tmp3B5->f3=exs,({struct _tuple10*_tmp83F=({struct _tuple10*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->f1=wc,_tmp3B4->f2=hides;_tmp3B4;});_tmp3B5->f4=_tmp83F;});_tmp3B5;});_tmp3B6->r=_tmp841;}),_tmp3B6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B6;});_tmp3B7->hd=_tmp842;}),({struct Cyc_List_List*_tmp83E=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B7->tl=_tmp83E;});_tmp3B7;}));}else{
# 1183
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({struct Cyc_Absyn_Decl*_tmp846=({struct Cyc_Absyn_Decl*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({void*_tmp845=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->tag=11U,({struct Cyc_List_List*_tmp844=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B8->f1=_tmp844;});_tmp3B8;});_tmp3B9->r=_tmp845;}),_tmp3B9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B9;});_tmp3BA->hd=_tmp846;}),({struct Cyc_List_List*_tmp843=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BA->tl=_tmp843;});_tmp3BA;}));}}else{
# 1187
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({struct Cyc_Absyn_Decl*_tmp84B=({struct Cyc_Absyn_Decl*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({void*_tmp84A=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->tag=12U,({struct Cyc_List_List*_tmp849=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BC->f1=_tmp849;}),_tmp3BC->f2=cycdecls,_tmp3BC->f3=exs,({struct _tuple10*_tmp848=({struct _tuple10*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->f1=wc,_tmp3BB->f2=hides;_tmp3BB;});_tmp3BC->f4=_tmp848;});_tmp3BC;});_tmp3BD->r=_tmp84A;}),_tmp3BD->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BD;});_tmp3BE->hd=_tmp84B;}),({struct Cyc_List_List*_tmp847=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BE->tl=_tmp847;});_tmp3BE;}));}
# 1190
goto _LL0;}}case 8U: _LLF: _LL10:
# 1191 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_Absyn_Decl*_tmp84D=({struct Cyc_Absyn_Decl*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3BF->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BF;});_tmp3C0->hd=_tmp84D;}),({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C0->tl=_tmp84C;});_tmp3C0;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1193 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Decl*_tmp84F=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C1;});_tmp3C2->hd=_tmp84F;}),({struct Cyc_List_List*_tmp84E=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C2->tl=_tmp84E;});_tmp3C2;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1195 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct Cyc_Absyn_Decl*_tmp851=({struct Cyc_Absyn_Decl*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C3->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C3;});_tmp3C4->hd=_tmp851;}),({struct Cyc_List_List*_tmp850=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C4->tl=_tmp850;});_tmp3C4;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1197 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct Cyc_Absyn_Decl*_tmp853=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3C5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C5;});_tmp3C6->hd=_tmp853;}),({struct Cyc_List_List*_tmp852=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C6->tl=_tmp852;});_tmp3C6;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1198 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1203 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1208 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1213 "parse.y"
struct _fat_ptr _tmp3C7=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3C7;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp854=(struct _fat_ptr)two;Cyc_strcmp(_tmp854,({const char*_tmp3C8="C";_tag_fat(_tmp3C8,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp855=(struct _fat_ptr)two;Cyc_strcmp(_tmp855,({const char*_tmp3C9="C include";_tag_fat(_tmp3C9,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1220
({void*_tmp3CA=0U;({unsigned _tmp857=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp856=({const char*_tmp3CB="expecting \"C\" or \"C include\"";_tag_fat(_tmp3CB,sizeof(char),29U);});Cyc_Warn_err(_tmp857,_tmp856,_tag_fat(_tmp3CA,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1224
goto _LL0;}case 16U: _LL1F: _LL20:
# 1227 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1231 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1232 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1236 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));({struct _tuple0*_tmp858=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CC->hd=_tmp858;}),_tmp3CC->tl=0;_tmp3CC;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1237 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct _tuple0*_tmp859=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CD->hd=_tmp859;}),_tmp3CD->tl=0;_tmp3CD;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1239 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct _tuple0*_tmp85B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CE->hd=_tmp85B;}),({struct Cyc_List_List*_tmp85A=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3CE->tl=_tmp85A;});_tmp3CE;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1243 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->f1=0,_tmp3CF->f2=0U;_tmp3CF;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1244 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1248 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct Cyc_List_List*_tmp85C=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D0->f1=_tmp85C;}),_tmp3D0->f2=0U;_tmp3D0;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1249 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=0,_tmp3D1->f2=0U;_tmp3D1;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1250 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=0,_tmp3D2->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D2;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1254 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _tuple32*_tmp85E=({struct _tuple32*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85D=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D3->f2=_tmp85D;}),_tmp3D3->f3=0;_tmp3D3;});_tmp3D4->hd=_tmp85E;}),_tmp3D4->tl=0;_tmp3D4;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1255 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _tuple32*_tmp860=({struct _tuple32*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85F=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D5->f2=_tmp85F;}),_tmp3D5->f3=0;_tmp3D5;});_tmp3D6->hd=_tmp860;}),_tmp3D6->tl=0;_tmp3D6;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1257 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));({struct _tuple32*_tmp863=({struct _tuple32*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp862=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D7->f2=_tmp862;}),_tmp3D7->f3=0;_tmp3D7;});_tmp3D8->hd=_tmp863;}),({struct Cyc_List_List*_tmp861=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D8->tl=_tmp861;});_tmp3D8;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1261 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1262 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1266 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({struct Cyc_Absyn_Decl*_tmp866=({void*_tmp865=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp864=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3D9->f1=_tmp864;});_tmp3D9;});Cyc_Absyn_new_decl(_tmp865,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3DA->hd=_tmp866;}),_tmp3DA->tl=0;_tmp3DA;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1267 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1268 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1277 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp869=yyr;struct Cyc_Parse_Declarator _tmp868=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp867=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp869,0,_tmp868,0,_tmp867,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1279 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DB=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DB;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp86C=yyr;struct Cyc_Parse_Declarator _tmp86B=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp86A=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp86C,& d,_tmp86B,0,_tmp86A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1292 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp870=yyr;struct Cyc_Parse_Declarator _tmp86F=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp86E=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp86D=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp870,0,_tmp86F,_tmp86E,_tmp86D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1294 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DC;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp874=yyr;struct Cyc_Parse_Declarator _tmp873=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp872=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp871=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp874,& d,_tmp873,_tmp872,_tmp871,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1302 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DD=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DD;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp877=yyr;struct Cyc_Parse_Declarator _tmp876=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp875=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp877,& d,_tmp876,0,_tmp875,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1305 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DE=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DE;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp87B=yyr;struct Cyc_Parse_Declarator _tmp87A=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp879=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp878=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp87B,& d,_tmp87A,_tmp879,_tmp878,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1310 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1313
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1316
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));({struct _fat_ptr _tmp87C=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3DF=_tmp87C;});_tmp3DF;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1319
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1325 "parse.y"
int _tmp3E0=((yyyvsp[0]).l).first_line;int location=_tmp3E0;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87E=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp87D=(unsigned)location;Cyc_Parse_make_declarations(_tmp87E,0,_tmp87D,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1328 "parse.y"
int _tmp3E1=((yyyvsp[0]).l).first_line;int location=_tmp3E1;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp881=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp880=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp87F=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp881,_tmp880,_tmp87F,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1332
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct Cyc_Absyn_Decl*_tmp884=({struct Cyc_Absyn_Pat*_tmp883=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp882=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp883,_tmp882,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E2->hd=_tmp884;}),_tmp3E2->tl=0;_tmp3E2;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1334 "parse.y"
struct Cyc_List_List*_tmp3E3=0;struct Cyc_List_List*vds=_tmp3E3;
{struct Cyc_List_List*_tmp3E4=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3E4;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3E5=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3E5;
struct _tuple0*qv=({struct _tuple0*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));({union Cyc_Absyn_Nmspace _tmp885=Cyc_Absyn_Rel_n(0);_tmp3E8->f1=_tmp885;}),_tmp3E8->f2=id;_tmp3E8;});
struct Cyc_Absyn_Vardecl*_tmp3E6=({struct _tuple0*_tmp886=qv;Cyc_Absyn_new_vardecl(0U,_tmp886,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3E6;
vds=({struct Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->hd=vd,_tmp3E7->tl=vds;_tmp3E7;});}}
# 1341
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct Cyc_Absyn_Decl*_tmp887=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3E9->hd=_tmp887;}),_tmp3E9->tl=0;_tmp3E9;}));
# 1344
goto _LL0;}case 51U: _LL61: _LL62: {
# 1347 "parse.y"
struct _fat_ptr _tmp3EA=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3EA;
Cyc_Parse_tvar_ok(three,(unsigned)((yyyvsp[2]).l).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({struct _fat_ptr*_tmp889=({struct _fat_ptr*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));*_tmp3EE=three;_tmp3EE;});_tmp3EF->name=_tmp889;}),_tmp3EF->identity=- 1,({void*_tmp888=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3EF->kind=_tmp888;});_tmp3EF;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp88D=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp88C=({struct _tuple0*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp88B=({struct _fat_ptr*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct _fat_ptr _tmp88A=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3EC=_tmp88A;});_tmp3EC;});_tmp3ED->f2=_tmp88B;});_tmp3ED;});Cyc_Absyn_new_vardecl(_tmp88D,_tmp88C,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct Cyc_Absyn_Decl*_tmp88E=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EB->hd=_tmp88E;}),_tmp3EB->tl=0;_tmp3EB;}));
# 1354
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1356
struct _fat_ptr _tmp3F0=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3F0;
if(({struct _fat_ptr _tmp88F=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp88F,({const char*_tmp3F1="H";_tag_fat(_tmp3F1,sizeof(char),2U);}));})== 0)
({void*_tmp3F2=0U;({unsigned _tmp891=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp890=({const char*_tmp3F3="bad occurrence of heap region `H";_tag_fat(_tmp3F3,sizeof(char),33U);});Cyc_Warn_err(_tmp891,_tmp890,_tag_fat(_tmp3F2,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp892=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp892,({const char*_tmp3F4="U";_tag_fat(_tmp3F4,sizeof(char),2U);}));})== 0)
({void*_tmp3F5=0U;({unsigned _tmp894=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp893=({const char*_tmp3F6="bad occurrence of unique region `U";_tag_fat(_tmp3F6,sizeof(char),35U);});Cyc_Warn_err(_tmp894,_tmp893,_tag_fat(_tmp3F5,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct _fat_ptr*_tmp898=({struct _fat_ptr*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));({struct _fat_ptr _tmp897=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FC=({struct Cyc_String_pa_PrintArg_struct _tmp71D;_tmp71D.tag=0U,_tmp71D.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp71D;});void*_tmp3FA[1U];_tmp3FA[0]=& _tmp3FC;({struct _fat_ptr _tmp896=({const char*_tmp3FB="`%s";_tag_fat(_tmp3FB,sizeof(char),4U);});Cyc_aprintf(_tmp896,_tag_fat(_tmp3FA,sizeof(void*),1U));});});*_tmp3FD=_tmp897;});_tmp3FD;});_tmp3FE->name=_tmp898;}),_tmp3FE->identity=- 1,({
void*_tmp895=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3FE->kind=_tmp895;});_tmp3FE;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp89B=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp89A=({struct _tuple0*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp899=({struct _fat_ptr*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));*_tmp3F8=two;_tmp3F8;});_tmp3F9->f2=_tmp899;});_tmp3F9;});Cyc_Absyn_new_vardecl(_tmp89B,_tmp89A,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));({struct Cyc_Absyn_Decl*_tmp89C=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F7->hd=_tmp89C;}),_tmp3F7->tl=0;_tmp3F7;}));
# 1367
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1369
struct _fat_ptr _tmp3FF=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3FF;
struct _fat_ptr _tmp400=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp400;
struct Cyc_Absyn_Exp*_tmp401=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp401;
if(({struct _fat_ptr _tmp89D=(struct _fat_ptr)four;Cyc_strcmp(_tmp89D,({const char*_tmp402="open";_tag_fat(_tmp402,sizeof(char),5U);}));})!= 0)({void*_tmp403=0U;({unsigned _tmp89F=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp89E=({const char*_tmp404="expecting `open'";_tag_fat(_tmp404,sizeof(char),17U);});Cyc_Warn_err(_tmp89F,_tmp89E,_tag_fat(_tmp403,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp40C=_cycalloc(sizeof(*_tmp40C));({struct _fat_ptr*_tmp8A3=({struct _fat_ptr*_tmp40B=_cycalloc(sizeof(*_tmp40B));({struct _fat_ptr _tmp8A2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmp71E;_tmp71E.tag=0U,_tmp71E.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp71E;});void*_tmp408[1U];_tmp408[0]=& _tmp40A;({struct _fat_ptr _tmp8A1=({const char*_tmp409="`%s";_tag_fat(_tmp409,sizeof(char),4U);});Cyc_aprintf(_tmp8A1,_tag_fat(_tmp408,sizeof(void*),1U));});});*_tmp40B=_tmp8A2;});_tmp40B;});_tmp40C->name=_tmp8A3;}),_tmp40C->identity=- 1,({
void*_tmp8A0=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp40C->kind=_tmp8A0;});_tmp40C;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A6=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8A5=({struct _tuple0*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A4=({struct _fat_ptr*_tmp406=_cycalloc(sizeof(*_tmp406));*_tmp406=two;_tmp406;});_tmp407->f2=_tmp8A4;});_tmp407;});Cyc_Absyn_new_vardecl(_tmp8A6,_tmp8A5,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp405=_cycalloc(sizeof(*_tmp405));({struct Cyc_Absyn_Decl*_tmp8A7=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp405->hd=_tmp8A7;}),_tmp405->tl=0;_tmp405;}));
# 1379
goto _LL0;}}case 54U: _LL67: _LL68:
# 1383 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55U: _LL69: _LL6A:
# 1385 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8A8=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8A8,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56U: _LL6B: _LL6C:
# 1391 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71F;({enum Cyc_Parse_Storage_class _tmp8AB=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp71F.sc=_tmp8AB;}),({struct Cyc_Absyn_Tqual _tmp8AA=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp71F.tq=_tmp8AA;}),({
struct Cyc_Parse_Type_specifier _tmp8A9=Cyc_Parse_empty_spec(0U);_tmp71F.type_specs=_tmp8A9;}),_tmp71F.is_inline=0,_tmp71F.attributes=0;_tmp71F;}));
goto _LL0;case 57U: _LL6D: _LL6E: {
# 1394 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp40D=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp40D;
if((int)two.sc != (int)Cyc_Parse_None_sc)
({void*_tmp40E=0U;({unsigned _tmp8AD=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8AC=({const char*_tmp40F="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp40F,sizeof(char),73U);});Cyc_Warn_warn(_tmp8AD,_tmp8AC,_tag_fat(_tmp40E,sizeof(void*),0U));});});
# 1398
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp720;({enum Cyc_Parse_Storage_class _tmp8AE=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp720.sc=_tmp8AE;}),_tmp720.tq=two.tq,_tmp720.type_specs=two.type_specs,_tmp720.is_inline=two.is_inline,_tmp720.attributes=two.attributes;_tmp720;}));
# 1402
goto _LL0;}case 58U: _LL6F: _LL70:
# 1403 "parse.y"
({void*_tmp410=0U;({unsigned _tmp8B0=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8AF=({const char*_tmp411="__extension__ keyword ignored in declaration";_tag_fat(_tmp411,sizeof(char),45U);});Cyc_Warn_warn(_tmp8B0,_tmp8AF,_tag_fat(_tmp410,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1406
goto _LL0;case 59U: _LL71: _LL72:
# 1407 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp721;_tmp721.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8B2=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp721.tq=_tmp8B2;}),({
struct Cyc_Parse_Type_specifier _tmp8B1=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp721.type_specs=_tmp8B1;}),_tmp721.is_inline=0,_tmp721.attributes=0;_tmp721;}));
goto _LL0;case 60U: _LL73: _LL74: {
# 1410 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp412=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp412;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp722;_tmp722.sc=two.sc,_tmp722.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8B5=({unsigned _tmp8B4=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8B3=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8B4,_tmp8B3,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp722.type_specs=_tmp8B5;}),_tmp722.is_inline=two.is_inline,_tmp722.attributes=two.attributes;_tmp722;}));
# 1416
goto _LL0;}case 61U: _LL75: _LL76:
# 1417 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp723;_tmp723.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8B7=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp723.tq=_tmp8B7;}),({struct Cyc_Parse_Type_specifier _tmp8B6=Cyc_Parse_empty_spec(0U);_tmp723.type_specs=_tmp8B6;}),_tmp723.is_inline=0,_tmp723.attributes=0;_tmp723;}));
goto _LL0;case 62U: _LL77: _LL78: {
# 1419 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp413=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp413;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp724;_tmp724.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8B9=({struct Cyc_Absyn_Tqual _tmp8B8=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8B8,two.tq);});_tmp724.tq=_tmp8B9;}),_tmp724.type_specs=two.type_specs,_tmp724.is_inline=two.is_inline,_tmp724.attributes=two.attributes;_tmp724;}));
# 1424
goto _LL0;}case 63U: _LL79: _LL7A:
# 1425 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp725;_tmp725.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8BB=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp725.tq=_tmp8BB;}),({
struct Cyc_Parse_Type_specifier _tmp8BA=Cyc_Parse_empty_spec(0U);_tmp725.type_specs=_tmp8BA;}),_tmp725.is_inline=1,_tmp725.attributes=0;_tmp725;}));
goto _LL0;case 64U: _LL7B: _LL7C: {
# 1428 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp414=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp414;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp726;_tmp726.sc=two.sc,_tmp726.tq=two.tq,_tmp726.type_specs=two.type_specs,_tmp726.is_inline=1,_tmp726.attributes=two.attributes;_tmp726;}));
# 1432
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1433 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp727;_tmp727.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8BE=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp727.tq=_tmp8BE;}),({
struct Cyc_Parse_Type_specifier _tmp8BD=Cyc_Parse_empty_spec(0U);_tmp727.type_specs=_tmp8BD;}),_tmp727.is_inline=0,({struct Cyc_List_List*_tmp8BC=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp727.attributes=_tmp8BC;});_tmp727;}));
goto _LL0;case 66U: _LL7F: _LL80: {
# 1436 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp415=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp415;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp728;_tmp728.sc=two.sc,_tmp728.tq=two.tq,_tmp728.type_specs=two.type_specs,_tmp728.is_inline=two.is_inline,({
# 1439
struct Cyc_List_List*_tmp8C0=({struct Cyc_List_List*_tmp8BF=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8BF,two.attributes);});_tmp728.attributes=_tmp8C0;});_tmp728;}));
goto _LL0;}case 67U: _LL81: _LL82:
# 1443 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Auto_sc);
goto _LL0;case 68U: _LL83: _LL84:
# 1444 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Register_sc);
goto _LL0;case 69U: _LL85: _LL86:
# 1445 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Static_sc);
goto _LL0;case 70U: _LL87: _LL88:
# 1446 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Extern_sc);
goto _LL0;case 71U: _LL89: _LL8A:
# 1448 "parse.y"
 if(({struct _fat_ptr _tmp8C1=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8C1,({const char*_tmp416="C";_tag_fat(_tmp416,sizeof(char),2U);}));})!= 0)
({void*_tmp417=0U;({unsigned _tmp8C3=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C2=({const char*_tmp418="only extern or extern \"C\" is allowed";_tag_fat(_tmp418,sizeof(char),37U);});Cyc_Warn_err(_tmp8C3,_tmp8C2,_tag_fat(_tmp417,sizeof(void*),0U));});});
yyval=Cyc_YY20(Cyc_Parse_ExternC_sc);
# 1452
goto _LL0;case 72U: _LL8B: _LL8C:
# 1452 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Typedef_sc);
goto _LL0;case 73U: _LL8D: _LL8E:
# 1454 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Abstract_sc);
goto _LL0;case 74U: _LL8F: _LL90:
# 1459 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 75U: _LL91: _LL92:
# 1460 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76U: _LL93: _LL94:
# 1465 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1469 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp419=_cycalloc(sizeof(*_tmp419));({void*_tmp8C4=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp419->hd=_tmp8C4;}),_tmp419->tl=0;_tmp419;}));
goto _LL0;case 78U: _LL97: _LL98:
# 1470 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp41A=_cycalloc(sizeof(*_tmp41A));({void*_tmp8C6=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp41A->hd=_tmp8C6;}),({struct Cyc_List_List*_tmp8C5=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp41A->tl=_tmp8C5;});_tmp41A;}));
goto _LL0;case 79U: _LL99: _LL9A: {
# 1475 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[19U]={{{_tmp41E,_tmp41E,_tmp41E + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp41F,_tmp41F,_tmp41F + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp420,_tmp420,_tmp420 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp421,_tmp421,_tmp421 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp422,_tmp422,_tmp422 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp423,_tmp423,_tmp423 + 8U},(void*)& att_aligned},{{_tmp424,_tmp424,_tmp424 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp425,_tmp425,_tmp425 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp426,_tmp426,_tmp426 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp427,_tmp427,_tmp427 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp428,_tmp428,_tmp428 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp429,_tmp429,_tmp429 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp42A,_tmp42A,_tmp42A + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp42B,_tmp42B,_tmp42B + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp42C,_tmp42C,_tmp42C + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp42D,_tmp42D,_tmp42D + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp42E,_tmp42E,_tmp42E + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp42F,_tmp42F,_tmp42F + 14U},(void*)& Cyc_Absyn_Always_inline_att_val},{{_tmp430,_tmp430,_tmp430 + 9U},(void*)& Cyc_Absyn_No_throw_att_val}};
# 1497
struct _fat_ptr _tmp41B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp41B;
# 1499
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1501
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 19U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,19U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1508
if((unsigned)i == 19U){
({void*_tmp41C=0U;({unsigned _tmp8C8=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C7=({const char*_tmp41D="unrecognized attribute";_tag_fat(_tmp41D,sizeof(char),23U);});Cyc_Warn_err(_tmp8C8,_tmp8C7,_tag_fat(_tmp41C,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1513
goto _LL0;}}case 80U: _LL9B: _LL9C:
# 1513 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 81U: _LL9D: _LL9E: {
# 1515 "parse.y"
struct _fat_ptr _tmp431=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp431;
struct Cyc_Absyn_Exp*_tmp432=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp432;
void*a;
if(({struct _fat_ptr _tmp8CA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CA,({const char*_tmp433="aligned";_tag_fat(_tmp433,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8C9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8C9,({const char*_tmp434="__aligned__";_tag_fat(_tmp434,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->tag=6U,_tmp435->f1=e;_tmp435;});else{
if(({struct _fat_ptr _tmp8CC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CC,({const char*_tmp436="section";_tag_fat(_tmp436,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8CB=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CB,({const char*_tmp437="__section__";_tag_fat(_tmp437,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=8U,({struct _fat_ptr _tmp8CD=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp438->f1=_tmp8CD;});_tmp438;});else{
if(({struct _fat_ptr _tmp8CE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CE,({const char*_tmp439="__mode__";_tag_fat(_tmp439,sizeof(char),9U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=24U,({struct _fat_ptr _tmp8CF=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp43A->f1=_tmp8CF;});_tmp43A;});else{
if(({struct _fat_ptr _tmp8D0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D0,({const char*_tmp43B="alias";_tag_fat(_tmp43B,sizeof(char),6U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=25U,({struct _fat_ptr _tmp8D1=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp43C->f1=_tmp8D1;});_tmp43C;});else{
# 1527
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8D3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D3,({const char*_tmp43D="regparm";_tag_fat(_tmp43D,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D2,({const char*_tmp43E="__regparm__";_tag_fat(_tmp43E,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp43F=0U;({unsigned _tmp8D5=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8D4=({const char*_tmp440="regparm requires value between 0 and 3";_tag_fat(_tmp440,sizeof(char),39U);});Cyc_Warn_err(_tmp8D5,_tmp8D4,_tag_fat(_tmp43F,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->tag=0U,_tmp441->f1=n;_tmp441;});}else{
if(({struct _fat_ptr _tmp8D7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D7,({const char*_tmp442="initializes";_tag_fat(_tmp442,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8D6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D6,({const char*_tmp443="__initializes__";_tag_fat(_tmp443,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->tag=20U,_tmp444->f1=n;_tmp444;});else{
if(({struct _fat_ptr _tmp8D9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D9,({const char*_tmp445="noliveunique";_tag_fat(_tmp445,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8D8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D8,({const char*_tmp446="__noliveunique__";_tag_fat(_tmp446,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=21U,_tmp447->f1=n;_tmp447;});else{
if(({struct _fat_ptr _tmp8DB=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DB,({const char*_tmp448="consume";_tag_fat(_tmp448,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8DA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DA,({const char*_tmp449="__consume__";_tag_fat(_tmp449,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->tag=22U,_tmp44A->f1=n;_tmp44A;});else{
# 1539
({void*_tmp44B=0U;({unsigned _tmp8DD=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8DC=({const char*_tmp44C="unrecognized attribute";_tag_fat(_tmp44C,sizeof(char),23U);});Cyc_Warn_err(_tmp8DD,_tmp8DC,_tag_fat(_tmp44B,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1543
yyval=Cyc_YY46(a);
# 1545
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1546 "parse.y"
struct _fat_ptr _tmp44D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp44D;
struct _fat_ptr _tmp44E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp44E;
unsigned _tmp44F=({unsigned _tmp8DE=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DE,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp44F;
unsigned _tmp450=({unsigned _tmp8DF=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DF,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp450;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8E1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E1,({const char*_tmp451="format";_tag_fat(_tmp451,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8E0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E0,({const char*_tmp452="__format__";_tag_fat(_tmp452,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8E3=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E3,({const char*_tmp453="printf";_tag_fat(_tmp453,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8E2=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E2,({const char*_tmp454="__printf__";_tag_fat(_tmp454,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->tag=19U,_tmp455->f1=Cyc_Absyn_Printf_ft,_tmp455->f2=(int)n,_tmp455->f3=(int)m;_tmp455;});else{
if(({struct _fat_ptr _tmp8E5=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E5,({const char*_tmp456="scanf";_tag_fat(_tmp456,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8E4=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E4,({const char*_tmp457="__scanf__";_tag_fat(_tmp457,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->tag=19U,_tmp458->f1=Cyc_Absyn_Scanf_ft,_tmp458->f2=(int)n,_tmp458->f3=(int)m;_tmp458;});else{
# 1557
({void*_tmp459=0U;({unsigned _tmp8E7=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8E6=({const char*_tmp45A="unrecognized format type";_tag_fat(_tmp45A,sizeof(char),25U);});Cyc_Warn_err(_tmp8E7,_tmp8E6,_tag_fat(_tmp459,sizeof(void*),0U));});});}}}else{
# 1559
({void*_tmp45B=0U;({unsigned _tmp8E9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8E8=({const char*_tmp45C="unrecognized attribute";_tag_fat(_tmp45C,sizeof(char),23U);});Cyc_Warn_err(_tmp8E9,_tmp8E8,_tag_fat(_tmp45B,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1562
goto _LL0;}case 83U: _LLA1: _LLA2:
# 1571 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84U: _LLA3: _LLA4:
# 1573 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EB=({struct _tuple0*_tmp8EA=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp8EA,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp8EB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85U: _LLA5: _LLA6:
# 1577 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1578 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1579 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1580 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1581 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1582 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1583 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1584 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1585 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1586 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95U: _LLB9: _LLBA:
# 1587 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1590
 yyval=Cyc_YY21(({void*_tmp8EC=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8EC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97U: _LLBD: _LLBE:
# 1592 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8ED=Cyc_Absyn_builtin_type(({const char*_tmp45D="__builtin_va_list";_tag_fat(_tmp45D,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp8ED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1594 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99U: _LLC1: _LLC2:
# 1596 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EE=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp8EE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100U: _LLC3: _LLC4:
# 1598 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EF=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8EF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1600 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F0=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp8F0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1602 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F3=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->tag=6U,({struct Cyc_List_List*_tmp8F2=({unsigned _tmp8F1=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8F1,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});_tmp45E->f1=_tmp8F2;});_tmp45E;});Cyc_Parse_type_spec(_tmp8F3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1604
goto _LL0;case 103U: _LLC9: _LLCA:
# 1605 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F4=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104U: _LLCB: _LLCC:
# 1607 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F5=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp8F5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1609
goto _LL0;case 105U: _LLCD: _LLCE:
# 1610 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F6=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106U: _LLCF: _LLD0:
# 1612 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F7=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp8F7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1614 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F8=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1619 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp8F9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp8F9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6: {
# 1623 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp729;_tmp729.print_const=1,_tmp729.q_volatile=0,_tmp729.q_restrict=0,_tmp729.real_const=1,_tmp729.loc=loc;_tmp729;}));
goto _LL0;}case 110U: _LLD7: _LLD8:
# 1625 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp72A;_tmp72A.print_const=0,_tmp72A.q_volatile=1,_tmp72A.q_restrict=0,_tmp72A.real_const=0,_tmp72A.loc=0U;_tmp72A;}));
goto _LL0;case 111U: _LLD9: _LLDA:
# 1626 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp72B;_tmp72B.print_const=0,_tmp72B.q_volatile=0,_tmp72B.q_restrict=1,_tmp72B.real_const=0,_tmp72B.loc=0U;_tmp72B;}));
goto _LL0;case 112U: _LLDB: _LLDC: {
# 1632 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp45F=({struct Cyc_Absyn_TypeDecl*_tmp464=_cycalloc(sizeof(*_tmp464));({void*_tmp8FE=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8FD=({struct Cyc_Absyn_Enumdecl*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8FC=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp462->name=_tmp8FC;}),({struct Cyc_Core_Opt*_tmp8FB=({struct Cyc_Core_Opt*_tmp461=_cycalloc(sizeof(*_tmp461));({struct Cyc_List_List*_tmp8FA=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp461->v=_tmp8FA;});_tmp461;});_tmp462->fields=_tmp8FB;});_tmp462;});_tmp463->f1=_tmp8FD;});_tmp463;});_tmp464->r=_tmp8FE;}),_tmp464->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp464;});struct Cyc_Absyn_TypeDecl*ed=_tmp45F;
# 1634
yyval=Cyc_YY21(({void*_tmp8FF=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->tag=10U,_tmp460->f1=ed,_tmp460->f2=0;_tmp460;});Cyc_Parse_type_spec(_tmp8FF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1636
goto _LL0;}case 113U: _LLDD: _LLDE:
# 1637 "parse.y"
 yyval=Cyc_YY21(({void*_tmp900=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp900,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114U: _LLDF: _LLE0:
# 1639 "parse.y"
 yyval=Cyc_YY21(({void*_tmp901=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp901,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1645 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp465=_cycalloc(sizeof(*_tmp465));({struct _tuple0*_tmp902=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp465->name=_tmp902;}),_tmp465->tag=0,_tmp465->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp465;}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1647 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp466=_cycalloc(sizeof(*_tmp466));({struct _tuple0*_tmp904=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp466->name=_tmp904;}),({struct Cyc_Absyn_Exp*_tmp903=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp466->tag=_tmp903;}),_tmp466->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp466;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1651 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_Absyn_Enumfield*_tmp905=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp467->hd=_tmp905;}),_tmp467->tl=0;_tmp467;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1652 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp468=_cycalloc(sizeof(*_tmp468));({struct Cyc_Absyn_Enumfield*_tmp906=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp468->hd=_tmp906;}),_tmp468->tl=0;_tmp468;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1653 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp469=_cycalloc(sizeof(*_tmp469));({struct Cyc_Absyn_Enumfield*_tmp908=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp469->hd=_tmp908;}),({struct Cyc_List_List*_tmp907=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp469->tl=_tmp907;});_tmp469;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1659 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90B=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=7U,({enum Cyc_Absyn_AggrKind _tmp90A=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp46A->f1=_tmp90A;}),({struct Cyc_List_List*_tmp909=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp46A->f2=_tmp909;});_tmp46A;});Cyc_Parse_type_spec(_tmp90B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121U: _LLED: _LLEE: {
# 1665 "parse.y"
struct Cyc_List_List*_tmp46B=({unsigned _tmp90C=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp90C,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp46B;
struct Cyc_List_List*_tmp46C=({unsigned _tmp90D=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp90D,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp46C;
struct Cyc_Absyn_TypeDecl*_tmp46D=({enum Cyc_Absyn_AggrKind _tmp913=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp912=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp911=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp910=({
struct Cyc_List_List*_tmp90F=exist_ts;struct Cyc_List_List*_tmp90E=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp90F,_tmp90E,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1667
Cyc_Absyn_aggr_tdecl(_tmp913,Cyc_Absyn_Public,_tmp912,_tmp911,_tmp910,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp46D;
# 1670
yyval=Cyc_YY21(({void*_tmp914=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=10U,_tmp46E->f1=td,_tmp46E->f2=0;_tmp46E;});Cyc_Parse_type_spec(_tmp914,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1672
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1676 "parse.y"
struct Cyc_List_List*_tmp46F=({unsigned _tmp915=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp915,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp46F;
struct Cyc_List_List*_tmp470=({unsigned _tmp916=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp916,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp470;
struct Cyc_Absyn_TypeDecl*_tmp471=({enum Cyc_Absyn_AggrKind _tmp91C=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp91B=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp91A=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp919=({
struct Cyc_List_List*_tmp918=exist_ts;struct Cyc_List_List*_tmp917=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp918,_tmp917,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1678
Cyc_Absyn_aggr_tdecl(_tmp91C,Cyc_Absyn_Public,_tmp91B,_tmp91A,_tmp919,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp471;
# 1681
yyval=Cyc_YY21(({void*_tmp91D=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->tag=10U,_tmp472->f1=td,_tmp472->f2=0;_tmp472;});Cyc_Parse_type_spec(_tmp91D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1683
goto _LL0;}case 123U: _LLF1: _LLF2:
# 1684 "parse.y"
 yyval=Cyc_YY21(({void*_tmp921=({union Cyc_Absyn_AggrInfo _tmp920=({enum Cyc_Absyn_AggrKind _tmp91F=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp91E=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp91F,_tmp91E,({struct Cyc_Core_Opt*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->v=(void*)1;_tmp473;}));});Cyc_Absyn_aggr_type(_tmp920,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp921,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1687
goto _LL0;case 124U: _LLF3: _LLF4:
# 1688 "parse.y"
 yyval=Cyc_YY21(({void*_tmp924=({union Cyc_Absyn_AggrInfo _tmp923=({enum Cyc_Absyn_AggrKind _tmp922=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp922,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp923,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp924,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 125U: _LLF5: _LLF6:
# 1693 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 126U: _LLF7: _LLF8:
# 1695 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 127U: _LLF9: _LLFA:
# 1699 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 128U: _LLFB: _LLFC:
# 1700 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1705 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 130U: _LLFF: _LL100: {
# 1707 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp474=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp474;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1711
struct Cyc_List_List*_tmp475=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp475;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1716
goto _LL0;}}case 131U: _LL101: _LL102:
# 1721 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp476=_cycalloc(sizeof(*_tmp476));({struct Cyc_List_List*_tmp925=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp476->hd=_tmp925;}),_tmp476->tl=0;_tmp476;}));
goto _LL0;case 132U: _LL103: _LL104:
# 1723 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp477=_cycalloc(sizeof(*_tmp477));({struct Cyc_List_List*_tmp927=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp477->hd=_tmp927;}),({struct Cyc_List_List*_tmp926=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp477->tl=_tmp926;});_tmp477;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1727 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 134U: _LL107: _LL108:
# 1733 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp478=_region_malloc(yyr,sizeof(*_tmp478));_tmp478->tl=0,({struct _tuple12 _tmp928=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp478->hd=_tmp928;});_tmp478;}));
goto _LL0;case 135U: _LL109: _LL10A:
# 1735 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp479=_region_malloc(yyr,sizeof(*_tmp479));({struct _tuple13*_tmp92A=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp479->tl=_tmp92A;}),({struct _tuple12 _tmp929=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp479->hd=_tmp929;});_tmp479;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1740 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp72C;({struct Cyc_Parse_Declarator _tmp92B=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp72C.f1=_tmp92B;}),_tmp72C.f2=0;_tmp72C;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1742 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp72D;({struct Cyc_Parse_Declarator _tmp92D=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp72D.f1=_tmp92D;}),({struct Cyc_Absyn_Exp*_tmp92C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp72D.f2=_tmp92C;});_tmp72D;}));
goto _LL0;case 138U: _LL10F: _LL110: {
# 1748 "parse.y"
struct _RegionHandle _tmp47A=_new_region("temp");struct _RegionHandle*temp=& _tmp47A;_push_region(temp);
{struct _tuple26 _tmp47B=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp16=_tmp47B;struct _tuple26 _tmp47C=_stmttmp16;struct Cyc_List_List*_tmp47F;struct Cyc_Parse_Type_specifier _tmp47E;struct Cyc_Absyn_Tqual _tmp47D;_LL467: _tmp47D=_tmp47C.f1;_tmp47E=_tmp47C.f2;_tmp47F=_tmp47C.f3;_LL468: {struct Cyc_Absyn_Tqual tq=_tmp47D;struct Cyc_Parse_Type_specifier tspecs=_tmp47E;struct Cyc_List_List*atts=_tmp47F;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp480=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp480;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp481=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp17=_tmp481;struct _tuple25*_tmp482=_stmttmp17;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Parse_Declarator _tmp483;_LL46A: _tmp483=_tmp482->f1;_tmp484=_tmp482->f2;_tmp485=_tmp482->f3;_LL46B: {struct Cyc_Parse_Declarator d=_tmp483;struct Cyc_Absyn_Exp*wd=_tmp484;struct Cyc_Absyn_Exp*wh=_tmp485;
decls=({struct _tuple11*_tmp486=_region_malloc(temp,sizeof(*_tmp486));_tmp486->tl=decls,_tmp486->hd=d;_tmp486;});
widths_and_reqs=({struct Cyc_List_List*_tmp488=_region_malloc(temp,sizeof(*_tmp488));
({struct _tuple17*_tmp92E=({struct _tuple17*_tmp487=_region_malloc(temp,sizeof(*_tmp487));_tmp487->f1=wd,_tmp487->f2=wh;_tmp487;});_tmp488->hd=_tmp92E;}),_tmp488->tl=widths_and_reqs;_tmp488;});}}}
# 1759
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp489=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp489;
struct Cyc_List_List*_tmp48A=({struct _RegionHandle*_tmp931=temp;struct _RegionHandle*_tmp930=temp;struct Cyc_List_List*_tmp92F=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1762
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp931,_tmp930,_tmp92F,widths_and_reqs);});struct Cyc_List_List*info=_tmp48A;
# 1765
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1767
_npop_handler(0U);goto _LL0;}}}}
# 1749
;_pop_region();}case 139U: _LL111: _LL112:
# 1775 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp72E;({struct Cyc_Absyn_Tqual _tmp933=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72E.f1=_tmp933;}),({struct Cyc_Parse_Type_specifier _tmp932=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp72E.f2=_tmp932;}),_tmp72E.f3=0;_tmp72E;}));
goto _LL0;case 140U: _LL113: _LL114: {
# 1777 "parse.y"
struct _tuple26 _tmp48B=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48B;yyval=Cyc_YY35(({struct _tuple26 _tmp72F;_tmp72F.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp936=({unsigned _tmp935=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp934=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp935,_tmp934,two.f2);});_tmp72F.f2=_tmp936;}),_tmp72F.f3=two.f3;_tmp72F;}));
goto _LL0;}case 141U: _LL115: _LL116:
# 1779 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp730;({struct Cyc_Absyn_Tqual _tmp938=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp730.f1=_tmp938;}),({struct Cyc_Parse_Type_specifier _tmp937=Cyc_Parse_empty_spec(0U);_tmp730.f2=_tmp937;}),_tmp730.f3=0;_tmp730;}));
goto _LL0;case 142U: _LL117: _LL118: {
# 1781 "parse.y"
struct _tuple26 _tmp48C=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48C;
yyval=Cyc_YY35(({struct _tuple26 _tmp731;({struct Cyc_Absyn_Tqual _tmp93A=({struct Cyc_Absyn_Tqual _tmp939=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp939,two.f1);});_tmp731.f1=_tmp93A;}),_tmp731.f2=two.f2,_tmp731.f3=two.f3;_tmp731;}));
goto _LL0;}case 143U: _LL119: _LL11A:
# 1784 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp732;({struct Cyc_Absyn_Tqual _tmp93D=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp732.f1=_tmp93D;}),({struct Cyc_Parse_Type_specifier _tmp93C=Cyc_Parse_empty_spec(0U);_tmp732.f2=_tmp93C;}),({struct Cyc_List_List*_tmp93B=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp732.f3=_tmp93B;});_tmp732;}));
goto _LL0;case 144U: _LL11B: _LL11C: {
# 1786 "parse.y"
struct _tuple26 _tmp48D=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48D;yyval=Cyc_YY35(({struct _tuple26 _tmp733;_tmp733.f1=two.f1,_tmp733.f2=two.f2,({struct Cyc_List_List*_tmp93F=({struct Cyc_List_List*_tmp93E=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp93E,two.f3);});_tmp733.f3=_tmp93F;});_tmp733;}));
goto _LL0;}case 145U: _LL11D: _LL11E:
# 1792 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp734;({struct Cyc_Absyn_Tqual _tmp941=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp734.f1=_tmp941;}),({struct Cyc_Parse_Type_specifier _tmp940=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp734.f2=_tmp940;}),_tmp734.f3=0;_tmp734;}));
goto _LL0;case 146U: _LL11F: _LL120: {
# 1794 "parse.y"
struct _tuple26 _tmp48E=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48E;yyval=Cyc_YY35(({struct _tuple26 _tmp735;_tmp735.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp944=({unsigned _tmp943=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp942=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp943,_tmp942,two.f2);});_tmp735.f2=_tmp944;}),_tmp735.f3=two.f3;_tmp735;}));
goto _LL0;}case 147U: _LL121: _LL122:
# 1796 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp736;({struct Cyc_Absyn_Tqual _tmp946=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp736.f1=_tmp946;}),({struct Cyc_Parse_Type_specifier _tmp945=Cyc_Parse_empty_spec(0U);_tmp736.f2=_tmp945;}),_tmp736.f3=0;_tmp736;}));
goto _LL0;case 148U: _LL123: _LL124: {
# 1798 "parse.y"
struct _tuple26 _tmp48F=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48F;
yyval=Cyc_YY35(({struct _tuple26 _tmp737;({struct Cyc_Absyn_Tqual _tmp948=({struct Cyc_Absyn_Tqual _tmp947=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp947,two.f1);});_tmp737.f1=_tmp948;}),_tmp737.f2=two.f2,_tmp737.f3=two.f3;_tmp737;}));
goto _LL0;}case 149U: _LL125: _LL126:
# 1801 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp738;({struct Cyc_Absyn_Tqual _tmp94B=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp738.f1=_tmp94B;}),({struct Cyc_Parse_Type_specifier _tmp94A=Cyc_Parse_empty_spec(0U);_tmp738.f2=_tmp94A;}),({struct Cyc_List_List*_tmp949=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp738.f3=_tmp949;});_tmp738;}));
goto _LL0;case 150U: _LL127: _LL128: {
# 1803 "parse.y"
struct _tuple26 _tmp490=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp490;yyval=Cyc_YY35(({struct _tuple26 _tmp739;_tmp739.f1=two.f1,_tmp739.f2=two.f2,({struct Cyc_List_List*_tmp94D=({struct Cyc_List_List*_tmp94C=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp94C,two.f3);});_tmp739.f3=_tmp94D;});_tmp739;}));
goto _LL0;}case 151U: _LL129: _LL12A:
# 1807 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 152U: _LL12B: _LL12C:
# 1813 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp491=_region_malloc(yyr,sizeof(*_tmp491));({struct _tuple25*_tmp94E=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp491->hd=_tmp94E;}),_tmp491->tl=0;_tmp491;}));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1815 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp492=_region_malloc(yyr,sizeof(*_tmp492));({struct _tuple25*_tmp950=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp492->hd=_tmp950;}),({struct Cyc_List_List*_tmp94F=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp492->tl=_tmp94F;});_tmp492;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1820 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp493=_region_malloc(yyr,sizeof(*_tmp493));({struct Cyc_Parse_Declarator _tmp952=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp493->f1=_tmp952;}),_tmp493->f2=0,({struct Cyc_Absyn_Exp*_tmp951=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp493->f3=_tmp951;});_tmp493;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1824 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp497=_region_malloc(yyr,sizeof(*_tmp497));({struct _tuple0*_tmp957=({struct _tuple0*_tmp496=_cycalloc(sizeof(*_tmp496));({union Cyc_Absyn_Nmspace _tmp956=Cyc_Absyn_Rel_n(0);_tmp496->f1=_tmp956;}),({struct _fat_ptr*_tmp955=({struct _fat_ptr*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _fat_ptr _tmp954=({const char*_tmp494="";_tag_fat(_tmp494,sizeof(char),1U);});*_tmp495=_tmp954;});_tmp495;});_tmp496->f2=_tmp955;});_tmp496;});(_tmp497->f1).id=_tmp957;}),(_tmp497->f1).varloc=0U,(_tmp497->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp953=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp497->f2=_tmp953;}),_tmp497->f3=0;_tmp497;}));
# 1826
goto _LL0;case 156U: _LL133: _LL134:
# 1829 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49B=_region_malloc(yyr,sizeof(*_tmp49B));({struct _tuple0*_tmp95B=({struct _tuple0*_tmp49A=_cycalloc(sizeof(*_tmp49A));({union Cyc_Absyn_Nmspace _tmp95A=Cyc_Absyn_Rel_n(0);_tmp49A->f1=_tmp95A;}),({struct _fat_ptr*_tmp959=({struct _fat_ptr*_tmp499=_cycalloc(sizeof(*_tmp499));({struct _fat_ptr _tmp958=({const char*_tmp498="";_tag_fat(_tmp498,sizeof(char),1U);});*_tmp499=_tmp958;});_tmp499;});_tmp49A->f2=_tmp959;});_tmp49A;});(_tmp49B->f1).id=_tmp95B;}),(_tmp49B->f1).varloc=0U,(_tmp49B->f1).tms=0,_tmp49B->f2=0,_tmp49B->f3=0;_tmp49B;}));
# 1831
goto _LL0;case 157U: _LL135: _LL136:
# 1832 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49C=_region_malloc(yyr,sizeof(*_tmp49C));({struct Cyc_Parse_Declarator _tmp95D=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp49C->f1=_tmp95D;}),({struct Cyc_Absyn_Exp*_tmp95C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp49C->f2=_tmp95C;}),_tmp49C->f3=0;_tmp49C;}));
goto _LL0;case 158U: _LL137: _LL138:
# 1836 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 159U: _LL139: _LL13A:
# 1837 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 160U: _LL13B: _LL13C:
# 1841 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 161U: _LL13D: _LL13E:
# 1842 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 162U: _LL13F: _LL140: {
# 1848 "parse.y"
int _tmp49D=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp49D;
struct Cyc_List_List*_tmp49E=({unsigned _tmp95E=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95E,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp49E;
struct Cyc_Absyn_TypeDecl*_tmp49F=({struct _tuple0*_tmp963=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp962=ts;struct Cyc_Core_Opt*_tmp961=({struct Cyc_Core_Opt*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));({struct Cyc_List_List*_tmp95F=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp4A1->v=_tmp95F;});_tmp4A1;});int _tmp960=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp963,_tmp962,_tmp961,_tmp960,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp49F;
# 1852
yyval=Cyc_YY21(({void*_tmp964=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->tag=10U,_tmp4A0->f1=dd,_tmp4A0->f2=0;_tmp4A0;});Cyc_Parse_type_spec(_tmp964,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1854
goto _LL0;}case 163U: _LL141: _LL142: {
# 1855 "parse.y"
int _tmp4A2=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A2;
yyval=Cyc_YY21(({void*_tmp967=({union Cyc_Absyn_DatatypeInfo _tmp966=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp73A;({struct _tuple0*_tmp965=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp73A.name=_tmp965;}),_tmp73A.is_extensible=is_extensible;_tmp73A;}));Cyc_Absyn_datatype_type(_tmp966,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp967,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1858
goto _LL0;}case 164U: _LL143: _LL144: {
# 1859 "parse.y"
int _tmp4A3=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A3;
yyval=Cyc_YY21(({void*_tmp96B=({union Cyc_Absyn_DatatypeFieldInfo _tmp96A=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp73B;({struct _tuple0*_tmp969=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp73B.datatype_name=_tmp969;}),({struct _tuple0*_tmp968=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp73B.field_name=_tmp968;}),_tmp73B.is_extensible=is_extensible;_tmp73B;}));Cyc_Absyn_datatype_field_type(_tmp96A,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp96B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1862
goto _LL0;}case 165U: _LL145: _LL146:
# 1865 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 166U: _LL147: _LL148:
# 1866 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 167U: _LL149: _LL14A:
# 1870 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));({struct Cyc_Absyn_Datatypefield*_tmp96C=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A4->hd=_tmp96C;}),_tmp4A4->tl=0;_tmp4A4;}));
goto _LL0;case 168U: _LL14B: _LL14C:
# 1871 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));({struct Cyc_Absyn_Datatypefield*_tmp96D=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A5->hd=_tmp96D;}),_tmp4A5->tl=0;_tmp4A5;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1872 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));({struct Cyc_Absyn_Datatypefield*_tmp96F=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A6->hd=_tmp96F;}),({struct Cyc_List_List*_tmp96E=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4A6->tl=_tmp96E;});_tmp4A6;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1873 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));({struct Cyc_Absyn_Datatypefield*_tmp971=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A7->hd=_tmp971;}),({struct Cyc_List_List*_tmp970=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4A7->tl=_tmp970;});_tmp4A7;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1877 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 172U: _LL153: _LL154:
# 1878 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 173U: _LL155: _LL156:
# 1879 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 174U: _LL157: _LL158:
# 1883 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));({struct _tuple0*_tmp973=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4A8->name=_tmp973;}),_tmp4A8->typs=0,_tmp4A8->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp972=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4A8->sc=_tmp972;});_tmp4A8;}));
goto _LL0;case 175U: _LL159: _LL15A: {
# 1885 "parse.y"
struct Cyc_List_List*_tmp4A9=({unsigned _tmp974=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp974,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4A9;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({struct _tuple0*_tmp976=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4AA->name=_tmp976;}),_tmp4AA->typs=typs,_tmp4AA->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp975=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4AA->sc=_tmp975;});_tmp4AA;}));
goto _LL0;}case 176U: _LL15B: _LL15C:
# 1891 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 177U: _LL15D: _LL15E: {
# 1893 "parse.y"
struct Cyc_Parse_Declarator _tmp4AB=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4AB;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73C;_tmp73C.id=two.id,_tmp73C.varloc=two.varloc,({struct Cyc_List_List*_tmp978=({struct Cyc_List_List*_tmp977=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp977,two.tms);});_tmp73C.tms=_tmp978;});_tmp73C;}));
goto _LL0;}case 178U: _LL15F: _LL160:
# 1900 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 179U: _LL161: _LL162: {
# 1902 "parse.y"
struct Cyc_Parse_Declarator _tmp4AC=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4AC;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73D;_tmp73D.id=two.id,_tmp73D.varloc=two.varloc,({struct Cyc_List_List*_tmp97A=({struct Cyc_List_List*_tmp979=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp979,two.tms);});_tmp73D.tms=_tmp97A;});_tmp73D;}));
goto _LL0;}case 180U: _LL163: _LL164:
# 1908 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73E;({struct _tuple0*_tmp97B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp73E.id=_tmp97B;}),_tmp73E.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp73E.tms=0;_tmp73E;}));
goto _LL0;case 181U: _LL165: _LL166:
# 1910 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 182U: _LL167: _LL168: {
# 1914 "parse.y"
struct Cyc_Parse_Declarator _tmp4AD=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4AD;
({struct Cyc_List_List*_tmp97E=({struct Cyc_List_List*_tmp4AF=_region_malloc(yyr,sizeof(*_tmp4AF));({void*_tmp97D=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4AE=_region_malloc(yyr,sizeof(*_tmp4AE));_tmp4AE->tag=5U,_tmp4AE->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4AE->f2=_tmp97C;});_tmp4AE;});_tmp4AF->hd=_tmp97D;}),_tmp4AF->tl=d.tms;_tmp4AF;});d.tms=_tmp97E;});
yyval=(yyyvsp[2]).v;
# 1918
goto _LL0;}case 183U: _LL169: _LL16A:
# 1919 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73F;({struct _tuple0*_tmp984=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp73F.id=_tmp984;}),({unsigned _tmp983=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp73F.varloc=_tmp983;}),({struct Cyc_List_List*_tmp982=({struct Cyc_List_List*_tmp4B1=_region_malloc(yyr,sizeof(*_tmp4B1));({void*_tmp981=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4B0=_region_malloc(yyr,sizeof(*_tmp4B0));_tmp4B0->tag=0U,({void*_tmp980=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4B0->f1=_tmp980;}),_tmp4B0->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4B0;});_tmp4B1->hd=_tmp981;}),({struct Cyc_List_List*_tmp97F=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B1->tl=_tmp97F;});_tmp4B1;});_tmp73F.tms=_tmp982;});_tmp73F;}));
goto _LL0;case 184U: _LL16B: _LL16C:
# 1921 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp740;({struct _tuple0*_tmp98B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp740.id=_tmp98B;}),({unsigned _tmp98A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp740.varloc=_tmp98A;}),({
struct Cyc_List_List*_tmp989=({struct Cyc_List_List*_tmp4B3=_region_malloc(yyr,sizeof(*_tmp4B3));({void*_tmp988=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B2=_region_malloc(yyr,sizeof(*_tmp4B2));_tmp4B2->tag=1U,({struct Cyc_Absyn_Exp*_tmp987=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4B2->f1=_tmp987;}),({void*_tmp986=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B2->f2=_tmp986;}),_tmp4B2->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4B2;});_tmp4B3->hd=_tmp988;}),({struct Cyc_List_List*_tmp985=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B3->tl=_tmp985;});_tmp4B3;});_tmp740.tms=_tmp989;});_tmp740;}));
goto _LL0;case 185U: _LL16D: _LL16E: {
# 1924 "parse.y"
struct _tuple27*_tmp4B4=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp18=_tmp4B4;struct _tuple27*_tmp4B5=_stmttmp18;struct Cyc_List_List*_tmp4BA;void*_tmp4B9;struct Cyc_Absyn_VarargInfo*_tmp4B8;int _tmp4B7;struct Cyc_List_List*_tmp4B6;_LL46D: _tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;_tmp4B8=_tmp4B5->f3;_tmp4B9=_tmp4B5->f4;_tmp4BA=_tmp4B5->f5;_LL46E: {struct Cyc_List_List*lis=_tmp4B6;int b=_tmp4B7;struct Cyc_Absyn_VarargInfo*c=_tmp4B8;void*eff=_tmp4B9;struct Cyc_List_List*po=_tmp4BA;
struct Cyc_Absyn_Exp*_tmp4BB=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4BB;
struct Cyc_Absyn_Exp*_tmp4BC=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4BC;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp741;({struct _tuple0*_tmp991=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp741.id=_tmp991;}),({unsigned _tmp990=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp741.varloc=_tmp990;}),({struct Cyc_List_List*_tmp98F=({struct Cyc_List_List*_tmp4BF=_region_malloc(yyr,sizeof(*_tmp4BF));({void*_tmp98E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4BE=_region_malloc(yyr,sizeof(*_tmp4BE));_tmp4BE->tag=3U,({void*_tmp98D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));_tmp4BD->tag=1U,_tmp4BD->f1=lis,_tmp4BD->f2=b,_tmp4BD->f3=c,_tmp4BD->f4=eff,_tmp4BD->f5=po,_tmp4BD->f6=req,_tmp4BD->f7=ens;_tmp4BD;});_tmp4BE->f1=_tmp98D;});_tmp4BE;});_tmp4BF->hd=_tmp98E;}),({struct Cyc_List_List*_tmp98C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4BF->tl=_tmp98C;});_tmp4BF;});_tmp741.tms=_tmp98F;});_tmp741;}));
# 1929
goto _LL0;}}case 186U: _LL16F: _LL170:
# 1930 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp742;({struct _tuple0*_tmp99B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp742.id=_tmp99B;}),({unsigned _tmp99A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp742.varloc=_tmp99A;}),({
struct Cyc_List_List*_tmp999=({struct Cyc_List_List*_tmp4C2=_region_malloc(yyr,sizeof(*_tmp4C2));({void*_tmp998=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C1=_region_malloc(yyr,sizeof(*_tmp4C1));_tmp4C1->tag=3U,({void*_tmp997=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));_tmp4C0->tag=1U,_tmp4C0->f1=0,_tmp4C0->f2=0,_tmp4C0->f3=0,({
# 1933
void*_tmp996=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4C0->f4=_tmp996;}),({struct Cyc_List_List*_tmp995=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4C0->f5=_tmp995;}),({struct Cyc_Absyn_Exp*_tmp994=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4C0->f6=_tmp994;}),({struct Cyc_Absyn_Exp*_tmp993=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4C0->f7=_tmp993;});_tmp4C0;});
# 1931
_tmp4C1->f1=_tmp997;});_tmp4C1;});_tmp4C2->hd=_tmp998;}),({
# 1934
struct Cyc_List_List*_tmp992=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C2->tl=_tmp992;});_tmp4C2;});
# 1931
_tmp742.tms=_tmp999;});_tmp742;}));
# 1936
goto _LL0;case 187U: _LL171: _LL172:
# 1937 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp743;({struct _tuple0*_tmp9A2=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp743.id=_tmp9A2;}),({unsigned _tmp9A1=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp743.varloc=_tmp9A1;}),({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));({void*_tmp99F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));_tmp4C4->tag=3U,({void*_tmp99E=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));_tmp4C3->tag=0U,({struct Cyc_List_List*_tmp99D=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4C3->f1=_tmp99D;}),_tmp4C3->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4C3;});_tmp4C4->f1=_tmp99E;});_tmp4C4;});_tmp4C5->hd=_tmp99F;}),({struct Cyc_List_List*_tmp99C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C5->tl=_tmp99C;});_tmp4C5;});_tmp743.tms=_tmp9A0;});_tmp743;}));
goto _LL0;case 188U: _LL173: _LL174: {
# 1940
struct Cyc_List_List*_tmp4C6=({unsigned _tmp9A3=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9A3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4C6;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp744;({struct _tuple0*_tmp9A8=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp744.id=_tmp9A8;}),({unsigned _tmp9A7=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp744.varloc=_tmp9A7;}),({struct Cyc_List_List*_tmp9A6=({struct Cyc_List_List*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));({void*_tmp9A5=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=4U,_tmp4C7->f1=ts,_tmp4C7->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4C7->f3=0;_tmp4C7;});_tmp4C8->hd=_tmp9A5;}),({struct Cyc_List_List*_tmp9A4=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C8->tl=_tmp9A4;});_tmp4C8;});_tmp744.tms=_tmp9A6;});_tmp744;}));
# 1943
goto _LL0;}case 189U: _LL175: _LL176:
# 1944 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;({struct _tuple0*_tmp9AE=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp745.id=_tmp9AE;}),({unsigned _tmp9AD=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp745.varloc=_tmp9AD;}),({struct Cyc_List_List*_tmp9AC=({struct Cyc_List_List*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));({void*_tmp9AB=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));_tmp4C9->tag=5U,_tmp4C9->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9AA=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4C9->f2=_tmp9AA;});_tmp4C9;});_tmp4CA->hd=_tmp9AB;}),({
struct Cyc_List_List*_tmp9A9=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CA->tl=_tmp9A9;});_tmp4CA;});
# 1944
_tmp745.tms=_tmp9AC;});_tmp745;}));
# 1947
goto _LL0;case 190U: _LL177: _LL178:
# 1952 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;({struct _tuple0*_tmp9AF=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp746.id=_tmp9AF;}),_tmp746.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp746.tms=0;_tmp746;}));
goto _LL0;case 191U: _LL179: _LL17A:
# 1954 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;({struct _tuple0*_tmp9B0=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp747.id=_tmp9B0;}),_tmp747.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp747.tms=0;_tmp747;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 1956 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 193U: _LL17D: _LL17E: {
# 1960 "parse.y"
struct Cyc_Parse_Declarator _tmp4CB=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4CB;
({struct Cyc_List_List*_tmp9B3=({struct Cyc_List_List*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));({void*_tmp9B2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));_tmp4CC->tag=5U,_tmp4CC->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B1=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4CC->f2=_tmp9B1;});_tmp4CC;});_tmp4CD->hd=_tmp9B2;}),_tmp4CD->tl=d.tms;_tmp4CD;});d.tms=_tmp9B3;});
yyval=(yyyvsp[2]).v;
# 1964
goto _LL0;}case 194U: _LL17F: _LL180: {
# 1965 "parse.y"
struct Cyc_Parse_Declarator _tmp4CE=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4CE;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;_tmp748.id=one.id,_tmp748.varloc=one.varloc,({
struct Cyc_List_List*_tmp9B6=({struct Cyc_List_List*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));({void*_tmp9B5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));_tmp4CF->tag=0U,({void*_tmp9B4=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4CF->f1=_tmp9B4;}),_tmp4CF->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4CF;});_tmp4D0->hd=_tmp9B5;}),_tmp4D0->tl=one.tms;_tmp4D0;});_tmp748.tms=_tmp9B6;});_tmp748;}));
goto _LL0;}case 195U: _LL181: _LL182: {
# 1969 "parse.y"
struct Cyc_Parse_Declarator _tmp4D1=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;_tmp749.id=one.id,_tmp749.varloc=one.varloc,({
struct Cyc_List_List*_tmp9BA=({struct Cyc_List_List*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));({void*_tmp9B9=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));_tmp4D2->tag=1U,({struct Cyc_Absyn_Exp*_tmp9B8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4D2->f1=_tmp9B8;}),({void*_tmp9B7=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4D2->f2=_tmp9B7;}),_tmp4D2->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4D2;});_tmp4D3->hd=_tmp9B9;}),_tmp4D3->tl=one.tms;_tmp4D3;});_tmp749.tms=_tmp9BA;});_tmp749;}));
# 1973
goto _LL0;}case 196U: _LL183: _LL184: {
# 1974 "parse.y"
struct _tuple27*_tmp4D4=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp19=_tmp4D4;struct _tuple27*_tmp4D5=_stmttmp19;struct Cyc_List_List*_tmp4DA;void*_tmp4D9;struct Cyc_Absyn_VarargInfo*_tmp4D8;int _tmp4D7;struct Cyc_List_List*_tmp4D6;_LL470: _tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;_tmp4D8=_tmp4D5->f3;_tmp4D9=_tmp4D5->f4;_tmp4DA=_tmp4D5->f5;_LL471: {struct Cyc_List_List*lis=_tmp4D6;int b=_tmp4D7;struct Cyc_Absyn_VarargInfo*c=_tmp4D8;void*eff=_tmp4D9;struct Cyc_List_List*po=_tmp4DA;
struct Cyc_Absyn_Exp*_tmp4DB=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4DB;
struct Cyc_Absyn_Exp*_tmp4DC=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4DC;
struct Cyc_Parse_Declarator _tmp4DD=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4DD;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;_tmp74A.id=one.id,_tmp74A.varloc=one.varloc,({struct Cyc_List_List*_tmp9BD=({struct Cyc_List_List*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));({void*_tmp9BC=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DF=_region_malloc(yyr,sizeof(*_tmp4DF));_tmp4DF->tag=3U,({void*_tmp9BB=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));_tmp4DE->tag=1U,_tmp4DE->f1=lis,_tmp4DE->f2=b,_tmp4DE->f3=c,_tmp4DE->f4=eff,_tmp4DE->f5=po,_tmp4DE->f6=req,_tmp4DE->f7=ens;_tmp4DE;});_tmp4DF->f1=_tmp9BB;});_tmp4DF;});_tmp4E0->hd=_tmp9BC;}),_tmp4E0->tl=one.tms;_tmp4E0;});_tmp74A.tms=_tmp9BD;});_tmp74A;}));
# 1980
goto _LL0;}}case 197U: _LL185: _LL186: {
# 1981 "parse.y"
struct Cyc_Parse_Declarator _tmp4E1=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;_tmp74B.id=one.id,_tmp74B.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C4=({struct Cyc_List_List*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));({void*_tmp9C3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E3=_region_malloc(yyr,sizeof(*_tmp4E3));_tmp4E3->tag=3U,({void*_tmp9C2=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E2=_region_malloc(yyr,sizeof(*_tmp4E2));_tmp4E2->tag=1U,_tmp4E2->f1=0,_tmp4E2->f2=0,_tmp4E2->f3=0,({
# 1985
void*_tmp9C1=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4E2->f4=_tmp9C1;}),({struct Cyc_List_List*_tmp9C0=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4E2->f5=_tmp9C0;}),({struct Cyc_Absyn_Exp*_tmp9BF=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4E2->f6=_tmp9BF;}),({struct Cyc_Absyn_Exp*_tmp9BE=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4E2->f7=_tmp9BE;});_tmp4E2;});
# 1983
_tmp4E3->f1=_tmp9C2;});_tmp4E3;});_tmp4E4->hd=_tmp9C3;}),_tmp4E4->tl=one.tms;_tmp4E4;});_tmp74B.tms=_tmp9C4;});_tmp74B;}));
# 1988
goto _LL0;}case 198U: _LL187: _LL188: {
# 1989 "parse.y"
struct Cyc_Parse_Declarator _tmp4E5=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E5;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74C;_tmp74C.id=one.id,_tmp74C.varloc=one.varloc,({struct Cyc_List_List*_tmp9C8=({struct Cyc_List_List*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));({void*_tmp9C7=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));_tmp4E7->tag=3U,({void*_tmp9C6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4E6=_region_malloc(yyr,sizeof(*_tmp4E6));_tmp4E6->tag=0U,({struct Cyc_List_List*_tmp9C5=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4E6->f1=_tmp9C5;}),_tmp4E6->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4E6;});_tmp4E7->f1=_tmp9C6;});_tmp4E7;});_tmp4E8->hd=_tmp9C7;}),_tmp4E8->tl=one.tms;_tmp4E8;});_tmp74C.tms=_tmp9C8;});_tmp74C;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 1993
struct Cyc_List_List*_tmp4E9=({unsigned _tmp9C9=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9C9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4E9;
struct Cyc_Parse_Declarator _tmp4EA=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EA;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74D;_tmp74D.id=one.id,_tmp74D.varloc=one.varloc,({struct Cyc_List_List*_tmp9CB=({struct Cyc_List_List*_tmp4EC=_region_malloc(yyr,sizeof(*_tmp4EC));({void*_tmp9CA=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));_tmp4EB->tag=4U,_tmp4EB->f1=ts,_tmp4EB->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4EB->f3=0;_tmp4EB;});_tmp4EC->hd=_tmp9CA;}),_tmp4EC->tl=one.tms;_tmp4EC;});_tmp74D.tms=_tmp9CB;});_tmp74D;}));
# 1997
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 1998 "parse.y"
struct Cyc_Parse_Declarator _tmp4ED=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4ED;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74E;_tmp74E.id=one.id,_tmp74E.varloc=one.varloc,({struct Cyc_List_List*_tmp9CE=({struct Cyc_List_List*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));({void*_tmp9CD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));_tmp4EE->tag=5U,_tmp4EE->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9CC=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4EE->f2=_tmp9CC;});_tmp4EE;});_tmp4EF->hd=_tmp9CD;}),_tmp4EF->tl=one.tms;_tmp4EF;});_tmp74E.tms=_tmp9CE;});_tmp74E;}));
# 2001
goto _LL0;}case 201U: _LL18D: _LL18E:
# 2005 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 202U: _LL18F: _LL190:
# 2006 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9CF=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9CF,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 203U: _LL191: _LL192: {
# 2010 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));({void*_tmp9D1=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F0=_region_malloc(yyr,sizeof(*_tmp4F0));_tmp4F0->tag=5U,_tmp4F0->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9D0=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4F0->f2=_tmp9D0;});_tmp4F0;});_tmp4F1->hd=_tmp9D1;}),_tmp4F1->tl=ans;_tmp4F1;});{
# 2014
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4F2=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1A=_tmp4F2;struct _tuple22 _tmp4F3=_stmttmp1A;void*_tmp4F6;void*_tmp4F5;unsigned _tmp4F4;_LL473: _tmp4F4=_tmp4F3.f1;_tmp4F5=_tmp4F3.f2;_tmp4F6=_tmp4F3.f3;_LL474: {unsigned ploc=_tmp4F4;void*nullable=_tmp4F5;void*bound=_tmp4F6;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->ptr_loc=ploc,_tmp4F7->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp4F7->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp4F7;});{
struct _tuple15 _tmp4F8=({unsigned _tmp9D5=ploc;void*_tmp9D4=nullable;void*_tmp9D3=bound;void*_tmp9D2=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9D5,_tmp9D4,_tmp9D3,_tmp9D2,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1B=_tmp4F8;struct _tuple15 _tmp4F9=_stmttmp1B;void*_tmp4FE;void*_tmp4FD;void*_tmp4FC;void*_tmp4FB;void*_tmp4FA;_LL476: _tmp4FA=_tmp4F9.f1;_tmp4FB=_tmp4F9.f2;_tmp4FC=_tmp4F9.f3;_tmp4FD=_tmp4F9.f4;_tmp4FE=_tmp4F9.f5;_LL477: {void*nullable=_tmp4FA;void*bound=_tmp4FB;void*zeroterm=_tmp4FC;void*rgn_opt=_tmp4FD;void*released=_tmp4FE;
ans=({struct Cyc_List_List*_tmp500=_region_malloc(yyr,sizeof(*_tmp500));({void*_tmp9D7=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4FF=_region_malloc(yyr,sizeof(*_tmp4FF));_tmp4FF->tag=2U,(_tmp4FF->f1).rgn=rgn_opt,(_tmp4FF->f1).nullable=nullable,(_tmp4FF->f1).bounds=bound,(_tmp4FF->f1).zero_term=zeroterm,(_tmp4FF->f1).ptrloc=ptrloc,(_tmp4FF->f1).released=released,({struct Cyc_Absyn_Tqual _tmp9D6=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp4FF->f2=_tmp9D6;});_tmp4FF;});_tmp500->hd=_tmp9D7;}),_tmp500->tl=ans;_tmp500;});
yyval=Cyc_YY26(ans);
# 2022
goto _LL0;}}}}}case 204U: _LL193: _LL194:
# 2024
 yyval=Cyc_YY56(0);
goto _LL0;case 205U: _LL195: _LL196:
# 2025 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp501=_region_malloc(yyr,sizeof(*_tmp501));({void*_tmp9D9=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp501->hd=_tmp9D9;}),({struct Cyc_List_List*_tmp9D8=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp501->tl=_tmp9D8;});_tmp501;}));
goto _LL0;case 206U: _LL197: _LL198:
# 2030 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp502=_region_malloc(yyr,sizeof(*_tmp502));_tmp502->tag=0U,({struct Cyc_Absyn_Exp*_tmp9DA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp502->f1=_tmp9DA;});_tmp502;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2032 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));_tmp503->tag=1U,({void*_tmp9DB=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp503->f1=_tmp9DB;});_tmp503;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2033 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));_tmp504->tag=2U;_tmp504;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2034 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp505=_region_malloc(yyr,sizeof(*_tmp505));_tmp505->tag=3U;_tmp505;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2035 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=6U;_tmp506;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2036 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));_tmp507->tag=4U;_tmp507;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2037 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));_tmp508->tag=5U;_tmp508;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2038 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=7U;_tmp509;}));
goto _LL0;case 214U: _LL1A7: _LL1A8:
# 2039 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));_tmp50A->tag=8U;_tmp50A;}));
goto _LL0;case 215U: _LL1A9: _LL1AA:
# 2045 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp50B->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp50B->f3=Cyc_Absyn_fat_bound_type:({void*_tmp9DC=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp50B->f3=_tmp9DC;});_tmp50B;}));
# 2047
goto _LL0;case 216U: _LL1AB: _LL1AC:
# 2047 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp50C->f2=Cyc_Absyn_false_type,({void*_tmp9DD=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp50C->f3=_tmp9DD;});_tmp50C;}));
goto _LL0;case 217U: _LL1AD: _LL1AE:
# 2048 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp50D->f2=Cyc_Absyn_true_type,_tmp50D->f3=Cyc_Absyn_fat_bound_type;_tmp50D;}));
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2051
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2052 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2055
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2056 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2057 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2062 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2063 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2064 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2068 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2069 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9DE=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9DE,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2074 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp50E=_cycalloc(sizeof(*_tmp50E));({struct Cyc_List_List*_tmp9E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp50E->f1=_tmp9E1;}),_tmp50E->f2=0,_tmp50E->f3=0,({void*_tmp9E0=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp50E->f4=_tmp9E0;}),({struct Cyc_List_List*_tmp9DF=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp50E->f5=_tmp9DF;});_tmp50E;}));
goto _LL0;case 229U: _LL1C5: _LL1C6:
# 2076 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp50F=_cycalloc(sizeof(*_tmp50F));({struct Cyc_List_List*_tmp9E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp50F->f1=_tmp9E4;}),_tmp50F->f2=1,_tmp50F->f3=0,({void*_tmp9E3=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp50F->f4=_tmp9E3;}),({struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp50F->f5=_tmp9E2;});_tmp50F;}));
goto _LL0;case 230U: _LL1C7: _LL1C8: {
# 2079
struct _tuple8*_tmp510=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1C=_tmp510;struct _tuple8*_tmp511=_stmttmp1C;void*_tmp514;struct Cyc_Absyn_Tqual _tmp513;struct _fat_ptr*_tmp512;_LL479: _tmp512=_tmp511->f1;_tmp513=_tmp511->f2;_tmp514=_tmp511->f3;_LL47A: {struct _fat_ptr*n=_tmp512;struct Cyc_Absyn_Tqual tq=_tmp513;void*t=_tmp514;
struct Cyc_Absyn_VarargInfo*_tmp515=({struct Cyc_Absyn_VarargInfo*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->name=n,_tmp517->tq=tq,_tmp517->type=t,({int _tmp9E5=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp517->inject=_tmp9E5;});_tmp517;});struct Cyc_Absyn_VarargInfo*v=_tmp515;
yyval=Cyc_YY39(({struct _tuple27*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->f1=0,_tmp516->f2=0,_tmp516->f3=v,({void*_tmp9E7=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp516->f4=_tmp9E7;}),({struct Cyc_List_List*_tmp9E6=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp516->f5=_tmp9E6;});_tmp516;}));
# 2083
goto _LL0;}}case 231U: _LL1C9: _LL1CA: {
# 2085
struct _tuple8*_tmp518=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1D=_tmp518;struct _tuple8*_tmp519=_stmttmp1D;void*_tmp51C;struct Cyc_Absyn_Tqual _tmp51B;struct _fat_ptr*_tmp51A;_LL47C: _tmp51A=_tmp519->f1;_tmp51B=_tmp519->f2;_tmp51C=_tmp519->f3;_LL47D: {struct _fat_ptr*n=_tmp51A;struct Cyc_Absyn_Tqual tq=_tmp51B;void*t=_tmp51C;
struct Cyc_Absyn_VarargInfo*_tmp51D=({struct Cyc_Absyn_VarargInfo*_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->name=n,_tmp51F->tq=tq,_tmp51F->type=t,({int _tmp9E8=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp51F->inject=_tmp9E8;});_tmp51F;});struct Cyc_Absyn_VarargInfo*v=_tmp51D;
yyval=Cyc_YY39(({struct _tuple27*_tmp51E=_cycalloc(sizeof(*_tmp51E));({struct Cyc_List_List*_tmp9EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp51E->f1=_tmp9EB;}),_tmp51E->f2=0,_tmp51E->f3=v,({void*_tmp9EA=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp51E->f4=_tmp9EA;}),({struct Cyc_List_List*_tmp9E9=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp51E->f5=_tmp9E9;});_tmp51E;}));
# 2089
goto _LL0;}}case 232U: _LL1CB: _LL1CC:
# 2093 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9EC=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9EC,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->tag=1U,_tmp520->f1=0;_tmp520;}));}));
goto _LL0;case 233U: _LL1CD: _LL1CE:
# 2094 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9ED=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9ED,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2097
 yyval=Cyc_YY49(0);
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2098 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2102 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 237U: _LL1D5: _LL1D6:
# 2103 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 238U: _LL1D7: _LL1D8: {
# 2111 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp521=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525->tag=2U,_tmp525->f1=0,_tmp525->f2=& Cyc_Kinds_trk;_tmp525;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp521;
void*_tmp522=({struct _fat_ptr _tmp9EE=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9EE,(void*)kb);});void*t=_tmp522;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp524=_cycalloc(sizeof(*_tmp524));({struct _tuple34*_tmp9F0=({struct _tuple34*_tmp523=_cycalloc(sizeof(*_tmp523));({void*_tmp9EF=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp523->f1=_tmp9EF;}),_tmp523->f2=t;_tmp523;});_tmp524->hd=_tmp9F0;}),_tmp524->tl=0;_tmp524;}));
# 2115
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2117 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp526=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->tag=2U,_tmp52A->f1=0,_tmp52A->f2=& Cyc_Kinds_trk;_tmp52A;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp526;
void*_tmp527=({struct _fat_ptr _tmp9F1=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9F1,(void*)kb);});void*t=_tmp527;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp529=_cycalloc(sizeof(*_tmp529));({struct _tuple34*_tmp9F4=({struct _tuple34*_tmp528=_cycalloc(sizeof(*_tmp528));({void*_tmp9F3=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp528->f1=_tmp9F3;}),_tmp528->f2=t;_tmp528;});_tmp529->hd=_tmp9F4;}),({struct Cyc_List_List*_tmp9F2=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp529->tl=_tmp9F2;});_tmp529;}));
# 2121
goto _LL0;}case 240U: _LL1DB: _LL1DC:
# 2125 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2127 "parse.y"
 if(({struct _fat_ptr _tmp9F5=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp9F5,({const char*_tmp52B="inject";_tag_fat(_tmp52B,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp74F;_tmp74F.tag=0U,({struct _fat_ptr _tmp9F6=({const char*_tmp52E="missing type in function declaration";_tag_fat(_tmp52E,sizeof(char),37U);});_tmp74F.f1=_tmp9F6;});_tmp74F;});void*_tmp52C[1U];_tmp52C[0]=& _tmp52D;({unsigned _tmp9F7=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp9F7,_tag_fat(_tmp52C,sizeof(void*),1U));});});
yyval=Cyc_YY31(1);
# 2131
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2134 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2135 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9F8=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F8,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2139 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2140 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2142 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*_tmp52F));({void*_tmp9F9=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp52F->hd=_tmp9F9;}),_tmp52F->tl=0;_tmp52F;}));
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2144 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));({void*_tmp9FA=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp530->hd=_tmp9FA;}),_tmp530->tl=0;_tmp530;}));
# 2147
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2152 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp531=_cycalloc(sizeof(*_tmp531));({void*_tmp9FC=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9FB=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9FB,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp531->hd=_tmp9FC;}),_tmp531->tl=0;_tmp531;}));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2154 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp532=_cycalloc(sizeof(*_tmp532));({void*_tmp9FF=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9FE=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9FE,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp532->hd=_tmp9FF;}),({struct Cyc_List_List*_tmp9FD=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp532->tl=_tmp9FD;});_tmp532;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2160 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp533=_cycalloc(sizeof(*_tmp533));({struct _tuple8*_tmpA00=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp533->hd=_tmpA00;}),_tmp533->tl=0;_tmp533;}));
goto _LL0;case 251U: _LL1F1: _LL1F2:
# 2162 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp534=_cycalloc(sizeof(*_tmp534));({struct _tuple8*_tmpA02=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp534->hd=_tmpA02;}),({struct Cyc_List_List*_tmpA01=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp534->tl=_tmpA01;});_tmp534;}));
goto _LL0;case 252U: _LL1F3: _LL1F4: {
# 2168 "parse.y"
struct _tuple26 _tmp535=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp1E=_tmp535;struct _tuple26 _tmp536=_stmttmp1E;struct Cyc_List_List*_tmp539;struct Cyc_Parse_Type_specifier _tmp538;struct Cyc_Absyn_Tqual _tmp537;_LL47F: _tmp537=_tmp536.f1;_tmp538=_tmp536.f2;_tmp539=_tmp536.f3;_LL480: {struct Cyc_Absyn_Tqual tq=_tmp537;struct Cyc_Parse_Type_specifier tspecs=_tmp538;struct Cyc_List_List*atts=_tmp539;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp53A=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1F=_tmp53A;struct Cyc_Parse_Declarator _tmp53B=_stmttmp1F;struct Cyc_List_List*_tmp53E;unsigned _tmp53D;struct _tuple0*_tmp53C;_LL482: _tmp53C=_tmp53B.id;_tmp53D=_tmp53B.varloc;_tmp53E=_tmp53B.tms;_LL483: {struct _tuple0*qv=_tmp53C;unsigned varloc=_tmp53D;struct Cyc_List_List*tms=_tmp53E;
void*_tmp53F=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp53F;
struct _tuple14 _tmp540=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp20=_tmp540;struct _tuple14 _tmp541=_stmttmp20;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp544;void*_tmp543;struct Cyc_Absyn_Tqual _tmp542;_LL485: _tmp542=_tmp541.f1;_tmp543=_tmp541.f2;_tmp544=_tmp541.f3;_tmp545=_tmp541.f4;_LL486: {struct Cyc_Absyn_Tqual tq2=_tmp542;void*t2=_tmp543;struct Cyc_List_List*tvs=_tmp544;struct Cyc_List_List*atts2=_tmp545;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp547=({struct Cyc_Warn_String_Warn_Warg_struct _tmp750;_tmp750.tag=0U,({struct _fat_ptr _tmpA03=({const char*_tmp548="parameter with bad type params";_tag_fat(_tmp548,sizeof(char),31U);});_tmp750.f1=_tmpA03;});_tmp750;});void*_tmp546[1U];_tmp546[0]=& _tmp547;({unsigned _tmpA04=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmpA04,_tag_fat(_tmp546,sizeof(void*),1U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp751;_tmp751.tag=0U,({struct _fat_ptr _tmpA05=({const char*_tmp54B="parameter cannot be qualified with a namespace";_tag_fat(_tmp54B,sizeof(char),47U);});_tmp751.f1=_tmpA05;});_tmp751;});void*_tmp549[1U];_tmp549[0]=& _tmp54A;({unsigned _tmpA06=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA06,_tag_fat(_tmp549,sizeof(void*),1U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp752;_tmp752.tag=0U,({struct _fat_ptr _tmpA07=({const char*_tmp54E="extra attributes on parameter, ignoring";_tag_fat(_tmp54E,sizeof(char),40U);});_tmp752.f1=_tmpA07;});_tmp752;});void*_tmp54C[1U];_tmp54C[0]=& _tmp54D;({unsigned _tmpA08=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA08,_tag_fat(_tmp54C,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->f1=idopt,_tmp54F->f2=tq2,_tmp54F->f3=t2;_tmp54F;}));
# 2182
goto _LL0;}}}}}}case 253U: _LL1F5: _LL1F6: {
# 2183 "parse.y"
struct _tuple26 _tmp550=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp550;struct _tuple26 _tmp551=_stmttmp21;struct Cyc_List_List*_tmp554;struct Cyc_Parse_Type_specifier _tmp553;struct Cyc_Absyn_Tqual _tmp552;_LL488: _tmp552=_tmp551.f1;_tmp553=_tmp551.f2;_tmp554=_tmp551.f3;_LL489: {struct Cyc_Absyn_Tqual tq=_tmp552;struct Cyc_Parse_Type_specifier tspecs=_tmp553;struct Cyc_List_List*atts=_tmp554;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp555=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp555;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp557=({struct Cyc_Warn_String_Warn_Warg_struct _tmp753;_tmp753.tag=0U,({struct _fat_ptr _tmpA09=({const char*_tmp558="bad attributes on parameter, ignoring";_tag_fat(_tmp558,sizeof(char),38U);});_tmp753.f1=_tmpA09;});_tmp753;});void*_tmp556[1U];_tmp556[0]=& _tmp557;({unsigned _tmpA0A=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA0A,_tag_fat(_tmp556,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->f1=0,_tmp559->f2=tq,_tmp559->f3=t;_tmp559;}));
# 2190
goto _LL0;}}}case 254U: _LL1F7: _LL1F8: {
# 2191 "parse.y"
struct _tuple26 _tmp55A=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp22=_tmp55A;struct _tuple26 _tmp55B=_stmttmp22;struct Cyc_List_List*_tmp55E;struct Cyc_Parse_Type_specifier _tmp55D;struct Cyc_Absyn_Tqual _tmp55C;_LL48B: _tmp55C=_tmp55B.f1;_tmp55D=_tmp55B.f2;_tmp55E=_tmp55B.f3;_LL48C: {struct Cyc_Absyn_Tqual tq=_tmp55C;struct Cyc_Parse_Type_specifier tspecs=_tmp55D;struct Cyc_List_List*atts=_tmp55E;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp55F=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp55F;
struct Cyc_List_List*_tmp560=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp560;
struct _tuple14 _tmp561=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp561;struct _tuple14 _tmp562=_stmttmp23;struct Cyc_List_List*_tmp566;struct Cyc_List_List*_tmp565;void*_tmp564;struct Cyc_Absyn_Tqual _tmp563;_LL48E: _tmp563=_tmp562.f1;_tmp564=_tmp562.f2;_tmp565=_tmp562.f3;_tmp566=_tmp562.f4;_LL48F: {struct Cyc_Absyn_Tqual tq2=_tmp563;void*t2=_tmp564;struct Cyc_List_List*tvs=_tmp565;struct Cyc_List_List*atts2=_tmp566;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp568=({struct Cyc_Warn_String_Warn_Warg_struct _tmp754;_tmp754.tag=0U,({
struct _fat_ptr _tmpA0B=({const char*_tmp569="bad type parameters on formal argument, ignoring";_tag_fat(_tmp569,sizeof(char),49U);});_tmp754.f1=_tmpA0B;});_tmp754;});void*_tmp567[1U];_tmp567[0]=& _tmp568;({unsigned _tmpA0C=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA0C,_tag_fat(_tmp567,sizeof(void*),1U));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp755;_tmp755.tag=0U,({struct _fat_ptr _tmpA0D=({const char*_tmp56C="bad attributes on parameter, ignoring";_tag_fat(_tmp56C,sizeof(char),38U);});_tmp755.f1=_tmpA0D;});_tmp755;});void*_tmp56A[1U];_tmp56A[0]=& _tmp56B;({unsigned _tmpA0E=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA0E,_tag_fat(_tmp56A,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->f1=0,_tmp56D->f2=tq2,_tmp56D->f3=t2;_tmp56D;}));
# 2203
goto _LL0;}}}}case 255U: _LL1F9: _LL1FA:
# 2207 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2211 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));({struct _fat_ptr*_tmpA10=({struct _fat_ptr*_tmp56E=_cycalloc(sizeof(*_tmp56E));({struct _fat_ptr _tmpA0F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp56E=_tmpA0F;});_tmp56E;});_tmp56F->hd=_tmpA10;}),_tmp56F->tl=0;_tmp56F;}));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2213 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));({struct _fat_ptr*_tmpA13=({struct _fat_ptr*_tmp570=_cycalloc(sizeof(*_tmp570));({struct _fat_ptr _tmpA12=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp570=_tmpA12;});_tmp570;});_tmp571->hd=_tmpA13;}),({struct Cyc_List_List*_tmpA11=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp571->tl=_tmpA11;});_tmp571;}));
goto _LL0;case 258U: _LL1FF: _LL200:
# 2217 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 259U: _LL201: _LL202:
# 2218 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 260U: _LL203: _LL204:
# 2223 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA14=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=36U,_tmp572->f1=0,_tmp572->f2=0;_tmp572;});Cyc_Absyn_new_exp(_tmpA14,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 261U: _LL205: _LL206:
# 2225 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA16=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=36U,_tmp573->f1=0,({struct Cyc_List_List*_tmpA15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp573->f2=_tmpA15;});_tmp573;});Cyc_Absyn_new_exp(_tmpA16,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208:
# 2227 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA18=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=36U,_tmp574->f1=0,({struct Cyc_List_List*_tmpA17=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp574->f2=_tmpA17;});_tmp574;});Cyc_Absyn_new_exp(_tmpA18,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 263U: _LL209: _LL20A: {
# 2229 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp575=({unsigned _tmpA1D=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA1C=({struct _tuple0*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA1A=({struct _fat_ptr*_tmp577=_cycalloc(sizeof(*_tmp577));({struct _fat_ptr _tmpA19=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp577=_tmpA19;});_tmp577;});_tmp578->f2=_tmpA1A;});_tmp578;});void*_tmpA1B=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA1D,_tmpA1C,_tmpA1B,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2229
struct Cyc_Absyn_Vardecl*vd=_tmp575;
# 2232
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA20=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=27U,_tmp576->f1=vd,({struct Cyc_Absyn_Exp*_tmpA1F=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp576->f2=_tmpA1F;}),({struct Cyc_Absyn_Exp*_tmpA1E=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp576->f3=_tmpA1E;}),_tmp576->f4=0;_tmp576;});Cyc_Absyn_new_exp(_tmpA20,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2235
goto _LL0;}case 264U: _LL20B: _LL20C: {
# 2237 "parse.y"
void*_tmp579=({struct _tuple8*_tmpA21=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA21,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp579;
yyval=Cyc_Exp_tok(({void*_tmpA23=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->tag=28U,({struct Cyc_Absyn_Exp*_tmpA22=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp57A->f1=_tmpA22;}),_tmp57A->f2=t,_tmp57A->f3=0;_tmp57A;});Cyc_Absyn_new_exp(_tmpA23,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2240
goto _LL0;}case 265U: _LL20D: _LL20E:
# 2245 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));({struct _tuple35*_tmpA25=({struct _tuple35*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->f1=0,({struct Cyc_Absyn_Exp*_tmpA24=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp57B->f2=_tmpA24;});_tmp57B;});_tmp57C->hd=_tmpA25;}),_tmp57C->tl=0;_tmp57C;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2247 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct _tuple35*_tmpA28=({struct _tuple35*_tmp57D=_cycalloc(sizeof(*_tmp57D));({struct Cyc_List_List*_tmpA27=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp57D->f1=_tmpA27;}),({struct Cyc_Absyn_Exp*_tmpA26=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp57D->f2=_tmpA26;});_tmp57D;});_tmp57E->hd=_tmpA28;}),_tmp57E->tl=0;_tmp57E;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2249 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp580=_cycalloc(sizeof(*_tmp580));({struct _tuple35*_tmpA2B=({struct _tuple35*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->f1=0,({struct Cyc_Absyn_Exp*_tmpA2A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp57F->f2=_tmpA2A;});_tmp57F;});_tmp580->hd=_tmpA2B;}),({struct Cyc_List_List*_tmpA29=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp580->tl=_tmpA29;});_tmp580;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2251 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp582=_cycalloc(sizeof(*_tmp582));({struct _tuple35*_tmpA2F=({struct _tuple35*_tmp581=_cycalloc(sizeof(*_tmp581));({struct Cyc_List_List*_tmpA2E=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp581->f1=_tmpA2E;}),({struct Cyc_Absyn_Exp*_tmpA2D=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp581->f2=_tmpA2D;});_tmp581;});_tmp582->hd=_tmpA2F;}),({struct Cyc_List_List*_tmpA2C=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp582->tl=_tmpA2C;});_tmp582;}));
goto _LL0;case 269U: _LL215: _LL216:
# 2255 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 270U: _LL217: _LL218:
# 2256 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));({void*_tmpA32=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->tag=1U,({struct _fat_ptr*_tmpA31=({struct _fat_ptr*_tmp583=_cycalloc(sizeof(*_tmp583));({struct _fat_ptr _tmpA30=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp583=_tmpA30;});_tmp583;});_tmp584->f1=_tmpA31;});_tmp584;});_tmp585->hd=_tmpA32;}),_tmp585->tl=0;_tmp585;}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2261 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp586=_cycalloc(sizeof(*_tmp586));({void*_tmpA33=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp586->hd=_tmpA33;}),_tmp586->tl=0;_tmp586;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2262 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({void*_tmpA35=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp587->hd=_tmpA35;}),({struct Cyc_List_List*_tmpA34=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp587->tl=_tmpA34;});_tmp587;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2266 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->tag=0U,({struct Cyc_Absyn_Exp*_tmpA36=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp588->f1=_tmpA36;});_tmp588;}));
goto _LL0;case 274U: _LL21F: _LL220:
# 2267 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->tag=1U,({struct _fat_ptr*_tmpA38=({struct _fat_ptr*_tmp589=_cycalloc(sizeof(*_tmp589));({struct _fat_ptr _tmpA37=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp589=_tmpA37;});_tmp589;});_tmp58A->f1=_tmpA38;});_tmp58A;}));
goto _LL0;case 275U: _LL221: _LL222: {
# 2272 "parse.y"
struct _tuple26 _tmp58B=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp58B;struct _tuple26 _tmp58C=_stmttmp24;struct Cyc_List_List*_tmp58F;struct Cyc_Parse_Type_specifier _tmp58E;struct Cyc_Absyn_Tqual _tmp58D;_LL491: _tmp58D=_tmp58C.f1;_tmp58E=_tmp58C.f2;_tmp58F=_tmp58C.f3;_LL492: {struct Cyc_Absyn_Tqual tq=_tmp58D;struct Cyc_Parse_Type_specifier tss=_tmp58E;struct Cyc_List_List*atts=_tmp58F;
void*_tmp590=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp590;
if(atts != 0)
({void*_tmp591=0U;({unsigned _tmpA3A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA39=({const char*_tmp592="ignoring attributes in type";_tag_fat(_tmp592,sizeof(char),28U);});Cyc_Warn_warn(_tmpA3A,_tmpA39,_tag_fat(_tmp591,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->f1=0,_tmp593->f2=tq,_tmp593->f3=t;_tmp593;}));
# 2278
goto _LL0;}}case 276U: _LL223: _LL224: {
# 2279 "parse.y"
struct _tuple26 _tmp594=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp594;struct _tuple26 _tmp595=_stmttmp25;struct Cyc_List_List*_tmp598;struct Cyc_Parse_Type_specifier _tmp597;struct Cyc_Absyn_Tqual _tmp596;_LL494: _tmp596=_tmp595.f1;_tmp597=_tmp595.f2;_tmp598=_tmp595.f3;_LL495: {struct Cyc_Absyn_Tqual tq=_tmp596;struct Cyc_Parse_Type_specifier tss=_tmp597;struct Cyc_List_List*atts=_tmp598;
void*_tmp599=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp599;
struct Cyc_List_List*_tmp59A=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp59A;
struct _tuple14 _tmp59B=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp59B;
if(t_info.f3 != 0)
# 2285
({void*_tmp59C=0U;({unsigned _tmpA3C=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA3B=({const char*_tmp59D="bad type params, ignoring";_tag_fat(_tmp59D,sizeof(char),26U);});Cyc_Warn_warn(_tmpA3C,_tmpA3B,_tag_fat(_tmp59C,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp59E=0U;({unsigned _tmpA3E=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA3D=({const char*_tmp59F="bad specifiers, ignoring";_tag_fat(_tmp59F,sizeof(char),25U);});Cyc_Warn_warn(_tmpA3E,_tmpA3D,_tag_fat(_tmp59E,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->f1=0,_tmp5A0->f2=t_info.f1,_tmp5A0->f3=t_info.f2;_tmp5A0;}));
# 2290
goto _LL0;}}case 277U: _LL225: _LL226:
# 2293 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA3F=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA3F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 278U: _LL227: _LL228:
# 2294 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 279U: _LL229: _LL22A:
# 2295 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2296 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2297 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));({void*_tmpA41=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A1->hd=_tmpA41;}),({struct Cyc_List_List*_tmpA40=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp5A1->tl=_tmpA40;});_tmp5A1;})));
goto _LL0;case 282U: _LL22F: _LL230:
# 2303 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));({void*_tmpA42=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A2->hd=_tmpA42;}),_tmp5A2->tl=0;_tmp5A2;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2304 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));({void*_tmpA44=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp5A3->hd=_tmpA44;}),({struct Cyc_List_List*_tmpA43=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp5A3->tl=_tmpA43;});_tmp5A3;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2309 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp756;({struct Cyc_List_List*_tmpA45=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp756.tms=_tmpA45;});_tmp756;}));
goto _LL0;case 285U: _LL235: _LL236:
# 2311 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 286U: _LL237: _LL238:
# 2313 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp757;({struct Cyc_List_List*_tmpA47=({struct Cyc_List_List*_tmpA46=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA46,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp757.tms=_tmpA47;});_tmp757;}));
goto _LL0;case 287U: _LL239: _LL23A:
# 2318 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 288U: _LL23B: _LL23C:
# 2320 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp758;({struct Cyc_List_List*_tmpA4A=({struct Cyc_List_List*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));({void*_tmpA49=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A4=_region_malloc(yyr,sizeof(*_tmp5A4));_tmp5A4->tag=0U,({void*_tmpA48=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5A4->f1=_tmpA48;}),_tmp5A4->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5A4;});_tmp5A5->hd=_tmpA49;}),_tmp5A5->tl=0;_tmp5A5;});_tmp758.tms=_tmpA4A;});_tmp758;}));
goto _LL0;case 289U: _LL23D: _LL23E:
# 2322 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp759;({struct Cyc_List_List*_tmpA4E=({struct Cyc_List_List*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));({void*_tmpA4D=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));_tmp5A6->tag=0U,({void*_tmpA4C=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A6->f1=_tmpA4C;}),_tmp5A6->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A6;});_tmp5A7->hd=_tmpA4D;}),({struct Cyc_List_List*_tmpA4B=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5A7->tl=_tmpA4B;});_tmp5A7;});_tmp759.tms=_tmpA4E;});_tmp759;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2324 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75A;({struct Cyc_List_List*_tmpA52=({struct Cyc_List_List*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));({void*_tmpA51=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));_tmp5A8->tag=1U,({struct Cyc_Absyn_Exp*_tmpA50=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5A8->f1=_tmpA50;}),({void*_tmpA4F=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A8->f2=_tmpA4F;}),_tmp5A8->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A8;});_tmp5A9->hd=_tmpA51;}),_tmp5A9->tl=0;_tmp5A9;});_tmp75A.tms=_tmpA52;});_tmp75A;}));
goto _LL0;case 291U: _LL241: _LL242:
# 2326 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75B;({struct Cyc_List_List*_tmpA57=({struct Cyc_List_List*_tmp5AB=_region_malloc(yyr,sizeof(*_tmp5AB));({void*_tmpA56=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));_tmp5AA->tag=1U,({struct Cyc_Absyn_Exp*_tmpA55=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5AA->f1=_tmpA55;}),({void*_tmpA54=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5AA->f2=_tmpA54;}),_tmp5AA->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5AA;});_tmp5AB->hd=_tmpA56;}),({
struct Cyc_List_List*_tmpA53=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5AB->tl=_tmpA53;});_tmp5AB;});
# 2326
_tmp75B.tms=_tmpA57;});_tmp75B;}));
# 2329
goto _LL0;case 292U: _LL243: _LL244:
# 2330 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75C;({struct Cyc_List_List*_tmpA5E=({struct Cyc_List_List*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));({void*_tmpA5D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5AD=_region_malloc(yyr,sizeof(*_tmp5AD));_tmp5AD->tag=3U,({void*_tmpA5C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5AC=_region_malloc(yyr,sizeof(*_tmp5AC));_tmp5AC->tag=1U,_tmp5AC->f1=0,_tmp5AC->f2=0,_tmp5AC->f3=0,({void*_tmpA5B=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5AC->f4=_tmpA5B;}),({struct Cyc_List_List*_tmpA5A=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5AC->f5=_tmpA5A;}),({struct Cyc_Absyn_Exp*_tmpA59=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5AC->f6=_tmpA59;}),({struct Cyc_Absyn_Exp*_tmpA58=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5AC->f7=_tmpA58;});_tmp5AC;});_tmp5AD->f1=_tmpA5C;});_tmp5AD;});_tmp5AE->hd=_tmpA5D;}),_tmp5AE->tl=0;_tmp5AE;});_tmp75C.tms=_tmpA5E;});_tmp75C;}));
# 2332
goto _LL0;case 293U: _LL245: _LL246: {
# 2333 "parse.y"
struct _tuple27*_tmp5AF=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp26=_tmp5AF;struct _tuple27*_tmp5B0=_stmttmp26;struct Cyc_List_List*_tmp5B5;void*_tmp5B4;struct Cyc_Absyn_VarargInfo*_tmp5B3;int _tmp5B2;struct Cyc_List_List*_tmp5B1;_LL497: _tmp5B1=_tmp5B0->f1;_tmp5B2=_tmp5B0->f2;_tmp5B3=_tmp5B0->f3;_tmp5B4=_tmp5B0->f4;_tmp5B5=_tmp5B0->f5;_LL498: {struct Cyc_List_List*lis=_tmp5B1;int b=_tmp5B2;struct Cyc_Absyn_VarargInfo*c=_tmp5B3;void*eff=_tmp5B4;struct Cyc_List_List*po=_tmp5B5;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpA63=({struct Cyc_List_List*_tmp5B8=_region_malloc(yyr,sizeof(*_tmp5B8));({void*_tmpA62=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));_tmp5B7->tag=3U,({void*_tmpA61=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B6=_region_malloc(yyr,sizeof(*_tmp5B6));_tmp5B6->tag=1U,_tmp5B6->f1=lis,_tmp5B6->f2=b,_tmp5B6->f3=c,_tmp5B6->f4=eff,_tmp5B6->f5=po,({struct Cyc_Absyn_Exp*_tmpA60=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5B6->f6=_tmpA60;}),({struct Cyc_Absyn_Exp*_tmpA5F=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B6->f7=_tmpA5F;});_tmp5B6;});_tmp5B7->f1=_tmpA61;});_tmp5B7;});_tmp5B8->hd=_tmpA62;}),_tmp5B8->tl=0;_tmp5B8;});_tmp75D.tms=_tmpA63;});_tmp75D;}));
# 2336
goto _LL0;}}case 294U: _LL247: _LL248:
# 2337 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75E;({struct Cyc_List_List*_tmpA6B=({struct Cyc_List_List*_tmp5BB=_region_malloc(yyr,sizeof(*_tmp5BB));({void*_tmpA6A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BA=_region_malloc(yyr,sizeof(*_tmp5BA));_tmp5BA->tag=3U,({void*_tmpA69=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B9=_region_malloc(yyr,sizeof(*_tmp5B9));_tmp5B9->tag=1U,_tmp5B9->f1=0,_tmp5B9->f2=0,_tmp5B9->f3=0,({void*_tmpA68=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5B9->f4=_tmpA68;}),({struct Cyc_List_List*_tmpA67=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5B9->f5=_tmpA67;}),({struct Cyc_Absyn_Exp*_tmpA66=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5B9->f6=_tmpA66;}),({struct Cyc_Absyn_Exp*_tmpA65=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5B9->f7=_tmpA65;});_tmp5B9;});_tmp5BA->f1=_tmpA69;});_tmp5BA;});_tmp5BB->hd=_tmpA6A;}),({
struct Cyc_List_List*_tmpA64=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5BB->tl=_tmpA64;});_tmp5BB;});
# 2337
_tmp75E.tms=_tmpA6B;});_tmp75E;}));
# 2340
goto _LL0;case 295U: _LL249: _LL24A: {
# 2341 "parse.y"
struct _tuple27*_tmp5BC=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp27=_tmp5BC;struct _tuple27*_tmp5BD=_stmttmp27;struct Cyc_List_List*_tmp5C2;void*_tmp5C1;struct Cyc_Absyn_VarargInfo*_tmp5C0;int _tmp5BF;struct Cyc_List_List*_tmp5BE;_LL49A: _tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;_tmp5C0=_tmp5BD->f3;_tmp5C1=_tmp5BD->f4;_tmp5C2=_tmp5BD->f5;_LL49B: {struct Cyc_List_List*lis=_tmp5BE;int b=_tmp5BF;struct Cyc_Absyn_VarargInfo*c=_tmp5C0;void*eff=_tmp5C1;struct Cyc_List_List*po=_tmp5C2;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75F;({struct Cyc_List_List*_tmpA71=({struct Cyc_List_List*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));({void*_tmpA70=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4->tag=3U,({void*_tmpA6F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));_tmp5C3->tag=1U,_tmp5C3->f1=lis,_tmp5C3->f2=b,_tmp5C3->f3=c,_tmp5C3->f4=eff,_tmp5C3->f5=po,({
struct Cyc_Absyn_Exp*_tmpA6E=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5C3->f6=_tmpA6E;}),({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5C3->f7=_tmpA6D;});_tmp5C3;});
# 2342
_tmp5C4->f1=_tmpA6F;});_tmp5C4;});_tmp5C5->hd=_tmpA70;}),({
struct Cyc_List_List*_tmpA6C=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C5->tl=_tmpA6C;});_tmp5C5;});
# 2342
_tmp75F.tms=_tmpA71;});_tmp75F;}));
# 2345
goto _LL0;}}case 296U: _LL24B: _LL24C: {
# 2347
struct Cyc_List_List*_tmp5C6=({unsigned _tmpA72=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA72,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5C6;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp760;({struct Cyc_List_List*_tmpA75=({struct Cyc_List_List*_tmp5C8=_region_malloc(yyr,sizeof(*_tmp5C8));({void*_tmpA74=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));_tmp5C7->tag=4U,_tmp5C7->f1=ts,_tmp5C7->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5C7->f3=0;_tmp5C7;});_tmp5C8->hd=_tmpA74;}),({
struct Cyc_List_List*_tmpA73=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C8->tl=_tmpA73;});_tmp5C8;});
# 2348
_tmp760.tms=_tmpA75;});_tmp760;}));
# 2351
goto _LL0;}case 297U: _LL24D: _LL24E:
# 2352 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp761;({struct Cyc_List_List*_tmpA79=({struct Cyc_List_List*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));({void*_tmpA78=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5C9=_region_malloc(yyr,sizeof(*_tmp5C9));_tmp5C9->tag=5U,_tmp5C9->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA77=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5C9->f2=_tmpA77;});_tmp5C9;});_tmp5CA->hd=_tmpA78;}),({struct Cyc_List_List*_tmpA76=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CA->tl=_tmpA76;});_tmp5CA;});_tmp761.tms=_tmpA79;});_tmp761;}));
# 2354
goto _LL0;case 298U: _LL24F: _LL250:
# 2358 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299U: _LL251: _LL252:
# 2359 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2360 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2361 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2362 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2363 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 304U: _LL25B: _LL25C:
# 2369 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA7D=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->tag=13U,({struct _fat_ptr*_tmpA7C=({struct _fat_ptr*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));({struct _fat_ptr _tmpA7B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CB=_tmpA7B;});_tmp5CB;});_tmp5CC->f1=_tmpA7C;}),({struct Cyc_Absyn_Stmt*_tmpA7A=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5CC->f2=_tmpA7A;});_tmp5CC;});Cyc_Absyn_new_stmt(_tmpA7D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 305U: _LL25D: _LL25E:
# 2373 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 306U: _LL25F: _LL260:
# 2374 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA7E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 307U: _LL261: _LL262:
# 2379 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 308U: _LL263: _LL264:
# 2380 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 309U: _LL265: _LL266:
# 2385 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7F=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7F,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2386 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA80=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA80,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 311U: _LL269: _LL26A:
# 2387 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA85=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->tag=13U,({struct _fat_ptr*_tmpA84=({struct _fat_ptr*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));({struct _fat_ptr _tmpA83=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CD=_tmpA83;});_tmp5CD;});_tmp5CE->f1=_tmpA84;}),({struct Cyc_Absyn_Stmt*_tmpA82=({struct Cyc_List_List*_tmpA81=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA81,Cyc_Absyn_skip_stmt(0U));});_tmp5CE->f2=_tmpA82;});_tmp5CE;});Cyc_Absyn_new_stmt(_tmpA85,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2389
goto _LL0;case 312U: _LL26B: _LL26C:
# 2389 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA8A=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->tag=13U,({struct _fat_ptr*_tmpA89=({struct _fat_ptr*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));({struct _fat_ptr _tmpA88=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CF=_tmpA88;});_tmp5CF;});_tmp5D0->f1=_tmpA89;}),({struct Cyc_Absyn_Stmt*_tmpA87=({struct Cyc_List_List*_tmpA86=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA86,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5D0->f2=_tmpA87;});_tmp5D0;});Cyc_Absyn_new_stmt(_tmpA8A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2391
goto _LL0;case 313U: _LL26D: _LL26E:
# 2391 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 314U: _LL26F: _LL270:
# 2392 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA8C=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA8B=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA8C,_tmpA8B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 315U: _LL271: _LL272:
# 2393 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA8F=({void*_tmpA8E=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA8D=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D1->f1=_tmpA8D;});_tmp5D1;});Cyc_Absyn_new_decl(_tmpA8E,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA8F,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 316U: _LL273: _LL274:
# 2396 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA92=({void*_tmpA91=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA90=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D2->f1=_tmpA90;});_tmp5D2;});Cyc_Absyn_new_decl(_tmpA91,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA92,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 317U: _LL275: _LL276:
# 2401 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA95=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA94=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA93=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA95,_tmpA94,_tmpA93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318U: _LL277: _LL278:
# 2403 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA98=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA97=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA96=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpA98,_tmpA97,_tmpA96,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A:
# 2409 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpA99=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpA9A,_tmpA99,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320U: _LL27B: _LL27C: {
# 2412
struct Cyc_Absyn_Exp*_tmp5D3=({struct _tuple0*_tmpA9B=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpA9B,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D3;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9D=e;struct Cyc_List_List*_tmpA9C=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpA9D,_tmpA9C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2416
struct Cyc_Absyn_Exp*_tmp5D4=({struct Cyc_List_List*_tmpA9E=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpA9E,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D4;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA0=e;struct Cyc_List_List*_tmpA9F=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpAA0,_tmpA9F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2419
goto _LL0;}case 322U: _LL27F: _LL280:
# 2422 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAA2=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpAA1=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpAA2,_tmpAA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 323U: _LL281: _LL282:
# 2436 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 324U: _LL283: _LL284:
# 2439 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct Cyc_Absyn_Switch_clause*_tmpAA6=({struct Cyc_Absyn_Switch_clause*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));({struct Cyc_Absyn_Pat*_tmpAA5=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5D5->pattern=_tmpAA5;}),_tmp5D5->pat_vars=0,_tmp5D5->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA4=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D5->body=_tmpAA4;}),_tmp5D5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D5;});
# 2439
_tmp5D6->hd=_tmpAA6;}),({
# 2441
struct Cyc_List_List*_tmpAA3=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D6->tl=_tmpAA3;});_tmp5D6;}));
goto _LL0;case 325U: _LL285: _LL286:
# 2443 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct Cyc_Absyn_Switch_clause*_tmpAAA=({struct Cyc_Absyn_Switch_clause*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));({struct Cyc_Absyn_Pat*_tmpAA9=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D7->pattern=_tmpAA9;}),_tmp5D7->pat_vars=0,_tmp5D7->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA8=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5D7->body=_tmpAA8;}),_tmp5D7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D7;});
# 2443
_tmp5D8->hd=_tmpAAA;}),({
# 2445
struct Cyc_List_List*_tmpAA7=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D8->tl=_tmpAA7;});_tmp5D8;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2447 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct Cyc_Absyn_Switch_clause*_tmpAAE=({struct Cyc_Absyn_Switch_clause*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct Cyc_Absyn_Pat*_tmpAAD=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D9->pattern=_tmpAAD;}),_tmp5D9->pat_vars=0,_tmp5D9->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAAC=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5D9->body=_tmpAAC;}),_tmp5D9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D9;});_tmp5DA->hd=_tmpAAE;}),({struct Cyc_List_List*_tmpAAB=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5DA->tl=_tmpAAB;});_tmp5DA;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2449 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct Cyc_Absyn_Switch_clause*_tmpAB3=({struct Cyc_Absyn_Switch_clause*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct Cyc_Absyn_Pat*_tmpAB2=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DB->pattern=_tmpAB2;}),_tmp5DB->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DB->where_clause=_tmpAB1;}),({
struct Cyc_Absyn_Stmt*_tmpAB0=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5DB->body=_tmpAB0;}),_tmp5DB->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DB;});
# 2449
_tmp5DC->hd=_tmpAB3;}),({
# 2451
struct Cyc_List_List*_tmpAAF=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5DC->tl=_tmpAAF;});_tmp5DC;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2453 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));({struct Cyc_Absyn_Switch_clause*_tmpAB8=({struct Cyc_Absyn_Switch_clause*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct Cyc_Absyn_Pat*_tmpAB7=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DD->pattern=_tmpAB7;}),_tmp5DD->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DD->where_clause=_tmpAB6;}),({struct Cyc_Absyn_Stmt*_tmpAB5=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5DD->body=_tmpAB5;}),_tmp5DD->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DD;});_tmp5DE->hd=_tmpAB8;}),({struct Cyc_List_List*_tmpAB4=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5DE->tl=_tmpAB4;});_tmp5DE;}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2460 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpABA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAB9=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpABA,_tmpAB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2464 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpABC=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpABB=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpABC,_tmpABB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 331U: _LL291: _LL292:
# 2468 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpABE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpABD=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2468
Cyc_Absyn_for_stmt(_tmpAC0,_tmpABF,_tmpABE,_tmpABD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2470
goto _LL0;case 332U: _LL293: _LL294:
# 2471 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC3=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAC1=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2471
Cyc_Absyn_for_stmt(_tmpAC4,_tmpAC3,_tmpAC2,_tmpAC1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2473
goto _LL0;case 333U: _LL295: _LL296:
# 2474 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC5=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2474
Cyc_Absyn_for_stmt(_tmpAC8,_tmpAC7,_tmpAC6,_tmpAC5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2476
goto _LL0;case 334U: _LL297: _LL298:
# 2477 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpACB=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpACA=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAC9=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2477
Cyc_Absyn_for_stmt(_tmpACC,_tmpACB,_tmpACA,_tmpAC9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2479
goto _LL0;case 335U: _LL299: _LL29A:
# 2480 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpACF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpACD=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2480
Cyc_Absyn_for_stmt(_tmpAD0,_tmpACF,_tmpACE,_tmpACD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2482
goto _LL0;case 336U: _LL29B: _LL29C:
# 2483 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD3=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAD2=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAD1=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2483
Cyc_Absyn_for_stmt(_tmpAD4,_tmpAD3,_tmpAD2,_tmpAD1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2485
goto _LL0;case 337U: _LL29D: _LL29E:
# 2486 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD5=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2486
Cyc_Absyn_for_stmt(_tmpAD8,_tmpAD7,_tmpAD6,_tmpAD5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2488
goto _LL0;case 338U: _LL29F: _LL2A0:
# 2489 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpADC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpADB=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpADA=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAD9=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2489
Cyc_Absyn_for_stmt(_tmpADC,_tmpADB,_tmpADA,_tmpAD9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2491
goto _LL0;case 339U: _LL2A1: _LL2A2: {
# 2492 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5DF=({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpADF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADD=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2492
Cyc_Absyn_for_stmt(_tmpAE0,_tmpADF,_tmpADE,_tmpADD,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5DF;
# 2494
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAE1=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAE1,s);}));
# 2496
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2497 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E0=({struct Cyc_Absyn_Exp*_tmpAE5=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE4=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAE3=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAE2=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAE5,_tmpAE4,_tmpAE3,_tmpAE2,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E0;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAE6=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAE6,s);}));
# 2500
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2501 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E1=({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE9=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAE7=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAEA,_tmpAE9,_tmpAE8,_tmpAE7,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E1;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAEB=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAEB,s);}));
# 2504
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2505 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5E2=({struct Cyc_Absyn_Exp*_tmpAEF=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAEE=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAED=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAEC=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpAEF,_tmpAEE,_tmpAED,_tmpAEC,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E2;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF0=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAF0,s);}));
# 2508
goto _LL0;}case 343U: _LL2A9: _LL2AA:
# 2511 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpAF2=({struct _fat_ptr*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));({struct _fat_ptr _tmpAF1=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5E3=_tmpAF1;});_tmp5E3;});Cyc_Absyn_goto_stmt(_tmpAF2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2512 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2513 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2514 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2515 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAF3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpAF3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2517 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2518 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2520 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF4=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpAF4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2529 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2532
 yyval=(yyyvsp[0]).v;
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2534 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAF5=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAF7,_tmpAF6,_tmpAF5,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2537
 yyval=(yyyvsp[0]).v;
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2539 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpAF9,_tmpAF8,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2542
 yyval=(yyyvsp[0]).v;
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2544 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpAFB,_tmpAFA,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2547
 yyval=(yyyvsp[0]).v;
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2549 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAFD,_tmpAFC,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2552
 yyval=(yyyvsp[0]).v;
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2554 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAFF,_tmpAFE,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2557
 yyval=(yyyvsp[0]).v;
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2559 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB00=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB01,_tmpB00,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2562
 yyval=(yyyvsp[0]).v;
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2564 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB02=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpB03,_tmpB02,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2566 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpB05,_tmpB04,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2569
 yyval=(yyyvsp[0]).v;
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2571 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB06=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB07,_tmpB06,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2573 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB08=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB09,_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2575 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB0B,_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2577 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB0D,_tmpB0C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2580
 yyval=(yyyvsp[0]).v;
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2582 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2584 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB11,_tmpB10,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2587
 yyval=(yyyvsp[0]).v;
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2589 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB12=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB13,_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2591 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB15,_tmpB14,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2594
 yyval=(yyyvsp[0]).v;
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2596 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB17,_tmpB16,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2598 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB19,_tmpB18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2600 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB1B,_tmpB1A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 382U: _LL2F7: _LL2F8:
# 2603
 yyval=(yyyvsp[0]).v;
goto _LL0;case 383U: _LL2F9: _LL2FA: {
# 2605 "parse.y"
void*_tmp5E4=({struct _tuple8*_tmpB1C=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB1C,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5E4;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB1E=t;struct Cyc_Absyn_Exp*_tmpB1D=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB1E,_tmpB1D,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2608
goto _LL0;}case 384U: _LL2FB: _LL2FC:
# 2611 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2614
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 386U: _LL2FF: _LL300:
# 2616 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB20=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB1F=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB20,_tmpB1F,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 387U: _LL301: _LL302: {
# 2618 "parse.y"
void*_tmp5E5=({struct _tuple8*_tmpB21=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB21,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5E5;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2621
goto _LL0;}case 388U: _LL303: _LL304:
# 2622 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB22=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB22,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 389U: _LL305: _LL306:
# 2624 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB24=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB23=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB24,_tmpB23,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 390U: _LL307: _LL308:
# 2629 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 391U: _LL309: _LL30A:
# 2637 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 392U: _LL30B: _LL30C:
# 2642 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 393U: _LL30D: _LL30E:
# 2644 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 394U: _LL30F: _LL310: {
# 2646 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5E6=e->r;void*_stmttmp28=_tmp5E6;void*_tmp5E7=_stmttmp28;int _tmp5E9;struct _fat_ptr _tmp5E8;int _tmp5EB;enum Cyc_Absyn_Sign _tmp5EA;short _tmp5ED;enum Cyc_Absyn_Sign _tmp5EC;char _tmp5EF;enum Cyc_Absyn_Sign _tmp5EE;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).LongLong_c).tag){case 2U: _LL49D: _tmp5EE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Char_c).val).f1;_tmp5EF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Char_c).val).f2;_LL49E: {enum Cyc_Absyn_Sign s=_tmp5EE;char i=_tmp5EF;
# 2650
yyval=Cyc_YY9(({void*_tmpB25=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->tag=11U,_tmp5F0->f1=i;_tmp5F0;});Cyc_Absyn_new_pat(_tmpB25,e->loc);}));goto _LL49C;}case 4U: _LL49F: _tmp5EC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Short_c).val).f1;_tmp5ED=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Short_c).val).f2;_LL4A0: {enum Cyc_Absyn_Sign s=_tmp5EC;short i=_tmp5ED;
# 2652
yyval=Cyc_YY9(({void*_tmpB26=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->tag=10U,_tmp5F1->f1=s,_tmp5F1->f2=(int)i;_tmp5F1;});Cyc_Absyn_new_pat(_tmpB26,e->loc);}));goto _LL49C;}case 5U: _LL4A1: _tmp5EA=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Int_c).val).f1;_tmp5EB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Int_c).val).f2;_LL4A2: {enum Cyc_Absyn_Sign s=_tmp5EA;int i=_tmp5EB;
# 2654
yyval=Cyc_YY9(({void*_tmpB27=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=10U,_tmp5F2->f1=s,_tmp5F2->f2=i;_tmp5F2;});Cyc_Absyn_new_pat(_tmpB27,e->loc);}));goto _LL49C;}case 7U: _LL4A3: _tmp5E8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Float_c).val).f1;_tmp5E9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E7)->f1).Float_c).val).f2;_LL4A4: {struct _fat_ptr s=_tmp5E8;int i=_tmp5E9;
# 2656
yyval=Cyc_YY9(({void*_tmpB28=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->tag=12U,_tmp5F3->f1=s,_tmp5F3->f2=i;_tmp5F3;});Cyc_Absyn_new_pat(_tmpB28,e->loc);}));goto _LL49C;}case 1U: _LL4A5: _LL4A6:
# 2658
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL49C;case 8U: _LL4A7: _LL4A8:
 goto _LL4AA;case 9U: _LL4A9: _LL4AA:
# 2661
({void*_tmp5F4=0U;({unsigned _tmpB2A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB29=({const char*_tmp5F5="strings cannot occur within patterns";_tag_fat(_tmp5F5,sizeof(char),37U);});Cyc_Warn_err(_tmpB2A,_tmpB29,_tag_fat(_tmp5F4,sizeof(void*),0U));});});goto _LL49C;case 6U: _LL4AB: _LL4AC:
# 2663
({void*_tmp5F6=0U;({unsigned _tmpB2C=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB2B=({const char*_tmp5F7="long long's in patterns not yet implemented";_tag_fat(_tmp5F7,sizeof(char),44U);});Cyc_Warn_err(_tmpB2C,_tmpB2B,_tag_fat(_tmp5F6,sizeof(void*),0U));});});goto _LL49C;default: goto _LL4AD;}else{_LL4AD: _LL4AE:
# 2665
({void*_tmp5F8=0U;({unsigned _tmpB2E=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB2D=({const char*_tmp5F9="bad constant in case";_tag_fat(_tmp5F9,sizeof(char),21U);});Cyc_Warn_err(_tmpB2E,_tmpB2D,_tag_fat(_tmp5F8,sizeof(void*),0U));});});}_LL49C:;}
# 2668
goto _LL0;}case 395U: _LL311: _LL312:
# 2669 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB30=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->tag=15U,({struct _tuple0*_tmpB2F=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp5FA->f1=_tmpB2F;});_tmp5FA;});Cyc_Absyn_new_pat(_tmpB30,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 396U: _LL313: _LL314:
# 2671 "parse.y"
 if(({struct _fat_ptr _tmpB31=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB31,({const char*_tmp5FB="as";_tag_fat(_tmp5FB,sizeof(char),3U);}));})!= 0)
({void*_tmp5FC=0U;({unsigned _tmpB33=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB32=({const char*_tmp5FD="expecting `as'";_tag_fat(_tmp5FD,sizeof(char),15U);});Cyc_Warn_err(_tmpB33,_tmpB32,_tag_fat(_tmp5FC,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB3A=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB39=({unsigned _tmpB38=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB37=({struct _tuple0*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB36=({struct _fat_ptr*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));({struct _fat_ptr _tmpB35=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5FE=_tmpB35;});_tmp5FE;});_tmp5FF->f2=_tmpB36;});_tmp5FF;});Cyc_Absyn_new_vardecl(_tmpB38,_tmpB37,Cyc_Absyn_void_type,0);});_tmp600->f1=_tmpB39;}),({
struct Cyc_Absyn_Pat*_tmpB34=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp600->f2=_tmpB34;});_tmp600;});
# 2673
Cyc_Absyn_new_pat(_tmpB3A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2676
goto _LL0;case 397U: _LL315: _LL316:
# 2677 "parse.y"
 if(({struct _fat_ptr _tmpB3B=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB3B,({const char*_tmp601="alias";_tag_fat(_tmp601,sizeof(char),6U);}));})!= 0)
({void*_tmp602=0U;({unsigned _tmpB3D=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB3C=({const char*_tmp603="expecting `alias'";_tag_fat(_tmp603,sizeof(char),18U);});Cyc_Warn_err(_tmpB3D,_tmpB3C,_tag_fat(_tmp602,sizeof(void*),0U));});});{
int _tmp604=((yyyvsp[0]).l).first_line;int location=_tmp604;
({struct _fat_ptr _tmpB3E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB3E,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp60A=_cycalloc(sizeof(*_tmp60A));({struct _fat_ptr*_tmpB41=({struct _fat_ptr*_tmp608=_cycalloc(sizeof(*_tmp608));({struct _fat_ptr _tmpB40=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp608=_tmpB40;});_tmp608;});_tmp60A->name=_tmpB41;}),_tmp60A->identity=- 1,({void*_tmpB3F=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->tag=0U,_tmp609->f1=& Cyc_Kinds_rk;_tmp609;});_tmp60A->kind=_tmpB3F;});_tmp60A;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB46=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB45=({struct _tuple0*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB43=({struct _fat_ptr*_tmp606=_cycalloc(sizeof(*_tmp606));({struct _fat_ptr _tmpB42=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp606=_tmpB42;});_tmp606;});_tmp607->f2=_tmpB43;});_tmp607;});Cyc_Absyn_new_vardecl(_tmpB46,_tmpB45,({
struct _tuple8*_tmpB44=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB44,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB47=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->tag=2U,_tmp605->f1=tv,_tmp605->f2=vd;_tmp605;});Cyc_Absyn_new_pat(_tmpB47,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2686
goto _LL0;}}case 398U: _LL317: _LL318:
# 2687 "parse.y"
 if(({struct _fat_ptr _tmpB48=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB48,({const char*_tmp60B="alias";_tag_fat(_tmp60B,sizeof(char),6U);}));})!= 0)
({void*_tmp60C=0U;({unsigned _tmpB4A=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB49=({const char*_tmp60D="expecting `alias'";_tag_fat(_tmp60D,sizeof(char),18U);});Cyc_Warn_err(_tmpB4A,_tmpB49,_tag_fat(_tmp60C,sizeof(void*),0U));});});{
int _tmp60E=((yyyvsp[0]).l).first_line;int location=_tmp60E;
({struct _fat_ptr _tmpB4B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB4B,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp614=_cycalloc(sizeof(*_tmp614));({struct _fat_ptr*_tmpB4E=({struct _fat_ptr*_tmp612=_cycalloc(sizeof(*_tmp612));({struct _fat_ptr _tmpB4D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp612=_tmpB4D;});_tmp612;});_tmp614->name=_tmpB4E;}),_tmp614->identity=- 1,({void*_tmpB4C=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->tag=0U,_tmp613->f1=& Cyc_Kinds_rk;_tmp613;});_tmp614->kind=_tmpB4C;});_tmp614;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB53=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB52=({struct _tuple0*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB50=({struct _fat_ptr*_tmp610=_cycalloc(sizeof(*_tmp610));({struct _fat_ptr _tmpB4F=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp610=_tmpB4F;});_tmp610;});_tmp611->f2=_tmpB50;});_tmp611;});Cyc_Absyn_new_vardecl(_tmpB53,_tmpB52,({
struct _tuple8*_tmpB51=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB51,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB54=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->tag=2U,_tmp60F->f1=tv,_tmp60F->f2=vd;_tmp60F;});Cyc_Absyn_new_pat(_tmpB54,(unsigned)location);}));
# 2696
goto _LL0;}}case 399U: _LL319: _LL31A: {
# 2697 "parse.y"
struct _tuple23 _tmp615=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp29=_tmp615;struct _tuple23 _tmp616=_stmttmp29;int _tmp618;struct Cyc_List_List*_tmp617;_LL4B0: _tmp617=_tmp616.f1;_tmp618=_tmp616.f2;_LL4B1: {struct Cyc_List_List*ps=_tmp617;int dots=_tmp618;
yyval=Cyc_YY9(({void*_tmpB55=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=5U,_tmp619->f1=ps,_tmp619->f2=dots;_tmp619;});Cyc_Absyn_new_pat(_tmpB55,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2700
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2701 "parse.y"
struct _tuple23 _tmp61A=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp61A;struct _tuple23 _tmp61B=_stmttmp2A;int _tmp61D;struct Cyc_List_List*_tmp61C;_LL4B3: _tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;_LL4B4: {struct Cyc_List_List*ps=_tmp61C;int dots=_tmp61D;
yyval=Cyc_YY9(({void*_tmpB57=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tag=16U,({struct _tuple0*_tmpB56=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp61E->f1=_tmpB56;}),_tmp61E->f2=ps,_tmp61E->f3=dots;_tmp61E;});Cyc_Absyn_new_pat(_tmpB57,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2704
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2705 "parse.y"
struct _tuple23 _tmp61F=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2B=_tmp61F;struct _tuple23 _tmp620=_stmttmp2B;int _tmp622;struct Cyc_List_List*_tmp621;_LL4B6: _tmp621=_tmp620.f1;_tmp622=_tmp620.f2;_LL4B7: {struct Cyc_List_List*fps=_tmp621;int dots=_tmp622;
struct Cyc_List_List*_tmp623=({unsigned _tmpB58=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB58,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp623;
yyval=Cyc_YY9(({void*_tmpB5B=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB5A=({union Cyc_Absyn_AggrInfo*_tmp624=_cycalloc(sizeof(*_tmp624));({union Cyc_Absyn_AggrInfo _tmpB59=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp624=_tmpB59;});_tmp624;});_tmp625->f1=_tmpB5A;}),_tmp625->f2=exist_ts,_tmp625->f3=fps,_tmp625->f4=dots;_tmp625;});Cyc_Absyn_new_pat(_tmpB5B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2710
goto _LL0;}}case 402U: _LL31F: _LL320: {
# 2711 "parse.y"
struct _tuple23 _tmp626=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp626;struct _tuple23 _tmp627=_stmttmp2C;int _tmp629;struct Cyc_List_List*_tmp628;_LL4B9: _tmp628=_tmp627.f1;_tmp629=_tmp627.f2;_LL4BA: {struct Cyc_List_List*fps=_tmp628;int dots=_tmp629;
struct Cyc_List_List*_tmp62A=({unsigned _tmpB5C=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB5C,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp62A;
yyval=Cyc_YY9(({void*_tmpB5D=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->tag=7U,_tmp62B->f1=0,_tmp62B->f2=exist_ts,_tmp62B->f3=fps,_tmp62B->f4=dots;_tmp62B;});Cyc_Absyn_new_pat(_tmpB5D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2715
goto _LL0;}}case 403U: _LL321: _LL322:
# 2716 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB5F=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5E=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp62C->f1=_tmpB5E;});_tmp62C;});Cyc_Absyn_new_pat(_tmpB5F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404U: _LL323: _LL324:
# 2718 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB63=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->tag=6U,({struct Cyc_Absyn_Pat*_tmpB62=({void*_tmpB61=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->tag=6U,({struct Cyc_Absyn_Pat*_tmpB60=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp62D->f1=_tmpB60;});_tmp62D;});Cyc_Absyn_new_pat(_tmpB61,(unsigned)((yyyvsp[0]).l).first_line);});_tmp62E->f1=_tmpB62;});_tmp62E;});Cyc_Absyn_new_pat(_tmpB63,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 405U: _LL325: _LL326:
# 2720 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB6A=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB69=({unsigned _tmpB68=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB67=({struct _tuple0*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB66=({struct _fat_ptr*_tmp62F=_cycalloc(sizeof(*_tmp62F));({struct _fat_ptr _tmpB65=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp62F=_tmpB65;});_tmp62F;});_tmp630->f2=_tmpB66;});_tmp630;});Cyc_Absyn_new_vardecl(_tmpB68,_tmpB67,Cyc_Absyn_void_type,0);});_tmp631->f1=_tmpB69;}),({
# 2722
struct Cyc_Absyn_Pat*_tmpB64=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp631->f2=_tmpB64;});_tmp631;});
# 2720
Cyc_Absyn_new_pat(_tmpB6A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2724
goto _LL0;case 406U: _LL327: _LL328:
# 2725 "parse.y"
 if(({struct _fat_ptr _tmpB6B=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB6B,({const char*_tmp632="as";_tag_fat(_tmp632,sizeof(char),3U);}));})!= 0)
({void*_tmp633=0U;({unsigned _tmpB6D=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB6C=({const char*_tmp634="expecting `as'";_tag_fat(_tmp634,sizeof(char),15U);});Cyc_Warn_err(_tmpB6D,_tmpB6C,_tag_fat(_tmp633,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB74=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB73=({unsigned _tmpB72=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB71=({struct _tuple0*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB70=({struct _fat_ptr*_tmp635=_cycalloc(sizeof(*_tmp635));({struct _fat_ptr _tmpB6F=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp635=_tmpB6F;});_tmp635;});_tmp636->f2=_tmpB70;});_tmp636;});Cyc_Absyn_new_vardecl(_tmpB72,_tmpB71,Cyc_Absyn_void_type,0);});_tmp637->f1=_tmpB73;}),({
# 2729
struct Cyc_Absyn_Pat*_tmpB6E=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp637->f2=_tmpB6E;});_tmp637;});
# 2727
Cyc_Absyn_new_pat(_tmpB74,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2731
goto _LL0;case 407U: _LL329: _LL32A: {
# 2732 "parse.y"
void*_tmp638=({struct _fat_ptr _tmpB75=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB75,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp638;
yyval=Cyc_YY9(({void*_tmpB7C=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB7B=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp63B->f1=_tmpB7B;}),({
struct Cyc_Absyn_Vardecl*_tmpB7A=({unsigned _tmpB79=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB78=({struct _tuple0*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB77=({struct _fat_ptr*_tmp639=_cycalloc(sizeof(*_tmp639));({struct _fat_ptr _tmpB76=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp639=_tmpB76;});_tmp639;});_tmp63A->f2=_tmpB77;});_tmp63A;});Cyc_Absyn_new_vardecl(_tmpB79,_tmpB78,
Cyc_Absyn_tag_type(tag),0);});
# 2734
_tmp63B->f2=_tmpB7A;});_tmp63B;});
# 2733
Cyc_Absyn_new_pat(_tmpB7C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2737
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2738 "parse.y"
struct Cyc_Absyn_Tvar*_tmp63C=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp63C;
yyval=Cyc_YY9(({void*_tmpB82=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->tag=4U,_tmp63F->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB81=({unsigned _tmpB80=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB7F=({struct _tuple0*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB7E=({struct _fat_ptr*_tmp63D=_cycalloc(sizeof(*_tmp63D));({struct _fat_ptr _tmpB7D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp63D=_tmpB7D;});_tmp63D;});_tmp63E->f2=_tmpB7E;});_tmp63E;});Cyc_Absyn_new_vardecl(_tmpB80,_tmpB7F,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2740
_tmp63F->f2=_tmpB81;});_tmp63F;});
# 2739
Cyc_Absyn_new_pat(_tmpB82,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2743
goto _LL0;}case 409U: _LL32D: _LL32E:
# 2746 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp640=_cycalloc(sizeof(*_tmp640));({struct Cyc_List_List*_tmpB83=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp640->f1=_tmpB83;}),_tmp640->f2=0;_tmp640;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2747 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp641=_cycalloc(sizeof(*_tmp641));({struct Cyc_List_List*_tmpB84=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp641->f1=_tmpB84;}),_tmp641->f2=1;_tmp641;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2748 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->f1=0,_tmp642->f2=1;_tmp642;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2753 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp643=_cycalloc(sizeof(*_tmp643));({struct Cyc_Absyn_Pat*_tmpB85=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp643->hd=_tmpB85;}),_tmp643->tl=0;_tmp643;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2755 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_Absyn_Pat*_tmpB87=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp644->hd=_tmpB87;}),({struct Cyc_List_List*_tmpB86=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp644->tl=_tmpB86;});_tmp644;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2760 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=0,({struct Cyc_Absyn_Pat*_tmpB88=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp645->f2=_tmpB88;});_tmp645;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2762 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp646=_cycalloc(sizeof(*_tmp646));({struct Cyc_List_List*_tmpB8A=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp646->f1=_tmpB8A;}),({struct Cyc_Absyn_Pat*_tmpB89=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp646->f2=_tmpB89;});_tmp646;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2765
 yyval=Cyc_YY14(({struct _tuple23*_tmp647=_cycalloc(sizeof(*_tmp647));({struct Cyc_List_List*_tmpB8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp647->f1=_tmpB8B;}),_tmp647->f2=0;_tmp647;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2766 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp648=_cycalloc(sizeof(*_tmp648));({struct Cyc_List_List*_tmpB8C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp648->f1=_tmpB8C;}),_tmp648->f2=1;_tmp648;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2767 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=0,_tmp649->f2=1;_tmp649;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2772 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct _tuple24*_tmpB8D=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp64A->hd=_tmpB8D;}),_tmp64A->tl=0;_tmp64A;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2774 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp64B=_cycalloc(sizeof(*_tmp64B));({struct _tuple24*_tmpB8F=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp64B->hd=_tmpB8F;}),({struct Cyc_List_List*_tmpB8E=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp64B->tl=_tmpB8E;});_tmp64B;}));
goto _LL0;case 421U: _LL345: _LL346:
# 2780 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 422U: _LL347: _LL348:
# 2782 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB91=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB91,_tmpB90,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 423U: _LL349: _LL34A:
# 2787 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 424U: _LL34B: _LL34C:
# 2789 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB94=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB93=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpB94,_tmpB93,_tmpB92,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2791 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB95=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpB96,_tmpB95,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 426U: _LL34F: _LL350:
# 2795 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 427U: _LL351: _LL352:
# 2796 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->v=(void*)Cyc_Absyn_Times;_tmp64C;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2797 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->v=(void*)Cyc_Absyn_Div;_tmp64D;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2798 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->v=(void*)Cyc_Absyn_Mod;_tmp64E;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2799 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->v=(void*)Cyc_Absyn_Plus;_tmp64F;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2800 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->v=(void*)Cyc_Absyn_Minus;_tmp650;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2801 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->v=(void*)Cyc_Absyn_Bitlshift;_tmp651;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2802 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->v=(void*)Cyc_Absyn_Bitlrshift;_tmp652;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2803 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->v=(void*)Cyc_Absyn_Bitand;_tmp653;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2804 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)Cyc_Absyn_Bitxor;_tmp654;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2805 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Absyn_Bitor;_tmp655;}));
goto _LL0;case 437U: _LL365: _LL366:
# 2810 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 438U: _LL367: _LL368:
# 2812 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB98=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB99,_tmpB98,_tmpB97,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 439U: _LL369: _LL36A:
# 2815
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpB9A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2818
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB9B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2820 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB9C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2822 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB9E,_tmpB9D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2824 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBA0,_tmpB9F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 444U: _LL373: _LL374:
# 2828 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2832 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446U: _LL377: _LL378:
# 2834 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpBA2,_tmpBA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 447U: _LL379: _LL37A:
# 2838 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 448U: _LL37B: _LL37C:
# 2840 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpBA4,_tmpBA3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 449U: _LL37D: _LL37E:
# 2844 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 450U: _LL37F: _LL380:
# 2846 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpBA6,_tmpBA5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 451U: _LL381: _LL382:
# 2850 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 452U: _LL383: _LL384:
# 2852 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBA8,_tmpBA7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 453U: _LL385: _LL386:
# 2856 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 454U: _LL387: _LL388:
# 2858 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBAA,_tmpBA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 455U: _LL389: _LL38A:
# 2862 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 456U: _LL38B: _LL38C:
# 2864 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBAC,_tmpBAB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 457U: _LL38D: _LL38E:
# 2866 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBAE,_tmpBAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 458U: _LL38F: _LL390:
# 2870 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 459U: _LL391: _LL392:
# 2872 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBB0,_tmpBAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 460U: _LL393: _LL394:
# 2874 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBB2,_tmpBB1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2876 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBB4,_tmpBB3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2878 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBB6,_tmpBB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463U: _LL399: _LL39A:
# 2882 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 464U: _LL39B: _LL39C:
# 2884 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBB8,_tmpBB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2886 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBBA,_tmpBB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2890 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2892 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBBC,_tmpBBB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 2894 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBBE,_tmpBBD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 2898 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 2900 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBC0,_tmpBBF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 2902 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBC2,_tmpBC1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 2904 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBC4,_tmpBC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 473U: _LL3AD: _LL3AE:
# 2908 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 474U: _LL3AF: _LL3B0: {
# 2910 "parse.y"
void*_tmp656=({struct _tuple8*_tmpBC5=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBC5,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp656;
yyval=Cyc_Exp_tok(({void*_tmpBC7=t;struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBC7,_tmpBC6,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2913
goto _LL0;}case 475U: _LL3B1: _LL3B2:
# 2916 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 476U: _LL3B3: _LL3B4:
# 2917 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC8,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 2918 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC9,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 2919 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCA=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBCA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 2920 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCB=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBCB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 2921 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCC=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBCC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE:
# 2922 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBCE=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCD=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBCE,_tmpBCD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 482U: _LL3BF: _LL3C0: {
# 2924 "parse.y"
void*_tmp657=({struct _tuple8*_tmpBCF=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBCF,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp657;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2927
goto _LL0;}case 483U: _LL3C1: _LL3C2:
# 2927 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBD0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 484U: _LL3C3: _LL3C4: {
# 2929 "parse.y"
void*_tmp658=({struct _tuple8*_tmpBD1=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBD1,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp658;
yyval=Cyc_Exp_tok(({void*_tmpBD3=t;struct Cyc_List_List*_tmpBD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBD3,_tmpBD2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2932
goto _LL0;}case 485U: _LL3C5: _LL3C6:
# 2934
 yyval=Cyc_Exp_tok(({void*_tmpBD5=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->tag=34U,(_tmp659->f1).is_calloc=0,(_tmp659->f1).rgn=0,(_tmp659->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp659->f1).num_elts=_tmpBD4;}),(_tmp659->f1).fat_result=0,(_tmp659->f1).inline_call=0;_tmp659;});Cyc_Absyn_new_exp(_tmpBD5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2936
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 2937 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD8=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->tag=34U,(_tmp65A->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65A->f1).rgn=_tmpBD7;}),(_tmp65A->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD6=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp65A->f1).num_elts=_tmpBD6;}),(_tmp65A->f1).fat_result=0,(_tmp65A->f1).inline_call=0;_tmp65A;});Cyc_Absyn_new_exp(_tmpBD8,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2939
goto _LL0;case 487U: _LL3C9: _LL3CA:
# 2940 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBDB=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->tag=34U,(_tmp65B->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBDA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65B->f1).rgn=_tmpBDA;}),(_tmp65B->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp65B->f1).num_elts=_tmpBD9;}),(_tmp65B->f1).fat_result=0,(_tmp65B->f1).inline_call=1;_tmp65B;});Cyc_Absyn_new_exp(_tmpBDB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2942
goto _LL0;case 488U: _LL3CB: _LL3CC: {
# 2943 "parse.y"
void*_tmp65C=({struct _tuple8*_tmpBDC=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBDC,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp65C;
yyval=Cyc_Exp_tok(({void*_tmpBDF=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->tag=34U,(_tmp65E->f1).is_calloc=1,(_tmp65E->f1).rgn=0,({void**_tmpBDE=({void**_tmp65D=_cycalloc(sizeof(*_tmp65D));*_tmp65D=t;_tmp65D;});(_tmp65E->f1).elt_type=_tmpBDE;}),({struct Cyc_Absyn_Exp*_tmpBDD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65E->f1).num_elts=_tmpBDD;}),(_tmp65E->f1).fat_result=0,(_tmp65E->f1).inline_call=0;_tmp65E;});Cyc_Absyn_new_exp(_tmpBDF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2946
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 2948
void*_tmp65F=({struct _tuple8*_tmpBE0=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBE0,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp65F;
yyval=Cyc_Exp_tok(({void*_tmpBE4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=34U,(_tmp661->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBE3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp661->f1).rgn=_tmpBE3;}),({void**_tmpBE2=({void**_tmp660=_cycalloc(sizeof(*_tmp660));*_tmp660=t;_tmp660;});(_tmp661->f1).elt_type=_tmpBE2;}),({struct Cyc_Absyn_Exp*_tmpBE1=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp661->f1).num_elts=_tmpBE1;}),(_tmp661->f1).fat_result=0,(_tmp661->f1).inline_call=0;_tmp661;});Cyc_Absyn_new_exp(_tmpBE4,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2951
goto _LL0;}case 490U: _LL3CF: _LL3D0:
# 2952 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBE6=({struct Cyc_Absyn_Exp*_tmp662[1U];({struct Cyc_Absyn_Exp*_tmpBE5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp662[0]=_tmpBE5;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp662,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBE6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 2954 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBEA=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=38U,({struct Cyc_Absyn_Exp*_tmpBE9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp664->f1=_tmpBE9;}),({struct _fat_ptr*_tmpBE8=({struct _fat_ptr*_tmp663=_cycalloc(sizeof(*_tmp663));({struct _fat_ptr _tmpBE7=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp663=_tmpBE7;});_tmp663;});_tmp664->f2=_tmpBE8;});_tmp664;});Cyc_Absyn_new_exp(_tmpBEA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4:
# 2956 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBEF=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->tag=38U,({struct Cyc_Absyn_Exp*_tmpBEE=({struct Cyc_Absyn_Exp*_tmpBED=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpBED,(unsigned)((yyyvsp[2]).l).first_line);});_tmp666->f1=_tmpBEE;}),({struct _fat_ptr*_tmpBEC=({struct _fat_ptr*_tmp665=_cycalloc(sizeof(*_tmp665));({struct _fat_ptr _tmpBEB=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp665=_tmpBEB;});_tmp665;});_tmp666->f2=_tmpBEC;});_tmp666;});Cyc_Absyn_new_exp(_tmpBEF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 493U: _LL3D5: _LL3D6: {
# 2958 "parse.y"
void*_tmp667=({struct _tuple8*_tmpBF0=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBF0,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp667;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 494U: _LL3D7: _LL3D8:
# 2961 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF1=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpBF1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA:
# 2962 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBF2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpBF2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC:
# 2964 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBF3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpBF3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 497U: _LL3DD: _LL3DE: {
# 2969 "parse.y"
struct _tuple29*_tmp668=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp2D=_tmp668;struct _tuple29*_tmp669=_stmttmp2D;struct Cyc_List_List*_tmp66C;struct Cyc_List_List*_tmp66B;struct Cyc_List_List*_tmp66A;_LL4BC: _tmp66A=_tmp669->f1;_tmp66B=_tmp669->f2;_tmp66C=_tmp669->f3;_LL4BD: {struct Cyc_List_List*outlist=_tmp66A;struct Cyc_List_List*inlist=_tmp66B;struct Cyc_List_List*clobbers=_tmp66C;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->tag=40U,({int _tmpBF5=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp66D->f1=_tmpBF5;}),({struct _fat_ptr _tmpBF4=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp66D->f2=_tmpBF4;}),_tmp66D->f3=outlist,_tmp66D->f4=inlist,_tmp66D->f5=clobbers;_tmp66D;}));
goto _LL0;}}case 498U: _LL3DF: _LL3E0:
# 2974 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 499U: _LL3E1: _LL3E2:
# 2975 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 500U: _LL3E3: _LL3E4:
# 2979 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->f1=0,_tmp66E->f2=0,_tmp66E->f3=0;_tmp66E;}));
goto _LL0;case 501U: _LL3E5: _LL3E6: {
# 2981 "parse.y"
struct _tuple30*_tmp66F=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp2E=_tmp66F;struct _tuple30*_tmp670=_stmttmp2E;struct Cyc_List_List*_tmp672;struct Cyc_List_List*_tmp671;_LL4BF: _tmp671=_tmp670->f1;_tmp672=_tmp670->f2;_LL4C0: {struct Cyc_List_List*inlist=_tmp671;struct Cyc_List_List*clobbers=_tmp672;
yyval=Cyc_YY59(({struct _tuple29*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->f1=0,_tmp673->f2=inlist,_tmp673->f3=clobbers;_tmp673;}));
goto _LL0;}}case 502U: _LL3E7: _LL3E8: {
# 2984 "parse.y"
struct _tuple30*_tmp674=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp2F=_tmp674;struct _tuple30*_tmp675=_stmttmp2F;struct Cyc_List_List*_tmp677;struct Cyc_List_List*_tmp676;_LL4C2: _tmp676=_tmp675->f1;_tmp677=_tmp675->f2;_LL4C3: {struct Cyc_List_List*inlist=_tmp676;struct Cyc_List_List*clobbers=_tmp677;
yyval=Cyc_YY59(({struct _tuple29*_tmp678=_cycalloc(sizeof(*_tmp678));({struct Cyc_List_List*_tmpBF6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp678->f1=_tmpBF6;}),_tmp678->f2=inlist,_tmp678->f3=clobbers;_tmp678;}));
goto _LL0;}}case 503U: _LL3E9: _LL3EA:
# 2989 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp679=_cycalloc(sizeof(*_tmp679));({struct _tuple31*_tmpBF7=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp679->hd=_tmpBF7;}),_tmp679->tl=0;_tmp679;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 2990 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));({struct _tuple31*_tmpBF9=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp67A->hd=_tmpBF9;}),({struct Cyc_List_List*_tmpBF8=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp67A->tl=_tmpBF8;});_tmp67A;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 2994 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->f1=0,_tmp67B->f2=0;_tmp67B;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 2996 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->f1=0,({struct Cyc_List_List*_tmpBFA=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp67C->f2=_tmpBFA;});_tmp67C;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 2998 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp67D=_cycalloc(sizeof(*_tmp67D));({struct Cyc_List_List*_tmpBFC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp67D->f1=_tmpBFC;}),({struct Cyc_List_List*_tmpBFB=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp67D->f2=_tmpBFB;});_tmp67D;}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 3002 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67E=_cycalloc(sizeof(*_tmp67E));({struct _tuple31*_tmpBFD=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp67E->hd=_tmpBFD;}),_tmp67E->tl=0;_tmp67E;}));
goto _LL0;case 509U: _LL3F5: _LL3F6:
# 3003 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct _tuple31*_tmpBFF=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp67F->hd=_tmpBFF;}),({struct Cyc_List_List*_tmpBFE=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp67F->tl=_tmpBFE;});_tmp67F;}));
goto _LL0;case 510U: _LL3F7: _LL3F8: {
# 3008 "parse.y"
struct Cyc_Absyn_Exp*_tmp680=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp680;
yyval=Cyc_YY63(({struct _tuple31*_tmp681=_cycalloc(sizeof(*_tmp681));({struct _fat_ptr _tmpC01=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp681->f1=_tmpC01;}),({struct Cyc_Absyn_Exp*_tmpC00=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp681->f2=_tmpC00;});_tmp681;}));
goto _LL0;}case 511U: _LL3F9: _LL3FA:
# 3014 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3015 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3016 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3020 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));({struct _fat_ptr*_tmpC03=({struct _fat_ptr*_tmp682=_cycalloc(sizeof(*_tmp682));({struct _fat_ptr _tmpC02=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp682=_tmpC02;});_tmp682;});_tmp683->hd=_tmpC03;}),_tmp683->tl=0;_tmp683;}));
goto _LL0;case 515U: _LL401: _LL402:
# 3021 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct _fat_ptr*_tmpC06=({struct _fat_ptr*_tmp684=_cycalloc(sizeof(*_tmp684));({struct _fat_ptr _tmpC05=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp684=_tmpC05;});_tmp684;});_tmp685->hd=_tmpC06;}),({struct Cyc_List_List*_tmpC04=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp685->tl=_tmpC04;});_tmp685;}));
goto _LL0;case 516U: _LL403: _LL404:
# 3025 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 517U: _LL405: _LL406:
# 3026 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 518U: _LL407: _LL408:
# 3027 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 519U: _LL409: _LL40A:
# 3032 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 520U: _LL40B: _LL40C:
# 3034 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC08=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpC07=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC08,_tmpC07,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3036 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC09=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC09,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3038 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC0A=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC0B,_tmpC0A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3040 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC0D=({struct _fat_ptr*_tmp686=_cycalloc(sizeof(*_tmp686));({struct _fat_ptr _tmpC0C=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp686=_tmpC0C;});_tmp686;});Cyc_Absyn_aggrmember_exp(_tmpC0E,_tmpC0D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3042 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC11=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC10=({struct _fat_ptr*_tmp687=_cycalloc(sizeof(*_tmp687));({struct _fat_ptr _tmpC0F=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp687=_tmpC0F;});_tmp687;});Cyc_Absyn_aggrarrow_exp(_tmpC11,_tmpC10,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3044 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC12=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC12,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3046 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC13=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC13,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3048 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC15=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->tag=25U,({struct _tuple8*_tmpC14=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp688->f1=_tmpC14;}),_tmp688->f2=0;_tmp688;});Cyc_Absyn_new_exp(_tmpC15,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3050 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC18=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->tag=25U,({struct _tuple8*_tmpC17=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp689->f1=_tmpC17;}),({struct Cyc_List_List*_tmpC16=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp689->f2=_tmpC16;});_tmp689;});Cyc_Absyn_new_exp(_tmpC18,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3052 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC1B=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->tag=25U,({struct _tuple8*_tmpC1A=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68A->f1=_tmpC1A;}),({struct Cyc_List_List*_tmpC19=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp68A->f2=_tmpC19;});_tmp68A;});Cyc_Absyn_new_exp(_tmpC1B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3057 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp68D=_cycalloc(sizeof(*_tmp68D));({void*_tmpC1E=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->tag=0U,({struct _fat_ptr*_tmpC1D=({struct _fat_ptr*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct _fat_ptr _tmpC1C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp68B=_tmpC1C;});_tmp68B;});_tmp68C->f1=_tmpC1D;});_tmp68C;});_tmp68D->hd=_tmpC1E;}),_tmp68D->tl=0;_tmp68D;}));
goto _LL0;case 531U: _LL421: _LL422:
# 3060
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp68F=_cycalloc(sizeof(*_tmp68F));({void*_tmpC21=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->tag=1U,({unsigned _tmpC20=({unsigned _tmpC1F=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC1F,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp68E->f1=_tmpC20;});_tmp68E;});_tmp68F->hd=_tmpC21;}),_tmp68F->tl=0;_tmp68F;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3062 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp692=_cycalloc(sizeof(*_tmp692));({void*_tmpC25=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->tag=0U,({struct _fat_ptr*_tmpC24=({struct _fat_ptr*_tmp690=_cycalloc(sizeof(*_tmp690));({struct _fat_ptr _tmpC23=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp690=_tmpC23;});_tmp690;});_tmp691->f1=_tmpC24;});_tmp691;});_tmp692->hd=_tmpC25;}),({struct Cyc_List_List*_tmpC22=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp692->tl=_tmpC22;});_tmp692;}));
goto _LL0;case 533U: _LL425: _LL426:
# 3065
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));({void*_tmpC29=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->tag=1U,({unsigned _tmpC28=({unsigned _tmpC27=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC27,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp693->f1=_tmpC28;});_tmp693;});_tmp694->hd=_tmpC29;}),({struct Cyc_List_List*_tmpC26=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp694->tl=_tmpC26;});_tmp694;}));
goto _LL0;case 534U: _LL427: _LL428:
# 3071 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC2A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC2A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535U: _LL429: _LL42A:
# 3073 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC2B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC2B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536U: _LL42B: _LL42C:
# 3075 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 537U: _LL42D: _LL42E:
# 3077 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC2C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC2C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 538U: _LL42F: _LL430:
# 3079 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC2D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC2D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 539U: _LL431: _LL432:
# 3081 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 540U: _LL433: _LL434:
# 3086 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC2E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC2E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 541U: _LL435: _LL436:
# 3088 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC30=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC2F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC30,_tmpC2F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3091
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC31=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC31,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3094
 yyval=Cyc_Exp_tok(({void*_tmpC35=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->tag=29U,({struct _tuple0*_tmpC34=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp695->f1=_tmpC34;}),({struct Cyc_List_List*_tmpC33=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp695->f2=_tmpC33;}),({struct Cyc_List_List*_tmpC32=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp695->f3=_tmpC32;}),_tmp695->f4=0;_tmp695;});Cyc_Absyn_new_exp(_tmpC35,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3097
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC36=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC36,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3101 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 546U: _LL43F: _LL440:
# 3107 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp696=_cycalloc(sizeof(*_tmp696));({struct Cyc_Absyn_Exp*_tmpC37=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp696->hd=_tmpC37;}),_tmp696->tl=0;_tmp696;}));
goto _LL0;case 547U: _LL441: _LL442:
# 3109 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp697=_cycalloc(sizeof(*_tmp697));({struct Cyc_Absyn_Exp*_tmpC39=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp697->hd=_tmpC39;}),({struct Cyc_List_List*_tmpC38=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp697->tl=_tmpC38;});_tmp697;}));
goto _LL0;case 548U: _LL443: _LL444:
# 3115 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC3A=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC3A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 549U: _LL445: _LL446:
# 3116 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC3B=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC3B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 550U: _LL447: _LL448:
# 3117 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC3C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC3C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 551U: _LL449: _LL44A: {
# 3119 "parse.y"
struct _fat_ptr _tmp698=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp698;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3127
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3129
goto _LL0;}case 552U: _LL44B: _LL44C:
# 3130 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 553U: _LL44D: _LL44E:
# 3134 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp69A=_cycalloc(sizeof(*_tmp69A));({union Cyc_Absyn_Nmspace _tmpC3F=Cyc_Absyn_Rel_n(0);_tmp69A->f1=_tmpC3F;}),({struct _fat_ptr*_tmpC3E=({struct _fat_ptr*_tmp699=_cycalloc(sizeof(*_tmp699));({struct _fat_ptr _tmpC3D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp699=_tmpC3D;});_tmp699;});_tmp69A->f2=_tmpC3E;});_tmp69A;}));
goto _LL0;case 554U: _LL44F: _LL450:
# 3135 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 555U: _LL451: _LL452:
# 3138
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp69C=_cycalloc(sizeof(*_tmp69C));({union Cyc_Absyn_Nmspace _tmpC42=Cyc_Absyn_Rel_n(0);_tmp69C->f1=_tmpC42;}),({struct _fat_ptr*_tmpC41=({struct _fat_ptr*_tmp69B=_cycalloc(sizeof(*_tmp69B));({struct _fat_ptr _tmpC40=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp69B=_tmpC40;});_tmp69B;});_tmp69C->f2=_tmpC41;});_tmp69C;}));
goto _LL0;case 556U: _LL453: _LL454:
# 3139 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 557U: _LL455: _LL456:
# 3144 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3145 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3148
 yyval=(yyyvsp[0]).v;
goto _LL0;case 560U: _LL45B: _LL45C:
# 3149 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 561U: _LL45D: _LL45E:
# 3154 "parse.y"
 goto _LL0;case 562U: _LL45F: _LL460:
# 3154 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL461: _LL462:
# 3158
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
({int _tmpC43=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC43;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC44=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 153));_tmpC44 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7695)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7696U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
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
if(yyn > - 32768 && yyn < 7695){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 315U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7696U,sizeof(short),x + yyn))== x)
({unsigned long _tmpC45=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,315U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC45;}),count ++;}
msg=({unsigned _tmp69E=(unsigned)(sze + 15)+ 1U;char*_tmp69D=({struct _RegionHandle*_tmpC46=yyregion;_region_malloc(_tmpC46,_check_times(_tmp69E,sizeof(char)));});({{unsigned _tmp762=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp762;++ i){_tmp69D[i]='\000';}_tmp69D[_tmp762]=0;}0;});_tag_fat(_tmp69D,sizeof(char),_tmp69E);});
({struct _fat_ptr _tmpC47=msg;Cyc_strcpy(_tmpC47,({const char*_tmp69F="parse error";_tag_fat(_tmp69F,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 315U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7696U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC48=msg;Cyc_strcat(_tmpC48,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp6A0=", expecting `";_tag_fat(_tmp6A0,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp6A1=" or `";_tag_fat(_tmp6A1,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,315U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC49=msg;Cyc_strcat(_tmpC49,({const char*_tmp6A2="'";_tag_fat(_tmp6A2,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC4B=({const char*_tmp6A3="parse error";_tag_fat(_tmp6A3,sizeof(char),12U);});int _tmpC4A=yystate;Cyc_yyerror(_tmpC4B,_tmpC4A,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp6A4=1;_npop_handler(0U);return _tmp6A4;}
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
 if(yyssp_offset == 0){int _tmp6A5=1;_npop_handler(0U);return _tmp6A5;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1138U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7695)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7696U,sizeof(short),yyn))!= 1)goto yyerrdefault;
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
if(yyn == 1137){
int _tmp6A6=0;_npop_handler(0U);return _tmp6A6;}
# 546
({struct Cyc_Yystacktype _tmpC4C=({struct Cyc_Yystacktype _tmp763;_tmp763.v=yylval,_tmp763.l=yylloc;_tmp763;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC4C;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3157 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6AA=v;struct Cyc_Absyn_Stmt*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AC;struct _tuple0*_tmp6AD;struct _fat_ptr _tmp6AE;char _tmp6AF;union Cyc_Absyn_Cnst _tmp6B0;switch((_tmp6AA.Stmt_tok).tag){case 1U: _LL1: _tmp6B0=(_tmp6AA.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6B0;
({struct Cyc_String_pa_PrintArg_struct _tmp6B3=({struct Cyc_String_pa_PrintArg_struct _tmp766;_tmp766.tag=0U,({struct _fat_ptr _tmpC4D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp766.f1=_tmpC4D;});_tmp766;});void*_tmp6B1[1U];_tmp6B1[0]=& _tmp6B3;({struct Cyc___cycFILE*_tmpC4F=Cyc_stderr;struct _fat_ptr _tmpC4E=({const char*_tmp6B2="%s";_tag_fat(_tmp6B2,sizeof(char),3U);});Cyc_fprintf(_tmpC4F,_tmpC4E,_tag_fat(_tmp6B1,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6AF=(_tmp6AA.Char_tok).val;_LL4: {char c=_tmp6AF;
({struct Cyc_Int_pa_PrintArg_struct _tmp6B6=({struct Cyc_Int_pa_PrintArg_struct _tmp767;_tmp767.tag=1U,_tmp767.f1=(unsigned long)((int)c);_tmp767;});void*_tmp6B4[1U];_tmp6B4[0]=& _tmp6B6;({struct Cyc___cycFILE*_tmpC51=Cyc_stderr;struct _fat_ptr _tmpC50=({const char*_tmp6B5="%c";_tag_fat(_tmp6B5,sizeof(char),3U);});Cyc_fprintf(_tmpC51,_tmpC50,_tag_fat(_tmp6B4,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6AE=(_tmp6AA.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6AE;
({struct Cyc_String_pa_PrintArg_struct _tmp6B9=({struct Cyc_String_pa_PrintArg_struct _tmp768;_tmp768.tag=0U,_tmp768.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp768;});void*_tmp6B7[1U];_tmp6B7[0]=& _tmp6B9;({struct Cyc___cycFILE*_tmpC53=Cyc_stderr;struct _fat_ptr _tmpC52=({const char*_tmp6B8="\"%s\"";_tag_fat(_tmp6B8,sizeof(char),5U);});Cyc_fprintf(_tmpC53,_tmpC52,_tag_fat(_tmp6B7,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6AD=(_tmp6AA.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6AD;
({struct Cyc_String_pa_PrintArg_struct _tmp6BC=({struct Cyc_String_pa_PrintArg_struct _tmp769;_tmp769.tag=0U,({struct _fat_ptr _tmpC54=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp769.f1=_tmpC54;});_tmp769;});void*_tmp6BA[1U];_tmp6BA[0]=& _tmp6BC;({struct Cyc___cycFILE*_tmpC56=Cyc_stderr;struct _fat_ptr _tmpC55=({const char*_tmp6BB="%s";_tag_fat(_tmp6BB,sizeof(char),3U);});Cyc_fprintf(_tmpC56,_tmpC55,_tag_fat(_tmp6BA,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6AC=(_tmp6AA.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6AC;
({struct Cyc_String_pa_PrintArg_struct _tmp6BF=({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0U,({struct _fat_ptr _tmpC57=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp76A.f1=_tmpC57;});_tmp76A;});void*_tmp6BD[1U];_tmp6BD[0]=& _tmp6BF;({struct Cyc___cycFILE*_tmpC59=Cyc_stderr;struct _fat_ptr _tmpC58=({const char*_tmp6BE="%s";_tag_fat(_tmp6BE,sizeof(char),3U);});Cyc_fprintf(_tmpC59,_tmpC58,_tag_fat(_tmp6BD,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6AB=(_tmp6AA.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6AB;
({struct Cyc_String_pa_PrintArg_struct _tmp6C2=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,({struct _fat_ptr _tmpC5A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp76B.f1=_tmpC5A;});_tmp76B;});void*_tmp6C0[1U];_tmp6C0[0]=& _tmp6C2;({struct Cyc___cycFILE*_tmpC5C=Cyc_stderr;struct _fat_ptr _tmpC5B=({const char*_tmp6C1="%s";_tag_fat(_tmp6C1,sizeof(char),3U);});Cyc_fprintf(_tmpC5C,_tmpC5B,_tag_fat(_tmp6C0,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6C3=0U;({struct Cyc___cycFILE*_tmpC5E=Cyc_stderr;struct _fat_ptr _tmpC5D=({const char*_tmp6C4="?";_tag_fat(_tmp6C4,sizeof(char),2U);});Cyc_fprintf(_tmpC5E,_tmpC5D,_tag_fat(_tmp6C3,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3169
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6C5="end-of-file";_tag_fat(_tmp6C5,sizeof(char),12U);});
if(token == 368)
return Cyc_Lex_token_string;
if(token == 377)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 380?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,381U,sizeof(short),token)): 315;
if((unsigned)z < 315U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 3183
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6C6=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6C6;_push_region(yyr);
({struct _RegionHandle*_tmpC5F=yyr;Cyc_yyparse(_tmpC5F,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6C7=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6C7;}
# 3186
;_pop_region();}}
