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
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 861
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 891
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 895
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 901
void*Cyc_Absyn_compress(void*);
# 915
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 917
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 920
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 922
extern void*Cyc_Absyn_sint_type;
# 924
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 927
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 931
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 933
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 958
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 962
void*Cyc_Absyn_bounds_one (void);
# 964
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 984
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 987
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 992
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1036
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
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1064
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1086
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
# 1095
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1097
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1104
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1111
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1114
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1119
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
({struct Cyc_List_List*_tmp768=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp768;});
({struct _tuple11*_tmp769=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp769;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmpA=0U;({unsigned _tmp76B=loc;struct _fat_ptr _tmp76A=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp76B,_tmp76A,_tag_fat(_tmpA,sizeof(void*),0U));});});}
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
return({struct _tuple15 _tmp6C4;_tmp6C4.f1=nullable,_tmp6C4.f2=bound,_tmp6C4.f3=zeroterm,_tmp6C4.f4=rgn;_tmp6C4;});}
# 238
struct _tuple0*Cyc_Parse_gensym_enum (void){
# 240
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp76F=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp76F;}),({
struct _fat_ptr*_tmp76E=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp76D=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6C5;_tmp6C5.tag=1U,_tmp6C5.f1=(unsigned long)enum_counter ++;_tmp6C5;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp76C=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp76C,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp76D;});_tmp13;});_tmp14->f2=_tmp76E;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 245
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 250
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp771=loc;struct _fat_ptr _tmp770=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp771,_tmp770,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp773=loc;struct _fat_ptr _tmp772=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp773,_tmp772,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 260
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6C6;_tmp6C6.Signed_spec=0,_tmp6C6.Unsigned_spec=0,_tmp6C6.Short_spec=0,_tmp6C6.Long_spec=0,_tmp6C6.Long_Long_spec=0,_tmp6C6.Valid_type_spec=0,_tmp6C6.Type_spec=Cyc_Absyn_sint_type,_tmp6C6.loc=loc;_tmp6C6;});}
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
void*_tmp774=t;Cyc_Tcutil_promote_array(_tmp774,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 314 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 319
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 323
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp776=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6C7;_tmp6C7.tag=0U,_tmp6C7.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6C7;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp775=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp775,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp776;});_tmp36;});
({void*_tmp778=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp777=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Kinds_ik;_tmp31;});_tmp32->kind=_tmp777;});_tmp32;}));*z=_tmp778;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 328
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp779=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp779;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 332
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp77B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6C8;_tmp6C8.tag=0U,_tmp6C8.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6C8;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp77A=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp77A,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp77B;});_tmp3E;});
({void*_tmp77D=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp77C=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Kinds_rk;_tmp39;});_tmp3A->kind=_tmp77C;});_tmp3A;}));*z=_tmp77D;});
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
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp77E=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp77E;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
return({void*_tmp780=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp77F=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp77F;});_tmp4D;});Cyc_Absyn_new_exp(_tmp780,e->loc);});}}}
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
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6C9;_tmp6C9.elt_type=et2,_tmp6C9.elt_tq=tq,(_tmp6C9.ptr_atts).rgn=r,(_tmp6C9.ptr_atts).nullable=n,(_tmp6C9.ptr_atts).bounds=b2,(_tmp6C9.ptr_atts).zero_term=zt,(_tmp6C9.ptr_atts).ptrloc=pl;_tmp6C9;}));
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
({void*_tmp781=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp781;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
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
if(({struct _fat_ptr _tmp782=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp782,({const char*_tmp64="`H";_tag_fat(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp783=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp783,({const char*_tmp65="`U";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp784=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp784,({const char*_tmp66="`RC";_tag_fat(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr*_tmp785=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp785;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}
# 433
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp786=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp786,({const char*_tmp69="`H";_tag_fat(_tmp69,sizeof(char),3U);}));})== 0)
({void*_tmp6A=0U;({unsigned _tmp788=loc;struct _fat_ptr _tmp787=({const char*_tmp6B="bad occurrence of heap region";_tag_fat(_tmp6B,sizeof(char),30U);});Cyc_Warn_err(_tmp788,_tmp787,_tag_fat(_tmp6A,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp789=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp789,({const char*_tmp6C="`U";_tag_fat(_tmp6C,sizeof(char),3U);}));})== 0)
({void*_tmp6D=0U;({unsigned _tmp78B=loc;struct _fat_ptr _tmp78A=({const char*_tmp6E="bad occurrence of unique region";_tag_fat(_tmp6E,sizeof(char),32U);});Cyc_Warn_err(_tmp78B,_tmp78A,_tag_fat(_tmp6D,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp78C=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78C,({const char*_tmp6F="`RC";_tag_fat(_tmp6F,sizeof(char),4U);}));})== 0)
({void*_tmp70=0U;({unsigned _tmp78E=loc;struct _fat_ptr _tmp78D=({const char*_tmp71="bad occurrence of refcounted region";_tag_fat(_tmp71,sizeof(char),36U);});Cyc_Warn_err(_tmp78E,_tmp78D,_tag_fat(_tmp70,sizeof(void*),0U));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp72=0U;({unsigned _tmp790=loc;struct _fat_ptr _tmp78F=({const char*_tmp73="bad occurrence of \"current\" region";_tag_fat(_tmp73,sizeof(char),35U);});Cyc_Warn_err(_tmp790,_tmp78F,_tag_fat(_tmp72,sizeof(void*),0U));});});}
# 448
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp74=t;struct Cyc_Absyn_Tvar*_tmp75;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp74)->tag == 2U){_LL1: _tmp75=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp74)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp75;
return pr;}}else{_LL3: _LL4:
({void*_tmp76=0U;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp77="expecting a list of type variables, not types";_tag_fat(_tmp77,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp792,_tmp791,_tag_fat(_tmp76,sizeof(void*),0U));});});}_LL0:;}
# 456
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp78=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp78;void*_tmp79=_stmttmp6;void**_tmp7A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp79)->tag == 2U){_LL1: _tmp7A=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp79)->f1)->kind;_LL2: {void**cptr=_tmp7A;
# 459
void*_tmp7B=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp7=_tmp7B;void*_tmp7C=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7C)->tag == 1U){_LL6: _LL7:
# 461
({void*_tmp793=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=2U,_tmp7D->f1=0,_tmp7D->f2=k;_tmp7D;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp793;});
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 465
 return;}_LL0:;}
# 470
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 478
if(tms == 0)return 0;{
# 480
void*_tmp7E=(void*)tms->hd;void*_stmttmp8=_tmp7E;void*_tmp7F=_stmttmp8;void*_tmp80;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7F)->tag == 3U){_LL1: _tmp80=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7F)->f1;_LL2: {void*args=_tmp80;
# 483
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 486
void*_tmp81=args;struct Cyc_List_List*_tmp82;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp81)->tag == 1U){_LL6: _LL7:
# 488
({void*_tmp83=0U;({unsigned _tmp795=loc;struct _fat_ptr _tmp794=({const char*_tmp84="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp84,sizeof(char),93U);});Cyc_Warn_warn(_tmp795,_tmp794,_tag_fat(_tmp83,sizeof(void*),0U));});});
# 490
return tms;}else{_LL8: _tmp82=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp81)->f1;_LL9: {struct Cyc_List_List*ids=_tmp82;
# 492
if(({int _tmp796=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp796 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp85=0U;({unsigned _tmp798=loc;struct _fat_ptr _tmp797=({const char*_tmp86="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp86,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp798,_tmp797,_tag_fat(_tmp85,sizeof(void*),0U));});});{
# 496
struct Cyc_List_List*rev_new_params=0;
for(0;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(0;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp87=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp87;
void*_tmp88=x->r;void*_stmttmp9=_tmp88;void*_tmp89=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp8A;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp89)->tag == 0U){_LLB: _tmp8A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp89)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp8A;
# 503
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({void*_tmp8B=0U;({unsigned _tmp79A=x->loc;struct _fat_ptr _tmp799=({const char*_tmp8C="initializer found in parameter declaration";_tag_fat(_tmp8C,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79A,_tmp799,_tag_fat(_tmp8B,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp8D=0U;({unsigned _tmp79C=x->loc;struct _fat_ptr _tmp79B=({const char*_tmp8E="namespaces forbidden in parameter declarations";_tag_fat(_tmp8E,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79C,_tmp79B,_tag_fat(_tmp8D,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp90=_cycalloc(sizeof(*_tmp90));({struct _tuple8*_tmp79D=({struct _tuple8*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=(*vd->name).f2,_tmp8F->f2=vd->tq,_tmp8F->f3=vd->type;_tmp8F;});_tmp90->hd=_tmp79D;}),_tmp90->tl=rev_new_params;_tmp90;});
# 511
goto L;}}else{_LLD: _LLE:
({void*_tmp91=0U;({unsigned _tmp79F=x->loc;struct _fat_ptr _tmp79E=({const char*_tmp92="nonvariable declaration in parameter type";_tag_fat(_tmp92,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79F,_tmp79E,_tag_fat(_tmp91,sizeof(void*),0U));});});}_LLA:;}
# 515
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp95=({struct Cyc_String_pa_PrintArg_struct _tmp6CA;_tmp6CA.tag=0U,_tmp6CA.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6CA;});void*_tmp93[1U];_tmp93[0]=& _tmp95;({unsigned _tmp7A1=loc;struct _fat_ptr _tmp7A0=({const char*_tmp94="%s is not given a type";_tag_fat(_tmp94,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A1,_tmp7A0,_tag_fat(_tmp93,sizeof(void*),1U));});});}
# 518
return({struct Cyc_List_List*_tmp98=_region_malloc(yy,sizeof(*_tmp98));
({void*_tmp7A4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp97=_region_malloc(yy,sizeof(*_tmp97));_tmp97->tag=3U,({void*_tmp7A3=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp96=_region_malloc(yy,sizeof(*_tmp96));_tmp96->tag=1U,({struct Cyc_List_List*_tmp7A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp96->f1=_tmp7A2;}),_tmp96->f2=0,_tmp96->f3=0,_tmp96->f4=0,_tmp96->f5=0,_tmp96->f6=0,_tmp96->f7=0;_tmp96;});_tmp97->f1=_tmp7A3;});_tmp97;});_tmp98->hd=_tmp7A4;}),_tmp98->tl=0;_tmp98;});}}}_LL5:;}
# 525
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp99=_region_malloc(yy,sizeof(*_tmp99));_tmp99->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7A5=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp99->tl=_tmp7A5;});_tmp99;});}_LL0:;}}
# 533
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 537
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6CB;_tmp6CB.id=d.id,_tmp6CB.varloc=d.varloc,({struct Cyc_List_List*_tmp7A6=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6CB.tms=_tmp7A6;});_tmp6CB;});{
# 540
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 546
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 552
enum Cyc_Parse_Storage_class _tmp9A=dso->sc;enum Cyc_Parse_Storage_class _stmttmpA=_tmp9A;enum Cyc_Parse_Storage_class _tmp9B=_stmttmpA;switch(_tmp9B){case Cyc_Parse_None_sc: _LL1: _LL2:
 goto _LL0;case Cyc_Parse_Extern_sc: _LL3: _LL4:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL5: _LL6:
 sc=0U;goto _LL0;default: _LL7: _LL8:
({void*_tmp9C=0U;({unsigned _tmp7A8=loc;struct _fat_ptr _tmp7A7=({const char*_tmp9D="bad storage class on function";_tag_fat(_tmp9D,sizeof(char),30U);});Cyc_Warn_err(_tmp7A8,_tmp7A7,_tag_fat(_tmp9C,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 559
void*_tmp9E=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp9E;
struct _tuple14 _tmp9F=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmp9F;struct _tuple14 _tmpA0=_stmttmpB;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA3;void*_tmpA2;struct Cyc_Absyn_Tqual _tmpA1;_LLA: _tmpA1=_tmpA0.f1;_tmpA2=_tmpA0.f2;_tmpA3=_tmpA0.f3;_tmpA4=_tmpA0.f4;_LLB: {struct Cyc_Absyn_Tqual fn_tqual=_tmpA1;void*fn_type=_tmpA2;struct Cyc_List_List*x=_tmpA3;struct Cyc_List_List*out_atts=_tmpA4;
# 564
if(x != 0)
({void*_tmpA5=0U;({unsigned _tmp7AA=loc;struct _fat_ptr _tmp7A9=({const char*_tmpA6="bad type params, ignoring";_tag_fat(_tmpA6,sizeof(char),26U);});Cyc_Warn_warn(_tmp7AA,_tmp7A9,_tag_fat(_tmpA5,sizeof(void*),0U));});});{
# 567
void*_tmpA7=fn_type;struct Cyc_Absyn_FnInfo _tmpA8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA7)->tag == 5U){_LLD: _tmpA8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA7)->f1;_LLE: {struct Cyc_Absyn_FnInfo i=_tmpA8;
# 569
{struct Cyc_List_List*_tmpA9=i.args;struct Cyc_List_List*args2=_tmpA9;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpAA=0U;({unsigned _tmp7AC=loc;struct _fat_ptr _tmp7AB=({const char*_tmpAB="missing argument variable in function prototype";_tag_fat(_tmpAB,sizeof(char),48U);});Cyc_Warn_err(_tmp7AC,_tmp7AB,_tag_fat(_tmpAA,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7AE=({struct _fat_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _fat_ptr _tmp7AD=({const char*_tmpAC="?";_tag_fat(_tmpAC,sizeof(char),2U);});*_tmpAD=_tmp7AD;});_tmpAD;});(*((struct _tuple8*)args2->hd)).f1=_tmp7AE;});}}}
# 576
({struct Cyc_List_List*_tmp7AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7AF;});
return({struct Cyc_Absyn_Fndecl*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->sc=sc,_tmpAE->is_inline=is_inline,_tmpAE->name=d.id,_tmpAE->body=body,_tmpAE->i=i,_tmpAE->cached_type=0,_tmpAE->param_vardecls=0,_tmpAE->fn_vardecl=0,_tmpAE->orig_scope=sc;_tmpAE;});}}else{_LLF: _LL10:
# 580
({void*_tmpAF=0U;({unsigned _tmp7B1=loc;struct _fat_ptr _tmp7B0=({const char*_tmpB0="declarator is not a function prototype";_tag_fat(_tmpB0,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7B1,_tmp7B0,_tag_fat(_tmpAF,sizeof(void*),0U));});});}_LLC:;}}}}}static char _tmpB1[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 584
static struct _fat_ptr Cyc_Parse_msg1={_tmpB1,_tmpB1,_tmpB1 + 76U};static char _tmpB2[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 586
static struct _fat_ptr Cyc_Parse_msg2={_tmpB2,_tmpB2,_tmpB2 + 84U};
# 593
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 596
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpB3=0U;({unsigned _tmp7B3=loc;struct _fat_ptr _tmp7B2=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7B3,_tmp7B2,_tag_fat(_tmpB3,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpB4=0U;({unsigned _tmp7B5=loc;struct _fat_ptr _tmp7B4=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp7B5,_tmp7B4,_tag_fat(_tmpB4,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpB5=0U;({unsigned _tmp7B7=loc;struct _fat_ptr _tmp7B6=({const char*_tmpB6="too many occurrences of short in specifiers";_tag_fat(_tmpB6,sizeof(char),44U);});Cyc_Warn_warn(_tmp7B7,_tmp7B6,_tag_fat(_tmpB5,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpB7=0U;({unsigned _tmp7B9=loc;struct _fat_ptr _tmp7B8=({const char*_tmpB8="too many occurrences of long in specifiers";_tag_fat(_tmpB8,sizeof(char),43U);});Cyc_Warn_warn(_tmp7B9,_tmp7B8,_tag_fat(_tmpB7,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpB9=0U;({unsigned _tmp7BB=loc;struct _fat_ptr _tmp7BA=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7BB,_tmp7BA,_tag_fat(_tmpB9,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 618
return s1;}
# 624
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 632
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpBA=0U;({unsigned _tmp7BD=loc;struct _fat_ptr _tmp7BC=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7BD,_tmp7BC,_tag_fat(_tmpBA,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpBB=0U;({unsigned _tmp7BF=loc;struct _fat_ptr _tmp7BE=Cyc_Parse_msg2;Cyc_Warn_err(_tmp7BF,_tmp7BE,_tag_fat(_tmpBB,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 645
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpBC=0U;({unsigned _tmp7C1=loc;struct _fat_ptr _tmp7C0=({const char*_tmpBD="missing type within specifier";_tag_fat(_tmpBD,sizeof(char),30U);});Cyc_Warn_warn(_tmp7C1,_tmp7C0,_tag_fat(_tmpBC,sizeof(void*),0U));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 650
{void*_tmpBE=t;enum Cyc_Absyn_Size_of _tmpC0;enum Cyc_Absyn_Sign _tmpBF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)){case 1U: _LL1: _tmpBF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)->f1;_tmpC0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpBF;enum Cyc_Absyn_Size_of sz2=_tmpC0;
# 652
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 656
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2U: _LL3: _LL4:
# 660
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
# 664
 if(seen_sign)
({void*_tmpC1=0U;({unsigned _tmp7C3=loc;struct _fat_ptr _tmp7C2=({const char*_tmpC2="sign specification on non-integral type";_tag_fat(_tmpC2,sizeof(char),40U);});Cyc_Warn_err(_tmp7C3,_tmp7C2,_tag_fat(_tmpC1,sizeof(void*),0U));});});
if(seen_size)
({void*_tmpC3=0U;({unsigned _tmp7C5=loc;struct _fat_ptr _tmp7C4=({const char*_tmpC4="size qualifier on non-integral type";_tag_fat(_tmpC4,sizeof(char),36U);});Cyc_Warn_err(_tmp7C5,_tmp7C4,_tag_fat(_tmpC3,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 670
return t;}
# 673
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 677
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpC5=d.id;struct _tuple0*q=_tmpC5;
unsigned _tmpC6=d.varloc;unsigned varloc=_tmpC6;
struct _tuple14 _tmpC7=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpC7;struct _tuple14 _tmpC8=_stmttmpC;struct Cyc_List_List*_tmpCC;struct Cyc_List_List*_tmpCB;void*_tmpCA;struct Cyc_Absyn_Tqual _tmpC9;_LL1: _tmpC9=_tmpC8.f1;_tmpCA=_tmpC8.f2;_tmpCB=_tmpC8.f3;_tmpCC=_tmpC8.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpC9;void*new_typ=_tmpCA;struct Cyc_List_List*tvs=_tmpCB;struct Cyc_List_List*atts=_tmpCC;
# 684
struct Cyc_List_List*_tmpCD=ds->tl == 0?0:({struct _RegionHandle*_tmp7C9=r;struct Cyc_Absyn_Tqual _tmp7C8=tq;void*_tmp7C7=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7C6=ds->tl;Cyc_Parse_apply_tmss(_tmp7C9,_tmp7C8,_tmp7C7,_tmp7C6,shared_atts);});struct Cyc_List_List*tl=_tmpCD;
return({struct Cyc_List_List*_tmpCF=_region_malloc(r,sizeof(*_tmpCF));({struct _tuple16*_tmp7CA=({struct _tuple16*_tmpCE=_region_malloc(r,sizeof(*_tmpCE));_tmpCE->f1=varloc,_tmpCE->f2=q,_tmpCE->f3=tq2,_tmpCE->f4=new_typ,_tmpCE->f5=tvs,_tmpCE->f6=atts;_tmpCE;});_tmpCF->hd=_tmp7CA;}),_tmpCF->tl=tl;_tmpCF;});}}}
# 688
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 691
if(tms == 0)return({struct _tuple14 _tmp6CC;_tmp6CC.f1=tq,_tmp6CC.f2=t,_tmp6CC.f3=0,_tmp6CC.f4=atts;_tmp6CC;});{
void*_tmpD0=(void*)tms->hd;void*_stmttmpD=_tmpD0;void*_tmpD1=_stmttmpD;struct Cyc_List_List*_tmpD3;unsigned _tmpD2;struct Cyc_Absyn_Tqual _tmpD5;struct Cyc_Absyn_PtrAtts _tmpD4;unsigned _tmpD7;struct Cyc_List_List*_tmpD6;void*_tmpD8;unsigned _tmpDB;void*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;unsigned _tmpDD;void*_tmpDC;switch(*((int*)_tmpD1)){case 0U: _LL1: _tmpDC=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpDD=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LL2: {void*zeroterm=_tmpDC;unsigned ztloc=_tmpDD;
# 694
return({struct Cyc_Absyn_Tqual _tmp7CD=Cyc_Absyn_empty_tqual(0U);void*_tmp7CC=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 694
struct Cyc_List_List*_tmp7CB=atts;Cyc_Parse_apply_tms(_tmp7CD,_tmp7CC,_tmp7CB,tms->tl);});}case 1U: _LL3: _tmpD9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpDA=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_tmpDB=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpD9;void*zeroterm=_tmpDA;unsigned ztloc=_tmpDB;
# 697
return({struct Cyc_Absyn_Tqual _tmp7D0=Cyc_Absyn_empty_tqual(0U);void*_tmp7CF=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 697
struct Cyc_List_List*_tmp7CE=atts;Cyc_Parse_apply_tms(_tmp7D0,_tmp7CF,_tmp7CE,tms->tl);});}case 3U: _LL5: _tmpD8=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_LL6: {void*args=_tmpD8;
# 700
void*_tmpDE=args;unsigned _tmpDF;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE4;void*_tmpE3;struct Cyc_Absyn_VarargInfo*_tmpE2;int _tmpE1;struct Cyc_List_List*_tmpE0;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->tag == 1U){_LLE: _tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f1;_tmpE1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f2;_tmpE2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f3;_tmpE3=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f4;_tmpE4=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f5;_tmpE5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f6;_tmpE6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f7;_LLF: {struct Cyc_List_List*args2=_tmpE0;int c_vararg=_tmpE1;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpE2;void*eff=_tmpE3;struct Cyc_List_List*rgn_po=_tmpE4;struct Cyc_Absyn_Exp*req=_tmpE5;struct Cyc_Absyn_Exp*ens=_tmpE6;
# 702
struct Cyc_List_List*typvars=0;
# 704
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=(void*)as->hd,_tmpE7->tl=fn_atts;_tmpE7;});else{
# 709
new_atts=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(void*)as->hd,_tmpE8->tl=new_atts;_tmpE8;});}}}
# 711
if(tms->tl != 0){
void*_tmpE9=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpE9;void*_tmpEA=_stmttmpE;struct Cyc_List_List*_tmpEB;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEA)->tag == 4U){_LL13: _tmpEB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEA)->f1;_LL14: {struct Cyc_List_List*ts=_tmpEB;
# 714
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;}
# 720
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 725
args2=0;{
# 728
struct Cyc_List_List*_tmpEC=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpEC;
# 730
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 735
{struct Cyc_List_List*_tmpED=args2;struct Cyc_List_List*a=_tmpED;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpEE=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpEE;struct _tuple8*_tmpEF=_stmttmpF;void**_tmpF2;struct Cyc_Absyn_Tqual _tmpF1;struct _fat_ptr*_tmpF0;_LL18: _tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;_tmpF2=(void**)& _tmpEF->f3;_LL19: {struct _fat_ptr*vopt=_tmpF0;struct Cyc_Absyn_Tqual tq=_tmpF1;void**t=_tmpF2;
if(tags != 0)
({void*_tmp7D1=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7D1;});
({void*_tmp7D2=Cyc_Parse_array2ptr(*t,1);*t=_tmp7D2;});}}}
# 747
return({struct Cyc_Absyn_Tqual _tmp7D5=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7D4=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 747
struct Cyc_List_List*_tmp7D3=new_atts;Cyc_Parse_apply_tms(_tmp7D5,_tmp7D4,_tmp7D3,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpDF=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpDE)->f2;_LL11: {unsigned loc=_tmpDF;
# 754
({void*_tmpF3=0U;({unsigned _tmp7D7=loc;struct _fat_ptr _tmp7D6=({const char*_tmpF4="function declaration without parameter types";_tag_fat(_tmpF4,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7D7,_tmp7D6,_tag_fat(_tmpF3,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpD7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD6;unsigned loc=_tmpD7;
# 761
if(tms->tl == 0)
return({struct _tuple14 _tmp6CD;_tmp6CD.f1=tq,_tmp6CD.f2=t,_tmp6CD.f3=ts,_tmp6CD.f4=atts;_tmp6CD;});
# 766
({void*_tmpF5=0U;({unsigned _tmp7D9=loc;struct _fat_ptr _tmp7D8=({const char*_tmpF6="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF6,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7D9,_tmp7D8,_tag_fat(_tmpF5,sizeof(void*),0U));});});}case 2U: _LL9: _tmpD4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpD5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpD4;struct Cyc_Absyn_Tqual tq2=_tmpD5;
# 769
return({struct Cyc_Absyn_Tqual _tmp7DC=tq2;void*_tmp7DB=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6CE;_tmp6CE.elt_type=t,_tmp6CE.elt_tq=tq,_tmp6CE.ptr_atts=ptratts;_tmp6CE;}));struct Cyc_List_List*_tmp7DA=atts;Cyc_Parse_apply_tms(_tmp7DC,_tmp7DB,_tmp7DA,tms->tl);});}default: _LLB: _tmpD2=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpD3=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LLC: {unsigned loc=_tmpD2;struct Cyc_List_List*atts2=_tmpD3;
# 774
return({struct Cyc_Absyn_Tqual _tmp7DF=tq;void*_tmp7DE=t;struct Cyc_List_List*_tmp7DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7DF,_tmp7DE,_tmp7DD,tms->tl);});}}_LL0:;}}
# 780
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 789
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpF7=t;struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFC;void*_tmpFB;struct Cyc_Absyn_Tqual _tmpFA;struct _tuple0*_tmpF9;unsigned _tmpF8;_LL1: _tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;_tmpFA=_tmpF7->f3;_tmpFB=_tmpF7->f4;_tmpFC=_tmpF7->f5;_tmpFD=_tmpF7->f6;_LL2: {unsigned varloc=_tmpF8;struct _tuple0*x=_tmpF9;struct Cyc_Absyn_Tqual tq=_tmpFA;void*typ=_tmpFB;struct Cyc_List_List*tvs=_tmpFC;struct Cyc_List_List*atts=_tmpFD;
# 792
Cyc_Lex_register_typedef(x);{
# 794
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpFE=typ;struct Cyc_Core_Opt*_tmpFF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFE)->tag == 1U){_LL4: _tmpFF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFE)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmpFF;
# 798
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 803
return({void*_tmp7E1=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7E0=({struct Cyc_Absyn_Typedefdecl*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->name=x,_tmp100->tvs=tvs,_tmp100->kind=kind,_tmp100->defn=type,_tmp100->atts=atts,_tmp100->tq=tq,_tmp100->extern_c=0;_tmp100;});_tmp101->f1=_tmp7E0;});_tmp101;});Cyc_Absyn_new_decl(_tmp7E1,loc);});}}}
# 810
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7E2=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=12U,_tmp102->f1=d,_tmp102->f2=s;_tmp102;});Cyc_Absyn_new_stmt(_tmp7E2,d->loc);});}
# 814
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 823
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 826
struct _RegionHandle _tmp103=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp103;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp104=ds;struct Cyc_List_List*_tmp107;struct Cyc_Parse_Type_specifier _tmp106;struct Cyc_Absyn_Tqual _tmp105;_LL1: _tmp105=_tmp104.tq;_tmp106=_tmp104.type_specs;_tmp107=_tmp104.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp105;struct Cyc_Parse_Type_specifier tss=_tmp106;struct Cyc_List_List*atts=_tmp107;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp108=0U;({unsigned _tmp7E4=loc;struct _fat_ptr _tmp7E3=({const char*_tmp109="inline qualifier on non-function definition";_tag_fat(_tmp109,sizeof(char),44U);});Cyc_Warn_warn(_tmp7E4,_tmp7E3,_tag_fat(_tmp108,sizeof(void*),0U));});});{
# 832
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmp10A=ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp10A;enum Cyc_Parse_Storage_class _tmp10B=_stmttmp10;switch(_tmp10B){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 s=Cyc_Flags_no_register?Cyc_Absyn_Public: Cyc_Absyn_Register;goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 s=1U;goto _LL3;default: _LL12: _LL13:
 goto _LL3;}_LL3:;}{
# 848
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 852
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 860
void*_tmp10C=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp10C;
if(declarators == 0){
# 864
void*_tmp10D=base_type;struct Cyc_List_List*_tmp10E;struct _tuple0*_tmp10F;struct Cyc_List_List*_tmp112;int _tmp111;struct _tuple0*_tmp110;struct Cyc_Absyn_Datatypedecl**_tmp113;struct Cyc_List_List*_tmp116;struct _tuple0*_tmp115;enum Cyc_Absyn_AggrKind _tmp114;struct Cyc_Absyn_Datatypedecl*_tmp117;struct Cyc_Absyn_Enumdecl*_tmp118;struct Cyc_Absyn_Aggrdecl*_tmp119;switch(*((int*)_tmp10D)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)){case 0U: _LL15: _tmp119=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp119;
# 866
({struct Cyc_List_List*_tmp7E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7E5;});
ad->sc=s;{
struct Cyc_List_List*_tmp11C=({struct Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_Absyn_Decl*_tmp7E7=({void*_tmp7E6=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=5U,_tmp11A->f1=ad;_tmp11A;});Cyc_Absyn_new_decl(_tmp7E6,loc);});_tmp11B->hd=_tmp7E7;}),_tmp11B->tl=0;_tmp11B;});_npop_handler(0U);return _tmp11C;}}case 1U: _LL17: _tmp118=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp118;
# 870
if(atts != 0)({void*_tmp11D=0U;({unsigned _tmp7E9=loc;struct _fat_ptr _tmp7E8=({const char*_tmp11E="attributes on enum not supported";_tag_fat(_tmp11E,sizeof(char),33U);});Cyc_Warn_err(_tmp7E9,_tmp7E8,_tag_fat(_tmp11D,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp121=({struct Cyc_List_List*_tmp120=_cycalloc(sizeof(*_tmp120));({struct Cyc_Absyn_Decl*_tmp7EB=({void*_tmp7EA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=7U,_tmp11F->f1=ed;_tmp11F;});Cyc_Absyn_new_decl(_tmp7EA,loc);});_tmp120->hd=_tmp7EB;}),_tmp120->tl=0;_tmp120;});_npop_handler(0U);return _tmp121;}}default: _LL19: _tmp117=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp117;
# 874
if(atts != 0)({void*_tmp122=0U;({unsigned _tmp7ED=loc;struct _fat_ptr _tmp7EC=({const char*_tmp123="attributes on datatypes not supported";_tag_fat(_tmp123,sizeof(char),38U);});Cyc_Warn_err(_tmp7ED,_tmp7EC,_tag_fat(_tmp122,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp126=({struct Cyc_List_List*_tmp125=_cycalloc(sizeof(*_tmp125));({struct Cyc_Absyn_Decl*_tmp7EF=({void*_tmp7EE=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=6U,_tmp124->f1=dd;_tmp124;});Cyc_Absyn_new_decl(_tmp7EE,loc);});_tmp125->hd=_tmp7EF;}),_tmp125->tl=0;_tmp125;});_npop_handler(0U);return _tmp126;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp114=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownAggr).val).f1;_tmp115=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownAggr).val).f2;_tmp116=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp114;struct _tuple0*n=_tmp115;struct Cyc_List_List*ts=_tmp116;
# 878
struct Cyc_List_List*_tmp127=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp127;
struct Cyc_Absyn_Aggrdecl*_tmp128=({struct Cyc_Absyn_Aggrdecl*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->kind=k,_tmp12E->sc=s,_tmp12E->name=n,_tmp12E->tvs=ts2,_tmp12E->impl=0,_tmp12E->attributes=0,_tmp12E->expected_mem_kind=0;_tmp12E;});struct Cyc_Absyn_Aggrdecl*ad=_tmp128;
if(atts != 0)({void*_tmp129=0U;({unsigned _tmp7F1=loc;struct _fat_ptr _tmp7F0=({const char*_tmp12A="bad attributes on type declaration";_tag_fat(_tmp12A,sizeof(char),35U);});Cyc_Warn_err(_tmp7F1,_tmp7F0,_tag_fat(_tmp129,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12D=({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct Cyc_Absyn_Decl*_tmp7F3=({void*_tmp7F2=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=5U,_tmp12B->f1=ad;_tmp12B;});Cyc_Absyn_new_decl(_tmp7F2,loc);});_tmp12C->hd=_tmp7F3;}),_tmp12C->tl=0;_tmp12C;});_npop_handler(0U);return _tmp12D;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp113=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp113;
# 883
if(atts != 0)({void*_tmp12F=0U;({unsigned _tmp7F5=loc;struct _fat_ptr _tmp7F4=({const char*_tmp130="bad attributes on datatype";_tag_fat(_tmp130,sizeof(char),27U);});Cyc_Warn_err(_tmp7F5,_tmp7F4,_tag_fat(_tmp12F,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));({struct Cyc_Absyn_Decl*_tmp7F7=({void*_tmp7F6=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=6U,_tmp131->f1=*tudp;_tmp131;});Cyc_Absyn_new_decl(_tmp7F6,loc);});_tmp132->hd=_tmp7F7;}),_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;}}}else{_LL1F: _tmp110=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownDatatype).val).name;_tmp111=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp112=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;_LL20: {struct _tuple0*n=_tmp110;int isx=_tmp111;struct Cyc_List_List*ts=_tmp112;
# 886
struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp134;
struct Cyc_Absyn_Decl*_tmp135=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp135;
if(atts != 0)({void*_tmp136=0U;({unsigned _tmp7F9=loc;struct _fat_ptr _tmp7F8=({const char*_tmp137="bad attributes on datatype";_tag_fat(_tmp137,sizeof(char),27U);});Cyc_Warn_err(_tmp7F9,_tmp7F8,_tag_fat(_tmp136,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp139=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->hd=tud,_tmp138->tl=0;_tmp138;});_npop_handler(0U);return _tmp139;}}}case 15U: _LL21: _tmp10F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_LL22: {struct _tuple0*n=_tmp10F;
# 891
struct Cyc_Absyn_Enumdecl*_tmp13A=({struct Cyc_Absyn_Enumdecl*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->sc=s,_tmp141->name=n,_tmp141->fields=0;_tmp141;});struct Cyc_Absyn_Enumdecl*ed=_tmp13A;
if(atts != 0)({void*_tmp13B=0U;({unsigned _tmp7FB=loc;struct _fat_ptr _tmp7FA=({const char*_tmp13C="bad attributes on enum";_tag_fat(_tmp13C,sizeof(char),23U);});Cyc_Warn_err(_tmp7FB,_tmp7FA,_tag_fat(_tmp13B,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp140=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct Cyc_Absyn_Decl*_tmp7FD=({struct Cyc_Absyn_Decl*_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp7FC=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->tag=7U,_tmp13D->f1=ed;_tmp13D;});_tmp13E->r=_tmp7FC;}),_tmp13E->loc=loc;_tmp13E;});_tmp13F->hd=_tmp7FD;}),_tmp13F->tl=0;_tmp13F;});_npop_handler(0U);return _tmp140;}}case 16U: _LL23: _tmp10E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp10E;
# 897
struct Cyc_Absyn_Enumdecl*_tmp142=({struct Cyc_Absyn_Enumdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->sc=s,({struct _tuple0*_tmp7FF=Cyc_Parse_gensym_enum();_tmp14A->name=_tmp7FF;}),({struct Cyc_Core_Opt*_tmp7FE=({struct Cyc_Core_Opt*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->v=fs;_tmp149;});_tmp14A->fields=_tmp7FE;});_tmp14A;});struct Cyc_Absyn_Enumdecl*ed=_tmp142;
if(atts != 0)({void*_tmp143=0U;({unsigned _tmp801=loc;struct _fat_ptr _tmp800=({const char*_tmp144="bad attributes on enum";_tag_fat(_tmp144,sizeof(char),23U);});Cyc_Warn_err(_tmp801,_tmp800,_tag_fat(_tmp143,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp148=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));({struct Cyc_Absyn_Decl*_tmp803=({struct Cyc_Absyn_Decl*_tmp146=_cycalloc(sizeof(*_tmp146));({void*_tmp802=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->tag=7U,_tmp145->f1=ed;_tmp145;});_tmp146->r=_tmp802;}),_tmp146->loc=loc;_tmp146;});_tmp147->hd=_tmp803;}),_tmp147->tl=0;_tmp147;});_npop_handler(0U);return _tmp148;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp14B=0U;({unsigned _tmp805=loc;struct _fat_ptr _tmp804=({const char*_tmp14C="missing declarator";_tag_fat(_tmp14C,sizeof(char),19U);});Cyc_Warn_err(_tmp805,_tmp804,_tag_fat(_tmp14B,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp14D=0;_npop_handler(0U);return _tmp14D;}}_LL14:;}{
# 904
struct Cyc_List_List*_tmp14E=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp14E;
if(istypedef){
# 909
if(!exps_empty)
({void*_tmp14F=0U;({unsigned _tmp807=loc;struct _fat_ptr _tmp806=({const char*_tmp150="initializer in typedef declaration";_tag_fat(_tmp150,sizeof(char),35U);});Cyc_Warn_err(_tmp807,_tmp806,_tag_fat(_tmp14F,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp151=decls;_npop_handler(0U);return _tmp151;}}{
# 915
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp152=fields;struct Cyc_List_List*ds=_tmp152;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp153=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp153;struct _tuple16*_tmp154=_stmttmp11;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp159;void*_tmp158;struct Cyc_Absyn_Tqual _tmp157;struct _tuple0*_tmp156;unsigned _tmp155;_LL28: _tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_tmp158=_tmp154->f4;_tmp159=_tmp154->f5;_tmp15A=_tmp154->f6;_LL29: {unsigned varloc=_tmp155;struct _tuple0*x=_tmp156;struct Cyc_Absyn_Tqual tq2=_tmp157;void*t2=_tmp158;struct Cyc_List_List*tvs2=_tmp159;struct Cyc_List_List*atts2=_tmp15A;
if(tvs2 != 0)
({void*_tmp15B=0U;({unsigned _tmp809=loc;struct _fat_ptr _tmp808=({const char*_tmp15C="bad type params, ignoring";_tag_fat(_tmp15C,sizeof(char),26U);});Cyc_Warn_warn(_tmp809,_tmp808,_tag_fat(_tmp15B,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp15D=0U;({unsigned _tmp80B=loc;struct _fat_ptr _tmp80A=({const char*_tmp15E="unexpected NULL in parse!";_tag_fat(_tmp15E,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp80B,_tmp80A,_tag_fat(_tmp15D,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp15F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp15F;
struct Cyc_Absyn_Vardecl*_tmp160=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp160;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp161=({struct Cyc_Absyn_Decl*_tmp164=_cycalloc(sizeof(*_tmp164));({void*_tmp80C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->tag=0U,_tmp163->f1=vd;_tmp163;});_tmp164->r=_tmp80C;}),_tmp164->loc=loc;_tmp164;});struct Cyc_Absyn_Decl*d=_tmp161;
decls=({struct Cyc_List_List*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->hd=d,_tmp162->tl=decls;_tmp162;});}}}}}{
# 930
struct Cyc_List_List*_tmp165=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp165;}}}}}}}}}
# 827
;_pop_region();}
# 934
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp166=e->r;void*_stmttmp12=_tmp166;void*_tmp167=_stmttmp12;int _tmp168;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp167)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp167)->f1).Int_c).tag == 5){_LL1: _tmp168=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp167)->f1).Int_c).val).f2;_LL2: {int i=_tmp168;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 938
({void*_tmp169=0U;({unsigned _tmp80E=loc;struct _fat_ptr _tmp80D=({const char*_tmp16A="expecting integer constant";_tag_fat(_tmp16A,sizeof(char),27U);});Cyc_Warn_err(_tmp80E,_tmp80D,_tag_fat(_tmp169,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 944
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16B=e->r;void*_stmttmp13=_tmp16B;void*_tmp16C=_stmttmp13;struct _fat_ptr _tmp16D;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16C)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16C)->f1).String_c).tag == 8){_LL1: _tmp16D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16C)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp16D;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 948
({void*_tmp16E=0U;({unsigned _tmp810=loc;struct _fat_ptr _tmp80F=({const char*_tmp16F="expecting string constant";_tag_fat(_tmp16F,sizeof(char),26U);});Cyc_Warn_err(_tmp810,_tmp80F,_tag_fat(_tmp16E,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 954
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp170=x;long long _tmp171;char _tmp172;int _tmp173;switch((_tmp170.LongLong_c).tag){case 5U: _LL1: _tmp173=((_tmp170.Int_c).val).f2;_LL2: {int i=_tmp173;
return(unsigned)i;}case 2U: _LL3: _tmp172=((_tmp170.Char_c).val).f2;_LL4: {char c=_tmp172;
return(unsigned)c;}case 6U: _LL5: _tmp171=((_tmp170.LongLong_c).val).f2;_LL6: {long long x=_tmp171;
# 959
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp174=0U;({unsigned _tmp812=loc;struct _fat_ptr _tmp811=({const char*_tmp175="integer constant too large";_tag_fat(_tmp175,sizeof(char),27U);});Cyc_Warn_err(_tmp812,_tmp811,_tag_fat(_tmp174,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 964
({struct Cyc_String_pa_PrintArg_struct _tmp178=({struct Cyc_String_pa_PrintArg_struct _tmp6CF;_tmp6CF.tag=0U,({struct _fat_ptr _tmp813=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6CF.f1=_tmp813;});_tmp6CF;});void*_tmp176[1U];_tmp176[0]=& _tmp178;({unsigned _tmp815=loc;struct _fat_ptr _tmp814=({const char*_tmp177="expected integer constant but found %s";_tag_fat(_tmp177,sizeof(char),39U);});Cyc_Warn_err(_tmp815,_tmp814,_tag_fat(_tmp176,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 970
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp179=p->r;void*_stmttmp14=_tmp179;void*_tmp17A=_stmttmp14;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_List_List*_tmp17D;struct _tuple0*_tmp17C;int _tmp17F;struct _fat_ptr _tmp17E;char _tmp180;int _tmp182;enum Cyc_Absyn_Sign _tmp181;struct Cyc_Absyn_Pat*_tmp183;struct Cyc_Absyn_Vardecl*_tmp184;struct _tuple0*_tmp185;switch(*((int*)_tmp17A)){case 15U: _LL1: _tmp185=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_LL2: {struct _tuple0*x=_tmp185;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17A)->f2)->r)->tag == 0U){_LL3: _tmp184=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp184;
# 974
return({struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp816,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp183=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp183;
return({struct Cyc_Absyn_Exp*_tmp817=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp817,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp181=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_tmp182=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17A)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp181;int i=_tmp182;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp180=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_LLC: {char c=_tmp180;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp17E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_tmp17F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17A)->f2;_LLE: {struct _fat_ptr s=_tmp17E;int i=_tmp17F;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17A)->f3 == 0){_LLF: _tmp17C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_tmp17D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17A)->f2;_LL10: {struct _tuple0*x=_tmp17C;struct Cyc_List_List*ps=_tmp17D;
# 981
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp17B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17A)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp17B;
return e;}default: _LL13: _LL14:
# 986
({void*_tmp186=0U;({unsigned _tmp819=p->loc;struct _fat_ptr _tmp818=({const char*_tmp187="cannot mix patterns and expressions in case";_tag_fat(_tmp187,sizeof(char),44U);});Cyc_Warn_err(_tmp819,_tmp818,_tag_fat(_tmp186,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1071
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->tag=Cyc_Core_Failure,_tmp188->f1=s;_tmp188;}));}static char _tmp18B[7U]="cnst_t";
# 1043 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18B,_tmp18B,_tmp18B + 7U};
union Cyc_YYSTYPE*_tmp189=yy1;union Cyc_Absyn_Cnst _tmp18A;if((((union Cyc_YYSTYPE*)_tmp189)->Int_tok).tag == 1){_LL1: _tmp18A=(_tmp189->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp18A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1050
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6D0;(_tmp6D0.Int_tok).tag=1U,(_tmp6D0.Int_tok).val=yy1;_tmp6D0;});}static char _tmp18E[5U]="char";
# 1044 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18E,_tmp18E,_tmp18E + 5U};
union Cyc_YYSTYPE*_tmp18C=yy1;char _tmp18D;if((((union Cyc_YYSTYPE*)_tmp18C)->Char_tok).tag == 2){_LL1: _tmp18D=(_tmp18C->Char_tok).val;_LL2: {char yy=_tmp18D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1051
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6D1;(_tmp6D1.Char_tok).tag=2U,(_tmp6D1.Char_tok).val=yy1;_tmp6D1;});}static char _tmp191[13U]="string_t<`H>";
# 1045 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp191,_tmp191,_tmp191 + 13U};
union Cyc_YYSTYPE*_tmp18F=yy1;struct _fat_ptr _tmp190;if((((union Cyc_YYSTYPE*)_tmp18F)->String_tok).tag == 3){_LL1: _tmp190=(_tmp18F->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp190;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1052
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6D2;(_tmp6D2.String_tok).tag=3U,(_tmp6D2.String_tok).val=yy1;_tmp6D2;});}static char _tmp194[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1048 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp194,_tmp194,_tmp194 + 35U};
union Cyc_YYSTYPE*_tmp192=yy1;struct _tuple22*_tmp193;if((((union Cyc_YYSTYPE*)_tmp192)->YY1).tag == 9){_LL1: _tmp193=(_tmp192->YY1).val;_LL2: {struct _tuple22*yy=_tmp193;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1055
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6D3;(_tmp6D3.YY1).tag=9U,(_tmp6D3.YY1).val=yy1;_tmp6D3;});}static char _tmp197[11U]="ptrbound_t";
# 1049 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp197,_tmp197,_tmp197 + 11U};
union Cyc_YYSTYPE*_tmp195=yy1;void*_tmp196;if((((union Cyc_YYSTYPE*)_tmp195)->YY2).tag == 10){_LL1: _tmp196=(_tmp195->YY2).val;_LL2: {void*yy=_tmp196;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1056
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6D4;(_tmp6D4.YY2).tag=10U,(_tmp6D4.YY2).val=yy1;_tmp6D4;});}static char _tmp19A[28U]="list_t<offsetof_field_t,`H>";
# 1050 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19A,_tmp19A,_tmp19A + 28U};
union Cyc_YYSTYPE*_tmp198=yy1;struct Cyc_List_List*_tmp199;if((((union Cyc_YYSTYPE*)_tmp198)->YY3).tag == 11){_LL1: _tmp199=(_tmp198->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp199;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1057
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D5;(_tmp6D5.YY3).tag=11U,(_tmp6D5.YY3).val=yy1;_tmp6D5;});}static char _tmp19D[6U]="exp_t";
# 1051 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19D,_tmp19D,_tmp19D + 6U};
union Cyc_YYSTYPE*_tmp19B=yy1;struct Cyc_Absyn_Exp*_tmp19C;if((((union Cyc_YYSTYPE*)_tmp19B)->Exp_tok).tag == 7){_LL1: _tmp19C=(_tmp19B->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp19C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1058
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6D6;(_tmp6D6.Exp_tok).tag=7U,(_tmp6D6.Exp_tok).val=yy1;_tmp6D6;});}static char _tmp1A0[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A0,_tmp1A0,_tmp1A0 + 17U};
union Cyc_YYSTYPE*_tmp19E=yy1;struct Cyc_List_List*_tmp19F;if((((union Cyc_YYSTYPE*)_tmp19E)->YY4).tag == 12){_LL1: _tmp19F=(_tmp19E->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp19F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1066
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.YY4).tag=12U,(_tmp6D7.YY4).val=yy1;_tmp6D7;});}static char _tmp1A3[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1060 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A3,_tmp1A3,_tmp1A3 + 47U};
union Cyc_YYSTYPE*_tmp1A1=yy1;struct Cyc_List_List*_tmp1A2;if((((union Cyc_YYSTYPE*)_tmp1A1)->YY5).tag == 13){_LL1: _tmp1A2=(_tmp1A1->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1A2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1067
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.YY5).tag=13U,(_tmp6D8.YY5).val=yy1;_tmp6D8;});}static char _tmp1A6[9U]="primop_t";
# 1061 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A6,_tmp1A6,_tmp1A6 + 9U};
union Cyc_YYSTYPE*_tmp1A4=yy1;enum Cyc_Absyn_Primop _tmp1A5;if((((union Cyc_YYSTYPE*)_tmp1A4)->YY6).tag == 14){_LL1: _tmp1A5=(_tmp1A4->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1A5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1068
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.YY6).tag=14U,(_tmp6D9.YY6).val=yy1;_tmp6D9;});}static char _tmp1A9[19U]="opt_t<primop_t,`H>";
# 1062 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A9,_tmp1A9,_tmp1A9 + 19U};
union Cyc_YYSTYPE*_tmp1A7=yy1;struct Cyc_Core_Opt*_tmp1A8;if((((union Cyc_YYSTYPE*)_tmp1A7)->YY7).tag == 15){_LL1: _tmp1A8=(_tmp1A7->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1A8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1069
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.YY7).tag=15U,(_tmp6DA.YY7).val=yy1;_tmp6DA;});}static char _tmp1AC[7U]="qvar_t";
# 1063 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AC,_tmp1AC,_tmp1AC + 7U};
union Cyc_YYSTYPE*_tmp1AA=yy1;struct _tuple0*_tmp1AB;if((((union Cyc_YYSTYPE*)_tmp1AA)->QualId_tok).tag == 5){_LL1: _tmp1AB=(_tmp1AA->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1AB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1070
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.QualId_tok).tag=5U,(_tmp6DB.QualId_tok).val=yy1;_tmp6DB;});}static char _tmp1AF[7U]="stmt_t";
# 1066 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AF,_tmp1AF,_tmp1AF + 7U};
union Cyc_YYSTYPE*_tmp1AD=yy1;struct Cyc_Absyn_Stmt*_tmp1AE;if((((union Cyc_YYSTYPE*)_tmp1AD)->Stmt_tok).tag == 8){_LL1: _tmp1AE=(_tmp1AD->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1AE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1073
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.Stmt_tok).tag=8U,(_tmp6DC.Stmt_tok).val=yy1;_tmp6DC;});}static char _tmp1B2[27U]="list_t<switch_clause_t,`H>";
# 1070 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B2,_tmp1B2,_tmp1B2 + 27U};
union Cyc_YYSTYPE*_tmp1B0=yy1;struct Cyc_List_List*_tmp1B1;if((((union Cyc_YYSTYPE*)_tmp1B0)->YY8).tag == 16){_LL1: _tmp1B1=(_tmp1B0->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1B1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1077
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY8).tag=16U,(_tmp6DD.YY8).val=yy1;_tmp6DD;});}static char _tmp1B5[6U]="pat_t";
# 1071 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B5,_tmp1B5,_tmp1B5 + 6U};
union Cyc_YYSTYPE*_tmp1B3=yy1;struct Cyc_Absyn_Pat*_tmp1B4;if((((union Cyc_YYSTYPE*)_tmp1B3)->YY9).tag == 17){_LL1: _tmp1B4=(_tmp1B3->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1B4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1078
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY9).tag=17U,(_tmp6DE.YY9).val=yy1;_tmp6DE;});}static char _tmp1B8[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1076 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B8,_tmp1B8,_tmp1B8 + 28U};
union Cyc_YYSTYPE*_tmp1B6=yy1;struct _tuple23*_tmp1B7;if((((union Cyc_YYSTYPE*)_tmp1B6)->YY10).tag == 18){_LL1: _tmp1B7=(_tmp1B6->YY10).val;_LL2: {struct _tuple23*yy=_tmp1B7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1083
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY10).tag=18U,(_tmp6DF.YY10).val=yy1;_tmp6DF;});}static char _tmp1BB[17U]="list_t<pat_t,`H>";
# 1077 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BB,_tmp1BB,_tmp1BB + 17U};
union Cyc_YYSTYPE*_tmp1B9=yy1;struct Cyc_List_List*_tmp1BA;if((((union Cyc_YYSTYPE*)_tmp1B9)->YY11).tag == 19){_LL1: _tmp1BA=(_tmp1B9->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1BA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1084
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY11).tag=19U,(_tmp6E0.YY11).val=yy1;_tmp6E0;});}static char _tmp1BE[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1078 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BE,_tmp1BE,_tmp1BE + 36U};
union Cyc_YYSTYPE*_tmp1BC=yy1;struct _tuple24*_tmp1BD;if((((union Cyc_YYSTYPE*)_tmp1BC)->YY12).tag == 20){_LL1: _tmp1BD=(_tmp1BC->YY12).val;_LL2: {struct _tuple24*yy=_tmp1BD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1085
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY12).tag=20U,(_tmp6E1.YY12).val=yy1;_tmp6E1;});}static char _tmp1C1[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1079 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C1,_tmp1C1,_tmp1C1 + 47U};
union Cyc_YYSTYPE*_tmp1BF=yy1;struct Cyc_List_List*_tmp1C0;if((((union Cyc_YYSTYPE*)_tmp1BF)->YY13).tag == 21){_LL1: _tmp1C0=(_tmp1BF->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1C0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1086
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY13).tag=21U,(_tmp6E2.YY13).val=yy1;_tmp6E2;});}static char _tmp1C4[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1080 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C4,_tmp1C4,_tmp1C4 + 58U};
union Cyc_YYSTYPE*_tmp1C2=yy1;struct _tuple23*_tmp1C3;if((((union Cyc_YYSTYPE*)_tmp1C2)->YY14).tag == 22){_LL1: _tmp1C3=(_tmp1C2->YY14).val;_LL2: {struct _tuple23*yy=_tmp1C3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1087
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY14).tag=22U,(_tmp6E3.YY14).val=yy1;_tmp6E3;});}static char _tmp1C7[9U]="fndecl_t";
# 1081 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C7,_tmp1C7,_tmp1C7 + 9U};
union Cyc_YYSTYPE*_tmp1C5=yy1;struct Cyc_Absyn_Fndecl*_tmp1C6;if((((union Cyc_YYSTYPE*)_tmp1C5)->YY15).tag == 23){_LL1: _tmp1C6=(_tmp1C5->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1C6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1088
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY15).tag=23U,(_tmp6E4.YY15).val=yy1;_tmp6E4;});}static char _tmp1CA[18U]="list_t<decl_t,`H>";
# 1082 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CA,_tmp1CA,_tmp1CA + 18U};
union Cyc_YYSTYPE*_tmp1C8=yy1;struct Cyc_List_List*_tmp1C9;if((((union Cyc_YYSTYPE*)_tmp1C8)->YY16).tag == 24){_LL1: _tmp1C9=(_tmp1C8->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1C9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1089
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY16).tag=24U,(_tmp6E5.YY16).val=yy1;_tmp6E5;});}static char _tmp1CD[12U]="decl_spec_t";
# 1085 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CD,_tmp1CD,_tmp1CD + 12U};
union Cyc_YYSTYPE*_tmp1CB=yy1;struct Cyc_Parse_Declaration_spec _tmp1CC;if((((union Cyc_YYSTYPE*)_tmp1CB)->YY17).tag == 25){_LL1: _tmp1CC=(_tmp1CB->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1CC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1092
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY17).tag=25U,(_tmp6E6.YY17).val=yy1;_tmp6E6;});}static char _tmp1D0[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1086 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D0,_tmp1D0,_tmp1D0 + 31U};
union Cyc_YYSTYPE*_tmp1CE=yy1;struct _tuple12 _tmp1CF;if((((union Cyc_YYSTYPE*)_tmp1CE)->YY18).tag == 26){_LL1: _tmp1CF=(_tmp1CE->YY18).val;_LL2: {struct _tuple12 yy=_tmp1CF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1093
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY18).tag=26U,(_tmp6E7.YY18).val=yy1;_tmp6E7;});}static char _tmp1D3[23U]="declarator_list_t<`yy>";
# 1087 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D3,_tmp1D3,_tmp1D3 + 23U};
union Cyc_YYSTYPE*_tmp1D1=yy1;struct _tuple13*_tmp1D2;if((((union Cyc_YYSTYPE*)_tmp1D1)->YY19).tag == 27){_LL1: _tmp1D2=(_tmp1D1->YY19).val;_LL2: {struct _tuple13*yy=_tmp1D2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1094
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY19).tag=27U,(_tmp6E8.YY19).val=yy1;_tmp6E8;});}static char _tmp1D6[16U]="storage_class_t";
# 1088 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D6,_tmp1D6,_tmp1D6 + 16U};
union Cyc_YYSTYPE*_tmp1D4=yy1;enum Cyc_Parse_Storage_class _tmp1D5;if((((union Cyc_YYSTYPE*)_tmp1D4)->YY20).tag == 28){_LL1: _tmp1D5=(_tmp1D4->YY20).val;_LL2: {enum Cyc_Parse_Storage_class yy=_tmp1D5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1095
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY20).tag=28U,(_tmp6E9.YY20).val=yy1;_tmp6E9;});}static char _tmp1D9[17U]="type_specifier_t";
# 1089 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D9,_tmp1D9,_tmp1D9 + 17U};
union Cyc_YYSTYPE*_tmp1D7=yy1;struct Cyc_Parse_Type_specifier _tmp1D8;if((((union Cyc_YYSTYPE*)_tmp1D7)->YY21).tag == 29){_LL1: _tmp1D8=(_tmp1D7->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1D8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1096
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY21).tag=29U,(_tmp6EA.YY21).val=yy1;_tmp6EA;});}static char _tmp1DC[12U]="aggr_kind_t";
# 1091 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DC,_tmp1DC,_tmp1DC + 12U};
union Cyc_YYSTYPE*_tmp1DA=yy1;enum Cyc_Absyn_AggrKind _tmp1DB;if((((union Cyc_YYSTYPE*)_tmp1DA)->YY22).tag == 30){_LL1: _tmp1DB=(_tmp1DA->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1DB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1098
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY22).tag=30U,(_tmp6EB.YY22).val=yy1;_tmp6EB;});}static char _tmp1DF[8U]="tqual_t";
# 1092 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DF,_tmp1DF,_tmp1DF + 8U};
union Cyc_YYSTYPE*_tmp1DD=yy1;struct Cyc_Absyn_Tqual _tmp1DE;if((((union Cyc_YYSTYPE*)_tmp1DD)->YY23).tag == 31){_LL1: _tmp1DE=(_tmp1DD->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1DE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1099
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY23).tag=31U,(_tmp6EC.YY23).val=yy1;_tmp6EC;});}static char _tmp1E2[23U]="list_t<aggrfield_t,`H>";
# 1093 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E2,_tmp1E2,_tmp1E2 + 23U};
union Cyc_YYSTYPE*_tmp1E0=yy1;struct Cyc_List_List*_tmp1E1;if((((union Cyc_YYSTYPE*)_tmp1E0)->YY24).tag == 32){_LL1: _tmp1E1=(_tmp1E0->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1E1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1100
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY24).tag=32U,(_tmp6ED.YY24).val=yy1;_tmp6ED;});}static char _tmp1E5[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1094 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E5,_tmp1E5,_tmp1E5 + 34U};
union Cyc_YYSTYPE*_tmp1E3=yy1;struct Cyc_List_List*_tmp1E4;if((((union Cyc_YYSTYPE*)_tmp1E3)->YY25).tag == 33){_LL1: _tmp1E4=(_tmp1E3->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1E4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1101
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY25).tag=33U,(_tmp6EE.YY25).val=yy1;_tmp6EE;});}static char _tmp1E8[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1095 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E8,_tmp1E8,_tmp1E8 + 33U};
union Cyc_YYSTYPE*_tmp1E6=yy1;struct Cyc_List_List*_tmp1E7;if((((union Cyc_YYSTYPE*)_tmp1E6)->YY26).tag == 34){_LL1: _tmp1E7=(_tmp1E6->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1E7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1102
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY26).tag=34U,(_tmp6EF.YY26).val=yy1;_tmp6EF;});}static char _tmp1EB[18U]="declarator_t<`yy>";
# 1096 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EB,_tmp1EB,_tmp1EB + 18U};
union Cyc_YYSTYPE*_tmp1E9=yy1;struct Cyc_Parse_Declarator _tmp1EA;if((((union Cyc_YYSTYPE*)_tmp1E9)->YY27).tag == 35){_LL1: _tmp1EA=(_tmp1E9->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1EA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY27).tag=35U,(_tmp6F0.YY27).val=yy1;_tmp6F0;});}static char _tmp1EE[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1097 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EE,_tmp1EE,_tmp1EE + 45U};
union Cyc_YYSTYPE*_tmp1EC=yy1;struct _tuple25*_tmp1ED;if((((union Cyc_YYSTYPE*)_tmp1EC)->YY28).tag == 36){_LL1: _tmp1ED=(_tmp1EC->YY28).val;_LL2: {struct _tuple25*yy=_tmp1ED;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY28).tag=36U,(_tmp6F1.YY28).val=yy1;_tmp6F1;});}static char _tmp1F1[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1098 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F1,_tmp1F1,_tmp1F1 + 57U};
union Cyc_YYSTYPE*_tmp1EF=yy1;struct Cyc_List_List*_tmp1F0;if((((union Cyc_YYSTYPE*)_tmp1EF)->YY29).tag == 37){_LL1: _tmp1F0=(_tmp1EF->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1F0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY29).tag=37U,(_tmp6F2.YY29).val=yy1;_tmp6F2;});}static char _tmp1F4[26U]="abstractdeclarator_t<`yy>";
# 1099 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F4,_tmp1F4,_tmp1F4 + 26U};
union Cyc_YYSTYPE*_tmp1F2=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F3;if((((union Cyc_YYSTYPE*)_tmp1F2)->YY30).tag == 38){_LL1: _tmp1F3=(_tmp1F2->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1F3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1106
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY30).tag=38U,(_tmp6F3.YY30).val=yy1;_tmp6F3;});}static char _tmp1F7[5U]="bool";
# 1100 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F7,_tmp1F7,_tmp1F7 + 5U};
union Cyc_YYSTYPE*_tmp1F5=yy1;int _tmp1F6;if((((union Cyc_YYSTYPE*)_tmp1F5)->YY31).tag == 39){_LL1: _tmp1F6=(_tmp1F5->YY31).val;_LL2: {int yy=_tmp1F6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1107
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY31).tag=39U,(_tmp6F4.YY31).val=yy1;_tmp6F4;});}static char _tmp1FA[8U]="scope_t";
# 1101 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FA,_tmp1FA,_tmp1FA + 8U};
union Cyc_YYSTYPE*_tmp1F8=yy1;enum Cyc_Absyn_Scope _tmp1F9;if((((union Cyc_YYSTYPE*)_tmp1F8)->YY32).tag == 40){_LL1: _tmp1F9=(_tmp1F8->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1F9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY32).tag=40U,(_tmp6F5.YY32).val=yy1;_tmp6F5;});}static char _tmp1FD[16U]="datatypefield_t";
# 1102 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FD,_tmp1FD,_tmp1FD + 16U};
union Cyc_YYSTYPE*_tmp1FB=yy1;struct Cyc_Absyn_Datatypefield*_tmp1FC;if((((union Cyc_YYSTYPE*)_tmp1FB)->YY33).tag == 41){_LL1: _tmp1FC=(_tmp1FB->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp1FC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY33).tag=41U,(_tmp6F6.YY33).val=yy1;_tmp6F6;});}static char _tmp200[27U]="list_t<datatypefield_t,`H>";
# 1103 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp200,_tmp200,_tmp200 + 27U};
union Cyc_YYSTYPE*_tmp1FE=yy1;struct Cyc_List_List*_tmp1FF;if((((union Cyc_YYSTYPE*)_tmp1FE)->YY34).tag == 42){_LL1: _tmp1FF=(_tmp1FE->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp1FF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY34).tag=42U,(_tmp6F7.YY34).val=yy1;_tmp6F7;});}static char _tmp203[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1104 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp203,_tmp203,_tmp203 + 41U};
union Cyc_YYSTYPE*_tmp201=yy1;struct _tuple26 _tmp202;if((((union Cyc_YYSTYPE*)_tmp201)->YY35).tag == 43){_LL1: _tmp202=(_tmp201->YY35).val;_LL2: {struct _tuple26 yy=_tmp202;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY35).tag=43U,(_tmp6F8.YY35).val=yy1;_tmp6F8;});}static char _tmp206[17U]="list_t<var_t,`H>";
# 1105 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp206,_tmp206,_tmp206 + 17U};
union Cyc_YYSTYPE*_tmp204=yy1;struct Cyc_List_List*_tmp205;if((((union Cyc_YYSTYPE*)_tmp204)->YY36).tag == 44){_LL1: _tmp205=(_tmp204->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp205;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1112
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY36).tag=44U,(_tmp6F9.YY36).val=yy1;_tmp6F9;});}static char _tmp209[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1106 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp209,_tmp209,_tmp209 + 31U};
union Cyc_YYSTYPE*_tmp207=yy1;struct _tuple8*_tmp208;if((((union Cyc_YYSTYPE*)_tmp207)->YY37).tag == 45){_LL1: _tmp208=(_tmp207->YY37).val;_LL2: {struct _tuple8*yy=_tmp208;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1113
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY37).tag=45U,(_tmp6FA.YY37).val=yy1;_tmp6FA;});}static char _tmp20C[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1107 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20C,_tmp20C,_tmp20C + 42U};
union Cyc_YYSTYPE*_tmp20A=yy1;struct Cyc_List_List*_tmp20B;if((((union Cyc_YYSTYPE*)_tmp20A)->YY38).tag == 46){_LL1: _tmp20B=(_tmp20A->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp20B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1114
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY38).tag=46U,(_tmp6FB.YY38).val=yy1;_tmp6FB;});}static char _tmp20F[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1108 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20F,_tmp20F,_tmp20F + 115U};
union Cyc_YYSTYPE*_tmp20D=yy1;struct _tuple27*_tmp20E;if((((union Cyc_YYSTYPE*)_tmp20D)->YY39).tag == 47){_LL1: _tmp20E=(_tmp20D->YY39).val;_LL2: {struct _tuple27*yy=_tmp20E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1115
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY39).tag=47U,(_tmp6FC.YY39).val=yy1;_tmp6FC;});}static char _tmp212[8U]="types_t";
# 1109 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp212,_tmp212,_tmp212 + 8U};
union Cyc_YYSTYPE*_tmp210=yy1;struct Cyc_List_List*_tmp211;if((((union Cyc_YYSTYPE*)_tmp210)->YY40).tag == 48){_LL1: _tmp211=(_tmp210->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp211;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1116
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY40).tag=48U,(_tmp6FD.YY40).val=yy1;_tmp6FD;});}static char _tmp215[24U]="list_t<designator_t,`H>";
# 1111 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp215,_tmp215,_tmp215 + 24U};
union Cyc_YYSTYPE*_tmp213=yy1;struct Cyc_List_List*_tmp214;if((((union Cyc_YYSTYPE*)_tmp213)->YY41).tag == 49){_LL1: _tmp214=(_tmp213->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp214;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1118
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY41).tag=49U,(_tmp6FE.YY41).val=yy1;_tmp6FE;});}static char _tmp218[13U]="designator_t";
# 1112 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp218,_tmp218,_tmp218 + 13U};
union Cyc_YYSTYPE*_tmp216=yy1;void*_tmp217;if((((union Cyc_YYSTYPE*)_tmp216)->YY42).tag == 50){_LL1: _tmp217=(_tmp216->YY42).val;_LL2: {void*yy=_tmp217;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY42).tag=50U,(_tmp6FF.YY42).val=yy1;_tmp6FF;});}static char _tmp21B[7U]="kind_t";
# 1113 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21B,_tmp21B,_tmp21B + 7U};
union Cyc_YYSTYPE*_tmp219=yy1;struct Cyc_Absyn_Kind*_tmp21A;if((((union Cyc_YYSTYPE*)_tmp219)->YY43).tag == 51){_LL1: _tmp21A=(_tmp219->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp21A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY43).tag=51U,(_tmp700.YY43).val=yy1;_tmp700;});}static char _tmp21E[7U]="type_t";
# 1114 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21E,_tmp21E,_tmp21E + 7U};
union Cyc_YYSTYPE*_tmp21C=yy1;void*_tmp21D;if((((union Cyc_YYSTYPE*)_tmp21C)->YY44).tag == 52){_LL1: _tmp21D=(_tmp21C->YY44).val;_LL2: {void*yy=_tmp21D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY44).tag=52U,(_tmp701.YY44).val=yy1;_tmp701;});}static char _tmp221[23U]="list_t<attribute_t,`H>";
# 1115 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp221,_tmp221,_tmp221 + 23U};
union Cyc_YYSTYPE*_tmp21F=yy1;struct Cyc_List_List*_tmp220;if((((union Cyc_YYSTYPE*)_tmp21F)->YY45).tag == 53){_LL1: _tmp220=(_tmp21F->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp220;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY45).tag=53U,(_tmp702.YY45).val=yy1;_tmp702;});}static char _tmp224[12U]="attribute_t";
# 1116 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp224,_tmp224,_tmp224 + 12U};
union Cyc_YYSTYPE*_tmp222=yy1;void*_tmp223;if((((union Cyc_YYSTYPE*)_tmp222)->YY46).tag == 54){_LL1: _tmp223=(_tmp222->YY46).val;_LL2: {void*yy=_tmp223;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY46).tag=54U,(_tmp703.YY46).val=yy1;_tmp703;});}static char _tmp227[12U]="enumfield_t";
# 1117 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp227,_tmp227,_tmp227 + 12U};
union Cyc_YYSTYPE*_tmp225=yy1;struct Cyc_Absyn_Enumfield*_tmp226;if((((union Cyc_YYSTYPE*)_tmp225)->YY47).tag == 55){_LL1: _tmp226=(_tmp225->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp226;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1124
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY47).tag=55U,(_tmp704.YY47).val=yy1;_tmp704;});}static char _tmp22A[23U]="list_t<enumfield_t,`H>";
# 1118 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22A,_tmp22A,_tmp22A + 23U};
union Cyc_YYSTYPE*_tmp228=yy1;struct Cyc_List_List*_tmp229;if((((union Cyc_YYSTYPE*)_tmp228)->YY48).tag == 56){_LL1: _tmp229=(_tmp228->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp229;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1125
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY48).tag=56U,(_tmp705.YY48).val=yy1;_tmp705;});}static char _tmp22D[11U]="type_opt_t";
# 1119 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22D,_tmp22D,_tmp22D + 11U};
union Cyc_YYSTYPE*_tmp22B=yy1;void*_tmp22C;if((((union Cyc_YYSTYPE*)_tmp22B)->YY49).tag == 57){_LL1: _tmp22C=(_tmp22B->YY49).val;_LL2: {void*yy=_tmp22C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1126
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY49).tag=57U,(_tmp706.YY49).val=yy1;_tmp706;});}static char _tmp230[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1120 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp230,_tmp230,_tmp230 + 31U};
union Cyc_YYSTYPE*_tmp22E=yy1;struct Cyc_List_List*_tmp22F;if((((union Cyc_YYSTYPE*)_tmp22E)->YY50).tag == 58){_LL1: _tmp22F=(_tmp22E->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp22F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1127
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY50).tag=58U,(_tmp707.YY50).val=yy1;_tmp707;});}static char _tmp233[11U]="booltype_t";
# 1121 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp233,_tmp233,_tmp233 + 11U};
union Cyc_YYSTYPE*_tmp231=yy1;void*_tmp232;if((((union Cyc_YYSTYPE*)_tmp231)->YY51).tag == 59){_LL1: _tmp232=(_tmp231->YY51).val;_LL2: {void*yy=_tmp232;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1128
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY51).tag=59U,(_tmp708.YY51).val=yy1;_tmp708;});}static char _tmp236[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp236,_tmp236,_tmp236 + 35U};
union Cyc_YYSTYPE*_tmp234=yy1;struct Cyc_List_List*_tmp235;if((((union Cyc_YYSTYPE*)_tmp234)->YY52).tag == 60){_LL1: _tmp235=(_tmp234->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp235;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1129
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY52).tag=60U,(_tmp709.YY52).val=yy1;_tmp709;});}static char _tmp239[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1123 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp239,_tmp239,_tmp239 + 48U};
union Cyc_YYSTYPE*_tmp237=yy1;struct _tuple28*_tmp238;if((((union Cyc_YYSTYPE*)_tmp237)->YY53).tag == 61){_LL1: _tmp238=(_tmp237->YY53).val;_LL2: {struct _tuple28*yy=_tmp238;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY53).tag=61U,(_tmp70A.YY53).val=yy1;_tmp70A;});}static char _tmp23C[18U]="list_t<qvar_t,`H>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23C,_tmp23C,_tmp23C + 18U};
union Cyc_YYSTYPE*_tmp23A=yy1;struct Cyc_List_List*_tmp23B;if((((union Cyc_YYSTYPE*)_tmp23A)->YY54).tag == 62){_LL1: _tmp23B=(_tmp23A->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp23B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY54).tag=62U,(_tmp70B.YY54).val=yy1;_tmp70B;});}static char _tmp23F[20U]="pointer_qual_t<`yy>";
# 1125 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23F,_tmp23F,_tmp23F + 20U};
union Cyc_YYSTYPE*_tmp23D=yy1;void*_tmp23E;if((((union Cyc_YYSTYPE*)_tmp23D)->YY55).tag == 63){_LL1: _tmp23E=(_tmp23D->YY55).val;_LL2: {void*yy=_tmp23E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1132
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY55).tag=63U,(_tmp70C.YY55).val=yy1;_tmp70C;});}static char _tmp242[21U]="pointer_quals_t<`yy>";
# 1126 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp242,_tmp242,_tmp242 + 21U};
union Cyc_YYSTYPE*_tmp240=yy1;struct Cyc_List_List*_tmp241;if((((union Cyc_YYSTYPE*)_tmp240)->YY56).tag == 64){_LL1: _tmp241=(_tmp240->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp241;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1133
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY56).tag=64U,(_tmp70D.YY56).val=yy1;_tmp70D;});}static char _tmp245[10U]="exp_opt_t";
# 1127 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp245,_tmp245,_tmp245 + 10U};
union Cyc_YYSTYPE*_tmp243=yy1;struct Cyc_Absyn_Exp*_tmp244;if((((union Cyc_YYSTYPE*)_tmp243)->YY57).tag == 65){_LL1: _tmp244=(_tmp243->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp244;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1134
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY57).tag=65U,(_tmp70E.YY57).val=yy1;_tmp70E;});}static char _tmp248[10U]="raw_exp_t";
# 1128 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp248,_tmp248,_tmp248 + 10U};
union Cyc_YYSTYPE*_tmp246=yy1;void*_tmp247;if((((union Cyc_YYSTYPE*)_tmp246)->YY58).tag == 66){_LL1: _tmp247=(_tmp246->YY58).val;_LL2: {void*yy=_tmp247;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1135
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY58).tag=66U,(_tmp70F.YY58).val=yy1;_tmp70F;});}static char _tmp24B[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1130 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24B,_tmp24B,_tmp24B + 112U};
union Cyc_YYSTYPE*_tmp249=yy1;struct _tuple29*_tmp24A;if((((union Cyc_YYSTYPE*)_tmp249)->YY59).tag == 67){_LL1: _tmp24A=(_tmp249->YY59).val;_LL2: {struct _tuple29*yy=_tmp24A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY59).tag=67U,(_tmp710.YY59).val=yy1;_tmp710;});}static char _tmp24E[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1131 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24E,_tmp24E,_tmp24E + 73U};
union Cyc_YYSTYPE*_tmp24C=yy1;struct _tuple30*_tmp24D;if((((union Cyc_YYSTYPE*)_tmp24C)->YY60).tag == 68){_LL1: _tmp24D=(_tmp24C->YY60).val;_LL2: {struct _tuple30*yy=_tmp24D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY60).tag=68U,(_tmp711.YY60).val=yy1;_tmp711;});}static char _tmp251[28U]="list_t<string_t<`H>@`H, `H>";
# 1132 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp251,_tmp251,_tmp251 + 28U};
union Cyc_YYSTYPE*_tmp24F=yy1;struct Cyc_List_List*_tmp250;if((((union Cyc_YYSTYPE*)_tmp24F)->YY61).tag == 69){_LL1: _tmp250=(_tmp24F->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp250;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1139
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY61).tag=69U,(_tmp712.YY61).val=yy1;_tmp712;});}static char _tmp254[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1133 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp254,_tmp254,_tmp254 + 38U};
union Cyc_YYSTYPE*_tmp252=yy1;struct Cyc_List_List*_tmp253;if((((union Cyc_YYSTYPE*)_tmp252)->YY62).tag == 70){_LL1: _tmp253=(_tmp252->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp253;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1140
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY62).tag=70U,(_tmp713.YY62).val=yy1;_tmp713;});}static char _tmp257[26U]="$(string_t<`H>, exp_t)@`H";
# 1134 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp257,_tmp257,_tmp257 + 26U};
union Cyc_YYSTYPE*_tmp255=yy1;struct _tuple31*_tmp256;if((((union Cyc_YYSTYPE*)_tmp255)->YY63).tag == 71){_LL1: _tmp256=(_tmp255->YY63).val;_LL2: {struct _tuple31*yy=_tmp256;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY63).tag=71U,(_tmp714.YY63).val=yy1;_tmp714;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1157
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp715;_tmp715.timestamp=0,_tmp715.first_line=0,_tmp715.first_column=0,_tmp715.last_line=0,_tmp715.last_column=0;_tmp715;});}
# 1160
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1171 "parse.y"
static short Cyc_yytranslate[380U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,151,2,2,136,149,146,2,133,134,129,143,128,147,138,148,2,2,2,2,2,2,2,2,2,2,137,125,131,130,132,142,141,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,139,2,140,145,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,126,144,127,150,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124};static char _tmp258[2U]="$";static char _tmp259[6U]="error";static char _tmp25A[12U]="$undefined.";static char _tmp25B[5U]="AUTO";static char _tmp25C[9U]="REGISTER";static char _tmp25D[7U]="STATIC";static char _tmp25E[7U]="EXTERN";static char _tmp25F[8U]="TYPEDEF";static char _tmp260[5U]="VOID";static char _tmp261[5U]="CHAR";static char _tmp262[6U]="SHORT";static char _tmp263[4U]="INT";static char _tmp264[5U]="LONG";static char _tmp265[6U]="FLOAT";static char _tmp266[7U]="DOUBLE";static char _tmp267[7U]="SIGNED";static char _tmp268[9U]="UNSIGNED";static char _tmp269[6U]="CONST";static char _tmp26A[9U]="VOLATILE";static char _tmp26B[9U]="RESTRICT";static char _tmp26C[7U]="STRUCT";static char _tmp26D[6U]="UNION";static char _tmp26E[5U]="CASE";static char _tmp26F[8U]="DEFAULT";static char _tmp270[7U]="INLINE";static char _tmp271[7U]="SIZEOF";static char _tmp272[9U]="OFFSETOF";static char _tmp273[3U]="IF";static char _tmp274[5U]="ELSE";static char _tmp275[7U]="SWITCH";static char _tmp276[6U]="WHILE";static char _tmp277[3U]="DO";static char _tmp278[4U]="FOR";static char _tmp279[5U]="GOTO";static char _tmp27A[9U]="CONTINUE";static char _tmp27B[6U]="BREAK";static char _tmp27C[7U]="RETURN";static char _tmp27D[5U]="ENUM";static char _tmp27E[7U]="TYPEOF";static char _tmp27F[16U]="BUILTIN_VA_LIST";static char _tmp280[10U]="EXTENSION";static char _tmp281[8U]="NULL_kw";static char _tmp282[4U]="LET";static char _tmp283[6U]="THROW";static char _tmp284[4U]="TRY";static char _tmp285[6U]="CATCH";static char _tmp286[7U]="EXPORT";static char _tmp287[9U]="OVERRIDE";static char _tmp288[5U]="HIDE";static char _tmp289[4U]="NEW";static char _tmp28A[9U]="ABSTRACT";static char _tmp28B[9U]="FALLTHRU";static char _tmp28C[6U]="USING";static char _tmp28D[10U]="NAMESPACE";static char _tmp28E[9U]="DATATYPE";static char _tmp28F[7U]="MALLOC";static char _tmp290[8U]="RMALLOC";static char _tmp291[15U]="RMALLOC_INLINE";static char _tmp292[7U]="CALLOC";static char _tmp293[8U]="RCALLOC";static char _tmp294[5U]="SWAP";static char _tmp295[9U]="REGION_T";static char _tmp296[6U]="TAG_T";static char _tmp297[7U]="REGION";static char _tmp298[5U]="RNEW";static char _tmp299[8U]="REGIONS";static char _tmp29A[7U]="PORTON";static char _tmp29B[8U]="PORTOFF";static char _tmp29C[7U]="PRAGMA";static char _tmp29D[10U]="TEMPESTON";static char _tmp29E[11U]="TEMPESTOFF";static char _tmp29F[8U]="NUMELTS";static char _tmp2A0[8U]="VALUEOF";static char _tmp2A1[10U]="VALUEOF_T";static char _tmp2A2[9U]="TAGCHECK";static char _tmp2A3[13U]="NUMELTS_QUAL";static char _tmp2A4[10U]="THIN_QUAL";static char _tmp2A5[9U]="FAT_QUAL";static char _tmp2A6[13U]="NOTNULL_QUAL";static char _tmp2A7[14U]="NULLABLE_QUAL";static char _tmp2A8[14U]="REQUIRES_QUAL";static char _tmp2A9[13U]="ENSURES_QUAL";static char _tmp2AA[12U]="REGION_QUAL";static char _tmp2AB[16U]="NOZEROTERM_QUAL";static char _tmp2AC[14U]="ZEROTERM_QUAL";static char _tmp2AD[12U]="TAGGED_QUAL";static char _tmp2AE[12U]="ASSERT_QUAL";static char _tmp2AF[16U]="EXTENSIBLE_QUAL";static char _tmp2B0[7U]="PTR_OP";static char _tmp2B1[7U]="INC_OP";static char _tmp2B2[7U]="DEC_OP";static char _tmp2B3[8U]="LEFT_OP";static char _tmp2B4[9U]="RIGHT_OP";static char _tmp2B5[6U]="LE_OP";static char _tmp2B6[6U]="GE_OP";static char _tmp2B7[6U]="EQ_OP";static char _tmp2B8[6U]="NE_OP";static char _tmp2B9[7U]="AND_OP";static char _tmp2BA[6U]="OR_OP";static char _tmp2BB[11U]="MUL_ASSIGN";static char _tmp2BC[11U]="DIV_ASSIGN";static char _tmp2BD[11U]="MOD_ASSIGN";static char _tmp2BE[11U]="ADD_ASSIGN";static char _tmp2BF[11U]="SUB_ASSIGN";static char _tmp2C0[12U]="LEFT_ASSIGN";static char _tmp2C1[13U]="RIGHT_ASSIGN";static char _tmp2C2[11U]="AND_ASSIGN";static char _tmp2C3[11U]="XOR_ASSIGN";static char _tmp2C4[10U]="OR_ASSIGN";static char _tmp2C5[9U]="ELLIPSIS";static char _tmp2C6[11U]="LEFT_RIGHT";static char _tmp2C7[12U]="COLON_COLON";static char _tmp2C8[11U]="IDENTIFIER";static char _tmp2C9[17U]="INTEGER_CONSTANT";static char _tmp2CA[7U]="STRING";static char _tmp2CB[8U]="WSTRING";static char _tmp2CC[19U]="CHARACTER_CONSTANT";static char _tmp2CD[20U]="WCHARACTER_CONSTANT";static char _tmp2CE[18U]="FLOATING_CONSTANT";static char _tmp2CF[9U]="TYPE_VAR";static char _tmp2D0[13U]="TYPEDEF_NAME";static char _tmp2D1[16U]="QUAL_IDENTIFIER";static char _tmp2D2[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D3[10U]="ATTRIBUTE";static char _tmp2D4[8U]="ASM_TOK";static char _tmp2D5[4U]="';'";static char _tmp2D6[4U]="'{'";static char _tmp2D7[4U]="'}'";static char _tmp2D8[4U]="','";static char _tmp2D9[4U]="'*'";static char _tmp2DA[4U]="'='";static char _tmp2DB[4U]="'<'";static char _tmp2DC[4U]="'>'";static char _tmp2DD[4U]="'('";static char _tmp2DE[4U]="')'";static char _tmp2DF[4U]="'_'";static char _tmp2E0[4U]="'$'";static char _tmp2E1[4U]="':'";static char _tmp2E2[4U]="'.'";static char _tmp2E3[4U]="'['";static char _tmp2E4[4U]="']'";static char _tmp2E5[4U]="'@'";static char _tmp2E6[4U]="'?'";static char _tmp2E7[4U]="'+'";static char _tmp2E8[4U]="'|'";static char _tmp2E9[4U]="'^'";static char _tmp2EA[4U]="'&'";static char _tmp2EB[4U]="'-'";static char _tmp2EC[4U]="'/'";static char _tmp2ED[4U]="'%'";static char _tmp2EE[4U]="'~'";static char _tmp2EF[4U]="'!'";static char _tmp2F0[5U]="prog";static char _tmp2F1[17U]="translation_unit";static char _tmp2F2[18U]="tempest_on_action";static char _tmp2F3[19U]="tempest_off_action";static char _tmp2F4[16U]="extern_c_action";static char _tmp2F5[13U]="end_extern_c";static char _tmp2F6[14U]="hide_list_opt";static char _tmp2F7[17U]="hide_list_values";static char _tmp2F8[16U]="export_list_opt";static char _tmp2F9[12U]="export_list";static char _tmp2FA[19U]="export_list_values";static char _tmp2FB[13U]="override_opt";static char _tmp2FC[21U]="external_declaration";static char _tmp2FD[15U]="optional_comma";static char _tmp2FE[20U]="function_definition";static char _tmp2FF[21U]="function_definition2";static char _tmp300[13U]="using_action";static char _tmp301[15U]="unusing_action";static char _tmp302[17U]="namespace_action";static char _tmp303[19U]="unnamespace_action";static char _tmp304[12U]="declaration";static char _tmp305[17U]="declaration_list";static char _tmp306[23U]="declaration_specifiers";static char _tmp307[24U]="storage_class_specifier";static char _tmp308[15U]="attributes_opt";static char _tmp309[11U]="attributes";static char _tmp30A[15U]="attribute_list";static char _tmp30B[10U]="attribute";static char _tmp30C[15U]="type_specifier";static char _tmp30D[25U]="type_specifier_notypedef";static char _tmp30E[5U]="kind";static char _tmp30F[15U]="type_qualifier";static char _tmp310[15U]="enum_specifier";static char _tmp311[11U]="enum_field";static char _tmp312[22U]="enum_declaration_list";static char _tmp313[26U]="struct_or_union_specifier";static char _tmp314[16U]="type_params_opt";static char _tmp315[16U]="struct_or_union";static char _tmp316[24U]="struct_declaration_list";static char _tmp317[25U]="struct_declaration_list0";static char _tmp318[21U]="init_declarator_list";static char _tmp319[22U]="init_declarator_list0";static char _tmp31A[16U]="init_declarator";static char _tmp31B[19U]="struct_declaration";static char _tmp31C[25U]="specifier_qualifier_list";static char _tmp31D[35U]="notypedef_specifier_qualifier_list";static char _tmp31E[23U]="struct_declarator_list";static char _tmp31F[24U]="struct_declarator_list0";static char _tmp320[18U]="struct_declarator";static char _tmp321[20U]="requires_clause_opt";static char _tmp322[19U]="ensures_clause_opt";static char _tmp323[19U]="datatype_specifier";static char _tmp324[14U]="qual_datatype";static char _tmp325[19U]="datatypefield_list";static char _tmp326[20U]="datatypefield_scope";static char _tmp327[14U]="datatypefield";static char _tmp328[11U]="declarator";static char _tmp329[23U]="declarator_withtypedef";static char _tmp32A[18U]="direct_declarator";static char _tmp32B[30U]="direct_declarator_withtypedef";static char _tmp32C[8U]="pointer";static char _tmp32D[12U]="one_pointer";static char _tmp32E[14U]="pointer_quals";static char _tmp32F[13U]="pointer_qual";static char _tmp330[23U]="pointer_null_and_bound";static char _tmp331[14U]="pointer_bound";static char _tmp332[18U]="zeroterm_qual_opt";static char _tmp333[8U]="rgn_opt";static char _tmp334[11U]="tqual_list";static char _tmp335[20U]="parameter_type_list";static char _tmp336[9U]="type_var";static char _tmp337[16U]="optional_effect";static char _tmp338[19U]="optional_rgn_order";static char _tmp339[10U]="rgn_order";static char _tmp33A[16U]="optional_inject";static char _tmp33B[11U]="effect_set";static char _tmp33C[14U]="atomic_effect";static char _tmp33D[11U]="region_set";static char _tmp33E[15U]="parameter_list";static char _tmp33F[22U]="parameter_declaration";static char _tmp340[16U]="identifier_list";static char _tmp341[17U]="identifier_list0";static char _tmp342[12U]="initializer";static char _tmp343[18U]="array_initializer";static char _tmp344[17U]="initializer_list";static char _tmp345[12U]="designation";static char _tmp346[16U]="designator_list";static char _tmp347[11U]="designator";static char _tmp348[10U]="type_name";static char _tmp349[14U]="any_type_name";static char _tmp34A[15U]="type_name_list";static char _tmp34B[20U]="abstract_declarator";static char _tmp34C[27U]="direct_abstract_declarator";static char _tmp34D[10U]="statement";static char _tmp34E[18U]="labeled_statement";static char _tmp34F[21U]="expression_statement";static char _tmp350[19U]="compound_statement";static char _tmp351[16U]="block_item_list";static char _tmp352[20U]="selection_statement";static char _tmp353[15U]="switch_clauses";static char _tmp354[20U]="iteration_statement";static char _tmp355[15U]="jump_statement";static char _tmp356[12U]="exp_pattern";static char _tmp357[20U]="conditional_pattern";static char _tmp358[19U]="logical_or_pattern";static char _tmp359[20U]="logical_and_pattern";static char _tmp35A[21U]="inclusive_or_pattern";static char _tmp35B[21U]="exclusive_or_pattern";static char _tmp35C[12U]="and_pattern";static char _tmp35D[17U]="equality_pattern";static char _tmp35E[19U]="relational_pattern";static char _tmp35F[14U]="shift_pattern";static char _tmp360[17U]="additive_pattern";static char _tmp361[23U]="multiplicative_pattern";static char _tmp362[13U]="cast_pattern";static char _tmp363[14U]="unary_pattern";static char _tmp364[16U]="postfix_pattern";static char _tmp365[16U]="primary_pattern";static char _tmp366[8U]="pattern";static char _tmp367[19U]="tuple_pattern_list";static char _tmp368[20U]="tuple_pattern_list0";static char _tmp369[14U]="field_pattern";static char _tmp36A[19U]="field_pattern_list";static char _tmp36B[20U]="field_pattern_list0";static char _tmp36C[11U]="expression";static char _tmp36D[22U]="assignment_expression";static char _tmp36E[20U]="assignment_operator";static char _tmp36F[23U]="conditional_expression";static char _tmp370[20U]="constant_expression";static char _tmp371[22U]="logical_or_expression";static char _tmp372[23U]="logical_and_expression";static char _tmp373[24U]="inclusive_or_expression";static char _tmp374[24U]="exclusive_or_expression";static char _tmp375[15U]="and_expression";static char _tmp376[20U]="equality_expression";static char _tmp377[22U]="relational_expression";static char _tmp378[17U]="shift_expression";static char _tmp379[20U]="additive_expression";static char _tmp37A[26U]="multiplicative_expression";static char _tmp37B[16U]="cast_expression";static char _tmp37C[17U]="unary_expression";static char _tmp37D[9U]="asm_expr";static char _tmp37E[13U]="volatile_opt";static char _tmp37F[12U]="asm_out_opt";static char _tmp380[12U]="asm_outlist";static char _tmp381[11U]="asm_in_opt";static char _tmp382[11U]="asm_inlist";static char _tmp383[11U]="asm_io_elt";static char _tmp384[16U]="asm_clobber_opt";static char _tmp385[17U]="asm_clobber_list";static char _tmp386[15U]="unary_operator";static char _tmp387[19U]="postfix_expression";static char _tmp388[17U]="field_expression";static char _tmp389[19U]="primary_expression";static char _tmp38A[25U]="argument_expression_list";static char _tmp38B[26U]="argument_expression_list0";static char _tmp38C[9U]="constant";static char _tmp38D[20U]="qual_opt_identifier";static char _tmp38E[17U]="qual_opt_typedef";static char _tmp38F[18U]="struct_union_name";static char _tmp390[11U]="field_name";static char _tmp391[12U]="right_angle";
# 1543 "parse.y"
static struct _fat_ptr Cyc_yytname[314U]={{_tmp258,_tmp258,_tmp258 + 2U},{_tmp259,_tmp259,_tmp259 + 6U},{_tmp25A,_tmp25A,_tmp25A + 12U},{_tmp25B,_tmp25B,_tmp25B + 5U},{_tmp25C,_tmp25C,_tmp25C + 9U},{_tmp25D,_tmp25D,_tmp25D + 7U},{_tmp25E,_tmp25E,_tmp25E + 7U},{_tmp25F,_tmp25F,_tmp25F + 8U},{_tmp260,_tmp260,_tmp260 + 5U},{_tmp261,_tmp261,_tmp261 + 5U},{_tmp262,_tmp262,_tmp262 + 6U},{_tmp263,_tmp263,_tmp263 + 4U},{_tmp264,_tmp264,_tmp264 + 5U},{_tmp265,_tmp265,_tmp265 + 6U},{_tmp266,_tmp266,_tmp266 + 7U},{_tmp267,_tmp267,_tmp267 + 7U},{_tmp268,_tmp268,_tmp268 + 9U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 9U},{_tmp26B,_tmp26B,_tmp26B + 9U},{_tmp26C,_tmp26C,_tmp26C + 7U},{_tmp26D,_tmp26D,_tmp26D + 6U},{_tmp26E,_tmp26E,_tmp26E + 5U},{_tmp26F,_tmp26F,_tmp26F + 8U},{_tmp270,_tmp270,_tmp270 + 7U},{_tmp271,_tmp271,_tmp271 + 7U},{_tmp272,_tmp272,_tmp272 + 9U},{_tmp273,_tmp273,_tmp273 + 3U},{_tmp274,_tmp274,_tmp274 + 5U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 6U},{_tmp277,_tmp277,_tmp277 + 3U},{_tmp278,_tmp278,_tmp278 + 4U},{_tmp279,_tmp279,_tmp279 + 5U},{_tmp27A,_tmp27A,_tmp27A + 9U},{_tmp27B,_tmp27B,_tmp27B + 6U},{_tmp27C,_tmp27C,_tmp27C + 7U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 7U},{_tmp27F,_tmp27F,_tmp27F + 16U},{_tmp280,_tmp280,_tmp280 + 10U},{_tmp281,_tmp281,_tmp281 + 8U},{_tmp282,_tmp282,_tmp282 + 4U},{_tmp283,_tmp283,_tmp283 + 6U},{_tmp284,_tmp284,_tmp284 + 4U},{_tmp285,_tmp285,_tmp285 + 6U},{_tmp286,_tmp286,_tmp286 + 7U},{_tmp287,_tmp287,_tmp287 + 9U},{_tmp288,_tmp288,_tmp288 + 5U},{_tmp289,_tmp289,_tmp289 + 4U},{_tmp28A,_tmp28A,_tmp28A + 9U},{_tmp28B,_tmp28B,_tmp28B + 9U},{_tmp28C,_tmp28C,_tmp28C + 6U},{_tmp28D,_tmp28D,_tmp28D + 10U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 7U},{_tmp290,_tmp290,_tmp290 + 8U},{_tmp291,_tmp291,_tmp291 + 15U},{_tmp292,_tmp292,_tmp292 + 7U},{_tmp293,_tmp293,_tmp293 + 8U},{_tmp294,_tmp294,_tmp294 + 5U},{_tmp295,_tmp295,_tmp295 + 9U},{_tmp296,_tmp296,_tmp296 + 6U},{_tmp297,_tmp297,_tmp297 + 7U},{_tmp298,_tmp298,_tmp298 + 5U},{_tmp299,_tmp299,_tmp299 + 8U},{_tmp29A,_tmp29A,_tmp29A + 7U},{_tmp29B,_tmp29B,_tmp29B + 8U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 10U},{_tmp29E,_tmp29E,_tmp29E + 11U},{_tmp29F,_tmp29F,_tmp29F + 8U},{_tmp2A0,_tmp2A0,_tmp2A0 + 8U},{_tmp2A1,_tmp2A1,_tmp2A1 + 10U},{_tmp2A2,_tmp2A2,_tmp2A2 + 9U},{_tmp2A3,_tmp2A3,_tmp2A3 + 13U},{_tmp2A4,_tmp2A4,_tmp2A4 + 10U},{_tmp2A5,_tmp2A5,_tmp2A5 + 9U},{_tmp2A6,_tmp2A6,_tmp2A6 + 13U},{_tmp2A7,_tmp2A7,_tmp2A7 + 14U},{_tmp2A8,_tmp2A8,_tmp2A8 + 14U},{_tmp2A9,_tmp2A9,_tmp2A9 + 13U},{_tmp2AA,_tmp2AA,_tmp2AA + 12U},{_tmp2AB,_tmp2AB,_tmp2AB + 16U},{_tmp2AC,_tmp2AC,_tmp2AC + 14U},{_tmp2AD,_tmp2AD,_tmp2AD + 12U},{_tmp2AE,_tmp2AE,_tmp2AE + 12U},{_tmp2AF,_tmp2AF,_tmp2AF + 16U},{_tmp2B0,_tmp2B0,_tmp2B0 + 7U},{_tmp2B1,_tmp2B1,_tmp2B1 + 7U},{_tmp2B2,_tmp2B2,_tmp2B2 + 7U},{_tmp2B3,_tmp2B3,_tmp2B3 + 8U},{_tmp2B4,_tmp2B4,_tmp2B4 + 9U},{_tmp2B5,_tmp2B5,_tmp2B5 + 6U},{_tmp2B6,_tmp2B6,_tmp2B6 + 6U},{_tmp2B7,_tmp2B7,_tmp2B7 + 6U},{_tmp2B8,_tmp2B8,_tmp2B8 + 6U},{_tmp2B9,_tmp2B9,_tmp2B9 + 7U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 11U},{_tmp2BC,_tmp2BC,_tmp2BC + 11U},{_tmp2BD,_tmp2BD,_tmp2BD + 11U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 12U},{_tmp2C1,_tmp2C1,_tmp2C1 + 13U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 10U},{_tmp2C5,_tmp2C5,_tmp2C5 + 9U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 12U},{_tmp2C8,_tmp2C8,_tmp2C8 + 11U},{_tmp2C9,_tmp2C9,_tmp2C9 + 17U},{_tmp2CA,_tmp2CA,_tmp2CA + 7U},{_tmp2CB,_tmp2CB,_tmp2CB + 8U},{_tmp2CC,_tmp2CC,_tmp2CC + 19U},{_tmp2CD,_tmp2CD,_tmp2CD + 20U},{_tmp2CE,_tmp2CE,_tmp2CE + 18U},{_tmp2CF,_tmp2CF,_tmp2CF + 9U},{_tmp2D0,_tmp2D0,_tmp2D0 + 13U},{_tmp2D1,_tmp2D1,_tmp2D1 + 16U},{_tmp2D2,_tmp2D2,_tmp2D2 + 18U},{_tmp2D3,_tmp2D3,_tmp2D3 + 10U},{_tmp2D4,_tmp2D4,_tmp2D4 + 8U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 5U},{_tmp2F1,_tmp2F1,_tmp2F1 + 17U},{_tmp2F2,_tmp2F2,_tmp2F2 + 18U},{_tmp2F3,_tmp2F3,_tmp2F3 + 19U},{_tmp2F4,_tmp2F4,_tmp2F4 + 16U},{_tmp2F5,_tmp2F5,_tmp2F5 + 13U},{_tmp2F6,_tmp2F6,_tmp2F6 + 14U},{_tmp2F7,_tmp2F7,_tmp2F7 + 17U},{_tmp2F8,_tmp2F8,_tmp2F8 + 16U},{_tmp2F9,_tmp2F9,_tmp2F9 + 12U},{_tmp2FA,_tmp2FA,_tmp2FA + 19U},{_tmp2FB,_tmp2FB,_tmp2FB + 13U},{_tmp2FC,_tmp2FC,_tmp2FC + 21U},{_tmp2FD,_tmp2FD,_tmp2FD + 15U},{_tmp2FE,_tmp2FE,_tmp2FE + 20U},{_tmp2FF,_tmp2FF,_tmp2FF + 21U},{_tmp300,_tmp300,_tmp300 + 13U},{_tmp301,_tmp301,_tmp301 + 15U},{_tmp302,_tmp302,_tmp302 + 17U},{_tmp303,_tmp303,_tmp303 + 19U},{_tmp304,_tmp304,_tmp304 + 12U},{_tmp305,_tmp305,_tmp305 + 17U},{_tmp306,_tmp306,_tmp306 + 23U},{_tmp307,_tmp307,_tmp307 + 24U},{_tmp308,_tmp308,_tmp308 + 15U},{_tmp309,_tmp309,_tmp309 + 11U},{_tmp30A,_tmp30A,_tmp30A + 15U},{_tmp30B,_tmp30B,_tmp30B + 10U},{_tmp30C,_tmp30C,_tmp30C + 15U},{_tmp30D,_tmp30D,_tmp30D + 25U},{_tmp30E,_tmp30E,_tmp30E + 5U},{_tmp30F,_tmp30F,_tmp30F + 15U},{_tmp310,_tmp310,_tmp310 + 15U},{_tmp311,_tmp311,_tmp311 + 11U},{_tmp312,_tmp312,_tmp312 + 22U},{_tmp313,_tmp313,_tmp313 + 26U},{_tmp314,_tmp314,_tmp314 + 16U},{_tmp315,_tmp315,_tmp315 + 16U},{_tmp316,_tmp316,_tmp316 + 24U},{_tmp317,_tmp317,_tmp317 + 25U},{_tmp318,_tmp318,_tmp318 + 21U},{_tmp319,_tmp319,_tmp319 + 22U},{_tmp31A,_tmp31A,_tmp31A + 16U},{_tmp31B,_tmp31B,_tmp31B + 19U},{_tmp31C,_tmp31C,_tmp31C + 25U},{_tmp31D,_tmp31D,_tmp31D + 35U},{_tmp31E,_tmp31E,_tmp31E + 23U},{_tmp31F,_tmp31F,_tmp31F + 24U},{_tmp320,_tmp320,_tmp320 + 18U},{_tmp321,_tmp321,_tmp321 + 20U},{_tmp322,_tmp322,_tmp322 + 19U},{_tmp323,_tmp323,_tmp323 + 19U},{_tmp324,_tmp324,_tmp324 + 14U},{_tmp325,_tmp325,_tmp325 + 19U},{_tmp326,_tmp326,_tmp326 + 20U},{_tmp327,_tmp327,_tmp327 + 14U},{_tmp328,_tmp328,_tmp328 + 11U},{_tmp329,_tmp329,_tmp329 + 23U},{_tmp32A,_tmp32A,_tmp32A + 18U},{_tmp32B,_tmp32B,_tmp32B + 30U},{_tmp32C,_tmp32C,_tmp32C + 8U},{_tmp32D,_tmp32D,_tmp32D + 12U},{_tmp32E,_tmp32E,_tmp32E + 14U},{_tmp32F,_tmp32F,_tmp32F + 13U},{_tmp330,_tmp330,_tmp330 + 23U},{_tmp331,_tmp331,_tmp331 + 14U},{_tmp332,_tmp332,_tmp332 + 18U},{_tmp333,_tmp333,_tmp333 + 8U},{_tmp334,_tmp334,_tmp334 + 11U},{_tmp335,_tmp335,_tmp335 + 20U},{_tmp336,_tmp336,_tmp336 + 9U},{_tmp337,_tmp337,_tmp337 + 16U},{_tmp338,_tmp338,_tmp338 + 19U},{_tmp339,_tmp339,_tmp339 + 10U},{_tmp33A,_tmp33A,_tmp33A + 16U},{_tmp33B,_tmp33B,_tmp33B + 11U},{_tmp33C,_tmp33C,_tmp33C + 14U},{_tmp33D,_tmp33D,_tmp33D + 11U},{_tmp33E,_tmp33E,_tmp33E + 15U},{_tmp33F,_tmp33F,_tmp33F + 22U},{_tmp340,_tmp340,_tmp340 + 16U},{_tmp341,_tmp341,_tmp341 + 17U},{_tmp342,_tmp342,_tmp342 + 12U},{_tmp343,_tmp343,_tmp343 + 18U},{_tmp344,_tmp344,_tmp344 + 17U},{_tmp345,_tmp345,_tmp345 + 12U},{_tmp346,_tmp346,_tmp346 + 16U},{_tmp347,_tmp347,_tmp347 + 11U},{_tmp348,_tmp348,_tmp348 + 10U},{_tmp349,_tmp349,_tmp349 + 14U},{_tmp34A,_tmp34A,_tmp34A + 15U},{_tmp34B,_tmp34B,_tmp34B + 20U},{_tmp34C,_tmp34C,_tmp34C + 27U},{_tmp34D,_tmp34D,_tmp34D + 10U},{_tmp34E,_tmp34E,_tmp34E + 18U},{_tmp34F,_tmp34F,_tmp34F + 21U},{_tmp350,_tmp350,_tmp350 + 19U},{_tmp351,_tmp351,_tmp351 + 16U},{_tmp352,_tmp352,_tmp352 + 20U},{_tmp353,_tmp353,_tmp353 + 15U},{_tmp354,_tmp354,_tmp354 + 20U},{_tmp355,_tmp355,_tmp355 + 15U},{_tmp356,_tmp356,_tmp356 + 12U},{_tmp357,_tmp357,_tmp357 + 20U},{_tmp358,_tmp358,_tmp358 + 19U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 21U},{_tmp35B,_tmp35B,_tmp35B + 21U},{_tmp35C,_tmp35C,_tmp35C + 12U},{_tmp35D,_tmp35D,_tmp35D + 17U},{_tmp35E,_tmp35E,_tmp35E + 19U},{_tmp35F,_tmp35F,_tmp35F + 14U},{_tmp360,_tmp360,_tmp360 + 17U},{_tmp361,_tmp361,_tmp361 + 23U},{_tmp362,_tmp362,_tmp362 + 13U},{_tmp363,_tmp363,_tmp363 + 14U},{_tmp364,_tmp364,_tmp364 + 16U},{_tmp365,_tmp365,_tmp365 + 16U},{_tmp366,_tmp366,_tmp366 + 8U},{_tmp367,_tmp367,_tmp367 + 19U},{_tmp368,_tmp368,_tmp368 + 20U},{_tmp369,_tmp369,_tmp369 + 14U},{_tmp36A,_tmp36A,_tmp36A + 19U},{_tmp36B,_tmp36B,_tmp36B + 20U},{_tmp36C,_tmp36C,_tmp36C + 11U},{_tmp36D,_tmp36D,_tmp36D + 22U},{_tmp36E,_tmp36E,_tmp36E + 20U},{_tmp36F,_tmp36F,_tmp36F + 23U},{_tmp370,_tmp370,_tmp370 + 20U},{_tmp371,_tmp371,_tmp371 + 22U},{_tmp372,_tmp372,_tmp372 + 23U},{_tmp373,_tmp373,_tmp373 + 24U},{_tmp374,_tmp374,_tmp374 + 24U},{_tmp375,_tmp375,_tmp375 + 15U},{_tmp376,_tmp376,_tmp376 + 20U},{_tmp377,_tmp377,_tmp377 + 22U},{_tmp378,_tmp378,_tmp378 + 17U},{_tmp379,_tmp379,_tmp379 + 20U},{_tmp37A,_tmp37A,_tmp37A + 26U},{_tmp37B,_tmp37B,_tmp37B + 16U},{_tmp37C,_tmp37C,_tmp37C + 17U},{_tmp37D,_tmp37D,_tmp37D + 9U},{_tmp37E,_tmp37E,_tmp37E + 13U},{_tmp37F,_tmp37F,_tmp37F + 12U},{_tmp380,_tmp380,_tmp380 + 12U},{_tmp381,_tmp381,_tmp381 + 11U},{_tmp382,_tmp382,_tmp382 + 11U},{_tmp383,_tmp383,_tmp383 + 11U},{_tmp384,_tmp384,_tmp384 + 16U},{_tmp385,_tmp385,_tmp385 + 17U},{_tmp386,_tmp386,_tmp386 + 15U},{_tmp387,_tmp387,_tmp387 + 19U},{_tmp388,_tmp388,_tmp388 + 17U},{_tmp389,_tmp389,_tmp389 + 19U},{_tmp38A,_tmp38A,_tmp38A + 25U},{_tmp38B,_tmp38B,_tmp38B + 26U},{_tmp38C,_tmp38C,_tmp38C + 9U},{_tmp38D,_tmp38D,_tmp38D + 20U},{_tmp38E,_tmp38E,_tmp38E + 17U},{_tmp38F,_tmp38F,_tmp38F + 18U},{_tmp390,_tmp390,_tmp390 + 11U},{_tmp391,_tmp391,_tmp391 + 12U}};
# 1601
static short Cyc_yyr1[562U]={0,152,153,153,153,153,153,153,153,153,153,153,153,154,155,156,157,158,158,159,159,159,160,160,161,161,161,162,162,162,163,163,164,164,164,165,165,166,166,166,166,167,167,168,169,170,171,172,172,172,172,172,172,172,173,173,174,174,174,174,174,174,174,174,174,174,174,175,175,175,175,175,175,175,176,176,177,178,178,179,179,179,179,180,180,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,182,183,183,183,184,184,184,185,185,186,186,186,187,187,187,187,187,188,188,189,189,190,190,191,191,192,193,193,194,194,195,196,196,196,196,196,196,197,197,197,197,197,197,198,199,199,200,200,200,200,201,201,202,202,203,203,203,204,204,205,205,205,205,206,206,206,207,207,208,208,209,209,210,210,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,211,212,212,213,214,214,215,215,215,215,215,215,215,215,216,216,216,217,217,218,218,218,219,219,219,220,220,221,221,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,228,228,229,229,230,230,231,231,231,232,233,233,234,234,235,235,235,235,235,236,236,236,236,237,237,238,238,239,239,240,240,241,241,241,241,241,242,242,243,243,243,244,244,244,244,244,244,244,244,244,244,244,245,245,245,245,245,245,246,247,247,248,248,249,249,249,249,249,249,249,249,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,252,252,252,252,252,252,252,252,253,253,253,253,253,253,253,253,254,255,255,256,256,257,257,258,258,259,259,260,260,261,261,261,262,262,262,262,262,263,263,263,264,264,264,265,265,265,265,266,266,267,267,267,267,267,267,268,269,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,271,271,271,272,272,273,273,274,274,274,275,275,276,276,277,277,277,278,278,278,278,278,278,278,278,278,278,278,279,279,279,279,279,279,279,280,281,281,282,282,283,283,284,284,285,285,286,286,286,287,287,287,287,287,288,288,288,289,289,289,290,290,290,290,291,291,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,293,294,294,295,295,295,296,296,297,297,297,298,298,299,300,300,300,301,301,302,302,302,303,303,303,303,303,303,303,303,303,303,303,304,304,304,304,305,305,305,305,305,305,305,305,305,305,305,306,307,307,308,308,308,308,308,309,309,310,310,311,311,312,312,313,313};
# 1661
static short Cyc_yyr2[562U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1721
static short Cyc_yydefact[1137U]={0,34,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,127,128,63,0,0,97,0,0,73,0,0,165,104,106,0,0,0,13,14,0,0,0,552,231,554,553,555,0,217,0,100,0,217,216,1,0,0,0,0,32,0,0,33,0,56,65,59,83,61,94,95,0,98,0,0,176,0,201,204,99,180,125,71,70,64,0,113,0,58,551,0,552,547,548,549,550,0,125,0,0,391,0,0,0,254,0,393,394,43,45,0,0,0,0,0,0,0,0,166,0,0,0,214,0,0,0,0,215,0,0,0,2,0,0,0,0,47,0,133,134,136,57,66,60,62,129,556,557,125,125,0,54,0,0,36,0,233,0,189,177,202,0,208,209,212,213,0,211,210,222,204,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,536,537,497,0,0,0,0,0,517,515,516,0,420,422,436,444,446,448,450,452,454,457,462,465,468,472,0,474,518,535,533,552,403,0,0,0,0,404,0,0,402,50,0,0,125,0,0,0,143,139,141,274,276,0,0,52,0,0,8,9,0,125,558,559,232,108,0,0,0,181,101,252,0,249,10,11,0,3,0,5,0,48,0,0,0,36,0,130,131,156,124,0,163,0,0,0,0,0,0,0,0,0,0,0,0,552,304,306,0,314,308,0,312,297,298,299,0,300,301,302,0,55,36,136,35,37,281,0,239,255,0,0,235,233,0,219,0,0,0,224,74,223,205,0,118,114,0,0,0,482,0,0,494,438,472,0,439,440,0,0,0,0,0,0,0,0,0,0,0,475,476,498,493,0,478,0,0,0,0,479,477,0,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,426,427,428,429,430,431,432,433,434,435,425,0,480,0,524,525,0,0,0,539,0,125,395,0,0,0,417,552,559,0,0,0,0,270,413,418,0,415,0,0,392,410,411,0,408,256,0,0,0,0,277,0,247,144,149,145,147,140,142,233,0,283,275,284,561,560,0,103,105,0,0,107,123,80,79,0,77,218,182,233,251,178,283,253,190,191,0,102,16,30,44,0,46,0,135,137,258,257,36,38,120,132,0,0,0,151,152,159,0,125,125,171,0,0,0,0,0,552,0,0,0,343,344,345,0,0,347,0,0,0,315,309,136,313,307,305,39,0,188,240,0,0,0,246,234,241,159,0,0,0,235,187,221,220,183,219,0,0,225,75,126,119,443,116,112,0,0,0,0,552,259,264,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,538,545,0,544,421,445,0,447,449,451,453,455,456,460,461,458,459,463,464,466,467,469,470,471,424,423,523,520,0,522,0,0,0,406,407,0,273,0,414,268,271,401,0,269,405,398,0,49,0,399,0,278,0,150,146,148,0,235,0,219,0,285,0,233,0,296,280,0,0,125,0,0,0,143,0,125,0,233,0,200,179,250,0,22,4,6,40,0,155,138,156,0,0,154,235,164,173,172,0,0,167,0,0,0,322,0,0,0,0,0,0,342,346,0,0,0,310,303,0,41,282,233,0,243,0,0,161,236,0,159,239,227,184,206,207,225,203,481,0,0,0,260,0,265,484,0,0,0,0,0,534,489,492,0,0,495,499,0,0,473,541,0,0,521,519,0,0,0,0,272,416,419,409,412,400,279,248,159,0,286,287,219,0,0,235,219,0,0,51,235,552,0,76,78,0,192,0,0,235,0,219,0,0,0,17,23,153,0,157,129,162,174,171,171,0,0,0,0,0,0,0,0,0,0,0,0,0,322,348,0,311,42,235,0,244,242,0,185,0,161,235,0,226,530,0,529,0,261,266,0,0,0,0,0,441,442,523,522,504,0,543,526,0,546,437,540,542,0,396,397,161,159,289,295,159,0,288,219,0,129,0,81,193,199,159,0,198,194,219,0,0,0,0,0,0,0,170,169,316,322,0,0,0,0,0,0,350,351,353,355,357,359,361,363,366,371,374,377,381,383,389,390,0,0,319,328,0,0,0,0,0,0,0,0,0,0,349,229,245,0,237,186,228,233,483,0,0,267,485,486,0,0,491,490,0,510,504,500,502,496,527,0,292,161,161,159,290,53,0,0,161,159,195,31,25,0,0,27,0,7,158,122,0,0,0,322,0,387,0,0,384,322,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,385,322,0,330,0,0,0,338,0,0,0,0,0,0,321,0,0,235,532,531,0,0,0,0,511,510,507,505,0,501,528,291,294,161,121,0,196,161,26,24,28,0,0,19,175,317,318,0,0,0,0,322,324,354,0,356,358,360,362,364,365,369,370,367,368,372,373,375,376,378,379,380,0,323,329,331,332,0,340,339,0,334,0,0,0,160,238,230,0,0,0,0,0,513,512,0,506,503,293,0,197,29,18,20,0,320,386,0,382,325,0,322,333,341,335,336,0,263,262,487,0,509,0,508,82,21,0,352,322,326,337,0,514,388,327,488,0,0,0};
# 1838
static short Cyc_yydefgoto[162U]={1134,53,54,55,56,487,878,1040,790,791,962,669,57,320,58,304,59,489,60,491,61,151,62,63,556,243,473,474,244,66,259,245,68,173,174,69,171,70,281,282,136,137,138,283,246,455,502,503,504,679,823,71,72,684,685,686,73,505,74,479,75,76,168,169,77,121,552,335,722,642,78,643,546,713,538,542,543,449,328,268,102,103,569,494,570,429,430,431,247,321,322,644,461,307,308,309,310,311,312,805,313,314,891,892,893,894,895,896,897,898,899,900,901,902,903,904,905,906,432,441,442,433,434,435,315,207,409,208,561,209,210,211,212,213,214,215,216,217,218,219,220,367,368,845,942,943,1023,944,1025,1092,221,222,830,223,588,589,224,225,80,963,436,465};
# 1858
static short Cyc_yypact[1137U]={3023,- -32768,- -32768,- -32768,- -32768,- 26,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3799,284,74,- -32768,3799,1492,- -32768,38,40,- -32768,110,136,58,157,185,- -32768,- -32768,203,100,264,- -32768,236,- -32768,- -32768,- -32768,227,263,1018,276,299,263,- -32768,- -32768,272,331,352,2880,- -32768,496,570,- -32768,1025,3799,3799,3799,- -32768,3799,- -32768,- -32768,476,- -32768,38,3709,71,199,250,750,- -32768,- -32768,392,407,429,- -32768,38,443,7242,- -32768,- -32768,2862,290,- -32768,- -32768,- -32768,- -32768,440,392,470,7242,- -32768,460,2862,459,484,504,- -32768,286,- -32768,- -32768,4015,4015,428,495,2880,2880,7242,424,- -32768,- 52,531,7242,- -32768,133,537,- 52,4495,- -32768,2880,2880,3165,- -32768,2880,3165,2880,3165,- -32768,553,554,- -32768,3575,- -32768,- -32768,- -32768,- -32768,4495,- -32768,- -32768,392,337,1844,- -32768,3709,1025,581,4015,3886,5338,- -32768,71,- -32768,580,- -32768,- -32768,- -32768,- -32768,596,- -32768,- -32768,159,750,4015,- -32768,- -32768,605,641,612,38,7553,633,7650,7242,7405,661,669,672,679,683,687,697,706,721,729,738,7650,7650,- -32768,- -32768,806,7701,2584,740,7701,7701,- -32768,- -32768,- -32768,360,- -32768,- -32768,45,761,716,732,741,280,132,691,138,72,- -32768,671,7701,91,- 39,- -32768,766,130,- -32768,2862,222,775,1133,786,393,1315,- -32768,- -32768,788,7242,392,1315,768,4234,4495,4582,4495,433,- -32768,12,12,- -32768,793,774,- -32768,- -32768,413,392,- -32768,- -32768,- -32768,- -32768,28,781,779,- -32768,- -32768,287,451,- -32768,- -32768,- -32768,787,- -32768,791,- -32768,792,- -32768,133,5450,3709,581,798,4495,- -32768,728,789,38,800,795,336,802,4651,803,819,808,820,5562,2440,4651,- 9,807,- -32768,- -32768,814,1993,1993,1025,1993,- -32768,- -32768,- -32768,822,- -32768,- -32768,- -32768,233,- -32768,581,818,- -32768,- -32768,810,81,838,- -32768,- 4,823,825,385,826,727,811,7242,4015,- -32768,831,- -32768,- -32768,81,38,- -32768,7242,829,2584,- -32768,4495,2584,- -32768,- -32768,- -32768,4763,- -32768,861,7242,7242,7242,7242,7242,7242,853,7242,4495,932,7242,- -32768,- -32768,- -32768,- -32768,834,- -32768,1993,841,453,7242,- -32768,- -32768,7242,- -32768,7701,7242,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7242,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7242,- -32768,- 52,- -32768,- -32768,5674,- 52,7242,- -32768,837,392,- -32768,847,848,851,- -32768,26,440,- 52,7242,2862,183,- -32768,- -32768,- -32768,857,859,852,2862,- -32768,- -32768,- -32768,854,865,- -32768,467,1133,864,4015,- -32768,858,871,- -32768,4582,4582,4582,- -32768,- -32768,3300,5786,478,- -32768,307,- -32768,- -32768,- 4,- -32768,- -32768,868,890,- -32768,879,- -32768,875,877,884,- -32768,- -32768,1458,- -32768,308,261,- -32768,- -32768,- -32768,4495,- -32768,- -32768,966,- -32768,2880,- -32768,2880,- -32768,- -32768,- -32768,- -32768,581,- -32768,- -32768,- -32768,849,7242,889,891,- -32768,19,232,392,392,817,7242,7242,888,898,7242,880,996,2291,902,- -32768,- -32768,- -32768,475,983,- -32768,5898,2142,2728,- -32768,- -32768,3575,- -32768,- -32768,- -32768,- -32768,4015,- -32768,- -32768,4495,897,4321,- -32768,- -32768,894,951,- 4,900,4408,825,- -32768,- -32768,- -32768,- -32768,727,904,208,636,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,905,912,909,920,914,- -32768,- -32768,709,5450,918,926,927,928,929,516,924,935,936,239,937,946,934,7502,- -32768,- -32768,938,968,- -32768,761,152,716,732,741,280,132,132,691,691,691,691,138,138,72,72,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,940,- -32768,68,4015,5223,4495,- -32768,4495,- -32768,957,- -32768,- -32768,- -32768,- -32768,1265,- -32768,- -32768,- -32768,1440,- -32768,939,- -32768,317,- -32768,4495,- -32768,- -32768,- -32768,964,825,965,727,960,307,4015,4105,6010,- -32768,- -32768,7242,979,392,7354,971,28,3435,972,392,4015,3886,6122,- -32768,308,- -32768,985,1061,- -32768,- -32768,- -32768,874,- -32768,- -32768,728,984,7242,- -32768,825,- -32768,- -32768,- -32768,991,38,508,545,546,7242,821,555,4651,986,6234,6346,566,- -32768,- -32768,994,987,988,1993,- -32768,3709,- -32768,810,999,4015,- -32768,998,- 4,1040,- -32768,995,951,102,- -32768,- -32768,- -32768,- -32768,636,- -32768,1000,204,1000,1001,- -32768,4878,- -32768,- -32768,7242,7242,1104,7242,7405,- -32768,- -32768,- -32768,- 52,- 52,- -32768,997,1006,4993,- -32768,- -32768,7242,7242,- -32768,- -32768,81,719,1021,1023,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,951,1011,- -32768,- -32768,727,81,1022,825,727,1024,564,- -32768,825,1034,1029,- -32768,- -32768,1031,- -32768,81,1035,825,1036,727,1028,3165,1045,1124,- -32768,- -32768,7242,- -32768,4495,- -32768,1042,88,817,4651,1047,1044,1207,1043,1055,4651,7242,6458,592,6570,639,6682,821,- -32768,1058,- -32768,- -32768,825,339,- -32768,- -32768,1051,- -32768,1068,1040,825,4495,- -32768,- -32768,342,- -32768,7242,- -32768,- -32768,5450,1056,1057,1059,1065,- -32768,861,1060,1064,4,1066,- -32768,- -32768,725,- -32768,- -32768,- -32768,- -32768,5223,- -32768,- -32768,1040,951,- -32768,- -32768,951,1069,- -32768,727,1070,4495,1076,- -32768,- -32768,- -32768,951,1073,- -32768,- -32768,727,1072,981,1078,2880,1074,1082,4495,- -32768,- -32768,1182,821,1085,7798,1080,2728,7701,1079,- -32768,50,- -32768,1118,1075,710,771,194,790,349,147,- -32768,- -32768,- -32768,- -32768,1121,7701,1993,- -32768,- -32768,588,4651,602,6794,4651,604,6906,7018,662,1093,- -32768,- -32768,- -32768,7242,1094,- -32768,- -32768,999,- -32768,323,362,- -32768,- -32768,- -32768,4495,1200,- -32768,- -32768,1095,120,365,- -32768,- -32768,- -32768,- -32768,5108,- -32768,1040,1040,951,- -32768,- -32768,1102,1106,1040,951,- -32768,- -32768,- -32768,1108,1109,664,424,- -32768,- -32768,- -32768,613,4651,1110,821,2584,- -32768,4495,1105,- -32768,1695,7701,7242,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7242,- -32768,821,1113,- -32768,4651,4651,615,- -32768,4651,4651,616,4651,617,7130,- -32768,1107,- 4,825,- -32768,- -32768,2728,1123,1111,7242,1126,370,- -32768,- -32768,1129,- -32768,- -32768,- -32768,- -32768,1040,- -32768,1134,- -32768,1040,- -32768,- -32768,- -32768,424,1125,668,- -32768,- -32768,- -32768,1128,1127,1135,7701,821,- -32768,761,381,716,732,732,280,132,132,691,691,691,691,138,138,72,72,- -32768,- -32768,- -32768,397,- -32768,- -32768,- -32768,- -32768,4651,- -32768,- -32768,4651,- -32768,4651,4651,625,- -32768,- -32768,- -32768,1137,757,1131,4495,628,- -32768,1139,1129,- -32768,- -32768,- -32768,1136,- -32768,- -32768,- -32768,- -32768,424,- -32768,1000,204,- -32768,- -32768,7242,1695,- -32768,- -32768,- -32768,- -32768,4651,- -32768,- -32768,- -32768,1140,- -32768,1159,- -32768,- -32768,- -32768,422,- -32768,821,- -32768,- -32768,1141,- -32768,- -32768,- -32768,- -32768,1276,1277,- -32768};
# 1975
static short Cyc_yypgoto[162U]={- -32768,119,- -32768,- -32768,- -32768,- -32768,- -32768,176,- -32768,- -32768,242,- -32768,- -32768,- 237,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 67,- 117,- 14,- -32768,- -32768,0,624,- -32768,235,- 182,1161,33,- -32768,- -32768,- 139,- -32768,30,1244,- 748,- -32768,- -32768,- -32768,1009,1007,659,355,- -32768,- -32768,614,- 211,- 231,- -32768,- -32768,92,- -32768,- -32768,121,- 154,1216,- 401,455,- -32768,1138,- -32768,- -32768,1241,- 452,- -32768,572,- 114,- 150,- 143,- 425,282,578,586,- 434,- 502,- 123,- 393,- 135,- -32768,- 270,- 168,- 576,- 247,- -32768,870,- 155,- 24,- 147,- 210,- 306,146,- -32768,- -32768,- 56,- 278,- -32768,- 423,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 23,1062,- -32768,675,860,- -32768,369,567,- -32768,- 170,- 298,- 165,- 363,- 361,- 353,930,- 351,- 345,- 279,- 321,- 317,- 197,695,- -32768,- -32768,- -32768,- -32768,367,- -32768,- 907,292,- -32768,507,954,216,- -32768,- 383,- -32768,- 12,529,- 35,- 61,- 72,- 54};
# 1999
static short Cyc_yytable[7950U]={64,369,267,104,374,375,150,83,493,147,348,87,327,351,105,591,352,153,336,593,329,64,279,338,410,64,528,529,594,531,652,614,596,67,1024,146,460,342,710,597,598,326,752,497,371,471,260,880,122,140,141,142,260,143,67,256,481,64,67,152,257,539,453,64,64,64,227,64,258,603,604,417,150,64,157,605,606,105,235,666,534,146,305,280,316,248,249,450,81,105,67,667,584,682,683,317,67,67,67,677,67,718,418,571,462,666,67,599,600,601,602,714,478,64,64,42,524,954,940,1095,19,20,540,717,525,152,231,64,64,64,623,64,64,64,64,306,762,152,228,64,472,941,701,378,463,707,67,67,978,64,41,64,108,647,667,464,678,229,157,44,67,67,67,- 558,67,67,67,67,848,123,111,560,67,462,647,541,130,285,287,411,412,413,67,139,67,548,1121,379,563,112,564,565,979,766,46,466,376,607,608,609,559,394,154,674,155,420,580,86,750,535,156,440,316,463,537,- 168,105,440,764,105,395,396,105,496,414,386,387,325,105,415,416,483,253,254,940,65,- 233,305,305,- 233,305,109,228,263,452,41,269,270,271,483,272,273,274,275,44,795,65,1022,560,672,65,229,47,388,389,152,48,110,536,445,453,453,453,318,51,52,994,454,42,64,376,392,113,87,558,393,470,986,987,748,306,306,65,306,334,995,996,64,65,65,65,729,65,305,64,64,802,64,65,555,114,41,67,625,541,858,257,829,117,862,44,427,428,660,258,988,989,739,412,413,67,560,48,712,873,557,115,67,67,612,67,421,720,615,41,861,660,118,65,65,865,464,43,44,45,622,306,422,533,871,119,376,65,65,65,500,65,65,65,65,64,571,414,41,65,384,385,740,416,47,794,43,44,45,65,450,65,124,745,120,921,51,52,923,477,541,41,127,318,41,458,928,228,67,267,44,624,43,44,45,84,952,239,704,630,- 255,47,105,- 255,240,477,229,958,636,816,105,458,530,51,52,46,46,125,105,929,257,1016,516,648,662,649,663,661,258,523,483,650,664,41,618,695,761,452,452,452,206,128,44,834,702,464,651,970,150,753,483,754,233,170,511,751,483,512,924,705,286,930,659,129,665,931,835,464,450,255,454,454,454,376,64,376,64,992,1026,377,879,993,571,1093,1018,673,768,941,152,825,780,770,1022,560,376,325,706,152,547,65,782,152,64,1108,780,784,376,67,170,67,376,64,438,785,79,64,159,65,- 15,1109,769,41,680,681,65,65,376,65,172,43,44,45,469,1045,783,67,85,856,250,1050,106,1131,107,251,67,931,541,47,67,818,933,457,840,372,176,841,230,458,826,51,52,1071,79,850,484,714,376,232,834,236,485,79,586,41,721,1085,79,633,234,927,376,43,44,45,145,698,148,144,376,79,65,835,444,670,759,671,457,237,175,252,1051,105,458,106,1053,105,131,132,560,661,948,1107,1013,1054,1055,106,999,1056,798,238,305,799,316,483,1057,1058,483,79,79,376,145,949,651,817,950,735,79,831,16,17,18,79,79,79,956,79,79,79,79,261,522,665,842,843,1063,1064,264,703,376,376,1065,1066,834,276,800,801,79,277,376,819,775,1127,262,306,806,152,812,376,976,376,133,134,851,864,1049,835,459,64,1132,64,175,319,1059,1060,1061,1062,998,372,332,859,372,376,915,1029,1030,376,480,1000,331,65,1034,65,577,869,333,376,397,376,339,975,67,1002,67,1006,506,1031,484,341,376,376,376,1035,1042,592,1075,1078,1080,65,376,721,560,376,106,968,1114,106,65,1119,106,918,65,345,376,340,106,398,399,400,401,402,403,404,405,406,407,907,1019,390,391,266,616,1015,1011,64,1038,376,105,1039,1101,353,482,1102,1067,1068,1069,1096,408,354,284,1098,355,79,639,640,641,550,551,356,482,266,508,357,1046,513,1047,358,67,682,683,366,160,161,162,163,164,359,1126,165,166,167,79,727,728,703,360,41,146,305,803,804,495,852,853,43,44,45,1106,946,947,361,982,983,47,380,1017,381,500,362,1086,541,501,984,985,175,51,52,363,344,373,347,349,349,382,64,687,688,990,991,691,1116,376,696,383,364,365,882,883,419,349,423,306,349,349,437,554,443,447,451,1041,456,467,468,875,475,64,305,67,459,476,486,507,349,495,488,490,572,573,574,575,576,498,509,579,510,146,582,518,480,519,1118,514,517,65,1125,65,587,284,67,590,526,520,884,527,278,532,537,553,911,464,46,506,562,544,106,378,549,41,545,306,610,578,106,583,617,43,44,45,46,88,106,585,611,64,47,619,620,587,500,621,627,637,41,628,631,629,51,52,347,632,43,44,45,965,635,638,188,653,654,47,146,655,482,500,656,482,67,657,658,668,675,51,52,692,79,676,79,689,773,65,690,646,693,697,699,482,708,677,726,831,715,482,349,711,719,723,724,1041,305,725,41,91,195,196,92,93,94,- 558,730,44,731,732,733,734,736,1001,742,743,1005,809,811,346,760,146,200,737,738,741,746,349,749,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,587,41,587,306,747,755,763,765,767,43,44,45,774,778,781,789,960,64,961,788,814,65,506,1043,266,793,796,807,813,822,815,349,325,820,744,824,506,838,41,506,832,854,844,855,266,41,495,44,846,46,67,266,65,857,44,47,1073,1074,135,48,1076,1077,47,1079,860,106,48,51,52,106,866,867,863,868,51,52,874,870,872,876,877,885,88,881,912,914,886,917,909,920,910,922,925,495,266,926,482,955,934,935,936,937,938,953,349,266,939,959,945,932,482,951,964,482,266,957,966,967,969,971,65,974,797,980,977,772,997,981,1012,1110,1014,777,1111,1020,1112,1113,1021,1032,89,787,887,888,1033,1036,1037,1044,1072,1048,1091,1083,424,940,1089,425,91,1097,88,92,93,94,1100,426,44,1103,587,1088,233,96,1128,1104,97,1105,1115,1117,98,1120,99,100,1122,427,428,1130,1129,1133,1135,1136,1123,101,349,1099,779,116,1004,265,492,1008,1010,499,792,158,126,828,827,495,1084,821,836,837,626,839,446,757,89,634,88,337,266,1027,908,495,595,0,849,1094,581,79,451,226,91,1124,266,92,93,94,0,95,44,0,0,0,106,96,0,0,97,0,0,0,889,372,99,100,65,0,0,0,1052,0,890,0,0,101,203,0,88,204,205,0,0,0,89,0,0,0,1070,0,0,0,0,0,0,349,756,0,0,425,91,0,1082,92,93,94,0,426,44,1087,0,0,1090,96,0,0,97,0,0,0,98,0,99,100,495,427,428,145,0,79,0,0,0,101,89,0,0,0,0,0,0,0,495,0,0,0,439,0,0,226,91,0,349,92,93,94,0,95,44,0,0,0,0,96,0,349,97,0,0,0,98,0,99,100,0,0,284,0,0,0,0,0,0,101,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,0,0,0,0,266,0,349,0,0,0,0,145,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,495,0,0,0,0,31,32,0,0,0,284,0,0,0,0,0,0,38,0,88,0,0,0,89,0,0,266,0,0,39,0,40,0,0,0,758,0,0,226,91,0,0,92,93,94,0,95,44,0,0,0,0,96,323,145,97,41,0,0,98,0,99,100,42,43,44,45,46,973,325,0,349,101,47,0,89,0,477,0,49,50,0,0,458,0,51,52,0,0,349,90,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,349,97,0,0,0,98,0,99,100,0,0,145,0,0,0,0,0,0,101,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,803,804,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,349,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,349,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,302,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,297,88,26,180,0,0,0,0,0,181,27,0,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,694,0,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,297,88,0,0,0,0,0,0,0,0,27,0,0,0,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,0,0,0,198,0,0,0,346,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,370,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,0,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,89,0,0,0,0,0,39,0,40,0,0,0,0,0,0,226,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,97,41,0,0,98,0,99,100,42,43,44,45,46,0,0,0,- 12,101,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,47,0,0,0,457,0,49,50,0,0,458,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,500,0,49,50,0,0,0,0,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,278,0,0,0,0,49,50,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,324,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,241,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,242,22,23,24,0,0,0,0,0,49,50,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,448,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,709,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,716,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,177,178,288,0,289,290,291,292,293,294,295,296,0,0,0,179,88,0,180,298,0,0,0,0,181,42,299,0,0,46,182,183,184,185,186,0,0,0,0,187,0,49,50,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,91,195,196,92,93,94,0,0,44,0,0,197,301,149,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,566,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,568,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,833,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,847,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,1028,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,0,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,330,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,350,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,521,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,613,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,645,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,700,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,771,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,786,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,808,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,810,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,913,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,916,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,919,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1003,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1007,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1009,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1081,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,776,91,195,196,92,93,94,188,0,44,189,190,197,191,0,0,0,198,0,0,0,199,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,350,0,0,198,0,0,0,199,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,195,196,92,93,94,188,0,44,189,190,197,191,744,0,0,198,0,0,0,199,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,0,0,0,198,0,0,0,343,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,195,196,92,93,94,188,0,44,189,190,197,191,0,0,0,198,0,0,0,346,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,0,0,0,198,0,0,0,199,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,972,0,0,200,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205};
# 2797
static short Cyc_yycheck[7950U]={0,198,125,26,201,202,73,21,278,70,180,25,155,181,26,378,181,73,168,380,155,21,139,170,221,25,304,305,381,307,464,414,383,0,941,70,246,176,540,384,385,155,618,280,199,17,118,795,48,63,64,65,124,67,21,116,266,57,25,73,112,65,244,63,64,65,89,67,120,390,391,110,139,73,74,392,393,89,101,480,317,116,149,139,151,109,110,242,114,101,57,484,370,5,6,151,63,64,65,80,67,553,141,350,92,506,73,386,387,388,389,545,266,113,114,119,125,865,114,1026,20,21,126,548,133,139,96,127,128,129,428,131,132,133,134,149,638,151,112,139,112,137,525,98,132,538,113,114,98,149,112,151,112,459,547,143,137,131,158,121,127,128,129,137,131,132,133,134,744,48,112,341,139,92,480,325,57,147,148,88,89,90,149,62,151,328,1093,142,343,131,345,346,142,645,123,249,128,394,395,396,339,129,131,501,133,228,361,133,140,128,139,234,279,132,112,127,228,240,643,231,148,149,234,279,133,93,94,125,240,138,139,266,113,114,114,0,134,304,305,137,307,131,112,122,244,112,127,128,129,284,131,132,133,134,121,680,21,137,428,496,25,131,129,131,132,279,133,131,322,239,452,453,454,152,141,142,129,244,119,279,128,143,125,297,338,147,256,93,94,137,304,305,57,307,135,148,149,297,63,64,65,571,67,370,304,305,689,307,73,333,125,112,279,130,464,767,112,113,54,771,121,138,139,477,120,131,132,88,89,90,297,501,133,544,786,335,133,304,305,411,307,119,134,415,112,770,500,111,113,114,775,143,120,121,122,427,370,135,125,784,133,128,127,128,129,133,131,132,133,134,370,618,133,112,139,95,96,138,139,129,678,120,121,122,149,540,151,111,585,126,813,141,142,818,133,545,112,125,277,112,139,826,112,370,527,121,429,120,121,122,126,863,126,530,437,125,129,429,128,133,133,131,874,447,702,437,139,306,141,142,123,123,133,445,827,112,113,291,131,131,133,133,477,120,298,480,139,139,112,419,517,134,452,453,454,86,125,121,728,526,143,461,885,530,619,500,621,98,131,133,617,506,136,134,530,138,134,477,126,479,138,728,143,638,115,452,453,454,128,489,128,491,143,128,134,793,147,744,128,137,500,648,137,517,715,659,649,137,678,128,125,535,526,128,279,662,530,517,137,673,663,128,489,131,491,128,526,134,663,0,530,76,297,126,137,649,112,507,508,304,305,128,307,114,120,121,122,134,971,663,517,22,763,125,977,26,134,28,130,526,138,711,129,530,707,835,133,735,199,126,735,131,139,716,141,142,999,48,748,128,1014,128,112,853,125,134,57,134,112,556,1015,62,125,133,825,128,120,121,122,70,125,72,126,128,75,370,853,238,489,632,491,133,128,84,119,978,628,139,89,980,632,125,126,793,659,856,1049,925,981,982,101,909,983,125,130,702,128,704,673,984,985,676,113,114,128,116,857,647,704,860,134,122,724,17,18,19,127,128,129,870,131,132,133,134,133,296,666,739,740,990,991,134,526,128,128,992,993,947,125,134,134,152,128,128,708,655,1109,120,702,134,704,125,128,890,128,125,126,751,134,977,947,246,702,1126,704,176,125,986,987,988,989,908,343,133,768,346,128,125,949,950,128,266,134,156,489,956,491,358,782,133,128,60,128,128,889,702,134,704,134,284,951,128,130,128,128,128,957,134,379,134,134,134,517,128,721,925,128,228,881,134,231,526,134,234,125,530,133,128,127,240,99,100,101,102,103,104,105,106,107,108,803,936,91,92,125,416,929,125,788,125,128,803,128,125,133,266,128,994,995,996,1031,130,133,144,1035,133,277,452,453,454,83,84,133,284,155,286,133,972,289,974,133,788,5,6,18,75,76,77,78,79,133,1109,82,83,84,306,127,128,692,133,112,876,909,22,23,278,127,128,120,121,122,1048,127,128,133,145,146,129,97,931,144,133,133,1018,1014,137,95,96,339,141,142,133,177,133,179,180,181,145,878,510,511,91,92,514,127,128,517,146,193,194,798,799,126,198,119,909,201,202,112,332,112,133,243,964,245,112,132,788,127,909,977,878,457,134,127,126,221,350,127,127,353,354,355,356,357,127,126,360,133,964,363,112,477,125,1089,133,133,702,1108,704,373,282,909,376,137,125,800,133,130,127,112,140,806,143,123,500,127,134,429,98,134,112,137,977,397,112,437,133,131,120,121,122,123,41,445,134,409,977,129,132,132,414,133,132,127,127,112,128,134,137,141,142,297,128,120,121,122,878,134,128,68,133,112,129,1039,126,477,133,133,480,977,134,128,47,125,141,142,137,489,128,491,133,653,788,126,458,30,125,45,500,133,80,112,1105,134,506,341,143,134,134,128,1102,1109,134,112,113,114,115,116,117,118,137,134,121,128,128,128,128,134,913,114,127,916,694,695,133,127,1102,136,134,134,134,134,378,134,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,525,112,527,1109,128,140,134,134,140,120,121,122,125,134,134,46,127,1109,129,126,125,878,659,969,457,133,127,133,126,81,134,428,125,127,126,132,673,25,112,676,131,112,137,112,477,112,571,121,134,123,1109,484,909,134,121,129,1002,1003,125,133,1006,1007,129,1009,134,628,133,141,142,632,128,134,140,134,141,142,140,134,134,126,48,126,41,133,807,808,134,810,137,812,127,125,133,618,527,119,659,113,134,134,133,128,134,125,501,538,134,127,134,832,673,134,126,676,547,134,134,127,28,126,977,133,685,97,137,650,97,144,127,1075,128,656,1078,25,1080,1081,133,127,97,664,25,26,128,127,127,127,125,134,114,134,109,114,133,112,113,113,41,116,117,118,127,120,121,127,689,134,889,126,1114,134,129,128,127,134,133,128,135,136,134,138,139,114,134,134,0,0,1102,146,585,1039,658,39,915,124,277,918,919,282,676,75,51,721,716,728,1014,711,731,732,430,734,240,628,97,445,41,169,649,942,803,744,382,- 1,747,1023,362,788,659,112,113,1105,663,116,117,118,- 1,120,121,- 1,- 1,- 1,803,126,- 1,- 1,129,- 1,- 1,- 1,133,972,135,136,1109,- 1,- 1,- 1,979,- 1,143,- 1,- 1,146,147,- 1,41,150,151,- 1,- 1,- 1,97,- 1,- 1,- 1,997,- 1,- 1,- 1,- 1,- 1,- 1,678,109,- 1,- 1,112,113,- 1,1011,116,117,118,- 1,120,121,1018,- 1,- 1,1021,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,835,138,139,876,- 1,878,- 1,- 1,- 1,146,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,853,- 1,- 1,- 1,109,- 1,- 1,112,113,- 1,735,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,748,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,795,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,- 1,- 1,- 1,- 1,827,- 1,793,- 1,- 1,- 1,- 1,964,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,947,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,865,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,41,- 1,- 1,- 1,97,- 1,- 1,881,- 1,- 1,85,- 1,87,- 1,- 1,- 1,109,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,109,1039,129,112,- 1,- 1,133,- 1,135,136,119,120,121,122,123,887,125,- 1,890,146,129,- 1,97,- 1,133,- 1,135,136,- 1,- 1,139,- 1,141,142,- 1,- 1,908,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,925,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,1102,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,978,- 1,980,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,1048,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1108,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,112,- 1,- 1,133,- 1,135,136,119,120,121,122,123,- 1,- 1,- 1,127,146,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,127,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,139,- 1,141,142,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,37,38,39,40,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,37,38,39,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,119,51,- 1,- 1,123,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,135,136,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,32,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp41C[8U]="stdcall";static char _tmp41D[6U]="cdecl";static char _tmp41E[9U]="fastcall";static char _tmp41F[9U]="noreturn";static char _tmp420[6U]="const";static char _tmp421[8U]="aligned";static char _tmp422[7U]="packed";static char _tmp423[7U]="shared";static char _tmp424[7U]="unused";static char _tmp425[5U]="weak";static char _tmp426[10U]="dllimport";static char _tmp427[10U]="dllexport";static char _tmp428[23U]="no_instrument_function";static char _tmp429[12U]="constructor";static char _tmp42A[11U]="destructor";static char _tmp42B[22U]="no_check_memory_usage";static char _tmp42C[5U]="pure";static char _tmp42D[14U]="always_inline";static char _tmp42E[9U]="no_throw";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp392=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp392;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp761;(_tmp761.YYINITIALSVAL).tag=72U,(_tmp761.YYINITIALSVAL).val=0;_tmp761;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp6A5=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp6A5),sizeof(short),_tmp6A5);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp6A4=200U;struct Cyc_Yystacktype*_tmp6A3=({struct _RegionHandle*_tmp81A=yyregion;_region_malloc(_tmp81A,_check_times(_tmp6A4,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp760=200U;unsigned i;for(i=0;i < _tmp760;++ i){(_tmp6A3[i]).v=yylval,({struct Cyc_Yyltype _tmp81B=Cyc_yynewloc();(_tmp6A3[i]).l=_tmp81B;});}}0;});_tmp6A3;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp81D=({const char*_tmp393="parser stack overflow";_tag_fat(_tmp393,sizeof(char),22U);});int _tmp81C=yystate;Cyc_yyerror(_tmp81D,_tmp81C,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp397=(unsigned)yystacksize;short*_tmp396=({struct _RegionHandle*_tmp81E=yyregion;_region_malloc(_tmp81E,_check_times(_tmp397,sizeof(short)));});({{unsigned _tmp717=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp717;++ i){
i <= (unsigned)yyssp_offset?_tmp396[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp396[i]=0);}}0;});_tag_fat(_tmp396,sizeof(short),_tmp397);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp395=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp394=({struct _RegionHandle*_tmp81F=yyregion;_region_malloc(_tmp81F,_check_times(_tmp395,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp716=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp716;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp394[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp394[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp394,sizeof(struct Cyc_Yystacktype),_tmp395);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1137U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 379?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,380U,sizeof(short),yychar)): 314;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7949)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),yyn))!= yychar1)goto yydefault;
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
if(yyn == 1136){
int _tmp398=0;_npop_handler(0U);return _tmp398;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp820=({struct Cyc_Yystacktype _tmp718;_tmp718.v=yylval,_tmp718.l=yylloc;_tmp718;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp820;});
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
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,562U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp399=yyn;switch(_tmp399){case 1U: _LL1: _LL2:
# 1141 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1144
goto _LL0;case 2U: _LL3: _LL4:
# 1147 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp821=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp821,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1151 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));({struct Cyc_Absyn_Decl*_tmp825=({struct Cyc_Absyn_Decl*_tmp39B=_cycalloc(sizeof(*_tmp39B));({void*_tmp824=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->tag=10U,({struct _tuple0*_tmp823=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39A->f1=_tmp823;}),({struct Cyc_List_List*_tmp822=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39A->f2=_tmp822;});_tmp39A;});_tmp39B->r=_tmp824;}),_tmp39B->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39B;});_tmp39C->hd=_tmp825;}),_tmp39C->tl=0;_tmp39C;}));
Cyc_Lex_leave_using();
# 1154
goto _LL0;case 4U: _LL7: _LL8:
# 1155 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct Cyc_Absyn_Decl*_tmp82A=({struct Cyc_Absyn_Decl*_tmp39E=_cycalloc(sizeof(*_tmp39E));({void*_tmp829=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->tag=10U,({struct _tuple0*_tmp828=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39D->f1=_tmp828;}),({struct Cyc_List_List*_tmp827=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39D->f2=_tmp827;});_tmp39D;});_tmp39E->r=_tmp829;}),_tmp39E->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39E;});_tmp39F->hd=_tmp82A;}),({struct Cyc_List_List*_tmp826=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp39F->tl=_tmp826;});_tmp39F;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1158
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_Absyn_Decl*_tmp82F=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({void*_tmp82E=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->tag=9U,({struct _fat_ptr*_tmp82D=({struct _fat_ptr*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct _fat_ptr _tmp82C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A0=_tmp82C;});_tmp3A0;});_tmp3A1->f1=_tmp82D;}),({struct Cyc_List_List*_tmp82B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A1->f2=_tmp82B;});_tmp3A1;});_tmp3A2->r=_tmp82E;}),_tmp3A2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A2;});_tmp3A3->hd=_tmp82F;}),_tmp3A3->tl=0;_tmp3A3;}));
Cyc_Lex_leave_namespace();
# 1161
goto _LL0;case 6U: _LLB: _LLC:
# 1162 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct Cyc_Absyn_Decl*_tmp835=({struct Cyc_Absyn_Decl*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({void*_tmp834=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=9U,({struct _fat_ptr*_tmp833=({struct _fat_ptr*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct _fat_ptr _tmp832=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A4=_tmp832;});_tmp3A4;});_tmp3A5->f1=_tmp833;}),({struct Cyc_List_List*_tmp831=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A5->f2=_tmp831;});_tmp3A5;});_tmp3A6->r=_tmp834;}),_tmp3A6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A6;});_tmp3A7->hd=_tmp835;}),({struct Cyc_List_List*_tmp830=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A7->tl=_tmp830;});_tmp3A7;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1164 "parse.y"
int _tmp3A8=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3A8;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3A9=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp15=_tmp3A9;struct _tuple28*_tmp3AA=_stmttmp15;unsigned _tmp3AC;struct Cyc_List_List*_tmp3AB;_LL462: _tmp3AB=_tmp3AA->f1;_tmp3AC=_tmp3AA->f2;_LL463: {struct Cyc_List_List*exs=_tmp3AB;unsigned wc=_tmp3AC;
struct Cyc_List_List*_tmp3AD=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3AD;
if(exs != 0 && hides != 0)
({void*_tmp3AE=0U;({unsigned _tmp837=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp836=({const char*_tmp3AF="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3AF,sizeof(char),65U);});Cyc_Warn_err(_tmp837,_tmp836,_tag_fat(_tmp3AE,sizeof(void*),0U));});});
# 1171
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1174
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3B0=0U;({unsigned _tmp839=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp838=({const char*_tmp3B1="expecting \"C include\"";_tag_fat(_tmp3B1,sizeof(char),22U);});Cyc_Warn_err(_tmp839,_tmp838,_tag_fat(_tmp3B0,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));({struct Cyc_Absyn_Decl*_tmp83E=({struct Cyc_Absyn_Decl*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));({void*_tmp83D=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->tag=12U,({struct Cyc_List_List*_tmp83C=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B3->f1=_tmp83C;}),_tmp3B3->f2=cycdecls,_tmp3B3->f3=exs,({struct _tuple10*_tmp83B=({struct _tuple10*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->f1=wc,_tmp3B2->f2=hides;_tmp3B2;});_tmp3B3->f4=_tmp83B;});_tmp3B3;});_tmp3B4->r=_tmp83D;}),_tmp3B4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B4;});_tmp3B5->hd=_tmp83E;}),({struct Cyc_List_List*_tmp83A=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B5->tl=_tmp83A;});_tmp3B5;}));}else{
# 1180
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct Cyc_Absyn_Decl*_tmp842=({struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({void*_tmp841=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=11U,({struct Cyc_List_List*_tmp840=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B6->f1=_tmp840;});_tmp3B6;});_tmp3B7->r=_tmp841;}),_tmp3B7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B7;});_tmp3B8->hd=_tmp842;}),({struct Cyc_List_List*_tmp83F=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B8->tl=_tmp83F;});_tmp3B8;}));}}else{
# 1184
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct Cyc_Absyn_Decl*_tmp847=({struct Cyc_Absyn_Decl*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({void*_tmp846=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->tag=12U,({struct Cyc_List_List*_tmp845=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BA->f1=_tmp845;}),_tmp3BA->f2=cycdecls,_tmp3BA->f3=exs,({struct _tuple10*_tmp844=({struct _tuple10*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->f1=wc,_tmp3B9->f2=hides;_tmp3B9;});_tmp3BA->f4=_tmp844;});_tmp3BA;});_tmp3BB->r=_tmp846;}),_tmp3BB->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BB;});_tmp3BC->hd=_tmp847;}),({struct Cyc_List_List*_tmp843=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BC->tl=_tmp843;});_tmp3BC;}));}
# 1187
goto _LL0;}}case 8U: _LLF: _LL10:
# 1188 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({struct Cyc_Absyn_Decl*_tmp849=({struct Cyc_Absyn_Decl*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3BD->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BD;});_tmp3BE->hd=_tmp849;}),({struct Cyc_List_List*_tmp848=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BE->tl=_tmp848;});_tmp3BE;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1190 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_Absyn_Decl*_tmp84B=({struct Cyc_Absyn_Decl*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3BF->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BF;});_tmp3C0->hd=_tmp84B;}),({struct Cyc_List_List*_tmp84A=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C0->tl=_tmp84A;});_tmp3C0;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1192 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Decl*_tmp84D=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C1;});_tmp3C2->hd=_tmp84D;}),({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C2->tl=_tmp84C;});_tmp3C2;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1194 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct Cyc_Absyn_Decl*_tmp84F=({struct Cyc_Absyn_Decl*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3C3->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C3;});_tmp3C4->hd=_tmp84F;}),({struct Cyc_List_List*_tmp84E=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C4->tl=_tmp84E;});_tmp3C4;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1195 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1200 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1205 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1210 "parse.y"
struct _fat_ptr _tmp3C5=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3C5;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp850=(struct _fat_ptr)two;Cyc_strcmp(_tmp850,({const char*_tmp3C6="C";_tag_fat(_tmp3C6,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp851=(struct _fat_ptr)two;Cyc_strcmp(_tmp851,({const char*_tmp3C7="C include";_tag_fat(_tmp3C7,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1217
({void*_tmp3C8=0U;({unsigned _tmp853=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp852=({const char*_tmp3C9="expecting \"C\" or \"C include\"";_tag_fat(_tmp3C9,sizeof(char),29U);});Cyc_Warn_err(_tmp853,_tmp852,_tag_fat(_tmp3C8,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1221
goto _LL0;}case 16U: _LL1F: _LL20:
# 1224 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1228 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1229 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1233 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct _tuple0*_tmp854=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CA->hd=_tmp854;}),_tmp3CA->tl=0;_tmp3CA;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1234 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));({struct _tuple0*_tmp855=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CB->hd=_tmp855;}),_tmp3CB->tl=0;_tmp3CB;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1236 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));({struct _tuple0*_tmp857=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CC->hd=_tmp857;}),({struct Cyc_List_List*_tmp856=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3CC->tl=_tmp856;});_tmp3CC;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1240 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->f1=0,_tmp3CD->f2=0U;_tmp3CD;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1241 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1245 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct Cyc_List_List*_tmp858=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3CE->f1=_tmp858;}),_tmp3CE->f2=0U;_tmp3CE;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1246 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->f1=0,_tmp3CF->f2=0U;_tmp3CF;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1247 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->f1=0,_tmp3D0->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D0;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1251 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct _tuple32*_tmp85A=({struct _tuple32*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp859=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D1->f2=_tmp859;}),_tmp3D1->f3=0;_tmp3D1;});_tmp3D2->hd=_tmp85A;}),_tmp3D2->tl=0;_tmp3D2;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1252 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _tuple32*_tmp85C=({struct _tuple32*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D3->f2=_tmp85B;}),_tmp3D3->f3=0;_tmp3D3;});_tmp3D4->hd=_tmp85C;}),_tmp3D4->tl=0;_tmp3D4;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1254 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _tuple32*_tmp85F=({struct _tuple32*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D5->f2=_tmp85E;}),_tmp3D5->f3=0;_tmp3D5;});_tmp3D6->hd=_tmp85F;}),({struct Cyc_List_List*_tmp85D=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D6->tl=_tmp85D;});_tmp3D6;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1258 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1259 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1263 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));({struct Cyc_Absyn_Decl*_tmp862=({void*_tmp861=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp860=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3D7->f1=_tmp860;});_tmp3D7;});Cyc_Absyn_new_decl(_tmp861,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3D8->hd=_tmp862;}),_tmp3D8->tl=0;_tmp3D8;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1264 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1265 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1274 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp865=yyr;struct Cyc_Parse_Declarator _tmp864=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp863=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp865,0,_tmp864,0,_tmp863,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1276 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D9=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3D9;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp868=yyr;struct Cyc_Parse_Declarator _tmp867=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp866=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp868,& d,_tmp867,0,_tmp866,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1289 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp86C=yyr;struct Cyc_Parse_Declarator _tmp86B=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp86A=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp869=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp86C,0,_tmp86B,_tmp86A,_tmp869,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1291 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DA=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DA;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp870=yyr;struct Cyc_Parse_Declarator _tmp86F=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp86E=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp86D=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp870,& d,_tmp86F,_tmp86E,_tmp86D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1299 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DB=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DB;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp873=yyr;struct Cyc_Parse_Declarator _tmp872=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp871=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp873,& d,_tmp872,0,_tmp871,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1302 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DC;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp877=yyr;struct Cyc_Parse_Declarator _tmp876=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp875=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp874=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp877,& d,_tmp876,_tmp875,_tmp874,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1307 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1310
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1313
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _fat_ptr _tmp878=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3DD=_tmp878;});_tmp3DD;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1316
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1322 "parse.y"
int _tmp3DE=((yyyvsp[0]).l).first_line;int location=_tmp3DE;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87A=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp879=(unsigned)location;Cyc_Parse_make_declarations(_tmp87A,0,_tmp879,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1325 "parse.y"
int _tmp3DF=((yyyvsp[0]).l).first_line;int location=_tmp3DF;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87D=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp87C=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp87B=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp87D,_tmp87C,_tmp87B,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1329
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct Cyc_Absyn_Decl*_tmp880=({struct Cyc_Absyn_Pat*_tmp87F=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp87E=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp87F,_tmp87E,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E0->hd=_tmp880;}),_tmp3E0->tl=0;_tmp3E0;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1331 "parse.y"
struct Cyc_List_List*_tmp3E1=0;struct Cyc_List_List*vds=_tmp3E1;
{struct Cyc_List_List*_tmp3E2=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3E2;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3E3=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3E3;
struct _tuple0*qv=({struct _tuple0*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({union Cyc_Absyn_Nmspace _tmp881=Cyc_Absyn_Rel_n(0);_tmp3E6->f1=_tmp881;}),_tmp3E6->f2=id;_tmp3E6;});
struct Cyc_Absyn_Vardecl*_tmp3E4=({struct _tuple0*_tmp882=qv;Cyc_Absyn_new_vardecl(0U,_tmp882,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3E4;
vds=({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->hd=vd,_tmp3E5->tl=vds;_tmp3E5;});}}
# 1338
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({struct Cyc_Absyn_Decl*_tmp883=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3E7->hd=_tmp883;}),_tmp3E7->tl=0;_tmp3E7;}));
# 1341
goto _LL0;}case 51U: _LL61: _LL62: {
# 1344 "parse.y"
struct _fat_ptr _tmp3E8=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3E8;
Cyc_Parse_tvar_ok(three,(unsigned)((yyyvsp[2]).l).first_line);{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));({struct _fat_ptr*_tmp885=({struct _fat_ptr*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));*_tmp3EC=three;_tmp3EC;});_tmp3ED->name=_tmp885;}),_tmp3ED->identity=- 1,({void*_tmp884=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3ED->kind=_tmp884;});_tmp3ED;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp889=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp888=({struct _tuple0*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp887=({struct _fat_ptr*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({struct _fat_ptr _tmp886=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3EA=_tmp886;});_tmp3EA;});_tmp3EB->f2=_tmp887;});_tmp3EB;});Cyc_Absyn_new_vardecl(_tmp889,_tmp888,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct Cyc_Absyn_Decl*_tmp88A=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3E9->hd=_tmp88A;}),_tmp3E9->tl=0;_tmp3E9;}));
# 1351
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1353
struct _fat_ptr _tmp3EE=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3EE;
if(({struct _fat_ptr _tmp88B=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp88B,({const char*_tmp3EF="H";_tag_fat(_tmp3EF,sizeof(char),2U);}));})== 0)
({void*_tmp3F0=0U;({unsigned _tmp88D=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp88C=({const char*_tmp3F1="bad occurrence of heap region `H";_tag_fat(_tmp3F1,sizeof(char),33U);});Cyc_Warn_err(_tmp88D,_tmp88C,_tag_fat(_tmp3F0,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp88E=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp88E,({const char*_tmp3F2="U";_tag_fat(_tmp3F2,sizeof(char),2U);}));})== 0)
({void*_tmp3F3=0U;({unsigned _tmp890=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp88F=({const char*_tmp3F4="bad occurrence of unique region `U";_tag_fat(_tmp3F4,sizeof(char),35U);});Cyc_Warn_err(_tmp890,_tmp88F,_tag_fat(_tmp3F3,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));({struct _fat_ptr*_tmp894=({struct _fat_ptr*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));({struct _fat_ptr _tmp893=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FA=({struct Cyc_String_pa_PrintArg_struct _tmp719;_tmp719.tag=0U,_tmp719.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp719;});void*_tmp3F8[1U];_tmp3F8[0]=& _tmp3FA;({struct _fat_ptr _tmp892=({const char*_tmp3F9="`%s";_tag_fat(_tmp3F9,sizeof(char),4U);});Cyc_aprintf(_tmp892,_tag_fat(_tmp3F8,sizeof(void*),1U));});});*_tmp3FB=_tmp893;});_tmp3FB;});_tmp3FC->name=_tmp894;}),_tmp3FC->identity=- 1,({
void*_tmp891=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3FC->kind=_tmp891;});_tmp3FC;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp897=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp896=({struct _tuple0*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp895=({struct _fat_ptr*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));*_tmp3F6=two;_tmp3F6;});_tmp3F7->f2=_tmp895;});_tmp3F7;});Cyc_Absyn_new_vardecl(_tmp897,_tmp896,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));({struct Cyc_Absyn_Decl*_tmp898=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F5->hd=_tmp898;}),_tmp3F5->tl=0;_tmp3F5;}));
# 1364
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1366
struct _fat_ptr _tmp3FD=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3FD;
struct _fat_ptr _tmp3FE=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp3FE;
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp3FF;
if(({struct _fat_ptr _tmp899=(struct _fat_ptr)four;Cyc_strcmp(_tmp899,({const char*_tmp400="open";_tag_fat(_tmp400,sizeof(char),5U);}));})!= 0)({void*_tmp401=0U;({unsigned _tmp89B=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp89A=({const char*_tmp402="expecting `open'";_tag_fat(_tmp402,sizeof(char),17U);});Cyc_Warn_err(_tmp89B,_tmp89A,_tag_fat(_tmp401,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp40A=_cycalloc(sizeof(*_tmp40A));({struct _fat_ptr*_tmp89F=({struct _fat_ptr*_tmp409=_cycalloc(sizeof(*_tmp409));({struct _fat_ptr _tmp89E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp408=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,_tmp71A.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp71A;});void*_tmp406[1U];_tmp406[0]=& _tmp408;({struct _fat_ptr _tmp89D=({const char*_tmp407="`%s";_tag_fat(_tmp407,sizeof(char),4U);});Cyc_aprintf(_tmp89D,_tag_fat(_tmp406,sizeof(void*),1U));});});*_tmp409=_tmp89E;});_tmp409;});_tmp40A->name=_tmp89F;}),_tmp40A->identity=- 1,({
void*_tmp89C=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp40A->kind=_tmp89C;});_tmp40A;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A2=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8A1=({struct _tuple0*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A0=({struct _fat_ptr*_tmp404=_cycalloc(sizeof(*_tmp404));*_tmp404=two;_tmp404;});_tmp405->f2=_tmp8A0;});_tmp405;});Cyc_Absyn_new_vardecl(_tmp8A2,_tmp8A1,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp403=_cycalloc(sizeof(*_tmp403));({struct Cyc_Absyn_Decl*_tmp8A3=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp403->hd=_tmp8A3;}),_tmp403->tl=0;_tmp403;}));
# 1376
goto _LL0;}}case 54U: _LL67: _LL68:
# 1380 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55U: _LL69: _LL6A:
# 1382 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8A4=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8A4,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56U: _LL6B: _LL6C:
# 1388 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71B;({enum Cyc_Parse_Storage_class _tmp8A7=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp71B.sc=_tmp8A7;}),({struct Cyc_Absyn_Tqual _tmp8A6=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp71B.tq=_tmp8A6;}),({
struct Cyc_Parse_Type_specifier _tmp8A5=Cyc_Parse_empty_spec(0U);_tmp71B.type_specs=_tmp8A5;}),_tmp71B.is_inline=0,_tmp71B.attributes=0;_tmp71B;}));
goto _LL0;case 57U: _LL6D: _LL6E: {
# 1391 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp40B=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp40B;
if((int)two.sc != (int)Cyc_Parse_None_sc)
({void*_tmp40C=0U;({unsigned _tmp8A9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8A8=({const char*_tmp40D="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp40D,sizeof(char),73U);});Cyc_Warn_warn(_tmp8A9,_tmp8A8,_tag_fat(_tmp40C,sizeof(void*),0U));});});
# 1395
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71C;({enum Cyc_Parse_Storage_class _tmp8AA=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp71C.sc=_tmp8AA;}),_tmp71C.tq=two.tq,_tmp71C.type_specs=two.type_specs,_tmp71C.is_inline=two.is_inline,_tmp71C.attributes=two.attributes;_tmp71C;}));
# 1399
goto _LL0;}case 58U: _LL6F: _LL70:
# 1400 "parse.y"
({void*_tmp40E=0U;({unsigned _tmp8AC=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8AB=({const char*_tmp40F="__extension__ keyword ignored in declaration";_tag_fat(_tmp40F,sizeof(char),45U);});Cyc_Warn_warn(_tmp8AC,_tmp8AB,_tag_fat(_tmp40E,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1403
goto _LL0;case 59U: _LL71: _LL72:
# 1404 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71D;_tmp71D.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8AE=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp71D.tq=_tmp8AE;}),({
struct Cyc_Parse_Type_specifier _tmp8AD=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp71D.type_specs=_tmp8AD;}),_tmp71D.is_inline=0,_tmp71D.attributes=0;_tmp71D;}));
goto _LL0;case 60U: _LL73: _LL74: {
# 1407 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp410=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp410;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71E;_tmp71E.sc=two.sc,_tmp71E.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8B1=({unsigned _tmp8B0=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8AF=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8B0,_tmp8AF,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp71E.type_specs=_tmp8B1;}),_tmp71E.is_inline=two.is_inline,_tmp71E.attributes=two.attributes;_tmp71E;}));
# 1413
goto _LL0;}case 61U: _LL75: _LL76:
# 1414 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71F;_tmp71F.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8B3=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp71F.tq=_tmp8B3;}),({struct Cyc_Parse_Type_specifier _tmp8B2=Cyc_Parse_empty_spec(0U);_tmp71F.type_specs=_tmp8B2;}),_tmp71F.is_inline=0,_tmp71F.attributes=0;_tmp71F;}));
goto _LL0;case 62U: _LL77: _LL78: {
# 1416 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp411=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp411;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp720;_tmp720.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8B5=({struct Cyc_Absyn_Tqual _tmp8B4=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8B4,two.tq);});_tmp720.tq=_tmp8B5;}),_tmp720.type_specs=two.type_specs,_tmp720.is_inline=two.is_inline,_tmp720.attributes=two.attributes;_tmp720;}));
# 1421
goto _LL0;}case 63U: _LL79: _LL7A:
# 1422 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp721;_tmp721.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8B7=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp721.tq=_tmp8B7;}),({
struct Cyc_Parse_Type_specifier _tmp8B6=Cyc_Parse_empty_spec(0U);_tmp721.type_specs=_tmp8B6;}),_tmp721.is_inline=1,_tmp721.attributes=0;_tmp721;}));
goto _LL0;case 64U: _LL7B: _LL7C: {
# 1425 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp412=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp412;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp722;_tmp722.sc=two.sc,_tmp722.tq=two.tq,_tmp722.type_specs=two.type_specs,_tmp722.is_inline=1,_tmp722.attributes=two.attributes;_tmp722;}));
# 1429
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1430 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp723;_tmp723.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp8BA=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp723.tq=_tmp8BA;}),({
struct Cyc_Parse_Type_specifier _tmp8B9=Cyc_Parse_empty_spec(0U);_tmp723.type_specs=_tmp8B9;}),_tmp723.is_inline=0,({struct Cyc_List_List*_tmp8B8=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp723.attributes=_tmp8B8;});_tmp723;}));
goto _LL0;case 66U: _LL7F: _LL80: {
# 1433 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp413=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp413;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp724;_tmp724.sc=two.sc,_tmp724.tq=two.tq,_tmp724.type_specs=two.type_specs,_tmp724.is_inline=two.is_inline,({
# 1436
struct Cyc_List_List*_tmp8BC=({struct Cyc_List_List*_tmp8BB=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8BB,two.attributes);});_tmp724.attributes=_tmp8BC;});_tmp724;}));
goto _LL0;}case 67U: _LL81: _LL82:
# 1440 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Auto_sc);
goto _LL0;case 68U: _LL83: _LL84:
# 1441 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Register_sc);
goto _LL0;case 69U: _LL85: _LL86:
# 1442 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Static_sc);
goto _LL0;case 70U: _LL87: _LL88:
# 1443 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Extern_sc);
goto _LL0;case 71U: _LL89: _LL8A:
# 1445 "parse.y"
 if(({struct _fat_ptr _tmp8BD=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8BD,({const char*_tmp414="C";_tag_fat(_tmp414,sizeof(char),2U);}));})!= 0)
({void*_tmp415=0U;({unsigned _tmp8BF=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8BE=({const char*_tmp416="only extern or extern \"C\" is allowed";_tag_fat(_tmp416,sizeof(char),37U);});Cyc_Warn_err(_tmp8BF,_tmp8BE,_tag_fat(_tmp415,sizeof(void*),0U));});});
yyval=Cyc_YY20(Cyc_Parse_ExternC_sc);
# 1449
goto _LL0;case 72U: _LL8B: _LL8C:
# 1449 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Typedef_sc);
goto _LL0;case 73U: _LL8D: _LL8E:
# 1451 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Abstract_sc);
goto _LL0;case 74U: _LL8F: _LL90:
# 1456 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 75U: _LL91: _LL92:
# 1457 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76U: _LL93: _LL94:
# 1462 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1466 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp417=_cycalloc(sizeof(*_tmp417));({void*_tmp8C0=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp417->hd=_tmp8C0;}),_tmp417->tl=0;_tmp417;}));
goto _LL0;case 78U: _LL97: _LL98:
# 1467 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp418=_cycalloc(sizeof(*_tmp418));({void*_tmp8C2=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp418->hd=_tmp8C2;}),({struct Cyc_List_List*_tmp8C1=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp418->tl=_tmp8C1;});_tmp418;}));
goto _LL0;case 79U: _LL99: _LL9A: {
# 1472 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[19U]={{{_tmp41C,_tmp41C,_tmp41C + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp41D,_tmp41D,_tmp41D + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp41E,_tmp41E,_tmp41E + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp41F,_tmp41F,_tmp41F + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp420,_tmp420,_tmp420 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp421,_tmp421,_tmp421 + 8U},(void*)& att_aligned},{{_tmp422,_tmp422,_tmp422 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp423,_tmp423,_tmp423 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp424,_tmp424,_tmp424 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp425,_tmp425,_tmp425 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp426,_tmp426,_tmp426 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp427,_tmp427,_tmp427 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp428,_tmp428,_tmp428 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp429,_tmp429,_tmp429 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp42A,_tmp42A,_tmp42A + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp42B,_tmp42B,_tmp42B + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp42C,_tmp42C,_tmp42C + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp42D,_tmp42D,_tmp42D + 14U},(void*)& Cyc_Absyn_Always_inline_att_val},{{_tmp42E,_tmp42E,_tmp42E + 9U},(void*)& Cyc_Absyn_No_throw_att_val}};
# 1494
struct _fat_ptr _tmp419=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp419;
# 1496
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1498
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 19U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,19U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1505
if((unsigned)i == 19U){
({void*_tmp41A=0U;({unsigned _tmp8C4=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C3=({const char*_tmp41B="unrecognized attribute";_tag_fat(_tmp41B,sizeof(char),23U);});Cyc_Warn_err(_tmp8C4,_tmp8C3,_tag_fat(_tmp41A,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1510
goto _LL0;}}case 80U: _LL9B: _LL9C:
# 1510 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 81U: _LL9D: _LL9E: {
# 1512 "parse.y"
struct _fat_ptr _tmp42F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp42F;
struct Cyc_Absyn_Exp*_tmp430=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp430;
void*a;
if(({struct _fat_ptr _tmp8C6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8C6,({const char*_tmp431="aligned";_tag_fat(_tmp431,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8C5=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8C5,({const char*_tmp432="__aligned__";_tag_fat(_tmp432,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=6U,_tmp433->f1=e;_tmp433;});else{
if(({struct _fat_ptr _tmp8C8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8C8,({const char*_tmp434="section";_tag_fat(_tmp434,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8C7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8C7,({const char*_tmp435="__section__";_tag_fat(_tmp435,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=8U,({struct _fat_ptr _tmp8C9=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp436->f1=_tmp8C9;});_tmp436;});else{
if(({struct _fat_ptr _tmp8CA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CA,({const char*_tmp437="__mode__";_tag_fat(_tmp437,sizeof(char),9U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=24U,({struct _fat_ptr _tmp8CB=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp438->f1=_tmp8CB;});_tmp438;});else{
if(({struct _fat_ptr _tmp8CC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CC,({const char*_tmp439="alias";_tag_fat(_tmp439,sizeof(char),6U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=25U,({struct _fat_ptr _tmp8CD=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp43A->f1=_tmp8CD;});_tmp43A;});else{
# 1524
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8CF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CF,({const char*_tmp43B="regparm";_tag_fat(_tmp43B,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8CE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CE,({const char*_tmp43C="__regparm__";_tag_fat(_tmp43C,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp43D=0U;({unsigned _tmp8D1=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8D0=({const char*_tmp43E="regparm requires value between 0 and 3";_tag_fat(_tmp43E,sizeof(char),39U);});Cyc_Warn_err(_tmp8D1,_tmp8D0,_tag_fat(_tmp43D,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=0U,_tmp43F->f1=n;_tmp43F;});}else{
if(({struct _fat_ptr _tmp8D3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D3,({const char*_tmp440="initializes";_tag_fat(_tmp440,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8D2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D2,({const char*_tmp441="__initializes__";_tag_fat(_tmp441,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=20U,_tmp442->f1=n;_tmp442;});else{
if(({struct _fat_ptr _tmp8D5=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D5,({const char*_tmp443="noliveunique";_tag_fat(_tmp443,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8D4=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D4,({const char*_tmp444="__noliveunique__";_tag_fat(_tmp444,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=21U,_tmp445->f1=n;_tmp445;});else{
if(({struct _fat_ptr _tmp8D7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D7,({const char*_tmp446="consume";_tag_fat(_tmp446,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D6,({const char*_tmp447="__consume__";_tag_fat(_tmp447,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->tag=22U,_tmp448->f1=n;_tmp448;});else{
# 1536
({void*_tmp449=0U;({unsigned _tmp8D9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8D8=({const char*_tmp44A="unrecognized attribute";_tag_fat(_tmp44A,sizeof(char),23U);});Cyc_Warn_err(_tmp8D9,_tmp8D8,_tag_fat(_tmp449,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1540
yyval=Cyc_YY46(a);
# 1542
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1543 "parse.y"
struct _fat_ptr _tmp44B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp44B;
struct _fat_ptr _tmp44C=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp44C;
unsigned _tmp44D=({unsigned _tmp8DA=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DA,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp44D;
unsigned _tmp44E=({unsigned _tmp8DB=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DB,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp44E;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8DD=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DD,({const char*_tmp44F="format";_tag_fat(_tmp44F,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8DC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DC,({const char*_tmp450="__format__";_tag_fat(_tmp450,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8DF=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8DF,({const char*_tmp451="printf";_tag_fat(_tmp451,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8DE=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8DE,({const char*_tmp452="__printf__";_tag_fat(_tmp452,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->tag=19U,_tmp453->f1=Cyc_Absyn_Printf_ft,_tmp453->f2=(int)n,_tmp453->f3=(int)m;_tmp453;});else{
if(({struct _fat_ptr _tmp8E1=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E1,({const char*_tmp454="scanf";_tag_fat(_tmp454,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8E0=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E0,({const char*_tmp455="__scanf__";_tag_fat(_tmp455,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->tag=19U,_tmp456->f1=Cyc_Absyn_Scanf_ft,_tmp456->f2=(int)n,_tmp456->f3=(int)m;_tmp456;});else{
# 1554
({void*_tmp457=0U;({unsigned _tmp8E3=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8E2=({const char*_tmp458="unrecognized format type";_tag_fat(_tmp458,sizeof(char),25U);});Cyc_Warn_err(_tmp8E3,_tmp8E2,_tag_fat(_tmp457,sizeof(void*),0U));});});}}}else{
# 1556
({void*_tmp459=0U;({unsigned _tmp8E5=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8E4=({const char*_tmp45A="unrecognized attribute";_tag_fat(_tmp45A,sizeof(char),23U);});Cyc_Warn_err(_tmp8E5,_tmp8E4,_tag_fat(_tmp459,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1559
goto _LL0;}case 83U: _LLA1: _LLA2:
# 1568 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84U: _LLA3: _LLA4:
# 1570 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8E7=({struct _tuple0*_tmp8E6=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp8E6,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp8E7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85U: _LLA5: _LLA6:
# 1574 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1575 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1576 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1577 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1578 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1579 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1580 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1581 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1582 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1583 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95U: _LLB9: _LLBA:
# 1584 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1587
 yyval=Cyc_YY21(({void*_tmp8E8=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8E8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97U: _LLBD: _LLBE:
# 1589 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8E9=Cyc_Absyn_builtin_type(({const char*_tmp45B="__builtin_va_list";_tag_fat(_tmp45B,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp8E9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1591 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99U: _LLC1: _LLC2:
# 1593 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EA=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp8EA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100U: _LLC3: _LLC4:
# 1595 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EB=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8EB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1597 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EC=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp8EC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1599 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EF=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=6U,({struct Cyc_List_List*_tmp8EE=({unsigned _tmp8ED=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8ED,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});_tmp45C->f1=_tmp8EE;});_tmp45C;});Cyc_Parse_type_spec(_tmp8EF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1601
goto _LL0;case 103U: _LLC9: _LLCA:
# 1602 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F0=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104U: _LLCB: _LLCC:
# 1604 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp8F1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1606
goto _LL0;case 105U: _LLCD: _LLCE:
# 1607 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F2=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106U: _LLCF: _LLD0:
# 1609 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F3=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp8F3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1611 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F4=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1616 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp8F5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp8F5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6: {
# 1620 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp725;_tmp725.print_const=1,_tmp725.q_volatile=0,_tmp725.q_restrict=0,_tmp725.real_const=1,_tmp725.loc=loc;_tmp725;}));
goto _LL0;}case 110U: _LLD7: _LLD8:
# 1622 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp726;_tmp726.print_const=0,_tmp726.q_volatile=1,_tmp726.q_restrict=0,_tmp726.real_const=0,_tmp726.loc=0U;_tmp726;}));
goto _LL0;case 111U: _LLD9: _LLDA:
# 1623 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp727;_tmp727.print_const=0,_tmp727.q_volatile=0,_tmp727.q_restrict=1,_tmp727.real_const=0,_tmp727.loc=0U;_tmp727;}));
goto _LL0;case 112U: _LLDB: _LLDC: {
# 1629 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp45D=({struct Cyc_Absyn_TypeDecl*_tmp462=_cycalloc(sizeof(*_tmp462));({void*_tmp8FA=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8F9=({struct Cyc_Absyn_Enumdecl*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8F8=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp460->name=_tmp8F8;}),({struct Cyc_Core_Opt*_tmp8F7=({struct Cyc_Core_Opt*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct Cyc_List_List*_tmp8F6=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp45F->v=_tmp8F6;});_tmp45F;});_tmp460->fields=_tmp8F7;});_tmp460;});_tmp461->f1=_tmp8F9;});_tmp461;});_tmp462->r=_tmp8FA;}),_tmp462->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp462;});struct Cyc_Absyn_TypeDecl*ed=_tmp45D;
# 1631
yyval=Cyc_YY21(({void*_tmp8FB=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->tag=10U,_tmp45E->f1=ed,_tmp45E->f2=0;_tmp45E;});Cyc_Parse_type_spec(_tmp8FB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1633
goto _LL0;}case 113U: _LLDD: _LLDE:
# 1634 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FC=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp8FC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114U: _LLDF: _LLE0:
# 1636 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FD=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8FD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1642 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp463=_cycalloc(sizeof(*_tmp463));({struct _tuple0*_tmp8FE=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp463->name=_tmp8FE;}),_tmp463->tag=0,_tmp463->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp463;}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1644 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp464=_cycalloc(sizeof(*_tmp464));({struct _tuple0*_tmp900=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp464->name=_tmp900;}),({struct Cyc_Absyn_Exp*_tmp8FF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp464->tag=_tmp8FF;}),_tmp464->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp464;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1648 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));({struct Cyc_Absyn_Enumfield*_tmp901=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp465->hd=_tmp901;}),_tmp465->tl=0;_tmp465;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1649 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp466=_cycalloc(sizeof(*_tmp466));({struct Cyc_Absyn_Enumfield*_tmp902=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp466->hd=_tmp902;}),_tmp466->tl=0;_tmp466;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1650 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_Absyn_Enumfield*_tmp904=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp467->hd=_tmp904;}),({struct Cyc_List_List*_tmp903=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp467->tl=_tmp903;});_tmp467;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1656 "parse.y"
 yyval=Cyc_YY21(({void*_tmp907=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->tag=7U,({enum Cyc_Absyn_AggrKind _tmp906=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp468->f1=_tmp906;}),({struct Cyc_List_List*_tmp905=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp468->f2=_tmp905;});_tmp468;});Cyc_Parse_type_spec(_tmp907,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121U: _LLED: _LLEE: {
# 1662 "parse.y"
struct Cyc_List_List*_tmp469=({unsigned _tmp908=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp908,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp469;
struct Cyc_List_List*_tmp46A=({unsigned _tmp909=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp909,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp46A;
struct Cyc_Absyn_TypeDecl*_tmp46B=({enum Cyc_Absyn_AggrKind _tmp90F=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp90E=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp90D=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp90C=({
struct Cyc_List_List*_tmp90B=exist_ts;struct Cyc_List_List*_tmp90A=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp90B,_tmp90A,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1664
Cyc_Absyn_aggr_tdecl(_tmp90F,Cyc_Absyn_Public,_tmp90E,_tmp90D,_tmp90C,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp46B;
# 1667
yyval=Cyc_YY21(({void*_tmp910=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=10U,_tmp46C->f1=td,_tmp46C->f2=0;_tmp46C;});Cyc_Parse_type_spec(_tmp910,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1669
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1673 "parse.y"
struct Cyc_List_List*_tmp46D=({unsigned _tmp911=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp911,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp46D;
struct Cyc_List_List*_tmp46E=({unsigned _tmp912=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp912,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp46E;
struct Cyc_Absyn_TypeDecl*_tmp46F=({enum Cyc_Absyn_AggrKind _tmp918=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp917=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp916=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp915=({
struct Cyc_List_List*_tmp914=exist_ts;struct Cyc_List_List*_tmp913=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp914,_tmp913,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1675
Cyc_Absyn_aggr_tdecl(_tmp918,Cyc_Absyn_Public,_tmp917,_tmp916,_tmp915,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp46F;
# 1678
yyval=Cyc_YY21(({void*_tmp919=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=10U,_tmp470->f1=td,_tmp470->f2=0;_tmp470;});Cyc_Parse_type_spec(_tmp919,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1680
goto _LL0;}case 123U: _LLF1: _LLF2:
# 1681 "parse.y"
 yyval=Cyc_YY21(({void*_tmp91D=({union Cyc_Absyn_AggrInfo _tmp91C=({enum Cyc_Absyn_AggrKind _tmp91B=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp91A=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp91B,_tmp91A,({struct Cyc_Core_Opt*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->v=(void*)1;_tmp471;}));});Cyc_Absyn_aggr_type(_tmp91C,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp91D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1684
goto _LL0;case 124U: _LLF3: _LLF4:
# 1685 "parse.y"
 yyval=Cyc_YY21(({void*_tmp920=({union Cyc_Absyn_AggrInfo _tmp91F=({enum Cyc_Absyn_AggrKind _tmp91E=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp91E,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp91F,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp920,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 125U: _LLF5: _LLF6:
# 1690 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 126U: _LLF7: _LLF8:
# 1692 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 127U: _LLF9: _LLFA:
# 1696 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 128U: _LLFB: _LLFC:
# 1697 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1702 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 130U: _LLFF: _LL100: {
# 1704 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp472=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp472;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1708
struct Cyc_List_List*_tmp473=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp473;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1713
goto _LL0;}}case 131U: _LL101: _LL102:
# 1718 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*_tmp474));({struct Cyc_List_List*_tmp921=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp474->hd=_tmp921;}),_tmp474->tl=0;_tmp474;}));
goto _LL0;case 132U: _LL103: _LL104:
# 1720 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp475=_cycalloc(sizeof(*_tmp475));({struct Cyc_List_List*_tmp923=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp475->hd=_tmp923;}),({struct Cyc_List_List*_tmp922=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp475->tl=_tmp922;});_tmp475;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1724 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 134U: _LL107: _LL108:
# 1730 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp476=_region_malloc(yyr,sizeof(*_tmp476));_tmp476->tl=0,({struct _tuple12 _tmp924=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp476->hd=_tmp924;});_tmp476;}));
goto _LL0;case 135U: _LL109: _LL10A:
# 1732 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp477=_region_malloc(yyr,sizeof(*_tmp477));({struct _tuple13*_tmp926=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp477->tl=_tmp926;}),({struct _tuple12 _tmp925=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp477->hd=_tmp925;});_tmp477;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1737 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp728;({struct Cyc_Parse_Declarator _tmp927=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp728.f1=_tmp927;}),_tmp728.f2=0;_tmp728;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1739 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp729;({struct Cyc_Parse_Declarator _tmp929=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp729.f1=_tmp929;}),({struct Cyc_Absyn_Exp*_tmp928=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp729.f2=_tmp928;});_tmp729;}));
goto _LL0;case 138U: _LL10F: _LL110: {
# 1745 "parse.y"
struct _RegionHandle _tmp478=_new_region("temp");struct _RegionHandle*temp=& _tmp478;_push_region(temp);
{struct _tuple26 _tmp479=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp16=_tmp479;struct _tuple26 _tmp47A=_stmttmp16;struct Cyc_List_List*_tmp47D;struct Cyc_Parse_Type_specifier _tmp47C;struct Cyc_Absyn_Tqual _tmp47B;_LL465: _tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;_tmp47D=_tmp47A.f3;_LL466: {struct Cyc_Absyn_Tqual tq=_tmp47B;struct Cyc_Parse_Type_specifier tspecs=_tmp47C;struct Cyc_List_List*atts=_tmp47D;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp47E=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp47E;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp47F=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp17=_tmp47F;struct _tuple25*_tmp480=_stmttmp17;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Parse_Declarator _tmp481;_LL468: _tmp481=_tmp480->f1;_tmp482=_tmp480->f2;_tmp483=_tmp480->f3;_LL469: {struct Cyc_Parse_Declarator d=_tmp481;struct Cyc_Absyn_Exp*wd=_tmp482;struct Cyc_Absyn_Exp*wh=_tmp483;
decls=({struct _tuple11*_tmp484=_region_malloc(temp,sizeof(*_tmp484));_tmp484->tl=decls,_tmp484->hd=d;_tmp484;});
widths_and_reqs=({struct Cyc_List_List*_tmp486=_region_malloc(temp,sizeof(*_tmp486));
({struct _tuple17*_tmp92A=({struct _tuple17*_tmp485=_region_malloc(temp,sizeof(*_tmp485));_tmp485->f1=wd,_tmp485->f2=wh;_tmp485;});_tmp486->hd=_tmp92A;}),_tmp486->tl=widths_and_reqs;_tmp486;});}}}
# 1756
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp487=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp487;
struct Cyc_List_List*_tmp488=({struct _RegionHandle*_tmp92D=temp;struct _RegionHandle*_tmp92C=temp;struct Cyc_List_List*_tmp92B=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1759
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp92D,_tmp92C,_tmp92B,widths_and_reqs);});struct Cyc_List_List*info=_tmp488;
# 1762
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1764
_npop_handler(0U);goto _LL0;}}}}
# 1746
;_pop_region();}case 139U: _LL111: _LL112:
# 1772 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp72A;({struct Cyc_Absyn_Tqual _tmp92F=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72A.f1=_tmp92F;}),({struct Cyc_Parse_Type_specifier _tmp92E=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp72A.f2=_tmp92E;}),_tmp72A.f3=0;_tmp72A;}));
goto _LL0;case 140U: _LL113: _LL114: {
# 1774 "parse.y"
struct _tuple26 _tmp489=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp489;yyval=Cyc_YY35(({struct _tuple26 _tmp72B;_tmp72B.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp932=({unsigned _tmp931=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp930=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp931,_tmp930,two.f2);});_tmp72B.f2=_tmp932;}),_tmp72B.f3=two.f3;_tmp72B;}));
goto _LL0;}case 141U: _LL115: _LL116:
# 1776 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp72C;({struct Cyc_Absyn_Tqual _tmp934=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp72C.f1=_tmp934;}),({struct Cyc_Parse_Type_specifier _tmp933=Cyc_Parse_empty_spec(0U);_tmp72C.f2=_tmp933;}),_tmp72C.f3=0;_tmp72C;}));
goto _LL0;case 142U: _LL117: _LL118: {
# 1778 "parse.y"
struct _tuple26 _tmp48A=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48A;
yyval=Cyc_YY35(({struct _tuple26 _tmp72D;({struct Cyc_Absyn_Tqual _tmp936=({struct Cyc_Absyn_Tqual _tmp935=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp935,two.f1);});_tmp72D.f1=_tmp936;}),_tmp72D.f2=two.f2,_tmp72D.f3=two.f3;_tmp72D;}));
goto _LL0;}case 143U: _LL119: _LL11A:
# 1781 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp72E;({struct Cyc_Absyn_Tqual _tmp939=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72E.f1=_tmp939;}),({struct Cyc_Parse_Type_specifier _tmp938=Cyc_Parse_empty_spec(0U);_tmp72E.f2=_tmp938;}),({struct Cyc_List_List*_tmp937=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp72E.f3=_tmp937;});_tmp72E;}));
goto _LL0;case 144U: _LL11B: _LL11C: {
# 1783 "parse.y"
struct _tuple26 _tmp48B=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48B;yyval=Cyc_YY35(({struct _tuple26 _tmp72F;_tmp72F.f1=two.f1,_tmp72F.f2=two.f2,({struct Cyc_List_List*_tmp93B=({struct Cyc_List_List*_tmp93A=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp93A,two.f3);});_tmp72F.f3=_tmp93B;});_tmp72F;}));
goto _LL0;}case 145U: _LL11D: _LL11E:
# 1789 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp730;({struct Cyc_Absyn_Tqual _tmp93D=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp730.f1=_tmp93D;}),({struct Cyc_Parse_Type_specifier _tmp93C=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp730.f2=_tmp93C;}),_tmp730.f3=0;_tmp730;}));
goto _LL0;case 146U: _LL11F: _LL120: {
# 1791 "parse.y"
struct _tuple26 _tmp48C=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48C;yyval=Cyc_YY35(({struct _tuple26 _tmp731;_tmp731.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp940=({unsigned _tmp93F=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp93E=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp93F,_tmp93E,two.f2);});_tmp731.f2=_tmp940;}),_tmp731.f3=two.f3;_tmp731;}));
goto _LL0;}case 147U: _LL121: _LL122:
# 1793 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp732;({struct Cyc_Absyn_Tqual _tmp942=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp732.f1=_tmp942;}),({struct Cyc_Parse_Type_specifier _tmp941=Cyc_Parse_empty_spec(0U);_tmp732.f2=_tmp941;}),_tmp732.f3=0;_tmp732;}));
goto _LL0;case 148U: _LL123: _LL124: {
# 1795 "parse.y"
struct _tuple26 _tmp48D=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48D;
yyval=Cyc_YY35(({struct _tuple26 _tmp733;({struct Cyc_Absyn_Tqual _tmp944=({struct Cyc_Absyn_Tqual _tmp943=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp943,two.f1);});_tmp733.f1=_tmp944;}),_tmp733.f2=two.f2,_tmp733.f3=two.f3;_tmp733;}));
goto _LL0;}case 149U: _LL125: _LL126:
# 1798 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp734;({struct Cyc_Absyn_Tqual _tmp947=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp734.f1=_tmp947;}),({struct Cyc_Parse_Type_specifier _tmp946=Cyc_Parse_empty_spec(0U);_tmp734.f2=_tmp946;}),({struct Cyc_List_List*_tmp945=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp734.f3=_tmp945;});_tmp734;}));
goto _LL0;case 150U: _LL127: _LL128: {
# 1800 "parse.y"
struct _tuple26 _tmp48E=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp48E;yyval=Cyc_YY35(({struct _tuple26 _tmp735;_tmp735.f1=two.f1,_tmp735.f2=two.f2,({struct Cyc_List_List*_tmp949=({struct Cyc_List_List*_tmp948=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp948,two.f3);});_tmp735.f3=_tmp949;});_tmp735;}));
goto _LL0;}case 151U: _LL129: _LL12A:
# 1804 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 152U: _LL12B: _LL12C:
# 1810 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp48F=_region_malloc(yyr,sizeof(*_tmp48F));({struct _tuple25*_tmp94A=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp48F->hd=_tmp94A;}),_tmp48F->tl=0;_tmp48F;}));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1812 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp490=_region_malloc(yyr,sizeof(*_tmp490));({struct _tuple25*_tmp94C=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp490->hd=_tmp94C;}),({struct Cyc_List_List*_tmp94B=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp490->tl=_tmp94B;});_tmp490;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1817 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp491=_region_malloc(yyr,sizeof(*_tmp491));({struct Cyc_Parse_Declarator _tmp94E=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp491->f1=_tmp94E;}),_tmp491->f2=0,({struct Cyc_Absyn_Exp*_tmp94D=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp491->f3=_tmp94D;});_tmp491;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1821 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp495=_region_malloc(yyr,sizeof(*_tmp495));({struct _tuple0*_tmp953=({struct _tuple0*_tmp494=_cycalloc(sizeof(*_tmp494));({union Cyc_Absyn_Nmspace _tmp952=Cyc_Absyn_Rel_n(0);_tmp494->f1=_tmp952;}),({struct _fat_ptr*_tmp951=({struct _fat_ptr*_tmp493=_cycalloc(sizeof(*_tmp493));({struct _fat_ptr _tmp950=({const char*_tmp492="";_tag_fat(_tmp492,sizeof(char),1U);});*_tmp493=_tmp950;});_tmp493;});_tmp494->f2=_tmp951;});_tmp494;});(_tmp495->f1).id=_tmp953;}),(_tmp495->f1).varloc=0U,(_tmp495->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp94F=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp495->f2=_tmp94F;}),_tmp495->f3=0;_tmp495;}));
# 1823
goto _LL0;case 156U: _LL133: _LL134:
# 1826 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp499=_region_malloc(yyr,sizeof(*_tmp499));({struct _tuple0*_tmp957=({struct _tuple0*_tmp498=_cycalloc(sizeof(*_tmp498));({union Cyc_Absyn_Nmspace _tmp956=Cyc_Absyn_Rel_n(0);_tmp498->f1=_tmp956;}),({struct _fat_ptr*_tmp955=({struct _fat_ptr*_tmp497=_cycalloc(sizeof(*_tmp497));({struct _fat_ptr _tmp954=({const char*_tmp496="";_tag_fat(_tmp496,sizeof(char),1U);});*_tmp497=_tmp954;});_tmp497;});_tmp498->f2=_tmp955;});_tmp498;});(_tmp499->f1).id=_tmp957;}),(_tmp499->f1).varloc=0U,(_tmp499->f1).tms=0,_tmp499->f2=0,_tmp499->f3=0;_tmp499;}));
# 1828
goto _LL0;case 157U: _LL135: _LL136:
# 1829 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49A=_region_malloc(yyr,sizeof(*_tmp49A));({struct Cyc_Parse_Declarator _tmp959=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp49A->f1=_tmp959;}),({struct Cyc_Absyn_Exp*_tmp958=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp49A->f2=_tmp958;}),_tmp49A->f3=0;_tmp49A;}));
goto _LL0;case 158U: _LL137: _LL138:
# 1833 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 159U: _LL139: _LL13A:
# 1834 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 160U: _LL13B: _LL13C:
# 1838 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 161U: _LL13D: _LL13E:
# 1839 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 162U: _LL13F: _LL140: {
# 1845 "parse.y"
int _tmp49B=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp49B;
struct Cyc_List_List*_tmp49C=({unsigned _tmp95A=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95A,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp49C;
struct Cyc_Absyn_TypeDecl*_tmp49D=({struct _tuple0*_tmp95F=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp95E=ts;struct Cyc_Core_Opt*_tmp95D=({struct Cyc_Core_Opt*_tmp49F=_cycalloc(sizeof(*_tmp49F));({struct Cyc_List_List*_tmp95B=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp49F->v=_tmp95B;});_tmp49F;});int _tmp95C=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp95F,_tmp95E,_tmp95D,_tmp95C,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp49D;
# 1849
yyval=Cyc_YY21(({void*_tmp960=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->tag=10U,_tmp49E->f1=dd,_tmp49E->f2=0;_tmp49E;});Cyc_Parse_type_spec(_tmp960,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1851
goto _LL0;}case 163U: _LL141: _LL142: {
# 1852 "parse.y"
int _tmp4A0=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A0;
yyval=Cyc_YY21(({void*_tmp963=({union Cyc_Absyn_DatatypeInfo _tmp962=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp736;({struct _tuple0*_tmp961=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp736.name=_tmp961;}),_tmp736.is_extensible=is_extensible;_tmp736;}));Cyc_Absyn_datatype_type(_tmp962,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp963,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1855
goto _LL0;}case 164U: _LL143: _LL144: {
# 1856 "parse.y"
int _tmp4A1=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A1;
yyval=Cyc_YY21(({void*_tmp967=({union Cyc_Absyn_DatatypeFieldInfo _tmp966=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp737;({struct _tuple0*_tmp965=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp737.datatype_name=_tmp965;}),({struct _tuple0*_tmp964=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp737.field_name=_tmp964;}),_tmp737.is_extensible=is_extensible;_tmp737;}));Cyc_Absyn_datatype_field_type(_tmp966,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp967,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1859
goto _LL0;}case 165U: _LL145: _LL146:
# 1862 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 166U: _LL147: _LL148:
# 1863 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 167U: _LL149: _LL14A:
# 1867 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));({struct Cyc_Absyn_Datatypefield*_tmp968=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A2->hd=_tmp968;}),_tmp4A2->tl=0;_tmp4A2;}));
goto _LL0;case 168U: _LL14B: _LL14C:
# 1868 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));({struct Cyc_Absyn_Datatypefield*_tmp969=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A3->hd=_tmp969;}),_tmp4A3->tl=0;_tmp4A3;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1869 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));({struct Cyc_Absyn_Datatypefield*_tmp96B=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A4->hd=_tmp96B;}),({struct Cyc_List_List*_tmp96A=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4A4->tl=_tmp96A;});_tmp4A4;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1870 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));({struct Cyc_Absyn_Datatypefield*_tmp96D=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4A5->hd=_tmp96D;}),({struct Cyc_List_List*_tmp96C=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4A5->tl=_tmp96C;});_tmp4A5;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1874 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 172U: _LL153: _LL154:
# 1875 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 173U: _LL155: _LL156:
# 1876 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 174U: _LL157: _LL158:
# 1880 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));({struct _tuple0*_tmp96F=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4A6->name=_tmp96F;}),_tmp4A6->typs=0,_tmp4A6->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp96E=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4A6->sc=_tmp96E;});_tmp4A6;}));
goto _LL0;case 175U: _LL159: _LL15A: {
# 1882 "parse.y"
struct Cyc_List_List*_tmp4A7=({unsigned _tmp970=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp970,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4A7;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));({struct _tuple0*_tmp972=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4A8->name=_tmp972;}),_tmp4A8->typs=typs,_tmp4A8->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp971=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4A8->sc=_tmp971;});_tmp4A8;}));
goto _LL0;}case 176U: _LL15B: _LL15C:
# 1888 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 177U: _LL15D: _LL15E: {
# 1890 "parse.y"
struct Cyc_Parse_Declarator _tmp4A9=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4A9;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp738;_tmp738.id=two.id,_tmp738.varloc=two.varloc,({struct Cyc_List_List*_tmp974=({struct Cyc_List_List*_tmp973=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp973,two.tms);});_tmp738.tms=_tmp974;});_tmp738;}));
goto _LL0;}case 178U: _LL15F: _LL160:
# 1897 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 179U: _LL161: _LL162: {
# 1899 "parse.y"
struct Cyc_Parse_Declarator _tmp4AA=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4AA;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp739;_tmp739.id=two.id,_tmp739.varloc=two.varloc,({struct Cyc_List_List*_tmp976=({struct Cyc_List_List*_tmp975=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp975,two.tms);});_tmp739.tms=_tmp976;});_tmp739;}));
goto _LL0;}case 180U: _LL163: _LL164:
# 1905 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73A;({struct _tuple0*_tmp977=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp73A.id=_tmp977;}),_tmp73A.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp73A.tms=0;_tmp73A;}));
goto _LL0;case 181U: _LL165: _LL166:
# 1907 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 182U: _LL167: _LL168: {
# 1911 "parse.y"
struct Cyc_Parse_Declarator _tmp4AB=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4AB;
({struct Cyc_List_List*_tmp97A=({struct Cyc_List_List*_tmp4AD=_region_malloc(yyr,sizeof(*_tmp4AD));({void*_tmp979=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4AC=_region_malloc(yyr,sizeof(*_tmp4AC));_tmp4AC->tag=5U,_tmp4AC->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp978=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4AC->f2=_tmp978;});_tmp4AC;});_tmp4AD->hd=_tmp979;}),_tmp4AD->tl=d.tms;_tmp4AD;});d.tms=_tmp97A;});
yyval=(yyyvsp[2]).v;
# 1915
goto _LL0;}case 183U: _LL169: _LL16A:
# 1916 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73B;({struct _tuple0*_tmp980=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp73B.id=_tmp980;}),({unsigned _tmp97F=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp73B.varloc=_tmp97F;}),({struct Cyc_List_List*_tmp97E=({struct Cyc_List_List*_tmp4AF=_region_malloc(yyr,sizeof(*_tmp4AF));({void*_tmp97D=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4AE=_region_malloc(yyr,sizeof(*_tmp4AE));_tmp4AE->tag=0U,({void*_tmp97C=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4AE->f1=_tmp97C;}),_tmp4AE->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4AE;});_tmp4AF->hd=_tmp97D;}),({struct Cyc_List_List*_tmp97B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4AF->tl=_tmp97B;});_tmp4AF;});_tmp73B.tms=_tmp97E;});_tmp73B;}));
goto _LL0;case 184U: _LL16B: _LL16C:
# 1918 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73C;({struct _tuple0*_tmp987=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp73C.id=_tmp987;}),({unsigned _tmp986=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp73C.varloc=_tmp986;}),({
struct Cyc_List_List*_tmp985=({struct Cyc_List_List*_tmp4B1=_region_malloc(yyr,sizeof(*_tmp4B1));({void*_tmp984=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B0=_region_malloc(yyr,sizeof(*_tmp4B0));_tmp4B0->tag=1U,({struct Cyc_Absyn_Exp*_tmp983=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4B0->f1=_tmp983;}),({void*_tmp982=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B0->f2=_tmp982;}),_tmp4B0->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4B0;});_tmp4B1->hd=_tmp984;}),({struct Cyc_List_List*_tmp981=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B1->tl=_tmp981;});_tmp4B1;});_tmp73C.tms=_tmp985;});_tmp73C;}));
goto _LL0;case 185U: _LL16D: _LL16E: {
# 1921 "parse.y"
struct _tuple27*_tmp4B2=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp18=_tmp4B2;struct _tuple27*_tmp4B3=_stmttmp18;struct Cyc_List_List*_tmp4B8;void*_tmp4B7;struct Cyc_Absyn_VarargInfo*_tmp4B6;int _tmp4B5;struct Cyc_List_List*_tmp4B4;_LL46B: _tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;_tmp4B6=_tmp4B3->f3;_tmp4B7=_tmp4B3->f4;_tmp4B8=_tmp4B3->f5;_LL46C: {struct Cyc_List_List*lis=_tmp4B4;int b=_tmp4B5;struct Cyc_Absyn_VarargInfo*c=_tmp4B6;void*eff=_tmp4B7;struct Cyc_List_List*po=_tmp4B8;
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4B9;
struct Cyc_Absyn_Exp*_tmp4BA=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4BA;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73D;({struct _tuple0*_tmp98D=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp73D.id=_tmp98D;}),({unsigned _tmp98C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp73D.varloc=_tmp98C;}),({struct Cyc_List_List*_tmp98B=({struct Cyc_List_List*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));({void*_tmp98A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));_tmp4BC->tag=3U,({void*_tmp989=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4BB=_region_malloc(yyr,sizeof(*_tmp4BB));_tmp4BB->tag=1U,_tmp4BB->f1=lis,_tmp4BB->f2=b,_tmp4BB->f3=c,_tmp4BB->f4=eff,_tmp4BB->f5=po,_tmp4BB->f6=req,_tmp4BB->f7=ens;_tmp4BB;});_tmp4BC->f1=_tmp989;});_tmp4BC;});_tmp4BD->hd=_tmp98A;}),({struct Cyc_List_List*_tmp988=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4BD->tl=_tmp988;});_tmp4BD;});_tmp73D.tms=_tmp98B;});_tmp73D;}));
# 1926
goto _LL0;}}case 186U: _LL16F: _LL170:
# 1927 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73E;({struct _tuple0*_tmp997=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp73E.id=_tmp997;}),({unsigned _tmp996=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp73E.varloc=_tmp996;}),({
struct Cyc_List_List*_tmp995=({struct Cyc_List_List*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));({void*_tmp994=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4BF=_region_malloc(yyr,sizeof(*_tmp4BF));_tmp4BF->tag=3U,({void*_tmp993=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4BE=_region_malloc(yyr,sizeof(*_tmp4BE));_tmp4BE->tag=1U,_tmp4BE->f1=0,_tmp4BE->f2=0,_tmp4BE->f3=0,({
# 1930
void*_tmp992=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4BE->f4=_tmp992;}),({struct Cyc_List_List*_tmp991=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4BE->f5=_tmp991;}),({struct Cyc_Absyn_Exp*_tmp990=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4BE->f6=_tmp990;}),({struct Cyc_Absyn_Exp*_tmp98F=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4BE->f7=_tmp98F;});_tmp4BE;});
# 1928
_tmp4BF->f1=_tmp993;});_tmp4BF;});_tmp4C0->hd=_tmp994;}),({
# 1931
struct Cyc_List_List*_tmp98E=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C0->tl=_tmp98E;});_tmp4C0;});
# 1928
_tmp73E.tms=_tmp995;});_tmp73E;}));
# 1933
goto _LL0;case 187U: _LL171: _LL172:
# 1934 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73F;({struct _tuple0*_tmp99E=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp73F.id=_tmp99E;}),({unsigned _tmp99D=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp73F.varloc=_tmp99D;}),({struct Cyc_List_List*_tmp99C=({struct Cyc_List_List*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));({void*_tmp99B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C2=_region_malloc(yyr,sizeof(*_tmp4C2));_tmp4C2->tag=3U,({void*_tmp99A=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4C1=_region_malloc(yyr,sizeof(*_tmp4C1));_tmp4C1->tag=0U,({struct Cyc_List_List*_tmp999=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4C1->f1=_tmp999;}),_tmp4C1->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4C1;});_tmp4C2->f1=_tmp99A;});_tmp4C2;});_tmp4C3->hd=_tmp99B;}),({struct Cyc_List_List*_tmp998=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C3->tl=_tmp998;});_tmp4C3;});_tmp73F.tms=_tmp99C;});_tmp73F;}));
goto _LL0;case 188U: _LL173: _LL174: {
# 1937
struct Cyc_List_List*_tmp4C4=({unsigned _tmp99F=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp99F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4C4;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp740;({struct _tuple0*_tmp9A4=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp740.id=_tmp9A4;}),({unsigned _tmp9A3=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp740.varloc=_tmp9A3;}),({struct Cyc_List_List*_tmp9A2=({struct Cyc_List_List*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));({void*_tmp9A1=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));_tmp4C5->tag=4U,_tmp4C5->f1=ts,_tmp4C5->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4C5->f3=0;_tmp4C5;});_tmp4C6->hd=_tmp9A1;}),({struct Cyc_List_List*_tmp9A0=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C6->tl=_tmp9A0;});_tmp4C6;});_tmp740.tms=_tmp9A2;});_tmp740;}));
# 1940
goto _LL0;}case 189U: _LL175: _LL176:
# 1941 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp741;({struct _tuple0*_tmp9AA=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp741.id=_tmp9AA;}),({unsigned _tmp9A9=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp741.varloc=_tmp9A9;}),({struct Cyc_List_List*_tmp9A8=({struct Cyc_List_List*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));({void*_tmp9A7=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=5U,_tmp4C7->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9A6=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4C7->f2=_tmp9A6;});_tmp4C7;});_tmp4C8->hd=_tmp9A7;}),({
struct Cyc_List_List*_tmp9A5=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C8->tl=_tmp9A5;});_tmp4C8;});
# 1941
_tmp741.tms=_tmp9A8;});_tmp741;}));
# 1944
goto _LL0;case 190U: _LL177: _LL178:
# 1949 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp742;({struct _tuple0*_tmp9AB=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp742.id=_tmp9AB;}),_tmp742.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp742.tms=0;_tmp742;}));
goto _LL0;case 191U: _LL179: _LL17A:
# 1951 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp743;({struct _tuple0*_tmp9AC=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp743.id=_tmp9AC;}),_tmp743.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp743.tms=0;_tmp743;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 1953 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 193U: _LL17D: _LL17E: {
# 1957 "parse.y"
struct Cyc_Parse_Declarator _tmp4C9=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4C9;
({struct Cyc_List_List*_tmp9AF=({struct Cyc_List_List*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));({void*_tmp9AE=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=5U,_tmp4CA->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9AD=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4CA->f2=_tmp9AD;});_tmp4CA;});_tmp4CB->hd=_tmp9AE;}),_tmp4CB->tl=d.tms;_tmp4CB;});d.tms=_tmp9AF;});
yyval=(yyyvsp[2]).v;
# 1961
goto _LL0;}case 194U: _LL17F: _LL180: {
# 1962 "parse.y"
struct Cyc_Parse_Declarator _tmp4CC=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4CC;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp744;_tmp744.id=one.id,_tmp744.varloc=one.varloc,({
struct Cyc_List_List*_tmp9B2=({struct Cyc_List_List*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));({void*_tmp9B1=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));_tmp4CD->tag=0U,({void*_tmp9B0=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4CD->f1=_tmp9B0;}),_tmp4CD->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4CD;});_tmp4CE->hd=_tmp9B1;}),_tmp4CE->tl=one.tms;_tmp4CE;});_tmp744.tms=_tmp9B2;});_tmp744;}));
goto _LL0;}case 195U: _LL181: _LL182: {
# 1966 "parse.y"
struct Cyc_Parse_Declarator _tmp4CF=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4CF;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;_tmp745.id=one.id,_tmp745.varloc=one.varloc,({
struct Cyc_List_List*_tmp9B6=({struct Cyc_List_List*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));({void*_tmp9B5=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));_tmp4D0->tag=1U,({struct Cyc_Absyn_Exp*_tmp9B4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4D0->f1=_tmp9B4;}),({void*_tmp9B3=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4D0->f2=_tmp9B3;}),_tmp4D0->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4D0;});_tmp4D1->hd=_tmp9B5;}),_tmp4D1->tl=one.tms;_tmp4D1;});_tmp745.tms=_tmp9B6;});_tmp745;}));
# 1970
goto _LL0;}case 196U: _LL183: _LL184: {
# 1971 "parse.y"
struct _tuple27*_tmp4D2=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp19=_tmp4D2;struct _tuple27*_tmp4D3=_stmttmp19;struct Cyc_List_List*_tmp4D8;void*_tmp4D7;struct Cyc_Absyn_VarargInfo*_tmp4D6;int _tmp4D5;struct Cyc_List_List*_tmp4D4;_LL46E: _tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;_tmp4D6=_tmp4D3->f3;_tmp4D7=_tmp4D3->f4;_tmp4D8=_tmp4D3->f5;_LL46F: {struct Cyc_List_List*lis=_tmp4D4;int b=_tmp4D5;struct Cyc_Absyn_VarargInfo*c=_tmp4D6;void*eff=_tmp4D7;struct Cyc_List_List*po=_tmp4D8;
struct Cyc_Absyn_Exp*_tmp4D9=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4D9;
struct Cyc_Absyn_Exp*_tmp4DA=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4DA;
struct Cyc_Parse_Declarator _tmp4DB=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4DB;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;_tmp746.id=one.id,_tmp746.varloc=one.varloc,({struct Cyc_List_List*_tmp9B9=({struct Cyc_List_List*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));({void*_tmp9B8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));_tmp4DD->tag=3U,({void*_tmp9B7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));_tmp4DC->tag=1U,_tmp4DC->f1=lis,_tmp4DC->f2=b,_tmp4DC->f3=c,_tmp4DC->f4=eff,_tmp4DC->f5=po,_tmp4DC->f6=req,_tmp4DC->f7=ens;_tmp4DC;});_tmp4DD->f1=_tmp9B7;});_tmp4DD;});_tmp4DE->hd=_tmp9B8;}),_tmp4DE->tl=one.tms;_tmp4DE;});_tmp746.tms=_tmp9B9;});_tmp746;}));
# 1977
goto _LL0;}}case 197U: _LL185: _LL186: {
# 1978 "parse.y"
struct Cyc_Parse_Declarator _tmp4DF=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4DF;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;_tmp747.id=one.id,_tmp747.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C0=({struct Cyc_List_List*_tmp4E2=_region_malloc(yyr,sizeof(*_tmp4E2));({void*_tmp9BF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E1=_region_malloc(yyr,sizeof(*_tmp4E1));_tmp4E1->tag=3U,({void*_tmp9BE=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));_tmp4E0->tag=1U,_tmp4E0->f1=0,_tmp4E0->f2=0,_tmp4E0->f3=0,({
# 1982
void*_tmp9BD=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4E0->f4=_tmp9BD;}),({struct Cyc_List_List*_tmp9BC=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4E0->f5=_tmp9BC;}),({struct Cyc_Absyn_Exp*_tmp9BB=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4E0->f6=_tmp9BB;}),({struct Cyc_Absyn_Exp*_tmp9BA=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4E0->f7=_tmp9BA;});_tmp4E0;});
# 1980
_tmp4E1->f1=_tmp9BE;});_tmp4E1;});_tmp4E2->hd=_tmp9BF;}),_tmp4E2->tl=one.tms;_tmp4E2;});_tmp747.tms=_tmp9C0;});_tmp747;}));
# 1985
goto _LL0;}case 198U: _LL187: _LL188: {
# 1986 "parse.y"
struct Cyc_Parse_Declarator _tmp4E3=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;_tmp748.id=one.id,_tmp748.varloc=one.varloc,({struct Cyc_List_List*_tmp9C4=({struct Cyc_List_List*_tmp4E6=_region_malloc(yyr,sizeof(*_tmp4E6));({void*_tmp9C3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));_tmp4E5->tag=3U,({void*_tmp9C2=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));_tmp4E4->tag=0U,({struct Cyc_List_List*_tmp9C1=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4E4->f1=_tmp9C1;}),_tmp4E4->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4E4;});_tmp4E5->f1=_tmp9C2;});_tmp4E5;});_tmp4E6->hd=_tmp9C3;}),_tmp4E6->tl=one.tms;_tmp4E6;});_tmp748.tms=_tmp9C4;});_tmp748;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 1990
struct Cyc_List_List*_tmp4E7=({unsigned _tmp9C5=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9C5,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4E7;
struct Cyc_Parse_Declarator _tmp4E8=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E8;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;_tmp749.id=one.id,_tmp749.varloc=one.varloc,({struct Cyc_List_List*_tmp9C7=({struct Cyc_List_List*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));({void*_tmp9C6=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4E9=_region_malloc(yyr,sizeof(*_tmp4E9));_tmp4E9->tag=4U,_tmp4E9->f1=ts,_tmp4E9->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4E9->f3=0;_tmp4E9;});_tmp4EA->hd=_tmp9C6;}),_tmp4EA->tl=one.tms;_tmp4EA;});_tmp749.tms=_tmp9C7;});_tmp749;}));
# 1994
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 1995 "parse.y"
struct Cyc_Parse_Declarator _tmp4EB=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EB;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;_tmp74A.id=one.id,_tmp74A.varloc=one.varloc,({struct Cyc_List_List*_tmp9CA=({struct Cyc_List_List*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));({void*_tmp9C9=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EC=_region_malloc(yyr,sizeof(*_tmp4EC));_tmp4EC->tag=5U,_tmp4EC->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9C8=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4EC->f2=_tmp9C8;});_tmp4EC;});_tmp4ED->hd=_tmp9C9;}),_tmp4ED->tl=one.tms;_tmp4ED;});_tmp74A.tms=_tmp9CA;});_tmp74A;}));
# 1998
goto _LL0;}case 201U: _LL18D: _LL18E:
# 2002 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 202U: _LL18F: _LL190:
# 2003 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9CB=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9CB,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 203U: _LL191: _LL192: {
# 2007 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));({void*_tmp9CD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));_tmp4EE->tag=5U,_tmp4EE->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9CC=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4EE->f2=_tmp9CC;});_tmp4EE;});_tmp4EF->hd=_tmp9CD;}),_tmp4EF->tl=ans;_tmp4EF;});{
# 2011
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4F0=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1A=_tmp4F0;struct _tuple22 _tmp4F1=_stmttmp1A;void*_tmp4F4;void*_tmp4F3;unsigned _tmp4F2;_LL471: _tmp4F2=_tmp4F1.f1;_tmp4F3=_tmp4F1.f2;_tmp4F4=_tmp4F1.f3;_LL472: {unsigned ploc=_tmp4F2;void*nullable=_tmp4F3;void*bound=_tmp4F4;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->ptr_loc=ploc,_tmp4F5->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp4F5->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp4F5;});{
struct _tuple15 _tmp4F6=({unsigned _tmp9D1=ploc;void*_tmp9D0=nullable;void*_tmp9CF=bound;void*_tmp9CE=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9D1,_tmp9D0,_tmp9CF,_tmp9CE,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1B=_tmp4F6;struct _tuple15 _tmp4F7=_stmttmp1B;void*_tmp4FB;void*_tmp4FA;void*_tmp4F9;void*_tmp4F8;_LL474: _tmp4F8=_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;_tmp4FA=_tmp4F7.f3;_tmp4FB=_tmp4F7.f4;_LL475: {void*nullable=_tmp4F8;void*bound=_tmp4F9;void*zeroterm=_tmp4FA;void*rgn_opt=_tmp4FB;
ans=({struct Cyc_List_List*_tmp4FD=_region_malloc(yyr,sizeof(*_tmp4FD));({void*_tmp9D3=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4FC=_region_malloc(yyr,sizeof(*_tmp4FC));_tmp4FC->tag=2U,(_tmp4FC->f1).rgn=rgn_opt,(_tmp4FC->f1).nullable=nullable,(_tmp4FC->f1).bounds=bound,(_tmp4FC->f1).zero_term=zeroterm,(_tmp4FC->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp9D2=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp4FC->f2=_tmp9D2;});_tmp4FC;});_tmp4FD->hd=_tmp9D3;}),_tmp4FD->tl=ans;_tmp4FD;});
yyval=Cyc_YY26(ans);
# 2019
goto _LL0;}}}}}case 204U: _LL193: _LL194:
# 2021
 yyval=Cyc_YY56(0);
goto _LL0;case 205U: _LL195: _LL196:
# 2022 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp4FE=_region_malloc(yyr,sizeof(*_tmp4FE));({void*_tmp9D5=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp4FE->hd=_tmp9D5;}),({struct Cyc_List_List*_tmp9D4=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp4FE->tl=_tmp9D4;});_tmp4FE;}));
goto _LL0;case 206U: _LL197: _LL198:
# 2027 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp4FF=_region_malloc(yyr,sizeof(*_tmp4FF));_tmp4FF->tag=0U,({struct Cyc_Absyn_Exp*_tmp9D6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4FF->f1=_tmp9D6;});_tmp4FF;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2029 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp500=_region_malloc(yyr,sizeof(*_tmp500));_tmp500->tag=1U,({void*_tmp9D7=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp500->f1=_tmp9D7;});_tmp500;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2030 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp501=_region_malloc(yyr,sizeof(*_tmp501));_tmp501->tag=2U;_tmp501;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2031 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp502=_region_malloc(yyr,sizeof(*_tmp502));_tmp502->tag=3U;_tmp502;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2032 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));_tmp503->tag=4U;_tmp503;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2033 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));_tmp504->tag=5U;_tmp504;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2034 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp505=_region_malloc(yyr,sizeof(*_tmp505));_tmp505->tag=6U;_tmp505;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2035 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=7U;_tmp506;}));
goto _LL0;case 214U: _LL1A7: _LL1A8:
# 2041 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp507->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp507->f3=Cyc_Absyn_fat_bound_type:({void*_tmp9D8=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp507->f3=_tmp9D8;});_tmp507;}));
# 2043
goto _LL0;case 215U: _LL1A9: _LL1AA:
# 2043 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp508->f2=Cyc_Absyn_false_type,({void*_tmp9D9=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp508->f3=_tmp9D9;});_tmp508;}));
goto _LL0;case 216U: _LL1AB: _LL1AC:
# 2044 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp509->f2=Cyc_Absyn_true_type,_tmp509->f3=Cyc_Absyn_fat_bound_type;_tmp509;}));
goto _LL0;case 217U: _LL1AD: _LL1AE:
# 2047
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2048 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2051
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2052 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2053 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2058 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2059 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2060 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2064 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2065 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9DA=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9DA,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2070 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp50A=_cycalloc(sizeof(*_tmp50A));({struct Cyc_List_List*_tmp9DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp50A->f1=_tmp9DD;}),_tmp50A->f2=0,_tmp50A->f3=0,({void*_tmp9DC=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp50A->f4=_tmp9DC;}),({struct Cyc_List_List*_tmp9DB=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp50A->f5=_tmp9DB;});_tmp50A;}));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2072 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp50B=_cycalloc(sizeof(*_tmp50B));({struct Cyc_List_List*_tmp9E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp50B->f1=_tmp9E0;}),_tmp50B->f2=1,_tmp50B->f3=0,({void*_tmp9DF=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp50B->f4=_tmp9DF;}),({struct Cyc_List_List*_tmp9DE=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp50B->f5=_tmp9DE;});_tmp50B;}));
goto _LL0;case 229U: _LL1C5: _LL1C6: {
# 2075
struct _tuple8*_tmp50C=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1C=_tmp50C;struct _tuple8*_tmp50D=_stmttmp1C;void*_tmp510;struct Cyc_Absyn_Tqual _tmp50F;struct _fat_ptr*_tmp50E;_LL477: _tmp50E=_tmp50D->f1;_tmp50F=_tmp50D->f2;_tmp510=_tmp50D->f3;_LL478: {struct _fat_ptr*n=_tmp50E;struct Cyc_Absyn_Tqual tq=_tmp50F;void*t=_tmp510;
struct Cyc_Absyn_VarargInfo*_tmp511=({struct Cyc_Absyn_VarargInfo*_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->name=n,_tmp513->tq=tq,_tmp513->type=t,({int _tmp9E1=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp513->inject=_tmp9E1;});_tmp513;});struct Cyc_Absyn_VarargInfo*v=_tmp511;
yyval=Cyc_YY39(({struct _tuple27*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=0,_tmp512->f2=0,_tmp512->f3=v,({void*_tmp9E3=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp512->f4=_tmp9E3;}),({struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp512->f5=_tmp9E2;});_tmp512;}));
# 2079
goto _LL0;}}case 230U: _LL1C7: _LL1C8: {
# 2081
struct _tuple8*_tmp514=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1D=_tmp514;struct _tuple8*_tmp515=_stmttmp1D;void*_tmp518;struct Cyc_Absyn_Tqual _tmp517;struct _fat_ptr*_tmp516;_LL47A: _tmp516=_tmp515->f1;_tmp517=_tmp515->f2;_tmp518=_tmp515->f3;_LL47B: {struct _fat_ptr*n=_tmp516;struct Cyc_Absyn_Tqual tq=_tmp517;void*t=_tmp518;
struct Cyc_Absyn_VarargInfo*_tmp519=({struct Cyc_Absyn_VarargInfo*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->name=n,_tmp51B->tq=tq,_tmp51B->type=t,({int _tmp9E4=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp51B->inject=_tmp9E4;});_tmp51B;});struct Cyc_Absyn_VarargInfo*v=_tmp519;
yyval=Cyc_YY39(({struct _tuple27*_tmp51A=_cycalloc(sizeof(*_tmp51A));({struct Cyc_List_List*_tmp9E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp51A->f1=_tmp9E7;}),_tmp51A->f2=0,_tmp51A->f3=v,({void*_tmp9E6=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp51A->f4=_tmp9E6;}),({struct Cyc_List_List*_tmp9E5=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp51A->f5=_tmp9E5;});_tmp51A;}));
# 2085
goto _LL0;}}case 231U: _LL1C9: _LL1CA:
# 2089 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9E8=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9E8,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->tag=1U,_tmp51C->f1=0;_tmp51C;}));}));
goto _LL0;case 232U: _LL1CB: _LL1CC:
# 2090 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9E9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9E9,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 233U: _LL1CD: _LL1CE:
# 2093
 yyval=Cyc_YY49(0);
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2094 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2098 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2099 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 237U: _LL1D5: _LL1D6: {
# 2107 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->tag=2U,_tmp521->f1=0,_tmp521->f2=& Cyc_Kinds_trk;_tmp521;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp51D;
void*_tmp51E=({struct _fat_ptr _tmp9EA=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9EA,(void*)kb);});void*t=_tmp51E;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp520=_cycalloc(sizeof(*_tmp520));({struct _tuple34*_tmp9EC=({struct _tuple34*_tmp51F=_cycalloc(sizeof(*_tmp51F));({void*_tmp9EB=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp51F->f1=_tmp9EB;}),_tmp51F->f2=t;_tmp51F;});_tmp520->hd=_tmp9EC;}),_tmp520->tl=0;_tmp520;}));
# 2111
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2113 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp522=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->tag=2U,_tmp526->f1=0,_tmp526->f2=& Cyc_Kinds_trk;_tmp526;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp522;
void*_tmp523=({struct _fat_ptr _tmp9ED=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp9ED,(void*)kb);});void*t=_tmp523;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp525=_cycalloc(sizeof(*_tmp525));({struct _tuple34*_tmp9F0=({struct _tuple34*_tmp524=_cycalloc(sizeof(*_tmp524));({void*_tmp9EF=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp524->f1=_tmp9EF;}),_tmp524->f2=t;_tmp524;});_tmp525->hd=_tmp9F0;}),({struct Cyc_List_List*_tmp9EE=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp525->tl=_tmp9EE;});_tmp525;}));
# 2117
goto _LL0;}case 239U: _LL1D9: _LL1DA:
# 2121 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 240U: _LL1DB: _LL1DC:
# 2123 "parse.y"
 if(({struct _fat_ptr _tmp9F1=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp9F1,({const char*_tmp527="inject";_tag_fat(_tmp527,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp529=({struct Cyc_Warn_String_Warn_Warg_struct _tmp74B;_tmp74B.tag=0U,({struct _fat_ptr _tmp9F2=({const char*_tmp52A="missing type in function declaration";_tag_fat(_tmp52A,sizeof(char),37U);});_tmp74B.f1=_tmp9F2;});_tmp74B;});void*_tmp528[1U];_tmp528[0]=& _tmp529;({unsigned _tmp9F3=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp9F3,_tag_fat(_tmp528,sizeof(void*),1U));});});
yyval=Cyc_YY31(1);
# 2127
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2130 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2131 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9F4=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F4,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2135 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2136 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2138 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp52B=_cycalloc(sizeof(*_tmp52B));({void*_tmp9F5=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp52B->hd=_tmp9F5;}),_tmp52B->tl=0;_tmp52B;}));
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2140 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp52C=_cycalloc(sizeof(*_tmp52C));({void*_tmp9F6=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp52C->hd=_tmp9F6;}),_tmp52C->tl=0;_tmp52C;}));
# 2143
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2148 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp52D=_cycalloc(sizeof(*_tmp52D));({void*_tmp9F8=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9F7=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9F7,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp52D->hd=_tmp9F8;}),_tmp52D->tl=0;_tmp52D;}));
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2150 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp52E=_cycalloc(sizeof(*_tmp52E));({void*_tmp9FB=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9FA=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9FA,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp52E->hd=_tmp9FB;}),({struct Cyc_List_List*_tmp9F9=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp52E->tl=_tmp9F9;});_tmp52E;}));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2156 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*_tmp52F));({struct _tuple8*_tmp9FC=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp52F->hd=_tmp9FC;}),_tmp52F->tl=0;_tmp52F;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2158 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));({struct _tuple8*_tmp9FE=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp530->hd=_tmp9FE;}),({struct Cyc_List_List*_tmp9FD=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp530->tl=_tmp9FD;});_tmp530;}));
goto _LL0;case 251U: _LL1F1: _LL1F2: {
# 2164 "parse.y"
struct _tuple26 _tmp531=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp1E=_tmp531;struct _tuple26 _tmp532=_stmttmp1E;struct Cyc_List_List*_tmp535;struct Cyc_Parse_Type_specifier _tmp534;struct Cyc_Absyn_Tqual _tmp533;_LL47D: _tmp533=_tmp532.f1;_tmp534=_tmp532.f2;_tmp535=_tmp532.f3;_LL47E: {struct Cyc_Absyn_Tqual tq=_tmp533;struct Cyc_Parse_Type_specifier tspecs=_tmp534;struct Cyc_List_List*atts=_tmp535;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp536=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1F=_tmp536;struct Cyc_Parse_Declarator _tmp537=_stmttmp1F;struct Cyc_List_List*_tmp53A;unsigned _tmp539;struct _tuple0*_tmp538;_LL480: _tmp538=_tmp537.id;_tmp539=_tmp537.varloc;_tmp53A=_tmp537.tms;_LL481: {struct _tuple0*qv=_tmp538;unsigned varloc=_tmp539;struct Cyc_List_List*tms=_tmp53A;
void*_tmp53B=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp53B;
struct _tuple14 _tmp53C=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp20=_tmp53C;struct _tuple14 _tmp53D=_stmttmp20;struct Cyc_List_List*_tmp541;struct Cyc_List_List*_tmp540;void*_tmp53F;struct Cyc_Absyn_Tqual _tmp53E;_LL483: _tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;_tmp540=_tmp53D.f3;_tmp541=_tmp53D.f4;_LL484: {struct Cyc_Absyn_Tqual tq2=_tmp53E;void*t2=_tmp53F;struct Cyc_List_List*tvs=_tmp540;struct Cyc_List_List*atts2=_tmp541;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp543=({struct Cyc_Warn_String_Warn_Warg_struct _tmp74C;_tmp74C.tag=0U,({struct _fat_ptr _tmp9FF=({const char*_tmp544="parameter with bad type params";_tag_fat(_tmp544,sizeof(char),31U);});_tmp74C.f1=_tmp9FF;});_tmp74C;});void*_tmp542[1U];_tmp542[0]=& _tmp543;({unsigned _tmpA00=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmpA00,_tag_fat(_tmp542,sizeof(void*),1U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp546=({struct Cyc_Warn_String_Warn_Warg_struct _tmp74D;_tmp74D.tag=0U,({struct _fat_ptr _tmpA01=({const char*_tmp547="parameter cannot be qualified with a namespace";_tag_fat(_tmp547,sizeof(char),47U);});_tmp74D.f1=_tmpA01;});_tmp74D;});void*_tmp545[1U];_tmp545[0]=& _tmp546;({unsigned _tmpA02=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA02,_tag_fat(_tmp545,sizeof(void*),1U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp549=({struct Cyc_Warn_String_Warn_Warg_struct _tmp74E;_tmp74E.tag=0U,({struct _fat_ptr _tmpA03=({const char*_tmp54A="extra attributes on parameter, ignoring";_tag_fat(_tmp54A,sizeof(char),40U);});_tmp74E.f1=_tmpA03;});_tmp74E;});void*_tmp548[1U];_tmp548[0]=& _tmp549;({unsigned _tmpA04=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA04,_tag_fat(_tmp548,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->f1=idopt,_tmp54B->f2=tq2,_tmp54B->f3=t2;_tmp54B;}));
# 2178
goto _LL0;}}}}}}case 252U: _LL1F3: _LL1F4: {
# 2179 "parse.y"
struct _tuple26 _tmp54C=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp54C;struct _tuple26 _tmp54D=_stmttmp21;struct Cyc_List_List*_tmp550;struct Cyc_Parse_Type_specifier _tmp54F;struct Cyc_Absyn_Tqual _tmp54E;_LL486: _tmp54E=_tmp54D.f1;_tmp54F=_tmp54D.f2;_tmp550=_tmp54D.f3;_LL487: {struct Cyc_Absyn_Tqual tq=_tmp54E;struct Cyc_Parse_Type_specifier tspecs=_tmp54F;struct Cyc_List_List*atts=_tmp550;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp551=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp551;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp553=({struct Cyc_Warn_String_Warn_Warg_struct _tmp74F;_tmp74F.tag=0U,({struct _fat_ptr _tmpA05=({const char*_tmp554="bad attributes on parameter, ignoring";_tag_fat(_tmp554,sizeof(char),38U);});_tmp74F.f1=_tmpA05;});_tmp74F;});void*_tmp552[1U];_tmp552[0]=& _tmp553;({unsigned _tmpA06=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA06,_tag_fat(_tmp552,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->f1=0,_tmp555->f2=tq,_tmp555->f3=t;_tmp555;}));
# 2186
goto _LL0;}}}case 253U: _LL1F5: _LL1F6: {
# 2187 "parse.y"
struct _tuple26 _tmp556=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp22=_tmp556;struct _tuple26 _tmp557=_stmttmp22;struct Cyc_List_List*_tmp55A;struct Cyc_Parse_Type_specifier _tmp559;struct Cyc_Absyn_Tqual _tmp558;_LL489: _tmp558=_tmp557.f1;_tmp559=_tmp557.f2;_tmp55A=_tmp557.f3;_LL48A: {struct Cyc_Absyn_Tqual tq=_tmp558;struct Cyc_Parse_Type_specifier tspecs=_tmp559;struct Cyc_List_List*atts=_tmp55A;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp55B=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp55B;
struct Cyc_List_List*_tmp55C=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp55C;
struct _tuple14 _tmp55D=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp55D;struct _tuple14 _tmp55E=_stmttmp23;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp561;void*_tmp560;struct Cyc_Absyn_Tqual _tmp55F;_LL48C: _tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;_tmp561=_tmp55E.f3;_tmp562=_tmp55E.f4;_LL48D: {struct Cyc_Absyn_Tqual tq2=_tmp55F;void*t2=_tmp560;struct Cyc_List_List*tvs=_tmp561;struct Cyc_List_List*atts2=_tmp562;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp564=({struct Cyc_Warn_String_Warn_Warg_struct _tmp750;_tmp750.tag=0U,({
struct _fat_ptr _tmpA07=({const char*_tmp565="bad type parameters on formal argument, ignoring";_tag_fat(_tmp565,sizeof(char),49U);});_tmp750.f1=_tmpA07;});_tmp750;});void*_tmp563[1U];_tmp563[0]=& _tmp564;({unsigned _tmpA08=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA08,_tag_fat(_tmp563,sizeof(void*),1U));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp567=({struct Cyc_Warn_String_Warn_Warg_struct _tmp751;_tmp751.tag=0U,({struct _fat_ptr _tmpA09=({const char*_tmp568="bad attributes on parameter, ignoring";_tag_fat(_tmp568,sizeof(char),38U);});_tmp751.f1=_tmpA09;});_tmp751;});void*_tmp566[1U];_tmp566[0]=& _tmp567;({unsigned _tmpA0A=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA0A,_tag_fat(_tmp566,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->f1=0,_tmp569->f2=tq2,_tmp569->f3=t2;_tmp569;}));
# 2199
goto _LL0;}}}}case 254U: _LL1F7: _LL1F8:
# 2203 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 255U: _LL1F9: _LL1FA:
# 2207 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));({struct _fat_ptr*_tmpA0C=({struct _fat_ptr*_tmp56A=_cycalloc(sizeof(*_tmp56A));({struct _fat_ptr _tmpA0B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp56A=_tmpA0B;});_tmp56A;});_tmp56B->hd=_tmpA0C;}),_tmp56B->tl=0;_tmp56B;}));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2209 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));({struct _fat_ptr*_tmpA0F=({struct _fat_ptr*_tmp56C=_cycalloc(sizeof(*_tmp56C));({struct _fat_ptr _tmpA0E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp56C=_tmpA0E;});_tmp56C;});_tmp56D->hd=_tmpA0F;}),({struct Cyc_List_List*_tmpA0D=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp56D->tl=_tmpA0D;});_tmp56D;}));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2213 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 258U: _LL1FF: _LL200:
# 2214 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 259U: _LL201: _LL202:
# 2219 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA10=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->tag=36U,_tmp56E->f1=0,_tmp56E->f2=0;_tmp56E;});Cyc_Absyn_new_exp(_tmpA10,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 260U: _LL203: _LL204:
# 2221 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA12=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=36U,_tmp56F->f1=0,({struct Cyc_List_List*_tmpA11=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp56F->f2=_tmpA11;});_tmp56F;});Cyc_Absyn_new_exp(_tmpA12,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 261U: _LL205: _LL206:
# 2223 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA14=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->tag=36U,_tmp570->f1=0,({struct Cyc_List_List*_tmpA13=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp570->f2=_tmpA13;});_tmp570;});Cyc_Absyn_new_exp(_tmpA14,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208: {
# 2225 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp571=({unsigned _tmpA19=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA18=({struct _tuple0*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA16=({struct _fat_ptr*_tmp573=_cycalloc(sizeof(*_tmp573));({struct _fat_ptr _tmpA15=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp573=_tmpA15;});_tmp573;});_tmp574->f2=_tmpA16;});_tmp574;});void*_tmpA17=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA19,_tmpA18,_tmpA17,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2225
struct Cyc_Absyn_Vardecl*vd=_tmp571;
# 2228
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA1C=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=27U,_tmp572->f1=vd,({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp572->f2=_tmpA1B;}),({struct Cyc_Absyn_Exp*_tmpA1A=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp572->f3=_tmpA1A;}),_tmp572->f4=0;_tmp572;});Cyc_Absyn_new_exp(_tmpA1C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2231
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2233 "parse.y"
void*_tmp575=({struct _tuple8*_tmpA1D=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA1D,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp575;
yyval=Cyc_Exp_tok(({void*_tmpA1F=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=28U,({struct Cyc_Absyn_Exp*_tmpA1E=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp576->f1=_tmpA1E;}),_tmp576->f2=t,_tmp576->f3=0;_tmp576;});Cyc_Absyn_new_exp(_tmpA1F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2236
goto _LL0;}case 264U: _LL20B: _LL20C:
# 2241 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp578=_cycalloc(sizeof(*_tmp578));({struct _tuple35*_tmpA21=({struct _tuple35*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->f1=0,({struct Cyc_Absyn_Exp*_tmpA20=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp577->f2=_tmpA20;});_tmp577;});_tmp578->hd=_tmpA21;}),_tmp578->tl=0;_tmp578;}));
goto _LL0;case 265U: _LL20D: _LL20E:
# 2243 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57A=_cycalloc(sizeof(*_tmp57A));({struct _tuple35*_tmpA24=({struct _tuple35*_tmp579=_cycalloc(sizeof(*_tmp579));({struct Cyc_List_List*_tmpA23=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp579->f1=_tmpA23;}),({struct Cyc_Absyn_Exp*_tmpA22=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp579->f2=_tmpA22;});_tmp579;});_tmp57A->hd=_tmpA24;}),_tmp57A->tl=0;_tmp57A;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2245 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));({struct _tuple35*_tmpA27=({struct _tuple35*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->f1=0,({struct Cyc_Absyn_Exp*_tmpA26=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp57B->f2=_tmpA26;});_tmp57B;});_tmp57C->hd=_tmpA27;}),({struct Cyc_List_List*_tmpA25=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp57C->tl=_tmpA25;});_tmp57C;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2247 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct _tuple35*_tmpA2B=({struct _tuple35*_tmp57D=_cycalloc(sizeof(*_tmp57D));({struct Cyc_List_List*_tmpA2A=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp57D->f1=_tmpA2A;}),({struct Cyc_Absyn_Exp*_tmpA29=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp57D->f2=_tmpA29;});_tmp57D;});_tmp57E->hd=_tmpA2B;}),({struct Cyc_List_List*_tmpA28=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp57E->tl=_tmpA28;});_tmp57E;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2251 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 269U: _LL215: _LL216:
# 2252 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp581=_cycalloc(sizeof(*_tmp581));({void*_tmpA2E=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->tag=1U,({struct _fat_ptr*_tmpA2D=({struct _fat_ptr*_tmp57F=_cycalloc(sizeof(*_tmp57F));({struct _fat_ptr _tmpA2C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp57F=_tmpA2C;});_tmp57F;});_tmp580->f1=_tmpA2D;});_tmp580;});_tmp581->hd=_tmpA2E;}),_tmp581->tl=0;_tmp581;}));
goto _LL0;case 270U: _LL217: _LL218:
# 2257 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp582=_cycalloc(sizeof(*_tmp582));({void*_tmpA2F=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp582->hd=_tmpA2F;}),_tmp582->tl=0;_tmp582;}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2258 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));({void*_tmpA31=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp583->hd=_tmpA31;}),({struct Cyc_List_List*_tmpA30=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp583->tl=_tmpA30;});_tmp583;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2262 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->tag=0U,({struct Cyc_Absyn_Exp*_tmpA32=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp584->f1=_tmpA32;});_tmp584;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2263 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->tag=1U,({struct _fat_ptr*_tmpA34=({struct _fat_ptr*_tmp585=_cycalloc(sizeof(*_tmp585));({struct _fat_ptr _tmpA33=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp585=_tmpA33;});_tmp585;});_tmp586->f1=_tmpA34;});_tmp586;}));
goto _LL0;case 274U: _LL21F: _LL220: {
# 2268 "parse.y"
struct _tuple26 _tmp587=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp587;struct _tuple26 _tmp588=_stmttmp24;struct Cyc_List_List*_tmp58B;struct Cyc_Parse_Type_specifier _tmp58A;struct Cyc_Absyn_Tqual _tmp589;_LL48F: _tmp589=_tmp588.f1;_tmp58A=_tmp588.f2;_tmp58B=_tmp588.f3;_LL490: {struct Cyc_Absyn_Tqual tq=_tmp589;struct Cyc_Parse_Type_specifier tss=_tmp58A;struct Cyc_List_List*atts=_tmp58B;
void*_tmp58C=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp58C;
if(atts != 0)
({void*_tmp58D=0U;({unsigned _tmpA36=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA35=({const char*_tmp58E="ignoring attributes in type";_tag_fat(_tmp58E,sizeof(char),28U);});Cyc_Warn_warn(_tmpA36,_tmpA35,_tag_fat(_tmp58D,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->f1=0,_tmp58F->f2=tq,_tmp58F->f3=t;_tmp58F;}));
# 2274
goto _LL0;}}case 275U: _LL221: _LL222: {
# 2275 "parse.y"
struct _tuple26 _tmp590=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp590;struct _tuple26 _tmp591=_stmttmp25;struct Cyc_List_List*_tmp594;struct Cyc_Parse_Type_specifier _tmp593;struct Cyc_Absyn_Tqual _tmp592;_LL492: _tmp592=_tmp591.f1;_tmp593=_tmp591.f2;_tmp594=_tmp591.f3;_LL493: {struct Cyc_Absyn_Tqual tq=_tmp592;struct Cyc_Parse_Type_specifier tss=_tmp593;struct Cyc_List_List*atts=_tmp594;
void*_tmp595=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp595;
struct Cyc_List_List*_tmp596=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp596;
struct _tuple14 _tmp597=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp597;
if(t_info.f3 != 0)
# 2281
({void*_tmp598=0U;({unsigned _tmpA38=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA37=({const char*_tmp599="bad type params, ignoring";_tag_fat(_tmp599,sizeof(char),26U);});Cyc_Warn_warn(_tmpA38,_tmpA37,_tag_fat(_tmp598,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp59A=0U;({unsigned _tmpA3A=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA39=({const char*_tmp59B="bad specifiers, ignoring";_tag_fat(_tmp59B,sizeof(char),25U);});Cyc_Warn_warn(_tmpA3A,_tmpA39,_tag_fat(_tmp59A,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->f1=0,_tmp59C->f2=t_info.f1,_tmp59C->f3=t_info.f2;_tmp59C;}));
# 2286
goto _LL0;}}case 276U: _LL223: _LL224:
# 2289 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA3B=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA3B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 277U: _LL225: _LL226:
# 2290 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 278U: _LL227: _LL228:
# 2291 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 279U: _LL229: _LL22A:
# 2292 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2293 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));({void*_tmpA3D=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp59D->hd=_tmpA3D;}),({struct Cyc_List_List*_tmpA3C=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp59D->tl=_tmpA3C;});_tmp59D;})));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2299 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp59E=_cycalloc(sizeof(*_tmp59E));({void*_tmpA3E=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp59E->hd=_tmpA3E;}),_tmp59E->tl=0;_tmp59E;}));
goto _LL0;case 282U: _LL22F: _LL230:
# 2300 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp59F=_cycalloc(sizeof(*_tmp59F));({void*_tmpA40=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp59F->hd=_tmpA40;}),({struct Cyc_List_List*_tmpA3F=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp59F->tl=_tmpA3F;});_tmp59F;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2305 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp752;({struct Cyc_List_List*_tmpA41=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp752.tms=_tmpA41;});_tmp752;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2307 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 285U: _LL235: _LL236:
# 2309 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp753;({struct Cyc_List_List*_tmpA43=({struct Cyc_List_List*_tmpA42=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA42,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp753.tms=_tmpA43;});_tmp753;}));
goto _LL0;case 286U: _LL237: _LL238:
# 2314 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 287U: _LL239: _LL23A:
# 2316 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp754;({struct Cyc_List_List*_tmpA46=({struct Cyc_List_List*_tmp5A1=_region_malloc(yyr,sizeof(*_tmp5A1));({void*_tmpA45=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A0=_region_malloc(yyr,sizeof(*_tmp5A0));_tmp5A0->tag=0U,({void*_tmpA44=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5A0->f1=_tmpA44;}),_tmp5A0->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5A0;});_tmp5A1->hd=_tmpA45;}),_tmp5A1->tl=0;_tmp5A1;});_tmp754.tms=_tmpA46;});_tmp754;}));
goto _LL0;case 288U: _LL23B: _LL23C:
# 2318 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp755;({struct Cyc_List_List*_tmpA4A=({struct Cyc_List_List*_tmp5A3=_region_malloc(yyr,sizeof(*_tmp5A3));({void*_tmpA49=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A2=_region_malloc(yyr,sizeof(*_tmp5A2));_tmp5A2->tag=0U,({void*_tmpA48=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A2->f1=_tmpA48;}),_tmp5A2->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A2;});_tmp5A3->hd=_tmpA49;}),({struct Cyc_List_List*_tmpA47=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5A3->tl=_tmpA47;});_tmp5A3;});_tmp755.tms=_tmpA4A;});_tmp755;}));
goto _LL0;case 289U: _LL23D: _LL23E:
# 2320 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp756;({struct Cyc_List_List*_tmpA4E=({struct Cyc_List_List*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));({void*_tmpA4D=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A4=_region_malloc(yyr,sizeof(*_tmp5A4));_tmp5A4->tag=1U,({struct Cyc_Absyn_Exp*_tmpA4C=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5A4->f1=_tmpA4C;}),({void*_tmpA4B=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A4->f2=_tmpA4B;}),_tmp5A4->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A4;});_tmp5A5->hd=_tmpA4D;}),_tmp5A5->tl=0;_tmp5A5;});_tmp756.tms=_tmpA4E;});_tmp756;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2322 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp757;({struct Cyc_List_List*_tmpA53=({struct Cyc_List_List*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));({void*_tmpA52=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));_tmp5A6->tag=1U,({struct Cyc_Absyn_Exp*_tmpA51=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5A6->f1=_tmpA51;}),({void*_tmpA50=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5A6->f2=_tmpA50;}),_tmp5A6->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5A6;});_tmp5A7->hd=_tmpA52;}),({
struct Cyc_List_List*_tmpA4F=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5A7->tl=_tmpA4F;});_tmp5A7;});
# 2322
_tmp757.tms=_tmpA53;});_tmp757;}));
# 2325
goto _LL0;case 291U: _LL241: _LL242:
# 2326 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp758;({struct Cyc_List_List*_tmpA5A=({struct Cyc_List_List*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));({void*_tmpA59=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));_tmp5A9->tag=3U,({void*_tmpA58=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));_tmp5A8->tag=1U,_tmp5A8->f1=0,_tmp5A8->f2=0,_tmp5A8->f3=0,({void*_tmpA57=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5A8->f4=_tmpA57;}),({struct Cyc_List_List*_tmpA56=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5A8->f5=_tmpA56;}),({struct Cyc_Absyn_Exp*_tmpA55=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5A8->f6=_tmpA55;}),({struct Cyc_Absyn_Exp*_tmpA54=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5A8->f7=_tmpA54;});_tmp5A8;});_tmp5A9->f1=_tmpA58;});_tmp5A9;});_tmp5AA->hd=_tmpA59;}),_tmp5AA->tl=0;_tmp5AA;});_tmp758.tms=_tmpA5A;});_tmp758;}));
# 2328
goto _LL0;case 292U: _LL243: _LL244: {
# 2329 "parse.y"
struct _tuple27*_tmp5AB=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp26=_tmp5AB;struct _tuple27*_tmp5AC=_stmttmp26;struct Cyc_List_List*_tmp5B1;void*_tmp5B0;struct Cyc_Absyn_VarargInfo*_tmp5AF;int _tmp5AE;struct Cyc_List_List*_tmp5AD;_LL495: _tmp5AD=_tmp5AC->f1;_tmp5AE=_tmp5AC->f2;_tmp5AF=_tmp5AC->f3;_tmp5B0=_tmp5AC->f4;_tmp5B1=_tmp5AC->f5;_LL496: {struct Cyc_List_List*lis=_tmp5AD;int b=_tmp5AE;struct Cyc_Absyn_VarargInfo*c=_tmp5AF;void*eff=_tmp5B0;struct Cyc_List_List*po=_tmp5B1;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp759;({struct Cyc_List_List*_tmpA5F=({struct Cyc_List_List*_tmp5B4=_region_malloc(yyr,sizeof(*_tmp5B4));({void*_tmpA5E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B3=_region_malloc(yyr,sizeof(*_tmp5B3));_tmp5B3->tag=3U,({void*_tmpA5D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B2=_region_malloc(yyr,sizeof(*_tmp5B2));_tmp5B2->tag=1U,_tmp5B2->f1=lis,_tmp5B2->f2=b,_tmp5B2->f3=c,_tmp5B2->f4=eff,_tmp5B2->f5=po,({struct Cyc_Absyn_Exp*_tmpA5C=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5B2->f6=_tmpA5C;}),({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B2->f7=_tmpA5B;});_tmp5B2;});_tmp5B3->f1=_tmpA5D;});_tmp5B3;});_tmp5B4->hd=_tmpA5E;}),_tmp5B4->tl=0;_tmp5B4;});_tmp759.tms=_tmpA5F;});_tmp759;}));
# 2332
goto _LL0;}}case 293U: _LL245: _LL246:
# 2333 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75A;({struct Cyc_List_List*_tmpA67=({struct Cyc_List_List*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));({void*_tmpA66=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B6=_region_malloc(yyr,sizeof(*_tmp5B6));_tmp5B6->tag=3U,({void*_tmpA65=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B5=_region_malloc(yyr,sizeof(*_tmp5B5));_tmp5B5->tag=1U,_tmp5B5->f1=0,_tmp5B5->f2=0,_tmp5B5->f3=0,({void*_tmpA64=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5B5->f4=_tmpA64;}),({struct Cyc_List_List*_tmpA63=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5B5->f5=_tmpA63;}),({struct Cyc_Absyn_Exp*_tmpA62=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5B5->f6=_tmpA62;}),({struct Cyc_Absyn_Exp*_tmpA61=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5B5->f7=_tmpA61;});_tmp5B5;});_tmp5B6->f1=_tmpA65;});_tmp5B6;});_tmp5B7->hd=_tmpA66;}),({
struct Cyc_List_List*_tmpA60=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5B7->tl=_tmpA60;});_tmp5B7;});
# 2333
_tmp75A.tms=_tmpA67;});_tmp75A;}));
# 2336
goto _LL0;case 294U: _LL247: _LL248: {
# 2337 "parse.y"
struct _tuple27*_tmp5B8=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp27=_tmp5B8;struct _tuple27*_tmp5B9=_stmttmp27;struct Cyc_List_List*_tmp5BE;void*_tmp5BD;struct Cyc_Absyn_VarargInfo*_tmp5BC;int _tmp5BB;struct Cyc_List_List*_tmp5BA;_LL498: _tmp5BA=_tmp5B9->f1;_tmp5BB=_tmp5B9->f2;_tmp5BC=_tmp5B9->f3;_tmp5BD=_tmp5B9->f4;_tmp5BE=_tmp5B9->f5;_LL499: {struct Cyc_List_List*lis=_tmp5BA;int b=_tmp5BB;struct Cyc_Absyn_VarargInfo*c=_tmp5BC;void*eff=_tmp5BD;struct Cyc_List_List*po=_tmp5BE;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75B;({struct Cyc_List_List*_tmpA6D=({struct Cyc_List_List*_tmp5C1=_region_malloc(yyr,sizeof(*_tmp5C1));({void*_tmpA6C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C0=_region_malloc(yyr,sizeof(*_tmp5C0));_tmp5C0->tag=3U,({void*_tmpA6B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5BF=_region_malloc(yyr,sizeof(*_tmp5BF));_tmp5BF->tag=1U,_tmp5BF->f1=lis,_tmp5BF->f2=b,_tmp5BF->f3=c,_tmp5BF->f4=eff,_tmp5BF->f5=po,({
struct Cyc_Absyn_Exp*_tmpA6A=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5BF->f6=_tmpA6A;}),({struct Cyc_Absyn_Exp*_tmpA69=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5BF->f7=_tmpA69;});_tmp5BF;});
# 2338
_tmp5C0->f1=_tmpA6B;});_tmp5C0;});_tmp5C1->hd=_tmpA6C;}),({
struct Cyc_List_List*_tmpA68=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C1->tl=_tmpA68;});_tmp5C1;});
# 2338
_tmp75B.tms=_tmpA6D;});_tmp75B;}));
# 2341
goto _LL0;}}case 295U: _LL249: _LL24A: {
# 2343
struct Cyc_List_List*_tmp5C2=({unsigned _tmpA6E=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA6E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5C2;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75C;({struct Cyc_List_List*_tmpA71=({struct Cyc_List_List*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));({void*_tmpA70=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));_tmp5C3->tag=4U,_tmp5C3->f1=ts,_tmp5C3->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5C3->f3=0;_tmp5C3;});_tmp5C4->hd=_tmpA70;}),({
struct Cyc_List_List*_tmpA6F=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C4->tl=_tmpA6F;});_tmp5C4;});
# 2344
_tmp75C.tms=_tmpA71;});_tmp75C;}));
# 2347
goto _LL0;}case 296U: _LL24B: _LL24C:
# 2348 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpA75=({struct Cyc_List_List*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));({void*_tmpA74=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));_tmp5C5->tag=5U,_tmp5C5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA73=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5C5->f2=_tmpA73;});_tmp5C5;});_tmp5C6->hd=_tmpA74;}),({struct Cyc_List_List*_tmpA72=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C6->tl=_tmpA72;});_tmp5C6;});_tmp75D.tms=_tmpA75;});_tmp75D;}));
# 2350
goto _LL0;case 297U: _LL24D: _LL24E:
# 2354 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298U: _LL24F: _LL250:
# 2355 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299U: _LL251: _LL252:
# 2356 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2357 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2358 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2359 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2365 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA79=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));_tmp5C8->tag=13U,({struct _fat_ptr*_tmpA78=({struct _fat_ptr*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));({struct _fat_ptr _tmpA77=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5C7=_tmpA77;});_tmp5C7;});_tmp5C8->f1=_tmpA78;}),({struct Cyc_Absyn_Stmt*_tmpA76=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5C8->f2=_tmpA76;});_tmp5C8;});Cyc_Absyn_new_stmt(_tmpA79,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304U: _LL25B: _LL25C:
# 2369 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305U: _LL25D: _LL25E:
# 2370 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA7A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 306U: _LL25F: _LL260:
# 2375 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 307U: _LL261: _LL262:
# 2376 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 308U: _LL263: _LL264:
# 2381 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7B=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7B,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 309U: _LL265: _LL266:
# 2382 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7C=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7C,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2383 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA81=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->tag=13U,({struct _fat_ptr*_tmpA80=({struct _fat_ptr*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));({struct _fat_ptr _tmpA7F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5C9=_tmpA7F;});_tmp5C9;});_tmp5CA->f1=_tmpA80;}),({struct Cyc_Absyn_Stmt*_tmpA7E=({struct Cyc_List_List*_tmpA7D=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA7D,Cyc_Absyn_skip_stmt(0U));});_tmp5CA->f2=_tmpA7E;});_tmp5CA;});Cyc_Absyn_new_stmt(_tmpA81,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2385
goto _LL0;case 311U: _LL269: _LL26A:
# 2385 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA86=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->tag=13U,({struct _fat_ptr*_tmpA85=({struct _fat_ptr*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));({struct _fat_ptr _tmpA84=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CB=_tmpA84;});_tmp5CB;});_tmp5CC->f1=_tmpA85;}),({struct Cyc_Absyn_Stmt*_tmpA83=({struct Cyc_List_List*_tmpA82=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA82,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5CC->f2=_tmpA83;});_tmp5CC;});Cyc_Absyn_new_stmt(_tmpA86,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2387
goto _LL0;case 312U: _LL26B: _LL26C:
# 2387 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 313U: _LL26D: _LL26E:
# 2388 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA88=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA87=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA88,_tmpA87,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 314U: _LL26F: _LL270:
# 2389 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA8B=({void*_tmpA8A=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA89=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5CD->f1=_tmpA89;});_tmp5CD;});Cyc_Absyn_new_decl(_tmpA8A,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA8B,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 315U: _LL271: _LL272:
# 2392 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA8E=({void*_tmpA8D=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA8C=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5CE->f1=_tmpA8C;});_tmp5CE;});Cyc_Absyn_new_decl(_tmpA8D,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA8E,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 316U: _LL273: _LL274:
# 2397 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA91=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA90=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA8F=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA91,_tmpA90,_tmpA8F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317U: _LL275: _LL276:
# 2399 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA94=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA93=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA92=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpA94,_tmpA93,_tmpA92,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318U: _LL277: _LL278:
# 2405 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA96=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpA95=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpA96,_tmpA95,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A: {
# 2408
struct Cyc_Absyn_Exp*_tmp5CF=({struct _tuple0*_tmpA97=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpA97,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5CF;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA99=e;struct Cyc_List_List*_tmpA98=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpA99,_tmpA98,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2412
struct Cyc_Absyn_Exp*_tmp5D0=({struct Cyc_List_List*_tmpA9A=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpA9A,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D0;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9C=e;struct Cyc_List_List*_tmpA9B=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpA9C,_tmpA9B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2415
goto _LL0;}case 321U: _LL27D: _LL27E:
# 2418 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA9E=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpA9D=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpA9E,_tmpA9D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 322U: _LL27F: _LL280:
# 2432 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 323U: _LL281: _LL282:
# 2435 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));({struct Cyc_Absyn_Switch_clause*_tmpAA2=({struct Cyc_Absyn_Switch_clause*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));({struct Cyc_Absyn_Pat*_tmpAA1=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5D1->pattern=_tmpAA1;}),_tmp5D1->pat_vars=0,_tmp5D1->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA0=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D1->body=_tmpAA0;}),_tmp5D1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D1;});
# 2435
_tmp5D2->hd=_tmpAA2;}),({
# 2437
struct Cyc_List_List*_tmpA9F=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D2->tl=_tmpA9F;});_tmp5D2;}));
goto _LL0;case 324U: _LL283: _LL284:
# 2439 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));({struct Cyc_Absyn_Switch_clause*_tmpAA6=({struct Cyc_Absyn_Switch_clause*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct Cyc_Absyn_Pat*_tmpAA5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D3->pattern=_tmpAA5;}),_tmp5D3->pat_vars=0,_tmp5D3->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA4=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5D3->body=_tmpAA4;}),_tmp5D3->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D3;});
# 2439
_tmp5D4->hd=_tmpAA6;}),({
# 2441
struct Cyc_List_List*_tmpAA3=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D4->tl=_tmpAA3;});_tmp5D4;}));
goto _LL0;case 325U: _LL285: _LL286:
# 2443 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct Cyc_Absyn_Switch_clause*_tmpAAA=({struct Cyc_Absyn_Switch_clause*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));({struct Cyc_Absyn_Pat*_tmpAA9=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D5->pattern=_tmpAA9;}),_tmp5D5->pat_vars=0,_tmp5D5->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAA8=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5D5->body=_tmpAA8;}),_tmp5D5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D5;});_tmp5D6->hd=_tmpAAA;}),({struct Cyc_List_List*_tmpAA7=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5D6->tl=_tmpAA7;});_tmp5D6;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2445 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct Cyc_Absyn_Switch_clause*_tmpAAF=({struct Cyc_Absyn_Switch_clause*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));({struct Cyc_Absyn_Pat*_tmpAAE=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D7->pattern=_tmpAAE;}),_tmp5D7->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5D7->where_clause=_tmpAAD;}),({
struct Cyc_Absyn_Stmt*_tmpAAC=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5D7->body=_tmpAAC;}),_tmp5D7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D7;});
# 2445
_tmp5D8->hd=_tmpAAF;}),({
# 2447
struct Cyc_List_List*_tmpAAB=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5D8->tl=_tmpAAB;});_tmp5D8;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2449 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct Cyc_Absyn_Switch_clause*_tmpAB4=({struct Cyc_Absyn_Switch_clause*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct Cyc_Absyn_Pat*_tmpAB3=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D9->pattern=_tmpAB3;}),_tmp5D9->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAB2=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5D9->where_clause=_tmpAB2;}),({struct Cyc_Absyn_Stmt*_tmpAB1=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5D9->body=_tmpAB1;}),_tmp5D9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D9;});_tmp5DA->hd=_tmpAB4;}),({struct Cyc_List_List*_tmpAB0=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5DA->tl=_tmpAB0;});_tmp5DA;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2456 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAB5=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpAB6,_tmpAB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2460 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAB8=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAB7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpAB8,_tmpAB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2464 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpABC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpABA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAB9=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2464
Cyc_Absyn_for_stmt(_tmpABC,_tmpABB,_tmpABA,_tmpAB9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2466
goto _LL0;case 331U: _LL291: _LL292:
# 2467 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpABD=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2467
Cyc_Absyn_for_stmt(_tmpAC0,_tmpABF,_tmpABE,_tmpABD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2469
goto _LL0;case 332U: _LL293: _LL294:
# 2470 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC1=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2470
Cyc_Absyn_for_stmt(_tmpAC4,_tmpAC3,_tmpAC2,_tmpAC1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2472
goto _LL0;case 333U: _LL295: _LL296:
# 2473 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC6=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAC5=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2473
Cyc_Absyn_for_stmt(_tmpAC8,_tmpAC7,_tmpAC6,_tmpAC5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2475
goto _LL0;case 334U: _LL297: _LL298:
# 2476 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpACB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC9=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2476
Cyc_Absyn_for_stmt(_tmpACC,_tmpACB,_tmpACA,_tmpAC9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2478
goto _LL0;case 335U: _LL299: _LL29A:
# 2479 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpACF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACE=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpACD=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2479
Cyc_Absyn_for_stmt(_tmpAD0,_tmpACF,_tmpACE,_tmpACD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2481
goto _LL0;case 336U: _LL29B: _LL29C:
# 2482 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD1=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2482
Cyc_Absyn_for_stmt(_tmpAD4,_tmpAD3,_tmpAD2,_tmpAD1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2484
goto _LL0;case 337U: _LL29D: _LL29E:
# 2485 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD6=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAD5=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2485
Cyc_Absyn_for_stmt(_tmpAD8,_tmpAD7,_tmpAD6,_tmpAD5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2487
goto _LL0;case 338U: _LL29F: _LL2A0: {
# 2488 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5DB=({struct Cyc_Absyn_Exp*_tmpADC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpADB=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD9=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2488
Cyc_Absyn_for_stmt(_tmpADC,_tmpADB,_tmpADA,_tmpAD9,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5DB;
# 2490
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpADD=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpADD,s);}));
# 2492
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2493 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5DC=({struct Cyc_Absyn_Exp*_tmpAE1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE0=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpADF=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADE=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAE1,_tmpAE0,_tmpADF,_tmpADE,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5DC;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAE2=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAE2,s);}));
# 2496
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2497 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5DD=({struct Cyc_Absyn_Exp*_tmpAE6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAE3=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAE6,_tmpAE5,_tmpAE4,_tmpAE3,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5DD;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAE7=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAE7,s);}));
# 2500
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2501 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5DE=({struct Cyc_Absyn_Exp*_tmpAEB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAEA=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAE9=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAE8=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpAEB,_tmpAEA,_tmpAE9,_tmpAE8,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5DE;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAEC=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAEC,s);}));
# 2504
goto _LL0;}case 342U: _LL2A7: _LL2A8:
# 2507 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpAEE=({struct _fat_ptr*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));({struct _fat_ptr _tmpAED=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5DF=_tmpAED;});_tmp5DF;});Cyc_Absyn_goto_stmt(_tmpAEE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 343U: _LL2A9: _LL2AA:
# 2508 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2509 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2510 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2511 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAEF=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpAEF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2513 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2514 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2516 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF0=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpAF0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2525 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2528
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2530 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAF1=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAF3,_tmpAF2,_tmpAF1,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2533
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2535 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpAF5,_tmpAF4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2538
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2540 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpAF7,_tmpAF6,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2543
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2545 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAF9,_tmpAF8,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2548
 yyval=(yyyvsp[0]).v;
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2550 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAFB,_tmpAFA,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2553
 yyval=(yyyvsp[0]).v;
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2555 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpAFD,_tmpAFC,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2558
 yyval=(yyyvsp[0]).v;
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2560 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpAFF,_tmpAFE,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2562 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB00=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpB01,_tmpB00,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2565
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2567 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB02=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB03,_tmpB02,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2569 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB05,_tmpB04,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2571 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB06=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB07,_tmpB06,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2573 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB08=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB09,_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2576
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2578 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB0B,_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2580 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB0D,_tmpB0C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2583
 yyval=(yyyvsp[0]).v;
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2585 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2587 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB11,_tmpB10,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2590
 yyval=(yyyvsp[0]).v;
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2592 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB12=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB13,_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2594 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB15,_tmpB14,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2596 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB17,_tmpB16,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2599
 yyval=(yyyvsp[0]).v;
goto _LL0;case 382U: _LL2F7: _LL2F8: {
# 2601 "parse.y"
void*_tmp5E0=({struct _tuple8*_tmpB18=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB18,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5E0;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB1A=t;struct Cyc_Absyn_Exp*_tmpB19=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB1A,_tmpB19,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2604
goto _LL0;}case 383U: _LL2F9: _LL2FA:
# 2607 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 384U: _LL2FB: _LL2FC:
# 2610
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2612 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB1C=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB1B=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB1C,_tmpB1B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 386U: _LL2FF: _LL300: {
# 2614 "parse.y"
void*_tmp5E1=({struct _tuple8*_tmpB1D=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB1D,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5E1;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2617
goto _LL0;}case 387U: _LL301: _LL302:
# 2618 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB1E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 388U: _LL303: _LL304:
# 2620 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB20=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB1F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB20,_tmpB1F,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 389U: _LL305: _LL306:
# 2625 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 390U: _LL307: _LL308:
# 2633 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 391U: _LL309: _LL30A:
# 2638 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 392U: _LL30B: _LL30C:
# 2640 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 393U: _LL30D: _LL30E: {
# 2642 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5E2=e->r;void*_stmttmp28=_tmp5E2;void*_tmp5E3=_stmttmp28;int _tmp5E5;struct _fat_ptr _tmp5E4;int _tmp5E7;enum Cyc_Absyn_Sign _tmp5E6;short _tmp5E9;enum Cyc_Absyn_Sign _tmp5E8;char _tmp5EB;enum Cyc_Absyn_Sign _tmp5EA;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).LongLong_c).tag){case 2U: _LL49B: _tmp5EA=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Char_c).val).f1;_tmp5EB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Char_c).val).f2;_LL49C: {enum Cyc_Absyn_Sign s=_tmp5EA;char i=_tmp5EB;
# 2646
yyval=Cyc_YY9(({void*_tmpB21=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->tag=11U,_tmp5EC->f1=i;_tmp5EC;});Cyc_Absyn_new_pat(_tmpB21,e->loc);}));goto _LL49A;}case 4U: _LL49D: _tmp5E8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Short_c).val).f1;_tmp5E9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Short_c).val).f2;_LL49E: {enum Cyc_Absyn_Sign s=_tmp5E8;short i=_tmp5E9;
# 2648
yyval=Cyc_YY9(({void*_tmpB22=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->tag=10U,_tmp5ED->f1=s,_tmp5ED->f2=(int)i;_tmp5ED;});Cyc_Absyn_new_pat(_tmpB22,e->loc);}));goto _LL49A;}case 5U: _LL49F: _tmp5E6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Int_c).val).f1;_tmp5E7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Int_c).val).f2;_LL4A0: {enum Cyc_Absyn_Sign s=_tmp5E6;int i=_tmp5E7;
# 2650
yyval=Cyc_YY9(({void*_tmpB23=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->tag=10U,_tmp5EE->f1=s,_tmp5EE->f2=i;_tmp5EE;});Cyc_Absyn_new_pat(_tmpB23,e->loc);}));goto _LL49A;}case 7U: _LL4A1: _tmp5E4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Float_c).val).f1;_tmp5E5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E3)->f1).Float_c).val).f2;_LL4A2: {struct _fat_ptr s=_tmp5E4;int i=_tmp5E5;
# 2652
yyval=Cyc_YY9(({void*_tmpB24=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->tag=12U,_tmp5EF->f1=s,_tmp5EF->f2=i;_tmp5EF;});Cyc_Absyn_new_pat(_tmpB24,e->loc);}));goto _LL49A;}case 1U: _LL4A3: _LL4A4:
# 2654
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL49A;case 8U: _LL4A5: _LL4A6:
 goto _LL4A8;case 9U: _LL4A7: _LL4A8:
# 2657
({void*_tmp5F0=0U;({unsigned _tmpB26=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB25=({const char*_tmp5F1="strings cannot occur within patterns";_tag_fat(_tmp5F1,sizeof(char),37U);});Cyc_Warn_err(_tmpB26,_tmpB25,_tag_fat(_tmp5F0,sizeof(void*),0U));});});goto _LL49A;case 6U: _LL4A9: _LL4AA:
# 2659
({void*_tmp5F2=0U;({unsigned _tmpB28=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB27=({const char*_tmp5F3="long long's in patterns not yet implemented";_tag_fat(_tmp5F3,sizeof(char),44U);});Cyc_Warn_err(_tmpB28,_tmpB27,_tag_fat(_tmp5F2,sizeof(void*),0U));});});goto _LL49A;default: goto _LL4AB;}else{_LL4AB: _LL4AC:
# 2661
({void*_tmp5F4=0U;({unsigned _tmpB2A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB29=({const char*_tmp5F5="bad constant in case";_tag_fat(_tmp5F5,sizeof(char),21U);});Cyc_Warn_err(_tmpB2A,_tmpB29,_tag_fat(_tmp5F4,sizeof(void*),0U));});});}_LL49A:;}
# 2664
goto _LL0;}case 394U: _LL30F: _LL310:
# 2665 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB2C=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=15U,({struct _tuple0*_tmpB2B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp5F6->f1=_tmpB2B;});_tmp5F6;});Cyc_Absyn_new_pat(_tmpB2C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 395U: _LL311: _LL312:
# 2667 "parse.y"
 if(({struct _fat_ptr _tmpB2D=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB2D,({const char*_tmp5F7="as";_tag_fat(_tmp5F7,sizeof(char),3U);}));})!= 0)
({void*_tmp5F8=0U;({unsigned _tmpB2F=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB2E=({const char*_tmp5F9="expecting `as'";_tag_fat(_tmp5F9,sizeof(char),15U);});Cyc_Warn_err(_tmpB2F,_tmpB2E,_tag_fat(_tmp5F8,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB36=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB35=({unsigned _tmpB34=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB33=({struct _tuple0*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB32=({struct _fat_ptr*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct _fat_ptr _tmpB31=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5FA=_tmpB31;});_tmp5FA;});_tmp5FB->f2=_tmpB32;});_tmp5FB;});Cyc_Absyn_new_vardecl(_tmpB34,_tmpB33,Cyc_Absyn_void_type,0);});_tmp5FC->f1=_tmpB35;}),({
struct Cyc_Absyn_Pat*_tmpB30=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp5FC->f2=_tmpB30;});_tmp5FC;});
# 2669
Cyc_Absyn_new_pat(_tmpB36,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2672
goto _LL0;case 396U: _LL313: _LL314:
# 2673 "parse.y"
 if(({struct _fat_ptr _tmpB37=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB37,({const char*_tmp5FD="alias";_tag_fat(_tmp5FD,sizeof(char),6U);}));})!= 0)
({void*_tmp5FE=0U;({unsigned _tmpB39=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB38=({const char*_tmp5FF="expecting `alias'";_tag_fat(_tmp5FF,sizeof(char),18U);});Cyc_Warn_err(_tmpB39,_tmpB38,_tag_fat(_tmp5FE,sizeof(void*),0U));});});{
int _tmp600=((yyyvsp[0]).l).first_line;int location=_tmp600;
({struct _fat_ptr _tmpB3A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB3A,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp606=_cycalloc(sizeof(*_tmp606));({struct _fat_ptr*_tmpB3D=({struct _fat_ptr*_tmp604=_cycalloc(sizeof(*_tmp604));({struct _fat_ptr _tmpB3C=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp604=_tmpB3C;});_tmp604;});_tmp606->name=_tmpB3D;}),_tmp606->identity=- 1,({void*_tmpB3B=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->tag=0U,_tmp605->f1=& Cyc_Kinds_rk;_tmp605;});_tmp606->kind=_tmpB3B;});_tmp606;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB42=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB41=({struct _tuple0*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB3F=({struct _fat_ptr*_tmp602=_cycalloc(sizeof(*_tmp602));({struct _fat_ptr _tmpB3E=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp602=_tmpB3E;});_tmp602;});_tmp603->f2=_tmpB3F;});_tmp603;});Cyc_Absyn_new_vardecl(_tmpB42,_tmpB41,({
struct _tuple8*_tmpB40=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB40,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB43=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=2U,_tmp601->f1=tv,_tmp601->f2=vd;_tmp601;});Cyc_Absyn_new_pat(_tmpB43,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2682
goto _LL0;}}case 397U: _LL315: _LL316:
# 2683 "parse.y"
 if(({struct _fat_ptr _tmpB44=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB44,({const char*_tmp607="alias";_tag_fat(_tmp607,sizeof(char),6U);}));})!= 0)
({void*_tmp608=0U;({unsigned _tmpB46=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB45=({const char*_tmp609="expecting `alias'";_tag_fat(_tmp609,sizeof(char),18U);});Cyc_Warn_err(_tmpB46,_tmpB45,_tag_fat(_tmp608,sizeof(void*),0U));});});{
int _tmp60A=((yyyvsp[0]).l).first_line;int location=_tmp60A;
({struct _fat_ptr _tmpB47=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpB47,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp610=_cycalloc(sizeof(*_tmp610));({struct _fat_ptr*_tmpB4A=({struct _fat_ptr*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct _fat_ptr _tmpB49=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60E=_tmpB49;});_tmp60E;});_tmp610->name=_tmpB4A;}),_tmp610->identity=- 1,({void*_tmpB48=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->tag=0U,_tmp60F->f1=& Cyc_Kinds_rk;_tmp60F;});_tmp610->kind=_tmpB48;});_tmp610;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB4F=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB4E=({struct _tuple0*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB4C=({struct _fat_ptr*_tmp60C=_cycalloc(sizeof(*_tmp60C));({struct _fat_ptr _tmpB4B=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp60C=_tmpB4B;});_tmp60C;});_tmp60D->f2=_tmpB4C;});_tmp60D;});Cyc_Absyn_new_vardecl(_tmpB4F,_tmpB4E,({
struct _tuple8*_tmpB4D=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB4D,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB50=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=2U,_tmp60B->f1=tv,_tmp60B->f2=vd;_tmp60B;});Cyc_Absyn_new_pat(_tmpB50,(unsigned)location);}));
# 2692
goto _LL0;}}case 398U: _LL317: _LL318: {
# 2693 "parse.y"
struct _tuple23 _tmp611=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp29=_tmp611;struct _tuple23 _tmp612=_stmttmp29;int _tmp614;struct Cyc_List_List*_tmp613;_LL4AE: _tmp613=_tmp612.f1;_tmp614=_tmp612.f2;_LL4AF: {struct Cyc_List_List*ps=_tmp613;int dots=_tmp614;
yyval=Cyc_YY9(({void*_tmpB51=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=5U,_tmp615->f1=ps,_tmp615->f2=dots;_tmp615;});Cyc_Absyn_new_pat(_tmpB51,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2696
goto _LL0;}}case 399U: _LL319: _LL31A: {
# 2697 "parse.y"
struct _tuple23 _tmp616=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp616;struct _tuple23 _tmp617=_stmttmp2A;int _tmp619;struct Cyc_List_List*_tmp618;_LL4B1: _tmp618=_tmp617.f1;_tmp619=_tmp617.f2;_LL4B2: {struct Cyc_List_List*ps=_tmp618;int dots=_tmp619;
yyval=Cyc_YY9(({void*_tmpB53=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=16U,({struct _tuple0*_tmpB52=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp61A->f1=_tmpB52;}),_tmp61A->f2=ps,_tmp61A->f3=dots;_tmp61A;});Cyc_Absyn_new_pat(_tmpB53,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2700
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2701 "parse.y"
struct _tuple23 _tmp61B=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2B=_tmp61B;struct _tuple23 _tmp61C=_stmttmp2B;int _tmp61E;struct Cyc_List_List*_tmp61D;_LL4B4: _tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;_LL4B5: {struct Cyc_List_List*fps=_tmp61D;int dots=_tmp61E;
struct Cyc_List_List*_tmp61F=({unsigned _tmpB54=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB54,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp61F;
yyval=Cyc_YY9(({void*_tmpB57=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB56=({union Cyc_Absyn_AggrInfo*_tmp620=_cycalloc(sizeof(*_tmp620));({union Cyc_Absyn_AggrInfo _tmpB55=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp620=_tmpB55;});_tmp620;});_tmp621->f1=_tmpB56;}),_tmp621->f2=exist_ts,_tmp621->f3=fps,_tmp621->f4=dots;_tmp621;});Cyc_Absyn_new_pat(_tmpB57,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2706
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2707 "parse.y"
struct _tuple23 _tmp622=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp622;struct _tuple23 _tmp623=_stmttmp2C;int _tmp625;struct Cyc_List_List*_tmp624;_LL4B7: _tmp624=_tmp623.f1;_tmp625=_tmp623.f2;_LL4B8: {struct Cyc_List_List*fps=_tmp624;int dots=_tmp625;
struct Cyc_List_List*_tmp626=({unsigned _tmpB58=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB58,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp626;
yyval=Cyc_YY9(({void*_tmpB59=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=7U,_tmp627->f1=0,_tmp627->f2=exist_ts,_tmp627->f3=fps,_tmp627->f4=dots;_tmp627;});Cyc_Absyn_new_pat(_tmpB59,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2711
goto _LL0;}}case 402U: _LL31F: _LL320:
# 2712 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB5B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5A=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp628->f1=_tmpB5A;});_tmp628;});Cyc_Absyn_new_pat(_tmpB5B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 403U: _LL321: _LL322:
# 2714 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB5F=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5E=({void*_tmpB5D=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5C=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp629->f1=_tmpB5C;});_tmp629;});Cyc_Absyn_new_pat(_tmpB5D,(unsigned)((yyyvsp[0]).l).first_line);});_tmp62A->f1=_tmpB5E;});_tmp62A;});Cyc_Absyn_new_pat(_tmpB5F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404U: _LL323: _LL324:
# 2716 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB66=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB65=({unsigned _tmpB64=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB63=({struct _tuple0*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB62=({struct _fat_ptr*_tmp62B=_cycalloc(sizeof(*_tmp62B));({struct _fat_ptr _tmpB61=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp62B=_tmpB61;});_tmp62B;});_tmp62C->f2=_tmpB62;});_tmp62C;});Cyc_Absyn_new_vardecl(_tmpB64,_tmpB63,Cyc_Absyn_void_type,0);});_tmp62D->f1=_tmpB65;}),({
# 2718
struct Cyc_Absyn_Pat*_tmpB60=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp62D->f2=_tmpB60;});_tmp62D;});
# 2716
Cyc_Absyn_new_pat(_tmpB66,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2720
goto _LL0;case 405U: _LL325: _LL326:
# 2721 "parse.y"
 if(({struct _fat_ptr _tmpB67=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB67,({const char*_tmp62E="as";_tag_fat(_tmp62E,sizeof(char),3U);}));})!= 0)
({void*_tmp62F=0U;({unsigned _tmpB69=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB68=({const char*_tmp630="expecting `as'";_tag_fat(_tmp630,sizeof(char),15U);});Cyc_Warn_err(_tmpB69,_tmpB68,_tag_fat(_tmp62F,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB70=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB6F=({unsigned _tmpB6E=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB6D=({struct _tuple0*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB6C=({struct _fat_ptr*_tmp631=_cycalloc(sizeof(*_tmp631));({struct _fat_ptr _tmpB6B=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp631=_tmpB6B;});_tmp631;});_tmp632->f2=_tmpB6C;});_tmp632;});Cyc_Absyn_new_vardecl(_tmpB6E,_tmpB6D,Cyc_Absyn_void_type,0);});_tmp633->f1=_tmpB6F;}),({
# 2725
struct Cyc_Absyn_Pat*_tmpB6A=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp633->f2=_tmpB6A;});_tmp633;});
# 2723
Cyc_Absyn_new_pat(_tmpB70,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2727
goto _LL0;case 406U: _LL327: _LL328: {
# 2728 "parse.y"
void*_tmp634=({struct _fat_ptr _tmpB71=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB71,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp634;
yyval=Cyc_YY9(({void*_tmpB78=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB77=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp637->f1=_tmpB77;}),({
struct Cyc_Absyn_Vardecl*_tmpB76=({unsigned _tmpB75=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB74=({struct _tuple0*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB73=({struct _fat_ptr*_tmp635=_cycalloc(sizeof(*_tmp635));({struct _fat_ptr _tmpB72=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp635=_tmpB72;});_tmp635;});_tmp636->f2=_tmpB73;});_tmp636;});Cyc_Absyn_new_vardecl(_tmpB75,_tmpB74,
Cyc_Absyn_tag_type(tag),0);});
# 2730
_tmp637->f2=_tmpB76;});_tmp637;});
# 2729
Cyc_Absyn_new_pat(_tmpB78,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2733
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2734 "parse.y"
struct Cyc_Absyn_Tvar*_tmp638=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp638;
yyval=Cyc_YY9(({void*_tmpB7E=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tag=4U,_tmp63B->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB7D=({unsigned _tmpB7C=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB7B=({struct _tuple0*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB7A=({struct _fat_ptr*_tmp639=_cycalloc(sizeof(*_tmp639));({struct _fat_ptr _tmpB79=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp639=_tmpB79;});_tmp639;});_tmp63A->f2=_tmpB7A;});_tmp63A;});Cyc_Absyn_new_vardecl(_tmpB7C,_tmpB7B,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2736
_tmp63B->f2=_tmpB7D;});_tmp63B;});
# 2735
Cyc_Absyn_new_pat(_tmpB7E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2739
goto _LL0;}case 408U: _LL32B: _LL32C:
# 2742 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct Cyc_List_List*_tmpB7F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp63C->f1=_tmpB7F;}),_tmp63C->f2=0;_tmp63C;}));
goto _LL0;case 409U: _LL32D: _LL32E:
# 2743 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp63D=_cycalloc(sizeof(*_tmp63D));({struct Cyc_List_List*_tmpB80=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp63D->f1=_tmpB80;}),_tmp63D->f2=1;_tmp63D;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2744 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->f1=0,_tmp63E->f2=1;_tmp63E;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2749 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp63F=_cycalloc(sizeof(*_tmp63F));({struct Cyc_Absyn_Pat*_tmpB81=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp63F->hd=_tmpB81;}),_tmp63F->tl=0;_tmp63F;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2751 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));({struct Cyc_Absyn_Pat*_tmpB83=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp640->hd=_tmpB83;}),({struct Cyc_List_List*_tmpB82=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp640->tl=_tmpB82;});_tmp640;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2756 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->f1=0,({struct Cyc_Absyn_Pat*_tmpB84=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp641->f2=_tmpB84;});_tmp641;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2758 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp642=_cycalloc(sizeof(*_tmp642));({struct Cyc_List_List*_tmpB86=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp642->f1=_tmpB86;}),({struct Cyc_Absyn_Pat*_tmpB85=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp642->f2=_tmpB85;});_tmp642;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2761
 yyval=Cyc_YY14(({struct _tuple23*_tmp643=_cycalloc(sizeof(*_tmp643));({struct Cyc_List_List*_tmpB87=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp643->f1=_tmpB87;}),_tmp643->f2=0;_tmp643;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2762 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_List_List*_tmpB88=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp644->f1=_tmpB88;}),_tmp644->f2=1;_tmp644;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2763 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=0,_tmp645->f2=1;_tmp645;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2768 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));({struct _tuple24*_tmpB89=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp646->hd=_tmpB89;}),_tmp646->tl=0;_tmp646;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2770 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp647=_cycalloc(sizeof(*_tmp647));({struct _tuple24*_tmpB8B=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp647->hd=_tmpB8B;}),({struct Cyc_List_List*_tmpB8A=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp647->tl=_tmpB8A;});_tmp647;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2776 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 421U: _LL345: _LL346:
# 2778 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB8C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB8D,_tmpB8C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422U: _LL347: _LL348:
# 2783 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 423U: _LL349: _LL34A:
# 2785 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB8F=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB8E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpB90,_tmpB8F,_tmpB8E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424U: _LL34B: _LL34C:
# 2787 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB91=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpB92,_tmpB91,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2791 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 426U: _LL34F: _LL350:
# 2792 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->v=(void*)Cyc_Absyn_Times;_tmp648;}));
goto _LL0;case 427U: _LL351: _LL352:
# 2793 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->v=(void*)Cyc_Absyn_Div;_tmp649;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2794 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->v=(void*)Cyc_Absyn_Mod;_tmp64A;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2795 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->v=(void*)Cyc_Absyn_Plus;_tmp64B;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2796 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->v=(void*)Cyc_Absyn_Minus;_tmp64C;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2797 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->v=(void*)Cyc_Absyn_Bitlshift;_tmp64D;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2798 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->v=(void*)Cyc_Absyn_Bitlrshift;_tmp64E;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2799 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->v=(void*)Cyc_Absyn_Bitand;_tmp64F;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2800 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->v=(void*)Cyc_Absyn_Bitxor;_tmp650;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2801 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->v=(void*)Cyc_Absyn_Bitor;_tmp651;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2806 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437U: _LL365: _LL366:
# 2808 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB95=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB94=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB95,_tmpB94,_tmpB93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438U: _LL367: _LL368:
# 2811
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpB96,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 439U: _LL369: _LL36A:
# 2814
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB97,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2816 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB98=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB98,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2818 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB9A,_tmpB99,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2820 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB9C,_tmpB9B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2824 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444U: _LL373: _LL374:
# 2828 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2830 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB9E,_tmpB9D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 446U: _LL377: _LL378:
# 2834 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 447U: _LL379: _LL37A:
# 2836 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpBA0,_tmpB9F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 448U: _LL37B: _LL37C:
# 2840 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 449U: _LL37D: _LL37E:
# 2842 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpBA2,_tmpBA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 450U: _LL37F: _LL380:
# 2846 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 451U: _LL381: _LL382:
# 2848 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBA4,_tmpBA3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 452U: _LL383: _LL384:
# 2852 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 453U: _LL385: _LL386:
# 2854 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBA6,_tmpBA5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 454U: _LL387: _LL388:
# 2858 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 455U: _LL389: _LL38A:
# 2860 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBA8,_tmpBA7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 456U: _LL38B: _LL38C:
# 2862 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBAA,_tmpBA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 457U: _LL38D: _LL38E:
# 2866 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 458U: _LL38F: _LL390:
# 2868 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBAC,_tmpBAB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 459U: _LL391: _LL392:
# 2870 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBAE,_tmpBAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 460U: _LL393: _LL394:
# 2872 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBB0,_tmpBAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2874 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBB2,_tmpBB1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2878 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 463U: _LL399: _LL39A:
# 2880 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBB4,_tmpBB3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464U: _LL39B: _LL39C:
# 2882 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBB6,_tmpBB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2886 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2888 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBB8,_tmpBB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2890 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBBA,_tmpBB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 2894 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 2896 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBBC,_tmpBBB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 2898 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBBE,_tmpBBD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 2900 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBC0,_tmpBBF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 2904 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 473U: _LL3AD: _LL3AE: {
# 2906 "parse.y"
void*_tmp652=({struct _tuple8*_tmpBC1=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBC1,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp652;
yyval=Cyc_Exp_tok(({void*_tmpBC3=t;struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBC3,_tmpBC2,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2909
goto _LL0;}case 474U: _LL3AF: _LL3B0:
# 2912 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 475U: _LL3B1: _LL3B2:
# 2913 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC4,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476U: _LL3B3: _LL3B4:
# 2914 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC5,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 2915 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBC6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 2916 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 2917 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBC8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 2918 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBCA=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBCA,_tmpBC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE: {
# 2920 "parse.y"
void*_tmp653=({struct _tuple8*_tmpBCB=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBCB,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp653;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2923
goto _LL0;}case 482U: _LL3BF: _LL3C0:
# 2923 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCC=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBCC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 483U: _LL3C1: _LL3C2: {
# 2925 "parse.y"
void*_tmp654=({struct _tuple8*_tmpBCD=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBCD,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp654;
yyval=Cyc_Exp_tok(({void*_tmpBCF=t;struct Cyc_List_List*_tmpBCE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBCF,_tmpBCE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2928
goto _LL0;}case 484U: _LL3C3: _LL3C4:
# 2930
 yyval=Cyc_Exp_tok(({void*_tmpBD1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->tag=34U,(_tmp655->f1).is_calloc=0,(_tmp655->f1).rgn=0,(_tmp655->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp655->f1).num_elts=_tmpBD0;}),(_tmp655->f1).fat_result=0,(_tmp655->f1).inline_call=0;_tmp655;});Cyc_Absyn_new_exp(_tmpBD1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2932
goto _LL0;case 485U: _LL3C5: _LL3C6:
# 2933 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->tag=34U,(_tmp656->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp656->f1).rgn=_tmpBD3;}),(_tmp656->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp656->f1).num_elts=_tmpBD2;}),(_tmp656->f1).fat_result=0,(_tmp656->f1).inline_call=0;_tmp656;});Cyc_Absyn_new_exp(_tmpBD4,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2935
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 2936 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD7=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->tag=34U,(_tmp657->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp657->f1).rgn=_tmpBD6;}),(_tmp657->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD5=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp657->f1).num_elts=_tmpBD5;}),(_tmp657->f1).fat_result=0,(_tmp657->f1).inline_call=1;_tmp657;});Cyc_Absyn_new_exp(_tmpBD7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2938
goto _LL0;case 487U: _LL3C9: _LL3CA: {
# 2939 "parse.y"
void*_tmp658=({struct _tuple8*_tmpBD8=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBD8,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp658;
yyval=Cyc_Exp_tok(({void*_tmpBDB=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->tag=34U,(_tmp65A->f1).is_calloc=1,(_tmp65A->f1).rgn=0,({void**_tmpBDA=({void**_tmp659=_cycalloc(sizeof(*_tmp659));*_tmp659=t;_tmp659;});(_tmp65A->f1).elt_type=_tmpBDA;}),({struct Cyc_Absyn_Exp*_tmpBD9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65A->f1).num_elts=_tmpBD9;}),(_tmp65A->f1).fat_result=0,(_tmp65A->f1).inline_call=0;_tmp65A;});Cyc_Absyn_new_exp(_tmpBDB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2942
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 2944
void*_tmp65B=({struct _tuple8*_tmpBDC=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBDC,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp65B;
yyval=Cyc_Exp_tok(({void*_tmpBE0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->tag=34U,(_tmp65D->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBDF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65D->f1).rgn=_tmpBDF;}),({void**_tmpBDE=({void**_tmp65C=_cycalloc(sizeof(*_tmp65C));*_tmp65C=t;_tmp65C;});(_tmp65D->f1).elt_type=_tmpBDE;}),({struct Cyc_Absyn_Exp*_tmpBDD=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp65D->f1).num_elts=_tmpBDD;}),(_tmp65D->f1).fat_result=0,(_tmp65D->f1).inline_call=0;_tmp65D;});Cyc_Absyn_new_exp(_tmpBE0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2947
goto _LL0;}case 489U: _LL3CD: _LL3CE:
# 2948 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBE2=({struct Cyc_Absyn_Exp*_tmp65E[1U];({struct Cyc_Absyn_Exp*_tmpBE1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp65E[0]=_tmpBE1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp65E,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBE2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 490U: _LL3CF: _LL3D0:
# 2950 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE6=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->tag=38U,({struct Cyc_Absyn_Exp*_tmpBE5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp660->f1=_tmpBE5;}),({struct _fat_ptr*_tmpBE4=({struct _fat_ptr*_tmp65F=_cycalloc(sizeof(*_tmp65F));({struct _fat_ptr _tmpBE3=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp65F=_tmpBE3;});_tmp65F;});_tmp660->f2=_tmpBE4;});_tmp660;});Cyc_Absyn_new_exp(_tmpBE6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 2952 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBEB=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->tag=38U,({struct Cyc_Absyn_Exp*_tmpBEA=({struct Cyc_Absyn_Exp*_tmpBE9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpBE9,(unsigned)((yyyvsp[2]).l).first_line);});_tmp662->f1=_tmpBEA;}),({struct _fat_ptr*_tmpBE8=({struct _fat_ptr*_tmp661=_cycalloc(sizeof(*_tmp661));({struct _fat_ptr _tmpBE7=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp661=_tmpBE7;});_tmp661;});_tmp662->f2=_tmpBE8;});_tmp662;});Cyc_Absyn_new_exp(_tmpBEB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4: {
# 2954 "parse.y"
void*_tmp663=({struct _tuple8*_tmpBEC=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBEC,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp663;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 493U: _LL3D5: _LL3D6:
# 2957 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBED=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpBED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 494U: _LL3D7: _LL3D8:
# 2958 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBEE=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpBEE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA:
# 2960 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBEF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpBEF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC: {
# 2965 "parse.y"
struct _tuple29*_tmp664=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp2D=_tmp664;struct _tuple29*_tmp665=_stmttmp2D;struct Cyc_List_List*_tmp668;struct Cyc_List_List*_tmp667;struct Cyc_List_List*_tmp666;_LL4BA: _tmp666=_tmp665->f1;_tmp667=_tmp665->f2;_tmp668=_tmp665->f3;_LL4BB: {struct Cyc_List_List*outlist=_tmp666;struct Cyc_List_List*inlist=_tmp667;struct Cyc_List_List*clobbers=_tmp668;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=40U,({int _tmpBF1=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp669->f1=_tmpBF1;}),({struct _fat_ptr _tmpBF0=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp669->f2=_tmpBF0;}),_tmp669->f3=outlist,_tmp669->f4=inlist,_tmp669->f5=clobbers;_tmp669;}));
goto _LL0;}}case 497U: _LL3DD: _LL3DE:
# 2970 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 498U: _LL3DF: _LL3E0:
# 2971 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 499U: _LL3E1: _LL3E2:
# 2975 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->f1=0,_tmp66A->f2=0,_tmp66A->f3=0;_tmp66A;}));
goto _LL0;case 500U: _LL3E3: _LL3E4: {
# 2977 "parse.y"
struct _tuple30*_tmp66B=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp2E=_tmp66B;struct _tuple30*_tmp66C=_stmttmp2E;struct Cyc_List_List*_tmp66E;struct Cyc_List_List*_tmp66D;_LL4BD: _tmp66D=_tmp66C->f1;_tmp66E=_tmp66C->f2;_LL4BE: {struct Cyc_List_List*inlist=_tmp66D;struct Cyc_List_List*clobbers=_tmp66E;
yyval=Cyc_YY59(({struct _tuple29*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->f1=0,_tmp66F->f2=inlist,_tmp66F->f3=clobbers;_tmp66F;}));
goto _LL0;}}case 501U: _LL3E5: _LL3E6: {
# 2980 "parse.y"
struct _tuple30*_tmp670=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp2F=_tmp670;struct _tuple30*_tmp671=_stmttmp2F;struct Cyc_List_List*_tmp673;struct Cyc_List_List*_tmp672;_LL4C0: _tmp672=_tmp671->f1;_tmp673=_tmp671->f2;_LL4C1: {struct Cyc_List_List*inlist=_tmp672;struct Cyc_List_List*clobbers=_tmp673;
yyval=Cyc_YY59(({struct _tuple29*_tmp674=_cycalloc(sizeof(*_tmp674));({struct Cyc_List_List*_tmpBF2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp674->f1=_tmpBF2;}),_tmp674->f2=inlist,_tmp674->f3=clobbers;_tmp674;}));
goto _LL0;}}case 502U: _LL3E7: _LL3E8:
# 2985 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));({struct _tuple31*_tmpBF3=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp675->hd=_tmpBF3;}),_tmp675->tl=0;_tmp675;}));
goto _LL0;case 503U: _LL3E9: _LL3EA:
# 2986 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp676=_cycalloc(sizeof(*_tmp676));({struct _tuple31*_tmpBF5=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp676->hd=_tmpBF5;}),({struct Cyc_List_List*_tmpBF4=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp676->tl=_tmpBF4;});_tmp676;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 2990 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->f1=0,_tmp677->f2=0;_tmp677;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 2992 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->f1=0,({struct Cyc_List_List*_tmpBF6=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp678->f2=_tmpBF6;});_tmp678;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 2994 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp679=_cycalloc(sizeof(*_tmp679));({struct Cyc_List_List*_tmpBF8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp679->f1=_tmpBF8;}),({struct Cyc_List_List*_tmpBF7=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp679->f2=_tmpBF7;});_tmp679;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 2998 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));({struct _tuple31*_tmpBF9=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp67A->hd=_tmpBF9;}),_tmp67A->tl=0;_tmp67A;}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 2999 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67B=_cycalloc(sizeof(*_tmp67B));({struct _tuple31*_tmpBFB=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp67B->hd=_tmpBFB;}),({struct Cyc_List_List*_tmpBFA=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp67B->tl=_tmpBFA;});_tmp67B;}));
goto _LL0;case 509U: _LL3F5: _LL3F6: {
# 3004 "parse.y"
struct Cyc_Absyn_Exp*_tmp67C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp67C;
yyval=Cyc_YY63(({struct _tuple31*_tmp67D=_cycalloc(sizeof(*_tmp67D));({struct _fat_ptr _tmpBFD=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp67D->f1=_tmpBFD;}),({struct Cyc_Absyn_Exp*_tmpBFC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp67D->f2=_tmpBFC;});_tmp67D;}));
goto _LL0;}case 510U: _LL3F7: _LL3F8:
# 3010 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 511U: _LL3F9: _LL3FA:
# 3011 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3012 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3016 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct _fat_ptr*_tmpBFF=({struct _fat_ptr*_tmp67E=_cycalloc(sizeof(*_tmp67E));({struct _fat_ptr _tmpBFE=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp67E=_tmpBFE;});_tmp67E;});_tmp67F->hd=_tmpBFF;}),_tmp67F->tl=0;_tmp67F;}));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3017 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));({struct _fat_ptr*_tmpC02=({struct _fat_ptr*_tmp680=_cycalloc(sizeof(*_tmp680));({struct _fat_ptr _tmpC01=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp680=_tmpC01;});_tmp680;});_tmp681->hd=_tmpC02;}),({struct Cyc_List_List*_tmpC00=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp681->tl=_tmpC00;});_tmp681;}));
goto _LL0;case 515U: _LL401: _LL402:
# 3021 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 516U: _LL403: _LL404:
# 3022 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 517U: _LL405: _LL406:
# 3023 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 518U: _LL407: _LL408:
# 3028 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 519U: _LL409: _LL40A:
# 3030 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC04=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpC03=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC04,_tmpC03,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520U: _LL40B: _LL40C:
# 3032 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC05=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC05,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3034 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC07=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC06=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC07,_tmpC06,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3036 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC09=({struct _fat_ptr*_tmp682=_cycalloc(sizeof(*_tmp682));({struct _fat_ptr _tmpC08=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp682=_tmpC08;});_tmp682;});Cyc_Absyn_aggrmember_exp(_tmpC0A,_tmpC09,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3038 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC0C=({struct _fat_ptr*_tmp683=_cycalloc(sizeof(*_tmp683));({struct _fat_ptr _tmpC0B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp683=_tmpC0B;});_tmp683;});Cyc_Absyn_aggrarrow_exp(_tmpC0D,_tmpC0C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3040 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC0E,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3042 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC0F,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3044 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC11=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->tag=25U,({struct _tuple8*_tmpC10=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp684->f1=_tmpC10;}),_tmp684->f2=0;_tmp684;});Cyc_Absyn_new_exp(_tmpC11,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3046 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC14=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->tag=25U,({struct _tuple8*_tmpC13=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp685->f1=_tmpC13;}),({struct Cyc_List_List*_tmpC12=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp685->f2=_tmpC12;});_tmp685;});Cyc_Absyn_new_exp(_tmpC14,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3048 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC17=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->tag=25U,({struct _tuple8*_tmpC16=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp686->f1=_tmpC16;}),({struct Cyc_List_List*_tmpC15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp686->f2=_tmpC15;});_tmp686;});Cyc_Absyn_new_exp(_tmpC17,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3053 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp689=_cycalloc(sizeof(*_tmp689));({void*_tmpC1A=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->tag=0U,({struct _fat_ptr*_tmpC19=({struct _fat_ptr*_tmp687=_cycalloc(sizeof(*_tmp687));({struct _fat_ptr _tmpC18=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp687=_tmpC18;});_tmp687;});_tmp688->f1=_tmpC19;});_tmp688;});_tmp689->hd=_tmpC1A;}),_tmp689->tl=0;_tmp689;}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3056
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp68B=_cycalloc(sizeof(*_tmp68B));({void*_tmpC1D=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->tag=1U,({unsigned _tmpC1C=({unsigned _tmpC1B=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC1B,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp68A->f1=_tmpC1C;});_tmp68A;});_tmp68B->hd=_tmpC1D;}),_tmp68B->tl=0;_tmp68B;}));
goto _LL0;case 531U: _LL421: _LL422:
# 3058 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp68E=_cycalloc(sizeof(*_tmp68E));({void*_tmpC21=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->tag=0U,({struct _fat_ptr*_tmpC20=({struct _fat_ptr*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct _fat_ptr _tmpC1F=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68C=_tmpC1F;});_tmp68C;});_tmp68D->f1=_tmpC20;});_tmp68D;});_tmp68E->hd=_tmpC21;}),({struct Cyc_List_List*_tmpC1E=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp68E->tl=_tmpC1E;});_tmp68E;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3061
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp690=_cycalloc(sizeof(*_tmp690));({void*_tmpC25=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=1U,({unsigned _tmpC24=({unsigned _tmpC23=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC23,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp68F->f1=_tmpC24;});_tmp68F;});_tmp690->hd=_tmpC25;}),({struct Cyc_List_List*_tmpC22=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp690->tl=_tmpC22;});_tmp690;}));
goto _LL0;case 533U: _LL425: _LL426:
# 3067 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC26=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC26,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534U: _LL427: _LL428:
# 3069 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC27=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC27,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535U: _LL429: _LL42A:
# 3071 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 536U: _LL42B: _LL42C:
# 3073 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC28=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC28,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 537U: _LL42D: _LL42E:
# 3075 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC29=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC29,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 538U: _LL42F: _LL430:
# 3077 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 539U: _LL431: _LL432:
# 3082 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC2A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC2A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 540U: _LL433: _LL434:
# 3084 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC2C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC2C,_tmpC2B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 541U: _LL435: _LL436:
# 3087
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC2D=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC2D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3090
 yyval=Cyc_Exp_tok(({void*_tmpC31=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->tag=29U,({struct _tuple0*_tmpC30=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp691->f1=_tmpC30;}),({struct Cyc_List_List*_tmpC2F=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp691->f2=_tmpC2F;}),({struct Cyc_List_List*_tmpC2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp691->f3=_tmpC2E;}),_tmp691->f4=0;_tmp691;});Cyc_Absyn_new_exp(_tmpC31,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3093
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC32=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC32,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3097 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3103 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp692=_cycalloc(sizeof(*_tmp692));({struct Cyc_Absyn_Exp*_tmpC33=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp692->hd=_tmpC33;}),_tmp692->tl=0;_tmp692;}));
goto _LL0;case 546U: _LL43F: _LL440:
# 3105 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp693=_cycalloc(sizeof(*_tmp693));({struct Cyc_Absyn_Exp*_tmpC35=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp693->hd=_tmpC35;}),({struct Cyc_List_List*_tmpC34=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp693->tl=_tmpC34;});_tmp693;}));
goto _LL0;case 547U: _LL441: _LL442:
# 3111 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC36=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC36,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 548U: _LL443: _LL444:
# 3112 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC37=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC37,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 549U: _LL445: _LL446:
# 3113 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC38=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC38,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 550U: _LL447: _LL448: {
# 3115 "parse.y"
struct _fat_ptr _tmp694=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp694;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3123
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3125
goto _LL0;}case 551U: _LL449: _LL44A:
# 3126 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 552U: _LL44B: _LL44C:
# 3130 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp696=_cycalloc(sizeof(*_tmp696));({union Cyc_Absyn_Nmspace _tmpC3B=Cyc_Absyn_Rel_n(0);_tmp696->f1=_tmpC3B;}),({struct _fat_ptr*_tmpC3A=({struct _fat_ptr*_tmp695=_cycalloc(sizeof(*_tmp695));({struct _fat_ptr _tmpC39=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp695=_tmpC39;});_tmp695;});_tmp696->f2=_tmpC3A;});_tmp696;}));
goto _LL0;case 553U: _LL44D: _LL44E:
# 3131 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 554U: _LL44F: _LL450:
# 3134
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp698=_cycalloc(sizeof(*_tmp698));({union Cyc_Absyn_Nmspace _tmpC3E=Cyc_Absyn_Rel_n(0);_tmp698->f1=_tmpC3E;}),({struct _fat_ptr*_tmpC3D=({struct _fat_ptr*_tmp697=_cycalloc(sizeof(*_tmp697));({struct _fat_ptr _tmpC3C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp697=_tmpC3C;});_tmp697;});_tmp698->f2=_tmpC3D;});_tmp698;}));
goto _LL0;case 555U: _LL451: _LL452:
# 3135 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 556U: _LL453: _LL454:
# 3140 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 557U: _LL455: _LL456:
# 3141 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3144
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3145 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 560U: _LL45B: _LL45C:
# 3150 "parse.y"
 goto _LL0;case 561U: _LL45D: _LL45E:
# 3150 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL45F: _LL460:
# 3154
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
({int _tmpC3F=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC3F;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC40=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 152));_tmpC40 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7949)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,162U,sizeof(short),yyn - 152));}
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
if(yyn > - 32768 && yyn < 7949){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 314U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),x + yyn))== x)
({unsigned long _tmpC41=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,314U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC41;}),count ++;}
msg=({unsigned _tmp69A=(unsigned)(sze + 15)+ 1U;char*_tmp699=({struct _RegionHandle*_tmpC42=yyregion;_region_malloc(_tmpC42,_check_times(_tmp69A,sizeof(char)));});({{unsigned _tmp75E=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp75E;++ i){_tmp699[i]='\000';}_tmp699[_tmp75E]=0;}0;});_tag_fat(_tmp699,sizeof(char),_tmp69A);});
({struct _fat_ptr _tmpC43=msg;Cyc_strcpy(_tmpC43,({const char*_tmp69B="parse error";_tag_fat(_tmp69B,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 314U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC44=msg;Cyc_strcat(_tmpC44,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp69C=", expecting `";_tag_fat(_tmp69C,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp69D=" or `";_tag_fat(_tmp69D,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,314U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC45=msg;Cyc_strcat(_tmpC45,({const char*_tmp69E="'";_tag_fat(_tmp69E,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC47=({const char*_tmp69F="parse error";_tag_fat(_tmp69F,sizeof(char),12U);});int _tmpC46=yystate;Cyc_yyerror(_tmpC47,_tmpC46,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp6A0=1;_npop_handler(0U);return _tmp6A0;}
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
 if(yyssp_offset == 0){int _tmp6A1=1;_npop_handler(0U);return _tmp6A1;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1137U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7949)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),yyn))!= 1)goto yyerrdefault;
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
if(yyn == 1136){
int _tmp6A2=0;_npop_handler(0U);return _tmp6A2;}
# 546
({struct Cyc_Yystacktype _tmpC48=({struct Cyc_Yystacktype _tmp75F;_tmp75F.v=yylval,_tmp75F.l=yylloc;_tmp75F;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC48;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3153 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6A6=v;struct Cyc_Absyn_Stmt*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A8;struct _tuple0*_tmp6A9;struct _fat_ptr _tmp6AA;char _tmp6AB;union Cyc_Absyn_Cnst _tmp6AC;switch((_tmp6A6.Stmt_tok).tag){case 1U: _LL1: _tmp6AC=(_tmp6A6.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6AC;
({struct Cyc_String_pa_PrintArg_struct _tmp6AF=({struct Cyc_String_pa_PrintArg_struct _tmp762;_tmp762.tag=0U,({struct _fat_ptr _tmpC49=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp762.f1=_tmpC49;});_tmp762;});void*_tmp6AD[1U];_tmp6AD[0]=& _tmp6AF;({struct Cyc___cycFILE*_tmpC4B=Cyc_stderr;struct _fat_ptr _tmpC4A=({const char*_tmp6AE="%s";_tag_fat(_tmp6AE,sizeof(char),3U);});Cyc_fprintf(_tmpC4B,_tmpC4A,_tag_fat(_tmp6AD,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6AB=(_tmp6A6.Char_tok).val;_LL4: {char c=_tmp6AB;
({struct Cyc_Int_pa_PrintArg_struct _tmp6B2=({struct Cyc_Int_pa_PrintArg_struct _tmp763;_tmp763.tag=1U,_tmp763.f1=(unsigned long)((int)c);_tmp763;});void*_tmp6B0[1U];_tmp6B0[0]=& _tmp6B2;({struct Cyc___cycFILE*_tmpC4D=Cyc_stderr;struct _fat_ptr _tmpC4C=({const char*_tmp6B1="%c";_tag_fat(_tmp6B1,sizeof(char),3U);});Cyc_fprintf(_tmpC4D,_tmpC4C,_tag_fat(_tmp6B0,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6AA=(_tmp6A6.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6AA;
({struct Cyc_String_pa_PrintArg_struct _tmp6B5=({struct Cyc_String_pa_PrintArg_struct _tmp764;_tmp764.tag=0U,_tmp764.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp764;});void*_tmp6B3[1U];_tmp6B3[0]=& _tmp6B5;({struct Cyc___cycFILE*_tmpC4F=Cyc_stderr;struct _fat_ptr _tmpC4E=({const char*_tmp6B4="\"%s\"";_tag_fat(_tmp6B4,sizeof(char),5U);});Cyc_fprintf(_tmpC4F,_tmpC4E,_tag_fat(_tmp6B3,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6A9=(_tmp6A6.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6A9;
({struct Cyc_String_pa_PrintArg_struct _tmp6B8=({struct Cyc_String_pa_PrintArg_struct _tmp765;_tmp765.tag=0U,({struct _fat_ptr _tmpC50=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp765.f1=_tmpC50;});_tmp765;});void*_tmp6B6[1U];_tmp6B6[0]=& _tmp6B8;({struct Cyc___cycFILE*_tmpC52=Cyc_stderr;struct _fat_ptr _tmpC51=({const char*_tmp6B7="%s";_tag_fat(_tmp6B7,sizeof(char),3U);});Cyc_fprintf(_tmpC52,_tmpC51,_tag_fat(_tmp6B6,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6A8=(_tmp6A6.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6A8;
({struct Cyc_String_pa_PrintArg_struct _tmp6BB=({struct Cyc_String_pa_PrintArg_struct _tmp766;_tmp766.tag=0U,({struct _fat_ptr _tmpC53=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp766.f1=_tmpC53;});_tmp766;});void*_tmp6B9[1U];_tmp6B9[0]=& _tmp6BB;({struct Cyc___cycFILE*_tmpC55=Cyc_stderr;struct _fat_ptr _tmpC54=({const char*_tmp6BA="%s";_tag_fat(_tmp6BA,sizeof(char),3U);});Cyc_fprintf(_tmpC55,_tmpC54,_tag_fat(_tmp6B9,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6A7=(_tmp6A6.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6A7;
({struct Cyc_String_pa_PrintArg_struct _tmp6BE=({struct Cyc_String_pa_PrintArg_struct _tmp767;_tmp767.tag=0U,({struct _fat_ptr _tmpC56=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp767.f1=_tmpC56;});_tmp767;});void*_tmp6BC[1U];_tmp6BC[0]=& _tmp6BE;({struct Cyc___cycFILE*_tmpC58=Cyc_stderr;struct _fat_ptr _tmpC57=({const char*_tmp6BD="%s";_tag_fat(_tmp6BD,sizeof(char),3U);});Cyc_fprintf(_tmpC58,_tmpC57,_tag_fat(_tmp6BC,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6BF=0U;({struct Cyc___cycFILE*_tmpC5A=Cyc_stderr;struct _fat_ptr _tmpC59=({const char*_tmp6C0="?";_tag_fat(_tmp6C0,sizeof(char),2U);});Cyc_fprintf(_tmpC5A,_tmpC59,_tag_fat(_tmp6BF,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3165
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6C1="end-of-file";_tag_fat(_tmp6C1,sizeof(char),12U);});
if(token == 367)
return Cyc_Lex_token_string;
if(token == 376)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 379?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,380U,sizeof(short),token)): 314;
if((unsigned)z < 314U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 3179
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6C2=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6C2;_push_region(yyr);
({struct _RegionHandle*_tmpC5B=yyr;Cyc_yyparse(_tmpC5B,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6C3=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6C3;}
# 3182
;_pop_region();}}
