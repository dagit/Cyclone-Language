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
# 173 "core.h"
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 450
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 457
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
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 696 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 858
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 887
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 891
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 896
void*Cyc_Absyn_compress_kb(void*);
# 900
void*Cyc_Absyn_compress(void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 919
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 921
extern void*Cyc_Absyn_sint_type;
# 923
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 957
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 961
void*Cyc_Absyn_bounds_one (void);
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 983
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 986
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1035
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
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1063
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1068
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1085
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1089
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
# 1094
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1096
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1103
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1110
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1113
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1118
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
# 53
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 46 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 147
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 149
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 151
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 156
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 166
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 250
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 278
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 288
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
# 72 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 97 "parse.y"
int Cyc_Parse_parsing_tempest=0;struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 103
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 106
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
# 134
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple11{struct _tuple11*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _tuple13*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 170
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple13*ds,struct _tuple11**decls,struct Cyc_List_List**es){
# 174
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple12 _tmp0=ds->hd;struct _tuple12 _stmttmp0=_tmp0;struct _tuple12 _tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp3;struct Cyc_Parse_Declarator _tmp2;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2: {struct Cyc_Parse_Declarator d=_tmp2;struct Cyc_Absyn_Exp*e=_tmp3;
declarators=({struct _tuple11*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->tl=declarators,_tmp4->hd=d;_tmp4;});
exprs=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=e,_tmp5->tl=exprs;_tmp5;});}}
# 181
({struct Cyc_List_List*_tmp780=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp780;});
({struct _tuple11*_tmp781=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp781;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmpA=0U;({unsigned _tmp783=loc;struct _fat_ptr _tmp782=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp783,_tmp782,_tag_fat(_tmpA,sizeof(void*),0U));});});}
# 212
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;};
# 215
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 221
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
# 233
return({struct _tuple15 _tmp6D7;_tmp6D7.f1=nullable,_tmp6D7.f2=bound,_tmp6D7.f3=zeroterm,_tmp6D7.f4=rgn;_tmp6D7;});}
# 239
struct _tuple0*Cyc_Parse_gensym_enum (void){
# 241
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp787=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp787;}),({
struct _fat_ptr*_tmp786=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp785=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6D8;_tmp6D8.tag=1U,_tmp6D8.f1=(unsigned long)enum_counter ++;_tmp6D8;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp784=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp784,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp785;});_tmp13;});_tmp14->f2=_tmp786;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 251
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp789=loc;struct _fat_ptr _tmp788=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp789,_tmp788,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp78B=loc;struct _fat_ptr _tmp78A=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp78B,_tmp78A,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 261
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6D9;_tmp6D9.Signed_spec=0,_tmp6D9.Unsigned_spec=0,_tmp6D9.Short_spec=0,_tmp6D9.Long_spec=0,_tmp6D9.Long_Long_spec=0,_tmp6D9.Valid_type_spec=0,_tmp6D9.Type_spec=Cyc_Absyn_sint_type,_tmp6D9.loc=loc;_tmp6D9;});}
# 272
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp23;
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 278
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp24;
s.Signed_spec=1;
return s;}
# 283
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp25;
s.Unsigned_spec=1;
return s;}
# 288
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp26;
s.Short_spec=1;
return s;}
# 293
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp27;
s.Long_spec=1;
return s;}
# 300
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 302
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp78C=t;Cyc_Tcutil_promote_array(_tmp78C,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 315 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 320
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 324
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp78E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6DA;_tmp6DA.tag=0U,_tmp6DA.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6DA;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp78D=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp78D,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp78E;});_tmp36;});
({void*_tmp790=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp78F=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Tcutil_ik;_tmp31;});_tmp32->kind=_tmp78F;});_tmp32;}));*z=_tmp790;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 329
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp791=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp791;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 333
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp793=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6DB;_tmp6DB.tag=0U,_tmp6DB.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6DB;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp792=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp792,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp793;});_tmp3E;});
({void*_tmp795=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp794=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Tcutil_rk;_tmp39;});_tmp3A->kind=_tmp794;});_tmp3A;}));*z=_tmp795;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 339
return res;}
# 343
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;struct Cyc_List_List*res=_tmp3F;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_stmttmp3=_tmp40;void*_tmp41=_stmttmp3;void*_tmp42;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp42=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2: {void*i=_tmp42;
# 348
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp796=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp796;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
return({void*_tmp798=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp797=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp797;});_tmp4D;});Cyc_Absyn_new_exp(_tmp798,e->loc);});}}}
# 364
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 367
return e;}
# 372
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp4F;void*_tmp50;struct Cyc_Absyn_PtrLoc*_tmp57;void*_tmp56;void*_tmp55;void*_tmp54;void*_tmp53;struct Cyc_Absyn_Tqual _tmp52;void*_tmp51;unsigned _tmp5C;void*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Tqual _tmp59;void*_tmp58;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp5A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp5B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp5C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {void*et=_tmp58;struct Cyc_Absyn_Tqual tq=_tmp59;struct Cyc_Absyn_Exp*nelts=_tmp5A;void*zt=_tmp5B;unsigned ztloc=_tmp5C;
# 375
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
# 379
void*_tmp5D=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5D;
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3U: _LL3: _tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp56=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp57=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_LL4: {void*et=_tmp51;struct Cyc_Absyn_Tqual tq=_tmp52;void*r=_tmp53;void*n=_tmp54;void*b=_tmp55;void*zt=_tmp56;struct Cyc_Absyn_PtrLoc*pl=_tmp57;
# 384
void*_tmp5E=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5E;
void*_tmp5F=Cyc_Parse_substitute_tags(tags,b);void*b2=_tmp5F;
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6DC;_tmp6DC.elt_type=et2,_tmp6DC.elt_tq=tq,(_tmp6DC.ptr_atts).rgn=r,(_tmp6DC.ptr_atts).nullable=n,(_tmp6DC.ptr_atts).bounds=b2,(_tmp6DC.ptr_atts).zero_term=zt,(_tmp6DC.ptr_atts).ptrloc=pl;_tmp6DC;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp50=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {void*t=_tmp50;
# 390
void*_tmp60=Cyc_Parse_substitute_tags(tags,t);void*t2=_tmp60;
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp4F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp4F;
# 394
struct Cyc_Absyn_Exp*_tmp61=Cyc_Parse_substitute_tags_exp(tags,e);struct Cyc_Absyn_Exp*e2=_tmp61;
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9: _LLA:
# 400
 goto _LL0;}_LL0:;}
# 402
return t;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp799=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp799;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple8*t){
# 416
return({struct _tuple20*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=(*t).f2,_tmp62->f2=(*t).f3;_tmp62;});}
# 419
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp63=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp63)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 428
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp79A=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp79A,({const char*_tmp64="`H";_tag_fat(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp79B=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp79B,({const char*_tmp65="`U";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp79C=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp79C,({const char*_tmp66="`RC";_tag_fat(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr*_tmp79D=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp79D;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}
# 440
static int Cyc_Parse_tvar_ok(struct _fat_ptr s,struct _fat_ptr*err){
if(({struct _fat_ptr _tmp79E=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp79E,({const char*_tmp69="`H";_tag_fat(_tmp69,sizeof(char),3U);}));})== 0){
({struct _fat_ptr _tmp79F=({const char*_tmp6A="bad occurrence of heap region";_tag_fat(_tmp6A,sizeof(char),30U);});*err=_tmp79F;});
return 0;}
# 445
if(({struct _fat_ptr _tmp7A0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp7A0,({const char*_tmp6B="`U";_tag_fat(_tmp6B,sizeof(char),3U);}));})== 0){
({struct _fat_ptr _tmp7A1=({const char*_tmp6C="bad occurrence of unique region";_tag_fat(_tmp6C,sizeof(char),32U);});*err=_tmp7A1;});
return 0;}
# 449
if(({struct _fat_ptr _tmp7A2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp7A2,({const char*_tmp6D="`RC";_tag_fat(_tmp6D,sizeof(char),4U);}));})== 0){
({struct _fat_ptr _tmp7A3=({const char*_tmp6E="bad occurrence of refcounted region";_tag_fat(_tmp6E,sizeof(char),36U);});*err=_tmp7A3;});
return 0;}
# 453
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0){
({struct _fat_ptr _tmp7A4=({const char*_tmp6F="bad occurrence of \"current\" region";_tag_fat(_tmp6F,sizeof(char),35U);});*err=_tmp7A4;});
return 0;}
# 457
return 1;}
# 464
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp70=t;struct Cyc_Absyn_Tvar*_tmp71;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->tag == 2U){_LL1: _tmp71=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp71;
return pr;}}else{_LL3: _LL4:
({void*_tmp72=0U;({unsigned _tmp7A6=loc;struct _fat_ptr _tmp7A5=({const char*_tmp73="expecting a list of type variables, not types";_tag_fat(_tmp73,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7A6,_tmp7A5,_tag_fat(_tmp72,sizeof(void*),0U));});});}_LL0:;}
# 472
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp74=Cyc_Absyn_compress(t);void*_stmttmp6=_tmp74;void*_tmp75=_stmttmp6;void**_tmp76;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->tag == 2U){_LL1: _tmp76=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1)->kind;_LL2: {void**cptr=_tmp76;
# 475
void*_tmp77=Cyc_Absyn_compress_kb(*cptr);void*_stmttmp7=_tmp77;void*_tmp78=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp78)->tag == 1U){_LL6: _LL7:
# 477
({void*_tmp7A7=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=2U,_tmp79->f1=0,_tmp79->f2=k;_tmp79;}): Cyc_Tcutil_kind_to_bound(k);*cptr=_tmp7A7;});
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 481
 return;}_LL0:;}
# 486
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 492
if(tds == 0)return tms;
# 497
if(tms == 0)return 0;{
# 499
void*_tmp7A=(void*)tms->hd;void*_stmttmp8=_tmp7A;void*_tmp7B=_stmttmp8;void*_tmp7C;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7B)->tag == 3U){_LL1: _tmp7C=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7B)->f1;_LL2: {void*args=_tmp7C;
# 502
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 505
void*_tmp7D=args;struct Cyc_List_List*_tmp7E;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp7D)->tag == 1U){_LL6: _LL7:
# 507
({void*_tmp7F=0U;({unsigned _tmp7A9=loc;struct _fat_ptr _tmp7A8=({const char*_tmp80="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp80,sizeof(char),93U);});Cyc_Warn_warn(_tmp7A9,_tmp7A8,_tag_fat(_tmp7F,sizeof(void*),0U));});});
# 509
return tms;}else{_LL8: _tmp7E=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp7D)->f1;_LL9: {struct Cyc_List_List*ids=_tmp7E;
# 511
if(({int _tmp7AA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp7AA != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp81=0U;({unsigned _tmp7AC=loc;struct _fat_ptr _tmp7AB=({const char*_tmp82="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp82,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AC,_tmp7AB,_tag_fat(_tmp81,sizeof(void*),0U));});});{
# 515
struct Cyc_List_List*rev_new_params=0;
for(0;ids != 0;ids=ids->tl){
struct Cyc_List_List*_tmp83=tds;struct Cyc_List_List*tds2=_tmp83;
for(0;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp84=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp84;
void*_tmp85=x->r;void*_stmttmp9=_tmp85;void*_tmp86=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp87;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp86)->tag == 0U){_LLB: _tmp87=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp86)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp87;
# 522
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({void*_tmp88=0U;({unsigned _tmp7AE=x->loc;struct _fat_ptr _tmp7AD=({const char*_tmp89="initializer found in parameter declaration";_tag_fat(_tmp89,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AE,_tmp7AD,_tag_fat(_tmp88,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp8A=0U;({unsigned _tmp7B0=x->loc;struct _fat_ptr _tmp7AF=({const char*_tmp8B="namespaces forbidden in parameter declarations";_tag_fat(_tmp8B,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7B0,_tmp7AF,_tag_fat(_tmp8A,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp8D=_cycalloc(sizeof(*_tmp8D));
({struct _tuple8*_tmp7B1=({struct _tuple8*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->f1=(*vd->name).f2,_tmp8C->f2=vd->tq,_tmp8C->f3=vd->type;_tmp8C;});_tmp8D->hd=_tmp7B1;}),_tmp8D->tl=rev_new_params;_tmp8D;});
# 531
goto L;}}else{_LLD: _LLE:
({void*_tmp8E=0U;({unsigned _tmp7B3=x->loc;struct _fat_ptr _tmp7B2=({const char*_tmp8F="nonvariable declaration in parameter type";_tag_fat(_tmp8F,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7B3,_tmp7B2,_tag_fat(_tmp8E,sizeof(void*),0U));});});}_LLA:;}
# 535
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp6DD;_tmp6DD.tag=0U,_tmp6DD.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6DD;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({unsigned _tmp7B5=loc;struct _fat_ptr _tmp7B4=({const char*_tmp91="%s is not given a type";_tag_fat(_tmp91,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7B5,_tmp7B4,_tag_fat(_tmp90,sizeof(void*),1U));});});}
# 538
return({struct Cyc_List_List*_tmp95=_region_malloc(yy,sizeof(*_tmp95));
({void*_tmp7B8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp94=_region_malloc(yy,sizeof(*_tmp94));_tmp94->tag=3U,({void*_tmp7B7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp93=_region_malloc(yy,sizeof(*_tmp93));_tmp93->tag=1U,({struct Cyc_List_List*_tmp7B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp93->f1=_tmp7B6;}),_tmp93->f2=0,_tmp93->f3=0,_tmp93->f4=0,_tmp93->f5=0,_tmp93->f6=0,_tmp93->f7=0;_tmp93;});_tmp94->f1=_tmp7B7;});_tmp94;});_tmp95->hd=_tmp7B8;}),_tmp95->tl=0;_tmp95;});}}}_LL5:;}
# 545
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp96=_region_malloc(yy,sizeof(*_tmp96));_tmp96->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7B9=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp96->tl=_tmp7B9;});_tmp96;});}_LL0:;}}
# 553
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 557
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6DE;_tmp6DE.id=d.id,_tmp6DE.varloc=d.varloc,({struct Cyc_List_List*_tmp7BA=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6DE.tms=_tmp7BA;});_tmp6DE;});{
# 560
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 566
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 572
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmp97=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _stmttmpA=_tmp97;enum Cyc_Parse_Storage_class _tmp98=_stmttmpA;switch(_tmp98){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=0U;goto _LL0;default: _LL5: _LL6:
({void*_tmp99=0U;({unsigned _tmp7BC=loc;struct _fat_ptr _tmp7BB=({const char*_tmp9A="bad storage class on function";_tag_fat(_tmp9A,sizeof(char),30U);});Cyc_Warn_err(_tmp7BC,_tmp7BB,_tag_fat(_tmp99,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 579
void*_tmp9B=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp9B;
struct _tuple14 _tmp9C=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmp9C;struct _tuple14 _tmp9D=_stmttmpB;struct Cyc_List_List*_tmpA1;struct Cyc_List_List*_tmpA0;void*_tmp9F;struct Cyc_Absyn_Tqual _tmp9E;_LL8: _tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;_tmpA0=_tmp9D.f3;_tmpA1=_tmp9D.f4;_LL9: {struct Cyc_Absyn_Tqual fn_tqual=_tmp9E;void*fn_type=_tmp9F;struct Cyc_List_List*x=_tmpA0;struct Cyc_List_List*out_atts=_tmpA1;
# 584
if(x != 0)
# 587
({void*_tmpA2=0U;({unsigned _tmp7BE=loc;struct _fat_ptr _tmp7BD=({const char*_tmpA3="bad type params, ignoring";_tag_fat(_tmpA3,sizeof(char),26U);});Cyc_Warn_warn(_tmp7BE,_tmp7BD,_tag_fat(_tmpA2,sizeof(void*),0U));});});{
# 589
void*_tmpA4=fn_type;struct Cyc_Absyn_FnInfo _tmpA5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA4)->tag == 5U){_LLB: _tmpA5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA4)->f1;_LLC: {struct Cyc_Absyn_FnInfo i=_tmpA5;
# 591
{struct Cyc_List_List*_tmpA6=i.args;struct Cyc_List_List*args2=_tmpA6;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpA7=0U;({unsigned _tmp7C0=loc;struct _fat_ptr _tmp7BF=({const char*_tmpA8="missing argument variable in function prototype";_tag_fat(_tmpA8,sizeof(char),48U);});Cyc_Warn_err(_tmp7C0,_tmp7BF,_tag_fat(_tmpA7,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7C2=({struct _fat_ptr*_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct _fat_ptr _tmp7C1=({const char*_tmpA9="?";_tag_fat(_tmpA9,sizeof(char),2U);});*_tmpAA=_tmp7C1;});_tmpAA;});(*((struct _tuple8*)args2->hd)).f1=_tmp7C2;});}}}
# 598
({struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7C3;});
return({struct Cyc_Absyn_Fndecl*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->sc=sc,_tmpAB->is_inline=is_inline,_tmpAB->name=d.id,_tmpAB->body=body,_tmpAB->i=i,_tmpAB->cached_type=0,_tmpAB->param_vardecls=0,_tmpAB->fn_vardecl=0,_tmpAB->orig_scope=sc;_tmpAB;});}}else{_LLD: _LLE:
# 603
({void*_tmpAC=0U;({unsigned _tmp7C5=loc;struct _fat_ptr _tmp7C4=({const char*_tmpAD="declarator is not a function prototype";_tag_fat(_tmpAD,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7C5,_tmp7C4,_tag_fat(_tmpAC,sizeof(void*),0U));});});}_LLA:;}}}}}static char _tmpAE[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 607
static struct _fat_ptr Cyc_Parse_msg1={_tmpAE,_tmpAE,_tmpAE + 76U};static char _tmpAF[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 609
static struct _fat_ptr Cyc_Parse_msg2={_tmpAF,_tmpAF,_tmpAF + 87U};static char _tmpB0[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 611
static struct _fat_ptr Cyc_Parse_msg3={_tmpB0,_tmpB0,_tmpB0 + 74U};static char _tmpB1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 613
static struct _fat_ptr Cyc_Parse_msg4={_tmpB1,_tmpB1,_tmpB1 + 84U};
# 620
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 623
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpB2=0U;({unsigned _tmp7C7=loc;struct _fat_ptr _tmp7C6=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7C7,_tmp7C6,_tag_fat(_tmpB2,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpB3=0U;({unsigned _tmp7C9=loc;struct _fat_ptr _tmp7C8=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7C9,_tmp7C8,_tag_fat(_tmpB3,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpB4=0U;({unsigned _tmp7CB=loc;struct _fat_ptr _tmp7CA=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7CB,_tmp7CA,_tag_fat(_tmpB4,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpB5=0U;({unsigned _tmp7CD=loc;struct _fat_ptr _tmp7CC=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7CD,_tmp7CC,_tag_fat(_tmpB5,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpB6=0U;({unsigned _tmp7CF=loc;struct _fat_ptr _tmp7CE=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7CF,_tmp7CE,_tag_fat(_tmpB6,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 645
return s1;}
# 651
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 659
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpB7=0U;({unsigned _tmp7D1=loc;struct _fat_ptr _tmp7D0=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7D1,_tmp7D0,_tag_fat(_tmpB7,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpB8=0U;({unsigned _tmp7D3=loc;struct _fat_ptr _tmp7D2=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7D3,_tmp7D2,_tag_fat(_tmpB8,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 671
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpB9=0U;({unsigned _tmp7D5=loc;struct _fat_ptr _tmp7D4=({const char*_tmpBA="missing type within specifier";_tag_fat(_tmpBA,sizeof(char),30U);});Cyc_Warn_warn(_tmp7D5,_tmp7D4,_tag_fat(_tmpB9,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 676
if(seen_sign){
void*_tmpBB=t;enum Cyc_Absyn_Size_of _tmpBD;enum Cyc_Absyn_Sign _tmpBC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->f1)->tag == 1U){_LL1: _tmpBC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->f1)->f1;_tmpBD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpBC;enum Cyc_Absyn_Size_of sz2=_tmpBD;
# 679
if((int)sgn2 != (int)sgn)
t=Cyc_Absyn_int_type(sgn,sz2);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpBE=0U;({unsigned _tmp7D7=loc;struct _fat_ptr _tmp7D6=({const char*_tmpBF="sign specification on non-integral type";_tag_fat(_tmpBF,sizeof(char),40U);});Cyc_Warn_err(_tmp7D7,_tmp7D6,_tag_fat(_tmpBE,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 684
if(seen_size){
void*_tmpC0=t;enum Cyc_Absyn_Size_of _tmpC2;enum Cyc_Absyn_Sign _tmpC1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)){case 1U: _LL6: _tmpC1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)->f1;_tmpC2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)->f2;_LL7: {enum Cyc_Absyn_Sign sgn2=_tmpC1;enum Cyc_Absyn_Size_of sz2=_tmpC2;
# 687
if((int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL5;}case 2U: _LL8: _LL9:
# 691
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpC3=0U;({unsigned _tmp7D9=loc;struct _fat_ptr _tmp7D8=({const char*_tmpC4="size qualifier on non-integral type";_tag_fat(_tmpC4,sizeof(char),36U);});Cyc_Warn_err(_tmp7D9,_tmp7D8,_tag_fat(_tmpC3,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 695
return t;}
# 698
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 702
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpC5=d.id;struct _tuple0*q=_tmpC5;
unsigned _tmpC6=d.varloc;unsigned varloc=_tmpC6;
struct _tuple14 _tmpC7=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpC7;struct _tuple14 _tmpC8=_stmttmpC;struct Cyc_List_List*_tmpCC;struct Cyc_List_List*_tmpCB;void*_tmpCA;struct Cyc_Absyn_Tqual _tmpC9;_LL1: _tmpC9=_tmpC8.f1;_tmpCA=_tmpC8.f2;_tmpCB=_tmpC8.f3;_tmpCC=_tmpC8.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpC9;void*new_typ=_tmpCA;struct Cyc_List_List*tvs=_tmpCB;struct Cyc_List_List*atts=_tmpCC;
# 709
struct Cyc_List_List*_tmpCD=ds->tl == 0?0:({struct _RegionHandle*_tmp7DD=r;struct Cyc_Absyn_Tqual _tmp7DC=tq;void*_tmp7DB=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7DA=ds->tl;Cyc_Parse_apply_tmss(_tmp7DD,_tmp7DC,_tmp7DB,_tmp7DA,shared_atts);});struct Cyc_List_List*tl=_tmpCD;
return({struct Cyc_List_List*_tmpCF=_region_malloc(r,sizeof(*_tmpCF));({struct _tuple16*_tmp7DE=({struct _tuple16*_tmpCE=_region_malloc(r,sizeof(*_tmpCE));_tmpCE->f1=varloc,_tmpCE->f2=q,_tmpCE->f3=tq2,_tmpCE->f4=new_typ,_tmpCE->f5=tvs,_tmpCE->f6=atts;_tmpCE;});_tmpCF->hd=_tmp7DE;}),_tmpCF->tl=tl;_tmpCF;});}}}
# 713
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 716
if(tms == 0)return({struct _tuple14 _tmp6DF;_tmp6DF.f1=tq,_tmp6DF.f2=t,_tmp6DF.f3=0,_tmp6DF.f4=atts;_tmp6DF;});{
void*_tmpD0=(void*)tms->hd;void*_stmttmpD=_tmpD0;void*_tmpD1=_stmttmpD;struct Cyc_List_List*_tmpD3;unsigned _tmpD2;struct Cyc_Absyn_Tqual _tmpD5;struct Cyc_Absyn_PtrAtts _tmpD4;unsigned _tmpD7;struct Cyc_List_List*_tmpD6;void*_tmpD8;unsigned _tmpDB;void*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;unsigned _tmpDD;void*_tmpDC;switch(*((int*)_tmpD1)){case 0U: _LL1: _tmpDC=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpDD=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LL2: {void*zeroterm=_tmpDC;unsigned ztloc=_tmpDD;
# 719
return({struct Cyc_Absyn_Tqual _tmp7E1=Cyc_Absyn_empty_tqual(0U);void*_tmp7E0=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 719
struct Cyc_List_List*_tmp7DF=atts;Cyc_Parse_apply_tms(_tmp7E1,_tmp7E0,_tmp7DF,tms->tl);});}case 1U: _LL3: _tmpD9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpDA=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_tmpDB=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpD9;void*zeroterm=_tmpDA;unsigned ztloc=_tmpDB;
# 722
return({struct Cyc_Absyn_Tqual _tmp7E4=Cyc_Absyn_empty_tqual(0U);void*_tmp7E3=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 722
struct Cyc_List_List*_tmp7E2=atts;Cyc_Parse_apply_tms(_tmp7E4,_tmp7E3,_tmp7E2,tms->tl);});}case 3U: _LL5: _tmpD8=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_LL6: {void*args=_tmpD8;
# 725
void*_tmpDE=args;unsigned _tmpDF;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE4;void*_tmpE3;struct Cyc_Absyn_VarargInfo*_tmpE2;int _tmpE1;struct Cyc_List_List*_tmpE0;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->tag == 1U){_LLE: _tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f1;_tmpE1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f2;_tmpE2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f3;_tmpE3=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f4;_tmpE4=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f5;_tmpE5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f6;_tmpE6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpDE)->f7;_LLF: {struct Cyc_List_List*args2=_tmpE0;int c_vararg=_tmpE1;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpE2;void*eff=_tmpE3;struct Cyc_List_List*rgn_po=_tmpE4;struct Cyc_Absyn_Exp*req=_tmpE5;struct Cyc_Absyn_Exp*ens=_tmpE6;
# 727
struct Cyc_List_List*typvars=0;
# 729
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->hd=(void*)as->hd,_tmpE7->tl=fn_atts;_tmpE7;});else{
# 734
new_atts=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(void*)as->hd,_tmpE8->tl=new_atts;_tmpE8;});}}}
# 737
if(tms->tl != 0){
void*_tmpE9=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpE9;void*_tmpEA=_stmttmpE;struct Cyc_List_List*_tmpEB;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEA)->tag == 4U){_LL13: _tmpEB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpEA)->f1;_LL14: {struct Cyc_List_List*ts=_tmpEB;
# 740
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;}
# 746
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 751
args2=0;{
# 754
struct Cyc_List_List*_tmpEC=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpEC;
# 756
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 761
{struct Cyc_List_List*_tmpED=args2;struct Cyc_List_List*a=_tmpED;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpEE=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpEE;struct _tuple8*_tmpEF=_stmttmpF;void**_tmpF2;struct Cyc_Absyn_Tqual _tmpF1;struct _fat_ptr*_tmpF0;_LL18: _tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;_tmpF2=(void**)& _tmpEF->f3;_LL19: {struct _fat_ptr*vopt=_tmpF0;struct Cyc_Absyn_Tqual tq=_tmpF1;void**t=_tmpF2;
if(tags != 0)
({void*_tmp7E5=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7E5;});
({void*_tmp7E6=Cyc_Parse_array2ptr(*t,1);*t=_tmp7E6;});}}}
# 773
return({struct Cyc_Absyn_Tqual _tmp7E9=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7E8=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 773
struct Cyc_List_List*_tmp7E7=new_atts;Cyc_Parse_apply_tms(_tmp7E9,_tmp7E8,_tmp7E7,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpDF=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpDE)->f2;_LL11: {unsigned loc=_tmpDF;
# 780
({void*_tmpF3=0U;({unsigned _tmp7EB=loc;struct _fat_ptr _tmp7EA=({const char*_tmpF4="function declaration without parameter types";_tag_fat(_tmpF4,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7EB,_tmp7EA,_tag_fat(_tmpF3,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpD7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD6;unsigned loc=_tmpD7;
# 787
if(tms->tl == 0)
return({struct _tuple14 _tmp6E0;_tmp6E0.f1=tq,_tmp6E0.f2=t,_tmp6E0.f3=ts,_tmp6E0.f4=atts;_tmp6E0;});
# 792
({void*_tmpF5=0U;({unsigned _tmp7ED=loc;struct _fat_ptr _tmp7EC=({const char*_tmpF6="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF6,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7ED,_tmp7EC,_tag_fat(_tmpF5,sizeof(void*),0U));});});}case 2U: _LL9: _tmpD4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpD5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpD4;struct Cyc_Absyn_Tqual tq2=_tmpD5;
# 795
return({struct Cyc_Absyn_Tqual _tmp7F0=tq2;void*_tmp7EF=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6E1;_tmp6E1.elt_type=t,_tmp6E1.elt_tq=tq,_tmp6E1.ptr_atts=ptratts;_tmp6E1;}));struct Cyc_List_List*_tmp7EE=atts;Cyc_Parse_apply_tms(_tmp7F0,_tmp7EF,_tmp7EE,tms->tl);});}default: _LLB: _tmpD2=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1)->f1;_tmpD3=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1)->f2;_LLC: {unsigned loc=_tmpD2;struct Cyc_List_List*atts2=_tmpD3;
# 800
return({struct Cyc_Absyn_Tqual _tmp7F3=tq;void*_tmp7F2=t;struct Cyc_List_List*_tmp7F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7F3,_tmp7F2,_tmp7F1,tms->tl);});}}_LL0:;}}
# 806
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 815
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpF7=t;struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFC;void*_tmpFB;struct Cyc_Absyn_Tqual _tmpFA;struct _tuple0*_tmpF9;unsigned _tmpF8;_LL1: _tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;_tmpFA=_tmpF7->f3;_tmpFB=_tmpF7->f4;_tmpFC=_tmpF7->f5;_tmpFD=_tmpF7->f6;_LL2: {unsigned varloc=_tmpF8;struct _tuple0*x=_tmpF9;struct Cyc_Absyn_Tqual tq=_tmpFA;void*typ=_tmpFB;struct Cyc_List_List*tvs=_tmpFC;struct Cyc_List_List*atts=_tmpFD;
# 818
Cyc_Lex_register_typedef(x);{
# 820
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpFE=typ;struct Cyc_Core_Opt*_tmpFF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFE)->tag == 1U){_LL4: _tmpFF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFE)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmpFF;
# 824
type=0;
kind=kopt == 0?& Cyc_Tcutil_bko: kopt;
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 829
return({void*_tmp7F5=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7F4=({struct Cyc_Absyn_Typedefdecl*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->name=x,_tmp100->tvs=tvs,_tmp100->kind=kind,_tmp100->defn=type,_tmp100->atts=atts,_tmp100->tq=tq,_tmp100->extern_c=0;_tmp100;});_tmp101->f1=_tmp7F4;});_tmp101;});Cyc_Absyn_new_decl(_tmp7F5,loc);});}}}
# 836
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7F6=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=12U,_tmp102->f1=d,_tmp102->f2=s;_tmp102;});Cyc_Absyn_new_stmt(_tmp7F6,d->loc);});}
# 840
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 849
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 853
struct _RegionHandle _tmp103=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp103;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp104=ds;struct Cyc_List_List*_tmp107;struct Cyc_Parse_Type_specifier _tmp106;struct Cyc_Absyn_Tqual _tmp105;_LL1: _tmp105=_tmp104.tq;_tmp106=_tmp104.type_specs;_tmp107=_tmp104.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp105;struct Cyc_Parse_Type_specifier tss=_tmp106;struct Cyc_List_List*atts=_tmp107;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp108=0U;({unsigned _tmp7F8=loc;struct _fat_ptr _tmp7F7=({const char*_tmp109="inline qualifier on non-function definition";_tag_fat(_tmp109,sizeof(char),44U);});Cyc_Warn_warn(_tmp7F8,_tmp7F7,_tag_fat(_tmp108,sizeof(void*),0U));});});{
# 859
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp10A=*ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp10A;enum Cyc_Parse_Storage_class _tmp10B=_stmttmp10;switch(_tmp10B){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 s=Cyc_Flags_no_register?Cyc_Absyn_Public: Cyc_Absyn_Register;goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 goto _LL13;default: _LL12: _LL13:
 s=1U;goto _LL3;}_LL3:;}{
# 876
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 880
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 888
void*_tmp10C=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp10C;
if(declarators == 0){
# 892
void*_tmp10D=base_type;struct Cyc_List_List*_tmp10E;struct _tuple0*_tmp10F;struct Cyc_List_List*_tmp112;int _tmp111;struct _tuple0*_tmp110;struct Cyc_Absyn_Datatypedecl**_tmp113;struct Cyc_List_List*_tmp116;struct _tuple0*_tmp115;enum Cyc_Absyn_AggrKind _tmp114;struct Cyc_Absyn_Datatypedecl*_tmp117;struct Cyc_Absyn_Enumdecl*_tmp118;struct Cyc_Absyn_Aggrdecl*_tmp119;switch(*((int*)_tmp10D)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)){case 0U: _LL15: _tmp119=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp119;
# 894
({struct Cyc_List_List*_tmp7F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7F9;});
ad->sc=s;{
struct Cyc_List_List*_tmp11C=({struct Cyc_List_List*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_Absyn_Decl*_tmp7FB=({void*_tmp7FA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=5U,_tmp11A->f1=ad;_tmp11A;});Cyc_Absyn_new_decl(_tmp7FA,loc);});_tmp11B->hd=_tmp7FB;}),_tmp11B->tl=0;_tmp11B;});_npop_handler(0U);return _tmp11C;}}case 1U: _LL17: _tmp118=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp118;
# 898
if(atts != 0)({void*_tmp11D=0U;({unsigned _tmp7FD=loc;struct _fat_ptr _tmp7FC=({const char*_tmp11E="attributes on enum not supported";_tag_fat(_tmp11E,sizeof(char),33U);});Cyc_Warn_err(_tmp7FD,_tmp7FC,_tag_fat(_tmp11D,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp121=({struct Cyc_List_List*_tmp120=_cycalloc(sizeof(*_tmp120));({struct Cyc_Absyn_Decl*_tmp7FF=({void*_tmp7FE=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=7U,_tmp11F->f1=ed;_tmp11F;});Cyc_Absyn_new_decl(_tmp7FE,loc);});_tmp120->hd=_tmp7FF;}),_tmp120->tl=0;_tmp120;});_npop_handler(0U);return _tmp121;}}default: _LL19: _tmp117=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10D)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp117;
# 902
if(atts != 0)({void*_tmp122=0U;({unsigned _tmp801=loc;struct _fat_ptr _tmp800=({const char*_tmp123="attributes on datatypes not supported";_tag_fat(_tmp123,sizeof(char),38U);});Cyc_Warn_err(_tmp801,_tmp800,_tag_fat(_tmp122,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp126=({struct Cyc_List_List*_tmp125=_cycalloc(sizeof(*_tmp125));({struct Cyc_Absyn_Decl*_tmp803=({void*_tmp802=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=6U,_tmp124->f1=dd;_tmp124;});Cyc_Absyn_new_decl(_tmp802,loc);});_tmp125->hd=_tmp803;}),_tmp125->tl=0;_tmp125;});_npop_handler(0U);return _tmp126;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp114=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownAggr).val).f1;_tmp115=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownAggr).val).f2;_tmp116=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp114;struct _tuple0*n=_tmp115;struct Cyc_List_List*ts=_tmp116;
# 906
struct Cyc_List_List*_tmp127=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp127;
struct Cyc_Absyn_Aggrdecl*_tmp128=({struct Cyc_Absyn_Aggrdecl*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->kind=k,_tmp12E->sc=s,_tmp12E->name=n,_tmp12E->tvs=ts2,_tmp12E->impl=0,_tmp12E->attributes=0,_tmp12E->expected_mem_kind=0;_tmp12E;});struct Cyc_Absyn_Aggrdecl*ad=_tmp128;
if(atts != 0)({void*_tmp129=0U;({unsigned _tmp805=loc;struct _fat_ptr _tmp804=({const char*_tmp12A="bad attributes on type declaration";_tag_fat(_tmp12A,sizeof(char),35U);});Cyc_Warn_err(_tmp805,_tmp804,_tag_fat(_tmp129,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12D=({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct Cyc_Absyn_Decl*_tmp807=({void*_tmp806=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=5U,_tmp12B->f1=ad;_tmp12B;});Cyc_Absyn_new_decl(_tmp806,loc);});_tmp12C->hd=_tmp807;}),_tmp12C->tl=0;_tmp12C;});_npop_handler(0U);return _tmp12D;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp113=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp113;
# 911
if(atts != 0)({void*_tmp12F=0U;({unsigned _tmp809=loc;struct _fat_ptr _tmp808=({const char*_tmp130="bad attributes on datatype";_tag_fat(_tmp130,sizeof(char),27U);});Cyc_Warn_err(_tmp809,_tmp808,_tag_fat(_tmp12F,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));({struct Cyc_Absyn_Decl*_tmp80B=({void*_tmp80A=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=6U,_tmp131->f1=*tudp;_tmp131;});Cyc_Absyn_new_decl(_tmp80A,loc);});_tmp132->hd=_tmp80B;}),_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;}}}else{_LL1F: _tmp110=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownDatatype).val).name;_tmp111=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp112=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f2;_LL20: {struct _tuple0*n=_tmp110;int isx=_tmp111;struct Cyc_List_List*ts=_tmp112;
# 914
struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp134;
struct Cyc_Absyn_Decl*_tmp135=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp135;
if(atts != 0)({void*_tmp136=0U;({unsigned _tmp80D=loc;struct _fat_ptr _tmp80C=({const char*_tmp137="bad attributes on datatype";_tag_fat(_tmp137,sizeof(char),27U);});Cyc_Warn_err(_tmp80D,_tmp80C,_tag_fat(_tmp136,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp139=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->hd=tud,_tmp138->tl=0;_tmp138;});_npop_handler(0U);return _tmp139;}}}case 15U: _LL21: _tmp10F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_LL22: {struct _tuple0*n=_tmp10F;
# 919
struct Cyc_Absyn_Enumdecl*_tmp13A=({struct Cyc_Absyn_Enumdecl*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->sc=s,_tmp141->name=n,_tmp141->fields=0;_tmp141;});struct Cyc_Absyn_Enumdecl*ed=_tmp13A;
if(atts != 0)({void*_tmp13B=0U;({unsigned _tmp80F=loc;struct _fat_ptr _tmp80E=({const char*_tmp13C="bad attributes on enum";_tag_fat(_tmp13C,sizeof(char),23U);});Cyc_Warn_err(_tmp80F,_tmp80E,_tag_fat(_tmp13B,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp140=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct Cyc_Absyn_Decl*_tmp811=({struct Cyc_Absyn_Decl*_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp810=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->tag=7U,_tmp13D->f1=ed;_tmp13D;});_tmp13E->r=_tmp810;}),_tmp13E->loc=loc;_tmp13E;});_tmp13F->hd=_tmp811;}),_tmp13F->tl=0;_tmp13F;});_npop_handler(0U);return _tmp140;}}case 16U: _LL23: _tmp10E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10D)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp10E;
# 925
struct Cyc_Absyn_Enumdecl*_tmp142=({struct Cyc_Absyn_Enumdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->sc=s,({struct _tuple0*_tmp813=Cyc_Parse_gensym_enum();_tmp14A->name=_tmp813;}),({struct Cyc_Core_Opt*_tmp812=({struct Cyc_Core_Opt*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->v=fs;_tmp149;});_tmp14A->fields=_tmp812;});_tmp14A;});struct Cyc_Absyn_Enumdecl*ed=_tmp142;
if(atts != 0)({void*_tmp143=0U;({unsigned _tmp815=loc;struct _fat_ptr _tmp814=({const char*_tmp144="bad attributes on enum";_tag_fat(_tmp144,sizeof(char),23U);});Cyc_Warn_err(_tmp815,_tmp814,_tag_fat(_tmp143,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp148=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));({struct Cyc_Absyn_Decl*_tmp817=({struct Cyc_Absyn_Decl*_tmp146=_cycalloc(sizeof(*_tmp146));({void*_tmp816=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->tag=7U,_tmp145->f1=ed;_tmp145;});_tmp146->r=_tmp816;}),_tmp146->loc=loc;_tmp146;});_tmp147->hd=_tmp817;}),_tmp147->tl=0;_tmp147;});_npop_handler(0U);return _tmp148;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp14B=0U;({unsigned _tmp819=loc;struct _fat_ptr _tmp818=({const char*_tmp14C="missing declarator";_tag_fat(_tmp14C,sizeof(char),19U);});Cyc_Warn_err(_tmp819,_tmp818,_tag_fat(_tmp14B,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp14D=0;_npop_handler(0U);return _tmp14D;}}_LL14:;}else{
# 932
struct Cyc_List_List*_tmp14E=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp14E;
if(istypedef){
# 937
if(!exps_empty)
({void*_tmp14F=0U;({unsigned _tmp81B=loc;struct _fat_ptr _tmp81A=({const char*_tmp150="initializer in typedef declaration";_tag_fat(_tmp150,sizeof(char),35U);});Cyc_Warn_err(_tmp81B,_tmp81A,_tag_fat(_tmp14F,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp151=decls;_npop_handler(0U);return _tmp151;}}else{
# 943
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp152=fields;struct Cyc_List_List*ds=_tmp152;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp153=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp153;struct _tuple16*_tmp154=_stmttmp11;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp159;void*_tmp158;struct Cyc_Absyn_Tqual _tmp157;struct _tuple0*_tmp156;unsigned _tmp155;_LL28: _tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_tmp158=_tmp154->f4;_tmp159=_tmp154->f5;_tmp15A=_tmp154->f6;_LL29: {unsigned varloc=_tmp155;struct _tuple0*x=_tmp156;struct Cyc_Absyn_Tqual tq2=_tmp157;void*t2=_tmp158;struct Cyc_List_List*tvs2=_tmp159;struct Cyc_List_List*atts2=_tmp15A;
if(tvs2 != 0)
({void*_tmp15B=0U;({unsigned _tmp81D=loc;struct _fat_ptr _tmp81C=({const char*_tmp15C="bad type params, ignoring";_tag_fat(_tmp15C,sizeof(char),26U);});Cyc_Warn_warn(_tmp81D,_tmp81C,_tag_fat(_tmp15B,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp15D=0U;({unsigned _tmp81F=loc;struct _fat_ptr _tmp81E=({const char*_tmp15E="unexpected NULL in parse!";_tag_fat(_tmp15E,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp81F,_tmp81E,_tag_fat(_tmp15D,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp15F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp15F;
struct Cyc_Absyn_Vardecl*_tmp160=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp160;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp161=({struct Cyc_Absyn_Decl*_tmp164=_cycalloc(sizeof(*_tmp164));({void*_tmp820=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->tag=0U,_tmp163->f1=vd;_tmp163;});_tmp164->r=_tmp820;}),_tmp164->loc=loc;_tmp164;});struct Cyc_Absyn_Decl*d=_tmp161;
decls=({struct Cyc_List_List*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->hd=d,_tmp162->tl=decls;_tmp162;});}}}}}{
# 958
struct Cyc_List_List*_tmp165=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp165;}}}}}}}}}
# 854
;_pop_region();}
# 964
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen((struct _fat_ptr)s)== (unsigned long)1 || Cyc_strlen((struct _fat_ptr)s)== (unsigned long)2){
char _tmp166=*((const char*)_check_fat_subscript(s,sizeof(char),0));char _stmttmp12=_tmp166;char _tmp167=_stmttmp12;switch(_tmp167){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 974
{char _tmp168=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _stmttmp13=_tmp168;char _tmp169=_stmttmp13;switch(_tmp169){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 981
goto _LL0;case 84U: _LLF: _LL10:
# 983
{char _tmp16A=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _stmttmp14=_tmp16A;char _tmp16B=_stmttmp14;switch(_tmp16B){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 990
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 993
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E6;_tmp6E6.tag=0U,({struct _fat_ptr _tmp821=({const char*_tmp173="bad kind: ";_tag_fat(_tmp173,sizeof(char),11U);});_tmp6E6.f1=_tmp821;});_tmp6E6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E5;_tmp6E5.tag=0U,_tmp6E5.f1=s;_tmp6E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E4;_tmp6E4.tag=0U,({struct _fat_ptr _tmp822=({const char*_tmp172=" strlen=";_tag_fat(_tmp172,sizeof(char),9U);});_tmp6E4.f1=_tmp822;});_tmp6E4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp170=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E3;_tmp6E3.tag=0U,_tmp6E3.f1=s;_tmp6E3;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp171=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp6E2;_tmp6E2.tag=12U,({int _tmp823=(int)Cyc_strlen((struct _fat_ptr)s);_tmp6E2.f1=_tmp823;});_tmp6E2;});void*_tmp16C[5U];_tmp16C[0]=& _tmp16D,_tmp16C[1]=& _tmp16E,_tmp16C[2]=& _tmp16F,_tmp16C[3]=& _tmp170,_tmp16C[4]=& _tmp171;({unsigned _tmp824=loc;Cyc_Warn_err2(_tmp824,_tag_fat(_tmp16C,sizeof(void*),5U));});});
return& Cyc_Tcutil_bk;}
# 998
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp174=e->r;void*_stmttmp15=_tmp174;void*_tmp175=_stmttmp15;int _tmp176;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp175)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp175)->f1).Int_c).tag == 5){_LL1: _tmp176=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp175)->f1).Int_c).val).f2;_LL2: {int i=_tmp176;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1002
({void*_tmp177=0U;({unsigned _tmp826=loc;struct _fat_ptr _tmp825=({const char*_tmp178="expecting integer constant";_tag_fat(_tmp178,sizeof(char),27U);});Cyc_Warn_err(_tmp826,_tmp825,_tag_fat(_tmp177,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 1008
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp179=e->r;void*_stmttmp16=_tmp179;void*_tmp17A=_stmttmp16;struct _fat_ptr _tmp17B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17A)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17A)->f1).String_c).tag == 8){_LL1: _tmp17B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp17A)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp17B;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1012
({void*_tmp17C=0U;({unsigned _tmp828=loc;struct _fat_ptr _tmp827=({const char*_tmp17D="expecting string constant";_tag_fat(_tmp17D,sizeof(char),26U);});Cyc_Warn_err(_tmp828,_tmp827,_tag_fat(_tmp17C,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 1018
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp17E=x;long long _tmp17F;char _tmp180;int _tmp181;switch((_tmp17E.LongLong_c).tag){case 5U: _LL1: _tmp181=((_tmp17E.Int_c).val).f2;_LL2: {int i=_tmp181;
return(unsigned)i;}case 2U: _LL3: _tmp180=((_tmp17E.Char_c).val).f2;_LL4: {char c=_tmp180;
return(unsigned)c;}case 6U: _LL5: _tmp17F=((_tmp17E.LongLong_c).val).f2;_LL6: {long long x=_tmp17F;
# 1023
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp182=0U;({unsigned _tmp82A=loc;struct _fat_ptr _tmp829=({const char*_tmp183="integer constant too large";_tag_fat(_tmp183,sizeof(char),27U);});Cyc_Warn_err(_tmp82A,_tmp829,_tag_fat(_tmp182,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp6E7;_tmp6E7.tag=0U,({struct _fat_ptr _tmp82B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6E7.f1=_tmp82B;});_tmp6E7;});void*_tmp184[1U];_tmp184[0]=& _tmp186;({unsigned _tmp82D=loc;struct _fat_ptr _tmp82C=({const char*_tmp185="expected integer constant but found %s";_tag_fat(_tmp185,sizeof(char),39U);});Cyc_Warn_err(_tmp82D,_tmp82C,_tag_fat(_tmp184,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp187=p->r;void*_stmttmp17=_tmp187;void*_tmp188=_stmttmp17;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_List_List*_tmp18B;struct _tuple0*_tmp18A;int _tmp18D;struct _fat_ptr _tmp18C;char _tmp18E;int _tmp190;enum Cyc_Absyn_Sign _tmp18F;struct Cyc_Absyn_Pat*_tmp191;struct Cyc_Absyn_Vardecl*_tmp192;struct _tuple0*_tmp193;switch(*((int*)_tmp188)){case 15U: _LL1: _tmp193=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_LL2: {struct _tuple0*x=_tmp193;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp188)->f2)->r)->tag == 0U){_LL3: _tmp192=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp192;
# 1038
return({struct Cyc_Absyn_Exp*_tmp82E=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp82E,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp191=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp191;
return({struct Cyc_Absyn_Exp*_tmp82F=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp82F,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp18F=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_tmp190=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp188)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp18F;int i=_tmp190;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp18E=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_LLC: {char c=_tmp18E;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp18C=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_tmp18D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp188)->f2;_LLE: {struct _fat_ptr s=_tmp18C;int i=_tmp18D;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp188)->f3 == 0){_LLF: _tmp18A=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_tmp18B=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp188)->f2;_LL10: {struct _tuple0*x=_tmp18A;struct Cyc_List_List*ps=_tmp18B;
# 1045
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp189=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp188)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp189;
return e;}default: _LL13: _LL14:
# 1050
({void*_tmp194=0U;({unsigned _tmp831=p->loc;struct _fat_ptr _tmp830=({const char*_tmp195="cannot mix patterns and expressions in case";_tag_fat(_tmp195,sizeof(char),44U);});Cyc_Warn_err(_tmp831,_tmp830,_tag_fat(_tmp194,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1135
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->tag=Cyc_Core_Failure,_tmp196->f1=s;_tmp196;}));}static char _tmp199[7U]="cnst_t";
# 1107 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp199,_tmp199,_tmp199 + 7U};
union Cyc_YYSTYPE*_tmp197=yy1;union Cyc_Absyn_Cnst _tmp198;if((((union Cyc_YYSTYPE*)_tmp197)->Int_tok).tag == 1){_LL1: _tmp198=(_tmp197->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp198;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1114
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.Int_tok).tag=1U,(_tmp6E8.Int_tok).val=yy1;_tmp6E8;});}static char _tmp19C[5U]="char";
# 1108 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19C,_tmp19C,_tmp19C + 5U};
union Cyc_YYSTYPE*_tmp19A=yy1;char _tmp19B;if((((union Cyc_YYSTYPE*)_tmp19A)->Char_tok).tag == 2){_LL1: _tmp19B=(_tmp19A->Char_tok).val;_LL2: {char yy=_tmp19B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1115
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.Char_tok).tag=2U,(_tmp6E9.Char_tok).val=yy1;_tmp6E9;});}static char _tmp19F[13U]="string_t<`H>";
# 1109 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19F,_tmp19F,_tmp19F + 13U};
union Cyc_YYSTYPE*_tmp19D=yy1;struct _fat_ptr _tmp19E;if((((union Cyc_YYSTYPE*)_tmp19D)->String_tok).tag == 3){_LL1: _tmp19E=(_tmp19D->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp19E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1116
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.String_tok).tag=3U,(_tmp6EA.String_tok).val=yy1;_tmp6EA;});}static char _tmp1A2[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1112 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A2,_tmp1A2,_tmp1A2 + 45U};
union Cyc_YYSTYPE*_tmp1A0=yy1;struct _tuple22*_tmp1A1;if((((union Cyc_YYSTYPE*)_tmp1A0)->YY1).tag == 9){_LL1: _tmp1A1=(_tmp1A0->YY1).val;_LL2: {struct _tuple22*yy=_tmp1A1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY1).tag=9U,(_tmp6EB.YY1).val=yy1;_tmp6EB;});}static char _tmp1A5[11U]="ptrbound_t";
# 1113 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A5,_tmp1A5,_tmp1A5 + 11U};
union Cyc_YYSTYPE*_tmp1A3=yy1;void*_tmp1A4;if((((union Cyc_YYSTYPE*)_tmp1A3)->YY2).tag == 10){_LL1: _tmp1A4=(_tmp1A3->YY2).val;_LL2: {void*yy=_tmp1A4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY2).tag=10U,(_tmp6EC.YY2).val=yy1;_tmp6EC;});}static char _tmp1A8[28U]="list_t<offsetof_field_t,`H>";
# 1114 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A8,_tmp1A8,_tmp1A8 + 28U};
union Cyc_YYSTYPE*_tmp1A6=yy1;struct Cyc_List_List*_tmp1A7;if((((union Cyc_YYSTYPE*)_tmp1A6)->YY3).tag == 11){_LL1: _tmp1A7=(_tmp1A6->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp1A7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY3).tag=11U,(_tmp6ED.YY3).val=yy1;_tmp6ED;});}static char _tmp1AB[6U]="exp_t";
# 1115 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AB,_tmp1AB,_tmp1AB + 6U};
union Cyc_YYSTYPE*_tmp1A9=yy1;struct Cyc_Absyn_Exp*_tmp1AA;if((((union Cyc_YYSTYPE*)_tmp1A9)->Exp_tok).tag == 7){_LL1: _tmp1AA=(_tmp1A9->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp1AA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.Exp_tok).tag=7U,(_tmp6EE.Exp_tok).val=yy1;_tmp6EE;});}static char _tmp1AE[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AE,_tmp1AE,_tmp1AE + 17U};
union Cyc_YYSTYPE*_tmp1AC=yy1;struct Cyc_List_List*_tmp1AD;if((((union Cyc_YYSTYPE*)_tmp1AC)->YY4).tag == 12){_LL1: _tmp1AD=(_tmp1AC->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp1AD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY4).tag=12U,(_tmp6EF.YY4).val=yy1;_tmp6EF;});}static char _tmp1B1[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B1,_tmp1B1,_tmp1B1 + 47U};
union Cyc_YYSTYPE*_tmp1AF=yy1;struct Cyc_List_List*_tmp1B0;if((((union Cyc_YYSTYPE*)_tmp1AF)->YY5).tag == 13){_LL1: _tmp1B0=(_tmp1AF->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1B0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY5).tag=13U,(_tmp6F0.YY5).val=yy1;_tmp6F0;});}static char _tmp1B4[9U]="primop_t";
# 1125 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B4,_tmp1B4,_tmp1B4 + 9U};
union Cyc_YYSTYPE*_tmp1B2=yy1;enum Cyc_Absyn_Primop _tmp1B3;if((((union Cyc_YYSTYPE*)_tmp1B2)->YY6).tag == 14){_LL1: _tmp1B3=(_tmp1B2->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1B3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1132
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY6).tag=14U,(_tmp6F1.YY6).val=yy1;_tmp6F1;});}static char _tmp1B7[19U]="opt_t<primop_t,`H>";
# 1126 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B7,_tmp1B7,_tmp1B7 + 19U};
union Cyc_YYSTYPE*_tmp1B5=yy1;struct Cyc_Core_Opt*_tmp1B6;if((((union Cyc_YYSTYPE*)_tmp1B5)->YY7).tag == 15){_LL1: _tmp1B6=(_tmp1B5->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1B6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1133
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY7).tag=15U,(_tmp6F2.YY7).val=yy1;_tmp6F2;});}static char _tmp1BA[7U]="qvar_t";
# 1127 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BA,_tmp1BA,_tmp1BA + 7U};
union Cyc_YYSTYPE*_tmp1B8=yy1;struct _tuple0*_tmp1B9;if((((union Cyc_YYSTYPE*)_tmp1B8)->QualId_tok).tag == 5){_LL1: _tmp1B9=(_tmp1B8->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1B9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1134
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.QualId_tok).tag=5U,(_tmp6F3.QualId_tok).val=yy1;_tmp6F3;});}static char _tmp1BD[7U]="stmt_t";
# 1130 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BD,_tmp1BD,_tmp1BD + 7U};
union Cyc_YYSTYPE*_tmp1BB=yy1;struct Cyc_Absyn_Stmt*_tmp1BC;if((((union Cyc_YYSTYPE*)_tmp1BB)->Stmt_tok).tag == 8){_LL1: _tmp1BC=(_tmp1BB->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1BC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.Stmt_tok).tag=8U,(_tmp6F4.Stmt_tok).val=yy1;_tmp6F4;});}static char _tmp1C0[27U]="list_t<switch_clause_t,`H>";
# 1134 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C0,_tmp1C0,_tmp1C0 + 27U};
union Cyc_YYSTYPE*_tmp1BE=yy1;struct Cyc_List_List*_tmp1BF;if((((union Cyc_YYSTYPE*)_tmp1BE)->YY8).tag == 16){_LL1: _tmp1BF=(_tmp1BE->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1BF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY8).tag=16U,(_tmp6F5.YY8).val=yy1;_tmp6F5;});}static char _tmp1C3[6U]="pat_t";
# 1135 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C3,_tmp1C3,_tmp1C3 + 6U};
union Cyc_YYSTYPE*_tmp1C1=yy1;struct Cyc_Absyn_Pat*_tmp1C2;if((((union Cyc_YYSTYPE*)_tmp1C1)->YY9).tag == 17){_LL1: _tmp1C2=(_tmp1C1->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1C2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY9).tag=17U,(_tmp6F6.YY9).val=yy1;_tmp6F6;});}static char _tmp1C6[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1140 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C6,_tmp1C6,_tmp1C6 + 28U};
union Cyc_YYSTYPE*_tmp1C4=yy1;struct _tuple23*_tmp1C5;if((((union Cyc_YYSTYPE*)_tmp1C4)->YY10).tag == 18){_LL1: _tmp1C5=(_tmp1C4->YY10).val;_LL2: {struct _tuple23*yy=_tmp1C5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1147
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY10).tag=18U,(_tmp6F7.YY10).val=yy1;_tmp6F7;});}static char _tmp1C9[17U]="list_t<pat_t,`H>";
# 1141 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C9,_tmp1C9,_tmp1C9 + 17U};
union Cyc_YYSTYPE*_tmp1C7=yy1;struct Cyc_List_List*_tmp1C8;if((((union Cyc_YYSTYPE*)_tmp1C7)->YY11).tag == 19){_LL1: _tmp1C8=(_tmp1C7->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1C8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1148
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY11).tag=19U,(_tmp6F8.YY11).val=yy1;_tmp6F8;});}static char _tmp1CC[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1142 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CC,_tmp1CC,_tmp1CC + 36U};
union Cyc_YYSTYPE*_tmp1CA=yy1;struct _tuple24*_tmp1CB;if((((union Cyc_YYSTYPE*)_tmp1CA)->YY12).tag == 20){_LL1: _tmp1CB=(_tmp1CA->YY12).val;_LL2: {struct _tuple24*yy=_tmp1CB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1149
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY12).tag=20U,(_tmp6F9.YY12).val=yy1;_tmp6F9;});}static char _tmp1CF[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1143 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CF,_tmp1CF,_tmp1CF + 47U};
union Cyc_YYSTYPE*_tmp1CD=yy1;struct Cyc_List_List*_tmp1CE;if((((union Cyc_YYSTYPE*)_tmp1CD)->YY13).tag == 21){_LL1: _tmp1CE=(_tmp1CD->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1CE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1150
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY13).tag=21U,(_tmp6FA.YY13).val=yy1;_tmp6FA;});}static char _tmp1D2[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1144 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D2,_tmp1D2,_tmp1D2 + 58U};
union Cyc_YYSTYPE*_tmp1D0=yy1;struct _tuple23*_tmp1D1;if((((union Cyc_YYSTYPE*)_tmp1D0)->YY14).tag == 22){_LL1: _tmp1D1=(_tmp1D0->YY14).val;_LL2: {struct _tuple23*yy=_tmp1D1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1151
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY14).tag=22U,(_tmp6FB.YY14).val=yy1;_tmp6FB;});}static char _tmp1D5[9U]="fndecl_t";
# 1145 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D5,_tmp1D5,_tmp1D5 + 9U};
union Cyc_YYSTYPE*_tmp1D3=yy1;struct Cyc_Absyn_Fndecl*_tmp1D4;if((((union Cyc_YYSTYPE*)_tmp1D3)->YY15).tag == 23){_LL1: _tmp1D4=(_tmp1D3->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1D4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1152
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY15).tag=23U,(_tmp6FC.YY15).val=yy1;_tmp6FC;});}static char _tmp1D8[18U]="list_t<decl_t,`H>";
# 1146 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D8,_tmp1D8,_tmp1D8 + 18U};
union Cyc_YYSTYPE*_tmp1D6=yy1;struct Cyc_List_List*_tmp1D7;if((((union Cyc_YYSTYPE*)_tmp1D6)->YY16).tag == 24){_LL1: _tmp1D7=(_tmp1D6->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1D7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1153
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY16).tag=24U,(_tmp6FD.YY16).val=yy1;_tmp6FD;});}static char _tmp1DB[12U]="decl_spec_t";
# 1149 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DB,_tmp1DB,_tmp1DB + 12U};
union Cyc_YYSTYPE*_tmp1D9=yy1;struct Cyc_Parse_Declaration_spec _tmp1DA;if((((union Cyc_YYSTYPE*)_tmp1D9)->YY17).tag == 25){_LL1: _tmp1DA=(_tmp1D9->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1DA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1156
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY17).tag=25U,(_tmp6FE.YY17).val=yy1;_tmp6FE;});}static char _tmp1DE[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1150 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DE,_tmp1DE,_tmp1DE + 31U};
union Cyc_YYSTYPE*_tmp1DC=yy1;struct _tuple12 _tmp1DD;if((((union Cyc_YYSTYPE*)_tmp1DC)->YY18).tag == 26){_LL1: _tmp1DD=(_tmp1DC->YY18).val;_LL2: {struct _tuple12 yy=_tmp1DD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1157
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY18).tag=26U,(_tmp6FF.YY18).val=yy1;_tmp6FF;});}static char _tmp1E1[23U]="declarator_list_t<`yy>";
# 1151 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E1,_tmp1E1,_tmp1E1 + 23U};
union Cyc_YYSTYPE*_tmp1DF=yy1;struct _tuple13*_tmp1E0;if((((union Cyc_YYSTYPE*)_tmp1DF)->YY19).tag == 27){_LL1: _tmp1E0=(_tmp1DF->YY19).val;_LL2: {struct _tuple13*yy=_tmp1E0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1158
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY19).tag=27U,(_tmp700.YY19).val=yy1;_tmp700;});}static char _tmp1E4[19U]="storage_class_t@`H";
# 1152 "parse.y"
static enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E4,_tmp1E4,_tmp1E4 + 19U};
union Cyc_YYSTYPE*_tmp1E2=yy1;enum Cyc_Parse_Storage_class*_tmp1E3;if((((union Cyc_YYSTYPE*)_tmp1E2)->YY20).tag == 28){_LL1: _tmp1E3=(_tmp1E2->YY20).val;_LL2: {enum Cyc_Parse_Storage_class*yy=_tmp1E3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1159
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY20).tag=28U,(_tmp701.YY20).val=yy1;_tmp701;});}static char _tmp1E7[17U]="type_specifier_t";
# 1153 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E7,_tmp1E7,_tmp1E7 + 17U};
union Cyc_YYSTYPE*_tmp1E5=yy1;struct Cyc_Parse_Type_specifier _tmp1E6;if((((union Cyc_YYSTYPE*)_tmp1E5)->YY21).tag == 29){_LL1: _tmp1E6=(_tmp1E5->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1E6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1160
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY21).tag=29U,(_tmp702.YY21).val=yy1;_tmp702;});}static char _tmp1EA[12U]="aggr_kind_t";
# 1155 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EA,_tmp1EA,_tmp1EA + 12U};
union Cyc_YYSTYPE*_tmp1E8=yy1;enum Cyc_Absyn_AggrKind _tmp1E9;if((((union Cyc_YYSTYPE*)_tmp1E8)->YY22).tag == 30){_LL1: _tmp1E9=(_tmp1E8->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1E9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1162
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY22).tag=30U,(_tmp703.YY22).val=yy1;_tmp703;});}static char _tmp1ED[8U]="tqual_t";
# 1156 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1ED,_tmp1ED,_tmp1ED + 8U};
union Cyc_YYSTYPE*_tmp1EB=yy1;struct Cyc_Absyn_Tqual _tmp1EC;if((((union Cyc_YYSTYPE*)_tmp1EB)->YY23).tag == 31){_LL1: _tmp1EC=(_tmp1EB->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1EC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1163
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY23).tag=31U,(_tmp704.YY23).val=yy1;_tmp704;});}static char _tmp1F0[23U]="list_t<aggrfield_t,`H>";
# 1157 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F0,_tmp1F0,_tmp1F0 + 23U};
union Cyc_YYSTYPE*_tmp1EE=yy1;struct Cyc_List_List*_tmp1EF;if((((union Cyc_YYSTYPE*)_tmp1EE)->YY24).tag == 32){_LL1: _tmp1EF=(_tmp1EE->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1EF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1164
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY24).tag=32U,(_tmp705.YY24).val=yy1;_tmp705;});}static char _tmp1F3[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1158 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F3,_tmp1F3,_tmp1F3 + 34U};
union Cyc_YYSTYPE*_tmp1F1=yy1;struct Cyc_List_List*_tmp1F2;if((((union Cyc_YYSTYPE*)_tmp1F1)->YY25).tag == 33){_LL1: _tmp1F2=(_tmp1F1->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1F2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1165
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY25).tag=33U,(_tmp706.YY25).val=yy1;_tmp706;});}static char _tmp1F6[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1159 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F6,_tmp1F6,_tmp1F6 + 33U};
union Cyc_YYSTYPE*_tmp1F4=yy1;struct Cyc_List_List*_tmp1F5;if((((union Cyc_YYSTYPE*)_tmp1F4)->YY26).tag == 34){_LL1: _tmp1F5=(_tmp1F4->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1F5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1166
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY26).tag=34U,(_tmp707.YY26).val=yy1;_tmp707;});}static char _tmp1F9[18U]="declarator_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F9,_tmp1F9,_tmp1F9 + 18U};
union Cyc_YYSTYPE*_tmp1F7=yy1;struct Cyc_Parse_Declarator _tmp1F8;if((((union Cyc_YYSTYPE*)_tmp1F7)->YY27).tag == 35){_LL1: _tmp1F8=(_tmp1F7->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1F8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1167
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY27).tag=35U,(_tmp708.YY27).val=yy1;_tmp708;});}static char _tmp1FC[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1161 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FC,_tmp1FC,_tmp1FC + 45U};
union Cyc_YYSTYPE*_tmp1FA=yy1;struct _tuple25*_tmp1FB;if((((union Cyc_YYSTYPE*)_tmp1FA)->YY28).tag == 36){_LL1: _tmp1FB=(_tmp1FA->YY28).val;_LL2: {struct _tuple25*yy=_tmp1FB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1168
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY28).tag=36U,(_tmp709.YY28).val=yy1;_tmp709;});}static char _tmp1FF[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1162 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FF,_tmp1FF,_tmp1FF + 57U};
union Cyc_YYSTYPE*_tmp1FD=yy1;struct Cyc_List_List*_tmp1FE;if((((union Cyc_YYSTYPE*)_tmp1FD)->YY29).tag == 37){_LL1: _tmp1FE=(_tmp1FD->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1FE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1169
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY29).tag=37U,(_tmp70A.YY29).val=yy1;_tmp70A;});}static char _tmp202[26U]="abstractdeclarator_t<`yy>";
# 1163 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp202,_tmp202,_tmp202 + 26U};
union Cyc_YYSTYPE*_tmp200=yy1;struct Cyc_Parse_Abstractdeclarator _tmp201;if((((union Cyc_YYSTYPE*)_tmp200)->YY30).tag == 38){_LL1: _tmp201=(_tmp200->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp201;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1170
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY30).tag=38U,(_tmp70B.YY30).val=yy1;_tmp70B;});}static char _tmp205[5U]="bool";
# 1164 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp205,_tmp205,_tmp205 + 5U};
union Cyc_YYSTYPE*_tmp203=yy1;int _tmp204;if((((union Cyc_YYSTYPE*)_tmp203)->YY31).tag == 39){_LL1: _tmp204=(_tmp203->YY31).val;_LL2: {int yy=_tmp204;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1171
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY31).tag=39U,(_tmp70C.YY31).val=yy1;_tmp70C;});}static char _tmp208[8U]="scope_t";
# 1165 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp208,_tmp208,_tmp208 + 8U};
union Cyc_YYSTYPE*_tmp206=yy1;enum Cyc_Absyn_Scope _tmp207;if((((union Cyc_YYSTYPE*)_tmp206)->YY32).tag == 40){_LL1: _tmp207=(_tmp206->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp207;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1172
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY32).tag=40U,(_tmp70D.YY32).val=yy1;_tmp70D;});}static char _tmp20B[16U]="datatypefield_t";
# 1166 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20B,_tmp20B,_tmp20B + 16U};
union Cyc_YYSTYPE*_tmp209=yy1;struct Cyc_Absyn_Datatypefield*_tmp20A;if((((union Cyc_YYSTYPE*)_tmp209)->YY33).tag == 41){_LL1: _tmp20A=(_tmp209->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp20A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1173
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY33).tag=41U,(_tmp70E.YY33).val=yy1;_tmp70E;});}static char _tmp20E[27U]="list_t<datatypefield_t,`H>";
# 1167 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20E,_tmp20E,_tmp20E + 27U};
union Cyc_YYSTYPE*_tmp20C=yy1;struct Cyc_List_List*_tmp20D;if((((union Cyc_YYSTYPE*)_tmp20C)->YY34).tag == 42){_LL1: _tmp20D=(_tmp20C->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp20D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1174
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY34).tag=42U,(_tmp70F.YY34).val=yy1;_tmp70F;});}static char _tmp211[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1168 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp211,_tmp211,_tmp211 + 41U};
union Cyc_YYSTYPE*_tmp20F=yy1;struct _tuple26 _tmp210;if((((union Cyc_YYSTYPE*)_tmp20F)->YY35).tag == 43){_LL1: _tmp210=(_tmp20F->YY35).val;_LL2: {struct _tuple26 yy=_tmp210;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1175
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY35).tag=43U,(_tmp710.YY35).val=yy1;_tmp710;});}static char _tmp214[17U]="list_t<var_t,`H>";
# 1169 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp214,_tmp214,_tmp214 + 17U};
union Cyc_YYSTYPE*_tmp212=yy1;struct Cyc_List_List*_tmp213;if((((union Cyc_YYSTYPE*)_tmp212)->YY36).tag == 44){_LL1: _tmp213=(_tmp212->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp213;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1176
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY36).tag=44U,(_tmp711.YY36).val=yy1;_tmp711;});}static char _tmp217[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1170 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp217,_tmp217,_tmp217 + 31U};
union Cyc_YYSTYPE*_tmp215=yy1;struct _tuple8*_tmp216;if((((union Cyc_YYSTYPE*)_tmp215)->YY37).tag == 45){_LL1: _tmp216=(_tmp215->YY37).val;_LL2: {struct _tuple8*yy=_tmp216;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1177
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY37).tag=45U,(_tmp712.YY37).val=yy1;_tmp712;});}static char _tmp21A[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1171 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21A,_tmp21A,_tmp21A + 42U};
union Cyc_YYSTYPE*_tmp218=yy1;struct Cyc_List_List*_tmp219;if((((union Cyc_YYSTYPE*)_tmp218)->YY38).tag == 46){_LL1: _tmp219=(_tmp218->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp219;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1178
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY38).tag=46U,(_tmp713.YY38).val=yy1;_tmp713;});}static char _tmp21D[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1172 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21D,_tmp21D,_tmp21D + 115U};
union Cyc_YYSTYPE*_tmp21B=yy1;struct _tuple27*_tmp21C;if((((union Cyc_YYSTYPE*)_tmp21B)->YY39).tag == 47){_LL1: _tmp21C=(_tmp21B->YY39).val;_LL2: {struct _tuple27*yy=_tmp21C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1179
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY39).tag=47U,(_tmp714.YY39).val=yy1;_tmp714;});}static char _tmp220[8U]="types_t";
# 1173 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp220,_tmp220,_tmp220 + 8U};
union Cyc_YYSTYPE*_tmp21E=yy1;struct Cyc_List_List*_tmp21F;if((((union Cyc_YYSTYPE*)_tmp21E)->YY40).tag == 48){_LL1: _tmp21F=(_tmp21E->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp21F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1180
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY40).tag=48U,(_tmp715.YY40).val=yy1;_tmp715;});}static char _tmp223[24U]="list_t<designator_t,`H>";
# 1175 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp223,_tmp223,_tmp223 + 24U};
union Cyc_YYSTYPE*_tmp221=yy1;struct Cyc_List_List*_tmp222;if((((union Cyc_YYSTYPE*)_tmp221)->YY41).tag == 49){_LL1: _tmp222=(_tmp221->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp222;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1182
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY41).tag=49U,(_tmp716.YY41).val=yy1;_tmp716;});}static char _tmp226[13U]="designator_t";
# 1176 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp226,_tmp226,_tmp226 + 13U};
union Cyc_YYSTYPE*_tmp224=yy1;void*_tmp225;if((((union Cyc_YYSTYPE*)_tmp224)->YY42).tag == 50){_LL1: _tmp225=(_tmp224->YY42).val;_LL2: {void*yy=_tmp225;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1183
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY42).tag=50U,(_tmp717.YY42).val=yy1;_tmp717;});}static char _tmp229[7U]="kind_t";
# 1177 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp229,_tmp229,_tmp229 + 7U};
union Cyc_YYSTYPE*_tmp227=yy1;struct Cyc_Absyn_Kind*_tmp228;if((((union Cyc_YYSTYPE*)_tmp227)->YY43).tag == 51){_LL1: _tmp228=(_tmp227->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp228;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1184
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY43).tag=51U,(_tmp718.YY43).val=yy1;_tmp718;});}static char _tmp22C[7U]="type_t";
# 1178 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22C,_tmp22C,_tmp22C + 7U};
union Cyc_YYSTYPE*_tmp22A=yy1;void*_tmp22B;if((((union Cyc_YYSTYPE*)_tmp22A)->YY44).tag == 52){_LL1: _tmp22B=(_tmp22A->YY44).val;_LL2: {void*yy=_tmp22B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1185
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp719;(_tmp719.YY44).tag=52U,(_tmp719.YY44).val=yy1;_tmp719;});}static char _tmp22F[23U]="list_t<attribute_t,`H>";
# 1179 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22F,_tmp22F,_tmp22F + 23U};
union Cyc_YYSTYPE*_tmp22D=yy1;struct Cyc_List_List*_tmp22E;if((((union Cyc_YYSTYPE*)_tmp22D)->YY45).tag == 53){_LL1: _tmp22E=(_tmp22D->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp22E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1186
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71A;(_tmp71A.YY45).tag=53U,(_tmp71A.YY45).val=yy1;_tmp71A;});}static char _tmp232[12U]="attribute_t";
# 1180 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp232,_tmp232,_tmp232 + 12U};
union Cyc_YYSTYPE*_tmp230=yy1;void*_tmp231;if((((union Cyc_YYSTYPE*)_tmp230)->YY46).tag == 54){_LL1: _tmp231=(_tmp230->YY46).val;_LL2: {void*yy=_tmp231;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1187
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp71B;(_tmp71B.YY46).tag=54U,(_tmp71B.YY46).val=yy1;_tmp71B;});}static char _tmp235[12U]="enumfield_t";
# 1181 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp235,_tmp235,_tmp235 + 12U};
union Cyc_YYSTYPE*_tmp233=yy1;struct Cyc_Absyn_Enumfield*_tmp234;if((((union Cyc_YYSTYPE*)_tmp233)->YY47).tag == 55){_LL1: _tmp234=(_tmp233->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp234;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1188
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp71C;(_tmp71C.YY47).tag=55U,(_tmp71C.YY47).val=yy1;_tmp71C;});}static char _tmp238[23U]="list_t<enumfield_t,`H>";
# 1182 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp238,_tmp238,_tmp238 + 23U};
union Cyc_YYSTYPE*_tmp236=yy1;struct Cyc_List_List*_tmp237;if((((union Cyc_YYSTYPE*)_tmp236)->YY48).tag == 56){_LL1: _tmp237=(_tmp236->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp237;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1189
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71D;(_tmp71D.YY48).tag=56U,(_tmp71D.YY48).val=yy1;_tmp71D;});}static char _tmp23B[11U]="type_opt_t";
# 1183 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23B,_tmp23B,_tmp23B + 11U};
union Cyc_YYSTYPE*_tmp239=yy1;void*_tmp23A;if((((union Cyc_YYSTYPE*)_tmp239)->YY49).tag == 57){_LL1: _tmp23A=(_tmp239->YY49).val;_LL2: {void*yy=_tmp23A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1190
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp71E;(_tmp71E.YY49).tag=57U,(_tmp71E.YY49).val=yy1;_tmp71E;});}static char _tmp23E[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1184 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23E,_tmp23E,_tmp23E + 31U};
union Cyc_YYSTYPE*_tmp23C=yy1;struct Cyc_List_List*_tmp23D;if((((union Cyc_YYSTYPE*)_tmp23C)->YY50).tag == 58){_LL1: _tmp23D=(_tmp23C->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp23D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1191
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71F;(_tmp71F.YY50).tag=58U,(_tmp71F.YY50).val=yy1;_tmp71F;});}static char _tmp241[11U]="booltype_t";
# 1185 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp241,_tmp241,_tmp241 + 11U};
union Cyc_YYSTYPE*_tmp23F=yy1;void*_tmp240;if((((union Cyc_YYSTYPE*)_tmp23F)->YY51).tag == 59){_LL1: _tmp240=(_tmp23F->YY51).val;_LL2: {void*yy=_tmp240;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1192
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp720;(_tmp720.YY51).tag=59U,(_tmp720.YY51).val=yy1;_tmp720;});}static char _tmp244[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1186 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp244,_tmp244,_tmp244 + 45U};
union Cyc_YYSTYPE*_tmp242=yy1;struct Cyc_List_List*_tmp243;if((((union Cyc_YYSTYPE*)_tmp242)->YY52).tag == 60){_LL1: _tmp243=(_tmp242->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp243;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1193
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp721;(_tmp721.YY52).tag=60U,(_tmp721.YY52).val=yy1;_tmp721;});}static char _tmp247[58U]="$(list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1187 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp247,_tmp247,_tmp247 + 58U};
union Cyc_YYSTYPE*_tmp245=yy1;struct _tuple28*_tmp246;if((((union Cyc_YYSTYPE*)_tmp245)->YY53).tag == 61){_LL1: _tmp246=(_tmp245->YY53).val;_LL2: {struct _tuple28*yy=_tmp246;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1194
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp722;(_tmp722.YY53).tag=61U,(_tmp722.YY53).val=yy1;_tmp722;});}static char _tmp24A[18U]="list_t<qvar_t,`H>";
# 1188 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24A,_tmp24A,_tmp24A + 18U};
union Cyc_YYSTYPE*_tmp248=yy1;struct Cyc_List_List*_tmp249;if((((union Cyc_YYSTYPE*)_tmp248)->YY54).tag == 62){_LL1: _tmp249=(_tmp248->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp249;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1195
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp723;(_tmp723.YY54).tag=62U,(_tmp723.YY54).val=yy1;_tmp723;});}static char _tmp24D[20U]="pointer_qual_t<`yy>";
# 1189 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24D,_tmp24D,_tmp24D + 20U};
union Cyc_YYSTYPE*_tmp24B=yy1;void*_tmp24C;if((((union Cyc_YYSTYPE*)_tmp24B)->YY55).tag == 63){_LL1: _tmp24C=(_tmp24B->YY55).val;_LL2: {void*yy=_tmp24C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1196
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp724;(_tmp724.YY55).tag=63U,(_tmp724.YY55).val=yy1;_tmp724;});}static char _tmp250[21U]="pointer_quals_t<`yy>";
# 1190 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp250,_tmp250,_tmp250 + 21U};
union Cyc_YYSTYPE*_tmp24E=yy1;struct Cyc_List_List*_tmp24F;if((((union Cyc_YYSTYPE*)_tmp24E)->YY56).tag == 64){_LL1: _tmp24F=(_tmp24E->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp24F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1197
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp725;(_tmp725.YY56).tag=64U,(_tmp725.YY56).val=yy1;_tmp725;});}static char _tmp253[10U]="exp_opt_t";
# 1191 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp253,_tmp253,_tmp253 + 10U};
union Cyc_YYSTYPE*_tmp251=yy1;struct Cyc_Absyn_Exp*_tmp252;if((((union Cyc_YYSTYPE*)_tmp251)->YY57).tag == 65){_LL1: _tmp252=(_tmp251->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp252;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1198
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp726;(_tmp726.YY57).tag=65U,(_tmp726.YY57).val=yy1;_tmp726;});}static char _tmp256[10U]="raw_exp_t";
# 1192 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp256,_tmp256,_tmp256 + 10U};
union Cyc_YYSTYPE*_tmp254=yy1;void*_tmp255;if((((union Cyc_YYSTYPE*)_tmp254)->YY58).tag == 66){_LL1: _tmp255=(_tmp254->YY58).val;_LL2: {void*yy=_tmp255;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1199
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp727;(_tmp727.YY58).tag=66U,(_tmp727.YY58).val=yy1;_tmp727;});}static char _tmp259[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1194 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp259,_tmp259,_tmp259 + 112U};
union Cyc_YYSTYPE*_tmp257=yy1;struct _tuple29*_tmp258;if((((union Cyc_YYSTYPE*)_tmp257)->YY59).tag == 67){_LL1: _tmp258=(_tmp257->YY59).val;_LL2: {struct _tuple29*yy=_tmp258;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1201
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp728;(_tmp728.YY59).tag=67U,(_tmp728.YY59).val=yy1;_tmp728;});}static char _tmp25C[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1195 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp25C,_tmp25C,_tmp25C + 73U};
union Cyc_YYSTYPE*_tmp25A=yy1;struct _tuple30*_tmp25B;if((((union Cyc_YYSTYPE*)_tmp25A)->YY60).tag == 68){_LL1: _tmp25B=(_tmp25A->YY60).val;_LL2: {struct _tuple30*yy=_tmp25B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1202
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp729;(_tmp729.YY60).tag=68U,(_tmp729.YY60).val=yy1;_tmp729;});}static char _tmp25F[28U]="list_t<string_t<`H>@`H, `H>";
# 1196 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp25F,_tmp25F,_tmp25F + 28U};
union Cyc_YYSTYPE*_tmp25D=yy1;struct Cyc_List_List*_tmp25E;if((((union Cyc_YYSTYPE*)_tmp25D)->YY61).tag == 69){_LL1: _tmp25E=(_tmp25D->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp25E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1203
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp72A;(_tmp72A.YY61).tag=69U,(_tmp72A.YY61).val=yy1;_tmp72A;});}static char _tmp262[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1197 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp262,_tmp262,_tmp262 + 38U};
union Cyc_YYSTYPE*_tmp260=yy1;struct Cyc_List_List*_tmp261;if((((union Cyc_YYSTYPE*)_tmp260)->YY62).tag == 70){_LL1: _tmp261=(_tmp260->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp261;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1204
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp72B;(_tmp72B.YY62).tag=70U,(_tmp72B.YY62).val=yy1;_tmp72B;});}static char _tmp265[26U]="$(string_t<`H>, exp_t)@`H";
# 1198 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp265,_tmp265,_tmp265 + 26U};
union Cyc_YYSTYPE*_tmp263=yy1;struct _tuple31*_tmp264;if((((union Cyc_YYSTYPE*)_tmp263)->YY63).tag == 71){_LL1: _tmp264=(_tmp263->YY63).val;_LL2: {struct _tuple31*yy=_tmp264;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1205
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp72C;(_tmp72C.YY63).tag=71U,(_tmp72C.YY63).val=yy1;_tmp72C;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1221
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp72D;_tmp72D.timestamp=0,_tmp72D.first_line=0,_tmp72D.first_column=0,_tmp72D.last_line=0,_tmp72D.last_column=0;_tmp72D;});}
# 1224
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1235 "parse.y"
static short Cyc_yytranslate[380U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,151,2,2,136,149,146,2,133,134,129,143,128,147,138,148,2,2,2,2,2,2,2,2,2,2,137,125,131,130,132,142,141,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,139,2,140,145,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,126,144,127,150,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124};static char _tmp266[2U]="$";static char _tmp267[6U]="error";static char _tmp268[12U]="$undefined.";static char _tmp269[5U]="AUTO";static char _tmp26A[9U]="REGISTER";static char _tmp26B[7U]="STATIC";static char _tmp26C[7U]="EXTERN";static char _tmp26D[8U]="TYPEDEF";static char _tmp26E[5U]="VOID";static char _tmp26F[5U]="CHAR";static char _tmp270[6U]="SHORT";static char _tmp271[4U]="INT";static char _tmp272[5U]="LONG";static char _tmp273[6U]="FLOAT";static char _tmp274[7U]="DOUBLE";static char _tmp275[7U]="SIGNED";static char _tmp276[9U]="UNSIGNED";static char _tmp277[6U]="CONST";static char _tmp278[9U]="VOLATILE";static char _tmp279[9U]="RESTRICT";static char _tmp27A[7U]="STRUCT";static char _tmp27B[6U]="UNION";static char _tmp27C[5U]="CASE";static char _tmp27D[8U]="DEFAULT";static char _tmp27E[7U]="INLINE";static char _tmp27F[7U]="SIZEOF";static char _tmp280[9U]="OFFSETOF";static char _tmp281[3U]="IF";static char _tmp282[5U]="ELSE";static char _tmp283[7U]="SWITCH";static char _tmp284[6U]="WHILE";static char _tmp285[3U]="DO";static char _tmp286[4U]="FOR";static char _tmp287[5U]="GOTO";static char _tmp288[9U]="CONTINUE";static char _tmp289[6U]="BREAK";static char _tmp28A[7U]="RETURN";static char _tmp28B[5U]="ENUM";static char _tmp28C[7U]="TYPEOF";static char _tmp28D[16U]="BUILTIN_VA_LIST";static char _tmp28E[10U]="EXTENSION";static char _tmp28F[8U]="NULL_kw";static char _tmp290[4U]="LET";static char _tmp291[6U]="THROW";static char _tmp292[4U]="TRY";static char _tmp293[6U]="CATCH";static char _tmp294[7U]="EXPORT";static char _tmp295[9U]="OVERRIDE";static char _tmp296[5U]="HIDE";static char _tmp297[4U]="NEW";static char _tmp298[9U]="ABSTRACT";static char _tmp299[9U]="FALLTHRU";static char _tmp29A[6U]="USING";static char _tmp29B[10U]="NAMESPACE";static char _tmp29C[9U]="DATATYPE";static char _tmp29D[7U]="MALLOC";static char _tmp29E[8U]="RMALLOC";static char _tmp29F[15U]="RMALLOC_INLINE";static char _tmp2A0[7U]="CALLOC";static char _tmp2A1[8U]="RCALLOC";static char _tmp2A2[5U]="SWAP";static char _tmp2A3[9U]="REGION_T";static char _tmp2A4[6U]="TAG_T";static char _tmp2A5[7U]="REGION";static char _tmp2A6[5U]="RNEW";static char _tmp2A7[8U]="REGIONS";static char _tmp2A8[7U]="PORTON";static char _tmp2A9[8U]="PORTOFF";static char _tmp2AA[7U]="PRAGMA";static char _tmp2AB[10U]="TEMPESTON";static char _tmp2AC[11U]="TEMPESTOFF";static char _tmp2AD[8U]="NUMELTS";static char _tmp2AE[8U]="VALUEOF";static char _tmp2AF[10U]="VALUEOF_T";static char _tmp2B0[9U]="TAGCHECK";static char _tmp2B1[13U]="NUMELTS_QUAL";static char _tmp2B2[10U]="THIN_QUAL";static char _tmp2B3[9U]="FAT_QUAL";static char _tmp2B4[13U]="NOTNULL_QUAL";static char _tmp2B5[14U]="NULLABLE_QUAL";static char _tmp2B6[14U]="REQUIRES_QUAL";static char _tmp2B7[13U]="ENSURES_QUAL";static char _tmp2B8[12U]="REGION_QUAL";static char _tmp2B9[16U]="NOZEROTERM_QUAL";static char _tmp2BA[14U]="ZEROTERM_QUAL";static char _tmp2BB[12U]="TAGGED_QUAL";static char _tmp2BC[12U]="ASSERT_QUAL";static char _tmp2BD[16U]="EXTENSIBLE_QUAL";static char _tmp2BE[7U]="PTR_OP";static char _tmp2BF[7U]="INC_OP";static char _tmp2C0[7U]="DEC_OP";static char _tmp2C1[8U]="LEFT_OP";static char _tmp2C2[9U]="RIGHT_OP";static char _tmp2C3[6U]="LE_OP";static char _tmp2C4[6U]="GE_OP";static char _tmp2C5[6U]="EQ_OP";static char _tmp2C6[6U]="NE_OP";static char _tmp2C7[7U]="AND_OP";static char _tmp2C8[6U]="OR_OP";static char _tmp2C9[11U]="MUL_ASSIGN";static char _tmp2CA[11U]="DIV_ASSIGN";static char _tmp2CB[11U]="MOD_ASSIGN";static char _tmp2CC[11U]="ADD_ASSIGN";static char _tmp2CD[11U]="SUB_ASSIGN";static char _tmp2CE[12U]="LEFT_ASSIGN";static char _tmp2CF[13U]="RIGHT_ASSIGN";static char _tmp2D0[11U]="AND_ASSIGN";static char _tmp2D1[11U]="XOR_ASSIGN";static char _tmp2D2[10U]="OR_ASSIGN";static char _tmp2D3[9U]="ELLIPSIS";static char _tmp2D4[11U]="LEFT_RIGHT";static char _tmp2D5[12U]="COLON_COLON";static char _tmp2D6[11U]="IDENTIFIER";static char _tmp2D7[17U]="INTEGER_CONSTANT";static char _tmp2D8[7U]="STRING";static char _tmp2D9[8U]="WSTRING";static char _tmp2DA[19U]="CHARACTER_CONSTANT";static char _tmp2DB[20U]="WCHARACTER_CONSTANT";static char _tmp2DC[18U]="FLOATING_CONSTANT";static char _tmp2DD[9U]="TYPE_VAR";static char _tmp2DE[13U]="TYPEDEF_NAME";static char _tmp2DF[16U]="QUAL_IDENTIFIER";static char _tmp2E0[18U]="QUAL_TYPEDEF_NAME";static char _tmp2E1[10U]="ATTRIBUTE";static char _tmp2E2[8U]="ASM_TOK";static char _tmp2E3[4U]="';'";static char _tmp2E4[4U]="'{'";static char _tmp2E5[4U]="'}'";static char _tmp2E6[4U]="','";static char _tmp2E7[4U]="'*'";static char _tmp2E8[4U]="'='";static char _tmp2E9[4U]="'<'";static char _tmp2EA[4U]="'>'";static char _tmp2EB[4U]="'('";static char _tmp2EC[4U]="')'";static char _tmp2ED[4U]="'_'";static char _tmp2EE[4U]="'$'";static char _tmp2EF[4U]="':'";static char _tmp2F0[4U]="'.'";static char _tmp2F1[4U]="'['";static char _tmp2F2[4U]="']'";static char _tmp2F3[4U]="'@'";static char _tmp2F4[4U]="'?'";static char _tmp2F5[4U]="'+'";static char _tmp2F6[4U]="'|'";static char _tmp2F7[4U]="'^'";static char _tmp2F8[4U]="'&'";static char _tmp2F9[4U]="'-'";static char _tmp2FA[4U]="'/'";static char _tmp2FB[4U]="'%'";static char _tmp2FC[4U]="'~'";static char _tmp2FD[4U]="'!'";static char _tmp2FE[5U]="prog";static char _tmp2FF[17U]="translation_unit";static char _tmp300[18U]="tempest_on_action";static char _tmp301[19U]="tempest_off_action";static char _tmp302[16U]="extern_c_action";static char _tmp303[13U]="end_extern_c";static char _tmp304[14U]="hide_list_opt";static char _tmp305[17U]="hide_list_values";static char _tmp306[16U]="export_list_opt";static char _tmp307[12U]="export_list";static char _tmp308[19U]="export_list_values";static char _tmp309[13U]="override_opt";static char _tmp30A[21U]="external_declaration";static char _tmp30B[15U]="optional_comma";static char _tmp30C[20U]="function_definition";static char _tmp30D[21U]="function_definition2";static char _tmp30E[13U]="using_action";static char _tmp30F[15U]="unusing_action";static char _tmp310[17U]="namespace_action";static char _tmp311[19U]="unnamespace_action";static char _tmp312[12U]="declaration";static char _tmp313[17U]="declaration_list";static char _tmp314[23U]="declaration_specifiers";static char _tmp315[24U]="storage_class_specifier";static char _tmp316[15U]="attributes_opt";static char _tmp317[11U]="attributes";static char _tmp318[15U]="attribute_list";static char _tmp319[10U]="attribute";static char _tmp31A[15U]="type_specifier";static char _tmp31B[25U]="type_specifier_notypedef";static char _tmp31C[5U]="kind";static char _tmp31D[15U]="type_qualifier";static char _tmp31E[15U]="enum_specifier";static char _tmp31F[11U]="enum_field";static char _tmp320[22U]="enum_declaration_list";static char _tmp321[26U]="struct_or_union_specifier";static char _tmp322[16U]="type_params_opt";static char _tmp323[16U]="struct_or_union";static char _tmp324[24U]="struct_declaration_list";static char _tmp325[25U]="struct_declaration_list0";static char _tmp326[21U]="init_declarator_list";static char _tmp327[22U]="init_declarator_list0";static char _tmp328[16U]="init_declarator";static char _tmp329[19U]="struct_declaration";static char _tmp32A[25U]="specifier_qualifier_list";static char _tmp32B[35U]="notypedef_specifier_qualifier_list";static char _tmp32C[23U]="struct_declarator_list";static char _tmp32D[24U]="struct_declarator_list0";static char _tmp32E[18U]="struct_declarator";static char _tmp32F[20U]="requires_clause_opt";static char _tmp330[19U]="ensures_clause_opt";static char _tmp331[19U]="datatype_specifier";static char _tmp332[14U]="qual_datatype";static char _tmp333[19U]="datatypefield_list";static char _tmp334[20U]="datatypefield_scope";static char _tmp335[14U]="datatypefield";static char _tmp336[11U]="declarator";static char _tmp337[23U]="declarator_withtypedef";static char _tmp338[18U]="direct_declarator";static char _tmp339[30U]="direct_declarator_withtypedef";static char _tmp33A[8U]="pointer";static char _tmp33B[12U]="one_pointer";static char _tmp33C[14U]="pointer_quals";static char _tmp33D[13U]="pointer_qual";static char _tmp33E[23U]="pointer_null_and_bound";static char _tmp33F[14U]="pointer_bound";static char _tmp340[18U]="zeroterm_qual_opt";static char _tmp341[8U]="rgn_opt";static char _tmp342[11U]="tqual_list";static char _tmp343[20U]="parameter_type_list";static char _tmp344[9U]="type_var";static char _tmp345[16U]="optional_effect";static char _tmp346[19U]="optional_rgn_order";static char _tmp347[10U]="rgn_order";static char _tmp348[16U]="optional_inject";static char _tmp349[11U]="effect_set";static char _tmp34A[14U]="atomic_effect";static char _tmp34B[11U]="region_set";static char _tmp34C[15U]="parameter_list";static char _tmp34D[22U]="parameter_declaration";static char _tmp34E[16U]="identifier_list";static char _tmp34F[17U]="identifier_list0";static char _tmp350[12U]="initializer";static char _tmp351[18U]="array_initializer";static char _tmp352[17U]="initializer_list";static char _tmp353[12U]="designation";static char _tmp354[16U]="designator_list";static char _tmp355[11U]="designator";static char _tmp356[10U]="type_name";static char _tmp357[14U]="any_type_name";static char _tmp358[15U]="type_name_list";static char _tmp359[20U]="abstract_declarator";static char _tmp35A[27U]="direct_abstract_declarator";static char _tmp35B[10U]="statement";static char _tmp35C[18U]="labeled_statement";static char _tmp35D[21U]="expression_statement";static char _tmp35E[19U]="compound_statement";static char _tmp35F[16U]="block_item_list";static char _tmp360[20U]="selection_statement";static char _tmp361[15U]="switch_clauses";static char _tmp362[20U]="iteration_statement";static char _tmp363[15U]="jump_statement";static char _tmp364[12U]="exp_pattern";static char _tmp365[20U]="conditional_pattern";static char _tmp366[19U]="logical_or_pattern";static char _tmp367[20U]="logical_and_pattern";static char _tmp368[21U]="inclusive_or_pattern";static char _tmp369[21U]="exclusive_or_pattern";static char _tmp36A[12U]="and_pattern";static char _tmp36B[17U]="equality_pattern";static char _tmp36C[19U]="relational_pattern";static char _tmp36D[14U]="shift_pattern";static char _tmp36E[17U]="additive_pattern";static char _tmp36F[23U]="multiplicative_pattern";static char _tmp370[13U]="cast_pattern";static char _tmp371[14U]="unary_pattern";static char _tmp372[16U]="postfix_pattern";static char _tmp373[16U]="primary_pattern";static char _tmp374[8U]="pattern";static char _tmp375[19U]="tuple_pattern_list";static char _tmp376[20U]="tuple_pattern_list0";static char _tmp377[14U]="field_pattern";static char _tmp378[19U]="field_pattern_list";static char _tmp379[20U]="field_pattern_list0";static char _tmp37A[11U]="expression";static char _tmp37B[22U]="assignment_expression";static char _tmp37C[20U]="assignment_operator";static char _tmp37D[23U]="conditional_expression";static char _tmp37E[20U]="constant_expression";static char _tmp37F[22U]="logical_or_expression";static char _tmp380[23U]="logical_and_expression";static char _tmp381[24U]="inclusive_or_expression";static char _tmp382[24U]="exclusive_or_expression";static char _tmp383[15U]="and_expression";static char _tmp384[20U]="equality_expression";static char _tmp385[22U]="relational_expression";static char _tmp386[17U]="shift_expression";static char _tmp387[20U]="additive_expression";static char _tmp388[26U]="multiplicative_expression";static char _tmp389[16U]="cast_expression";static char _tmp38A[17U]="unary_expression";static char _tmp38B[9U]="asm_expr";static char _tmp38C[13U]="volatile_opt";static char _tmp38D[12U]="asm_out_opt";static char _tmp38E[12U]="asm_outlist";static char _tmp38F[11U]="asm_in_opt";static char _tmp390[11U]="asm_inlist";static char _tmp391[11U]="asm_io_elt";static char _tmp392[16U]="asm_clobber_opt";static char _tmp393[17U]="asm_clobber_list";static char _tmp394[15U]="unary_operator";static char _tmp395[19U]="postfix_expression";static char _tmp396[17U]="field_expression";static char _tmp397[19U]="primary_expression";static char _tmp398[25U]="argument_expression_list";static char _tmp399[26U]="argument_expression_list0";static char _tmp39A[9U]="constant";static char _tmp39B[20U]="qual_opt_identifier";static char _tmp39C[17U]="qual_opt_typedef";static char _tmp39D[18U]="struct_union_name";static char _tmp39E[11U]="field_name";static char _tmp39F[12U]="right_angle";
# 1607 "parse.y"
static struct _fat_ptr Cyc_yytname[314U]={{_tmp266,_tmp266,_tmp266 + 2U},{_tmp267,_tmp267,_tmp267 + 6U},{_tmp268,_tmp268,_tmp268 + 12U},{_tmp269,_tmp269,_tmp269 + 5U},{_tmp26A,_tmp26A,_tmp26A + 9U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 7U},{_tmp26D,_tmp26D,_tmp26D + 8U},{_tmp26E,_tmp26E,_tmp26E + 5U},{_tmp26F,_tmp26F,_tmp26F + 5U},{_tmp270,_tmp270,_tmp270 + 6U},{_tmp271,_tmp271,_tmp271 + 4U},{_tmp272,_tmp272,_tmp272 + 5U},{_tmp273,_tmp273,_tmp273 + 6U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 6U},{_tmp278,_tmp278,_tmp278 + 9U},{_tmp279,_tmp279,_tmp279 + 9U},{_tmp27A,_tmp27A,_tmp27A + 7U},{_tmp27B,_tmp27B,_tmp27B + 6U},{_tmp27C,_tmp27C,_tmp27C + 5U},{_tmp27D,_tmp27D,_tmp27D + 8U},{_tmp27E,_tmp27E,_tmp27E + 7U},{_tmp27F,_tmp27F,_tmp27F + 7U},{_tmp280,_tmp280,_tmp280 + 9U},{_tmp281,_tmp281,_tmp281 + 3U},{_tmp282,_tmp282,_tmp282 + 5U},{_tmp283,_tmp283,_tmp283 + 7U},{_tmp284,_tmp284,_tmp284 + 6U},{_tmp285,_tmp285,_tmp285 + 3U},{_tmp286,_tmp286,_tmp286 + 4U},{_tmp287,_tmp287,_tmp287 + 5U},{_tmp288,_tmp288,_tmp288 + 9U},{_tmp289,_tmp289,_tmp289 + 6U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 5U},{_tmp28C,_tmp28C,_tmp28C + 7U},{_tmp28D,_tmp28D,_tmp28D + 16U},{_tmp28E,_tmp28E,_tmp28E + 10U},{_tmp28F,_tmp28F,_tmp28F + 8U},{_tmp290,_tmp290,_tmp290 + 4U},{_tmp291,_tmp291,_tmp291 + 6U},{_tmp292,_tmp292,_tmp292 + 4U},{_tmp293,_tmp293,_tmp293 + 6U},{_tmp294,_tmp294,_tmp294 + 7U},{_tmp295,_tmp295,_tmp295 + 9U},{_tmp296,_tmp296,_tmp296 + 5U},{_tmp297,_tmp297,_tmp297 + 4U},{_tmp298,_tmp298,_tmp298 + 9U},{_tmp299,_tmp299,_tmp299 + 9U},{_tmp29A,_tmp29A,_tmp29A + 6U},{_tmp29B,_tmp29B,_tmp29B + 10U},{_tmp29C,_tmp29C,_tmp29C + 9U},{_tmp29D,_tmp29D,_tmp29D + 7U},{_tmp29E,_tmp29E,_tmp29E + 8U},{_tmp29F,_tmp29F,_tmp29F + 15U},{_tmp2A0,_tmp2A0,_tmp2A0 + 7U},{_tmp2A1,_tmp2A1,_tmp2A1 + 8U},{_tmp2A2,_tmp2A2,_tmp2A2 + 5U},{_tmp2A3,_tmp2A3,_tmp2A3 + 9U},{_tmp2A4,_tmp2A4,_tmp2A4 + 6U},{_tmp2A5,_tmp2A5,_tmp2A5 + 7U},{_tmp2A6,_tmp2A6,_tmp2A6 + 5U},{_tmp2A7,_tmp2A7,_tmp2A7 + 8U},{_tmp2A8,_tmp2A8,_tmp2A8 + 7U},{_tmp2A9,_tmp2A9,_tmp2A9 + 8U},{_tmp2AA,_tmp2AA,_tmp2AA + 7U},{_tmp2AB,_tmp2AB,_tmp2AB + 10U},{_tmp2AC,_tmp2AC,_tmp2AC + 11U},{_tmp2AD,_tmp2AD,_tmp2AD + 8U},{_tmp2AE,_tmp2AE,_tmp2AE + 8U},{_tmp2AF,_tmp2AF,_tmp2AF + 10U},{_tmp2B0,_tmp2B0,_tmp2B0 + 9U},{_tmp2B1,_tmp2B1,_tmp2B1 + 13U},{_tmp2B2,_tmp2B2,_tmp2B2 + 10U},{_tmp2B3,_tmp2B3,_tmp2B3 + 9U},{_tmp2B4,_tmp2B4,_tmp2B4 + 13U},{_tmp2B5,_tmp2B5,_tmp2B5 + 14U},{_tmp2B6,_tmp2B6,_tmp2B6 + 14U},{_tmp2B7,_tmp2B7,_tmp2B7 + 13U},{_tmp2B8,_tmp2B8,_tmp2B8 + 12U},{_tmp2B9,_tmp2B9,_tmp2B9 + 16U},{_tmp2BA,_tmp2BA,_tmp2BA + 14U},{_tmp2BB,_tmp2BB,_tmp2BB + 12U},{_tmp2BC,_tmp2BC,_tmp2BC + 12U},{_tmp2BD,_tmp2BD,_tmp2BD + 16U},{_tmp2BE,_tmp2BE,_tmp2BE + 7U},{_tmp2BF,_tmp2BF,_tmp2BF + 7U},{_tmp2C0,_tmp2C0,_tmp2C0 + 7U},{_tmp2C1,_tmp2C1,_tmp2C1 + 8U},{_tmp2C2,_tmp2C2,_tmp2C2 + 9U},{_tmp2C3,_tmp2C3,_tmp2C3 + 6U},{_tmp2C4,_tmp2C4,_tmp2C4 + 6U},{_tmp2C5,_tmp2C5,_tmp2C5 + 6U},{_tmp2C6,_tmp2C6,_tmp2C6 + 6U},{_tmp2C7,_tmp2C7,_tmp2C7 + 7U},{_tmp2C8,_tmp2C8,_tmp2C8 + 6U},{_tmp2C9,_tmp2C9,_tmp2C9 + 11U},{_tmp2CA,_tmp2CA,_tmp2CA + 11U},{_tmp2CB,_tmp2CB,_tmp2CB + 11U},{_tmp2CC,_tmp2CC,_tmp2CC + 11U},{_tmp2CD,_tmp2CD,_tmp2CD + 11U},{_tmp2CE,_tmp2CE,_tmp2CE + 12U},{_tmp2CF,_tmp2CF,_tmp2CF + 13U},{_tmp2D0,_tmp2D0,_tmp2D0 + 11U},{_tmp2D1,_tmp2D1,_tmp2D1 + 11U},{_tmp2D2,_tmp2D2,_tmp2D2 + 10U},{_tmp2D3,_tmp2D3,_tmp2D3 + 9U},{_tmp2D4,_tmp2D4,_tmp2D4 + 11U},{_tmp2D5,_tmp2D5,_tmp2D5 + 12U},{_tmp2D6,_tmp2D6,_tmp2D6 + 11U},{_tmp2D7,_tmp2D7,_tmp2D7 + 17U},{_tmp2D8,_tmp2D8,_tmp2D8 + 7U},{_tmp2D9,_tmp2D9,_tmp2D9 + 8U},{_tmp2DA,_tmp2DA,_tmp2DA + 19U},{_tmp2DB,_tmp2DB,_tmp2DB + 20U},{_tmp2DC,_tmp2DC,_tmp2DC + 18U},{_tmp2DD,_tmp2DD,_tmp2DD + 9U},{_tmp2DE,_tmp2DE,_tmp2DE + 13U},{_tmp2DF,_tmp2DF,_tmp2DF + 16U},{_tmp2E0,_tmp2E0,_tmp2E0 + 18U},{_tmp2E1,_tmp2E1,_tmp2E1 + 10U},{_tmp2E2,_tmp2E2,_tmp2E2 + 8U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 4U},{_tmp2F4,_tmp2F4,_tmp2F4 + 4U},{_tmp2F5,_tmp2F5,_tmp2F5 + 4U},{_tmp2F6,_tmp2F6,_tmp2F6 + 4U},{_tmp2F7,_tmp2F7,_tmp2F7 + 4U},{_tmp2F8,_tmp2F8,_tmp2F8 + 4U},{_tmp2F9,_tmp2F9,_tmp2F9 + 4U},{_tmp2FA,_tmp2FA,_tmp2FA + 4U},{_tmp2FB,_tmp2FB,_tmp2FB + 4U},{_tmp2FC,_tmp2FC,_tmp2FC + 4U},{_tmp2FD,_tmp2FD,_tmp2FD + 4U},{_tmp2FE,_tmp2FE,_tmp2FE + 5U},{_tmp2FF,_tmp2FF,_tmp2FF + 17U},{_tmp300,_tmp300,_tmp300 + 18U},{_tmp301,_tmp301,_tmp301 + 19U},{_tmp302,_tmp302,_tmp302 + 16U},{_tmp303,_tmp303,_tmp303 + 13U},{_tmp304,_tmp304,_tmp304 + 14U},{_tmp305,_tmp305,_tmp305 + 17U},{_tmp306,_tmp306,_tmp306 + 16U},{_tmp307,_tmp307,_tmp307 + 12U},{_tmp308,_tmp308,_tmp308 + 19U},{_tmp309,_tmp309,_tmp309 + 13U},{_tmp30A,_tmp30A,_tmp30A + 21U},{_tmp30B,_tmp30B,_tmp30B + 15U},{_tmp30C,_tmp30C,_tmp30C + 20U},{_tmp30D,_tmp30D,_tmp30D + 21U},{_tmp30E,_tmp30E,_tmp30E + 13U},{_tmp30F,_tmp30F,_tmp30F + 15U},{_tmp310,_tmp310,_tmp310 + 17U},{_tmp311,_tmp311,_tmp311 + 19U},{_tmp312,_tmp312,_tmp312 + 12U},{_tmp313,_tmp313,_tmp313 + 17U},{_tmp314,_tmp314,_tmp314 + 23U},{_tmp315,_tmp315,_tmp315 + 24U},{_tmp316,_tmp316,_tmp316 + 15U},{_tmp317,_tmp317,_tmp317 + 11U},{_tmp318,_tmp318,_tmp318 + 15U},{_tmp319,_tmp319,_tmp319 + 10U},{_tmp31A,_tmp31A,_tmp31A + 15U},{_tmp31B,_tmp31B,_tmp31B + 25U},{_tmp31C,_tmp31C,_tmp31C + 5U},{_tmp31D,_tmp31D,_tmp31D + 15U},{_tmp31E,_tmp31E,_tmp31E + 15U},{_tmp31F,_tmp31F,_tmp31F + 11U},{_tmp320,_tmp320,_tmp320 + 22U},{_tmp321,_tmp321,_tmp321 + 26U},{_tmp322,_tmp322,_tmp322 + 16U},{_tmp323,_tmp323,_tmp323 + 16U},{_tmp324,_tmp324,_tmp324 + 24U},{_tmp325,_tmp325,_tmp325 + 25U},{_tmp326,_tmp326,_tmp326 + 21U},{_tmp327,_tmp327,_tmp327 + 22U},{_tmp328,_tmp328,_tmp328 + 16U},{_tmp329,_tmp329,_tmp329 + 19U},{_tmp32A,_tmp32A,_tmp32A + 25U},{_tmp32B,_tmp32B,_tmp32B + 35U},{_tmp32C,_tmp32C,_tmp32C + 23U},{_tmp32D,_tmp32D,_tmp32D + 24U},{_tmp32E,_tmp32E,_tmp32E + 18U},{_tmp32F,_tmp32F,_tmp32F + 20U},{_tmp330,_tmp330,_tmp330 + 19U},{_tmp331,_tmp331,_tmp331 + 19U},{_tmp332,_tmp332,_tmp332 + 14U},{_tmp333,_tmp333,_tmp333 + 19U},{_tmp334,_tmp334,_tmp334 + 20U},{_tmp335,_tmp335,_tmp335 + 14U},{_tmp336,_tmp336,_tmp336 + 11U},{_tmp337,_tmp337,_tmp337 + 23U},{_tmp338,_tmp338,_tmp338 + 18U},{_tmp339,_tmp339,_tmp339 + 30U},{_tmp33A,_tmp33A,_tmp33A + 8U},{_tmp33B,_tmp33B,_tmp33B + 12U},{_tmp33C,_tmp33C,_tmp33C + 14U},{_tmp33D,_tmp33D,_tmp33D + 13U},{_tmp33E,_tmp33E,_tmp33E + 23U},{_tmp33F,_tmp33F,_tmp33F + 14U},{_tmp340,_tmp340,_tmp340 + 18U},{_tmp341,_tmp341,_tmp341 + 8U},{_tmp342,_tmp342,_tmp342 + 11U},{_tmp343,_tmp343,_tmp343 + 20U},{_tmp344,_tmp344,_tmp344 + 9U},{_tmp345,_tmp345,_tmp345 + 16U},{_tmp346,_tmp346,_tmp346 + 19U},{_tmp347,_tmp347,_tmp347 + 10U},{_tmp348,_tmp348,_tmp348 + 16U},{_tmp349,_tmp349,_tmp349 + 11U},{_tmp34A,_tmp34A,_tmp34A + 14U},{_tmp34B,_tmp34B,_tmp34B + 11U},{_tmp34C,_tmp34C,_tmp34C + 15U},{_tmp34D,_tmp34D,_tmp34D + 22U},{_tmp34E,_tmp34E,_tmp34E + 16U},{_tmp34F,_tmp34F,_tmp34F + 17U},{_tmp350,_tmp350,_tmp350 + 12U},{_tmp351,_tmp351,_tmp351 + 18U},{_tmp352,_tmp352,_tmp352 + 17U},{_tmp353,_tmp353,_tmp353 + 12U},{_tmp354,_tmp354,_tmp354 + 16U},{_tmp355,_tmp355,_tmp355 + 11U},{_tmp356,_tmp356,_tmp356 + 10U},{_tmp357,_tmp357,_tmp357 + 14U},{_tmp358,_tmp358,_tmp358 + 15U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 27U},{_tmp35B,_tmp35B,_tmp35B + 10U},{_tmp35C,_tmp35C,_tmp35C + 18U},{_tmp35D,_tmp35D,_tmp35D + 21U},{_tmp35E,_tmp35E,_tmp35E + 19U},{_tmp35F,_tmp35F,_tmp35F + 16U},{_tmp360,_tmp360,_tmp360 + 20U},{_tmp361,_tmp361,_tmp361 + 15U},{_tmp362,_tmp362,_tmp362 + 20U},{_tmp363,_tmp363,_tmp363 + 15U},{_tmp364,_tmp364,_tmp364 + 12U},{_tmp365,_tmp365,_tmp365 + 20U},{_tmp366,_tmp366,_tmp366 + 19U},{_tmp367,_tmp367,_tmp367 + 20U},{_tmp368,_tmp368,_tmp368 + 21U},{_tmp369,_tmp369,_tmp369 + 21U},{_tmp36A,_tmp36A,_tmp36A + 12U},{_tmp36B,_tmp36B,_tmp36B + 17U},{_tmp36C,_tmp36C,_tmp36C + 19U},{_tmp36D,_tmp36D,_tmp36D + 14U},{_tmp36E,_tmp36E,_tmp36E + 17U},{_tmp36F,_tmp36F,_tmp36F + 23U},{_tmp370,_tmp370,_tmp370 + 13U},{_tmp371,_tmp371,_tmp371 + 14U},{_tmp372,_tmp372,_tmp372 + 16U},{_tmp373,_tmp373,_tmp373 + 16U},{_tmp374,_tmp374,_tmp374 + 8U},{_tmp375,_tmp375,_tmp375 + 19U},{_tmp376,_tmp376,_tmp376 + 20U},{_tmp377,_tmp377,_tmp377 + 14U},{_tmp378,_tmp378,_tmp378 + 19U},{_tmp379,_tmp379,_tmp379 + 20U},{_tmp37A,_tmp37A,_tmp37A + 11U},{_tmp37B,_tmp37B,_tmp37B + 22U},{_tmp37C,_tmp37C,_tmp37C + 20U},{_tmp37D,_tmp37D,_tmp37D + 23U},{_tmp37E,_tmp37E,_tmp37E + 20U},{_tmp37F,_tmp37F,_tmp37F + 22U},{_tmp380,_tmp380,_tmp380 + 23U},{_tmp381,_tmp381,_tmp381 + 24U},{_tmp382,_tmp382,_tmp382 + 24U},{_tmp383,_tmp383,_tmp383 + 15U},{_tmp384,_tmp384,_tmp384 + 20U},{_tmp385,_tmp385,_tmp385 + 22U},{_tmp386,_tmp386,_tmp386 + 17U},{_tmp387,_tmp387,_tmp387 + 20U},{_tmp388,_tmp388,_tmp388 + 26U},{_tmp389,_tmp389,_tmp389 + 16U},{_tmp38A,_tmp38A,_tmp38A + 17U},{_tmp38B,_tmp38B,_tmp38B + 9U},{_tmp38C,_tmp38C,_tmp38C + 13U},{_tmp38D,_tmp38D,_tmp38D + 12U},{_tmp38E,_tmp38E,_tmp38E + 12U},{_tmp38F,_tmp38F,_tmp38F + 11U},{_tmp390,_tmp390,_tmp390 + 11U},{_tmp391,_tmp391,_tmp391 + 11U},{_tmp392,_tmp392,_tmp392 + 16U},{_tmp393,_tmp393,_tmp393 + 17U},{_tmp394,_tmp394,_tmp394 + 15U},{_tmp395,_tmp395,_tmp395 + 19U},{_tmp396,_tmp396,_tmp396 + 17U},{_tmp397,_tmp397,_tmp397 + 19U},{_tmp398,_tmp398,_tmp398 + 25U},{_tmp399,_tmp399,_tmp399 + 26U},{_tmp39A,_tmp39A,_tmp39A + 9U},{_tmp39B,_tmp39B,_tmp39B + 20U},{_tmp39C,_tmp39C,_tmp39C + 17U},{_tmp39D,_tmp39D,_tmp39D + 18U},{_tmp39E,_tmp39E,_tmp39E + 11U},{_tmp39F,_tmp39F,_tmp39F + 12U}};
# 1665
static short Cyc_yyr1[562U]={0,152,153,153,153,153,153,153,153,153,153,153,153,154,155,156,157,158,158,159,159,159,160,160,161,161,161,162,162,162,163,163,164,164,164,165,165,166,166,166,166,167,167,168,169,170,171,172,172,172,172,172,172,172,173,173,174,174,174,174,174,174,174,174,174,174,174,175,175,175,175,175,175,175,176,176,177,178,178,179,179,179,179,180,180,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,182,183,183,183,184,184,184,185,185,186,186,186,187,187,187,187,187,188,188,189,189,190,190,191,191,192,193,193,194,194,195,196,196,196,196,196,196,197,197,197,197,197,197,198,199,199,200,200,200,200,201,201,202,202,203,203,203,204,204,205,205,205,205,206,206,206,207,207,208,208,209,209,210,210,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,211,212,212,213,214,214,215,215,215,215,215,215,215,215,216,216,216,217,217,218,218,218,219,219,219,220,220,221,221,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,228,228,229,229,230,230,231,231,231,232,233,233,234,234,235,235,235,235,235,236,236,236,236,237,237,238,238,239,239,240,240,241,241,241,241,241,242,242,243,243,243,244,244,244,244,244,244,244,244,244,244,244,245,245,245,245,245,245,246,247,247,248,248,249,249,249,249,249,249,249,249,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,252,252,252,252,252,252,252,252,253,253,253,253,253,253,253,253,254,255,255,256,256,257,257,258,258,259,259,260,260,261,261,261,262,262,262,262,262,263,263,263,264,264,264,265,265,265,265,266,266,267,267,267,267,267,267,268,269,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,271,271,271,272,272,273,273,274,274,274,275,275,276,276,277,277,277,278,278,278,278,278,278,278,278,278,278,278,279,279,279,279,279,279,279,280,281,281,282,282,283,283,284,284,285,285,286,286,286,287,287,287,287,287,288,288,288,289,289,289,290,290,290,290,291,291,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,293,294,294,295,295,295,296,296,297,297,297,298,298,299,300,300,300,301,301,302,302,302,303,303,303,303,303,303,303,303,303,303,303,304,304,304,304,305,305,305,305,305,305,305,305,305,305,305,306,307,307,308,308,308,308,308,309,309,310,310,311,311,312,312,313,313};
# 1725
static short Cyc_yyr2[562U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1785
static short Cyc_yydefact[1137U]={0,34,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,127,128,63,0,0,97,0,0,73,0,0,165,104,106,0,0,0,13,14,0,0,0,552,231,554,553,555,0,217,0,100,0,217,216,1,0,0,0,0,32,0,0,33,0,56,65,59,83,61,94,95,0,98,0,0,176,0,201,204,99,180,125,71,70,64,0,113,0,58,551,0,552,547,548,549,550,0,125,0,0,391,0,0,0,254,0,393,394,43,45,0,0,0,0,0,0,0,0,166,0,0,0,214,0,0,0,0,215,0,0,0,2,0,0,0,0,47,0,133,134,136,57,66,60,62,129,556,557,125,125,0,54,0,0,36,0,233,0,189,177,202,0,208,209,212,213,0,211,210,222,204,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,536,537,497,0,0,0,0,0,517,515,516,0,420,422,436,444,446,448,450,452,454,457,462,465,468,472,0,474,518,535,533,552,403,0,0,0,0,404,0,0,402,50,0,0,125,0,0,0,143,139,141,274,276,0,0,52,0,0,8,9,0,125,558,559,232,108,0,0,0,181,101,252,0,249,10,11,0,3,0,5,0,48,0,0,0,36,0,130,131,156,124,0,163,0,0,0,0,0,0,0,0,0,0,0,0,552,304,306,0,314,308,0,312,297,298,299,0,300,301,302,0,55,36,136,35,37,281,0,239,255,0,0,235,233,0,219,0,0,0,224,74,223,205,0,118,114,0,0,0,482,0,0,494,438,472,0,439,440,0,0,0,0,0,0,0,0,0,0,0,475,476,498,493,0,478,0,0,0,0,479,477,0,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,426,427,428,429,430,431,432,433,434,435,425,0,480,0,524,525,0,0,0,539,0,125,395,0,0,0,417,552,559,0,0,0,0,270,413,418,0,415,0,0,392,410,411,0,408,256,0,0,0,0,277,0,247,144,149,145,147,140,142,233,0,283,275,284,561,560,0,103,105,0,0,107,123,80,79,0,77,218,182,233,251,178,283,253,190,191,0,102,16,30,44,0,46,0,135,137,258,257,36,38,120,132,0,0,0,151,152,159,0,125,125,171,0,0,0,0,0,552,0,0,0,343,344,345,0,0,347,0,0,0,315,309,136,313,307,305,39,0,188,240,0,0,0,246,234,241,159,0,0,0,235,187,221,220,183,219,0,0,225,75,126,119,443,116,112,0,0,0,0,552,259,264,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,538,545,0,544,421,445,0,447,449,451,453,455,456,460,461,458,459,463,464,466,467,469,470,471,424,423,523,520,0,522,0,0,0,406,407,0,273,0,414,268,271,401,0,269,405,398,0,49,0,399,0,278,0,150,146,148,0,235,0,219,0,285,0,233,0,296,280,0,0,125,0,0,0,143,0,125,0,233,0,200,179,250,0,22,4,6,40,0,155,138,156,0,0,154,235,164,173,172,0,0,167,0,0,0,322,0,0,0,0,0,0,342,346,0,0,0,310,303,0,41,282,233,0,243,0,0,161,236,0,159,239,227,184,206,207,225,203,481,0,0,0,260,0,265,484,0,0,0,0,0,534,489,492,0,0,495,499,0,0,473,541,0,0,521,519,0,0,0,0,272,416,419,409,412,400,279,248,159,0,286,287,219,0,0,235,219,0,0,51,235,552,0,76,78,0,192,0,0,235,0,219,0,0,0,17,23,153,0,157,129,162,174,171,171,0,0,0,0,0,0,0,0,0,0,0,0,0,322,348,0,311,42,235,0,244,242,0,185,0,161,235,0,226,530,0,529,0,261,266,0,0,0,0,0,441,442,523,522,504,0,543,526,0,546,437,540,542,0,396,397,161,159,289,295,159,0,288,219,0,129,0,81,193,199,159,0,198,194,219,0,0,0,0,0,0,0,170,169,316,322,0,0,0,0,0,0,350,351,353,355,357,359,361,363,366,371,374,377,381,383,389,390,0,0,319,328,0,0,0,0,0,0,0,0,0,0,349,229,245,0,237,186,228,233,483,0,0,267,485,486,0,0,491,490,0,510,504,500,502,496,527,0,292,161,161,159,290,53,0,0,161,159,195,31,25,0,0,27,0,7,158,122,0,0,0,322,0,387,0,0,384,322,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,385,322,0,330,0,0,0,338,0,0,0,0,0,0,321,0,0,235,532,531,0,0,0,0,511,510,507,505,0,501,528,291,294,161,121,0,196,161,26,24,28,0,0,19,175,317,318,0,0,0,0,322,324,354,0,356,358,360,362,364,365,369,370,367,368,372,373,375,376,378,379,380,0,323,329,331,332,0,340,339,0,334,0,0,0,160,238,230,0,0,0,0,0,513,512,0,506,503,293,0,197,29,18,20,0,320,386,0,382,325,0,322,333,341,335,336,0,263,262,487,0,509,0,508,82,21,0,352,322,326,337,0,514,388,327,488,0,0,0};
# 1902
static short Cyc_yydefgoto[162U]={1134,53,54,55,56,487,878,1040,790,791,962,669,57,320,58,304,59,489,60,491,61,151,62,63,556,243,473,474,244,66,259,245,68,173,174,69,171,70,281,282,136,137,138,283,246,455,502,503,504,679,823,71,72,684,685,686,73,505,74,479,75,76,168,169,77,121,552,335,722,642,78,643,546,713,538,542,543,449,328,268,102,103,569,494,570,429,430,431,247,321,322,644,461,307,308,309,310,311,312,805,313,314,891,892,893,894,895,896,897,898,899,900,901,902,903,904,905,906,432,441,442,433,434,435,315,207,409,208,561,209,210,211,212,213,214,215,216,217,218,219,220,367,368,845,942,943,1023,944,1025,1092,221,222,830,223,588,589,224,225,80,963,436,465};
# 1922
static short Cyc_yypact[1137U]={3023,- -32768,- -32768,- -32768,- -32768,- 26,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3799,284,74,- -32768,3799,1492,- -32768,38,40,- -32768,110,136,58,157,185,- -32768,- -32768,203,100,264,- -32768,236,- -32768,- -32768,- -32768,227,263,1018,276,299,263,- -32768,- -32768,272,331,352,2880,- -32768,496,570,- -32768,1025,3799,3799,3799,- -32768,3799,- -32768,- -32768,476,- -32768,38,3709,71,199,250,750,- -32768,- -32768,392,407,429,- -32768,38,443,7242,- -32768,- -32768,2862,290,- -32768,- -32768,- -32768,- -32768,440,392,470,7242,- -32768,460,2862,459,484,504,- -32768,286,- -32768,- -32768,4015,4015,428,495,2880,2880,7242,424,- -32768,- 52,531,7242,- -32768,133,537,- 52,4495,- -32768,2880,2880,3165,- -32768,2880,3165,2880,3165,- -32768,553,554,- -32768,3575,- -32768,- -32768,- -32768,- -32768,4495,- -32768,- -32768,392,337,1844,- -32768,3709,1025,581,4015,3886,5338,- -32768,71,- -32768,580,- -32768,- -32768,- -32768,- -32768,596,- -32768,- -32768,159,750,4015,- -32768,- -32768,605,641,612,38,7553,633,7650,7242,7405,661,669,672,679,683,687,697,706,721,729,738,7650,7650,- -32768,- -32768,806,7701,2584,740,7701,7701,- -32768,- -32768,- -32768,360,- -32768,- -32768,45,761,716,732,741,280,132,691,138,72,- -32768,671,7701,91,- 39,- -32768,766,130,- -32768,2862,222,775,1133,786,393,1315,- -32768,- -32768,788,7242,392,1315,768,4234,4495,4582,4495,433,- -32768,12,12,- -32768,793,774,- -32768,- -32768,413,392,- -32768,- -32768,- -32768,- -32768,28,781,779,- -32768,- -32768,287,451,- -32768,- -32768,- -32768,787,- -32768,791,- -32768,792,- -32768,133,5450,3709,581,798,4495,- -32768,728,789,38,800,795,336,802,4651,803,819,808,820,5562,2440,4651,- 9,807,- -32768,- -32768,814,1993,1993,1025,1993,- -32768,- -32768,- -32768,822,- -32768,- -32768,- -32768,233,- -32768,581,818,- -32768,- -32768,810,81,838,- -32768,- 4,823,825,385,826,727,811,7242,4015,- -32768,831,- -32768,- -32768,81,38,- -32768,7242,829,2584,- -32768,4495,2584,- -32768,- -32768,- -32768,4763,- -32768,861,7242,7242,7242,7242,7242,7242,853,7242,4495,932,7242,- -32768,- -32768,- -32768,- -32768,834,- -32768,1993,841,453,7242,- -32768,- -32768,7242,- -32768,7701,7242,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7242,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7242,- -32768,- 52,- -32768,- -32768,5674,- 52,7242,- -32768,837,392,- -32768,847,848,851,- -32768,26,440,- 52,7242,2862,183,- -32768,- -32768,- -32768,857,859,852,2862,- -32768,- -32768,- -32768,854,865,- -32768,467,1133,864,4015,- -32768,858,871,- -32768,4582,4582,4582,- -32768,- -32768,3300,5786,478,- -32768,307,- -32768,- -32768,- 4,- -32768,- -32768,868,890,- -32768,879,- -32768,875,877,884,- -32768,- -32768,1458,- -32768,308,261,- -32768,- -32768,- -32768,4495,- -32768,- -32768,966,- -32768,2880,- -32768,2880,- -32768,- -32768,- -32768,- -32768,581,- -32768,- -32768,- -32768,849,7242,889,891,- -32768,19,232,392,392,817,7242,7242,888,898,7242,880,996,2291,902,- -32768,- -32768,- -32768,475,983,- -32768,5898,2142,2728,- -32768,- -32768,3575,- -32768,- -32768,- -32768,- -32768,4015,- -32768,- -32768,4495,897,4321,- -32768,- -32768,894,951,- 4,900,4408,825,- -32768,- -32768,- -32768,- -32768,727,904,208,636,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,905,912,909,920,914,- -32768,- -32768,709,5450,918,926,927,928,929,516,924,935,936,239,937,946,934,7502,- -32768,- -32768,938,968,- -32768,761,152,716,732,741,280,132,132,691,691,691,691,138,138,72,72,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,940,- -32768,68,4015,5223,4495,- -32768,4495,- -32768,957,- -32768,- -32768,- -32768,- -32768,1265,- -32768,- -32768,- -32768,1440,- -32768,939,- -32768,317,- -32768,4495,- -32768,- -32768,- -32768,964,825,965,727,960,307,4015,4105,6010,- -32768,- -32768,7242,979,392,7354,971,28,3435,972,392,4015,3886,6122,- -32768,308,- -32768,985,1061,- -32768,- -32768,- -32768,874,- -32768,- -32768,728,984,7242,- -32768,825,- -32768,- -32768,- -32768,991,38,508,545,546,7242,821,555,4651,986,6234,6346,566,- -32768,- -32768,994,987,988,1993,- -32768,3709,- -32768,810,999,4015,- -32768,998,- 4,1040,- -32768,995,951,102,- -32768,- -32768,- -32768,- -32768,636,- -32768,1000,204,1000,1001,- -32768,4878,- -32768,- -32768,7242,7242,1104,7242,7405,- -32768,- -32768,- -32768,- 52,- 52,- -32768,997,1006,4993,- -32768,- -32768,7242,7242,- -32768,- -32768,81,719,1021,1023,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,951,1011,- -32768,- -32768,727,81,1022,825,727,1024,564,- -32768,825,1034,1029,- -32768,- -32768,1031,- -32768,81,1035,825,1036,727,1028,3165,1045,1124,- -32768,- -32768,7242,- -32768,4495,- -32768,1042,88,817,4651,1047,1044,1207,1043,1055,4651,7242,6458,592,6570,639,6682,821,- -32768,1058,- -32768,- -32768,825,339,- -32768,- -32768,1051,- -32768,1068,1040,825,4495,- -32768,- -32768,342,- -32768,7242,- -32768,- -32768,5450,1056,1057,1059,1065,- -32768,861,1060,1064,4,1066,- -32768,- -32768,725,- -32768,- -32768,- -32768,- -32768,5223,- -32768,- -32768,1040,951,- -32768,- -32768,951,1069,- -32768,727,1070,4495,1076,- -32768,- -32768,- -32768,951,1073,- -32768,- -32768,727,1072,981,1078,2880,1074,1082,4495,- -32768,- -32768,1182,821,1085,7798,1080,2728,7701,1079,- -32768,50,- -32768,1118,1075,710,771,194,790,349,147,- -32768,- -32768,- -32768,- -32768,1121,7701,1993,- -32768,- -32768,588,4651,602,6794,4651,604,6906,7018,662,1093,- -32768,- -32768,- -32768,7242,1094,- -32768,- -32768,999,- -32768,323,362,- -32768,- -32768,- -32768,4495,1200,- -32768,- -32768,1095,120,365,- -32768,- -32768,- -32768,- -32768,5108,- -32768,1040,1040,951,- -32768,- -32768,1102,1106,1040,951,- -32768,- -32768,- -32768,1108,1109,664,424,- -32768,- -32768,- -32768,613,4651,1110,821,2584,- -32768,4495,1105,- -32768,1695,7701,7242,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7242,- -32768,821,1113,- -32768,4651,4651,615,- -32768,4651,4651,616,4651,617,7130,- -32768,1107,- 4,825,- -32768,- -32768,2728,1123,1111,7242,1126,370,- -32768,- -32768,1129,- -32768,- -32768,- -32768,- -32768,1040,- -32768,1134,- -32768,1040,- -32768,- -32768,- -32768,424,1125,668,- -32768,- -32768,- -32768,1128,1127,1135,7701,821,- -32768,761,381,716,732,732,280,132,132,691,691,691,691,138,138,72,72,- -32768,- -32768,- -32768,397,- -32768,- -32768,- -32768,- -32768,4651,- -32768,- -32768,4651,- -32768,4651,4651,625,- -32768,- -32768,- -32768,1137,757,1131,4495,628,- -32768,1139,1129,- -32768,- -32768,- -32768,1136,- -32768,- -32768,- -32768,- -32768,424,- -32768,1000,204,- -32768,- -32768,7242,1695,- -32768,- -32768,- -32768,- -32768,4651,- -32768,- -32768,- -32768,1140,- -32768,1159,- -32768,- -32768,- -32768,422,- -32768,821,- -32768,- -32768,1141,- -32768,- -32768,- -32768,- -32768,1276,1277,- -32768};
# 2039
static short Cyc_yypgoto[162U]={- -32768,119,- -32768,- -32768,- -32768,- -32768,- -32768,176,- -32768,- -32768,242,- -32768,- -32768,- 237,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 67,- 117,- 14,- -32768,- -32768,0,624,- -32768,235,- 182,1161,33,- -32768,- -32768,- 139,- -32768,30,1244,- 748,- -32768,- -32768,- -32768,1009,1007,659,355,- -32768,- -32768,614,- 211,- 231,- -32768,- -32768,92,- -32768,- -32768,121,- 154,1216,- 401,455,- -32768,1138,- -32768,- -32768,1241,- 452,- -32768,572,- 114,- 150,- 143,- 425,282,578,586,- 434,- 502,- 123,- 393,- 135,- -32768,- 270,- 168,- 576,- 247,- -32768,870,- 155,- 24,- 147,- 210,- 306,146,- -32768,- -32768,- 56,- 278,- -32768,- 423,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 23,1062,- -32768,675,860,- -32768,369,567,- -32768,- 170,- 298,- 165,- 363,- 361,- 353,930,- 351,- 345,- 279,- 321,- 317,- 197,695,- -32768,- -32768,- -32768,- -32768,367,- -32768,- 907,292,- -32768,507,954,216,- -32768,- 383,- -32768,- 12,529,- 35,- 61,- 72,- 54};
# 2063
static short Cyc_yytable[7950U]={64,369,267,104,374,375,150,83,493,147,348,87,327,351,105,591,352,153,336,593,329,64,279,338,410,64,528,529,594,531,652,614,596,67,1024,146,460,342,710,597,598,326,752,497,371,471,260,880,122,140,141,142,260,143,67,256,481,64,67,152,257,539,453,64,64,64,227,64,258,603,604,417,150,64,157,605,606,105,235,666,534,146,305,280,316,248,249,450,81,105,67,667,584,682,683,317,67,67,67,677,67,718,418,571,462,666,67,599,600,601,602,714,478,64,64,42,524,954,940,1095,19,20,540,717,525,152,231,64,64,64,623,64,64,64,64,306,762,152,228,64,472,941,701,378,463,707,67,67,978,64,41,64,108,647,667,464,678,229,157,44,67,67,67,- 558,67,67,67,67,848,123,111,560,67,462,647,541,130,285,287,411,412,413,67,139,67,548,1121,379,563,112,564,565,979,766,46,466,376,607,608,609,559,394,154,674,155,420,580,86,750,535,156,440,316,463,537,- 168,105,440,764,105,395,396,105,496,414,386,387,325,105,415,416,483,253,254,940,65,- 233,305,305,- 233,305,109,228,263,452,41,269,270,271,483,272,273,274,275,44,795,65,1022,560,672,65,229,47,388,389,152,48,110,536,445,453,453,453,318,51,52,994,454,42,64,376,392,113,87,558,393,470,986,987,748,306,306,65,306,334,995,996,64,65,65,65,729,65,305,64,64,802,64,65,555,114,41,67,625,541,858,257,829,117,862,44,427,428,660,258,988,989,739,412,413,67,560,48,712,873,557,115,67,67,612,67,421,720,615,41,861,660,118,65,65,865,464,43,44,45,622,306,422,533,871,119,376,65,65,65,500,65,65,65,65,64,571,414,41,65,384,385,740,416,47,794,43,44,45,65,450,65,124,745,120,921,51,52,923,477,541,41,127,318,41,458,928,228,67,267,44,624,43,44,45,84,952,239,704,630,- 255,47,105,- 255,240,477,229,958,636,816,105,458,530,51,52,46,46,125,105,929,257,1016,516,648,662,649,663,661,258,523,483,650,664,41,618,695,761,452,452,452,206,128,44,834,702,464,651,970,150,753,483,754,233,170,511,751,483,512,924,705,286,930,659,129,665,931,835,464,450,255,454,454,454,376,64,376,64,992,1026,377,879,993,571,1093,1018,673,768,941,152,825,780,770,1022,560,376,325,706,152,547,65,782,152,64,1108,780,784,376,67,170,67,376,64,438,785,79,64,159,65,- 15,1109,769,41,680,681,65,65,376,65,172,43,44,45,469,1045,783,67,85,856,250,1050,106,1131,107,251,67,931,541,47,67,818,933,457,840,372,176,841,230,458,826,51,52,1071,79,850,484,714,376,232,834,236,485,79,586,41,721,1085,79,633,234,927,376,43,44,45,145,698,148,144,376,79,65,835,444,670,759,671,457,237,175,252,1051,105,458,106,1053,105,131,132,560,661,948,1107,1013,1054,1055,106,999,1056,798,238,305,799,316,483,1057,1058,483,79,79,376,145,949,651,817,950,735,79,831,16,17,18,79,79,79,956,79,79,79,79,261,522,665,842,843,1063,1064,264,703,376,376,1065,1066,834,276,800,801,79,277,376,819,775,1127,262,306,806,152,812,376,976,376,133,134,851,864,1049,835,459,64,1132,64,175,319,1059,1060,1061,1062,998,372,332,859,372,376,915,1029,1030,376,480,1000,331,65,1034,65,577,869,333,376,397,376,339,975,67,1002,67,1006,506,1031,484,341,376,376,376,1035,1042,592,1075,1078,1080,65,376,721,560,376,106,968,1114,106,65,1119,106,918,65,345,376,340,106,398,399,400,401,402,403,404,405,406,407,907,1019,390,391,266,616,1015,1011,64,1038,376,105,1039,1101,353,482,1102,1067,1068,1069,1096,408,354,284,1098,355,79,639,640,641,550,551,356,482,266,508,357,1046,513,1047,358,67,682,683,366,160,161,162,163,164,359,1126,165,166,167,79,727,728,703,360,41,146,305,803,804,495,852,853,43,44,45,1106,946,947,361,982,983,47,380,1017,381,500,362,1086,541,501,984,985,175,51,52,363,344,373,347,349,349,382,64,687,688,990,991,691,1116,376,696,383,364,365,882,883,419,349,423,306,349,349,437,554,443,447,451,1041,456,467,468,875,475,64,305,67,459,476,486,507,349,495,488,490,572,573,574,575,576,498,509,579,510,146,582,518,480,519,1118,514,517,65,1125,65,587,284,67,590,526,520,884,527,278,532,537,553,911,464,46,506,562,544,106,378,549,41,545,306,610,578,106,583,617,43,44,45,46,88,106,585,611,64,47,619,620,587,500,621,627,637,41,628,631,629,51,52,347,632,43,44,45,965,635,638,188,653,654,47,146,655,482,500,656,482,67,657,658,668,675,51,52,692,79,676,79,689,773,65,690,646,693,697,699,482,708,677,726,831,715,482,349,711,719,723,724,1041,305,725,41,91,195,196,92,93,94,- 558,730,44,731,732,733,734,736,1001,742,743,1005,809,811,346,760,146,200,737,738,741,746,349,749,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,587,41,587,306,747,755,763,765,767,43,44,45,774,778,781,789,960,64,961,788,814,65,506,1043,266,793,796,807,813,822,815,349,325,820,744,824,506,838,41,506,832,854,844,855,266,41,495,44,846,46,67,266,65,857,44,47,1073,1074,135,48,1076,1077,47,1079,860,106,48,51,52,106,866,867,863,868,51,52,874,870,872,876,877,885,88,881,912,914,886,917,909,920,910,922,925,495,266,926,482,955,934,935,936,937,938,953,349,266,939,959,945,932,482,951,964,482,266,957,966,967,969,971,65,974,797,980,977,772,997,981,1012,1110,1014,777,1111,1020,1112,1113,1021,1032,89,787,887,888,1033,1036,1037,1044,1072,1048,1091,1083,424,940,1089,425,91,1097,88,92,93,94,1100,426,44,1103,587,1088,233,96,1128,1104,97,1105,1115,1117,98,1120,99,100,1122,427,428,1130,1129,1133,1135,1136,1123,101,349,1099,779,116,1004,265,492,1008,1010,499,792,158,126,828,827,495,1084,821,836,837,626,839,446,757,89,634,88,337,266,1027,908,495,595,0,849,1094,581,79,451,226,91,1124,266,92,93,94,0,95,44,0,0,0,106,96,0,0,97,0,0,0,889,372,99,100,65,0,0,0,1052,0,890,0,0,101,203,0,88,204,205,0,0,0,89,0,0,0,1070,0,0,0,0,0,0,349,756,0,0,425,91,0,1082,92,93,94,0,426,44,1087,0,0,1090,96,0,0,97,0,0,0,98,0,99,100,495,427,428,145,0,79,0,0,0,101,89,0,0,0,0,0,0,0,495,0,0,0,439,0,0,226,91,0,349,92,93,94,0,95,44,0,0,0,0,96,0,349,97,0,0,0,98,0,99,100,0,0,284,0,0,0,0,0,0,101,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,0,0,0,0,266,0,349,0,0,0,0,145,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,495,0,0,0,0,31,32,0,0,0,284,0,0,0,0,0,0,38,0,88,0,0,0,89,0,0,266,0,0,39,0,40,0,0,0,758,0,0,226,91,0,0,92,93,94,0,95,44,0,0,0,0,96,323,145,97,41,0,0,98,0,99,100,42,43,44,45,46,973,325,0,349,101,47,0,89,0,477,0,49,50,0,0,458,0,51,52,0,0,349,90,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,349,97,0,0,0,98,0,99,100,0,0,145,0,0,0,0,0,0,101,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,803,804,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,349,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,349,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,302,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,297,88,26,180,0,0,0,0,0,181,27,0,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,694,0,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,297,88,0,0,0,0,0,0,0,0,27,0,0,0,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,0,0,0,198,0,0,0,346,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,370,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,0,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,89,0,0,0,0,0,39,0,40,0,0,0,0,0,0,226,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,97,41,0,0,98,0,99,100,42,43,44,45,46,0,0,0,- 12,101,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,47,0,0,0,457,0,49,50,0,0,458,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,500,0,49,50,0,0,0,0,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,278,0,0,0,0,49,50,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,324,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,241,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,242,22,23,24,0,0,0,0,0,49,50,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,448,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,709,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,716,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,177,178,288,0,289,290,291,292,293,294,295,296,0,0,0,179,88,0,180,298,0,0,0,0,181,42,299,0,0,46,182,183,184,185,186,0,0,0,0,187,0,49,50,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,91,195,196,92,93,94,0,0,44,0,0,197,301,149,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,566,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,568,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,833,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,847,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,1028,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,0,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,330,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,350,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,521,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,613,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,645,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,700,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,771,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,786,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,808,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,810,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,913,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,916,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,919,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1003,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1007,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1009,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1081,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,776,91,195,196,92,93,94,188,0,44,189,190,197,191,0,0,0,198,0,0,0,199,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,350,0,0,198,0,0,0,199,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,195,196,92,93,94,188,0,44,189,190,197,191,744,0,0,198,0,0,0,199,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,0,0,0,198,0,0,0,343,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,195,196,92,93,94,188,0,44,189,190,197,191,0,0,0,198,0,0,0,346,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,0,0,0,198,0,0,0,199,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,972,0,0,200,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205};
# 2861
static short Cyc_yycheck[7950U]={0,198,125,26,201,202,73,21,278,70,180,25,155,181,26,378,181,73,168,380,155,21,139,170,221,25,304,305,381,307,464,414,383,0,941,70,246,176,540,384,385,155,618,280,199,17,118,795,48,63,64,65,124,67,21,116,266,57,25,73,112,65,244,63,64,65,89,67,120,390,391,110,139,73,74,392,393,89,101,480,317,116,149,139,151,109,110,242,114,101,57,484,370,5,6,151,63,64,65,80,67,553,141,350,92,506,73,386,387,388,389,545,266,113,114,119,125,865,114,1026,20,21,126,548,133,139,96,127,128,129,428,131,132,133,134,149,638,151,112,139,112,137,525,98,132,538,113,114,98,149,112,151,112,459,547,143,137,131,158,121,127,128,129,137,131,132,133,134,744,48,112,341,139,92,480,325,57,147,148,88,89,90,149,62,151,328,1093,142,343,131,345,346,142,645,123,249,128,394,395,396,339,129,131,501,133,228,361,133,140,128,139,234,279,132,112,127,228,240,643,231,148,149,234,279,133,93,94,125,240,138,139,266,113,114,114,0,134,304,305,137,307,131,112,122,244,112,127,128,129,284,131,132,133,134,121,680,21,137,428,496,25,131,129,131,132,279,133,131,322,239,452,453,454,152,141,142,129,244,119,279,128,143,125,297,338,147,256,93,94,137,304,305,57,307,135,148,149,297,63,64,65,571,67,370,304,305,689,307,73,333,125,112,279,130,464,767,112,113,54,771,121,138,139,477,120,131,132,88,89,90,297,501,133,544,786,335,133,304,305,411,307,119,134,415,112,770,500,111,113,114,775,143,120,121,122,427,370,135,125,784,133,128,127,128,129,133,131,132,133,134,370,618,133,112,139,95,96,138,139,129,678,120,121,122,149,540,151,111,585,126,813,141,142,818,133,545,112,125,277,112,139,826,112,370,527,121,429,120,121,122,126,863,126,530,437,125,129,429,128,133,133,131,874,447,702,437,139,306,141,142,123,123,133,445,827,112,113,291,131,131,133,133,477,120,298,480,139,139,112,419,517,134,452,453,454,86,125,121,728,526,143,461,885,530,619,500,621,98,131,133,617,506,136,134,530,138,134,477,126,479,138,728,143,638,115,452,453,454,128,489,128,491,143,128,134,793,147,744,128,137,500,648,137,517,715,659,649,137,678,128,125,535,526,128,279,662,530,517,137,673,663,128,489,131,491,128,526,134,663,0,530,76,297,126,137,649,112,507,508,304,305,128,307,114,120,121,122,134,971,663,517,22,763,125,977,26,134,28,130,526,138,711,129,530,707,835,133,735,199,126,735,131,139,716,141,142,999,48,748,128,1014,128,112,853,125,134,57,134,112,556,1015,62,125,133,825,128,120,121,122,70,125,72,126,128,75,370,853,238,489,632,491,133,128,84,119,978,628,139,89,980,632,125,126,793,659,856,1049,925,981,982,101,909,983,125,130,702,128,704,673,984,985,676,113,114,128,116,857,647,704,860,134,122,724,17,18,19,127,128,129,870,131,132,133,134,133,296,666,739,740,990,991,134,526,128,128,992,993,947,125,134,134,152,128,128,708,655,1109,120,702,134,704,125,128,890,128,125,126,751,134,977,947,246,702,1126,704,176,125,986,987,988,989,908,343,133,768,346,128,125,949,950,128,266,134,156,489,956,491,358,782,133,128,60,128,128,889,702,134,704,134,284,951,128,130,128,128,128,957,134,379,134,134,134,517,128,721,925,128,228,881,134,231,526,134,234,125,530,133,128,127,240,99,100,101,102,103,104,105,106,107,108,803,936,91,92,125,416,929,125,788,125,128,803,128,125,133,266,128,994,995,996,1031,130,133,144,1035,133,277,452,453,454,83,84,133,284,155,286,133,972,289,974,133,788,5,6,18,75,76,77,78,79,133,1109,82,83,84,306,127,128,692,133,112,876,909,22,23,278,127,128,120,121,122,1048,127,128,133,145,146,129,97,931,144,133,133,1018,1014,137,95,96,339,141,142,133,177,133,179,180,181,145,878,510,511,91,92,514,127,128,517,146,193,194,798,799,126,198,119,909,201,202,112,332,112,133,243,964,245,112,132,788,127,909,977,878,457,134,127,126,221,350,127,127,353,354,355,356,357,127,126,360,133,964,363,112,477,125,1089,133,133,702,1108,704,373,282,909,376,137,125,800,133,130,127,112,140,806,143,123,500,127,134,429,98,134,112,137,977,397,112,437,133,131,120,121,122,123,41,445,134,409,977,129,132,132,414,133,132,127,127,112,128,134,137,141,142,297,128,120,121,122,878,134,128,68,133,112,129,1039,126,477,133,133,480,977,134,128,47,125,141,142,137,489,128,491,133,653,788,126,458,30,125,45,500,133,80,112,1105,134,506,341,143,134,134,128,1102,1109,134,112,113,114,115,116,117,118,137,134,121,128,128,128,128,134,913,114,127,916,694,695,133,127,1102,136,134,134,134,134,378,134,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,525,112,527,1109,128,140,134,134,140,120,121,122,125,134,134,46,127,1109,129,126,125,878,659,969,457,133,127,133,126,81,134,428,125,127,126,132,673,25,112,676,131,112,137,112,477,112,571,121,134,123,1109,484,909,134,121,129,1002,1003,125,133,1006,1007,129,1009,134,628,133,141,142,632,128,134,140,134,141,142,140,134,134,126,48,126,41,133,807,808,134,810,137,812,127,125,133,618,527,119,659,113,134,134,133,128,134,125,501,538,134,127,134,832,673,134,126,676,547,134,134,127,28,126,977,133,685,97,137,650,97,144,127,1075,128,656,1078,25,1080,1081,133,127,97,664,25,26,128,127,127,127,125,134,114,134,109,114,133,112,113,113,41,116,117,118,127,120,121,127,689,134,889,126,1114,134,129,128,127,134,133,128,135,136,134,138,139,114,134,134,0,0,1102,146,585,1039,658,39,915,124,277,918,919,282,676,75,51,721,716,728,1014,711,731,732,430,734,240,628,97,445,41,169,649,942,803,744,382,- 1,747,1023,362,788,659,112,113,1105,663,116,117,118,- 1,120,121,- 1,- 1,- 1,803,126,- 1,- 1,129,- 1,- 1,- 1,133,972,135,136,1109,- 1,- 1,- 1,979,- 1,143,- 1,- 1,146,147,- 1,41,150,151,- 1,- 1,- 1,97,- 1,- 1,- 1,997,- 1,- 1,- 1,- 1,- 1,- 1,678,109,- 1,- 1,112,113,- 1,1011,116,117,118,- 1,120,121,1018,- 1,- 1,1021,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,835,138,139,876,- 1,878,- 1,- 1,- 1,146,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,853,- 1,- 1,- 1,109,- 1,- 1,112,113,- 1,735,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,748,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,795,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,- 1,- 1,- 1,- 1,827,- 1,793,- 1,- 1,- 1,- 1,964,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,947,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,865,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,41,- 1,- 1,- 1,97,- 1,- 1,881,- 1,- 1,85,- 1,87,- 1,- 1,- 1,109,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,109,1039,129,112,- 1,- 1,133,- 1,135,136,119,120,121,122,123,887,125,- 1,890,146,129,- 1,97,- 1,133,- 1,135,136,- 1,- 1,139,- 1,141,142,- 1,- 1,908,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,925,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,1102,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,978,- 1,980,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,1048,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1108,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,112,- 1,- 1,133,- 1,135,136,119,120,121,122,123,- 1,- 1,- 1,127,146,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,127,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,139,- 1,141,142,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,37,38,39,40,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,37,38,39,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,119,51,- 1,- 1,123,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,135,136,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,32,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp42C[8U]="stdcall";static char _tmp42D[6U]="cdecl";static char _tmp42E[9U]="fastcall";static char _tmp42F[9U]="noreturn";static char _tmp430[6U]="const";static char _tmp431[8U]="aligned";static char _tmp432[7U]="packed";static char _tmp433[7U]="shared";static char _tmp434[7U]="unused";static char _tmp435[5U]="weak";static char _tmp436[10U]="dllimport";static char _tmp437[10U]="dllexport";static char _tmp438[23U]="no_instrument_function";static char _tmp439[12U]="constructor";static char _tmp43A[11U]="destructor";static char _tmp43B[22U]="no_check_memory_usage";static char _tmp43C[5U]="pure";static char _tmp43D[14U]="always_inline";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp3A0=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp3A0;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp779;(_tmp779.YYINITIALSVAL).tag=72U,(_tmp779.YYINITIALSVAL).val=0;_tmp779;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp6B8=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp6B8),sizeof(short),_tmp6B8);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp6B7=200U;struct Cyc_Yystacktype*_tmp6B6=({struct _RegionHandle*_tmp832=yyregion;_region_malloc(_tmp832,_check_times(_tmp6B7,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp778=200U;unsigned i;for(i=0;i < _tmp778;++ i){(_tmp6B6[i]).v=yylval,({struct Cyc_Yyltype _tmp833=Cyc_yynewloc();(_tmp6B6[i]).l=_tmp833;});}}0;});_tmp6B6;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp835=({const char*_tmp3A1="parser stack overflow";_tag_fat(_tmp3A1,sizeof(char),22U);});int _tmp834=yystate;Cyc_yyerror(_tmp835,_tmp834,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp3A5=(unsigned)yystacksize;short*_tmp3A4=({struct _RegionHandle*_tmp836=yyregion;_region_malloc(_tmp836,_check_times(_tmp3A5,sizeof(short)));});({{unsigned _tmp72F=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp72F;++ i){
i <= (unsigned)yyssp_offset?_tmp3A4[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp3A4[i]=0);}}0;});_tag_fat(_tmp3A4,sizeof(short),_tmp3A5);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp3A3=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp3A2=({struct _RegionHandle*_tmp837=yyregion;_region_malloc(_tmp837,_check_times(_tmp3A3,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp72E=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp72E;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp3A2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp3A2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp3A2,sizeof(struct Cyc_Yystacktype),_tmp3A3);});
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
int _tmp3A6=0;_npop_handler(0U);return _tmp3A6;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp838=({struct Cyc_Yystacktype _tmp730;_tmp730.v=yylval,_tmp730.l=yylloc;_tmp730;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp838;});
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
{int _tmp3A7=yyn;switch(_tmp3A7){case 1U: _LL1: _LL2:
# 1205 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1208
goto _LL0;case 2U: _LL3: _LL4:
# 1211 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp839=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp839,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1215 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct Cyc_Absyn_Decl*_tmp83D=({struct Cyc_Absyn_Decl*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({void*_tmp83C=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=10U,({struct _tuple0*_tmp83B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A8->f1=_tmp83B;}),({struct Cyc_List_List*_tmp83A=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A8->f2=_tmp83A;});_tmp3A8;});_tmp3A9->r=_tmp83C;}),_tmp3A9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A9;});_tmp3AA->hd=_tmp83D;}),_tmp3AA->tl=0;_tmp3AA;}));
Cyc_Lex_leave_using();
# 1218
goto _LL0;case 4U: _LL7: _LL8:
# 1219 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_Decl*_tmp842=({struct Cyc_Absyn_Decl*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({void*_tmp841=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->tag=10U,({struct _tuple0*_tmp840=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3AB->f1=_tmp840;}),({struct Cyc_List_List*_tmp83F=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3AB->f2=_tmp83F;});_tmp3AB;});_tmp3AC->r=_tmp841;}),_tmp3AC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AC;});_tmp3AD->hd=_tmp842;}),({struct Cyc_List_List*_tmp83E=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3AD->tl=_tmp83E;});_tmp3AD;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1222
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct Cyc_Absyn_Decl*_tmp847=({struct Cyc_Absyn_Decl*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));({void*_tmp846=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->tag=9U,({struct _fat_ptr*_tmp845=({struct _fat_ptr*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));({struct _fat_ptr _tmp844=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3AE=_tmp844;});_tmp3AE;});_tmp3AF->f1=_tmp845;}),({struct Cyc_List_List*_tmp843=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3AF->f2=_tmp843;});_tmp3AF;});_tmp3B0->r=_tmp846;}),_tmp3B0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B0;});_tmp3B1->hd=_tmp847;}),_tmp3B1->tl=0;_tmp3B1;}));
Cyc_Lex_leave_namespace();
# 1225
goto _LL0;case 6U: _LLB: _LLC:
# 1226 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));({struct Cyc_Absyn_Decl*_tmp84D=({struct Cyc_Absyn_Decl*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));({void*_tmp84C=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->tag=9U,({struct _fat_ptr*_tmp84B=({struct _fat_ptr*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct _fat_ptr _tmp84A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3B2=_tmp84A;});_tmp3B2;});_tmp3B3->f1=_tmp84B;}),({struct Cyc_List_List*_tmp849=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B3->f2=_tmp849;});_tmp3B3;});_tmp3B4->r=_tmp84C;}),_tmp3B4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B4;});_tmp3B5->hd=_tmp84D;}),({struct Cyc_List_List*_tmp848=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3B5->tl=_tmp848;});_tmp3B5;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1228 "parse.y"
int _tmp3B6=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3B6;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3B7=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp18=_tmp3B7;struct _tuple28*_tmp3B8=_stmttmp18;unsigned _tmp3BA;struct Cyc_List_List*_tmp3B9;_LL462: _tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B8->f2;_LL463: {struct Cyc_List_List*exs=_tmp3B9;unsigned wc=_tmp3BA;
struct Cyc_List_List*_tmp3BB=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3BB;
if(exs != 0 && hides != 0)
({void*_tmp3BC=0U;({unsigned _tmp84F=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp84E=({const char*_tmp3BD="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3BD,sizeof(char),65U);});Cyc_Warn_err(_tmp84F,_tmp84E,_tag_fat(_tmp3BC,sizeof(void*),0U));});});
# 1235
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1238
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3BE=0U;({unsigned _tmp851=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp850=({const char*_tmp3BF="expecting \"C include\"";_tag_fat(_tmp3BF,sizeof(char),22U);});Cyc_Warn_err(_tmp851,_tmp850,_tag_fat(_tmp3BE,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct Cyc_Absyn_Decl*_tmp856=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({void*_tmp855=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->tag=12U,({struct Cyc_List_List*_tmp854=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C1->f1=_tmp854;}),_tmp3C1->f2=cycdecls,_tmp3C1->f3=exs,({struct _tuple10*_tmp853=({struct _tuple10*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->f1=wc,_tmp3C0->f2=hides;_tmp3C0;});_tmp3C1->f4=_tmp853;});_tmp3C1;});_tmp3C2->r=_tmp855;}),_tmp3C2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C2;});_tmp3C3->hd=_tmp856;}),({struct Cyc_List_List*_tmp852=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3C3->tl=_tmp852;});_tmp3C3;}));}else{
# 1244
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct Cyc_Absyn_Decl*_tmp85A=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({void*_tmp859=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->tag=11U,({struct Cyc_List_List*_tmp858=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C4->f1=_tmp858;});_tmp3C4;});_tmp3C5->r=_tmp859;}),_tmp3C5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C5;});_tmp3C6->hd=_tmp85A;}),({struct Cyc_List_List*_tmp857=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3C6->tl=_tmp857;});_tmp3C6;}));}}else{
# 1248
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct Cyc_Absyn_Decl*_tmp85F=({struct Cyc_Absyn_Decl*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({void*_tmp85E=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=12U,({struct Cyc_List_List*_tmp85D=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C8->f1=_tmp85D;}),_tmp3C8->f2=cycdecls,_tmp3C8->f3=exs,({struct _tuple10*_tmp85C=({struct _tuple10*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->f1=wc,_tmp3C7->f2=hides;_tmp3C7;});_tmp3C8->f4=_tmp85C;});_tmp3C8;});_tmp3C9->r=_tmp85E;}),_tmp3C9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C9;});_tmp3CA->hd=_tmp85F;}),({struct Cyc_List_List*_tmp85B=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3CA->tl=_tmp85B;});_tmp3CA;}));}
# 1251
goto _LL0;}}case 8U: _LLF: _LL10:
# 1252 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));({struct Cyc_Absyn_Decl*_tmp861=({struct Cyc_Absyn_Decl*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3CB->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3CB;});_tmp3CC->hd=_tmp861;}),({struct Cyc_List_List*_tmp860=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3CC->tl=_tmp860;});_tmp3CC;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1254 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct Cyc_Absyn_Decl*_tmp863=({struct Cyc_Absyn_Decl*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3CD->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3CD;});_tmp3CE->hd=_tmp863;}),({struct Cyc_List_List*_tmp862=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3CE->tl=_tmp862;});_tmp3CE;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1256 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct Cyc_Absyn_Decl*_tmp865=({struct Cyc_Absyn_Decl*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3CF->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3CF;});_tmp3D0->hd=_tmp865;}),({struct Cyc_List_List*_tmp864=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3D0->tl=_tmp864;});_tmp3D0;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1258 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct Cyc_Absyn_Decl*_tmp867=({struct Cyc_Absyn_Decl*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3D1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D1;});_tmp3D2->hd=_tmp867;}),({struct Cyc_List_List*_tmp866=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3D2->tl=_tmp866;});_tmp3D2;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1259 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1264 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1269 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1274 "parse.y"
struct _fat_ptr _tmp3D3=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3D3;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp868=(struct _fat_ptr)two;Cyc_strcmp(_tmp868,({const char*_tmp3D4="C";_tag_fat(_tmp3D4,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp869=(struct _fat_ptr)two;Cyc_strcmp(_tmp869,({const char*_tmp3D5="C include";_tag_fat(_tmp3D5,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1281
({void*_tmp3D6=0U;({unsigned _tmp86B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp86A=({const char*_tmp3D7="expecting \"C\" or \"C include\"";_tag_fat(_tmp3D7,sizeof(char),29U);});Cyc_Warn_err(_tmp86B,_tmp86A,_tag_fat(_tmp3D6,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1285
goto _LL0;}case 16U: _LL1F: _LL20:
# 1288 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1292 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1293 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1297 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));({struct _tuple0*_tmp86C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D8->hd=_tmp86C;}),_tmp3D8->tl=0;_tmp3D8;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1298 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct _tuple0*_tmp86D=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D9->hd=_tmp86D;}),_tmp3D9->tl=0;_tmp3D9;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1300 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({struct _tuple0*_tmp86F=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3DA->hd=_tmp86F;}),({struct Cyc_List_List*_tmp86E=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3DA->tl=_tmp86E;});_tmp3DA;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1304 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->f1=0,_tmp3DB->f2=0U;_tmp3DB;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1305 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1309 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));({struct Cyc_List_List*_tmp870=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3DC->f1=_tmp870;}),_tmp3DC->f2=0U;_tmp3DC;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1310 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->f1=0,_tmp3DD->f2=0U;_tmp3DD;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1311 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->f1=0,_tmp3DE->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3DE;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1315 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _tuple32*_tmp872=({struct _tuple32*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp871=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3DF->f2=_tmp871;}),_tmp3DF->f3=0;_tmp3DF;});_tmp3E0->hd=_tmp872;}),_tmp3E0->tl=0;_tmp3E0;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1316 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct _tuple32*_tmp874=({struct _tuple32*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp873=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3E1->f2=_tmp873;}),_tmp3E1->f3=0;_tmp3E1;});_tmp3E2->hd=_tmp874;}),_tmp3E2->tl=0;_tmp3E2;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1318 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct _tuple32*_tmp877=({struct _tuple32*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp876=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3E3->f2=_tmp876;}),_tmp3E3->f3=0;_tmp3E3;});_tmp3E4->hd=_tmp877;}),({struct Cyc_List_List*_tmp875=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3E4->tl=_tmp875;});_tmp3E4;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1322 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1323 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1327 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({struct Cyc_Absyn_Decl*_tmp87A=({void*_tmp879=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp878=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3E5->f1=_tmp878;});_tmp3E5;});Cyc_Absyn_new_decl(_tmp879,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E6->hd=_tmp87A;}),_tmp3E6->tl=0;_tmp3E6;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1328 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1329 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1338 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp87D=yyr;struct Cyc_Parse_Declarator _tmp87C=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp87B=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp87D,0,_tmp87C,0,_tmp87B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1340 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E7=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3E7;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp880=yyr;struct Cyc_Parse_Declarator _tmp87F=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp87E=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp880,& d,_tmp87F,0,_tmp87E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1353 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp884=yyr;struct Cyc_Parse_Declarator _tmp883=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp882=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp881=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp884,0,_tmp883,_tmp882,_tmp881,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1355 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E8=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3E8;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp888=yyr;struct Cyc_Parse_Declarator _tmp887=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp886=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp885=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp888,& d,_tmp887,_tmp886,_tmp885,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1363 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E9=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3E9;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp88B=yyr;struct Cyc_Parse_Declarator _tmp88A=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp889=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp88B,& d,_tmp88A,0,_tmp889,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1366 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3EA=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3EA;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp88F=yyr;struct Cyc_Parse_Declarator _tmp88E=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp88D=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp88C=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp88F,& d,_tmp88E,_tmp88D,_tmp88C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1371 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1374
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1377
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct _fat_ptr _tmp890=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3EB=_tmp890;});_tmp3EB;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1380
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1386 "parse.y"
int _tmp3EC=((yyyvsp[0]).l).first_line;int location=_tmp3EC;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp892=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp891=(unsigned)location;Cyc_Parse_make_declarations(_tmp892,0,_tmp891,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1389 "parse.y"
int _tmp3ED=((yyyvsp[0]).l).first_line;int location=_tmp3ED;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp895=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp894=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp893=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp895,_tmp894,_tmp893,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1393
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));({struct Cyc_Absyn_Decl*_tmp898=({struct Cyc_Absyn_Pat*_tmp897=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp896=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp897,_tmp896,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3EE->hd=_tmp898;}),_tmp3EE->tl=0;_tmp3EE;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1395 "parse.y"
struct Cyc_List_List*_tmp3EF=0;struct Cyc_List_List*vds=_tmp3EF;
{struct Cyc_List_List*_tmp3F0=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3F0;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3F1=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3F1;
struct _tuple0*qv=({struct _tuple0*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));({union Cyc_Absyn_Nmspace _tmp899=Cyc_Absyn_Rel_n(0);_tmp3F4->f1=_tmp899;}),_tmp3F4->f2=id;_tmp3F4;});
struct Cyc_Absyn_Vardecl*_tmp3F2=({struct _tuple0*_tmp89A=qv;Cyc_Absyn_new_vardecl(0U,_tmp89A,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3F2;
vds=({struct Cyc_List_List*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->hd=vd,_tmp3F3->tl=vds;_tmp3F3;});}}
# 1402
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));({struct Cyc_Absyn_Decl*_tmp89B=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F5->hd=_tmp89B;}),_tmp3F5->tl=0;_tmp3F5;}));
# 1405
goto _LL0;}case 51U: _LL61: _LL62: {
# 1408 "parse.y"
struct _fat_ptr _tmp3F6=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3F6;
struct _fat_ptr err=({const char*_tmp3FD="";_tag_fat(_tmp3FD,sizeof(char),1U);});
if(!Cyc_Parse_tvar_ok(three,& err))({void*_tmp3F7=0U;({unsigned _tmp89D=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp89C=err;Cyc_Warn_err(_tmp89D,_tmp89C,_tag_fat(_tmp3F7,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));({struct _fat_ptr*_tmp89F=({struct _fat_ptr*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));*_tmp3FB=three;_tmp3FB;});_tmp3FC->name=_tmp89F;}),_tmp3FC->identity=- 1,({void*_tmp89E=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3FC->kind=_tmp89E;});_tmp3FC;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A3=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp8A2=({struct _tuple0*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));_tmp3FA->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A1=({struct _fat_ptr*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({struct _fat_ptr _tmp8A0=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3F9=_tmp8A0;});_tmp3F9;});_tmp3FA->f2=_tmp8A1;});_tmp3FA;});Cyc_Absyn_new_vardecl(_tmp8A3,_tmp8A2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));({struct Cyc_Absyn_Decl*_tmp8A4=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F8->hd=_tmp8A4;}),_tmp3F8->tl=0;_tmp3F8;}));
# 1416
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1418
struct _fat_ptr _tmp3FE=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3FE;
if(({struct _fat_ptr _tmp8A5=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp8A5,({const char*_tmp3FF="H";_tag_fat(_tmp3FF,sizeof(char),2U);}));})== 0)
({void*_tmp400=0U;({unsigned _tmp8A7=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp8A6=({const char*_tmp401="bad occurrence of heap region `H";_tag_fat(_tmp401,sizeof(char),33U);});Cyc_Warn_err(_tmp8A7,_tmp8A6,_tag_fat(_tmp400,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp8A8=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp8A8,({const char*_tmp402="U";_tag_fat(_tmp402,sizeof(char),2U);}));})== 0)
({void*_tmp403=0U;({unsigned _tmp8AA=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp8A9=({const char*_tmp404="bad occurrence of unique region `U";_tag_fat(_tmp404,sizeof(char),35U);});Cyc_Warn_err(_tmp8AA,_tmp8A9,_tag_fat(_tmp403,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp40C=_cycalloc(sizeof(*_tmp40C));({struct _fat_ptr*_tmp8AE=({struct _fat_ptr*_tmp40B=_cycalloc(sizeof(*_tmp40B));({struct _fat_ptr _tmp8AD=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmp731;_tmp731.tag=0U,_tmp731.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp731;});void*_tmp408[1U];_tmp408[0]=& _tmp40A;({struct _fat_ptr _tmp8AC=({const char*_tmp409="`%s";_tag_fat(_tmp409,sizeof(char),4U);});Cyc_aprintf(_tmp8AC,_tag_fat(_tmp408,sizeof(void*),1U));});});*_tmp40B=_tmp8AD;});_tmp40B;});_tmp40C->name=_tmp8AE;}),_tmp40C->identity=- 1,({
void*_tmp8AB=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp40C->kind=_tmp8AB;});_tmp40C;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8B1=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp8B0=({struct _tuple0*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8AF=({struct _fat_ptr*_tmp406=_cycalloc(sizeof(*_tmp406));*_tmp406=two;_tmp406;});_tmp407->f2=_tmp8AF;});_tmp407;});Cyc_Absyn_new_vardecl(_tmp8B1,_tmp8B0,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp405=_cycalloc(sizeof(*_tmp405));({struct Cyc_Absyn_Decl*_tmp8B2=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp405->hd=_tmp8B2;}),_tmp405->tl=0;_tmp405;}));
# 1429
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1431
struct _fat_ptr _tmp40D=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp40D;
struct _fat_ptr _tmp40E=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp40E;
struct Cyc_Absyn_Exp*_tmp40F=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp40F;
if(({struct _fat_ptr _tmp8B3=(struct _fat_ptr)four;Cyc_strcmp(_tmp8B3,({const char*_tmp410="open";_tag_fat(_tmp410,sizeof(char),5U);}));})!= 0)({void*_tmp411=0U;({unsigned _tmp8B5=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp8B4=({const char*_tmp412="expecting `open'";_tag_fat(_tmp412,sizeof(char),17U);});Cyc_Warn_err(_tmp8B5,_tmp8B4,_tag_fat(_tmp411,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp41A=_cycalloc(sizeof(*_tmp41A));({struct _fat_ptr*_tmp8B9=({struct _fat_ptr*_tmp419=_cycalloc(sizeof(*_tmp419));({struct _fat_ptr _tmp8B8=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp418=({struct Cyc_String_pa_PrintArg_struct _tmp732;_tmp732.tag=0U,_tmp732.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp732;});void*_tmp416[1U];_tmp416[0]=& _tmp418;({struct _fat_ptr _tmp8B7=({const char*_tmp417="`%s";_tag_fat(_tmp417,sizeof(char),4U);});Cyc_aprintf(_tmp8B7,_tag_fat(_tmp416,sizeof(void*),1U));});});*_tmp419=_tmp8B8;});_tmp419;});_tmp41A->name=_tmp8B9;}),_tmp41A->identity=- 1,({
void*_tmp8B6=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp41A->kind=_tmp8B6;});_tmp41A;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8BC=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8BB=({struct _tuple0*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8BA=({struct _fat_ptr*_tmp414=_cycalloc(sizeof(*_tmp414));*_tmp414=two;_tmp414;});_tmp415->f2=_tmp8BA;});_tmp415;});Cyc_Absyn_new_vardecl(_tmp8BC,_tmp8BB,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp413=_cycalloc(sizeof(*_tmp413));({struct Cyc_Absyn_Decl*_tmp8BD=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp413->hd=_tmp8BD;}),_tmp413->tl=0;_tmp413;}));
# 1441
goto _LL0;}}case 54U: _LL67: _LL68:
# 1445 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55U: _LL69: _LL6A:
# 1447 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8BE=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8BE,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56U: _LL6B: _LL6C:
# 1453 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp733;({enum Cyc_Parse_Storage_class*_tmp8C1=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp733.sc=_tmp8C1;}),({struct Cyc_Absyn_Tqual _tmp8C0=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp733.tq=_tmp8C0;}),({
struct Cyc_Parse_Type_specifier _tmp8BF=Cyc_Parse_empty_spec(0U);_tmp733.type_specs=_tmp8BF;}),_tmp733.is_inline=0,_tmp733.attributes=0;_tmp733;}));
goto _LL0;case 57U: _LL6D: _LL6E: {
# 1456 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41B=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41B;
if(two.sc != 0)
({void*_tmp41C=0U;({unsigned _tmp8C3=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C2=({const char*_tmp41D="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp41D,sizeof(char),73U);});Cyc_Warn_warn(_tmp8C3,_tmp8C2,_tag_fat(_tmp41C,sizeof(void*),0U));});});
# 1460
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp734;({enum Cyc_Parse_Storage_class*_tmp8C4=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp734.sc=_tmp8C4;}),_tmp734.tq=two.tq,_tmp734.type_specs=two.type_specs,_tmp734.is_inline=two.is_inline,_tmp734.attributes=two.attributes;_tmp734;}));
# 1464
goto _LL0;}case 58U: _LL6F: _LL70:
# 1465 "parse.y"
({void*_tmp41E=0U;({unsigned _tmp8C6=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C5=({const char*_tmp41F="__extension__ keyword ignored in declaration";_tag_fat(_tmp41F,sizeof(char),45U);});Cyc_Warn_warn(_tmp8C6,_tmp8C5,_tag_fat(_tmp41E,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1468
goto _LL0;case 59U: _LL71: _LL72:
# 1469 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp735;_tmp735.sc=0,({struct Cyc_Absyn_Tqual _tmp8C8=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp735.tq=_tmp8C8;}),({
struct Cyc_Parse_Type_specifier _tmp8C7=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp735.type_specs=_tmp8C7;}),_tmp735.is_inline=0,_tmp735.attributes=0;_tmp735;}));
goto _LL0;case 60U: _LL73: _LL74: {
# 1472 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp420=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp420;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp736;_tmp736.sc=two.sc,_tmp736.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8CB=({unsigned _tmp8CA=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8C9=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8CA,_tmp8C9,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp736.type_specs=_tmp8CB;}),_tmp736.is_inline=two.is_inline,_tmp736.attributes=two.attributes;_tmp736;}));
# 1478
goto _LL0;}case 61U: _LL75: _LL76:
# 1479 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp737;_tmp737.sc=0,({struct Cyc_Absyn_Tqual _tmp8CD=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp737.tq=_tmp8CD;}),({struct Cyc_Parse_Type_specifier _tmp8CC=Cyc_Parse_empty_spec(0U);_tmp737.type_specs=_tmp8CC;}),_tmp737.is_inline=0,_tmp737.attributes=0;_tmp737;}));
goto _LL0;case 62U: _LL77: _LL78: {
# 1481 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp421=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp421;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp738;_tmp738.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8CF=({struct Cyc_Absyn_Tqual _tmp8CE=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8CE,two.tq);});_tmp738.tq=_tmp8CF;}),_tmp738.type_specs=two.type_specs,_tmp738.is_inline=two.is_inline,_tmp738.attributes=two.attributes;_tmp738;}));
# 1486
goto _LL0;}case 63U: _LL79: _LL7A:
# 1487 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp739;_tmp739.sc=0,({struct Cyc_Absyn_Tqual _tmp8D1=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp739.tq=_tmp8D1;}),({
struct Cyc_Parse_Type_specifier _tmp8D0=Cyc_Parse_empty_spec(0U);_tmp739.type_specs=_tmp8D0;}),_tmp739.is_inline=1,_tmp739.attributes=0;_tmp739;}));
goto _LL0;case 64U: _LL7B: _LL7C: {
# 1490 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp422=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp422;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp73A;_tmp73A.sc=two.sc,_tmp73A.tq=two.tq,_tmp73A.type_specs=two.type_specs,_tmp73A.is_inline=1,_tmp73A.attributes=two.attributes;_tmp73A;}));
# 1494
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1495 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp73B;_tmp73B.sc=0,({struct Cyc_Absyn_Tqual _tmp8D4=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73B.tq=_tmp8D4;}),({
struct Cyc_Parse_Type_specifier _tmp8D3=Cyc_Parse_empty_spec(0U);_tmp73B.type_specs=_tmp8D3;}),_tmp73B.is_inline=0,({struct Cyc_List_List*_tmp8D2=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp73B.attributes=_tmp8D2;});_tmp73B;}));
goto _LL0;case 66U: _LL7F: _LL80: {
# 1498 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp423=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp423;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp73C;_tmp73C.sc=two.sc,_tmp73C.tq=two.tq,_tmp73C.type_specs=two.type_specs,_tmp73C.is_inline=two.is_inline,({
# 1501
struct Cyc_List_List*_tmp8D6=({struct Cyc_List_List*_tmp8D5=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8D5,two.attributes);});_tmp73C.attributes=_tmp8D6;});_tmp73C;}));
goto _LL0;}case 67U: _LL81: _LL82: {
# 1505 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;}case 68U: _LL83: _LL84: {
# 1507 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;}case 69U: _LL85: _LL86: {
# 1509 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;}case 70U: _LL87: _LL88: {
# 1511 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1514 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _fat_ptr _tmp8D7=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8D7,({const char*_tmp424="C";_tag_fat(_tmp424,sizeof(char),2U);}));})!= 0)
({void*_tmp425=0U;({unsigned _tmp8D9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8D8=({const char*_tmp426="only extern or extern \"C\" is allowed";_tag_fat(_tmp426,sizeof(char),37U);});Cyc_Warn_err(_tmp8D9,_tmp8D8,_tag_fat(_tmp425,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1519
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1519 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1522 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;}case 74U: _LL8F: _LL90:
# 1528 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 75U: _LL91: _LL92:
# 1529 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76U: _LL93: _LL94:
# 1534 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1538 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp427=_cycalloc(sizeof(*_tmp427));({void*_tmp8DA=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp427->hd=_tmp8DA;}),_tmp427->tl=0;_tmp427;}));
goto _LL0;case 78U: _LL97: _LL98:
# 1539 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));({void*_tmp8DC=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp428->hd=_tmp8DC;}),({struct Cyc_List_List*_tmp8DB=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp428->tl=_tmp8DB;});_tmp428;}));
goto _LL0;case 79U: _LL99: _LL9A: {
# 1544 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[18U]={{{_tmp42C,_tmp42C,_tmp42C + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp42D,_tmp42D,_tmp42D + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp42E,_tmp42E,_tmp42E + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp42F,_tmp42F,_tmp42F + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp430,_tmp430,_tmp430 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp431,_tmp431,_tmp431 + 8U},(void*)& att_aligned},{{_tmp432,_tmp432,_tmp432 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp433,_tmp433,_tmp433 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp434,_tmp434,_tmp434 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp435,_tmp435,_tmp435 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp436,_tmp436,_tmp436 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp437,_tmp437,_tmp437 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp438,_tmp438,_tmp438 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp439,_tmp439,_tmp439 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp43A,_tmp43A,_tmp43A + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp43B,_tmp43B,_tmp43B + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp43C,_tmp43C,_tmp43C + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp43D,_tmp43D,_tmp43D + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1565
struct _fat_ptr _tmp429=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp429;
# 1567
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1569
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 18U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1576
if((unsigned)i == 18U){
({void*_tmp42A=0U;({unsigned _tmp8DE=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8DD=({const char*_tmp42B="unrecognized attribute";_tag_fat(_tmp42B,sizeof(char),23U);});Cyc_Warn_err(_tmp8DE,_tmp8DD,_tag_fat(_tmp42A,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1581
goto _LL0;}}case 80U: _LL9B: _LL9C:
# 1581 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 81U: _LL9D: _LL9E: {
# 1583 "parse.y"
struct _fat_ptr _tmp43E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp43E;
struct Cyc_Absyn_Exp*_tmp43F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp43F;
void*a;
if(({struct _fat_ptr _tmp8E0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E0,({const char*_tmp440="aligned";_tag_fat(_tmp440,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8DF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DF,({const char*_tmp441="__aligned__";_tag_fat(_tmp441,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=6U,_tmp442->f1=e;_tmp442;});else{
if(({struct _fat_ptr _tmp8E2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E2,({const char*_tmp443="section";_tag_fat(_tmp443,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8E1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E1,({const char*_tmp444="__section__";_tag_fat(_tmp444,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=8U,({struct _fat_ptr _tmp8E3=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp445->f1=_tmp8E3;});_tmp445;});else{
if(({struct _fat_ptr _tmp8E4=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E4,({const char*_tmp446="__mode__";_tag_fat(_tmp446,sizeof(char),9U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=24U,({struct _fat_ptr _tmp8E5=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp447->f1=_tmp8E5;});_tmp447;});else{
if(({struct _fat_ptr _tmp8E6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E6,({const char*_tmp448="alias";_tag_fat(_tmp448,sizeof(char),6U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->tag=25U,({struct _fat_ptr _tmp8E7=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);_tmp449->f1=_tmp8E7;});_tmp449;});else{
# 1595
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8E9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E9,({const char*_tmp44A="regparm";_tag_fat(_tmp44A,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8E8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E8,({const char*_tmp44B="__regparm__";_tag_fat(_tmp44B,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp44C=0U;({unsigned _tmp8EB=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8EA=({const char*_tmp44D="regparm requires value between 0 and 3";_tag_fat(_tmp44D,sizeof(char),39U);});Cyc_Warn_err(_tmp8EB,_tmp8EA,_tag_fat(_tmp44C,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=0U,_tmp44E->f1=n;_tmp44E;});}else{
if(({struct _fat_ptr _tmp8ED=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8ED,({const char*_tmp44F="initializes";_tag_fat(_tmp44F,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8EC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8EC,({const char*_tmp450="__initializes__";_tag_fat(_tmp450,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=20U,_tmp451->f1=n;_tmp451;});else{
if(({struct _fat_ptr _tmp8EF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8EF,({const char*_tmp452="noliveunique";_tag_fat(_tmp452,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8EE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8EE,({const char*_tmp453="__noliveunique__";_tag_fat(_tmp453,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->tag=21U,_tmp454->f1=n;_tmp454;});else{
if(({struct _fat_ptr _tmp8F1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8F1,({const char*_tmp455="consume";_tag_fat(_tmp455,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8F0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8F0,({const char*_tmp456="__consume__";_tag_fat(_tmp456,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->tag=22U,_tmp457->f1=n;_tmp457;});else{
# 1607
({void*_tmp458=0U;({unsigned _tmp8F3=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8F2=({const char*_tmp459="unrecognized attribute";_tag_fat(_tmp459,sizeof(char),23U);});Cyc_Warn_err(_tmp8F3,_tmp8F2,_tag_fat(_tmp458,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1611
yyval=Cyc_YY46(a);
# 1613
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1614 "parse.y"
struct _fat_ptr _tmp45A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp45A;
struct _fat_ptr _tmp45B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp45B;
unsigned _tmp45C=({unsigned _tmp8F4=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8F4,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp45C;
unsigned _tmp45D=({unsigned _tmp8F5=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8F5,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp45D;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8F7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8F7,({const char*_tmp45E="format";_tag_fat(_tmp45E,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8F6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8F6,({const char*_tmp45F="__format__";_tag_fat(_tmp45F,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8F9=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8F9,({const char*_tmp460="printf";_tag_fat(_tmp460,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8F8=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8F8,({const char*_tmp461="__printf__";_tag_fat(_tmp461,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->tag=19U,_tmp462->f1=Cyc_Absyn_Printf_ft,_tmp462->f2=(int)n,_tmp462->f3=(int)m;_tmp462;});else{
if(({struct _fat_ptr _tmp8FB=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8FB,({const char*_tmp463="scanf";_tag_fat(_tmp463,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8FA=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8FA,({const char*_tmp464="__scanf__";_tag_fat(_tmp464,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->tag=19U,_tmp465->f1=Cyc_Absyn_Scanf_ft,_tmp465->f2=(int)n,_tmp465->f3=(int)m;_tmp465;});else{
# 1625
({void*_tmp466=0U;({unsigned _tmp8FD=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8FC=({const char*_tmp467="unrecognized format type";_tag_fat(_tmp467,sizeof(char),25U);});Cyc_Warn_err(_tmp8FD,_tmp8FC,_tag_fat(_tmp466,sizeof(void*),0U));});});}}}else{
# 1627
({void*_tmp468=0U;({unsigned _tmp8FF=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8FE=({const char*_tmp469="unrecognized attribute";_tag_fat(_tmp469,sizeof(char),23U);});Cyc_Warn_err(_tmp8FF,_tmp8FE,_tag_fat(_tmp468,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1630
goto _LL0;}case 83U: _LLA1: _LLA2:
# 1639 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84U: _LLA3: _LLA4:
# 1641 "parse.y"
 yyval=Cyc_YY21(({void*_tmp901=({struct _tuple0*_tmp900=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp900,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp901,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85U: _LLA5: _LLA6:
# 1645 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1646 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1647 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1648 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1649 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1650 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1651 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1652 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1653 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1654 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95U: _LLB9: _LLBA:
# 1655 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1658
 yyval=Cyc_YY21(({void*_tmp902=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp902,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97U: _LLBD: _LLBE:
# 1660 "parse.y"
 yyval=Cyc_YY21(({void*_tmp903=Cyc_Absyn_builtin_type(({const char*_tmp46A="__builtin_va_list";_tag_fat(_tmp46A,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp903,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1662 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99U: _LLC1: _LLC2:
# 1664 "parse.y"
 yyval=Cyc_YY21(({void*_tmp904=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp904,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100U: _LLC3: _LLC4:
# 1666 "parse.y"
 yyval=Cyc_YY21(({void*_tmp905=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp905,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1668 "parse.y"
 yyval=Cyc_YY21(({void*_tmp906=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp906,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1670 "parse.y"
 yyval=Cyc_YY21(({void*_tmp909=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B->tag=6U,({struct Cyc_List_List*_tmp908=({unsigned _tmp907=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp907,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});
# 1670
_tmp46B->f1=_tmp908;});_tmp46B;});Cyc_Parse_type_spec(_tmp909,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1673
goto _LL0;case 103U: _LLC9: _LLCA:
# 1674 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90A=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp90A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104U: _LLCB: _LLCC:
# 1676 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90B=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp90B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1678
goto _LL0;case 105U: _LLCD: _LLCE:
# 1679 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90C=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp90C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106U: _LLCF: _LLD0:
# 1681 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90D=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp90D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1683 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90E=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp90E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1689 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp90F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id_to_kind(_tmp90F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6: {
# 1693 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp73D;_tmp73D.print_const=1,_tmp73D.q_volatile=0,_tmp73D.q_restrict=0,_tmp73D.real_const=1,_tmp73D.loc=loc;_tmp73D;}));
goto _LL0;}case 110U: _LLD7: _LLD8:
# 1695 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp73E;_tmp73E.print_const=0,_tmp73E.q_volatile=1,_tmp73E.q_restrict=0,_tmp73E.real_const=0,_tmp73E.loc=0U;_tmp73E;}));
goto _LL0;case 111U: _LLD9: _LLDA:
# 1696 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp73F;_tmp73F.print_const=0,_tmp73F.q_volatile=0,_tmp73F.q_restrict=1,_tmp73F.real_const=0,_tmp73F.loc=0U;_tmp73F;}));
goto _LL0;case 112U: _LLDB: _LLDC: {
# 1702 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp46C=({struct Cyc_Absyn_TypeDecl*_tmp471=_cycalloc(sizeof(*_tmp471));({void*_tmp914=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp913=({struct Cyc_Absyn_Enumdecl*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp912=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp46F->name=_tmp912;}),({struct Cyc_Core_Opt*_tmp911=({struct Cyc_Core_Opt*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct Cyc_List_List*_tmp910=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp46E->v=_tmp910;});_tmp46E;});_tmp46F->fields=_tmp911;});_tmp46F;});_tmp470->f1=_tmp913;});_tmp470;});_tmp471->r=_tmp914;}),_tmp471->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp471;});struct Cyc_Absyn_TypeDecl*ed=_tmp46C;
# 1704
yyval=Cyc_YY21(({void*_tmp915=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=10U,_tmp46D->f1=ed,_tmp46D->f2=0;_tmp46D;});Cyc_Parse_type_spec(_tmp915,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1706
goto _LL0;}case 113U: _LLDD: _LLDE:
# 1707 "parse.y"
 yyval=Cyc_YY21(({void*_tmp916=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp916,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114U: _LLDF: _LLE0:
# 1709 "parse.y"
 yyval=Cyc_YY21(({void*_tmp917=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp917,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1715 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp472=_cycalloc(sizeof(*_tmp472));({struct _tuple0*_tmp918=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp472->name=_tmp918;}),_tmp472->tag=0,_tmp472->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp472;}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1717 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp473=_cycalloc(sizeof(*_tmp473));({struct _tuple0*_tmp91A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp473->name=_tmp91A;}),({struct Cyc_Absyn_Exp*_tmp919=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp473->tag=_tmp919;}),_tmp473->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp473;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1721 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*_tmp474));({struct Cyc_Absyn_Enumfield*_tmp91B=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp474->hd=_tmp91B;}),_tmp474->tl=0;_tmp474;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1722 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp475=_cycalloc(sizeof(*_tmp475));({struct Cyc_Absyn_Enumfield*_tmp91C=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp475->hd=_tmp91C;}),_tmp475->tl=0;_tmp475;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1723 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp476=_cycalloc(sizeof(*_tmp476));({struct Cyc_Absyn_Enumfield*_tmp91E=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp476->hd=_tmp91E;}),({struct Cyc_List_List*_tmp91D=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp476->tl=_tmp91D;});_tmp476;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1729 "parse.y"
 yyval=Cyc_YY21(({void*_tmp921=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=7U,({enum Cyc_Absyn_AggrKind _tmp920=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp477->f1=_tmp920;}),({struct Cyc_List_List*_tmp91F=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp477->f2=_tmp91F;});_tmp477;});Cyc_Parse_type_spec(_tmp921,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121U: _LLED: _LLEE: {
# 1735 "parse.y"
struct Cyc_List_List*_tmp478=({unsigned _tmp922=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp922,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp478;
struct Cyc_List_List*_tmp479=({unsigned _tmp923=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp923,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp479;
struct Cyc_Absyn_TypeDecl*_tmp47A=({enum Cyc_Absyn_AggrKind _tmp929=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp928=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp927=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp926=({
struct Cyc_List_List*_tmp925=exist_ts;struct Cyc_List_List*_tmp924=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp925,_tmp924,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1737
Cyc_Absyn_aggr_tdecl(_tmp929,Cyc_Absyn_Public,_tmp928,_tmp927,_tmp926,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp47A;
# 1740
yyval=Cyc_YY21(({void*_tmp92A=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->tag=10U,_tmp47B->f1=td,_tmp47B->f2=0;_tmp47B;});Cyc_Parse_type_spec(_tmp92A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1742
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1746 "parse.y"
struct Cyc_List_List*_tmp47C=({unsigned _tmp92B=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp92B,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp47C;
struct Cyc_List_List*_tmp47D=({unsigned _tmp92C=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp92C,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp47D;
struct Cyc_Absyn_TypeDecl*_tmp47E=({enum Cyc_Absyn_AggrKind _tmp932=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp931=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp930=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp92F=({
struct Cyc_List_List*_tmp92E=exist_ts;struct Cyc_List_List*_tmp92D=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp92E,_tmp92D,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1748
Cyc_Absyn_aggr_tdecl(_tmp932,Cyc_Absyn_Public,_tmp931,_tmp930,_tmp92F,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp47E;
# 1751
yyval=Cyc_YY21(({void*_tmp933=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->tag=10U,_tmp47F->f1=td,_tmp47F->f2=0;_tmp47F;});Cyc_Parse_type_spec(_tmp933,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1753
goto _LL0;}case 123U: _LLF1: _LLF2:
# 1754 "parse.y"
 yyval=Cyc_YY21(({void*_tmp937=({union Cyc_Absyn_AggrInfo _tmp936=({enum Cyc_Absyn_AggrKind _tmp935=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp934=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp935,_tmp934,({struct Cyc_Core_Opt*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->v=(void*)1;_tmp480;}));});Cyc_Absyn_aggr_type(_tmp936,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp937,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1757
goto _LL0;case 124U: _LLF3: _LLF4:
# 1758 "parse.y"
 yyval=Cyc_YY21(({void*_tmp93A=({union Cyc_Absyn_AggrInfo _tmp939=({enum Cyc_Absyn_AggrKind _tmp938=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp938,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp939,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp93A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 125U: _LLF5: _LLF6:
# 1763 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 126U: _LLF7: _LLF8:
# 1765 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 127U: _LLF9: _LLFA:
# 1769 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 128U: _LLFB: _LLFC:
# 1770 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1775 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 130U: _LLFF: _LL100: {
# 1777 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp481=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp481;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1781
struct Cyc_List_List*_tmp482=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp482;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1786
goto _LL0;}}case 131U: _LL101: _LL102:
# 1791 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp483=_cycalloc(sizeof(*_tmp483));({struct Cyc_List_List*_tmp93B=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp483->hd=_tmp93B;}),_tmp483->tl=0;_tmp483;}));
goto _LL0;case 132U: _LL103: _LL104:
# 1793 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp484=_cycalloc(sizeof(*_tmp484));({struct Cyc_List_List*_tmp93D=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp484->hd=_tmp93D;}),({struct Cyc_List_List*_tmp93C=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp484->tl=_tmp93C;});_tmp484;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1797 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 134U: _LL107: _LL108:
# 1803 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp485=_region_malloc(yyr,sizeof(*_tmp485));_tmp485->tl=0,({struct _tuple12 _tmp93E=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp485->hd=_tmp93E;});_tmp485;}));
goto _LL0;case 135U: _LL109: _LL10A:
# 1805 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp486=_region_malloc(yyr,sizeof(*_tmp486));({struct _tuple13*_tmp940=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp486->tl=_tmp940;}),({struct _tuple12 _tmp93F=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp486->hd=_tmp93F;});_tmp486;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1810 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp740;({struct Cyc_Parse_Declarator _tmp941=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp740.f1=_tmp941;}),_tmp740.f2=0;_tmp740;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1812 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp741;({struct Cyc_Parse_Declarator _tmp943=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp741.f1=_tmp943;}),({struct Cyc_Absyn_Exp*_tmp942=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp741.f2=_tmp942;});_tmp741;}));
goto _LL0;case 138U: _LL10F: _LL110: {
# 1818 "parse.y"
struct _RegionHandle _tmp487=_new_region("temp");struct _RegionHandle*temp=& _tmp487;_push_region(temp);
{struct _tuple26 _tmp488=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp19=_tmp488;struct _tuple26 _tmp489=_stmttmp19;struct Cyc_List_List*_tmp48C;struct Cyc_Parse_Type_specifier _tmp48B;struct Cyc_Absyn_Tqual _tmp48A;_LL465: _tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;_tmp48C=_tmp489.f3;_LL466: {struct Cyc_Absyn_Tqual tq=_tmp48A;struct Cyc_Parse_Type_specifier tspecs=_tmp48B;struct Cyc_List_List*atts=_tmp48C;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp48D=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp48D;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp48E=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp1A=_tmp48E;struct _tuple25*_tmp48F=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Parse_Declarator _tmp490;_LL468: _tmp490=_tmp48F->f1;_tmp491=_tmp48F->f2;_tmp492=_tmp48F->f3;_LL469: {struct Cyc_Parse_Declarator d=_tmp490;struct Cyc_Absyn_Exp*wd=_tmp491;struct Cyc_Absyn_Exp*wh=_tmp492;
decls=({struct _tuple11*_tmp493=_region_malloc(temp,sizeof(*_tmp493));_tmp493->tl=decls,_tmp493->hd=d;_tmp493;});
widths_and_reqs=({struct Cyc_List_List*_tmp495=_region_malloc(temp,sizeof(*_tmp495));
({struct _tuple17*_tmp944=({struct _tuple17*_tmp494=_region_malloc(temp,sizeof(*_tmp494));_tmp494->f1=wd,_tmp494->f2=wh;_tmp494;});_tmp495->hd=_tmp944;}),_tmp495->tl=widths_and_reqs;_tmp495;});}}}
# 1829
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp496=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp496;
struct Cyc_List_List*_tmp497=({struct _RegionHandle*_tmp947=temp;struct _RegionHandle*_tmp946=temp;struct Cyc_List_List*_tmp945=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1832
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp947,_tmp946,_tmp945,widths_and_reqs);});struct Cyc_List_List*info=_tmp497;
# 1835
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1837
_npop_handler(0U);goto _LL0;}}}}
# 1819
;_pop_region();}case 139U: _LL111: _LL112:
# 1845 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp742;({struct Cyc_Absyn_Tqual _tmp949=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp742.f1=_tmp949;}),({struct Cyc_Parse_Type_specifier _tmp948=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp742.f2=_tmp948;}),_tmp742.f3=0;_tmp742;}));
goto _LL0;case 140U: _LL113: _LL114: {
# 1847 "parse.y"
struct _tuple26 _tmp498=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp498;yyval=Cyc_YY35(({struct _tuple26 _tmp743;_tmp743.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp94C=({unsigned _tmp94B=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp94A=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp94B,_tmp94A,two.f2);});_tmp743.f2=_tmp94C;}),_tmp743.f3=two.f3;_tmp743;}));
goto _LL0;}case 141U: _LL115: _LL116:
# 1849 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp744;({struct Cyc_Absyn_Tqual _tmp94E=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp744.f1=_tmp94E;}),({struct Cyc_Parse_Type_specifier _tmp94D=Cyc_Parse_empty_spec(0U);_tmp744.f2=_tmp94D;}),_tmp744.f3=0;_tmp744;}));
goto _LL0;case 142U: _LL117: _LL118: {
# 1851 "parse.y"
struct _tuple26 _tmp499=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp499;
yyval=Cyc_YY35(({struct _tuple26 _tmp745;({struct Cyc_Absyn_Tqual _tmp950=({struct Cyc_Absyn_Tqual _tmp94F=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp94F,two.f1);});_tmp745.f1=_tmp950;}),_tmp745.f2=two.f2,_tmp745.f3=two.f3;_tmp745;}));
goto _LL0;}case 143U: _LL119: _LL11A:
# 1854 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp746;({struct Cyc_Absyn_Tqual _tmp953=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp746.f1=_tmp953;}),({struct Cyc_Parse_Type_specifier _tmp952=Cyc_Parse_empty_spec(0U);_tmp746.f2=_tmp952;}),({struct Cyc_List_List*_tmp951=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp746.f3=_tmp951;});_tmp746;}));
goto _LL0;case 144U: _LL11B: _LL11C: {
# 1856 "parse.y"
struct _tuple26 _tmp49A=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp49A;yyval=Cyc_YY35(({struct _tuple26 _tmp747;_tmp747.f1=two.f1,_tmp747.f2=two.f2,({struct Cyc_List_List*_tmp955=({struct Cyc_List_List*_tmp954=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp954,two.f3);});_tmp747.f3=_tmp955;});_tmp747;}));
goto _LL0;}case 145U: _LL11D: _LL11E:
# 1862 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp748;({struct Cyc_Absyn_Tqual _tmp957=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp748.f1=_tmp957;}),({struct Cyc_Parse_Type_specifier _tmp956=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp748.f2=_tmp956;}),_tmp748.f3=0;_tmp748;}));
goto _LL0;case 146U: _LL11F: _LL120: {
# 1864 "parse.y"
struct _tuple26 _tmp49B=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp49B;yyval=Cyc_YY35(({struct _tuple26 _tmp749;_tmp749.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp95A=({unsigned _tmp959=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp958=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp959,_tmp958,two.f2);});_tmp749.f2=_tmp95A;}),_tmp749.f3=two.f3;_tmp749;}));
goto _LL0;}case 147U: _LL121: _LL122:
# 1866 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp74A;({struct Cyc_Absyn_Tqual _tmp95C=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp74A.f1=_tmp95C;}),({struct Cyc_Parse_Type_specifier _tmp95B=Cyc_Parse_empty_spec(0U);_tmp74A.f2=_tmp95B;}),_tmp74A.f3=0;_tmp74A;}));
goto _LL0;case 148U: _LL123: _LL124: {
# 1868 "parse.y"
struct _tuple26 _tmp49C=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp49C;
yyval=Cyc_YY35(({struct _tuple26 _tmp74B;({struct Cyc_Absyn_Tqual _tmp95E=({struct Cyc_Absyn_Tqual _tmp95D=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp95D,two.f1);});_tmp74B.f1=_tmp95E;}),_tmp74B.f2=two.f2,_tmp74B.f3=two.f3;_tmp74B;}));
goto _LL0;}case 149U: _LL125: _LL126:
# 1871 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp74C;({struct Cyc_Absyn_Tqual _tmp961=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp74C.f1=_tmp961;}),({struct Cyc_Parse_Type_specifier _tmp960=Cyc_Parse_empty_spec(0U);_tmp74C.f2=_tmp960;}),({struct Cyc_List_List*_tmp95F=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp74C.f3=_tmp95F;});_tmp74C;}));
goto _LL0;case 150U: _LL127: _LL128: {
# 1873 "parse.y"
struct _tuple26 _tmp49D=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp49D;yyval=Cyc_YY35(({struct _tuple26 _tmp74D;_tmp74D.f1=two.f1,_tmp74D.f2=two.f2,({struct Cyc_List_List*_tmp963=({struct Cyc_List_List*_tmp962=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp962,two.f3);});_tmp74D.f3=_tmp963;});_tmp74D;}));
goto _LL0;}case 151U: _LL129: _LL12A:
# 1877 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 152U: _LL12B: _LL12C:
# 1883 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));({struct _tuple25*_tmp964=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp49E->hd=_tmp964;}),_tmp49E->tl=0;_tmp49E;}));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1885 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp49F=_region_malloc(yyr,sizeof(*_tmp49F));({struct _tuple25*_tmp966=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp49F->hd=_tmp966;}),({struct Cyc_List_List*_tmp965=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp49F->tl=_tmp965;});_tmp49F;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1890 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A0=_region_malloc(yyr,sizeof(*_tmp4A0));({struct Cyc_Parse_Declarator _tmp968=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp4A0->f1=_tmp968;}),_tmp4A0->f2=0,({struct Cyc_Absyn_Exp*_tmp967=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp4A0->f3=_tmp967;});_tmp4A0;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1894 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A4=_region_malloc(yyr,sizeof(*_tmp4A4));({struct _tuple0*_tmp96D=({struct _tuple0*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));({union Cyc_Absyn_Nmspace _tmp96C=Cyc_Absyn_Rel_n(0);_tmp4A3->f1=_tmp96C;}),({struct _fat_ptr*_tmp96B=({struct _fat_ptr*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));({struct _fat_ptr _tmp96A=({const char*_tmp4A1="";_tag_fat(_tmp4A1,sizeof(char),1U);});*_tmp4A2=_tmp96A;});_tmp4A2;});_tmp4A3->f2=_tmp96B;});_tmp4A3;});(_tmp4A4->f1).id=_tmp96D;}),(_tmp4A4->f1).varloc=0U,(_tmp4A4->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp969=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp4A4->f2=_tmp969;}),_tmp4A4->f3=0;_tmp4A4;}));
# 1896
goto _LL0;case 156U: _LL133: _LL134:
# 1899 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A8=_region_malloc(yyr,sizeof(*_tmp4A8));({struct _tuple0*_tmp971=({struct _tuple0*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));({union Cyc_Absyn_Nmspace _tmp970=Cyc_Absyn_Rel_n(0);_tmp4A7->f1=_tmp970;}),({struct _fat_ptr*_tmp96F=({struct _fat_ptr*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));({struct _fat_ptr _tmp96E=({const char*_tmp4A5="";_tag_fat(_tmp4A5,sizeof(char),1U);});*_tmp4A6=_tmp96E;});_tmp4A6;});_tmp4A7->f2=_tmp96F;});_tmp4A7;});(_tmp4A8->f1).id=_tmp971;}),(_tmp4A8->f1).varloc=0U,(_tmp4A8->f1).tms=0,_tmp4A8->f2=0,_tmp4A8->f3=0;_tmp4A8;}));
# 1901
goto _LL0;case 157U: _LL135: _LL136:
# 1902 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A9=_region_malloc(yyr,sizeof(*_tmp4A9));({struct Cyc_Parse_Declarator _tmp973=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp4A9->f1=_tmp973;}),({struct Cyc_Absyn_Exp*_tmp972=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4A9->f2=_tmp972;}),_tmp4A9->f3=0;_tmp4A9;}));
goto _LL0;case 158U: _LL137: _LL138:
# 1906 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 159U: _LL139: _LL13A:
# 1907 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 160U: _LL13B: _LL13C:
# 1911 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 161U: _LL13D: _LL13E:
# 1912 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 162U: _LL13F: _LL140: {
# 1918 "parse.y"
int _tmp4AA=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4AA;
struct Cyc_List_List*_tmp4AB=({unsigned _tmp974=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp974,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp4AB;
struct Cyc_Absyn_TypeDecl*_tmp4AC=({struct _tuple0*_tmp979=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp978=ts;struct Cyc_Core_Opt*_tmp977=({struct Cyc_Core_Opt*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));({struct Cyc_List_List*_tmp975=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp4AE->v=_tmp975;});_tmp4AE;});int _tmp976=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp979,_tmp978,_tmp977,_tmp976,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp4AC;
# 1922
yyval=Cyc_YY21(({void*_tmp97A=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->tag=10U,_tmp4AD->f1=dd,_tmp4AD->f2=0;_tmp4AD;});Cyc_Parse_type_spec(_tmp97A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1924
goto _LL0;}case 163U: _LL141: _LL142: {
# 1925 "parse.y"
int _tmp4AF=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4AF;
yyval=Cyc_YY21(({void*_tmp97D=({union Cyc_Absyn_DatatypeInfo _tmp97C=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp74E;({struct _tuple0*_tmp97B=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp74E.name=_tmp97B;}),_tmp74E.is_extensible=is_extensible;_tmp74E;}));Cyc_Absyn_datatype_type(_tmp97C,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp97D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1928
goto _LL0;}case 164U: _LL143: _LL144: {
# 1929 "parse.y"
int _tmp4B0=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4B0;
yyval=Cyc_YY21(({void*_tmp981=({union Cyc_Absyn_DatatypeFieldInfo _tmp980=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp74F;({struct _tuple0*_tmp97F=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp74F.datatype_name=_tmp97F;}),({struct _tuple0*_tmp97E=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp74F.field_name=_tmp97E;}),_tmp74F.is_extensible=is_extensible;_tmp74F;}));Cyc_Absyn_datatype_field_type(_tmp980,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp981,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1932
goto _LL0;}case 165U: _LL145: _LL146:
# 1935 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 166U: _LL147: _LL148:
# 1936 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 167U: _LL149: _LL14A:
# 1940 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct Cyc_Absyn_Datatypefield*_tmp982=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4B1->hd=_tmp982;}),_tmp4B1->tl=0;_tmp4B1;}));
goto _LL0;case 168U: _LL14B: _LL14C:
# 1941 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({struct Cyc_Absyn_Datatypefield*_tmp983=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4B2->hd=_tmp983;}),_tmp4B2->tl=0;_tmp4B2;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1942 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));({struct Cyc_Absyn_Datatypefield*_tmp985=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4B3->hd=_tmp985;}),({struct Cyc_List_List*_tmp984=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4B3->tl=_tmp984;});_tmp4B3;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1943 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));({struct Cyc_Absyn_Datatypefield*_tmp987=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4B4->hd=_tmp987;}),({struct Cyc_List_List*_tmp986=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4B4->tl=_tmp986;});_tmp4B4;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1947 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 172U: _LL153: _LL154:
# 1948 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 173U: _LL155: _LL156:
# 1949 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 174U: _LL157: _LL158:
# 1953 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));({struct _tuple0*_tmp989=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4B5->name=_tmp989;}),_tmp4B5->typs=0,_tmp4B5->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp988=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4B5->sc=_tmp988;});_tmp4B5;}));
goto _LL0;case 175U: _LL159: _LL15A: {
# 1955 "parse.y"
struct Cyc_List_List*_tmp4B6=({unsigned _tmp98A=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp98A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4B6;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({struct _tuple0*_tmp98C=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4B7->name=_tmp98C;}),_tmp4B7->typs=typs,_tmp4B7->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp98B=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4B7->sc=_tmp98B;});_tmp4B7;}));
goto _LL0;}case 176U: _LL15B: _LL15C:
# 1961 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 177U: _LL15D: _LL15E: {
# 1963 "parse.y"
struct Cyc_Parse_Declarator _tmp4B8=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B8;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp750;_tmp750.id=two.id,_tmp750.varloc=two.varloc,({struct Cyc_List_List*_tmp98E=({struct Cyc_List_List*_tmp98D=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp98D,two.tms);});_tmp750.tms=_tmp98E;});_tmp750;}));
goto _LL0;}case 178U: _LL15F: _LL160:
# 1970 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 179U: _LL161: _LL162: {
# 1972 "parse.y"
struct Cyc_Parse_Declarator _tmp4B9=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B9;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp751;_tmp751.id=two.id,_tmp751.varloc=two.varloc,({struct Cyc_List_List*_tmp990=({struct Cyc_List_List*_tmp98F=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp98F,two.tms);});_tmp751.tms=_tmp990;});_tmp751;}));
goto _LL0;}case 180U: _LL163: _LL164:
# 1978 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp752;({struct _tuple0*_tmp991=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp752.id=_tmp991;}),_tmp752.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp752.tms=0;_tmp752;}));
goto _LL0;case 181U: _LL165: _LL166:
# 1980 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 182U: _LL167: _LL168: {
# 1984 "parse.y"
struct Cyc_Parse_Declarator _tmp4BA=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4BA;
({struct Cyc_List_List*_tmp994=({struct Cyc_List_List*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));({void*_tmp993=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4BB=_region_malloc(yyr,sizeof(*_tmp4BB));_tmp4BB->tag=5U,_tmp4BB->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp992=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4BB->f2=_tmp992;});_tmp4BB;});_tmp4BC->hd=_tmp993;}),_tmp4BC->tl=d.tms;_tmp4BC;});d.tms=_tmp994;});
yyval=(yyyvsp[2]).v;
# 1988
goto _LL0;}case 183U: _LL169: _LL16A:
# 1989 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp753;({struct _tuple0*_tmp99A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp753.id=_tmp99A;}),({unsigned _tmp999=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp753.varloc=_tmp999;}),({struct Cyc_List_List*_tmp998=({struct Cyc_List_List*_tmp4BE=_region_malloc(yyr,sizeof(*_tmp4BE));({void*_tmp997=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));_tmp4BD->tag=0U,({void*_tmp996=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4BD->f1=_tmp996;}),_tmp4BD->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4BD;});_tmp4BE->hd=_tmp997;}),({struct Cyc_List_List*_tmp995=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4BE->tl=_tmp995;});_tmp4BE;});_tmp753.tms=_tmp998;});_tmp753;}));
goto _LL0;case 184U: _LL16B: _LL16C:
# 1991 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp754;({struct _tuple0*_tmp9A1=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp754.id=_tmp9A1;}),({unsigned _tmp9A0=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp754.varloc=_tmp9A0;}),({
struct Cyc_List_List*_tmp99F=({struct Cyc_List_List*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));({void*_tmp99E=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4BF=_region_malloc(yyr,sizeof(*_tmp4BF));_tmp4BF->tag=1U,({struct Cyc_Absyn_Exp*_tmp99D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4BF->f1=_tmp99D;}),({void*_tmp99C=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4BF->f2=_tmp99C;}),_tmp4BF->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4BF;});_tmp4C0->hd=_tmp99E;}),({struct Cyc_List_List*_tmp99B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C0->tl=_tmp99B;});_tmp4C0;});_tmp754.tms=_tmp99F;});_tmp754;}));
goto _LL0;case 185U: _LL16D: _LL16E: {
# 1994 "parse.y"
struct _tuple27*_tmp4C1=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp1B=_tmp4C1;struct _tuple27*_tmp4C2=_stmttmp1B;struct Cyc_List_List*_tmp4C7;void*_tmp4C6;struct Cyc_Absyn_VarargInfo*_tmp4C5;int _tmp4C4;struct Cyc_List_List*_tmp4C3;_LL46B: _tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C2->f2;_tmp4C5=_tmp4C2->f3;_tmp4C6=_tmp4C2->f4;_tmp4C7=_tmp4C2->f5;_LL46C: {struct Cyc_List_List*lis=_tmp4C3;int b=_tmp4C4;struct Cyc_Absyn_VarargInfo*c=_tmp4C5;void*eff=_tmp4C6;struct Cyc_List_List*po=_tmp4C7;
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4C8;
struct Cyc_Absyn_Exp*_tmp4C9=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4C9;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp755;({struct _tuple0*_tmp9A7=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp755.id=_tmp9A7;}),({unsigned _tmp9A6=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp755.varloc=_tmp9A6;}),({struct Cyc_List_List*_tmp9A5=({struct Cyc_List_List*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));({void*_tmp9A4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));_tmp4CB->tag=3U,({void*_tmp9A3=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=1U,_tmp4CA->f1=lis,_tmp4CA->f2=b,_tmp4CA->f3=c,_tmp4CA->f4=eff,_tmp4CA->f5=po,_tmp4CA->f6=req,_tmp4CA->f7=ens;_tmp4CA;});_tmp4CB->f1=_tmp9A3;});_tmp4CB;});_tmp4CC->hd=_tmp9A4;}),({struct Cyc_List_List*_tmp9A2=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CC->tl=_tmp9A2;});_tmp4CC;});_tmp755.tms=_tmp9A5;});_tmp755;}));
# 1999
goto _LL0;}}case 186U: _LL16F: _LL170:
# 2000 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp756;({struct _tuple0*_tmp9B1=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp756.id=_tmp9B1;}),({unsigned _tmp9B0=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp756.varloc=_tmp9B0;}),({
struct Cyc_List_List*_tmp9AF=({struct Cyc_List_List*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));({void*_tmp9AE=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));_tmp4CE->tag=3U,({void*_tmp9AD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));_tmp4CD->tag=1U,_tmp4CD->f1=0,_tmp4CD->f2=0,_tmp4CD->f3=0,({
# 2003
void*_tmp9AC=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4CD->f4=_tmp9AC;}),({struct Cyc_List_List*_tmp9AB=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4CD->f5=_tmp9AB;}),({struct Cyc_Absyn_Exp*_tmp9AA=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4CD->f6=_tmp9AA;}),({struct Cyc_Absyn_Exp*_tmp9A9=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4CD->f7=_tmp9A9;});_tmp4CD;});
# 2001
_tmp4CE->f1=_tmp9AD;});_tmp4CE;});_tmp4CF->hd=_tmp9AE;}),({
# 2004
struct Cyc_List_List*_tmp9A8=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CF->tl=_tmp9A8;});_tmp4CF;});
# 2001
_tmp756.tms=_tmp9AF;});_tmp756;}));
# 2006
goto _LL0;case 187U: _LL171: _LL172:
# 2007 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp757;({struct _tuple0*_tmp9B8=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp757.id=_tmp9B8;}),({unsigned _tmp9B7=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp757.varloc=_tmp9B7;}),({struct Cyc_List_List*_tmp9B6=({struct Cyc_List_List*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));({void*_tmp9B5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));_tmp4D1->tag=3U,({void*_tmp9B4=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));_tmp4D0->tag=0U,({struct Cyc_List_List*_tmp9B3=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4D0->f1=_tmp9B3;}),_tmp4D0->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4D0;});_tmp4D1->f1=_tmp9B4;});_tmp4D1;});_tmp4D2->hd=_tmp9B5;}),({struct Cyc_List_List*_tmp9B2=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4D2->tl=_tmp9B2;});_tmp4D2;});_tmp757.tms=_tmp9B6;});_tmp757;}));
goto _LL0;case 188U: _LL173: _LL174: {
# 2010
struct Cyc_List_List*_tmp4D3=({unsigned _tmp9B9=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9B9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4D3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp758;({struct _tuple0*_tmp9BE=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp758.id=_tmp9BE;}),({unsigned _tmp9BD=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp758.varloc=_tmp9BD;}),({struct Cyc_List_List*_tmp9BC=({struct Cyc_List_List*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));({void*_tmp9BB=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));_tmp4D4->tag=4U,_tmp4D4->f1=ts,_tmp4D4->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4D4->f3=0;_tmp4D4;});_tmp4D5->hd=_tmp9BB;}),({struct Cyc_List_List*_tmp9BA=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4D5->tl=_tmp9BA;});_tmp4D5;});_tmp758.tms=_tmp9BC;});_tmp758;}));
# 2013
goto _LL0;}case 189U: _LL175: _LL176:
# 2014 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp759;({struct _tuple0*_tmp9C4=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp759.id=_tmp9C4;}),({unsigned _tmp9C3=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp759.varloc=_tmp9C3;}),({struct Cyc_List_List*_tmp9C2=({struct Cyc_List_List*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));({void*_tmp9C1=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));_tmp4D6->tag=5U,_tmp4D6->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9C0=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D6->f2=_tmp9C0;});_tmp4D6;});_tmp4D7->hd=_tmp9C1;}),({
struct Cyc_List_List*_tmp9BF=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4D7->tl=_tmp9BF;});_tmp4D7;});
# 2014
_tmp759.tms=_tmp9C2;});_tmp759;}));
# 2017
goto _LL0;case 190U: _LL177: _LL178:
# 2022 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp75A;({struct _tuple0*_tmp9C5=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp75A.id=_tmp9C5;}),_tmp75A.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp75A.tms=0;_tmp75A;}));
goto _LL0;case 191U: _LL179: _LL17A:
# 2024 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp75B;({struct _tuple0*_tmp9C6=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp75B.id=_tmp9C6;}),_tmp75B.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp75B.tms=0;_tmp75B;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 2026 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 193U: _LL17D: _LL17E: {
# 2030 "parse.y"
struct Cyc_Parse_Declarator _tmp4D8=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4D8;
({struct Cyc_List_List*_tmp9C9=({struct Cyc_List_List*_tmp4DA=_region_malloc(yyr,sizeof(*_tmp4DA));({void*_tmp9C8=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));_tmp4D9->tag=5U,_tmp4D9->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9C7=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D9->f2=_tmp9C7;});_tmp4D9;});_tmp4DA->hd=_tmp9C8;}),_tmp4DA->tl=d.tms;_tmp4DA;});d.tms=_tmp9C9;});
yyval=(yyyvsp[2]).v;
# 2034
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2035 "parse.y"
struct Cyc_Parse_Declarator _tmp4DB=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4DB;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp75C;_tmp75C.id=one.id,_tmp75C.varloc=one.varloc,({
struct Cyc_List_List*_tmp9CC=({struct Cyc_List_List*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));({void*_tmp9CB=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));_tmp4DC->tag=0U,({void*_tmp9CA=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4DC->f1=_tmp9CA;}),_tmp4DC->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4DC;});_tmp4DD->hd=_tmp9CB;}),_tmp4DD->tl=one.tms;_tmp4DD;});_tmp75C.tms=_tmp9CC;});_tmp75C;}));
goto _LL0;}case 195U: _LL181: _LL182: {
# 2039 "parse.y"
struct Cyc_Parse_Declarator _tmp4DE=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4DE;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp75D;_tmp75D.id=one.id,_tmp75D.varloc=one.varloc,({
struct Cyc_List_List*_tmp9D0=({struct Cyc_List_List*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));({void*_tmp9CF=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4DF=_region_malloc(yyr,sizeof(*_tmp4DF));_tmp4DF->tag=1U,({struct Cyc_Absyn_Exp*_tmp9CE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4DF->f1=_tmp9CE;}),({void*_tmp9CD=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4DF->f2=_tmp9CD;}),_tmp4DF->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4DF;});_tmp4E0->hd=_tmp9CF;}),_tmp4E0->tl=one.tms;_tmp4E0;});_tmp75D.tms=_tmp9D0;});_tmp75D;}));
# 2043
goto _LL0;}case 196U: _LL183: _LL184: {
# 2044 "parse.y"
struct _tuple27*_tmp4E1=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp1C=_tmp4E1;struct _tuple27*_tmp4E2=_stmttmp1C;struct Cyc_List_List*_tmp4E7;void*_tmp4E6;struct Cyc_Absyn_VarargInfo*_tmp4E5;int _tmp4E4;struct Cyc_List_List*_tmp4E3;_LL46E: _tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;_tmp4E5=_tmp4E2->f3;_tmp4E6=_tmp4E2->f4;_tmp4E7=_tmp4E2->f5;_LL46F: {struct Cyc_List_List*lis=_tmp4E3;int b=_tmp4E4;struct Cyc_Absyn_VarargInfo*c=_tmp4E5;void*eff=_tmp4E6;struct Cyc_List_List*po=_tmp4E7;
struct Cyc_Absyn_Exp*_tmp4E8=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4E8;
struct Cyc_Absyn_Exp*_tmp4E9=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4E9;
struct Cyc_Parse_Declarator _tmp4EA=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EA;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp75E;_tmp75E.id=one.id,_tmp75E.varloc=one.varloc,({struct Cyc_List_List*_tmp9D3=({struct Cyc_List_List*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));({void*_tmp9D2=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EC=_region_malloc(yyr,sizeof(*_tmp4EC));_tmp4EC->tag=3U,({void*_tmp9D1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));_tmp4EB->tag=1U,_tmp4EB->f1=lis,_tmp4EB->f2=b,_tmp4EB->f3=c,_tmp4EB->f4=eff,_tmp4EB->f5=po,_tmp4EB->f6=req,_tmp4EB->f7=ens;_tmp4EB;});_tmp4EC->f1=_tmp9D1;});_tmp4EC;});_tmp4ED->hd=_tmp9D2;}),_tmp4ED->tl=one.tms;_tmp4ED;});_tmp75E.tms=_tmp9D3;});_tmp75E;}));
# 2050
goto _LL0;}}case 197U: _LL185: _LL186: {
# 2051 "parse.y"
struct Cyc_Parse_Declarator _tmp4EE=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EE;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp75F;_tmp75F.id=one.id,_tmp75F.varloc=one.varloc,({
struct Cyc_List_List*_tmp9DA=({struct Cyc_List_List*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));({void*_tmp9D9=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4F0=_region_malloc(yyr,sizeof(*_tmp4F0));_tmp4F0->tag=3U,({void*_tmp9D8=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));_tmp4EF->tag=1U,_tmp4EF->f1=0,_tmp4EF->f2=0,_tmp4EF->f3=0,({
# 2055
void*_tmp9D7=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4EF->f4=_tmp9D7;}),({struct Cyc_List_List*_tmp9D6=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4EF->f5=_tmp9D6;}),({struct Cyc_Absyn_Exp*_tmp9D5=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4EF->f6=_tmp9D5;}),({struct Cyc_Absyn_Exp*_tmp9D4=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4EF->f7=_tmp9D4;});_tmp4EF;});
# 2053
_tmp4F0->f1=_tmp9D8;});_tmp4F0;});_tmp4F1->hd=_tmp9D9;}),_tmp4F1->tl=one.tms;_tmp4F1;});_tmp75F.tms=_tmp9DA;});_tmp75F;}));
# 2058
goto _LL0;}case 198U: _LL187: _LL188: {
# 2059 "parse.y"
struct Cyc_Parse_Declarator _tmp4F2=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp760;_tmp760.id=one.id,_tmp760.varloc=one.varloc,({struct Cyc_List_List*_tmp9DE=({struct Cyc_List_List*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));({void*_tmp9DD=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));_tmp4F4->tag=3U,({void*_tmp9DC=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));_tmp4F3->tag=0U,({struct Cyc_List_List*_tmp9DB=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4F3->f1=_tmp9DB;}),_tmp4F3->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4F3;});_tmp4F4->f1=_tmp9DC;});_tmp4F4;});_tmp4F5->hd=_tmp9DD;}),_tmp4F5->tl=one.tms;_tmp4F5;});_tmp760.tms=_tmp9DE;});_tmp760;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2063
struct Cyc_List_List*_tmp4F6=({unsigned _tmp9DF=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9DF,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4F6;
struct Cyc_Parse_Declarator _tmp4F7=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F7;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp761;_tmp761.id=one.id,_tmp761.varloc=one.varloc,({struct Cyc_List_List*_tmp9E1=({struct Cyc_List_List*_tmp4F9=_region_malloc(yyr,sizeof(*_tmp4F9));({void*_tmp9E0=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F8=_region_malloc(yyr,sizeof(*_tmp4F8));_tmp4F8->tag=4U,_tmp4F8->f1=ts,_tmp4F8->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4F8->f3=0;_tmp4F8;});_tmp4F9->hd=_tmp9E0;}),_tmp4F9->tl=one.tms;_tmp4F9;});_tmp761.tms=_tmp9E1;});_tmp761;}));
# 2067
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2068 "parse.y"
struct Cyc_Parse_Declarator _tmp4FA=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4FA;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp762;_tmp762.id=one.id,_tmp762.varloc=one.varloc,({struct Cyc_List_List*_tmp9E4=({struct Cyc_List_List*_tmp4FC=_region_malloc(yyr,sizeof(*_tmp4FC));({void*_tmp9E3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4FB=_region_malloc(yyr,sizeof(*_tmp4FB));_tmp4FB->tag=5U,_tmp4FB->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4FB->f2=_tmp9E2;});_tmp4FB;});_tmp4FC->hd=_tmp9E3;}),_tmp4FC->tl=one.tms;_tmp4FC;});_tmp762.tms=_tmp9E4;});_tmp762;}));
# 2071
goto _LL0;}case 201U: _LL18D: _LL18E:
# 2075 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 202U: _LL18F: _LL190:
# 2076 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9E5=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9E5,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 203U: _LL191: _LL192: {
# 2080 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4FE=_region_malloc(yyr,sizeof(*_tmp4FE));({void*_tmp9E7=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4FD=_region_malloc(yyr,sizeof(*_tmp4FD));_tmp4FD->tag=5U,_tmp4FD->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9E6=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4FD->f2=_tmp9E6;});_tmp4FD;});_tmp4FE->hd=_tmp9E7;}),_tmp4FE->tl=ans;_tmp4FE;});{
# 2084
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4FF=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1D=_tmp4FF;struct _tuple22 _tmp500=_stmttmp1D;void*_tmp503;void*_tmp502;unsigned _tmp501;_LL471: _tmp501=_tmp500.f1;_tmp502=_tmp500.f2;_tmp503=_tmp500.f3;_LL472: {unsigned ploc=_tmp501;void*nullable=_tmp502;void*bound=_tmp503;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->ptr_loc=ploc,_tmp504->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp504->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp504;});{
struct _tuple15 _tmp505=({unsigned _tmp9EB=ploc;void*_tmp9EA=nullable;void*_tmp9E9=bound;void*_tmp9E8=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9EB,_tmp9EA,_tmp9E9,_tmp9E8,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1E=_tmp505;struct _tuple15 _tmp506=_stmttmp1E;void*_tmp50A;void*_tmp509;void*_tmp508;void*_tmp507;_LL474: _tmp507=_tmp506.f1;_tmp508=_tmp506.f2;_tmp509=_tmp506.f3;_tmp50A=_tmp506.f4;_LL475: {void*nullable=_tmp507;void*bound=_tmp508;void*zeroterm=_tmp509;void*rgn_opt=_tmp50A;
ans=({struct Cyc_List_List*_tmp50C=_region_malloc(yyr,sizeof(*_tmp50C));({void*_tmp9ED=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=2U,(_tmp50B->f1).rgn=rgn_opt,(_tmp50B->f1).nullable=nullable,(_tmp50B->f1).bounds=bound,(_tmp50B->f1).zero_term=zeroterm,(_tmp50B->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp9EC=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp50B->f2=_tmp9EC;});_tmp50B;});_tmp50C->hd=_tmp9ED;}),_tmp50C->tl=ans;_tmp50C;});
yyval=Cyc_YY26(ans);
# 2092
goto _LL0;}}}}}case 204U: _LL193: _LL194:
# 2094
 yyval=Cyc_YY56(0);
goto _LL0;case 205U: _LL195: _LL196:
# 2095 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp50D=_region_malloc(yyr,sizeof(*_tmp50D));({void*_tmp9EF=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp50D->hd=_tmp9EF;}),({struct Cyc_List_List*_tmp9EE=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp50D->tl=_tmp9EE;});_tmp50D;}));
goto _LL0;case 206U: _LL197: _LL198:
# 2100 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp50E=_region_malloc(yyr,sizeof(*_tmp50E));_tmp50E->tag=0U,({struct Cyc_Absyn_Exp*_tmp9F0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp50E->f1=_tmp9F0;});_tmp50E;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2102 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp50F=_region_malloc(yyr,sizeof(*_tmp50F));_tmp50F->tag=1U,({void*_tmp9F1=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp50F->f1=_tmp9F1;});_tmp50F;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2103 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp510=_region_malloc(yyr,sizeof(*_tmp510));_tmp510->tag=2U;_tmp510;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2104 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp511=_region_malloc(yyr,sizeof(*_tmp511));_tmp511->tag=3U;_tmp511;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2105 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp512=_region_malloc(yyr,sizeof(*_tmp512));_tmp512->tag=4U;_tmp512;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2106 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp513=_region_malloc(yyr,sizeof(*_tmp513));_tmp513->tag=5U;_tmp513;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2107 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp514=_region_malloc(yyr,sizeof(*_tmp514));_tmp514->tag=6U;_tmp514;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2108 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp515=_region_malloc(yyr,sizeof(*_tmp515));_tmp515->tag=7U;_tmp515;}));
goto _LL0;case 214U: _LL1A7: _LL1A8:
# 2114 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp516->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp516->f3=Cyc_Absyn_fat_bound_type:({void*_tmp9F2=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp516->f3=_tmp9F2;});_tmp516;}));
# 2116
goto _LL0;case 215U: _LL1A9: _LL1AA:
# 2116 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp517->f2=Cyc_Absyn_false_type,({void*_tmp9F3=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp517->f3=_tmp9F3;});_tmp517;}));
goto _LL0;case 216U: _LL1AB: _LL1AC:
# 2117 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp518->f2=Cyc_Absyn_true_type,_tmp518->f3=Cyc_Absyn_fat_bound_type;_tmp518;}));
goto _LL0;case 217U: _LL1AD: _LL1AE:
# 2120
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2121 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2124
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2125 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2126 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2131 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2132 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Tcutil_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2133 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2137 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2138 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9F4=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9F4,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2143 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp519=_cycalloc(sizeof(*_tmp519));({struct Cyc_List_List*_tmp9F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp519->f1=_tmp9F7;}),_tmp519->f2=0,_tmp519->f3=0,({void*_tmp9F6=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp519->f4=_tmp9F6;}),({struct Cyc_List_List*_tmp9F5=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp519->f5=_tmp9F5;});_tmp519;}));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2145 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp51A=_cycalloc(sizeof(*_tmp51A));({struct Cyc_List_List*_tmp9FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp51A->f1=_tmp9FA;}),_tmp51A->f2=1,_tmp51A->f3=0,({void*_tmp9F9=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp51A->f4=_tmp9F9;}),({struct Cyc_List_List*_tmp9F8=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp51A->f5=_tmp9F8;});_tmp51A;}));
goto _LL0;case 229U: _LL1C5: _LL1C6: {
# 2148
struct _tuple8*_tmp51B=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1F=_tmp51B;struct _tuple8*_tmp51C=_stmttmp1F;void*_tmp51F;struct Cyc_Absyn_Tqual _tmp51E;struct _fat_ptr*_tmp51D;_LL477: _tmp51D=_tmp51C->f1;_tmp51E=_tmp51C->f2;_tmp51F=_tmp51C->f3;_LL478: {struct _fat_ptr*n=_tmp51D;struct Cyc_Absyn_Tqual tq=_tmp51E;void*t=_tmp51F;
struct Cyc_Absyn_VarargInfo*_tmp520=({struct Cyc_Absyn_VarargInfo*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->name=n,_tmp522->tq=tq,_tmp522->type=t,({int _tmp9FB=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp522->inject=_tmp9FB;});_tmp522;});struct Cyc_Absyn_VarargInfo*v=_tmp520;
yyval=Cyc_YY39(({struct _tuple27*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->f1=0,_tmp521->f2=0,_tmp521->f3=v,({void*_tmp9FD=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp521->f4=_tmp9FD;}),({struct Cyc_List_List*_tmp9FC=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp521->f5=_tmp9FC;});_tmp521;}));
# 2152
goto _LL0;}}case 230U: _LL1C7: _LL1C8: {
# 2154
struct _tuple8*_tmp523=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp20=_tmp523;struct _tuple8*_tmp524=_stmttmp20;void*_tmp527;struct Cyc_Absyn_Tqual _tmp526;struct _fat_ptr*_tmp525;_LL47A: _tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;_LL47B: {struct _fat_ptr*n=_tmp525;struct Cyc_Absyn_Tqual tq=_tmp526;void*t=_tmp527;
struct Cyc_Absyn_VarargInfo*_tmp528=({struct Cyc_Absyn_VarargInfo*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->name=n,_tmp52A->tq=tq,_tmp52A->type=t,({int _tmp9FE=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp52A->inject=_tmp9FE;});_tmp52A;});struct Cyc_Absyn_VarargInfo*v=_tmp528;
yyval=Cyc_YY39(({struct _tuple27*_tmp529=_cycalloc(sizeof(*_tmp529));({struct Cyc_List_List*_tmpA01=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp529->f1=_tmpA01;}),_tmp529->f2=0,_tmp529->f3=v,({void*_tmpA00=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp529->f4=_tmpA00;}),({struct Cyc_List_List*_tmp9FF=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp529->f5=_tmp9FF;});_tmp529;}));
# 2158
goto _LL0;}}case 231U: _LL1C9: _LL1CA:
# 2162 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmpA02=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmpA02,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->tag=1U,_tmp52B->f1=0;_tmp52B;}));}));
goto _LL0;case 232U: _LL1CB: _LL1CC:
# 2163 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmpA03=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmpA03,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 233U: _LL1CD: _LL1CE:
# 2166
 yyval=Cyc_YY49(0);
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2167 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2171 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2172 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 237U: _LL1D5: _LL1D6: {
# 2180 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52C=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->tag=2U,_tmp530->f1=0,_tmp530->f2=& Cyc_Tcutil_trk;_tmp530;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp52C;
void*_tmp52D=({struct _fat_ptr _tmpA04=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpA04,(void*)kb);});void*t=_tmp52D;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*_tmp52F));({struct _tuple34*_tmpA06=({struct _tuple34*_tmp52E=_cycalloc(sizeof(*_tmp52E));({void*_tmpA05=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp52E->f1=_tmpA05;}),_tmp52E->f2=t;_tmp52E;});_tmp52F->hd=_tmpA06;}),_tmp52F->tl=0;_tmp52F;}));
# 2184
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2186 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp531=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->tag=2U,_tmp535->f1=0,_tmp535->f2=& Cyc_Tcutil_trk;_tmp535;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp531;
void*_tmp532=({struct _fat_ptr _tmpA07=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpA07,(void*)kb);});void*t=_tmp532;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp534=_cycalloc(sizeof(*_tmp534));({struct _tuple34*_tmpA0A=({struct _tuple34*_tmp533=_cycalloc(sizeof(*_tmp533));({void*_tmpA09=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp533->f1=_tmpA09;}),_tmp533->f2=t;_tmp533;});_tmp534->hd=_tmpA0A;}),({struct Cyc_List_List*_tmpA08=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp534->tl=_tmpA08;});_tmp534;}));
# 2190
goto _LL0;}case 239U: _LL1D9: _LL1DA:
# 2194 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 240U: _LL1DB: _LL1DC:
# 2196 "parse.y"
 if(({struct _fat_ptr _tmpA0B=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmpA0B,({const char*_tmp536="inject";_tag_fat(_tmp536,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp538=({struct Cyc_Warn_String_Warn_Warg_struct _tmp763;_tmp763.tag=0U,({struct _fat_ptr _tmpA0C=({const char*_tmp539="missing type in function declaration";_tag_fat(_tmp539,sizeof(char),37U);});_tmp763.f1=_tmpA0C;});_tmp763;});void*_tmp537[1U];_tmp537[0]=& _tmp538;({unsigned _tmpA0D=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA0D,_tag_fat(_tmp537,sizeof(void*),1U));});});
yyval=Cyc_YY31(1);
# 2200
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2203 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2204 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmpA0E=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA0E,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2208 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2209 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2211 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*_tmp53A));({void*_tmpA0F=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp53A->hd=_tmpA0F;}),_tmp53A->tl=0;_tmp53A;}));
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2213 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*_tmp53B));({void*_tmpA10=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp53B->hd=_tmpA10;}),_tmp53B->tl=0;_tmp53B;}));
# 2216
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2233 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp53C=_cycalloc(sizeof(*_tmp53C));({void*_tmpA12=Cyc_Absyn_access_eff(({struct _tuple8*_tmpA11=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA11,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp53C->hd=_tmpA12;}),_tmp53C->tl=0;_tmp53C;}));
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2235 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp53D=_cycalloc(sizeof(*_tmp53D));({void*_tmpA15=Cyc_Absyn_access_eff(({struct _tuple8*_tmpA14=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA14,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp53D->hd=_tmpA15;}),({struct Cyc_List_List*_tmpA13=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp53D->tl=_tmpA13;});_tmp53D;}));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2241 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53E=_cycalloc(sizeof(*_tmp53E));({struct _tuple8*_tmpA16=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp53E->hd=_tmpA16;}),_tmp53E->tl=0;_tmp53E;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2243 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53F=_cycalloc(sizeof(*_tmp53F));({struct _tuple8*_tmpA18=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp53F->hd=_tmpA18;}),({struct Cyc_List_List*_tmpA17=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp53F->tl=_tmpA17;});_tmp53F;}));
goto _LL0;case 251U: _LL1F1: _LL1F2: {
# 2249 "parse.y"
struct _tuple26 _tmp540=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp540;struct _tuple26 _tmp541=_stmttmp21;struct Cyc_List_List*_tmp544;struct Cyc_Parse_Type_specifier _tmp543;struct Cyc_Absyn_Tqual _tmp542;_LL47D: _tmp542=_tmp541.f1;_tmp543=_tmp541.f2;_tmp544=_tmp541.f3;_LL47E: {struct Cyc_Absyn_Tqual tq=_tmp542;struct Cyc_Parse_Type_specifier tspecs=_tmp543;struct Cyc_List_List*atts=_tmp544;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp545=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp22=_tmp545;struct Cyc_Parse_Declarator _tmp546=_stmttmp22;struct Cyc_List_List*_tmp549;unsigned _tmp548;struct _tuple0*_tmp547;_LL480: _tmp547=_tmp546.id;_tmp548=_tmp546.varloc;_tmp549=_tmp546.tms;_LL481: {struct _tuple0*qv=_tmp547;unsigned varloc=_tmp548;struct Cyc_List_List*tms=_tmp549;
void*_tmp54A=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp54A;
struct _tuple14 _tmp54B=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp54B;struct _tuple14 _tmp54C=_stmttmp23;struct Cyc_List_List*_tmp550;struct Cyc_List_List*_tmp54F;void*_tmp54E;struct Cyc_Absyn_Tqual _tmp54D;_LL483: _tmp54D=_tmp54C.f1;_tmp54E=_tmp54C.f2;_tmp54F=_tmp54C.f3;_tmp550=_tmp54C.f4;_LL484: {struct Cyc_Absyn_Tqual tq2=_tmp54D;void*t2=_tmp54E;struct Cyc_List_List*tvs=_tmp54F;struct Cyc_List_List*atts2=_tmp550;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp552=({struct Cyc_Warn_String_Warn_Warg_struct _tmp764;_tmp764.tag=0U,({struct _fat_ptr _tmpA19=({const char*_tmp553="parameter with bad type params";_tag_fat(_tmp553,sizeof(char),31U);});_tmp764.f1=_tmpA19;});_tmp764;});void*_tmp551[1U];_tmp551[0]=& _tmp552;({unsigned _tmpA1A=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmpA1A,_tag_fat(_tmp551,sizeof(void*),1U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp555=({struct Cyc_Warn_String_Warn_Warg_struct _tmp765;_tmp765.tag=0U,({struct _fat_ptr _tmpA1B=({const char*_tmp556="parameter cannot be qualified with a namespace";_tag_fat(_tmp556,sizeof(char),47U);});_tmp765.f1=_tmpA1B;});_tmp765;});void*_tmp554[1U];_tmp554[0]=& _tmp555;({unsigned _tmpA1C=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmpA1C,_tag_fat(_tmp554,sizeof(void*),1U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp558=({struct Cyc_Warn_String_Warn_Warg_struct _tmp766;_tmp766.tag=0U,({struct _fat_ptr _tmpA1D=({const char*_tmp559="extra attributes on parameter, ignoring";_tag_fat(_tmp559,sizeof(char),40U);});_tmp766.f1=_tmpA1D;});_tmp766;});void*_tmp557[1U];_tmp557[0]=& _tmp558;({unsigned _tmpA1E=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA1E,_tag_fat(_tmp557,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->f1=idopt,_tmp55A->f2=tq2,_tmp55A->f3=t2;_tmp55A;}));
# 2263
goto _LL0;}}}}}}case 252U: _LL1F3: _LL1F4: {
# 2264 "parse.y"
struct _tuple26 _tmp55B=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp55B;struct _tuple26 _tmp55C=_stmttmp24;struct Cyc_List_List*_tmp55F;struct Cyc_Parse_Type_specifier _tmp55E;struct Cyc_Absyn_Tqual _tmp55D;_LL486: _tmp55D=_tmp55C.f1;_tmp55E=_tmp55C.f2;_tmp55F=_tmp55C.f3;_LL487: {struct Cyc_Absyn_Tqual tq=_tmp55D;struct Cyc_Parse_Type_specifier tspecs=_tmp55E;struct Cyc_List_List*atts=_tmp55F;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp560=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp560;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp562=({struct Cyc_Warn_String_Warn_Warg_struct _tmp767;_tmp767.tag=0U,({struct _fat_ptr _tmpA1F=({const char*_tmp563="bad attributes on parameter, ignoring";_tag_fat(_tmp563,sizeof(char),38U);});_tmp767.f1=_tmpA1F;});_tmp767;});void*_tmp561[1U];_tmp561[0]=& _tmp562;({unsigned _tmpA20=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA20,_tag_fat(_tmp561,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->f1=0,_tmp564->f2=tq,_tmp564->f3=t;_tmp564;}));
# 2271
goto _LL0;}}}case 253U: _LL1F5: _LL1F6: {
# 2272 "parse.y"
struct _tuple26 _tmp565=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp565;struct _tuple26 _tmp566=_stmttmp25;struct Cyc_List_List*_tmp569;struct Cyc_Parse_Type_specifier _tmp568;struct Cyc_Absyn_Tqual _tmp567;_LL489: _tmp567=_tmp566.f1;_tmp568=_tmp566.f2;_tmp569=_tmp566.f3;_LL48A: {struct Cyc_Absyn_Tqual tq=_tmp567;struct Cyc_Parse_Type_specifier tspecs=_tmp568;struct Cyc_List_List*atts=_tmp569;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp56A=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp56A;
struct Cyc_List_List*_tmp56B=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp56B;
struct _tuple14 _tmp56C=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp26=_tmp56C;struct _tuple14 _tmp56D=_stmttmp26;struct Cyc_List_List*_tmp571;struct Cyc_List_List*_tmp570;void*_tmp56F;struct Cyc_Absyn_Tqual _tmp56E;_LL48C: _tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;_tmp570=_tmp56D.f3;_tmp571=_tmp56D.f4;_LL48D: {struct Cyc_Absyn_Tqual tq2=_tmp56E;void*t2=_tmp56F;struct Cyc_List_List*tvs=_tmp570;struct Cyc_List_List*atts2=_tmp571;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp573=({struct Cyc_Warn_String_Warn_Warg_struct _tmp768;_tmp768.tag=0U,({
struct _fat_ptr _tmpA21=({const char*_tmp574="bad type parameters on formal argument, ignoring";_tag_fat(_tmp574,sizeof(char),49U);});_tmp768.f1=_tmpA21;});_tmp768;});void*_tmp572[1U];_tmp572[0]=& _tmp573;({unsigned _tmpA22=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA22,_tag_fat(_tmp572,sizeof(void*),1U));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp576=({struct Cyc_Warn_String_Warn_Warg_struct _tmp769;_tmp769.tag=0U,({struct _fat_ptr _tmpA23=({const char*_tmp577="bad attributes on parameter, ignoring";_tag_fat(_tmp577,sizeof(char),38U);});_tmp769.f1=_tmpA23;});_tmp769;});void*_tmp575[1U];_tmp575[0]=& _tmp576;({unsigned _tmpA24=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmpA24,_tag_fat(_tmp575,sizeof(void*),1U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->f1=0,_tmp578->f2=tq2,_tmp578->f3=t2;_tmp578;}));
# 2284
goto _LL0;}}}}case 254U: _LL1F7: _LL1F8:
# 2288 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 255U: _LL1F9: _LL1FA:
# 2292 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp57A=_cycalloc(sizeof(*_tmp57A));({struct _fat_ptr*_tmpA26=({struct _fat_ptr*_tmp579=_cycalloc(sizeof(*_tmp579));({struct _fat_ptr _tmpA25=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp579=_tmpA25;});_tmp579;});_tmp57A->hd=_tmpA26;}),_tmp57A->tl=0;_tmp57A;}));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2294 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));({struct _fat_ptr*_tmpA29=({struct _fat_ptr*_tmp57B=_cycalloc(sizeof(*_tmp57B));({struct _fat_ptr _tmpA28=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp57B=_tmpA28;});_tmp57B;});_tmp57C->hd=_tmpA29;}),({struct Cyc_List_List*_tmpA27=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp57C->tl=_tmpA27;});_tmp57C;}));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2298 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 258U: _LL1FF: _LL200:
# 2299 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 259U: _LL201: _LL202:
# 2304 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA2A=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=36U,_tmp57D->f1=0,_tmp57D->f2=0;_tmp57D;});Cyc_Absyn_new_exp(_tmpA2A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 260U: _LL203: _LL204:
# 2306 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA2C=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->tag=36U,_tmp57E->f1=0,({struct Cyc_List_List*_tmpA2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp57E->f2=_tmpA2B;});_tmp57E;});Cyc_Absyn_new_exp(_tmpA2C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 261U: _LL205: _LL206:
# 2308 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA2E=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->tag=36U,_tmp57F->f1=0,({struct Cyc_List_List*_tmpA2D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp57F->f2=_tmpA2D;});_tmp57F;});Cyc_Absyn_new_exp(_tmpA2E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208: {
# 2310 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp580=({unsigned _tmpA33=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA32=({struct _tuple0*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA30=({struct _fat_ptr*_tmp582=_cycalloc(sizeof(*_tmp582));({struct _fat_ptr _tmpA2F=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp582=_tmpA2F;});_tmp582;});_tmp583->f2=_tmpA30;});_tmp583;});void*_tmpA31=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA33,_tmpA32,_tmpA31,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2310
struct Cyc_Absyn_Vardecl*vd=_tmp580;
# 2313
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA36=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=27U,_tmp581->f1=vd,({struct Cyc_Absyn_Exp*_tmpA35=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp581->f2=_tmpA35;}),({struct Cyc_Absyn_Exp*_tmpA34=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp581->f3=_tmpA34;}),_tmp581->f4=0;_tmp581;});Cyc_Absyn_new_exp(_tmpA36,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2316
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2318 "parse.y"
void*_tmp584=({struct _tuple8*_tmpA37=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA37,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp584;
yyval=Cyc_Exp_tok(({void*_tmpA39=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->tag=28U,({struct Cyc_Absyn_Exp*_tmpA38=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp585->f1=_tmpA38;}),_tmp585->f2=t,_tmp585->f3=0;_tmp585;});Cyc_Absyn_new_exp(_tmpA39,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2321
goto _LL0;}case 264U: _LL20B: _LL20C:
# 2326 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({struct _tuple35*_tmpA3B=({struct _tuple35*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->f1=0,({struct Cyc_Absyn_Exp*_tmpA3A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp586->f2=_tmpA3A;});_tmp586;});_tmp587->hd=_tmpA3B;}),_tmp587->tl=0;_tmp587;}));
goto _LL0;case 265U: _LL20D: _LL20E:
# 2328 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));({struct _tuple35*_tmpA3E=({struct _tuple35*_tmp588=_cycalloc(sizeof(*_tmp588));({struct Cyc_List_List*_tmpA3D=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp588->f1=_tmpA3D;}),({struct Cyc_Absyn_Exp*_tmpA3C=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp588->f2=_tmpA3C;});_tmp588;});_tmp589->hd=_tmpA3E;}),_tmp589->tl=0;_tmp589;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2330 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));({struct _tuple35*_tmpA41=({struct _tuple35*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->f1=0,({struct Cyc_Absyn_Exp*_tmpA40=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp58A->f2=_tmpA40;});_tmp58A;});_tmp58B->hd=_tmpA41;}),({struct Cyc_List_List*_tmpA3F=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp58B->tl=_tmpA3F;});_tmp58B;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2332 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp58D=_cycalloc(sizeof(*_tmp58D));({struct _tuple35*_tmpA45=({struct _tuple35*_tmp58C=_cycalloc(sizeof(*_tmp58C));({struct Cyc_List_List*_tmpA44=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp58C->f1=_tmpA44;}),({struct Cyc_Absyn_Exp*_tmpA43=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp58C->f2=_tmpA43;});_tmp58C;});_tmp58D->hd=_tmpA45;}),({struct Cyc_List_List*_tmpA42=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp58D->tl=_tmpA42;});_tmp58D;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2336 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 269U: _LL215: _LL216:
# 2337 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp590=_cycalloc(sizeof(*_tmp590));({void*_tmpA48=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->tag=1U,({struct _fat_ptr*_tmpA47=({struct _fat_ptr*_tmp58E=_cycalloc(sizeof(*_tmp58E));({struct _fat_ptr _tmpA46=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp58E=_tmpA46;});_tmp58E;});_tmp58F->f1=_tmpA47;});_tmp58F;});_tmp590->hd=_tmpA48;}),_tmp590->tl=0;_tmp590;}));
goto _LL0;case 270U: _LL217: _LL218:
# 2342 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp591=_cycalloc(sizeof(*_tmp591));({void*_tmpA49=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp591->hd=_tmpA49;}),_tmp591->tl=0;_tmp591;}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2343 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp592=_cycalloc(sizeof(*_tmp592));({void*_tmpA4B=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp592->hd=_tmpA4B;}),({struct Cyc_List_List*_tmpA4A=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp592->tl=_tmpA4A;});_tmp592;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2347 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->tag=0U,({struct Cyc_Absyn_Exp*_tmpA4C=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp593->f1=_tmpA4C;});_tmp593;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2348 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->tag=1U,({struct _fat_ptr*_tmpA4E=({struct _fat_ptr*_tmp594=_cycalloc(sizeof(*_tmp594));({struct _fat_ptr _tmpA4D=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp594=_tmpA4D;});_tmp594;});_tmp595->f1=_tmpA4E;});_tmp595;}));
goto _LL0;case 274U: _LL21F: _LL220: {
# 2353 "parse.y"
struct _tuple26 _tmp596=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp27=_tmp596;struct _tuple26 _tmp597=_stmttmp27;struct Cyc_List_List*_tmp59A;struct Cyc_Parse_Type_specifier _tmp599;struct Cyc_Absyn_Tqual _tmp598;_LL48F: _tmp598=_tmp597.f1;_tmp599=_tmp597.f2;_tmp59A=_tmp597.f3;_LL490: {struct Cyc_Absyn_Tqual tq=_tmp598;struct Cyc_Parse_Type_specifier tss=_tmp599;struct Cyc_List_List*atts=_tmp59A;
void*_tmp59B=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp59B;
if(atts != 0)
({void*_tmp59C=0U;({unsigned _tmpA50=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA4F=({const char*_tmp59D="ignoring attributes in type";_tag_fat(_tmp59D,sizeof(char),28U);});Cyc_Warn_warn(_tmpA50,_tmpA4F,_tag_fat(_tmp59C,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->f1=0,_tmp59E->f2=tq,_tmp59E->f3=t;_tmp59E;}));
# 2359
goto _LL0;}}case 275U: _LL221: _LL222: {
# 2360 "parse.y"
struct _tuple26 _tmp59F=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp28=_tmp59F;struct _tuple26 _tmp5A0=_stmttmp28;struct Cyc_List_List*_tmp5A3;struct Cyc_Parse_Type_specifier _tmp5A2;struct Cyc_Absyn_Tqual _tmp5A1;_LL492: _tmp5A1=_tmp5A0.f1;_tmp5A2=_tmp5A0.f2;_tmp5A3=_tmp5A0.f3;_LL493: {struct Cyc_Absyn_Tqual tq=_tmp5A1;struct Cyc_Parse_Type_specifier tss=_tmp5A2;struct Cyc_List_List*atts=_tmp5A3;
void*_tmp5A4=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp5A4;
struct Cyc_List_List*_tmp5A5=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp5A5;
struct _tuple14 _tmp5A6=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp5A6;
if(t_info.f3 != 0)
# 2366
({void*_tmp5A7=0U;({unsigned _tmpA52=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA51=({const char*_tmp5A8="bad type params, ignoring";_tag_fat(_tmp5A8,sizeof(char),26U);});Cyc_Warn_warn(_tmpA52,_tmpA51,_tag_fat(_tmp5A7,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp5A9=0U;({unsigned _tmpA54=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA53=({const char*_tmp5AA="bad specifiers, ignoring";_tag_fat(_tmp5AA,sizeof(char),25U);});Cyc_Warn_warn(_tmpA54,_tmpA53,_tag_fat(_tmp5A9,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->f1=0,_tmp5AB->f2=t_info.f1,_tmp5AB->f3=t_info.f2;_tmp5AB;}));
# 2371
goto _LL0;}}case 276U: _LL223: _LL224:
# 2374 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA55=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA55,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 277U: _LL225: _LL226:
# 2375 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 278U: _LL227: _LL228:
# 2376 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 279U: _LL229: _LL22A:
# 2377 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2378 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));({void*_tmpA57=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5AC->hd=_tmpA57;}),({struct Cyc_List_List*_tmpA56=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp5AC->tl=_tmpA56;});_tmp5AC;})));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2384 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));({void*_tmpA58=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5AD->hd=_tmpA58;}),_tmp5AD->tl=0;_tmp5AD;}));
goto _LL0;case 282U: _LL22F: _LL230:
# 2385 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));({void*_tmpA5A=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp5AE->hd=_tmpA5A;}),({struct Cyc_List_List*_tmpA59=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp5AE->tl=_tmpA59;});_tmp5AE;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2390 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp76A;({struct Cyc_List_List*_tmpA5B=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp76A.tms=_tmpA5B;});_tmp76A;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2392 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 285U: _LL235: _LL236:
# 2394 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp76B;({struct Cyc_List_List*_tmpA5D=({struct Cyc_List_List*_tmpA5C=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA5C,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp76B.tms=_tmpA5D;});_tmp76B;}));
goto _LL0;case 286U: _LL237: _LL238:
# 2399 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 287U: _LL239: _LL23A:
# 2401 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp76C;({struct Cyc_List_List*_tmpA60=({struct Cyc_List_List*_tmp5B0=_region_malloc(yyr,sizeof(*_tmp5B0));({void*_tmpA5F=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));_tmp5AF->tag=0U,({void*_tmpA5E=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5AF->f1=_tmpA5E;}),_tmp5AF->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5AF;});_tmp5B0->hd=_tmpA5F;}),_tmp5B0->tl=0;_tmp5B0;});_tmp76C.tms=_tmpA60;});_tmp76C;}));
goto _LL0;case 288U: _LL23B: _LL23C:
# 2403 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp76D;({struct Cyc_List_List*_tmpA64=({struct Cyc_List_List*_tmp5B2=_region_malloc(yyr,sizeof(*_tmp5B2));({void*_tmpA63=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5B1=_region_malloc(yyr,sizeof(*_tmp5B1));_tmp5B1->tag=0U,({void*_tmpA62=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5B1->f1=_tmpA62;}),_tmp5B1->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5B1;});_tmp5B2->hd=_tmpA63;}),({struct Cyc_List_List*_tmpA61=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5B2->tl=_tmpA61;});_tmp5B2;});_tmp76D.tms=_tmpA64;});_tmp76D;}));
goto _LL0;case 289U: _LL23D: _LL23E:
# 2405 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp76E;({struct Cyc_List_List*_tmpA68=({struct Cyc_List_List*_tmp5B4=_region_malloc(yyr,sizeof(*_tmp5B4));({void*_tmpA67=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5B3=_region_malloc(yyr,sizeof(*_tmp5B3));_tmp5B3->tag=1U,({struct Cyc_Absyn_Exp*_tmpA66=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5B3->f1=_tmpA66;}),({void*_tmpA65=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5B3->f2=_tmpA65;}),_tmp5B3->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5B3;});_tmp5B4->hd=_tmpA67;}),_tmp5B4->tl=0;_tmp5B4;});_tmp76E.tms=_tmpA68;});_tmp76E;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2407 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp76F;({struct Cyc_List_List*_tmpA6D=({struct Cyc_List_List*_tmp5B6=_region_malloc(yyr,sizeof(*_tmp5B6));({void*_tmpA6C=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5B5=_region_malloc(yyr,sizeof(*_tmp5B5));_tmp5B5->tag=1U,({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5B5->f1=_tmpA6B;}),({void*_tmpA6A=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5B5->f2=_tmpA6A;}),_tmp5B5->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5B5;});_tmp5B6->hd=_tmpA6C;}),({
struct Cyc_List_List*_tmpA69=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5B6->tl=_tmpA69;});_tmp5B6;});
# 2407
_tmp76F.tms=_tmpA6D;});_tmp76F;}));
# 2410
goto _LL0;case 291U: _LL241: _LL242:
# 2411 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp770;({struct Cyc_List_List*_tmpA74=({struct Cyc_List_List*_tmp5B9=_region_malloc(yyr,sizeof(*_tmp5B9));({void*_tmpA73=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B8=_region_malloc(yyr,sizeof(*_tmp5B8));_tmp5B8->tag=3U,({void*_tmpA72=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));_tmp5B7->tag=1U,_tmp5B7->f1=0,_tmp5B7->f2=0,_tmp5B7->f3=0,({void*_tmpA71=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5B7->f4=_tmpA71;}),({struct Cyc_List_List*_tmpA70=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5B7->f5=_tmpA70;}),({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B7->f6=_tmpA6F;}),({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5B7->f7=_tmpA6E;});_tmp5B7;});_tmp5B8->f1=_tmpA72;});_tmp5B8;});_tmp5B9->hd=_tmpA73;}),_tmp5B9->tl=0;_tmp5B9;});_tmp770.tms=_tmpA74;});_tmp770;}));
# 2413
goto _LL0;case 292U: _LL243: _LL244: {
# 2414 "parse.y"
struct _tuple27*_tmp5BA=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp29=_tmp5BA;struct _tuple27*_tmp5BB=_stmttmp29;struct Cyc_List_List*_tmp5C0;void*_tmp5BF;struct Cyc_Absyn_VarargInfo*_tmp5BE;int _tmp5BD;struct Cyc_List_List*_tmp5BC;_LL495: _tmp5BC=_tmp5BB->f1;_tmp5BD=_tmp5BB->f2;_tmp5BE=_tmp5BB->f3;_tmp5BF=_tmp5BB->f4;_tmp5C0=_tmp5BB->f5;_LL496: {struct Cyc_List_List*lis=_tmp5BC;int b=_tmp5BD;struct Cyc_Absyn_VarargInfo*c=_tmp5BE;void*eff=_tmp5BF;struct Cyc_List_List*po=_tmp5C0;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp771;({struct Cyc_List_List*_tmpA79=({struct Cyc_List_List*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));({void*_tmpA78=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C2=_region_malloc(yyr,sizeof(*_tmp5C2));_tmp5C2->tag=3U,({void*_tmpA77=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C1=_region_malloc(yyr,sizeof(*_tmp5C1));_tmp5C1->tag=1U,_tmp5C1->f1=lis,_tmp5C1->f2=b,_tmp5C1->f3=c,_tmp5C1->f4=eff,_tmp5C1->f5=po,({struct Cyc_Absyn_Exp*_tmpA76=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5C1->f6=_tmpA76;}),({struct Cyc_Absyn_Exp*_tmpA75=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5C1->f7=_tmpA75;});_tmp5C1;});_tmp5C2->f1=_tmpA77;});_tmp5C2;});_tmp5C3->hd=_tmpA78;}),_tmp5C3->tl=0;_tmp5C3;});_tmp771.tms=_tmpA79;});_tmp771;}));
# 2417
goto _LL0;}}case 293U: _LL245: _LL246:
# 2418 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp772;({struct Cyc_List_List*_tmpA81=({struct Cyc_List_List*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));({void*_tmpA80=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));_tmp5C5->tag=3U,({void*_tmpA7F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4->tag=1U,_tmp5C4->f1=0,_tmp5C4->f2=0,_tmp5C4->f3=0,({void*_tmpA7E=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5C4->f4=_tmpA7E;}),({struct Cyc_List_List*_tmpA7D=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5C4->f5=_tmpA7D;}),({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5C4->f6=_tmpA7C;}),({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5C4->f7=_tmpA7B;});_tmp5C4;});_tmp5C5->f1=_tmpA7F;});_tmp5C5;});_tmp5C6->hd=_tmpA80;}),({
struct Cyc_List_List*_tmpA7A=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C6->tl=_tmpA7A;});_tmp5C6;});
# 2418
_tmp772.tms=_tmpA81;});_tmp772;}));
# 2421
goto _LL0;case 294U: _LL247: _LL248: {
# 2422 "parse.y"
struct _tuple27*_tmp5C7=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp2A=_tmp5C7;struct _tuple27*_tmp5C8=_stmttmp2A;struct Cyc_List_List*_tmp5CD;void*_tmp5CC;struct Cyc_Absyn_VarargInfo*_tmp5CB;int _tmp5CA;struct Cyc_List_List*_tmp5C9;_LL498: _tmp5C9=_tmp5C8->f1;_tmp5CA=_tmp5C8->f2;_tmp5CB=_tmp5C8->f3;_tmp5CC=_tmp5C8->f4;_tmp5CD=_tmp5C8->f5;_LL499: {struct Cyc_List_List*lis=_tmp5C9;int b=_tmp5CA;struct Cyc_Absyn_VarargInfo*c=_tmp5CB;void*eff=_tmp5CC;struct Cyc_List_List*po=_tmp5CD;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp773;({struct Cyc_List_List*_tmpA87=({struct Cyc_List_List*_tmp5D0=_region_malloc(yyr,sizeof(*_tmp5D0));({void*_tmpA86=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CF=_region_malloc(yyr,sizeof(*_tmp5CF));_tmp5CF->tag=3U,({void*_tmpA85=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5CE=_region_malloc(yyr,sizeof(*_tmp5CE));_tmp5CE->tag=1U,_tmp5CE->f1=lis,_tmp5CE->f2=b,_tmp5CE->f3=c,_tmp5CE->f4=eff,_tmp5CE->f5=po,({
struct Cyc_Absyn_Exp*_tmpA84=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5CE->f6=_tmpA84;}),({struct Cyc_Absyn_Exp*_tmpA83=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5CE->f7=_tmpA83;});_tmp5CE;});
# 2423
_tmp5CF->f1=_tmpA85;});_tmp5CF;});_tmp5D0->hd=_tmpA86;}),({
struct Cyc_List_List*_tmpA82=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5D0->tl=_tmpA82;});_tmp5D0;});
# 2423
_tmp773.tms=_tmpA87;});_tmp773;}));
# 2426
goto _LL0;}}case 295U: _LL249: _LL24A: {
# 2428
struct Cyc_List_List*_tmp5D1=({unsigned _tmpA88=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA88,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5D1;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp774;({struct Cyc_List_List*_tmpA8B=({struct Cyc_List_List*_tmp5D3=_region_malloc(yyr,sizeof(*_tmp5D3));({void*_tmpA8A=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5D2=_region_malloc(yyr,sizeof(*_tmp5D2));_tmp5D2->tag=4U,_tmp5D2->f1=ts,_tmp5D2->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5D2->f3=0;_tmp5D2;});_tmp5D3->hd=_tmpA8A;}),({
struct Cyc_List_List*_tmpA89=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5D3->tl=_tmpA89;});_tmp5D3;});
# 2429
_tmp774.tms=_tmpA8B;});_tmp774;}));
# 2432
goto _LL0;}case 296U: _LL24B: _LL24C:
# 2433 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp775;({struct Cyc_List_List*_tmpA8F=({struct Cyc_List_List*_tmp5D5=_region_malloc(yyr,sizeof(*_tmp5D5));({void*_tmpA8E=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5D4=_region_malloc(yyr,sizeof(*_tmp5D4));_tmp5D4->tag=5U,_tmp5D4->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA8D=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5D4->f2=_tmpA8D;});_tmp5D4;});_tmp5D5->hd=_tmpA8E;}),({struct Cyc_List_List*_tmpA8C=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5D5->tl=_tmpA8C;});_tmp5D5;});_tmp775.tms=_tmpA8F;});_tmp775;}));
# 2435
goto _LL0;case 297U: _LL24D: _LL24E:
# 2439 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298U: _LL24F: _LL250:
# 2440 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299U: _LL251: _LL252:
# 2441 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2442 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2443 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2444 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2450 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA93=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->tag=13U,({struct _fat_ptr*_tmpA92=({struct _fat_ptr*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct _fat_ptr _tmpA91=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D6=_tmpA91;});_tmp5D6;});_tmp5D7->f1=_tmpA92;}),({struct Cyc_Absyn_Stmt*_tmpA90=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D7->f2=_tmpA90;});_tmp5D7;});Cyc_Absyn_new_stmt(_tmpA93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304U: _LL25B: _LL25C:
# 2454 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305U: _LL25D: _LL25E:
# 2455 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA94=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA94,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 306U: _LL25F: _LL260:
# 2460 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 307U: _LL261: _LL262:
# 2461 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 308U: _LL263: _LL264:
# 2466 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA95=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA95,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 309U: _LL265: _LL266:
# 2467 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA96=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA96,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2468 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA9B=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->tag=13U,({struct _fat_ptr*_tmpA9A=({struct _fat_ptr*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct _fat_ptr _tmpA99=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D8=_tmpA99;});_tmp5D8;});_tmp5D9->f1=_tmpA9A;}),({struct Cyc_Absyn_Stmt*_tmpA98=({struct Cyc_List_List*_tmpA97=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA97,Cyc_Absyn_skip_stmt(0U));});_tmp5D9->f2=_tmpA98;});_tmp5D9;});Cyc_Absyn_new_stmt(_tmpA9B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2470
goto _LL0;case 311U: _LL269: _LL26A:
# 2470 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpAA0=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->tag=13U,({struct _fat_ptr*_tmpA9F=({struct _fat_ptr*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct _fat_ptr _tmpA9E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5DA=_tmpA9E;});_tmp5DA;});_tmp5DB->f1=_tmpA9F;}),({struct Cyc_Absyn_Stmt*_tmpA9D=({struct Cyc_List_List*_tmpA9C=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA9C,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5DB->f2=_tmpA9D;});_tmp5DB;});Cyc_Absyn_new_stmt(_tmpAA0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2472
goto _LL0;case 312U: _LL26B: _LL26C:
# 2472 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 313U: _LL26D: _LL26E:
# 2473 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAA2=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpAA1=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpAA2,_tmpAA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 314U: _LL26F: _LL270:
# 2474 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpAA5=({void*_tmpAA4=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpAA3=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5DC->f1=_tmpAA3;});_tmp5DC;});Cyc_Absyn_new_decl(_tmpAA4,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpAA5,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 315U: _LL271: _LL272:
# 2477 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpAA8=({void*_tmpAA7=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpAA6=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5DD->f1=_tmpAA6;});_tmp5DD;});Cyc_Absyn_new_decl(_tmpAA7,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpAA8,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 316U: _LL273: _LL274:
# 2482 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAAA=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAA9=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpAAB,_tmpAAA,_tmpAA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317U: _LL275: _LL276:
# 2484 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAAD=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAAC=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpAAE,_tmpAAD,_tmpAAC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318U: _LL277: _LL278:
# 2490 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAB0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpAAF=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpAB0,_tmpAAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A: {
# 2493
struct Cyc_Absyn_Exp*_tmp5DE=({struct _tuple0*_tmpAB1=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpAB1,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5DE;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAB3=e;struct Cyc_List_List*_tmpAB2=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpAB3,_tmpAB2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2497
struct Cyc_Absyn_Exp*_tmp5DF=({struct Cyc_List_List*_tmpAB4=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpAB4,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5DF;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAB6=e;struct Cyc_List_List*_tmpAB5=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpAB6,_tmpAB5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2500
goto _LL0;}case 321U: _LL27D: _LL27E:
# 2503 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAB8=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpAB7=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpAB8,_tmpAB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 322U: _LL27F: _LL280:
# 2517 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 323U: _LL281: _LL282:
# 2520 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));({struct Cyc_Absyn_Switch_clause*_tmpABC=({struct Cyc_Absyn_Switch_clause*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));({struct Cyc_Absyn_Pat*_tmpABB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5E0->pattern=_tmpABB;}),_tmp5E0->pat_vars=0,_tmp5E0->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpABA=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5E0->body=_tmpABA;}),_tmp5E0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E0;});
# 2520
_tmp5E1->hd=_tmpABC;}),({
# 2522
struct Cyc_List_List*_tmpAB9=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5E1->tl=_tmpAB9;});_tmp5E1;}));
goto _LL0;case 324U: _LL283: _LL284:
# 2524 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));({struct Cyc_Absyn_Switch_clause*_tmpAC0=({struct Cyc_Absyn_Switch_clause*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));({struct Cyc_Absyn_Pat*_tmpABF=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E2->pattern=_tmpABF;}),_tmp5E2->pat_vars=0,_tmp5E2->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpABE=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5E2->body=_tmpABE;}),_tmp5E2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E2;});
# 2524
_tmp5E3->hd=_tmpAC0;}),({
# 2526
struct Cyc_List_List*_tmpABD=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5E3->tl=_tmpABD;});_tmp5E3;}));
goto _LL0;case 325U: _LL285: _LL286:
# 2528 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));({struct Cyc_Absyn_Switch_clause*_tmpAC4=({struct Cyc_Absyn_Switch_clause*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));({struct Cyc_Absyn_Pat*_tmpAC3=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E4->pattern=_tmpAC3;}),_tmp5E4->pat_vars=0,_tmp5E4->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAC2=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5E4->body=_tmpAC2;}),_tmp5E4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E4;});_tmp5E5->hd=_tmpAC4;}),({struct Cyc_List_List*_tmpAC1=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5E5->tl=_tmpAC1;});_tmp5E5;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2530 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));({struct Cyc_Absyn_Switch_clause*_tmpAC9=({struct Cyc_Absyn_Switch_clause*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));({struct Cyc_Absyn_Pat*_tmpAC8=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E6->pattern=_tmpAC8;}),_tmp5E6->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5E6->where_clause=_tmpAC7;}),({
struct Cyc_Absyn_Stmt*_tmpAC6=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5E6->body=_tmpAC6;}),_tmp5E6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E6;});
# 2530
_tmp5E7->hd=_tmpAC9;}),({
# 2532
struct Cyc_List_List*_tmpAC5=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5E7->tl=_tmpAC5;});_tmp5E7;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2534 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));({struct Cyc_Absyn_Switch_clause*_tmpACE=({struct Cyc_Absyn_Switch_clause*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));({struct Cyc_Absyn_Pat*_tmpACD=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5E8->pattern=_tmpACD;}),_tmp5E8->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpACC=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5E8->where_clause=_tmpACC;}),({struct Cyc_Absyn_Stmt*_tmpACB=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5E8->body=_tmpACB;}),_tmp5E8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5E8;});_tmp5E9->hd=_tmpACE;}),({struct Cyc_List_List*_tmpACA=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5E9->tl=_tmpACA;});_tmp5E9;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2541 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpACF=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpAD0,_tmpACF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2545 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAD2=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAD1=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpAD2,_tmpAD1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2549 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAD5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD3=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2549
Cyc_Absyn_for_stmt(_tmpAD6,_tmpAD5,_tmpAD4,_tmpAD3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2551
goto _LL0;case 331U: _LL291: _LL292:
# 2552 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpADA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAD9=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAD7=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2552
Cyc_Absyn_for_stmt(_tmpADA,_tmpAD9,_tmpAD8,_tmpAD7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2554
goto _LL0;case 332U: _LL293: _LL294:
# 2555 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpADD=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpADC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADB=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2555
Cyc_Absyn_for_stmt(_tmpADE,_tmpADD,_tmpADC,_tmpADB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2557
goto _LL0;case 333U: _LL295: _LL296:
# 2558 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE1=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAE0=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpADF=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2558
Cyc_Absyn_for_stmt(_tmpAE2,_tmpAE1,_tmpAE0,_tmpADF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2560
goto _LL0;case 334U: _LL297: _LL298:
# 2561 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAE6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAE5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAE3=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2561
Cyc_Absyn_for_stmt(_tmpAE6,_tmpAE5,_tmpAE4,_tmpAE3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2563
goto _LL0;case 335U: _LL299: _LL29A:
# 2564 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAE9=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAE7=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2564
Cyc_Absyn_for_stmt(_tmpAEA,_tmpAE9,_tmpAE8,_tmpAE7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2566
goto _LL0;case 336U: _LL29B: _LL29C:
# 2567 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAEE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAED=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAEC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAEB=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2567
Cyc_Absyn_for_stmt(_tmpAEE,_tmpAED,_tmpAEC,_tmpAEB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2569
goto _LL0;case 337U: _LL29D: _LL29E:
# 2570 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAF2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAF1=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAF0=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAEF=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2570
Cyc_Absyn_for_stmt(_tmpAF2,_tmpAF1,_tmpAF0,_tmpAEF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2572
goto _LL0;case 338U: _LL29F: _LL2A0: {
# 2573 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5EA=({struct Cyc_Absyn_Exp*_tmpAF6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAF5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAF4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAF3=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2573
Cyc_Absyn_for_stmt(_tmpAF6,_tmpAF5,_tmpAF4,_tmpAF3,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5EA;
# 2575
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAF7=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAF7,s);}));
# 2577
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2578 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5EB=({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAFA=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAF9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAF8=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpAFB,_tmpAFA,_tmpAF9,_tmpAF8,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5EB;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAFC=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpAFC,s);}));
# 2581
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2582 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5EC=({struct Cyc_Absyn_Exp*_tmpB00=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAFD=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_for_stmt(_tmpB00,_tmpAFF,_tmpAFE,_tmpAFD,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5EC;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpB01=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpB01,s);}));
# 2585
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2586 "parse.y"
struct Cyc_Absyn_Stmt*_tmp5ED=({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpB03=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpB02=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpB05,_tmpB04,_tmpB03,_tmpB02,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5ED;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpB06=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpB06,s);}));
# 2589
goto _LL0;}case 342U: _LL2A7: _LL2A8:
# 2592 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpB08=({struct _fat_ptr*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));({struct _fat_ptr _tmpB07=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5EE=_tmpB07;});_tmp5EE;});Cyc_Absyn_goto_stmt(_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 343U: _LL2A9: _LL2AA:
# 2593 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2594 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2595 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2596 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpB09,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2598 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2599 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2601 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpB0A=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2610 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2613
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2615 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB0B=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB0D,_tmpB0C,_tmpB0B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2618
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2620 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2623
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2625 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpB11,_tmpB10,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2628
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2630 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB12=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB13,_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2633
 yyval=(yyyvsp[0]).v;
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2635 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB15,_tmpB14,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2638
 yyval=(yyyvsp[0]).v;
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2640 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB17,_tmpB16,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2643
 yyval=(yyyvsp[0]).v;
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2645 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpB19,_tmpB18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2647 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpB1B,_tmpB1A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2650
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2652 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB1D,_tmpB1C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2654 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB1E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB1F,_tmpB1E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2656 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB21=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB20=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB21,_tmpB20,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2658 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB23=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB22=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB23,_tmpB22,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2661
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2663 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB25=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB24=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB25,_tmpB24,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2665 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB27=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB26=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB27,_tmpB26,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2668
 yyval=(yyyvsp[0]).v;
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2670 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB29=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB28=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB29,_tmpB28,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2672 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB2B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB2A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB2B,_tmpB2A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2675
 yyval=(yyyvsp[0]).v;
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2677 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB2D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB2C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB2D,_tmpB2C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2679 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB2F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB2E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB2F,_tmpB2E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2681 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB31=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB30=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB31,_tmpB30,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2684
 yyval=(yyyvsp[0]).v;
goto _LL0;case 382U: _LL2F7: _LL2F8: {
# 2686 "parse.y"
void*_tmp5EF=({struct _tuple8*_tmpB32=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB32,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5EF;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB34=t;struct Cyc_Absyn_Exp*_tmpB33=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB34,_tmpB33,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2689
goto _LL0;}case 383U: _LL2F9: _LL2FA:
# 2692 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 384U: _LL2FB: _LL2FC:
# 2695
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2697 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB36=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB35=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB36,_tmpB35,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 386U: _LL2FF: _LL300: {
# 2699 "parse.y"
void*_tmp5F0=({struct _tuple8*_tmpB37=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB37,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5F0;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2702
goto _LL0;}case 387U: _LL301: _LL302:
# 2703 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB38=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB38,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 388U: _LL303: _LL304:
# 2705 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB3A=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB39=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB3A,_tmpB39,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 389U: _LL305: _LL306:
# 2710 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 390U: _LL307: _LL308:
# 2718 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 391U: _LL309: _LL30A:
# 2723 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 392U: _LL30B: _LL30C:
# 2725 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 393U: _LL30D: _LL30E: {
# 2727 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5F1=e->r;void*_stmttmp2B=_tmp5F1;void*_tmp5F2=_stmttmp2B;int _tmp5F4;struct _fat_ptr _tmp5F3;int _tmp5F6;enum Cyc_Absyn_Sign _tmp5F5;short _tmp5F8;enum Cyc_Absyn_Sign _tmp5F7;char _tmp5FA;enum Cyc_Absyn_Sign _tmp5F9;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).LongLong_c).tag){case 2U: _LL49B: _tmp5F9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Char_c).val).f1;_tmp5FA=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Char_c).val).f2;_LL49C: {enum Cyc_Absyn_Sign s=_tmp5F9;char i=_tmp5FA;
# 2731
yyval=Cyc_YY9(({void*_tmpB3B=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->tag=11U,_tmp5FB->f1=i;_tmp5FB;});Cyc_Absyn_new_pat(_tmpB3B,e->loc);}));goto _LL49A;}case 4U: _LL49D: _tmp5F7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Short_c).val).f1;_tmp5F8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Short_c).val).f2;_LL49E: {enum Cyc_Absyn_Sign s=_tmp5F7;short i=_tmp5F8;
# 2733
yyval=Cyc_YY9(({void*_tmpB3C=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=10U,_tmp5FC->f1=s,_tmp5FC->f2=(int)i;_tmp5FC;});Cyc_Absyn_new_pat(_tmpB3C,e->loc);}));goto _LL49A;}case 5U: _LL49F: _tmp5F5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Int_c).val).f1;_tmp5F6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Int_c).val).f2;_LL4A0: {enum Cyc_Absyn_Sign s=_tmp5F5;int i=_tmp5F6;
# 2735
yyval=Cyc_YY9(({void*_tmpB3D=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->tag=10U,_tmp5FD->f1=s,_tmp5FD->f2=i;_tmp5FD;});Cyc_Absyn_new_pat(_tmpB3D,e->loc);}));goto _LL49A;}case 7U: _LL4A1: _tmp5F3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Float_c).val).f1;_tmp5F4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F2)->f1).Float_c).val).f2;_LL4A2: {struct _fat_ptr s=_tmp5F3;int i=_tmp5F4;
# 2737
yyval=Cyc_YY9(({void*_tmpB3E=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->tag=12U,_tmp5FE->f1=s,_tmp5FE->f2=i;_tmp5FE;});Cyc_Absyn_new_pat(_tmpB3E,e->loc);}));goto _LL49A;}case 1U: _LL4A3: _LL4A4:
# 2739
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL49A;case 8U: _LL4A5: _LL4A6:
 goto _LL4A8;case 9U: _LL4A7: _LL4A8:
# 2742
({void*_tmp5FF=0U;({unsigned _tmpB40=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB3F=({const char*_tmp600="strings cannot occur within patterns";_tag_fat(_tmp600,sizeof(char),37U);});Cyc_Warn_err(_tmpB40,_tmpB3F,_tag_fat(_tmp5FF,sizeof(void*),0U));});});goto _LL49A;case 6U: _LL4A9: _LL4AA:
# 2744
({void*_tmp601=0U;({unsigned _tmpB42=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB41=({const char*_tmp602="long long's in patterns not yet implemented";_tag_fat(_tmp602,sizeof(char),44U);});Cyc_Warn_err(_tmpB42,_tmpB41,_tag_fat(_tmp601,sizeof(void*),0U));});});goto _LL49A;default: goto _LL4AB;}else{_LL4AB: _LL4AC:
# 2746
({void*_tmp603=0U;({unsigned _tmpB44=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB43=({const char*_tmp604="bad constant in case";_tag_fat(_tmp604,sizeof(char),21U);});Cyc_Warn_err(_tmpB44,_tmpB43,_tag_fat(_tmp603,sizeof(void*),0U));});});}_LL49A:;}
# 2749
goto _LL0;}case 394U: _LL30F: _LL310:
# 2750 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB46=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->tag=15U,({struct _tuple0*_tmpB45=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp605->f1=_tmpB45;});_tmp605;});Cyc_Absyn_new_pat(_tmpB46,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 395U: _LL311: _LL312:
# 2752 "parse.y"
 if(({struct _fat_ptr _tmpB47=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB47,({const char*_tmp606="as";_tag_fat(_tmp606,sizeof(char),3U);}));})!= 0)
({void*_tmp607=0U;({unsigned _tmpB49=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB48=({const char*_tmp608="expecting `as'";_tag_fat(_tmp608,sizeof(char),15U);});Cyc_Warn_err(_tmpB49,_tmpB48,_tag_fat(_tmp607,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB50=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB4F=({unsigned _tmpB4E=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB4D=({struct _tuple0*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB4C=({struct _fat_ptr*_tmp609=_cycalloc(sizeof(*_tmp609));({struct _fat_ptr _tmpB4B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp609=_tmpB4B;});_tmp609;});_tmp60A->f2=_tmpB4C;});_tmp60A;});Cyc_Absyn_new_vardecl(_tmpB4E,_tmpB4D,Cyc_Absyn_void_type,0);});_tmp60B->f1=_tmpB4F;}),({
struct Cyc_Absyn_Pat*_tmpB4A=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp60B->f2=_tmpB4A;});_tmp60B;});
# 2754
Cyc_Absyn_new_pat(_tmpB50,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2757
goto _LL0;case 396U: _LL313: _LL314:
# 2758 "parse.y"
 if(({struct _fat_ptr _tmpB51=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB51,({const char*_tmp60C="alias";_tag_fat(_tmp60C,sizeof(char),6U);}));})!= 0)
({void*_tmp60D=0U;({unsigned _tmpB53=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB52=({const char*_tmp60E="expecting `alias'";_tag_fat(_tmp60E,sizeof(char),18U);});Cyc_Warn_err(_tmpB53,_tmpB52,_tag_fat(_tmp60D,sizeof(void*),0U));});});{
int _tmp60F=((yyyvsp[0]).l).first_line;int location=_tmp60F;
struct _fat_ptr err=({const char*_tmp617="";_tag_fat(_tmp617,sizeof(char),1U);});
if(!Cyc_Parse_tvar_ok(Cyc_yyget_String_tok(&(yyyvsp[2]).v),& err))({void*_tmp610=0U;({unsigned _tmpB55=(unsigned)location;struct _fat_ptr _tmpB54=err;Cyc_Warn_err(_tmpB55,_tmpB54,_tag_fat(_tmp610,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp616=_cycalloc(sizeof(*_tmp616));({struct _fat_ptr*_tmpB58=({struct _fat_ptr*_tmp614=_cycalloc(sizeof(*_tmp614));({struct _fat_ptr _tmpB57=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp614=_tmpB57;});_tmp614;});_tmp616->name=_tmpB58;}),_tmp616->identity=- 1,({void*_tmpB56=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=0U,_tmp615->f1=& Cyc_Tcutil_rk;_tmp615;});_tmp616->kind=_tmpB56;});_tmp616;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB5D=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB5C=({struct _tuple0*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB5A=({struct _fat_ptr*_tmp612=_cycalloc(sizeof(*_tmp612));({struct _fat_ptr _tmpB59=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp612=_tmpB59;});_tmp612;});_tmp613->f2=_tmpB5A;});_tmp613;});Cyc_Absyn_new_vardecl(_tmpB5D,_tmpB5C,({
struct _tuple8*_tmpB5B=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB5B,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB5E=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->tag=2U,_tmp611->f1=tv,_tmp611->f2=vd;_tmp611;});Cyc_Absyn_new_pat(_tmpB5E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2768
goto _LL0;}}case 397U: _LL315: _LL316:
# 2769 "parse.y"
 if(({struct _fat_ptr _tmpB5F=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB5F,({const char*_tmp618="alias";_tag_fat(_tmp618,sizeof(char),6U);}));})!= 0)
({void*_tmp619=0U;({unsigned _tmpB61=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB60=({const char*_tmp61A="expecting `alias'";_tag_fat(_tmp61A,sizeof(char),18U);});Cyc_Warn_err(_tmpB61,_tmpB60,_tag_fat(_tmp619,sizeof(void*),0U));});});{
int _tmp61B=((yyyvsp[0]).l).first_line;int location=_tmp61B;
struct _fat_ptr err=({const char*_tmp623="";_tag_fat(_tmp623,sizeof(char),1U);});
if(!Cyc_Parse_tvar_ok(Cyc_yyget_String_tok(&(yyyvsp[2]).v),& err))({void*_tmp61C=0U;({unsigned _tmpB63=(unsigned)location;struct _fat_ptr _tmpB62=err;Cyc_Warn_err(_tmpB63,_tmpB62,_tag_fat(_tmp61C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp622=_cycalloc(sizeof(*_tmp622));({struct _fat_ptr*_tmpB66=({struct _fat_ptr*_tmp620=_cycalloc(sizeof(*_tmp620));({struct _fat_ptr _tmpB65=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp620=_tmpB65;});_tmp620;});_tmp622->name=_tmpB66;}),_tmp622->identity=- 1,({void*_tmpB64=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->tag=0U,_tmp621->f1=& Cyc_Tcutil_rk;_tmp621;});_tmp622->kind=_tmpB64;});_tmp622;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB6B=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB6A=({struct _tuple0*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB68=({struct _fat_ptr*_tmp61E=_cycalloc(sizeof(*_tmp61E));({struct _fat_ptr _tmpB67=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp61E=_tmpB67;});_tmp61E;});_tmp61F->f2=_tmpB68;});_tmp61F;});Cyc_Absyn_new_vardecl(_tmpB6B,_tmpB6A,({
struct _tuple8*_tmpB69=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB69,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB6C=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->tag=2U,_tmp61D->f1=tv,_tmp61D->f2=vd;_tmp61D;});Cyc_Absyn_new_pat(_tmpB6C,(unsigned)location);}));
# 2779
goto _LL0;}}case 398U: _LL317: _LL318: {
# 2780 "parse.y"
struct _tuple23 _tmp624=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp624;struct _tuple23 _tmp625=_stmttmp2C;int _tmp627;struct Cyc_List_List*_tmp626;_LL4AE: _tmp626=_tmp625.f1;_tmp627=_tmp625.f2;_LL4AF: {struct Cyc_List_List*ps=_tmp626;int dots=_tmp627;
yyval=Cyc_YY9(({void*_tmpB6D=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=5U,_tmp628->f1=ps,_tmp628->f2=dots;_tmp628;});Cyc_Absyn_new_pat(_tmpB6D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2783
goto _LL0;}}case 399U: _LL319: _LL31A: {
# 2784 "parse.y"
struct _tuple23 _tmp629=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2D=_tmp629;struct _tuple23 _tmp62A=_stmttmp2D;int _tmp62C;struct Cyc_List_List*_tmp62B;_LL4B1: _tmp62B=_tmp62A.f1;_tmp62C=_tmp62A.f2;_LL4B2: {struct Cyc_List_List*ps=_tmp62B;int dots=_tmp62C;
yyval=Cyc_YY9(({void*_tmpB6F=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->tag=16U,({struct _tuple0*_tmpB6E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp62D->f1=_tmpB6E;}),_tmp62D->f2=ps,_tmp62D->f3=dots;_tmp62D;});Cyc_Absyn_new_pat(_tmpB6F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2787
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2788 "parse.y"
struct _tuple23 _tmp62E=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2E=_tmp62E;struct _tuple23 _tmp62F=_stmttmp2E;int _tmp631;struct Cyc_List_List*_tmp630;_LL4B4: _tmp630=_tmp62F.f1;_tmp631=_tmp62F.f2;_LL4B5: {struct Cyc_List_List*fps=_tmp630;int dots=_tmp631;
struct Cyc_List_List*_tmp632=({unsigned _tmpB70=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB70,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp632;
yyval=Cyc_YY9(({void*_tmpB73=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB72=({union Cyc_Absyn_AggrInfo*_tmp633=_cycalloc(sizeof(*_tmp633));({union Cyc_Absyn_AggrInfo _tmpB71=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp633=_tmpB71;});_tmp633;});_tmp634->f1=_tmpB72;}),_tmp634->f2=exist_ts,_tmp634->f3=fps,_tmp634->f4=dots;_tmp634;});Cyc_Absyn_new_pat(_tmpB73,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2793
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2794 "parse.y"
struct _tuple23 _tmp635=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2F=_tmp635;struct _tuple23 _tmp636=_stmttmp2F;int _tmp638;struct Cyc_List_List*_tmp637;_LL4B7: _tmp637=_tmp636.f1;_tmp638=_tmp636.f2;_LL4B8: {struct Cyc_List_List*fps=_tmp637;int dots=_tmp638;
struct Cyc_List_List*_tmp639=({unsigned _tmpB74=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB74,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp639;
yyval=Cyc_YY9(({void*_tmpB75=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->tag=7U,_tmp63A->f1=0,_tmp63A->f2=exist_ts,_tmp63A->f3=fps,_tmp63A->f4=dots;_tmp63A;});Cyc_Absyn_new_pat(_tmpB75,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2798
goto _LL0;}}case 402U: _LL31F: _LL320:
# 2799 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB77=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tag=6U,({struct Cyc_Absyn_Pat*_tmpB76=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp63B->f1=_tmpB76;});_tmp63B;});Cyc_Absyn_new_pat(_tmpB77,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 403U: _LL321: _LL322:
# 2801 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB7B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->tag=6U,({struct Cyc_Absyn_Pat*_tmpB7A=({void*_tmpB79=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->tag=6U,({struct Cyc_Absyn_Pat*_tmpB78=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp63C->f1=_tmpB78;});_tmp63C;});Cyc_Absyn_new_pat(_tmpB79,(unsigned)((yyyvsp[0]).l).first_line);});_tmp63D->f1=_tmpB7A;});_tmp63D;});Cyc_Absyn_new_pat(_tmpB7B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404U: _LL323: _LL324:
# 2803 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB82=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB81=({unsigned _tmpB80=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB7F=({struct _tuple0*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB7E=({struct _fat_ptr*_tmp63E=_cycalloc(sizeof(*_tmp63E));({struct _fat_ptr _tmpB7D=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp63E=_tmpB7D;});_tmp63E;});_tmp63F->f2=_tmpB7E;});_tmp63F;});Cyc_Absyn_new_vardecl(_tmpB80,_tmpB7F,Cyc_Absyn_void_type,0);});_tmp640->f1=_tmpB81;}),({
# 2805
struct Cyc_Absyn_Pat*_tmpB7C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp640->f2=_tmpB7C;});_tmp640;});
# 2803
Cyc_Absyn_new_pat(_tmpB82,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2807
goto _LL0;case 405U: _LL325: _LL326:
# 2808 "parse.y"
 if(({struct _fat_ptr _tmpB83=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB83,({const char*_tmp641="as";_tag_fat(_tmp641,sizeof(char),3U);}));})!= 0)
({void*_tmp642=0U;({unsigned _tmpB85=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB84=({const char*_tmp643="expecting `as'";_tag_fat(_tmp643,sizeof(char),15U);});Cyc_Warn_err(_tmpB85,_tmpB84,_tag_fat(_tmp642,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB8C=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB8B=({unsigned _tmpB8A=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB89=({struct _tuple0*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB88=({struct _fat_ptr*_tmp644=_cycalloc(sizeof(*_tmp644));({struct _fat_ptr _tmpB87=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp644=_tmpB87;});_tmp644;});_tmp645->f2=_tmpB88;});_tmp645;});Cyc_Absyn_new_vardecl(_tmpB8A,_tmpB89,Cyc_Absyn_void_type,0);});_tmp646->f1=_tmpB8B;}),({
# 2812
struct Cyc_Absyn_Pat*_tmpB86=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp646->f2=_tmpB86;});_tmp646;});
# 2810
Cyc_Absyn_new_pat(_tmpB8C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2814
goto _LL0;case 406U: _LL327: _LL328: {
# 2815 "parse.y"
void*_tmp647=({struct _fat_ptr _tmpB8D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB8D,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});void*tag=_tmp647;
yyval=Cyc_YY9(({void*_tmpB94=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB93=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp64A->f1=_tmpB93;}),({
struct Cyc_Absyn_Vardecl*_tmpB92=({unsigned _tmpB91=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB90=({struct _tuple0*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB8F=({struct _fat_ptr*_tmp648=_cycalloc(sizeof(*_tmp648));({struct _fat_ptr _tmpB8E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp648=_tmpB8E;});_tmp648;});_tmp649->f2=_tmpB8F;});_tmp649;});Cyc_Absyn_new_vardecl(_tmpB91,_tmpB90,
Cyc_Absyn_tag_type(tag),0);});
# 2817
_tmp64A->f2=_tmpB92;});_tmp64A;});
# 2816
Cyc_Absyn_new_pat(_tmpB94,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2820
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2821 "parse.y"
struct Cyc_Absyn_Tvar*_tmp64B=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));struct Cyc_Absyn_Tvar*tv=_tmp64B;
yyval=Cyc_YY9(({void*_tmpB9A=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->tag=4U,_tmp64E->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB99=({unsigned _tmpB98=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB97=({struct _tuple0*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB96=({struct _fat_ptr*_tmp64C=_cycalloc(sizeof(*_tmp64C));({struct _fat_ptr _tmpB95=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp64C=_tmpB95;});_tmp64C;});_tmp64D->f2=_tmpB96;});_tmp64D;});Cyc_Absyn_new_vardecl(_tmpB98,_tmpB97,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2823
_tmp64E->f2=_tmpB99;});_tmp64E;});
# 2822
Cyc_Absyn_new_pat(_tmpB9A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2826
goto _LL0;}case 408U: _LL32B: _LL32C:
# 2829 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp64F=_cycalloc(sizeof(*_tmp64F));({struct Cyc_List_List*_tmpB9B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp64F->f1=_tmpB9B;}),_tmp64F->f2=0;_tmp64F;}));
goto _LL0;case 409U: _LL32D: _LL32E:
# 2830 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp650=_cycalloc(sizeof(*_tmp650));({struct Cyc_List_List*_tmpB9C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp650->f1=_tmpB9C;}),_tmp650->f2=1;_tmp650;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2831 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->f1=0,_tmp651->f2=1;_tmp651;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2836 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp652=_cycalloc(sizeof(*_tmp652));({struct Cyc_Absyn_Pat*_tmpB9D=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp652->hd=_tmpB9D;}),_tmp652->tl=0;_tmp652;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2838 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp653=_cycalloc(sizeof(*_tmp653));({struct Cyc_Absyn_Pat*_tmpB9F=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp653->hd=_tmpB9F;}),({struct Cyc_List_List*_tmpB9E=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp653->tl=_tmpB9E;});_tmp653;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2843 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->f1=0,({struct Cyc_Absyn_Pat*_tmpBA0=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp654->f2=_tmpBA0;});_tmp654;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2845 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp655=_cycalloc(sizeof(*_tmp655));({struct Cyc_List_List*_tmpBA2=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp655->f1=_tmpBA2;}),({struct Cyc_Absyn_Pat*_tmpBA1=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp655->f2=_tmpBA1;});_tmp655;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2848
 yyval=Cyc_YY14(({struct _tuple23*_tmp656=_cycalloc(sizeof(*_tmp656));({struct Cyc_List_List*_tmpBA3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp656->f1=_tmpBA3;}),_tmp656->f2=0;_tmp656;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2849 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp657=_cycalloc(sizeof(*_tmp657));({struct Cyc_List_List*_tmpBA4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp657->f1=_tmpBA4;}),_tmp657->f2=1;_tmp657;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2850 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->f1=0,_tmp658->f2=1;_tmp658;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2855 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp659=_cycalloc(sizeof(*_tmp659));({struct _tuple24*_tmpBA5=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp659->hd=_tmpBA5;}),_tmp659->tl=0;_tmp659;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2857 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp65A=_cycalloc(sizeof(*_tmp65A));({struct _tuple24*_tmpBA7=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp65A->hd=_tmpBA7;}),({struct Cyc_List_List*_tmpBA6=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp65A->tl=_tmpBA6;});_tmp65A;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2863 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 421U: _LL345: _LL346:
# 2865 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpBA9,_tmpBA8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422U: _LL347: _LL348:
# 2870 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 423U: _LL349: _LL34A:
# 2872 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpBAB=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpBAC,_tmpBAB,_tmpBAA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424U: _LL34B: _LL34C:
# 2874 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpBAE,_tmpBAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2878 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 426U: _LL34F: _LL350:
# 2879 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->v=(void*)Cyc_Absyn_Times;_tmp65B;}));
goto _LL0;case 427U: _LL351: _LL352:
# 2880 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->v=(void*)Cyc_Absyn_Div;_tmp65C;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2881 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->v=(void*)Cyc_Absyn_Mod;_tmp65D;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2882 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->v=(void*)Cyc_Absyn_Plus;_tmp65E;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2883 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->v=(void*)Cyc_Absyn_Minus;_tmp65F;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2884 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->v=(void*)Cyc_Absyn_Bitlshift;_tmp660;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2885 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->v=(void*)Cyc_Absyn_Bitlrshift;_tmp661;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2886 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->v=(void*)Cyc_Absyn_Bitand;_tmp662;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2887 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->v=(void*)Cyc_Absyn_Bitxor;_tmp663;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2888 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->v=(void*)Cyc_Absyn_Bitor;_tmp664;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2893 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437U: _LL365: _LL366:
# 2895 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpBB1,_tmpBB0,_tmpBAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438U: _LL367: _LL368:
# 2898
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpBB2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 439U: _LL369: _LL36A:
# 2901
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpBB3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2903 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpBB4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2905 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBB6,_tmpBB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2907 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpBB8,_tmpBB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2911 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444U: _LL373: _LL374:
# 2915 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2917 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpBBA,_tmpBB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 446U: _LL377: _LL378:
# 2921 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 447U: _LL379: _LL37A:
# 2923 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpBBC,_tmpBBB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 448U: _LL37B: _LL37C:
# 2927 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 449U: _LL37D: _LL37E:
# 2929 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpBBE,_tmpBBD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 450U: _LL37F: _LL380:
# 2933 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 451U: _LL381: _LL382:
# 2935 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBC0,_tmpBBF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 452U: _LL383: _LL384:
# 2939 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 453U: _LL385: _LL386:
# 2941 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBC2,_tmpBC1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 454U: _LL387: _LL388:
# 2945 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 455U: _LL389: _LL38A:
# 2947 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBC4,_tmpBC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 456U: _LL38B: _LL38C:
# 2949 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBC6,_tmpBC5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 457U: _LL38D: _LL38E:
# 2953 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 458U: _LL38F: _LL390:
# 2955 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBC8,_tmpBC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 459U: _LL391: _LL392:
# 2957 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBCA,_tmpBC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 460U: _LL393: _LL394:
# 2959 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBCC,_tmpBCB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2961 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBCE,_tmpBCD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2965 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 463U: _LL399: _LL39A:
# 2967 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBCF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBD0,_tmpBCF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464U: _LL39B: _LL39C:
# 2969 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBD2,_tmpBD1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2973 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2975 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBD4,_tmpBD3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2977 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBD6,_tmpBD5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 2981 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 2983 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBD8,_tmpBD7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 2985 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBDA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBD9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBDA,_tmpBD9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 2987 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBDC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBDB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBDC,_tmpBDB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 2991 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 473U: _LL3AD: _LL3AE: {
# 2993 "parse.y"
void*_tmp665=({struct _tuple8*_tmpBDD=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBDD,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp665;
yyval=Cyc_Exp_tok(({void*_tmpBDF=t;struct Cyc_Absyn_Exp*_tmpBDE=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBDF,_tmpBDE,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2996
goto _LL0;}case 474U: _LL3AF: _LL3B0:
# 2999 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 475U: _LL3B1: _LL3B2:
# 3000 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE0=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBE0,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476U: _LL3B3: _LL3B4:
# 3001 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE1=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBE1,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 3002 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBE2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 3003 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBE3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 3004 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBE4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 3005 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBE6=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBE5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBE6,_tmpBE5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE: {
# 3007 "parse.y"
void*_tmp666=({struct _tuple8*_tmpBE7=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBE7,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp666;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 3010
goto _LL0;}case 482U: _LL3BF: _LL3C0:
# 3010 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBE8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 483U: _LL3C1: _LL3C2: {
# 3012 "parse.y"
void*_tmp667=({struct _tuple8*_tmpBE9=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBE9,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp667;
yyval=Cyc_Exp_tok(({void*_tmpBEB=t;struct Cyc_List_List*_tmpBEA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBEB,_tmpBEA,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3015
goto _LL0;}case 484U: _LL3C3: _LL3C4:
# 3017
 yyval=Cyc_Exp_tok(({void*_tmpBED=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->tag=34U,(_tmp668->f1).is_calloc=0,(_tmp668->f1).rgn=0,(_tmp668->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBEC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp668->f1).num_elts=_tmpBEC;}),(_tmp668->f1).fat_result=0,(_tmp668->f1).inline_call=0;_tmp668;});Cyc_Absyn_new_exp(_tmpBED,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3019
goto _LL0;case 485U: _LL3C5: _LL3C6:
# 3020 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=34U,(_tmp669->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBEF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp669->f1).rgn=_tmpBEF;}),(_tmp669->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBEE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp669->f1).num_elts=_tmpBEE;}),(_tmp669->f1).fat_result=0,(_tmp669->f1).inline_call=0;_tmp669;});Cyc_Absyn_new_exp(_tmpBF0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3022
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 3023 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBF3=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->tag=34U,(_tmp66A->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBF2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp66A->f1).rgn=_tmpBF2;}),(_tmp66A->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp66A->f1).num_elts=_tmpBF1;}),(_tmp66A->f1).fat_result=0,(_tmp66A->f1).inline_call=1;_tmp66A;});Cyc_Absyn_new_exp(_tmpBF3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3025
goto _LL0;case 487U: _LL3C9: _LL3CA: {
# 3026 "parse.y"
void*_tmp66B=({struct _tuple8*_tmpBF4=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBF4,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp66B;
yyval=Cyc_Exp_tok(({void*_tmpBF7=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->tag=34U,(_tmp66D->f1).is_calloc=1,(_tmp66D->f1).rgn=0,({void**_tmpBF6=({void**_tmp66C=_cycalloc(sizeof(*_tmp66C));*_tmp66C=t;_tmp66C;});(_tmp66D->f1).elt_type=_tmpBF6;}),({struct Cyc_Absyn_Exp*_tmpBF5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp66D->f1).num_elts=_tmpBF5;}),(_tmp66D->f1).fat_result=0,(_tmp66D->f1).inline_call=0;_tmp66D;});Cyc_Absyn_new_exp(_tmpBF7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3029
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3031
void*_tmp66E=({struct _tuple8*_tmpBF8=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBF8,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp66E;
yyval=Cyc_Exp_tok(({void*_tmpBFC=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->tag=34U,(_tmp670->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBFB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp670->f1).rgn=_tmpBFB;}),({void**_tmpBFA=({void**_tmp66F=_cycalloc(sizeof(*_tmp66F));*_tmp66F=t;_tmp66F;});(_tmp670->f1).elt_type=_tmpBFA;}),({struct Cyc_Absyn_Exp*_tmpBF9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp670->f1).num_elts=_tmpBF9;}),(_tmp670->f1).fat_result=0,(_tmp670->f1).inline_call=0;_tmp670;});Cyc_Absyn_new_exp(_tmpBFC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3034
goto _LL0;}case 489U: _LL3CD: _LL3CE:
# 3035 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBFE=({struct Cyc_Absyn_Exp*_tmp671[1U];({struct Cyc_Absyn_Exp*_tmpBFD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp671[0]=_tmpBFD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp671,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBFE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 490U: _LL3CF: _LL3D0:
# 3037 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC02=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->tag=38U,({struct Cyc_Absyn_Exp*_tmpC01=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp673->f1=_tmpC01;}),({struct _fat_ptr*_tmpC00=({struct _fat_ptr*_tmp672=_cycalloc(sizeof(*_tmp672));({struct _fat_ptr _tmpBFF=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp672=_tmpBFF;});_tmp672;});_tmp673->f2=_tmpC00;});_tmp673;});Cyc_Absyn_new_exp(_tmpC02,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 3039 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC07=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=38U,({struct Cyc_Absyn_Exp*_tmpC06=({struct Cyc_Absyn_Exp*_tmpC05=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpC05,(unsigned)((yyyvsp[2]).l).first_line);});_tmp675->f1=_tmpC06;}),({struct _fat_ptr*_tmpC04=({struct _fat_ptr*_tmp674=_cycalloc(sizeof(*_tmp674));({struct _fat_ptr _tmpC03=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp674=_tmpC03;});_tmp674;});_tmp675->f2=_tmpC04;});_tmp675;});Cyc_Absyn_new_exp(_tmpC07,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4: {
# 3041 "parse.y"
void*_tmp676=({struct _tuple8*_tmpC08=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpC08,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp676;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 493U: _LL3D5: _LL3D6:
# 3044 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC09=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpC09,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 494U: _LL3D7: _LL3D8:
# 3045 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0A=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpC0A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA:
# 3047 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpC0B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC: {
# 3052 "parse.y"
struct _tuple29*_tmp677=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp30=_tmp677;struct _tuple29*_tmp678=_stmttmp30;struct Cyc_List_List*_tmp67B;struct Cyc_List_List*_tmp67A;struct Cyc_List_List*_tmp679;_LL4BA: _tmp679=_tmp678->f1;_tmp67A=_tmp678->f2;_tmp67B=_tmp678->f3;_LL4BB: {struct Cyc_List_List*outlist=_tmp679;struct Cyc_List_List*inlist=_tmp67A;struct Cyc_List_List*clobbers=_tmp67B;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->tag=40U,({int _tmpC0D=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp67C->f1=_tmpC0D;}),({struct _fat_ptr _tmpC0C=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp67C->f2=_tmpC0C;}),_tmp67C->f3=outlist,_tmp67C->f4=inlist,_tmp67C->f5=clobbers;_tmp67C;}));
goto _LL0;}}case 497U: _LL3DD: _LL3DE:
# 3057 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 498U: _LL3DF: _LL3E0:
# 3058 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 499U: _LL3E1: _LL3E2:
# 3062 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->f1=0,_tmp67D->f2=0,_tmp67D->f3=0;_tmp67D;}));
goto _LL0;case 500U: _LL3E3: _LL3E4: {
# 3064 "parse.y"
struct _tuple30*_tmp67E=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp31=_tmp67E;struct _tuple30*_tmp67F=_stmttmp31;struct Cyc_List_List*_tmp681;struct Cyc_List_List*_tmp680;_LL4BD: _tmp680=_tmp67F->f1;_tmp681=_tmp67F->f2;_LL4BE: {struct Cyc_List_List*inlist=_tmp680;struct Cyc_List_List*clobbers=_tmp681;
yyval=Cyc_YY59(({struct _tuple29*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->f1=0,_tmp682->f2=inlist,_tmp682->f3=clobbers;_tmp682;}));
goto _LL0;}}case 501U: _LL3E5: _LL3E6: {
# 3067 "parse.y"
struct _tuple30*_tmp683=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp32=_tmp683;struct _tuple30*_tmp684=_stmttmp32;struct Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp685;_LL4C0: _tmp685=_tmp684->f1;_tmp686=_tmp684->f2;_LL4C1: {struct Cyc_List_List*inlist=_tmp685;struct Cyc_List_List*clobbers=_tmp686;
yyval=Cyc_YY59(({struct _tuple29*_tmp687=_cycalloc(sizeof(*_tmp687));({struct Cyc_List_List*_tmpC0E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp687->f1=_tmpC0E;}),_tmp687->f2=inlist,_tmp687->f3=clobbers;_tmp687;}));
goto _LL0;}}case 502U: _LL3E7: _LL3E8:
# 3072 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp688=_cycalloc(sizeof(*_tmp688));({struct _tuple31*_tmpC0F=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp688->hd=_tmpC0F;}),_tmp688->tl=0;_tmp688;}));
goto _LL0;case 503U: _LL3E9: _LL3EA:
# 3073 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp689=_cycalloc(sizeof(*_tmp689));({struct _tuple31*_tmpC11=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp689->hd=_tmpC11;}),({struct Cyc_List_List*_tmpC10=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp689->tl=_tmpC10;});_tmp689;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 3077 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->f1=0,_tmp68A->f2=0;_tmp68A;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 3079 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B->f1=0,({struct Cyc_List_List*_tmpC12=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp68B->f2=_tmpC12;});_tmp68B;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 3081 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct Cyc_List_List*_tmpC14=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp68C->f1=_tmpC14;}),({struct Cyc_List_List*_tmpC13=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp68C->f2=_tmpC13;});_tmp68C;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 3085 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct _tuple31*_tmpC15=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp68D->hd=_tmpC15;}),_tmp68D->tl=0;_tmp68D;}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 3086 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp68E=_cycalloc(sizeof(*_tmp68E));({struct _tuple31*_tmpC17=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp68E->hd=_tmpC17;}),({struct Cyc_List_List*_tmpC16=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp68E->tl=_tmpC16;});_tmp68E;}));
goto _LL0;case 509U: _LL3F5: _LL3F6: {
# 3091 "parse.y"
struct Cyc_Absyn_Exp*_tmp68F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp68F;
yyval=Cyc_YY63(({struct _tuple31*_tmp690=_cycalloc(sizeof(*_tmp690));({struct _fat_ptr _tmpC19=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp690->f1=_tmpC19;}),({struct Cyc_Absyn_Exp*_tmpC18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp690->f2=_tmpC18;});_tmp690;}));
goto _LL0;}case 510U: _LL3F7: _LL3F8:
# 3097 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 511U: _LL3F9: _LL3FA:
# 3098 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3099 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3103 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp692=_cycalloc(sizeof(*_tmp692));({struct _fat_ptr*_tmpC1B=({struct _fat_ptr*_tmp691=_cycalloc(sizeof(*_tmp691));({struct _fat_ptr _tmpC1A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp691=_tmpC1A;});_tmp691;});_tmp692->hd=_tmpC1B;}),_tmp692->tl=0;_tmp692;}));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3104 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));({struct _fat_ptr*_tmpC1E=({struct _fat_ptr*_tmp693=_cycalloc(sizeof(*_tmp693));({struct _fat_ptr _tmpC1D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp693=_tmpC1D;});_tmp693;});_tmp694->hd=_tmpC1E;}),({struct Cyc_List_List*_tmpC1C=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp694->tl=_tmpC1C;});_tmp694;}));
goto _LL0;case 515U: _LL401: _LL402:
# 3108 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 516U: _LL403: _LL404:
# 3109 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 517U: _LL405: _LL406:
# 3110 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 518U: _LL407: _LL408:
# 3115 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 519U: _LL409: _LL40A:
# 3117 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC20=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpC1F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC20,_tmpC1F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520U: _LL40B: _LL40C:
# 3119 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC21=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC21,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3121 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC23=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC22=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC23,_tmpC22,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3123 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC26=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC25=({struct _fat_ptr*_tmp695=_cycalloc(sizeof(*_tmp695));({struct _fat_ptr _tmpC24=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp695=_tmpC24;});_tmp695;});Cyc_Absyn_aggrmember_exp(_tmpC26,_tmpC25,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3125 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC29=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC28=({struct _fat_ptr*_tmp696=_cycalloc(sizeof(*_tmp696));({struct _fat_ptr _tmpC27=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp696=_tmpC27;});_tmp696;});Cyc_Absyn_aggrarrow_exp(_tmpC29,_tmpC28,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3127 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC2A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC2A,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3129 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC2B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC2B,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3131 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC2D=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->tag=25U,({struct _tuple8*_tmpC2C=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp697->f1=_tmpC2C;}),_tmp697->f2=0;_tmp697;});Cyc_Absyn_new_exp(_tmpC2D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3133 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC30=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tag=25U,({struct _tuple8*_tmpC2F=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp698->f1=_tmpC2F;}),({struct Cyc_List_List*_tmpC2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp698->f2=_tmpC2E;});_tmp698;});Cyc_Absyn_new_exp(_tmpC30,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3135 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC33=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->tag=25U,({struct _tuple8*_tmpC32=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp699->f1=_tmpC32;}),({struct Cyc_List_List*_tmpC31=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp699->f2=_tmpC31;});_tmp699;});Cyc_Absyn_new_exp(_tmpC33,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3140 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp69C=_cycalloc(sizeof(*_tmp69C));({void*_tmpC36=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->tag=0U,({struct _fat_ptr*_tmpC35=({struct _fat_ptr*_tmp69A=_cycalloc(sizeof(*_tmp69A));({struct _fat_ptr _tmpC34=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp69A=_tmpC34;});_tmp69A;});_tmp69B->f1=_tmpC35;});_tmp69B;});_tmp69C->hd=_tmpC36;}),_tmp69C->tl=0;_tmp69C;}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3143
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));({void*_tmpC39=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->tag=1U,({unsigned _tmpC38=({unsigned _tmpC37=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC37,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp69D->f1=_tmpC38;});_tmp69D;});_tmp69E->hd=_tmpC39;}),_tmp69E->tl=0;_tmp69E;}));
goto _LL0;case 531U: _LL421: _LL422:
# 3145 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({void*_tmpC3D=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->tag=0U,({struct _fat_ptr*_tmpC3C=({struct _fat_ptr*_tmp69F=_cycalloc(sizeof(*_tmp69F));({struct _fat_ptr _tmpC3B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp69F=_tmpC3B;});_tmp69F;});_tmp6A0->f1=_tmpC3C;});_tmp6A0;});_tmp6A1->hd=_tmpC3D;}),({struct Cyc_List_List*_tmpC3A=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp6A1->tl=_tmpC3A;});_tmp6A1;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3148
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));({void*_tmpC41=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->tag=1U,({unsigned _tmpC40=({unsigned _tmpC3F=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC3F,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp6A2->f1=_tmpC40;});_tmp6A2;});_tmp6A3->hd=_tmpC41;}),({struct Cyc_List_List*_tmpC3E=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp6A3->tl=_tmpC3E;});_tmp6A3;}));
goto _LL0;case 533U: _LL425: _LL426:
# 3154 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC42=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC42,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534U: _LL427: _LL428:
# 3156 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC43=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC43,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535U: _LL429: _LL42A:
# 3158 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 536U: _LL42B: _LL42C:
# 3160 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC44=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC44,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 537U: _LL42D: _LL42E:
# 3162 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC45=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC45,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 538U: _LL42F: _LL430:
# 3164 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 539U: _LL431: _LL432:
# 3169 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC46=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC46,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 540U: _LL433: _LL434:
# 3171 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC48=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC47=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC48,_tmpC47,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 541U: _LL435: _LL436:
# 3174
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC49=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC49,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3177
 yyval=Cyc_Exp_tok(({void*_tmpC4D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->tag=29U,({struct _tuple0*_tmpC4C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6A4->f1=_tmpC4C;}),({struct Cyc_List_List*_tmpC4B=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp6A4->f2=_tmpC4B;}),({struct Cyc_List_List*_tmpC4A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp6A4->f3=_tmpC4A;}),_tmp6A4->f4=0;_tmp6A4;});Cyc_Absyn_new_exp(_tmpC4D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3180
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC4E=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC4E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3184 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3190 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));({struct Cyc_Absyn_Exp*_tmpC4F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp6A5->hd=_tmpC4F;}),_tmp6A5->tl=0;_tmp6A5;}));
goto _LL0;case 546U: _LL43F: _LL440:
# 3192 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));({struct Cyc_Absyn_Exp*_tmpC51=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6A6->hd=_tmpC51;}),({struct Cyc_List_List*_tmpC50=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp6A6->tl=_tmpC50;});_tmp6A6;}));
goto _LL0;case 547U: _LL441: _LL442:
# 3198 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC52=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC52,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 548U: _LL443: _LL444:
# 3199 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC53=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC53,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 549U: _LL445: _LL446:
# 3200 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC54=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC54,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 550U: _LL447: _LL448: {
# 3202 "parse.y"
struct _fat_ptr _tmp6A7=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp6A7;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3210
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3212
goto _LL0;}case 551U: _LL449: _LL44A:
# 3213 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 552U: _LL44B: _LL44C:
# 3217 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));({union Cyc_Absyn_Nmspace _tmpC57=Cyc_Absyn_Rel_n(0);_tmp6A9->f1=_tmpC57;}),({struct _fat_ptr*_tmpC56=({struct _fat_ptr*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));({struct _fat_ptr _tmpC55=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6A8=_tmpC55;});_tmp6A8;});_tmp6A9->f2=_tmpC56;});_tmp6A9;}));
goto _LL0;case 553U: _LL44D: _LL44E:
# 3218 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 554U: _LL44F: _LL450:
# 3221
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));({union Cyc_Absyn_Nmspace _tmpC5A=Cyc_Absyn_Rel_n(0);_tmp6AB->f1=_tmpC5A;}),({struct _fat_ptr*_tmpC59=({struct _fat_ptr*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));({struct _fat_ptr _tmpC58=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6AA=_tmpC58;});_tmp6AA;});_tmp6AB->f2=_tmpC59;});_tmp6AB;}));
goto _LL0;case 555U: _LL451: _LL452:
# 3222 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 556U: _LL453: _LL454:
# 3227 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 557U: _LL455: _LL456:
# 3228 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3231
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3232 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 560U: _LL45B: _LL45C:
# 3237 "parse.y"
 goto _LL0;case 561U: _LL45D: _LL45E:
# 3237 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL45F: _LL460:
# 3241
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
({int _tmpC5B=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC5B;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC5C=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 152));_tmpC5C + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
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
({unsigned long _tmpC5D=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,314U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC5D;}),count ++;}
msg=({unsigned _tmp6AD=(unsigned)(sze + 15)+ 1U;char*_tmp6AC=({struct _RegionHandle*_tmpC5E=yyregion;_region_malloc(_tmpC5E,_check_times(_tmp6AD,sizeof(char)));});({{unsigned _tmp776=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp776;++ i){_tmp6AC[i]='\000';}_tmp6AC[_tmp776]=0;}0;});_tag_fat(_tmp6AC,sizeof(char),_tmp6AD);});
({struct _fat_ptr _tmpC5F=msg;Cyc_strcpy(_tmpC5F,({const char*_tmp6AE="parse error";_tag_fat(_tmp6AE,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 314U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC60=msg;Cyc_strcat(_tmpC60,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp6AF=", expecting `";_tag_fat(_tmp6AF,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp6B0=" or `";_tag_fat(_tmp6B0,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,314U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC61=msg;Cyc_strcat(_tmpC61,({const char*_tmp6B1="'";_tag_fat(_tmp6B1,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC63=({const char*_tmp6B2="parse error";_tag_fat(_tmp6B2,sizeof(char),12U);});int _tmpC62=yystate;Cyc_yyerror(_tmpC63,_tmpC62,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp6B3=1;_npop_handler(0U);return _tmp6B3;}
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
 if(yyssp_offset == 0){int _tmp6B4=1;_npop_handler(0U);return _tmp6B4;}
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
int _tmp6B5=0;_npop_handler(0U);return _tmp6B5;}
# 546
({struct Cyc_Yystacktype _tmpC64=({struct Cyc_Yystacktype _tmp777;_tmp777.v=yylval,_tmp777.l=yylloc;_tmp777;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC64;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3240 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6B9=v;struct Cyc_Absyn_Stmt*_tmp6BA;struct Cyc_Absyn_Exp*_tmp6BB;struct _tuple0*_tmp6BC;struct _fat_ptr _tmp6BD;char _tmp6BE;union Cyc_Absyn_Cnst _tmp6BF;switch((_tmp6B9.Stmt_tok).tag){case 1U: _LL1: _tmp6BF=(_tmp6B9.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6BF;
({struct Cyc_String_pa_PrintArg_struct _tmp6C2=({struct Cyc_String_pa_PrintArg_struct _tmp77A;_tmp77A.tag=0U,({struct _fat_ptr _tmpC65=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp77A.f1=_tmpC65;});_tmp77A;});void*_tmp6C0[1U];_tmp6C0[0]=& _tmp6C2;({struct Cyc___cycFILE*_tmpC67=Cyc_stderr;struct _fat_ptr _tmpC66=({const char*_tmp6C1="%s";_tag_fat(_tmp6C1,sizeof(char),3U);});Cyc_fprintf(_tmpC67,_tmpC66,_tag_fat(_tmp6C0,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6BE=(_tmp6B9.Char_tok).val;_LL4: {char c=_tmp6BE;
({struct Cyc_Int_pa_PrintArg_struct _tmp6C5=({struct Cyc_Int_pa_PrintArg_struct _tmp77B;_tmp77B.tag=1U,_tmp77B.f1=(unsigned long)((int)c);_tmp77B;});void*_tmp6C3[1U];_tmp6C3[0]=& _tmp6C5;({struct Cyc___cycFILE*_tmpC69=Cyc_stderr;struct _fat_ptr _tmpC68=({const char*_tmp6C4="%c";_tag_fat(_tmp6C4,sizeof(char),3U);});Cyc_fprintf(_tmpC69,_tmpC68,_tag_fat(_tmp6C3,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6BD=(_tmp6B9.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6BD;
({struct Cyc_String_pa_PrintArg_struct _tmp6C8=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,_tmp77C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp77C;});void*_tmp6C6[1U];_tmp6C6[0]=& _tmp6C8;({struct Cyc___cycFILE*_tmpC6B=Cyc_stderr;struct _fat_ptr _tmpC6A=({const char*_tmp6C7="\"%s\"";_tag_fat(_tmp6C7,sizeof(char),5U);});Cyc_fprintf(_tmpC6B,_tmpC6A,_tag_fat(_tmp6C6,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6BC=(_tmp6B9.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6BC;
({struct Cyc_String_pa_PrintArg_struct _tmp6CB=({struct Cyc_String_pa_PrintArg_struct _tmp77D;_tmp77D.tag=0U,({struct _fat_ptr _tmpC6C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp77D.f1=_tmpC6C;});_tmp77D;});void*_tmp6C9[1U];_tmp6C9[0]=& _tmp6CB;({struct Cyc___cycFILE*_tmpC6E=Cyc_stderr;struct _fat_ptr _tmpC6D=({const char*_tmp6CA="%s";_tag_fat(_tmp6CA,sizeof(char),3U);});Cyc_fprintf(_tmpC6E,_tmpC6D,_tag_fat(_tmp6C9,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6BB=(_tmp6B9.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6BB;
({struct Cyc_String_pa_PrintArg_struct _tmp6CE=({struct Cyc_String_pa_PrintArg_struct _tmp77E;_tmp77E.tag=0U,({struct _fat_ptr _tmpC6F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp77E.f1=_tmpC6F;});_tmp77E;});void*_tmp6CC[1U];_tmp6CC[0]=& _tmp6CE;({struct Cyc___cycFILE*_tmpC71=Cyc_stderr;struct _fat_ptr _tmpC70=({const char*_tmp6CD="%s";_tag_fat(_tmp6CD,sizeof(char),3U);});Cyc_fprintf(_tmpC71,_tmpC70,_tag_fat(_tmp6CC,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6BA=(_tmp6B9.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6BA;
({struct Cyc_String_pa_PrintArg_struct _tmp6D1=({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0U,({struct _fat_ptr _tmpC72=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp77F.f1=_tmpC72;});_tmp77F;});void*_tmp6CF[1U];_tmp6CF[0]=& _tmp6D1;({struct Cyc___cycFILE*_tmpC74=Cyc_stderr;struct _fat_ptr _tmpC73=({const char*_tmp6D0="%s";_tag_fat(_tmp6D0,sizeof(char),3U);});Cyc_fprintf(_tmpC74,_tmpC73,_tag_fat(_tmp6CF,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6D2=0U;({struct Cyc___cycFILE*_tmpC76=Cyc_stderr;struct _fat_ptr _tmpC75=({const char*_tmp6D3="?";_tag_fat(_tmp6D3,sizeof(char),2U);});Cyc_fprintf(_tmpC76,_tmpC75,_tag_fat(_tmp6D2,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3252
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6D4="end-of-file";_tag_fat(_tmp6D4,sizeof(char),12U);});
if(token == 367)
return Cyc_Lex_token_string;
if(token == 376)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 379?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,380U,sizeof(short),token)): 314;
if((unsigned)z < 314U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 3266
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6D5=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6D5;_push_region(yyr);
({struct _RegionHandle*_tmpC77=yyr;Cyc_yyparse(_tmpC77,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6D6=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6D6;}
# 3269
;_pop_region();}}
