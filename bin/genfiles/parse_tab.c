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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
# 78 "lexing.h"
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 52
int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 54
int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 60
struct _fat_ptr Cyc_strcat(struct _fat_ptr dest,struct _fat_ptr src);
# 71
struct _fat_ptr Cyc_strcpy(struct _fat_ptr dest,struct _fat_ptr src);
# 109 "string.h"
struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);struct Cyc_Position_Error;
# 28 "warn.h"
void Cyc_Warn_warn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 32
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 34
void Cyc_Warn_err(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 301
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 314
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 462
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 469
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
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 864
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 893
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 915
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 918
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 920
extern void*Cyc_Absyn_sint_type;
# 922
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 925
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 929
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 931
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 956
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 960
void*Cyc_Absyn_bounds_one();
# 962
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 986
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr body,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1062
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
# 1067
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1086
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
# 1095
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1097
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1104
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc);
# 1118
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc);
# 1124
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1153
int Cyc_Absyn_fntype_att(void*);
# 1175
extern int Cyc_Absyn_porting_c_code;struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 45 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 107
void*Cyc_Tcutil_compress(void*);
# 134
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 153
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 155
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 157
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 162
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 172
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 269
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 309
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 319
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 66 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 94 "parse.y"
int Cyc_Parse_parsing_tempest=0;struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 100
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 103
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 112
return first;}}
# 117
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};
# 134
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 170
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 174
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
declarators=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators,_tmp2->hd=_tmp5;_tmp2;});
exprs=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4,_tmp3->tl=exprs;_tmp3;});}
# 181
({struct Cyc_List_List*_tmp71B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp71B;});
({struct _tuple10*_tmp71C=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp71C;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 191
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
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
struct _tuple8*_tmp7=tqt;struct Cyc_Absyn_Tqual _tmpB;void*_tmpA;_LL1: _tmpB=_tmp7->f2;_tmpA=_tmp7->f3;_LL2:;
if((_tmpB.print_const || _tmpB.q_volatile)|| _tmpB.q_restrict){
if(_tmpB.loc != (unsigned)0)loc=_tmpB.loc;
({void*_tmp8=0U;({unsigned _tmp71E=loc;struct _fat_ptr _tmp71D=({const char*_tmp9="qualifier on type is ignored";_tag_fat(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp71E,_tmp71D,_tag_fat(_tmp8,sizeof(void*),0U));});});}
# 212
return _tmpA;}struct _tuple14{void*f1;void*f2;void*f3;void*f4;};
# 215
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 221
void*zeroterm=Cyc_Tcutil_any_bool(0);
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_tmpD=_tmpC;void*_tmpF;struct Cyc_Absyn_Exp*_tmpE;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7U: _LL5: _LL6:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 6U: _LL7: _LL8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3U: _LL9: _LLA:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2U: _LLB: _LLC:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0U: _LLD: _tmpE=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LLE:
 bound=Cyc_Absyn_thin_bounds_exp(_tmpE);goto _LL0;default: _LLF: _tmpF=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10:
 rgn=_tmpF;goto _LL0;}_LL0:;}
# 233
return({struct _tuple14 _tmp683;_tmp683.f1=nullable,_tmp683.f2=bound,_tmp683.f3=zeroterm,_tmp683.f4=rgn;_tmp683;});}
# 239
struct _tuple0*Cyc_Parse_gensym_enum(){
# 241
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp722=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp722;}),({
struct _fat_ptr*_tmp721=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp720=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp684;_tmp684.tag=1U,_tmp684.f1=(unsigned long)enum_counter ++;_tmp684;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp71F=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp71F,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp720;});_tmp13;});_tmp14->f2=_tmp721;});_tmp14;});}struct _tuple15{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){
# 251
struct _tuple17*_tmp15=field_info;unsigned _tmp22;struct _tuple0*_tmp21;struct Cyc_Absyn_Tqual _tmp20;void*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;_LL1: _tmp22=(_tmp15->f1)->f1;_tmp21=(_tmp15->f1)->f2;_tmp20=(_tmp15->f1)->f3;_tmp1F=(_tmp15->f1)->f4;_tmp1E=(_tmp15->f1)->f5;_tmp1D=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1B=(_tmp15->f2)->f2;_LL2:;
if(_tmp1E != 0)
({void*_tmp16=0U;({unsigned _tmp724=loc;struct _fat_ptr _tmp723=({const char*_tmp17="bad type params in struct field";_tag_fat(_tmp17,sizeof(char),32U);});Cyc_Warn_err(_tmp724,_tmp723,_tag_fat(_tmp16,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp21))
({void*_tmp18=0U;({unsigned _tmp726=loc;struct _fat_ptr _tmp725=({const char*_tmp19="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp19,sizeof(char),59U);});Cyc_Warn_err(_tmp726,_tmp725,_tag_fat(_tmp18,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->name=(*_tmp21).f2,_tmp1A->tq=_tmp20,_tmp1A->type=_tmp1F,_tmp1A->width=_tmp1C,_tmp1A->attributes=_tmp1D,_tmp1A->requires_clause=_tmp1B;_tmp1A;});}
# 261
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp685;_tmp685.Signed_spec=0,_tmp685.Unsigned_spec=0,_tmp685.Short_spec=0,_tmp685.Long_spec=0,_tmp685.Long_Long_spec=0,_tmp685.Valid_type_spec=0,_tmp685.Type_spec=Cyc_Absyn_sint_type,_tmp685.loc=loc;_tmp685;});}
# 272
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);
_tmp23.Type_spec=t;
_tmp23.Valid_type_spec=1;
return _tmp23;}
# 278
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);
_tmp24.Signed_spec=1;
return _tmp24;}
# 283
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);
_tmp25.Unsigned_spec=1;
return _tmp25;}
# 288
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);
_tmp26.Short_spec=1;
return _tmp26;}
# 293
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);
_tmp27.Long_spec=1;
return _tmp27;}
# 300
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 302
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp727=t;Cyc_Tcutil_promote_array(_tmp727,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple18{struct _fat_ptr*f1;void*f2;};
# 315 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_tmp2A=_tmp29;struct _fat_ptr _tmp3E;void**_tmp3D;struct _fat_ptr*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp3C=_tmp2A->f1;_tmp3B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp3C != 0){_LL2:
# 320
{void*_tmp2B=_tmp3B;void**_tmp32;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->tag == 1U){_LL8: _tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {
# 324
struct _fat_ptr*nm=({struct _fat_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _fat_ptr _tmp729=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp686;_tmp686.tag=0U,_tmp686.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp3C);_tmp686;});void*_tmp2E[1U];_tmp2E[0]=& _tmp30;({struct _fat_ptr _tmp728=({const char*_tmp2F="`%s";_tag_fat(_tmp2F,sizeof(char),4U);});Cyc_aprintf(_tmp728,_tag_fat(_tmp2E,sizeof(void*),1U));});});*_tmp31=_tmp729;});_tmp31;});
({void*_tmp72B=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=nm,_tmp2D->identity=- 1,({void*_tmp72A=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=& Cyc_Tcutil_ik;_tmp2C;});_tmp2D->kind=_tmp72A;});_tmp2D;}));*_tmp32=_tmp72B;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 329
_tmp28=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple18*_tmp72C=({struct _tuple18*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=_tmp3C,_tmp33->f2=_tmp3B;_tmp33;});_tmp34->hd=_tmp72C;}),_tmp34->tl=_tmp28;_tmp34;});goto _LL0;}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp3E=*_tmp2A->f1;_tmp3D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {
# 333
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct _fat_ptr _tmp72E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp687;_tmp687.tag=0U,_tmp687.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp3E);_tmp687;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({struct _fat_ptr _tmp72D=({const char*_tmp38="`%s";_tag_fat(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp72D,_tag_fat(_tmp37,sizeof(void*),1U));});});*_tmp3A=_tmp72E;});_tmp3A;});
({void*_tmp730=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->name=nm,_tmp36->identity=- 1,({void*_tmp72F=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=0U,_tmp35->f1=& Cyc_Tcutil_rk;_tmp35;});_tmp36->kind=_tmp72F;});_tmp36;}));*_tmp3D=_tmp730;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 339
return _tmp28;}
# 343
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp41=_tmp40;void*_tmp44;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp44=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2:
# 348
 _tmp3F=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple18*_tmp731=({struct _tuple18*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp42->f2=_tmp44;_tmp42;});_tmp43->hd=_tmp731;}),_tmp43->tl=_tmp3F;_tmp43;});goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 352
return _tmp3F;}
# 356
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp45=e->r;void*_tmp46=_tmp45;struct _fat_ptr*_tmp4D;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp4D=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f2;_LL2:
# 359
{struct Cyc_List_List*_tmp47=tags;for(0;_tmp47 != 0;_tmp47=_tmp47->tl){
struct _tuple18*_tmp48=(struct _tuple18*)_tmp47->hd;struct _tuple18*_tmp49=_tmp48;struct _fat_ptr*_tmp4C;void*_tmp4B;_LL6: _tmp4C=_tmp49->f1;_tmp4B=_tmp49->f2;_LL7:;
if(Cyc_strptrcmp(_tmp4C,_tmp4D)== 0)
return({void*_tmp733=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=39U,({void*_tmp732=Cyc_Tcutil_copy_type(_tmp4B);_tmp4A->f1=_tmp732;});_tmp4A;});Cyc_Absyn_new_exp(_tmp733,e->loc);});}}
# 364
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 367
return e;}
# 372
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp61;void*_tmp60;void*_tmp5F;struct Cyc_Absyn_Tqual _tmp5E;void*_tmp5D;void*_tmp5C;void*_tmp5B;void*_tmp5A;struct Cyc_Absyn_PtrLoc*_tmp59;void*_tmp58;struct Cyc_Absyn_Tqual _tmp57;struct Cyc_Absyn_Exp*_tmp56;void*_tmp55;unsigned _tmp54;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {
# 375
struct Cyc_Absyn_Exp*nelts2=_tmp56;
if(_tmp56 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp56);{
# 379
void*_tmp4F=Cyc_Parse_substitute_tags(tags,_tmp58);
if(_tmp56 != nelts2 || _tmp58 != _tmp4F)
return Cyc_Absyn_array_type(_tmp4F,_tmp57,nelts2,_tmp55,_tmp54);
goto _LL0;};}case 3U: _LL3: _tmp5F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp5E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp5B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp59=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_LL4: {
# 384
void*_tmp50=Cyc_Parse_substitute_tags(tags,_tmp5F);
void*_tmp51=Cyc_Parse_substitute_tags(tags,_tmp5B);
if(_tmp50 != _tmp5F || _tmp51 != _tmp5B)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp688;_tmp688.elt_type=_tmp50,_tmp688.elt_tq=_tmp5E,(_tmp688.ptr_atts).rgn=_tmp5D,(_tmp688.ptr_atts).nullable=_tmp5C,(_tmp688.ptr_atts).bounds=_tmp51,(_tmp688.ptr_atts).zero_term=_tmp5A,(_tmp688.ptr_atts).ptrloc=_tmp59;_tmp688;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp60=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {
# 390
void*_tmp52=Cyc_Parse_substitute_tags(tags,_tmp60);
if(_tmp60 != _tmp52)return Cyc_Absyn_thin_bounds_type(_tmp52);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {
# 394
struct Cyc_Absyn_Exp*_tmp53=Cyc_Parse_substitute_tags_exp(tags,_tmp61);
if(_tmp53 != _tmp61)return Cyc_Absyn_valueof_type(_tmp53);
goto _LL0;}default: _LL9: _LLA:
# 400
 goto _LL0;}_LL0:;}
# 402
return t;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp734=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp734;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple8*t){
# 416
return({struct _tuple19*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=(*t).f2,_tmp62->f2=(*t).f3;_tmp62;});}
# 419
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp63=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp63)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 428
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp735=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp735,({const char*_tmp64="`H";_tag_fat(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _fat_ptr _tmp736=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp736,({const char*_tmp65="`U";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _fat_ptr _tmp737=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp737,({const char*_tmp66="`RC";_tag_fat(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 436
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr*_tmp738=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp738;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 443
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2:
 return _tmp6C;}else{_LL3: _LL4:
({void*_tmp6A=0U;({unsigned _tmp73A=loc;struct _fat_ptr _tmp739=({const char*_tmp6B="expecting a list of type variables, not types";_tag_fat(_tmp6B,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp73A,_tmp739,_tag_fat(_tmp6A,sizeof(void*),0U));});});}_LL0:;}
# 451
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp6E=_tmp6D;void**_tmp72;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp72=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {
# 454
void*_tmp6F=Cyc_Absyn_compress_kb(*_tmp72);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp70)->tag == 1U){_LL6: _LL7:
# 456
 if(!leq)({void*_tmp73B=Cyc_Tcutil_kind_to_bound(k);*_tmp72=_tmp73B;});else{
({void*_tmp73C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2U,_tmp71->f1=0,_tmp71->f2=k;_tmp71;});*_tmp72=_tmp73C;});}
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 461
 return;}_LL0:;}
# 466
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 472
if(tds == 0)return tms;
# 477
if(tms == 0)return 0;{
# 479
void*_tmp73=(void*)tms->hd;void*_tmp74=_tmp73;void*_tmp8F;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->tag == 3U){_LL1: _tmp8F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->f1;_LL2:
# 482
 if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 485
void*_tmp75=_tmp8F;struct Cyc_List_List*_tmp8D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp75)->tag == 1U){_LL6: _LL7:
# 487
({void*_tmp76=0U;({unsigned _tmp73E=loc;struct _fat_ptr _tmp73D=({const char*_tmp77="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp77,sizeof(char),93U);});Cyc_Warn_warn(_tmp73E,_tmp73D,_tag_fat(_tmp76,sizeof(void*),0U));});});
# 489
return tms;}else{_LL8: _tmp8D=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;_LL9:
# 491
 if(({int _tmp73F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp73F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp78=0U;({unsigned _tmp741=loc;struct _fat_ptr _tmp740=({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp79,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp741,_tmp740,_tag_fat(_tmp78,sizeof(void*),0U));});});{
# 495
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
struct Cyc_List_List*_tmp7A=tds;
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
struct Cyc_Absyn_Decl*_tmp7B=(struct Cyc_Absyn_Decl*)_tmp7A->hd;
void*_tmp7C=_tmp7B->r;void*_tmp7D=_tmp7C;struct Cyc_Absyn_Vardecl*_tmp86;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->tag == 0U){_LLB: _tmp86=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LLC:
# 502
 if(Cyc_zstrptrcmp((*_tmp86->name).f2,(struct _fat_ptr*)_tmp8D->hd)!= 0)
continue;
if(_tmp86->initializer != 0)
({void*_tmp7E=0U;({unsigned _tmp743=_tmp7B->loc;struct _fat_ptr _tmp742=({const char*_tmp7F="initializer found in parameter declaration";_tag_fat(_tmp7F,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp743,_tmp742,_tag_fat(_tmp7E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp86->name))
({void*_tmp80=0U;({unsigned _tmp745=_tmp7B->loc;struct _fat_ptr _tmp744=({const char*_tmp81="namespaces forbidden in parameter declarations";_tag_fat(_tmp81,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp745,_tmp744,_tag_fat(_tmp80,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));
({struct _tuple8*_tmp746=({struct _tuple8*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(*_tmp86->name).f2,_tmp82->f2=_tmp86->tq,_tmp82->f3=_tmp86->type;_tmp82;});_tmp83->hd=_tmp746;}),_tmp83->tl=rev_new_params;_tmp83;});
# 511
goto L;}else{_LLD: _LLE:
({void*_tmp84=0U;({unsigned _tmp748=_tmp7B->loc;struct _fat_ptr _tmp747=({const char*_tmp85="nonvariable declaration in parameter type";_tag_fat(_tmp85,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp748,_tmp747,_tag_fat(_tmp84,sizeof(void*),0U));});});}_LLA:;}
# 515
L: if(_tmp7A == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp689;_tmp689.tag=0U,_tmp689.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_tmp8D->hd));_tmp689;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned _tmp74A=loc;struct _fat_ptr _tmp749=({const char*_tmp88="%s is not given a type";_tag_fat(_tmp88,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp74A,_tmp749,_tag_fat(_tmp87,sizeof(void*),1U));});});}
# 518
return({struct Cyc_List_List*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));
({void*_tmp74D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8B=_region_malloc(yy,sizeof(*_tmp8B));_tmp8B->tag=3U,({void*_tmp74C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8A=_region_malloc(yy,sizeof(*_tmp8A));_tmp8A->tag=1U,({struct Cyc_List_List*_tmp74B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8A->f1=_tmp74B;}),_tmp8A->f2=0,_tmp8A->f3=0,_tmp8A->f4=0,_tmp8A->f5=0,_tmp8A->f6=0,_tmp8A->f7=0;_tmp8A;});_tmp8B->f1=_tmp74C;});_tmp8B;});_tmp8C->hd=_tmp74D;}),_tmp8C->tl=0;_tmp8C;});};}_LL5:;}
# 525
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));_tmp8E->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp74E=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8E->tl=_tmp74E;});_tmp8E;});}_LL0:;};}
# 533
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 537
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp68A;_tmp68A.id=d.id,_tmp68A.varloc=d.varloc,({struct Cyc_List_List*_tmp74F=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp68A.tms=_tmp74F;});_tmp68A;});{
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
atts=dso->attributes;
# 552
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmp90=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmp91=_tmp90;switch(_tmp91){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=0U;goto _LL0;default: _LL5: _LL6:
({void*_tmp92=0U;({unsigned _tmp751=loc;struct _fat_ptr _tmp750=({const char*_tmp93="bad storage class on function";_tag_fat(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp751,_tmp750,_tag_fat(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 559
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp95=Cyc_Parse_apply_tms(tq,_tmp94,atts,d.tms);struct _tuple13 _tmp96=_tmp95;struct Cyc_Absyn_Tqual _tmpA6;void*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA3;_LL8: _tmpA6=_tmp96.f1;_tmpA5=_tmp96.f2;_tmpA4=_tmp96.f3;_tmpA3=_tmp96.f4;_LL9:;
# 564
if(_tmpA4 != 0)
# 567
({void*_tmp97=0U;({unsigned _tmp753=loc;struct _fat_ptr _tmp752=({const char*_tmp98="bad type params, ignoring";_tag_fat(_tmp98,sizeof(char),26U);});Cyc_Warn_warn(_tmp753,_tmp752,_tag_fat(_tmp97,sizeof(void*),0U));});});{
# 569
void*_tmp99=_tmpA5;struct Cyc_Absyn_FnInfo _tmpA2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->tag == 5U){_LLB: _tmpA2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1;_LLC:
# 571
{struct Cyc_List_List*_tmp9A=_tmpA2.args;for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
if((*((struct _tuple8*)_tmp9A->hd)).f1 == 0){
({void*_tmp9B=0U;({unsigned _tmp755=loc;struct _fat_ptr _tmp754=({const char*_tmp9C="missing argument variable in function prototype";_tag_fat(_tmp9C,sizeof(char),48U);});Cyc_Warn_err(_tmp755,_tmp754,_tag_fat(_tmp9B,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp757=({struct _fat_ptr*_tmp9E=_cycalloc(sizeof(*_tmp9E));({struct _fat_ptr _tmp756=({const char*_tmp9D="?";_tag_fat(_tmp9D,sizeof(char),2U);});*_tmp9E=_tmp756;});_tmp9E;});(*((struct _tuple8*)_tmp9A->hd)).f1=_tmp757;});}}}
# 578
({struct Cyc_List_List*_tmp758=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA2.attributes,_tmpA3);_tmpA2.attributes=_tmp758;});
return({struct Cyc_Absyn_Fndecl*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->sc=sc,_tmp9F->is_inline=is_inline,_tmp9F->name=d.id,_tmp9F->body=body,_tmp9F->i=_tmpA2,_tmp9F->cached_type=0,_tmp9F->param_vardecls=0,_tmp9F->fn_vardecl=0;_tmp9F;});}else{_LLD: _LLE:
# 582
({void*_tmpA0=0U;({unsigned _tmp75A=loc;struct _fat_ptr _tmp759=({const char*_tmpA1="declarator is not a function prototype";_tag_fat(_tmpA1,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp75A,_tmp759,_tag_fat(_tmpA0,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpA7[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 586
static struct _fat_ptr Cyc_Parse_msg1={_tmpA7,_tmpA7,_tmpA7 + 76U};static char _tmpA8[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 588
static struct _fat_ptr Cyc_Parse_msg2={_tmpA8,_tmpA8,_tmpA8 + 87U};static char _tmpA9[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 590
static struct _fat_ptr Cyc_Parse_msg3={_tmpA9,_tmpA9,_tmpA9 + 74U};static char _tmpAA[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 592
static struct _fat_ptr Cyc_Parse_msg4={_tmpAA,_tmpAA,_tmpAA + 84U};
# 599
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 602
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpAB=0U;({unsigned _tmp75C=loc;struct _fat_ptr _tmp75B=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp75C,_tmp75B,_tag_fat(_tmpAB,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpAC=0U;({unsigned _tmp75E=loc;struct _fat_ptr _tmp75D=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp75E,_tmp75D,_tag_fat(_tmpAC,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpAD=0U;({unsigned _tmp760=loc;struct _fat_ptr _tmp75F=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp760,_tmp75F,_tag_fat(_tmpAD,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpAE=0U;({unsigned _tmp762=loc;struct _fat_ptr _tmp761=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp762,_tmp761,_tag_fat(_tmpAE,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpAF=0U;({unsigned _tmp764=loc;struct _fat_ptr _tmp763=Cyc_Parse_msg1;Cyc_Warn_err(_tmp764,_tmp763,_tag_fat(_tmpAF,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 624
return s1;}
# 630
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
# 633
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 640
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpB0=0U;({unsigned _tmp766=loc;struct _fat_ptr _tmp765=Cyc_Parse_msg4;Cyc_Warn_err(_tmp766,_tmp765,_tag_fat(_tmpB0,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpB1=0U;({unsigned _tmp768=loc;struct _fat_ptr _tmp767=Cyc_Parse_msg4;Cyc_Warn_err(_tmp768,_tmp767,_tag_fat(_tmpB1,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 652
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpB2=0U;({unsigned _tmp76A=loc;struct _fat_ptr _tmp769=({const char*_tmpB3="missing type within specifier";_tag_fat(_tmpB3,sizeof(char),30U);});Cyc_Warn_warn(_tmp76A,_tmp769,_tag_fat(_tmpB2,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 657
if(seen_sign){
void*_tmpB4=t;enum Cyc_Absyn_Sign _tmpB8;enum Cyc_Absyn_Size_of _tmpB7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->tag == 1U){_LL1: _tmpB8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f1;_tmpB7=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f2;_LL2:
# 660
 if((int)_tmpB8 != (int)sgn)
t=Cyc_Absyn_int_type(sgn,_tmpB7);
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpB5=0U;({unsigned _tmp76C=loc;struct _fat_ptr _tmp76B=({const char*_tmpB6="sign specification on non-integral type";_tag_fat(_tmpB6,sizeof(char),40U);});Cyc_Warn_err(_tmp76C,_tmp76B,_tag_fat(_tmpB5,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 665
if(seen_size){
void*_tmpB9=t;enum Cyc_Absyn_Sign _tmpBD;enum Cyc_Absyn_Size_of _tmpBC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)){case 1U: _LL6: _tmpBD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f1;_tmpBC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f2;_LL7:
# 668
 if((int)_tmpBC != (int)sz)
t=Cyc_Absyn_int_type(_tmpBD,sz);
goto _LL5;case 2U: _LL8: _LL9:
# 672
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpBA=0U;({unsigned _tmp76E=loc;struct _fat_ptr _tmp76D=({const char*_tmpBB="size qualifier on non-integral type";_tag_fat(_tmpBB,sizeof(char),36U);});Cyc_Warn_err(_tmp76E,_tmp76D,_tag_fat(_tmpBA,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 676
return t;}
# 679
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 683
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpBE=d.id;
unsigned _tmpBF=d.varloc;
struct _tuple13 _tmpC0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpC1=_tmpC0;struct Cyc_Absyn_Tqual _tmpC9;void*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC6;_LL1: _tmpC9=_tmpC1.f1;_tmpC8=_tmpC1.f2;_tmpC7=_tmpC1.f3;_tmpC6=_tmpC1.f4;_LL2:;
# 690
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpC3=_region_malloc(r,sizeof(*_tmpC3));({struct _tuple15*_tmp76F=({struct _tuple15*_tmpC2=_region_malloc(r,sizeof(*_tmpC2));_tmpC2->f1=_tmpBF,_tmpC2->f2=_tmpBE,_tmpC2->f3=_tmpC9,_tmpC2->f4=_tmpC8,_tmpC2->f5=_tmpC7,_tmpC2->f6=_tmpC6;_tmpC2;});_tmpC3->hd=_tmp76F;}),_tmpC3->tl=0;_tmpC3;});else{
# 693
return({struct Cyc_List_List*_tmpC5=_region_malloc(r,sizeof(*_tmpC5));({struct _tuple15*_tmp775=({struct _tuple15*_tmpC4=_region_malloc(r,sizeof(*_tmpC4));_tmpC4->f1=_tmpBF,_tmpC4->f2=_tmpBE,_tmpC4->f3=_tmpC9,_tmpC4->f4=_tmpC8,_tmpC4->f5=_tmpC7,_tmpC4->f6=_tmpC6;_tmpC4;});_tmpC5->hd=_tmp775;}),({
struct Cyc_List_List*_tmp774=({struct _RegionHandle*_tmp773=r;struct Cyc_Absyn_Tqual _tmp772=tq;void*_tmp771=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp770=ds->tl;Cyc_Parse_apply_tmss(_tmp773,_tmp772,_tmp771,_tmp770,shared_atts);});_tmpC5->tl=_tmp774;});_tmpC5;});}};}
# 697
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 700
if(tms == 0)return({struct _tuple13 _tmp68B;_tmp68B.f1=tq,_tmp68B.f2=t,_tmp68B.f3=0,_tmp68B.f4=atts;_tmp68B;});{
void*_tmpCA=(void*)tms->hd;void*_tmpCB=_tmpCA;unsigned _tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_PtrAtts _tmpEE;struct Cyc_Absyn_Tqual _tmpED;struct Cyc_List_List*_tmpEC;unsigned _tmpEB;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpE8;unsigned _tmpE7;void*_tmpE6;unsigned _tmpE5;switch(*((int*)_tmpCB)){case 0U: _LL1: _tmpE6=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpE5=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL2:
# 703
 return({struct Cyc_Absyn_Tqual _tmp778=Cyc_Absyn_empty_tqual(0U);void*_tmp777=
Cyc_Absyn_array_type(t,tq,0,_tmpE6,_tmpE5);
# 703
struct Cyc_List_List*_tmp776=atts;Cyc_Parse_apply_tms(_tmp778,_tmp777,_tmp776,tms->tl);});case 1U: _LL3: _tmpE9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpE8=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_tmpE7=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f3;_LL4:
# 706
 return({struct Cyc_Absyn_Tqual _tmp77B=Cyc_Absyn_empty_tqual(0U);void*_tmp77A=
Cyc_Absyn_array_type(t,tq,_tmpE9,_tmpE8,_tmpE7);
# 706
struct Cyc_List_List*_tmp779=atts;Cyc_Parse_apply_tms(_tmp77B,_tmp77A,_tmp779,tms->tl);});case 3U: _LL5: _tmpEA=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_LL6: {
# 709
void*_tmpCC=_tmpEA;unsigned _tmpE2;struct Cyc_List_List*_tmpE1;int _tmpE0;struct Cyc_Absyn_VarargInfo*_tmpDF;void*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->tag == 1U){_LLE: _tmpE1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f1;_tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f3;_tmpDE=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f4;_tmpDD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f5;_tmpDC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f6;_tmpDB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f7;_LLF: {
# 711
struct Cyc_List_List*typvars=0;
# 713
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)as->hd,_tmpCD->tl=fn_atts;_tmpCD;});else{
# 718
new_atts=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=(void*)as->hd,_tmpCE->tl=new_atts;_tmpCE;});}}}
# 721
if(tms->tl != 0){
void*_tmpCF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpD0=_tmpCF;struct Cyc_List_List*_tmpD1;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->tag == 4U){_LL13: _tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->f1;_LL14:
# 724
 typvars=_tmpD1;
tms=tms->tl;
goto _LL12;}else{_LL15: _LL16:
# 728
 goto _LL12;}_LL12:;}
# 732
if(((((!_tmpE0 && _tmpDF == 0)&& _tmpE1 != 0)&& _tmpE1->tl == 0)&&(*((struct _tuple8*)_tmpE1->hd)).f1 == 0)&&(*((struct _tuple8*)_tmpE1->hd)).f3 == Cyc_Absyn_void_type)
# 737
_tmpE1=0;{
# 740
struct Cyc_List_List*_tmpD2=Cyc_Parse_get_arg_tags(_tmpE1);
# 742
if(_tmpD2 != 0)
t=Cyc_Parse_substitute_tags(_tmpD2,t);
t=Cyc_Parse_array2ptr(t,0);
# 747
{struct Cyc_List_List*_tmpD3=_tmpE1;for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
struct _tuple8*_tmpD4=(struct _tuple8*)_tmpD3->hd;struct _tuple8*_tmpD5=_tmpD4;struct _fat_ptr*_tmpD8;struct Cyc_Absyn_Tqual _tmpD7;void**_tmpD6;_LL18: _tmpD8=_tmpD5->f1;_tmpD7=_tmpD5->f2;_tmpD6=(void**)& _tmpD5->f3;_LL19:;
if(_tmpD2 != 0)
({void*_tmp77C=Cyc_Parse_substitute_tags(_tmpD2,*_tmpD6);*_tmpD6=_tmp77C;});
({void*_tmp77D=Cyc_Parse_array2ptr(*_tmpD6,1);*_tmpD6=_tmp77D;});}}
# 759
return({struct Cyc_Absyn_Tqual _tmp780=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp77F=
Cyc_Absyn_function_type(typvars,_tmpDE,tq,t,_tmpE1,_tmpE0,_tmpDF,_tmpDD,fn_atts,_tmpDC,_tmpDB);
# 759
struct Cyc_List_List*_tmp77E=new_atts;Cyc_Parse_apply_tms(_tmp780,_tmp77F,_tmp77E,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpE2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpCC)->f2;_LL11:
# 766
({void*_tmpD9=0U;({unsigned _tmp782=_tmpE2;struct _fat_ptr _tmp781=({const char*_tmpDA="function declaration without parameter types";_tag_fat(_tmpDA,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp782,_tmp781,_tag_fat(_tmpD9,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmpEC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpEB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL8:
# 773
 if(tms->tl == 0)
return({struct _tuple13 _tmp68C;_tmp68C.f1=tq,_tmp68C.f2=t,_tmp68C.f3=_tmpEC,_tmp68C.f4=atts;_tmp68C;});
# 778
({void*_tmpE3=0U;({unsigned _tmp784=_tmpEB;struct _fat_ptr _tmp783=({const char*_tmpE4="type parameters must appear before function arguments in declarator";_tag_fat(_tmpE4,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp784,_tmp783,_tag_fat(_tmpE3,sizeof(void*),0U));});});case 2U: _LL9: _tmpEE=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpED=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLA:
# 781
 return({struct Cyc_Absyn_Tqual _tmp787=_tmpED;void*_tmp786=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp68D;_tmp68D.elt_type=t,_tmp68D.elt_tq=tq,_tmp68D.ptr_atts=_tmpEE;_tmp68D;}));struct Cyc_List_List*_tmp785=atts;Cyc_Parse_apply_tms(_tmp787,_tmp786,_tmp785,tms->tl);});default: _LLB: _tmpF0=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpEF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLC:
# 786
 return({struct Cyc_Absyn_Tqual _tmp78A=tq;void*_tmp789=t;struct Cyc_List_List*_tmp788=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmpEF);Cyc_Parse_apply_tms(_tmp78A,_tmp789,_tmp788,tms->tl);});}_LL0:;};}
# 792
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 801
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){
struct _tuple15*_tmpF1=t;unsigned _tmpFB;struct _tuple0*_tmpFA;struct Cyc_Absyn_Tqual _tmpF9;void*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF6;_LL1: _tmpFB=_tmpF1->f1;_tmpFA=_tmpF1->f2;_tmpF9=_tmpF1->f3;_tmpF8=_tmpF1->f4;_tmpF7=_tmpF1->f5;_tmpF6=_tmpF1->f6;_LL2:;
# 804
Cyc_Lex_register_typedef(_tmpFA);{
# 806
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF2=_tmpF8;struct Cyc_Core_Opt*_tmpF3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->tag == 1U){_LL4: _tmpF3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->f1;_LL5:
# 810
 type=0;
if(_tmpF3 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmpF3;}
goto _LL3;}else{_LL6: _LL7:
 kind=0;type=_tmpF8;goto _LL3;}_LL3:;}
# 816
return({void*_tmp78C=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp78B=({struct Cyc_Absyn_Typedefdecl*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->name=_tmpFA,_tmpF4->tvs=_tmpF7,_tmpF4->kind=kind,_tmpF4->defn=type,_tmpF4->atts=_tmpF6,_tmpF4->tq=_tmpF9,_tmpF4->extern_c=0;_tmpF4;});_tmpF5->f1=_tmp78B;});_tmpF5;});Cyc_Absyn_new_decl(_tmp78C,loc);});};}
# 823
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp78D=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=12U,_tmpFC->f1=d,_tmpFC->f2=s;_tmpFC;});Cyc_Absyn_new_stmt(_tmp78D,d->loc);});}
# 829
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 838
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned tqual_loc,unsigned loc){
# 842
struct _RegionHandle _tmpFD=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFD;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFE=ds;struct Cyc_Absyn_Tqual _tmp15F;struct Cyc_Parse_Type_specifier _tmp15E;struct Cyc_List_List*_tmp15D;_LL1: _tmp15F=_tmpFE.tq;_tmp15E=_tmpFE.type_specs;_tmp15D=_tmpFE.attributes;_LL2:;
if(_tmp15F.loc == (unsigned)0)_tmp15F.loc=tqual_loc;
if(ds.is_inline)
({void*_tmpFF=0U;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp100="inline qualifier on non-function definition";_tag_fat(_tmp100,sizeof(char),44U);});Cyc_Warn_warn(_tmp78F,_tmp78E,_tag_fat(_tmpFF,sizeof(void*),0U));});});{
# 848
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp101=*ds.sc;enum Cyc_Parse_Storage_class _tmp102=_tmp101;switch(_tmp102){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 if(Cyc_Parse_no_register)s=2U;else{s=5U;}goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 goto _LL13;default: _LL12: _LL13:
 s=1U;goto _LL3;}_LL3:;}{
# 865
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 869
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 877
void*_tmp103=Cyc_Parse_collapse_type_specifiers(_tmp15E,loc);
if(declarators == 0){
# 881
void*_tmp104=_tmp103;struct Cyc_List_List*_tmp144;struct _tuple0*_tmp143;struct _tuple0*_tmp142;int _tmp141;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Datatypedecl**_tmp13F;enum Cyc_Absyn_AggrKind _tmp13E;struct _tuple0*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Datatypedecl*_tmp13B;struct Cyc_Absyn_Enumdecl*_tmp13A;struct Cyc_Absyn_Aggrdecl*_tmp139;switch(*((int*)_tmp104)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)){case 0U: _LL15: _tmp139=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL16:
# 883
({struct Cyc_List_List*_tmp790=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp139->attributes,_tmp15D);_tmp139->attributes=_tmp790;});
_tmp139->sc=s;{
struct Cyc_List_List*_tmp107=({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_Absyn_Decl*_tmp792=({void*_tmp791=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=5U,_tmp105->f1=_tmp139;_tmp105;});Cyc_Absyn_new_decl(_tmp791,loc);});_tmp106->hd=_tmp792;}),_tmp106->tl=0;_tmp106;});_npop_handler(0U);return _tmp107;};case 1U: _LL17: _tmp13A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL18:
# 887
 if(_tmp15D != 0)({void*_tmp108=0U;({unsigned _tmp794=loc;struct _fat_ptr _tmp793=({const char*_tmp109="attributes on enum not supported";_tag_fat(_tmp109,sizeof(char),33U);});Cyc_Warn_err(_tmp794,_tmp793,_tag_fat(_tmp108,sizeof(void*),0U));});});
_tmp13A->sc=s;{
struct Cyc_List_List*_tmp10C=({struct Cyc_List_List*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct Cyc_Absyn_Decl*_tmp796=({void*_tmp795=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->tag=7U,_tmp10A->f1=_tmp13A;_tmp10A;});Cyc_Absyn_new_decl(_tmp795,loc);});_tmp10B->hd=_tmp796;}),_tmp10B->tl=0;_tmp10B;});_npop_handler(0U);return _tmp10C;};default: _LL19: _tmp13B=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL1A:
# 891
 if(_tmp15D != 0)({void*_tmp10D=0U;({unsigned _tmp798=loc;struct _fat_ptr _tmp797=({const char*_tmp10E="attributes on datatypes not supported";_tag_fat(_tmp10E,sizeof(char),38U);});Cyc_Warn_err(_tmp798,_tmp797,_tag_fat(_tmp10D,sizeof(void*),0U));});});
_tmp13B->sc=s;{
struct Cyc_List_List*_tmp111=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Decl*_tmp79A=({void*_tmp799=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=_tmp13B;_tmp10F;});Cyc_Absyn_new_decl(_tmp799,loc);});_tmp110->hd=_tmp79A;}),_tmp110->tl=0;_tmp110;});_npop_handler(0U);return _tmp111;};}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp13E=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).val).f1;_tmp13D=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).val).f2;_tmp13C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f2;_LL1C: {
# 895
struct Cyc_List_List*_tmp112=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp13C);
struct Cyc_Absyn_Aggrdecl*_tmp113=({struct Cyc_Absyn_Aggrdecl*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->kind=_tmp13E,_tmp119->sc=s,_tmp119->name=_tmp13D,_tmp119->tvs=_tmp112,_tmp119->impl=0,_tmp119->attributes=0,_tmp119->expected_mem_kind=0;_tmp119;});
if(_tmp15D != 0)({void*_tmp114=0U;({unsigned _tmp79C=loc;struct _fat_ptr _tmp79B=({const char*_tmp115="bad attributes on type declaration";_tag_fat(_tmp115,sizeof(char),35U);});Cyc_Warn_err(_tmp79C,_tmp79B,_tag_fat(_tmp114,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp118=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Decl*_tmp79E=({void*_tmp79D=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=5U,_tmp116->f1=_tmp113;_tmp116;});Cyc_Absyn_new_decl(_tmp79D,loc);});_tmp117->hd=_tmp79E;}),_tmp117->tl=0;_tmp117;});_npop_handler(0U);return _tmp118;};}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp13F=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).KnownDatatype).val;_LL1E:
# 900
 if(_tmp15D != 0)({void*_tmp11A=0U;({unsigned _tmp7A0=loc;struct _fat_ptr _tmp79F=({const char*_tmp11B="bad attributes on datatype";_tag_fat(_tmp11B,sizeof(char),27U);});Cyc_Warn_err(_tmp7A0,_tmp79F,_tag_fat(_tmp11A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp11E=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Decl*_tmp7A2=({void*_tmp7A1=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=6U,_tmp11C->f1=*_tmp13F;_tmp11C;});Cyc_Absyn_new_decl(_tmp7A1,loc);});_tmp11D->hd=_tmp7A2;}),_tmp11D->tl=0;_tmp11D;});_npop_handler(0U);return _tmp11E;};}else{_LL1F: _tmp142=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownDatatype).val).name;_tmp141=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp140=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f2;_LL20: {
# 903
struct Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp140);
struct Cyc_Absyn_Decl*_tmp120=Cyc_Absyn_datatype_decl(s,_tmp142,_tmp11F,0,_tmp141,loc);
if(_tmp15D != 0)({void*_tmp121=0U;({unsigned _tmp7A4=loc;struct _fat_ptr _tmp7A3=({const char*_tmp122="bad attributes on datatype";_tag_fat(_tmp122,sizeof(char),27U);});Cyc_Warn_err(_tmp7A4,_tmp7A3,_tag_fat(_tmp121,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp124=({struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->hd=_tmp120,_tmp123->tl=0;_tmp123;});_npop_handler(0U);return _tmp124;};}}case 15U: _LL21: _tmp143=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1;_LL22: {
# 908
struct Cyc_Absyn_Enumdecl*_tmp125=({struct Cyc_Absyn_Enumdecl*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->sc=s,_tmp12C->name=_tmp143,_tmp12C->fields=0;_tmp12C;});
if(_tmp15D != 0)({void*_tmp126=0U;({unsigned _tmp7A6=loc;struct _fat_ptr _tmp7A5=({const char*_tmp127="bad attributes on enum";_tag_fat(_tmp127,sizeof(char),23U);});Cyc_Warn_err(_tmp7A6,_tmp7A5,_tag_fat(_tmp126,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12B=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct Cyc_Absyn_Decl*_tmp7A8=({struct Cyc_Absyn_Decl*_tmp129=_cycalloc(sizeof(*_tmp129));({void*_tmp7A7=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=7U,_tmp128->f1=_tmp125;_tmp128;});_tmp129->r=_tmp7A7;}),_tmp129->loc=loc;_tmp129;});_tmp12A->hd=_tmp7A8;}),_tmp12A->tl=0;_tmp12A;});_npop_handler(0U);return _tmp12B;};}case 16U: _LL23: _tmp144=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1;_LL24: {
# 914
struct Cyc_Absyn_Enumdecl*_tmp12D=({struct Cyc_Absyn_Enumdecl*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->sc=s,({struct _tuple0*_tmp7AA=Cyc_Parse_gensym_enum();_tmp135->name=_tmp7AA;}),({struct Cyc_Core_Opt*_tmp7A9=({struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->v=_tmp144;_tmp134;});_tmp135->fields=_tmp7A9;});_tmp135;});
if(_tmp15D != 0)({void*_tmp12E=0U;({unsigned _tmp7AC=loc;struct _fat_ptr _tmp7AB=({const char*_tmp12F="bad attributes on enum";_tag_fat(_tmp12F,sizeof(char),23U);});Cyc_Warn_err(_tmp7AC,_tmp7AB,_tag_fat(_tmp12E,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));({struct Cyc_Absyn_Decl*_tmp7AE=({struct Cyc_Absyn_Decl*_tmp131=_cycalloc(sizeof(*_tmp131));({void*_tmp7AD=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=7U,_tmp130->f1=_tmp12D;_tmp130;});_tmp131->r=_tmp7AD;}),_tmp131->loc=loc;_tmp131;});_tmp132->hd=_tmp7AE;}),_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;};}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp136=0U;({unsigned _tmp7B0=loc;struct _fat_ptr _tmp7AF=({const char*_tmp137="missing declarator";_tag_fat(_tmp137,sizeof(char),19U);});Cyc_Warn_err(_tmp7B0,_tmp7AF,_tag_fat(_tmp136,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp138=0;_npop_handler(0U);return _tmp138;};}_LL14:;}else{
# 921
struct Cyc_List_List*_tmp145=Cyc_Parse_apply_tmss(mkrgn,_tmp15F,_tmp103,declarators,_tmp15D);
if(istypedef){
# 926
if(!exps_empty)
({void*_tmp146=0U;({unsigned _tmp7B2=loc;struct _fat_ptr _tmp7B1=({const char*_tmp147="initializer in typedef declaration";_tag_fat(_tmp147,sizeof(char),35U);});Cyc_Warn_err(_tmp7B2,_tmp7B1,_tag_fat(_tmp146,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple15*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp145);
struct Cyc_List_List*_tmp148=decls;_npop_handler(0U);return _tmp148;};}else{
# 932
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp149=_tmp145;for(0;_tmp149 != 0;(_tmp149=_tmp149->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp14A=(struct _tuple15*)_tmp149->hd;struct _tuple15*_tmp14B=_tmp14A;unsigned _tmp15B;struct _tuple0*_tmp15A;struct Cyc_Absyn_Tqual _tmp159;void*_tmp158;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp156;_LL28: _tmp15B=_tmp14B->f1;_tmp15A=_tmp14B->f2;_tmp159=_tmp14B->f3;_tmp158=_tmp14B->f4;_tmp157=_tmp14B->f5;_tmp156=_tmp14B->f6;_LL29:;
if(_tmp157 != 0)
({void*_tmp14C=0U;({unsigned _tmp7B4=loc;struct _fat_ptr _tmp7B3=({const char*_tmp14D="bad type params, ignoring";_tag_fat(_tmp14D,sizeof(char),26U);});Cyc_Warn_warn(_tmp7B4,_tmp7B3,_tag_fat(_tmp14C,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp14E=0U;({unsigned _tmp7B6=loc;struct _fat_ptr _tmp7B5=({const char*_tmp14F="unexpected NULL in parse!";_tag_fat(_tmp14F,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7B6,_tmp7B5,_tag_fat(_tmp14E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp150=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp151=Cyc_Absyn_new_vardecl(_tmp15B,_tmp15A,_tmp158,_tmp150);
_tmp151->tq=_tmp159;
_tmp151->sc=s;
_tmp151->attributes=_tmp156;{
struct Cyc_Absyn_Decl*_tmp152=({struct Cyc_Absyn_Decl*_tmp155=_cycalloc(sizeof(*_tmp155));({void*_tmp7B7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->tag=0U,_tmp154->f1=_tmp151;_tmp154;});_tmp155->r=_tmp7B7;}),_tmp155->loc=loc;_tmp155;});
decls=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=_tmp152,_tmp153->tl=decls;_tmp153;});};};}}{
# 947
struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp15C;};}}};};};};}
# 843
;_pop_region(mkrgn);}
# 953
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen((struct _fat_ptr)s)== (unsigned long)1 || Cyc_strlen((struct _fat_ptr)s)== (unsigned long)2){
char _tmp160=*((const char*)_check_fat_subscript(s,sizeof(char),0));char _tmp161=_tmp160;switch(_tmp161){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 963
{char _tmp162=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _tmp163=_tmp162;switch(_tmp163){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 970
goto _LL0;case 84U: _LLF: _LL10:
# 972
{char _tmp164=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _tmp165=_tmp164;switch(_tmp165){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 979
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 982
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp68F;_tmp68F.tag=0U,_tmp68F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp68F;});struct Cyc_Int_pa_PrintArg_struct _tmp169=({struct Cyc_Int_pa_PrintArg_struct _tmp68E;_tmp68E.tag=1U,({unsigned long _tmp7B8=(unsigned long)((int)Cyc_strlen((struct _fat_ptr)s));_tmp68E.f1=_tmp7B8;});_tmp68E;});void*_tmp166[2U];_tmp166[0]=& _tmp168,_tmp166[1]=& _tmp169;({unsigned _tmp7BA=loc;struct _fat_ptr _tmp7B9=({const char*_tmp167="bad kind: %s; strlen=%d";_tag_fat(_tmp167,sizeof(char),24U);});Cyc_Warn_err(_tmp7BA,_tmp7B9,_tag_fat(_tmp166,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 987
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16A=e->r;void*_tmp16B=_tmp16A;int _tmp16E;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).tag == 5){_LL1: _tmp16E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).val).f2;_LL2:
 return _tmp16E;}else{goto _LL3;}}else{_LL3: _LL4:
# 991
({void*_tmp16C=0U;({unsigned _tmp7BC=loc;struct _fat_ptr _tmp7BB=({const char*_tmp16D="expecting integer constant";_tag_fat(_tmp16D,sizeof(char),27U);});Cyc_Warn_err(_tmp7BC,_tmp7BB,_tag_fat(_tmp16C,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 997
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16F=e->r;void*_tmp170=_tmp16F;struct _fat_ptr _tmp173;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).tag == 8){_LL1: _tmp173=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).val;_LL2:
 return _tmp173;}else{goto _LL3;}}else{_LL3: _LL4:
# 1001
({void*_tmp171=0U;({unsigned _tmp7BE=loc;struct _fat_ptr _tmp7BD=({const char*_tmp172="expecting string constant";_tag_fat(_tmp172,sizeof(char),26U);});Cyc_Warn_err(_tmp7BE,_tmp7BD,_tag_fat(_tmp171,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 1007
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp174=x;long long _tmp17C;char _tmp17B;int _tmp17A;switch((_tmp174.LongLong_c).tag){case 5U: _LL1: _tmp17A=((_tmp174.Int_c).val).f2;_LL2:
 return(unsigned)_tmp17A;case 2U: _LL3: _tmp17B=((_tmp174.Char_c).val).f2;_LL4:
 return(unsigned)_tmp17B;case 6U: _LL5: _tmp17C=((_tmp174.LongLong_c).val).f2;_LL6: {
# 1012
unsigned long long y=(unsigned long long)_tmp17C;
if(y > (unsigned long long)-1)
({void*_tmp175=0U;({unsigned _tmp7C0=loc;struct _fat_ptr _tmp7BF=({const char*_tmp176="integer constant too large";_tag_fat(_tmp176,sizeof(char),27U);});Cyc_Warn_err(_tmp7C0,_tmp7BF,_tag_fat(_tmp175,sizeof(void*),0U));});});
return(unsigned)_tmp17C;}default: _LL7: _LL8:
# 1017
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp690;_tmp690.tag=0U,({struct _fat_ptr _tmp7C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp690.f1=_tmp7C1;});_tmp690;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({unsigned _tmp7C3=loc;struct _fat_ptr _tmp7C2=({const char*_tmp178="expected integer constant but found %s";_tag_fat(_tmp178,sizeof(char),39U);});Cyc_Warn_err(_tmp7C3,_tmp7C2,_tag_fat(_tmp177,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17D=p->r;void*_tmp17E=_tmp17D;struct Cyc_Absyn_Exp*_tmp18B;struct _tuple0*_tmp18A;struct Cyc_List_List*_tmp189;struct _fat_ptr _tmp188;int _tmp187;char _tmp186;enum Cyc_Absyn_Sign _tmp185;int _tmp184;struct Cyc_Absyn_Pat*_tmp183;struct Cyc_Absyn_Vardecl*_tmp182;struct _tuple0*_tmp181;switch(*((int*)_tmp17E)){case 15U: _LL1: _tmp181=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp181,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f2)->r)->tag == 0U){_LL3: _tmp182=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL4:
# 1027
 return({struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Absyn_unknownid_exp(_tmp182->name,p->loc);Cyc_Absyn_deref_exp(_tmp7C4,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp183=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Parse_pat2exp(_tmp183);Cyc_Absyn_address_exp(_tmp7C5,p->loc);});case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp185=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp184=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLA:
 return Cyc_Absyn_int_exp(_tmp185,_tmp184,p->loc);case 11U: _LLB: _tmp186=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LLC:
 return Cyc_Absyn_char_exp(_tmp186,p->loc);case 12U: _LLD: _tmp188=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp187=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLE:
 return Cyc_Absyn_float_exp(_tmp188,_tmp187,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f3 == 0){_LLF: _tmp18A=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp189=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LL10: {
# 1034
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp18A,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp189);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp18B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL12:
 return _tmp18B;default: _LL13: _LL14:
# 1039
({void*_tmp17F=0U;({unsigned _tmp7C7=p->loc;struct _fat_ptr _tmp7C6=({const char*_tmp180="cannot mix patterns and expressions in case";_tag_fat(_tmp180,sizeof(char),44U);});Cyc_Warn_err(_tmp7C7,_tmp7C6,_tag_fat(_tmp17F,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1116
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->tag=Cyc_Core_Failure,_tmp18C->f1=s;_tmp18C;}));}static char _tmp18F[7U]="cnst_t";
# 1096 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18F,_tmp18F,_tmp18F + 7U};
union Cyc_YYSTYPE*_tmp18D=yy1;union Cyc_Absyn_Cnst _tmp18E;if((((union Cyc_YYSTYPE*)_tmp18D)->Int_tok).tag == 1){_LL1: _tmp18E=(_tmp18D->Int_tok).val;_LL2:
 return _tmp18E;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp691;(_tmp691.Int_tok).tag=1U,(_tmp691.Int_tok).val=yy1;_tmp691;});}static char _tmp192[5U]="char";
# 1097 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp192,_tmp192,_tmp192 + 5U};
union Cyc_YYSTYPE*_tmp190=yy1;char _tmp191;if((((union Cyc_YYSTYPE*)_tmp190)->Char_tok).tag == 2){_LL1: _tmp191=(_tmp190->Char_tok).val;_LL2:
 return _tmp191;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp692;(_tmp692.Char_tok).tag=2U,(_tmp692.Char_tok).val=yy1;_tmp692;});}static char _tmp195[13U]="string_t<`H>";
# 1098 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp195,_tmp195,_tmp195 + 13U};
union Cyc_YYSTYPE*_tmp193=yy1;struct _fat_ptr _tmp194;if((((union Cyc_YYSTYPE*)_tmp193)->String_tok).tag == 3){_LL1: _tmp194=(_tmp193->String_tok).val;_LL2:
 return _tmp194;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp693;(_tmp693.String_tok).tag=3U,(_tmp693.String_tok).val=yy1;_tmp693;});}static char _tmp198[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1101 "parse.y"
static struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp198,_tmp198,_tmp198 + 45U};
union Cyc_YYSTYPE*_tmp196=yy1;struct _tuple21*_tmp197;if((((union Cyc_YYSTYPE*)_tmp196)->YY1).tag == 9){_LL1: _tmp197=(_tmp196->YY1).val;_LL2:
 return _tmp197;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp694;(_tmp694.YY1).tag=9U,(_tmp694.YY1).val=yy1;_tmp694;});}static char _tmp19B[11U]="ptrbound_t";
# 1102 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19B,_tmp19B,_tmp19B + 11U};
union Cyc_YYSTYPE*_tmp199=yy1;void*_tmp19A;if((((union Cyc_YYSTYPE*)_tmp199)->YY2).tag == 10){_LL1: _tmp19A=(_tmp199->YY2).val;_LL2:
 return _tmp19A;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp695;(_tmp695.YY2).tag=10U,(_tmp695.YY2).val=yy1;_tmp695;});}static char _tmp19E[28U]="list_t<offsetof_field_t,`H>";
# 1103 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19E,_tmp19E,_tmp19E + 28U};
union Cyc_YYSTYPE*_tmp19C=yy1;struct Cyc_List_List*_tmp19D;if((((union Cyc_YYSTYPE*)_tmp19C)->YY3).tag == 11){_LL1: _tmp19D=(_tmp19C->YY3).val;_LL2:
 return _tmp19D;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp696;(_tmp696.YY3).tag=11U,(_tmp696.YY3).val=yy1;_tmp696;});}static char _tmp1A1[6U]="exp_t";
# 1104 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A1,_tmp1A1,_tmp1A1 + 6U};
union Cyc_YYSTYPE*_tmp19F=yy1;struct Cyc_Absyn_Exp*_tmp1A0;if((((union Cyc_YYSTYPE*)_tmp19F)->Exp_tok).tag == 7){_LL1: _tmp1A0=(_tmp19F->Exp_tok).val;_LL2:
 return _tmp1A0;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp697;(_tmp697.Exp_tok).tag=7U,(_tmp697.Exp_tok).val=yy1;_tmp697;});}static char _tmp1A4[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A4,_tmp1A4,_tmp1A4 + 17U};
union Cyc_YYSTYPE*_tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;if((((union Cyc_YYSTYPE*)_tmp1A2)->YY4).tag == 12){_LL1: _tmp1A3=(_tmp1A2->YY4).val;_LL2:
 return _tmp1A3;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp698;(_tmp698.YY4).tag=12U,(_tmp698.YY4).val=yy1;_tmp698;});}static char _tmp1A7[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1113 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A7,_tmp1A7,_tmp1A7 + 47U};
union Cyc_YYSTYPE*_tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;if((((union Cyc_YYSTYPE*)_tmp1A5)->YY5).tag == 13){_LL1: _tmp1A6=(_tmp1A5->YY5).val;_LL2:
 return _tmp1A6;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp699;(_tmp699.YY5).tag=13U,(_tmp699.YY5).val=yy1;_tmp699;});}static char _tmp1AA[9U]="primop_t";
# 1114 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AA,_tmp1AA,_tmp1AA + 9U};
union Cyc_YYSTYPE*_tmp1A8=yy1;enum Cyc_Absyn_Primop _tmp1A9;if((((union Cyc_YYSTYPE*)_tmp1A8)->YY6).tag == 14){_LL1: _tmp1A9=(_tmp1A8->YY6).val;_LL2:
 return _tmp1A9;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp69A;(_tmp69A.YY6).tag=14U,(_tmp69A.YY6).val=yy1;_tmp69A;});}static char _tmp1AD[19U]="opt_t<primop_t,`H>";
# 1115 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AD,_tmp1AD,_tmp1AD + 19U};
union Cyc_YYSTYPE*_tmp1AB=yy1;struct Cyc_Core_Opt*_tmp1AC;if((((union Cyc_YYSTYPE*)_tmp1AB)->YY7).tag == 15){_LL1: _tmp1AC=(_tmp1AB->YY7).val;_LL2:
 return _tmp1AC;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp69B;(_tmp69B.YY7).tag=15U,(_tmp69B.YY7).val=yy1;_tmp69B;});}static char _tmp1B0[7U]="qvar_t";
# 1116 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B0,_tmp1B0,_tmp1B0 + 7U};
union Cyc_YYSTYPE*_tmp1AE=yy1;struct _tuple0*_tmp1AF;if((((union Cyc_YYSTYPE*)_tmp1AE)->QualId_tok).tag == 5){_LL1: _tmp1AF=(_tmp1AE->QualId_tok).val;_LL2:
 return _tmp1AF;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp69C;(_tmp69C.QualId_tok).tag=5U,(_tmp69C.QualId_tok).val=yy1;_tmp69C;});}static char _tmp1B3[7U]="stmt_t";
# 1119 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B3,_tmp1B3,_tmp1B3 + 7U};
union Cyc_YYSTYPE*_tmp1B1=yy1;struct Cyc_Absyn_Stmt*_tmp1B2;if((((union Cyc_YYSTYPE*)_tmp1B1)->Stmt_tok).tag == 8){_LL1: _tmp1B2=(_tmp1B1->Stmt_tok).val;_LL2:
 return _tmp1B2;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1126
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp69D;(_tmp69D.Stmt_tok).tag=8U,(_tmp69D.Stmt_tok).val=yy1;_tmp69D;});}static char _tmp1B6[27U]="list_t<switch_clause_t,`H>";
# 1123 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B6,_tmp1B6,_tmp1B6 + 27U};
union Cyc_YYSTYPE*_tmp1B4=yy1;struct Cyc_List_List*_tmp1B5;if((((union Cyc_YYSTYPE*)_tmp1B4)->YY8).tag == 16){_LL1: _tmp1B5=(_tmp1B4->YY8).val;_LL2:
 return _tmp1B5;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp69E;(_tmp69E.YY8).tag=16U,(_tmp69E.YY8).val=yy1;_tmp69E;});}static char _tmp1B9[6U]="pat_t";
# 1124 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B9,_tmp1B9,_tmp1B9 + 6U};
union Cyc_YYSTYPE*_tmp1B7=yy1;struct Cyc_Absyn_Pat*_tmp1B8;if((((union Cyc_YYSTYPE*)_tmp1B7)->YY9).tag == 17){_LL1: _tmp1B8=(_tmp1B7->YY9).val;_LL2:
 return _tmp1B8;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp69F;(_tmp69F.YY9).tag=17U,(_tmp69F.YY9).val=yy1;_tmp69F;});}static char _tmp1BC[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1129 "parse.y"
static struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BC,_tmp1BC,_tmp1BC + 28U};
union Cyc_YYSTYPE*_tmp1BA=yy1;struct _tuple22*_tmp1BB;if((((union Cyc_YYSTYPE*)_tmp1BA)->YY10).tag == 18){_LL1: _tmp1BB=(_tmp1BA->YY10).val;_LL2:
 return _tmp1BB;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1136
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6A0;(_tmp6A0.YY10).tag=18U,(_tmp6A0.YY10).val=yy1;_tmp6A0;});}static char _tmp1BF[17U]="list_t<pat_t,`H>";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BF,_tmp1BF,_tmp1BF + 17U};
union Cyc_YYSTYPE*_tmp1BD=yy1;struct Cyc_List_List*_tmp1BE;if((((union Cyc_YYSTYPE*)_tmp1BD)->YY11).tag == 19){_LL1: _tmp1BE=(_tmp1BD->YY11).val;_LL2:
 return _tmp1BE;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6A1;(_tmp6A1.YY11).tag=19U,(_tmp6A1.YY11).val=yy1;_tmp6A1;});}static char _tmp1C2[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1131 "parse.y"
static struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C2,_tmp1C2,_tmp1C2 + 36U};
union Cyc_YYSTYPE*_tmp1C0=yy1;struct _tuple23*_tmp1C1;if((((union Cyc_YYSTYPE*)_tmp1C0)->YY12).tag == 20){_LL1: _tmp1C1=(_tmp1C0->YY12).val;_LL2:
 return _tmp1C1;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6A2;(_tmp6A2.YY12).tag=20U,(_tmp6A2.YY12).val=yy1;_tmp6A2;});}static char _tmp1C5[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1132 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C5,_tmp1C5,_tmp1C5 + 47U};
union Cyc_YYSTYPE*_tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;if((((union Cyc_YYSTYPE*)_tmp1C3)->YY13).tag == 21){_LL1: _tmp1C4=(_tmp1C3->YY13).val;_LL2:
 return _tmp1C4;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1139
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6A3;(_tmp6A3.YY13).tag=21U,(_tmp6A3.YY13).val=yy1;_tmp6A3;});}static char _tmp1C8[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1133 "parse.y"
static struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C8,_tmp1C8,_tmp1C8 + 58U};
union Cyc_YYSTYPE*_tmp1C6=yy1;struct _tuple22*_tmp1C7;if((((union Cyc_YYSTYPE*)_tmp1C6)->YY14).tag == 22){_LL1: _tmp1C7=(_tmp1C6->YY14).val;_LL2:
 return _tmp1C7;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1140
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6A4;(_tmp6A4.YY14).tag=22U,(_tmp6A4.YY14).val=yy1;_tmp6A4;});}static char _tmp1CB[9U]="fndecl_t";
# 1134 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CB,_tmp1CB,_tmp1CB + 9U};
union Cyc_YYSTYPE*_tmp1C9=yy1;struct Cyc_Absyn_Fndecl*_tmp1CA;if((((union Cyc_YYSTYPE*)_tmp1C9)->YY15).tag == 23){_LL1: _tmp1CA=(_tmp1C9->YY15).val;_LL2:
 return _tmp1CA;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6A5;(_tmp6A5.YY15).tag=23U,(_tmp6A5.YY15).val=yy1;_tmp6A5;});}static char _tmp1CE[18U]="list_t<decl_t,`H>";
# 1135 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CE,_tmp1CE,_tmp1CE + 18U};
union Cyc_YYSTYPE*_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;if((((union Cyc_YYSTYPE*)_tmp1CC)->YY16).tag == 24){_LL1: _tmp1CD=(_tmp1CC->YY16).val;_LL2:
 return _tmp1CD;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6A6;(_tmp6A6.YY16).tag=24U,(_tmp6A6.YY16).val=yy1;_tmp6A6;});}static char _tmp1D1[12U]="decl_spec_t";
# 1138 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D1,_tmp1D1,_tmp1D1 + 12U};
union Cyc_YYSTYPE*_tmp1CF=yy1;struct Cyc_Parse_Declaration_spec _tmp1D0;if((((union Cyc_YYSTYPE*)_tmp1CF)->YY17).tag == 25){_LL1: _tmp1D0=(_tmp1CF->YY17).val;_LL2:
 return _tmp1D0;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1145
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6A7;(_tmp6A7.YY17).tag=25U,(_tmp6A7.YY17).val=yy1;_tmp6A7;});}static char _tmp1D4[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1139 "parse.y"
static struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D4,_tmp1D4,_tmp1D4 + 31U};
union Cyc_YYSTYPE*_tmp1D2=yy1;struct _tuple11 _tmp1D3;if((((union Cyc_YYSTYPE*)_tmp1D2)->YY18).tag == 26){_LL1: _tmp1D3=(_tmp1D2->YY18).val;_LL2:
 return _tmp1D3;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1146
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6A8;(_tmp6A8.YY18).tag=26U,(_tmp6A8.YY18).val=yy1;_tmp6A8;});}static char _tmp1D7[23U]="declarator_list_t<`yy>";
# 1140 "parse.y"
static struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D7,_tmp1D7,_tmp1D7 + 23U};
union Cyc_YYSTYPE*_tmp1D5=yy1;struct _tuple12*_tmp1D6;if((((union Cyc_YYSTYPE*)_tmp1D5)->YY19).tag == 27){_LL1: _tmp1D6=(_tmp1D5->YY19).val;_LL2:
 return _tmp1D6;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1147
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6A9;(_tmp6A9.YY19).tag=27U,(_tmp6A9.YY19).val=yy1;_tmp6A9;});}static char _tmp1DA[19U]="storage_class_t@`H";
# 1141 "parse.y"
static enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DA,_tmp1DA,_tmp1DA + 19U};
union Cyc_YYSTYPE*_tmp1D8=yy1;enum Cyc_Parse_Storage_class*_tmp1D9;if((((union Cyc_YYSTYPE*)_tmp1D8)->YY20).tag == 28){_LL1: _tmp1D9=(_tmp1D8->YY20).val;_LL2:
 return _tmp1D9;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1148
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6AA;(_tmp6AA.YY20).tag=28U,(_tmp6AA.YY20).val=yy1;_tmp6AA;});}static char _tmp1DD[17U]="type_specifier_t";
# 1142 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DD,_tmp1DD,_tmp1DD + 17U};
union Cyc_YYSTYPE*_tmp1DB=yy1;struct Cyc_Parse_Type_specifier _tmp1DC;if((((union Cyc_YYSTYPE*)_tmp1DB)->YY21).tag == 29){_LL1: _tmp1DC=(_tmp1DB->YY21).val;_LL2:
 return _tmp1DC;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1149
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6AB;(_tmp6AB.YY21).tag=29U,(_tmp6AB.YY21).val=yy1;_tmp6AB;});}static char _tmp1E0[12U]="aggr_kind_t";
# 1144 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E0,_tmp1E0,_tmp1E0 + 12U};
union Cyc_YYSTYPE*_tmp1DE=yy1;enum Cyc_Absyn_AggrKind _tmp1DF;if((((union Cyc_YYSTYPE*)_tmp1DE)->YY22).tag == 30){_LL1: _tmp1DF=(_tmp1DE->YY22).val;_LL2:
 return _tmp1DF;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1151
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6AC;(_tmp6AC.YY22).tag=30U,(_tmp6AC.YY22).val=yy1;_tmp6AC;});}static char _tmp1E3[8U]="tqual_t";
# 1145 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E3,_tmp1E3,_tmp1E3 + 8U};
union Cyc_YYSTYPE*_tmp1E1=yy1;struct Cyc_Absyn_Tqual _tmp1E2;if((((union Cyc_YYSTYPE*)_tmp1E1)->YY23).tag == 31){_LL1: _tmp1E2=(_tmp1E1->YY23).val;_LL2:
 return _tmp1E2;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1152
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6AD;(_tmp6AD.YY23).tag=31U,(_tmp6AD.YY23).val=yy1;_tmp6AD;});}static char _tmp1E6[23U]="list_t<aggrfield_t,`H>";
# 1146 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E6,_tmp1E6,_tmp1E6 + 23U};
union Cyc_YYSTYPE*_tmp1E4=yy1;struct Cyc_List_List*_tmp1E5;if((((union Cyc_YYSTYPE*)_tmp1E4)->YY24).tag == 32){_LL1: _tmp1E5=(_tmp1E4->YY24).val;_LL2:
 return _tmp1E5;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1153
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6AE;(_tmp6AE.YY24).tag=32U,(_tmp6AE.YY24).val=yy1;_tmp6AE;});}static char _tmp1E9[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1147 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E9,_tmp1E9,_tmp1E9 + 34U};
union Cyc_YYSTYPE*_tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;if((((union Cyc_YYSTYPE*)_tmp1E7)->YY25).tag == 33){_LL1: _tmp1E8=(_tmp1E7->YY25).val;_LL2:
 return _tmp1E8;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1154
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6AF;(_tmp6AF.YY25).tag=33U,(_tmp6AF.YY25).val=yy1;_tmp6AF;});}static char _tmp1EC[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1148 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EC,_tmp1EC,_tmp1EC + 33U};
union Cyc_YYSTYPE*_tmp1EA=yy1;struct Cyc_List_List*_tmp1EB;if((((union Cyc_YYSTYPE*)_tmp1EA)->YY26).tag == 34){_LL1: _tmp1EB=(_tmp1EA->YY26).val;_LL2:
 return _tmp1EB;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1155
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B0;(_tmp6B0.YY26).tag=34U,(_tmp6B0.YY26).val=yy1;_tmp6B0;});}static char _tmp1EF[18U]="declarator_t<`yy>";
# 1149 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EF,_tmp1EF,_tmp1EF + 18U};
union Cyc_YYSTYPE*_tmp1ED=yy1;struct Cyc_Parse_Declarator _tmp1EE;if((((union Cyc_YYSTYPE*)_tmp1ED)->YY27).tag == 35){_LL1: _tmp1EE=(_tmp1ED->YY27).val;_LL2:
 return _tmp1EE;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1156
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6B1;(_tmp6B1.YY27).tag=35U,(_tmp6B1.YY27).val=yy1;_tmp6B1;});}static char _tmp1F2[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1150 "parse.y"
static struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F2,_tmp1F2,_tmp1F2 + 45U};
union Cyc_YYSTYPE*_tmp1F0=yy1;struct _tuple24*_tmp1F1;if((((union Cyc_YYSTYPE*)_tmp1F0)->YY28).tag == 36){_LL1: _tmp1F1=(_tmp1F0->YY28).val;_LL2:
 return _tmp1F1;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1157
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6B2;(_tmp6B2.YY28).tag=36U,(_tmp6B2.YY28).val=yy1;_tmp6B2;});}static char _tmp1F5[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1151 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F5,_tmp1F5,_tmp1F5 + 57U};
union Cyc_YYSTYPE*_tmp1F3=yy1;struct Cyc_List_List*_tmp1F4;if((((union Cyc_YYSTYPE*)_tmp1F3)->YY29).tag == 37){_LL1: _tmp1F4=(_tmp1F3->YY29).val;_LL2:
 return _tmp1F4;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1158
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B3;(_tmp6B3.YY29).tag=37U,(_tmp6B3.YY29).val=yy1;_tmp6B3;});}static char _tmp1F8[26U]="abstractdeclarator_t<`yy>";
# 1152 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F8,_tmp1F8,_tmp1F8 + 26U};
union Cyc_YYSTYPE*_tmp1F6=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F7;if((((union Cyc_YYSTYPE*)_tmp1F6)->YY30).tag == 38){_LL1: _tmp1F7=(_tmp1F6->YY30).val;_LL2:
 return _tmp1F7;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1159
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6B4;(_tmp6B4.YY30).tag=38U,(_tmp6B4.YY30).val=yy1;_tmp6B4;});}static char _tmp1FB[5U]="bool";
# 1153 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FB,_tmp1FB,_tmp1FB + 5U};
union Cyc_YYSTYPE*_tmp1F9=yy1;int _tmp1FA;if((((union Cyc_YYSTYPE*)_tmp1F9)->YY31).tag == 39){_LL1: _tmp1FA=(_tmp1F9->YY31).val;_LL2:
 return _tmp1FA;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1160
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6B5;(_tmp6B5.YY31).tag=39U,(_tmp6B5.YY31).val=yy1;_tmp6B5;});}static char _tmp1FE[8U]="scope_t";
# 1154 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FE,_tmp1FE,_tmp1FE + 8U};
union Cyc_YYSTYPE*_tmp1FC=yy1;enum Cyc_Absyn_Scope _tmp1FD;if((((union Cyc_YYSTYPE*)_tmp1FC)->YY32).tag == 40){_LL1: _tmp1FD=(_tmp1FC->YY32).val;_LL2:
 return _tmp1FD;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1161
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6B6;(_tmp6B6.YY32).tag=40U,(_tmp6B6.YY32).val=yy1;_tmp6B6;});}static char _tmp201[16U]="datatypefield_t";
# 1155 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp201,_tmp201,_tmp201 + 16U};
union Cyc_YYSTYPE*_tmp1FF=yy1;struct Cyc_Absyn_Datatypefield*_tmp200;if((((union Cyc_YYSTYPE*)_tmp1FF)->YY33).tag == 41){_LL1: _tmp200=(_tmp1FF->YY33).val;_LL2:
 return _tmp200;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1162
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6B7;(_tmp6B7.YY33).tag=41U,(_tmp6B7.YY33).val=yy1;_tmp6B7;});}static char _tmp204[27U]="list_t<datatypefield_t,`H>";
# 1156 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp204,_tmp204,_tmp204 + 27U};
union Cyc_YYSTYPE*_tmp202=yy1;struct Cyc_List_List*_tmp203;if((((union Cyc_YYSTYPE*)_tmp202)->YY34).tag == 42){_LL1: _tmp203=(_tmp202->YY34).val;_LL2:
 return _tmp203;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1163
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B8;(_tmp6B8.YY34).tag=42U,(_tmp6B8.YY34).val=yy1;_tmp6B8;});}static char _tmp207[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1157 "parse.y"
static struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp207,_tmp207,_tmp207 + 41U};
union Cyc_YYSTYPE*_tmp205=yy1;struct _tuple25 _tmp206;if((((union Cyc_YYSTYPE*)_tmp205)->YY35).tag == 43){_LL1: _tmp206=(_tmp205->YY35).val;_LL2:
 return _tmp206;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1164
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp6B9;(_tmp6B9.YY35).tag=43U,(_tmp6B9.YY35).val=yy1;_tmp6B9;});}static char _tmp20A[17U]="list_t<var_t,`H>";
# 1158 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20A,_tmp20A,_tmp20A + 17U};
union Cyc_YYSTYPE*_tmp208=yy1;struct Cyc_List_List*_tmp209;if((((union Cyc_YYSTYPE*)_tmp208)->YY36).tag == 44){_LL1: _tmp209=(_tmp208->YY36).val;_LL2:
 return _tmp209;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1165
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BA;(_tmp6BA.YY36).tag=44U,(_tmp6BA.YY36).val=yy1;_tmp6BA;});}static char _tmp20D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1159 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20D,_tmp20D,_tmp20D + 31U};
union Cyc_YYSTYPE*_tmp20B=yy1;struct _tuple8*_tmp20C;if((((union Cyc_YYSTYPE*)_tmp20B)->YY37).tag == 45){_LL1: _tmp20C=(_tmp20B->YY37).val;_LL2:
 return _tmp20C;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1166
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6BB;(_tmp6BB.YY37).tag=45U,(_tmp6BB.YY37).val=yy1;_tmp6BB;});}static char _tmp210[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1160 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp210,_tmp210,_tmp210 + 42U};
union Cyc_YYSTYPE*_tmp20E=yy1;struct Cyc_List_List*_tmp20F;if((((union Cyc_YYSTYPE*)_tmp20E)->YY38).tag == 46){_LL1: _tmp20F=(_tmp20E->YY38).val;_LL2:
 return _tmp20F;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1167
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BC;(_tmp6BC.YY38).tag=46U,(_tmp6BC.YY38).val=yy1;_tmp6BC;});}static char _tmp213[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1161 "parse.y"
static struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp213,_tmp213,_tmp213 + 115U};
union Cyc_YYSTYPE*_tmp211=yy1;struct _tuple26*_tmp212;if((((union Cyc_YYSTYPE*)_tmp211)->YY39).tag == 47){_LL1: _tmp212=(_tmp211->YY39).val;_LL2:
 return _tmp212;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1168
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp6BD;(_tmp6BD.YY39).tag=47U,(_tmp6BD.YY39).val=yy1;_tmp6BD;});}static char _tmp216[8U]="types_t";
# 1162 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp216,_tmp216,_tmp216 + 8U};
union Cyc_YYSTYPE*_tmp214=yy1;struct Cyc_List_List*_tmp215;if((((union Cyc_YYSTYPE*)_tmp214)->YY40).tag == 48){_LL1: _tmp215=(_tmp214->YY40).val;_LL2:
 return _tmp215;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1169
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BE;(_tmp6BE.YY40).tag=48U,(_tmp6BE.YY40).val=yy1;_tmp6BE;});}static char _tmp219[24U]="list_t<designator_t,`H>";
# 1164 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp219,_tmp219,_tmp219 + 24U};
union Cyc_YYSTYPE*_tmp217=yy1;struct Cyc_List_List*_tmp218;if((((union Cyc_YYSTYPE*)_tmp217)->YY41).tag == 49){_LL1: _tmp218=(_tmp217->YY41).val;_LL2:
 return _tmp218;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1171
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BF;(_tmp6BF.YY41).tag=49U,(_tmp6BF.YY41).val=yy1;_tmp6BF;});}static char _tmp21C[13U]="designator_t";
# 1165 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21C,_tmp21C,_tmp21C + 13U};
union Cyc_YYSTYPE*_tmp21A=yy1;void*_tmp21B;if((((union Cyc_YYSTYPE*)_tmp21A)->YY42).tag == 50){_LL1: _tmp21B=(_tmp21A->YY42).val;_LL2:
 return _tmp21B;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1172
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp6C0;(_tmp6C0.YY42).tag=50U,(_tmp6C0.YY42).val=yy1;_tmp6C0;});}static char _tmp21F[7U]="kind_t";
# 1166 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21F,_tmp21F,_tmp21F + 7U};
union Cyc_YYSTYPE*_tmp21D=yy1;struct Cyc_Absyn_Kind*_tmp21E;if((((union Cyc_YYSTYPE*)_tmp21D)->YY43).tag == 51){_LL1: _tmp21E=(_tmp21D->YY43).val;_LL2:
 return _tmp21E;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1173
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp6C1;(_tmp6C1.YY43).tag=51U,(_tmp6C1.YY43).val=yy1;_tmp6C1;});}static char _tmp222[7U]="type_t";
# 1167 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp222,_tmp222,_tmp222 + 7U};
union Cyc_YYSTYPE*_tmp220=yy1;void*_tmp221;if((((union Cyc_YYSTYPE*)_tmp220)->YY44).tag == 52){_LL1: _tmp221=(_tmp220->YY44).val;_LL2:
 return _tmp221;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1174
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp6C2;(_tmp6C2.YY44).tag=52U,(_tmp6C2.YY44).val=yy1;_tmp6C2;});}static char _tmp225[23U]="list_t<attribute_t,`H>";
# 1168 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp225,_tmp225,_tmp225 + 23U};
union Cyc_YYSTYPE*_tmp223=yy1;struct Cyc_List_List*_tmp224;if((((union Cyc_YYSTYPE*)_tmp223)->YY45).tag == 53){_LL1: _tmp224=(_tmp223->YY45).val;_LL2:
 return _tmp224;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1175
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C3;(_tmp6C3.YY45).tag=53U,(_tmp6C3.YY45).val=yy1;_tmp6C3;});}static char _tmp228[12U]="attribute_t";
# 1169 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp228,_tmp228,_tmp228 + 12U};
union Cyc_YYSTYPE*_tmp226=yy1;void*_tmp227;if((((union Cyc_YYSTYPE*)_tmp226)->YY46).tag == 54){_LL1: _tmp227=(_tmp226->YY46).val;_LL2:
 return _tmp227;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1176
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp6C4;(_tmp6C4.YY46).tag=54U,(_tmp6C4.YY46).val=yy1;_tmp6C4;});}static char _tmp22B[12U]="enumfield_t";
# 1170 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22B,_tmp22B,_tmp22B + 12U};
union Cyc_YYSTYPE*_tmp229=yy1;struct Cyc_Absyn_Enumfield*_tmp22A;if((((union Cyc_YYSTYPE*)_tmp229)->YY47).tag == 55){_LL1: _tmp22A=(_tmp229->YY47).val;_LL2:
 return _tmp22A;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1177
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp6C5;(_tmp6C5.YY47).tag=55U,(_tmp6C5.YY47).val=yy1;_tmp6C5;});}static char _tmp22E[23U]="list_t<enumfield_t,`H>";
# 1171 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22E,_tmp22E,_tmp22E + 23U};
union Cyc_YYSTYPE*_tmp22C=yy1;struct Cyc_List_List*_tmp22D;if((((union Cyc_YYSTYPE*)_tmp22C)->YY48).tag == 56){_LL1: _tmp22D=(_tmp22C->YY48).val;_LL2:
 return _tmp22D;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1178
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C6;(_tmp6C6.YY48).tag=56U,(_tmp6C6.YY48).val=yy1;_tmp6C6;});}static char _tmp231[11U]="type_opt_t";
# 1172 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp231,_tmp231,_tmp231 + 11U};
union Cyc_YYSTYPE*_tmp22F=yy1;void*_tmp230;if((((union Cyc_YYSTYPE*)_tmp22F)->YY49).tag == 57){_LL1: _tmp230=(_tmp22F->YY49).val;_LL2:
 return _tmp230;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1179
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp6C7;(_tmp6C7.YY49).tag=57U,(_tmp6C7.YY49).val=yy1;_tmp6C7;});}static char _tmp234[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1173 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp234,_tmp234,_tmp234 + 31U};
union Cyc_YYSTYPE*_tmp232=yy1;struct Cyc_List_List*_tmp233;if((((union Cyc_YYSTYPE*)_tmp232)->YY50).tag == 58){_LL1: _tmp233=(_tmp232->YY50).val;_LL2:
 return _tmp233;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1180
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C8;(_tmp6C8.YY50).tag=58U,(_tmp6C8.YY50).val=yy1;_tmp6C8;});}static char _tmp237[11U]="booltype_t";
# 1174 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp237,_tmp237,_tmp237 + 11U};
union Cyc_YYSTYPE*_tmp235=yy1;void*_tmp236;if((((union Cyc_YYSTYPE*)_tmp235)->YY51).tag == 59){_LL1: _tmp236=(_tmp235->YY51).val;_LL2:
 return _tmp236;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1181
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp6C9;(_tmp6C9.YY51).tag=59U,(_tmp6C9.YY51).val=yy1;_tmp6C9;});}static char _tmp23A[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1175 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23A,_tmp23A,_tmp23A + 45U};
union Cyc_YYSTYPE*_tmp238=yy1;struct Cyc_List_List*_tmp239;if((((union Cyc_YYSTYPE*)_tmp238)->YY52).tag == 60){_LL1: _tmp239=(_tmp238->YY52).val;_LL2:
 return _tmp239;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1182
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CA;(_tmp6CA.YY52).tag=60U,(_tmp6CA.YY52).val=yy1;_tmp6CA;});}static char _tmp23D[20U]="pointer_qual_t<`yy>";
# 1176 "parse.y"
static void*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23D,_tmp23D,_tmp23D + 20U};
union Cyc_YYSTYPE*_tmp23B=yy1;void*_tmp23C;if((((union Cyc_YYSTYPE*)_tmp23B)->YY53).tag == 61){_LL1: _tmp23C=(_tmp23B->YY53).val;_LL2:
 return _tmp23C;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1183
static union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp6CB;(_tmp6CB.YY53).tag=61U,(_tmp6CB.YY53).val=yy1;_tmp6CB;});}static char _tmp240[21U]="pointer_quals_t<`yy>";
# 1177 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp240,_tmp240,_tmp240 + 21U};
union Cyc_YYSTYPE*_tmp23E=yy1;struct Cyc_List_List*_tmp23F;if((((union Cyc_YYSTYPE*)_tmp23E)->YY54).tag == 62){_LL1: _tmp23F=(_tmp23E->YY54).val;_LL2:
 return _tmp23F;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1184
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CC;(_tmp6CC.YY54).tag=62U,(_tmp6CC.YY54).val=yy1;_tmp6CC;});}static char _tmp243[21U]="$(bool,string_t<`H>)";
# 1178 "parse.y"
static struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp243,_tmp243,_tmp243 + 21U};
union Cyc_YYSTYPE*_tmp241=yy1;struct _tuple20 _tmp242;if((((union Cyc_YYSTYPE*)_tmp241)->Asm_tok).tag == 6){_LL1: _tmp242=(_tmp241->Asm_tok).val;_LL2:
 return _tmp242;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1185
static union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp6CD;(_tmp6CD.Asm_tok).tag=6U,(_tmp6CD.Asm_tok).val=yy1;_tmp6CD;});}static char _tmp246[10U]="exp_opt_t";
# 1179 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp246,_tmp246,_tmp246 + 10U};
union Cyc_YYSTYPE*_tmp244=yy1;struct Cyc_Absyn_Exp*_tmp245;if((((union Cyc_YYSTYPE*)_tmp244)->YY55).tag == 63){_LL1: _tmp245=(_tmp244->YY55).val;_LL2:
 return _tmp245;}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1186
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6CE;(_tmp6CE.YY55).tag=63U,(_tmp6CE.YY55).val=yy1;_tmp6CE;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1202
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp6CF;_tmp6CF.timestamp=0,_tmp6CF.first_line=0,_tmp6CF.first_column=0,_tmp6CF.last_line=0,_tmp6CF.last_column=0;_tmp6CF;});}
# 1205
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1216 "parse.y"
static short Cyc_yytranslate[378U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,149,2,2,133,147,144,2,130,131,138,141,126,145,135,146,2,2,2,2,2,2,2,2,2,2,134,123,128,127,129,140,139,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,136,2,137,143,132,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,124,142,125,148,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122};static char _tmp247[2U]="$";static char _tmp248[6U]="error";static char _tmp249[12U]="$undefined.";static char _tmp24A[5U]="AUTO";static char _tmp24B[9U]="REGISTER";static char _tmp24C[7U]="STATIC";static char _tmp24D[7U]="EXTERN";static char _tmp24E[8U]="TYPEDEF";static char _tmp24F[5U]="VOID";static char _tmp250[5U]="CHAR";static char _tmp251[6U]="SHORT";static char _tmp252[4U]="INT";static char _tmp253[5U]="LONG";static char _tmp254[6U]="FLOAT";static char _tmp255[7U]="DOUBLE";static char _tmp256[7U]="SIGNED";static char _tmp257[9U]="UNSIGNED";static char _tmp258[6U]="CONST";static char _tmp259[9U]="VOLATILE";static char _tmp25A[9U]="RESTRICT";static char _tmp25B[7U]="STRUCT";static char _tmp25C[6U]="UNION";static char _tmp25D[5U]="CASE";static char _tmp25E[8U]="DEFAULT";static char _tmp25F[7U]="INLINE";static char _tmp260[7U]="SIZEOF";static char _tmp261[9U]="OFFSETOF";static char _tmp262[3U]="IF";static char _tmp263[5U]="ELSE";static char _tmp264[7U]="SWITCH";static char _tmp265[6U]="WHILE";static char _tmp266[3U]="DO";static char _tmp267[4U]="FOR";static char _tmp268[5U]="GOTO";static char _tmp269[9U]="CONTINUE";static char _tmp26A[6U]="BREAK";static char _tmp26B[7U]="RETURN";static char _tmp26C[5U]="ENUM";static char _tmp26D[7U]="TYPEOF";static char _tmp26E[16U]="BUILTIN_VA_LIST";static char _tmp26F[10U]="EXTENSION";static char _tmp270[8U]="NULL_kw";static char _tmp271[4U]="LET";static char _tmp272[6U]="THROW";static char _tmp273[4U]="TRY";static char _tmp274[6U]="CATCH";static char _tmp275[7U]="EXPORT";static char _tmp276[9U]="OVERRIDE";static char _tmp277[4U]="NEW";static char _tmp278[9U]="ABSTRACT";static char _tmp279[9U]="FALLTHRU";static char _tmp27A[6U]="USING";static char _tmp27B[10U]="NAMESPACE";static char _tmp27C[9U]="DATATYPE";static char _tmp27D[7U]="MALLOC";static char _tmp27E[8U]="RMALLOC";static char _tmp27F[15U]="RMALLOC_INLINE";static char _tmp280[7U]="CALLOC";static char _tmp281[8U]="RCALLOC";static char _tmp282[5U]="SWAP";static char _tmp283[9U]="REGION_T";static char _tmp284[6U]="TAG_T";static char _tmp285[7U]="REGION";static char _tmp286[5U]="RNEW";static char _tmp287[8U]="REGIONS";static char _tmp288[7U]="PORTON";static char _tmp289[8U]="PORTOFF";static char _tmp28A[7U]="PRAGMA";static char _tmp28B[10U]="TEMPESTON";static char _tmp28C[11U]="TEMPESTOFF";static char _tmp28D[8U]="NUMELTS";static char _tmp28E[8U]="VALUEOF";static char _tmp28F[10U]="VALUEOF_T";static char _tmp290[9U]="TAGCHECK";static char _tmp291[13U]="NUMELTS_QUAL";static char _tmp292[10U]="THIN_QUAL";static char _tmp293[9U]="FAT_QUAL";static char _tmp294[13U]="NOTNULL_QUAL";static char _tmp295[14U]="NULLABLE_QUAL";static char _tmp296[14U]="REQUIRES_QUAL";static char _tmp297[13U]="ENSURES_QUAL";static char _tmp298[12U]="REGION_QUAL";static char _tmp299[16U]="NOZEROTERM_QUAL";static char _tmp29A[14U]="ZEROTERM_QUAL";static char _tmp29B[12U]="TAGGED_QUAL";static char _tmp29C[16U]="EXTENSIBLE_QUAL";static char _tmp29D[7U]="PTR_OP";static char _tmp29E[7U]="INC_OP";static char _tmp29F[7U]="DEC_OP";static char _tmp2A0[8U]="LEFT_OP";static char _tmp2A1[9U]="RIGHT_OP";static char _tmp2A2[6U]="LE_OP";static char _tmp2A3[6U]="GE_OP";static char _tmp2A4[6U]="EQ_OP";static char _tmp2A5[6U]="NE_OP";static char _tmp2A6[7U]="AND_OP";static char _tmp2A7[6U]="OR_OP";static char _tmp2A8[11U]="MUL_ASSIGN";static char _tmp2A9[11U]="DIV_ASSIGN";static char _tmp2AA[11U]="MOD_ASSIGN";static char _tmp2AB[11U]="ADD_ASSIGN";static char _tmp2AC[11U]="SUB_ASSIGN";static char _tmp2AD[12U]="LEFT_ASSIGN";static char _tmp2AE[13U]="RIGHT_ASSIGN";static char _tmp2AF[11U]="AND_ASSIGN";static char _tmp2B0[11U]="XOR_ASSIGN";static char _tmp2B1[10U]="OR_ASSIGN";static char _tmp2B2[9U]="ELLIPSIS";static char _tmp2B3[11U]="LEFT_RIGHT";static char _tmp2B4[12U]="COLON_COLON";static char _tmp2B5[11U]="IDENTIFIER";static char _tmp2B6[17U]="INTEGER_CONSTANT";static char _tmp2B7[7U]="STRING";static char _tmp2B8[8U]="WSTRING";static char _tmp2B9[19U]="CHARACTER_CONSTANT";static char _tmp2BA[20U]="WCHARACTER_CONSTANT";static char _tmp2BB[18U]="FLOATING_CONSTANT";static char _tmp2BC[9U]="TYPE_VAR";static char _tmp2BD[13U]="TYPEDEF_NAME";static char _tmp2BE[16U]="QUAL_IDENTIFIER";static char _tmp2BF[18U]="QUAL_TYPEDEF_NAME";static char _tmp2C0[10U]="ATTRIBUTE";static char _tmp2C1[4U]="ASM";static char _tmp2C2[4U]="';'";static char _tmp2C3[4U]="'{'";static char _tmp2C4[4U]="'}'";static char _tmp2C5[4U]="','";static char _tmp2C6[4U]="'='";static char _tmp2C7[4U]="'<'";static char _tmp2C8[4U]="'>'";static char _tmp2C9[4U]="'('";static char _tmp2CA[4U]="')'";static char _tmp2CB[4U]="'_'";static char _tmp2CC[4U]="'$'";static char _tmp2CD[4U]="':'";static char _tmp2CE[4U]="'.'";static char _tmp2CF[4U]="'['";static char _tmp2D0[4U]="']'";static char _tmp2D1[4U]="'*'";static char _tmp2D2[4U]="'@'";static char _tmp2D3[4U]="'?'";static char _tmp2D4[4U]="'+'";static char _tmp2D5[4U]="'|'";static char _tmp2D6[4U]="'^'";static char _tmp2D7[4U]="'&'";static char _tmp2D8[4U]="'-'";static char _tmp2D9[4U]="'/'";static char _tmp2DA[4U]="'%'";static char _tmp2DB[4U]="'~'";static char _tmp2DC[4U]="'!'";static char _tmp2DD[5U]="prog";static char _tmp2DE[17U]="translation_unit";static char _tmp2DF[18U]="tempest_on_action";static char _tmp2E0[19U]="tempest_off_action";static char _tmp2E1[16U]="extern_c_action";static char _tmp2E2[13U]="end_extern_c";static char _tmp2E3[16U]="export_list_opt";static char _tmp2E4[12U]="export_list";static char _tmp2E5[19U]="export_list_values";static char _tmp2E6[13U]="override_opt";static char _tmp2E7[21U]="external_declaration";static char _tmp2E8[15U]="optional_comma";static char _tmp2E9[20U]="function_definition";static char _tmp2EA[21U]="function_definition2";static char _tmp2EB[13U]="using_action";static char _tmp2EC[15U]="unusing_action";static char _tmp2ED[17U]="namespace_action";static char _tmp2EE[19U]="unnamespace_action";static char _tmp2EF[12U]="declaration";static char _tmp2F0[17U]="declaration_list";static char _tmp2F1[23U]="declaration_specifiers";static char _tmp2F2[24U]="storage_class_specifier";static char _tmp2F3[15U]="attributes_opt";static char _tmp2F4[11U]="attributes";static char _tmp2F5[15U]="attribute_list";static char _tmp2F6[10U]="attribute";static char _tmp2F7[15U]="type_specifier";static char _tmp2F8[25U]="type_specifier_notypedef";static char _tmp2F9[5U]="kind";static char _tmp2FA[15U]="type_qualifier";static char _tmp2FB[15U]="enum_specifier";static char _tmp2FC[11U]="enum_field";static char _tmp2FD[22U]="enum_declaration_list";static char _tmp2FE[26U]="struct_or_union_specifier";static char _tmp2FF[16U]="type_params_opt";static char _tmp300[16U]="struct_or_union";static char _tmp301[24U]="struct_declaration_list";static char _tmp302[25U]="struct_declaration_list0";static char _tmp303[21U]="init_declarator_list";static char _tmp304[22U]="init_declarator_list0";static char _tmp305[16U]="init_declarator";static char _tmp306[19U]="struct_declaration";static char _tmp307[25U]="specifier_qualifier_list";static char _tmp308[35U]="notypedef_specifier_qualifier_list";static char _tmp309[23U]="struct_declarator_list";static char _tmp30A[24U]="struct_declarator_list0";static char _tmp30B[18U]="struct_declarator";static char _tmp30C[20U]="requires_clause_opt";static char _tmp30D[19U]="ensures_clause_opt";static char _tmp30E[19U]="datatype_specifier";static char _tmp30F[14U]="qual_datatype";static char _tmp310[19U]="datatypefield_list";static char _tmp311[20U]="datatypefield_scope";static char _tmp312[14U]="datatypefield";static char _tmp313[11U]="declarator";static char _tmp314[23U]="declarator_withtypedef";static char _tmp315[18U]="direct_declarator";static char _tmp316[30U]="direct_declarator_withtypedef";static char _tmp317[8U]="pointer";static char _tmp318[12U]="one_pointer";static char _tmp319[14U]="pointer_quals";static char _tmp31A[13U]="pointer_qual";static char _tmp31B[23U]="pointer_null_and_bound";static char _tmp31C[14U]="pointer_bound";static char _tmp31D[18U]="zeroterm_qual_opt";static char _tmp31E[8U]="rgn_opt";static char _tmp31F[11U]="tqual_list";static char _tmp320[20U]="parameter_type_list";static char _tmp321[9U]="type_var";static char _tmp322[16U]="optional_effect";static char _tmp323[19U]="optional_rgn_order";static char _tmp324[10U]="rgn_order";static char _tmp325[16U]="optional_inject";static char _tmp326[11U]="effect_set";static char _tmp327[14U]="atomic_effect";static char _tmp328[11U]="region_set";static char _tmp329[15U]="parameter_list";static char _tmp32A[22U]="parameter_declaration";static char _tmp32B[16U]="identifier_list";static char _tmp32C[17U]="identifier_list0";static char _tmp32D[12U]="initializer";static char _tmp32E[18U]="array_initializer";static char _tmp32F[17U]="initializer_list";static char _tmp330[12U]="designation";static char _tmp331[16U]="designator_list";static char _tmp332[11U]="designator";static char _tmp333[10U]="type_name";static char _tmp334[14U]="any_type_name";static char _tmp335[15U]="type_name_list";static char _tmp336[20U]="abstract_declarator";static char _tmp337[27U]="direct_abstract_declarator";static char _tmp338[10U]="statement";static char _tmp339[18U]="labeled_statement";static char _tmp33A[21U]="expression_statement";static char _tmp33B[19U]="compound_statement";static char _tmp33C[16U]="block_item_list";static char _tmp33D[20U]="selection_statement";static char _tmp33E[15U]="switch_clauses";static char _tmp33F[20U]="iteration_statement";static char _tmp340[15U]="jump_statement";static char _tmp341[12U]="exp_pattern";static char _tmp342[20U]="conditional_pattern";static char _tmp343[19U]="logical_or_pattern";static char _tmp344[20U]="logical_and_pattern";static char _tmp345[21U]="inclusive_or_pattern";static char _tmp346[21U]="exclusive_or_pattern";static char _tmp347[12U]="and_pattern";static char _tmp348[17U]="equality_pattern";static char _tmp349[19U]="relational_pattern";static char _tmp34A[14U]="shift_pattern";static char _tmp34B[17U]="additive_pattern";static char _tmp34C[23U]="multiplicative_pattern";static char _tmp34D[13U]="cast_pattern";static char _tmp34E[14U]="unary_pattern";static char _tmp34F[16U]="postfix_pattern";static char _tmp350[16U]="primary_pattern";static char _tmp351[8U]="pattern";static char _tmp352[19U]="tuple_pattern_list";static char _tmp353[20U]="tuple_pattern_list0";static char _tmp354[14U]="field_pattern";static char _tmp355[19U]="field_pattern_list";static char _tmp356[20U]="field_pattern_list0";static char _tmp357[11U]="expression";static char _tmp358[22U]="assignment_expression";static char _tmp359[20U]="assignment_operator";static char _tmp35A[23U]="conditional_expression";static char _tmp35B[20U]="constant_expression";static char _tmp35C[22U]="logical_or_expression";static char _tmp35D[23U]="logical_and_expression";static char _tmp35E[24U]="inclusive_or_expression";static char _tmp35F[24U]="exclusive_or_expression";static char _tmp360[15U]="and_expression";static char _tmp361[20U]="equality_expression";static char _tmp362[22U]="relational_expression";static char _tmp363[17U]="shift_expression";static char _tmp364[20U]="additive_expression";static char _tmp365[26U]="multiplicative_expression";static char _tmp366[16U]="cast_expression";static char _tmp367[17U]="unary_expression";static char _tmp368[15U]="unary_operator";static char _tmp369[19U]="postfix_expression";static char _tmp36A[17U]="field_expression";static char _tmp36B[19U]="primary_expression";static char _tmp36C[25U]="argument_expression_list";static char _tmp36D[26U]="argument_expression_list0";static char _tmp36E[9U]="constant";static char _tmp36F[20U]="qual_opt_identifier";static char _tmp370[17U]="qual_opt_typedef";static char _tmp371[18U]="struct_union_name";static char _tmp372[11U]="field_name";static char _tmp373[12U]="right_angle";
# 1573 "parse.y"
static struct _fat_ptr Cyc_yytname[301U]={{_tmp247,_tmp247,_tmp247 + 2U},{_tmp248,_tmp248,_tmp248 + 6U},{_tmp249,_tmp249,_tmp249 + 12U},{_tmp24A,_tmp24A,_tmp24A + 5U},{_tmp24B,_tmp24B,_tmp24B + 9U},{_tmp24C,_tmp24C,_tmp24C + 7U},{_tmp24D,_tmp24D,_tmp24D + 7U},{_tmp24E,_tmp24E,_tmp24E + 8U},{_tmp24F,_tmp24F,_tmp24F + 5U},{_tmp250,_tmp250,_tmp250 + 5U},{_tmp251,_tmp251,_tmp251 + 6U},{_tmp252,_tmp252,_tmp252 + 4U},{_tmp253,_tmp253,_tmp253 + 5U},{_tmp254,_tmp254,_tmp254 + 6U},{_tmp255,_tmp255,_tmp255 + 7U},{_tmp256,_tmp256,_tmp256 + 7U},{_tmp257,_tmp257,_tmp257 + 9U},{_tmp258,_tmp258,_tmp258 + 6U},{_tmp259,_tmp259,_tmp259 + 9U},{_tmp25A,_tmp25A,_tmp25A + 9U},{_tmp25B,_tmp25B,_tmp25B + 7U},{_tmp25C,_tmp25C,_tmp25C + 6U},{_tmp25D,_tmp25D,_tmp25D + 5U},{_tmp25E,_tmp25E,_tmp25E + 8U},{_tmp25F,_tmp25F,_tmp25F + 7U},{_tmp260,_tmp260,_tmp260 + 7U},{_tmp261,_tmp261,_tmp261 + 9U},{_tmp262,_tmp262,_tmp262 + 3U},{_tmp263,_tmp263,_tmp263 + 5U},{_tmp264,_tmp264,_tmp264 + 7U},{_tmp265,_tmp265,_tmp265 + 6U},{_tmp266,_tmp266,_tmp266 + 3U},{_tmp267,_tmp267,_tmp267 + 4U},{_tmp268,_tmp268,_tmp268 + 5U},{_tmp269,_tmp269,_tmp269 + 9U},{_tmp26A,_tmp26A,_tmp26A + 6U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 5U},{_tmp26D,_tmp26D,_tmp26D + 7U},{_tmp26E,_tmp26E,_tmp26E + 16U},{_tmp26F,_tmp26F,_tmp26F + 10U},{_tmp270,_tmp270,_tmp270 + 8U},{_tmp271,_tmp271,_tmp271 + 4U},{_tmp272,_tmp272,_tmp272 + 6U},{_tmp273,_tmp273,_tmp273 + 4U},{_tmp274,_tmp274,_tmp274 + 6U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 4U},{_tmp278,_tmp278,_tmp278 + 9U},{_tmp279,_tmp279,_tmp279 + 9U},{_tmp27A,_tmp27A,_tmp27A + 6U},{_tmp27B,_tmp27B,_tmp27B + 10U},{_tmp27C,_tmp27C,_tmp27C + 9U},{_tmp27D,_tmp27D,_tmp27D + 7U},{_tmp27E,_tmp27E,_tmp27E + 8U},{_tmp27F,_tmp27F,_tmp27F + 15U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 8U},{_tmp282,_tmp282,_tmp282 + 5U},{_tmp283,_tmp283,_tmp283 + 9U},{_tmp284,_tmp284,_tmp284 + 6U},{_tmp285,_tmp285,_tmp285 + 7U},{_tmp286,_tmp286,_tmp286 + 5U},{_tmp287,_tmp287,_tmp287 + 8U},{_tmp288,_tmp288,_tmp288 + 7U},{_tmp289,_tmp289,_tmp289 + 8U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 10U},{_tmp28C,_tmp28C,_tmp28C + 11U},{_tmp28D,_tmp28D,_tmp28D + 8U},{_tmp28E,_tmp28E,_tmp28E + 8U},{_tmp28F,_tmp28F,_tmp28F + 10U},{_tmp290,_tmp290,_tmp290 + 9U},{_tmp291,_tmp291,_tmp291 + 13U},{_tmp292,_tmp292,_tmp292 + 10U},{_tmp293,_tmp293,_tmp293 + 9U},{_tmp294,_tmp294,_tmp294 + 13U},{_tmp295,_tmp295,_tmp295 + 14U},{_tmp296,_tmp296,_tmp296 + 14U},{_tmp297,_tmp297,_tmp297 + 13U},{_tmp298,_tmp298,_tmp298 + 12U},{_tmp299,_tmp299,_tmp299 + 16U},{_tmp29A,_tmp29A,_tmp29A + 14U},{_tmp29B,_tmp29B,_tmp29B + 12U},{_tmp29C,_tmp29C,_tmp29C + 16U},{_tmp29D,_tmp29D,_tmp29D + 7U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 7U},{_tmp2A0,_tmp2A0,_tmp2A0 + 8U},{_tmp2A1,_tmp2A1,_tmp2A1 + 9U},{_tmp2A2,_tmp2A2,_tmp2A2 + 6U},{_tmp2A3,_tmp2A3,_tmp2A3 + 6U},{_tmp2A4,_tmp2A4,_tmp2A4 + 6U},{_tmp2A5,_tmp2A5,_tmp2A5 + 6U},{_tmp2A6,_tmp2A6,_tmp2A6 + 7U},{_tmp2A7,_tmp2A7,_tmp2A7 + 6U},{_tmp2A8,_tmp2A8,_tmp2A8 + 11U},{_tmp2A9,_tmp2A9,_tmp2A9 + 11U},{_tmp2AA,_tmp2AA,_tmp2AA + 11U},{_tmp2AB,_tmp2AB,_tmp2AB + 11U},{_tmp2AC,_tmp2AC,_tmp2AC + 11U},{_tmp2AD,_tmp2AD,_tmp2AD + 12U},{_tmp2AE,_tmp2AE,_tmp2AE + 13U},{_tmp2AF,_tmp2AF,_tmp2AF + 11U},{_tmp2B0,_tmp2B0,_tmp2B0 + 11U},{_tmp2B1,_tmp2B1,_tmp2B1 + 10U},{_tmp2B2,_tmp2B2,_tmp2B2 + 9U},{_tmp2B3,_tmp2B3,_tmp2B3 + 11U},{_tmp2B4,_tmp2B4,_tmp2B4 + 12U},{_tmp2B5,_tmp2B5,_tmp2B5 + 11U},{_tmp2B6,_tmp2B6,_tmp2B6 + 17U},{_tmp2B7,_tmp2B7,_tmp2B7 + 7U},{_tmp2B8,_tmp2B8,_tmp2B8 + 8U},{_tmp2B9,_tmp2B9,_tmp2B9 + 19U},{_tmp2BA,_tmp2BA,_tmp2BA + 20U},{_tmp2BB,_tmp2BB,_tmp2BB + 18U},{_tmp2BC,_tmp2BC,_tmp2BC + 9U},{_tmp2BD,_tmp2BD,_tmp2BD + 13U},{_tmp2BE,_tmp2BE,_tmp2BE + 16U},{_tmp2BF,_tmp2BF,_tmp2BF + 18U},{_tmp2C0,_tmp2C0,_tmp2C0 + 10U},{_tmp2C1,_tmp2C1,_tmp2C1 + 4U},{_tmp2C2,_tmp2C2,_tmp2C2 + 4U},{_tmp2C3,_tmp2C3,_tmp2C3 + 4U},{_tmp2C4,_tmp2C4,_tmp2C4 + 4U},{_tmp2C5,_tmp2C5,_tmp2C5 + 4U},{_tmp2C6,_tmp2C6,_tmp2C6 + 4U},{_tmp2C7,_tmp2C7,_tmp2C7 + 4U},{_tmp2C8,_tmp2C8,_tmp2C8 + 4U},{_tmp2C9,_tmp2C9,_tmp2C9 + 4U},{_tmp2CA,_tmp2CA,_tmp2CA + 4U},{_tmp2CB,_tmp2CB,_tmp2CB + 4U},{_tmp2CC,_tmp2CC,_tmp2CC + 4U},{_tmp2CD,_tmp2CD,_tmp2CD + 4U},{_tmp2CE,_tmp2CE,_tmp2CE + 4U},{_tmp2CF,_tmp2CF,_tmp2CF + 4U},{_tmp2D0,_tmp2D0,_tmp2D0 + 4U},{_tmp2D1,_tmp2D1,_tmp2D1 + 4U},{_tmp2D2,_tmp2D2,_tmp2D2 + 4U},{_tmp2D3,_tmp2D3,_tmp2D3 + 4U},{_tmp2D4,_tmp2D4,_tmp2D4 + 4U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 5U},{_tmp2DE,_tmp2DE,_tmp2DE + 17U},{_tmp2DF,_tmp2DF,_tmp2DF + 18U},{_tmp2E0,_tmp2E0,_tmp2E0 + 19U},{_tmp2E1,_tmp2E1,_tmp2E1 + 16U},{_tmp2E2,_tmp2E2,_tmp2E2 + 13U},{_tmp2E3,_tmp2E3,_tmp2E3 + 16U},{_tmp2E4,_tmp2E4,_tmp2E4 + 12U},{_tmp2E5,_tmp2E5,_tmp2E5 + 19U},{_tmp2E6,_tmp2E6,_tmp2E6 + 13U},{_tmp2E7,_tmp2E7,_tmp2E7 + 21U},{_tmp2E8,_tmp2E8,_tmp2E8 + 15U},{_tmp2E9,_tmp2E9,_tmp2E9 + 20U},{_tmp2EA,_tmp2EA,_tmp2EA + 21U},{_tmp2EB,_tmp2EB,_tmp2EB + 13U},{_tmp2EC,_tmp2EC,_tmp2EC + 15U},{_tmp2ED,_tmp2ED,_tmp2ED + 17U},{_tmp2EE,_tmp2EE,_tmp2EE + 19U},{_tmp2EF,_tmp2EF,_tmp2EF + 12U},{_tmp2F0,_tmp2F0,_tmp2F0 + 17U},{_tmp2F1,_tmp2F1,_tmp2F1 + 23U},{_tmp2F2,_tmp2F2,_tmp2F2 + 24U},{_tmp2F3,_tmp2F3,_tmp2F3 + 15U},{_tmp2F4,_tmp2F4,_tmp2F4 + 11U},{_tmp2F5,_tmp2F5,_tmp2F5 + 15U},{_tmp2F6,_tmp2F6,_tmp2F6 + 10U},{_tmp2F7,_tmp2F7,_tmp2F7 + 15U},{_tmp2F8,_tmp2F8,_tmp2F8 + 25U},{_tmp2F9,_tmp2F9,_tmp2F9 + 5U},{_tmp2FA,_tmp2FA,_tmp2FA + 15U},{_tmp2FB,_tmp2FB,_tmp2FB + 15U},{_tmp2FC,_tmp2FC,_tmp2FC + 11U},{_tmp2FD,_tmp2FD,_tmp2FD + 22U},{_tmp2FE,_tmp2FE,_tmp2FE + 26U},{_tmp2FF,_tmp2FF,_tmp2FF + 16U},{_tmp300,_tmp300,_tmp300 + 16U},{_tmp301,_tmp301,_tmp301 + 24U},{_tmp302,_tmp302,_tmp302 + 25U},{_tmp303,_tmp303,_tmp303 + 21U},{_tmp304,_tmp304,_tmp304 + 22U},{_tmp305,_tmp305,_tmp305 + 16U},{_tmp306,_tmp306,_tmp306 + 19U},{_tmp307,_tmp307,_tmp307 + 25U},{_tmp308,_tmp308,_tmp308 + 35U},{_tmp309,_tmp309,_tmp309 + 23U},{_tmp30A,_tmp30A,_tmp30A + 24U},{_tmp30B,_tmp30B,_tmp30B + 18U},{_tmp30C,_tmp30C,_tmp30C + 20U},{_tmp30D,_tmp30D,_tmp30D + 19U},{_tmp30E,_tmp30E,_tmp30E + 19U},{_tmp30F,_tmp30F,_tmp30F + 14U},{_tmp310,_tmp310,_tmp310 + 19U},{_tmp311,_tmp311,_tmp311 + 20U},{_tmp312,_tmp312,_tmp312 + 14U},{_tmp313,_tmp313,_tmp313 + 11U},{_tmp314,_tmp314,_tmp314 + 23U},{_tmp315,_tmp315,_tmp315 + 18U},{_tmp316,_tmp316,_tmp316 + 30U},{_tmp317,_tmp317,_tmp317 + 8U},{_tmp318,_tmp318,_tmp318 + 12U},{_tmp319,_tmp319,_tmp319 + 14U},{_tmp31A,_tmp31A,_tmp31A + 13U},{_tmp31B,_tmp31B,_tmp31B + 23U},{_tmp31C,_tmp31C,_tmp31C + 14U},{_tmp31D,_tmp31D,_tmp31D + 18U},{_tmp31E,_tmp31E,_tmp31E + 8U},{_tmp31F,_tmp31F,_tmp31F + 11U},{_tmp320,_tmp320,_tmp320 + 20U},{_tmp321,_tmp321,_tmp321 + 9U},{_tmp322,_tmp322,_tmp322 + 16U},{_tmp323,_tmp323,_tmp323 + 19U},{_tmp324,_tmp324,_tmp324 + 10U},{_tmp325,_tmp325,_tmp325 + 16U},{_tmp326,_tmp326,_tmp326 + 11U},{_tmp327,_tmp327,_tmp327 + 14U},{_tmp328,_tmp328,_tmp328 + 11U},{_tmp329,_tmp329,_tmp329 + 15U},{_tmp32A,_tmp32A,_tmp32A + 22U},{_tmp32B,_tmp32B,_tmp32B + 16U},{_tmp32C,_tmp32C,_tmp32C + 17U},{_tmp32D,_tmp32D,_tmp32D + 12U},{_tmp32E,_tmp32E,_tmp32E + 18U},{_tmp32F,_tmp32F,_tmp32F + 17U},{_tmp330,_tmp330,_tmp330 + 12U},{_tmp331,_tmp331,_tmp331 + 16U},{_tmp332,_tmp332,_tmp332 + 11U},{_tmp333,_tmp333,_tmp333 + 10U},{_tmp334,_tmp334,_tmp334 + 14U},{_tmp335,_tmp335,_tmp335 + 15U},{_tmp336,_tmp336,_tmp336 + 20U},{_tmp337,_tmp337,_tmp337 + 27U},{_tmp338,_tmp338,_tmp338 + 10U},{_tmp339,_tmp339,_tmp339 + 18U},{_tmp33A,_tmp33A,_tmp33A + 21U},{_tmp33B,_tmp33B,_tmp33B + 19U},{_tmp33C,_tmp33C,_tmp33C + 16U},{_tmp33D,_tmp33D,_tmp33D + 20U},{_tmp33E,_tmp33E,_tmp33E + 15U},{_tmp33F,_tmp33F,_tmp33F + 20U},{_tmp340,_tmp340,_tmp340 + 15U},{_tmp341,_tmp341,_tmp341 + 12U},{_tmp342,_tmp342,_tmp342 + 20U},{_tmp343,_tmp343,_tmp343 + 19U},{_tmp344,_tmp344,_tmp344 + 20U},{_tmp345,_tmp345,_tmp345 + 21U},{_tmp346,_tmp346,_tmp346 + 21U},{_tmp347,_tmp347,_tmp347 + 12U},{_tmp348,_tmp348,_tmp348 + 17U},{_tmp349,_tmp349,_tmp349 + 19U},{_tmp34A,_tmp34A,_tmp34A + 14U},{_tmp34B,_tmp34B,_tmp34B + 17U},{_tmp34C,_tmp34C,_tmp34C + 23U},{_tmp34D,_tmp34D,_tmp34D + 13U},{_tmp34E,_tmp34E,_tmp34E + 14U},{_tmp34F,_tmp34F,_tmp34F + 16U},{_tmp350,_tmp350,_tmp350 + 16U},{_tmp351,_tmp351,_tmp351 + 8U},{_tmp352,_tmp352,_tmp352 + 19U},{_tmp353,_tmp353,_tmp353 + 20U},{_tmp354,_tmp354,_tmp354 + 14U},{_tmp355,_tmp355,_tmp355 + 19U},{_tmp356,_tmp356,_tmp356 + 20U},{_tmp357,_tmp357,_tmp357 + 11U},{_tmp358,_tmp358,_tmp358 + 22U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 23U},{_tmp35B,_tmp35B,_tmp35B + 20U},{_tmp35C,_tmp35C,_tmp35C + 22U},{_tmp35D,_tmp35D,_tmp35D + 23U},{_tmp35E,_tmp35E,_tmp35E + 24U},{_tmp35F,_tmp35F,_tmp35F + 24U},{_tmp360,_tmp360,_tmp360 + 15U},{_tmp361,_tmp361,_tmp361 + 20U},{_tmp362,_tmp362,_tmp362 + 22U},{_tmp363,_tmp363,_tmp363 + 17U},{_tmp364,_tmp364,_tmp364 + 20U},{_tmp365,_tmp365,_tmp365 + 26U},{_tmp366,_tmp366,_tmp366 + 16U},{_tmp367,_tmp367,_tmp367 + 17U},{_tmp368,_tmp368,_tmp368 + 15U},{_tmp369,_tmp369,_tmp369 + 19U},{_tmp36A,_tmp36A,_tmp36A + 17U},{_tmp36B,_tmp36B,_tmp36B + 19U},{_tmp36C,_tmp36C,_tmp36C + 25U},{_tmp36D,_tmp36D,_tmp36D + 26U},{_tmp36E,_tmp36E,_tmp36E + 9U},{_tmp36F,_tmp36F,_tmp36F + 20U},{_tmp370,_tmp370,_tmp370 + 17U},{_tmp371,_tmp371,_tmp371 + 18U},{_tmp372,_tmp372,_tmp372 + 11U},{_tmp373,_tmp373,_tmp373 + 12U}};
# 1630
static short Cyc_yyr1[536U]={0,150,151,151,151,151,151,151,151,151,151,151,151,152,153,154,155,156,156,157,157,158,158,158,159,159,160,160,160,161,161,162,162,162,162,163,163,164,165,166,167,168,168,168,168,168,168,168,169,169,170,170,170,170,170,170,170,170,170,170,170,171,171,171,171,171,171,171,172,172,173,174,174,175,175,175,175,176,176,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,178,179,179,179,180,180,180,181,181,182,182,182,183,183,183,183,183,184,184,185,185,186,186,187,187,188,189,189,190,190,191,192,192,192,192,192,192,193,193,193,193,193,193,194,195,195,196,196,196,196,197,197,198,198,199,199,199,200,200,201,201,201,201,202,202,202,203,203,204,204,205,205,206,206,206,206,206,206,206,206,206,206,207,207,207,207,207,207,207,207,207,207,207,208,208,209,210,210,211,211,211,211,211,211,211,211,212,212,212,213,213,214,214,214,215,215,215,216,216,217,217,217,217,218,218,219,219,220,220,221,221,222,222,223,223,224,224,224,224,225,225,226,226,227,227,227,228,229,229,230,230,231,231,231,231,231,232,232,232,232,233,233,234,234,235,235,236,236,237,237,237,237,237,238,238,239,239,239,240,240,240,240,240,240,240,240,240,240,240,241,241,241,241,241,241,242,243,243,244,244,245,245,245,245,245,245,245,245,246,246,246,246,246,246,247,247,247,247,247,247,248,248,248,248,248,248,248,248,248,248,248,248,248,248,249,249,249,249,249,249,249,249,250,251,251,252,252,253,253,254,254,255,255,256,256,257,257,257,258,258,258,258,258,259,259,259,260,260,260,261,261,261,261,262,262,263,263,263,263,263,263,264,265,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,267,267,267,268,268,269,269,270,270,270,271,271,272,272,273,273,273,274,274,274,274,274,274,274,274,274,274,274,275,275,275,275,275,275,275,276,277,277,278,278,279,279,280,280,281,281,282,282,282,283,283,283,283,283,284,284,284,285,285,285,286,286,286,286,287,287,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,289,289,289,290,290,290,290,290,290,290,290,290,290,290,291,291,291,291,292,292,292,292,292,292,292,292,292,292,292,293,294,294,295,295,295,295,295,296,296,297,297,298,298,299,299,300,300};
# 1687
static short Cyc_yyr2[536U]={0,1,2,3,5,3,5,7,3,3,3,3,0,1,1,2,1,0,1,4,3,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,2,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1744
static short Cyc_yydefact[1092U]={0,28,61,62,63,64,66,79,80,81,82,83,84,85,86,87,103,104,105,121,122,57,0,0,91,0,0,67,0,0,159,98,100,0,0,0,13,14,0,0,0,526,225,528,527,529,0,0,94,0,211,211,210,1,0,0,0,0,26,0,0,27,0,50,59,53,77,55,88,89,0,92,0,0,170,0,195,198,93,174,119,65,64,58,0,107,0,52,525,0,526,521,522,523,524,0,119,0,385,0,0,0,0,248,0,387,388,37,39,0,0,0,0,0,0,0,0,160,0,0,0,0,0,0,0,208,209,0,0,0,2,0,0,0,0,41,0,127,128,130,51,60,54,56,123,530,531,119,119,0,48,0,0,30,0,227,0,183,171,196,0,202,203,206,207,0,205,204,216,198,0,78,65,111,0,109,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,511,487,0,0,0,0,0,491,489,490,0,414,416,430,438,440,442,444,446,448,451,456,459,462,466,0,468,492,509,507,526,397,0,0,0,0,0,0,398,396,44,0,0,119,0,0,0,137,133,135,268,270,0,0,46,0,0,8,9,0,119,532,533,226,102,0,0,175,95,246,0,243,0,10,11,0,3,0,5,0,42,0,0,0,30,0,124,125,150,118,0,157,0,0,0,0,0,0,0,0,0,0,0,0,526,298,300,0,308,302,0,306,291,292,293,0,294,295,296,0,49,30,130,29,31,275,0,233,249,0,0,229,227,0,213,0,0,0,218,68,217,199,0,112,108,0,0,0,476,0,0,488,432,466,0,433,434,0,0,0,0,0,0,0,0,0,0,469,470,0,0,0,0,472,473,471,0,90,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,420,421,422,423,424,425,426,427,428,429,419,0,474,0,498,499,0,0,0,513,0,119,389,0,0,0,411,526,533,0,0,0,0,264,407,412,0,409,0,386,404,405,0,402,0,250,0,0,0,0,271,0,241,138,143,139,141,134,136,227,0,277,269,278,535,534,0,97,99,0,0,101,117,74,73,0,71,176,227,245,172,277,247,184,185,0,96,212,16,24,38,0,40,0,129,131,252,251,30,32,114,126,0,0,0,145,146,153,0,119,119,165,0,0,0,0,0,526,0,0,0,337,338,339,0,0,341,0,0,0,309,303,130,307,301,299,33,0,182,234,0,0,0,240,228,235,153,0,0,0,229,181,215,214,177,213,0,0,219,69,120,113,437,110,106,0,0,0,0,526,253,258,0,0,0,0,0,0,0,0,0,0,0,0,0,0,512,519,0,518,415,439,0,441,443,445,447,449,450,454,455,452,453,457,458,460,461,463,464,465,418,417,497,494,0,496,0,0,0,400,401,0,267,0,408,262,265,395,0,263,392,0,399,43,0,393,0,272,0,144,140,142,0,229,0,213,0,279,0,227,0,290,274,0,0,119,0,0,0,137,0,119,0,227,0,194,173,244,0,17,4,6,34,0,149,132,150,0,0,148,229,158,167,166,0,0,161,0,0,0,316,0,0,0,0,0,0,336,340,0,0,0,304,297,0,35,276,227,0,237,0,0,155,230,0,153,233,221,178,200,201,219,197,475,0,0,0,254,0,259,478,0,0,0,0,0,508,483,486,0,0,0,0,467,515,0,0,495,493,0,0,0,0,266,410,413,403,406,394,273,242,153,0,280,281,213,0,0,229,213,0,0,45,229,526,0,70,72,0,186,0,0,229,0,213,0,0,0,0,18,147,0,151,123,156,168,165,165,0,0,0,0,0,0,0,0,0,0,0,0,0,316,342,0,305,36,229,0,238,236,0,179,0,155,229,0,220,504,0,503,0,255,260,0,0,0,0,0,435,436,497,496,517,500,0,520,431,514,516,0,390,391,155,153,283,289,153,0,282,213,0,123,0,75,187,193,153,0,192,188,213,0,0,7,0,0,0,164,163,310,316,0,0,0,0,0,0,344,345,347,349,351,353,355,357,360,365,368,371,375,377,383,384,0,0,313,322,0,0,0,0,0,0,0,0,0,0,343,223,239,0,231,180,222,227,477,0,0,261,479,480,0,0,485,484,501,0,286,155,155,153,284,47,0,0,155,153,189,25,20,0,21,152,116,0,0,0,316,0,381,0,0,378,316,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,379,316,0,324,0,0,0,332,0,0,0,0,0,0,315,0,0,229,506,505,0,0,0,502,285,288,155,115,0,190,155,19,22,0,169,311,312,0,0,0,0,316,318,348,0,350,352,354,356,358,359,363,364,361,362,366,367,369,370,372,373,374,0,317,323,325,326,0,334,333,0,328,0,0,0,154,232,224,0,0,0,0,287,0,191,23,314,380,0,376,319,0,316,327,335,329,330,0,257,256,481,0,76,0,346,316,320,331,0,382,321,482,0,0,0};
# 1857
static short Cyc_yydefgoto[151U]={1089,53,54,55,56,482,781,782,943,662,57,319,58,303,59,484,60,486,61,151,62,63,551,242,468,469,243,66,258,244,68,173,174,69,171,70,280,281,136,137,138,282,245,450,497,498,499,672,814,71,72,677,678,679,73,500,74,473,75,76,168,169,77,125,547,334,715,635,78,636,541,706,533,537,538,444,327,266,102,103,564,489,565,424,425,426,246,320,321,637,456,306,307,308,309,310,311,796,312,313,879,880,881,882,883,884,885,886,887,888,889,890,891,892,893,894,427,435,436,428,429,430,314,206,404,207,556,208,209,210,211,212,213,214,215,216,217,218,219,220,221,821,222,581,582,223,224,80,944,431,460};
# 1875
static short Cyc_yypact[1092U]={2924,- -32768,- -32768,- -32768,- -32768,- 18,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3820,94,- 9,- -32768,3820,3395,- -32768,119,30,- -32768,28,55,26,62,86,- -32768,- -32768,100,445,196,- -32768,170,- -32768,- -32768,- -32768,187,756,234,215,176,176,- -32768,- -32768,258,275,298,2783,- -32768,142,466,- -32768,1021,3820,3820,3820,- -32768,3820,- -32768,- -32768,463,- -32768,119,3732,272,59,652,1071,- -32768,- -32768,313,351,402,- -32768,119,365,6901,- -32768,- -32768,3426,295,- -32768,- -32768,- -32768,- -32768,395,313,6901,- -32768,407,492,3426,420,480,501,- -32768,- 42,- -32768,- -32768,3905,3905,193,505,2783,2783,6901,513,- -32768,61,526,244,527,61,4374,6901,- -32768,- -32768,2783,2783,3064,- -32768,2783,3064,2783,3064,- -32768,544,552,- -32768,3601,- -32768,- -32768,- -32768,- -32768,4374,- -32768,- -32768,313,73,1761,- -32768,3732,1021,561,3905,1160,5207,- -32768,272,- -32768,562,- -32768,- -32768,- -32768,- -32768,564,- -32768,- -32768,- 15,1071,3905,- -32768,- -32768,578,581,589,119,7303,608,7402,6901,7099,612,616,628,630,636,648,653,655,658,667,7402,7402,- -32768,- -32768,- -32768,2491,670,7501,7501,7501,- -32768,- -32768,- -32768,218,- -32768,- -32768,3,624,660,663,669,545,169,556,266,231,- -32768,895,7501,112,- 28,- -32768,699,31,- -32768,3426,- 13,714,1244,359,3130,715,- -32768,- -32768,717,6901,313,3130,707,4119,4374,4459,4374,408,- -32768,- 3,- 3,- -32768,741,735,- -32768,- -32768,370,313,- -32768,- -32768,- -32768,- -32768,43,734,- -32768,- -32768,751,373,- -32768,743,- -32768,- -32768,749,- -32768,757,- -32768,767,- -32768,244,5306,3732,561,768,4374,- -32768,625,774,119,775,750,222,770,4527,771,796,801,803,5416,2349,4527,286,794,- -32768,- -32768,804,1908,1908,1021,1908,- -32768,- -32768,- -32768,806,- -32768,- -32768,- -32768,205,- -32768,561,808,- -32768,- -32768,797,85,830,- -32768,- 2,810,809,245,814,644,811,6901,3905,- -32768,825,- -32768,- -32768,85,119,- -32768,6901,826,2491,- -32768,4374,2491,- -32768,- -32768,- -32768,4637,- -32768,854,6901,6901,6901,6901,6901,6901,842,6901,4374,246,- -32768,- -32768,1908,827,377,6901,- -32768,- -32768,- -32768,6901,- -32768,7501,6901,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,6901,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6901,- -32768,61,- -32768,- -32768,5515,61,6901,- -32768,829,313,- -32768,831,832,834,- -32768,40,395,61,6901,3426,297,- -32768,- -32768,- -32768,841,833,835,- -32768,- -32768,- -32768,836,844,3426,- -32768,312,1244,840,3905,- -32768,847,848,- -32768,4459,4459,4459,- -32768,- -32768,3330,5614,158,- -32768,333,- -32768,- -32768,- 2,- -32768,- -32768,843,865,- -32768,852,- -32768,853,851,858,- -32768,3197,- -32768,593,358,- -32768,- -32768,- -32768,4374,- -32768,- -32768,- -32768,939,- -32768,2783,- -32768,2783,- -32768,- -32768,- -32768,- -32768,561,- -32768,- -32768,- -32768,911,6901,864,862,- -32768,- 4,569,313,313,805,6901,6901,860,867,6901,868,973,2202,881,- -32768,- -32768,- -32768,327,960,- -32768,5713,2055,2633,- -32768,- -32768,3601,- -32768,- -32768,- -32768,- -32768,3905,- -32768,- -32768,4374,876,4204,- -32768,- -32768,866,929,- 2,879,4289,809,- -32768,- -32768,- -32768,- -32768,644,880,75,777,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,882,888,885,907,884,- -32768,- -32768,690,5306,892,893,900,908,909,434,902,906,912,260,913,7201,- -32768,- -32768,914,918,- -32768,624,157,660,663,669,545,169,169,556,556,556,556,266,266,231,231,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,915,- -32768,44,3905,5093,4374,- -32768,4374,- -32768,903,- -32768,- -32768,- -32768,- -32768,1469,- -32768,- -32768,3311,- -32768,- -32768,922,- -32768,132,- -32768,4374,- -32768,- -32768,- -32768,917,809,921,644,916,333,3905,3993,5812,- -32768,- -32768,6901,931,313,7000,925,43,3463,928,313,3905,1160,5911,- -32768,593,- -32768,936,993,- -32768,- -32768,- -32768,999,- -32768,- -32768,625,935,6901,- -32768,809,- -32768,- -32768,- -32768,942,119,341,435,454,6901,799,495,4527,940,6010,6109,379,- -32768,- -32768,947,949,943,1908,- -32768,3732,- -32768,797,953,3905,- -32768,952,- 2,998,- -32768,951,929,204,- -32768,- -32768,- -32768,- -32768,777,- -32768,958,276,958,955,- -32768,4751,- -32768,- -32768,6901,6901,1064,6901,7099,- -32768,- -32768,- -32768,61,61,959,4865,- -32768,- -32768,6901,6901,- -32768,- -32768,85,708,982,983,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,929,963,- -32768,- -32768,644,85,964,809,644,961,503,- -32768,809,970,966,- -32768,- -32768,969,- -32768,85,975,809,976,644,967,3064,977,2783,- -32768,- -32768,6901,- -32768,4374,- -32768,978,80,805,4527,987,981,1422,979,991,4527,6901,6208,413,6307,540,6406,799,- -32768,1004,- -32768,- -32768,809,164,- -32768,- -32768,992,- -32768,1011,998,809,4374,- -32768,- -32768,385,- -32768,6901,- -32768,- -32768,5306,990,1001,1000,1007,- -32768,854,1003,1012,- -32768,- -32768,710,- -32768,- -32768,- -32768,- -32768,5093,- -32768,- -32768,998,929,- -32768,- -32768,929,1024,- -32768,644,1019,4374,1045,- -32768,- -32768,- -32768,929,1026,- -32768,- -32768,644,1010,115,- -32768,1027,1038,4374,- -32768,- -32768,1136,799,1042,7600,1037,2633,7501,1048,- -32768,12,- -32768,1088,1044,697,754,194,755,381,299,- -32768,- -32768,- -32768,- -32768,1089,7501,1908,- -32768,- -32768,551,4527,559,6505,4527,565,6604,6703,582,1062,- -32768,- -32768,- -32768,6901,1063,- -32768,- -32768,953,- -32768,422,265,- -32768,- -32768,- -32768,4374,1163,- -32768,- -32768,- -32768,4979,- -32768,998,998,929,- -32768,- -32768,1065,1068,998,929,- -32768,- -32768,- -32768,1066,592,- -32768,- -32768,576,4527,1070,799,2491,- -32768,4374,1069,- -32768,1614,7501,6901,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,6901,- -32768,799,1073,- -32768,4527,4527,610,- -32768,4527,4527,622,4527,623,6802,- -32768,1074,- 2,809,- -32768,- -32768,2633,1075,1072,- -32768,- -32768,- -32768,998,- -32768,1090,- -32768,998,- -32768,- -32768,513,- -32768,- -32768,- -32768,1079,1077,1083,7501,799,- -32768,624,325,660,663,663,545,169,169,556,556,556,556,266,266,231,231,- -32768,- -32768,- -32768,336,- -32768,- -32768,- -32768,- -32768,4527,- -32768,- -32768,4527,- -32768,4527,4527,631,- -32768,- -32768,- -32768,1086,724,1081,4374,- -32768,1084,- -32768,- -32768,- -32768,958,276,- -32768,- -32768,6901,1614,- -32768,- -32768,- -32768,- -32768,4527,- -32768,- -32768,- -32768,1085,- -32768,433,- -32768,799,- -32768,- -32768,1087,- -32768,- -32768,- -32768,1214,1217,- -32768};
# 1988
static short Cyc_yypgoto[151U]={- -32768,443,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,213,- -32768,- -32768,- 233,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6,- 95,- 14,- -32768,- -32768,0,580,- -32768,123,- 203,1105,33,- -32768,- -32768,- 110,- -32768,296,1190,- 745,- -32768,- -32768,- -32768,957,954,824,360,- -32768,- -32768,567,- 199,- 145,- -32768,- -32768,95,- -32768,- -32768,155,- 194,1159,- 406,191,- -32768,1078,- -32768,- -32768,1186,101,- -32768,524,- 127,- 151,- 150,- 442,247,530,537,- 398,- 515,- 119,- 430,- 126,- -32768,- 264,- 149,- 540,- 293,- -32768,817,- 121,- 91,- 158,- 154,- 382,76,- -32768,- -32768,- 46,- 280,- -32768,- 697,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 12,1009,- -32768,632,815,- -32768,427,732,- -32768,- 143,- 378,- 142,- 363,- 339,- 368,869,- 335,- 364,- 113,- 355,- 353,- 198,532,449,890,184,- -32768,- 357,- -32768,- 20,541,- 61,8,- 80,- 22};
# 2010
static short Cyc_yytable[7750U]={64,368,369,370,265,326,105,83,587,146,584,87,337,488,104,590,591,335,247,248,703,64,405,523,524,64,526,153,325,328,596,597,350,67,598,599,586,347,259,351,448,868,259,589,278,616,492,120,660,140,141,142,607,143,67,146,566,64,67,152,466,645,534,64,64,64,341,64,659,105,472,743,640,64,157,670,365,226,147,150,412,105,238,529,577,675,676,457,239,234,67,455,640,279,81,659,67,67,67,373,67,710,42,700,416,316,67,909,957,936,475,413,660,64,64,42,753,333,667,417,445,86,535,65,255,152,458,64,64,64,671,64,64,64,64,305,111,152,459,64,108,227,707,374,65,150,67,67,65,64,227,64,958,467,112,304,109,315,157,228,67,67,67,694,67,67,67,67,228,41,371,256,67,536,- 532,457,949,543,44,257,65,741,67,110,67,113,65,65,65,47,65,600,601,602,755,837,65,555,406,407,408,170,121,477,41,- 162,713,105,285,114,105,530,105,44,458,415,459,139,84,105,434,558,477,559,560,41,461,434,554,41,115,786,491,43,44,45,65,65,44,575,942,550,409,447,448,448,448,410,411,117,65,65,65,1014,65,65,65,65,665,1019,381,382,65,752,152,131,132,159,592,593,594,595,65,459,65,261,449,653,64,118,555,1040,87,371,315,965,966,88,452,305,305,739,305,785,453,912,64,383,384,531,124,653,722,64,64,459,64,317,536,304,304,67,304,188,532,553,249,119,566,850,250,1067,967,968,854,793,605,324,528,67,608,371,41,860,552,- 227,67,67,- 227,67,705,44,615,122,371,123,732,407,408,372,305,629,506,555,41,507,41,91,194,195,92,93,94,44,64,44,511,911,324,389,304,542,1083,518,47,916,345,390,391,198,736,127,50,51,52,1087,256,820,917,536,409,371,230,46,257,733,411,67,128,997,154,65,155,265,105,227,867,387,156,519,654,388,617,477,445,807,520,105,- 249,65,105,- 249,129,228,618,625,65,65,826,65,697,317,422,423,477,626,454,973,371,699,477,170,566,284,286,974,975,447,447,447,691,371,742,371,46,474,644,825,771,1068,525,641,371,642,789,19,20,790,41,643,1069,652,771,658,501,- 15,43,44,45,698,449,449,449,759,64,371,64,65,471,176,432,744,761,745,453,666,371,773,152,478,130,464,803,371,479,371,775,152,579,816,445,152,64,205,172,760,918,67,688,67,919,64,971,229,231,64,972,695,555,774,776,150,256,995,440,992,903,232,452,371,257,79,254,235,453,67,50,51,52,826,809,465,1054,536,67,845,252,253,67,817,371,371,921,85,1086,728,791,106,919,107,268,269,270,41,271,272,273,274,825,831,371,43,44,45,714,792,832,144,79,133,134,654,1023,1024,1020,707,839,696,79,1026,1027,105,233,79,105,477,236,65,477,65,810,145,750,148,1032,1033,79,978,1034,1035,1022,371,251,41,366,175,797,1025,237,371,106,43,44,45,853,65,826,822,379,380,644,555,106,454,65,385,386,931,65,711,932,808,833,834,79,79,260,145,262,658,938,79,474,906,439,825,371,275,79,79,79,915,79,79,79,79,1018,371,276,41,955,305,979,152,318,371,501,43,44,45,981,371,331,79,332,64,985,64,977,495,930,304,478,315,338,990,339,1011,371,343,611,346,348,348,46,1009,340,175,1010,375,840,655,517,656,362,363,545,546,67,657,67,348,348,348,1003,41,371,848,344,757,1007,1044,352,43,44,45,353,714,371,371,947,858,348,1047,1049,495,954,371,354,496,355,696,1074,50,51,52,356,994,106,366,555,106,366,106,105,1036,1037,1038,357,64,106,64,895,358,572,359,1001,1002,360,1082,50,51,52,1006,16,17,18,361,673,674,367,585,376,998,146,476,377,632,633,634,675,676,67,378,67,720,721,79,65,1066,65,794,795,414,476,437,503,438,346,508,1015,418,1016,841,842,928,929,442,609,996,961,962,536,501,969,970,79,963,964,1076,371,462,1028,1029,1030,1031,267,501,1059,847,501,41,1061,851,463,470,41,872,480,43,44,45,348,899,481,44,1055,46,862,175,505,471,483,305,870,871,47,453,330,50,51,52,485,493,50,51,52,64,502,504,509,512,65,304,65,348,513,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,514,1081,515,663,521,664,67,527,680,681,522,277,684,1078,459,689,532,539,305,540,766,544,46,264,548,146,373,557,573,934,392,348,64,610,578,621,612,613,304,614,940,106,620,623,283,622,624,628,630,646,631,647,648,980,106,264,984,106,650,649,651,822,661,668,669,67,682,683,393,394,395,396,397,398,399,400,401,402,685,686,690,692,701,704,670,490,708,712,476,716,717,476,718,719,- 532,724,65,41,403,723,1012,79,725,79,348,43,44,45,46,729,726,727,476,730,734,780,746,495,476,731,738,737,740,751,754,50,51,52,756,758,765,305,769,1042,1043,772,779,1045,1046,549,1048,784,446,787,451,64,798,804,805,764,806,304,324,811,813,65,815,490,735,823,567,568,569,570,571,829,835,574,843,844,846,849,855,856,852,580,857,865,67,583,863,283,859,861,869,41,348,873,874,897,800,802,898,43,44,45,1070,922,913,1071,603,1072,1073,910,914,495,924,41,923,925,926,941,604,50,51,52,44,580,935,927,135,160,161,162,163,164,1084,47,165,166,167,933,937,939,945,50,51,52,106,946,948,106,950,953,7,8,9,10,11,12,13,14,15,16,17,18,19,20,956,959,976,639,960,991,999,993,1004,1008,65,476,1005,1013,1041,22,23,24,1017,1060,1058,348,1063,1052,1057,476,1064,1065,476,1075,1077,30,1090,1079,1085,1091,1088,788,31,32,864,1062,866,900,902,263,905,116,908,770,38,487,158,494,783,126,819,818,1053,812,619,896,39,40,588,336,441,1080,920,576,580,748,580,627,0,0,0,0,348,0,0,0,0,0,0,322,0,0,323,348,0,0,0,0,264,42,43,0,45,46,0,324,0,88,0,0,0,0,0,0,48,49,0,264,0,0,490,0,0,0,264,0,231,0,0,0,0,0,0,0,0,0,0,0,348,0,0,0,79,0,79,0,0,0,0,0,0,0,983,0,0,987,989,106,0,0,0,89,0,0,0,490,0,0,264,0,0,0,0,419,0,0,420,91,0,264,92,93,94,0,421,44,0,0,264,0,96,0,0,0,0,0,97,763,98,99,366,422,423,768,100,0,0,1021,0,0,101,778,0,0,0,0,0,0,0,0,0,0,0,0,0,1039,0,0,145,952,0,0,348,0,0,0,580,0,0,1051,0,0,0,0,0,0,1056,0,0,0,348,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,348,0,875,876,0,0,0,0,490,0,0,827,828,0,830,0,0,0,88,0,0,264,490,0,0,838,0,0,0,0,0,446,0,0,0,264,0,0,0,0,0,0,0,0,348,0,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,0,0,88,0,0,0,0,0,0,89,0,0,0,0,0,0,0,0,0,0,0,0,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,348,0,145,877,0,98,99,0,0,490,0,100,0,0,878,89,0,101,202,0,0,203,204,0,0,490,0,747,0,0,420,91,0,0,92,93,94,0,421,44,0,0,0,0,96,0,0,0,0,0,97,348,98,99,0,422,423,0,100,0,0,283,0,0,101,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,794,795,21,177,178,287,264,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,490,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,283,0,0,188,0,0,189,190,38,191,0,0,0,0,0,264,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,301,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,296,88,26,180,0,0,0,0,181,27,0,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,687,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,296,88,0,0,0,0,0,0,0,27,0,0,0,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,0,0,0,345,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,364,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,89,0,0,0,0,0,0,0,0,22,23,24,433,0,0,225,91,0,0,92,93,94,0,95,44,30,0,0,0,96,0,0,31,32,0,97,0,98,99,0,0,0,0,100,38,0,0,0,0,101,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,324,0,0,0,0,0,0,471,0,48,49,0,0,453,0,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,749,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,88,322,0,0,0,97,0,98,99,0,0,42,43,100,45,46,0,324,0,101,0,0,0,0,452,0,48,49,0,0,453,88,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,89,0,0,0,0,0,0,0,0,0,22,23,24,0,0,90,91,0,0,92,93,94,0,95,44,0,30,0,0,96,0,89,0,31,32,97,0,98,99,0,0,0,0,100,0,38,225,91,0,101,92,93,94,0,95,44,0,39,40,0,96,0,0,0,0,0,97,0,98,99,0,0,0,0,100,0,0,0,0,0,101,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,495,0,48,49,0,0,0,0,50,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,277,0,0,0,0,48,49,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,48,49,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,240,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,241,22,23,24,0,0,0,0,48,49,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,0,0,0,0,0,48,49,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,443,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,702,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,709,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,177,178,287,0,288,289,290,291,292,293,294,295,0,0,0,179,88,0,180,297,0,0,0,181,42,298,0,0,46,182,183,184,185,186,0,0,0,0,187,48,49,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,0,0,44,0,0,196,300,149,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,561,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,563,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,824,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,836,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,1000,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,329,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,516,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,606,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,638,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,693,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,762,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,777,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,799,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,801,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,901,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,904,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,907,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,982,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,986,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,988,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,1050,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,767,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,177,178,0,197,0,0,198,0,0,0,0,199,0,0,200,179,88,201,202,0,0,203,204,0,0,0,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,735,0,0,177,178,0,197,0,0,198,0,0,0,0,199,0,0,200,179,88,201,202,0,0,203,204,0,0,0,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,342,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,345,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,0,0,0,951,0,0,198,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204};
# 2788
static short Cyc_yycheck[7750U]={0,199,200,201,123,155,26,21,376,70,373,25,170,277,26,379,380,168,109,110,535,21,220,303,304,25,306,73,155,155,385,386,181,0,387,388,375,180,118,181,243,786,122,378,139,423,279,47,478,63,64,65,409,67,21,116,349,57,25,73,17,459,64,63,64,65,176,67,474,89,264,611,454,73,74,79,197,89,70,73,108,101,124,316,364,5,6,90,130,101,57,245,474,139,112,501,63,64,65,96,67,543,117,533,117,151,73,804,96,854,264,139,542,113,114,117,631,132,496,132,241,130,124,0,116,139,129,127,128,129,134,131,132,133,134,149,110,151,141,139,110,110,540,140,21,139,113,114,25,149,110,151,140,110,128,149,128,151,158,128,127,128,129,520,131,132,133,134,128,110,126,110,139,324,134,90,873,327,119,118,57,137,149,128,151,123,63,64,65,130,67,389,390,391,636,735,73,340,86,87,88,128,47,264,110,125,131,227,135,123,230,126,232,119,129,227,141,62,124,239,232,342,283,344,345,110,248,239,338,110,130,673,278,118,119,120,113,114,119,360,125,332,130,243,447,448,449,135,136,53,127,128,129,950,131,132,133,134,491,956,91,92,139,131,278,123,124,76,381,382,383,384,149,141,151,120,243,471,278,109,423,978,296,126,278,91,92,41,130,303,304,134,306,671,136,131,296,128,129,321,124,495,566,303,304,141,306,152,459,303,304,278,306,67,110,337,123,130,611,761,127,1018,128,129,766,682,406,123,123,296,410,126,110,775,334,131,303,304,134,306,539,119,422,109,126,130,86,87,88,131,364,442,130,496,110,133,110,111,112,113,114,115,116,119,364,119,290,809,123,138,364,126,1069,297,130,817,130,146,147,133,578,123,138,139,140,1082,110,111,818,540,130,126,96,121,118,135,136,364,123,134,128,278,130,522,424,110,784,141,136,123,471,145,424,474,535,695,130,437,123,296,440,126,124,128,127,437,303,304,721,306,525,276,135,136,495,123,245,138,126,530,501,128,735,147,148,146,147,447,448,449,123,126,610,126,121,264,456,721,652,134,305,128,126,130,123,20,21,126,110,136,134,471,666,473,283,124,118,119,120,525,447,448,449,641,484,126,486,364,130,124,131,612,642,614,136,495,126,655,512,126,57,131,123,126,131,126,656,521,131,708,631,525,512,86,112,642,131,484,512,486,135,521,141,128,97,525,145,521,671,656,656,525,110,111,238,913,123,130,130,126,118,0,115,123,136,512,138,139,140,842,700,255,994,704,521,754,113,114,525,709,126,126,826,22,131,131,131,26,135,28,127,128,129,110,131,132,133,134,842,728,126,118,119,120,551,131,728,124,47,123,124,652,960,961,957,993,739,521,57,963,964,621,110,62,624,666,126,484,669,486,701,70,624,72,969,970,75,897,971,972,959,126,117,110,197,84,131,962,127,126,89,118,119,120,131,512,929,717,93,94,640,784,101,452,521,89,90,846,525,548,849,697,732,733,113,114,130,116,131,659,859,120,471,123,237,929,126,123,127,128,129,816,131,132,133,134,956,126,126,110,878,695,131,697,123,126,495,118,119,120,131,126,130,152,130,695,131,697,896,130,845,695,126,697,126,123,125,131,126,177,414,179,180,181,121,123,127,176,126,95,742,128,295,130,192,193,82,83,695,136,697,199,200,201,933,110,126,759,130,638,939,131,130,118,119,120,130,714,126,126,869,773,220,131,131,130,877,126,130,134,130,685,131,138,139,140,130,917,227,342,913,230,345,232,794,973,974,975,130,779,239,781,794,130,357,130,931,932,130,1069,138,139,140,938,17,18,19,130,502,503,130,374,142,924,865,264,143,447,448,449,5,6,779,144,781,125,126,276,695,1017,697,22,23,124,283,110,285,110,296,288,951,117,953,125,126,125,126,130,411,919,143,144,993,652,89,90,305,93,94,125,126,110,965,966,967,968,124,666,1003,758,669,110,1007,762,129,131,110,791,125,118,119,120,340,797,125,119,997,121,777,338,130,130,125,897,789,790,130,136,156,138,139,140,125,125,138,139,140,897,124,124,130,130,779,897,781,373,110,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,123,1068,123,484,134,486,897,125,505,506,130,127,509,1058,141,512,110,131,956,134,648,131,121,123,137,1010,96,125,110,852,59,423,956,128,131,126,129,129,956,129,863,424,125,131,144,134,126,131,125,130,126,110,124,901,437,155,904,440,131,130,126,1065,47,123,126,956,130,124,97,98,99,100,101,102,103,104,105,106,134,30,123,45,130,141,79,277,131,131,471,131,126,474,131,110,134,126,897,110,127,131,948,484,126,486,496,118,119,120,121,131,126,126,495,131,125,46,137,130,501,131,126,131,131,125,131,138,139,140,131,137,123,1069,131,981,982,131,124,985,986,331,988,130,242,125,244,1069,130,124,123,646,131,1069,123,125,80,956,129,349,124,128,352,353,354,355,356,25,131,359,110,110,131,131,126,131,137,367,131,124,1069,371,137,281,131,131,130,110,578,124,131,134,687,688,125,118,119,120,1044,131,130,1047,392,1049,1050,123,117,130,130,110,131,126,131,125,404,138,139,140,119,409,123,131,123,74,75,76,77,78,1074,130,81,82,83,131,111,131,131,138,139,140,621,125,28,624,124,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,134,95,95,453,142,125,25,126,125,125,1069,652,126,125,123,37,38,39,131,111,130,671,125,131,131,666,131,126,669,125,131,53,0,131,131,0,131,678,60,61,779,1010,781,798,799,122,801,39,803,651,72,276,75,281,669,51,714,709,993,704,425,794,84,85,377,169,239,1065,823,361,520,621,522,440,- 1,- 1,- 1,- 1,728,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,739,- 1,- 1,- 1,- 1,452,117,118,- 1,120,121,- 1,123,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,471,- 1,- 1,566,- 1,- 1,- 1,478,- 1,877,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,784,- 1,- 1,- 1,779,- 1,781,- 1,- 1,- 1,- 1,- 1,- 1,- 1,903,- 1,- 1,906,907,794,- 1,- 1,- 1,95,- 1,- 1,- 1,611,- 1,- 1,522,- 1,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,533,114,115,116,- 1,118,119,- 1,- 1,542,- 1,124,- 1,- 1,- 1,- 1,- 1,130,643,132,133,951,135,136,649,138,- 1,- 1,958,- 1,- 1,144,657,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,976,- 1,- 1,865,875,- 1,- 1,878,- 1,- 1,- 1,682,- 1,- 1,990,- 1,- 1,- 1,- 1,- 1,- 1,997,- 1,- 1,- 1,896,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,913,- 1,25,26,- 1,- 1,- 1,- 1,721,- 1,- 1,724,725,- 1,727,- 1,- 1,- 1,41,- 1,- 1,642,735,- 1,- 1,738,- 1,- 1,- 1,- 1,- 1,652,- 1,- 1,- 1,656,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,957,- 1,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,975,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,- 1,- 1,- 1,124,- 1,- 1,1017,- 1,1010,130,- 1,132,133,- 1,- 1,826,- 1,138,- 1,- 1,141,95,- 1,144,145,- 1,- 1,148,149,- 1,- 1,842,- 1,107,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,- 1,130,1068,132,133,- 1,135,136,- 1,138,- 1,- 1,786,- 1,- 1,144,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,818,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,929,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,854,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,869,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,51,52,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,65,66,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,51,52,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,65,66,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,51,52,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,65,66,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,107,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,53,- 1,- 1,- 1,124,- 1,- 1,60,61,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,72,- 1,- 1,- 1,- 1,144,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,136,- 1,138,139,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,- 1,- 1,- 1,124,41,107,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,117,118,138,120,121,- 1,123,- 1,144,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,136,41,138,139,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,53,- 1,- 1,124,- 1,95,- 1,60,61,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,72,110,111,- 1,144,114,115,116,- 1,118,119,- 1,84,85,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,132,133,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,- 1,124,37,38,39,40,- 1,- 1,- 1,132,133,- 1,- 1,- 1,49,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,- 1,124,37,38,39,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,48,117,50,- 1,- 1,121,54,55,56,57,58,- 1,- 1,- 1,- 1,63,132,133,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,25,26,- 1,138,- 1,- 1,141,32,- 1,144,145,- 1,- 1,148,149,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,25,26,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,25,26,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,40,41,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,25,26,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,40,41,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};
# 143 "cycbison.simple"
static struct Cyc_Yystacktype*Cyc_yyget_yyyyvsp(struct _fat_ptr yyvs,int yyvsp_offset,int rulelen){
# 146
return(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset - rulelen),sizeof(struct Cyc_Yystacktype),12U));}struct _tuple27{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _fat_ptr f1;void*f2;};static char _tmp3F4[8U]="stdcall";static char _tmp3F5[6U]="cdecl";static char _tmp3F6[9U]="fastcall";static char _tmp3F7[9U]="noreturn";static char _tmp3F8[6U]="const";static char _tmp3F9[8U]="aligned";static char _tmp3FA[7U]="packed";static char _tmp3FB[7U]="shared";static char _tmp3FC[7U]="unused";static char _tmp3FD[5U]="weak";static char _tmp3FE[10U]="dllimport";static char _tmp3FF[10U]="dllexport";static char _tmp400[23U]="no_instrument_function";static char _tmp401[12U]="constructor";static char _tmp402[11U]="destructor";static char _tmp403[22U]="no_check_memory_usage";static char _tmp404[5U]="pure";static char _tmp405[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 152
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 155
struct _RegionHandle _tmp374=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp374;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 161
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp714;(_tmp714.YYINITIALSVAL).tag=64U,(_tmp714.YYINITIALSVAL).val=0;_tmp714;});
int yynerrs;
# 165
struct Cyc_Yyltype yylloc;
# 169
int yyssp_offset;
# 171
struct _fat_ptr yyss=({unsigned _tmp664=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp664),sizeof(short),_tmp664);});
# 173
int yyvsp_offset;
# 175
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp663=200U;struct Cyc_Yystacktype*_tmp662=({struct _RegionHandle*_tmp7C8=yyregion;_region_malloc(_tmp7C8,_check_times(_tmp663,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp713=200U;unsigned i;for(i=0;i < _tmp713;++ i){(_tmp662[i]).v=yylval,({struct Cyc_Yyltype _tmp7C9=Cyc_yynewloc();(_tmp662[i]).l=_tmp7C9;});}}0;});_tmp662;}),sizeof(struct Cyc_Yystacktype),200U);
# 181
struct Cyc_Yystacktype*yyyyvsp;
# 184
int yystacksize=200;
# 186
union Cyc_YYSTYPE yyval=yylval;
# 190
int yylen;
# 197
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 207
yyssp_offset=-1;
yyvsp_offset=0;
# 213
yynewstate:
# 215
*((short*)_check_fat_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 217
if(yyssp_offset >= (yystacksize - 1)- 12){
# 219
if(yystacksize >= 10000){
({struct _fat_ptr _tmp7CB=({const char*_tmp375="parser stack overflow";_tag_fat(_tmp375,sizeof(char),22U);});int _tmp7CA=yystate;Cyc_yyerror(_tmp7CB,_tmp7CA,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 223
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp379=(unsigned)yystacksize;short*_tmp378=({struct _RegionHandle*_tmp7CC=yyregion;_region_malloc(_tmp7CC,_check_times(_tmp379,sizeof(short)));});({{unsigned _tmp6D1=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp6D1;++ i){
i <= (unsigned)yyssp_offset?_tmp378[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp378[i]=0);}}0;});_tag_fat(_tmp378,sizeof(short),_tmp379);});
# 229
struct _fat_ptr yyvs1=({unsigned _tmp377=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp376=({struct _RegionHandle*_tmp7CD=yyregion;_region_malloc(_tmp7CD,_check_times(_tmp377,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp6D0=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp6D0;++ i){
# 231
i <= (unsigned)yyssp_offset?_tmp376[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp376[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp376,sizeof(struct Cyc_Yystacktype),_tmp377);});
# 237
yyss=yyss1;
yyvs=yyvs1;};}
# 247
goto yybackup;
# 249
yybackup:
# 261 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1092U,sizeof(short),yystate));
if(yyn == -32768)goto yydefault;
# 268
if(yychar == -2)
# 274
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 278
if(yychar <= 0){
# 280
yychar1=0;
yychar=0;}else{
# 289
yychar1=yychar > 0 && yychar <= 377?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,378U,sizeof(short),yychar)): 301;}
# 306 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7749)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),yyn))!= yychar1)goto yydefault;
# 309
yyn=(int)Cyc_yytable[yyn];
# 316
if(yyn < 0){
# 318
if(yyn == -32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 322
if(yyn == 0)goto yyerrlab;}
# 324
if(yyn == 1091){
int _tmp37A=0;_npop_handler(0U);return _tmp37A;}
# 335 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 339
({struct Cyc_Yystacktype _tmp7CE=({struct Cyc_Yystacktype _tmp6D2;_tmp6D2.v=yylval,_tmp6D2.l=yylloc;_tmp6D2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp7CE;});
# 345
if(yyerrstatus != 0)-- yyerrstatus;
# 347
yystate=yyn;
goto yynewstate;
# 351
yydefault:
# 353
 yyn=(int)Cyc_yydefact[yystate];
if(yyn == 0)goto yyerrlab;
# 358
yyreduce:
# 360
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,536U,sizeof(short),yyn));
if(yylen > 0)
yyval=(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen))).v;
# 377 "cycbison.simple"
{int _tmp37B=yyn;switch(_tmp37B){case 1U: _LL1: _LL2:
# 380
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1186 "parse.y"
yyval=(yyyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyyvsp[0]).v);
# 1189
goto _LL0;case 2U: _LL3: _LL4:
# 1191
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp7CF=Cyc_yyget_YY16(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7CF,Cyc_yyget_YY16(&(yyyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1195
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp37E=_cycalloc(sizeof(*_tmp37E));({struct Cyc_Absyn_Decl*_tmp7D3=({struct Cyc_Absyn_Decl*_tmp37D=_cycalloc(sizeof(*_tmp37D));({void*_tmp7D2=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->tag=10U,({struct _tuple0*_tmp7D1=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp37C->f1=_tmp7D1;}),({struct Cyc_List_List*_tmp7D0=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp37C->f2=_tmp7D0;});_tmp37C;});_tmp37D->r=_tmp7D2;}),_tmp37D->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp37D;});_tmp37E->hd=_tmp7D3;}),_tmp37E->tl=0;_tmp37E;}));
Cyc_Lex_leave_using();
# 1199
goto _LL0;case 4U: _LL7: _LL8:
# 1201
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 1200 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp381=_cycalloc(sizeof(*_tmp381));({struct Cyc_Absyn_Decl*_tmp7D8=({struct Cyc_Absyn_Decl*_tmp380=_cycalloc(sizeof(*_tmp380));({void*_tmp7D7=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->tag=10U,({struct _tuple0*_tmp7D6=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp37F->f1=_tmp7D6;}),({struct Cyc_List_List*_tmp7D5=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp37F->f2=_tmp7D5;});_tmp37F;});_tmp380->r=_tmp7D7;}),_tmp380->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp380;});_tmp381->hd=_tmp7D8;}),({struct Cyc_List_List*_tmp7D4=Cyc_yyget_YY16(&(yyyyvsp[4]).v);_tmp381->tl=_tmp7D4;});_tmp381;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1203
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1203 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp385=_cycalloc(sizeof(*_tmp385));({struct Cyc_Absyn_Decl*_tmp7DD=({struct Cyc_Absyn_Decl*_tmp384=_cycalloc(sizeof(*_tmp384));({void*_tmp7DC=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->tag=9U,({struct _fat_ptr*_tmp7DB=({struct _fat_ptr*_tmp382=_cycalloc(sizeof(*_tmp382));({struct _fat_ptr _tmp7DA=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp382=_tmp7DA;});_tmp382;});_tmp383->f1=_tmp7DB;}),({struct Cyc_List_List*_tmp7D9=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp383->f2=_tmp7D9;});_tmp383;});_tmp384->r=_tmp7DC;}),_tmp384->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp384;});_tmp385->hd=_tmp7DD;}),_tmp385->tl=0;_tmp385;}));
Cyc_Lex_leave_namespace();
# 1206
goto _LL0;case 6U: _LLB: _LLC:
# 1208
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 1207 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));({struct Cyc_Absyn_Decl*_tmp7E3=({struct Cyc_Absyn_Decl*_tmp388=_cycalloc(sizeof(*_tmp388));({void*_tmp7E2=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->tag=9U,({struct _fat_ptr*_tmp7E1=({struct _fat_ptr*_tmp386=_cycalloc(sizeof(*_tmp386));({struct _fat_ptr _tmp7E0=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp386=_tmp7E0;});_tmp386;});_tmp387->f1=_tmp7E1;}),({struct Cyc_List_List*_tmp7DF=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp387->f2=_tmp7DF;});_tmp387;});_tmp388->r=_tmp7E2;}),_tmp388->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp388;});_tmp389->hd=_tmp7E3;}),({struct Cyc_List_List*_tmp7DE=Cyc_yyget_YY16(&(yyyyvsp[4]).v);_tmp389->tl=_tmp7DE;});_tmp389;}));
goto _LL0;case 7U: _LLD: _LLE:
# 1210
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);{
# 1209 "parse.y"
int _tmp38A=Cyc_yyget_YY31(&(yyyyvsp[0]).v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyyvsp[4]).v);
struct Cyc_List_List*exs=Cyc_yyget_YY52(&(yyyyvsp[5]).v);
if(!_tmp38A){
if(exs != 0 || cycdecls != 0){
({void*_tmp38B=0U;({unsigned _tmp7E5=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp7E4=({const char*_tmp38C="expecting \"C include\"";_tag_fat(_tmp38C,sizeof(char),22U);});Cyc_Warn_err(_tmp7E5,_tmp7E4,_tag_fat(_tmp38B,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp38F=_cycalloc(sizeof(*_tmp38F));({struct Cyc_Absyn_Decl*_tmp7E9=({struct Cyc_Absyn_Decl*_tmp38E=_cycalloc(sizeof(*_tmp38E));({void*_tmp7E8=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->tag=12U,({struct Cyc_List_List*_tmp7E7=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp38D->f1=_tmp7E7;}),_tmp38D->f2=cycdecls,_tmp38D->f3=exs;_tmp38D;});_tmp38E->r=_tmp7E8;}),_tmp38E->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp38E;});_tmp38F->hd=_tmp7E9;}),({struct Cyc_List_List*_tmp7E6=Cyc_yyget_YY16(&(yyyyvsp[6]).v);_tmp38F->tl=_tmp7E6;});_tmp38F;}));}else{
# 1218
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));({struct Cyc_Absyn_Decl*_tmp7ED=({struct Cyc_Absyn_Decl*_tmp391=_cycalloc(sizeof(*_tmp391));({void*_tmp7EC=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->tag=11U,({struct Cyc_List_List*_tmp7EB=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp390->f1=_tmp7EB;});_tmp390;});_tmp391->r=_tmp7EC;}),_tmp391->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp391;});_tmp392->hd=_tmp7ED;}),({struct Cyc_List_List*_tmp7EA=Cyc_yyget_YY16(&(yyyyvsp[6]).v);_tmp392->tl=_tmp7EA;});_tmp392;}));}}else{
# 1222
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp395=_cycalloc(sizeof(*_tmp395));({struct Cyc_Absyn_Decl*_tmp7F1=({struct Cyc_Absyn_Decl*_tmp394=_cycalloc(sizeof(*_tmp394));({void*_tmp7F0=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->tag=12U,({struct Cyc_List_List*_tmp7EF=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp393->f1=_tmp7EF;}),_tmp393->f2=cycdecls,_tmp393->f3=exs;_tmp393;});_tmp394->r=_tmp7F0;}),_tmp394->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp394;});_tmp395->hd=_tmp7F1;}),({struct Cyc_List_List*_tmp7EE=Cyc_yyget_YY16(&(yyyyvsp[6]).v);_tmp395->tl=_tmp7EE;});_tmp395;}));}
# 1225
goto _LL0;};case 8U: _LLF: _LL10:
# 1227
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1226 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));({struct Cyc_Absyn_Decl*_tmp7F3=({struct Cyc_Absyn_Decl*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp396->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp396;});_tmp397->hd=_tmp7F3;}),({struct Cyc_List_List*_tmp7F2=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp397->tl=_tmp7F2;});_tmp397;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1229
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1228 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));({struct Cyc_Absyn_Decl*_tmp7F5=({struct Cyc_Absyn_Decl*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp398->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp398;});_tmp399->hd=_tmp7F5;}),({struct Cyc_List_List*_tmp7F4=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp399->tl=_tmp7F4;});_tmp399;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1231
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1230 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct Cyc_Absyn_Decl*_tmp7F7=({struct Cyc_Absyn_Decl*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp39A->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp39A;});_tmp39B->hd=_tmp7F7;}),({struct Cyc_List_List*_tmp7F6=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp39B->tl=_tmp7F6;});_tmp39B;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1233
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1232 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct Cyc_Absyn_Decl*_tmp7F9=({struct Cyc_Absyn_Decl*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp39C->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp39C;});_tmp39D->hd=_tmp7F9;}),({struct Cyc_List_List*_tmp7F8=Cyc_yyget_YY16(&(yyyyvsp[2]).v);_tmp39D->tl=_tmp7F8;});_tmp39D;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1235
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1238
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1238 "parse.y"
Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1241
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1243 "parse.y"
Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E:
# 1246
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1248 "parse.y"
struct _fat_ptr _tmp39E=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp7FA=(struct _fat_ptr)_tmp39E;Cyc_strcmp(_tmp7FA,({const char*_tmp39F="C";_tag_fat(_tmp39F,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp7FB=(struct _fat_ptr)_tmp39E;Cyc_strcmp(_tmp7FB,({const char*_tmp3A0="C include";_tag_fat(_tmp3A0,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1255
({void*_tmp3A1=0U;({unsigned _tmp7FD=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp7FC=({const char*_tmp3A2="expecting \"C\" or \"C include\"";_tag_fat(_tmp3A2,sizeof(char),29U);});Cyc_Warn_err(_tmp7FD,_tmp7FC,_tag_fat(_tmp3A1,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1259
goto _LL0;};case 16U: _LL1F: _LL20:
# 1261
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1265
 yyval=Cyc_YY52(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1268
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1267 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1270
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
yyval=(yyyyvsp[2]).v;
goto _LL0;case 20U: _LL27: _LL28:
# 1274
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1272 "parse.y"
yyval=Cyc_YY52(0);
goto _LL0;case 21U: _LL29: _LL2A:
# 1275
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct _tuple27*_tmp7FF=({struct _tuple27*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->f1=(unsigned)((yyyyvsp[0]).l).first_line,({struct _tuple0*_tmp7FE=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp3A3->f2=_tmp7FE;}),_tmp3A3->f3=0;_tmp3A3;});_tmp3A4->hd=_tmp7FF;}),_tmp3A4->tl=0;_tmp3A4;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1279
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1277 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct _tuple27*_tmp801=({struct _tuple27*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->f1=(unsigned)((yyyyvsp[0]).l).first_line,({struct _tuple0*_tmp800=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp3A5->f2=_tmp800;}),_tmp3A5->f3=0;_tmp3A5;});_tmp3A6->hd=_tmp801;}),_tmp3A6->tl=0;_tmp3A6;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1280
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1279 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct _tuple27*_tmp804=({struct _tuple27*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->f1=(unsigned)((yyyyvsp[0]).l).first_line,({struct _tuple0*_tmp803=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp3A7->f2=_tmp803;}),_tmp3A7->f3=0;_tmp3A7;});_tmp3A8->hd=_tmp804;}),({struct Cyc_List_List*_tmp802=Cyc_yyget_YY52(&(yyyyvsp[2]).v);_tmp3A8->tl=_tmp802;});_tmp3A8;}));
goto _LL0;case 24U: _LL2F: _LL30:
# 1282
 yyval=Cyc_YY16(0);
goto _LL0;case 25U: _LL31: _LL32:
# 1285
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1284 "parse.y"
yyval=(yyyyvsp[2]).v;
goto _LL0;case 26U: _LL33: _LL34:
# 1287
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct Cyc_Absyn_Decl*_tmp807=({void*_tmp806=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp805=Cyc_yyget_YY15(&(yyyyvsp[0]).v);_tmp3A9->f1=_tmp805;});_tmp3A9;});Cyc_Absyn_new_decl(_tmp806,(unsigned)((yyyyvsp[0]).l).first_line);});_tmp3AA->hd=_tmp807;}),_tmp3AA->tl=0;_tmp3AA;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1291
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1289 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 28U: _LL37: _LL38:
# 1292
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1290 "parse.y"
yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL39: _LL3A:
# 1293
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1299 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp80A=yyr;struct Cyc_Parse_Declarator _tmp809=Cyc_yyget_YY27(&(yyyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp808=Cyc_yyget_Stmt_tok(&(yyyyvsp[1]).v);Cyc_Parse_make_function(_tmp80A,0,_tmp809,0,_tmp808,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 32U: _LL3B: _LL3C:
# 1302
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 1301 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3AB=Cyc_yyget_YY17(&(yyyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp80D=yyr;struct Cyc_Parse_Declarator _tmp80C=Cyc_yyget_YY27(&(yyyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp80B=Cyc_yyget_Stmt_tok(&(yyyyvsp[2]).v);Cyc_Parse_make_function(_tmp80D,& _tmp3AB,_tmp80C,0,_tmp80B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;};case 33U: _LL3D: _LL3E:
# 1305
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1314 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp811=yyr;struct Cyc_Parse_Declarator _tmp810=Cyc_yyget_YY27(&(yyyyvsp[0]).v);struct Cyc_List_List*_tmp80F=Cyc_yyget_YY16(&(yyyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp80E=Cyc_yyget_Stmt_tok(&(yyyyvsp[2]).v);Cyc_Parse_make_function(_tmp811,0,_tmp810,_tmp80F,_tmp80E,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 34U: _LL3F: _LL40:
# 1317
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 1316 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3AC=Cyc_yyget_YY17(&(yyyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp815=yyr;struct Cyc_Parse_Declarator _tmp814=Cyc_yyget_YY27(&(yyyyvsp[1]).v);struct Cyc_List_List*_tmp813=Cyc_yyget_YY16(&(yyyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp812=Cyc_yyget_Stmt_tok(&(yyyyvsp[3]).v);Cyc_Parse_make_function(_tmp815,& _tmp3AC,_tmp814,_tmp813,_tmp812,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;};case 35U: _LL41: _LL42:
# 1320
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 1324 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3AD=Cyc_yyget_YY17(&(yyyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp818=yyr;struct Cyc_Parse_Declarator _tmp817=Cyc_yyget_YY27(&(yyyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp816=Cyc_yyget_Stmt_tok(&(yyyyvsp[2]).v);Cyc_Parse_make_function(_tmp818,& _tmp3AD,_tmp817,0,_tmp816,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;};case 36U: _LL43: _LL44:
# 1328
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 1327 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3AE=Cyc_yyget_YY17(&(yyyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp81C=yyr;struct Cyc_Parse_Declarator _tmp81B=Cyc_yyget_YY27(&(yyyyvsp[1]).v);struct Cyc_List_List*_tmp81A=Cyc_yyget_YY16(&(yyyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp819=Cyc_yyget_Stmt_tok(&(yyyyvsp[3]).v);Cyc_Parse_make_function(_tmp81C,& _tmp3AE,_tmp81B,_tmp81A,_tmp819,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;};case 37U: _LL45: _LL46:
# 1331
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v));yyval=(yyyyvsp[1]).v;
goto _LL0;case 38U: _LL47: _LL48:
# 1335
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1335 "parse.y"
Cyc_Lex_leave_using();
goto _LL0;case 39U: _LL49: _LL4A:
# 1338
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1338 "parse.y"
Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));({struct _fat_ptr _tmp81D=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);*_tmp3AF=_tmp81D;});_tmp3AF;}));yyval=(yyyyvsp[1]).v;
goto _LL0;case 40U: _LL4B: _LL4C:
# 1341
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1341 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL0;case 41U: _LL4D: _LL4E:
# 1344
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1347 "parse.y"
int _tmp3B0=((yyyyvsp[0]).l).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp81F=Cyc_yyget_YY17(&(yyyyvsp[0]).v);unsigned _tmp81E=(unsigned)_tmp3B0;Cyc_Parse_make_declarations(_tmp81F,0,_tmp81E,(unsigned)_tmp3B0);}));
goto _LL0;};case 42U: _LL4F: _LL50:
# 1351
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 1350 "parse.y"
int _tmp3B1=((yyyyvsp[0]).l).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp822=Cyc_yyget_YY17(&(yyyyvsp[0]).v);struct _tuple12*_tmp821=Cyc_yyget_YY19(&(yyyyvsp[1]).v);unsigned _tmp820=(unsigned)((yyyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp822,_tmp821,_tmp820,(unsigned)_tmp3B1);}));
goto _LL0;};case 43U: _LL51: _LL52:
# 1354
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 1354 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct Cyc_Absyn_Decl*_tmp825=({struct Cyc_Absyn_Pat*_tmp824=Cyc_yyget_YY9(&(yyyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp823=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp824,_tmp823,(unsigned)((yyyyvsp[0]).l).first_line);});_tmp3B2->hd=_tmp825;}),_tmp3B2->tl=0;_tmp3B2;}));
goto _LL0;case 44U: _LL53: _LL54:
# 1357
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 1356 "parse.y"
struct Cyc_List_List*_tmp3B3=0;
{struct Cyc_List_List*_tmp3B4=Cyc_yyget_YY36(&(yyyyvsp[1]).v);for(0;_tmp3B4 != 0;_tmp3B4=_tmp3B4->tl){
struct _fat_ptr*_tmp3B5=(struct _fat_ptr*)_tmp3B4->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({union Cyc_Absyn_Nmspace _tmp826=Cyc_Absyn_Rel_n(0);_tmp3B8->f1=_tmp826;}),_tmp3B8->f2=_tmp3B5;_tmp3B8;});
struct Cyc_Absyn_Vardecl*_tmp3B6=({struct _tuple0*_tmp827=qv;Cyc_Absyn_new_vardecl(0U,_tmp827,Cyc_Absyn_wildtyp(0),0);});
_tmp3B3=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->hd=_tmp3B6,_tmp3B7->tl=_tmp3B3;_tmp3B7;});}}
# 1363
_tmp3B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3B3);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct Cyc_Absyn_Decl*_tmp828=Cyc_Absyn_letv_decl(_tmp3B3,(unsigned)((yyyyvsp[0]).l).first_line);_tmp3B9->hd=_tmp828;}),_tmp3B9->tl=0;_tmp3B9;}));
# 1366
goto _LL0;};case 45U: _LL55: _LL56:
# 1368
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
struct _fat_ptr _tmp3BA=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);
# 1371
if(({struct _fat_ptr _tmp829=(struct _fat_ptr)_tmp3BA;Cyc_zstrcmp(_tmp829,({const char*_tmp3BB="`H";_tag_fat(_tmp3BB,sizeof(char),3U);}));})== 0)
({void*_tmp3BC=0U;({unsigned _tmp82B=(unsigned)((yyyyvsp[2]).l).first_line;struct _fat_ptr _tmp82A=({const char*_tmp3BD="bad occurrence of heap region";_tag_fat(_tmp3BD,sizeof(char),30U);});Cyc_Warn_err(_tmp82B,_tmp82A,_tag_fat(_tmp3BC,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp82C=(struct _fat_ptr)_tmp3BA;Cyc_zstrcmp(_tmp82C,({const char*_tmp3BE="`U";_tag_fat(_tmp3BE,sizeof(char),3U);}));})== 0)
({void*_tmp3BF=0U;({unsigned _tmp82E=(unsigned)((yyyyvsp[2]).l).first_line;struct _fat_ptr _tmp82D=({const char*_tmp3C0="bad occurrence of unique region";_tag_fat(_tmp3C0,sizeof(char),32U);});Cyc_Warn_err(_tmp82E,_tmp82D,_tag_fat(_tmp3BF,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct _fat_ptr*_tmp830=({struct _fat_ptr*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));*_tmp3C4=_tmp3BA;_tmp3C4;});_tmp3C5->name=_tmp830;}),_tmp3C5->identity=- 1,({void*_tmp82F=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3C5->kind=_tmp82F;});_tmp3C5;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp834=(unsigned)((yyyyvsp[4]).l).first_line;struct _tuple0*_tmp833=({struct _tuple0*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp832=({struct _fat_ptr*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct _fat_ptr _tmp831=Cyc_yyget_String_tok(&(yyyyvsp[4]).v);*_tmp3C2=_tmp831;});_tmp3C2;});_tmp3C3->f2=_tmp832;});_tmp3C3;});Cyc_Absyn_new_vardecl(_tmp834,_tmp833,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({struct Cyc_Absyn_Decl*_tmp835=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyyvsp[0]).l).first_line);_tmp3C1->hd=_tmp835;}),_tmp3C1->tl=0;_tmp3C1;}));
# 1380
goto _LL0;};};case 46U: _LL57: _LL58:
# 1382
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 1382 "parse.y"
struct _fat_ptr _tmp3C6=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);
if(({struct _fat_ptr _tmp836=(struct _fat_ptr)_tmp3C6;Cyc_zstrcmp(_tmp836,({const char*_tmp3C7="H";_tag_fat(_tmp3C7,sizeof(char),2U);}));})== 0)
({void*_tmp3C8=0U;({unsigned _tmp838=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmp837=({const char*_tmp3C9="bad occurrence of heap region `H";_tag_fat(_tmp3C9,sizeof(char),33U);});Cyc_Warn_err(_tmp838,_tmp837,_tag_fat(_tmp3C8,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp839=(struct _fat_ptr)_tmp3C6;Cyc_zstrcmp(_tmp839,({const char*_tmp3CA="U";_tag_fat(_tmp3CA,sizeof(char),2U);}));})== 0)
({void*_tmp3CB=0U;({unsigned _tmp83B=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmp83A=({const char*_tmp3CC="bad occurrence of unique region `U";_tag_fat(_tmp3CC,sizeof(char),35U);});Cyc_Warn_err(_tmp83B,_tmp83A,_tag_fat(_tmp3CB,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _fat_ptr*_tmp83F=({struct _fat_ptr*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct _fat_ptr _tmp83E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D2=({struct Cyc_String_pa_PrintArg_struct _tmp6D3;_tmp6D3.tag=0U,_tmp6D3.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp3C6);_tmp6D3;});void*_tmp3D0[1U];_tmp3D0[0]=& _tmp3D2;({struct _fat_ptr _tmp83D=({const char*_tmp3D1="`%s";_tag_fat(_tmp3D1,sizeof(char),4U);});Cyc_aprintf(_tmp83D,_tag_fat(_tmp3D0,sizeof(void*),1U));});});*_tmp3D3=_tmp83E;});_tmp3D3;});_tmp3D4->name=_tmp83F;}),_tmp3D4->identity=- 1,({
void*_tmp83C=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3D4->kind=_tmp83C;});_tmp3D4;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp842=(unsigned)((yyyyvsp[1]).l).first_line;struct _tuple0*_tmp841=({struct _tuple0*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp840=({struct _fat_ptr*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));*_tmp3CE=_tmp3C6;_tmp3CE;});_tmp3CF->f2=_tmp840;});_tmp3CF;});Cyc_Absyn_new_vardecl(_tmp842,_tmp841,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct Cyc_Absyn_Decl*_tmp843=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyyvsp[0]).l).first_line);_tmp3CD->hd=_tmp843;}),_tmp3CD->tl=0;_tmp3CD;}));
# 1393
goto _LL0;};};case 47U: _LL59: _LL5A:
# 1395
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 1395 "parse.y"
struct _fat_ptr _tmp3D5=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);
struct _fat_ptr _tmp3D6=Cyc_yyget_String_tok(&(yyyyvsp[3]).v);
struct Cyc_Absyn_Exp*_tmp3D7=Cyc_yyget_Exp_tok(&(yyyyvsp[5]).v);
if(({struct _fat_ptr _tmp844=(struct _fat_ptr)_tmp3D6;Cyc_strcmp(_tmp844,({const char*_tmp3D8="open";_tag_fat(_tmp3D8,sizeof(char),5U);}));})!= 0)({void*_tmp3D9=0U;({unsigned _tmp846=(unsigned)((yyyyvsp[3]).l).first_line;struct _fat_ptr _tmp845=({const char*_tmp3DA="expecting `open'";_tag_fat(_tmp3DA,sizeof(char),17U);});Cyc_Warn_err(_tmp846,_tmp845,_tag_fat(_tmp3D9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct _fat_ptr*_tmp84A=({struct _fat_ptr*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));({struct _fat_ptr _tmp849=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E0=({struct Cyc_String_pa_PrintArg_struct _tmp6D4;_tmp6D4.tag=0U,_tmp6D4.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp3D5);_tmp6D4;});void*_tmp3DE[1U];_tmp3DE[0]=& _tmp3E0;({struct _fat_ptr _tmp848=({const char*_tmp3DF="`%s";_tag_fat(_tmp3DF,sizeof(char),4U);});Cyc_aprintf(_tmp848,_tag_fat(_tmp3DE,sizeof(void*),1U));});});*_tmp3E1=_tmp849;});_tmp3E1;});_tmp3E2->name=_tmp84A;}),_tmp3E2->identity=- 1,({
void*_tmp847=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3E2->kind=_tmp847;});_tmp3E2;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp84D=(unsigned)((yyyyvsp[2]).l).first_line;struct _tuple0*_tmp84C=({struct _tuple0*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp84B=({struct _fat_ptr*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));*_tmp3DC=_tmp3D5;_tmp3DC;});_tmp3DD->f2=_tmp84B;});_tmp3DD;});Cyc_Absyn_new_vardecl(_tmp84D,_tmp84C,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({struct Cyc_Absyn_Decl*_tmp84E=Cyc_Absyn_region_decl(tv,vd,_tmp3D7,(unsigned)((yyyyvsp[0]).l).first_line);_tmp3DB->hd=_tmp84E;}),_tmp3DB->tl=0;_tmp3DB;}));
# 1405
goto _LL0;};};case 48U: _LL5B: _LL5C:
# 1407
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1409 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 49U: _LL5D: _LL5E:
# 1412
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1411 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp84F=Cyc_yyget_YY16(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp84F,Cyc_yyget_YY16(&(yyyyvsp[1]).v));}));
goto _LL0;case 50U: _LL5F: _LL60:
# 1414
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1417 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6D5;({enum Cyc_Parse_Storage_class*_tmp852=Cyc_yyget_YY20(&(yyyyvsp[0]).v);_tmp6D5.sc=_tmp852;}),({struct Cyc_Absyn_Tqual _tmp851=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6D5.tq=_tmp851;}),({
struct Cyc_Parse_Type_specifier _tmp850=Cyc_Parse_empty_spec(0U);_tmp6D5.type_specs=_tmp850;}),_tmp6D5.is_inline=0,_tmp6D5.attributes=0;_tmp6D5;}));
goto _LL0;case 51U: _LL61: _LL62:
# 1421
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1420 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E3=Cyc_yyget_YY17(&(yyyyvsp[1]).v);
if(_tmp3E3.sc != 0)
({void*_tmp3E4=0U;({unsigned _tmp854=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp853=({const char*_tmp3E5="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp3E5,sizeof(char),73U);});Cyc_Warn_warn(_tmp854,_tmp853,_tag_fat(_tmp3E4,sizeof(void*),0U));});});
# 1424
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6D6;({enum Cyc_Parse_Storage_class*_tmp855=Cyc_yyget_YY20(&(yyyyvsp[0]).v);_tmp6D6.sc=_tmp855;}),_tmp6D6.tq=_tmp3E3.tq,_tmp6D6.type_specs=_tmp3E3.type_specs,_tmp6D6.is_inline=_tmp3E3.is_inline,_tmp6D6.attributes=_tmp3E3.attributes;_tmp6D6;}));
# 1428
goto _LL0;};case 52U: _LL63: _LL64:
# 1430
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1429 "parse.y"
({void*_tmp3E6=0U;({unsigned _tmp857=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp856=({const char*_tmp3E7="__extension__ keyword ignored in declaration";_tag_fat(_tmp3E7,sizeof(char),45U);});Cyc_Warn_warn(_tmp857,_tmp856,_tag_fat(_tmp3E6,sizeof(void*),0U));});});
yyval=(yyyyvsp[1]).v;
# 1432
goto _LL0;case 53U: _LL65: _LL66:
# 1434
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1433 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6D7;_tmp6D7.sc=0,({struct Cyc_Absyn_Tqual _tmp859=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6D7.tq=_tmp859;}),({
struct Cyc_Parse_Type_specifier _tmp858=Cyc_yyget_YY21(&(yyyyvsp[0]).v);_tmp6D7.type_specs=_tmp858;}),_tmp6D7.is_inline=0,_tmp6D7.attributes=0;_tmp6D7;}));
goto _LL0;case 54U: _LL67: _LL68:
# 1437
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1436 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E8=Cyc_yyget_YY17(&(yyyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6D8;_tmp6D8.sc=_tmp3E8.sc,_tmp6D8.tq=_tmp3E8.tq,({
struct Cyc_Parse_Type_specifier _tmp85C=({unsigned _tmp85B=(unsigned)((yyyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp85A=_tmp3E8.type_specs;Cyc_Parse_combine_specifiers(_tmp85B,_tmp85A,Cyc_yyget_YY21(&(yyyyvsp[0]).v));});_tmp6D8.type_specs=_tmp85C;}),_tmp6D8.is_inline=_tmp3E8.is_inline,_tmp6D8.attributes=_tmp3E8.attributes;_tmp6D8;}));
# 1442
goto _LL0;};case 55U: _LL69: _LL6A:
# 1444
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1443 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6D9;_tmp6D9.sc=0,({struct Cyc_Absyn_Tqual _tmp85E=Cyc_yyget_YY23(&(yyyyvsp[0]).v);_tmp6D9.tq=_tmp85E;}),({struct Cyc_Parse_Type_specifier _tmp85D=Cyc_Parse_empty_spec(0U);_tmp6D9.type_specs=_tmp85D;}),_tmp6D9.is_inline=0,_tmp6D9.attributes=0;_tmp6D9;}));
goto _LL0;case 56U: _LL6B: _LL6C:
# 1446
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1445 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E9=Cyc_yyget_YY17(&(yyyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6DA;_tmp6DA.sc=_tmp3E9.sc,({struct Cyc_Absyn_Tqual _tmp860=({struct Cyc_Absyn_Tqual _tmp85F=Cyc_yyget_YY23(&(yyyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp85F,_tmp3E9.tq);});_tmp6DA.tq=_tmp860;}),_tmp6DA.type_specs=_tmp3E9.type_specs,_tmp6DA.is_inline=_tmp3E9.is_inline,_tmp6DA.attributes=_tmp3E9.attributes;_tmp6DA;}));
# 1450
goto _LL0;};case 57U: _LL6D: _LL6E:
# 1452
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1451 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6DB;_tmp6DB.sc=0,({struct Cyc_Absyn_Tqual _tmp862=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6DB.tq=_tmp862;}),({
struct Cyc_Parse_Type_specifier _tmp861=Cyc_Parse_empty_spec(0U);_tmp6DB.type_specs=_tmp861;}),_tmp6DB.is_inline=1,_tmp6DB.attributes=0;_tmp6DB;}));
goto _LL0;case 58U: _LL6F: _LL70:
# 1455
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1454 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3EA=Cyc_yyget_YY17(&(yyyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6DC;_tmp6DC.sc=_tmp3EA.sc,_tmp6DC.tq=_tmp3EA.tq,_tmp6DC.type_specs=_tmp3EA.type_specs,_tmp6DC.is_inline=1,_tmp6DC.attributes=_tmp3EA.attributes;_tmp6DC;}));
# 1458
goto _LL0;};case 59U: _LL71: _LL72:
# 1460
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1459 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6DD;_tmp6DD.sc=0,({struct Cyc_Absyn_Tqual _tmp865=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6DD.tq=_tmp865;}),({
struct Cyc_Parse_Type_specifier _tmp864=Cyc_Parse_empty_spec(0U);_tmp6DD.type_specs=_tmp864;}),_tmp6DD.is_inline=0,({struct Cyc_List_List*_tmp863=Cyc_yyget_YY45(&(yyyyvsp[0]).v);_tmp6DD.attributes=_tmp863;});_tmp6DD;}));
goto _LL0;case 60U: _LL73: _LL74:
# 1463
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1462 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3EB=Cyc_yyget_YY17(&(yyyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6DE;_tmp6DE.sc=_tmp3EB.sc,_tmp6DE.tq=_tmp3EB.tq,_tmp6DE.type_specs=_tmp3EB.type_specs,_tmp6DE.is_inline=_tmp3EB.is_inline,({
# 1465
struct Cyc_List_List*_tmp867=({struct Cyc_List_List*_tmp866=Cyc_yyget_YY45(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp866,_tmp3EB.attributes);});_tmp6DE.attributes=_tmp867;});_tmp6DE;}));
goto _LL0;};case 61U: _LL75: _LL76:
# 1468
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;};case 62U: _LL77: _LL78:
# 1473
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1471 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;};case 63U: _LL79: _LL7A:
# 1475
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1473 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;};case 64U: _LL7B: _LL7C:
# 1477
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1475 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;};case 65U: _LL7D: _LL7E:
# 1479
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1478 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _fat_ptr _tmp868=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyyvsp[1]).v);Cyc_strcmp(_tmp868,({const char*_tmp3EC="C";_tag_fat(_tmp3EC,sizeof(char),2U);}));})!= 0)
({void*_tmp3ED=0U;({unsigned _tmp86A=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp869=({const char*_tmp3EE="only extern or extern \"C\" is allowed";_tag_fat(_tmp3EE,sizeof(char),37U);});Cyc_Warn_err(_tmp86A,_tmp869,_tag_fat(_tmp3ED,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1483
goto _LL0;};case 66U: _LL7F: _LL80:
# 1485
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1483 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;};case 67U: _LL81: _LL82:
# 1487
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1486 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;};case 68U: _LL83: _LL84:
# 1490
 yyval=Cyc_YY45(0);
goto _LL0;case 69U: _LL85: _LL86:
# 1493
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1493 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 70U: _LL87: _LL88:
# 1496
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 1498 "parse.y"
yyval=(yyyyvsp[3]).v;
goto _LL0;case 71U: _LL89: _LL8A:
# 1501
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({void*_tmp86B=Cyc_yyget_YY46(&(yyyyvsp[0]).v);_tmp3EF->hd=_tmp86B;}),_tmp3EF->tl=0;_tmp3EF;}));
goto _LL0;case 72U: _LL8B: _LL8C:
# 1505
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1503 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));({void*_tmp86D=Cyc_yyget_YY46(&(yyyyvsp[0]).v);_tmp3F0->hd=_tmp86D;}),({struct Cyc_List_List*_tmp86C=Cyc_yyget_YY45(&(yyyyvsp[2]).v);_tmp3F0->tl=_tmp86C;});_tmp3F0;}));
goto _LL0;case 73U: _LL8D: _LL8E:
# 1506
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1508 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp3F4,_tmp3F4,_tmp3F4 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp3F5,_tmp3F5,_tmp3F5 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp3F6,_tmp3F6,_tmp3F6 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp3F7,_tmp3F7,_tmp3F7 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp3F8,_tmp3F8,_tmp3F8 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp3F9,_tmp3F9,_tmp3F9 + 8U},(void*)& att_aligned},{{_tmp3FA,_tmp3FA,_tmp3FA + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp3FB,_tmp3FB,_tmp3FB + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp3FC,_tmp3FC,_tmp3FC + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp3FD,_tmp3FD,_tmp3FD + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp3FE,_tmp3FE,_tmp3FE + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp3FF,_tmp3FF,_tmp3FF + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp400,_tmp400,_tmp400 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp401,_tmp401,_tmp401 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp402,_tmp402,_tmp402 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp403,_tmp403,_tmp403 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp404,_tmp404,_tmp404 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp405,_tmp405,_tmp405 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1529
struct _fat_ptr _tmp3F1=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);
# 1531
if((((_get_fat_size(_tmp3F1,sizeof(char))> (unsigned)4 &&(int)((const char*)_tmp3F1.curr)[0]== (int)'_')&&(int)((const char*)_tmp3F1.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(_tmp3F1,sizeof(char),(int)(_get_fat_size(_tmp3F1,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(_tmp3F1,sizeof(char),(int)(_get_fat_size(_tmp3F1,sizeof(char))- (unsigned)3)))== (int)'_')
# 1533
_tmp3F1=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)_tmp3F1,2,_get_fat_size(_tmp3F1,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 18U;++ i){
if(Cyc_strcmp((struct _fat_ptr)_tmp3F1,(struct _fat_ptr)(*((struct _tuple28*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple28),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1540
if((unsigned)i == 18U){
({void*_tmp3F2=0U;({unsigned _tmp86F=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp86E=({const char*_tmp3F3="unrecognized attribute";_tag_fat(_tmp3F3,sizeof(char),23U);});Cyc_Warn_err(_tmp86F,_tmp86E,_tag_fat(_tmp3F2,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1545
goto _LL0;};};case 74U: _LL8F: _LL90:
# 1547
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1545 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 75U: _LL91: _LL92:
# 1548
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 1547 "parse.y"
struct _fat_ptr _tmp406=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);
struct Cyc_Absyn_Exp*_tmp407=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);
void*a;
if(({struct _fat_ptr _tmp871=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp871,({const char*_tmp408="aligned";_tag_fat(_tmp408,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp870=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp870,({const char*_tmp409="__aligned__";_tag_fat(_tmp409,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->tag=6U,_tmp40A->f1=_tmp407;_tmp40A;});else{
if(({struct _fat_ptr _tmp873=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp873,({const char*_tmp40B="section";_tag_fat(_tmp40B,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp872=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp872,({const char*_tmp40C="__section__";_tag_fat(_tmp40C,sizeof(char),12U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyyvsp[2]).l).first_line,_tmp407);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=8U,_tmp40D->f1=str;_tmp40D;});}else{
if(({struct _fat_ptr _tmp874=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp874,({const char*_tmp40E="__mode__";_tag_fat(_tmp40E,sizeof(char),9U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyyvsp[2]).l).first_line,_tmp407);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->tag=24U,_tmp40F->f1=str;_tmp40F;});}else{
if(({struct _fat_ptr _tmp875=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp875,({const char*_tmp410="alias";_tag_fat(_tmp410,sizeof(char),6U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyyvsp[2]).l).first_line,_tmp407);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=25U,_tmp411->f1=str;_tmp411;});}else{
# 1563
int n=Cyc_Parse_exp2int((unsigned)((yyyyvsp[2]).l).first_line,_tmp407);
if(({struct _fat_ptr _tmp877=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp877,({const char*_tmp412="regparm";_tag_fat(_tmp412,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp876=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp876,({const char*_tmp413="__regparm__";_tag_fat(_tmp413,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp414=0U;({unsigned _tmp879=(unsigned)((yyyyvsp[2]).l).first_line;struct _fat_ptr _tmp878=({const char*_tmp415="regparm requires value between 0 and 3";_tag_fat(_tmp415,sizeof(char),39U);});Cyc_Warn_err(_tmp879,_tmp878,_tag_fat(_tmp414,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->tag=0U,_tmp416->f1=n;_tmp416;});}else{
if(({struct _fat_ptr _tmp87B=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp87B,({const char*_tmp417="initializes";_tag_fat(_tmp417,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp87A=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp87A,({const char*_tmp418="__initializes__";_tag_fat(_tmp418,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->tag=20U,_tmp419->f1=n;_tmp419;});else{
if(({struct _fat_ptr _tmp87D=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp87D,({const char*_tmp41A="noliveunique";_tag_fat(_tmp41A,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp87C=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp87C,({const char*_tmp41B="__noliveunique__";_tag_fat(_tmp41B,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->tag=21U,_tmp41C->f1=n;_tmp41C;});else{
if(({struct _fat_ptr _tmp87F=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp87F,({const char*_tmp41D="consume";_tag_fat(_tmp41D,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp87E=(struct _fat_ptr)_tmp406;Cyc_zstrcmp(_tmp87E,({const char*_tmp41E="__consume__";_tag_fat(_tmp41E,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->tag=22U,_tmp41F->f1=n;_tmp41F;});else{
# 1575
({void*_tmp420=0U;({unsigned _tmp881=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp880=({const char*_tmp421="unrecognized attribute";_tag_fat(_tmp421,sizeof(char),23U);});Cyc_Warn_err(_tmp881,_tmp880,_tag_fat(_tmp420,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1579
yyval=Cyc_YY46(a);
# 1581
goto _LL0;};case 76U: _LL93: _LL94:
# 1583
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 1582 "parse.y"
struct _fat_ptr _tmp422=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);
struct _fat_ptr _tmp423=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);
unsigned _tmp424=({unsigned _tmp882=(unsigned)((yyyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp882,Cyc_yyget_Int_tok(&(yyyyvsp[4]).v));});
unsigned _tmp425=({unsigned _tmp883=(unsigned)((yyyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp883,Cyc_yyget_Int_tok(&(yyyyvsp[6]).v));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp885=(struct _fat_ptr)_tmp422;Cyc_zstrcmp(_tmp885,({const char*_tmp426="format";_tag_fat(_tmp426,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp884=(struct _fat_ptr)_tmp422;Cyc_zstrcmp(_tmp884,({const char*_tmp427="__format__";_tag_fat(_tmp427,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp887=(struct _fat_ptr)_tmp423;Cyc_zstrcmp(_tmp887,({const char*_tmp428="printf";_tag_fat(_tmp428,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp886=(struct _fat_ptr)_tmp423;Cyc_zstrcmp(_tmp886,({const char*_tmp429="__printf__";_tag_fat(_tmp429,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=19U,_tmp42A->f1=Cyc_Absyn_Printf_ft,_tmp42A->f2=(int)_tmp424,_tmp42A->f3=(int)_tmp425;_tmp42A;});else{
if(({struct _fat_ptr _tmp889=(struct _fat_ptr)_tmp423;Cyc_zstrcmp(_tmp889,({const char*_tmp42B="scanf";_tag_fat(_tmp42B,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp888=(struct _fat_ptr)_tmp423;Cyc_zstrcmp(_tmp888,({const char*_tmp42C="__scanf__";_tag_fat(_tmp42C,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->tag=19U,_tmp42D->f1=Cyc_Absyn_Scanf_ft,_tmp42D->f2=(int)_tmp424,_tmp42D->f3=(int)_tmp425;_tmp42D;});else{
# 1593
({void*_tmp42E=0U;({unsigned _tmp88B=(unsigned)((yyyyvsp[2]).l).first_line;struct _fat_ptr _tmp88A=({const char*_tmp42F="unrecognized format type";_tag_fat(_tmp42F,sizeof(char),25U);});Cyc_Warn_err(_tmp88B,_tmp88A,_tag_fat(_tmp42E,sizeof(void*),0U));});});}}}else{
# 1595
({void*_tmp430=0U;({unsigned _tmp88D=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp88C=({const char*_tmp431="unrecognized attribute";_tag_fat(_tmp431,sizeof(char),23U);});Cyc_Warn_err(_tmp88D,_tmp88C,_tag_fat(_tmp430,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1598
goto _LL0;};case 77U: _LL95: _LL96:
# 1600
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1607 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 78U: _LL97: _LL98:
# 1610
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1609 "parse.y"
yyval=Cyc_YY21(({void*_tmp88F=({struct _tuple0*_tmp88E=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp88E,Cyc_yyget_YY40(&(yyyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp88F,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 79U: _LL99: _LL9A:
# 1612
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 80U: _LL9B: _LL9C:
# 1616
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1614 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 81U: _LL9D: _LL9E:
# 1617
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1615 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 82U: _LL9F: _LLA0:
# 1618
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1616 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 83U: _LLA1: _LLA2:
# 1619
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1617 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 84U: _LLA3: _LLA4:
# 1620
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1618 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 85U: _LLA5: _LLA6:
# 1621
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1619 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1622
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1620 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1623
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1621 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1624
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1622 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 89U: _LLAD: _LLAE:
# 1625
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1623 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 90U: _LLAF: _LLB0:
# 1626
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1626 "parse.y"
yyval=Cyc_YY21(({void*_tmp890=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v));Cyc_Parse_type_spec(_tmp890,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1629
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1628 "parse.y"
yyval=Cyc_YY21(({void*_tmp891=Cyc_Absyn_builtin_type(({const char*_tmp432="__builtin_va_list";_tag_fat(_tmp432,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp891,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1631
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1630 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 93U: _LLB5: _LLB6:
# 1633
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1632 "parse.y"
yyval=Cyc_YY21(({void*_tmp892=Cyc_yyget_YY44(&(yyyyvsp[0]).v);Cyc_Parse_type_spec(_tmp892,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1635
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1634 "parse.y"
yyval=Cyc_YY21(({void*_tmp893=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp893,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 95U: _LLB9: _LLBA:
# 1637
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1636 "parse.y"
yyval=Cyc_YY21(({void*_tmp894=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(&(yyyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp894,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 96U: _LLBB: _LLBC:
# 1639
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1638 "parse.y"
yyval=Cyc_YY21(({void*_tmp897=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=6U,({struct Cyc_List_List*_tmp896=({unsigned _tmp895=(unsigned)((yyyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp895,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyyvsp[2]).v)));});
# 1638
_tmp433->f1=_tmp896;});_tmp433;});Cyc_Parse_type_spec(_tmp897,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1641
goto _LL0;case 97U: _LLBD: _LLBE:
# 1643
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1642 "parse.y"
yyval=Cyc_YY21(({void*_tmp898=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyyvsp[2]).v));Cyc_Parse_type_spec(_tmp898,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1645
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1644 "parse.y"
yyval=Cyc_YY21(({void*_tmp899=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp899,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1646
goto _LL0;case 99U: _LLC1: _LLC2:
# 1648
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1647 "parse.y"
yyval=Cyc_YY21(({void*_tmp89A=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyyvsp[2]).v));Cyc_Parse_type_spec(_tmp89A,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 100U: _LLC3: _LLC4:
# 1650
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1649 "parse.y"
yyval=Cyc_YY21(({void*_tmp89B=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp89B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1652
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1651 "parse.y"
yyval=Cyc_YY21(({void*_tmp89C=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v));Cyc_Parse_type_spec(_tmp89C,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1654
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1657 "parse.y"
yyval=Cyc_YY43(({struct _fat_ptr _tmp89D=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_Parse_id_to_kind(_tmp89D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 103U: _LLC9: _LLCA:
# 1660
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
unsigned loc=(unsigned)(Cyc_Absyn_porting_c_code?((yyyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6DF;_tmp6DF.print_const=1,_tmp6DF.q_volatile=0,_tmp6DF.q_restrict=0,_tmp6DF.real_const=1,_tmp6DF.loc=loc;_tmp6DF;}));
goto _LL0;};case 104U: _LLCB: _LLCC:
# 1665
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1663 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6E0;_tmp6E0.print_const=0,_tmp6E0.q_volatile=1,_tmp6E0.q_restrict=0,_tmp6E0.real_const=0,_tmp6E0.loc=0U;_tmp6E0;}));
goto _LL0;case 105U: _LLCD: _LLCE:
# 1666
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1664 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6E1;_tmp6E1.print_const=0,_tmp6E1.q_volatile=0,_tmp6E1.q_restrict=1,_tmp6E1.real_const=0,_tmp6E1.loc=0U;_tmp6E1;}));
goto _LL0;case 106U: _LLCF: _LLD0:
# 1667
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 1670 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp434=({struct Cyc_Absyn_TypeDecl*_tmp439=_cycalloc(sizeof(*_tmp439));({void*_tmp8A2=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8A1=({struct Cyc_Absyn_Enumdecl*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8A0=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);_tmp437->name=_tmp8A0;}),({struct Cyc_Core_Opt*_tmp89F=({struct Cyc_Core_Opt*_tmp436=_cycalloc(sizeof(*_tmp436));({struct Cyc_List_List*_tmp89E=Cyc_yyget_YY48(&(yyyyvsp[3]).v);_tmp436->v=_tmp89E;});_tmp436;});_tmp437->fields=_tmp89F;});_tmp437;});_tmp438->f1=_tmp8A1;});_tmp438;});_tmp439->r=_tmp8A2;}),_tmp439->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp439;});
# 1672
yyval=Cyc_YY21(({void*_tmp8A3=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->tag=10U,_tmp435->f1=_tmp434,_tmp435->f2=0;_tmp435;});Cyc_Parse_type_spec(_tmp8A3,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1674
goto _LL0;};case 107U: _LLD1: _LLD2:
# 1676
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1675 "parse.y"
yyval=Cyc_YY21(({void*_tmp8A4=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp8A4,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1678
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1677 "parse.y"
yyval=Cyc_YY21(({void*_tmp8A5=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8A5,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6:
# 1680
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1683 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp43A=_cycalloc(sizeof(*_tmp43A));({struct _tuple0*_tmp8A6=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp43A->name=_tmp8A6;}),_tmp43A->tag=0,_tmp43A->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp43A;}));
goto _LL0;case 110U: _LLD7: _LLD8:
# 1686
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1685 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp43B=_cycalloc(sizeof(*_tmp43B));({struct _tuple0*_tmp8A8=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp43B->name=_tmp8A8;}),({struct Cyc_Absyn_Exp*_tmp8A7=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp43B->tag=_tmp8A7;}),_tmp43B->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp43B;}));
goto _LL0;case 111U: _LLD9: _LLDA:
# 1688
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp43C=_cycalloc(sizeof(*_tmp43C));({struct Cyc_Absyn_Enumfield*_tmp8A9=Cyc_yyget_YY47(&(yyyyvsp[0]).v);_tmp43C->hd=_tmp8A9;}),_tmp43C->tl=0;_tmp43C;}));
goto _LL0;case 112U: _LLDB: _LLDC:
# 1692
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1690 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp43D=_cycalloc(sizeof(*_tmp43D));({struct Cyc_Absyn_Enumfield*_tmp8AA=Cyc_yyget_YY47(&(yyyyvsp[0]).v);_tmp43D->hd=_tmp8AA;}),_tmp43D->tl=0;_tmp43D;}));
goto _LL0;case 113U: _LLDD: _LLDE:
# 1693
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1691 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp43E=_cycalloc(sizeof(*_tmp43E));({struct Cyc_Absyn_Enumfield*_tmp8AC=Cyc_yyget_YY47(&(yyyyvsp[0]).v);_tmp43E->hd=_tmp8AC;}),({struct Cyc_List_List*_tmp8AB=Cyc_yyget_YY48(&(yyyyvsp[2]).v);_tmp43E->tl=_tmp8AB;});_tmp43E;}));
goto _LL0;case 114U: _LLDF: _LLE0:
# 1694
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1697 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AF=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=7U,({enum Cyc_Absyn_AggrKind _tmp8AE=Cyc_yyget_YY22(&(yyyyvsp[0]).v);_tmp43F->f1=_tmp8AE;}),({struct Cyc_List_List*_tmp8AD=Cyc_yyget_YY24(&(yyyyvsp[2]).v);_tmp43F->f2=_tmp8AD;});_tmp43F;});Cyc_Parse_type_spec(_tmp8AF,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1700
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,8);{
# 1703 "parse.y"
struct Cyc_List_List*_tmp440=({unsigned _tmp8B0=(unsigned)((yyyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8B0,Cyc_yyget_YY40(&(yyyyvsp[3]).v));});
struct Cyc_List_List*_tmp441=({unsigned _tmp8B1=(unsigned)((yyyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8B1,Cyc_yyget_YY40(&(yyyyvsp[5]).v));});
struct Cyc_Absyn_TypeDecl*_tmp442=({enum Cyc_Absyn_AggrKind _tmp8B7=Cyc_yyget_YY22(&(yyyyvsp[1]).v);struct _tuple0*_tmp8B6=Cyc_yyget_QualId_tok(&(yyyyvsp[2]).v);struct Cyc_List_List*_tmp8B5=_tmp440;struct Cyc_Absyn_AggrdeclImpl*_tmp8B4=({
struct Cyc_List_List*_tmp8B3=_tmp441;struct Cyc_List_List*_tmp8B2=Cyc_yyget_YY50(&(yyyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp8B3,_tmp8B2,Cyc_yyget_YY24(&(yyyyvsp[7]).v),1);});
# 1705
Cyc_Absyn_aggr_tdecl(_tmp8B7,Cyc_Absyn_Public,_tmp8B6,_tmp8B5,_tmp8B4,0,(unsigned)((yyyyvsp[0]).l).first_line);});
# 1708
yyval=Cyc_YY21(({void*_tmp8B8=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->tag=10U,_tmp443->f1=_tmp442,_tmp443->f2=0;_tmp443;});Cyc_Parse_type_spec(_tmp8B8,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1710
goto _LL0;};case 116U: _LLE3: _LLE4:
# 1712
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 1714
struct Cyc_List_List*_tmp444=({unsigned _tmp8B9=(unsigned)((yyyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8B9,Cyc_yyget_YY40(&(yyyyvsp[2]).v));});
struct Cyc_List_List*_tmp445=({unsigned _tmp8BA=(unsigned)((yyyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8BA,Cyc_yyget_YY40(&(yyyyvsp[4]).v));});
struct Cyc_Absyn_TypeDecl*_tmp446=({enum Cyc_Absyn_AggrKind _tmp8C0=Cyc_yyget_YY22(&(yyyyvsp[0]).v);struct _tuple0*_tmp8BF=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);struct Cyc_List_List*_tmp8BE=_tmp444;struct Cyc_Absyn_AggrdeclImpl*_tmp8BD=({
struct Cyc_List_List*_tmp8BC=_tmp445;struct Cyc_List_List*_tmp8BB=Cyc_yyget_YY50(&(yyyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp8BC,_tmp8BB,Cyc_yyget_YY24(&(yyyyvsp[6]).v),0);});
# 1716
Cyc_Absyn_aggr_tdecl(_tmp8C0,Cyc_Absyn_Public,_tmp8BF,_tmp8BE,_tmp8BD,0,(unsigned)((yyyyvsp[0]).l).first_line);});
# 1719
yyval=Cyc_YY21(({void*_tmp8C1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=10U,_tmp447->f1=_tmp446,_tmp447->f2=0;_tmp447;});Cyc_Parse_type_spec(_tmp8C1,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1721
goto _LL0;};case 117U: _LLE5: _LLE6:
# 1723
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1722 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C5=({union Cyc_Absyn_AggrInfo _tmp8C4=({enum Cyc_Absyn_AggrKind _tmp8C3=Cyc_yyget_YY22(&(yyyyvsp[1]).v);struct _tuple0*_tmp8C2=Cyc_yyget_QualId_tok(&(yyyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp8C3,_tmp8C2,({struct Cyc_Core_Opt*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->v=(void*)1;_tmp448;}));});Cyc_Absyn_aggr_type(_tmp8C4,Cyc_yyget_YY40(&(yyyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp8C5,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1725
goto _LL0;case 118U: _LLE7: _LLE8:
# 1727
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1726 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C8=({union Cyc_Absyn_AggrInfo _tmp8C7=({enum Cyc_Absyn_AggrKind _tmp8C6=Cyc_yyget_YY22(&(yyyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp8C6,Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp8C7,Cyc_yyget_YY40(&(yyyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp8C8,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1729
 yyval=Cyc_YY40(0);
goto _LL0;case 120U: _LLEB: _LLEC:
# 1732
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1733 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyyvsp[1]).v)));
goto _LL0;case 121U: _LLED: _LLEE:
# 1736
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 122U: _LLEF: _LLF0:
# 1740
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1738 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 123U: _LLF1: _LLF2:
# 1741
 yyval=Cyc_YY24(0);
goto _LL0;case 124U: _LLF3: _LLF4:
# 1744
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 1745 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp449=Cyc_yyget_YY25(&(yyyyvsp[0]).v);for(0;_tmp449 != 0;_tmp449=_tmp449->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp449->hd,decls);}}{
# 1749
struct Cyc_List_List*_tmp44A=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp44A != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp44A,decls);
yyval=Cyc_YY24(decls);
# 1754
goto _LL0;};};case 125U: _LLF5: _LLF6:
# 1756
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1759 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp44B=_cycalloc(sizeof(*_tmp44B));({struct Cyc_List_List*_tmp8C9=Cyc_yyget_YY24(&(yyyyvsp[0]).v);_tmp44B->hd=_tmp8C9;}),_tmp44B->tl=0;_tmp44B;}));
goto _LL0;case 126U: _LLF7: _LLF8:
# 1762
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1761 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp44C=_cycalloc(sizeof(*_tmp44C));({struct Cyc_List_List*_tmp8CB=Cyc_yyget_YY24(&(yyyyvsp[1]).v);_tmp44C->hd=_tmp8CB;}),({struct Cyc_List_List*_tmp8CA=Cyc_yyget_YY25(&(yyyyvsp[0]).v);_tmp44C->tl=_tmp8CA;});_tmp44C;}));
goto _LL0;case 127U: _LLF9: _LLFA:
# 1764
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyyvsp[0]).v)));
goto _LL0;case 128U: _LLFB: _LLFC:
# 1768
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1771 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp44D=_region_malloc(yyr,sizeof(*_tmp44D));_tmp44D->tl=0,({struct _tuple11 _tmp8CC=Cyc_yyget_YY18(&(yyyyvsp[0]).v);_tmp44D->hd=_tmp8CC;});_tmp44D;}));
goto _LL0;case 129U: _LLFD: _LLFE:
# 1774
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1773 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp44E=_region_malloc(yyr,sizeof(*_tmp44E));({struct _tuple12*_tmp8CE=Cyc_yyget_YY19(&(yyyyvsp[0]).v);_tmp44E->tl=_tmp8CE;}),({struct _tuple11 _tmp8CD=Cyc_yyget_YY18(&(yyyyvsp[2]).v);_tmp44E->hd=_tmp8CD;});_tmp44E;}));
goto _LL0;case 130U: _LLFF: _LL100:
# 1776
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1778 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp6E2;({struct Cyc_Parse_Declarator _tmp8CF=Cyc_yyget_YY27(&(yyyyvsp[0]).v);_tmp6E2.f1=_tmp8CF;}),_tmp6E2.f2=0;_tmp6E2;}));
goto _LL0;case 131U: _LL101: _LL102:
# 1781
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1780 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp6E3;({struct Cyc_Parse_Declarator _tmp8D1=Cyc_yyget_YY27(&(yyyyvsp[0]).v);_tmp6E3.f1=_tmp8D1;}),({struct Cyc_Absyn_Exp*_tmp8D0=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp6E3.f2=_tmp8D0;});_tmp6E3;}));
goto _LL0;case 132U: _LL103: _LL104:
# 1783
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 1786 "parse.y"
struct _RegionHandle _tmp44F=_new_region("temp");struct _RegionHandle*temp=& _tmp44F;_push_region(temp);
{struct _tuple25 _tmp450=Cyc_yyget_YY35(&(yyyyvsp[0]).v);struct _tuple25 _tmp451=_tmp450;struct Cyc_Absyn_Tqual _tmp45F;struct Cyc_Parse_Type_specifier _tmp45E;struct Cyc_List_List*_tmp45D;_LL42E: _tmp45F=_tmp451.f1;_tmp45E=_tmp451.f2;_tmp45D=_tmp451.f3;_LL42F:;
if(_tmp45F.loc == (unsigned)0)_tmp45F.loc=(unsigned)((yyyyvsp[0]).l).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp452=Cyc_yyget_YY29(&(yyyyvsp[1]).v);for(0;_tmp452 != 0;_tmp452=_tmp452->tl){
struct _tuple24*_tmp453=(struct _tuple24*)_tmp452->hd;struct _tuple24*_tmp454=_tmp453;struct Cyc_Parse_Declarator _tmp45A;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp458;_LL431: _tmp45A=_tmp454->f1;_tmp459=_tmp454->f2;_tmp458=_tmp454->f3;_LL432:;
decls=({struct _tuple10*_tmp455=_region_malloc(temp,sizeof(*_tmp455));_tmp455->tl=decls,_tmp455->hd=_tmp45A;_tmp455;});
widths_and_reqs=({struct Cyc_List_List*_tmp457=_region_malloc(temp,sizeof(*_tmp457));
({struct _tuple16*_tmp8D2=({struct _tuple16*_tmp456=_region_malloc(temp,sizeof(*_tmp456));_tmp456->f1=_tmp459,_tmp456->f2=_tmp458;_tmp456;});_tmp457->hd=_tmp8D2;}),_tmp457->tl=widths_and_reqs;_tmp457;});}}
# 1797
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp45B=Cyc_Parse_speclist2typ(_tmp45E,(unsigned)((yyyyvsp[0]).l).first_line);
struct Cyc_List_List*_tmp45C=({struct _RegionHandle*_tmp8D5=temp;struct _RegionHandle*_tmp8D4=temp;struct Cyc_List_List*_tmp8D3=
Cyc_Parse_apply_tmss(temp,_tmp45F,_tmp45B,decls,_tmp45D);
# 1800
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp8D5,_tmp8D4,_tmp8D3,widths_and_reqs);});
# 1803
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple17*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyyvsp[0]).l).first_line,_tmp45C));
# 1805
_npop_handler(0U);goto _LL0;};};}
# 1787
;_pop_region(temp);};case 133U: _LL105: _LL106:
# 1807
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1813 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6E4;({struct Cyc_Absyn_Tqual _tmp8D7=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6E4.f1=_tmp8D7;}),({struct Cyc_Parse_Type_specifier _tmp8D6=Cyc_yyget_YY21(&(yyyyvsp[0]).v);_tmp6E4.f2=_tmp8D6;}),_tmp6E4.f3=0;_tmp6E4;}));
goto _LL0;case 134U: _LL107: _LL108:
# 1816
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1815 "parse.y"
struct _tuple25 _tmp460=Cyc_yyget_YY35(&(yyyyvsp[1]).v);yyval=Cyc_YY35(({struct _tuple25 _tmp6E5;_tmp6E5.f1=_tmp460.f1,({struct Cyc_Parse_Type_specifier _tmp8DA=({unsigned _tmp8D9=(unsigned)((yyyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8D8=Cyc_yyget_YY21(&(yyyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp8D9,_tmp8D8,_tmp460.f2);});_tmp6E5.f2=_tmp8DA;}),_tmp6E5.f3=_tmp460.f3;_tmp6E5;}));
goto _LL0;};case 135U: _LL109: _LL10A:
# 1818
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1817 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6E6;({struct Cyc_Absyn_Tqual _tmp8DC=Cyc_yyget_YY23(&(yyyyvsp[0]).v);_tmp6E6.f1=_tmp8DC;}),({struct Cyc_Parse_Type_specifier _tmp8DB=Cyc_Parse_empty_spec(0U);_tmp6E6.f2=_tmp8DB;}),_tmp6E6.f3=0;_tmp6E6;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1820
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1819 "parse.y"
struct _tuple25 _tmp461=Cyc_yyget_YY35(&(yyyyvsp[1]).v);
yyval=Cyc_YY35(({struct _tuple25 _tmp6E7;({struct Cyc_Absyn_Tqual _tmp8DE=({struct Cyc_Absyn_Tqual _tmp8DD=Cyc_yyget_YY23(&(yyyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8DD,_tmp461.f1);});_tmp6E7.f1=_tmp8DE;}),_tmp6E7.f2=_tmp461.f2,_tmp6E7.f3=_tmp461.f3;_tmp6E7;}));
goto _LL0;};case 137U: _LL10D: _LL10E:
# 1823
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1822 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6E8;({struct Cyc_Absyn_Tqual _tmp8E1=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6E8.f1=_tmp8E1;}),({struct Cyc_Parse_Type_specifier _tmp8E0=Cyc_Parse_empty_spec(0U);_tmp6E8.f2=_tmp8E0;}),({struct Cyc_List_List*_tmp8DF=Cyc_yyget_YY45(&(yyyyvsp[0]).v);_tmp6E8.f3=_tmp8DF;});_tmp6E8;}));
goto _LL0;case 138U: _LL10F: _LL110:
# 1825
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1824 "parse.y"
struct _tuple25 _tmp462=Cyc_yyget_YY35(&(yyyyvsp[1]).v);yyval=Cyc_YY35(({struct _tuple25 _tmp6E9;_tmp6E9.f1=_tmp462.f1,_tmp6E9.f2=_tmp462.f2,({struct Cyc_List_List*_tmp8E3=({struct Cyc_List_List*_tmp8E2=Cyc_yyget_YY45(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E2,_tmp462.f3);});_tmp6E9.f3=_tmp8E3;});_tmp6E9;}));
goto _LL0;};case 139U: _LL111: _LL112:
# 1827
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1830 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6EA;({struct Cyc_Absyn_Tqual _tmp8E5=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6EA.f1=_tmp8E5;}),({struct Cyc_Parse_Type_specifier _tmp8E4=Cyc_yyget_YY21(&(yyyyvsp[0]).v);_tmp6EA.f2=_tmp8E4;}),_tmp6EA.f3=0;_tmp6EA;}));
goto _LL0;case 140U: _LL113: _LL114:
# 1833
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1832 "parse.y"
struct _tuple25 _tmp463=Cyc_yyget_YY35(&(yyyyvsp[1]).v);yyval=Cyc_YY35(({struct _tuple25 _tmp6EB;_tmp6EB.f1=_tmp463.f1,({struct Cyc_Parse_Type_specifier _tmp8E8=({unsigned _tmp8E7=(unsigned)((yyyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8E6=Cyc_yyget_YY21(&(yyyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp8E7,_tmp8E6,_tmp463.f2);});_tmp6EB.f2=_tmp8E8;}),_tmp6EB.f3=_tmp463.f3;_tmp6EB;}));
goto _LL0;};case 141U: _LL115: _LL116:
# 1835
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1834 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6EC;({struct Cyc_Absyn_Tqual _tmp8EA=Cyc_yyget_YY23(&(yyyyvsp[0]).v);_tmp6EC.f1=_tmp8EA;}),({struct Cyc_Parse_Type_specifier _tmp8E9=Cyc_Parse_empty_spec(0U);_tmp6EC.f2=_tmp8E9;}),_tmp6EC.f3=0;_tmp6EC;}));
goto _LL0;case 142U: _LL117: _LL118:
# 1837
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1836 "parse.y"
struct _tuple25 _tmp464=Cyc_yyget_YY35(&(yyyyvsp[1]).v);
yyval=Cyc_YY35(({struct _tuple25 _tmp6ED;({struct Cyc_Absyn_Tqual _tmp8EC=({struct Cyc_Absyn_Tqual _tmp8EB=Cyc_yyget_YY23(&(yyyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8EB,_tmp464.f1);});_tmp6ED.f1=_tmp8EC;}),_tmp6ED.f2=_tmp464.f2,_tmp6ED.f3=_tmp464.f3;_tmp6ED;}));
goto _LL0;};case 143U: _LL119: _LL11A:
# 1840
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1839 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6EE;({struct Cyc_Absyn_Tqual _tmp8EF=Cyc_Absyn_empty_tqual((unsigned)((yyyyvsp[0]).l).first_line);_tmp6EE.f1=_tmp8EF;}),({struct Cyc_Parse_Type_specifier _tmp8EE=Cyc_Parse_empty_spec(0U);_tmp6EE.f2=_tmp8EE;}),({struct Cyc_List_List*_tmp8ED=Cyc_yyget_YY45(&(yyyyvsp[0]).v);_tmp6EE.f3=_tmp8ED;});_tmp6EE;}));
goto _LL0;case 144U: _LL11B: _LL11C:
# 1842
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1841 "parse.y"
struct _tuple25 _tmp465=Cyc_yyget_YY35(&(yyyyvsp[1]).v);yyval=Cyc_YY35(({struct _tuple25 _tmp6EF;_tmp6EF.f1=_tmp465.f1,_tmp6EF.f2=_tmp465.f2,({struct Cyc_List_List*_tmp8F1=({struct Cyc_List_List*_tmp8F0=Cyc_yyget_YY45(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8F0,_tmp465.f3);});_tmp6EF.f3=_tmp8F1;});_tmp6EF;}));
goto _LL0;};case 145U: _LL11D: _LL11E:
# 1844
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyyvsp[0]).v)));
goto _LL0;case 146U: _LL11F: _LL120:
# 1848
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1851 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp466=_region_malloc(yyr,sizeof(*_tmp466));({struct _tuple24*_tmp8F2=Cyc_yyget_YY28(&(yyyyvsp[0]).v);_tmp466->hd=_tmp8F2;}),_tmp466->tl=0;_tmp466;}));
goto _LL0;case 147U: _LL121: _LL122:
# 1854
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1853 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp467=_region_malloc(yyr,sizeof(*_tmp467));({struct _tuple24*_tmp8F4=Cyc_yyget_YY28(&(yyyyvsp[2]).v);_tmp467->hd=_tmp8F4;}),({struct Cyc_List_List*_tmp8F3=Cyc_yyget_YY29(&(yyyyvsp[0]).v);_tmp467->tl=_tmp8F3;});_tmp467;}));
goto _LL0;case 148U: _LL123: _LL124:
# 1856
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1858 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp468=_region_malloc(yyr,sizeof(*_tmp468));({struct Cyc_Parse_Declarator _tmp8F6=Cyc_yyget_YY27(&(yyyyvsp[0]).v);_tmp468->f1=_tmp8F6;}),_tmp468->f2=0,({struct Cyc_Absyn_Exp*_tmp8F5=Cyc_yyget_YY55(&(yyyyvsp[1]).v);_tmp468->f3=_tmp8F5;});_tmp468;}));
goto _LL0;case 149U: _LL125: _LL126:
# 1861
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1862 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp46C=_region_malloc(yyr,sizeof(*_tmp46C));({struct _tuple0*_tmp8FB=({struct _tuple0*_tmp46B=_cycalloc(sizeof(*_tmp46B));({union Cyc_Absyn_Nmspace _tmp8FA=Cyc_Absyn_Rel_n(0);_tmp46B->f1=_tmp8FA;}),({struct _fat_ptr*_tmp8F9=({struct _fat_ptr*_tmp46A=_cycalloc(sizeof(*_tmp46A));({struct _fat_ptr _tmp8F8=({const char*_tmp469="";_tag_fat(_tmp469,sizeof(char),1U);});*_tmp46A=_tmp8F8;});_tmp46A;});_tmp46B->f2=_tmp8F9;});_tmp46B;});(_tmp46C->f1).id=_tmp8FB;}),(_tmp46C->f1).varloc=0U,(_tmp46C->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);_tmp46C->f2=_tmp8F7;}),_tmp46C->f3=0;_tmp46C;}));
# 1864
goto _LL0;case 150U: _LL127: _LL128:
# 1868
 yyval=Cyc_YY28(({struct _tuple24*_tmp470=_region_malloc(yyr,sizeof(*_tmp470));({struct _tuple0*_tmp8FF=({struct _tuple0*_tmp46F=_cycalloc(sizeof(*_tmp46F));({union Cyc_Absyn_Nmspace _tmp8FE=Cyc_Absyn_Rel_n(0);_tmp46F->f1=_tmp8FE;}),({struct _fat_ptr*_tmp8FD=({struct _fat_ptr*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct _fat_ptr _tmp8FC=({const char*_tmp46D="";_tag_fat(_tmp46D,sizeof(char),1U);});*_tmp46E=_tmp8FC;});_tmp46E;});_tmp46F->f2=_tmp8FD;});_tmp46F;});(_tmp470->f1).id=_tmp8FF;}),(_tmp470->f1).varloc=0U,(_tmp470->f1).tms=0,_tmp470->f2=0,_tmp470->f3=0;_tmp470;}));
# 1870
goto _LL0;case 151U: _LL129: _LL12A:
# 1872
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1870 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp471=_region_malloc(yyr,sizeof(*_tmp471));({struct Cyc_Parse_Declarator _tmp901=Cyc_yyget_YY27(&(yyyyvsp[0]).v);_tmp471->f1=_tmp901;}),({struct Cyc_Absyn_Exp*_tmp900=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp471->f2=_tmp900;}),_tmp471->f3=0;_tmp471;}));
goto _LL0;case 152U: _LL12B: _LL12C:
# 1873
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1877
 yyval=Cyc_YY55(0);
goto _LL0;case 154U: _LL12F: _LL130:
# 1880
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1879 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v));
goto _LL0;case 155U: _LL131: _LL132:
# 1882
 yyval=Cyc_YY55(0);
goto _LL0;case 156U: _LL133: _LL134:
# 1885
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
# 1886 "parse.y"
int _tmp472=Cyc_yyget_YY31(&(yyyyvsp[0]).v);
struct Cyc_List_List*_tmp473=({unsigned _tmp902=(unsigned)((yyyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp902,Cyc_yyget_YY40(&(yyyyvsp[2]).v));});
struct Cyc_Absyn_TypeDecl*_tmp474=({struct _tuple0*_tmp907=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);struct Cyc_List_List*_tmp906=_tmp473;struct Cyc_Core_Opt*_tmp905=({struct Cyc_Core_Opt*_tmp476=_cycalloc(sizeof(*_tmp476));({struct Cyc_List_List*_tmp903=Cyc_yyget_YY34(&(yyyyvsp[4]).v);_tmp476->v=_tmp903;});_tmp476;});int _tmp904=_tmp472;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp907,_tmp906,_tmp905,_tmp904,(unsigned)((yyyyvsp[0]).l).first_line);});
# 1890
yyval=Cyc_YY21(({void*_tmp908=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=10U,_tmp475->f1=_tmp474,_tmp475->f2=0;_tmp475;});Cyc_Parse_type_spec(_tmp908,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1892
goto _LL0;};case 157U: _LL135: _LL136:
# 1894
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 1893 "parse.y"
int _tmp477=Cyc_yyget_YY31(&(yyyyvsp[0]).v);
yyval=Cyc_YY21(({void*_tmp90B=({union Cyc_Absyn_DatatypeInfo _tmp90A=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp6F0;({struct _tuple0*_tmp909=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);_tmp6F0.name=_tmp909;}),_tmp6F0.is_extensible=_tmp477;_tmp6F0;}));Cyc_Absyn_datatype_type(_tmp90A,Cyc_yyget_YY40(&(yyyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp90B,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1896
goto _LL0;};case 158U: _LL137: _LL138:
# 1898
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 1897 "parse.y"
int _tmp478=Cyc_yyget_YY31(&(yyyyvsp[0]).v);
yyval=Cyc_YY21(({void*_tmp90F=({union Cyc_Absyn_DatatypeFieldInfo _tmp90E=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp6F1;({struct _tuple0*_tmp90D=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);_tmp6F1.datatype_name=_tmp90D;}),({struct _tuple0*_tmp90C=Cyc_yyget_QualId_tok(&(yyyyvsp[3]).v);_tmp6F1.field_name=_tmp90C;}),_tmp6F1.is_extensible=_tmp478;_tmp6F1;}));Cyc_Absyn_datatype_field_type(_tmp90E,Cyc_yyget_YY40(&(yyyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp90F,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 1900
goto _LL0;};case 159U: _LL139: _LL13A:
# 1902
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY31(0);
goto _LL0;case 160U: _LL13B: _LL13C:
# 1906
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1904 "parse.y"
yyval=Cyc_YY31(1);
goto _LL0;case 161U: _LL13D: _LL13E:
# 1907
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp479=_cycalloc(sizeof(*_tmp479));({struct Cyc_Absyn_Datatypefield*_tmp910=Cyc_yyget_YY33(&(yyyyvsp[0]).v);_tmp479->hd=_tmp910;}),_tmp479->tl=0;_tmp479;}));
goto _LL0;case 162U: _LL13F: _LL140:
# 1911
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1909 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct Cyc_Absyn_Datatypefield*_tmp911=Cyc_yyget_YY33(&(yyyyvsp[0]).v);_tmp47A->hd=_tmp911;}),_tmp47A->tl=0;_tmp47A;}));
goto _LL0;case 163U: _LL141: _LL142:
# 1912
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1910 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp47B=_cycalloc(sizeof(*_tmp47B));({struct Cyc_Absyn_Datatypefield*_tmp913=Cyc_yyget_YY33(&(yyyyvsp[0]).v);_tmp47B->hd=_tmp913;}),({struct Cyc_List_List*_tmp912=Cyc_yyget_YY34(&(yyyyvsp[2]).v);_tmp47B->tl=_tmp912;});_tmp47B;}));
goto _LL0;case 164U: _LL143: _LL144:
# 1913
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1911 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct Cyc_Absyn_Datatypefield*_tmp915=Cyc_yyget_YY33(&(yyyyvsp[0]).v);_tmp47C->hd=_tmp915;}),({struct Cyc_List_List*_tmp914=Cyc_yyget_YY34(&(yyyyvsp[2]).v);_tmp47C->tl=_tmp914;});_tmp47C;}));
goto _LL0;case 165U: _LL145: _LL146:
# 1914
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 166U: _LL147: _LL148:
# 1917
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1916 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 167U: _LL149: _LL14A:
# 1919
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1917 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 168U: _LL14B: _LL14C:
# 1920
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct _tuple0*_tmp917=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);_tmp47D->name=_tmp917;}),_tmp47D->typs=0,_tmp47D->loc=(unsigned)((yyyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp916=Cyc_yyget_YY32(&(yyyyvsp[0]).v);_tmp47D->sc=_tmp916;});_tmp47D;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1924
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 1923 "parse.y"
struct Cyc_List_List*_tmp47E=({unsigned _tmp918=(unsigned)((yyyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp918,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyyvsp[3]).v)));});
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct _tuple0*_tmp91A=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);_tmp47F->name=_tmp91A;}),_tmp47F->typs=_tmp47E,_tmp47F->loc=(unsigned)((yyyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp919=Cyc_yyget_YY32(&(yyyyvsp[0]).v);_tmp47F->sc=_tmp919;});_tmp47F;}));
goto _LL0;};case 170U: _LL14F: _LL150:
# 1927
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1929 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 171U: _LL151: _LL152:
# 1932
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1931 "parse.y"
struct Cyc_Parse_Declarator _tmp480=Cyc_yyget_YY27(&(yyyyvsp[1]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F2;_tmp6F2.id=_tmp480.id,_tmp6F2.varloc=_tmp480.varloc,({struct Cyc_List_List*_tmp91C=({struct Cyc_List_List*_tmp91B=Cyc_yyget_YY26(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp91B,_tmp480.tms);});_tmp6F2.tms=_tmp91C;});_tmp6F2;}));
goto _LL0;};case 172U: _LL153: _LL154:
# 1935
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1938 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 173U: _LL155: _LL156:
# 1941
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 1940 "parse.y"
struct Cyc_Parse_Declarator _tmp481=Cyc_yyget_YY27(&(yyyyvsp[1]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F3;_tmp6F3.id=_tmp481.id,_tmp6F3.varloc=_tmp481.varloc,({struct Cyc_List_List*_tmp91E=({struct Cyc_List_List*_tmp91D=Cyc_yyget_YY26(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp91D,_tmp481.tms);});_tmp6F3.tms=_tmp91E;});_tmp6F3;}));
goto _LL0;};case 174U: _LL157: _LL158:
# 1944
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1946 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F4;({struct _tuple0*_tmp91F=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp6F4.id=_tmp91F;}),_tmp6F4.varloc=(unsigned)((yyyyvsp[0]).l).first_line,_tmp6F4.tms=0;_tmp6F4;}));
goto _LL0;case 175U: _LL159: _LL15A:
# 1949
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1948 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 176U: _LL15B: _LL15C:
# 1951
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
struct Cyc_Parse_Declarator _tmp482=Cyc_yyget_YY27(&(yyyyvsp[2]).v);
({struct Cyc_List_List*_tmp922=({struct Cyc_List_List*_tmp484=_region_malloc(yyr,sizeof(*_tmp484));({void*_tmp921=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp483=_region_malloc(yyr,sizeof(*_tmp483));_tmp483->tag=5U,_tmp483->f1=(unsigned)((yyyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp920=Cyc_yyget_YY45(&(yyyyvsp[1]).v);_tmp483->f2=_tmp920;});_tmp483;});_tmp484->hd=_tmp921;}),_tmp484->tl=_tmp482.tms;_tmp484;});_tmp482.tms=_tmp922;});
yyval=(yyyyvsp[2]).v;
# 1956
goto _LL0;};case 177U: _LL15D: _LL15E:
# 1958
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1957 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F5;({struct _tuple0*_tmp928=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6F5.id=_tmp928;}),({unsigned _tmp927=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6F5.varloc=_tmp927;}),({struct Cyc_List_List*_tmp926=({struct Cyc_List_List*_tmp486=_region_malloc(yyr,sizeof(*_tmp486));({void*_tmp925=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp485=_region_malloc(yyr,sizeof(*_tmp485));_tmp485->tag=0U,({void*_tmp924=Cyc_yyget_YY51(&(yyyyvsp[3]).v);_tmp485->f1=_tmp924;}),_tmp485->f2=(unsigned)((yyyyvsp[3]).l).first_line;_tmp485;});_tmp486->hd=_tmp925;}),({struct Cyc_List_List*_tmp923=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp486->tl=_tmp923;});_tmp486;});_tmp6F5.tms=_tmp926;});_tmp6F5;}));
goto _LL0;case 178U: _LL15F: _LL160:
# 1960
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 1959 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F6;({struct _tuple0*_tmp92F=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6F6.id=_tmp92F;}),({unsigned _tmp92E=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6F6.varloc=_tmp92E;}),({
struct Cyc_List_List*_tmp92D=({struct Cyc_List_List*_tmp488=_region_malloc(yyr,sizeof(*_tmp488));({void*_tmp92C=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp487=_region_malloc(yyr,sizeof(*_tmp487));_tmp487->tag=1U,({struct Cyc_Absyn_Exp*_tmp92B=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp487->f1=_tmp92B;}),({void*_tmp92A=Cyc_yyget_YY51(&(yyyyvsp[4]).v);_tmp487->f2=_tmp92A;}),_tmp487->f3=(unsigned)((yyyyvsp[4]).l).first_line;_tmp487;});_tmp488->hd=_tmp92C;}),({struct Cyc_List_List*_tmp929=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp488->tl=_tmp929;});_tmp488;});_tmp6F6.tms=_tmp92D;});_tmp6F6;}));
goto _LL0;case 179U: _LL161: _LL162:
# 1963
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
# 1962 "parse.y"
struct _tuple26*_tmp489=Cyc_yyget_YY39(&(yyyyvsp[2]).v);struct _tuple26*_tmp48A=_tmp489;struct Cyc_List_List*_tmp494;int _tmp493;struct Cyc_Absyn_VarargInfo*_tmp492;void*_tmp491;struct Cyc_List_List*_tmp490;_LL434: _tmp494=_tmp48A->f1;_tmp493=_tmp48A->f2;_tmp492=_tmp48A->f3;_tmp491=_tmp48A->f4;_tmp490=_tmp48A->f5;_LL435:;{
struct Cyc_Absyn_Exp*_tmp48B=Cyc_yyget_YY55(&(yyyyvsp[4]).v);
struct Cyc_Absyn_Exp*_tmp48C=Cyc_yyget_YY55(&(yyyyvsp[5]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F7;({struct _tuple0*_tmp935=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6F7.id=_tmp935;}),({unsigned _tmp934=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6F7.varloc=_tmp934;}),({struct Cyc_List_List*_tmp933=({struct Cyc_List_List*_tmp48F=_region_malloc(yyr,sizeof(*_tmp48F));({void*_tmp932=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp48E=_region_malloc(yyr,sizeof(*_tmp48E));_tmp48E->tag=3U,({void*_tmp931=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp48D=_region_malloc(yyr,sizeof(*_tmp48D));_tmp48D->tag=1U,_tmp48D->f1=_tmp494,_tmp48D->f2=_tmp493,_tmp48D->f3=_tmp492,_tmp48D->f4=_tmp491,_tmp48D->f5=_tmp490,_tmp48D->f6=_tmp48B,_tmp48D->f7=_tmp48C;_tmp48D;});_tmp48E->f1=_tmp931;});_tmp48E;});_tmp48F->hd=_tmp932;}),({struct Cyc_List_List*_tmp930=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp48F->tl=_tmp930;});_tmp48F;});_tmp6F7.tms=_tmp933;});_tmp6F7;}));
# 1967
goto _LL0;};};case 180U: _LL163: _LL164:
# 1969
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 1968 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F8;({struct _tuple0*_tmp93F=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6F8.id=_tmp93F;}),({unsigned _tmp93E=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6F8.varloc=_tmp93E;}),({
struct Cyc_List_List*_tmp93D=({struct Cyc_List_List*_tmp497=_region_malloc(yyr,sizeof(*_tmp497));({void*_tmp93C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp496=_region_malloc(yyr,sizeof(*_tmp496));_tmp496->tag=3U,({void*_tmp93B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp495=_region_malloc(yyr,sizeof(*_tmp495));_tmp495->tag=1U,_tmp495->f1=0,_tmp495->f2=0,_tmp495->f3=0,({
# 1971
void*_tmp93A=Cyc_yyget_YY49(&(yyyyvsp[2]).v);_tmp495->f4=_tmp93A;}),({struct Cyc_List_List*_tmp939=Cyc_yyget_YY50(&(yyyyvsp[3]).v);_tmp495->f5=_tmp939;}),({struct Cyc_Absyn_Exp*_tmp938=Cyc_yyget_YY55(&(yyyyvsp[5]).v);_tmp495->f6=_tmp938;}),({struct Cyc_Absyn_Exp*_tmp937=Cyc_yyget_YY55(&(yyyyvsp[6]).v);_tmp495->f7=_tmp937;});_tmp495;});
# 1969
_tmp496->f1=_tmp93B;});_tmp496;});_tmp497->hd=_tmp93C;}),({
# 1972
struct Cyc_List_List*_tmp936=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp497->tl=_tmp936;});_tmp497;});
# 1969
_tmp6F8.tms=_tmp93D;});_tmp6F8;}));
# 1974
goto _LL0;case 181U: _LL165: _LL166:
# 1976
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 1975 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6F9;({struct _tuple0*_tmp946=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6F9.id=_tmp946;}),({unsigned _tmp945=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6F9.varloc=_tmp945;}),({struct Cyc_List_List*_tmp944=({struct Cyc_List_List*_tmp49A=_region_malloc(yyr,sizeof(*_tmp49A));({void*_tmp943=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp499=_region_malloc(yyr,sizeof(*_tmp499));_tmp499->tag=3U,({void*_tmp942=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));_tmp498->tag=0U,({struct Cyc_List_List*_tmp941=Cyc_yyget_YY36(&(yyyyvsp[2]).v);_tmp498->f1=_tmp941;}),_tmp498->f2=(unsigned)((yyyyvsp[0]).l).first_line;_tmp498;});_tmp499->f1=_tmp942;});_tmp499;});_tmp49A->hd=_tmp943;}),({struct Cyc_List_List*_tmp940=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp49A->tl=_tmp940;});_tmp49A;});_tmp6F9.tms=_tmp944;});_tmp6F9;}));
goto _LL0;case 182U: _LL167: _LL168:
# 1978
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 1978 "parse.y"
struct Cyc_List_List*_tmp49B=({unsigned _tmp947=(unsigned)((yyyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp947,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyyvsp[2]).v)));});
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6FA;({struct _tuple0*_tmp94C=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6FA.id=_tmp94C;}),({unsigned _tmp94B=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6FA.varloc=_tmp94B;}),({struct Cyc_List_List*_tmp94A=({struct Cyc_List_List*_tmp49D=_region_malloc(yyr,sizeof(*_tmp49D));({void*_tmp949=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp49C=_region_malloc(yyr,sizeof(*_tmp49C));_tmp49C->tag=4U,_tmp49C->f1=_tmp49B,_tmp49C->f2=(unsigned)((yyyyvsp[0]).l).first_line,_tmp49C->f3=0;_tmp49C;});_tmp49D->hd=_tmp949;}),({struct Cyc_List_List*_tmp948=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp49D->tl=_tmp948;});_tmp49D;});_tmp6FA.tms=_tmp94A;});_tmp6FA;}));
# 1981
goto _LL0;};case 183U: _LL169: _LL16A:
# 1983
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 1982 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6FB;({struct _tuple0*_tmp952=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).id;_tmp6FB.id=_tmp952;}),({unsigned _tmp951=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).varloc;_tmp6FB.varloc=_tmp951;}),({struct Cyc_List_List*_tmp950=({struct Cyc_List_List*_tmp49F=_region_malloc(yyr,sizeof(*_tmp49F));({void*_tmp94F=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));_tmp49E->tag=5U,_tmp49E->f1=(unsigned)((yyyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp94E=Cyc_yyget_YY45(&(yyyyvsp[1]).v);_tmp49E->f2=_tmp94E;});_tmp49E;});_tmp49F->hd=_tmp94F;}),({
struct Cyc_List_List*_tmp94D=(Cyc_yyget_YY27(&(yyyyvsp[0]).v)).tms;_tmp49F->tl=_tmp94D;});_tmp49F;});
# 1982
_tmp6FB.tms=_tmp950;});_tmp6FB;}));
# 1985
goto _LL0;case 184U: _LL16B: _LL16C:
# 1987
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1990 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6FC;({struct _tuple0*_tmp953=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp6FC.id=_tmp953;}),_tmp6FC.varloc=(unsigned)((yyyyvsp[0]).l).first_line,_tmp6FC.tms=0;_tmp6FC;}));
goto _LL0;case 185U: _LL16D: _LL16E:
# 1993
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 1992 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6FD;({struct _tuple0*_tmp954=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp6FD.id=_tmp954;}),_tmp6FD.varloc=(unsigned)((yyyyvsp[0]).l).first_line,_tmp6FD.tms=0;_tmp6FD;}));
goto _LL0;case 186U: _LL16F: _LL170:
# 1995
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 1994 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 187U: _LL171: _LL172:
# 1997
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
struct Cyc_Parse_Declarator _tmp4A0=Cyc_yyget_YY27(&(yyyyvsp[2]).v);
({struct Cyc_List_List*_tmp957=({struct Cyc_List_List*_tmp4A2=_region_malloc(yyr,sizeof(*_tmp4A2));({void*_tmp956=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4A1=_region_malloc(yyr,sizeof(*_tmp4A1));_tmp4A1->tag=5U,_tmp4A1->f1=(unsigned)((yyyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp955=Cyc_yyget_YY45(&(yyyyvsp[1]).v);_tmp4A1->f2=_tmp955;});_tmp4A1;});_tmp4A2->hd=_tmp956;}),_tmp4A2->tl=_tmp4A0.tms;_tmp4A2;});_tmp4A0.tms=_tmp957;});
yyval=(yyyyvsp[2]).v;
# 2002
goto _LL0;};case 188U: _LL173: _LL174:
# 2004
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2003 "parse.y"
struct Cyc_Parse_Declarator _tmp4A3=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6FE;_tmp6FE.id=_tmp4A3.id,_tmp6FE.varloc=_tmp4A3.varloc,({
struct Cyc_List_List*_tmp95A=({struct Cyc_List_List*_tmp4A5=_region_malloc(yyr,sizeof(*_tmp4A5));({void*_tmp959=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4A4=_region_malloc(yyr,sizeof(*_tmp4A4));_tmp4A4->tag=0U,({void*_tmp958=Cyc_yyget_YY51(&(yyyyvsp[3]).v);_tmp4A4->f1=_tmp958;}),_tmp4A4->f2=(unsigned)((yyyyvsp[3]).l).first_line;_tmp4A4;});_tmp4A5->hd=_tmp959;}),_tmp4A5->tl=_tmp4A3.tms;_tmp4A5;});_tmp6FE.tms=_tmp95A;});_tmp6FE;}));
goto _LL0;};case 189U: _LL175: _LL176:
# 2008
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 2007 "parse.y"
struct Cyc_Parse_Declarator _tmp4A6=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp6FF;_tmp6FF.id=_tmp4A6.id,_tmp6FF.varloc=_tmp4A6.varloc,({
struct Cyc_List_List*_tmp95E=({struct Cyc_List_List*_tmp4A8=_region_malloc(yyr,sizeof(*_tmp4A8));({void*_tmp95D=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4A7=_region_malloc(yyr,sizeof(*_tmp4A7));_tmp4A7->tag=1U,({struct Cyc_Absyn_Exp*_tmp95C=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp4A7->f1=_tmp95C;}),({void*_tmp95B=Cyc_yyget_YY51(&(yyyyvsp[4]).v);_tmp4A7->f2=_tmp95B;}),_tmp4A7->f3=(unsigned)((yyyyvsp[4]).l).first_line;_tmp4A7;});_tmp4A8->hd=_tmp95D;}),_tmp4A8->tl=_tmp4A6.tms;_tmp4A8;});_tmp6FF.tms=_tmp95E;});_tmp6FF;}));
# 2011
goto _LL0;};case 190U: _LL177: _LL178:
# 2013
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
# 2012 "parse.y"
struct _tuple26*_tmp4A9=Cyc_yyget_YY39(&(yyyyvsp[2]).v);struct _tuple26*_tmp4AA=_tmp4A9;struct Cyc_List_List*_tmp4B5;int _tmp4B4;struct Cyc_Absyn_VarargInfo*_tmp4B3;void*_tmp4B2;struct Cyc_List_List*_tmp4B1;_LL437: _tmp4B5=_tmp4AA->f1;_tmp4B4=_tmp4AA->f2;_tmp4B3=_tmp4AA->f3;_tmp4B2=_tmp4AA->f4;_tmp4B1=_tmp4AA->f5;_LL438:;{
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_yyget_YY55(&(yyyyvsp[4]).v);
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_yyget_YY55(&(yyyyvsp[5]).v);
struct Cyc_Parse_Declarator _tmp4AD=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp700;_tmp700.id=_tmp4AD.id,_tmp700.varloc=_tmp4AD.varloc,({struct Cyc_List_List*_tmp961=({struct Cyc_List_List*_tmp4B0=_region_malloc(yyr,sizeof(*_tmp4B0));({void*_tmp960=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4AF=_region_malloc(yyr,sizeof(*_tmp4AF));_tmp4AF->tag=3U,({void*_tmp95F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4AE=_region_malloc(yyr,sizeof(*_tmp4AE));_tmp4AE->tag=1U,_tmp4AE->f1=_tmp4B5,_tmp4AE->f2=_tmp4B4,_tmp4AE->f3=_tmp4B3,_tmp4AE->f4=_tmp4B2,_tmp4AE->f5=_tmp4B1,_tmp4AE->f6=_tmp4AB,_tmp4AE->f7=_tmp4AC;_tmp4AE;});_tmp4AF->f1=_tmp95F;});_tmp4AF;});_tmp4B0->hd=_tmp960;}),_tmp4B0->tl=_tmp4AD.tms;_tmp4B0;});_tmp700.tms=_tmp961;});_tmp700;}));
# 2018
goto _LL0;};};case 191U: _LL179: _LL17A:
# 2020
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);{
# 2019 "parse.y"
struct Cyc_Parse_Declarator _tmp4B6=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp701;_tmp701.id=_tmp4B6.id,_tmp701.varloc=_tmp4B6.varloc,({
struct Cyc_List_List*_tmp968=({struct Cyc_List_List*_tmp4B9=_region_malloc(yyr,sizeof(*_tmp4B9));({void*_tmp967=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4B8=_region_malloc(yyr,sizeof(*_tmp4B8));_tmp4B8->tag=3U,({void*_tmp966=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));_tmp4B7->tag=1U,_tmp4B7->f1=0,_tmp4B7->f2=0,_tmp4B7->f3=0,({
# 2023
void*_tmp965=Cyc_yyget_YY49(&(yyyyvsp[2]).v);_tmp4B7->f4=_tmp965;}),({struct Cyc_List_List*_tmp964=Cyc_yyget_YY50(&(yyyyvsp[3]).v);_tmp4B7->f5=_tmp964;}),({struct Cyc_Absyn_Exp*_tmp963=Cyc_yyget_YY55(&(yyyyvsp[5]).v);_tmp4B7->f6=_tmp963;}),({struct Cyc_Absyn_Exp*_tmp962=Cyc_yyget_YY55(&(yyyyvsp[6]).v);_tmp4B7->f7=_tmp962;});_tmp4B7;});
# 2021
_tmp4B8->f1=_tmp966;});_tmp4B8;});_tmp4B9->hd=_tmp967;}),_tmp4B9->tl=_tmp4B6.tms;_tmp4B9;});_tmp701.tms=_tmp968;});_tmp701;}));
# 2026
goto _LL0;};case 192U: _LL17B: _LL17C:
# 2028
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2027 "parse.y"
struct Cyc_Parse_Declarator _tmp4BA=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp702;_tmp702.id=_tmp4BA.id,_tmp702.varloc=_tmp4BA.varloc,({struct Cyc_List_List*_tmp96C=({struct Cyc_List_List*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));({void*_tmp96B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));_tmp4BC->tag=3U,({void*_tmp96A=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4BB=_region_malloc(yyr,sizeof(*_tmp4BB));_tmp4BB->tag=0U,({struct Cyc_List_List*_tmp969=Cyc_yyget_YY36(&(yyyyvsp[2]).v);_tmp4BB->f1=_tmp969;}),_tmp4BB->f2=(unsigned)((yyyyvsp[0]).l).first_line;_tmp4BB;});_tmp4BC->f1=_tmp96A;});_tmp4BC;});_tmp4BD->hd=_tmp96B;}),_tmp4BD->tl=_tmp4BA.tms;_tmp4BD;});_tmp702.tms=_tmp96C;});_tmp702;}));
goto _LL0;};case 193U: _LL17D: _LL17E:
# 2031
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2031 "parse.y"
struct Cyc_List_List*_tmp4BE=({unsigned _tmp96D=(unsigned)((yyyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp96D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyyvsp[2]).v)));});
struct Cyc_Parse_Declarator _tmp4BF=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp703;_tmp703.id=_tmp4BF.id,_tmp703.varloc=_tmp4BF.varloc,({struct Cyc_List_List*_tmp96F=({struct Cyc_List_List*_tmp4C1=_region_malloc(yyr,sizeof(*_tmp4C1));({void*_tmp96E=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));_tmp4C0->tag=4U,_tmp4C0->f1=_tmp4BE,_tmp4C0->f2=(unsigned)((yyyyvsp[0]).l).first_line,_tmp4C0->f3=0;_tmp4C0;});_tmp4C1->hd=_tmp96E;}),_tmp4C1->tl=_tmp4BF.tms;_tmp4C1;});_tmp703.tms=_tmp96F;});_tmp703;}));
# 2035
goto _LL0;};case 194U: _LL17F: _LL180:
# 2037
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 2036 "parse.y"
struct Cyc_Parse_Declarator _tmp4C2=Cyc_yyget_YY27(&(yyyyvsp[0]).v);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp704;_tmp704.id=_tmp4C2.id,_tmp704.varloc=_tmp4C2.varloc,({struct Cyc_List_List*_tmp972=({struct Cyc_List_List*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));({void*_tmp971=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));_tmp4C3->tag=5U,_tmp4C3->f1=(unsigned)((yyyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp970=Cyc_yyget_YY45(&(yyyyvsp[1]).v);_tmp4C3->f2=_tmp970;});_tmp4C3;});_tmp4C4->hd=_tmp971;}),_tmp4C4->tl=_tmp4C2.tms;_tmp4C4;});_tmp704.tms=_tmp972;});_tmp704;}));
# 2039
goto _LL0;};case 195U: _LL181: _LL182:
# 2041
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2043 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 196U: _LL183: _LL184:
# 2046
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2044 "parse.y"
yyval=Cyc_YY26(({struct Cyc_List_List*_tmp973=Cyc_yyget_YY26(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp973,Cyc_yyget_YY26(&(yyyyvsp[1]).v));}));
goto _LL0;case 197U: _LL185: _LL186:
# 2047
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));({void*_tmp975=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));_tmp4C5->tag=5U,_tmp4C5->f1=(unsigned)((yyyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp974=Cyc_yyget_YY45(&(yyyyvsp[3]).v);_tmp4C5->f2=_tmp974;});_tmp4C5;});_tmp4C6->hd=_tmp975;}),_tmp4C6->tl=ans;_tmp4C6;});{
# 2052
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp4C7=*Cyc_yyget_YY1(&(yyyyvsp[0]).v);struct _tuple21 _tmp4C8=_tmp4C7;unsigned _tmp4D4;void*_tmp4D3;void*_tmp4D2;_LL43A: _tmp4D4=_tmp4C8.f1;_tmp4D3=_tmp4C8.f2;_tmp4D2=_tmp4C8.f3;_LL43B:;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->ptr_loc=_tmp4D4,_tmp4C9->rgn_loc=(unsigned)((yyyyvsp[2]).l).first_line,_tmp4C9->zt_loc=(unsigned)((yyyyvsp[1]).l).first_line;_tmp4C9;});{
# 2057
struct _tuple14 _tmp4CA=({unsigned _tmp979=_tmp4D4;void*_tmp978=_tmp4D3;void*_tmp977=_tmp4D2;void*_tmp976=Cyc_yyget_YY44(&(yyyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp979,_tmp978,_tmp977,_tmp976,Cyc_yyget_YY54(&(yyyyvsp[1]).v));});struct _tuple14 _tmp4CB=_tmp4CA;void*_tmp4D1;void*_tmp4D0;void*_tmp4CF;void*_tmp4CE;_LL43D: _tmp4D1=_tmp4CB.f1;_tmp4D0=_tmp4CB.f2;_tmp4CF=_tmp4CB.f3;_tmp4CE=_tmp4CB.f4;_LL43E:;
ans=({struct Cyc_List_List*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));({void*_tmp97B=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));_tmp4CC->tag=2U,(_tmp4CC->f1).rgn=_tmp4CE,(_tmp4CC->f1).nullable=_tmp4D1,(_tmp4CC->f1).bounds=_tmp4D0,(_tmp4CC->f1).zero_term=_tmp4CF,(_tmp4CC->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp97A=Cyc_yyget_YY23(&(yyyyvsp[4]).v);_tmp4CC->f2=_tmp97A;});_tmp4CC;});_tmp4CD->hd=_tmp97B;}),_tmp4CD->tl=ans;_tmp4CD;});
yyval=Cyc_YY26(ans);
# 2061
goto _LL0;};};};case 198U: _LL187: _LL188:
# 2063
 yyval=Cyc_YY54(0);
goto _LL0;case 199U: _LL189: _LL18A:
# 2066
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2064 "parse.y"
yyval=Cyc_YY54(({struct Cyc_List_List*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));({void*_tmp97D=Cyc_yyget_YY53(&(yyyyvsp[0]).v);_tmp4D5->hd=_tmp97D;}),({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY54(&(yyyyvsp[1]).v);_tmp4D5->tl=_tmp97C;});_tmp4D5;}));
goto _LL0;case 200U: _LL18B: _LL18C:
# 2067
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2069 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));_tmp4D6->tag=0U,({struct Cyc_Absyn_Exp*_tmp97E=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp4D6->f1=_tmp97E;});_tmp4D6;}));
goto _LL0;case 201U: _LL18D: _LL18E:
# 2072
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2071 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));_tmp4D7->tag=1U,({void*_tmp97F=Cyc_yyget_YY44(&(yyyyvsp[2]).v);_tmp4D7->f1=_tmp97F;});_tmp4D7;}));
goto _LL0;case 202U: _LL18F: _LL190:
# 2074
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2073 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp4D8=_region_malloc(yyr,sizeof(*_tmp4D8));_tmp4D8->tag=2U;_tmp4D8;}));
goto _LL0;case 203U: _LL191: _LL192:
# 2076
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2075 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));_tmp4D9->tag=3U;_tmp4D9;}));
goto _LL0;case 204U: _LL193: _LL194:
# 2078
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2077 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp4DA=_region_malloc(yyr,sizeof(*_tmp4DA));_tmp4DA->tag=4U;_tmp4DA;}));
goto _LL0;case 205U: _LL195: _LL196:
# 2080
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2079 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp4DB=_region_malloc(yyr,sizeof(*_tmp4DB));_tmp4DB->tag=5U;_tmp4DB;}));
goto _LL0;case 206U: _LL197: _LL198:
# 2082
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2081 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));_tmp4DC->tag=6U;_tmp4DC;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2084
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2083 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));_tmp4DD->tag=7U;_tmp4DD;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2086
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 2089 "parse.y"
unsigned loc=(unsigned)((yyyyvsp[0]).l).first_line;
if(!Cyc_Parse_parsing_tempest)
yyval=Cyc_YY1(({struct _tuple21*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->f1=loc,_tmp4DE->f2=Cyc_Absyn_true_type,({void*_tmp980=Cyc_yyget_YY2(&(yyyyvsp[1]).v);_tmp4DE->f3=_tmp980;});_tmp4DE;}));else{
# 2093
yyval=Cyc_YY1(({struct _tuple21*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->f1=loc,_tmp4DF->f2=Cyc_Absyn_true_type,_tmp4DF->f3=Cyc_Absyn_fat_bound_type;_tmp4DF;}));}
# 2095
goto _LL0;};case 209U: _LL19D: _LL19E:
# 2097
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 2096 "parse.y"
unsigned loc=(unsigned)((yyyyvsp[0]).l).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=loc,_tmp4E0->f2=Cyc_Absyn_false_type,({void*_tmp981=Cyc_yyget_YY2(&(yyyyvsp[1]).v);_tmp4E0->f3=_tmp981;});_tmp4E0;}));
# 2099
goto _LL0;};case 210U: _LL19F: _LL1A0:
# 2101
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 2100 "parse.y"
unsigned loc=(unsigned)((yyyyvsp[0]).l).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->f1=loc,_tmp4E1->f2=Cyc_Absyn_true_type,_tmp4E1->f3=Cyc_Absyn_fat_bound_type;_tmp4E1;}));
# 2103
goto _LL0;};case 211U: _LL1A1: _LL1A2:
# 2105
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2108
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2106 "parse.y"
yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v)));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2109
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 214U: _LL1A7: _LL1A8:
# 2112
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2110 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 215U: _LL1A9: _LL1AA:
# 2113
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2111 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 216U: _LL1AB: _LL1AC:
# 2114
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 217U: _LL1AD: _LL1AE:
# 2117
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2117 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyyvsp[0]).v),& Cyc_Tcutil_trk,1);yyval=(yyyyvsp[0]).v;
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2120
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2118 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2121
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2124
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2123 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp982=Cyc_yyget_YY23(&(yyyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp982,Cyc_yyget_YY23(&(yyyyvsp[1]).v));}));
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2126
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2128 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));({struct Cyc_List_List*_tmp985=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyyvsp[0]).v));_tmp4E2->f1=_tmp985;}),_tmp4E2->f2=0,_tmp4E2->f3=0,({void*_tmp984=Cyc_yyget_YY49(&(yyyyvsp[1]).v);_tmp4E2->f4=_tmp984;}),({struct Cyc_List_List*_tmp983=Cyc_yyget_YY50(&(yyyyvsp[2]).v);_tmp4E2->f5=_tmp983;});_tmp4E2;}));
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2131
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2130 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));({struct Cyc_List_List*_tmp988=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyyvsp[0]).v));_tmp4E3->f1=_tmp988;}),_tmp4E3->f2=1,_tmp4E3->f3=0,({void*_tmp987=Cyc_yyget_YY49(&(yyyyvsp[3]).v);_tmp4E3->f4=_tmp987;}),({struct Cyc_List_List*_tmp986=Cyc_yyget_YY50(&(yyyyvsp[4]).v);_tmp4E3->f5=_tmp986;});_tmp4E3;}));
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2133
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 2133 "parse.y"
struct _tuple8*_tmp4E4=Cyc_yyget_YY37(&(yyyyvsp[2]).v);struct _tuple8*_tmp4E5=_tmp4E4;struct _fat_ptr*_tmp4EB;struct Cyc_Absyn_Tqual _tmp4EA;void*_tmp4E9;_LL440: _tmp4EB=_tmp4E5->f1;_tmp4EA=_tmp4E5->f2;_tmp4E9=_tmp4E5->f3;_LL441:;{
struct Cyc_Absyn_VarargInfo*_tmp4E6=({struct Cyc_Absyn_VarargInfo*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));_tmp4E8->name=_tmp4EB,_tmp4E8->tq=_tmp4EA,_tmp4E8->type=_tmp4E9,({int _tmp989=Cyc_yyget_YY31(&(yyyyvsp[1]).v);_tmp4E8->inject=_tmp989;});_tmp4E8;});
yyval=Cyc_YY39(({struct _tuple26*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->f1=0,_tmp4E7->f2=0,_tmp4E7->f3=_tmp4E6,({void*_tmp98B=Cyc_yyget_YY49(&(yyyyvsp[3]).v);_tmp4E7->f4=_tmp98B;}),({struct Cyc_List_List*_tmp98A=Cyc_yyget_YY50(&(yyyyvsp[4]).v);_tmp4E7->f5=_tmp98A;});_tmp4E7;}));
# 2137
goto _LL0;};};case 224U: _LL1BB: _LL1BC:
# 2139
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);{
# 2139 "parse.y"
struct _tuple8*_tmp4EC=Cyc_yyget_YY37(&(yyyyvsp[4]).v);struct _tuple8*_tmp4ED=_tmp4EC;struct _fat_ptr*_tmp4F3;struct Cyc_Absyn_Tqual _tmp4F2;void*_tmp4F1;_LL443: _tmp4F3=_tmp4ED->f1;_tmp4F2=_tmp4ED->f2;_tmp4F1=_tmp4ED->f3;_LL444:;{
struct Cyc_Absyn_VarargInfo*_tmp4EE=({struct Cyc_Absyn_VarargInfo*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->name=_tmp4F3,_tmp4F0->tq=_tmp4F2,_tmp4F0->type=_tmp4F1,({int _tmp98C=Cyc_yyget_YY31(&(yyyyvsp[3]).v);_tmp4F0->inject=_tmp98C;});_tmp4F0;});
yyval=Cyc_YY39(({struct _tuple26*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));({struct Cyc_List_List*_tmp98F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyyvsp[0]).v));_tmp4EF->f1=_tmp98F;}),_tmp4EF->f2=0,_tmp4EF->f3=_tmp4EE,({void*_tmp98E=Cyc_yyget_YY49(&(yyyyvsp[5]).v);_tmp4EF->f4=_tmp98E;}),({struct Cyc_List_List*_tmp98D=Cyc_yyget_YY50(&(yyyyvsp[6]).v);_tmp4EF->f5=_tmp98D;});_tmp4EF;}));
# 2143
goto _LL0;};};case 225U: _LL1BD: _LL1BE:
# 2145
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2147 "parse.y"
yyval=Cyc_YY44(({struct _fat_ptr _tmp990=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_Parse_id2type(_tmp990,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->tag=1U,_tmp4F4->f1=0;_tmp4F4;}));}));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2150
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2148 "parse.y"
yyval=Cyc_YY44(({struct _fat_ptr _tmp991=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_Parse_id2type(_tmp991,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(&(yyyyvsp[2]).v)));}));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2151
 yyval=Cyc_YY49(0);
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2154
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2152 "parse.y"
yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyyvsp[1]).v)));
goto _LL0;case 229U: _LL1C5: _LL1C6:
# 2155
 yyval=Cyc_YY50(0);
goto _LL0;case 230U: _LL1C7: _LL1C8:
# 2158
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2157 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 231U: _LL1C9: _LL1CA:
# 2160
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);{
# 2165 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4F5=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->tag=2U,_tmp4FA->f1=0,_tmp4FA->f2=& Cyc_Tcutil_trk;_tmp4FA;});
struct _fat_ptr _tmp4F6=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);
void*_tmp4F7=Cyc_Parse_id2type(_tmp4F6,(void*)_tmp4F5);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));({struct _tuple29*_tmp993=({struct _tuple29*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));({void*_tmp992=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyyvsp[0]).v));_tmp4F8->f1=_tmp992;}),_tmp4F8->f2=_tmp4F7;_tmp4F8;});_tmp4F9->hd=_tmp993;}),_tmp4F9->tl=0;_tmp4F9;}));
# 2170
goto _LL0;};case 232U: _LL1CB: _LL1CC:
# 2172
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 2172 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4FB=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->tag=2U,_tmp500->f1=0,_tmp500->f2=& Cyc_Tcutil_trk;_tmp500;});
struct _fat_ptr _tmp4FC=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);
void*_tmp4FD=Cyc_Parse_id2type(_tmp4FC,(void*)_tmp4FB);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));({struct _tuple29*_tmp996=({struct _tuple29*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));({void*_tmp995=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyyvsp[0]).v));_tmp4FE->f1=_tmp995;}),_tmp4FE->f2=_tmp4FD;_tmp4FE;});_tmp4FF->hd=_tmp996;}),({struct Cyc_List_List*_tmp994=Cyc_yyget_YY50(&(yyyyvsp[4]).v);_tmp4FF->tl=_tmp994;});_tmp4FF;}));
# 2177
goto _LL0;};case 233U: _LL1CD: _LL1CE:
# 2179
 yyval=Cyc_YY31(0);
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2182
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2183 "parse.y"
if(({struct _fat_ptr _tmp997=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_zstrcmp(_tmp997,({const char*_tmp501="inject";_tag_fat(_tmp501,sizeof(char),7U);}));})!= 0)
({void*_tmp502=0U;({unsigned _tmp999=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp998=({const char*_tmp503="missing type in function declaration";_tag_fat(_tmp503,sizeof(char),37U);});Cyc_Warn_err(_tmp999,_tmp998,_tag_fat(_tmp502,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);
# 2187
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2189
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2193
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2191 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp99A=Cyc_yyget_YY40(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp99A,Cyc_yyget_YY40(&(yyyyvsp[2]).v));}));
goto _LL0;case 237U: _LL1D5: _LL1D6:
# 2194
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
yyval=Cyc_YY40(0);
goto _LL0;case 238U: _LL1D7: _LL1D8:
# 2198
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2196 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 239U: _LL1D9: _LL1DA:
# 2199
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2198 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));({void*_tmp99B=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyyvsp[2]).v));_tmp504->hd=_tmp99B;}),_tmp504->tl=0;_tmp504;}));
goto _LL0;case 240U: _LL1DB: _LL1DC:
# 2201
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2200 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyyvsp[0]).v),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp505=_cycalloc(sizeof(*_tmp505));({void*_tmp99C=Cyc_yyget_YY44(&(yyyyvsp[0]).v);_tmp505->hd=_tmp99C;}),_tmp505->tl=0;_tmp505;}));
# 2203
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2205
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2220 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp506=_cycalloc(sizeof(*_tmp506));({void*_tmp99E=Cyc_Absyn_access_eff(({struct _tuple8*_tmp99D=Cyc_yyget_YY37(&(yyyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp99D,(unsigned)((yyyyvsp[0]).l).first_line);}));_tmp506->hd=_tmp99E;}),_tmp506->tl=0;_tmp506;}));
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2223
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2222 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp507=_cycalloc(sizeof(*_tmp507));({void*_tmp9A1=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9A0=Cyc_yyget_YY37(&(yyyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9A0,(unsigned)((yyyyvsp[0]).l).first_line);}));_tmp507->hd=_tmp9A1;}),({struct Cyc_List_List*_tmp99F=Cyc_yyget_YY40(&(yyyyvsp[2]).v);_tmp507->tl=_tmp99F;});_tmp507;}));
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2225
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2228 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp508=_cycalloc(sizeof(*_tmp508));({struct _tuple8*_tmp9A2=Cyc_yyget_YY37(&(yyyyvsp[0]).v);_tmp508->hd=_tmp9A2;}),_tmp508->tl=0;_tmp508;}));
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2231
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2230 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp509=_cycalloc(sizeof(*_tmp509));({struct _tuple8*_tmp9A4=Cyc_yyget_YY37(&(yyyyvsp[2]).v);_tmp509->hd=_tmp9A4;}),({struct Cyc_List_List*_tmp9A3=Cyc_yyget_YY38(&(yyyyvsp[0]).v);_tmp509->tl=_tmp9A3;});_tmp509;}));
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2233
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 2236 "parse.y"
struct _tuple25 _tmp50A=Cyc_yyget_YY35(&(yyyyvsp[0]).v);struct _tuple25 _tmp50B=_tmp50A;struct Cyc_Absyn_Tqual _tmp521;struct Cyc_Parse_Type_specifier _tmp520;struct Cyc_List_List*_tmp51F;_LL446: _tmp521=_tmp50B.f1;_tmp520=_tmp50B.f2;_tmp51F=_tmp50B.f3;_LL447:;
if(_tmp521.loc == (unsigned)0)_tmp521.loc=(unsigned)((yyyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp50C=Cyc_yyget_YY27(&(yyyyvsp[1]).v);struct Cyc_Parse_Declarator _tmp50D=_tmp50C;struct _tuple0*_tmp51E;unsigned _tmp51D;struct Cyc_List_List*_tmp51C;_LL449: _tmp51E=_tmp50D.id;_tmp51D=_tmp50D.varloc;_tmp51C=_tmp50D.tms;_LL44A:;{
void*_tmp50E=Cyc_Parse_speclist2typ(_tmp520,(unsigned)((yyyyvsp[0]).l).first_line);
struct _tuple13 _tmp50F=Cyc_Parse_apply_tms(_tmp521,_tmp50E,_tmp51F,_tmp51C);struct _tuple13 _tmp510=_tmp50F;struct Cyc_Absyn_Tqual _tmp51B;void*_tmp51A;struct Cyc_List_List*_tmp519;struct Cyc_List_List*_tmp518;_LL44C: _tmp51B=_tmp510.f1;_tmp51A=_tmp510.f2;_tmp519=_tmp510.f3;_tmp518=_tmp510.f4;_LL44D:;
if(_tmp519 != 0)
({void*_tmp511=0U;({unsigned _tmp9A6=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmp9A5=({const char*_tmp512="parameter with bad type params";_tag_fat(_tmp512,sizeof(char),31U);});Cyc_Warn_err(_tmp9A6,_tmp9A5,_tag_fat(_tmp511,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp51E))
({void*_tmp513=0U;({unsigned _tmp9A8=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp9A7=({const char*_tmp514="parameter cannot be qualified with a namespace";_tag_fat(_tmp514,sizeof(char),47U);});Cyc_Warn_err(_tmp9A8,_tmp9A7,_tag_fat(_tmp513,sizeof(void*),0U));});});{
struct _fat_ptr*idopt=(*_tmp51E).f2;
if(_tmp518 != 0)
({void*_tmp515=0U;({unsigned _tmp9AA=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp9A9=({const char*_tmp516="extra attributes on parameter, ignoring";_tag_fat(_tmp516,sizeof(char),40U);});Cyc_Warn_warn(_tmp9AA,_tmp9A9,_tag_fat(_tmp515,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->f1=idopt,_tmp517->f2=_tmp51B,_tmp517->f3=_tmp51A;_tmp517;}));
# 2250
goto _LL0;};};};};case 246U: _LL1E7: _LL1E8:
# 2252
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 2251 "parse.y"
struct _tuple25 _tmp522=Cyc_yyget_YY35(&(yyyyvsp[0]).v);struct _tuple25 _tmp523=_tmp522;struct Cyc_Absyn_Tqual _tmp52A;struct Cyc_Parse_Type_specifier _tmp529;struct Cyc_List_List*_tmp528;_LL44F: _tmp52A=_tmp523.f1;_tmp529=_tmp523.f2;_tmp528=_tmp523.f3;_LL450:;
if(_tmp52A.loc == (unsigned)0)_tmp52A.loc=(unsigned)((yyyyvsp[0]).l).first_line;{
void*_tmp524=Cyc_Parse_speclist2typ(_tmp529,(unsigned)((yyyyvsp[0]).l).first_line);
if(_tmp528 != 0)
({void*_tmp525=0U;({unsigned _tmp9AC=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp9AB=({const char*_tmp526="bad attributes on parameter, ignoring";_tag_fat(_tmp526,sizeof(char),38U);});Cyc_Warn_warn(_tmp9AC,_tmp9AB,_tag_fat(_tmp525,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->f1=0,_tmp527->f2=_tmp52A,_tmp527->f3=_tmp524;_tmp527;}));
# 2258
goto _LL0;};};case 247U: _LL1E9: _LL1EA:
# 2260
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 2259 "parse.y"
struct _tuple25 _tmp52B=Cyc_yyget_YY35(&(yyyyvsp[0]).v);struct _tuple25 _tmp52C=_tmp52B;struct Cyc_Absyn_Tqual _tmp53C;struct Cyc_Parse_Type_specifier _tmp53B;struct Cyc_List_List*_tmp53A;_LL452: _tmp53C=_tmp52C.f1;_tmp53B=_tmp52C.f2;_tmp53A=_tmp52C.f3;_LL453:;
if(_tmp53C.loc == (unsigned)0)_tmp53C.loc=(unsigned)((yyyyvsp[0]).l).first_line;{
void*_tmp52D=Cyc_Parse_speclist2typ(_tmp53B,(unsigned)((yyyyvsp[0]).l).first_line);
struct Cyc_List_List*_tmp52E=(Cyc_yyget_YY30(&(yyyyvsp[1]).v)).tms;
struct _tuple13 _tmp52F=Cyc_Parse_apply_tms(_tmp53C,_tmp52D,_tmp53A,_tmp52E);struct _tuple13 _tmp530=_tmp52F;struct Cyc_Absyn_Tqual _tmp539;void*_tmp538;struct Cyc_List_List*_tmp537;struct Cyc_List_List*_tmp536;_LL455: _tmp539=_tmp530.f1;_tmp538=_tmp530.f2;_tmp537=_tmp530.f3;_tmp536=_tmp530.f4;_LL456:;
if(_tmp537 != 0)
({void*_tmp531=0U;({unsigned _tmp9AE=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp9AD=({const char*_tmp532="bad type parameters on formal argument, ignoring";_tag_fat(_tmp532,sizeof(char),49U);});Cyc_Warn_warn(_tmp9AE,_tmp9AD,_tag_fat(_tmp531,sizeof(void*),0U));});});
# 2267
if(_tmp536 != 0)
({void*_tmp533=0U;({unsigned _tmp9B0=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp9AF=({const char*_tmp534="bad attributes on parameter, ignoring";_tag_fat(_tmp534,sizeof(char),38U);});Cyc_Warn_warn(_tmp9B0,_tmp9AF,_tag_fat(_tmp533,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->f1=0,_tmp535->f2=_tmp539,_tmp535->f3=_tmp538;_tmp535;}));
# 2271
goto _LL0;};};case 248U: _LL1EB: _LL1EC:
# 2273
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2275 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyyvsp[0]).v)));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2278
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp53E=_cycalloc(sizeof(*_tmp53E));({struct _fat_ptr*_tmp9B2=({struct _fat_ptr*_tmp53D=_cycalloc(sizeof(*_tmp53D));({struct _fat_ptr _tmp9B1=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp53D=_tmp9B1;});_tmp53D;});_tmp53E->hd=_tmp9B2;}),_tmp53E->tl=0;_tmp53E;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2282
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2281 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp540=_cycalloc(sizeof(*_tmp540));({struct _fat_ptr*_tmp9B5=({struct _fat_ptr*_tmp53F=_cycalloc(sizeof(*_tmp53F));({struct _fat_ptr _tmp9B4=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp53F=_tmp9B4;});_tmp53F;});_tmp540->hd=_tmp9B5;}),({struct Cyc_List_List*_tmp9B3=Cyc_yyget_YY36(&(yyyyvsp[0]).v);_tmp540->tl=_tmp9B3;});_tmp540;}));
goto _LL0;case 251U: _LL1F1: _LL1F2:
# 2284
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 252U: _LL1F3: _LL1F4:
# 2288
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2286 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 253U: _LL1F5: _LL1F6:
# 2289
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2291 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9B6=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->tag=36U,_tmp541->f1=0,_tmp541->f2=0;_tmp541;});Cyc_Absyn_new_exp(_tmp9B6,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 254U: _LL1F7: _LL1F8:
# 2294
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2293 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9B8=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->tag=36U,_tmp542->f1=0,({struct Cyc_List_List*_tmp9B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyyvsp[1]).v));_tmp542->f2=_tmp9B7;});_tmp542;});Cyc_Absyn_new_exp(_tmp9B8,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 255U: _LL1F9: _LL1FA:
# 2296
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2295 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9BA=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->tag=36U,_tmp543->f1=0,({struct Cyc_List_List*_tmp9B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyyvsp[1]).v));_tmp543->f2=_tmp9B9;});_tmp543;});Cyc_Absyn_new_exp(_tmp9BA,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2298
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 2297 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp544=({unsigned _tmp9BF=(unsigned)((yyyyvsp[2]).l).first_line;struct _tuple0*_tmp9BE=({struct _tuple0*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp9BC=({struct _fat_ptr*_tmp546=_cycalloc(sizeof(*_tmp546));({struct _fat_ptr _tmp9BB=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp546=_tmp9BB;});_tmp546;});_tmp547->f2=_tmp9BC;});_tmp547;});void*_tmp9BD=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp9BF,_tmp9BE,_tmp9BD,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyyvsp[2]).l).first_line));});
# 2300
(_tmp544->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp9C2=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->tag=27U,_tmp545->f1=_tmp544,({struct Cyc_Absyn_Exp*_tmp9C1=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);_tmp545->f2=_tmp9C1;}),({struct Cyc_Absyn_Exp*_tmp9C0=Cyc_yyget_Exp_tok(&(yyyyvsp[6]).v);_tmp545->f3=_tmp9C0;}),_tmp545->f4=0;_tmp545;});Cyc_Absyn_new_exp(_tmp9C2,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2303
goto _LL0;};case 257U: _LL1FD: _LL1FE:
# 2305
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 2305 "parse.y"
void*_tmp548=({struct _tuple8*_tmp9C3=Cyc_yyget_YY37(&(yyyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmp9C3,(unsigned)((yyyyvsp[6]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_tmp9C5=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->tag=28U,({struct Cyc_Absyn_Exp*_tmp9C4=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);_tmp549->f1=_tmp9C4;}),_tmp549->f2=_tmp548,_tmp549->f3=0;_tmp549;});Cyc_Absyn_new_exp(_tmp9C5,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2308
goto _LL0;};case 258U: _LL1FF: _LL200:
# 2310
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2313 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));({struct _tuple30*_tmp9C7=({struct _tuple30*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->f1=0,({struct Cyc_Absyn_Exp*_tmp9C6=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);_tmp54A->f2=_tmp9C6;});_tmp54A;});_tmp54B->hd=_tmp9C7;}),_tmp54B->tl=0;_tmp54B;}));
goto _LL0;case 259U: _LL201: _LL202:
# 2316
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2315 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp54D=_cycalloc(sizeof(*_tmp54D));({struct _tuple30*_tmp9CA=({struct _tuple30*_tmp54C=_cycalloc(sizeof(*_tmp54C));({struct Cyc_List_List*_tmp9C9=Cyc_yyget_YY41(&(yyyyvsp[0]).v);_tmp54C->f1=_tmp9C9;}),({struct Cyc_Absyn_Exp*_tmp9C8=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);_tmp54C->f2=_tmp9C8;});_tmp54C;});_tmp54D->hd=_tmp9CA;}),_tmp54D->tl=0;_tmp54D;}));
goto _LL0;case 260U: _LL203: _LL204:
# 2318
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2317 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp54F=_cycalloc(sizeof(*_tmp54F));({struct _tuple30*_tmp9CD=({struct _tuple30*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->f1=0,({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp54E->f2=_tmp9CC;});_tmp54E;});_tmp54F->hd=_tmp9CD;}),({struct Cyc_List_List*_tmp9CB=Cyc_yyget_YY5(&(yyyyvsp[0]).v);_tmp54F->tl=_tmp9CB;});_tmp54F;}));
goto _LL0;case 261U: _LL205: _LL206:
# 2320
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2319 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));({struct _tuple30*_tmp9D1=({struct _tuple30*_tmp550=_cycalloc(sizeof(*_tmp550));({struct Cyc_List_List*_tmp9D0=Cyc_yyget_YY41(&(yyyyvsp[2]).v);_tmp550->f1=_tmp9D0;}),({struct Cyc_Absyn_Exp*_tmp9CF=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);_tmp550->f2=_tmp9CF;});_tmp550;});_tmp551->hd=_tmp9D1;}),({struct Cyc_List_List*_tmp9CE=Cyc_yyget_YY5(&(yyyyvsp[0]).v);_tmp551->tl=_tmp9CE;});_tmp551;}));
goto _LL0;case 262U: _LL207: _LL208:
# 2322
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyyvsp[0]).v)));
goto _LL0;case 263U: _LL209: _LL20A:
# 2326
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2324 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp554=_cycalloc(sizeof(*_tmp554));({void*_tmp9D4=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=1U,({struct _fat_ptr*_tmp9D3=({struct _fat_ptr*_tmp552=_cycalloc(sizeof(*_tmp552));({struct _fat_ptr _tmp9D2=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp552=_tmp9D2;});_tmp552;});_tmp553->f1=_tmp9D3;});_tmp553;});_tmp554->hd=_tmp9D4;}),_tmp554->tl=0;_tmp554;}));
goto _LL0;case 264U: _LL20B: _LL20C:
# 2327
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2329 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp555=_cycalloc(sizeof(*_tmp555));({void*_tmp9D5=Cyc_yyget_YY42(&(yyyyvsp[0]).v);_tmp555->hd=_tmp9D5;}),_tmp555->tl=0;_tmp555;}));
goto _LL0;case 265U: _LL20D: _LL20E:
# 2332
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2330 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp556=_cycalloc(sizeof(*_tmp556));({void*_tmp9D7=Cyc_yyget_YY42(&(yyyyvsp[1]).v);_tmp556->hd=_tmp9D7;}),({struct Cyc_List_List*_tmp9D6=Cyc_yyget_YY41(&(yyyyvsp[0]).v);_tmp556->tl=_tmp9D6;});_tmp556;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2333
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->tag=0U,({struct Cyc_Absyn_Exp*_tmp9D8=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);_tmp557->f1=_tmp9D8;});_tmp557;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2337
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2335 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->tag=1U,({struct _fat_ptr*_tmp9DA=({struct _fat_ptr*_tmp558=_cycalloc(sizeof(*_tmp558));({struct _fat_ptr _tmp9D9=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);*_tmp558=_tmp9D9;});_tmp558;});_tmp559->f1=_tmp9DA;});_tmp559;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2338
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 2340 "parse.y"
struct _tuple25 _tmp55A=Cyc_yyget_YY35(&(yyyyvsp[0]).v);struct _tuple25 _tmp55B=_tmp55A;struct Cyc_Absyn_Tqual _tmp562;struct Cyc_Parse_Type_specifier _tmp561;struct Cyc_List_List*_tmp560;_LL458: _tmp562=_tmp55B.f1;_tmp561=_tmp55B.f2;_tmp560=_tmp55B.f3;_LL459:;{
void*_tmp55C=Cyc_Parse_speclist2typ(_tmp561,(unsigned)((yyyyvsp[0]).l).first_line);
if(_tmp560 != 0)
({void*_tmp55D=0U;({unsigned _tmp9DC=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmp9DB=({const char*_tmp55E="ignoring attributes in type";_tag_fat(_tmp55E,sizeof(char),28U);});Cyc_Warn_warn(_tmp9DC,_tmp9DB,_tag_fat(_tmp55D,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->f1=0,_tmp55F->f2=_tmp562,_tmp55F->f3=_tmp55C;_tmp55F;}));
# 2346
goto _LL0;};};case 269U: _LL215: _LL216:
# 2348
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);{
# 2347 "parse.y"
struct _tuple25 _tmp563=Cyc_yyget_YY35(&(yyyyvsp[0]).v);struct _tuple25 _tmp564=_tmp563;struct Cyc_Absyn_Tqual _tmp56F;struct Cyc_Parse_Type_specifier _tmp56E;struct Cyc_List_List*_tmp56D;_LL45B: _tmp56F=_tmp564.f1;_tmp56E=_tmp564.f2;_tmp56D=_tmp564.f3;_LL45C:;{
void*_tmp565=Cyc_Parse_speclist2typ(_tmp56E,(unsigned)((yyyyvsp[0]).l).first_line);
struct Cyc_List_List*_tmp566=(Cyc_yyget_YY30(&(yyyyvsp[1]).v)).tms;
struct _tuple13 _tmp567=Cyc_Parse_apply_tms(_tmp56F,_tmp565,_tmp56D,_tmp566);
if(_tmp567.f3 != 0)
# 2353
({void*_tmp568=0U;({unsigned _tmp9DE=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmp9DD=({const char*_tmp569="bad type params, ignoring";_tag_fat(_tmp569,sizeof(char),26U);});Cyc_Warn_warn(_tmp9DE,_tmp9DD,_tag_fat(_tmp568,sizeof(void*),0U));});});
if(_tmp567.f4 != 0)
({void*_tmp56A=0U;({unsigned _tmp9E0=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmp9DF=({const char*_tmp56B="bad specifiers, ignoring";_tag_fat(_tmp56B,sizeof(char),25U);});Cyc_Warn_warn(_tmp9E0,_tmp9DF,_tag_fat(_tmp56A,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->f1=0,_tmp56C->f2=_tmp567.f1,_tmp56C->f3=_tmp567.f2;_tmp56C;}));
# 2358
goto _LL0;};};case 270U: _LL217: _LL218:
# 2360
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY44(({struct _tuple8*_tmp9E1=Cyc_yyget_YY37(&(yyyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9E1,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2364
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2362 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2365
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2363 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyyvsp[1]).v)));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2366
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2364 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyyvsp[2]).v)));
goto _LL0;case 274U: _LL21F: _LL220:
# 2367
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2365 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp570=_cycalloc(sizeof(*_tmp570));({void*_tmp9E3=Cyc_yyget_YY44(&(yyyyvsp[0]).v);_tmp570->hd=_tmp9E3;}),({struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY40(&(yyyyvsp[2]).v);_tmp570->tl=_tmp9E2;});_tmp570;})));
goto _LL0;case 275U: _LL221: _LL222:
# 2368
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2371 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));({void*_tmp9E4=Cyc_yyget_YY44(&(yyyyvsp[0]).v);_tmp571->hd=_tmp9E4;}),_tmp571->tl=0;_tmp571;}));
goto _LL0;case 276U: _LL223: _LL224:
# 2374
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2372 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));({void*_tmp9E6=Cyc_yyget_YY44(&(yyyyvsp[2]).v);_tmp572->hd=_tmp9E6;}),({struct Cyc_List_List*_tmp9E5=Cyc_yyget_YY40(&(yyyyvsp[0]).v);_tmp572->tl=_tmp9E5;});_tmp572;}));
goto _LL0;case 277U: _LL225: _LL226:
# 2375
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2377 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp705;({struct Cyc_List_List*_tmp9E7=Cyc_yyget_YY26(&(yyyyvsp[0]).v);_tmp705.tms=_tmp9E7;});_tmp705;}));
goto _LL0;case 278U: _LL227: _LL228:
# 2380
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2379 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 279U: _LL229: _LL22A:
# 2382
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2381 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp706;({struct Cyc_List_List*_tmp9E9=({struct Cyc_List_List*_tmp9E8=Cyc_yyget_YY26(&(yyyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9E8,(Cyc_yyget_YY30(&(yyyyvsp[1]).v)).tms);});_tmp706.tms=_tmp9E9;});_tmp706;}));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2384
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2386 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 281U: _LL22D: _LL22E:
# 2389
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2388 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp707;({struct Cyc_List_List*_tmp9EC=({struct Cyc_List_List*_tmp574=_region_malloc(yyr,sizeof(*_tmp574));({void*_tmp9EB=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp573=_region_malloc(yyr,sizeof(*_tmp573));_tmp573->tag=0U,({void*_tmp9EA=Cyc_yyget_YY51(&(yyyyvsp[2]).v);_tmp573->f1=_tmp9EA;}),_tmp573->f2=(unsigned)((yyyyvsp[2]).l).first_line;_tmp573;});_tmp574->hd=_tmp9EB;}),_tmp574->tl=0;_tmp574;});_tmp707.tms=_tmp9EC;});_tmp707;}));
goto _LL0;case 282U: _LL22F: _LL230:
# 2391
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2390 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp708;({struct Cyc_List_List*_tmp9F0=({struct Cyc_List_List*_tmp576=_region_malloc(yyr,sizeof(*_tmp576));({void*_tmp9EF=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp575=_region_malloc(yyr,sizeof(*_tmp575));_tmp575->tag=0U,({void*_tmp9EE=Cyc_yyget_YY51(&(yyyyvsp[3]).v);_tmp575->f1=_tmp9EE;}),_tmp575->f2=(unsigned)((yyyyvsp[3]).l).first_line;_tmp575;});_tmp576->hd=_tmp9EF;}),({struct Cyc_List_List*_tmp9ED=(Cyc_yyget_YY30(&(yyyyvsp[0]).v)).tms;_tmp576->tl=_tmp9ED;});_tmp576;});_tmp708.tms=_tmp9F0;});_tmp708;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2393
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2392 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp709;({struct Cyc_List_List*_tmp9F4=({struct Cyc_List_List*_tmp578=_region_malloc(yyr,sizeof(*_tmp578));({void*_tmp9F3=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp577=_region_malloc(yyr,sizeof(*_tmp577));_tmp577->tag=1U,({struct Cyc_Absyn_Exp*_tmp9F2=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);_tmp577->f1=_tmp9F2;}),({void*_tmp9F1=Cyc_yyget_YY51(&(yyyyvsp[3]).v);_tmp577->f2=_tmp9F1;}),_tmp577->f3=(unsigned)((yyyyvsp[3]).l).first_line;_tmp577;});_tmp578->hd=_tmp9F3;}),_tmp578->tl=0;_tmp578;});_tmp709.tms=_tmp9F4;});_tmp709;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2395
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2394 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70A;({struct Cyc_List_List*_tmp9F9=({struct Cyc_List_List*_tmp57A=_region_malloc(yyr,sizeof(*_tmp57A));({void*_tmp9F8=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp579=_region_malloc(yyr,sizeof(*_tmp579));_tmp579->tag=1U,({struct Cyc_Absyn_Exp*_tmp9F7=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp579->f1=_tmp9F7;}),({void*_tmp9F6=Cyc_yyget_YY51(&(yyyyvsp[4]).v);_tmp579->f2=_tmp9F6;}),_tmp579->f3=(unsigned)((yyyyvsp[4]).l).first_line;_tmp579;});_tmp57A->hd=_tmp9F8;}),({
struct Cyc_List_List*_tmp9F5=(Cyc_yyget_YY30(&(yyyyvsp[0]).v)).tms;_tmp57A->tl=_tmp9F5;});_tmp57A;});
# 2394
_tmp70A.tms=_tmp9F9;});_tmp70A;}));
# 2397
goto _LL0;case 285U: _LL235: _LL236:
# 2399
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 2398 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70B;({struct Cyc_List_List*_tmpA00=({struct Cyc_List_List*_tmp57D=_region_malloc(yyr,sizeof(*_tmp57D));({void*_tmp9FF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp57C=_region_malloc(yyr,sizeof(*_tmp57C));_tmp57C->tag=3U,({void*_tmp9FE=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp57B=_region_malloc(yyr,sizeof(*_tmp57B));_tmp57B->tag=1U,_tmp57B->f1=0,_tmp57B->f2=0,_tmp57B->f3=0,({void*_tmp9FD=Cyc_yyget_YY49(&(yyyyvsp[1]).v);_tmp57B->f4=_tmp9FD;}),({struct Cyc_List_List*_tmp9FC=Cyc_yyget_YY50(&(yyyyvsp[2]).v);_tmp57B->f5=_tmp9FC;}),({struct Cyc_Absyn_Exp*_tmp9FB=Cyc_yyget_YY55(&(yyyyvsp[4]).v);_tmp57B->f6=_tmp9FB;}),({struct Cyc_Absyn_Exp*_tmp9FA=Cyc_yyget_YY55(&(yyyyvsp[5]).v);_tmp57B->f7=_tmp9FA;});_tmp57B;});_tmp57C->f1=_tmp9FE;});_tmp57C;});_tmp57D->hd=_tmp9FF;}),_tmp57D->tl=0;_tmp57D;});_tmp70B.tms=_tmpA00;});_tmp70B;}));
# 2400
goto _LL0;case 286U: _LL237: _LL238:
# 2402
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 2401 "parse.y"
struct _tuple26*_tmp57E=Cyc_yyget_YY39(&(yyyyvsp[1]).v);struct _tuple26*_tmp57F=_tmp57E;struct Cyc_List_List*_tmp587;int _tmp586;struct Cyc_Absyn_VarargInfo*_tmp585;void*_tmp584;struct Cyc_List_List*_tmp583;_LL45E: _tmp587=_tmp57F->f1;_tmp586=_tmp57F->f2;_tmp585=_tmp57F->f3;_tmp584=_tmp57F->f4;_tmp583=_tmp57F->f5;_LL45F:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70C;({struct Cyc_List_List*_tmpA05=({struct Cyc_List_List*_tmp582=_region_malloc(yyr,sizeof(*_tmp582));({void*_tmpA04=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp581=_region_malloc(yyr,sizeof(*_tmp581));_tmp581->tag=3U,({void*_tmpA03=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp580=_region_malloc(yyr,sizeof(*_tmp580));_tmp580->tag=1U,_tmp580->f1=_tmp587,_tmp580->f2=_tmp586,_tmp580->f3=_tmp585,_tmp580->f4=_tmp584,_tmp580->f5=_tmp583,({struct Cyc_Absyn_Exp*_tmpA02=Cyc_yyget_YY55(&(yyyyvsp[3]).v);_tmp580->f6=_tmpA02;}),({struct Cyc_Absyn_Exp*_tmpA01=Cyc_yyget_YY55(&(yyyyvsp[4]).v);_tmp580->f7=_tmpA01;});_tmp580;});_tmp581->f1=_tmpA03;});_tmp581;});_tmp582->hd=_tmpA04;}),_tmp582->tl=0;_tmp582;});_tmp70C.tms=_tmpA05;});_tmp70C;}));
# 2404
goto _LL0;};case 287U: _LL239: _LL23A:
# 2406
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2405 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70D;({struct Cyc_List_List*_tmpA0D=({struct Cyc_List_List*_tmp58A=_region_malloc(yyr,sizeof(*_tmp58A));({void*_tmpA0C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp589=_region_malloc(yyr,sizeof(*_tmp589));_tmp589->tag=3U,({void*_tmpA0B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp588=_region_malloc(yyr,sizeof(*_tmp588));_tmp588->tag=1U,_tmp588->f1=0,_tmp588->f2=0,_tmp588->f3=0,({void*_tmpA0A=Cyc_yyget_YY49(&(yyyyvsp[2]).v);_tmp588->f4=_tmpA0A;}),({struct Cyc_List_List*_tmpA09=Cyc_yyget_YY50(&(yyyyvsp[3]).v);_tmp588->f5=_tmpA09;}),({struct Cyc_Absyn_Exp*_tmpA08=Cyc_yyget_YY55(&(yyyyvsp[5]).v);_tmp588->f6=_tmpA08;}),({struct Cyc_Absyn_Exp*_tmpA07=Cyc_yyget_YY55(&(yyyyvsp[6]).v);_tmp588->f7=_tmpA07;});_tmp588;});_tmp589->f1=_tmpA0B;});_tmp589;});_tmp58A->hd=_tmpA0C;}),({
struct Cyc_List_List*_tmpA06=(Cyc_yyget_YY30(&(yyyyvsp[0]).v)).tms;_tmp58A->tl=_tmpA06;});_tmp58A;});
# 2405
_tmp70D.tms=_tmpA0D;});_tmp70D;}));
# 2408
goto _LL0;case 288U: _LL23B: _LL23C:
# 2410
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
# 2409 "parse.y"
struct _tuple26*_tmp58B=Cyc_yyget_YY39(&(yyyyvsp[2]).v);struct _tuple26*_tmp58C=_tmp58B;struct Cyc_List_List*_tmp594;int _tmp593;struct Cyc_Absyn_VarargInfo*_tmp592;void*_tmp591;struct Cyc_List_List*_tmp590;_LL461: _tmp594=_tmp58C->f1;_tmp593=_tmp58C->f2;_tmp592=_tmp58C->f3;_tmp591=_tmp58C->f4;_tmp590=_tmp58C->f5;_LL462:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70E;({struct Cyc_List_List*_tmpA13=({struct Cyc_List_List*_tmp58F=_region_malloc(yyr,sizeof(*_tmp58F));({void*_tmpA12=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp58E=_region_malloc(yyr,sizeof(*_tmp58E));_tmp58E->tag=3U,({void*_tmpA11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp58D=_region_malloc(yyr,sizeof(*_tmp58D));_tmp58D->tag=1U,_tmp58D->f1=_tmp594,_tmp58D->f2=_tmp593,_tmp58D->f3=_tmp592,_tmp58D->f4=_tmp591,_tmp58D->f5=_tmp590,({
struct Cyc_Absyn_Exp*_tmpA10=Cyc_yyget_YY55(&(yyyyvsp[4]).v);_tmp58D->f6=_tmpA10;}),({struct Cyc_Absyn_Exp*_tmpA0F=Cyc_yyget_YY55(&(yyyyvsp[5]).v);_tmp58D->f7=_tmpA0F;});_tmp58D;});
# 2410
_tmp58E->f1=_tmpA11;});_tmp58E;});_tmp58F->hd=_tmpA12;}),({
struct Cyc_List_List*_tmpA0E=(Cyc_yyget_YY30(&(yyyyvsp[0]).v)).tms;_tmp58F->tl=_tmpA0E;});_tmp58F;});
# 2410
_tmp70E.tms=_tmpA13;});_tmp70E;}));
# 2413
goto _LL0;};case 289U: _LL23D: _LL23E:
# 2415
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2415 "parse.y"
struct Cyc_List_List*_tmp595=({unsigned _tmpA14=(unsigned)((yyyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA14,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyyvsp[2]).v)));});
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70F;({struct Cyc_List_List*_tmpA17=({struct Cyc_List_List*_tmp597=_region_malloc(yyr,sizeof(*_tmp597));({void*_tmpA16=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp596=_region_malloc(yyr,sizeof(*_tmp596));_tmp596->tag=4U,_tmp596->f1=_tmp595,_tmp596->f2=(unsigned)((yyyyvsp[1]).l).first_line,_tmp596->f3=0;_tmp596;});_tmp597->hd=_tmpA16;}),({
struct Cyc_List_List*_tmpA15=(Cyc_yyget_YY30(&(yyyyvsp[0]).v)).tms;_tmp597->tl=_tmpA15;});_tmp597;});
# 2416
_tmp70F.tms=_tmpA17;});_tmp70F;}));
# 2419
goto _LL0;};case 290U: _LL23F: _LL240:
# 2421
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2420 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp710;({struct Cyc_List_List*_tmpA1B=({struct Cyc_List_List*_tmp599=_region_malloc(yyr,sizeof(*_tmp599));({void*_tmpA1A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp598=_region_malloc(yyr,sizeof(*_tmp598));_tmp598->tag=5U,_tmp598->f1=(unsigned)((yyyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA19=Cyc_yyget_YY45(&(yyyyvsp[1]).v);_tmp598->f2=_tmpA19;});_tmp598;});_tmp599->hd=_tmpA1A;}),({struct Cyc_List_List*_tmpA18=(Cyc_yyget_YY30(&(yyyyvsp[0]).v)).tms;_tmp599->tl=_tmpA18;});_tmp599;});_tmp710.tms=_tmpA1B;});_tmp710;}));
# 2422
goto _LL0;case 291U: _LL241: _LL242:
# 2424
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2426 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 292U: _LL243: _LL244:
# 2429
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2427 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 293U: _LL245: _LL246:
# 2430
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2428 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 294U: _LL247: _LL248:
# 2431
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2429 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 295U: _LL249: _LL24A:
# 2432
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2430 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 296U: _LL24B: _LL24C:
# 2433
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2431 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 297U: _LL24D: _LL24E:
# 2434
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2437 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA1F=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->tag=13U,({struct _fat_ptr*_tmpA1E=({struct _fat_ptr*_tmp59A=_cycalloc(sizeof(*_tmp59A));({struct _fat_ptr _tmpA1D=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp59A=_tmpA1D;});_tmp59A;});_tmp59B->f1=_tmpA1E;}),({struct Cyc_Absyn_Stmt*_tmpA1C=Cyc_yyget_Stmt_tok(&(yyyyvsp[2]).v);_tmp59B->f2=_tmpA1C;});_tmp59B;});Cyc_Absyn_new_stmt(_tmpA1F,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 298U: _LL24F: _LL250:
# 2440
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 299U: _LL251: _LL252:
# 2444
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2442 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA20=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA20,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 300U: _LL253: _LL254:
# 2445
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2447 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 301U: _LL255: _LL256:
# 2450
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2448 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2451
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2453 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA21=Cyc_yyget_YY16(&(yyyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA21,Cyc_Absyn_skip_stmt((unsigned)((yyyyvsp[0]).l).first_line));}));
goto _LL0;case 303U: _LL259: _LL25A:
# 2456
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2454 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA22=Cyc_yyget_YY16(&(yyyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA22,Cyc_yyget_Stmt_tok(&(yyyyvsp[1]).v));}));
goto _LL0;case 304U: _LL25B: _LL25C:
# 2457
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2455 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA27=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->tag=13U,({struct _fat_ptr*_tmpA26=({struct _fat_ptr*_tmp59C=_cycalloc(sizeof(*_tmp59C));({struct _fat_ptr _tmpA25=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp59C=_tmpA25;});_tmp59C;});_tmp59D->f1=_tmpA26;}),({struct Cyc_Absyn_Stmt*_tmpA24=({struct Cyc_List_List*_tmpA23=Cyc_yyget_YY16(&(yyyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA23,Cyc_Absyn_skip_stmt(0U));});_tmp59D->f2=_tmpA24;});_tmp59D;});Cyc_Absyn_new_stmt(_tmpA27,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2457
goto _LL0;case 305U: _LL25D: _LL25E:
# 2459
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2457 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA2C=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->tag=13U,({struct _fat_ptr*_tmpA2B=({struct _fat_ptr*_tmp59E=_cycalloc(sizeof(*_tmp59E));({struct _fat_ptr _tmpA2A=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp59E=_tmpA2A;});_tmp59E;});_tmp59F->f1=_tmpA2B;}),({struct Cyc_Absyn_Stmt*_tmpA29=({struct Cyc_List_List*_tmpA28=Cyc_yyget_YY16(&(yyyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA28,Cyc_yyget_Stmt_tok(&(yyyyvsp[3]).v));});_tmp59F->f2=_tmpA29;});_tmp59F;});Cyc_Absyn_new_stmt(_tmpA2C,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2459
goto _LL0;case 306U: _LL25F: _LL260:
# 2461
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2459 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 307U: _LL261: _LL262:
# 2462
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2460 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA2E=Cyc_yyget_Stmt_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA2D=Cyc_yyget_Stmt_tok(&(yyyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA2E,_tmpA2D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 308U: _LL263: _LL264:
# 2463
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2461 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA31=({void*_tmpA30=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA2F=Cyc_yyget_YY15(&(yyyyvsp[0]).v);_tmp5A0->f1=_tmpA2F;});_tmp5A0;});Cyc_Absyn_new_decl(_tmpA30,(unsigned)((yyyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA31,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 309U: _LL265: _LL266:
# 2465
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2464 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA34=({void*_tmpA33=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA32=Cyc_yyget_YY15(&(yyyyvsp[0]).v);_tmp5A1->f1=_tmpA32;});_tmp5A1;});Cyc_Absyn_new_decl(_tmpA33,(unsigned)((yyyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA34,Cyc_yyget_Stmt_tok(&(yyyyvsp[1]).v));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2467
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2469 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA37=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA36=Cyc_yyget_Stmt_tok(&(yyyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA35=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA37,_tmpA36,_tmpA35,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 311U: _LL269: _LL26A:
# 2472
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2471 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA3A=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA39=Cyc_yyget_Stmt_tok(&(yyyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA38=Cyc_yyget_Stmt_tok(&(yyyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpA3A,_tmpA39,_tmpA38,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 312U: _LL26B: _LL26C:
# 2474
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2477 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA3C=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_List_List*_tmpA3B=Cyc_yyget_YY8(&(yyyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpA3C,_tmpA3B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 313U: _LL26D: _LL26E:
# 2480
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 2480 "parse.y"
struct Cyc_Absyn_Exp*_tmp5A2=({struct _tuple0*_tmpA3D=Cyc_yyget_QualId_tok(&(yyyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpA3D,(unsigned)((yyyyvsp[1]).l).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA3F=_tmp5A2;struct Cyc_List_List*_tmpA3E=Cyc_yyget_YY8(&(yyyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpA3F,_tmpA3E,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;};case 314U: _LL26F: _LL270:
# 2484
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 2484 "parse.y"
struct Cyc_Absyn_Exp*_tmp5A3=({struct Cyc_List_List*_tmpA40=Cyc_yyget_YY4(&(yyyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpA40,(unsigned)((yyyyvsp[1]).l).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA42=_tmp5A3;struct Cyc_List_List*_tmpA41=Cyc_yyget_YY8(&(yyyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpA42,_tmpA41,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2487
goto _LL0;};case 315U: _LL271: _LL272:
# 2489
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA44=Cyc_yyget_Stmt_tok(&(yyyyvsp[1]).v);struct Cyc_List_List*_tmpA43=Cyc_yyget_YY8(&(yyyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpA44,_tmpA43,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 316U: _LL273: _LL274:
# 2493
 yyval=Cyc_YY8(0);
goto _LL0;case 317U: _LL275: _LL276:
# 2496
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2508 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));({struct Cyc_Absyn_Switch_clause*_tmpA48=({struct Cyc_Absyn_Switch_clause*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));({struct Cyc_Absyn_Pat*_tmpA47=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyyvsp[0]).l).first_line);_tmp5A4->pattern=_tmpA47;}),_tmp5A4->pat_vars=0,_tmp5A4->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA46=Cyc_yyget_Stmt_tok(&(yyyyvsp[2]).v);_tmp5A4->body=_tmpA46;}),_tmp5A4->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp5A4;});
# 2508
_tmp5A5->hd=_tmpA48;}),({
# 2510
struct Cyc_List_List*_tmpA45=Cyc_yyget_YY8(&(yyyyvsp[3]).v);_tmp5A5->tl=_tmpA45;});_tmp5A5;}));
goto _LL0;case 318U: _LL277: _LL278:
# 2513
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2512 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));({struct Cyc_Absyn_Switch_clause*_tmpA4C=({struct Cyc_Absyn_Switch_clause*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({struct Cyc_Absyn_Pat*_tmpA4B=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp5A6->pattern=_tmpA4B;}),_tmp5A6->pat_vars=0,_tmp5A6->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA4A=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyyvsp[2]).l).first_line);_tmp5A6->body=_tmpA4A;}),_tmp5A6->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp5A6;});
# 2512
_tmp5A7->hd=_tmpA4C;}),({
# 2514
struct Cyc_List_List*_tmpA49=Cyc_yyget_YY8(&(yyyyvsp[3]).v);_tmp5A7->tl=_tmpA49;});_tmp5A7;}));
goto _LL0;case 319U: _LL279: _LL27A:
# 2517
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2516 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));({struct Cyc_Absyn_Switch_clause*_tmpA50=({struct Cyc_Absyn_Switch_clause*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));({struct Cyc_Absyn_Pat*_tmpA4F=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp5A8->pattern=_tmpA4F;}),_tmp5A8->pat_vars=0,_tmp5A8->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpA4E=Cyc_yyget_Stmt_tok(&(yyyyvsp[3]).v);_tmp5A8->body=_tmpA4E;}),_tmp5A8->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp5A8;});_tmp5A9->hd=_tmpA50;}),({struct Cyc_List_List*_tmpA4D=Cyc_yyget_YY8(&(yyyyvsp[4]).v);_tmp5A9->tl=_tmpA4D;});_tmp5A9;}));
goto _LL0;case 320U: _LL27B: _LL27C:
# 2519
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 2518 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));({struct Cyc_Absyn_Switch_clause*_tmpA55=({struct Cyc_Absyn_Switch_clause*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));({struct Cyc_Absyn_Pat*_tmpA54=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp5AA->pattern=_tmpA54;}),_tmp5AA->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA53=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);_tmp5AA->where_clause=_tmpA53;}),({
struct Cyc_Absyn_Stmt*_tmpA52=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyyvsp[4]).l).first_line);_tmp5AA->body=_tmpA52;}),_tmp5AA->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp5AA;});
# 2518
_tmp5AB->hd=_tmpA55;}),({
# 2520
struct Cyc_List_List*_tmpA51=Cyc_yyget_YY8(&(yyyyvsp[5]).v);_tmp5AB->tl=_tmpA51;});_tmp5AB;}));
goto _LL0;case 321U: _LL27D: _LL27E:
# 2523
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2522 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));({struct Cyc_Absyn_Switch_clause*_tmpA5A=({struct Cyc_Absyn_Switch_clause*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));({struct Cyc_Absyn_Pat*_tmpA59=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp5AC->pattern=_tmpA59;}),_tmp5AC->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA58=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);_tmp5AC->where_clause=_tmpA58;}),({struct Cyc_Absyn_Stmt*_tmpA57=Cyc_yyget_Stmt_tok(&(yyyyvsp[5]).v);_tmp5AC->body=_tmpA57;}),_tmp5AC->loc=(unsigned)((yyyyvsp[0]).l).first_line;_tmp5AC;});_tmp5AD->hd=_tmpA5A;}),({struct Cyc_List_List*_tmpA56=Cyc_yyget_YY8(&(yyyyvsp[6]).v);_tmp5AD->tl=_tmpA56;});_tmp5AD;}));
goto _LL0;case 322U: _LL27F: _LL280:
# 2525
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2529 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA5C=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA5B=Cyc_yyget_Stmt_tok(&(yyyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpA5C,_tmpA5B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 323U: _LL281: _LL282:
# 2532
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA5E=Cyc_yyget_Stmt_tok(&(yyyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA5D=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpA5E,_tmpA5D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 324U: _LL283: _LL284:
# 2536
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA62=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA61=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA60=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA5F=
Cyc_yyget_Stmt_tok(&(yyyyvsp[5]).v);
# 2537
Cyc_Absyn_for_stmt(_tmpA62,_tmpA61,_tmpA60,_tmpA5F,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2539
goto _LL0;case 325U: _LL285: _LL286:
# 2541
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2540 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA66=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA65=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA64=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA63=
Cyc_yyget_Stmt_tok(&(yyyyvsp[6]).v);
# 2540
Cyc_Absyn_for_stmt(_tmpA66,_tmpA65,_tmpA64,_tmpA63,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2542
goto _LL0;case 326U: _LL287: _LL288:
# 2544
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2543 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA6A=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA69=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpA68=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA67=
Cyc_yyget_Stmt_tok(&(yyyyvsp[6]).v);
# 2543
Cyc_Absyn_for_stmt(_tmpA6A,_tmpA69,_tmpA68,_tmpA67,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2545
goto _LL0;case 327U: _LL289: _LL28A:
# 2547
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);
# 2546 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA6D=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpA6C=Cyc_yyget_Exp_tok(&(yyyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpA6B=
Cyc_yyget_Stmt_tok(&(yyyyvsp[7]).v);
# 2546
Cyc_Absyn_for_stmt(_tmpA6E,_tmpA6D,_tmpA6C,_tmpA6B,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2548
goto _LL0;case 328U: _LL28B: _LL28C:
# 2550
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 2549 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA72=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA71=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA70=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA6F=
Cyc_yyget_Stmt_tok(&(yyyyvsp[6]).v);
# 2549
Cyc_Absyn_for_stmt(_tmpA72,_tmpA71,_tmpA70,_tmpA6F,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2551
goto _LL0;case 329U: _LL28D: _LL28E:
# 2553
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);
# 2552 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA76=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA75=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA74=Cyc_yyget_Exp_tok(&(yyyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpA73=
Cyc_yyget_Stmt_tok(&(yyyyvsp[7]).v);
# 2552
Cyc_Absyn_for_stmt(_tmpA76,_tmpA75,_tmpA74,_tmpA73,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2554
goto _LL0;case 330U: _LL28F: _LL290:
# 2556
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);
# 2555 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA79=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpA78=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA77=
Cyc_yyget_Stmt_tok(&(yyyyvsp[7]).v);
# 2555
Cyc_Absyn_for_stmt(_tmpA7A,_tmpA79,_tmpA78,_tmpA77,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2557
goto _LL0;case 331U: _LL291: _LL292:
# 2559
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,8);
# 2558 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA7D=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpA7C=Cyc_yyget_Exp_tok(&(yyyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpA7B=
Cyc_yyget_Stmt_tok(&(yyyyvsp[8]).v);
# 2558
Cyc_Absyn_for_stmt(_tmpA7E,_tmpA7D,_tmpA7C,_tmpA7B,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2560
goto _LL0;case 332U: _LL293: _LL294:
# 2562
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
# 2561 "parse.y"
struct Cyc_List_List*_tmp5AE=Cyc_yyget_YY16(&(yyyyvsp[2]).v);
struct Cyc_Absyn_Stmt*_tmp5AF=({struct Cyc_Absyn_Exp*_tmpA82=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA80=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA7F=
Cyc_yyget_Stmt_tok(&(yyyyvsp[5]).v);
# 2562
Cyc_Absyn_for_stmt(_tmpA82,_tmpA81,_tmpA80,_tmpA7F,(unsigned)((yyyyvsp[0]).l).first_line);});
# 2564
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5AE,_tmp5AF));
# 2566
goto _LL0;};case 333U: _LL295: _LL296:
# 2568
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);{
# 2567 "parse.y"
struct Cyc_List_List*_tmp5B0=Cyc_yyget_YY16(&(yyyyvsp[2]).v);
struct Cyc_Absyn_Stmt*_tmp5B1=({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA85=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpA84=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA83=
Cyc_yyget_Stmt_tok(&(yyyyvsp[6]).v);
# 2568
Cyc_Absyn_for_stmt(_tmpA86,_tmpA85,_tmpA84,_tmpA83,(unsigned)((yyyyvsp[0]).l).first_line);});
# 2570
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5B0,_tmp5B1));
# 2572
goto _LL0;};case 334U: _LL297: _LL298:
# 2574
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);{
# 2573 "parse.y"
struct Cyc_List_List*_tmp5B2=Cyc_yyget_YY16(&(yyyyvsp[2]).v);
struct Cyc_Absyn_Stmt*_tmp5B3=({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA89=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA88=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA87=
Cyc_yyget_Stmt_tok(&(yyyyvsp[6]).v);
# 2574
Cyc_Absyn_for_stmt(_tmpA8A,_tmpA89,_tmpA88,_tmpA87,(unsigned)((yyyyvsp[0]).l).first_line);});
# 2576
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5B2,_tmp5B3));
# 2578
goto _LL0;};case 335U: _LL299: _LL29A:
# 2580
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,7);{
# 2579 "parse.y"
struct Cyc_List_List*_tmp5B4=Cyc_yyget_YY16(&(yyyyvsp[2]).v);
struct Cyc_Absyn_Stmt*_tmp5B5=({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA8D=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpA8C=Cyc_yyget_Exp_tok(&(yyyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpA8B=
Cyc_yyget_Stmt_tok(&(yyyyvsp[7]).v);
# 2580
Cyc_Absyn_for_stmt(_tmpA8E,_tmpA8D,_tmpA8C,_tmpA8B,(unsigned)((yyyyvsp[0]).l).first_line);});
# 2582
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5B4,_tmp5B5));
# 2584
goto _LL0;};case 336U: _LL29B: _LL29C:
# 2586
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2589 "parse.y"
yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpA90=({struct _fat_ptr*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));({struct _fat_ptr _tmpA8F=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);*_tmp5B6=_tmpA8F;});_tmp5B6;});Cyc_Absyn_goto_stmt(_tmpA90,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 337U: _LL29D: _LL29E:
# 2592
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2590 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 338U: _LL29F: _LL2A0:
# 2593
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2591 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 339U: _LL2A1: _LL2A2:
# 2594
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2592 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 340U: _LL2A3: _LL2A4:
# 2595
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2593 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA91=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpA91,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 341U: _LL2A5: _LL2A6:
# 2596
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2595 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 342U: _LL2A7: _LL2A8:
# 2598
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2596 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 343U: _LL2A9: _LL2AA:
# 2599
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2598 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA92=Cyc_yyget_YY4(&(yyyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpA92,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2601
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2607 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2610
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2610 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2613
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2612 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA95=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA94=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA93=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpA95,_tmpA94,_tmpA93,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2615
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2615 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2618
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2617 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA97=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA96=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpA97,_tmpA96,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2620
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2620 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2623
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2622 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA99=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA98=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpA99,_tmpA98,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2625
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2625 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2628
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2627 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA9A=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpA9B,_tmpA9A,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2630
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2630 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2633
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2632 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA9C=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpA9D,_tmpA9C,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2635
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2635 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2638
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2637 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA9F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA9E=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpA9F,_tmpA9E,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2640
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2640 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2643
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2642 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpAA1,_tmpAA0,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2645
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2644 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAA2=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpAA3,_tmpAA2,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2647
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2647 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2650
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2649 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAA5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAA4=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpAA5,_tmpAA4,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2652
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2651 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAA7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAA6=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpAA7,_tmpAA6,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2654
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2653 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAA8=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpAA9,_tmpAA8,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2656
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2655 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAAA=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpAAB,_tmpAAA,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2658
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2658 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2661
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2660 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAAC=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpAAD,_tmpAAC,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2663
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2662 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpAAF,_tmpAAE,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2665
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2665 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2668
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2667 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAB0=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpAB1,_tmpAB0,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2670
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2669 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAB2=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpAB3,_tmpAB2,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2672
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2672 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2675
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2674 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAB4=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpAB5,_tmpAB4,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2677
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2676 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpAB7,_tmpAB6,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2679
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2678 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpAB9,_tmpAB8,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2681
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2681 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2684
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2683 "parse.y"
void*_tmp5B7=({struct _tuple8*_tmpABA=Cyc_yyget_YY37(&(yyyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpABA,(unsigned)((yyyyvsp[1]).l).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpABC=_tmp5B7;struct Cyc_Absyn_Exp*_tmpABB=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpABC,_tmpABB,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyyvsp[0]).l).first_line);})));
# 2686
goto _LL0;};case 377U: _LL2ED: _LL2EE:
# 2688
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2692
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2692 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v)));
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2695
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2694 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpABE=Cyc_yyget_YY6(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpABD=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpABE,_tmpABD,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2697
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2696 "parse.y"
void*_tmp5B8=({struct _tuple8*_tmpABF=Cyc_yyget_YY37(&(yyyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpABF,(unsigned)((yyyyvsp[2]).l).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(_tmp5B8,(unsigned)((yyyyvsp[0]).l).first_line)));
# 2699
goto _LL0;};case 381U: _LL2F5: _LL2F6:
# 2701
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2700 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpAC0,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 382U: _LL2F7: _LL2F8:
# 2703
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 2702 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpAC2=(*Cyc_yyget_YY37(&(yyyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpAC1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpAC2,_tmpAC1,(unsigned)((yyyyvsp[0]).l).first_line);})));
goto _LL0;case 383U: _LL2F9: _LL2FA:
# 2705
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2707 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 384U: _LL2FB: _LL2FC:
# 2710
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2715 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2718
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2720 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 386U: _LL2FF: _LL300:
# 2723
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2722 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v)));
goto _LL0;case 387U: _LL301: _LL302:
# 2725
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 2724 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);
{void*_tmp5B9=e->r;void*_tmp5BA=_tmp5B9;struct _fat_ptr _tmp5CE;int _tmp5CD;enum Cyc_Absyn_Sign _tmp5CC;int _tmp5CB;enum Cyc_Absyn_Sign _tmp5CA;short _tmp5C9;enum Cyc_Absyn_Sign _tmp5C8;char _tmp5C7;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).LongLong_c).tag){case 2U: _LL464: _tmp5C8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Char_c).val).f1;_tmp5C7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Char_c).val).f2;_LL465:
# 2728
 yyval=Cyc_YY9(({void*_tmpAC3=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=11U,_tmp5BB->f1=_tmp5C7;_tmp5BB;});Cyc_Absyn_new_pat(_tmpAC3,e->loc);}));goto _LL463;case 4U: _LL466: _tmp5CA=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Short_c).val).f1;_tmp5C9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Short_c).val).f2;_LL467:
# 2730
 yyval=Cyc_YY9(({void*_tmpAC4=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->tag=10U,_tmp5BC->f1=_tmp5CA,_tmp5BC->f2=(int)_tmp5C9;_tmp5BC;});Cyc_Absyn_new_pat(_tmpAC4,e->loc);}));goto _LL463;case 5U: _LL468: _tmp5CC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Int_c).val).f1;_tmp5CB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Int_c).val).f2;_LL469:
# 2732
 yyval=Cyc_YY9(({void*_tmpAC5=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=10U,_tmp5BD->f1=_tmp5CC,_tmp5BD->f2=_tmp5CB;_tmp5BD;});Cyc_Absyn_new_pat(_tmpAC5,e->loc);}));goto _LL463;case 7U: _LL46A: _tmp5CE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Float_c).val).f1;_tmp5CD=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5BA)->f1).Float_c).val).f2;_LL46B:
# 2734
 yyval=Cyc_YY9(({void*_tmpAC6=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->tag=12U,_tmp5BE->f1=_tmp5CE,_tmp5BE->f2=_tmp5CD;_tmp5BE;});Cyc_Absyn_new_pat(_tmpAC6,e->loc);}));goto _LL463;case 1U: _LL46C: _LL46D:
# 2736
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL463;case 8U: _LL46E: _LL46F:
# 2738
({void*_tmp5BF=0U;({unsigned _tmpAC8=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmpAC7=({const char*_tmp5C0="strings cannot occur within patterns";_tag_fat(_tmp5C0,sizeof(char),37U);});Cyc_Warn_err(_tmpAC8,_tmpAC7,_tag_fat(_tmp5BF,sizeof(void*),0U));});});goto _LL463;case 9U: _LL470: _LL471:
# 2740
({void*_tmp5C1=0U;({unsigned _tmpACA=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmpAC9=({const char*_tmp5C2="strings cannot occur within patterns";_tag_fat(_tmp5C2,sizeof(char),37U);});Cyc_Warn_err(_tmpACA,_tmpAC9,_tag_fat(_tmp5C1,sizeof(void*),0U));});});goto _LL463;case 6U: _LL472: _LL473:
# 2742
({void*_tmp5C3=0U;({unsigned _tmpACC=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmpACB=({const char*_tmp5C4="long long's in patterns not yet implemented";_tag_fat(_tmp5C4,sizeof(char),44U);});Cyc_Warn_err(_tmpACC,_tmpACB,_tag_fat(_tmp5C3,sizeof(void*),0U));});});goto _LL463;default: goto _LL474;}else{_LL474: _LL475:
# 2744
({void*_tmp5C5=0U;({unsigned _tmpACE=(unsigned)((yyyyvsp[0]).l).first_line;struct _fat_ptr _tmpACD=({const char*_tmp5C6="bad constant in case";_tag_fat(_tmp5C6,sizeof(char),21U);});Cyc_Warn_err(_tmpACE,_tmpACD,_tag_fat(_tmp5C5,sizeof(void*),0U));});});}_LL463:;}
# 2747
goto _LL0;};case 388U: _LL303: _LL304:
# 2749
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2748 "parse.y"
yyval=Cyc_YY9(({void*_tmpAD0=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->tag=15U,({struct _tuple0*_tmpACF=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp5CF->f1=_tmpACF;});_tmp5CF;});Cyc_Absyn_new_pat(_tmpAD0,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 389U: _LL305: _LL306:
# 2751
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2750 "parse.y"
if(({struct _fat_ptr _tmpAD1=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyyvsp[1]).v);Cyc_strcmp(_tmpAD1,({const char*_tmp5D0="as";_tag_fat(_tmp5D0,sizeof(char),3U);}));})!= 0)
({void*_tmp5D1=0U;({unsigned _tmpAD3=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmpAD2=({const char*_tmp5D2="expecting `as'";_tag_fat(_tmp5D2,sizeof(char),15U);});Cyc_Warn_err(_tmpAD3,_tmpAD2,_tag_fat(_tmp5D1,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpADA=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpAD9=({unsigned _tmpAD8=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpAD7=({struct _tuple0*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpAD6=({struct _fat_ptr*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct _fat_ptr _tmpAD5=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp5D3=_tmpAD5;});_tmp5D3;});_tmp5D4->f2=_tmpAD6;});_tmp5D4;});Cyc_Absyn_new_vardecl(_tmpAD8,_tmpAD7,Cyc_Absyn_void_type,0);});_tmp5D5->f1=_tmpAD9;}),({
struct Cyc_Absyn_Pat*_tmpAD4=Cyc_yyget_YY9(&(yyyyvsp[2]).v);_tmp5D5->f2=_tmpAD4;});_tmp5D5;});
# 2752
Cyc_Absyn_new_pat(_tmpADA,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2755
goto _LL0;case 390U: _LL307: _LL308:
# 2757
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 2756 "parse.y"
if(({struct _fat_ptr _tmpADB=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_strcmp(_tmpADB,({const char*_tmp5D6="alias";_tag_fat(_tmp5D6,sizeof(char),6U);}));})!= 0)
({void*_tmp5D7=0U;({unsigned _tmpADD=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmpADC=({const char*_tmp5D8="expecting `alias'";_tag_fat(_tmp5D8,sizeof(char),18U);});Cyc_Warn_err(_tmpADD,_tmpADC,_tag_fat(_tmp5D7,sizeof(void*),0U));});});{
int _tmp5D9=((yyyyvsp[0]).l).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));({struct _fat_ptr*_tmpAE0=({struct _fat_ptr*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct _fat_ptr _tmpADF=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp5DD=_tmpADF;});_tmp5DD;});_tmp5DF->name=_tmpAE0;}),_tmp5DF->identity=- 1,({void*_tmpADE=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->tag=0U,_tmp5DE->f1=& Cyc_Tcutil_rk;_tmp5DE;});_tmp5DF->kind=_tmpADE;});_tmp5DF;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpAE5=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpAE4=({struct _tuple0*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpAE2=({struct _fat_ptr*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct _fat_ptr _tmpAE1=Cyc_yyget_String_tok(&(yyyyvsp[5]).v);*_tmp5DB=_tmpAE1;});_tmp5DB;});_tmp5DC->f2=_tmpAE2;});_tmp5DC;});Cyc_Absyn_new_vardecl(_tmpAE5,_tmpAE4,({
struct _tuple8*_tmpAE3=Cyc_yyget_YY37(&(yyyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpAE3,(unsigned)((yyyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpAE6=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->tag=2U,_tmp5DA->f1=tv,_tmp5DA->f2=vd;_tmp5DA;});Cyc_Absyn_new_pat(_tmpAE6,(unsigned)_tmp5D9);}));
# 2764
goto _LL0;};case 391U: _LL309: _LL30A:
# 2766
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 2765 "parse.y"
if(({struct _fat_ptr _tmpAE7=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_strcmp(_tmpAE7,({const char*_tmp5E0="alias";_tag_fat(_tmp5E0,sizeof(char),6U);}));})!= 0)
({void*_tmp5E1=0U;({unsigned _tmpAE9=(unsigned)((yyyyvsp[1]).l).first_line;struct _fat_ptr _tmpAE8=({const char*_tmp5E2="expecting `alias'";_tag_fat(_tmp5E2,sizeof(char),18U);});Cyc_Warn_err(_tmpAE9,_tmpAE8,_tag_fat(_tmp5E1,sizeof(void*),0U));});});{
int _tmp5E3=((yyyyvsp[0]).l).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));({struct _fat_ptr*_tmpAEC=({struct _fat_ptr*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));({struct _fat_ptr _tmpAEB=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp5E7=_tmpAEB;});_tmp5E7;});_tmp5E9->name=_tmpAEC;}),_tmp5E9->identity=- 1,({void*_tmpAEA=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=0U,_tmp5E8->f1=& Cyc_Tcutil_rk;_tmp5E8;});_tmp5E9->kind=_tmpAEA;});_tmp5E9;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpAF1=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpAF0=({struct _tuple0*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpAEE=({struct _fat_ptr*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));({struct _fat_ptr _tmpAED=Cyc_yyget_String_tok(&(yyyyvsp[5]).v);*_tmp5E5=_tmpAED;});_tmp5E5;});_tmp5E6->f2=_tmpAEE;});_tmp5E6;});Cyc_Absyn_new_vardecl(_tmpAF1,_tmpAF0,({
struct _tuple8*_tmpAEF=Cyc_yyget_YY37(&(yyyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpAEF,(unsigned)((yyyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpAF2=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=2U,_tmp5E4->f1=tv,_tmp5E4->f2=vd;_tmp5E4;});Cyc_Absyn_new_pat(_tmpAF2,(unsigned)_tmp5E3);}));
# 2773
goto _LL0;};case 392U: _LL30B: _LL30C:
# 2775
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2774 "parse.y"
struct _tuple22 _tmp5EA=*Cyc_yyget_YY10(&(yyyyvsp[2]).v);struct _tuple22 _tmp5EB=_tmp5EA;struct Cyc_List_List*_tmp5EE;int _tmp5ED;_LL477: _tmp5EE=_tmp5EB.f1;_tmp5ED=_tmp5EB.f2;_LL478:;
yyval=Cyc_YY9(({void*_tmpAF3=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->tag=5U,_tmp5EC->f1=_tmp5EE,_tmp5EC->f2=_tmp5ED;_tmp5EC;});Cyc_Absyn_new_pat(_tmpAF3,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2777
goto _LL0;};case 393U: _LL30D: _LL30E:
# 2779
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2778 "parse.y"
struct _tuple22 _tmp5EF=*Cyc_yyget_YY10(&(yyyyvsp[2]).v);struct _tuple22 _tmp5F0=_tmp5EF;struct Cyc_List_List*_tmp5F3;int _tmp5F2;_LL47A: _tmp5F3=_tmp5F0.f1;_tmp5F2=_tmp5F0.f2;_LL47B:;
yyval=Cyc_YY9(({void*_tmpAF5=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->tag=16U,({struct _tuple0*_tmpAF4=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp5F1->f1=_tmpAF4;}),_tmp5F1->f2=_tmp5F3,_tmp5F1->f3=_tmp5F2;_tmp5F1;});Cyc_Absyn_new_pat(_tmpAF5,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2781
goto _LL0;};case 394U: _LL30F: _LL310:
# 2783
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);{
# 2782 "parse.y"
struct _tuple22 _tmp5F4=*Cyc_yyget_YY14(&(yyyyvsp[3]).v);struct _tuple22 _tmp5F5=_tmp5F4;struct Cyc_List_List*_tmp5FA;int _tmp5F9;_LL47D: _tmp5FA=_tmp5F5.f1;_tmp5F9=_tmp5F5.f2;_LL47E:;{
struct Cyc_List_List*_tmp5F6=({unsigned _tmpAF6=(unsigned)((yyyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAF6,Cyc_yyget_YY40(&(yyyyvsp[2]).v));});
yyval=Cyc_YY9(({void*_tmpAF9=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpAF8=({union Cyc_Absyn_AggrInfo*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({union Cyc_Absyn_AggrInfo _tmpAF7=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v),0);*_tmp5F7=_tmpAF7;});_tmp5F7;});_tmp5F8->f1=_tmpAF8;}),_tmp5F8->f2=_tmp5F6,_tmp5F8->f3=_tmp5FA,_tmp5F8->f4=_tmp5F9;_tmp5F8;});Cyc_Absyn_new_pat(_tmpAF9,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2787
goto _LL0;};};case 395U: _LL311: _LL312:
# 2789
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2788 "parse.y"
struct _tuple22 _tmp5FB=*Cyc_yyget_YY14(&(yyyyvsp[2]).v);struct _tuple22 _tmp5FC=_tmp5FB;struct Cyc_List_List*_tmp600;int _tmp5FF;_LL480: _tmp600=_tmp5FC.f1;_tmp5FF=_tmp5FC.f2;_LL481:;{
struct Cyc_List_List*_tmp5FD=({unsigned _tmpAFA=(unsigned)((yyyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAFA,Cyc_yyget_YY40(&(yyyyvsp[1]).v));});
yyval=Cyc_YY9(({void*_tmpAFB=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->tag=7U,_tmp5FE->f1=0,_tmp5FE->f2=_tmp5FD,_tmp5FE->f3=_tmp600,_tmp5FE->f4=_tmp5FF;_tmp5FE;});Cyc_Absyn_new_pat(_tmpAFB,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2792
goto _LL0;};};case 396U: _LL313: _LL314:
# 2794
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2793 "parse.y"
yyval=Cyc_YY9(({void*_tmpAFD=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=6U,({struct Cyc_Absyn_Pat*_tmpAFC=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp601->f1=_tmpAFC;});_tmp601;});Cyc_Absyn_new_pat(_tmpAFD,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 397U: _LL315: _LL316:
# 2796
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2795 "parse.y"
yyval=Cyc_YY9(({void*_tmpB01=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->tag=6U,({struct Cyc_Absyn_Pat*_tmpB00=({void*_tmpAFF=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->tag=6U,({struct Cyc_Absyn_Pat*_tmpAFE=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp602->f1=_tmpAFE;});_tmp602;});Cyc_Absyn_new_pat(_tmpAFF,(unsigned)((yyyyvsp[0]).l).first_line);});_tmp603->f1=_tmpB00;});_tmp603;});Cyc_Absyn_new_pat(_tmpB01,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 398U: _LL317: _LL318:
# 2798
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2797 "parse.y"
yyval=Cyc_YY9(({void*_tmpB08=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB07=({unsigned _tmpB06=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpB05=({struct _tuple0*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB04=({struct _fat_ptr*_tmp604=_cycalloc(sizeof(*_tmp604));({struct _fat_ptr _tmpB03=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);*_tmp604=_tmpB03;});_tmp604;});_tmp605->f2=_tmpB04;});_tmp605;});Cyc_Absyn_new_vardecl(_tmpB06,_tmpB05,Cyc_Absyn_void_type,0);});_tmp606->f1=_tmpB07;}),({
# 2799
struct Cyc_Absyn_Pat*_tmpB02=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyyvsp[1]).l).first_line);_tmp606->f2=_tmpB02;});_tmp606;});
# 2797
Cyc_Absyn_new_pat(_tmpB08,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2801
goto _LL0;case 399U: _LL319: _LL31A:
# 2803
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 2802 "parse.y"
if(({struct _fat_ptr _tmpB09=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyyvsp[2]).v);Cyc_strcmp(_tmpB09,({const char*_tmp607="as";_tag_fat(_tmp607,sizeof(char),3U);}));})!= 0)
({void*_tmp608=0U;({unsigned _tmpB0B=(unsigned)((yyyyvsp[2]).l).first_line;struct _fat_ptr _tmpB0A=({const char*_tmp609="expecting `as'";_tag_fat(_tmp609,sizeof(char),15U);});Cyc_Warn_err(_tmpB0B,_tmpB0A,_tag_fat(_tmp608,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB12=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB11=({unsigned _tmpB10=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpB0F=({struct _tuple0*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB0E=({struct _fat_ptr*_tmp60A=_cycalloc(sizeof(*_tmp60A));({struct _fat_ptr _tmpB0D=Cyc_yyget_String_tok(&(yyyyvsp[1]).v);*_tmp60A=_tmpB0D;});_tmp60A;});_tmp60B->f2=_tmpB0E;});_tmp60B;});Cyc_Absyn_new_vardecl(_tmpB10,_tmpB0F,Cyc_Absyn_void_type,0);});_tmp60C->f1=_tmpB11;}),({
# 2806
struct Cyc_Absyn_Pat*_tmpB0C=Cyc_yyget_YY9(&(yyyyvsp[3]).v);_tmp60C->f2=_tmpB0C;});_tmp60C;});
# 2804
Cyc_Absyn_new_pat(_tmpB12,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2808
goto _LL0;case 400U: _LL31B: _LL31C:
# 2810
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2809 "parse.y"
void*_tmp60D=({struct _fat_ptr _tmpB13=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);Cyc_Parse_id2type(_tmpB13,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
yyval=Cyc_YY9(({void*_tmpB1A=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB19=Cyc_Parse_typ2tvar((unsigned)((yyyyvsp[2]).l).first_line,_tmp60D);_tmp610->f1=_tmpB19;}),({
struct Cyc_Absyn_Vardecl*_tmpB18=({unsigned _tmpB17=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpB16=({struct _tuple0*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB15=({struct _fat_ptr*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct _fat_ptr _tmpB14=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp60E=_tmpB14;});_tmp60E;});_tmp60F->f2=_tmpB15;});_tmp60F;});Cyc_Absyn_new_vardecl(_tmpB17,_tmpB16,
Cyc_Absyn_tag_type(_tmp60D),0);});
# 2811
_tmp610->f2=_tmpB18;});_tmp610;});
# 2810
Cyc_Absyn_new_pat(_tmpB1A,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2814
goto _LL0;};case 401U: _LL31D: _LL31E:
# 2816
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2815 "parse.y"
struct Cyc_Absyn_Tvar*_tmp611=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(({void*_tmpB20=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->tag=4U,_tmp614->f1=_tmp611,({
struct Cyc_Absyn_Vardecl*_tmpB1F=({unsigned _tmpB1E=(unsigned)((yyyyvsp[0]).l).first_line;struct _tuple0*_tmpB1D=({struct _tuple0*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB1C=({struct _fat_ptr*_tmp612=_cycalloc(sizeof(*_tmp612));({struct _fat_ptr _tmpB1B=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp612=_tmpB1B;});_tmp612;});_tmp613->f2=_tmpB1C;});_tmp613;});Cyc_Absyn_new_vardecl(_tmpB1E,_tmpB1D,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(_tmp611)),0);});
# 2817
_tmp614->f2=_tmpB1F;});_tmp614;});
# 2816
Cyc_Absyn_new_pat(_tmpB20,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2820
goto _LL0;};case 402U: _LL31F: _LL320:
# 2822
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY10(({struct _tuple22*_tmp615=_cycalloc(sizeof(*_tmp615));({struct Cyc_List_List*_tmpB21=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyyvsp[0]).v));_tmp615->f1=_tmpB21;}),_tmp615->f2=0;_tmp615;}));
goto _LL0;case 403U: _LL321: _LL322:
# 2826
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2824 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp616=_cycalloc(sizeof(*_tmp616));({struct Cyc_List_List*_tmpB22=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyyvsp[0]).v));_tmp616->f1=_tmpB22;}),_tmp616->f2=1;_tmp616;}));
goto _LL0;case 404U: _LL323: _LL324:
# 2827
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2825 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=0,_tmp617->f2=1;_tmp617;}));
goto _LL0;case 405U: _LL325: _LL326:
# 2828
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2830 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp618=_cycalloc(sizeof(*_tmp618));({struct Cyc_Absyn_Pat*_tmpB23=Cyc_yyget_YY9(&(yyyyvsp[0]).v);_tmp618->hd=_tmpB23;}),_tmp618->tl=0;_tmp618;}));
goto _LL0;case 406U: _LL327: _LL328:
# 2833
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2832 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));({struct Cyc_Absyn_Pat*_tmpB25=Cyc_yyget_YY9(&(yyyyvsp[2]).v);_tmp619->hd=_tmpB25;}),({struct Cyc_List_List*_tmpB24=Cyc_yyget_YY11(&(yyyyvsp[0]).v);_tmp619->tl=_tmpB24;});_tmp619;}));
goto _LL0;case 407U: _LL329: _LL32A:
# 2835
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2837 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->f1=0,({struct Cyc_Absyn_Pat*_tmpB26=Cyc_yyget_YY9(&(yyyyvsp[0]).v);_tmp61A->f2=_tmpB26;});_tmp61A;}));
goto _LL0;case 408U: _LL32B: _LL32C:
# 2840
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2839 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp61B=_cycalloc(sizeof(*_tmp61B));({struct Cyc_List_List*_tmpB28=Cyc_yyget_YY41(&(yyyyvsp[0]).v);_tmp61B->f1=_tmpB28;}),({struct Cyc_Absyn_Pat*_tmpB27=Cyc_yyget_YY9(&(yyyyvsp[1]).v);_tmp61B->f2=_tmpB27;});_tmp61B;}));
goto _LL0;case 409U: _LL32D: _LL32E:
# 2842
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2842 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp61C=_cycalloc(sizeof(*_tmp61C));({struct Cyc_List_List*_tmpB29=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyyvsp[0]).v));_tmp61C->f1=_tmpB29;}),_tmp61C->f2=0;_tmp61C;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2845
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2843 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp61D=_cycalloc(sizeof(*_tmp61D));({struct Cyc_List_List*_tmpB2A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyyvsp[0]).v));_tmp61D->f1=_tmpB2A;}),_tmp61D->f2=1;_tmp61D;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2846
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2844 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->f1=0,_tmp61E->f2=1;_tmp61E;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2847
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2849 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));({struct _tuple23*_tmpB2B=Cyc_yyget_YY12(&(yyyyvsp[0]).v);_tmp61F->hd=_tmpB2B;}),_tmp61F->tl=0;_tmp61F;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2852
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2851 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp620=_cycalloc(sizeof(*_tmp620));({struct _tuple23*_tmpB2D=Cyc_yyget_YY12(&(yyyyvsp[2]).v);_tmp620->hd=_tmpB2D;}),({struct Cyc_List_List*_tmpB2C=Cyc_yyget_YY13(&(yyyyvsp[0]).v);_tmp620->tl=_tmpB2C;});_tmp620;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2854
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2857 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 415U: _LL339: _LL33A:
# 2860
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2859 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB2F=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB2E=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB2F,_tmpB2E,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2862
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2864 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 417U: _LL33D: _LL33E:
# 2867
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2866 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB32=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB31=Cyc_yyget_YY7(&(yyyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB30=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpB32,_tmpB31,_tmpB30,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2869
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2868 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB34=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB33=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpB34,_tmpB33,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 419U: _LL341: _LL342:
# 2871
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY7(0);
goto _LL0;case 420U: _LL343: _LL344:
# 2875
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2873 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->v=(void*)Cyc_Absyn_Times;_tmp621;}));
goto _LL0;case 421U: _LL345: _LL346:
# 2876
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2874 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->v=(void*)Cyc_Absyn_Div;_tmp622;}));
goto _LL0;case 422U: _LL347: _LL348:
# 2877
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2875 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->v=(void*)Cyc_Absyn_Mod;_tmp623;}));
goto _LL0;case 423U: _LL349: _LL34A:
# 2878
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2876 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->v=(void*)Cyc_Absyn_Plus;_tmp624;}));
goto _LL0;case 424U: _LL34B: _LL34C:
# 2879
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2877 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->v=(void*)Cyc_Absyn_Minus;_tmp625;}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2880
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2878 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->v=(void*)Cyc_Absyn_Bitlshift;_tmp626;}));
goto _LL0;case 426U: _LL34F: _LL350:
# 2881
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2879 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->v=(void*)Cyc_Absyn_Bitlrshift;_tmp627;}));
goto _LL0;case 427U: _LL351: _LL352:
# 2882
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2880 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->v=(void*)Cyc_Absyn_Bitand;_tmp628;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2883
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2881 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->v=(void*)Cyc_Absyn_Bitxor;_tmp629;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2884
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2882 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->v=(void*)Cyc_Absyn_Bitor;_tmp62A;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2885
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 2887 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 431U: _LL359: _LL35A:
# 2890
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2889 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB37=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB36=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB35=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB37,_tmpB36,_tmpB35,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2892
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2892 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB38=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpB38,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2895
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2895 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB39=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB39,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2898
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2897 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB3A=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB3A,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 435U: _LL361: _LL362:
# 2900
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2899 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB3C=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB3B=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB3C,_tmpB3B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 436U: _LL363: _LL364:
# 2902
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 2901 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB3E=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB3D=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB3E,_tmpB3D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 437U: _LL365: _LL366:
# 2904
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 438U: _LL367: _LL368:
# 2908
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 439U: _LL369: _LL36A:
# 2912
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2911 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB40=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB3F=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB40,_tmpB3F,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2914
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 441U: _LL36D: _LL36E:
# 2918
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2917 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB42=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB41=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpB42,_tmpB41,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2920
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 443U: _LL371: _LL372:
# 2924
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2923 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB44=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB43=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB44,_tmpB43,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 444U: _LL373: _LL374:
# 2926
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2930
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2929 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB46=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB45=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB46,_tmpB45,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 446U: _LL377: _LL378:
# 2932
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 447U: _LL379: _LL37A:
# 2936
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2935 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB48=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB47=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB48,_tmpB47,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 448U: _LL37B: _LL37C:
# 2938
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 449U: _LL37D: _LL37E:
# 2942
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2941 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4A=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB49=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpB4A,_tmpB49,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 450U: _LL37F: _LL380:
# 2944
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2943 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4C=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB4B=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpB4C,_tmpB4B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 451U: _LL381: _LL382:
# 2946
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 452U: _LL383: _LL384:
# 2950
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2949 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4E=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB4D=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB4E,_tmpB4D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 453U: _LL385: _LL386:
# 2952
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2951 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB50=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB4F=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB50,_tmpB4F,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 454U: _LL387: _LL388:
# 2954
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2953 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB52=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB51=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB52,_tmpB51,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 455U: _LL389: _LL38A:
# 2956
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2955 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB54=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB53=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB54,_tmpB53,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 456U: _LL38B: _LL38C:
# 2958
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 457U: _LL38D: _LL38E:
# 2962
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2961 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB56=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB55=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB56,_tmpB55,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 458U: _LL38F: _LL390:
# 2964
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2963 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB58=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB57=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB58,_tmpB57,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 459U: _LL391: _LL392:
# 2966
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 460U: _LL393: _LL394:
# 2970
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2969 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5A=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB59=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB5A,_tmpB59,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2972
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2971 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5C=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB5B=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB5C,_tmpB5B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2974
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 463U: _LL399: _LL39A:
# 2978
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2977 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5E=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB5D=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB5E,_tmpB5D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 464U: _LL39B: _LL39C:
# 2980
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2979 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB60=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB5F=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB60,_tmpB5F,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2982
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 2981 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB62=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB61=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB62,_tmpB61,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2984
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2988
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 2987 "parse.y"
void*_tmp62B=({struct _tuple8*_tmpB63=Cyc_yyget_YY37(&(yyyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB63,(unsigned)((yyyyvsp[1]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB65=_tmp62B;struct Cyc_Absyn_Exp*_tmpB64=Cyc_yyget_Exp_tok(&(yyyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB65,_tmpB64,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 2990
goto _LL0;};case 468U: _LL3A3: _LL3A4:
# 2992
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=(yyyyvsp[0]).v;
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 2996
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2994 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB66=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpB66,Cyc_Absyn_PreInc,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 2997
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2995 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB67=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpB67,Cyc_Absyn_PreDec,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 2998
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2996 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB68=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpB68,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 2999
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2997 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB69=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpB69,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 473U: _LL3AD: _LL3AE:
# 3000
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2998 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6A=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpB6A,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 474U: _LL3AF: _LL3B0:
# 3001
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 2999 "parse.y"
yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpB6C=Cyc_yyget_YY6(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB6B=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB6C,_tmpB6B,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 475U: _LL3B1: _LL3B2:
# 3002
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 3001 "parse.y"
void*_tmp62C=({struct _tuple8*_tmpB6D=Cyc_yyget_YY37(&(yyyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB6D,(unsigned)((yyyyvsp[2]).l).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(_tmp62C,(unsigned)((yyyyvsp[0]).l).first_line));
# 3004
goto _LL0;};case 476U: _LL3B3: _LL3B4:
# 3006
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 3004 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6E=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB6E,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 3007
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);{
# 3006 "parse.y"
void*_tmp62D=({struct _tuple8*_tmpB6F=Cyc_yyget_YY37(&(yyyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB6F,(unsigned)((yyyyvsp[2]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB71=_tmp62D;struct Cyc_List_List*_tmpB70=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB71,_tmpB70,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 3009
goto _LL0;};case 478U: _LL3B7: _LL3B8:
# 3011
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 3011 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB73=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->tag=34U,(_tmp62E->f1).is_calloc=0,(_tmp62E->f1).rgn=0,(_tmp62E->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB72=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);(_tmp62E->f1).num_elts=_tmpB72;}),(_tmp62E->f1).fat_result=0,(_tmp62E->f1).inline_call=0;_tmp62E;});Cyc_Absyn_new_exp(_tmpB73,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 3013
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 3015
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 3014 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB76=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=34U,(_tmp62F->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpB75=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);(_tmp62F->f1).rgn=_tmpB75;}),(_tmp62F->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB74=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);(_tmp62F->f1).num_elts=_tmpB74;}),(_tmp62F->f1).fat_result=0,(_tmp62F->f1).inline_call=0;_tmp62F;});Cyc_Absyn_new_exp(_tmpB76,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 3016
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 3018
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 3017 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB79=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->tag=34U,(_tmp630->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpB78=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);(_tmp630->f1).rgn=_tmpB78;}),(_tmp630->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB77=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);(_tmp630->f1).num_elts=_tmpB77;}),(_tmp630->f1).fat_result=0,(_tmp630->f1).inline_call=1;_tmp630;});Cyc_Absyn_new_exp(_tmpB79,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 3019
goto _LL0;case 481U: _LL3BD: _LL3BE:
# 3021
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,8);{
# 3020 "parse.y"
void*_tmp631=({struct _tuple8*_tmpB7A=Cyc_yyget_YY37(&(yyyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpB7A,(unsigned)((yyyyvsp[6]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB7D=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->tag=34U,(_tmp633->f1).is_calloc=1,(_tmp633->f1).rgn=0,({void**_tmpB7C=({void**_tmp632=_cycalloc(sizeof(*_tmp632));*_tmp632=_tmp631;_tmp632;});(_tmp633->f1).elt_type=_tmpB7C;}),({struct Cyc_Absyn_Exp*_tmpB7B=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);(_tmp633->f1).num_elts=_tmpB7B;}),(_tmp633->f1).fat_result=0,(_tmp633->f1).inline_call=0;_tmp633;});Cyc_Absyn_new_exp(_tmpB7D,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 3023
goto _LL0;};case 482U: _LL3BF: _LL3C0:
# 3025
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,10);{
# 3025 "parse.y"
void*_tmp634=({struct _tuple8*_tmpB7E=Cyc_yyget_YY37(&(yyyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpB7E,(unsigned)((yyyyvsp[8]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB82=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->tag=34U,(_tmp636->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpB81=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);(_tmp636->f1).rgn=_tmpB81;}),({void**_tmpB80=({void**_tmp635=_cycalloc(sizeof(*_tmp635));*_tmp635=_tmp634;_tmp635;});(_tmp636->f1).elt_type=_tmpB80;}),({struct Cyc_Absyn_Exp*_tmpB7F=Cyc_yyget_Exp_tok(&(yyyyvsp[4]).v);(_tmp636->f1).num_elts=_tmpB7F;}),(_tmp636->f1).fat_result=0,(_tmp636->f1).inline_call=0;_tmp636;});Cyc_Absyn_new_exp(_tmpB82,(unsigned)((yyyyvsp[0]).l).first_line);}));
# 3028
goto _LL0;};case 483U: _LL3C1: _LL3C2:
# 3030
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 3029 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpB84=({struct Cyc_Absyn_Exp*_tmp637[1U];({struct Cyc_Absyn_Exp*_tmpB83=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp637[0]=_tmpB83;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp637,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpB84,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 484U: _LL3C3: _LL3C4:
# 3032
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 3031 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB88=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->tag=38U,({struct Cyc_Absyn_Exp*_tmpB87=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp639->f1=_tmpB87;}),({struct _fat_ptr*_tmpB86=({struct _fat_ptr*_tmp638=_cycalloc(sizeof(*_tmp638));({struct _fat_ptr _tmpB85=Cyc_yyget_String_tok(&(yyyyvsp[4]).v);*_tmp638=_tmpB85;});_tmp638;});_tmp639->f2=_tmpB86;});_tmp639;});Cyc_Absyn_new_exp(_tmpB88,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 485U: _LL3C5: _LL3C6:
# 3034
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 3033 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB8D=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tag=38U,({struct Cyc_Absyn_Exp*_tmpB8C=({struct Cyc_Absyn_Exp*_tmpB8B=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpB8B,(unsigned)((yyyyvsp[2]).l).first_line);});_tmp63B->f1=_tmpB8C;}),({struct _fat_ptr*_tmpB8A=({struct _fat_ptr*_tmp63A=_cycalloc(sizeof(*_tmp63A));({struct _fat_ptr _tmpB89=Cyc_yyget_String_tok(&(yyyyvsp[4]).v);*_tmp63A=_tmpB89;});_tmp63A;});_tmp63B->f2=_tmpB8A;});_tmp63B;});Cyc_Absyn_new_exp(_tmpB8D,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 3036
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);{
# 3035 "parse.y"
void*_tmp63C=({struct _tuple8*_tmpB8E=Cyc_yyget_YY37(&(yyyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB8E,(unsigned)((yyyyvsp[2]).l).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp63C,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;};case 487U: _LL3C9: _LL3CA:
# 3039
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 3038 "parse.y"
struct _tuple20 _tmp63D=Cyc_yyget_Asm_tok(&(yyyyvsp[0]).v);struct _tuple20 _tmp63E=_tmp63D;int _tmp640;struct _fat_ptr _tmp63F;_LL483: _tmp640=_tmp63E.f1;_tmp63F=_tmp63E.f2;_LL484:;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp640,_tmp63F,(unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;};case 488U: _LL3CB: _LL3CC:
# 3042
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 3040 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_yyget_Exp_tok(&(yyyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpB8F,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 489U: _LL3CD: _LL3CE:
# 3043
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 490U: _LL3CF: _LL3D0:
# 3047
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3045 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 3048
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3046 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 492U: _LL3D3: _LL3D4:
# 3049
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3051 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 493U: _LL3D5: _LL3D6:
# 3054
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 3053 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB91=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpB91,_tmpB90,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 494U: _LL3D7: _LL3D8:
# 3056
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3055 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpB92,0,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA:
# 3058
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 3057 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB94=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_List_List*_tmpB93=Cyc_yyget_YY4(&(yyyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpB94,_tmpB93,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC:
# 3060
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3059 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct _fat_ptr*_tmpB96=({struct _fat_ptr*_tmp641=_cycalloc(sizeof(*_tmp641));({struct _fat_ptr _tmpB95=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp641=_tmpB95;});_tmp641;});Cyc_Absyn_aggrmember_exp(_tmpB97,_tmpB96,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 497U: _LL3DD: _LL3DE:
# 3062
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3061 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct _fat_ptr*_tmpB99=({struct _fat_ptr*_tmp642=_cycalloc(sizeof(*_tmp642));({struct _fat_ptr _tmpB98=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp642=_tmpB98;});_tmp642;});Cyc_Absyn_aggrarrow_exp(_tmpB9A,_tmpB99,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 498U: _LL3DF: _LL3E0:
# 3064
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 3063 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpB9B,Cyc_Absyn_PostInc,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 499U: _LL3E1: _LL3E2:
# 3066
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 3065 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpB9C,Cyc_Absyn_PostDec,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 500U: _LL3E3: _LL3E4:
# 3068
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 3067 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB9E=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->tag=25U,({struct _tuple8*_tmpB9D=Cyc_yyget_YY37(&(yyyyvsp[1]).v);_tmp643->f1=_tmpB9D;}),_tmp643->f2=0;_tmp643;});Cyc_Absyn_new_exp(_tmpB9E,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 501U: _LL3E5: _LL3E6:
# 3070
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,5);
# 3069 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->tag=25U,({struct _tuple8*_tmpBA0=Cyc_yyget_YY37(&(yyyyvsp[1]).v);_tmp644->f1=_tmpBA0;}),({struct Cyc_List_List*_tmpB9F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyyvsp[4]).v));_tmp644->f2=_tmpB9F;});_tmp644;});Cyc_Absyn_new_exp(_tmpBA1,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 502U: _LL3E7: _LL3E8:
# 3072
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,6);
# 3071 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA4=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tag=25U,({struct _tuple8*_tmpBA3=Cyc_yyget_YY37(&(yyyyvsp[1]).v);_tmp645->f1=_tmpBA3;}),({struct Cyc_List_List*_tmpBA2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyyvsp[4]).v));_tmp645->f2=_tmpBA2;});_tmp645;});Cyc_Absyn_new_exp(_tmpBA4,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 503U: _LL3E9: _LL3EA:
# 3074
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3076 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp648=_cycalloc(sizeof(*_tmp648));({void*_tmpBA7=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->tag=0U,({struct _fat_ptr*_tmpBA6=({struct _fat_ptr*_tmp646=_cycalloc(sizeof(*_tmp646));({struct _fat_ptr _tmpBA5=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp646=_tmpBA5;});_tmp646;});_tmp647->f1=_tmpBA6;});_tmp647;});_tmp648->hd=_tmpBA7;}),_tmp648->tl=0;_tmp648;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 3079
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3079 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({void*_tmpBAA=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->tag=1U,({unsigned _tmpBA9=({unsigned _tmpBA8=(unsigned)((yyyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpBA8,Cyc_yyget_Int_tok(&(yyyyvsp[0]).v));});_tmp649->f1=_tmpBA9;});_tmp649;});_tmp64A->hd=_tmpBAA;}),_tmp64A->tl=0;_tmp64A;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 3082
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3081 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));({void*_tmpBAE=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->tag=0U,({struct _fat_ptr*_tmpBAD=({struct _fat_ptr*_tmp64B=_cycalloc(sizeof(*_tmp64B));({struct _fat_ptr _tmpBAC=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);*_tmp64B=_tmpBAC;});_tmp64B;});_tmp64C->f1=_tmpBAD;});_tmp64C;});_tmp64D->hd=_tmpBAE;}),({struct Cyc_List_List*_tmpBAB=Cyc_yyget_YY3(&(yyyyvsp[0]).v);_tmp64D->tl=_tmpBAB;});_tmp64D;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 3084
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3084 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp64F=_cycalloc(sizeof(*_tmp64F));({void*_tmpBB2=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->tag=1U,({unsigned _tmpBB1=({unsigned _tmpBB0=(unsigned)((yyyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpBB0,Cyc_yyget_Int_tok(&(yyyyvsp[2]).v));});_tmp64E->f1=_tmpBB1;});_tmp64E;});_tmp64F->hd=_tmpBB2;}),({struct Cyc_List_List*_tmpBAF=Cyc_yyget_YY3(&(yyyyvsp[0]).v);_tmp64F->tl=_tmpBAF;});_tmp64F;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 3087
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3090 "parse.y"
yyval=Cyc_Exp_tok(({struct _tuple0*_tmpBB3=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpBB3,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 3093
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 3092 "parse.y"
yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpBB4=Cyc_yyget_String_tok(&(yyyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpBB4,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 509U: _LL3F5: _LL3F6:
# 3095
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3094 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 510U: _LL3F7: _LL3F8:
# 3097
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3096 "parse.y"
yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpBB5=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpBB5,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 511U: _LL3F9: _LL3FA:
# 3099
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3098 "parse.y"
yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpBB6=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpBB6,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3101
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3100 "parse.y"
yyval=(yyyyvsp[1]).v;
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3103
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,1);
# 3105 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpBB7,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3108
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 3107 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);struct Cyc_List_List*_tmpBB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpBB9,_tmpBB8,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 515U: _LL401: _LL402:
# 3110
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,3);
# 3110 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBBA=Cyc_yyget_YY4(&(yyyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpBBA,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 516U: _LL403: _LL404:
# 3113
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 3113 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBBE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->tag=29U,({struct _tuple0*_tmpBBD=Cyc_yyget_QualId_tok(&(yyyyvsp[0]).v);_tmp650->f1=_tmpBBD;}),({struct Cyc_List_List*_tmpBBC=Cyc_yyget_YY40(&(yyyyvsp[2]).v);_tmp650->f2=_tmpBBC;}),({struct Cyc_List_List*_tmpBBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyyvsp[3]).v));_tmp650->f3=_tmpBBB;}),_tmp650->f4=0;_tmp650;});Cyc_Absyn_new_exp(_tmpBBE,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 517U: _LL405: _LL406:
# 3116
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,4);
# 3116 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpBBF=Cyc_yyget_Stmt_tok(&(yyyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpBBF,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 518U: _LL407: _LL408:
# 3119
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyyvsp[0]).v)));
goto _LL0;case 519U: _LL409: _LL40A:
# 3123
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3126 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp651=_cycalloc(sizeof(*_tmp651));({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyyvsp[0]).v);_tmp651->hd=_tmpBC0;}),_tmp651->tl=0;_tmp651;}));
goto _LL0;case 520U: _LL40B: _LL40C:
# 3129
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,2);
# 3128 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp652=_cycalloc(sizeof(*_tmp652));({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyyvsp[2]).v);_tmp652->hd=_tmpBC2;}),({struct Cyc_List_List*_tmpBC1=Cyc_yyget_YY4(&(yyyyvsp[0]).v);_tmp652->tl=_tmpBC1;});_tmp652;}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3131
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3134 "parse.y"
yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpBC3=Cyc_yyget_Int_tok(&(yyyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpBC3,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3137
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3135 "parse.y"
yyval=Cyc_Exp_tok(({char _tmpBC4=Cyc_yyget_Char_tok(&(yyyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpBC4,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3138
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3136 "parse.y"
yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpBC5=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpBC5,(unsigned)((yyyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3139
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);{
# 3138 "parse.y"
struct _fat_ptr _tmp653=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);
int l=(int)Cyc_strlen((struct _fat_ptr)_tmp653);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(_tmp653,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3146
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp653,i,(unsigned)((yyyyvsp[0]).l).first_line));
# 3148
goto _LL0;};case 525U: _LL415: _LL416:
# 3150
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3149 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyyvsp[0]).l).first_line));
goto _LL0;case 526U: _LL417: _LL418:
# 3152
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp655=_cycalloc(sizeof(*_tmp655));({union Cyc_Absyn_Nmspace _tmpBC8=Cyc_Absyn_Rel_n(0);_tmp655->f1=_tmpBC8;}),({struct _fat_ptr*_tmpBC7=({struct _fat_ptr*_tmp654=_cycalloc(sizeof(*_tmp654));({struct _fat_ptr _tmpBC6=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp654=_tmpBC6;});_tmp654;});_tmp655->f2=_tmpBC7;});_tmp655;}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3156
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3154 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 528U: _LL41B: _LL41C:
# 3157
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3157 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp657=_cycalloc(sizeof(*_tmp657));({union Cyc_Absyn_Nmspace _tmpBCB=Cyc_Absyn_Rel_n(0);_tmp657->f1=_tmpBCB;}),({struct _fat_ptr*_tmpBCA=({struct _fat_ptr*_tmp656=_cycalloc(sizeof(*_tmp656));({struct _fat_ptr _tmpBC9=Cyc_yyget_String_tok(&(yyyyvsp[0]).v);*_tmp656=_tmpBC9;});_tmp656;});_tmp657->f2=_tmpBCA;});_tmp657;}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3160
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3158 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 530U: _LL41F: _LL420:
# 3161
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3163 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 531U: _LL421: _LL422:
# 3166
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3164 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 532U: _LL423: _LL424:
# 3167
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3167 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 533U: _LL425: _LL426:
# 3170
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3168 "parse.y"
yyval=(yyyyvsp[0]).v;
goto _LL0;case 534U: _LL427: _LL428:
# 3171
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3173
goto _LL0;case 535U: _LL429: _LL42A:
# 3175
 yyyyvsp=Cyc_yyget_yyyyvsp(yyvs,yyvsp_offset,0);
# 3173 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL42B: _LL42C:
# 3178
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
({int _tmpBCC=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpBCC;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpBCD=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,151U,sizeof(short),yyn - 150));_tmpBCD + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7749)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,151U,sizeof(short),yyn - 150));}
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
if(yyn > - 32768 && yyn < 7749){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 301U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),x + yyn))== x)
({unsigned long _tmpBCE=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,301U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpBCE;}),count ++;}
msg=({unsigned _tmp659=(unsigned)(sze + 15)+ 1U;char*_tmp658=({struct _RegionHandle*_tmpBCF=yyregion;_region_malloc(_tmpBCF,_check_times(_tmp659,sizeof(char)));});({{unsigned _tmp711=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp711;++ i){_tmp658[i]='\000';}_tmp658[_tmp711]=0;}0;});_tag_fat(_tmp658,sizeof(char),_tmp659);});
({struct _fat_ptr _tmpBD0=msg;Cyc_strcpy(_tmpBD0,({const char*_tmp65A="parse error";_tag_fat(_tmp65A,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 301U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpBD1=msg;Cyc_strcat(_tmpBD1,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp65B=", expecting `";_tag_fat(_tmp65B,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp65C=" or `";_tag_fat(_tmp65C,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,301U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpBD2=msg;Cyc_strcat(_tmpBD2,({const char*_tmp65D="'";_tag_fat(_tmp65D,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpBD4=({const char*_tmp65E="parse error";_tag_fat(_tmp65E,sizeof(char),12U);});int _tmpBD3=yystate;Cyc_yyerror(_tmpBD4,_tmpBD3,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp65F=1;_npop_handler(0U);return _tmp65F;}
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
 if(yyssp_offset == 0){int _tmp660=1;_npop_handler(0U);return _tmp660;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1092U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7749)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),yyn))!= 1)goto yyerrdefault;
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
if(yyn == 1091){
int _tmp661=0;_npop_handler(0U);return _tmp661;}
# 546
({struct Cyc_Yystacktype _tmpBD5=({struct Cyc_Yystacktype _tmp712;_tmp712.v=yylval,_tmp712.l=yylloc;_tmp712;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpBD5;});
# 551
goto yynewstate;}
# 156 "cycbison.simple"
;_pop_region(yyregion);}
# 3176 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp665=v;struct Cyc_Absyn_Stmt*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct _tuple0*_tmp67D;struct _fat_ptr _tmp67C;char _tmp67B;union Cyc_Absyn_Cnst _tmp67A;switch((_tmp665.Stmt_tok).tag){case 1U: _LL1: _tmp67A=(_tmp665.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp668=({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0U,({struct _fat_ptr _tmpBD6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(_tmp67A));_tmp715.f1=_tmpBD6;});_tmp715;});void*_tmp666[1U];_tmp666[0]=& _tmp668;({struct Cyc___cycFILE*_tmpBD8=Cyc_stderr;struct _fat_ptr _tmpBD7=({const char*_tmp667="%s";_tag_fat(_tmp667,sizeof(char),3U);});Cyc_fprintf(_tmpBD8,_tmpBD7,_tag_fat(_tmp666,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp67B=(_tmp665.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp66B=({struct Cyc_Int_pa_PrintArg_struct _tmp716;_tmp716.tag=1U,_tmp716.f1=(unsigned long)((int)_tmp67B);_tmp716;});void*_tmp669[1U];_tmp669[0]=& _tmp66B;({struct Cyc___cycFILE*_tmpBDA=Cyc_stderr;struct _fat_ptr _tmpBD9=({const char*_tmp66A="%c";_tag_fat(_tmp66A,sizeof(char),3U);});Cyc_fprintf(_tmpBDA,_tmpBD9,_tag_fat(_tmp669,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp67C=(_tmp665.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp66E=({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U,_tmp717.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp67C);_tmp717;});void*_tmp66C[1U];_tmp66C[0]=& _tmp66E;({struct Cyc___cycFILE*_tmpBDC=Cyc_stderr;struct _fat_ptr _tmpBDB=({const char*_tmp66D="\"%s\"";_tag_fat(_tmp66D,sizeof(char),5U);});Cyc_fprintf(_tmpBDC,_tmpBDB,_tag_fat(_tmp66C,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp67D=(_tmp665.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp671=({struct Cyc_String_pa_PrintArg_struct _tmp718;_tmp718.tag=0U,({struct _fat_ptr _tmpBDD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp67D));_tmp718.f1=_tmpBDD;});_tmp718;});void*_tmp66F[1U];_tmp66F[0]=& _tmp671;({struct Cyc___cycFILE*_tmpBDF=Cyc_stderr;struct _fat_ptr _tmpBDE=({const char*_tmp670="%s";_tag_fat(_tmp670,sizeof(char),3U);});Cyc_fprintf(_tmpBDF,_tmpBDE,_tag_fat(_tmp66F,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp67E=(_tmp665.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp674=({struct Cyc_String_pa_PrintArg_struct _tmp719;_tmp719.tag=0U,({struct _fat_ptr _tmpBE0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(_tmp67E));_tmp719.f1=_tmpBE0;});_tmp719;});void*_tmp672[1U];_tmp672[0]=& _tmp674;({struct Cyc___cycFILE*_tmpBE2=Cyc_stderr;struct _fat_ptr _tmpBE1=({const char*_tmp673="%s";_tag_fat(_tmp673,sizeof(char),3U);});Cyc_fprintf(_tmpBE2,_tmpBE1,_tag_fat(_tmp672,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp67F=(_tmp665.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp677=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,({struct _fat_ptr _tmpBE3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(_tmp67F));_tmp71A.f1=_tmpBE3;});_tmp71A;});void*_tmp675[1U];_tmp675[0]=& _tmp677;({struct Cyc___cycFILE*_tmpBE5=Cyc_stderr;struct _fat_ptr _tmpBE4=({const char*_tmp676="%s";_tag_fat(_tmp676,sizeof(char),3U);});Cyc_fprintf(_tmpBE5,_tmpBE4,_tag_fat(_tmp675,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp678=0U;({struct Cyc___cycFILE*_tmpBE7=Cyc_stderr;struct _fat_ptr _tmpBE6=({const char*_tmp679="?";_tag_fat(_tmp679,sizeof(char),2U);});Cyc_fprintf(_tmpBE7,_tmpBE6,_tag_fat(_tmp678,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3188
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp680="end-of-file";_tag_fat(_tmp680,sizeof(char),12U);});
if(token == 365)
return Cyc_Lex_token_string;else{
if(token == 374)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0 && token <= 377?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,378U,sizeof(short),token)): 301;
if((unsigned)z < 301U)
return Cyc_yytname[z];else{
return _tag_fat(0,0,0);}};}
# 3202
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp681=_new_region("yyr");struct _RegionHandle*yyr=& _tmp681;_push_region(yyr);
({struct _RegionHandle*_tmpBE8=yyr;Cyc_yyparse(_tmpBE8,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp682=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp682;};
# 3205
;_pop_region(yyr);};}
